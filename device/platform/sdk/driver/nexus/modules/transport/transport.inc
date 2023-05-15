############################################################
#     (c)2007-2012 Broadcom Corporation
#
#  This program is the proprietary software of Broadcom Corporation and/or its licensors,
#  and may only be used, duplicated, modified or distributed pursuant to the terms and
#  conditions of a separate, written license agreement executed between you and Broadcom
#  (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
#  no license (express or implied), right to use, or waiver of any kind with respect to the
#  Software, and Broadcom expressly reserves all rights in and to the Software and all
#  intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
#  HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
#  NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
#
#  Except as expressly set forth in the Authorized License,
#
#  1.     This program, including its structure, sequence and organization, constitutes the valuable trade
#  secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
#  and to use this information only in connection with your use of Broadcom integrated circuit products.
#
#  2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
#  AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
#  WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
#  THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
#  OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
#  LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
#  OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
#  USE OR PERFORMANCE OF THE SOFTWARE.
#
#  3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
#  LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
#  EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
#  USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
#  THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
#  ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
#  LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
#  ANY LIMITED REMEDY.
#
# $brcm_Workfile: transport.inc $
# $brcm_Revision: 80 $
# $brcm_Date: 10/15/12 12:01p $
#
# Module Description:
#
# Revision History:
#
# $brcm_Log: /nexus/modules/transport/transport.inc $
# 
# 80   10/15/12 12:01p erickson
# SW7435-375: transport module should not call into security module
# 
# 79   8/27/12 4:14p erickson
# SW7401-4517: remove options for old code
# 
# 78   8/22/12 12:04p erickson
# SW7425-3785: add macro which tells XPT PI to not automatically enable
#  pid channels when starting msg filters. it's not needed; and it
#  prevents users from starting msg filter on disabled pid channel
#  without automatically enabling it.
# 
# 77   7/27/12 2:14p jgarrett
# SW7425-3281: Merge to main branch
# 
# 76   7/16/12 6:24p piyushg
# SW7231-904: Remove support for playready from current Brutus.
# 
# 75   7/16/12 1:20p erickson
# SW7435-235: refactor directories to remove symbolic links
# 
# SW7425-3281/1   6/20/12 5:16p piyushg
# SW7425-3281: Remove playready precompiled objects from transport.inc
# 
# 74   6/5/12 1:27p erickson
# SW7425-3169: add libcmncrypto.a
# 
# 73   5/17/12 3:21p cdisc
# SW7125-1289: adding NEXUS prefix to common crypto variable
# 
# 72   5/15/12 9:33a cdisc
# SW7125-1289: adding Common Crypto support
# 
# 71   11/2/11 11:19a vsilyaev
# SW7425-1654: Merged decryption for stream of MP4 Fragments
# 
# SW7425-1654/1   11/2/11 1:50p hugost
# Added playready files to NEXUS_TRANSPORT_SOURCES. Force sw decryption
#  when robustiness rules are not enabled. And use libdrmrootfs when
#  robustness rules are enabled.
# 
# 70   10/5/11 1:31p jtna
# SW7425-1189: fix non-7425 platforms
# 
# 69   10/5/11 12:23p jtna
# SW7425-1189: add transport and frontend TSMF support
# 
# 68   10/4/11 11:29a piyushg
# SW7425-1395: Fix compilation error for non robust playready build
# 
# 67   10/3/11 6:00p piyushg
# SW7425-1395: Add support in brutus for playready with robustness
#  PRDY_ROBUSTNESS_ENABLE.
# 
# 66   10/1/11 4:39p piyushg
# SW7425-1376: Modified playready to use common_crypto and drmrootfs
#  libraries for Robusteness related security interface
# 
# 65   9/27/11 7:34p piyushg
# SW7425-1358: Make playready compliant with robustness rules and
#  maintain one branch for playready with robustness and without. Use
#  PRDY_ROBUSTNESS_ENABLE=y along with MSDRM_PRDY_SUPPORT=y to have
#  playready with robustness.
# 
# 64   9/22/11 1:37p jtna
# SW7425-1333: use MEDIA_RMFF_SUPPORT flag
# 
# 63   8/4/11 7:39p mphillip
# SW7422-404: Update region verification code
# 
# 62   6/7/11 6:54p jhaberf
# SWDTV-7444: added support for 35xxx DTV chips
# 
# 61   6/6/11 4:08p jtna
# SW7425-644: add transport SAM support
# 
# 60   5/18/11 7:24p jgarrett
# SW7231-148: Adding RMFF and Cook support
# 
# 59   4/28/11 7:22p vsilyaev
# SW7425-232: Always compile file module
# 
# 58   2/22/11 7:53p vsilyaev
# SW7422-107: Added FLV mapping
# 
# 57   12/28/10 5:44p vsilyaev
# SW7425-39: Added Mux specific interfaces
# 
# 56   12/17/10 2:31p gmohile
# SW7405-5065 : Refactor to build divx drm from binaries
# 
# 55   12/15/10 12:03p erickson
# SW35230-2585: reduce DEPENDENCY list, depend on recursion
# 
# 54   12/7/10 7:36p piyushg
# SW7405-4386:Initial checkin for playready intigration with mainline
#  brutus.
# 
# 53   10/28/10 5:09p erickson
# SW7422-20: adapt to new XPT PI (backward compat using magnum and nexus
#  macros)
# 
# 52   10/26/10 6:21p nickh
# SW7422-73: Add 7422 support
# 
# 51   8/12/10 5:24p jgarrett
# SW7405-4034: Adding NEXUS_DRM_BUILD flag to enable NEXUS_Dma use in DRM
# 
# 50   7/20/10 6:26p jfisher
# SW7572-30:  Add modification for ucos_ii support.
# 
# 49   7/12/10 4:49p erickson
# SW7405-4561: add support for 32 byte filters using export
#  BXPT_FILTER_32=y
#
# 48   5/27/10 6:27p mward
# SW7400-2786:  Support DivX DRM under Nexus on 7400/7403.
#
# 47   5/18/10 11:16a erickson
# SW7550-424: add TS PSI parsing source if PVR_SUPPORT=n
#
# 46   4/9/10 1:07p jgarrett
# SW7405-4034: Moving MSDRM PD/ND binaries into nexus as required
#
# DrmMakefileUpdates/1   4/8/10 4:46p jgarrett
# SW7405-4034: Including binaries for MSDRM (PD/ND) in the nexus builds
#  to avoid link issues
#
# 45   2/26/10 4:37p gmohile
# SW7408-79 : Move subtitle parser functionality to app
#
# 44   12/15/09 4:52p gmohile
# SW7408-1 : Add 7408 support
#
# 43   12/11/09 3:32p erickson
# SW7550-112: merge 7550 code. add support for sw message filtering.
#  allow for no HW message filtering using standard XPT define.
#
# 42   11/19/09 4:25p gmohile
# SW7408-1 : Add 7408 support
#
# 41   10/8/09 1:23p erickson
# SWDEPRECATED-3717: default nexus_message_pid2buf.c for all silicon that
#  supports it
#
# 40   9/10/09 2:56p erickson
# SW7335-529: remove unneeded variable
#
# 39   9/9/09 4:58p erickson
# SW7405-2964: move HSM build from transport.inc to security.inc
#
# 38   9/1/09 3:52p jgarrett
# SW7335-529: Replacing direct access to MAGNUM_MODULES with
#  NEXUS_EXTRA_MODULES to avoid magnum paths leaking into application
#  include paths
#
# 37   8/26/09 7:37a erickson
# SWDEPRECATED-3995: add PID2BUF define
#
# 36   7/16/09 12:43p erickson
# PR56657: added NEXUS_RecpumpOpenSettings.nullifyVct
#
# 35   6/8/09 11:49a erickson
# PR53324: add PID2BUF impl, default off
#
# 34   5/26/09 11:39a erickson
# PR55105: add packet sub api
#
# 33   4/8/09 4:31p vsilyaev
# PR 54016: Added support for precompiled header
#
# 32   3/9/09 4:37p yili
# PR52990:Add region verification
#
# 31   2/4/09 7:09p vsilyaev
# PR 50887: Added bit to enable OTF PVR mode
#
# 30   11/5/08 2:10p erickson
# PR45959: added NEXUS_TRANSPORT_EXTENSION_INC
#
# 29   10/6/08 4:58p vsilyaev
# PR 47440: Build DIVX DRM as a separate library
#
# 28   9/11/08 10:36a jrubio
# PR42593: add remux to all chips
#
# 27   9/2/08 3:25p jrubio
# PR42353: 7335 does have remux
#
# 26   8/7/08 4:47p erickson
# PR45422: DivX DRM fix
#
# 25   8/5/08 10:57a erickson
# PR45422: allow c99 exception for divxdrm5 library
#
# 24   8/5/08 10:26a erickson
# PR45422: add DIVX_DRM_SUPPORT
#
# 23   7/26/08 3:49p maivu
# PR 44679: Added divx subtitle build option for nexus
#
# 22   7/23/08 2:46p erickson
# PR45016: add SUBTITLE_SUPPORT
#
# 21   7/21/08 3:16p erickson
# PR45016: added SUBTITLE_SUPPORT
#
# 20   6/30/08 10:34a erickson
# PR44320: enable remux for 3548/3556
#
# 19   6/13/08 7:00p erickson
# PR40817: fix PVR_SUPPORT=n mode
#
# 18   5/30/08 10:54a erickson
# PR43123: add USE_SETTOP_API_SW_RAVE build option, but default it off.
#
# 17   5/21/08 3:24p jrubio
# PR42353: 3556/3548 no remux support
#
# 16   5/21/08 3:09p jrubio
# PR42353: remove 7325,7335 from remux correctly
#
# 15   5/20/08 7:48p shyam
# PR 42103 : Add transport MPOD support
#
# 14   5/7/08 2:00p vsilyaev
# PR 41869: Added core for re-packetize MPEG-2 TS data for encrypted PVR
#
# 13   4/18/08 5:41p vsilyaev
# PR 41868: Connected playpump security to MS DRM
#
# 12   4/18/08 4:03p vsilyaev
# PR 41868: Added security API to playpump and recpump
#
# 11   4/10/08 1:58p erickson
# PR40079: added PidChannel ScramblingCheck
#
# 10   4/10/08 11:08a erickson
# PR41597: fix file name
#
# 9   4/7/08 10:15a erickson
# PR37893: convert to XPT PI sw rave
#
# 8   3/31/08 4:41p erickson
# PR40817: compile with PVR_SUPPORT=n
#
# 7   3/4/08 5:06p jrubio
# PR40169: add 7325 and remove remux for 7325
#
# 6   3/4/08 3:31p erickson
# PR40080: add transport error callbacks
#
# 5   2/26/08 10:01a erickson
# PR39934: added SW RAVE support
#
# 4   2/22/08 5:52p shyam
# PR39435 : Add remux capability to Nexus
#
# 3   2/20/08 10:21a erickson
# PR34925: allow pcrlib to compile without MVD support
#
# 2   1/24/08 5:18p vsilyaev
# PR 38682: Compile bmedia framework into the kernel
#
# 1   1/18/08 2:16p jgarrett
# PR 38808: Merging to main branch
#
# Nexus_Devel/14   1/17/08 6:07p jgarrett
# PR 38808: Updating file locations
#
# Nexus_Devel/13   12/4/07 6:19p vsilyaev
# PR 37574: Added more bmedia support
#
# Nexus_Devel/12   11/9/07 3:07p jgarrett
# PR 36774: Editing unified ITB definition
#
# Nexus_Devel/11   11/6/07 4:22p jgarrett
# PR 34954: Migrating to audio connector model
#
# Nexus_Devel/10   9/28/07 4:41p jgarrett
# PR 35002: Adding UNIFIED_ITB_SUPPORT for 7400
#
# Nexus_Devel/9   9/28/07 10:00a erickson
# PR34925: 0.1 prerel review
#
# Nexus_Devel/8   9/27/07 7:52p jgarrett
# PR 34954: Adding HSM
#
# Nexus_Devel/7   9/27/07 2:00p erickson
# PR34925: added syncthunk
#
# Nexus_Devel/6   9/26/07 4:15p erickson
# PR34925: define BSEAV
#
# Nexus_Devel/5   9/26/07 3:11p erickson
# PR34925: initial playpump checkin
#
# Nexus_Devel/4   9/24/07 2:36p erickson
# PR34925: added Message
#
# Nexus_Devel/3   9/21/07 11:18a erickson
# PR34925: rename parserband.c to demux.c
#
# Nexus_Devel/2   9/17/07 4:12p erickson
# PR34925: added timebase and stcchannel
#
# Nexus_Devel/1   9/14/07 3:27p erickson
# PR34925: initial compilation
#
############################################################

