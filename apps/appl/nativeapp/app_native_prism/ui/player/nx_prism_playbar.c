/*
 * (c) 2011-2013 Humax Co., Ltd.
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
 * CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.

 * This License is effective until terminated. You may terminate this License at any time by destroying all copies
 * of the Software including all documentation. This License will terminate immediately without notice from Humax
 * to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
 * of the Software and all documentation.

 * The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
 * All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
 * in the Republic of Korea.
*/

/******************************************************************************/
/**
 * @file	  		nx_prism_playbar.h
 *
 * 	Description:  									\n
 *
 * @author											\n
 * @date											\n
 * @Attention		Copyright (c) 2013 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/

/******************************************************************************/
#define _________Private_Include______________________________________________
/******************************************************************************/
#include <nx_common.h>

#include <nx_prism_app.h>


/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/

#define NX_PLAYBAR_ITEM_ID			eMenuFrame_Window_Playbar

#define NX_PLAYBAR_DIM_IMG_ID	(NX_PLAYBAR_ITEM_ID + 1)
#define NX_PLAYBAR_DIM_IMG_X	0
#define NX_PLAYBAR_DIM_IMG_Y	320
#define NX_PLAYBAR_DIM_IMG_W	1280
#define NX_PLAYBAR_DIM_IMG_H	400

#define NX_PLAYBAR_TITLE_ID		(NX_PLAYBAR_ITEM_ID + 2)
#define NX_PLAYBAR_TITLE_X		90
#define NX_PLAYBAR_TITLE_Y		(513-NX_PRISM_FONT_VSPACE)
#define NX_PLAYBAR_TITLE_W		1100
#define NX_PLAYBAR_TITLE_H		(NX_PRISM_FONT_SIZE_26 + NX_PRISM_FONT_VSPACE)

#define NX_PLAYBAR_PLAYICON_ID	(NX_PLAYBAR_ITEM_ID + 3)
#define NX_PLAYBAR_PLAYICON_X	93
#define NX_PLAYBAR_PLAYICON_Y	574
#define NX_PLAYBAR_PLAYICON_W	99
#define NX_PLAYBAR_PLAYICON_H	34

#define NX_PLAYBAR_TIME_CURRENT_ID	(NX_PLAYBAR_ITEM_ID + 4)
#define NX_PLAYBAR_TIME_CURRENT_X	238
#define	NX_PLAYBAR_TIME_CURRENT_X2	226
#define NX_PLAYBAR_TIME_CURRENT_Y	(584-NX_PRISM_FONT_VSPACE)
#define NX_PLAYBAR_TIME_CURRENT_W	105
#define NX_PLAYBAR_TIME_CURRENT_H	(NX_PRISM_FONT_SIZE_20 + NX_PRISM_FONT_VSPACE)

#define	NX_PLAYBAR_TIME_TOTAL_ID	(NX_PLAYBAR_ITEM_ID + 5)
#define NX_PLAYBAR_TIME_TOTAL_X		1100
//#define NX_PLAYBAR_TIME_TOTAL_Y		()

#define NX_PLAYBAR_BTN_FR_ID		(NX_PLAYBAR_ITEM_ID + 6)
#define NX_PLAYBAR_BTN_FR_X			432
#define NX_PLAYBAR_BTN_FR_Y			625
#define NX_PLAYBAR_BTN_FR_W			100
#define NX_PLAYBAR_BTN_FR_H			50

#define NX_PLAYBAR_BTN_PLAY_PAUSE_ID		(NX_PLAYBAR_ITEM_ID + 7)
#define NX_PLAYBAR_BTN_PLAY_PAUSE_X			(NX_PLAYBAR_BTN_FR_X + NX_PLAYBAR_BTN_FR_W+ 5)
#define NX_PLAYBAR_BTN_PLAY_PAUSE_Y			625
#define NX_PLAYBAR_BTN_PLAY_PAUSE_W			100
#define NX_PLAYBAR_BTN_PLAY_PAUSE_H			50

#define NX_PLAYBAR_BTN_FF_ID		(NX_PLAYBAR_ITEM_ID + 8)
#define NX_PLAYBAR_BTN_FF_X			(NX_PLAYBAR_BTN_PLAY_PAUSE_X + NX_PLAYBAR_BTN_PLAY_PAUSE_W + 5)
#define NX_PLAYBAR_BTN_FF_Y			625
#define NX_PLAYBAR_BTN_FF_W			100
#define NX_PLAYBAR_BTN_FF_H			50

#define NX_PLAYBAR_BTN_STOP_ID		(NX_PLAYBAR_ITEM_ID + 9)
#define NX_PLAYBAR_BTN_STOP_X		(NX_PLAYBAR_BTN_FF_X + NX_PLAYBAR_BTN_FF_W + 5)
#define NX_PLAYBAR_BTN_STOP_Y		625
#define NX_PLAYBAR_BTN_STOP_W		200	//1 Need to check the guide document is right or not. This value is 100 in guide doc.
#define NX_PLAYBAR_BTN_STOP_H		50

#define	NX_PLAYBAR_BTN_SUB_ID		(NX_PLAYBAR_ITEM_ID + 10)
#define	NX_PLAYBAR_BTN_SUB_X		(NX_PLAYBAR_BTN_STOP_X + NX_PLAYBAR_BTN_STOP_W + 5) - (NX_PLAYBAR_BTN_SUB_W / 2)
#define	NX_PLAYBAR_BTN_SUB_Y		625
#define	NX_PLAYBAR_BTN_SUB_W		200
#define	NX_PLAYBAR_BTN_SUB_H		50

#define NX_PLAYBAR_POINTER_01_ID		(NX_PLAYBAR_ITEM_ID + 11)
#define NX_PLAYBAR_POINTER_X_01_INITIAL	301
#define NX_PLAYBAR_POINTER_01_Y		578 //1 need to check
#define NX_PLAYBAR_POINTER_01_W		28
#define NX_PLAYBAR_POINTER_01_H		28

#define NX_PLAYBAR_POINTER_02_ID		(NX_PLAYBAR_ITEM_ID + 12)
#define NX_PLAYBAR_POINTER_X_02_INITIAL	301
#define NX_PLAYBAR_POINTER_02_Y		542
#define NX_PLAYBAR_POINTER_02_W		159
#define NX_PLAYBAR_POINTER_02_H		64

#define NX_PLAYBAR_POINTER_TIME_ID		(NX_PLAYBAR_ITEM_ID + 13)
#define NX_PLAYBAR_POINTER_TIME_X_INITIAL	NX_PLAYBAR_POINTER_X_02_INITIAL+35
#define NX_PLAYBAR_POINTER_TIME_Y		NX_PLAYBAR_POINTER_02_Y+8 //need to check
#define NX_PLAYBAR_POINTER_TIME_W		104
#define NX_PLAYBAR_POINTER_TIME_H		64

#define NX_PLAYBAR_RECORDING_BAR_ID	(NX_PLAYBAR_ITEM_ID + 14)
#define NX_PLAYBAR_RECORDING_BAR_X	314
#define NX_PLAYBAR_RECORDING_BAR_Y	589
#define NX_PLAYBAR_RECORDING_BAR_W	781//1 these need to check also. have nothing on the document
#define NX_PLAYBAR_RECORDING_BAR_H	4

#define NX_PLAYBAR_IMG_LINE_01_ID		(NX_PLAYBAR_ITEM_ID + 15)
#define NX_PLAYBAR_IMG_LINE_01_X		208
#define NX_PLAYBAR_IMG_LINE_01_Y		570
#define NX_PLAYBAR_IMG_LINE_01_W		1
#define NX_PLAYBAR_IMG_LINE_01_H		42

#define NX_PLAYBAR_IMG_LINE_02_ID		(NX_PLAYBAR_ITEM_ID + 16)
#define NX_PLAYBAR_IMG_LINE_02_X		209
#define NX_PLAYBAR_IMG_LINE_02_Y		570
#define NX_PLAYBAR_IMG_LINE_02_W		1
#define NX_PLAYBAR_IMG_LINE_02_H		42

#define	NX_PLAYBAR_HIDE_TIMEOUT_ID		(NX_PLAYBAR_ITEM_ID + 17)
#define	NX_PLAYBAR_HIDE_TIMEOUT_PERIOD	10000


/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/
typedef struct NxPlaybar_Context
{
	HUINT32			ulCurFocus;
	HUINT32			ulPointerPosition;
	HUINT32			ulProgressValue;

	HINT32			ulCurrentPlayTime;
	HUINT32			ulDuration;
	HINT32			lSpeed;
	HBOOL			bHide;

	HCHAR			szCurrentPlayTime[NX_TEXT_SIZE_64];
	HCHAR			szTotalTime[NX_TEXT_SIZE_64];
	HCHAR			szStartTime[NX_TEXT_SIZE_64];
	HCHAR			szPlayTitle[NX_TEXT_SIZE_64];
	NxPlayerType_e	ePlayerType;
	//HUINT32			ulNumOfSubItem, ulMaxNumOfSubItem;
	//HUINT32			ulSelectedItem;
	//HCHAR			szNumText[NX_TEXT_SIZE_64];
} NxPlaybar_Context_t;

/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/
static NxPlaybar_Context_t s_stNxPlaybar_Context;


/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/
static ONDK_Result_t nx_Playbar_ProcessLeftSeek(NxPlaybar_Context_t *pstContext, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t nx_Playbar_ProcessRightSeek(NxPlaybar_Context_t *pstContext, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t nx_Playbar_UpdatePlaybarPointerObj(NxPlaybar_Context_t *pstContext);
static ONDK_Result_t nx_Playbar_GetTimeStr(HCHAR* pszTime, HINT32 ulTime, HBOOL bMSec);

/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/

static NxPlaybar_Context_t* nx_playbar_PlaybarGetContext(void)
{
	return &s_stNxPlaybar_Context;
}

#if 0
static ONDK_Result_t nx_Paybar_SetFocusToPointer(NxPlaybar_Context_t *pstContext, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_GWM_COM_Image_SetImage(NX_PLAYBAR_POINTER_02_ID, RES_340_00_PB_PLAYER_POINTER03_C_PNG);
	ONDK_GWM_COM_Text_ChangeColor(NX_PLAYBAR_POINTER_TIME_ID, COL(C_T_Bang_100),(ONDK_Color_t)NULL);
}

static ONDK_Result_t nx_Paybar_UnsetFocusToPointer(NxPlaybar_Context_t *pstContext, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_GWM_COM_Image_SetImage(NX_PLAYBAR_POINTER_02_ID, RES_340_00_PB_PLAYER_POINTER03_N_PNG);
	ONDK_GWM_COM_Text_ChangeColor(NX_PLAYBAR_POINTER_TIME_ID, COL(C_T_Whistle_100),(ONDK_Color_t)NULL);
}
#endif

static ONDK_Result_t nx_Paybar_MoveFocusToPointer(NxPlaybar_Context_t *pstContext, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if(pstContext->ulCurFocus == NX_PLAYBAR_POINTER_01_ID)
	{
		//Do nothing...
		return ONDK_RET_OK;
	}
	else
	{
		ONDK_GWM_COM_Image_SetImage(NX_PLAYBAR_POINTER_02_ID, RES_340_00_PB_PLAYER_POINTER03_C_PNG);
		ONDK_GWM_COM_Text_ChangeColor(NX_PLAYBAR_POINTER_TIME_ID, COL(C_T_Bang_100),(ONDK_Color_t)NULL);
		ONDK_GWM_APP_InvalidateObject(NX_PLAYBAR_POINTER_02_ID);
		pstContext->ulCurFocus = NX_PLAYBAR_POINTER_01_ID;
		ONDK_GWM_Obj_SetFocus(pstContext->ulCurFocus);
		return ONDK_RET_OK;
	}
}

static ONDK_Result_t nx_Paybar_MoveFocusToBtn(NxPlaybar_Context_t *pstContext, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32	lObjID = p1;

	switch (lObjID)
	{
		case NX_PLAYBAR_BTN_FR_ID:
			pstContext->ulCurFocus = NX_PLAYBAR_BTN_FR_ID;
			break;
		case NX_PLAYBAR_BTN_PLAY_PAUSE_ID:
			pstContext->ulCurFocus = NX_PLAYBAR_BTN_PLAY_PAUSE_ID;
			break;
		case NX_PLAYBAR_BTN_FF_ID:
			pstContext->ulCurFocus = NX_PLAYBAR_BTN_FF_ID;
			break;
		case NX_PLAYBAR_BTN_STOP_ID:
			pstContext->ulCurFocus = NX_PLAYBAR_BTN_STOP_ID;
			break;
		default:
			if(pstContext->ulCurFocus == NX_PLAYBAR_POINTER_01_ID)
			{
				if(NX_PLAYBAR_POINTER_X_01_INITIAL+pstContext->ulPointerPosition < NX_PLAYBAR_BTN_FR_X + NX_PLAYBAR_BTN_FR_W)
				{
					pstContext->ulCurFocus = NX_PLAYBAR_BTN_FR_ID;
				}
				else if(NX_PLAYBAR_POINTER_X_01_INITIAL+pstContext->ulPointerPosition < NX_PLAYBAR_BTN_PLAY_PAUSE_X+ NX_PLAYBAR_BTN_PLAY_PAUSE_W)
				{
					pstContext->ulCurFocus = NX_PLAYBAR_BTN_PLAY_PAUSE_ID;
				}
				else if(NX_PLAYBAR_POINTER_X_01_INITIAL+pstContext->ulPointerPosition < NX_PLAYBAR_BTN_FF_X+ NX_PLAYBAR_BTN_FF_W)
				{
					pstContext->ulCurFocus = NX_PLAYBAR_BTN_FF_ID;
				}
				else
				{
					pstContext->ulCurFocus = NX_PLAYBAR_BTN_STOP_ID;
				}
			}
			else
			{
				return ONDK_RET_OK;
			}
			break;
	}

	ONDK_GWM_Obj_SetFocus(pstContext->ulCurFocus);
	ONDK_GWM_COM_Image_SetImage(NX_PLAYBAR_POINTER_02_ID, RES_340_00_PB_PLAYER_POINTER03_N_PNG);
	ONDK_GWM_COM_Text_ChangeColor(NX_PLAYBAR_POINTER_TIME_ID, COL(C_T_Whistle_100),(ONDK_Color_t)NULL);
	ONDK_GWM_APP_InvalidateObject(NX_PLAYBAR_POINTER_02_ID);

	return ONDK_RET_OK;
}

static ONDK_Result_t nx_Paybar_MoveFocusLeft(NxPlaybar_Context_t *pstContext, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32		ulSeekTimeMSec = 15000;

	if(pstContext->ulCurFocus == NX_PLAYBAR_POINTER_01_ID)
	{
		if (pstContext->ulCurrentPlayTime - ulSeekTimeMSec <= 0)
		{
			ulSeekTimeMSec = pstContext->ulCurrentPlayTime;
		}

		if (pstContext->ulDuration > 0)
		{
			pstContext->ulPointerPosition = (HUINT32)((NX_PLAYBAR_RECORDING_BAR_W * (pstContext->ulCurrentPlayTime - ulSeekTimeMSec)) / pstContext->ulDuration);
			nx_Playbar_UpdatePlaybarPointerObj(pstContext);
			nx_Playbar_GetTimeStr((HCHAR*)pstContext->szCurrentPlayTime, (pstContext->ulCurrentPlayTime - ulSeekTimeMSec), TRUE);
		}

		pstContext->ulCurrentPlayTime = pstContext->ulCurrentPlayTime - ulSeekTimeMSec;
		nx_Playbar_ProcessLeftSeek(pstContext, ulSeekTimeMSec, p2, p3);
	}
	else
	{
		if(pstContext->ulCurFocus == NX_PLAYBAR_BTN_FR_ID)
		{
			return ONDK_RET_OK;
		}
		else
		{
			ONDK_GWM_Obj_SetFocus(--(pstContext->ulCurFocus));
			return ONDK_RET_OK;
		}
	}
	return ONDK_RET_OK;
}

static ONDK_Result_t nx_Paybar_MoveFocusRight(NxPlaybar_Context_t *pstContext, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32		ulSeekTimeMSec = 120000;

	if(pstContext->ulCurFocus == NX_PLAYBAR_POINTER_01_ID)
	{
		if ((pstContext->ulCurrentPlayTime + ulSeekTimeMSec) > pstContext->ulDuration)
		{
			ulSeekTimeMSec = (HINT32)((HINT32)pstContext->ulDuration - pstContext->ulCurrentPlayTime);
		}

		if (pstContext->ulDuration > 0)
		{
			pstContext->ulPointerPosition = (HUINT32)((NX_PLAYBAR_RECORDING_BAR_W * (pstContext->ulCurrentPlayTime + ulSeekTimeMSec)) / pstContext->ulDuration);
			nx_Playbar_UpdatePlaybarPointerObj(pstContext);
			nx_Playbar_GetTimeStr((HCHAR*)pstContext->szCurrentPlayTime, (pstContext->ulCurrentPlayTime + ulSeekTimeMSec), TRUE);
		}

		pstContext->ulCurrentPlayTime = pstContext->ulCurrentPlayTime + ulSeekTimeMSec;
		nx_Playbar_ProcessRightSeek(pstContext, ulSeekTimeMSec, p2, p3);
	}
	else
	{
		if (pstContext->ePlayerType == ePlayerType_MEDIAPLAY)
		{
			if(pstContext->ulCurFocus == NX_PLAYBAR_BTN_SUB_ID)
			{
				return ONDK_RET_OK;
			}
			else
			{
				ONDK_GWM_Obj_SetFocus(++(pstContext->ulCurFocus));
				return ONDK_RET_OK;
			}
		}
		else
		{
			if(pstContext->ulCurFocus == NX_PLAYBAR_BTN_STOP_ID)
			{
				return ONDK_RET_OK;
			}
			else
			{
				ONDK_GWM_Obj_SetFocus(++(pstContext->ulCurFocus));
				return ONDK_RET_OK;
			}
		}
	}
	return ONDK_RET_OK;
}

static ONDK_Result_t nx_Playbar_GetTimeStr(HCHAR* pszTime, HINT32 ulTime, HBOOL bMSec)
{
	HINT32	ulTempTime = 0;
	HINT32	lHour = 0, lMin = 0, lSec = 0;
	if (bMSec == TRUE)
	{
		ulTempTime = (HINT32)(ulTime / 1000);
	}
	else
	{
		ulTempTime = ulTime;
	}

	lHour = ulTempTime / (60 * 60);
	ulTempTime = ulTempTime - (lHour * (60 * 60));

	lMin = ulTempTime / 60;
	ulTempTime = ulTempTime - (lMin * 60);

	lSec = ulTempTime;

	HxSTD_sprintf((char*)pszTime, "%02d:%02d:%02d", lHour, lMin, lSec);

	return ONDK_RET_OK;
}

static ONDK_Result_t nx_Playbar_UpdatePlayTitleStr(NxPlaybar_Context_t *pstContext, NxPlayerContentInform_t *pstCurPlayerContent)
{
	if (pstCurPlayerContent == NULL)
	{
		NX_APP_Error("pstCurPlayerContent is NULL!\n");
		return ONDK_RET_INITFAIL;
	}

	switch(pstCurPlayerContent->ePlayerType)
	{
		case ePlayerType_LIVE:
			HxSTD_sprintf((char*)pstContext->szPlayTitle, "%s", (char*)"TEST TITLE...");
			break;
		case ePlayerType_PVRPLAYBACK:
			HxSTD_sprintf((char*)pstContext->szPlayTitle, "%s", (char*)"TEST TITLE...");
			break;
		case ePlayerType_TSRPLAYBACK:
			HxSTD_sprintf((char*)pstContext->szPlayTitle, "%s", (char*)"TEST TITLE...");
			break;
		case ePlayerType_MEDIAPLAY:
			HxSTD_sprintf((char*)pstContext->szPlayTitle, "%s", (char*)NX_FILEMGR_FILE_ExtractNameFromPath(pstCurPlayerContent->stStartInfo.stMediaPlay.szUrl));
			break;
		case ePlayerType_NONE:
		default:
			break;
	}

	return ONDK_RET_OK;
}

static ONDK_Result_t nx_Playbar_UpdatePlaybarPointerObj(NxPlaybar_Context_t *pstContext)
{
	ONDK_Rect_t rect;

	rect = ONDK_Rect(NX_PLAYBAR_POINTER_X_01_INITIAL + pstContext->ulPointerPosition, NX_PLAYBAR_POINTER_01_Y,
	NX_PLAYBAR_POINTER_01_W, NX_PLAYBAR_POINTER_01_H);
	ONDK_GWM_COM_Image_ChangeRect(NX_PLAYBAR_POINTER_01_ID, &rect);

	rect = ONDK_Rect(NX_PLAYBAR_POINTER_X_02_INITIAL + pstContext->ulPointerPosition, NX_PLAYBAR_POINTER_02_Y,
		NX_PLAYBAR_POINTER_02_W, NX_PLAYBAR_POINTER_02_H);
	ONDK_GWM_COM_Image_ChangeRect(NX_PLAYBAR_POINTER_02_ID, &rect);

	rect = ONDK_Rect(NX_PLAYBAR_POINTER_TIME_X_INITIAL + pstContext->ulPointerPosition, NX_PLAYBAR_POINTER_TIME_Y,
		NX_PLAYBAR_POINTER_TIME_W, NX_PLAYBAR_POINTER_TIME_H);
	ONDK_GWM_COM_Text_ChangeLocation(NX_PLAYBAR_POINTER_TIME_ID, &rect);

	return ONDK_RET_OK;
}

static ONDK_Result_t nx_Playbar_UpdatePlayInfoData(NxPlaybar_Context_t *pstContext, NxPlayerContentInform_t *pstCurPlayerContent)
{
	if (pstCurPlayerContent == NULL)
	{
		NX_APP_Error("pstCurPlayerContent is NULL!\n");
		return ONDK_RET_INITFAIL;
	}

	// Set Current Position
	if (pstContext->lSpeed != 0)
	{
		if (pstCurPlayerContent->ulPlayMaxTime == 0)
		{
			pstContext->ulPointerPosition = 0;
			nx_Playbar_GetTimeStr((HCHAR*)pstContext->szCurrentPlayTime, 0, TRUE);
		}
		else
		{
			if ((HINT32)(pstContext->ulCurrentPlayTime / 1000) != (HINT32)(pstCurPlayerContent->ulPlayTime / 1000))
			{
				pstContext->ulCurrentPlayTime = pstCurPlayerContent->ulPlayTime;
				pstContext->ulPointerPosition = (HUINT32)((NX_PLAYBAR_RECORDING_BAR_W * pstCurPlayerContent->ulPlayTime) / pstCurPlayerContent->ulPlayMaxTime);
				nx_Playbar_GetTimeStr((HCHAR*)pstContext->szCurrentPlayTime, (HINT32)pstCurPlayerContent->ulPlayTime, TRUE);
			}
		}
	}

	// Set Total Time
	if (pstContext->ePlayerType == ePlayerType_MEDIAPLAY)
	{
		if (pstContext->ulDuration != pstCurPlayerContent->ulPlayMaxTime)
		{
			if (pstCurPlayerContent->ulPlayMaxTime > 0)
			{
				pstContext->ulDuration = pstCurPlayerContent->ulPlayMaxTime;
			}
			else
			{
				pstContext->ulDuration = 0;
			}
			nx_Playbar_GetTimeStr((HCHAR*)pstContext->szTotalTime, pstContext->ulDuration, TRUE);
			ONDK_GWM_APP_InvalidateObject(NX_PLAYBAR_TIME_TOTAL_ID);
		}
	}

	return ONDK_RET_OK;
}

static ONDK_Result_t nx_Playbar_UpdatePlayIconObj(NxPlaybar_Context_t *pstContext)
{
	HCHAR	*pszImgPath = NULL;

	switch(pstContext->lSpeed)
	{
		case -64:	pszImgPath = RES_341_00_PB_FB_X64_PNG;	break;
		case -32:	pszImgPath = RES_341_00_PB_FB_X32_PNG;	break;
		case -16:	pszImgPath = RES_341_00_PB_FB_X16_PNG;	break;
		case -8:	pszImgPath = RES_341_00_PB_FB_X08_PNG;	break;
		case -4:	pszImgPath = RES_341_00_PB_FB_X04_PNG;	break;
		case -2:	pszImgPath = RES_341_00_PB_FB_X02_PNG;	break;
		case 0:		pszImgPath = RES_341_00_PB_PAUSE_PNG;	break;
		case 1:		pszImgPath = RES_341_00_PB_PLAY_PNG;	break;
		case 2:		pszImgPath = RES_341_00_PB_FF_X02_PNG;	break;
		case 4:		pszImgPath = RES_341_00_PB_FF_X04_PNG;	break;
		case 8:		pszImgPath = RES_341_00_PB_FF_X08_PNG;	break;
		case 16:	pszImgPath = RES_341_00_PB_FF_X16_PNG;	break;
		case 32:	pszImgPath = RES_341_00_PB_FF_X32_PNG;	break;
		case 64:	pszImgPath = RES_341_00_PB_FF_X64_PNG;	break;
		default:	pszImgPath = RES_341_00_PB_PLAY_PNG;	break;
	}

	ONDK_GWM_COM_Image_SetImage(NX_PLAYBAR_PLAYICON_ID, pszImgPath);
	ONDK_GWM_APP_InvalidateObject(NX_PLAYBAR_PLAYICON_ID);

	return ONDK_RET_OK;
}

static ONDK_Result_t nx_Playbar_SetHideUI(HBOOL bHide)
{
	NxPlaybar_Context_t	*pstContext = nx_playbar_PlaybarGetContext();

	if (pstContext->bHide != bHide)
	{
		ONDK_GWM_Obj_SetHide(NX_PLAYBAR_DIM_IMG_ID, bHide);
		ONDK_GWM_Obj_SetHide(NX_PLAYBAR_TITLE_ID, bHide);
		ONDK_GWM_Obj_SetHide(NX_PLAYBAR_PLAYICON_ID, bHide);
		ONDK_GWM_Obj_SetHide(NX_PLAYBAR_TIME_CURRENT_ID, bHide);
		ONDK_GWM_Obj_SetHide(NX_PLAYBAR_TIME_TOTAL_ID, bHide);
		ONDK_GWM_Obj_SetHide(NX_PLAYBAR_BTN_FR_ID, bHide);
		ONDK_GWM_Obj_SetHide(NX_PLAYBAR_BTN_PLAY_PAUSE_ID, bHide);
		ONDK_GWM_Obj_SetHide(NX_PLAYBAR_BTN_FF_ID, bHide);
		ONDK_GWM_Obj_SetHide(NX_PLAYBAR_BTN_STOP_ID, bHide);
		if (pstContext->ePlayerType == ePlayerType_MEDIAPLAY)
		{
			ONDK_GWM_Obj_SetHide(NX_PLAYBAR_BTN_SUB_ID, bHide);
		}
		ONDK_GWM_Obj_SetHide(NX_PLAYBAR_POINTER_01_ID, bHide);
		ONDK_GWM_Obj_SetHide(NX_PLAYBAR_POINTER_02_ID, bHide);
		ONDK_GWM_Obj_SetHide(NX_PLAYBAR_POINTER_TIME_ID, bHide);
		ONDK_GWM_Obj_SetHide(NX_PLAYBAR_RECORDING_BAR_ID, bHide);
		ONDK_GWM_Obj_SetHide(NX_PLAYBAR_IMG_LINE_01_ID, bHide);
		ONDK_GWM_Obj_SetHide(NX_PLAYBAR_IMG_LINE_02_ID, bHide);

		pstContext->bHide = bHide;

		if (pstContext->bHide == FALSE)
		{
			ONDK_GWM_SetTimer(NX_PLAYBAR_HIDE_TIMEOUT_ID, (HUINT32)NX_PLAYBAR_HIDE_TIMEOUT_PERIOD);
		}
		else
		{
			ONDK_GWM_KillTimer(NX_PLAYBAR_HIDE_TIMEOUT_ID);
		}
	}

	return ONDK_RET_OK;
}

static ONDK_Result_t nx_Playbar_ProcessPlay(NxPlaybar_Context_t *pstContext, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if(pstContext->lSpeed != 1)
	{
		pstContext->lSpeed = 1;
		ONDK_GWM_PostMessage(NULL, MSG_APP_PLAYER_DO_SPEED_CHANGE, 0, pstContext->lSpeed, 0, 0);
		nx_Playbar_UpdatePlayIconObj(pstContext);
		ONDK_GWM_COM_Button_ChangeImage(NX_PLAYBAR_BTN_PLAY_PAUSE_ID,RES_102_04_BTN_PAUSE_N_PNG,RES_102_04_BTN_PAUSE_C_PNG,NULL,NULL);

		return ONDK_RET_OK;
	}
	else
	{
		pstContext->lSpeed = 0;
		ONDK_GWM_PostMessage(NULL, MSG_APP_PLAYER_DO_SPEED_CHANGE, 0, pstContext->lSpeed, 0, 0);
		nx_Playbar_UpdatePlayIconObj(pstContext);
		ONDK_GWM_COM_Button_ChangeImage(NX_PLAYBAR_BTN_PLAY_PAUSE_ID,RES_102_04_BTN_PLAY_N_PNG,RES_102_04_BTN_PLAY_C_PNG,NULL,NULL);

		return ONDK_RET_OK;
	}
}

static ONDK_Result_t nx_Playbar_ProcessLeftSeek(NxPlaybar_Context_t *pstContext, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_GWM_PostMessage(NULL, MSG_APP_PLAYER_DO_LEFTSEEK, 0, p1, 0, 0);
	return ONDK_RET_OK;
}

static ONDK_Result_t nx_Playbar_ProcessRightSeek(NxPlaybar_Context_t *pstContext, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_GWM_PostMessage(NULL, MSG_APP_PLAYER_DO_RIGHTSEEK, 0, p1, 0, 0);
	return ONDK_RET_OK;
}

static ONDK_Result_t nx_Playbar_ProcessBackward(NxPlaybar_Context_t *pstContext, HINT32 p1, HINT32 p2, HINT32 p3)
{
	switch(pstContext->lSpeed)
	{
		case 0:
		case 1:
			pstContext->lSpeed = -2;
			break;
		case 2:
		case 4:
		case 8:
		case 16:
		case 32:
		case 64:
			pstContext->lSpeed = pstContext->lSpeed / 2;
			break;
		case -2:
		case -4:
		case -8:
		case -16:
		case -32:
			pstContext->lSpeed = pstContext->lSpeed * 2;
			break;
		default:
			pstContext->lSpeed = pstContext->lSpeed;
			break;
	}
	nx_Playbar_UpdatePlayIconObj(pstContext);
	if (pstContext->lSpeed != 1)
	{
		ONDK_GWM_COM_Button_ChangeImage(NX_PLAYBAR_BTN_PLAY_PAUSE_ID,RES_102_04_BTN_PLAY_N_PNG,RES_102_04_BTN_PLAY_C_PNG,NULL,NULL);
	}
	else
	{
		ONDK_GWM_COM_Button_ChangeImage(NX_PLAYBAR_BTN_PLAY_PAUSE_ID,RES_102_04_BTN_PAUSE_N_PNG,RES_102_04_BTN_PAUSE_C_PNG,NULL,NULL);
	}

	ONDK_GWM_PostMessage(NULL, MSG_APP_PLAYER_DO_SPEED_CHANGE, 0, pstContext->lSpeed, 0, 0);

	return ONDK_RET_OK;
}

static ONDK_Result_t nx_Playbar_ProcessForward(NxPlaybar_Context_t *pstContext, HINT32 p1, HINT32 p2, HINT32 p3)
{
	switch(pstContext->lSpeed)
	{
		case 0:
		case 1:
			pstContext->lSpeed = 2;
			break;
		case 2:
		case 4:
		case 8:
		case 16:
		case 32:
			pstContext->lSpeed = pstContext->lSpeed * 2;
			break;
		case -2:
			pstContext->lSpeed = 1;
			break;
		case -4:
		case -8:
		case -16:
		case -32:
		case -64:
			pstContext->lSpeed = pstContext->lSpeed / 2;
			break;
	}
	nx_Playbar_UpdatePlayIconObj(pstContext);
	if (pstContext->lSpeed != 1)
	{
		ONDK_GWM_COM_Button_ChangeImage(NX_PLAYBAR_BTN_PLAY_PAUSE_ID,RES_102_04_BTN_PLAY_N_PNG,RES_102_04_BTN_PLAY_C_PNG,NULL,NULL);
	}
	else
	{
		ONDK_GWM_COM_Button_ChangeImage(NX_PLAYBAR_BTN_PLAY_PAUSE_ID,RES_102_04_BTN_PAUSE_N_PNG,RES_102_04_BTN_PAUSE_C_PNG,NULL,NULL);
	}

	ONDK_GWM_PostMessage(NULL, MSG_APP_PLAYER_DO_SPEED_CHANGE, 0, pstContext->lSpeed, 0, 0);

	return ONDK_RET_OK;
}

static ONDK_Result_t nx_Playbar_ProcessStop(NxPlaybar_Context_t *pstContext, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_GWM_PostMessage(NULL, MSG_APP_PLAYER_DO_STOP, 0, 0, 0, 0);
	ONDK_GWM_APP_Destroy(0);
	return ONDK_RET_OK;
}

static ONDK_Result_t nx_Playbar_ProcessSubtitle(NxPlaybar_Context_t *pstContext, HINT32 p1, HINT32 p2, HINT32 p3)
{
	nx_Playbar_SetHideUI(TRUE);
#if defined(CONFIG_MW_MM_MEDIA)
	ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, "NX_PRISM_SUBTITLE_Option_Proc", APP_DEFAULT_POPUP_PRIORITY, NX_PRISM_SUBTITLE_Option_Proc, 0, p1, p2, p3);
#endif

	return ONDK_RET_OK;
}

static ONDK_Result_t nx_Playbar_ProcessOK(NxPlaybar_Context_t *pstContext, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32 ulFocus = s_stNxPlaybar_Context.ulCurFocus;
	switch(ulFocus)
	{
		case NX_PLAYBAR_BTN_FR_ID:
			nx_Playbar_ProcessBackward(pstContext, p1, p2, p3);
			break;
		case NX_PLAYBAR_BTN_PLAY_PAUSE_ID:
			nx_Playbar_ProcessPlay(pstContext, p1, p2, p3);
			break;
		case NX_PLAYBAR_BTN_FF_ID:
			nx_Playbar_ProcessForward(pstContext, p1, p2, p3);
			break;
		case NX_PLAYBAR_BTN_STOP_ID:
			nx_Playbar_ProcessStop(pstContext, p1, p2, p3);
			break;
		case NX_PLAYBAR_BTN_SUB_ID:
			nx_Playbar_ProcessSubtitle(pstContext, p1, p2, p3);
			break;

		default:
			break;
	}

	return ONDK_RET_OK;
}


static ONDK_Result_t nx_playbar_DrawDefaultUI(NxPlaybar_Context_t *pstContext)
{
	ONDK_Rect_t	rect;

	ONDK_GWM_APP_SetAppArea(0, 0, 1280, 720);

	//1 Dim image
	rect = ONDK_Rect(NX_PLAYBAR_DIM_IMG_X,NX_PLAYBAR_DIM_IMG_Y,NX_PLAYBAR_DIM_IMG_W,NX_PLAYBAR_DIM_IMG_H);
	ONDK_GWM_COM_Image_Create(NX_PLAYBAR_DIM_IMG_ID,&rect);
	ONDK_GWM_COM_Image_SetImage(NX_PLAYBAR_DIM_IMG_ID,RES_105_00_BG_BOTTOM_400_PNG);

	//1 Title
	rect = ONDK_Rect(NX_PLAYBAR_TITLE_X, NX_PLAYBAR_TITLE_Y, NX_PLAYBAR_TITLE_W, NX_PLAYBAR_TITLE_H);
	ONDK_GWM_COM_Text_Create(NX_PLAYBAR_TITLE_ID, &rect, pstContext->szPlayTitle);
	ONDK_GWM_COM_Text_SetFont(NX_PLAYBAR_TITLE_ID, eFont_SystemBold);
	ONDK_GWM_COM_Text_SetSize(NX_PLAYBAR_TITLE_ID, NX_PRISM_FONT_SIZE_26);
	ONDK_GWM_COM_Text_SetAlign(NX_PLAYBAR_TITLE_ID, TEXT_ALIGN_CENTER);
	ONDK_GWM_COM_Text_ChangeColor(NX_PLAYBAR_TITLE_ID, COL(C_T_Whistle_100), (ONDK_Color_t)NULL);

	//1 Play icon
	rect = ONDK_Rect(NX_PLAYBAR_PLAYICON_X, NX_PLAYBAR_PLAYICON_Y, NX_PLAYBAR_PLAYICON_W, NX_PLAYBAR_PLAYICON_H);
	ONDK_GWM_COM_Image_Create(NX_PLAYBAR_PLAYICON_ID, &rect);
	ONDK_GWM_COM_Image_SetImage(NX_PLAYBAR_PLAYICON_ID, RES_341_00_PB_PLAY_PNG);

	// TODO: Line 01, 02
	//rect = ONDK_Rect(NX_PLAYBAR_IMG_LINE_01_X, NX_PLAYBAR_IMG_LINE_01_Y, NX_PLAYBAR_IMG_LINE_01_W, NX_PLAYBAR_IMG_LINE_01_H);
	//ondk_gwm_com_

	//1 Time(current)
	if (pstContext->ePlayerType == ePlayerType_MEDIAPLAY)
	{
		rect = ONDK_Rect(NX_PLAYBAR_TIME_CURRENT_X2, NX_PLAYBAR_TIME_CURRENT_Y, NX_PLAYBAR_TIME_CURRENT_W, NX_PLAYBAR_TIME_CURRENT_H);
	}
	else
	{
		rect = ONDK_Rect(NX_PLAYBAR_TIME_CURRENT_X, NX_PLAYBAR_TIME_CURRENT_Y, NX_PLAYBAR_TIME_CURRENT_W, NX_PLAYBAR_TIME_CURRENT_H);
	}
	ONDK_GWM_COM_Text_Create(NX_PLAYBAR_TIME_CURRENT_ID, &rect, (HCHAR*)pstContext->szStartTime);
	ONDK_GWM_COM_Text_SetSize(NX_PLAYBAR_TIME_CURRENT_ID, NX_PRISM_FONT_SIZE_20);
	ONDK_GWM_COM_Text_SetAlign(NX_PLAYBAR_TIME_CURRENT_ID, TEXT_ALIGN_LEFT);
	ONDK_GWM_COM_Text_ChangeColor(NX_PLAYBAR_TIME_CURRENT_ID, COL(C_T_Gong02),(ONDK_Color_t)NULL);

	//1 Time(total)
	rect = ONDK_Rect(NX_PLAYBAR_TIME_TOTAL_X, NX_PLAYBAR_TIME_CURRENT_Y, NX_PLAYBAR_TIME_CURRENT_W, NX_PLAYBAR_TIME_CURRENT_H);
	ONDK_GWM_COM_Text_Create(NX_PLAYBAR_TIME_TOTAL_ID, &rect, (HCHAR*)pstContext->szTotalTime);
	ONDK_GWM_COM_Text_SetSize(NX_PLAYBAR_TIME_TOTAL_ID, NX_PRISM_FONT_SIZE_20);
	ONDK_GWM_COM_Text_SetAlign(NX_PLAYBAR_TIME_TOTAL_ID, TEXT_ALIGN_LEFT);
	ONDK_GWM_COM_Text_ChangeColor(NX_PLAYBAR_TIME_TOTAL_ID, COL(C_T_Gong02),(ONDK_Color_t)NULL);

	//1 Buttons
	if (pstContext->ePlayerType == ePlayerType_MEDIAPLAY)
	{
		rect = ONDK_Rect(NX_PLAYBAR_BTN_FR_X - (NX_PLAYBAR_BTN_SUB_W / 2), NX_PLAYBAR_BTN_FR_Y, NX_PLAYBAR_BTN_FR_W, NX_PLAYBAR_BTN_FR_H);
	}
	else
	{
		rect = ONDK_Rect(NX_PLAYBAR_BTN_FR_X, NX_PLAYBAR_BTN_FR_Y, NX_PLAYBAR_BTN_FR_W, NX_PLAYBAR_BTN_FR_H);
	}
	ONDK_GWM_COM_Button_Create(NX_PLAYBAR_BTN_FR_ID, &rect, NULL);
	ONDK_GWM_APP_SetObjectDrawMethod(NX_PLAYBAR_BTN_FR_ID, NX_PRISM_COMP_DrawButton_B1);
	ONDK_GWM_COM_Button_ChangeImage(NX_PLAYBAR_BTN_FR_ID, RES_102_04_BTN_FR_N_PNG, RES_102_04_BTN_FR_C_PNG,NULL,NULL);

	if (pstContext->ePlayerType == ePlayerType_MEDIAPLAY)
	{
		rect = ONDK_Rect(NX_PLAYBAR_BTN_PLAY_PAUSE_X - (NX_PLAYBAR_BTN_SUB_W / 2), NX_PLAYBAR_BTN_PLAY_PAUSE_Y, NX_PLAYBAR_BTN_PLAY_PAUSE_W, NX_PLAYBAR_BTN_PLAY_PAUSE_H);
	}
	else
	{
		rect = ONDK_Rect(NX_PLAYBAR_BTN_PLAY_PAUSE_X, NX_PLAYBAR_BTN_PLAY_PAUSE_Y, NX_PLAYBAR_BTN_PLAY_PAUSE_W, NX_PLAYBAR_BTN_PLAY_PAUSE_H);
	}
	ONDK_GWM_COM_Button_Create(NX_PLAYBAR_BTN_PLAY_PAUSE_ID, &rect, NULL);
	ONDK_GWM_APP_SetObjectDrawMethod(NX_PLAYBAR_BTN_PLAY_PAUSE_ID, NX_PRISM_COMP_DrawButton_B1);
	ONDK_GWM_COM_Button_ChangeImage(NX_PLAYBAR_BTN_PLAY_PAUSE_ID, RES_102_04_BTN_PAUSE_N_PNG, RES_102_04_BTN_PAUSE_C_PNG,NULL,NULL);

	if (pstContext->ePlayerType == ePlayerType_MEDIAPLAY)
	{
		rect = ONDK_Rect(NX_PLAYBAR_BTN_FF_X - (NX_PLAYBAR_BTN_SUB_W / 2), NX_PLAYBAR_BTN_FF_Y, NX_PLAYBAR_BTN_FF_W, NX_PLAYBAR_BTN_FF_H);
	}
	else
	{
		rect = ONDK_Rect(NX_PLAYBAR_BTN_FF_X, NX_PLAYBAR_BTN_FF_Y, NX_PLAYBAR_BTN_FF_W, NX_PLAYBAR_BTN_FF_H);
	}
	ONDK_GWM_COM_Button_Create(NX_PLAYBAR_BTN_FF_ID, &rect, NULL);
	ONDK_GWM_APP_SetObjectDrawMethod(NX_PLAYBAR_BTN_FF_ID, NX_PRISM_COMP_DrawButton_B1);
	ONDK_GWM_COM_Button_ChangeImage(NX_PLAYBAR_BTN_FF_ID, RES_102_04_BTN_FF_N_PNG, RES_102_04_BTN_FF_C_PNG,NULL,NULL);

	if (pstContext->ePlayerType == ePlayerType_MEDIAPLAY)
	{
		rect = ONDK_Rect(NX_PLAYBAR_BTN_STOP_X - (NX_PLAYBAR_BTN_SUB_W / 2), NX_PLAYBAR_BTN_STOP_Y, NX_PLAYBAR_BTN_STOP_W, NX_PLAYBAR_BTN_STOP_H);
	}
	else
	{
		rect = ONDK_Rect(NX_PLAYBAR_BTN_STOP_X, NX_PLAYBAR_BTN_STOP_Y, NX_PLAYBAR_BTN_STOP_W, NX_PLAYBAR_BTN_STOP_H);
	}
	ONDK_GWM_COM_Button_Create(NX_PLAYBAR_BTN_STOP_ID, &rect, NULL);
	ONDK_GWM_COM_Button_ChangeTitle(NX_PLAYBAR_BTN_STOP_ID,"STOP");
	ONDK_GWM_COM_Button_SetTitleSize(NX_PLAYBAR_BTN_STOP_ID, NX_PRISM_FONT_SIZE_20);
	ONDK_GWM_COM_Button_SetFont(NX_PLAYBAR_BTN_STOP_ID,eFont_SystemBold);
	ONDK_GWM_COM_Button_SetAlign(NX_PLAYBAR_BTN_STOP_ID, TEXT_ALIGN_CENTER);
	ONDK_GWM_APP_SetObjectDrawMethod(NX_PLAYBAR_BTN_STOP_ID, NX_PRISM_COMP_DrawButton_B1);

	if (pstContext->ePlayerType == ePlayerType_MEDIAPLAY)
	{
		rect = ONDK_Rect(NX_PLAYBAR_BTN_SUB_X, NX_PLAYBAR_BTN_SUB_Y, NX_PLAYBAR_BTN_SUB_W, NX_PLAYBAR_BTN_SUB_H);
		ONDK_GWM_COM_Button_Create(NX_PLAYBAR_BTN_SUB_ID, &rect, NULL);
		ONDK_GWM_COM_Button_ChangeTitle(NX_PLAYBAR_BTN_SUB_ID,"SUB");
		ONDK_GWM_COM_Button_SetTitleSize(NX_PLAYBAR_BTN_SUB_ID, NX_PRISM_FONT_SIZE_20);
		ONDK_GWM_COM_Button_SetFont(NX_PLAYBAR_BTN_SUB_ID,eFont_SystemBold);
		ONDK_GWM_COM_Button_SetAlign(NX_PLAYBAR_BTN_SUB_ID, TEXT_ALIGN_CENTER);
		ONDK_GWM_APP_SetObjectDrawMethod(NX_PLAYBAR_BTN_SUB_ID, NX_PRISM_COMP_DrawButton_B1);
	}

	//1 Bar C1
	rect = ONDK_Rect(NX_PLAYBAR_RECORDING_BAR_X, NX_PLAYBAR_RECORDING_BAR_Y, NX_PLAYBAR_RECORDING_BAR_W, NX_PLAYBAR_RECORDING_BAR_H);
	ONDK_GWM_COM_Rect_Create(NX_PLAYBAR_RECORDING_BAR_ID,&rect,COL(C_G_Whistle_100));

	//1 Pointer 01, 02

	rect = ONDK_Rect(NX_PLAYBAR_POINTER_X_01_INITIAL + pstContext->ulPointerPosition, NX_PLAYBAR_POINTER_01_Y,
		NX_PLAYBAR_POINTER_01_W, NX_PLAYBAR_POINTER_01_H);
	ONDK_GWM_COM_Image_Create(NX_PLAYBAR_POINTER_01_ID, &rect);
	ONDK_GWM_COM_Image_SetImage(NX_PLAYBAR_POINTER_01_ID, RES_340_00_PB_PLAYER_POINTER01_PNG);

	rect = ONDK_Rect(NX_PLAYBAR_POINTER_X_02_INITIAL + pstContext->ulPointerPosition, NX_PLAYBAR_POINTER_02_Y,
		NX_PLAYBAR_POINTER_02_W, NX_PLAYBAR_POINTER_02_H);
	ONDK_GWM_COM_Image_Create(NX_PLAYBAR_POINTER_02_ID, &rect);
	ONDK_GWM_COM_Image_SetImage(NX_PLAYBAR_POINTER_02_ID, RES_340_00_PB_PLAYER_POINTER03_N_PNG);

	//1 Pointer time
	rect = ONDK_Rect(NX_PLAYBAR_POINTER_TIME_X_INITIAL+pstContext->ulPointerPosition, NX_PLAYBAR_POINTER_TIME_Y,
	NX_PLAYBAR_POINTER_TIME_W, NX_PLAYBAR_POINTER_TIME_H);
	ONDK_GWM_COM_Text_Create(NX_PLAYBAR_POINTER_TIME_ID, &rect, pstContext->szCurrentPlayTime);
	ONDK_GWM_COM_Text_SetSize(NX_PLAYBAR_POINTER_TIME_ID, NX_PRISM_FONT_SIZE_20);
	ONDK_GWM_COM_Text_SetFont(NX_PLAYBAR_POINTER_TIME_ID, eFont_SystemBold);
	ONDK_GWM_COM_Text_SetAlign(NX_PLAYBAR_POINTER_TIME_ID, TEXT_ALIGN_CENTER);
	ONDK_GWM_COM_Text_ChangeColor(NX_PLAYBAR_POINTER_TIME_ID, COL(C_T_Whistle_100),(ONDK_Color_t)NULL);


	ONDK_GWM_Obj_SetFocus(pstContext->ulCurFocus);

	return ONDK_RET_OK;
}

static ONDK_Result_t nx_Playbar_MsgGwmCreate(NxPlaybar_Context_t *pstContext, HINT32 p1, HINT32 p2, HINT32 p3)
{
	NxPlayerContentInform_t *contentInform = (NxPlayerContentInform_t*)p1;

	if(pstContext == NULL)
	{
		ONDK_GWM_APP_Destroy(0);
		return ONDK_RET_INITFAIL;
	}

	ONDK_GWM_SetTimer(NX_PLAYBAR_HIDE_TIMEOUT_ID, (HUINT32)NX_PLAYBAR_HIDE_TIMEOUT_PERIOD);

	pstContext->ulCurFocus = NX_PLAYBAR_BTN_PLAY_PAUSE_ID;
	pstContext->lSpeed = 1;
	pstContext->ePlayerType = contentInform->ePlayerType;
	pstContext->ulDuration = 0;
	pstContext->ulCurrentPlayTime = 0;
	pstContext->bHide = FALSE;
	if (pstContext->ePlayerType == ePlayerType_MEDIAPLAY)
	{
		nx_Playbar_GetTimeStr((HCHAR*)pstContext->szStartTime, pstContext->ulDuration, TRUE);
		nx_Playbar_GetTimeStr((HCHAR*)pstContext->szTotalTime, pstContext->ulDuration, TRUE);
	}
	else
	{
		HxSTD_sprintf((char*)pstContext->szStartTime, "00:00");
		HxSTD_sprintf((char*)pstContext->szTotalTime, "00:00");
	}

	nx_Playbar_UpdatePlayInfoData(pstContext, contentInform);
	nx_Playbar_UpdatePlayTitleStr(pstContext, contentInform);

	nx_playbar_DrawDefaultUI(pstContext);

	return ONDK_RET_OK;
}

static ONDK_Result_t nx_Playbar_MsgGwmDestroy(NxPlaybar_Context_t *pstContext, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_GWM_KillTimer(NX_PLAYBAR_HIDE_TIMEOUT_ID);

	ONDK_GWM_PostMessage(NULL, MSG_APP_PLAYER_DESTROY, 0, 0, 0, 0);

	HxSTD_memset(pstContext, 0x00, sizeof(NxPlaybar_Context_t));

	return ONDK_RET_OK;
}

static ONDK_Result_t nx_Playbar_MsgGwmKeyDown(NxPlaybar_Context_t *pstContext, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32 lKeyId = p1;

	if(pstContext == NULL)
	{
		NX_APP_Error("Error!!! Please check!! \n");
		return ONDK_RET_FAIL;
	}

	NX_PRISM_UTIL_KeyConv_UiCommon(&lKeyId);

	switch(lKeyId)
	{
		case KEY_BACK:
			if (pstContext->bHide == TRUE)
			{
				nx_Playbar_ProcessStop(pstContext, p1, p2, p3);
			}
			else
			{
				nx_Playbar_SetHideUI(TRUE);
			}
			return ONDK_MESSAGE_BREAK;

		case KEY_MENU:
			nx_Playbar_SetHideUI(TRUE);
			return ONDK_MESSAGE_PASS;

		case KEY_EXIT:
			nx_Playbar_ProcessStop(pstContext, p1, p2, p3);
			return ONDK_MESSAGE_BREAK;

		case KEY_OK:
			if (pstContext->bHide == FALSE)
			{
				ONDK_GWM_ResetTimer(NX_PLAYBAR_HIDE_TIMEOUT_ID);
				nx_Playbar_ProcessOK(pstContext, p1, p2, p3);
			}
			return ONDK_MESSAGE_BREAK;

		case KEY_SPECIAL_FASTFORWARD:
			if (pstContext->bHide == FALSE)
			{
				ONDK_GWM_ResetTimer(NX_PLAYBAR_HIDE_TIMEOUT_ID);
			}
			nx_Playbar_ProcessForward(pstContext, p1, p2, p3);
			nx_Paybar_MoveFocusToBtn(pstContext, NX_PLAYBAR_BTN_FF_ID, p2, p3);
			nx_Playbar_SetHideUI(FALSE);
			return ONDK_MESSAGE_BREAK;

		case KEY_SPECIAL_REWIND:
			if (pstContext->bHide == FALSE)
			{
				ONDK_GWM_ResetTimer(NX_PLAYBAR_HIDE_TIMEOUT_ID);
			}
			nx_Playbar_ProcessBackward(pstContext, p1, p2, p3);
			nx_Paybar_MoveFocusToBtn(pstContext, NX_PLAYBAR_BTN_FR_ID, p2, p3);
			nx_Playbar_SetHideUI(FALSE);
			return ONDK_MESSAGE_BREAK;

		case KEY_PLAY:
			if (pstContext->bHide == FALSE)
			{
				ONDK_GWM_ResetTimer(NX_PLAYBAR_HIDE_TIMEOUT_ID);
			}
			pstContext->lSpeed = 0; // will be changed to 1
			nx_Playbar_ProcessPlay(pstContext, p1, p2, p3);
			nx_Paybar_MoveFocusToBtn(pstContext, NX_PLAYBAR_BTN_PLAY_PAUSE_ID, p2, p3);
			nx_Playbar_SetHideUI(FALSE);
			return ONDK_MESSAGE_BREAK;

		case KEY_PAUSE:
			if (pstContext->bHide == FALSE)
			{
				ONDK_GWM_ResetTimer(NX_PLAYBAR_HIDE_TIMEOUT_ID);
			}
			pstContext->lSpeed = 1; // will be changed to 0
			nx_Playbar_ProcessPlay(pstContext, p1, p2, p3);
			nx_Paybar_MoveFocusToBtn(pstContext, NX_PLAYBAR_BTN_PLAY_PAUSE_ID, p2, p3);
			nx_Playbar_SetHideUI(FALSE);
			return ONDK_MESSAGE_BREAK;

		case KEY_STOP:
			nx_Playbar_ProcessStop(pstContext, p1, p2, p3);
			return ONDK_MESSAGE_BREAK;

		case KEY_ARROWUP:
			if (pstContext->bHide == FALSE)
			{
				ONDK_GWM_ResetTimer(NX_PLAYBAR_HIDE_TIMEOUT_ID);
				nx_Paybar_MoveFocusToPointer(pstContext, p1, p2, p3);
			}
			return ONDK_MESSAGE_BREAK;

		case KEY_ARROWDOWN:
			if (pstContext->bHide == FALSE)
			{
				ONDK_GWM_ResetTimer(NX_PLAYBAR_HIDE_TIMEOUT_ID);
				nx_Paybar_MoveFocusToBtn(pstContext, 0, p2, p3);
			}
			return ONDK_MESSAGE_BREAK;

		case KEY_ARROWLEFT:
			if (pstContext->bHide == TRUE)
			{
				nx_Paybar_MoveFocusToPointer(pstContext, p1, p2, p3);
				nx_Playbar_SetHideUI(FALSE);
			}
			else
			{
				ONDK_GWM_ResetTimer(NX_PLAYBAR_HIDE_TIMEOUT_ID);
			}
			nx_Paybar_MoveFocusLeft(pstContext, p1, p2, p3);
			return ONDK_MESSAGE_BREAK;

		case KEY_ARROWRIGHT:
			if (pstContext->bHide == TRUE)
			{
				nx_Paybar_MoveFocusToPointer(pstContext, p1, p2, p3);
				nx_Playbar_SetHideUI(FALSE);
			}
			else
			{
				ONDK_GWM_ResetTimer(NX_PLAYBAR_HIDE_TIMEOUT_ID);
			}
			nx_Paybar_MoveFocusRight(pstContext, p1, p2, p3);
			return ONDK_MESSAGE_BREAK;

		case KEY_OPT:
			nx_Playbar_SetHideUI(TRUE);
			ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, "NX_PRISM_OPTION_Proc", APP_DEFAULT_POPUP_PRIORITY, NX_PRISM_OPTION_Proc, 0, p1, p2, p3);
			return ONDK_MESSAGE_BREAK;

		case KEY_VOLUMEUP:
		case KEY_VOLUMEDOWN:
		case KEY_STANDBY:
		default:
			return	ONDK_MESSAGE_PASS;
	}
}

