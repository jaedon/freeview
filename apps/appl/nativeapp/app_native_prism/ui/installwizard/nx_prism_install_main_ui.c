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
 * @file	  		nx_prism_install_main_ui.c
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
#include <nx_prism_install_main_ui.h>
#include <nx_prism_install_sub_welcome.h>
#include <nx_prism_install_sub_language.h>
#include <nx_prism_install_sub_country.h>
#include <nx_prism_install_sub_network.h>
#include <nx_prism_install_sub_search.h>
#include <nx_prism_install_sub_result.h>
#include <nx_prism_install_sub_result_noui.h>

#include <nx_common.h>

#include <nx_prism_app.h>
#include <nx_prism_menu_contents.h>
#include <nx_prism_ui.h>
#include <nx_prism_component.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/
#define	NX_INSTALL_FRAME_OVERLAP_OBJECT_ID			    	    (NX_PRISM_INSTALL_ITEM_ID+1000)

#define	NX_PRISM_INSTALL_UI_OBJECT_ID						    ((NX_PRISM_INSTALL_ITEM_ID)+1)

#define	NX_PRISM_INSTALL_UI_FRAME_ID						    ((NX_PRISM_INSTALL_UI_OBJECT_ID)+100)
#define	NX_PRISM_INSTALL_UI_RECT_TITLE_LINE_ID				    ((NX_PRISM_INSTALL_UI_FRAME_ID)+1)
#define	NX_PRISM_INSTALL_UI_TEXT_EXPLAIN_ID	  				    (NX_PRISM_INSTALL_ITEM_ID+110)

#define	NX_INSTALL_STEP_INDICATOR_IMG_ID				        (NX_PRISM_INSTALL_ITEM_ID+160) // + N




//POPUP
#define	NX_PRISM_INSTALL_POPUP_PROCESSING_ID                    ((NX_PRISM_INSTALL_POPUP_ID))
#define	NX_PRISM_INSTALL_POPUP_PROCESSING_RECT_BG_ID            ((NX_PRISM_INSTALL_POPUP_PROCESSING_ID)+1)
#define NX_PRISM_INSTALL_POPUP_PROCESSING_RECT_LINE_ID          ((NX_PRISM_INSTALL_POPUP_PROCESSING_ID)+2)
#define NX_PRISM_INSTALL_POPUP_PROCESSING_TEXT_CONTEXT_ID       ((NX_PRISM_INSTALL_POPUP_PROCESSING_ID)+3)
#define	NX_PRISM_INSTALL_POPUP_PROCESSING_IMG_LOADING_ID        ((NX_PRISM_INSTALL_POPUP_PROCESSING_ID)+10)//1~4

#define	NX_PRISM_INSTALL_POPUP_SELECTIVE_RECT_BG_ID             ((NX_PRISM_INSTALL_POPUP_SELECTIVE_ID) + 1)
#define	NX_PRISM_INSTALL_POPUP_SELECTIVE_STROKE_ID              ((NX_PRISM_INSTALL_POPUP_SELECTIVE_ID) + 2)
#define	NX_PRISM_INSTALL_POPUP_SELECTIVE_STROKE_BOTTOM_ID       ((NX_PRISM_INSTALL_POPUP_SELECTIVE_ID) + 3)
#define	NX_PRISM_INSTALL_POPUP_SELECTIVE_TEXT_CONTEXT_ID        ((NX_PRISM_INSTALL_POPUP_SELECTIVE_ID) + 4)//+5

#define	NX_PRISM_INSTALL_POPUP_SELECTIVE_BTN_POSITIVE_ID        ((NX_PRISM_INSTALL_POPUP_SELECTIVE_ID) + 10)
#define	NX_PRISM_INSTALL_POPUP_SELECTIVE_BTN_NEGATIVE_ID        ((NX_PRISM_INSTALL_POPUP_SELECTIVE_ID) + 11)
#define	NX_PRISM_INSTALL_POPUP_SELECTIVE_BTN_CONFIRM_ID         ((NX_PRISM_INSTALL_POPUP_SELECTIVE_ID) + 12)

#define	NX_PRISM_INSTALL_POPUP_RESPONSE_RECT_BG_ID              ((NX_PRISM_INSTALL_POPUP_RESPONSE_ID) + 1)
#define	NX_PRISM_INSTALL_POPUP_RESPONSE_RECT_STROKE_ID          ((NX_PRISM_INSTALL_POPUP_RESPONSE_ID) + 2)
#define	NX_PRISM_INSTALL_POPUP_RESPONSE_TEXT_CONTEXT_ID         ((NX_PRISM_INSTALL_POPUP_RESPONSE_ID) + 3)

#define	NX_PRISM_INSTALL_WINDOW_X				                0
#define	NX_PRISM_INSTALL_WINDOW_Y			                	0

#define	NX_PRISM_INSTALL_WINDOW_WIDTH				    		NX_PRISM_OSD_HORIZONTAL
#define	NX_PRISM_INSTALL_WINDOW_HEIGHT					    	NX_PRISM_OSD_VERTICAL

#define	NX_INSTALL_STEP_INDICATOR_X                             1122
#define	NX_INSTALL_STEP_INDICATOR_Y                             47
#define	NX_INSTALL_STEP_INDICATOR_W                             94
#define	NX_INSTALL_STEP_INDICATOR_H                             28

#define	NX_INSTALL_STEP_INDICATOR_IMG_W                         26
#define	NX_INSTALL_STEP_INDICATOR_IMG_H                         28
#define	NX_INSTALL_STEP_INDICATOR_IMG_OVERLAP                   8

#define NX_PRISM_INSTALL_POPUP_PROCESSING_RECT_BG_X             290
#define NX_PRISM_INSTALL_POPUP_PROCESSING_RECT_BG_Y             0
#define NX_PRISM_INSTALL_POPUP_PROCESSING_RECT_BG_W             700
#define NX_PRISM_INSTALL_POPUP_PROCESSING_RECT_BG_H             101

#define NX_PRISM_INSTALL_POPUP_PROCESSING_RECT_LINE_W           700
#define NX_PRISM_INSTALL_POPUP_PROCESSING_RECT_LINE_H           1

#define NX_PRISM_INSTALL_POPUP_PROCESSING_IMG_LOADING_X         ((NX_PRISM_INSTALL_POPUP_PROCESSING_RECT_BG_X)+30)
#define NX_PRISM_INSTALL_POPUP_PROCESSING_IMG_LOADING_Y         ((NX_PRISM_INSTALL_POPUP_PROCESSING_RECT_BG_Y)+40)
#define NX_PRISM_INSTALL_POPUP_PROCESSING_IMG_LOADING_W         (46)
#define NX_PRISM_INSTALL_POPUP_PROCESSING_IMG_LOADING_H         (46)

#define NX_PRISM_INSTALL_POPUP_PROCESSING_TEXT_CONTEXT_X        ((NX_PRISM_INSTALL_POPUP_PROCESSING_RECT_BG_X)+106)
#define NX_PRISM_INSTALL_POPUP_PROCESSING_TEXT_CONTEXT_Y        ((NX_PRISM_INSTALL_POPUP_PROCESSING_RECT_BG_Y)+46)
#define NX_PRISM_INSTALL_POPUP_PROCESSING_TEXT_CONTEXT_W        (564)
#define NX_PRISM_INSTALL_POPUP_PROCESSING_TEXT_CONTEXT_H        (32)

