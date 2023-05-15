#--------------------------------------------------------------------------------------
# Seperation by funcion
#--------------------------------------------------------------------------------------

obj-y += xsvc_si_live_amx_cenam.o
obj-y += xsvc_si_search_amx_cenam.o
obj-y += xsvc_si_lcn_amx_cenam.o

# Basic include path
include $(OCTO_PLUGIN_INCLUDE_DIR)/Makefile.include.xsvc

# Additional include path
ccflags-y += -I$(OCTO_TVSERVICE_DIR)/svc/si/local_include
ccflags-y += -I$(OCTO_TVSERVICE_PLUGIN_DIR)/xsvc/si/local_include
