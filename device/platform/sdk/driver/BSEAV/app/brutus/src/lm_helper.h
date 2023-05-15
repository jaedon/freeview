/***************************************************************************
 *     Copyright (c) 2004-2006, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: lm_helper.h $
 * $brcm_Revision: 2 $
 * $brcm_Date: 10/3/06 4:52p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/lm_helper.h $
 * 
 * 2   10/3/06 4:52p jjordan
 * PR24714: Add support for RTSP protocol
 * 
 * 1   7/25/06 5:04p jjordan
 * PR22906: Integrate liveMedia open-source for RTP and RTCP support
 * 
 ***************************************************************************/

/**
Summary:
	lm_helper is a C wrapper around the LiveMedia library.
Description:
	It provides C callable functions for opening, closing and related 
    operations on a liveMedia session. 
	Each LiveMedia session supports a single RTP stream.
**/

#ifndef LM_HELPER_H__
#define LM_HELPER_H__

#include "bsettop_types.h"
#include "BasicUsageEnvironment.hh"
#include "GroupsockHelper.hh"
#include "RTCP.hh"
#include "liveMedia.hh"

struct bplayback_socket_open_params;
struct bplayback_rtsp_ctrl_cmd;

void liveMediaOpen(void);
void liveMediaClose(void);

/* open a livemedia RTP or RTSP session */
void liveMediaSessionOpen(void **context, bplayback_socket_open_params *socket_params);

/* start a livemedia RTP or RTSP session */
void liveMediaSessionStart(void *context, bplaypump_t pump);

/* control an RTSP session (eg FFW, REW, etc) */
void liveMediaSessionRTSPControl(void *context, bplayback_rtsp_ctrl_cmd *cmd);

/* get data from an RTP or RTSP session */
void liveMediaSessionGetData(void *context, void *buffer, unsigned int *buffer_size);

/* stop a livemedia RTP or RTSP session */
void liveMediaSessionStop(void *context);

/* close a livemedia RTP or RTSP session */
void liveMediaSessionClose(void *context);

#endif /* LM_HELPER_H__ */