#
# Conditional Options for this Makefile
#

######################
# Define this module #
######################
NEXUS_MODULES += TRANSPORT

##################
# Module Options #
##################

###################
# Module Includes #
###################
NEXUS_TRANSPORT_PUBLIC_INCLUDES = $(NEXUS_TOP)/modules/transport/include

NEXUS_TRANSPORT_PRECOMPILED_HEADER = $(NEXUS_TOP)/modules/transport/src/nexus_transport_module.h

##################
# Module Sources #
##################
NEXUS_TRANSPORT_SOURCES := \
    $(NEXUS_TOP)/modules/transport/src/nexus_demux.c \
    $(NEXUS_TOP)/modules/transport/src/nexus_pid_channel_scrambling.c \
    $(NEXUS_TOP)/modules/transport/src/nexus_parser_band.c \
    $(NEXUS_TOP)/modules/transport/src/nexus_rave.c \
    $(NEXUS_TOP)/modules/transport/src/nexus_timebase.c \
    $(NEXUS_TOP)/modules/transport/src/nexus_stc_channel.c \
    $(NEXUS_TOP)/modules/transport/src/nexus_playpump.c \
    $(NEXUS_TOP)/modules/transport/src/nexus_playpump_crypto.c \
    $(NEXUS_TOP)/modules/transport/src/nexus_playpump_media.c \
    $(NEXUS_TOP)/modules/transport/src/nexus_playpump_priv.c \
    $(NEXUS_TOP)/modules/transport/src/nexus_recpump.c \
    $(NEXUS_TOP)/modules/transport/src/nexus_transport_capabilities.c \
    $(NEXUS_TOP)/modules/transport/src/nexus_transport_module.c \
    $(NEXUS_TOP)/modules/transport/src/nexus_tsmux.c \
    $(NEXUS_TOP)/modules/transport/src/nexus_mpod.c   \
    $(NEXUS_TOP)/modules/transport/src/nexus_packetsub.c \
    $(NEXUS_TOP)/modules/transport/src/nexus_remux.c \
    $(NEXUS_TOP)/modules/transport/src/nexus_tsmf.c 

