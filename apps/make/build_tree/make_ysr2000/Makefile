#
# Copyright (C) 2011 HUMAX Co., Ltd. All rights reserved.
#

# define SHELL

LC_ALL		= C
LANG		= C
export SHELL	= /usr/bin/env bash

# define base dir path
HUMAX_MAKE_DIR=$(shell pwd)
HUMAX_TOP_DIR=$(HUMAX_MAKE_DIR)/../..
USER=$(shell echo $(shell whoami) | sed "s/[ ]//g")
APPLICATION_DIR = $(HUMAX_TOP_DIR)/apps

HUMAX_GLOBALENV_SODIR=$(HUMAX_MAKE_DIR)/global_make/sharedobject
HUMAX_GLOBALENV_BINDIR=$(HUMAX_MAKE_DIR)/global_make/executable
HUMAX_GLOBALENV_CONFDIR=$(HUMAX_MAKE_DIR)/global_make

# define each dir path
HUMAX_PLATFORM_IMG_BASE_DIR=$(HUMAX_TOP_DIR)/device/images
HUMAX_PLATFORM_MAKE_DIR=$(HUMAX_TOP_DIR)/device/make
HUMAX_PLATFORM_INC_DIR=$(HUMAX_TOP_DIR)/device/include
HUMAX_TOOL_DIR=$(HUMAX_TOP_DIR)/model/tool
HUMAX_PACKAGE_DIR=$(HUMAX_TOP_DIR)/packages
HUMAX_STATIC_LIB_DIR=$(HUMAX_TOP_DIR)/apps/make/libs/static_lib

# filter makecmdgold and include .config
NOCONFIG_TARGETS := menuconfig oldconfig defconfig
ifeq ($(filter $(NOCONFIG_TARGETS),$(MAKECMDGOALS)),)
-include $(HUMAX_MAKE_DIR)/.config
endif

# if you want to change target folder, please modify HUMAX_INSTALL_NAME.
#HUMAX_INSTALL_NAME = "hmstest"
HUMAX_INSTALL_NAME = $(CONFIG_PRODUCT_NAME)
export HUMAX_INSTALL_NAME

