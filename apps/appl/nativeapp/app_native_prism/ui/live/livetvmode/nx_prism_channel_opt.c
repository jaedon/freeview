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
 * @file	  		nx_prism_channellist_opt.c
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
#include <nx_main.h>
#include <nx_core.h>
#include <nx_port.h>

#include <nx_common.h>
#include <nx_core_message.h>
#include <nx_prism_app.h>
#include <nx_prism_channellist.h>

#include "nx_prism_definition.h"


/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/
#define	NX_PRISM_CHLIST_OPTION_ITEM_ID						(eMenuFrame_Window_CHList + 256)

#define	NX_PRISM_CHLIST_OPTION_EXPIRE_TIMER_ID				(NX_PRISM_CHLIST_OPTION_ITEM_ID + 1)
#define	NX_PRISM_CHLIST_OPTION_EXPIRE_TIME					(30 * NX_COMMON_TIME_SECOND)

#define	NX_PRISM_CHLIST_OPTION_SELECTLIST_MAX				5

#define	NX_PRISM_CHLIST_OPTION_LIST_Y2						673
#define	NX_PRISM_CHLIST_OPTION_LIST_INFOHUB_Y2				620


/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/
typedef struct
{
	HUINT32				ulSvcUid;
	NX_ChListGroup_t	eCurGroup;

	// Option Item
	HUINT8			**szItemTextList;
	HUINT8			**szImageNItemTextList;
	HUINT8			**szImageCItemTextList;
	NX_Util_OPTIONList_t	stOptList;

	NX_UI_CallerType_t		eDispOptType;
	eNxChlist_OptItem_t		eCurOptItem;
} NxUiChannelListOptContents_t;


/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/

static NxUiChannelListOptContents_t		s_UiChannelListOptContents;
static Nx_PrismListDialog_Content_t		s_stChListOptContents;


/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/

static ONDK_Result_t	nx_channellistOpt_DisplayListPopup(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_channellistOpt_DestroyListPopup(void);

/* Channel List Option - Change Group */
static void				nx_channellistOpt_CreateOptionChangeGroupListPopup(NxUiChannelListOptContents_t *pstContents);
static ONDK_Result_t	nx_channellistOpt_OptionActionChangeGroup(NxUiChannelListOptContents_t *pstContents, HUINT32 ulCurIdx);
static ONDK_Result_t	nx_channellistOpt_OptionChangeGroupInitData(NxUiChannelListOptContents_t *pstContents);
static void				nx_channellistOpt_OptionLoadGroupInitDataDefault(NxUiChannelListOptContents_t *pstContents);


/* Channel List Option - Change Alphabet Group */
static void				nx_channellistOpt_CreateOptionChangeAlphabetGroupListPopup(NxUiChannelListOptContents_t *pstContents);
static ONDK_Result_t	nx_channellistOpt_OptionActionChangeAlphabetGroup(NxUiChannelListOptContents_t *pstContents, HUINT32 ulCurIdx);
static void				nx_channellistOpt_OptionLoadAlphabetGroupInitDataDefault(NxUiChannelListOptContents_t *pstContents);
static ONDK_Result_t	nx_channellistOpt_OptionChangeAlphabetGroupInitData(NxUiChannelListOptContents_t *pstContents);

/* Channel List Option - Add Favourite */
static ONDK_Result_t	nx_channellistOpt_OptionActionAddFavourite(NxUiChannelListOptContents_t *pstContents);

/* Channel List Option - Sort */
static void				nx_channellistOpt_CreateOptionSortListPopup(NxUiChannelListOptContents_t *pstContents);
static ONDK_Result_t	nx_channellistOpt_OptionActionSort(NxUiChannelListOptContents_t *pstContents,  HUINT32 ulCurIdx);
static ONDK_Result_t	nx_channellistOpt_OptionSortInitData(NxUiChannelListOptContents_t *pstContents);

/* Channel List Option */
static void				nx_channellistOpt_CreateOptionListPopup(NxUiChannelListOptContents_t *pstContents, HUINT32 ulCurItemIdx);
static ONDK_Result_t	nx_channellistOpt_OptionMakeListChangeGroup(NxUiChannelListOptContents_t *pstContents);
static ONDK_Result_t	nx_channellistOpt_OptionMakeListChangeAlphabetGroup(NxUiChannelListOptContents_t *pstContents);
static ONDK_Result_t	nx_channellistOpt_OptionMakeListSort(NxUiChannelListOptContents_t *pstContents);
static ONDK_Result_t	nx_channellistOpt_OptionMakeListAddFavourite(NxUiChannelListOptContents_t *pstContents);
static ONDK_Result_t	nx_channellistOpt_OptionActionDelete(NxUiChannelListOptContents_t *pstContents);
static ONDK_Result_t	nx_channellistOpt_OptionActionLockUnLock(NxUiChannelListOptContents_t *pstContents, HBOOL bLock);
static ONDK_Result_t	nx_channellistOpt_OptionActionRename(NxUiChannelListOptContents_t *pstContents);
static ONDK_Result_t	nx_channellistOpt_OptionChangedItemValue(NxUiChannelListOptContents_t *pstContents, HUINT32 ulCurIdx);

/* Channel List Option */
static inline NxUiChannelListOptContents_t* nx_channellistOpt_GetContents(void);
static ONDK_Result_t	nx_channellistOpt_ResetOptionExpireTimer(void);
static ONDK_Result_t	nx_channellistOpt_ChangedOptionItemValue(NxUiChannelListOptContents_t *pstContents, HUINT32 ulCurIdx);
static ONDK_Result_t	nx_channellistOpt_AddOptionDataForChanneList(NxUiChannelListOptContents_t *pstContents);
static ONDK_Result_t	nx_channellistOpt_AddOptionDataForInforHub(NxUiChannelListOptContents_t *pstContents);
static ONDK_Result_t	nx_channellistOpt_InitOptionData(NxUiChannelListOptContents_t *pstContents);
static ONDK_Result_t	nx_channellistOpt_ReCreateMenu(NxUiChannelListOptContents_t *pstContents);

static ONDK_Result_t	nx_channellistOpt_MsgGwmCreate(NxUiChannelListOptContents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	nx_channellistOpt_MsgGwmDestroy(NxUiChannelListOptContents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_channellistOpt_MsgGwmHapiMsg(NxUiChannelListOptContents_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_channellistOpt_MsgGwmKeyDown(NxUiChannelListOptContents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_channellistOpt_MsgGwmTimer(NxUiChannelListOptContents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);


/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/

static ONDK_Result_t	nx_channellistOpt_DisplayListPopup(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return	NX_PRISM_COMP_ListDlg_ProtoProc(&s_stChListOptContents, message, hAction, p1, p2, p3);
}


static ONDK_Result_t	nx_channellistOpt_DestroyListPopup(void)
{
	return ONDK_GWM_APP_Destroy(nx_channellistOpt_DisplayListPopup);
}


/******************************************************************************/
#define __Option_ChangeGroup_Functions________________________________________
/******************************************************************************/

static void		nx_channellistOpt_CreateOptionChangeGroupListPopup(NxUiChannelListOptContents_t *pstContents)
{
	HINT32		lLineCnt = 0;
	HUINT32		ulTextListNum = 0, ulIndex = 0, ulAttribute = 0, i = 0, ulMaxListNum = 0;
	HUINT8		**pptextList = NULL;
	NX_ChListGroup_t	eCurGroup = eNxChListGroup_TV;
	ONDK_Rect_t	stRect = { 0, 0, 0, 0 }, stListRect = { 0, 0, 0, 0 };


	if (pstContents->stOptList.ulOptItemNum == 0 )
	{
		NX_APP_Info("Error!!! Please Check added Option item !!\n");
		return;
	}

	ulTextListNum = pstContents->stOptList.ulOptItemNum;
	if (pstContents->szItemTextList == NULL)
	{
		/* Make Option Test List */
		pstContents->szItemTextList = NX_PRISM_UTIL_OptionMakeChannlGroupItemTextList(eNxUtil_OptType_TEXT, &pstContents->stOptList);
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

	/* Find Current group index for List Index */
	ulIndex	= 0;
	eCurGroup = pstContents->eCurGroup;

	// eNxChListGroup_AlphabetAll ~ eNxChListGroup_AlphabetEtc is included in the eNxChListGroup_ALPHABET Group
	if ((eCurGroup >= eNxChListGroup_AlphabetAll) && (eCurGroup <= eNxChListGroup_AlphabetEtc))
	{
		eCurGroup = eNxChListGroup_ALPHABET;
	}

	for (i = 0; i<pstContents->stOptList.ulOptItemNum; i++)
	{
		if (pstContents->stOptList.aeOptItemList[i] == (HUINT32)eCurGroup)
		{
			ulIndex = i;
			break;
		}
	}

	lLineCnt = NX_PRISM_UTIL_GetTextLineCount(eFont_SystemNormal, NX_PRISM_FONT_SIZE_22, ONDK_GetString(RES_MESG_5650_ID), NX_PRISM_UI_DLG_MULTISEL_MSG_TEXT_W);

	// Set Text List
	ulMaxListNum = (ulTextListNum > NX_PRISM_CHLIST_OPTION_SELECTLIST_MAX) ? NX_PRISM_CHLIST_OPTION_SELECTLIST_MAX : ulTextListNum;
	ulAttribute = (eListDlg_OSD_USER | eListDlg_OSD_BG | eListDlg_Title | eListDlg_SelectItemApp);
	stRect = NX_PRISM_DIALOGUE_MultiSelectMessage_GetRect(eNX_DLG_MultiSel_List, lLineCnt, ulMaxListNum);
	if (ulAttribute & eListDlg_OSD_USER)
	{
		stListRect = NX_PRISM_DIALOGUE_MultiSelectMessage_GetRect(eNX_DLG_MultiSel_OSD, lLineCnt, ulMaxListNum);
		stRect.x = stListRect.x + stRect.x;
		stRect.y = stListRect.y + stRect.y;
	}

	NX_PRISM_COMP_ListDlg_SetListPopup(&s_stChListOptContents, ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis()),
							&stRect, (NX_PRISM_UI_DLG_MULTISEL_MSG_LIST_H + NX_PRISM_UI_DLG_MULTISEL_MSG_CELL_MG),
							ulAttribute, ulTextListNum, ulMaxListNum, ulIndex, (HUINT32)pptextList);

	NX_PRISM_COMP_ListDlg_SetFontSize(&s_stChListOptContents, NX_PRISM_FONT_SIZE_22);
	NX_PRISM_COMP_ListDlg_SetTitle(&s_stChListOptContents, ONDK_GetString(RES_MESG_5650_ID));

	if (ulAttribute & eListDlg_OSD_USER)
	{
		NX_PRISM_COMP_ListDlg_SetExternalArea(&s_stChListOptContents, &stListRect);
	}

	/* Create Pop Up */
	ONDK_GWM_APP_Create (ONDK_UPPER_WINDOW, "nx_channellistOpt_DisplayGroupListPopup", APP_DEFAULT_POPUP_PRIORITY, nx_channellistOpt_DisplayListPopup, 0, 0, 0, 0);

	//ONDK_GWM_KillTimer(NX_PRISM_CHLIST_EXIT_TIMER_ID);
}


static ONDK_Result_t	nx_channellistOpt_OptionActionChangeGroup(NxUiChannelListOptContents_t *pstContents, HUINT32 ulCurIdx)
{
	NX_UI_CallerType_t		eDispOptType = eNX_UCT_None;
	eNxChlist_OptItem_t		eCurOptItem = eNxChlist_OptItem_All;
	NX_ChListGroup_t 		eChGroupType = eNxChListGroup_TV;

	if (pstContents->stOptList.ulOptItemNum == 0)
	{
		NX_APP_Info("Error!!! Please Check Option item was Zero!!\n");
		return	ONDK_MESSAGE_BREAK;
	}

	eChGroupType = (NX_ChListGroup_t)pstContents->stOptList.aeOptItemList[ulCurIdx];
	if (eChGroupType == eNxChListGroup_ALPHABET)
	{
		nx_channellistOpt_OptionMakeListChangeAlphabetGroup(pstContents);
		return ONDK_RET_OK;
	}

	switch (eChGroupType)
	{
		case eNxChListGroup_TV:
		case eNxChListGroup_RADIO:
		case eNxChListGroup_HDTV:
		case eNxChListGroup_FTA:
		case eNxChListGroup_CAS:
		case eNxChListGroup_AlphabetAll:
		case eNxChListGroup_AlphabetA:
		case eNxChListGroup_AlphabetB:
		case eNxChListGroup_AlphabetC:
		case eNxChListGroup_AlphabetD:
		case eNxChListGroup_AlphabetE:
		case eNxChListGroup_AlphabetF:
		case eNxChListGroup_AlphabetG:
		case eNxChListGroup_AlphabetH:
		case eNxChListGroup_AlphabetI:
		case eNxChListGroup_AlphabetJ:
		case eNxChListGroup_AlphabetK:
		case eNxChListGroup_AlphabetL:
		case eNxChListGroup_AlphabetM:
		case eNxChListGroup_AlphabetN:
		case eNxChListGroup_AlphabetO:
		case eNxChListGroup_AlphabetP:
		case eNxChListGroup_AlphabetQ:
		case eNxChListGroup_AlphabetR:
		case eNxChListGroup_AlphabetS:
		case eNxChListGroup_AlphabetT:
		case eNxChListGroup_AlphabetU:
		case eNxChListGroup_AlphabetV:
		case eNxChListGroup_AlphabetW:
		case eNxChListGroup_AlphabetX:
		case eNxChListGroup_AlphabetY:
		case eNxChListGroup_AlphabetZ:
		case eNxChListGroup_AlphabetDigit:
		case eNxChListGroup_AlphabetEtc:
		case eNxChListGroup_Favorite1:
		case eNxChListGroup_Favorite2:
		case eNxChListGroup_Favorite3:
		case eNxChListGroup_Favorite4:
		case eNxChListGroup_Favorite5:
			break;

		default:
			NX_APP_Info("Error!!! Not Support Group Item [0x%x] !!\n", eChGroupType);
			break;
	}

	eDispOptType = pstContents->eDispOptType;
	eCurOptItem = pstContents->eCurOptItem;
	ONDK_GWM_SendMessage(NULL, MSG_APP_POPUP_OPT_VALUE_CHANGED, (Handle_t)NULL, eDispOptType, eCurOptItem, eChGroupType);
	ONDK_GWM_APP_Destroy(0);

	NX_COMMON_NOT_USED_PARAM(pstContents);
	return ONDK_RET_OK;
}


static ONDK_Result_t	nx_channellistOpt_OptionChangeGroupInitData(NxUiChannelListOptContents_t *pstContents)
{
	DxOperator_e	eOpType = eDxOPERATOR_None;


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
	pstContents->eCurOptItem = eNxChlist_OptItem_Group;

	// Operator Type
	NX_SYSTEM_GetOperatorType(&eOpType);
	switch (eOpType)
	{
		// TODO: Add Other Change Group for Operator if you need

		default:
			nx_channellistOpt_OptionLoadGroupInitDataDefault(pstContents);
			break;
	}

	// TODO: Add Project config if you want to do

	return ONDK_RET_OK;
}


static void		nx_channellistOpt_OptionLoadGroupInitDataDefault(NxUiChannelListOptContents_t *pstContents)
{
	HUINT32			ulCount = 0;

	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_TV);
	if (ulCount > 0)
		NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxChListGroup_TV);

	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_RADIO);
	if (ulCount > 0)
		NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxChListGroup_RADIO);

	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_HDTV);
	if (ulCount > 0)
		NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxChListGroup_HDTV);

	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_FTA);
	if (ulCount > 0)
		NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxChListGroup_FTA);

	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_CAS);
	if (ulCount > 0)
		NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxChListGroup_CAS);

	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_ALPHABET);
	if (ulCount > 0)
		NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxChListGroup_ALPHABET);

	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_Favorite1);
	if (ulCount > 0)
		NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxChListGroup_Favorite1);

	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_Favorite2);
	if (ulCount > 0)
		NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxChListGroup_Favorite2);

	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_Favorite3);
	if (ulCount > 0)
		NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxChListGroup_Favorite3);

	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_Favorite4);
	if (ulCount > 0)
		NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxChListGroup_Favorite4);

	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_Favorite5);
	if (ulCount > 0)
		NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxChListGroup_Favorite5);

}


