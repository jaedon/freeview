#
# Copyright (C) 2011 HUMAX Co., Ltd. All rights reserved.
#

# define SHELL

LC_ALL		= C
LANG		= C
export SHELL	= /usr/bin/env bash

# define base dir path
HUMAX_MAKE_DIR=$(shell pwd)
HUMAX_WORK_DIR=$(subst /make,,$(HUMAX_MAKE_DIR))
USER=$(shell echo $(shell whoami) | sed 's/[ ]//g')

# define each dir path
HUMAX_PRODUCT_CONFIG_DIR=$(HUMAX_WORK_DIR)/product_config
HUMAX_PLATFORM_DIR=$(HUMAX_WORK_DIR)/platform
HUMAX_TOOL_DIR=$(HUMAX_WORK_DIR)/../model/tool
NEXUS_TOP=$(HUMAX_WORK_DIR)/platform/sdk/driver/nexus

# define Makefile Path
HUMAX_PORTING_MAKE_DIR=$(HUMAX_PLATFORM_DIR)/porting_make
HUMAX_DRIVER_DIR=$(HUMAX_PLATFORM_DIR)/driver
HUMAX_PACKAGE_MAKE_DIR=$(HUMAX_WORK_DIR)/../packages/
HUMAX_HWTEST_MAKE_DIR=$(HUMAX_WORK_DIR)/apps/make/hwtest_make

# filter makecmdgold and include .config
NOCONFIG_TARGETS := menuconfig oldconfig defconfig
ifeq ($(filter $(NOCONFIG_TARGETS),$(MAKECMDGOALS)),)
	-include $(HUMAX_MAKE_DIR)/.config
	HUMAX_PLATFORM_CONFIG_DIR=$(HUMAX_MAKE_DIR)/$(shell echo $(CONFIG_PRODUCT_NAME) | sed '/^.\"/d')
	-include $(HUMAX_PLATFORM_CONFIG_DIR)/.config
	-include .tools
endif

# if .config, then use
ifeq ($(strip $(CONFIG_BCM_DOT_CONFIG)),y)
	PLATFORM=$(shell echo $(CONFIG_PLATFORM) | sed '/^.\"/d')
	BCHP_VER=$(shell echo $(CONFIG_CHIP_REV) | sed '/^.\"/d')
	BCHP_VER_LOWER=$(shell awk 'BEGIN{print tolower("$(BCHP_VER)")}')
	BCHP_CHIP=$(shell echo $(CONFIG_CHIP) | sed '/^.\"/d')
	HUMAX_USER_DIR=$(CONFIG_PRODUCT_NFSROOT_DIR)/$(USER)
	HUMAX_NFS_DIR=$(HUMAX_USER_DIR)/$(CONFIG_PRODUCT_NAME)
	HUMAX_NFS_INSTALL_DIR=$(shell echo $(CONFIG_PRODUCT_NFSROOT_DIR)/$(USER)/$(CONFIG_PRODUCT_NAME)/root | sed '/^.\"/d')
	HUMAX_PRODUCT_BUILD_NAME=$(shell echo $(CONFIG_PRODUCT_NAME) | sed '/^.\"/d')

	HUMAX_BOARD_REVISION=$(shell echo $(CONFIG_BOARD_REVISION) | sed '/^.\"/d')
	HUMAX_PREPARE_DIR=$(HUMAX_MAKE_DIR)/prepare
	HUMAX_PREPARE_ROOT_DIR=$(HUMAX_PREPARE_DIR)/rootfs-package
	PATH=$(shell echo $(CONFIG_TOOLCHAIN_PATH) | sed '/^.\"/d')/bin:/bin:/sbin:/usr/bin:/usr/sbin
	export HUMAX_NFS_INSTALL_DIR
endif

# top kconfig dir path
HUMAX_KCONFIG_TOOL_DIR=$(HUMAX_TOOL_DIR)/kconfig

# project defconfig dir path
HUMAX_PRODUCT_NAME=$(shell echo $@ | sed 's/[_].*//g')
HUMAX_DEFCONFIG_DIR=$(HUMAX_PRODUCT_CONFIG_DIR)/defconfigs/$(HUMAX_PRODUCT_NAME)
HUMAX_PROJECT_KCONFIG_FILE=$(HUMAX_PRODUCT_CONFIG_DIR)/defconfigs/kconfigs/Kconfig
HUMAX_KCONFIG_PARAMS= Kconfig=$(HUMAX_PROJECT_KCONFIG_FILE) KconfigOutputDir=$(HUMAX_MAKE_DIR)

# platform defconfig dir path
HUMAX_PLATFORM_DOT_CONFIG_DIR=$(HUMAX_MAKE_DIR)/$(CONFIG_PRODUCT_NAME)
HUMAX_PLATFORM_KCONFIG_FILE=$(HUMAX_PRODUCT_CONFIG_DIR)/defconfigs/kconfigs/Kconfig.bcm
HUMAX_PLATFORM_KCONFIG_PARAMS= Kconfig=$(HUMAX_PLATFORM_KCONFIG_FILE) KconfigOutputDir=$(HUMAX_PLATFORM_DOT_CONFIG_DIR)

# library output dir path
HUMAX_OUTPUT_DIR := $(HUMAX_MAKE_DIR)/output

# include bcm_environment.config for using NEXUS_XXX variable here
include $(NEXUS_TOP)/../bcm_environment.config

# include sub-makefiles by .config
ifeq ($(strip $(CONFIG_BCM_DOT_CONFIG)),y)
	TARGETS =
	include $(HUMAX_PORTING_MAKE_DIR)/HumaxToolset.mak
	include $(HUMAX_PORTING_MAKE_DIR)/HumaxPlatform.mak
	include $(HUMAX_MAKE_DIR)/Makefile.basefiles
	include $(HUMAX_MAKE_DIR)/Makefile.AppLibs
	include $(HUMAX_MAKE_DIR)/Makefile.package
	include $(HUMAX_MAKE_DIR)/Makefile.3rdparty
	include $(HUMAX_MAKE_DIR)/Makefile.driver
ifeq ($(CONFIG_CABLE_MODEM),y)
	include $(HUMAX_MAKE_DIR)/Makefile.dsgcc
ifeq ($(CONFIG_ARM),y)
else
	include $(HUMAX_MAKE_DIR)/Makefile.pcie
endif
endif
	include $(HUMAX_MAKE_DIR)/Makefile.main
	include $(HUMAX_MAKE_DIR)/Makefile.image
	include $(HUMAX_MAKE_DIR)/Makefile.app
	include $(HUMAX_MAKE_DIR)/Makefile.squashfs
ifdef CONFIG_AUTO_BUILD_IMAGE
	include $(HUMAX_MAKE_DIR)/Makefile.buildImage
endif
ifeq ($(CONFIG_INITRAMFS_LINUX),y)
	TARGETS += kernel
endif
endif

# include TARGETS and use clean distclean on activate TARGETS
reverse = $(if $1,$(call reverse,$(wordlist 2,$(words $1),$1)) $(firstword $1))
TARGETS_CLEAN = $(call reverse, $(patsubst %,%-clean,$(filter-out $(IGNORE),$(TARGETS))))
TARGETS_DISTCLEAN = $(call reverse, $(patsubst %,%-distclean,$(filter-out $(IGNORE),$(TARGETS))))

# find target and re-order TARGETS
find_string=$(if $(filter $2,$1),$(call find_string,$(filter-out $(word $(words $1), $1), $1),$2) $(word $(words $1),$1))
PART_TARGETS=$(call find_string,$(TARGETS),$(subst _all,,$(MAKECMDGOALS)))

define rm-kconfig-include
	@rm -rf $(HUMAX_KCONFIG_TOOL_DIR)/include
endef

define rm-dot-config-space
	@mv $1 $1.org
	sed 's/ *$$//' $1.org > $1
	@rm $1.org
endef

# prepare0 oldconfig for specific platform when .config is valid.
ifeq ($(CONFIG_HAVE_DOT_CONFIG),y)
$(HUMAX_PLATFORM_CONFIG_DIR):
	@if [ ! -d $(CONFIG_PRODUCT_NAME) ]; then \
		mkdir -p $(CONFIG_PRODUCT_NAME); \
	fi

