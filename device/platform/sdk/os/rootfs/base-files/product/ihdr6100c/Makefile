curdir=${shell pwd}

rootfs-product:
	@if [ -d "$(curdir)/$(CONFIG_BOARD_REVISION)/base" ] ; then \
		cp -fa $(curdir)/$(CONFIG_BOARD_REVISION)/base/* $(prefix)/; \
	fi;

rootfs-unneeded:
ifeq ($(CONFIG_BOARD_REVISION),rev0.2)
	@rm $(prefix)/usr/sbin/ubi*
endif
