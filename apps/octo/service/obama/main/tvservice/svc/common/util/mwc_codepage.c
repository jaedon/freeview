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
#include <mwc_codepage.h>

#include <octo_common.h>

/* local header file */
#include "isosvc.h"

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
/* override symbol "NULL" (sbchung) */
#ifdef NULL
#undef NULL
#endif

#define NULL 0





#define DVB_UTF8_CHAR_NUM(ch, num)		{ if  (ch < 0xC0) num = 1; else if (ch < 0xE1) num = 2; else if (ch < 0xF0) num = 3;  else num = 4; }

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/


/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/


/* -------------------------------------------------------------------------------------------------- */
#define _________________________CODEPAGE__COMMON_PUBLIC____________________________________________________
/* -------------------------------------------------------------------------------------------------- */

STATIC HERROR		local_mwc_util_codepage_GetEncodingWithLenByLocaleCode(const CH_1BYTE *pcStr, eHxCP_CodePage_e *peEncoding, HINT32 *pulEncodingLen, eHxCP_CodePage_e eDefaultEncoding)
{
	eHxCP_CodePage_e	eTmpEncoding = eHCodepage_MAX;
	HINT32			nEncodingLen = 0;


	switch (*pcStr)
	{
	/* 8859 first byte */
	case eHxCP_DVBLANG_VALUE_8859_05:		nEncodingLen = 1;		eTmpEncoding = eHxCP_ISO_8859_5;				break;
	case eHxCP_DVBLANG_VALUE_8859_06:		nEncodingLen = 1;		eTmpEncoding = eHxCP_ISO_8859_6;				break;
	case eHxCP_DVBLANG_VALUE_8859_07:		nEncodingLen = 1;		eTmpEncoding = eHxCP_ISO_8859_7;				break;
	case eHxCP_DVBLANG_VALUE_8859_08:		nEncodingLen = 1;		eTmpEncoding = eHxCP_ISO_8859_8;				break;
	case eHxCP_DVBLANG_VALUE_8859_09:		nEncodingLen = 1;		eTmpEncoding = eHxCP_ISO_8859_9;				break;
	case eHxCP_DVBLANG_VALUE_8859_10:		nEncodingLen = 1;		eTmpEncoding = eHxCP_ISO_8859_10;				break;
	case eHxCP_DVBLANG_VALUE_8859_11:		nEncodingLen = 1;		eTmpEncoding = eHxCP_ISO_8859_11;				break;
	case eHxCP_DVBLANG_VALUE_8859_12:		nEncodingLen = 1;		eTmpEncoding = eHxCP_ISO_8859_12;				break;
	case eHxCP_DVBLANG_VALUE_8859_13:		nEncodingLen = 1;		eTmpEncoding = eHxCP_ISO_8859_13;				break;
	case eHxCP_DVBLANG_VALUE_8859_14:		nEncodingLen = 1;		eTmpEncoding = eHxCP_ISO_8859_14;				break;
	case eHxCP_DVBLANG_VALUE_8859_15:		nEncodingLen = 1;		eTmpEncoding = eHxCP_ISO_8859_15;				break;

	/* UNICODE */
	case eHxCP_DVBLANG_VALUE_UCS2:		nEncodingLen = 1;		eTmpEncoding = eHxCP_UNICODE_DVB_UCS2;					break;
	case eHxCP_DVBLANG_VALUE_UTF_8:		nEncodingLen = 1;		eTmpEncoding = eHxCP_UNICODE_DVB_UTF8;					break;

	/* 300468 - 8859 Multi Byte */
	case eHxCP_DVBLANG_VALUE_8859:
		if(*(pcStr+1) == 0x00)
		{
			nEncodingLen = 3;

			switch(*(pcStr+2))
			{
			case 0x01: eTmpEncoding = eHxCP_ISO_8859_1;		break;
			case 0x02: eTmpEncoding = eHxCP_ISO_8859_2; 	break;
			case 0x03: eTmpEncoding = eHxCP_ISO_8859_3; 	break;
			case 0x04: eTmpEncoding = eHxCP_ISO_8859_4; 	break;
			case 0x05: eTmpEncoding = eHxCP_ISO_8859_5; 	break;
			case 0x06: eTmpEncoding = eHxCP_ISO_8859_6; 	break;
			case 0x07: eTmpEncoding = eHxCP_ISO_8859_7; 	break;
			case 0x08: eTmpEncoding = eHxCP_ISO_8859_8; 	break;
			case 0x09: eTmpEncoding = eHxCP_ISO_8859_9; 	break;
			case 0x0A: eTmpEncoding = eHxCP_ISO_8859_10;	break;
			case 0x0B: eTmpEncoding = eHxCP_ISO_8859_11;	break;

			case 0x0D: eTmpEncoding = eHxCP_ISO_8859_13;	break;
			case 0x0E: eTmpEncoding = eHxCP_ISO_8859_14;	break;
			case 0x0F: eTmpEncoding = eHxCP_ISO_8859_15;	break;

			default:
				nEncodingLen = 0;
				eTmpEncoding = eHCodepage_MAX;
				break;
			}

		}
		// 앞 문자가 0x10 , 0x69, 0x37 이 온다.
		// 이는 어느 Spec에도 존재하지 않는 OCTO만의 6937 처리 법이다.
		else if(*(pcStr+1) == 0x69 && *(pcStr+2) == 0x37)
		{
			eTmpEncoding = eHxCP_ISO_6937;
			nEncodingLen = 3;
		}

		// 아시아향에서 OCTO만의 LocaleCode는 이쪽으로...
		else if(*(pcStr+1) == 0x4A)
		{
			nEncodingLen = 3;
			switch(*(pcStr+2))
			{
			/* japan only */
			case 0x45:	eTmpEncoding = eHxCP_ISO_JAPAN_EUC;		break;	// (Ascii : "JE" Hex code) : 0x10 0x4A 0x45
			case 0x50:	eTmpEncoding = eHxCP_ISO_JAPAN_JIS;		break;	// (Ascii : "JP" Hex code) : 0x10 0x4A 0x50
			case 0x53:	eTmpEncoding = eHxCP_ISO_JAPAN_SJIS;	break;	// (Ascii : "JS" Hex code) : 0x10 0x4A 0x53


			/* korea only */

			/* etc */

			default:
				eTmpEncoding = eDefaultEncoding;
				nEncodingLen = 0;
				break;
			}
		}
		// 러시아향에서 OCTO만의 LocaleCode,
		else if(*(pcStr+1) == 0x5A)
		{
			nEncodingLen = 3;
			switch(*(pcStr+2))
			{
			/* CP1251 code (Cyrillic)*/
			case 0x45:	eTmpEncoding = eHxCP_WIN_1251;		break;

			/* etc */

			default:
				eTmpEncoding = eDefaultEncoding;
				nEncodingLen = 0;
				break;
			}
		}

 		break;

	default:
		eTmpEncoding = eDefaultEncoding;
		nEncodingLen = 0;
		break;

	}

	*peEncoding = eTmpEncoding;
	*pulEncodingLen = nEncodingLen;

	return ERR_OK;
}

