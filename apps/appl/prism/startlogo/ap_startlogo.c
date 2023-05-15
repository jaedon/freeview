/**************************************************************
*	@file		ap_startlogo.c
*	Minimized Graphic Library for DirectFB
*
*	http://www.humaxdigital.com
*
*	@data			2012/09/17
*	@author			humax
*	@brief			Minimized Graphic Library for DirectFB
**************************************************************/


/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding 을 사용하는 source file 입니다.
* MS949 encodingを使用して source fileです。
* Quelldatei, die MS949-Codierung verwendet.
*
*/


/*
* ⓒ 2011-2012 Humax Co., Ltd.
* This program is produced by Humax Co., Ltd. ("Humax") and
* the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
* non-assignable, non-transferable and non-exclusive license to use this Software.
* You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
* you agree to the responsibility to take all reasonable efforts to protect the any information
* you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
* reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
* If you have no authorized license, discontinue using this Software immediately.
*
* THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS,
* IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
* IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
*  CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.
*
* This License is effective until terminated. You may terminate this License at any time by destroying all copies
* of the Software including all documentation. This License will terminate immediately without notice from Humax
* to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
* of the Software and all documentation.
*
* The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
* All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
* in the Republic of Korea.
*/

/**************************************************************************************************/
#define _________START_LOGO_Private_Include_________________________________________________
/**************************************************************************************************/
#include <prism.h>
#include "ap_startlogo.h"

/**************************************************************************************************/
#define _________START_LOGO_Private_Macro_________________________________________________
/**************************************************************************************************/

//eMctrlEvent_LIVE_EmptySvc가 삭제되고 view state에 통합됨에 따라 view state를 읽어서 empty svc를 처리하도록 수정한 define
#define TEST_NEW_EMPTY_SVC_MSG

#if defined(CONFIG_PRISM_ZAPPING)
#define STARTLOGO_TIME_SECOND				500
#define STARTLOGO_SYSMSG_CHECK				1000
#else
#define STARTLOGO_TIME_SECOND				500
#endif


#if defined(STARTLOGO_LOW_QUALITY)
#define STARTLOGO_LOOPCNT					40
#else
#define STARTLOGO_LOOPCNT					10				//	10회 출력, WebApp이 Ready를 주면 자동으로 없어진다.
#endif
/**************************************************************************************************/
#define _________START_LOGO_Private_Struct_________________________________________________
/**************************************************************************************************/
typedef struct
{
	HBOOL				bAVStarted;
	ONDKVideoHandle 	hVideoHandle;
	HUINT32				ulDrawCount;
	HINT32				nCurDrawStrNum;
#if defined(CONFIG_PRISM_ZAPPING)
	HUINT8				*pucLoadingText;
	HUINT32				ulCurViewState;
	HUINT32				ulNewViewState;
	HUINT8				**ppSysMsgForLine;
	HUINT8				*pSysMsg;
	HINT32				nSysMsgLineNum;
	HxVector_t			*pServiceList;
	HINT32				nCurSvcUid;
	HBOOL				bLoadedServiceList;

#endif
} startlogo_Contents_t;


#if defined(CONFIG_PRISM_ZAPPING)
typedef enum
{
	eServiceGroupType_ALL		= 0x0000,
	eServiceGroupType_TV		= 0x0001,
	eServiceGroupType_Radio		= 0x0002,
	eServiceGroupType_HDTV		= 0x0003,
	eServiceGroupType_PayTV		= 0x0004,
	eServiceGroupType_FreeTV	= 0x0005,

	eServiceGroupType_AlphabetAll	= 0x1000,
	eServiceGroupType_AlphabetA	= 0x1041,
	eServiceGroupType_AlphabetB	= 0x1042,
	eServiceGroupType_AlphabetC	= 0x1043,
	eServiceGroupType_AlphabetD	= 0x1044,
	eServiceGroupType_AlphabetE	= 0x1045,
	eServiceGroupType_AlphabetF	= 0x1046,
	eServiceGroupType_AlphabetG	= 0x1047,
	eServiceGroupType_AlphabetH	= 0x1048,
	eServiceGroupType_AlphabetI	= 0x1049,
	eServiceGroupType_AlphabetJ	= 0x104A,
	eServiceGroupType_AlphabetK	= 0x104B,
	eServiceGroupType_AlphabetL	= 0x104C,
	eServiceGroupType_AlphabetM	= 0x104D,
	eServiceGroupType_AlphabetN	= 0x104E,
	eServiceGroupType_AlphabetO	= 0x104F,
	eServiceGroupType_AlphabetP	= 0x1050,
	eServiceGroupType_AlphabetQ 	= 0x1051,
	eServiceGroupType_AlphabetR 	= 0x1052,
	eServiceGroupType_AlphabetS 	= 0x1053,
	eServiceGroupType_AlphabetT 	= 0x1054,
	eServiceGroupType_AlphabetU 	= 0x1055,
	eServiceGroupType_AlphabetV 	= 0x1056,
	eServiceGroupType_AlphabetW 	= 0x1057,
	eServiceGroupType_AlphabetX 	= 0x1058,
	eServiceGroupType_AlphabetY 	= 0x1059,
	eServiceGroupType_AlphabetZ 	= 0x105A,
	eServiceGroupType_AlphabetDigit	= 0x105B,
	eServiceGroupType_AlphabetETC	= 0x105C,

	eServiceGroupType_Favorite1 	= 0x2000,
	eServiceGroupType_Favorite2 	= 0x2001,
	eServiceGroupType_Favorite3 	= 0x2002,
	eServiceGroupType_Favorite4 	= 0x2003,
	eServiceGroupType_Favorite5 	= 0x2004
} LastServiceGroup_e;

#endif

/**************************************************************************************************/
#define _________START_LOGO_Private_Value_________________________________________________
/**************************************************************************************************/

static startlogo_Contents_t	s_stStartLogoContent;

ONDKVideoHandle 	hVideoHandle = NULL;

/**************************************************************************************************/
#define _________START_LOGO_Private_Prototype_________________________________________________
/**************************************************************************************************/


//static startlogo_Contents_t*	 local_startlogo_GetContents(void);

