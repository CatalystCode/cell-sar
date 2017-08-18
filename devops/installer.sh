#!/bin/bash
###############################################################################
# Search and Rescue
# Installer Utility
#    This tool builds and configures the Search and Rescue components for a
#    Debian base system. This script was originally designed for a Raspberry Pi
#    3B running Raspbian Lite (July 2017 release).
#
#    All components are installed to /usr/share by default.
#
#    Configuration files are placed in /boot (FAT32 segment on Raspbian) for
#    easy access and editing from any machine.
###############################################################################

### DEFAULTS ###
CONFIGURATION_DIR=/boot
LOG_FILE=${CONFIGURATION_DIR}/sar_installer.log
PRECONFIGURED_DIR=${CONFIGURATION_DIR}/sar_overlay
BASEDIR=/root/sar
### /DEFAULTS ###

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
args=$(getopt -l "searchpath:" -o "s:h" -- "$@")
eval set -- "$args"
while [ $# -ge 1 ]; do
    case "$1" in
        --) # No more options left.
        shift
        break
        ;;
        -c|--configuration-dir)
        CONFIGURATION_DIR=$(readlink -f "$2")
        shift
        ;;
        -l|--log)
        LOG_FILE=$(readlink -f "$2")
        shift
        ;;
        -p|--preconfigured-dir)
		PRECONFIGURED_DIR=$(readlink -f "$2")
        shift
        ;;
        -w|--working-dir)
        BASEDIR=$(readlink -f "$2")
        shift
        ;;
        -h|--help)
        echo " "
        echo "Search and Rescue / Installer Utility"
        echo "  --configuration-dir  Where SAR configuration files will be stored"
		echo "  --log                Location for installer log to be written"
		echo "  --preconfigured-dir  Path to existing sar.conf, ybts.conf, UserConfig.txt to use for install"
        echo "  --working-dir        Location where the SAR build will take place"
        echo " "
        echo "syn: installer.sh <-c|--configuration-dir=${CONFIGURATION_DIR}> [-l|--log=${LOG_FILE}] [-p|--preconfigured-dir=${PRECONFIGURED_DIR}] [-w|--working-dir=${BASEDIR}]"
        echo " "
        exit 0
        ;;
    esac
    shift
done
### /COMMAND LINE ARGUMENT PARSING ###

if [[ $EUID -ne 0 ]]; then
	errorOut "The Search and Rescue Installer script must be run as root."
	exit 1
fi

SAR_VERSION=$(git --git-dir="${BASEDIR}/.git" --work-tree="${BASEDIR}" rev-parse --short HEAD)

PURPLE='\033[00;35m'
CYAN='\033[00;36m'
LIGHTGRAY='\033[00;37m'
echo " "
echo -e "${GREEN}███████╗███████╗ █████╗ ██████╗  ██████╗██╗  ██╗     █████╗ ███╗   ██╗██████╗ "
echo -e "${GREEN}██╔════╝██╔════╝██╔══██╗██╔══██╗██╔════╝██║  ██║    ██╔══██╗████╗  ██║██╔══██╗"
echo -e "${GREEN}███████╗█████╗  ███████║██████╔╝██║     ███████║    ███████║██╔██╗ ██║██║  ██║"
echo -e "${GREEN}╚════██║██╔══╝  ██╔══██║██╔══██╗██║     ██╔══██║    ██╔══██║██║╚██╗██║██║  ██║"
echo -e "${GREEN}███████║███████╗██║  ██║██║  ██║╚██████╗██║  ██║    ██║  ██║██║ ╚████║██████╔╝"
echo -e "${GREEN}╚══════╝╚══════╝╚═╝  ╚═╝╚═╝  ╚═╝ ╚═════╝╚═╝  ╚═╝    ╚═╝  ╚═╝╚═╝  ╚═══╝╚═════╝ "
echo -e "${GREEN}██████╗ ███████╗███████╗ ██████╗██╗   ██╗███████╗  ${CYAN}╔════════════════════════╗"
echo -e "${GREEN}██╔══██╗██╔════╝██╔════╝██╔════╝██║   ██║██╔════╝  ${CYAN}║   ${LIGHTGRAY}Relative  Location   ${CYAN}║"
echo -e "${GREEN}██████╔╝█████╗  ███████╗██║     ██║   ██║█████╗    ${CYAN}║    ${LIGHTGRAY}Sensing with GSM    ${CYAN}║"
echo -e "${GREEN}██╔══██╗██╔══╝  ╚════██║██║     ██║   ██║██╔══╝    ${CYAN}║      ${PURPLE}git  ${SAR_VERSION}      ${CYAN}║"
echo -e "${GREEN}██║  ██║███████╗███████║╚██████╗╚██████╔╝███████╗  ${CYAN}╚════════════════════════╝"
echo -e "${GREEN}╚═╝  ╚═╝╚══════╝╚══════╝ ╚═════╝ ╚═════╝ ╚══════╝"
echo -e "${RESTORE} "