#define NX_PRISM_INSTALL_UI_TEXT_EXPLAIN_X                      60
#define NX_PRISM_INSTALL_UI_TEXT_EXPLAIN_Y                      141
#define NX_PRISM_INSTALL_UI_TEXT_EXPLAIN_W                      1160
#define NX_PRISM_INSTALL_UI_TEXT_EXPLAIN_H                      32

#define	NX_PRISM_INSTALL_POPUP_SELECTIVE_X                      290
#define	NX_PRISM_INSTALL_POPUP_SELECTIVE_Y                      266
#define	NX_PRISM_INSTALL_POPUP_SELECTIVE_W                      700
#define	NX_PRISM_INSTALL_POPUP_SELECTIVE_H                      188
#define	NX_PRISM_INSTALL_POPUP_SELECTIVE_H_GAP                  32

#define	NX_PRISM_INSTALL_POPUP_SELECTIVE_STROKE_H               1

#define	NX_PRISM_INSTALL_POPUP_SELECTIVE_TEXT_X                 30
#define	NX_PRISM_INSTALL_POPUP_SELECTIVE_TEXT_Y                 46
#define	NX_PRISM_INSTALL_POPUP_SELECTIVE_TEXT_W                 640
#define	NX_PRISM_INSTALL_POPUP_SELECTIVE_TEXT_H                 32

#define	NX_PRISM_INSTALL_POPUP_SELECTIVE_BTN_CONFIRM_X          240
#define	NX_PRISM_INSTALL_POPUP_SELECTIVE_BTN_CONFIRM_Y          108
#define	NX_PRISM_INSTALL_POPUP_SELECTIVE_BTN_CONFIRM_W          220
#define	NX_PRISM_INSTALL_POPUP_SELECTIVE_BTN_CONFIRM_H          50

#define	NX_PRISM_INSTALL_POPUP_SELECTIVE_BTN_POSITIVE_X         358
#define	NX_PRISM_INSTALL_POPUP_SELECTIVE_BTN_POSITIVE_Y         76
#define	NX_PRISM_INSTALL_POPUP_SELECTIVE_BTN_POSITIVE_W         (NX_PRISM_INSTALL_POPUP_SELECTIVE_BTN_CONFIRM_W)
#define	NX_PRISM_INSTALL_POPUP_SELECTIVE_BTN_POSITIVE_H         (NX_PRISM_INSTALL_POPUP_SELECTIVE_BTN_CONFIRM_H)

#define	NX_PRISM_INSTALL_POPUP_SELECTIVE_BTN_NEGATIVE_X         123
#define	NX_PRISM_INSTALL_POPUP_SELECTIVE_BTN_NEGATIVE_Y         76
#define	NX_PRISM_INSTALL_POPUP_SELECTIVE_BTN_NEGATIVE_W         (NX_PRISM_INSTALL_POPUP_SELECTIVE_BTN_CONFIRM_W)
#define	NX_PRISM_INSTALL_POPUP_SELECTIVE_BTN_NEGATIVE_H         (NX_PRISM_INSTALL_POPUP_SELECTIVE_BTN_CONFIRM_H)

#define	NX_PRISM_INSTALL_POPUP_RESPONSE_RECT_BG_X               290
#define	NX_PRISM_INSTALL_POPUP_RESPONSE_RECT_BG_Y               0
#define	NX_PRISM_INSTALL_POPUP_RESPONSE_RECT_BG_W               700
#define	NX_PRISM_INSTALL_POPUP_RESPONSE_RECT_BG_H               101
#define	NX_PRISM_INSTALL_POPUP_RESPONSE_RECT_STROKE_H                1
#define	NX_PRISM_INSTALL_POPUP_RESPONSE_TEXT_CONTEXT_X          30
#define	NX_PRISM_INSTALL_POPUP_RESPONSE_TEXT_CONTEXT_Y          71
#define	NX_PRISM_INSTALL_POPUP_RESPONSE_TEXT_CONTEXT_W          640
#define	NX_PRISM_INSTALL_POPUP_RESPONSE_TEXT_CONTEXT_H          22

/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/

/******************************************************************************/
#define _________Private_Value________________________________________________
static HUINT8								s_strBuf[NX_TEXT_SIZE_128];

/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/

/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/

ONDK_Result_t	nx_install_ui_MsgGwmKeyDown(HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32	ulCustomCode = 1;

	switch (p1)
	{
			// custom key setting
		case KEY_CUSTOM_MULTI_CUSTOM:
			(void)NX_SYSTEM_GetCustomCode(&ulCustomCode);
			HxSTD_MemSet(s_strBuf, 0, NX_TEXT_SIZE_128);
			HxSTD_snprintf((char*)s_strBuf, NX_TEXT_SIZE_128, "%s %d", ONDK_GetString(RES_CUSTOMCODE_CHANGE), ulCustomCode);
			nx_install_ui_CreateResponseDialouge(s_strBuf, 24*60*60*1000); // wait forevere
			return ONDK_MESSAGE_BREAK;
		case KEY_CUSTOM_MULTI_CUSTOM_SUCCESS:
			(void)NX_SYSTEM_GetCustomCode(&ulCustomCode);
			HxSTD_MemSet(s_strBuf, 0, NX_TEXT_SIZE_128);
			HxSTD_snprintf((char*)s_strBuf, NX_TEXT_SIZE_128, "%s %d", ONDK_GetString(RES_CUSTOMCODE_CHANGE_SUCCESS), ulCustomCode);
			nx_install_ui_CreateResponseDialouge(s_strBuf, 0);
			return ONDK_MESSAGE_BREAK;
		case KEY_CUSTOM_MULTI_CUSTOM_FAIL:
			nx_install_ui_CreateResponseDialouge(ONDK_GetString(RES_CUSTOMCODE_CHANGE_FAILURE), 0);
			return ONDK_MESSAGE_BREAK;
        default:
            return ONDK_MESSAGE_PASS;
    }
}

ONDK_Result_t	nx_install_ui_ResponseDialogProc(HINT32 lMessage, Handle_t hHandle, HINT32 lParam1, HINT32 lParam2, HINT32 lParam3)
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

ONDK_Result_t	nx_install_ui_DestroyResponseDialouge(void)
{
	ONDK_GWM_App_t	pGwmApp = NULL;

	pGwmApp = ONDK_GWM_APP_Get((ONDK_GWM_Callback_t)nx_install_ui_ResponseDialogProc);
	if (pGwmApp != NULL)
	{
		return ONDK_GWM_APP_Destroy(nx_install_ui_ResponseDialogProc);
	}
	else
	{
		return	ONDK_RET_OK;
	}
}


void		nx_install_ui_CreateResponseDialouge(HUINT8* pszString, HINT32 nTimeout)
{
	if (pszString == NULL)
	{
		NX_APP_Error("Error!!! Pls check pszString was NULL!! \n");
		return;
	}

	nx_install_ui_DestroyResponseDialouge();

	ONDK_GWM_APP_Create (ONDK_UPPER_WINDOW, "nx_prism_MgrInit_ResponseDialogProc", APP_MULTI_CUSTOMCODE_PRIORITY,
						nx_install_ui_ResponseDialogProc, 0, (HINT32)pszString, 0, nTimeout);
}


