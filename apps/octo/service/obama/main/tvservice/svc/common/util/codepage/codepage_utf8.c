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
#include <isosvc.h>


/*******************************************************************/
/********************      Debug     ********************/
/*******************************************************************/





/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/



/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/


/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

#define __________________________CODEPAGE_TO_UCS2__________________________________________


STATIC HERROR	local_mw_cp_ConvUtf8ToUCS2(const CH_UTF8 *pStrUtf8, const HINT32 nStrUtf8Len, HINT32 *pnUsedByteLen, CH_UCS2 *pUcs2Str)
{
	HUINT8 c = pStrUtf8[0];

	/* 1byte character */
	if (c < 0x20)
	{
		switch(c)
		{
		case 0x0a:
			return ERR_NEWLINE_CH;

		default:
			return ERR_CONTROL_CH;
		}
	}
	else if (c < 0x80)
	{
		*pUcs2Str = (HUINT16) c;
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
			return ERR_ILLEGAL_SEQ;//RET_ILSEQ;

		*pUcs2Str = ((HUINT16) (c & 0x1f) << 6) | (HUINT16) (pStrUtf8[1] ^ 0x80);
		*pnUsedByteLen=2;
		return ERR_OK;
	}
	/* 3byte character */
	else if (c < 0xf0)
	{
		if (nStrUtf8Len < 3)
			return ERR_TOOSHORT;
		if (!((pStrUtf8[1] ^ 0x80) < 0x40 && (pStrUtf8[2] ^ 0x80) < 0x40 && (c >= 0xe1 || pStrUtf8[1] >= 0xa0)))
			return ERR_ILLEGAL_SEQ; //RET_ILSEQ;

		*pUcs2Str = ((HUINT16) (c & 0x0f) << 12) | ((HUINT16) (pStrUtf8[1] ^ 0x80) << 6) | (HUINT16) (pStrUtf8[2] ^ 0x80);
		*pnUsedByteLen=3;
		return ERR_OK;
	}
	/* ucs2 (Universal Character Set 2)는 Utf8 문자에서 3byte글자까지만 지원한다. 이유는 Gooogling~ wjmoh */
	else
	{
		return ERR_FAIL; //RET_ILSEQ;
	}
}

STATIC ConvEncoding_Err_t	local_mw_cp_ConvertUtf8ToUCS2(const CH_UTF8 *pStrUtf8, const HINT32 nStrUtf8Len, HINT32 *pnUsedByte, CH_UCS2 *pUcs2Str, HINT32 *pnUcs2StrLen)
{
	HINT32		i = 0;
	HINT32		usedBuf = 0;
	HINT32		nConvertedCharacterCount = 0;

	ConvEncoding_Err_t	resultErr = ERR_FAIL;


	*pnUsedByte=0;
	*pnUcs2StrLen=0;

	for(i=0; i<nStrUtf8Len; /*NOT*/)
	{
		resultErr=local_mw_cp_ConvUtf8ToUCS2(pStrUtf8+i, nStrUtf8Len-i, &usedBuf, pUcs2Str+nConvertedCharacterCount);
		if(resultErr==ERR_TOOSHORT)
		{
			/* no more converted */
			++i;
			break;
		}
		else if(resultErr==ERR_ILLEGAL_SEQ)
		{
			/* find sequence */
			++i;
			continue;
		}
		else if(resultErr!=ERR_OK)
		{
			/* some error case */
			++i;
			continue;
		}

		HxLOG_Print("[u+%04x]count[%d]\n", *(pUcs2Str+nConvertedCharacterCount), nConvertedCharacterCount);

		i+=usedBuf;
		nConvertedCharacterCount++;
	}

	*pnUsedByte=i;
	*pnUcs2StrLen=nConvertedCharacterCount;


	return resultErr;

}

