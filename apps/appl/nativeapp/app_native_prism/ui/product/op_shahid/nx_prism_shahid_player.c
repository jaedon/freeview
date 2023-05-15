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
 * @file	  		nx_prism_shahid_player.c
 *
 * 	Description:  													\n
 *
 * @author															\n
 * @date															\n
 * @Attention		Copyright (c) 2015 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/
#if defined(CONFIG_OP_SHAHID_APPS)
/******************************************************************************/
#define _________Private_Include______________________________________________
/******************************************************************************/
#include <nx_common.h>

#include <nx_prism_app.h>
#include <nx_prism_ui.h>

/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/
#define	NX_PRISM_SHAHID_PLAYER_ITEM_ID					eMenuFrame_Window_Shahid_Player

#define	NX_SHAHID_PLAYER_WINDOW_X						0
#define	NX_SHAHID_PLAYER_WINDOW_Y						0
#define	NX_SHAHID_PLAYER_WINDOW_W						NX_PRISM_OSD_HORIZONTAL
#define	NX_SHAHID_PLAYER_WINDOW_H						NX_PRISM_OSD_VERTICAL

#define NX_SHAHID_PLAYER_DIM_IMG_ID						NX_PRISM_SHAHID_PLAYER_ITEM_ID+1
#define NX_SHAHID_PLAYER_DIM_IMG_X						0
#define NX_SHAHID_PLAYER_DIM_IMG_Y						320
#define NX_SHAHID_PLAYER_DIM_IMG_W						1280
#define NX_SHAHID_PLAYER_DIM_IMG_H						400

#define NX_SHAHID_PLAYER_TITLE_ID						NX_PRISM_SHAHID_PLAYER_ITEM_ID+2
#define NX_SHAHID_PLAYER_TITLE_X						60
#define NX_SHAHID_PLAYER_TITLE_Y						526
#define NX_SHAHID_PLAYER_TITLE_W						1160
#define NX_SHAHID_PLAYER_TITLE_H						40

#define NX_SHAHID_PLAYER_PROGRESSBAR_BG_ID				NX_PRISM_SHAHID_PLAYER_ITEM_ID+3
#define NX_SHAHID_PLAYER_PROGRESSBAR_BG_X				60
#define NX_SHAHID_PLAYER_PROGRESSBAR_BG_Y				584
#define NX_SHAHID_PLAYER_PROGRESSBAR_BG_W				1160
#define NX_SHAHID_PLAYER_PROGRESSBAR_BG_H				6

#define NX_SHAHID_PLAYER_PROGRESSBAR_BAR_ID				NX_PRISM_SHAHID_PLAYER_ITEM_ID+4
#define NX_SHAHID_PLAYER_PROGRESSBAR_BAR_X				60
#define NX_SHAHID_PLAYER_PROGRESSBAR_BAR_Y				584
#define NX_SHAHID_PLAYER_PROGRESSBAR_BAR_H				6
#define NX_SHAHID_PLAYER_PROGRESSBAR_BAR_W_MAX			1160

#define NX_SHAHID_PLAYER_PROGRESSBAR_PTR_ID				NX_PRISM_SHAHID_PLAYER_ITEM_ID+5
#define NX_SHAHID_PLAYER_PROGRESSBAR_PTR_X_MIN			41
#define NX_SHAHID_PLAYER_PROGRESSBAR_PTR_X_MAX			41+NX_SHAHID_PLAYER_PROGRESSBAR_BAR_W_MAX
#define NX_SHAHID_PLAYER_PROGRESSBAR_PTR_Y				567
#define NX_SHAHID_PLAYER_PROGRESSBAR_PTR_W				40
#define NX_SHAHID_PLAYER_PROGRESSBAR_PTR_H				40

#define NX_SHAHID_PLAYER_CURRENT_TIME_ID				NX_PRISM_SHAHID_PLAYER_ITEM_ID+6
#define NX_SHAHID_PLAYER_CURRENT_TIME_X					60
#define NX_SHAHID_PLAYER_CURRENT_TIME_Y					636
#define NX_SHAHID_PLAYER_CURRENT_TIME_W					106
#define NX_SHAHID_PLAYER_CURRENT_TIME_H					30

#define NX_SHAHID_PLAYER_TOTAL_TIME_ID					NX_PRISM_SHAHID_PLAYER_ITEM_ID+7
#define NX_SHAHID_PLAYER_TOTAL_TIME_X					169
#define NX_SHAHID_PLAYER_TOTAL_TIME_Y					636
#define NX_SHAHID_PLAYER_TOTAL_TIME_W					106
#define NX_SHAHID_PLAYER_TOTAL_TIME_H					30

#define NX_SHAHID_PLAYER_BTN_SR_ID						NX_PRISM_SHAHID_PLAYER_ITEM_ID+8
#define NX_SHAHID_PLAYER_BTN_SR_X						461

#define NX_SHAHID_PLAYER_BTN_Y							610
#define NX_SHAHID_PLAYER_BTN_W							70
#define NX_SHAHID_PLAYER_BTN_H							70
#define NX_SHAHID_PLAYER_BTN_MARGIN						2

#define NX_SHAHID_PLAYER_BTN_FR_ID						NX_PRISM_SHAHID_PLAYER_ITEM_ID+9
#define NX_SHAHID_PLAYER_BTN_FR_X						NX_SHAHID_PLAYER_BTN_SR_X+NX_SHAHID_PLAYER_BTN_W+NX_SHAHID_PLAYER_BTN_MARGIN

#define NX_SHAHID_PLAYER_BTN_PLAY_PAUSE_ID				NX_PRISM_SHAHID_PLAYER_ITEM_ID+10
#define NX_SHAHID_PLAYER_BTN_PLAY_PAUSE_X				NX_SHAHID_PLAYER_BTN_FR_X+NX_SHAHID_PLAYER_BTN_W+NX_SHAHID_PLAYER_BTN_MARGIN

#define NX_SHAHID_PLAYER_BTN_FF_ID						NX_PRISM_SHAHID_PLAYER_ITEM_ID+11
#define NX_SHAHID_PLAYER_BTN_FF_X						NX_SHAHID_PLAYER_BTN_PLAY_PAUSE_X+NX_SHAHID_PLAYER_BTN_W+NX_SHAHID_PLAYER_BTN_MARGIN

