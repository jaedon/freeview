-include $(HUMAX_PLATFORM_CONFIG_DIR)/.config
-include $(HUMAX_MAKE_DIR)/.config
include $(HUMAX_WORK_DIR)/platform/sdk/driver/bcm_environment.config
include $(HUMAX_PORTING_MAKE_DIR)/HumaxPlatformDef.mak
include $(HUMAX_PORTING_MAKE_DIR)/HumaxPlatformPath.mak
##include $(NEXUS_TOP)/platforms/$(PLATFORM)/build/platform_app.inc

HUMAX_PRODUCT_NAME=$(shell echo $(CONFIG_PRODUCT_NAME) | sed "/^.\"/d")
HUMAX_BOARD_REVISION=$(shell echo $(CONFIG_BOARD_REVISION) | sed "/^.\"/d")

HUMAX_FLASHMAP_FILE=$(FLASHMAP_CONFIG_DIR)/$(HUMAX_PRODUCT_NAME)/$(HUMAX_BOARD_REVISION)/flash_map.h
HUMAX_FLASHMAP_DRV_FILE=$(HUMAX_WORK_DIR)/platform/driver/di/drv/include/flash_map.h

HUMAX_RAMMAP_FILE=$(RAMMAP_CONFIG_DIR)/$(HUMAX_PRODUCT_NAME)/$(HUMAX_BOARD_REVISION)/ram_map.h
HUMAX_RAMMAP_DRV_FILE=$(HUMAX_WORK_DIR)/platform/driver/di/drv/include/ram_map.h

HUMAX_I2C_FILE=$(DEVICE_CONFIG_DIR)/$(HUMAX_PRODUCT_NAME)/$(HUMAX_BOARD_REVISION)/i2c_bus_index.h
HUMAX_I2C_DRV_FILE=$(HUMAX_WORK_DIR)/platform/driver/di/drv/include/i2c_bus_index.h

NEXUS_VERSION_MINOR ?= 0

obj-y := 
ifeq ($(CONFIG_PRODUCT_DI_DEBUG_LOG),y)
obj-y += cmd/
endif
obj-y += di/
obj-y += utils/

ifeq ($(CONFIG_OS_LINUX),y)
obj-vkernel-y += vkernel/
else
obj-y += vkernel/
endif

ifdef CONFIG_DI_SHARED_LIB
	obj-di_so-y += di/
endif

obj-mi-${CONFIG_CALYPSO} += mi/

ifdef CONFIG_DI_DEMUX_EXT
	obj-didemux_ext-y += di_ext/
endif

ifeq ($(CONFIG_MHEG_SUPPORT),y)
obj-$(CONFIG_ICS) += di_ics/
endif

inccflag-y := -I$(HUMAX_WORK_DIR)/include/di
inccflag-y += -I$(HUMAX_WORK_DIR)/include/hos
inccflag-y += -I$(HUMAX_WORK_DIR)/include/cmd
inccflag-y += -I$(HUMAX_WORK_DIR)/include/ai
inccflag-y += -I$(HUMAX_DRIVER_DIR)/di/drv/include
inccflag-y += -I$(HUMAX_DRIVER_DIR)/di/channel/inc
inccflag-y += -I$(HUMAX_DRIVER_DIR)/utils/include
inccflag-y += -I$(HUMAX_DRIVER_DIR)/cmd/include
inccflag-y += -I$(HUMAX_DRIVER_DIR)/cmd/unittest
inccflag-y += -I$(HUMAX_DRIVER_DIR)/di/channel/ter/mxl683/
inccflag-y += -I$(HUMAX_DRIVER_DIR)/di/channel/ter/ds2820/
inccflag-y += -I$(HUMAX_DRIVER_DIR)/di/channel/ter/ds2820/include
inccflag-y += -I$(HUMAX_DRIVER_DIR)/di/channel/ter/tn18273/inc
inccflag-y += -I$(HUMAX_DRIVER_DIR)/di/channel/ter/tn18273/tmbslTDA18273/inc
inccflag-y += -I$(HUMAX_DRIVER_DIR)/di/channel/ter/tn18273/tmbslTDA18273/cfg
inccflag-y += -I$(HUMAX_DRIVER_DIR)/di/nvram
inccflag-y += -I$(HUMAX_DRIVER_DIR)/di

ifeq ($(CONFIG_DEVICE_PRIVILEGE),y)
inccflag-y += -I$(HUMAX_PACKAGE_MAKE_DIR)/libcap/usr/include
endif

ifeq ($(CONFIG_MHEG_SUPPORT),y)
inccflag-$(CONFIG_ICS) += -I$(HUMAX_WORK_DIR)/include/ics
endif

ifeq ($(CONFIG_CABLE_MODEM),y)
ifeq ($(CONFIG_CABLE_MODEM_BRCM_DSGCC),y)
	inccflag-y += -I$(HUMAX_DRIVER_DIR)/di/cm/bdsgcc
endif
ifeq ($(CONFIG_CABLE_MODEM_BRCM_RPC),y)
	inccflag-y += -I$(HUMAX_DRIVER_DIR)/di/cm/brpc
endif
endif

inccflag-y += -I$(HUMAX_DRIVER_DIR)/di/rf4ce
ifeq ($(CONFIG_RF4CE_MOTION_TOUCH_UEI_HILCREST), y)
	inccflag-y += -I$(HUMAX_DRIVER_DIR)/di/drv/hilcrest/uei_hilcrest
endif
###inccflag-y += -I$(NEXUS_APP_INCLUDE_PATHS)

ifeq ($(shell expr $(NEXUS_VERSION) ">=" 1501),1)
inccflag-y += -I$(NEXUS_TOP)/../BSEAV/linux/driver/wakeup
endif

HUMAX_EXTRA_CFLAG := $(inccflag-y) -include $(HUMAX_MAKE_DIR)/$(CONFIG_PRODUCT_NAME)/autoconf.h
HUMAX_EXTRA_CFLAG += $(addprefix -I,$(NEXUS_APP_INCLUDE_PATHS)) $(addprefix -D,$(NEXUS_APP_DEFINES))
HUMAX_EXTRA_CFLAG += -D_GNU_SOURCE=1 -DHUMAX_PLATFORM_BASE=y
HUMAX_EXTRA_CFLAG += $(filter-out $(HUMAX_DEFAULT_CFLAG),$(subst -std=c89,,$(NEXUS_CFLAGS)))
HUMAX_EXTRA_CFLAG += $(addprefix -D,$(subst HUMAX_PLATFORM_BASE,,$(BCM_FIX)))
HUMAX_EXTRA_CFLAG += -include $(HUMAX_MAKE_DIR)/autoconf.h

ifneq ($(CONFIG_PRODUCT_IMAGE_DEBUG),)
ifeq ($(CONFIG_OS_LINUX),y)
	HUMAX_EXTRA_CFLAG += -fstack-protector-all
endif
endif

ifeq ($(CONFIG_PRODUCT_DI_DEBUG_LOG),y)
	HUMAX_EXTRA_CFLAG += -DCONFIG_DEBUG
endif
ifneq ($(CONFIG_PRODUCT_IMAGE_FACTORY),)
else
	ifeq ($(CONFIG_DIRECTFB),y)
		HUMAX_EXTRA_CFLAG += -I$(HUMAX_WORK_DIR)/platform/sdk/driver/AppLibs/opensource/directfb/src/DirectFB-$(DIRECTFB_VER)
		HUMAX_EXTRA_CFLAG += -I$(HUMAX_WORK_DIR)/platform/sdk/driver/AppLibs/opensource/directfb/src/DirectFB-$(DIRECTFB_VER)/include
		HUMAX_EXTRA_CFLAG += -I$(HUMAX_WORK_DIR)/platform/sdk/driver/AppLibs/opensource/directfb/src/DirectFB-$(DIRECTFB_VER)/lib
		HUMAX_EXTRA_CFLAG += -I$(HUMAX_WORK_DIR)/platform/sdk/driver/AppLibs/opensource/directfb/src/DirectFB-Broadcom
		HUMAX_EXTRA_CFLAG += -I$(HUMAX_WORK_DIR)/platform/sdk/driver/AppLibs/opensource/directfb/src/DirectFB-Broadcom/platform
		HUMAX_EXTRA_CFLAG += -DCONFIG_DIRECTFB=1
		HUMAX_EXTRA_CFLAG += -DCONFIG_DIRECTFB_VER_$(subst .,_,$(DIRECTFB_VER))=1
	endif
endif