#if 0 //Not used..
STATIC HERROR	local_mw_cp_ConvertDvbUtf8ToUCS2(const CH_1BYTE *pStrDvbUtf8, const HINT32 nStrDvbUtf8Len, HINT32 *pnUsedByte, CH_UCS2 *pUCS2Str, HINT32 *pnUCS2StrLen)
{
	HINT32		i = 0;
	HINT32		nConvertedCharacterCount = 0;
	HINT32		nLocaleCodeLen = 0;
	HINT32		nUsedUtf8Byte = 0;
	HBOOL				bIsLocaleCode = FALSE;
	CH_1BYTE			dvbUtf8Ch = 0;
	CH_UCS2				tmpUCS2Ch = 0;
	ConvEncoding_Err_t	hResErr = ERR_FAIL;

	HxLOG_Assert(pStrDvbUtf8);
	HxLOG_Assert(pUCS2Str);
	HxLOG_Info("local_mw_cp_ConvertDvbUtf8ToUCS2 nStrDvbUtf8Len(%d)\n", nStrDvbUtf8Len);

	if(nStrDvbUtf8Len == 0)
	{
		/* check number to converte */
		HxLOG_Info("local_mw_cp_ConvertDvbUtf8ToUCS2() error: demand length is Zero -\n");

		return ERR_OK;
	}

	if((pStrDvbUtf8 == NULL) || (pUCS2Str == NULL))
	{
		/* pStrDvbUtf8 storage check */
		HxLOG_Info("local_mw_cp_ConvertDvbUtf8ToUCS2 error: storage is NULL\n");

		return ERR_FAIL;
	}


	/* check Locale Code */ /* 처음부터 locale code가 없고 0x20이상이면 8859_1이라고 생각한다. */
	if(! /*NOT*/((pStrDvbUtf8[0] >= CH_1BYTE_EFFECTIVE_CH_START_VALUE)
		||	((nStrDvbUtf8Len >= 1)
		&& (MWC_UTIL_IsEncodingValid(eHxCP_UNICODE_DVB_UTF8, pStrDvbUtf8) == TRUE))))
	{
		return ERR_FAIL;
	}

	*pnUsedByte = 0;
	*pnUCS2StrLen = 0;

	/* init */
	nLocaleCodeLen = MWC_UTIL_GetLocaleCodeLen((const HUINT8 *)pStrDvbUtf8);
	if(nLocaleCodeLen > 0)
	{
		bIsLocaleCode = TRUE;
	}

	/* Character Table 정보를 뺀 그 다음 실제 코드 위치 index from nLocaleCodeLen */
	for(i = nLocaleCodeLen; i < nStrDvbUtf8Len; i++)
	{
		dvbUtf8Ch = pStrDvbUtf8[i];

		/* 0x00 */
		if(dvbUtf8Ch == CH_1BYTE_NULL_CHARACTER)
		{
			hResErr = ERR_NULL_CH;
			break;
		}
		/* 0x01 ~ 0x1F */
		else if(dvbUtf8Ch < CH_1BYTE_EFFECTIVE_CH_START_VALUE)
		{
			hResErr = ERR_WHITE_CH;
#if DECODE_CHARACTER_COPY_WHITECH
			pUCS2Str[nConvertedCharacterCount] = dvbUtf8Ch;
#else
			continue;
#endif
		}
		/* 0x20 ~ 0x7E */
		else if(dvbUtf8Ch <= CH_1BYTE_ASCII_END_VALUE)
		{
			/* ascii position */
			pUCS2Str[nConvertedCharacterCount] = dvbUtf8Ch;
		}
		/* 0x7F ~ 0x9F */
		else if((dvbUtf8Ch >= CH_1BYTE_USERDEFINE_CODE_START_VALUE) && (dvbUtf8Ch <= CH_1BYTE_USERDEFINE_CODE_END_VALUE))
		{
			/* control ch position */
			/* 당장 emphasis 등을 어떤 문자로 바꾸지는 않는다. */
			/* nothing to do */
			hResErr = ERR_USERDEFINE_CH;

			continue;
		}
		/* 0xA0 ~ 0xFF */
		else
		{
			/* to calc array nCodeIndex using character code */
			hResErr = local_mw_cp_ConvUtf8ToUCS2((const CH_UTF8 *)(pStrDvbUtf8 + i), (const HINT32)(nStrDvbUtf8Len - i), (HINT32 *)&nUsedUtf8Byte, (CH_UCS2 *)&tmpUCS2Ch);
			if(hResErr == ERR_OK)
			{
				i += nUsedUtf8Byte;
			}
		}
		HxLOG_Print("[u+%04x]count[%d]\n", *(pUCS2Str+nConvertedCharacterCount), nConvertedCharacterCount);
		hResErr = ERR_ENCODING_OK;
		nConvertedCharacterCount++;
	}

	*pnUsedByte = (i * sizeof(CH_UCS2));
	*pnUCS2StrLen = nConvertedCharacterCount;

	HxLOG_Info("local_mw_cp_ConvertDvbUtf8ToUCS2() result error(0x%x) -\n", hResErr);

	return hResErr;
}
#endif
#define __________________________________CODEPAGE_TO_UCS4__________________________________________

STATIC HERROR	local_mw_cp_ConvUtf8ToUCS4(const CH_UTF8 *pStrUtf8, const HINT32 nStrUtf8Len, HINT32 *pnUsedByteLen, CH_UCS4 *pUcs4Str)
{
	HUINT8 c = pStrUtf8[0];

	/* 1byte character */
	if (c < 0x80)
	{
		*pUcs4Str = c;
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

		*pUcs4Str = ((HUINT16) (c & 0x1f) << 6) | (HUINT16) (pStrUtf8[1] ^ 0x80);
		*pnUsedByteLen=2;
		return ERR_OK;
	}
	/* 3byte character */
	else if (c < 0xf0)
	{
		if (nStrUtf8Len < 3)
			return ERR_TOOSHORT;
		if (!((pStrUtf8[1] ^ 0x80) < 0x40 && (pStrUtf8[2] ^ 0x80) < 0x40 && (c >= 0xe1 || pStrUtf8[1] >= 0xa0)))
			return ERR_ILLEGAL_SEQ; //RET_ILSEQ;

		*pUcs4Str = ((HUINT16) (c & 0x0f) << 12) | ((HUINT16) (pStrUtf8[1] ^ 0x80) << 6) | (HUINT16) (pStrUtf8[2] ^ 0x80);
		*pnUsedByteLen=3;
		return ERR_OK;
	}
	/* 4byte character */
	else if (c < 0xf8 && sizeof(HUINT32)*8 >= 32)
	{
		if (nStrUtf8Len < 4)
			return ERR_TOOSHORT;
		if (!((pStrUtf8[1] ^ 0x80) < 0x40 && (pStrUtf8[2] ^ 0x80) < 0x40 && (pStrUtf8[3] ^ 0x80) < 0x40 && (c >= 0xf1 || pStrUtf8[1] >= 0x90)))
			return ERR_ILLEGAL_SEQ; //RET_ILSEQ;

		*pUcs4Str = ((HUINT32) (c & 0x07) << 18) | ((HUINT32) (pStrUtf8[1] ^ 0x80) << 12) | ((HUINT32) (pStrUtf8[2] ^ 0x80) << 6) | (HUINT32) (pStrUtf8[3] ^ 0x80);
		*pnUsedByteLen=4;
		return ERR_OK;
	}
	/* 5byte character */
	else if (c < 0xfc && sizeof(HUINT32)*8 >= 32)
	{
		if (nStrUtf8Len < 5)
			return ERR_TOOSHORT;
		if (!((pStrUtf8[1] ^ 0x80) < 0x40 && (pStrUtf8[2] ^ 0x80) < 0x40
			&& (pStrUtf8[3] ^ 0x80) < 0x40 && (pStrUtf8[4] ^ 0x80) < 0x40
			&& (c >= 0xf9 || pStrUtf8[1] >= 0x88)))
			return ERR_ILLEGAL_SEQ; //RET_ILSEQ;

		*pUcs4Str = ((HUINT32) (c & 0x03) << 24)
			| ((HUINT32) (pStrUtf8[1] ^ 0x80) << 18)
			| ((HUINT32) (pStrUtf8[2] ^ 0x80) << 12)
			| ((HUINT32) (pStrUtf8[3] ^ 0x80) << 6)
			| (HUINT32) (pStrUtf8[4] ^ 0x80);
		*pnUsedByteLen=5;
		return ERR_OK;
	}
	/* 6byte character */
	else if (c < 0xfe && sizeof(HUINT32)*8 >= 32)
	{
		if (nStrUtf8Len < 6)
			return ERR_TOOSHORT;
		if (!((pStrUtf8[1] ^ 0x80) < 0x40 && (pStrUtf8[2] ^ 0x80) < 0x40
			&& (pStrUtf8[3] ^ 0x80) < 0x40 && (pStrUtf8[4] ^ 0x80) < 0x40
			&& (pStrUtf8[5] ^ 0x80) < 0x40
			&& (c >= 0xfd || pStrUtf8[1] >= 0x84)))
			return ERR_ILLEGAL_SEQ; //RET_ILSEQ;

		*pUcs4Str = ((HUINT32) (c & 0x01) << 30)
			| ((HUINT32) (pStrUtf8[1] ^ 0x80) << 24)
			| ((HUINT32) (pStrUtf8[2] ^ 0x80) << 18)
			| ((HUINT32) (pStrUtf8[3] ^ 0x80) << 12)
			| ((HUINT32) (pStrUtf8[4] ^ 0x80) << 6)
			| (HUINT32) (pStrUtf8[5] ^ 0x80);
		*pnUsedByteLen=6;
		return ERR_OK;
	}
	else
	{
		return ERR_FAIL; //RET_ILSEQ;
	}
}


