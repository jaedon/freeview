/*************************************************************
 * @file		hxmath.h
 * @date		2011/01/20
 * @author	Jin Hyun (Jinh@humaxdigital.com)
 * @brief		HUMAX common library header, related with math functon. (CRC, BCD...)

 * http://www.humaxdigital.com
 *************************************************************/

#ifndef HXMATH_H_
#define HXMATH_H_

#include "htype.h"


/**
 * HLIB_MATH_GetCrc64 : CRC64 �� ���
 * pvData : ����Ϸ��� Binary
 * nLen: Binary�� byte number
 * Returns : CRC64 ��
 */
HUINT64 HLIB_MATH_GetCrc64 (void *pvData, HUINT64 nLen);


/**
 * HLIB_MATH_GetCrc32 : CRC32 �� ���
 * pvData : ����Ϸ��� Binary
 * nLen: Binary�� byte number
 * Returns : CRC32 ��
 */
HUINT32 HLIB_MATH_GetCrc32 (void *pvData, HUINT32 nLen);


/**
 * HLIB_MATH_GetCrc32 : CRC32 �� ���
 * ulInitCrc : Crc �ʱⰪ
 * pvData : ����Ϸ��� Binary
 * nLen: Binary�� byte number
 * Returns : CRC32 ��
 */
HUINT32 HLIB_MATH_GetCrc32WithInitial (HUINT32 ulInitCrc, void *pvData, HUINT32 nLen);



/**
 * HLIB_MATH_GetCrc32Ex : �����ؼ� CRC32 �� ���
 * pvData : ����Ϸ��� Binary
 * nLen: Binary�� byte number
 * ulPrevCrc32: ���� ���� CRC32 ��
 * Returns : CRC32 ��
 */
HUINT32 HLIB_MATH_GetCrc32Ex (void *pvData, HUINT32 nLen, HUINT32 ulPrevCrc32);



/**
 * HLIB_MATH_GetCrc16 : CRC16 �� ���
 * pvData : ����Ϸ��� Binary
 * nLen: Binary�� byte number
 * Returns : CRC16 ��
 */
HUINT16 HLIB_MATH_GetCrc16 (void *pvData, HUINT32 nLen);



/**
 * HLIB_MATH_GetCrc16Ccitt : CCITT�� CRC16 �� ���
 * pvData : ����Ϸ��� Binary
 * nLen: Binary�� byte number
 * Returns : CRC16 ��
 */
HUINT16 HLIB_MATH_GetCrc16Ccitt (void *pvData, HUINT32 nLen);


HUINT8 HLIB_MATH_Bcd2Dec (HUINT8 ucBcd);

HUINT8 HLIB_MATH_Dec2Bcd (HUINT8 ucDec);

HUINT32	HLIB_MATH_Sqrt (HUINT32 x);

HUINT32	HLIB_MATH_Random (void);


/**
 * src�� �ش�Ǵ� ����Ʈ ��Ʈ���� base64�� ���ڵ��Ͽ� ����.
 * dst�� ũ��� dst ���ڸ� NULL�� �־� �� �� �ְ�,
 * ���� src�� ũ���� 4/3 ������ �ʿ��ϴ�.
 *
 * @param	dst		destination string buffer (if NULL, �ʿ��� ������ ����� ����)
 * @param	src		source byte buffer
 * @param	len		number of bytes of 'src'
 * @return	number of bytes of 'dst'
 */
HBOOL		HLIB_MATH_IsBase64 (const HCHAR *text);
HUINT32		HLIB_MATH_EncodeBase64 (HCHAR *dst, const void *src, HUINT32 len);
HCHAR *		HLIB_MATH_EncodeBase64Ex (HCHAR *dst, HUINT32 dlen, const void *src, HUINT32 slen);

/**
 * src�� �ش�Ǵ� base64�� ���ڵ� �� ��Ʈ���� ���ڵ� �Ͽ� ����.
 * src�� ũ��� dst ���ڸ� NULL�� �־� �� �� �ְ�,
 * ���� src�� ũ���� 3/4 ������ �ʿ��ϴ�.
 *
 * @param	dst		destination byte buffer (if NULL, �ʿ��� ������ ����� ����)
 * @param	src		source string buffer
 * @param	len		number of bytes of 'src'
 * @return	number of bytes of 'dst'
 */
HUINT32 	HLIB_MATH_DecodeBase64 (void *dst, const HCHAR *src, HUINT32 len);
void *		HLIB_MATH_DecodeBase64Ex (void *dst, HUINT32 dlen, const HCHAR *src, HUINT32 slen);

HCHAR *		HLIB_MATH_EncodeBase64Easy (const void *src, HUINT32 slen);
void *		HLIB_MATH_DecodeBase64Easy (const HCHAR *src, HUINT32 *dlen);

void		HLIB_MATH_FreeBase64 (void *p);

#define HxMATH_Min(a, b)		((a) < (b) ? (a) : (b))
#define HxMATH_Max(a, b)		((a) > (b) ? (a) : (b))
#define HxMATH_Abs(x) 		  	((x) > 0 ? (x) : -(x))
#define HxMATH_Clamp(x,min,max) ((x) < (min) ? (min) : (x) > (max) ? (max) : (x))
#define HxMATH_Bswap16(x) 		(((HUINT16)(x)>>8) | ((HUINT16)(x)<<8))
#define HxMATH_Bswap32(x) 		((((HUINT32)(x)>>24) & 0x000000ff) | (((HUINT32)(x)>> 8) & 0x0000ff00) | \
               				    (((HUINT32)(x)<< 8) & 0x00ff0000) | (((HUINT32)(x)<<24) & 0xff000000))





#endif
