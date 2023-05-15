#####################################################
# Module dependent link and library
#####################################################
APP-LINK-y := -L$(HUMAX_APP_STAG_DIR)/root/lib  -L$(HUMAX_APP_STAG_DIR)/root/usr/lib -L$(AP_STATIC_LIB_DIR)

ifeq ($(CONFIG_PROD_DEVICE_KERNEL_MODE), y)
APP-LINK-y			+= -lnexus
else
APP-LINK-y			+= -lnexus_client
endif

APP-LINK-y			+= -lmemory-null
#APP-LINK-y			+= -lmemory-debug
## Di demux Porting Library 
APP-LINK-y			+= -ldidemux_ext

APP-LINK-y			+= -pthread -lrt -lssp
APP-LINK-y			+= -lappkit
APP-LINK-y			+= -loapi -lhapi -ldapi -ldlib -lsitextlib -lsilib -lnapi -lsapi -lrlib -luapi -lsamalib -lpapi -lhlib -lvkernel -ltapi
APP-LINK-y			+= -lredkey $(AP_STATIC_LIB_DIR)/license.lo
APP-LINK-y			+= -lfreetype

APP-LINK-$(CONFIG_FUNC_USE_DAPI_DIRECT_QUERY)			+= -lsqlite3

APP-LINK-y += -ldirectfb -lfusion -ldirect -ldl -lsawman -lz
APP-LINK-y += -lm
APP-LINK-y += -lstdc++
APP-LINK-y += -ljpeg -lpng -lcrypto -lcurl -lssl
APP-LINK-$(CONFIG_PACKAGE_CAP)	+= -lcap -lattr

ifeq ($(CONFIG_SUPPORT_NOVA_APP), y)
APP-LINK-y += -lnovaapi
endif

PACKAGE_INC_DIR-y += -I$(HUMAX_TOP_DIR)/device/include/directfb
PACKAGE_INC_DIR-y += -I$(HUMAX_TOP_DIR)/device/include/fusion
PACKAGE_INC_DIR-y += -I$(HUMAX_TOP_DIR)/device/include/sawman
PACKAGE_INC_DIR-y += -I$(PACKAGES_DIR)/pcd/lib/include
PACKAGE_INC_DIR-y += -I$(PACKAGES_DIR)/sqlite/include

PACKAGE_INC_DIR-y += -I$(THIRDPARTY_DIR)/mheg/itk/include
