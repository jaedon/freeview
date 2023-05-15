/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:		$Workfile:   util_md5.c  $
 * Version:			$Revision:   1.0  $
 * Original Author:		SungYong Jeon $
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
#include "di_iptuner_config.h"
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

#ifdef WORDS_BIGENDIAN
static void Reverse( HUINT32 *p_buffer, int n )
{
    int iIndex;

    for( iIndex = 0; iIndex < n; iIndex++ )
    {
        p_buffer[ iIndex ] = GetDWLE(&p_buffer[ iIndex ]);
    }
}
	#define REVERSE_ORDER( p, n ) Reverse( p, n )
#else
	#define REVERSE_ORDER( p, n )
#endif

#define F1( x, y, z ) ((z) ^ ((x) & ((y) ^ (z))))
#define F2( x, y, z ) F1((z), (x), (y))
#define F3( x, y, z ) ((x) ^ (y) ^ (z))
#define F4( x, y, z ) ((y) ^ ((x) | ~(z)))

#define MD5_DO( f, w, x, y, z, data, s ) \
    ( w += f(x, y, z) + data,  w = w<<s | w>>(32-s),  w += x )

static void DigestMD5( struct md5_s *p_md5, HUINT32 *src )
{
    HUINT32 a, b, c, d;

    REVERSE_ORDER( src, 16 );

    a = p_md5->p_digest[ 0 ];
    b = p_md5->p_digest[ 1 ];
    c = p_md5->p_digest[ 2 ];
    d = p_md5->p_digest[ 3 ];

    MD5_DO( F1, a, b, c, d, src[  0 ] + 0xd76aa478,  7 );
    MD5_DO( F1, d, a, b, c, src[  1 ] + 0xe8c7b756, 12 );
    MD5_DO( F1, c, d, a, b, src[  2 ] + 0x242070db, 17 );
    MD5_DO( F1, b, c, d, a, src[  3 ] + 0xc1bdceee, 22 );
    MD5_DO( F1, a, b, c, d, src[  4 ] + 0xf57c0faf,  7 );
    MD5_DO( F1, d, a, b, c, src[  5 ] + 0x4787c62a, 12 );
    MD5_DO( F1, c, d, a, b, src[  6 ] + 0xa8304613, 17 );
    MD5_DO( F1, b, c, d, a, src[  7 ] + 0xfd469501, 22 );
    MD5_DO( F1, a, b, c, d, src[  8 ] + 0x698098d8,  7 );
    MD5_DO( F1, d, a, b, c, src[  9 ] + 0x8b44f7af, 12 );
    MD5_DO( F1, c, d, a, b, src[ 10 ] + 0xffff5bb1, 17 );
    MD5_DO( F1, b, c, d, a, src[ 11 ] + 0x895cd7be, 22 );
    MD5_DO( F1, a, b, c, d, src[ 12 ] + 0x6b901122,  7 );
    MD5_DO( F1, d, a, b, c, src[ 13 ] + 0xfd987193, 12 );
    MD5_DO( F1, c, d, a, b, src[ 14 ] + 0xa679438e, 17 );
    MD5_DO( F1, b, c, d, a, src[ 15 ] + 0x49b40821, 22 );

    MD5_DO( F2, a, b, c, d, src[  1 ] + 0xf61e2562,  5 );
    MD5_DO( F2, d, a, b, c, src[  6 ] + 0xc040b340,  9 );
    MD5_DO( F2, c, d, a, b, src[ 11 ] + 0x265e5a51, 14 );
    MD5_DO( F2, b, c, d, a, src[  0 ] + 0xe9b6c7aa, 20 );
    MD5_DO( F2, a, b, c, d, src[  5 ] + 0xd62f105d,  5 );
    MD5_DO( F2, d, a, b, c, src[ 10 ] + 0x02441453,  9 );
    MD5_DO( F2, c, d, a, b, src[ 15 ] + 0xd8a1e681, 14 );
    MD5_DO( F2, b, c, d, a, src[  4 ] + 0xe7d3fbc8, 20 );
    MD5_DO( F2, a, b, c, d, src[  9 ] + 0x21e1cde6,  5 );
    MD5_DO( F2, d, a, b, c, src[ 14 ] + 0xc33707d6,  9 );
    MD5_DO( F2, c, d, a, b, src[  3 ] + 0xf4d50d87, 14 );
    MD5_DO( F2, b, c, d, a, src[  8 ] + 0x455a14ed, 20 );
    MD5_DO( F2, a, b, c, d, src[ 13 ] + 0xa9e3e905,  5 );
    MD5_DO( F2, d, a, b, c, src[  2 ] + 0xfcefa3f8,  9 );
    MD5_DO( F2, c, d, a, b, src[  7 ] + 0x676f02d9, 14 );
    MD5_DO( F2, b, c, d, a, src[ 12 ] + 0x8d2a4c8a, 20 );

    MD5_DO( F3, a, b, c, d, src[  5 ] + 0xfffa3942,  4 );
    MD5_DO( F3, d, a, b, c, src[  8 ] + 0x8771f681, 11 );
    MD5_DO( F3, c, d, a, b, src[ 11 ] + 0x6d9d6122, 16 );
    MD5_DO( F3, b, c, d, a, src[ 14 ] + 0xfde5380c, 23 );
    MD5_DO( F3, a, b, c, d, src[  1 ] + 0xa4beea44,  4 );
    MD5_DO( F3, d, a, b, c, src[  4 ] + 0x4bdecfa9, 11 );
    MD5_DO( F3, c, d, a, b, src[  7 ] + 0xf6bb4b60, 16 );
    MD5_DO( F3, b, c, d, a, src[ 10 ] + 0xbebfbc70, 23 );
    MD5_DO( F3, a, b, c, d, src[ 13 ] + 0x289b7ec6,  4 );
    MD5_DO( F3, d, a, b, c, src[  0 ] + 0xeaa127fa, 11 );
    MD5_DO( F3, c, d, a, b, src[  3 ] + 0xd4ef3085, 16 );
    MD5_DO( F3, b, c, d, a, src[  6 ] + 0x04881d05, 23 );
    MD5_DO( F3, a, b, c, d, src[  9 ] + 0xd9d4d039,  4 );
    MD5_DO( F3, d, a, b, c, src[ 12 ] + 0xe6db99e5, 11 );
    MD5_DO( F3, c, d, a, b, src[ 15 ] + 0x1fa27cf8, 16 );
    MD5_DO( F3, b, c, d, a, src[  2 ] + 0xc4ac5665, 23 );

    MD5_DO( F4, a, b, c, d, src[  0 ] + 0xf4292244,  6 );
    MD5_DO( F4, d, a, b, c, src[  7 ] + 0x432aff97, 10 );
    MD5_DO( F4, c, d, a, b, src[ 14 ] + 0xab9423a7, 15 );
    MD5_DO( F4, b, c, d, a, src[  5 ] + 0xfc93a039, 21 );
    MD5_DO( F4, a, b, c, d, src[ 12 ] + 0x655b59c3,  6 );
    MD5_DO( F4, d, a, b, c, src[  3 ] + 0x8f0ccc92, 10 );
    MD5_DO( F4, c, d, a, b, src[ 10 ] + 0xffeff47d, 15 );
    MD5_DO( F4, b, c, d, a, src[  1 ] + 0x85845dd1, 21 );
    MD5_DO( F4, a, b, c, d, src[  8 ] + 0x6fa87e4f,  6 );
    MD5_DO( F4, d, a, b, c, src[ 15 ] + 0xfe2ce6e0, 10 );
    MD5_DO( F4, c, d, a, b, src[  6 ] + 0xa3014314, 15 );
    MD5_DO( F4, b, c, d, a, src[ 13 ] + 0x4e0811a1, 21 );
    MD5_DO( F4, a, b, c, d, src[  4 ] + 0xf7537e82,  6 );
    MD5_DO( F4, d, a, b, c, src[ 11 ] + 0xbd3af235, 10 );
    MD5_DO( F4, c, d, a, b, src[  2 ] + 0x2ad7d2bb, 15 );
    MD5_DO( F4, b, c, d, a, src[  9 ] + 0xeb86d391, 21 );

    p_md5->p_digest[ 3 ] += d;
	p_md5->p_digest[ 2 ] += c;
    p_md5->p_digest[ 1 ] += b;
    p_md5->p_digest[ 0 ] += a;
}

