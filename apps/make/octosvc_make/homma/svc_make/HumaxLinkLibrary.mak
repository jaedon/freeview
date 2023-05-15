#####################################################
# Private application link option
#####################################################
ifeq ($(CONFIG_PROD_DEVICE_KERNEL_MODE),y)
NEXUS_LIB=nexus
else
NEXUS_LIB=nexus_client
endif

APP-LINK-$(CONFIG_SUPPORT_HOMMA_ASAN) += -lasan
APP-LINK-y += -lmxdlna_core -lmxkernel
APP-LINK-y += -lmxdtcp -lmxfile -lmxmedia -lmxres

APP-LINK-$(CONFIG_HOMMA_USERCP) += -lmxdlna_usercp
APP-LINK-$(CONFIG_HOMMA_DMP) += -lmxdlna_mscp
APP-LINK-$(CONFIG_HOMMA_DMS) += -lmxdlna_dms -lTranscoder_Control -lHMS_transcoderWrapper -lsqlite3 -lmxdbc
APP-LINK-$(CONFIG_HOMMA_DMR) += -lmxdlna_dmr
APP-LINK-$(CONFIG_HOMMA_WOON) += -lras
APP-LINK-$(CONFIG_HOMMA_DIAL) += -lmxdial_server

APP-LINK-$(CONFIG_HOMMA_CDI) += -l$(NEXUS_LIB)
APP-LINK-$(CONFIG_HOMMA_CDI) += -ldi_crypt
APP-LINK-$(CONFIG_HOMMA_SATIP) += -lmxsatip_server -lmxsatip_upnp_server -lmxrtsp_server -lmxrtp_server -llive555 -lTranscoder_Control -lHMS_transcoderWrapper
APP-LINK-$(CONFIG_HOMMA_AIRPLAY) += -lairplay -lavahi-common -lavahi-core -lplist -lxml2 -lssl -lcrypto

APP-LINK-y			+= -lmemory-null
#APP-LINK-y			+= -lmemory-debug

APP-LINK-y += -lhlib -lhapi -lpapi -loapi -ldapi -lnapi -lsitextlib -lsamalib -lsapi -ldlib -lrlib -luapi -lthapi -lclib -ltapi
APP-LINK-$(CONFIG_FUNC_USE_DAPI_DIRECT_QUERY)			+= -lsqlite3
APP-LINK-$(CONFIG_HUMAX_CRASHREPORT) += -lhcrlib
APP-LINK-$(CONFIG_TRAUMA_PROCESS)		+= -ltrapi
# library for HLIB
APP-LINK-y					+= -lstdc++
APP-LINK-$(CONFIG_HPACKAGE_VKERNEL)		+= -lvkernel
APP-LINK-$(CONFIG_PACKAGE_CAP)		+= -lcap -lattr
APP-LINK-y += -lssl -lxml2 -lcrypto
#----------------------------------------------
#		NAGRA CAS
#----------------------------------------------
ifeq ($(CONFIG_HOMMA_CDI), y)
APP-LINK-$(CONFIG_MW_CAS_NAGRA) += -lnocs
# APP-LINK-$(CONFIG_MW_CAS_NAGRA_NOCS_3_X) += -lcert
endif

###########################################################################

APP-LINK-y								+= -pthread -lrt -lssp -lm

