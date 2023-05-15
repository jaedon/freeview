#
# config file for the Opera Build Environment
#
# Wrriten by Jongwon Kim.
# 2011. 06. 17
#

################################################################################
# Global Configuration
################################################################################
-include $(PWD)/.globalconfig
-include $(PWD)/global_make/HumaxGlobalConf.mak

################################################################################
# User configuration
# the user configuration should be set by own development environment.
################################################################################
ifeq ($(CONFIG_PROD_CPU_BCM_ARM), y)
CHIP_CORE := arm
else
CHIP_CORE := mipsel
endif

CROSS	:= $(CONFIG_TOOLCHAIN_PATH)/bin/$(CHIP_CORE)-linux-
NATIVE_APP_PATH			:= $(PWD)/../../

################################################################################
# BUILD configuration
# The following configurations are managed by the opera part engineers.
################################################################################
TOP							:= $(PWD)/../appl/browser/corsair
NATIVE_PORT_PATH			:= $(NATIVE_APP_PATH)/apps/appl/browser/corsair/base-2.0/port/
OPERA_HBBTV_PACKAGE			:= hbbtv-oipf
OPERA_OOIF_PACKAGE			:= opera-linux-devices-sdk

ifeq ($(CHIP_CORE),mipsel)
	ifeq ($(CONFIG_PROD_GCC_GLIBC), y)
	GCC_LIBC_STRING				:= gcc-$(shell $(CONFIG_TOOLCHAIN_PATH)/bin/$(CHIP_CORE)-linux-gcc -dumpversion)-glibc-2.18
	else 
	GCC_LIBC_STRING				:= gcc-$(shell $(CONFIG_TOOLCHAIN_PATH)/bin/$(CHIP_CORE)-linux-gcc -dumpversion)-uClibc-0.9.32
	endif
else ifeq ($(CHIP_CORE),arm)
GCC_LIBC_STRING				:= gcc-$(shell $(CONFIG_TOOLCHAIN_PATH)/bin/$(CHIP_CORE)-linux-gcc -dumpversion)-glibc-2.18
endif

PLATFORM					:= $(CHIP_CORE)
PRODUCT_ENV					:= product-env
BUILD_SRC_DIR				:= $(TOP)/base-2.0/build/src/
BUILD_OUT_DIR				:= $(TOP)/base-2.0/build/out/
OPERA_SDK_HOME				:= $(TOP)/opera-sdk
OPERA_HUMAX_IMPL_HOME		:= $(TOP)/base-2.0/humax-impl/
OPERA_HBBTV_PACKAGE_PATH	:= $(OPERA_SDK_HOME)/$(OPERA_HBBTV_PACKAGE).tar.bz2
OPERA_OOIF_PACKAGE_PATH		:= $(OPERA_SDK_HOME)/$(OPERA_OOIF_PACKAGE).tar.bz2

################################################################################
# Set the env values for corsair and jsplugins
################################################################################
ifeq ($(CONFIG_PROD_HMS1000S),y)
	USE_OPERA_TVSTORE	:=YES
endif
ifeq ($(CONFIG_PROD_HMS1000T),y)
	USE_OPERA_TVSTORE	:=YES
endif

USE_NATIVE_PRISM = NO