STATIC HERROR		local_mwc_util_codepage_GetCharSizeByEncoding(eHxCP_CodePage_e eEncoding, HINT32 *pnCharLength)
{
	HINT32			ulCount = 0;

	//순수하게 buffer length을 알기위한 함수. (char 의 개수가 아님.)
	switch(eEncoding)
	{
	/* 2byte 문자 : NULL(0x0000) 으로 end */
	case eHxCP_UNICODE_UCS2:
	case eHxCP_UNICODE_DVB_UCS2:
		ulCount = 2;
		break;

	/* 1byte 문자 : NULL(0x00) 으로 end */
	case eHxCP_ISO_JAPAN_SJIS:
	case eHxCP_WIN_1251:
	default:
		ulCount = 1;
		break;
	}

	if(pnCharLength)
	{
		*pnCharLength = ulCount;
	}

	return ERR_OK;
}



/* -------------------------------------------------------------------------------------------------- */
#define _________________________CODEPAGE_COMMON_PRIVATE____________________________________________________
/* -------------------------------------------------------------------------------------------------- */


HERROR		MWC_UTIL_CodePage_GetEncodingWithLenByLocaleCode(const CH_1BYTE *pcStr, eHxCP_CodePage_e *peEncoding, HINT32 *pulEncodingLen, eHxCP_CodePage_e eDefaultEncoding)
{
	return 	local_mwc_util_codepage_GetEncodingWithLenByLocaleCode(pcStr, peEncoding, pulEncodingLen, eDefaultEncoding);
}

