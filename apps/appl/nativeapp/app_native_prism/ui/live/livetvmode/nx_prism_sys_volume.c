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
 * @file	  		nx_prism_sys_volume.c
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

#include <nx_core.h>
#include <nx_port.h>


/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/
#define	NX_PRISM_VOLUME_ITEM_ID					eMenuFrame_Window_Volume

#define	NX_PRISM_VOLUME_EXPIRE_TIMER_ID			(NX_PRISM_VOLUME_ITEM_ID + 1)
#define	NX_PRISM_VOLUME_EXPIRE_TIME				(3 * NX_COMMON_TIME_SECOND)

// Object ID
#define	NX_PRISM_VOLUME_FRAME_ID				(NX_PRISM_VOLUME_ITEM_ID + 2)
#define	NX_PRISM_VOLUME_NUMTEXT_ID				(NX_PRISM_VOLUME_ITEM_ID + 3)
#define	NX_PRISM_VOLUME_PROGRESS_ID				(NX_PRISM_VOLUME_ITEM_ID + 4)

#define	NX_PRISM_VOLUME_FRAME_X					1020
#define	NX_PRISM_VOLUME_FRAME_Y					0
#define	NX_PRISM_VOLUME_FRAME_W					260
#define	NX_PRISM_VOLUME_FRAME_H					720

#define	NX_PRISM_VOLUME_PROGRESS_ITEM			20
#define	NX_PRISM_VOLUME_PROGRESS_X				144
#define	NX_PRISM_VOLUME_PROGRESS_Y				188
#define	NX_PRISM_VOLUME_PROGRESS_W				36
#if defined(WIN32)
#define	NX_PRISM_VOLUME_PROGRESS_H				10
#else
//#define	NX_PRISM_VOLUME_PROGRESS_H				8
#define	NX_PRISM_VOLUME_PROGRESS_H				10
#endif
#define	NX_PRISM_VOLUME_PROGRESS_VS				5

#define	NX_PRISM_VOLUME_NUMTEXT_X				127
#define	NX_PRISM_VOLUME_NUMTEXT_Y				506
#define	NX_PRISM_VOLUME_NUMTEXT_W				70
#define	NX_PRISM_VOLUME_NUMTEXT_H				(NX_PRISM_FONT_SIZE_36 + NX_PRISM_FONT_VSPACE)


/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/
typedef struct NxVolume_Context
{
	HUINT32			ulCurFocus;
	HUINT32			ulNumOfSubItem, ulMaxNumOfSubItem;
	HINT32			appvolume;
	HINT32          octovolume;
	HINT32          volumeMax;
	HINT32          volumeMin;
	HBOOL           isMute;

	HCHAR			szNumText[NX_TEXT_SIZE_64];
} NxVolume_Context_t;


/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/
static	HUINT32	s_gvolumetable[NX_SYS_VOLUME_STEP +1]={
	0,3,5,7,10,13,15,17,20,23, 25,27,30,33,35,37,40,43,45,47,50
};


static NxVolume_Context_t		s_stVolume_Contents;


/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/

static ONDK_Result_t	nx_volume_ResponseDialogProc(HINT32 lMessage, Handle_t hHandle, HINT32 lParam1, HINT32 lParam2, HINT32 lParam3);
static void				nx_volume_CreateResponseDialouge(HUINT8* pszString);

static NxVolume_Context_t*	nx_volume_VolumeGetContents (void);
static HUINT32 			nx_volume_ConvertOctoToAPP(HUINT32 ulVolume);
static ONDK_Result_t	nx_volume_CheckDolbyAudioByPass(void);
static ONDK_Result_t	nx_volume_Initialize(NxVolume_Context_t *pstContents);
static void				nx_volume_Setting(NxVolume_Context_t *pstContents, HINT32 lKey);
static ONDK_Result_t 	nx_volume_SetUiObject(NxVolume_Context_t *pstContents);

static ONDK_Result_t	nx_volume_DestroyVolume(NxVolume_Context_t *pstContents);
static ONDK_Result_t 	nx_volume_MsgGwmCreate(NxVolume_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	nx_volume_MsgGwmDestroy(NxVolume_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	nx_volume_MsgGwmKeyUp(NxVolume_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	nx_volume_MsgGwmKeyDown(NxVolume_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_volume_MsgGwmHapiMsg(NxVolume_Context_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	nx_volume_MsgGwmTimer(NxVolume_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);


/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/

/******************************************************************************/
#define __Response_Dialog_Functions___________________________________________
/******************************************************************************/

static ONDK_Result_t	nx_volume_ResponseDialogProc(HINT32 lMessage, Handle_t hHandle, HINT32 lParam1, HINT32 lParam2, HINT32 lParam3)
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
					ONDK_GWM_APP_Destroy(0);
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


static void		nx_volume_CreateResponseDialouge(HUINT8* pszString)
{
	if (pszString == NULL)
	{
		NX_APP_Error("Error!!! Pls check pszString was NULL!! \n");
		return;
	}

	ONDK_GWM_APP_Create (ONDK_UPPER_WINDOW, "nx_volume_CreateResponseDialouge", APP_DEFAULT_PRIORITY,
							nx_volume_ResponseDialogProc, 0, (HINT32)pszString, 0, 0);
}


/******************************************************************************/
#define __Local_Functions______________________________________________________
/******************************************************************************/

static NxVolume_Context_t*	nx_volume_VolumeGetContents(void)
{
	return &s_stVolume_Contents;
}


static HUINT32	nx_volume_ConvertOctoToAPP(HUINT32 ulVolume)
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


static ONDK_Result_t	nx_volume_CheckDolbyAudioByPass(void)
{
	HERROR				hError = ERR_FAIL;
	HBOOL				bFindDolby =  FALSE;
	DxAudioCodec_e		eAudioCodec = eDxAUDIO_CODEC_UNKNOWN;
	DxDigitalAudioFormat_e		eHDMI = eDxDIGITALAUDIO_FORMAT_UNKNOWN;


	NX_PRISM_LIVE_GetCurChannelAudioCodec(&eAudioCodec);
	switch (eAudioCodec)
	{
		case eDxAUDIO_CODEC_DOLBY_AC3:
		case eDxAUDIO_CODEC_DOLBY_AC3P:
			bFindDolby = TRUE;
			break;
		default:
			bFindDolby = FALSE;
			NX_APP_Info("Other Audio Codec : (%d)\n", eAudioCodec);
			break;
	}

	hError = NX_SYSTEM_GetHdmiAudioOutputMode(&eHDMI);
	if (hError != ERR_OK)
	{
		NX_APP_Error("Error!!! NX_SYSTEM_GetHdmiAudioOutputMode() Fail !!\n");
		return	ONDK_RET_FAIL;
	}
	else
	{
		if ((bFindDolby == TRUE) && (eHDMI == eDxDIGITALAUDIO_FORMAT_MULTICHANNEL))
		{
			NX_PRISM_DOLBY_ShowDolbyLogo(eAudioCodec);
			NX_APP_Info("HDMI Audio was set eDxDIGITALAUDIO_FORMAT_MULTICHANNEL!! \n");
			return	ONDK_RET_OK;
		}
		else
		{
			return	ONDK_RET_FAIL;
		}
	}
}


static ONDK_Result_t	nx_volume_Initialize(NxVolume_Context_t *pstContents)
{
	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! Please check!! \n");
		return	ONDK_RET_FAIL;
	}

	pstContents->volumeMin = NX_SYS_VOLUME_MIN;
	NX_SYSTEM_GetVolume(&pstContents->octovolume);
	NX_SYSTEM_GetVolumeMax(&pstContents->volumeMax);
    NX_SYSTEM_GetMute(&pstContents->isMute);

	if(pstContents->volumeMax - pstContents->volumeMin > 0)
	{
		pstContents->appvolume = (pstContents->octovolume * NX_SYS_VOLUME_STEP) / (pstContents->volumeMax - pstContents->volumeMin);
	}
	HxSTD_sprintf(pstContents->szNumText, "%d", pstContents->appvolume);

	return	ONDK_RET_OK;
}


static void		nx_volume_Setting(NxVolume_Context_t *pstContents, HINT32 lKey)
{
	HBOOL	bDrawPanelVolText = FALSE;
	HUINT8	*pszStr = NULL;
	HCHAR	szPanelStr[NX_TEXT_SIZE_32] = {0, };

	switch (lKey)
	{
		case KEY_VOLUMEUP:
			if (NX_SYS_VOLUME_MAX > pstContents->appvolume)
			{
				++pstContents->appvolume;
				pstContents->octovolume = (pstContents->volumeMax - pstContents->volumeMin)/NX_SYS_VOLUME_STEP * pstContents->appvolume;
				NX_APP_Print("volume-octo [%d] volume-app[%d]\n",pstContents->octovolume,pstContents->appvolume);
				NX_SYSTEM_SetVolume(pstContents->octovolume);
				NX_APP_Print("volume-octo [%d] volume-app[%d]\n",s_gvolumetable[pstContents->appvolume],pstContents->appvolume);
			}
			bDrawPanelVolText = TRUE;
			break;

		case KEY_VOLUMEDOWN:
			if (NX_SYS_VOLUME_MIN < pstContents->appvolume)
			{
				--pstContents->appvolume;
				pstContents->octovolume = (pstContents->volumeMax - pstContents->volumeMin)/NX_SYS_VOLUME_STEP * pstContents->appvolume;
				NX_APP_Print("volume-octo [%d] volume-app[%d]\n",pstContents->octovolume,pstContents->appvolume);
				NX_SYSTEM_SetVolume(pstContents->octovolume);
				NX_APP_Print("volume-octo [%d] volume-app[%d]\n",s_gvolumetable[pstContents->appvolume],pstContents->appvolume);
			}
			bDrawPanelVolText = TRUE;
			break;

		default:
			return;
 	}

	ONDK_GWM_Obj_SetHide(NX_PRISM_VOLUME_NUMTEXT_ID, pstContents->isMute);

	if (bDrawPanelVolText == TRUE)
	{
		pszStr = ONDK_GetString(RES_VOLUME_ID);
		if(pszStr == NULL) pszStr = (HUINT8*)"Volume";

		HxSTD_sprintf(szPanelStr, "%s %d", pszStr, pstContents->appvolume);
		NX_PANEL_SetString(szPanelStr, eDxPANEL_ALIGN_CENTER, FALSE);
		HxSTD_sprintf(pstContents->szNumText, "%d", pstContents->appvolume);
		ONDK_GWM_COM_VProgress_SetPos(NX_PRISM_VOLUME_PROGRESS_ID, pstContents->appvolume);
	}

	ONDK_GWM_APP_InvalidateAPP(ONDK_GWM_APP_GetThis());
}


static ONDK_Result_t nx_volume_DestroyVolume(NxVolume_Context_t *pstContents)
{
	ONDK_GWM_KillTimer(NX_PRISM_VOLUME_EXPIRE_TIMER_ID);

	NX_COMMON_NOT_USED_PARAM(pstContents);
	return ONDK_RET_OK;
}


static ONDK_Result_t 	nx_volume_SetUiObject(NxVolume_Context_t *pstContents)
{
	HINT32			h = 0;
	ONDK_Rect_t 	stRect;

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! Please check!! \n");
		return	ONDK_RET_FAIL;
	}

	ONDK_GWM_APP_SetAppArea(NX_PRISM_VOLUME_FRAME_X, NX_PRISM_VOLUME_FRAME_Y, NX_PRISM_VOLUME_FRAME_W, NX_PRISM_VOLUME_FRAME_H);

	stRect = ONDK_Rect(0, 0, NX_PRISM_VOLUME_FRAME_W, NX_PRISM_VOLUME_FRAME_H);
	ONDK_GWM_COM_Image_Create(NX_PRISM_VOLUME_FRAME_ID, &stRect);
	ONDK_GWM_COM_Image_SetImage(NX_PRISM_VOLUME_FRAME_ID, RES_105_00_BG_COM_CONTEXT_R02_PNG);

	h = (NX_PRISM_VOLUME_PROGRESS_H * NX_PRISM_VOLUME_PROGRESS_ITEM) + (NX_PRISM_VOLUME_PROGRESS_VS* (NX_PRISM_VOLUME_PROGRESS_ITEM-1));
	stRect = ONDK_Rect(NX_PRISM_VOLUME_PROGRESS_X, NX_PRISM_VOLUME_PROGRESS_Y,
						NX_PRISM_VOLUME_PROGRESS_W, h);
	ONDK_GWM_COM_VProgress_Create(NX_PRISM_VOLUME_PROGRESS_ID, &stRect, NX_SYS_VOLUME_MIN, NX_SYS_VOLUME_MAX, 0);
	ONDK_GWM_COM_VProgress_SetBlankImg(NX_PRISM_VOLUME_PROGRESS_ID, RES_580_00_VOL_BLANK_PNG);
	ONDK_GWM_COM_VProgress_SetFillImg(NX_PRISM_VOLUME_PROGRESS_ID, RES_580_00_VOL_FILL_PNG);
	ONDK_GWM_COM_VProgress_SetVspace(NX_PRISM_VOLUME_PROGRESS_ID, NX_PRISM_VOLUME_PROGRESS_VS);
	ONDK_GWM_COM_VProgress_SetMax(NX_PRISM_VOLUME_PROGRESS_ID, NX_SYS_VOLUME_MAX);
	ONDK_GWM_COM_VProgress_SetMin(NX_PRISM_VOLUME_PROGRESS_ID, NX_SYS_VOLUME_MIN);
	ONDK_GWM_COM_VProgress_SetPos(NX_PRISM_VOLUME_PROGRESS_ID, pstContents->appvolume);

	stRect = ONDK_Rect(NX_PRISM_VOLUME_NUMTEXT_X, NX_PRISM_VOLUME_NUMTEXT_Y,
						NX_PRISM_VOLUME_NUMTEXT_W, NX_PRISM_VOLUME_NUMTEXT_H);
	ONDK_GWM_COM_Text_Create(NX_PRISM_VOLUME_NUMTEXT_ID, &stRect, pstContents->szNumText);
	ONDK_GWM_COM_Text_SetFont(NX_PRISM_VOLUME_NUMTEXT_ID, eFont_SystemBold);
	ONDK_GWM_COM_Text_SetSize(NX_PRISM_VOLUME_NUMTEXT_ID, NX_PRISM_FONT_SIZE_36);
	ONDK_GWM_COM_Text_SetAlign(NX_PRISM_VOLUME_NUMTEXT_ID, TEXT_ALIGN_CENTER);
	ONDK_GWM_COM_Text_ChangeColor(NX_PRISM_VOLUME_NUMTEXT_ID, COL(C_T_Base01), COL(C_T_Bang_100));

	ONDK_GWM_Obj_SetHide(NX_PRISM_VOLUME_NUMTEXT_ID, pstContents->isMute);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_volume_MsgGwmCreate(NxVolume_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t		eResult = ONDK_RET_FAIL;


	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! Please check!! \n");
		return	ONDK_RET_FAIL;
	}

	eResult = nx_volume_CheckDolbyAudioByPass();
	if (eResult == ONDK_RET_OK)
	{
		NX_APP_Info("nx_volume_CheckDolbyAudioByPass() Fail !!\n");
		nx_volume_CreateResponseDialouge((HUINT8*)ONDK_GetString(RES_MESG_5677_ID));
		ONDK_GWM_APP_Destroy(0);
		return ONDK_RET_OK;
	}

	/* 절대 hide가 되지 않는다. */
	ONDK_GWM_APP_SetUIHideFlag(GWM_UI_Hide_Never);

	nx_volume_Initialize(pstContents);
	nx_volume_SetUiObject(pstContents);
	nx_volume_Setting(pstContents, p1);

	ONDK_GWM_SetTimer(NX_PRISM_VOLUME_EXPIRE_TIMER_ID, NX_PRISM_VOLUME_EXPIRE_TIME);

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	return ONDK_RET_OK;
}


static ONDK_Result_t	nx_volume_MsgGwmKeyUp(NxVolume_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
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
			return ONDK_MESSAGE_PASS;
		default:
			break;
	}

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);
	return ONDK_MESSAGE_NONCONSUMED;
}

static ONDK_Result_t	nx_volume_MsgGwmKeyDown(NxVolume_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32		lKeyId = p1;

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! Please check!! \n");
		return	ONDK_RET_FAIL;
	}

	ONDK_GWM_ResetTimer(NX_PRISM_VOLUME_EXPIRE_TIMER_ID);

	switch (lKeyId)
	{
		case KEY_VOLUMEUP:
		case KEY_VOLUMEDOWN:
			nx_volume_Setting(pstContents, lKeyId);
			return ONDK_MESSAGE_BREAK;

			/* keys to exit */
		case KEY_OK:
		case KEY_BACK:
		case KEY_EXIT:
			ONDK_GWM_APP_Destroy(0);
			return ONDK_MESSAGE_BREAK;

		case KEY_STANDBY:
		case KEY_GUIDE:
			ONDK_GWM_APP_Destroy(0);
			return ONDK_MESSAGE_PASS;

		default:
			return ONDK_MESSAGE_PASS;
	}

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);
}


