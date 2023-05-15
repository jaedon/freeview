#######################################
# include
#######################################
# Basic include path
include $(OCTO_PLUGIN_INCLUDE_DIR)/Makefile.include.xsvc
include $(OCTO_PLUGIN_INCLUDE_DIR)/Makefile.include.xcas

# Additional include path
ccflags-$(CONFIG_MW_CI) += -I$(OCTO_TVSERVICE_PLUGIN_DIR)/xsvc/cas/base/ci

#######################################
# make
#######################################
	obj-y += ci_mgr2.o
	obj-y += xsvc_cas_ci.o

	obj-y += ci_pack/
	obj-y += ci_res/
	obj-y += control/