ifeq ($(CONFIG_SECURITY_EXTENSION),y)
	ifeq ($(NEXUS_SECURITY_TREE), SECURITY_TREE_1290)
		HUMAX_EXTRA_CFLAG += -I$(HUMAX_WORK_DIR)/platform/sdk/driver/nexus/extensions/security/keyladder/include
		HUMAX_EXTRA_CFLAG += -I$(HUMAX_WORK_DIR)/platform/sdk/driver/nexus/extensions/security/otpmsp/include
		HUMAX_EXTRA_CFLAG += -I$(HUMAX_WORK_DIR)/platform/sdk/driver/nexus/extensions/security/usercmd/include
		HUMAX_EXTRA_CFLAG += -I$(HUMAX_WORK_DIR)/platform/sdk/driver/nexus/extensions/security/rawcommand/include
	else ifeq ($(NEXUS_SECURITY_TREE), SECURITY_TREE_1303)
		ifeq ($(filter $(BCHP_CHIP),7422 7425 7429 7344 7346 7231 7435 7563 7358 7360 7552 7584 7543 7145 7445 7366),$(BCHP_CHIP))
			BCHP_CHIP_TYPE = 40nm
		else	# els BCHP_CHIP
			BCHP_CHIP_TYPE = 28nm
		endif	# end BCHP_CHIP
		HUMAX_EXTRA_CFLAG += -I$(HUMAX_WORK_DIR)/platform/sdk/driver/nexus/extensions/security/keyladder/include/$(BCHP_CHIP_TYPE)
		HUMAX_EXTRA_CFLAG += -I$(HUMAX_WORK_DIR)/platform/sdk/driver/nexus/extensions/security/otpmsp/include/$(BCHP_CHIP_TYPE)
		HUMAX_EXTRA_CFLAG += -I$(HUMAX_WORK_DIR)/platform/sdk/driver/nexus/extensions/security/usercmd/include/$(BCHP_CHIP_TYPE)
	else ifeq ($(NEXUS_SECURITY_TREE), SECURITY_TREE_1304)
		ifeq ($(filter $(BCHP_CHIP),7422 7425 7429 7344 7346 7231 7435 7563 7358 7360 7552 7584 7543 7145 7445 7366),$(BCHP_CHIP))
			BCHP_CHIP_TYPE = 40nm
		else	# els BCHP_CHIP
			BCHP_CHIP_TYPE = 28nm
		endif	# end BCHP_CHIP
		HUMAX_EXTRA_CFLAG += -I$(HUMAX_WORK_DIR)/platform/sdk/driver/nexus/extensions/security/keyladder/include/$(BCHP_CHIP_TYPE)
		HUMAX_EXTRA_CFLAG += -I$(HUMAX_WORK_DIR)/platform/sdk/driver/nexus/extensions/security/otpmsp/include/$(BCHP_CHIP_TYPE)
		HUMAX_EXTRA_CFLAG += -I$(HUMAX_WORK_DIR)/platform/sdk/driver/nexus/extensions/security/usercmd/include/$(BCHP_CHIP_TYPE)
	else ifeq ($(NEXUS_SECURITY_TREE), SECURITY_TREE_1401)
		ifeq ($(filter $(BCHP_CHIP),7241 7250 7422 7425 7429 7344 7346 7231 7435 7563 7358 7360 7552 7584 7543 7145 7445 7366 7362),$(BCHP_CHIP))
			BCHP_CHIP_TYPE = 40nm
		else	# els BCHP_CHIP
			BCHP_CHIP_TYPE = 28nm
		endif	# end BCHP_CHIP
		HUMAX_EXTRA_CFLAG += -I$(HUMAX_WORK_DIR)/platform/sdk/driver/nexus/extensions/security/keyladder/include/$(BCHP_CHIP_TYPE)
		HUMAX_EXTRA_CFLAG += -I$(HUMAX_WORK_DIR)/platform/sdk/driver/nexus/extensions/security/otpmsp/include/$(BCHP_CHIP_TYPE)
		HUMAX_EXTRA_CFLAG += -I$(HUMAX_WORK_DIR)/platform/sdk/driver/nexus/extensions/security/usercmd/include/$(BCHP_CHIP_TYPE)
	else ifeq ($(NEXUS_SECURITY_TREE), SECURITY_TREE_1000)
		ifeq ($(filter $(BCHP_CHIP),7422 7425 7429 7344 7346 7231 7435 7563 7358 7360 7552 7584 7543 7145 7445 7366),$(BCHP_CHIP))
			BCHP_CHIP_TYPE = 40nm
		else	# els BCHP_CHIP
			BCHP_CHIP_TYPE = 28nm
		endif	# end BCHP_CHIP
		HUMAX_EXTRA_CFLAG += -I$(HUMAX_WORK_DIR)/platform/sdk/driver/nexus/extensions/security/keyladder/include
		HUMAX_EXTRA_CFLAG += -I$(HUMAX_WORK_DIR)/platform/sdk/driver/nexus/extensions/security/otpmsp/include
		HUMAX_EXTRA_CFLAG += -I$(HUMAX_WORK_DIR)/platform/sdk/driver/nexus/extensions/security/usercmd/include
	else	# els SECURITY_TREE
		HUMAX_EXTRA_CFLAG += -I$(HUMAX_WORK_DIR)/platform/sdk/driver/nexus/extensions/security/keyladder/$(BCM_CHIP)/include
		HUMAX_EXTRA_CFLAG += -I$(HUMAX_WORK_DIR)/platform/sdk/driver/nexus/extensions/security/otpmsp/$(BCM_CHIP)/include
		HUMAX_EXTRA_CFLAG += -I$(HUMAX_WORK_DIR)/platform/sdk/driver/nexus/extensions/security/usercmd/$(BCM_CHIP)/include
	endif # end SECURITY_TREE	
endif # end CONFIG_SECURITY_EXTENSION


