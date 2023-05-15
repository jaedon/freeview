#--------------------------------------------------------------------------------------
# 기능단위로 나눠지는 부분들
#--------------------------------------------------------------------------------------

obj-$(CONFIG_PROD_HDR1000S)			+= xsvc_epg_init_hdr1000s.o
obj-$(CONFIG_PROD_HMS1000S)			+= xsvc_epg_init_hms1000s.o
obj-$(CONFIG_PROD_HMSLITE)			+= xsvc_epg_init_hms1000s.o
obj-$(CONFIG_PROD_IR4000HD)			+= xsvc_epg_init_ir4000hd.o
obj-$(CONFIG_PROD_ICORDPRO)			+= xsvc_epg_init_icordpro.o
obj-$(CONFIG_PROD_HDNEO)			+= xsvc_epg_init_hdneo.o
obj-$(CONFIG_PROD_HGS1000S)			+= xsvc_epg_init_hgs1000s.o
obj-$(CONFIG_PROD_HD9000I)			+= xsvc_epg_init_hd9000i.o
obj-$(CONFIG_PROD_HMS1000T)			+= xsvc_epg_init_hms1000t.o
obj-$(CONFIG_PROD_IHDR6100C)		+= xsvc_epg_init_ihdr6100c.o
obj-$(CONFIG_PROD_FVP4000T)			+= xsvc_epg_init_fvp4000t.o
obj-$(CONFIG_PROD_HMX4KBBC)			+= xsvc_epg_init_hmx4kbbc.o
obj-$(CONFIG_PROD_DIGIMAXT2)		+= xsvc_epg_init_digimaxt2.o
obj-$(CONFIG_PROD_HDFOXIRCONNECT)	+= xsvc_epg_init_hdfoxirconnect.o
obj-$(CONFIG_PROD_YSR2000)			+= xsvc_epg_init_ysr2000.o
obj-$(CONFIG_PROD_YS1000)			+= xsvc_epg_init_ys1000.o
obj-$(CONFIG_PROD_CDIG2000C)		+= xsvc_epg_init_cdig2000c.o
obj-$(CONFIG_PROD_ROCKYPOM)		+= xsvc_epg_init_rockypom.o
obj-$(CONFIG_PROD_ROCKYWPOM)		+= xsvc_epg_init_rockypom.o
obj-$(CONFIG_PROD_HA7100S) 		+= xsvc_epg_init_ha7100s.o
obj-$(CONFIG_PROD_M1) 				+= xsvc_epg_init_m1.o

# Basic include path
include $(OCTO_PLUGIN_INCLUDE_DIR)/Makefile.include.xsvc

# Additional include path
ccflags-y += -I$(OCTO_TVSERVICE_DIR)/svc/epg/local_include
ccflags-y += -I$(OCTO_TVSERVICE_PLUGIN_DIR)/xsvc/epg/local_include

