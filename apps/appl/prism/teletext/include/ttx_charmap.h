

/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  layer_module.h File Name을 적습니다.
	@brief	  file_name & simple comment.

	Description: File에 대한 설명을 적습니다.		\n
	Module: Layer와 모듈 명을 적습니다.			 	\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	__TTX_CHARMAP_H__
#define	__TTX_CHARMAP_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include "htype.h"

#include "ttx_decoder.h"


enum	// User defined character
{
	eTTX_CHAR_USER_NONE,
	eTTX_CHAR_USER_ROTATE,

	eTTX_CHAR_USER_LAST
};


/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/

HERROR	TTX_CHAR_ToUCS2 (
						const TTX_Cell_t *pCell,
						HUINT16 *	pusUCS2,
						HUINT16 *	pusDiaMark
						);

/**
 * TTX Char의 정보를 보고, unicode로 매핑한다.
 *
 * @param	pusUnicode	Output되는 Unicode
 * @param	ucCharCode	ttx char code (Latin G0.. , G1, G2, ...)
 * @param	ucNationalOption	National Option Character Subset
 * @param	ucChar		ttx char
 * @return	ERR_OK / ERR_FAIL
 */
HERROR	TTX_CHAR_ToUnicode (
						HUINT16 *		pusUnicode,
						const HUINT8 	ucCharCode,
						const HUINT8 	ucNationalOption,
						const HUINT8 	ucChar
						);

/**
 * 조합형 TTX Char를 unicode로 매핑한다.
 *
 * @param	pusUnicode	Output되는 Unicode
 * @param	ucCharCode	ttx char code (Latin G0.. , G1, G2, ...)
 * @param	ucChar		ttx char
 * @param	ucDiacriticalMark	구분 기호
 * @return	ERR_OK / ERR_FAIL
 */
HERROR	TTX_CHAR_CompositeCharToUnicode	(
						HUINT16 *		pusUnicode,
						const HUINT8 	ucCharCode,
						const HUINT8 	ucChar,
						const HUINT8 	ucDiacriticalMark
						);

#endif //__TTX_CHARMAP_H__



