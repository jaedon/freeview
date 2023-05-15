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
 * @file	  		nx_prism_audio_ui.c
 *
 * 	Description:  													\n
 *
 * @author															\n
 * @date															\n
 * @Attention		Copyright (c) 2013 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/


/******************************************************************************/
#define _________Private_Include______________________________________________
/******************************************************************************/
#include <nx_common.h>
#include <nx_core.h>

#include <nx_prism_app.h>
#include <nx_prism_ui.h>
#include <nx_prism_component.h>


/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/

/*******************************************************************/
/********************      Definitions    **************************/
/*******************************************************************/

#define	NX_PRISM_AUDIO_UI_ITEM_ID					eMenuFrame_Window_AudioUI

#define	NX_PRISM_AUDIO_UI_EXPIRE_TIMER_ID			(NX_PRISM_AUDIO_UI_ITEM_ID + 1)
#define	NX_PRISM_AUDIO_UI_EXPIRE_TIME				(3 * NX_COMMON_TIME_SECOND)

#define	NX_PRISM_AUDIO_UI_WINDOW_X					290
#define	NX_PRISM_AUDIO_UI_WINDOW_Y					0
#define	NX_PRISM_AUDIO_UI_WINDOW_W					700
#define	NX_PRISM_AUDIO_UI_WINDOW_H					101

#define	NX_PRISM_AUDIO_UI_FRAME_ID					(NX_PRISM_AUDIO_UI_ITEM_ID + 2)
#define	NX_PRISM_AUDIO_UI_FRAME_X					0
#define	NX_PRISM_AUDIO_UI_FRAME_Y					0
#define	NX_PRISM_AUDIO_UI_FRAME_W					NX_PRISM_AUDIO_UI_WINDOW_W
#define	NX_PRISM_AUDIO_UI_FRAME_H					NX_PRISM_AUDIO_UI_WINDOW_H

#define	NX_PRISM_AUDIO_UI_TEXT_ITEM_ID				(NX_PRISM_AUDIO_UI_ITEM_ID + 3)
#define	NX_PRISM_AUDIO_UI_TEXT_ITEM_X				0
#define	NX_PRISM_AUDIO_UI_TEXT_ITEM_Y				49
#define	NX_PRISM_AUDIO_UI_TEXT_ITEM_W				NX_PRISM_AUDIO_UI_FRAME_W
#define	NX_PRISM_AUDIO_UI_TEXT_ITEM_H				(NX_PRISM_FONT_SIZE_22 + NX_PRISM_FONT_VSPACE)

#define	NX_PRISM_AUDIO_UI_ICON_ID					(NX_PRISM_AUDIO_UI_ITEM_ID + 4)
#define	NX_PRISM_AUDIO_UI_ICON_X					0
#define	NX_PRISM_AUDIO_UI_ICON_Y					50
#define	NX_PRISM_AUDIO_UI_ICON_W					33
#define	NX_PRISM_AUDIO_UI_ICON_H					25

#define	NX_PRISM_AUDIO_UI_LTYPE_ID					(NX_PRISM_AUDIO_UI_ITEM_ID + 5)
#define	NX_PRISM_AUDIO_UI_LTYPE_ICON_W				38
#define	NX_PRISM_AUDIO_UI_LTYPE_ICON_H				32
#define	NX_PRISM_AUDIO_UI_LTYPE_ICON_MG				10
#define	NX_PRISM_AUDIO_UI_LTYPE_X					18
#define	NX_PRISM_AUDIO_UI_LTYPE_Y					47
#define	NX_PRISM_AUDIO_UI_LTYPE_W					(60 + NX_PRISM_AUDIO_UI_LTYPE_ICON_W + NX_PRISM_AUDIO_UI_LTYPE_ICON_MG)
#define	NX_PRISM_AUDIO_UI_LTYPE_H					NX_PRISM_AUDIO_UI_LTYPE_ICON_H

#define	NX_PRISM_AUDIO_UI_RTYPE_ID					(NX_PRISM_AUDIO_UI_ITEM_ID + 6)
#define	NX_PRISM_AUDIO_UI_RTYPE_ICON_W				38
#define	NX_PRISM_AUDIO_UI_RTYPE_ICON_H				32
#define	NX_PRISM_AUDIO_UI_RTYPE_ICON_MG				10
#define	NX_PRISM_AUDIO_UI_RTYPE_X					578
#define	NX_PRISM_AUDIO_UI_RTYPE_Y					47
#define	NX_PRISM_AUDIO_UI_RTYPE_W					(60 + NX_PRISM_AUDIO_UI_RTYPE_ICON_W + NX_PRISM_AUDIO_UI_RTYPE_ICON_MG)
#define	NX_PRISM_AUDIO_UI_RTYPE_H					NX_PRISM_AUDIO_UI_RTYPE_ICON_H

#define	NX_PRISM_AUDIO_UI_OFF_ITEM_NUM				1
#define	NX_PRISM_AUDIO_UI_OFF_ITEM_INDEX			0
#define	NX_PRISM_AUDIO_UI_OFF_ITEM_TYPE				0xFF

#define	NX_PRISM_AUDIO_UI_IMG_MG					10
#define	NX_PRISM_AUDIO_UI_IMG_W						NX_PRISM_AUDIO_UI_ICON_W
#define	NX_PRISM_AUDIO_UI_TEXT_MG					10


/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/

typedef struct NX_AUDIO_UI_Contents
{
	HUINT32 					ulUserSelectIdx;
	HUINT32						ulViewId;
	OxMediaPlay_MediaType_e		eMediaType;
	HBOOL						bDualMonoFullName;
	HCHAR						szStrBuf[NX_TEXT_SIZE_64];
	NX_Media_AUDIOList_t		stAudioList;
	NX_MediaPlay_AudioOtherComp_t	stAudioOthInfo[NX_CORE_AUDIO_MAX_NUM];		// Used Dual Mono full Name Mode
} Nx_AudioUIContent_t;


/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/
static Nx_AudioUIContent_t			s_stAudioUiContent;


/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/

static ONDK_Result_t	nx_audioUi_ResponseDialogProc(HINT32 lMessage, Handle_t hHandle, HINT32 lParam1, HINT32 lParam2, HINT32 lParam3);
static void				nx_audioUi_CreateResponseDialouge(void);

static inline Nx_AudioUIContent_t* nx_audioUi_GetContents(void);

static ONDK_Result_t	nx_audioUi_ActionChangeAudioWithDualMonoFullName(Nx_AudioUIContent_t *pstContents);
static ONDK_Result_t	nx_audioUi_SetAudioOtherInfo(Nx_AudioUIContent_t *pstContents, HUINT32 ulIndex, HUINT32 ulOrgIndex, DxStereoSelect_e eAudioType);
static ONDK_Result_t	nx_audioUi_MakeAudioListWithDualMonoFullName(Nx_AudioUIContent_t *pstContents, NX_Media_AUDIOList_t *stComponetAudio);

static ONDK_Result_t	nx_audioUi_ActionChangeAudio(Nx_AudioUIContent_t *pstContents);
static ONDK_Result_t	nx_audioUi_MakeAudioList(Nx_AudioUIContent_t *pstContents, NX_Media_AUDIOList_t *stComponetAudio);

static ONDK_Result_t	nx_audioUi_LoadItemsValue(Nx_AudioUIContent_t *pstContents);

static ONDK_Result_t 	nx_audioUi_UpdateUiObject(Nx_AudioUIContent_t *pstContents);
static ONDK_Result_t 	nx_audioUi_SetUiObject(Nx_AudioUIContent_t *pstContents);
static ONDK_Result_t	nx_audioUi_Initialize(Nx_AudioUIContent_t *pstContents);

