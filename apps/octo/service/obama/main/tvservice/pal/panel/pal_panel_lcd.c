/**
	@file     pal_panel_kraken.c
	@brief    Kraken platform specific front panel information

	Description: 각 제품별로 선택적으로 컴파일되어 platform이 제공하는 기능을 고른다.
	Module: PAL/PANEL			 	\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#include <isosvc.h>

#include <di_led.h>

#include <pal_panel.h>
#include "_pal_panel.h"

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/



#if defined(CONFIG_DEBUG)
#define	LOG					HxLOG_Print
#define INFO_LOG			HxLOG_Info
#define	ERR_LOG				HxLOG_Error
#define MSG_LOG				HxLOG_Message
#define	PLCDENTER			HxLOG_Info("(+)Enter!!\n");
#define	PLCDLEAVE(X)		HxLOG_Info("(-)Leave -- (0x%X)\n",X);
#else
#define	LOG					OCTO_NULL_PRINT
#define INFO_LOG			OCTO_NULL_PRINT
#define	ERR_LOG				OCTO_NULL_PRINT
#define MSG_LOG				OCTO_NULL_PRINT
#define	PLCDENTER			OCTO_NULL_PRINT
#define	PLCDLEAVE(X)		OCTO_NULL_PRINT
#endif

#define PAL_PANEL_KRAKEN_STRING_LEN			CONFIG_MW_SYS_STRING_LEN
#define PAL_PANEL_STRING_LEN					(PAL_PANEL_KRAKEN_STRING_LEN)

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

typedef struct {
	DxLedId_e	eOctoLedId;
	DI_LED_ID	eDiLedId;
} LedInfo_t;

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

LedInfo_t stLedInfo[] =
{
	{ eDxLED_ID_TV,				 DI_LED_NUM0}
	,{ eDxLED_ID_RADIO,			 DI_LED_NUM1}

	,{ eDxLED_ID_REC,			 DI_LED_NUM2}
	,{ eDxLED_ID_PLAY,			 DI_LED_NUM3}
	,{ eDxLED_ID_PAUSE,			 DI_LED_NUM4}
	,{ eDxLED_ID_RESERVE,		 DI_LED_NUM5}

	// 0 means not exist.
	,{ eDxLED_ID_SRS,			 0}
	,{ eDxLED_ID_SRC_TS,		 0}
	,{ eDxLED_ID_SRC_XT,		 0}

	,{ eDxLED_ID_DOLBY,			 DI_LED_NUM10}
	,{ eDxLED_ID_HD,			 DI_LED_NUM9}
	,{ eDxLED_ID_1080P,			 DI_LED_NUM11}

	,{ eDxLED_ID_720P,			 DI_LED_NUM12}
	,{ eDxLED_ID_576P,			 DI_LED_NUM13}
	,{ eDxLED_ID_480P,			 DI_LED_NUM14}
	,{ eDxLED_ID_1080I,			 DI_LED_NUM15}
	,{ eDxLED_ID_576I,			 DI_LED_NUM17}
	,{ eDxLED_ID_480I,			 DI_LED_NUM16}

	,{ eDxLED_ID_DOLBY_PLUS,	 0}
	,{ eDxLED_ID_DOLBY_PULSE,	 0}

	,{ eDxLED_ID_WIFI,			 0}
	,{ eDxLED_ID_USB,			 0}

	,{ eDxLED_ID_HDD_25_FULL,	 0}
	,{ eDxLED_ID_HDD_50_FULL,	 0}
	,{ eDxLED_ID_HDD_75_FULL,	 0}
	,{ eDxLED_ID_HDD_100_FULL,	 0}

	,{ eDxLED_ID_LINK,			 0}

	,{ eDxLED_ID_MAIL,			 0}
	,{ eDxLED_ID_RED_COLOR,		 0}
	,{ eDxLED_ID_BLUE_COLOR,	 0}
	,{ eDxLED_ID_ORANGE_COLOR,	 0}
};

/*******************************************************************/
/******************* Static Function Prototypes  *************************/
/*******************************************************************/
static HERROR		pal_panel_setAlign(HUINT8 *pString, DxPanelAlign_e eAlign);

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/
#ifdef CONFIG_MW_SYS_RUSSIAN_FRONT_PANEL

HUINT8 pal_panel_ConvertCharToRussianFront(HUINT8 ucChar)
{
/* 0x20: space, 0x3f: ? */
	HUINT8 aucCharMap8859_1_fixed[] = {
					0x20, 0xA2, 0x3F, 0x3F, 0x3F, 0x53, 0x49, 0x49, 0x4A, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0xcc, 0x3f, /* A0 */
                                 	0x41, 0xA0, 0x42, 0xA1, 0xE0, 0x45, 0xA3, 0xA4, 0xA5, 0xA6, 0x4B, 0xA7, 0x4D, 0x48, 0x4F, 0xA8, /* B0 */
                                  	0x50, 0x43, 0x54, 0xA9, 0xAA, 0x58, 0xE1, 0xAB, 0xAC, 0xE2, 0xAD, 0xAE, 0x62, 0xAF, 0xB0, 0xB1, /* C0 */
					// micom1.3: 뒤집힌 N 소문자가 잘못되어 임시로 뒤집힌 N 대문자를 대신쓰기로 함. --> OP요청으로 "u"로 변경
                            	0x61, 0xB2, 0xB3, 0xB4, 0xE3, 0x65, 0xB6, 0xB7, 0x75 /* 0xB8 */, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0x6F, 0xBE, /* D0 */
                                  	0x70, 0x63, 0xBF, 0x79, 0xE4, 0x78, 0xE5, 0xC0, 0xC1, 0xE6, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, /* E0 */
                                 	0x4E, 0xB5, 0x3f, 0x3f, 0x3f, 0x73, 0x69, 0x69, 0x6A, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0xcc, 0x3f, /* F0 */
                                 	};
HUINT8 aucCharMap8859_1[] = {
								0x20, 0xA2, 0x3F, 0x3F, 0x3F, 0x53, 0x49, 0x49, 0x4A, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0xcc, 0x3f, /* A0 */
                                                 	0x41, 0xA0, 0x42, 0xA1, 0xE0, 0x45, 0xA3, 0xA4, 0xA5, 0xA6, 0x4B, 0xA7, 0x4D, 0x48, 0x4F, 0xA8, /* B0 */
                                                 	0x50, 0x43, 0x54, 0xA9, 0xAA, 0x58, 0xE1, 0xAB, 0xAC, 0xE2, 0xAD, 0xAE, 0x62, 0xAF, 0xB0, 0xB1, /* C0 */
					// micom 1.4:  뒤집힌 N 소문자는 micom에서 그려서 보여줌
                          		0x61, 0xB2, 0xB3, 0xB4, 0xE3, 0x65, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0x6F, 0xBE, /* D0 */
                                                     	0x70, 0x63, 0xBF, 0x79, 0xE4, 0x78, 0xE5, 0xC0, 0xC1, 0xE6, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, /* E0 */
                                                     	0x4E, 0xB5, 0x3f, 0x3f, 0x3f, 0x73, 0x69, 0x69, 0x6A, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0xcc, 0x3f, /* F0 */
                                                     	};

	if (ucChar < 0xA0)
		return ucChar;

	return aucCharMap8859_1[ucChar - 0xA0];
}

// 0x0으로 끝나지 않고, 0xD0 0x00으로 끝나는 경우 있음..
void pal_panel_SetRussianNullTermination(HUINT8 *pszStr)
{
	HUINT8 *pucIdx;

	if (pszStr[0] != 0x15)
		return;

	pucIdx = pszStr + 1;

	while (*pucIdx != 0)
	{
		// 1 byte
		if (*pucIdx < 0x80)
		{
			pucIdx++;
		}
		else if (*pucIdx < 0xc2)
		{
			return;
		}
		// 2 byte
		else if (*pucIdx < 0xe0)
		{
			if ((*pucIdx == 0xD0 || *pucIdx == 0xD1) && *(pucIdx + 1) == 0)
			{
				*pucIdx = 0;
				return;
			}
			pucIdx += 2;
		}
		// 3 byte
		else if (*pucIdx < 0xf0)
		{
			pucIdx += 3;
		}
		else
			return;
	}
}

#endif

DI_LED_ID pal_panel_GetDiLedIndex(DxLedId_e eLedId)
{
	HUINT32		i,length;

	if (eLedId >= NUM_MAX_DxLED)
	{
		ERR_LOG("[%s:%d] Not Valid Value --  eLedId:0x%X !!!!!!!\n",
			__FUNCTION__,__LINE__,eLedId);
		return 0;
	}

	length = sizeof(stLedInfo) / sizeof(LedInfo_t);

	for (i=0; i < length; i++)
	{
		if (stLedInfo[i].eOctoLedId == eLedId)
		{
			INFO_LOG("[%s:%d] Found Match eLedId:0x%X -- stLedInfo[%d].eDiLedId:0x%X\n",
						__FUNCTION__,__LINE__,eLedId,i,stLedInfo[i].eDiLedId);
			stLedInfo[i].eDiLedId;
		}
	}

	ERR_LOG("[%s:%d] Can not Found Match eLedId:0x%X !!!!!!!\n",
				__FUNCTION__,__LINE__,eLedId);

	return 0;
}

DxLedId_e pal_panel_GetOctoLedIndex(DI_LED_ID eDiLedId)
{
	HUINT32	idx,length;

	length = sizeof(stLedInfo) / sizeof(LedInfo_t);

	for (idx = 0; idx < length ; idx++)
	{
		if (eDiLedId == stLedInfo[idx].eDiLedId)
		{
			INFO_LOG("[%s:%d] Found Match eDiLedId:0x%X -- stLedInfo[%d].eOctoLedId:0x%X\n",
						__FUNCTION__,__LINE__,eDiLedId,idx,stLedInfo[idx].eOctoLedId);
			return stLedInfo[idx].eOctoLedId;
		}
	}

	ERR_LOG("[%s:%d] Can not Found Match eDiLedId:0x%X !!!!!!!\n",
				__FUNCTION__,__LINE__,eDiLedId);

	return 0;
}

HERROR PAL_PANEL_GetMaxStringLen(HUINT32 *pulMaxLen)
{
	*pulMaxLen = PAL_PANEL_STRING_LEN;

	return ERR_OK;
}


HERROR PAL_PANEL_ConvertStringToPanelString(HUINT8 *pszStr, DxPanelAlign_e eAlign)
{
	HUINT32		ulCharCnt = 0;
	HUINT32		ulCharNum = 0;
	HUINT32		ulCharLenWithCode = 0;
	HUINT32		ulCharLenOnly = 0;
	HUINT32		ulDstStrPnt = 0;
	HUINT32		ulSourceLen = 0;
	HUINT8		*pTempSouceString = NULL;
	HUINT8		*pConvertString = NULL;
	HUINT8		szCharWithCode[8] = {0, };
	HUINT8		szCharOnly[8] = {0, };
	HERROR		hErr = ERR_FAIL;

	if (pszStr == NULL)
	{
		goto EXIT_PANEL_FUNC;
	}

	if (pszStr[0] == '\0')
	{
		goto EXIT_PANEL_FUNC;
	}

	ulSourceLen = MWC_UTIL_DvbStrlenStrOnly(pszStr);
	pConvertString = (HUINT8*)OxMW_Malloc(sizeof(HUINT8)*(ulSourceLen + 1));
	if(pConvertString == NULL)
	{
		goto EXIT_PANEL_FUNC;
	}
	VK_memset32(pConvertString, 0x00, sizeof(HUINT8)*(ulSourceLen + 1));

#ifdef CONFIG_MW_SYS_RUSSIAN_FRONT_PANEL
	pal_panel_SetRussianNullTermination(pszStr);
#endif

	pTempSouceString = (pszStr[0] == 0x15) ? pszStr + 1 : pszStr;
	ulCharNum = HLIB_CP_NumOfChars(eHxCP_UNICODE_UTF8, pTempSouceString);
	if(0 >= ulCharNum)
	{
		goto EXIT_PANEL_FUNC;
	}

	if (ulCharNum > ulSourceLen)
	{
		ulCharNum = ulSourceLen;
	}

	ulDstStrPnt = 0;

	for (ulCharCnt = 0; ulCharCnt < ulCharNum; ulCharCnt++)
	{
		ulCharLenOnly = 0;
		pConvertString[ulDstStrPnt] = '?';

		ulCharLenOnly = HLIB_CP_CharAt(eHxCP_UNICODE_UTF8, pTempSouceString, 0, szCharOnly);
		pTempSouceString += ulCharLenOnly;
		if (ulCharLenOnly >= 1)
		{
			// 0x00 ~ 0x7f 까지는 그대로 카피한다 : ASCII
			if (szCharOnly[0] <= 0x7f)
			{
				pConvertString[ulDstStrPnt++] = szCharOnly[0];
			}
			else if (szCharOnly[0] == 0xC3 && ulCharLenOnly >= 2)
			{
				if (szCharOnly[1] >= 0x80 && szCharOnly[1] <= 0x85)					// 'A' Related
				{
					pConvertString[ulDstStrPnt++] = 'A';
				}
				else if (szCharOnly[1] >= 0x88 && szCharOnly[1] <= 0x8B)			// 'E' Related
				{
					pConvertString[ulDstStrPnt++] = 'E';
				}
				else if (szCharOnly[1] >= 0x8C && szCharOnly[1] <= 0x8F)			// 'I' Related
				{
					pConvertString[ulDstStrPnt++] = 'I';
				}
				else if (szCharOnly[1] >= 0x92 && szCharOnly[1] <= 0x96)			// 'O' Related
				{
					pConvertString[ulDstStrPnt++] = 'O';
				}
				else if (szCharOnly[1] >= 0x99 && szCharOnly[1] <= 0x9C)			// 'U' Related
				{
					pConvertString[ulDstStrPnt++] = 'U';
				}
				else if (szCharOnly[1] >= 0xA0 && szCharOnly[1] <= 0xA5)			// 'a' Related
				{
					pConvertString[ulDstStrPnt++] = 'a';
				}
				else if (szCharOnly[1] >= 0xA8 && szCharOnly[1] <= 0xAB)			// 'e' Related
				{
					pConvertString[ulDstStrPnt++] = 'e';
				}
				else if (szCharOnly[1] >= 0xAC && szCharOnly[1] <= 0xAF)			// 'i' Related
				{
					pConvertString[ulDstStrPnt++] = 'i';
				}
				else if (szCharOnly[1] >= 0xB2 && szCharOnly[1] <= 0xB6)			// 'o' Related
				{
					pConvertString[ulDstStrPnt++] = 'o';
				}
				else if (szCharOnly[1] >= 0xB9 && szCharOnly[1] <= 0xBC)			// 'U' Related
				{
					pConvertString[ulDstStrPnt++] = 'u';
				}
			}
			else if (szCharOnly[0] == 0xC4 && ulCharLenOnly >= 2)
			{
				if (szCharOnly[1] >= 0x80 && szCharOnly[1] <= 0x85)					// 'A/a' Related
				{
					pConvertString[ulDstStrPnt++] = ((szCharOnly[1] & 0x01) == 0) ? 'A' : 'a';
				}
				else if (szCharOnly[1] >= 0x86 && szCharOnly[1] <= 0x8D)			// 'C'/c Related
				{
					pConvertString[ulDstStrPnt++] = ((szCharOnly[1] & 0x01) == 0) ? 'C' : 'c';
				}
				else if (szCharOnly[1] >= 0x8E && szCharOnly[1] <= 0x91)			// 'D/d' Related
				{
					pConvertString[ulDstStrPnt++] = ((szCharOnly[1] & 0x01) == 0) ? 'D' : 'd';
				}
				else if (szCharOnly[1] >= 0x92 && szCharOnly[1] <= 0x9b)			// 'E/e' Related
				{
					pConvertString[ulDstStrPnt++] = ((szCharOnly[1] & 0x01) == 0) ? 'E' : 'e';
				}
				else if (szCharOnly[1] >= 0x9C && szCharOnly[1] <= 0xA3)			// 'G/g' Related
				{
					pConvertString[ulDstStrPnt++] = ((szCharOnly[1] & 0x01) == 0) ? 'G' : 'g';
				}
				else if (szCharOnly[1] >= 0xA4 && szCharOnly[1] <= 0xA7)			// 'H/f' Related
				{
					pConvertString[ulDstStrPnt++] = ((szCharOnly[1] & 0x01) == 0) ? 'H' : 'h';
				}
				else if (szCharOnly[1] >= 0xA8 && szCharOnly[1] <= 0xB3)			// 'I/i' Related
				{
					pConvertString[ulDstStrPnt++] = ((szCharOnly[1] & 0x01) == 0) ? 'I' : 'i';
				}
				else if (szCharOnly[1] >= 0xB4 && szCharOnly[1] <= 0xB5)			// 'J/j' Related
				{
					pConvertString[ulDstStrPnt++] = ((szCharOnly[1] & 0x01) == 0) ? 'J' : 'j';
				}
				else if (szCharOnly[1] >= 0xB6 && szCharOnly[1] <= 0xB7)			// 'K/k' Related
				{
					pConvertString[ulDstStrPnt++] = ((szCharOnly[1] & 0x01) == 0) ? 'K' : 'k';
				}
				else if (szCharOnly[1] == 0xB8)										// 'k' Related
				{
					pConvertString[ulDstStrPnt++] = 'k';
				}
				else if (szCharOnly[1] >= 0xB9 && szCharOnly[1] <= 0xBF)			// 'L/l' Related
				{
					pConvertString[ulDstStrPnt++] = ((szCharOnly[1] & 0x01) == 1) ? 'L' : 'l';
				}
			}
			else if (szCharOnly[0] == 0xC5 && ulCharLenOnly >= 2)
			{
				if (szCharOnly[1] >= 0x80 && szCharOnly[1] <= 0x82)					// 'L/l' Related
				{
					pConvertString[ulDstStrPnt++] = ((szCharOnly[1] & 0x01) == 1) ? 'L' : 'l';
				}
				else if (szCharOnly[1] >= 0x83 && szCharOnly[1] <= 0x8B)			// 'N/n' Related
				{
					if (szCharOnly[1] >= 0x83 && szCharOnly[1] <= 0x88)
					{
						pConvertString[ulDstStrPnt++] = ((szCharOnly[1] & 0x01) == 1) ? 'N' : 'n';
					}
					else if (szCharOnly[1] == 0x89)
					{
						pConvertString[ulDstStrPnt++] = 'n';
					}
					else
					{
						pConvertString[ulDstStrPnt++] = ((szCharOnly[1] & 0x01) == 0) ? 'N' : 'n';
					}
				}
				else if (szCharOnly[1] >= 0x8C && szCharOnly[1] <= 0x93)			// 'O/o' Related
				{
					pConvertString[ulDstStrPnt++] = ((szCharOnly[1] & 0x01) == 0) ? 'O' : 'o';
				}
				else if (szCharOnly[1] >= 0x94 && szCharOnly[1] <= 0x99)			// 'R/r' Related
				{
					pConvertString[ulDstStrPnt++] = ((szCharOnly[1] & 0x01) == 0) ? 'R' : 'r';
				}
				else if (szCharOnly[1] >= 0x9A && szCharOnly[1] <= 0xA1)			// 'S/s' Related
				{
					pConvertString[ulDstStrPnt++] = ((szCharOnly[1] & 0x01) == 0) ? 'S' : 's';
				}
				else if (szCharOnly[1] >= 0xA2 && szCharOnly[1] <= 0xA7)			// 'T/t' Related
				{
					pConvertString[ulDstStrPnt++] = ((szCharOnly[1] & 0x01) == 0) ? 'S' : 's';
				}
				else if (szCharOnly[1] >= 0xA8 && szCharOnly[1] <= 0xB3)			// 'U/u' Related
				{
					pConvertString[ulDstStrPnt++] = ((szCharOnly[1] & 0x01) == 0) ? 'U' : 'u';
				}
				else if (szCharOnly[1] >= 0xB4 && szCharOnly[1] <= 0xB5)			// 'W/w' Related
				{
					pConvertString[ulDstStrPnt++] = ((szCharOnly[1] & 0x01) == 0) ? 'W' : 'w';
				}
				else if (szCharOnly[1] >= 0xB6 && szCharOnly[1] <= 0xB8)			// 'Y/y' Related
				{
					pConvertString[ulDstStrPnt++] = ((szCharOnly[1] & 0x01) == 0) ? 'Y' : 'y';
				}
				else if (szCharOnly[1] >= 0xB9 && szCharOnly[1] <= 0xBE)			// 'Z/z' Related
				{
					pConvertString[ulDstStrPnt++] = ((szCharOnly[1] & 0x01) == 1) ? 'Z' : 'z';
				}
			}
#ifdef CONFIG_MW_SYS_RUSSIAN_FRONT_PANEL
			else if (szCharOnly[0] == 0xD0 && ulCharLenOnly >= 2)
			{
				pConvertString[ulDstStrPnt++] = pal_panel_ConvertCharToRussianFront(szCharOnly[1] + 0x20);
			}
			else if (szCharOnly[0] == 0xD1 && ulCharLenOnly >= 2)
			{
				pConvertString[ulDstStrPnt++] = pal_panel_ConvertCharToRussianFront(szCharOnly[1] + 0x60);
			}
#endif

		}
#ifdef CONFIG_MW_SYS_RUSSIAN_FRONT_PANEL
// 0x80번대의 특수 문자 처리 (UTF-8에서는 에러가 리턴됨)
		else
		{
			if((szCharOnly[0] >= 0x8B && szCharOnly[0] <= 0x8E) || szCharOnly[0] == 0x95)
			{
				pConvertString[ulDstStrPnt++] = szCharOnly[0];
			}
		}
#endif

		if (pConvertString[ulDstStrPnt] == '?')
		{
			ulDstStrPnt ++;
		}
	}

	pConvertString[ulDstStrPnt] = '\0';
	MWC_UTIL_DvbStrcpy (pszStr, pConvertString);

	hErr = pal_panel_setAlign(pszStr, eAlign);
	if (hErr != ERR_OK)
	{
		goto EXIT_PANEL_FUNC;
	}

	hErr = ERR_OK;

EXIT_PANEL_FUNC :

	if(pConvertString)
	{
		OxMW_Free(pConvertString);
	}

	return ERR_OK;
}