/******************************************************************************/
#define __Option_ChangeAlphabetGroup_Functions_________________________________
/******************************************************************************/

static void		nx_channellistOpt_CreateOptionChangeAlphabetGroupListPopup(NxUiChannelListOptContents_t *pstContents)
{
	HINT32		lLineCnt = 0;
	HUINT32		ulTextListNum = 0, ulIndex = 0, ulAttribute = 0, i = 0, ulMaxListNum = 0;
	HUINT8		**pptextList = NULL;
	NX_ChListGroup_t	eCurGroup = eNxChListGroup_TV;
	ONDK_Rect_t	stRect = { 0, 0, 0, 0 }, stListRect = { 0, 0, 0, 0 };


	if (pstContents->stOptList.ulOptItemNum == 0 )
	{
		NX_APP_Info("Error!!! Please Check added Option item !!\n");
		return;
	}

	ulTextListNum = pstContents->stOptList.ulOptItemNum;
	if (pstContents->szItemTextList == NULL)
	{
		/* Make Option Test List */
		pstContents->szItemTextList = NX_PRISM_UTIL_OptionMakeChannlGroupItemTextList(eNxUtil_OptType_TEXT, &pstContents->stOptList);
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

	/* Find Current group index for List Index */
	ulIndex	= 0;
	eCurGroup = pstContents->eCurGroup;
	for (i = 0; i<pstContents->stOptList.ulOptItemNum; i++)
	{
		if (pstContents->stOptList.aeOptItemList[i] == (HUINT32)eCurGroup)
		{
			ulIndex = i;
			break;
		}
	}

	lLineCnt = NX_PRISM_UTIL_GetTextLineCount(eFont_SystemNormal, NX_PRISM_FONT_SIZE_22, ONDK_GetString(RES_MESG_5650_ID), NX_PRISM_UI_DLG_MULTISEL_MSG_TEXT_W);

	// Set Text List
	ulMaxListNum = (ulTextListNum > NX_PRISM_CHLIST_OPTION_SELECTLIST_MAX) ? NX_PRISM_CHLIST_OPTION_SELECTLIST_MAX : ulTextListNum;
	ulAttribute = (eListDlg_OSD_USER | eListDlg_OSD_BG | eListDlg_Title | eListDlg_SelectItemApp);
	stRect = NX_PRISM_DIALOGUE_MultiSelectMessage_GetRect(eNX_DLG_MultiSel_List, lLineCnt, ulMaxListNum);
	if (ulAttribute & eListDlg_OSD_USER)
	{
		stListRect = NX_PRISM_DIALOGUE_MultiSelectMessage_GetRect(eNX_DLG_MultiSel_OSD, lLineCnt, ulMaxListNum);
		stRect.x = stListRect.x + stRect.x;
		stRect.y = stListRect.y + stRect.y;
	}

	NX_PRISM_COMP_ListDlg_SetListPopup(&s_stChListOptContents, ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis()),
							&stRect, (NX_PRISM_UI_DLG_MULTISEL_MSG_LIST_H + NX_PRISM_UI_DLG_MULTISEL_MSG_CELL_MG),
							ulAttribute, ulTextListNum, ulMaxListNum, ulIndex, (HUINT32)pptextList);

	NX_PRISM_COMP_ListDlg_SetFontSize(&s_stChListOptContents, NX_PRISM_FONT_SIZE_22);
	NX_PRISM_COMP_ListDlg_SetTitle(&s_stChListOptContents, ONDK_GetString(RES_MESG_5650_ID));

	if (ulAttribute & eListDlg_OSD_USER)
	{
		NX_PRISM_COMP_ListDlg_SetExternalArea(&s_stChListOptContents, &stListRect);
	}

	/* Create Pop Up */
	ONDK_GWM_APP_Create (ONDK_UPPER_WINDOW, "nx_channellistOpt_DisplayGroupListPopup", APP_DEFAULT_POPUP_PRIORITY, nx_channellistOpt_DisplayListPopup, 0, 0, 0, 0);

	//ONDK_GWM_KillTimer(NX_PRISM_CHLIST_EXIT_TIMER_ID);
}


