#!/bin/sh

. /lib/lsb/init-functions
. /etc/default/rcS
if [ "$VERBOSE" != no ]; then
	log_begin_msg "Mounting local filesystems..."
fi

mount -t ext4 -o rw,sync,errors=remount-ro,barrier=1 /dev/mmcblk1p1 /var/lib/humaxtv
mount -t ext4 -o rw,sync,errors=remount-ro,barrier=1 /dev/mmcblk1p2 /var/lib/humaxtv_backup
mount -t ext4 -o rw,sync,errors=remount-ro,barrier=1 /dev/mmcblk1p3 /var/lib/humaxtv_user

# format and mount emmc for tsr
emmc_tsr_device="/dev/mmcblk0"
emmc_tsr_fstype="ext4"
emmc_tsr_mountd="/mnt/sd0"

if ! mount -t $emmc_tsr_fstype $emmc_tsr_device $emmc_tsr_mountd 2> /dev/null; then
	case "$emmc_tsr_fstype" in
	ext2)
		mkfs.ext2 $emmc_tsr_device
		mount -t ext2 $emmc_tsr_device $emmc_tsr_mountd
		;;
	ext3)
		mkfs.ext3 $emmc_tsr_device
		mount -t ext3 $emmc_tsr_device $emmc_tsr_mountd
		;;
	ext4)
		mkfs.ext4 $emmc_tsr_device
		mount -t ext4 $emmc_tsr_device $emmc_tsr_mountd
		;;
	vfat)
		mkdosfs -F 32 $emmc_tsr_device
		mount -t  $emmc_tsr_device $emmc_tsr_mountd
		;;
	*)
		echo "not support fstype for tsr emmc($emmc_tsr_fstype)"
		;;
	esac	

fi

# Set up MoCA link, if present
[ -e /dev/bmoca0 ] && mocacfg boot