HERROR		MWC_UTIL_CodePage_GetCharSizeByEncoding(eHxCP_CodePage_e eDefaultEncoding, HINT32 *pnCharLength)
{
	return 	local_mwc_util_codepage_GetCharSizeByEncoding(eDefaultEncoding, pnCharLength);
}

/* -------------------------------------------------------------------------------------------------- */
#define _________________________CHANGE_CODEPAGE_PRIVATE___________________________________________________
/* -------------------------------------------------------------------------------------------------- */

STATIC HERROR	local_mwc_util_ConvUtf8ToSingleByte(const CH_UTF8 *pStrUtf8, const HINT32 nStrUtf8Len, HINT32 *pnUsedByteLen, CH_1BYTE *pSingleByteStr)
{
	HUINT8 c = pStrUtf8[0];

	/* 1byte character */
	if (c < 0x80)
	{
		*pSingleByteStr = c;
		*pnUsedByteLen=1;
		return ERR_OK;
	}
	/* illegal sequence */
	else if (c < 0xc2)
	{
		return ERR_ILLEGAL_SEQ; //RET_ILSEQ;
	}
	/* 2byte character */
	else if (c < 0xe0)
	{
		if (nStrUtf8Len < 2)
			return ERR_TOOSHORT;
		if (!((pStrUtf8[1] ^ 0x80) < 0x40))
			return ERR_ILLEGAL_SEQ; //RET_ILSEQ;

		*pSingleByteStr = ((HUINT8) (c & 0x1f) << 6) | (HUINT8) (pStrUtf8[1] ^ 0x80);
		*pnUsedByteLen=2;
		return ERR_OK;
	}
	else
	{
		return ERR_FAIL; //RET_ILSEQ;
	}
}

STATIC HERROR	local_mwc_util_ConvSingleByteToUtf8(const CH_1BYTE strSinglebyte, const HINT32 nUtf8BufferLen, HINT32 *pnUcs2Utf8Len, CH_UTF8 *pUcs2Utf8)
{
	HUINT16 count = 0;
	HUINT8	ch = strSinglebyte;

	if (ch < 0x80)
		count = 1;
	else //if (ch < 0x100)
		count = 2;
#if 0
	else
		return ERR_ILLEGAL_SEQ;
#endif

		if (nUtf8BufferLen < count)
			return ERR_BUFFER_TOOSMALL;

		switch (count) { /* note: code falls through cases! */
		case 2: pUcs2Utf8[1] = 0x80 | (ch & 0x3f); ch = ch >> 6; ch |= 0xc0;
		case 1: pUcs2Utf8[0] = ch;
		}

		*pnUcs2Utf8Len = count;

		return ERR_OK;
}


STATIC HERROR	local_mwc_util_ConvUCS4ToUtf8(const CH_UCS4 strUcs4, const HINT32 nUtf8BufferLen, HINT32 *pnUcs2Utf8Len, CH_UTF8 *pUcs2Utf8)
{
	HUINT16 count = 0;
	HUINT32	ch = strUcs4;

	if (ch < 0x80)
		count = 1;
	else if (ch < 0x800)
		count = 2;
	else if (ch < 0x10000)
		count = 3;
	else if (ch < 0x200000)
		count = 4;
	else if (ch < 0x4000000)
		count = 5;
	else if (ch <= 0x7fffffff)
		count = 6;
	else
		return ERR_ILLEGAL_SEQ;

	if (nUtf8BufferLen < count)
		return ERR_BUFFER_TOOSMALL;

	switch (count) { /* note: code falls through cases! */
		case 6: pUcs2Utf8[5] = 0x80 | (ch & 0x3f); ch = ch >> 6; ch |= 0x4000000;
		case 5: pUcs2Utf8[4] = 0x80 | (ch & 0x3f); ch = ch >> 6; ch |= 0x200000;
		case 4: pUcs2Utf8[3] = 0x80 | (ch & 0x3f); ch = ch >> 6; ch |= 0x10000;
		case 3: pUcs2Utf8[2] = 0x80 | (ch & 0x3f); ch = ch >> 6; ch |= 0x800;
		case 2: pUcs2Utf8[1] = 0x80 | (ch & 0x3f); ch = ch >> 6; ch |= 0xc0;
		case 1: pUcs2Utf8[0] = ch;
	}

	*pnUcs2Utf8Len = count;

	return ERR_OK;
}