ifneq ($(findstring $(BCHP_CHIP),7550 7408),)
# Software-based message filtering. Required for chips w/o HW filtering. Optional for other chips.
NEXUS_USE_SW_FILTER=y
endif

ifeq ($(NEXUS_USE_SW_FILTER),y)
NEXUS_TRANSPORT_SOURCES += $(NEXUS_TOP)/modules/transport/src/nexus_message_swfilter.c
NEXUS_TRANSPORT_SOURCES += $(NEXUS_TOP)/modules/transport/src/nexus_message_swfilter_parser.c
NEXUS_TRANSPORT_DEFINES += NEXUS_USE_SW_FILTER=1
else
NEXUS_TRANSPORT_SOURCES += $(NEXUS_TOP)/modules/transport/src/nexus_message_pid2buf.c
endif
NEXUS_TRANSPORT_SOURCES += $(NEXUS_TOP)/modules/transport/src/nexus_message_sam.c

ifeq ($(findstring $(BCHP_CHIP), 7425), $(BCHP_CHIP))

ifeq ($(NEXUS_FRONTEND_45216),y)

else
NEXUS_TRANSPORT_DEFINES += NEXUS_HAS_TSMF=1
endif

endif

ifeq ($(BXPT_VCT_SUPPORT),y)
NEXUS_TRANSPORT_DEFINES += BXPT_VCT_SUPPORT=1
endif

