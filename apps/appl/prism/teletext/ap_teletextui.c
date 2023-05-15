/**************************************************************
*	http://www.humaxdigital.com
*	@author			humax
**************************************************************/


/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding �� ����ϴ� source file �Դϴ�.
* MS949 encoding�����Ī��� source file�Ǫ���
* Quelldatei, die MS949-Codierung verwendet.
*
*/


/*
* �� 2011-2012 Humax Co., Ltd.
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
#define _________AP_TTX_MENU_Private_Include_________________________________________________
/**************************************************************************************************/
#include <teletext.h>


/**************************************************************************************************/
#define _________AP_TTX_MENU_Private_Macro_________________________________________________
/**************************************************************************************************/

#define TTX_MENU_UI_TIMEOUT				(3*1000)	// 3 seconds

#define TTX_MENU_GRAP 					8
#define TTX_MENU_WIDTH					75
#define TTX_MENU_HEIGHT				46

#define TTX_MENU_Y						18

 /* Main Frame. */
#define D_TTX_DIALOG_X					350
#define D_TTX_DIALOG_Y					596
#define D_TTX_DIALOG_W					650
#define D_TTX_DIALOG_H					78

 /* Frame. */
#define D_TTX_DIALOG_FRAME_X			0
#define D_TTX_DIALOG_FRAME_Y			0
#define D_TTX_DIALOG_FRAME_W			D_TTX_DIALOG_W
#define D_TTX_DIALOG_FRAME_H			D_TTX_DIALOG_H

 /* TTX Menu UI Index. */
#define D_TTX_MENU_INDEX_X				38

 /* TTX Menu UI Sub-Page. */
#define D_TTX_MENU_SUB_PAGE_X			(D_TTX_MENU_INDEX_X + TTX_MENU_WIDTH + TTX_MENU_GRAP)

 /* TTX Menu UI Mix. */
#define D_TTX_MENU_MIX_X				(D_TTX_MENU_SUB_PAGE_X+ TTX_MENU_WIDTH+ TTX_MENU_GRAP)

 /* TTX Menu UI Cancel. */
#define D_TTX_MENU_CANCEL_X				(D_TTX_MENU_MIX_X+ TTX_MENU_WIDTH+ TTX_MENU_GRAP)

 /* TTX Menu UI Reveal. */
#define D_TTX_MENU_REVEAL_X				(D_TTX_MENU_CANCEL_X+ TTX_MENU_WIDTH+ TTX_MENU_GRAP)

 /* TTX Menu UI Double. */
#define D_TTX_MENU_DOUBLE_X				(D_TTX_MENU_REVEAL_X+ TTX_MENU_WIDTH+ TTX_MENU_GRAP)

 /* TTX Menu UI Hold. */
#define D_TTX_MENU_HOLD_X				(D_TTX_MENU_DOUBLE_X+ TTX_MENU_WIDTH+ TTX_MENU_GRAP)


#define IMAGE_PATH                          "/usr/image"

#define TTxMenuObject_Macro(type,val) s_stTTxMenu[index].eID =type; s_stTTxMenu[index++].hHandle = local_ttxui_createImage(val);

/**************************************************************************************************/
#define _________AP_TTX_MENU_Private_Struct_________________________________________________
/**************************************************************************************************/
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
    Teletext_Index_C_Off =0x00010000,
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
}TtxMenuIMG_Id_e;

typedef struct{
	TtxMenuIMG_Id_e 	eID;
	ONDKImageHandle 	hHandle;
}TtxMenuObject_t;



/**************************************************************************************************/
#define _________AP_TTX_MENU_Private_Value_________________________________________________
/**************************************************************************************************/
#ifdef CONFIG_DEBUG
HUINT32 g_ApTtxMenuUi_Level = 0x00;
#endif

