/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:		$Workfile:   util_md5.c  $
 * Version:			$Revision:   1.0  $
 * Original Author:	$ SungYong Jeon $
 * Current Author:	$Author: junsy@humaxdigital.com $
 * Date:			$Date: Fri June 05 10:20:48 KST 2009 $
 * File Description:	not so strong MD5 hashing
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

#include "di_iptuner_common.h"
#include "util_md5.h"
#include "vkernel.h"
#include "di_uart.h"
/* End Including Headers */
/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
#ifdef WORDS_BIGENDIAN
/*************************
* Reverse byte order
*************************/
static void HUMAX_Reverse(HUINT32 *p_buffer, HINT32 n )
{
    HINT32 index;

    for(index = 0; index < n; index++ )
    {
        p_buffer[index] = GetDWLE(&p_buffer[index]);
    }
}
	#define HUMAX_REVERSE( p, n ) HUMAX_Reverse( p, n )
#else
	#define HUMAX_REVERSE( p, n )
#endif

#define CUSTOM_FUNCTION1( a, b, c ) ((c) ^ ((a) & ((b) ^ (c))))
#define CUSTOM_FUNCTION2( a, b, c ) CUSTOM_FUNCTION1((c), (a), (b))
#define CUSTOM_FUNCTION3( a, b, c ) ((a) ^ (b) ^ (c))
#define CUSTOM_FUNCTION4( a, b, c ) ((b) ^ ((a) | ~(c)))

#define CUSTOM_FUNCTION5( w, a, b, c, data, s, f ) \
    ( w += f(a, b, c) + data,  w = w<<s | w>>(32-s),  w += a )

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

/*************************
* Update the MD5 digest with 64 bytes of data
*************************/
static void HUMAX_DigestMD5( struct md5_s *p_md5, HUINT32 *p_input )
{
    HUINT32 uiValue1, uiValue2, uiValue3, uiValue4;

    HUMAX_REVERSE( p_input, 16 );

    uiValue1 = p_md5->uiDigest[0];
    uiValue2 = p_md5->uiDigest[1];
    uiValue3 = p_md5->uiDigest[2];
    uiValue4 = p_md5->uiDigest[3];

    CUSTOM_FUNCTION5(uiValue1, uiValue2, uiValue3, uiValue4, p_input[0] + 0xd76aa478,  7, CUSTOM_FUNCTION1 );
    CUSTOM_FUNCTION5(  uiValue4, uiValue1, uiValue2, uiValue3, p_input[1] + 0xe8c7b756, 12, CUSTOM_FUNCTION1 );
    CUSTOM_FUNCTION5(  uiValue3, uiValue4, uiValue1, uiValue2, p_input[2] + 0x242070db, 17, CUSTOM_FUNCTION1 );
    CUSTOM_FUNCTION5(  uiValue2, uiValue3, uiValue4, uiValue1, p_input[3] + 0xc1bdceee, 22, CUSTOM_FUNCTION1 );
    CUSTOM_FUNCTION5(  uiValue1, uiValue2, uiValue3, uiValue4, p_input[4] + 0xf57c0faf,  7, CUSTOM_FUNCTION1 );
    CUSTOM_FUNCTION5(  uiValue4, uiValue1, uiValue2, uiValue3, p_input[5] + 0x4787c62a, 12, CUSTOM_FUNCTION1 );
    CUSTOM_FUNCTION5(  uiValue3, uiValue4, uiValue1, uiValue2, p_input[6] + 0xa8304613, 17, CUSTOM_FUNCTION1 );
    CUSTOM_FUNCTION5(  uiValue2, uiValue3, uiValue4, uiValue1, p_input[7] + 0xfd469501, 22, CUSTOM_FUNCTION1 );
    CUSTOM_FUNCTION5(  uiValue1, uiValue2, uiValue3, uiValue4, p_input[8] + 0x698098d8,  7, CUSTOM_FUNCTION1 );
    CUSTOM_FUNCTION5(  uiValue4, uiValue1, uiValue2, uiValue3, p_input[9] + 0x8b44f7af, 12, CUSTOM_FUNCTION1 );
    CUSTOM_FUNCTION5(  uiValue3, uiValue4, uiValue1, uiValue2, p_input[10] + 0xffff5bb1, 17, CUSTOM_FUNCTION1 );
    CUSTOM_FUNCTION5(  uiValue2, uiValue3, uiValue4, uiValue1, p_input[11] + 0x895cd7be, 22, CUSTOM_FUNCTION1 );
    CUSTOM_FUNCTION5(  uiValue1, uiValue2, uiValue3, uiValue4, p_input[12] + 0x6b901122,  7, CUSTOM_FUNCTION1 );
    CUSTOM_FUNCTION5(  uiValue4, uiValue1, uiValue2, uiValue3, p_input[13] + 0xfd987193, 12, CUSTOM_FUNCTION1 );
    CUSTOM_FUNCTION5(  uiValue3, uiValue4, uiValue1, uiValue2, p_input[14] + 0xa679438e, 17, CUSTOM_FUNCTION1 );
    CUSTOM_FUNCTION5(  uiValue2, uiValue3, uiValue4, uiValue1, p_input[15] + 0x49b40821, 22, CUSTOM_FUNCTION1 );

    CUSTOM_FUNCTION5(  uiValue1, uiValue2, uiValue3, uiValue4, p_input[1] + 0xf61e2562,  5, CUSTOM_FUNCTION2 );
    CUSTOM_FUNCTION5(  uiValue4, uiValue1, uiValue2, uiValue3, p_input[6] + 0xc040b340,  9, CUSTOM_FUNCTION2 );
    CUSTOM_FUNCTION5(  uiValue3, uiValue4, uiValue1, uiValue2, p_input[11] + 0x265e5a51, 14, CUSTOM_FUNCTION2 );
    CUSTOM_FUNCTION5(  uiValue2, uiValue3, uiValue4, uiValue1, p_input[0] + 0xe9b6c7aa, 20, CUSTOM_FUNCTION2 );
    CUSTOM_FUNCTION5(  uiValue1, uiValue2, uiValue3, uiValue4, p_input[5] + 0xd62f105d,  5, CUSTOM_FUNCTION2 );
    CUSTOM_FUNCTION5(  uiValue4, uiValue1, uiValue2, uiValue3, p_input[10] + 0x02441453,  9, CUSTOM_FUNCTION2 );
    CUSTOM_FUNCTION5(  uiValue3, uiValue4, uiValue1, uiValue2, p_input[15] + 0xd8a1e681, 14, CUSTOM_FUNCTION2 );
    CUSTOM_FUNCTION5(  uiValue2, uiValue3, uiValue4, uiValue1, p_input[4] + 0xe7d3fbc8, 20, CUSTOM_FUNCTION2 );
    CUSTOM_FUNCTION5(  uiValue1, uiValue2, uiValue3, uiValue4, p_input[9] + 0x21e1cde6,  5, CUSTOM_FUNCTION2 );
    CUSTOM_FUNCTION5(  uiValue4, uiValue1, uiValue2, uiValue3, p_input[14] + 0xc33707d6,  9, CUSTOM_FUNCTION2 );
    CUSTOM_FUNCTION5(  uiValue3, uiValue4, uiValue1, uiValue2, p_input[3] + 0xf4d50d87, 14, CUSTOM_FUNCTION2 );
    CUSTOM_FUNCTION5(  uiValue2, uiValue3, uiValue4, uiValue1, p_input[8] + 0x455a14ed, 20, CUSTOM_FUNCTION2 );
    CUSTOM_FUNCTION5(  uiValue1, uiValue2, uiValue3, uiValue4, p_input[13] + 0xa9e3e905,  5, CUSTOM_FUNCTION2 );
    CUSTOM_FUNCTION5(  uiValue4, uiValue1, uiValue2, uiValue3, p_input[2] + 0xfcefa3f8,  9, CUSTOM_FUNCTION2 );
    CUSTOM_FUNCTION5(  uiValue3, uiValue4, uiValue1, uiValue2, p_input[7] + 0x676f02d9, 14, CUSTOM_FUNCTION2 );
    CUSTOM_FUNCTION5(  uiValue2, uiValue3, uiValue4, uiValue1, p_input[12] + 0x8d2a4c8a, 20, CUSTOM_FUNCTION2 );

    CUSTOM_FUNCTION5(  uiValue1, uiValue2, uiValue3, uiValue4, p_input[5] + 0xfffa3942,  4, CUSTOM_FUNCTION3 );
    CUSTOM_FUNCTION5(  uiValue4, uiValue1, uiValue2, uiValue3, p_input[8] + 0x8771f681, 11, CUSTOM_FUNCTION3 );
    CUSTOM_FUNCTION5(  uiValue3, uiValue4, uiValue1, uiValue2, p_input[11] + 0x6d9d6122, 16, CUSTOM_FUNCTION3 );
    CUSTOM_FUNCTION5(  uiValue2, uiValue3, uiValue4, uiValue1, p_input[14] + 0xfde5380c, 23, CUSTOM_FUNCTION3 );
    CUSTOM_FUNCTION5(  uiValue1, uiValue2, uiValue3, uiValue4, p_input[1] + 0xa4beea44,  4, CUSTOM_FUNCTION3 );
    CUSTOM_FUNCTION5(  uiValue4, uiValue1, uiValue2, uiValue3, p_input[4] + 0x4bdecfa9, 11, CUSTOM_FUNCTION3 );
    CUSTOM_FUNCTION5(  uiValue3, uiValue4, uiValue1, uiValue2, p_input[7] + 0xf6bb4b60, 16, CUSTOM_FUNCTION3 );
    CUSTOM_FUNCTION5(  uiValue2, uiValue3, uiValue4, uiValue1, p_input[10] + 0xbebfbc70, 23, CUSTOM_FUNCTION3 );
    CUSTOM_FUNCTION5(  uiValue1, uiValue2, uiValue3, uiValue4, p_input[13] + 0x289b7ec6,  4, CUSTOM_FUNCTION3 );
    CUSTOM_FUNCTION5(  uiValue4, uiValue1, uiValue2, uiValue3, p_input[0] + 0xeaa127fa, 11, CUSTOM_FUNCTION3 );
    CUSTOM_FUNCTION5(  uiValue3, uiValue4, uiValue1, uiValue2, p_input[3] + 0xd4ef3085, 16, CUSTOM_FUNCTION3 );
    CUSTOM_FUNCTION5(  uiValue2, uiValue3, uiValue4, uiValue1, p_input[6] + 0x04881d05, 23, CUSTOM_FUNCTION3 );
    CUSTOM_FUNCTION5(  uiValue1, uiValue2, uiValue3, uiValue4, p_input[9] + 0xd9d4d039,  4, CUSTOM_FUNCTION3 );
    CUSTOM_FUNCTION5(  uiValue4, uiValue1, uiValue2, uiValue3, p_input[12] + 0xe6db99e5, 11, CUSTOM_FUNCTION3 );
    CUSTOM_FUNCTION5(  uiValue3, uiValue4, uiValue1, uiValue2, p_input[15] + 0x1fa27cf8, 16, CUSTOM_FUNCTION3 );
    CUSTOM_FUNCTION5(  uiValue2, uiValue3, uiValue4, uiValue1, p_input[2] + 0xc4ac5665, 23, CUSTOM_FUNCTION3 );

    CUSTOM_FUNCTION5(uiValue1, uiValue2, uiValue3, uiValue4, p_input[0] + 0xf4292244,  6, CUSTOM_FUNCTION4 );
    CUSTOM_FUNCTION5(  uiValue4, uiValue1, uiValue2, uiValue3, p_input[7] + 0x432aff97, 10, CUSTOM_FUNCTION4 );
    CUSTOM_FUNCTION5(  uiValue3, uiValue4, uiValue1, uiValue2, p_input[14] + 0xab9423a7, 15, CUSTOM_FUNCTION4 );
    CUSTOM_FUNCTION5(  uiValue2, uiValue3, uiValue4, uiValue1, p_input[5] + 0xfc93a039, 21, CUSTOM_FUNCTION4 );
    CUSTOM_FUNCTION5(  uiValue1, uiValue2, uiValue3, uiValue4, p_input[12] + 0x655b59c3,  6, CUSTOM_FUNCTION4 );
    CUSTOM_FUNCTION5(  uiValue4, uiValue1, uiValue2, uiValue3, p_input[3] + 0x8f0ccc92, 10, CUSTOM_FUNCTION4 );
    CUSTOM_FUNCTION5(  uiValue3, uiValue4, uiValue1, uiValue2, p_input[10] + 0xffeff47d, 15, CUSTOM_FUNCTION4 );
    CUSTOM_FUNCTION5(  uiValue2, uiValue3, uiValue4, uiValue1, p_input[1] + 0x85845dd1, 21, CUSTOM_FUNCTION4 );
    CUSTOM_FUNCTION5(  uiValue1, uiValue2, uiValue3, uiValue4, p_input[8] + 0x6fa87e4f,  6, CUSTOM_FUNCTION4 );
    CUSTOM_FUNCTION5(  uiValue4, uiValue1, uiValue2, uiValue3, p_input[15] + 0xfe2ce6e0, 10, CUSTOM_FUNCTION4 );
    CUSTOM_FUNCTION5(  uiValue3, uiValue4, uiValue1, uiValue2, p_input[6] + 0xa3014314, 15, CUSTOM_FUNCTION4 );
    CUSTOM_FUNCTION5(  uiValue2, uiValue3, uiValue4, uiValue1, p_input[13] + 0x4e0811a1, 21, CUSTOM_FUNCTION4 );
    CUSTOM_FUNCTION5(  uiValue1, uiValue2, uiValue3, uiValue4, p_input[4] + 0xf7537e82,  6, CUSTOM_FUNCTION4 );
    CUSTOM_FUNCTION5(  uiValue4, uiValue1, uiValue2, uiValue3, p_input[11] + 0xbd3af235, 10, CUSTOM_FUNCTION4 );
    CUSTOM_FUNCTION5(  uiValue3, uiValue4, uiValue1, uiValue2, p_input[2] + 0x2ad7d2bb, 15, CUSTOM_FUNCTION4 );
    CUSTOM_FUNCTION5(  uiValue2, uiValue3, uiValue4, uiValue1, p_input[9] + 0xeb86d391, 21, CUSTOM_FUNCTION4 );

    p_md5->uiDigest[0] += uiValue1;
    p_md5->uiDigest[1] += uiValue2;
    p_md5->uiDigest[2] += uiValue3;
    p_md5->uiDigest[3] += uiValue4;
}

