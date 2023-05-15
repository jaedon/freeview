#####################################################
# This is for package's link and compile
#####################################################

#----------------------------------------------
#		Default path setting.
#----------------------------------------------
#
APP-LINK-y := -L$(HUMAX_APP_STAG_ROOT_DIR)/lib  -L$(HUMAX_APP_STAG_ROOT_DIR)/usr/lib -L$(AP_STATIC_LIB_DIR)
#----------------------------------------------

PRIVATE_LIB=$(wildcard $(HUMAX_CURMAKE_DIR)/HumaxLinkLibrary.mak)
ifneq ($(PRIVATE_LIB),)
include $(HUMAX_CURMAKE_DIR)/HumaxLinkLibrary.mak
else

######################################################################################
#
#        Platform default Library : gcc specific options.
#        (please refer http://goo.gl/lWJmM)
#
######################################################################################

APP-LINK-y			+= -pthread -lrt -lssp -lm

######################################################################################
#
#        HUMAX default Application Library :
#
######################################################################################

#APP-LINK-y += -lmemory-debug
APP-LINK-y += -lmemory-null

ifneq ($(wildcard $(HUMAX_APP_STAG_DIR)/root/usr/lib/libhlib.so),)
APP-LINK-y += -lhlib
endif

ifneq ($(wildcard $(HUMAX_APP_STAG_DIR)/root/usr/lib/libhapi.so),)
APP-LINK-y += -lhapi
endif
ifneq ($(wildcard $(HUMAX_APP_STAG_DIR)/root/usr/lib/liboapi.so),)
APP-LINK-y += -loapi
endif
ifneq ($(wildcard $(HUMAX_APP_STAG_DIR)/root/usr/lib/libdapi.so),)
APP-LINK-y += -ldapi
endif
ifneq ($(wildcard $(HUMAX_APP_STAG_DIR)/root/usr/lib/libummalib.so),)
APP-LINK-y += -lummalib
endif
ifneq ($(wildcard $(HUMAX_APP_STAG_DIR)/root/usr/lib/libuapi.so),)
APP-LINK-y += -luapi
endif
ifneq ($(wildcard $(HUMAX_APP_STAG_DIR)/root/usr/lib/libthapi.so),)
APP-LINK-y += -lthapi
endif
ifneq ($(wildcard $(HUMAX_APP_STAG_DIR)/root/usr/lib/libdlib.so),)
APP-LINK-y += -ldlib
endif
ifneq ($(wildcard $(HUMAX_APP_STAG_DIR)/root/usr/lib/libclib.so),)
APP-LINK-y += -lclib
endif
ifneq ($(wildcard $(HUMAX_APP_STAG_DIR)/root/usr/lib/libsilib.so),)
APP-LINK-y += -lsilib
endif
ifneq ($(wildcard $(HUMAX_APP_STAG_DIR)/root/usr/lib/libsitextlib.so),)
APP-LINK-y += -lsitextlib
endif
ifneq ($(wildcard $(HUMAX_APP_STAG_DIR)/root/usr/lib/librlib.so),)
APP-LINK-y += -lrlib
endif
ifneq ($(wildcard $(HUMAX_APP_STAG_DIR)/root/usr/lib/libsecapi.so),)
APP-LINK-y += -lsecapi
endif
ifneq ($(wildcard $(HUMAX_APP_STAG_DIR)/root/usr/lib/libpapi.so),)
APP-LINK-y += -lpapi
endif
ifneq ($(wildcard $(HUMAX_APP_STAG_DIR)/root/usr/lib/libsamalib.so),)
APP-LINK-y += -lsamalib
endif
ifneq ($(wildcard $(HUMAX_APP_STAG_DIR)/root/usr/lib/libsapi.so),)
APP-LINK-y += -lsapi
endif
ifneq ($(wildcard $(HUMAX_APP_STAG_DIR)/root/usr/lib/libnapi.so),)
APP-LINK-y += -lnapi
endif
ifneq ($(wildcard $(HUMAX_APP_STAG_DIR)/root/usr/lib/libncapi.so),)
APP-LINK-y += -lncapi
endif
ifneq ($(wildcard $(HUMAX_APP_STAG_DIR)/root/usr/lib/libtrapi.so),)
APP-LINK-y += -ltrapi
endif
######################################################################################
#
#        GENERAL Open Source Library
#
######################################################################################

