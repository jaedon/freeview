/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:		$Workfile:   stream_rtsp.h  $
 * Version:			$Revision:   1.0  $
 * Original Author: $
 * Current Author:	$Author: junsy@humaxdigital.com $
 * Date:			$Date: 2012.07.30
 * File Description:	RTSP Stream Controller (WMS-RTSP/Standard-RTSP)
 * Module:
 * Remarks:
 */	

/**
 * @ingroup RTSP_STREAM_CONTROLLER
 */

/**
 * @author Jin Lee(jlee@humaxdigital.com)
 * @date 30 Jul 2012
 */

/**
 * @note
 * Copyright (C) 2012 Humax Corporation. All Rights Reserved. <br>
 * This software is the confidential and proprietary information
 * of Humax Corporation. You may not use or distribute this software
 * except in compliance with the terms and conditions of any applicable license
 * agreement in writing between Humax Corporation and you.
 */

 /*@{*/

/**
 * @file stream_rtsp.h
 */

#ifndef __STREAM_RTSP_H__
#define __STREAM_RTSP_H__

#include "stream_global.h"
#include "mms_asf.h"
#include "rtsp_lib.h" /* struct RTSP_ACCESS_SYS_s is defined */

#define STREAM_RTSP_CMD_OPTION 				"OPTIONS"
#define STREAM_RTSP_CMD_DESCRIBE			"DESCRIBE"
#define STREAM_RTSP_CMD_SETUP				"SETUP"
#define STREAM_RTSP_CMD_SET_PARAMETER	"SET_PARAMETER"
#define STREAM_RTSP_CMD_GET_PARAMETER	"GET_PARAMETER"
#define STREAM_RTSP_CMD_PLAY				"PLAY"
#define STREAM_RTSP_CMD_PAUSE				"PAUSE"
#define STREAM_RTSP_CMD_TEARDOWN			"TEARDOWN"

enum {
	MINIMUM_RESUME_START_TIME = 10,   /* second	 */
	RESUME_BUFFERING_TIME = 3 * 1000, /* milisecond */
};

enum {
	STREAM_CHECK_DATA_TIME = 5,
};

enum {
	AUTO_RETRY_DEFAULT = 10,
};

enum {
	RTSP_STATUS_OK = 200,
	RTSP_STATUS_SET_PARAMETER = 10,
	RTSP_STATUS_OPTIONS = 20,
	RTSP_STATUS_ANNOUNCE = 30,
};

typedef enum {
	RTSP_PROTOCOL_UNKNOWN = 0,
	RTSP_PROTOCOL_STANDARD,
	RTSP_PROTOCOL_WMS,
	RTSP_PROTOCOL_REAL
} RTSP_PROTOCOL_TYPE_e;

struct rtsp_field_t {
	char *field_name;
	struct rtsp_field_t *next;
};


typedef enum 
{
	RTSP_SERVER_Unknown=0,  /* Assumed with v1.0               */
	RTSP_SERVER_Standard,   /* RTSP v1.0 or v2.0 (Unsupported) */
	RTSP_SERVER_WMS,        /* WMS                             */
	RTSP_SERVER_Real        /* Real/helix: Unsupported         */
} RTSP_SERVER_TYPE_e;

/*
 * option to pass to download_streaming function.
 * everything not file pointer and url goes here.
 */
struct download_opts_t 
{
	RTSP_SERVER_TYPE_e expected_server_type;
	HUINT32 i_bandwidth;
	char *p_speed;
	char *p_range;
	char *p_byterange;
	HINT32 i_oobPort; /* RTP or RAW/UDP */
	RTSP_TRANSPORT_TYPE_e data_transport_protocol; /* Interleaved, UDP, UDP/RTP */
};

struct rtsp_ctrl_t 
{
	/* RTSP_SERVER_TYPE_e */
	RTSP_PROTOCOL_TYPE_e rtsp_protocol_type; /* Standard, WMS, Real/Helix */
  
