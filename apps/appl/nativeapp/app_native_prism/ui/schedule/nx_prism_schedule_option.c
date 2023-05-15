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
 * @file	  		nx_prism_schedule_option.c
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

#define	NX_PRISM_UI_SCHEDULE_OPT_BG_X						0
#define	NX_PRISM_UI_SCHEDULE_OPT_BG_Y						0
#define	NX_PRISM_UI_SCHEDULE_OPT_BG_W						660
#define	NX_PRISM_UI_SCHEDULE_OPT_BG_H						720

#define	NX_PRISM_UI_SCHEDULE_OPT_OPT_X						200
#define	NX_PRISM_UI_SCHEDULE_OPT_OPT_Y						674
#define	NX_PRISM_UI_SCHEDULE_OPT_OPT_W						400
#define	NX_PRISM_UI_SCHEDULE_OPT_OPT_H						30

#define	NX_PRISM_UI_SCHEDULE_OPT_LIST_Y2					694


/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/
typedef enum {
	eNxUiScheduleItem_Opt_Text = 0,
	eNxUiScheduleItem_Opt_Opt,
	eNxUiScheduleItem_Opt_FrameBG,
	eNxUiScheduleItem_Opt_MAX
} NxUiScheduleItem_OptList_t;


typedef enum {
	NxUiScheduleItem_SetOptItem_ALL = 0,
	NxUiScheduleItem_SetOptItem_Edit,
	NxUiScheduleItem_SetOptItem_MAX
} NxUiSchedule_SetOptItem_t;

typedef struct
{
	NxUiSchedule_SetOptItem_t	eOptItem;
	eNxUiUtil_OptItem_t			eCurOptItemValue;
	NX_UI_CallerType_t			eDispOptType;

	// Option Item
	HUINT8			**szItemTextList;
	HUINT8			**szImageNItemTextList;
	HUINT8			**szImageCItemTextList;
	NX_Util_OPTIONList_t	stOptList;
} Nx_MenuSearchSatOptContent_t;


/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/
static Nx_MenuSearchSatOptContent_t		s_stSearchSatOptContent;
static Nx_PrismListDialog_Content_t		s_stPopupListSearchOptContent;


/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/

