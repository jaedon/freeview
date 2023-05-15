HUMAX_GLOBALCONF_DIR=$(HUMAX_GLOBALENV_CONFDIR)
HUMAX_GLOBALENV_DIR = $(HUMAX_GLOBALENV_BINDIR)

HUMAX_CONFIG_MODE := .config

export HUMAX_APP_NAME
#export HUMAX_SO_NAME
export HUMAX_GLOBALENV_DIR
export HUMAX_CONFIG_MODE

include $(HUMAX_MAKE_DIR)/$(HUMAX_CONFIG_MODE)


ifeq ($(CONFIG_DEVICE_DEBUG), y)
ifeq ($(CONFIG_PRODUCT_NAME),"hdr1000sph2")
DIRECTFB_DIR=${HUMAX_TOP_DIR}/device/platform/sdk/driver/AppLibs/opensource/directfb/bin/DirectFB-1.4.17_multi_debug_build.97346B0
endif
ifeq ($(CONFIG_PRODUCT_NAME),"hb1000sph2")
DIRECTFB_DIR=${HUMAX_TOP_DIR}/device/platform/sdk/driver/AppLibs/opensource/directfb/bin/DirectFB-1.4.17_multi_debug_build.97344B1
endif
ifeq ($(CONFIG_PRODUCT_NAME),"hdr3000t")
DIRECTFB_DIR=${HUMAX_TOP_DIR}/device/platform/sdk/driver/AppLibs/opensource/directfb/bin/DirectFB-1.4.17_multi_debug_build.97241B0
endif
ifeq ($(CONFIG_PRODUCT_NAME),"hms1000t")
DIRECTFB_DIR=${HUMAX_TOP_DIR}/device/platform/sdk/driver/AppLibs/opensource/directfb/bin/DirectFB-1.4.17_multi_debug_build.97241B0
endif
ifeq ($(CONFIG_PRODUCT_NAME),"fvp4000t")
DIRECTFB_DIR=${HUMAX_TOP_DIR}/device/platform/sdk/driver/AppLibs/opensource/directfb/bin/DirectFB-1.4.17_multi_debug_build.97241B0
endif
else
ifeq ($(CONFIG_PRODUCT_NAME),"hdr1000sph2")
DIRECTFB_DIR=${HUMAX_TOP_DIR}/device/platform/sdk/driver/AppLibs/opensource/directfb/bin/DirectFB-1.4.17_multi_release_build.97346B0
endif
ifeq ($(CONFIG_PRODUCT_NAME),"hb1000sph2")
DIRECTFB_DIR=${HUMAX_TOP_DIR}/device/platform/sdk/driver/AppLibs/opensource/directfb/bin/DirectFB-1.4.17_multi_release_build.97344B1
endif
ifeq ($(CONFIG_PRODUCT_NAME),"hdr3000t")
DIRECTFB_DIR=${HUMAX_TOP_DIR}/device/platform/sdk/driver/AppLibs/opensource/directfb/bin/DirectFB-1.4.17_multi_release_build.97241B0
endif
ifeq ($(CONFIG_PRODUCT_NAME),"hms1000t")
DIRECTFB_DIR=${HUMAX_TOP_DIR}/device/platform/sdk/driver/AppLibs/opensource/directfb/bin/DirectFB-1.4.17_multi_release_build.97241B0
endif
ifeq ($(CONFIG_PRODUCT_NAME),"fvp4000t")
DIRECTFB_DIR=${HUMAX_TOP_DIR}/device/platform/sdk/driver/AppLibs/opensource/directfb/bin/DirectFB-1.4.17_multi_release_build.97241B0
endif
endif

ifeq ($(CONFIG_DEBUG),y)
	NRD_BUILD_SCRIPT_NAME=build_dev.sh
else
	NRD_BUILD_SCRIPT_NAME=build_rel.sh
endif



#ifeq ($(CONFIG_NETFLIX),y)
include $(HUMAX_GLOBALENV_DIR)/HumaxDefaultMake.mak
#else
#	netflix : nxlink nrd
#
#clean:
#endif

