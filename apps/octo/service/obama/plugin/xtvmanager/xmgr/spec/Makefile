
obj-y += base/

obj-y += xmgr_spec.o
obj-y += pmgr_spec.o


# Basic include path
include $(OCTO_PLUGIN_INCLUDE_DIR)/Makefile.include.xmgr
include $(OCTO_PLUGIN_INCLUDE_DIR)/Makefile.include.xcas
include $(OCTO_PLUGIN_INCLUDE_DIR)/Makefile.include.xmodule
#include $(OCTO_PLUGIN_INCLUDE_DIR)/Makefile.include.xmgr
#include $(OCTO_PLUGIN_INCLUDE_DIR)/Makefile.include.xsvc
include $(OCTO_MAIN_INCLUDE_DIR)/Makefile.include.3rdparty

# Additional include path
ccflags-$(CONFIG_MW_CAS_IRDETO)		+= -I$(OCTO_TVMANAGER_PLUGIN_DIR)/xmgr/cas/irdeto/include

