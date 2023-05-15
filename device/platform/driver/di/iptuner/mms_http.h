/*****************************************************************************
 * mmsh.h:
 *****************************************************************************
 * Copyright (C) 2001, 2002 the VideoLAN team
 * $Id$
 *
 * Authors: Laurent Aimar <fenrir@via.ecp.fr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston MA 02110-1301, USA.
 *****************************************************************************/

#ifndef __MMS_HTTP_H__
#define __MMS_HTTP_H__

#include "util_url.h"
#include "htype.h"
#include "mms_asf.h"
#include "stream_global.h"
#include "di_iptuner.h"

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

void MMSH_SetUserAgentInfo (HINT8 *pUserAgentInfo);

#endif /* __MMS_HTTP_H__ */
