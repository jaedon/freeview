HUMAX_NATIVE_RESOURCE_PATH := $(HUMAX_TOP_DIR)/apps/octo/library/ondk/resource
HUMAX_STRINGS_ENV := $(HUMAX_NATIVE_RESOURCE_PATH)/string/model
HUMAX_STRINGS_PATH := $(HUMAX_NATIVE_RESOURCE_PATH)/string/strings
HUMAX_FONT_PATH := $(HUMAX_NATIVE_RESOURCE_PATH)/fonts
HUMAX_AUTOCONF := $(HUMAX_MAKE_DIR)/include/autoconf.h
HUMAX_APPL_DIR := $(HUMAX_TOP_DIR)/apps/appl
HUMAX_NATIVEAPP_DIR := $(HUMAX_APPL_DIR)/nativeapp

HUMAX_MKDIR := mkdir
HUMAX_CP    := cp
ifeq ($(CONFIG_ENHANCED_SECURITY), y)
HUMAX_SUDO	:= sudo
endif


RESOURCE_STR_CONFIG += export STRTOOL_PROD_COUNTRY="strgen_str_ucs2.txt"
RESOURCE_STR_CONFIG += export CONFIG_PRODUCT_NAME=$(CONFIG_PRODUCT_NAME)
ifeq ($(CONFIG_APUI_NATIVE_APP), y)

# SKIN Theme (Default Prism Alpha)
THEME01_DIR	:= prism
ifeq ($(CONFIG_APUI_SKIN_PRISM),y)
THEME01_DIR	:= prism
endif
ifeq ($(CONFIG_APUI_SKIN_MBC),y)
THEME01_DIR	:= mbc
endif

RESOURCE_STR_FILE		 = nxres_id_$(CONFIG_PRODUCT_NAME).h
RESOURCE_STR_PATH		 = $(HUMAX_NATIVEAPP_DIR)/include
RESOURCE_STR_ORG_FILE		= $(HUMAX_NATIVEAPP_DIR)/$(CONFIG_APUI_NAME)/resource/string/$(RESOURCE_STR_FILE)
RESOURCE_STR_CONFIG 		+= export CONFIG_APUI_NATIVE_APP=$(CONFIG_APUI_NATIVE_APP)
RESOURCE_STR_CONFIG 		+= export CONFIG_RES_STRING_FILE=$(RESOURCE_STR_ORG_FILE)
RESOURCE_STR_CONFIG 		+= export CONFIG_RES_STRING_OUTPUT_FILE=$(RESOURCE_STR_PATH)/nxres_strid.h
endif

RESOURCE_IMG_CONFIG += export RESOURCE_IMGTOOL_HEADERPATH=$(HUMAX_NATIVEAPP_DIR)/include/nxres_imgid.h
RESOURCE_IMG_CONFIG += export RESOURCE_IMGTOOL_CFG_SRCPATH=$(HUMAX_NATIVEAPP_DIR)/$(CONFIG_APUI_NAME)/resource/image/$(THEME01_DIR)
RESOURCE_IMG_CONFIG += export RESOURCE_IMGTOOL_CFG_DSTPATH=$(HUMAX_NFS_INSTALL_DIR)/usr/image

