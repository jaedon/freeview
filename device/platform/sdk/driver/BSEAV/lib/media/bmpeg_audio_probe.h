/***************************************************************************
 *     Copyright (c) 2007 Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bmpeg_audio_probe.h $
 * $brcm_Revision: 4 $
 * $brcm_Date: 7/17/09 10:06a $
 *
 * Module Description:
 *
 * BMedia library, MPEG Audio elementary stream probe
 * 
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/media/bmpeg_audio_probe.h $
 * 
 * 4   7/17/09 10:06a vsilyaev
 * PR 56734: Separated public and private interfaces on parsing mpeg audio
 * frames
 * 
 * 3   12/10/08 1:39a vsilyaev
 * PR 48760: Added ES player for accurate progress monitor and navigation
 * 
 * 2   11/17/08 7:28p vsilyaev
 * PR 49199: Added option to probe files from arbitrary file offset
 * 
 * 1   7/20/07 12:52p vsilyaev
 * PR 32813: Added elementary stream probes
 * 
 * 
 *******************************************************************************/
#ifndef _BMPEG_AUDIO_PROBE_H__
#define _BMPEG_AUDIO_PROBE_H__

#include "bmedia_probe_es.h"
#include "bmpeg_audio_util.h"

#ifdef __cplusplus
extern "C"
{
#endif


extern const bmedia_probe_es_desc bmpeg_audio_probe;
/* custom version of feed function that also parses VBR information */
bmedia_probe_track *bmpeg_audio_probe_feed(bmedia_probe_base_es_t probe, batom_t atom, bmedia_mpeg_audio_info *mpeg_info, bmp3_vbr_frame_info *vbr_info);

#ifdef __cplusplus
}
#endif


#endif /* _BMPEG_AUDIO_PROBE_H__ */

