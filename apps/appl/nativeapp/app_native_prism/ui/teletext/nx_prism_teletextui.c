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
 * @file	  		nx_prism_teletextui.c
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

#include <nx_prism_teletext.h>
#include <nx_main.h>

/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/

#define	NX_PRISM_TTX_MENU_UI_TIMEOUT			(3*1000)	// 3 seconds

#define	NX_PRISM_TTX_MENU_GRAP 					8
#define	NX_PRISM_TTX_MENU_WIDTH					75
#define	NX_PRISM_TTX_MENU_HEIGHT				46

#define	NX_PRISM_TTX_MENU_Y						NX_PRISM_TTX_DIALOG_Y + 18

 /* Main Frame. */
#define	NX_PRISM_TTX_DIALOG_X					350
#define	NX_PRISM_TTX_DIALOG_Y					596
#define	NX_PRISM_TTX_DIALOG_W					650
#define	NX_PRISM_TTX_DIALOG_H					78

 /* Frame. */
#define	NX_PRISM_TTX_DIALOG_FRAME_X				0
#define	NX_PRISM_TTX_DIALOG_FRAME_Y				0
#define	NX_PRISM_TTX_DIALOG_FRAME_W				NX_PRISM_TTX_DIALOG_W
#define	NX_PRISM_TTX_DIALOG_FRAME_H				NX_PRISM_TTX_DIALOG_H

 /* TTX Menu UI Index. */
#define	NX_PRISM_TTX_MENU_INDEX_X				NX_PRISM_TTX_DIALOG_X + 38

 /* TTX Menu UI Sub-Page. */
#define	NX_PRISM_TTX_MENU_SUB_PAGE_X			(NX_PRISM_TTX_MENU_INDEX_X + NX_PRISM_TTX_MENU_WIDTH + NX_PRISM_TTX_MENU_GRAP)

 /* TTX Menu UI Mix. */
#define	NX_PRISM_TTX_MENU_MIX_X					(NX_PRISM_TTX_MENU_SUB_PAGE_X + NX_PRISM_TTX_MENU_WIDTH + NX_PRISM_TTX_MENU_GRAP)

 /* TTX Menu UI Cancel. */
#define	NX_PRISM_TTX_MENU_CANCEL_X				(NX_PRISM_TTX_MENU_MIX_X + NX_PRISM_TTX_MENU_WIDTH + NX_PRISM_TTX_MENU_GRAP)

 /* TTX Menu UI Reveal. */
#define	NX_PRISM_TTX_MENU_REVEAL_X				(NX_PRISM_TTX_MENU_CANCEL_X + NX_PRISM_TTX_MENU_WIDTH + NX_PRISM_TTX_MENU_GRAP)

 /* TTX Menu UI Double. */
#define	NX_PRISM_TTX_MENU_DOUBLE_X				(NX_PRISM_TTX_MENU_REVEAL_X + NX_PRISM_TTX_MENU_WIDTH + NX_PRISM_TTX_MENU_GRAP)

 /* TTX Menu UI Hold. */
#define	NX_PRISM_TTX_MENU_HOLD_X				(NX_PRISM_TTX_MENU_DOUBLE_X + NX_PRISM_TTX_MENU_WIDTH + NX_PRISM_TTX_MENU_GRAP)

#define NX_PRISM_TTXMenuObject_Macro(type,val)	s_stTTxMenu[index].eID =type; s_stTTxMenu[index++].hHandle = nx_ttxui_createImage(val);

/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/

typedef enum
{
	TTX_M_UI_MAIN_FRAME_ID,
	TTX_M_UI_FRAME_ID,

	TTX_M_UI_BTN_INDEX_ID,
	TTX_M_UI_BTN_SUB_PAGE_ID,
	TTX_M_UI_BTN_MIX_ID,
	TTX_M_UI_BTN_CANCEL_ID,
	TTX_M_UI_BTN_REVEAL_ID,
	TTX_M_UI_BTN_DOUBLE_ID,
	TTX_M_UI_BTN_HOLD_ID,

	TTX_M_UI_IMG_INDEX_ID,
	TTX_M_UI_IMG_SUB_PAGE_ID,
	TTX_M_UI_IMG_MIX_ID,
	TTX_M_UI_IMG_CANCEL_ID,
	TTX_M_UI_IMG_REVEAL_ID,
	TTX_M_UI_IMG_DOUBLE_ID,
	TTX_M_UI_IMG_HOLD_ID,

	TTX_M_UI_ID_MAX,
} eTtxMenuUiCompIds;

typedef enum
{
    Teletext_Index_C_Off = 0x00010000,
	Teletext_Index_C_On,
	Teletext_Index_N_Off,
	Teletext_Subpage_C_Off ,
    Teletext_Subpage_C_On,
    Teletext_Subpage_N_Off,
	Teletext_Mix_C_50,
	Teletext_Mix_C_Off,
	Teletext_Mix_C_On ,
	Teletext_Mix_N_50 ,
	Teletext_Mix_N_Off,
	Teletext_Mix_N_On ,
	Teletext_Cancel_C_Off,
	Teletext_Cancel_C_On,
	Teletext_Cancel_N_Off,
	Teletext_Cancel_N_On,
	Teletext_Reveal_C_Off,
	Teletext_Reveal_C_On,
	Teletext_Reveal_N_Off,
	Teletext_Reveal_N_On,
	Teletext_DoubleSize_C_Off,
	Teletext_DoubleSize_C_On ,
	Teletext_DoubleSize_N_Off,
	Teletext_DoubleSize_N_On,
	Teletext_PageHold_C_Off,
	Teletext_PageHold_C_On,
	Teletext_PageHold_N_Off,
	Teletext_PageHold_N_On,
} NxUiTtxMenuIMG_Id_e;

typedef struct{
	NxUiTtxMenuIMG_Id_e 	eID;
	ONDKImageHandle 	hHandle;
} NxUiTtxMenuObject_t;


/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/

#ifdef CONFIG_DEBUG
HUINT32		g_ApTtxMenuUi_Level = 0x00;
#endif

const static ONDK_Rect_t		s_stTtxMenuUiRectComponent[TTX_M_UI_ID_MAX] =
{
		{	NX_PRISM_TTX_DIALOG_FRAME_X,		NX_PRISM_TTX_DIALOG_FRAME_Y,		NX_PRISM_TTX_DIALOG_FRAME_W,		NX_PRISM_TTX_DIALOG_FRAME_H			},
		{	NX_PRISM_TTX_DIALOG_X,				NX_PRISM_TTX_DIALOG_Y,				NX_PRISM_TTX_DIALOG_W,				NX_PRISM_TTX_DIALOG_H				},
		{	NX_PRISM_TTX_MENU_INDEX_X,			NX_PRISM_TTX_MENU_Y,				NX_PRISM_TTX_MENU_WIDTH,			NX_PRISM_TTX_MENU_HEIGHT			},
		{	NX_PRISM_TTX_MENU_SUB_PAGE_X,		NX_PRISM_TTX_MENU_Y,				NX_PRISM_TTX_MENU_WIDTH,			NX_PRISM_TTX_MENU_HEIGHT			},
		{	NX_PRISM_TTX_MENU_MIX_X,			NX_PRISM_TTX_MENU_Y,				NX_PRISM_TTX_MENU_WIDTH,			NX_PRISM_TTX_MENU_HEIGHT			},
		{	NX_PRISM_TTX_MENU_CANCEL_X,			NX_PRISM_TTX_MENU_Y,				NX_PRISM_TTX_MENU_WIDTH,			NX_PRISM_TTX_MENU_HEIGHT			},
		{	NX_PRISM_TTX_MENU_REVEAL_X,			NX_PRISM_TTX_MENU_Y,				NX_PRISM_TTX_MENU_WIDTH,			NX_PRISM_TTX_MENU_HEIGHT			},
		{	NX_PRISM_TTX_MENU_DOUBLE_X,			NX_PRISM_TTX_MENU_Y,				NX_PRISM_TTX_MENU_WIDTH,			NX_PRISM_TTX_MENU_HEIGHT			},
		{	NX_PRISM_TTX_MENU_HOLD_X,			NX_PRISM_TTX_MENU_Y,				NX_PRISM_TTX_MENU_WIDTH,			NX_PRISM_TTX_MENU_HEIGHT			},
		{	0,									0,									0,									0									},
};


static NxUiTtxMenuObject_t  		s_stTTxMenu[28];

/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/

static ONDK_Result_t	nx_ttxui_DrawTtxMenuUiButton(ONDKSurfaceHandle screen, ONDK_Region_t *area,
								ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop);
static ONDK_Result_t	nx_ttxui_Drawframe(ONDKSurfaceHandle screen, ONDK_Region_t *area,
								ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop);

static ONDKImageHandle *nx_ttxui_getIMGHandle(NxUiTtxMenuIMG_Id_e eID);
static ONDKImageHandle	nx_ttxui_createImage(HCHAR *filename);

static void				nx_ttxui_GetTtxMenuUiPosition(eTtxMenuUiCompIds eUiId, ONDK_Rect_t *rect);
static void				nx_ttxui_InitTtxMenuUiObj(void);
static void				nx_ttxui_KeyConv_UiCommon(HINT32 *nKeyCode);

static ONDK_Result_t	nx_ttxui_MsgGwmCreate(void);
static ONDK_Result_t	nx_ttxui_MsgGwmDlgDestroy(void);
static ONDK_Result_t	nx_ttxui_MsgGwmClicked(HINT32 p1);
static ONDK_Result_t	nx_ttxui_MsgGwmKeyDown(HINT32 p1);
static ONDK_Result_t	nx_ttxui_MsgGwmKeyUp(HINT32 p1);
static ONDK_Result_t	nx_ttxui_MsgGwmProcessTimer(HINT32 p1);
static ONDK_Result_t	nx_ttxui_MsgGwmWrapAround(HINT32 p1);


/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/

