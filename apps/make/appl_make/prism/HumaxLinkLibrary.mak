#####################################################
# Module dependent link and library
#####################################################
APP-LINK-y := -L$(HUMAX_APP_STAG_DIR)/root/lib  -L$(HUMAX_APP_STAG_DIR)/root/usr/lib -L$(AP_STATIC_LIB_DIR)

NEXUS_CLIENT=$(wildcard $(HUMAX_APP_STAG_DIR)/root/usr/lib/libnexus_client.so)
ifneq ($(NEXUS_CLIENT),)
APP-LINK-y			+= -lnexus_client
endif
APP-LINK-$(CONFIG_SUPPORT_PRISM_APP_ASAN) += -lasan
APP-LINK-y			+= -lmemory-null
#APP-LINK-y			+= -lmemory-debug

ifeq ($(CONFIG_PRISM_ZAPPING),y)
APP-LINK-y += -lhlib -lhapi -londk -lappkit -loapi -ldlib -lsitextlib -luapi -ldapi -lrlib -lsamalib -lsapi -lpapi -ltapi -lclib
else
APP-LINK-y += -lhlib -lhapi -londk -lappkit -loapi -ldlib -lsitextlib -luapi -ldapi -lrlib -lsamalib -lsapi -lpapi -ltapi
endif
APP-LINK-$(CONFIG_SUPPORT_NIMA)			+= -lnapi
APP-LINK-$(CONFIG_FUNC_USE_DAPI_DIRECT_QUERY)			+= -lsqlite3

APP-LINK-$(CONFIG_HUMAX_CRASHREPORT) += -lhcrlib
# library for HLIB
APP-LINK-$(CONFIG_PACKAGE_CAP)  		+= -lcap -lattr

APP-LINK-$(CONFIG_HPACKAGE_VKERNEL)		+= -lvkernel
###########################################################################

APP-LINK-y								+= -pthread -lrt -lssp -lm
APP-LINK-$(CONFIG_PACKAGE_DIRECTFB)		+= -ldirectfb -lfusion -ldirect -ldl -lsawman -lstdc++ -lz

ifeq ($(CONFIG_SUPPORT_NOVA_APP), y)
APP-LINK-y += -lnovaapi
endif

