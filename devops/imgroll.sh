#!/bin/bash
WORKING_DIR=/root/imgroll
IMAGE_WORKING_DIR=/root/sar

#########################################################################################

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
	"$@" >> "${LOGFILE}" 2>&1 # evaluates program LOGFILE including STDERR
	local errCode=$?
	if [ $errCode -ne 0 ]; then
		echo "-- '$@' exited with code $errCode" >> "${LOGFILE}"
	fi
	echo $errCode
}
### /SAR INSTALLER COMMON CODE ###

if [ $# -lt 1 ]; then
    echo "syn: imgroll.sh <image file> [git repository path, drops to shell if not given] [working dir, defaults to ${WORKING_DIR}]"
    exit 1
fi

if [[ $EUID -ne 0 ]]; then
    errorOut "SAR image roller script must be run as root."
	exit 1
fi

IMAGE_PATH=$(readlink -f "$1")
if [ $# -gt 1 ]; then
    GIT_PATH=$(readlink -f "$2")
fi
if [ $# -gt 2 ]; then
    WORKING_DIR=$(readlink -f "$3")
fi

headerOut "Setting up working environment at ${WORKING_DIR}..."
((OUTPUT_DEPTH++))

if [ -d "${WORKING_DIR}" ]; then
    errorOut "Working directory already exists. Please remove it."
    exit 1
fi
if [ ! -z "${GIT_PATH}" ]; then
    if [ ! -d "${GIT_PATH}" ]; then
        errorOut "Git repository path does not exist."
        exit 1
    fi
else
    normalOut "Shell mode, skipping Git automation."    
fi

mkdir -p "${WORKING_DIR}"

headerOut "Interrogating image's partition table..."
((OUTPUT_DEPTH++))
PARTITION_BOOT_START=$(fdisk -l "${IMAGE_PATH}" | grep FAT32 | awk '{print $2}')
PARTITION_ROOT_START=$(fdisk -l "${IMAGE_PATH}" | grep Linux | awk '{print $2}')
PARTITION_STRIDE=$(fdisk -l "${IMAGE_PATH}" | grep Units | awk '{print $(NF-1)}')
BOOTFS_START=$((PARTITION_BOOT_START*PARTITION_STRIDE))
ROOTFS_START=$((PARTITION_ROOT_START*PARTITION_STRIDE))

normalOut "Partition stride is ${PARTITION_STRIDE} bytes"
normalOut "Boot partition (FAT32) starts at ( ${PARTITION_BOOT_START} * ${PARTITION_STRIDE}b = ${BOOTFS_START} )"
normalOut "Root partition (EXT4) starts at ( ${PARTITION_ROOT_START} * ${PARTITION_STRIDE}b = ${ROOTFS_START} )"
((OUTPUT_DEPTH--))

headerOut "Mounting image..."
((OUTPUT_DEPTH++))
mount -v -o offset=${ROOTFS_START} -t ext4 "${IMAGE_PATH}" "${WORKING_DIR}"
normalOut "rootfs @ ${ROOTFS_START} -> ${WORKING_DIR}"
if [ -d "${WORKING_DIR}/boot" ]; then
    mkdir -p "${WORKING_DIR}/boot"
fi
mount -v -o offset=${BOOTFS_START} -t vfat "${IMAGE_PATH}" "${WORKING_DIR}"/boot
normalOut "bootfs @ ${BOOTFS_START} -> ${WORKING_DIR}/boot"

mount -t proc proc "${WORKING_DIR}/proc"
normalOut "proc (host system) -> ${WORKING_DIR}/proc"

mount -t sysfs sys "${WORKING_DIR}/sys"
normalOut "sysfs (host system) -> ${WORKING_DIR}/sys"

mount -o bind /dev "${WORKING_DIR}/dev"
normalOut "udev (bound) -> ${WORKING_DIR}/dev"
((OUTPUT_DEPTH--))
((OUTPUT_DEPTH--))

headerOut "Preparing content of jailed environment..."
((OUTPUT_DEPTH++))
chroot "${WORKING_DIR}" /bin/bash -c "source /etc/profile"
normalOut "Set environment profile."

if [ ! -z ${GIT_PATH} ]; then
    mkdir -p "${WORKING_DIR}${IMAGE_WORKING_DIR}"
    cp -R "${GIT_PATH}"/* "${WORKING_DIR}${IMAGE_WORKING_DIR}"
    cp -R "${GIT_PATH}"/.git "${WORKING_DIR}${IMAGE_WORKING_DIR}"
    normalOut "Moved git repository to rootfs..."

    normalOut "Setting up prerequisite packages..."
    chroot "${WORKING_DIR}" /bin/bash -c "apt-get update && apt-get -y install git dos2unix"

    normalOut "Verifying permissions..."
    chroot "${WORKING_DIR}" /bin/bash -c "chmod a+x ${IMAGE_WORKING_DIR}/devops/*.sh"

    normalOut "Running SAR installer..."
    chroot "${WORKING_DIR}" /bin/bash -c "${IMAGE_WORKING_DIR}/devops/installer.sh"
else
    warningOut "Skipping git automation, dropping to chroot environment instead."
    chroot "${WORKING_DIR}" /bin/bash
fi

((OUTPUT_DEPTH--))

headerOut "Cleaning up..."
((OUTPUT_DEPTH++))

chroot "${WORKING_DIR}" /bin/bash -c "sync;sync"
normalOut "Requested virtual block device synchronization."

headerOut "Unmounting filesystems..."
((OUTPUT_DEPTH++))

umount "${WORKING_DIR}/dev"
normalOut "udev"

umount "${WORKING_DIR}/sys"
normalOut "sysfs"

umount "${WORKING_DIR}/proc"
normalOut "proc"

umount "${WORKING_DIR}/boot"
normalOut "bootfs"

umount "${WORKING_DIR}"
normalOut "rootfs"
((OUTPUT_DEPTH--))

rm -r "${WORKING_DIR}"
normalOut "Removed working environment"