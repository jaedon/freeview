############################################################
#     (c)2008-2012 Broadcom Corporation
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
# $brcm_Workfile: Makefile $
# $brcm_Revision: 82 $
# $brcm_Date: 10/5/12 11:27a $
#
#
# Module Description:
#
# Revision History:
#
# $brcm_Log: /nexus/lib/playback_ip/apps/Makefile $
# 
# 82   10/5/12 11:27a cdisc
# SW7425-3979: updating Makefile to copy ssl and crypto symlinks
# 
# 81   10/4/12 11:29a cdisc
# SW7425-3979: DTCP-IP streamlining
# 
# SW7425-3979/2   10/3/12 1:26p cdisc
# SW7425-3979: updating Makefile
# 
# SW7425-3979/1   9/28/12 2:35p cdisc
# SW7425-3979:  streamlining DTCP-IP
# 
# 79   9/11/12 3:57p leisun
# SWSECURITY-48: DTCP-IP change. Auto-generating test keys, consolidate
#  test/production build, make common DRM default ON
# 
# 78   7/26/12 8:33p ssood
# SW7346-944: add support to stream from OFDM-capable frontend
# 
# 77   7/19/12 4:07p gskerl
# SW7346-853: Merged from branch SW7346-853 to main.
# 
# SW7346-853/3   7/10/12 3:27p gskerl
# SW7346-853: Updated branch SW7346-853 from main/LATEST.
# 
# SW7346-853/2   6/21/12 12:45p gskerl
# SW7346-853: Fixed typo... changed PLAYBACK_IP_XML_SUPPORT to
#  B_PLAYBACK_IP_XML_SUPPORT.
# 
# SW7346-853/1   6/20/12 6:36p gskerl
# SW7346-853: Added handling for BSEAV mlibs XML parsing code (libmnet,
#  libcommon).
# 
# 76   6/20/12 5:00p ssood
# SW7231-858: copy OpenSSL libs independent of the DTCP/IP libs
# 
# 75   5/31/12 8:12p ssood
# SW7425-2894: print console message to indicate its building OpenSSL lib
#  as it takes a while to build this lib
# 
# 74   5/31/12 7:20p ssood
# SW7425-2894: modify makefiles to use the latest OpenSSL lib
# 
# 73   5/25/12 4:07p ssood
# SW7425-3042: define LIVEMEDIA_SUPPORT in playback_ip lib inc file so
#  apps dont have to explicitly define in their apps
# 
# 72   5/23/12 3:37p celysee
# SW7425-2894: Adding common drm and dtcp-ip
# 
# 71   5/23/12 11:47a celysee
# SW7425-2894: Changes to makefile to copy libnexus_security library
#  under nexus/bin
# 
# 70   5/22/12 6:56p ssood
# SW7425-2894: copy libssl file as well
# 
# 69   5/22/12 6:15p ssood
# SW7425-2894: Ip Applib changes for supporting DTCP-IP with robustness
#  rules
# 
# 68   5/22/12 8:56a ssood
# SW7425-3042: enabled compilation of diags.c file
# 
# 67   5/17/12 6:36p ssood
# SW7425-3042: simplified build flags & code by using nexus multi process
#  capabilities
# 
# 66   5/15/12 4:13p ssood
# SW7425-3042: disable compilation of diags file for now
# 
# 65   5/14/12 4:33p ssood
# SW7425-3042: refactor ip_streamer into multiple sub files
# 
# 64   4/30/12 11:33a ssood
# SW7425-2690: build tshdrbuilder lib for all 40nm chips for supporting
#  live streaming using raaga encoder
# 
# 63   4/18/12 6:17p celysee
# SW7425-2894: Support for DTCP-IP with robustness rules
# 
# 62   3/22/12 9:18a ssood
# SW7425-2690: remove deprecated kernel mode build flags
# 
# 61   3/12/12 4:41p gkamosa
# SW7425-2543: Merge support for 3383 SMS platform
# 
# Centaurus_Phase_1.0/2   3/6/12 6:03p gkamosa
# SW7425-2337: Merge branch to tip
# 
# Centaurus_Phase_1.0/1   2/6/12 6:06p jmiddelk
# SW7425-2337: Adding support for 93383sms
# 
# 60   1/27/12 2:05p mward
# SW7435-7:  Add 97435, same capabilities as 97425.
# 
# 59   1/18/12 3:14p ssood
# SW7231-603: dont call netaccel install for brutus builds
# 
# 58   1/12/12 5:01p ssood
# SW7425-2151: explicitly copy libraries over to the BSEAV/bin
# 
# 57   1/4/12 3:19p steven
# SW7429-37: Add AppLibs/Trellis Support for 7241
# 
# 56   12/13/11 7:33p ssood
# SW7231-516: enabled support for local display while encoding & live
#  channels encoding & streaming out using raaga dsp
# 
# 55   12/8/11 7:40p ssood
# SW7231-516: enhance ip_streamer to stream out raaga encoded TS streams
#  from live or file sources
# 
# 54   10/31/11 5:54p ssood
# SW7425-889: build tshdrbuilder library outside of netaccel
# 
# 53   10/18/11 7:25a ssood
# SW7425-889: enabled audio in the live xcoded streams for streaming to
#  iOS devices
# 
# 52   10/7/11 11:32a ssood
# SW7425-1436: Rename DEBUG to B_REFSW_DEBUG
# 
# 51   9/19/11 1:50p ssood
# SW7425-1040: added logic to better handle network congestion
# 
# 50   8/21/11 10:40p ssood
# SW7425-1040: correct dtcp flags to turn off h/w decryption & encryption
# 
# 49   8/7/11 7:00p ssood
# SW7425-1040: more changes to support FCC over IP sessions
# 
# 48   7/14/11 12:01p ssood
# SW7346-309: Add support for Streaming from Satellite Source
# 
# 47   6/6/11 1:35a ssood
# SW7231-166: refine trickplay rates, account for timeSeekEnd from
#  ip_client
# 
# 46   5/12/11 4:21p ssood
# SW7420-741: change the default B_REFSW_ARCH value to mipsel-linux so
#  that it matches reference sw value while building ip apps
# 
# 45   5/3/11 3:49p ssood
# SW7425-492: dont include netaccel header file for non-netaccel builds
# 
# 44   3/9/11 7:47p ssood
# SW7420-1617: use B_REFSW_ARCH instead of ARCH for default arch variable
# 
# 43   12/31/10 3:25p ssood
# SW7425-59: add change to support clean stop/stop sequence w/ 7425
#  xcoder + insert PAT/PMT into outgoing stream
# 
# 42   12/30/10 12:55p ssood
# SW7425-59: initial 7425 transcoder support on h/w platform (no PSI, no
#  PCRs, no Audio in xcoded stream)
# 
# 41   12/14/10 12:08p ssood
# SW7420-1217: add support for encryption key parsing, downloading and
#  receiving encrypted content
# 
# 40   12/13/10 10:30p ssood
# SW7420-1257: add compile time option to support streaming out LIVE
#  channels (mainly for Live IP channels)
# 
# 39   11/24/10 2:58p ssood
# SW7420-1257: Video Conferencing Demo: add support for receiving input
#  camera input and stream it out
# 
# 38   9/17/10 8:21a ssood
# SWGERRARD-547: support to expose multiple multiple programs and tracks
#  within a multi-program stream
# 
# 37   9/2/10 2:51p garetht
# SW7420-1046: Add support for sending 6xQAM256 streams to the cablecard
# 
# 36   8/6/10 11:24a garetht
# SW7420-919: Add support for DSG CableCARD
# 
# 35   7/30/10 3:42p garetht
# SW7420-919: Add CableCARD support to Ip_streamer
# 
# 34   7/30/10 3:19p ssood
# SW7420-883: add support to transcode/pass-thru audio to IP Streamer
# 
# 33   7/27/10 2:08p ssood
# SW7420-883: add transcoding support to IP Streamer (no audio
#  transcoding yet)
# 
# 32   5/18/10 1:31p sgundime
# SW7420-559: set env. variable EROUTER_SUPPORT=n to build dms stand
#  alone with out eRouter support.
# 
# 31   4/26/10 6:00p mward
# SW7400-2511: don't add B_REFSW_ to ARCH, BSEAV build system uses ARCH.
# 
# 30   4/23/10 9:19a mward
# SW7125-325: B_REFSW_ARCH is now used by liveMedia.
# 
# 29   4/22/10 4:13p mward
# SW7400-2511: don't add B_REFSW_ to ARCH, BSEAV build system uses ARCH.
# 
# 28   4/22/10 12:06p mward
# SW7400-2511: add B_REFSW_ or NEXUS_ prefix to build variables
# SW7125-325: must link c++ liveMedia lib using c++ when using stbgcc-
#  4.4.3-1.0
# 
# 27   4/2/10 5:58p ssood
# SW7420-561: add support to handle large URLs (HTML5 URLs can be over
#  256bytes) + Coverity fixes
# 
# 26   3/16/10 12:46p ssood
# SW7420-561: fixed an cp syntax error
# 
# 25   3/15/10 3:13p ssood
# SW7420-561: suppress the extra prints for default build
# 
# 24   3/4/10 12:17p ssood
# SW7420-561: merge from branch to mainline
# 
# SW7420-561/2   3/1/10 7:51a ssood
# SW7420-561: compile livemedia library only if LIVEMEDIA_SUPPORT is
#  defined
# 
# SW7420-561/1   2/16/10 10:11a ssood
# SW7420-561: initial cut of new APIs to meet Media Browser, DMR, &
#  Philips RTSP requirements
# 
# 23   2/18/10 7:14p jrubio
# SW7342-159: Do not compile nexus if being built from SETTOP API
# 
# 22   2/3/10 6:05p ismailk
# SW7420-570: Add 93380vms support to Makefile.
#
# 20   1/29/10 6:12p ismailk
# SW7420-559: Enable EROUTER_SUPPORT flag to enable the WAN unicast
#  ioctl.
#
# 19   1/28/10 12:14p ssood
# SW7420-454: define RAD_EA flag is RAD_EA_SUPPORT is enabled
#
# 18   1/27/10 10:18a ssood
# SW7420-454: conditionally compile Live Streaming Code using
#  LIVE_STREAMING_SUPPORT
#
# 17   1/26/10 4:54p ssood
# SW7420-454: added command to clean nexus build as well
#
# 16   1/26/10 4:43p ssood
# SW7420-454: set defult build mode to DEBUG if not defined
#
# 15   1/25/10 6:35p ssood
# SW7420-454: set default ARCH if not defined
#
# 14   1/20/10 5:30p ssood
# SW7420-454: added option to compile out auto PSI detection via
#  AUTO_PSI_SUPPORT=n
#
# 13   12/10/09 7:29p ssood
# SW7420-502: Added compile time flags to enable chunk encoding support
#  on the server
#
# 12   11/17/09 2:58p ssood
# SW7420-454: Enhance IP Streamer to stream files from local disk
#
# 11   10/20/09 6:33p ssood
# SW7420-340: remove nexus playback, audio, video decoder dependencies
#  for SMS compilation
#
# 10   10/13/09 6:19p ssood
# SW7420-340: add option to enable kernel mode builds
#
# 9   10/7/09 3:46p ssood
# SW7420-340: added STANDL
#
# 8   10/6/09 12:18p ssood
# SW7420-340: Add DTCP/IP support to ip_streamer & IP Applib
#
# 7   9/30/09 9:50a ssood
# SW7420-340: took out extra print statements
#
# 6   9/29/09 3:59p ssood
# SW7420-340: add command to copy binaries for each app build also
#
# 5   9/24/09 9:02a ssood
# SW7420-340: Copy binaries to Nexus bin directory as well
#
# 4   9/17/09 4:52p ssood
# SW7420-340: dont compile settop api by default
#
# 3   9/17/09 2:58p ssood
# SW7420-340: compiling for SMS
#
# 2   9/16/09 11:21a ssood
# SW7420-340: modified binary name to ip_client
#
# 1   9/16/09 10:50a ssood
# SW7420-340: Adding example apps for demonstrating IP Streaming & Client
#  features of IP Applib
#
#
############################################################

