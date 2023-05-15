/***************************************************************************
 *     Copyright (c) 2007-2009, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bavi_probe.h $
 * $brcm_Revision: 11 $
 * $brcm_Date: 10/1/09 12:07p $
 *
 * Module Description:
 *
 * BMedia library, stream probe module
 * 
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/avi/bavi_probe.h $
 * 
 * 11   10/1/09 12:07p vsilyaev
 * SW7405-3121: Merge changes for handling not interleaved AVI streams
 * 
 * fix_bdvd_v1.5_pr15387/1   10/1/09 2:08p gooslin
 * Move max_av_byte_delta to the proper probe struct
 * 
 * 10   6/30/09 3:06p vsilyaev
 * PR 56485: Added media_type for AVI tracks
 * 
 * 9   12/1/08 5:55p vsilyaev
 * PR 49826: Add more fields from the AVI headers
 * 
 * 8   11/20/08 12:33p vsilyaev
 * PR 49522: Added frame rate for AVI streams
 * 
 * 7   10/15/08 11:28p vsilyaev
 * PR 47927: Return duration for AVI tracks
 * 
 * 6   9/3/08 5:28p vsilyaev
 * PR46187: extract audio language string
 * 
 * PR46187/2   9/3/08 4:05p jtna
 * PR46187: extract audio language string
 * 
 * PR46187/1   9/2/08 6:58p jtna
 * PR46187: extract audio language string
 * 
 * 5   6/10/08 10:57a gmohile
 * PR 43477 : Checking HD support for subtitles
 * 
 * 4   9/25/07 2:45p gmohile
 * PR 34640 : Add flag to bool to indicate encrypted track
 * 
 * 3   7/25/07 1:50p vsilyaev
 * PR 32813: Separated public and internal API of bmedia_probe
 * 
 * 2   7/17/07 4:30p vsilyaev
 * PR 32813: Added MPEG-2 TS support for the probe library
 * 
 * 1   7/9/07 3:59p vsilyaev
 * PR 32813: AVI probe module
 * 
 *******************************************************************************/
#ifndef _BAVI_PROBE_H__
#define _BAVI_PROBE_H__

#include "bmedia_probe_impl.h"
#include "bavi_util.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef enum bavi_subtitle_type {
	bavi_subtitle_type_dxsa,
	bavi_subtitle_type_dxsb,
	bavi_subtitle_type_none
} bavi_subtitle_type;

typedef struct bavi_probe_stream {
	  bmedia_probe_stream media;
      unsigned video_framerate; /* framerate of video stream, in 1000 FPS, derived from the dwMicroSecPerFrame in AVI mainframe */
      unsigned suggestedBufferSize; /* value of dwSuggestedBufferSize from  the AVI header */
      off_t max_av_byte_delta; /* the maximum byte difference between a single audio and video frame of the same PTS. -1 if unknown */ 
} bavi_probe_stream;

typedef struct bavi_probe_track {
	  bmedia_probe_track media;
	  uint8_t *private_data;
	  unsigned private_data_length;
	  bavi_subtitle_type subtitle_type;
	  unsigned duration; /* duration of stream, in milliseconds or 0 if unknown */
      unsigned suggestedBufferSize; /* value of dwSuggestedBufferSize from  the stream header */
      unsigned averageBitrate; /* for audio streams, this is WAVEFORMATEX.nAvgBytesPerSec*8, otherwise it's 0 */
      uint32_t media_type; /* FOURCC for video track or wFormatTag for audio tracks */
	  bool encryptedContentFlag;
	  char language[16]; 
} bavi_probe_track;
		

extern const bmedia_probe_format_desc bavi_probe;

#ifdef __cplusplus
}
#endif


#endif /* _BAVI_PROBE_H__ */