static HERROR		pal_panel_setAlign(HUINT8 *pString, DxPanelAlign_e eAlign)
{
	HERROR		hError = ERR_FAIL;
	HUINT8		szConvertString[PAL_PANEL_STRING_LEN + 1] = {0, };
	HUINT32		ulLen = 0;
	HUINT32		ulStartPos = 0;

	if(pString == NULL)
	{
		goto EXIT_PANEL_FUNC;
	}

	ulLen = MWC_UTIL_DvbStrlen(pString);
	if(ulLen > PAL_PANEL_STRING_LEN)
	{
		hError = ERR_OK;
		goto EXIT_PANEL_FUNC;
	}

	switch(eAlign)
	{
		case eDxPANEL_ALIGN_LEFT :
			break;

		case eDxPANEL_ALIGN_CENTER :
			HxSTD_memset(szConvertString, ' ', sizeof(HUINT8)*(PAL_PANEL_STRING_LEN + 1));
			ulStartPos = (PAL_PANEL_STRING_LEN - ulLen)/2;

			HLIB_STD_StrUtf8NCpy(&szConvertString[ulStartPos], pString, PAL_PANEL_STRING_LEN);

			HLIB_STD_StrUtf8NCpy(pString, szConvertString, PAL_PANEL_STRING_LEN);
			break;

		case eDxPANEL_ALIGN_RIGHT :
			HxSTD_memset(szConvertString, ' ', sizeof(HUINT8)*(PAL_PANEL_STRING_LEN + 1));
			ulStartPos = (PAL_PANEL_STRING_LEN - ulLen);

			HLIB_STD_StrUtf8NCpy(&szConvertString[ulStartPos], pString, PAL_PANEL_STRING_LEN);

			HLIB_STD_StrUtf8NCpy(pString, szConvertString, PAL_PANEL_STRING_LEN);
			break;

		default :
			break;
	}

	hError = ERR_OK;

EXIT_PANEL_FUNC :

	return hError;
}

/* end of file */