	char *p_mrl;			/* this is rtsp-conpatible URL. rtsp://ss:port/pp  */
	char *p_serverName;
	HUINT32 i_server_state;
	HUINT32 i_server_caps;

	HUINT32 i_cseq;
	HUINT32 i_lastAnswerCSeq;
	char *p_sessionNum;
	HUINT32 i_keepaliveInterval; /* MS-RTSP 3.1.2 min 10 sec max 4294967 sec */
	char *p_etag;
	char *p_challenge; /* only in Real */

	char *p_transport;
	char *p_transDest; /* from Transport field or SDP mediaDesc */
	char *p_transSrc; /* from Transport field or SDP mediaDesc */
	HUINT16 i_rtp_client_port;  /* Transport field */
	HUINT16 i_rtp_server_port;  /* Transport field */
	HUINT16 i_rtcp_client_port; /* Transport field */
	HUINT16 i_rtcp_server_port; /* Transport field */
	char *p_rtpinfo;
	char *p_rtpinfo_url; /* from Transport field or SDP mediaDesc */

	HINT32 interleaved_data_packet_size;
	
	ASF_HEADER_t *pAsfHeaderInfo;
	/*struct rmff_header_t *rmff_header;*/
};


struct RTSP_PROT_HEADER_s 
{
	char *p_protocol;/* RTSP string in request/response */
	char *p_method;
	char *p_uri;
	HUINT32 i_statusCode;
	char *p_reasonPhrase;
	HUINT32 i_rtsp_minor_version;
	HUINT32 i_content_length;
  

	/* field variables */
	struct rtsp_field_t *first_field;
	struct rtsp_field_t *last_field;
	HUINT32 i_field_nb;
	char *field_search;
	struct rtsp_field_t *field_search_pos;
  
	/* body variables */
	char *body;
	size_t body_len;				 /* NOT include '\0' at the end. */
	char *buffer;
	size_t buffer_len;			   /* NOT include '\0' at the end. */
	HUINT32 is_parsed;
};


struct RTSP_PROT_HEADER_s *STREAM_RTSP_NewMessageHeader(void);
struct RTSP_PROT_HEADER_s *STREAM_RTSP_NewMessageHeaderAndFillBasicFields(struct rtsp_ctrl_t *rtsp_ctrl);
void STREAM_RTSP_FreeMessageHeader(struct RTSP_PROT_HEADER_s *h);
char *STREAM_RTSP_FindMessageHeaderField(struct RTSP_PROT_HEADER_s *rtsp_hdr, const char *field_name);
void STREAM_RTSP_SetMessageHeaderField(struct RTSP_PROT_HEADER_s *rtsp_hdr, const char *field_name);
void STREAM_RTSP_SetMessageHeaderURI(struct RTSP_PROT_HEADER_s *rtsp_hdr,const char *uri);
void STREAM_RTSP_SetMessageHeaderMethod(struct RTSP_PROT_HEADER_s *rtsp_hdr, const char *method);
char *STREAM_RTSP_SerializeMessageHeader(struct RTSP_PROT_HEADER_s *rtsp_hdr);

HINT32 STREAM_RTSP_RequestMessageAndFree(struct RTSP_ACCESS_SYS_s *stream, struct RTSP_PROT_HEADER_s *rtsp_hdr);
HINT32 STREAM_RTSP_CheckMessageResponseOk(HINT32 status_code);
HINT32 STREAM_RTSP_ReadMessageAndFreeBody(struct RTSP_ACCESS_SYS_s *stream,struct RTSP_PROT_HEADER_s *rtsp_hdr);
HINT32 STREAM_RTSP_ReadMessageAndParseBasic(struct RTSP_ACCESS_SYS_s *stream,struct RTSP_PROT_HEADER_s *rtsp_hdr);
HINT32 STREAM_RTSP_RequestMessageOK(struct RTSP_ACCESS_SYS_s *stream,HINT32 cseq,char *session);
void STREAM_RTSP_SetRangeField(struct RTSP_PROT_HEADER_s *rtsp_hdr,const char *rangestr);
void STREAM_RTSP_SetSpeedField(struct RTSP_PROT_HEADER_s *rtsp_hdr,const char *speedstr);