# thunk layer
NEXUS_TRANSPORT_PRIVATE_INCLUDES += $(NEXUS_TOP)/modules/transport/src $(NEXUS_SYNCTHUNK_DIR)
NEXUS_TRANSPORT_SOURCES += $(NEXUS_SYNCTHUNK_DIR)/nexus_transport_thunks.c

# pvr utils
NEXUS_TRANSPORT_PRIVATE_INCLUDES += \
    $(BSEAV)/lib/utils
NEXUS_TRANSPORT_SOURCES += \
    $(BSEAV)/lib/utils/bsink_playback.c

#######################
# Module Dependencies #
#######################
NEXUS_TRANSPORT_DEPENDENCIES := DMA

##################
# Magnum Modules #
##################
# These modules will be excluded from app-only builds
BPCRLIB_HAS_RAP := n
BPCRLIB_HAS_APE := n
BPCRLIB_HAS_XVD := n
BPCRLIB_HAS_MVD := n
NEXUS_TRANSPORT_MAGNUM_MODULES =  \
    $(MAGNUM)/portinginterface/xpt/bxpt.inc \
    $(MAGNUM)/syslib/pvrlib/bpvrlib.inc \
    $(MAGNUM)/syslib/pcrlib/bpcrlib.inc

# change XPT PI default behavior so that app has option of starting msg filter with disabled pid channel
NEXUS_TRANSPORT_DEFINES += BXPT_MESG_DONT_AUTO_ENABLE_PID_CHANNEL

