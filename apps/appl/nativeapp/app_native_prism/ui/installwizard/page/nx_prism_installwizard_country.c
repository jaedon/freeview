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
 * @file	  		nx_prism_installwizard_country.c
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
#include <nx_prism_menu_contents.h>
#include <nx_prism_component.h>
#include <nx_prism_menu_items.h>
#include <nx_prism_install_sub_country.h>


/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

#define	NX_PRISM_INSTALL_COUNTRY_ITEM_ID			        (NX_PRISM_INSTALL_ITEM_ID + 1000)

#define	NX_PRISM_INSTALL_COUNTRY_FRAME_ID			        (NX_PRISM_INSTALL_COUNTRY_ITEM_ID + 1)
#define	NX_PRISM_INSTALL_COUNTRY_TITLE_LINE_ID     			(NX_PRISM_INSTALL_COUNTRY_ITEM_ID + 2)

#define	NX_PRISM_INSTALL_COUNTRY_LIST_ITEM_MAX				8

#define	NX_PRISM_INSTALL_COUNTRY_LIST_CELL_W				822
#define	NX_PRISM_INSTALL_COUNTRY_LIST_CELL_H				60
#define	NX_PRISM_INSTALL_COUNTRY_LIST_CELL_MG				2
#define	NX_PRISM_INSTALL_COUNTRY_LIST_LS					(NX_PRISM_INSTALL_COUNTRY_LIST_CELL_H + NX_PRISM_INSTALL_COUNTRY_LIST_CELL_MG)

#define	NX_PRISM_INSTALL_COUNTRY_LIST_ID					(NX_PRISM_INSTALL_COUNTRY_ITEM_ID + 3)
#define	NX_PRISM_INSTALL_COUNTRY_LIST_X						229
#define	NX_PRISM_INSTALL_COUNTRY_LIST_Y						212
#define	NX_PRISM_INSTALL_COUNTRY_LIST_W						822
#define	NX_PRISM_INSTALL_COUNTRY_LIST_H						(NX_PRISM_INSTALL_COUNTRY_LIST_ITEM_MAX * NX_PRISM_INSTALL_COUNTRY_LIST_CELL_H)

#define	NX_PRISM_INSTALL_COUNTRY_1_HOUR						60


/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/

typedef struct tagInstall_Country_Context {
	HINT32		lCurFocus;
	HUINT32		ulNumOfMainItem, ulMaxOfMainItem;
    Nx_InstallSubPage_t	*pstSubItems;

	HINT32		lDefaultFocusIdx;
	HUINT8		ucTitle[NX_PRISM_INSTALL_MAX_ELEMENT_TEXT_LENGTH];
    HUINT8      ulNumOfSubPage;// Nx_InstallSubPage_t sub page number

    NxInstallPage_t ePage;

	HxCountry_e		eCountryCode;						/**< Wizard Country */
	HUINT32			ulNumOfItemTextList;				///< total num of sub item
	HUINT32			ulIdxOfTextList;					///< index of sub item
	HUINT8			**szItemTextList;					///< list of sub item
} Nx_InstallCountryContext_t;

/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/
static Nx_InstallCountryContext_t		s_stInstallCountryContent;


/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/

static inline Nx_InstallCountryContext_t	*nx_install_Country_GetContents(void);
static void				nx_install_Country_DrawCountryBtn(Nx_InstallCountryContext_t *pstContents);
static ONDK_Result_t	nx_install_Country_DrawDefaultUI(Nx_InstallCountryContext_t *pstContents);

static ONDK_Result_t	nx_install_Country_InitApplication (Nx_InstallCountryContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);