//draw UI API
void	nx_install_ui_DrawTitle(HUINT8 *pTitle)
{
	ONDK_Rect_t		stRect = { 0, 0, 0, 0 };
	ONDK_Result_t	eResult = ONDK_RET_FAIL;
	HUINT32			ulID = (NX_PRISM_UI_SET_TITLE_ID);

	stRect = ONDK_Rect(NX_PRISM_UI_SET_TITLE_X, NX_PRISM_UI_SET_TITLE_Y, NX_PRISM_UI_SET_TITLE_WIDTH, NX_PRISM_UI_SET_TITLE_HEIGHT+2);

	eResult = ONDK_GWM_COM_Text_Create(ulID, &stRect, (HCHAR*)pTitle);
	if (eResult == ONDK_RET_OK)
	{
		ONDK_GWM_COM_Text_SetFont(ulID, eFont_SystemBold);
		ONDK_GWM_COM_Text_SetSize(ulID, NX_PRISM_FONT_SIZE_46);
		ONDK_GWM_COM_Text_SetAlign(ulID, TEXT_ALIGN_LEFT);
		ONDK_GWM_COM_Text_ChangeColor(ulID, COL(C_T_Gong01), COL(C_T_Gong01));
	}
	else if( (eResult == GWM_RET_LINK_CONFLICT) && (NULL != pTitle) )
	{
		//update Text
		ONDK_GWM_COM_Text_SetText(ulID, (HCHAR*)pTitle);
	}
}

void	nx_install_ui_DrawExplain(HCHAR *pText)
{
	ONDK_Rect_t		stRect = { 0, 0, 0, 0 };
	ONDK_Result_t	eResult = ONDK_RET_FAIL;
	HUINT32         ulID = 0;

	ulID = (NX_PRISM_INSTALL_UI_TEXT_EXPLAIN_ID);

	stRect = ONDK_Rect(NX_PRISM_INSTALL_UI_TEXT_EXPLAIN_X, NX_PRISM_INSTALL_UI_TEXT_EXPLAIN_Y,
						NX_PRISM_INSTALL_UI_TEXT_EXPLAIN_W, NX_PRISM_INSTALL_UI_TEXT_EXPLAIN_H);

	eResult = ONDK_GWM_COM_Text_Create(ulID, &stRect, pText);
	if (eResult == ONDK_RET_OK)
	{
		ONDK_GWM_COM_Text_SetFont(ulID, eFont_SystemNormal);
		ONDK_GWM_COM_Text_SetSize(ulID, NX_PRISM_FONT_SIZE_24);
		ONDK_GWM_COM_Text_SetAlign(ulID, TEXT_ALIGN_CENTER);
		ONDK_GWM_COM_Text_ChangeColor(ulID, COL(C_T_Gong04), COL(C_T_Gong01));
	}
	else if ((eResult == GWM_RET_LINK_CONFLICT) && (NULL != pText))
	{
		//update Text
		ONDK_GWM_COM_Text_SetText(ulID, pText);
	}
}

HBOOL   nx_install_ui_isAvailableIndicatorStep(NxInstallPage_t eStep) //available Indicator
{
    switch(eStep)
    {
        case    eNxInstallPage_Welcome:
        case    eNxInstallPage_Result_NoUI:
        case    eNxInstallPage_Result:
            return FALSE;
        default:
            return TRUE;
    }
}

ONDK_Result_t    nx_install_ui_DrawStepIndicatior(HUINT8 ulCurrentStep)
{
	ONDK_Rect_t		stRect = { 0, 0, 0, 0 };
	HUINT8          ulIndicatorUIIndex = 0, ulIndex = 0;
	HUINT32         x,y, rightAlignX;
	HUINT32         ulImgID = 0;
	HUINT8*         imgPath = NULL;
    ONDK_Result_t	eResult = ONDK_RET_OK;

    Nx_InstallSubPage_t	*pstSubItemArray = NULL;
    HUINT8 zNumOfSubPage, zNumOfAvailableStep = 0;
    Nx_InstallSubPage_t	*pstCurrentPageItem = NULL;

    eResult = nx_install_ui_GetListOfSubPage(&pstSubItemArray, &zNumOfSubPage);

	if(eResult != ONDK_RET_OK)
	{
		return eResult;
	}

    pstCurrentPageItem = &pstSubItemArray[ulCurrentStep];

    for(ulIndex = 0; ulIndex<zNumOfSubPage;ulIndex++)
    {
        if(nx_install_ui_isAvailableIndicatorStep(pstSubItemArray[ulIndex].ePageName))
        {
            zNumOfAvailableStep++;
        }
    }

	rightAlignX = (NX_INSTALL_STEP_INDICATOR_X) + ((NX_INSTALL_STEP_INDICATOR_W)-(NX_INSTALL_STEP_INDICATOR_IMG_W)) + (NX_INSTALL_STEP_INDICATOR_IMG_OVERLAP);

	for (ulIndex = 0; ulIndex < zNumOfSubPage; ulIndex++)
	{
	    if(nx_install_ui_isAvailableIndicatorStep(pstSubItemArray[ulIndex].ePageName))
        {
    		x = rightAlignX - ( (NX_INSTALL_STEP_INDICATOR_IMG_W) * (zNumOfAvailableStep - (ulIndicatorUIIndex+1)))
    		+ ( (NX_INSTALL_STEP_INDICATOR_IMG_OVERLAP) * (zNumOfAvailableStep- (ulIndicatorUIIndex+1)));

    		y = (NX_INSTALL_STEP_INDICATOR_Y);
    		ulImgID = (NX_INSTALL_STEP_INDICATOR_IMG_ID)+ulIndicatorUIIndex;
    		if (ulIndicatorUIIndex == 0)
    		{
    			//first
                if(pstCurrentPageItem->ePageName == pstSubItemArray[ulIndex].ePageName)
    			{
    				imgPath = (HUINT8*)RES_320_00_WIZ_STEP01_ON_PNG;
    			}
    			else
    			{
    				imgPath = (HUINT8*)RES_320_00_WIZ_STEP01_OFF_PNG;
    			}
    		}else
    		{
                if(pstCurrentPageItem->ePageName == pstSubItemArray[ulIndex].ePageName)
    			{
    				imgPath = (HUINT8*)RES_320_00_WIZ_STEP02_ON_PNG;
    			}
    			else
    			{
    				imgPath = (HUINT8*)RES_320_00_WIZ_STEP02_OFF_PNG;
    			}
    		}

    		stRect = ONDK_Rect(x, y, NX_INSTALL_STEP_INDICATOR_IMG_W, NX_INSTALL_STEP_INDICATOR_IMG_H);
    		ONDK_GWM_COM_Image_Create(ulImgID, &stRect);
    		ONDK_GWM_COM_Image_SetImage(ulImgID, (HCHAR*)imgPath);
            ulIndicatorUIIndex++;
        }
	}

	if(pstSubItemArray)
	{
		NX_APP_Free(pstSubItemArray);
		pstSubItemArray = NULL;
	}

	return	ONDK_RET_OK;
}

void    nx_install_ui_SetDimBackgroundUI(HBOOL bDim)
{
   	ONDK_Rect_t		stRect = { 0, 0, 0, 0 };
   	stRect = ONDK_Rect(NX_PRISM_UI_BACKGROUND_BOX_X, NX_PRISM_UI_BACKGROUND_BOX_Y, NX_PRISM_UI_BACKGROUND_BOX_WIDTH, NX_PRISM_UI_BACKGROUND_BOX_HEIGHT);
   	ONDK_GWM_COM_Rect_Create(NX_INSTALL_FRAME_OVERLAP_OBJECT_ID, &stRect, COL(C_G_Bang_85));
    ONDK_GWM_Obj_SetVisible(NX_INSTALL_FRAME_OVERLAP_OBJECT_ID, bDim);
}

