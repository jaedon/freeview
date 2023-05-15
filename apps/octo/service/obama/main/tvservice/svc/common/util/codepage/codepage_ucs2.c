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

STATIC HERROR	local_mw_cp_ConvertDvbUCS2ToUCS2(const CH_1BYTE *pStrDvbUCS2, const HINT32 nStrDvbUCS2Len, HINT32 *pnUsedByte, CH_UCS2 *pUCS2Str, HINT32 *pnUCS2StrLen)
{
	HINT32		i = 0;
	HINT32		nConvertedCharacterCount = 0;
	HINT32		nLocaleCodeLen = 0;
	HINT32		rest = 0;
	HINT32		nStrDvbUCS2ByteLen = 0;
	HBOOL				bIsLocaleCode = FALSE;
	CH_UCS2				dvbUcs2Ch = 0;
	CH_UCS2				*pStrUCS2 = NULL;
	ConvEncoding_Err_t	hResErr = ERR_FAIL;
	eHxCP_CodePage_e		eInputStrEncoding = eHCodepage_MAX;

	HxLOG_Assert(pStrDvbUCS2);
	HxLOG_Assert(pUCS2Str);
	HxLOG_Info("local_mw_cp_ConvertDvbUCS2ToUCS2 nStrDvbUCS2Len(%d)\n", nStrDvbUCS2Len);

	if(nStrDvbUCS2Len == 0)
	{
		/* check number to converte */
		HxLOG_Info("local_mw_cp_ConvertDvbUCS2ToUCS2() error: demand length is Zero -\n");

		return ERR_OK;
	}

	if((pStrDvbUCS2 == NULL) || (pUCS2Str == NULL))
	{
		/* pStrDvbUCS2 storage check */
		HxLOG_Info("local_mw_cp_ConvertDvbUCS2ToUCS2 error: storage is NULL\n");

		return ERR_FAIL;
	}

	pStrUCS2 = (CH_UCS2 *)pStrDvbUCS2;

	/* check Locale Code */ /* 처음부터 locale code가 없고 0x20이상이면 8859_1이라고 생각한다. */
	if(! /*NOT*/((pStrUCS2[0] >= CH_UCS2_EFFECTIVE_CH_START_VALUE)
		||	((nStrDvbUCS2Len >= 1)
		&& (MWC_UTIL_IsEncodingValid(eHxCP_UNICODE_DVB_UCS2, pStrDvbUCS2) == TRUE))	))
	{
		return ERR_FAIL;
	}

	*pnUsedByte = 0;
	*pnUCS2StrLen = 0;

	nStrDvbUCS2ByteLen = nStrDvbUCS2Len / 2;

	/* init */
	nLocaleCodeLen = MWC_UTIL_GetLocaleCodeLen((const HUINT8 *)pStrDvbUCS2);
	eInputStrEncoding = MWC_UTIL_GetEncodingByLocaleCode((const CH_1BYTE *)pStrDvbUCS2);
	rest = nStrDvbUCS2Len % 2;
	if((eInputStrEncoding == eHxCP_UNICODE_DVB_UCS2) && (rest == 1) )//&& (nLocaleCodeLen == 1) && (pStrDvbUCS2[0] == 0x11))
	{
		bIsLocaleCode = TRUE;
		pStrUCS2 = (CH_UCS2 *)(pStrDvbUCS2 + nLocaleCodeLen);

		//rest = (nStrDvbUCS2Len - nLocaleCodeLen) % 2;
		nStrDvbUCS2ByteLen = (nStrDvbUCS2Len - nLocaleCodeLen) / 2;
	}

	/* Character Table 정보를 뺀 그 다음 실제 코드 위치 index from nLocaleCodeLen */
	for(i = 0; i < nStrDvbUCS2ByteLen; i++)
	{
		dvbUcs2Ch = pStrUCS2[i];

		/* 0x0000 */
		if(dvbUcs2Ch == CH_UCS2_NULL_CHARACTER)
		{
			hResErr = ERR_NULL_CH;
			break;
		}
		/* 0x0001 ~ 0x001F */
		else if(dvbUcs2Ch < CH_UCS2_EFFECTIVE_CH_START_VALUE)
		{
			hResErr = ERR_WHITE_CH;
#if DECODE_CHARACTER_COPY_WHITECH
			pUCS2Str[nConvertedCharacterCount] = dvbUcs2Ch;
#else
			continue;
#endif
		}
		/* 0x0020 ~ 0x007E */
		else if(dvbUcs2Ch <= CH_UCS2_ASCII_END_VALUE)
		{
			/* ascii position */
			pUCS2Str[nConvertedCharacterCount] = dvbUcs2Ch;
		}
		/* 0x007F ~ 0x009F */
		else if((dvbUcs2Ch >= CH_UCS2_USERDEFINE_CODE_START_VALUE) && (dvbUcs2Ch <= CH_UCS2_USERDEFINE_CODE_END_VALUE))
		{
			/* control ch position */
			/* 당장 emphasis 등을 어떤 문자로 바꾸지는 않는다. */
			/* nothing to do */
			hResErr = ERR_USERDEFINE_CH;

			continue;
		}
		/* 0x00A0 ~ 0xFFFF */
		else
		{
			/* to calc array nCodeIndex using character code */
			pUCS2Str[nConvertedCharacterCount] = dvbUcs2Ch;
		}
		HxLOG_Print("[u+%04x]count[%d]\n", *(pUCS2Str+nConvertedCharacterCount), nConvertedCharacterCount);
		hResErr = ERR_ENCODING_OK;
		nConvertedCharacterCount++;
	}

	*pnUsedByte = (i * sizeof(CH_UCS2));
	*pnUCS2StrLen = nConvertedCharacterCount;

	HxLOG_Info("local_mw_cp_ConvertDvbUCS2ToUCS2() result error(0x%x) -\n", hResErr);

	return hResErr;
}

