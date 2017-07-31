#!/bin/bash
BASEDIR=/root/sar
SANDBOXDIR=${BASEDIR}/.sandbox

if [[ $EUID -ne 0 ]]; then
   echo "The Search and Rescue Installer script must be run as root."
   exit 1
fi

# ---=[ PREREQUISITES ]=--- #
echo "Retrieving and installing required packages from Aptitude."
apt-get -y install cmake build-essential subversion autoconf libusb-1.0-0-dev

# ---=[ GIT CONFIG ]=--- #
echo "Pre-configuring git..."
git config --global user.email "sar@raspberrypi.local"
git config --global user.name "SAR"

# ---=[ PREP REPOSITORY ]=--- #
echo "Preparing repository..."
${BASEDIR}/devops/generatePatchedTree.sh

# ---=[ BUILDS ]=--- #
echo "Starting builds..."

# ---=[ BladeRF Build ]=--- #
echo "	Building Nuand bladeRF drivers..."
cd "${SANDBOXDIR}/bladeRF"
if [[ -d "${SANDBOXDIR}/bladeRF/build" ]]; then
	echo "		bladeRF build directory exists. Attempting to make build environment sane."
	cd build
	make clean
	cd ..
	rm -rf build/
fi
mkdir build
cd build
cmake ..
echo "		Build environment ready, starting build."
make install
echo "		Done building drivers. Updating ldconfig cache."
ldconfig

# ---=[ Yate Build ]=--- #
echo "	Building Yate core..."
cd "${SANDBOXDIR}/yate"
if [[ -f "${SANDBOXDIR}/yate/configure" ]]; then
	echo "		Yate configure script exists. Attempting to make build environment sane."
	make clean
fi
./autogen.sh
./configure
echo "		Build environment ready, starting build."
make
make install-noapi
echo "		Done building Yate. Updating ldconfig cache."
ldconfig

# ---=[ YateBTS Build ]=--- #
echo "	Building YateBTS..."
cd "${SANDBOXDIR}/yate-bts"
if [ -f "${SANDBOXDIR}/yate-bts/configure" ]; then
	echo "		YateBTS configure script exists. Attempting to make build environment sane."
	make clean
fi
./autogen.sh
./configure
echo "		Build environment ready, starting build."
make install
echo "		Done building YateBTS. Updating ldconfig cache."
ldconfig

echo "Builds complete."

# ---=[ USERS/GROUPS ]=--- #
echo "Creating users and groups..."
useradd yate
usermod -a -G yate yate
chown yate:yate /usr/local/etc/yate/*.conf
chmod g+w /usr/local/etc/yate/*.conf

# ---=[ UDEV/PRIORITIES ]=--- #
echo "Assigning priorities and udev rules..."

# Commented out because bladeRF installation handles this
# echo "# nuand bladeRF" > /etc/udev/rules.d/90-yate.rules
# echo "ATTR{idVendor}==\"1d50\", ATTR{idProduct}==\"6066\", MODE=\"660\", GROUP=\"yate\"" >> /etc/udev/rules.d/90-yate.rules
udevadm control --reload-rules

echo "@yate hard nice -20" >> /etc/security/limits.conf
echo "@yate hard rtprio 99" >> /etc/security/limits.conf

# ---=[ REORG CONFIG FILES ]=--- #
echo "Shifting configuration files to FAT32 partition..."
echo "	ybts.conf"
mv /usr/local/etc/yate/ybts.conf /boot/ybts.conf
ln -s /boot/ybts.conf /usr/local/etc/yate/ybts.conf
echo "	subscribers.conf"
mv /usr/local/etc/yate/subscribers.conf /boot/subscribers.conf
ln -s /boot/subscribers.conf /usr/local/etc/yate/subscribers.conf
echo "	sar.conf"
mv /usr/local/etc/yate/sar.conf /boot/sar.conf
ln -s /boot/sar.conf /usr/local/etc/yate/sar.conf

# ---=[ INIT SCRIPT ]=--- #
echo "Creating init script..."
cp ${BASEDIR}/devops/raspberrypi/YateBTS /etc/init.d/YateBTS
update-rc.d YateBTS enable

# ---=[ PI ENVIRONMENT ]=--- #
echo "Adjusting command line settings for Pi environment..."
cp ${BASEDIR}/devops/raspberrypi/cmdline.txt /boot/cmdline.txt
cp ${BASEDIR}/devops/raspberrypi/config.txt /boot/config.txt

# ---=[ BLADERF SYMLINK ]=--- #
echo "Creating bladeRF transceiver link..."
ln -s /usr/local/lib/yate/server/bts/transceiver-bladerf /usr/local/lib/yate/server/bts/transceiver

# TODO: Automatically write the SAR bits into javascript.conf and ybts.conf
# ybts.conf -> [transceiver] -> radio_read_priority=highest
# ybts.conf -> [transceiver] -> radio_send_priority=high

# ---=[ JS CONFIGURATION ]=--- #
#sed -i -- 's/\;routing=/routing=searchandrescue.js/g' /usr/local/etc/yate/javascript.conf
sed -i -- 's/\;mode=nib/mode=searchandrescue/g' /boot/ybts.conf