
-include $(HUMAX_MAKE_DIR)/.config
-include $(HUMAX_PLATFORM_CONFIG_DIR)/.config

obj-y := $(HUMAX_HWTEST_DIR)/src/
ifdef CONFIG_CI
obj-y += $(HUMAX_HWTEST_DIR)/src/ci_stack/
endif
obj-y += $(HUMAX_HWTEST_DIR)/src/cmd_hwtest_ext/

inccflag-y := -I$(HUMAX_WORK_DIR)/include/di
inccflag-y += -I$(HUMAX_WORK_DIR)/include/hos
inccflag-y += -I$(HUMAX_WORK_DIR)/include/cmd
inccflag-y += -I$(HUMAX_HWTEST_DIR)/include/
inccflag-y += -I$(HUMAX_HWTEST_DIR)/include/ext
inccflag-y += -I$(HUMAX_HWTEST_DIR)/include/ext/channel
inccflag-y += -I$(HUMAX_HWTEST_DIR)/include/ext/thermal
inccflag-y += -I$(HUMAX_HWTEST_DIR)/src/tester/include/
inccflag-y += -I$(HUMAX_HWTEST_DIR)/src/ci_stack/include
inccflag-y += -I$(HUMAX_HWTEST_DIR)/src/ci_stack/include/ext
inccflag-y += -I$(HUMAX_DRIVER_DIR)/utils/include/
inccflag-y += -I$(HUMAX_DRIVER_DIR)/di/drv/include/

HUMAX_CONFIG_DIR := $(HUMAX_WORK_DIR)/product_config/apps/hwtest
HUMAX_KCONFIG_PARAMS := Kconfig=$(HUMAX_WORK_DIR)/product_config/apps/hwtest/kconfigs/Kconfig KconfigOutputDir=$(HUMAX_HWTEST_MAKE_DIR)
HUMAX_CONFIG_FILE_SRC := $(HUMAX_CONFIG_DIR)/$(CONFIG_PRODUCT_NAME)/hwtest_config.cfg

HUMAX_EXTRA_CFLAG := $(inccflag-y) 
HUMAX_EXTRA_CFLAG += -include $(HUMAX_PLATFORM_CONFIG_DIR)/autoconf.h
HUMAX_EXTRA_CFLAG += -DCONFIG_PRODUCT_NAME=$(CONFIG_PRODUCT_NAME)

ifneq ($(CONFIG_PRODUCT_IMAGE_DEBUG),)
HUMAX_EXTRA_CFLAG += -fstack-protector-all
endif

ifeq ($(CONFIG_PRODUCT_APP_DEBUG_LOG), y)
HUMAX_EXTRA_CFLAG += -DCONFIG_DEBUG=1
endif

ifeq ($(CONFIG_PRODUCT_IMAGE_RELEASE),y)
HUMAX_EXTRA_CFLAG += -DCONFIG_PRODUCT_IMAGE_RELEASE=y
else
HUMAX_EXTRA_CFLAG += -DCONFIG_PRODUCT_IMAGE_DEBUG=y
endif

ifeq ($(CONFIG_DINETWORK),y)
HUMAX_EXTRA_CFLAG += -DCONFIG_DINETWORK=y
endif

export HUMAX_EXTRA_CFLAG
export HUMAX_MODULE=APPS

include $(HUMAX_PORTING_MAKE_DIR)/HumaxToolset.mak

hwtest: BUILD_DATE
	$(HUMAX_MAKE) -f $(HUMAX_MAKE_DIR)/HumaxDefaultRule.mak HUMAX_CONFIG_DIR=$(HUMAX_PLATFORM_CONFIG_DIR) HUMAX_OBJ='$(obj-y)' LIBNAME=$@ static-library

BUILD_DATE: FORCE
	@echo "const char *g_pszBuildDate = \"`date +%G%m%d-%H%M%S`;\";" > $(HUMAX_HWTEST_DIR)/src/hwtest_build_date.c

clean_hwtest:
	$(HUMAX_MAKE) -f $(HUMAX_MAKE_DIR)/HumaxDefaultRule.mak HUMAX_CONFIG_DIR=$(HUMAX_PLATFORM_CONFIG_DIR) HUMAX_OBJ='$(obj-y)' clean
	rm -f $(HUMAX_HWTEST_DIR)/src/hwtest_build_date.c

FORCE:
