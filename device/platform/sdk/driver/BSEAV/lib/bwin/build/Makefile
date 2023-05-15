############################################################
#     Copyright (c) 2003-2012, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: Makefile $
# $brcm_Revision: 45 $
# $brcm_Date: 9/6/12 11:28p $
#
# Module Description:
#
# Revision History:
#
# Created: 02/09/2001 by Marcus Kellerman
#
# $brcm_Log: /BSEAV/lib/bwin/build/Makefile $
# 
# 45   9/6/12 11:28p mphillip
# SW7425-3739: Merge symlink removal to main
# 
# SW7425-3739/1   9/5/12 5:16p mphillip
# SW7425-3739: Remove BSEAV/lib symlinks
# 
# 44   9/6/12 6:04p erickson
# SW7435-349: add standard header
# 
# 43   6/15/12 4:11p tokushig
# SW7231-749: add dependency fix for parallel make problems in libjpeg
# and libexif
# 
# 42   3/24/11 4:35p jrubio
# SW7400-2511: take out ARCH and use B_REFSW_ARCH
# 
# 41   3/24/11 4:24p jrubio
# SW7400-2511: take out ARCH and use B_REFSW_ARCH
# 
# 40   1/8/10 10:57a erickson
# SW7550-121: set SHELL variable for bash syntax
#
# 39   8/12/08 4:54p erickson
# PR42789: move bluetooth build logic into
# BSEAV/app/brutus/build/Makefile.bluetooth
#
# 38   6/11/08 6:22p jgarrett
# PR 43556: Adding 97458 platform support
#
# 37   2/13/08 4:52p tokushig
# PR39534: added bluetooth remote capabilities to brutus
#
# 36   1/10/07 4:32p erickson
# PR26631: get rid of "Makefile has modification time in the future" by
# adding sleep. It's unforunate, but it appears the granularity of file
# creation vs. modification time can make this unavoidable. We need
# clean builds, so it seems worth it.
#
# 35   1/8/07 11:12a gmohile
# PR 25109: add 7403 support
#
# 34   12/19/06 11:42a erickson
# PR26631: don't use Makefiles, which are regenerated as part of the
# configure rule, as part of your build rules. I also made the
# libjpeg/libexif Config output uniform.
#
# 33   9/29/06 4:33p mward
# PR24604: create PLATFORM for 97118RNG.
#
# 32   9/12/06 5:01p tokushig
# PR24045, PR24048: move asf/drm from bwin to brutus
#
# SanDiego_DRM_ASF/9   9/12/06 3:56p tokushig
# moved asf/drm from bwin to brutus
#
# SanDiego_DRM_ASF/8   9/7/06 6:21p tokushig
# merge from main
#
# SanDiego_DRM_ASF/7   9/7/06 5:24p tokushig
# merge from main
#
# SanDiego_DRM_ASF/6   8/21/06 12:21p tokushig
# merge from 97398_200607
#
# 30   9/11/06 11:47a ptimariu
# PR20685: added lib/asf to -I CFLAGS in drm build
#
# 31   9/11/06 6:57p ptimariu
# PR20685: removed reference to drmutil.c
#
# 30   9/11/06 11:47a ptimariu
# PR20685: added lib/asf to -I CFLAGS in drm build
#
# 29   9/1/06 2:14p erickson
# PR22526: added ASF support to Brutus
#
# 28   8/22/06 4:15p dlwin
# PR 23822: Disable ASF/DRM support on default build
#
# 27   8/21/06 1:19a ptimariu
# PR20685: added drmhttp.c
#
# 26   8/11/06 3:17p erickson
# PR23574: configure libexif using ARCH
#
# 25   8/3/06 4:29p ptimariu
# PR20685: fix for KERNELMODE_SETTOPAPI=y asf build
#
# 24   7/29/06 4:00a ptimariu
# PR20685: reverted to platform check for ASF support
#
# 23   7/28/06 10:22p ptimariu
# PR20685: added check for ASF dir
#
# SanDiego_DRM_ASF/5   7/28/06 6:01p tokushig
# merge from main
#
# 22   7/28/06 12:46p tokushig
# PR20685: show unencrypted asf streams even if DRM_SUPPORT != y.  hide
# encrypted asf streams if DRM_SUPPORT != y.  show all asf streams if
# DRM_SUPPORT=y.
#
# 21   7/26/06 2:15p tokushig
# PR20685: added DRM support to brutus
#
# SanDiego_DRM_ASF/4   7/25/06 3:57p tokushig
# removed redundant cflags and unused lines from DRM_SUPPORT block
#
# SanDiego_DRM_ASF/3   7/20/06 6:06p tokushig
# merge from main branch
#
# 20   7/19/06 5:39p jgarrett
# PR 19909: Fixing make -j
#
# 19   7/17/06 5:09p tokushig
# PR19909: removed build warnings in libexif
#
# 18   7/14/06 11:31a jgarrett
# PR 19909: Merging to latest baseline
#
# PR19909/2   7/6/06 4:44p jgarrett
# PR 19909: Merging to latest baseline
#
# SanDiego_DRM_ASF/2   7/20/06 3:27p tokushig
# added asf/drm libs to the build
#
# 17   6/26/06 12:27p tokushig
# PR22205: temp fix for make -j 4 distclean
#
# 16   6/7/06 10:25a jgarrett
# PR 20139: Adding soft float for 97455/97456
#
# 15   6/6/06 5:46p tokushig
# PR21976: copy _stdint.h.orig to _stdint.h before building libexif
#
# 14   5/31/06 4:19p tokushig
# PR21891: Merging updated picture viewer into mainline.
#
# SanDiego_Brutus_Skin/12   5/30/06 5:26p tokushig
# add missing ';'
#
# SanDiego_Brutus_Skin/11   5/30/06 4:39p tokushig
# merge from main
#
# SanDiego_DRM_ASF/1   6/20/06 3:07p tokushig
# add windows media header parsing support
#
# 13   5/26/06 9:05a mward
# PR21671: Add support for 7118 chip 97118 board.
#
# SanDiego_Brutus_Skin/10   5/30/06 3:04p tokushig
# update call to libexif's 'configure' to accomodate current linux
# toolchain
#
# SanDiego_Brutus_Skin/9   5/16/06 10:17a tokushig
# Merge from main
#
# SanDiego_Brutus_Skin/8   5/10/06 2:41p tokushig
# fix for libexif build
#
# SanDiego_Brutus_Skin/7   4/27/06 10:53a tokushig
# added exif lib to build
#
# 12   4/17/06 7:47p jgarrett
# PR 20951: Adding EMULATED_FLOAT_SUPPORT option
#
# 11   4/17/06 5:08p jgarrett
# PR 20951: Merging skinned UI into mainline
#
# SanDiego_Brutus_Skin/6   4/14/06 8:00p tokushig
# disabled floating point support for 97400 (FPU currently broken)
#
# SanDiego_Brutus_Skin/5   4/14/06 3:17p tokushig
# disable floating point usage for png decoding on 97401
# remove DECOMPRESS_ONCE flag - integer based png decoding is fast enough
#
# SanDiego_Brutus_Skin/4   4/14/06 2:33p tokushig
# for 97401 set flag to DECOMPRESS_ONCE for fast png decoding
#
# SanDiego_Brutus_Skin/3   3/21/06 10:27a tokushig
# removed -O2 and add -g compilation flags for debugger support
#
# SanDiego_Brutus_Skin/2   2/28/06 2:03p tokushig
# remove -DDECOMPRESS_ONCE due to memory constraints on 97398 - only
# useful on the 97400 b/c no fpu
#
# SanDiego_Brutus_Skin/1   2/21/06 4:54p tokushig
# forced decompression only once for png files.  this will speed up skin
# UI loading/navigation considerably as well as allow us to avoid using
# pixmaps for the background images.
#
# PR19909/1   7/6/06 10:51a jgarrett
# PR 19909: Reducing brutus build output
#
# 10   1/18/06 11:59a rjlewis
# PR19044: changes for vxworks 6.
#
# 9   12/7/05 5:10p hkuhnert
# PR15072: Change the copy and removal of jconfig.h to allow for Linux
# host builds.
#
# 8   11/2/05 1:49p vsilyaev
# PR 15577:  Fixed sequence of jpeg configure and bwin build
#
# 7   8/4/05 4:44p erickson
# PR15139: consolidated system-specific build configuration into
# BSEAV/build/refsw_inc.mak
#
# 6   5/5/05 2:30p erickson
# PR15159: added double quotes for DOS builds
#
# 5   4/27/05 4:29p erickson
# PR9497: DOS mkdir doesn't do multiple subdirs (inside makefile at
# least), so be more explicit about it
#
# 4   2/24/05 1:17p erickson
# PR14180: adapted for vxworks
#
# 3   2/17/05 1:52p erickson
# PR9497: make more vxworks-friendly
#
# 2   2/14/05 1:57p erickson
# PR13908: clean up warnings (ignored errors) in make clean of jpeg
#
# 1   2/7/05 9:02p dlwin
# Merge down for release 2005_REFSW_MERGETOMAIN:
#
# Irvine_BSEAVSW_Devel/33   2/7/05 4:34p erickson
# PR13908: remove rule to eliminate warnings in libjpeg
#
# Irvine_BSEAVSW_Devel/32   2/4/05 2:24p erickson
# PR13829: added ike's upscale algorithm for test only, defaults off
#
# Irvine_BSEAVSW_Devel/31   1/27/05 7:15p vsilyaev
# PR13639: Fixed compiling of JPEG library.
#
# Irvine_BSEAVSW_Devel/29   1/14/05 1:02p biyongc
# PR2807: BSE PR - update for VxWorks build
#
# Irvine_BSEAVSW_Devel/28   1/10/05 4:08p erickson
# PR13386: border stripes are an issue
#
# Irvine_BSEAVSW_Devel/27   1/6/05 10:38a erickson
# PR13761: fix freetype build option
#
# Irvine_BSEAVSW_Devel/26   1/5/05 4:26p erickson
# PR13639: added bmp support
#
# Irvine_BSEAVSW_Devel/25   1/5/05 2:59p erickson
# PR13639: added jpeg build
#
# Irvine_BSEAVSW_Devel/24   12/22/04 5:04p erickson
# PR13639: removed unneeded files
#
# Irvine_BSEAVSW_Devel/23   12/13/04 11:30a erickson
# PR13495: removed bwin_image_png_io to reduce layers of indirection
#
# Irvine_BSEAVSW_Devel/22   12/13/04 9:04a erickson
# PR13495: changed zlib directory name
#
# Irvine_BSEAVSW_Devel/21   12/9/04 9:37a erickson
# PR12610: allow png to be cleaned from bwin
#
# Irvine_BSEAVSW_Devel/20   12/9/04 9:31a erickson
# PR12610: added png build to bwin
#
# Irvine_BSEAVSW_Devel/19   12/8/04 11:23a erickson
# PR12610: some rearranging
#
# Irvine_BSEAVSW_Devel/18   5/12/04 10:28a erickson
# PR10967: use std header
############################################################

