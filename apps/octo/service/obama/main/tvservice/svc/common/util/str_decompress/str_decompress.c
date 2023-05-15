/**
	@file     isosvc.c
	@brief

	Description: 		\n
	Module: mw/common/util			 	\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
/* global header file */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>

#include <octo_common.h>

/* local header file */
#include "isosvc.h"


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/



extern HBOOL DTG1_Huffman_DecompData(int nCompressMethod, unsigned char *szCompressedStr, unsigned long ulCompStrLen, void **szDestStrBuf,  HUINT32 *pulDecompStrLen);

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/


/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

/***************************************************************************/
#define _______PUBLIC_FUNCTIONS________
/***************************************************************************/
/****************************************************************************
Description	:	임시로 한 알고리즘만 적용될 수 있게 함.
Output		:
Return		:	 TRUE or FALSE
Description	:
****************************************************************************/
HBOOL UTIL_STR_Decompress(int nCompressMethod, unsigned char *szCompressedStr, unsigned long ulCompStrLen, void **szDestStrBuf,  HUINT32 *pulDecompStrLen, HUINT8 *ucCodeType)
{
	HBOOL	retValue = FALSE;

#if defined(CONFIG_MWC_STRING_DECOMPRESS_HUFFMAN_DTG1)
	retValue = DTG1_Huffman_DecompData(nCompressMethod, szCompressedStr, ulCompStrLen, szDestStrBuf, pulDecompStrLen);
	if (ucCodeType != NULL)
	{
		*ucCodeType = eHxCP_DVBLANG_VALUE_UTF_8;
	}
#endif
	return retValue;

}

/*********************** End of File ******************************/
