###################################################################
# HUMAX makefile.
###################################################################

###################################################################
# setting the path for cross compiler & build env
###################################################################
CUR_DIR			= $(shell pwd)
TOP_DIR			= $(CUR_DIR)
WL_NIC_PATH		= $(TOP_DIR)/src/dhd/linux
WL_APP_PATH		= $(TOP_DIR)/src/wl/exe
WL_WPS_PATH		= $(TOP_DIR)/src/wps/linux/enr

WLTEST=1
LINUXVER=$(shell echo $(KERNEL_WIRELESS_RELEASE) | cut -d'-' -f1)
TARGETDIR=${LINUXVER}
CHIPVER=43236b
BUILDCFG=$(HUMAX_BUILDCFG)

ifneq (,$(findstring mips,$(CC)))
	ARCH				:= mips
	TARGETARCH	:= mips
	TARGETMACH	:= mipsel
	TARGETENV		:= linuxmips
	STRIP				:= mipsel-linux-strip
else ifneq (,$(findstring arm,$(CC)))
	ARCH				:= arm
	TARGETARCH	:= arm_le
	TARGETMACH	:= armle
	TARGETENV		:= linuxarm_le
	STRIP				:= arm-linux-strip
else
	ARCH				:= arm
	TARGETARCH	:= arm_le
	TARGETMACH	:= armle
	TARGETENV		:= linuxarm_le
	STRIP				:= arm-linux-strip
endif

###################################################################
# Install macro
###################################################################
WL_NIC_OUTPUT = ${TARGETDIR}/dhd.ko
WL_NIC_OUTPUT_INSTALL = $(HUMAX_PREPARE_ROOT_DIR)/lib/modules/$(KERNEL_WIRELESS_RELEASE)/kernel/drivers/wireless/bcm43236

WL_APP_OUTPUT = ${TARGETDIR}/wl
WL_APP_OUTPUT_INSTALL = $(HUMAX_PREPARE_ROOT_DIR)/usr/bin

WL_WPS_OUTPUT = ${WL_WPS_PATH}/$(ARCH)-linux-gcc/wpsenr
WL_WPS_OUTPUT_INSTALL = $(HUMAX_PREPARE_ROOT_DIR)/usr/bin

WL_FW_OUTPUT = ./firmware/43236b-roml/$(FIRMWARE)
WL_FW_OUTPUT_INSTALL = $(HUMAX_PREPARE_ROOT_DIR)/lib/firmware/brcm/

WL_NVM_OUTPUT = ./nvrams/$(CONFIG_PRODUCT_NAME)/$(CONFIG_PRODUCT_NAME).nvm
WL_NVM_OUTPUT_INSTALL = $(HUMAX_PREPARE_ROOT_DIR)/lib/firmware/brcm/

WL_HOTPLUG = ./hmx_utils/hotplug
WL_HOTPLUG_INSTALL = $(HUMAX_PREPARE_ROOT_DIR)/sbin/

###################################################################
# Build parameters
###################################################################
SWB_BUILD_TARGET := ${BUILDCFG}
WL_APP_BUILD_TARGET := all

COMMON_BUILD_PARAMS += WLTEST=${WLTEST}
COMMON_BUILD_PARAMS += WLLXIW=${WLLXIW}
COMMON_BUILD_PARAMS += LINUXVER=${LINUXVER}
COMMON_BUILD_PARAMS += LINUXDIR=$(KERNEL_DIR)
COMMON_BUILD_PARAMS += TARGETDIR=${TARGETDIR}
COMMON_BUILD_PARAMS += TARGETARCH=$(TARGETARCH)
COMMON_BUILD_PARAMS += TARGETMACH=$(TARGETARCH)
COMMON_BUILD_PARAMS += TARGETENV=$(TARGETENV)
COMMON_BUILD_PARAMS += STRIP=$(STRIP)
COMMON_BUILD_PARAMS += STBLINUX=1
COMMON_BUILD_PARAMS += BRAND=linux-external-wl
COMMON_BUILD_PARAMS += CHIPVER=$(CHIPVER)
COMMON_BUILD_PARAMS += USBSHIM=0
COMMON_BUILD_PARAMS += V=1
COMMON_BUILD_PARAMS += BRCM_WLAN_IFNAME=wlan
COMMON_BUILD_PARAMS += HUMAX_PLATFORM_BASE=$(HUMAX_PLATFORM_BASE)
COMMON_BUILD_PARAMS += CONFIG_SUPPORT_WPS=$(CONFIG_SUPPORT_WPS)
COMMON_BUILD_PARAMS += CONFIG_SUPPORT_PBC_PHYSICAL=$(CONFIG_SUPPORT_PBC_PHYSICAL)
COMMON_BUILD_PARAMS += CONFIG_SUPPORT_ENTERPRISE=$(CONFIG_SUPPORT_ENTERPRISE)

