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
 * @file	  		nx_prism_installwizard_welcome.c
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
#include <nx_prism_ui.h>
#include <nx_prism_component.h>

#include <nx_prism_install_sub_welcome.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/

#define NX_INSTALL_01_YELLOW_ITEM_X                             295
#define NX_INSTALL_01_YELLOW_ITEM_Y                             297
#define NX_INSTALL_01_YELLOW_ITEM_W                             230
#define NX_INSTALL_01_YELLOW_ITEM_H                             155

#define NX_INSTALL_01_YELLOW_ITEM_NUMBER_X                      102
#define NX_INSTALL_01_YELLOW_ITEM_NUMBER_Y                      23

#define NX_INSTALL_01_STEP_MAIN_X                               0
#define NX_INSTALL_01_STEP_MAIN_Y                               103

#define NX_INSTALL_01_STEP_MAIN_W                               230
#define NX_INSTALL_01_STEP_MAIN_H                               26

#define NX_INSTALL_01_STEP_SUB_X                                0
#define NX_INSTALL_01_STEP_SUB_Y                                (NX_INSTALL_01_STEP_MAIN_Y+32)

#define NX_INSTALL_01_START_BTN_X                               495
#define NX_INSTALL_01_START_BTN_Y                               615
#define NX_INSTALL_01_START_BTN_W                               290
#define NX_INSTALL_01_START_BTN_H                               55

#define	NX_INSTALL_01_YELLOW_ITEM_OBJECT_ID	    	    	(NX_PRISM_INSTALL_ITEM_ID+120)
#define	NX_INSTALL_01_STEP_MAIN_TEXT_OBJECT_ID		    	(NX_PRISM_INSTALL_ITEM_ID+130)
#define	NX_INSTALL_01_STEP_SUB_TEXT_OBJECT_ID			    (NX_PRISM_INSTALL_ITEM_ID+140)
#define	NX_INSTALL_01_NEXT_BTN_OBJECT_ID			    	(NX_PRISM_INSTALL_ITEM_ID+150)


/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/

/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/
static Nx_InstallContext_t		            s_stInstallContent;

/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/

static inline Nx_InstallContext_t  *nx_install_welcome_GetContents(void);
static ONDK_Result_t	nx_install_welcome_InitApplication (Nx_InstallContext_t *pstContents);
static ONDK_Result_t	nx_install_welcome_DrawDefaultUI(Nx_InstallContext_t *pstContents);

static void				nx_install_welcome_DrawStartBtn(void);
static void				nx_install_welcome_DrawStep(HUINT8 *pMainText, HUINT8 *pSubText, HUINT8 index, ONDK_Rect_t *stFrame);