static ONDK_Result_t	nx_ttxui_DrawTtxMenuUiButton (ONDKSurfaceHandle screen, ONDK_Region_t *area,
														ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	HERROR 				hErr = ERR_OK;
	HINT32 				push_status, objectId;
	HBOOL				bSet = FALSE;
	TTX_Mix_t			mix;
	TTX_DoubleSize_t	doubleSize;

	NOT_USED_PARAM (arg);
	NOT_USED_PARAM (bUiTop);

	push_status = ONDK_GWM_COM_Button_GetStatusByObj ((BUTTON_OBJECT)object);
	objectId = ONDK_GWM_Obj_GetObjectID (object);

    HxLOG_Print("objectId = %d , push_status = 0x%x\n", objectId,push_status);

	switch (objectId)
	{
		case TTX_M_UI_BTN_INDEX_ID :
			if ( push_status & GUIOBJ_PUSHED )
				ONDK_GWM_COM_Image_Change(TTX_M_UI_IMG_INDEX_ID,nx_ttxui_getIMGHandle(Teletext_Index_C_On) );
			else
			{
				if(push_status & GUIOBJ_HILIGHTED)
					ONDK_GWM_COM_Image_Change(TTX_M_UI_IMG_INDEX_ID,nx_ttxui_getIMGHandle(Teletext_Index_C_Off) );
				else
					ONDK_GWM_COM_Image_Change(TTX_M_UI_IMG_INDEX_ID,nx_ttxui_getIMGHandle(Teletext_Index_N_Off) );
			}
			break;
		case TTX_M_UI_BTN_SUB_PAGE_ID :
			if ( push_status & GUIOBJ_PUSHED )
				ONDK_GWM_COM_Image_Change(TTX_M_UI_IMG_SUB_PAGE_ID,nx_ttxui_getIMGHandle(Teletext_Subpage_C_On) );
			else
			{
				if(push_status & GUIOBJ_HILIGHTED)
					ONDK_GWM_COM_Image_Change(TTX_M_UI_IMG_SUB_PAGE_ID,nx_ttxui_getIMGHandle(Teletext_Subpage_C_Off) );
				else
					ONDK_GWM_COM_Image_Change(TTX_M_UI_IMG_SUB_PAGE_ID,nx_ttxui_getIMGHandle(Teletext_Subpage_N_Off) );
			}
			break;

		case TTX_M_UI_BTN_MIX_ID :
			hErr = NX_PRISM_Teletext_GetControl( eTTX_CONTROL_MIX, &mix);
			if(hErr!=ERR_OK)
		        HxLOG_Assert("NX_PRISM_Teletext_GetControl( eTTX_CONTROL_MIX); \n");

			switch (mix)
			{
				case eTTX_MIX_100_PERCENT :	/* On */
					{
						if(push_status & GUIOBJ_HILIGHTED)
							ONDK_GWM_COM_Image_Change(TTX_M_UI_IMG_MIX_ID,nx_ttxui_getIMGHandle(Teletext_Mix_C_On) );

						else
							ONDK_GWM_COM_Image_Change(TTX_M_UI_IMG_MIX_ID,nx_ttxui_getIMGHandle(Teletext_Mix_N_On) );
					}
					break;

				case eTTX_MIX_50_PERCENT :
					{
						if(push_status & GUIOBJ_HILIGHTED)
							ONDK_GWM_COM_Image_Change(TTX_M_UI_IMG_MIX_ID,nx_ttxui_getIMGHandle(Teletext_Mix_C_50) );
						else
							ONDK_GWM_COM_Image_Change(TTX_M_UI_IMG_MIX_ID,nx_ttxui_getIMGHandle(Teletext_Mix_N_50) );
					}
					break;

				case eTTX_MIX_0_PERCENT :	/* Off */
				default :
					{
						if(push_status & GUIOBJ_HILIGHTED)
							ONDK_GWM_COM_Image_Change(TTX_M_UI_IMG_MIX_ID,nx_ttxui_getIMGHandle(Teletext_Mix_C_Off) );
						else
							ONDK_GWM_COM_Image_Change(TTX_M_UI_IMG_MIX_ID,nx_ttxui_getIMGHandle((Teletext_Mix_N_Off)) );
					}
					break;
			}
			break;

		case TTX_M_UI_BTN_CANCEL_ID :
			hErr = NX_PRISM_Teletext_GetControl( eTTX_CONTROL_CANCEL, &bSet);
			if(hErr!=ERR_OK)
		        HxLOG_Assert("NX_PRISM_Teletext_GetControl( eTTX_CONTROL_CANCEL); \n");

			if ( bSet == TRUE )
			{
				if(push_status & GUIOBJ_HILIGHTED)
					ONDK_GWM_COM_Image_Change(TTX_M_UI_IMG_CANCEL_ID,nx_ttxui_getIMGHandle(Teletext_Cancel_C_On) );
				else
					ONDK_GWM_COM_Image_Change(TTX_M_UI_IMG_CANCEL_ID,nx_ttxui_getIMGHandle(Teletext_Cancel_N_On) );
			}
			else
			{
				if(push_status & GUIOBJ_HILIGHTED)
					ONDK_GWM_COM_Image_Change(TTX_M_UI_IMG_CANCEL_ID,nx_ttxui_getIMGHandle(Teletext_Cancel_C_Off) );
				else
					ONDK_GWM_COM_Image_Change(TTX_M_UI_IMG_CANCEL_ID,nx_ttxui_getIMGHandle(Teletext_Cancel_N_Off) );
			}
			break;

		case TTX_M_UI_BTN_REVEAL_ID :
			hErr = NX_PRISM_Teletext_GetControl( eTTX_CONTROL_REVEAL, &bSet);
			if(hErr!=ERR_OK)
		        HxLOG_Assert("NX_PRISM_Teletext_GetControl( eTTX_CONTROL_REVEAL); \n");

			if ( bSet == TRUE )
			{
				if(push_status & GUIOBJ_HILIGHTED)
					ONDK_GWM_COM_Image_Change(TTX_M_UI_IMG_REVEAL_ID,nx_ttxui_getIMGHandle(Teletext_Reveal_C_On) );
				else
					ONDK_GWM_COM_Image_Change(TTX_M_UI_IMG_REVEAL_ID,nx_ttxui_getIMGHandle(Teletext_Reveal_N_On) );
			}
			else
			{
				if(push_status & GUIOBJ_HILIGHTED)
					ONDK_GWM_COM_Image_Change(TTX_M_UI_IMG_REVEAL_ID,nx_ttxui_getIMGHandle(Teletext_Reveal_C_Off) );
				else
					ONDK_GWM_COM_Image_Change(TTX_M_UI_IMG_REVEAL_ID,nx_ttxui_getIMGHandle(Teletext_Reveal_N_Off) );
			}
			break;

		case TTX_M_UI_BTN_DOUBLE_ID :
			hErr = NX_PRISM_Teletext_GetControl(eTTX_CONTROL_DOUBLE, &doubleSize);
			if(hErr!=ERR_OK)
		        HxLOG_Assert("NX_PRISM_Teletext_GetControl( eTTX_CONTROL_DOUBLE); \n");

			if ( doubleSize == eTTX_DOUBLE_OFF )
			{
				if(push_status & GUIOBJ_HILIGHTED)
					ONDK_GWM_COM_Image_Change(TTX_M_UI_IMG_DOUBLE_ID,nx_ttxui_getIMGHandle(Teletext_DoubleSize_C_Off) );
				else
					ONDK_GWM_COM_Image_Change(TTX_M_UI_IMG_DOUBLE_ID,nx_ttxui_getIMGHandle(Teletext_DoubleSize_N_Off) );
			}
			else
			{
				if(push_status & GUIOBJ_HILIGHTED)
					ONDK_GWM_COM_Image_Change(TTX_M_UI_IMG_DOUBLE_ID,nx_ttxui_getIMGHandle(Teletext_DoubleSize_C_On) );
				else
					ONDK_GWM_COM_Image_Change(TTX_M_UI_IMG_DOUBLE_ID,nx_ttxui_getIMGHandle(Teletext_DoubleSize_N_On) );
			}
			break;

		case TTX_M_UI_BTN_HOLD_ID :
			hErr = NX_PRISM_Teletext_GetControl(eTTX_CONTROL_HOLD, &bSet);
			if(hErr!=ERR_OK)
                HxLOG_Assert("NX_PRISM_Teletext_GetControl( eTTX_CONTROL_HOLD); \n");

			if ( bSet == TRUE )
			{
				if(push_status & GUIOBJ_HILIGHTED)
					ONDK_GWM_COM_Image_Change(TTX_M_UI_IMG_HOLD_ID,nx_ttxui_getIMGHandle(Teletext_PageHold_C_On) );
				else
					ONDK_GWM_COM_Image_Change(TTX_M_UI_IMG_HOLD_ID,nx_ttxui_getIMGHandle(Teletext_PageHold_N_On) );
			}
			else
			{
				if(push_status & GUIOBJ_HILIGHTED)
					ONDK_GWM_COM_Image_Change(TTX_M_UI_IMG_HOLD_ID,nx_ttxui_getIMGHandle(Teletext_PageHold_C_Off) );
				else
					ONDK_GWM_COM_Image_Change(TTX_M_UI_IMG_HOLD_ID,nx_ttxui_getIMGHandle(Teletext_PageHold_N_Off) );
			}
			break;
		default :
			break;
	}
	return ONDK_RET_OK;
}

static ONDK_Result_t nx_ttxui_Drawframe (ONDKSurfaceHandle screen, ONDK_Region_t *area,
								ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	ONDK_Rect_t drawRect;

	drawRect = ONDK_Rect(rect->x,rect->y,rect->w,rect->h);
	ONDK_DRAW_FillRect(screen, drawRect ,0xAA000000);
	return ONDK_RET_OK;
}

static ONDKImageHandle	nx_ttxui_createImage(HCHAR *filename)
{
	return ONDK_IMG_CreateSelf((HUINT8*)filename);
}

static ONDKImageHandle	*nx_ttxui_getIMGHandle(NxUiTtxMenuIMG_Id_e eID)
{
	HINT32 i = 0, j = 0;

	for (i = Teletext_Index_C_Off, j = 0; i<= Teletext_PageHold_N_On; i++, j++)
	{
		if(i == eID)
			return &s_stTTxMenu[j].hHandle;
	}
	return NULL;
}

static void		nx_ttxui_InitTtxMenuUiObj(void)
{
	ONDK_Rect_t		 	stRect;

    /* Set Area. */
	nx_ttxui_GetTtxMenuUiPosition(TTX_M_UI_MAIN_FRAME_ID, &stRect);
	ONDK_GWM_APP_SetAppArea (stRect.x, stRect.y, stRect.w, stRect.h);

    /* Draw Frame */
	nx_ttxui_GetTtxMenuUiPosition(TTX_M_UI_FRAME_ID, &stRect);
    ONDK_GWM_COM_Frame_Create(TTX_M_UI_MAIN_FRAME_ID, &stRect, NULL);
	ONDK_GWM_APP_SetObjectDrawMethod(TTX_M_UI_MAIN_FRAME_ID, nx_ttxui_Drawframe);

	/* Button Index */
	nx_ttxui_GetTtxMenuUiPosition(TTX_M_UI_BTN_INDEX_ID, &stRect);
	ONDK_GWM_COM_Button_Create (TTX_M_UI_BTN_INDEX_ID, &stRect, NULL);
    ONDK_GWM_APP_SetObjectDrawMethod (TTX_M_UI_BTN_INDEX_ID, nx_ttxui_DrawTtxMenuUiButton);
	ONDK_GWM_COM_Image_Create(TTX_M_UI_IMG_INDEX_ID,&stRect);
	ONDK_GWM_Obj_SetVisible((TTX_M_UI_IMG_INDEX_ID), TRUE);
	ONDK_GWM_Obj_DisableObject(TTX_M_UI_IMG_INDEX_ID);

	/* Button Sub-Page */
	nx_ttxui_GetTtxMenuUiPosition(TTX_M_UI_BTN_SUB_PAGE_ID, &stRect);
	ONDK_GWM_COM_Button_Create (TTX_M_UI_BTN_SUB_PAGE_ID, &stRect, NULL);
	ONDK_GWM_APP_SetObjectDrawMethod (TTX_M_UI_BTN_SUB_PAGE_ID, nx_ttxui_DrawTtxMenuUiButton);
	ONDK_GWM_COM_Image_Create(TTX_M_UI_IMG_SUB_PAGE_ID,&stRect);
	ONDK_GWM_Obj_SetVisible((TTX_M_UI_IMG_SUB_PAGE_ID), TRUE);
	ONDK_GWM_Obj_DisableObject(TTX_M_UI_IMG_SUB_PAGE_ID);

	/* Button Mix */
	nx_ttxui_GetTtxMenuUiPosition(TTX_M_UI_BTN_MIX_ID, &stRect);
	ONDK_GWM_COM_Button_Create (TTX_M_UI_BTN_MIX_ID, &stRect, NULL);
	ONDK_GWM_APP_SetObjectDrawMethod (TTX_M_UI_BTN_MIX_ID, nx_ttxui_DrawTtxMenuUiButton);

	ONDK_GWM_COM_Image_Create(TTX_M_UI_IMG_MIX_ID,&stRect);
	ONDK_GWM_Obj_SetVisible((TTX_M_UI_IMG_MIX_ID), TRUE);
	ONDK_GWM_Obj_DisableObject(TTX_M_UI_IMG_MIX_ID);

	/* Button Cancel */
	nx_ttxui_GetTtxMenuUiPosition(TTX_M_UI_BTN_CANCEL_ID, &stRect);
	ONDK_GWM_COM_Button_Create (TTX_M_UI_BTN_CANCEL_ID, &stRect, NULL);
	ONDK_GWM_APP_SetObjectDrawMethod (TTX_M_UI_BTN_CANCEL_ID, nx_ttxui_DrawTtxMenuUiButton);
	ONDK_GWM_COM_Image_Create(TTX_M_UI_IMG_CANCEL_ID,&stRect);
	ONDK_GWM_Obj_SetVisible((TTX_M_UI_IMG_CANCEL_ID), TRUE);
	ONDK_GWM_Obj_DisableObject(TTX_M_UI_IMG_CANCEL_ID);

	/* Button Reveal */
	nx_ttxui_GetTtxMenuUiPosition(TTX_M_UI_BTN_REVEAL_ID, &stRect);
	ONDK_GWM_COM_Button_Create (TTX_M_UI_BTN_REVEAL_ID, &stRect, NULL);
	ONDK_GWM_APP_SetObjectDrawMethod (TTX_M_UI_BTN_REVEAL_ID, nx_ttxui_DrawTtxMenuUiButton);
	ONDK_GWM_COM_Image_Create(TTX_M_UI_IMG_REVEAL_ID,&stRect);
	ONDK_GWM_Obj_SetVisible((TTX_M_UI_IMG_REVEAL_ID), TRUE);
	ONDK_GWM_Obj_DisableObject(TTX_M_UI_IMG_REVEAL_ID);

	/* Button Double */
	nx_ttxui_GetTtxMenuUiPosition(TTX_M_UI_BTN_DOUBLE_ID, &stRect);
	ONDK_GWM_COM_Button_Create (TTX_M_UI_BTN_DOUBLE_ID, &stRect, NULL);
	ONDK_GWM_APP_SetObjectDrawMethod (TTX_M_UI_BTN_DOUBLE_ID, nx_ttxui_DrawTtxMenuUiButton);
	ONDK_GWM_COM_Image_Create(TTX_M_UI_IMG_DOUBLE_ID,&stRect);
	ONDK_GWM_Obj_SetVisible((TTX_M_UI_IMG_DOUBLE_ID), TRUE);
	ONDK_GWM_Obj_DisableObject(TTX_M_UI_IMG_DOUBLE_ID);

	/* Button Hold */
	nx_ttxui_GetTtxMenuUiPosition(TTX_M_UI_BTN_HOLD_ID, &stRect);
	ONDK_GWM_COM_Button_Create (TTX_M_UI_BTN_HOLD_ID, &stRect, NULL);
	ONDK_GWM_APP_SetObjectDrawMethod (TTX_M_UI_BTN_HOLD_ID, nx_ttxui_DrawTtxMenuUiButton);
	ONDK_GWM_COM_Image_Create(TTX_M_UI_IMG_HOLD_ID,&stRect);
	ONDK_GWM_Obj_SetVisible((TTX_M_UI_IMG_HOLD_ID), TRUE);
	ONDK_GWM_Obj_DisableObject(TTX_M_UI_IMG_HOLD_ID);

	/* Set focus */
	ONDK_GWM_Obj_SetFocus (TTX_M_UI_BTN_INDEX_ID);

}

