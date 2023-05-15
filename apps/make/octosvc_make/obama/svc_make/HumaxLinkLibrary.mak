#####################################################
# Private application link option
#####################################################

#CROSS_COMPILER relatives
APP-LINK-$(CONFIG_PROD_GCC_GLIBC) += -lm
APP-LINK-$(CONFIG_SUPPORT_OBAMA_ASAN) += -lasan

# Driver relatives
ifeq ($(CONFIG_NO_DRIVER), y)
APP-LINK-y += -lrt
else
APP-LINK-y += -ldriver
ifeq ($(CONFIG_SUPPORT_NIMA), y)
APP-LINK-y += -ldi_network
APP-LINK-$(CONFIG_MW_INET_WLAN) += -ldi_wifi
APP-LINK-$(CONFIG_MW_INET_WLAN) += -ldrv_hotplug
APP-LINK-$(CONFIG_MW_INET_PPPOE) += -ldi_pppoe
APP-LINK-$(CONFIG_PROD_DEVICE_BLUETOOTH) += -lbsa -ldi_bt -lrt
endif

APP-LINK-y += -ldrv_hotplug
APP-LINK-$(CONFIG_PROD_DEVICE_STORAGE_USB) += -ldi_usb
APP-LINK-$(CONFIG_PROD_DEVICE_STORAGE_MMC) += -ldi_mmc
APP-LINK-$(CONFIG_PROD_DEVICE_FILE_SYSTEM) += -ldi_fs
APP-LINK-$(CONFIG_PROD_DEVICE_STORAGE_HDD) += -ldi_hdd

ifeq ($(CONFIG_HOMMA_CDI), y)
APP-LINK-y += -ldi_crypt
endif

ifeq ($(CONFIG_MW_OUTPUT_HDMI_HDCP22_PROVISIONING)$(CONFIG_DEBUG),yy)
APP-LINK-y += -lsrai
endif
#Tempory apply by moonsg
ifeq ($(CONFIG_SUPPORT_NATIVE_APP), y) 
ifeq ($(CONFIG_PRODUCT_NAME), "dietocto")
APP-LINK-y += -ldi_crypt
endif
endif

ifeq ($(CONFIG_PROD_DEVICE_KERNEL_MODE), y)
APP-LINK-y			+= -lnexus
else
APP-LINK-y			+= -lnexus_client
endif

ifeq ($(CONFIG_PROD_DEVICE_KERNEL_MODE), y)
APP-LINK-y					+= -linit
else
APP-LINK-y					+= -linit_client
endif

endif

ifeq ($(CONFIG_SUPPORT_IMMA), y)
APP-LINK-y += -lmediapb_ipc
APP-LINK-y += -lmi
APP-LINK-y += -lprdy
APP-LINK-y += -lcrypto
APP-LINK-y += -lssl
endif

APP-LINK-y			+= -lmemory-null
#APP-LINK-y			+= -lmemory-debug

APP-LINK-y								+= -pthread -lssp

APP-LINK-y								+= -lhlib -lhapi -ldlib -lpapi -lsilib -lsitextlib -ldapi -lrlib -lsamalib -lsapi -luapi -lthapi -ltapi

APP-LINK-$(CONFIG_SUPPORT_NIMA)			+= -lnapi

APP-LINK-$(CONFIG_TRAUMA_PROCESS)		+= -ltrapi

APP-LINK-$(CONFIG_FUNC_USE_DAPI_DIRECT_QUERY)			+= -lsqlite3

ifeq ($(CONFIG_HUMAX_CRASHREPORT),y)
APP-LINK-y			+= -lhcrlib
endif
ifeq ($(CONFIG_DEBUG),y)
APP-LINK-y			+= -lcutest
endif

APP-LINK-$(CONFIG_PACKAGE_FFMPEG) += -lavformat -lavcodec -lswscale -lavutil
APP-LINK-$(CONFIG_PACKAGE_LIBXML2) += -lxml2
APP-LINK-$(CONFIG_PACKAGE_LIBPNG) += -lpng
APP-LINK-$(CONFIG_PACKAGE_DIRECTFB)		+= -ldirectfb -lfusion -ldirect -ldl -lsawman -lstdc++ -lz

# library for HLIB
APP-LINK-y					+= -lstdc++
APP-LINK-$(CONFIG_HPACKAGE_VKERNEL)		+= -lvkernel
APP-LINK-$(CONFIG_PACKAGE_CAP)		+= -lcap -lattr
#############################################################################