const static ONDK_Rect_t		s_stTtxMenuUiRectComponent[TTX_M_UI_ID_MAX] =
{
		{	D_TTX_DIALOG_X,					D_TTX_DIALOG_Y, 					D_TTX_DIALOG_W,					D_TTX_DIALOG_H			},
		{	D_TTX_DIALOG_FRAME_X,				D_TTX_DIALOG_FRAME_Y,				D_TTX_DIALOG_FRAME_W,				D_TTX_DIALOG_FRAME_H	},
		{	D_TTX_MENU_INDEX_X,				TTX_MENU_Y,				TTX_MENU_WIDTH,				TTX_MENU_HEIGHT		},
		{	D_TTX_MENU_SUB_PAGE_X,			TTX_MENU_Y,			TTX_MENU_WIDTH,				TTX_MENU_HEIGHT	},
		{	D_TTX_MENU_MIX_X,					TTX_MENU_Y,				TTX_MENU_WIDTH,				TTX_MENU_HEIGHT		},
		{	D_TTX_MENU_CANCEL_X,				TTX_MENU_Y,				TTX_MENU_WIDTH,				TTX_MENU_HEIGHT		},
		{	D_TTX_MENU_REVEAL_X,				TTX_MENU_Y,				TTX_MENU_WIDTH,				TTX_MENU_HEIGHT		},
		{	D_TTX_MENU_DOUBLE_X,				TTX_MENU_Y,				TTX_MENU_WIDTH,				TTX_MENU_HEIGHT		},
		{	D_TTX_MENU_HOLD_X,				TTX_MENU_Y,				TTX_MENU_WIDTH,				TTX_MENU_HEIGHT		},
		{	0,									0,									0,									0						},
};


static TtxMenuObject_t  		s_stTTxMenu[28];

/**************************************************************************************************/
#define _________AP_TTX_MENU_Private_Prototype_________________________________________________
/**************************************************************************************************/
static ONDK_Result_t local_ttxui_DrawTtxMenuUiButton (ONDKSurfaceHandle screen, ONDK_Region_t *area,
								ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop);

static void local_ttxui_InitTtxMenuUiObj (void);
static void local_ttxui_GetTtxMenuUiPosition(eTtxMenuUiCompIds eUiId, ONDK_Rect_t *rect);
/**************************************************************************************************/
#define _________AP_TTX_MENU_Private_Init__________________________________________________________
/**************************************************************************************************/


static ONDK_Result_t local_ttxui_MsgGwmCreate(void)
{
	HxLOG_Print("[local_ttxui_MsgGwmCreate]\n");

	/* Init Ttx Menu UI. */
	local_ttxui_InitTtxMenuUiObj();

	ONDK_GWM_SetTimer(GWMPOPUP_TIMER_TTXMENU, TTX_MENU_UI_TIMEOUT);
	return ONDK_MESSAGE_CONSUMED;
}

static ONDKImageHandle local_ttxui_createImage(HCHAR *filename)
{
    HCHAR               imagepath[256];

	HxSTD_snprintf(imagepath, 256, "%s/510_%s.png", IMAGE_PATH, filename);
	imagepath[255] = '\0';

	return ONDK_IMG_CreateSelf(imagepath);
}



static ONDKImageHandle *local_ttxui_getIMGHandle(TtxMenuIMG_Id_e eID)
{
	HINT32 i,j;
	for(i=Teletext_Index_C_Off ,j=0; i<= Teletext_PageHold_N_On; i++,j++)
	{
		if(i == eID)
			return &s_stTTxMenu[j].hHandle;
	}
	return NULL;
}


static ONDK_Result_t local_ttxui_framedraw (ONDKSurfaceHandle screen, ONDK_Region_t *area,
								ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	ONDK_DRAW_FillRect(screen,ONDK_Rect(rect->x,rect->y,rect->w,rect->h),0xAA000000);
	return ONDK_RET_OK;
}

void local_ttxui_InitTtxMenuUiObj (void)
{
	ONDK_Rect_t		 	stRect;

    /* Set Area. */
	local_ttxui_GetTtxMenuUiPosition(TTX_M_UI_MAIN_FRAME_ID, &stRect);
	ONDK_GWM_APP_SetAppArea (stRect.x, stRect.y, stRect.w, stRect.h);
    ONDK_GWM_COM_Frame_Create(TTX_M_UI_MAIN_FRAME_ID, &stRect, NULL);
	ONDK_GWM_APP_SetObjectDrawMethod(TTX_M_UI_MAIN_FRAME_ID, local_ttxui_framedraw);


	/* Button Index */
	local_ttxui_GetTtxMenuUiPosition(TTX_M_UI_BTN_INDEX_ID, &stRect);
	ONDK_GWM_COM_Button_Create (TTX_M_UI_BTN_INDEX_ID, &stRect, NULL);
    ONDK_GWM_APP_SetObjectDrawMethod (TTX_M_UI_BTN_INDEX_ID, local_ttxui_DrawTtxMenuUiButton);
	ONDK_GWM_COM_Image_Create(TTX_M_UI_IMG_INDEX_ID,&stRect);
	ONDK_GWM_Obj_SetVisible((TTX_M_UI_IMG_INDEX_ID), TRUE);
	ONDK_GWM_Obj_DisableObject(TTX_M_UI_IMG_INDEX_ID);


	/* Button Sub-Page */
	local_ttxui_GetTtxMenuUiPosition(TTX_M_UI_BTN_SUB_PAGE_ID, &stRect);
	ONDK_GWM_COM_Button_Create (TTX_M_UI_BTN_SUB_PAGE_ID, &stRect, NULL);
	ONDK_GWM_APP_SetObjectDrawMethod (TTX_M_UI_BTN_SUB_PAGE_ID, local_ttxui_DrawTtxMenuUiButton);
	ONDK_GWM_COM_Image_Create(TTX_M_UI_IMG_SUB_PAGE_ID,&stRect);
	ONDK_GWM_Obj_SetVisible((TTX_M_UI_IMG_SUB_PAGE_ID), TRUE);
	ONDK_GWM_Obj_DisableObject(TTX_M_UI_IMG_SUB_PAGE_ID);


	/* Button Mix */
	local_ttxui_GetTtxMenuUiPosition(TTX_M_UI_BTN_MIX_ID, &stRect);
	ONDK_GWM_COM_Button_Create (TTX_M_UI_BTN_MIX_ID, &stRect, NULL);
	ONDK_GWM_APP_SetObjectDrawMethod (TTX_M_UI_BTN_MIX_ID, local_ttxui_DrawTtxMenuUiButton);

	ONDK_GWM_COM_Image_Create(TTX_M_UI_IMG_MIX_ID,&stRect);
	ONDK_GWM_Obj_SetVisible((TTX_M_UI_IMG_MIX_ID), TRUE);
	ONDK_GWM_Obj_DisableObject(TTX_M_UI_IMG_MIX_ID);



	/* Button Cancel */
	local_ttxui_GetTtxMenuUiPosition(TTX_M_UI_BTN_CANCEL_ID, &stRect);
	ONDK_GWM_COM_Button_Create (TTX_M_UI_BTN_CANCEL_ID, &stRect, NULL);
	ONDK_GWM_APP_SetObjectDrawMethod (TTX_M_UI_BTN_CANCEL_ID, local_ttxui_DrawTtxMenuUiButton);
	ONDK_GWM_COM_Image_Create(TTX_M_UI_IMG_CANCEL_ID,&stRect);
	ONDK_GWM_Obj_SetVisible((TTX_M_UI_IMG_CANCEL_ID), TRUE);
	ONDK_GWM_Obj_DisableObject(TTX_M_UI_IMG_CANCEL_ID);


	/* Button Reveal */
	local_ttxui_GetTtxMenuUiPosition(TTX_M_UI_BTN_REVEAL_ID, &stRect);
	ONDK_GWM_COM_Button_Create (TTX_M_UI_BTN_REVEAL_ID, &stRect, NULL);
	ONDK_GWM_APP_SetObjectDrawMethod (TTX_M_UI_BTN_REVEAL_ID, local_ttxui_DrawTtxMenuUiButton);
	ONDK_GWM_COM_Image_Create(TTX_M_UI_IMG_REVEAL_ID,&stRect);
	ONDK_GWM_Obj_SetVisible((TTX_M_UI_IMG_REVEAL_ID), TRUE);
	ONDK_GWM_Obj_DisableObject(TTX_M_UI_IMG_REVEAL_ID);


	/* Button Double */
	local_ttxui_GetTtxMenuUiPosition(TTX_M_UI_BTN_DOUBLE_ID, &stRect);
	ONDK_GWM_COM_Button_Create (TTX_M_UI_BTN_DOUBLE_ID, &stRect, NULL);
	ONDK_GWM_APP_SetObjectDrawMethod (TTX_M_UI_BTN_DOUBLE_ID, local_ttxui_DrawTtxMenuUiButton);
	ONDK_GWM_COM_Image_Create(TTX_M_UI_IMG_DOUBLE_ID,&stRect);
	ONDK_GWM_Obj_SetVisible((TTX_M_UI_IMG_DOUBLE_ID), TRUE);
	ONDK_GWM_Obj_DisableObject(TTX_M_UI_IMG_DOUBLE_ID);

	/* Button Hold */
	local_ttxui_GetTtxMenuUiPosition(TTX_M_UI_BTN_HOLD_ID, &stRect);
	ONDK_GWM_COM_Button_Create (TTX_M_UI_BTN_HOLD_ID, &stRect, NULL);
	ONDK_GWM_APP_SetObjectDrawMethod (TTX_M_UI_BTN_HOLD_ID, local_ttxui_DrawTtxMenuUiButton);
	ONDK_GWM_COM_Image_Create(TTX_M_UI_IMG_HOLD_ID,&stRect);
	ONDK_GWM_Obj_SetVisible((TTX_M_UI_IMG_HOLD_ID), TRUE);
	ONDK_GWM_Obj_DisableObject(TTX_M_UI_IMG_HOLD_ID);

	/* Set focus */
	ONDK_GWM_Obj_SetFocus (TTX_M_UI_BTN_INDEX_ID);

}


/**************************************************************************************************/
#define _________AP_TTX_MENU_Private_Update__________________________________________________________
/**************************************************************************************************/

ONDK_Result_t local_ttxui_DrawTtxMenuUiButton (ONDKSurfaceHandle screen, ONDK_Region_t *area,
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
				ONDK_GWM_COM_Image_Change(TTX_M_UI_IMG_INDEX_ID,local_ttxui_getIMGHandle(Teletext_Index_C_On) );
			else
			{
				if(push_status & GUIOBJ_HILIGHTED)
					ONDK_GWM_COM_Image_Change(TTX_M_UI_IMG_INDEX_ID,local_ttxui_getIMGHandle(Teletext_Index_C_Off) );
				else
					ONDK_GWM_COM_Image_Change(TTX_M_UI_IMG_INDEX_ID,local_ttxui_getIMGHandle(Teletext_Index_N_Off) );
			}
			break;
		case TTX_M_UI_BTN_SUB_PAGE_ID :

			if ( push_status & GUIOBJ_PUSHED )
				ONDK_GWM_COM_Image_Change(TTX_M_UI_IMG_SUB_PAGE_ID,local_ttxui_getIMGHandle(Teletext_Subpage_C_On) );
			else
			{
				if(push_status & GUIOBJ_HILIGHTED)
					ONDK_GWM_COM_Image_Change(TTX_M_UI_IMG_SUB_PAGE_ID,local_ttxui_getIMGHandle(Teletext_Subpage_C_Off) );
				else
					ONDK_GWM_COM_Image_Change(TTX_M_UI_IMG_SUB_PAGE_ID,local_ttxui_getIMGHandle(Teletext_Subpage_N_Off) );
			}
			break;

		case TTX_M_UI_BTN_MIX_ID :

			hErr = Teletext_GetControl( eTTX_CONTROL_MIX, &mix);
			if(hErr!=ERR_OK)
		        HxLOG_Assert("Teletext_GetControl( eTTX_CONTROL_MIX); \n");

			switch (mix)
			{
				case eTTX_MIX_100_PERCENT :	/* On */
					{
						if(push_status & GUIOBJ_HILIGHTED)
							ONDK_GWM_COM_Image_Change(TTX_M_UI_IMG_MIX_ID,local_ttxui_getIMGHandle(Teletext_Mix_C_On) );

						else
							ONDK_GWM_COM_Image_Change(TTX_M_UI_IMG_MIX_ID,local_ttxui_getIMGHandle(Teletext_Mix_N_On) );
					}
					break;

				case eTTX_MIX_50_PERCENT :
					{
						if(push_status & GUIOBJ_HILIGHTED)
							ONDK_GWM_COM_Image_Change(TTX_M_UI_IMG_MIX_ID,local_ttxui_getIMGHandle(Teletext_Mix_C_50) );
						else
							ONDK_GWM_COM_Image_Change(TTX_M_UI_IMG_MIX_ID,local_ttxui_getIMGHandle(Teletext_Mix_N_50) );
					}
					break;

				case eTTX_MIX_0_PERCENT :	/* Off */
				default :
					{
						if(push_status & GUIOBJ_HILIGHTED)
							ONDK_GWM_COM_Image_Change(TTX_M_UI_IMG_MIX_ID,local_ttxui_getIMGHandle(Teletext_Mix_C_Off) );
						else
							ONDK_GWM_COM_Image_Change(TTX_M_UI_IMG_MIX_ID,local_ttxui_getIMGHandle((Teletext_Mix_N_Off)) );
					}
					break;
			}
			break;

		case TTX_M_UI_BTN_CANCEL_ID :
			hErr = Teletext_GetControl( eTTX_CONTROL_CANCEL, &bSet);
			if(hErr!=ERR_OK)
		        HxLOG_Assert("Teletext_GetControl( eTTX_CONTROL_CANCEL); \n");

			if ( bSet == TRUE )
			{
				if(push_status & GUIOBJ_HILIGHTED)
					ONDK_GWM_COM_Image_Change(TTX_M_UI_IMG_CANCEL_ID,local_ttxui_getIMGHandle(Teletext_Cancel_C_On) );
				else
					ONDK_GWM_COM_Image_Change(TTX_M_UI_IMG_CANCEL_ID,local_ttxui_getIMGHandle(Teletext_Cancel_N_On) );
			}
			else
			{

				if(push_status & GUIOBJ_HILIGHTED)
					ONDK_GWM_COM_Image_Change(TTX_M_UI_IMG_CANCEL_ID,local_ttxui_getIMGHandle(Teletext_Cancel_C_Off) );
				else
					ONDK_GWM_COM_Image_Change(TTX_M_UI_IMG_CANCEL_ID,local_ttxui_getIMGHandle(Teletext_Cancel_N_Off) );
			}
			break;

		case TTX_M_UI_BTN_REVEAL_ID :
			hErr = Teletext_GetControl( eTTX_CONTROL_REVEAL, &bSet);
			if(hErr!=ERR_OK)
		        HxLOG_Assert("Teletext_GetControl( eTTX_CONTROL_REVEAL); \n");

			if ( bSet == TRUE )
			{
				if(push_status & GUIOBJ_HILIGHTED)
					ONDK_GWM_COM_Image_Change(TTX_M_UI_IMG_REVEAL_ID,local_ttxui_getIMGHandle(Teletext_Reveal_C_On) );
				else
					ONDK_GWM_COM_Image_Change(TTX_M_UI_IMG_REVEAL_ID,local_ttxui_getIMGHandle(Teletext_Reveal_N_On) );
			}
			else
			{
				if(push_status & GUIOBJ_HILIGHTED)
					ONDK_GWM_COM_Image_Change(TTX_M_UI_IMG_REVEAL_ID,local_ttxui_getIMGHandle(Teletext_Reveal_C_Off) );
				else
					ONDK_GWM_COM_Image_Change(TTX_M_UI_IMG_REVEAL_ID,local_ttxui_getIMGHandle(Teletext_Reveal_N_Off) );
			}
			break;

		case TTX_M_UI_BTN_DOUBLE_ID :
			hErr = Teletext_GetControl(eTTX_CONTROL_DOUBLE, &doubleSize);
			if(hErr!=ERR_OK)
		        HxLOG_Assert("Teletext_GetControl( eTTX_CONTROL_DOUBLE); \n");

			if ( doubleSize == eTTX_DOUBLE_OFF )
			{
				if(push_status & GUIOBJ_HILIGHTED)
					ONDK_GWM_COM_Image_Change(TTX_M_UI_IMG_DOUBLE_ID,local_ttxui_getIMGHandle(Teletext_DoubleSize_C_Off) );
				else
					ONDK_GWM_COM_Image_Change(TTX_M_UI_IMG_DOUBLE_ID,local_ttxui_getIMGHandle(Teletext_DoubleSize_N_Off) );
			}
			else
			{
				if(push_status & GUIOBJ_HILIGHTED)
					ONDK_GWM_COM_Image_Change(TTX_M_UI_IMG_DOUBLE_ID,local_ttxui_getIMGHandle(Teletext_DoubleSize_C_On) );
				else
					ONDK_GWM_COM_Image_Change(TTX_M_UI_IMG_DOUBLE_ID,local_ttxui_getIMGHandle(Teletext_DoubleSize_N_On) );
			}
			break;

		case TTX_M_UI_BTN_HOLD_ID :
			hErr = Teletext_GetControl(eTTX_CONTROL_HOLD, &bSet);
			if(hErr!=ERR_OK)
                HxLOG_Assert("Teletext_GetControl( eTTX_CONTROL_HOLD); \n");

			if ( bSet == TRUE )
			{
				if(push_status & GUIOBJ_HILIGHTED)
					ONDK_GWM_COM_Image_Change(TTX_M_UI_IMG_HOLD_ID,local_ttxui_getIMGHandle(Teletext_PageHold_C_On) );
				else
					ONDK_GWM_COM_Image_Change(TTX_M_UI_IMG_HOLD_ID,local_ttxui_getIMGHandle(Teletext_PageHold_N_On) );
			}
			else
			{
				if(push_status & GUIOBJ_HILIGHTED)
					ONDK_GWM_COM_Image_Change(TTX_M_UI_IMG_HOLD_ID,local_ttxui_getIMGHandle(Teletext_PageHold_C_Off) );
				else
					ONDK_GWM_COM_Image_Change(TTX_M_UI_IMG_HOLD_ID,local_ttxui_getIMGHandle(Teletext_PageHold_N_Off) );
			}
			break;
		default :
			break;
	}
	return ONDK_RET_OK;
}

