#!/bin/bash
BASEDIR=/root/sar
SANDBOXDIR=${BASEDIR}/.sandbox
LOGDIR=/boot/sar_installer
SAR_VERSION=`git --git-dir="${BASEDIR}/.git" --work-tree="${BASEDIR}" rev-parse --short HEAD`

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

echo -e "${CYAN}+ Creating logging environment.${RESTORE}"
mkdir -p ${LOGDIR}
echo "${SAR_VERSION}" >> "${LOGDIR}/sar_${SAR_VERSION}.version"

# ---=[ PREREQUISITES ]=--- #
echo -e "${CYAN}+ Retrieving and installing required packages from Aptitude.${RESTORE}"
apt-get -y install cmake build-essential autoconf libusb-1.0-0-dev > "${LOGDIR}/00_aptitude.log" 2>&1
if [[ $? != 0 ]]; then
    echo -e "${RED}	! Aptitude exited with error code $?.${RESTORE}"
	exit 1
fi

# ---=[ GIT CONFIG ]=--- #
echo -e "${CYAN}+ Configuring git...${RESTORE}"
if [[ $(git config --global user.email) ]]; then
	echo -e "${YELLOW}	! Git global credentials exist, using those instead"
else
	echo -e "${GREEN}	+ Setting default sandbox credentials${RESTORE}"
	git config --global user.email "sar@raspberrypi.local" > "${LOGDIR}/01_git_config.log" 2>&1
	git config --global user.name "SAR" >> "${LOGDIR}/01_git_config.log" 2>&1
fi

# ---=[ PREP REPOSITORY ]=--- #
echo -e "${GREEN}	+ Preparing repository (this will take a moment)${RESTORE}"
${BASEDIR}/devops/generatePatchedTree.sh > "${LOGDIR}/02_deploy_source.log" 2>&1

# ---=[ BUILDS ]=--- #
echo -e "${CYAN}+ Starting builds...${RESTORE}"

# ---=[ Yate Build ]=--- #
echo -e "${CYAN}	+ Building Yate core...${RESTORE}"
cd "${SANDBOXDIR}/yate"
if [[ -f "${SANDBOXDIR}/yate/configure" ]]; then
	echo -e "${YELLOW}		! Yate configure script exists. Attempting to make build environment sane.${RESTORE}"
	make clean > "${LOGDIR}/03_yate_build.log" 2>&1
	if [[ $? != 0 ]]; then
		echo -e "${RED}		! Cannot establish build environment sanity. (Error code $?).${RESTORE}"
		exit 1
	fi