APP-LINK-$(CONFIG_PACKAGE_LIBEXT2FS)	+= -lblkid -lcom_err -le2p -lss -luuid -lext2fs

APP-LINK-$(CONFIG_3RD_RFVOD) 			+= -lnfhtmlnetfront -lnfpeersup -lnfrtsp_hfcp -lnfstandard -lnftarget
APP-LINK-$(CONFIG_3RD_BML) 				+= -lbml

######################################################################################
#
#        HUMAX default Driver Library : BCM's nexus driver and humax di driver.
#
######################################################################################
APP-LINK-$(CONFIG_PROD_DEVICE_CABLE_MODEM) += -ldsgcc
APP-LINK-$(CONFIG_PROD_RCU_RF4CE_UEI_HILCREST) += -luapi -lMotionEngine-bundle -lMotionEngine-core -lMotionEngine-ext -lfreespace-hid
APP-LINK-$(CONFIG_PROD_DEVICE_BLUETOOTH) += -lbsa
APP-LINK-$(CONFIG_PROD_DEVICE_SERVER_CLIENT) += -ldi_power_client

######################################################################################
#
#        Package Include Path
#
######################################################################################

######################################################################################
#
#        3rd party DLNA Libraries
#
######################################################################################

#----------------------------------------------
#		Broadcom DLNA Libraries..
#----------------------------------------------
#
APP-LINK-$(CONFIG_PROD_DEVICE_DLNA) += -lcurl -ljpeg -lpng -lxml2 -lz -lbdlna -lbdlna-dmp -lbdlna-nexus
APP-LINK-$(CONFIG_PROD_DEVICE_DLNA) += -lb_playback_ip -lb_os -lm -lbdlna-dms -lbdlna-dms-aal -lb_psip -lssl -lb_dtcp_ip -lcrypto -ldl -lexpat

######################################################################################
#
#        3rd party broadcast middle ware Libraries
#
######################################################################################

#----------------------------------------------
#		S&T MHEG Engine
#----------------------------------------------
APP-LINK-$(CONFIG_3RD_MHEG_USE_SNT_ENGINE)		+= -lredkey $(AP_STATIC_LIB_DIR)/license.lo

######################################################################################
#
#        Irdeto CAS LDVT Libraries
#
######################################################################################
#it shall be ignored below. 20160212
#APP-LINK-$(CONFIG_MW_CAS_IRDETO_LOADER_DVT) += -L$(APPL_MAKE_DIR)/irldvt/lib
#APP-LINK-$(CONFIG_MW_CAS_IRDETO_LOADER_DVT) += -lirldvt
######################################################################################
#
#        3rd party CAS Libraries
#
######################################################################################

#----------------------------------------------
#		NAGRA CAS
#----------------------------------------------
APP-LINK-$(CONFIG_MW_CAS_NAGRA) += -lnocs

ifeq ($(CONFIG_MW_CAS_NAGRA_NOCS_3_X), y)
APP-LINK-y					+= -lssl -lcrypto
APP-LINK-y					+= -ltommath
APP-LINK-y					+= -ltomcrypt
endif

ifeq ($(CONFIG_MW_CAS_NAGRA_DALTEST), y)
APP-LINK-y					+= -ldalts
else

ifeq ($(CONFIG_MW_CAS_NAGRA_CA_PRODUCT_ICME), y)
ifeq ($(CONFIG_DEBUG), y)
APP-LINK-y								+= -lcak_icme_debug
#APP-LINK-$(CONFIG_MW_CAS_NAGRA_OP_ERT)	+= -lcak_icme_debug_NAK
APP-LINK-$(CONFIG_MW_CAS_NAGRA_OP_ERT)	+= -lcak_icme_debug_ERC
else
APP-LINK-y								+= -lcak_icme_release
#APP-LINK-$(CONFIG_MW_CAS_NAGRA_OP_ERT)	+= -lcak_icme_release_NAK
APP-LINK-$(CONFIG_MW_CAS_NAGRA_OP_ERT)	+= -lcak_icme_release_ERC
endif
endif