ONDK_Result_t    nx_install_ui_DrawBackgroundUI(void)
{
	ONDK_Rect_t		stRect = { 0, 0, 0, 0 };

	//set Frame
	ONDK_GWM_APP_SetAppArea(NX_PRISM_INSTALL_WINDOW_X, NX_PRISM_INSTALL_WINDOW_Y, NX_PRISM_INSTALL_WINDOW_WIDTH, NX_PRISM_INSTALL_WINDOW_HEIGHT);

	// Draw - Background Frame
	stRect = ONDK_Rect(NX_PRISM_UI_BACKGROUND_BOX_X, NX_PRISM_UI_BACKGROUND_BOX_Y, NX_PRISM_UI_BACKGROUND_BOX_WIDTH, NX_PRISM_UI_BACKGROUND_BOX_HEIGHT);
	ONDK_GWM_COM_Rect_Create(NX_PRISM_INSTALL_UI_FRAME_ID, &stRect, COL(C_G_Bang_100));

	stRect = ONDK_Rect(NX_PRISM_UI_SET_TITLE_LINE_X, NX_PRISM_UI_SET_TITLE_LINE_Y, NX_PRISM_UI_SET_TITLE_LINE_WIDTH, NX_PRISM_UI_SET_TITLE_LINE_HEIGHT);
	ONDK_GWM_COM_Rect_Create(NX_PRISM_INSTALL_UI_RECT_TITLE_LINE_ID, &stRect, COL(C_G_Whistle_30));

	return	ONDK_RET_OK;
}

ONDK_Result_t	nx_install_ui_drawBasicPopupUI(HBOOL bShow, HCHAR** strContextList, HINT32 lineNum)
{
    ONDK_Result_t	eResult = ONDK_RET_OK;
    HUINT32         ulID = 0;
   	ONDK_Rect_t		stRect = { 0, 0, 0, 0 };
    HINT32          lIndex = 0;

    //BG
    ulID = (NX_PRISM_INSTALL_POPUP_SELECTIVE_RECT_BG_ID);
   	stRect = ONDK_Rect(NX_PRISM_INSTALL_POPUP_SELECTIVE_X,
        NX_PRISM_INSTALL_POPUP_SELECTIVE_Y,
        NX_PRISM_INSTALL_POPUP_SELECTIVE_W,
        NX_PRISM_INSTALL_POPUP_SELECTIVE_H+(NX_PRISM_INSTALL_POPUP_SELECTIVE_H_GAP*lineNum));
   	eResult = ONDK_GWM_COM_Rect_Create(ulID, &stRect, COL(C_G_Bang_90));
    ONDK_GWM_Obj_SetVisible(ulID, bShow);
    ONDK_GWM_Obj_DisableObject(ulID);

    //STROKE
    ulID = (NX_PRISM_INSTALL_POPUP_SELECTIVE_STROKE_ID);
   	stRect = ONDK_Rect(NX_PRISM_INSTALL_POPUP_SELECTIVE_X,
        NX_PRISM_INSTALL_POPUP_SELECTIVE_Y,
        NX_PRISM_INSTALL_POPUP_SELECTIVE_W,
        NX_PRISM_INSTALL_POPUP_SELECTIVE_STROKE_H);
    ONDK_GWM_COM_Rect_Create(ulID, &stRect, COL(C_G_Whistle_100));
    ONDK_GWM_Obj_SetVisible(ulID, bShow);
    ONDK_GWM_Obj_DisableObject(ulID);

    ulID = (NX_PRISM_INSTALL_POPUP_SELECTIVE_STROKE_BOTTOM_ID);
   	stRect = ONDK_Rect(NX_PRISM_INSTALL_POPUP_SELECTIVE_X,
        NX_PRISM_INSTALL_POPUP_SELECTIVE_Y
        +(NX_PRISM_INSTALL_POPUP_SELECTIVE_H+(NX_PRISM_INSTALL_POPUP_SELECTIVE_H_GAP*lineNum))
        -(NX_PRISM_INSTALL_POPUP_PROCESSING_RECT_LINE_H),
        NX_PRISM_INSTALL_POPUP_SELECTIVE_W,
        NX_PRISM_INSTALL_POPUP_SELECTIVE_STROKE_H);
    ONDK_GWM_COM_Rect_Create(ulID, &stRect, COL(C_G_Whistle_100));
    ONDK_GWM_Obj_SetVisible(ulID, bShow);
    ONDK_GWM_Obj_DisableObject(ulID);

    //TEXT
    for(lIndex = 0;lIndex<lineNum;lIndex++)
    {
        HINT32 x,y=0;
        x = (NX_PRISM_INSTALL_POPUP_SELECTIVE_X + NX_PRISM_INSTALL_POPUP_SELECTIVE_TEXT_X);
        y = (NX_PRISM_INSTALL_POPUP_SELECTIVE_Y + NX_PRISM_INSTALL_POPUP_SELECTIVE_TEXT_Y)
            +(NX_PRISM_INSTALL_POPUP_SELECTIVE_TEXT_H*lIndex);

        ulID = (NX_PRISM_INSTALL_POPUP_SELECTIVE_TEXT_CONTEXT_ID)+lIndex;
   	    stRect = ONDK_Rect(x,y,NX_PRISM_INSTALL_POPUP_SELECTIVE_TEXT_W,NX_PRISM_INSTALL_POPUP_SELECTIVE_TEXT_H);
    	eResult = ONDK_GWM_COM_Text_Create(ulID, &stRect, strContextList[lIndex]);
        ONDK_GWM_COM_Text_SetFont(ulID, eFont_SystemNormal);
        ONDK_GWM_COM_Text_SetSize(ulID, NX_PRISM_FONT_SIZE_22);
        ONDK_GWM_COM_Text_SetAlign(ulID,TEXT_ALIGN_CENTER|TEXT_ALIGN_MIDDLE);
        ONDK_GWM_COM_Text_ChangeColor(ulID, COL(C_T_Whistle_100), COL(C_T_Whistle_100));
        ONDK_GWM_Obj_DisableObject(ulID);
    }
    return eResult;
}

