/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  mw_ttxerrchk.h
	@brief	  mw_ttxerrchk.h : TTX Error Protection

	Description: File에 대한 설명을 적습니다.		\n
	Module: Layer와 모듈 명을 적습니다.			 	\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	__MW_TTXERRCHK_H__
#define	__MW_TTXERRCHK_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include "htype.h"


/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

enum
{
	eTTX_ERR_PARITY		= 0xFF,
	eTTX_ERR_HAMMING84	= 0xFF
};




/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/


//1 TELETEXT Error Protection APIs

/**
 * 홀수 패리티 검사
 * [D1|D2|D3|D4|D5|D6|D7| P]
 * => [ 0|D7]D6|D5|D4|D3|D2|D1]
 *
 * @param	pucDest	검사된 결과 배열 (bit order = big endian)
 * @param	pucSrc	홀수 패리티로 인코딩된 바이트 배열(bit order = little endian)
 * @param	ulLen	검사하고자 하는 배열의 길이
 * @return	에러 개수 or -1(완전 에러!!)
 */
HINT32	TTX_ERRCHK_OddParity	(HUINT8 *pucDest, const HUINT8 *pucSrc, HUINT32 ulLen);


/**
 * 해밍 8/4 코드
 * [P1|D1|P2|D2|P3|D3|P4|D4]
 * => [ 0| 0| 0| 0|D4|D3|D2|D1]
 *
 * @param	pucDest	디코딩 된 결과 값 배열(bit order = big endian)
 * @param	pucSrc	해밍 8/4 인코딩 된 바이트 배열(bit order = little endian)
 * @param	ulLen	검사하고자 하는 배열의 길이
 * @return	에러 개수 or -1(완전 에러!!)
 */
HINT32	TTX_ERRCHK_Hamming84	(HUINT8 *pucDest, const HUINT8 *pucSrc, HUINT32 ulLen);


/**
 * 해밍 24/18 코드
 * [P1|P2|D1|P3|D2|D3|D4|P4] [D5|D6|D7|D8|D9|D10|D11|P5] [D12|D13|D14|D15|D16|D17|D18|P6]
 * => [D4|D3|D2|D1] [D11|D10|D9|D8|D7|D6|D5] [D18|D17|D16|D15|D14|D13|D12]
 *
 * @param	punDest	디코딩 된 결과 값 (bit order = big endian)
 * @param	pucSrc	해밍 24/18로 인코딩된 연속된 세 개의 바이트(bit order = little endian)
 * @param	ulLen	검사하고자 하는 배열의 길이
 * @return	에러 개수 or -1(완전 에러!!)
 */
HINT32	TTX_ERRCHK_Hamming2418 (HUINT8 *pucDest, const HUINT8 *pucSrc, HUINT32 ulLen);


/**
 * Change bit order (big to little, little to big)
 *
 * @param	pucDest	bit order = big endian
 * @param	pucSrc	bit order = little endian
 * @param	ulLen	검사하고자 하는 배열의 길이
 * @return	에러 개수 or -1(완전 에러!!)
 */
HINT32	TTX_ERRCHK_ReverseBit	(HUINT8 *pucDest, const HUINT8 *pucSrc, HUINT32 ulLen);


#endif /* !__MW_TTXERRCHK_H__ */

