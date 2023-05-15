
obj-y += xsvc_si_lib_live_base.o
obj-y += xsvc_si_lib_search_base.o
obj-y += xsvc_si_lib_service_base.o
obj-y += xsvc_si_lib_time_base.o
obj-y += xsvc_si_lib_lcn_base.o
obj-y += xsvc_si_lib_otacheck_base.o
obj-y += xsvc_si_lib_makesec_base.o
obj-$(CONFIG_MW_SI_AIT) += xsvc_si_lib_hbbtv_base.o

# Basic include path
include $(OCTO_PLUGIN_INCLUDE_DIR)/Makefile.include.xsvc

# Additional include path
ccflags-y += -I$(OCTO_TVSERVICE_DIR)/svc/si/local_include
ccflags-y += -I$(OCTO_TVSERVICE_PLUGIN_DIR)/xsvc/si/local_include
