#
# Copyright (C) 2006 HUMAX Co., Ltd. All rights reserved.
#

PRODUCT_NAME=$(CONFIG_PRODUCT_NAME)
MODULE_NAME=$(CONFIG_MODULE_NAME)

#for product.config
DEBUG=$(CONFIG_APP_DEBUG)

DI_CFLAG = $(addprefix -D,$(DI_MODULE))
export DI_CFLAG
export DI_MODULE

APP_CONFIG =\
	PRODUCT_NAME=$(PRODUCT_NAME) \
	MODULE_NAME=$(MODULE_NAME) \
	CROSS_COMPILE=mipsel-linux- \
	CROSS_PLATFORM_USR_LIB_DIR=$(TOOLCHAIN_PATH)/mipsel-linux/lib	\
	TOP_DIR=$(TOP_DIR) \
	MODEL_DIR=$(TOP_DIR)/model \
	NEXUS_LIB_DIR=$(MAKE_DIR)/lib \
	DI_INC_DIR=$(DI_INC) \
	SHARED_LIB_DIR=$(MAKE_DIR)/lib \
	STATIC_LIB_DIR=$(STATIC_LIB_DIR) \
	PACKAGES_DIR=$(TOP_DIR)/apps/packages \
	DEBUG=$(CONFIG_APP_DEBUG) \
	BUILD_USER_DIR=$(BUILD_USER_DIR)

##########################################################################################

ifneq ($(subst IRLDVT,,$(MODULE_NAME)), $(MODULE_NAME))
CONFIG_DEF_TAIL = ldvt_
endif

ifeq ($(CONFIG_APPLICATION_OCTO),y)

TARGETS += packages-defconfig packages octo-defconfig octo

define build_packages
	$(MAKE) -C $(APPLICATION_DIR)/packages $(1)
endef

packages-defconfig:
	@test -f $(APPLICATION_DIR)/packages/.config || $(call build_packages,$(CONFIG_PRODUCT_NAME)_defconfig)
packages:
	$(call build_packages,)
packages-clean:
	$(call build_packages,clean)
packages-distclean:
	$(call build_packages,distclean)
.PHONY: packages packages-clean packages-distclean

define build_octo
	$(MAKE) -C $(APPLICATION_DIR)/humaxtv $(APP_CONFIG) $(1) 2>&1 | tee $(TOP_DIR)/make/app-compile.log
endef
#2>&1 | tee $(DI_DIR)/make/compile.log

octo-defconfig:
ifeq ($(CONFIG_DEBUG_MODE),y)
	@test -f $(APPLICATION_DIR)/humaxtv/.config || $(call build_octo,$(CONFIG_PRODUCT_NAME)_$(CONFIG_DEF_TAIL)defconfig)
else
ifeq ($(CONFIG_APP_DEBUG),y)
	$(call build_octo,$(CONFIG_PRODUCT_NAME)_$(CONFIG_DEF_TAIL)defconfig)
else
	$(call build_octo,$(CONFIG_PRODUCT_NAME)_$(CONFIG_DEF_TAIL)release_defconfig)
endif
endif

octo:
	$(call build_octo,)
octo-clean:
	$(call build_octo,clean)
	$(RM) $(TOP_DIR)/make/app-compile.log
octo-distclean:
	$(call build_octo,distclean)

.PHONY: octo octo-clean octo-distclean

endif

##########################################################################################

ifeq ($(CONFIG_APPLICATION_EAGLE),y)

TARGETS +=  eagle-defconfig eagle

define build_eagle
	$(MAKE) -C $(APPLICATION_DIR)/humaxstb/make $(APP_CONFIG) $(1)
endef

eagle-defconfig:
ifeq ($(CONFIG_DEBUG_MODE),y)
	@test -f $(APPLICATION_DIR)/humaxstb/.config || $(call build_eagle,$(CONFIG_PRODUCT_NAME)_defconfig)
else
	$(call build_eagle,$(CONFIG_PRODUCT_NAME)_release_defconfig)
endif

eagle: factorysw_lib
	$(call build_eagle,)
eagle-clean: factorysw_lib-clean
	$(call build_eagle,clean)
eagle-distclean: factorysw_lib-distclean
	$(call build_eagle,clean)
.PHONY: eagle eagle-clean eagle-distclean

define build_factory_lib
	$(MAKE) -C $(APPLICATION_DIR)/factory/make/build_rule $(1) $(APP_CONFIG) BUILD_LIB=libfactory.a
endef

factorysw_lib:
	$(call build_factory_lib,build)
	$(Q)cp $(APPLICATION_DIR)/factory/libfactory.a $(STATIC_LIB_DIR)

factorysw_lib-clean:
	$(call build_factory_lib,clean)

factorysw_lib-distclean:
	$(call build_factory_lib,clean)
.PHONY: factorysw factorysw-clean factorysw-distclean

endif

##########################################################################################

ifeq ($(CONFIG_IMAGE_HW_TEST),y)
TARGETS += di-hwtest
endif

define build_di_hwtest
	$(MAKE) -C $(APPLICATION_DIR)/hwtest/make CC="$(TARGET_CC)" CROSS="$(TARGET_CROSS)" \
			$(APP_CONFIG) DI_DIR=$(DI_DIR) $(1) 2>&1
endef
di-hwtest:
	$(call build_di_hwtest,hwtest)

di-hwtest-clean:
	$(call build_di_hwtest,clean)

di-hwtest-distclean:
	$(call build_di_hwtest,distclean)
.PHONY: di-hwtest di-hwtest-clean di-hwtest-distclean

##########################################################################################

ifeq ($(CONFIG_IMAGE_UOCTO),y)
TARGETS += di-uocto
endif

define build_di_uocto
	$(MAKE) -C $(APPLICATION_DIR)/uocto/make CC="$(TARGET_CC)" CROSS="$(TARGET_CROSS)" \
			$(APP_CONFIG) DI_DIR=$(DI_DIR) $(1) 2>&1
endef
di-uocto:
	$(call build_di_uocto,uocto)

di-uocto-clean:
	$(call build_di_uocto,clean)

di-uocto-distclean:
	$(call build_di_uocto,distclean)
.PHONY: di-uocto di-uocto-clean di-uocto-distclean

##########################################################################################

ifeq ($(CONFIG_IMAGE_FACTORY),y)
TARGETS += factorysw
endif

define build_factory
	$(MAKE) -C $(APPLICATION_DIR)/factory/make CC="$(TARGET_CC)" CROSS="$(TARGET_CROSS)" \
			$(APP_CONFIG) $(1) 2>&1
endef
factorysw:
	$(call build_factory,build)

factorysw-clean:
	$(call build_factory,clean)


factorysw-distclean:
	$(call build_factory,distclean)
.PHONY: factorysw factorysw-clean factorysw-distclean


##########################################################################################

ifeq ($(CONFIG_DIAGNOSTIC),y)
TARGETS += diagnostic
endif

define build_diagnostic
	$(MAKE) -C $(APPLICATION_DIR)/diagnostic/make CC="$(TARGET_CC)" CROSS="$(TARGET_CROSS)" \
			$(APP_CONFIG) DI_DIR=$(DI_DIR) $(1) 2>&1
endef

diagnostic:
	$(call build_diagnostic,diagnostic)

diagnostic-clean:
	$(call build_diagnostic,clean)

diagnostic-distclean:
	$(call build_diagnostic,distclean)
.PHONY: diagnostic diagnostic-clean diagnostic-distclean


