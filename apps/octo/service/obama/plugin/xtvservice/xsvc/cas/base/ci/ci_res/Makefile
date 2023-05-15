obj-y += ci.o
obj-y += ci_adapt.o
obj-y += ci_rm.o
obj-y += ci_ai.o
obj-y += ci_cas.o
obj-y += ci_dt.o
obj-y += ci_mmi.o
obj-y += ci_reshandler.o
obj-y += ci_utils/

ifeq ($(CONFIG_MW_CI_PLUS),y)
obj-y += ci_hci.o
obj-y += ci_ammi.o
obj-y += ci_cc_main.o
obj-y += ci_hlc.o
obj-y += ci_cu.o
obj-y += ci_plus.o
ifeq ($(CONFIG_MW_CI_PLUS_LSC_V_3),y)
obj-y += ci_lsc.o
endif
ifeq ($(CONFIG_MW_CI_PLUS_OP_V_1),y)
obj-y += ci_op.o
endif
obj-y += ci_ammi_lib/
obj-y += ci_cc_lib/
endif

obj-$(CONFIG_MW_CI_PLUS_SAS) += ci_sas.o
ifeq ($(CONFIG_MW_CI_PLUS_V_1_3),n)
obj-$(CONFIG_MW_CI_PLUS_PVR) += ci_capvr.o
endif
obj-$(CONFIG_OP_CANALREADY) += ci_auth.o

# Basic include path
include $(OCTO_PLUGIN_INCLUDE_DIR)/Makefile.include.xsvc
include $(OCTO_PLUGIN_INCLUDE_DIR)/Makefile.include.xcas

# Additional include path
