
include $(HUMAX_WORK_DIR)/platform/sdk/driver/bcm_environment.config
include $(HUMAX_PORTING_MAKE_DIR)/HumaxToolset.mak
include $(HUMAX_PORTING_MAKE_DIR)/HumaxPlatformDef.mak
-include $(HUMAX_WORK_DIR)/packages/.config
ifeq ($(CONFIG_NXCLIENT),y)
include $(NEXUS_TOP)/nxclient/include/nxclient.inc
endif

HUMAX_FLASHMAP_H=$(FLASHMAP_CONFIG_DIR)/$(HUMAX_PRODUCT_BUILD_NAME)/$(HUMAX_BOARD_REVISION)/flash_map.h
ifeq ($(CONFIG_ARM), y)
#If toolchain >= 4.8, /arch/arm/mach-bcm/flash_map.h
#else ,/arch/arm/mach-brcmstb/flash_map.h
	HUMAX_FLASHMAP_KER_H=$(KERNEL_DIR)/arch/arm/mach-brcmstb/flash_map.h
ifeq ($(shell expr $(TOOLCHAIN_MAJOR) ">" 4),1)
	HUMAX_FLASHMAP_KER_H=$(KERNEL_DIR)/arch/arm/mach-bcm/flash_map.h
else
	ifeq ($(shell expr $(TOOLCHAIN_MAJOR) "==" 4),1)
		ifeq ($(shell expr $(TOOLCHAIN_MINOR) ">=" 8),1)
			HUMAX_FLASHMAP_KER_H=$(KERNEL_DIR)/arch/arm/mach-bcm/flash_map.h
		endif
	endif
endif	
else
	HUMAX_FLASHMAP_KER_H=$(KERNEL_DIR)/include/linux/flash_map.h
endif

HUMAX_I2C_H=$(DEVICE_CONFIG_DIR)/$(HUMAX_PRODUCT_BUILD_NAME)/$(HUMAX_BOARD_REVISION)/i2c_bus_index.h
ifeq ($(shell expr $(NEXUS_TREE) ">=" TREE_1250),1)
	HUMAX_I2C_DEV_H=$(NEXUS_TOP)/platforms/common/src/humax_i2c_bus_index.h
else
	HUMAX_I2C_DEV_H=$(NEXUS_TOP)/platforms/$(PLATFORM)/src/humax_i2c_bus_index.h
endif

HUMAX_PINMUX_C=$(DEVICE_CONFIG_DIR)/$(HUMAX_PRODUCT_BUILD_NAME)/$(HUMAX_BOARD_REVISION)/pinmux.c 
HUMAX_PINMUX_DEV_C=$(NEXUS_TOP)/platforms/$(PLATFORM)/src/humax_pinmux.c

HUMAX_PINMUX_H=$(DEVICE_CONFIG_DIR)/$(HUMAX_PRODUCT_BUILD_NAME)/$(HUMAX_BOARD_REVISION)/humax_pinmux.h 
HUMAX_PINMUX_DEV_H=$(NEXUS_TOP)/platforms/$(PLATFORM)/src/humax_pinmux.h

HUMAX_HDMI_INFO_H=$(DEVICE_CONFIG_DIR)/$(HUMAX_PRODUCT_BUILD_NAME)/$(HUMAX_BOARD_REVISION)/hdmi_info.h
ifeq ($(shell expr $(NEXUS_TREE) ">=" TREE_1250),1)
	HUMAX_HDMI_INFO_DEV_H=$(NEXUS_TOP)/modules/hdmi_output/src/humax_hdmi_info.h
else
	HUMAX_HDMI_INFO_DEV_H=$(NEXUS_TOP)/modules/hdmi_output/$(BCHP_CHIP)/src/humax_hdmi_info.h
endif

HUMAX_RTS_H=$(DEVICE_CONFIG_DIR)/$(HUMAX_PRODUCT_BUILD_NAME)/$(HUMAX_BOARD_REVISION)/humax_rts.h 
ifeq ($(shell expr $(NEXUS_TREE) ">=" TREE_1250),1)
	HUMAX_RTS_DEV_H=$(NEXUS_TOP)/platforms/common/src/humax_rts.h
else
	HUMAX_RTS_DEV_H=$(NEXUS_TOP)/platforms/$(PLATFORM)/src/humax_rts.h
endif

# nocs library
NOCS_TARGET = 
NOCS_TARGET_CLEAN =
ifeq ($(CONFIG_CAS_NA_NOCS_3X),y)
	NOCS_TARGET_CLEAN = clean
ifeq ($(shell expr $(NEXUS_SECURITY_TREE) ">=" SECURITY_TREE_1501),1)
	NOCS_DRIVER_PATH=$(NEXUS_TOP)/../rockford/applications/security
	NOCS_DRIVER_DIR=$(NOCS_DRIVER_PATH)/nagra/examples/csd_example
	NOCS_LIB_OBJ_PATH=$(NOCS_DRIVER_PATH)/nagra/nocs3/$(CONFIG_CAS_NA_NOCS_3X_VERSION)/os/obj.$(PLATFORM)
	NOCS_DRIVER_OBJ=$(NOCS_LIB_OBJ_PATH)/csd/csd.o
	NOCS_CERT_DRIVER_OBJ=$(NOCS_LIB_OBJ_PATH)/cert/ca_cert.o
	NOCS_CERT_DRIVER_OBJ += $(NOCS_LIB_OBJ_PATH)/dpt/ca_dpt.o
	NOCS_BRCM_DRIVER_OBJ := $(NOCS_DRIVER_PATH)/brcm/bcsd/obj.$(PLATFORM).nagra/bcsd/cavendor.o \
		 $(NOCS_DRIVER_PATH)/brcm/bcsd/obj.$(PLATFORM).nagra/bcsd/cavendor_nocs3.o \
		 $(NOCS_DRIVER_PATH)/brcm/bcsd/obj.$(PLATFORM).common/bcsd/bcsd_core.o \
		 $(NOCS_DRIVER_PATH)/brcm/bcsd/obj.$(PLATFORM).common/bcsd/bcsd_rng.o \
		 $(NOCS_DRIVER_PATH)/brcm/bcsd/obj.$(PLATFORM).common/bcsd/bcsd_rsa.o \
		 $(NOCS_DRIVER_PATH)/brcm/bcsd/obj.$(PLATFORM).common/bcsd/bcsd_sha.o \
		 $(NOCS_DRIVER_PATH)/brcm/bcsd/obj.$(PLATFORM).common/bcsd/bcsd_prod_otp.o \
		 $(NOCS_DRIVER_PATH)/brcm/bcsd/obj.$(PLATFORM).common/bcsd/security_m2m_util.o
	NOCS_SEC_DRIVER_OBJ=$(NOCS_LIB_OBJ_PATH)/sec/ca_sec.o
	NOCS_SEC_BRCM_DRIVER_OBJ := \
		 $(NOCS_DRIVER_PATH)/nagra/nocs_common/os/obj.$(PLATFORM)/session_manager/brcm_ca_session_manager.o \
		 $(NOCS_DRIVER_PATH)/nagra/nocs_common/os/obj.$(PLATFORM)/session_manager/brcm_scrambling_session_manager.o
else ifeq ($(shell expr $(NEXUS_SECURITY_TREE) "==" SECURITY_TREE_1401),1)
	ifeq ($(CONFIG_CAS_NA_NOCS_3X),y)
		NOCS_DRIVER_PATH=$(NEXUS_TOP)/../rockford/applications/security
		NOCS_DRIVER_DIR=$(NOCS_DRIVER_PATH)/nagra/examples/csd_example
		NOCS_LIB_OBJ_PATH=$(NOCS_DRIVER_PATH)/nagra/nocs3/$(CONFIG_CAS_NA_NOCS_3X_VERSION)/os/obj.$(PLATFORM)
		NOCS_DRIVER_OBJ=$(NOCS_LIB_OBJ_PATH)/csd/csd.o
		NOCS_CERT_DRIVER_OBJ=$(NOCS_LIB_OBJ_PATH)/cert/ca_cert.o
		NOCS_BRCM_DRIVER_OBJ := $(NOCS_DRIVER_PATH)/brcm/bcsd/obj.$(PLATFORM).nagra/bcsd/cavendor.o \
			 $(NOCS_DRIVER_PATH)/brcm/bcsd/obj.$(PLATFORM).nagra/bcsd/cavendor_nocs3.o \
			 $(NOCS_DRIVER_PATH)/brcm/bcsd/obj.$(PLATFORM).common/bcsd/bcsd_core.o \
			 $(NOCS_DRIVER_PATH)/brcm/bcsd/obj.$(PLATFORM).common/bcsd/bcsd_rng.o \
			 $(NOCS_DRIVER_PATH)/brcm/bcsd/obj.$(PLATFORM).common/bcsd/bcsd_rsa.o \
			 $(NOCS_DRIVER_PATH)/brcm/bcsd/obj.$(PLATFORM).common/bcsd/bcsd_sha.o \
			 $(NOCS_DRIVER_PATH)/brcm/bcsd/obj.$(PLATFORM).common/bcsd/bcsd_prod_otp.o \
			 $(NOCS_DRIVER_PATH)/brcm/bcsd/obj.$(PLATFORM).common/bcsd/security_m2m_util.o\
			 $(NOCS_DRIVER_PATH)/brcm/bcsd/obj.$(PLATFORM).common/bcsd/bcsd_sha_sw_tomcrypt.o
		NOCS_SEC_DRIVER_OBJ=$(NOCS_LIB_OBJ_PATH)/sec/ca_sec.o
		NOCS_CERT_DRIVER_OBJ += $(NOCS_LIB_OBJ_PATH)/dpt/ca_dpt.o
		NOCS_SEC_BRCM_DRIVER_OBJ := \
			 $(NOCS_DRIVER_PATH)/nagra/nocs_common/os/obj.$(PLATFORM)/session_manager/brcm_ca_session_manager.o \
			 $(NOCS_DRIVER_PATH)/nagra/nocs_common/os/obj.$(PLATFORM)/session_manager/brcm_scrambling_session_manager.o
	else
		NOCS_DRIVER_PATH=$(NEXUS_TOP)/../rockford/applications/nocs3/$(CONFIG_CAS_NA_NOCS_3X_VERSION)/os
		NOCS_LIB_OBJ_PATH=$(NEXUS_TOP)/../rockford/applications/nocs3/$(CONFIG_CAS_NA_NOCS_3X_VERSION)/os/obj.$(PLATFORM)
		############## NOCS 3X CSD driver #####################
		NOCS_DRIVER_DIR=$(NOCS_DRIVER_PATH)/csd
		NOCS_DRIVER_OBJ=$(NOCS_LIB_OBJ_PATH)/csd/csd.o
		NOCS_BRCM_DRIVER_DIR=$(NOCS_DRIVER_PATH)/nocscommon
		NOCS_BRCM_DRIVER_OBJ=$(NOCS_LIB_OBJ_PATH)/nocscommon/emitools.o
		############## NOCS 3X CERT driver ####################
		ifeq ($(CONFIG_CAS_NA_NOCS_CERT),y)
		NOCS_CERT_DRIVER_DIR=$(NOCS_DRIVER_PATH)/cert
		NOCS_CERT_DRIVER_OBJ=$(NOCS_LIB_OBJ_PATH)/cert/ca_cert.o
		endif
		############## NOCS 3X SEC driver #####################
		ifeq ($(CONFIG_CAS_NA_NOCS_SEC),y)
		NOCS_SEC_DRIVER_DIR=$(NOCS_DRIVER_PATH)/sec
		NOCS_SEC_DRIVER_OBJ=$(NOCS_LIB_OBJ_PATH)/sec/ca_sec.o
		NOCS_SEC_BRCM_DRIVER_OBJ=$(NOCS_LIB_OBJ_PATH)/sec/brcm_ca_session_manager.o
		endif
	endif
else
	NOCS_DRIVER_PATH=$(NEXUS_TOP)/../rockford/applications/nocs3/$(CONFIG_CAS_NA_NOCS_3X_VERSION)/os
	NOCS_LIB_OBJ_PATH=$(NEXUS_TOP)/../rockford/applications/nocs3/$(CONFIG_CAS_NA_NOCS_3X_VERSION)/os/obj.$(PLATFORM)
	############## NOCS 3X CSD driver #####################
	NOCS_DRIVER_DIR=$(NOCS_DRIVER_PATH)/csd
	NOCS_DRIVER_OBJ=$(NOCS_LIB_OBJ_PATH)/csd/csd.o
	NOCS_BRCM_DRIVER_DIR=$(NOCS_DRIVER_PATH)/nocscommon
	NOCS_BRCM_DRIVER_OBJ=$(NOCS_LIB_OBJ_PATH)/nocscommon/emitools.o
	############## NOCS 3X CERT driver ####################
	ifeq ($(CONFIG_CAS_NA_NOCS_CERT),y)
	NOCS_CERT_DRIVER_DIR=$(NOCS_DRIVER_PATH)/cert
	NOCS_CERT_DRIVER_OBJ=$(NOCS_LIB_OBJ_PATH)/cert/ca_cert.o
	endif
	############## NOCS 3X SEC driver #####################
	ifeq ($(CONFIG_CAS_NA_NOCS_SEC),y)
	NOCS_SEC_DRIVER_DIR=$(NOCS_DRIVER_PATH)/sec
	NOCS_SEC_DRIVER_OBJ=$(NOCS_LIB_OBJ_PATH)/sec/ca_sec.o
	NOCS_SEC_BRCM_DRIVER_OBJ=$(NOCS_LIB_OBJ_PATH)/sec/brcm_ca_session_manager.o
	endif
endif
else	# CONFIG_CAS_NA_NOCS_3X
	############## NOCS 1X CSD driver #####################
	NOCS_DRIVER_DIR=$(NEXUS_TOP)/../rockford/applications/nocs/$(BCHP_CHIP)
	NOCS_TARGET = static-lib
	NOCS_TARGET_CLEAN = static-lib-clean
endif

# bcsd library
BCSD_TARGET =
BCSD_TARGET_CLEAN =
ifeq ($(CONFIG_CAS_BCSD),y)
	BCSD_TARGET_CLEAN = clean
	BCSD_DRIVER_PATH=$(NEXUS_TOP)/../rockford/applications/security
	BCSD_DRIVER_DIR=$(BCSD_DRIVER_PATH)/conax/os
	BCSD_BRCM_DRIVER_OBJ := $(BCSD_DRIVER_PATH)/brcm/bcsd/obj.$(PLATFORM).conax/bcsd/cavendor.o \
		$(BCSD_DRIVER_PATH)/brcm/bcsd/obj.$(PLATFORM).common/bcsd/bcsd_core.o \
		$(BCSD_DRIVER_PATH)/brcm/bcsd/obj.$(PLATFORM).common/bcsd/bcsd_rng.o \
		$(BCSD_DRIVER_PATH)/brcm/bcsd/obj.$(PLATFORM).common/bcsd/bcsd_rsa.o \
		$(BCSD_DRIVER_PATH)/brcm/bcsd/obj.$(PLATFORM).common/bcsd/bcsd_sha.o
endif

#
# tuxera ntfs
#
ifeq ($(CONFIG_NTFS_RW_LIB),y)
NTFS_DRIVER_PATH = $(HUMAX_WORK_DIR)/platform/sdk/os/drivers/fs/ntfs
NTFS_DRIVER_PREPPARE_ROOT_DIR = $(HUMAX_PREPARE_ROOT_DIR)/lib/modules/drivers/fs/ntfs
endif


#
# register ctl driver
#
ifeq ($(CONFIG_DI_SHARED_LIB),y)
	BRCM_REGISTER_CTRL_DRIVER_PATH=$(HUMAX_WORK_DIR)/platform/sdk/os/drivers/nexus_driver
else ifeq ($(CONFIG_DI_MODULE_SHARED_LIB),y)
	BRCM_REGISTER_CTRL_DRIVER_PATH=$(HUMAX_WORK_DIR)/platform/sdk/os/drivers/nexus_driver
endif

#
# wireless devices
#
ifeq ($(CONFIG_WIRELESS),y)
	include $(HUMAX_PORTING_MAKE_DIR)/HumaxPlatformWireless.mak
else
	WIRELESS_INSTALL :=
endif

#
# external kernel modules
#
ifeq ($(CONFIG_RTL8105E),y)
BUILD_EXTERNAL_KERNEL_MODULES = y
endif

ifeq ($(BUILD_EXTERNAL_KERNEL_MODULES),y)
include $(HUMAX_PORTING_MAKE_DIR)/HumaxPlatformKernelModules.mak
endif


#
# link
#
HUMAX_LD_LIB_PATH = -L$(HUMAX_UCLIBC_OLD_DIR)/lib \
					-L$(HUMAX_OUTPUT_DIR)/lib \
					-L$(HUMAX_NFS_INSTALL_DIR)/lib \
					-L$(HUMAX_NFS_INSTALL_DIR)/usr/lib \
					-L$(HUMAX_WORK_DIR)/packages/libext2fs/lib

#
# target
#

ifeq ($(CONFIG_INITRAMFS_LINUX),y)
	TARGETS += kernel-prepare
	IGNORE += kernel
else
	TARGETS += kernel
endif

ifeq ($(CONFIG_WIRELESS),y)
TARGETS += $(WIRELESS_TARGETS)
endif

ifeq ($(CONFIG_NTFS_RW_LIB),y)
TARGETS += tuxera_ntfs
endif

ifeq ($(BUILD_EXTERNAL_KERNEL_MODULES),y)
TARGETS += $(EXTERNAL_MODULES)	# See HumaxPlatformKernelModules.mak
endif

TARGETS += rootfs device

ifeq ($(CONFIG_HDCP_22_PROVISIONING),y)
TARGETS += provisioning
else ifeq ($(CONFIG_KEY_PROVISIONING),y)
TARGETS += provisioning
endif

ifeq ($(CONFIG_BRCM_MOD_MSDRM_PRDY_25_SUPPORT),y)
TARGETS += playready25_brcm
ifeq ($(CONFIG_PRODUCT_IMAGE_HWTEST),y)
TARGETS += openssl_for_hwtest
endif
ifeq ($(CONFIG_PRODUCT_IMAGE_FACTORY),y)
TARGETS += openssl_for_factory
endif
endif

ifeq ($(CONFIG_PRODUCT_DEVICE_DEBUG_LOG),y)
DEVICE_DEBUG=y
else
DEVICE_DEBUG=n
endif

ifeq ($(CONFIG_CAS_NA),y)
	ifeq ($(shell expr $(NEXUS_SECURITY_TREE) "<" SECURITY_TREE_1501),1)
	ifneq ($(shell expr $(NEXUS_SECURITY_TREE) "==" SECURITY_TREE_1401),1)
		ifeq ($(CONFIG_CAS_NA_NOCS_3X),y)
			TARGETS += nocs-brcm
			ifeq ($(CONFIG_CAS_NA_NOCS_CERT),y)
				TARGETS += nocs-cert
			endif
			ifeq ($(CONFIG_CAS_NA_NOCS_SEC),y)
				TARGETS += nocs-sec
			endif
		endif
	endif
	endif
	TARGETS += nocs
endif

ifeq ($(CONFIG_CAS_BCSD),y)
	TARGETS += bcsd
endif

$(HUMAX_PINMUX_DEV_C): $(HUMAX_PINMUX_C)
	$(HUMAX_CP) $< $@

$(HUMAX_PINMUX_DEV_H): $(HUMAX_PINMUX_H)
	$(HUMAX_CP) $< $@

$(HUMAX_HDMI_INFO_DEV_H): $(HUMAX_HDMI_INFO_H)
	$(HUMAX_CP) $< $@

$(HUMAX_I2C_DEV_H): $(HUMAX_I2C_H)
	$(HUMAX_CP) $< $@

$(HUMAX_RTS_DEV_H): $(HUMAX_RTS_H)
	$(HUMAX_CP) $< $@

$(HUMAX_OUTPUT_DIR) :
	$(HUMAX_MKDIR) $(HUMAX_OUTPUT_DIR)/lib
	

ifeq ($(filter $(CONFIG_HDCP_22_PROVISIONING) $(CONFIG_KEY_PROVISIONING),y),y)
provisioning: $(HUMAX_OUTPUT_DIR) $(HUMAX_PINMUX_DEV_C) $(HUMAX_PINMUX_DEV_H) $(HUMAX_HDMI_INFO_DEV_H) $(HUMAX_I2C_DEV_H) $(WIRELESS_INSTALL) $(HUMAX_RTS_DEV_H)
ifeq ($(shell expr $(NEXUS_TREE) ">=" TREE_1501),1)
	$(HUMAX_MAKE) -C $(NEXUS_TOP)/../BSEAV/lib/security/sage/manufacturing/app $(NEXUS_CONFIG) SAGE_SUPPORT=y NEXUS_PLATFORM=$(PLATFORM)  re
else
	$(HUMAX_MAKE) -C $(NEXUS_TOP)/../BSEAV/lib/security/sage/manufacturing/provisioning $(NEXUS_CONFIG) SAGE_SUPPORT=y NEXUS_PLATFORM=$(PLATFORM)  re
endif
	$(HUMAX_CP) $(NEXUS_TOP)/bin/libsrai.so $(HUMAX_PREPARE_ROOT_DIR)/usr/lib
ifeq ($(CONFIG_BCM_SAGE_AUTO_FW),y)
	@test -d $(HUMAX_PREPARE_ROOT_DIR)/usr/sbin/provision || install -d -m 755 $(HUMAX_PREPARE_ROOT_DIR)/usr/sbin/provision
	$(HUMAX_CP) $(HUMAX_PREPARE_ROOT_DIR)/usr/sbin/sage*.bin $(HUMAX_PREPARE_ROOT_DIR)/usr/sbin/provision
	$(HUMAX_CP) $(PROVISIONING_TOOL_DIR)/$(HUMAX_BOARD_REVISION)/SAGESW*.bin $(HUMAX_PREPARE_ROOT_DIR)/usr/sbin/provision/sage_os_app.bin
	@test ! -f $(PROVISIONING_TOOL_DIR)/$(HUMAX_BOARD_REVISION)/SAGEBL*.bin || $(HUMAX_CP) $(PROVISIONING_TOOL_DIR)/$(HUMAX_BOARD_REVISION)/SAGEBL*.bin $(HUMAX_PREPARE_ROOT_DIR)/usr/sbin/provision/sage_bl.bin	
else
	$(HUMAX_CP) $(PROVISIONING_TOOL_DIR)/$(HUMAX_BOARD_REVISION)/SAGESW*.bin $(HUMAX_PREPARE_ROOT_DIR)/usr/sbin/sage_os_app.bin
	@test ! -f $(PROVISIONING_TOOL_DIR)/$(HUMAX_BOARD_REVISION)/SAGEBL*.bin || $(HUMAX_CP) $(PROVISIONING_TOOL_DIR)/$(HUMAX_BOARD_REVISION)/SAGEBL*.bin $(HUMAX_PREPARE_ROOT_DIR)/usr/sbin/sage_bl.bin	
endif	
#	$(HUMAX_CP) $(NEXUS_TOP)/bin/brcm_drm_binfile_provisioning_tool $(HUMAX_PREPARE_ROOT_DIR)/usr/sbin

provisioning-clean:
	-$(HUMAX_RM) $(NEXUS_TOP)/bin/libsrai.so
#	$(HUMAX_RM) $(NEXUS_TOP)/bin/brcm_drm_binfile_provisioning_tool

provisioning-distclean: provisioning-clean
endif

NXCLIENT_CONFIG := NXCLIENT_CFLAGS="$(NXCLIENT_CFLAGS)"
NXCLIENT_CONFIG += NXCLIENT_LDFLAGS="$(NXCLIENT_LDFLAGS)"
NXCLIENT_CONFIG += B_REFSW_ARCH="$(B_REFSW_ARCH)"
NXCLIENT_CONFIG += BCHP_VER="$(BCHP_VER)"
NXCLIENT_CONFIG += NEXUS_PLATFORM="$(PLATFORM)"
ifeq ($(CONFIG_DEVICE_MODE_KERNEL),y)
#default is client
NXCLIENT_CONFIG += NEXUS_MODE=proxy
endif

device: $(HUMAX_OUTPUT_DIR) $(HUMAX_PINMUX_DEV_C) $(HUMAX_PINMUX_DEV_H) $(HUMAX_HDMI_INFO_DEV_H) $(HUMAX_I2C_DEV_H) $(WIRELESS_INSTALL) $(HUMAX_RTS_DEV_H)
	@test -d $(HUMAX_PREPARE_ROOT_DIR)/usr/lib || install -d -m 755 $(HUMAX_PREPARE_ROOT_DIR)/usr/lib
ifeq ($(CONFIG_DEVICE_MODE_KERNEL),y)	
	$(HUMAX_MAKE) -C $(NEXUS_TOP)/build $(HUMAX_MULTI_JOBS) DEBUG=$(DEVICE_DEBUG) $(NEXUS_CONFIG)
else
	$(HUMAX_MAKE) -C $(NEXUS_TOP)/build $(HUMAX_MULTI_JOBS) DEBUG=$(DEVICE_DEBUG) $(NEXUS_CONFIG) nexus_install
endif	
ifeq ($(CONFIG_DEVICE_MODE_USER),y)
ifeq ($(CONFIG_PRODUCT_IMAGE_OCTO),y)
else ifeq ($(CONFIG_PRODUCT_IMAGE_UOCTO),y)
else ifeq ($(CONFIG_PRODUCT_IMAGE_UPGRADER),y)
	$(HUMAX_MAKE) -C $(NEXUS_TOP)/build $(HUMAX_MULTI_JOBS) DEBUG=$(DEVICE_DEBUG) $(NEXUS_CONFIG) NEXUS_MODE=client nexus_install
else
	$(HUMAX_MAKE) -C $(NEXUS_TOP)/build $(HUMAX_MULTI_JOBS) DEBUG=$(DEVICE_DEBUG) $(NEXUS_CONFIG) NEXUS_MODE=client nexus_install
endif
endif
ifeq ($(CONFIG_DEVICE_MODE_USER),y)
	$(HUMAX_MAKE) -C $(NEXUS_UMDRIVER_DIR) DEBUG=$(DEVICE_DEBUG) $(NEXUS_CONFIG) all
	-$(HUMAX_CP) $(NEXUS_MOD) $(NEXUS_TOP)/bin
endif

ifeq ($(CONFIG_NXCLIENT),y)
	$(HUMAX_MAKE) -C $(NEXUS_TOP)/nxclient/build $(NXCLIENT_CONFIG)
	$(HUMAX_CP) $(NEXUS_MOD) $(NEXUS_TOP)/bin/libnxclient.so $(HUMAX_PREPARE_ROOT_DIR)/usr/lib
	$(HUMAX_CP) $(NEXUS_MOD) $(NEXUS_TOP)/bin/libnxclient_local.so $(HUMAX_PREPARE_ROOT_DIR)/usr/lib
endif

ifeq ($(CONFIG_DEVICE_LIB_STATIC),y)
	$(HUMAX_CP) $(NEXUS_LIB) $(HUMAX_OUTPUT_DIR)/lib
else
ifneq ($(DEVICE_DEBUG),y)
	$(HUMAX_STRIP) --strip-debug --strip-unneeded $(NEXUS_LIB)
endif
	$(HUMAX_CP) $(NEXUS_LIB) $(HUMAX_PREPARE_ROOT_DIR)/usr/lib
ifeq ($(CONFIG_HDCP_22),y)
	$(HUMAX_CP) $(NEXUS_TOP)/bin/sage*.bin $(HUMAX_PREPARE_ROOT_DIR)/usr/sbin
	@test ! -f $(SAGESW_MAINAPP_DIR)/$(HUMAX_BOARD_REVISION)/SAGESW*.bin || $(HUMAX_CP) $(SAGESW_MAINAPP_DIR)/$(HUMAX_BOARD_REVISION)/SAGESW*.bin $(HUMAX_PREPARE_ROOT_DIR)/usr/sbin/sage_os_app.bin
	@test ! -f $(SAGESW_MAINAPP_DIR)/$(HUMAX_BOARD_REVISION)/SAGEBL*.bin || $(HUMAX_CP) $(SAGESW_MAINAPP_DIR)/$(HUMAX_BOARD_REVISION)/SAGEBL*.bin $(HUMAX_PREPARE_ROOT_DIR)/usr/sbin/sage_bl.bin

endif
endif
	@test -d $(HUMAX_PREPARE_ROOT_DIR)/$(NEXUS_MOD_PREFIX) || install -d -m 755 $(HUMAX_PREPARE_ROOT_DIR)/$(NEXUS_MOD_PREFIX)
	@install -m 644 $(NEXUS_MOD) $(HUMAX_PREPARE_ROOT_DIR)/$(NEXUS_MOD_PREFIX)

	perl $(HUMAX_TOOL_DIR)/depmod/depmod.pl -b $(HUMAX_PREPARE_ROOT_DIR)/lib/modules/$(KERNELRELEASE) -k $(KERNEL_DIR)/vmlinux

