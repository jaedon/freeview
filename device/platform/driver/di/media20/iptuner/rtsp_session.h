/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:		$Workfile:   rtsp_session.h  $
 * Version:			$Revision:   1.0  $
 * Original Author:	JungWook Chai
 * Current Author:	$Author: juchae@humaxdigital.com $
 * Date:			$Date: Thur Oct 29 10:20:48 KST 2009 $
 * File Description:	Real-Time STREAMMING IP Tuner Header File
 * Module: 		
 * Remarks:	
 */

/*******************************************************************/
/* Copyright (c) 2009 HUMAX Co., Ltd.							  */
/* All rights reserved.											*/
/*******************************************************************/
#ifndef __RTSP_SESSION_H__
#define __RTSP_SESSION_H__

#include "rtsp_lib.h"


HINT32 RTSP_SESSION_RequestDescribeAndGetSDP(struct RTSP_ACCESS_SYS_s *stream);
HINT32 RTSP_SESSION_RequestSetup(struct RTSP_ACCESS_SYS_s *stream);
HINT32 RTSP_SESSION_RequestPlay(struct RTSP_ACCESS_SYS_s *stream);
#if 0 /* Currently, unused at standard RTSP (OOB channel) */
HINT32 RTSP_SESSION_StartDataChannelIfNotStarted(struct RTSP_ACCESS_SYS_s *stream);
#endif
HINT32 RTSP_SESSION_RequestAsyncPlay(struct RTSP_ACCESS_SYS_s *stream, float scale);
HINT32 RTSP_SESSION_ProcessInterleavedChannel(struct RTSP_ACCESS_SYS_s *stream);
HINT32 RTSP_SESSION_ProcessOobUDPChannel(struct RTSP_CHANNEL_t *pChannel, HUINT8 *p_buffer, size_t i_max_size);
HINT32 RTSP_SESSION_ProcessOobRTPChannel(struct RTSP_CHANNEL_t *pChannel, HUINT8 *p_buffer, size_t i_max_size);

#endif /* __RTSP_SESSION_H__ */
