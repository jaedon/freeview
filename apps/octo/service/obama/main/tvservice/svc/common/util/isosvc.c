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

#include <hlib.h>
#include <silib.h>
//#include <mwc_codepage.h>
#include <octo_common.h>

/* local header file */
#include "isosvc.h"
#include "charactermap.h"


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





#define		NOT_USED_FUNCTION	0

#define DVB_UTF8_CHAR_NUM(ch, num)		{ if  (ch < 0xC0) num = 1; else if (ch < 0xE1) num = 2; else if (ch < 0xF0) num = 3;  else num = 4; }



/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/


/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

// SI Source 등이 Encoding 되는 Type
STATIC SiTxtEncodedType_t		 s_IsoSvc_eSiEncodedType = eSITEXT_ENCODED_300468_EUROPE;

#if defined(CONFIG_PROD_LANG_ARABIC)
HINT32	CurrentArabicPersian = eLangID_Arabic;
#endif

// 실제 Spec에 겹치지 않는 코드로 0x10 0x69 0x37 으로 시작하는 것을 강제적으로 6937로 처리하도록 한다.


mwUtIso639Font	g_iso639EncodingTable[] =
{
	{	"sqi",		"alb",		"alb",		"\x05",			1	},		//"Albanian"
	{	"cat",		"cat",		"cat",		"\x05",			1	},		//"Catalan"
	{	"dan",		"dan",		"dan",		"\x05",			1	},		//"Danish"
	{	"nla",		"dut",		"nld",		"\x05",			1	},		//"Dutch"
#if 0
#ifdef SUPPORT_ISO6937
	{	"eng",		"eng",		"eng",		"\x10\x69\x37",	3	},		//"English"
#else
	{	"eng",		"eng",		"eng",		"\x05",			1	},		//"English"
#endif
#else	/* 6937은 그냥 0x20~0xff 그대로 두면 된다. */
	{	"eng",		"eng",		"eng",		"",			0	},		//"English"
#endif
	{	"fao",		"fao",		"fao",		"\x05",			1	},		//"Faroese"
	{	"fin",		"fin",		"fin",		"\x05",			1	},		//"Finnish"
	{	"fra",		"fre",		"fre",		"\x05",			1	},		//"French"
	{	"deu",		"ger",		"ger",		"\x05",			1	},		//"German"
	{	"gai",		"iri",		"gle",		"\x05",			1	},		//"Irish"
	{	"isl",		"ice",		"ice",		"\x05",			1	},		//"Icelandic"
	{	"ita",		"ita",		"ita",		"\x05",			1	},		//"Italian"
	{	"nor",		"nor",		"nor",		"\x05",			1	},		//"Norwegian"
	{	"por",		"por",		"por",		"\x05",			1	},		//"Portuguese"
	{	"esl",		"spa",		"esp",		"\x05",			1	},		//"Spanish"
	{	"sve",		"swe",		"swe",		"\x05",			1	},		//"Swedish"
	{	"tur",		"tur",		"tur",		"\x05",			1	},		//"Turkish"
	{	"tha",		"tha",		"tha",		"\x07",			1	},		//"Thai"
	{	"ara",		"ara",		"ara",		"\x02", 		1	},		//"Arabic"
	{	"per",		"fas",		"per",		"\x02",			1	},		//"Persian"
	//	{	"ces",		"cze",		"cze",		"\x10\x00\x02",	3	},		//"Czech"
	{	"ces",		"cze",		"cze",		"\x10\x69\x37",	3	},		//"Czech"  : 체코는 6937로 처리하고자 한다.
	{	"hrv",		"scr",		"scr",		"\x10\x00\x02", 3	},		//"Croatian"
	{	"hun",		"hun",		"hun",		"\x10\x00\x02", 3	},		//"Hungrian"
	{	"pol",		"pol",		"pol",		"\x10\x00\x02", 3	},		//"Polish"
	{	"ron",		"rum",		"rum",		"\x10\x00\x02", 3	},		//"Romanian"
	{	"slk",		"slo",		"slo",		"\x10\x00\x02", 3	},		//"Slovak"
	{	"slv",		"slv",		"slv",		"\x10\x00\x02", 3	},		//"Slovenian"
	{	"ell",		"gre",		"gre",		"\x10\x00\x07",	3	},		//"Greek"
	{	"eur",		"eur",		"eur",		"\x10\x00\x0F",	3	},		//"8859-15"
	{	"\0\0\0",	"\0\0\0",	"\0\0\0",	"",			0	},		// no matching iso code
};

#define		SETTING_DEFAULT_ENCODING	eHxCP_ISO_6937

eHxCP_CodePage_e		s_SystemEncoding = eHxCP_ISO_300468;


#define		NEW_DVB_STRFUNCS	1

#if defined(CONFIG_MWC_STRING_DECOMPRESS_300468)
extern HBOOL UTIL_STR_Decompress(int nCompressMethod, unsigned char *szCompressedStr, unsigned long ulCompStrLen, void **szDestStrBuf,  HUINT32 *pulDecompStrLen, HUINT8 *ucCodeType);
#endif
/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

#define	___________PROTO_TYPES____________________________

#if (NOT_USED_FUNCTION)
STATIC HERROR	local_mwc_util_ConvertSingleByteToUCS4(const CH_1BYTE *pSingleByteStr, const HINT32 nSingleByteStrLen, HINT32 *pnUsedByte, CH_UCS4 *pUcs4Str, HINT32 *pnUcs4StrLen);
#endif
STATIC HERROR	local_mwc_util_Convert300468ToUtf8(const CH_1BYTE *pStr300468, const HINT32 nStr300468Len, const HINT32 nBufferLen, HINT32 *pnUsedByte, CH_UTF8 *pUtf8Str, HINT32 *pnUtf8StrLen);
STATIC HERROR	local_mwc_util_GetSprintToken (HUINT8 *szString, HUINT8 *szTokenString, HUINT32 *pulTokenSize);
STATIC	HINT32	local_mwc_util_Compare639(const void *str639_key, const void *str639_item);

//cpine	___________DEFINITION_____________________________
STATIC HERROR		local_mwc_util_GetEncodingWithLenByLocaleCode(const CH_1BYTE *pcStr, eHxCP_CodePage_e *peEncoding, HINT32 *pulEncodingLen, eHxCP_CodePage_e eDefaultEncoding)
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

#if defined(CONFIG_PROD_LANG_ARABIC)
void	MWC_UTIL_RegisterCurrentArabicPersian(HINT32 langID)
{
	if ( langID != NULL)
	{
		CurrentArabicPersian = langID;
	}
	else
	{
		CurrentArabicPersian = eLangID_Arabic;
	}
	HxLOG_Info("[MWC_UTIL_RegisterCurrentArabicPersian] current language (%d) \n", CurrentArabicPersian);
}
#endif


STATIC HUINT8 	local_mwc_util_MakeUpperCase(HUINT8 ch)
{
	if ( isalpha(ch) ) {
		if ( ch >= 'a' && ch <= 'z' ) {
			ch += ('A' - 'a') ;
		}
	}
	return ch ;
}


STATIC HUINT8 	local_mwc_util_MakeLowerCase(HUINT8 ch)
{
	if ( isalpha(ch) ) {
		if ( ch >= 'A' && ch <= 'Z' ) {
			ch += ('a' - 'A') ;
		}
	}

	return ch ;
}

STATIC		HUINT32 local_mwc_util_DvbStrlenBy1Byte(const CH_1BYTE *pcStr)
{
	return strlen(pcStr);
}

STATIC		HUINT32	local_mwc_util_DvbStrlenBy2Byte(const CH_1BYTE *pcStr)
{
	const HUINT8	*pcIdx = pcStr;

	HUINT16 *p2cIdx = NULL;

	p2cIdx = (HUINT16 *)(pcIdx);

	while (*p2cIdx++)
	{
		if(*p2cIdx == 0x0000)
		{
			break;
		}
	}
	pcIdx = (HUINT8 *)p2cIdx;
	return (pcIdx - pcStr);
}

STATIC 		HUINT32 local_mwc_util_DvbStrlen(const CH_1BYTE *pcStr, eHxCP_CodePage_e	eEncoding)
{
	HUINT32 ulCount = 0;

	switch(eEncoding)
	{
		/* 2byte 문자 : NULL(0x0000) 으로 end */
	case eHxCP_UNICODE_UCS2:
	case eHxCP_UNICODE_DVB_UCS2:
		ulCount = local_mwc_util_DvbStrlenBy2Byte(pcStr);
		break;

		/* 1byte 문자 : NULL(0x00) 으로 end */
	default:
		ulCount = local_mwc_util_DvbStrlenBy1Byte(pcStr);
		break;
	}

	return ulCount;
}


void		MWC_UTIL_ConvertCharUpper(HUINT8 *pStr)
{
	if(pStr == NULL)	return;

	*pStr = local_mwc_util_MakeUpperCase(*pStr);
}

void		MWC_UTIL_ConvertCharLower(HUINT8 *pStr)
{
	if(pStr == NULL)	return;

	*pStr = local_mwc_util_MakeLowerCase(*pStr);
}

void 		MWC_UTIL_ConvertStringUpper(HUINT8 *string)
{
	HUINT32	strLen, i ;

	strLen = MWC_UTIL_DvbStrlen(string) ;
	for ( i = 0 ; i < strLen ; i++ ) {
		string[i] = local_mwc_util_MakeUpperCase(string[i]) ;
	}
}



/****************************************************************************
Description	:
Input		:
Output		:
Return		:
****************************************************************************/
HUINT32 MWC_UTIL_EliminateShyAndControlCharacter(HUINT8 *string, HUINT32 strLen)
{
	HUINT32	resultLen;
	HUINT32	orgStrLen = strLen;
	HUINT32	i, j ;
	HUINT32	thisCharByteNum;

	if (string == NULL)
		return strLen;
	if (strLen == 0)
		return 0;
	resultLen = strLen;

	i = j = 0;
	for ( ; i < orgStrLen ; )
	{
		DVB_UTF8_CHAR_NUM(string[i], thisCharByteNum);
		if ((string[i] == 0xC2) && (string[i+1] >= 0x80 && string[i+1] <= 0x9F))
		{
			i += thisCharByteNum;
		}
		else if ((string[i] == 0xC2) && (string[i+1] ==0xAD))	/* UTF8 SHY */
		{
			i += thisCharByteNum;
		}
		else if ((string[i] == 0xEE) && (string[i+1] == 0x82) && (string[i+2] >= 0x80 && string[i+2] <= 0x9F))
		{
			i += thisCharByteNum;
		}
		else
		{
			if (j < i)
			{
				if (j + thisCharByteNum > orgStrLen)
					break;
				HxSTD_MemCopy(&string[j], &string[i], thisCharByteNum);
			}
			j += thisCharByteNum;
			i += thisCharByteNum;
		}


	}
	if (j < orgStrLen)
		string[j] = 0 ;
	resultLen = j;

	return resultLen;
}


HBOOL		MWC_UTIL_IsIso639Language(HUINT8 *pcInCode)
{
	HBOOL bRetVal = TRUE;

	if ((pcInCode[0] < 'A') || (pcInCode[0] > 'z'))
	{
		bRetVal = FALSE;
	}
	if ((pcInCode[1] < 'A') || (pcInCode[1] > 'z'))
	{
		bRetVal = FALSE;
	}
	/*
	if ((pcInCode[2] < 'A') || (pcInCode[2] > 'z'))
	{
		bRetVal = FALSE;
	}
	*/
	return bRetVal;
}

HUINT32		MWC_UTIL_SelectIso639LanguageEncoding(HUINT8 *pcInCode, HUINT8 *pcOutFont, HUINT32 *punLen)
{
	HINT32	i = 0;

	if(pcInCode[0] <= 'Z') { pcInCode[0] += 32; }	// 'a' - 'A' = 32
	if(pcInCode[1] <= 'Z') { pcInCode[1] += 32; }
	if(pcInCode[2] <= 'Z') { pcInCode[2] += 32; }


	while ( g_iso639EncodingTable[i].acCode1[0] != (HUINT8)NULL)
	{
		// InCode가 Code1, Code2, Code3 중 하나라도 일치하면, ...
		if( !VK_strncmp( (HINT8 *)pcInCode, (HINT8 *)g_iso639EncodingTable[i].acCode1, 3 )
			|| !VK_strncmp( (HINT8 *)pcInCode, (HINT8 *)g_iso639EncodingTable[i].acCode2, 3 )
			|| !VK_strncmp( (HINT8 *)pcInCode, (HINT8 *)g_iso639EncodingTable[i].acCode3, 3 ) )
		{
			*punLen = g_iso639EncodingTable[i].unFontSelLen;
			if(*punLen != 0)
			{
				HxSTD_MemCopy( pcOutFont, g_iso639EncodingTable[i].acFontSel, *punLen );
			}

			return (ERR_OK);
		}

		i++;
	}

	*punLen = 0;

	return ERR_OK;
}


HBOOL		MWC_UTIL_DvbBeInString(HINT8 *szObj, HINT8 *szString)
{
	HUINT32		ulObjLen, ulObjOffset, ulStrLen, ulStrOffset;
	HUINT32		i;
	HINT8		*Obj, *String;


	//	ulObjLen = strlen((HINT8 *)szObj);
	//	ulStrLen = strlen((HINT8 *)szString);
	ulObjLen = (HUINT32)MWC_UTIL_DvbStrlen((HUINT8 *)szObj);
	ulObjOffset = ulObjLen - (HUINT32)MWC_UTIL_DvbStrlenStrOnly((HUINT8 *)szObj);
	ulStrLen = (HUINT32)MWC_UTIL_DvbStrlen((HUINT8 *)szString);
	ulStrOffset = ulStrLen - (HUINT32)MWC_UTIL_DvbStrlenStrOnly((HUINT8 *)szString);

	if(ulObjLen > ulStrLen)
	{
		return FALSE;
	}


	Obj = (HINT8 *)OxMW_Malloc(sizeof(HINT8)*(ulObjLen+1));
	if(!Obj) return FALSE;

	for(i=0; i<ulObjLen; i++)
	{
		*(Obj+i) = ( ((*(szObj+ulObjOffset+i) >= 'A') && (*(szObj+ulObjOffset+i) <= 'Z')) ? (HINT8)(*(szObj+ulObjOffset+i)+'a'-'A') : *(szObj+ulObjOffset+i) );
	}
	*(Obj+i) = '\0';


	String = (HINT8 *)OxMW_Malloc(sizeof(HINT8)*(ulStrLen+1));
	if(!String)
	{
		OxMW_Free(Obj);
		return FALSE;
	}

	for(i=0; i<ulStrLen; i++)
	{
		*(String+i) = ( ((*(szString+ulStrOffset+i) >= 'A') && (*(szString+ulStrOffset+i) <= 'Z')) ? (HINT8)(*(szString+ulStrOffset+i)+'a'-'A') : *(szString+ulStrOffset+i) );
	}
	*(String+i) = '\0';


	if( strstr((HINT8 *)String, (HINT8 *)Obj) )
	{
		OxMW_Free(Obj);
		OxMW_Free(String);
		return TRUE;
	}
	else
	{
		OxMW_Free(Obj);
		OxMW_Free(String);
		return FALSE;
	}
}


HBOOL		MWC_UTIL_DvbBeInString_CaseSensitive(HINT8 *szObj, HINT8 *szString)
{
	HUINT32		ulObjLen, ulObjOffset, ulStrLen, ulStrOffset;

	ulObjLen = (HUINT32)MWC_UTIL_DvbStrlen((HUINT8 *)szObj);
	ulObjOffset = ulObjLen - (HUINT32)MWC_UTIL_DvbStrlenStrOnly((HUINT8 *)szObj);
	ulStrLen = (HUINT32)MWC_UTIL_DvbStrlen((HUINT8 *)szString);
	ulStrOffset = ulStrLen - (HUINT32)MWC_UTIL_DvbStrlenStrOnly((HUINT8 *)szString);

	if(ulObjLen > ulStrLen)
	{
		return FALSE;
	}

	/* 국가 코드는 들어가면 안됨 */
	if(strstr((szString + ulStrOffset), (szObj + ulObjOffset)) == NULL)
	{
		return FALSE;
	}

	return TRUE;
}