ifeq ($(CONFIG_CAS_NA),y)
	NOCS_BCHP_VER = $(shell echo $(CONFIG_CHIP_REV) | tr '[:upper:]' '[:lower:]')
	ifeq ($(NEXUS_SECURITY_TREE), SECURITY_TREE_1290)
		ifeq ($(CONFIG_CAS_NA_NOCS_3X),y)
			NOCS_DIR_PATH = $(NEXUS_TOP)/../rockford/applications/nocs3/$(CONFIG_CAS_NA_NOCS_3X_VERSION)/os
			NOCS_DIR_MAIN_INC = $(NOCS_DIR_PATH)/../include
			NOCS_DIR_CSD_INC = $(NOCS_DIR_PATH)/csd/include
			HUMAX_EXTRA_CFLAG += -I$(NOCS_DIR_MAIN_INC)
			HUMAX_EXTRA_CFLAG += -I$(NOCS_DIR_CSD_INC)
			ifeq ($(CONFIG_CAS_NA_NOCS_CERT),y)
				NOCS_DIR_CERT_INC = $(NOCS_DIR_PATH)/cert/include
				HUMAX_EXTRA_CFLAG += -I$(NOCS_DIR_CERT_INC)
			endif
			ifeq ($(CONFIG_CAS_NA_NOCS_SEC),y)
				NOCS_DIR_SEC_INC = $(NOCS_DIR_PATH)/sec/include
				HUMAX_EXTRA_CFLAG += -I$(NOCS_DIR_SEC_INC)
			endif
		else
			NOCS_DIR_PATH = $(NEXUS_TOP)/../rockford/applications/nocs/api/csd/$(BCHP_CHIP)
		endif
		
		MAGNUM_HSM_PATH = $(NEXUS_TOP)/../magnum/portinginterface/hsm
		MAGNUM_BASE_PATH = $(NEXUS_TOP)/../magnum/basemodules
		
		HUMAX_EXTRA_CFLAG += -I$(MAGNUM_BASE_PATH)/int
		HUMAX_EXTRA_CFLAG += -I$(MAGNUM_BASE_PATH)/mem
		HUMAX_EXTRA_CFLAG += -I$(NOCS_DIR_PATH)
		HUMAX_EXTRA_CFLAG += -I$(HUMAX_WORK_DIR)/platform/sdk/driver/nexus/modules/security/include/priv
		HUMAX_EXTRA_CFLAG += -I$(MAGNUM_HSM_PATH)/src/core40nm/common
		HUMAX_EXTRA_CFLAG += -I$(MAGNUM_HSM_PATH)/src/core40nm/keyladder
		HUMAX_EXTRA_CFLAG += -I$(MAGNUM_HSM_PATH)/src/core40nm/otpmsp
		HUMAX_EXTRA_CFLAG += -I$(MAGNUM_HSM_PATH)/include/core40nm/common/Zeus20
		HUMAX_EXTRA_CFLAG += -I$(MAGNUM_HSM_PATH)/include/core40nm/common/Zeus20/aegis
		HUMAX_EXTRA_CFLAG += -I$(MAGNUM_HSM_PATH)/include/core40nm/common/Zeus20/cust_nagra/aegis
		HUMAX_EXTRA_CFLAG += -I$(HUMAX_WORK_DIR)/platform/sdk/driver/magnum/portinginterface/dma/$(BCHP_CHIP)
		ifeq ($(CONFIG_CAS_NA_NOCS_1X),y)
			HUMAX_EXTRA_CFLAG += -I$(HUMAX_WORK_DIR)/platform/sdk/driver/magnum/syslib/csdlib/$(BCM_CHIP)
		endif
	else ifeq ($(shell expr $(NEXUS_SECURITY_TREE) ">=" SECURITY_TREE_1501),1)
		NOCS_DIR_PATH = $(NEXUS_TOP)/../rockford/applications/security/nagra/nocs3/$(CONFIG_CAS_NA_NOCS_3X_VERSION)/os
		NOCS_DIR_MAIN_INC = $(NOCS_DIR_PATH)/../include
		NOCS_DIR_CSD_INC = $(NOCS_DIR_PATH)/csd/include
		NOCS_DIR_BCSD_INC = $(NOCS_DIR_PATH)/../../../../brcm/bcsd/include
		NOCS_DIR_BCSD_OS_INC = $(NOCS_DIR_PATH)/../../../../brcm/bcsd/core/os/include
		HUMAX_EXTRA_CFLAG += -I$(NOCS_DIR_MAIN_INC)
		HUMAX_EXTRA_CFLAG += -I$(NOCS_DIR_CSD_INC)
		HUMAX_EXTRA_CFLAG += -I$(NOCS_DIR_BCSD_INC)
		HUMAX_EXTRA_CFLAG += -I$(NOCS_DIR_BCSD_OS_INC)
		ifeq ($(CONFIG_CAS_NA_NOCS_CERT),y)
			NOCS_DIR_CERT_INC = $(NOCS_DIR_PATH)/cert/include
			HUMAX_EXTRA_CFLAG += -I$(NOCS_DIR_CERT_INC)
		endif
		ifeq ($(CONFIG_CAS_NA_NOCS_SEC),y)
			NOCS_DIR_SEC_INC = $(NOCS_DIR_PATH)/sec/include
			HUMAX_EXTRA_CFLAG += -I$(NOCS_DIR_SEC_INC)
		endif
		ifeq ($(CONFIG_CAS_NA_NOCS_DPT),y)
			NOCS_DIR_DPT_INC = $(NOCS_DIR_PATH)/dpt/include
			HUMAX_EXTRA_CFLAG += -I$(NOCS_DIR_DPT_INC)
		endif

		MAGNUM_HSM_PATH = $(NEXUS_TOP)/../magnum/portinginterface/hsm
		MAGNUM_BASE_PATH = $(NEXUS_TOP)/../magnum/basemodules

		HUMAX_EXTRA_CFLAG += -I$(MAGNUM_BASE_PATH)/int
		HUMAX_EXTRA_CFLAG += -I$(MAGNUM_BASE_PATH)/mem
		HUMAX_EXTRA_CFLAG += -I$(NOCS_DIR_PATH)
		HUMAX_EXTRA_CFLAG += -I$(HUMAX_WORK_DIR)/platform/sdk/driver/nexus/modules/security/include/priv
		HUMAX_EXTRA_CFLAG += -I$(MAGNUM_HSM_PATH)/src/core40nm/common
		HUMAX_EXTRA_CFLAG += -I$(MAGNUM_HSM_PATH)/src/core40nm/keyladder
		HUMAX_EXTRA_CFLAG += -I$(MAGNUM_HSM_PATH)/src/core40nm/otpmsp
		HUMAX_EXTRA_CFLAG += -I$(MAGNUM_HSM_PATH)/include/core40nm/common/Zeus20
		HUMAX_EXTRA_CFLAG += -I$(MAGNUM_HSM_PATH)/include/core40nm/common/Zeus20/aegis
		HUMAX_EXTRA_CFLAG += -I$(MAGNUM_HSM_PATH)/include/core40nm/common/Zeus20/cust_nagra/aegis
		HUMAX_EXTRA_CFLAG += -I$(HUMAX_WORK_DIR)/platform/sdk/driver/magnum/portinginterface/dma/$(BCHP_CHIP)
	else ifeq ($(shell expr $(NEXUS_SECURITY_TREE) "==" SECURITY_TREE_1401),1)
		ifeq ($(CONFIG_CAS_NA_NOCS_3X),y)
			NOCS_DIR_PATH = $(NEXUS_TOP)/../rockford/applications/security/nagra/nocs3/$(CONFIG_CAS_NA_NOCS_3X_VERSION)/os
			NOCS_DIR_MAIN_INC = $(NOCS_DIR_PATH)/../include
			NOCS_DIR_CSD_INC = $(NOCS_DIR_PATH)/csd/include
			NOCS_DIR_BCSD_INC = $(NOCS_DIR_PATH)/../../../../brcm/bcsd/include
			NOCS_DIR_BCSD_OS_INC = $(NOCS_DIR_PATH)/../../../../brcm/bcsd/core/os/include
			HUMAX_EXTRA_CFLAG += -I$(NOCS_DIR_MAIN_INC)
			HUMAX_EXTRA_CFLAG += -I$(NOCS_DIR_CSD_INC)
			HUMAX_EXTRA_CFLAG += -I$(NOCS_DIR_BCSD_INC)
			HUMAX_EXTRA_CFLAG += -I$(NOCS_DIR_BCSD_OS_INC)
			ifeq ($(CONFIG_CAS_NA_NOCS_CERT),y)
				NOCS_DIR_CERT_INC = $(NOCS_DIR_PATH)/cert/include
				HUMAX_EXTRA_CFLAG += -I$(NOCS_DIR_CERT_INC)
			endif
			ifeq ($(CONFIG_CAS_NA_NOCS_SEC),y)
				NOCS_DIR_SEC_INC = $(NOCS_DIR_PATH)/sec/include
				HUMAX_EXTRA_CFLAG += -I$(NOCS_DIR_SEC_INC)
			endif
			ifeq ($(CONFIG_CAS_NA_NOCS_DPT),y)
				NOCS_DIR_DPT_INC = $(NOCS_DIR_PATH)/dpt/include
				HUMAX_EXTRA_CFLAG += -I$(NOCS_DIR_DPT_INC)
			endif
	
			MAGNUM_HSM_PATH = $(NEXUS_TOP)/../magnum/portinginterface/hsm
			MAGNUM_BASE_PATH = $(NEXUS_TOP)/../magnum/basemodules
	
			HUMAX_EXTRA_CFLAG += -I$(MAGNUM_BASE_PATH)/int
			HUMAX_EXTRA_CFLAG += -I$(MAGNUM_BASE_PATH)/mem
			HUMAX_EXTRA_CFLAG += -I$(NOCS_DIR_PATH)
			HUMAX_EXTRA_CFLAG += -I$(HUMAX_WORK_DIR)/platform/sdk/driver/nexus/modules/security/include/priv
			HUMAX_EXTRA_CFLAG += -I$(MAGNUM_HSM_PATH)/src/core40nm/common
			HUMAX_EXTRA_CFLAG += -I$(MAGNUM_HSM_PATH)/src/core40nm/keyladder
			HUMAX_EXTRA_CFLAG += -I$(MAGNUM_HSM_PATH)/src/core40nm/otpmsp
			HUMAX_EXTRA_CFLAG += -I$(MAGNUM_HSM_PATH)/include/core40nm/common/Zeus20
			HUMAX_EXTRA_CFLAG += -I$(MAGNUM_HSM_PATH)/include/core40nm/common/Zeus20/aegis
			HUMAX_EXTRA_CFLAG += -I$(MAGNUM_HSM_PATH)/include/core40nm/common/Zeus20/cust_nagra/aegis
			HUMAX_EXTRA_CFLAG += -I$(HUMAX_WORK_DIR)/platform/sdk/driver/magnum/portinginterface/dma/$(BCHP_CHIP)
		else
			HUMAX_EXTRA_CFLAG += -I$(HUMAX_WORK_DIR)/platform/sdk/driver/magnum/syslib/bsdlib/$(BCM_CHIP)
			HUMAX_EXTRA_CFLAG += -I$(HUMAX_WORK_DIR)/platform/sdk/driver/magnum/syslib/csdlib/$(BCM_CHIP)
			HUMAX_EXTRA_CFLAG += -I$(HUMAX_WORK_DIR)/platform/sdk/driver/magnum/syslib/ctdlib/$(BCM_CHIP)
			HUMAX_EXTRA_CFLAG += -I$(HUMAX_WORK_DIR)/platform/sdk/driver/nexus/modules/security/$(BCM_CHIP)/include/priv
			HUMAX_EXTRA_CFLAG += -I$(HUMAX_WORK_DIR)/platform/sdk/driver/magnum/portinginterface/hsm/$(BCM_CHIP)/$(NOCS_BCHP_VER)/common
			HUMAX_EXTRA_CFLAG += -I$(HUMAX_WORK_DIR)/platform/sdk/driver/magnum/basemodules/int
			HUMAX_EXTRA_CFLAG += -I$(HUMAX_WORK_DIR)/platform/sdk/driver/magnum/basemodules/mem
			HUMAX_EXTRA_CFLAG += -I$(HUMAX_WORK_DIR)/platform/sdk/driver/magnum/portinginterface/hsm/$(BCM_CHIP)/$(NOCS_BCHP_VER)/common/aegis
			HUMAX_EXTRA_CFLAG += -I$(HUMAX_WORK_DIR)/platform/sdk/driver/magnum/portinginterface/hsm/$(BCM_CHIP)/$(NOCS_BCHP_VER)/cust_nagra/aegis
			HUMAX_EXTRA_CFLAG += -I$(HUMAX_WORK_DIR)/platform/sdk/driver/magnum/portinginterface/hsm/$(BCM_CHIP)/$(NOCS_BCHP_VER)/keyladder
			HUMAX_EXTRA_CFLAG += -I$(HUMAX_WORK_DIR)/platform/sdk/driver/magnum/portinginterface/hsm/$(BCM_CHIP)/$(NOCS_BCHP_VER)/otpmsp
			HUMAX_EXTRA_CFLAG += -I$(HUMAX_WORK_DIR)/platform/sdk/driver/magnum/portinginterface/dma/$(BCM_CHIP)
		endif
	else
		HUMAX_EXTRA_CFLAG += -I$(HUMAX_WORK_DIR)/platform/sdk/driver/magnum/syslib/bsdlib/$(BCM_CHIP)
		HUMAX_EXTRA_CFLAG += -I$(HUMAX_WORK_DIR)/platform/sdk/driver/magnum/syslib/csdlib/$(BCM_CHIP)
		HUMAX_EXTRA_CFLAG += -I$(HUMAX_WORK_DIR)/platform/sdk/driver/magnum/syslib/ctdlib/$(BCM_CHIP)
		HUMAX_EXTRA_CFLAG += -I$(HUMAX_WORK_DIR)/platform/sdk/driver/nexus/modules/security/$(BCM_CHIP)/include/priv
		HUMAX_EXTRA_CFLAG += -I$(HUMAX_WORK_DIR)/platform/sdk/driver/magnum/portinginterface/hsm/$(BCM_CHIP)/$(NOCS_BCHP_VER)/common
		HUMAX_EXTRA_CFLAG += -I$(HUMAX_WORK_DIR)/platform/sdk/driver/magnum/basemodules/int
		HUMAX_EXTRA_CFLAG += -I$(HUMAX_WORK_DIR)/platform/sdk/driver/magnum/basemodules/mem
		HUMAX_EXTRA_CFLAG += -I$(HUMAX_WORK_DIR)/platform/sdk/driver/magnum/portinginterface/hsm/$(BCM_CHIP)/$(NOCS_BCHP_VER)/common/aegis
		HUMAX_EXTRA_CFLAG += -I$(HUMAX_WORK_DIR)/platform/sdk/driver/magnum/portinginterface/hsm/$(BCM_CHIP)/$(NOCS_BCHP_VER)/cust_nagra/aegis
		HUMAX_EXTRA_CFLAG += -I$(HUMAX_WORK_DIR)/platform/sdk/driver/magnum/portinginterface/hsm/$(BCM_CHIP)/$(NOCS_BCHP_VER)/keyladder
		HUMAX_EXTRA_CFLAG += -I$(HUMAX_WORK_DIR)/platform/sdk/driver/magnum/portinginterface/hsm/$(BCM_CHIP)/$(NOCS_BCHP_VER)/otpmsp
		HUMAX_EXTRA_CFLAG += -I$(HUMAX_WORK_DIR)/platform/sdk/driver/magnum/portinginterface/dma/$(BCM_CHIP)
	endif