static ONDK_Result_t	nx_install_Country_MsgGwmCreate(Nx_InstallCountryContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_install_Country_MsgGwmDestroy(Nx_InstallCountryContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_install_Country_MsgGwmHapiMsg(Nx_InstallCountryContext_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_install_Country_MsgGwmKeyDown(Nx_InstallCountryContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_install_Country_MsgGwmFocusChanged(Nx_InstallCountryContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);


/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/

static inline Nx_InstallCountryContext_t	*nx_install_Country_GetContents(void)
{
	return	&s_stInstallCountryContent;
}

static void		nx_install_Country_DrawCountryBtn(Nx_InstallCountryContext_t *pstContents)
{
	ONDK_Rect_t		stRect = { 0, 0, 0, 0 };
	HINT32			lList_h = 0;

	/* Create List */
	lList_h = (NX_PRISM_INSTALL_COUNTRY_LIST_ITEM_MAX * NX_PRISM_INSTALL_COUNTRY_LIST_CELL_H) + ((NX_PRISM_INSTALL_COUNTRY_LIST_ITEM_MAX-1) * NX_PRISM_INSTALL_COUNTRY_LIST_CELL_MG);
	stRect = ONDK_Rect(NX_PRISM_INSTALL_COUNTRY_LIST_X, NX_PRISM_INSTALL_COUNTRY_LIST_Y,
						NX_PRISM_INSTALL_COUNTRY_LIST_W, lList_h);
	ONDK_GWM_List_Create(NX_PRISM_INSTALL_COUNTRY_LIST_ID, &stRect, (HCHAR*)"WizCountryList", NX_PRISM_INSTALL_COUNTRY_LIST_LS,
							pstContents->ulNumOfItemTextList, NX_PRISM_INSTALL_COUNTRY_LIST_ITEM_MAX, (HUINT32)pstContents->szItemTextList);
	ONDK_GWM_List_SetFontHandle(NX_PRISM_INSTALL_COUNTRY_LIST_ID, eFont_SystemBold, NX_PRISM_FONT_SIZE_34);
	ONDK_GWM_List_SetLineInterval(NX_PRISM_INSTALL_COUNTRY_LIST_ID, NX_PRISM_INSTALL_COUNTRY_LIST_CELL_MG);
	ONDK_GWM_List_SetCurrentIndex(NX_PRISM_INSTALL_COUNTRY_LIST_ID, pstContents->ulIdxOfTextList);
	ONDK_GWM_APP_SetObjectDrawMethod(NX_PRISM_INSTALL_COUNTRY_LIST_ID, (GWM_ObjDraw_t)NX_PRISM_COMP_DrawList_Cursor_A1);
}


static ONDK_Result_t	nx_install_Country_DrawDefaultUI(Nx_InstallCountryContext_t *pstContents)
{
	ONDK_Result_t	eResult = ONDK_RET_OK;
	HUINT8          ulPageIndex = 0, ulNumofPages = 0, ulIndex = 0;
	Nx_InstallSubPage_t	*pstSubItem = NULL;

	for (ulIndex = 0; ulIndex < pstContents->ulNumOfSubPage; ulIndex++)
	{
		pstSubItem = &(pstContents->pstSubItems[ulIndex]);
		if(pstSubItem->ePageName == pstContents->ePage)
		{
			ulPageIndex = ulIndex;
			break;
		}
	}

	ulNumofPages = pstContents->ulNumOfSubPage;

	nx_install_ui_DrawBackgroundUI();
	nx_install_ui_DrawTitle((HUINT8*)ONDK_GetString(RES_COUNTRY_06_ID));
	nx_install_ui_DrawStepIndicatior(ulPageIndex);

	nx_install_ui_DrawExplain((HCHAR*)ONDK_GetString(RES_SELECT_YOUR_COUNTRY_ID));

	//Language Selection
	nx_install_Country_DrawCountryBtn(pstContents);
	ONDK_GWM_Obj_SetFocus(NX_PRISM_INSTALL_COUNTRY_LIST_ID);

	return eResult;
}


static ONDK_Result_t	nx_install_Country_InitApplication (Nx_InstallCountryContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR			hError = ERR_FAIL;
    HUINT8			ulNumOfSubPage = 0, **ppListOfItem = NULL;
	HxCountry_e		currentCountry = eCountryID_NONE;
	HUINT32			ulItems = 0, ulListIdx = 0, ulMenuItemId = 0;
	ONDK_Result_t	eResult = ONDK_RET_FAIL;
    Nx_InstallSubPage_t	*pstSubItemArray = NULL;


	if (pstContents == NULL)
	{
		return GWM_RET_SOURCE_NULL;
	}

	HxSTD_MemSet(pstContents, 0x00, sizeof(Nx_InstallCountryContext_t));

	// Set Page Info
    pstContents->ePage = eNxInstallPage_Country;
    nx_install_ui_GetListOfSubPage(&pstSubItemArray, &ulNumOfSubPage);
    pstContents->pstSubItems = pstSubItemArray;
    pstContents->ulNumOfSubPage= ulNumOfSubPage;

	// Set Language Info
	hError = NX_SYSTEM_GetCountryCode(&currentCountry);
	if (hError == ERR_FAIL)
	{
		currentCountry = eCountryID_NONE;
		NX_APP_Error("Error!!! NX_SYSTEM_GetMenuLanuage() Fail!! ");
	}
	pstContents->eCountryCode = (HxCountry_e)currentCountry;
	ulMenuItemId = NX_MENUITEM_ID_SET_COUNTRYSETTING;

	eResult = NX_PRISM_MENU_Menuitem_CountExpandSubitems((Nx_MenuSubItemId_t)ulMenuItemId, &ulItems);
	if ((eResult != ONDK_RET_OK) || (ulItems == 0))
	{
		NX_APP_Error("Error!!! NX_PRISM_MENU_Menuitem_CountExpandSubitems() Fail!! ulMenuId:(0x%x)]\n", ulMenuItemId);
		return	eResult;
	}

	/* List를 만든다. */
	ppListOfItem = NX_PRISM_MENU_MenuItem_MakeExpandTextListFromMenuItemId((Nx_MenuSubItemId_t)ulMenuItemId, FALSE);
	if ( ppListOfItem == NULL )
	{
		NX_APP_Error("Error!!! NX_PRISM_MENU_MenuItem_MakeExpandTextListFromMenuItemId() Fail!! ulMenuItemId:(0x%x)]\n", ulMenuItemId);
		return	ONDK_RET_FAIL;
	}

	pstContents->ulNumOfItemTextList = ulItems;
	pstContents->szItemTextList = ppListOfItem;

	/* List의 Index를 얻어온다. */
	eResult = NX_PRISM_MENU_MenuItem_GetExpandIndexFromValue((Nx_MenuSubItemId_t)ulMenuItemId, (HINT32)(pstContents->eCountryCode), &ulListIdx);
	if (eResult != ONDK_RET_OK)
	{
		ulListIdx = 0;
	}

	pstContents->ulIdxOfTextList = ulListIdx;

	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return ONDK_RET_OK;
}


static ONDK_Result_t	nx_install_Country_MsgGwmCreate(Nx_InstallCountryContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t	eResult = ONDK_RET_OK;
	HUINT8		*pszPanelStr = NULL;

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		ONDK_GWM_APP_Destroy(0);
		return ONDK_RET_INITFAIL;
	}

	eResult = nx_install_Country_InitApplication(pstContents,p1,p2,p3);
	if (eResult != ONDK_RET_OK)
	{
		NX_APP_Error("Error!!! (eResult != ONDK_RET_OK) \n");
		ONDK_GWM_APP_Destroy(0);
		return eResult;
	}
	nx_install_Country_DrawDefaultUI(pstContents);

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


static ONDK_Result_t	nx_install_Country_MsgGwmDestroy(Nx_InstallCountryContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (pstContents->szItemTextList)
	{
		NX_PRISM_MENU_MenuItem_FreeTextList(pstContents->ulNumOfItemTextList, pstContents->szItemTextList);
		pstContents->szItemTextList = NULL;
	}

	NX_PANEL_ClearString();

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_install_Country_MsgGwmHapiMsg(Nx_InstallCountryContext_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);
	NX_COMMON_NOT_USED_PARAM(handle);

	return	ONDK_MESSAGE_PASS;
}

static ONDK_Result_t	nx_install_Country_MsgGwmFocusChanged(Nx_InstallCountryContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32		lObjId = p1, lListIdx = p2, lCountryValue = 0;


    if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		return	GWM_RET_SOURCE_NULL;
	}

    if ((HUINT32)lListIdx >= pstContents->ulNumOfItemTextList)
    {
    	NX_APP_Error("Error!!! Invalid List Index : (%u)\n", lListIdx);
		return	GWM_RET_SOURCE_NULL;
    }

	if (lObjId == NX_PRISM_INSTALL_COUNTRY_LIST_ID)
	{
		pstContents->ulIdxOfTextList = (HUINT32)lListIdx;
		NX_PRISM_MENU_MenuItem_GetExpandValueFromIndex(NX_MENUITEM_ID_SET_COUNTRYSETTING, lListIdx, &lCountryValue);
		pstContents->eCountryCode = (HxCountry_e)lCountryValue;
	}
	else
	{
    	NX_APP_Error("Error!!! Invalid List Object: (0x%x)\n", lObjId);
		return	ONDK_RET_FAIL;
	}

    return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_install_Country_MsgGwmKeyDown(Nx_InstallCountryContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR			hError = ERR_FAIL;
	ONDK_Result_t	eResult = ONDK_RET_OK;
	HINT32			lKeyId = p1, lHour = 0, lGmtOffset = 0;
	HUINT32 		ulMinute = 0;

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		return	GWM_RET_SOURCE_NULL;
	}

    eResult = nx_install_ui_MsgGwmKeyDown(p1,p2,p3);
    if(eResult != ONDK_MESSAGE_PASS)
    {
        return eResult;
    }

	switch (lKeyId)
	{
		/* keys to exit */
		case KEY_BACK:
			//move to previous Step
			eResult = nx_install_ui_DoNextStep(pstContents->ePage, eNxInstallPage_PreviousPage, p1, p2, p3);
			if(ONDK_RET_OK == eResult)
			{
				ONDK_GWM_APP_Destroy(0);
			}
			return ONDK_MESSAGE_BREAK;

		case KEY_EXIT:
		case KEY_TVRADIO:
		case KEY_VFORMAT:
		case KEY_AUDIO:
		case KEY_SUBTITLE:
		case KEY_TEXT:
		case KEY_MENU:
		case KEY_GUIDE:
			NX_PRISM_NotAvailable_Show();
			return	ONDK_MESSAGE_BREAK;

		case KEY_OK:
			{
				NX_SYSTEM_SetCountryCode(pstContents->eCountryCode);
				hError = HLIB_DATETIME_GetTimeOffsetByCountryIndex(pstContents->eCountryCode, &lHour, &ulMinute);
				if (hError == ERR_OK)
				{
					lGmtOffset = (lHour * NX_PRISM_INSTALL_COUNTRY_1_HOUR) + (HINT32)ulMinute;
					NX_SYSTEM_SetGMTOffset(lGmtOffset);
				}

				eResult = nx_install_ui_DoNextStep(pstContents->ePage, eNxInstallPage_NextPage, p1, p2, p3);
				if (ONDK_RET_OK == eResult)
				{
					ONDK_GWM_APP_Destroy(0);
				}
				return	ONDK_MESSAGE_BREAK;
			}
		case KEY_ARROWLEFT:
		case KEY_ARROWRIGHT:
		case KEY_ARROWUP:
		case KEY_ARROWDOWN:
			break;

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
ONDK_Result_t	NX_PRISM_Install_Country_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t		eResult = ONDK_RET_OK;
	Nx_InstallCountryContext_t	*pstContents = NULL;

	pstContents = nx_install_Country_GetContents();
	switch (lMessage)
	{
		case MSG_GWM_CREATE:
			eResult = nx_install_Country_MsgGwmCreate(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_KEYUP:
			break;

		case MSG_GWM_KEYDOWN:
			eResult = nx_install_Country_MsgGwmKeyDown(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_CLICKED:
		case MSG_GWM_SCROLLRIGHT:
			break;

		case MSG_GWM_LIST_CHANGED:
            eResult = nx_install_Country_MsgGwmFocusChanged(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_DESTROY:
			eResult = nx_install_Country_MsgGwmDestroy(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_HAPI_MESSAGE:
			eResult = nx_install_Country_MsgGwmHapiMsg(pstContents, handle, p1, p2, p3);
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