static ONDK_Result_t	local_startlogo_MsgGwmCreate(HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	local_startlogo_MsgGwmDestroy(HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	local_startlogo_SetObject(startlogo_Contents_t *pstContent);
static ONDK_Result_t 	local_startlogo_MsgGwmKeyDown(HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	local_startlogo_MsgGwmTimer(HINT32 p1, HINT32 p2, HINT32 p3);

/**************************************************************************************************/
#define _________START_LOGO_Private_functions__________________________________________________________
/**************************************************************************************************/

static startlogo_Contents_t* local_startlogo_GetContents(void)
{
	return &s_stStartLogoContent;
}

static void local_startlogo_initContents(startlogo_Contents_t *pstContent)
{
	if(pstContent)
	{
		pstContent->bAVStarted = FALSE;
		pstContent->hVideoHandle = NULL;
		pstContent->ulDrawCount = 0;
#if defined(CONFIG_PRISM_ZAPPING)
		pstContent->nCurDrawStrNum = 0;
		pstContent->pucLoadingText = NULL;
		pstContent->ppSysMsgForLine = NULL;
		pstContent->pSysMsg = NULL;
		pstContent->nSysMsgLineNum = 0;
		pstContent->ulCurViewState = eOxMP_LiveViewState_NONE;
		pstContent->ulNewViewState = eOxMP_LiveViewState_NONE;
		pstContent->pServiceList = NULL;
		pstContent->bLoadedServiceList = FALSE;
#endif
	}
}

#if defined(CONFIG_PRISM_ZAPPING)
static HCHAR *local_startlogo_ConvertIndexToURL(HUINT32 ulImgUrl)
{
	switch(ulImgUrl)
	{
		case STARTLOGO_LOADING_IMG_1:	return "/usr/image/206_Loading_DW_02.png";
		case STARTLOGO_LOADING_IMG_2:	return "/usr/image/206_Loading_DW_03.png";
		case STARTLOGO_LOADING_IMG_3:	return "/usr/image/206_Loading_DW_04.png";
		case STARTLOGO_LOADING_IMG_4:	return "/usr/image/206_Loading_DW_05.png";
		case STARTLOGO_LOADING_IMG_5:	return "/usr/image/206_Loading_DW_06.png";
		case STARTLOGO_LOADING_IMG_6:	return "/usr/image/206_Loading_DW_07.png";
		case STARTLOGO_LOADING_IMG_7:	return "/usr/image/206_Loading_DW_08.png";
		case STARTLOGO_LOADING_IMG_8:	return "/usr/image/206_Loading_DW_09.png";
		case STARTLOGO_LOADING_IMG_9:	return "/usr/image/206_Loading_DW_10.png";
		case STARTLOGO_LOADING_IMG_10:	return "/usr/image/206_Loading_DW_11.png";
		case STARTLOGO_LOADING_IMG_11:	return "/usr/image/206_Loading_DW_12.png";
		case STARTLOGO_LOADING_IMG_12:	return "/usr/image/206_Loading_DW_13.png";
		case STARTLOGO_LOADING_IMG_13:	return "/usr/image/206_Loading_DW_14.png";
		case STARTLOGO_LOADING_IMG_14:	return "/usr/image/206_Loading_DW_15.png";
		case STARTLOGO_LOADING_IMG_15:	return "/usr/image/206_Loading_DW_16.png";
		default: 						return NULL;
	}
}
#endif


#if defined(CONFIG_DEBUG)
static void local_startlogo_HapiMsgConvertToString(HAPIMSG_e eHapiMsg)
{
	switch(eHapiMsg)
	{
		case eHMSG_APP_READY:
			HxLOG_Debug("\teHMSG_APP_READY\n");
			break;
		case eHMSG_BOOTUPSTATE_CHANGE:
			HxLOG_Debug("\teHMSG_BOOTUPSTATE_CHANGE\n");
			break;
		case eHMSG_OSK_SHOW:
			HxLOG_Debug("\teHMSG_OSK_SHOW\n");
			break;
		case eHMSG_OSK_DESTROY:
			HxLOG_Debug("\teHMSG_OSK_DESTROY\n");
			break;
		case eHMSG_OSK_MODECHANGE:
			HxLOG_Debug("\teHMSG_OSK_MODECHANGE\n");
			break;
		case eHMSG_OSK_SHOW_SIMPLE:
			HxLOG_Debug("\teHMSG_OSK_SHOW_SIMPLE\n");
			break;
		case eHMSG_REMOTEAPP_STRING:
			HxLOG_Debug("\teHMSG_REMOTEAPP_STRING\n");
			break;
		case eHMSG_SYSSTATE_CHANGE:
			HxLOG_Debug("\teHMSG_SYSSTATE_CHANGE\n");
			break;
		case eHMSG_SUBTITLE_STATE:
			HxLOG_Debug("\teHMSG_SUBTITLE_STATE\n");
			break;
		case eHMSG_TELETEXT_STATE:
			HxLOG_Debug("\teHMSG_TELETEXT_STATE\n");
			break;
		case eHMSG_SUBTITLE_TYPE_CHANGE:
			HxLOG_Debug("\teHMSG_SUBTITLE_TYPE_CHANGE\n");
			break;
		case eHMSG_SUBTITLE_FONTTYPE_CHANGE:
			HxLOG_Debug("\teHMSG_SUBTITLE_FONTTYPE_CHANGE\n");
			break;
		case eHMSG_MAINAPP_READY:
			HxLOG_Debug("\teHMSG_MAINAPP_READY\n");
			break;
		case eHMSG_MAINLANG_CHANGED:
			HxLOG_Debug("\teHMSG_MAINLANG_CHANGED\n");
			break;
		case eHMSG_INPUT_NOTIFY:
			HxLOG_Debug("\teHMSG_INPUT_NOTIFY\n");
			break;
		case eHMSG_PANEL_PRIORITY:
			HxLOG_Debug("\teHMSG_PANEL_PRIORITY\n");
			break;
		case eHMSG_IS_OTA_CHECK:
			HxLOG_Debug("\teHMSG_IS_OTA_CHECK\n");
			break;
		case eHMSG_MSGBOX_SHOW:
			HxLOG_Debug("\teHMSG_MSGBOX_SHOW\n");
			break;
		default:
			HxLOG_Debug("\tUnknown Message.\n");
			break;
	}
}
#endif

static void local_startlogo_SetVFDString(HCHAR * panelString)
{
//	APK_OUTPUT_PANEL_SetPanelDimLevel(eApkPANEL_PRIORITY_MIDDLE, 100);
	APK_OUTPUT_PANEL_SetPanel(eApkPANEL_PRIORITY_MIDDLE, panelString, eDxPANEL_ALIGN_CENTER);
}


static void local_startlogo_ClearVFDString(void)
{
	APK_OUTPUT_PANEL_SetPanelAuto(eApkPANEL_PRIORITY_MIDDLE);
}

#if defined(CONFIG_PRISM_ZAPPING)
static void local_startlogo_DrawSystemMessage(startlogo_Contents_t *pstContent, OxMediaPlay_LiveViewState_e eViewState)
{
	ONDK_Rect_t 		stRect;
	HUINT8	*cpy_str = NULL;
	HUINT8	*tok_str = NULL;
	HCHAR	*save = NULL;
	HINT32	nStrlen		= 0;
	HINT32	n1LineLimit	= 45;
	HINT32	nLineGap	= 0;
	HINT32	nTmpLen		= 0;
	HINT32	nLineUpY	= 0;
	HINT32	nLineDownY	= 0;
	HINT32	nTextY		= 0;
	HINT32	nImageY		= 0;
	HINT32	nLoopCount	= 0;

	if(pstContent==NULL)
	{
		HxLOG_Error("Content is Null.\n");
		return;
	}

	switch(eViewState)
	{
		case eOxMP_LiveViewState_AV_UNDER_RUN:
			// The channel is scrambled or not available.
			HxLOG_Debug("#############################\n");
			HxLOG_Debug("AV_UNDER_RUN\n");
			HxLOG_Debug("#############################\n");
			cpy_str = ONDK_GetString("STR_MESG_012_ID");
			break;
		case eOxMP_LiveViewState_NO_SIGNAL:
			// The receiver is not receiving a signal or the signal is too weak.
			HxLOG_Debug("#############################\n");
			HxLOG_Debug("NO_SIGNAL\n");
			HxLOG_Debug("#############################\n");
			cpy_str = ONDK_GetString("STR_MESG_1014_ID");
			break;
		case eOxMP_LiveViewState_HDMI_BLOCKED:
			/* The programme cannot be viewed via HDMI connection because your TV does not support digital content protection(HDCP).
				Disconnect the HDMI cable and connect composite video connector to watch the programme in standard resolution. */
			HxLOG_Debug("#############################\n");
			HxLOG_Debug("HDMI_BLOCKED\n");
			HxLOG_Debug("#############################\n");
			cpy_str = ONDK_GetString("STR_MESG_3191_ID");
			break;
		case eOxMP_LiveViewState_NO_VIDEO:
			// The video is not available.
			HxLOG_Debug("#############################\n");
			HxLOG_Debug("NO_VIDEO\n");
			HxLOG_Debug("#############################\n");
			cpy_str = ONDK_GetString("STR_MESG_117_ID");
			break;
		case eOxMP_LiveViewState_NO_AV:
			// AV is not available.
			HxLOG_Debug("#############################\n");
			HxLOG_Debug("NO_AV\n");
			HxLOG_Debug("#############################\n");
			cpy_str = ONDK_GetString("STR_MESG_076_ID");
			break;
		default:
			HxLOG_Debug("Need not display System Message. (%d)\n", eViewState);
			return;
	}

	if(cpy_str == NULL)
	{
		HxLOG_Error("No String.\n");
		return;
	}

	{
		HINT32	i=0;

		if(pstContent->ppSysMsgForLine)
		{
			for(i=0;i<pstContent->nSysMsgLineNum;i++)
			{
				if(pstContent->ppSysMsgForLine[i])
				{
					HxSTR_MemFree(pstContent->ppSysMsgForLine[i]);
				}
			}
			HxSTR_MemFree(pstContent->ppSysMsgForLine);
		}
	}

	if(pstContent->pSysMsg == NULL)
	{
		// pstContent->pSysMsg Destroy할 때 Free 해주도록 한다.
		pstContent->pSysMsg = AP_Malloc(512);
		if(pstContent->pSysMsg == NULL)
		{
			HxLOG_Error("System Message Buffer Allocate Fail.\n");
			return;
		}
	}

	HxSTR_Memset(pstContent->pSysMsg,0, 512);

	HxSTD_StrNCpy(pstContent->pSysMsg, cpy_str, HxSTD_StrLen(cpy_str));
	pstContent->pSysMsg[HxSTD_StrLen(cpy_str)] = '\0';

	HxLOG_Debug("#############################\n");
	HxLOG_Debug("1: %s\n", cpy_str);
	HxLOG_Debug("2: %s\n", pstContent->pSysMsg);
	HxLOG_Debug("#############################\n");

	nStrlen = HxSTD_StrLen(pstContent->pSysMsg);
	pstContent->nSysMsgLineNum = nStrlen /n1LineLimit;

	if(nStrlen%n1LineLimit != 0)
	{
		pstContent->nSysMsgLineNum++;
	}

	pstContent->ppSysMsgForLine = HxSTR_MemAlloc(pstContent->nSysMsgLineNum*sizeof(HUINT8*));

	tok_str = HxSTD_StrTok(pstContent->pSysMsg," ", &save);
	if(pstContent->nSysMsgLineNum == 4)
		nLineGap = 20;
	else
		nLineGap = 0;

	if(pstContent->nSysMsgLineNum <= 0)
	{
		HxLOG_Error("There is no Message to display.\n");
		if(pstContent->pSysMsg)
		{
			HxSTR_MemFree(pstContent->pSysMsg);
		}
		return;
	}
	else
	{
		nLineUpY	= SYSTEM_MSG_YELLOWLINE_UP_Y - (pstContent->nSysMsgLineNum - 1) * (SYSTEM_MSG_TEXT_H/2);
		nLineDownY	= SYSTEM_MSG_YELLOWLINE_DOWN_Y + (pstContent->nSysMsgLineNum - 1) * (SYSTEM_MSG_TEXT_H/2);
		nImageY 	= SYSTEM_MSG_ICON_Y;
		nTextY		= SYSTEM_MSG_TEXT_Y - (pstContent->nSysMsgLineNum - 1) * (SYSTEM_MSG_TEXT_H/2);
	}

	for(nLoopCount=0;nLoopCount< pstContent->nSysMsgLineNum;nLoopCount++)
	{
		pstContent->ppSysMsgForLine[nLoopCount] = HxSTR_MemAlloc(sizeof(HUINT8)*512);
		HxSTR_Memset(pstContent->ppSysMsgForLine[nLoopCount],0,sizeof(HUINT8)*512);

		while(1)
		{
			if(tok_str == NULL)
				break;
			HxSTD_StrNCat(pstContent->ppSysMsgForLine[nLoopCount],tok_str,HxSTD_StrLen(tok_str));
			nTmpLen = HxSTD_StrLen(pstContent->ppSysMsgForLine[nLoopCount]);
			pstContent->ppSysMsgForLine[nLoopCount][nTmpLen] = '\0';
			if(nTmpLen<n1LineLimit)
			{
				HxSTD_StrNCat(pstContent->ppSysMsgForLine[nLoopCount]," ",1);
				pstContent->ppSysMsgForLine[nLoopCount][nTmpLen+1] = '\0';
			}
			else
			{
				tok_str = HxSTD_StrTok(NULL," ",&save);
				break;
			}

			tok_str = HxSTD_StrTok(NULL," ",&save);
			if(tok_str == NULL)
				break;
		}

		stRect = ONDK_Rect(SYSTEM_MSG_TEXT_X,nTextY+SYSTEM_MSG_TEXT_H*nLoopCount+nLineGap,SYSTEM_MSG_TEXT_W,SYSTEM_MSG_TEXT_H);
		ONDK_GWM_COM_Text_Create(STARTLOGO_SYSMSG_ID+nLoopCount,&stRect,pstContent->ppSysMsgForLine[nLoopCount]);
		ONDK_GWM_COM_Text_SetSize(STARTLOGO_SYSMSG_ID+nLoopCount, SYSTEM_MSG_FONT_SIZE);
		ONDK_GWM_COM_Text_SetFont(STARTLOGO_SYSMSG_ID+nLoopCount, eFont_SystemBold);
		ONDK_GWM_COM_Text_ChangeColor(STARTLOGO_SYSMSG_ID+nLoopCount, SYSTEM_MSG_FONT_COLOR, SYSTEM_MSG_FONT_COLOR);
	}

	stRect = ONDK_Rect(SYSTEM_MSG_YELLOWLINE_UP_X, nLineUpY, SYSTEM_MSG_YELLOWLINE_UP_W, SYSTEM_MSG_YELLOWLINE_UP_H);
	ONDK_GWM_COM_Image_Create(STARTLOGO_SYSMSG_YELLOWLINE_UP_ID, &stRect);
	ONDK_GWM_COM_Image_SetImage(STARTLOGO_SYSMSG_YELLOWLINE_UP_ID, SYSTEM_MSG_YELLOWLINE);

	stRect = ONDK_Rect(SYSTEM_MSG_ICON_X, nImageY, SYSTEM_MSG_ICON_W, SYSTEM_MSG_ICON_H);
	ONDK_GWM_COM_Image_Create(STARTLOGO_SYSMSG_ICON_ID, &stRect);
	ONDK_GWM_COM_Image_SetImage(STARTLOGO_SYSMSG_ICON_ID, SYSTEM_MSG_IMG_ICON);

	stRect = ONDK_Rect(SYSTEM_MSG_YELLOWLINE_DOWN_X, nLineDownY, SYSTEM_MSG_YELLOWLINE_DOWN_W, SYSTEM_MSG_YELLOWLINE_DOWN_H);
	ONDK_GWM_COM_Image_Create(STARTLOGO_SYSMSG_YELLOWLINE_DOWN_ID, &stRect);
	ONDK_GWM_COM_Image_SetImage(STARTLOGO_SYSMSG_YELLOWLINE_DOWN_ID, SYSTEM_MSG_YELLOWLINE);

	ONDK_GWM_APP_InvalidateAPP(ONDK_GWM_APP_GetThis());
}

#endif





static void local_startlogo_PapiStatusConvertToString(PxSTATUS_e	ePapiStatus)
{
	switch(ePapiStatus)
	{
		case ePAMA_STATUS_Operation:
			HxLOG_Debug("\t ePAMA_STATUS_Operation\n");
			break;
		case ePAMA_STATUS_Reboot:
			HxLOG_Debug("\t ePAMA_STATUS_Reboot\n");
			break;
		case ePAMA_STATUS_Shutdown:
			HxLOG_Debug("\t ePAMA_STATUS_Shutdown\n");
			break;
		case ePAMA_STATUS_RebootDone:
			HxLOG_Debug("\t ePAMA_STATUS_RebootDone\n");
			break;
		case ePAMA_STATUS_ShutdownDone:
			HxLOG_Debug("\t ePAMA_STATUS_ShutdownDone\n");
			break;
		default:
			HxLOG_Debug("\tUnknown Status.\n");
			break;
	}
}



static ONDK_Result_t local_startlogo_MsgGwmDestroy(HINT32 p1, HINT32 p2, HINT32 p3)
{
	startlogo_Contents_t	*pstContent = NULL;

	HxLOG_Trace();

	pstContent = local_startlogo_GetContents();
	if(pstContent == NULL)
	{
		HxLOG_Error("pstContent is Null.\n");
		return GWM_RET_OK;
	}

	local_startlogo_ClearVFDString();
	ONDK_GWM_KillTimer(GWMPOPUP_TIMER_STARTLOGOID);
#if defined(CONFIG_PRISM_ZAPPING)
	ONDK_GWM_KillTimer(GWMPOPUP_TIMER_STARTLOGO_SYSMSG);

	ONDK_GWM_KillTimer(GWMPOPUP_TIMER_STARTLOGO_LOADING);
	if(pstContent->pucLoadingText)
	{
		AP_Free(pstContent->pucLoadingText);
	}

	{
		HINT32	i=0;

		if(pstContent->ppSysMsgForLine)
		{
			for(i=0;i<pstContent->nSysMsgLineNum;i++)
			{
				if(pstContent->ppSysMsgForLine[i])
				{
					HxSTR_MemFree(pstContent->ppSysMsgForLine[i]);
				}
			}
			HxSTR_MemFree(pstContent->ppSysMsgForLine);
		}
	}

	if(pstContent->pSysMsg)
	{
		AP_Free(pstContent->pSysMsg);
	}
#endif
	if(pstContent->hVideoHandle)
	{
		ONDK_VIDEO_Release(pstContent->hVideoHandle);
	}

	pstContent->hVideoHandle = NULL;
	ONDK_WINDOW_Clear(ONDK_GWM_GetWindow(ONDK_UPPER_WINDOW));
	ONDK_WINDOW_Update(ONDK_GWM_GetWindow(ONDK_UPPER_WINDOW),NULL);

#if defined(CONFIG_PRISM_ZAPPING)
	if(pstContent->pServiceList)
	{
		CLIB_RemoveItemList(pstContent->pServiceList);
		pstContent->pServiceList = NULL;
	}
#endif

	return GWM_RET_OK;
}


static void local_startlogo_updatewindow(void)
{
#if !defined(CONFIG_PRISM_ZAPPING)
	startlogo_Contents_t	*pstContent = NULL;

	pstContent = local_startlogo_GetContents();
	if(pstContent && pstContent->hVideoHandle)
	{
		if(pstContent->ulDrawCount++ == 0)
		{
			ONDK_DRAW_FillRect(ONDK_GWM_GetSurface(ONDK_UPPER_WINDOW),
					ONDK_Rect(0, 0, GWM_SCREEN_WIDTH, GWM_SCREEN_HEIGHT),
					ONDK_Color(0xFF, 0, 0, 0));
		}
	}

#endif
	ONDK_WINDOW_Update(ONDK_GWM_GetWindow(ONDK_UPPER_WINDOW),NULL);
}

static ONDK_Result_t 	local_startlogo_SetObject(startlogo_Contents_t *pstContent)
{
	ONDK_Rect_t 		stRect;
	ONDK_Result_t		lRet = ONDK_RET_OK;

	if(pstContent)
	{
#if defined(CONFIG_PRISM_ZAPPING)
		ONDK_GWM_APP_SetAppArea(STARTLOGO_X, STARTLOGO_Y, STARTLOGO_W, STARTLOGO_H);
		stRect = ONDK_Rect(STARTLOGO_IMG_X,STARTLOGO_IMG_Y,STARTLOGO_IMG_W,STARTLOGO_IMG_H);

		if(pstContent->bAVStarted == FALSE)
		{
			pstContent->hVideoHandle = ONDK_VIDEO_Create(STARTLOGO_IMG);
			if(pstContent->hVideoHandle)
			{
				lRet = ONDK_VIDEO_PlayOnece(pstContent->hVideoHandle, ONDK_GWM_GetSurface(ONDK_UPPER_WINDOW),&stRect,(void *)local_startlogo_updatewindow,NULL);
				ONDK_GWM_SetTimer(GWMPOPUP_TIMER_STARTLOGOID, STARTLOGO_TIME_SECOND);
			}
		}
#else
		ONDK_GWM_APP_SetAppArea(STARTLOGO_X, STARTLOGO_Y, STARTLOGO_W, STARTLOGO_H);
		stRect = ONDK_Rect(STARTLOGO_X,STARTLOGO_Y,STARTLOGO_W,STARTLOGO_H);


		pstContent->hVideoHandle = ONDK_VIDEO_Create(STARTLOGO_IMG);
		if(pstContent->hVideoHandle)
		{
			lRet = ONDK_VIDEO_PlayOnece(pstContent->hVideoHandle,ONDK_GWM_GetSurface(ONDK_UPPER_WINDOW),&stRect,(void *)local_startlogo_updatewindow,NULL);
			ONDK_GWM_SetTimer(GWMPOPUP_TIMER_STARTLOGOID, STARTLOGO_TIME_SECOND);
		}
#endif

	}
	return lRet;
}


//MSG_GWM_TIMER
static ONDK_Result_t local_startlogo_MsgGwmTimer(HINT32 p1, HINT32 p2, HINT32 p3)
{
	static HUINT32		loopcnt = 0;
	ONDK_Rect_t			stRect;
	startlogo_Contents_t	*pstContent = NULL;

	pstContent = local_startlogo_GetContents();
	if(pstContent == NULL)
	{
		HxLOG_Error("pstContent is Null.\n");
		return GWM_MESSAGE_PASS;
	}

	if(p1 == GWMPOPUP_TIMER_STARTLOGOID)
	{
		if(pstContent->hVideoHandle)
		{
			// 약 2초에 한번 해제됨...
			if(ONDK_VIDEO_IsBusy(pstContent->hVideoHandle)== FALSE)
			{
				ONDK_VIDEO_Release(pstContent->hVideoHandle);
				pstContent->hVideoHandle = NULL;

				if(++loopcnt > STARTLOGO_LOOPCNT)
				{
					ONDK_GWM_KillTimer(GWMPOPUP_TIMER_STARTLOGOID);
					ONDK_WINDOW_Clear(ONDK_GWM_GetWindow(ONDK_UPPER_WINDOW));
					ONDK_WINDOW_Update(ONDK_GWM_GetWindow(ONDK_UPPER_WINDOW),NULL);

					pstContent->hVideoHandle = NULL;
					ONDK_GWM_APP_Destroy(0);
					return GWM_MESSAGE_BREAK;
				}
				else
				{
					if (pstContent->nCurDrawStrNum < VFD_STRING_COUNT){
						local_startlogo_SetVFDString(s_ucVFDStrings[pstContent->nCurDrawStrNum]);
						pstContent->nCurDrawStrNum++;
					}
					else
						pstContent->nCurDrawStrNum=0;

#if defined(CONFIG_PRISM_ZAPPING)
					stRect = ONDK_Rect(STARTLOGO_IMG_X, STARTLOGO_IMG_Y, STARTLOGO_IMG_W, STARTLOGO_IMG_H);
#else
					stRect = ONDK_Rect(STARTLOGO_X,STARTLOGO_Y,STARTLOGO_W,STARTLOGO_H);
#endif
					pstContent->hVideoHandle = ONDK_VIDEO_Create(STARTLOGO_IMG);
					if(pstContent->hVideoHandle)
					{
						ONDK_VIDEO_PlayOnece(pstContent->hVideoHandle, ONDK_GWM_GetSurface(ONDK_UPPER_WINDOW),&stRect,(void *)local_startlogo_updatewindow,NULL);
						HxLOG_Print(HxANSI_COLOR_YELLOW("################### Start Logo[%d/%d]##################\n") ,loopcnt,STARTLOGO_LOOPCNT);
					}

				}
			}
		}
		return GWM_MESSAGE_BREAK;
	}
#if defined(CONFIG_PRISM_ZAPPING)
	else if(p1 == GWMPOPUP_TIMER_STARTLOGO_LOADING)
	{
		if(pstContent->ulDrawCount < STARTLOGO_LOADING_IMG_1 || pstContent->ulDrawCount >= IMG_URL_MAX)
		{
			pstContent->ulDrawCount = STARTLOGO_LOADING_IMG_1;
		}
		ONDK_GWM_COM_Image_SetImage(STARTLOGO_LOADING_IMG_ID, local_startlogo_ConvertIndexToURL(pstContent->ulDrawCount));
		ONDK_GWM_APP_InvalidateObject(STARTLOGO_LOADING_IMG_ID);
		pstContent->ulDrawCount ++;
	}
	else if(p1 == GWMPOPUP_TIMER_STARTLOGO_SYSMSG)
	{
		// 여기서 시스템 메세지를 띄우는 걸로...
		if(pstContent->hVideoHandle == NULL)
		{
			if(pstContent->ulCurViewState != pstContent->ulNewViewState)
			{
				pstContent->ulCurViewState = pstContent->ulNewViewState;
				local_startlogo_DrawSystemMessage(pstContent, pstContent->ulNewViewState);
			}
		}
	}
#endif
	return GWM_MESSAGE_PASS;
}

static ONDK_Result_t local_startlogo_HAPINotifier(HAPIMSG_e eMsg, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
#if defined(CONFIG_DEBUG)
	local_startlogo_HapiMsgConvertToString(eMsg);
#endif

	switch (eMsg)
	{
		case eHMSG_MAINAPP_READY:
			ONDK_GWM_APP_Destroy(0);
			return GWM_MESSAGE_PASS;

		case eHMSG_SYSSTATE_CHANGE:
			{
				PxSTATUS_e	ePapiStatus = nParam1;
#if defined(CONFIG_DEBUG)
				local_startlogo_PapiStatusConvertToString(ePapiStatus);
#endif
				if(ePapiStatus == ePAMA_STATUS_Reboot || ePapiStatus == ePAMA_STATUS_Shutdown)
				{
					ONDK_GWM_APP_Destroy(0);
				}
				return GWM_MESSAGE_PASS;
			}

		default:
			return GWM_MESSAGE_PASS;
	}

	return GWM_MESSAGE_PASS;
}



#if defined(CONFIG_PRISM_ZAPPING)


static void local_startlogo_GetSpecificInformation(HUINT32 ulOperatorId)
{
#if defined(CONFIG_PROD_HMS1000S) || defined(CONFIG_PROD_HMSLITE)
	ulOperatorId = CLIB_OP_NULL;
#elif defined(CONFIG_PROD_HMS1000T)
	ulOperatorId = CLIB_OP_AUSDTT;
#elif defined(CONFIG_OP_MIDDLE_EAST)
	ulOperatorId = CLIB_OP_ME;
#elif defined(CONFIG_OP_ERTELECOM)
	ulOperatorId = CLIB_OP_ERTELECOM;
#elif defined(CONFIG_OP_SES)
	ulOperatorId = CLIB_OP_SES;
#elif defined(CONFIG_OP_UK_DTT)
	ulOperatorId = CLIB_OP_UK_DTT;
#else
	ulOperatorId = CLIB_OP_NULL;
#endif
}

static HINT32	local_startlogo_SortByLCN (const void *pvArg1, const void *pvArg2)
{
	const DxService_t *pstService1 = *(const DxService_t **)pvArg1;
	const DxService_t *pstService2 = *(const DxService_t **)pvArg2;

	return (pstService1->lcn - pstService2->lcn);
}

static HxVector_t * local_startlogo_GetServiceList(void)
{
	HxVector_t	*pWholeSvcList = NULL; // DxService_t 타입의 데이터...
	HxVector_t	*pstReturnSvcList = NULL; // CListItem_t 타입의 데이터...
	HUINT32		ulOperatorId = CLIB_OP_NULL;
	HUINT32		ulGroupType = CLIB_TYPE_TV;
	HINT32		ulLastSvcGroupType = eServiceGroupType_TV;
	HUINT32		ulExt = 0;

	HxLOG_Trace();

	// STEP 1. 전체 Service List를 가져온다.  Sort는 ... CLIB Generate에서 하는 걸로...
	pWholeSvcList = APK_META_SVC_GetServiceList();
	if(pWholeSvcList == NULL)
	{
		HxLOG_Error("Empty Service List ... \n");
		return NULL;
	}

	// Step 2. Service List를 특정 모델이나 OP사양에 맞추도록 가공하기 위해 정보를 가져옴...
	local_startlogo_GetSpecificInformation(ulOperatorId);

	// Step 3. 저장된 Last Group을 가져온다.
	APK_SCENARIO_GetLastServiceGroup(&ulLastSvcGroupType);

	// Step 4. 해당 Group의 Service List로 가공한다.

	switch(ulLastSvcGroupType)
	{
		case eServiceGroupType_TV:
			HxLOG_Print("TV TYPE\n");
			ulGroupType = CLIB_TYPE_TV;
			ulExt = 0;
			break;
		case eServiceGroupType_Radio:
			HxLOG_Print("RADIO TYPE\n");
			ulGroupType = CLIB_TYPE_RADIO;
			ulExt = 0;
			break;
		case eServiceGroupType_HDTV:
			HxLOG_Print("HDTV TYPE\n");
			ulGroupType = CLIB_TYPE_HD;
			ulExt = 0;
			break;
		case eServiceGroupType_PayTV:
			HxLOG_Print("CAS TYPE\n");
			// eDxCAS_TYPE_FTA 제외...
			ulGroupType = CLIB_TYPE_CAS;
			ulExt = 0xFFFE;
			break;
		case eServiceGroupType_FreeTV:
			HxLOG_Print("FREE TYPE\n");
			ulGroupType = CLIB_TYPE_CAS;
			ulExt = eDxCAS_TYPE_FTA;
			break;
		default:
			HxLOG_Print("ulGroupType (0x%x)\n", ulLastSvcGroupType);
			// Alphabet Type ...
			if(ulLastSvcGroupType>=eServiceGroupType_AlphabetAll && ulLastSvcGroupType<= eServiceGroupType_AlphabetETC)
			{
				ulGroupType = CLIB_TYPE_ALPABET;
				ulExt = ulLastSvcGroupType & 0x00FF;
				// DxAlpabetGruopType_b 여기에 해당하는 값으로 맞춰줌...
			}
			else if(ulLastSvcGroupType>=eServiceGroupType_Favorite1 && ulLastSvcGroupType<= eServiceGroupType_Favorite5)
			{
				// 현재 Favorite채널은 위에서 들고 있으므로...처리불가...
				return NULL;
			}
			else
			{
				// 기타 등등은...아직 없지만..일단...
				return NULL;
			}
			break;
	}

	// sort by number
	(void) HLIB_VECTOR_Sort(pWholeSvcList, local_startlogo_SortByLCN);
	pstReturnSvcList = CLIB_GenerateList(pWholeSvcList, NULL, ulGroupType, ulOperatorId, ulExt, TRUE);
	return pstReturnSvcList;
}


static HERROR local_startlogo_LoadChannelList(void)
{
	HxVector_t				*list;
	startlogo_Contents_t	*pstContent = NULL;

	HxLOG_Trace();

	pstContent = local_startlogo_GetContents();
	if(pstContent == NULL)
	{
		HxLOG_Error("Cannot get Context.\n");
		return ERR_FAIL;
	}

	list = local_startlogo_GetServiceList();
	if(list == NULL)
	{
		pstContent->bLoadedServiceList = FALSE;
		pstContent->pServiceList = NULL;
		return ERR_FAIL;
	}
	else
	{
		pstContent->bLoadedServiceList = TRUE;
		pstContent->pServiceList = list;
		return ERR_OK;
	}

}


static CListItem_t *local_startlogo_FindChannel(HINT32	nSvcUid, startLogoChannel_e eZappingMode)
{
//	HxVector_t				*list;
	startlogo_Contents_t	*pstContent = NULL;
	HINT32					nServiceListLength = 0;
	HINT32					nLoopCount = 0;
	HINT32					nTargetChannelIndex = 0;
	CListItem_t				*pstServiceItem = NULL;

	pstContent = local_startlogo_GetContents();
	if(pstContent == NULL)
	{
		HxLOG_Error("Context is Null.\n");
		return NULL;
	}

	if(pstContent->pServiceList == NULL)
	{
		HxLOG_Error("There is no Service List ... \n");
		return NULL;
	}

	nServiceListLength = HLIB_VECTOR_Length(pstContent->pServiceList);
	HxLOG_Print("CurSvc Uid : [%d]\n", nSvcUid);
	HxLOG_Print("Service Count : [%d]\n", nServiceListLength);
	for (nLoopCount = 0 ; nLoopCount < nServiceListLength ; nLoopCount++)
	{
		// 전제조건 : pstContent->pServiceList 는 lcn으로 sort되어 있어야 한다.
		pstServiceItem = HLIB_VECTOR_ItemAt(pstContent->pServiceList, nLoopCount);
		if(pstServiceItem == NULL)
		{
			continue;
		}

		if(pstServiceItem->uid != nSvcUid)
		{
			continue;
		}

		HxLOG_Error("curr channel[%d]\n", pstServiceItem->nLogicalNo);

		switch(eZappingMode)
		{
			case STARTLOGO_CHANNEL_CURRENT:
				return pstServiceItem;

			case STARTLOGO_CHANNEL_PREV:
				if(nLoopCount >= 1)
				{
					nTargetChannelIndex = nLoopCount - 1;
				}
				else
				{
					nTargetChannelIndex = nServiceListLength - 1;
				}
				HxLOG_Print("nTargetChannelIndex : [%d]\n", nTargetChannelIndex);
				pstServiceItem = HLIB_VECTOR_ItemAt(pstContent->pServiceList, nTargetChannelIndex);
				HxLOG_Error("prev channel[%d]\n", pstServiceItem->nLogicalNo);
				return pstServiceItem;

			case STARTLOGO_CHANNEL_NEXT:
				if(nLoopCount < nServiceListLength -1)
				{
					nTargetChannelIndex = nLoopCount + 1;
				}
				else
				{
					nTargetChannelIndex = 0;
				}
				HxLOG_Print("nTargetChannelIndex : [%d]\n", nTargetChannelIndex);
				pstServiceItem = HLIB_VECTOR_ItemAt(pstContent->pServiceList, nTargetChannelIndex);
				HxLOG_Error("next channel[%d]\n", pstServiceItem->nLogicalNo);
				return pstServiceItem;
			default:
				// Error...
				return NULL;
		}
	}
	HxLOG_Error("Can not Found Target Channel.\n");
	return NULL;
}



static void local_startlogo_SetStartupZapping(startLogoChannel_e eZappingMode)
{
	STATIC		HBOOL bSetZapping = FALSE;
	HBOOL		pbCheckColdBootDone = FALSE;
	HUINT32 	ulMasterViewId;
	HUINT32 	ulLastSvcUid;
	HUINT32 	ulSessionId;
	CListItem_t	*pstService = NULL;
	DxService_t	*pstTmpService = NULL;
	startlogo_Contents_t	*pstContent = NULL;
	OxMediaPlay_StartInfo_t	unStart;
	DxWakeUpReason_e		eWakeUpReason = eDxWAKEUP_NONE;

	pstContent = local_startlogo_GetContents();
	if(pstContent == NULL)
	{
		HxLOG_Error("Context is Null.\n");
		return;
	}

	if(APK_POWER_GetWakeupReason(&eWakeUpReason) == ERR_OK)
	{
		switch(eWakeUpReason)
		{
			case eDxWAKEUP_TIMER:
				HxLOG_Warning("Dont't Zapping Because Wake up by Timer.\n");
				return;
			case eDxWAKEUP_NONE:
			case eDxWAKEUP_KEY:
			case eDxWAKEUP_ACPOWER:
			case eDxWAKEUP_HDMICEC:
			case eDxWAKEUP_REBOOT:
			default:
				break;
		}
	}

	if(APK_POWER_IsStandbyMode()==TRUE)
	{
		HxLOG_Warning("Dont't Zapping Because Standby Mode.\n");
		return;
	}

	APK_SYSINFO_STB_GetColdBootDone( &pbCheckColdBootDone, NULL);
	// APK_SYSINFO_STB_GetColdBootDone의 Return값은 의미 없음...
	HxLOG_Print("pbCheckColdBootDone[%d], bSetZapping[%d]\n", pbCheckColdBootDone, bSetZapping);

	// zapping을 하려면 ColdBoot Done은 필수조건...
	if(pbCheckColdBootDone == TRUE)
	{
		if(bSetZapping == FALSE)
		{
			// 처음은 무조건 Last Service로...
			if (HAPI_GetRegistryInt(PRISM_DB_LASTSERVICE, &ulLastSvcUid) == ERR_OK)
			{
				if(ulLastSvcUid == -1)
				{
					HxLOG_Error("Can not Find Last Service.\n");
					return;
				}
				pstContent->nCurSvcUid = ulLastSvcUid;
			}
			else
			{
				// 이러면 뭐...어디로 Zapping해야하나...괜히 1번 보여주면 문제겠지....Web이 SetChannel할때까지 기다리는 수 밖에
				HxLOG_Error("Can not Find Last Service.\n");
				return;
			}
		}
		else
		{
			// 그 외에는 Zapping Mode에 따라서...
			// 현재 채널이 아닌경우에만 Start를 하면 될듯... bSetZapping 값에 따라 이미 Live중이므로...
			if(eZappingMode == STARTLOGO_CHANNEL_CURRENT)
			{
				// 이미 Zapping된 상태이고 현재 채널에 대해서는 Live Start를 내려줄 필요가 없다.
				HxLOG_Print("The Service is already On Air.\n");
				return;
			}
			else
			{
				if(pstContent->bLoadedServiceList == FALSE)
				{
					// Service List를 Load하지 못했다면....다른 채널로 돌릴 수 없음...
					HxLOG_Print("There is only One Channel.\n");
					return;
				}
			}
		}

		if(pstContent->bLoadedServiceList == TRUE)
		{
			// Serive List를 Load했다면...내부 Function을 사용함...
			pstService = local_startlogo_FindChannel(pstContent->nCurSvcUid, eZappingMode);
			if(pstService)
			{
				unStart.stLive.ulMajorCHNum 	= pstService->nLogicalNo;
				pstContent->nCurSvcUid			= (HINT32)pstService->uid;
			}
			else
			{
				// 현재 Group으로 만들어진 채널리스트에 없는 채널이 방송되고 있다? 이러면 Last Service와 Last Group이 같이 저장되지 않고 있다는 얘기...
				HxLOG_Error("Error, Can not found Channel.\n");
				return;
			}
		}
		else
		{
			// Load 못했다면... 그냥 해당 Service만 가져오자...
			pstTmpService = APK_META_SVC_GetService(pstContent->nCurSvcUid);
			if(pstTmpService)
			{
				unStart.stLive.ulMajorCHNum 	= pstTmpService->lcn;
				APK_META_SVC_Delete(pstTmpService);
			}
			else
			{
				HxLOG_Error("Error, Can not found Channel.\n");
				return;
			}
		}
		unStart.stLive.eType			= eOxMP_MEDIATYPE_LIVE;
		unStart.stLive.eLiveType		= eOxMP_LIVETYPE_MAIN;
		unStart.stLive.ulMasterSvcUid	= pstContent->nCurSvcUid;
		unStart.stLive.ulSuppleSvcUid	= pstContent->nCurSvcUid;

//		HLIB_DATETIME_CheckBoot("StartLogo Zapping");

		if(APK_MEDIA_PLAY_GetMainViewId(&ulMasterViewId) == ERR_OK)
		{
			APK_MEDIA_PLAY_Start(ulMasterViewId, &unStart, &ulSessionId);
		}

		bSetZapping = TRUE;
	}
}

static ONDK_Result_t local_startlogo_MsgApkSysinfo(HINT32 p1, HINT32 p2, HINT32 p3)
{
	local_startlogo_SetStartupZapping(STARTLOGO_CHANNEL_CURRENT);
	return ONDK_MESSAGE_CONSUMED;
}


static ONDK_Result_t local_startlogo_MsgApkInitialized(HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t			hRet = ONDK_RET_OK;
	startlogo_Contents_t	*pstContent = NULL;

	pstContent = local_startlogo_GetContents();
	if(pstContent)
	{
		hRet = local_startlogo_SetObject(pstContent);
		if(hRet != ONDK_RET_OK)
		{
			HxLOG_Warning("error!! local_startlogo_SetObject\r\n");
		}
	}
	local_startlogo_SetStartupZapping(STARTLOGO_CHANNEL_CURRENT);
	return hRet;
}



static ONDK_Result_t	local_startlogo_APKMessage(Handle_t hAct,HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3)
{
	GWM_APK_MSG_Type_e			eAPK_Type 	= (GWM_APK_MSG_Type_e)hAct;
	HUINT32						ulViewId	=	(ulParam1 >> 24) & 0x000F;
	// OxMediaPlay_MediaType_e	eType		=	(ulParam1 >> 16) & 0x000F;
	OxMediaPlay_Event_e			eEvent		=	(ulParam1 & 0x00FF);
	OxMediaPlay_LiveViewState_e	eViewState;
	startlogo_Contents_t		*pstContent	= NULL;
	HINT32						nLoopCount	= 0;

	pstContent = local_startlogo_GetContents();
	if(pstContent == NULL)
	{
		HxLOG_Error("Something Wrong.\n");
		return GWM_RET_FAIL;
	}

	HxLOG_Debug("liveMediaNotifier...\n");

	if(eAPK_Type == GWM_APK_MEDIA)
	{
		HxLOG_Debug("[%s] -- ulViewId : %d, eEvent(%d) \n", __FUNCTION__, ulViewId, eEvent);
		switch (eEvent)
		{
			case eOxMP_EVENT_AV_STARTED:
				// Obama의 Lock proc과 live proc 두군데서 날라옴...
				HxLOG_Debug("Received eOxMP_EVENT_AV_STARTED\n");
				ONDK_GWM_KillTimer(GWMPOPUP_TIMER_STARTLOGOID);
				if(pstContent->bAVStarted == FALSE)
				{
					if (pstContent->hVideoHandle)
					{
						ONDK_VIDEO_Release(pstContent->hVideoHandle);
						pstContent->hVideoHandle = NULL;
					}
					HxLOG_Debug("ONDK_GWM_APP_InvalidateAPP 1\n");
				}
				for(nLoopCount = 0; nLoopCount < pstContent->nSysMsgLineNum; nLoopCount++)
				{
					ONDK_GWM_Obj_Destroy(STARTLOGO_SYSMSG_ID + nLoopCount);
				}
				ONDK_GWM_Obj_Destroy(STARTLOGO_SYSMSG_YELLOWLINE_UP_ID);
				ONDK_GWM_Obj_Destroy(STARTLOGO_SYSMSG_ICON_ID);
				ONDK_GWM_Obj_Destroy(STARTLOGO_SYSMSG_YELLOWLINE_DOWN_ID);

				ONDK_GWM_APP_InvalidateAPP(ONDK_GWM_APP_GetThis());
				ONDK_WINDOW_Clear(ONDK_GWM_GetWindow(ONDK_UPPER_WINDOW));
				ONDK_WINDOW_Update(ONDK_GWM_GetWindow(ONDK_UPPER_WINDOW),NULL);

				pstContent->bAVStarted = TRUE;
				break;
			case eOxMP_EVENT_LIVE_ViewStateChanged:
				eViewState = ulParam2;
				pstContent->ulNewViewState = eViewState;
				HxLOG_Debug("Received eOxMP_EVENT_LIVE_ViewStateChanged [%d]\n", eViewState);
				if(eViewState == eOxMP_LiveViewState_OK)
				{
					HxLOG_Debug("System Message Clear ~!!!!\n");
					for(nLoopCount = 0; nLoopCount < pstContent->nSysMsgLineNum; nLoopCount++)
					{
						ONDK_GWM_Obj_Destroy(STARTLOGO_SYSMSG_ID + nLoopCount);
					}
					ONDK_GWM_Obj_Destroy(STARTLOGO_SYSMSG_YELLOWLINE_UP_ID);
					ONDK_GWM_Obj_Destroy(STARTLOGO_SYSMSG_ICON_ID);
					ONDK_GWM_Obj_Destroy(STARTLOGO_SYSMSG_YELLOWLINE_DOWN_ID);
				}

				// 밑에서 뭔가 그리라고 메세지 보냈으므로 가운데 화면은 제거...Timer 도 제거...
				ONDK_GWM_KillTimer(GWMPOPUP_TIMER_STARTLOGOID);
				if (pstContent->hVideoHandle)
				{
					ONDK_VIDEO_Release(pstContent->hVideoHandle);
					pstContent->hVideoHandle = NULL;
				}
				ONDK_GWM_APP_InvalidateAPP(ONDK_GWM_APP_GetThis());
				break;

			default:
				HxLOG_Debug("Start Logo module doesn't need other media control event [%d]\n", eEvent);
				break;
		}
	}

	return GWM_RET_OK;
}

#endif

static ONDK_Result_t local_startlogo_MsgGwmKeyDown(HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32					nKeyId = p1;
#if defined(CONFIG_PRISM_ZAPPING)
	ONDK_Rect_t 			stLoadingBGRect;
	ONDK_Rect_t 			stLoadingImgRect;
	ONDK_Rect_t 			stLoadingTextRect;
	startlogo_Contents_t	*pstContent = NULL;
	HUINT8					*pstTmpText = NULL;

	if(ONDK_GWM_COM_Image_GetImage(STARTLOGO_LOADING_IMG_ID) == NULL)
	{
		stLoadingBGRect = ONDK_Rect(LOADING_BG_X, LOADING_BG_Y, LOADING_BG_W, LOADING_BG_H);
		ONDK_GWM_COM_Rect_Create(STARTLOGO_LOADING_BG_ID, &stLoadingBGRect, 0xD8000000);

		stLoadingImgRect = ONDK_Rect(LOADING_IMG_X,LOADING_IMG_Y,LOADING_IMG_W, LOADING_IMG_H);
		ONDK_GWM_COM_Image_Create(STARTLOGO_LOADING_IMG_ID, &stLoadingImgRect);
		ONDK_GWM_COM_Image_SetImage(STARTLOGO_LOADING_IMG_ID, local_startlogo_ConvertIndexToURL(STARTLOGO_LOADING_IMG_1));

		pstContent = local_startlogo_GetContents();
		if(pstContent)
		{
			if(pstContent->pucLoadingText == NULL)
			{
				pstContent->pucLoadingText = AP_Malloc(128);
			}

			if(pstContent->pucLoadingText)
			{
				HxSTD_MemSet(pstContent->pucLoadingText, 0, 128);
				pstTmpText = ONDK_GetString("STR_LOADING_ID");
				HxSTD_MemCopy(pstContent->pucLoadingText, pstTmpText, HxSTD_StrLen(pstTmpText));

				stLoadingTextRect = ONDK_Rect(LOADING_TEXT_X, LOADING_TEXT_Y, LOADING_TEXT_W, LOADING_TEXT_H);
				ONDK_GWM_COM_Text_Create(STARTLOGO_LOADING_TEXT_ID, &stLoadingTextRect, pstContent->pucLoadingText);
				ONDK_GWM_COM_Text_SetSize(STARTLOGO_LOADING_TEXT_ID, LOADING_TEXT_FONT_SIZE);
				ONDK_GWM_COM_Text_ChangeColor(STARTLOGO_LOADING_TEXT_ID, LOADING_TEXT_FONT_COLOR, LOADING_TEXT_FONT_COLOR);
				ONDK_GWM_COM_Text_SetFont(STARTLOGO_LOADING_TEXT_ID, eFont_SystemBold);
			}
		}
	}
	else
	{
		HxLOG_Warning("The image is already exist.\n");
	}

	ONDK_GWM_SetTimer(GWMPOPUP_TIMER_STARTLOGO_LOADING, STARTLOGO_TIME_SECOND); // ONDK_GWM_SetTimer 안에서 중복체크 하므로 별도 reset이나 kill은 하지 않는다.
#endif
	switch (nKeyId)
	{
		case KEY_STANDBY:
			return ONDK_MESSAGE_NONCONSUMED;
#if defined(CONFIG_PRISM_ZAPPING)
		case KEY_VOLUMEDOWN:
		case KEY_VOLUMEUP:
		case KEY_MUTE:
			return ONDK_RET_OK;
		case KEY_CH_PLUS:
			HxLOG_Print(HxANSI_COLOR_PURPLE(">>>>>>>>> KEY_CH_PLUS <<<<<<<<<<<\n"));
			local_startlogo_SetStartupZapping(STARTLOGO_CHANNEL_NEXT);
			return ONDK_MESSAGE_CONSUMED;
		case KEY_CH_MINUS:
			HxLOG_Print(HxANSI_COLOR_PURPLE(">>>>>>>>> KEY_CH_MINUS <<<<<<<<<<<\n"));
			local_startlogo_SetStartupZapping(STARTLOGO_CHANNEL_PREV);
			return ONDK_MESSAGE_CONSUMED;
#endif
		default:
			return ONDK_MESSAGE_CONSUMED;
	}
}



static ONDK_Result_t local_startlogo_MsgGwmCreate(HINT32 p1, HINT32 p2, HINT32 p3)
{
	startlogo_Contents_t	*pstContent = NULL;
	HINT32					nValue = FALSE;
	DxWakeUpReason_e		eWakeupReason = eDxWAKEUP_NONE;

#if defined(CONFIG_PRISM_ZAPPING)
	HCHAR					szLang[4]	= {0,};
#endif
	pstContent = local_startlogo_GetContents();
	if(pstContent)
	{
#if defined(CONFIG_PRISM_ZAPPING)
		local_startlogo_initContents(pstContent);
#else
		APK_POWER_GetWakeupReason(&eWakeupReason);

		if (APK_POWER_GetLastStandby(&nValue) != ERR_OK)
		{
			nValue = FALSE;
		}

		if (!(eWakeupReason == eDxWAKEUP_TIMER
			|| (eWakeupReason == eDxWAKEUP_ACPOWER && nValue == TRUE)))
		{
			local_startlogo_SetVFDString(s_ucVFDStrings[0]);
			(void)local_startlogo_SetObject(pstContent);
		}
#endif
	}

#if defined(CONFIG_PROD_HMS1000T)
	// web app이 동작을 해야 HDMI enable되고 있어서 먼저 뜨는 start logo가 보이지 않음. 그래서, 여기서 먼저 HDMI Enable을 시도함
	// 그런데 이상하게 HMS-1000S는 잘 나오고, HMS-1000T만 안나오고 있어서, 1000T만 적용
	APK_OUTPUT_VIDEO_Init();
	APK_OUTPUT_VIDEO_SetAvEnable(TRUE);
#endif

#if defined(CONFIG_PRISM_ZAPPING)
	if(local_startlogo_LoadChannelList() == ERR_FAIL)
	{
		HxLOG_Error("Fail to Load Channel List.\n");
	}

	if(APK_SCENARIO_GetMainLanguage(szLang)==ERR_OK)
	{
		//HxLOG_Print("____local_startlogo_MsgGwmCreate ONDK_SetLanguage\n");
		ONDK_SetLanguage(szLang);
	}

	ONDK_GWM_SetTimer(GWMPOPUP_TIMER_STARTLOGO_SYSMSG, STARTLOGO_SYSMSG_CHECK);
#endif
	return ONDK_RET_OK;
}


/**************************************************************************************************/
#define _________START_LOGO_Public_Functions__________________________________________________________
/**************************************************************************************************/


ONDK_Result_t		AP_Startlogo_Proc(HINT32 lMessage, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t			lRet = GWM_RET_OK;
	ONDK_Print(HxANSI_COLOR_YELLOW("[%s:%d] Message(%d) : %s, handle: %p p1(0x%x) p2(0x%x) p3(0x%x) \n"), __FUNCTION__, __LINE__, lMessage, ONDK_GWM_GwmMsg2String(lMessage), p1, p2, p3);

	switch(lMessage)
	{
		case MSG_GWM_CREATE:
#if defined(CONFIG_PRISM_ZAPPING)
			lRet = local_startlogo_MsgGwmCreate(p1, p2, p3);
#else
			// Create 만 하고 animation 은 MSG_GWM_APK_INITIALIZED 받은 후 실행.
			//lRet = local_startlogo_MsgGwmCreate(p1, p2, p3);
#endif
			break;
		case MSG_GWM_TIMER:
			lRet = local_startlogo_MsgGwmTimer(p1, p2, p3);
			break;
		case MSG_GWM_APK_INITIALIZED:
#if defined(CONFIG_PRISM_ZAPPING)
			lRet = local_startlogo_MsgApkInitialized(p1, p2, p3);
#else
			lRet = local_startlogo_MsgGwmCreate(p1, p2, p3);
#endif
			break;
		case MSG_GWM_HAPI_MESSAGE:
			lRet = local_startlogo_HAPINotifier((HAPIMSG_e)hHandle, p1, p2, p3);
			break;
		case MSG_GWM_KEYDOWN:

			lRet = local_startlogo_MsgGwmKeyDown(p1, p2, p3);
			break;
		case MSG_GWM_KEYUP:
			lRet = ONDK_MESSAGE_CONSUMED;
			break;
#if defined(CONFIG_PRISM_ZAPPING)
		case MSG_GWM_APK_MESSAGE:
			lRet = local_startlogo_APKMessage(hHandle,p1, p2, p3);
			break;
		case MSG_GWM_APK_SYSINFO:
			lRet = local_startlogo_MsgApkSysinfo(p1, p2, p3);
			break;
#endif
		case MSG_GWM_DESTROY:
			lRet = local_startlogo_MsgGwmDestroy(p1, p2, p3);
			break;
	}

	if(lRet != GWM_RET_OK)
	{
		return lRet;
	}

	return ONDK_GWM_ProcessMessageDefault(lMessage, hHandle, p1, p2, p3);
}



/**************************************************************************************************/
#define _________START_LOGO_End_Of_File____________________________________________________
/**************************************************************************************************/


