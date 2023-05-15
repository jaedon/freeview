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

#define		CH_1BYTE_EFFECTIVE_CH_VALUE			(CH_1BYTE)0x7F
#define		CH_1BYTE_CP1251_START_CYRILLIC		(CH_1BYTE)0x80
#define		CH_1BYTE_CP1251_END_CYRILLIC		(CH_1BYTE)0xBF
#define		CH_1BYTE_CP1251_TO_UNI_CYRILLIC		(CH_UCS2)0x0350


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/


/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

// 0x00~0x7E: 8859 spec과 같이 처리 가능, 0x7F: 제어문자, 0xC0~0xFF: 0x0350을 더하면 unicode 값
STATIC CHARMAPITEM_t WINCP_1251_UNICODE[] =
{
	{0x80, 0x0402},
	{0x81, 0x0403},
	{0x82, 0x201A},
	{0x83, 0x0453},
	{0x84, 0x201E},
	{0x85, 0x2026},
	{0x86, 0x2020},
	{0x87, 0x2021},
	{0x88, 0x20AC},
	{0x89, 0x2030},
	{0x8A, 0x0409},
	{0x8B, 0x2039},
	{0x8C, 0x040A},
	{0x8D, 0x040C},
	{0x8E, 0x040B},
	{0x8F, 0x040F},

	{0x90, 0x0452},
	{0x91, 0x2018},
	{0x92, 0x2019},
	{0x93, 0x201C},
	{0x94, 0x201D},
	{0x95, 0x2022},
	{0x96, 0x2013},
	{0x97, 0x2014},
	{0x98, 0x0020}, //TEMP
	{0x99, 0x2122},
	{0x9A, 0x0459},
	{0x9B, 0x203A},
	{0x9C, 0x045A},
	{0x9D, 0x045C},
	{0x9E, 0x045B},
	{0x9F, 0x045F},

	{0xA0, 0x00A0},
	{0xA1, 0x040E},
	{0xA2, 0x045E},
	{0xA3, 0x0408},
	{0xA4, 0x00A4},
	{0xA5, 0x0490},
	{0xA6, 0x00A6},
	{0xA7, 0x00A7},
	{0xA8, 0x0401},
	{0xA9, 0x00A9},
	{0xAA, 0x0404},
	{0xAB, 0x00AB},
	{0xAC, 0x00AC},
	{0xAD, 0x00AD},
	{0xAE, 0x00AE},
	{0xAF, 0x0407},

	{0xB0, 0x00B0},
	{0xB1, 0x00B1},
	{0xB2, 0x0406},
	{0xB3, 0x0456},
	{0xB4, 0x0491},
	{0xB5, 0x00B5},
	{0xB6, 0x00B6},
	{0xB7, 0x00B7},
	{0xB8, 0x0451},
	{0xB9, 0x2116},
	{0xBA, 0x0454},
	{0xBB, 0x00BB},
	{0xBC, 0x0458},
	{0xBD, 0x0405},
	{0xBE, 0x0455},
	{0xBF, 0x0457},

};


STATIC HERROR	local_mw_cp_ConvertWincpToUCS2(const CH_1BYTE 			*pStrWincp,
												const HINT32 			nStrWincpLen,
												HINT32 					*pnUsedByte,
												CH_UCS2 				*pUCS2Str,
												HINT32 					*pnUCS2StrLen,
												eHxCP_CodePage_e			eTextEncoding,
												CHARMAPITEM_t 	*pchMapWincptoUCS2
												)
{
	HINT32				i = 0;
	HINT32				nConvertedCharacterCount = 0;
	HINT32 				nCodeIndex = 0;
	HINT32				nLocaleCodeLen = 0;
//	HBOOL				bKnownChar = FALSE;
	CH_1BYTE			cWinCpCh = 0;
//	CH_UCS2				usDvbCh = 0;
	ConvEncoding_Err_t	hResErr = ERR_FAIL;
	eHxCP_CodePage_e		eEncoding = eHCodepage_MAX;

	HxLOG_Assert(pStrWincp);
	HxLOG_Assert(pUCS2Str);

	if((pStrWincp == NULL) || (pUCS2Str == NULL) || (pchMapWincptoUCS2 == NULL) || (eTextEncoding == eHCodepage_MAX))
	{

		HxLOG_Info("local_mw_cp_ConvertWincpToUCS2 eTextEncoding(%d) nStrWincpLen(%d)\n", eTextEncoding, nStrWincpLen);

		return ERR_FAIL;
	}

	*pnUsedByte = 0;
	*pnUCS2StrLen = 0;

	hResErr = MWC_UTIL_CodePage_GetEncodingWithLenByLocaleCode(pStrWincp, &eEncoding, &nLocaleCodeLen, eTextEncoding);

	if(hResErr == ERR_FAIL || eEncoding != eTextEncoding || nStrWincpLen <= nLocaleCodeLen)
	{
		HxLOG_Info("local_mw_cp_ConvertWincpToUCS2 eTextEncoding(%d) eTextEncoding(%d)\n", eTextEncoding, eTextEncoding);

		return ERR_FAIL;
	}

	/* Character Table 정보를 뺀 그 다음 실제 코드 위치 index from nLocaleCodeLen */
	for(i = nLocaleCodeLen; i < nStrWincpLen; i++)
	{
		cWinCpCh = pStrWincp[i];

		/* 0x00 */
		if(cWinCpCh == CH_1BYTE_NULL_CHARACTER)
		{
			hResErr = ERR_NULL_CH;
			break;
		}
		/* 0x01 ~ 0x1F */
		else if(cWinCpCh < CH_1BYTE_EFFECTIVE_CH_START_VALUE)
		{
			hResErr = ERR_WHITE_CH;
#if DECODE_CHARACTER_COPY_WHITECH
			pUCS2Str[nConvertedCharacterCount] = cWinCpCh;
#else
			continue;
#endif
		}
		/* 0x20 ~ 0x7E */
		else if(cWinCpCh <= CH_1BYTE_ASCII_END_VALUE)
		{
			/* ascii position */
			pUCS2Str[nConvertedCharacterCount] = cWinCpCh;
		}
		/* 0x7F */
		else if(cWinCpCh == CH_1BYTE_EFFECTIVE_CH_VALUE)
		{
			hResErr = ERR_WHITE_CH;
#if DECODE_CHARACTER_COPY_WHITECH
			pUCS2Str[nConvertedCharacterCount] = cWinCpCh;
#else
			continue;
#endif
		}
		/* 0x80 ~ 0xBF */
		else if((cWinCpCh >= CH_1BYTE_CP1251_START_CYRILLIC) && (cWinCpCh <= CH_1BYTE_CP1251_END_CYRILLIC))
		{
			/* to calc array nCodeIndex using character code */
			nCodeIndex = cWinCpCh - ((CH_UCS2)(0x0000 | pchMapWincptoUCS2[0].ucISOCode));
			pUCS2Str[nConvertedCharacterCount] = pchMapWincptoUCS2[nCodeIndex].usUNICODE;

			HxLOG_Print("[u+%04x]count[%d]\n", *(pUCS2Str+nConvertedCharacterCount), nConvertedCharacterCount);
			hResErr = ERR_ENCODING_OK;
		}
		/* 0xC0 ~ 0xFF */
		else
		{
			pUCS2Str[nConvertedCharacterCount] = cWinCpCh + CH_1BYTE_CP1251_TO_UNI_CYRILLIC;

			HxLOG_Print("[u+%04x]count[%d]\n", *(pUCS2Str+nConvertedCharacterCount), nConvertedCharacterCount);
		}
		nConvertedCharacterCount++;
	}

	*pnUsedByte = i;
	*pnUCS2StrLen = nConvertedCharacterCount;

	HxLOG_Info("local_mw_cp_ConvertWincpToUCS2() result error(0x%x) -\n", hResErr);

	return hResErr;
}



