#####################################################
# Module dependent link and library
#####################################################

NEXUS_CLIENT=$(wildcard $(HUMAX_APP_STAG_DIR)/root/usr/lib/libnexus_client.so)
ifneq ($(NEXUS_CLIENT),)
APP-LINK-y			+= -lnexus_client
endif

APP-LINK-y			+= -lmemory-null
#APP-LINK-y			+= -lmemory-debug

APP-LINK-y += -lsapi -lhlib -lhapi -lpapi -ldapi -ldlib -lrlib -lsilib -lsamalib -loapi

# library for HLIB
APP-LINK-y					+= -lstdc++
APP-LINK-$(CONFIG_HPACKAGE_VKERNEL)		+= -lvkernel
APP-LINK-$(CONFIG_PACKAGE_CAP)		+= -lcap -lattr
###########################################################################

APP-LINK-y								+= -pthread -lrt -lssp -lm

APP-LINK-y += -L$(OCTOTOP_SRC_DIR)/testkit/cthrough/lib/lua/lib
APP-LINK-y += -llua
APP-LINK-$(CONFIG_FUNC_USE_DAPI_DIRECT_QUERY)			+= -lsqlite3
