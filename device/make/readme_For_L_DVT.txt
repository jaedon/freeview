

[ For DVT Application ]
# YOU SHOULD MODIFY BELOW CODE TO OPEN AND COMMENT #
# Input Command In Current Position 'device/make' #

 * release build
    : vi ../../device/platform/sdk/os/rootfs/base-files/nand/release/base/etc/init.d/S35mountall.sh

 * debug build
    : vi ../../device/platform/sdk/os/rootfs/base-files/nand/debug/base/etc/init.d/S35mountall.sh

   # Should modify below line to comment(block)
    [ FILE : S35mountall.sh ]
     (comment)# mount -t ubifs ubi1:dbdata /var/lib/humaxtv
     (comment)# mount -t ubifs ubi1:dbbackup /var/lib/humaxtv_backup
     (comment)# mount -t ubifs ubi1:dbuser /var/lib/humaxtv_user

 * Open 'define' (Should open below line)
  [ FILE : di_flash_nand.c ]
    : vi ../platform/driver/di/flash/di_flash_nand.c
    
      (open)  #define CONFIG_IRDETO_LOADER_DVT_TEST

