#!/bin/bash
###############################################################################
# Search and Rescue
# Image Rolling Utility
#    This tool mounts and optionally executes the installer against a Raspbian
#    base image. Once complete, the input image will be pre-loaded with the
#    Search and Rescue software preconfigured; configuration files will be in
#    the FAT32 partition of the image.
###############################################################################

### DEFAULTS ###
IMAGE_WORKING_DIR=/root/sar
WORKING_DIR=/root/imgroll
GIT_PATH=""
IMAGE_PATH=""
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
	"$@" >> "${LOG_FILE}" 2>&1 # evaluates program to LOG_FILE including STDERR
	local errCode=$?
	if [ $errCode -ne 0 ]; then
		echo "-- '$*' exited with code $errCode" >> "${LOG_FILE}"
	fi
	echo $errCode
}
### /SAR INSTALLER COMMON CODE ###

### COMMAND LINE ARGUMENT PARSING ###
ADDL_ARGS=""
args=$(getopt -o igwd:h:: --long image:,git-path:,working-dir:,image-working-dir:,help -- "$@")
eval set -- "$args"
while [ $# -ge 1 ]; do
    case "$1" in
        -i|--image) IMAGE_PATH=$(readlink -f "$2") ; shift 2 ;;
        -g|--git-path) GIT_PATH=$(readlink -f "$2") ; shift 2 ;;
        -w|--working-dir) WORKING_DIR=$(readlink -f "$2") ; shift 2 ;;
        -d|--image-working-dir) IMAGE_WORKING_DIR=$(readlink -f "$2") ; shift 2 ;;
        -h|--help)
        echo " "
        echo "Search and Rescue / Image Rolling Utility"
        echo "  --image              Raspbian IMG file to mount"
        echo "  --git-path           Path to Git repository for SAR installer. If this is not given, the image"
        echo "                       will be mounted and the user will be put into a chroot environment inside it."
        echo "  --working-dir        Where the Raspbian image will be mounted"
        echo "  --image-working-dir  Location inside the image where the SAR build will take place"
        echo " "
        echo "syn: imgroll.sh <-i|--image=raspbian.img> [-g|--git-path=/path/to/git/repo] [-w|--working-dir=${WORKING_DIR}] [-i|--image-working-dir=${IMAGE_WORKING_DIR}]"
        echo " "
        exit 0
        ;;
        --) shift ; break ;;
        *) ADDL_ARGS="${ADDL_ARGS} $1=$2" ; shift 2 ;;
    esac
done
### /COMMAND LINE ARGUMENT PARSING ###

if [[ -z "${IMAGE_PATH}" ]]; then
    errorOut "No Raspbian image specified; use --help to show command syntax."
    exit 1
fi

if [[ $EUID -ne 0 ]]; then
    errorOut "SAR image roller script must be run as root."
	exit 1
fi

headerOut "Setting up working environment at ${WORKING_DIR}..."
((OUTPUT_DEPTH++))

if [ "$(ls -A ${WORKING_DIR})" ]; then
    errorOut "Working directory is not empty."
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

if [ ! -z "${GIT_PATH}" ]; then
    mkdir -p "${WORKING_DIR}${IMAGE_WORKING_DIR}"
    cp -R "${GIT_PATH}"/* "${WORKING_DIR}${IMAGE_WORKING_DIR}"
    cp -R "${GIT_PATH}"/.git "${WORKING_DIR}${IMAGE_WORKING_DIR}"
    normalOut "Moved git repository to rootfs..."

    normalOut "Setting up prerequisite packages..."
    chroot "${WORKING_DIR}" /bin/bash -c "apt-get update && apt-get -y install git dos2unix"

    normalOut "Verifying permissions..."
    chroot "${WORKING_DIR}" /bin/bash -c "chmod a+x ${IMAGE_WORKING_DIR}/devops/*.sh"

    normalOut "Running SAR installer..."
    chroot "${WORKING_DIR}" /bin/bash -c "${IMAGE_WORKING_DIR}/devops/installer.sh --working-dir=${IMAGE_WORKING_DIR} ${ADDL_ARGS}"
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