STATIC HERROR	local_mw_cp_ConvertWincp_1251ToUCS2(const CH_1BYTE *pStrWincp1251, const HINT32 nStrWincp1251Len, HINT32 *pnUsedByte, CH_UCS2 *pUCS2Str, HINT32 *pnUCS2StrLen)
{
	return local_mw_cp_ConvertWincpToUCS2(pStrWincp1251, nStrWincp1251Len, pnUsedByte, pUCS2Str, pnUCS2StrLen,
											eHxCP_WIN_1251, WINCP_1251_UNICODE);
}

STATIC HERROR local_mw_cp_CountDrawableCharNumWincp (HUINT8 *szString, HUINT32 *pulCharNum)
{
	HUINT32		ulCount = 0;
	HINT32		nStrLen = 0;
	HUINT32		ulUseByte = 0;
	HERROR		hErr =ERR_OK;

	nStrLen = (HINT32)MWC_UTIL_DvbStrlen (szString);

	hErr = local_mw_cp_ConvertWincp_1251ToUCS2(szString, nStrLen, &ulUseByte, NULL, &ulCount);
	if (hErr == ERR_OK && pulCharNum != NULL)
	{
		*pulCharNum = ulCount;
	}

	return hErr;

}

STATIC HERROR local_mw_cp_GetDrawableCharWincp (HUINT8 *szString, HUINT32 ulIndex,
													   HUINT8 *szCharWithCode, HUINT32 *pulCharLenWithCode,
													   HUINT8 *szCharOnly, HUINT32 *pulCharLenOnly)
{
//	CH_1BYTE	szSJISStr[4] = {0,};
//	CH_1BYTE	szSJISPointer[4] = {0,};
//	CH_UCS2		szUCS2StrPtr[4] = {0,};

	HINT32		nStrCodeLen = 0;
//	HINT32		nMaxCount = 0;
//	HINT32		nIdx = 1;
	HUINT32		ulUseStrLen = 0;


	nStrCodeLen = MWC_UTIL_GetLocaleCodeLen(szString);

	if (szCharWithCode != NULL)
	{
		HxSTD_memcpy (szCharWithCode, szString, nStrCodeLen);
		HxSTD_memcpy (szCharWithCode+nStrCodeLen, szString + ulIndex, ulUseStrLen);
	}

	if (pulCharLenWithCode != NULL)
	{
		*pulCharLenWithCode = ulUseStrLen + nStrCodeLen;
	}

	if (szCharOnly != NULL)
	{
		HxSTD_memcpy (szCharOnly,  szString + ulIndex, ulUseStrLen);
	}

	if (pulCharLenOnly != NULL)
	{
		*pulCharLenOnly = ulUseStrLen;
	}

	return ERR_OK;

}

HERROR MWC_UTIL_CodePage_RegisterWINCP(void)
{
	MWC_UTIL_CodePage_Register(eHxCP_WIN_1251,
		local_mw_cp_CountDrawableCharNumWincp, local_mw_cp_GetDrawableCharWincp,
		local_mw_cp_ConvertWincp_1251ToUCS2,
		NULL,
		NULL,
		WINCP_1251_UNICODE);

	return ERR_OK;
}



