obj-y += nxlink.o

###############################################################################################
## Group #1 : HDR-1010S (base), HDR-1000S and HB-1000S  (URSR12.04) / HDR-1010S VE
## Group #2 : HDR-4000T, iCord Pro, HDR-3000T (base) and HD NANO NEO (above URSR12.04)
## Group #3 : HMS-1000T(base), "iCord Evolution (base)" and "iCord Evolution Lite (HMS Lite)"
###############################################################################################
## Appkit_v1 : HDR-1010S (base), HDR-1000S and HB-1000S  (URSR12.04) / HDR-1010S VE
###############################################################################################
ifeq ($(CONFIG_PRODUCT_NAME),"hdr1000s")
#HUMAX_CFLAG += -D__NXLINK_IPC_APKV1__=1
ccflags-y += -D__NXLINK_IPC_APKV1__
obj-y += nxlink_ipc_apkv1.o
endif
ifeq ($(CONFIG_PRODUCT_NAME),"hdr1000sph2")
#HUMAX_CFLAG += -D__NXLINK_IPC_APKV1__=1
ccflags-y += -D__NXLINK_IPC_APKV1__
obj-y += nxlink_ipc_apkv1.o
endif
ifeq ($(CONFIG_PRODUCT_NAME),"hdr1010s")
ccflags-y += -D__NXLINK_IPC_APKV1__
obj-y += nxlink_ipc_apkv1.o
endif
ifeq ($(CONFIG_PRODUCT_NAME),"hb1000s")
ccflags-y += -D__NXLINK_IPC_APKV1__
obj-y += nxlink_ipc_apkv1.o
endif
ifeq ($(CONFIG_PRODUCT_NAME),"hb1000sph2")
ccflags-y += -D__NXLINK_IPC_APKV1__
obj-y += nxlink_ipc_apkv1.o
endif
###############################################################################################
## Appkit_v2 : HMS-1000T(base), "iCord Evolution (base)" and "iCord Evolution Lite (HMS Lite)"
###############################################################################################
ifeq ($(CONFIG_PRODUCT_NAME),"hms1000sph2")
ccflags-y += -D__NXLINK_IPC_APKV2__
obj-y += nxlink_ipc_apkv2.o
endif
ifeq ($(CONFIG_PRODUCT_NAME),"hms1000t")
ccflags-y += -D__NXLINK_IPC_APKV2__
ccflags-y += -D__NXLINK_IPC_APKV2_MEDIAPB_FILTERCB__
obj-y += nxlink_ipc_apkv2.o
endif
ifeq ($(CONFIG_PRODUCT_NAME),"hdr3000t")
ccflags-y += -D__NXLINK_IPC_APKV2__
ccflags-y += -D__NXLINK_IPC_APKV2_MEDIAPB_FILTERCB__
obj-y += nxlink_ipc_apkv2.o
endif
ifeq ($(CONFIG_PRODUCT_NAME),"hmslite")
ccflags-y += -D__NXLINK_IPC_APKV2__
obj-y += nxlink_ipc_apkv2.o
endif
ifeq ($(CONFIG_PRODUCT_NAME),"hdr4000t")
ccflags-y += -D__NXLINK_IPC_APKV2__
ccflags-y += -D__NXLINK_IPC_APKV2_MEDIAPB_FILTERCB__
obj-y += nxlink_ipc_apkv2.o
endif
ifeq ($(CONFIG_PRODUCT_NAME),"fvp4000t")
ccflags-y += -D__NXLINK_IPC_APKV2__
ccflags-y += -D__NXLINK_IPC_APKV2_MEDIAPB_FILTERCB__
obj-y += nxlink_ipc_apkv2.o
endif
ifeq ($(CONFIG_PRODUCT_NAME),"ys1000")
ccflags-y += -D__NXLINK_IPC_APKV2__
ccflags-y += -D__NXLINK_IPC_APKV2_MEDIAPB_FILTERCB__
obj-y += nxlink_ipc_apkv2.o
endif
ifeq ($(CONFIG_PRODUCT_NAME),"ysr2000")
ccflags-y += -D__NXLINK_IPC_APKV2__
ccflags-y += -D__NXLINK_IPC_APKV2_MEDIAPB_FILTERCB__
obj-y += nxlink_ipc_apkv2.o
endif
ifeq ($(CONFIG_PRODUCT_NAME),"marqueepoc")
ccflags-y += -D__NXLINK_IPC_APKV2__
ccflags-y += -D__NXLINK_IPC_APKV2_MEDIAPB_FILTERCB__
obj-y += nxlink_ipc_apkv2.o
endif
###############################################################################################
## ProcessAPI #1 : iCord Pro, HD NANO NEO (above URSR12.04)
###############################################################################################
ifeq ($(CONFIG_PRODUCT_NAME),"icordpro")
ccflags-y += -D__NXLINK_IPC_PROCAPI__
obj-y += nxlink_ipc_procapi.o
endif
ifeq ($(CONFIG_PRODUCT_NAME),"hdnanoneo")
ccflags-y += -D__NXLINK_IPC_PROCAPI__
obj-y += nxlink_ipc_procapi.o
endif


# Additional include path
ccflags-y += -I./include

# Getting CONFIG_PRODUCT_NAME
ifeq ($(CONFIG_PRODUCT_NAME),"hdr1000s")
ccflags-y += -I../../make/include
endif
ifeq ($(CONFIG_PRODUCT_NAME),"hdr1000sph2")
ccflags-y += -I../../make/include
endif
ifeq ($(CONFIG_PRODUCT_NAME),"hb1000s")
ccflags-y += -I../../make/include
endif
ifeq ($(CONFIG_PRODUCT_NAME),"hb1000sph2")
ccflags-y += -I../../make/include
endif
ifeq ($(CONFIG_PRODUCT_NAME),"hms1000t")
ccflags-y += -I../../../make/include
endif
ifeq ($(CONFIG_PRODUCT_NAME),"hdr3000t")
ccflags-y += -I../../../make/include
endif
ifeq ($(CONFIG_PRODUCT_NAME),"hdr4000t")
ccflags-y += -I../../../make/include
endif
ifeq ($(CONFIG_PRODUCT_NAME),"fvp4000t")
ccflags-y += -I../../../make/include
endif
ifeq ($(CONFIG_PRODUCT_NAME),"ys1000")
ccflags-y += -I../../../make/include
endif
ifeq ($(CONFIG_PRODUCT_NAME),"ysr2000")
ccflags-y += -I../../../make/include
endif
ifeq ($(CONFIG_PRODUCT_NAME),"marqueepoc")
ccflags-y += -I../../../make/include
endif