RESOURCE_IMG_CONFIG += export RESOURCE_IMGCFG_PVR=$(CONFIG_MW_MM_PVR)
RESOURCE_IMG_CONFIG += export RESOURCE_IMGCFG_BASE=1
RESOURCE_IMG_CONFIG += export RESOURCE_IMGCFG_MEDIA=$(CONFIG_MW_MULTIMEDIA)
RESOURCE_IMG_CONFIG += export RESOURCE_IMGCFG_SAT=$(CONFIG_MW_CH_SATELLITE)
RESOURCE_IMG_CONFIG += export RESOURCE_IMGCFG_CAB=$(CONFIG_MW_CH_CABLE)
RESOURCE_IMG_CONFIG += export RESOURCE_IMGCFG_ANALOG=$(CONFIG_MW_CH_ANALOG)
RESOURCE_IMG_CONFIG += export RESOURCE_IMGCFG_TER=$(CONFIG_MW_CH_TERRESTRIAL)
RESOURCE_IMG_CONFIG += export RESOURCE_IMGCFG_CA_CX=$(CONFIG_MW_CAS_CONAX)
RESOURCE_IMG_CONFIG += export RESOURCE_IMGCFG_CA_NA=$(CONFIG_MW_CAS_NAGRA)
RESOURCE_IMG_CONFIG += export RESOURCE_IMGCFG_CA_IR=$(CONFIG_MW_CAS_IRDETO)
RESOURCE_IMG_CONFIG += export RESOURCE_IMGCFG_CA_VA=$(CONFIG_MW_CAS_VIACCESS)
RESOURCE_IMG_CONFIG += export RESOURCE_IMGCFG_NETWORK=$(CONFIG_MW_INET)
RESOURCE_IMG_CONFIG += export RESOURCE_IMGCFG_TVAPP=$(CONFIG_MWC_OSD_PROFILE_BROWSER)
RESOURCE_IMG_CONFIG += export RESOURCE_IMGCFG_GIF=1
RESOURCE_IMG_CONFIG += export RESOURCE_IMGCFG_BLUETOOTH=$(CONFIG_MW_BLUETOOTH)
RESOURCE_IMG_CONFIG += export RESOURCE_IMGCFG_SCREENSAVER=$(CONFIG_APUI_SCREEN_SAVER)
RESOURCE_IMG_CONFIG += export RESOURCE_IMGCFG_TELETEXT=$(CONFIG_MW_TELETEXT)
RESOURCE_IMG_CONFIG += export RESOURCE_IMGCFG_OP_MBC=$(CONFIG_APUI_SKIN_MBC)
RESOURCE_IMG_CONFIG += export RESOURCE_IMGCFG_$(strip $(subst ",,$(CONFIG_PRODUCT_NAME)))=1


nativeresource:
	@sudo $(MAKE) -C $(HUMAX_NATIVE_RESOURCE_PATH)/imagetool
	@sudo $(MAKE) -C $(HUMAX_NATIVE_RESOURCE_PATH)/stringtool

	@if [ ! -d $(HUMAX_NFS_INSTALL_DIR)/usr/string/  ]; then \
		mkdir -p $(HUMAX_NFS_INSTALL_DIR)/usr/string/;	\
	fi;\

	@if [ ! -d $(HUMAX_NFS_INSTALL_DIR)/usr/fonts/ ]; then \
		mkdir -p $(HUMAX_NFS_INSTALL_DIR)/usr/fonts/;	\
	fi;\

ifeq ($(CONFIG_APUI_NATIVE_APP), y)
	@sudo echo ==========================
	@sudo echo =   Enable NATIVE APP    =
	@sudo echo ==========================
	@sudo touch $(RESOURCE_STR_ORG_FILE)
else
	@sudo echo ==========================
	@sudo echo =   DISABLE NATIVE APP   =
	@sudo echo ==========================
	@sudo find $(HUMAX_APPL_DIR) -name "*.c" | xargs grep -h 'ONDK_GetString' | cut -f 2- -d "O" | cut -f 2- -d "N" | cut -f 2- -d "D" | cut -f 2- -d "K" | cut -f 2- -d "_" | cut -f 2- -d "G" | cut -f 2- -d "e" | cut -f 2- -d "t" | cut -f 2- -d "S" | cut -f 2- -d "t" | cut -f 2- -d "r" | cut -f 2- -d "i" | cut -f 2- -d "n" | cut -f 2- -d "g" | cut -f 2 -d "(" | cut -f -1 -d ")"   > temp
	@sudo sort temp | uniq > $(HUMAX_STRINGS_ENV)/$(CONFIG_PRODUCT_NAME)_using.txt
	@sudo rm temp
endif
	@sudo find $(HUMAX_AUTOCONF) -name "*" | xargs grep -h 'CONFIG_PROD_LANG' | cut -f 2 -d " "  |  cut -f 4 -d "_"  > $(HUMAX_STRINGS_ENV)/$(CONFIG_PRODUCT_NAME)_language.txt
	@sudo rm -rf $(HUMAX_STRINGS_PATH)/*
	@sudo rm -rf $(HUMAX_NFS_INSTALL_DIR)/usr/string/*

	cd $(HUMAX_NATIVE_RESOURCE_PATH)/stringtool/StrGen;$(RESOURCE_STR_CONFIG);sudo chmod 755 strgen;./strgen
ifeq ($(CONFIG_AP_SUPPORT_IMAGETOOL), y)
	cd $(HUMAX_NATIVE_RESOURCE_PATH)/imagetool/ImgGen;$(RESOURCE_IMG_CONFIG);sudo chmod 777 imggen;./imggen
	sudo find $(HUMAX_NFS_INSTALL_DIR)/usr/image/ -name "*.svn*" | xargs rm -rf
endif
ifeq ($(CONFIG_APUI_NATIVE_APP), y)
	$(HUMAX_SUDO) $(HUMAX_CP) -rdf $(HUMAX_FONT_PATH)/nativeapp/* 	$(HUMAX_NFS_INSTALL_DIR)/usr/fonts
else
	$(HUMAX_SUDO) $(HUMAX_CP) -rdf $(HUMAX_FONT_PATH)/webapp/* 	$(HUMAX_NFS_INSTALL_DIR)/usr/fonts
endif
	$(HUMAX_SUDO) $(HUMAX_CP) -rdf $(HUMAX_STRINGS_PATH)/*.txt 	$(HUMAX_NFS_INSTALL_DIR)/usr/string

	@if [ ! -d $(CONFIG_SUPPORT_NODE_RESTAPI) ]; then \
		echo ========================; \
		echo = Web-REST-App Project =; \
		echo ========================; \
		cd $(HUMAX_APPL_DIR)/webappl/base_prism/alpha;./hms1000sph2_setenv.sh; \
		echo $(HUMAX_NFS_INSTALL_DIR); \
		mkdir $(HUMAX_NFS_INSTALL_DIR)/usr/jstbpage; \
		$(HUMAX_CP) -rdf $(HUMAX_APPL_DIR)/webappl/base_prism/alpha.ts/* $(HUMAX_NFS_INSTALL_DIR)/usr/jstbpage/; \
		find $(HUMAX_NFS_INSTALL_DIR)/usr/jstbpage/ -name ".svn" | xargs sudo rm -rf; \
	fi;

nativeresource-clean:

nativeresource-distclean:

.PHONY: nativeresource nativeresource-clean nativeresource-distclean

