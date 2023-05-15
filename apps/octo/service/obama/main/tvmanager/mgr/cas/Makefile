ifeq ($(strip $(CONFIG_MW_CAS)),y)
obj-y += mgr_cas.o
else
ifeq ($(strip $(CONFIG_MW_CI)),y)
obj-y += mgr_cas.o
endif
endif

# Basic include path
include $(OCTO_MAIN_INCLUDE_DIR)/Makefile.include.mgr

# Additional include path
ccflags-y += -I$(OCTO_DIR)/main/oxserver
ccflags-y += -I$(OCTO_TVMANAGER_DIR)/include_plugin


export ccflags-y