/*! 
 *	\brief Open WMS-RTSP stream
 *	\param pAccess: STREAM_ACCESS_t handle
 *	\return HINT32: 1 (success), -1 (failture)
 *	\see 
 */
HINT32  MMSR_Open ( STREAM_ACCESS_t *pAccess );

/*! 
 *	\brief Read WMS-RTSP stream
 *	\param pAccess: STREAM_ACCESS_t handle, p_buffer: buffer, i_len: buffer length
 *	\return ssize_t: reading total size
 *	\see 
 */
ssize_t MMSR_Read ( STREAM_ACCESS_t *pAccess, HUINT8 *p_buffer, size_t i_len );

/*! 
 *	\brief byte seek for WMS-RTSP stream
 *	\param pAccess: STREAM_ACCESS_t handle, i_pos: seek offset
 *	\return HINT32: DI_IPT_SUCCESS if success
 *	\see 
 */
HINT32 MMSR_Seek ( STREAM_ACCESS_t * pAccess, HINT64 i_pos );

/*! 
 *	\brief Control(trick/timed seek) WMS-RTSP stream
 *	\param pAccess: STREAM_ACCESS_t handle, cmd: DI_IPTUNER_CMD_e type command
 *	\return int: DI_IPT_SUCCESS if success
 *	\see 
 */
HINT32 MMSR_Control ( STREAM_ACCESS_t *pAccess, DI_IPTUNER_CMD_e cmd, HUINT64 val, HUINT64 val2);

/*! 
 *	\brief Close WMS-RTSP stream
 *	\param pAccess: STREAM_ACCESS_t handle
 *	\return void
 *	\see 
 */
void MMSR_Close ( STREAM_ACCESS_t *pAccess );

/*! 
 *	\brief Open standard RTSP stream
 *	\param pAccess: STREAM_ACCESS_t handle
 *	\return HINT32: 1 (success), -1 (failture)
 *	\see 
 */
HINT32  STREAM_RTSP_Open ( STREAM_ACCESS_t *pAccess );

/*! 
 *	\brief Read standard RTSP stream
 *	\param pAccess: STREAM_ACCESS_t handle, p_buffer: buffer, i_len: buffer length
 *	\return ssize_t: reading total size
 *	\see 
 */
ssize_t STREAM_RTSP_Read ( STREAM_ACCESS_t *pAccess, HUINT8 *p_buffer, size_t i_len );

/*! 
 *	\brief byte seek for standard RTSP stream
 *	\param pAccess: STREAM_ACCESS_t handle, i_pos: seek offset
 *	\return HINT32: DI_IPT_SUCCESS if success
 *	\see 
 */
HINT32  STREAM_RTSP_Seek ( STREAM_ACCESS_t * pAccess, HINT64 i_pos );

/*! 
 *	\brief Control(trick/timed seek) standard RTSP stream
 *	\param pAccess: STREAM_ACCESS_t handle, cmd: DI_IPTUNER_CMD_e type command
 *	\return int: DI_IPT_SUCCESS if success
 *	\see 
 */
HINT32  STREAM_RTSP_Control ( STREAM_ACCESS_t *pAccess, DI_IPTUNER_CMD_e cmd, HUINT64 val, HUINT64 val2);

/*! 
 *	\brief Close standard RTSP stream
 *	\param pAccess: STREAM_ACCESS_t handle
 *	\return void
 *	\see 
 */
void    STREAM_RTSP_Close ( STREAM_ACCESS_t *pAccess );

#endif /* __STREAM_RTSP_H__ */