static ONDK_Result_t	nx_channellistOpt_OptionActionChangeAlphabetGroup(NxUiChannelListOptContents_t *pstContents, HUINT32 ulCurIdx)
{
	NX_UI_CallerType_t		eDispOptType = eNX_UCT_None;
	eNxChlist_OptItem_t		eCurOptItem = eNxChlist_OptItem_All;
	NX_ChListGroup_t 		eChGroupType = eNxChListGroup_TV;

	if (pstContents->stOptList.ulOptItemNum == 0)
	{
		NX_APP_Info("Error!!! Please Check Option item was Zero!!\n");
		return	ONDK_MESSAGE_BREAK;
	}

	eChGroupType = (NX_ChListGroup_t)pstContents->stOptList.aeOptItemList[ulCurIdx];
	switch (eChGroupType)
	{
		case eNxChListGroup_AlphabetAll:
		case eNxChListGroup_AlphabetA:
		case eNxChListGroup_AlphabetB:
		case eNxChListGroup_AlphabetC:
		case eNxChListGroup_AlphabetD:
		case eNxChListGroup_AlphabetE:
		case eNxChListGroup_AlphabetF:
		case eNxChListGroup_AlphabetG:
		case eNxChListGroup_AlphabetH:
		case eNxChListGroup_AlphabetI:
		case eNxChListGroup_AlphabetJ:
		case eNxChListGroup_AlphabetK:
		case eNxChListGroup_AlphabetL:
		case eNxChListGroup_AlphabetM:
		case eNxChListGroup_AlphabetN:
		case eNxChListGroup_AlphabetO:
		case eNxChListGroup_AlphabetP:
		case eNxChListGroup_AlphabetQ:
		case eNxChListGroup_AlphabetR:
		case eNxChListGroup_AlphabetS:
		case eNxChListGroup_AlphabetT:
		case eNxChListGroup_AlphabetU:
		case eNxChListGroup_AlphabetV:
		case eNxChListGroup_AlphabetW:
		case eNxChListGroup_AlphabetX:
		case eNxChListGroup_AlphabetY:
		case eNxChListGroup_AlphabetZ:
		case eNxChListGroup_AlphabetDigit:
		case eNxChListGroup_AlphabetEtc:
			break;
		default:
			NX_APP_Info("Error!!! Not Support Alphabet Group Item [0x%x] !!\n", eChGroupType);
			break;
	}

	eDispOptType = pstContents->eDispOptType;
	eCurOptItem = pstContents->eCurOptItem;
	ONDK_GWM_SendMessage(NULL, MSG_APP_POPUP_OPT_VALUE_CHANGED, (Handle_t)NULL, eDispOptType, eCurOptItem, eChGroupType);

	NX_COMMON_NOT_USED_PARAM(pstContents);
	return ONDK_RET_OK;
}


static void		nx_channellistOpt_OptionLoadAlphabetGroupInitDataDefault(NxUiChannelListOptContents_t *pstContents)
{
	HUINT32			ulCount = 0;

	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_AlphabetAll);
	if (ulCount > 0)	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxChListGroup_AlphabetAll);
	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_AlphabetA);
	if (ulCount > 0)	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxChListGroup_AlphabetA);
	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_AlphabetB);
	if (ulCount > 0)	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxChListGroup_AlphabetB);
	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_AlphabetC);
	if (ulCount > 0)	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxChListGroup_AlphabetC);
	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_AlphabetD);
	if (ulCount > 0)	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxChListGroup_AlphabetD);
	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_AlphabetE);
	if (ulCount > 0)	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxChListGroup_AlphabetE);
	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_AlphabetF);
	if (ulCount > 0)	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxChListGroup_AlphabetF);
	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_AlphabetG);
	if (ulCount > 0)	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxChListGroup_AlphabetG);
	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_AlphabetH);
	if (ulCount > 0)	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxChListGroup_AlphabetH);
	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_AlphabetI);
	if (ulCount > 0)	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxChListGroup_AlphabetI);
	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_AlphabetJ);
	if (ulCount > 0)	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxChListGroup_AlphabetJ);
	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_AlphabetK);
	if (ulCount > 0)	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxChListGroup_AlphabetK);
	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_AlphabetL);
	if (ulCount > 0)	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxChListGroup_AlphabetL);
	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_AlphabetM);
	if (ulCount > 0)	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxChListGroup_AlphabetM);
	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_AlphabetN);
	if (ulCount > 0)	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxChListGroup_AlphabetN);
	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_AlphabetO);
	if (ulCount > 0)	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxChListGroup_AlphabetO);
	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_AlphabetP);
	if (ulCount > 0)	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxChListGroup_AlphabetP);
	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_AlphabetQ);
	if (ulCount > 0)	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxChListGroup_AlphabetQ);
	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_AlphabetR);
	if (ulCount > 0)	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxChListGroup_AlphabetR);
	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_AlphabetS);
	if (ulCount > 0)	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxChListGroup_AlphabetS);
	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_AlphabetT);
	if (ulCount > 0)	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxChListGroup_AlphabetT);
	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_AlphabetU);
	if (ulCount > 0)	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxChListGroup_AlphabetU);
	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_AlphabetV);
	if (ulCount > 0)	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxChListGroup_AlphabetV);
	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_AlphabetW);
	if (ulCount > 0)	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxChListGroup_AlphabetW);
	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_AlphabetX);
	if (ulCount > 0)	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxChListGroup_AlphabetX);
	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_AlphabetY);
	if (ulCount > 0)	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxChListGroup_AlphabetY);
	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_AlphabetZ);
	if (ulCount > 0)	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxChListGroup_AlphabetZ);
	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_AlphabetDigit);
	if (ulCount > 0)	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxChListGroup_AlphabetDigit);
	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_AlphabetEtc);
	if (ulCount > 0)	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxChListGroup_AlphabetEtc);

}


static ONDK_Result_t	nx_channellistOpt_OptionChangeAlphabetGroupInitData(NxUiChannelListOptContents_t *pstContents)
{
	DxOperator_e	eOpType = eDxOPERATOR_None;


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
	pstContents->eCurOptItem = eNxChlist_OptItem_Group;

	// Operator Type
	NX_SYSTEM_GetOperatorType(&eOpType);
	switch (eOpType)
	{
		// TODO: Add Other Change Group for Operator if you need

		default:
			nx_channellistOpt_OptionLoadAlphabetGroupInitDataDefault(pstContents);
			break;
	}

	// TODO: Add Project config if you want to do

	return ONDK_RET_OK;
}


/******************************************************************************/
#define __Option_AddFavourite_Functions________________________________________
/******************************************************************************/

static ONDK_Result_t	nx_channellistOpt_OptionActionAddFavourite(NxUiChannelListOptContents_t *pstContents)
{
	// Action Favourite Proc
	NX_COMMON_NOT_USED_PARAM(pstContents);
	return ONDK_RET_OK;
}