static void		nx_ttxui_GetTtxMenuUiPosition(eTtxMenuUiCompIds eUiId, ONDK_Rect_t *rect)
{
	if (rect != NULL)
	{
		if ( eUiId >= TTX_M_UI_ID_MAX )
		{
			ONDK_Error("\n[%s:%d] Ui ID Error..[%d]\n",__FUNCTION__,__LINE__,eUiId);

			HxSTD_MemCopy(rect, &s_stTtxMenuUiRectComponent[TTX_M_UI_BTN_INDEX_ID], sizeof(ONDK_Rect_t));
		}
		else
		{
			HxSTD_MemCopy(rect, &s_stTtxMenuUiRectComponent[eUiId], sizeof(ONDK_Rect_t));
		}
	}
}

static void 	nx_ttxui_KeyConv_UiCommon (HINT32 *nKeyCode)
{
	switch(*nKeyCode)
	{
		case KEY_FRONT_STANDBY:
			*nKeyCode = KEY_STANDBY;
			break;

		case KEY_FRONT_EXT_AV:
			*nKeyCode = KEY_BACK;
			break;

		case KEY_FRONT_MENU:
			*nKeyCode = KEY_OK;
			break;

		case KEY_FRONT_CH_PLUS:
			*nKeyCode = KEY_ARROWUP;
			break;

		case KEY_FRONT_CH_MINUS:
			*nKeyCode = KEY_ARROWDOWN;
			break;

		case KEY_FRONT_VOLUME_UP:
			*nKeyCode = KEY_ARROWRIGHT;
			break;

		case KEY_FRONT_VOLUME_DOWN:
			*nKeyCode = KEY_ARROWLEFT;
			break;

		default:
			break;
	}
}