ConvEncoding_Err_t	MWC_UTIL_ConvertUtf8ToUCS4(const CH_UTF8 *pStrUtf8, const HINT32 nStrUtf8Len, HINT32 *pnUsedByte, CH_UCS4 *pUcs4Str, HINT32 *pnUcs4StrLen)
{
	HINT32		i = 0;
	HINT32		nUsedBuf = 0;
	HINT32		nConvertedCharacterCount = 0;

	ConvEncoding_Err_t	resultErr=ERR_FAIL;

	HxLOG_Info("MWC_UTIL_ConvertUtf8ToUCS4 nStrUtf8Len(%d) +\n", nStrUtf8Len);

	*pnUsedByte=0;
	*pnUcs4StrLen=0;

	for(i=0; i<nStrUtf8Len; /*NOT*/)
	{
		resultErr=local_mw_cp_ConvUtf8ToUCS4(pStrUtf8+i, nStrUtf8Len-i, &nUsedBuf, pUcs4Str+nConvertedCharacterCount);
		if(resultErr==ERR_TOOSHORT)
		{
			/* no more converted */
			++i;
			break;
		}
		else if(resultErr==ERR_ILLEGAL_SEQ)
		{
			/* find sequence */
			++i;
			continue;
		}
		else if(resultErr!=ERR_OK)
		{
			/* some error case */
			++i;
			continue;
		}

		HxLOG_Print("[u+%08x]count[%d]\n", (HUINT32)pUcs4Str[nConvertedCharacterCount], nConvertedCharacterCount);

		i+=nUsedBuf;
		nConvertedCharacterCount++;
	}

	*pnUsedByte=i;
	*pnUcs4StrLen=nConvertedCharacterCount;

	HxLOG_Info("MWC_UTIL_ConvertUtf8ToUCS4() result error(0x%x) -\n", resultErr);

	return resultErr;

}

ConvEncoding_Err_t	MWC_UTIL_ConvertEncodingToUCS4(const eHxCP_CodePage_e eEncoding, const CH_1BYTE *pSrcStr, const HINT32 nSrcStrLen, HINT32 *pnUsedByte, CH_UCS4 *pUcs4Str, HINT32 *pnUcs4StrLen)
{
	HxLOG_Info("MWC_UTIL_ConvertEncodingToUCS4 eEncoding(0x%x) pSrcStr(%s) +\n", eEncoding, pSrcStr);

	/* check code page, and map the coverting function */
	switch(eEncoding)
	{
	case eHxCP_UNICODE_UTF8:
		return MWC_UTIL_ConvertUtf8ToUCS4(pSrcStr, nSrcStrLen, pnUsedByte, pUcs4Str, pnUcs4StrLen);

	default:
		HxLOG_Info("MWC_UTIL_ConvertEncodingToUCS4 error, check it\n");
		return ERR_FAIL;
	}

	HxLOG_Info("MWC_UTIL_ConvertEncodingToUCS4() -\n");

	return ERR_OK;
}

#define __________________________UCS2_TO_CODEPAGE___________________________________________


STATIC HERROR	local_mw_cp_ConvUCS2ToUtf8(const CH_UCS2 strUcs2, const HINT32 nUtf8BufferLen, HINT32 *pnUcs2Utf8Len, CH_UTF8 *pUcs2Utf8)
{
	HUINT16 count = 0;
	HUINT16	ch = strUcs2;

	if (ch < 0x80)
		count = 1;
	else if (ch < 0x800)
		count = 2;
	else //if (ch < 0x10000)
		count = 3;
#if 0
	else
		return ERR_ILLEGAL_SEQ;
#endif

		if (nUtf8BufferLen < count)
			return ERR_BUFFER_TOOSMALL;

		switch (count) { /* note: code falls through cases! */
		case 3: pUcs2Utf8[2] = 0x80 | (ch & 0x3f); ch = ch >> 6; ch |= 0x800;
		case 2: pUcs2Utf8[1] = 0x80 | (ch & 0x3f); ch = ch >> 6; ch |= 0xc0;
		case 1: pUcs2Utf8[0] = ch;
		}

		*pnUcs2Utf8Len = count;

		return ERR_OK;
}


STATIC  ConvEncoding_Err_t	local_mw_cp_ConvertUCS2ToUtf8(const CH_UCS2 *pStrUcs2, const HINT32 nStrUcs2Len, const HINT32 nBufferLen, HINT32 *pnUsedByte, CH_UTF8 *pUtf8Str, HINT32 *pnUtf8StrLen)
{
	HINT32		i = 0;
	HINT32		nConvertedCharacterCount = 0;

	ConvEncoding_Err_t	resultErr = ERR_FAIL;

	*pnUsedByte = 0;
	*pnUtf8StrLen = 0;

	for(i = 0; i < nStrUcs2Len; i++)
	{
		nConvertedCharacterCount = 0;

		resultErr = local_mw_cp_ConvUCS2ToUtf8(pStrUcs2[i], nBufferLen-(*pnUtf8StrLen), &nConvertedCharacterCount, pUtf8Str+(*pnUtf8StrLen));
		if(resultErr == ERR_BUFFER_TOOSMALL)
		{
			/* 버퍼가 모자라다, 변환 마침. */
			break;
		}
		else if(resultErr != ERR_OK)
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


	return resultErr;
}

#define __________________________Drawable___________________________________________

STATIC HERROR local_mw_cp_CountDrawableCharNumUtf8 (HUINT8 *szString, HUINT32 *pulCharNum)
{
	HUINT8			*szPos;
	HINT32			 nStrLen, ulCharNum;

	szPos = szString;
	nStrLen = (HINT32)MWC_UTIL_DvbStrlen (szString);
	ulCharNum = 0;

	while (nStrLen > 0)
	{
		// 1 byte
		if (*szPos < 0x80)
		{
			ulCharNum ++;
			szPos ++;
			nStrLen --;
		}
		else if (*szPos < 0xc2)
		{
			return ERR_FAIL;
		}
		// 2 byte
		else if (*szPos < 0xe0)
		{
			if (nStrLen < 2)			{ return ERR_FAIL; }
			ulCharNum ++;
			szPos += 2;
			nStrLen -= 2;
		}
		// 3 byte
		else if (*szPos < 0xf0)
		{
			if (nStrLen < 3)			{ return ERR_FAIL; }
			ulCharNum ++;
			szPos += 3;
			nStrLen -= 3;
		}
		else
		{
			return ERR_FAIL;
		}
	}

	*pulCharNum = ulCharNum;
	return ERR_OK;

}



STATIC HERROR local_mw_cp_GetDrawableCharUtf8 (HUINT8 *szString, HUINT32 ulIndex,
												 HUINT8 *szCharWithCode, HUINT32 *pulCharLenWithCode,
												 HUINT8 *szCharOnly, HUINT32 *pulCharLenOnly)
{
	HUINT32		 ulCount, ulCharLen;
	HINT32		 nStrLen;
	HUINT8		*szPos;

	if(szString[0] == 0x15)
	{
		szPos = szString + 1;
	}
	else
	{
		szPos = szString;
	}
	nStrLen = (HINT32)MWC_UTIL_DvbStrlen (szPos);

	for (ulCount = 0; ulCount < ulIndex; ulCount++)
	{
		if (nStrLen <= 0)					{ return ERR_FAIL; }

		if (szPos[0] < 0x80)
		{
			szPos++;
		}
		else if (szPos[0] < 0xc2)
		{
			return ERR_FAIL;
		}
		else if (szPos[0] < 0xe0)
		{
			if (nStrLen < 2)				{ return ERR_FAIL; }
			szPos += 2;
		}
		else if (szPos[0] < 0xf0)
		{
			if (nStrLen < 3)				{ return ERR_FAIL; }
			szPos += 3;
		}
		else
		{
			return ERR_FAIL;
		}
	}

	if (nStrLen <= 0)					{ return ERR_FAIL; }

	if (szPos[0] < 0x80)
	{
		ulCharLen = 1;
	}
	else if (szPos[0] < 0xc2)
	{
		return ERR_FAIL;
	}
	else if (szPos[0] < 0xe0)
	{
		if (nStrLen < 2)				{ return ERR_FAIL; }
		ulCharLen = 2;
	}
	else if (szPos[0] < 0xf0)
	{
		if (nStrLen < 3)				{ return ERR_FAIL; }
		ulCharLen = 3;
	}
	else
	{
		return ERR_FAIL;
	}

	if (szCharWithCode != NULL)
	{
		szCharWithCode[0] = 0x15;
		HxSTD_memcpy (szCharWithCode+1, szPos, ulCharLen);
	}

	if (pulCharLenWithCode != NULL)
	{
		*pulCharLenWithCode = ulCharLen + 1;
	}

	if (szCharOnly != NULL)
	{
		HxSTD_memcpy (szCharOnly, szPos, ulCharLen);
	}

	if (pulCharLenOnly != NULL)
	{
		*pulCharLenOnly = ulCharLen;
	}

	return ERR_OK;
}


#define _________________________EmphasizedString________________________________________

STATIC HERROR local_mw_cp_GetOneCharUtf8 (HUINT8 *szString, HUINT8 **pszNextString,
											HUINT8 *szOneChar, HUINT32 *pulOneCharLen)
{
	HUINT32		 ulCount, ulOneCharLen;

	if (szString[0] < 0x80)
	{
		ulOneCharLen = 1;
	}
	else if (szString[0] < 0xc2)
	{
		return ERR_FAIL;
	}
	else if (szString[0] < 0xe0)
	{
		ulOneCharLen = 2;
	}
	else if (szString[0] < 0xf0)
	{
		ulOneCharLen = 3;
	}
	else
	{
		return ERR_FAIL;
	}

	for (ulCount = 0; ulCount < ulOneCharLen; ulCount++)
	{
		if (szString[ulCount] == '\0')
		{
			return ERR_FAIL;
		}

		szOneChar[ulCount] = szString[ulCount];
	}

	szOneChar[ulOneCharLen] = '\0';
	*pszNextString = szString + ulOneCharLen;
	*pulOneCharLen = ulOneCharLen;

	return ERR_OK;
}


STATIC HERROR local_mw_cp_GetEmphasizedStringFromUtf8String (HUINT8 *szSrcStr, HUINT8 *szDstStr)
{
	HBOOL		 bStartMakeEmphasizedCode = FALSE;
	HBOOL		 bEndMakeEmphasizedCode = FALSE;
	HBOOL		 bEmphasis;
	HUINT32		 ulOneCharLen;
	HUINT8		 szOneChar[8];
	HUINT8		*szNextSrcStr;
	HERROR		 hErr;

	bEmphasis = FALSE;

	while (1)
	{
		hErr = local_mw_cp_GetOneCharUtf8 (szSrcStr, &szNextSrcStr, szOneChar, &ulOneCharLen);
		if (hErr != ERR_OK)
		{
			break;
		}

		if (bEmphasis == FALSE)
		{
			// 0xEE 0x82 0x86 : Emphasis On
			if (ulOneCharLen == 3 && szOneChar[0] == 0xEE && szOneChar[1] == 0x82 && szOneChar[2] == 0x86)
			{
				bEmphasis = TRUE;
				bStartMakeEmphasizedCode = TRUE;
			}
		}
		else
		{
			// 0xEE 0x82 0x87 : Emphasis Off
			if (ulOneCharLen == 3 && szOneChar[0] == 0xEE && szOneChar[1] == 0x82 && szOneChar[2] == 0x87)
			{
				bEmphasis = FALSE;
				if (bStartMakeEmphasizedCode == TRUE)
				{
					bEndMakeEmphasizedCode = TRUE;
				}
				continue;
			}

			HxSTD_memcpy (szDstStr, szOneChar, ulOneCharLen);
			szDstStr += ulOneCharLen;
		}

		szSrcStr = szNextSrcStr;
	}

	/* at least one emphasized word needed */
	if (bStartMakeEmphasizedCode == TRUE && bEndMakeEmphasizedCode == TRUE)
	{
		*szDstStr = '\0';
		return ERR_OK;
	}
	else
	{
		return ERR_FAIL;
	}
}



#define _________________________PUBLIC_FUNCTION__________________________________

HERROR MWC_UTIL_CodePage_RegisterUFT8(void)
{
	MWC_UTIL_CodePage_Register(eHxCP_UNICODE_UTF8,
		local_mw_cp_CountDrawableCharNumUtf8, local_mw_cp_GetDrawableCharUtf8,
		local_mw_cp_ConvertUtf8ToUCS2,
		local_mw_cp_ConvertUCS2ToUtf8,
		local_mw_cp_GetEmphasizedStringFromUtf8String,
		NULL);

	MWC_UTIL_CodePage_Register(eHxCP_UNICODE_DVB_UTF8,
		local_mw_cp_CountDrawableCharNumUtf8, local_mw_cp_GetDrawableCharUtf8,
		local_mw_cp_ConvertUtf8ToUCS2,
		local_mw_cp_ConvertUCS2ToUtf8,
		local_mw_cp_GetEmphasizedStringFromUtf8String,
		NULL);

	return ERR_OK;
}