/******************************************************************************/
#define __Option_Sort_Functions______________________________________________
/******************************************************************************/

static void		nx_channellistOpt_CreateOptionSortListPopup(NxUiChannelListOptContents_t *pstContents)
{
	HINT32		lLineCnt = 0;
	HUINT32		ulTextListNum = 0, ulIndex = 0, ulAttribute = 0, ulCurSort = 0, i = 0;
	HUINT8		**pptextList = NULL;
	ONDK_Rect_t	stRect = { 0, 0, 0, 0 }, stListRect = { 0, 0, 0, 0 };


	if (pstContents->stOptList.ulOptItemNum == 0 )
	{
		NX_APP_Info("Error!!! Please Check added Option item !!\n");
		return;
	}

	ulTextListNum = pstContents->stOptList.ulOptItemNum;
	ulIndex	= 0;
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

	ulCurSort = NX_PRISM_CHANNELLIST_GetCurrentSort();

	for (i = 0; i<pstContents->stOptList.ulOptItemNum; i++)
	{
		if (pstContents->stOptList.aeOptItemList[i] == ulCurSort)
		{
			ulIndex = i;
			break;
		}
	}

	lLineCnt = NX_PRISM_UTIL_GetTextLineCount(eFont_SystemNormal, NX_PRISM_FONT_SIZE_22, ONDK_GetString(RES_MESG_5651_ID), NX_PRISM_UI_DLG_MULTISEL_MSG_TEXT_W);
	ulAttribute = (eListDlg_OSD_USER | eListDlg_OSD_BG | eListDlg_Title | eListDlg_SelectItemApp);

	// Set Text List
	stRect = NX_PRISM_DIALOGUE_MultiSelectMessage_GetRect(eNX_DLG_MultiSel_List, lLineCnt, ulTextListNum);
	if (ulAttribute & eListDlg_OSD_USER)
	{
		stListRect = NX_PRISM_DIALOGUE_MultiSelectMessage_GetRect(eNX_DLG_MultiSel_OSD, lLineCnt, ulTextListNum);
		stRect.x = stListRect.x + stRect.x;
		stRect.y = stListRect.y + stRect.y;
	}

	NX_PRISM_COMP_ListDlg_SetListPopup(&s_stChListOptContents, ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis()),
							&stRect, (NX_PRISM_UI_DLG_MULTISEL_MSG_LIST_H + NX_PRISM_UI_DLG_MULTISEL_MSG_CELL_MG),
							ulAttribute, ulTextListNum, ulTextListNum, ulIndex, (HUINT32)pptextList);

	NX_PRISM_COMP_ListDlg_SetFontSize(&s_stChListOptContents, NX_PRISM_FONT_SIZE_22);
	NX_PRISM_COMP_ListDlg_SetTitle(&s_stChListOptContents, ONDK_GetString(RES_MESG_5651_ID));

	if (ulAttribute & eListDlg_OSD_USER)
	{
		NX_PRISM_COMP_ListDlg_SetExternalArea(&s_stChListOptContents, &stListRect);
	}

	/* Create Pop Up */
	ONDK_GWM_APP_Create (ONDK_UPPER_WINDOW, "nx_channellistOpt_DisplaySortListPopup", APP_DEFAULT_POPUP_PRIORITY, nx_channellistOpt_DisplayListPopup, 0, 0, 0, 0);

	//ONDK_GWM_KillTimer(NX_PRISM_CHLIST_EXIT_TIMER_ID);
}


static ONDK_Result_t	nx_channellistOpt_OptionActionSort(NxUiChannelListOptContents_t *pstContents, HUINT32 ulCurIdx)
{
	NX_UI_CallerType_t		eDispOptType = eNX_UCT_None;
	eNxChlist_OptItem_t		eCurOptItem = eNxChlist_OptItem_All;
	eNxUiUtil_OptItem_t		eOptItem = eNxUtil_OptItem_None;

	if (pstContents->stOptList.ulOptItemNum == 0)
	{
		NX_APP_Info("Error!!! Please Check Option item was Zero!!\n");
		return	ONDK_MESSAGE_BREAK;
	}

	eOptItem = pstContents->stOptList.aeOptItemList[ulCurIdx];
	switch (eOptItem)
	{
		case eNxUtil_OptItem_SortNumber:
			break;
		case eNxUtil_OptItem_SortName:
			break;
		default:
			NX_APP_Info("Error!!! Not Support Sort Item [%d] !!\n", eOptItem);
			break;
	}

	eDispOptType = pstContents->eDispOptType;
	eCurOptItem = pstContents->eCurOptItem;
	ONDK_GWM_SendMessage(NULL, MSG_APP_POPUP_OPT_VALUE_CHANGED, (Handle_t)NULL, eDispOptType, eCurOptItem, eOptItem);

	NX_COMMON_NOT_USED_PARAM(pstContents);
	return ONDK_RET_OK;
}


static ONDK_Result_t	nx_channellistOpt_OptionSortInitData(NxUiChannelListOptContents_t *pstContents)
{
	DxOperator_e	eOpType = eDxOPERATOR_None;


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
	pstContents->eCurOptItem = eNxChlist_OptItem_Sort;

	// Operator Type
	NX_SYSTEM_GetOperatorType(&eOpType);
	switch (eOpType)
	{
		default:
			NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxUtil_OptItem_SortNumber);
			NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxUtil_OptItem_SortName);
			break;
	}

	// TODO: Add Project config if you want to do

	return ONDK_RET_OK;
}


/******************************************************************************/
#define __Option_Functions______________________________________________
/******************************************************************************/

