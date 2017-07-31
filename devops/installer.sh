#!/bin/bash
BASEDIR=/root/sar
SANDBOXDIR=${BASEDIR}/.sandbox
LOGDIR=/boot/sar_installer

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

echo -e "${BLUE}+ Creating logging environment.${RESTORE}"
mkdir -p ${LOGDIR}

# ---=[ PREREQUISITES ]=--- #
echo -e "${BLUE}+ Retrieving and installing required packages from Aptitude.${RESTORE}"
apt-get -y install cmake build-essential subversion autoconf libusb-1.0-0-dev > "${LOGDIR}/00_aptitude.log" 2>&1

# ---=[ GIT CONFIG ]=--- #
echo -e "${BLUE}+ Configuring git...${RESTORE}"
echo -e "${GREEN}	- Setting default sandbox credentials${RESTORE}"
git config --global user.email "sar@raspberrypi.local" > "${LOGDIR}/01_git_config.log" 2>&1
git config --global user.name "SAR" >> "${LOGDIR}/01_git_config.log" 2>&1

# ---=[ PREP REPOSITORY ]=--- #
echo -e "${GREEN}	- Preparing repository (this will take a moment)${RESTORE}"
${BASEDIR}/devops/generatePatchedTree.sh > "${LOGDIR}/02_deploy_source.log" 2>&1

# ---=[ BUILDS ]=--- #
echo -e "${BLUE}+ Starting builds...${RESTORE}"

# ---=[ BladeRF Build ]=--- #
echo -e "${BLUE}	+ Building Nuand bladeRF drivers...${RESTORE}"
cd "${SANDBOXDIR}/bladeRF"
if [[ -d "${SANDBOXDIR}/bladeRF/build" ]]; then
	echo -e "${YELLOW}		! bladeRF build directory exists. Attempting to make build environment sane."
	cd build
	make clean > "${LOGDIR}/03_bladerf_build.log" 2>&1
	cd ..
	rm -rf build/
fi
mkdir build
cd build
cmake .. >> "${LOGDIR}/03_bladerf_build.log" 2>&1
echo -e "${GREEN}		+ Build environment ready, starting build.${RESTORE}"
make install >> "${LOGDIR}/03_bladerf_build.log" 2>&1
echo -e "${GREEN}		+ Done building drivers. Updating ldconfig cache.${RESTORE}"
ldconfig >> "${LOGDIR}/03_bladerf_build.log" 2>&1

# ---=[ Yate Build ]=--- #
echo -e "${BLUE}	+ Building Yate core...${RESTORE}"
cd "${SANDBOXDIR}/yate"
if [[ -f "${SANDBOXDIR}/yate/configure" ]]; then
	echo -e "${YELLOW}		! Yate configure script exists. Attempting to make build environment sane.${RESTORE}"
	make clean > "${LOGDIR}/04_yate_build.log" 2>&1
fi
./autogen.sh >> "${LOGDIR}/04_yate_build.log" 2>&1
./configure >> "${LOGDIR}/04_yate_build.log" 2>&1
echo -e "${GREEN}		+ Build environment ready, starting build.${RESTORE}"
make >> "${LOGDIR}/04_yate_build.log" 2>&1
make install-noapi >> "${LOGDIR}/04_yate_build.log" 2>&1
echo -e "${GREEN}		+ Done building Yate. Updating ldconfig cache."
ldconfig >> "${LOGDIR}/04_yate_build.log" 2>&1

# ---=[ YateBTS Build ]=--- #
echo -e "${BLUE}	+ Building YateBTS...${RESTORE}"
cd "${SANDBOXDIR}/yate-bts"
if [ -f "${SANDBOXDIR}/yate-bts/configure" ]; then
	echo -e "${YELLOW}		! YateBTS configure script exists. Attempting to make build environment sane.${RESTORE}"
	make clean > "${LOGDIR}/05_yatebts_build.log" 2>&1
fi
./autogen.sh >> "${LOGDIR}/05_yatebts_build.log" 2>&1
./configure >> "${LOGDIR}/05_yatebts_build.log" 2>&1
echo -e "${GREEN}		+ Build environment ready, starting build.${RESTORE}"
make install >> "${LOGDIR}/05_yatebts_build.log" 2>&1
echo -e "${GREEN}		+ Done building YateBTS. Updating ldconfig cache.${RESTORE}"
ldconfig >> "${LOGDIR}/05_yatebts_build.log" 2>&1