/* -------------------------------------------------------------------------------------------------- */
#define _________________________CHANGE_CODEPAGE_PUBLIC___________________________________________________
/* -------------------------------------------------------------------------------------------------- */


ConvEncoding_Err_t	MWC_UTIL_ConvertEncodingToUtf8(const eHxCP_CodePage_e eSourceEncoding, const CH_1BYTE *pSrcStr, const HINT32 nSrcStrLen, const HINT32 nBufferLen, HINT32 *pnUsedByte, CH_UTF8 *pUtf8Str, HINT32 *pnUtf8StrLen)
{
	HINT32		nTmpUCS2StrLen = 0;
	CH_UCS2		*pTmpUCS2Buf = NULL;

	ConvEncoding_Err_t	hErr = ERR_ENCODING_FAIL;

	HxLOG_Info("MWC_UTIL_ConvertEncodingToUtf8 eSourceEncoding(0x%x) pSrcStr(%s) +\n", eSourceEncoding, pSrcStr);

	*pnUsedByte = 0;
	*pnUtf8StrLen = 0;

	if(nSrcStrLen == 0)
	{
		return ERR_ENCODING_FAIL;
	}

	/* check code page, and map the coverting function */
	switch(eSourceEncoding)
	{
	case eHxCP_UNICODE_UCS2:
		*pnUtf8StrLen = HLIB_CP_ConvertUCS2ToCodepage(eHxCP_UNICODE_DVB_UTF8, (const HUINT16 *)pSrcStr, pUtf8Str, nBufferLen);
		break;

	case eHxCP_UNICODE_DVB_UTF8:
	case eHxCP_UNICODE_UTF8:
		if(nSrcStrLen <= nBufferLen)
		{
			HxSTD_memcpy(pUtf8Str, pSrcStr, nSrcStrLen);
			*pnUtf8StrLen = nSrcStrLen;
			*pnUsedByte = nSrcStrLen;
		}
		else
		{
			HxSTD_memcpy(pUtf8Str, pSrcStr, nBufferLen);
			pUtf8Str[nBufferLen-1] = 0x00;
			*pnUtf8StrLen = nBufferLen;
			*pnUsedByte = nBufferLen;
		}
		break;

	default:

		pTmpUCS2Buf = (CH_UCS2 *)OxMW_Malloc(sizeof(CH_UCS2) * nSrcStrLen);
		VK_memset32(pTmpUCS2Buf, 0, sizeof(CH_UCS2) * nSrcStrLen);
		hErr = MWC_UTIL_ConvertEncodingToUCS2((const eHxCP_CodePage_e)eSourceEncoding, (const CH_1BYTE *)pSrcStr, (const HINT32)nSrcStrLen, pnUsedByte, pTmpUCS2Buf, &nTmpUCS2StrLen);
		if(*pnUsedByte == 0)
		{
			OxMW_Free(pTmpUCS2Buf);
			return hErr;
		}

		hErr = MWC_UTIL_ConvertUCS2ToUtf8((CH_UCS2 *)pTmpUCS2Buf, nSrcStrLen, nBufferLen, pnUsedByte, pUtf8Str, pnUtf8StrLen);
		OxMW_Free(pTmpUCS2Buf);
		return hErr;
	}

	HxLOG_Info("MWC_UTIL_ConvertEncodingToUtf8() -\n");

	return ERR_OK;
}

ConvEncoding_Err_t	MWC_UTIL_ConvertEncodingToUCS2(const eHxCP_CodePage_e eSourceEncoding, const CH_1BYTE *pSrcStr, const HINT32 nSrcStrLen, HINT32 *pnUsedByte, CH_UCS2 *pUcs2Str, HINT32 *pnUcs2StrLen)
{
	HERROR		hErr = ERR_FAIL;
	CPContext_t *pCPContext = NULL;

	HxLOG_Info("MWCS_UTIL_ConvEncodingToUCS2 eSourceEncoding(0x%x) pSrcStr(%s) +\n", eSourceEncoding, pSrcStr);
	if(pnUsedByte == NULL || pnUcs2StrLen == NULL)
	{
		return ERR_ENCODING_FAIL;
	}
	*pnUsedByte = 0;
	*pnUcs2StrLen = 0;

	pCPContext = MWC_UTIL_CodePage_GetContext(eSourceEncoding);
	if(pCPContext == NULL)
	{
		return ERR_ENCODING_FAIL;
	}

	if(pCPContext->cbConvertCodepageToUCS2 != NULL)
	{
		hErr = pCPContext->cbConvertCodepageToUCS2(pSrcStr, nSrcStrLen, pnUsedByte, pUcs2Str, pnUcs2StrLen);
	}

	HxLOG_Info("MWCS_UTIL_ConvEncodingToUCS2() -\n");

	return hErr;
}

