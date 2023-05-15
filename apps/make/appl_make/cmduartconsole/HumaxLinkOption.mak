ifeq ($(CONFIG_ENHANCED_SECURITY),y)
HUMAX_SUDO := sudo
else
HUMAX_SUDO :=
endif

cmduartconsole:
ifeq ($(PARALLEL_MAKE_APPS), yes)
	$(HUMAX_MAKE) -f $(HUMAX_GLOBALENV_DIR)/HumaxObjList.mak HUMAX_OBJ='$(obj-y)' HUMAX_CONF_MODE='FULL' objlist
endif
	$(HUMAX_MAKE) -f $(HUMAX_DEFAULT_RULE_MAKEFILE) HUMAX_OBJ='$(obj-y)' library
	$(HUMAX_MAKE_LINK) link
	
	@$(HUMAX_SUDO) mkdir -p $(HUMAX_NFS_INSTALL_DIR)/home/$@
	@$(HUMAX_MKDIR) $(HUMAX_APP_STAG_DIR)/root/home/$@
	@$(HUMAX_SUDO) $(HUMAX_CP) -rdf $@ $(HUMAX_NFS_INSTALL_DIR)/home/$@
	@$(HUMAX_CP) -rdf $@ $(HUMAX_APP_STAG_DIR)/root/home/$@
	$(HUMAX_RM) $@
	@$(HUMAX_SUDO) $(HUMAX_RM) $(HUMAX_NFS_INSTALL_DIR)/usr/bin/$@
	@$(HUMAX_SUDO) ln -s ../../home/$@/$@ $(HUMAX_NFS_INSTALL_DIR)/usr/bin/$@
clean:
	$(HUMAX_MAKE) -f $(HUMAX_DEFAULT_RULE_MAKEFILE) HUMAX_OBJ='$(obj-y)' clean-act

