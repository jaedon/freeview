IMG_PATH := $(APPLTOP_SRC_DIR)/prism/image
STARTLOGO_IMG_PATH := $(APPLTOP_SRC_DIR)/prism/startlogoimage

ifeq ($(CONFIG_ENHANCED_SECURITY),y)
HUMAX_SUDO := sudo
else
HUMAX_SUDO :=
endif

prism:
ifeq ($(PARALLEL_MAKE_APPS), yes)
	$(HUMAX_MAKE) -f $(HUMAX_GLOBALENV_DIR)/HumaxObjList.mak HUMAX_OBJ='$(obj-y)' HUMAX_CONF_MODE='FULL' objlist
endif
	$(HUMAX_MAKE) -f $(HUMAX_DEFAULT_RULE_MAKEFILE) HUMAX_OBJ='$(obj-y)' library
	$(HUMAX_MAKE_LINK) link
	$(HUMAX_CP) -rdf $(IMG_PATH)/* $(HUMAX_APP_STAG_DIR)/root/usr/image
	$(HUMAX_RM) -f $(HUMAX_APP_STAG_DIR)/root/usr/image/05_LiveTV_13Screensaver_*.gif
	$(HUMAX_RM) -f $(HUMAX_APP_STAG_DIR)/root/usr/image/startlogo_*.gif
ifeq ($(CONFIG_APUI_SCREEN_SAVER),y)
	$(HUMAX_CP) -rdf $(IMG_PATH)/05_LiveTV_13Screensaver_*.gif $(HUMAX_APP_STAG_DIR)/root/usr/image/
endif
# default : copy default startlogo first
	$(HUMAX_CP) -rdf $(IMG_PATH)/startlogo.gif $(HUMAX_APP_STAG_DIR)/root/usr/image/startlogo.gif
ifeq ($(CONFIG_PROD_HMS1000S),y)
	$(HUMAX_CP) -rdf $(IMG_PATH)/startlogo_hms1000s.gif $(HUMAX_APP_STAG_DIR)/root/usr/image/startlogo.gif
endif
ifeq ($(CONFIG_PROD_IR4000HD),y)
	$(HUMAX_CP) -rdf $(IMG_PATH)/startlogo_hms1000s.gif $(HUMAX_APP_STAG_DIR)/root/usr/image/startlogo.gif
endif
ifeq ($(CONFIG_PROD_HDR1000S),y)
	$(HUMAX_CP) -rdf $(IMG_PATH)/startlogo_hms1000s.gif $(HUMAX_APP_STAG_DIR)/root/usr/image/startlogo.gif
endif
ifeq ($(CONFIG_PROD_ICORDPRO),y)
	$(HUMAX_CP) -rdf $(IMG_PATH)/startlogo_hms1000s.gif $(HUMAX_APP_STAG_DIR)/root/usr/image/startlogo.gif
endif
ifeq ($(CONFIG_PROD_HDNEO),y)
	$(HUMAX_CP) -rdf $(IMG_PATH)/startlogo_hms1000s.gif $(HUMAX_APP_STAG_DIR)/root/usr/image/startlogo.gif
endif
ifeq ($(CONFIG_PROD_HD9000I),y)
	$(HUMAX_CP) -rdf $(IMG_PATH)/startlogo_hd9000iru.gif $(HUMAX_APP_STAG_DIR)/root/usr/image/startlogo.gif
endif
ifeq ($(CONFIG_PROD_HMS1000T),y)
	$(HUMAX_CP) -rdf $(IMG_PATH)/startlogo_hms1000s.gif $(HUMAX_APP_STAG_DIR)/root/usr/image/startlogo.gif
endif
ifeq ($(CONFIG_PROD_IHDR6100C),y)
	$(HUMAX_CP) -rdf $(IMG_PATH)/startlogo_ihdr6100c.gif $(HUMAX_APP_STAG_DIR)/root/usr/image/startlogo.gif
endif
ifeq ($(CONFIG_PROD_HMX4KBBC),y)
	$(HUMAX_CP) -rdf $(IMG_PATH)/startlogo.gif $(HUMAX_APP_STAG_DIR)/root/usr/image/startlogo.gif
endif
ifeq ($(CONFIG_PROD_YSR2000),y)
	$(HUMAX_CP) -rdf $(IMG_PATH)/startlogo_yousee.gif $(HUMAX_APP_STAG_DIR)/root/usr/image/startlogo.gif
endif
ifeq ($(CONFIG_PROD_YS1000),y)
	$(HUMAX_CP) -rdf $(IMG_PATH)/startlogo_yousee.gif $(HUMAX_APP_STAG_DIR)/root/usr/image/startlogo.gif
endif
ifeq ($(CONFIG_PROD_HMSLITE),y)
	$(HUMAX_CP) -rdf $(IMG_PATH)/startlogo_hms1000s.gif $(HUMAX_APP_STAG_DIR)/root/usr/image/startlogo.gif
endif
ifeq ($(CONFIG_PROD_CDIG2000C),y)
	$(HUMAX_CP) -rdf $(IMG_PATH)/startlogo_hms1000s.gif $(HUMAX_APP_STAG_DIR)/root/usr/image/startlogo.gif
endif
ifeq ($(CONFIG_PROD_ROCKYPOM),y)
	$(HUMAX_CP) -rdf $(IMG_PATH)/startlogo_hms1000s.gif $(HUMAX_APP_STAG_DIR)/root/usr/image/startlogo.gif
endif

ifeq ($(CONFIG_PRISM_ZAPPING),y)
	$(HUMAX_CP) -rdf $(STARTLOGO_IMG_PATH)/* $(HUMAX_APP_STAG_DIR)/root/usr/image
endif
	$(HUMAX_SUDO) $(HUMAX_CP) -rdf $(HUMAX_APP_STAG_DIR)/root/usr/image/* $(HUMAX_NFS_INSTALL_DIR)/usr/image

	@$(HUMAX_SUDO) mkdir -p $(HUMAX_NFS_INSTALL_DIR)/home/$@
	@$(HUMAX_MKDIR) $(HUMAX_APP_STAG_DIR)/root/home/$@
	@$(HUMAX_SUDO) $(HUMAX_CP) -rdf $@ $(HUMAX_NFS_INSTALL_DIR)/home/$@
	@$(HUMAX_CP) -rdf $@ $(HUMAX_APP_STAG_DIR)/root/home/$@
	$(HUMAX_RM) $@
clean:
	$(HUMAX_MAKE) -f $(HUMAX_DEFAULT_RULE_MAKEFILE) HUMAX_OBJ='$(obj-y)' clean-act

