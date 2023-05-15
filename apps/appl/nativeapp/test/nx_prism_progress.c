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
 * @file	  		nx_prism_system_volume.c
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
#include <nx_port_system.h>

/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/
#define	NX_PRISM_PROGRESS_EXPIRE_TIMER_ID			0x001
#define	NX_PRISM_PROGRESS_EXPIRE_TIME				(3 * NX_COMMON_TIME_SECOND)

// Object ID
#define	NX_PRISM_PROGRESS_FRAME_ID				1
#define	NX_PRISM_PROGRESS_NUMTEXT_ID				2
#define	NX_PRISM_PROGRESS_PROGRESS_ID				3
#define	NX_PRISM_PROGRESS_MUTEICON_ID				4
#define	NX_PRISM_PROGRESS_MUTETEXT_ID				5

#define	NX_PRISM_PROGRESS_FRAME_X					0
#define	NX_PRISM_PROGRESS_FRAME_Y					0
#define	NX_PRISM_PROGRESS_FRAME_W					20
#define	NX_PRISM_PROGRESS_FRAME_H					500

#define	NX_PRISM_PROGRESS_PROGRESS_X				197
#define	NX_PRISM_PROGRESS_PROGRESS_Y				174
#define	NX_PRISM_PROGRESS_PROGRESS_W				33
#define	NX_PRISM_PROGRESS_PROGRESS_H				312

#define	NX_PRISM_PROGRESS_NUMTEXT_X				190
#define	NX_PRISM_PROGRESS_NUMTEXT_Y				529
#define	NX_PRISM_PROGRESS_NUMTEXT_W				47
#define	NX_PRISM_PROGRESS_NUMTEXT_H				40

#define	NX_PRISM_PROGRESS_MUTETEXT_X				190
#define	NX_PRISM_PROGRESS_MUTETEXT_Y				514
#define	NX_PRISM_PROGRESS_MUTETEXT_W				47
#define	NX_PRISM_PROGRESS_MUTETEXT_H				26

#define	NX_PRISM_PROGRESS_MUTEICON_X				200
#define	NX_PRISM_PROGRESS_MUTEICON_Y				525
#define	NX_PRISM_PROGRESS_MUTEICON_W				33
#define	NX_PRISM_PROGRESS_MUTEICON_H				33


// TODO: delete
#define	PROGRESS_FRAME					"/usr/image/104_Background_R.png"
#define	PROGRESS_BLANK					"/usr/image/210_Vol_Blank.png"
#define	PROGRESS_FILL						"/usr/image/210_Progress_Fill.png"
#define	PROGRESS_MUTE						"/usr/image/210_Vol_Mute.png"

/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/
typedef struct NxProgress_Context
{
	HUINT32			ulCurFocus;
	HUINT32			ulNumOfSubItem, ulMaxNumOfSubItem;
	HINT32			appprogress;
	HINT32          octoprogress;
	HINT32          progressMax;
	HINT32          progressMin;
	HBOOL           isMute;

	HCHAR			szNumText[NX_TEXT_SIZE_64];
	HCHAR			szMuteText[NX_TEXT_SIZE_64];
} NxProgress_Context_t;


/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/
static	HUINT32	s_gvolumetable[NX_SYS_VOLUME_STEP +1]={
	0,3,5,7,10,13,15,17,20,23, 25,27,30,33,35,37,40,43,45,47,50
};


static NxProgress_Context_t		s_stProgress_Contents;


/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/

static NxProgress_Context_t*	local_PRISM_progress_ProgressGetContents (void );
static ONDK_Result_t	local_PRISM_progress_Initialize(NxProgress_Context_t *pstContents);
//static HUINT32 			local_PRISM_progress_ConvertOctoToAPP(HUINT32 ulVolume);
static void				local_PRISM_progress_Setting(NxProgress_Context_t *pstContents, HINT32 lKey);

