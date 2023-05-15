curdir = $(shell pwd)

ifeq ($(CONFIG_MEDIA20),y)
OPENSSL=$(HUMAX_PACKAGE_DIR)/openssl/host_bin/openssl
export OPENSSL
endif
ifeq ($(CONFIG_CALYPSO),y)
OPENSSL=$(HUMAX_PACKAGE_DIR)/openssl/host_bin/openssl
export OPENSSL
endif

ifeq ($(CONFIG_ROOTFS_NAND_ONLY),y)
NORNAND_ENV=nand
else
NORNAND_ENV=nor
endif

ifeq ($(CONFIG_IMAGE_RELEASE),y)
ifeq ($(CONFIG_INITRAMFS_LINUX),y)
DEBUG_RELEASE_ENV=initramfs
else
DEBUG_RELEASE_ENV=release
endif
else
DEBUG_RELEASE_ENV=debug
endif

HUMAX_DEVICE_NOD_DIR=$(curdir)/../target_device

base-rootfs: rootfs-default rootfs-function rootfs-product generate-dev-node rootfs-unneeded
	@echo "---------------------------------------------------------------"
	@echo "Complete organizing base rootfs!!"
	@echo "---------------------------------------------------------------"

generate-dev-node:
ifneq ($(CONFIG_KERNEL_DEVTMPFS),y)
	@echo "---------------------------------------------------------------"
	@echo "Generating device nod"
	@echo "HUMAX_DEVICE_NOD_DIR : $(HUMAX_DEVICE_NOD_DIR)"
	@echo "CONFIG_PRODUCT_NAME : $(CONFIG_PRODUCT_NAME)"
	@echo "prefix : $(prefix)"
	@echo "---------------------------------------------------------------"
	@cp $(HUMAX_DEVICE_NOD_DIR)/device_table.txt $(prefix)/dev
	@-if [ -f "$(curdir)/product/$(CONFIG_PRODUCT_NAME)/device_table.txt" ] ; \
	then \
		echo "Overwriting device_table.txt..."; \
		cp $(curdir)/product/$(CONFIG_PRODUCT_NAME)/device_table.txt $(prefix)/dev; \
	fi;
	@$(HUMAX_DEVICE_NOD_DIR)/gendev.sh $(prefix)
	@$(HUMAX_DEVICE_NOD_DIR)/gendev-usb.sh $(prefix)
endif