ConvEncoding_Err_t	MWC_UTIL_ConvertUCS2ToEncoding(const eHxCP_CodePage_e eSourceEncoding, const CH_UCS2 *pSrcUcs2Str, const HINT32 nSrcStrLen, const HINT32 nBufferLen, HINT32 *pnUsedByte, CH_1BYTE  *pCodepageStr, HINT32 *pnUcs2StrLen)
{
	HERROR		hErr = ERR_FAIL;
	CPContext_t *pCPContext = NULL;

	if(pnUsedByte == NULL || pnUcs2StrLen == NULL)
	{
		return ERR_ENCODING_FAIL;
	}
	*pnUsedByte = 0;
	*pnUcs2StrLen = 0;

	pCPContext = MWC_UTIL_CodePage_GetContext(eSourceEncoding);
	if(pCPContext == NULL)
	{
		return ERR_ENCODING_FAIL;
	}

	if(pCPContext->cbConvertUCS2ToCodepage != NULL)
	{
		hErr = pCPContext->cbConvertUCS2ToCodepage(pSrcUcs2Str, nSrcStrLen, nBufferLen, pnUsedByte, pCodepageStr, pnUcs2StrLen);
	}

	HxLOG_Info("MWCS_UTIL_ConvEncodingToUCS2() -\n");

	return hErr;
}


ConvEncoding_Err_t	MWC_UTIL_ConvertSingleByteToUtf8(const CH_1BYTE *pStrSingle, const HINT32 nStrSingleLen, const HINT32 nBufferLen, HINT32 *pnUsedByte, CH_UTF8 *pUtf8Str, HINT32 *pnUtf8StrLen)
{
	HINT32		i = 0;
	HINT32		nConvertedCharacterCount = 0;

	ConvEncoding_Err_t	hResErr=ERR_FAIL;

	HxLOG_Info("MWC_UTIL_ConvertSingleByteToUtf8 nStrSingleLen(%d) +\n", nStrSingleLen);

	*pnUsedByte=0;
	*pnUtf8StrLen=0;

	for(i = 0; i < nStrSingleLen; i++)
	{
		nConvertedCharacterCount = 0;

		hResErr = local_mwc_util_ConvSingleByteToUtf8(pStrSingle[i], nBufferLen-(*pnUtf8StrLen), &nConvertedCharacterCount, pUtf8Str+(*pnUtf8StrLen));
		if(hResErr == ERR_BUFFER_TOOSMALL)
		{
			/* 버퍼가 모자라다, 변환 마침. */
			break;
		}
		else if(hResErr != ERR_OK)
		{
			continue;
		}
		else
		{
			/* nothing to do */
		}

		/* 누적되면 변환된 utf8 스트링 길이 */
		*pnUtf8StrLen += nConvertedCharacterCount;
	}

	HxLOG_Info("MWC_UTIL_ConvertSingleByteToUtf8 result error(0x%x) -\n", hResErr);

	return hResErr;
}




ConvEncoding_Err_t	MWC_UTIL_ConvertUCS2ToUtf8(const CH_UCS2 *pStrUcs2,	const HINT32 nStrUcs2Len, const HINT32 nBufferLen,
													HINT32 *pnUsedByte, CH_UTF8 *pUtf8Str, HINT32 *pnUtf8StrLen)
{
	CPContext_t *pCPContext = NULL;

	pCPContext = MWC_UTIL_CodePage_GetContext(eHxCP_UNICODE_UTF8);
	if(pCPContext == NULL)
	{
		return ERR_ENCODING_FAIL;
	}

	if(pCPContext->cbConvertUCS2ToCodepage == NULL)
	{
		return ERR_FAIL;
	}

	return pCPContext->cbConvertUCS2ToCodepage(pStrUcs2, nStrUcs2Len, nBufferLen, pnUsedByte, pUtf8Str, pnUtf8StrLen);
}


