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

APP-LINK-y += -lhlib -lappkit -loapi -luapi -lsamalib -lsapi -lnapi -lrlib -lhapi -ldlib -ldapi -lpapi -lsilib -lthapi -ltapi -lz
APP-LINK-$(CONFIG_FUNC_USE_DAPI_DIRECT_QUERY)			+= -lsqlite3

# library for HLIB
APP-LINK-y					+= -lstdc++
APP-LINK-$(CONFIG_HPACKAGE_VKERNEL)		+= -lvkernel
###########################################################################

APP-LINK-y								+= -pthread -lrt -lssp -lm

######################################################################################
#
#        Package Include Path
#
######################################################################################



