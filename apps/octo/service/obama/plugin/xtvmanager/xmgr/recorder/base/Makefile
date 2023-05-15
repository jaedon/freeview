obj-$(CONFIG_MW_CI_PLUS) 		+= ci_plus/
obj-$(CONFIG_MW_CAS_IRDETO) 	+= irdeto/
obj-$(CONFIG_MW_CAS_VIACCESS) 	+= viaccess/
obj-$(CONFIG_MW_CAS_NAGRA) 		+= nagra/
obj-$(CONFIG_MW_CAS_CONAX) 		+= conax/
obj-$(CONFIG_MW_CAS_VERIMATRIX)	+= verimatrix/

obj-y += xmgr_recorder_base.o

# Basic include path
include $(OCTO_PLUGIN_INCLUDE_DIR)/Makefile.include.xmgr
include $(OCTO_PLUGIN_INCLUDE_DIR)/Makefile.include.xcas.mgr

# Additional include path
ccflags-y += -I$(OCTO_TVMANAGER_PLUGIN_DIR)/xmgr/recorder/local_include

ccflags-y += -I$(OCTO_TVSERVICE_PLUGIN_DIR)/xsvc/cas/base/ci/control
