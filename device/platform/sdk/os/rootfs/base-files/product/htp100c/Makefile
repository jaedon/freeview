curdir=${shell pwd}

APP=APP

ifeq ($(CONFIG_PRODUCT_IMAGE_HWTEST),y)
APP=HWTEST
else # Not HWTEST
ifeq ($(CONFIG_PRODUCT_IMAGE_FACTORY),y)
APP=FACTORY
endif # CONFIG_PRODUCT_IMAGE_FACTORY
endif # CONFIG_PRODUCT_IMAGE_HWTEST

rootfs-product:
	@echo "[APP : $(APP)]"
ifeq ($(APP),APP)
	@echo "[DEBUG_RELEASE_ENV : $(DEBUG_RELEASE_ENV)]"
ifeq ($(DEBUG_RELEASE_ENV),release)
	@echo "copy files for release"
	@if [ -d "$(curdir)/release" ] ; then \
		cp -fa $(curdir)/release/* $(prefix)/; \
	fi;
	@rm -f $(prefix)/etc/shadow;
	@rm -f $(prefix)/etc/fstab;
endif
endif # APP

rootfs-unneeded:
	@echo "[APP : $(APP)]"
ifeq ($(APP),APP)
	@echo "[DEBUG_RELEASE_ENV : $(DEBUG_RELEASE_ENV)]"
ifeq ($(DEBUG_RELEASE_ENV),release)
	@echo "Deleting hdd nodes"
	@-rm -rf $(prefix)/dev/hda*
	@echo "Deleting usb buses"
	@-rm -rf $(prefix)/dev/bus
	@echo "Deleting usb nodes"
	@-rm -rf $(prefix)/dev/sda*
	@-rm -rf $(prefix)/dev/sdb*
	@-rm -rf $(prefix)/dev/sdc*
	@-rm -rf $(prefix)/dev/sdd*
	@-rm -rf $(prefix)/dev/sde*
	@-rm -rf $(prefix)/dev/sdf*
	@-rm -rf $(prefix)/dev/sdg*
	@-rm -rf $(prefix)/dev/sdh*
	@-rm -rf $(prefix)/dev/sdi*
	@echo "Deleting bt nodes"
	@-rm -rf $(prefix)/dev/bt*
	@echo "Deleting emmc nodes"
	@-rm -rf $(prefix)/dev/mmcblk*
	@echo "Deleting nexus nodes"
	@-rm -rf $(prefix)/dev/nexus_*
	@echo "Deleting ptyp"
	@-rm -rf $(prefix)/dev/ptyp*
#	@echo "Deleting tty"
#	@-rm -rf $(prefix)/dev/tty*
	@echo "Deleting mixer"
	@-rm -rf $(prefix)/dev/mixer*
	@echo "Deleting dsp"
	@-rm -rf $(prefix)/dev/dsp*
	@echo "Deleting etc..."
	-rm -rf $(prefix)/dev/brcm_cma0
	-rm -rf $(prefix)/dev/bmoca0
	-rm -rf $(prefix)/dev/bcmfpga
	-rm -rf $(prefix)/dev/kmem
	-rm -rf $(prefix)/dev/kmsg
	-rm -rf $(prefix)/dev/m01fmb0
	-rm -rf $(prefix)/dev/hmxsdcon
	-rm -rf $(prefix)/dev/ppp
	-rm -rf $(prefix)/dev/port
	-rm -rf $(prefix)/dev/ptmx
	-rm -rf $(prefix)/dev/ram
	-rm -rf $(prefix)/dev/ram1
	-rm -rf $(prefix)/dev/ram2
	-rm -rf $(prefix)/dev/ram3
	-rm -rf $(prefix)/dev/random
	-rm -rf $(prefix)/dev/urandom
	-rm -rf $(prefix)/dev/rtc
	-rm -rf $(prefix)/dev/tty
	-rm -rf $(prefix)/dev/tty0
	-rm -rf $(prefix)/dev/tty1*
	-rm -rf $(prefix)/dev/tty2*
	-rm -rf $(prefix)/dev/tty3*
	-rm -rf $(prefix)/dev/tty4*
	-rm -rf $(prefix)/dev/tty5*
	-rm -rf $(prefix)/dev/tty6*
	-rm -rf $(prefix)/dev/tty7*
	-rm -rf $(prefix)/dev/tty8*
	-rm -rf $(prefix)/dev/tty9*
	-rm -rf $(prefix)/dev/ttyS1*
	-rm -rf $(prefix)/dev/ttyS2*
	-rm -rf $(prefix)/dev/ttyS3*
	-rm -rf $(prefix)/dev/ttyS4*
	-rm -rf $(prefix)/dev/ttyS5*
	-rm -rf $(prefix)/dev/ttyS6*
	-rm -rf $(prefix)/dev/ttyUSB*
	-rm -rf $(prefix)/dev/mtd0
	-rm -rf $(prefix)/dev/mtd2
	-rm -rf $(prefix)/dev/mtd6
	-rm -rf $(prefix)/dev/mtd7
	-rm -rf $(prefix)/dev/mtd8
	-rm -rf $(prefix)/dev/mtd9
	-rm -rf $(prefix)/dev/mtd10
	-rm -rf $(prefix)/dev/mtd11
	-rm -rf $(prefix)/dev/mtd12
	-rm -rf $(prefix)/dev/mtd13
	-rm -rf $(prefix)/dev/mtd14
	-rm -rf $(prefix)/dev/mtd15
	-rm -rf $(prefix)/dev/mtdblock0
	-rm -rf $(prefix)/dev/mtdblock2
	-rm -rf $(prefix)/dev/mtdblock6
	-rm -rf $(prefix)/dev/mtdblock7
	-rm -rf $(prefix)/dev/mtdblock8
	-rm -rf $(prefix)/dev/mtdblock9
	-rm -rf $(prefix)/dev/mtdblock10
	-rm -rf $(prefix)/dev/mtdblock11
	-rm -rf $(prefix)/dev/mtdblock12
	-rm -rf $(prefix)/dev/mtdblock13
	-rm -rf $(prefix)/dev/mtdblock14
	-rm -rf $(prefix)/dev/mtdblock15
	-rm -rf $(prefix)/dev/wake0
endif
endif # APP
