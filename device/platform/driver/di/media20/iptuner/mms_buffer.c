/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:		$Workfile:   mms_buffer.c  $
 * Version:		2.0 (for Media 2.0 with 40nm)
 * Original Author:	Jin Lee
 * Current Author:	$Author: jlee@humaxdigital.com $
 * Date:			$Date: 2012.07.30
 * File Description:	     MMS - Buffer Controller
 * Module:
 * Remarks:	
 */	

/**
 * @defgroup MMS_BUFFER_CONTROLLER
 * @brief MMS BUFFER Controller.
 */

/**
 * @author Jin Lee (jlee@humaxdigital.com)
 * @date 30 Jul 2012
 */

/**
 * @brief MMS BUFFER Controller.
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
 * @file mms_buffer.c
 */

/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/
/* Start Including Header Files */
#include "htype.h"
#include "di_iptuner_config.h"
#include "di_iptuner_common.h"

#include <stdlib.h>
#include <iconv.h>
#include <string.h>

#include "vkernel.h"
#include "mms_asf.h"
#include "mms_buffer.h"

#define UNUSED_PARAM(x)  ((void)x)

/*****************************************************************************
 * Buffer management functions
 *****************************************************************************/
int MMS_BUF_Initwrite( var_buffer_t *ptBuf, int i_default_size )
{
	ptBuf->i_size =  ( i_default_size > 0 ) ? i_default_size : 2048;
	ptBuf->i_data = 0;
	if( !( ptBuf->p_data = DD_MEM_Alloc ( ptBuf->i_size ) ) )
	{
		return( -1 );
	}
	return( 0 );
}

int MMS_BUF_Reinitwrite( var_buffer_t *ptBuf, int i_default_size )
{
	ptBuf->i_data = 0;
	if( ptBuf->i_size < i_default_size )
	{
		ptBuf->i_size = i_default_size;
		DD_MEM_Free ( ptBuf->p_data );
		ptBuf->p_data = DD_MEM_Alloc ( ptBuf->i_size );
	}
	if( !ptBuf->p_data )
	{
		ptBuf->i_size =  ( i_default_size > 0 ) ? i_default_size : 2048;
		ptBuf->p_data = DD_MEM_Alloc ( ptBuf->i_size );
	}
	if( !ptBuf->p_data )
	{
		return( -1 );
	}
	return( 0 );
}

void MMS_BUF_Add8 ( var_buffer_t *ptBuf, HUINT8  i_byte )
{
	/* check if there is enough data */
	if( ptBuf->i_data >= ptBuf->i_size )
	{
		ptBuf->i_size += 1024;
		ptBuf->p_data = realloc ( ptBuf->p_data, ptBuf->i_size );
	}
	ptBuf->p_data[ptBuf->i_data] = i_byte&0xff;
	ptBuf->i_data++;
}

void MMS_BUF_Add16( var_buffer_t *ptBuf, HUINT16 i_word )
{
	MMS_BUF_Add8( ptBuf, i_word&0xff );
	MMS_BUF_Add8( ptBuf, ( i_word >> 8 )&0xff );
}

void MMS_BUF_Add32( var_buffer_t *ptBuf, HUINT32 i_dword )
{
	MMS_BUF_Add16( ptBuf, i_dword&0xffff );
	MMS_BUF_Add16( ptBuf, ( i_dword >> 16 )&0xffff );
}

void MMS_BUF_Add64( var_buffer_t *ptBuf, HUINT64 i_long )
{
    MMS_BUF_Add32( ptBuf, i_long&0xffffffff );
    MMS_BUF_Add32( ptBuf, ( i_long >> 32 )&0xffffffff );
}

void MMS_BUF_Addmemory( var_buffer_t *ptBuf, void *p_mem, int i_mem )
{
	/* check if there is enough data */
	if( ptBuf->i_data + i_mem >= ptBuf->i_size )
	{
		ptBuf->i_size += i_mem + 1024;
		ptBuf->p_data = realloc( ptBuf->p_data, ptBuf->i_size );
	}

	VK_memcpy( ptBuf->p_data + ptBuf->i_data, p_mem, i_mem );

	ptBuf->i_data += i_mem;
}