ifeq ($(BXPT_FILTER_32),y)
NEXUS_TRANSPORT_DEFINES += BXPT_FILTER_32 NEXUS_MESSAGE_FILTER_SIZE=32
endif

# BSEAV makefiles don't include their module names into MAGNUM_MODULES
NEXUS_EXTRA_MODULES += BFILE BMEDIA BMEDIA_FILTER BATOM

ifeq ($(PVR_SUPPORT),n)
# we don't need the media framework, but still need some code from BSEAV
NEXUS_TRANSPORT_DEFINES += B_HAS_MEDIA=0
ifneq ($(B_REFSW_OS),ucos_ii)
NEXUS_TRANSPORT_MAGNUM_MODULES +=  \
    ${BSEAV}/lib/bfile/bfile.inc 
endif

NEXUS_TRANSPORT_MAGNUM_MODULES +=  \
    ${BSEAV}/lib/utils/batom.inc

NEXUS_TRANSPORT_SOURCES += \
    ${BSEAV}/lib/mpeg2_ts_parse/ts_pmt.c \
    ${BSEAV}/lib/mpeg2_ts_parse/ts_pat.c \
    ${BSEAV}/lib/mpeg2_ts_parse/ts_priv.c \
    ${BSEAV}/lib/mpeg2_ts_parse/ts_psi.c \
    ${BSEAV}/lib/mpeg2_ts_parse/tspsi_validate.c
NEXUS_TRANSPORT_PRIVATE_INCLUDES += ${BSEAV}/lib/mpeg2_ts_parse
else
NEXUS_TRANSPORT_MAGNUM_MODULES +=  \
    ${BSEAV}/lib/utils/batom.inc \
    ${BSEAV}/lib/media/bmedia.inc

ifeq ($(MEDIA_ASF_SUPPORT), y)
NEXUS_TRANSPORT_MAGNUM_MODULES +=  \
    ${BSEAV}/lib/asf/basf.inc
endif


ifeq ($(MEDIA_ASF_SUPPORT), y)
NEXUS_TRANSPORT_MAGNUM_MODULES +=  \
    ${BSEAV}/lib/asf/basf.inc

ifeq ($(MSDRM_PD_SUPPORT), y)
NEXUS_TRANSPORT_DEFINES+=B_HAS_MSDRM_PD=1

MSDRM_PD_BIN_DIR := $(BSEAV)/lib/drm/wmdrmpd10/bin

# These are really mis-named and are actually .lo files
MAGNUM_PRECOMPILED_OBJECTS += \
        $(MSDRM_PD_BIN_DIR)/oem_ansi.a   \
        $(MSDRM_PD_BIN_DIR)/drmmanager.a  \
        $(MSDRM_PD_BIN_DIR)/drmcore.a    \
        $(MSDRM_PD_BIN_DIR)/drmpkcrypto.a \
        $(MSDRM_PD_BIN_DIR)/drmbignum.a  \
        $(MSDRM_PD_BIN_DIR)/drmdevcert.a  \
        $(MSDRM_PD_BIN_DIR)/drmblackbox.a \
        $(MSDRM_PD_BIN_DIR)/drmcrypto.a  \
        $(MSDRM_PD_BIN_DIR)/drmdevicedevcert.a

