#####################################################
# Private application link option
#####################################################

# HOMMA use appkit
APP-LINK-y += -lappkit

APP-LINK-y += -lclib
APP-LINK-y += -lsqlite3

APP-LINK-y += -lmxdlna_core -lmxkernel
APP-LINK-y += -lmxdtcp -lmxfile -lmxmedia -lmxres

APP-LINK-$(CONFIG_HOMMA_DMP) += -lmxdlna_mscp
APP-LINK-$(CONFIG_HOMMA_DMS) += -lmxdlna_dms -lTranscoder_Control -lHMS_transcoderWrapper -lmxsqlite -lmxdbc
APP-LINK-$(CONFIG_HOMMA_DMR) += -lmxdlna_dmr
APP-LINK-$(CONFIG_HOMMA_WOON) += -lras
APP-LINK-$(CONFIG_HOMMA_DIAL) += -lmxdial_server

APP-LINK-$(CONFIG_HOMMA_CDI) += -ldi_crypt
APP-LINK-$(CONFIG_HOMMA_SATIP) += -lmxsatip_server -lmxsatip_upnp_server -lmxrtsp_server -lmxrtp_server -llive555
APP-LINK-$(CONFIG_HOMMA_AIRPLAY) += -lairplay -lavahi-common -lavahi-core -lplist -lxml2 -lssl -lcrypto

## Calypso Core Library
APP-LINK-y += -lcalypso
## Calypso IPC Library
APP-LINK-y += -lcso_ipc
## PlayReady Library
APP-LINK-y += -lprdy
## ViewRightWeb Library
APP-LINK-y += -lViewRightWebWrapper -lViewRightWebClient -lstdc++
## Calypso OEM Porting Library
APP-LINK-y += -lmi

APP-LINK-$(CONFIG_PACKAGE_FFMPEG)		+= -lavcodec -lavfilter -lavformat -lavutil -lswresample -lswscale

APP-LINK-$(CONFIG_SUPPORT_UMMA_FTP_SERVER) += -lcrypt

# Driver relatives
APP-LINK-y += -ldriver
ifeq ($(CONFIG_SUPPORT_NIMA), y)
APP-LINK-y += -ldi_network
APP-LINK-y += -ldi_wifi
APP-LINK-y += -ldrv_hotplug
APP-LINK-$(CONFIG_MW_INET_PPPOE) += -ldi_pppoe
APP-LINK-$(CONFIG_PROD_DEVICE_BLUETOOTH) += -lbsa -ldi_bt
endif


APP-LINK-y += -ldrv_hotplug
APP-LINK-$(CONFIG_PROD_DEVICE_STORAGE_USB) += -ldi_usb
APP-LINK-$(CONFIG_PROD_DEVICE_STORAGE_MMC) += -ldi_mmc
APP-LINK-$(CONFIG_PROD_DEVICE_FILE_SYSTEM) += -ldi_fs
APP-LINK-$(CONFIG_PROD_DEVICE_STORAGE_HDD) += -ldi_hdd


ifeq ($(CONFIG_SUPPORT_IMMA), y)
APP-LINK-y += -lmediapb_ipc
endif

ifeq ($(CONFIG_HOMMA_CDI), y)
APP-LINK-y += -ldi_crypt
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

APP-LINK-y			+= -lmemory-null
#APP-LINK-y			+= -lmemory-debug

APP-LINK-y								+= -pthread -lrt -lssp -lm
APP-LINK-y								+= -lhlib -ldlib -lsilib -lrlib -lsamalib
APP-LINK-y			+= -locto.oneapi
ifeq ($(CONFIG_HUMAX_CRASHREPORT),y)
APP-LINK-y			+= -lhcrlib
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
APP-LINK-$(CONFIG_MW_CAS_NAGRA_OP_SES)	+= -lcak_me_debug_NAK
else
APP-LINK-y					+= -lcak_me_release
APP-LINK-$(CONFIG_MW_CAS_NAGRA_OP_SES)	+= -lcak_me_release_DOL
APP-LINK-$(CONFIG_MW_CAS_NAGRA_OP_SES)	+= -lcak_me_release_NAK
endif
endif

ifeq ($(CONFIG_MW_CAS_NAGRA_PRM), y)
ifeq ($(CONFIG_DEBUG), y)
APP-LINK-y								+= -ldvl_debug
#APP-LINK-$(CONFIG_MW_CAS_NAGRA_OP_ERT)	+= -ldvl_debug_ert
APP-LINK-$(CONFIG_MW_CAS_NAGRA_OP_ERT)	+= -ldvl_debug_cso_test
APP-LINK-$(CONFIG_MW_CAS_NAGRA_OP_SES)	+= -ldvl_debug_cso_test
else
APP-LINK-y								+= -ldvl_release
APP-LINK-$(CONFIG_MW_CAS_NAGRA_OP_ERT)	+= -ldvl_release_ert
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


endif
#----------------------------------------------
#		VIACCESS CAS
#----------------------------------------------
ifeq ($(CONFIG_MW_CAS_VIACCESS), y)
APP-LINK-$(CONFIG_MW_CAS_VIACCESS)				+= $(AP_STATIC_LIB_DIR)/acs_va.a
endif