static void		nx_channellistOpt_CreateOptionListPopup(NxUiChannelListOptContents_t *pstContents, HUINT32 ulCurItemIdx)
{
	HINT32		y = 0, h = 0, lOffset_startY = 0;
	HUINT32		ulTextListNum = 0, ulAttribute = 0;
	HUINT8		**pptextList = NULL, **ppImageListOfItem = NULL;
	ONDK_Rect_t	stRect;


	if (pstContents->stOptList.ulOptItemNum == 0)
	{
		NX_APP_Info("Error!!! Please Check added Option item !!\n");
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

	ulAttribute = (eListDlg_OSD_USER | eListDlg_OSD_BG | eListDlg_IMAGE_OptionLApp);
	// Set Text List
	h = (NX_PRISM_UI_OPT_POPUP_CELL_HEIGHT * ulTextListNum) + (NX_PRISM_UI_OPT_POPUP_CELL_MG * (ulTextListNum-1)) + NX_PRISM_UI_OPT_POPUP_POINTER_H;
	lOffset_startY = (pstContents->eDispOptType == eNX_UCT_Infohub) ? NX_PRISM_CHLIST_OPTION_LIST_INFOHUB_Y2 : NX_PRISM_CHLIST_OPTION_LIST_Y2 ;
	y = lOffset_startY - h - NX_PRISM_UI_OPT_POPUP_POINTER_H;
	y = (y < NX_PRISM_UI_SET_WINDOW_Y) ? NX_PRISM_UI_SET_WINDOW_Y : y ;
	stRect = ONDK_Rect(NX_PRISM_UI_OPT_POPUP_LIST_L_X, y, NX_PRISM_UI_OPT_POPUP_LIST_WIDTH, h);
	NX_PRISM_COMP_ListDlg_SetListPopup(&s_stChListOptContents, ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis()),
							&stRect, (NX_PRISM_UI_OPT_POPUP_CELL_HEIGHT + NX_PRISM_UI_OPT_POPUP_CELL_MG),
							ulAttribute, ulTextListNum, ulTextListNum, ulCurItemIdx, (HUINT32)pptextList);

	NX_PRISM_COMP_ListDlg_SetFontSize(&s_stChListOptContents, NX_PRISM_FONT_SIZE_22);

	if (ulAttribute & eListDlg_OSD_USER)
	{
		stRect = ONDK_Rect(NX_PRISM_UI_OPT_POPUP_L_X, NX_PRISM_UI_OPT_POPUP_L_Y,
							NX_PRISM_UI_OPT_POPUP_WIDTH, NX_PRISM_UI_OPT_POPUP_HEIGHT);
		NX_PRISM_COMP_ListDlg_SetExternalArea(&s_stChListOptContents, &stRect);
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

	NX_PRISM_COMP_ListDlg_SetImageList(&s_stChListOptContents, (HUINT32)ppImageListOfItem);

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

	NX_PRISM_COMP_ListDlg_SetImageExtList(&s_stChListOptContents, (HUINT32)ppImageListOfItem);

	/* Create Pop Up */
	ONDK_GWM_APP_Create (ONDK_UPPER_WINDOW, "nx_channellistOpt_DisplayListPopup", APP_DEFAULT_POPUP_PRIORITY, nx_channellistOpt_DisplayListPopup, 0, 0, 0, 0);

//	ONDK_GWM_KillTimer(NX_PRISM_CHLIST_EXIT_TIMER_ID);
}


static ONDK_Result_t	nx_channellistOpt_OptionMakeListChangeGroup(NxUiChannelListOptContents_t *pstContents)
{
	nx_channellistOpt_OptionChangeGroupInitData(pstContents);
	nx_channellistOpt_CreateOptionChangeGroupListPopup(pstContents);

	NX_COMMON_NOT_USED_PARAM(pstContents);
	return ONDK_RET_OK;
}


static ONDK_Result_t	nx_channellistOpt_OptionMakeListChangeAlphabetGroup(NxUiChannelListOptContents_t *pstContents)
{
	nx_channellistOpt_OptionChangeAlphabetGroupInitData(pstContents);
	nx_channellistOpt_CreateOptionChangeAlphabetGroupListPopup(pstContents);

	NX_COMMON_NOT_USED_PARAM(pstContents);
	return ONDK_RET_OK;
}


static ONDK_Result_t	nx_channellistOpt_OptionMakeListSort(NxUiChannelListOptContents_t *pstContents)
{
	nx_channellistOpt_OptionSortInitData(pstContents);
	nx_channellistOpt_CreateOptionSortListPopup(pstContents);

	NX_COMMON_NOT_USED_PARAM(pstContents);
	return ONDK_RET_OK;
}


static ONDK_Result_t	nx_channellistOpt_OptionMakeListAddFavourite(NxUiChannelListOptContents_t *pstContents)
{
	HUINT32					ulSvcUid = 0;
	NX_ChListGroup_t		eCurGroup = eNxChListGroup_TV;
	NX_UI_CallerType_t		eDispOptType = eNX_UCT_Chlist;

	ulSvcUid = pstContents->ulSvcUid;
	eCurGroup = pstContents->eCurGroup;
	eDispOptType= pstContents->eDispOptType;

	ONDK_GWM_APP_Destroy(0);

	ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, "NX_PRISM_CHANNELLIST_ADDFAV_Proc", APP_DEFAULT_PRIORITY,
				NX_PRISM_CHANNELLIST_ADDFAV_Proc, 0, eDispOptType, ulSvcUid, eCurGroup);

	NX_COMMON_NOT_USED_PARAM(pstContents);

	return ONDK_RET_OK;
}


static ONDK_Result_t	nx_channellistOpt_OptionActionDelete(NxUiChannelListOptContents_t *pstContents)
{
	NX_UI_CallerType_t		eDispOptType = eNX_UCT_None;

	eDispOptType = pstContents->eDispOptType;
	ONDK_GWM_SendMessage(NULL, MSG_APP_POPUP_OPT_VALUE_CHANGED, (Handle_t)NULL, eDispOptType, eNxChlist_OptItem_Delete, eNxUtil_OptItem_Delete);
	ONDK_GWM_APP_Destroy(0);

	NX_COMMON_NOT_USED_PARAM(pstContents);
	return ONDK_RET_OK;
}


static ONDK_Result_t	nx_channellistOpt_OptionActionLockUnLock(NxUiChannelListOptContents_t *pstContents, HBOOL bLock)
{
	NX_UI_CallerType_t		eDispOptType = eNX_UCT_None;
	eNxUiUtil_OptItem_t		eUtilOptItem = eNxUtil_OptItem_Lock;

	eDispOptType = pstContents->eDispOptType;
	eUtilOptItem = (bLock == TRUE) ? eNxUtil_OptItem_Lock : eNxUtil_OptItem_UnLock ;
	ONDK_GWM_SendMessage(NULL, MSG_APP_POPUP_OPT_VALUE_CHANGED, (Handle_t)NULL, eDispOptType, eNxChlist_OptItem_Lock, eUtilOptItem);
	ONDK_GWM_APP_Destroy(0);

	NX_COMMON_NOT_USED_PARAM(pstContents);
	return ONDK_RET_OK;
}


static ONDK_Result_t	nx_channellistOpt_OptionActionRename(NxUiChannelListOptContents_t *pstContents)
{
	NX_UI_CallerType_t		eDispOptType = eNX_UCT_None;

	eDispOptType = pstContents->eDispOptType;

	ONDK_GWM_SendMessage(NULL, MSG_APP_POPUP_OPT_VALUE_CHANGED, (Handle_t)NULL, eDispOptType, eNxChlist_OptItem_Rename, eNxUtil_OptItem_Rename);
	ONDK_GWM_APP_Destroy(0);

	NX_COMMON_NOT_USED_PARAM(pstContents);
	return ONDK_RET_OK;
}