NEXUS_TRANSPORT_DEFINES += DRM_SUPPORT_DLA=1 D_DEBUG DEBUG B_HAS_DRM=1 NEXUS_DRM_BUILD=1
NEXUS_TRANSPORT_PRIVATE_INCLUDES += $(BSEAV)/lib/drm $(BSEAV)/lib/drm/wmdrmpd10/inc $(BSEAV)/lib/utils $(BSEAV)/lib/asf

NEXUS_TRANSPORT_SOURCES += \
        $(BSEAV)/lib/drm/drmcore.c \
        $(BSEAV)/lib/drm/drmhttp.c \
        $(BSEAV)/lib/drm/drmplay.c \
        $(BSEAV)/lib/drm/drmchallenge.c \
        $(BSEAV)/lib/drm/drmresponse.c \
        $(BSEAV)/lib/drm/drmenv.c \
        $(BSEAV)/lib/drm/drmdecryptor.c \
        $(BSEAV)/lib/drm/asfpars.c
endif

ifeq ($(MSDRM_PRDY_SUPPORT), y)
ifeq ($(HUMAX_PLATFORM_BASE),y)
else
NEXUS_TRANSPORT_PRIVATE_INCLUDES += $(BSEAV)/lib/playready $(BSEAV)/lib/playready/inc $(BSEAV)/lib/utils $(BSEAV)/lib/asf
endif
endif

ifeq ($(MSDRM_ND_SUPPORT), y)
NEXUS_TRANSPORT_DEFINES+=B_HAS_MSDRM_ND=1
NEXUS_TRANSPORT_PRIVATE_INCLUDES += $(BSEAV)/lib/drmnd/ $(BSEAV)/lib/drmnd/wmdrmnd10/port $(BSEAV)/lib/drmnd/wmdrmnd10/inc $(BSEAV)/lib/drmnd/wmdrmnd10/oem
MAGNUM_PRECOMPILED_OBJECTS += $(BSEAV)/lib/drmnd/wmdrmnd10/bin/drmndport.a $(BSEAV)/lib/drmnd/wmdrmnd10/oem/ansi/oem_ansi.a
NEXUS_TRANSPORT_SOURCES += $(BSEAV)/lib/drmnd/bdrmnd_decryptor.c
endif

#end of MEDIA_ASF_SUPPORT
endif

ifeq ($(MEDIA_AVI_SUPPORT), y)
ifeq ($(DIVX_DRM_SUPPORT), y)
ifeq ($(findstring $(PLATFORM), 97400 97456 97403),)
include $(BSEAV)/lib/divxdrm5/Makefile.drm
else
include $(BSEAV)/lib/divxdrm/Makefile.drm
endif
NEXUS_TRANSPORT_SOURCES += $(DIVX_DRM_SRCS)
NEXUS_CFLAGS += $(DIVX_DRM_CFLAGS)
MAGNUM_PRECOMPILED_OBJECTS += $(DIVX_DRM_OBJS)
# the divxdrm5 library requires c99
NEXUS_C_STD=c99
endif
NEXUS_TRANSPORT_MAGNUM_MODULES +=  \
    ${BSEAV}/lib/avi/bavi.inc
endif

# end of MEDIA_AVI_SUPPORT
endif

ifeq ($(MEDIA_FLV_SUPPORT), y)
NEXUS_TRANSPORT_MAGNUM_MODULES +=  \
    ${BSEAV}/lib/media/flv/bflv.inc
endif

ifeq ($(MEDIA_RMFF_SUPPORT), y)
NEXUS_TRANSPORT_MAGNUM_MODULES +=  \
    ${BSEAV}/lib/media/rmff/brmff.inc
endif


ifeq ($(NEXUS_COMMON_CRYPTO_SUPPORT), y)
NEXUS_EXTRA_MODULES += COMMON_CRYPTO
NEXUS_TRANSPORT_MAGNUM_MODULES += $(BSEAV)/lib/security/common_crypto/common_crypto.inc
endif


#####################
# Module Extensions #
#####################
ifneq ($(NEXUS_TRANSPORT_EXTENSION_INC),)
include $(NEXUS_TRANSPORT_EXTENSION_INC)
endif

ifeq ($(RAVE_REGION_VERIFICATION_SUPPORT),y)
include $(NEXUS_TRANSPORT_SECURITY_EXTENSION_INC)
endif

