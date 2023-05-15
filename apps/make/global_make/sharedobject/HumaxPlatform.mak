
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
HUMAX_LD_LIB_PATH = -L$(HUMAX_OUTPUT_DIR)/lib

# $(sort ) is added not to linked the PNG functions of libant.a, and to linked them of libpng.a
link:
	$(HUMAX_RANLIB) -t $(HUMAX_CURMAKE_DIR)/lib/lib$(HUMAX_SO_NAME).a
	$(HUMAX_RANLIB) -t $(HUMAX_CURMAKE_DIR)/lib$(HUMAX_SO_NAME).a
	$(HUMAX_CC) -Wl,--start-group $(sort $(HUMAX_LIB_LIST)) -pthread $(HUMAX_LD_LIB_PATH) -Wl,--end-group -Wl,-soname,lib$(HUMAX_SO_NAME).so -shared -o $(HUMAX_CURMAKE_DIR)/lib$(HUMAX_SO_NAME).so 

clean_link:
	@$(HUMAX_RM) $(HUMAX_LIB_LIST_FILE)
#	$(HUMAX_RM) $(HUMAX_NFS_INSTALL_DIR)/u
