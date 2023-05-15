#
# Copyright (C) 2011 HUMAX Co., Ltd. All rights reserved.
#

# 
################## HW TEST S/W ###################

ifeq ($(CONFIG_PRODUCT_IMAGE_HWTEST),y)
TARGETS += hwtest main link
IGNORE += link

# hwtest app dir path
HUMAX_HWTEST_DIR=$(HUMAX_WORK_DIR)/apps/hwtest
HUMAX_HWTEST_MAKE_DIR=$(HUMAX_WORK_DIR)/apps/make/hwtest_make

HWTEST_COMPILE_FLAGS=CONFIG_TOOLCHAIN_PATH=$(CONFIG_TOOLCHAIN_PATH) \
	HUMAX_PORTING_MAKE_DIR=$(HUMAX_PORTING_MAKE_DIR) \
	HUMAX_WORK_DIR=$(HUMAX_WORK_DIR) \
	HUMAX_HWTEST_DIR=$(HUMAX_HWTEST_DIR) \
	HUMAX_HWTEST_MAKE_DIR=$(HUMAX_HWTEST_MAKE_DIR) \
	HUMAX_PLATFORM_CONFIG_DIR=$(HUMAX_PLATFORM_CONFIG_DIR) \
	HUMAX_DRIVER_DIR=$(HUMAX_DRIVER_DIR) \
	CONFIG_PRODUCT_NAME=$(CONFIG_PRODUCT_NAME) \
	CONFIG_PRODUCT_IMAGE_TYPE=$(CONFIG_PRODUCT_IMAGE_TYPE) \
	CONFIG_BOARD_REVISION=$(CONFIG_BOARD_REVISION) \
	HUMAX_MAKE_DIR=$(HUMAX_MAKE_DIR) \
	HUMAX_KCONFIG_TOOL_DIR=$(HUMAX_KCONFIG_TOOL_DIR) \
	CONFIG_PRODUCT_DI_DEBUG_SYMBOL=$(CONFIG_PRODUCT_DI_DEBUG_SYMBOL) \
	HUMAX_OUTPUT_DIR=$(HUMAX_OUTPUT_DIR) \
	NEXUS_TOP=$(NEXUS_TOP)

 
ifeq ($(CONFIG_PRODUCT_IMAGE_RELEASE),y)
	HWTEST_COMPILE_FLAGS += CONFIG_PRODUCT_IMAGE_RELEASE=$(CONFIG_PRODUCT_IMAGE_RELEASE)
else
	HWTEST_COMPILE_FLAGS += CONFIG_PRODUCT_IMAGE_DEBUG=$(CONFIG_PRODUCT_IMAGE_DEBUG)
endif

ifeq ($(CONFIG_DINETWORK),y)
	HWTEST_COMPILE_FLAGS += CONFIG_DINETWORK=$(CONFIG_DINETWORK)
endif

define build_hwtest
	$(HUMAX_MAKE) -C $(HUMAX_HWTEST_MAKE_DIR) $(HWTEST_COMPILE_FLAGS) $(1)
endef

hwtest:
	$(call build_hwtest, hwtest)

hwtest-clean:
	$(call build_hwtest, clean_hwtest)

hwtest-distclean:
	$(call build_hwtest, clean_hwtest)
endif

##################################################

################## HW TEST MULTI-PROCESS S/W ###################

ifeq ($(CONFIG_PRODUCT_IMAGE_HWTEST_MULTI),y)
TARGETS += hwtestav hwtestav-link hwtestosd hwtestosd-link hwtestcore hwtestcore-link
IGNORE += linkav linkosd

# hwtest app dir path
HUMAX_HWTEST_DIR=$(HUMAX_WORK_DIR)/apps/hwtest_multi
HUMAX_HWTEST_MAKE_DIR=$(HUMAX_WORK_DIR)/apps/make/hwtest_multi_make

