/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:		$Workfile:   rtsp_sdp.h  $
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
#ifndef __RTSP_SDP_H__
#define __RTSP_SDP_H__

#include "rtsp_lib.h" /* struct RTSP_SDP_Context_s is defined */

struct RTSP_SDP_MediaDesc_s 
{
  char *p_id;
  HINT32 i_stream_num;
  char *p_control;
};

void RTSP_SDP_FreeContext(struct RTSP_SDP_Context_s *p);
struct RTSP_SDP_Context_s *RTSP_SDP_ParseDescription(char *data);

#endif /* __RTSP_SDP_H__ */

