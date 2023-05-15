############################################################
#     Copyright (c) 2007 Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: bmedia.inc $
# $brcm_Revision: 50 $
# $brcm_Date: 7/9/12 3:48p $
#
# Module Description:
#   Media framework module descriptor
#
# Revision History:
#
# $brcm_Log: /BSEAV/lib/media/bmedia.inc $
# 
# 50   7/9/12 3:48p vsilyaev
# SW7435-256: Use BLST_AAT implementation of AA Tree
# 
# 49   6/21/12 2:53p vsilyaev
# SW7420-2315: Promoted AA tree as separate modules
# 
# 48   3/16/12 4:06p vsilyaev
# SW7231-662: Added AIFF support
# 
# 47   2/23/12 7:53p vsilyaev
# SW7425-2179: Added MJPEG support
# 
# 46   2/21/12 5:11p vsilyaev
# SW7425-2378: Support for Monkey Audio (APE) files
# 
# 45   12/7/11 4:00p vsilyaev
# SW7425-1917: Added player for RMFF content
# 
# 44   11/11/11 5:24p vsilyaev
# SW7425-1752: Added FLAC format support
# 
# 43   8/25/11 6:21p vsilyaev
# SWDTV-8388: Added OGG support
 
# 42   8/2/11 12:07p vsilyaev
# SWDTV-7897, SW7468-69: Can't use eager evaluation since order in what
# .inc files are included is undefined
# 
# 41   7/13/11 8:39p vsilyaev
# SWDTV-7897: Added code to wrap PCM file with the WAV header
# 
# 40   7/11/11 4:57p vsilyaev
# SWDTV-7845: Added WAV player
# 
# 39   5/3/11 11:59a vsilyaev
# SW7425-232: Fixed dependencies
# 
# 38   4/27/11 6:20p vsilyaev
# SW7425-232: Promote ATOM module to separate component
# 
# 37   12/10/10 1:53p gmohile
# SW35230-2517 : Initial version for AVS probe
# 
# 36   10/20/10 11:07a vsilyaev
# SW7422-14: Moved B_MEDIA_PLAYER_NAV to bmedia.inc
# 
# 35   7/14/10 6:10p vsilyaev
# SW3556-1131: Added basic support for CDXA format
# 
# 34   5/17/10 3:07p erickson
# SW7405-3773: fix tspsi_validate path
# 
# 33   5/11/10 1:04p vsilyaev
# SW7405-3773: Fixed kernel mode build
# 
# 32   5/6/10 3:42p vsilyaev
# SW7405-3773: Added support for demuxing fragments from fragmented MP4
# container
# 
# 31   4/2/10 2:23p erickson
# SW7405-4162: bmedia.inc must include BSEAV/lib/mpeg2_ts_parse code used
# by bmpeg2psi_parser.c
#
# 30   2/24/10 4:34p vsilyaev
# SW7468-69: Use lazy binding so do not depnend on order of 'include'
#
# 29   2/2/10 3:51p vsilyaev
# SW7468-69: Added RMFF support
#
# 28   9/29/09 2:52p vsilyaev
# SWDEPRECATED-3624: Added AAC LOAS probe
#
# 27   9/10/09 2:56p erickson
# SW7335-529: add bfile
#
# 26   8/20/09 10:02a gmohile
# PR 55991 : Add PES/VOB trickmode
#
# 25   7/15/09 7:35p vsilyaev
# PR 55653: Added WAV format
#
# 24   7/8/09 11:51a vsilyaev
# PR 55989: Added support of trickmodes
#
# 23   1/13/09 6:42p vsilyaev
# PR 51078: Added code to detect and skip ID3V2 tag
#
# 22   12/10/08 1:38a vsilyaev
# PR 48760: Added ES player for accurate progress monitor and navigation
#
# 21   11/26/08 12:12p vsilyaev
# PR 49646: Use bmedia_probe_util
#
# 20   11/26/08 12:04p vsilyaev
# PR 47650: Adding MP3 ID3V1 and ID3V2 parsers
#
# 19   11/19/08 9:56a gmohile
# PR 48852 : Add DTS probe for ES layer
#
# 18   10/10/08 7:21p jtna
# PR45520: BMedia support for H.264 (AVC) video
#
# 17   10/3/08 4:01p rjlewis
# PR33812: was including file in brutus and nexus (vxworks gets duplicate
# symbols when images combined).
#
# 16   8/29/08 5:48p vsilyaev
# PR 33812: Added support for navigation in the MPEG-2 TS file without an
# index
#
# 15   7/11/08 4:01p vsilyaev
# PR 31188: Added support for FLV probe
#
# 14   7/9/08 11:09a vsilyaev
# PR 33812: Fixed location of btime_indexer.c
#
# 13   3/18/08 3:47p rjlewis
# PR39818: already in Makefile.pvr -- putting it here generates duplicate
# symbols in VxWorks.
#
# 12   3/4/08 3:43p gmohile
# PR 39818 : Fix makefile for MKV support
#
# 11   3/4/08 12:22p vsilyaev
# PR 39818: Added MKV stream probes
#
# 10   3/3/08 12:20p vsilyaev
# PR 39818: Support of MKV format
#
# 9   12/10/07 4:34p vsilyaev
# PR 38073: Added generic (no index player)
#
# 8   8/17/07 3:57p vsilyaev
# PR 33812: Initial version of player of  MPEG2TS streams (no index)
#
# 7   7/24/07 7:21p vsilyaev
# PR 32813: Added MPEG-2 PES probe
#
# 6   7/23/07 7:14p vsilyaev
# PR 32813: Added AAC ADTS probe
#
# 5   7/23/07 11:36a vsilyaev
# PR 32813: Added MPEG-2 TS payload probe
#
# 4   7/21/07 9:53p vsilyaev
# PR 32813: Added MPEG-1 System probe
#
# 3   7/20/07 11:57p vsilyaev
# PR 32813: Added MPEG video probe
#
# 2   7/20/07 1:35p vsilyaev
# PR 32813: Improved usefulnes of sub-modules
#
# 1   7/20/07 12:43p vsilyaev
# PR 32813: Module description file
#
#
############################################################

