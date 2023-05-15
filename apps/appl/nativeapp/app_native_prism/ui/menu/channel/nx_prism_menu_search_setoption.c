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
 * @file	  		nx_prism_menu_search_setoption.c
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
#include <nx_port.h>

#include <nx_prism_app.h>
#include <nx_prism_menu_contents.h>
#include <nx_prism_ui.h>
#include <nx_prism_component.h>
#include <nx_prism_menu_items.h>


/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/

#define	NX_PRISM_UI_MENU_SEARCH_OPT_BG_X						0
#define	NX_PRISM_UI_MENU_SEARCH_OPT_BG_Y						0
#define	NX_PRISM_UI_MENU_SEARCH_OPT_BG_W						660
#define	NX_PRISM_UI_MENU_SEARCH_OPT_BG_H						720

#define	NX_PRISM_UI_MENU_SEARCH_OPT_OPT_X						200
#define	NX_PRISM_UI_MENU_SEARCH_OPT_OPT_Y						674
#define	NX_PRISM_UI_MENU_SEARCH_OPT_OPT_W						400
#define	NX_PRISM_UI_MENU_SEARCH_OPT_OPT_H						30

#define	NX_PRISM_UI_MENU_SEARCH_OPT_LIST_Y2						694

#define	NX_PRISM_UI_MENU_SEARCH_OPT_ANTENNA_X					290
#define	NX_PRISM_UI_MENU_SEARCH_OPT_ANTENNA_Y					211
#define	NX_PRISM_UI_MENU_SEARCH_OPT_ANTENNA_W					700
#define	NX_PRISM_UI_MENU_SEARCH_OPT_ANTENNA_H					299

#define	NX_PRISM_UI_MENU_SEARCH_OPT_ANTENNA_LIST_ITEM_W			134

#define	NX_PRISM_UI_MENU_SEARCH_OPT_ANTENNA_LIST_X				(NX_PRISM_UI_MENU_SEARCH_OPT_ANTENNA_X + 96)
#define	NX_PRISM_UI_MENU_SEARCH_OPT_ANTENNA_LIST_Y				(NX_PRISM_UI_MENU_SEARCH_OPT_ANTENNA_Y + 76)
#define	NX_PRISM_UI_MENU_SEARCH_OPT_ANTENNA_LIST_W				700
#define	NX_PRISM_UI_MENU_SEARCH_OPT_ANTENNA_LIST_H				208

#define	NX_PRISM_UI_MENU_SEARCH_OPT_SELECTLIST_MAX				3


/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/
typedef enum {
	//eNxSettingsItem_SetOpt_Hidden = 0,
	eNxSettingsItem_SetOpt_Text = 0,
	eNxSettingsItem_SetOpt_Opt,
	eNxSettingsItem_SetOpt_FrameBG,
	eNxSettingsItem_SetOpt_MAX
} NxSettingsItem_SearchSetOptList_t;

typedef enum {
	eNxSettingsItem_SetOptItem_ALL = 0,
	eNxSettingsItem_SetOptItem_AntType,
	eNxSettingsItem_SetOptItem_SearchOpt,
	eNxSettingsItem_SetOptItem_MAX
} NxSettingsItem_SearchSetOptItem_t;

typedef struct
{
	NxSettingsItem_SearchSetOptItem_t	eOptItem;
	eNxUiUtil_OptItem_t			eCurOptItemValue;

	NX_SAT_AntennaType_e		eAntennaType;

	// Option Item
	HUINT8			**szItemTextList;
	HUINT8			**szImageNItemTextList;
	HUINT8			**szImageCItemTextList;
	NX_Util_OPTIONList_t	stOptList;
} Nx_MenuSearchSetOptContent_t;


/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/
static Nx_MenuSearchSetOptContent_t		s_stSearchSatOptContent;
static Nx_PrismListDialog_Content_t		s_stPopupListSearchOptContent;


/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/

static ONDK_Result_t	nx_menuSearchChannel_Opt_DisplayListPopup(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuSearchChannel_Opt_DestroyListPopup(void);

static void				nx_menuSearchChannel_Opt_CreateAntennaTypeListPopup(Nx_MenuSearchSetOptContent_t *pstContents);
static ONDK_Result_t	nx_menuSearchChannel_Opt_AntennaTypeInitData(Nx_MenuSearchSetOptContent_t *pstContents);
static void				nx_menuSearchChannel_Opt_AntennaTypeInitDataDefault(Nx_MenuSearchSetOptContent_t *pstContents);

static void 			nx_menuSearchChannel_Opt_CreateListPopup(Nx_MenuSearchSetOptContent_t *pstContents);

static inline Nx_MenuSearchSetOptContent_t* nx_menuSearchChannel_Opt_GetContents(void);

static HINT32			nx_menuSearchChannel_Opt_ConvertObjIdToItemIdx(HINT32 lObjId);
static HINT32			nx_menuSearchChannel_Opt_ConvertItemIdxToObjId(HINT32 lItemId);

static ONDK_Result_t	nx_menuSearchChannel_Opt_ActionAntennaType(Nx_MenuSearchSetOptContent_t *pstContents);
static ONDK_Result_t	nx_menuSearchChannel_Opt_ChangedSubItemValue(Nx_MenuSearchSetOptContent_t *pstContents, HUINT32 ulCurIdx);

static ONDK_Result_t	nx_menuSearchChannel_Opt_AddSearchOptionData(Nx_MenuSearchSetOptContent_t *pstContents);
static ONDK_Result_t	nx_menuSearchChannel_Opt_InitSettingsItems(Nx_MenuSearchSetOptContent_t *pstContents);
static ONDK_Result_t	nx_menuSearchChannel_Opt_ReCreateMenu(Nx_MenuSearchSetOptContent_t *pstContents);

static ONDK_Result_t	nx_menuSearchChannel_Opt_DrawOption(Nx_MenuSearchSetOptContent_t *pstContents);
static ONDK_Result_t	nx_menuSearchChannel_Opt_DrawDefaultUI(Nx_MenuSearchSetOptContent_t *pstContents);

static ONDK_Result_t	nx_menuSearchChannel_Opt_MsgGwmCreate(Nx_MenuSearchSetOptContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	nx_menuSearchChannel_Opt_MsgGwmDestroy(Nx_MenuSearchSetOptContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuSearchChannel_Opt_MsgGwmHapiMsg(Nx_MenuSearchSetOptContent_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuSearchChannel_Opt_MsgGwmKeyDown(Nx_MenuSearchSetOptContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);


/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/
static ONDK_Result_t	nx_menuSearchChannel_Opt_DisplayListPopup(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return	NX_PRISM_COMP_ListDlg_ProtoProc(&s_stPopupListSearchOptContent, message, hAction, p1, p2, p3);
}


static ONDK_Result_t	nx_menuSearchChannel_Opt_DestroyListPopup(void)
{
	return ONDK_GWM_APP_Destroy(nx_menuSearchChannel_Opt_DisplayListPopup);
}


/******************************************************************************/
#define __Satellite_Option_Functions___________________________________________
/******************************************************************************/

static void		nx_menuSearchChannel_Opt_CreateAntennaTypeListPopup(Nx_MenuSearchSetOptContent_t *pstContents)
{
	HINT32			lListX = 0, lListW = 0, lItemW = 0;
	HUINT32			ulTextListNum = 0, ulIndex = 0, i = 0, ulMaxListNum = 0, ulAttribute = 0;
	HUINT8			**pptextList = NULL, **ppImageListOfItem = NULL;
	ONDK_Rect_t		stRect = { 0, 0, 0, 0 };


	if (pstContents->stOptList.ulOptItemNum == 0)
	{
		NX_APP_Error("Error!!! Please Check added Option item !!\n");
		return;
	}

	ulTextListNum = pstContents->stOptList.ulOptItemNum;
	if (pstContents->szItemTextList == NULL)
	{
		pstContents->szItemTextList = NX_PRISM_UTIL_OptionMakeItemTextList(eNxUtil_OptType_TEXT, &pstContents->stOptList);
		pptextList = pstContents->szItemTextList;
		if (pptextList == NULL)
		{
			NX_APP_Info("Error!!! Please Check added Option item !!\n");
			return;
		}
	}
	else
	{
		pptextList = pstContents->szItemTextList;
	}

	ulIndex = 0;
	for (i = 0; i<pstContents->stOptList.ulOptItemNum; i++)
	{
		if (pstContents->stOptList.aeOptItemList[i] == (HUINT32)pstContents->eCurOptItemValue)
		{
			ulIndex = i;
			break;
		}
	}

	// Set Text List
	ulMaxListNum = (ulTextListNum > NX_PRISM_UI_MENU_SEARCH_OPT_SELECTLIST_MAX) ? NX_PRISM_UI_MENU_SEARCH_OPT_SELECTLIST_MAX : ulTextListNum;
	ulAttribute = (eListDlg_OSD_USER | eListDlg_OSD_BG | eListDlg_Title | eListDlg_MultiSelectHApp);
	lListW = (ulMaxListNum * NX_PRISM_UI_MENU_SEARCH_OPT_ANTENNA_LIST_ITEM_W) + (NX_PRISM_UI_DLG_MULTISEL_H_LIST_ITEM_TAB * (ulMaxListNum-1));
	if (ulTextListNum >= NX_PRISM_UI_MENU_SEARCH_OPT_SELECTLIST_MAX)
	{
		lListX = NX_PRISM_UI_MENU_SEARCH_OPT_ANTENNA_LIST_X;
	}
	else
	{
		lItemW = (NX_PRISM_UI_MENU_SEARCH_OPT_ANTENNA_LIST_ITEM_W + (NX_PRISM_UI_DLG_MULTISEL_H_LIST_ITEM_TAB/2));
		lListX = NX_PRISM_UI_MENU_SEARCH_OPT_ANTENNA_LIST_Y + ((NX_PRISM_UI_MENU_SEARCH_OPT_SELECTLIST_MAX - ulTextListNum)*lItemW);
	}

	stRect = ONDK_Rect(lListX, NX_PRISM_UI_MENU_SEARCH_OPT_ANTENNA_LIST_Y,
						lListW, NX_PRISM_UI_MENU_SEARCH_OPT_ANTENNA_LIST_H);
	NX_PRISM_COMP_ListDlg_SetListPopup(&s_stPopupListSearchOptContent, ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis()),
							&stRect, NX_PRISM_UI_MENU_SEARCH_OPT_ANTENNA_LIST_H, ulAttribute, ulTextListNum, ulMaxListNum, ulIndex, (HUINT32)pptextList);
	lItemW = (NX_PRISM_UI_MENU_SEARCH_OPT_ANTENNA_LIST_ITEM_W + (NX_PRISM_UI_DLG_MULTISEL_H_LIST_ITEM_TAB/2));
	NX_PRISM_COMP_ListDlg_SetLineWithSpace(&s_stPopupListSearchOptContent, lItemW);
	NX_PRISM_COMP_ListDlg_SetFontSize(&s_stPopupListSearchOptContent, NX_PRISM_FONT_SIZE_22);
	NX_PRISM_COMP_ListDlg_SetTitle(&s_stPopupListSearchOptContent, ONDK_GetString(RES_MESG_025_ID));

	if (ulAttribute & eListDlg_OSD_USER)
	{
		stRect = ONDK_Rect(NX_PRISM_UI_MENU_SEARCH_OPT_ANTENNA_X, NX_PRISM_UI_MENU_SEARCH_OPT_ANTENNA_Y,
							NX_PRISM_UI_MENU_SEARCH_OPT_ANTENNA_W, NX_PRISM_UI_MENU_SEARCH_OPT_ANTENNA_H);
		NX_PRISM_COMP_ListDlg_SetExternalArea(&s_stPopupListSearchOptContent, &stRect);
	}

	// Set Normal Image List
	if (pstContents->szImageNItemTextList == NULL)
	{
		pstContents->szImageNItemTextList = NX_PRISM_UTIL_OptionMakeItemTextList(eNxUtil_OptType_IMG_N, &pstContents->stOptList);
		ppImageListOfItem = pstContents->szImageNItemTextList;
		if (ppImageListOfItem == NULL)
		{
			NX_APP_Info("Error!!! Please Check added Option Normal Image item !!\n");
			return;
		}
	}
	else
	{
		ppImageListOfItem = pstContents->szImageNItemTextList;
	}

	NX_PRISM_COMP_ListDlg_SetImageList(&s_stPopupListSearchOptContent, (HUINT32)ppImageListOfItem);

	// Set Cursor Image List
	if (pstContents->szImageCItemTextList == NULL)
	{
		pstContents->szImageCItemTextList = NX_PRISM_UTIL_OptionMakeItemTextList(eNxUtil_OptType_IMG_C, &pstContents->stOptList);
		ppImageListOfItem = pstContents->szImageCItemTextList;
		if (ppImageListOfItem == NULL)
		{
			NX_APP_Info("Error!!! Please Check added Option Cursor Image item !!\n");
			return;
		}
	}
	else
	{
		ppImageListOfItem = pstContents->szImageCItemTextList;
	}

	NX_PRISM_COMP_ListDlg_SetImageExtList(&s_stPopupListSearchOptContent, (HUINT32)ppImageListOfItem);

	/* Create Pop Up */
	ONDK_GWM_APP_Create (ONDK_UPPER_WINDOW, "nx_menuSearchChannel_Opt_DisplayListPopup", APP_DEFAULT_PRIORITY, nx_menuSearchChannel_Opt_DisplayListPopup, 0, 0, 0, 0);
}


static ONDK_Result_t	nx_menuSearchChannel_Opt_AntennaTypeInitData(Nx_MenuSearchSetOptContent_t *pstContents)
{
	HINT32			lValue = 0;
	HERROR			hError = ERR_FAIL;
	DxOperator_e	eOpType = eDxOPERATOR_None;
	NX_SAT_AntennaType_e	eAntennaType = eNxSat_AntType_LNB_Only;


	if (pstContents->szItemTextList)
	{
		NX_PRISM_UTIL_OptionDeleteItemTextList(&pstContents->stOptList, pstContents->szItemTextList);
		pstContents->szItemTextList = NULL;
	}
	if (pstContents->szImageNItemTextList)
	{
		NX_PRISM_UTIL_OptionDeleteItemTextList(&pstContents->stOptList, pstContents->szImageNItemTextList);
		pstContents->szImageNItemTextList = NULL;
	}
	if (pstContents->szImageCItemTextList)
	{
		NX_PRISM_UTIL_OptionDeleteItemTextList(&pstContents->stOptList, pstContents->szImageCItemTextList);
		pstContents->szImageCItemTextList = NULL;
	}

	HxSTD_MemSet(&pstContents->stOptList, 0x00, sizeof(NX_Util_OPTIONList_t));
	pstContents->eOptItem = eNxSettingsItem_SetOptItem_AntType;

	// Antenna Type
	hError = NX_ANTENNA_GetAntennaType(&lValue);
	if (hError != ERR_FAIL)
	{
		NX_APP_Error("Error!!! NX_ANTENNA_GetAntennaType() Fail !! \n");
		eAntennaType = eNxSat_AntType_LNB_Only;
	}
	else
	{
		eAntennaType = (NX_SAT_AntennaType_e)lValue;
	}
	pstContents->eAntennaType = eAntennaType;

	// Operator Type
	NX_SYSTEM_GetOperatorType(&eOpType);
	switch (eOpType)
	{
		// TODO: other data
		default:
			nx_menuSearchChannel_Opt_AntennaTypeInitDataDefault(pstContents);
			break;
	}

	return ONDK_RET_OK;
}


static void		nx_menuSearchChannel_Opt_AntennaTypeInitDataDefault(Nx_MenuSearchSetOptContent_t *pstContents)
{
	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxUtil_OptItem_AntennaTypeFixed);
	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxUtil_OptItem_AntennaTypeScd);
	// TODO: [JHLEE] Add Enable when Support Motor Antenna
	//NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxUtil_OptItem_AntennaTypeMotor);
}


/******************************************************************************/
#define __Satellite_Option_Functions___________________________________________
/******************************************************************************/

static void		nx_menuSearchChannel_Opt_CreateListPopup(Nx_MenuSearchSetOptContent_t *pstContents)
{
	HINT32		y = 0, h = 0;
	HUINT32		ulTextListNum = 0, ulIndex = 0, i = 0;
	HUINT8		**pptextList = NULL, **ppImageListOfItem = NULL;
	ONDK_Rect_t		stRect;


	if (pstContents->stOptList.ulOptItemNum == 0)
	{
		NX_APP_Error("Error!!! Please Check added Option item !!\n");
		return;
	}

	ulTextListNum = pstContents->stOptList.ulOptItemNum;
	if (pstContents->szItemTextList == NULL)
	{
		pstContents->szItemTextList = NX_PRISM_UTIL_OptionMakeItemTextList(eNxUtil_OptType_TEXT, &pstContents->stOptList);
		pptextList = pstContents->szItemTextList;
		if (pptextList == NULL)
		{
			NX_APP_Info("Error!!! Please Check added Option item !!\n");
			return;
		}
	}
	else
	{
		pptextList = pstContents->szItemTextList;
	}

	ulIndex = 0;
	for (i = 0; i<pstContents->stOptList.ulOptItemNum; i++)
	{
		if (pstContents->stOptList.aeOptItemList[i] == (HUINT32)pstContents->eCurOptItemValue)
		{
			ulIndex = i;
			break;
		}
	}

	// Set Text List
	h = (NX_PRISM_UI_OPT_POPUP_CELL_HEIGHT * pstContents->stOptList.ulOptItemNum) + (NX_PRISM_UI_OPT_POPUP_CELL_MG * (pstContents->stOptList.ulOptItemNum - 1)) + NX_PRISM_UI_OPT_POPUP_POINTER_H;
	y = NX_PRISM_UI_MENU_SEARCH_OPT_LIST_Y2 - h - NX_PRISM_UI_OPT_POPUP_POINTER_H;
	stRect = ONDK_Rect(NX_PRISM_UI_OPT_POPUP_LIST_R_X, y, NX_PRISM_UI_OPT_POPUP_LIST_WIDTH, h);
	NX_PRISM_COMP_ListDlg_SetListPopup (&s_stPopupListSearchOptContent, ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis()),
										&stRect, (NX_PRISM_UI_OPT_POPUP_CELL_HEIGHT + NX_PRISM_UI_OPT_POPUP_CELL_MG),
										eListDlg_IMAGE_OptionRApp, ulTextListNum, ulTextListNum, ulIndex, (HUINT32)pptextList);

	// Set Normal Image List
	if (pstContents->szImageNItemTextList == NULL)
	{
		pstContents->szImageNItemTextList = NX_PRISM_UTIL_OptionMakeItemTextList(eNxUtil_OptType_IMG_N, &pstContents->stOptList);
		ppImageListOfItem = pstContents->szImageNItemTextList;
		if (ppImageListOfItem == NULL)
		{
			NX_APP_Info("Error!!! Please Check added Option Normal Image item !!\n");
			return;
		}
	}
	else
	{
		ppImageListOfItem = pstContents->szImageNItemTextList;
	}

	NX_PRISM_COMP_ListDlg_SetImageList(&s_stPopupListSearchOptContent, (HUINT32)ppImageListOfItem);

	// Set Cursor Image List
	if (pstContents->szImageCItemTextList == NULL)
	{
		pstContents->szImageCItemTextList = NX_PRISM_UTIL_OptionMakeItemTextList(eNxUtil_OptType_IMG_C, &pstContents->stOptList);
		ppImageListOfItem = pstContents->szImageCItemTextList;
		if (ppImageListOfItem == NULL)
		{
			NX_APP_Info("Error!!! Please Check added Option Cursor Image item !!\n");
			return;
		}
	}
	else
	{
		ppImageListOfItem = pstContents->szImageCItemTextList;
	}

	NX_PRISM_COMP_ListDlg_SetImageExtList(&s_stPopupListSearchOptContent, (HUINT32)ppImageListOfItem);

	/* Create Pop Up */
	ONDK_GWM_APP_Create (ONDK_UPPER_WINDOW, "nx_menuSearchChannel_Opt_DisplayListPopup", APP_DEFAULT_PRIORITY, nx_menuSearchChannel_Opt_DisplayListPopup, 0, 0, 0, 0);
}