HUMAX_LIB_LIST_FILE := $(CURDIR)/lib/lib.lst

NETFLIX_APP_DIR := $(APPLTOP_SRC_DIR)/netflix
NRD_NETFLIX_DIR := $(NETFLIX_APP_DIR)/nrd/netflix
NXLINK_INC_DIR := $(NETFLIX_APP_DIR)/nxlink/include

USER=$(shell echo $(shell whoami) | sed "s/[ ]//g")
HUMAX_CURMAKE_DIR := $(CURDIR)
HUMAX_BUILD_DIR := $(CURDIR)/
HUMAX_OUTPUT_DIR := $(CURDIR)
HUMAX_NFS_INSTALL_DIR := $(strip $(subst ",,/nfsroot/$(USER)/$(CONFIG_PRODUCT_NAME)/root"))


export HUMAX_TOP_DIR
export PACKAGES_DIR
export HUMAX_EXTRA_CFLAG
export HUMAX_OUTPUT_DIR
export HUMAX_CURMAKE_DIR
export HUMAX_LIB_LIST_FILE
export HUMAX_BUILD_DIR
export HUMAX_NFS_INSTALL_DIR
export CONFIG_DEBUG
export AP_STATIC_LIB_DIR
export DIRECTFB_DIR

export NETFLIX_APP_DIR
export NRD_NETFLIX_DIR
export NXLINK_INC_DIR
export CONFIG_TOOLCHAIN_PATH
export HUMAX_APP_STAG_DIR
export HUMAX_NFS_INSTALL_DIR
export HUMAX_CROSS
export HUMAX_MAKE
export HUMAX_CC
export HUMAX_CXX
export HUMAX_AR
export HUMAX_LD
export HUMAX_STRIP
export HUMAX_SED
export HUMAX_CP
export HUMAX_LN
export HUMAX_NM
export HUMAX_RM


netflix: nrd
clean : nrd-clean
distclean : clean
nrd:
	@echo "NRD Compile..."
	cd nrd.build; \
	sh $(NRD_BUILD_SCRIPT_NAME)
	$(HUMAX_MKDIR) $(HUMAX_NFS_INSTALL_DIR)/usr/bin/netflix

	@echo "Remove CJK Font files"
	rm -f nrd.build/install-files/bin/data/fonts/Heisei*
	rm -f nrd.build/install-files/bin/data/fonts/YD*
	rm -f nrd.build/install-files/bin/data/fonts/MH*
	rm -f nrd.build/install-files/bin/data/fonts/HandsetSansBengali-*
	rm -f nrd.build/install-files/bin/data/fonts/HandsetSansDevanagari-*
	rm -f nrd.build/install-files/bin/data/fonts/HandsetSansGujarati-*
	rm -f nrd.build/install-files/bin/data/fonts/HandsetSansGurmukhi-*
	rm -f nrd.build/install-files/bin/data/fonts/HandsetSansKannada-*
	rm -f nrd.build/install-files/bin/data/fonts/HandsetSansMalayalam-*
	rm -f nrd.build/install-files/bin/data/fonts/HandsetSansTamil-*
	rm -f nrd.build/install-files/bin/data/fonts/HandsetSansTelugu-*
	rm -f nrd.build/install-files/bin/data/fonts/HandsetSansOriya-*

	@echo "INSTALL binary file to NFS ROOT"
	@chmod +x nrd.build/install-files/bin/netflix nrd.build/install-files/bin/libWTF.so nrd.build/install-files/bin/libJavaScriptCore.so
	@$(HUMAX_CP) -rf nrd.build/install-files/bin/netflix $(HUMAX_NFS_INSTALL_DIR)/usr/bin/netflix/
	@$(HUMAX_CP) -rf nrd.build/install-files/bin/libWTF.so $(HUMAX_NFS_INSTALL_DIR)/usr/lib/
	@$(HUMAX_CP) -rf nrd.build/install-files/bin/libJavaScriptCore.so $(HUMAX_NFS_INSTALL_DIR)/usr/lib/
	@chmod +x $(HUMAX_NFS_INSTALL_DIR)/usr/bin/netflix/netflix

	@echo "INSTALL Tools and Temp ESN to NFS ROOT, will be removed"
	@chmod +x nrd.build/src/platform/gibbon/humax_manufss
	@$(HUMAX_CP) -rf nrd.build/src/platform/gibbon/humax_manufss $(HUMAX_NFS_INSTALL_DIR)/usr/bin/netflix/
	@chmod +x $(HUMAX_NFS_INSTALL_DIR)/usr/bin/netflix/humax_manufss
	
