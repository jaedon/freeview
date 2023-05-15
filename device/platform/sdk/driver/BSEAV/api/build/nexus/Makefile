############################################################
#	  Copyright (c) 2007-2012, Broadcom Corporation
#	  All Rights Reserved
#	  Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: Makefile $
# $brcm_Revision: 89 $
# $brcm_Date: 6/20/12 3:09p $
#
# Module Description:
#
# Revision History:
#
# Created: 02/09/2001 by Marcus Kellerman
#
# $brcm_Log: /BSEAV/api/build/nexus/Makefile $
# 
# 89   6/20/12 3:09p erickson
# SW7405-641: if you set brutus env variable for macrovision, have
# nexus/magnum follow
# 
# 88   6/6/12 12:42p tokushig
# SW7231-846: update brutus/settopapi makefile to build new openssl v1.0
# shared libs and copy them for inclusion in the brutus make install
# tarball.
# 
# 87   5/2/12 6:46p jgarrett
# SW7425-2802: Removing RAP_XXX build variables for audio post processing
# in favor of runtime detection
# 
# 86   4/27/12 1:24p jgarrett
# SW7358-272: Removing checks for adding both AVL/TVOL/DolbyVol at the
# same time
# 
# 85   4/26/12 9:24p sgadara
# SW7358-272: [7358,7420,7405] Add check as only one of the audio volume
# algorithms SRS Truvolume or Dolby Volume or AVL is supported a time
# 
# 84   4/25/12 11:48a sgadara
# SW7358-272: [7358] Support AVL on Brutus
# 
# 83   1/12/12 11:53a ssood
# SW7425-2151: build playback_ip apps w/ the install option which will
# copy the libraries needed by ip_streamer into BSEAV/bin directory
# 
# 82   12/21/11 3:47p mward
# SW7435-7:  Support Brutus build on (new, 7435-based) platform 97435.
# 
# 81   9/23/11 3:37p katrep
# SW7429-1:7429 support
# 
# 80   5/18/11 1:07p erickson
# SW7420-1264: protect if NEXUS_BIN_DIR and INSTALL_DIR are the same
# 
# 78   4/15/11 6:20a jgarrett
# SW7425-299: Refactoring audio build options for 40nm
# 
# 77   3/18/11 11:43a ssood
# SW7408-255: build ip library using the makefile of its apps directory
# (this picks up all the default build variables for ip)
# 
# 76   2/15/11 7:14p xhuang
# SW7358-34: Add 7358/7552 support
# 
# 75   12/27/10 3:37p erickson
# SW7400-2511: fix STATIC_BRUTUS=y builds
#
# 74   12/16/10 11:18a katrep
# SW7231-4:add 7231 support
#
# 73   12/15/10 10:09a jrubio
# SW7344-9: add 7344/7346 support
#
# 72   10/20/10 3:33p nickh
# SW7422-80: Add tuner support and dolby and srs tru volume support for
# 7422
#
# 71   8/6/10 3:27p calvinho
# SW7405-4431: Adding nexus to playback_ip_lib dependency list in order
# to resolve Brutus parallel compile error
#
# 70   7/30/10 9:06a erickson
# SW7420-918: B_REFSW_SHAREABLE=n should build only static settop api
#
# 69   5/7/10 5:16p jgarrett
# SW7405-4302: Removing warnings on 3rd-party libraries
#
# 68   4/28/10 3:50p mward
# SW7125-226: In RAP, RAP_DTSENC_SUPPORT defaults n.  Therefore, we will
# use RAP_DTSENC_SUPPORT=y here to set CFLAGS += -DB_HAS_DTS_ENCODE=1,
# not a list of chips that may (or may not) have RAP_DTSENC_SUPPORT
# enabled.
#
# 67   4/19/10 4:04p nickh
# SW7420-709: Add tuner support for 7420DBS
#
# 66   4/16/10 5:42p nitinb
# SW7550-382: Enabling DTS encode for 7550
#
# 65   4/9/10 1:07p jgarrett
# SW7405-4034: Moving MSDRM PD/ND binaries into nexus as required
#
# 64   4/8/10 4:32p mward
# SW7125-226: Corrected logic to separate volume processing options and
# DTS encode.
#
# DrmMakefileUpdates/1   4/8/10 4:49p jgarrett
# SW7405-4034: Including binaries for MSDRM (PD/ND) in the nexus builds
# to avoid link issues
#
# 63   3/22/10 10:27a mward
# SW7125-226: Enabling AC3 Encode for 7125, 7340, 7342 (Only AC3 not DTS
# encode).
#
# 62   3/19/10 10:39a ssood
# SW7420-561: copy psip library in BSEAV/bin upon install
#
# 61   3/16/10 12:45p ssood
# SW7420-561: suppress the extra prints for default applibs build
#
# 60   3/15/10 7:24p randyjew
# SW7468-6:Enable DTS Encode for 7468
#
# 59   2/18/10 7:16p jrubio
# SW7342-159: Clean Playback IP apps, Pass in flag to playback_ip
#
# 58   2/3/10 3:26p rjain
# SW7550-234: Enabling DTS for 7550
#
# 57   1/28/10 5:16p ssood
# SW7420-454: build & install ip_streamer & ip_client test apps part of
# the brutus build
#
# 56   1/18/10 5:20p rjain
# SW7550-190: Enabling AC3 encode for 7550
#
# 55   1/5/10 10:22a jrubio
# SW7342-11: add SRSTRU and DOLBY support for 73XX and 7125
#
# 54   12/11/09 4:30p mphillip
# SW7550-112: Merge 7550 changes to main branch
#
# 53   12/9/09 12:04p jgarrett
# SW7408-17: Fixing shim layer build for soft audio
#
# 52   11/17/09 2:15p erickson
# SW7405-3408: use $(filter) instead of $(findstring)
#
# 51   9/18/09 11:56a erickson
# SW7400-2511: add B_REFSW_ or NEXUS_ prefix to build variables
#
# 50   8/25/09 11:24a katrep
# SW7405-2934:add suppoort for audio capture to a file
#
# 49   7/27/09 5:21p katrep
# PR48700: added SHAREABLE=n to build nexus static library
#
# 48   7/13/09 3:30p jtna
# PR56505: libsettop depends on nexus
#
# 47   6/25/09 1:03a ssood
# PR53773: More changes for DTCP/IP + SSL Support
#
# 46   6/24/09 5:55p ssood
# PR53773: Adding support for compiling DTCP/IP + SSL libraries
#
# 45   6/1/09 3:13p mward
# PR 50613: Add SRS VIQ for 7400 under Nexus.
#
# 44   5/22/09 11:55a nickh
# PR53563: Add 7420 support for AACHE to AC3 transcode, AACHE to DTS
# transcode, SRS Tru volume.  Also see PR's 53564, 53565.
#
# 43   4/30/09 7:14p katrep
# PR43491: fixed issue with 97405 656/i2s input
#
# 42   3/31/09 7:31p katrep
# PR53733: MSDRM_PD_HWDECRYPT support,moved msdrm pd compile to settop
# api from nexus
#
# 41   3/20/09 1:09p katrep
# PR52340: Support srs true volume and dolby volume through brutus ans
# settop api
#
# 40   3/13/09 2:49p vsilyaev
# PR 53225: Adeed NEXUS_BIN_DIR_SUFFIX and NEXUS_BIN_DIR
#
# 39   3/4/09 7:52p jgarrett
# PR 52269: Adding XMP support
#
# 38   3/2/09 6:10p katrep
# PR52340: Add support srs_true_volume
#
# 37   1/15/09 3:05p prasadv
# PR50240: Adding Cablecard support
#
# 36   1/8/09 12:21p katrep
# PR50063: Add suppport dor Ac3 encode in Settop shim and burtus.
#
# 35   12/24/08 11:57a jgarrett
# PR 50703: Changes for 7420 bringup
#
# 34   9/17/08 6:20p katrep
# PR47061: Add support for DTS encode
#
# 33   8/14/08 11:46a jgarrett
# PR 32047: Adding PCM capture
#
# 32   8/6/08 1:48p mphillip
# PR40027: Build crypto
#
# 31   8/5/08 10:24a erickson
# PR45422: add DIVX_DRM_SUPPORT
#
# 30   7/24/08 1:23p ssood
# PR42739: moving the compilation of rtp library part of the ip applib
#
# 29   7/9/08 4:10p katrep
# PR43490: Add support 656/i2s tuning using analog tune api.
#
# 28   7/2/08 5:00p vishk
# PR 40020: bsettop_stream (crypto, network decryption): Develop
# SettopAPI-to-Nexus shim layer
#
# 27   6/26/08 6:21p mphillip
# PR42901: Encrypted PVR playback support
#
# 26   6/26/08 3:44p shyam
# PR43426 : Enable stub functions
#
# 25   6/23/08 5:41p shyam
# PR43426 : Add 7043 Transcode support
#
# 24   6/20/08 3:13p katrep
# PR42141: Single compile time MACROVISION_SUPPORT=y and DCS_SUPPORT=y is
# required to build brutus with Macrovison support.
#
# 23   6/12/08 9:07p katrep
# PR43636: Add support for MSDRM
#
# 22   6/12/08 4:42p erickson
# PR42739: remove rtp for non-IP builds
#
# 21   6/11/08 10:26p ssood
# PR42739: modifications to build IP Playback Applib and Settop API Shim
# layer for IP Playback
#
# 20   6/11/08 11:53a jgarrett
# PR 43489: Adding brutus support for 93549/93556
#
# 19   6/3/08 6:53p rjlewis
# PR40352: don't link against libraries for Vxworks.
#
# 18   5/21/08 11:49a rjlewis
# PR40352: Added support for VxWorks.
#
# 17   5/14/08 7:25p jrubio
# PR42694: add TUNER_ENABLE_7335 flag as a workaround for streamer/tuner
# not working together
#
# 16   4/17/08 5:29p katrep
# PR40803: Use HDMI_DDP_PASSTHROUGH=1 compile time flag to enable DDP
# passthrough on HDMI.
#
# 15   4/7/08 4:11p erickson
# PR40024: impl bdecode_read_metadata
#
# 14   4/4/08 10:43a jgarrett
# PR 41312: Synchronizing PVR
#
# 13   3/31/08 6:04p jgarrett
# PR 40606: Adding OS lib
#
# 12   3/25/08 10:13a jgarrett
# PR 40857: Adding thunks
#
# 11   3/11/08 11:08a erickson
# PR36068: added bsettop_timers
#
# 10   3/6/08 4:10p erickson
# PR36068: verify that MEDIA_ASF_SUPPORT and MEDIA_AVI_SUPPORT are used,
# not the Settop API variants
#
# 9   2/8/08 9:56a vsilyaev
# PR 38682: Added support for Nexus proxy mode
#
# 8   1/22/08 10:10a erickson
# PR36068: update NEXUS_TOP
#
# 7   1/3/08 10:40a erickson
# PR36068: map legacy env variables to Nexus
#
# 6   1/2/08 9:55a erickson
# PR36068: add clean
#
# 5   11/12/07 3:57p erickson
# PR36068: update
#
# 4   11/12/07 2:34p erickson
# PR36068: added fileio
#
# 3   10/16/07 2:27p erickson
# PR36068: build nexus too
#
# 2   10/16/07 12:37p erickson
# PR36068: updates
#
# 1   10/15/07 2:37p erickson
# PR36068: intial
#
############################################################

ifdef COMSPEC
BSEAV=$(shell cd ../../.. && cd)
else
BSEAV=$(shell cd ../../../; pwd)
endif
BSETTOP=$(BSEAV)/api

# backward compatibility
B_REFSW_OS ?= $(OS)
B_REFSW_SHAREABLE ?= $(SHAREABLE)

ifeq ($(B_REFSW_OS),)
B_REFSW_OS=linuxuser
else
ifeq ($(B_REFSW_OS),linuxkernel)
$(error Can not build linuxkernel for a nexus environment)
endif
endif

ifeq ($(filter y,$(STATIC_SETTOPAPI) $(STATIC_BRUTUS)),y)
B_REFSW_SHAREABLE=n
export B_REFSW_SHAREABLE
endif

ifeq ($(B_REFSW_SHAREABLE),n)
# api.mak uses SHAREDLIB
SHAREDLIB=no
BUILD_RULE = static
else
BUILD_RULE = shared
endif

NEXUS_TOP=$(BSETTOP)/../../nexus
B_LIB_TOP := $(NEXUS_TOP)/lib

all: check_environment $(BSETTOP)/build/nexus/bsettop_autogen_thunks.c nexus os_lib ssl_lib dtcp_ip_lib playback_ip_lib softgfx_lib $(BUILD_RULE)