# bwin Makefile
SHELL=/bin/bash

include ../../../api/build/tools.mak
BSEAV = $(shell cd "../../.." && ${PWD})

.PHONY: check_environment static libpng libjpeg libexif freetype
all: check_environment libpng libjpeg libexif freetype static

OTHER_CLEANS += clean-libpng clean-libjpeg clean-libexif clean-freetype
.PHONY: clean-libpng clean-libjpeg clean-libexif clean-freetype
clean: clean-libpng clean-libjpeg clean-libexif clean-freetype

include ../include/bwin.mak
include $(BSEAV)/build/refsw_inc.mak

ifeq ($(VERBOSE),)
MAKEFLAGS += -s
endif

LIB = bwin

ifeq ($(SYSTEM),vxworks)
CFLAGS += -I$(WIND_BASE)/target/h
endif
CFLAGS += -W -Wall
CFLAGS += -I$(BWIN_DIR)/include -I$(BWIN_DIR)/src
CFLAGS += $(B_REFSW_CFLAGS) $(B_REFSW_GENERIC_MAGNUM_CFLAGS) $(B_REFSW_MAGNUM_INCLUDE_DIRS)

# Using floating point and math.h allows cpu-based circle
# drawing.
ifeq ($(findstring $(PLATFORM),97400 97401 97403 97118 97118RNG 97455 97456 97458),)
CFLAGS += -DFLOAT_SUPPORT
else
CFLAGS += -DEMULATED_FLOAT_SUPPORT
endif