HBOOL		MWC_UTIL_DvbBeInStringn(HINT8 *szObj, HINT32 size, HINT8 *szString)
{
	HUINT32		ulObjLen, ulObjOffset, ulStrLen, ulStrOffset;
	HUINT32		i;
	HINT8		*Obj, *String;


	//	ulObjLen = strlen((HINT8 *)szObj);
	//	ulStrLen = strlen((HINT8 *)szString);
	ulObjLen = (HUINT32)MWC_UTIL_DvbStrlen((HUINT8 *)szObj);
	ulObjOffset = ulObjLen - (HUINT32)MWC_UTIL_DvbStrlenStrOnly((HUINT8 *)szObj);
	ulStrLen = (HUINT32)size;
	ulStrOffset = 0;

	if(ulObjLen > ulStrLen)
	{
		return FALSE;
	}


	Obj = (HINT8 *)OxMW_Malloc(sizeof(HINT8)*(ulObjLen+1));
	if(!Obj) return FALSE;

	for(i=0; i<ulObjLen; i++)
	{
		*(Obj+i) = ( ((*(szObj+ulObjOffset+i) >= 'A') && (*(szObj+ulObjOffset+i) <= 'Z')) ? (HINT8)(*(szObj+ulObjOffset+i)+'a'-'A') : *(szObj+ulObjOffset+i) );
	}
	*(Obj+i) = '\0';


	String = (HINT8 *)OxMW_Malloc(sizeof(HINT8)*(ulStrLen+1));
	if(!String)
	{
		OxMW_Free(Obj);
		return FALSE;
	}

	for(i=0; i<ulStrLen; i++)
	{
		*(String+i) = ( ((*(szString+ulStrOffset+i) >= 'A') && (*(szString+ulStrOffset+i) <= 'Z')) ? (HINT8)(*(szString+ulStrOffset+i)+'a'-'A') : *(szString+ulStrOffset+i) );
	}
	*(String+i) = '\0';


	if( strstr((HINT8 *)String, (HINT8 *)Obj) )
	{
		OxMW_Free(Obj);
		OxMW_Free(String);
		return TRUE;
	}
	else
	{
		OxMW_Free(Obj);
		OxMW_Free(String);
		return FALSE;
	}
}

HUINT32		MWC_UTIL_DvbStrlen(const HUINT8 *pcStr)
{
//	HINT32			ulCount = 0;
//	HINT32			ulStrlen = 0;
//	eHxCP_CodePage_e eEncoding = eHCodepage_MAX;

	if(pcStr == NULL) return 0;

	//local_mwc_util_GetEncodingWithLenByLocaleCode(pcStr, &eEncoding, &ulCount, SETTING_DEFAULT_ENCODING);
	//ulStrlen = local_mwc_util_DvbStrlen(pcStr+ulCount, eEncoding);

	return HxSTD_StrLen(pcStr);

//	return ulStrlen + ulCount;
}

/* eliminate character to select code table */
HUINT32		MWC_UTIL_DvbStrlenStrOnly(const HUINT8 *pcStr)
{
	HINT32			ulCount = 0;
	HINT32			ulStrlen = 0;
	eHxCP_CodePage_e eEncoding = eHCodepage_MAX;

	if(pcStr == NULL)	 return 0;

	local_mwc_util_GetEncodingWithLenByLocaleCode(pcStr, &eEncoding, &ulCount, SETTING_DEFAULT_ENCODING);
	ulStrlen = local_mwc_util_DvbStrlen(pcStr+ulCount, eEncoding);

	return ulStrlen;
}


HUINT8		*MWC_UTIL_DvbStrcpy(HUINT8 *pcDst, const HUINT8 *pcSrc)
{
	HINT32			ulCount = 0;
	HINT32			ulStrLen = 0;
	eHxCP_CodePage_e eEncoding = eHCodepage_MAX;

	if(pcDst == NULL)	return NULL;
	*pcDst = 0;

	if(pcSrc == NULL)	return NULL;

	local_mwc_util_GetEncodingWithLenByLocaleCode(pcSrc, &eEncoding, &ulCount, eHCodepage_MAX);
	ulStrLen = local_mwc_util_DvbStrlen(pcSrc+ulCount, eEncoding);
	if(ulStrLen+ulCount == 0)
	{
		*pcDst = 0;
		return NULL;
	}

	ulStrLen+= ulCount;
	HxSTD_MemCopy(pcDst, pcSrc, ulStrLen);
	pcDst[ulStrLen] = '\0';

	return ( pcDst );

}

HUINT8		*MWC_UTIL_DvbStrcpyStrOnly(HUINT8 *pcDst, const HUINT8 *pcSrc)
{
	HINT32			ulCount = 0;
	HINT32			ulStrLen = 0;
	eHxCP_CodePage_e eEncoding = eHCodepage_MAX;

	if(pcDst == NULL)	return NULL;
	*pcDst = 0;

	if(pcSrc == NULL)	return NULL;

	local_mwc_util_GetEncodingWithLenByLocaleCode(pcSrc, &eEncoding, &ulCount, eHCodepage_MAX);
	ulStrLen = local_mwc_util_DvbStrlen(pcSrc+ulCount, eEncoding);
	if(ulStrLen == 0)
	{
		*pcDst = 0;
		return NULL;
	}

	HxSTD_MemCopy(pcDst, pcSrc+ulCount, ulStrLen);
	pcDst[ulStrLen] = '\0';

	return ( pcDst );
}

HUINT8		*UTIL_DvbStrcpyStrOnlySelf(HUINT8 *pcSrc)
{
	HINT32			ulCount = 0;
	HINT32			ulStrLen = 0;
	HUINT8 			pcDst[512];
	eHxCP_CodePage_e eEncoding = eHCodepage_MAX;

	if(pcSrc == NULL)	return NULL;

	local_mwc_util_GetEncodingWithLenByLocaleCode(pcSrc, &eEncoding, &ulCount, eHCodepage_MAX);
	ulStrLen = local_mwc_util_DvbStrlen(pcSrc+ulCount, eEncoding);

	if(ulStrLen == 0)
	{
		return NULL;
	}
	HxSTD_MemSet(pcDst, 0, 512);
	HxSTD_MemCopy(pcDst, pcSrc+ulCount, ulStrLen);
	pcDst[ulStrLen] = '\0';
	HxSTD_MemCopy(pcSrc, pcDst, ulStrLen);
	pcSrc[ulStrLen] = '\0';

	return ( pcSrc );
}

HUINT8		*MWC_UTIL_DvbStrncpy(HUINT8 *pcDst, const HUINT8 *pcSrc, HUINT32 unLen)
{
	HINT32			ulCount = 0;
	HINT32			ulStrLen = 0, ulCopyLen = 0;
	eHxCP_CodePage_e eEncoding = eHCodepage_MAX;

	if(pcDst == NULL)
		return NULL;

	local_mwc_util_GetEncodingWithLenByLocaleCode(pcSrc, &eEncoding, &ulCount, eHCodepage_MAX);
	ulStrLen = local_mwc_util_DvbStrlen(pcSrc+ulCount, eEncoding);
	if(ulStrLen + ulCount == 0)
	{
		*pcDst = 0;
		return NULL;
	}

	ulStrLen = ulStrLen + ulCount + 1;
	ulCopyLen = (ulStrLen < unLen) ? ulStrLen : unLen;

	HxSTD_MemCopy(pcDst, pcSrc, ulCopyLen);

	return ( pcDst );
}


HUINT8		*MWC_UTIL_DvbStrncpyStrOnly(HUINT8 *pcDst, const HUINT8 *pcSrc, HUINT32 unLen)
{
	HINT32			ulCount = 0;
	HINT32			ulStrLen = 0;
	eHxCP_CodePage_e eEncoding = eHCodepage_MAX;

	if(pcDst == NULL)
		return NULL;

	local_mwc_util_GetEncodingWithLenByLocaleCode(pcSrc, &eEncoding, &ulCount, eHCodepage_MAX);
	ulStrLen = local_mwc_util_DvbStrlen(pcSrc+ulCount, eEncoding);
	if(ulStrLen == 0)
	{
		*pcDst = 0;
		return NULL;
	}

	ulStrLen = ulStrLen + 1;
	ulStrLen = (ulStrLen < unLen) ? ulStrLen : unLen;

	HxSTD_MemCopy(pcDst, pcSrc+ulCount, ulStrLen);

	return ( pcDst );
}


HUINT8		*MWC_UTIL_DvbStrcat(HUINT8 *pcDst, const HUINT8 *pcSrc)
{
	const HUINT8		*pcSrcIdx = pcSrc;
	HUINT8		*pcDstIdx = pcDst;
	HUINT32		ulDstStrLen, ulSrcStrLen;

	ulDstStrLen = MWC_UTIL_DvbStrlen(pcDst);
	ulSrcStrLen = MWC_UTIL_DvbStrlen(pcSrc);

	if(ulDstStrLen == 0)
	{
		HxSTD_MemCopy(pcDst, pcSrc, ulSrcStrLen);
		pcDst[ulSrcStrLen] = '\0';
		return pcDst;
	}

	pcDstIdx += ulDstStrLen;


	switch( *pcSrc )
	{
	case 0x01:
	case 0x02:
	case 0x03:
	case 0x04:
	case 0x05:
		if(ulSrcStrLen < 2) { return pcDst; }
		pcSrcIdx++;
		ulSrcStrLen--;
		break;

	case 0x11:
		if(ulSrcStrLen < 2) { return pcDst; }
		pcSrcIdx++;
		ulSrcStrLen--;
		break;


	case 0x10:
		if(ulSrcStrLen < 4) { return pcDst; }
		pcSrcIdx += 3;
		ulSrcStrLen -= 3;
		break;

	case 0x15:
		// Src String이 UTF-8 일 경우 :
		{
			HINT32		nBufferLen = 0, nUsedByte = 0, nUtf8SrcLen = 0;
			HUINT8		*szUtf8Src;
			HERROR		hErr;

			nBufferLen = (HINT32)(ulDstStrLen * 2);
			szUtf8Src = (HUINT8 *)OxMW_Malloc (nBufferLen);
			if (szUtf8Src == NULL)
			{
				return NULL;
			}

			hErr = local_mwc_util_Convert300468ToUtf8 (pcDst, ulDstStrLen, nBufferLen, &nUsedByte, szUtf8Src, &nUtf8SrcLen);
			if (hErr != ERR_OK)
			{
				OxMW_Free (szUtf8Src);
				return NULL;
			}

			pcDst[0] = 0x15;		// UTF-8
			HxSTD_MemCopy (pcDst + 1, szUtf8Src, nUtf8SrcLen);
			HxSTD_MemCopy (pcDst + 1 + nUtf8SrcLen, pcSrc + 1, ulSrcStrLen - 1);
			pcDst[nUtf8SrcLen + ulSrcStrLen] = '\0';

			OxMW_Free (szUtf8Src);

			return pcDst;
		}
	}

	HxSTD_MemCopy(pcDstIdx, pcSrcIdx, ulSrcStrLen);
	pcDstIdx[ulSrcStrLen] = '\0';


	return ( pcDst );
}


HUINT8		*MWC_UTIL_DvbStrncat(HUINT8 *pcDst, const HUINT8 *pcSrc, HUINT32 unLen)
{
	const HUINT8		*pcSrcIdx = pcSrc;
	HUINT8		*pcDstIdx = pcDst;
	HUINT32		ulDstStrLen, ulSrcStrLen;
	HUINT32		ulStrLen, ulCopyLen;


	if(unLen == 0) { return pcDst; }


	ulDstStrLen = MWC_UTIL_DvbStrlen(pcDst);
	ulSrcStrLen = MWC_UTIL_DvbStrlen(pcSrc);

	if(ulDstStrLen == 0)
	{
		ulStrLen = ulSrcStrLen + 1;
		ulCopyLen = (ulStrLen < unLen) ? ulStrLen : unLen;
		if(ulCopyLen > 0) { HxSTD_MemCopy(pcDst, pcSrc, ulCopyLen); }
		return pcDst;
	}

	pcDstIdx += ulDstStrLen;

	switch( *pcSrc )
	{
	case 0x01:
	case 0x02:
	case 0x03:
	case 0x04:
	case 0x05:
	case 0x11:
		if(ulSrcStrLen < 2) { return pcDst; }
		pcSrcIdx++;
		ulSrcStrLen--;
		break;

	case 0x10:
		if(ulSrcStrLen < 4) { return pcDst; }
		pcSrcIdx += 3;
		ulSrcStrLen -= 3;
		break;

	case 0x15:
		// Src String이 UTF-8 일 경우 :
		{
			HINT32 	 nBufferLen= 0, nUsedByte = 0, nUtf8DstLen= 0;
			HUINT8		*szUtf8Dst;
			HERROR		 hErr;

			nBufferLen = (HUINT16)(ulDstStrLen * 2);
			szUtf8Dst = (HUINT8 *)OxMW_Malloc (nBufferLen);
			if (szUtf8Dst == NULL)
			{
				return NULL;
			}

			hErr = local_mwc_util_Convert300468ToUtf8 (pcDst, ulDstStrLen, nBufferLen, &nUsedByte, szUtf8Dst, &nUtf8DstLen);
			if (hErr != ERR_OK)
			{
				OxMW_Free (szUtf8Dst);
				return NULL;
			}

			pcDstIdx = pcDst;

			if (unLen > 0)
			{
				pcDst[0] = 0x15;		// UTF-8

				pcDstIdx ++;
				unLen --;

				ulCopyLen = (nUtf8DstLen < unLen) ? nUtf8DstLen : unLen;
				HxSTD_MemCopy (pcDstIdx, szUtf8Dst, ulCopyLen);
				unLen -= ulCopyLen;
				pcDstIdx += ulCopyLen;

				if (unLen > 0)
				{
					ulCopyLen = ((ulSrcStrLen - 1) < unLen) ? (ulSrcStrLen - 1) : unLen;
					HxSTD_MemCopy (pcDstIdx, pcSrc + 1, ulCopyLen);
					pcDstIdx += ulCopyLen;
				}
			}

			*pcDstIdx = '\0';

			OxMW_Free (szUtf8Dst);

			return pcDst;
		}
	}


	ulStrLen = ulSrcStrLen + 1;
	ulCopyLen = (ulStrLen < unLen) ? ulStrLen : unLen;
	if(ulCopyLen > 0) { HxSTD_MemCopy(pcDstIdx, pcSrcIdx, ulCopyLen); }


	return ( pcDst );
}


HUINT8		*MWC_UTIL_DvbStrcatLang(HUINT8 *pcDst, const HUINT8 *pcSrc)
{
	const HUINT8		*pcSrcIdx = pcSrc;
	HUINT8		*pcDstIdx = pcDst;
	HUINT32		ulDstStrLen, ulSrcStrLen;

	ulDstStrLen = MWC_UTIL_DvbStrlen(pcDst);
	ulSrcStrLen = MWC_UTIL_DvbStrlen(pcSrc);

	if(ulDstStrLen == 0)
	{
		HxSTD_MemCopy(pcDst, pcSrc, ulSrcStrLen);
		pcDst[ulSrcStrLen] = '\0';
		return pcDst;
	}

	pcDstIdx += ulDstStrLen;

	HxSTD_MemCopy(pcDstIdx, pcSrcIdx, ulSrcStrLen);
	pcDstIdx[ulSrcStrLen] = '\0';


	return ( pcDst );
}

STATIC HINT32 local_mwc_util_dvbstrcmp(const HUINT8 *pcStr1, const HUINT8 *pcStr2, HINT32 *pnAddPoint, HINT32 *pnCharSize)
{
	HUINT32			ulStr1Cnt = 0;
	HUINT32			ulStr2Cnt = 0;
	eHxCP_CodePage_e eStr1Encoding = eHCodepage_MAX;
	eHxCP_CodePage_e eStr2Encoding = eHCodepage_MAX;

	if (pcStr1 == NULL || pcStr2 == NULL)
	{
		return	(HINT32)-1;
	}

	local_mwc_util_GetEncodingWithLenByLocaleCode(pcStr1, &eStr1Encoding, &ulStr1Cnt, eHCodepage_MAX);
	local_mwc_util_GetEncodingWithLenByLocaleCode(pcStr2, &eStr2Encoding, &ulStr2Cnt, eHCodepage_MAX);

	if (eStr1Encoding != eStr2Encoding )
	{
		return ( (HINT32)(eStr1Encoding - eStr2Encoding) );
	}

	if(pnAddPoint != NULL)
	{
		*pnAddPoint = ulStr1Cnt;
	}

	if(pnCharSize != NULL)
	{
		switch(eStr1Encoding)
		{
			/* 2byte 문자 : NULL(0x0000) 으로 end */
		case eHxCP_UNICODE_UCS2:
		case eHxCP_UNICODE_DVB_UCS2:
			*pnCharSize = 2;
			break;

		default:
			*pnCharSize = 1;
		}
	}

	return 0;
}

HINT32		MWC_UTIL_DvbStrcmp(const HUINT8 *pcStr1, const HUINT8 *pcStr2)
{
	HINT32			nAddPoint = 0;
	HINT32			nRet = 0;
	HUINT32			ulCharSize = 0;

	nRet = local_mwc_util_dvbstrcmp(pcStr1, pcStr2, &nAddPoint, &ulCharSize);
	if(nRet != 0)
	{
		return nRet;
	}
	pcStr1 += nAddPoint;
	pcStr2 += nAddPoint;

	switch(ulCharSize)
	{
	case 4:
		break;

	case 2:
		while( ((*pcStr1 | *(pcStr1+1)) & (*pcStr2 | *(pcStr2+1)))
			&& ((*pcStr1 == *pcStr2) && (*(pcStr1+1) == *(pcStr2+1))) )
		{
			pcStr1 += 2;
			pcStr2 += 2;
		}

		return ( (HINT32)( ((*pcStr1 - *pcStr2) << 8) | (*(pcStr1 +1) - *(pcStr2+1)) ));

	default:
		while( (*pcStr1 == *pcStr2) && (*pcStr1 & *pcStr2) )
		{
			pcStr1++;
			pcStr2++;
		}

		return ( (HINT32)(*pcStr1 - *pcStr2) );
	}
	return -1;
}


HINT32		MWC_UTIL_DvbStrncmp(const HUINT8 *pcStr1, const HUINT8 *pcStr2, HUINT32 unLen)
{
	HINT32			nAddPoint = 0;
	HINT32			n = (HINT32)(unLen & 0x7FFFFFFF);
	HUINT32			ulCharSize = 0;
	HINT32			nRet = 0;


	nRet = local_mwc_util_dvbstrcmp(pcStr1, pcStr2, &nAddPoint, &ulCharSize);
	if(nRet != 0)
	{
		return nRet;
	}
	pcStr1 += nAddPoint;
	pcStr2 += nAddPoint;

	switch(ulCharSize)
	{
	case 4:
		break;

	case 2:
		while( ((*pcStr1 | *(pcStr1+1)) & (*pcStr2 | *(pcStr2+1)))
			&& ((*pcStr1 == *pcStr2) && (*(pcStr1+1) == *(pcStr2+1))) )
		{
			if ( n == 1 ) { return ( (HINT32)((*pcStr1 - *pcStr2) << 8)); }
			n -= 2;
			if ( n <= 0 ) { break; }

			pcStr1+=2;
			pcStr2+=2;
		}
		return ( (HINT32)( ((*pcStr1 - *pcStr2) << 8) | (*(pcStr1 +1) - *(pcStr2+1)) ));

	default:
		while( (*pcStr1 == *pcStr2) && (*pcStr1 & *pcStr2) )
		{
			n--;
			if( n <= 0) { break; }
			pcStr1++;
			pcStr2++;
		}

		return ( (HINT32)(*pcStr1 - *pcStr2) );
	}

	return nRet;	// fix "control reaches end of non-void function compile warning".. return 0 in case of ulCharSize == 4
}


HUINT32		MWC_UTIL_UCS2Strlen(const HUINT16 *pusStr)
{
	const HUINT16	*pusIdx = pusStr;

	/* check parameter */
	if((pusStr == NULL) || (*pusStr == 0x0000))		{ return 0; }

	/* following text */
	for (/*none*/; *pusIdx != 0x0000; ++pusIdx)/* nothing */;

	/* buffer의 크기임 */
	return (pusIdx - pusStr);
}


CH_UCS2		*MWC_UTIL_UCS2Strcat(CH_UCS2 *pcDst, const CH_UCS2 *pcSrc)
{
	const CH_UCS2		*pcSrcIdx = pcSrc;
	CH_UCS2		*pcDstIdx = pcDst;
	HINT32		nDstStrLen = 0, nSrcStrLen = 0;
	HINT32		nStrLen = 0;

	nDstStrLen = MWC_UTIL_UCS2Strlen(pcDst);
	nSrcStrLen = MWC_UTIL_UCS2Strlen(pcSrc);

	pcDstIdx += nDstStrLen;

	nStrLen = nSrcStrLen + 1;
	if(nStrLen > 0) { HxSTD_MemCopy(pcDstIdx, pcSrcIdx, nStrLen  *  sizeof(CH_UCS2)); }

	return ( pcDst );
}

CH_UCS2		*MWC_UTIL_UCS2Strncat(CH_UCS2 *pcDst, const CH_UCS2 *pcSrc, HUINT32 unLen)
{
	const CH_UCS2		*pcSrcIdx = pcSrc;
	CH_UCS2		*pcDstIdx = pcDst;
	HINT32		nDstStrLen = 0, nSrcStrLen = 0;
	HINT32		nStrLen = 0, nCopyLen = 0;


	if(unLen == 0) { return pcDst; }


	nDstStrLen = MWC_UTIL_UCS2Strlen(pcDst);
	nSrcStrLen = MWC_UTIL_UCS2Strlen(pcSrc);

	pcDstIdx += nDstStrLen;

	nStrLen = nSrcStrLen + 1;
	nCopyLen = (nStrLen < unLen) ? nStrLen : unLen;
	if(nCopyLen > 0) { HxSTD_MemCopy(pcDstIdx, pcSrcIdx, nCopyLen  *  sizeof(CH_UCS2)); }

	return ( pcDst );
}


HUINT32		MWC_UTIL_ChangeIso639toLowercase(HUINT8 *pcStr)
{
	HINT32		i;

	// change first 3 characters into lowercase
	for ( i = 0; i < 3; i++, pcStr++ ) {
		*pcStr = ( ((*pcStr > 'A') && (*pcStr < 'Z')) ? (HUINT8)(*pcStr+'a'-'A') : *pcStr );
	}

	return (ERR_OK);
}

HUINT8*		MWC_UTIL_DvbGetStringOnly(HUINT8* pcStr)
{
	HUINT8	*pcIdx = pcStr;

	if(*pcStr == '\0') { return NULL; }

	switch( *pcStr )
	{
	case 0x01:
	case 0x02:
	case 0x03:
	case 0x04:
	case 0x05:
	case 0x11:
		pcIdx++;
		break;

	case 0x10:
		if( *(pcStr+1) == '\0' && *(pcStr+2) == '\0' ) { return NULL; }
		pcIdx += 3;
		break;
	}

	return pcIdx;
}



#define		CH_TBL_0X10_TABLE	1
#define		CH_TBL_0X20_DEFAULT_TABLE	2

#if defined(____DONT_USE_____)
eHxCP_CodePage_e		MWC_UTIL_GetSystemEncoding(void)
{
	return s_SystemEncoding;
}

HERROR		MWC_UTIL_SetSystemEncoding(eHxCP_CodePage_e eEncoding)
{
	if((eEncoding < eHxCP_ISO_ASCII) || (eEncoding >= eHCodepage_MAX))
	{
		return ERR_FAIL;
	}

	s_SystemEncoding = eEncoding;

	return ERR_OK;
}
#endif

#define	____________TO_UCS4_______________________________________________________________

#if (NOT_USED_FUNCTION)
STATIC HERROR	local_mwc_util_ConvertSingleByteToUCS4(const CH_1BYTE *pSingleByteStr, const HINT32 nSingleByteStrLen, HINT32 *pnUsedByte, CH_UCS4 *pUcs4Str, HINT32 *pnUcs4StrLen)
{
	HUINT16		i=0;
	HUINT16		usConvertedCharacterCount=0;
	ConvEncoding_Err_t	hResErr=ERR_FAIL;

	HxLOG_Assert(pSingleByteStr);
	HxLOG_Assert(pUcs4Str);

	HxLOG_Info("local_mwc_util_ConvertSingleByteToUCS4() nSingleByteStrLen(%d) +\n", nSingleByteStrLen);

	if(nSingleByteStrLen==0)
	{
		/* check number to converte */
		*pnUcs4StrLen=0;
		HxLOG_Info("local_mwc_util_ConvertSingleByteToUCS4() error: demand length is Zero -\n");

		return ERR_OK;
	}

	if(pSingleByteStr==NULL||pUcs4Str==NULL)
	{
		/* pSingleByteStr storage check */
		*pnUcs4StrLen=0;
		HxLOG_Info("local_mwc_util_ConvertSingleByteToUCS4() error: storage is NULL -\n");

		return ERR_FAIL;
	}

	/* ascii encoding 변경 시 dvb 스트링 길이에 맨앞자리 컨트롤 바이트는 white space로 생각 되어져 무시된다.  */
	for(i=0; i<nSingleByteStrLen; i++)
	{
		HxLOG_Print("%02x.", pSingleByteStr[i]);

		if(pSingleByteStr[i]==0x00)
		{
			hResErr=ERR_NULL_CH;

			break;
		}
		else if(pSingleByteStr[i]<0x20)
		{
			/* its white space character */
			hResErr=ERR_WHITE_CH;
		}
		else if(pSingleByteStr[i]>0x7f)
		{
			/* out of ascii code page */
			hResErr=ERR_OUTOF_CODEPAGE;

			continue;
		}
		else	/* ~ 0x7f */
		{
			/* ascii code */
			hResErr=ERR_OK;
		}

		pUcs4Str[usConvertedCharacterCount++]=0x00000000|pSingleByteStr[i];
		HxLOG_Print("u+%08x.", (HUINT32)pUcs4Str[usConvertedCharacterCount]);
	}
	HxLOG_Print("\n");

	*pnUsedByte=i;
	*pnUcs4StrLen=usConvertedCharacterCount;

	HxLOG_Info("local_mwc_util_ConvertSingleByteToUCS4() result error(0x%x) -\n", hResErr);

	return hResErr;
}
#endif



#define	_________________________PUBLICE_FUNCTIONS____________________________________________
/*
우리가 흔히 639Code라고 부르는 것은 정확히 iso 639-2 에 있는 3byte string이다.
639code는 iso 639-1 2byte string, iso 639-2 3byte string 두 종류가 있다.
우리는 639Code로 iso 639-2를 쓰고 있다.

아래에 지정된 language fullname은 일밙적으로 통용되는 해당 언어의 fullname이다
(실제로 iso 639-2에 표기된 english fullname을 기준으로 한다).
특정 spec이나 OP에서 요구하는 string일 경우 특별히 바꿔서 써야 하나,
iso 639-2에 표기된 영문 fullname은 옆 commnet에 꼭 남기도록 한다.
*/

typedef struct {

	/* ISO-639 info */
	HUINT8			s639code[4];
	HUINT32			codeMode;		/**< 0: biblio mode, 1: Termino mode */
	eISO639Idx_t	eISO639Idx;

	/* language info */
	HUINT8*			Lang_Fullname;
	HxLANG_Id_e	eLangID;
} Language_639_t;

/*
639-2 code 알파벳 순으로 무조건 정렬시켜 놓을 것, bsearch()에 사용 해야하므로,
울트라 에디트에서 정렬됨. 굳이 정렬 프로그램 짜지 말고.

아래 내용은 JVC에서 Galician을 Gallegan으로 요구한 사항이 반영되어 있다. wjmoh 2008.10.29
*/

STATIC Language_639_t s_lang639_Info[] = {
	{	"alb",	BIBLIOGRAPHIC_WORD,		eISO639_Albanian_1,	"Albanian",		eLangID_Albanian, },
	{	"ara",	ISO639_DEFAULT_WORD,	eISO639_Arabic,	"Arabic",	eLangID_Arabic, },
	{	"aus",	ISO639_DEFAULT_WORD,	eISO639_Australian,	"Australian",	eLangID_Australian, },
	{	"baq",	ISO639_DEFAULT_WORD,	eISO639_Basque,	"Basque",		eLangID_Basque, },
	{	"bas",	ISO639_DEFAULT_WORD,	eISO639_Basaa,	"Basaa",		eLangID_Basaa, },
	{	"bul",	ISO639_DEFAULT_WORD,	eISO639_Bulgarian,	"Bulgarian",	eLangID_Bulgarian,	},
	{	"cat",	ISO639_DEFAULT_WORD,	eISO639_Catalan,	"Catalan",		eLangID_Catalan,	},
	{	"ces",	TERMINOLOGY_WORD,	eISO639_Czech_2,	"Czech",		eLangID_Czech,	},
	{	"cym",	TERMINOLOGY_WORD,	eISO639_Welsh_2,	"Welsh",	eLangID_Welsh,	},
	{	"cze",	BIBLIOGRAPHIC_WORD,	eISO639_Czech_1,	"Czech",		eLangID_Czech,	},
	{	"dan",	ISO639_DEFAULT_WORD,	eISO639_Danish,	"Danish",		eLangID_Danish,	},
	{	"deu",	TERMINOLOGY_WORD,	eISO639_German_2,	"German",		eLangID_German,	},
	{	"dut",	BIBLIOGRAPHIC_WORD,	eISO639_Dutch_1,	"Dutch",		eLangID_Dutch,	},
	{	"ell",	TERMINOLOGY_WORD,	eISO639_Greek_2,	"Greek",		eLangID_Greek,	},
	{	"eng",	ISO639_DEFAULT_WORD,	eISO639_English,	"English",		eLangID_English,	},
	{	"esl",	HUMAX_WORKAROUND_WORD,	eISO639_Spanish_3,	"Spanish",		eLangID_Spanish,	},
	{	"esp",	HUMAX_WORKAROUND_WORD,	eISO639_Spanish_4,	"Spanish",		eLangID_Spanish,	},
	{	"est",	ISO639_DEFAULT_WORD,	eISO639_Estonian,	"Estonian",		eLangID_Estonian,	},
	{	"fao",	ISO639_DEFAULT_WORD,	eISO639_Faroese,	"Faroese",		eLangID_Faroese,	},
	{	"fas",	TERMINOLOGY_WORD,	eISO639_Persian_2,	"Persian",		eLangID_Persian,	},
	{	"fin",	ISO639_DEFAULT_WORD,	eISO639_Finnish,	"Finnish",		eLangID_Finnish,	},
	{	"fra",	TERMINOLOGY_WORD,	eISO639_French_2,	"French",		eLangID_French,	},
	{	"fre",	BIBLIOGRAPHIC_WORD,	eISO639_French_1,	"French",		eLangID_French,	},
	{	"gai",	HUMAX_WORKAROUND_WORD,	eISO639_Irish_3,	"Irish",		eLangID_Irish,	},
	{	"ger",	BIBLIOGRAPHIC_WORD,	eISO639_German_1,	"German",		eLangID_German,	},
	{	"gla",	ISO639_DEFAULT_WORD,	eISO639_Gaelic,	"Scottish Gaelic",		eLangID_Gaelic,	},	// 범용 fullname -> {	"gla",	ISO639_DEFAULT_WORD,	eISO639_Gaelic,	"Gaelic",		eLangID_Gaelic,	},
	{	"gle",	ISO639_DEFAULT_WORD,	eISO639_Irish,	"Irish",		eLangID_Irish,	},
	{	"glg",	ISO639_DEFAULT_WORD,	eISO639_Galician,	"Galician",		eLangID_Galician,	},	// 범용 fullname -> {	"glg",	ISO639_DEFAULT_WORD,	eISO639_Galician,	"Galician",		eISO639_Galician,	},
	{	"gre",	BIBLIOGRAPHIC_WORD,	eISO639_Greek_1,	"Greek",		eLangID_Greek,	},
	{	"heb",	ISO639_DEFAULT_WORD,	eISO639_Hebrew,	"Hebrew",		eLangID_Hebrew,	},
	{	"hin",	ISO639_DEFAULT_WORD,	eISO639_Hindi,	"Hindi",		eLangID_Hindi,	},
	{	"hrv",	ISO639_DEFAULT_WORD,	eISO639_Croatian,	"Croatian",		eLangID_Croatian,	},
	{	"hun",	ISO639_DEFAULT_WORD,	eISO639_Hungarian,	"Hungarian",	eLangID_Hungarian,	},
	{	"ice",	BIBLIOGRAPHIC_WORD,	eISO639_Icelandic_1,	"Icelandic",		eLangID_Icelandic,	},
	{	"iri",	HUMAX_WORKAROUND_WORD,	eISO639_Irish_4,	"Irish",		eLangID_Irish,	},
	{	"isl",	TERMINOLOGY_WORD,	eISO639_Icelandic_2,	"Icelandic",		eLangID_Icelandic,	},
	{	"ita",	ISO639_DEFAULT_WORD,	eISO639_Italian,	"Italian",		eLangID_Italian,	},
	{	"jpn",	ISO639_DEFAULT_WORD,	eISO639_Japanese,	"Japanese",		eLangID_Japanese,	},
	{	"kor",	ISO639_DEFAULT_WORD,	eISO639_Korean,	"Korean",		eLangID_Korean,	},
	{	"lav",	ISO639_DEFAULT_WORD,	eISO639_Latvian,	"Latvian",		eLangID_Latvian,	},
	{	"lit",	ISO639_DEFAULT_WORD,	eISO639_Lithuanian,	"Lithuanian",		eLangID_Lithuanian,	},
	{	"nar",	EN300468_SPECIFIC_WORD ,eISO639_300468,	"Audio Description",	eLangID_Narration,	},
	{	"nla",	HUMAX_WORKAROUND_WORD,	eISO639_Dutch_3,	"Dutch",		eLangID_Dutch,	},
	{	"nld",	TERMINOLOGY_WORD,	eISO639_Dutch_2,	"Dutch",		eLangID_Dutch,	},
	{	"nor",	ISO639_DEFAULT_WORD,	eISO639_Norwegian,	"Norwegian",	eLangID_Norwegian,	},
	{	"per",	BIBLIOGRAPHIC_WORD,	BIBLIOGRAPHIC_WORD,	"Persian",		eLangID_Persian,	},
	{	"pol",	ISO639_DEFAULT_WORD,	eISO639_Polish,	"Polish",		eLangID_Polish,	},
	{	"por",	ISO639_DEFAULT_WORD,	eISO639_Portuguese,	"Portuguese",	eLangID_Portuguese,	},
#if defined(CONFIG_OP_FREESAT)
	{	"qaa",	EN300468_SPECIFIC_WORD,	eISO639_300468,	"Original Language",	eLangID_OriginalAudio,	},
#else
	{	"qaa",	EN300468_SPECIFIC_WORD,	eISO639_300468,	"original audio",	eLangID_OriginalAudio,	},
#endif
	{	"ron",	TERMINOLOGY_WORD,	eISO639_Romanian_2,	"Romanian",		eLangID_Romanian,	},
	{	"rum",	BIBLIOGRAPHIC_WORD,	eISO639_Romanian_1,	"Romanian",		eLangID_Romanian,	},
	{	"rus",	ISO639_DEFAULT_WORD,	eISO639_Russian,	"Russian",		eLangID_Russian,	},
	{	"scc",	TERMINOLOGY_WORD,	eISO639_Serbian_2,	"Serbian",		eLangID_Serbian, },
	{	"scr",	ISO639_DEFAULT_WORD,	eISO639_Croatian,	"Croatian",		eLangID_Croatian,	},
#if defined(CONFIG_OP_FREESAT)
	{	"sga",	ISO639_DEFAULT_WORD,	eISO639_Irish,	"Irish",		eLangID_Irish,	},
#endif
	{	"slk",	TERMINOLOGY_WORD,	eISO639_Slovak_2,	"Slovak",		eLangID_Slovak,	},
	{	"slo",	BIBLIOGRAPHIC_WORD,	eISO639_Slovak_1,	"Slovak",		eLangID_Slovak,	},
	{	"slv",	ISO639_DEFAULT_WORD,	eISO639_Slovenian,	"Slovenian",	eLangID_Slovenian,	},
	{	"spa",	ISO639_DEFAULT_WORD,	eISO639_Spanish,	"Spanish",		eLangID_Spanish,	},
	{	"sqi",	TERMINOLOGY_WORD,	eISO639_Albanian_2,	"Albanian",		eLangID_Albanian, },
	{	"srp",	BIBLIOGRAPHIC_WORD,	eISO639_Serbian_1,	"Serbian",		eLangID_Serbian, },
	{	"sve",	HUMAX_WORKAROUND_WORD,	eISO639_Swedish_3,	"Swedish",		eLangID_Swedish,	},
	{	"swe",	ISO639_DEFAULT_WORD,	eISO639_Swedish,	"Swedish",		eLangID_Swedish,	},
	{	"tha",	ISO639_DEFAULT_WORD,	eISO639_Thai,	"Thai", eLangID_Thai,	},
	{	"tur",	ISO639_DEFAULT_WORD,	eISO639_Turkish,	"Turkish",		eLangID_Turkish,	},
	{	"ukr",	ISO639_DEFAULT_WORD,	eISO639_Ukrainian,	"Ukrainian",	eLangID_Ukrainian,	},
	{	"und",	UNDEFINED_WORD,	eISO639_Undefined,	"Undefine",	eLangID_Undefined,	},
	{	"vie",	ISO639_DEFAULT_WORD,	eISO639_Vietnamese,	"Vietnamese",	eLangID_Vietnamese,	},
	{	"wel",	BIBLIOGRAPHIC_WORD,	eISO639_Welsh_1,	"Welsh",	eLangID_Welsh,	},
	//		{	"NON",	0xff,		"Humax_not_supported",	eISO639_Humax_not_supported,	},
};

STATIC	HINT32	local_mwc_util_Compare639(const void *str639_key, const void *str639_item)
{
	//	fprintf(stderr, "compare 1(%s) with 2(%s)\n", (str639_key), (((Language_639_t *)str639_item)->s639code));
	return VK_strncmp((HINT8 *)str639_key, (HINT8 *)(((Language_639_t *)str639_item)->s639code), 3);
}

#if 0
HERROR		MWC_UTIL_GetISO639IdxBy639Code(HUINT8 *p639Code, eISO639Idx_t *pLangIdx)
{
	HUINT8	*pStrSearchResult = NULL;
	HUINT8	temp639Code[32] = {0, };
	HINT32	nNumOfLang639 = sizeof(s_lang639_Info)/sizeof(Language_639_t);
	HINT32	i = 0, nLen = 0;
	HERROR	resError = ERR_FAIL;

	if((p639Code == NULL) || (pLangIdx == NULL) || (p639Code[0] == '\0') || (p639Code[1] == '\0') || (p639Code[2] == '\0'))
	{
		return ERR_FAIL;
	}

	nLen = MWC_UTIL_DvbStrlen(p639Code);
	if(nLen > 32)
	{
		nLen = 32;
	}

	for(i = 0; i < nLen; i++)
	{
		temp639Code[i] = local_mwc_util_MakeLowerCase(p639Code[i]);
	}

	/* binary search */
	pStrSearchResult = bsearch((const void *)temp639Code, (const void *)&s_lang639_Info, nNumOfLang639,
		sizeof(Language_639_t), local_mwc_util_Compare639);

	if(pStrSearchResult != NULL)
	{
		*pLangIdx = (((Language_639_t *)pStrSearchResult)->eISO639Idx);
		resError = ERR_OK;
	}
	else
	{
		*pLangIdx = eISO639_Humax_not_supported;
		resError = ERR_FAIL;
	}

	return resError;
}

HERROR		MWC_UTIL_Get639CodeByISO639Idx(const eISO639Idx_t eLangIdx, HUINT8 *pStr)
{
	HINT32	i = 0;
	HINT32	nNumOfLang639 = sizeof(s_lang639_Info)/sizeof(Language_639_t);

	if(pStr == NULL)
	{
		HxLOG_Critical("\n\n");

		return ERR_FAIL;
	}

	for(i = 0; i < nNumOfLang639; i++)
	{
		if(s_lang639_Info[i].eISO639Idx == eLangIdx)
		{
			MWC_UTIL_DvbStrcpy(pStr, s_lang639_Info[i].s639code);

			return ERR_OK;
		}
	}

	return ERR_FAIL;
}
#endif

HERROR		MWC_UTIL_Get639CodeByLangIdx(const HxLANG_Id_e eLangIdx, HINT32 *plMatchCount, HUINT8 *pStr1, HUINT8 *pStr2, HUINT8 *pStr3)
{
	HINT32	i = 0;
	HINT32	nNumOfLang639 = sizeof(s_lang639_Info)/sizeof(Language_639_t);
	HINT32	lMatchCount = 0;

	if(pStr1 == NULL || plMatchCount == NULL)
	{
		HxLOG_Critical("\n\n");

		return ERR_FAIL;
	}

	for(i = 0; i < nNumOfLang639; i++)
	{
		if(s_lang639_Info[i].eLangID == eLangIdx)
		{
			if(lMatchCount == 0)
			{
				MWC_UTIL_DvbStrcpy(pStr1, s_lang639_Info[i].s639code);
			}
			else if(lMatchCount == 1)
			{
				if (NULL != pStr2)
					MWC_UTIL_DvbStrcpy(pStr2, s_lang639_Info[i].s639code);
				else
					goto OUT;
			}
			else if(lMatchCount == 2)
			{
				if (NULL != pStr3)
					MWC_UTIL_DvbStrcpy(pStr3, s_lang639_Info[i].s639code);
				else
					goto OUT;
			}

			lMatchCount++;
			if(lMatchCount >= 3)
			{
				*plMatchCount = lMatchCount;
				return ERR_OK;
			}
		}
	}

OUT:
	*plMatchCount = lMatchCount;
	if(lMatchCount == 0)
	{
		return ERR_FAIL;
	}

	return ERR_OK;

}

#if 0
HERROR		MWC_UTIL_GetLangFullNameByISO639Idx(const eISO639Idx_t eISO639Idx, HUINT8 *pFullnameStr)
{
	HINT32	i = 0;
	HINT32	nNumOfLang639 = sizeof(s_lang639_Info)/sizeof(Language_639_t);

	if(pFullnameStr == NULL)
	{
		HxLOG_Critical("\n\n");

		return ERR_FAIL;
	}

	for(i = 0; i < nNumOfLang639; i++)
	{
		if(s_lang639_Info[i].eISO639Idx == eISO639Idx)
		{
			MWC_UTIL_DvbStrcpy(pFullnameStr, s_lang639_Info[i].Lang_Fullname);

			return ERR_OK;
		}
	}

	return ERR_FAIL;
}

HERROR		MWC_UTIL_GetLangFullNameByLangIdx(const HxLANG_Id_e eLangIdx, HUINT8 *pFullnameStr)
{
	HINT32	i = 0;
	HINT32	nNumOfLang639 = sizeof(s_lang639_Info)/sizeof(Language_639_t);

	if(pFullnameStr == NULL)
	{
		HxLOG_Critical("\n\n");

		return ERR_FAIL;
	}

	for(i = 0; i < nNumOfLang639; i++)
	{
		if(s_lang639_Info[i].eLangID == eLangIdx)
		{
			MWC_UTIL_DvbStrcpy(pFullnameStr, s_lang639_Info[i].Lang_Fullname);

			return ERR_OK;
		}
	}

	return ERR_FAIL;
}

HERROR		MWC_UTIL_GetLangFullNameBy639Code(HUINT8 *p639Code, HUINT8 *pFullnameStr)
{
	HUINT8	*pStrSearchResult = NULL;
	HUINT8	temp639Code[32] = {0, };
	HINT32	nNumOfLang639 = sizeof(s_lang639_Info)/sizeof(Language_639_t);
	HINT32	i = 0, nLen = 0;
	HERROR	resError = ERR_FAIL;

	if((p639Code == NULL) || (pFullnameStr == NULL) || (p639Code[0] == '\0') || (p639Code[1] == '\0') || (p639Code[2] == '\0'))
	{
		return ERR_FAIL;
	}

	nLen = MWC_UTIL_DvbStrlen(p639Code);
	if(nLen > 32)
	{
		nLen = 32;
	}

	for(i = 0; i < nLen; i++)
	{
		temp639Code[i] = local_mwc_util_MakeLowerCase(p639Code[i]);
	}

	/* binary search */
	pStrSearchResult = bsearch((const void *)temp639Code, (const void *)&s_lang639_Info, nNumOfLang639,
		sizeof(Language_639_t), local_mwc_util_Compare639);

	if(pStrSearchResult != NULL)
	{
		MWC_UTIL_DvbStrcpy(pFullnameStr, (((Language_639_t *)pStrSearchResult)->Lang_Fullname));
		resError = ERR_OK;
	}
	else
	{
		pFullnameStr[0] = '\0';
		resError = ERR_FAIL;
	}

	return resError;
}

HERROR		MWC_UTIL_GetLangIDBy639Idx(const eISO639Idx_t e639Idx, HxLANG_Id_e *peLangId)
{
	HINT32	i = 0;
	HINT32	nNumOfLang639 = sizeof(s_lang639_Info)/sizeof(Language_639_t);

	if(peLangId == NULL)
	{
		HxLOG_Critical("\n\n");

		return ERR_FAIL;
	}

	for(i = 0; i < nNumOfLang639; i++)
	{
		if(s_lang639_Info[i].eISO639Idx == e639Idx)
		{
			*peLangId = s_lang639_Info[i].eLangID;

			return ERR_OK;
		}
	}

	return ERR_FAIL;
}
#endif

HERROR		MWC_UTIL_GetLangIDBy639Code(HUINT8 *p639Code, HxLANG_Id_e *peLangId)
{
	HUINT8	*pStrSearchResult = NULL;
	HUINT8	temp639Code[32] = {0, };
	HINT32	nNumOfLang639 = sizeof(s_lang639_Info)/sizeof(Language_639_t);
	HINT32	i = 0, nLen = 0;
	HERROR	resError = ERR_FAIL;

	if((p639Code == NULL) || (peLangId == NULL) || (p639Code[0] == '\0') || (p639Code[1] == '\0') || (p639Code[2] == '\0'))
	{
		return ERR_FAIL;
	}
	*peLangId = eLangID_MAX;

	nLen = MWC_UTIL_DvbStrlen(p639Code);
	if(nLen > 32)
	{
		nLen = 32;
	}

	for(i = 0; i < nLen; i++)
	{
		temp639Code[i] = local_mwc_util_MakeLowerCase(p639Code[i]);
	}

	/* binary search */
	pStrSearchResult = bsearch((const void *)temp639Code, (const void *)&s_lang639_Info, nNumOfLang639,
		sizeof(Language_639_t), local_mwc_util_Compare639);

	if(pStrSearchResult != NULL)
	{
		*peLangId = ((Language_639_t *)pStrSearchResult)->eLangID;
		resError = ERR_OK;
	}
	else
	{
		resError = ERR_FAIL;
	}

	return resError;
}

typedef struct {

	/* ISO-639-1 info */
	HUINT8			s639code[4];

	/* language info */
	HUINT8*			Lang_Fullname;
	HxLANG_Id_e	eLangID;
} Language_639_1_t;

STATIC Language_639_1_t s_lang639_1_Info[] = {
	{	"ar",	"Arabic",	eLangID_Arabic, },
	{	"bg",	"Bulgarian",	eLangID_Bulgarian,	},
	{	"ca",	"Catalan",		eLangID_Catalan,	},
	{	"cs",	"Czech",		eLangID_Czech,	},
	{	"cy",	"Welsh",	eLangID_Welsh,	},
	{	"da",	"Danish",		eLangID_Danish,	},
	{	"de",	"German",		eLangID_German,	},
	{	"el",	"Greek",		eLangID_Greek,	},
	{	"en",	"English",		eLangID_English,	},
	{	"es",	"Spanish",		eLangID_Spanish,	},
	{	"et",	"Estonian",		eLangID_Estonian,	},
	{	"eu",	"Basque",		eLangID_Basque, },
	{	"fa",	"Persian",		eLangID_Persian,	},
	{	"fi",	"Finnish",		eLangID_Finnish,	},
	{	"fo",	"Faroese",		eLangID_Faroese,	},
	{	"fr",	"French",		eLangID_French,	},
	{	"ga",	"Irish",		eLangID_Irish,	},
	{	"gd",	"Gaelic",		eLangID_Gaelic,	},
	{	"gl",	"Galician",		eLangID_Galician,	},
	{	"he",	"Hebrew",		eLangID_Hebrew,	},
	{	"hi",	"Hindi",		eLangID_Hindi,	},
	{	"hr",	"Croatian",		eLangID_Croatian,	},
	{	"hu",	"Hungarian",	eLangID_Hungarian,	},
	{	"is",	"Icelandic",		eLangID_Icelandic,	},
	{	"it",	"Italian",		eLangID_Italian,	},
	{	"ja",	"Japanese",		eLangID_Japanese,	},
	{	"ko",	"Korean",		eLangID_Korean,	},
	{	"lt",	"Lithuanian",		eLangID_Lithuanian,	},
	{	"lv",	"Latvian",		eLangID_Latvian,	},
	{	"nl",	"Dutch",		eLangID_Dutch,	},
	{	"no",	"Norwegian",	eLangID_Norwegian,	},
	{	"pl",	"Polish",		eLangID_Polish,	},
	{	"pt",	"Portuguese",	eLangID_Portuguese,	},
	{	"ro",	"Romanian",		eLangID_Romanian,	},
	{	"ru",	"Russian",		eLangID_Russian,	},
	{	"sk",	"Slovak",		eLangID_Slovak,	},
	{	"sl",	"Slovenian",	eLangID_Slovenian,	},
	{	"sq",	"Albanian",		eLangID_Albanian, },
	{	"sr",	"Serbian",		eLangID_Serbian, },
	{	"sv",	"Swedish",		eLangID_Swedish,	},
	{	"th",	"Thai",			eLangID_Thai,	},
	{	"tr",	"Turkish",		eLangID_Turkish,	},
	{	"uk",	"Ukrainian",	eLangID_Ukrainian,	},
	{	"vi",	"Vietnamese",	eLangID_Vietnamese,	},
};


STATIC	HINT32	local_mwc_util_Compare639_1(const void *str639_key, const void *str639_item)
{
	//	fprintf(stderr, "compare 1(%s) with 2(%s)\n", (str639_key), (((Language_639_t *)str639_item)->s639code));
	return VK_strncmp((HINT8 *)str639_key, (HINT8 *)(((Language_639_1_t *)str639_item)->s639code), 2);
}

HERROR		MWC_UTIL_GetLangIDBy639_1Code(HUINT8 *p639_1Code, HxLANG_Id_e *peLangId)
{
	HUINT8	*pStrSearchResult = NULL;
	HUINT8	temp639Code[32] = {0, };
	HINT32	nNumOfLang639 = sizeof(s_lang639_1_Info)/sizeof(Language_639_1_t);
	HINT32	i = 0, nLen = 0;
	HERROR	resError = ERR_FAIL;

	if((p639_1Code == NULL) || (peLangId == NULL) || (p639_1Code[0] == '\0') || (p639_1Code[1] == '\0'))
	{
		return ERR_FAIL;
	}

	nLen = MWC_UTIL_DvbStrlen(p639_1Code);
	if(nLen > 32)
	{
		nLen = 32;
	}

	for(i = 0; i < nLen; i++)
	{
		temp639Code[i] = local_mwc_util_MakeLowerCase(p639_1Code[i]);
	}

	/* binary search */
	pStrSearchResult = bsearch((const void *)temp639Code, (const void *)&s_lang639_1_Info, nNumOfLang639,
		sizeof(Language_639_1_t), local_mwc_util_Compare639_1);

	if(pStrSearchResult != NULL)
	{
		*peLangId = ((Language_639_1_t *)pStrSearchResult)->eLangID;
		resError = ERR_OK;
	}
	else
	{
		*peLangId = eLangID_MAX;
		resError = ERR_FAIL;
	}

	return resError;
}

#if 0
HERROR		MWC_UTIL_GetLangFullNameBy639_1Code(HUINT8 *p639_1Code, HUINT8 *pFullnameStr)
{
	HUINT8	*pStrSearchResult = NULL;
	HUINT8	temp639Code[32] = {0, };
	HINT32	nNumOfLang639 = sizeof(s_lang639_1_Info)/sizeof(Language_639_1_t);
	HINT32	i = 0, nLen = 0;
	HERROR	resError = ERR_FAIL;

	if((p639_1Code == NULL) || (pFullnameStr == NULL) || (p639_1Code[0] == '\0') || (p639_1Code[1] == '\0'))
	{
		return ERR_FAIL;
	}

	nLen = MWC_UTIL_DvbStrlen(p639_1Code);
	if(nLen > 32)
	{
		nLen = 32;
	}

	for(i = 0; i < nLen; i++)
	{
		temp639Code[i] = local_mwc_util_MakeLowerCase(p639_1Code[i]);
	}

	/* binary search */
	pStrSearchResult = bsearch((const void *)temp639Code, (const void *)&s_lang639_1_Info, nNumOfLang639,
		sizeof(Language_639_1_t), local_mwc_util_Compare639);

	if(pStrSearchResult != NULL)
	{
		MWC_UTIL_DvbStrcpy(pFullnameStr, (((Language_639_1_t *)pStrSearchResult)->Lang_Fullname));
		resError = ERR_OK;
	}
	else
	{
		pFullnameStr[0] = '\0';
		resError = ERR_FAIL;
	}

	return resError;
}
#endif

