#!/bin/bash
###############################################################################
# Search and Rescue
# Installer Bootstrapper Utility
#    This tool runs on a Raspberry Pi preconfigured by PiBakery or other 3rd
#    party automation software. The intent is to provide an environment
#    bootstrapping script that retrieves the source code, overlays pre-packed
#    files and configurations, and installs any prerequisite packages.
#
#    The format of the overlays supported is as follows:
#
#    <OVERLAY_DIRECTORY>/conf/[ybts.conf/sar.conf/UserConfig.txt]
#        Pre-packed configuration files for YBTS/SAR/OCP that will be used
#
#    <OVERLAY_DIRECTORY>/packages/*.deb
#        Debian packages to install prior to running the installer script
#
#    <OVERLAY_DIRECTORY>/skel/*
#        Filesystem overlay; contents of this folder will be placed in /
#        relative to their paths in skel/. For example, to put a file in
#        /root/dir/file.txt, it would be placed in the location
#        <OVERLAY_DIRECTORY/skel/root/dir/file.txt
#
#    <OVERLAY_DIRECTORY>/preinstall.sh
#        Script that is run before the installer is started
#
#    <OVERLAY_DIRECTORY>/postinstall.sh
#        Script that is run after the installer has completed
#
#    Processing order:
#        1. Install debian packages from <OVERLAY_DIRECTORY>/packages
#        2. Overlay filesystem from <OVERLAY_DIRECTORY>/skel/*
#        3. Reconcile any remaining packages from Aptitude (requires internet)
#        4. Run preinstall.sh
#        5. Clone git repository if not already in working directory
#        5. Run installer
#        6. Run postinstall.sh
###############################################################################

### DEFAULTS ###
LOG_FILE=/boot/bootstrapper.log
SAR_GIT_URL=https://github.com/CatalystCode/cell-sar.git
OVERLAYDIR=/boot/sar_overlay
BASEDIR=/root/sar
### /DEFAULTS ###

RUNNING_FORKED=0

### SAR INSTALLER COMMON CODE ###
RESTORE='\033[0m'
RED='\033[00;31m'
GREEN='\033[00;32m'
YELLOW='\033[00;33m'
CYAN='\033[00;36m'
OUTPUT_DEPTH=0

function writeOut() { echo -e "$2$(printf '%*s' "${OUTPUT_DEPTH}" | tr ' ' "\t")$1${RESTORE}" ; }
function headerOut() { writeOut "# $1" "${CYAN}" ; }
function errorOut() { writeOut "- $1" "${RED}" ; }
function normalOut() { writeOut "+ $1" "${GREEN}" ; }
function warningOut() { writeOut "! $1" "${YELLOW}" ; }

function execAndCheck()
{
	"$@" >> "${LOG_FILE}" 2>&1 # evaluates program LOG_FILE including STDERR
	local errCode=$?
	if [ $errCode -ne 0 ]; then
		echo "-- '$*' exited with code $errCode" >> "${LOG_FILE}"
	fi
	echo $errCode
}
### /SAR INSTALLER COMMON CODE ###

### COMMAND LINE ARGUMENT PARSING ###
ADDL_ARGS=""
args=$(getopt -o glow:h:: --long git-url:,log:,overlay-dir:,working-dir:,help,nofork -- "$@")
eval set -- "$args"
while [ $# -ge 1 ]; do
    case "$1" in
		-g|--git-url) SAR_GIT_URL="$2" ; shift 2 ;;
		-l|--log) LOG_FILE=$(readlink -f "$2") ; shift 2 ;;
        -o|--overlay-dir) OVERLAYDIR=$(readlink -f "$2") ; shift 2 ;;
        -w|--working-dir) BASEDIR=$(readlink -f "$2") ; shift 2 ;;
		--nofork) RUNNING_FORKED=1 ; shift ;;
        -h|--help)
        echo " "
        echo "Search and Rescue / Installation Bootstrapper"
		echo "  --git-url            URL to clone the git repository from"
        echo "  --log                Log file to use for bootstrapper and installer"
		echo "  --nofork             Do not use openvt to fork the process to its own TTY"
        echo "  --overlay-dir        Path to filesystem overlay that will be placed prior to installation"
        echo "  --working-dir        Location where the SAR build will take place"
        echo " "
        echo "syn: bootstrapper.sh [-g|--git-url=${SAR_GIT_URL}] [-l|--log=${LOG_FILE}] [-o|--overlay-dir=${OVERLAYDIR}] [-w|--working-dir=${BASEDIR}] [--nofork]"
        echo " "
        exit 0
        ;;
		--) shift ; break ;;
        *) ADDL_ARGS="${ADDL_ARGS} $1=$2" ; shift 2 ;;
    esac