ConvEncoding_Err_t	MWC_UTIL_ConvertUCS4ToUtf8(const CH_UCS4 *pStrUcs4, const HINT32 nStrUcs4Len, const HINT32 nBufferLen, HINT32 *pnUsedByte, CH_UTF8 *pUtf8Str, HINT32 *pnUtf8StrLen)
{
	HINT32		i = 0;
	HINT32		nConvertedCharacterCount = 0;

	ConvEncoding_Err_t	hResErr=ERR_FAIL;

	HxLOG_Info("MWC_UTIL_ConvertUCS4ToUtf8 strSingleLen(%d) +\n", nStrUcs4Len);

	*pnUsedByte = 0;
	*pnUtf8StrLen = 0;

	for(i = 0; i < nStrUcs4Len; i++)
	{
		nConvertedCharacterCount = 0;

		hResErr = local_mwc_util_ConvUCS4ToUtf8(pStrUcs4[i], nBufferLen-(*pnUtf8StrLen), &nConvertedCharacterCount, pUtf8Str+(*pnUtf8StrLen));
		if(hResErr == ERR_BUFFER_TOOSMALL)
		{
			/* 버퍼가 모자라다, 변환 마침. */
			break;
		}
		else if(hResErr != ERR_OK)
		{
			continue;
		}
		else
		{
			/* nothing to do */
		}

		/* 누적되면 변환된 utf8 스트링 길이 */
		*pnUtf8StrLen += nConvertedCharacterCount;
	}

	*pnUsedByte = i;

	HxLOG_Info("MWC_UTIL_ConvertUCS4ToUtf8 result error(0x%x) -\n", hResErr);

	return hResErr;
}

ConvEncoding_Err_t	MWC_UTIL_ConvertUtf8ToSingleByte(const CH_UTF8 *pStrUtf8, const HINT32 nStrUtf8Len, HINT32 *pnUsedByte, CH_1BYTE *pSingleByteStr, HINT32 *pSingleByteStrLen)
{
	HINT32		i = 0;
	HINT32		nUsedBuf = 0;
	HINT32		nConvertedCharacterCount = 0;

	ConvEncoding_Err_t	hResErr = ERR_FAIL;

	HxLOG_Info("MWC_UTIL_ConvertUtf8ToSingleByte nStrUtf8Len(%d) +\n", nStrUtf8Len);

	*pnUsedByte=0;
	*pSingleByteStrLen=0;

	for(i=0; i<nStrUtf8Len; /*NOT*/)
	{
		hResErr=local_mwc_util_ConvUtf8ToSingleByte(pStrUtf8+i, nStrUtf8Len-i, &nUsedBuf, pSingleByteStr+nConvertedCharacterCount);
		if(hResErr==ERR_TOOSHORT)
		{
			/* no more converted */
			++i;
			break;
		}
		else if(hResErr==ERR_ILLEGAL_SEQ)
		{
			/* find sequence */
			++i;
			continue;
		}
		else if(hResErr!=ERR_OK)
		{
			/* some error case */
			++i;
			continue;
		}

		HxLOG_Print("[a+%02x]count[%d]\n", *(pSingleByteStr+nConvertedCharacterCount), nConvertedCharacterCount);

		i+=nUsedBuf;
		nConvertedCharacterCount++;
	}

	*pnUsedByte=i;
	*pSingleByteStrLen=nConvertedCharacterCount;

	HxLOG_Info("MWC_UTIL_ConvertUtf8ToSingleByte() result error(0x%x) -\n", hResErr);

	return hResErr;
}


/* -------------------------------------------------------------------------------------------------- */
#define _________________________CODEPAGE__GetEmphasizedString____________________________________________________
/* -------------------------------------------------------------------------------------------------- */


HERROR MWC_UTIL_GetEmphasizedStringFrom300468String (HUINT8 *szSrcStr, HUINT8 *szDstStr)
{
	CPContext_t *pCPContext = NULL;

	pCPContext = MWC_UTIL_CodePage_GetContext(eHxCP_ISO_300468);
	if(pCPContext == NULL)
	{
		return ERR_ENCODING_FAIL;
	}

	if(pCPContext->cbConvertUCS2ToCodepage == NULL)
	{
		return ERR_FAIL;
	}

	return pCPContext->cbGetEmphasizedString(szSrcStr, szDstStr);
}


/* -------------------------------------------------------------------------------------------------- */
#define _________________________CODEPAGE__INSERT_PRIVATE____________________________________________________
/* -------------------------------------------------------------------------------------------------- */


STATIC CPContext_t 	s_stCpContext[eHCodepage_MAX];

STATIC HERROR local_mwc_util_codepage_Init(void)
{
	HERROR hErr = ERR_OK;

	HxSTD_memset(&s_stCpContext, 0, sizeof(CPContext_t) * eHCodepage_MAX);

//default
	hErr |= MWC_UTIL_CodePage_RegisterASCII();
	hErr |= MWC_UTIL_CodePage_RegisterUFT8();
	hErr |= MWC_UTIL_CodePage_RegisterUCS2();
	hErr |= MWC_UTIL_CodePage_Register6937With300468();

//config
//#if defined(CONFIG_CODEPAGE_8859)
	hErr |= MWC_UTIL_CodePage_Register8859();
//#endif

#if defined(CONFIG_MWC_CODEPAGE_JAPAN)
	hErr |= MWC_UTIL_CodePage_RegisterJP();
#endif

#if defined(CONFIG_MWC_CODEPAGE_WINCP) //codepage config CONFIG_MWC_CODEPAGE_WINCP1251 추가 후 변경 할 것임.
	hErr |= MWC_UTIL_CodePage_RegisterWINCP();
#endif

	return hErr;
}

STATIC CPContext_t *local_mwc_util_codepage_GetCPContextByTextEncoding(eHxCP_CodePage_e eTextEncoding)
{
	if(eTextEncoding < eHCodepage_MAX)
	{
		return &s_stCpContext[eTextEncoding];
	}

	return NULL;
}

STATIC HERROR 		local_mwc_util_codepage_Register(eHxCP_CodePage_e eTextEncoding,
											MwcCountDrawableCharNum 	cbCountDrawableCharNum,
										  	MwcGetDrawableChar			cbGetDrawableChar,
										  	MwcConvertCodepageToUCS2	cbConvertCodepageToUCS2,
										  	MwcConvertUCS2ToCodepage	cbConvertUCS2ToCodepage, /* optional */
										  	MwcGetEmphasizedString		cbGetEmphasizedString,	 /* optional */
										  	void					*pTable
)
{
	CPContext_t *pstCPContext = NULL;

	pstCPContext = local_mwc_util_codepage_GetCPContextByTextEncoding(eTextEncoding);
	if(pstCPContext == NULL)
	{
		return ERR_FAIL;
	}

	pstCPContext->cbCountDrawableCharNum 	= cbCountDrawableCharNum;
	pstCPContext->cbGetDrawableChar 		= cbGetDrawableChar;
	pstCPContext->cbConvertCodepageToUCS2 	= cbConvertCodepageToUCS2;
	pstCPContext->cbConvertUCS2ToCodepage 	= cbConvertUCS2ToCodepage;
	pstCPContext->cbGetEmphasizedString 	= cbGetEmphasizedString;
	pstCPContext->pTable					= pTable;

	return ERR_OK;
}


/* -------------------------------------------------------------------------------------------------- */
#define ___________________CODEPAGE__INSERT_PUBLIC____________________________________________________
/* -------------------------------------------------------------------------------------------------- */

/*

 1. isosvc.h -  Text_Encoding_t에 enum값 추가
 3. codepage_xxx.c 파일 추가(MW_C_Register함수 이용)
 3. codepage.c - local_mwc_util_codepage_Init에 code 추가
 4. isosvc.c - local_mwc_util_DvbStrlen에 length에 enum값 추가
 5.

*/

CPContext_t *MWC_UTIL_CodePage_GetContext(eHxCP_CodePage_e eTextEncoding)
{
	return local_mwc_util_codepage_GetCPContextByTextEncoding(eTextEncoding);
}