headerOut "Creating logging environment."
execAndCheck mkdir -p "${LOG_FILE}"
execAndCheck mkdir -p "${CONFIGURATION_DIR}"
echo "${SAR_VERSION}" >> "${CONFIGURATION_DIR}/sar_${SAR_VERSION}.version"

# ---=[ PREREQUISITES ]=--- #
headerOut "Retrieving and installing required packages from Aptitude."
if [ "$(execAndCheck apt-get -y install cmake build-essential autoconf libusb-1.0-0-dev)" -ne 0 ]; then
	((OUTPUT_DEPTH++))
		errorOut "Aptitude exited with an error."
	((OUTPUT_DEPTH--))
	exit 1
fi

# ---=[ BUILDS ]=--- #
headerOut "Starting builds..."
((OUTPUT_DEPTH++))

# ---=[ Yate Build ]=--- #
headerOut "Building Yate core..."
((OUTPUT_DEPTH++))
cd "${BASEDIR}/bts/yate" || return
if [[ -f "${BASEDIR}/bts/yate/configure" ]]; then
	warningOut "Yate configure script exists. Attempting to make build environment sane."
	if [ "$(execAndCheck make clean)" -ne 0 ]; then
		((OUTPUT_DEPTH++))
			errorOut "Cannot establish build environment sanity."
		((OUTPUT_DEPTH--))
		exit 1
	fi
fi

chmod a+x "${BASEDIR}/bts/yate/"*.sh

if [ "$(execAndCheck ./autogen.sh)" -ne 0 ]; then
	errorOut "Configuration generation step failed."
	exit 1
fi

if [ "$(execAndCheck ./configure)" -ne 0 ]; then
	errorOut "Configuration failed."
	exit 1
fi

normalOut "Build environment ready, starting build."
if [ "$(execAndCheck make)" -ne 0 ]; then
	((OUTPUT_DEPTH++))
		errorOut "Build failed."
	((OUTPUT_DEPTH--))
	exit 1
fi

normalOut "Build complete, installing."
if [ "$(execAndCheck make install-noapi)" -ne 0 ]; then
	((OUTPUT_DEPTH++))
		errorOut "Install failed."
	((OUTPUT_DEPTH--))
	exit 1
fi

normalOut "Done building Yate. Updating ldconfig cache."
if [ "$(execAndCheck ldconfig)" -ne 0 ]; then
	((OUTPUT_DEPTH++))
		errorOut "ldconfig cache update failed."
	((OUTPUT_DEPTH--))
	exit 1
fi

((OUTPUT_DEPTH--))

# ---=[ YateBTS Build ]=--- #
headerOut "Building YateBTS..."
((OUTPUT_DEPTH++))
cd "${BASEDIR}/bts/yatebts" || return
if [ -f "${BASEDIR}/bts/yatebts/config.log" ]; then
	warningOut "YateBTS configure script exists. Attempting to make build environment sane."
	if [ "$(execAndCheck make clean)" -ne 0 ]; then
		((OUTPUT_DEPTH++))
			errorOut "Cannot establish build environment sanity."
		((OUTPUT_DEPTH--))
		exit 1
	fi