HWTEST_COMPILE_FLAGS=CONFIG_TOOLCHAIN_PATH=$(CONFIG_TOOLCHAIN_PATH) \
	HUMAX_PORTING_MAKE_DIR=$(HUMAX_PORTING_MAKE_DIR) \
	HUMAX_WORK_DIR=$(HUMAX_WORK_DIR) \
	HUMAX_HWTEST_DIR=$(HUMAX_HWTEST_DIR) \
	HUMAX_HWTEST_MAKE_DIR=$(HUMAX_HWTEST_MAKE_DIR) \
	HUMAX_DRIVER_DIR=$(HUMAX_DRIVER_DIR) \
	CONFIG_PRODUCT_NAME=$(CONFIG_PRODUCT_NAME) \
	CONFIG_PRODUCT_IMAGE_TYPE=$(CONFIG_PRODUCT_IMAGE_TYPE) \
	CONFIG_BOARD_REVISION=$(CONFIG_BOARD_REVISION) \
	HUMAX_MAKE_DIR=$(HUMAX_MAKE_DIR) \
	HUMAX_KCONFIG_TOOL_DIR=$(HUMAX_KCONFIG_TOOL_DIR) \
	CONFIG_PRODUCT_DI_DEBUG_SYMBOL=$(CONFIG_PRODUCT_DI_DEBUG_SYMBOL) \
	HUMAX_OUTPUT_DIR=$(HUMAX_OUTPUT_DIR) 

define build_hwtest
	$(HUMAX_MAKE) -C $(HUMAX_HWTEST_MAKE_DIR) $(HWTEST_COMPILE_FLAGS) $(1)
endef

hwtestav:
	$(call build_hwtest, hwtest_av)

hwtestav-link:
	$(call build_hwtest, link_av)

hwtestav-clean:
	$(call build_hwtest, clean_hwtest_av)

hwtestav-distclean:
	$(call build_hwtest, clean_hwtest_av)
	$(call build_hwtest, clean_config)

hwtestosd:
	$(call build_hwtest, hwtest_osd)

hwtestosd-link:
	$(call build_hwtest, link_osd)

hwtestosd-clean:
	$(call build_hwtest, clean_hwtest_osd)

hwtestosd-distclean:
	$(call build_hwtest, clean_hwtest_osd)
	$(call build_hwtest, clean_config)

hwtestcore:
	$(call build_hwtest, hwtest_core)

hwtestcore-link:
	$(call build_hwtest, link_core)

hwtestcore-clean:
	$(call build_hwtest, clean_hwtest_core)

hwtestcore-distclean:
	$(call build_hwtest, clean_hwtest_core)
	$(call build_hwtest, clean_config)
endif

##################################################

##################  OCTO S/W  ####################

ifeq ($(CONFIG_PRODUCT_IMAGE_OCTO),y)
TARGETS += octo-packages octo main link
IGNORE += link

# octo app dir path
HUMAX_OCTO_DIR=$(HUMAX_WORK_DIR)/../apps/humaxtv
HUMAX_OCTO_PACKAGES_DIR=$(HUMAX_WORK_DIR)/../apps/packages
CONFIG_OCTO_PRODUCT_NAME=$(HUMAX_PRODUCT_BUILD_NAME)

OCTO_COMPILE_FLAGS= \
	PRODUCT_NAME=$(CONFIG_OCTO_PRODUCT_NAME) \
	MODULE_NAME=$(CONFIG_MODULE_NAME) \
	CROSS_COMPILE=$(HUMAX_CROSS_COMPILE) \
	CROSS_PLATFORM_USR_LIB_DIR=$(HUMAX_UCLIBC_OLD_DIR)/lib	\
	TOP_DIR=$(HUMAX_WORK_DIR)/.. \
	MODEL_DIR=$(HUMAX_WORK_DIR)/../model \
	DI_INC_DIR=$(HUMAX_WORK_DIR)/include/di \
	DI_ICS_INC_DIR=$(HUMAX_WORK_DIR)/include/ics \
	NEXUS_LIB_DIR=$(HUMAX_OUTPUT_DIR)/lib \
	SHARED_LIB_DIR=$(HUMAX_OUTPUT_DIR)/lib \
	STATIC_LIB_DIR=$(HUMAX_OUTPUT_DIR)/lib \
	PACKAGES_DIR=$(HUMAX_WORK_DIR)/../apps/packages \
	DEBUG=$(CONFIG_PRODUCT_APP_DEBUG_LOG) \
	BUILD_USER_DIR=$(BUILD_USER_DIR)

