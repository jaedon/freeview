/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:		$Workfile:   di_iptuner_rtsp.h  $
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




HINT32 P_RTSP_MS_Setup_and_get_header(struct RTSP_ACCESS_SYS_s *stream,
				  ASF_HEADER_t **pAsfHeaderInfo_ret);
HINT32 RTSP_MS_check_announce_continue(struct RTSP_ACCESS_SYS_s *stream);
HINT32 rtsp_RTSP_MS_get_data(struct RTSP_ACCESS_SYS_s *stream,HUINT8 *buffer,size_t size);
HINT32 RTSP_MS_ReadAnAsfPacket(struct RTSP_ACCESS_SYS_s *stream,
				  HUINT8 *buffer,size_t max_size);
HINT32 RTSP_MS_AsyncPlay(struct RTSP_ACCESS_SYS_s *stream, float scale);
HINT32 RTSP_MS_Play(struct RTSP_ACCESS_SYS_s *stream);
HINT32 RTSP_MS_ProcessRtspResponse(struct RTSP_ACCESS_SYS_s *stream);
HINT32 RTSP_MS_ProcessInterleavedPacket(struct RTSP_ACCESS_SYS_s *stream);
HINT32 RTSP_MS_ProcessRtpPacket(struct RTSP_ACCESS_SYS_s *stream, HUINT8 *buffer,size_t max_size);
HINT32 RTSP_MS_StartDataChannelIfNotStarted(struct RTSP_ACCESS_SYS_s *stream);




#endif /* __RTSP_MS_H__ */
