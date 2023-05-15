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
#define	P7SEGENTER			HxLOG_Info("(+)Enter!!\n");
#define	P7SEGLEAVE(X)		HxLOG_Info("(-)Leave -- (0x%X)\n",X);
#else
#define	LOG					OCTO_NULL_PRINT
#define INFO_LOG			OCTO_NULL_PRINT
#define	ERR_LOG				OCTO_NULL_PRINT
#define MSG_LOG				OCTO_NULL_PRINT
#define	P7SEGENTER			OCTO_NULL_PRINT
#define	P7SEGLEAVE(X)		OCTO_NULL_PRINT
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
	{  eDxLED_ID_TV,				 0}
	,{  eDxLED_ID_RADIO,			 0}
	,{  eDxLED_ID_REC,			 DI_LED_NUM4}
	,{  eDxLED_ID_PLAY,			 0}
	,{  eDxLED_ID_PAUSE,			 0}
	,{  eDxLED_ID_RESERVE,		 DI_LED_NUM5}

	// 0 means not exist.
	,{  eDxLED_ID_SRS,			 0}
	,{  eDxLED_ID_SRC_TS,		 0}
	,{  eDxLED_ID_SRC_XT,		 0}

	,{  eDxLED_ID_DOLBY,			 0}
	,{  eDxLED_ID_HD,			 0}
	,{  eDxLED_ID_1080P,			 0}
	,{  eDxLED_ID_720P,			 0}
	,{  eDxLED_ID_576P,			 0}
	,{  eDxLED_ID_480P,			 0}
	,{  eDxLED_ID_1080I,			 0}
	,{  eDxLED_ID_576I,			 0}
	,{  eDxLED_ID_480I,			 0}

	,{  eDxLED_ID_DOLBY_PLUS,	 0}
	,{  eDxLED_ID_DOLBY_PULSE,	 0}

	,{  eDxLED_ID_WIFI,			 0}
	,{  eDxLED_ID_USB,			 0}

	,{  eDxLED_ID_HDD_25_FULL,	 0}
	,{  eDxLED_ID_HDD_50_FULL,	 0}
	,{  eDxLED_ID_HDD_75_FULL,	 0}
	,{  eDxLED_ID_HDD_100_FULL,	 0}

	,{  eDxLED_ID_LINK,			 0}

	,{  eDxLED_ID_DATA,			 DI_LED_NUM6}

	,{  eDxLED_ID_MAIL,			 DI_LED_NUM3}
	,{  eDxLED_ID_RED_COLOR,		 DI_LED_NUM0}
	,{  eDxLED_ID_BLUE_COLOR,	 DI_LED_NUM1}
	,{  eDxLED_ID_ORANGE_COLOR,	 (DI_LED_NUM0 | DI_LED_NUM1)}
};

/*******************************************************************/
/******************* Static Function Prototypes  *************************/
/*******************************************************************/
static HERROR		pal_panel_setAlign(HUINT8 *pString, DxPanelAlign_e eAlign);
/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

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
			return stLedInfo[i].eDiLedId;
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
//	HUINT32		ulCharLenWithCode = 0;
	HUINT32		ulCharLenOnly = 0;
	HUINT32		ulDstStrPnt = 0;
	HUINT32		ulSourceLen = 0;
	HUINT8		*pTempSouceString = NULL;
	HUINT8		*pConvertString = NULL;
//	HUINT8		szCharWithCode[8] = {0, };
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