ifeq ($(MSDRM_PD_SUPPORT), y)
ifeq ($(MSDRM_PD_HWDECRYPT),y)
NEXUS_MAKE_VARIABLES+=KEYLADDER_SUPPORT=y
KEYLADDER_SUPPORT=y
export KEYLADDER_SUPPORT
BHSM_KEYLADDER=ON
export BHSM_KEYLADDER
CFLAGS +=-DMSDRM_PD_HWDECRYPT=1
endif
endif



ifeq ($(B_HAS_EXTERNAL_ANALOG),y)
NEXUS_CCIR656_INPUT=y
export NEXUS_CCIR656_INPUT
endif


include $(BSETTOP)/build/tools.mak
include $(BSETTOP)/include/api.mak
include $(NEXUS_TOP)/platforms/$(PLATFORM)/build/platform_app.inc
include $(NEXUS_TOP)/lib/os/b_os_lib.inc

ifeq ($(PLAYBACK_IP_SUPPORT),y)
include $(NEXUS_TOP)/lib/playback_ip/b_playback_ip_lib.inc
ifeq ($(DTCP_IP_SUPPORT),y)
include $(NEXUS_TOP)/lib/dtcp_ip/dtcp_ip_lib.inc
endif
endif

ifeq ($(SOFT_GFX_SUPPORT),y)
include $(NEXUS_TOP)/lib/softgfx/b_softgfx_lib.inc
endif #SOFT_GFX_SUPPORT

ODIR=$(BSETTOP_BIN)

SRCS += \
	bsettop.c \
	bsettop_autogen_thunks.c \
	bsettop_os.c \
	bsettop_cablecard.c \
	bsettop_cipher.c \
	bsettop_config.c \
	bsettop_crypto.c \
	bsettop_decode.c \
	bsettop_display.c \
	bsettop_encode.c \
	bsettop_fileio.c \
	bsettop_fileio_fifo.c \
	bsettop_graphics.c \
	bsettop_keyslot_priv.c \
	bsettop_message.c \
	bsettop_pcm_playback.c \
	bsettop_pcm_capture.c \
	bsettop_playpump.c \
	bsettop_pvr.c \
	bsettop_pvr_util.c \
	bsettop_recpump.c \
	bsettop_smartcard.c \
	bsettop_stream_encryption.c \
	bsettop_stream.c \
	bsettop_tuner.c \
	bsettop_user_io.c \
	bsettop_vbi.c \
	bsettop_transcode.c\
        bsettop_audio_capture.c

ifeq ($(XMP1_SUPPORT),y)
CFLAGS += -DXMP1_SUPPORT
endif

ifeq ($(XMP2_SUPPORT),y)
CFLAGS += -DXMP2_SUPPORT
SRCS += bsettop_xmp.c
endif


CFLAGS += \
	-I$(BSETTOP)/include

#Compile Settop API's Playback IP Shim: compiles stubs if PLAYBACK_IP_SUPPORT is not defined
CFLAGS += \
	-I$(BSEAV)/lib/netaccel/include
SRCS += \
	bsettop_playback_ip.c

ifeq ($(PLAYBACK_IP_SUPPORT),y)
CFLAGS += -DB_HAS_IP=1
endif

ifeq ($(SOFT_GFX_SUPPORT),y)
CFLAGS += -DB_HAS_SOFT_GFX=1
CFLAGS += -I$(NEXUS_TOP)/lib/softgfx
endif #SOFT_GFX_SUPPORT

ifeq ($(B_REFSW_OS),vxworks)
CFLAGS += -I$(BSETTOP)/include/vxworks
else
CFLAGS += -I$(BSETTOP)/include/linux
endif

CFLAGS += \
	-I$(BSETTOP)/build/nexus \
	-I$(BSETTOP)/src/nexus \
	-I$(BSETTOP)/src/nexus/board \
	-I$(BSETTOP)/../lib/bfile \
	-I$(BSETTOP)/../lib/utils \
	-I$(BSETTOP)/../lib/rtp \
	$(NEXUS_CFLAGS) $(addprefix -I,$(NEXUS_APP_INCLUDE_PATHS)) $(addprefix -D,$(NEXUS_APP_DEFINES)) \
	$(addprefix -I,$(B_OS_LIB_PUBLIC_INCLUDES)) $(addprefix -D,$(B_OS_LIB_DEFINES)) \
	$(addprefix -I,$(B_PLAYBACK_IP_LIB_PUBLIC_INCLUDES)) $(addprefix -D,$(B_PLAYBACK_IP_LIB_DEFINES)) \
	$(addprefix -I,$(B_SOFTGFX_LIB_PUBLIC_INCLUDES)) $(addprefix -D,$(B_SOFTGFX_LIB_DEFINES)) \
	-DBCM_BOARD=$(BCM_BOARD_NO) \
	-g
CFLAGS += -DBCM_BOARD_STR=\"$(BCM_BOARD_NO)\"
CFLAGS += -DB_SYSTEM_$(SYSTEM)


ifeq ($(B_DECODE_CRC_CAPTURE),y)
CFLAGS += -DB_DECODE_CRC_CAPTURE=1
NEXUS_MAKE_VARIABLES+="NEXUS_CRC_CAPTURE=y"
endif

ifeq ($(BVDC_MACROVISION),y)
MACROVISION_SUPPORT=y
endif
ifeq ($(MACROVISION_SUPPORT),y)
CFLAGS += -DMACROVISION_SUPPORT=1
NEXUS_MAKE_VARIABLES+="BVDC_MACROVISION=y"
endif

ifeq ($(DCS_SUPPORT),y)
CFLAGS += -DDCS_SUPPORT=1
NEXUS_MAKE_VARIABLES+="BVDC_DCS=y"
endif

ifeq ($(B_HAS_EXTERNAL_ANALOG),y)
CFLAGS += -DB_HAS_EXTERNAL_ANALOG=1
#following is require for settop api include match with nexus includes
#ifeq ($(BCHP_CHIP),7405)
#CFLAGS += -DNEXUS_NUM_656_INPUTS=1 -DNEXUS_NUM_I2S_OUTPUTS=0
#endif
endif

ifeq ($(TUNER_ENABLE_7335),y)
CFLAGS += -DTUNER_ENABLE_7335=1
endif

ifeq ($(TUNER_ENABLE_7420),y)
CFLAGS += -DTUNER_ENABLE_7420=1
endif

ifeq ($(TUNER_ENABLE_7422),y)
CFLAGS += -DTUNER_ENABLE_7422=1
endif

ifeq ($(MSDRM_PD_SUPPORT), y)
CFLAGS += -DB_HAS_MSDRM_PD=1
endif

ifeq ($(MSDRM_ND_SUPPORT), y)
CFLAGS += -DB_HAS_MSDRM_ND=1
endif

ifeq ($(DIVX_DRM_SUPPORT), y)
CFLAGS += -DB_HAS_DIVX_DRM=1
endif

# Add 7043 Trasncode support
ifeq ($(BENC_CHIP),7043)
CFLAGS += -DB_HAS_TRANSCODE=1
endif

ifeq ($(CABLECARD_SUPPORT), MPOD)
CFLAGS += -DMPOD_SUPPORT
endif

ifeq ($(B_REFSW_OS),vxworks)
#LDFLAGS += -lnexus -lb_os -L$(NEXUS_BIN_DIR)
LDFLAGS += -L${NEXUS_BIN_DIR}
else
LDFLAGS +=  -lnexus -lb_os -L${NEXUS_BIN_DIR} -lpthread
endif

LIB=settop

${ODIR}/lib$(LIB).$(BLIB_EXT): nexus

vpath %.c $(BSETTOP)/src/nexus
vpath %.c $(BSETTOP)/src/nexus/board
vpath %.c $(BSETTOP)/build/nexus

ifneq ($(ASF_SUPPORT),)
ifeq ($(MEDIA_ASF_SUPPORT),)
$(error You must define MEDIA_ASF_SUPPORT, not ASF_SUPPORT.)
endif
endif
ifneq ($(AVI_SUPPORT),)
ifeq ($(MEDIA_AVI_SUPPORT),)
$(error You must define MEDIA_AVI_SUPPORT, not AVI_SUPPORT.)
endif
endif

nexus:
	${Q_}$(MAKE) $(NEXUS_MAKE_VARIABLES) -C $(NEXUS_TOP)/build

OTHER_CLEANS += nexus-clean thunk-clean os_lib_clean playback_ip_lib_clean dtcp_ip_lib_clean ssl_lib_clean softgfx_lib_clean

nexus-clean:
	$(Q_)make -C $(NEXUS_TOP)/build clean

include $(BSETTOP)/build/rules.mak

install:
	@echo "[Install... libs]"
ifneq ($(INSTALL_DIR),$(NEXUS_BIN_DIR))
	${Q_}$(CP) $(BSETTOP_LIBS) $(INSTALL_DIR)
ifeq ($(PLAYBACK_IP_SUPPORT),y)
	${Q_}$(CP) $(NEXUS_BIN_DIR)/ip_* $(INSTALL_DIR)
endif
ifeq ($(AUTO_PSI_SUPPORT),y)
	${Q_}$(CP) $(NEXUS_BIN_DIR)/libb_psip.* $(INSTALL_DIR)
endif
ifeq ($(SSL_SUPPORT),y)
	$(CP) -fP $(NEXUS_BIN_DIR)/libcrypto.* $(INSTALL_DIR)
	$(CP) -fP $(NEXUS_BIN_DIR)/libssl.* $(INSTALL_DIR)
endif
endif

