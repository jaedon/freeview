# MOD_DB1 : ddi i/f를 사용하는 cmd module은 compile 과정에서 제외

obj-y += cmd_octo.o
obj-y += cmd_vk.o


###################################
# OCTO flags & include dirs
###################################

# Basic include path
include $(OCTO_MAIN_INCLUDE_DIR)/Makefile.include.mgr
include $(OCTO_MAIN_INCLUDE_DIR)/Makefile.include.mw
include $(OCTO_MAIN_INCLUDE_DIR)/Makefile.include.pal

# Additional include path
ccflags-$(CONFIG_MW_CI) 			+= -I$(OCTO_TVSERVICE_PLUGIN_DIR)/xsvc/cas/base/ci/include
ccflags-$(CONFIG_MW_CAS_NAGRA) 		+= -I$(OCTO_TVSERVICE_PLUGIN_DIR)/xsvc/cas/base/nagra/cak
ccflags-y							+= -I$(OCTO_TVSERVICE_PLUGIN_DIR)/xsvc/cas/base/nagra/include/$(CONFIG_PRODUCT_NAME)

