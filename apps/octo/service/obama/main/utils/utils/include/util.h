/* $Header:   //BASE/archives/STING_T/include/util.h-arc   1.0   17 Dec 2004 10:31:50   jhkim1  $ */
/********************************************************************
 * $Workfile:   util.h  $
 * Project    : PINE-II ST
 * Author     : Jino Lee
 * Description:
 *
 *                              Copyright (c) 2002 by HUMAX Co., Ltd.
 *                                               All rights reserved.
 ********************************************************************/

#ifndef _UTIL_H_
#define _UTIL_H_

/********************************************************************
 Header Files
 ********************************************************************/
#include <string.h>
#include "htype.h"


/********************************************************************
 Constant/Macro Definition
 ********************************************************************/
/* IRD to IRD type definition */
#define DOWNLOAD_TYPE_LOADER		0
#define DOWNLOAD_TYPE_APPL		1
#define DOWNLOAD_TYPE_EEPROM		2
#define DOWNLOAD_TYPE_CHANNEL	3
#define DOWNLOAD_TYPE_DVD			4
/* IRD to IRD Error definition */
#define DOWNLOAD_ERROR				0xFFFFFFFF


/********************************************************************
 Structure and Type Definition
 ********************************************************************/
typedef enum
{
	CHECK_SUM_OK,
	CHECK_SUM_ERROR
}CHECK_SUM_RESULT;

/********************************************************************
 Global Function Declaration
 ********************************************************************/
/* IRD to IRD Function Prototype */
int Download(unsigned long ulDataType, void (*pFnDisplayCallback)(unsigned long, unsigned int));

/************************************************************************
	Function	: CheckCheckSum
	Description	: pData 의 nDataNum 위치의  CheckSum 이 정확한지 Check
	Input		: pData : Data to Rx/Tx , nCheckSumPos : Data Size
	Output		:
	Return		: 0(CHECK_SUM_OK) - CheckSum OK , -1(CHECK_SUM_ERROR) : CheckSum Error
************************************************************************/
extern unsigned long GetCrc32(void *p, unsigned long n);
extern unsigned short GetCrc16(void *p, unsigned short n);
unsigned long GetCrc32Ex(unsigned char* p, unsigned long n, unsigned long* pCrc);

unsigned short GetCRC16_CCITT(const void *buf, int len);

#ifndef MAX
#define MAX(a,b)	((a) > (b) ? (a) : (b))
#endif

#ifndef MIN
#define MIN(a,b) 	((a) < (b) ? (a) : (b))
#endif

#ifndef ABS
#define ABS(a)      (((a) < 0) ? (-(a)) : (a))
#endif

#define	HIGHWORD(longWord)	((HUINT16)( sizeof(longWord) == 4 ? ((HUINT16)((0xFFFF0000 & longWord) >> 16)) : 0 ))
#define	LOWWORD(longWord)	((HUINT16)( sizeof(longWord) == 4 ? ((HUINT16)(0x0000FFFF & longWord)) : longWord ))
#define	HIGHBYTE(word)		((HUINT8)((0xFF00 & (HUINT16)word) >> 8))
#define	LOWBYTE(word)		((HUINT8)(0x00FF & (HUINT16)word))
#define	HIGHNIBBLE(byte)		((HUINT8)((0xF0 & (HUINT8)byte) >> 4))
#define	LOWNIBBLE(byte)		((HUINT8)(0x0F & (HUINT8)byte))

#define	BITMASK_LWORDBIT(n)	((HUINT32) 1 << n )
#define	BITMASK_WORDBIT(n)	((HUINT16) 1 << n )
#define	BITMASK_BYTEBIT(n)	((HUINT8) 1 << n )

#define MAKE_HUINT16(hiByte, lowByte)		(HUINT16)(((HUINT16)(hiByte << 8) | (HUINT16)lowByte))
#define MAKE_HUINT32(hiWord, lowWord)		(HUINT32)(((HUINT32)((HUINT32)hiWord << 16) | (HUINT32)lowWord))


#define get8bit(q) (unsigned char)(*((unsigned char *)(q)) & 0xff) // (unsigned char)((q)[0])
#define get10bit(q) (unsigned short)((((*((unsigned char *)(q))) << 8) | (*((unsigned char *)(q)+1))) & 0x03ff)/* 0000 0011 1111 1111 */
#define get12bit(q) (unsigned short)((((*((unsigned char *)(q))) << 8) | (*((unsigned char *)(q)+1))) & 0x0fff)/* 0000 1111 1111 1111 */
#define get13bit(q) (unsigned short)((((*((unsigned char *)(q))) << 8) | (*((unsigned char *)(q)+1))) & 0x1fff)/* 0001 1111 1111 1111 */
#define get14bit(q) (unsigned short)((((*((unsigned char *)(q))) << 8) | (*((unsigned char *)(q)+1))) & 0x3fff)/* 0011 1111 1111 1111 */
#define get16bit(q) (unsigned short)((((*((unsigned char *)(q))) << 8) | (*((unsigned char *)(q)+1))        ))

#define get24bit(q) (unsigned int)(((*(unsigned char *)(q)) << 16) | (*((unsigned char *)(q)+1) << 8) | (*((unsigned char *)(q)+2)) )
#define get32bit(q) (unsigned int)(((*(unsigned char *)(q)) << 24) | (*((unsigned char *)(q)+1) << 16) | (*((unsigned char *)(q)+2) << 8) | (*((unsigned char *)(q)+3)) )

#define put16bit(p, data) {*((unsigned char *)(p)) = (unsigned char)((data) >> 8);*((unsigned char *)((p) + 1)) = (unsigned char)(data);}
#define put32bit(p, data) {*((unsigned char *)(p)) = (unsigned char)((data) >> 24);*((unsigned char *)((p) + 1)) = (unsigned char)((data) >> 16);*((unsigned char *)((p) + 2)) = (unsigned char)((data) >> 8);*((unsigned char *)((p) + 3)) = (unsigned char)(data);}
#define GET_ASCII_NUM(X)	((X) - '0')

unsigned long Rand(void);
unsigned char DEC2BCD(unsigned char ucDec);
unsigned char BCD2DEC(unsigned char ucBcd);


#if defined(CONFIG_DEBUG)
void Util_Print_Dump (HUINT8 *pucBuffer, HUINT32 unSize, HUINT32 unBaseAddr, HUINT8 bFlagDetail);
#endif

#endif

/* end of file */