endif

ifeq ($(CONFIG_CAS_BCSD),y)
	BCSD_DIR_PATH = $(NEXUS_TOP)/../rockford/applications/security/conax/os
	BCSD_DIR_CONAX_INC = $(BCSD_DIR_PATH)/include
	BCSD_DIR_CONAX_SRC = $(BCSD_DIR_PATH)/src
	BCSD_DIR_BCSD_INC = $(BCSD_DIR_PATH)/../../brcm/bcsd/include
	BCSD_DIR_BCSD_OS_INC = $(BCSD_DIR_PATH)/../../brcm/bcsd/core/os/include
	BCSD_DIR_BCSD_CONFIG_CONAX_OS_INC = $(BCSD_DIR_PATH)/../../brcm/bcsd/configs/conax/os/include
	BCSD_DIR_BCSD_CONFIG_COMMON_INC = $(BCSD_DIR_PATH)/../../brcm/bcsd/configs/common/include

	HUMAX_EXTRA_CFLAG += -I$(BCSD_DIR_CONAX_INC)
	HUMAX_EXTRA_CFLAG += -I$(BCSD_DIR_CONAX_SRC)
	HUMAX_EXTRA_CFLAG += -I$(BCSD_DIR_BCSD_INC)
	HUMAX_EXTRA_CFLAG += -I$(BCSD_DIR_BCSD_OS_INC)
	HUMAX_EXTRA_CFLAG += -I$(BCSD_DIR_BCSD_CONFIG_CONAX_OS_INC)
	HUMAX_EXTRA_CFLAG += -I$(BCSD_DIR_BCSD_CONFIG_COMMON_INC)

	MAGNUM_HSM_PATH = $(NEXUS_TOP)/../magnum/portinginterface/hsm
	MAGNUM_BASE_PATH = $(NEXUS_TOP)/../magnum/basemodules

	HUMAX_EXTRA_CFLAG += -I$(MAGNUM_BASE_PATH)/int
	HUMAX_EXTRA_CFLAG += -I$(MAGNUM_BASE_PATH)/mem
	HUMAX_EXTRA_CFLAG += -I$(BCSD_DIR_PATH)/src
	HUMAX_EXTRA_CFLAG += -I$(HUMAX_WORK_DIR)/platform/sdk/driver/nexus/modules/security/include/priv
	HUMAX_EXTRA_CFLAG += -I$(MAGNUM_HSM_PATH)/include/common
	HUMAX_EXTRA_CFLAG += -I$(MAGNUM_HSM_PATH)/include/keyladder
	HUMAX_EXTRA_CFLAG += -I$(MAGNUM_HSM_PATH)/include/otpmsp
	HUMAX_EXTRA_CFLAG += -I$(MAGNUM_HSM_PATH)/include/common/core28nm
	HUMAX_EXTRA_CFLAG += -I$(HUMAX_WORK_DIR)/platform/sdk/driver/magnum/portinginterface/dma/
endif

ifeq ($(CONFIG_MARLIN_DRM),y)
	HUMAX_EXTRA_CFLAG += -I$(HUMAX_WORK_DIR)/../packages/marlin/include
endif

HUMAX_EXTRA_CFLAG += -I$(HUMAX_WORK_DIR)/../packages/libext2fs/include
HUMAX_EXTRA_CFLAG += -DNEXUS_VERSION=$(NEXUS_VERSION)
HUMAX_EXTRA_CFLAG += -DNEXUS_VERSION_MINOR=$(NEXUS_VERSION_MINOR)

ifeq ($(CONFIG_MEDIA20),y)
	HUMAX_EXTRA_CFLAG += -I$(HUMAX_WORK_DIR)/../packages/openssl/include
	HUMAX_EXTRA_CFLAG += -I$(HUMAX_WORK_DIR)/../3rd_party/dlna/mxlib/mxdtcp/include/
	HUMAX_EXTRA_CFLAG += -I$(HUMAX_WORK_DIR)/../3rd_party/dlna/mxlib/mxdtcp/
	HUMAX_EXTRA_CFLAG += -I$(HUMAX_WORK_DIR)/../3rd_party/dlna/mxlib/mxkernel/
	HUMAX_EXTRA_CFLAG += -I$(HUMAX_WORK_DIR)/../3rd_party/dlna/mxlib/
endif
ifeq ($(CONFIG_OPENSSL),y)
	HUMAX_EXTRA_CFLAG += -I$(HUMAX_WORK_DIR)/../include/
endif

ifeq ($(CONFIG_DEVICE_MODE_KERNEL),y)
	HUMAX_EXTRA_CFLAG += -DCONFIG_DEVICE_MODE_KERNEL=1
endif

ifneq ($(CONFIG_PRODUCT_IMAGE_OCTO20)$(CONFIG_PRODUCT_IMAGE_NOAPP),)
ifneq ($(CONFIG_PRODUCT_DI_DEBUG_LOG),)
ifeq ($(CONFIG_OS_LINUX),y)
	HUMAX_EXTRA_CFLAG += -DCONFIG_TELNET_CMD
endif
endif
endif

ifeq ($(CONFIG_NETAPP),y)
	#HUMAX_EXTRA_CFLAG += -I$(HUMAX_WORK_DIR)/platform/sdk/os/drivers/NETAPP/stage/usr/local/include
	#export -p
	ifneq ($(wildcard ${NETAPP_OUTPUT_DIR}/include/netapp.inc),)
		include ${NETAPP_OUTPUT_DIR}/include/netapp.inc
		HUMAX_EXTRA_CFLAG += $(filter-out -fPIC, ${NETAPP_CFLAGS})
	endif
endif
 
ifeq ($(CONFIG_BCM_UART),y)
	ifeq ($(NEXUS_TREE), TREE_1303)
		HUMAX_EXTRA_CFLAG += -I$(HUMAX_WORK_DIR)/platform/sdk/driver/nexus/modules/uart/include
	else
		HUMAX_EXTRA_CFLAG += -I$(HUMAX_WORK_DIR)/platform/sdk/driver/nexus/modules/uart/$(BCM_CHIP)/include
	endif
endif

ifeq ($(CONFIG_DINETWORK),y)
	ifeq ($(CONFIG_BLUETOOTH),y)
		ifeq ($(CONFIG_SWB_B70_BT),y)
			HUMAX_EXTRA_CFLAG += -I$(HUMAX_WORK_DIR)/platform/sdk/os/drivers/bluetooth/SWB-B70/3rdparty/embedded/bsa_examples/linux/libbsa/include
		else
			HUMAX_EXTRA_CFLAG += -I$(HUMAX_WORK_DIR)/platform/sdk/os/drivers/bluetooth/bsa/3rdparty/embedded/bsa_examples/linux/libbsa/include
		endif
	endif
endif

ifeq ($(CONFIG_OS_UCOS),y)
	
ROCKFORD_DIR=$(HUMAX_WORK_DIR)/platform/sdk/driver/rockford
	
HUMAX_EXTRA_CFLAG += \
		-I$(HUMAX_WORK_DIR)/platform/sdk/driver/BSEAV/app/bsu/src

HUMAX_EXTRA_CFLAG += \
        -I$(ROCKFORD_DIR)/bsp/common/ucos_iii/Micrium/Software/uCOS-III/Source \
        -I$(ROCKFORD_DIR)/bsp/common/ucos_iii/Micrium/Software/uCOS-III/Cfg/Template \
        -I$(ROCKFORD_DIR)/bsp/common/ucos_iii/Micrium/Software/uC-CPU \
        -I$(ROCKFORD_DIR)/bsp/common/ucos_iii/Micrium/Software/uC-CPU/Cfg/Template \
        -I$(ROCKFORD_DIR)/bsp/common/ucos_iii/Micrium/Software/uC-LIB

ifeq ($(CONFIG_ARM), y)
HUMAX_EXTRA_CFLAG += \
        -I$(ROCKFORD_DIR)/bsp/common/ucos_iii/Micrium/Software/uCOS-III/Ports/ARM-Cortex-A15/Generic/GNU \
        -I$(ROCKFORD_DIR)/bsp/common/ucos_iii/Micrium/Software/uC-CPU/ARM-Cortex-A15/GNU \
		-I$(HUMAX_WORK_DIR)/platform/sdk/driver/BOLT/include
else
HUMAX_EXTRA_CFLAG += \
        -I$(ROCKFORD_DIR)/bsp/common/ucos_iii/Micrium/Software/uCOS-III/Ports/Broadcom-MIPS/SDE \
        -I$(ROCKFORD_DIR)/bsp/common/ucos_iii/Micrium/Software/uC-CPU/Broadcom-MIPS \
		-I$(HUMAX_WORK_DIR)/platform/sdk/driver/CFE/cfe/include \
		-I$(HUMAX_WORK_DIR)/platform/sdk/driver/CFE/cfe/arch/include
endif
endif

HUMAX_EXTRA_CFLAG += -DLINUX_VERSION=$(LINUX_VERSION)
HUMAX_EXTRA_CFLAG += -DLINUX_PATCHLEVEL=$(LINUX_PATCHLEVEL)
HUMAX_EXTRA_CFLAG += -DLINUX_SUBLEVEL=$(LINUX_SUBLEVEL)
HUMAX_EXTRA_CFLAG += -DLINUX_EXTRAVERSION=$(LINUX_EXTRAVERSION)

HUMAX_CONFIG_C_FILE_SRC := $(HUMAX_PRODUCT_CONFIG_DIR)/platform/driver/$(CONFIG_PRODUCT_NAME)/$(CONFIG_BOARD_REVISION)/di_config.c
HUMAX_CONFIG_H_FILE_SRC := $(HUMAX_PRODUCT_CONFIG_DIR)/platform/driver/$(CONFIG_PRODUCT_NAME)/$(CONFIG_BOARD_REVISION)/di_config.h
HUMAX_CONFIG_C_FILE_COPY := $(HUMAX_WORK_DIR)/platform/driver/di/di_config.c
HUMAX_CONFIG_H_FILE_COPY := $(HUMAX_WORK_DIR)/platform/driver/di/di_config.h

ifdef CONFIG_DI_SHARED_LIB
	HUMAX_CONFIG_C_FILE_OBJ := $(HUMAX_WORK_DIR)/platform/driver/di/di_config.o
endif

export HUMAX_EXTRA_CFLAG
export HUMAX_MODULE=DRIVER

include $(HUMAX_PORTING_MAKE_DIR)/HumaxToolset.mak

driver: copy_config_c_file
ifdef CONFIG_DI_SHARED_LIB
	$(HUMAX_RM) $(HUMAX_CONFIG_C_FILE_OBJ)
endif
	@test -f $(HUMAX_WORK_DIR)/platform/driver/di/drv/include/i2c_bus_index.h || $(HUMAX_CP) $(DEVICE_CONFIG_DIR)/$(CONFIG_PRODUCT_NAME)/$(CONFIG_BOARD_REVISION)/i2c_bus_index.h $(HUMAX_WORK_DIR)/platform/driver/di/drv/include/i2c_bus_index.h
	@test -f $(HUMAX_WORK_DIR)/platform/driver/di/drv/include/flash_map.h || $(HUMAX_CP) $(FLASHMAP_CONFIG_DIR)/$(CONFIG_PRODUCT_NAME)/$(CONFIG_BOARD_REVISION)/flash_map.h $(HUMAX_WORK_DIR)/platform/driver/di/drv/include/flash_map.h
	-@test -f $(HUMAX_WORK_DIR)/platform/driver/di/drv/include/ram_map.h || $(HUMAX_CP) $(RAMMAP_CONFIG_DIR)/$(CONFIG_PRODUCT_NAME)/$(CONFIG_BOARD_REVISION)/ram_map.h $(HUMAX_WORK_DIR)/platform/driver/di/drv/include/ram_map.h
	$(HUMAX_MAKE) -f $(HUMAX_MAKE_DIR)/HumaxDefaultRule.mak LIB_TYPE=STATIC HUMAX_OBJ='$(obj-y)' HUMAX_CONFIG_DIR="$(HUMAX_MAKE_DIR)/$(CONFIG_PRODUCT_NAME)" V=$(V) LIBNAME=$@ static-library

clean_driver:
	$(HUMAX_MAKE) -f $(HUMAX_MAKE_DIR)/HumaxDefaultRule.mak LIB_TYPE=STATIC HUMAX_OBJ='$(obj-y)' HUMAX_CONFIG_DIR="$(HUMAX_MAKE_DIR)/$(CONFIG_PRODUCT_NAME)" clean
	$(HUMAX_RM) -f libdriver.a
ifdef CONFIG_DI_SHARED_LIB
	$(HUMAX_RM) $(HUMAX_CONFIG_C_FILE_OBJ)
endif
	$(HUMAX_RM) $(HUMAX_CONFIG_C_FILE_COPY)
	$(HUMAX_RM) $(HUMAX_CONFIG_H_FILE_COPY)
	$(HUMAX_RM) $(HUMAX_WORK_DIR)/platform/driver/di/drv/include/i2c_bus_index.h
	$(HUMAX_RM) $(HUMAX_WORK_DIR)/platform/driver/di/drv/include/humax_*
	$(HUMAX_RM) $(HUMAX_WORK_DIR)/platform/driver/di/drv/include/flash_map.h
	-$(HUMAX_RM) $(HUMAX_WORK_DIR)/platform/driver/di/drv/include/ram_map.h

copy_config_c_file: di/$(notdir $(HUMAX_CONFIG_C_FILE_SRC)) di/$(notdir $(HUMAX_CONFIG_H_FILE_SRC))
ifeq ($(CONFIG_DI_MODULE_NETWORK_SHARED_LIB),y)
	$(HUMAX_CP) -f $(HUMAX_PRODUCT_CONFIG_DIR)/platform/driver/$(CONFIG_PRODUCT_NAME)/$(CONFIG_BOARD_REVISION)/di_config_network.c $(HUMAX_DRIVER_DIR)/di/di_config_network.c	
endif
ifeq ($(CONFIG_DI_MODULE_WIFI_SHARED_LIB),y)
	$(HUMAX_CP) -f $(HUMAX_PRODUCT_CONFIG_DIR)/platform/driver/$(CONFIG_PRODUCT_NAME)/$(CONFIG_BOARD_REVISION)/di_config_network.c $(HUMAX_DRIVER_DIR)/di/di_config_network.c
endif
ifeq ($(CONFIG_DI_MODULE_PPPOE_SHARED_LIB),y)
	$(HUMAX_CP) -f $(HUMAX_PRODUCT_CONFIG_DIR)/platform/driver/$(CONFIG_PRODUCT_NAME)/$(CONFIG_BOARD_REVISION)/di_config_network.c $(HUMAX_DRIVER_DIR)/di/di_config_network.c