############ MUST USE USB TO MAKE/CHANGE CREDENTIALS
##@$(HUMAX_CP) -f *.idfile $(HUMAX_NFS_INSTALL_DIR)/usr/bin/netflix/

#	@if [ -L $(HUMAX_NFS_INSTALL_DIR)/usr/bin/netflix/data ] ; then \
#		$(HUMAX_RM) $(HUMAX_NFS_INSTALL_DIR)/usr/bin/netflix/data; \
#	fi

	@sudo $(HUMAX_RM) -rf $(HUMAX_NFS_INSTALL_DIR)/usr/bin/netflix/data
	@echo "INSTALL data folder to NFS ROOT, Some files should be replaced to persistent memroy(/var/lib/humaxtv_user)"
	-@$(HUMAX_MKDIR) $(HUMAX_NFS_INSTALL_DIR)/usr/bin/netflix/data
	@$(HUMAX_CP) -rfu nrd.build/install-files/bin/data/* $(HUMAX_NFS_INSTALL_DIR)/usr/bin/netflix/data
	-@$(HUMAX_LN)  -sf /mnt/hd3 $(HUMAX_NFS_INSTALL_DIR)/usr/bin/netflix/data/cache
	-@$(HUMAX_LN)  -sf /var/lib/humaxtv_user/netflix/var $(HUMAX_NFS_INSTALL_DIR)/usr/bin/netflix/data/var

nrd-clean: 
	@echo "NRD Clean..."
	@cd nrd.build; \
	make clean; \
	find . -mindepth 1 -maxdepth 1 \( ! -path "*/\.*" -and ! -name "*.sh" \) -exec rm -rf {} \;

nrd-distclean: nrd-clean
	@echo "NRD Distclean..."
#nxlink:
#	@echo "nxlink Install..."
#	@cd nxlink.build ; \
#	ln -sf libnxlink.so.0.0.1 libnxlink.so.0 ; \
#	ln -sf libnxlink.so.0 libnxlink.so ; 
#	@sudo $(HUMAX_CP) -rdf $(HUMAX_CURMAKE_DIR)/nxlink.build/libnxlink.so* $(HUMAX_NFS_INSTALL_DIR)/usr/lib/ 
#	@sudo $(HUMAX_CP) -rdf $(HUMAX_CURMAKE_DIR)/nxlink.build/libnxlink.so* $(HUMAX_APP_STAG_DIR)/root/usr/lib/

## nxlink compiling is replaced with appl_make/HumaxMakeObject.mak's (build_nxlink)
## $(HUMAX_MAKE) -f $(HUMAX_CURMAKE_DIR)/scripts/HumaxDefaultRule.mak HUMAX_OBJ='$(obj-y)' clean-act
#nxlink-clean:
#	@echo "nxlink Clean..."
#	@$(HUMAX_RM) -f $(HUMAX_CURMAKE_DIR)/nxlink.build/libnxlink.so*
#	@sudo $(HUMAX_RM) -f $(HUMAX_NFS_INSTALL_DIR)/usr/lib/libnxlink.so*
#	@sudo $(HUMAX_RM) -f $(HUMAX_APP_STAG_DIR)/root/usr/lib/libnxlink.so*
#
#nxlink-distclean: nxlink-clean
#	@echo "NxLink Distclean..."
#
#nxmanager:
#nxmanager-clean:
#nxmanager-distclean:
