#####################################################
# Module dependent link and library
#####################################################
APP-LINK-y := -L$(HUMAX_APP_STAG_DIR)/root/lib  -L$(HUMAX_APP_STAG_DIR)/root/usr/lib -L$(AP_STATIC_LIB_DIR)

NEXUS_CLIENT=$(wildcard $(HUMAX_APP_STAG_DIR)/root/usr/lib/libnexus_client.so)
ifneq ($(NEXUS_CLIENT),)
APP-LINK-y			+= -lnexus_client
endif

APP-LINK-y			+= -lmemory-null
#APP-LINK-y			+= -lmemory-debug

APP-LINK-y += -lhlib -lhapi -lappkit -loapi -ldlib -ldapi -lpapi  -luapi -lsamalib -lsapi -lrlib -lsitextlib -ltapi
APP-LINK-$(CONFIG_SUPPORT_NIMA)			+= -lnapi

APP-LINK-$(CONFIG_FUNC_USE_DAPI_DIRECT_QUERY)			+= -lsqlite3

ifeq ($(CONFIG_SUPPORT_REMOTEAPP_ON_USERDEVICE),y)
APP-LINK-y += -lmxdlna_core -lmxkernel
APP-LINK-y += -lmxdtcp -lmxfile
APP-LINK-y += -lmxdlna_userdevice
APP-LINK-y += -lssl -lxml2 -lcrypto
endif
APP-LINK-$(CONFIG_PACKAGE_CAP)  += -lcap -lattr

ifeq ($(CONFIG_SUPPORT_NOVA_APP),y)
APP-LINK-y += -lnovaapi
endif

# library for HLIB
APP-LINK-$(CONFIG_HPACKAGE_VKERNEL)		+= -lvkernel
###########################################################################

APP-LINK-y								+= -pthread -lrt -lssp -lm