ifneq ($(CONFIG_PRODUCT_IMAGE_FACTORY),)
	if [ -f $(HUMAX_WORK_DIR)/product_config/defconfigs/$(CONFIG_PRODUCT_NAME)/$(CONFIG_BOARD_REVISION)/platform_factory_defconfig ]; then \
		cp $(HUMAX_WORK_DIR)/product_config/defconfigs/$(CONFIG_PRODUCT_NAME)/$(CONFIG_BOARD_REVISION)/platform_factory_defconfig ./$(CONFIG_PRODUCT_NAME)/.config; \
	else \
		cp $(HUMAX_WORK_DIR)/product_config/defconfigs/$(CONFIG_PRODUCT_NAME)/$(CONFIG_BOARD_REVISION)/platform_defconfig ./$(CONFIG_PRODUCT_NAME)/.config; \
	fi
else ifneq ($(CONFIG_PRODUCT_IMAGE_HWTEST),)
	if [ -f $(HUMAX_WORK_DIR)/product_config/defconfigs/$(CONFIG_PRODUCT_NAME)/$(CONFIG_BOARD_REVISION)/platform_hwtest_defconfig ]; then \
		cp $(HUMAX_WORK_DIR)/product_config/defconfigs/$(CONFIG_PRODUCT_NAME)/$(CONFIG_BOARD_REVISION)/platform_hwtest_defconfig ./$(CONFIG_PRODUCT_NAME)/.config; \
	else \
		cp $(HUMAX_WORK_DIR)/product_config/defconfigs/$(CONFIG_PRODUCT_NAME)/$(CONFIG_BOARD_REVISION)/platform_defconfig ./$(CONFIG_PRODUCT_NAME)/.config; \
	fi
else ifneq ($(CONFIG_PRODUCT_IMAGE_UOCTO),)
	if [ -f $(HUMAX_WORK_DIR)/product_config/defconfigs/$(CONFIG_PRODUCT_NAME)/$(CONFIG_BOARD_REVISION)/platform_uocto_defconfig ]; then \
		cp $(HUMAX_WORK_DIR)/product_config/defconfigs/$(CONFIG_PRODUCT_NAME)/$(CONFIG_BOARD_REVISION)/platform_uocto_defconfig ./$(CONFIG_PRODUCT_NAME)/.config; \
	else \
		cp $(HUMAX_WORK_DIR)/product_config/defconfigs/$(CONFIG_PRODUCT_NAME)/$(CONFIG_BOARD_REVISION)/platform_defconfig ./$(CONFIG_PRODUCT_NAME)/.config; \
	fi
else ifneq ($(CONFIG_PRODUCT_IMAGE_UPGRADER),)
	if [ -f $(HUMAX_WORK_DIR)/product_config/defconfigs/$(CONFIG_PRODUCT_NAME)/$(CONFIG_BOARD_REVISION)/platform_upgrader_defconfig ]; then \
		cp $(HUMAX_WORK_DIR)/product_config/defconfigs/$(CONFIG_PRODUCT_NAME)/$(CONFIG_BOARD_REVISION)/platform_upgrader_defconfig ./$(CONFIG_PRODUCT_NAME)/.config; \
	else \
		cp $(HUMAX_WORK_DIR)/product_config/defconfigs/$(CONFIG_PRODUCT_NAME)/$(CONFIG_BOARD_REVISION)/platform_defconfig ./$(CONFIG_PRODUCT_NAME)/.config; \
	fi
else ifneq ($(CONFIG_PRODUCT_IMAGE_LAUNCHER),)
	if [ -f $(HUMAX_WORK_DIR)/product_config/defconfigs/$(CONFIG_PRODUCT_NAME)/$(CONFIG_BOARD_REVISION)/platform_launcher_defconfig ]; then \
		cp $(HUMAX_WORK_DIR)/product_config/defconfigs/$(CONFIG_PRODUCT_NAME)/$(CONFIG_BOARD_REVISION)/platform_launcher_defconfig ./$(CONFIG_PRODUCT_NAME)/.config; \
	else \
		cp $(HUMAX_WORK_DIR)/product_config/defconfigs/$(CONFIG_PRODUCT_NAME)/$(CONFIG_BOARD_REVISION)/platform_defconfig ./$(CONFIG_PRODUCT_NAME)/.config; \
	fi