endif

di/$(notdir $(HUMAX_CONFIG_C_FILE_SRC)) : $(HUMAX_CONFIG_C_FILE_SRC)
	$(HUMAX_RM) $(HUMAX_CONFIG_C_FILE_COPY)
	$(HUMAX_CP) $< $@

di/$(notdir $(HUMAX_CONFIG_H_FILE_SRC)) : $(HUMAX_CONFIG_H_FILE_SRC)
	$(HUMAX_RM) $(HUMAX_CONFIG_H_FILE_COPY)
	$(HUMAX_CP) $< $@

$(HUMAX_FLASHMAP_DRV_FILE): $(HUMAX_FLASHMAP_FILE)
	$(HUMAX_CP) $< $@

$(HUMAX_RAMMAP_DRV_FILE): $(HUMAX_RAMMAP_FILE)
	$(HUMAX_CP) $< $@

$(HUMAX_I2C_DRV_FILE): $(HUMAX_I2C_FILE)
	$(HUMAX_CP) $< $@

#################### vkernel ##########################
TARGET_GOAL := vkernel 
ifeq ($(filter-out $(TARGET_GOAL),$(MAKECMDGOALS)),)
HUMAX_EXTRA_CFLAG += -fPIC
endif

vkernel: FORCE
	$(Q_)echo "Compiling vkernel for Shared Library!!!"
	$(HUMAX_MAKE) -f $(HUMAX_MAKE_DIR)/HumaxDefaultRule.mak LIB_TYPE=SHARED HUMAX_OBJ='$(obj-vkernel-y)' HUMAX_CONFIG_DIR="$(HUMAX_MAKE_DIR)/$(CONFIG_PRODUCT_NAME)" V=$(V) LIBNAME=$@ shared-library

vkernel-clean:
	$(HUMAX_MAKE) -f $(HUMAX_MAKE_DIR)/HumaxDefaultRule.mak LIB_TYPE=SHARED HUMAX_OBJ='$(obj-vkernel-y)' HUMAX_CONFIG_DIR="$(HUMAX_MAKE_DIR)/$(CONFIG_PRODUCT_NAME)" V=$(V) clean
	$(HUMAX_RM) -f libvkernel.so

vkernel-distclean: vkernel-clean

################## DI Server/Client Module ###################
TARGET_GOAL := di_servcli
ifeq ($(filter-out $(TARGET_GOAL),$(MAKECMDGOALS)),)
HUMAX_EXTRA_CFLAG += -fPIC
endif

obj-power_server-y := di/power/server_api/
obj-power_client-y := di/power/client_api/

di_servcli: FORCE
	$(Q_)echo "Compiling di server/client modules!!!"
	$(HUMAX_MAKE) -f $(HUMAX_MAKE_DIR)/HumaxDefaultRule.mak LIB_TYPE=SHARED HUMAX_OBJ='$(obj-power_server-y)' HUMAX_CONFIG_DIR="$(HUMAX_MAKE_DIR)/$(CONFIG_PRODUCT_NAME)" V=$(V) LIBNAME=di_power_server shared-library
	$(HUMAX_MAKE) -f $(HUMAX_MAKE_DIR)/HumaxDefaultRule.mak LIB_TYPE=SHARED HUMAX_OBJ='$(obj-power_client-y)' HUMAX_CONFIG_DIR="$(HUMAX_MAKE_DIR)/$(CONFIG_PRODUCT_NAME)" V=$(V) LIBNAME=di_power_client shared-library

di_servcli-clean:
	$(HUMAX_MAKE) -f $(HUMAX_MAKE_DIR)/HumaxDefaultRule.mak LIB_TYPE=SHARED HUMAX_OBJ='$(obj-power_server-y)' HUMAX_CONFIG_DIR="$(HUMAX_MAKE_DIR)/$(CONFIG_PRODUCT_NAME)" V=$(V) clean
	$(HUMAX_MAKE) -f $(HUMAX_MAKE_DIR)/HumaxDefaultRule.mak LIB_TYPE=SHARED HUMAX_OBJ='$(obj-power_client-y)' HUMAX_CONFIG_DIR="$(HUMAX_MAKE_DIR)/$(CONFIG_PRODUCT_NAME)" V=$(V) clean
	$(HUMAX_RM) libdi_*_client.so
	$(HUMAX_RM) libdi_*_server.so

di_servcli-distclean: di_servcli-clean

#################### volume manager ##########################
TARGET_GOAL := dbus_lib_volmgr 
ifeq ($(filter-out $(TARGET_GOAL),$(MAKECMDGOALS)),)
HUMAX_EXTRA_CFLAG += -fPIC
endif

dbus_lib_volmgr: FORCE
	$(Q_)echo "Compiling dbus_lib_volmgr for Shared Library!!!"
	$(HUMAX_MAKE) -f $(HUMAX_MAKE_DIR)/HumaxDefaultRule.mak LIB_TYPE=SHARED HUMAX_OBJ='$(obj-dbus_lib_volmgr-y)' HUMAX_CONFIG_DIR="$(HUMAX_MAKE_DIR)/$(CONFIG_PRODUCT_NAME)" V=$(V) LIBNAME=$@ shared-library

dbus_lib_volmgr-clean:
	$(HUMAX_MAKE) -f $(HUMAX_MAKE_DIR)/HumaxDefaultRule.mak LIB_TYPE=SHARED HUMAX_OBJ='$(obj-dbus_lib_volmgr-y)' HUMAX_CONFIG_DIR="$(HUMAX_MAKE_DIR)/$(CONFIG_PRODUCT_NAME)" V=$(V) clean
	$(HUMAX_RM) -f libdbus_volmgr.so

dbus_lib_volmgr-distclean: dbus_lib_volmgr-clean

ifdef CONFIG_CALYPSO
#################### mi ##########################
TARGET_GOAL := mi
ifeq ($(filter-out $(TARGET_GOAL),$(MAKECMDGOALS)),)
	HUMAX_EXTRA_CFLAG += -fPIC
endif

mi: FORCE
	$(Q_)echo "Compiling mi for Shared Library!!!"
	$(HUMAX_MAKE) -f $(HUMAX_MAKE_DIR)/HumaxDefaultRule.mak LIB_TYPE=SHARED HUMAX_OBJ='$(obj-mi-y)' HUMAX_CONFIG_DIR="$(HUMAX_MAKE_DIR)/$(CONFIG_PRODUCT_NAME)" V=$(V) LIBNAME=$@ shared-library

mi-clean:
	$(HUMAX_MAKE) -f $(HUMAX_MAKE_DIR)/HumaxDefaultRule.mak LIB_TYPE=SHARED HUMAX_OBJ='$(obj-mi-y)' HUMAX_CONFIG_DIR="$(HUMAX_MAKE_DIR)/$(CONFIG_PRODUCT_NAME)" V=$(V) clean
	$(HUMAX_RM) -f libmi.so

mi-distclean: mi-clean
endif

ifdef CONFIG_DI_DEMUX_EXT
#################### DI Demux Extension ##########################
TARGET_GOAL := didemux_ext
ifeq ($(filter-out $(TARGET_GOAL),$(MAKECMDGOALS)),)
        HUMAX_EXTRA_CFLAG += -fPIC
endif

didemux_ext: FORCE
	$(Q_)echo "Compiling DI Demux Extension for Shared Library!!!"
	$(HUMAX_MAKE) -f $(HUMAX_MAKE_DIR)/HumaxDefaultRule.mak LIB_TYPE=SHARED HUMAX_OBJ='$(obj-didemux_ext-y)' HUMAX_CONFIG_DIR="$(HUMAX_MAKE_DIR)/$(CONFIG_PRODUCT_NAME)" V=$(V) LIBNAME=$@ shared-library

didemux_ext-clean:
	$(HUMAX_MAKE) -f $(HUMAX_MAKE_DIR)/HumaxDefaultRule.mak LIB_TYPE=SHARED HUMAX_OBJ='$(obj-didemux_ext-y)' HUMAX_CONFIG_DIR="$(HUMAX_MAKE_DIR)/$(CONFIG_PRODUCT_NAME)" V=$(V) clean
	$(HUMAX_RM) -f libdidemux_ext.so

didemux_ext-distclean: didemux_ext-clean
endif

ifdef CONFIG_DI_SHARED_LIB
#################### libdi ##########################
TARGET_GOAL := di_so 
ifeq ($(filter-out $(TARGET_GOAL),$(MAKECMDGOALS)),)
HUMAX_EXTRA_CFLAG += -fPIC
endif

di_so: copy_config_c_file
	$(Q_)echo "Compiling libdi for Shared Library!!!"
	$(HUMAX_RM) $(HUMAX_CONFIG_C_FILE_OBJ)
	$(HUMAX_MAKE) -f $(HUMAX_MAKE_DIR)/HumaxDefaultRule.mak LIB_TYPE=SHARED HUMAX_OBJ='$(obj-di_so-y)' HUMAX_CONFIG_DIR="$(HUMAX_MAKE_DIR)/$(CONFIG_PRODUCT_NAME)" V=$(V) LIBNAME="di" shared-library

