
If you have built as hdr1000sph2_netflix_defconfig in apps makefile, 
   below action is already applied automatically. please check this on hdr1000sph2_defconfig.

   "cp hdr1000sph2_netflix_defconfig  hdr1000sph2_defconfig"

Changes as following.
Enable CONFIG_NETFLIX_TEMP flag on hdr1000sph2_defconfig.
Change all of board revision folder to rev0.41. (rev0.4 means original freesat software, But rev0.41 includes "bmem size up" etc.)


