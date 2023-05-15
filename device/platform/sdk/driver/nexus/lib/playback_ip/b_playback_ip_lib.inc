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
# $brcm_Workfile: b_playback_ip_lib.inc $
# $brcm_Revision: 55 $
# $brcm_Date: 9/7/12 10:45a $
#
# Module Description:
#
# Revision History:
#
# Created:
#
# $brcm_Log: /nexus/lib/playback_ip/b_playback_ip_lib.inc $
# 
# 55   9/7/12 10:45a jrubio
# SW7346-853: add vlc , needed by bcmindexer.c
# 
# 54   7/19/12 4:05p gskerl
# SW7346-853: Merged from branch SW7346-853 to main.
# 
# SW7346-853/1   6/20/12 6:33p gskerl
# SW7346-853: Added handling for b_playback_ip_mpeg_dash.c and
#  b_playback_ip_xml_helper.cpp files.
# 
# 53   7/16/12 9:33a ssood
# SWANDROID-145: exposed option to enable/disable h/w decryption in HLS
#  protocol using export HLS_HARDWARE_DECRYPTION
# 
# 52   5/31/12 7:20p ssood
# SW7425-2894: modify makefiles to use the latest OpenSSL lib
# 
# 51   5/25/12 4:07p ssood
# SW7425-3042: define LIVEMEDIA_SUPPORT in playback_ip lib inc file so
#  apps dont have to explicitly define in their apps
# 
# 50   5/22/12 6:15p ssood
# SW7425-2894: Ip Applib changes for supporting DTCP-IP with robustness
#  rules
# 
# 49   3/12/12 4:41p gkamosa
# SW7425-2543: Merge support for 3383 SMS platform
# 
# Centaurus_Phase_1.0/1   2/10/12 5:37p jmiddelk
# SW7425-2337: Add support for 3383SMS platform
# 
# 48   9/30/11 9:46a ssood
# SW7346-341: keep netrecord function outside the live streaming so that
#  brutus can build
# 
# 47   9/29/11 6:00p ssood
# SW7346-341: encapsulate nexus dependent changes so that non-nexus
#  platforms continue to build
# 
# 46   6/30/11 9:13a ssood
# SW7231-268: Add FLV container support
# 
# 45   4/22/11 7:47p ssood
# SW7420-1616: pass-in LIVE_STREAMING_SUPPORT flag to ip library
# 
# 44   12/23/10 5:23p ssood
# SW7420-1257: Set RTP Marker bit in last RTP packet belonging to an AV
#  frame to allow receiver distiniguish frame boudnaries for ES streams +
#  changes to include new camera APIs
# 
# 43   12/14/10 12:07p ssood
# SW7420-1217: add support for encryption key parsing, downloading and
#  receiving encrypted content
# 
# 42   11/24/10 2:58p ssood
# SW7420-1257: Video Conferencing Demo: add support for receiving input
#  camera input and stream it out
# 
# 41   10/27/10 8:28p ssood
# SW7420-1217: initial check-in for HLS support
# 
# 40   9/7/10 11:32a ssood
# SW7420-883: define PLAYBACK_IP_SUPPORT by default
# 
# 39   7/30/10 3:19p ssood
# SW7420-883: add support to transcode/pass-thru audio to IP Streamer
# 
# 38   5/18/10 1:30p sgundime
# SW7420-559: set env. variable EROUTER_SUPPORT=n to build dms stand
#  alone with out eRouter support.
# 
# 37   3/4/10 12:17p ssood
# SW7420-561: merge from branch to mainline
# 
# SW7420-561/1   2/16/10 10:11a ssood
# SW7420-561: initial cut of new APIs to meet Media Browser, DMR, &
#  Philips RTSP requirements
# 
# 36   1/29/10 6:12p ismailk
# SW7420-559: Enable EROUTER_SUPPORT flag to enable the WAN unicast
#  ioctl.
# 
# 35   1/18/10 5:02p ssood
# SW7420-454: Add support to create index w/o using the createindex
#  utility
# 
# 35   1/18/10 5:00p ssood
# SW7420-454: Add support to create index w/o using the createindex
#  utility
# 
# 34   12/30/09 9:13p dliu
# SW7405-3400: Add RVU build flag
# 
# 33   12/30/09 1:19a ssood
# SW7420-454: compile in ASF & AVI related files
# 
# 32   12/10/09 7:29p ssood
# SW7420-502: Added compile time flags to enable chunk encoding support
#  on the server
# 
# 31   12/8/09 4:15p ssood
# SW7420-454: use the default nav player
# 
# 30   12/8/09 2:04p ssood
# SW7420-454: add include path for nav indexer & player header files
# 
# 29   12/8/09 1:37p ssood
# SW7420-454: compile nav indexer/player related files
# 
# 28   11/17/09 4:11p ssood
# SW7420-340: include BFILE_STDIO files
# 
# 27   11/17/09 2:58p ssood
# SW7420-454: Enhance IP Streamer to stream files from local disk
# 
# 26   10/20/09 6:33p ssood
# SW7420-340: remove nexus playback, audio, video decoder dependencies
#  for SMS compilation
# 
# 25   8/14/09 4:37p ssood
# PR53773: Adding support for AVI playback over HTTP
# 
# 24   6/24/09 4:38p ssood
# PR53773: simplified the OpenSSL directory path
# 
# 23   6/11/09 10:56a leisun
# PR 55907: Add HW M2M feature for DTCP-IP
# 
# 22   6/10/09 11:02a leisun
# PR 55693: checking in DTCP-IP lib
# 
# 21   6/10/09 11:00a leisun
# PR 55693: checking in DTCP-IP lib
# 
# 20   5/8/09 2:54p lwhite
# PR53373: Removed SSL compilation
# 
# 19   5/6/09 3:36p lwhite
# PR53373: Improved SSL library configuration
# 
# 18   5/5/09 5:38p lwhite
# PR53373: Add SSL library configuration
# 
# 17   5/1/09 12:00p ssood
# PR53773: RAD/EA specific changes: EA data is in not yet SSL encrypted
# 
# 16   4/13/09 10:05a lwhite
# PR53773: fixed dependencies
# 
# 15   4/3/09 3:05p lwhite
# PR53773: Added SSL support
# 
# 14   4/2/09 11:36p ssood
# PR53773 : changes to support security framework for SSL, DTCP-IP,
#  RAD/ES protocols
# 
# 13   11/7/08 2:55p jrubio
# PR48782: adding netrecord functions
# 
# 12   11/5/08 8:54p ssood
# PR48146: Enabling MP4 by default
# 
# 11   10/31/08 12:30p jjordan
# PR47230: Add TTS Pacing
# 
# 10   10/31/08 11:39a ssood
# PR48146: cleaning up support needed for MP4/ASF compilation
# 
# 9   10/28/08 1:43p ssood
# PR48146: conditionally compiling HTTP MP4 support w/ HTTP_MP4_SUPPORT
#  env variable
# 
# 8   10/27/08 11:45a ssood
# PR45431: added support to compile probe interface of bmedia
# 
# 7   8/14/08 12:29p ssood
# PR45431: fixed compilation errors w/ LIVEMEDIA_SUPPORT=n
# 
# 6   8/11/08 10:13a ssood
# PR45431: Add HTTP playback support for ASF content
# 
# 5   8/5/08 1:12p ssood
# PR42739: splitting up UDP & RTP Processing code into separate sub-
#  modules
# 
# 4   7/24/08 1:23p ssood
# PR42739: changes for supporting example app for IP Applib
# 
# 3   7/21/08 4:06p ssood
# PR42739: code changes to simplify ip app lib interface
# 
# 2   6/13/08 7:29p ssood
# PR42739: Adding support for HTTP & RTSP playback
# 
############################################################

#
# Conditional options for this makefile
#
# None at this time
#

#
# Identify this library
#
B_LIBS += B_PLAYBACK_IP_LIB

ifeq ($(CONFIG_VFIO),y)
B_PLAYBACK_IP_LIB_DEFINES += CONFIG_VFIO
B_PLAYBACK_IP_LIB_DEFINES += HUMAX_PLATFORM_BASE
B_PLAYBACK_IP_LIB_DEFINES += CONFIG_BCM_TBD
B_PLAYBACK_IP_LIB_DEFINES += B_HAS_LIVE_STREAMING
endif

export PLAYBACK_IP_SUPPORT=y
ifeq ($(PLAYBACK_IP_SUPPORT),y)
export HTTP_MP4_SUPPORT=y
B_PLAYBACK_IP_LIB_DEFINES += B_HAS_HTTP_MP4_SUPPORT
endif #PLAYBACK_IP_SUPPORT

ifneq ($(LIVEMEDIA_SUPPORT),n)
LIVEMEDIA_SUPPORT=y
export LIVEMEDIA_SUPPORT
endif #LM

ifeq ($(HLS_PROTOCOL_SUPPORT), y)
export HLS_PROTOCOL_SUPPORT=y
export HTTP_AES_SUPPORT=y
B_PLAYBACK_IP_LIB_DEFINES += B_HAS_HLS_PROTOCOL_SUPPORT
B_PLAYBACK_IP_LIB_DEFINES += B_HAS_HTTP_AES_SUPPORT
endif

ifeq ($(MPEG_DASH_PROTOCOL_SUPPORT), y)
export MPEG_DASH_PROTOCOL_SUPPORT=y
export B_PLAYBACK_IP_XML_SUPPORT=y
B_PLAYBACK_IP_LIB_DEFINES += B_HAS_MPEG_DASH_PROTOCOL_SUPPORT
endif

ifeq ($(SMS_GATEWAY), y)
# When compiling for SMS platforms, .inc files of Nexus Playback, Audio, Video modules are not included.
# Thus, we explictly include their paths for header files.
B_PLAYBACK_IP_LIB_DEFINES += B_HAS_SMS_GATEWAY 

ifneq ($(EROUTER_SUPPORT),n)
B_PLAYBACK_IP_LIB_DEFINES += EROUTER_SUPPORT
endif

B_PLAYBACK_IP_LIB_PUBLIC_INCLUDES += $(NEXUS_TOP)/modules/playback/include 
B_PLAYBACK_IP_LIB_PUBLIC_INCLUDES += $(NEXUS_TOP)/modules/video_decoder/${BCHP_CHIP}/include 
B_PLAYBACK_IP_LIB_PUBLIC_INCLUDES += $(NEXUS_TOP)/modules/audio/${BCHP_CHIP}/include
B_PLAYBACK_IP_LIB_DEFINES += NEXUS_HAS_PLAYBACK
endif

#
# Define source files in this library
#
B_PLAYBACK_IP_LIB_SOURCES += \
                    $(B_LIB_TOP)/playback_ip/src/b_playback_ip.c \
                    $(B_LIB_TOP)/playback_ip/src/b_playback_ip_udp.c \
                    $(B_LIB_TOP)/playback_ip/src/b_playback_ip_rtp.c \
                    $(B_LIB_TOP)/playback_ip/src/b_playback_ip_file_streaming.c \
                    $(B_LIB_TOP)/playback_ip/src/b_playback_ip_live_streaming.c \
                    $(B_LIB_TOP)/playback_ip/src/b_playback_ip_utils.c \
                    $(B_LIB_TOP)/playback_ip/src/b_playback_ip_tts_throttle.c \
                    $(B_LIB_TOP)/../../BSEAV/lib/rtp/brtp_spf.c \
                    $(B_LIB_TOP)/../../BSEAV/lib/rtp/btimestamp.c
ifeq ($(HTTP_MP4_SUPPORT), y)
B_PLAYBACK_IP_LIB_SOURCES += \
                    $(B_LIB_TOP)/../../BSEAV/lib/mpeg2_ts_parse/ts_priv.c \
                    $(B_LIB_TOP)/../../BSEAV/lib/mpeg2_ts_parse/ts_psi.c \
                    $(B_LIB_TOP)/../../BSEAV/lib/mpeg2_ts_parse/ts_pmt.c \
                    $(B_LIB_TOP)/../../BSEAV/lib/mpeg2_ts_parse/ts_pat.c \
                    $(B_LIB_TOP)/../../BSEAV/lib/tspsi/tspsi_validate.c
endif

# bcmplayer & index creation related files
B_PLAYBACK_IP_LIB_DEFINES += CMDLINE_TEST 
B_PLAYBACK_IP_LIB_DEFINES += B_IS_SOFTINDEXER 
B_PLAYBACK_IP_LIB_PUBLIC_INCLUDES += $(B_LIB_TOP)/../../BSEAV/lib/bcmplayer/include
B_PLAYBACK_IP_LIB_SOURCES += \
                    $(B_LIB_TOP)/playback_ip/src/b_playback_ip_nav_indexer.c\
                    $(B_LIB_TOP)/../../BSEAV/lib/bcmplayer/src/tsindexer.c\
                    $(B_LIB_TOP)/../../BSEAV/lib/bcmplayer/src/bcmindexer.c

# we dont compile these files for SMS as they have depdendencies on the AV decoder headers which are not included in SMS

ifneq ($(SMS_GATEWAY), y)
B_PLAYBACK_IP_LIB_SOURCES += \
                    $(B_LIB_TOP)/playback_ip/src/b_playback_ip_http.c \
                    $(B_LIB_TOP)/playback_ip/src/b_playback_ip_trickmode.c 
endif

ifeq ($(NEXUS_PLATFORM), 93383sms)
#Special case sms does have decode
B_PLAYBACK_IP_LIB_SOURCES += \
                    $(B_LIB_TOP)/playback_ip/src/b_playback_ip_http.c \
                    $(B_LIB_TOP)/playback_ip/src/b_playback_ip_trickmode.c 
endif


ifeq ($(HLS_PROTOCOL_SUPPORT), y)
B_PLAYBACK_IP_LIB_SOURCES += \
                    $(B_LIB_TOP)/playback_ip/src/b_playback_ip_hls.c
endif

ifeq ($(MPEG_DASH_PROTOCOL_SUPPORT), y)
B_PLAYBACK_IP_LIB_SOURCES += \
                    $(B_LIB_TOP)/playback_ip/src/b_playback_ip_mpeg_dash.c
endif

ifeq ($(B_PLAYBACK_IP_XML_SUPPORT), y)
B_PLAYBACK_IP_LIB_SOURCES += \
                    $(B_LIB_TOP)/playback_ip/src/b_playback_ip_xml_helper.cpp
endif

ifeq ($(HTTP_AES_SUPPORT), y)
B_PLAYBACK_IP_LIB_SOURCES += \
                    $(B_LIB_TOP)/playback_ip/src/b_playback_ip_aes.c
endif

ifeq ($(LIVEMEDIA_SUPPORT), y)
B_PLAYBACK_IP_LIB_SOURCES += \
                    $(B_LIB_TOP)/playback_ip/src/b_playback_ip_lm_helper.cpp \
                    $(B_LIB_TOP)/playback_ip/src/b_playback_ip_rtsp.c \
                    $(B_LIB_TOP)/playback_ip/src/b_playback_ip_lm_rtp_src.cpp
endif

ifeq ($(LIVE_STREAMING_SUPPORT), y)
B_PLAYBACK_IP_LIB_DEFINES += B_HAS_LIVE_STREAMING
endif

ifeq ($(DTCP_IP_SUPPORT), y)
B_PLAYBACK_IP_LIB_DEFINES += B_HAS_DTCP_IP
B_PLAYBACK_IP_LIB_SOURCES += $(B_LIB_TOP)/playback_ip/src/b_playback_ip_dtcp_ip.c
endif

ifeq ($(SSL_SUPPORT), y)
B_PLAYBACK_IP_LIB_DEFINES += B_HAS_SSL
B_PLAYBACK_IP_LIB_SOURCES += $(B_LIB_TOP)/playback_ip/src/b_playback_ip_ssl.c
endif

ifeq ($(RAD_EA_SUPPORT), y)
B_PLAYBACK_IP_LIB_DEFINES += B_HAS_RAD_EA
B_PLAYBACK_IP_LIB_SOURCES += $(B_LIB_TOP)/playback_ip/src/b_playback_ip_rad_ea.c \
                             $(B_LIB_TOP)/playback_ip/src/cryptdes.cpp \
                             $(B_LIB_TOP)/playback_ip/src/radeadecoder.cpp                    
endif

ifeq ($(HTTP_MP4_SUPPORT), y)
#
# Add media related sources
#
MEDIA_MODULES := BFLV_PROBE BMEDIA_PROBE BASF_PROBE BAVI_PROBE BFILE_STDIO 
ifeq ($(SMS_GATEWAY), y)
MEDIA_MODULES += BMEDIA_PLAYER BFILE BFILE_CACHE BFILE_MEDIA B_MEDIA_NAV_PLAYER BASF_PLAYER BAVI_PLAYER
endif
B_PLAYBACK_IP_LIB_SOURCES += $(sort $(foreach module, $(MEDIA_MODULES), $($(module)_SOURCES)))
endif

ifeq ($(IP_STREAMER_DEMO_MODE), y)
B_PLAYBACK_IP_LIB_DEFINES += IP_STREAMER_DEMO_MODE
endif

#
# Define public include path(s) for this library
#
B_PLAYBACK_IP_LIB_PUBLIC_INCLUDES += $(B_LIB_TOP)/os/include $(B_LIB_TOP)/os/include/linuxuser
B_PLAYBACK_IP_LIB_PUBLIC_INCLUDES += $(B_LIB_TOP)/../../BSEAV/lib/netaccel/include
B_PLAYBACK_IP_LIB_PUBLIC_INCLUDES += $(B_LIB_TOP)/../../BSEAV/api/include
B_PLAYBACK_IP_LIB_PUBLIC_INCLUDES += $(B_LIB_TOP)/../../BSEAV/lib/utils
B_PLAYBACK_IP_LIB_PUBLIC_INCLUDES += $(B_LIB_TOP)/../../BSEAV/lib/rtp
B_PLAYBACK_IP_LIB_PUBLIC_INCLUDES += $(B_LIB_TOP)/../../BSEAV/lib/tspsi
B_PLAYBACK_IP_LIB_PUBLIC_INCLUDES += $(B_LIB_TOP)/../../BSEAV/lib/mpeg2_ts_parse
B_PLAYBACK_IP_LIB_PUBLIC_INCLUDES += $(B_LIB_TOP)/../../BSEAV/lib/bfile
B_PLAYBACK_IP_LIB_PUBLIC_INCLUDES += $(B_LIB_TOP)/../../BSEAV/lib/media
B_PLAYBACK_IP_LIB_PUBLIC_INCLUDES += $(B_LIB_TOP)/../../BSEAV/lib/asf
B_PLAYBACK_IP_LIB_PUBLIC_INCLUDES += $(B_LIB_TOP)/../../BSEAV/lib/media/flv
B_PLAYBACK_IP_LIB_PUBLIC_INCLUDES += $(BAVI_INCLUDES)
B_PLAYBACK_IP_LIB_PUBLIC_INCLUDES += $(B_LIB_TOP)/../modules/file/include/priv
B_PLAYBACK_IP_LIB_PUBLIC_INCLUDES += $(B_LIB_TOP)/../../BSEAV/lib/livemedia/blive_ext/include
B_PLAYBACK_IP_LIB_PUBLIC_INCLUDES += $(B_LIB_TOP)/../../BSEAV/lib/livemedia/live/liveMedia/include
B_PLAYBACK_IP_LIB_PUBLIC_INCLUDES += $(B_LIB_TOP)/../../BSEAV/lib/livemedia/live/groupsock/include
B_PLAYBACK_IP_LIB_PUBLIC_INCLUDES += $(B_LIB_TOP)/../../BSEAV/lib/livemedia/live/UsageEnvironment/include
B_PLAYBACK_IP_LIB_PUBLIC_INCLUDES += $(B_LIB_TOP)/../../BSEAV/lib/livemedia/live/BasicUsageEnvironment/include
B_PLAYBACK_IP_LIB_PUBLIC_INCLUDES += $(B_LIB_TOP)/../../magnum/commonutils/lst
B_PLAYBACK_IP_LIB_PUBLIC_INCLUDES += $(B_LIB_TOP)/../../magnum/commonutils/vlc
B_PLAYBACK_IP_LIB_PUBLIC_INCLUDES += $(B_LIB_TOP)/../base/include
B_PLAYBACK_IP_LIB_PUBLIC_INCLUDES += $(B_LIB_TOP)/playback_ip/include
ifeq ($(HTTP_MP4_SUPPORT), y)
B_PLAYBACK_IP_LIB_PUBLIC_INCLUDES += $(sort $(foreach module, $(MEDIA_MODULES), $($(module)_INCLUDES)))
endif
ifeq ($(DTCP_IP_SUPPORT),y)
B_PLAYBACK_IP_LIB_PUBLIC_INCLUDES += $(B_LIB_TOP)/dtcp_ip/include
endif
ifeq ($(MPEG_DASH_PROTOCOL_SUPPORT), y)
B_PLAYBACK_IP_LIB_PUBLIC_INCLUDES += $(B_LIB_TOP)/../../BSEAV/lib/mlibs
endif


#
# Define private include path(s) for this library
#
B_PLAYBACK_IP_LIB_PRIVATE_INCLUDES += $(B_LIB_TOP)/playback_ip/src \
                             $(B_LIB_TOP)/playback_ip/src/$(SAMPLE)

#
# Define other libs that this library is dependent upon.  
# This will check for cyclic dependencies and setup include paths
#
B_PLAYBACK_IP_LIB_DEPENDENCIES := B_OS_LIB

ifeq ($(SSL_SUPPORT), y)
OPENSSL_DIR := $(BSEAV)/lib/openssl/$(PLATFORM)
B_PLAYBACK_IP_LIB_PUBLIC_INCLUDES += $(OPENSSL_DIR)/include
endif

#
# Define extra C definitions
#
#B_PLAYBACK_IP_LIB_DEFINES += ## N/A for this library
ifeq ($(NETACCEL_SUPPORT), y)
B_PLAYBACK_IP_LIB_DEFINES += B_HAS_NETACCEL
endif

ifeq ($(LIVEMEDIA_SUPPORT), y)
B_PLAYBACK_IP_LIB_DEFINES += LIVEMEDIA_SUPPORT
endif

ifeq ($(B_HAS_PLAYPUMP_IP), y)
B_PLAYBACK_IP_LIB_DEFINES += B_HAS_PLAYPUMP_IP
endif

ifeq ($(MEDIA_ASF_SUPPORT), y)
B_PLAYBACK_IP_LIB_DEFINES += B_HAS_ASF
endif

ifeq ($(MEDIA_FLV_SUPPORT), y)
B_PLAYBACK_IP_LIB_DEFINES += B_HAS_FLV
endif

ifeq ($(MEDIA_AVI_SUPPORT), y)
B_PLAYBACK_IP_LIB_DEFINES += B_HAS_AVI
#B_PLAYBACK_IP_LIB_SOURCES += $(sort $(BAVI_PROBE_SOURCES))
endif

ifeq ($(USE_HTTP_CHUNK_ENCODING),y)
B_PLAYBACK_IP_LIB_DEFINES += B_USE_HTTP_CHUNK_ENCODING
endif


ifeq ($(DTCP_IP_HARDWARE_DECRYPTION), y)
B_PLAYBACK_IP_LIB_DEFINES += B_DTCP_IP_HW_DECRYPTION
endif

ifeq ($(DTCP_IP_HARDWARE_ENCRYPTION), y)
B_PLAYBACK_IP_LIB_DEFINES += B_DTCP_IP_HW_ENCRYPTION
endif
ifeq ($(RVU_SUPPORT), y)
B_PLAYBACK_IP_LIB_DEFINES += RVU_SUPPORT
endif

ifeq ($(HLS_HARDWARE_DECRYPTION), y)
B_PLAYBACK_IP_LIB_DEFINES += HW_DEC
endif


B_PLAYBACK_IP_LIB_DEFINES += $(sort $(foreach module, $(MEDIA_MODULES), $($(module)_DEFINES)))
#
# Define possible options - this is only for debug, it does _not_ actually define them.
#
#B_PLAYBACK_IP_LIB_OPTIONS += ## N/A for this library