di_so-clean:
	$(HUMAX_MAKE) -f $(HUMAX_MAKE_DIR)/HumaxDefaultRule.mak LIB_TYPE=SHARED HUMAX_OBJ='$(obj-di_so-y)' HUMAX_CONFIG_DIR="$(HUMAX_MAKE_DIR)/$(CONFIG_PRODUCT_NAME)" V=$(V) so_clean
	$(HUMAX_RM) $(HUMAX_CONFIG_C_FILE_OBJ)
	$(HUMAX_RM) $(HUMAX_CONFIG_C_FILE_COPY)
	$(HUMAX_RM) $(HUMAX_CONFIG_H_FILE_COPY)
	$(HUMAX_RM) -f di_so.so

di_so-distclean: di_so-clean
endif

############ DI Module Shared Library ###################
## Network ##############################################
ifeq ($(filter-out di_so_network,$(MAKECMDGOALS)),)
HUMAX_EXTRA_CFLAG += -fPIC
endif

NETWORK_SHARED_OBJ := $(HUMAX_DRIVER_DIR)/di/network/di_network.o
NETWORK_SHARED_OBJ += $(HUMAX_DRIVER_DIR)/di/di_config_network.o

NETWORK_SHAREDLIB_NAME := di_network

prepare_network_sharedlib:
	$(HUMAX_CP) -f $(HUMAX_PRODUCT_CONFIG_DIR)/platform/driver/$(CONFIG_PRODUCT_NAME)/$(CONFIG_BOARD_REVISION)/di_config_network.c \
					$(HUMAX_DRIVER_DIR)/di

di_so_network: prepare_network_sharedlib
	$(Q_)echo "Compiling libdi_network.so for Shared Library!!!"
	$(HUMAX_MAKE) -f $(HUMAX_MAKE_DIR)/HumaxDefaultRule.mak LIB_TYPE=SHARED HUMAX_OBJ="$(NETWORK_SHARED_OBJ)" \
					HUMAX_CONFIG_DIR="$(HUMAX_MAKE_DIR)/$(CONFIG_PRODUCT_NAME)" V=$(V) LIBNAME=$(NETWORK_SHAREDLIB_NAME) shared-library

di_so_network-clean:
	$(HUMAX_RM) $(NETWORK_SHARED_OBJ)
	$(HUMAX_RM) $(HUMAX_DRIVER_DIR)/lib$(NETWORK_SHAREDLIB_NAME).so

di_so_network-distclean: di_so_network-clean

## WIFI #################################################
ifeq ($(filter-out di_so_wifi,$(MAKECMDGOALS)),)
HUMAX_EXTRA_CFLAG += -fPIC
endif

WIFI_SHARED_OBJ := $(HUMAX_DRIVER_DIR)/di/network/di_wifi.o
WIFI_SHARED_OBJ += $(HUMAX_DRIVER_DIR)/di/di_config_network.o
WIFI_SHARED_OBJ += $(HUMAX_DRIVER_DIR)/di/drv/wireless/drv_wireless_hotplug.o

WIFI_SHAREDLIB_NAME := di_wifi

prepare_wifi_sharedlib:
	$(HUMAX_CP) -f $(HUMAX_PRODUCT_CONFIG_DIR)/platform/driver/$(CONFIG_PRODUCT_NAME)/$(CONFIG_BOARD_REVISION)/di_config_network.c $(HUMAX_DRIVER_DIR)/di

di_so_wifi: prepare_wifi_sharedlib
	$(Q_)echo "Compiling libdi_wifi.so for Shared Library!!!"
	$(HUMAX_MAKE) -f $(HUMAX_MAKE_DIR)/HumaxDefaultRule.mak LIB_TYPE=SHARED HUMAX_OBJ="$(WIFI_SHARED_OBJ)" HUMAX_CONFIG_DIR="$(HUMAX_MAKE_DIR)/$(CONFIG_PRODUCT_NAME)" V=$(V) LIBNAME=$(WIFI_SHAREDLIB_NAME) shared-library

di_so_wifi-clean:
	$(HUMAX_RM) $(WIFI_SHARED_OBJ)
	$(HUMAX_RM) $(HUMAX_DRIVER_DIR)/lib$(WIFI_SHAREDLIB_NAME).so

di_so_wifi-distclean: di_so_wifi-clean

## BT ###################################################
ifeq ($(filter-out di_so_bt,$(MAKECMDGOALS)),)
HUMAX_EXTRA_CFLAG += -fPIC
endif

ifneq ($(CONFIG_BLUETOOTH_CHIP),)
HUMAX_EXTRA_CFLAG += -D$(CONFIG_BLUETOOTH_CHIP)
HUMAX_EXTRA_CFLAG += -DBT_CHIP="\"$(CONFIG_BLUETOOTH_CHIP)\""
endif
HUMAX_EXTRA_CFLAG += -DBT_FIRMWARE_PATH="\"/lib/firmware/brcm\""
ifeq ($(CONFIG_SWB_B70_BT), y)
HUMAX_EXTRA_CFLAG += -DBT_DEVNAME="\"/dev/btusb0\""
else
ifeq ($(CONFIG_BLUETOOTH_INTERFACE), "USB")
HUMAX_EXTRA_CFLAG += -DBT_DEVNAME="\"/dev/btusb0\""
else
endif
endif
ifeq ($(CONFIG_NETAPP_BLUETOOTH),y)
HUMAX_EXTRA_CFLAG += -DBT_DEVNAME="\"$(CONFIG_NETAPP_BTDEV_PATH)\""
endif

BT_SHARED_OBJ := $(HUMAX_DRIVER_DIR)/di/network/di_bt.o
BT_SHARED_OBJ += $(HUMAX_DRIVER_DIR)/di/network/di_bt_audio.o

BT_SHAREDLIB_NAME := di_bt

di_so_bt: di_so_bt-clean
	$(Q_)echo "Compiling libdi_bt.so for Shared Library!!!"
	$(HUMAX_MAKE) -f $(HUMAX_MAKE_DIR)/HumaxDefaultRule.mak LIB_TYPE=SHARED HUMAX_OBJ="$(BT_SHARED_OBJ)" \
					HUMAX_CONFIG_DIR="$(HUMAX_MAKE_DIR)/$(CONFIG_PRODUCT_NAME)" V=$(V) LIBNAME=$(BT_SHAREDLIB_NAME) shared-library

di_so_bt-clean:
	$(HUMAX_RM) $(BT_SHARED_OBJ)
	$(HUMAX_RM) $(HUMAX_DRIVER_DIR)/lib$(BT_SHAREDLIB_NAME).so

di_so_bt-distclean: di_so_bt-clean

## PPPOE ###################################################
ifeq ($(filter-out di_so_pppoe,$(MAKECMDGOALS)),)
HUMAX_EXTRA_CFLAG += -fPIC
endif

PPPOE_SHARED_OBJ := $(HUMAX_DRIVER_DIR)/di/network/di_pppoe.o
PPPOE_SHARED_OBJ += $(HUMAX_DRIVER_DIR)/di/di_config_network.o

PPPOE_SHAREDLIB_NAME := di_pppoe

prepare_pppoe_sharedlib:
	$(HUMAX_CP) -f $(HUMAX_PRODUCT_CONFIG_DIR)/platform/driver/$(CONFIG_PRODUCT_NAME)/$(CONFIG_BOARD_REVISION)/di_config_network.c $(HUMAX_DRIVER_DIR)/di

di_so_pppoe: prepare_pppoe_sharedlib
	$(Q_)echo "Compiling libdi_pppoe.so for Shared Library!!!"
	$(HUMAX_MAKE) -f $(HUMAX_MAKE_DIR)/HumaxDefaultRule.mak LIB_TYPE=SHARED HUMAX_OBJ="$(PPPOE_SHARED_OBJ)" \
					HUMAX_CONFIG_DIR="$(HUMAX_MAKE_DIR)/$(CONFIG_PRODUCT_NAME)" V=$(V) LIBNAME=$(PPPOE_SHAREDLIB_NAME) shared-library

di_so_pppoe-clean:
	$(HUMAX_RM) $(PPPOE_SHARED_OBJ)
	$(HUMAX_RM) $(HUMAX_DRIVER_DIR)/lib$(PPPOE_SHAREDLIB_NAME).so

di_so_pppoe-distclean: di_so_pppoe-clean

## FS ###################################################
ifeq ($(filter-out di_so_fs,$(MAKECMDGOALS)),)
HUMAX_EXTRA_CFLAG += -fPIC
endif

FS_SHARED_OBJ := $(HUMAX_DRIVER_DIR)/di/fs/di_fs.o
FS_SHARED_OBJ += $(HUMAX_DRIVER_DIR)/di/fs/di_fs_partition.o

FS_SHAREDLIB_NAME := di_fs