static ONDK_Result_t	nx_Playbar_MsgGwmTimer(NxPlaybar_Context_t *pstContext, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (p1 == NX_PLAYBAR_HIDE_TIMEOUT_ID)
	{
		nx_Playbar_SetHideUI(TRUE);
	}

	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}

static ONDK_Result_t nx_Playbar_MsgAppPlayStart(NxPlaybar_Context_t *pstContext, HINT32 p1, HINT32 p2, HINT32 p3)
{
	NxPlayerContentInform_t *contentInform = (NxPlayerContentInform_t*)p1;

	pstContext->ePlayerType = contentInform->ePlayerType;

	// Change Title
	nx_Playbar_UpdatePlayTitleStr(pstContext, contentInform);
	ONDK_GWM_APP_InvalidateObject(NX_PLAYBAR_TITLE_ID);

	// Set Total time
	pstContext->ulDuration = 0;
	if (pstContext->ePlayerType == ePlayerType_MEDIAPLAY)
	{
		nx_Playbar_GetTimeStr((HCHAR*)pstContext->szStartTime, pstContext->ulDuration, TRUE);
		nx_Playbar_GetTimeStr((HCHAR*)pstContext->szTotalTime, pstContext->ulDuration, TRUE);
	}
	else
	{
		HxSTD_sprintf((char*)pstContext->szStartTime, "00:00");
		HxSTD_sprintf((char*)pstContext->szTotalTime, "00:00");
	}

	// Set Pointer to 0
	pstContext->ulPointerPosition = 0;
	pstContext->lSpeed = 1;
	pstContext->ulCurrentPlayTime = 0;
	nx_Playbar_UpdatePlayIconObj(pstContext);
	nx_Playbar_GetTimeStr((HCHAR*)pstContext->szCurrentPlayTime, 0, TRUE);
	nx_Playbar_UpdatePlaybarPointerObj(pstContext);

	return ONDK_MESSAGE_BREAK;
}

static ONDK_Result_t nx_Playbar_MsgAppPlayStop(NxPlaybar_Context_t *pstContext, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_GWM_PostMessage(NULL, MSG_APP_PLAYER_DO_STOP, 0, 0, 0, 0);
	ONDK_GWM_APP_Destroy(0);

	return ONDK_MESSAGE_BREAK;
}

static ONDK_Result_t nx_Playbar_MsgAppPlayInfoUpdated(NxPlaybar_Context_t *pstContext, HINT32 p1, HINT32 p2, HINT32 p3)
{
	NxPlayerContentInform_t	*contentInform = (NxPlayerContentInform_t*)p1;

	if (pstContext->lSpeed != contentInform->nPlaySpeed)
	{
		pstContext->lSpeed = contentInform->nPlaySpeed;
		nx_Playbar_UpdatePlayIconObj(pstContext);
		if (pstContext->lSpeed != 1)
		{
			ONDK_GWM_COM_Button_ChangeImage(NX_PLAYBAR_BTN_PLAY_PAUSE_ID,RES_102_04_BTN_PLAY_N_PNG,RES_102_04_BTN_PLAY_C_PNG,NULL,NULL);
		}
		else
		{
			ONDK_GWM_COM_Button_ChangeImage(NX_PLAYBAR_BTN_PLAY_PAUSE_ID,RES_102_04_BTN_PAUSE_N_PNG,RES_102_04_BTN_PAUSE_C_PNG,NULL,NULL);
		}
	}

	nx_Playbar_UpdatePlayInfoData(pstContext, contentInform);
	if (pstContext->lSpeed != 0)
	{
		nx_Playbar_UpdatePlaybarPointerObj(pstContext);
	}

	return ONDK_MESSAGE_BREAK;
}