static ONDK_Result_t	nx_channellistOpt_OptionChangedItemValue(NxUiChannelListOptContents_t *pstContents, HUINT32 ulCurIdx)
{
	eNxUiUtil_OptItem_t		eOptItem = eNxUtil_OptItem_None;

	if (pstContents->stOptList.ulOptItemNum == 0)
	{
		NX_APP_Info("Error!!! Please Check Option item was Zero!!\n");
		return	ONDK_MESSAGE_BREAK;
	}

	eOptItem = pstContents->stOptList.aeOptItemList[ulCurIdx];
	switch (eOptItem)
	{
		case eNxUtil_OptItem_Group:
			nx_channellistOpt_OptionMakeListChangeGroup(pstContents);
			break;
		case eNxUtil_OptItem_Sort:
			nx_channellistOpt_OptionMakeListSort(pstContents);
			break;
		case eNxUtil_OptItem_AddFav:
			nx_channellistOpt_OptionMakeListAddFavourite(pstContents);
			break;
		case eNxUtil_OptItem_Delete:
			nx_channellistOpt_OptionActionDelete(pstContents);
			break;
		case eNxUtil_OptItem_Lock:
			nx_channellistOpt_OptionActionLockUnLock(pstContents, TRUE);
			break;
		case eNxUtil_OptItem_UnLock:
			nx_channellistOpt_OptionActionLockUnLock(pstContents, FALSE);
			break;
		case eNxUtil_OptItem_Rename:
			nx_channellistOpt_OptionActionRename(pstContents);
			break;
		default:
			NX_APP_Info("Error!!! Invalid Option item:[%d]!!\n", eOptItem);
			break;
	}

	return	ONDK_MESSAGE_BREAK;
}


/******************************************************************************/
#define __Channel_OPT_list_Functions______________________________________________
/******************************************************************************/

static inline NxUiChannelListOptContents_t* nx_channellistOpt_GetContents(void)
{
	return	&s_UiChannelListOptContents;
}


static ONDK_Result_t	nx_channellistOpt_ResetOptionExpireTimer(void)
{
	ONDK_Result_t	eResult = ONDK_RET_FAIL;

	// set timer
	eResult = ONDK_GWM_SetTimer(NX_PRISM_CHLIST_OPTION_EXPIRE_TIMER_ID, NX_PRISM_CHLIST_OPTION_EXPIRE_TIME);
	if (eResult == GWM_RET_LINK_CONFLICT)	// 이미 등록되어 종료되지 않은거라면..
	{
		ONDK_GWM_ResetTimer(NX_PRISM_CHLIST_OPTION_EXPIRE_TIMER_ID);
	}

	return eResult;
}


static ONDK_Result_t	nx_channellistOpt_ChangedOptionItemValue(NxUiChannelListOptContents_t *pstContents, HUINT32 ulCurIdx)
{
	ONDK_Result_t			eResult = ONDK_RET_FAIL;


	switch (pstContents->eCurOptItem)
	{
		case eNxChlist_OptItem_All:
			eResult = nx_channellistOpt_OptionChangedItemValue(pstContents, ulCurIdx);
			break;

		case eNxChlist_OptItem_Group:
			eResult = nx_channellistOpt_OptionActionChangeGroup(pstContents, ulCurIdx);
			break;

		case eNxChlist_OptItem_Sort:
			eResult = nx_channellistOpt_OptionActionSort(pstContents, ulCurIdx);
			ONDK_GWM_APP_Destroy(0);
			break;

		case eNxChlist_OptItem_AddFav:
			eResult = nx_channellistOpt_OptionActionAddFavourite(pstContents);
			ONDK_GWM_APP_Destroy(0);
			break;

		default:
			NX_APP_Info("Error!!! Invalid Option item:[%d]!!\n", pstContents->eCurOptItem);
			break;
	}

	return ONDK_RET_OK;
}


static ONDK_Result_t	nx_channellistOpt_AddOptionDataForChanneList(NxUiChannelListOptContents_t *pstContents)
{
	HBOOL			bSvcLock = FALSE;
	DxOperator_e	eOpType = eDxOPERATOR_None;
	DxService_t		*stSvcInfo = NULL;


	stSvcInfo = NX_CHANNEL_FindServiceBySvcUid(pstContents->ulSvcUid);
	if (stSvcInfo == NULL)
	{
		NX_APP_Error("Error!!! Not found DxService info!!\n");
		bSvcLock = FALSE;
	}
	else
	{
		bSvcLock = stSvcInfo->locked;
	}

	// Operator Type
	NX_SYSTEM_GetOperatorType(&eOpType);
	switch (eOpType)
	{
		default:
			#if !defined(CONFIG_OP_MBC)
			NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxUtil_OptItem_Group);
			#endif
			NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxUtil_OptItem_Sort);
			NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxUtil_OptItem_AddFav);
			NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxUtil_OptItem_Delete);
			if (bSvcLock)
			{
				NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxUtil_OptItem_UnLock);
			}
			else
			{
				NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxUtil_OptItem_Lock);
			}

			NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxUtil_OptItem_Rename);
			break;
	}

	// TODO: Add Project config if you want to do

	return ONDK_RET_OK;
}


static ONDK_Result_t	nx_channellistOpt_AddOptionDataForInforHub(NxUiChannelListOptContents_t *pstContents)
{
	HBOOL			bSvcLock = FALSE;
	DxOperator_e	eOpType = eDxOPERATOR_None;
	DxService_t		*stSvcInfo = NULL;

	stSvcInfo = NX_CHANNEL_FindServiceBySvcUid(pstContents->ulSvcUid);
	if (stSvcInfo == NULL)
	{
		NX_APP_Error("Error!!! Not found DxService info!!\n");
		bSvcLock = FALSE;
	}
	else
	{
		bSvcLock = stSvcInfo->locked;
	}

	// Operator Type
	NX_SYSTEM_GetOperatorType(&eOpType);
	switch (eOpType)
	{
		default:
			NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxUtil_OptItem_AddFav);
			NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxUtil_OptItem_Delete);
			if (bSvcLock)
			{
				NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxUtil_OptItem_UnLock);
			}
			else
			{
				NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxUtil_OptItem_Lock);
			}
			NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxUtil_OptItem_Rename);
			break;
	}

	// TODO: Add Project config if you want to do

	return ONDK_RET_OK;
}


static ONDK_Result_t	nx_channellistOpt_InitOptionData(NxUiChannelListOptContents_t *pstContents)
{
	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! pstContents was NULL !! \n");
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

	NX_APP_Info("Init Info - eDispOptType:[%d], eCurOptItem:[%d]\n", pstContents->eDispOptType, pstContents->eCurOptItem);

	return ONDK_RET_OK;
}


