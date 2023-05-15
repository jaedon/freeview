obj-y += xsvc_epg_context_raw_base.o
obj-y += xsvc_epg_context_raw_eit_pf.o
obj-y += xsvc_epg_context_raw_eit_sch.o
obj-y += xsvc_epg_db_raw_base.o
obj-y += xsvc_epg_eit_base.o
obj-y += xsvc_epg_process_base_main.o

obj-$(CONFIG_MW_EPG_TVTV) += tvtv/

# Basic include path
include $(OCTO_PLUGIN_INCLUDE_DIR)/Makefile.include.xsvc
include $(OCTO_PLUGIN_INCLUDE_DIR)/Makefile.include.xmodule

# Additional include path
ccflags-y += -I$(OCTO_TVSERVICE_PLUGIN_DIR)/xsvc/epg/local_include