ifeq ($(CONFIG_MW_CAS_NAGRA_CA_PRODUCT_ME), y)
ifeq ($(CONFIG_DEBUG), y)
APP-LINK-y					+= -lcak_me_debug
APP-LINK-$(CONFIG_MW_CAS_NAGRA_OP_SES)	+= -lcak_me_debug_DOL
#APP-LINK-$(CONFIG_MW_CAS_NAGRA_OP_SES)	+= -lcak_me_debug_NAK
else
APP-LINK-y					+= -lcak_me_release
APP-LINK-$(CONFIG_MW_CAS_NAGRA_OP_SES)	+= -lcak_me_release_DOL
#APP-LINK-$(CONFIG_MW_CAS_NAGRA_OP_SES)	+= -lcak_me_release_NAK
endif
endif

ifeq ($(CONFIG_MW_CAS_NAGRA_CA_PRODUCT_ALME), y)
ifeq ($(CONFIG_MW_CAS_NAGRA_OP_AMX), y)
ifeq ($(CONFIG_DEBUG), y)
#APP-LINK-y	+=	-lcak_alme_debug_NAK
#APP-LINK-y  +=	-lsub_alme_debug_NAK
APP-LINK-y  +=  -lcak_alme_debug_AMX
APP-LINK-y  +=  -lsub_alme_debug_AMX
else
#APP-LINK-y  +=  -lcak_alme_release_NAK
#APP-LINK-y  +=  -lsub_alme_release_NAK
APP-LINK-y  +=  -lcak_alme_release_AMX
APP-LINK-y  +=  -lsub_alme_release_AMX
endif
endif
endif

ifeq ($(CONFIG_MW_CAS_NAGRA_PRM), y)
ifeq ($(CONFIG_DEBUG), y)
APP-LINK-y								+= -ldvl_debug
APP-LINK-$(CONFIG_MW_CAS_NAGRA_OP_ERT)	+= -ldvl_debug_ert
#APP-LINK-$(CONFIG_MW_CAS_NAGRA_OP_ERT)	+= -ldvl_debug_cso_test
APP-LINK-$(CONFIG_MW_CAS_NAGRA_OP_SES)	+= -ldvl_debug_aps_hd
else
APP-LINK-y								+= -ldvl_release
APP-LINK-$(CONFIG_MW_CAS_NAGRA_OP_ERT)	+= -ldvl_release_ert
#APP-LINK-$(CONFIG_MW_CAS_NAGRA_OP_ERT)	+= -ldvl_release_cso_test
APP-LINK-$(CONFIG_MW_CAS_NAGRA_OP_SES)	+= -ldvl_release_aps_hd
endif
endif
endif
#----------------------------------------------
#		IRDETO CAS
#----------------------------------------------
ifeq ($(CONFIG_MW_CAS_IRDETO), y)
ifeq ($(CONFIG_PROD_ICORDHDPLUSIR), y)
APP-LINK-$(CONFIG_MW_CAS_IRDETO)				+= -ls3_dvb_client_3_6_23
endif
ifeq ($(CONFIG_PROD_IRHD5100C), y)
APP-LINK-$(CONFIG_MW_CAS_IRDETO)				+= -ls3_dvb_client_3_6_28
endif
ifeq ($(CONFIG_PROD_IHDR5200C), y)
APP-LINK-$(CONFIG_MW_CAS_IRDETO)				+= -ls3_dvb_client_3_6_28
endif
ifeq ($(CONFIG_PROD_ICORDHDPLUSME), y)
APP-LINK-$(CONFIG_MW_CAS_IRDETO)				+= -ls3_dvb_client_3_10_30
endif
ifeq ($(CONFIG_PROD_HD5500T), y)
APP-LINK-$(CONFIG_MW_CAS_IRDETO)				+= -ls3_dvb_client_3_6_28
endif
ifeq ($(CONFIG_PROD_HDH100S), y)
APP-LINK-$(CONFIG_MW_CAS_IRDETO)				+= -ls3_dvb_client_3_9_8
endif
ifeq ($(CONFIG_PROD_HM9502HD), y)
APP-LINK-$(CONFIG_MW_CAS_IRDETO)				+= -ls3_dvb_client_3_9_16
endif
ifeq ($(CONFIG_PROD_IR2020HD), y)
APP-LINK-$(CONFIG_MW_CAS_IRDETO)				+= -ls3_dvb_client_3_10_30
endif
ifeq ($(CONFIG_PROD_IRHD5100S), y)
APP-LINK-$(CONFIG_MW_CAS_IRDETO)				+= -ls3_dvb_client_3_10_30
endif
ifeq ($(CONFIG_PROD_IRHD5100CTVT), y)
APP-LINK-$(CONFIG_MW_CAS_IRDETO)				+= -ls3_dvb_client_3_6_28
endif
ifeq ($(CONFIG_PROD_IRHD5200S), y)
APP-LINK-$(CONFIG_MW_CAS_IRDETO)				+= -ls3_dvb_client_3_9_21
endif
ifeq ($(CONFIG_PROD_ICORDHDPLUS), y)
APP-LINK-$(CONFIG_MW_CAS_IRDETO)				+= -ls3_dvb_client_3_10_30
endif
ifeq ($(CONFIG_PROD_BASE7356), y)
APP-LINK-$(CONFIG_MW_CAS_IRDETO)				+= -ls3_dvb_client_3_10_30
endif

