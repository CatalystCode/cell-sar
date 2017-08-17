#!/bin/bash
CONFIGURATIONDIR=/boot
LOGDIR=${CONFIGURATIONDIR}/sar_installer_log
OVERLAYDIR=${CONFIGURATIONDIR}/sar_overlay

BASEDIR=/root/sar
SAR_VERSION=`git --git-dir="${BASEDIR}/.git" --work-tree="${BASEDIR}" rev-parse --short HEAD`

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

# ---=[ BUILDS ]=--- #
echo -e "${CYAN}+ Starting builds...${RESTORE}"

# ---=[ Yate Build ]=--- #
echo -e "${CYAN}	+ Building Yate core...${RESTORE}"
cd "${BASEDIR}/bts/yate"
if [[ -f "${BASEDIR}/bts/yate/configure" ]]; then
	echo -e "${YELLOW}		! Yate configure script exists. Attempting to make build environment sane.${RESTORE}"
	make clean > "${LOGDIR}/03_yate_build.log" 2>&1
	if [[ $? != 0 ]]; then
		echo -e "${RED}		! Cannot establish build environment sanity. (Error code $?).${RESTORE}"
		exit 1
	fi
fi
chmod a+x ${BASEDIR}/bts/yate/*.sh
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
cd "${BASEDIR}/bts/yatebts"
if [ -f "${BASEDIR}/bts/yatebts/config.log" ]; then
	echo -e "${YELLOW}		! YateBTS configure script exists. Attempting to make build environment sane.${RESTORE}"
	make clean > "${LOGDIR}/04_yatebts_build.log" 2>&1
	if [[ $? != 0 ]]; then
		echo -e "${RED}		! Cannot establish build environment sanity. (Error code $?).${RESTORE}"
		exit 1
	fi
fi

chmod a+x ${BASEDIR}/bts/yatebts/*.sh
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

# ---=[ OCP Build ]=--- #
echo -e "${CYAN}	+ Building OCP...${RESTORE}"
if [ -d "/home/yate/ocp" ]; then
	echo -e "${YELLOW}		! OCP build already exists. Attempting to make build environment sane.${RESTORE}"
	rm -rf "/home/yate/ocp"
fi

mkdir -p /home/yate/ocp
cd "/home/yate/ocp"
cmake ${BASEDIR}/ocp >> "${LOGDIR}/05_ocp_build.log" 2>&1
if [[ $? != 0 ]]; then
	echo -e "${RED}		! cmake failed. (Error code $?).${RESTORE}"
	exit 1
fi

echo -e "${GREEN}		+ Build environment ready, starting build.${RESTORE}"
make >> "${LOGDIR}/05_ocp_build.log" 2>&1
if [[ $? != 0 ]]; then
	echo -e "${RED}			! Build failed. (Error code $?).${RESTORE}"
	exit 1
fi

echo -e "${CYAN}+ Builds complete.${RESTORE}"

# ---=[ USERS/GROUPS ]=--- #
echo -e "${CYAN}+ Creating users and groups...${RESTORE}"
useradd yate >> "${LOGDIR}/05_user_config.log" 2>&1
usermod -a -G yate yate >> "${LOGDIR}/05_user_config.log" 2>&1
usermod -a -G dialout yate >> "${LOGDIR}/05_user_config.log" 2>&1 # for UART access
chown yate:yate /usr/local/etc/yate/*.conf >> "${LOGDIR}/05_user_config.log" 2>&1
chmod g+w /usr/local/etc/yate/*.conf >> "${LOGDIR}/05_user_config.log" 2>&1

# ---=[ REORG CONFIG FILES ]=--- #
echo -e "${CYAN}+ Moving/linking configuration files on FAT32 partition...${RESTORE}"
if [[ -e "${OVERLAYDIR}/ybts.conf" ]]; then
	cp "${OVERLAYDIR}/ybts.conf" "${CONFIGURATIONDIR}/ybts.conf"
	rm /usr/local/etc/yate/ybts.conf
	ln -s "${CONFIGURATIONDIR}/ybts.conf" /usr/local/etc/yate/ybts.conf >> "${LOGDIR}/07_configs.log" 2>&1
	echo -e "${GREEN}	+ ybts.conf (from overlay)${RESTORE}"
else
	mv /usr/local/etc/yate/ybts.conf "${CONFIGURATIONDIR}/ybts.conf" >> "${LOGDIR}/07_configs.log" 2>&1
	ln -s "${CONFIGURATIONDIR}/ybts.conf" /usr/local/etc/yate/ybts.conf >> "${LOGDIR}/07_configs.log" 2>&1
	echo -e "${GREEN}	+ ybts.conf${RESTORE}"
fi

if [[ -e "${OVERLAYDIR}/sar.conf" ]]; then
	cp "${OVERLAYDIR}/sar.conf" "${CONFIGURATIONDIR}/sar.conf"
	rm /usr/local/etc/yate/sar.conf
	ln -s "${CONFIGURATIONDIR}/sar.conf" /usr/local/etc/yate/sar.conf >> "${LOGDIR}/07_configs.log" 2>&1
	echo -e "${GREEN}	+ sar.conf (from overlay)${RESTORE}"
else
	mv /usr/local/etc/yate/sar.conf "${CONFIGURATIONDIR}/sar.conf" >> "${LOGDIR}/07_configs.log" 2>&1
	ln -s "${CONFIGURATIONDIR}/sar.conf" /usr/local/etc/yate/sar.conf >> "${LOGDIR}/07_configs.log" 2>&1
	echo -e "${GREEN}	+ sar.conf${RESTORE}"
fi

if [[ -e "${OVERLAYDIR}/UserConfig.txt" ]]; then
	cp "${OVERLAYDIR}/UserConfig.txt" "${CONFIGURATIONDIR}/UserConfig.txt"
	rm /home/yate/ocp/bin/UserConfig.txt
	ln -s "${CONFIGURATIONDIR}/UserConfig.txt" /home/yate/ocp/bin/UserConfig.txt >> "${LOGDIR}/07_configs.log" 2>&1
	echo -e "${GREEN}	+ UserConfig.txt (from overlay)${RESTORE}"
else
	if [[ ! -e "/home/yate/ocp/bin/UserConfig.txt" ]]; then
		touch /home/yate/ocp/bin/UserConfig.txt
	fi
	mv /home/yate/ocp/bin/UserConfig.txt "${CONFIGURATIONDIR}/UserConfig.txt" >> "${LOGDIR}/07_configs.log" 2>&1
	ln -s "${CONFIGURATIONDIR}/UserConfig.txt" /home/yate/ocp/bin/UserConfig.txt >> "${LOGDIR}/07_configs.log" 2>&1
	echo -e "${GREEN}	+ UserConfig.txt${RESTORE}"
fi

# ---=[ POST-INSTALL TASKS ]=--- #
echo -e "${CYAN}+ Running system post-installation tasks...${RESTORE}"

chown -R yate:yate /home/yate >> "${LOGDIR}/05_ocp_build.log" 2>&1
echo -e "${GREEN}	+ Ensured yate user ownership of home directory.${RESTORE}"

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
echo -e "${GREEN}	+ Created YateBTS init script${RESTORE}"

cp ${BASEDIR}/devops/raspberrypi/ocp.service /etc/systemd/system/ocp.service >> "${LOGDIR}/08_init.log" 2>&1
echo -e "${GREEN}  + Created ocp.service for systemd${RESTORE}"

touch /var/log/YateBTS.log
chown yate:yate /var/log/YateBTS.log
echo -e "${GREEN}	+ Created log file${RESTORE}"

# ---=[ PI ENVIRONMENT ]=--- #
echo -e "${CYAN}+ Overlaying custom configuration for Pi environment...${RESTORE}"
# cp ${BASEDIR}/devops/raspberrypi/cmdline.txt /boot/cmdline.txt
cp ${BASEDIR}/devops/raspberrypi/config.txt /boot/config.txt

# ---=[ POST-INSTALL SCRIPT ]=--- #
echo -e "${CYAN}+ Checking for post-install actions...${RESTORE}"
if [[ -f "${OVERLAYDIR}/postinstall.sh" ]]; then
	echo -e "${GREEN}	+ Running post-install script."
	chmod a+x ${OVERLAYDIR}/postinstall.sh
	${OVERLAYDIR}/postinstall.sh > "${LOGDIR}/99_postinstall.log"
else
	echo -e "${GREEN}	+ No post-install script found."
fi

echo " "
echo -e "${GREEN}-- Done! --${RESTORE}"
echo " "
