/***************************************************************************
 *     (c)2003-2009 Broadcom Corporation
 *  
 *  This program is the proprietary software of Broadcom Corporation and/or its licensors,
 *  and may only be used, duplicated, modified or distributed pursuant to the terms and
 *  conditions of a separate, written license agreement executed between you and Broadcom
 *  (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
 *  no license (express or implied), right to use, or waiver of any kind with respect to the
 *  Software, and Broadcom expressly reserves all rights in and to the Software and all
 *  intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
 *  HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
 *  NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.  
 *   
 *  Except as expressly set forth in the Authorized License,
 *   
 *  1.     This program, including its structure, sequence and organization, constitutes the valuable trade
 *  secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
 *  and to use this information only in connection with your use of Broadcom integrated circuit products.
 *   
 *  2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS" 
 *  AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR 
 *  WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO 
 *  THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES 
 *  OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, 
 *  LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION 
 *  OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF 
 *  USE OR PERFORMANCE OF THE SOFTWARE.
 *  
 *  3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS 
 *  LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR 
 *  EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR 
 *  USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF 
 *  THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT 
 *  ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE 
 *  LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF 
 *  ANY LIMITED REMEDY.
 * 
 * $brcm_Workfile: bmedia_types.h $
 * $brcm_Revision: 17 $
 * $brcm_Date: 6/4/12 5:52p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/media/bmedia_types.h $
 * 
 * 17   6/4/12 5:52p jgarrett
 * SW7425-3065: Adding BD audio codecs to PSI parser
 * 
 * 17   6/4/12 5:51p jgarrett
 * SW7425-3065: Adding BD audio codecs to PSI parser
 * 
 * 16   3/16/12 4:06p vsilyaev
 * SW7231-662: Added AIFF support
 * 
 * 15   2/21/12 5:11p vsilyaev
 * SW7425-2378: Support for Monkey Audio (APE) files
 * 
 * 14   1/20/12 1:00p vsilyaev
 * SW7425-2178: Added MJPEG video codec
 * 
 * 13   12/29/11 3:12p vsilyaev
 * SW7231-500: Merged IEEE-1394 LPCM support
 * 
 * SW7231-500/1   12/29/11 1:11p jgarrett
 * SW7231-500: Adding IEEE-1394 LPCM support
 * 
 * 12   11/11/11 5:24p vsilyaev
 * SW7425-1752: Added FLAC format support
 * 
 * 11   8/25/11 6:21p vsilyaev
 * SWDTV-8388: Added OGG support
 * 
 * 10   8/15/11 3:11p vsilyaev
 * SW7425-938: Added mapping for SH263 and VP8 video codecs
 * 
 * 9   7/6/11 6:40p vsilyaev
 * FWAVD-138: Added support for VP8 video
 * 
 * 8   7/1/11 5:43p vsilyaev
 * SWDTV-7732: Added vorbis support
 * 
 * 7   3/2/11 12:19p vsilyaev
 * SWBLURAY-24399: Merged DTS-CD support
 * 
 * fix_bdvd_v3.0_SWBLURAY-24399/1   2/18/11 1:43p ialauder
 * Add DTS-CD enum
 * 
 * 6   12/14/10 1:31p gmohile
 * SW7422-134 : Add SVC/MVC probe
 * 
 * 5   10/27/10 2:35p vsilyaev
 * SW35230-1952: Added support for MS ADPCM audio
 * 
 * 4   7/14/10 6:10p vsilyaev
 * SW3556-1131: Added basic support for CDXA format
 * 
 * 3   5/6/10 3:42p vsilyaev
 * SW7405-3773: Added support for demuxing fragments from fragmented MP4
 * container
 * 
 * 2   2/2/10 3:51p vsilyaev
 * SW7468-69: Added RMFF support
 * 
 * 1   9/10/09 12:16p vsilyaev
 * SW7335-529: Use bmedia_types for container, video and audio types
 * 
 * 9   9/1/09 4:06p vsilyaev
 * SW3548-2442: Added support for AMR audio
 * 
 * 8   7/15/09 7:25p vsilyaev
 * PR 55653: Added WAV format
 * 
 * 7   2/4/09 10:31a vsilyaev
 * PR 46190: Fixed typo
 * 
 * 6   2/4/09 10:10a vsilyaev
 * PR 46190: Added PCM audio type
 * 
 * 5   7/24/08 11:05a erickson
 * PR44143: changing default aac-he format
 * 
 * 4   7/11/08 5:56p vsilyaev
 * PR 44727: Added softcodec types
 * 
 * 3   3/12/08 2:31p katrep
 * PR40514:Add support for AVS codecs
 * 
 * 2   3/3/08 2:03p vsilyaev
 * PR 39818: Support of MKV format
 * 
 * 1   1/18/08 2:36p jgarrett
 * PR 38808: Merging to main branch
 * 
 * Nexus_Devel/3   11/13/07 3:29p vsilyaev
 * PR 37015: Fixed dependency on sys/types.h
 * 
 * Nexus_Devel/2   11/12/07 1:05p vsilyaev
 * PR 35824: Added more media modules
 * 
 * Nexus_Devel/1   10/10/07 4:55p vsilyaev
 * PR 35824: Playback module
 * 
 * 48   8/22/07 5:29p jgarrett
 * PR 32044: Merging to main branch
 * 
 ***************************************************************************/
#ifndef BMEDIA_TYPES_H__
#define BMEDIA_TYPES_H__

#ifdef __cplusplus
extern "C"
{
#endif

/*
Summary:
    MPEG2 Transport types
*/
typedef enum bstream_mpeg_type {
    bstream_mpeg_type_unknown,  /* unknown or not supported stream format */
    bstream_mpeg_type_es,       /* Elementary stream */
    bstream_mpeg_type_bes,      /* Broadcom elementary stream */
    bstream_mpeg_type_pes,      /* Packetized elementary stream */
    bstream_mpeg_type_ts,       /* Transport stream */
    bstream_mpeg_type_dss_es,
    bstream_mpeg_type_dss_pes,
    bstream_mpeg_type_vob,      /* video object, used with DVD */
    bstream_mpeg_type_asf,      /* Advanced Systems Format */
    bstream_mpeg_type_avi,      /* Audio Video Interleave */
    bstream_mpeg_type_mpeg1,    /* MPEG1 System Stream */
    bstream_mpeg_type_mp4,      /* MP4 (MPEG-4 Part12) container */
    bstream_mpeg_type_flv,      /* Flash video container */
    bstream_mpeg_type_mkv,      /* Matroska container */
    bstream_mpeg_type_wav,      /* WAVE audio container */
    bstream_mpeg_type_rmff,     /* RealMedia File Format container */
    bstream_mpeg_type_mp4_fragment, /* portions of MPEG4 Part12 fragmented stream */
    bstream_mpeg_type_ogg,      /* The Ogg Encapsulation Format */
    bstream_mpeg_type_flac,     /* The FLAC Encapsulation Format */
    bstream_mpeg_type_ape,      /* Monkey's Audio */
    bstream_mpeg_type_aiff,     /* Audio Interchange File Format */
    bstream_mpeg_type_cdxa      /* CDXA RIFF file */
} bstream_mpeg_type;

/*
Summary:
    Digital audio formats
Description:
    Values 0, 3 and 4 are reservered for baudio_format_mpeg, even though baudio_format_mpeg == 0x3.
        3 is the value for MPEG1 audio (ISO/IEC 11172-3).
        4 is the value for MPEG2 audio (ISO/IEC 13818-3).
*/
typedef enum baudio_format {
   baudio_format_unknown = 0,           /* unknown/not supported audio format */
   baudio_format_mpeg = 0x3,        /* MPEG1/2, layer 1/2. This does not support layer 3 (mp3). */
   baudio_format_mp3 = 0x1,         /* MPEG1/2, layer 3. */
   baudio_format_aac = 0xF,         /* Advanced audio coding. Part of MPEG-4 */
   baudio_format_aac_plus = 0x11,   /* AAC plus SBR. aka MPEG-4 High Efficiency (AAC-HE) */
   baudio_format_aac_plus_adts = 0x12,  /* AAC plus SBR. aka MPEG-4 High Efficiency (AAC-HE), with ADTS (Audio Data Transport Format) */
   baudio_format_aac_plus_loas = 0x11,  /* AAC plus SBR. aka MPEG-4 High Efficiency (AAC-HE), with LOAS (Low Overhead Audio Stream) */
   baudio_format_ac3 = 0x81,        /* Dolby Digital AC3 audio */
   baudio_format_ac3_plus = 0x6,    /* Dolby Digital Plus (AC3+ or DDP) audio */
   baudio_format_dts = 0x82,        /* Digital Digital Surround sound. */
   baudio_format_lpcm_hddvd = 0x83, /* LPCM, HD-DVD mode */
   baudio_format_lpcm_bluray = 0x84,/* LPCM, Blu-Ray mode */
   baudio_format_dts_hd = 0x85,     /* Digital Digital Surround sound, HD */
   baudio_format_wma_std = 0x86,    /* WMA Standard */  
   baudio_format_wma_pro = 0x87,    /* WMA Professional */  
   baudio_format_lpcm_dvd = 0x88,   /* LPCM, DVD mode */
   baudio_format_g726 = 0x89,       /* G.726 ITU-T ADPCM  */
   baudio_format_adpcm = 0x8A,      /* MS ADPCM Format */
   baudio_format_dvi_adpcm = 0x8B,  /* DVI ADPCM Format */
   baudio_format_avs = 0x43,        /* AVS Audio */
   baudio_format_pcm,
   baudio_format_amr,
   baudio_format_vorbis,            
   baudio_format_flac,            
   baudio_format_ape,            
   baudio_format_dra = 0xda,
   baudio_format_cook = 0xf0,
   baudio_format_dts_cd = 0x8C,
   baudio_format_lpcm_1394 = 0x8d,  /* IEEE-1394 LPCM */
   baudio_format_mlp = 0x8e         /* Dolby True-HD */
} baudio_format;

/*
Summary:
    Codec type for digital video 
*/
typedef enum bvideo_codec {
    bvideo_codec_none = 0,              /* not coded video */ 
    bvideo_codec_unknown = 0,           /* unknown/not supported video codec */
    bvideo_codec_mpeg1 = 1,             /* MPEG-1 Video (ISO/IEC 11172-2) */
    bvideo_codec_mpeg2 = 2,             /* MPEG-2 Video (ISO/IEC 13818-2) */
    bvideo_codec_mpeg4_part2 = 0x10,    /* MPEG-4 Part 2 Video */
    bvideo_codec_h263 = 0x1A,           /* H.263 Video. The value of the enum is not based on PSI standards. */
    bvideo_codec_h264 = 0x1B,           /* H.264 (ITU-T) or ISO/IEC 14496-10/MPEG-4 AVC */
    bvideo_codec_h264_svc = 0x1F,       /* H.264 (ITU-T) or ISO/IEC 14496-10/MPEG-4 AVC Annex G*/
    bvideo_codec_h264_mvc = 0x20,       /* H.264 (ITU-T) or ISO/IEC 14496-10/MPEG-4 AVC Annex H*/
    bvideo_codec_vc1 = 0xEA,            /* VC-1 Advanced Profile */
    bvideo_codec_vc1_sm = 0xEB,         /* VC-1 Simple&Main Profile */
    bvideo_codec_divx_311 = 0x311,      /* DivX 3.11 coded video */
    bvideo_codec_avs = 0x42,            /* AVS video */
    bvideo_codec_spark = 0xF0,          /* H.263 Sorenson Spark */
    bvideo_codec_vp6=0xF1,
    bvideo_codec_rv40=0xF2,
    bvideo_codec_vp8=0xF3,
    bvideo_codec_mjpeg=0xF4
} bvideo_codec;

#ifdef __cplusplus
}
#endif

#endif /* BMEDIA_TYPES_H__*/

