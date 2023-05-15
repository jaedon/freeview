obj-y += netflixmanager.o

# Additional include path
#ccflags-y += -I./
#ccflags-y += -I../../../make/include

###############################################################################################
## Appkit_v1 : HDR-1010S (base), HDR-1000S and HB-1000S  (URSR12.04) / HDR-1010S VE
###############################################################################################
ifeq ($(CONFIG_PRODUCT_NAME),"hdr1000s")
#HUMAX_CFLAG += -D__NFLIX_IPC_APKV1__=1
ccflags-y += -D__NFLIX_IPC_APKV1__
endif
ifeq ($(CONFIG_PRODUCT_NAME),"hdr1000sph2")
#HUMAX_CFLAG += -D__NFLIX_IPC_APKV1__=1
ccflags-y += -D__NFLIX_IPC_APKV1__
endif
ifeq ($(CONFIG_PRODUCT_NAME),"hdr1010s")
ccflags-y += -D__NFLIX_IPC_APKV1__
endif
ifeq ($(CONFIG_PRODUCT_NAME),"hb1000s")
ccflags-y += -D__NFLIX_IPC_APKV1__
endif
ifeq ($(CONFIG_PRODUCT_NAME),"hb1000sph2")
ccflags-y += -D__NFLIX_IPC_APKV1__
endif
###############################################################################################
## Appkit_v2 : HMS-1000T(base), "iCord Evolution (base)" and "iCord Evolution Lite (HMS Lite)"
###############################################################################################
ifeq ($(CONFIG_PRODUCT_NAME),"hms1000sph2")
ccflags-y += -D__NFLIX_IPC_APKV2__
endif
ifeq ($(CONFIG_PRODUCT_NAME),"hms1000t")
ccflags-y += -D__NFLIX_IPC_APKV2__
ccflags-y += -D__NFLIX_IPC_APKV2_MEDIAPB_FILTERCB__
endif
ifeq ($(CONFIG_PRODUCT_NAME),"hdr3000t")
ccflags-y += -D__NFLIX_IPC_APKV2__
ccflags-y += -D__NFLIX_IPC_APKV2_MEDIAPB_FILTERCB__
endif
ifeq ($(CONFIG_PRODUCT_NAME),"hmslite")
ccflags-y += -D__NFLIX_IPC_APKV2__
endif
ifeq ($(CONFIG_PRODUCT_NAME),"hdr4000t")
ccflags-y += -D__NFLIX_IPC_APKV2__
ccflags-y += -D__NFLIX_IPC_APKV2_MEDIAPB_FILTERCB__
endif
ifeq ($(CONFIG_PRODUCT_NAME),"fvp4000t")
ccflags-y += -D__NFLIX_IPC_APKV2__
ccflags-y += -D__NFLIX_IPC_APKV2_MEDIAPB_FILTERCB__
endif
ifeq ($(CONFIG_PRODUCT_NAME),"ysr2000")
ccflags-y += -D__NFLIX_IPC_APKV2__
ccflags-y += -D__NFLIX_IPC_APKV2_MEDIAPB_FILTERCB__
endif
ifeq ($(CONFIG_PRODUCT_NAME),"ys1000")
ccflags-y += -D__NFLIX_IPC_APKV2__
ccflags-y += -D__NFLIX_IPC_APKV2_MEDIAPB_FILTERCB__
endif
ifeq ($(CONFIG_PRODUCT_NAME),"marqueepoc")
ccflags-y += -D__NFLIX_IPC_APKV2__
ccflags-y += -D__NFLIX_IPC_APKV2_MEDIAPB_FILTERCB__
endif
