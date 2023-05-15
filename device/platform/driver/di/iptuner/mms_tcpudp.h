/*****************************************************************************
 * mms.h: MMS access plug-in
 *****************************************************************************
 * Copyright (C) 2001, 2002 the VideoLAN team
 * $Id: 527c14cb535154362ae60c311384bb22cea27cb0 $
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

#ifndef __MMS_TCPUDP_H__
#define __MMS_TCPUDP_H__ 

#include "htype.h"
#include "util_url.h"
#include "util_getaddrinfo.h"
#include "mms_asf.h"
#include "stream_global.h"
#include "di_iptuner.h"

#include <pthread.h>

#define MMS_PROTO_AUTO 0
#define MMS_PROTO_TCP 1
#define MMS_PROTO_UDP 2

#define MMS_PACKET_ANY          0
#define MMS_PACKET_CMD          1
#define MMS_PACKET_HEADER       2
#define MMS_PACKET_MEDIA        3
#define MMS_PACKET_UDP_TIMING   4

#define MMS_CMD_HEADERSIZE  48

#define MMS_BUFFER_SIZE 1000000

typedef struct mmstu_keepalive_thread_t mmstu_keepalive_thread_t;

typedef struct {
	int         i_proto;        /* MMS_PROTO_TCP, MMS_PROTO_UDP */
	int         i_handle_tcp;   /* TCP socket for communication with server */
	int         i_handle_udp;   /* Optional UDP socket for data(media/header packet) */
	/* send by server */
	char        sz_bind_addr [NI_MAXNUMERICHOST]; /* used by udp */
	URL_t   	url;
	ASF_HEADER_t    asfh;

	unsigned    i_timeout;

	/* */
	HUINT8     buffer_tcp[MMS_BUFFER_SIZE];
	int        i_buffer_tcp;

	HUINT8     buffer_udp[MMS_BUFFER_SIZE];
	int        i_buffer_udp;

	/* data necessary to send data to server */
	ASF_GUID_t      guid;
	int        i_command_level;
	int        i_seq_num;
	HUINT32    i_header_packet_id_type;
	HUINT32    i_media_packet_id_type;

	int         i_packet_seq_num;

	HUINT8     *p_cmd;     /* latest command read */
	size_t      i_cmd;      /* allocated at the begining */

	HUINT8     *p_header;  /* allocated by mms_ReadPacket */
	size_t      i_header;

	HUINT8     *p_media;   /* allocated by mms_ReadPacket */
	size_t      i_media;
	size_t      i_media_used;

	/* extracted information */
	int         i_command;

	/* from 0x01 answer (not yet set) */
	char        *psz_server_version;
	char        *psz_tool_version;
	char        *psz_update_player_url;
	char        *psz_encryption_type;

	/* from 0x06 answer */
	HUINT32		i_flags_broadcast;
	HUINT32		i_media_length;		/** secs */
	size_t		i_packet_length;	/** maxmum packet length (bytes)*/
	HUINT32		i_packet_count;		/** the number of ASF Data Packets in the content */
	int			i_max_bit_rate;		/** sum of the maximum bit rates of all streams in the current playlist entry(in bits per secs) */
	size_t		i_header_size;

	/* misc */
	HBOOL  		b_seekable;

	/* seek related */
	HBOOL		bDuringSeeking;
	HBOOL		bFeedASFHeaderFirst;
	HUINT8		*pBufferingBuffer;
	HINT32		nBufferingBufferLength;
	HINT32		nBufferingBufferDataStart;
	HINT32		nBufferingBufferDataEnd;
	

	mmstu_keepalive_thread_t *p_keepalive_thread;

	HULONG lock_netwrite;
	HULONG lock_readapacket;
} MMSTU_ACCESS_SYS_t;

struct mmstu_keepalive_thread_t
{
    STREAM_ACCESS_t *pAccess;
	pthread_t thread_id;
	HBOOL b_quit;
    HBOOL b_paused;
    HBOOL b_thread_error;
};

int  MMSTU_Open( STREAM_ACCESS_t *pAccess );
void  MMSTU_Close  ( STREAM_ACCESS_t * );
ssize_t MMSTU_Read( STREAM_ACCESS_t *, HUINT8 *, size_t );
int MMSTU_Seek( STREAM_ACCESS_t *, HINT64 );
int MMSTU_Control ( STREAM_ACCESS_t *pAccess, DI_IPTUNER_CMD_e cmd, HUINT64 val );

#endif /* __MMS_TCPUDP_H__ */