fi

chmod a+x "${BASEDIR}/bts/yatebts/"*.sh

if [ "$(execAndCheck ./autogen.sh)" -ne 0 ]; then
	errorOut "Configuration generation step failed."
	exit 1
fi

if [ "$(execAndCheck ./configure)" -ne 0 ]; then
	errorOut "Configuration failed."
	exit 1
fi

normalOut "Build environment ready, starting build."
if [ "$(execAndCheck make install)" -ne 0 ]; then
	((OUTPUT_DEPTH++))
		errorOut "Build failed."
	((OUTPUT_DEPTH--))
	exit 1
fi

normalOut "Done building YateBTS. Updating ldconfig cache."
if [ "$(execAndCheck ldconfig)" -ne 0 ]; then
	((OUTPUT_DEPTH++))
		errorOut "ldconfig cache update failed."
	((OUTPUT_DEPTH--))
	exit 1
fi

((OUTPUT_DEPTH--))

# ---=[ OCP Build ]=--- #
headerOut "Building OCP..."
((OUTPUT_DEPTH++))
if [ -d "/home/yate/ocp" ]; then
	warningOut "OCP build already exists. Attempting to make build environment sane."
	execAndCheck rm -rf "/home/yate/ocp"
fi

execAndCheck mkdir -p /home/yate/ocp
cd "/home/yate/ocp" || return

if [ "$(execAndCheck cmake "${BASEDIR}/ocp")" -ne 0 ]; then
	((OUTPUT_DEPTH++))
		errorOut "cmake failed."
	((OUTPUT_DEPTH--))
	exit 1
fi

normalOut "Build environment ready, starting build."
if [ "$(execAndCheck make)" -ne 0 ]; then
	((OUTPUT_DEPTH++))
		errorOut "Build failed."
	((OUTPUT_DEPTH--))
	exit 1
fi

((OUTPUT_DEPTH--))
headerOut "Builds complete."

