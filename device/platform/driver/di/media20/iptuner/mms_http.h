/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:		$Workfile:   mms_http.h  $
 * Version:			$Revision:   1.0  $
 * Original Author: $
 * Current Author:	$Author: junsy@humaxdigital.com $
 * Date:			$Date: 2012.07.30
 * File Description:	DI Stream (MMS) Header
 * Module:
 * Remarks:
 */	

/**
 * @ingroup MMS_HTTP_CONTROLLER
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
 * @file mms_http.h
 */

#ifndef __MMS_HTTP_H__
#define __MMS_HTTP_H__

#include "util_url.h"
#include "htype.h"
#include "mms_asf.h"
#include "stream_global.h"
#include "di_iptuner20.h"

typedef struct
{
    HUINT16 i_type;
    HUINT16 i_size;

    HUINT32 i_sequence;
    HUINT16 i_unknown;

    HUINT16 i_size2;

    int      i_data;
    HUINT8  *p_data;

} chunk_t;

#define BUFFER_SIZE 65536
typedef struct {
	int             i_proto;
	int             fd;
	URL_t       	url;

	HBOOL      		b_proxy;
	URL_t       	proxy;

	int             i_request_context;

	HUINT8         buffer[BUFFER_SIZE + 1];

	HBOOL      		b_broadcast;

	HUINT8         *p_header;
	int             i_header;

	HUINT8         *p_packet;
	HUINT32        i_packet_sequence;
	unsigned int    i_packet_used;
	unsigned int    i_packet_length;

	HINT64         i_start;

	unsigned int	i_playlist_gen_id;
	unsigned int	i_client_id;
	unsigned int	i_broadcast_id;

	ASF_HEADER_t    asfh;
	ASF_GUID_t          guid;
} MMSH_ACCESS_SYS_t;


int  MMSH_Open  ( STREAM_ACCESS_t *pAccess );
void MMSH_Close ( STREAM_ACCESS_t *pAccess );
int  MMSH_Seek ( STREAM_ACCESS_t *pAccess, HINT64 );
int MMSH_Control( STREAM_ACCESS_t *pAccess, DI_IPTUNER_CMD_e cmd, HINT32 val );
ssize_t MMSH_Read ( STREAM_ACCESS_t *pAccess, HUINT8 *p_buffer, size_t i_len );

#endif /* __MMS_HTTP_H__ */
