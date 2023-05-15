

/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  layer_module.h File Name�� �����ϴ�.
	@brief	  file_name & simple comment.

	Description: File�� ���� ������ �����ϴ�.		\n
	Module: Layer�� ��� ���� �����ϴ�.			 	\n
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
 * TTX Char�� ������ ����, unicode�� �����Ѵ�.
 *
 * @param	pusUnicode	Output�Ǵ� Unicode
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
 * ������ TTX Char�� unicode�� �����Ѵ�.
 *
 * @param	pusUnicode	Output�Ǵ� Unicode
 * @param	ucCharCode	ttx char code (Latin G0.. , G1, G2, ...)
 * @param	ucChar		ttx char
 * @param	ucDiacriticalMark	���� ��ȣ
 * @return	ERR_OK / ERR_FAIL
 */
HERROR	TTX_CHAR_CompositeCharToUnicode	(
						HUINT16 *		pusUnicode,
						const HUINT8 	ucCharCode,
						const HUINT8 	ucChar,
						const HUINT8 	ucDiacriticalMark
						);

#endif //__TTX_CHARMAP_H__



