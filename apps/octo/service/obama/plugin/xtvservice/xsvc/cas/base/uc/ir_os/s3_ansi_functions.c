/*
 * (c) 2011-2012 Humax Co., Ltd. 
 * This program is produced by Humax Co., Ltd. ("Humax") and 
 * the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee, 
 * non-assignable, non-transferable and non-exclusive license to use this Software. 
 * You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software 
 * you agree to the responsibility to take all reasonable efforts to protect the any information 
 * you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and 
 * reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
 * If you have no authorized license, discontinue using this Software immediately. 
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, 
 * IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE. 
 * IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
 * CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.

 * This License is effective until terminated. You may terminate this License at any time by destroying all copies 
 * of the Software including all documentation. This License will terminate immediately without notice from Humax 
 * to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies 
 * of the Software and all documentation.  

 * The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice. 
 * All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court, 
 * in the Republic of Korea.
*/
/**
 * $Id: s3_ansi_functions.c,v 1.7 2005/02/23 10:59:52 dwinkel Exp $
 *
 * Copyright © 2003-2005 Irdeto Access B.V., All rights reserved.
 *
 * This file is proivided as part of the SoftCell 3 Integration library.
 * 
 * This file, together with s3_ansi_functions.h, provides an abstraction layer between SoftCell 
 * and the ANSI-C standard library.
 * 
 * This file is provided as an example and may need to be altered to match the target platform.
 * Functions may need to meet additional requirements above those described in the ANSI C standard.
 * 
 * Most functions map directly to the ANSI defined library function.
 **/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include "s3_ansi_functions.h"

void* ansi_memcpy( void *pvDest, const void *pvSrc, unsigned int uiCount )
{
	return memcpy( pvDest, pvSrc, uiCount );
}

void* ansi_memmove( void *pvDest, const void *pvSrc, unsigned int uiCount )
{
	return memmove( pvDest, pvSrc, uiCount );
}

int ansi_memcmp( const void *pvData1, const void *pvData2, unsigned int uiLength )
{
	return memcmp( pvData1, pvData2, uiLength );
}

void* ansi_memset( void *pvData, int iData, unsigned int uiLength )
{
	return memset( pvData, iData, uiLength );
}

unsigned int ansi_strlen( const char *pcString )
{
	return strlen( pcString );
}

unsigned int ansi_sprintf( char *pcbuffer, const char *pcFormat, ... )
{
	unsigned int retVal;
	va_list args;
	
	va_start( args, pcFormat );
	retVal = vsprintf( pcbuffer, pcFormat, args );
	va_end( args );
	return retVal;
}

long int ansi_time( long int *pliTimer )
{
	return time( pliTimer );
}