static ONDK_Result_t 	local_PRISM_progress_MsgGwmDestroy(NxProgress_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	local_PRISM_progress_MsgGwmCreate(NxProgress_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	local_PRISM_progress_MsgGwmKeyUp(NxProgress_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	local_PRISM_progress_MsgGwmKeyDown(NxProgress_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);

/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/

static NxProgress_Context_t*	local_PRISM_progress_ProgressGetContents(void)
{
	return &s_stProgress_Contents;
}

#if 0
static HUINT32	local_PRISM_progress_ConvertOctoToAPP(HUINT32 ulProgress)
{
	HUINT32 i;

	for(i=0; i<= NX_SYS_VOLUME_STEP; i++)
	{
		if (ulVolume == s_gvolumetable[i])
		{
			return i;
		}
	}

	return	0;
}

#endif
static ONDK_Result_t	local_PRISM_progress_Initialize(NxProgress_Context_t *pstContents)
{
	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! Please check!! \n");
		return	ONDK_RET_FAIL;
	}

	pstContents->progressMin = NX_SYS_VOLUME_MIN;

    NX_PORT_SYSTEM_GetMute(&pstContents->isMute);

	if(pstContents->progressMax - pstContents->progressMin > 0)
	{
		pstContents->appprogress = (pstContents->octoprogress * NX_SYS_VOLUME_STEP) / (pstContents->progressMax - pstContents->progressMin);
	}
	HxSTD_sprintf(pstContents->szNumText, "%d",pstContents->appprogress);

	return	ONDK_RET_OK;
}

static void		local_PRISM_progress_Setting(NxProgress_Context_t *pstContents, HINT32 lKey)
{
	HBOOL	bDrawPanelVolText = FALSE;
	HUINT8	*pszStr = NULL;
	HCHAR	szPanelStr[NX_TEXT_SIZE_32] = {0, };

	switch (lKey)
	{
		case KEY_VOLUMEUP:
			if(NX_SYS_VOLUME_MAX	> pstContents->appprogress)
			{
				++pstContents->appprogress;
				pstContents->octoprogress = (pstContents->progressMax - pstContents->progressMin)/NX_SYS_VOLUME_STEP * pstContents->appprogress;
				NX_APP_Print("volume-octo [%d] volume-app[%d]\n",pstContents->octoprogress,pstContents->appprogress);
				NX_PORT_SYSTEM_SetVolume(pstContents->octoprogress);
				NX_APP_Print("volume-octo [%d] volume-app[%d]\n",s_gvolumetable[pstContents->appprogress],pstContents->appprogress);
			}
			bDrawPanelVolText = TRUE;
			break;

		case KEY_VOLUMEDOWN:
			if(NX_SYS_VOLUME_MIN	< pstContents->appprogress)
			{
				--pstContents->appprogress;
				NX_APP_Print("volume-octo [%d] volume-app[%d]\n",s_gvolumetable[pstContents->appprogress],pstContents->appprogress);
				NX_PORT_SYSTEM_SetVolume(pstContents->octoprogress);
			}
			bDrawPanelVolText = TRUE;
			break;

		case KEY_MUTE:
			NX_APP_Print("[%s:%d] Mute (%d)\n", __FUNCTION__, __LINE__, pstContents->isMute);
			pstContents->isMute = !pstContents->isMute;

			if(pstContents->isMute == TRUE)
			{
				pszStr = ONDK_GetString(RES_MUTE_ID);
				if(pszStr == NULL) pszStr = "# Mute #";

				HxSTD_sprintf(pstContents->szMuteText, "%s", pszStr);
			}
			else
			{
				bDrawPanelVolText = TRUE;
			}

			break;

		default:
			return;
 	}

	ONDK_GWM_Obj_SetHide((NX_PRISM_PROGRESS_NUMTEXT_ID), pstContents->isMute);
	ONDK_GWM_Obj_SetVisible((NX_PRISM_PROGRESS_MUTETEXT_ID), pstContents->isMute);
	ONDK_GWM_Obj_SetVisible((NX_PRISM_PROGRESS_MUTEICON_ID), pstContents->isMute);

	if (bDrawPanelVolText == TRUE)
	{

		pszStr = ONDK_GetString(RES_VOLUME_ID);
		if(pszStr == NULL) pszStr = "# Volume #";

		HxSTD_sprintf(szPanelStr, "%s %d", pszStr, pstContents->appprogress);
		NX_PANEL_SetString(szPanelStr, eDxPANEL_ALIGN_CENTER, FALSE);
		HxSTD_sprintf(pstContents->szNumText, "%d", pstContents->appprogress);
		ONDK_GWM_COM_VProgress_SetPos(NX_PRISM_PROGRESS_PROGRESS_ID, pstContents->appprogress);
	}
	else
	{
		ONDK_GWM_COM_VProgress_SetPos(NX_PRISM_PROGRESS_PROGRESS_ID, 0);
	}

	ONDK_GWM_APP_InvalidateAPP(ONDK_GWM_APP_GetThis());
}


static ONDK_Result_t local_PRISM_progress_DestroyVolume(NxProgress_Context_t *pstContents)
{
	return ONDK_RET_OK;
}



static	ONDK_Result_t	nx_ui_PRISM_progress_testDraw (ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	ONDK_Rect_t 	stRect;
	stRect.x = area->x1 + rect->x;
	stRect.y = area->y1 + rect->y;
	stRect.w = rect->w;
	stRect.h = rect->h;

	ONDK_GUICOM_VBox(screen, stRect, "/usr/image/166_1_00_Scroll_B1_1T.png", "/usr/image/166_1_00_Scroll_B1_2M.png", "/usr/image/166_1_00_Scroll_B1_3B.png");
	//ONDK_GUICOM_HBox(screen, stRect, "/usr/image/370_H_Shelf_1L.png", "/usr/image/370_H_Shelf_2C.png", "/usr/image/370_H_Shelf_3R.png");

	return ONDK_RET_OK;
}


static ONDK_Result_t 	local_PRISM_progress_SetUiObject(NxProgress_Context_t *pstContents)
{
	ONDK_Rect_t 	stRect;

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! Please check!! \n");
		return	ONDK_RET_FAIL;
	}

	ONDK_GWM_APP_SetAppArea(NX_PRISM_PROGRESS_FRAME_X, NX_PRISM_PROGRESS_FRAME_Y, NX_PRISM_PROGRESS_FRAME_W, NX_PRISM_PROGRESS_FRAME_H);
	stRect = ONDK_Rect(0,100,NX_PRISM_PROGRESS_FRAME_W,NX_PRISM_PROGRESS_FRAME_H);

	ONDK_GWM_COM_Frame_Create(NX_PRISM_PROGRESS_FRAME_ID, &stRect, NULL);
	ONDK_GWM_APP_SetObjectDrawMethod(NX_PRISM_PROGRESS_FRAME_ID, (GWM_ObjDraw_t)nx_ui_PRISM_progress_testDraw);
#if 0
	ONDK_GWM_APP_SetAppArea(NX_PRISM_PROGRESS_FRAME_X, NX_PRISM_PROGRESS_FRAME_Y, NX_PRISM_PROGRESS_FRAME_W, NX_PRISM_PROGRESS_FRAME_H);

	stRect = ONDK_Rect(0,0,NX_PRISM_PROGRESS_FRAME_W,NX_PRISM_PROGRESS_FRAME_H);
	//ONDK_GWM_COM_Image_Create(NX_PRISM_PROGRESS_FRAME_ID, &stRect);
	//ONDK_GWM_COM_Image_SetImage(NX_PRISM_PROGRESS_FRAME_ID, PROGRESS_FRAME);

	//stRect = ONDK_Rect(NX_PRISM_PROGRESS_PROGRESS_X,NX_PRISM_PROGRESS_PROGRESS_Y,NX_PRISM_PROGRESS_PROGRESS_W,NX_PRISM_PROGRESS_PROGRESS_H);

	ONDK_GWM_COM_VProgress_Create(NX_PRISM_PROGRESS_PROGRESS_ID, &stRect, 0, 0, 0);
	ONDK_GWM_COM_VProgress_SetBlankImg(NX_PRISM_PROGRESS_PROGRESS_ID, PROGRESS_FRAME);
	ONDK_GWM_COM_VProgress_SetFillImg(NX_PRISM_PROGRESS_PROGRESS_ID, PROGRESS_FILL);

#endif

	/*ONDK_GWM_COM_VProgress_SetVspace(NX_PRISM_PROGRESS_PROGRESS_ID, 1);
	ONDK_GWM_COM_VProgress_SetMax(NX_PRISM_PROGRESS_PROGRESS_ID, 1);
	ONDK_GWM_COM_VProgress_SetMin(NX_PRISM_PROGRESS_PROGRESS_ID, 1);
	ONDK_GWM_COM_VProgress_SetPos(NX_PRISM_PROGRESS_PROGRESS_ID, pstContents->appprogress);
*/
	return	ONDK_RET_OK;
}


static ONDK_Result_t	local_PRISM_progress_MsgGwmCreate(NxProgress_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! Please check!! \n");
		return	ONDK_RET_FAIL;
	}

	/* 절대 hide가 되지 않는다. */
	ONDK_GWM_APP_SetUIHideFlag(GWM_UI_Hide_Never);

	local_PRISM_progress_Initialize(pstContents);
	local_PRISM_progress_SetUiObject(pstContents);

	local_PRISM_progress_Setting(pstContents,p1);

	ONDK_GWM_SetTimer(NX_PRISM_PROGRESS_EXPIRE_TIMER_ID, NX_PRISM_PROGRESS_EXPIRE_TIME);

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	return ONDK_RET_OK;
}