di_so_fs:
	$(Q_)echo "Compiling libdi_fs.so for Shared Library!!!"
	$(HUMAX_MAKE) -f $(HUMAX_MAKE_DIR)/HumaxDefaultRule.mak LIB_TYPE=SHARED HUMAX_OBJ="$(FS_SHARED_OBJ)" \
					HUMAX_CONFIG_DIR="$(HUMAX_MAKE_DIR)/$(CONFIG_PRODUCT_NAME)" V=$(V) LIBNAME=$(FS_SHAREDLIB_NAME) shared-library

di_so_fs-clean:
	$(HUMAX_RM) $(FS_SHARED_OBJ)
	$(HUMAX_RM) $(HUMAX_DRIVER_DIR)/lib$(FS_SHAREDLIB_NAME).so

di_so_fs-distclean: di_so_fs-clean


## CRYPT #################################################
ifeq ($(filter-out di_so_crypt,$(MAKECMDGOALS)),)
HUMAX_EXTRA_CFLAG += -fPIC
endif

CRYPT_SHARED_OBJ := $(HUMAX_DRIVER_DIR)/di/crypt/di_crypt.o

CRYPT_SHAREDLIB_NAME := di_crypt

di_so_crypt: 
	$(Q_)echo "Compiling libdi_crypt.so for Shared Library!!!"
	$(HUMAX_MAKE) -f $(HUMAX_MAKE_DIR)/HumaxDefaultRule.mak LIB_TYPE=SHARED HUMAX_OBJ="$(CRYPT_SHARED_OBJ)" HUMAX_CONFIG_DIR="$(HUMAX_MAKE_DIR)/$(CONFIG_PRODUCT_NAME)" V=$(V) LIBNAME=$(CRYPT_SHAREDLIB_NAME) shared-library

di_so_crypt-clean:
	$(HUMAX_RM) $(CRYPT_SHARED_OBJ)
	$(HUMAX_RM) $(HUMAX_DRIVER_DIR)/lib$(CRYPT_SHAREDLIB_NAME).so

di_so_crypt-distclean: di_so_crypt-clean


## HOTPLUG #############################################
ifeq ($(filter-out drv_so_hotplug,$(MAKECMDGOALS)),)
HUMAX_EXTRA_CFLAG += -fPIC
endif

HOTPLUG_SHARED_OBJ := $(HUMAX_DRIVER_DIR)/di/drv/hotplug/drv_hotplug.o

HOTPLUG_SHAREDLIB_NAME := drv_hotplug

drv_so_hotplug:
	$(Q_)echo "Compiling libdrv_hotplug.so for Shared Library!!!"
	$(HUMAX_MAKE) -f $(HUMAX_MAKE_DIR)/HumaxDefaultRule.mak LIB_TYPE=SHARED HUMAX_OBJ="$(HOTPLUG_SHARED_OBJ)" \
					HUMAX_CONFIG_DIR="$(HUMAX_MAKE_DIR)/$(CONFIG_PRODUCT_NAME)" V=$(V) LIBNAME=$(HOTPLUG_SHAREDLIB_NAME) shared-library

drv_so_hotplug-clean:
	$(HUMAX_RM) $(HOTPLUG_SHARED_OBJ)
	$(HUMAX_RM) $(HUMAX_DRIVER_DIR)/lib$(HOTPLUG_SHAREDLIB_NAME).so

drv_so_hotplug-distclean: drv_so_hotplug-clean	

## MMC ################################################
ifeq ($(filter-out di_so_mmc,$(MAKECMDGOALS)),)
HUMAX_EXTRA_CFLAG += -fPIC
endif

MMC_SHARED_OBJ := $(HUMAX_DRIVER_DIR)/di/mmc/di_mmc.o
MMC_SHAREDLIB_NAME := di_mmc

di_so_mmc:
	$(Q_)echo "Compiling libdi_mmc.so for Shared Library!!!"
	$(HUMAX_MAKE) -f $(HUMAX_MAKE_DIR)/HumaxDefaultRule.mak LIB_TYPE=SHARED HUMAX_OBJ="$(MMC_SHARED_OBJ)" \
					HUMAX_CONFIG_DIR="$(HUMAX_MAKE_DIR)/$(CONFIG_PRODUCT_NAME)" V=$(V) LIBNAME=$(MMC_SHAREDLIB_NAME) shared-library

di_so_mmc-clean:
	$(HUMAX_RM) $(MMC_SHARED_OBJ)
	$(HUMAX_RM) $(HUMAX_DRIVER_DIR)/lib$(MMC_SHAREDLIB_NAME).so

di_so_mmc-distclean: di_so_mmc-clean
	

## USB ################################################
ifeq ($(filter-out di_so_usb,$(MAKECMDGOALS)),)
HUMAX_EXTRA_CFLAG += -fPIC
endif

USB_SHARED_OBJ := $(HUMAX_DRIVER_DIR)/di/usb/di_usb.o
ifeq ($(CONFIG_USB_HID),y)
USB_SHARED_OBJ += $(HUMAX_DRIVER_DIR)/di/usb/di_usb_hid.o
endif
ifeq ($(CONFIG_USB_WIRELESS),y)
ifneq ($(CONFIG_DINETWORK), y)
USB_SHARED_OBJ += $(HUMAX_DRIVER_DIR)/di/usb/di_usb_wireless.o
endif
endif

USB_SHAREDLIB_NAME := di_usb

di_so_usb:
	$(Q_)echo "Compiling libdi_usb.so for Shared Library!!!"
	$(HUMAX_MAKE) -f $(HUMAX_MAKE_DIR)/HumaxDefaultRule.mak LIB_TYPE=SHARED HUMAX_OBJ="$(USB_SHARED_OBJ)" \
					HUMAX_CONFIG_DIR="$(HUMAX_MAKE_DIR)/$(CONFIG_PRODUCT_NAME)" V=$(V) LIBNAME=$(USB_SHAREDLIB_NAME) shared-library

di_so_usb-clean:
	$(HUMAX_RM) $(USB_SHARED_OBJ)
	$(HUMAX_RM) $(HUMAX_DRIVER_DIR)/lib$(USB_SHAREDLIB_NAME).so

di_so_usb-distclean: di_so_usb-clean
	

## HDD ################################################
ifeq ($(filter-out di_so_hdd,$(MAKECMDGOALS)),)
HUMAX_EXTRA_CFLAG += -fPIC
endif

HDD_SHARED_OBJ := $(HUMAX_DRIVER_DIR)/di/hdd/di_hdd.o
ifeq ($(findstring DI_HDD_SMART_TYPE_NONE, $(CONFIG_DI_HDD_SMART_TYPE)),)
HDD_SHARED_OBJ += $(HUMAX_DRIVER_DIR)/di/drv/smart/drv_smart.o
endif

HDD_SHAREDLIB_NAME := di_hdd

di_so_hdd:
	$(Q_)echo "Compiling libdi_hdd.so for Shared Library!!!"
	$(HUMAX_CP) -f $(HUMAX_PRODUCT_CONFIG_DIR)/platform/driver/$(CONFIG_PRODUCT_NAME)/$(CONFIG_BOARD_REVISION)/hdd_gpio_map.h $(HUMAX_DRIVER_DIR)/di/hdd/hdd_gpio_map.h
	$(HUMAX_MAKE) -f $(HUMAX_MAKE_DIR)/HumaxDefaultRule.mak LIB_TYPE=SHARED HUMAX_OBJ="$(HDD_SHARED_OBJ)" \
					HUMAX_CONFIG_DIR="$(HUMAX_MAKE_DIR)/$(CONFIG_PRODUCT_NAME)" V=$(V) LIBNAME=$(HDD_SHAREDLIB_NAME) shared-library

di_so_hdd-clean:
	$(HUMAX_RM) $(HUMAX_DRIVER_DIR)/di/hdd/hdd_gpio_map.h
	$(HUMAX_RM) $(HDD_SHARED_OBJ)
	$(HUMAX_RM) $(HUMAX_DRIVER_DIR)/lib$(HDD_SHAREDLIB_NAME).so

di_so_hdd-distclean: di_so_hdd-clean
	

########################################################
## Executable
########################################################
EXEC_COMPILE_FLAGS = \
	HUMAX_CC=$(HUMAX_CC) \
	HUMAX_PREPARE_ROOT_DIR=$(HUMAX_PREPARE_ROOT_DIR) \
	HUMAX_WORK_DIR=$(HUMAX_WORK_DIR) \
	CONFIG_PRODUCT_IMAGE_DEBUG=$(CONFIG_PRODUCT_IMAGE_DEBUG)

di_exec: FORCE
	$(HUMAX_MAKE) -C $(HUMAX_DRIVER_DIR)/di_exec EXEC_COMPILE_FLAGS=$(EXEC_COMPILE_FLAGS) $@

di_exec-clean:
	$(HUMAX_MAKE) -C $(HUMAX_DRIVER_DIR)/di_exec EXEC_COMPILE_FLAGS=$(EXEC_COMPILE_FLAGS) $@

di_exec-distclean: di_exec-clean
	
FORCE:

