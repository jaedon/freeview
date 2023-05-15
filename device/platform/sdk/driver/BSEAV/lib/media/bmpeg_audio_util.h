/***************************************************************************
 *     Copyright (c) 2007 Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bmpeg_audio_util.h $
 * $brcm_Revision: 7 $
 * $brcm_Date: 11/23/09 7:46p $
 *
 * Module Description:
 *
 * BMedia library, MPEG Audio elementary stream probe
 * 
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/media/bmpeg_audio_util.h $
 * 
 * 7   11/23/09 7:46p vsilyaev
 * SW3548-1609: Distinguish between Layer3 and other streams when
 * selecting sample rate scale
 * 
 * 6   7/17/09 10:06a vsilyaev
 * PR 56734: Separated public and private interfaces on parsing mpeg audio
 * frames
 * 
 * 5   7/16/09 4:03p vsilyaev
 * PR 56734: Streamlined parsing of the VBR header
 * 
 * 4   7/16/09 12:45p vsilyaev
 * PR 56734: Merged probing of VBR MP3 streams
 * 
 * fix_bdvd_v1.5_pr15251/4   7/13/09 9:54a gooslin
 * Change assume_cbr stream type to unknown stream type
 * 
 * fix_bdvd_v1.5_pr15251/3   7/13/09 9:30a gooslin
 * Correct frame info funtion name
 * 
 * fix_bdvd_v1.5_pr15251/2   7/10/09 5:14p gooslin
 * Move mpeg frame parsing to use an independent structure
 * 
 * fix_bdvd_v1.5_pr15251/1   7/9/09 3:36p gooslin
 * Add support for getting MP3 VBR information from the Xing/VBRI header
 * 
 * 3   1/23/09 9:07p vsilyaev
 * PR 51351: Fixed navigation and resycing in CBR streams with variable
 * frame length
 * 
 * 2   1/20/09 5:25p vsilyaev
 * PR 51351: Added code for on-the-fly indexing of MP3 streams
 * 
 * 1   12/10/08 1:25a vsilyaev
 * PR 48768: MPEG Audio parsing routines
 * 
 * 
 *******************************************************************************/
#ifndef _BMPEG_AUDIO_UTIL_H__
#define _BMPEG_AUDIO_UTIL_H__

#include "bmedia_probe_es.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define B_MPEG_AUDIO_SYNC	        0x7FF
#define B_MPEG_AUDIO_SYNC_LEN       2
#define B_MPEG_AUDIO_HEADER         (B_MPEG_AUDIO_SYNC_LEN+2)

#define B_MPEG_AUDIO_LAYER_I        0x03
#define B_MPEG_AUDIO_LAYER_II       0x02
#define B_MPEG_AUDIO_LAYER_III      0x01

#define B_MPEG_AUDIO_VERSION_1      0x03
#define B_MPEG_AUDIO_VERSION_2      0x02
#define B_MPEG_AUDIO_VERSION_2_5    0x00

#define B_MPEG_AUDIO_CHANNEL_MODE_STEREO            0x00
#define B_MPEG_AUDIO_CHANNEL_MODE_JOINT_STEREO      0x01
#define B_MPEG_AUDIO_CHANNEL_MODE_DUAL_MONO         0x02
#define B_MPEG_AUDIO_CHANNEL_MODE_MONO              0x03


/*
Summary:
   This enumeration defines the type of bitrate encoding used to encode the stream
*/
typedef enum bmp3_stream_type {
    bmp3_stream_type_unknown = 0,
    bmp3_stream_type_cbr,
    bmp3_stream_type_vbr,
    bmp3_stream_type_abr
} bmp3_stream_type;

/*
Summary:
   This structure defines MP3 audio specific properties of audio track
*/
typedef struct bmp3_vbr_frame_info {
    bmp3_stream_type stream_type;
    unsigned num_frames;
    unsigned stream_bytes;
    unsigned stream_quality;
} bmp3_vbr_frame_info;

typedef struct bmedia_mpeg_audio_info {
    uint8_t layer;
    uint8_t version;
    uint8_t channel_mode;
    uint16_t samples;
} bmedia_mpeg_audio_info;

void
bmp3_parse_vbr_frame_info(batom_cursor *cursor, const bmedia_mpeg_audio_info *mpeg_info, bmp3_vbr_frame_info *vbr_info);

size_t
bmpeg_audio_probe_parse_header(batom_cursor *cursor, uint16_t sync_word, bmedia_probe_audio *info, bmedia_mpeg_audio_info *mpeg_info);

/*
Summary:
   This structure defines MPEG audio specific properties of audio track
*/

extern const bmedia_probe_es_desc bmpeg_audio_probe;

#ifdef __cplusplus
}
#endif


#endif /* _BMPEG_AUDIO_UTIL_H__ */

