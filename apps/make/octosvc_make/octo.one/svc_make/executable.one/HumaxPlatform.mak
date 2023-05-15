
include $(HUMAX_GLOBALENV_DIR)/HumaxToolset.mak
-include $(HUMAX_LIB_LIST_FILE)


#
# update libraty list
#
define update_lib_list
	$(HUMAX_SED) s/'$(1) '// $(HUMAX_LIB_LIST_FILE) | $(HUMAX_SED) s/':='/':= $(1)'/ > $(HUMAX_LIB_LIST_FILE)_tmp
	$(HUMAX_MV) $(HUMAX_LIB_LIST_FILE)_tmp $(HUMAX_LIB_LIST_FILE)
endef

#
# link
#
HUMAX_LD_LIB_PATH = -L$(AP_STATIC_LIB_DIR)

#
# target
#

# $(sort ) is added not to linked the PNG functions of libant.a, and to linked them of libpng.a
link:
	$(HUMAX_CC) main.o $(HUMAX_GCSECTION) -Wl,--start-group $(sort $(HUMAX_LIB_LIST)) $(HUMAX_LD_LIB_PATH) -L$(AP_STATIC_LIB_DIR) $(APP-LINK-y) -Wl,--end-group -o $(HUMAX_CURMAKE_DIR)/$(HUMAX_APP_NAME)
#ifneq ($(CONFIG_DEBUG), y)
#	$(HUMAX_STRIP) --strip-debug --strip-unneeded $(HUMAX_CURMAKE_DIR)/$(HUMAX_APP_NAME)
#endif


clean_link:
	@$(HUMAX_RM) $(HUMAX_LIB_LIST_FILE)
	@$(HUMAX_RM) $(HUMAX_NFS_INSTALL_DIR)/usr/bin/$(HUMAX_APP_NAME)