ifdef COMSPEC
# Any DOS environment
NEXUS_TOP := $(shell cd ../../../../nexus && cd)
else
NEXUS_TOP := $(shell cd ../../../../nexus ; pwd)
endif
B_PSIP_TOP := $(NEXUS_TOP)/../rockford/lib/psip
B_MPOD_TOP := $(NEXUS_TOP)/../rockford/lib/ocap/mpod
B_MPOD_DRIVERDIR = $(B_MPOD_TOP)/src/driver/build/bcm$(PLATFORM)$(shell awk 'BEGIN{print tolower("$(BCHP_VER)")}')/linux
BSEAV := $(NEXUS_TOP)/../BSEAV
LIVEMEDIA_DIR := $(BSEAV)/lib/livemedia/live

ifndef PLATFORM
$(error PLATFORM is not defined)
endif

ifeq ($(findstring $(PLATFORM),93380sms,93383sms), $(PLATFORM))
CFLAGS += -DB_HAS_SMS_GATEWAY 
export SMS_GATEWAY=y
ifneq ($(EROUTER_SUPPORT),n)
export EROUTER_SUPPORT=y
CFLAGS += -DEROUTER_SUPPORT
endif

# When compiling for SMS platforms, .inc files of Nexus Playback, Audio, Video modules are not included.
# Thus, we explictly include their paths for header files.
CFLAGS += -DNEXUS_HAS_PLAYBACK -I$(NEXUS_TOP)/modules/playback/include -I$(NEXUS_TOP)/modules/video_decoder/${BCHP_CHIP}/include -I$(NEXUS_TOP)/modules/audio/${BCHP_CHIP}/include
endif

