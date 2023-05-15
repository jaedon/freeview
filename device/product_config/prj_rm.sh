#! /bin/sh
if [ $# -eq 0 ];
then
	echo "usage : prj_rm.sh target_project"
fi

if [ $# -eq 1 ];
then
	svn rm --force 		./defconfigs/$1
	svn rm --force		./flashmap/$1
	svn rm --force		./package/$1
	svn rm --force		./platform/driver/$1
	svn rm --force		./platform/sdk/driver/$1
	svn rm --force		./platform/sdk/os/kernel/$1
	svn rm --force		./platform/sdk/os/rootfs/$1
	svn rm --force		../platform/sdk/os/rootfs/base-files/product/$1
	svn rm --force		../platform/sdk/os/drivers/wlan/bcm43236/production/nvrams/$1

	rm -rf	./defconfigs/$1
	rm -rf	./flashmap/$1
	rm -rf	./package/$1
	rm -rf	./platform/driver/$1
	rm -rf	./platform/sdk/driver/$1
	rm -rf	./platform/sdk/os/kernel/$1
	rm -rf	./platform/sdk/os/rootfs/$1
	rm -rf	../platform/sdk/os/rootfs/base-files/product/$1
	rm -rf	../platform/sdk/os/drivers/wlan/bcm43236/production/nvrams/$1

	echo "modify the file ../platform/sdk/os/rootfs/base-files/Makefile"
fi