void InitMD5( struct md5_s *p_md5 )
{
	VK_memset ( p_md5->p_data, 0, 64 );
    p_md5->i_bits = 0;		

    p_md5->p_digest[ 3 ] = 0x10325476;
    p_md5->p_digest[ 2 ] = 0x98badcfe;
    p_md5->p_digest[ 1 ] = 0xefcdab89;
    p_md5->p_digest[ 0 ] = 0x67452301;
}

void AddMD5( struct md5_s *p_md5, const void *p_src, size_t length )
{
    unsigned int i_current;
    size_t offset = 0;

    i_current = (p_md5->i_bits / 8) & 63;

    p_md5->i_bits += 8 * length;

    if( length >= (64 - i_current) )
    {
        VK_memcpy( ((HUINT8 *)p_md5->p_data) + i_current, p_src,
                (64 - i_current) );
        DigestMD5( p_md5, p_md5->p_data );

        offset += (64 - i_current);
        length -= (64 - i_current);
        i_current = 0;
    }

    while( length >= 64 )
    {
        HUINT32 p_tmp[ 16 ];
        VK_memcpy( p_tmp, ((const HUINT8 *)p_src) + offset, 64 );
        DigestMD5( p_md5, p_tmp );
        offset += 64;
        length -= 64;
    }

    VK_memcpy( ((HUINT8 *)p_md5->p_data) + i_current,
            ((const HUINT8 *)p_src) + offset, length );
}

void EndMD5 ( struct md5_s *p_md5 )
{
    unsigned int uiCurrent;

    uiCurrent = (p_md5->i_bits / 8) & 63;

    ((HUINT8 *)p_md5->p_data)[ uiCurrent++ ] = 0x80;

    if( uiCurrent > 56 )
    {
        VK_memset ( ((HUINT8 *)p_md5->p_data) + uiCurrent, 0, (64 - uiCurrent) );
        DigestMD5( p_md5, p_md5->p_data );
        uiCurrent = 0;
    }

    VK_memset ( ((HUINT8 *)p_md5->p_data) + uiCurrent, 0, (56 - uiCurrent) );
    p_md5->p_data[ 14 ] = p_md5->i_bits & 0xffffffff;
    p_md5->p_data[ 15 ] = (p_md5->i_bits >> 32);
    REVERSE_ORDER( &p_md5->p_data[ 14 ], 2 );

    DigestMD5( p_md5, p_md5->p_data );
}