/******************************
* Initialise an MD5 message
******************************/
void HUMAX_InitMD5( struct md5_s *p_md5 )
{
    p_md5->uiDigest[0] = 0x67452301;
    p_md5->uiDigest[1] = 0xefcdab89;
    p_md5->uiDigest[2] = 0x98badcfe;
    p_md5->uiDigest[3] = 0x10325476;
    p_md5->i_bits = 0;

    VK_memset (p_md5->p_data, 0, 64);
}

/******************************
* Add i_len bytes to an MD5 message
******************************/
void HUMAX_AddMD5( struct md5_s *p_md5, const void *p_src, size_t i_len )
{
	size_t offset=0;
	HUINT32 uiCurrent;
	HUINT32 p_tmp[16];

	uiCurrent = (p_md5->i_bits/8) & 63;
	p_md5->i_bits += (8*i_len);

	if(i_len >= (64-uiCurrent))
	{
		VK_memcpy(((HUINT8 *)p_md5->p_data) + uiCurrent, p_src, (64 - uiCurrent));
		HUMAX_DigestMD5(p_md5, p_md5->p_data);
        i_len -= (64 - uiCurrent);
		offset += (64 - uiCurrent);
		uiCurrent = 0;
	}

	while(i_len >= 64)
	{
		VK_memcpy( p_tmp, ((const HUINT8 *)p_src) + offset, 64 );
        HUMAX_DigestMD5( p_md5, p_tmp );
        offset += 64;
        i_len -= 64;
	}
	VK_memcpy(((HUINT8 *)p_md5->p_data) + uiCurrent, ((const HUINT8 *)p_src) + offset, i_len);
}

/******************************
* Finish an MD5 message
******************************/
void HUMAX_EndMD5 ( struct md5_s *p_md5 )
{
    HUINT32 uiCurrent;

    uiCurrent = (p_md5->i_bits / 8) & 63;
    ((HUINT8 *)p_md5->p_data)[uiCurrent++] = 0x80;

    if( uiCurrent > 56 )
    {
        VK_memset (((HUINT8 *)p_md5->p_data) + uiCurrent, 0, (64 - uiCurrent));
        HUMAX_DigestMD5(p_md5, p_md5->p_data);
        uiCurrent = 0;
    }

    VK_memset ( ((HUINT8 *)p_md5->p_data) + uiCurrent, 0, (56 - uiCurrent) );
    p_md5->p_data[14] = (p_md5->i_bits & 0xffffffff);
    p_md5->p_data[15] = (p_md5->i_bits >> 32);
    HUMAX_REVERSE(&p_md5->p_data[14], 2);

    HUMAX_DigestMD5(p_md5, p_md5->p_data);
}