#define NX_SHAHID_PLAYER_BTN_SF_ID						NX_PRISM_SHAHID_PLAYER_ITEM_ID+12
#define NX_SHAHID_PLAYER_BTN_SF_X						NX_SHAHID_PLAYER_BTN_FF_X+NX_SHAHID_PLAYER_BTN_W+NX_SHAHID_PLAYER_BTN_MARGIN

#define NX_SHAHID_PLAYER_IMG_SPEED_ID					NX_PRISM_SHAHID_PLAYER_ITEM_ID+13
#define NX_SHAHID_PLAYER_IMG_SPEED_X					1135
#define NX_SHAHID_PLAYER_IMG_SPEED_Y					47
#define NX_SHAHID_PLAYER_IMG_SPEED_W					64
#define NX_SHAHID_PLAYER_IMG_SPEED_H					30

#define NX_SHAHID_PLAYER_IMG_SPEED_BG_ID				NX_PRISM_SHAHID_PLAYER_ITEM_ID+14
#define NX_SHAHID_PLAYER_IMG_SPEED_BG_X					1107
#define NX_SHAHID_PLAYER_IMG_SPEED_BG_Y					26
#define NX_SHAHID_PLAYER_IMG_SPEED_BG_W					128
#define NX_SHAHID_PLAYER_IMG_SPEED_BG_H					72

#define NX_SHAHID_PLAYER_HIDE_TIMER_ID					NX_PRISM_SHAHID_PLAYER_ITEM_ID+15
#define NX_SHAHID_PLAYER_HIDE_TIMER_TIMEOUT				10000

#define NX_SHAHID_PLAYER_TEMP_TIMER_ID					NX_PRISM_SHAHID_PLAYER_ITEM_ID+16
#define NX_SHAHID_PLAYER_TEMP_TIMER_PERIOD				1000

#define NX_SHAHID_PLAYER_CONTENT_ID						0





/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/

typedef struct tagShahidPlayerContext{

	HUINT32			ulCurFocusIndex;
	//HUINT32			ulPointerPosition;
	HUINT32			ulProgressValue;

	HBOOL			isPlaying;

	HINT32			ulCurrentPlayTime;
	HUINT32			ulDuration;
	HINT32			lSpeed;
	HBOOL			bHide;

	HCHAR			szCurrentPlayTime[NX_TEXT_SIZE_64];
	HCHAR			szDuration[NX_TEXT_SIZE_64];
	HCHAR			szTitle[NX_TEXT_SIZE_64];

}Nx_ShahidPlayerContext_t;