static ONDK_Result_t	nx_schedule_Opt_DisplayListPopup(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_schedule_Opt_DestroyListPopup(void);

static void 			nx_schedule_Opt_CreateListPopup(Nx_MenuSearchSatOptContent_t *pstContents);

static inline Nx_MenuSearchSatOptContent_t* nx_schedule_Opt_GetContents(void);

static HINT32			nx_schedule_Opt_ConvertObjIdToItemIdx(HINT32 lObjId);
static HINT32			nx_schedule_Opt_ConvertItemIdxToObjId(HINT32 lItemId);

static ONDK_Result_t	nx_schedule_Opt_ChangedSubItemValue(Nx_MenuSearchSatOptContent_t *pstContents, HUINT32 ulCurIdx);

static ONDK_Result_t	nx_schedule_Opt_AddEditOptionData(Nx_MenuSearchSatOptContent_t *pstContents);
static ONDK_Result_t	nx_schedule_Opt_AddAllOptionData(Nx_MenuSearchSatOptContent_t *pstContents);
static ONDK_Result_t	nx_schedule_Opt_InitSettingsItems(Nx_MenuSearchSatOptContent_t *pstContents);
static ONDK_Result_t	nx_schedule_Opt_ReCreateMenu(Nx_MenuSearchSatOptContent_t *pstContents);

static ONDK_Result_t	nx_schedule_Opt_DrawOption(Nx_MenuSearchSatOptContent_t *pstContents);
static ONDK_Result_t	nx_schedule_Opt_DrawDefaultUI(Nx_MenuSearchSatOptContent_t *pstContents);

static ONDK_Result_t	nx_schedule_Opt_MsgGwmCreate(Nx_MenuSearchSatOptContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	nx_schedule_Opt_MsgGwmDestroy(Nx_MenuSearchSatOptContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_schedule_Opt_MsgGwmHapiMsg(Nx_MenuSearchSatOptContent_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_schedule_Opt_MsgGwmKeyDown(Nx_MenuSearchSatOptContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);


/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/
static ONDK_Result_t	nx_schedule_Opt_DisplayListPopup(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return	NX_PRISM_COMP_ListDlg_ProtoProc(&s_stPopupListSearchOptContent, message, hAction, p1, p2, p3);
}


static ONDK_Result_t	nx_schedule_Opt_DestroyListPopup(void)
{
	return ONDK_GWM_APP_Destroy(nx_schedule_Opt_DisplayListPopup);
}


/******************************************************************************/
#define __ALL_Option_Functions___________________________________________
/******************************************************************************/

static void		nx_schedule_Opt_CreateListPopup(Nx_MenuSearchSatOptContent_t *pstContents)
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
	y = NX_PRISM_UI_SCHEDULE_OPT_LIST_Y2 - h - NX_PRISM_UI_OPT_POPUP_POINTER_H;
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
	ONDK_GWM_APP_Create (ONDK_UPPER_WINDOW, "nx_schedule_Opt_DisplayListPopup", APP_DEFAULT_PRIORITY, nx_schedule_Opt_DisplayListPopup, 0, 0, 0, 0);
}


static inline Nx_MenuSearchSatOptContent_t* nx_schedule_Opt_GetContents(void)
{
	return	&s_stSearchSatOptContent;
}


static HINT32	nx_schedule_Opt_ConvertObjIdToItemIdx(HINT32 lObjId)
{
	HINT32	lItemId = 0;

	lItemId = (lObjId & ~eMenuFrame_Window_ScheduleOpt);

	if ((lItemId < 0) || (lItemId >= eNxUiScheduleItem_Opt_MAX))
	{
		return -1;
	}

	return	lItemId;
}


static HINT32	nx_schedule_Opt_ConvertItemIdxToObjId(HINT32 lItemId)
{
	if ((lItemId < 0) || (lItemId >= eNxUiScheduleItem_Opt_MAX))
	{
		return 0;
	}

	return	(eMenuFrame_Window_ScheduleOpt | lItemId);
}


static ONDK_Result_t	nx_schedule_Opt_ChangedSubItemValue(Nx_MenuSearchSatOptContent_t *pstContents, HUINT32 ulCurIdx)
{
	eNxUiUtil_OptItem_t		eCurOptionItem = eNxUtil_OptItem_None;

	switch (pstContents->eOptItem)
	{
		case NxUiScheduleItem_SetOptItem_ALL:
			nx_schedule_Opt_DestroyListPopup();
			if (ulCurIdx >= pstContents->stOptList.ulOptItemNum)
			{
				NX_APP_Error("Error!!! Unknow Index - ulCurIdx(%d)!! \n", ulCurIdx);
				break;
			}

			eCurOptionItem = pstContents->stOptList.aeOptItemList[ulCurIdx];
			switch (eCurOptionItem)
			{
				case eNxUtil_OptItem_Delete:
				case eNxUtil_OptItem_ReSchedule:
				case eNxUtil_OptItem_Select:
				case eNxUtil_OptItem_SelectAll:
					ONDK_GWM_SendMessage(NULL, MSG_APP_POPUP_OPT_VALUE_CHANGED, (Handle_t)NULL, pstContents->eDispOptType, eCurOptionItem, 0);
					ONDK_GWM_APP_Destroy(0);
					break;
				default:
					NX_APP_Error("Error!!! Unknow Option Item- (%d)!! \n", eCurOptionItem);
					break;
			}
			break;

		case NxUiScheduleItem_SetOptItem_Edit:
			nx_schedule_Opt_DestroyListPopup();
			if (ulCurIdx >= pstContents->stOptList.ulOptItemNum)
			{
				NX_APP_Error("Error!!! Unknow Index - ulCurIdx(%d)!! \n", ulCurIdx);
				break;
			}

			eCurOptionItem = pstContents->stOptList.aeOptItemList[ulCurIdx];
			switch (eCurOptionItem)
			{
				case eNxUtil_OptItem_Delete:
				case eNxUtil_OptItem_DeSelectAll:
					ONDK_GWM_SendMessage(NULL, MSG_APP_POPUP_OPT_VALUE_CHANGED, (Handle_t)NULL, pstContents->eDispOptType, eCurOptionItem, 0);
					ONDK_GWM_APP_Destroy(0);
					break;
				default:
					NX_APP_Error("Error!!! Unknow Option Item- (%d)!! \n", eCurOptionItem);
					break;
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


static ONDK_Result_t	nx_schedule_Opt_AddEditOptionData(Nx_MenuSearchSatOptContent_t *pstContents)
{
	DxOperator_e	eOpType = eDxOPERATOR_None;

	pstContents->eOptItem = NxUiScheduleItem_SetOptItem_Edit;
	// Operator Type
	NX_SYSTEM_GetOperatorType(&eOpType);
	switch (eOpType)
	{
		default:
			NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxUtil_OptItem_Delete);
			NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxUtil_OptItem_DeSelectAll);
			pstContents->eCurOptItemValue = eNxUtil_OptItem_Delete;
			break;
	}

	// TODO: Add Project config if you want to do

	return ONDK_RET_OK;
}


static ONDK_Result_t	nx_schedule_Opt_AddAllOptionData(Nx_MenuSearchSatOptContent_t *pstContents)
{
	DxOperator_e	eOpType = eDxOPERATOR_None;

	pstContents->eOptItem = NxUiScheduleItem_SetOptItem_ALL;
	// Operator Type
	NX_SYSTEM_GetOperatorType(&eOpType);
	switch (eOpType)
	{
		default:
			NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxUtil_OptItem_Delete);
			NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxUtil_OptItem_ReSchedule);
			NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxUtil_OptItem_Select);
			NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxUtil_OptItem_SelectAll);
			pstContents->eCurOptItemValue = eNxUtil_OptItem_Delete;
			break;
	}

	// TODO: Add Project config if you want to do

	return ONDK_RET_OK;
}