HERROR MWC_UTIL_CodePage_Register(eHxCP_CodePage_e eTextEncoding,
						MwcCountDrawableCharNum 	cbCountDrawableCharNum,
					  	MwcGetDrawableChar			cbGetDrawableChar,
					  	MwcConvertCodepageToUCS2	cbConvertCodepageToUCS2,
					  	MwcConvertUCS2ToCodepage	cbConvertUCS2ToCodepage, /* optional */
					  	MwcGetEmphasizedString		cbGetEmphasizedString,	 /* optional */
					  	void					*pTable
)
{
	return local_mwc_util_codepage_Register(eTextEncoding, cbCountDrawableCharNum, cbGetDrawableChar, cbConvertCodepageToUCS2, cbConvertUCS2ToCodepage, cbGetEmphasizedString, pTable);
}

HERROR MWC_UTIL_CodePage_Init(void)
{
	HERROR hErr = ERR_OK;
	//default 8859-x

	hErr |= local_mwc_util_codepage_Init();

	return hErr;
}

/* -------------------------------------------------------------------------------------------------- */
#define ___________________OP_CODEPAGE_SETTING____________________________________________________
/* -------------------------------------------------------------------------------------------------- */

STATIC HERROR local_mwc_util_codepage_DvbStrGetSHYValue(HUINT8 *pucMem, HUINT32 ucMemLen, HUINT32 *pCharCodeTableLen, HUINT32 *pShyValue)
{
	HUINT32	charCodeTableLen = 0, shyValue = 0;

	if (NULL != pCharCodeTableLen)
	{
		*pCharCodeTableLen = 0;
	}

	if (pucMem == NULL || ucMemLen == 0 || pShyValue == NULL)
	{
		return ERR_FAIL;
	}
	*pShyValue = 0;

	switch (*pucMem)
	{
	case eHxCP_DVBLANG_VALUE_8859_05:
	case eHxCP_DVBLANG_VALUE_8859_06:
	case eHxCP_DVBLANG_VALUE_8859_07:
	case eHxCP_DVBLANG_VALUE_8859_08:
	case eHxCP_DVBLANG_VALUE_8859_09:
	case eHxCP_DVBLANG_VALUE_8859_10:
	case eHxCP_DVBLANG_VALUE_8859_12:
	case eHxCP_DVBLANG_VALUE_8859_13:
	case eHxCP_DVBLANG_VALUE_8859_14:
	case eHxCP_DVBLANG_VALUE_8859_15:
		shyValue= 0xAD;
		charCodeTableLen = 1;
		break;
	case eHxCP_DVBLANG_VALUE_8859_11: /* SHY 문자가 없음 */
		break;
	case eHxCP_DVBLANG_VALUE_8859:	/* charCodeTableLen 이 3 byte 계열 */
		if (ucMemLen >= 3)
		{
			charCodeTableLen = 3;
			if ((pucMem[1] == 0x69) && (pucMem[2] == 0x37))
			{
				shyValue = 0xFF;
			}
			else if ((pucMem[1] == 0x0) && (0x0 < pucMem[2] && pucMem[2] < 0x10))
			{
				if (pucMem[2] != eHxCP_DVBLANG_VALUE_8859_11)
				{
					shyValue = 0xAD;
				}
			}
		}
		break;

	case eHxCP_DVBLANG_VALUE_UTF_8:	/* 값을 모름 */
		break;
	case eHxCP_DVBLANG_VALUE_UCS2:	/* 값을 모름 */
		break;
	default:
		if (*pucMem >= 0x20)
		{
			shyValue = 0xFF;
			charCodeTableLen = 0;
		}
		break;
	}

	if (pCharCodeTableLen != NULL)
	{
		*pCharCodeTableLen = charCodeTableLen;
	}

	*pShyValue = shyValue;
	if (*pShyValue > 0)
		return ERR_OK;

	return ERR_FAIL;
}

//Event Name & Service Name 에서는 SHY 처리시 특수문자(0x8A) 도 같이 처리하도록 수정. -r21347 / UK spec.
HERROR 		MWC_UTIL_DvbStrGetSHYValue(HUINT8 *pucMem, HUINT32 ucMemLen, HUINT32 *pCharCodeTableLen, HUINT32 *pShyValue)
{
	return local_mwc_util_codepage_DvbStrGetSHYValue(pucMem, ucMemLen, pCharCodeTableLen, pShyValue);
}


/*********************** End of File ******************************/