vpath %.c $(BWIN_DIR)/src

SRCS = \
	bwin.c \
	bwin_draw.c \
	bwin_default_drawops.c \
	bwin_font.c \
	bwin_rect.c \
	bwin_image.c \
	bwin_transform.c \
	bwin_image_bmp.c

ifeq ($(PNG_SUPPORT),y)
CFLAGS += -DPNG_SUPPORT -I$(LIBPNG_DIR) -I$(ZLIB_DIR)/include
SRCS += \
	bwin_image_png.c \
	bwin_image_png_priv.c
libz:
ifeq ($(SYSTEM),linux)
	if ! $(MAKE) -q -C $(BSEAV)/lib/zlib; then \
		echo [Build..... zlib]; \
		$(MAKE) -C $(BSEAV)/lib/zlib; \
	fi
else
	echo [Build..... zlib]
	$(MAKE) -C $(BSEAV)/lib/zlib
endif

libpng: libz
ifeq ($(SYSTEM),linux)
	if ! $(MAKE) -q -C $(BSEAV)/lib/libpng; then \
		echo [Build..... libpng]; \
		$(MAKE) -C $(BSEAV)/lib/libpng; \
	fi
else
	echo [Build..... libpng]
	$(MAKE) -C $(BSEAV)/lib/libpng