###################################################################
# Build Targets
###################################################################

outdir:
	@test -d $(TARGETDIR) || install -d -m 777 $(TARGETDIR)

wl:
	@ echo "*************************************************"
	@ echo "*************** Build DHD Driver ****************"
	@ echo "*************************************************"
	@$(MAKE) -C ./src/include/
	@$(MAKE) -C $(WL_NIC_PATH) $(COMMON_BUILD_PARAMS) $(SWB_BUILD_TARGET)
	@cp  -v $(WL_NIC_PATH)/${BUILDCFG}-${LINUXVER}/dhd.ko ${TARGETDIR}/dhd.ko
	@cp  -v $(WL_NIC_PATH)/${BUILDCFG}-${LINUXVER}/dhd.ko ${TARGETDIR}/${CHIPVER}-${BUILDCFG}-wl.ko

app-wl:
	@ echo "*************************************************"
	@ echo "************* Build WL Application **************"
	@ echo "*************************************************"
	@echo $(MAKE) -C $(WL_APP_PATH) ASD=0 $(COMMON_BUILD_PARAMS) $(WL_APP_BUILD_TARGET)
	@$(MAKE) -C $(WL_APP_PATH) ASD=0 $(COMMON_BUILD_PARAMS) $(WL_APP_BUILD_TARGET)
	@cp -v $(WL_APP_PATH)/wl${TARGETARCH} ${TARGETDIR}/wl
	@cp -v $(WL_APP_PATH)/wl${TARGETARCH} ${TARGETDIR}/wl-$(ARCH)

app-wps:
ifeq ($(CONFIG_SUPPORT_WPS),y)
	@ echo "*************************************************"
	@ echo "*************** Build WPS Enrolee ***************"
	@ echo "*************************************************"
	@echo $(MAKE) $(COMMON_BUILD_PARAMS) PWD=$(WL_WPS_PATH) -C $(WL_WPS_PATH) -f wps_enr_app.mk
	@$(MAKE) $(COMMON_BUILD_PARAMS) PWD=$(WL_WPS_PATH) -C $(WL_WPS_PATH) -f wps_enr_app.mk
endif

all: outdir wl app-wl app-wps

clean:
	@rm -rf ${TARGETDIR}
	@rm -vrf ./src/wl/linux/obj-*
	@$(MAKE) -C $(WL_NIC_PATH) $(COMMON_BUILD_PARAMS) clean
	@$(MAKE) -C $(WL_APP_PATH) ASD=0 $(COMMON_BUILD_PARAMS) clean

###################################################################
# Install target
###################################################################
install:
	@install -d $(WL_NIC_OUTPUT_INSTALL)	
	@install -d $(WL_FW_OUTPUT_INSTALL)	
	@install -m 644 $(WL_NIC_OUTPUT) $(WL_NIC_OUTPUT_INSTALL)
	@install -m 777 $(WL_FW_OUTPUT) $(WL_FW_OUTPUT_INSTALL)/bcm43236-firmware.bin
	@install -m 777 $(WL_NVM_OUTPUT) $(WL_NVM_OUTPUT_INSTALL)/bcm43236.nvm
	@install -m 777 $(WL_APP_OUTPUT) $(WL_APP_OUTPUT_INSTALL)
ifeq ($(CONFIG_SUPPORT_WPS),y)
	@install -m 777 $(WL_WPS_OUTPUT) $(WL_WPS_OUTPUT_INSTALL)	
endif
	@install -m 777 $(WL_HOTPLUG) $(WL_HOTPLUG_INSTALL)