# if .config, then use
ifeq ($(strip $(CONFIG_HAVE_DOT_CONFIG)),y)
HUMAX_NFS_INSTALL_DIR = $(strip $(subst ",,$(CONFIG_PRODUCT_NFSROOT_DIR)/$(USER)/$(HUMAX_INSTALL_NAME)/root))
HUMAX_NFS_TOP_DIR = $(strip $(subst ",,$(CONFIG_PRODUCT_NFSROOT_DIR)/$(USER)/$(HUMAX_INSTALL_NAME)))
PATH=$(shell echo $(CONFIG_TOOLCHAIN_PATH) | sed "/^.\"/d")/bin:/bin:/sbin:/usr/bin:/usr/sbin
export HUMAX_NFS_INSTALL_DIR
export HUMAX_NFS_TOP_DIR
endif

export HUMAX_MAKE_DIR
export HUMAX_GLOBALENV_SODIR
export HUMAX_GLOBALENV_BINDIR
export HUMAX_GLOBALENV_CONFDIR

# top kconfig dir path
HUMAX_KCONFIG_TOOL_DIR=$(HUMAX_TOP_DIR)/model/tool/kconfig

# project defconfig dir path
HUMAX_PRODUCT_NAME=$(shell echo $@ | sed "s/[_].*//g")
HUMAX_DEFCONFIG_DIR=$(HUMAX_MAKE_DIR)/product_configs
HUMAX_PROJECT_KCONFIG_FILE=$(HUMAX_MAKE_DIR)/octo_Kconfig/Kconfig
HUMAX_KCONFIG_PARAMS= Kconfig=$(HUMAX_PROJECT_KCONFIG_FILE) KconfigOutputDir=$(HUMAX_MAKE_DIR)
ifeq ($(CONFIG_DEVICE_DEBUG),y)
HUMAX_DBG_MODEL_FOLDER = debug
else
HUMAX_DBG_MODEL_FOLDER = release
endif
HUMAX_PLATFORM_SRC = $(strip $(subst ",,$(HUMAX_PLATFORM_IMG_BASE_DIR)/$(CONFIG_PRODUCT_NAME)/platforms/$(HUMAX_DBG_MODEL_FOLDER)))
HUMAX_PLATFORM_LIB_SRC = $(strip $(subst ",,$(HUMAX_PLATFORM_IMG_BASE_DIR)/$(CONFIG_PRODUCT_NAME)/libs/$(HUMAX_DBG_MODEL_FOLDER)))

# include sub-makefiles by .config
ifeq ($(strip $(CONFIG_HAVE_DOT_CONFIG)),y)
include $(HUMAX_MAKE_DIR)/Makefile.app_new
include $(HUMAX_MAKE_DIR)/Makefile.extract
include $(HUMAX_MAKE_DIR)/Makefile.driver
include $(HUMAX_MAKE_DIR)/Makefile.cas
include $(HUMAX_MAKE_DIR)/Makefile.3rdparty
include $(HUMAX_MAKE_DIR)/Makefile.browser
include $(HUMAX_MAKE_DIR)/Makefile.tvbrowser
include $(HUMAX_MAKE_DIR)/Makefile.buildtree
include $(HUMAX_MAKE_DIR)/Makefile.app_web
endif
include $(HUMAX_MAKE_DIR)/Makefile.squashfs
ifneq ($(strip $(CONFIG_HAVE_DOT_CONFIG)),y)
include $(HUMAX_MAKE_DIR)/Makefile.noconfig
endif

OCTOLIBS := appkit
OCTOLIBSCLEAN := appkit-clean
OCTOSERVICEAPPS := dama hama comma pama thma sama homma umma hamalauncher tima
OCTOSERVICELIBS := dapi hapi papi samalib sapi homapi ummalib uapi thapi tapi
OCTOSERVICECLEANAPPS := dama-clean hama-clean pama-clean comma-clean thma-clean imma-clean sama-clean homma-clean umma-clean hamalauncher-clean tima-clean
OCTOSERVICECLEANLIBS := dapi-clean hapi-clean papi-clean samalib-clean sapi-clean homapi-clean ummalib-clean uapi-clean thapi-clean tapi-clean

ifeq ($(CONFIG_HUMAX_CRASHREPORT),y)
OCTOLIBS += hcrlib
OCTOLIBSCLEAN += hcrlib-clean
endif
ifeq ($(CONFIG_SUPPORT_IMMA),y)
OCTOSERVICEAPPS += imma
OCTOSERVICECLEANAPPS += imma-clean
endif
ifeq ($(CONFIG_SUPPORT_NIMA),y)
ifeq ($(CONFIG_ENHANCED_SECURITY),y)
OCTOSERVICEAPPS += senima
OCTOSERVICECLEANAPPS += senima-clean
endif
OCTOSERVICEAPPS += nima
OCTOSERVICELIBS += napi
OCTOSERVICECLEANAPPS += nima-clean
OCTOSERVICECLEANLIBS += napi-clean
endif

ifeq ($(CONFIG_SUPPORT_REMA),y)
OCTOSERVICEAPPS += rema 
OCTOSERVICECLEANAPPS += rema-clean
endif


APPL_LIST := cmduartconsole keyboard tr069 prism ap_browser ap_tvbrowser ap_web
APPLCLEAN_LIST := cmduartconsole-clean keyboard-clean prism-clean tr069-clean ap_browser_clean ap_web_clean

ifeq ($(CONFIG_SUPPORT_NODE_RESTAPI),y)
APPL_LIST += ap_node restnode nodebinder
APPLCLEAN_LIST += ap_node_clean restnode-clean nodebinder-clean
endif


octolibs    : $(OCTOLIBS)
octoservice : $(OCTOSERVICEAPPS)
octosvclib  : $(OCTOSERVICELIBS)
octotestkit : otestkit
octo        : octolibs octosvclib octoservice
appl        : $(APPL_LIST)

octolibs-clean    : $(OCTOLIBSCLEAN)
octoservice-clean : $(OCTOSERVICECLEANAPPS)
octosvclib-clean  : $(OCTOSERVICECLEANLIBS)
octotestkit-clean : otestkit-clean
octo-clean        : octolibs-clean octosvclib-clean octoservice-clean
appl-clean        : $(APPLCLEAN_LIST)

app : ap_cas ap_3rdparty ap_driver octo appl
app-clean : ap_cas_clean ap_3rdparty_clean ap_driver_clean octo-clean appl-clean

ifeq ($(CONFIG_ENHANCED_SECURITY),y)
app_world : basefiles app secure_platform
else
app_world : basefiles app
endif

platform_world :
#	Platform defconfig
ifeq ($(CONFIG_DEVICE_DEBUG),y)
	@if [ -d $(HUMAX_PLATFORM_MAKE_DIR) ]; then \
		$(MAKE) -C  $(HUMAX_PLATFORM_MAKE_DIR) $(CONFIG_PRODUCT_PLATFORM_NAME)_noapp_defconfig; \
	fi;
else
	@if [ -d $(HUMAX_PLATFORM_MAKE_DIR) ]; then \
		$(MAKE) -C  $(HUMAX_PLATFORM_MAKE_DIR) $(CONFIG_PRODUCT_PLATFORM_NAME)_noapp_release_defconfig; \
	fi;
endif
	echo $(HUMAX_PLATFORM_MAKE_DIR)
	@if [ -d $(HUMAX_PLATFORM_MAKE_DIR) ]; then \
		$(MAKE) -C  $(HUMAX_PLATFORM_MAKE_DIR) world; \
	fi;

app_squashfs: squashfs-tools
	@echo app_squashfs!!

app_squashfs-clean: squashfs-tools-clean
	@echo app_squashfs-clean!!

app_squashfs-distclean: squashfs-tools-distclean
	@echo $@!!

world: platform_world app_world app_squashfs FORCE
	@echo world!!

menuconfig: FORCE
	@$(MAKE) -C $(HUMAX_KCONFIG_TOOL_DIR) menuconfig $(HUMAX_KCONFIG_PARAMS)
	@$(MAKE) -C $(HUMAX_KCONFIG_TOOL_DIR) silentoldconfig $(HUMAX_KCONFIG_PARAMS)
	@cp $(CURDIR)/autoconf.h $(CURDIR)/include
	$(CURDIR)/GlobalConf

%_defconfig: FORCE
	@echo $(HUMAX_PRODUCT_NAME) project build start !!
	@cp $(shell find $(HUMAX_DEFCONFIG_DIR)/ -name $@) $(HUMAX_MAKE_DIR)/.config
	@$(MAKE) -C $(HUMAX_KCONFIG_TOOL_DIR) silentoldconfig $(HUMAX_KCONFIG_PARAMS)
	@test -d $(CURDIR)/include || install -d -m 755 $(CURDIR)/include
	@cp $(CURDIR)/autoconf.h $(CURDIR)/include
	$(CURDIR)/GlobalConf

clean_nfs:
	@sudo rm -rf $(HUMAX_NFS_INSTALL_DIR)

# build platfotm's driver and build driver image files
platform_driver:
	@if [ -d $(HUMAX_PLATFORM_MAKE_DIR) ]; then \
		$(MAKE) -C  $(HUMAX_PLATFORM_MAKE_DIR) vkernel; \
		$(MAKE) -C  $(HUMAX_PLATFORM_MAKE_DIR) driver_all; \
	fi;

ifeq ($(CONFIG_ENHANCED_SECURITY),y)
driver: platform_driver ap_driver basefiles app secure_platform FORCE
else
driver: platform_driver ap_driver basefiles app FORCE
endif

driver_clean: ap_driver_clean
	@if [ -d $(HUMAX_PLATFORM_MAKE_DIR) ]; then \
		$(MAKE) -C  $(HUMAX_PLATFORM_MAKE_DIR) driver-clean; \
	fi;

di_only:
	$(MAKE) -C  $(HUMAX_PLATFORM_MAKE_DIR) driver_all;

di: di_only ap_driver octo

di_clean:
	$(MAKE) -C  $(HUMAX_PLATFORM_MAKE_DIR) driver-clean;

app_distclean : basefiles_distclean ap_driver_distclean ap_3rdparty_distclean ap_cas_distclean app-clean ap_browser_distclean app_squashfs-distclean ap_tvbrowser_distclean
	@rm -f .config
	@rm -f .config.old
	@rm -r .globalconfig
	@rm -rf $(CURDIR)/include
	@sudo rm -rf $(CURDIR)/_gen_rootfs/image
	@for delpath in $(CURDIR)/_gen_rootfs/root/*; do $(CURDIR)/distclean_gen_rootfs.sh $$delpath; done

platform_distclean:
ifeq ($(strip $(CONFIG_HAVE_DOT_CONFIG)),y)
	@if [ -d $(HUMAX_PLATFORM_MAKE_DIR) ]; then \
		$(MAKE) -C  $(HUMAX_PLATFORM_MAKE_DIR) distclean; \
	fi;
endif
ifneq ($(strip $(CONFIG_HAVE_DOT_CONFIG)),y)
	@echo you have to do xxx_defconfig, at first. in $@
endif

distclean: app_distclean platform_distclean clean_nfs FORCE
	@echo distclean!!

FORCE:
