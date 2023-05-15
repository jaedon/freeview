#####################################################
# Module dependent link and library
#####################################################
APP-LINK-y := -L$(HUMAX_APP_STAG_DIR)/root/lib  -L$(HUMAX_APP_STAG_DIR)/root/usr/lib -L$(AP_STATIC_LIB_DIR)

#NEXUS_CLIENT=$(wildcard $(HUMAX_APP_STAG_DIR)/root/usr/lib/libnexus_client.so)
#ifneq ($(NEXUS_CLIENT),)
#APP-LINK-y			+= -lnexus_client
#endif

APP-LINK-y			+= -lmemory-null
#APP-LINK-y			+= -lmemory-debug

APP-LINK-y			+= -pthread -lrt -lssp
APP-LINK-y			+= -lhlib -loapi -lvkernel -lappkit -lhapi -lnapi -luapi -lsapi -lpapi -lsamalib -lrlib -londk -ldapi -lsilib -ldlib -ldriver

ifneq ($(wildcard $(HUMAX_APP_STAG_DIR)/root/usr/lib/libtapi.so),)
APP-LINK-y += -ltapi
endif

ifneq ($(wildcard $(HUMAX_APP_STAG_DIR)/root/usr/lib/libsitextlib.so),)
APP-LINK-y += -lsitextlib
endif
APP-LINK-$(CONFIG_HUMAX_CRASHREPORT) += -lhcrlib

APP-LINK-y += -ldirectfb -lfusion -ldirect  -lstdc++ -lz
#APP-LINK-y += -ldirectfb -lfusion -ldirect -ldl -lsawman -lz

APP-LINK-y += -lm -ljansson
APP-LINK-y += -lcrypto
APP-LINK-y += -lcurl
APP-LINK-y += -lssl
APP-LINK-y += -lxml2

#PACKAGE_INC_DIR-y += -I$(HUMAX_TOP_DIR)/device/include/directfb
#PACKAGE_INC_DIR-y += -I$(HUMAX_TOP_DIR)/device/include/fusion
#PACKAGE_INC_DIR-y += -I$(HUMAX_TOP_DIR)/device/include/sawman