static ONDK_Result_t 	nx_volume_MsgGwmTimer(NxVolume_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32		ulTimerID = p1;

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! Please check!! \n");
		return	ONDK_RET_FAIL;
	}

	if (ulTimerID == NX_PRISM_VOLUME_EXPIRE_TIMER_ID)
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


static ONDK_Result_t	nx_volume_MsgGwmDestroy(NxVolume_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! Please check!! \n");
		return	ONDK_RET_FAIL;
	}

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	return	nx_volume_DestroyVolume(pstContents);
}


static ONDK_Result_t	nx_volume_MsgGwmHapiMsg(NxVolume_Context_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! Please check!! \n");
		return	ONDK_RET_FAIL;
	}
	NX_COMMON_NOT_USED_PARAM(handle);
	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	return	ONDK_MESSAGE_PASS;
}


/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/
ONDK_Result_t		NX_PRISM_SYSTEMVolume_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t		eResult	= ONDK_RET_OK;
	NxVolume_Context_t *pstContents = nx_volume_VolumeGetContents();

	switch (lMessage)
	{
		case MSG_GWM_CREATE:
			eResult = nx_volume_MsgGwmCreate(pstContents, p1, p2, p3);
			break;
		case MSG_GWM_KEYUP:
			eResult = nx_volume_MsgGwmKeyUp(pstContents, p1, p2, p3);
			break;
		case MSG_GWM_KEYDOWN:
			eResult = nx_volume_MsgGwmKeyDown(pstContents, p1, p2, p3);
			break;
		case MSG_GWM_TIMER:
			eResult = nx_volume_MsgGwmTimer(pstContents, p1, p2, p3);
			break;
		case MSG_GWM_DESTROY:
			eResult = nx_volume_MsgGwmDestroy(pstContents, p1, p2, p3);
			break;
		case MSG_GWM_HAPI_MESSAGE:
			eResult = nx_volume_MsgGwmHapiMsg(pstContents, handle, p1, p2, p3);
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

