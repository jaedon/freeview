#--------------------------------------------------------------------------------------
# 기능단위로 나눠지는 부분들
#--------------------------------------------------------------------------------------
obj-y += library/
obj-y += svc_si_api.o
obj-y += svc_si_main.o
obj-y += svc_si_filtering.o
obj-y += svc_si_cb.o

obj-y += svc_si_live.o
obj-y += svc_si_search.o
obj-y += svc_si_extsearch.o
obj-y += svc_si_satrecord.o
obj-y += svc_si_swup.o
obj-y += svc_si_time.o
obj-y += svc_si_dynamicupdate.o
obj-y += svc_si_lcn.o
obj-y += svc_si_mediapb.o

obj-$(CONFIG_MW_SI_AIT) += svc_si_hbbtv.o
obj-$(CONFIG_MW_SI_HOME_CHANNEL) += svc_si_homechannel.o
obj-$(CONFIG_3RD_MHEG_SI_MONITORING) += svc_si_mheg.o

obj-$(CONFIG_DEBUG) += svc_si_cmd.o
obj-$(CONFIG_DEBUG) += svc_si_cmd_util.o
obj-$(CONFIG_DEBUG) += svc_si_cmd_es_audio.o
obj-$(CONFIG_DEBUG) += svc_si_cmd_es_subtitle.o

# Basic include path
include $(OCTO_MAIN_INCLUDE_DIR)/Makefile.include.mw
# Additional include path
ccflags-y += -I$(OCTO_TVSERVICE_DIR)/svc/si/local_include