static ONDK_Result_t	nx_schedule_Opt_InitSettingsItems(Nx_MenuSearchSatOptContent_t *pstContents)
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


static ONDK_Result_t	nx_schedule_Opt_ReCreateMenu(Nx_MenuSearchSatOptContent_t *pstContents)
{
	NX_UI_CallerType_t		eDispOptType;

	eDispOptType = pstContents->eDispOptType;

	// Init Menu Item
	nx_schedule_Opt_InitSettingsItems(pstContents);
	pstContents->eDispOptType = eDispOptType;

	// Search Channel Option
	nx_schedule_Opt_AddAllOptionData(pstContents);

	nx_schedule_Opt_CreateListPopup(pstContents);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_schedule_Opt_DrawOption(Nx_MenuSearchSatOptContent_t *pstContents)
{
	HINT32			lObjId = -1;
	ONDK_Rect_t 	stRect = { 0, 0, 0, 0 };

	// Option
	stRect = ONDK_Rect(NX_PRISM_UI_SCHEDULE_OPT_OPT_X, NX_PRISM_UI_SCHEDULE_OPT_OPT_Y, NX_PRISM_UI_SCHEDULE_OPT_OPT_W, NX_PRISM_UI_SCHEDULE_OPT_OPT_H);
	lObjId = nx_schedule_Opt_ConvertItemIdxToObjId(eNxUiScheduleItem_Opt_Opt);
	ONDK_GWM_COM_Rect_Create(lObjId, &stRect, 0);
	ONDK_GWM_APP_SetObjectDrawMethod(lObjId, NX_PRISM_COMP_DrawEdit_HelpR);

	NX_COMMON_NOT_USED_PARAM(pstContents);
	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_schedule_Opt_DrawDefaultUI(Nx_MenuSearchSatOptContent_t *pstContents)
{
	HUINT32			ulObjId = 0;
	ONDK_Rect_t		stRect;


	NX_COMMON_NOT_USED_PARAM(pstContents);

	// Draw BackGround
	stRect = ONDK_Rect(NX_PRISM_UI_SCHEDULE_OPT_BG_X, NX_PRISM_UI_SCHEDULE_OPT_BG_Y, NX_PRISM_UI_SCHEDULE_OPT_BG_W, NX_PRISM_UI_SCHEDULE_OPT_BG_H);
	ulObjId = (HUINT32)nx_schedule_Opt_ConvertItemIdxToObjId(eNxUiScheduleItem_Opt_FrameBG);
	ONDK_GWM_COM_Rect_Create(ulObjId, &stRect, 0);
	ONDK_GWM_APP_SetObjectDrawMethod(ulObjId, NX_PRISM_COMP_DrawWindowOptionRight);

	// Draw Temp Object
	stRect = ONDK_Rect(0, 0, 0, 0);
	ulObjId = (HUINT32)nx_schedule_Opt_ConvertItemIdxToObjId(eNxUiScheduleItem_Opt_Text);
	ONDK_GWM_COM_Button_Create(ulObjId, &stRect, NULL);
	ONDK_GWM_APP_SetObjectDrawMethod(ulObjId, NX_PRISM_COMP_DrawWEmpty);
	ONDK_GWM_Obj_SetFocus(ulObjId);

	// Draw Option
	nx_schedule_Opt_DrawOption(pstContents);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_schedule_Opt_MsgGwmCreate(Nx_MenuSearchSatOptContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_GWM_APP_SetAppArea(NX_PRISM_UI_OPT_POPUP_R_X, NX_PRISM_UI_OPT_POPUP_R_Y, NX_PRISM_UI_OPT_POPUP_WIDTH, NX_PRISM_UI_OPT_POPUP_HEIGHT);

	// Init Menu Item
	nx_schedule_Opt_InitSettingsItems(pstContents);
	pstContents->eDispOptType = p1;

	// All Schedule Option
	if (pstContents->eDispOptType == eNX_UCT_Schedule)
	{
		nx_schedule_Opt_AddAllOptionData(pstContents);
	}
	else
	{
		nx_schedule_Opt_AddEditOptionData(pstContents);
	}

	// Draw Menu Item
	nx_schedule_Opt_DrawDefaultUI(pstContents);

	nx_schedule_Opt_CreateListPopup(pstContents);

	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_schedule_Opt_MsgGwmDestroy(Nx_MenuSearchSatOptContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (pstContents == NULL)
	{
		return ONDK_RET_FAIL;
	}

	nx_schedule_Opt_DestroyListPopup();

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
	HxSTD_memset(pstContents, 0x00, sizeof(Nx_MenuSearchSatOptContent_t));

	NX_COMMON_NOT_USED_PARAM(pstContents);
	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_schedule_Opt_MsgGwmHapiMsg(Nx_MenuSearchSatOptContent_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	NX_COMMON_NOT_USED_PARAM(pstContents);
	NX_COMMON_NOT_USED_PARAM(handle);
	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_MESSAGE_PASS;
}


static ONDK_Result_t	nx_schedule_Opt_MsgGwmKeyDown(Nx_MenuSearchSatOptContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
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
		case KEY_MENU:
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
			NX_APP_Print("nx_schedule_Opt_MsgGwmKeyDown() Not used lKeyId(0x%x)\n", lKeyId);
			return	ONDK_MESSAGE_PASS;
	}

	return	ONDK_RET_OK;
}


/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/

ONDK_Result_t	NX_PRISM_SCHEDULE_Option_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t			eResult = ONDK_RET_OK;
	Nx_MenuSearchSatOptContent_t	*pstContents = NULL;

	pstContents = nx_schedule_Opt_GetContents();

	switch (lMessage)
	{
		case MSG_GWM_CREATE:
			NX_APP_Message("MSG_GWM_CREATE!\n");
			eResult = nx_schedule_Opt_MsgGwmCreate(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_KEYUP:
			break;
		case MSG_GWM_KEYDOWN:
			eResult = nx_schedule_Opt_MsgGwmKeyDown(pstContents, p1, p2, p3);
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
			eResult = nx_schedule_Opt_MsgGwmDestroy(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_HAPI_MESSAGE:
			NX_APP_Message("MSG_GWM_HAPI_MESSAGE!\n");
			eResult = nx_schedule_Opt_MsgGwmHapiMsg(pstContents, handle, p1, p2, p3);
			break;

		case MSG_APP_SETTING_VALUE_CHANGED:
			NX_APP_Message("MSG_APP_POPUP_LIST_DESTROYED & MSG_APP_SETTING_VALUE_CHANGED\n");
			if (lMessage == MSG_APP_SETTING_VALUE_CHANGED)
			{
				NX_APP_Message("MSG_APP_SETTING_VALUE_CHANGED\n");
				nx_schedule_Opt_DestroyListPopup();
				eResult = nx_schedule_Opt_ChangedSubItemValue(pstContents, p1);
			}
			break;

		case MSG_APP_POPUP_LIST_DESTROYED:
			NX_APP_Message("MSG_APP_POPUP_LIST_DESTROYED!\n");
			if ((p1 == KEY_BACK) && (pstContents->eOptItem != NxUiScheduleItem_SetOptItem_ALL))
			{
				nx_schedule_Opt_DestroyListPopup();
				nx_schedule_Opt_ReCreateMenu(pstContents);
			}
			else
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