echo -e "${BLUE}+ Builds complete.${RESTORE}"

# ---=[ USERS/GROUPS ]=--- #
echo -e "${BLUE}+ Creating users and groups...${RESTORE}"
useradd yate >> "${LOGDIR}/06_user_config.log" 2>&1
usermod -a -G yate yate >> "${LOGDIR}/06_user_config.log" 2>&1
chown yate:yate /usr/local/etc/yate/*.conf >> "${LOGDIR}/06_user_config.log" 2>&1
chmod g+w /usr/local/etc/yate/*.conf >> "${LOGDIR}/06_user_config.log" 2>&1

# ---=[ UDEV/PRIORITIES ]=--- #
echo -e "${BLUE}+ Assigning priorities and udev rules...${RESTORE}"

# Commented out because bladeRF installation handles this
# echo "# nuand bladeRF" > /etc/udev/rules.d/90-yate.rules
# echo "ATTR{idVendor}==\"1d50\", ATTR{idProduct}==\"6066\", MODE=\"660\", GROUP=\"yate\"" >> /etc/udev/rules.d/90-yate.rules
udevadm control --reload-rules >> "${LOGDIR}/07_udev.log" 2>&1

echo "@yate hard nice -20" >> /etc/security/limits.conf
echo "@yate hard rtprio 99" >> /etc/security/limits.conf

# ---=[ REORG CONFIG FILES ]=--- #
echo -e "${BLUE}+ Shifting configuration files to FAT32 partition...${RESTORE}"
mv /usr/local/etc/yate/ybts.conf /boot/ybts.conf >> "${LOGDIR}/08_configs.log" 2>&1
ln -s /boot/ybts.conf /usr/local/etc/yate/ybts.conf >> "${LOGDIR}/08_configs.log" 2>&1
echo -e "${GREEN}	+ ybts.conf${RESTORE}"

mv /usr/local/etc/yate/subscribers.conf /boot/subscribers.conf >> "${LOGDIR}/08_configs.log" 2>&1
ln -s /boot/subscribers.conf /usr/local/etc/yate/subscribers.conf >> "${LOGDIR}/08_configs.log" 2>&1
echo -e "${GREEN}	+ subscribers.conf${RESTORE}"

mv /usr/local/etc/yate/sar.conf /boot/sar.conf >> "${LOGDIR}/08_configs.log" 2>&1
ln -s /boot/sar.conf /usr/local/etc/yate/sar.conf >> "${LOGDIR}/08_configs.log" 2>&1
echo -e "${GREEN}	+ sar.conf${RESTORE}"

# ---=[ INIT SCRIPT ]=--- #
echo -e "${BLUE}+ Creating init script...${RESTORE}"
cp ${BASEDIR}/devops/raspberrypi/YateBTS /etc/init.d/YateBTS >> "${LOGDIR}/09_init.log" 2>&1
update-rc.d YateBTS enable >> "${LOGDIR}/09_init.log" 2>&1

# ---=[ PI ENVIRONMENT ]=--- #
echo -e "${BLUE}+ Adjusting command line settings for Pi environment...${RESTORE}"
cp ${BASEDIR}/devops/raspberrypi/cmdline.txt /boot/cmdline.txt
cp ${BASEDIR}/devops/raspberrypi/config.txt /boot/config.txt

# ---=[ BLADERF SYMLINK ]=--- #
echo -e "${BLUE}+ Creating bladeRF transceiver link...${RESTORE}"
ln -s /usr/local/lib/yate/server/bts/transceiver-bladerf /usr/local/lib/yate/server/bts/transceiver

# TODO: Automatically write the SAR bits into javascript.conf and ybts.conf
# ybts.conf -> [transceiver] -> radio_read_priority=highest
# ybts.conf -> [transceiver] -> radio_send_priority=high

# ---=[ JS CONFIGURATION ]=--- #
#sed -i -- 's/\;routing=/routing=searchandrescue.js/g' /usr/local/etc/yate/javascript.conf
sed -i -- 's/\;mode=nib/mode=searchandrescue/g' /boot/ybts.conf

echo -e "${GREEN}-- Done! --${RESTORE}"