ONDK_Result_t local_ttxui_MsgGwmProcessTimer(HINT32 p1)
{
	if ( p1 == GWMPOPUP_TIMER_TTXMENU )
	{
		ONDK_GWM_APP_Destroy (0);

		return GWM_MESSAGE_BREAK;
	}

	return ONDK_RET_OK;
}


ONDK_Result_t local_ttxui_MsgGwmWrapAround(HINT32 p1)
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


/**************************************************************************************************/
#define _________AP_TTX_MENU_Private_Process__________________________________________________________
/**************************************************************************************************/


void local_ttxui_GetTtxMenuUiPosition(eTtxMenuUiCompIds eUiId, ONDK_Rect_t *rect)
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


ONDK_Result_t local_ttxui_MsgGwmDlgDestroy(void)
{
	ONDK_GWM_KillTimer(GWMPOPUP_TIMER_TTXMENU);


	ONDK_GWM_COM_Image_Change(TTX_M_UI_IMG_INDEX_ID,NULL );
	ONDK_GWM_COM_Image_Change(TTX_M_UI_IMG_SUB_PAGE_ID,NULL );
	ONDK_GWM_COM_Image_Change(TTX_M_UI_IMG_MIX_ID,NULL );
	ONDK_GWM_COM_Image_Change(TTX_M_UI_IMG_CANCEL_ID,NULL );
	ONDK_GWM_COM_Image_Change(TTX_M_UI_IMG_REVEAL_ID,NULL );
	ONDK_GWM_COM_Image_Change(TTX_M_UI_IMG_DOUBLE_ID,NULL );
	ONDK_GWM_COM_Image_Change(TTX_M_UI_IMG_HOLD_ID,NULL);

	return ONDK_RET_OK;
}


