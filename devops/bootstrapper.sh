#!/bin/bash

# Working folder for the installer
BASEDIR=/root/sar

# Overlay path
OVERLAYDIR=/boot/sar_overlay

# URL to clone the Search and Rescue code from
SAR_GIT_URL=https://github.com/CatalystCode/cell-sar.git

# Log file to hold bootstrapper log information
LOGFILE=/boot/bootstrapper.log

# Note:
# This installer supports the use of the FAT32 partition for overlays/automation:
# + /boot/sar_overlay/skel/*			Will be overlaid on the root filesystem.
#										For example, to put a file in /root/dir/file.txt,
#										You would put it in /boot/skel/root/dir/file.txt.
#
# + /boot/sar_overlay/preinstall.sh		Run before the installer. Best used for things
#										like turning on SSH, checking permissions, etc.
#
# + /boot/sar_overlay/postinstall.sh	Run after the install is complete. Best used for
#										things like setting an IP address, etc
#
# + /boot/sar_overlay/ybts.conf			Pre-configured ybts.conf to be used for sensor
#
# + /boot/sar_overlay/sar.conf			Pre-configured sar.conf to be used for sensor
#########################################################################################

# Colors and Styling
RESTORE='\033[0m'
RED='\033[00;31m'
GREEN='\033[00;32m'
YELLOW='\033[00;33m'
BLUE='\033[00;34m'
PURPLE='\033[00;35m'
CYAN='\033[00;36m'
LIGHTGRAY='\033[00;37m'

if [[ $EUID -ne 0 ]]; then
   echo -e "${RED}The Search and Rescue Installer script must be run as root.${RESTORE}"
   exit 1
fi

# ---=[ FORK CHECK ]=--- #
# Note: this is to get around PiBakery's use of whiptail on the console, so we can see our progress
if [ "$1" != "--nofork" ]; then
	openvt -s -w -- $(readlink -f $0) --nofork
	exit 0
fi

# ---=[ PREREQUISITES ]=--- #
echo -e "${CYAN}+ Retrieving and installing required packages from Aptitude.${RESTORE}"
apt-get -y install git dos2unix >> "${LOGFILE}" 2>&1
if [[ $? != 0 ]]; then
    echo -e "${RED}	! Aptitude exited with error code $?.${RESTORE}"
	exit 1
fi

# ---=[ ENVIRONMENT CONFIG ]=--- #
echo -e "${CYAN}+ Configuring environment for installer.${RESTORE}"
# This is bad practice in most cases but for a headless install, is necessary.
mkdir -p ~/.ssh
echo -e "Host github.com\n\tStrictHostKeyChecking no\n" >> ~/.ssh/config
echo -e "${GREEN}	+ Disable host key checking for github.com${RESTORE}"
dos2unix -R ${OVERLAYDIR}/* >> "${LOGFILE}" 2>&1
echo -e "${GREEN}	+ Normalize line endings to UNIX-compatible for overlay${RESTORE}"
echo -e "${CYAN}	+ Checking for skeleton files...${RESTORE}"
if [[ -d "${OVERLAYDIR}/skel" ]]; then
	echo -e "${GREEN}		+ Found skeleton files, overlaying.${RESTORE}"
	cp -R ${OVERLAYDIR}/skel/* /
else
	echo -e "${GREEN}		+ No skeleton files found.${RESTORE}"
fi
echo -e "${CYAN}	+ Checking for pre-install actions...${RESTORE}"
if [[ -f "${OVERLAYDIR}/preinstall.sh" ]]; then
	echo -e "${GREEN}		+ Running pre-install script.${RESTORE}"
	chmod a+x ${OVERLAYDIR}/preinstall.sh
	${OVERLAYDIR}/preinstall.sh > "${LOGDIR}/bootstrapper_preinstall.log"
else
	echo -e "${GREEN}		+ No pre-install script found.${RESTORE}"
fi


# ---=[ ACQUIRE PACKAGES ]=--- #
echo -e "${CYAN}+ Downloading packages...${RESTORE}"

if [[ ! -d "${BASEDIR}" ]]; then
	echo -e "${CYAN}	+ Cloning cell-sar from ${SAR_GIT_URL}...${RESTORE}"
	git clone --depth=1 --recursive "${SAR_GIT_URL}" "${BASEDIR}" >> "${LOGFILE}" 2>&1
	if [[ $? != 0 ]]; then
		echo -e "${RED}		! Repository clone failed."
	fi
else
	echo -e "${YELLOW}		! SAR git repository exists, skipping.${RESTORE}"
fi

# ---=[ XFER CONTROL ]=---
echo -e "${CYAN}+ Transferring control to installer...${RESTORE}"

chmod a+x ${BASEDIR}/devops/installer.sh
${BASEDIR}/devops/installer.sh