ONDK_Result_t	nx_install_ui_OpenResponsePopupUI(HCHAR* strContext)
{
    ONDK_Result_t	eResult = ONDK_RET_OK;
    HUINT32         ulID = 0;
   	ONDK_Rect_t		stRect = { 0, 0, 0, 0 };
    HUINT32         x,y = 0;
    //BG
    ulID = (NX_PRISM_INSTALL_POPUP_RESPONSE_RECT_BG_ID);
   	stRect = ONDK_Rect(NX_PRISM_INSTALL_POPUP_RESPONSE_RECT_BG_X,
        NX_PRISM_INSTALL_POPUP_RESPONSE_RECT_BG_Y,
        NX_PRISM_INSTALL_POPUP_RESPONSE_RECT_BG_W,
        NX_PRISM_INSTALL_POPUP_RESPONSE_RECT_BG_H);
   	eResult = ONDK_GWM_COM_Rect_Create(ulID, &stRect, COL(C_G_Bang_90));
    ONDK_GWM_Obj_SetVisible(ulID, TRUE);
    ONDK_GWM_Obj_DisableObject(ulID);

    //STROKE
    ulID = (NX_PRISM_INSTALL_POPUP_RESPONSE_RECT_STROKE_ID);

   	stRect = ONDK_Rect(NX_PRISM_INSTALL_POPUP_RESPONSE_RECT_BG_X,
        (NX_PRISM_INSTALL_POPUP_RESPONSE_RECT_BG_Y+NX_PRISM_INSTALL_POPUP_RESPONSE_RECT_BG_H)
        - (NX_PRISM_INSTALL_POPUP_RESPONSE_RECT_STROKE_H),
        NX_PRISM_INSTALL_POPUP_RESPONSE_RECT_BG_W,
        (NX_PRISM_INSTALL_POPUP_RESPONSE_RECT_STROKE_H));
    ONDK_GWM_COM_Rect_Create(ulID, &stRect, COL(C_T_Yoyo01));
    ONDK_GWM_Obj_SetVisible(ulID, TRUE);
    ONDK_GWM_Obj_DisableObject(ulID);

    //TEXT
    ulID = (NX_PRISM_INSTALL_POPUP_RESPONSE_TEXT_CONTEXT_ID);

    x = NX_PRISM_INSTALL_POPUP_RESPONSE_RECT_BG_X + NX_PRISM_INSTALL_POPUP_RESPONSE_TEXT_CONTEXT_X;
    y = NX_PRISM_INSTALL_POPUP_RESPONSE_RECT_BG_Y + (NX_PRISM_INSTALL_POPUP_RESPONSE_TEXT_CONTEXT_Y-(NX_PRISM_INSTALL_POPUP_RESPONSE_TEXT_CONTEXT_H));

	stRect = ONDK_Rect(x,
        y,
        NX_PRISM_INSTALL_POPUP_RESPONSE_TEXT_CONTEXT_W,
        NX_PRISM_INSTALL_POPUP_RESPONSE_TEXT_CONTEXT_H);

    eResult = ONDK_GWM_COM_Text_Create(ulID, &stRect, strContext);
    ONDK_GWM_COM_Text_SetFont(ulID, eFont_SystemBold);
    ONDK_GWM_COM_Text_SetSize(ulID, NX_PRISM_FONT_SIZE_22);
    ONDK_GWM_COM_Text_SetAlign(ulID,TEXT_ALIGN_CENTER|TEXT_ALIGN_MIDDLE);
    ONDK_GWM_COM_Text_ChangeColor(ulID, COL(C_T_Yoyo01), COL(C_T_Yoyo01));
    ONDK_GWM_Obj_DisableObject(ulID);

    return eResult;
}

ONDK_Result_t	nx_install_ui_OpenConfirmPopupUI(HCHAR** strContextList, HINT32 lineNum)
{
    ONDK_Result_t	eResult = ONDK_RET_FAIL;
    HUINT32         ulId = 0;
   	ONDK_Rect_t		stRect = { 0, 0, 0, 0 };
    HINT32          y = 0;

    if(lineNum <= 0 || lineNum >5)
    {
        return ONDK_RET_FAIL;
    }
    nx_install_ui_SetDimBackgroundUI(TRUE);
    eResult = nx_install_ui_drawBasicPopupUI(TRUE,strContextList,lineNum);
    //BTN

    y =  (NX_PRISM_INSTALL_POPUP_SELECTIVE_Y + NX_PRISM_INSTALL_POPUP_SELECTIVE_BTN_CONFIRM_Y)
          +(NX_PRISM_INSTALL_POPUP_SELECTIVE_TEXT_H*(lineNum-1));

    ulId = (NX_PRISM_INSTALL_POPUP_SELECTIVE_BTN_CONFIRM_ID);
    stRect = ONDK_Rect(NX_PRISM_INSTALL_POPUP_SELECTIVE_X + (NX_PRISM_INSTALL_POPUP_SELECTIVE_BTN_CONFIRM_X),y,
        NX_PRISM_INSTALL_POPUP_SELECTIVE_BTN_CONFIRM_W,
        NX_PRISM_INSTALL_POPUP_SELECTIVE_BTN_CONFIRM_H);
    ONDK_GWM_COM_Button_Create (ulId, &stRect,(char*)ONDK_GetString(RES_OK_ID));

    ONDK_GWM_COM_Button_SetFontColor(ulId, COL(C_G_Whistle_100), COL(C_G_Bang_100));
    ONDK_GWM_COM_Button_SetFont(ulId, eFont_SystemBold);
    ONDK_GWM_COM_Button_SetSimpleButton(ulId,TRUE);
    ONDK_GWM_COM_Button_SetButtonColor(ulId,  COL(C_G_Base01_100), COL(C_G_Gong02), COL(C_Table_MAX), COL(C_G_Gong02));
    ONDK_GWM_COM_Button_SetButtonDimmedAlpha(ulId,100);

 	ONDK_GWM_COM_Button_SetTitleSize(ulId,NX_PRISM_FONT_SIZE_24);
    ONDK_GWM_COM_Button_SetAlign(ulId,ONDKSTF_CENTER);
    ONDK_GWM_Obj_SetVisible(ulId, TRUE);
    ONDK_GWM_COM_Button_SetStatus(ulId,GUIOBJ_HILIGHTED);
    ONDK_GWM_Obj_SetFocus(ulId);

    return eResult;
}

ONDK_Result_t	nx_install_ui_CloseResponsePopupUI(void)
{
    ONDK_Result_t	eResult = ONDK_RET_FAIL;
    ONDK_GWM_Obj_Destroy(NX_PRISM_INSTALL_POPUP_RESPONSE_RECT_BG_ID);
    ONDK_GWM_Obj_Destroy(NX_PRISM_INSTALL_POPUP_RESPONSE_RECT_STROKE_ID);
    ONDK_GWM_Obj_Destroy(NX_PRISM_INSTALL_POPUP_RESPONSE_TEXT_CONTEXT_ID);
    return eResult;
}

ONDK_Result_t	nx_install_ui_CloseConfirmPopupUI(void)
{
    ONDK_Result_t	eResult = ONDK_RET_FAIL;
    HUINT32         ulIndex = 0;
    ONDK_GWM_Obj_Destroy(NX_PRISM_INSTALL_POPUP_SELECTIVE_RECT_BG_ID);
    ONDK_GWM_Obj_Destroy(NX_PRISM_INSTALL_POPUP_SELECTIVE_STROKE_ID);
    ONDK_GWM_Obj_Destroy(NX_PRISM_INSTALL_POPUP_SELECTIVE_STROKE_BOTTOM_ID);

    for(ulIndex=0;ulIndex<5;ulIndex++)
    {
        ONDK_GWM_Obj_Destroy(NX_PRISM_INSTALL_POPUP_SELECTIVE_TEXT_CONTEXT_ID + ulIndex);
    }

    ONDK_GWM_Obj_Destroy(NX_PRISM_INSTALL_POPUP_SELECTIVE_BTN_CONFIRM_ID);
    nx_install_ui_SetDimBackgroundUI(FALSE);
    return eResult;
}