/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/

ONDK_Result_t		NX_PRISM_PLAYBAR_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t		eResult	= ONDK_RET_OK;
	//NxVolume_Context_t *pstContents = nx_volume_VolumeGetContents();
	NxPlaybar_Context_t	*pstContext = nx_playbar_PlaybarGetContext();
	if(pstContext == NULL)
	{
		eResult = ONDK_RET_FAIL;
	}

	switch (lMessage)
	{
		case MSG_GWM_CREATE:
			eResult = nx_Playbar_MsgGwmCreate(pstContext, p1, p2, p3);
			break;
		case MSG_GWM_KEYUP:
			break;
		case MSG_GWM_KEYDOWN:
			eResult = nx_Playbar_MsgGwmKeyDown(pstContext, p1, p2, p3);
			break;
		case MSG_GWM_DESTROY:
			eResult = nx_Playbar_MsgGwmDestroy(pstContext, p1, p2, p3);
			break;
		case MSG_GWM_TIMER:
			eResult = nx_Playbar_MsgGwmTimer(pstContext, p1, p2, p3);
			break;
		case MSG_GWM_HAPI_MESSAGE:
			break;
		case MSG_GWM_OBJ_FOCUS_CHANGED:
			break;
		case MSG_GWM_CLICKED:
			break;
		case MSG_GWM_SCROLLLEFT:
			break;
		case MSG_GWM_SCROLLRIGHT:
			break;
		case MSG_APP_PLAYMGR_PLAY_START:
			eResult = nx_Playbar_MsgAppPlayStart(pstContext, p1, p2, p3);
			break;
		case MSG_APP_PLAYMGR_PLAY_STOP:
			eResult = nx_Playbar_MsgAppPlayStop(pstContext, p1, p2, p3);
			break;
		case MSG_APP_PLAYMGR_PLAYINFO_UPDATED:
			eResult = nx_Playbar_MsgAppPlayInfoUpdated(pstContext, p1, p2, p3);
			break;
		default:
			break;
	}

	if(eResult != ONDK_RET_OK)
	{
		return	eResult;
	}

	return	ONDK_GWM_ProcessMessageDefault(lMessage, handle, p1, p2, p3);
}

/******************************************************************************/
#define _________End_Of_File____________________________________________________
/******************************************************************************/

