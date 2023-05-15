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

#ifndef __STREAM_GLOBAL_H__
#define __STREAM_GLOBAL_H__


#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif
#include <stdarg.h>
#include "htype.h"


typedef enum
{
	PROTOCOL_HTTP,
	PROTOCOL_MMSTU,
	PROTOCOL_MMSH,
	PROTOCOL_RTSP,
	PROTOCOL_UNKNOWN
}STREAM_PROTOCOL_e;


typedef struct stream_access_s
{
	/* Access path/url/filename/.... */
	char        	*psz_path;
	char			*psz_cookie;
	/* Access has to maintain them uptodate */
	STREAM_PROTOCOL_e  proto;
	HBOOL			b_alive;
	HBOOL			b_openfromnexus;
	HBOOL			b_use_persist; /* for http persistent connection */
	struct
	{
		unsigned int 	i_update;  	/* Access sets them on change, Input removes them once take into account*/
		HINT64      	i_size;    		/* Write only for access, read only for input */
		HINT64      	i_pos;     		/* idem */
		HBOOL   		b_eof;     		/* idem */
		int				i_title;    	/* idem, start from 0 (could be menu) */
		int				i_seekpoint;	/* idem, start from 0 */
		HBOOL   		b_prebuffered; 	/* Read only for input */
		int				i_httpstatecode; /* HTTP State Code (for YouTube Redirection */
	} info;
	void *p_sys;	/** Stream Specific Structure . see protocol the controller's header file*/
} STREAM_ACCESS_t;
#endif /* __STREAM_GLOBAL_H__ */