# Input variables:
#   BASF_XXX - defined in basf.inc
#   BAVI_XXX - defined in bavi.inc
#   BFLV_XXX - defined in bflv.inc
#   BRMFF_XXX - defined in brmff.inc
# Output variables:
#   BMEDIA_SOURCES, BMEDIA_INCLUDES, BMEDIA_CFLAGS - core interface
#   BMEDIA_PLAYER_SOURCES - player sources
#   BMEDIA_FILTER_SOURCES - filter (media -> PES) probe sources
#   BMEDIA_PROBE_SOURCES - probe sources
#   BMEDIA_DEPENDS - modules used by bmedia library

BMEDIA_DEPENDS := BATOM BFILE BFILE_CACHE BFILE_MEDIA 


BMEDIA_INCLUDES = ${BSEAV}/lib/media ${BATOM_INCLUDES} ${BAVI_INCLUDES} ${BASF_INCLUDES} ${BFLV_INCLUDES} ${BRMFF_INCLUDES} ${BSEAV}/lib/mpeg2_ts_parse ${BSEAV}/lib/tspsi
BMEDIA_DEFINES = ${BAVI_DEFINES} ${BASF_DEFINES} ${BFLV_DEFINES} ${BRMFF_DEFINES}

# Core module
BMEDIA_SOURCES = \
	${BSEAV}/lib/media/bmedia_util.c \
	${BAVI_SOURCES} ${BASF_SOURCES}

# MP4 player
B_MEDIA_FILTER_MP4_SOURCES :=  \
	${BSEAV}/lib/media/bmp4_fragment_demux.c \
	${BSEAV}/lib/media/bmp4_parser.c \
	${BSEAV}/lib/media/bmp4_util.c \
	${BSEAV}/lib/media/bmpeg4_util.c

# FILTER module
BMEDIA_FILTER_SOURCES = \
    ${BSEAV}/lib/media/baiff_filter.c \
    ${BSEAV}/lib/media/baiff_util.c \
    ${BSEAV}/lib/media/bape_util.c \
    ${BSEAV}/lib/media/bflac_parser.c \
    ${BSEAV}/lib/media/bflac_util.c \
    ${BSEAV}/lib/media/bmedia_filter.c \
    ${BSEAV}/lib/media/bmedia_pes.c \
    ${BSEAV}/lib/media/bmedia_es.c \
	${BSEAV}/lib/media/bmedia_mjpeg.c \
    ${BSEAV}/lib/media/bmpeg1_parser.c  \
    ${BSEAV}/lib/media/bogg_parser.c \
    ${BSEAV}/lib/media/bogg_util.c \
    ${BSEAV}/lib/media/briff_parser.c \
    ${BSEAV}/lib/media/bwav_filter.c \
    ${BAVI_FILTER_SOURCES} ${BASF_FILTER_SOURCES} ${BFLV_FILTER_SOURCES} ${BRMFF_FILTER_SOURCES} ${B_MEDIA_FILTER_MP4_SOURCES} \

BMEDIA_FILTER_INCLUDES = ${BSEAV}/lib/bfile

# Player modules
B_MEDIA_PLAYER_CORE_SOURCES := \
	${BSEAV}/lib/media/bape_player.c \
	${BSEAV}/lib/media/bid3_parser.c \
	${BSEAV}/lib/media/bmedia_index.c \
	${BSEAV}/lib/media/bmedia_player.c \
	${BSEAV}/lib/media/bmedia_player_es.c \
	${BSEAV}/lib/media/bmedia_player_generic.c \
	${BSEAV}/lib/media/bmpeg_audio_util.c \
	${BSEAV}/lib/media/bwav_player.c

# MP4 player
B_MEDIA_PLAYER_MP4_SOURCES :=  \
	${BSEAV}/lib/media/bmp4_track.c \
	${BSEAV}/lib/media/bmp4_player.c 

# MKV player
B_MEDIA_PLAYER_MKV_SOURCES :=  \
	${BSEAV}/lib/media/bmkv_parser.c  \
	${BSEAV}/lib/media/bmkv_file_parser.c  \
	${BSEAV}/lib/media/bmkv_player.c  \
	${BSEAV}/lib/media/bmkv_util.c

#  MPEG-2 NAV Player (requires also ${BSEAV}/lib/bcmplayer/bcmplayer.inc )
B_MEDIA_NAV_PLAYER_SOURCES := ${BSEAV}/lib/media/bmedia_player_nav.c 
B_MEDIA_NAV_PLAYER_DEFINES := B_HAS_NAV_PLAYER=1

# No index/timestamp based player
B_MEDIA_PLAYER_TIME_SOURCES := \
	${BSEAV}/lib/media/bmpeg2ts_parser.c \
	${BSEAV}/lib/media/bmpeg2ts_player.c \
	${BSEAV}/lib/media/bmpeg2pes_parser.c \
	${BSEAV}/lib/media/bmpeg2pes_player.c \
	${BSEAV}/lib/media/btime_indexer.c \
	${BSEAV}/lib/otfpvr/botf_marker.c 
B_MEDIA_PLAYER_TIME_INCLUDES := ${BSEAV}/lib/otfpvr


BMEDIA_PLAYER_SOURCES = ${B_MEDIA_PLAYER_CORE_SOURCES} ${B_MEDIA_PLAYER_MP4_SOURCES} ${B_MEDIA_PLAYER_MKV_SOURCES} ${B_MEDIA_PLAYER_TIME_SOURCES} \
    ${BASF_PLAYER_SOURCES} ${BAVI_PLAYER_SOURCES} ${BRMFF_PLAYER_SOURCES}

BMEDIA_PLAYER_INCLUDES = ${BMEDIA_INCLUDES} ${B_MEDIA_PLAYER_TIME_INCLUDES}
BMEDIA_PLAYER_DEFINES = ${BMEDIA_DEFINES}


# Probe MPEG2 TS module
B_MEDIA_PROBE_MPEG2TS_SOURCES := \
	${BSEAV}/lib/media/bmpeg2pes_parser.c \
	${BSEAV}/lib/media/bmpeg2psi_parser.c \
	${BSEAV}/lib/media/bmpeg2ts_probe.c \
	${BSEAV}/lib/media/bmpeg2pes_probe.c \
	${BSEAV}/lib/media/bmpeg2ts_psi_probe.c \
	${BSEAV}/lib/mpeg2_ts_parse/ts_pmt.c \
	${BSEAV}/lib/mpeg2_ts_parse/ts_pat.c \
	${BSEAV}/lib/mpeg2_ts_parse/ts_priv.c \
	${BSEAV}/lib/mpeg2_ts_parse/ts_psi.c \
	${BSEAV}/lib/mpeg2_ts_parse/tspsi_validate.c

B_MEDIA_PROBE_ES_SOURCES := \
	${BSEAV}/lib/media/bac3_probe.c \
	${BSEAV}/lib/media/baac_adts_probe.c \
	${BSEAV}/lib/media/baac_loas_probe.c \
	${BSEAV}/lib/media/bavc_video_probe.c \
	${BSEAV}/lib/media/bavs_video_probe.c \
	${BSEAV}/lib/media/bdts_probe.c \
	${BSEAV}/lib/media/bmedia_probe_es.c \
	${BSEAV}/lib/media/bmp3_probe.c \
	${BSEAV}/lib/media/bmpeg_audio_probe.c \
	${BSEAV}/lib/media/bmpeg_video_probe.c

# Probe module
BMEDIA_PROBE_SOURCES = \
    ${BSEAV}/lib/media/baiff_probe.c \
	${BSEAV}/lib/media/bape_probe.c \
	${BSEAV}/lib/media/bflac_probe.c \
	${BSEAV}/lib/media/bmedia_probe.c \
	${BSEAV}/lib/media/bmedia_probe_demux.c \
	${BSEAV}/lib/media/bmedia_probe_util.c \
	${BSEAV}/lib/media/bmedia_cdxa.c \
	${BSEAV}/lib/media/bmedia_pcm.c \
	${BSEAV}/lib/media/bmkv_probe.c \
	${BSEAV}/lib/media/bmp4_probe.c \
	${BSEAV}/lib/media/bmpeg1_probe.c \
	${BSEAV}/lib/media/bogg_probe.c \
	${BSEAV}/lib/media/bwav_probe.c \
	${B_MEDIA_PROBE_ES_SOURCES} \
	${B_MEDIA_PROBE_MPEG2TS_SOURCES} \
	${BFLV_PROBE_SOURCES} \
	${BAVI_PROBE_SOURCES} \
	${BASF_PROBE_SOURCES} \
	${BRMFF_PROBE_SOURCES}

BMEDIA_PROBE_INCLUDES = ${BMEDIA_INCLUDES}
BMEDIA_PROBE_DEFINES = ${BMEDIA_DEFINES}

ifeq ($(HUMAX_PLATFORM_BASE),y)
ifeq ($(CONFIG_MEDIA20),y)
ifeq ($(MSDRM_PRDY_SUPPORT), y)
ifeq ($(CONFIG_PLAYREADY_25),y)
include $(BSEAV)/lib/media/playready25.inc
else
#BMEDIA_SOURCES += $(BSEAV)/lib/playready/prdy_http.c
include $(BSEAV)/lib/media/playready.inc
endif
endif
endif
endif

