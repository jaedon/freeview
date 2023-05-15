/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:		$Workfile:   mms_buffer.c  $
 * Version:			$Revision:   1.0  $
 * Original Author:		SungYong Jeon $
 * Current Author:	$Author: junsy@humaxdigital.com $
 * Date:			$Date: Fri June 05 10:20:48 KST 2009 $
 * File Description:	MMS Packet Maker
 * Module: 			          
 * Remarks:		
 */

/*******************************************************************/ 	
/* Copyright (c) 2009 HUMAX Co., Ltd. 								*/
/* All rights reserved.												*/
/*******************************************************************/


/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/
/* Start Including Header Files */
#include "htype.h"
#include "di_iptuner_config.h"
#include "di_iptuner_common.h"
#include "di_uart.h"

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
HINT32 mms_buffer_initwrite( var_buffer_t *pBuf, HINT32 iDefaultSize )
{
	if(iDefaultSize > 0)
	{
		pBuf->i_size = iDefaultSize;
	}
	else
	{
		pBuf->i_size = 2048;
	}
	pBuf->i_data = 0;

	if( !( pBuf->p_data = DD_MEM_Alloc ( pBuf->i_size ) ) )
	{
		return( -1 );
	}
	return( 0 );
}

HINT32 mms_buffer_reinitwrite( var_buffer_t *pBuf, HINT32 iDefaultSize )
{
	pBuf->i_data = 0;
	if( pBuf->i_size < iDefaultSize )
	{
		pBuf->i_size = iDefaultSize;
		DD_MEM_Free ( pBuf->p_data );
		pBuf->p_data = DD_MEM_Alloc ( pBuf->i_size );
	}
	
	if( !pBuf->p_data )
	{
		if(iDefaultSize > 0)
		{
			pBuf->i_size = iDefaultSize;
		}
		else
		{
			pBuf->i_size = 2048;
		}
		pBuf->p_data = DD_MEM_Alloc ( pBuf->i_size );
	}
	
	if( !pBuf->p_data )
	{
		return( -1 );
	}
	return( 0 );
}

void mms_buffer_add8 ( var_buffer_t *pBuf, HUINT8  ucData )
{
	/* check if there is enough data */
	if( pBuf->i_data >= pBuf->i_size )
	{
		pBuf->i_size += 1024;
		pBuf->p_data = realloc ( pBuf->p_data, pBuf->i_size );
	}
	pBuf->p_data[pBuf->i_data] = (ucData&0xff);
	pBuf->i_data++;
}

void mms_buffer_add16( var_buffer_t *pBuf, HUINT16 usData )
{
	mms_buffer_add8( pBuf, (usData&0xff) );
	mms_buffer_add8( pBuf, (( usData >> 8 )&0xff) );
}

void mms_buffer_add32( var_buffer_t *pBuf, HUINT32 uiData )
{
	mms_buffer_add16( pBuf, (uiData&0xffff) );
	mms_buffer_add16( pBuf, (( uiData >> 16 )&0xffff) );
}

void mms_buffer_add64( var_buffer_t *pBuf, HUINT64 ullData )
{
    mms_buffer_add32( pBuf, (ullData&0xffffffff) );
    mms_buffer_add32( pBuf, (( ullData >> 32 )&0xffffffff) );
}
//////////////////////////////////////////////////////////////////////
void mms_buffer_addmemory( var_buffer_t *pBuf, void *vMem, HINT32 iSize )
{
	/* check if there is enough data */
	if( pBuf->i_data + iSize >= pBuf->i_size )
	{
		pBuf->i_size += iSize + 1024;
		pBuf->p_data = realloc( pBuf->p_data, pBuf->i_size );
	}

	VK_memcpy( pBuf->p_data + pBuf->i_data, vMem, iSize );

	pBuf->i_data += iSize;
}

void mms_buffer_addUTF16( var_buffer_t *pBuf, char *p_str )
{
	HUINT32 uiIndex;
	HUINT16 v;
		
	if(p_str)
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

		for( uiIndex = 0; ; uiIndex += 2 )
		{
			v = GetWordLE( &psz_out[uiIndex] );
			mms_buffer_add16( pBuf, v );
			if( !v )
				break;
		}
		DD_MEM_Free ( psz_out );
	}
	else
	{		
		mms_buffer_add16( pBuf, 0 );
	}
}

void mms_buffer_free( var_buffer_t *pBuf )
{
    DD_MEM_Free ( pBuf->p_data );
    pBuf->i_size = 0;
    pBuf->i_data = 0;
}

void mms_buffer_initread( var_buffer_t *pBuf, void *p_data, int i_data )
{
    pBuf->i_size = i_data;
    pBuf->p_data = p_data;
    pBuf->i_data = 0;
}

HUINT8 mms_buffer_get8 ( var_buffer_t *pBuf )
{
    HUINT8 uiReturnValue;
	
    if( pBuf->i_data >= pBuf->i_size )
    {
        return( 0 );
    }
    uiReturnValue = pBuf->p_data[pBuf->i_data];
    pBuf->i_data++;
    return( uiReturnValue );
}

HUINT16 mms_buffer_get16( var_buffer_t *pBuf )
{
    HUINT16 usLowByte, usHighByte;

    usLowByte = mms_buffer_get8( pBuf );
    usHighByte = mms_buffer_get8( pBuf );

    return( ( usHighByte << 8 ) + usLowByte );
}

HUINT32 mms_buffer_get32( var_buffer_t *pBuf )
{
    HUINT32 uiLowWord, uiHighWord;

    uiLowWord = mms_buffer_get16( pBuf );
    uiHighWord = mms_buffer_get16( pBuf );

    return( ( uiHighWord << 16 ) + uiLowWord );
}

HUINT64 mms_buffer_get64( var_buffer_t *pBuf )
{
    HUINT64 ullLowDword, ullHighDword;

    ullLowDword = mms_buffer_get32( pBuf );
    ullHighDword = mms_buffer_get32( pBuf );

    return( ( ullHighDword << 32 ) + ullLowDword );
}

int mms_buffer_getmemory ( var_buffer_t *pBuf, void *p_mem, int64_t i_mem )
{
    HINT32 iReturnValue;

    iReturnValue = __MIN( i_mem, pBuf->i_size - pBuf->i_data );
    if( iReturnValue > 0 && p_mem != NULL)
    {
        VK_memcpy( p_mem, pBuf + pBuf->i_data, iReturnValue );
    }
    if( iReturnValue < 0 )
    {
        iReturnValue = 0;
    }
    pBuf->i_data += iReturnValue;
    return( iReturnValue );
}

int mms_buffer_readempty( var_buffer_t *pBuf )
{
	if( pBuf->i_data >= pBuf->i_size )
	{
		return 1;
	}
	return 0;
}

void mms_buffer_getguid( var_buffer_t *pBuf, ASF_GUID_t *p_guid )
{
    HINT32 iIndex;

    p_guid->v1 = mms_buffer_get32( pBuf );
    p_guid->v2 = mms_buffer_get16( pBuf );
    p_guid->v3 = mms_buffer_get16( pBuf );

    for( iIndex = 0; iIndex < 8; iIndex++ )
    {
        p_guid->v4[iIndex] = mms_buffer_get8( pBuf );
    }
}
