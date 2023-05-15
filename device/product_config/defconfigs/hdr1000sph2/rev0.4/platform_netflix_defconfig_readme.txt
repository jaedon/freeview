
[device\product_config\defconfigs\hdr1000sph2\rev0.4]
"platform_netflix_defconfig" is a temporary defconfig for Netflix service and Playready 2.5. 
"platform_netflix_defconfig" file will be erased when Netflix and Playready 2.5 have stabilization, and it will be replaced with the platform_defconfig.


[Usage]
To enable the temporary defconfig for Netflix service and Playready 2.5,
edit the device\product_config\defconfigs\hdr1000sph2\hdr1000sph2_defconfig" and turn on "CONFIG_SUPPORT_NETFLIX_TEMP=y"
Then, di defconfig for hdr1000sph2 will be changed with "platform_netflix_defconfig", not original "platform_defconfig".


[Reference]
======================================================
Revision: 20028
======================================================
Author: jokim
Date: 2015년 3월 4일 수요일 오후 9:19:49
Message:
1. jokim
2. hdr1000sph2 netflix
3. N/A
4. Add netflix config temporary
----
Modified : /trunk/platform/make_bcm2/Makefile
Modified : /trunk/platform/make_bcm2/config.sh


======================================================
Revision: 20027
======================================================
Author: jokim
Date: 2015년 3월 4일 수요일 오후 9:19:34
Message:
1. jokim
2. hdr1000sph2 netflix
3. N/A
4. Add netflix config temporary
----
Modified : /trunk/platform/product_config/defconfigs/hdr1000sph2/hdr1000sph2_defconfig