ONDK_Result_t	nx_install_ui_CloseSelectivePopupUI(void)
{
    ONDK_Result_t	eResult = ONDK_RET_FAIL;
    HUINT32         ulIndex = 0;
    ONDK_GWM_Obj_Destroy(NX_PRISM_INSTALL_POPUP_SELECTIVE_RECT_BG_ID);
    ONDK_GWM_Obj_Destroy(NX_PRISM_INSTALL_POPUP_SELECTIVE_STROKE_ID);
    ONDK_GWM_Obj_Destroy(NX_PRISM_INSTALL_POPUP_SELECTIVE_STROKE_BOTTOM_ID);

    for(ulIndex=0;ulIndex<5;ulIndex++)
    {
        ONDK_GWM_Obj_Destroy(NX_PRISM_INSTALL_POPUP_SELECTIVE_TEXT_CONTEXT_ID + ulIndex);
    }
    ONDK_GWM_Obj_Destroy(NX_PRISM_INSTALL_POPUP_SELECTIVE_BTN_POSITIVE_ID);
    ONDK_GWM_Obj_Destroy(NX_PRISM_INSTALL_POPUP_SELECTIVE_BTN_NEGATIVE_ID);
    nx_install_ui_SetDimBackgroundUI(FALSE);
    return eResult;
}

ONDK_Result_t	nx_install_ui_OpenSelectivePopupUI(HCHAR** strContextList, HINT32 lineNum,HCHAR* strPositive, HCHAR* strNegative)
{
    ONDK_Result_t	eResult = ONDK_RET_FAIL;
    HUINT32         ulId = 0;
   	ONDK_Rect_t		stRect = { 0, 0, 0, 0 };
    HINT32          y = 0;
    if(lineNum <= 0 || lineNum >5)
    {
        return ONDK_RET_FAIL;
    }
    nx_install_ui_SetDimBackgroundUI(TRUE);
    eResult = nx_install_ui_drawBasicPopupUI(TRUE,strContextList,lineNum);

    y =  (NX_PRISM_INSTALL_POPUP_SELECTIVE_Y + NX_PRISM_INSTALL_POPUP_SELECTIVE_BTN_POSITIVE_Y)
          +(NX_PRISM_INSTALL_POPUP_SELECTIVE_TEXT_H*(lineNum));

    //BTN
    ulId = (NX_PRISM_INSTALL_POPUP_SELECTIVE_BTN_POSITIVE_ID);
    stRect = ONDK_Rect((NX_PRISM_INSTALL_POPUP_SELECTIVE_X + NX_PRISM_INSTALL_POPUP_SELECTIVE_BTN_POSITIVE_X),
        y,
        NX_PRISM_INSTALL_POPUP_SELECTIVE_BTN_POSITIVE_W,
        NX_PRISM_INSTALL_POPUP_SELECTIVE_BTN_POSITIVE_H);
    ONDK_GWM_COM_Button_Create (ulId, &stRect, strPositive);

    ONDK_GWM_COM_Button_SetFontColor(ulId, COL(C_G_Whistle_100), COL(C_G_Bang_100));
    ONDK_GWM_COM_Button_SetFont(ulId, eFont_SystemBold);
    ONDK_GWM_COM_Button_SetSimpleButton(ulId,TRUE);
    ONDK_GWM_COM_Button_SetButtonColor(ulId,  COL(C_G_Base01_100), COL(C_G_Gong02), COL(C_Table_MAX), COL(C_G_Gong02));
    ONDK_GWM_COM_Button_SetButtonDimmedAlpha(ulId,100);

 	ONDK_GWM_COM_Button_SetTitleSize(ulId,NX_PRISM_FONT_SIZE_24);
    ONDK_GWM_COM_Button_SetAlign(ulId,ONDKSTF_CENTER);
    ONDK_GWM_Obj_SetVisible(ulId, TRUE);
    ONDK_GWM_COM_Button_SetStatus(ulId,GUIOBJ_HILIGHTED);
    ONDK_GWM_Obj_SetFocus(ulId);

    ulId = (NX_PRISM_INSTALL_POPUP_SELECTIVE_BTN_NEGATIVE_ID);

    y =  (NX_PRISM_INSTALL_POPUP_SELECTIVE_Y + NX_PRISM_INSTALL_POPUP_SELECTIVE_BTN_NEGATIVE_Y)
          +(NX_PRISM_INSTALL_POPUP_SELECTIVE_TEXT_H*(lineNum));

    stRect = ONDK_Rect((NX_PRISM_INSTALL_POPUP_SELECTIVE_X + NX_PRISM_INSTALL_POPUP_SELECTIVE_BTN_NEGATIVE_X),
        y,
        NX_PRISM_INSTALL_POPUP_SELECTIVE_BTN_NEGATIVE_W,
        NX_PRISM_INSTALL_POPUP_SELECTIVE_BTN_NEGATIVE_H);

    ONDK_GWM_COM_Button_Create (ulId, &stRect, strNegative);

    ONDK_GWM_COM_Button_SetFontColor(ulId, COL(C_G_Whistle_100), COL(C_G_Bang_100));
    ONDK_GWM_COM_Button_SetFont(ulId, eFont_SystemBold);
    ONDK_GWM_COM_Button_SetSimpleButton(ulId,TRUE);
    ONDK_GWM_COM_Button_SetButtonColor(ulId,  COL(C_G_Base01_100), COL(C_G_Gong02), COL(C_Table_MAX), COL(C_G_Gong02));
    ONDK_GWM_COM_Button_SetButtonDimmedAlpha(ulId,100);

 	ONDK_GWM_COM_Button_SetTitleSize(ulId,NX_PRISM_FONT_SIZE_24);
    ONDK_GWM_COM_Button_SetAlign(ulId,ONDKSTF_CENTER);
    ONDK_GWM_Obj_SetVisible(ulId, TRUE);
    ONDK_GWM_COM_Button_SetStatus(ulId,GUIOBJ_NORMAL);

    return eResult;
}