endif
clean-libpng:
	${Q_}$(MAKE) -C $(BSEAV)/lib/libpng clean
	${Q_}$(MAKE) -C $(BSEAV)/lib/zlib clean
else
# do nothing
libpng:
clean-libpng:
endif

ifeq ($(JPEG_SUPPORT),y)
CFLAGS += -DJPEG_SUPPORT -I$(LIBJPEG_DIR)
SRCS += \
	bwin_image_jpeg.c


ifeq ($(SYSTEM),vxworks)
# Use pre-configured Makefile
libjpeg: $(LIBJPEG_DIR)/jconfig.h $(LIBJPEG_DIR)/libjpeg.a

$(LIBJPEG_DIR)/jconfig.h:
	$(CP) $(LIBJPEG_DIR)/$(B_REFSW_ARCH)/jconfig.h $(LIBJPEG_DIR)

$(LIBJPEG_DIR)/libjpeg.a: $(LIBJPEG_DIR)/jconfig.h
	${Q_}$(MAKE) -C $(LIBJPEG_DIR) -f $(B_REFSW_ARCH)/Makefile libjpeg.a

ifeq ($(vxWorksVersion), )
#DOS script to remove jconfig.h
clean-libjpeg:
	${Q_}$(MAKE) -C $(LIBJPEG_DIR) -f $(B_REFSW_ARCH)/Makefile clean
	if exist $(LIBJPEG_DIR)/jconfig.h $(RM) $(LIBJPEG_DIR)/jconfig.h
else
#This is vxworks 6 and vxworks for linux side
clean-libjpeg:
	${Q_}$(MAKE) -C $(LIBJPEG_DIR) -f $(B_REFSW_ARCH)/Makefile clean
	@if [ -e $(LIBJPEG_DIR)/jconfig.h ]; then \
		$(RM) -f $(LIBJPEG_DIR)/jconfig.h; \
	fi
endif
else
# There is no separate configure step for bwin, so handle it with make,
# but don't configure if Makefile already exists.
libjpeg: $(LIBJPEG_DIR)/libjpeg.a

# configure libjpeg if Makefile doesn't exist
.PHONY: config-libjpeg
config-libjpeg:
#speed of machine vs. granularity of creation/modification creates the need for a little sleep here
	@if [ ! -e $(LIBJPEG_DIR)/Makefile ]; then \
		echo [Config.... libjpeg]; \
		cd $(LIBJPEG_DIR); RANLIB=$(RANLIB) AR="$(AR) rc" CC=$(CC) sh ./configure 1> /dev/null; \
		sleep 1; \
	fi

$(LIBJPEG_DIR)/libjpeg.a: config-libjpeg
	@if ! $(MAKE) -q -C $(LIBJPEG_DIR) libjpeg.a; then \
		echo [Build..... libjpeg]; \
		$(MAKE) -C $(LIBJPEG_DIR) libjpeg.a; \
	fi

clean-libjpeg:
	@if [ -e $(LIBJPEG_DIR)/Makefile ]; then \
		$(MAKE) -C $(LIBJPEG_DIR) distclean; \
	fi
