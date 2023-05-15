/***************************************************************************
 *     Copyright (c) 2006, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bsettop_playback_rtsp.h $
 * $brcm_Revision: 1 $
 * $brcm_Date: 10/3/06 5:16p $
 *
 * Module Description:
 *
 * RTP playback module
 * 
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/rtp/bsettop_playback_rtsp.h $
 * 
 * 1   10/3/06 5:16p vsilyaev
 * PR 23826: Added bsettop_playback_rtsp
 * 
 * 
 *******************************************************************************/
#ifndef _BSETTOP_PLAYBACK_RTSP_H__
#define _BSETTOP_PLAYBACK_RTSP_H__

#ifdef __cplusplus
extern "C"
{
#endif

bplayback_t bplayback_rtsp_open(void);
bplayback_file_t bplayback_rtsp_open_source(bplayback_t playback, const char *url);

#ifdef __cplusplus
}
#endif

#endif /* _BSETTOP_PLAYBACK_RTSP_H__ */