PACKAGES_COMPILE_FLAGS=TOOLCHAIN_PATH=$(CONFIG_TOOLCHAIN_PATH) \
	CONFIG_PRODUCT_NAME=$(CONFIG_OCTO_PRODUCT_NAME) \
	CONFIG_APP_DEBUG=$(CONFIG_PRODUCT_APP_DEBUG_LOG) \
	MAKE_DIR=$(HUMAX_NFS_INSTALL_DIR) \
	APPLICATION_DIR=$(HUMAX_WORK_DIR)/../apps

define build_octo
	$(HUMAX_MAKE) -C $(HUMAX_OCTO_DIR) $(OCTO_COMPILE_FLAGS) $(1)
endef

octo: octo-defconfig 
	@echo $(CONFIG_OCTO_PRODUCT_NAME)
	$(call build_octo, all)
octo-defconfig:
ifeq ($(CONFIG_PRODUCT_APP_DEBUG_LOG),y)
	@test -f $(HUMAX_OCTO_DIR)/.config || $(call build_octo, $(CONFIG_OCTO_PRODUCT_NAME)_defconfig)
else
	@test -f $(HUMAX_OCTO_DIR)/.config || $(call build_octo, $(CONFIG_OCTO_PRODUCT_NAME)_release_defconfig)
endif
	
octo-clean:
	$(call build_octo, clean)
octo-distclean:
	$(call build_octo, distclean)

define build_octo-packages
	$(HUMAX_MAKE) -C $(HUMAX_OCTO_PACKAGES_DIR) $(PACKAGES_COMPILE_FLAGS) $(1) 
endef

octo-packages: octo-packages-defconfig 
	$(call build_octo-packages,)

octo-packages-defconfig:
	@test -f $(HUMAX_OCTO_PACKAGES_DIR)/.config || $(call build_octo-packages, $(CONFIG_OCTO_PRODUCT_NAME)_defconfig)

octo-packages-clean:
	$(call build_octo-packages, clean)

octo-packages-distclean :
	$(call build_octo-packages, distclean)

copy-db:
	$(HUMAX_CP) $(HUMAX_WORK_DIR)/../apps/humaxtv/emulator/make_iHDR5200C/Emulator/*.db $(HUMAX_NFS_INSTALL_DIR)/var/lib/humaxtv/
	chmod 777 -R $(HUMAX_NFS_INSTALL_DIR)/var/lib/humaxtv
endif

##################################################

##################  FACTORY S/W  ####################

ifdef CONFIG_PRODUCT_IMAGE_FACTORY
TARGETS += factorysw main link
IGNORE += link

# octo app dir path
HUMAX_FACTORY_DIR=$(HUMAX_WORK_DIR)/apps/factory/make/build_rule
HUMAX_FACTORY_INCLUDE=-I$(HUMAX_WORK_DIR)/include/di
HUMAX_FACTORY_INCLUDE+=-I$(HUMAX_WORK_DIR)/include/hos
HUMAX_FACTORY_INCLUDE+=-I$(HUMAX_WORK_DIR)/include/cmd
HUMAX_FACTORY_INCLUDE+=-I$(HUMAX_WORK_DIR)/include/ai
HUMAX_FACTORY_INCLUDE+=-I$(HUMAX_WORK_DIR)/platform/driver/di/drv/include
HUMAX_FACTORY_INCLUDE+=-include $(HUMAX_WORK_DIR)/make/autoconf.h
HUMAX_FACTORY_INCLUDE+=-include $(HUMAX_WORK_DIR)/make/$(CONFIG_PRODUCT_NAME)/autoconf.h
HUMAX_FACTORY_CC = $(HUMAX_CC)
HUMAX_FACTORY_AR = $(HUMAX_AR)
HUMAX_FACTORY_LIB=libfactory.a

#CONFIG_PRODUCT_NAME

FACTORY_COMPILE_FLAGS= \
	PRODUCT_NAME=$(CONFIG_PRODUCT_NAME) \
	MODULE_NAME=$(CONFIG_MODULE_NAME) \
	CROSS_COMPILE=$(HUMAX_CROSS_COMPILE) \
	CROSS_PLATFORM_USR_LIB_DIR=$(HUMAX_UCLIBC_OLD_DIR)/lib	\
	TOP_DIR=$(HUMAX_WORK_DIR)/.. \
	MODEL_DIR=$(HUMAX_WORK_DIR)/../model \
	NEXUS_LIB_DIR=$(HUMAX_OUTPUT_DIR)/lib \
	FACTORYSW_INCLUDE="$(HUMAX_FACTORY_INCLUDE)" \
	FACTORYSW_CC="$(HUMAX_FACTORY_CC)" \
	FACTORYSW_AR="$(HUMAX_FACTORY_AR)" \
	SHARED_LIB_DIR=$(HUMAX_OUTPUT_DIR)/lib \
	STATIC_LIB_DIR=$(HUMAX_OUTPUT_DIR)/lib \
	PACKAGES_DIR=$(HUMAX_WORK_DIR)/../apps/packages \
	DEBUG=$(CONFIG_PRODUCT_APP_DEBUG_LOG) \
	BUILD_USER_DIR=$(BUILD_USER_DIR)

define build_factory
	$(HUMAX_MAKE) -C $(HUMAX_FACTORY_DIR) \
			$(FACTORY_COMPILE_FLAGS) $(1) BUILD_LIB=$(HUMAX_FACTORY_LIB)
endef

factorysw : 
	$(call build_factory, build)
	@cp $(HUMAX_FACTORY_DIR)/../../$(HUMAX_FACTORY_LIB) $(HUMAX_OUTPUT_DIR)/lib

factorysw-clean:
	$(call build_factory, clean)

factorysw-distclean:
	$(call build_factory, clean)

endif

##################################################


################## OCTO2.0 S/W ###################

ifneq ($(CONFIG_PRODUCT_IMAGE_OCTO20),)
ifneq ($(CONFIG_PRODUCT_MULTI_PROCESS),)
TARGETS += main link octo
else
TARGETS += octo
endif

# octo app dir path
HUMAX_OCTO_MAKE_DIR=$(HUMAX_WORK_DIR)/../apps/make

define build_octo
	$(HUMAX_MAKE) -C $(HUMAX_OCTO_MAKE_DIR) $(1)
endef

octo: octo-defconfig 
	$(call build_octo, app_world)

octo-defconfig:
ifeq ($(CONFIG_PRODUCT_APP_DEBUG_LOG),y)
	@test -f $(HUMAX_OCTO_MAKE_DIR)/.config || $(call build_octo, $(CONFIG_PRODUCT_NAME)_defconfig)
else
	@test -f $(HUMAX_OCTO_MAKE_DIR)/.config || $(call build_octo, $(CONFIG_PRODUCT_NAME)_release_defconfig)
endif
	
octo-clean:
	$(call build_octo, app_clean)
octo-distclean:
	$(call build_octo, app_distclean)
endif

##################################################

################## UOCTO S/W ###################

ifeq ($(CONFIG_PRODUCT_IMAGE_UOCTO),y)
TARGETS += uocto main link
IGNORE += link
APPLICATION_DIR=$(HUMAX_WORK_DIR)/../apps/uocto
APPLICATION_MAKE_DIR=$(APPLICATION_DIR)/make

# uocto app dir path
HUMAX_UOCTO_DIR=$(APPLICATION_DIR)/uocto
HUMAX_UOCTO_MAIN_DIR=$(HUMAX_UOCTO_DIR)/application
HUMAX_UOCTO_MAKE_DIR=$(HUMAX_UOCTO_DIR)/make
CONFIG_UOCTO_PRODUCT_NAME=$(HUMAX_PRODUCT_BUILD_NAME)
HUMAX_UOCTO_LIB=libapp.a

UOCTO_COMPILE_FLAGS = \
	CONFIG_PRODUCT_IMAGE_UOCTO=$(CONFIG_PRODUCT_IMAGE_UOCTO) \
	CONFIG_MAKE_DIR=$(APPLICATION_MAKE_DIR) \
	TOP_MAKE_DIR=$(APPLICATION_MAKE_DIR) \
	PLOADER_MAKE_DIR=$(HUMAX_WORK_DIR)/../make \
	MAKE_IMAGE_DIR=$(APPLICATION_MAKE_DIR) \

UOCTO_COMPILE_DI_CFLAGS = \
	DI_CFLAG+=-I$(HUMAX_WORK_DIR)/include/di \
	DI_CFLAG+=-I$(HUMAX_WORK_DIR)/include/hos \
	DI_CFLAG+=-I$(HUMAX_WORK_DIR)/include/ai \
	DI_CFLAG+=-I$(HUMAX_WORK_DIR)/include/cmd \
	DI_CFLAG+=-include$(APPLICATION_MAKE_DIR)/autoconf.h \

define build_apps
	$(HUMAX_MAKE) -C $(APPLICATION_MAKE_DIR) $(UOCTO_COMPILE_FLAGS) $(1)
endef

uocto-defconfig:
ifeq ($(CONFIG_PRODUCT_APP_DEBUG_LOG),y)
	@test -f $(APPLICATION_MAKE_DIR)/.config || $(call build_apps, $(CONFIG_UOCTO_PRODUCT_NAME)_defconfig)
else
	@test -f $(APPLICATION_MAKE_DIR)/.config || $(call build_apps, $(CONFIG_UOCTO_PRODUCT_NAME)_release_defconfig)
endif

uocto-build_date:
	@echo "const char *g_pszBuildDate = \"`date +%G%m%d-%H%M%S`\";" > $(HUMAX_UOCTO_MAIN_DIR)/ldr_builddate.c

uocto-clean-build_date:
	$(HUMAX_RM) $(HUMAX_UOCTO_MAIN_DIR)/ldr_builddate.c

define build_uocto
	$(HUMAX_MAKE) -C $(HUMAX_UOCTO_MAKE_DIR) $(UOCTO_COMPILE_FLAGS) $(UOCTO_COMPILE_DI_CFLAGS) $(1)
endef

uocto : uocto-defconfig uocto-build_date
	@echo $(CONFIG_UOCTO_PRODUCT_NAME)
ifeq ($(CONFIG_PRODUCT_IMAGE_RELEASE),y)	
	@if [ ! -d $(HUMAX_UOCTO_MAKE_DIR)/RELEASE ]; then\
		mkdir -p $(HUMAX_UOCTO_MAKE_DIR)/RELEASE; \
	fi
else
	@if [ ! -d $(HUMAX_UOCTO_MAKE_DIR)/DEBUG ]; then\
		mkdir -p $(HUMAX_UOCTO_MAKE_DIR)/DEBUG; \
	fi
endif
	$(call build_uocto, all)
ifeq ($(CONFIG_PRODUCT_IMAGE_RELEASE),y)	
	$(HUMAX_CP) $(HUMAX_UOCTO_MAKE_DIR)/RELEASE/$(HUMAX_UOCTO_LIB) $(HUMAX_OUTPUT_DIR)/lib
else
	$(HUMAX_CP) $(HUMAX_UOCTO_MAKE_DIR)/DEBUG/$(HUMAX_UOCTO_LIB) $(HUMAX_OUTPUT_DIR)/lib
endif

uocto-clean: uocto-clean-build_date
	$(call build_uocto, clean)

uocto-distclean:uocto-clean
	$(call build_apps, distclean)
endif

##################################################

################## UPGRADER S/W ###################

ifeq ($(CONFIG_PRODUCT_IMAGE_UPGRADER),y)
TARGETS += upgraderlib main link
IGNORE += upgraderlib link
UPGRADER_DIR=$(HUMAX_WORK_DIR)/../apps/upgrader
UPGRADER_MAKE_DIR=$(UPGRADER_DIR)/make
UPGRADER_LIB=libuocto.a

upgraderlib : 
ifeq ($(CONFIG_PRODUCT_IMAGE_RELEASE),y)
	@if [ ! -f $(UPGRADER_MAKE_DIR)/RELEASE/$(UPGRADER_LIB) ]; then\
		$(HUMAX_CP) $(UPGRADER_MAKE_DIR)/DEBUG/$(UPGRADER_LIB) $(HUMAX_OUTPUT_DIR)/lib/libupgrader.a; \
	else \
		$(HUMAX_CP) $(UPGRADER_MAKE_DIR)/RELEASE/$(UPGRADER_LIB) $(HUMAX_OUTPUT_DIR)/lib/libupgrader.a; \
	fi	
else
	$(HUMAX_CP) $(UPGRADER_MAKE_DIR)/DEBUG/$(UPGRADER_LIB) $(HUMAX_OUTPUT_DIR)/lib/libupgrader.a
endif

endif

##################################################