/* code length : 3 bytes */
#if 0
// TODO: stb_params.h에 국가가 추가되는 경우, 이 함수에도 추가해줘야함
HERROR MWC_UTIL_GetIso3166CodeByCountryIndex(Country_Idx_t eCountryIndex, HUINT8 *szIso3166Code)
{
	switch(eCountryIndex)
	{
	case eCountry_AUSTRALIA:	MWC_UTIL_DvbStrncpy(szIso3166Code, ISO3166CODE_AUS, 3); szIso3166Code[3] = '\0'; break;
	case eCountry_AUSTRIA:		MWC_UTIL_DvbStrncpy(szIso3166Code, ISO3166CODE_AUT, 3); szIso3166Code[3] = '\0'; break;
	case eCountry_CAMEROON:		MWC_UTIL_DvbStrncpy(szIso3166Code, ISO3166CODE_CMR, 3); szIso3166Code[3] = '\0'; break;
	case eCountry_CROATIA:		MWC_UTIL_DvbStrncpy(szIso3166Code, ISO3166CODE_HRV, 3); szIso3166Code[3] = '\0'; break;
	case eCountry_CZECH:		MWC_UTIL_DvbStrncpy(szIso3166Code, ISO3166CODE_CZE, 3); szIso3166Code[3] = '\0'; break;
	case eCountry_DENMARK:		MWC_UTIL_DvbStrncpy(szIso3166Code, ISO3166CODE_DNK, 3); szIso3166Code[3] = '\0'; break;
	case eCountry_ENGLAND:		MWC_UTIL_DvbStrncpy(szIso3166Code, ISO3166CODE_GBR, 3); szIso3166Code[3] = '\0'; break;
	case eCountry_ESTONIA:		MWC_UTIL_DvbStrncpy(szIso3166Code, ISO3166CODE_EST, 3); szIso3166Code[3] = '\0'; break;
	case eCountry_FINLAND:		MWC_UTIL_DvbStrncpy(szIso3166Code, ISO3166CODE_FIN, 3); szIso3166Code[3] = '\0'; break;
	case eCountry_FRANCE:		MWC_UTIL_DvbStrncpy(szIso3166Code, ISO3166CODE_FRA, 3); szIso3166Code[3] = '\0'; break;
	case eCountry_GERMANY:		MWC_UTIL_DvbStrncpy(szIso3166Code, ISO3166CODE_DEU, 3); szIso3166Code[3] = '\0'; break;
	case eCountry_GREECE:		MWC_UTIL_DvbStrncpy(szIso3166Code, ISO3166CODE_GRC, 3); szIso3166Code[3] = '\0'; break;
	case eCountry_HUNGARY:		MWC_UTIL_DvbStrncpy(szIso3166Code, ISO3166CODE_HUN, 3); szIso3166Code[3] = '\0'; break;
	case eCountry_IRELAND:		MWC_UTIL_DvbStrncpy(szIso3166Code, ISO3166CODE_IRL, 3); szIso3166Code[3] = '\0'; break;
	case eCountry_ITALY:		MWC_UTIL_DvbStrncpy(szIso3166Code, ISO3166CODE_ITA, 3); szIso3166Code[3] = '\0'; break;
	case eCountry_JAPAN:		MWC_UTIL_DvbStrncpy(szIso3166Code, ISO3166CODE_JPN, 3); szIso3166Code[3] = '\0'; break;
	case eCountry_KOREA:		MWC_UTIL_DvbStrncpy(szIso3166Code, ISO3166CODE_KOR, 3); szIso3166Code[3] = '\0'; break;
	case eCountry_LATVIA:		MWC_UTIL_DvbStrncpy(szIso3166Code, ISO3166CODE_LVA, 3); szIso3166Code[3] = '\0'; break;
	case eCountry_LITHUANIA:	MWC_UTIL_DvbStrncpy(szIso3166Code, ISO3166CODE_LTU, 3); szIso3166Code[3] = '\0'; break;
	case eCountry_NETHERLANDS:	MWC_UTIL_DvbStrncpy(szIso3166Code, ISO3166CODE_NLD, 3); szIso3166Code[3] = '\0'; break;
	case eCountry_NORWAY:		MWC_UTIL_DvbStrncpy(szIso3166Code, ISO3166CODE_NOR, 3); szIso3166Code[3] = '\0'; break;
	case eCountry_POLAND:		MWC_UTIL_DvbStrncpy(szIso3166Code, ISO3166CODE_POL, 3); szIso3166Code[3] = '\0'; break;
	case eCountry_PORTUGAL:		MWC_UTIL_DvbStrncpy(szIso3166Code, ISO3166CODE_PRT, 3); szIso3166Code[3] = '\0'; break;
	case eCountry_RUSSIA:		MWC_UTIL_DvbStrncpy(szIso3166Code, ISO3166CODE_RUS, 3); szIso3166Code[3] = '\0'; break;
	case eCountry_SINGAPORE:	MWC_UTIL_DvbStrncpy(szIso3166Code, ISO3166CODE_SGP, 3); szIso3166Code[3] = '\0'; break;
	case eCountry_SLOVAKIA:		MWC_UTIL_DvbStrncpy(szIso3166Code, ISO3166CODE_SVK, 3); szIso3166Code[3] = '\0'; break;
	case eCountry_SLOVENIA:		MWC_UTIL_DvbStrncpy(szIso3166Code, ISO3166CODE_SVN, 3); szIso3166Code[3] = '\0'; break;
	case eCountry_SPAIN:		MWC_UTIL_DvbStrncpy(szIso3166Code, ISO3166CODE_ESP, 3); szIso3166Code[3] = '\0'; break;
	case eCountry_SWEDEN:		MWC_UTIL_DvbStrncpy(szIso3166Code, ISO3166CODE_SWE, 3); szIso3166Code[3] = '\0'; break;
	case eCountry_SWITZERLAND:	MWC_UTIL_DvbStrncpy(szIso3166Code, ISO3166CODE_CHE, 3); szIso3166Code[3] = '\0'; break;
	case eCountry_TURKEY:		MWC_UTIL_DvbStrncpy(szIso3166Code, ISO3166CODE_TUR, 3); szIso3166Code[3] = '\0'; break;
	case eCountry_UKRAINE:		MWC_UTIL_DvbStrncpy(szIso3166Code, ISO3166CODE_UKR, 3); szIso3166Code[3] = '\0'; break;
	case eCountry_USA:			MWC_UTIL_DvbStrncpy(szIso3166Code, ISO3166CODE_USA, 3); szIso3166Code[3] = '\0'; break;
	case eCountry_VIETNAM:		MWC_UTIL_DvbStrncpy(szIso3166Code, ISO3166CODE_VNM, 3); szIso3166Code[3] = '\0'; break;
	case eCountry_BELGIUM:		MWC_UTIL_DvbStrncpy(szIso3166Code, ISO3166CODE_BEL, 3); szIso3166Code[3] = '\0'; break;
	case eCountry_LUXEMBOURG:	MWC_UTIL_DvbStrncpy(szIso3166Code, ISO3166CODE_LUX, 3); szIso3166Code[3] = '\0'; break;
	case eCountry_ROMANIA:		MWC_UTIL_DvbStrncpy(szIso3166Code, ISO3166CODE_ROU, 3); szIso3166Code[3] = '\0'; break;
	case eCountry_THAILAND: 	MWC_UTIL_DvbStrncpy(szIso3166Code, ISO3166CODE_THA, 3); szIso3166Code[3] = '\0'; break;
	case eCountry_UAE:			MWC_UTIL_DvbStrncpy(szIso3166Code, ISO3166CODE_ARE, 3); szIso3166Code[3] = '\0'; break;
	case eCountry_BAHRAIN:		MWC_UTIL_DvbStrncpy(szIso3166Code, ISO3166CODE_BHR, 3); szIso3166Code[3] = '\0'; break;
	case eCountry_EGYPT:		MWC_UTIL_DvbStrncpy(szIso3166Code, ISO3166CODE_EGY, 3); szIso3166Code[3] = '\0'; break;
	case eCountry_IRAN: 		MWC_UTIL_DvbStrncpy(szIso3166Code, ISO3166CODE_IRN, 3); szIso3166Code[3] = '\0'; break;
	case eCountry_IRAQ:			MWC_UTIL_DvbStrncpy(szIso3166Code, ISO3166CODE_IRQ, 3); szIso3166Code[3] = '\0'; break;
	case eCountry_KUWAIT:		MWC_UTIL_DvbStrncpy(szIso3166Code, ISO3166CODE_KWT, 3); szIso3166Code[3] = '\0'; break;
	case eCountry_OMAN: 		MWC_UTIL_DvbStrncpy(szIso3166Code, ISO3166CODE_OMN, 3); szIso3166Code[3] = '\0'; break;
	case eCountry_QATAR:		MWC_UTIL_DvbStrncpy(szIso3166Code, ISO3166CODE_QAT, 3); szIso3166Code[3] = '\0'; break;
	case eCountry_SAUDIARABIA:	MWC_UTIL_DvbStrncpy(szIso3166Code, ISO3166CODE_SAU, 3); szIso3166Code[3] = '\0'; break;
	case eCountry_AZERBAIJAN:	MWC_UTIL_DvbStrncpy(szIso3166Code, ISO3166CODE_AZE, 3); szIso3166Code[3] = '\0'; break;
	case eCountry_BULGARIA: 	MWC_UTIL_DvbStrncpy(szIso3166Code, ISO3166CODE_BGR, 3); szIso3166Code[3] = '\0'; break;
	case eCountry_TURKMENISTAN: MWC_UTIL_DvbStrncpy(szIso3166Code, ISO3166CODE_TKM, 3); szIso3166Code[3] = '\0'; break;
	case eCountry_UZBEKISTAN:	MWC_UTIL_DvbStrncpy(szIso3166Code, ISO3166CODE_UZB, 3); szIso3166Code[3] = '\0'; break;
	case eCountry_Other_Country:		MWC_UTIL_DvbStrncpy(szIso3166Code, "Oth", 3); szIso3166Code[3] = '\0'; break;
	default:
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR MWC_UTIL_GetCountryIndexByIso3166Code(HUINT8 *szIso3166Code, Country_Idx_t *peCountryIndex)
{
	HUINT8	aucIso3166[4];

	if(szIso3166Code == NULL)
		return ERR_FAIL;
	HxSTD_MemCopy(aucIso3166, szIso3166Code, 3);
	aucIso3166[3] = '\0';
	MWC_UTIL_ConvertStringUpper(aucIso3166);

	if(0 == MWC_UTIL_DvbStrncmp(aucIso3166, ISO3166CODE_AUS, 3))	{ *peCountryIndex = eCountry_AUSTRALIA; 	return ERR_OK; }
	if(0 == MWC_UTIL_DvbStrncmp(aucIso3166, ISO3166CODE_AUT, 3))	{ *peCountryIndex = eCountry_AUSTRIA; 		return ERR_OK; }
	if(0 == MWC_UTIL_DvbStrncmp(aucIso3166, ISO3166CODE_CMR, 3))	{ *peCountryIndex = eCountry_CAMEROON; 		return ERR_OK; }
	if(0 == MWC_UTIL_DvbStrncmp(aucIso3166, ISO3166CODE_HRV, 3))	{ *peCountryIndex = eCountry_CROATIA; 		return ERR_OK; }
	if(0 == MWC_UTIL_DvbStrncmp(aucIso3166, ISO3166CODE_CZE, 3))	{ *peCountryIndex = eCountry_CZECH; 		return ERR_OK; }
	if(0 == MWC_UTIL_DvbStrncmp(aucIso3166, ISO3166CODE_DNK, 3))	{ *peCountryIndex = eCountry_DENMARK; 		return ERR_OK; }
	if(0 == MWC_UTIL_DvbStrncmp(aucIso3166, ISO3166CODE_GBR, 3))	{ *peCountryIndex = eCountry_ENGLAND; 		return ERR_OK; }
	if(0 == MWC_UTIL_DvbStrncmp(aucIso3166, ISO3166CODE_EST, 3))	{ *peCountryIndex = eCountry_ESTONIA; 		return ERR_OK; }
	if(0 == MWC_UTIL_DvbStrncmp(aucIso3166, ISO3166CODE_FIN, 3))	{ *peCountryIndex = eCountry_FINLAND; 		return ERR_OK; }
	if(0 == MWC_UTIL_DvbStrncmp(aucIso3166, ISO3166CODE_FRA, 3))	{ *peCountryIndex = eCountry_FRANCE; 		return ERR_OK; }
	if(0 == MWC_UTIL_DvbStrncmp(aucIso3166, ISO3166CODE_DEU, 3))	{ *peCountryIndex = eCountry_GERMANY; 		return ERR_OK; }
	if(0 == MWC_UTIL_DvbStrncmp(aucIso3166, ISO3166CODE_GRC, 3))	{ *peCountryIndex = eCountry_GREECE; 		return ERR_OK; }
	if(0 == MWC_UTIL_DvbStrncmp(aucIso3166, ISO3166CODE_HUN, 3))	{ *peCountryIndex = eCountry_HUNGARY; 		return ERR_OK; }
	if(0 == MWC_UTIL_DvbStrncmp(aucIso3166, ISO3166CODE_IRL, 3))	{ *peCountryIndex = eCountry_IRELAND; 		return ERR_OK; }
	if(0 == MWC_UTIL_DvbStrncmp(aucIso3166, ISO3166CODE_ITA, 3))	{ *peCountryIndex = eCountry_ITALY; 		return ERR_OK; }
	if(0 == MWC_UTIL_DvbStrncmp(aucIso3166, ISO3166CODE_JPN, 3))	{ *peCountryIndex = eCountry_JAPAN; 		return ERR_OK; }
	if(0 == MWC_UTIL_DvbStrncmp(aucIso3166, ISO3166CODE_KOR, 3))	{ *peCountryIndex = eCountry_KOREA; 		return ERR_OK; }
	if(0 == MWC_UTIL_DvbStrncmp(aucIso3166, ISO3166CODE_LVA, 3))	{ *peCountryIndex = eCountry_LATVIA; 		return ERR_OK; }
	if(0 == MWC_UTIL_DvbStrncmp(aucIso3166, ISO3166CODE_LTU, 3))	{ *peCountryIndex = eCountry_LITHUANIA; 	return ERR_OK; }
	if(0 == MWC_UTIL_DvbStrncmp(aucIso3166, ISO3166CODE_NLD, 3))	{ *peCountryIndex = eCountry_NETHERLANDS; 	return ERR_OK; }
	if(0 == MWC_UTIL_DvbStrncmp(aucIso3166, ISO3166CODE_NOR, 3))	{ *peCountryIndex = eCountry_NORWAY; 		return ERR_OK; }
	if(0 == MWC_UTIL_DvbStrncmp(aucIso3166, ISO3166CODE_POL, 3))	{ *peCountryIndex = eCountry_POLAND; 		return ERR_OK; }
	if(0 == MWC_UTIL_DvbStrncmp(aucIso3166, ISO3166CODE_PRT, 3))	{ *peCountryIndex = eCountry_PORTUGAL; 		return ERR_OK; }
	if(0 == MWC_UTIL_DvbStrncmp(aucIso3166, ISO3166CODE_RUS, 3))	{ *peCountryIndex = eCountry_RUSSIA; 		return ERR_OK; }
	if(0 == MWC_UTIL_DvbStrncmp(aucIso3166, ISO3166CODE_SGP, 3))	{ *peCountryIndex = eCountry_SINGAPORE; 	return ERR_OK; }
	if(0 == MWC_UTIL_DvbStrncmp(aucIso3166, ISO3166CODE_SVK, 3))	{ *peCountryIndex = eCountry_SLOVAKIA; 		return ERR_OK; }
	if(0 == MWC_UTIL_DvbStrncmp(aucIso3166, ISO3166CODE_SVN, 3))	{ *peCountryIndex = eCountry_SLOVENIA; 		return ERR_OK; }
	if(0 == MWC_UTIL_DvbStrncmp(aucIso3166, ISO3166CODE_ESP, 3))	{ *peCountryIndex = eCountry_SPAIN; 		return ERR_OK; }
	if(0 == MWC_UTIL_DvbStrncmp(aucIso3166, ISO3166CODE_SWE, 3))	{ *peCountryIndex = eCountry_SWEDEN; 		return ERR_OK; }
	if(0 == MWC_UTIL_DvbStrncmp(aucIso3166, ISO3166CODE_CHE, 3))	{ *peCountryIndex = eCountry_SWITZERLAND; 	return ERR_OK; }
	if(0 == MWC_UTIL_DvbStrncmp(aucIso3166, ISO3166CODE_TUR, 3))	{ *peCountryIndex = eCountry_TURKEY; 		return ERR_OK; }
	if(0 == MWC_UTIL_DvbStrncmp(aucIso3166, ISO3166CODE_UKR, 3))	{ *peCountryIndex = eCountry_UKRAINE; 		return ERR_OK; }
	if(0 == MWC_UTIL_DvbStrncmp(aucIso3166, ISO3166CODE_USA, 3))	{ *peCountryIndex = eCountry_USA; 			return ERR_OK; }
	if(0 == MWC_UTIL_DvbStrncmp(aucIso3166, ISO3166CODE_VNM, 3))	{ *peCountryIndex = eCountry_VIETNAM; 		return ERR_OK; }
	if(0 == MWC_UTIL_DvbStrncmp(aucIso3166, ISO3166CODE_BEL, 3))	{ *peCountryIndex = eCountry_BELGIUM; 		return ERR_OK; }
	if(0 == MWC_UTIL_DvbStrncmp(aucIso3166, ISO3166CODE_LUX, 3))	{ *peCountryIndex = eCountry_LUXEMBOURG; 	return ERR_OK; }
	if(0 == MWC_UTIL_DvbStrncmp(aucIso3166, ISO3166CODE_ROU, 3))	{ *peCountryIndex = eCountry_ROMANIA; 		return ERR_OK; }
	if(0 == MWC_UTIL_DvbStrncmp(aucIso3166, "Oth", 3))				{ *peCountryIndex = eCountry_Other_Country; return ERR_OK; }

	return ERR_FAIL;
}
#endif

HERROR MWC_UTIL_GetIso3166CodeByCountryId(HxCountry_e eCountryId, HUINT8 *szIso3166Code)
{
	if(szIso3166Code == NULL)
		return ERR_FAIL;

	switch(eCountryId)
	{
	case eCountryID_AUS:	MWC_UTIL_DvbStrncpy(szIso3166Code, ISO3166CODE_AUS, 3); szIso3166Code[3] = '\0'; break;
	case eCountryID_AUT:	MWC_UTIL_DvbStrncpy(szIso3166Code, ISO3166CODE_AUT, 3); szIso3166Code[3] = '\0'; break;
	case eCountryID_CMR:	MWC_UTIL_DvbStrncpy(szIso3166Code, ISO3166CODE_CMR, 3); szIso3166Code[3] = '\0'; break;
	case eCountryID_HRV:	MWC_UTIL_DvbStrncpy(szIso3166Code, ISO3166CODE_HRV, 3); szIso3166Code[3] = '\0'; break;
	case eCountryID_CZE:	MWC_UTIL_DvbStrncpy(szIso3166Code, ISO3166CODE_CZE, 3); szIso3166Code[3] = '\0'; break;
	case eCountryID_DNK:	MWC_UTIL_DvbStrncpy(szIso3166Code, ISO3166CODE_DNK, 3); szIso3166Code[3] = '\0'; break;
	case eCountryID_GBR:	MWC_UTIL_DvbStrncpy(szIso3166Code, ISO3166CODE_GBR, 3); szIso3166Code[3] = '\0'; break;
	case eCountryID_EST:	MWC_UTIL_DvbStrncpy(szIso3166Code, ISO3166CODE_EST, 3); szIso3166Code[3] = '\0'; break;
	case eCountryID_FIN:	MWC_UTIL_DvbStrncpy(szIso3166Code, ISO3166CODE_FIN, 3); szIso3166Code[3] = '\0'; break;
	case eCountryID_FRA:	MWC_UTIL_DvbStrncpy(szIso3166Code, ISO3166CODE_FRA, 3); szIso3166Code[3] = '\0'; break;
	case eCountryID_DEU:	MWC_UTIL_DvbStrncpy(szIso3166Code, ISO3166CODE_DEU, 3); szIso3166Code[3] = '\0'; break;
	case eCountryID_GRC:	MWC_UTIL_DvbStrncpy(szIso3166Code, ISO3166CODE_GRC, 3); szIso3166Code[3] = '\0'; break;
	case eCountryID_HUN:	MWC_UTIL_DvbStrncpy(szIso3166Code, ISO3166CODE_HUN, 3); szIso3166Code[3] = '\0'; break;
	case eCountryID_IRL:	MWC_UTIL_DvbStrncpy(szIso3166Code, ISO3166CODE_IRL, 3); szIso3166Code[3] = '\0'; break;
	case eCountryID_ITA:	MWC_UTIL_DvbStrncpy(szIso3166Code, ISO3166CODE_ITA, 3); szIso3166Code[3] = '\0'; break;
	case eCountryID_JPN:	MWC_UTIL_DvbStrncpy(szIso3166Code, ISO3166CODE_JPN, 3); szIso3166Code[3] = '\0'; break;
	case eCountryID_KOR:	MWC_UTIL_DvbStrncpy(szIso3166Code, ISO3166CODE_KOR, 3); szIso3166Code[3] = '\0'; break;
	case eCountryID_LVA:	MWC_UTIL_DvbStrncpy(szIso3166Code, ISO3166CODE_LVA, 3); szIso3166Code[3] = '\0'; break;
	case eCountryID_LTU:	MWC_UTIL_DvbStrncpy(szIso3166Code, ISO3166CODE_LTU, 3); szIso3166Code[3] = '\0'; break;
	case eCountryID_NLD:	MWC_UTIL_DvbStrncpy(szIso3166Code, ISO3166CODE_NLD, 3); szIso3166Code[3] = '\0'; break;
	case eCountryID_NOR:	MWC_UTIL_DvbStrncpy(szIso3166Code, ISO3166CODE_NOR, 3); szIso3166Code[3] = '\0'; break;
	case eCountryID_POL:	MWC_UTIL_DvbStrncpy(szIso3166Code, ISO3166CODE_POL, 3); szIso3166Code[3] = '\0'; break;
	case eCountryID_PRT:	MWC_UTIL_DvbStrncpy(szIso3166Code, ISO3166CODE_PRT, 3); szIso3166Code[3] = '\0'; break;
	case eCountryID_RUS:	MWC_UTIL_DvbStrncpy(szIso3166Code, ISO3166CODE_RUS, 3); szIso3166Code[3] = '\0'; break;
	case eCountryID_SGP:	MWC_UTIL_DvbStrncpy(szIso3166Code, ISO3166CODE_SGP, 3); szIso3166Code[3] = '\0'; break;
	case eCountryID_SVK:	MWC_UTIL_DvbStrncpy(szIso3166Code, ISO3166CODE_SVK, 3); szIso3166Code[3] = '\0'; break;
	case eCountryID_SVN:	MWC_UTIL_DvbStrncpy(szIso3166Code, ISO3166CODE_SVN, 3); szIso3166Code[3] = '\0'; break;
	case eCountryID_ESP:	MWC_UTIL_DvbStrncpy(szIso3166Code, ISO3166CODE_ESP, 3); szIso3166Code[3] = '\0'; break;
	case eCountryID_SWE:	MWC_UTIL_DvbStrncpy(szIso3166Code, ISO3166CODE_SWE, 3); szIso3166Code[3] = '\0'; break;
	case eCountryID_CHE:	MWC_UTIL_DvbStrncpy(szIso3166Code, ISO3166CODE_CHE, 3); szIso3166Code[3] = '\0'; break;
	case eCountryID_TUR:	MWC_UTIL_DvbStrncpy(szIso3166Code, ISO3166CODE_TUR, 3); szIso3166Code[3] = '\0'; break;
	case eCountryID_UKR:	MWC_UTIL_DvbStrncpy(szIso3166Code, ISO3166CODE_UKR, 3); szIso3166Code[3] = '\0'; break;
	case eCountryID_USA:	MWC_UTIL_DvbStrncpy(szIso3166Code, ISO3166CODE_USA, 3); szIso3166Code[3] = '\0'; break;
	case eCountryID_VNM:	MWC_UTIL_DvbStrncpy(szIso3166Code, ISO3166CODE_VNM, 3); szIso3166Code[3] = '\0'; break;
	case eCountryID_BEL:	MWC_UTIL_DvbStrncpy(szIso3166Code, ISO3166CODE_BEL, 3); szIso3166Code[3] = '\0'; break;
	case eCountryID_LUX:	MWC_UTIL_DvbStrncpy(szIso3166Code, ISO3166CODE_LUX, 3); szIso3166Code[3] = '\0'; break;
	case eCountryID_ROU:	MWC_UTIL_DvbStrncpy(szIso3166Code, ISO3166CODE_ROU, 3); szIso3166Code[3] = '\0'; break;
	case eCountryID_THA: 	MWC_UTIL_DvbStrncpy(szIso3166Code, ISO3166CODE_THA, 3); szIso3166Code[3] = '\0'; break;
	case eCountryID_ARE:	MWC_UTIL_DvbStrncpy(szIso3166Code, ISO3166CODE_ARE, 3); szIso3166Code[3] = '\0'; break;
	case eCountryID_BHR:	MWC_UTIL_DvbStrncpy(szIso3166Code, ISO3166CODE_BHR, 3); szIso3166Code[3] = '\0'; break;
	case eCountryID_EGY:	MWC_UTIL_DvbStrncpy(szIso3166Code, ISO3166CODE_EGY, 3); szIso3166Code[3] = '\0'; break;
	case eCountryID_IRN: 	MWC_UTIL_DvbStrncpy(szIso3166Code, ISO3166CODE_IRN, 3); szIso3166Code[3] = '\0'; break;
	case eCountryID_IRQ:	MWC_UTIL_DvbStrncpy(szIso3166Code, ISO3166CODE_IRQ, 3); szIso3166Code[3] = '\0'; break;
	case eCountryID_KWT:	MWC_UTIL_DvbStrncpy(szIso3166Code, ISO3166CODE_KWT, 3); szIso3166Code[3] = '\0'; break;
	case eCountryID_OMN: 	MWC_UTIL_DvbStrncpy(szIso3166Code, ISO3166CODE_OMN, 3); szIso3166Code[3] = '\0'; break;
	case eCountryID_QAT:	MWC_UTIL_DvbStrncpy(szIso3166Code, ISO3166CODE_QAT, 3); szIso3166Code[3] = '\0'; break;
	case eCountryID_SAU:	MWC_UTIL_DvbStrncpy(szIso3166Code, ISO3166CODE_SAU, 3); szIso3166Code[3] = '\0'; break;
	case eCountryID_AZE:	MWC_UTIL_DvbStrncpy(szIso3166Code, ISO3166CODE_AZE, 3); szIso3166Code[3] = '\0'; break;
	case eCountryID_BGR: 	MWC_UTIL_DvbStrncpy(szIso3166Code, ISO3166CODE_BGR, 3); szIso3166Code[3] = '\0'; break;
	case eCountryID_TKM:	MWC_UTIL_DvbStrncpy(szIso3166Code, ISO3166CODE_TKM, 3); szIso3166Code[3] = '\0'; break;
	case eCountryID_UZB:	MWC_UTIL_DvbStrncpy(szIso3166Code, ISO3166CODE_UZB, 3); szIso3166Code[3] = '\0'; break;
	case eCountryID_OTHER:	MWC_UTIL_DvbStrncpy(szIso3166Code, "Oth", 3); szIso3166Code[3] = '\0'; break;
	default:
		return ERR_FAIL;
	}

	return ERR_OK;
}

#if 0
HERROR MWC_UTIL_GetCountryIdByIso3166Code(HUINT8 *szIso3166Code, HxCountry_e *peCountryId)
{
	HERROR			hErr = ERR_FAIL;
	HUINT8			aucIso3166[4];
	HxCountry_e		eCountryId = eCountryID_NONE;

	if(szIso3166Code == NULL)
		return ERR_FAIL;

	if (peCountryId == NULL)
		return ERR_FAIL;

	HxSTD_MemCopy(aucIso3166, szIso3166Code, 3);
	aucIso3166[3] = '\0';
	MWC_UTIL_ConvertStringUpper(aucIso3166);

	if(0 == MWC_UTIL_DvbStrncmp(aucIso3166, ISO3166CODE_AUS, 3))	{ eCountryId = eCountryID_AUS; 		hErr = ERR_OK; }
	if(0 == MWC_UTIL_DvbStrncmp(aucIso3166, ISO3166CODE_AUT, 3))	{ eCountryId = eCountryID_AUT; 		hErr = ERR_OK; }
	if(0 == MWC_UTIL_DvbStrncmp(aucIso3166, ISO3166CODE_CMR, 3))	{ eCountryId = eCountryID_CMR; 		hErr = ERR_OK; }
	if(0 == MWC_UTIL_DvbStrncmp(aucIso3166, ISO3166CODE_HRV, 3))	{ eCountryId = eCountryID_HRV; 		hErr = ERR_OK; }
	if(0 == MWC_UTIL_DvbStrncmp(aucIso3166, ISO3166CODE_CZE, 3))	{ eCountryId = eCountryID_CZE; 		hErr = ERR_OK; }
	if(0 == MWC_UTIL_DvbStrncmp(aucIso3166, ISO3166CODE_DNK, 3))	{ eCountryId = eCountryID_DNK; 		hErr = ERR_OK; }
	if(0 == MWC_UTIL_DvbStrncmp(aucIso3166, ISO3166CODE_GBR, 3))	{ eCountryId = eCountryID_GBR; 		hErr = ERR_OK; }
	if(0 == MWC_UTIL_DvbStrncmp(aucIso3166, ISO3166CODE_EST, 3))	{ eCountryId = eCountryID_EST; 		hErr = ERR_OK; }
	if(0 == MWC_UTIL_DvbStrncmp(aucIso3166, ISO3166CODE_FIN, 3))	{ eCountryId = eCountryID_FIN; 		hErr = ERR_OK; }
	if(0 == MWC_UTIL_DvbStrncmp(aucIso3166, ISO3166CODE_FRA, 3))	{ eCountryId = eCountryID_FRA; 		hErr = ERR_OK; }
	if(0 == MWC_UTIL_DvbStrncmp(aucIso3166, ISO3166CODE_DEU, 3))	{ eCountryId = eCountryID_DEU; 		hErr = ERR_OK; }
	if(0 == MWC_UTIL_DvbStrncmp(aucIso3166, ISO3166CODE_GRC, 3))	{ eCountryId = eCountryID_GRC; 		hErr = ERR_OK; }
	if(0 == MWC_UTIL_DvbStrncmp(aucIso3166, ISO3166CODE_HUN, 3))	{ eCountryId = eCountryID_HUN; 		hErr = ERR_OK; }
	if(0 == MWC_UTIL_DvbStrncmp(aucIso3166, ISO3166CODE_IRL, 3))	{ eCountryId = eCountryID_IRL; 		hErr = ERR_OK; }
	if(0 == MWC_UTIL_DvbStrncmp(aucIso3166, ISO3166CODE_ITA, 3))	{ eCountryId = eCountryID_ITA; 		hErr = ERR_OK; }
	if(0 == MWC_UTIL_DvbStrncmp(aucIso3166, ISO3166CODE_JPN, 3))	{ eCountryId = eCountryID_JPN; 		hErr = ERR_OK; }
	if(0 == MWC_UTIL_DvbStrncmp(aucIso3166, ISO3166CODE_KOR, 3))	{ eCountryId = eCountryID_KOR; 		hErr = ERR_OK; }
	if(0 == MWC_UTIL_DvbStrncmp(aucIso3166, ISO3166CODE_LVA, 3))	{ eCountryId = eCountryID_LVA; 		hErr = ERR_OK; }
	if(0 == MWC_UTIL_DvbStrncmp(aucIso3166, ISO3166CODE_LTU, 3))	{ eCountryId = eCountryID_LTU; 		hErr = ERR_OK; }
	if(0 == MWC_UTIL_DvbStrncmp(aucIso3166, ISO3166CODE_NLD, 3))	{ eCountryId = eCountryID_NLD; 		hErr = ERR_OK; }
	if(0 == MWC_UTIL_DvbStrncmp(aucIso3166, ISO3166CODE_NOR, 3))	{ eCountryId = eCountryID_NOR; 		hErr = ERR_OK; }
	if(0 == MWC_UTIL_DvbStrncmp(aucIso3166, ISO3166CODE_POL, 3))	{ eCountryId = eCountryID_POL; 		hErr = ERR_OK; }
	if(0 == MWC_UTIL_DvbStrncmp(aucIso3166, ISO3166CODE_PRT, 3))	{ eCountryId = eCountryID_PRT; 		hErr = ERR_OK; }
	if(0 == MWC_UTIL_DvbStrncmp(aucIso3166, ISO3166CODE_RUS, 3))	{ eCountryId = eCountryID_RUS; 		hErr = ERR_OK; }
	if(0 == MWC_UTIL_DvbStrncmp(aucIso3166, ISO3166CODE_SGP, 3))	{ eCountryId = eCountryID_SGP; 		hErr = ERR_OK; }
	if(0 == MWC_UTIL_DvbStrncmp(aucIso3166, ISO3166CODE_SVK, 3))	{ eCountryId = eCountryID_SVK; 		hErr = ERR_OK; }
	if(0 == MWC_UTIL_DvbStrncmp(aucIso3166, ISO3166CODE_SVN, 3))	{ eCountryId = eCountryID_SVN; 		hErr = ERR_OK; }
	if(0 == MWC_UTIL_DvbStrncmp(aucIso3166, ISO3166CODE_ESP, 3))	{ eCountryId = eCountryID_ESP; 		hErr = ERR_OK; }
	if(0 == MWC_UTIL_DvbStrncmp(aucIso3166, ISO3166CODE_SWE, 3))	{ eCountryId = eCountryID_SWE; 		hErr = ERR_OK; }
	if(0 == MWC_UTIL_DvbStrncmp(aucIso3166, ISO3166CODE_CHE, 3))	{ eCountryId = eCountryID_CHE; 		hErr = ERR_OK; }
	if(0 == MWC_UTIL_DvbStrncmp(aucIso3166, ISO3166CODE_TUR, 3))	{ eCountryId = eCountryID_TUR; 		hErr = ERR_OK; }
	if(0 == MWC_UTIL_DvbStrncmp(aucIso3166, ISO3166CODE_UKR, 3))	{ eCountryId = eCountryID_UKR; 		hErr = ERR_OK; }
	if(0 == MWC_UTIL_DvbStrncmp(aucIso3166, ISO3166CODE_USA, 3))	{ eCountryId = eCountryID_USA; 		hErr = ERR_OK; }
	if(0 == MWC_UTIL_DvbStrncmp(aucIso3166, ISO3166CODE_VNM, 3))	{ eCountryId = eCountryID_VNM; 		hErr = ERR_OK; }
	if(0 == MWC_UTIL_DvbStrncmp(aucIso3166, ISO3166CODE_BEL, 3))	{ eCountryId = eCountryID_BEL; 		hErr = ERR_OK; }
	if(0 == MWC_UTIL_DvbStrncmp(aucIso3166, ISO3166CODE_LUX, 3))	{ eCountryId = eCountryID_LUX; 		hErr = ERR_OK; }
	if(0 == MWC_UTIL_DvbStrncmp(aucIso3166, ISO3166CODE_ROU, 3))	{ eCountryId = eCountryID_ROU; 		hErr = ERR_OK; }
	if(0 == MWC_UTIL_DvbStrncmp(aucIso3166, ISO3166CODE_THA, 3))	{ eCountryId = eCountryID_THA; 		hErr = ERR_OK; }
	if(0 == MWC_UTIL_DvbStrncmp(aucIso3166, ISO3166CODE_ARE, 3))	{ eCountryId = eCountryID_ARE; 		hErr = ERR_OK; }
	if(0 == MWC_UTIL_DvbStrncmp(aucIso3166, ISO3166CODE_BHR, 3))	{ eCountryId = eCountryID_BHR; 		hErr = ERR_OK; }
	if(0 == MWC_UTIL_DvbStrncmp(aucIso3166, ISO3166CODE_EGY, 3))	{ eCountryId = eCountryID_EGY; 		hErr = ERR_OK; }
	if(0 == MWC_UTIL_DvbStrncmp(aucIso3166, ISO3166CODE_IRN, 3))	{ eCountryId = eCountryID_IRN; 		hErr = ERR_OK; }
	if(0 == MWC_UTIL_DvbStrncmp(aucIso3166, ISO3166CODE_IRQ, 3))	{ eCountryId = eCountryID_IRQ; 		hErr = ERR_OK; }
	if(0 == MWC_UTIL_DvbStrncmp(aucIso3166, ISO3166CODE_KWT, 3))	{ eCountryId = eCountryID_KWT; 		hErr = ERR_OK; }
	if(0 == MWC_UTIL_DvbStrncmp(aucIso3166, ISO3166CODE_OMN, 3))	{ eCountryId = eCountryID_OMN; 		hErr = ERR_OK; }
	if(0 == MWC_UTIL_DvbStrncmp(aucIso3166, ISO3166CODE_QAT, 3))	{ eCountryId = eCountryID_QAT; 		hErr = ERR_OK; }
	if(0 == MWC_UTIL_DvbStrncmp(aucIso3166, ISO3166CODE_SAU, 3))	{ eCountryId = eCountryID_SAU; 		hErr = ERR_OK; }
	if(0 == MWC_UTIL_DvbStrncmp(aucIso3166, ISO3166CODE_AZE, 3))	{ eCountryId = eCountryID_AZE; 		hErr = ERR_OK; }
	if(0 == MWC_UTIL_DvbStrncmp(aucIso3166, ISO3166CODE_BGR, 3))	{ eCountryId = eCountryID_BGR; 		hErr = ERR_OK; }
	if(0 == MWC_UTIL_DvbStrncmp(aucIso3166, ISO3166CODE_TKM, 3))	{ eCountryId = eCountryID_TKM; 		hErr = ERR_OK; }
	if(0 == MWC_UTIL_DvbStrncmp(aucIso3166, ISO3166CODE_UZB, 3))	{ eCountryId = eCountryID_UZB; 		hErr = ERR_OK; }
	if(0 == MWC_UTIL_DvbStrncmp(aucIso3166, "Oth", 3))				{ eCountryId = eCountryID_OTHER;	hErr = ERR_OK; }

	*peCountryId = eCountryId;

	return hErr;
}
#endif

#define _____EXTRACT_ONE_CHARACTER_FROM_STRING_____
// ==============================================================
#if 0
HERROR MWC_UTIL_CountDrawableCharNumInString (eHxCP_CodePage_e eEncoding, HUINT8 *szString, HUINT32 *pulCharNum)
{
	HERROR			hErr = ERR_FAIL;
	CPContext_t 	*pCPContext = NULL;

	if (szString == NULL || pulCharNum == NULL)
	{
		return ERR_FAIL;
	}

	pCPContext = MWC_UTIL_CodePage_GetContext(eEncoding);
	if(pCPContext == NULL)
	{
		return ERR_ENCODING_FAIL;
	}

	if(pCPContext->cbCountDrawableCharNum != NULL)
	{
		hErr = pCPContext->cbCountDrawableCharNum(szString, pulCharNum);
	}

	return hErr;
}

HERROR MWC_UTIL_GetDrawableCharInString (eHxCP_CodePage_e eEncoding, HUINT8 *szString, HUINT32 ulIndex,
										 HUINT8 *szCharWithCode, HUINT32 *pulCharLenWithCode,
										 HUINT8 *szCharOnly, HUINT32 *pulCharLenOnly)
{

	HERROR			hErr = ERR_FAIL;
	CPContext_t 	*pCPContext = NULL;

	if (szString == NULL)
	{
		return ERR_FAIL;
	}

	pCPContext = MWC_UTIL_CodePage_GetContext(eEncoding);
	if(pCPContext == NULL)
	{
		return ERR_ENCODING_FAIL;
	}

	if(pCPContext->cbGetDrawableChar != NULL)
	{
		hErr = pCPContext->cbGetDrawableChar(szString, ulIndex, szCharWithCode, pulCharLenWithCode, szCharOnly, pulCharLenOnly);
	}

	return hErr;
}

#endif

/****************************************************************************
Description	:	일단은 기존 방식대로 1Byte Char 로 바꾸는 방식 사용함.
nLen 은 1 이 입력되어야 함. pucSrcStr[0] = 0x11
Input		:
Output		:
Return		:
****************************************************************************/
STATIC HUINT32 local_mwc_util_DvbUnicodeStrPostProcessing(HUINT8 *pucSrcStr, HUINT32 ucSrcLen, HUINT32 nLen)
{
	HUINT32 i, j;
	HUINT32	len = ucSrcLen;

	for (i = nLen, j = 0; i < len - 1; i+=2) // 2 byte 단위
	{
		if ((pucSrcStr[i] == 0xE0) && (0x80<=pucSrcStr[i+1] && pucSrcStr[i+1] <= 0x9F))
		{
			if ((pucSrcStr[i] == 0xE0) && (pucSrcStr[i+1] == 0x8A))
			{
				pucSrcStr[j] = pucSrcStr[i+1];
				j++;
			}
		}
		else if (pucSrcStr[i] != 0x00)
		{
			// '0x0000' ~ '0x00FF'(ASCII + Latin-1 code)를 제외한 나머지 2-byte code는 삭제.
		}
		else
		{
			if (pucSrcStr[i+1] < 0x20)
			{
				/* 1F 이하 또는 NIL 문자 삭제 */
			}
			else
			{
				pucSrcStr[j] = pucSrcStr[i+1];
				j++;
			}
		}
	}

	return (HUINT32)j;
}

/****************************************************************************
Description	:	UTF8을 지원하는 여부에 따라서 달리 구현.
nLen 은 1 이 입력되어야 함. pucSrcStr[0] = 0x15
Input		:
Output		:
Return		:
****************************************************************************/
STATIC HUINT32 local_mwc_util_DvbUtf8StrPostProcessing(HUINT8 *pucSrcStr, HUINT32 ucSrcLen, HUINT32 nLen)
{
	HUINT32 i, j;
	HUINT32	len = ucSrcLen;

	/* 임시, NIL 또는 Ctrl 문자 삭제해야 함. */
	for (i = nLen, j = nLen ; i < len ; )
	{
		if( (pucSrcStr[i] & 0x80) == 0x00) // 1 바이트를 읽어야 하는 부분
		{
			if (pucSrcStr[i] >= 0x20)
			{
				/* 1F 이하 또는 NIL 문자 삭제 */
				pucSrcStr[j] = pucSrcStr[i];
				j++;
			}
			i++;
		}
		else if( ((pucSrcStr[i] & 0xC0) == 0xC0) && ((pucSrcStr[i] & 0xE0) != 0xE0)) // 2 바이트를 읽어야 하는 부분
		{
			if ((pucSrcStr[i] == 0xC2) && ((0x80 <= pucSrcStr[i+1]) && (pucSrcStr[i+1] < 0x9F)))
			{
				/* Ctl 문자 삭제  - CRLF */
				if (((i+1) < len ) && (pucSrcStr[i] == 0xC2) && (pucSrcStr[i+1] == 0x8A))
				{
					pucSrcStr[j] = pucSrcStr[i];
					pucSrcStr[j+1] = pucSrcStr[i+1];
					j += 2;
				}
			}
			else
			{
				if ((i + 1) < len)
				{
					pucSrcStr[j] = pucSrcStr[i];
					pucSrcStr[j+1] = pucSrcStr[i+1];
					j += 2;
				}
			}
			i += 2;
		}
		else if( (pucSrcStr[i] & 0xE0) == 0xE0 ) // 3 바이트를 읽어야 하는 부분
		{
			if ((pucSrcStr[i] == 0xEE) && (pucSrcStr[i+1] == 0x82) && ((0x80 <= pucSrcStr[i+2]) && (pucSrcStr[i+2] < 0x9F)))
			{
				/* Ctl 문자 삭제  - CRLF */
				if (((i+2) < len ) && (pucSrcStr[i] == 0xEE) && (pucSrcStr[i+1] == 0x82) && (pucSrcStr[i+2] == 0x8A))
				{
					pucSrcStr[j] = pucSrcStr[i];
					pucSrcStr[j+1] = pucSrcStr[i+1];
					pucSrcStr[j+2] = pucSrcStr[i+2];
					j += 3;
				}
			}
			else
			{
				if ((i+2) < len)
				{
					pucSrcStr[j] = pucSrcStr[i];
					pucSrcStr[j+1] = pucSrcStr[i+1];
					pucSrcStr[j+2] = pucSrcStr[i+2];
					j += 3;
				}
			}
			i += 3;
		}
		else
		{
			i += 4;
		}
	}
	return (HUINT32) j;
}


/****************************************************************************
Description	:
Input		:
Output		:
Return		:
****************************************************************************/
STATIC HUINT32 local_mwc_util_DvbOneByteCharStrPostProcessing(HUINT8 *pucSrcStr, HUINT32 ucSrcLen, HUINT32 nLen)
{
	HUINT32	i, j;
	HUINT32	len = ucSrcLen;

	for(i = nLen, j = nLen; i < len; i++)
	{
		if (0x80 <= pucSrcStr[i] && pucSrcStr[i] <= 0x9F)
		{
			if (pucSrcStr[i] == 0x8A)	/* CR/LF */
			{
				pucSrcStr[j]= pucSrcStr[i];
				j++;
			}
		}
		else if (pucSrcStr[i] < 0x20)
		{
			/* 1F 이하 또는 NULL 문자 삭제 */
		}
		else
		{
			pucSrcStr[j] = pucSrcStr[i];
			j++;
		}
	}

	return (HUINT32) j;
}

/****************************************************************************
Description	:	Char code table value가 없는 경우는 ISO 6937 로 고려.
UK 향 각 Char table 에서 특수 문자 제거 등의 처리
Input	  	:	pucMem 은 Prefix + 실제 MemData	=> 마지막의 Null 문자는 Caller의 책임.
Output		:
Return		:
****************************************************************************/
HUINT32 local_mwc_util_DvbStrPostProcessing(HUINT8 *pucMem, HUINT32 ucMemLen)
{
	HUINT32	resultLen;
	HUINT32	charCodeTableLen = 0;
	HBOOL	isNeedPostProcessing = FALSE;
	HUINT32	typePostProcessing = 0;

	if (pucMem == NULL)
		return ucMemLen;
	if (ucMemLen == 0)
		return 0;
	resultLen = ucMemLen;

	switch (*pucMem)
	{
	case eHxCP_DVBLANG_VALUE_8859_05:
	case eHxCP_DVBLANG_VALUE_8859_06:
	case eHxCP_DVBLANG_VALUE_8859_07:
	case eHxCP_DVBLANG_VALUE_8859_08:
	case eHxCP_DVBLANG_VALUE_8859_09:
	case eHxCP_DVBLANG_VALUE_8859_10:
	case eHxCP_DVBLANG_VALUE_8859_11:
	case eHxCP_DVBLANG_VALUE_8859_12:
	case eHxCP_DVBLANG_VALUE_8859_13:
	case eHxCP_DVBLANG_VALUE_8859_14:
	case eHxCP_DVBLANG_VALUE_8859_15:
		isNeedPostProcessing = TRUE;
		typePostProcessing = 0;
		charCodeTableLen= 1;
		break;
	case eHxCP_DVBLANG_VALUE_8859:	/* charCodeTableLen 이 3 byte 계열 */
		if (ucMemLen >= 3)
		{
			if ((pucMem[1] == 0x69) && (pucMem[2] == 0x37))
			{
				isNeedPostProcessing = TRUE;
				typePostProcessing = 0;
				charCodeTableLen= 3;
			}
			else if ((pucMem[1] == 0x0) && (0x0 < pucMem[2] && pucMem[2] < 0x10))
			{
				isNeedPostProcessing = TRUE;
				typePostProcessing = 0;
				charCodeTableLen= 3;
			}
		}
		break;

	case eHxCP_DVBLANG_VALUE_UTF_8:
		isNeedPostProcessing = TRUE;
		typePostProcessing = 1;
		charCodeTableLen= 1;
		break;
	case eHxCP_DVBLANG_VALUE_UCS2:	//eHxCP_DVBLANG_VALUE_UNICODE:
		isNeedPostProcessing = TRUE;
		typePostProcessing = 2;
		charCodeTableLen= 1;
		break;

	default:
		if (*pucMem >= 0x20)
		{
			isNeedPostProcessing = TRUE;
			typePostProcessing = 0;
			charCodeTableLen= 0;
		}
		break;
	}


	if (FALSE == isNeedPostProcessing)
	{
		return resultLen;
	}

	if (typePostProcessing == 0) 		/* ISO 6937 or eHxCP_DVBLANG_VALUE_8859_XX 계열 */
	{
		resultLen = local_mwc_util_DvbOneByteCharStrPostProcessing(pucMem, ucMemLen, charCodeTableLen);
		if (resultLen < ucMemLen)
			pucMem[resultLen] = 0;
	}
	else if (typePostProcessing == 1)	/* UTF-8 */
	{
		resultLen = local_mwc_util_DvbUtf8StrPostProcessing(pucMem, ucMemLen, charCodeTableLen);
		if (resultLen < ucMemLen)
			pucMem[resultLen] = 0;
	}
	else if (typePostProcessing == 2)	/* Unicode */
	{
		resultLen = local_mwc_util_DvbUnicodeStrPostProcessing(pucMem, ucMemLen, charCodeTableLen);
		if (resultLen < ucMemLen)
			pucMem[resultLen] = 0;
		if (resultLen+1 < ucMemLen)
			pucMem[resultLen+1] = 0;
	}

	return resultLen;
}

#define _____SI_SOURCE_STRING_CONVERTER_____
// SI 등 외부 Signal을 통해 오는 String을 하나의 통일된 format으로 수정하는 기능들



HERROR MWC_UTIL_GetSiStringEncodedType (SiTxtEncodedType_t *peEncType)
{
	if (peEncType == NULL)							{ return ERR_FAIL; }

	*peEncType = s_IsoSvc_eSiEncodedType;
	return ERR_OK;
}


HERROR MWC_UTIL_SetSiStringEncodedType (SiTxtEncodedType_t eEncType)
{
	if (eEncType >= eSITEXT_ENCODED_MAX)			{ return ERR_FAIL; }

	s_IsoSvc_eSiEncodedType = eEncType;
	return ERR_OK;
}


//#define	TEST_SILIB
static HCHAR *	local_mwc_util_HandleDecodeResult (const HCHAR *result, void *userdata)
{
	static const HUINT32 utf8_skip_data[256] = {
		1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
		1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
		1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
		1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
		1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
		1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
		2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
		3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,4,4,4,4,4,4,4,4,5,5,5,5,6,6,1,1
	};
	HUINT32	skip;
	HUINT32	len;
	HCHAR	*dup;
	HUINT8	*d;
	const HUINT8 *s;

	len = HxSTD_StrLen(result);
	dup = (HCHAR *)OxSI_Malloc(len + 1);	// 아래 size가 늘어나는 작업이 없기 때문에 같은 사이즈로 alloc
	if (dup == NULL)
		return NULL;

	for (s = (const HUINT8 *)result, d = (HUINT8 *)dup ; *s != '\0' ; )
	{
		skip = utf8_skip_data[(int)*s];

		if (skip == 1)
		{

			if ((*s == 0xa) || (*s == 0xd))
			{
				*d++ = '\n'; // carrage return;
				s++;
				continue;
			}

			if (*s < 0x20)
			{
				*d++ = 0x20; // space;
				s++;
				continue;
			}

		}
		else if (skip == 3)
		{
			if (s[0] == 0xEE && s[1] == 0x82)
			{
#if defined(CONFIG_OP_FREESAT)
				if (s[2] == 0x8A)
				{
					*d++ = '\n';
					s += 3;
					continue;
				}
#endif
#if defined(CONFIG_PROD_HMS1000S) || defined(CONFIG_PROD_HMSLITE) || defined(CONFIG_PROD_HMS1000T) || defined(CONFIG_PROD_DIGIMAXT2) // HMS에서 임시로 사용한다. Web에서 처리하는 방안 확실해 지면 지워도록 하자.
				if (s[2] >= 0x80)
				{
					if (s[2] >= 0x8A)
					{
						*d++ = '\n';
					}
					s += 3;
					continue;
				}
#endif
			}
		}

		while(skip--)
			*d++ = *s++;
	}
	*d = '\0';

#if defined(CONFIG_DEBUG) && defined(TEST_SILIB)
	if (userdata)
	{
		if (strcmp(dup, userdata) != 0)
		{
			HxLOG_Print("A:%s\n", dup);
			HxLOG_Print("B:%s\n", userdata);
			abort();
		}
		OxSI_Free(dup);
	}
#endif

	(void)userdata;
	return dup;
}


HUINT32 MWC_UTIL_ConvSiStringToUtf8 (HUINT8 *szLangCode, HUINT32 ulSrcStrLen, HUINT8 *szSrcStr, HUINT8 **pszDstStr)
{
	static const HINT32	s_textEncMap[][2] =
	{
		{eSITEXT_ENCODED_300468,        eSIxTEXT_ENC_300468},
		{eSITEXT_ENCODED_300468_EUROPE, eSIxTEXT_ENC_DEFAULT},
		{eSITEXT_ENCODED_300468_EUROPE, eSIxTEXT_ENC_300468_8859_9_BASE},
		{eSITEXT_ENCODED_300468_UK,     eSIxTEXT_ENC_300468_UK},
		{eSITEXT_ENCODED_JAPAN_JIS,     eSIxTEXT_ENC_JIS}
	};
	SiTxtEncodedType_t	encType;
	SIxTextEncoding_e		sxEncType;

	(void) MWC_UTIL_GetSiStringEncodedType (&encType);

	sxEncType  = (SIxTextEncoding_e)HLIB_STD_Map(0, (HINT32)encType, s_textEncMap
							, sizeof(s_textEncMap) / sizeof(s_textEncMap[0]), eSIxTEXT_ENC_DEFAULT);

	*pszDstStr = SILIB_TEXT_MakeUtf8Ex(sxEncType, (HCHAR *)szLangCode, szSrcStr, ulSrcStrLen, local_mwc_util_HandleDecodeResult, NULL);
	if (*pszDstStr)
		return HxSTD_StrLen(*pszDstStr);
	return 0;
}


#define _____TRIM_____

HUINT32 MWC_UTIL_LTrimString(HUINT8 *src, HINT32 length, HUINT8 *dest)
{
	HINT32 i;

	for (i=0; i<length; i++)
	{
		if (src[i] != 0x20/*SPACE*/)
			break;
	}

	if (i >= length)
		return !ERR_OK;

	HxSTD_MemCopy(dest, src+i, length-i);
	return ERR_OK;
}

HUINT32 MWC_UTIL_RTrimString(HUINT8 *src, HINT32 length, HUINT8 *dest)
{
	HINT32 i;

	for (i=length-1; i>=0; i--)
	{
		if (src[i] != 0x20/*SPACE*/)
			break;
	}

	if (i <= 0)
		return !ERR_OK;

	HxSTD_MemCopy(dest, src, i+1);
	return ERR_OK;
}

HUINT32 MWC_UTIL_BothTrimString( HUINT8 *src, HINT32 length, HUINT8 *dest)
{
	if (src == NULL || length <= 0 || dest == NULL)
		return !ERR_OK;

	MWC_UTIL_LTrimString(src, length, dest);
	HxSTD_MemSet(src, 0, length);
	HxSTD_MemCopy(src, dest, MWC_UTIL_DvbStrlen(dest));
	HxSTD_MemSet(dest, 0, MWC_UTIL_DvbStrlen(dest));
	return MWC_UTIL_RTrimString(src, MWC_UTIL_DvbStrlen(src), dest);
}

static ConvEncoding_Err_t	local_mwc_util_ConvertEncodingToUtf8(const eHxCP_CodePage_e eSourceEncoding, const CH_1BYTE *pSrcStr, const HINT32 nSrcStrLen, const HINT32 nBufferLen, HINT32 *pnUsedByte, CH_UTF8 *pUtf8Str, HINT32 *pnUtf8StrLen)
{
	ConvEncoding_Err_t	hErr = ERR_ENCODING_FAIL;

	HxLOG_Info("local_mwc_util_ConvertEncodingToUtf8 eSourceEncoding(0x%x) pSrcStr(%s) +\n", eSourceEncoding, pSrcStr);

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
		*pnUsedByte = *pnUtf8StrLen;
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
		*pnUtf8StrLen = HLIB_CP_UTIL_ConvertCodepageToUTF8(eSourceEncoding, pSrcStr, pUtf8Str, nBufferLen);
		*pnUsedByte = *pnUtf8StrLen;
		return hErr;
	}

	HxLOG_Info("local_mwc_util_ConvertEncodingToUtf8() -\n");

	return ERR_OK;
}

/******************************************************************/
#define ______________CODEPAGE_UTIL_PRIVATE________________________________
/******************************************************************/
STATIC HERROR	local_mwc_util_Convert300468ToUtf8(const CH_1BYTE *pStr300468, const HINT32 nStr300468Len, const HINT32 nBufferLen, HINT32 *pnUsedByte, CH_UTF8 *pUtf8Str, HINT32 *pnUtf8StrLen)
{
	HINT32 			nLocaleCodeLen = 0;
	eHxCP_CodePage_e eEncoding = eHCodepage_MAX;

	if(pStr300468 == NULL)	{ return 0; }

	local_mwc_util_GetEncodingWithLenByLocaleCode(pStr300468, &eEncoding, &nLocaleCodeLen, eHxCP_ISO_300468);

	return local_mwc_util_ConvertEncodingToUtf8(eEncoding, pStr300468+nLocaleCodeLen, nStr300468Len-nLocaleCodeLen, nBufferLen, pnUsedByte, pUtf8Str, pnUtf8StrLen);
}


STATIC HERROR local_mwc_util_GetSprintToken (HUINT8 *szString, HUINT8 *szTokenString, HUINT32 *pulTokenSize)
{
	// 맨 앞이 % 가 온 것을 확인하고 들어온 함수이므로 처음 체크는 당연히 생략한다.
	// %s, %% 는 이전에 이미 걸러 졌음을 확인
	HUINT8			*szStartString;

	szStartString = szString;
	*szTokenString = *szString;

	szString ++;
	szTokenString ++;
	*pulTokenSize = 1;

	while (*szString != '\0')
	{
		switch (*szString)
		{
		case 'c':		// Character
		case 'd':		// decimal
		case 'i':		// decimal
		case 'u':		// unsigned decimal
		case 'f':		// floating point
		case 'x':		// hexadecimal (lower)
		case 'X':		// hexadecimal (upper)
			*szTokenString = *szString;

			szString ++;
			szTokenString ++;
			*pulTokenSize = *pulTokenSize + 1;

			*szTokenString = '\0';
			return ERR_OK;
		}

		if (*szString == '+' || *szString == '-')
		{
			if ((szStartString + 1) != szString)
			{
				*szTokenString = *szString;

				szString ++;
				szTokenString ++;
				*pulTokenSize = *pulTokenSize + 1;

				*szTokenString = '\0';
				return ERR_FAIL;
			}
		}
		else if ((*szString < '0' || *szString > '9') && (*szString != '.'))
		{
			*szTokenString = *szString;

			szString ++;
			szTokenString ++;
			*pulTokenSize = *pulTokenSize + 1;

			*szTokenString = '\0';
			return ERR_FAIL;
		}

		*szTokenString = *szString;

		szString ++;
		szTokenString ++;
		*pulTokenSize = *pulTokenSize + 1;
	}

	*szTokenString = *szString;

	return ERR_FAIL;
}




/******************************************************************/
#define ______________CODEPAGE_UTIL_PUBLIC________________________________
/******************************************************************/


HUINT32		MWC_UTIL_GetLocaleCodeLen(const HUINT8 *pcStr)
{
	HUINT32	ulEncodingLen = 0;
	eHxCP_CodePage_e eEncoding = eHCodepage_MAX;

	if((pcStr == NULL) || (*pcStr == 0x00))		{ return 0; }

	local_mwc_util_GetEncodingWithLenByLocaleCode(pcStr, &eEncoding, &ulEncodingLen, SETTING_DEFAULT_ENCODING);

	return ulEncodingLen;
}


eHxCP_CodePage_e		MWC_UTIL_GetEncodingByLocaleCode(const CH_1BYTE *pcStr)
{
	eHxCP_CodePage_e	eTmpEncoding = eHCodepage_MAX;
	HINT32			nEncodingCountLen = 0;

	if((pcStr == NULL))		{ return eHCodepage_MAX; }

//	local_mwc_util_GetEncodingWithLenByLocaleCode(pcStr, &eTmpEncoding, &nEncodingCountLen, eHCodepage_MAX);
	local_mwc_util_GetEncodingWithLenByLocaleCode(pcStr, &eTmpEncoding, &nEncodingCountLen, SETTING_DEFAULT_ENCODING);

	return eTmpEncoding;
}


#define	TMPBUF_SIZE_FOR_CONV_UTF8_STRING	512

HUINT8	*MWC_UTIL_DvbUtf8Sprintf(HUINT8 *pStr, const HUINT8 *format, ...)
{
	HUINT8	*pFmt = NULL;
	HUINT8	*pStrArg = NULL;
	CH_1BYTE	*tmpBuf = NULL;

	HINT32	nTmpBufLen = 0, n = 0;
	HINT32	nNumOfFmt = 0, i  = 0;

	HINT32	nTmpUsedByte = 0;
	HINT32	nConvertedUtf8Len = 0;
	va_list argument;
	HUINT32	ulTokenSize;
	HUINT8	szTokenString[16];
	double	fVal;
	HINT8	ucVal;
	HERROR	hErr = ERR_FAIL;

	if(pStr == NULL)
	{
		return NULL;
	}

	tmpBuf = (CH_1BYTE *)OxMW_Malloc (TMPBUF_SIZE_FOR_CONV_UTF8_STRING);
	if (tmpBuf == NULL)
	{
		return NULL;
	}

	va_start(argument, format);
	nNumOfFmt = MWC_UTIL_DvbStrlen(format);

	pStr[0] = 0x15;
	pStr[1] = 0x0;

	HxSTD_MemSet(tmpBuf, 0, 512);

	for(i = 0, pFmt = (HUINT8 *)format; ((i < nNumOfFmt) && (*pFmt != NULL)); pFmt++, i++)
	{
		if(*pFmt == '%')
		{
			// String :
			if (VK_strncmp (pFmt, "%s", 2) == 0)
			{
				pStrArg = (HUINT8 *)va_arg (argument, HINT8 *);
				nTmpBufLen = MWC_UTIL_DvbStrlen(pStrArg);

				nConvertedUtf8Len = 0;
				hErr = local_mwc_util_Convert300468ToUtf8((const HUINT8 *) pStrArg,
					(const HINT32) nTmpBufLen,
					(const HINT32) TMPBUF_SIZE_FOR_CONV_UTF8_STRING,
					(HINT32 *) &nTmpUsedByte,
					(CH_1BYTE *) tmpBuf,
					(HINT32 *) &nConvertedUtf8Len);
				if (hErr != ERR_OK)
				{
					HxLOG_Debug("local_mwc_util_Convert300468ToUtf8 error !!! \n");
				}

				tmpBuf[nConvertedUtf8Len] = '\0';
				// 쓸데없는 계산을 줄이기 위해 이미 UTF8화된 것을 DvbStrcat로 더 복잡하게 하지 않는다.
				VK_strcat (pStr, tmpBuf);
				// MWC_UTIL_DvbStrcat (pStr, tmpBuf);
				pFmt ++;		i++;
			}
			else if (VK_strncmp (pFmt, "%%", 2) == 0)
			{
				// 쓸데없는 계산을 줄이기 위해 이미 UTF8화된 것을 DvbStrcat로 더 복잡하게 하지 않는다.
				VK_strcat (pStr, "%");
				// MWC_UTIL_DvbStrcat (pStr, "%");
				pFmt ++;		i++;
			}
			else
			{
				hErr = local_mwc_util_GetSprintToken (pFmt, szTokenString, &ulTokenSize);
				if (hErr == ERR_OK)
				{
					// Token에 따라 처리가 약간씩 달라진다 .
					switch (szTokenString[ulTokenSize - 1])
					{
					case 'f':
						fVal = (double)va_arg(argument, double);
						sprintf(tmpBuf, szTokenString, fVal);
						break;
					case 'c':
						ucVal = (HINT8)va_arg(argument, int);
						sprintf(tmpBuf, szTokenString, ucVal);
						break;
					default:
						n = va_arg(argument, int);
						sprintf(tmpBuf, szTokenString, n);
						break;
					}

					// 쓸데없는 계산을 줄이기 위해 이미 UTF8화된 것을 DvbStrcat로 더 복잡하게 하지 않는다.
					VK_strcat (pStr, tmpBuf);
					// MWC_UTIL_DvbStrcat (pStr, tmpBuf);

					pFmt += (ulTokenSize - 1);
					i += (ulTokenSize - 1);
				}
				else
				{
					// 제대로 된 Token이 아니면 다 Skip 되어야 한다.
					pFmt += (ulTokenSize - 1);
					i += (ulTokenSize - 1);
				}
			}
#if 0
			switch(*pFmt)
			{
			case 'd':
			case 'i':
				n = va_arg(argument, int);
				sprintf(tmpBuf, "%d", n);
				MWC_UTIL_DvbStrcat(pStr, tmpBuf);
				break;

			case '%':
				MWC_UTIL_DvbStrcat(pStr, "%");
				break;

			case 'x':
			case 'X':
				n = va_arg(argument, int);
				sprintf(tmpBuf, "%x", n);
				MWC_UTIL_DvbStrcat(pStr, tmpBuf);
				break;

			case 'c':
				ch = (HINT8)va_arg(argument, int);
				sprintf (tmpBuf, "%c", ch);
				MWC_UTIL_DvbStrcat(pStr, tmpBuf);
				break;

			case 's':
				pStrArg = (HUINT8 *)va_arg (argument, HINT8 *);
				nTmpBufLen = MWC_UTIL_DvbStrlen(pStrArg);

				nConvertedUtf8Len = 0;
				hErr = local_mwc_util_Convert300468ToUtf8((const HUINT8 *) pStrArg,
					(const HUINT16) nTmpBufLen,
					(const HUINT16) 512,
					(HUINT16 *) &nTmpUsedByte,
					(HUINT8 *) tmpBuf,
					(HUINT16 *) &nConvertedUtf8Len);

				tmpBuf[nConvertedUtf8Len] = '\0';
				MWC_UTIL_DvbStrcat (pStr, tmpBuf);
				break;

			default:
				break;
			}
#endif
		}
		else
		{
			sprintf (tmpBuf, "%c", *pFmt);
			// 쓸데없는 계산을 줄이기 위해 이미 UTF8화된 것을 DvbStrcat로 더 복잡하게 하지 않는다.
			VK_strcat (pStr, tmpBuf);
			// MWC_UTIL_DvbStrcat (pStr, tmpBuf);
		}
	}

	va_end(argument);

	OxMW_Free (tmpBuf);

	return pStr;
}



/*********************** End of File ******************************/
