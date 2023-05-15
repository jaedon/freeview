# bwin.mak
# This file should be included by applications linking to bwin.
#
# Inputs: BSEAV, B_RESW_ARCH
# Outputs: BWIN_CFLAGS, BWIN_LDFLAGS

BWIN_DIR = $(BSEAV)/lib/bwin
ifeq ($(B_REFSW_DEBUG),n)
DEBUG_SUFFIX=release
else
DEBUG_SUFFIX=debug
endif

BWIN_LIBDIR = $(BWIN_DIR)/lib/$(B_REFSW_ARCH).$(DEBUG_SUFFIX)

BWIN_CFLAGS = -I$(BWIN_DIR)/include
BWIN_LDFLAGS = -L$(BWIN_LIBDIR) -lbwin

# By default, use freetype offline and use
# prerendered fonts in the application.
FREETYPE_SUPPORT ?= n
FREETYPE_DIR = $(BSEAV)/lib/freetype-2.1.5

PNG_SUPPORT = y
JPEG_SUPPORT = y

LIB_DIR = $(BSEAV)/lib

ifeq ($(SYSTEM),vxworks)
EXIF_SUPPORT = n
else
EXIF_SUPPORT = y
endif

LIBEXIF_DIR = $(BSEAV)/lib/libexif-0.6.12
LIBPNG_DIR = $(BSEAV)/lib/libpng
ZLIB_DIR = $(BSEAV)/lib/zlib
LIBJPEG_DIR = $(BSEAV)/lib/jpeg-6b

ifeq ($(FREETYPE_SUPPORT),y)
BWIN_LDFLAGS += -L$(FREETYPE_DIR)/lib/$(B_REFSW_ARCH).$(DEBUG_SUFFIX) -lfreetype
endif

ifeq ($(PNG_SUPPORT),y)
BWIN_LDFLAGS += -L$(LIBPNG_DIR) -lpng -L$(ZLIB_DIR)/$(B_REFSW_ARCH) -lz
endif

ifeq ($(JPEG_SUPPORT),y)
BWIN_LDFLAGS += -L$(LIBJPEG_DIR) -ljpeg
endif

ifeq ($(EXIF_SUPPORT),y)
BWIN_LDFLAGS += -L$(LIBEXIF_DIR)/libexif/.libs -lexif
endif

