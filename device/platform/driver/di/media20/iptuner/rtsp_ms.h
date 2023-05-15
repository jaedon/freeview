/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:		$Workfile:   rtsp_ms.h  $
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
#ifndef __RTSP_MS_H__
#define __RTSP_MS_H__

#include "rtsp_lib.h"

HINT32 RTSP_MS_RequestDescribeAndGetHeader(struct RTSP_ACCESS_SYS_s *stream, ASF_HEADER_t **pAsfHeaderInfo_ret);
HINT32 RTSP_MS_RequestSetup(struct RTSP_ACCESS_SYS_s *stream);
HINT32 RTSP_MS_RequestPlay(struct RTSP_ACCESS_SYS_s *stream);
HINT32 RTSP_MS_StartDataChannelIfNotStarted(struct RTSP_ACCESS_SYS_s *stream);
HINT32 RTSP_MS_RequestAsyncPlay(struct RTSP_ACCESS_SYS_s *stream, float scale);

HINT32 RTSP_MS_ProcessInterleavedPacket(struct RTSP_ACCESS_SYS_s *stream);
HINT32 RTSP_MS_ProcessRtpPacket(struct RTSP_ACCESS_SYS_s *stream, HUINT8 *buffer,size_t max_size);

#endif /* __RTSP_MS_H__ */