# enable IP Streamer in demo mode (default setting): loops around when EOF is reached during trickplays
ifneq ($(IP_STREAMER_DEMO_MODE),n)
IP_STREAMER_DEMO_MODE=y
export IP_STREAMER_DEMO_MODE
CFLAGS += -DIP_STREAMER_DEMO_MODE
endif

ifeq ($(DISPLAY_LOCAL_FOR_ENCODE),y)
CFLAGS += -DB_HAS_DISPLAY_LOCAL_FOR_ENCODE
endif

ifeq ($(B_REFSW_ARCH),)
B_REFSW_ARCH=mipsel-linux
export B_REFSW_ARCH
endif

ifneq ($(B_REFSW_DEBUG),n)
B_REFSW_DEBUG=y
export B_REFSW_DEBUG
endif

ifeq ($(USE_HTTP_CHUNK_ENCODING),y)
export USE_HTTP_CHUNK_ENCODING
CFLAGS += -DB_USE_HTTP_CHUNK_ENCODING
endif

OPENSSL_TOP := $(BSEAV)/lib/openssl
IP_APPLIB_TEST_LIB_FLAGS := -L$(NEXUS_TOP)/bin -lb_playback_ip -lnexus -lb_os

################################# Start of DTCP_IP_SUPPORT ##############################
ifeq ($(DTCP_IP_SUPPORT),y)

