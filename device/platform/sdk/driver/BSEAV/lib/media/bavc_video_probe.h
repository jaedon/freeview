/***************************************************************************
 *     Copyright (c) 2008 Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bavc_video_probe.h $
 * $brcm_Revision: 5 $
 * $brcm_Date: 4/12/12 4:03p $
 *
 * Module Description:
 *
 * BMedia library, H.264/MPEG-4 Part 10 video elementary stream probe
 * 
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/media/bavc_video_probe.h $
 * 
 * 5   4/12/12 4:03p vsilyaev
 * SW7425-2544: Extract frame_mbs_only instead of ct_type
 * 
 * 4   3/5/12 5:44p vsilyaev
 * SW7425-2544: Added code to extract 'ct_type' from the H264 headers
 * 
 * 3   11/26/08 12:12p vsilyaev
 * PR 49646: Use bmedia_probe_util
 * 
 * 2   11/25/08 1:13a vsilyaev
 * PR 49646: Added code to parse and print H264 profile/level information
 * 
 * 1   10/10/08 7:16p jtna
 * PR45520: BMedia support for H.264 (AVC) video
 * 
 * 
 *******************************************************************************/
#ifndef _BAVC_VIDEO_PROBE_H__
#define _BAVC_VIDEO_PROBE_H__

#include "bmedia_probe_es.h"
#include "bmedia_probe_util.h"

#ifdef __cplusplus
extern "C"
{
#endif
/*
Summary:
   This structure defines H264 specific properties of video track
*/
typedef struct bmedia_probe_h264_video {
    b_h264_video_sps sps;
    bool frame_mbs_only;  /* value of  frame_mbs_only_flag from ITU-T Rec. H.264 */
} bmedia_probe_h264_video;

	
extern const bmedia_probe_es_desc bavc_video_probe;

#ifdef __cplusplus
}
#endif


#endif /* _BAVC_VIDEO_PROBE_H__ */

