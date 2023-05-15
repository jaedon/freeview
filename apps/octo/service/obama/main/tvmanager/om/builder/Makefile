obj-y += om_builder.o
obj-y += om_builder_svcsrch.o
obj-y += om_builder_optplistsrch.o
obj-y += om_builder_tune.o

# satellite
obj-$(CONFIG_MW_CH_SATELLITE) += om_builder_antenna.o
obj-$(CONFIG_MW_CH_SATELLITE) += om_builder_diseqc.o

# ci+ v1.3
obj-$(CONFIG_MW_CI_PLUS_V_1_3) += om_builder_ciplussrch.o

# ertelecom
obj-$(CONFIG_OP_ERTELECOM) += om_builder_ertsrch.o

# Basic include path
include $(OCTO_MAIN_INCLUDE_DIR)/Makefile.include.oapimgr
# Additional include path
ccflags-y += -I$(OCTO_DIR)/oapi/lib
ccflags-y += -I$(OCTO_DIR)/oapi/server
ccflags-y += -I$(OCTO_DIR)/oapi
ccflags-y += -I$(OCTO_DIR)/oapi/protocol
ccflags-y += -I$(OCTO_DIR)/main/oxserver