static inline Nx_MenuSearchSetOptContent_t* nx_menuSearchChannel_Opt_GetContents(void)
{
	return	&s_stSearchSatOptContent;
}


static HINT32	nx_menuSearchChannel_Opt_ConvertObjIdToItemIdx(HINT32 lObjId)
{
	HINT32	lItemId = 0;

	lItemId = (lObjId & ~eNxSetting_SatelliteOption);

	if ((lItemId < 0) || (lItemId >= eNxSettingsItem_SetOpt_MAX))
	{
		return -1;
	}

	return	lItemId;
}


static HINT32	nx_menuSearchChannel_Opt_ConvertItemIdxToObjId(HINT32 lItemId)
{
	if ((lItemId < 0) || (lItemId >= eNxSettingsItem_SetOpt_MAX))
	{
		return 0;
	}

	return	(eNxSetting_SatelliteOption | lItemId);
}


static ONDK_Result_t	nx_menuSearchChannel_Opt_ActionAntennaType(Nx_MenuSearchSetOptContent_t *pstContents)
{
	// Init Data
	nx_menuSearchChannel_Opt_AntennaTypeInitData(pstContents);

	// Current Index
	switch (pstContents->eAntennaType)
	{
		case eNxSat_AntType_LNB_Only:
		case eNxSat_AntType_DiSEqC_1_0:
		case eNxSat_AntType_DiSEqC_2_0:
			pstContents->eCurOptItemValue = eNxUtil_OptItem_AntennaTypeFixed;
			break;
		case eNxSat_AntType_SCD:
			pstContents->eCurOptItemValue = eNxUtil_OptItem_AntennaTypeScd;
			break;
		case eNxSat_AntType_Motor:
			pstContents->eCurOptItemValue = eNxUtil_OptItem_AntennaTypeMotor;
			break;
		default:
			pstContents->eCurOptItemValue = eNxUtil_OptItem_AntennaTypeFixed;
			break;
	}

	// Show List
	nx_menuSearchChannel_Opt_CreateAntennaTypeListPopup(pstContents);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSearchChannel_Opt_ChangedSubItemValue(Nx_MenuSearchSetOptContent_t *pstContents, HUINT32 ulCurIdx)
{
	HINT32		lObjId = -1;
	eNxUiUtil_OptItem_t		eCurOptionItem = eNxUtil_OptItem_None;

	switch (pstContents->eOptItem)
	{
		case eNxSettingsItem_SetOptItem_ALL:
			nx_menuSearchChannel_Opt_DestroyListPopup();
			if (ulCurIdx >= pstContents->stOptList.ulOptItemNum)
			{
				NX_APP_Error("Error!!! Unknow Index - ulCurIdx(%d)!! \n", ulCurIdx);
				break;
			}

			if (pstContents->stOptList.aeOptItemList[ulCurIdx] == eNxUtil_OptItem_AntennaType)
			{
				lObjId = nx_menuSearchChannel_Opt_ConvertItemIdxToObjId(eNxSettingsItem_SetOpt_FrameBG);
				if (lObjId > 0)
				{
					ONDK_GWM_Obj_Destroy(lObjId);
				}

				lObjId = nx_menuSearchChannel_Opt_ConvertItemIdxToObjId(eNxSettingsItem_SetOpt_Text);
				if (lObjId > 0)
				{
					ONDK_GWM_Obj_Destroy(lObjId);
				}

				lObjId = nx_menuSearchChannel_Opt_ConvertItemIdxToObjId(eNxSettingsItem_SetOpt_Opt);
				if (lObjId > 0)
				{
					ONDK_GWM_Obj_Destroy(lObjId);
				}

				nx_menuSearchChannel_Opt_ActionAntennaType(pstContents);
			}
			else if (pstContents->stOptList.aeOptItemList[ulCurIdx] == eNxUtil_OptItem_SearchOpt)
			{
				ONDK_GWM_APP_Destroy(0);
				ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, (HCHAR*)"NX_PRISM_MENU_Search_Option_Proc", (APP_DEFAULT_PRIORITY | UISTATE_MENU),
									(ONDK_GWM_Callback_t)NX_PRISM_MENU_Search_Option_Proc, 0, 0, 0, 0);
			}
			else
			{
				NX_APP_Error("Error!!! Unknow Index - ulCurIdx(%d)!! \n", ulCurIdx);
				ONDK_GWM_APP_Destroy(0);
			}
			break;

		case eNxSettingsItem_SetOptItem_AntType:
			nx_menuSearchChannel_Opt_DestroyListPopup();
			if (ulCurIdx >= pstContents->stOptList.ulOptItemNum)
			{
				NX_APP_Error("Error!!! Unknow Index - ulCurIdx(%d)!! \n", ulCurIdx);
				break;
			}

			eCurOptionItem = (eNxUiUtil_OptItem_t)pstContents->stOptList.aeOptItemList[ulCurIdx];
			ONDK_GWM_APP_Destroy(0);
			if (eCurOptionItem == (HUINT32)eNxUtil_OptItem_AntennaTypeFixed)
			{
				ONDK_GWM_SendMessage(NULL, MSG_APP_POPUP_OPT_VALUE_CHANGED, (Handle_t)NULL, eNX_UCT_Menu,
										0, eCurOptionItem);
			}
			else if (eCurOptionItem == (HUINT32)eNxUtil_OptItem_AntennaTypeScd)
			{
				ONDK_GWM_SendMessage(NULL, MSG_APP_POPUP_OPT_VALUE_CHANGED, (Handle_t)NULL, eNX_UCT_Menu,
										0, eCurOptionItem);
			}
			else if (eCurOptionItem == (HUINT32)eNxUtil_OptItem_AntennaTypeMotor)
			{
				ONDK_GWM_SendMessage(NULL, MSG_APP_POPUP_OPT_VALUE_CHANGED, (Handle_t)NULL, eNX_UCT_Menu,
										0, eCurOptionItem);
			}
			else
			{
				NX_APP_Error("Error!!! Unknow Item - Item(%d)!! \n", eCurOptionItem);
			}
			break;
		default:
			NX_APP_Error("Error!!! Not Support option Item - (%d)!! \n", pstContents->eOptItem);
			break;
	}

	NX_COMMON_NOT_USED_PARAM(pstContents);
	NX_COMMON_NOT_USED_PARAM(ulCurIdx);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSearchChannel_Opt_AddSearchOptionData(Nx_MenuSearchSetOptContent_t *pstContents)
{
	DxOperator_e	eOpType = eDxOPERATOR_None;

	pstContents->eOptItem = eNxSettingsItem_SetOptItem_ALL;
	// Operator Type
	NX_SYSTEM_GetOperatorType(&eOpType);
	switch (eOpType)
	{
		default:
#if defined(CONFIG_MW_CH_SATELLITE)
			NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxUtil_OptItem_AntennaType);
			pstContents->eCurOptItemValue = eNxUtil_OptItem_AntennaType;
#else
			pstContents->eCurOptItemValue = eNxUtil_OptItem_SearchOpt;
#endif
			NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxUtil_OptItem_SearchOpt);
			break;
	}

	// TODO: Add Project config if you want to do

	return ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSearchChannel_Opt_InitSettingsItems(Nx_MenuSearchSetOptContent_t *pstContents)
{
	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! pstContents == NULL !! \n");
		return ONDK_RET_INITFAIL;
	}

	if (pstContents->szItemTextList)
	{
		NX_PRISM_UTIL_OptionDeleteItemTextList(&pstContents->stOptList, pstContents->szItemTextList);
		pstContents->szItemTextList = NULL;
	}
	if (pstContents->szImageNItemTextList)
	{
		NX_PRISM_UTIL_OptionDeleteItemTextList(&pstContents->stOptList, pstContents->szImageNItemTextList);
		pstContents->szImageNItemTextList = NULL;
	}
	if (pstContents->szImageCItemTextList)
	{
		NX_PRISM_UTIL_OptionDeleteItemTextList(&pstContents->stOptList, pstContents->szImageCItemTextList);
		pstContents->szImageCItemTextList = NULL;
	}

	HxSTD_MemSet(&pstContents->stOptList, 0x00, sizeof(NX_Util_OPTIONList_t));

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSearchChannel_Opt_ReCreateMenu(Nx_MenuSearchSetOptContent_t *pstContents)
{
	// Init Menu Item
	nx_menuSearchChannel_Opt_InitSettingsItems(pstContents);

	// Search Channel Option
	nx_menuSearchChannel_Opt_AddSearchOptionData(pstContents);

	nx_menuSearchChannel_Opt_CreateListPopup(pstContents);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSearchChannel_Opt_DrawOption(Nx_MenuSearchSetOptContent_t *pstContents)
{
	HINT32			lObjId = -1;
	ONDK_Rect_t 	stRect = { 0, 0, 0, 0 };

	// Option
	stRect = ONDK_Rect(NX_PRISM_UI_MENU_SEARCH_OPT_OPT_X, NX_PRISM_UI_MENU_SEARCH_OPT_OPT_Y, NX_PRISM_UI_MENU_SEARCH_OPT_OPT_W, NX_PRISM_UI_MENU_SEARCH_OPT_OPT_H);
	lObjId = nx_menuSearchChannel_Opt_ConvertItemIdxToObjId(eNxSettingsItem_SetOpt_Opt);
	ONDK_GWM_COM_Rect_Create(lObjId, &stRect, 0);
	ONDK_GWM_APP_SetObjectDrawMethod(lObjId, NX_PRISM_COMP_DrawOption_HelpR);

	NX_COMMON_NOT_USED_PARAM(pstContents);
	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSearchChannel_Opt_DrawDefaultUI(Nx_MenuSearchSetOptContent_t *pstContents)
{
	HUINT32			ulObjId = 0;
	ONDK_Rect_t		stRect;


	NX_COMMON_NOT_USED_PARAM(pstContents);

	// Draw BackGround
	stRect = ONDK_Rect(NX_PRISM_UI_MENU_SEARCH_OPT_BG_X, NX_PRISM_UI_MENU_SEARCH_OPT_BG_Y, NX_PRISM_UI_MENU_SEARCH_OPT_BG_W, NX_PRISM_UI_MENU_SEARCH_OPT_BG_H);
	ulObjId = (HUINT32)nx_menuSearchChannel_Opt_ConvertItemIdxToObjId(eNxSettingsItem_SetOpt_FrameBG);
	ONDK_GWM_COM_Rect_Create(ulObjId, &stRect, 0);
	ONDK_GWM_APP_SetObjectDrawMethod(ulObjId, NX_PRISM_COMP_DrawWindowOptionRight);

	// Draw Temp Object
	stRect = ONDK_Rect(0, 0, 0, 0);
	ulObjId = (HUINT32)nx_menuSearchChannel_Opt_ConvertItemIdxToObjId(eNxSettingsItem_SetOpt_Text);
	ONDK_GWM_COM_Button_Create(ulObjId, &stRect, NULL);
	ONDK_GWM_APP_SetObjectDrawMethod(ulObjId, NX_PRISM_COMP_DrawWEmpty);
	ONDK_GWM_Obj_SetFocus(ulObjId);

	// Draw Option
	nx_menuSearchChannel_Opt_DrawOption(pstContents);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSearchChannel_Opt_MsgGwmCreate(Nx_MenuSearchSetOptContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_GWM_APP_SetAppArea(NX_PRISM_UI_OPT_POPUP_R_X, NX_PRISM_UI_OPT_POPUP_R_Y, NX_PRISM_UI_OPT_POPUP_WIDTH, NX_PRISM_UI_OPT_POPUP_HEIGHT);

	// Init Menu Item
	nx_menuSearchChannel_Opt_InitSettingsItems(pstContents);

	// Search Channel Option
	nx_menuSearchChannel_Opt_AddSearchOptionData(pstContents);

	// Draw Menu Item
	nx_menuSearchChannel_Opt_DrawDefaultUI(pstContents);

	nx_menuSearchChannel_Opt_CreateListPopup(pstContents);

	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSearchChannel_Opt_MsgGwmDestroy(Nx_MenuSearchSetOptContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (pstContents == NULL)
	{
		return ONDK_RET_FAIL;
	}

	nx_menuSearchChannel_Opt_DestroyListPopup();

	if (pstContents->szItemTextList)
	{
		NX_PRISM_UTIL_OptionDeleteItemTextList(&pstContents->stOptList, pstContents->szItemTextList);
		pstContents->szItemTextList = NULL;
	}
	if (pstContents->szImageNItemTextList)
	{
		NX_PRISM_UTIL_OptionDeleteItemTextList(&pstContents->stOptList, pstContents->szImageNItemTextList);
		pstContents->szImageNItemTextList = NULL;
	}
	if (pstContents->szImageCItemTextList)
	{
		NX_PRISM_UTIL_OptionDeleteItemTextList(&pstContents->stOptList, pstContents->szImageCItemTextList);
		pstContents->szImageCItemTextList = NULL;
	}
	HxSTD_MemSet(&pstContents->stOptList, 0x00, sizeof(NX_Util_OPTIONList_t));
	HxSTD_memset(pstContents, 0x00, sizeof(Nx_MenuSearchSetOptContent_t));

	NX_COMMON_NOT_USED_PARAM(pstContents);
	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSearchChannel_Opt_MsgGwmHapiMsg(Nx_MenuSearchSetOptContent_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	NX_COMMON_NOT_USED_PARAM(pstContents);
	NX_COMMON_NOT_USED_PARAM(handle);
	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_MESSAGE_PASS;
}


static ONDK_Result_t	nx_menuSearchChannel_Opt_MsgGwmKeyDown(Nx_MenuSearchSetOptContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32			lKeyId = p1;

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! Please check!! \n");
		return	GWM_RET_SOURCE_NULL;
	}

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	switch (lKeyId)
	{
		/* keys to exit */
		case KEY_EXIT:
		case KEY_GUIDE:
		case KEY_STANDBY:
			ONDK_GWM_APP_Destroy(0);
			return	ONDK_MESSAGE_PASS;

		case KEY_OPT:
			ONDK_GWM_APP_Destroy(0);
			return	ONDK_MESSAGE_BREAK;

		case KEY_TVRADIO:
		case KEY_VFORMAT:
		case KEY_AUDIO:
		case KEY_SUBTITLE:
		case KEY_TEXT:
			NX_PRISM_NotAvailable_Show();
			return	ONDK_MESSAGE_BREAK;

		case KEY_OK:
		case KEY_ARROWRIGHT:
		case KEY_ARROWUP:
		case KEY_ARROWDOWN:
			break;

		default:
			NX_APP_Print("nx_menuSearchChannel_Opt_MsgGwmKeyDown() Not used lKeyId(0x%x)\n", lKeyId);
			return	ONDK_MESSAGE_PASS;
	}

	return	ONDK_RET_OK;
}


/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/

ONDK_Result_t	NX_PRISM_MENU_Search_SetOption_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t			eResult = ONDK_RET_OK;
	Nx_MenuSearchSetOptContent_t	*pstContents = NULL;

	pstContents = nx_menuSearchChannel_Opt_GetContents();

	switch (lMessage)
	{
		case MSG_GWM_CREATE:
			NX_APP_Message("MSG_GWM_CREATE!\n");
			eResult = nx_menuSearchChannel_Opt_MsgGwmCreate(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_KEYUP:
			break;
		case MSG_GWM_KEYDOWN:
			eResult = nx_menuSearchChannel_Opt_MsgGwmKeyDown(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_CLICKED:
		case MSG_GWM_SCROLLRIGHT:
			NX_APP_Message("MSG_GWM_CLICKED or MSG_GWM_SCROLLRIGHT!\n");
			break;

		case MSG_GWM_SCROLLUP:
			NX_APP_Message("MSG_GWM_SCROLLUP!\n");
			break;

		case MSG_GWM_SCROLLDOWN:
			NX_APP_Message("MSG_GWM_SCROLLDOWN!\n");
			break;

		case MSG_GWM_DESTROY:
			NX_APP_Message("MSG_GWM_DESTROY!\n");
			eResult = nx_menuSearchChannel_Opt_MsgGwmDestroy(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_HAPI_MESSAGE:
			NX_APP_Message("MSG_GWM_HAPI_MESSAGE!\n");
			eResult = nx_menuSearchChannel_Opt_MsgGwmHapiMsg(pstContents, handle, p1, p2, p3);
			break;

		case MSG_APP_SETTING_VALUE_CHANGED:
			NX_APP_Message("MSG_APP_POPUP_LIST_DESTROYED & MSG_APP_SETTING_VALUE_CHANGED\n");
			if (lMessage == MSG_APP_SETTING_VALUE_CHANGED)
			{
				NX_APP_Message("MSG_APP_SETTING_VALUE_CHANGED\n");
				nx_menuSearchChannel_Opt_DestroyListPopup();
				eResult = nx_menuSearchChannel_Opt_ChangedSubItemValue(pstContents, p1);
			}
			break;

		case MSG_APP_POPUP_LIST_DESTROYED:
			NX_APP_Message("MSG_APP_POPUP_LIST_DESTROYED!\n");
			if ((p1 == KEY_BACK) && (pstContents->eOptItem != eNxSettingsItem_SetOptItem_ALL))
			{
				nx_menuSearchChannel_Opt_DestroyListPopup();
				nx_menuSearchChannel_Opt_ReCreateMenu(pstContents);
			}
			else
			{
				ONDK_GWM_APP_Destroy(0);
			}
			break;

		case MSG_APP_HOMEMENU_UI_LAUNCH:
			NX_APP_Message("MSG_APP_HOMEMENU_UI_LAUNCH\n");
			if (p1 != eNX_UCT_Menu)
			{
				ONDK_GWM_APP_Destroy(0);
			}
			break;

		default:
			break;
	}

	if(eResult != ONDK_RET_OK)
	{
		return eResult;
	}

	return	ONDK_GWM_ProcessMessageDefault(lMessage, handle, p1, p2, p3);
}


/******************************************************************************/
#define _________End_Of_File____________________________________________________
/******************************************************************************/