# ---=[ USERS/GROUPS ]=--- #
headerOut "Creating users and groups..."
execAndCheck useradd yate
execAndCheck usermod -a -G yate yate
execAndCheck usermod -a -G dialout yate
execAndCheck chown yate:yate /usr/local/etc/yate/*.conf
execAndCheck chmod g+w /usr/local/etc/yate/*.conf

# ---=[ REORG CONFIG FILES ]=--- #
headerOut "Moving/linking configuration files on FAT32 partition..."
((OUTPUT_DEPTH++))

if [[ -e "${PRECONFIGURED_DIR}/ybts.conf" ]]; then
	execAndCheck cp "${PRECONFIGURED_DIR}/ybts.conf" "${CONFIGURATION_DIR}/ybts.conf"
	execAndCheck rm /usr/local/etc/yate/ybts.conf
	execAndCheck ln -s "${CONFIGURATION_DIR}/ybts.conf" /usr/local/etc/yate/ybts.conf
	normalOut "ybts.conf (from overlay)"
else
	execAndCheck mv /usr/local/etc/yate/ybts.conf "${CONFIGURATION_DIR}/ybts.conf"
	execAndCheck ln -s "${CONFIGURATION_DIR}/ybts.conf" /usr/local/etc/yate/ybts.conf
	normalOut "ybts.conf"
fi

if [[ -e "${PRECONFIGURED_DIR}/sar.conf" ]]; then
	execAndCheck cp "${PRECONFIGURED_DIR}/sar.conf" "${CONFIGURATION_DIR}/sar.conf"
	execAndCheck rm /usr/local/etc/yate/sar.conf
	execAndCheck ln -s "${CONFIGURATION_DIR}/sar.conf" /usr/local/etc/yate/sar.conf
	normalOut "sar.conf (from overlay)"
else
	execAndCheck mv /usr/local/etc/yate/sar.conf "${CONFIGURATION_DIR}/sar.conf"
	execAndCheck ln -s "${CONFIGURATION_DIR}/sar.conf" /usr/local/etc/yate/sar.conf
	normalOut "sar.conf"
fi

if [[ -e "${PRECONFIGURED_DIR}/UserConfig.txt" ]]; then
	execAndCheck cp "${PRECONFIGURED_DIR}/UserConfig.txt" "${CONFIGURATION_DIR}/UserConfig.txt"
	execAndCheck rm /home/yate/ocp/bin/UserConfig.txt
	execAndCheck ln -s "${CONFIGURATION_DIR}/UserConfig.txt" /home/yate/ocp/bin/UserConfig.txt
	normalOut "UserConfig.txt (from overlay)"
else
	if [[ ! -e "/home/yate/ocp/bin/UserConfig.txt" ]]; then
		execAndCheck touch /home/yate/ocp/bin/UserConfig.txt
	fi
	execAndCheck mv /home/yate/ocp/bin/UserConfig.txt "${CONFIGURATION_DIR}/UserConfig.txt"
	execAndCheck ln -s "${CONFIGURATION_DIR}/UserConfig.txt" /home/yate/ocp/bin/UserConfig.txt
	normalOut "UserConfig.txt"
fi
((OUTPUT_DEPTH--))

# ---=[ POST-INSTALL TASKS ]=--- #
headerOut "Running system post-installation tasks..."
((OUTPUT_DEPTH++))

execAndCheck chown -R yate:yate /home/yate
normalOut "Ensured yate user ownership of home directory."

echo "# nuand bladeRF" > /etc/udev/rules.d/90-yate.rules
echo "ATTR{idVendor}==\"1d50\", ATTR{idProduct}==\"6066\", MODE=\"660\", GROUP=\"yate\"" >> /etc/udev/rules.d/90-yate.rules
execAndCheck udevadm control --reload-rules
normalOut "Added udev rule for yate user."

echo "@yate hard nice -20" >> /etc/security/limits.conf
echo "@yate hard rtprio 99" >> /etc/security/limits.conf
normalOut "Added kernel scheduling rules."

execAndCheck sed -i -- 's/\;mode=nib/mode=searchandrescue/g' /boot/ybts.conf
normalOut "Set YBTS into searchandrescue mode."

execAndCheck sed -i -- 's/\;radio_read_priority=highest/radio_read_priority=highest/g' /boot/ybts.conf
execAndCheck sed -i -- 's/\;radio_send_priority=high/radio_send_priority=high/g' /boot/ybts.conf
normalOut "Changed radio transmit/receive priorities."

execAndCheck cp "${BASEDIR}/devops/raspberrypi/YateBTS" /etc/init.d/YateBTS
execAndCheck chmod a+x /etc/init.d/YateBTS
execAndCheck update-rc.d YateBTS defaults
normalOut "Created YateBTS init script."

execAndCheck cp "${BASEDIR}/devops/ocp.service" /etc/systemd/system/ocp.service
normalOut "Created ocp.service systemd script."

execAndCheck touch /var/log/YateBTS.log
execAndCheck chown yate:yate /var/log/YateBTS.log
normalOut "Created YBTS log file."
((OUTPUT_DEPTH--))

# ---=[ PI ENVIRONMENT ]=--- #
headerOut "Overlaying custom configuration for Pi environment..."
# cp ${BASEDIR}/devops/raspberrypi/cmdline.txt /boot/cmdline.txt
cp "${BASEDIR}/devops/raspberrypi/config.txt" /boot/config.txt

# ---=[ POST-INSTALL SCRIPT ]=--- #
headerOut "Checking for post-install actions..."
((OUTPUT_DEPTH++))
if [[ -f "${PRECONFIGURED_DIR}/postinstall.sh" ]]; then
	normalOut "Running post-install script."
	chmod a+x "${PRECONFIGURED_DIR}/postinstall.sh"
	execAndCheck "${PRECONFIGURED_DIR}/postinstall.sh"
else
	normalOut "No post-install script found."
fi
((OUTPUT_DEPTH--))

echo " "
normalOut "Done! +" # + Done! +
echo " "
