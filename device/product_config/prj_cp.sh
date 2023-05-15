#! /bin/sh
if [ $# -eq 0 ];
then
	echo "usage : prj_cp.sh source_project target_project"
fi

if [ $# -eq 1 ];
then
	echo "usage : prj_cp.sh source_project target_project"
fi

if [ $# -eq 2 ];
then
	echo "copy project config from $1 to $2"
	
	svn export	./defconfigs/$1					./defconfigs/$2
	svn export	./flashmap/$1					./flashmap/$2
	svn export	./package/$1					./package/$2
	svn export	./platform/driver/$1			./platform/driver/$2
	svn export	./platform/sdk/driver/$1		./platform/sdk/driver/$2
	svn export	./platform/sdk/os/kernel/$1		./platform/sdk/os/kernel/$2
	svn export	./platform/sdk/os/rootfs/$1		./platform/sdk/os/rootfs/$2
	svn export	../platform/sdk/os/rootfs/base-files/product/$1		../platform/sdk/os/rootfs/base-files/product/$2
	svn export	../platform/sdk/os/drivers/wlan/bcm43236/production/nvrams/$1		../platform/sdk/os/drivers/wlan/bcm43236/production/nvrams/$2
	mv 			../platform/sdk/os/drivers/wlan/bcm43236/production/nvrams/$1/$1.nvm	../platform/sdk/os/drivers/wlan/bcm43236/production/nvrams/$2/$2.nvm

	mv 			./defconfigs/$2/$1_defconfig	./defconfigs/$2/$2_defconfig
	
svn add		./defconfigs/$2
svn add		./flashmap/$2
svn add		./package/$2
svn add		./platform/driver/$2
svn add		./platform/sdk/driver/$2
svn add		./platform/sdk/os/kernel/$2
svn add		./platform/sdk/os/rootfs/$2
svn add		../platform/sdk/os/rootfs/base-files/product/$2
svn add		../platform/sdk/os/drivers/wlan/bcm43236/production/nvrams/$2

echo "#need modify. created by script." >> ../platform/sdk/os/rootfs/base-files/Makefile
echo "modify the file ../platform/sdk/os/rootfs/base-files/Makefile"
echo "modify the file ./defconfigs/$2/$2_defconfig"
fi