static ONDK_Result_t	nx_ttxui_MsgGwmCreate(void)
{
	HxLOG_Print("[nx_ttxui_MsgGwmCreate]\n");

	NX_PRISM_TTxMenuObject_Create();

	/* Init Ttx Menu UI. */
	nx_ttxui_InitTtxMenuUiObj();

	ONDK_GWM_SetTimer(GWMPOPUP_TIMER_TTXMENU, NX_PRISM_TTX_MENU_UI_TIMEOUT);
	return ONDK_MESSAGE_CONSUMED;
}

static ONDK_Result_t	nx_ttxui_MsgGwmDlgDestroy(void)
{
	HxLOG_Print("[nx_ttxui_MsgGwmDlgDestroy]\n");
	ONDK_GWM_KillTimer(GWMPOPUP_TIMER_TTXMENU);

	ONDK_GWM_COM_Image_Change(TTX_M_UI_IMG_INDEX_ID, NULL);
	ONDK_GWM_COM_Image_Change(TTX_M_UI_IMG_SUB_PAGE_ID, NULL);
	ONDK_GWM_COM_Image_Change(TTX_M_UI_IMG_MIX_ID, NULL);
	ONDK_GWM_COM_Image_Change(TTX_M_UI_IMG_CANCEL_ID, NULL);
	ONDK_GWM_COM_Image_Change(TTX_M_UI_IMG_REVEAL_ID, NULL);
	ONDK_GWM_COM_Image_Change(TTX_M_UI_IMG_DOUBLE_ID, NULL);
	ONDK_GWM_COM_Image_Change(TTX_M_UI_IMG_HOLD_ID, NULL);

	return ONDK_RET_OK;
}

static ONDK_Result_t	nx_ttxui_MsgGwmClicked(HINT32 p1)
{
	HERROR				hErr;
	TTX_Mix_t			mix;
	TTX_DoubleSize_t	doubleSize;
    ONDK_Print("\n[%s:%d] p1= [%d]\n",__FUNCTION__,__LINE__,p1);

	switch (p1)
	{
		case TTX_M_UI_BTN_INDEX_ID :
			NX_PRISM_Teletext_SetControl( eTTX_CONTROL_INDEX, 0);
			ONDK_GWM_APP_Destroy (0);
			break;

		case TTX_M_UI_BTN_SUB_PAGE_ID :
			NX_PRISM_Teletext_SetControl(eTTX_CONTROL_SUBPAGE, 0);
			ONDK_GWM_APP_Destroy (0);
			break;

		case TTX_M_UI_BTN_MIX_ID :
			hErr = NX_PRISM_Teletext_GetControl(eTTX_CONTROL_MIX, &mix);
			if(hErr==ERR_OK)
		        HxLOG_Assert("NX_PRISM_Teletext_GetControl( TTX_M_UI_BTN_MIX_ID); \n");
			switch (mix)
			{
				case eTTX_MIX_100_PERCENT :	/* On */
					mix = eTTX_MIX_0_PERCENT;
					break;

				case eTTX_MIX_50_PERCENT :
					mix = eTTX_MIX_100_PERCENT;
					break;

				case eTTX_MIX_0_PERCENT :	/* Off */
				default :
					mix = eTTX_MIX_50_PERCENT;
					break;
			}

			NX_PRISM_Teletext_SetControl(eTTX_CONTROL_MIX, mix);
			break;

		case TTX_M_UI_BTN_CANCEL_ID :
			NX_PRISM_Teletext_SetControl( eTTX_CONTROL_CANCEL, 0);
			break;

		case TTX_M_UI_BTN_REVEAL_ID :
			NX_PRISM_Teletext_SetControl( eTTX_CONTROL_REVEAL, 0);
			break;

		case TTX_M_UI_BTN_DOUBLE_ID :
			NX_PRISM_Teletext_GetControl( eTTX_CONTROL_DOUBLE, &doubleSize);
			if ( doubleSize == eTTX_DOUBLE_OFF )
			{
				NX_PRISM_Teletext_SetControl( eTTX_CONTROL_DOUBLE, eTTX_DOUBLE_ON /* eTTX_DOUBLE_FOCUS_ON_TOP */);
			}
			else
			{
				NX_PRISM_Teletext_SetControl(eTTX_CONTROL_DOUBLE, eTTX_DOUBLE_OFF);
			}
			break;

		case TTX_M_UI_BTN_HOLD_ID :
			NX_PRISM_Teletext_SetControl( eTTX_CONTROL_HOLD, 0);
			break;

		default :
			break;

	}

	return ONDK_MESSAGE_CONSUMED;
}