ifeq ($(CONFIG_PROD_HDFOXIRCONNECT), y)
APP-LINK-$(CONFIG_MW_CAS_IRDETO_SCELL)			+= -ls3_dvb_client_3_17_0
endif

ifeq ($(CONFIG_PROD_IR4000HD), y)
APP-LINK-$(CONFIG_MW_CAS_IRDETO_SCELL)			+= -ls3_dvb_client_3_15_0
# Irdeto Universal Client
APP-LINK-$(CONFIG_MW_CAS_IRDETO_UCAS)			+= -lCloakedCAAgent
APP-LINK-$(CONFIG_MW_CAS_IRDETO_UCAS)			+= -ls3_dvb_client_3_15_0
#APP-LINK-$(CONFIG_MW_CAS_IRDETO_UCAS)			+= -lxc_wb
endif
ifeq ($(CONFIG_PROD_IHDR6100C), y)
APP-LINK-$(CONFIG_MW_CAS_IRDETO)                                += -ls3_dvb_client_3_15_0
endif

ifeq ($(CONFIG_PROD_IRUC), y)
# temporary using softcell3
APP-LINK-$(CONFIG_MW_CAS_IRDETO)				+= -ls3_dvb_client_3_6_28
# Irdeto Universal Client
APP-LINK-$(CONFIG_MW_CAS_IRDETO_UCAS)			+= -lUniversalClient
APP-LINK-$(CONFIG_MW_CAS_IRDETO_UCAS)			+= -lsecure_core_C00_S00_V01_T01
APP-LINK-$(CONFIG_MW_CAS_IRDETO_UCAS)			+= -llibxc_wb

endif

ifeq ($(CONFIG_PROD_IRUDTT), y)
# temporary using softcell3
APP-LINK-$(CONFIG_MW_CAS_IRDETO)				+= -ls3_dvb_client_3_6_28
# Irdeto Universal Client
APP-LINK-$(CONFIG_MW_CAS_IRDETO_UCAS)			+= -lUniversalClient
APP-LINK-$(CONFIG_MW_CAS_IRDETO_UCAS)			+= -lsecure_core_C00_S00_V01_T01
APP-LINK-$(CONFIG_MW_CAS_IRDETO_UCAS)			+= -llibxc_wb
endif

ifeq ($(CONFIG_PROD_M1), y)
	ifeq ($(CONFIG_MW_CAS_IRDETO_UCAS_VM_VAILDATOR), y)
		APP-LINK-y								+= -lIrdetoVMValidator
	else
		APP-LINK-y								+= -lCloakedCAAgent
		APP-LINK-y								+= -ls3_dvb_client_3.21.0
	endif
endif

endif

#----------------------------------------------
#		VIACCESS CAS
#----------------------------------------------
ifeq ($(CONFIG_MW_CAS_VIACCESS), y)
APP-LINK-$(CONFIG_MW_CAS_VIACCESS)				+= $(AP_STATIC_LIB_DIR)/acs_va.a
endif

#----------------------------------------------
#		VERIMATRIX CAS
#----------------------------------------------
ifeq ($(CONFIG_MW_CAS_VERIMATRIX), y)
ifeq ($(CONFIG_MW_CAS_VERIMATRIX_IPTV), y)
ifeq ($(CONFIG_DEBUG), y)
APP-LINK-y								+= -lvmclient
APP-LINK-y								+= -lvmlogger
else
APP-LINK-y								+= -lvmclient
APP-LINK-y								+= -lvmlogger
endif
else

endif
endif

