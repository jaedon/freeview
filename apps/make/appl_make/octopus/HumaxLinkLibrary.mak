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

APP-LINK-y += -lhlib -lhapi -londk -lappkit -loapi -ldlib -luapi -ldapi -lrlib -lsamalib -lsapi -lnapi -lpapi -lsilib -ltapi -lsitextlib

APP-LINK-$(CONFIG_FUNC_USE_DAPI_DIRECT_QUERY)			+= -lsqlite3

# library for HLIB
APP-LINK-$(CONFIG_HPACKAGE_VKERNEL)		+= -lvkernel
###########################################################################

APP-LINK-y								+= -pthread -lrt -lssp -lm
APP-LINK-$(CONFIG_PACKAGE_DIRECTFB)		+= -ldirectfb -lfusion -ldirect -ldl -lsawman -lstdc++ -lz

# LUA
#APP-LINK-y += -L$(APPLTOP_SRC_DIR)/octopus/parser/luascript/lib
#APP-LINK-y += -llua

APP-LINK-y += -ldbus-1
APP-LINK-y += -ldbus-c++-1 -lintl