fi
chmod a+x ${SANDBOXDIR}/yate/*.sh
./autogen.sh >> "${LOGDIR}/03_yate_build.log" 2>&1
if [[ $? != 0 ]]; then
	echo -e "${RED}		! Configuration generation step failed. (Error code $?).${RESTORE}"
	exit 1
fi

./configure >> "${LOGDIR}/03_yate_build.log" 2>&1
if [[ $? != 0 ]]; then
	echo -e "${RED}		! Configuration failed. (Error code $?).${RESTORE}"
	exit 1
fi

echo -e "${GREEN}		+ Build environment ready, starting build.${RESTORE}"
make >> "${LOGDIR}/03_yate_build.log" 2>&1
if [[ $? != 0 ]]; then
	echo -e "${RED}			! Build failed. (Error code $?).${RESTORE}"
	exit 1
fi

echo -e "${GREEN}		+ Build complete, installing.${RESTORE}"
make install-noapi >> "${LOGDIR}/03_yate_build.log" 2>&1
if [[ $? != 0 ]]; then
	echo -e "${RED}			! Install failed. (Error code $?).${RESTORE}"
	exit 1
fi

echo -e "${GREEN}		+ Done building Yate. Updating ldconfig cache."
ldconfig >> "${LOGDIR}/03_yate_build.log" 2>&1
if [[ $? != 0 ]]; then
	echo -e "${RED}			! ldconfig cache update failed. (Error code $?).${RESTORE}"
	exit 1
fi

# ---=[ YateBTS Build ]=--- #
echo -e "${CYAN}	+ Building YateBTS...${RESTORE}"
cd "${SANDBOXDIR}/yatebts"
if [ -f "${SANDBOXDIR}/yatebts/config.log" ]; then
	echo -e "${YELLOW}		! YateBTS configure script exists. Attempting to make build environment sane.${RESTORE}"
	make clean > "${LOGDIR}/04_yatebts_build.log" 2>&1
	if [[ $? != 0 ]]; then
		echo -e "${RED}		! Cannot establish build environment sanity. (Error code $?).${RESTORE}"
		exit 1
	fi
fi

chmod a+x ${SANDBOXDIR}/yatebts/*.sh
./autogen.sh >> "${LOGDIR}/04_yatebts_build.log" 2>&1
if [[ $? != 0 ]]; then
	echo -e "${RED}		! Configuration generation step failed. (Error code $?).${RESTORE}"
	exit 1
fi

./configure >> "${LOGDIR}/04_yatebts_build.log" 2>&1
if [[ $? != 0 ]]; then
	echo -e "${RED}		! Configuration failed. (Error code $?).${RESTORE}"
	exit 1
fi

echo -e "${GREEN}		+ Build environment ready, starting build.${RESTORE}"
make install >> "${LOGDIR}/04_yatebts_build.log" 2>&1
if [[ $? != 0 ]]; then
	echo -e "${RED}			! Build failed. (Error code $?).${RESTORE}"
	exit 1
fi

echo -e "${GREEN}		+ Done building YateBTS. Updating ldconfig cache.${RESTORE}"
ldconfig >> "${LOGDIR}/04_yatebts_build.log" 2>&1
if [[ $? != 0 ]]; then
	echo -e "${RED}			! ldconfig cache update failed. (Error code $?).${RESTORE}"
	exit 1
fi

echo -e "${CYAN}+ Builds complete.${RESTORE}"

# ---=[ USERS/GROUPS ]=--- #
echo -e "${CYAN}+ Creating users and groups...${RESTORE}"
useradd yate >> "${LOGDIR}/05_user_config.log" 2>&1
usermod -a -G yate yate >> "${LOGDIR}/05_user_config.log" 2>&1
chown yate:yate /usr/local/etc/yate/*.conf >> "${LOGDIR}/05_user_config.log" 2>&1
chmod g+w /usr/local/etc/yate/*.conf >> "${LOGDIR}/05_user_config.log" 2>&1

# ---=[ REORG CONFIG FILES ]=--- #
echo -e "${CYAN}+ Moving/linking configuration files on FAT32 partition...${RESTORE}"
mv /usr/local/etc/yate/ybts.conf /boot/ybts.conf >> "${LOGDIR}/07_configs.log" 2>&1
ln -s /boot/ybts.conf /usr/local/etc/yate/ybts.conf >> "${LOGDIR}/07_configs.log" 2>&1
echo -e "${GREEN}	+ ybts.conf${RESTORE}"

mv /usr/local/etc/yate/sar.conf /boot/sar.conf >> "${LOGDIR}/07_configs.log" 2>&1
ln -s /boot/sar.conf /usr/local/etc/yate/sar.conf >> "${LOGDIR}/07_configs.log" 2>&1
echo -e "${GREEN}	+ sar.conf${RESTORE}"

# ---=[ POST-INSTALL TASKS ]=--- #
echo -e "${CYAN}+ Running system post-installation tasks...${RESTORE}"

echo "# nuand bladeRF" > /etc/udev/rules.d/90-yate.rules
echo "ATTR{idVendor}==\"1d50\", ATTR{idProduct}==\"6066\", MODE=\"660\", GROUP=\"yate\"" >> /etc/udev/rules.d/90-yate.rules
udevadm control --reload-rules >> "${LOGDIR}/06_udev.log" 2>&1
echo -e "${GREEN}	+ Added udev rule for yate user${RESTORE}"

echo "@yate hard nice -20" >> /etc/security/limits.conf
echo "@yate hard rtprio 99" >> /etc/security/limits.conf
echo -e "${GREEN}	+ Added kernel scheduling rules${RESTORE}"

sed -i -- 's/\;mode=nib/mode=searchandrescue/g' /boot/ybts.conf
echo -e "${GREEN}	+ Set YBTS into searchandrescue mode${RESTORE}"

sed -i -- 's/\;radio_read_priority=highest/radio_read_priority=highest/g' /boot/ybts.conf
sed -i -- 's/\;radio_send_priority=high/radio_send_priority=high/g' /boot/ybts.conf
echo -e "${GREEN}	+ Changed radio transmit/receive priorities${RESTORE}"

cp ${BASEDIR}/devops/raspberrypi/YateBTS /etc/init.d/YateBTS >> "${LOGDIR}/08_init.log" 2>&1
chmod a+x /etc/init.d/YateBTS >> "${LOGDIR}/08_init.log" 2>&1
update-rc.d YateBTS defaults >> "${LOGDIR}/08_init.log" 2>&1
echo -e "${GREEN}	+ Created init script${RESTORE}"

# ---=[ PI ENVIRONMENT ]=--- #
echo -e "${CYAN}+ Overlaying custom configuration for Pi environment...${RESTORE}"
cp ${BASEDIR}/devops/raspberrypi/cmdline.txt /boot/cmdline.txt
cp ${BASEDIR}/devops/raspberrypi/config.txt /boot/config.txt

echo " "
echo -e "${GREEN}-- Done! --${RESTORE}"
echo " "