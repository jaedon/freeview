$(HUMAX_APP_NAME):
ifeq ($(PARALLEL_MAKE_APPS), yes)
	$(HUMAX_MAKE) -f $(HUMAX_GLOBALENV_DIR)/HumaxObjList.mak HUMAX_OBJ='$(obj-y)' HUMAX_CONF_MODE='FULL' objlist
endif
	$(HUMAX_MAKE) -f $(HUMAX_DEFAULT_RULE_MAKEFILE) HUMAX_OBJ='$(obj-y)' library
	$(HUMAX_MAKE_LINK) link
	$(HUMAX_NM) $@ | grep -v '\(compiled\)\|\(\.o$$\)\|\( [aUw] \)\|\(\.\.ng$$\)\|\(    LASH[RL]DI\)' | sort > $@.map
	@$(HUMAX_SUDO) mkdir -p $(HUMAX_NFS_INSTALL_DIR)/$(HUMAX_APP_FOLDER)
	@$(HUMAX_MKDIR) $(HUMAX_APP_STAG_DIR)/root/$(HUMAX_APP_FOLDER)
	@$(HUMAX_SUDO) cp $@ $(HUMAX_NFS_INSTALL_DIR)/$(HUMAX_APP_FOLDER)
	@cp $@ $(HUMAX_APP_STAG_DIR)/root/$(HUMAX_APP_FOLDER)
	@$(HUMAX_RM) $@
	
	@sudo $(HUMAX_CP) $(OCTO_UMMA_FS_PROPILE_DIR)/$(CONFIG_PRODUCT_NAME)_storage_profile $(HUMAX_NFS_INSTALL_DIR)/usr/etc/storage_profile.json
	@sudo $(HUMAX_CP) $(OCTO_UMMA_FS_PROPILE_DIR)/$(CONFIG_PRODUCT_NAME)_storage_profile $(HUMAX_APP_STAG_ROOT_DIR)/usr/etc/storage_profile.json
	
clean:
	$(HUMAX_MAKE) -f $(HUMAX_DEFAULT_RULE_MAKEFILE) HUMAX_OBJ='$(obj-y)' clean-act