else ifneq ($(CONFIG_SUPPORT_NETFLIX_TEMP),)
	if [ -f $(HUMAX_WORK_DIR)/product_config/defconfigs/$(CONFIG_PRODUCT_NAME)/$(CONFIG_BOARD_REVISION)/platform_netflix_defconfig ]; then \
		cp $(HUMAX_WORK_DIR)/product_config/defconfigs/$(CONFIG_PRODUCT_NAME)/$(CONFIG_BOARD_REVISION)/platform_netflix_defconfig ./$(CONFIG_PRODUCT_NAME)/.config; \
	else \
		cp $(HUMAX_WORK_DIR)/product_config/defconfigs/$(CONFIG_PRODUCT_NAME)/$(CONFIG_BOARD_REVISION)/platform_defconfig ./$(CONFIG_PRODUCT_NAME)/.config; \
	fi	
else ifeq ($(CONFIG_PRODUCT_IMAGE_RELEASE)$(CONFIG_DEVELOP),yy)
	if [ -f $(HUMAX_WORK_DIR)/product_config/defconfigs/$(CONFIG_PRODUCT_NAME)/$(CONFIG_BOARD_REVISION)/platform_develop_defconfig ]; then \
		cp $(HUMAX_WORK_DIR)/product_config/defconfigs/$(CONFIG_PRODUCT_NAME)/$(CONFIG_BOARD_REVISION)/platform_develop_defconfig ./$(CONFIG_PRODUCT_NAME)/.config; \
	else \
		cp $(HUMAX_WORK_DIR)/product_config/defconfigs/$(CONFIG_PRODUCT_NAME)/$(CONFIG_BOARD_REVISION)/platform_defconfig ./$(CONFIG_PRODUCT_NAME)/.config; \
	fi	
else
	cp $(HUMAX_WORK_DIR)/product_config/defconfigs/$(CONFIG_PRODUCT_NAME)/$(CONFIG_BOARD_REVISION)/platform_defconfig ./$(CONFIG_PRODUCT_NAME)/.config
endif
	$(call rm-dot-config-space,./$(CONFIG_PRODUCT_NAME)/.config)
endif

# prepare directories to build
show-build:
	@echo
	@echo
	@echo ============================
	@echo
	@echo Building Start !!!!
	@echo
	@echo Product Name : $(CONFIG_PRODUCT_NAME)
	@echo Platform Name : $(PLATFORM)
	@echo Platform Chip : $(BCHP_CHIP)$(BCHP_VER)
	@echo Board Rev : $(CONFIG_BOARD_REVISION)
	@echo
	@echo ============================
	@echo
	@echo Building orders : $(TARGETS)
	@echo
	@echo ============================

prepare1: FORCE
ifeq ($(shell expr $(NEXUS_TREE) ">=" TREE_1303),1)
	@if [ ! -d $(NEXUS_TOP)/bin ]; then \
		ln -sf $(NEXUS_TOP)/../obj.$(PLATFORM)/nexus/bin $(NEXUS_TOP)/bin; \
	fi;
endif

	@if [ ! -d $(HUMAX_NFS_DIR) ]; then \
		mkdir -p $(HUMAX_NFS_DIR); \
	fi;

	@if [ ! -d $(HUMAX_NFS_DIR)/image ]; then \
		mkdir -p $(HUMAX_NFS_DIR)/image; \
	fi;

	@if [ ! -d $(HUMAX_PREPARE_DIR) ]; then \
		mkdir -p $(HUMAX_PREPARE_DIR) ; \
	fi;

	@if [ ! -d $(HUMAX_PREPARE_ROOT_DIR) ]; then \
		mkdir -p $(HUMAX_PREPARE_ROOT_DIR) ; \
	fi;
ifdef CONFIG_INITRAMFS_LINUX
	@if [ ! -d $(HUMAX_NFS_INSTALL_DIR) ]; then \
		mkdir -p $(HUMAX_NFS_INSTALL_DIR) ; \
	fi;
endif

world-targets: show-build prepare1 $(TARGETS) FORCE

world:
ifeq ($(strip $(CONFIG_BCM_DOT_CONFIG)),y)
	@$(MAKE) platform-oldconfig
else
	@$(MAKE) platform-defconfig
endif
	@$(MAKE) world-targets

$(HUMAX_PLATFORM_CONFIG_DIR)/autoconf.h: $(HUMAX_PLATFORM_CONFIG_DIR)/.config
	$(call rm-kconfig-include,)
	@$(MAKE) -C $(HUMAX_KCONFIG_TOOL_DIR) silentoldconfig $(HUMAX_PLATFORM_KCONFIG_PARAMS)

platform-oldconfig: $(HUMAX_PLATFORM_CONFIG_DIR) $(HUMAX_PLATFORM_CONFIG_DIR)/autoconf.h

platform-menuconfig: $(HUMAX_PLATFORM_CONFIG_DIR)
	$(call rm-kconfig-include,)
	@$(MAKE) -C $(HUMAX_KCONFIG_TOOL_DIR) menuconfig $(HUMAX_PLATFORM_KCONFIG_PARAMS)

platform-defconfig: $(HUMAX_PLATFORM_CONFIG_DIR)
	$(call rm-kconfig-include,)
	@$(MAKE) -C $(HUMAX_KCONFIG_TOOL_DIR) silentoldconfig $(HUMAX_PLATFORM_KCONFIG_PARAMS)

%_all : $(PART_TARGETS) FORCE
	@echo "Compiled" $(PART_TARGETS)

menuconfig: FORCE
	@$(MAKE) -C $(HUMAX_KCONFIG_TOOL_DIR) menuconfig $(HUMAX_KCONFIG_PARAMS)

%_defconfig:
	@rm -rf .config .config.old
	@cp $(shell find $(HUMAX_DEFCONFIG_DIR)/ -name $(HUMAX_PRODUCT_NAME)_defconfig) $(HUMAX_MAKE_DIR)/.config
	@sh config.sh $@ .config
	$(call rm-dot-config-space,.config)
	$(call rm-kconfig-include,)
	@$(MAKE) -C $(HUMAX_KCONFIG_TOOL_DIR) silentoldconfig $(HUMAX_KCONFIG_PARAMS)
	@echo Oprofile support = $(OPROFILE)
ifeq ($(OPROFILE),y)
	@echo CONFIG_TOOLS_OPROFILE=y > .tools
else
	@if [ -f .tools ]; then \
		rm .tools; \
	fi
endif
ifeq ($(AUTOCRCTEST),y)
	@echo CRC AUTOTEST ENABLED
	@echo CONFIG_CRC_AUTOTEST=y > .crc_config
else
	@if [ -f .crc_config ]; then \
		rm .crc_config; \
	fi
endif
ifeq ($(AUTOMEMTEST),y)
	@echo MEM AUTOTEST ENABLED
	@echo CONFIG_MEM_AUTOTEST=y > .mem_config
else
	@if [ -f .mem_config ]; then \
		rm .mem_config; \
	fi
endif

config-clean:
	-@rm -f $(HUMAX_MAKE_DIR)/autoconf.h
	-@rm -f $(HUMAX_MAKE_DIR)/.config
	-@rm -f $(HUMAX_MAKE_DIR)/.config.old
	-@rm -rf $(HUMAX_MAKE_DIR)/$(CONFIG_PRODUCT_NAME)

clean_nfs:
	@rm -rf $(HUMAX_NFS_INSTALL_DIR)

clean: $(TARGETS_CLEAN)
	@echo clean!!

distclean: $(TARGETS_DISTCLEAN)
	@rm -f $(HUMAX_MAKE_DIR)/autoconf.h
	@rm -f $(HUMAX_MAKE_DIR)/.config
	@rm -f $(HUMAX_MAKE_DIR)/.config.old
	@rm -rf $(HUMAX_MAKE_DIR)/output
	@rm -rf $(HUMAX_MAKE_DIR)/prepare
ifneq ($(CONFIG_PRODUCT_NAME),)
	@if [ -d $(HUMAX_MAKE_DIR)/$(CONFIG_PRODUCT_NAME) ]; then \
		rm -r $(HUMAX_MAKE_DIR)/$(CONFIG_PRODUCT_NAME); \
	fi
endif
	@rm -f $(HUMAX_MAKE_DIR)/System.map
ifeq ($(shell expr $(NEXUS_TREE) ">=" TREE_1303),1)
	@rm -f $(NEXUS_TOP)/bin
endif
	@echo distclean!!

FORCE:
