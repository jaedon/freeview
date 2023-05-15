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
 * @file	  		nx_prism_chinfo.c
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
#include <nx_prism_app.h>
#include <nx_port.h>
#include <nx_core_message.h>
#include <nx_core_metadata_pf.h>
#include <nx_core_channel.h>

/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/
#define	NX_PRISM_CHINFO_EXPIRE_TIMER_ID				0x001

#define	NX_PRISM_CHINFO_ANIMATION_START_ID			0x002
#define	NX_PRISM_CHINFO_ANIMATION_END_ID			0x003

#define	NX_PRISM_CHINFO_ITEM_ID						(eMenuFrame_Window_CHList + 128)

// Object ID
#define	NX_PRISM_CHINFO_DIM_ID						(NX_PRISM_CHINFO_ITEM_ID + 1)
#define	NX_PRISM_CHINFO_LINE1_ID					(NX_PRISM_CHINFO_ITEM_ID + 2)
#define	NX_PRISM_CHINFO_LINE2_ID					(NX_PRISM_CHINFO_ITEM_ID + 3)
#define	NX_PRISM_CHINFO_NUM_ID						(NX_PRISM_CHINFO_ITEM_ID + 4)
#define	NX_PRISM_CHINFO_NAME_ID						(NX_PRISM_CHINFO_ITEM_ID + 5)
#define	NX_PRISM_CHINFO_EVENT_ID					(NX_PRISM_CHINFO_ITEM_ID + 6)
#define	NX_PRISM_CHINFO_EVENT_STIME_ID				(NX_PRISM_CHINFO_ITEM_ID + 7)
#define	NX_PRISM_CHINFO_EVENT_ETIME_ID				(NX_PRISM_CHINFO_ITEM_ID + 8)
#define	NX_PRISM_CHINFO_EVENT_BAR_L_ID				(NX_PRISM_CHINFO_ITEM_ID + 9)
#define	NX_PRISM_CHINFO_EVENT_BAR_H_ID				(NX_PRISM_CHINFO_ITEM_ID + 10)

// ui frame
#define	NX_PRISM_CHINFO_FRAME_X						880
#define	NX_PRISM_CHINFO_FRAME_Y						100
#define	NX_PRISM_CHINFO_FRAME_W						340
#define	NX_PRISM_CHINFO_FRAME_H						170

// dim
#define	NX_PRISM_CHINFO_DIMICON_X					0
#define	NX_PRISM_CHINFO_DIMICON_Y					0
#define	NX_PRISM_CHINFO_DIMICON_W					340
#define	NX_PRISM_CHINFO_DIMICON_H					170

// line 1
#define	NX_PRISM_CHINFO_LINE1_X						0
#define	NX_PRISM_CHINFO_LINE1_Y						0
#define	NX_PRISM_CHINFO_LINE1_W						340
#define	NX_PRISM_CHINFO_LINE1_H						1

// line 2
#define	NX_PRISM_CHINFO_LINE2_X						0
#define	NX_PRISM_CHINFO_LINE2_Y						169
#define	NX_PRISM_CHINFO_LINE2_W						340
#define	NX_PRISM_CHINFO_LINE2_H						1

// svc num
#define	NX_PRISM_CHINFO_NUM_X						15
#define	NX_PRISM_CHINFO_NUM_Y						(40 - NX_PRISM_FONT_SIZE_24)
#define	NX_PRISM_CHINFO_NUM_W						55
#define	NX_PRISM_CHINFO_NUM_H						(NX_PRISM_FONT_SIZE_24 + NX_PRISM_FONT_VSPACE)

// svc name
#define	NX_PRISM_CHINFO_NAME_X						78
#define	NX_PRISM_CHINFO_NAME_Y						(40 - NX_PRISM_FONT_SIZE_24)
#define	NX_PRISM_CHINFO_NAME_W						247
#define	NX_PRISM_CHINFO_NAME_H						(NX_PRISM_FONT_SIZE_24 + NX_PRISM_FONT_VSPACE)

// svc event
#define	NX_PRISM_CHINFO_EVENT_LS					30
#define	NX_PRISM_CHINFO_EVENT_X						15
#define	NX_PRISM_CHINFO_EVENT_Y						(72 - NX_PRISM_FONT_SIZE_20)
#define	NX_PRISM_CHINFO_EVENT_W						310
#define	NX_PRISM_CHINFO_EVENT_H						(NX_PRISM_CHINFO_EVENT_LS *2 + NX_PRISM_FONT_VSPACE)

// evt stime
#define	NX_PRISM_CHINFO_EVENT_STIME_X				15
#define	NX_PRISM_CHINFO_EVENT_STIME_Y				(139 - NX_PRISM_FONT_SIZE_18)
#define	NX_PRISM_CHINFO_EVENT_STIME_W				48
#define	NX_PRISM_CHINFO_EVENT_STIME_H				(NX_PRISM_FONT_SIZE_18 + NX_PRISM_FONT_VSPACE)

// evt etime
#define	NX_PRISM_CHINFO_EVENT_ETIME_X				277
#define	NX_PRISM_CHINFO_EVENT_ETIME_Y				(139 - NX_PRISM_FONT_SIZE_18)
#define	NX_PRISM_CHINFO_EVENT_ETIME_W				48
#define	NX_PRISM_CHINFO_EVENT_ETIME_H				(NX_PRISM_FONT_SIZE_18 + NX_PRISM_FONT_VSPACE)

// evt progress bar L
#define	NX_PRISM_CHINFO_EVENT_LL_X					15
#define	NX_PRISM_CHINFO_EVENT_LL_Y					145
#define	NX_PRISM_CHINFO_EVENT_LL_W					310
#define	NX_PRISM_CHINFO_EVENT_LL_H					2

// evt progress bar H
#define	NX_PRISM_CHINFO_EVENT_LH_X					15
#define	NX_PRISM_CHINFO_EVENT_LH_Y					144
#define	NX_PRISM_CHINFO_EVENT_LH_W					(310/2) // 310
#define	NX_PRISM_CHINFO_EVENT_LH_H					3

#define	NX_PRSIM_CHINFO_MAX_NUMLEN					8
#define	NX_PRSIM_CHINFO_MAX_NAMELEN					256

/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/
typedef struct NxChInfo_Context
{
	HUINT32			ulSvcNum;
	HUINT32 		svcUid;
	HUINT32			aucSvcNum[NX_PRSIM_CHINFO_MAX_NUMLEN];
	HUINT8			aucSvcName[NX_PRSIM_CHINFO_MAX_NAMELEN];
	HUINT8			aucEvent[EPG_HUMAX_NAME_LEN];
	HUINT8			aucEventSt[NX_PRSIM_CHINFO_MAX_NUMLEN];
	HUINT8			aucEventEt[NX_PRSIM_CHINFO_MAX_NUMLEN];
	HUINT32			ulProgress;
} NxChInfo_Context_t;


/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/

static NxChInfo_Context_t		s_stChInfo_Contents;


/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/

static NxChInfo_Context_t*	nx_ChInfo_GetContents (void );
static ONDK_Result_t 	nx_ChInfo_SetUiObject(NxChInfo_Context_t *pstContents);

static ONDK_Result_t 	nx_ChInfo_MsgGwmDestroy(NxChInfo_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	nx_ChInfo_MsgGwmCreate(NxChInfo_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	nx_ChInfo_MsgGwmKeyUp(NxChInfo_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	nx_ChInfo_MsgGwmKeyDown(NxChInfo_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	nx_ChInfo_MsgGwmTimer(NxChInfo_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_ChInfo_MsgGwmHapiMsg(NxChInfo_Context_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_ChInfo_MsgPFUpdate(NxChInfo_Context_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_ChInfo_MsgGwmO2animationUpdate(NxChInfo_Context_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_ChInfo_MsgGwmO2animationEnd(NxChInfo_Context_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);


/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/

static NxChInfo_Context_t*	nx_ChInfo_GetContents(void)
{
	return &s_stChInfo_Contents;
}

static ONDK_Result_t	nx_ChInfo_DestroyChInfo(NxChInfo_Context_t *pstContents)
{
	ONDK_GWM_KillTimer(NX_PRISM_CHINFO_EXPIRE_TIMER_ID);

	NX_COMMON_NOT_USED_PARAM(pstContents);

	return ONDK_RET_OK;
}


static ONDK_Result_t	nx_ChInfo_initContext(NxChInfo_Context_t *pstContents, HUINT32 lcn, HUINT32 uid)
{
	DxService_t *pService = NULL;

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		return	ONDK_RET_FAIL;
	}

	// init
	HxSTD_memset(pstContents, 0x00, sizeof(NxChInfo_Context_t));

	// ch num
	pstContents->ulSvcNum = lcn;
	pstContents->svcUid = uid;
	HxSTD_snprintf((HCHAR*)pstContents->aucSvcNum, NX_PRSIM_CHINFO_MAX_NUMLEN, (HCHAR*)"%04d", pstContents->ulSvcNum);

	// ch name
	pService = NX_CHANNEL_FindServiceBySvcUid(uid);
	if (pService != NULL)
	{
		HxSTD_StrNCpy((HCHAR*)pstContents->aucSvcName, (HCHAR*)pService->name, NX_PRSIM_CHINFO_MAX_NAMELEN);
	}
	else
	{
		HxSTD_snprintf((HCHAR*)pstContents->aucSvcName, NX_PRSIM_CHINFO_MAX_NAMELEN, (HCHAR*)"%s", (HUINT8*)ONDK_GetString(RES_UNKNOWN_ID));
	}

	return ONDK_RET_OK;
}


static ONDK_Result_t 	nx_ChInfo_SetUiObject(NxChInfo_Context_t *pstContents)
{
	ONDK_Rect_t 	stRect;

	NX_APP_Assert(pstContents);

	ONDK_GWM_APP_SetAppArea(NX_PRISM_CHINFO_FRAME_X, NX_PRISM_CHINFO_FRAME_Y, NX_PRISM_CHINFO_FRAME_W, NX_PRISM_CHINFO_FRAME_H);

	// dim
	stRect = ONDK_Rect(NX_PRISM_CHINFO_DIMICON_X, NX_PRISM_CHINFO_DIMICON_Y, NX_PRISM_CHINFO_DIMICON_W, NX_PRISM_CHINFO_DIMICON_H);
	ONDK_GWM_COM_Rect_Create(NX_PRISM_CHINFO_DIM_ID, &stRect, COL(C_G_Bangk_75));

	// line 1
	stRect = ONDK_Rect(NX_PRISM_CHINFO_LINE1_X, NX_PRISM_CHINFO_LINE1_Y, NX_PRISM_CHINFO_LINE1_W, NX_PRISM_CHINFO_LINE1_H);
	ONDK_GWM_COM_Rect_Create(NX_PRISM_CHINFO_LINE1_ID, &stRect, COL(C_G_Whistle_100));

	// line 2
	stRect = ONDK_Rect(NX_PRISM_CHINFO_LINE2_X, NX_PRISM_CHINFO_LINE2_Y, NX_PRISM_CHINFO_LINE2_W, NX_PRISM_CHINFO_LINE2_H);
	ONDK_GWM_COM_Rect_Create(NX_PRISM_CHINFO_LINE2_ID, &stRect, COL(C_G_Whistle_100));

	// number
	stRect = ONDK_Rect(NX_PRISM_CHINFO_NUM_X, NX_PRISM_CHINFO_NUM_Y, NX_PRISM_CHINFO_NUM_W, NX_PRISM_CHINFO_NUM_H);
	ONDK_GWM_COM_Text_Create(NX_PRISM_CHINFO_NUM_ID, &stRect, (HCHAR*)pstContents->aucSvcNum);
	ONDK_GWM_COM_Text_SetFont(NX_PRISM_CHINFO_NUM_ID, eFont_SystemNormal);
	ONDK_GWM_COM_Text_SetSize(NX_PRISM_CHINFO_NUM_ID, NX_PRISM_FONT_SIZE_24);
	ONDK_GWM_COM_Text_SetAlign(NX_PRISM_CHINFO_NUM_ID, TEXT_ALIGN_LEFT);
	ONDK_GWM_COM_Text_ChangeColor(NX_PRISM_CHINFO_NUM_ID, COL(C_T_Whistle_100), COL(C_T_Bang_100));

	// name
	stRect = ONDK_Rect(NX_PRISM_CHINFO_NAME_X,NX_PRISM_CHINFO_NAME_Y,NX_PRISM_CHINFO_NAME_W,NX_PRISM_CHINFO_NAME_H);
	ONDK_GWM_COM_Text_Create(NX_PRISM_CHINFO_NAME_ID, &stRect, (HCHAR*)pstContents->aucSvcName);
	ONDK_GWM_COM_Text_SetFont(NX_PRISM_CHINFO_NAME_ID, eFont_SystemBold);
	ONDK_GWM_COM_Text_SetSize(NX_PRISM_CHINFO_NAME_ID, NX_PRISM_FONT_SIZE_24);
	ONDK_GWM_COM_Text_SetAlign(NX_PRISM_CHINFO_NAME_ID, TEXT_ALIGN_LEFT);
	ONDK_GWM_COM_Text_ChangeColor(NX_PRISM_CHINFO_NAME_ID, COL(C_T_Whistle_100), COL(C_T_Bang_100));

	// event
	stRect = ONDK_Rect(NX_PRISM_CHINFO_EVENT_X,NX_PRISM_CHINFO_EVENT_Y,NX_PRISM_CHINFO_EVENT_W,NX_PRISM_CHINFO_EVENT_H);
	ONDK_GWM_COM_Text_Create(NX_PRISM_CHINFO_EVENT_ID, &stRect, "");
	ONDK_GWM_COM_Text_SetFont(NX_PRISM_CHINFO_EVENT_ID, eFont_SystemNormal);
	ONDK_GWM_COM_Text_SetSize(NX_PRISM_CHINFO_EVENT_ID, NX_PRISM_FONT_SIZE_20);
	ONDK_GWM_COM_Text_SetAlign(NX_PRISM_CHINFO_EVENT_ID, (TEXT_ALIGN_LEFT | TEXT_MULTILINED | TEXT_WORDWRAP));
	ONDK_GWM_COM_Text_ChangeColor(NX_PRISM_CHINFO_EVENT_ID, COL(C_T_Gong02), COL(C_T_Bang_100));

	// evt stime
	stRect = ONDK_Rect(NX_PRISM_CHINFO_EVENT_STIME_X,NX_PRISM_CHINFO_EVENT_STIME_Y,NX_PRISM_CHINFO_EVENT_STIME_W,NX_PRISM_CHINFO_EVENT_STIME_H);
	ONDK_GWM_COM_Text_Create(NX_PRISM_CHINFO_EVENT_STIME_ID, &stRect, "");
	ONDK_GWM_COM_Text_SetFont(NX_PRISM_CHINFO_EVENT_STIME_ID, eFont_SystemNormal);
	ONDK_GWM_COM_Text_SetSize(NX_PRISM_CHINFO_EVENT_STIME_ID, NX_PRISM_FONT_SIZE_18);
	ONDK_GWM_COM_Text_SetAlign(NX_PRISM_CHINFO_EVENT_STIME_ID, TEXT_ALIGN_LEFT);
	ONDK_GWM_COM_Text_ChangeColor(NX_PRISM_CHINFO_EVENT_STIME_ID, COL(C_T_Gong02), COL(C_T_Bang_100));

	// evt etime
	stRect = ONDK_Rect(NX_PRISM_CHINFO_EVENT_ETIME_X,NX_PRISM_CHINFO_EVENT_ETIME_Y,NX_PRISM_CHINFO_EVENT_ETIME_W,NX_PRISM_CHINFO_EVENT_ETIME_H);
	ONDK_GWM_COM_Text_Create(NX_PRISM_CHINFO_EVENT_ETIME_ID, &stRect, "");
	ONDK_GWM_COM_Text_SetFont(NX_PRISM_CHINFO_EVENT_ETIME_ID, eFont_SystemNormal);
	ONDK_GWM_COM_Text_SetSize(NX_PRISM_CHINFO_EVENT_ETIME_ID, NX_PRISM_FONT_SIZE_18);
	ONDK_GWM_COM_Text_SetAlign(NX_PRISM_CHINFO_EVENT_ETIME_ID, TEXT_ALIGN_RIGHT);
	ONDK_GWM_COM_Text_ChangeColor(NX_PRISM_CHINFO_EVENT_ETIME_ID, COL(C_T_Gong02), COL(C_T_Bang_100));

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_ChInfo_MsgGwmCreate(NxChInfo_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR		hError = ERR_FAIL;
    HINT32		lInfoDisplayTime = NX_COMMON_TIME_DEFAULT;
	HUINT32		ulLcn = p1;
	HUINT32		ulSvcUid = p2;

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		return	ONDK_RET_FAIL;
	}

	/* 절대 hide가 되지 않는다. */
	ONDK_GWM_APP_SetUIHideFlag(GWM_UI_Hide_Never);

	nx_ChInfo_initContext(pstContents, ulLcn, ulSvcUid);
	nx_ChInfo_SetUiObject(pstContents);

	hError = NX_SYSTEM_GetInfoDisplayTime(&lInfoDisplayTime);
	if (hError != ERR_OK)
	{
		NX_APP_Error("Error!!! NX_SYSTEM_GetInfoDisplayTime() Set Default time 5sec!!!");
	}
	ONDK_GWM_SetTimer(NX_PRISM_CHINFO_EXPIRE_TIMER_ID, (lInfoDisplayTime * NX_COMMON_TIME_SECOND));

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	return ONDK_RET_OK;
}

static ONDK_Result_t	nx_ChInfo_MsgGwmKeyUp(NxChInfo_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		return	ONDK_RET_FAIL;
	}

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	return ONDK_MESSAGE_PASS;
}

static ONDK_Result_t	nx_ChInfo_MsgGwmKeyDown(NxChInfo_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32		lKeyId = p1;
	ONDK_Result_t	lResult = ONDK_RET_OK;

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		return	ONDK_RET_FAIL;
	}

	switch (lKeyId)
	{
		case KEY_BACK:
		case KEY_EXIT:
		case KEY_GUIDE:
		case KEY_MENU:
		case KEY_0:
		case KEY_1:
		case KEY_2:
		case KEY_3:
		case KEY_4:
		case KEY_5:
		case KEY_6:
		case KEY_7:
		case KEY_8:
		case KEY_9:
			ONDK_GWM_ResetTimer(NX_PRISM_CHINFO_EXPIRE_TIMER_ID);

			{
				ONDK_GWM_O2Ani_Settings_t stO2AniSettings;

				ONDK_GWM_O2ANI_GetDefaultSettings((eONDK_O2ANITYPE_MoveTo | eONDK_O2ANITYPE_Fade), &stO2AniSettings, NULL);
				//stO2AniSettings.nSpeed = 40;
				stO2AniSettings.nStartAlpha = 100;
				stO2AniSettings.nEndAlpha = 0;
				lResult = ONDK_GWM_O2ANI_AddItem(NX_PRISM_CHINFO_ANIMATION_END_ID, &stO2AniSettings, DEF_O2ANI_EFFECT_IGNORE_KEYPROCESS);
				if(GWM_RET_FAIL == lResult)
				{
					ONDK_GWM_APP_Destroy (0);
				}
			}
			break;

		case KEY_STANDBY:
			ONDK_GWM_APP_Destroy(0);
			return ONDK_MESSAGE_PASS;

		default:
			break;
	}


	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	return ONDK_MESSAGE_PASS;
}


static ONDK_Result_t 	nx_ChInfo_MsgGwmTimer(NxChInfo_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32			ulTimerID = p1;
	ONDK_Result_t	lResult = ONDK_RET_OK;

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		return	ONDK_RET_FAIL;
	}

	if (ulTimerID == NX_PRISM_CHINFO_EXPIRE_TIMER_ID)
	{
		{
			ONDK_GWM_O2Ani_Settings_t stO2AniSettings;

			ONDK_GWM_O2ANI_GetDefaultSettings((eONDK_O2ANITYPE_MoveTo | eONDK_O2ANITYPE_Fade), &stO2AniSettings, NULL);
			stO2AniSettings.nStartAlpha = 100;
			stO2AniSettings.nEndAlpha = 0;
			lResult = ONDK_GWM_O2ANI_AddItem(NX_PRISM_CHINFO_ANIMATION_END_ID, &stO2AniSettings, DEF_O2ANI_EFFECT_IGNORE_KEYPROCESS);
			if(GWM_RET_FAIL == lResult)
			{
				ONDK_GWM_APP_Destroy (0);
			}
		}
		return	ONDK_MESSAGE_BREAK;
	}

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_ChInfo_MsgGwmDestroy(NxChInfo_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		return	ONDK_RET_FAIL;
	}

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	return	nx_ChInfo_DestroyChInfo(pstContents);
}

static ONDK_Result_t	nx_ChInfo_MsgGwmHapiMsg(NxChInfo_Context_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		return	ONDK_RET_FAIL;
	}

	NX_COMMON_NOT_USED_PARAM(handle);
	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	return	ONDK_MESSAGE_PASS;
}


static ONDK_Result_t	nx_ChInfo_MsgPFUpdate(NxChInfo_Context_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Rect_t 	stRect = {0, 0, 0, 0};
	HERROR			hErr = ERR_FAIL;

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		return	ONDK_RET_FAIL;
	}

	NX_COMMON_NOT_USED_PARAM(handle);
	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	hErr = NX_PF_PROGRAMMES_GetNameTime(pstContents->svcUid, pstContents->aucEvent, pstContents->aucEventSt, pstContents->aucEventEt, &pstContents->ulProgress);
	if (hErr == ERR_OK)
	{
		HUINT32 w = 0;

		ONDK_GWM_COM_Text_SetText(NX_PRISM_CHINFO_EVENT_ID, (HCHAR*)pstContents->aucEvent);
		ONDK_GWM_COM_Text_SetText(NX_PRISM_CHINFO_EVENT_STIME_ID, (HCHAR*)pstContents->aucEventSt);
		ONDK_GWM_COM_Text_SetText(NX_PRISM_CHINFO_EVENT_ETIME_ID, (HCHAR*)pstContents->aucEventEt);

		// evt bar L
		stRect = ONDK_Rect(NX_PRISM_CHINFO_EVENT_LL_X, NX_PRISM_CHINFO_EVENT_LL_Y, NX_PRISM_CHINFO_EVENT_LL_W, NX_PRISM_CHINFO_EVENT_LL_H);
		ONDK_GWM_COM_Rect_Create(NX_PRISM_CHINFO_EVENT_BAR_L_ID, &stRect, COL(C_T_Gong02));

		// evt bar H
		w = (NX_PRISM_CHINFO_EVENT_LH_W * pstContents->ulProgress) / 100;
		if (w > 0 )
		{
			stRect = ONDK_Rect(NX_PRISM_CHINFO_EVENT_LH_X,NX_PRISM_CHINFO_EVENT_LH_Y, w,NX_PRISM_CHINFO_EVENT_LH_H);
			ONDK_GWM_COM_Rect_Create(NX_PRISM_CHINFO_EVENT_BAR_H_ID, &stRect, COL(C_G_Whistle_100));
		}

		ONDK_GWM_APP_InvalidateRect(NX_PRISM_CHINFO_FRAME_X, NX_PRISM_CHINFO_FRAME_Y, NX_PRISM_CHINFO_FRAME_W, NX_PRISM_CHINFO_FRAME_H + 1);
	}

	NX_COMMON_NOT_USED_PARAM(handle);
	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	return	ONDK_MESSAGE_PASS;
}

static ONDK_Result_t	nx_ChInfo_MsgGwmO2animationUpdate(NxChInfo_Context_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32			nAniId = p1;
	HUINT8			ucCurAlpha = (HUINT8)p3;
	ONDK_Color_t	ulColor = 0;


	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		return	ONDK_RET_FAIL;
	}

	NX_COMMON_NOT_USED_PARAM(handle);
	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	switch(nAniId)
	{
	case NX_PRISM_CHINFO_ANIMATION_START_ID:
	case NX_PRISM_CHINFO_ANIMATION_END_ID:
		ulColor = ONDK_GWM_GFX_GetSysDimmedAlpha(NULL, COL(C_G_Bangk_75), ucCurAlpha);
		ONDK_GWM_COM_Rect_ChangeColor(NX_PRISM_CHINFO_DIM_ID, ulColor);

		ulColor = ONDK_GWM_GFX_GetSysDimmedAlpha(NULL, COL(C_G_Whistle_100), ucCurAlpha);
		ONDK_GWM_COM_Rect_ChangeColor(NX_PRISM_CHINFO_LINE1_ID, ulColor);
		ONDK_GWM_COM_Rect_ChangeColor(NX_PRISM_CHINFO_LINE2_ID, ulColor);

		ulColor = ONDK_GWM_GFX_GetSysDimmedAlpha(NULL, COL(C_T_Whistle_100), ucCurAlpha);
		ONDK_GWM_COM_Text_ChangeColor(NX_PRISM_CHINFO_NUM_ID, ulColor, COL(C_T_Gong01));
		ONDK_GWM_COM_Text_ChangeColor(NX_PRISM_CHINFO_NAME_ID, ulColor, COL(C_T_Gong01));

		ulColor = ONDK_GWM_GFX_GetSysDimmedAlpha(NULL, COL(C_T_Gong02), ucCurAlpha);
		ONDK_GWM_COM_Text_ChangeColor(NX_PRISM_CHINFO_EVENT_ID, ulColor, COL(C_T_Gong01));
		ONDK_GWM_COM_Text_ChangeColor(NX_PRISM_CHINFO_EVENT_STIME_ID, ulColor, COL(C_T_Gong01));
		ONDK_GWM_COM_Text_ChangeColor(NX_PRISM_CHINFO_EVENT_ETIME_ID, ulColor, COL(C_T_Gong01));

		ulColor = ONDK_GWM_GFX_GetSysDimmedAlpha(NULL, COL(C_T_Gong02), ucCurAlpha);
		ONDK_GWM_COM_Rect_ChangeColor(NX_PRISM_CHINFO_EVENT_BAR_L_ID, ulColor);
		ONDK_GWM_COM_Rect_ChangeColor(NX_PRISM_CHINFO_EVENT_BAR_H_ID, ulColor);
		break;

	default:
		break;
	}


	return	ONDK_MESSAGE_BREAK;
}


static ONDK_Result_t	nx_ChInfo_MsgGwmO2animationEnd(NxChInfo_Context_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32			nAniId = p2;

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		return	ONDK_RET_FAIL;
	}

	NX_COMMON_NOT_USED_PARAM(handle);
	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	switch(nAniId)
	{
	case NX_PRISM_CHINFO_ANIMATION_END_ID:
		ONDK_GWM_APP_Destroy (0);
		break;

	default:
		break;
	}

	NX_APP_Debug("[%s:%d] id(%d)\r\n", __FUNCTION__, __LINE__, nAniId);

	return	ONDK_MESSAGE_BREAK;
}


/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/
ONDK_Result_t		NX_PRISM_CHINFO_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t		eResult	= ONDK_RET_OK;
	NxChInfo_Context_t *pstContents = nx_ChInfo_GetContents();

	switch (lMessage)
	{
		case MSG_GWM_CREATE:
			eResult = nx_ChInfo_MsgGwmCreate(pstContents, p1, p2, p3);
			break;
		case MSG_GWM_KEYUP:
//			eResult = nx_ChInfo_MsgGwmKeyUp(pstContents, p1, p2, p3);
			break;
		case MSG_GWM_KEYDOWN:
			eResult = nx_ChInfo_MsgGwmKeyDown(pstContents, p1, p2, p3);
			break;
		case MSG_GWM_TIMER:
			eResult = nx_ChInfo_MsgGwmTimer(pstContents, p1, p2, p3);
			break;
		case MSG_GWM_DESTROY:
			eResult = nx_ChInfo_MsgGwmDestroy(pstContents, p1, p2, p3);
			break;
		case MSG_GWM_HAPI_MESSAGE:
			eResult = nx_ChInfo_MsgGwmHapiMsg(pstContents, handle, p1, p2, p3);
			break;
		case MSG_CORE_SERVICE_PF_CHANGED_PROGRAMMES:
			eResult = nx_ChInfo_MsgPFUpdate(pstContents, handle, p1, p2, p3);
			break;
		case MSG_GWM_O2ANIMATION_UPDATE:
			eResult = nx_ChInfo_MsgGwmO2animationUpdate(pstContents, handle, p1, p2, p3);
			break;
		case MSG_GWM_O2ANIMATION_END:
			eResult = nx_ChInfo_MsgGwmO2animationEnd(pstContents, handle, p1, p2, p3);
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