device-clean:
	$(HUMAX_MAKE) -C $(NEXUS_TOP)/build DEBUG=$(DEVICE_DEBUG) $(NEXUS_CONFIG) clean
	$(HUMAX_RM) $(NEXUS_TOP)/bin/*nexus*
	-$(HUMAX_RM) $(NEXUS_TOP)/platforms/$(PLATFORM)/bin
	$(HUMAX_RM) $(HUMAX_PINMUX_DEV_C) $(HUMAX_PINMUX_DEV_H) $(HUMAX_HDMI_INFO_DEV_H) $(HUMAX_I2C_DEV_H) $(HUMAX_RTS_DEV_H)

device-distclean: device-clean

################### nexus examples folder compile ####################
nexus-examples: 
	$(HUMAX_MAKE) -C $(NEXUS_TOP)/examples DEBUG=$(DEVICE_DEBUG) $(NEXUS_CONFIG) all install
	
nexus-examples-clean:
	$(HUMAX_MAKE) -C $(NEXUS_TOP)/examples DEBUG=$(DEVICE_DEBUG) $(NEXUS_CONFIG) clean

bmemconfig-examples: 
	$(HUMAX_MAKE) -C $(NEXUS_TOP)/../BSEAV/tools/bmemconfig DEBUG=$(DEVICE_DEBUG) $(NEXUS_CONFIG) all install
	
bmemconfig-examples-clean:
	$(HUMAX_MAKE) -C $(NEXUS_TOP)/examples DEBUG=$(DEVICE_DEBUG) $(NEXUS_CONFIG) clean

printindex:
	$(HUMAX_MAKE) -C $(NEXUS_TOP)/../BSEAV/lib/bcmplayer/utils DEBUG=$(DEVICE_DEBUG) $(NEXUS_CONFIG) printindex

################### bsu compile ####################
bsu:  $(HUMAX_OUTPUT_DIR) $(HUMAX_PINMUX_DEV_C) $(HUMAX_PINMUX_DEV_H) $(HUMAX_HDMI_INFO_DEV_H) $(HUMAX_I2C_DEV_H) $(HUMAX_RTS_DEV_H)
	export PATH=$(CONFIG_BSU_TOOLCHAINS)/bin:/bin:/sbin:/usr/bin:/usr/sbin;\
	$(HUMAX_MAKE) -C $(NEXUS_TOP)/../BSEAV/app/bsu/build $(NEXUS_CONFIG) $(NEXUS_BSU_CONFIG) BUILD=test

bsu-clean:
	$(HUMAX_MAKE) -C $(NEXUS_TOP)/../BSEAV/app/bsu/build $(NEXUS_CONFIG) $(NEXUS_BSU_CONFIG) clean
	-$(HUMAX_RM) $(NEXUS_TOP)/bin/*nexus*
	-$(HUMAX_RM) $(NEXUS_TOP)/platforms/$(PLATFORM)/bin
	-$(HUMAX_RM) $(NEXUS_TOP)/../BSEAV/app/bsu/build/*.debug
	-$(HUMAX_RM) $(NEXUS_TOP)/../BSEAV/app/bsu/build/*.release
	-$(HUMAX_RM) $(HUMAX_PINMUX_DEV_C) $(HUMAX_PINMUX_DEV_H) $(HUMAX_HDMI_INFO_DEV_H) $(HUMAX_I2C_DEV_H) $(HUMAX_RTS_DEV_H)

bsu-distclean: bsu-clean

################### bootloader compile (BOLT/CFE) ###################

HUMAX_BOOTLOADER_DATE_FILE := $(HUMAX_PRODUCT_CONFIG_DIR)/bootloader/build_date.c

HUMAX_BOOTLOADER_BUILDYEAR := $(shell date +"%Y")
HUMAX_BOOTLOADER_BUILDDATE := $(shell date +"%Y-%m-%d %H:%M:%S")
HUMAX_BOOTLOADER_BUILDUSER := $(shell whoami)
HUMAX_BOOTLOADER_BUILDHOST := $(shell hostname)
HUMAX_BOOTLOADER_BUILDMODEL := $(call plaintext,$(CONFIG_PRODUCT_NAME))

bootloader-builddate:
	$(HUMAX_RM) -f $(HUMAX_BOOTLOADER_DATE_FILE)
	$(HUMAX_ECHO) "const int buildyear = $(HUMAX_BOOTLOADER_BUILDYEAR);" > $(HUMAX_BOOTLOADER_DATE_FILE)
	$(HUMAX_ECHO) "const char builddate[] = \"$(HUMAX_BOOTLOADER_BUILDDATE)\";" >> $(HUMAX_BOOTLOADER_DATE_FILE)
	$(HUMAX_ECHO) "const char builduser[] = \"$(HUMAX_BOOTLOADER_BUILDUSER)@$(HUMAX_BOOTLOADER_BUILDHOST)\";" >> $(HUMAX_BOOTLOADER_DATE_FILE)
	$(HUMAX_ECHO) "const char buildtag[] = \"$(HUMAX_BOOTLOADER_BUILDTAG)\";" >> $(HUMAX_BOOTLOADER_DATE_FILE)
	$(HUMAX_ECHO) "" >> $(HUMAX_BOOTLOADER_DATE_FILE)
	
bootloader: bootloader-builddate	
	$(HUMAX_ECHO) "BOOTLOADER CFG = $(BOOTLOADER_CONFIG)"
	$(HUMAX_ECHO) "BOOTLOADER BUILD COMMAND = $(BOOTLOADER_BUILD_COMMAND)"
	$(HUMAX_ECHO) "Toolchains : $(CONFIG_BOOLOADER_TOOLCHAINS)"

	@cp $(HUMAX_WORK_DIR)/product_config/flashmap/$(CONFIG_PRODUCT_NAME)/$(CONFIG_BOARD_REVISION)/flash_map.h $(HUMAX_WORK_DIR)/product_config/bootloader/flash_map.h
ifeq ($(CONFIG_MIPS), y)
	@cp $(HUMAX_WORK_DIR)/product_config/flashmap/$(CONFIG_PRODUCT_NAME)/$(CONFIG_BOARD_REVISION)/flash_map.h $(HUMAX_WORK_DIR)/platform/sdk/driver/CFE/cfe/arch/include/flash_map.h
endif

	export PATH=$(CONFIG_BOOLOADER_TOOLCHAINS)/bin:/bin:/sbin:/usr/bin:/usr/sbin; \
	$(HUMAX_MAKE) -C $(NEXUS_TOP)/../$(BOOTLOADER_TARGET) $(HUMAX_MULTI_JOBS) $(BOOTLOADER_BUILD_COMMAND) $(BOOTLOADER_CONFIG) SECURE_BOOT=$(SECURE_BOOT) HUMAX_PLATFORM_BASE=y

bootloader-clean:
	$(HUMAX_MAKE) -C $(NEXUS_TOP)/../$(BOOTLOADER_TARGET) $(HUMAX_MULTI_JOBS) clean

bootloader-distclean: bootloader-clean


################### NAGRA NOCS driver ####################
NEXUS_CONFIG += NEXUS_TOP=$(NEXUS_TOP)
NEXUS_CONFIG += CFG_BIG_ENDIAN=0
NEXUS_CONFIG += NEXUS_PLATFORM=$(PLATFORM)
NEXUS_CONFIG += NEXUS_VERSION=$(NEXUS_VERSION)
ifeq ($(CONFIG_CAS_NA_NOCS_SEC),y)
NEXUS_CONFIG += TOMCRYPT_INC=$(HUMAX_PACKAGE_MAKE_DIR)/libtomcrypt/usr/include \
		TOMMATH_INC=$(HUMAX_PACKAGE_MAKE_DIR)/libtommath/usr/include \
		OPENSSL_INC=$(HUMAX_PACKAGE_MAKE_DIR)/openssl/include
endif

define nocs-driver
	$(HUMAX_MAKE) -C $(1) $(2)
endef

nocs:
	$(call nocs-driver,$(NOCS_DRIVER_DIR) $(NEXUS_CONFIG), $(NOCS_TARGET))

ifeq ($(CONFIG_CAS_NA_NOCS_3X),y)
	@echo "[Create... NOCS3 statis library]"
	@ar rcs $(NEXUS_NOCS_LIB) $(NOCS_DRIVER_OBJ) $(NOCS_BRCM_DRIVER_OBJ) $(NOCS_CERT_DRIVER_OBJ) $(NOCS_SEC_DRIVER_OBJ) $(NOCS_SEC_BRCM_DRIVER_OBJ)
endif

ifneq ($(DEVICE_DEBUG),y)
	$(HUMAX_STRIP) --strip-debug --strip-unneeded $(NEXUS_NOCS_LIB)
endif
	@$(HUMAX_CP) $(NEXUS_NOCS_LIB)  $(HUMAX_OUTPUT_DIR)/lib

nocs-clean:
	$(call nocs-driver,$(NOCS_DRIVER_DIR) $(NEXUS_CONFIG), $(NOCS_TARGET_CLEAN))
	@$(HUMAX_RM) $(NEXUS_NOCS_LIB)

nocs-distclean: nocs-clean

################### NAGRA NOCS3 CERT driver ################
nocs-cert:
	$(call nocs-driver,$(NOCS_CERT_DRIVER_DIR) $(NEXUS_CONFIG), $(NOCS_TARGET))

nocs-cert-clean:
	$(call nocs-driver,$(NOCS_CERT_DRIVER_DIR) $(NEXUS_CONFIG), $(NOCS_TARGET_CLEAN))

nocs-cert-distclean: nocs-cert-clean

################### NAGRA NOCS3 SEC driver #################
nocs-sec: 
	$(call nocs-driver,$(NOCS_SEC_DRIVER_DIR) $(NEXUS_CONFIG), $(NOCS_TARGET))

nocs-sec-clean:
	$(call nocs-driver,$(NOCS_SEC_DRIVER_DIR) $(NEXUS_CONFIG), $(NOCS_TARGET_CLEAN))

nocs-sec-distclean: nocs-sec-clean

################### NAGRA NOCS3 Brcm driver #################
nocs-brcm: 
	$(call nocs-driver,$(NOCS_BRCM_DRIVER_DIR) $(NEXUS_CONFIG), $(NOCS_TARGET))

nocs-brcm-clean:
	$(call nocs-driver,$(NOCS_BRCM_DRIVER_DIR) $(NEXUS_CONFIG), $(NOCS_TARGET_CLEAN))

nocs-brcm-distclean: nocs-brcm-clean

################### BCSD driver ####################
NEXUS_CONFIG += NEXUS_TOP=$(NEXUS_TOP)
NEXUS_CONFIG += CFG_BIG_ENDIAN=0
NEXUS_CONFIG += NEXUS_PLATFORM=$(PLATFORM)
NEXUS_CONFIG += NEXUS_VERSION=$(NEXUS_VERSION)
NEXUS_CONFIG += TOMCRYPT_INC=$(HUMAX_PACKAGE_MAKE_DIR)/libtomcrypt/usr/include \
		TOMMATH_INC=$(HUMAX_PACKAGE_MAKE_DIR)/libtommath/usr/include \
		OPENSSL_INC=$(HUMAX_PACKAGE_MAKE_DIR)/openssl/include

define bcsd-driver
	$(HUMAX_MAKE) -C $(1) $(2)
endef

bcsd:
	@echo "[Build... BCSD statis library]"
	$(call bcsd-driver,$(BCSD_DRIVER_DIR) $(NEXUS_CONFIG), $(BCSD_TARGET))

ifeq ($(CONFIG_CAS_BCSD),y)
	@echo "[Create... BCSD statis library]"
	@ar rcs $(NEXUS_BCSD_LIB) $(BCSD_BRCM_DRIVER_OBJ)
endif

ifneq ($(DEVICE_DEBUG),y)
	$(HUMAX_STRIP) --strip-debug --strip-unneeded $(NEXUS_BCSD_LIB)
endif
	@$(HUMAX_CP) $(NEXUS_BCSD_LIB)  $(HUMAX_OUTPUT_DIR)/lib

bcsd-clean:
	$(call bcsd-driver,$(BCSD_DRIVER_DIR) $(NEXUS_CONFIG), $(BCSD_TARGET_CLEAN))
	@$(HUMAX_RM) $(NEXUS_BCSD_LIB)

bcsd-distclean: bcsd-clean

########### Add libraries #############
ifdef CONFIG_PRODUCT_IMAGE_DEBUG
HUMAX_LIB_EXTRA_LIST += -lssp
else ifdef CONFIG_PRODUCT_IMAGE_HWTEST
HUMAX_LIB_EXTRA_LIST += -lssp
else ifdef CONFIG_PRODUCT_DI_DEBUG_LOG
HUMAX_LIB_EXTRA_LIST += -lssp
else ifdef CONFIG_PRODUCT_APP_DEBUG_LOG
HUMAX_LIB_EXTRA_LIST += -lssp
endif

ifdef CONFIG_EXT
HUMAX_LIB_EXTRA_LIST += -lblkid -lcom_err -le2p -lss -luuid -lext2fs
HUMAX_LIB_EXTRA_PATH += 
endif

ifdef CONFIG_OPENSSL
HUMAX_LIB_EXTRA_LIST += -lssl -lcrypto
HUMAX_LIB_EXTRA_PATH += 
endif

ifeq ($(CONFIG_CAS_NA),y)
HUMAX_LIB_EXTRA_LIST += -lnocs
ifeq ($(CONFIG_CAS_NA_NOCS_SEC),y)
HUMAX_LIB_EXTRA_LIST += -lcrypto -lssl -ltommath -ltomcrypt
endif
endif

ifeq ($(CONFIG_CAS_BCSD),y)
HUMAX_LIB_EXTRA_LIST += -lbcsd -lssl -ltommath -ltomcrypt
endif

ifneq ($(CONFIG_PRODUCT_IMAGE_FACTORY),)
else
ifeq ($(CONFIG_DIRECTFB),y)
HUMAX_LIB_EXTRA_LIST += -ldirectfb -lfusion -ldirect -ldl -lz -lstdc++
ifeq ($(CONFIG_DEVICE_MODE_USER),y)
ifeq ($(CONFIG_DIRECTFB_MASTER),y)
HUMAX_LIB_EXTRA_LIST += -linit_client 
else
HUMAX_LIB_EXTRA_LIST += -linit
endif
else
HUMAX_LIB_EXTRA_LIST += -linit
endif
HUMAX_LIB_EXTRA_PATH += -L$(DIRECTFB_TARGET_DIR)/usr/lib
endif
endif

ifdef CONFIG_MEDIA20
HUMAX_LIB_EXTRA_LIST += -lcrypto -lssl
HUMAX_LIB_EXTRA_LIST += -lmxfile -lmxdtcp -lmxkernel -lmxdlna_core
endif

ifdef CONFIG_LIBXML2
HUMAX_LIB_EXTRA_LIST += -lxml2 -lz
HUMAX_LIB_EXTRA_PATH +=
endif

ifdef CONFIG_VIEWRIGHTWEB
HUMAX_LIB_EXTRA_LIST += -lViewRightWebClient -lViewRightWebWrapper -lstdc++
HUMAX_LIB_EXTRA_PATH +=
endif

ifdef CONFIG_MARLIN_DRM
HUMAX_LIB_EXTRA_LIST += -lWasabi -lSkb -lMarlinWrapper -lstdc++
HUMAX_LIB_EXTRA_PATH +=
endif

#######################################

######## Add Main libraries #########
HUMAX_LIB_LIST = -ldriver 
ifeq ($(CONFIG_DEVICE_MODE_USER),y)
ifeq ($(CONFIG_DIRECTFB_MASTER),y)
HUMAX_LIB_LIST += -lnexus_client
else
HUMAX_LIB_LIST += -lnexus
endif
else
HUMAX_LIB_LIST += -lnexus
endif
HUMAX_LIB_LIST += -lmain
HUMAX_LIB_LIST += -lvkernel
HUMAX_LIB_LIST += -lm
HUMAX_LIB_LIST += -ldl
ifeq ($(CONFIG_DI_SHARED_LIB),y)
HUMAX_LIB_LIST += -ldi
endif
ifeq ($(CONFIG_DRV_MODULE_HOTPLUG_SHARED_LIB),y)
HUMAX_LIB_LIST += -ldrv_hotplug
endif
ifeq ($(CONFIG_DI_MODULE_NETWORK_SHARED_LIB),y)
HUMAX_LIB_LIST += -ldi_network
endif
ifeq ($(CONFIG_DI_MODULE_NVRAM_SHARED_LIB),y)
HUMAX_LIB_LIST += -ldi_nvram
endif
ifeq ($(CONFIG_DI_MODULE_FLASH_SHARED_LIB),y)
HUMAX_LIB_LIST += -ldi_flash
endif
ifeq ($(CONFIG_DI_MODULE_WIFI_SHARED_LIB),y)
HUMAX_LIB_LIST += -ldi_wifi
endif
ifeq ($(CONFIG_DI_MODULE_BT_SHARED_LIB),y)
HUMAX_LIB_LIST += -ldi_bt
endif
ifeq ($(CONFIG_DI_MODULE_PPPOE_SHARED_LIB),y)
HUMAX_LIB_LIST += -ldi_pppoe
endif
ifeq ($(CONFIG_DI_MODULE_FS_SHARED_LIB),y)
HUMAX_LIB_LIST += -ldi_fs
endif
ifeq ($(CONFIG_DI_MODULE_CRYPT_SHARED_LIB),y)
HUMAX_LIB_LIST += -ldi_crypt
endif
ifeq ($(CONFIG_DI_MODULE_MMC_SHARED_LIB),y)
HUMAX_LIB_LIST += -ldi_mmc
endif
ifeq ($(CONFIG_DI_MODULE_HDD_SHARED_LIB),y)
HUMAX_LIB_LIST += -ldi_hdd
endif
ifeq ($(CONFIG_DI_MODULE_USB_SHARED_LIB),y)
HUMAX_LIB_LIST += -ldi_usb
endif
ifeq ($(CONFIG_DI_SERVER_CLIENT),y)
HUMAX_LIB_LIST += -ldi_power_client
endif
ifeq ($(CONFIG_HDCP_22_PROVISIONING),y)
HUMAX_LIB_LIST += -lsrai
else ifeq ($(CONFIG_KEY_PROVISIONING),y)
HUMAX_LIB_LIST += -lsrai
endif
ifeq ($(CONFIG_PRODUCT_IMAGE_OCTO),y)
HUMAX_LIB_LIST += -lapp -lssp
HUMAX_LIB_LIST += $(subst LDFLAGS = ,,$(shell cat $(HUMAX_OUTPUT_DIR)/lib/app.ld))
endif

ifeq ($(CONFIG_PRODUCT_IMAGE_HWTEST),y)
HUMAX_LIB_LIST += -lhwtest
endif

ifdef CONFIG_PRODUCT_IMAGE_FACTORY
HUMAX_LIB_LIST += -lfactory
endif

ifdef CONFIG_PRODUCT_MULTI_PROCESS
HUMAX_LIB_LIST += -lpcd -lipc
endif

ifdef CONFIG_PRODUCT_IMAGE_UOCTO
HUMAX_LIB_LIST += -lapp
endif

ifdef CONFIG_PRODUCT_IMAGE_UPGRADER
HUMAX_LIB_LIST += -lupgrader
endif

ifeq ($(CONFIG_PRODUCT_IMAGE_DEBUG),y)
ifdef CONFIG_ASAN
HUMAX_LIB_LIST += -lasan
endif
endif
################# for calypso #################
ifeq ($(CONFIG_PRODUCT_IMAGE_HWTEST),y)
ifdef CONFIG_CALYPSO
HUMAX_LIB_LIST += -lmi
ifneq ($(CONFIG_BRCM_MOD_MSDRM_PRDY_SUPPORT),y)
HUMAX_LIB_LIST += -lprdy
else
ifneq ($(CONFIG_MEDIA20),y)
HUMAX_LIB_LIST += -lprdy
endif
endif
HUMAX_LIB_LIST += -lcalypso 
HUMAX_LIB_LIST += -lViewRightWebClient -lViewRightWebWrapper -lstdc++
HUMAX_LIB_LIST += -lavformat
HUMAX_LIB_LIST += -lavcodec
HUMAX_LIB_LIST += -lavfilter
HUMAX_LIB_LIST += -lavutil
HUMAX_LIB_LIST += -lswresample
HUMAX_LIB_LIST += -lswscale
endif
endif

ifdef CONFIG_DINETWORK
ifdef CONFIG_BLUETOOTH
HUMAX_LIB_LIST += -lbsa -lrt
endif
endif

################# for Playready2.5 BRCM #################
ifeq ($(CONFIG_PRODUCT_IMAGE_HWTEST),y)
ifeq ($(CONFIG_BRCM_MOD_MSDRM_PRDY_25_SUPPORT),y)
HUMAX_LIB_LIST += -lcmndrm_tl -lcmndrm -lcmndrmprdy -ldrmrootfs -lsrai -lbcrypt -lplayreadypk_host

HUMAX_LIB_LIST += -lssl -lcrypto
endif
endif

ifeq ($(CONFIG_PRODUCT_IMAGE_FACTORY),y)
ifeq ($(CONFIG_BRCM_MOD_MSDRM_PRDY_25_SUPPORT),y)
HUMAX_LIB_LIST += -lcmndrm_tl -lcmndrm -lcmndrmprdy -ldrmrootfs -lsrai -lbcrypt -lplayreadypk_host

HUMAX_LIB_LIST += -lssl -lcrypto
endif
endif
#########################################################

################# for NetApp #################
ifeq ($(CONFIG_PRODUCT_IMAGE_HWTEST), y) #TODO
ifeq ($(CONFIG_NETAPP),y)
ifneq ($(wildcard ${NETAPP_OUTPUT_DIR}/include/netapp.inc),)
include ${NETAPP_OUTPUT_DIR}/include/netapp.inc
HUMAX_LIB_LIST += ${NETAPP_LDFLAGS}
endif

ifeq ($(CONFIG_DEVICE_PRIVILEGE),y)
HUMAX_LIB_LIST += -lcap -lattr
endif

endif#End of $(CONFIG_NETAPP)
endif
#######################################

######## Add Main libraries #########
HUMAX_LIB_DFBM_LIST = -ldriver
HUMAX_LIB_DFBM_LIST += -lnexus
HUMAX_LIB_DFBM_LIST += -lvkernel
HUMAX_LIB_DFBM_LIST += -lm -lrt

HUMAX_LIB_DFBM_LIST += $(subst -linit_client,,$(HUMAX_LIB_EXTRA_LIST))
HUMAX_LIB_DFBM_LIST += -linit

ifeq ($(CONFIG_NXCLIENT),y)
HUMAX_LIB_DFBM_LIST += -lnxclient
endif

ifeq ($(CONFIG_DI_SERVER_CLIENT),y)
HUMAX_LIB_DFBM_LIST += -ldi_power_server
endif

ifeq ($(CONFIG_DEVICE_PRIVILEGE),y)
HUMAX_LIB_DFBM_LIST += -lcap -lattr
endif

HUMAX_LIB_DFBM_PATH += -L$(HUMAX_PREPARE_ROOT_DIR)/lib -L$(HUMAX_PREPARE_ROOT_DIR)/usr/lib
HUMAX_LIB_DFBM_PATH += $(HUMAX_LIB_EXTRA_PATH)
#######################################

ifneq ($(CONFIG_CABLE_MODEM),)
HUMAX_LIB_LIST += -ldsgcc
HUMAX_LIB_LIST += -lstdc++ -lrt
endif


ifeq ($(CONFIG_RF4CE_MOTION_TOUCH_UEI_HILCREST),y)
HUMAX_LIB_LIST += -luapi -lMotionEngine-bundle -lMotionEngine-core -lMotionEngine-ext -lfreespace-hid
endif
#######################################

# $(sort ) is added not to linked the PNG functions of libant.a, and to linked them of libpng.a
link:
	@make link-final

link-dfbmaster:
	@make link-dfbmaster-final

link-dfbmaster-final:
	@echo 
	@echo 
	@echo ============================
	@echo 
	@echo dfbmaster link
	@echo 
	@echo ============================
	$(HUMAX_CC) $(HUMAX_WORK_DIR)/apps/dfbmaster/dfbmaster.o -Wl,--gc-sections -Wl,--start-group -lpthread $(HUMAX_LD_LIB_PATH) $(HUMAX_LIB_DFBM_PATH) $(HUMAX_LIB_DFBM_LIST) -Wl,--end-group -o $(HUMAX_PREPARE_ROOT_DIR)/usr/bin/dfbmaster
	@echo size --- name - not stripped
	@echo 
	@du -h $(HUMAX_PREPARE_ROOT_DIR)/usr/bin/dfbmaster
	@echo 
	@echo ============================
	@nm $(HUMAX_PREPARE_ROOT_DIR)/usr/bin/dfbmaster | grep -v '\(compiled\)\|\(\.o$$\)\|\( [aUw] \)\|\(\.\.ng$$\)\|\(    LASH[RL]DI\)' | sort > System.dfbmaster.map
ifneq ($(CONFIG_PRODUCT_IMAGE_RELEASE),)
	@$(HUMAX_CP) $(HUMAX_PREPARE_ROOT_DIR)/usr/bin/dfbmaster $(HUMAX_OUTPUT_DIR)/
	@$(HUMAX_STRIP) $(HUMAX_PREPARE_ROOT_DIR)/usr/bin/dfbmaster
	@echo ============================
	@echo 
	@echo size --- name - stripped for Release
	@echo 
	@du -h $(HUMAX_PREPARE_ROOT_DIR)/usr/bin/dfbmaster
	@echo 
	@echo ============================
endif

link-final: 
	@echo 
	@echo 
	@echo ============================
	@echo 
	@echo Application Program link
	@echo 
	@echo ============================
	@sudo $(HUMAX_CC) $(HUMAX_WORK_DIR)/apps/main/main.o -Wl,--gc-sections -Wl,--start-group $(sort $(HUMAX_LIB_LIST)) -lpthread $(HUMAX_LD_LIB_PATH) $(HUMAX_LIB_EXTRA_PATH) $(HUMAX_LIB_EXTRA_LIST) -Wl,--end-group -o $(HUMAX_NFS_INSTALL_DIR)/usr/bin/humaxtv
	@echo ============================
	@echo 
	@echo size --- name - not stripped
	@echo 
	@sudo du -h $(HUMAX_NFS_INSTALL_DIR)/usr/bin/humaxtv
	@echo 
	@echo ============================
	@sudo nm $(HUMAX_NFS_INSTALL_DIR)/usr/bin/humaxtv | grep -v '\(compiled\)\|\(\.o$$\)\|\( [aUw] \)\|\(\.\.ng$$\)\|\(    LASH[RL]DI\)' | sort > System.map
ifneq ($(CONFIG_PRODUCT_IMAGE_RELEASE),)
	@sudo $(HUMAX_CP) $(HUMAX_NFS_INSTALL_DIR)/usr/bin/humaxtv $(HUMAX_OUTPUT_DIR)/ 
ifneq ($(CONFIG_PRODUCT_IMAGE_HWTEST),y)
	@sudo $(HUMAX_STRIP) $(HUMAX_NFS_INSTALL_DIR)/usr/bin/humaxtv
endif

	@echo ============================
	@echo 
	@echo size --- name - stripped for humaxtv Release
	@echo 
	@sudo du -h $(HUMAX_NFS_INSTALL_DIR)/usr/bin/humaxtv
	@echo 
	@echo ============================
endif

link-dbus_volmgr:
	@echo 
	@echo ===============================================
	@echo dbus volume manager link
	@echo ===============================================
	$(HUMAX_CC) $(HUMAX_WORK_DIR)/apps/dbus_volmgr/dbus_volmgr.o -Wl,--gc-sections -Wl,--start-group -lpthread $(HUMAX_LD_LIB_PATH) $(HUMAX_LIB_DFBM_PATH) $(HUMAX_LIB_DFBM_LIST) -ldbus-1 -ldbus_lib_volmgr -Wl,--end-group -o $(HUMAX_PREPARE_ROOT_DIR)/usr/bin/dbus_volmgr
	@echo size --- name - not stripped
	@echo 
	@du -h $(HUMAX_PREPARE_ROOT_DIR)/usr/bin/dbus_volmgr
	@echo 
	@echo ============================
	@nm $(HUMAX_PREPARE_ROOT_DIR)/usr/bin/dbus_volmgr | grep -v '\(compiled\)\|\(\.o$$\)\|\( [aUw] \)\|\(\.\.ng$$\)\|\(    LASH[RL]DI\)' | sort > System.dfbmaster.map
ifneq ($(CONFIG_PRODUCT_IMAGE_RELEASE),)
	@$(HUMAX_CP) $(HUMAX_PREPARE_ROOT_DIR)/usr/bin/dbus_volmgr $(HUMAX_OUTPUT_DIR)/
	@$(HUMAX_STRIP) $(HUMAX_PREPARE_ROOT_DIR)/usr/bin/dbus_volmgr
	@echo ===============================================
	@echo size --- name - stripped for Release
	@echo 
	@du -h $(HUMAX_PREPARE_ROOT_DIR)/usr/bin/dbus_volmgr
	@echo ===============================================
endif

clean_link:
	$(HUMAX_RM) $(HUMAX_PREPARE_ROOT_DIR)/usr/bin/humaxtv

$(KERNEL_DIR)/.config:
ifeq ($(CONFIG_INITRAMFS_LINUX),y)
ifeq ($(CONFIG_PRODUCT_KERNEL_DEBUG_LOG),y)

	@cp $(KERNEL_CONFIG_DIR)/$(CONFIG_PRODUCT_NAME)/$(CONFIG_BOARD_REVISION)/kernel_config_initramfs.cfg $(KERNEL_DIR)/.config

else #CONFIG_PRODUCT_KERNEL_DEBUG_LOG

ifeq ($(CONFIG_PRODUCT_IMAGE_UOCTO),y)
	@cp $(KERNEL_CONFIG_DIR)/$(CONFIG_PRODUCT_NAME)/$(CONFIG_BOARD_REVISION)/kernel_uocto_config_initramfs_rel.cfg $(KERNEL_DIR)/.config
else ifeq ($(CONFIG_PRODUCT_IMAGE_UPGRADER),y)
	@cp $(KERNEL_CONFIG_DIR)/$(CONFIG_PRODUCT_NAME)/$(CONFIG_BOARD_REVISION)/kernel_upgrader_config_initramfs_rel.cfg $(KERNEL_DIR)/.config
else		
	@cp $(KERNEL_CONFIG_DIR)/$(CONFIG_PRODUCT_NAME)/$(CONFIG_BOARD_REVISION)/kernel_config_initramfs_rel.cfg $(KERNEL_DIR)/.config
endif

endif
	sed -e "s/\$$(HUMAX_NFS_INSTALL_DIR)/$(subst /,\/,$(HUMAX_NFS_INSTALL_DIR))/g" $(KERNEL_DIR)/.config > $(KERNEL_DIR)/.config.new
	mv $(KERNEL_DIR)/.config.new $(KERNEL_DIR)/.config
else	# !CONFIG_INITRAMFS_LINUX
ifeq ($(CONFIG_PRODUCT_KERNEL_DEBUG_LOG),y)
ifeq ($(CONFIG_TOOLS_OPROFILE),y)
	@echo "Kernel config with Oprofile option!!!"
	@cp $(KERNEL_CONFIG_DIR)/$(CONFIG_PRODUCT_NAME)/$(CONFIG_BOARD_REVISION)/kernel_config_oprofile.cfg $(KERNEL_DIR)/.config
else

ifeq ($(CONFIG_PRODUCT_IMAGE_UOCTO),y)
	@cp $(KERNEL_CONFIG_DIR)/$(CONFIG_PRODUCT_NAME)/$(CONFIG_BOARD_REVISION)/kernel_uocto_config.cfg $(KERNEL_DIR)/.config
else ifeq ($(CONFIG_PRODUCT_IMAGE_UPGRADER),y)
	@cp $(KERNEL_CONFIG_DIR)/$(CONFIG_PRODUCT_NAME)/$(CONFIG_BOARD_REVISION)/kernel_upgrader_config.cfg $(KERNEL_DIR)/.config
else
	@cp $(KERNEL_CONFIG_DIR)/$(CONFIG_PRODUCT_NAME)/$(CONFIG_BOARD_REVISION)/kernel_config.cfg $(KERNEL_DIR)/.config
endif

endif
else	# !CONFIG_PRODUCT_KERNEL_DEBUG_LOG

	@cp $(KERNEL_CONFIG_DIR)/$(CONFIG_PRODUCT_NAME)/$(CONFIG_BOARD_REVISION)/kernel_config_rel.cfg $(KERNEL_DIR)/.config

endif
endif
ifeq ($(filter-out n ,$(strip $(subst ",,$(CONFIG_KERNEL_TOOLCHAIN_PATH)))),)
	$(HUMAX_MAKE) -C $(KERNEL_DIR) CROSS_COMPILE=$(HUMAX_CROSS) CC="$(HUMAX_CC)" $(HUMAX_MODIFY_KERNEL) $(HUMAX_MODIFY_MODULE) oldconfig prepare scripts
else
	$(HUMAX_MAKE) -C $(KERNEL_DIR) CROSS_COMPILE=$(CONFIG_KERNEL_TOOLCHAIN_PATH)/bin/mipsel-linux- CC="$(CONFIG_KERNEL_TOOLCHAIN_PATH)"/bin/mipsel-linux-gcc $(HUMAX_MODIFY_KERNEL) $(HUMAX_MODIFY_MODULE) oldconfig prepare scripts
endif

$(ROOTFS_DIR)/busybox/.config:
ifeq ($(CONFIG_PRODUCT_ROOTFS_DEBUG_LOG),y)
ifeq ($(CONFIG_ROOTFS_TELNETD),y)
	cp $(ROOTFS_CONFIG_DIR)/$(CONFIG_PRODUCT_NAME)/rootfs_config_telnet.cfg $(ROOTFS_DIR)/busybox/.config
else
	cp $(ROOTFS_CONFIG_DIR)/$(CONFIG_PRODUCT_NAME)/rootfs_config.cfg $(ROOTFS_DIR)/busybox/.config
endif
else
ifeq ($(CONFIG_ROOTFS_TELNETD),y)
	cp $(ROOTFS_CONFIG_DIR)/$(CONFIG_PRODUCT_NAME)/rootfs_config_telnet.cfg $(ROOTFS_DIR)/busybox/.config
else
	cp $(ROOTFS_CONFIG_DIR)/$(CONFIG_PRODUCT_NAME)/rootfs_config_rel.cfg $(ROOTFS_DIR)/busybox/.config
endif
endif
	$(HUMAX_MAKE) -C $(ROOTFS_DIR)/busybox CC="$(HUMAX_CC)" CROSS_COMPILE="$(HUMAX_CROSS)" $(TARGET_CONFIG) PREFIX=$(HUMAX_PREPARE_ROOT_DIR) oldconfig

$(HUMAX_FLASHMAP_KER_H): $(HUMAX_FLASHMAP_H)
	$(HUMAX_CP) $< $@

kernel-prepare: $(KERNEL_DIR)/.config $(HUMAX_FLASHMAP_KER_H)

	@rm -f $(KERNEL_DIR)/usr/gen_init_cpio $(KERNEL_DIR)/usr/initramfs_data.cpio $(KERNEL_DIR)/usr/initramfs_data.cpio.gz $(KERNEL_DIR)/usr/*.o $(KERNEL_DIR)/usr/initramfs_list
	$(HUMAX_MAKE) -C $(KERNEL_DIR) $(HUMAX_MULTI_JOBS) CROSS_COMPILE=$(HUMAX_CROSS) CC="$(HUMAX_CC)" LOCALVERSION= $(HUMAX_MODIFY_KERNEL) vmlinux
	$(HUMAX_MAKE) -C $(KERNEL_DIR) $(HUMAX_MULTI_JOBS) CROSS_COMPILE=$(HUMAX_CROSS) CC="$(HUMAX_CC)" LOCALVERSION= $(HUMAX_MODIFY_KERNEL) $(HUMAX_MODIFY_MODULE) modules
	$(HUMAX_MAKE) -C $(KERNEL_DIR) $(HUMAX_MULTI_JOBS) CROSS_COMPILE=$(HUMAX_CROSS) CC="$(HUMAX_CC)" LOCALVERSION= INSTALL_MOD_PATH=$(HUMAX_PREPARE_ROOT_DIR) modules_install

.PHONY : kernel-unpack kernel-dot_config kernel-flash_map
.PHONY : kernel-patch humax-kernel
.PHONY : kernel-ready kernel kernel-prepare

ifeq ($(CONFIG_TAR_BUILD_KERNEL),y)
include $(HUMAX_PORTING_MAKE_DIR)/kernel.mk
endif

kernel-patch : humax-kernel $(KERNEL_DIR)/.config $(HUMAX_FLASHMAP_KER_H)

kernel-ready : $(KERNEL_DIR) kernel-patch

kernel : kernel-ready
ifdef CONFIG_INITRAMFS_LINUX
	@rm -f $(KERNEL_DIR)/usr/gen_init_cpio $(KERNEL_DIR)/usr/initramfs_data.cpio $(KERNEL_DIR)/usr/initramfs_data.cpio.gz $(KERNEL_DIR)/usr/*.o $(KERNEL_DIR)/usr/initramfs_list
ifeq ($(CONFIG_ARM), y)
	$(HUMAX_MAKE) -C $(KERNEL_DIR) $(HUMAX_MULTI_JOBS) CROSS_COMPILE=$(HUMAX_CROSS) CC="$(HUMAX_CC)" LOCALVERSION= $(HUMAX_MODIFY_KERNEL) zImage
else	
	$(HUMAX_MAKE) -C $(KERNEL_DIR) $(HUMAX_MULTI_JOBS) CROSS_COMPILE=$(HUMAX_CROSS) CC="$(HUMAX_CC)" LOCALVERSION= $(HUMAX_MODIFY_KERNEL) vmlinux
endif	
else
	@rm -f $(KERNEL_DIR)/usr/gen_init_cpio $(KERNEL_DIR)/usr/initramfs_data.cpio $(KERNEL_DIR)/usr/initramfs_data.cpio.gz $(KERNEL_DIR)/usr/*.o $(KERNEL_DIR)/usr/initramfs_list
ifeq ($(CONFIG_ARM), y)
	$(HUMAX_MAKE) -C $(KERNEL_DIR) $(HUMAX_MULTI_JOBS) CROSS_COMPILE=$(HUMAX_CROSS) CC="$(HUMAX_CC)" LOCALVERSION= $(HUMAX_MODIFY_KERNEL) zImage
else	
	$(HUMAX_MAKE) -C $(KERNEL_DIR) $(HUMAX_MULTI_JOBS) CROSS_COMPILE=$(HUMAX_CROSS) CC="$(HUMAX_CC)" LOCALVERSION= $(HUMAX_MODIFY_KERNEL) vmlinux
endif	
	$(HUMAX_MAKE) -C $(KERNEL_DIR) $(HUMAX_MULTI_JOBS) CROSS_COMPILE=$(HUMAX_CROSS) CC="$(HUMAX_CC)" LOCALVERSION= $(HUMAX_MODIFY_KERNEL) $(HUMAX_MODIFY_MODULE) modules
	$(HUMAX_MAKE) -C $(KERNEL_DIR) $(HUMAX_MULTI_JOBS) CROSS_COMPILE=$(HUMAX_CROSS) CC="$(HUMAX_CC)" LOCALVERSION= INSTALL_MOD_PATH=$(HUMAX_PREPARE_ROOT_DIR) modules_install
endif
ifeq ($(CONFIG_DEVICE_MODE_USER),y)
ifeq ($(CONFIG_DI_SHARED_LIB),y)
	$(HUMAX_MAKE) -C $(KERNEL_DIR) SUBDIRS=$(BRCM_REGISTER_CTRL_DRIVER_PATH) $(HUMAX_MULTI_JOBS) CROSS_COMPILE=$(HUMAX_CROSS) CC="$(HUMAX_CC)" LOCALVERSION= $(HUMAX_MODIFY_KERNEL) $(HUMAX_MODIFY_MODULE) modules
	$(HUMAX_MAKE) -C $(KERNEL_DIR) SUBDIRS=$(BRCM_REGISTER_CTRL_DRIVER_PATH) $(HUMAX_MULTI_JOBS) CROSS_COMPILE=$(HUMAX_CROSS) CC="$(HUMAX_CC)" LOCALVERSION= INSTALL_MOD_PATH=$(HUMAX_PREPARE_ROOT_DIR) modules_install
else ifeq ($(CONFIG_DI_MODULE_SHARED_LIB),y)
	$(HUMAX_MAKE) -C $(KERNEL_DIR) SUBDIRS=$(BRCM_REGISTER_CTRL_DRIVER_PATH) $(HUMAX_MULTI_JOBS) CROSS_COMPILE=$(HUMAX_CROSS) CC="$(HUMAX_CC)" LOCALVERSION= $(HUMAX_MODIFY_KERNEL) $(HUMAX_MODIFY_MODULE) modules
	$(HUMAX_MAKE) -C $(KERNEL_DIR) SUBDIRS=$(BRCM_REGISTER_CTRL_DRIVER_PATH) $(HUMAX_MULTI_JOBS) CROSS_COMPILE=$(HUMAX_CROSS) CC="$(HUMAX_CC)" LOCALVERSION= INSTALL_MOD_PATH=$(HUMAX_PREPARE_ROOT_DIR) modules_install
endif
endif
ifeq ($(CONFIG_ARM), y)
	cp -f $(KERNEL_DIR)/arch/arm/boot/zImage $(CONFIG_PRODUCT_TFTPBOOT_DIR)/vmlinuz-$(LOGNAME)-$(HUMAX_PRODUCT_BUILD_NAME);
else
	@if [ -f $(KERNEL_DIR)/vmlinux_wo_symbol ]; then \
		gzip --best -cfv $(KERNEL_DIR)/vmlinux_wo_symbol > $(CONFIG_PRODUCT_TFTPBOOT_DIR)/vmlinuz-$(LOGNAME)-$(HUMAX_PRODUCT_BUILD_NAME); \
	else \
		gzip --best -cfv $(KERNEL_DIR)/vmlinux > $(CONFIG_PRODUCT_TFTPBOOT_DIR)/vmlinuz-$(LOGNAME)-$(HUMAX_PRODUCT_BUILD_NAME); \
	fi	
endif
ifeq ($(CONFIG_TOOLS_OPROFILE),y)
	@-cp $(KERNEL_DIR)/vmlinux $(HUMAX_PREPARE_ROOT_DIR)
endif

ifdef CONFIG_INITRAMFS_LINUX
	@cp $(CONFIG_PRODUCT_TFTPBOOT_DIR)/vmlinuz-$(LOGNAME)-$(CONFIG_PRODUCT_NAME) $(HUMAX_NFS_DIR)/image/vmlinuz.tet
ifeq ($(CONFIG_PRODUCT_IMAGE_UPGRADER),y)
	@if [ ! -d $(UPGRADER_MAKE_DIR)/RELEASE ]; then \
		cp $(CONFIG_PRODUCT_TFTPBOOT_DIR)/vmlinuz-$(LOGNAME)-$(CONFIG_PRODUCT_NAME) $(UPGRADER_MAKE_DIR)/DEBUG/upgrader.bin; \
	else \
		cp $(CONFIG_PRODUCT_TFTPBOOT_DIR)/vmlinuz-$(LOGNAME)-$(CONFIG_PRODUCT_NAME) $(UPGRADER_MAKE_DIR)/RELEASE/upgrader.bin; \
	fi
endif
endif

kernel-clean kernel-prepare-clean :
	$(HUMAX_MAKE) -C $(KERNEL_DIR) clean
ifeq ($(CONFIG_DI_SHARED_LIB),y)	
	$(HUMAX_MAKE) -C $(KERNEL_DIR) SUBDIRS=$(BRCM_REGISTER_CTRL_DRIVER_PATH) clean
endif
	$(HUMAX_RM) $(HUMAX_FLASHMAP_KER_H)
	
kernel-distclean kernel-prepare-distclean : kernel-clean
	$(HUMAX_MAKE) -C $(KERNEL_DIR) mrproper
ifeq ($(CONFIG_TAR_BUILD_KERNEL),y)
	rm -rf $(KERNEL_DIR)
endif
	

BASEFILES_COMPILE_FLAGS = \
			CONFIG_ROOTFS_MODULE_TYPE=$(CONFIG_ROOTFS_MODULE_TYPE) \
			CONFIG_PRODUCT_NAME=$(CONFIG_PRODUCT_NAME) \
			CONFIG_ROOTFS_PVR=$(CONFIG_ROOTFS_PVR) \
			CONFIG_ROOTFS_TTY0=$(CONFIG_ROOTFS_TTY0) \
			CONFIG_ROOTFS_ETH=$(CONFIG_ROOTFS_ETH) \
			CONFIG_ROOTFS_MSDRM=$(CONFIG_ROOTFS_MSDRM) \
			CONFIG_ROOTFS_TELNETD=$(CONFIG_ROOTFS_TELNETD) \
			CONFIG_ROOTFS_NAND_ONLY=$(CONFIG_ROOTFS_NAND_ONLY) \
			CONFIG_ROOTFS_NOR_NAND=$(CONFIG_ROOTFS_NOR_NAND) \
			CONFIG_ROOTFS_ONEIMAGE=$(CONFIG_ROOTFS_ONEIMAGE) \
			CONFIG_ROOTFS_PLAYREADY=$(CONFIG_PLAYREADY) \
			CONFIG_ROOTFS_PLAYREADY_ROBUST=$(CONFIG_BRCM_MOD_PRDY_ROBUSTNESS_ENABLE) \
			CONFIG_INITRAMFS_LINUX=$(CONFIG_INITRAMFS_LINUX) \
			CONFIG_PRODUCT_IMAGE_OCTO=$(CONFIG_PRODUCT_IMAGE_OCTO) \
			CONFIG_PRODUCT_IMAGE_HWTEST=$(CONFIG_PRODUCT_IMAGE_HWTEST) \
			CONFIG_PRODUCT_IMAGE_FACTORY=$(CONFIG_PRODUCT_IMAGE_FACTORY) \
			CONFIG_PRODUCT_IMAGE_UPGRADER=$(CONFIG_PRODUCT_IMAGE_UPGRADER) \
			CONFIG_IMAGE_RELEASE=$(CONFIG_PRODUCT_IMAGE_RELEASE) \
			CONFIG_TOOLS_OPROFILE=$(CONFIG_TOOLS_OPROFILE) \
			CONFIG_CRC_AUTOTEST=$(CONFIG_CRC_AUTOTEST) \
			CONFIG_DIRECTFB_MASTER=$(CONFIG_DIRECTFB_MASTER) \
			CONFIG_WIFI_SCRIPT_TEST=$(CONFIG_WIFI_SCRIPT_TEST) \
			CONFIG_MEDIA20=$(CONFIG_MEDIA20) \
			CONFIG_MEDIA10=$(CONFIG_MEDIA10) \
			CONFIG_CALYPSO=$(CONFIG_CALYPSO) \
			CONFIG_DBUS=$(CONFIG_DBUS) \
			CONFIG_IPV6=$(CONFIG_IPV6) \
			CONFIG_BOARD_REVISION=$(CONFIG_BOARD_REVISION) \
			CONFIG_KERNEL_DEVTMPFS=$(CONFIG_KERNEL_DEVTMPFS) \
			HUMAX_PACKAGE_DIR=$(HUMAX_PACKAGE_DIR) \
			CONFIG_LXC=$(CONFIG_LXC) \
			CONFIG_PRODUCT_DI_DEBUG_LOG=$(CONFIG_PRODUCT_DI_DEBUG_LOG) \
			CONFIG_ASAN=$(CONFIG_ASAN) \
			prefix=$(HUMAX_PREPARE_ROOT_DIR) \
			HUMAX_MAKE=$(HUMAX_MAKE) \
			HUMAX_UCLIBC_OLD_DIR=$(HUMAX_UCLIBC_OLD_DIR) \
			CONFIG_WAYLAND=$(CONFIG_WAYLAND) \
			CONFIG_S3_WARM_SECURE_BOOT=$(CONFIG_S3_WARM_SECURE_BOOT) \
			CONFIG_DEVELOP=$(CONFIG_DEVELOP)




ifeq ($(CONFIG_PRODUCT_ROOTFS_DEBUG_LOG),y)
BUSYBOX_TAR = $(wildcard $(HUMAX_PACKAGE_MAKE_DIR)/busybox/debug/busybox-*.tar.gz)
else
BUSYBOX_TAR = $(wildcard $(HUMAX_PACKAGE_MAKE_DIR)/busybox/release/busybox-*.tar.gz)
endif

ifeq ($(CONFIG_BUSYBOX_COMPILE),y)
busybox : $(ROOTFS_DIR)/busybox/.config 
	$(HUMAX_MAKE) -C $(ROOTFS_DIR)/busybox CC="$(HUMAX_CC)" CROSS_COMPILE="$(HUMAX_CROSS)" $(TARGET_CONFIG) CONFIG_PREFIX=$(HUMAX_PREPARE_ROOT_DIR) install
else
busybox : $(BUSYBOX_TAR)
	@echo "Untar $(BUSYBOX_TAR)..."
	@tar -xvzf $(BUSYBOX_TAR) -C $(HUMAX_PREPARE_ROOT_DIR) --strip 1
endif

.PHONY : basefiles

basefiles : 
	@echo "Make basefiles..."
	$(HUMAX_MAKE) -C $(ROOTFS_DIR)/base-files libcs="$(UCLIBC)" libcs-strip="$(UCLIBC_STRIP)" $(BASEFILES_COMPILE_FLAGS) prefix=$(HUMAX_PREPARE_ROOT_DIR) install

rootfs : busybox basefiles
	@-find $(HUMAX_PREPARE_ROOT_DIR) -type d \( -name CVS -o -name .svn \) | xargs rm -rf

rootfs-clean :
ifeq ($(CONFIG_BUSYBOX_COMPILE),y)
	$(HUMAX_MAKE) -C $(ROOTFS_DIR)/busybox CC="$(HUMAX_CC)" CROSS_COMPILE="$(HUMAX_CROSS)" PREFIX=$(HUMAX_WORK_DIR)/make/root clean
endif

rootfs-distclean :
ifeq ($(CONFIG_BUSYBOX_COMPILE),y)
	$(HUMAX_MAKE) -C $(ROOTFS_DIR)/busybox CC="$(HUMAX_CC)" CROSS_COMPILE="$(HUMAX_CROSS)" PREFIX=$(HUMAX_WORK_DIR)/make/root distclean
endif

ifeq ($(CONFIG_NTFS_RW_LIB),y)
tuxera_ntfs : 
	@if [ ! -d $(NTFS_DRIVER_PREPPARE_ROOT_DIR) ]; then\
		mkdir -p $(NTFS_DRIVER_PREPPARE_ROOT_DIR); \
	fi
ifeq ($(CONFIG_INITRAMFS_LINUX),y)
ifeq ($(CONFIG_PRODUCT_KERNEL_DEBUG_LOG),y)
	@echo -- NTFS
	@cp -rf ${NTFS_DRIVER_PATH}/initramfs_debug/ntfs/kernel-module/tntfs.ko ${NTFS_DRIVER_PREPPARE_ROOT_DIR}
else
	@echo -- NTFS
	@cp -rf ${NTFS_DRIVER_PATH}/initramfs_release/ntfs/kernel-module/tntfs.ko ${NTFS_DRIVER_PREPPARE_ROOT_DIR}
endif
else
ifeq ($(CONFIG_PRODUCT_KERNEL_DEBUG_LOG),y)
ifeq ($(CONFIG_TOOLS_OPROFILE),y)
	@echo -- "NTFS for debug with oprofile"
	@cp -rf ${NTFS_DRIVER_PATH}/debug_oprofile/ntfs/kernel-module/tntfs.ko ${NTFS_DRIVER_PREPPARE_ROOT_DIR}
else
	@echo -- "NTFS for debug"
	@cp -rf ${NTFS_DRIVER_PATH}/debug/ntfs/kernel-module/tntfs.ko ${NTFS_DRIVER_PREPPARE_ROOT_DIR}
endif
else
	@echo -- NTFS
	@cp -rf ${NTFS_DRIVER_PATH}/release/ntfs/kernel-module/tntfs.ko ${NTFS_DRIVER_PREPPARE_ROOT_DIR}	
endif
endif

tuxera_ntfs-clean :
	@echo "tuxera_ntfs-clean"

tuxera_ntfs-distclean : tuxera_ntfs-clean
	@echo "tuxera_ntfs-distclean"
	
endif

################# for MoCA #################
ifeq ($(CONFIG_MOCA), y)
TARGETS += moca
ifeq ($(CONFIG_BRCM_MOCA), y)
BRCM_MOCA_DRV_VER=MoCA_2.10.3.2
BRCM_MOCA_DRV_PATH=$(HUMAX_WORK_DIR)/platform/sdk/os/drivers/moca/$(BRCM_MOCA_DRV_VER)
endif
moca-untar :
	$(HUMAX_ECHO) "====== Extracting MoCA Driver($(BRCM_MOCA_DRV_VER))"
ifeq ($(CONFIG_BRCM_MOCA), y)
	@if [ ! -d $(BRCM_MOCA_DRV_PATH) ]; then \
		mkdir -p $(BRCM_MOCA_DRV_PATH); \
		tar -xvf $(BRCM_MOCA_DRV_PATH)/../$(BRCM_MOCA_DRV_VER).tar -C $(BRCM_MOCA_DRV_PATH); \
	fi	
endif

moca-build : moca-untar
	$(HUMAX_ECHO) "====== Compiling MoCA Driver"
ifeq ($(CONFIG_BRCM_MOCA), y)
	make -C $(BRCM_MOCA_DRV_PATH) CROSS=$(HUMAX_CROSS) all
endif

moca-install : moca-build
	$(HUMAX_ECHO) "====== Installing MoCA Driver in HUMAX_PREPARE_ROOT_DIR"
ifeq ($(CONFIG_BRCM_MOCA_20), y)
	@test -d $(HUMAX_PREPARE_ROOT_DIR)/etc/moca || \
		install -d -m 755 $(HUMAX_PREPARE_ROOT_DIR)/etc/moca
	@install -m 755 $(BRCM_MOCA_DRV_PATH)/../mocacfg $(HUMAX_PREPARE_ROOT_DIR)/usr/bin
ifeq "$(BCHP_CHIP)$(BCHP_VER)" "7435B0"
#	$(HUMAX_ECHO) $(BCHP_CHIP)$(BCHP_VER)
	$(HUMAX_CP) -f $(BRCM_MOCA_DRV_PATH)/moca20core-gen22.bin $(HUMAX_PREPARE_ROOT_DIR)/etc/moca
	@ln -sf moca20core-gen22.bin $(HUMAX_PREPARE_ROOT_DIR)/etc/moca/mocacore.bin
endif
	$(HUMAX_CP) -f $(BRCM_MOCA_DRV_PATH)/bin/mocad $(HUMAX_PREPARE_ROOT_DIR)/usr/bin
	$(HUMAX_CP) -f $(BRCM_MOCA_DRV_PATH)/bin/mocap $(HUMAX_PREPARE_ROOT_DIR)/usr/bin
	$(HUMAX_CP) -f $(BRCM_MOCA_DRV_PATH)/bin/GCAP $(HUMAX_PREPARE_ROOT_DIR)/usr/bin
	$(HUMAX_CP) -f $(BRCM_MOCA_DRV_PATH)/bin/tpcap $(HUMAX_PREPARE_ROOT_DIR)/usr/bin
#	$(HUMAX_CP) $(BRCM_MOCA_DRV_PATH)/bin/libmocacli.a $(HUMAX_OUTPUT_DIR)/lib
# Make symbolic links(DCAPs&GCAPs) to GCAP for MoCA 2.0
	@for p in $(filter-out GCAP_Common GCAP, $(patsubst %.c, %, $(notdir $(shell find $(BRCM_MOCA_DRV_PATH)/apps/GCAP -name '*.c')))); \
	do \
		ln -sf GCAP $(HUMAX_PREPARE_ROOT_DIR)/usr/bin/$$p ;\
	done
	@for p in $(patsubst %.c, %, $(notdir $(shell find $(BRCM_MOCA_DRV_PATH)/apps/DCAP -name '*.c'))); \
	do \
		ln -sf GCAP $(HUMAX_PREPARE_ROOT_DIR)/usr/bin/$$p ;\
	done
# Make symbolic links to read-write region on Read-only file system
	@for p in lof.eth1 rfcalibdata.eth1 probe2res.eth1 mocad-eth1.pid moca.conf.default; \
	do \
		ln -sf /var/lib/humaxtv_user/$$p $(HUMAX_PREPARE_ROOT_DIR)/etc/moca/$$p; \
	done
endif

moca : moca-install
	$(HUMAX_ECHO) "====== Copying MoCA apps&F/W in HUMAX_NFS_INSTALL_DIR"
ifeq ($(CONFIG_BRCM_MOCA_20), y)
	@test -d $(HUMAX_NFS_INSTALL_DIR)/etc/moca || \
		install -d -m 755 $(HUMAX_NFS_INSTALL_DIR)/etc/moca
	-$(HUMAX_CP) -rf $(HUMAX_PREPARE_ROOT_DIR)/etc/moca/* $(HUMAX_NFS_INSTALL_DIR)/etc/moca
	-@install -m 755 $(HUMAX_PREPARE_ROOT_DIR)/usr/bin/mocacfg $(HUMAX_NFS_INSTALL_DIR)/usr/bin
	-$(HUMAX_CP) -f $(HUMAX_PREPARE_ROOT_DIR)/usr/bin/mocad $(HUMAX_NFS_INSTALL_DIR)/usr/bin
	-$(HUMAX_CP) -f $(HUMAX_PREPARE_ROOT_DIR)/usr/bin/mocap $(HUMAX_NFS_INSTALL_DIR)/usr/bin
	-$(HUMAX_CP) -fa $(HUMAX_PREPARE_ROOT_DIR)/usr/bin/GCAP* $(HUMAX_NFS_INSTALL_DIR)/usr/bin
	-$(HUMAX_CP) -fa $(HUMAX_PREPARE_ROOT_DIR)/usr/bin/DCAP* $(HUMAX_NFS_INSTALL_DIR)/usr/bin
	-$(HUMAX_CP) -f $(HUMAX_PREPARE_ROOT_DIR)/usr/bin/tpcap $(HUMAX_NFS_INSTALL_DIR)/usr/bin
#	$(HUMAX_ECHO) "Stripping MoCA Driver in HUMAX_NFS_INSTALL_DIR"
#	$(HUMAX_STRIP)
endif

moca-uninstall :
	$(HUMAX_ECHO) "====== Uninstalling MoCA apps&F/Wr in HUMAX_PREPARE_ROOT_DIR"
ifeq ($(CONFIG_BRCM_MOCA_20), y)
	$(HUMAX_RM) $(HUMAX_PREPARE_ROOT_DIR)/etc/moca
	$(HUMAX_RM) $(HUMAX_PREPARE_ROOT_DIR)/usr/bin/moca* 
	$(HUMAX_RM) $(HUMAX_PREPARE_ROOT_DIR)/usr/bin/GCAP*
	$(HUMAX_RM) $(HUMAX_PREPARE_ROOT_DIR)/usr/bin/DCAP* 
	$(HUMAX_RM) $(HUMAX_PREPARE_ROOT_DIR)/usr/bin/tpcap 
#	$(HUMAX_RM) $(HUMAX_OUTPUT_DIR)/lib/libmocacli.a
endif

moca-clean : moca-uninstall
	$(HUMAX_ECHO) "====== Cleaning MoCA apps&F/W in HUMAX_NFS_INSTALL_DIR"
ifeq ($(CONFIG_BRCM_MOCA_20), y)
	$(HUMAX_RM) $(HUMAX_NFS_INSTALL_DIR)/etc/moca
	$(HUMAX_RM) $(HUMAX_NFS_INSTALL_DIR)/usr/bin/moca*
	$(HUMAX_RM) $(HUMAX_NFS_INSTALL_DIR)/usr/bin/GCAP*
	$(HUMAX_RM) $(HUMAX_NFS_INSTALL_DIR)/usr/bin/DCAP*
	$(HUMAX_RM) $(HUMAX_NFS_INSTALL_DIR)/usr/bin/tpcap 
endif

moca-distclean : moca-clean
	$(HUMAX_ECHO) "====== Distcleaning MoCA Driver"
ifeq ($(CONFIG_BRCM_MOCA), y)
	@if [ -d $(BRCM_MOCA_DRV_PATH) ]; then \
		make -C $(BRCM_MOCA_DRV_PATH) CROSS=$(HUMAX_CROSS) clean; \
	fi
	$(HUMAX_ECHO) "====== Removing MoCA Driver"
#	@cd $(BRCM_MOCA_DRV_PATH) && tar -tf $(BRCM_MOCA_DRV_PATH)/../$(BRCM_MOCA_DRV_VER).tar | xargs rm -rf
	$(HUMAX_RM) $(BRCM_MOCA_DRV_PATH)

endif
endif
################# for MoCA #################

################# for Playready #################
Q_ ?= @

playready20:
	@echo "==== $@"
	$(Q_)$(HUMAX_MAKE) -C $(NEXUS_TOP)/../BSEAV/lib/playready20/source $(HUMAX_MULTI_JOBS) DEBUG=$(DEVICE_DEBUG) $(NEXUS_CONFIG) NEXUS_MODE=client packaging
	
playready20-clean playready20-distclean:
	@echo "==== $@"
	$(Q_)$(HUMAX_MAKE) -C $(NEXUS_TOP)/../BSEAV/lib/playready20/source DEBUG=$(DEVICE_DEBUG) $(NEXUS_CONFIG) NEXUS_MODE=client $(@:playready20-%=%)

################# for Playready2.5 BRCM #################
Q_ ?= @

playready25_brcm:
	@echo "==== playready2.5 NEXUS Enable"
	#libbcrypt
	$(Q_)$(HUMAX_MAKE) DEBUG=$(DEVICE_DEBUG) $(NEXUS_CONFIG) -C $(NEXUS_TOP)/../BSEAV/lib/security/bcrypt B_OPENSSL_BUILD_COMPLETED=y B_OPENSSL_CFLAGS="-I$(HUMAX_PACKAGE_MAKE_DIR)/openssl/include" B_OPENSSL_LDFLAGS="-L$(HUMAX_PACKAGE_MAKE_DIR)/openssl/lib" all install
	$(Q_)$(HUMAX_MAKE) DEBUG=$(DEVICE_DEBUG) $(NEXUS_CONFIG) -C $(NEXUS_TOP)/../BSEAV/lib/drmrootfs install
	$(Q_)$(HUMAX_MAKE) DEBUG=$(DEVICE_DEBUG) $(NEXUS_CONFIG) -C $(NEXUS_TOP)/../BSEAV/lib/security/sage/srai all
	#libcmndrmprdy.so
	$(Q_)$(HUMAX_MAKE) DEBUG=$(DEVICE_DEBUG) $(NEXUS_CONFIG) -C $(NEXUS_TOP)/../BSEAV/lib/security/common_drm URSR_TOP=$(NEXUS_TOP)/.. B_OPENSSL_CFLAGS="-I$(HUMAX_PACKAGE_MAKE_DIR)/openssl/include" playready
	#libcmndrm_tl.so
	$(Q_)$(HUMAX_MAKE) DEBUG=$(DEVICE_DEBUG) $(NEXUS_CONFIG) -C $(NEXUS_TOP)/../BSEAV/lib/security/common_drm URSR_TOP=$(NEXUS_TOP)/.. all copy
	$(Q_)$(HUMAX_CP) $(NEXUS_TOP)/../BSEAV/lib/security/common_drm/libcmndrmprdy.so $(NEXUS_TOP)/bin
ifeq ($(CONFIG_ARM), y)
	$(Q_)$(HUMAX_CP) $(NEXUS_TOP)/../BSEAV/lib/playready/2.5/bin/arm/lib/libplayreadypk_host.so $(NEXUS_TOP)/bin
else
	$(Q_)$(HUMAX_CP) $(NEXUS_TOP)/../BSEAV/lib/playready/2.5/bin/mips/lib/libplayreadypk_host.so $(NEXUS_TOP)/bin
endif
	$(Q_)$(HUMAX_CP) $(NEXUS_TOP)/bin/libbcrypt.so $(HUMAX_PREPARE_ROOT_DIR)/usr/lib
	$(Q_)$(HUMAX_CP) $(NEXUS_TOP)/bin/libcmndrmprdy.so $(HUMAX_PREPARE_ROOT_DIR)/usr/lib
	$(Q_)$(HUMAX_CP) $(NEXUS_TOP)/bin/libcmndrm_tl.so $(HUMAX_PREPARE_ROOT_DIR)/usr/lib
	$(Q_)$(HUMAX_CP) $(NEXUS_TOP)/bin/libdrmrootfs.so $(HUMAX_PREPARE_ROOT_DIR)/usr/lib
	$(Q_)$(HUMAX_CP) $(NEXUS_TOP)/bin/libsrai.so $(HUMAX_PREPARE_ROOT_DIR)/usr/lib
	$(Q_)$(HUMAX_CP) $(NEXUS_TOP)/bin/libcmndrm.so $(HUMAX_PREPARE_ROOT_DIR)/usr/lib
	$(Q_)$(HUMAX_CP) $(NEXUS_TOP)/bin/libplayreadypk_host.so $(HUMAX_PREPARE_ROOT_DIR)/usr/lib
	@echo "==== playready2.5 NEXUS Compile Success"

playready25_brcm-clean playready25_brcm-distclean:
	$(Q_)$(HUMAX_MAKE) DEBUG=$(DEVICE_DEBUG) $(NEXUS_CONFIG) -C $(NEXUS_TOP)/../BSEAV/lib/security/bcrypt B_OPENSSL_BUILD_COMPLETED=y clean
	$(Q_)$(HUMAX_MAKE) DEBUG=$(DEVICE_DEBUG) $(NEXUS_CONFIG) -C $(NEXUS_TOP)/../BSEAV/lib/drmrootfs clean
	$(Q_)$(HUMAX_MAKE) DEBUG=$(DEVICE_DEBUG) $(NEXUS_CONFIG) -C $(NEXUS_TOP)/../BSEAV/lib/security/sage/srai clean
	$(Q_)$(HUMAX_MAKE) DEBUG=$(DEVICE_DEBUG) $(NEXUS_CONFIG) -C $(NEXUS_TOP)/../BSEAV/lib/security/common_drm URSR_TOP=$(NEXUS_TOP)/.. clean
	$(Q_)$(HUMAX_RM) $(HUMAX_PREPARE_ROOT_DIR)/usr/lib/libbcrypt.so $(NEXUS_TOP)/bin/libbcrypt.so
	$(Q_)$(HUMAX_RM) $(HUMAX_PREPARE_ROOT_DIR)/usr/lib/libcmndrm.so $(NEXUS_TOP)/bin/libcmndrm.so
	$(Q_)$(HUMAX_RM) $(HUMAX_PREPARE_ROOT_DIR)/usr/lib/libcmndrm_tl.so $(NEXUS_TOP)/bin/libcmndrm_tl.so
	$(Q_)$(HUMAX_RM) $(HUMAX_PREPARE_ROOT_DIR)/usr/lib/libcmndrmprdy.so $(NEXUS_TOP)/bin/libcmndrmprdy.so
	$(Q_)$(HUMAX_RM) $(HUMAX_PREPARE_ROOT_DIR)/usr/lib/libdrmrootfs.so $(NEXUS_TOP)/bin/libdrmrootfs.so
	$(Q_)$(HUMAX_RM) $(HUMAX_PREPARE_ROOT_DIR)/usr/lib/libplayreadypk_host.so $(NEXUS_TOP)/bin/libplayreadypk_host.so
	$(Q_)$(HUMAX_RM) $(HUMAX_PREPARE_ROOT_DIR)/usr/lib/libsrai.so  $(NEXUS_TOP)/bin/libsrai.so
#########################################################

openssl_for_hwtest openssl_for_factory:
	@echo -- openssl for hwtest
	$(HUMAX_CP) -drpf $(HUMAX_WORK_DIR)/../packages/openssl/lib/*.so* $(HUMAX_PREPARE_ROOT_DIR)/usr/lib
	(rm -rf $(HUMAX_WORK_DIR)/../include/openssl)
	mkdir -p $(HUMAX_WORK_DIR)/../include
	(cd $(HUMAX_WORK_DIR)/../include; ln -sf $(HUMAX_OPENSSL_INCLUDE_SRC) openssl;)
openssl_for_hwtest-clean openssl_for_hwtest-distclean openssl_for_factory-clean openssl_for_factory-distclean:
	$(HUMAX_RM) $(HUMAX_PREPARE_ROOT_DIR)/usr/lib/libssl.so* $(HUMAX_PREPARE_ROOT_DIR)/usr/lib/libcrypto.so*

-include $(HUMAX_PORTING_MAKE_DIR)/HumaxPlatformKernelTools.mak 