static ONDK_Result_t	local_PRISM_progress_MsgGwmKeyUp(NxProgress_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32		lKeyId = p1;

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! Please check!! \n");
		return	ONDK_RET_FAIL;
	}

	switch (lKeyId)
	{
		case KEY_VOLUMEUP:
		case KEY_VOLUMEDOWN:
		case KEY_MUTE:
			return ONDK_MESSAGE_CONSUMED;
		default:
			break;
	}

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);
	return ONDK_MESSAGE_NONCONSUMED;
}

static ONDK_Result_t	local_PRISM_progress_MsgGwmKeyDown(NxProgress_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32		lKeyId = p1;

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! Please check!! \n");
		return	ONDK_RET_FAIL;
	}

	ONDK_GWM_ResetTimer(NX_PRISM_PROGRESS_EXPIRE_TIMER_ID);

	switch (lKeyId)
	{
	case KEY_VOLUMEUP:
	case KEY_VOLUMEDOWN:
	case KEY_MUTE:
		local_PRISM_progress_Setting(pstContents, lKeyId);
		return ONDK_MESSAGE_CONSUMED;

		/* keys to exit */
	case KEY_OK:
	case KEY_BACK:
	case KEY_EXIT:
		ONDK_GWM_APP_Destroy(0);
		return ONDK_MESSAGE_CONSUMED;

	default:
		return ONDK_MESSAGE_PASS;
	}

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);
}


