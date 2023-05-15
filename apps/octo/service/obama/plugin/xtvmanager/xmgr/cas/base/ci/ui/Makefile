obj-$(CONFIG_MW_CI) += xmgr_cas_ci_ui.o
obj-$(CONFIG_MW_CI) += xmgr_cas_ci_mmi.o
ifeq ($(CONFIG_MW_CI_PLUS),y)
obj-y += xmgr_cas_ci_ciplus.o
endif

# Basic include path
include $(OCTO_PLUGIN_INCLUDE_DIR)/Makefile.include.xmgr
include $(OCTO_PLUGIN_INCLUDE_DIR)/Makefile.include.xcas.mgr
include $(OCTO_PLUGIN_INCLUDE_DIR)/Makefile.include.xcas

# Additional include path