static ONDK_Result_t	nx_channellistOpt_ReCreateMenu(NxUiChannelListOptContents_t *pstContents)
{
	HUINT32			i = 0, ulIndex = 0;
	ONDK_Result_t	eResult = ONDK_RET_FAIL;
	eNxChlist_OptItem_t 	ePrevOptItem = eNxChlist_OptItem_All;


	// Init Item Data
	eResult = nx_channellistOpt_InitOptionData(pstContents);
	if (eResult != ONDK_RET_OK)
	{
		ONDK_GWM_APP_Destroy(0);
		return	ONDK_RET_FAIL;
	}

	switch (pstContents->eDispOptType)
	{
		case eNX_UCT_Chlist:
			nx_channellistOpt_AddOptionDataForChanneList(pstContents);
			break;
		case eNX_UCT_Infohub:
			nx_channellistOpt_AddOptionDataForInforHub(pstContents);
			break;
		default:
			ONDK_GWM_APP_Destroy(0);
			break;
	}

	ePrevOptItem = pstContents->eCurOptItem;
	pstContents->eCurOptItem = eNxChlist_OptItem_All;

	for (i = 0; i<pstContents->stOptList.ulOptItemNum; i++)
	{
		if (pstContents->stOptList.aeOptItemList[i] == (HUINT32)ePrevOptItem)
		{
			ulIndex = i;
			break;
		}
	}

	nx_channellistOpt_CreateOptionListPopup(pstContents, ulIndex);
	nx_channellistOpt_ResetOptionExpireTimer();

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_channellistOpt_MsgGwmCreate(NxUiChannelListOptContents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t	eResult = ONDK_RET_FAIL;


	ONDK_GWM_APP_SetAppArea(NX_PRISM_UI_SET_WINDOW_X, NX_PRISM_UI_SET_WINDOW_Y,
								NX_PRISM_UI_SET_WINDOW_W, NX_PRISM_UI_SET_WINDOW_H);

	HxSTD_MemSet(pstContents, 0x00, sizeof(NxUiChannelListOptContents_t));

	// Init Item Data
	eResult = nx_channellistOpt_InitOptionData(pstContents);
	if (eResult != ONDK_RET_OK)
	{
		ONDK_GWM_APP_Destroy(0);
		return	ONDK_RET_FAIL;
	}

	pstContents->eCurOptItem = eNxChlist_OptItem_All;
	pstContents->eDispOptType = (NX_UI_CallerType_t)p1;
	pstContents->ulSvcUid = (HUINT32)p2;
	pstContents->eCurGroup = (NX_ChListGroup_t)p3;

	switch (pstContents->eDispOptType)
	{
		case eNX_UCT_Chlist:
			nx_channellistOpt_AddOptionDataForChanneList(pstContents);
			break;
		case eNX_UCT_Infohub:
			nx_channellistOpt_AddOptionDataForInforHub(pstContents);
			break;
		default:
			ONDK_GWM_APP_Destroy(0);
			break;
	}

	nx_channellistOpt_CreateOptionListPopup(pstContents, 0);
	nx_channellistOpt_ResetOptionExpireTimer();

	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_channellistOpt_MsgGwmDestroy(NxUiChannelListOptContents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	nx_channellistOpt_DestroyListPopup();

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

	ONDK_GWM_KillTimer(NX_PRISM_CHLIST_OPTION_EXPIRE_TIMER_ID);

	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_channellistOpt_MsgGwmHapiMsg(NxUiChannelListOptContents_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	NX_COMMON_NOT_USED_PARAM(pstContents);
	NX_COMMON_NOT_USED_PARAM(handle);
	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_MESSAGE_PASS;
}


static ONDK_Result_t	nx_channellistOpt_MsgGwmKeyDown(NxUiChannelListOptContents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32		lKeyId = p1;


	nx_channellistOpt_ResetOptionExpireTimer();
	switch (lKeyId)
	{
		case KEY_OPT:
			ONDK_GWM_APP_Destroy(0);
			return	ONDK_MESSAGE_BREAK;

		case KEY_STANDBY:
		case KEY_MENU:
		case KEY_GUIDE:
			ONDK_GWM_APP_Destroy(0);
			return ONDK_MESSAGE_PASS;

		case KEY_VFORMAT:
		case KEY_SUBTITLE:
		case KEY_AUDIO:
		case KEY_TEXT:
			NX_PRISM_NotAvailable_Show();
			return	ONDK_MESSAGE_BREAK;

		default:
			return ONDK_MESSAGE_PASS;
	}

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);
}


static ONDK_Result_t	nx_channellistOpt_MsgGwmTimer(NxUiChannelListOptContents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (p1 == NX_PRISM_CHLIST_OPTION_EXPIRE_TIMER_ID)
	{
		ONDK_GWM_APP_Destroy(0);
	}

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	return GWM_RET_OK;
}


/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/

ONDK_Result_t	NX_PRISM_CHANNELLIST_OPT_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t		eResult = ONDK_RET_OK;
	NxUiChannelListOptContents_t	*pstContents = NULL;

	pstContents = nx_channellistOpt_GetContents();

	switch (lMessage)
	{
		case MSG_GWM_CREATE:
			eResult = nx_channellistOpt_MsgGwmCreate(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_KEYUP:
			break;
		case MSG_GWM_KEYDOWN:
			eResult = nx_channellistOpt_MsgGwmKeyDown(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_DESTROY:
			eResult = nx_channellistOpt_MsgGwmDestroy(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_HAPI_MESSAGE:
			eResult = nx_channellistOpt_MsgGwmHapiMsg(pstContents, handle, p1, p2, p3);
			break;

		case MSG_GWM_TIMER:
			NX_APP_Message("MSG_GWM_TIMER\n");
			eResult = nx_channellistOpt_MsgGwmTimer(pstContents, p1, p2, p3);
			break;

		case MSG_APP_POPUP_LIST_DESTROYED:
			if ((p1 == KEY_BACK) && (pstContents->eCurOptItem != eNxChlist_OptItem_All))
			{
				nx_channellistOpt_DestroyListPopup();
				nx_channellistOpt_ReCreateMenu(pstContents);
			}
			else
			{
				ONDK_GWM_APP_Destroy(0);
			}
			break;

		case MSG_APP_POPUP_LIST_INPUTED_KEYDOWN:
			eResult = nx_channellistOpt_MsgGwmKeyDown(pstContents, p1, p2, p3);
			break;

		case MSG_APP_SETTING_VALUE_CHANGED:
			NX_APP_Message("MSG_APP_POPUP_LIST_DESTROYED & MSG_APP_SETTING_VALUE_CHANGED\n");
			if (lMessage == MSG_APP_SETTING_VALUE_CHANGED)
			{
				NX_APP_Message("MSG_APP_SETTING_VALUE_CHANGED\n");
				nx_channellistOpt_DestroyListPopup();
				eResult = nx_channellistOpt_ChangedOptionItemValue(pstContents, p1);
			}
			break;

		case MSG_APP_HOMEMENU_UI_LAUNCH:
			NX_APP_Message("MSG_APP_HOMEMENU_UI_LAUNCH\n");
			if (p1 != eNX_UCT_Live)
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

	return ONDK_GWM_ProcessMessageDefault(lMessage, handle, p1, p2, p3);
}


/******************************************************************************/
#define _________End_Of_File____________________________________________________
/******************************************************************************/

