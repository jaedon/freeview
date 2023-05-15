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

APP-LINK-y += -lhlib -lappkit -loapi -luapi -lsamalib -lsapi -lnapi -lrlib -lhapi -ldlib -ldapi -lsitextlib -lpapi -lthapi -ltapi
APP-LINK-$(CONFIG_SUPPORT_NIMA)		+= -lnapi
APP-LINK-$(CONFIG_SUPPORT_REMA)		+= -lrapi
APP-LINK-$(CONFIG_FUNC_USE_DAPI_DIRECT_QUERY)			+= -lsqlite3

APP-LINK-$(CONFIG_HUMAX_CRASHREPORT) += -lhcrlib
# library for HLIB
APP-LINK-y					+= -lstdc++
APP-LINK-$(CONFIG_HPACKAGE_VKERNEL)		+= -lvkernel
APP-LINK-$(CONFIG_PACKAGE_CAP)		+= -lcap -lattr
###########################################################################

APP-LINK-y								+= -pthread -lrt -lssp -lm

ifeq ($(CONFIG_SUPPORT_NOVA_APP), y)
APP-LINK-y += -lnovaapi
endif

######################################################################################
#
#        Package Include Path
#
######################################################################################