ifeq ($(SSL_SUPPORT), n)
$(error SSL_SUPPORT is disabled, cannot enable DTCP_IP_SUPPORT)
endif
export SSL_SUPPORT=y

ifneq ($(HSM_SOURCE_AVAILABLE),y)
ifeq ($(NEXUS_EXTRALIBS),)
ifeq ($(NEXUS_EXTRA_SECLIBS_FILES),)
$(error HSM_SOURCE_AVAILABLE is disabled. Define either NEXUS_EXTRALIBS (user mode) or NEXUS_EXTRA_SECLIBS_FILES (proxy) to point to static libnexus_security library for 40nm)
endif
endif
endif #HSM_SOURCE_AVAILABLE


ifneq ($(DTCP_IP_HARDWARE_DECRYPTION),n)
DTCP_IP_HARDWARE_DECRYPTION=y
export DTCP_IP_HARDWARE_DECRYPTION
endif #DTCP_IP_HARDWARE_DECRYPTION

ifneq ($(DTCP_IP_HARDWARE_ENCRYPTION),n)
DTCP_IP_HARDWARE_ENCRYPTION=y
export DTCP_IP_HARDWARE_ENCRYPTION
endif #DTCP_IP_HARDWARE_ENCRYPTION

#always add '-lb_dtcp_ip' to link flag
IP_APPLIB_TEST_LIB_FLAGS +=  -lb_dtcp_ip
IP_APPLIB_TEST_LIB_FLAGS +=  -L$(BSEAV)/lib/security/bcrypt/lib -lbcrypt
CFLAGS += -DB_HAS_DTCP_IP

ifeq ($(DTCP_IP_HARDWARE_ENCRYPTION),y)
CFLAGS += -DB_DTCP_IP_HW_ENCRYPTION
endif

ifeq ($(DTCP_IP_HARDWARE_DECRYPTION),y)
CFLAGS += -DB_DTCP_IP_HW_DECRYPTION
endif

include $(BSEAV)/lib/security/common_drm/lib/cmndrm_dir.inc
IP_APPLIB_TEST_LIB_FLAGS +=  -L$(BSEAV)/lib/security/common_drm/lib/$(CMNDRM_DIR)/ -lcmndrm
IP_APPLIB_TEST_LIB_FLAGS +=  -L$(BSEAV)/lib/drmrootfs/ -ldrmrootfs

endif 
####################################### End of DTCP_IP_SUPPORT ##############################


ifeq ($(RAD_EA_SUPPORT), y)
CFLAGS += -DB_HAS_RAD_EA
endif

include $(BSEAV)/lib/tshdrbuilder/tshdrbuilder.inc
CFLAGS += $(addprefix -I,$(TSHDRBUILDER_INCLUDES))

# include cross-compiler definitions
include $(NEXUS_TOP)/platforms/$(PLATFORM)/build/platform_app.inc

# include "LiveMedia extension" library definitions
include $(BSEAV)/lib/livemedia/blive_ext/include/blive_ext.mak

#include applib definitions
include $(NEXUS_TOP)/lib/playback_ip/b_playback_ip_lib.inc
include $(NEXUS_TOP)/lib/os/b_os_lib.inc
include $(B_PSIP_TOP)/b_psip_lib.inc
ifeq ($(STREAMER_CABLECARD_SUPPORT),y)
ESTB_CFG_SUPPORT=y
export ESTB_CFG_SUPPORT
include $(B_MPOD_TOP)/b_mpod_lib.inc
include $(B_MPOD_TOP)/../estb_cfg/b_estb_cfg_lib.inc
endif


#off by default
ifeq ($(NETACCEL_SUPPORT),y)
export NETACCEL_SUPPORT
CFLAGS += -DB_HAS_NETACCEL_SUPPORT
endif #NETACCEL_SUPPORT

ifeq ($(HLS_PROTOCOL_SUPPORT), y)
export HLS_PROTOCOL_SUPPORT=y
ifeq ($(HTTP_AES_SUPPORT), n)
$(error HTTP_AES_SUPPORT is disabled, cannot enable HLS_PROTOCOL_SUPPORT)
endif
export HTTP_AES_SUPPORT=y
endif

ifeq ($(HTTP_AES_SUPPORT), y)
ifeq ($(SSL_SUPPORT), n)
$(error SSL_SUPPORT is disabled, cannot enable HTTP_AES_SUPPORT)
endif
export SSL_SUPPORT=y
CFLAGS += -DB_HAS_HTTP_AES_SUPPORT
endif