endif
else
# do nothing
libjpeg:
clean-libjpeg:
endif

ifeq ($(EXIF_SUPPORT),y)
CFLAGS += -DEXIF_SUPPORT -I$(LIBEXIF_DIR) -I$(LIBEXIF_DIR)/libexif

ifeq ($(VERBOSE),)
REDIR_LIBEXIF := > /dev/null
else
REDIR_LIBEXIF :=
endif

# libexif doesn't support mips-uclibc, so convert
ifeq ($(B_REFSW_ARCH),)
LIBEXIF_ARCH=mipsel-linux
else
LIBEXIF_ARCH=$(B_REFSW_ARCH)
endif

# configure libexif if Makefile doesn't exist
.PHONY: config-libexif
config-libexif:
	#speed of machine vs. granularity of creation/modification creates the need for a little sleep after configure
	@if [ ! -e $(LIBEXIF_DIR)/Makefile ]; then \
		cd $(LIBEXIF_DIR); \
		$(CP) -f libexif/_stdint.h.orig libexif/_stdint.h; \
		chmod 777 libexif/_stdint.h; \
		$(CP) -f aclocal.m4.orig aclocal.m4; \
		chmod 777 aclocal.m4; \
		$(CP) -f config.h.in.orig config.h.in; \
		chmod 777 config.h.in; \
		$(CP) -f configure.orig configure; \
		chmod 777 configure; \
		$(CP) -f Makefile.in.orig Makefile.in; \
		chmod 777 Makefile.in; \
		$(CP) -f libexif.spec.orig libexif.spec; \
		chmod 777 libexif.spec; \
		EXIF_PREFIX=($(addsuffix $(LIBEXIF_ARCH),$(dirname $(dirname`which $(CC)`)))); \
		echo [Config.... libexif]; \
		./configure --prefix=$(EXIF_PREFIX) --exec-prefix=$(EXIF_PREFIX) \
			--host=$(LIBEXIF_ARCH) \
			--enable-static=yes --enable-shared=no $(REDIR_LIBEXIF); \
		sleep 1; \
	fi

libexif: $(LIBEXIF_DIR)/.libs/libexif.a

$(LIBEXIF_DIR)/.libs/libexif.a: config-libexif
	@if ! $(MAKE) -q -C $(LIBEXIF_DIR) >/dev/null 2>&1; then \
		echo [Build..... libexif]; \
		$(MAKE) -C $(LIBEXIF_DIR) $(REDIR_LIBEXIF); \
	fi

clean-libexif:
	-@if [ -e $(LIBEXIF_DIR)/Makefile ]; then \
		$(MAKE) -C $(LIBEXIF_DIR) distclean >/dev/null 2>&1; \
	fi
else
# do nothing
libexif:
clean-libexif:
endif

ifeq ($(VERBOSE),)
REDIR_LIBBTE := > /dev/null
else
REDIR_LIBBTE :=
endif

ifeq ($(FREETYPE_SUPPORT),y)
CFLAGS += -DFREETYPE_SUPPORT -I$(FREETYPE_DIR)/include
# We've checked in preconfigured source which uses our standard tools.mak
# for cross compiling. configure isn't going to work on DOS for vxworks.
freetype:
ifeq ($(SYSTEM),linux)
	@if ! $(MAKE) -q -C $(FREETYPE_DIR); then \
		echo [Build..... freetype]; \
		$(MAKE) -C $(FREETYPE_DIR); \
	fi
else
	echo [Build..... freetype];
	$(MAKE) -C $(FREETYPE_DIR)
endif
clean-freetype:
	${Q_}$(MAKE) -C $(FREETYPE_DIR) clean
else
# do nothing
freetype:
clean-freetype:
endif

ODIR = $(BWIN_DIR)/lib/$(B_REFSW_ARCH).$(DEBUG_SUFFIX)

check_environment: $(BWIN_DIR)/lib $(ODIR)

$(BWIN_DIR)/lib:
	${Q_}$(MKDIR) "$@"

include ../../../api/build/rules.mak

# required for parallel make (make -jN)
$(ODIR)/bwin_image_jpeg.o : config-libjpeg
$(ODIR)/bwin_image.o : config-libexif