$(BSETTOP)/build/nexus/bsettop_autogen_thunks.c: $(BSEAV)/api/include/bsettop_display.h $(BSEAV)/api/include/bsettop_message.h $(BSEAV)/api/include/bsettop_recpump.h $(BSEAV)/api/include/bsettop_types.h $(BSEAV)/api/include/bsettop_cipher.h $(BSEAV)/api/include/bsettop_encode.h $(BSEAV)/api/include/bsettop_pcm.h $(BSEAV)/api/include/bsettop_smartcard.h $(BSEAV)/api/include/bsettop_user_io.h $(BSEAV)/api/include/bsettop_config.h $(BSEAV)/api/include/bsettop_graphics.h $(BSEAV)/api/include/bsettop_playpump.h $(BSEAV)/api/include/bsettop_stream.h $(BSEAV)/api/include/bsettop_vbi.h $(BSEAV)/api/include/bsettop_decode.h $(BSEAV)/api/include/bsettop.h $(BSEAV)/api/include/bsettop_tuner.h $(BSEAV)/api/include/bsettop_mem.h $(BSEAV)/api/include/bsettop_cablecard.h $(BSEAV)/api/include/bsettop_pvr.h
	@echo "[Thunk... bsettop]"
	@$(PERL) $(BSEAV)/api/build/nexus/bapi_build.pl $^

$(BSETTOP)/build/nexus/bsettop_autogen_thunks.h: $(BSEAV)/api/build/nexus/bsettop_autogen_thunks.c

thunk-clean:
	@$(RM) $(BSETTOP)/build/nexus/bsettop_autogen_thunks.*

os_lib:
	${Q_}$(MAKE) -C $(NEXUS_TOP)/lib/os

os_lib_clean:
	${Q_}$(MAKE) -C $(NEXUS_TOP)/lib/os clean

ifeq ($(PLAYBACK_IP_SUPPORT),y)
playback_ip_lib: nexus
	${Q_}$(MAKE) REFSW_BUILD=y -C $(NEXUS_TOP)/lib/playback_ip/apps install

playback_ip_lib_clean:
	${Q_}$(MAKE) -C $(NEXUS_TOP)/lib/playback_ip/apps clean

else #PLAYBACK_IP_SUPPORT is not defined, so define empty targets
playback_ip_lib:
#intentionally left empty

playback_ip_lib_clean:
#intentionally left empty
endif # PLAYBACK_IP_SUPPORT

ifeq ($(DTCP_IP_SUPPORT),y)
dtcp_ip_lib:
	${Q_}$(MAKE) -C $(NEXUS_TOP)/lib/dtcp_ip

dtcp_ip_lib_clean:
	${Q_}$(MAKE) -C $(NEXUS_TOP)/lib/dtcp_ip clean
else #DTCP_IP_SUPPORT is not defined, so define empty targets
dtcp_ip_lib:
#intentionally left empty

dtcp_ip_lib_clean:
#intentionally left empty
endif # DTCP_IP_SUPPORT

ifeq ($(SSL_SUPPORT),y)
ssl_lib:
	${Q_}if [ ! -d "$(BSEAV)/lib/openssl/$(PLATFORM)" ]; then \
		echo "$(BSEAV)/lib/openssl/$(PLATFORM) not found, calling make in $(BSEAV)/lib/openssl/"; \
		make -C $(BSEAV)/lib/openssl; \
	fi

	${Q_}$(CP) -fP $(BSEAV)/lib/openssl/$(PLATFORM)/lib/libcrypto.so* $(NEXUS_BIN_DIR)
	${Q_}$(CP) -fP $(BSEAV)/lib/openssl/$(PLATFORM)/lib/libssl.so* $(NEXUS_BIN_DIR)

ssl_lib_clean:
	${Q_}$(MAKE) -C $(BSEAV)/lib/openssl clean
else #SSL_SUPPORT is not defined, so define empty targets
ssl_lib:

ssl_lib_clean:
endif # SSL_SUPPORT

ifeq ($(SOFT_GFX_SUPPORT),y)
softgfx_lib:
	${Q_}$(MAKE) -C $(NEXUS_TOP)/lib/softgfx

softgfx_lib_clean:
	${Q_}$(MAKE) -C $(NEXUS_TOP)/lib/softgfx clean

else #SOFT_GFX_SUPPORT is not defined, so define empty targets
softgfx_lib:
#intentionally left empty

softgfx_lib_clean:
#intentionally left empty

endif # SOFT_GFX_SUPPORT

-include $(ODIR)/*.d