/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/
static Nx_ShahidPlayerContext_t		s_stShahidPlayerContext;
/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/
static inline Nx_ShahidPlayerContext_t	*nx_shahid_Player_GetContext(void);
static ONDK_Result_t	nx_shahid_player_MsgGwmCreate(Nx_ShahidPlayerContext_t *pstContext, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_shahid_player_MsgGwmDestroy(Nx_ShahidPlayerContext_t *pstContext, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_shahid_player_MsgGwmKeyDown(Nx_ShahidPlayerContext_t *pstContext, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_shahid_player_MsgGwmTimer(Nx_ShahidPlayerContext_t *pstContext, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_shahid_player_MsgPlayStateChanged(Nx_ShahidPlayerContext_t *pstContext, HINT32 p1, HINT32 p2, HINT32 p3);

static ONDK_Result_t	nx_shahid_player_DrawUiObject(Nx_ShahidPlayerContext_t *pstContext);
static ONDK_Result_t	nx_shahid_player_SetProgrammeTitle(Nx_ShahidPlayerContext_t *pstContext, NxPlayerContentInform_t *pstContentsInform);
static ONDK_Result_t	nx_shahid_player_UpdateProgressData(Nx_ShahidPlayerContext_t *pstContext, NxPlayerContentInform_t *pstContentsInform);
static ONDK_Result_t	nx_shahid_player_UpdateProgressUI(Nx_ShahidPlayerContext_t *pstContext);
static ONDK_Result_t	nx_shahid_player_BtnDrawFunc(ONDKSurfaceHandle screen, ONDK_Region_t *area,
										ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop);
static ONDK_Result_t	nx_shahid_player_PlayPause(Nx_ShahidPlayerContext_t *pstContext);
static ONDK_Result_t	nx_shahid_player_ProcessKeyOK(Nx_ShahidPlayerContext_t *pstContext);

static ONDK_Result_t	nx_shahid_player_GetTimeStr(HCHAR* pszTime, HINT32 ulTime);

static void	nx_shahid_player_HideUI(HBOOL bHide);

/******************************************************************************/
#define _________Private_functions____________________________________________
static inline Nx_ShahidPlayerContext_t	*nx_shahid_Player_GetContext(void)
{
	return &s_stShahidPlayerContext;
}

static ONDK_Result_t	nx_shahid_player_SetProgrammeTitle(Nx_ShahidPlayerContext_t * pstContext, NxPlayerContentInform_t *pstContentsInform)
{
	//set title
	HxSTD_StrNCpy(pstContext->szTitle, "Test title ... Season @@ Episode @@", NX_TEXT_SIZE_64);
	return ONDK_RET_OK;
}

static ONDK_Result_t	nx_shahid_player_UpdateProgressData(Nx_ShahidPlayerContext_t * pstContext, NxPlayerContentInform_t *pstContentsInform)
{
	if(pstContentsInform == NULL)
	{
		NX_APP_Error("pstContentsInform is NULL\n");
		return ONDK_RET_INITFAIL;
	}

	// Set Current position
	if(pstContext->lSpeed != 0)
	{
		if(pstContentsInform->ulPlayMaxTime == 0)
		{
			pstContext->ulProgressValue= 0;
			nx_shahid_player_GetTimeStr((HCHAR*)pstContext->szCurrentPlayTime, 0);
		}
		else
		{
			if((HINT32)(pstContext->ulCurrentPlayTime) != (HINT32)pstContentsInform->ulPlayTime)
			{
				pstContext->ulCurrentPlayTime = pstContentsInform->ulPlayTime;
				pstContext->ulProgressValue= (NX_SHAHID_PLAYER_PROGRESSBAR_BAR_W_MAX)*((HFLOAT32)pstContentsInform->ulPlayTime/(HFLOAT32)pstContentsInform->ulPlayMaxTime);
				nx_shahid_player_GetTimeStr((HCHAR*)pstContext->szCurrentPlayTime, pstContext->ulCurrentPlayTime);
			}
		}
		ONDK_GWM_APP_InvalidateObject(NX_SHAHID_PLAYER_CURRENT_TIME_ID);
	}
	// Set total time
	if(pstContext->ulDuration != pstContentsInform->ulPlayMaxTime)
	{
		if(pstContentsInform->ulPlayMaxTime > 0)
		{
			pstContext->ulDuration = pstContentsInform->ulPlayMaxTime;
		}
		else
		{
			pstContext->ulDuration = 0;
		}
		nx_shahid_player_GetTimeStr((HCHAR*)pstContext->szDuration, pstContext->ulDuration);
		ONDK_GWM_APP_InvalidateObject(NX_SHAHID_PLAYER_TOTAL_TIME_ID);
	}


	return ONDK_RET_OK;
}

static ONDK_Result_t	nx_shahid_player_UpdateProgressUI(Nx_ShahidPlayerContext_t * pstContext)
{
	ONDK_Rect_t stRect;

	if(pstContext->ulProgressValue == 0)
	{
		pstContext->ulProgressValue ++;
	}

	stRect = ONDK_Rect(NX_SHAHID_PLAYER_PROGRESSBAR_BAR_X, NX_SHAHID_PLAYER_PROGRESSBAR_BAR_Y, pstContext->ulProgressValue, NX_SHAHID_PLAYER_PROGRESSBAR_BAR_H);
	ONDK_GWM_COM_Rect_ChangeSize(NX_SHAHID_PLAYER_PROGRESSBAR_BAR_ID,NX_SHAHID_PLAYER_PROGRESSBAR_BAR_X,NX_SHAHID_PLAYER_PROGRESSBAR_BAR_Y,
		pstContext->ulProgressValue,NX_SHAHID_PLAYER_PROGRESSBAR_BAR_H);
	ONDK_GWM_APP_InvalidateObject(NX_SHAHID_PLAYER_PROGRESSBAR_BAR_ID);

	stRect = ONDK_Rect(NX_SHAHID_PLAYER_PROGRESSBAR_PTR_X_MIN+pstContext->ulProgressValue, NX_SHAHID_PLAYER_PROGRESSBAR_PTR_Y,
		NX_SHAHID_PLAYER_PROGRESSBAR_PTR_W, NX_SHAHID_PLAYER_PROGRESSBAR_PTR_H);
	ONDK_GWM_COM_Image_ChangeRect(NX_SHAHID_PLAYER_PROGRESSBAR_PTR_ID,&stRect);
	ONDK_GWM_APP_InvalidateObject(NX_SHAHID_PLAYER_PROGRESSBAR_PTR_ID);

	return ONDK_RET_OK;
}

static ONDK_Result_t	nx_shahid_player_UpdateSpeedIconObj(Nx_ShahidPlayerContext_t *pstContext)
{
	HCHAR *pszImgPath = NULL;
	switch(pstContext->lSpeed)
	{
		case -16:
			pszImgPath = RES_602_11_SHAHID_PB_X16_PNG;
			break;
		case -8:
			pszImgPath = RES_602_11_SHAHID_PB_X8_PNG;
			break;
		case -4:
			pszImgPath = RES_602_11_SHAHID_PB_X4_PNG;
			break;
		case -2:
			pszImgPath = RES_602_11_SHAHID_PB_X2_PNG;
			break;
		case 1:
			break;
		case 0:
			break;
		case 2:
			pszImgPath = RES_602_11_SHAHID_PB_X2_PNG;
			break;
		case 4:
			pszImgPath = RES_602_11_SHAHID_PB_X4_PNG;
			break;
		case 8:
			pszImgPath = RES_602_11_SHAHID_PB_X8_PNG;
			break;
		case 16:
			pszImgPath = RES_602_11_SHAHID_PB_X16_PNG;
			break;
		default:
			HxLOG_Print("Play speed out of range\n");
			return ONDK_RET_OK;
	}

	if(pstContext->lSpeed == 1 || pstContext->lSpeed == 0)
	{
		ONDK_GWM_COM_Rect_ChangeColor(NX_SHAHID_PLAYER_IMG_SPEED_BG_ID, 0);
		ONDK_GWM_Obj_SetHide(NX_SHAHID_PLAYER_IMG_SPEED_ID, TRUE);
	}
	else
	{
		ONDK_GWM_COM_Rect_ChangeColor(NX_SHAHID_PLAYER_IMG_SPEED_BG_ID, COL(C_G_Bang_50));
		ONDK_GWM_Obj_SetHide(NX_SHAHID_PLAYER_IMG_SPEED_ID, FALSE);
		ONDK_GWM_COM_Image_SetImage(NX_SHAHID_PLAYER_IMG_SPEED_ID, pszImgPath);
	}

	ONDK_GWM_APP_InvalidateObject(NX_SHAHID_PLAYER_IMG_SPEED_BG_ID);
	ONDK_GWM_APP_InvalidateObject(NX_SHAHID_PLAYER_IMG_SPEED_ID);

	return ONDK_RET_OK;
}

static ONDK_Result_t	nx_shahid_player_BtnDrawFunc(ONDKSurfaceHandle screen, ONDK_Region_t *area,
										ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	HINT32		x1 = 0, y1 = 0, x2 = 0, y2 = 0;
	HINT32		w = 0, h = 0;

	HUINT32 	ulStatus = 0;

	ONDK_Size_t stImgSize;
	ONDKImageHandle		hNormalImg, hFocusedImg, hDisabledImg, hPushedImg;

	ONDK_Color_t	cursorColor = 0x00000000;
	BUTTON_OBJECT	objBtn = (BUTTON_OBJECT)object;


	/* init coordinate */
	x1 = area->x1 + rect->x;
	y1 = area->y1 + rect->y;
	x2 = area->x1 + rect->x + rect->w;
	y2 = area->y1 + rect->y + rect->h;

	w = rect->w;
	h = rect->h;

	cursorColor = COL(C_G_Shahid_03);

	ulStatus = ONDK_GWM_COM_Button_GetStatusByObj(objBtn);

	if (bUiTop > 0)
	{
		if (ulStatus & GUIOBJ_PUSHED)
		{
			/* button feedback */
			cursorColor = COL(C_G_Shahid_03);
			ONDK_DRAW_FillRectElement(screen, x1, y1, w, h, cursorColor);
		}
		else if (ulStatus & GUIOBJ_HILIGHTED)
		{
			/* get focused */
			cursorColor = COL(C_G_Shahid_03);
			ONDK_DRAW_FillRectElement(screen, x1, y1, w, h, cursorColor);
		}
	}

	// Draw image in button
	ONDK_GWM_COM_Button_GetImageHandleByObj(objBtn, &hNormalImg, &hFocusedImg, &hDisabledImg, &hPushedImg);

	if (( ulStatus& GUIOBJ_DISABLED) && (hDisabledImg != NULL))
	{
		stImgSize = ONDK_IMG_GetSize(hDisabledImg);
		ONDK_IMG_Blit(hDisabledImg, screen, x1+(w/2)-(stImgSize.w/2), (y1+(h/2)-(stImgSize.h/2)));
	}
	else if ((ulStatus & GUIOBJ_PUSHED) && (hPushedImg != NULL))
	{
		stImgSize = ONDK_IMG_GetSize(hPushedImg);
		ONDK_IMG_Blit(hPushedImg, screen, x1+(w/2)-(stImgSize.w/2), (y1+(h/2)-(stImgSize.h/2)));
	}
	else if ((ulStatus & GUIOBJ_HILIGHTED) && (hFocusedImg != NULL))
	{
		stImgSize = ONDK_IMG_GetSize(hFocusedImg);
		ONDK_IMG_Blit(hFocusedImg, screen, x1+(w/2)-(stImgSize.w/2), (y1+(h/2)-(stImgSize.h/2)));
	}
	else if (hNormalImg!= NULL)
	{
		stImgSize = ONDK_IMG_GetSize(hNormalImg);
		ONDK_IMG_Blit(hNormalImg, screen, x1+(w/2)-(stImgSize.w/2), (y1+(h/2)-(stImgSize.h/2)));
	}

	NX_COMMON_NOT_USED_ARGUMENT(screen, area, rect, object);
	NX_COMMON_NOT_USED_PARAM(arg);
	NX_COMMON_NOT_USED_PARAM(bUiTop);

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_shahid_player_PlayPause(Nx_ShahidPlayerContext_t *pstContext)
{
	if(pstContext->isPlaying == FALSE)
	{
		pstContext->isPlaying = TRUE;
		pstContext->lSpeed = 1;
		ONDK_GWM_COM_Button_ChangeImage(NX_SHAHID_PLAYER_BTN_PLAY_PAUSE_ID, RES_602_11_SHAHID_PB_BTN_PAUSE_N_PNG, RES_602_11_SHAHID_PB_BTN_PAUSE_C_PNG, NULL, NULL);
	}
	else
	{
		pstContext->isPlaying = FALSE;
		pstContext->lSpeed = 0;
		ONDK_GWM_COM_Button_ChangeImage(NX_SHAHID_PLAYER_BTN_PLAY_PAUSE_ID, RES_602_11_SHAHID_PB_BTN_PLAY_N_PNG, RES_602_11_SHAHID_PB_BTN_PLAY_C_PNG, NULL, NULL);
	}
	ONDK_GWM_PostMessage(NULL, MSG_APP_PLAYER_DO_SPEED_CHANGE, 0, pstContext->lSpeed, 0, 0);
	ONDK_GWM_APP_InvalidateObject(NX_SHAHID_PLAYER_BTN_PLAY_PAUSE_ID);
	return ONDK_RET_OK;
}

static ONDK_Result_t	nx_shahid_player_Stop(Nx_ShahidPlayerContext_t *pstContext)
{
	ONDK_GWM_APP_Destroy(0);
	return ONDK_RET_OK;
}

static ONDK_Result_t	nx_shahid_player_LeftSeek(Nx_ShahidPlayerContext_t *pstContext)
{
	HINT32	ulSeekTime = 5000;

	if( (pstContext->ulCurrentPlayTime - ulSeekTime) <= 0 )
	{
		ulSeekTime = pstContext->ulCurrentPlayTime;
	}
	else
	{
		pstContext->ulCurrentPlayTime -= ulSeekTime;
	}
	pstContext->ulProgressValue= (NX_SHAHID_PLAYER_PROGRESSBAR_BAR_W_MAX)*((HFLOAT32)pstContext->ulCurrentPlayTime/(HFLOAT32)pstContext->ulDuration);
	nx_shahid_player_GetTimeStr((HCHAR*)pstContext->szCurrentPlayTime, pstContext->ulCurrentPlayTime);

	ONDK_GWM_PostMessage(NULL, MSG_APP_PLAYER_DO_LEFTSEEK, 0, ulSeekTime, 0, 0);
	return ONDK_RET_OK;
}

static ONDK_Result_t	nx_shahid_player_RightSeek(Nx_ShahidPlayerContext_t *pstContext)
{
	HINT32	ulSeekTime = 5000; // 1 sec = 1000 in shahid stream

	if( (pstContext->ulCurrentPlayTime + ulSeekTime) >= pstContext->ulDuration)
	{
		ulSeekTime = pstContext->ulDuration - pstContext->ulCurrentPlayTime;
	}
	else
	{
		pstContext->ulCurrentPlayTime += ulSeekTime;

	}

	pstContext->ulProgressValue= (NX_SHAHID_PLAYER_PROGRESSBAR_BAR_W_MAX)*((HFLOAT32)pstContext->ulCurrentPlayTime/(HFLOAT32)pstContext->ulDuration);
	nx_shahid_player_GetTimeStr((HCHAR*)pstContext->szCurrentPlayTime, pstContext->ulCurrentPlayTime);

	ONDK_GWM_PostMessage(NULL, MSG_APP_PLAYER_DO_RIGHTSEEK, 0, ulSeekTime, 0, 0);
	return ONDK_RET_OK;
}

static ONDK_Result_t	nx_shahid_player_FastForward(Nx_ShahidPlayerContext_t *pstContext)
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
			pstContext->lSpeed = pstContext->lSpeed * 2;
			break;
		case -2:
			pstContext->lSpeed = 1;
			break;
		case -4:
		case -8:
		case -16:
			pstContext->lSpeed = pstContext->lSpeed / 2;
			break;
	}

	nx_shahid_player_UpdateSpeedIconObj(pstContext);

	ONDK_GWM_PostMessage(NULL, MSG_APP_PLAYER_DO_SPEED_CHANGE, 0, pstContext->lSpeed, 0, 0);
	return ONDK_RET_OK;
}

static ONDK_Result_t	nx_shahid_player_FastRewind(Nx_ShahidPlayerContext_t *pstContext)
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
			pstContext->lSpeed = pstContext->lSpeed / 2;
			break;
		case -2:
		case -4:
		case -8:
		case -16:
			pstContext->lSpeed = pstContext->lSpeed * 2;
			break;
		default:
			pstContext->lSpeed = pstContext->lSpeed;
			break;
	}

	nx_shahid_player_UpdateSpeedIconObj(pstContext);

	ONDK_GWM_PostMessage(NULL, MSG_APP_PLAYER_DO_SPEED_CHANGE, 0, pstContext->lSpeed, 0, 0);
	return ONDK_RET_OK;
}

static ONDK_Result_t	nx_shahid_player_ProcessKeyOK(Nx_ShahidPlayerContext_t *pstContext)
{
	HINT32  objID;
	objID = ONDK_GWM_Obj_GetObjectID(ONDK_GWM_Obj_GetFocusedObject());

	switch(objID)
	{
		case NX_SHAHID_PLAYER_BTN_SR_ID:
			nx_shahid_player_LeftSeek(pstContext);
			break;
		case NX_SHAHID_PLAYER_BTN_FR_ID:
			nx_shahid_player_FastRewind(pstContext);
			break;
		case NX_SHAHID_PLAYER_BTN_PLAY_PAUSE_ID:
			nx_shahid_player_PlayPause(pstContext);
			break;
		case NX_SHAHID_PLAYER_BTN_FF_ID:
			nx_shahid_player_FastForward(pstContext);
			break;
		case NX_SHAHID_PLAYER_BTN_SF_ID:
			nx_shahid_player_RightSeek(pstContext);
			break;
		default:
			break;

	}
	return ONDK_RET_OK;
}

static void	nx_shahid_player_HideUI(HBOOL bHide)
{
	Nx_ShahidPlayerContext_t *pstContext;
	pstContext = nx_shahid_Player_GetContext();
	if(pstContext->bHide != bHide)
	{
		ONDK_GWM_Obj_SetHide(NX_SHAHID_PLAYER_DIM_IMG_ID,bHide);
		ONDK_GWM_Obj_SetHide(NX_SHAHID_PLAYER_TITLE_ID,bHide);
		ONDK_GWM_Obj_SetHide(NX_SHAHID_PLAYER_PROGRESSBAR_BG_ID,bHide);
		ONDK_GWM_Obj_SetHide(NX_SHAHID_PLAYER_PROGRESSBAR_BAR_ID,bHide);
		ONDK_GWM_Obj_SetHide(NX_SHAHID_PLAYER_PROGRESSBAR_PTR_ID,bHide);
		ONDK_GWM_Obj_SetHide(NX_SHAHID_PLAYER_CURRENT_TIME_ID,bHide);
		ONDK_GWM_Obj_SetHide(NX_SHAHID_PLAYER_TOTAL_TIME_ID,bHide);
		ONDK_GWM_Obj_SetHide(NX_SHAHID_PLAYER_BTN_SR_ID,bHide);
		ONDK_GWM_Obj_SetHide(NX_SHAHID_PLAYER_BTN_FR_ID,bHide);
		ONDK_GWM_Obj_SetHide(NX_SHAHID_PLAYER_BTN_PLAY_PAUSE_ID,bHide);
		ONDK_GWM_Obj_SetHide(NX_SHAHID_PLAYER_BTN_FF_ID,bHide);
		ONDK_GWM_Obj_SetHide(NX_SHAHID_PLAYER_BTN_SF_ID,bHide);
		ONDK_GWM_Obj_SetHide(NX_SHAHID_PLAYER_IMG_SPEED_ID,bHide);
		ONDK_GWM_Obj_SetHide(NX_SHAHID_PLAYER_IMG_SPEED_BG_ID, bHide);

		pstContext->bHide = bHide;

		if(pstContext->bHide == FALSE)
		{
			ONDK_GWM_SetTimer(NX_SHAHID_PLAYER_HIDE_TIMER_ID, NX_SHAHID_PLAYER_HIDE_TIMER_TIMEOUT);
		}
		else
		{
			ONDK_GWM_KillTimer(NX_SHAHID_PLAYER_HIDE_TIMER_ID);
		}

	}

}

static ONDK_Result_t	nx_shahid_player_GetTimeStr(HCHAR * pszTime,HINT32 ulTime)
{
	HINT32	ulTmpTime = 0;
	HINT32 lHour = 0, lMin = 0, lSec = 0;

	ulTmpTime = (HINT32)ulTime/1000;

	lHour = ulTmpTime / (60 * 60);
	ulTmpTime = ulTmpTime % (60*60);
	lMin = ulTmpTime / 60;
	ulTmpTime = ulTmpTime % 60;
	lSec = ulTmpTime;

	HxSTD_sprintf((HCHAR*)pszTime, "%02d:%02d:%02d",lHour, lMin, lSec);

	return ONDK_RET_OK;
}

static ONDK_Result_t	nx_shahid_player_DrawUiObject(Nx_ShahidPlayerContext_t *pstContext)
{
	ONDK_Rect_t stRect;

	if(pstContext == NULL)
	{
		ONDK_GWM_APP_Destroy(0);
		return ONDK_RET_INITFAIL;
	}

	//dim image
	stRect = ONDK_Rect(NX_SHAHID_PLAYER_DIM_IMG_X, NX_SHAHID_PLAYER_DIM_IMG_Y, NX_SHAHID_PLAYER_DIM_IMG_W, NX_SHAHID_PLAYER_DIM_IMG_H);
	ONDK_GWM_COM_Image_Create(NX_SHAHID_PLAYER_DIM_IMG_ID, &stRect);
	ONDK_GWM_COM_Image_SetImage(NX_SHAHID_PLAYER_DIM_IMG_ID, RES_105_00_BG_BOTTOM_400_PNG);

	//draw title
	stRect = ONDK_Rect(NX_SHAHID_PLAYER_TITLE_X, NX_SHAHID_PLAYER_TITLE_Y, NX_SHAHID_PLAYER_TITLE_W, NX_SHAHID_PLAYER_TITLE_H);
	ONDK_GWM_COM_Text_Create(NX_SHAHID_PLAYER_TITLE_ID, &stRect, pstContext->szTitle);
	ONDK_GWM_COM_Text_SetFont(NX_SHAHID_PLAYER_TITLE_ID, eFont_SystemBold);
	ONDK_GWM_COM_Text_SetSize(NX_SHAHID_PLAYER_TITLE_ID, NX_PRISM_FONT_SIZE_26);
	ONDK_GWM_COM_Text_SetAlign(NX_SHAHID_PLAYER_TITLE_ID, TEXT_ALIGN_CENTER);
	ONDK_GWM_COM_Text_ChangeColor(NX_SHAHID_PLAYER_TITLE_ID, COL(C_T_Whistle_100), 0);

	//Draw progress bar & background
	stRect = ONDK_Rect(NX_SHAHID_PLAYER_PROGRESSBAR_BG_X, NX_SHAHID_PLAYER_PROGRESSBAR_BG_Y, NX_SHAHID_PLAYER_PROGRESSBAR_BG_W, NX_SHAHID_PLAYER_PROGRESSBAR_BG_H);
	ONDK_GWM_COM_Rect_Create(NX_SHAHID_PLAYER_PROGRESSBAR_BG_ID, &stRect, COL(C_G_Shahid_02));

	if(pstContext->ulProgressValue == 0)
	{
		pstContext->ulProgressValue ++;
	}
	stRect = ONDK_Rect(NX_SHAHID_PLAYER_PROGRESSBAR_BAR_X, NX_SHAHID_PLAYER_PROGRESSBAR_BAR_Y, pstContext->ulProgressValue, NX_SHAHID_PLAYER_PROGRESSBAR_BAR_H);
	ONDK_GWM_COM_Rect_Create(NX_SHAHID_PLAYER_PROGRESSBAR_BAR_ID, &stRect,COL(C_G_Shahid_03));
	stRect = ONDK_Rect(NX_SHAHID_PLAYER_PROGRESSBAR_PTR_X_MIN+pstContext->ulProgressValue, NX_SHAHID_PLAYER_PROGRESSBAR_PTR_Y,
		NX_SHAHID_PLAYER_PROGRESSBAR_PTR_W, NX_SHAHID_PLAYER_PROGRESSBAR_PTR_H);
	ONDK_GWM_COM_Image_Create(NX_SHAHID_PLAYER_PROGRESSBAR_PTR_ID, &stRect);
	ONDK_GWM_COM_Image_SetImage(NX_SHAHID_PLAYER_PROGRESSBAR_PTR_ID, RES_602_11_SHAHID_PB_POINTER_PNG);

	//draw current&total time
	stRect = ONDK_Rect(NX_SHAHID_PLAYER_CURRENT_TIME_X, NX_SHAHID_PLAYER_CURRENT_TIME_Y, NX_SHAHID_PLAYER_CURRENT_TIME_W, NX_SHAHID_PLAYER_CURRENT_TIME_H);
	ONDK_GWM_COM_Text_Create(NX_SHAHID_PLAYER_CURRENT_TIME_ID, &stRect, pstContext->szCurrentPlayTime);
	ONDK_GWM_COM_Text_SetFont(NX_SHAHID_PLAYER_CURRENT_TIME_ID, eFont_SystemBold);
	ONDK_GWM_COM_Text_SetSize(NX_SHAHID_PLAYER_CURRENT_TIME_ID, NX_PRISM_FONT_SIZE_24);
	ONDK_GWM_COM_Text_SetAlign(NX_SHAHID_PLAYER_CURRENT_TIME_ID, TEXT_ALIGN_LEFT);
	ONDK_GWM_COM_Text_ChangeColor(NX_SHAHID_PLAYER_CURRENT_TIME_ID, COL(C_G_Shahid_03), 0);
	stRect = ONDK_Rect(NX_SHAHID_PLAYER_TOTAL_TIME_X, NX_SHAHID_PLAYER_TOTAL_TIME_Y, NX_SHAHID_PLAYER_TOTAL_TIME_W, NX_SHAHID_PLAYER_TOTAL_TIME_H);
	ONDK_GWM_COM_Text_Create(NX_SHAHID_PLAYER_TOTAL_TIME_ID, &stRect, pstContext->szDuration);
	ONDK_GWM_COM_Text_SetFont(NX_SHAHID_PLAYER_TOTAL_TIME_ID, eFont_SystemNormal);
	ONDK_GWM_COM_Text_SetSize(NX_SHAHID_PLAYER_TOTAL_TIME_ID, NX_PRISM_FONT_SIZE_24);
	ONDK_GWM_COM_Text_SetAlign(NX_SHAHID_PLAYER_TOTAL_TIME_ID, TEXT_ALIGN_LEFT);
	ONDK_GWM_COM_Text_ChangeColor(NX_SHAHID_PLAYER_TOTAL_TIME_ID, COL(C_G_Shahid_02), 0);

	//draw buttons
	stRect = ONDK_Rect(NX_SHAHID_PLAYER_BTN_SR_X, NX_SHAHID_PLAYER_BTN_Y, NX_SHAHID_PLAYER_BTN_W, NX_SHAHID_PLAYER_BTN_H);
	ONDK_GWM_COM_Button_Create(NX_SHAHID_PLAYER_BTN_SR_ID, &stRect, NULL);
	ONDK_GWM_COM_Button_ChangeImage(NX_SHAHID_PLAYER_BTN_SR_ID, RES_602_11_SHAHID_PB_BTN_SR_N_PNG, RES_602_11_SHAHID_PB_BTN_SR_C_PNG, NULL, NULL);
	ONDK_GWM_Obj_SetObjectDrawMethod(NX_SHAHID_PLAYER_BTN_SR_ID,nx_shahid_player_BtnDrawFunc);
	stRect = ONDK_Rect(NX_SHAHID_PLAYER_BTN_FR_X, NX_SHAHID_PLAYER_BTN_Y, NX_SHAHID_PLAYER_BTN_W, NX_SHAHID_PLAYER_BTN_H);
	ONDK_GWM_COM_Button_Create(NX_SHAHID_PLAYER_BTN_FR_ID, &stRect, NULL);
	ONDK_GWM_COM_Button_ChangeImage(NX_SHAHID_PLAYER_BTN_FR_ID, RES_602_11_SHAHID_PB_BTN_FR_N_PNG, RES_602_11_SHAHID_PB_BTN_FR_C_PNG, NULL, NULL);
	ONDK_GWM_Obj_SetObjectDrawMethod(NX_SHAHID_PLAYER_BTN_FR_ID,nx_shahid_player_BtnDrawFunc);
	stRect = ONDK_Rect(NX_SHAHID_PLAYER_BTN_PLAY_PAUSE_X, NX_SHAHID_PLAYER_BTN_Y, NX_SHAHID_PLAYER_BTN_W, NX_SHAHID_PLAYER_BTN_H);
	ONDK_GWM_COM_Button_Create(NX_SHAHID_PLAYER_BTN_PLAY_PAUSE_ID, &stRect, NULL);
	ONDK_GWM_COM_Button_ChangeImage(NX_SHAHID_PLAYER_BTN_PLAY_PAUSE_ID, RES_602_11_SHAHID_PB_BTN_PAUSE_N_PNG, RES_602_11_SHAHID_PB_BTN_PAUSE_C_PNG, NULL, NULL);
	ONDK_GWM_Obj_SetObjectDrawMethod(NX_SHAHID_PLAYER_BTN_PLAY_PAUSE_ID,nx_shahid_player_BtnDrawFunc);
	stRect = ONDK_Rect(NX_SHAHID_PLAYER_BTN_FF_X, NX_SHAHID_PLAYER_BTN_Y, NX_SHAHID_PLAYER_BTN_W, NX_SHAHID_PLAYER_BTN_H);
	ONDK_GWM_COM_Button_Create(NX_SHAHID_PLAYER_BTN_FF_ID, &stRect, NULL);
	ONDK_GWM_COM_Button_ChangeImage(NX_SHAHID_PLAYER_BTN_FF_ID, RES_602_11_SHAHID_PB_BTN_FF_N_PNG, RES_602_11_SHAHID_PB_BTN_FF_C_PNG, NULL, NULL);
	ONDK_GWM_Obj_SetObjectDrawMethod(NX_SHAHID_PLAYER_BTN_FF_ID,nx_shahid_player_BtnDrawFunc);
	stRect = ONDK_Rect(NX_SHAHID_PLAYER_BTN_SF_X, NX_SHAHID_PLAYER_BTN_Y, NX_SHAHID_PLAYER_BTN_W, NX_SHAHID_PLAYER_BTN_H);
	ONDK_GWM_COM_Button_Create(NX_SHAHID_PLAYER_BTN_SF_ID, &stRect, NULL);
	ONDK_GWM_COM_Button_ChangeImage(NX_SHAHID_PLAYER_BTN_SF_ID, RES_602_11_SHAHID_PB_BTN_SF_N_PNG, RES_602_11_SHAHID_PB_BTN_SF_C_PNG, NULL, NULL);
	ONDK_GWM_Obj_SetObjectDrawMethod(NX_SHAHID_PLAYER_BTN_SF_ID,nx_shahid_player_BtnDrawFunc);

	//draw speed image & background
	stRect = ONDK_Rect(NX_SHAHID_PLAYER_IMG_SPEED_BG_X, NX_SHAHID_PLAYER_IMG_SPEED_BG_Y, NX_SHAHID_PLAYER_IMG_SPEED_BG_W, NX_SHAHID_PLAYER_IMG_SPEED_BG_H);
	ONDK_GWM_COM_Rect_Create(NX_SHAHID_PLAYER_IMG_SPEED_BG_ID, &stRect,(HUINT32)NULL);
	stRect = ONDK_Rect(NX_SHAHID_PLAYER_IMG_SPEED_X, NX_SHAHID_PLAYER_IMG_SPEED_Y, NX_SHAHID_PLAYER_IMG_SPEED_W, NX_SHAHID_PLAYER_IMG_SPEED_H);
	ONDK_GWM_COM_Image_Create(NX_SHAHID_PLAYER_IMG_SPEED_ID,&stRect);

	ONDK_GWM_Obj_SetFocus(NX_SHAHID_PLAYER_BTN_PLAY_PAUSE_ID);

	return ONDK_RET_OK;
}

static ONDK_Result_t	nx_shahid_player_MsgGwmCreate(Nx_ShahidPlayerContext_t * pstContext,HINT32 p1,HINT32 p2,HINT32 p3)
{
	NxPlayerContentInform_t	*contentInform = (NxPlayerContentInform_t*)p1;

	if(pstContext == NULL)
	{
		ONDK_GWM_APP_Destroy(0);
		return ONDK_RET_INITFAIL;
	}


	ONDK_GWM_SetTimer(NX_SHAHID_PLAYER_HIDE_TIMER_ID,(HUINT32)NX_SHAHID_PLAYER_HIDE_TIMER_TIMEOUT);

	pstContext->ulCurFocusIndex = NX_SHAHID_PLAYER_BTN_PLAY_PAUSE_ID;
	pstContext->lSpeed = 1;
	pstContext->ulDuration = 0;
	pstContext->ulCurrentPlayTime = 0;
	pstContext->isPlaying = TRUE;
	pstContext->bHide = FALSE;

	nx_shahid_player_GetTimeStr((HCHAR*)pstContext->szCurrentPlayTime, pstContext->ulCurrentPlayTime);
	nx_shahid_player_GetTimeStr((HCHAR*)pstContext->szDuration, pstContext->ulDuration);

	nx_shahid_player_SetProgrammeTitle(pstContext, contentInform);
	nx_shahid_player_UpdateProgressData(pstContext, contentInform);

	nx_shahid_player_DrawUiObject(pstContext);


	return ONDK_RET_OK;
}

static ONDK_Result_t	nx_shahid_player_MsgGwmDestroy(Nx_ShahidPlayerContext_t * pstContext,HINT32 p1,HINT32 p2,HINT32 p3)
{
	ONDK_GWM_KillTimer(NX_SHAHID_PLAYER_HIDE_TIMER_ID);

	ONDK_GWM_PostMessage(NULL, MSG_APP_PLAYER_DO_STOP, 0, 0, 0, 0);

	ONDK_GWM_PostMessage(NULL, MSG_APP_PLAYER_DESTROY, 0, 0, 0, 0);

	HxSTD_memset(pstContext, 0x00, sizeof(Nx_ShahidPlayerContext_t));

	NX_COMMON_NOT_USED_PARAM(pstContext);
	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_shahid_player_MsgGwmKeyDown(Nx_ShahidPlayerContext_t * pstContext,HINT32 p1,HINT32 p2,HINT32 p3)
{

	if (pstContext == NULL)
	{
		NX_APP_Error("Error!!! Please check!! \n");
		return	GWM_RET_SOURCE_NULL;
	}

	if(pstContext->bHide == TRUE)
	{
		nx_shahid_player_HideUI(FALSE);
		return ONDK_MESSAGE_BREAK;
	}
	else
	{
		ONDK_GWM_ResetTimer(NX_SHAHID_PLAYER_HIDE_TIMER_ID);
	}

	NX_PRISM_UTIL_KeyConv_UiCommon(&p1);
	NX_COMMON_NOT_USED_ARGUMENT(pstContext, p1, p2, p3);

	switch (p1)
	{
		/* keys to exit */
		case KEY_BACK:
			ONDK_GWM_APP_Destroy(0);
			return	ONDK_MESSAGE_BREAK;

		case KEY_EXIT:
		case KEY_STANDBY:
		case KEY_GUIDE:
			ONDK_GWM_APP_Destroy(0);
			return	ONDK_MESSAGE_PASS;

		case KEY_TVRADIO:
		case KEY_VFORMAT:
		case KEY_AUDIO:
		case KEY_SUBTITLE:
		case KEY_TEXT:
			NX_PRISM_NotAvailable_Show();
			return	ONDK_MESSAGE_BREAK;

		case KEY_OK:
			nx_shahid_player_ProcessKeyOK(pstContext);
			break;
		case KEY_ARROWLEFT:
		case KEY_ARROWRIGHT:
			break;
		case KEY_ARROWUP:
		case KEY_ARROWDOWN:
			break;

		default:
			NX_APP_Print("nx_shahid_detail_MsgGwmKeyDown() Not used lKeyId(0x%x)\n", p1);
			return	ONDK_MESSAGE_PASS;
	}

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_shahid_player_MsgGwmTimer(Nx_ShahidPlayerContext_t *pstContext, HINT32 p1, HINT32 p2, HINT32 p3)
{
	switch(p1)
	{
		case NX_SHAHID_PLAYER_HIDE_TIMER_ID:
			nx_shahid_player_HideUI(TRUE);
			break;
		case NX_SHAHID_PLAYER_TEMP_TIMER_ID:
			break;
		default:
			break;
	}

	NX_COMMON_NOT_USED_PARAM(pstContext);
	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_shahid_player_MsgPlayStateChanged(Nx_ShahidPlayerContext_t *pstContext, HINT32 p1, HINT32 p2, HINT32 p3)
{
	switch(p1)
	{
		case eOxMP_PLAYSTATE_EOS:
			NX_PLAYER_Stop(NX_SHAHID_PLAYER_CONTENT_ID);
			ONDK_GWM_APP_Destroy(0);
			break;

		default:
			break;

	}
	return ONDK_RET_OK;
}

static ONDK_Result_t	nx_shahid_player_MsgAppPlayMgrStart(Nx_ShahidPlayerContext_t * pstContext,HINT32 p1,HINT32 p2,HINT32 p3)
{
	//NxPlayerContentInform_t	*contentInform = (NxPlayerContentInform_t*)p1;

	// TODO: Change title

	// set duration
	pstContext->ulDuration = 0;
	pstContext->ulCurrentPlayTime = 0;
	nx_shahid_player_GetTimeStr((HCHAR*)pstContext->szCurrentPlayTime, pstContext->ulCurrentPlayTime);
	nx_shahid_player_GetTimeStr((HCHAR*)pstContext->szDuration, pstContext->ulDuration);

	pstContext->ulProgressValue = 0;
	pstContext->lSpeed = 1;

	return ONDK_RET_OK;
}

static ONDK_Result_t	nx_shahid_player_MsgAppPlayMgrStop(Nx_ShahidPlayerContext_t * pstContext,HINT32 p1,HINT32 p2,HINT32 p3)
{
	ONDK_GWM_APP_Destroy(0);

	return ONDK_MESSAGE_BREAK;
}

static ONDK_Result_t	nx_shahid_player_MsgAppPlayMgrPlayInfoUpdate(Nx_ShahidPlayerContext_t * pstContext,HINT32 p1,HINT32 p2,HINT32 p3)
{
	NxPlayerContentInform_t	*contentInform = (NxPlayerContentInform_t*)p1;

	nx_shahid_player_UpdateProgressData(pstContext, contentInform);
	nx_shahid_player_UpdateProgressUI(pstContext);
	return ONDK_RET_OK;
}



/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/
ONDK_Result_t	NX_PRISM_SHAHID_Player_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t		eResult = ONDK_RET_OK;
	Nx_ShahidPlayerContext_t	*pstContext = NULL;

	pstContext = nx_shahid_Player_GetContext();

	switch (lMessage)
	{
		case MSG_GWM_CREATE:
			NX_APP_Message("MSG_GWM_CREATE!\n");
			eResult = nx_shahid_player_MsgGwmCreate(pstContext, p1, p2, p3);
			break;

		case MSG_GWM_KEYUP:
			break;

		case MSG_GWM_KEYDOWN:
			eResult = nx_shahid_player_MsgGwmKeyDown(pstContext, p1, p2, p3);
			break;

		case MSG_GWM_DESTROY:
			NX_APP_Message("MSG_GWM_DESTROY!\n");
			eResult = nx_shahid_player_MsgGwmDestroy(pstContext, p1, p2, p3);
			break;

		case MSG_GWM_HAPI_MESSAGE:
			break;

		case MSG_GWM_CLICKED:
			break;
		case eOxMP_PLAYSTATE_EOS:
			nx_shahid_player_MsgPlayStateChanged(pstContext, p1, p2, p3);
			break;

		case MSG_GWM_TIMER:
			eResult = nx_shahid_player_MsgGwmTimer(pstContext, p1, p2, p3);
			break;

		case MSG_APP_PLAYMGR_PLAY_START:
			eResult = nx_shahid_player_MsgAppPlayMgrStart(pstContext, p1, p2, p3);
			break;
		case MSG_APP_PLAYMGR_PLAY_STOP:
			eResult = nx_shahid_player_MsgAppPlayMgrStop(pstContext, p1, p2, p3);
			break;
		case MSG_APP_PLAYMGR_PLAYINFO_UPDATED:
			eResult = nx_shahid_player_MsgAppPlayMgrPlayInfoUpdate(pstContext, p1, p2, p3);
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

#endif


