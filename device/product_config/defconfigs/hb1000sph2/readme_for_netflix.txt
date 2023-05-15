
If you have built as hb1000sph2_netflix_defconfig in apps makefile, 
   below action is already applied automatically. please check this on hb1000sph2_defconfig.

   "cp hb1000sph2_netflix_defconfig  hb1000sph2_defconfig"

Changes as following.
Enable CONFIG_NETFLIX_TEMP flag on hb1000sph2_defconfig.
Change all of board revision folder to rev0.21. (rev0.2 means original freesat software, But rev0.21 includes "bmem size up" etc.)