IP_APPLIB_TEST_SRC_PATH = .
IP_APPLIB_TEST_INC_PATH = .
IP_APPLIB_TEST_ODIR = $(PLATFORM).$(B_REFSW_ARCH)


ifeq ($(SSL_SUPPORT),y)
IP_APPLIB_TEST_LIB_FLAGS +=  -ldl
IP_APPLIB_TEST_LIB_FLAGS +=  -L$(BSEAV)/lib/openssl/$(PLATFORM)/lib -lssl
IP_APPLIB_TEST_LIB_FLAGS +=  -L$(BSEAV)/lib/openssl/$(PLATFORM)/lib -lcrypto 
CFLAGS += -DB_HAS_SSL
endif # SSL_SUPPORT

ifeq ($(B_PLAYBACK_IP_XML_SUPPORT), y)
IP_APPLIB_TEST_LIB_FLAGS +=  -L$(BSEAV)/lib/mlibs/$(B_REFSW_ARCH).$(DEBUG_SUFFIX)/ -lmnet -lmcommon
endif


ifeq ($(STREAMER_CABLECARD_SUPPORT),y)
CFLAGS += -DSTREAMER_CABLECARD_SUPPORT -DDSG_SUPPORT -DBCM_DSG_DUAL_PROCESSOR_INTERFACE -DMPOD_SUPPORT -DINTEGRATE_BCM_DSG_HOST -DESTB_CFG_SUPPORT
ifeq ($(MPODXSIX),y)
CFLAGS += -DMPODXSIX
endif
endif

IP_APPLIB_TEST_LIB_FLAGS += -lm -lpthread
IP_APPLIB_TEST_LIB_FLAGS += -lb_psip

ifeq ($(STREAMER_CABLECARD_SUPPORT),y)
IP_APPLIB_TEST_LIB_FLAGS += -lb_mpod -lb_estb_cfg -L$(B_MPOD_TOP)/lib/cp/build -lcp -L$(B_MPOD_TOP)/lib/cp/crypto/linux -lcrypto.0.9.7b_uclibc $(BSEAV)/docsis/StbHostApps/DsgCC/build/dsgcclib.a -lrt
endif

ifeq ($(LIVEMEDIA_SUPPORT),y)
    IP_APPLIB_TEST_LIB_FLAGS += -L$(LIVEMEDIA_DIR)/liveMedia -lliveMedia
    IP_APPLIB_TEST_LIB_FLAGS += -L$(LIVEMEDIA_DIR)/groupsock -lgroupsock
    IP_APPLIB_TEST_LIB_FLAGS += -L$(LIVEMEDIA_DIR)/UsageEnvironment -lUsageEnvironment
    IP_APPLIB_TEST_LIB_FLAGS += -L$(LIVEMEDIA_DIR)/BasicUsageEnvironment -lBasicUsageEnvironment
    IP_APPLIB_TEST_LIB_FLAGS += $(BLIVE_EXT_LDFLAGS)
    B_LINKER := $(CXX)
else
    B_LINKER := $(CC)
endif
ifeq ($(STREAMER_CABLECARD_SUPPORT),y)
    B_LINKER := $(CXX)
endif

NEXUS_APP_INCLUDE_PATHS += \
	$(NEXUS_TOP)/lib/playback_ip/include \
	$(NEXUS_TOP)/lib/os/include/ \
	$(NEXUS_TOP)/lib/os/include/linuxuser \
	$(IP_APPLIB_TEST_INC_PATH) \
    $(BSEAV)/lib/rtp \
    $(BSEAV)/lib/utils \
	$(BSEAV)/lib/mpeg2_ts_parse \
	$(BSEAV)/lib/tspsi \
	$(B_PSIP_TOP)/include \
	$(BSEAV)/api/include \
	$(BSEAV)/lib/netaccel/include \
	$(BSEAV)/lib/media \
	$(BSEAV)/lib/bfile \
	$(NEXUS_TOP)/lib/dtcp_ip/include \

ifeq ($(STREAMER_CABLECARD_SUPPORT),y)
NEXUS_APP_INCLUDE_PATHS += \
	$(B_MPOD_TOP)/include \
	$(B_MPOD_TOP)/src/driver \
	$(B_MPOD_TOP)/src/app_layer \
	$(B_MPOD_TOP)/src/session_layer \
	$(B_MPOD_TOP)/src/link_layer \
	$(B_MPOD_TOP)/src/util

NEXUS_APP_INCLUDE_PATHS += \
	$(B_MPOD_TOP)/../estb_cfg/include	
endif


NEXUS_APP_DEFINES += LINUX BCM_DEBUG
NEXUS_APP_DEFINES += MAJOR_VERSION=0 MINOR_VERSION=0 SUB_VERSION=1

IP_CLIENT_SRCS = \
	ip_client.c \
	ip_psi.c

IP_CLIENT_FCC_SRCS = \
	ip_client_fcc.c


IP_STREAMER_CPP_SRCS = \
	ip_http.c \
	ip_streamer_lib.c \
	ip_streamer_file_src.c \
	ip_psi.c \
	ip_streamer.c

IP_STREAMER_CPP_SRCS += \
	ip_streamer_qam_src.c \
	ip_streamer_streamer_src.c \
	ip_streamer_sat_src.c \
	ip_streamer_ofdm_src.c \
	ip_streamer_vsb_src.c \
	ip_streamer_ip_src.c \
	ip_streamer_ip_dst.c \
	ip_streamer_diags.c \
	ip_streamer_rec_dst.c

ifeq ($(VIDEO_ENCODER_SUPPORT),y)
IP_STREAMER_CPP_SRCS += \
	ip_streamer_hdmi_src.c \
	ip_streamer_transcode.c
endif


ifeq ($(STREAMER_CABLECARD_SUPPORT),y)
IP_STREAMER_CPP_SRCS += \
	ip_strm_cablecard.c
endif


SRCS =

vpath %.cpp $(IP_APPLIB_TEST_SRC_PATH)
vpath %.c $(IP_APPLIB_TEST_SRC_PATH)

IP_CLIENT_OBJS := $(addprefix $(IP_APPLIB_TEST_ODIR)/, $(patsubst %.c,%.o,$(notdir $(IP_CLIENT_SRCS))))
IP_CLIENT_APP = ip_client

IP_CLIENT_FCC_OBJS := $(addprefix $(IP_APPLIB_TEST_ODIR)/, $(patsubst %.c,%.o,$(notdir $(IP_CLIENT_FCC_SRCS))))
IP_CLIENT_FCC_APP = ip_client_fcc

IP_STREAMER_OBJS := $(addprefix $(IP_APPLIB_TEST_ODIR)/, $(patsubst %.c,%.o,$(notdir $(IP_STREAMER_CPP_SRCS))))
IP_STREAMER_APP = ip_streamer

ifeq ($(STREAMER_CABLECARD_SUPPORT),y)
CP_ARCH = mipsel-linux
CP_TOOLCHAIN_DIR:=$(shell dirname $(shell dirname $(shell which ${CP_ARCH}-gcc 2>/dev/null) 2>/dev/null))
CP_CROSS_COMPILE ?= ${CP_ARCH}-
ECM_IF =
ifeq ($(findstring $(PLATFORM), 97420), $(PLATFORM))
ECM_IF = eth2
endif
CDL_CMDS =
DSG_SUPPORT = y
endif

.PHONY: api clean

ifeq ($(findstring $(PLATFORM),93380sms,93383sms), $(PLATFORM))
all: $(IP_STREAMER_APP)
else
all: $(IP_CLIENT_APP) $(IP_STREAMER_APP) $(IP_CLIENT_FCC_APP)
endif

# For linux builds, link to the correct libraries
ifeq ($(B_REFSW_OS),linuxkernel)
LDFLAGS := ${NEXUS_LDFLAGS}
else
ifneq ($(findstring linux,$(B_REFSW_OS)),)
LDFLAGS := -lnexus -L${NEXUS_TOP}/bin -lpthread -lm  $(EXTRA_DRM_OBJS)
endif
endif

LDFLAGS += -ltshdrbuilder

# Suppress LiveMedia Output for non VERBOSE builds
ifeq ($(VERBOSE),)
REDIR_LIVEMEDIA := &> /dev/null
else
REDIR_LIVEMEDIA :=
endif

# This builds the nexus api and other library dependencies
api:

ifneq ($(REFSW_BUILD),y)
	$(Q_)$(MAKE) -C $(NEXUS_TOP)/build
	$(Q_)$(MAKE) -C $(NEXUS_TOP)/lib/os
endif
ifeq ($(LIVEMEDIA_SUPPORT),y)
	$(Q_)$(MAKE) -C $(LIVEMEDIA_DIR) $(REDIR_LIVEMEDIA) 
	$(Q_)$(MAKE) -C $(BLIVE_EXT_DIR)/build
endif

ifeq ($(SSL_SUPPORT),y)
	-@if [ ! -d $(BSEAV)/lib/openssl/$(PLATFORM) ]; then echo "Building OpenSSL lib"; $(MAKE) -C $(OPENSSL_TOP) &> /dev/null; fi
endif #SSL_SUPPORT

ifeq ($(B_PLAYBACK_IP_XML_SUPPORT), y)
	${Q_}$(MAKE) -C $(BSEAV)/lib/mlibs
endif

ifeq ($(DTCP_IP_SUPPORT),y)
	$(Q_)$(MAKE) -C $(NEXUS_TOP)/lib/dtcp_ip install
endif #DTCP_IP_SUPPORT

	$(Q_)$(MAKE) -C $(BSEAV)/lib/tshdrbuilder
	$(Q_)$(MAKE) -C $(NEXUS_TOP)/lib/playback_ip
	$(Q_)$(MAKE) -C $(B_PSIP_TOP)

ifeq ($(STREAMER_CABLECARD_SUPPORT),y)
	@if [ -d $(BSEAV)/docsis/StbHostApps ];then echo ""; else cd $(BSEAV)/docsis;tar fxvz stb_src.tgz;fi
	$(Q_)$(MAKE) -C $(B_MPOD_TOP) DSG_SUPPORT=$(DSG_SUPPORT)
	$(Q_)$(MAKE) -C $(B_MPOD_TOP)/lib/cp/build -f makefile.cplib TOOLCHAIN=uclibc TOOLCHAIN_DIR=$(CP_TOOLCHAIN_DIR) CROSS_COMPILE=$(CP_CROSS_COMPILE)
	$(Q_)$(MAKE) -C $(B_MPOD_TOP)/../estb_cfg
	$(Q_)$(MAKE) -C $(B_MPOD_DRIVERDIR) ARCH=mips CROSS_COMPILE=$(CP_CROSS_COMPILE)
	$(Q_)$(RM) -f $(BSEAV)/docsis/StbHostApps/DsgCC/build/dsgcclib.a
	$(Q_)export CROSSDEV_ROOT=$(CP_TOOLCHAIN_DIR);\
	cd $(BSEAV)/docsis/StbHostApps/DsgCC/build; \
	./build dsgcc pvnet $(ECM_IF) applib cablecard  $(CDL_CMDS);\
	cd $(NEXUS_TOP)/lib/playback_ip/apps;
endif
ifeq ($(SETTOP_API_SUPPORT),y)
	$(Q_)$(MAKE) -C $(BSEAV)/api/build/nexus
endif
ifneq ($(wildcard $(BSEAV)/lib/netaccel/module/Makefile),)
  ifeq ($(NETACCEL_SUPPORT),y)
	$(Q_)$(MAKE) NETACCEL_BUILD_PRIVATE=y IDLE_STATS=y NETACCEL_STANDALONE=y -C $(BSEAV)/lib/netaccel
  endif
	
endif


# This cleans nexus and local apps
clean:
ifneq ($(REFSW_BUILD),y)
	$(Q_)$(MAKE) -C $(NEXUS_TOP)/lib/os clean
	$(Q_)$(MAKE) -C $(NEXUS_TOP)/build clean