static ONDK_Result_t	nx_ttxui_MsgGwmKeyDown(HINT32 p1)
{

	ONDK_GWM_ResetTimer (GWMPOPUP_TIMER_TTXMENU);

	nx_ttxui_KeyConv_UiCommon(&p1);
    ONDK_Print("[%s:%d] key = [0x%x]\n",__FUNCTION__,__LINE__,p1);
	switch (p1)
	{
		case KEY_STANDBY:
			ONDK_GWM_APP_Destroy (0);
			ONDK_GWM_APP_Destroy(NX_PRISM_Teletext_Proc);
			return ONDK_MESSAGE_NONCONSUMED;
		case KEY_OPT:
		case KEY_BACK :
			ONDK_GWM_APP_Destroy (0);
			return ONDK_MESSAGE_CONSUMED;
		case KEY_EXIT :
		case KEY_GUIDE:
		case KEY_SPECIAL_HOME:
			ONDK_GWM_APP_Destroy (0);
			 ONDK_Print("[%s:%d] key = [0x%x] ONDK_MESSAGE_CONSUMED\n",__FUNCTION__,__LINE__,p1);
			return ONDK_MESSAGE_PASS;
		case KEY_TEXT:
			 ONDK_Print("[%s:%d] key = [0x%x] ONDK_MESSAGE_NONCONSUMED\n",__FUNCTION__,__LINE__,p1);
			return ONDK_MESSAGE_NONCONSUMED;
		case KEY_OK:
			 ONDK_Print("[%s:%d] key = [0x%x] ONDK_MESSAGE_OK\n",__FUNCTION__,__LINE__,p1);
			return ONDK_MESSAGE_OK;
		case KEY_ARROWLEFT:
		case KEY_ARROWRIGHT:
			 ONDK_Print("[%s:%d] key = [0x%x] ONDK_MESSAGE_OK\n",__FUNCTION__,__LINE__,p1);
			return ONDK_MESSAGE_OK;
		default :
			 ONDK_Print("[%s:%d] key = [0x%x] ONDK_MESSAGE_CONSUMED\n",__FUNCTION__,__LINE__,p1);
			return ONDK_MESSAGE_CONSUMED;
	}
}

static ONDK_Result_t	nx_ttxui_MsgGwmKeyUp(HINT32 p1)
{
	nx_ttxui_KeyConv_UiCommon(&p1);
    ONDK_Print("[%s:%d] key = [0x%x]\n",__FUNCTION__,__LINE__,p1);
	switch (p1)
	{
		case KEY_STANDBY:
			return ONDK_MESSAGE_PASS;
		case KEY_BACK :
			ONDK_GWM_APP_Destroy (0);
			return ONDK_MESSAGE_CONSUMED;
		case KEY_EXIT :
			ONDK_GWM_APP_Destroy (0);
			 ONDK_Print("[%s:%d] key = [0x%x] ONDK_MESSAGE_CONSUMED\n",__FUNCTION__,__LINE__,p1);
			return ONDK_MESSAGE_PASS;
		case KEY_TEXT:
			 ONDK_Print("[%s:%d] key = [0x%x] ONDK_MESSAGE_NONCONSUMED\n",__FUNCTION__,__LINE__,p1);
			 return ONDK_MESSAGE_NONCONSUMED;
		case KEY_OK:
			 ONDK_Print("[%s:%d] key = [0x%x] ONDK_MESSAGE_OK\n",__FUNCTION__,__LINE__,p1);
			return ONDK_MESSAGE_OK;
		default :
			ONDK_Print("[%s:%d] key = [0x%x] ONDK_MESSAGE_CONSUMED\n",__FUNCTION__,__LINE__,p1);
			return ONDK_MESSAGE_CONSUMED;
	}
}

static ONDK_Result_t	nx_ttxui_MsgGwmProcessTimer(HINT32 p1)
{
	if ( p1 == GWMPOPUP_TIMER_TTXMENU )
	{
		ONDK_GWM_APP_Destroy (0);

		return ONDK_MESSAGE_BREAK;
	}

	return ONDK_RET_OK;
}


static ONDK_Result_t	nx_ttxui_MsgGwmWrapAround(HINT32 p1)
{
	ONDK_Print("TTXMenu around>>> [%d]\n",p1);

	switch (p1)
	{
		case TTX_M_UI_BTN_INDEX_ID :
			ONDK_GWM_Obj_SetFocus(TTX_M_UI_BTN_HOLD_ID);
			break;

		case TTX_M_UI_BTN_HOLD_ID :
			ONDK_GWM_Obj_SetFocus(TTX_M_UI_BTN_INDEX_ID);
			break;

		default :
			break;
	}

	return ONDK_MESSAGE_CONSUMED;
}


/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/