##--------------------------------------------------------------------------------------
## default base-files
##--------------------------------------------------------------------------------------
rootfs-default:
	@echo "----------------------------------------------------------------"
	@echo "copying default base-rootfs files......"
	@echo "----------------------------------------------------------------"
	@-if [ -d "$(curdir)/base" ] ; then \
		 cp -fa $(curdir)/base/* $(prefix)/; \
	fi;

##--------------------------------------------------------------------------------------
## files which depends on function
##--------------------------------------------------------------------------------------
rootfs-function:
	@echo "----------------------------------------------------------------"
	@echo "copying files for function......"
	@echo "----------------------------------------------------------------"
ifeq ($(CONFIG_ROOTFS_PVR),y)
	@echo "copy files for PVR......"
	@-if [ -d "$(curdir)/pvr/base" ] ; then \
		 cp -fa $(curdir)/pvr/base/* $(prefix)/; \
	fi;
endif
ifeq ($(CONFIG_ROOTFS_TTY0),y)
	@echo "copy files for TTY0......"
	@-if [ -d "$(curdir)/tty/tty0" ] ; then \
		 cp -fa $(curdir)/tty/tty0/* $(prefix)/; \
	fi;
else
	@echo "copy files for TTY0(BASE)......"
	@-if [ -d "$(curdir)/tty/base" ] ; then \
		 cp -fa $(curdir)/tty/base/* $(prefix)/; \
	fi;
endif
ifeq ($(CONFIG_ROOTFS_ETH),y)
	@echo "copy files for Network......"
	@-if [ -d "$(curdir)/network/support" ] ; then \
		 cp -fa $(curdir)/network/support/* $(prefix)/; \
	fi;
endif
ifeq ($(CONFIG_ROOTFS_PLAYREADY),y)
	@echo "copy files for PLAYREADY......"
ifeq ($(CONFIG_ROOTFS_PLAYREADY_ROBUST),y)
	@-if [ -d "$(curdir)/product/$(CONFIG_PRODUCT_NAME)/playready" ] ; then \
		cp -fa $(curdir)/product/$(CONFIG_PRODUCT_NAME)/playready/$(DEBUG_RELEASE_ENV)/playready*.bin $(prefix)/usr/drm/; \
	fi;
else
	@-if [ -d "$(curdir)/product/$(CONFIG_PRODUCT_NAME)/playready" ] ; then \
		cp -fa $(curdir)/product/$(CONFIG_PRODUCT_NAME)/playready/$(DEBUG_RELEASE_ENV)/*.dat $(prefix)/usr/drm/; \
	fi;
endif
endif
ifeq ($(CONFIG_ROOTFS_MSDRM),y)
	@echo "copy files for MS DRM......"
	@-if [ -d "$(curdir)/msdrm/base" ] ; then \
		cp -fa $(curdir)/msdrm/base/* $(prefix)/; \
	fi;
endif
ifeq ($(CONFIG_IMAGE_RELEASE),y)
	@echo "copy files for Release......"
	@-if [ -d "$(curdir)/release/base" ] ; then \
		cp -fa $(curdir)/release/base/* $(prefix)/; \
	fi;
endif
ifeq ($(CONFIG_INITRAMFS_LINUX),y)
ifeq ($(CONFIG_IMAGE_RELEASE),y)
	@echo "copy files for Initramfs Release......"
	@-if [ -d "$(curdir)/initramfs/release/base" ] ; then \
		cp -fa $(curdir)/initramfs/release/base/* $(prefix)/; \
	fi;
else
	@echo "copy files for Initramfs Debug......"
	@-if [ -d "$(curdir)/initramfs/base" ] ; then \
		 cp -fa $(curdir)/initramfs/base/* $(prefix)/; \
	fi;
endif
	@echo "copy files for Network on initramfs......"
	@-if [ -d "$(curdir)/network/support" ] ; then \
		 cp -fa $(curdir)/network/support/* $(prefix)/; \
	fi;
endif
ifeq ($(CONFIG_ROOTFS_TELNETD),y)
	@echo "copy files for TELNETD(Network setting)......"
	@-if [ -d "$(curdir)/network/telnet" ] ; then \
		 cp -fa $(curdir)/network/telnet/* $(prefix)/; \
	fi;

	@-if [ -d "$(curdir)/network/no_ifup" ] ; then \
		 cp -fa $(curdir)/network/no_ifup/* $(prefix)/; \
	fi;
endif
ifeq ($(CONFIG_IPV6),y)
	@echo "copy files for IPV6......"
	@-if [ -d "$(curdir)/network/ipv6" ] ; then \
		 cp -fa $(curdir)/network/ipv6/* $(prefix)/; \
	fi;
endif
ifeq ($(CONFIG_PRODUCT_IMAGE_OCTO),y)
	@echo "copy files for OCTO1.0 auto-ip script......"
	@-if [ -d "$(curdir)/network/octo10_auto_ip" ] ; then \
		 cp -fa $(curdir)/network/octo10_auto_ip/* $(prefix)/; \
	fi;
endif
ifeq ($(CONFIG_CALYPSO),y)
	@echo "copy files for Opensslaa which calypso needs......"
	@-if [ -d "$(curdir)/openssl" ] ; then \
		if [ -d "$(curdir)/openssl/product/$(CONFIG_PRODUCT_NAME)" ] ; then \
			cp -fa $(curdir)/openssl/product/$(CONFIG_PRODUCT_NAME)/* $(curdir)/openssl/base/etc/pki/tls; \
		fi; \
		if [ -d "$(HUMAX_PACKAGE_DIR)/openssl/host_bin" -a ! -x "$(HUMAX_PACKAGE_DIR)/openssl/host_bin/c_rehash" ] ; then \
			echo "Add Execute Permission $(HUMAX_PACKAGE_DIR)/host_bin/c_rehash and $(HUMAX_PACKAGE_DIR)/host_bin/openssl..."; \
			chmod +x $(HUMAX_PACKAGE_DIR)/openssl/host_bin/c_rehash; \
			chmod +x $(HUMAX_PACKAGE_DIR)/openssl/host_bin/openssl; \
		fi; \
		if [ -f "$(HUMAX_PACKAGE_DIR)/openssl/host_bin/c_rehash" ] ; then \
			$(HUMAX_PACKAGE_DIR)/openssl/host_bin/c_rehash $(curdir)/openssl/base/etc/pki/tls; \
		fi; \
		cp -fa $(curdir)/openssl/base/* $(prefix)/; \
	fi;
	@echo "copy files for PLAYREADY which calypso needs on $(DEBUG_RELEASE_ENV)......"
	@-if [ -d "$(curdir)/product/$(CONFIG_PRODUCT_NAME)/playready" ] ; then \
		cp -fa $(curdir)/product/$(CONFIG_PRODUCT_NAME)/playready/$(DEBUG_RELEASE_ENV)/playready*.bin $(prefix)/usr/drm/; \
	fi;
endif
ifeq ($(CONFIG_MEDIA20),y)
	@echo "copy files for Openssl which media20 needs......"
	@-if [ -d "$(curdir)/openssl" ] ; then \
		if [ -d "$(curdir)/openssl/product/$(CONFIG_PRODUCT_NAME)" ] ; then \
			cp -fa $(curdir)/openssl/product/$(CONFIG_PRODUCT_NAME)/* $(curdir)/openssl/base/etc/pki/tls; \
		fi; \
		if [ -d "$(HUMAX_PACKAGE_DIR)/openssl/host_bin" -a ! -x "$(HUMAX_PACKAGE_DIR)/openssl/host_bin/c_rehash" ] ; then \
			echo "Add Execute Permission $(HUMAX_PACKAGE_DIR)/host_bin/c_rehash and $(HUMAX_PACKAGE_DIR)/host_bin/openssl..."; \
			chmod +x $(HUMAX_PACKAGE_DIR)/openssl/host_bin/c_rehash; \
			chmod +x $(HUMAX_PACKAGE_DIR)/openssl/host_bin/openssl; \
		fi; \
		if [ -f "$(HUMAX_PACKAGE_DIR)/openssl/host_bin/c_rehash" ] ; then \
			$(HUMAX_PACKAGE_DIR)/openssl/host_bin/c_rehash $(curdir)/openssl/base/etc/pki/tls; \
		fi; \
		cp -fa $(curdir)/openssl/base/* $(prefix)/; \
	fi;
endif
ifeq ($(CONFIG_MEDIA10),y)
	@# to avoid base-files/ subfolder becomes dirty, do not make hash directly here.
	@# instead, copy all files to $(prefix) and apply rehasing there. $(prefix) is temporary and will be removed at distclean.
	@echo "copy files for Openssl which media10 needs......"
	@-if [ -d "$(curdir)/openssl/base/" ] ; then \
		cp -fa $(curdir)/openssl/base/* $(prefix)/; \
	fi;
	@-if [ -d "$(curdir)/openssl/product/$(CONFIG_PRODUCT_NAME)" ] ; then \
		cp -fa $(curdir)/openssl/product/$(CONFIG_PRODUCT_NAME)/* $(prefix)/etc/pki/tls; \
	fi;
	@# apply re-hasing of root certificates. there is something to be noted:
	@#  - this openssl binary is for host (x86). 
	@#  - it's version should match with openssl of drr_package. (libcrypto.so and libssl.so)
	@#  - we are not sure where is openssl.cnf, so we use provided empty config file.
	@-if [ -f "$(HUMAX_PACKAGE_DIR)/openssl/host_bin/c_rehash" ] ; then \
		echo "rehashing root certificates......"; \
		if [ -d "$(HUMAX_PACKAGE_DIR)/openssl/host_bin" -a ! -x "$(HUMAX_PACKAGE_DIR)/openssl/host_bin/c_rehash" ] ; then \
			echo "Add Execute Permission $(HUMAX_PACKAGE_DIR)/host_bin/c_rehash and $(HUMAX_PACKAGE_DIR)/host_bin/openssl..."; \
			chmod +x $(HUMAX_PACKAGE_DIR)/openssl/host_bin/c_rehash; \
			chmod +x $(HUMAX_PACKAGE_DIR)/openssl/host_bin/openssl; \
		fi; \
		OPENSSL=$(HUMAX_PACKAGE_DIR)/openssl/host_bin/openssl OPENSSL_CONF=$(HUMAX_PACKAGE_DIR)/openssl/host_bin/openssl.cnf \
		$(HUMAX_PACKAGE_DIR)/openssl/host_bin/c_rehash $(prefix)/etc/pki/tls; \
	fi;
endif
ifeq ($(CONFIG_ROOTFS_NAND_ONLY),y)
	@echo "copy files for NAND $(DEBUG_RELEASE_ENV)......"
	@-if [ -d "$(curdir)/nand/$(DEBUG_RELEASE_ENV)/base" ] ; then \
		 cp -fa $(curdir)/nand/$(DEBUG_RELEASE_ENV)/base/* $(prefix)/; \
	fi;
endif
ifeq ($(CONFIG_ROOTFS_NOR_NAND),y)
	@echo "copy files for NOR+NAND $(DEBUG_RELEASE_ENV) Only......"
	@-if [ -d "$(curdir)/nornand/$(DEBUG_RELEASE_ENV)/base" ] ; then \
		 cp -fa $(curdir)/nornand/$(DEBUG_RELEASE_ENV)/base/* $(prefix)/; \
	fi;
endif
ifeq ($(CONFIG_KERNEL_DEVTMPFS),y)
	@echo "copy files for Kernel devtmpfs......"
	@-if [ -d "$(curdir)/mknod/base" ] ; then \
		 cp -fa $(curdir)/mknod/base/* $(prefix)/; \
	fi;
endif
ifeq ($(CONFIG_TOOLS_OPROFILE),y)
	@echo "copy files for Oprofile......"
	@-if [ -d "$(curdir)/oprofile/base" ] ; then \
		 cp -fa $(curdir)/oprofile/base/* $(prefix)/; \
	fi;
endif
ifeq ($(CONFIG_LXC),y)
	@echo "copy files for LXC......"
	@-if [ -d "$(curdir)/lxc/base" ] ; then \
		 cp -fa $(curdir)/lxc/base/* $(prefix)/; \
	fi;
endif
ifeq ($(CONFIG_ROOTFS_ONEIMAGE),y)
	@echo "copy files for Release-One Image......"
	@-if [ -d "$(curdir)/oneimage/base" ] ; then \
		 cp -fa $(curdir)/oneimage/base/* $(prefix)/; \
	fi;
endif
ifeq ($(CONFIG_PRODUCT_IMAGE_HWTEST),y)
ifeq ($(CONFIG_DIRECTFB_MASTER),y)
ifeq ($(CONFIG_IMAGE_RELEASE),y)
ifneq ($(CONFIG_CRC_AUTOTEST),y)
	@echo "Dfbmaster depend"
	@-if [ -d "$(curdir)/dfbmaster/base" ] ; then \
		 cp -fa $(curdir)/dfbmaster/base/* $(prefix)/; \
	fi;
endif	
endif
endif
endif
ifeq ($(CONFIG_ROOTFS_TELNETD),y)
ifeq ($(CONFIG_WIFI_SCRIPT_TEST),y)
	@echo "WIFI -TELENT test"
	@-if [ -d "$(curdir)/network/wifi_telnet_test" ] ; then \
		 cp -fa $(curdir)/network/wifi_telnet_test/* $(prefix)/; \
	fi;
endif
endif
ifeq ($(CONFIG_S3_WARM_SECURE_BOOT),y)
	@echo "copy files for S3 WARM SECURE BOOT......"
	@-if [ -d "$(curdir)/s3warm/base/" ] ; then \
		 cp -fa $(curdir)/s3warm/base/* $(prefix)/; \
	fi;
endif
ifneq (,$(findstring uclib,$(HUMAX_UCLIBC_OLD_DIR)))
else
ifneq ($(CONFIG_PRODUCT_NAME),"uhd2500")
	@echo "copy timezone files for glibc"
	@-if [ -d "$(curdir)/glibc/base" ] ; then \
		cp -fa $(curdir)/glibc/base/* $(prefix)/; \
	fi;
endif
endif
ifeq ($(CONFIG_WAYLAND),y)
	@echo "copy files for Wayland......"
	@-if [ -d "$(curdir)/wayland/base" ] ; then \
		 cp -fa $(curdir)/wayland/base/* $(prefix)/; \
	fi;
endif
##--------------------------------------------------------------------------------------
## files which depends on product
##--------------------------------------------------------------------------------------
BASEFILES_PRODUCT_FLAGS=CONFIG_PRODUCT_NAME=$(CONFIG_PRODUCT_NAME) \
						CONFIG_BOARD_REVISION=$(CONFIG_BOARD_REVISION) \
						DEBUG_RELEASE_ENV=$(DEBUG_RELEASE_ENV) \
						NORNAND_ENV=$(NORNAND_ENV) \
						prefix=$(prefix)
						
rootfs-product:
	@echo "----------------------------------------------------------------"
	@echo "copying files for product $(CONFIG_PRODUCT_NAME) ......"
	@echo "----------------------------------------------------------------"
	@-if [ -d "$(curdir)/product/$(CONFIG_PRODUCT_NAME)/base" ] ; then \
		cp -fa $(curdir)/product/$(CONFIG_PRODUCT_NAME)/base/* $(prefix)/; \
	fi;	
	@-if [ -d "$(curdir)/product/$(CONFIG_PRODUCT_NAME)/nornand/$(DEBUG_RELEASE_ENV)/base" ] ; then \
		 cp -fa $(curdir)/product/$(CONFIG_PRODUCT_NAME)/nornand/$(DEBUG_RELEASE_ENV)/base/* $(prefix)/; \
	fi;
	@-if [ -d "$(curdir)/product/$(CONFIG_PRODUCT_NAME)/$(NORNAND_ENV)/$(DEBUG_RELEASE_ENV)/base" ] ; then \
		 cp -fa $(curdir)/product/$(CONFIG_PRODUCT_NAME)/$(NORNAND_ENV)/$(DEBUG_RELEASE_ENV)/base/* $(prefix)/; \
	fi;
ifeq ($(CONFIG_PRODUCT_IMAGE_HWTEST),y)
	@-if [ -d "$(curdir)/product/$(CONFIG_PRODUCT_NAME)/hwtest/base" ] ; then \
		 cp -fa $(curdir)/product/$(CONFIG_PRODUCT_NAME)/hwtest/base/* $(prefix)/; \
	fi;
endif
ifeq ($(CONFIG_PRODUCT_IMAGE_FACTORY),y)
	@-if [ -d "$(curdir)/product/$(CONFIG_PRODUCT_NAME)/factory/base" ] ; then \
		 cp -fa $(curdir)/product/$(CONFIG_PRODUCT_NAME)/factory/base/* $(prefix)/; \
	fi;
endif
ifeq ($(CONFIG_PRODUCT_IMAGE_UPGRADER),y)
	@-if [ -d "$(curdir)/product/$(CONFIG_PRODUCT_NAME)/upgrader/base" ] ; then \
		 cp -fa $(curdir)/product/$(CONFIG_PRODUCT_NAME)/upgrader/base/* $(prefix)/; \
	fi;
endif

ifneq ($(CONFIG_IMAGE_RELEASE),y)
ifeq ($(CONFIG_ASAN),y)
ifeq ($(CONFIG_DIRECTFB_MASTER),y)
	@-echo "Change Address for Address Sanitizer"
	@-sudo sed -i 's/0x20000000/0x40000000/g' $(prefix)/etc/default/modules;
endif
endif
endif

	@-if [ -d "$(curdir)/product/$(CONFIG_PRODUCT_NAME)" ] ; then \
		$(HUMAX_MAKE) -C  $(curdir)/product/$(CONFIG_PRODUCT_NAME) $(BASEFILES_PRODUCT_FLAGS) $@; \
	fi;

##--------------------------------------------------------------------------------------
## Delete unneeded files
##--------------------------------------------------------------------------------------
BASEFILES_UNNEEDED_FLAGS=CONFIG_PRODUCT_NAME=$(CONFIG_PRODUCT_NAME) \
						CONFIG_BOARD_REVISION=$(CONFIG_BOARD_REVISION) \
						DEBUG_RELEASE_ENV=$(DEBUG_RELEASE_ENV) \
						NORNAND_ENV=$(NORNAND_ENV) \
						prefix=$(prefix)
rootfs-unneeded:
	@echo "----------------------------------------------------------------"
	@echo "removing unneeded files......"
	@echo "----------------------------------------------------------------"
ifeq ($(CONFIG_IMAGE_RELEASE)$(CONFIG_DEVELOP),y )
	@echo "Delete files for debugging"
ifneq ($(CONFIG_PRODUCT_IMAGE_HWTEST),y)
	rm -rf $(prefix)/usr/sbin/addr2line
endif
	rm -rf $(prefix)/usr/sbin/gdb
	rm -rf $(prefix)/usr/sbin/gdbserver
	rm -rf $(prefix)/usr/sbin/calltrace.sh
	rm -rf $(prefix)/usr/sbin/coredump.sh
endif
	@-if [ -d "$(curdir)/product/$(CONFIG_PRODUCT_NAME)" ] ; then \
		$(HUMAX_MAKE) -C  $(curdir)/product/$(CONFIG_PRODUCT_NAME) $(BASEFILES_UNNEEDED_FLAGS) $@; \
	fi;

#######################################################################################
## Library files
#######################################################################################
CP_LIBC = libc
CP_LIBC += strip-stdc

STRIP_FLAGS = --strip-debug --strip-unneeded

strip-stdc:
	@-if [ ! -f $(prefix)/lib/libstdc++.so ]; then \
		cp -fa $(libcs-strip) $(prefix)/lib/; \
		$(HUMAX_STRIP) $(STRIP_FLAGS) $(prefix)/lib/libstdc++.so; \
	fi;

libc:
	@-cp -fa $(libcs) $(prefix)/lib/

install: base-rootfs $(CP_LIBC)

.SILENT: libc strip-stdc
#need modify. created by script.
#need modify. created by script.
#need modify. created by script.
#need modify. created by script.
#need modify. created by script.
#need modify. created by script.
