#--------------------------------------------------------------------------------------
# 기능단위로 나눠지는 부분들
#--------------------------------------------------------------------------------------

obj-y += xsvc_si_live_eucab.o
obj-y += xsvc_si_search_eucab.o
obj-y += xsvc_si_lcn_eucab.o

# Basic include path
include $(OCTO_PLUGIN_INCLUDE_DIR)/Makefile.include.xsvc

# Additional include path
ccflags-y += -I$(OCTO_TVSERVICE_DIR)/svc/si/local_include
ccflags-y += -I$(OCTO_TVSERVICE_PLUGIN_DIR)/xsvc/si/local_include