ONDK_Result_t	NX_PRISM_TtxMenuUi_Proc (HINT32 message, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t			eResult = ONDK_RET_OK;
	ONDK_Print(HxANSI_COLOR_YELLOW("[%s:%d] Message(0x%x) : %s  p1(0x%x) p2(0x%x) p3(0x%x) \n"), __FUNCTION__, __LINE__, message, ONDK_GWM_GwmMsg2String(message), p1, p2, p3);

	switch (message)
	{
		/**********************************************************************
			GWM MESSAGES
		**********************************************************************/
		case MSG_GWM_CREATE :
			eResult = nx_ttxui_MsgGwmCreate();
			break;

		case MSG_GWM_KEYDOWN :
			eResult = nx_ttxui_MsgGwmKeyDown(p1);
			break;
		case MSG_GWM_KEYUP:
			eResult = nx_ttxui_MsgGwmKeyUp(p1);
			break;
		case MSG_GWM_CLICKED :
			ONDK_GWM_ResetTimer(GWMPOPUP_TIMER_TTXMENU);
			eResult = nx_ttxui_MsgGwmClicked(p1);
			break;
		case MSG_GWM_SCROLLLEFT :
		case MSG_GWM_SCROLLRIGHT :
			ONDK_GWM_ResetTimer(GWMPOPUP_TIMER_TTXMENU);
			eResult = nx_ttxui_MsgGwmWrapAround(p1);
			break;
		case MSG_GWM_TIMER :
			eResult = nx_ttxui_MsgGwmProcessTimer(p1);
			break;

		case MSG_GWM_DESTROY :
			eResult = nx_ttxui_MsgGwmDlgDestroy();
			break;

		default :
			break;

	}

	if ( eResult != ONDK_RET_OK )
	{
		return eResult;
	}

	return ONDK_GWM_ProcessMessageDefault (message, handle, p1, p2, p3);
}


void	NX_PRISM_TTxMenuObject_Create(void)
{
	HINT32		index = 0;

	NX_PRISM_TTXMenuObject_Macro(Teletext_Index_C_Off, RES_561_00_T_INDEX_OFF_C_PNG);
	NX_PRISM_TTXMenuObject_Macro(Teletext_Index_C_On, RES_561_00_T_INDEX_ON_C_PNG);
	NX_PRISM_TTXMenuObject_Macro(Teletext_Index_N_Off, RES_561_00_T_INDEX_OFF_N_PNG);

	NX_PRISM_TTXMenuObject_Macro(Teletext_Subpage_C_Off,RES_561_00_T_SUBPAGE_OFF_C_PNG );
    NX_PRISM_TTXMenuObject_Macro(Teletext_Subpage_C_On, RES_561_00_T_SUBPAGE_ON_C_PNG);
    NX_PRISM_TTXMenuObject_Macro(Teletext_Subpage_N_Off, RES_561_00_T_SUBPAGE_OFF_N_PNG);

	NX_PRISM_TTXMenuObject_Macro(Teletext_Mix_C_50, RES_561_00_T_MIX_50_C_PNG);
	NX_PRISM_TTXMenuObject_Macro(Teletext_Mix_C_Off, RES_561_00_T_MIX_OFF_C_PNG);
	NX_PRISM_TTXMenuObject_Macro(Teletext_Mix_C_On, RES_561_00_T_MIX_ON_C_PNG);
	NX_PRISM_TTXMenuObject_Macro(Teletext_Mix_N_50, RES_561_00_T_MIX_50_N_PNG);
	NX_PRISM_TTXMenuObject_Macro(Teletext_Mix_N_Off, RES_561_00_T_MIX_OFF_N_PNG);
	NX_PRISM_TTXMenuObject_Macro(Teletext_Mix_N_On, RES_561_00_T_MIX_ON_N_PNG);

	NX_PRISM_TTXMenuObject_Macro(Teletext_Cancel_C_Off, RES_561_00_T_CANCEL_OFF_C_PNG);
	NX_PRISM_TTXMenuObject_Macro(Teletext_Cancel_C_On, RES_561_00_T_CANCEL_ON_C_PNG);
	NX_PRISM_TTXMenuObject_Macro(Teletext_Cancel_N_Off, RES_561_00_T_CANCEL_OFF_N_PNG);
	NX_PRISM_TTXMenuObject_Macro(Teletext_Cancel_N_On, RES_561_00_T_CANCEL_ON_N_PNG);

	NX_PRISM_TTXMenuObject_Macro(Teletext_Reveal_C_Off, RES_561_00_T_REVEAL_OFF_C_PNG);
	NX_PRISM_TTXMenuObject_Macro(Teletext_Reveal_C_On, RES_561_00_T_REVEAL_ON_C_PNG);
	NX_PRISM_TTXMenuObject_Macro(Teletext_Reveal_N_Off, RES_561_00_T_REVEAL_OFF_N_PNG);
	NX_PRISM_TTXMenuObject_Macro(Teletext_Reveal_N_On, RES_561_00_T_REVEAL_ON_N_PNG);

	NX_PRISM_TTXMenuObject_Macro(Teletext_DoubleSize_C_Off, RES_561_00_T_DOUBLESIZE_OFF_C_PNG);
	NX_PRISM_TTXMenuObject_Macro(Teletext_DoubleSize_C_On, RES_561_00_T_DOUBLESIZE_ON_C_PNG);
	NX_PRISM_TTXMenuObject_Macro(Teletext_DoubleSize_N_Off, RES_561_00_T_DOUBLESIZE_OFF_N_PNG);
	NX_PRISM_TTXMenuObject_Macro(Teletext_DoubleSize_N_On, RES_561_00_T_DOUBLESIZE_ON_N_PNG);

	NX_PRISM_TTXMenuObject_Macro(Teletext_PageHold_C_Off, RES_561_00_T_PAGEHOLD_OFF_C_PNG);
	NX_PRISM_TTXMenuObject_Macro(Teletext_PageHold_C_On, RES_561_00_T_PAGEHOLD_ON_C_PNG);
	NX_PRISM_TTXMenuObject_Macro(Teletext_PageHold_N_Off, RES_561_00_T_PAGEHOLD_OFF_N_PNG);
	NX_PRISM_TTXMenuObject_Macro(Teletext_PageHold_N_On, RES_561_00_T_PAGEHOLD_ON_N_PNG);
}

/**************************************************************************************************/
#define _________AP_TTX_MENU_End_Of_File____________________________________________________
/**************************************************************************************************/