#if 0 //Not used..
STATIC HERROR	local_mw_cp_ConvertUCS2ToUCS2(const CH_1BYTE *pStrSrcUCS2, const HINT32 nStrUCS2Len, HINT32 *pnUsedByte, CH_UCS2 *pUCS2Str, HINT32 *pnUCS2StrLen)
{
	*pnUCS2StrLen = nStrUCS2Len;
	*pnUsedByte = nStrUCS2Len;
	HxSTD_memcpy(pUCS2Str, pStrSrcUCS2, nStrUCS2Len);

	return 	ERR_OK;
}

STATIC HERROR	local_mw_cp_ConvertUCS2ToUCS2nBuffer(const CH_1BYTE *pStrSrcUCS2, const HINT32 nStrUCS2Len, const HINT32 nBuffer, HINT32 *pnUsedByte, CH_UCS2 *pUCS2Str, HINT32 *pnUCS2StrLen)
{
	HINT32 ulSize = nStrUCS2Len;

	if(ulSize > nBuffer)
	{
		ulSize = nBuffer;
	}
	*pnUCS2StrLen = ulSize;
	*pnUsedByte = ulSize;

	HxSTD_memcpy(pUCS2Str, pStrSrcUCS2, ulSize);

	return 	ERR_OK;
}
#endif


#define __________________________UCS2_TO_CODEPAGE___________________________________________

#define __________________________Drawable___________________________________________


STATIC HERROR local_mw_cp_GetDrawableCharUcs2 (HUINT8 *szString, HUINT32 ulIndex,
												 HUINT8 *szCharWithCode, HUINT32 *pulCharLenWithCode,
												 HUINT8 *szCharOnly, HUINT32 *pulCharLenOnly)
{
	HUINT32		 ulCount;
	HUINT8		*szPos;

	szPos = szString;

	for (ulCount = 0; ulCount < ulIndex; ulCount+=2)
	{
		if (szPos[0] == 0x00 && szPos[1] == 0x00)			{ return ERR_FAIL; }
		szPos += 2;
	}

	if (szPos[0] == 0x00 && szPos[1] == 0x00)			{ return ERR_FAIL; }

	if (szCharWithCode != NULL)
	{
		HxSTD_memcpy (szCharWithCode, szPos, 2);
	}

	if (pulCharLenWithCode != NULL)
	{
		*pulCharLenWithCode = 2;
	}

	if (szCharOnly != NULL)
	{
		HxSTD_memcpy (szCharOnly, szPos, 2);
	}

	if (pulCharLenOnly != NULL)
	{
		*pulCharLenOnly = 2;
	}

	return ERR_OK;
}

STATIC HERROR local_mw_cp_CountDrawableCharNumUcs2 (HUINT8 *szString, HUINT32 *pulCharNum)
{
	HUINT32		 ulCharNum;
	HUINT8		*szPos;

	szPos = szString;
	ulCharNum = 0;

	while (szPos[0] != 0x00 || szPos[1] != 0x00)
	{
		ulCharNum ++;
		szPos += 2;
	}

	*pulCharNum = ulCharNum;
	return ERR_OK;
}


#define _________________________EmphasizedString________________________________________

#define _________________________PUBLIC_FUNCTION__________________________________


HERROR MWC_UTIL_CodePage_RegisterUCS2(void)
{
	MWC_UTIL_CodePage_Register(eHxCP_UNICODE_UCS2,
		local_mw_cp_CountDrawableCharNumUcs2, local_mw_cp_GetDrawableCharUcs2,
		local_mw_cp_ConvertDvbUCS2ToUCS2,
		NULL,
		NULL,
		NULL);

	MWC_UTIL_CodePage_Register(eHxCP_UNICODE_DVB_UCS2,
		local_mw_cp_CountDrawableCharNumUcs2, local_mw_cp_GetDrawableCharUcs2,
		local_mw_cp_ConvertDvbUCS2ToUCS2,
		NULL,
		NULL,
		NULL);

	return ERR_OK;
}