ONDK_Result_t	nx_install_ui_SetProcessingPopupUI(HBOOL bShow, HCHAR* strContext)
{
    ONDK_Result_t	eResult = ONDK_RET_FAIL;
    HUINT32         ulID = 0;
   	ONDK_Rect_t		stRect = { 0, 0, 0, 0 };
    HUINT32         ulIndex = 0;
    HCHAR*          pszResource = NULL;

    //BG
    ulID = (NX_PRISM_INSTALL_POPUP_PROCESSING_RECT_BG_ID);
   	stRect = ONDK_Rect(NX_PRISM_INSTALL_POPUP_PROCESSING_RECT_BG_X,
        NX_PRISM_INSTALL_POPUP_PROCESSING_RECT_BG_Y,
        NX_PRISM_INSTALL_POPUP_PROCESSING_RECT_BG_W,
        NX_PRISM_INSTALL_POPUP_PROCESSING_RECT_BG_H);
   	eResult = ONDK_GWM_COM_Rect_Create(ulID, &stRect, COL(C_G_Bang_90));
    ONDK_GWM_Obj_SetVisible(ulID, bShow);

    //BOTTOM Stroke
    ulID = (NX_PRISM_INSTALL_POPUP_PROCESSING_RECT_LINE_ID);
   	stRect = ONDK_Rect(NX_PRISM_INSTALL_POPUP_PROCESSING_RECT_BG_X,
        (NX_PRISM_INSTALL_POPUP_PROCESSING_RECT_BG_Y)+(NX_PRISM_INSTALL_POPUP_PROCESSING_RECT_BG_H),
        NX_PRISM_INSTALL_POPUP_PROCESSING_RECT_LINE_W,
        NX_PRISM_INSTALL_POPUP_PROCESSING_RECT_LINE_H);
    ONDK_GWM_COM_Rect_Create(ulID, &stRect, COL(C_G_Whistle_100));
    ONDK_GWM_Obj_SetVisible(ulID, bShow);

    //IMAGE
    stRect = ONDK_Rect(NX_PRISM_INSTALL_POPUP_PROCESSING_IMG_LOADING_X,
        (NX_PRISM_INSTALL_POPUP_PROCESSING_IMG_LOADING_Y),
        NX_PRISM_INSTALL_POPUP_PROCESSING_IMG_LOADING_W,
        NX_PRISM_INSTALL_POPUP_PROCESSING_IMG_LOADING_H);

    for(ulIndex =0 ;ulIndex<4;ulIndex++)
    {
        ulID = (NX_PRISM_INSTALL_POPUP_PROCESSING_IMG_LOADING_ID)+ulIndex;
        switch(ulIndex){
            case 0:		pszResource = RES_206_00_LOADING_01_PNG;	break;
    		case 1:		pszResource = RES_206_00_LOADING_02_PNG;	break;
	    	case 2:		pszResource = RES_206_00_LOADING_03_PNG;	break;
		    case 3:		pszResource = RES_206_00_LOADING_04_PNG;	break;
        }

        ONDK_GWM_COM_Image_Create(ulID, &stRect);
        ONDK_GWM_COM_Image_SetImage(ulID, pszResource);
        if(TRUE==bShow && 0 == ulIndex){
            ONDK_GWM_Obj_SetVisible(ulID, bShow);
        }else{
            ONDK_GWM_Obj_SetVisible(ulID, FALSE);
        }
    }

    //CONTEXT

    ulID = (NX_PRISM_INSTALL_POPUP_PROCESSING_TEXT_CONTEXT_ID);
   	stRect = ONDK_Rect(NX_PRISM_INSTALL_POPUP_PROCESSING_TEXT_CONTEXT_X,
        NX_PRISM_INSTALL_POPUP_PROCESSING_TEXT_CONTEXT_Y,
        NX_PRISM_INSTALL_POPUP_PROCESSING_TEXT_CONTEXT_W,
        NX_PRISM_INSTALL_POPUP_PROCESSING_TEXT_CONTEXT_H);

	eResult = ONDK_GWM_COM_Text_Create(ulID, &stRect, strContext);

    if(ONDK_RET_OK != eResult && (NULL != strContext))
    {
        ONDK_GWM_COM_Text_SetText(ulID, strContext);
    }else
    {
        ONDK_GWM_COM_Text_SetFont(ulID, eFont_SystemBold);
        ONDK_GWM_COM_Text_SetSize(ulID, NX_PRISM_FONT_SIZE_22);
        ONDK_GWM_COM_Text_SetAlign(ulID,TEXT_ALIGN_LEFT|TEXT_ALIGN_MIDDLE);
        ONDK_GWM_COM_Text_ChangeColor(ulID, COL(C_T_Whistle_100), COL(C_T_Whistle_100));
    }

    ONDK_GWM_Obj_SetVisible(ulID, bShow);
    return eResult;
}

ONDK_Result_t	nx_install_ui_UpdateProcessingPopupUI(HINT32 index)
{
    ONDK_Result_t	eResult = ONDK_RET_FAIL;
    HUINT32         ulID = 0;
    HINT32          ulIndex = 0;

    for(ulIndex =0 ;ulIndex<4;ulIndex++)
    {
        ulID = (NX_PRISM_INSTALL_POPUP_PROCESSING_IMG_LOADING_ID)+ulIndex;
        if(index == ulIndex){
            ONDK_GWM_Obj_SetVisible(ulID, TRUE);
        }else{
            ONDK_GWM_Obj_SetVisible(ulID, FALSE);
        }
    }

    return eResult;
}

ONDK_Result_t  nx_install_ui_GetListOfSubPage(Nx_InstallSubPage_t	**pstRetSubItemArray, HUINT8 *ulNumOfPage)
{
    NxInstallPage_t *eNxInstallPages = NULL;
    HUINT8 index = 0;
    HUINT32 ulNumOfSubPage = 0;

    NxInstallPage_t eNxPages = 0;
    Nx_InstallSubPage_t	*pstSubItemArray = NULL;
    Nx_InstallSubPage_t	*pstSubItem = NULL;


    if (NULL!=*pstRetSubItemArray)
	{
		NX_APP_Error("Error!!! (NULL!=*pstRetSubItemArray) \n");
        return GWM_RET_OUT_OF_MEMORY;
    }

    nx_install_model_GetPageInfo(&eNxInstallPages,(HINT32*)&ulNumOfSubPage);
	pstSubItemArray = (Nx_InstallSubPage_t *)NX_APP_Malloc (sizeof(Nx_InstallSubPage_t) * ulNumOfSubPage);
	if (pstSubItemArray == NULL)
	{
		NX_APP_Error("Error!!! NX_APP_Malloc() Fail!!  \n");
	    if (NULL != eNxInstallPages)
	    {
	        NX_APP_Free(eNxInstallPages);
			eNxInstallPages = NULL;
	    }
		return GWM_RET_OUT_OF_MEMORY;
	}

	HxSTD_MemSet (pstSubItemArray, 0, sizeof(Nx_InstallSubPage_t) * ulNumOfSubPage);

    for(index=0; index<ulNumOfSubPage;index++){
        eNxPages = eNxInstallPages[index];
        pstSubItem = &(pstSubItemArray[index]);
        switch(eNxPages){
            case    eNxInstallPage_Welcome:
                HxSTD_StrNCpy((char*)(pstSubItem->ucTitle), (char*)ONDK_GetString(RES_INSTALLATION_WIZARD_01_ID), MAX_ELEMENT_TEXT_LENGTH);
                HxSTD_StrNCpy((char*)(pstSubItem->ucSubTitle), (char*)ONDK_GetString(RES_INSTALLATION_WIZARD_01_ID), MAX_ELEMENT_TEXT_LENGTH);
                pstSubItem->procCallback = NX_PRISM_Install_Welcome_Proc;
                break;
            case    eNxInstallPage_Language:
                HxSTD_StrNCpy((char*)(pstSubItem->ucTitle), (char*)ONDK_GetString(RES_LANGUAGE_01_ID ), MAX_ELEMENT_TEXT_LENGTH);
                HxSTD_StrNCpy((char*)(pstSubItem->ucSubTitle), (char*)ONDK_GetString(RES_SELECTION_ID ), MAX_ELEMENT_TEXT_LENGTH);
                pstSubItem->procCallback = NX_PRISM_Install_Language_Proc;
                break;
#if defined(CONFIG_MW_INET)
            case    eNxInstallPage_Network:
                HxSTD_StrNCpy((char*)(pstSubItem->ucTitle), (char*)ONDK_GetString(RES_NETWORK_ID), MAX_ELEMENT_TEXT_LENGTH);
                HxSTD_StrNCpy((char*)(pstSubItem->ucSubTitle), (char*)ONDK_GetString(RES_CONNECTION_ID), MAX_ELEMENT_TEXT_LENGTH);
                pstSubItem->procCallback = NX_PRISM_Install_Network_Proc;
                break;
#endif
            case    eNxInstallPage_Country:
                HxSTD_StrNCpy((char*)(pstSubItem->ucTitle), (char*)ONDK_GetString(RES_COUNTRY_06_ID ), MAX_ELEMENT_TEXT_LENGTH);
                HxSTD_StrNCpy((char*)(pstSubItem->ucSubTitle), (char*)ONDK_GetString(RES_SELECTION_ID ), MAX_ELEMENT_TEXT_LENGTH);
                pstSubItem->procCallback = NX_PRISM_Install_Country_Proc;
                break;
            case    eNxInstallPage_Search:
                HxSTD_StrNCpy((char*)(pstSubItem->ucTitle), (char*)ONDK_GetString(RES_CHANNEL_03_ID), MAX_ELEMENT_TEXT_LENGTH);
                HxSTD_StrNCpy((char*)(pstSubItem->ucSubTitle), (char*)ONDK_GetString(RES_SEARCH_03_ID ), MAX_ELEMENT_TEXT_LENGTH);
                pstSubItem->procCallback = NX_PRISM_Install_Search_Proc;
                break;
            case    eNxInstallPage_Search_Auto:
                HxSTD_StrNCpy((char*)(pstSubItem->ucTitle), (char*)ONDK_GetString(RES_CHANNEL_03_ID), MAX_ELEMENT_TEXT_LENGTH);
                HxSTD_StrNCpy((char*)(pstSubItem->ucSubTitle), (char*)ONDK_GetString(RES_SEARCH_03_ID ), MAX_ELEMENT_TEXT_LENGTH);
                pstSubItem->procCallback = NX_PRISM_Install_Search_Auto_Proc;
                break;

            case    eNxInstallPage_Result:
                HxSTD_StrNCpy((char*)(pstSubItem->ucTitle), (char*)ONDK_GetString(RES_RESULT_01_ID), MAX_ELEMENT_TEXT_LENGTH);
                HxSTD_StrNCpy((char*)(pstSubItem->ucSubTitle), (char*)ONDK_GetString(RES_RESULT_01_ID ), MAX_ELEMENT_TEXT_LENGTH);
                pstSubItem->procCallback = NX_PRISM_Install_Result_Proc;
                break;
            case    eNxInstallPage_Result_NoUI:
                HxSTD_StrNCpy((char*)(pstSubItem->ucTitle), (char*)ONDK_GetString(RES_RESULT_01_ID), MAX_ELEMENT_TEXT_LENGTH);
                HxSTD_StrNCpy((char*)(pstSubItem->ucSubTitle), (char*)ONDK_GetString(RES_RESULT_01_ID ), MAX_ELEMENT_TEXT_LENGTH);
                pstSubItem->procCallback = NX_PRISM_Install_Result_NoUI_Proc;
            default:
                break;
        }
        pstSubItem->ePageName = eNxPages;
    }

    *pstRetSubItemArray = (Nx_InstallSubPage_t *)NX_APP_Malloc (sizeof(Nx_InstallSubPage_t) * ulNumOfSubPage);

	if (NULL == *pstRetSubItemArray)
    {
	    if(NULL != pstSubItemArray)
	    {
	        NX_APP_Free(pstSubItemArray);
			pstSubItemArray = NULL;
	    }

	    if(NULL != eNxInstallPages)
	    {
	        NX_APP_Free(eNxInstallPages);
			eNxInstallPages = NULL;
	    }
        return GWM_RET_OUT_OF_MEMORY;
    }

    HxSTD_memcpy(*pstRetSubItemArray,pstSubItemArray,(HUINT32)(sizeof(Nx_InstallSubPage_t) * ulNumOfSubPage));
    *ulNumOfPage =(HUINT8)ulNumOfSubPage;

    if(NULL != pstSubItemArray)
    {
        NX_APP_Free(pstSubItemArray);
    }

    if(NULL != eNxInstallPages)
    {
        NX_APP_Free(eNxInstallPages);
    }
    return ONDK_RET_OK;
}

