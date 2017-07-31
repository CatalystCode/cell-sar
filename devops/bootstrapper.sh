#!/bin/bash
BASEDIR=/root/sar

SAR_GIT_URL=https://github.com/CatalystCode/cell-sar.git

if [[ $EUID -ne 0 ]]; then
   echo "The Search and Rescue Installer script must be run as root."
   exit 1
fi

# ---=[ PREREQUISITES ]=--- #
echo "Retrieving and installing required packages from Aptitude."
apt-get -y install git

# ---=[ ACQUIRE PACKAGES ]=--- #
echo "Downloading packages..."

if [[ ! -d "${BASEDIR}" ]]; then
	echo "	Cloning cell-sar from ${SAR_GIT_URL}..."
	git clone "${SAR_GIT_URL}" "${BASEDIR}"
else
	echo "	SAR git repository exists, skipping."
fi

# ---=[ XFER CONTROL ]=---
echo "Transferring control to installer..."

${BASEDIR}/devops/installer.sh > /boot/install.log