done
### /COMMAND LINE ARGUMENT PARSING ###

if [[ $EUID -ne 0 ]]; then
	errorOut "The Search and Rescue Bootstrapper script must be run as root."
	exit 1
fi

# ---=[ FORK CHECK ]=--- #
# Note: this is to get around PiBakery's use of whiptail on the console, so we can see our progress
if [ $RUNNING_FORKED -eq 0 ]; then
	openvt -s -w -- $(readlink -f "$0") --nofork
	exit 0
fi

# ---=[ LOCAL PACKAGE INSTALL ]=--- #
headerOut "Checking for local packages to install..."
((OUTPUT_DEPTH++))
if [[ -d "${OVERLAYDIR}/packages" ]]; then
	if [ "$(execAndCheck dpkg -R -a --install "${OVERLAYDIR}/packages")" -ne 0 ]; then
		errorOut "Local packages found but dpkg failed to install them."
		exit 1
	else
		normalOut "Local packages installed successfully."
	fi
else
	normalOut "No local packages found."
fi
((OUTPUT_DEPTH--))

# ---=[ PREREQUISITES ]=--- #
headerOut "Retrieving and installing required packages from Aptitude."
if [ "$(execAndCheck apt-get -y install git dos2unix)" -ne 0 ]; then
	((OUTPUT_DEPTH++))
		errorOut "Aptitude exited with an error."
	((OUTPUT_DEPTH--))
	exit 1
fi

# ---=[ ENVIRONMENT CONFIG ]=--- #
headerOut "Configuring environment for installer."
((OUTPUT_DEPTH++))

# This is bad practice in most cases but for a headless install, is necessary.
execAndCheck mkdir -p ~/.ssh
echo -e "Host github.com\n\tStrictHostKeyChecking no\n" >> ~/.ssh/config
normalOut "Disable host key checking for github.com."

execAndCheck dos2unix -R "${OVERLAYDIR}/"*
normalOut "Normalized line endings to UNIX-compatible for overlay."

headerOut "Checking for skeleton files..."
((OUTPUT_DEPTH++))
if [[ -d "${OVERLAYDIR}/skel" ]]; then
	normalOut "Found skeleton files, overlaying."
	execAndCheck cp -R "${OVERLAYDIR}/skel/"* /
else
	normalOut "No skeleton files found."
fi
((OUTPUT_DEPTH--))

headerOut "Checking for pre-install actions..."
((OUTPUT_DEPTH++))
if [[ -f "${OVERLAYDIR}/preinstall.sh" ]]; then
	normalOut "Running pre-install script."
	execAndCheck chmod a+x "${OVERLAYDIR}/preinstall.sh"
	execAndCheck "${OVERLAYDIR}/preinstall.sh"
else
	normalOut "No pre-install script found."
fi
((OUTPUT_DEPTH--))

# ---=[ ACQUIRE PACKAGES ]=--- #
headerOut "Downloading packages..."
((OUTPUT_DEPTH++))

if [[ ! -d "${BASEDIR}" ]]; then
	headerOut "Cloning cell-sar from ${SAR_GIT_URL}..."
	if [ "$(execAndCheck git clone --depth=1 --recursive "${SAR_GIT_URL}" "${BASEDIR}")" -ne 0 ]; then
		((OUTPUT_DEPTH++))
			errorOut "Repository clone failed."
		((OUTPUT_DEPTH--))
	fi
else
	warningOut "SAR git repository exists, skipping."
fi
((OUTPUT_DEPTH--))

# ---=[ XFER CONTROL ]=---
headerOut "Transferring control to installer..."

execAndCheck chmod a+x "${BASEDIR}/devops/installer.sh"
"${BASEDIR}/devops/installer.sh" --log "${LOG_FILE}" --working-dir "${BASEDIR}" ${ADDL_ARGS}