void MMS_BUF_AddUTF16( var_buffer_t *ptBuf, char *p_str )
{
	unsigned int i;
	if( !p_str )
	{
		MMS_BUF_Add16( ptBuf, 0 );
	}
	else
	{
		iconv_t iconv_handle;
		size_t i_in = VK_strlen ( p_str );
		size_t i_out = i_in * 4;
		char *psz_out, *psz_tmp;

		psz_out = psz_tmp = DD_MEM_Alloc ( i_out + 1 );
		iconv_handle = iconv_open( "UTF-16LE", "UTF-8" );

		iconv ( iconv_handle, &p_str, &i_in, &psz_tmp, &i_out );
		iconv_close( iconv_handle );
		psz_tmp[0] = '\0';
		psz_tmp[1] = '\0';

		for( i = 0; ; i += 2 )
		{
			HUINT16 v = GetWLE( &psz_out[i] );
			MMS_BUF_Add16( ptBuf, v );
			if( !v )
				break;
		}
		DD_MEM_Free ( psz_out );
	}
}

void MMS_BUF_Free( var_buffer_t *ptBuf )
{
    DD_MEM_Free ( ptBuf->p_data );
    ptBuf->i_data = 0;
    ptBuf->i_size = 0;
}

void MMS_BUF_Initread( var_buffer_t *ptBuf, void *p_data, int i_data )
{
    ptBuf->i_size = i_data;
    ptBuf->i_data = 0;
    ptBuf->p_data = p_data;
}

HUINT8 MMS_BUF_Get8 ( var_buffer_t *ptBuf )
{
    HUINT8  i_byte;
    if( ptBuf->i_data >= ptBuf->i_size )
    {
        return( 0 );
    }
    i_byte = ptBuf->p_data[ptBuf->i_data];
    ptBuf->i_data++;
    return( i_byte );
}

HUINT16 MMS_BUF_Get16( var_buffer_t *ptBuf )
{
    HUINT16 i_b1, i_b2;

    i_b1 = MMS_BUF_Get8( ptBuf );
    i_b2 = MMS_BUF_Get8( ptBuf );

    return( i_b1 + ( i_b2 << 8 ) );

}

HUINT32 MMS_BUF_Get32( var_buffer_t *ptBuf )
{
    HUINT32 i_w1, i_w2;

    i_w1 = MMS_BUF_Get16( ptBuf );
    i_w2 = MMS_BUF_Get16( ptBuf );

    return( i_w1 + ( i_w2 << 16 ) );
}

HUINT64 MMS_BUF_Get64( var_buffer_t *ptBuf )
{
    HUINT64 i_dw1, i_dw2;

    i_dw1 = MMS_BUF_Get32( ptBuf );
    i_dw2 = MMS_BUF_Get32( ptBuf );

    return( i_dw1 + ( i_dw2 << 32 ) );
}

int MMS_BUF_Getmemory ( var_buffer_t *ptBuf, void *p_mem, int64_t i_mem )
{
    int i_copy;

    i_copy = __MIN( i_mem, ptBuf->i_size - ptBuf->i_data );
    if( i_copy > 0 && p_mem != NULL)
    {
        VK_memcpy( p_mem, ptBuf + ptBuf->i_data, i_copy );
    }
    if( i_copy < 0 )
    {
        i_copy = 0;
    }
    ptBuf->i_data += i_copy;
    return( i_copy );
}

int MMS_BUF_Readempty( var_buffer_t *ptBuf )
{
    return( ( ptBuf->i_data >= ptBuf->i_size ) ? 1 : 0 );
}

void MMS_BUF_Getguid( var_buffer_t *ptBuf, ASF_GUID_t *p_guid )
{
    int i;

    p_guid->v1 = MMS_BUF_Get32( ptBuf );
    p_guid->v2 = MMS_BUF_Get16( ptBuf );
    p_guid->v3 = MMS_BUF_Get16( ptBuf );

    for( i = 0; i < 8; i++ )
    {
        p_guid->v4[i] = MMS_BUF_Get8( ptBuf );
    }
}
