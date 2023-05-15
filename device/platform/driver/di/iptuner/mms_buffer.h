/*****************************************************************************
 * buffer.h: MMS access plug-in
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

#ifndef __MMS_BUFFER_H__
#define __MMS_BUFFER_H__
#include "htype.h"

typedef struct
{
    HUINT8 *p_data;    // pointer on data
    HINT32 i_data;     // number of bytes set in p_data

    // private
    HINT32 i_size;     // size of p_data memory allocated
} var_buffer_t;

/*****************************************************************************
 * Macro/Function to create/manipulate buffer
 *****************************************************************************/
int  mms_buffer_initwrite ( var_buffer_t *pBuf, int iDefaultSize );
int  mms_buffer_reinitwrite ( var_buffer_t *pBuf, int iDefaultSize );
void mms_buffer_add8 ( var_buffer_t *pBuf, HUINT8  ucData );
void mms_buffer_add16 ( var_buffer_t *pBuf, HUINT16 usData );
void mms_buffer_add32 ( var_buffer_t *pBuf, HUINT32 uiData );
void mms_buffer_add64 ( var_buffer_t *pBuf, HUINT64 ullData );
void mms_buffer_addmemory ( var_buffer_t *pBuf, void *vMem, int iSize );
void mms_buffer_addUTF16 ( var_buffer_t *pBuf, char *p_str );
void mms_buffer_free ( var_buffer_t *pBuf );


void      mms_buffer_initread ( var_buffer_t *p_bpBufuf, void *p_data, int i_data );
HUINT8   mms_buffer_get8 ( var_buffer_t *pBuf );
HUINT16  mms_buffer_get16 ( var_buffer_t *pBuf );
HUINT32  mms_buffer_get32 ( var_buffer_t *pBuf );
HUINT64  mms_buffer_get64 ( var_buffer_t *pBuf );
int       mms_buffer_getmemory ( var_buffer_t *pBuf, void *p_mem, HINT64 i_mem );
int       mms_buffer_readempty ( var_buffer_t *pBuf );
void      mms_buffer_getguid ( var_buffer_t *pBuf, ASF_GUID_t *p_guid );

#endif /* __MMS_BUFFER_H__ */