static ONDK_Result_t 	local_PRISM_progress_MsgGwmTimer(NxProgress_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32		ulTimerID = p1;

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! Please check!! \n");
		return	ONDK_RET_FAIL;
	}

	if (ulTimerID == NX_PRISM_PROGRESS_EXPIRE_TIMER_ID)
	{
		if (pstContents->isMute != TRUE)
		{
			ONDK_GWM_APP_Destroy(0); //mute 상태는 숨지 않는다.
		}
		else
		{
			// Not To do
		}
		return	ONDK_MESSAGE_BREAK;
	}

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	local_PRISM_progress_MsgGwmDestroy(NxProgress_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! Please check!! \n");
		return	ONDK_RET_FAIL;
	}


	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	return	local_PRISM_progress_DestroyVolume(pstContents);
}

static ONDK_Result_t	local_PRISM_progress_MsgGwmHapiMsg(NxProgress_Context_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! Please check!! \n");
		return	ONDK_RET_FAIL;
	}

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	return	ONDK_MESSAGE_PASS;
}


/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/
ONDK_Result_t		NX_PRISM_progress_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t		eResult	= ONDK_RET_OK;
	NxProgress_Context_t *pstContents = local_PRISM_progress_ProgressGetContents();

	switch (lMessage)
	{
		case MSG_GWM_CREATE:
			eResult = local_PRISM_progress_MsgGwmCreate(pstContents, p1, p2, p3);
			break;
		case MSG_GWM_KEYUP:
			eResult = local_PRISM_progress_MsgGwmKeyUp(pstContents, p1, p2, p3);
			break;
		case MSG_GWM_KEYDOWN:
			eResult = local_PRISM_progress_MsgGwmKeyDown(pstContents, p1, p2, p3);
			break;
		case MSG_GWM_TIMER:
			eResult = local_PRISM_progress_MsgGwmTimer(pstContents, p1, p2, p3);
			break;
		case MSG_GWM_DESTROY:
			eResult = local_PRISM_progress_MsgGwmDestroy(pstContents, p1, p2, p3);
			break;
		case MSG_GWM_HAPI_MESSAGE:
			//eResult = local_PRISM_progress_MsgGwmHapiMsg(pstContents, handle, p1, p2, p3);
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