ONDK_Result_t local_ttxui_MsgGwmClicked(HINT32 p1)
{
	HERROR				hErr;
	TTX_Mix_t			mix;
	TTX_DoubleSize_t	doubleSize;
    ONDK_Print("\n[%s:%d] p1= [%d]\n",__FUNCTION__,__LINE__,p1);

	switch (p1)
	{
		case TTX_M_UI_BTN_INDEX_ID :
			Teletext_SetControl( eTTX_CONTROL_INDEX, 0);
			ONDK_GWM_APP_Destroy (0);
			break;

		case TTX_M_UI_BTN_SUB_PAGE_ID :
			Teletext_SetControl(eTTX_CONTROL_SUBPAGE, 0);
			ONDK_GWM_APP_Destroy (0);
			break;

		case TTX_M_UI_BTN_MIX_ID :
			hErr = Teletext_GetControl(eTTX_CONTROL_MIX, &mix);
			if(hErr==ERR_OK)
		        HxLOG_Assert("Teletext_GetControl( TTX_M_UI_BTN_MIX_ID); \n");
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

			Teletext_SetControl(eTTX_CONTROL_MIX, mix);
			break;

		case TTX_M_UI_BTN_CANCEL_ID :
			Teletext_SetControl( eTTX_CONTROL_CANCEL, 0);
			break;

		case TTX_M_UI_BTN_REVEAL_ID :
			Teletext_SetControl( eTTX_CONTROL_REVEAL, 0);
			break;

		case TTX_M_UI_BTN_DOUBLE_ID :
			Teletext_GetControl( eTTX_CONTROL_DOUBLE, &doubleSize);
			if ( doubleSize == eTTX_DOUBLE_OFF )
			{
				Teletext_SetControl( eTTX_CONTROL_DOUBLE, eTTX_DOUBLE_ON /* eTTX_DOUBLE_FOCUS_ON_TOP */);
			}
			else
			{
				Teletext_SetControl(eTTX_CONTROL_DOUBLE, eTTX_DOUBLE_OFF);
			}
			break;

		case TTX_M_UI_BTN_HOLD_ID :
			Teletext_SetControl( eTTX_CONTROL_HOLD, 0);
			break;

		default :
			break;

	}

	return ONDK_MESSAGE_CONSUMED;
}

