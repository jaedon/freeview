/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:		$Workfile:   util_b64_encode.c  $
 * Version:			$Revision:   1.0  $
 * Original Author:		SungYong Jeon $
 * Current Author:	$Author: junsy@humaxdigital.com $
 * Date:			$Date: Fri June 05 10:20:48 KST 2009 $
 * File Description:	BASE64Encoder Utility Funcstions
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
#include <string.h>
#include <stdlib.h>

#include "htype.h"
#include "vkernel.h"
#include "di_uart.h"
#include "util_b64_encode.h"
/* End Including Headers */
/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
/* Start #define */
/* End #define */

/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
/* Start typedef */
/* End typedef */


/*******************************************************************/
/************************ global variables *************************/
/*******************************************************************/
/* Start global variable */
/* End global variable */


/*******************************************************************/
/************************ static variables *************************/
/*******************************************************************/
/* Start static variable */
/* End static variable */


/*******************************************************************/
/************************ static funtions **************************/
/*******************************************************************/
static char *int_b64_encode_binary( const HUINT8 *src, size_t i_src )
{
    static const char b64[] =
           "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    char *ret = DD_MEM_Alloc ( ( i_src + 4 ) * 4 / 3 );
    char *dst = ret;

    if( dst == NULL )
	{
        return NULL;
	}

    while( i_src > 0 )
    {
        /* pops (up to) 3 bytes of input, push 4 bytes */
        HINT32 v;

        /* 1/3 -> 1/4 */
        v = *src++ << 24;
        *dst++ = b64[v >> 26];
        v = v << 6;

        /* 2/3 -> 2/4 */
        if( i_src >= 2 )
		{
            v |= *src++ << 22;
		}
        *dst++ = b64[v >> 26];
        v = v << 6;

        /* 3/3 -> 3/4 */
        if( i_src >= 3 )
		{
            v |= *src++ << 20; // 3/3
		}
        *dst++ = ( i_src >= 2 ) ? b64[v >> 26] : '='; // 3/4
        v = v << 6;

        /* -> 4/4 */
        *dst++ = ( i_src >= 3 ) ? b64[v >> 26] : '='; // 4/4

        if( i_src <= 3 )
		{
            break;
		}
        i_src -= 3;
    }

    *dst = '\0';

    return ret;
}

char *UTIL_B64Encode( const char *src )
{
	if( src )
	{
#if 0 /* warning: cast discards qualifiers from pointer target type */
		return int_b64_encode_binary( (const HUINT8*)src, VK_strlen (src) );
#else
		return int_b64_encode_binary( (const HUINT8*)src, VK_strlen (src) );
#endif
	}
	else
	{
		return int_b64_encode_binary( (const HUINT8*)"", 0 );
	}
}






/*
 *
 *   always 'Decoded length' < 'Encoded length'
 *   return value : origical length bytearray
 *
 */
int base64_decode(char *encode,char *decode,int decodesize)
{
	int i = 0, j = 0;
	char decodeTable[256]={0,};


    for( i = 0; i < 255; i++) {
#if 0 /* Caution to warning: overflow in implicit constant conversion */
		decodeTable[i] = 0x80;
#else
		decodeTable[i] = (char)0x80;
#endif
    }
    for(i = 'A' ; i <= 'Z'; i++) {
		decodeTable[i] = 0 + (i - 'A');
    }
    for(i = 'a' ; i <= 'z'; i++) {
		decodeTable[i] = 26 + (i - 'a');
    }
    for(i = '0' ; i <= '9'; i++) {
		decodeTable[i] = 52 + (i - '0');
    }
	decodeTable['+'] = 62;
	decodeTable['/'] = 63;
	decodeTable['='] = 0;


	for(i = j = 0; i<(int)VK_strlen(encode) ; i += 4) {
	char input[4], output[3];

		input[0] = decodeTable[(encode[i + 0] & 0xff)];
		input[1] = decodeTable[(encode[i + 1] & 0xff)];
		input[2] = decodeTable[(encode[i + 2] & 0xff)];
		input[3] = decodeTable[(encode[i + 3] & 0xff)];


	output[0] = (input[0] << 2) | (input[1] >> 4);
	output[1] = (input[1] << 4) | (input[2] >> 2);
	output[2] = (input[2] << 6) | (input[3]);

		if(j >= decodesize) goto overflow_end;
		decode[j] = output[0];
		if(encode[i + 2] == '=') return j+1;

		if(j + 1 >= decodesize) goto overflow_end;
		decode[j + 1] = output[1];
		if(encode[i + 3] == '=') return j+2;

		if(j + 2 >= decodesize) goto overflow_end;
		decode[j + 2] = output[2];
	j += 3;
    }

    return j;

overflow_end:
    //PrintCriticalDebug("internal error: base64 overflow!\n");
    return -1;
}