static ONDK_Result_t	nx_audioUi_MsgGwmCreate(Nx_AudioUIContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	nx_audioUi_MsgGwmDestroy(Nx_AudioUIContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_audioUi_MsgGwmHapiMsg(Nx_AudioUIContent_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_audioUi_MsgGwmKeyDown(Nx_AudioUIContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	nx_audioUi_MsgGwmTimer(Nx_AudioUIContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);


/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/

static ONDK_Result_t	nx_audioUi_ResponseDialogProc(HINT32 lMessage, Handle_t hHandle, HINT32 lParam1, HINT32 lParam2, HINT32 lParam3)
{
	switch (lMessage)
	{
		case MSG_GWM_KEYDOWN:
			NX_PRISM_UTIL_KeyConv_UiCommon (&lParam1);
			switch (lParam1)
			{
				case KEY_RED:
				case KEY_GREEN:
				case KEY_YELLOW:
				case KEY_BLUE:
				case KEY_ARROWUP :
				case KEY_ARROWDOWN:
				case KEY_TVRADIO:
				case KEY_EXT_AV:
				case KEY_COMBO:
				case KEY_TEXT:
				case KEY_TTX_HOLD:
				case KEY_TTX_SUBPAGE:
				case KEY_AUDIO:
				case KEY_SUBTITLE:
				case KEY_WIDE:
				case KEY_3D:
				case KEY_INFO:
				case KEY_BACK:
				case KEY_EXT_AV_PLUS:
				case KEY_EXT_AV_MINUS:
					return	ONDK_MESSAGE_BREAK;
				default:
					break;
			}
			break;

		case MSG_APP_SYSTEM_SVC_CHANGED :
			ONDK_GWM_APP_Destroy(0);
			return	ONDK_MESSAGE_PASS;

		default :
			break;
	}

	return	NX_PRISM_DIALOGUE_ResponseMessage_Proc(lMessage, hHandle, lParam1, lParam2, lParam3);
}


static void		nx_audioUi_CreateResponseDialouge(void)
{
	ONDK_GWM_APP_Create (ONDK_UPPER_WINDOW, "nx_audioUi_ResponseDialogProc", APP_DEFAULT_PRIORITY,
						nx_audioUi_ResponseDialogProc, 0, (HINT32)ONDK_GetString(RES_NO_AUDIO_DATA_ID), 0, 0);
}


static inline Nx_AudioUIContent_t* nx_audioUi_GetContents(void)
{
	return	&s_stAudioUiContent;
}


static ONDK_Result_t	nx_audioUi_SetAudioOtherInfo(Nx_AudioUIContent_t *pstContents, HUINT32 ulIndex, HUINT32 ulOrgIndex, DxStereoSelect_e eAudioType)
{
	pstContents->stAudioOthInfo[ulIndex].eAudioType = eAudioType;
	pstContents->stAudioOthInfo[ulIndex].ulOrgIndex= ulOrgIndex;

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_audioUi_ActionChangeAudioWithDualMonoFullName(Nx_AudioUIContent_t *pstContents)
{
	HUINT32				ulSelectIdx = 0, ulOrgIndex = 0;
	DxStereoSelect_e	eAudioType = eDxSTEREO_SELECT_STEREO;

	NX_APP_Trace("(+) \n");

	ulSelectIdx = (++pstContents->ulUserSelectIdx) % pstContents->stAudioList.ulAudioNum;
	pstContents->ulUserSelectIdx = ulSelectIdx;

	NX_PRISM_UTIL_MakeAudioString(pstContents->stAudioList, pstContents->ulUserSelectIdx, pstContents->szStrBuf, pstContents->bDualMonoFullName);
	nx_audioUi_UpdateUiObject(pstContents);

	ulOrgIndex = pstContents->stAudioOthInfo[pstContents->ulUserSelectIdx].ulOrgIndex;
	NX_MEDIA_PLAY_SetComponentIndex(pstContents->ulViewId, eOxMP_COMPONENT_AUDIO, ulOrgIndex);

	eAudioType = pstContents->stAudioOthInfo[pstContents->ulUserSelectIdx].eAudioType;
	NX_SYSTEM_SetSoundMode(eAudioType);

	NX_APP_Info(" Change Audio dual mono Full Name Set : cur[%d], component Idx[%d], Audio Type : [%d]! \n", pstContents->ulUserSelectIdx, ulOrgIndex, eAudioType);

	NX_APP_Trace("(-) \n");

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_audioUi_MakeAudioListWithDualMonoFullName(Nx_AudioUIContent_t *pstContents, NX_Media_AUDIOList_t *stComponetAudio)
{
	HUINT32 	i = 0, ulCnt = 0, ulAddIndex = 0;
	OxMediaPlay_AudioComp_t *pListInfo = NULL;


	NX_APP_Trace("(+) \n");

	ulCnt = stComponetAudio->ulAudioNum;
	if (stComponetAudio->ulAudioNum >= NX_CORE_AUDIO_MAX_NUM)
	{
		ulCnt = NX_CORE_AUDIO_MAX_NUM ;
	}

	/* Add Audio Item */
	ulAddIndex = 0;
	for (i = 0; i < ulCnt; i++)
	{
		if (ulAddIndex >= NX_CORE_AUDIO_MAX_NUM)
		{
			NX_APP_Error("Error!!! Alreay Full itme due to dual mono ulAddIndex:(%d) \n", ulAddIndex);
			break;
		}

		if (stComponetAudio->pstAudioInfo[i].nLangNum > 1)
		{
			// Add 1st Audio Language
			pListInfo = &pstContents->stAudioList.pstAudioInfo[ulAddIndex];
			HxSTD_memcpy(pListInfo, &stComponetAudio->pstAudioInfo[i], sizeof(OxMediaPlay_AudioComp_t));

			HxSTD_memset(pListInfo->astLangInfo[1].szLangCode, 0, sizeof(HUINT8)*DxLANG_CODE_LEN);
			pListInfo->astLangInfo[1].szLangCode[0] = '\0';
			pListInfo->nLangNum = 1;
			pListInfo->eMode = eOxMP_AUDMODE_DUALMONO;
			nx_audioUi_SetAudioOtherInfo(pstContents, ulAddIndex, i, eDxSTEREO_SELECT_MONOLEFT);

			ulAddIndex++;

			// Add 2nd Audio Language
			pListInfo = &pstContents->stAudioList.pstAudioInfo[ulAddIndex];
			HxSTD_memcpy(pListInfo, &stComponetAudio->pstAudioInfo[i], sizeof(OxMediaPlay_AudioComp_t));

			HxSTD_StrNCpy((HCHAR*)pListInfo->astLangInfo[0].szLangCode, (HCHAR*)stComponetAudio->pstAudioInfo[i].astLangInfo[1].szLangCode, sizeof(HUINT8)*DxLANG_CODE_LEN);
			HxSTD_memset(pListInfo->astLangInfo[1].szLangCode, 0, sizeof(HUINT8)*DxLANG_CODE_LEN);
			pListInfo->astLangInfo[1].szLangCode[0] = '\0';
			pListInfo->nLangNum = 1;
			pListInfo->eMode = eOxMP_AUDMODE_DUALMONO;
			nx_audioUi_SetAudioOtherInfo(pstContents, ulAddIndex, i, eDxSTEREO_SELECT_MONORIGHT);

			ulAddIndex++;
		}
		else
		{
			pListInfo = &pstContents->stAudioList.pstAudioInfo[ulAddIndex];
			HxSTD_memcpy(pListInfo, &stComponetAudio->pstAudioInfo[i], sizeof(OxMediaPlay_AudioComp_t));
			nx_audioUi_SetAudioOtherInfo(pstContents, ulAddIndex, i, eDxSTEREO_SELECT_STEREO);

			ulAddIndex++;
		}
	}

	/* Add audio Count*/
	pstContents->stAudioList.ulAudioNum = ulAddIndex;

	NX_APP_Trace("(-) \n");

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_audioUi_ActionChangeAudio(Nx_AudioUIContent_t *pstContents)
{
	HUINT32		ulSelectIdx = 0;

	if (pstContents->bDualMonoFullName == TRUE)
	{
		return nx_audioUi_ActionChangeAudioWithDualMonoFullName(pstContents);
	}

	NX_APP_Trace("(+) \n");

	ulSelectIdx = (++pstContents->ulUserSelectIdx) % pstContents->stAudioList.ulAudioNum;
	pstContents->ulUserSelectIdx = ulSelectIdx;

	NX_PRISM_UTIL_MakeAudioString(pstContents->stAudioList, pstContents->ulUserSelectIdx, pstContents->szStrBuf, pstContents->bDualMonoFullName);
	nx_audioUi_UpdateUiObject(pstContents);

	NX_MEDIA_PLAY_SetComponentIndex(pstContents->ulViewId, eOxMP_COMPONENT_AUDIO, ulSelectIdx);

	NX_APP_Info(" Change Audio Set : cur[%d], component Idx[%d]\n", pstContents->ulUserSelectIdx, pstContents->ulUserSelectIdx);
	NX_APP_Trace("(-) \n");
	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_audioUi_MakeAudioList(Nx_AudioUIContent_t *pstContents, NX_Media_AUDIOList_t *stComponetAudio)
{
	HUINT32		i = 0, ulCnt = 0;
	OxMediaPlay_AudioComp_t	*pListInfo = NULL;

	if (pstContents->bDualMonoFullName == TRUE)
	{
		return	nx_audioUi_MakeAudioListWithDualMonoFullName(pstContents, stComponetAudio);
	}

	NX_APP_Trace("(+) \n");

	ulCnt = stComponetAudio->ulAudioNum;
	if (stComponetAudio->ulAudioNum >= NX_CORE_AUDIO_MAX_NUM)
	{
		ulCnt = NX_CORE_AUDIO_MAX_NUM ;
	}

	/* Add Audio Item */
	for (i = 0; i < ulCnt; i++)
	{
		pListInfo = &pstContents->stAudioList.pstAudioInfo[i];
		HxSTD_memcpy(pListInfo, &stComponetAudio->pstAudioInfo[i], sizeof(OxMediaPlay_AudioComp_t));

		if (pListInfo->nLangNum > 1)
/*
		if ((HxSTD_StrLen((HCHAR*)pstContents->stAudioList.pstAudioInfo[pstContents->ulUserSelectIdx].astLangInfo[0].szLangCode) > 0) &&
			(HxSTD_StrLen((HCHAR*)pstContents->stAudioList.pstAudioInfo[pstContents->ulUserSelectIdx].astLangInfo[1].szLangCode) > 0))
*/
		{
			pListInfo->eMode = eOxMP_AUDMODE_DUALMONO;
		}
	}

	/* Add audio Count*/
	pstContents->stAudioList.ulAudioNum = ulCnt;

	NX_APP_Trace("(-) \n");

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_audioUi_LoadItemsValue(Nx_AudioUIContent_t *pstContents)
{
	HERROR				hError = ERR_FAIL;
	ONDK_Result_t		eResult = ONDK_RET_FAIL;
	NX_Media_AUDIOList_t	stComponetAudio;


	stComponetAudio.pstAudioInfo = NULL;
	stComponetAudio.pstAudioInfo = (OxMediaPlay_AudioComp_t*)NX_APP_Calloc(sizeof(OxMediaPlay_AudioComp_t) * NX_CORE_AUDIO_MAX_NUM);
	if (stComponetAudio.pstAudioInfo == NULL)
	{
		NX_APP_Error(" Error!!! NX_APP_Calloc() Fail !!\n");
		return	ONDK_RET_NOLOCALMEMORY;
	}

	NX_PRISM_UTIL_InitNUllPIDValue(&stComponetAudio);

	hError = NX_MEDIA_PLAY_GetComponentListForAudio(pstContents->ulViewId, eOxMP_COMPONENT_AUDIO, &stComponetAudio);
	if (hError != ERR_OK)
	{
		if (stComponetAudio.pstAudioInfo)
		{
			NX_APP_Free(stComponetAudio.pstAudioInfo);
			stComponetAudio.pstAudioInfo = NULL;
		}
		NX_APP_Error(" Error!!! NX_MEDIA_PLAY_GetComponentListForAudio() !!\n");
		return	ONDK_RET_INITFAIL;
	}
	else
	{
		if (stComponetAudio.ulAudioNum < 1)
		{
			if (stComponetAudio.pstAudioInfo)
			{
				NX_APP_Free(stComponetAudio.pstAudioInfo);
				stComponetAudio.pstAudioInfo = NULL;
			}
			NX_APP_Info(" NX_MEDIA_PLAY_GetComponentListForAudio() - No audio item !!\n");
			return	ONDK_RET_INVARG;
		}
		else
		{
			eResult = nx_audioUi_MakeAudioList(pstContents, &stComponetAudio);
			if (stComponetAudio.pstAudioInfo)
			{
				NX_APP_Free(stComponetAudio.pstAudioInfo);
				stComponetAudio.pstAudioInfo = NULL;
			}
			return	eResult;
		}
	}
}


static ONDK_Result_t 	nx_audioUi_UpdateUiObject(Nx_AudioUIContent_t *pstContents)
{
	HERROR				hError = ERR_FAIL;
	HCHAR				*szDisplyImage = NULL, *szLTypeImage = NULL, *szRTypeImage = NULL;
	HINT32				lStrWidth = 0, x = 0, lTempW = 0, lImageW = 0;
	HUINT32			 	ulLTypeFontColor = 0, ulRTypeFontColor = 0;
	DxStereoSelect_e	eAudioType = eDxSTEREO_SELECT_STEREO;
	ONDK_Rect_t 		stRect;


	ONDK_GWM_COM_Text_SetText(NX_PRISM_AUDIO_UI_TEXT_ITEM_ID, pstContents->szStrBuf);

	/* Draw Dolby Icon */
	szDisplyImage = NX_PRISM_UTIL_GetDolbyIconResource(pstContents->stAudioList.pstAudioInfo[pstContents->ulUserSelectIdx].eCodec);

	lStrWidth = ONDK_GWM_COM_Text_GetStringWidth(eFont_SystemNormal, NX_PRISM_FONT_SIZE_22, pstContents->szStrBuf);
	lTempW = (NX_PRISM_AUDIO_UI_FRAME_W - lStrWidth) / 2;
	lImageW = (NX_PRISM_AUDIO_UI_IMG_W + (NX_PRISM_AUDIO_UI_IMG_MG *2));
	if (szDisplyImage != NULL)
	{
		x = NX_PRISM_AUDIO_UI_ICON_X + lTempW;
		if (x > lImageW)
		{
			x -= lImageW;
		}
	}
	else
	{
		x = NX_PRISM_AUDIO_UI_ICON_X;
	}

	stRect = ONDK_Rect(x, NX_PRISM_AUDIO_UI_ICON_Y, NX_PRISM_AUDIO_UI_ICON_W, NX_PRISM_AUDIO_UI_ICON_H);
	if (szDisplyImage != NULL)
	{
		ONDK_GWM_COM_Image_ChangeRect(NX_PRISM_AUDIO_UI_ICON_ID, &stRect);
		ONDK_GWM_COM_Image_SetImage(NX_PRISM_AUDIO_UI_ICON_ID, szDisplyImage);
		ONDK_GWM_Obj_SetHide(NX_PRISM_AUDIO_UI_ICON_ID, FALSE);
		ONDK_GWM_APP_InvalidateObject(NX_PRISM_AUDIO_UI_ICON_ID);
	}
	else
	{
		ONDK_GWM_Obj_SetHide(NX_PRISM_AUDIO_UI_ICON_ID, TRUE);
	}

	if (pstContents->bDualMonoFullName == TRUE)
	{
		eAudioType = pstContents->stAudioOthInfo[pstContents->ulUserSelectIdx].eAudioType;
	}
	else
	{
		// Get Audio Type - eDxSTEREO_SELECT_STEREO, eDxSTEREO_SELECT_MONOLEFT , eDxSTEREO_SELECT_MONORIGHT
		hError = NX_SYSTEM_GetSoundMode(&eAudioType);
		if (hError != ERR_OK)
		{
			NX_APP_Error("ERROR!!! NX_SYSTEM_GetSoundMode() Fail! \n");
		}
	}

	switch (eAudioType)
	{
		case eDxSTEREO_SELECT_STEREO:
			ulLTypeFontColor = COL(C_T_Whistle_100);
			ulRTypeFontColor = COL(C_T_Whistle_100);
			szLTypeImage = RES_217_00_AUDIO_LEFT_ON_PNG;
			szRTypeImage = RES_217_00_AUDIO_RIGHT_ON_PNG;
			break;
		case eDxSTEREO_SELECT_MONOLEFT:
			ulLTypeFontColor = COL(C_T_Whistle_100);
			ulRTypeFontColor = COL(C_T_Gong06);
			szLTypeImage = RES_217_00_AUDIO_LEFT_ON_PNG;
			szRTypeImage = RES_217_00_AUDIO_RIGHT_OFF_PNG;
			break;
		case eDxSTEREO_SELECT_MONORIGHT:
			ulLTypeFontColor = COL(C_T_Gong06);
			ulRTypeFontColor = COL(C_T_Whistle_100);
			szLTypeImage = RES_217_00_AUDIO_LEFT_OFF_PNG;
			szRTypeImage = RES_217_00_AUDIO_RIGHT_ON_PNG;
			break;
		default:
			NX_APP_Error("ERROR !!! Not Support Audio Type : [%d]! \n", eAudioType);
			ulLTypeFontColor = COL(C_T_Gong06);
			ulRTypeFontColor = COL(C_T_Gong06);
			szLTypeImage = RES_217_00_AUDIO_LEFT_OFF_PNG;
			szRTypeImage = RES_217_00_AUDIO_RIGHT_OFF_PNG;
			break;
	}

	/* Draw Left Audio Type Icon */
	ONDK_GWM_COM_Multi_ChangeNormalImage(NX_PRISM_AUDIO_UI_LTYPE_ID, szLTypeImage);
	ONDK_GWM_COM_Multi_SetFontColor(NX_PRISM_AUDIO_UI_LTYPE_ID, ulLTypeFontColor, COL(C_T_Bang_100));

	/* Draw Right Audio Type Icon */
	ONDK_GWM_COM_Multi_ChangeNormalImage(NX_PRISM_AUDIO_UI_RTYPE_ID, szRTypeImage);
	ONDK_GWM_COM_Multi_SetFontColor(NX_PRISM_AUDIO_UI_RTYPE_ID, ulRTypeFontColor, COL(C_T_Bang_100));

	return	ONDK_RET_OK;
}


static ONDK_Result_t 	nx_audioUi_SetUiObject(Nx_AudioUIContent_t *pstContents)
{
	HERROR				hError = ERR_FAIL;
	HCHAR				*szDisplyImage = NULL, *szLTypeImage = NULL, *szRTypeImage = NULL;
	HINT32				lStrWidth = 0, x = 0, lTempW = 0, lImageW = 0;
	HUINT32			 	ulLTypeFontColor = 0, ulRTypeFontColor = 0;
	DxStereoSelect_e	eAudioType = eDxSTEREO_SELECT_STEREO;
	ONDK_Rect_t 		stRect;


	ONDK_GWM_APP_SetAppArea(NX_PRISM_AUDIO_UI_WINDOW_X, NX_PRISM_AUDIO_UI_WINDOW_Y,
							NX_PRISM_AUDIO_UI_WINDOW_W, NX_PRISM_AUDIO_UI_WINDOW_H);

	/* Draw BG Frame */
	stRect = ONDK_Rect(NX_PRISM_AUDIO_UI_FRAME_X, NX_PRISM_AUDIO_UI_FRAME_Y,
						NX_PRISM_AUDIO_UI_FRAME_W, NX_PRISM_AUDIO_UI_FRAME_H);
	ONDK_GWM_COM_Rect_Create(NX_PRISM_AUDIO_UI_FRAME_ID, &stRect, COL(C_G_Bang_90));
	ONDK_GWM_APP_SetObjectDrawMethod(NX_PRISM_AUDIO_UI_FRAME_ID, NX_PRISM_COMP_DrawFrameDialogueWindowBG_BottomLine);

	NX_PRISM_UTIL_MakeAudioString(pstContents->stAudioList, pstContents->ulUserSelectIdx, pstContents->szStrBuf, pstContents->bDualMonoFullName);

	/* Draw Audio TEXT */
	stRect = ONDK_Rect(NX_PRISM_AUDIO_UI_TEXT_ITEM_X, NX_PRISM_AUDIO_UI_TEXT_ITEM_Y,
						NX_PRISM_AUDIO_UI_TEXT_ITEM_W, NX_PRISM_AUDIO_UI_TEXT_ITEM_H);
	ONDK_GWM_COM_Text_Create(NX_PRISM_AUDIO_UI_TEXT_ITEM_ID, &stRect, (HCHAR*)pstContents->szStrBuf);
	ONDK_GWM_COM_Text_SetFont(NX_PRISM_AUDIO_UI_TEXT_ITEM_ID, eFont_SystemBold);
	ONDK_GWM_COM_Text_SetSize(NX_PRISM_AUDIO_UI_TEXT_ITEM_ID, NX_PRISM_FONT_SIZE_22);
	ONDK_GWM_COM_Text_SetAlign(NX_PRISM_AUDIO_UI_TEXT_ITEM_ID, TEXT_ALIGN_CENTER);
	ONDK_GWM_COM_Text_ChangeColor(NX_PRISM_AUDIO_UI_TEXT_ITEM_ID, COL(C_T_Whistle_100), COL(C_T_Bang_100));

	/* Draw Dolby Icon */
	szDisplyImage = NX_PRISM_UTIL_GetDolbyIconResource(pstContents->stAudioList.pstAudioInfo[pstContents->ulUserSelectIdx].eCodec);

	lStrWidth = ONDK_GWM_COM_Text_GetStringWidth(eFont_SystemNormal, NX_PRISM_FONT_SIZE_22, pstContents->szStrBuf);
	lTempW = (NX_PRISM_AUDIO_UI_FRAME_W - lStrWidth) / 2;
	lImageW = (NX_PRISM_AUDIO_UI_IMG_W + (NX_PRISM_AUDIO_UI_IMG_MG *2));
	if (szDisplyImage)
	{
		x = NX_PRISM_AUDIO_UI_ICON_X + lTempW;
		if (x > lImageW)
		{
			x -= lImageW;
		}
	}
	else
	{
		x = NX_PRISM_AUDIO_UI_ICON_X;
	}
	stRect = ONDK_Rect(x, NX_PRISM_AUDIO_UI_ICON_Y,
						NX_PRISM_AUDIO_UI_ICON_W, NX_PRISM_AUDIO_UI_ICON_H);
	ONDK_GWM_COM_Image_Create(NX_PRISM_AUDIO_UI_ICON_ID, &stRect);
	if (szDisplyImage)
	{
		ONDK_GWM_COM_Image_SetImage(NX_PRISM_AUDIO_UI_ICON_ID, szDisplyImage);
	}

	if (pstContents->bDualMonoFullName == TRUE)
	{
		eAudioType = pstContents->stAudioOthInfo[pstContents->ulUserSelectIdx].eAudioType;
	}
	else
	{
		// Get Audio Type - eDxSTEREO_SELECT_STEREO, eDxSTEREO_SELECT_MONOLEFT , eDxSTEREO_SELECT_MONORIGHT
		hError = NX_SYSTEM_GetSoundMode(&eAudioType);
		if (hError != ERR_OK)
		{
			NX_APP_Error("ERROR!!! NX_SYSTEM_GetSoundMode() Fail! \n");
		}
	}

	switch (eAudioType)
	{
		case eDxSTEREO_SELECT_STEREO:
			ulLTypeFontColor = COL(C_T_Whistle_100);
			ulRTypeFontColor = COL(C_T_Whistle_100);
			szLTypeImage = RES_217_00_AUDIO_LEFT_ON_PNG;
			szRTypeImage = RES_217_00_AUDIO_RIGHT_ON_PNG;
			break;
		case eDxSTEREO_SELECT_MONOLEFT:
			ulLTypeFontColor = COL(C_T_Whistle_100);
			ulRTypeFontColor = COL(C_T_Gong06);
			szLTypeImage = RES_217_00_AUDIO_LEFT_ON_PNG;
			szRTypeImage = RES_217_00_AUDIO_RIGHT_OFF_PNG;
			break;
		case eDxSTEREO_SELECT_MONORIGHT:
			ulLTypeFontColor = COL(C_T_Gong06);
			ulRTypeFontColor = COL(C_T_Whistle_100);
			szLTypeImage = RES_217_00_AUDIO_LEFT_OFF_PNG;
			szRTypeImage = RES_217_00_AUDIO_RIGHT_ON_PNG;
			break;
		default:
			NX_APP_Error("ERROR !!! Not Support Audio Type : [%d]! \n", eAudioType);
			ulLTypeFontColor = COL(C_T_Gong06);
			ulRTypeFontColor = COL(C_T_Gong06);
			szLTypeImage = RES_217_00_AUDIO_LEFT_OFF_PNG;
			szRTypeImage = RES_217_00_AUDIO_RIGHT_OFF_PNG;
			break;
	}

	/* Draw Left Audio Type Icon */
	stRect = ONDK_Rect(NX_PRISM_AUDIO_UI_LTYPE_X, NX_PRISM_AUDIO_UI_LTYPE_Y,
						NX_PRISM_AUDIO_UI_LTYPE_W, NX_PRISM_AUDIO_UI_LTYPE_H);
	ONDK_GWM_COM_Multi_Create(NX_PRISM_AUDIO_UI_LTYPE_ID, &stRect);
	ONDK_GWM_COM_Multi_SetContents(NX_PRISM_AUDIO_UI_LTYPE_ID, (HUINT8*)szLTypeImage,
								NULL, (HUINT8*)"L", NULL, NX_PRISM_AUDIO_UI_LTYPE_ICON_MG);
	ONDK_GWM_COM_Multi_SetFontHandle(NX_PRISM_AUDIO_UI_LTYPE_ID, eFont_SystemNormal, NX_PRISM_FONT_SIZE_24);
	ONDK_GWM_COM_Multi_SetFontColor(NX_PRISM_AUDIO_UI_LTYPE_ID, ulLTypeFontColor, COL(C_T_Bang_100));
	ONDK_GWM_COM_Multi_SetFocusFlag(NX_PRISM_AUDIO_UI_LTYPE_ID, FALSE);

	/* Draw Right Audio Type Icon */
	stRect = ONDK_Rect(NX_PRISM_AUDIO_UI_RTYPE_X, NX_PRISM_AUDIO_UI_RTYPE_Y,
						NX_PRISM_AUDIO_UI_RTYPE_W, NX_PRISM_AUDIO_UI_RTYPE_H);
	ONDK_GWM_COM_Multi_Create(NX_PRISM_AUDIO_UI_RTYPE_ID, &stRect);
	ONDK_GWM_COM_Multi_SetContents(NX_PRISM_AUDIO_UI_RTYPE_ID, (HUINT8*)szRTypeImage,
								NULL, NULL, (HUINT8*)"R", NX_PRISM_AUDIO_UI_RTYPE_ICON_MG);
	ONDK_GWM_COM_Multi_SetFontHandle(NX_PRISM_AUDIO_UI_RTYPE_ID, eFont_SystemNormal, NX_PRISM_FONT_SIZE_24);
	ONDK_GWM_COM_Multi_SetFontColor(NX_PRISM_AUDIO_UI_RTYPE_ID, ulRTypeFontColor, COL(C_T_Bang_100));
	ONDK_GWM_COM_Multi_SetFocusFlag(NX_PRISM_AUDIO_UI_RTYPE_ID, FALSE);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_audioUi_Initialize(Nx_AudioUIContent_t *pstContents)
{
	HERROR			hError = ERR_FAIL;
	HINT32			lAudioTrackIndex = -1;
	HUINT32			ulViewId = 0, i = 0;
	DxStereoSelect_e			eAudioType = eDxSTEREO_SELECT_STEREO;
	OxMediaPlay_MediaType_e		eMediaType = eOxMP_MEDIATYPE_NONE;
	OxMediaPlay_Component_t		*pstMediaComponet = NULL;

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		ONDK_GWM_APP_Destroy(0);
		return	ONDK_RET_INITFAIL;
	}

	// Init
	HxSTD_memset(pstContents, 0x00, sizeof(Nx_AudioUIContent_t));
	pstContents->szStrBuf[0] = '\0';

	NX_MEDIA_PLAY_GetMainViewId(&pstContents->ulViewId);
	pstContents->ulViewId = ulViewId;
	NX_MEDIA_PLAY_GetPlayType(ulViewId, &eMediaType);
	pstContents->eMediaType = eMediaType;

	// Pls change value set FALSE if used normal display
	pstContents->bDualMonoFullName = TRUE;

	// Set NULL PID to Audio list
	pstContents->stAudioList.pstAudioInfo = NULL;
	pstContents->stAudioList.pstAudioInfo = (OxMediaPlay_AudioComp_t*)NX_APP_Calloc(sizeof(OxMediaPlay_AudioComp_t) * NX_CORE_AUDIO_MAX_NUM);
	if (pstContents->stAudioList.pstAudioInfo == NULL)
	{
		NX_APP_Error(" Error!!! NX_APP_Calloc() Fail !!\n");
		return	ONDK_RET_NOLOCALMEMORY;
	}

	NX_PRISM_UTIL_InitNUllPIDValue(&pstContents->stAudioList);

	// Load Value
	hError = nx_audioUi_LoadItemsValue(pstContents);
	if (hError != ERR_OK)
	{
		nx_audioUi_CreateResponseDialouge();
		ONDK_GWM_APP_Destroy(0);
		return	ONDK_RET_INITFAIL;
	}

	hError = NX_MEDIA_PLAY_GetComponentIndex(ulViewId, eOxMP_COMPONENT_AUDIO, &lAudioTrackIndex, NULL);
	if (hError != ERR_OK)
	{
		NX_APP_Error("ERROR!!! NX_MEDIA_PLAY_GetComponentIndex() Fail! \n");
		lAudioTrackIndex = 0;
	}

	hError = NX_SYSTEM_GetSoundMode(&eAudioType);
	if (hError != ERR_OK)
	{
		NX_APP_Error("ERROR!!! NX_SYSTEM_GetSoundMode() Fail! \n");
	}

	pstContents->ulUserSelectIdx = 0;

	pstMediaComponet = (OxMediaPlay_Component_t*)NX_APP_Calloc(sizeof(OxMediaPlay_Component_t));
	hError = NX_MEDIA_PLAY_GetComponent(ulViewId, eOxMP_COMPONENT_AUDIO, lAudioTrackIndex, pstMediaComponet);
	if (hError == ERR_OK)
	{
		for (i = 0; i < pstContents->stAudioList.ulAudioNum; i++)
		{
			// TODO: Check PID Only. Pls add other check value
			if (pstContents->stAudioList.pstAudioInfo[i].nPid == pstMediaComponet->stAudio.nPid)
			{
				pstContents->ulUserSelectIdx = i;
				if (pstContents->bDualMonoFullName == TRUE)
				{
					if ((eAudioType == eDxSTEREO_SELECT_MONORIGHT) && (pstContents->stAudioList.pstAudioInfo[i].nPid == pstContents->stAudioList.pstAudioInfo[i+1].nPid))
					{
						pstContents->ulUserSelectIdx += 1;
					}

					// Menu Audio가 eDxSTEREO_SELECT_STEREO이고 Live에 Dual Modo Audio가 나올 경우 FULL Name Mode에서는 Sound Mode를 변환해야 한다.
					if ((eAudioType != eDxSTEREO_SELECT_MONOLEFT) && (eAudioType != eDxSTEREO_SELECT_MONORIGHT))
					{
						eAudioType = pstContents->stAudioOthInfo[pstContents->ulUserSelectIdx].eAudioType;
						NX_SYSTEM_SetSoundMode(eAudioType);
					}
				}

				break;
			}
		}
	}
	else
	{
		NX_APP_Error("ERROR!!! NX_MEDIA_PLAY_GetComponent() Fail! \n");
		pstContents->ulUserSelectIdx = 0;
	}

	if (pstMediaComponet)
	{
		NX_APP_Free(pstMediaComponet);
		pstMediaComponet = NULL;
	}

	NX_APP_Info("NX_MEDIA_PLAY_GetComponentIndex() lAudioTrackIndex :[0x%x][%d], ulUserSelectIdx:[%d]\n", lAudioTrackIndex, lAudioTrackIndex, pstContents->ulUserSelectIdx);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_audioUi_MsgGwmCreate(Nx_AudioUIContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t		hResult  = ONDK_RET_FAIL;

	hResult = nx_audioUi_Initialize(pstContents);
	if (hResult != ONDK_RET_OK)
	{
		NX_APP_Error("ERROR!!! nx_audioUi_Initialize() Fail! \n");
		return	ONDK_RET_FAIL;
	}

	nx_audioUi_SetUiObject(pstContents);
	ONDK_GWM_SetTimer(NX_PRISM_AUDIO_UI_EXPIRE_TIMER_ID, NX_PRISM_AUDIO_UI_EXPIRE_TIME);

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_audioUi_MsgGwmDestroy(Nx_AudioUIContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_GWM_KillTimer(NX_PRISM_AUDIO_UI_EXPIRE_TIMER_ID);
	if (pstContents->stAudioList.pstAudioInfo)
	{
		NX_APP_Free(pstContents->stAudioList.pstAudioInfo);
		pstContents->stAudioList.pstAudioInfo = NULL;
	}

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_audioUi_MsgGwmHapiMsg(Nx_AudioUIContent_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	NX_COMMON_NOT_USED_PARAM(handle);
	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	return	ONDK_MESSAGE_PASS;
}


static ONDK_Result_t	nx_audioUi_MsgGwmKeyDown(Nx_AudioUIContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32		lKeyId = p1;

	switch (lKeyId)
	{
		case KEY_BACK:
		case KEY_EXIT:
			ONDK_GWM_APP_Destroy(0);
			return ONDK_MESSAGE_BREAK;

		case KEY_STANDBY:
		case KEY_MENU:
		case KEY_GUIDE:
			ONDK_GWM_APP_Destroy(0);
			return ONDK_MESSAGE_PASS;

		case KEY_AUDIO:
			ONDK_GWM_ResetTimer(NX_PRISM_AUDIO_UI_EXPIRE_TIMER_ID);
			nx_audioUi_ActionChangeAudio(pstContents);
			return ONDK_MESSAGE_BREAK;

		case KEY_TVRADIO:
		case KEY_WIDE:
		case KEY_VFORMAT:
		case KEY_SUBTITLE:
		case KEY_TEXT:
			NX_PRISM_NotAvailable_Show();
			return	ONDK_MESSAGE_BREAK;

		default:
			return ONDK_MESSAGE_PASS;
	}

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);
}


static ONDK_Result_t 	nx_audioUi_MsgGwmTimer(Nx_AudioUIContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32		ulTimerID = p1;

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		return	ONDK_RET_FAIL;
	}

	if (ulTimerID == NX_PRISM_AUDIO_UI_EXPIRE_TIMER_ID)
	{
		NX_APP_Info("nx_audioUi_MsgGwmTimer() - Destory window by timer of NX_PRISM_AUDIO_UI_EXPIRE_TIMER_ID !!\n");
		ONDK_GWM_APP_Destroy(0);
		return	ONDK_MESSAGE_BREAK;
	}

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	return	ONDK_RET_OK;
}


/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/
ONDK_Result_t	NX_PRISM_AUDIO_UI_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t		eResult = ONDK_RET_OK;
	Nx_AudioUIContent_t	*pstContents = NULL;

	pstContents = nx_audioUi_GetContents();

	switch (lMessage)
	{
		case MSG_GWM_CREATE:
			eResult = nx_audioUi_MsgGwmCreate(pstContents, p1, p2, p3);
			break;
		case MSG_GWM_KEYUP:
			break;
		case MSG_GWM_KEYDOWN:
			eResult = nx_audioUi_MsgGwmKeyDown(pstContents, p1, p2, p3);
			break;
		case MSG_GWM_DESTROY:
			eResult = nx_audioUi_MsgGwmDestroy(pstContents, p1, p2, p3);
			break;
		case MSG_GWM_HAPI_MESSAGE:
			eResult = nx_audioUi_MsgGwmHapiMsg(pstContents, handle, p1, p2, p3);
			break;
		case MSG_GWM_TIMER:
			eResult = nx_audioUi_MsgGwmTimer(pstContents, p1, p2, p3);
			break;
		case MSG_APP_SYSTEM_SVC_CHANGED:
			ONDK_GWM_APP_Destroy(0);
			break;
		default:
			break;
	}
	if(eResult != ONDK_RET_OK)
	{
		return eResult;
	}

	return ONDK_GWM_ProcessMessageDefault(lMessage, handle, p1, p2, p3);
}


/******************************************************************************/
#define _________End_Of_File____________________________________________________
/******************************************************************************/

