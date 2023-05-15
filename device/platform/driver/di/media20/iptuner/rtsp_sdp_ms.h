/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:		$Workfile:   rtsp_sdp_ms.h  $
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
#ifndef __RTSP_SDPWMS_H__
#define __RTSP_SDPWMS_H__


struct RTSP_SDP_MS_MediaDesc_s {

  char *p_id;
  HINT32 i_stream_num;
  char *p_control;

};

void RTSP_SDP_MS_FreeSdpMsCtx(struct RTSP_MS_SdpMsCtx_s *p);
struct RTSP_MS_SdpMsCtx_s *RTSP_SDP_MS_ParseSdp(char *data);

#endif /* __RTSP_SDPWMS_H__ */