ONDK_Result_t local_ttxui_MsgGwmKeyDown(HINT32 p1)
{

	ONDK_GWM_ResetTimer (GWMPOPUP_TIMER_TTXMENU);
	//AP_KeyConv_UiCommon(&p1);
    ONDK_Print("[%s:%d] key = [0x%x]\n",__FUNCTION__,__LINE__,p1);
	switch (p1)
	{
		case KEY_STANDBY:
			ONDK_GWM_APP_Destroy (0);
			ONDK_GWM_APP_Destroy(AP_Teletext_Proc);
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

	return ONDK_RET_OK;
}


ONDK_Result_t local_ttxui_MsgGwmKeyUp(HINT32 p1)
{
	//AP_KeyConv_UiCommon(&p1);
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

	return ONDK_RET_OK;
}


/**************************************************************************************************/
#define _________AP_TTX_MENU_Public_Functions__________________________________________________________
/**************************************************************************************************/

ONDK_Result_t AP_TtxMenuUi_Proc (HINT32 message, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t			eResult = ONDK_RET_OK;
	ONDK_Print(HxANSI_COLOR_YELLOW("[%s:%d] Message(0x%x) : %s  p1(0x%x) p2(0x%x) p3(0x%x) \n"), __FUNCTION__, __LINE__, message, ONDK_GWM_GwmMsg2String(message), p1, p2, p3);

	switch (message)
	{
		/**********************************************************************
			GWM MESSAGES
		**********************************************************************/
		case MSG_GWM_CREATE :
			eResult = local_ttxui_MsgGwmCreate();
			break;

		case MSG_GWM_KEYDOWN :
			eResult = local_ttxui_MsgGwmKeyDown(p1);
			break;
		case MSG_GWM_KEYUP:
			eResult = local_ttxui_MsgGwmKeyUp(p1);
			break;
		case MSG_GWM_CLICKED :
			ONDK_GWM_ResetTimer(GWMPOPUP_TIMER_TTXMENU);
			eResult = local_ttxui_MsgGwmClicked(p1);
			break;
		case MSG_GWM_SCROLLLEFT :
		case MSG_GWM_SCROLLRIGHT :
			ONDK_GWM_ResetTimer(GWMPOPUP_TIMER_TTXMENU);
			eResult = local_ttxui_MsgGwmWrapAround(p1);
			break;
		case MSG_GWM_TIMER :
			eResult = local_ttxui_MsgGwmProcessTimer(p1);
			break;

		case MSG_GWM_DESTROY :
			eResult = local_ttxui_MsgGwmDlgDestroy();
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



void TTxMenuObject_Create(void)
{
	HINT32 index =0;

	TTxMenuObject_Macro(Teletext_Index_C_Off,VALUE_TO_STR(Teletext_Index_C_Off));
	TTxMenuObject_Macro(Teletext_Index_C_On,VALUE_TO_STR(Teletext_Index_C_On));
	TTxMenuObject_Macro(Teletext_Index_N_Off,VALUE_TO_STR(Teletext_Index_N_Off));

	TTxMenuObject_Macro(Teletext_Subpage_C_Off ,VALUE_TO_STR(Teletext_Subpage_C_Off));
    TTxMenuObject_Macro(Teletext_Subpage_C_On,VALUE_TO_STR(Teletext_Subpage_C_On));
    TTxMenuObject_Macro(Teletext_Subpage_N_Off,VALUE_TO_STR(Teletext_Subpage_N_Off));

	TTxMenuObject_Macro(Teletext_Mix_C_50,VALUE_TO_STR(Teletext_Mix_C_50));
	TTxMenuObject_Macro(Teletext_Mix_C_Off,VALUE_TO_STR(Teletext_Mix_C_Off));
	TTxMenuObject_Macro(Teletext_Mix_C_On ,VALUE_TO_STR(Teletext_Mix_C_On));
	TTxMenuObject_Macro(Teletext_Mix_N_50 ,VALUE_TO_STR(Teletext_Mix_N_50));
	TTxMenuObject_Macro(Teletext_Mix_N_Off,VALUE_TO_STR(Teletext_Mix_N_Off));
	TTxMenuObject_Macro(Teletext_Mix_N_On ,VALUE_TO_STR(Teletext_Mix_N_On));

	TTxMenuObject_Macro(Teletext_Cancel_C_Off,VALUE_TO_STR(Teletext_Cancel_C_Off));
	TTxMenuObject_Macro(Teletext_Cancel_C_On,VALUE_TO_STR(Teletext_Cancel_C_On));
	TTxMenuObject_Macro(Teletext_Cancel_N_Off,VALUE_TO_STR(Teletext_Cancel_N_Off));
	TTxMenuObject_Macro(Teletext_Cancel_N_On,VALUE_TO_STR(Teletext_Cancel_N_On));

	TTxMenuObject_Macro(Teletext_Reveal_C_Off,VALUE_TO_STR(Teletext_Reveal_C_Off));
	TTxMenuObject_Macro(Teletext_Reveal_C_On,VALUE_TO_STR(Teletext_Reveal_C_On));
	TTxMenuObject_Macro(Teletext_Reveal_N_Off,VALUE_TO_STR(Teletext_Reveal_N_Off));
	TTxMenuObject_Macro(Teletext_Reveal_N_On,VALUE_TO_STR(Teletext_Reveal_N_On));

	TTxMenuObject_Macro(Teletext_DoubleSize_C_Off,VALUE_TO_STR(Teletext_DoubleSize_C_Off));
	TTxMenuObject_Macro(Teletext_DoubleSize_C_On ,VALUE_TO_STR(Teletext_DoubleSize_C_On));
	TTxMenuObject_Macro(Teletext_DoubleSize_N_Off,VALUE_TO_STR(Teletext_DoubleSize_N_Off));
	TTxMenuObject_Macro(Teletext_DoubleSize_N_On,VALUE_TO_STR(Teletext_DoubleSize_N_On));

	TTxMenuObject_Macro(Teletext_PageHold_C_Off,VALUE_TO_STR(Teletext_PageHold_C_Off));
	TTxMenuObject_Macro(Teletext_PageHold_C_On,VALUE_TO_STR(Teletext_PageHold_C_On));
	TTxMenuObject_Macro(Teletext_PageHold_N_Off,VALUE_TO_STR(Teletext_PageHold_N_Off));
	TTxMenuObject_Macro(Teletext_PageHold_N_On,VALUE_TO_STR(Teletext_PageHold_N_On));
}




/**************************************************************************************************/
#define _________AP_TTX_MENU_End_Of_File____________________________________________________
/**************************************************************************************************/