static ONDK_Result_t	nx_install_welcome_ActionGoNext(Nx_InstallContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_install_welcome_MsgGwmCreate(Nx_InstallContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_install_welcome_MsgGwmDestroy(Nx_InstallContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_install_welcome_MsgGwmHapiMsg(Nx_InstallContext_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_install_welcome_MsgGwmKeyDown(Nx_InstallContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);


/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/

static inline Nx_InstallContext_t	*nx_install_welcome_GetContents(void)
{
	return	&s_stInstallContent;
}

static ONDK_Result_t	nx_install_welcome_InitApplication (Nx_InstallContext_t *pstContents)
{
	ONDK_Result_t	eResult = ONDK_RET_FAIL;

	if (pstContents == NULL)
	{
		return GWM_RET_SOURCE_NULL;
	}

	HxSTD_MemSet(pstContents, 0, sizeof(Nx_InstallContext_t));

    eResult = nx_install_ui_GetSubPage(pstContents);

    pstContents->ePage = eNxInstallPage_Welcome;

	return ONDK_RET_OK;
}

static void nx_install_welcome_DrawStartBtn(void)
{
    ONDK_Rect_t		stRect = { 0, 0, 0, 0 };
	/* Button Index */
	stRect = ONDK_Rect(NX_INSTALL_01_START_BTN_X, NX_INSTALL_01_START_BTN_Y, NX_INSTALL_01_START_BTN_W, NX_INSTALL_01_START_BTN_H);

	ONDK_GWM_COM_Button_Create (NX_INSTALL_01_NEXT_BTN_OBJECT_ID, &stRect, (char*)ONDK_GetString(RES_START_NOW_02_ID));
    ONDK_GWM_COM_Button_SetFontColor(NX_INSTALL_01_NEXT_BTN_OBJECT_ID, COL(C_G_Bang_100), COL(C_G_Bang_100));
    ONDK_GWM_COM_Button_SetFont(NX_INSTALL_01_NEXT_BTN_OBJECT_ID, eFont_SystemBold);
    ONDK_GWM_COM_Button_SetSimpleButton(NX_INSTALL_01_NEXT_BTN_OBJECT_ID,TRUE); // molink_ksjung_0151014
    ONDK_GWM_COM_Button_SetButtonColor(NX_INSTALL_01_NEXT_BTN_OBJECT_ID,  COL(C_G_Base01_100), COL(C_G_Base01_100), COL(C_G_Base01_100), COL(C_G_Base01_100)); // molink_ksjung_0151014
    ONDK_GWM_COM_Button_SetButtonDimmedAlpha(NX_INSTALL_01_NEXT_BTN_OBJECT_ID,100);

    ONDK_GWM_COM_Button_SetStatus(NX_INSTALL_01_NEXT_BTN_OBJECT_ID,GUIOBJ_NORMAL);
	ONDK_GWM_COM_Button_SetTitleSize(NX_INSTALL_01_NEXT_BTN_OBJECT_ID,NX_PRISM_FONT_SIZE_24);
    ONDK_GWM_COM_Button_SetAlign(NX_INSTALL_01_NEXT_BTN_OBJECT_ID,ONDKSTF_CENTER);
	ONDK_GWM_Obj_SetVisible(NX_INSTALL_01_NEXT_BTN_OBJECT_ID, TRUE);
    ONDK_GWM_Obj_SetFocus(NX_INSTALL_01_NEXT_BTN_OBJECT_ID);
}

static void nx_install_welcome_DrawStep(HUINT8 *pMainText, HUINT8 *pSubText, HUINT8 index, ONDK_Rect_t *stFrame)
{
    ONDK_Rect_t		stRect = { 0, 0, 0, 0 };
    HUINT32 id = (NX_INSTALL_01_STEP_MAIN_TEXT_OBJECT_ID+index);
    HUINT32 subid = (NX_INSTALL_01_STEP_SUB_TEXT_OBJECT_ID+index);

	stRect = ONDK_Rect((stFrame->x + NX_INSTALL_01_STEP_MAIN_X) ,(stFrame->y + NX_INSTALL_01_STEP_MAIN_Y) , NX_INSTALL_01_STEP_MAIN_W, NX_INSTALL_01_STEP_MAIN_H);

	ONDK_GWM_COM_Text_Create(id , &stRect, (HCHAR*)pMainText);
	ONDK_GWM_COM_Text_SetFont(id , eFont_SystemBold);
	ONDK_GWM_COM_Text_SetSize(id , NX_PRISM_FONT_SIZE_24);
	ONDK_GWM_COM_Text_SetAlign(id , TEXT_ALIGN_CENTER);
	ONDK_GWM_COM_Text_ChangeColor(id , COL(C_T_Whistle_100), COL(C_T_Whistle_100));

	stRect = ONDK_Rect((stFrame->x + NX_INSTALL_01_STEP_SUB_X) ,(stFrame->y + NX_INSTALL_01_STEP_SUB_Y) , NX_INSTALL_01_STEP_MAIN_W, NX_INSTALL_01_STEP_MAIN_H);

	ONDK_GWM_COM_Text_Create(subid , &stRect, (HCHAR*)pSubText);
	ONDK_GWM_COM_Text_SetFont(subid , eFont_SystemNormal);
	ONDK_GWM_COM_Text_SetSize(subid , NX_PRISM_FONT_SIZE_24);
	ONDK_GWM_COM_Text_SetAlign(subid , TEXT_ALIGN_CENTER);
	ONDK_GWM_COM_Text_ChangeColor(subid , COL(C_T_Gong04), COL(C_T_Gong04));
}

static ONDK_Result_t	nx_install_welcome_DrawDefaultUI(Nx_InstallContext_t *pstContents)
{
	ONDK_Result_t	eResult = ONDK_RET_FAIL;

	if (pstContents == NULL)
	{
		return GWM_RET_SOURCE_NULL;
	}

    nx_install_ui_DrawBackgroundUI();

    //Title
    HxSTD_StrNCpy((char*)pstContents->ucTitle, (char*)ONDK_GetString(RES_INSTALLATION_WIZARD_01_ID), NX_PRISM_INSTALL_MAX_ELEMENT_TEXT_LENGTH);
    nx_install_ui_DrawTitle(pstContents->ucTitle);

    //Main Page
    {
        HUINT8      index = 0;
        HUINT16     pageNumber = 0;
       	HCHAR		*pageIndexStr;
        HINT32      lYellowItemX = 0;
        HUINT16     realPageStepNumber = 0;
        ONDK_Rect_t stRect;
        HUINT32 id = 0;
        Nx_InstallSubPage_t	*pstSubItems;
        HINT32      lSubItemDrawingIndex = 0;

        for(index=0;index<pstContents->ulNumOfSubPage;index++)
        {
            pstSubItems = pstContents->pstSubItems+index;
            if(nx_install_ui_isAvailableIndicatorStep(pstSubItems->ePageName))
            {
                realPageStepNumber++;
            }
        }

        pageNumber = realPageStepNumber;

        nx_install_ui_DrawExplain((HCHAR*)ONDK_GetString(RES_MESG_3082_ID));

        for(index=0;index<pstContents->ulNumOfSubPage;index++)
        {
            id = (NX_INSTALL_01_YELLOW_ITEM_OBJECT_ID+index);
            pstSubItems = pstContents->pstSubItems+index;

            if(nx_install_ui_isAvailableIndicatorStep(pstSubItems->ePageName))
            {
                pageIndexStr = HxSTR_MemAlloc(sizeof(HUINT8)*2);
                HxSTD_MemSet(pageIndexStr, 0x00, 2);

                if(pageNumber<=3)
                {
                    lYellowItemX = (NX_INSTALL_01_YELLOW_ITEM_X+(NX_INSTALL_01_YELLOW_ITEM_W*lSubItemDrawingIndex));
                }
                else
                {
                    lYellowItemX = (NX_INSTALL_01_YELLOW_ITEM_X-((NX_INSTALL_01_YELLOW_ITEM_W/2)*(pageNumber-3) )+(NX_INSTALL_01_YELLOW_ITEM_W*lSubItemDrawingIndex));
                }


                stRect = ONDK_Rect(lYellowItemX, NX_INSTALL_01_YELLOW_ITEM_Y, NX_INSTALL_01_YELLOW_ITEM_W, NX_INSTALL_01_YELLOW_ITEM_H);


                HxSTD_snprintf(pageIndexStr, 5, "%d", (lSubItemDrawingIndex+1));

                ONDK_GWM_COM_Multi_Create(id, &stRect);

                if(lSubItemDrawingIndex == 0)
                {
                    //FIRST
                    ONDK_GWM_COM_Multi_SetContents(id, (HUINT8*)RES_320_00_WIZ_PROCESS_1L_PNG,NULL, NULL, NULL, 0);
                }
                else if(lSubItemDrawingIndex == (pageNumber-1))
                {
                    //LAST
                     ONDK_GWM_COM_Multi_SetContents(id, (HUINT8*)RES_320_00_WIZ_PROCESS_3R_PNG,NULL, NULL, NULL, 0);
                }else
                {
                    //MIDDLE
                     ONDK_GWM_COM_Multi_SetContents(id, (HUINT8*)RES_320_00_WIZ_PROCESS_2C_PNG,NULL, NULL, NULL, 0);
                }

                ONDK_GWM_COM_Multi_ChangeImagePosition(id, 0, 0);
                ONDK_GWM_COM_Multi_SetFontHandle(id, eFont_SystemBold, NX_PRISM_FONT_SIZE_22);
        	    ONDK_GWM_COM_Multi_SetFontColor(id, COL(C_T_Bang_100), COL(C_T_Bang_100));
        	    ONDK_GWM_COM_Multi_SetFocusFlag(id, FALSE);
                ONDK_GWM_COM_Multi_ChangeCenterStr(id,pageIndexStr);

                if(lSubItemDrawingIndex == 0)
                {
                    ONDK_GWM_COM_Multi_ChangeStrPosition(id, NX_INSTALL_01_YELLOW_ITEM_NUMBER_X, (NX_INSTALL_01_YELLOW_ITEM_NUMBER_Y));
                }
                else if(lSubItemDrawingIndex == (pageNumber-1))
                {
                    ONDK_GWM_COM_Multi_ChangeStrPosition(id, NX_INSTALL_01_YELLOW_ITEM_NUMBER_X+8, (NX_INSTALL_01_YELLOW_ITEM_NUMBER_Y));
                }else
                {
                    ONDK_GWM_COM_Multi_ChangeStrPosition(id, NX_INSTALL_01_YELLOW_ITEM_NUMBER_X+6, (NX_INSTALL_01_YELLOW_ITEM_NUMBER_Y));
                }

                nx_install_welcome_DrawStep(pstSubItems->ucTitle,pstSubItems->ucSubTitle,lSubItemDrawingIndex,&stRect);

                ONDK_GWM_Obj_DisableObject(id);
                lSubItemDrawingIndex++;
            }
        }

        nx_install_welcome_DrawStartBtn();
    }

   	return	eResult;
}


static ONDK_Result_t	nx_install_welcome_ActionGoNext(Nx_InstallContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t	eResult = ONDK_RET_OK;

	eResult = nx_install_ui_DoNextStep(pstContents->ePage, eNxInstallPage_NextPage, p1, p2, p3);
	if (eResult != ONDK_RET_OK)
	{
		NX_APP_Error("Error!!! nx_install_ui_DoNextStep() Fail!! \n");
	}

	ONDK_GWM_APP_Destroy(0);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_install_welcome_MsgGwmCreate(Nx_InstallContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t	eResult = ONDK_RET_OK;
	HUINT8		*pszPanelStr = NULL;

    HxLOG_Error("\n\n####### nx_install_welcome_MsgGwmCreate ########\n\n");

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		ONDK_GWM_APP_Destroy(0);

		return ONDK_RET_INITFAIL;
	}

	/* alloc mainItems, mainItem has each infos of sub menu to launch */
	eResult = nx_install_welcome_InitApplication(pstContents);
	if (eResult != ONDK_RET_OK)
	{
		NX_APP_Error("Error!!! (eResult != ONDK_RET_OK) \n");
		ONDK_GWM_APP_Destroy(0);

		return eResult;
	}

	nx_install_welcome_DrawDefaultUI(pstContents);

	pszPanelStr = NX_PRISM_SPEC_GetPanelString(eNX_PANEL_STR_Wizard);
	if (pszPanelStr == NULL)
	{
		NX_APP_Error("Error!! NX_PRISM_SPEC_GetPanelString() - eNX_PANEL_STR_Wizard !!!\n");
	}
	NX_PANEL_SetString(pszPanelStr, eDxPANEL_ALIGN_CENTER, FALSE);

	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	eResult;
}


static ONDK_Result_t	nx_install_welcome_MsgGwmDestroy(Nx_InstallContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (pstContents->pstSubItems)
	{
   		NX_APP_Free(pstContents->pstSubItems);
		pstContents->pstSubItems = NULL;
	}
	NX_PANEL_ClearString();

	NX_COMMON_NOT_USED_PARAM(pstContents);
	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_install_welcome_MsgGwmHapiMsg(Nx_InstallContext_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	NX_COMMON_NOT_USED_PARAM(pstContents);
	NX_COMMON_NOT_USED_PARAM(handle);
	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);
	return	ONDK_MESSAGE_PASS;
}


static ONDK_Result_t	nx_install_welcome_MsgGwmKeyDown(Nx_InstallContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32		lKeyId = p1, lObjId = -1;
	ONDK_Result_t ret = ONDK_MESSAGE_PASS;

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		return	GWM_RET_SOURCE_NULL;
	}

    //Check common key
    ret = nx_install_ui_MsgGwmKeyDown(p1,p2,p3);
    if(ret != ONDK_MESSAGE_PASS)
    {
        return ret;
    }

	switch (lKeyId)
	{
		/* keys to exit */
		case KEY_BACK:
		case KEY_EXIT:
			return	ONDK_MESSAGE_BREAK;
		case KEY_OK:
            {
				lObjId = GWM_GetFocusedObjectID();
                switch(lObjId)
				{
                    case NX_INSTALL_01_NEXT_BTN_OBJECT_ID://start
                    	nx_install_welcome_ActionGoNext(pstContents, p1, p2, p3);
						break;

                    default:
                        break;
                }
             }
            return	ONDK_MESSAGE_BREAK;
		case KEY_ARROWLEFT:
		case KEY_ARROWRIGHT:
		case KEY_ARROWUP:
		case KEY_ARROWDOWN:
			break;
		case KEY_TVRADIO:
		case KEY_VFORMAT:
		case KEY_AUDIO:
		case KEY_SUBTITLE:
		case KEY_TEXT:
		case KEY_MENU:
		case KEY_GUIDE:
			NX_PRISM_NotAvailable_Show();
			return	ONDK_MESSAGE_BREAK;
		case KEY_STANDBY:
            ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, "NX_PRISM_STANDBY_Proc", APP_DEFAULT_POPUP_PRIORITY, NX_PRISM_STANDBY_Proc, 0, 0, 0, 0);
			ONDK_GWM_APP_Destroy(0);
			return ONDK_MESSAGE_BREAK;
		default:
			NX_APP_Print("nx_menuSettings_MsgGwmKeyDown() Not used lKeyId(0x%x)\n", lKeyId);
			return	ONDK_MESSAGE_PASS;
	}

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);
	return	ONDK_RET_OK;
}

/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/
ONDK_Result_t	NX_PRISM_Install_Welcome_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t		eResult = ONDK_RET_OK;
	Nx_InstallContext_t	*pstContents = NULL;

	pstContents = nx_install_welcome_GetContents();
	switch (lMessage)
	{
		case MSG_GWM_CREATE:
			eResult = nx_install_welcome_MsgGwmCreate(pstContents, p1, p2, p3);
			break;
		case MSG_GWM_KEYDOWN:
			eResult = nx_install_welcome_MsgGwmKeyDown(pstContents, p1, p2, p3);
			break;
		case MSG_GWM_DESTROY:
			eResult = nx_install_welcome_MsgGwmDestroy(pstContents, p1, p2, p3);
			break;
		case MSG_GWM_HAPI_MESSAGE:
			eResult = nx_install_welcome_MsgGwmHapiMsg(pstContents, handle, p1, p2, p3);
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

