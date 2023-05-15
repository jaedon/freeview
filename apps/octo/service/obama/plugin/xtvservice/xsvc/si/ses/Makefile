#--------------------------------------------------------------------------------------
# 기능단위로 나눠지는 부분들
#--------------------------------------------------------------------------------------

obj-y += xsvc_si_live_ses.o
obj-y += xsvc_si_search_ses.o
obj-y += xsvc_si_lcn_ses.o
obj-y += xsvc_si_lcnupdate_ses.o
obj-y += xsvc_si_service_ses.o
obj-y += xsvc_si_satrecord_ses.o
obj-y += xsvc_si_mediapb_ses.o

# Basic include path
include $(OCTO_PLUGIN_INCLUDE_DIR)/Makefile.include.xsvc
include $(OCTO_PLUGIN_INCLUDE_DIR)/Makefile.include.xmgr

# Additional include path
ccflags-y += -I$(OCTO_TVSERVICE_DIR)/svc/si/local_include
ccflags-y += -I$(OCTO_TVSERVICE_PLUGIN_DIR)/xsvc/si/local_include