APP-LINK-$(CONFIG_PACKAGE_WEBSOCKET)		+= -lwebsockets
APP-LINK-$(CONFIG_PACKAGE_JPEG)			+= -ljpeg
APP-LINK-$(CONFIG_PACKAGE_FREETYPE)		+= -lfreetype
APP-LINK-$(CONFIG_PACKAGE_LIBPNG)		+= -lpng -lz
APP-LINK-$(CONFIG_PACKAGE_ID3LIB)		+= -lid3 -lz -lstdc++
APP-LINK-$(CONFIG_PACKAGE_LIBEXIF)		+= -lexif
APP-LINK-$(CONFIG_PACKAGE_LIBEXT2FS)	+= -lblkid -lcom_err -le2p -lss -luuid -lext2fs
APP-LINK-$(CONFIG_PACKAGE_CURL)			+= -lcurl
APP-LINK-$(CONFIG_PACKAGE_OPENSSL)		+= -lssl -lcrypto
APP-LINK-$(CONFIG_PACKAGE_LIBXML)		+= -lxml
APP-LINK-$(CONFIG_PACKAGE_LIBXML2)		+= -lxml2
APP-LINK-$(CONFIG_PACKAGE_LIBNCURSES)	+= -lform -lmenu -lncurses -lpanel
#APP-LINK-$(CONFIG_PACKAGE_LIBLUA)		+= -llua
APP-LINK-$(CONFIG_PACKAGE_JANSSON)		+= -ljansson
APP-LINK-$(CONFIG_PACKAGE_LOG4C)		+= -llog4c
APP-LINK-$(CONFIG_PACKAGE_PCD)			+= -lpcd -lipc
APP-LINK-$(CONFIG_PACKAGE_ZLIB)			+= -lz
APP-LINK-$(CONFIG_PACKAGE_FFMPEG)		+= -lavcodec -lavfilter -lavformat -lavutil -lswresample -lswscale
APP-LINK-$(CONFIG_PACKAGE_CAP)			+= -lcap -lattr
APP-LINK-$(CONFIG_PACKAGE_KEYS) 		+= -lgpg-error -lgcrypt -lkeyutils -lecryptfs

### FOR DirectFB Platform
APP-LINK-$(CONFIG_PACKAGE_DIRECTFB)		+= -ldirectfb -lfusion -ldirect -ldl -lsawman
### FOR Shared library Vkernel.
APP-LINK-$(CONFIG_HPACKAGE_VKERNEL)		+= -lvkernel

APP-LINK-y				+= -lsqlite3

endif

PACKAGE_INC_DIR-$(CONFIG_PACKAGE_JPEG)			:= -I$(PACKAGES_DIR)/jpeg/include
PACKAGE_INC_DIR-$(CONFIG_PACKAGE_FREETYPE)		+= -I$(PACKAGES_DIR)/freetype/include -I$(PACKAGES_DIR)/freetype/include/freetype2
PACKAGE_INC_DIR-$(CONFIG_PACKAGE_LIBPNG)		+= -I$(PACKAGES_DIR)/libpng/include -I$(PACKAGES_DIR)/zlib/include
PACKAGE_INC_DIR-$(CONFIG_PACKAGE_ID3LIB)		+= -I$(PACKAGES_DIR)/id3lib/include -I$(PACKAGES_DIR)/zlib/include
PACKAGE_INC_DIR-$(CONFIG_PACKAGE_LIBEXIF)		+= -I$(PACKAGES_DIR)/libexif/include
PACKAGE_INC_DIR-$(CONFIG_PACKAGE_LIBEXT2FS)		+= -I$(PACKAGES_DIR)/libext2fs/include
PACKAGE_INC_DIR-$(CONFIG_PACKAGE_CURL)			+= -I$(PACKAGES_DIR)/libcurl/include
PACKAGE_INC_DIR-$(CONFIG_PACKAGE_OPENSSL)		+= -I$(PACKAGES_DIR)/openssl/include
PACKAGE_INC_DIR-$(CONFIG_PACKAGE_LIBXML)		+= -I$(PACKAGES_DIR)/libxml/include
PACKAGE_INC_DIR-$(CONFIG_PACKAGE_LIBXML2)		+= -I$(PACKAGES_DIR)/libxml2/include
PACKAGE_INC_DIR-$(CONFIG_PACKAGE_LIBNCURSES)	+= -I$(PACKAGES_DIR)/ncurses/include
PACKAGE_INC_DIR-$(CONFIG_PACKAGE_LIBLUA)		+= -I$(PACKAGES_DIR)/lua/lib/include
PACKAGE_INC_DIR-$(CONFIG_PACKAGE_JANSSON)		+= -I$(PACKAGES_DIR)/jansson/include
PACKAGE_INC_DIR-$(CONFIG_PACKAGE_WEBSOCKET)		+= -I$(PACKAGES_DIR)/libwebsocket/include
PACKAGE_INC_DIR-$(CONFIG_PACKAGE_LOG4C)			+= -I$(PACKAGES_DIR)/log4c/include
PACKAGE_INC_DIR-$(CONFIG_PACKAGE_PCD)			+= -I$(PACKAGES_DIR)/pcd/lib/include
PACKAGE_INC_DIR-$(CONFIG_PACKAGE_CAP)		+= -I$(PACKAGES_DIR)/libcap/usr/include

PACKAGE_INC_DIR-$(CONFIG_PACKAGE_FFMPEG)		+= -I$(PACKAGES_DIR)/ffmpeg/include

PACKAGE_INC_DIR-$(CONFIG_PACKAGE_DIRECTFB)			+= -I$(HUMAX_TOP_DIR)/device/include/directfb
PACKAGE_INC_DIR-$(CONFIG_PACKAGE_DIRECTFB)			+= -I$(HUMAX_TOP_DIR)/device/include/fusion
PACKAGE_INC_DIR-$(CONFIG_PACKAGE_DIRECTFB)			+= -I$(HUMAX_TOP_DIR)/device/include/sawman

PACKAGE_INC_DIR-y		+= -I$(PACKAGES_DIR)/sqlite/include

PACKAGE_INC_DIR-y		+= -I$(OCTOLIB_SRC_DIR)/memory
#----------------------------------------------


