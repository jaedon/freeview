/***************************************************************************
 *     Copyright (c) 2007 Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bmpeg_video_probe.h $
 * $brcm_Revision: 6 $
 * $brcm_Date: 7/5/12 1:47p $
 *
 * Module Description:
 *
 * BMedia library, MPEG Video elementary stream probe
 * 
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/media/bmpeg_video_probe.h $
 * 
 * 6   7/5/12 1:47p vsilyaev
 * SW7425-3414: Merged low_delay flag support in the media probe for MPEG2
 * video.
 * 
 * SW7425-3414/1   7/2/12 6:33p spothana
 * SW7425-3414: Add low_delay flag support in the media probe for MPEG2
 * video.
 * 
 * 5   2/13/12 1:04p vsilyaev
 * SW7425-2298: Added progressive_sequence from the MPEG-2 sequence
 * extension
 * 
 * 4   1/30/12 3:08p vsilyaev
 * SW7425-2298: Added profile and level extraction for MPEG-2 ES streams
 * 
 * 3   3/14/11 5:55p vsilyaev
 * SWDTV-5905: Added count of picture and sequence start codes
 * 
 * 2   1/15/10 2:21p vsilyaev
 * SW7405-3779: Return frame rate for the MPEG-2 video streams
 * 
 * 1   7/20/07 11:56p vsilyaev
 * PR 32813: MPEG1/2 Elementary stream probe
 * 
 * 
 *******************************************************************************/
#ifndef _BMPEG_VIDEO_PROBE_H__
#define _BMPEG_VIDEO_PROBE_H__

#include "bmedia_probe_es.h"

#ifdef __cplusplus
extern "C"
{
#endif
	
extern const bmedia_probe_es_desc bmpeg_video_probe;
typedef struct bmedia_probe_mpeg_video {
    unsigned framerate; /* framerate of video stream, in 1000 FPS, derived from the frame_rate_code, 0 if unknown */
	uint16_t seq_scode_count; /* number of sequence start codes seen when parsing payload */
	uint16_t pic_scode_count; /* number of picture start codes seen when parsing payload */
    uint8_t profile;  /* ISO/IEC 13818-2 , Table 8-2 . Profile identification */
    uint8_t level; /* ISO/IEC 13818-2 , Table 8-3 . Level identification */
    bool progressive_sequence; /*  ISO/IEC 13818-2, Sequence extension, progressive_sequence */ 
    bool low_delay;  /*  ISO/IEC 13818-2, Sequence extension, low_delay */ 
} bmedia_probe_mpeg_video;


#ifdef __cplusplus
}
#endif


#endif /* _BMPEG_VIDEO_PROBE_H__ */