endif
	$(Q_)$(MAKE) -C $(NEXUS_TOP)/lib/playback_ip clean
	$(Q_)$(RM) $(BSEAV)/bin/$(IP_CLIENT_APP) $(IP_CLIENT_FCC_APP) $(IP_APPLIB_TEST_ODIR)/*.d $(IP_APPLIB_TEST_ODIR)/*.o $(IP_APPLIB_TEST_ODIR)/*.out;
	$(Q_)$(RM) $(IP_APPLIB_TEST_ODIR)/$(IP_CLIENT_APP) $(IP_CLIENT_FCC_APP)
	$(Q_)$(RM) $(BSEAV)/bin/$(IP_STREAMER_APP)
ifeq ($(LIVEMEDIA_SUPPORT),y)
	$(Q_)$(MAKE) -C $(LIVEMEDIA_DIR) clean $(REDIR_LIVEMEDIA)
	$(Q_)$(MAKE) -C $(BLIVE_EXT_DIR)/build clean
endif
ifeq ($(SETTOP_API_SUPPORT),y)
	$(Q_)$(MAKE) -C $(BSEAV)/api/build/nexus clean
endif
ifeq ($(SSL_SUPPORT),y)
	$(Q_)$(MAKE) -C $(OPENSSL_TOP) clean > /dev/null
endif
ifeq ($(B_PLAYBACK_IP_XML_SUPPORT), y)
	${Q_}$(MAKE) -C $(BSEAV)/lib/mlibs clean
endif
ifeq ($(DTCP_IP_SUPPORT),y)
	$(Q_)$(MAKE) -C $(NEXUS_TOP)/lib/dtcp_ip clean
endif #DTCP_IP_SUPPORT
	$(Q_)$(MAKE) -C $(BSEAV)/lib/tshdrbuilder clean

ifeq ($(STREAMER_CABLECARD_SUPPORT),y)
	$(Q_)$(MAKE) -C $(B_MPOD_TOP)/../estb_cfg clean
	$(Q_)$(MAKE) -C $(B_MPOD_TOP)/lib/cp/build -f makefile.cplib TOOLCHAIN=uclibc clean
	-$(RM) $(B_MPOD_TOP)/lib/cp/build/cplib.lib $(B_CP_LIB)/build/libcp.a
	$(Q_)$(MAKE) -C $(POD_DRIVERDIR) clean ARCH=mips CROSS_COMPILE=$(CP_CROSS_COMPILE)
	$(Q_)$(RM) -f $(BSEAV)/docsis/StbHostApps/DsgCC/build/dsgcclib.a
	$(Q_)export CROSSDEV_ROOT=$(CP_TOOLCHAIN_DIR);\
	cd $(BSEAV)/docsis/StbHostApps/DsgCC/build; \
	./build dsgcc clean;\
	cd $(NEXUS_TOP)/lib/playback_ip/apps;

endif


# This is the minimum needed to compile and link with Nexus
CFLAGS += $(NEXUS_CFLAGS) $(addprefix -I,$(NEXUS_APP_INCLUDE_PATHS)) $(addprefix -D,$(NEXUS_APP_DEFINES))

# Always build with debug
CFLAGS += -MD -g

C_ONLY_FLAGS := -std=c99 -Wstrict-prototypes
CPPFLAGS := -fno-rtti -fno-exceptions


# Implicit rule for building local app
$(IP_CLIENT_APP): odir $(IP_CLIENT_OBJS) api
	@echo [Linking... $< ...]
	$(Q_)$(B_LINKER) -o $(IP_APPLIB_TEST_ODIR)/$@ $(IP_CLIENT_OBJS) $(IP_APPLIB_TEST_LIB_FLAGS) $(LDFLAGS)
	$(Q_)cp -f $(IP_APPLIB_TEST_ODIR)/$(IP_CLIENT_APP) $(NEXUS_TOP)/bin
	@if [ -d $(BSEAV)/bin ]; then \
		cp -f $(IP_APPLIB_TEST_ODIR)/$(IP_CLIENT_APP) $(BSEAV)/bin/ ; \
	fi

# Implicit rule for building local app
$(IP_CLIENT_FCC_APP): odir $(IP_CLIENT_FCC_OBJS) api
	@echo [Linking... $<]
	$(Q_)$(B_LINKER) -o $(IP_APPLIB_TEST_ODIR)/$@ $(IP_CLIENT_FCC_OBJS) $(IP_APPLIB_TEST_LIB_FLAGS) $(LDFLAGS)
	$(Q_)cp -f $(IP_APPLIB_TEST_ODIR)/$(IP_CLIENT_FCC_APP) $(NEXUS_TOP)/bin
	@if [ -d $(BSEAV)/bin ]; then \
		cp -f $(IP_APPLIB_TEST_ODIR)/$(IP_CLIENT_FCC_APP) $(BSEAV)/bin/ ; \
	fi

$(IP_STREAMER_APP): odir $(IP_STREAMER_OBJS) api
	@echo [Linking... $< ....]
	$(Q_)$(B_LINKER) -o $(IP_APPLIB_TEST_ODIR)/$@ $(IP_STREAMER_OBJS) $(IP_APPLIB_TEST_LIB_FLAGS) $(LDFLAGS) 
	$(Q_)cp -f $(IP_APPLIB_TEST_ODIR)/$(IP_STREAMER_APP) $(NEXUS_TOP)/bin
	@if [ -d $(BSEAV)/bin ]; then \
		cp -f $(IP_APPLIB_TEST_ODIR)/$(IP_STREAMER_APP) $(BSEAV)/bin/ ; \
	fi

odir:
	-@if [ ! -d $(IP_APPLIB_TEST_ODIR) ]; then mkdir $(IP_APPLIB_TEST_ODIR); fi


$(IP_APPLIB_TEST_ODIR)/%.o: %.c
	@echo [Compile... $<]
	$(Q_)$(CC) -c -o $@ $< $(CFLAGS) $(C_ONLY_FLAGS)

$(IP_APPLIB_TEST_ODIR)/%.o: %.cpp
	@echo [Compile... $<]
	$(Q_)$(CXX) -c -o $@ $< $(filter-out $(C_ONLY_FLAGS),$(CFLAGS)) $(CPPFLAGS)

install: all
ifneq ($(REFSW_BUILD),y)
ifeq ($(NETACCEL_SUPPORT),y)
	$(Q_)$(MAKE) -C $(BSEAV)/lib/netaccel install
endif
endif

ifeq ($(DTCP_IP_SUPPORT),y)
	@cp -f $(BSEAV)/lib/security/bcrypt/lib/libbcrypt.so $(NEXUS_TOP)/bin
	@cp -f $(BSEAV)/lib/security/common_drm/lib/$(CMNDRM_DIR)/libcmndrm.so $(NEXUS_TOP)/bin 
	@cp -f $(BSEAV)/lib/drmrootfs/lib/linuxuser/libdrmrootfs.so $(NEXUS_TOP)/bin
endif #DTCP_IP_SUPPORT


ifeq ($(SSL_SUPPORT),y)
	@cp -f -P $(BSEAV)/lib/openssl/$(PLATFORM)/lib/libcrypto.so* $(NEXUS_TOP)/bin
	@cp -f -P $(BSEAV)/lib/openssl/$(PLATFORM)/lib/libssl.so* $(NEXUS_TOP)/bin
endif
	@if [ -d $(BSEAV)/bin ]; then \
	    cp -f $(NEXUS_TOP)/bin/*.so $(BSEAV)/bin; \
	    cp -f $(NEXUS_TOP)/bin/*.a $(BSEAV)/bin; \
	    cp -f $(NEXUS_TOP)/bin/ip_* $(BSEAV)/bin; \
	fi


-include $(IP_APPLIB_TEST_ODIR)/*.d

