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

APP-LINK-y			+= -pthread -lrt -lssp
APP-LINK-y			+= -lappkit
APP-LINK-y			+= -loapi -lhapi -ldapi -ldlib -lsitextlib -lsilib -lnapi -lsapi -lrlib -luapi -lsamalib -lpapi -lhlib -lvkernel -ltapi -lnovaapi
#APP-LINK-y			+= -lNovaspreadServer
APP-LINK-y			+= -lNovaspreadServerBCM7252SHGS1000S
APP-LINK-y			+= -lInarisCommonBCM7252SHGS1000S
APP-LINK-y			+= -lInarisHttpServerBCM7252SHGS1000S
APP-LINK-y			+= -lInarisLogManagerBCM7252SHGS1000S
APP-LINK-y			+= -lInarisOsalBCM7252SHGS1000S
APP-LINK-y			+= -lInarisRtspServerBCM7252SHGS1000S
APP-LINK-y			+= -lInarisSatIpServerBCM7252SHGS1000S
APP-LINK-y			+= -lInarisUpnpBCM7252SHGS1000S
APP-LINK-y			+= -lInarisXmlParserBCM7252SHGS1000S
APP-LINK-y			+= -lInarisXmlWriterBCM7252SHGS1000S
APP-LINK-y			+= -lixml
APP-LINK-y			+= -lthreadutil
APP-LINK-y			+= -lupnp
APP-LINK-y			+= -luuid
APP-LINK-y			+= -lxml2
APP-LINK-$(CONFIG_PACKAGE_CAP)	+= -lcap -lattr
APP-LINK-y			+= -lmi
APP-LINK-y          += -lnexus


APP-LINK-$(CONFIG_FUNC_USE_DAPI_DIRECT_QUERY)			+= -lsqlite3

APP-LINK-y += -ldirectfb -lfusion -ldirect -ldl -lsawman -lz
APP-LINK-y += -lm
APP-LINK-y += -lstdc++
APP-LINK-y += -ljpeg -lpng -lcrypto -lcurl -lssl

PACKAGE_INC_DIR-y += -I$(HUMAX_TOP_DIR)/device/include/directfb
PACKAGE_INC_DIR-y += -I$(HUMAX_TOP_DIR)/device/include/fusion
PACKAGE_INC_DIR-y += -I$(HUMAX_TOP_DIR)/device/include/sawman
PACKAGE_INC_DIR-y += -I$(PACKAGES_DIR)/pcd/lib/include
PACKAGE_INC_DIR-y += -I$(PACKAGES_DIR)/sqlite/include

PACKAGE_INC_DIR-y += -I$(THIRDPARTY_DIR)/nova/include