ONDK_Result_t  nx_install_ui_GetSubPage(Nx_InstallContext_t *pstContents)
{
    Nx_InstallSubPage_t	*pstSubItemArray = NULL;
    HUINT8 ulNumOfSubPage = 0;

    nx_install_ui_GetListOfSubPage(&pstSubItemArray, &ulNumOfSubPage);

    pstContents->pstSubItems = pstSubItemArray;
    pstContents->ulNumOfSubPage = ulNumOfSubPage;
    return GWM_RET_OK;
}

ONDK_Result_t	nx_install_ui_DoNextStep(NxInstallPage_t currentPage,NxInstallPage_t nextPage, HINT32 p1, HINT32 p2, HINT32 p3)
{
    ONDK_Result_t	eResult = ONDK_RET_OK;
    Nx_InstallSubPage_t	*pstNextSubItem;
    Nx_InstallSubPage_t	*pstCurrentSubItem;

    Nx_InstallSubPage_t *pstSubItem = NULL;
    HINT8           zIndex, zCurrentStep = -1;
    Nx_InstallSubPage_t	*pstSubItemArray = NULL;
    HUINT8 zNumOfSubPage = 0;
    HUINT8 zNextPageIndex = 0;

    eResult = nx_install_ui_GetListOfSubPage(&pstSubItemArray, &zNumOfSubPage);
	if(eResult != ONDK_RET_OK)
	{
		return eResult;
	}

    for(zIndex=0;zIndex<zNumOfSubPage;zIndex++)
    {
        pstSubItem = &(pstSubItemArray[zIndex]);
        if(currentPage == pstSubItem->ePageName){
            zCurrentStep = zIndex;
            break;
        }
    }

    //if(eNxInstallPage_Start == currentPage)
    if(pstSubItemArray != NULL  && zNumOfSubPage > 0
        &&  pstSubItemArray[0].ePageName == currentPage)
    {
        zCurrentStep = 0;
    }

    switch(nextPage){
        case eNxInstallPage_NextPage:
            zNextPageIndex = (zCurrentStep+1);
            if(zNextPageIndex >= zNumOfSubPage){
                //End of Subpage
                zNextPageIndex = eNxInstallPage_Result_NoUI;
            }
            break;
        case eNxInstallPage_PreviousPage:
            if(zCurrentStep==0)
            {
				if(pstSubItemArray)
				{
					NX_APP_Free(pstSubItemArray);
					pstSubItemArray = NULL;
				}
                return ONDK_RET_FAIL;
            }
            else if( ((zCurrentStep-1) == 0) && (pstSubItemArray[0].ePageName == eNxInstallPage_Welcome))
            {
                //Do not move to pre step
            	NX_APP_Free(pstSubItemArray);
            	pstSubItemArray = NULL;

                return ONDK_RET_FAIL;
            }
            else if(zCurrentStep == zNumOfSubPage)
            {
                zNextPageIndex = zNumOfSubPage-1;
            }
            else
            {
                zNextPageIndex = (zCurrentStep-1);
            }
            break;
        default:
            zNextPageIndex = nextPage;
            break;
    }

    pstCurrentSubItem = &(pstSubItemArray[zNextPageIndex]);

    if(zNextPageIndex < zNumOfSubPage)
    {
      // go to subpage
       pstNextSubItem = &(pstSubItemArray[zNextPageIndex]);
       ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, (HCHAR*)pstNextSubItem->ucTitle, APP_UIMGR_PRIORITY,
       pstNextSubItem->procCallback, 0, p1, p2, p3);
    }
    else if(zNextPageIndex == eNxInstallPage_Result_NoUI)
    {
      //go to result page
       ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, "NX_PRISM_INSTALL_RESULT_NoUI_Proc", APP_UIMGR_PRIORITY,
        NX_PRISM_Install_Result_NoUI_Proc, 0, p1, p2, p3);
    }

	if(pstSubItemArray)
	{
		NX_APP_Free(pstSubItemArray);
		pstSubItemArray = NULL;
	}

    return eResult;
}

/******************************************************************************/
#define _________End_Of_File____________________________________________________
/******************************************************************************/

