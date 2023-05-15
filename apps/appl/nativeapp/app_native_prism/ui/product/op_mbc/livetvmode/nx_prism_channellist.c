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
 * @file	  		nx_prism_channel_list.c
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

#include <nx_core_metadata_pf.h>


/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/

#define NX_PRISM_CHLIST_EVENT_DATE_TEXT_BUFFER		16
#define NX_PRISM_CHLIST_ITEM_MAX					10
#define NX_PRISM_CHLIST_ITEM_FOCUS_INDEX			2	// 3th

// channel list
#define	NX_PRISM_CHLIST_POS_X						0
#define	NX_PRISM_CHLIST_POS_Y						(62 - NX_PRISM_FONT_SIZE_26)

#define	NX_PRISM_CHLIST_ITEM_ID						eMenuFrame_Window_CHList

// timer
#define	NX_PRISM_CHLIST_PROGRAMME_TIMER_ID			(NX_PRISM_CHLIST_ITEM_ID + 1)
#define	NX_PRISM_CHLIST_PROGRAMME_EXPIRE_TIME		(1 * NX_COMMON_TIME_SECOND)

#define	NX_PRISM_CHLIST_EXPIRE_TIMER_ID				(NX_PRISM_CHLIST_ITEM_ID + 2)
#define	NX_PRISM_CHLIST_EXPIRE_TIME					(30 * NX_COMMON_TIME_SECOND)

// area
#define	NX_PRISM_CHLIST_FRAME_X						0
#define	NX_PRISM_CHLIST_FRAME_Y						0
#define	NX_PRISM_CHLIST_FRAME_W						1198
#define	NX_PRISM_CHLIST_FRAME_H						NX_PRISM_OSD_VERTICAL

#define	NX_PRISM_CHLIST_ID							(NX_PRISM_CHLIST_ITEM_ID + 3)

#define	NX_PRISM_CHLIST_BG_ID						(NX_PRISM_CHLIST_ITEM_ID + 4)
#define	NX_PRISM_CHLIST_BG_X						NX_PRISM_CHLIST_FRAME_X
#define	NX_PRISM_CHLIST_BG_Y						NX_PRISM_CHLIST_FRAME_Y
#define	NX_PRISM_CHLIST_BG_W						NX_PRISM_CHLIST_FRAME_W
#define	NX_PRISM_CHLIST_BG_H						NX_PRISM_CHLIST_FRAME_H

#define	NX_PRISM_CHLIST_CHBOX_LINE_ID				(NX_PRISM_CHLIST_ITEM_ID + 5)
#define	NX_PRISM_CHLIST_CHBOX_LINE_X				(NX_PRISM_CHLIST_FRAME_X + 157)
#define	NX_PRISM_CHLIST_CHBOX_LINE_Y				(132 + 14)
#define	NX_PRISM_CHLIST_CHBOX_LINE_W				1
#define	NX_PRISM_CHLIST_CHBOX_LINE_H				42

#define	NX_PRISM_CHLIST_LIST_LINE_ID				(NX_PRISM_CHLIST_ITEM_ID + 6)
#define	NX_PRISM_CHLIST_LIST_LINE_X					(NX_PRISM_CHLIST_FRAME_X + 60)
#define	NX_PRISM_CHLIST_LIST_LINE_Y					632
#define	NX_PRISM_CHLIST_LIST_LINE_W					405
#define	NX_PRISM_CHLIST_LIST_LINE_H					1

#define	NX_PRISM_CHLIST_OPTION_TEXT_ID				(NX_PRISM_CHLIST_ITEM_ID + 7)
#define	NX_PRISM_CHLIST_OPTION_TEXT_X				60
#define	NX_PRISM_CHLIST_OPTION_TEXT_Y				653
#define	NX_PRISM_CHLIST_OPTION_TEXT_W				400
#define	NX_PRISM_CHLIST_OPTION_TEXT_H				28
#define	NX_PRISM_CHLIST_OPTION_IMG_MG				10

#define NX_PRISM_CHLIST_FOCUS_ITEM_H				151
#define NX_PRISM_CHLIST_FOCUS_H						66

#define NX_PRISM_CHLIST_ITEM_C_TEXT_OFFSET_Y		16
#define NX_PRISM_CHLIST_ITEM_C_NUMBER_X				100
#define NX_PRISM_CHLIST_ITEM_C_NUMBER_W				86

#define NX_PRISM_CHLIST_ITEM_C_NAME_OFFSET_Y		15
#define NX_PRISM_CHLIST_ITEM_C_NAME_X				185
#define NX_PRISM_CHLIST_ITEM_C_NAME_W				480

#define NX_PRISM_CHLIST_ITEM_C_EVENT_OFFSET_Y		(NX_PRISM_CHLIST_FOCUS_H + 16)
#define NX_PRISM_CHLIST_ITEM_C_EVENT_X				304
#define NX_PRISM_CHLIST_ITEM_C_EVENT_Y				214
#define NX_PRISM_CHLIST_ITEM_C_EVENT_W				430
#define NX_PRISM_CHLIST_ITEM_C_EVENT_H				(ONDK_FONT_SIZE_24 + 8)

#define NX_PRISM_CHLIST_ITEM_C_NEXT_EVENT_X			304
#define NX_PRISM_CHLIST_ITEM_C_NEXT_EVENT_Y			282
#define NX_PRISM_CHLIST_ITEM_C_NEXT_EVENT_W			430

#define NX_PRISM_CHLIST_ITEM_C_THUMBNAIL_X			100
#define NX_PRISM_CHLIST_ITEM_C_THUMBNAIL_Y			194
#define NX_PRISM_CHLIST_ITEM_C_THUMBNAIL_W			199
#define NX_PRISM_CHLIST_ITEM_C_THUMBNAIL_H			112

#define NX_PRISM_CHLIST_ITEM_C_PREMIUM_X			100
#define NX_PRISM_CHLIST_ITEM_C_PREMIUM_Y			194
#define NX_PRISM_CHLIST_ITEM_C_PREMIUM_W			124
#define NX_PRISM_CHLIST_ITEM_C_PREMIUM_H			55

#define NX_PRISM_CHLIST_ITEM_C_ICON_OFFSET_Y		NX_PRISM_CHLIST_ITEM_C_EVENT_OFFSET_Y
#define NX_PRISM_CHLIST_ITEM_C_ICON_X				460
#define NX_PRISM_CHLIST_ITEM_C_ICON_H				28

#define NX_PRISM_CHLIST_ITEM_C_ICON_LOCK_X			( NX_PRISM_CHLIST_ITEM_C_THUMBNAIL_X + 175)
#define NX_PRISM_CHLIST_ITEM_C_ICON_LOCK_Y			276
#define NX_PRISM_CHLIST_ITEM_C_ICON_LOCK_W			22
#define NX_PRISM_CHLIST_ITEM_C_ICON_LOCK_H			28

#define NX_PRISM_CHLIST_ITEM_C_ICON_CAS_X			( NX_PRISM_CHLIST_ITEM_C_THUMBNAIL_X + 139)
#define NX_PRISM_CHLIST_ITEM_C_ICON_CAS_Y			276
#define NX_PRISM_CHLIST_ITEM_C_ICON_CAS_H			28
#define NX_PRISM_CHLIST_ITEM_C_ICON_CAS_W			36


#define NX_PRISM_CHLIST_ITEM_C_EVENT_TIME_OFFSET_Y	(NX_PRISM_CHLIST_FOCUS_H + NX_PRISM_CHLIST_ITEM_C_EVENT_H + 16 + 8)
#define NX_PRISM_CHLIST_ITEM_C_EVENT_TIME_X			60
#define NX_PRISM_CHLIST_ITEM_C_EVENT_TIME_W			430

#define NX_PRISM_CHLIST_ITEM_C_CURSOR_W				124
#define NX_PRISM_CHLIST_ITEM_C_CURSOR_TRANSFER_W	53
#define NX_PRISM_CHLIST_ITEM_C_CURSOR_TRANSFER_X	72
#define NX_PRISM_CHLIST_ITEM_C_CURSOR_H				55

#define NX_PRISM_CHLIST_ITEM_C_CURSOR_IMG01_X		124
#define NX_PRISM_CHLIST_ITEM_C_CURSOR_IMG01_W		68
#define NX_PRISM_CHLIST_ITEM_C_CURSOR_IMG01_H		55

#define NX_PRISM_CHLIST_ITEM_C_CURSOR_IMG02_X		667
#define NX_PRISM_CHLIST_ITEM_C_CURSOR_IMG02_W		68
#define NX_PRISM_CHLIST_ITEM_C_CURSOR_IMG02_H		55

#define NX_PRISM_CHLIST_ITEM_C_NOW_X				304
#define NX_PRISM_CHLIST_ITEM_C_NOW_Y				194
#define NX_PRISM_CHLIST_ITEM_C_NOW_W				52
#define NX_PRISM_CHLIST_ITEM_C_NOW_H				17

#define NX_PRISM_CHLIST_ITEM_C_NEXT_X				304
#define NX_PRISM_CHLIST_ITEM_C_NEXT_Y				265
#define NX_PRISM_CHLIST_ITEM_C_NEXT_W				45
#define NX_PRISM_CHLIST_ITEM_C_NEXT_H				14


#define NX_PRISM_CHLIST_ITEM_NUMBER_X				100
#define NX_PRISM_CHLIST_ITEM_NUMBER_W				50

#define NX_PRISM_CHLIST_ITEM_NAME_X					155
#define NX_PRISM_CHLIST_ITEM_NAME_W					400


#define NX_PRISM_CHLIST_ITEM_TEXT_BUFFER			256

#define NX_PRISM_CHLIST_FOCUS_ITEM_NO				3

#define NX_PRISM_CHLIST_ITEM_W						490

#define NX_PRISM_CHLIST_TRANSFER_X					229

#define NX_PRISM_CHLIST_CHANNELGROUP_ID				(NX_PRISM_CHLIST_ITEM_ID + 8)
#define NX_PRISM_CHLIST_CHANNELGROUP_X				0
#define NX_PRISM_CHLIST_CHANNELGROUP_Y				0
#define NX_PRISM_CHLIST_CHANNELGROUP_W				300
#define NX_PRISM_CHLIST_CHANNELGROUP_H				NX_PRISM_OSD_VERTICAL
#define NX_PRISM_CHLIST_CHANNELGROUP_ITEM_H			64
#define NX_PRISM_CHLIST_CHANNELGROUP_BG_TOP_H		62

#define NX_PRISM_CHLIST_CHANNELGROUP_ITEM_TEXT_X	60
#define NX_PRISM_CHLIST_CHANNELGROUP_ITEM_TEXT_Y	25
#define	NX_PRISM_CHLIST_CHANNELGROUP_ITEM_TEXT_W	180

#define NX_PRISM_CHLIST_CHANNELGROUP_COUNT			9

#define NX_PRISM_CHLIST_DIM_ID						(NX_PRISM_CHLIST_ITEM_ID + 9)
#define NX_PRISM_CHLIST_DIM_X						301
#define NX_PRISM_CHLIST_DIM_Y						0
#define NX_PRISM_CHLIST_DIM_W						222
#define NX_PRISM_CHLIST_DIM_H						NX_PRISM_OSD_VERTICAL

#define	NX_PRISM_CHLIST_OPTION_SELECTLIST_MAX		5

#define	NX_PRISM_CHLIST_SRCHSAT_FIXED_MAX_ANT_NUM_IN_UI					CONFIG_APUI_SAT_ANTENNA_MAX_SLOT

#define NX_PRISM_CHLIST_GROUP_TEXT_X				77
#define NX_PRISM_CHLIST_GROUP_TEXT_Y				321
#define NX_PRISM_CHLIST_GROUP_TEXT_H				200

#define NX_PRISM_CHLIST_GROUP_ARROW_X				57
#define NX_PRISM_CHLIST_GROUP_ARROW_Y				286
#define NX_PRISM_CHLIST_GROUP_ARROW_W				20
#define NX_PRISM_CHLIST_GROUP_ARROW_H				26

/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/

typedef struct
{
	HINT32		lcn;
	HCHAR		*pszChName;
	HCHAR		pszEventName[EPG_HUMAX_NAME_LEN];
	HCHAR		pszNextEventName[EPG_HUMAX_NAME_LEN];
	HCHAR		pszEventDate[NX_TEXT_SIZE_64];
	DxSvcType_e svcType;
} NxChListData_t;



typedef struct tagLinkObject
{
	struct tagLinkObject	*nextLink;
} *LINK_OBJECT;


typedef struct
{
	LINK_OBJECT			link;
	GWM_ObjectType_t	type;
	HINT32				ID;
	ONDK_Rect_t			rect;
	HINT32				z_order;
	HINT32				attr;
	HINT32				focus;
	GWM_Proc_t			Proc;
	GWM_ObjDraw_t		objDrawMethod;
	HBOOL				isHide;

	void	*userdata;
	HINT32	lUserDataCount;

	HCHAR	*blankImg;
	HCHAR	*fillImg;
	HCHAR	*backgroundImg;

	ONDKImageHandle	hBlankImg;
	ONDKImageHandle	hFillImg;
	ONDKImageHandle	hBackgroundImg;

	// my kim
	HINT32	item_col_h;
	HUINT32	startTime, duration;

	HCHAR	*casImgPath;
	ONDKImageHandle	hCasImg;

	HCHAR	*szLockImgPath;
	ONDKImageHandle hLockImg;

}NxChListObject_t;

typedef enum {
	eCHLIST_CHANNELLIST,
	eCHLIST_CHANNELGROUP
} Nx_ChList_FocusType_e;

typedef struct
{
	NX_ChListGroup_t eChannelGroupIndex;
	HCHAR			 szChannelGroupName[DxNAME_LEN];

} NxChList_ChGroupItem_t;

typedef struct
{
	HINT32				nChannelNum;
	HINT32				nCurItemIndex;					// channel list 에서 리스트의 channel 목록 유지 및 처리를 위해.
	HCHAR				szOptionText[NX_TEXT_SIZE_64];
	HCHAR				szRenameText[DxNAME_LEN];

	NX_Channel_t 		*pstChannels;					// live proc 에서 얻어옴..
	NxChListData_t		*pstChListData;

	HBOOL				bChangedGroup;					// User flag Set Change Group
	NX_ChListGroup_t	eChangedGroup;					// Group Item

	eNxUiUtil_OptItem_t	eChangedSort;					// Sorting Item
	eNxChlist_OptItem_t	ePinCodeActiontItem;			// Pin-Code Ation Item

	//Channel Group
	HxVector_t	*pstChannelGroupList;///List of Channel Group

	Nx_ChList_FocusType_e eFocusType;

	//ALphabet Opt
	NX_Util_OPTIONList_t	stAlphabetOptList;
	HUINT8		**szItemTextList;

	// Antena Informations
	HUINT32				ulNumAnt;
	HINT32				alAntIdxArray[NX_PRISM_CHLIST_SRCHSAT_FIXED_MAX_ANT_NUM_IN_UI];
	DxAntInfo_t			astAntInfo[NX_PRISM_CHLIST_SRCHSAT_FIXED_MAX_ANT_NUM_IN_UI];

	//ALphabet Opt
	NX_Util_OPTIONList_t	stSatOptList;
	HUINT8		**szSatItemTextList;

	//used for Satellite and Alphabet
	NX_ChListGroup_t	eOptGroup;

	//handle for TV and Radio
	ONDKImageHandle hTvImg;
	ONDKImageHandle hRadioImg;

} NxUiChannelListContents_t;


/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/

static NxUiChannelListContents_t		s_stUiChListContents;
static NX_PinDialogInstance_t			s_stUiChList_PinCodeDialog;
// option
static Nx_PrismListDialog_Content_t		s_stUiChList_OptContents;
static NX_ChListGroup_t					s_eLastChangedGroup;


/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/

static ONDK_Result_t	nx_chlist_DisplayPinCodeDialog(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_chlist_DestroyPinCodeDialog(void);
static void				nx_chlist_CreatePinCodeDialog(void);
static ONDK_Result_t	nx_chlist_ResponseDialogProc(HINT32 lMessage, Handle_t hHandle, HINT32 lParam1, HINT32 lParam2, HINT32 lParam3);
static void				nx_chlist_CreateResponseDialouge(HUINT8* pStrinId);

/* Channel List Option */
static ONDK_Result_t	nx_chlist_OptionActionChangeGroup(NxUiChannelListContents_t *pstContents, HINT32 p3);
static ONDK_Result_t	nx_chlist_OptionActionAddFavourite(NxUiChannelListContents_t *pstContents, HINT32 p3);
static ONDK_Result_t	nx_chlist_OptionActionSort(NxUiChannelListContents_t *pstContents, HINT32 p3);
static ONDK_Result_t	nx_chlist_OptionActionDelete(NxUiChannelListContents_t *pstContents, HINT32 p3);
static ONDK_Result_t	nx_chlist_OptionActionLockUnLock(NxUiChannelListContents_t *pstContents, HINT32 p3);
static ONDK_Result_t	nx_chlist_OptionActionLock(NxUiChannelListContents_t *pstContents);
static ONDK_Result_t	nx_chlist_OptionActionUnLock(NxUiChannelListContents_t *pstContents);
static ONDK_Result_t	nx_chlist_OptionActionRename(NxUiChannelListContents_t *pstContents, HINT32 p3);
static ONDK_Result_t	nx_chlist_OptionActionSaveRename(NxUiChannelListContents_t *pstContents);
static ONDK_Result_t	nx_chlist_MakeNxChannelListGroupBySatellite(NX_Channel_t **ddstChannelList, HINT32 *nListNum, NX_ChListGroup_t eChGroupType);

/* Channel List */
static NxUiChannelListContents_t* 	nx_chlist_GetContents(void);

static ONDK_Result_t	nx_chlist_ResetProgrammeTimer(NxUiChannelListContents_t *pstContents);
static ONDK_Result_t	nx_chlist_ResetExitChListTimer(NxUiChannelListContents_t *pstContents);
static HCHAR 			*nx_chlist_GetCasTypeImagPath(NX_Channel_t *channel);
static ONDK_Result_t	nx_chlist_ReloadChannelList(NxUiChannelListContents_t *pstContents);
static ONDK_Result_t 	nx_chlist_ActionChannelList(NxUiChannelListContents_t *pstContents, HINT32 keyId);
static ONDK_Result_t	nx_chlist_SetPfSearchCurChannel(NxUiChannelListContents_t *pstContents);
static ONDK_Result_t	nx_chlist_SearchStart(NxUiChannelListContents_t *pstContents);
static HERROR			nx_chlist_SetChannelListData (NxUiChannelListContents_t *pstContents, HBOOL bProgramme);
static NX_Programme_t*	nx_chlist_GetPfProgramme(NX_Channel_t *pstCurChannel, HCHAR* szNextEventName);
static void				nx_chlist_UpdateUiFocusItem(NxUiChannelListContents_t *pstContents);
static void				nx_chlist_UpdateUiOptionMenu(NxUiChannelListContents_t *pstContents);
static void				nx_chlist_SetUiCreateOptionMenu(NxUiChannelListContents_t *pstContents);
static HERROR 			nx_chlist_SetUiObject(NxUiChannelListContents_t *pstContents, Handle_t hAct, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_chlist_DrawFrameBG_SS(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop);
static ONDK_Result_t	nx_chlist_DrawChannelListMethod(ONDKWindowHandle *screen, ONDK_Region_t *area, ONDK_Rect_t *rect,
																	ONDK_GWM_GuiObject_t object, void *arg, int param1);
static ONDK_Result_t	nx_chlist_AddDimImage(NxUiChannelListContents_t *pstContents);
static ONDK_Result_t	nx_chlist_DeleteDimImage(NxUiChannelListContents_t *pstContents);
static	ONDK_Result_t	nx_chlist_DrawDimImage(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop);
static HUINT32 			nx_chlist_GetFocusIndexOfChannelGroup(NxUiChannelListContents_t *pstContent);
static ONDK_Result_t	nx_chlist_InitChannelGroupList(NxUiChannelListContents_t *pstContents);
static ONDK_Result_t    nx_chlist_DrawCursor(ONDKSurfaceHandle *screen, NxChList_ChGroupItem_t *pstData, ONDK_Rect_t *stListRegion,
								HINT32 nOffsetY, ONDKFontHandle *hFontHandle, ONDK_Color_t textFontColor, ONDK_Color_t bgColor, HUINT32 ulCellHeight, HBOOL bIsFocus, int bUiTop);
static ONDK_Result_t 	nx_chlist_CreateChannelGroupUI(NxUiChannelListContents_t *pstContents);
static void				nx_chlist_OptionLoadAlphabetGroupInitDataDefault(NxUiChannelListContents_t *pstContents);
static void				nx_chlist_CreateOptionChangeAlphabetGroupListPopup(NxUiChannelListContents_t *pstContents);
static ONDK_Result_t	nx_chlist_DisplayListPopup(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_chlist_LoadAntennaInfos(NxUiChannelListContents_t *pstContents);
static void				nx_chlist_CreateOptionSatelliteGroupListPopup(NxUiChannelListContents_t *pstContents);
HUINT8**				nx_chlist_OptionMakeSatelliteGroupItemTextList(NxUiChannelListContents_t *pstContents, NX_Util_OPTIONList_t *pstOptItemList);
static ONDK_Result_t 	nx_chlist_MsgGwmCreate(NxUiChannelListContents_t *pstContents, Handle_t hAct, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_chlist_MsgGwmDestroy(NxUiChannelListContents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	nx_chlist_MsgGwmKeyDown(NxUiChannelListContents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	nx_chlist_MsgGwmTimer(NxUiChannelListContents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_chlist_MsgPopUpOptValueChange(NxUiChannelListContents_t *pstContents, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_chlist_AlphabetOptionChange(NxUiChannelListContents_t *pstContents, HUINT32 ulCurIdx);
static ONDK_Result_t	nx_chlist_SatelliteOptionChange(NxUiChannelListContents_t *pstContents, HUINT32 ulCurIdx);


/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/

/******************************************************************************/
#define __Pin_Code_Dialog_Functions___________________________________________
/******************************************************************************/

static ONDK_Result_t	nx_chlist_DisplayPinCodeDialog(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return	NX_PRISM_COMP_PinDlalog_ProtoProc(&s_stUiChList_PinCodeDialog, message, hAction, p1, p2, p3);
}


static ONDK_Result_t	nx_chlist_DestroyPinCodeDialog(void)
{
	return ONDK_GWM_APP_Destroy(nx_chlist_DisplayPinCodeDialog);
}


static void		nx_chlist_CreatePinCodeDialog(void)
{
	ONDK_Rect_t		stRect;
	HUINT32			ulAttribute = 0;

	stRect = ONDK_Rect(NX_PRISM_UI_PIN_DIALOG_X, NX_PRISM_UI_PIN_DIALOG_Y, NX_PRISM_UI_PIN_DIALOG_WIDTH, NX_PRISM_UI_PIN_DIALOG_HEIGHT);
	ulAttribute = (NX_DLG_TYPE_OSD_FULL|NX_DLG_TYPE_BG_DIM|NX_DLG_TYPE_MESSAGE|NX_DLG_TIMEOUT_NOTIMEOUT);
	NX_PRISM_COMP_PinDlalog_SetDialog(&s_stUiChList_PinCodeDialog, ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis()),
								&stRect, ulAttribute, COL(C_T_Whistle_100), ONDK_GetString(RES_MESG_3684_ID),
								eNxPinCodeDataType_ParentalControl);
	/* Create Pop Up */
	ONDK_GWM_APP_Create (ONDK_UPPER_WINDOW, "nx_menuGeneral_DisplayPinCodeDialog", APP_DEFAULT_PRIORITY, nx_chlist_DisplayPinCodeDialog, 0, 0, 0, 0);
}


/******************************************************************************/
#define __Response_Dialog_Functions___________________________________________
/******************************************************************************/

static ONDK_Result_t	nx_chlist_ResponseDialogProc(HINT32 lMessage, Handle_t hHandle, HINT32 lParam1, HINT32 lParam2, HINT32 lParam3)
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


static void		nx_chlist_CreateResponseDialouge(HUINT8* pszString)
{
	if (pszString == NULL)
	{
		NX_APP_Error("Error!!! Pls check pszString was NULL!! \n");
		return;
	}

	ONDK_GWM_APP_Create (ONDK_UPPER_WINDOW, "nx_chlist_ResponseDialogProc", APP_DEFAULT_PRIORITY,
						nx_chlist_ResponseDialogProc, 0, (HINT32)pszString, 0, 0);
}


/******************************************************************************/
#define __Option_Functions______________________________________________
/******************************************************************************/

static ONDK_Result_t	nx_chlist_OptionActionChangeGroup(NxUiChannelListContents_t *pstContents, HINT32 p3)
{
	HBOOL		bFindEqualChannel = FALSE;
	HINT32		lGroupNum = 0, i = 0;
	HUINT32		ulExtraFilter = 0;
	NX_ChListGroup_t eChGroupType = eNxChListGroup_TV;
	NX_Channel_t	*pstGroupList = NULL, *pstCurChannel = NULL, *pstFindChannel = NULL;
	NX_Channel_t	stLastChannel;

	eChGroupType = (NX_ChListGroup_t)p3;

	if (pstContents->eChangedGroup == eChGroupType)
	{
		HxLOG_Debug("\n\neChGroupType is Equal eCurChGroupType\n\n");
		return ONDK_RET_OK;
	}

	if ((pstContents->bChangedGroup == FALSE) && (pstContents->pstChannels == NULL))
	{
		NX_APP_Error("Error!!! pstContents->pstChannels is NULL !!\n");
		return ONDK_RET_FAIL;
	}

	if(eChGroupType == eNxChListGroup_ALL)
	{
		NX_CHANNEL_GROUP_MakeNxChannelListAll(&pstGroupList, &lGroupNum);
	}
	else
	{
		if(eChGroupType >= eNxChListGroup_Satellite)
		{
			nx_chlist_MakeNxChannelListGroupBySatellite(&pstGroupList,&lGroupNum, eChGroupType);
		}
		else
		{
			ulExtraFilter = NX_CHANNEL_GROUP_GetExtraFilterByGroupType(eChGroupType);

			// make group list
			if (ERR_OK != NX_CHANNEL_GROUP_MakeNxChannelListGroup(&pstGroupList, &lGroupNum, eChGroupType, ulExtraFilter	/* DxAlpabetGruopType_b .. etc*/))
			{
				HxLOG_Debug("\nFailed Make Group list! (eChGroupType : %d)\n", eChGroupType);
				return ONDK_RET_FAIL;
			}
		}
	}

	if (lGroupNum <= 0)
	{
		NX_APP_Error("Error!!! eChGroupType : (0x%x)(lGroupNum <= 0) !!\n", eChGroupType);
		if(pstGroupList)
		{
			NX_APP_Free(pstGroupList);
			pstGroupList = NULL;
		}
		return ONDK_RET_FAIL;
	}

	HxSTD_MemSet(&stLastChannel, 0, sizeof(NX_Channel_t));
	HxSTD_MemCopy(&stLastChannel, &pstContents->pstChannels[pstContents->nCurItemIndex], sizeof(NX_Channel_t));

	// bChangedGroup 가 true 이면, channel list mem free
	if (pstContents->bChangedGroup && pstContents->pstChannels != NULL)
	{
		// bChangedGroup 가 true 이면, pstContents->pstChannels 는 재할당 받은것이다..
		NX_APP_Free(pstContents->pstChannels);
		pstContents->pstChannels = NULL;
	}

	// change channel list in prism channel list context
	pstContents->pstChannels = pstGroupList;
	pstContents->nChannelNum = lGroupNum;

	HxLOG_Debug("\nNew group list Num : %d \n\n", lGroupNum);
	// find last service in new group channel list.
	bFindEqualChannel = FALSE;
	for (i = 0; i < lGroupNum; i++)
	{
		pstFindChannel = &pstContents->pstChannels[i];

		if (pstFindChannel	&&
			(pstFindChannel->svcUid == stLastChannel.svcUid) &&
			(pstFindChannel->lcn == stLastChannel.lcn))
		{
			bFindEqualChannel = TRUE;
			pstContents->nCurItemIndex = i;
			break;
		}
	}

	if (bFindEqualChannel == FALSE)
	{
		pstContents->nCurItemIndex = 0;
	}

	pstContents->bChangedGroup = TRUE;
	pstContents->eChangedGroup = eChGroupType;

	// update ui
	if (pstContents->pstChannels == NULL)
	{
		NX_APP_Error("Error!!! pstContents->pstChannels is NULL !!\n");
		return ONDK_RET_FAIL;
	}

	pstCurChannel = &pstContents->pstChannels[pstContents->nCurItemIndex];
	if (pstCurChannel != NULL)
	{
		nx_chlist_SetChannelListData(pstContents, FALSE);
		nx_chlist_SetPfSearchCurChannel(pstContents);
		nx_chlist_ResetProgrammeTimer(pstContents);
	}
	else
	{
		NX_APP_Error("Error!!! pstCurChannel was NULL!!\n");
		return ONDK_RET_FAIL;
	}

	NX_COMMON_NOT_USED_PARAM(pstContents);

	return ONDK_RET_OK;
}


static ONDK_Result_t	nx_chlist_OptionActionAddFavourite(NxUiChannelListContents_t *pstContents, HINT32 p3)
{
	HBOOL	bIsFavGroupList = FALSE;

	// Alreay Saved Action, so Just update list if in favourite list
	bIsFavGroupList = NX_CHANNEL_GROUP_IsFavouriteGroup(pstContents->eChangedGroup);
	if (bIsFavGroupList == TRUE)
	{
		nx_chlist_ReloadChannelList(pstContents);
	}
	else
	{
		// Not to do
	}

	//Reload ChannelGroup
	HLIB_VECTOR_RemoveAll(pstContents->pstChannelGroupList);
	ONDK_GWM_List_ChangeItems((LIST_OBJECT)ONDK_GWM_Obj_GetObjectByID(NX_PRISM_CHLIST_CHANNELGROUP_ID), HLIB_VECTOR_Length(pstContents->pstChannelGroupList), (GWM_LIST_ITEMS)pstContents->pstChannelGroupList);

	nx_chlist_InitChannelGroupList(pstContents);
	ONDK_GWM_List_ChangeItems((LIST_OBJECT)ONDK_GWM_Obj_GetObjectByID(NX_PRISM_CHLIST_CHANNELGROUP_ID), HLIB_VECTOR_Length(pstContents->pstChannelGroupList), (GWM_LIST_ITEMS)pstContents->pstChannelGroupList);
	NX_COMMON_NOT_USED_PARAM(pstContents);
	NX_COMMON_NOT_USED_PARAM(p3);

	return ONDK_RET_OK;
}


static ONDK_Result_t	nx_chlist_OptionActionSort(NxUiChannelListContents_t *pstContents, HINT32 p3)
{
	HINT32			i = 0, lSortChNum = 0;
	HBOOL			bFindEqualChannel = FALSE;
	HxList_t		*pSortList = NULL;
	NX_Channel_t	*pstCurChannel = NULL, *pstFindChannel = NULL,  *pstSortChLists = NULL;
	NX_Channel_t	stLastChannel;
	eNxUiUtil_OptItem_t		eOptItem = eNxUtil_OptItem_None;


	eOptItem = (eNxUiUtil_OptItem_t)p3;

	switch (eOptItem)
	{
		case eNxUtil_OptItem_SortNumber:
			{
				if (pstContents->pstChannels == NULL)
				{
					NX_APP_Error("Error!!! pstContents->pstChannels is NULL !!\n");
					return ONDK_RET_FAIL;
				}

				pstContents->eChangedSort = eNxUtil_OptItem_SortNumber;
				NX_CHANNEL_GROUP_SortNxChannelsByLcn(pstContents->pstChannels, pstContents->nChannelNum);

				HxSTD_MemSet(&stLastChannel, 0, sizeof(NX_Channel_t));
				HxSTD_MemCopy(&stLastChannel, &pstContents->pstChannels[pstContents->nCurItemIndex], sizeof(NX_Channel_t));

				bFindEqualChannel = FALSE;
				for (i = 0; i < pstContents->nChannelNum; i++)
				{
					pstFindChannel = &pstContents->pstChannels[i];

					if (pstFindChannel	&&
						(pstFindChannel->svcUid == stLastChannel.svcUid )&&
						(pstFindChannel->lcn	== stLastChannel.lcn))
					{
						bFindEqualChannel = TRUE;
						pstContents->nCurItemIndex = i;
						break;
					}
				}

				if (bFindEqualChannel == FALSE)
				{
					pstContents->nCurItemIndex = 0;
				}

				// update ui
				pstCurChannel = &pstContents->pstChannels[pstContents->nCurItemIndex];
				if (pstCurChannel != NULL)
				{
					nx_chlist_SetChannelListData(pstContents, FALSE);
					nx_chlist_SetPfSearchCurChannel(pstContents);
					nx_chlist_ResetProgrammeTimer(pstContents);
				}
				else
				{
					return ONDK_RET_FAIL;
				}
			}
			break;

		case eNxUtil_OptItem_SortName:
			{
				if (pstContents->pstChannels == NULL)
				{
					NX_APP_Error("Error!!! pstContents->pstChannels is NULL !!\n");
					return ONDK_RET_FAIL;
				}

				pstContents->eChangedSort = eNxUtil_OptItem_SortName;
				pSortList = NX_CHANNEL_GROUP_SortNxChannelsByName(pstContents->pstChannels, pstContents->nChannelNum);
				if (pSortList == NULL)
				{
					break;
				}

				NX_CHANNEL_GROUP_MakeNxChannelListByList(pSortList,  &pstSortChLists, &lSortChNum);

				// sort list 제거
				HLIB_LIST_RemoveAll(pSortList);

				HxSTD_MemSet(&stLastChannel, 0, sizeof(NX_Channel_t));
				HxSTD_MemCopy(&stLastChannel, &pstContents->pstChannels[pstContents->nCurItemIndex], sizeof(NX_Channel_t));

				// bChangedGroup 가 true 이면, channel list mem free
				if (pstContents->bChangedGroup && pstContents->pstChannels != NULL)
				{
					// bChangedGroup 가 true 이면, pstContents->pstChannels 는 재할당 받은것이다..
					NX_APP_Free(pstContents->pstChannels);
					pstContents->pstChannels = NULL;
				}

				// change channel list in prism channel list context
				pstContents->pstChannels = pstSortChLists;
				pstContents->nChannelNum = lSortChNum;

				bFindEqualChannel = FALSE;
				for (i = 0; i < lSortChNum; i++)
				{
					pstFindChannel = &pstContents->pstChannels[i];

					if (pstFindChannel	&&
						(pstFindChannel->svcUid == stLastChannel.svcUid )&&
						(pstFindChannel->lcn	== stLastChannel.lcn))
					{
						bFindEqualChannel = TRUE;
						pstContents->nCurItemIndex = i;
						break;
					}
				}

				if (bFindEqualChannel == FALSE)
				{
					pstContents->nCurItemIndex = 0;
				}
				// update ui
				pstCurChannel = &pstContents->pstChannels[pstContents->nCurItemIndex];
				if (pstCurChannel != NULL)
				{
					nx_chlist_SetChannelListData(pstContents, FALSE);
					nx_chlist_SetPfSearchCurChannel(pstContents);
					nx_chlist_ResetProgrammeTimer(pstContents);
				}
			}
			break;
		default:
			NX_APP_Info("Error!!! Not Support Sort Item [%d] !!\n", eOptItem);
			break;
	}

	NX_COMMON_NOT_USED_PARAM(pstContents);
	return ONDK_RET_OK;
}


static ONDK_Result_t	nx_chlist_OptionActionDelete(NxUiChannelListContents_t *pstContents, HINT32 p3)
{
	HERROR				hError = ERR_FAIL;
	NX_Channel_t		*stSelectChannel = NULL, *stCurPlayChannel = NULL;
	NX_ChListGroup_t	eCurPlayChListGroup = eNxChListGroup_TV;
	DxService_t			*stSvcInfo = NULL, *stFindSvcInfo = NULL;
	NX_FavGroupID_e 	eFavGroupID = eNxFavGroupID_None;


	if (pstContents->pstChannels == NULL)
	{
		NX_APP_Error("Error!!! pstContents->pstChannels was NULL!!\n");
		return ONDK_RET_FAIL;
	}

	if (pstContents->nCurItemIndex >= pstContents->nChannelNum)
	{
		NX_APP_Error("Error!!! Invalid Channel Index:(%u), ChNum(%u) !!\n", pstContents->nCurItemIndex, pstContents->nChannelNum);
		return ONDK_RET_FAIL;
	}

	if (pstContents->pstChannels == NULL)
	{
		NX_APP_Error("Error!!! pstContents->pstChannels is NULL !!\n");
		return ONDK_RET_FAIL;
	}

	stSelectChannel = &pstContents->pstChannels[pstContents->nCurItemIndex];
	if (stSelectChannel == NULL)
	{
		NX_APP_Error("Error!!! stSelectChannel was NULL!!\n");
		return ONDK_RET_FAIL;
	}

	stSvcInfo = NX_CHANNEL_FindServiceBySvcUid(stSelectChannel->svcUid);
	if (stSvcInfo == NULL)
	{
		NX_APP_Error("Error!!! stSvcInfo(0x%x) was NULL!!\n", stSelectChannel->svcUid);
		return ONDK_RET_FAIL;
	}

	// 1. Check it is delete Live Service
	stCurPlayChannel = NX_PRISM_LIVE_GetMasterChannel();
	if ((stCurPlayChannel != NULL) && (stCurPlayChannel->svcUid == stSelectChannel->svcUid))
	{
		eCurPlayChListGroup = NX_PRISM_LIVE_GetCurrentChGroupType();
		// 1.1 Find Next Service
		stFindSvcInfo = NX_CHANNEL_GROUP_FindServiceInGroup(eCurPlayChListGroup, eNxFindSvcDir_Next, stCurPlayChannel);
		if (stFindSvcInfo)
		{
			NX_APP_Info("Find Next Service eCurPlayChListGroup:(0x%x), SvcUID:(0x%x), Name:(%s)!!\n", eCurPlayChListGroup, stFindSvcInfo->uid, stFindSvcInfo->name);
		}

		// 1.2 Find Prev Service
		if (stFindSvcInfo == NULL)
		{
			NX_APP_Error("Error!!! Do not find Next Service eCurPlayChListGroup:(0x%x), Svc UID:(0x%x)!!\n", eCurPlayChListGroup, stCurPlayChannel->svcUid);
			stFindSvcInfo = NX_CHANNEL_GROUP_FindServiceInGroup(eCurPlayChListGroup, eNxFindSvcDir_Prev, stCurPlayChannel);
			if (stFindSvcInfo)
			{
				NX_APP_Info("Find Prev Service eCurPlayChListGroup:(0x%x), SvcUID:(0x%x), Name:(%s)!!\n", eCurPlayChListGroup, stFindSvcInfo->uid, stFindSvcInfo->name);
			}
		}

		// 1.3 Find First Service
		if (stFindSvcInfo == NULL)
		{
			NX_APP_Error("Error!!! Do not find Prev Service eCurPlayChListGroup:(0x%x), Svc UID:(0x%x)!!\n", eCurPlayChListGroup, stCurPlayChannel->svcUid);
			stFindSvcInfo = NX_CHANNEL_GROUP_FindServiceInGroup(eCurPlayChListGroup, eNxFindSvcDir_First, stCurPlayChannel);
			if (stFindSvcInfo)
			{
				NX_APP_Info("Find First Service eCurPlayChListGroup:(0x%x), SvcUID:(0x%x), Name:(%s)!!\n", eCurPlayChListGroup, stFindSvcInfo->uid, stFindSvcInfo->name);
			}
		}

		if (stFindSvcInfo != NULL)
		{
			NX_PRISM_LIVE_ServiceStartMainView(TRUE, stFindSvcInfo->uid, eNxChListGroup_CURRENT, 0);
		}
		else
		{
			NX_APP_Error("Error!!! Do not find Next tunning Service eCurPlayChListGroup:(0x%x)!!\n", eCurPlayChListGroup);
		}
	}

	/* Update Service Info */
	if ((eCurPlayChListGroup >= eNxChListGroup_Favorite1) && (eCurPlayChListGroup <= eNxChListGroup_Favorite5))
	{
		eFavGroupID = NX_CHANNEL_FAV_ConvertChannelFavGroupToFavGroup(eCurPlayChListGroup);
		if (eFavGroupID != eNxFavGroupID_None)
		{
			hError = NX_CHANNEL_FAV_RemoveFavouriteItem(eFavGroupID, stSelectChannel);
			if (hError != ERR_OK)
			{
				NX_APP_Error("Error!!! NX_CHANNEL_FAV_RemoveFavouriteItem() - eFavGroupID :(%d)!!\n", eFavGroupID);
			}
		}
	}
	else
	{
		hError = NX_CHANNEL_RemoveService(stSvcInfo);
		if (hError != ERR_OK)
		{
			NX_APP_Error("Error!!! NX_CHANNEL_RemoveService() - Svc Uid :(x0%x)!!\n", stSvcInfo->uid);
		}
	}

	/* Noti - System Service Update */
	ONDK_GWM_SendMessage(NULL, MSG_APP_SYSTEM_UPDATE_SERVICE, 0, 0, 0, 0);

	/* Show update Info Message */
	nx_chlist_CreateResponseDialouge((HUINT8*)ONDK_GetString(RES_MESG_5653_ID));

	NX_COMMON_NOT_USED_PARAM(p3);

	return ONDK_RET_OK;
}


static ONDK_Result_t	nx_chlist_OptionActionLockUnLock(NxUiChannelListContents_t *pstContents, HINT32 p3)
{
	eNxUiUtil_OptItem_t 	eUtilOptItem = (eNxUiUtil_OptItem_t)p3;

	if (eUtilOptItem == eNxUtil_OptItem_Lock)
	{
		nx_chlist_OptionActionLock(pstContents);
	}
	else
	{
		pstContents->ePinCodeActiontItem = eNxChlist_OptItem_Lock;
		nx_chlist_CreatePinCodeDialog();
	}

	return ONDK_RET_OK;
}


static ONDK_Result_t	nx_chlist_OptionActionLock(NxUiChannelListContents_t *pstContents)
{
	NX_Channel_t	*stSelectChannel = NULL;
	DxService_t		*stSvcInfo = NULL;


	NX_APP_Info("Action - eNxUtil_OptItem_Lock !!\n");

	if (pstContents->pstChannels == NULL)
	{
		NX_APP_Error("Error!!! pstContents->pstChannels was NULL!!\n");
		return ONDK_RET_FAIL;
	}

	if (pstContents->nCurItemIndex >= pstContents->nChannelNum)
	{
		NX_APP_Error("Error!!! Invalid Channel Index:(%u), ChNum(%u) !!\n", pstContents->nCurItemIndex, pstContents->nChannelNum);
		return ONDK_RET_FAIL;
	}

	if (pstContents->pstChannels == NULL)
	{
		NX_APP_Error("Error!!! pstContents->pstChannels is NULL !!\n");
		return ONDK_RET_FAIL;
	}

	stSelectChannel = &pstContents->pstChannels[pstContents->nCurItemIndex];
	if (stSelectChannel == NULL)
	{
		NX_APP_Error("Error!!! stSelectChannel was NULL!!\n");
		return ONDK_RET_FAIL;
	}

	stSvcInfo = NX_CHANNEL_FindServiceBySvcUid(stSelectChannel->svcUid);
	if (stSvcInfo == NULL)
	{
		NX_APP_Error("Error!!! stSvcInfo(0x%x) was NULL!!\n", stSelectChannel->svcUid);
		return ONDK_RET_FAIL;
	}

	if (stSvcInfo->locked == TRUE)
	{
		NX_APP_Info("Alreay Locked Service:(%s) !!\n", stSvcInfo->name);
		return ONDK_RET_OK;
	}

	stSvcInfo->locked = TRUE;

	/* Update Service Info */
	NX_CHANNEL_UpdateService(stSvcInfo);

	/* Update Focus Item lock Info */
	nx_chlist_UpdateUiFocusItem(pstContents);

	/* Show update Info Message */
	nx_chlist_CreateResponseDialouge((HUINT8*)ONDK_GetString(RES_MESG_5654_ID));

	return ONDK_RET_OK;
}


static ONDK_Result_t	nx_chlist_OptionActionUnLock(NxUiChannelListContents_t *pstContents)
{
	NX_Channel_t	*stSelectChannel = NULL;
	DxService_t		*stSvcInfo = NULL;


	NX_APP_Info("Action - eNxUtil_OptItem_UnLock !!\n");

	if (pstContents->pstChannels == NULL)
	{
		NX_APP_Error("Error!!! pstContents->pstChannels was NULL!!\n");
		return ONDK_RET_FAIL;
	}

	if (pstContents->nCurItemIndex >= pstContents->nChannelNum)
	{
		NX_APP_Error("Error!!! Invalid Channel Index:(%u), ChNum(%u) !!\n", pstContents->nCurItemIndex, pstContents->nChannelNum);
		return ONDK_RET_FAIL;
	}

	if (pstContents->pstChannels == NULL)
	{
		NX_APP_Error("Error!!! pstContents->pstChannels is NULL !!\n");
		return ONDK_RET_FAIL;
	}

	stSelectChannel = &pstContents->pstChannels[pstContents->nCurItemIndex];
	if (stSelectChannel == NULL)
	{
		NX_APP_Error("Error!!! stSelectChannel was NULL!!\n");
		return ONDK_RET_FAIL;
	}

	stSvcInfo = NX_CHANNEL_FindServiceBySvcUid(stSelectChannel->svcUid);
	if (stSvcInfo == NULL)
	{
		NX_APP_Error("Error!!! stSvcInfo(0x%x) was NULL!!\n", stSelectChannel->svcUid);
		return ONDK_RET_FAIL;
	}

	if (stSvcInfo->locked == FALSE)
	{
		NX_APP_Info("Alreay UnLocked Service:(%s) !!\n", stSvcInfo->name);
		return ONDK_RET_OK;
	}

	stSvcInfo->locked = FALSE;

	/* Update Service Info */
	NX_CHANNEL_UpdateService(stSvcInfo);

	/* Update Focus Item lock Info */
	nx_chlist_UpdateUiFocusItem(pstContents);

	/* Show update Info Message */
	nx_chlist_CreateResponseDialouge((HUINT8*)ONDK_GetString(RES_MESG_5655_ID));

	return ONDK_RET_OK;
}


static ONDK_Result_t	nx_chlist_OptionActionRename(NxUiChannelListContents_t *pstContents, HINT32 p3)
{
	NX_Channel_t	*stSelectChannel = NULL;
	DxService_t		*stSvcInfo = NULL;


	NX_APP_Info("Action - eNxChlist_OptItem_Rename !!\n");

	if (pstContents->pstChannels == NULL)
	{
		NX_APP_Error("Error!!! pstContents->pstChannels was NULL!!\n");
		return ONDK_RET_FAIL;
	}

	if (pstContents->nCurItemIndex >= pstContents->nChannelNum)
	{
		NX_APP_Error("Error!!! Invalid Channel Index:(%u), ChNum(%u) !!\n", pstContents->nCurItemIndex, pstContents->nChannelNum);
		return ONDK_RET_FAIL;
	}

	if (pstContents->pstChannels == NULL)
	{
		NX_APP_Error("Error!!! pstContents->pstChannels is NULL !!\n");
		return ONDK_RET_FAIL;
	}

	stSelectChannel = &pstContents->pstChannels[pstContents->nCurItemIndex];
	if (stSelectChannel == NULL)
	{
		NX_APP_Error("Error!!! stSelectChannel was NULL!!\n");
		return ONDK_RET_FAIL;
	}

	stSvcInfo = NX_CHANNEL_FindServiceBySvcUid(stSelectChannel->svcUid);
	if (stSvcInfo == NULL)
	{
		NX_APP_Error("Error!!! stSvcInfo(0x%x) was NULL!!\n", stSelectChannel->svcUid);
		return ONDK_RET_FAIL;
	}

	ONDK_GWM_KillTimer(NX_PRISM_CHLIST_EXPIRE_TIMER_ID);
	ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, "NX_PRISM_KeyBoard_Proc", APP_DEFAULT_POPUP_PRIORITY,
				NX_PRISM_KeyBoard_Proc, HANDLE_NULL, 0, NX_PRISM_KEYBOARD_INPUT_NORMAL, (HINT32)stSvcInfo->name);

	NX_COMMON_NOT_USED_PARAM(p3);
	NX_COMMON_NOT_USED_PARAM(pstContents);

	return ONDK_RET_OK;
}


static ONDK_Result_t	nx_chlist_OptionActionSaveRename(NxUiChannelListContents_t *pstContents)
{
	NX_Channel_t	*stSelectChannel = NULL;
	DxService_t 	*stSvcInfo = NULL;
	HCHAR			*pszStr = NULL;
	HUINT8			*pszStringBuf = NULL;

	HxSTD_MemSet(pstContents->szRenameText, 0, sizeof(HUINT8) * DxNAME_LEN);
	pstContents->szRenameText[0] = '\0';
	pszStr = NX_PRISM_KeyBoard_GetResultString();
	HxSTD_StrNCpy(pstContents->szRenameText, pszStr, DxNAME_LEN);
	if(pszStr)
	{
		NX_APP_Free(pszStr);
		pszStr = NULL;
	}
	NX_PRISM_KeyBoard_Destroy();

	NX_APP_Info("Action - eNxChlist_OptItem_Rename - Save !!\n");

	if (pstContents->pstChannels == NULL)
	{
		NX_APP_Error("Error!!! pstContents->pstChannels was NULL!!\n");
		return ONDK_RET_FAIL;
	}

	if (pstContents->nCurItemIndex >= pstContents->nChannelNum)
	{
		NX_APP_Error("Error!!! Invalid Channel Index:(%u), ChNum(%u) !!\n", pstContents->nCurItemIndex, pstContents->nChannelNum);
		return ONDK_RET_FAIL;
	}

	if (pstContents->pstChannels == NULL)
	{
		NX_APP_Error("Error!!! pstContents->pstChannels is NULL !!\n");
		return ONDK_RET_FAIL;
	}

	stSelectChannel = &pstContents->pstChannels[pstContents->nCurItemIndex];
	if (stSelectChannel == NULL)
	{
		NX_APP_Error("Error!!! stSelectChannel was NULL!!\n");
		return ONDK_RET_FAIL;
	}

	stSvcInfo = NX_CHANNEL_FindServiceBySvcUid(stSelectChannel->svcUid);
	if (stSvcInfo == NULL)
	{
		NX_APP_Error("Error!!! stSvcInfo(0x%x) was NULL!!\n", stSelectChannel->svcUid);
		return ONDK_RET_FAIL;
	}

	if (HxSTD_StrCmp((char*)stSvcInfo->name, (char*)pstContents->szRenameText) == 0)
	{
		NX_APP_Info("Not Change Service Name stSvcInfo->name:(%s), pstContents->szRenameText(%s) !!\n", stSvcInfo->name, pstContents->szRenameText);
	}
	else
	{
		HxSTD_StrNCpy(stSvcInfo->name, pstContents->szRenameText, DxNAME_LEN);

		/* Update Service Info */
		NX_CHANNEL_UpdateService(stSvcInfo);

		/* Update Focus Item lock Info */
		nx_chlist_UpdateUiFocusItem(pstContents);
	}

	/* String Buffer 초기화. */
	pszStringBuf = NX_PRISM_GetTextBuffer(NX_TEXT_SIZE_512);
	if (pszStringBuf)
	{
		HxSTD_MemSet(pszStringBuf, 0x00, NX_TEXT_SIZE_512);
		HxSTD_sprintf((char*)pszStringBuf, (char*)ONDK_GetString(RES_MESG_6113_ID), pstContents->szRenameText);

		/* Show update Info Message */
		nx_chlist_CreateResponseDialouge(pszStringBuf);
	}
	else
	{
		/* Show update Info Message */
		nx_chlist_CreateResponseDialouge((HUINT8*)ONDK_GetString(RES_MESG_6113_ID));
	}

	return ONDK_RET_OK;
}

static ONDK_Result_t	nx_chlist_MakeNxChannelListGroupBySatellite(NX_Channel_t **ddstChannelList, HINT32 *nListNum, NX_ChListGroup_t eChGroupType)
{
	HINT32			i = 0, svcNum = 0, ulGroupLen = 0, lGroupNum = 0;
	HxVector_t		*pstResGroupList = NULL;
	NX_Channel_t	*pstRetChannelList = NULL;
	NX_Channel_t	*pstGroupList = NULL, *tmpChannel = NULL;
	DxService_t		*tempSvc = NULL;
	HUINT32			ulSatIndex = 0;
	NxUiChannelListContents_t *pstContents = NULL;
	DxAntInfo_t 	stSatInfo;

	pstContents = nx_chlist_GetContents();
	ulSatIndex = eChGroupType ^ eNxChListGroup_Satellite;

	if(ulSatIndex >= pstContents->ulNumAnt)
	{
		HxLOG_Error("out of index\n\n");
		goto END_FUNC;
	}

	stSatInfo = pstContents->astAntInfo[ulSatIndex];

	pstResGroupList = HLIB_VECTOR_NewEx(NULL, 0, NULL, (void *(*)(void *))DLIB_Free_CB, NULL);

	NX_CHANNEL_GROUP_MakeNxChannelListAll(&pstGroupList, &lGroupNum);

	for(i = 0; i < lGroupNum; i++)
	{
		tmpChannel  = &pstGroupList[i];
		tempSvc = NX_CHANNEL_FindServiceBySvcUid(tmpChannel->svcUid);
		HxLOG_Print("equal (%s) (%s) ?\n\n",stSatInfo.satType,tempSvc->satType);
		if(HxSTD_StrCmp(stSatInfo.satType,tempSvc->satType) == 0)
		{
			HLIB_VECTOR_Add(pstResGroupList, tmpChannel);
		}
	}

	ulGroupLen = HLIB_VECTOR_Length(pstResGroupList);

	pstRetChannelList = NX_APP_Calloc(sizeof(NX_Channel_t) * ulGroupLen);

	for (i = 0; i < ulGroupLen; i++)
	{
		CListItem_t *data = (CListItem_t*)HLIB_VECTOR_ItemAt(pstResGroupList, i);
		if (data)
		{
			pstRetChannelList[i].svcUid = (HUINT32)data->uid;
			pstRetChannelList[i].lcn = (HUINT32)data->nLogicalNo;
			svcNum++;
		}
	}

END_FUNC:
	if (pstResGroupList)
	{
		pstResGroupList = NULL;
	}

	*nListNum = svcNum;
	*ddstChannelList = pstRetChannelList;

	return ONDK_RET_OK;
}


/******************************************************************************/
#define __Channel_list_Functions______________________________________________
/******************************************************************************/

static NxUiChannelListContents_t*	nx_chlist_GetContents(void)
{
	return &s_stUiChListContents;
}


static ONDK_Result_t	nx_chlist_ResetProgrammeTimer(NxUiChannelListContents_t *pstContents)
{
	ONDK_Result_t	retTimer = ONDK_RET_FAIL;
	// set timer
	retTimer = ONDK_GWM_SetTimer(NX_PRISM_CHLIST_PROGRAMME_TIMER_ID, NX_PRISM_CHLIST_PROGRAMME_EXPIRE_TIME);

	if (retTimer == GWM_RET_LINK_CONFLICT)	// 이미 등록되어 종료되지 않은거라면..
	{
		ONDK_GWM_ResetTimer(NX_PRISM_CHLIST_PROGRAMME_TIMER_ID);
	}

	NX_COMMON_NOT_USED_PARAM(pstContents);

	return retTimer;
}


static ONDK_Result_t	nx_chlist_ResetExitChListTimer(NxUiChannelListContents_t *pstContents)
{
	ONDK_Result_t	retTimer = ONDK_RET_FAIL;

	// set timer
	retTimer = ONDK_GWM_SetTimer(NX_PRISM_CHLIST_EXPIRE_TIMER_ID, NX_PRISM_CHLIST_EXPIRE_TIME);
	if (retTimer == GWM_RET_LINK_CONFLICT)	// 이미 등록되어 종료되지 않은거라면..
	{
		ONDK_GWM_ResetTimer(NX_PRISM_CHLIST_EXPIRE_TIMER_ID);
	}

	NX_COMMON_NOT_USED_PARAM(pstContents);

	return retTimer;
}

static HCHAR 	*nx_chlist_GetCasTypeImagPath(NX_Channel_t *channel)
{
	DxService_t	*findService 	= NULL;

	if (channel == NULL)
	{
		HxLOG_Debug("channel is NULL!\n");
		return NULL;
	}

	findService = NX_CHANNEL_FindServiceBySvcUid(channel->svcUid);
	if (findService == NULL)
	{
		HxLOG_Debug("Not found DxService info!\n");
		return NULL;
	}

	return	NX_PRISM_UTIL_GetCasTypeResource(findService->casType);
}

static ONDK_Result_t	nx_chlist_SearchStart(NxUiChannelListContents_t *pstContents)
{
	NX_MetadataSearch_t *pstMetaSearch = NULL;
	NX_QUERY_t	endQuery, startQuery, andQuery;
	HUINT32 	unixtime = 0, queryTime = 0;
	HCHAR 		value[64] = {'\0',};

	NX_Channel_t	*pstCurChannel = NULL;

	if (NX_SYSTEM_GetTime(&unixtime) != ERR_OK)
	{
		HxLOG_Error("time is Invalid!!!\n");
		return ONDK_RET_FAIL;
	}

	queryTime	= unixtime - (SECONDS_PER_HOUR * 1);

	HxSTD_sprintf(value, "%u", queryTime);
	endQuery = NX_METADATA_SEARCH_CreateQuery("endTime", ">", value);

	queryTime 	= unixtime + SECONDS_PER_HOUR * 6; // plus 12 hour
	HxSTD_sprintf(value, "%u", queryTime);
	startQuery = NX_METADATA_SEARCH_CreateQuery("startTime", "<", value);

	andQuery = NX_PORT_META_QUERY_AND(endQuery, startQuery);

	NX_PORT_META_QUERY_DeleteQuery(endQuery);
	NX_PORT_META_QUERY_DeleteQuery(startQuery);

	pstMetaSearch = NX_METADATA_SEARCH_New(eNX_META_SEARCH_TARGET_SCHEDULED);
	if (pstMetaSearch == NULL)
	{
		return ERR_FAIL;
	}

	NX_PORT_META_QUERY_SetQuery(pstMetaSearch->context, andQuery);
	NX_PORT_META_QUERY_DeleteQuery(andQuery);

	pstCurChannel = &pstContents->pstChannels[pstContents->nCurItemIndex];

	NX_METADATA_SEARCH_AddChannelConstraint(pstMetaSearch, pstCurChannel);

	NX_PORT_META_QUERY_SetFilter(pstMetaSearch->context, eNxQUERY_FILTER_ALL);

	NX_METADATA_EPG_SearchEPG(pstMetaSearch);

	return GWM_RET_OK;
}

static ONDK_Result_t	nx_chlist_SetPfSearchCurChannel(NxUiChannelListContents_t *pstContents)
{
	NX_Channel_t	*pstCurChannel = NULL;

	if (pstContents->pstChannels == NULL)
	{
		NX_APP_Error("Error!!! pstContents->pstChannels is NULL !!\n");
		return ONDK_RET_FAIL;
	}

	pstCurChannel = &pstContents->pstChannels[pstContents->nCurItemIndex];
	if (pstCurChannel != NULL)
	{
		nx_chlist_SearchStart(pstContents);
	}

	return ONDK_RET_OK;
}

static ONDK_Result_t	nx_chlist_ReloadChannelList(NxUiChannelListContents_t *pstContents)
{
	HERROR			hError = ERR_FAIL;
	HBOOL			bFindEqualChannel = FALSE;
	HINT32			lGroupNum = 0, i = 0;
	HUINT32 		ulExtraFilter = 0;
	NX_Channel_t	*pstGroupList = NULL, *pstCurChannel = NULL, *pstFindChannel = NULL;
	NX_Channel_t	stLastChannel;
	NX_ChListGroup_t eChGroupType = eNxChListGroup_TV;


	eChGroupType = pstContents->eChangedGroup;

	if ((pstContents->bChangedGroup == FALSE) && (pstContents->pstChannels == NULL))
	{
		NX_APP_Error("Error!!! pstContents->pstChannels is NULL !!\n");
		return ONDK_RET_FAIL;
	}

	if(eChGroupType == eNxChListGroup_ALL)
	{
		NX_CHANNEL_GROUP_MakeNxChannelListAll(&pstGroupList, &lGroupNum);
	}
	else
	{
		ulExtraFilter = NX_CHANNEL_GROUP_GetExtraFilterByGroupType(eChGroupType);

		// make group list
		hError = NX_CHANNEL_GROUP_MakeNxChannelListGroup(&pstGroupList, &lGroupNum, eChGroupType, ulExtraFilter	/* DxAlpabetGruopType_b .. etc*/);
		if (ERR_OK != hError)
		{
			NX_APP_Error("\nFailed Make Group list! (eChGroupType : 0x%x)\n", eChGroupType);
			return	ONDK_RET_FAIL;
		}
	}

	if (lGroupNum <= 0)
	{
		NX_APP_Info(" Make Group was Zero list! (eChGroupType : 0x%x)\n", eChGroupType);
		if(pstGroupList)
		{
			NX_APP_Free(pstGroupList);
			pstGroupList = NULL;
		}
		return ONDK_RET_OK;
	}

	HxSTD_MemSet(&stLastChannel, 0, sizeof(NX_Channel_t));
	HxSTD_MemCopy(&stLastChannel, &pstContents->pstChannels[pstContents->nCurItemIndex], sizeof(NX_Channel_t));

	// bChangedGroup 가 true 이면, channel list mem free
	if (pstContents->bChangedGroup && pstContents->pstChannels != NULL)
	{
		// bChangedGroup 가 true 이면, pstContents->pstChannels 는 재할당 받은것이다..
		NX_APP_Free(pstContents->pstChannels);
		pstContents->pstChannels = NULL;
	}

	// change channel list in prism channel list context
	pstContents->pstChannels = pstGroupList;
	pstContents->nChannelNum = lGroupNum;

	HxLOG_Debug("\nNew group list Num : %d \n\n", lGroupNum);
	// find last service in new group channel list.
	bFindEqualChannel = FALSE;
	for (i = 0; i < lGroupNum; i++)
	{
		pstFindChannel = &pstContents->pstChannels[i];

		if (pstFindChannel	&&
			(pstFindChannel->svcUid == stLastChannel.svcUid) &&
			(pstFindChannel->lcn == stLastChannel.lcn))
		{
			bFindEqualChannel = TRUE;
			pstContents->nCurItemIndex = i;
			break;
		}
	}

	if (bFindEqualChannel == FALSE)
	{
		if (pstContents->nCurItemIndex >= pstContents->nChannelNum)
		{
			pstContents->nCurItemIndex = 0;
		}
	}

	// update ui
	if (pstContents->pstChannels == NULL)
	{
		NX_APP_Error("Error!!! pstContents->pstChannels is NULL !!\n");
		return ONDK_RET_FAIL;
	}

	pstCurChannel = &pstContents->pstChannels[pstContents->nCurItemIndex];
	if (pstCurChannel != NULL)
	{
		nx_chlist_SetPfSearchCurChannel(pstContents);
		nx_chlist_SetChannelListData(pstContents, FALSE);
		nx_chlist_ResetProgrammeTimer(pstContents);
	}
	else
	{
		NX_APP_Error("Error!!! pstCurChannel was NULL!!\n");
		return ONDK_RET_FAIL;
	}

	NX_COMMON_NOT_USED_PARAM(pstContents);

	return ONDK_RET_OK;
}


static ONDK_Result_t	nx_chlist_ActionChannelList(NxUiChannelListContents_t *pstContents, HINT32 keyId)
{
	ONDK_Result_t		ret = ONDK_RET_OK;
	NX_Channel_t		*stCurPlayChannel = NULL, *stSelectChannel = NULL;
	NX_ChListGroup_t	eChListGroupType = eNxChListGroup_CURRENT, eChListPrevGroupType = eNxChListGroup_CURRENT;

	switch(keyId)
	{
		case KEY_EXIT:
		case KEY_BACK:
			// 지금은 사라지는 것만..
			{
				ONDK_GWM_APP_Destroy(0);
				return	ONDK_MESSAGE_BREAK;
			}
			break;

		case KEY_OK:
			{
				// 그룹 체인지가 되고, channel 변경이 이루어 진다면,
				// main service list 를 현재 그룹으로 셋팅하고, (nx_core_channel)
				// 관련 모듈들에서 service list 를 reload 하도록 메세지를 날린다.
				if (pstContents->pstChannels == NULL)
				{
					NX_APP_Error("Error!!! pstContents->pstChannels is NULL !!\n");
					return ONDK_RET_FAIL;
				}

				if (pstContents->bChangedGroup)
				{
					eChListGroupType = NX_PRISM_LIVE_GetCurrentChGroupType();
					eChListPrevGroupType = eChListGroupType;
					if (eChListGroupType != pstContents->eChangedGroup)
					{
						eChListGroupType = pstContents->eChangedGroup;
					}
					else
					{
						eChListGroupType = eNxChListGroup_CURRENT;
					}
				}

				stCurPlayChannel = NX_PRISM_LIVE_GetMasterChannel();
				stSelectChannel = &pstContents->pstChannels[pstContents->nCurItemIndex];
				if ((stCurPlayChannel != NULL) && (stSelectChannel != NULL) && (stSelectChannel->svcUid != stCurPlayChannel->svcUid))
				{
					NX_PRISM_LIVE_ServiceStartMainView(TRUE, pstContents->pstChannels[pstContents->nCurItemIndex].svcUid, eChListGroupType, 0);
				}
				else if ((stCurPlayChannel != NULL) && (stSelectChannel != NULL) && ((stSelectChannel->svcUid == stCurPlayChannel->svcUid) && (pstContents->bChangedGroup)))
				{
					NX_PRISM_LIVE_ServiceStartMainView(TRUE, pstContents->pstChannels[pstContents->nCurItemIndex].svcUid, eChListGroupType, 0);
				}

				ONDK_GWM_APP_Destroy(0);
			}
			return	ONDK_MESSAGE_BREAK;

		case KEY_ARROWUP:
			{
				pstContents->nCurItemIndex--;
				if (pstContents->nCurItemIndex < 0)
				{
					pstContents->nCurItemIndex = pstContents->nChannelNum - 1;
				}

				if (pstContents->nChannelNum > 1)
				{
					nx_chlist_SetPfSearchCurChannel(pstContents);
					nx_chlist_SetChannelListData(pstContents, FALSE);
				}
				ret = ONDK_MESSAGE_BREAK;
			}
			break;

		case KEY_ARROWDOWN:
			{
				pstContents->nCurItemIndex++;
				if (pstContents->nCurItemIndex >= pstContents->nChannelNum)
				{
					pstContents->nCurItemIndex = 0;
				}

				if (pstContents->nChannelNum > 1)
				{
					nx_chlist_SetPfSearchCurChannel(pstContents);
					nx_chlist_SetChannelListData(pstContents, FALSE);
				}
				ret = ONDK_MESSAGE_BREAK;
			}
			break;

		case KEY_CH_PLUS:
			{
				if (pstContents->nChannelNum < NX_PRISM_CHLIST_ITEM_MAX)
				{
					ret = ONDK_MESSAGE_BREAK;
					break;
				}

				pstContents->nCurItemIndex -= NX_PRISM_CHLIST_ITEM_MAX;
				if (pstContents->nCurItemIndex < 0)
				{
					pstContents->nCurItemIndex = pstContents->nChannelNum - NX_PRISM_CHLIST_ITEM_MAX;
				}
				nx_chlist_SetPfSearchCurChannel(pstContents);
				nx_chlist_SetChannelListData(pstContents, FALSE);
				ret = ONDK_MESSAGE_BREAK;
			}
			break;

		case KEY_CH_MINUS:
			{
				if (pstContents->nChannelNum < NX_PRISM_CHLIST_ITEM_MAX)
				{
					ret = ONDK_MESSAGE_BREAK;
					break;
				}

				pstContents->nCurItemIndex += NX_PRISM_CHLIST_ITEM_MAX;
				if (pstContents->nCurItemIndex >= pstContents->nChannelNum)
				{
					pstContents->nCurItemIndex = (pstContents->nCurItemIndex - pstContents->nChannelNum);
				}
				nx_chlist_SetPfSearchCurChannel(pstContents);
				nx_chlist_SetChannelListData(pstContents, FALSE);
				ret = ONDK_MESSAGE_BREAK;
			}
			break;

		default:
			break;
	}

	nx_chlist_ResetProgrammeTimer(pstContents);
	nx_chlist_ResetExitChListTimer(pstContents);

	return ret;
}


static HERROR	nx_chlist_SetChannelListData (NxUiChannelListContents_t *pstContents, HBOOL bProgramme)
{
	HINT32			i = 0, getIndex = 0, lFocusIdx = 0, lFocusItemCnt = 0;
	HUINT32			ulStartTime = 0, ulDuration = 0, ulWeekIndex = 0;
	HCHAR 			*pszCasImgPath = NULL, szEventName[EPG_HUMAX_NAME_LEN], szNextEventName[EPG_HUMAX_NAME_LEN];
	HBOOL 			isFreedMem = FALSE;
	HxDATETIME_t	stStartTime;
	NX_Channel_t 	*channel = NULL;
	DxService_t 	*stSvcInfo = NULL;
	NX_Programme_t	*programme = NULL;
#if defined(CONFIG_PROD_LANG_ARABIC)
	HINT32	currentLanguage = eLangID_NONE;
#endif


	if (pstContents->pstChListData == NULL)	// Make Alloc Data if it's first
	{
		pstContents->pstChListData = NX_APP_Calloc(sizeof(NxChListData_t) * NX_PRISM_CHLIST_ITEM_MAX);
	}

	lFocusItemCnt = (NX_PRISM_CHLIST_ITEM_FOCUS_INDEX + 1);

	// Service List less than 3 channel item
	if (pstContents->nChannelNum < lFocusItemCnt)
	{
		if (pstContents->nChannelNum < (lFocusItemCnt - 1))	// 1 channel Item
		{
			getIndex = 0;
		}
		else
		{
			getIndex = (pstContents->nCurItemIndex == 0) ? (pstContents->nCurItemIndex + 1) : (pstContents->nCurItemIndex - 1) ;
		}
	}
	else
	{
		// current channel 이 2번 이므로, 처음 표시될 item 부터 표시하도록 설정.
		getIndex = pstContents->nCurItemIndex - 2;
		if (getIndex < 0)
		{
			getIndex = pstContents->nChannelNum + getIndex;
		}
	}

	// clear cas img
	ONDK_GWM_COM_ChannelList_SetCasImg(NX_PRISM_CHLIST_ID, NULL);
	ONDK_GWM_COM_ChannelList_SetLockImg(NX_PRISM_CHLIST_ID, NULL);

	// clear time
	ONDK_GWM_COM_ChannelList_SetProgrammeTime(NX_PRISM_CHLIST_ID, 0, 0);

	if (pstContents->nChannelNum < lFocusItemCnt)
	{
		lFocusIdx = (pstContents->nChannelNum - 1);
		lFocusIdx = (lFocusIdx < 0) ? 0 : lFocusIdx;
	}
	else
	{
		lFocusIdx = NX_PRISM_CHLIST_ITEM_FOCUS_INDEX;
	}

	for (i = 0; i < NX_PRISM_CHLIST_ITEM_MAX; i++)
	{
		// 채널리스트가 리스트 아이템보다 작으면 처리안함.
		if (i >= pstContents->nChannelNum)
		{
			pstContents->pstChListData[i].lcn = 0;
			pstContents->pstChListData[i].pszChName = NULL;
			HxSTD_sprintf(pstContents->pstChListData[i].pszEventDate, "  ");
			HxSTD_sprintf(pstContents->pstChListData[i].pszEventName, "  ");
			break;
		}

		channel = &pstContents->pstChannels[getIndex];
		stSvcInfo = NX_CHANNEL_FindServiceBySvcUid(channel->svcUid);
		if (stSvcInfo == NULL)
		{
			NX_APP_Error("Error!!! NX_CHANNEL_FindServiceBySvcUid() - svcUid:(0x%x) Fail !! \n", channel->svcUid);
			continue;
		}

		HxSTD_MemSet(&pstContents->pstChListData[i], 0, sizeof(NxChListData_t));
		pstContents->pstChListData[i].lcn = channel->lcn;
		pstContents->pstChListData[i].pszChName = stSvcInfo->name;
		//set service type
		pstContents->pstChListData[i].svcType = stSvcInfo->svcType;

		if ((i == lFocusIdx) && (bProgramme == TRUE))
		{
			isFreedMem = FALSE;
			HxSTD_MemSet(szNextEventName, 0, EPG_HUMAX_NAME_LEN);
			programme = nx_chlist_GetPfProgramme(channel, szNextEventName);

			if(programme == NULL)
			{
				isFreedMem = TRUE;
				programme = NX_METADATA_PF_GetPF(channel);
			}

			if (programme)
			{
				NX_PF_PROGRAMMES_GetNameByProgramme(programme, szEventName);
				HxSTD_MemSet(pstContents->pstChListData[i].pszEventName, 0, EPG_HUMAX_NAME_LEN);
				HLIB_STD_StrUtf8NCpy(pstContents->pstChListData[i].pszEventName, szEventName, EPG_HUMAX_NAME_LEN);

				//set next program
				HxSTD_MemSet(pstContents->pstChListData[i].pszNextEventName, 0, EPG_HUMAX_NAME_LEN);
				HLIB_STD_StrUtf8NCpy(pstContents->pstChListData[i].pszNextEventName, szNextEventName, EPG_HUMAX_NAME_LEN);

				ulStartTime = NX_PF_PROGRAMMES_GetStartTimeByProgramme(programme);
				ulDuration = NX_PF_PROGRAMMES_GetDurationByProgramme(programme);

				HLIB_DATETIME_ConvertUnixTimeToDateTime(ulStartTime, &stStartTime);
				ulWeekIndex = HLIB_DATETIME_GetWeekDayIndex(stStartTime.stDate);
				HxSTD_sprintf(pstContents->pstChListData[i].pszEventDate, "%s %02d %s",
							ONDK_GetString(NX_PRISM_UTIL_GetResStringIdByWeekIndex(ulWeekIndex)),
							stStartTime.stDate.ucDay, ONDK_GetString(NX_PRISM_UTIL_GetResStringIdByMonth(stStartTime.stDate.ucMonth)));

				// set programme time
				ONDK_GWM_COM_ChannelList_SetProgrammeTime(NX_PRISM_CHLIST_ID, ulStartTime, ulDuration);

				//Free programme memory
				if(isFreedMem)
				{
					NX_APP_Free(programme);
				}
			}
			// set cas img
			pszCasImgPath = nx_chlist_GetCasTypeImagPath(channel);
			ONDK_GWM_COM_ChannelList_SetCasImg(NX_PRISM_CHLIST_ID, pszCasImgPath);

			// Set Lock Image
			if (stSvcInfo && (stSvcInfo->locked == TRUE))
			{
				ONDK_GWM_COM_ChannelList_SetLockImg(NX_PRISM_CHLIST_ID, RES_211_00_LOCK_PNG);
			}
		}

		getIndex++;
		if (getIndex >= pstContents->nChannelNum)
			getIndex = 0;
	}

#if defined(CONFIG_PROD_LANG_ARABIC)
	NX_SYSTEM_GetMenuLanuage(&currentLanguage);
	ONDK_GWM_COM_ChannelList_SetUserdata(NX_PRISM_CHLIST_ID, pstContents->pstChListData, pstContents->nChannelNum, currentLanguage);
#else
	ONDK_GWM_COM_ChannelList_SetUserdata(NX_PRISM_CHLIST_ID, pstContents->pstChListData, pstContents->nChannelNum);
#endif
	ONDK_GWM_APP_InvalidateObject(NX_PRISM_CHLIST_ID);

	return ERR_OK;
}

static NX_Programme_t*	nx_chlist_GetPfProgramme(NX_Channel_t *pstCurChannel, HCHAR* szNextEventName)
{
	HINT32			num 		= 0;
	HUINT32			unixtime	= 0;
	NX_Programme_t	*programme 	= NULL, *nextProgramme 	= NULL;
	HxVector_t		*result 	= NULL;

	result = HLIB_VECTOR_New(NULL,NULL);

	if (NX_METADATA_EPG_GetResultProgrammeBySvcUid(pstCurChannel->svcUid, result, &num) == ERR_FAIL)
	{
		HxLOG_Debug("No There Programme Data!\n");
		goto END_FUNC;
	}

	if (NX_SYSTEM_GetTime(&unixtime) != ERR_OK)
	{
		HxLOG_Error("time is Invalid!!!\n");
		goto END_FUNC;
	}

	if (num > 0 && (result != NULL))
	{
		HINT32	j = 0;
		HUINT32 startTime = 0, duration = 0;
		for (j = 0; j < num; j++)
		{
			programme = HLIB_VECTOR_ItemAt(result, j);

			startTime	= NX_METADATA_PROGRAMMES_GetStartTime(programme);
			duration	= NX_METADATA_PROGRAMMES_GetDuration(programme);

			if ((unixtime >= startTime) &&
				(unixtime <= (startTime + duration)))
			{
				HxLOG_Debug("\nFind Programme ! (svcuid (%d))\n\n", pstCurChannel->svcUid);
				if( (j + 1) < num)
				{
					nextProgramme = HLIB_VECTOR_ItemAt(result, j + 1);
					if(nextProgramme != NULL)
						NX_PF_PROGRAMMES_GetNameByProgramme(nextProgramme, szNextEventName);
				}
				goto END_FUNC;
			}
		}
	}

END_FUNC:
	HLIB_VECTOR_Delete(result);		// new list 만 날리고, core epg 의 data 는 날리면 안됨.

	return programme;
}

static void		nx_chlist_UpdateUiFocusItem(NxUiChannelListContents_t *pstContents)
{
	NX_Channel_t	*stSelectChannel = NULL;
	DxService_t		*stSvcInfo = NULL;

	if (pstContents->pstChannels == NULL)
	{
		NX_APP_Error("Error!!! pstContents->pstChannels was NULL!!\n");
		return;
	}

	if (pstContents->nCurItemIndex >= pstContents->nChannelNum)
	{
		NX_APP_Error("Error!!! Invalid Channel Index:(%u), ChNum(%u) !!\n", pstContents->nCurItemIndex, pstContents->nChannelNum);
		return;
	}

	if (pstContents->pstChannels == NULL)
	{
		NX_APP_Error("Error!!! pstContents->pstChannels is NULL !!\n");
		return;
	}

	stSelectChannel = &pstContents->pstChannels[pstContents->nCurItemIndex];
	if (stSelectChannel == NULL)
	{
		NX_APP_Error("Error!!! stSelectChannel was NULL!!\n");
		return;
	}

	stSvcInfo = NX_CHANNEL_FindServiceBySvcUid(stSelectChannel->svcUid);
	if (stSvcInfo == NULL)
	{
		NX_APP_Error("Error!!! stSvcInfo(0x%x) was NULL!!\n", stSelectChannel->svcUid);
		return;
	}

	if (stSvcInfo->locked == TRUE)
	{
		ONDK_GWM_COM_ChannelList_SetLockImg(NX_PRISM_CHLIST_ID, RES_211_00_LOCK_PNG);
	}
	else
	{
		ONDK_GWM_COM_ChannelList_SetLockImg(NX_PRISM_CHLIST_ID, NULL);
	}
}

static void		nx_chlist_SetUiCreateOptionMenu(NxUiChannelListContents_t *pstContents)
{
	ONDK_Rect_t			stRect;

	stRect = ONDK_Rect(NX_PRISM_CHLIST_OPTION_TEXT_X, NX_PRISM_CHLIST_OPTION_TEXT_Y,
						NX_PRISM_CHLIST_OPTION_TEXT_W, NX_PRISM_CHLIST_OPTION_TEXT_H);

	HxSTD_sprintf(pstContents->szOptionText, "%s", (HCHAR*)ONDK_GetString(RES_OPTION_01_ID));
	ONDK_GWM_COM_Multi_Create(NX_PRISM_CHLIST_OPTION_TEXT_ID, &stRect);
	ONDK_GWM_COM_Multi_SetContents(NX_PRISM_CHLIST_OPTION_TEXT_ID, (HUINT8*)RES_201_00_OPTION_PNG,
								NULL, (HUINT8*)pstContents->szOptionText, NULL, NX_PRISM_CHLIST_OPTION_IMG_MG);
	ONDK_GWM_COM_Multi_SetFontHandle(NX_PRISM_CHLIST_OPTION_TEXT_ID, eFont_SystemBold, NX_PRISM_FONT_SIZE_22);
	ONDK_GWM_COM_Multi_SetFontColor(NX_PRISM_CHLIST_OPTION_TEXT_ID, COL(C_T_Gong02), COL(C_T_Bang_100));
	ONDK_GWM_COM_Multi_SetFocusFlag(NX_PRISM_CHLIST_OPTION_TEXT_ID, FALSE);
}


static HERROR	nx_chlist_SetUiObject(NxUiChannelListContents_t *pstContents, Handle_t hAct, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Rect_t		stRect = { 0, 0, 0, 0 };

	// frame
	ONDK_GWM_APP_SetAppArea(NX_PRISM_CHLIST_FRAME_X, NX_PRISM_CHLIST_FRAME_Y, NX_PRISM_CHLIST_FRAME_W, NX_PRISM_CHLIST_FRAME_H);

	// Draw - Background Frame
	stRect = ONDK_Rect(NX_PRISM_CHLIST_BG_X, NX_PRISM_CHLIST_FRAME_Y, NX_PRISM_CHLIST_FRAME_W, NX_PRISM_CHLIST_FRAME_H);
	ONDK_GWM_COM_Rect_Create(NX_PRISM_CHLIST_BG_ID, &stRect, 0);
	ONDK_GWM_APP_SetObjectDrawMethod(NX_PRISM_CHLIST_BG_ID, (GWM_ObjDraw_t)nx_chlist_DrawFrameBG_SS);

	// channel list
	ONDK_GWM_COM_ChannelList_Create(NX_PRISM_CHLIST_ID, NX_PRISM_CHLIST_POS_X, NX_PRISM_CHLIST_POS_Y);
	ONDK_GWM_APP_SetObjectDrawMethod(NX_PRISM_CHLIST_ID, (GWM_ObjDraw_t)nx_chlist_DrawChannelListMethod);
	nx_chlist_SetChannelListData(pstContents, TRUE);
	ONDK_GWM_Obj_SetVisible(NX_PRISM_CHLIST_ID, TRUE);
	ONDK_GWM_APP_InvalidateObject(NX_PRISM_CHLIST_ID);

	// Chlist Line
	stRect = ONDK_Rect(NX_PRISM_CHLIST_LIST_LINE_X, NX_PRISM_CHLIST_LIST_LINE_Y,
						NX_PRISM_CHLIST_LIST_LINE_W, NX_PRISM_CHLIST_LIST_LINE_H);
	ONDK_GWM_COM_Image_Create(NX_PRISM_CHLIST_LIST_LINE_ID, &stRect);
	ONDK_GWM_COM_Image_SetImage(NX_PRISM_CHLIST_LIST_LINE_ID, (HCHAR*)RES_107_00_LINE_CONTEXT_L_PNG);
	// Option
	nx_chlist_SetUiCreateOptionMenu(pstContents);

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);
	NX_COMMON_NOT_USED_PARAM(hAct);

	return ERR_OK;
}

static ONDK_Result_t	nx_chlist_DrawFrameBG_SS(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	ONDK_Rect_t			stRect = { 0, 0, 0, 0 };

	stRect.x = area->x1 + rect->x;
	stRect.y = area->y1 + rect->y;
	stRect.w = rect->w;
	stRect.h = rect->h;

	ONDK_GUICOM_VBox(screen, stRect, RES_600_11_LTV_BG01_PNG, RES_600_11_LTV_BG01_PNG, RES_600_11_LTV_BG01_PNG);

	NX_COMMON_NOT_USED_ARGUMENT(screen, area, rect, object);
	NX_COMMON_NOT_USED_PARAM(arg);
	NX_COMMON_NOT_USED_PARAM(bUiTop);

	return	ONDK_RET_OK;
}


static ONDK_Result_t		nx_chlist_DrawChannelListMethod(ONDKWindowHandle *screen, ONDK_Region_t *area, ONDK_Rect_t *rect,
																	ONDK_GWM_GuiObject_t object, void *arg, int param1)
{
	HINT32				i = 0, hFocusExtH = NX_PRISM_CHLIST_FOCUS_ITEM_H, lIconX = 0;
	HINT32				lStartDrawIdx = 0, lDrawCnt = 0;
	//HINT32			eventNamePlusPos = 0;
	HCHAR 				szBuf[NX_PRISM_CHLIST_ITEM_TEXT_BUFFER] = {'\0',};
	ONDK_Rect_t 		drawRect, item_rect, drawCursor;
	ONDKFontHandle		*font = NULL, *fontNBold = NULL, *focusFont = NULL, *lcnFont = NULL, *eventNameFont = NULL;
	ONDKFontHandle		*channelFont = NULL, *nextEventFont = NULL, *hRotateFont = NULL;
	NxChListData_t		*pstChData = NULL;
	NxChListObject_t	*channelList = (NxChListObject_t*)object;
	NxUiChannelListContents_t *pstContents;
	HCHAR				szGroupText[NX_TEXT_SIZE_64];
	HUINT8				stGroupStr[NX_TEXT_SIZE_128] = {'\0',};
	NX_ChListGroup_t	eCurGroup = eNxChListGroup_TV;

	HxLOG_Print("draw channel list\\n\n");
	ONDK_Assert(screen);
	ONDK_Assert(area);
	ONDK_Assert(rect);
	ONDK_Assert(object);

	// Normal Font
	font = ONDK_FONT_CreateSystemFont(eFont_SystemNormal, ONDK_FONT_SIZE_24);
	channelFont = ONDK_FONT_CreateSystemFont(eFont_SystemNormal, ONDK_FONT_SIZE_22);
	hRotateFont = ONDK_FONT_CreateRotationFont(eFont_SystemNormal, ONDK_FONT_SIZE_20, -90);
	if (font == NULL)
	{
		GWM_DBG_Print(DBGMSG_ERROR, ("fail to set a font size.\n"));
		ONDK_FONT_Release(font);
		return GWM_RET_FAIL;
	}

	// Normal Bold Font
	fontNBold = ONDK_FONT_CreateSystemFont(eFont_SystemBold, ONDK_FONT_SIZE_24);
	if (fontNBold == NULL)
	{
		GWM_DBG_Print(DBGMSG_ERROR, ("fail to set a font size.\n"));
		ONDK_FONT_Release(font);
		ONDK_FONT_Release(fontNBold);
		return GWM_RET_FAIL;
	}

	if(hRotateFont == NULL)
	{
		HxLOG_Error("Cannot create Rotate Font\n\n");
		return GWM_RET_FAIL;
	}

	pstContents = nx_chlist_GetContents();

	item_rect.x = channelList->rect.x;
	item_rect.y = channelList->rect.y;
	item_rect.w = NX_PRISM_CHLIST_ITEM_W;
	item_rect.h = channelList->item_col_h;

	if(pstContents->eFocusType == eCHLIST_CHANNELGROUP)
	{
		item_rect.x += NX_PRISM_CHLIST_TRANSFER_X;
	}

	if (channelList->userdata)
	{
		pstChData = (NxChListData_t*)channelList->userdata;
	}
	else
	{
		pstChData = NULL;
	}

	if (channelList->lUserDataCount < NX_PRISM_CHLIST_FOCUS_ITEM_NO)
	{
		lStartDrawIdx = (NX_PRISM_CHLIST_FOCUS_ITEM_NO - channelList->lUserDataCount);
	}

	if(pstContents->eFocusType == eCHLIST_CHANNELLIST)
	{
		//Draw group arrow
		drawRect = ONDK_Rect(NX_PRISM_CHLIST_GROUP_ARROW_X, NX_PRISM_CHLIST_GROUP_ARROW_Y, NX_PRISM_CHLIST_GROUP_ARROW_W, NX_PRISM_CHLIST_GROUP_ARROW_H);
		ONDK_GUICOM_IMG_Blit(screen, drawRect, RES_600_11_LTV_ARROWL_PNG);

		//Draw group text
		eCurGroup = pstContents->eChangedGroup;
		/* eNxChListGroup_AlphabetAll ~ eNxChListGroup_AlphabetEtc is included in the eNxChListGroup_ALPHABET Group */
		if ((eCurGroup >= eNxChListGroup_AlphabetAll) && (eCurGroup <= eNxChListGroup_AlphabetEtc))
		{
			eCurGroup = eNxChListGroup_ALPHABET;
		}

		if(eCurGroup >= eNxChListGroup_Satellite)
		{
			eCurGroup = eNxChListGroup_Satellite;
		}

		NX_PRISM_UTIL_GetResStringIdByChannelListGroupType(eCurGroup, (HCHAR*)stGroupStr);
		HxSTD_sprintf(szGroupText, "%s - %s", (HCHAR*)ONDK_GetString(RES_GROUP_01_ID), (HCHAR*)stGroupStr);
		ONDK_FONT_DrawStringAlignAbbr(screen,hRotateFont,(HCHAR*)szGroupText ,NX_PRISM_CHLIST_GROUP_TEXT_X ,NX_PRISM_CHLIST_GROUP_TEXT_Y ,NX_PRISM_CHLIST_GROUP_TEXT_H,COL(C_T_Gong07),ONDKSTF_LEFT);
	}

	lDrawCnt = 0;
	for (i = 0; i < NX_PRISM_CHLIST_ITEM_MAX; i++)
	{
		if (i < lStartDrawIdx)
		{
			item_rect.y += channelList->item_col_h;
			continue;
		}

		// 채널리스트가 리스트 아이템보다 작으면 처리안함.
		if (lDrawCnt >= channelList->lUserDataCount)
			break;

		if (i == (NX_PRISM_CHLIST_FOCUS_ITEM_NO - 1))	// Draw Focus
		{
			lcnFont	= ONDK_FONT_CreateSystemFont(eFont_SystemBold, ONDK_FONT_SIZE_32);
			if (lcnFont == NULL)
			{
				GWM_DBG_Print(DBGMSG_ERROR, ("fail to set a font size.\n"));
				ONDK_FONT_Release(lcnFont);
				ONDK_FONT_Release(font);
				ONDK_FONT_Release(fontNBold);
				return GWM_RET_FAIL;
			}
			focusFont = ONDK_FONT_CreateSystemFont(eFont_SystemBold, ONDK_FONT_SIZE_40);
			if (focusFont == NULL)
			{
				GWM_DBG_Print(DBGMSG_ERROR, ("fail to set a font size.\n"));
				ONDK_FONT_Release(focusFont);
				ONDK_FONT_Release(lcnFont);
				ONDK_FONT_Release(font);
				ONDK_FONT_Release(fontNBold);
				return GWM_RET_FAIL;
			}
			eventNameFont = ONDK_FONT_CreateSystemFont(eFont_SystemBold, ONDK_FONT_SIZE_30);
			if (eventNameFont == NULL)
			{
				GWM_DBG_Print(DBGMSG_ERROR, ("fail to set a font size.\n"));
				ONDK_FONT_Release(focusFont);
				ONDK_FONT_Release(eventNameFont);
				ONDK_FONT_Release(lcnFont);
				ONDK_FONT_Release(font);
				ONDK_FONT_Release(fontNBold);
				return GWM_RET_FAIL;
			}
			nextEventFont = ONDK_FONT_CreateSystemFont(eFont_SystemNormal, ONDK_FONT_SIZE_24);
			if (nextEventFont == NULL)
			{
				GWM_DBG_Print(DBGMSG_ERROR, ("fail to set a font size.\n"));
				ONDK_FONT_Release(focusFont);
				ONDK_FONT_Release(eventNameFont);
				ONDK_FONT_Release(nextEventFont);
				ONDK_FONT_Release(lcnFont);
				ONDK_FONT_Release(font);
				ONDK_FONT_Release(fontNBold);
				return GWM_RET_FAIL;
			}

			// focus item height 적용.
			item_rect.h += hFocusExtH;
			drawRect = ONDK_Rect(item_rect.x, item_rect.y, NX_PRISM_CHLIST_ITEM_C_CURSOR_W, NX_PRISM_CHLIST_ITEM_C_CURSOR_H);
			//Draw Cursor
			drawCursor = ONDK_Rect(item_rect.x, item_rect.y, NX_PRISM_CHLIST_ITEM_C_CURSOR_W, NX_PRISM_CHLIST_ITEM_C_CURSOR_H);

			if(pstContents->eFocusType == eCHLIST_CHANNELGROUP)
			{
				drawCursor.x += NX_PRISM_CHLIST_ITEM_C_CURSOR_TRANSFER_X;
				drawCursor.w = NX_PRISM_CHLIST_ITEM_C_CURSOR_TRANSFER_W;
			}

			ONDK_DRAW_FillRect(screen, drawCursor, COL(C_G_Base01_100));

			//Draw Cursor Image 01
			drawRect.x += NX_PRISM_CHLIST_ITEM_C_CURSOR_IMG01_X;
			drawRect.w = NX_PRISM_CHLIST_ITEM_C_CURSOR_IMG01_W;
			drawRect.h = NX_PRISM_CHLIST_ITEM_C_CURSOR_IMG01_H;
			ONDK_GUICOM_IMG_Blit(screen, drawRect, RES_600_11_CURSORR1_PNG);

			//Draw Cursor Image 02
			drawRect.x += NX_PRISM_CHLIST_ITEM_C_CURSOR_IMG02_X;
			drawRect.w = NX_PRISM_CHLIST_ITEM_C_CURSOR_IMG02_W;
			drawRect.h = NX_PRISM_CHLIST_ITEM_C_CURSOR_IMG02_H;
			ONDK_GUICOM_IMG_Blit(screen, drawRect, RES_600_11_LTV_CURSOR_PNG);

			if (pstChData != NULL)
			{
				// Focus Channel Number
				szBuf[0] = '\0';
				HxSTD_sprintf(szBuf, "%03d", pstChData[lDrawCnt].lcn);
				ONDK_FONT_DrawStringAlignAbbr(screen, lcnFont, szBuf, item_rect.x + NX_PRISM_CHLIST_ITEM_C_NUMBER_X,
									item_rect.y + NX_PRISM_CHLIST_ITEM_C_TEXT_OFFSET_Y, NX_PRISM_CHLIST_ITEM_C_NUMBER_W,
									COL(C_T_Whistle_100), ONDKSTF_TOPLEFT);

				// Focus Channel Name
				szBuf[0] = '\0';
				if(pstChData[lDrawCnt].pszChName != NULL)
				{
					HxSTD_sprintf(szBuf, "%s", pstChData[lDrawCnt].pszChName);
				}
				else
				{
					HxSTD_sprintf(szBuf, " ");
				}
				ONDK_FONT_DrawStringAlignAbbr(screen, focusFont, szBuf,item_rect.x + NX_PRISM_CHLIST_ITEM_C_NAME_X,
									item_rect.y + NX_PRISM_CHLIST_ITEM_C_NAME_OFFSET_Y, NX_PRISM_CHLIST_ITEM_C_NAME_W,
									COL(C_T_Whistle_100), ONDKSTF_TOPLEFT);
				//Draw Thumbnail
				drawRect = ONDK_Rect(item_rect.x  + NX_PRISM_CHLIST_ITEM_C_THUMBNAIL_X, NX_PRISM_CHLIST_ITEM_C_THUMBNAIL_Y, NX_PRISM_CHLIST_ITEM_C_THUMBNAIL_W, NX_PRISM_CHLIST_ITEM_C_THUMBNAIL_H);
				if(pstChData[lDrawCnt].svcType == eDxSVC_TYPE_TV)
				{
					ONDK_IMG_StretchBlit(pstContents->hTvImg, screen, drawRect);
				}
				else
				{
					ONDK_IMG_StretchBlit(pstContents->hRadioImg, screen, drawRect);
				}
				//Draw Now Program
				//Draw Now Image
				drawRect = ONDK_Rect(item_rect.x + NX_PRISM_CHLIST_ITEM_C_NOW_X, NX_PRISM_CHLIST_ITEM_C_NOW_Y, NX_PRISM_CHLIST_ITEM_C_NOW_W, NX_PRISM_CHLIST_ITEM_C_NOW_H);
				ONDK_GUICOM_IMG_Blit(screen, drawRect, RES_600_11_LTV_NOW_PNG);
				// Focus Channel Event Name
				if (pstChData[lDrawCnt].pszEventName && (HxSTD_StrLen(pstChData[lDrawCnt].pszEventName) > 0))
				{
					ONDK_FONT_DrawStringAlignAbbr(screen, eventNameFont, pstChData[lDrawCnt].pszEventName, item_rect.x + NX_PRISM_CHLIST_ITEM_C_EVENT_X,
							NX_PRISM_CHLIST_ITEM_C_EVENT_Y, NX_PRISM_CHLIST_ITEM_C_EVENT_W, COL(C_T_Whistle_100), ONDKSTF_TOPLEFT);
				}
				else
				{	// 없으면 clear
					HxLOG_Debug("This is Invalid Event!!!\n");
					//ONDK_FONT_DrawStringAlignAbbr(screen, eventNameFont, "UNKNOWN",	item_rect.x + eventNamePlusPos, item_rect.y + 76, (wFont - eventNamePlusPos), ONDK_CHLIST_ITEM_COLOR_T_GREY_02, ONDKSTF_TOPLEFT);
				}

				//Draw Next Program
				//Draw Next Image
				drawRect = ONDK_Rect(item_rect.x + NX_PRISM_CHLIST_ITEM_C_NEXT_X, NX_PRISM_CHLIST_ITEM_C_NEXT_Y, NX_PRISM_CHLIST_ITEM_C_NEXT_W, NX_PRISM_CHLIST_ITEM_C_NEXT_H);
				ONDK_GUICOM_IMG_Blit(screen, drawRect, RES_600_11_LTV_NEXT_PNG);
				// Focus Channel Next Event Name
				if (pstChData[lDrawCnt].pszNextEventName && (HxSTD_StrLen(pstChData[lDrawCnt].pszNextEventName) > 0))
				{
					ONDK_FONT_DrawStringAlignAbbr(screen, nextEventFont, pstChData[lDrawCnt].pszNextEventName, item_rect.x + NX_PRISM_CHLIST_ITEM_C_NEXT_EVENT_X,
							NX_PRISM_CHLIST_ITEM_C_NEXT_EVENT_Y, NX_PRISM_CHLIST_ITEM_C_NEXT_EVENT_W, COL(C_T_Gong04), ONDKSTF_TOPLEFT);
				}

				//Draw Premium
				drawRect = ONDK_Rect(item_rect.x + NX_PRISM_CHLIST_ITEM_C_PREMIUM_X, NX_PRISM_CHLIST_ITEM_C_PREMIUM_Y, NX_PRISM_CHLIST_ITEM_C_PREMIUM_W, NX_PRISM_CHLIST_ITEM_C_PREMIUM_H);
				ONDK_GUICOM_IMG_Blit(screen, drawRect, RES_600_11_LTV_PREMIUM_PNG);

				// set cas image
				if(channelList->hCasImg)
				{
					drawRect = ONDK_Rect(item_rect.x + NX_PRISM_CHLIST_ITEM_C_ICON_CAS_X, NX_PRISM_CHLIST_ITEM_C_ICON_CAS_Y, NX_PRISM_CHLIST_ITEM_C_ICON_CAS_W, NX_PRISM_CHLIST_ITEM_C_ICON_CAS_H);
					ONDK_IMG_StretchBlit(channelList->hCasImg, screen, drawRect);
				}

				// set Lock image
				if(channelList->hLockImg)
				{
					drawRect = ONDK_Rect(item_rect.x + NX_PRISM_CHLIST_ITEM_C_ICON_LOCK_X, NX_PRISM_CHLIST_ITEM_C_ICON_LOCK_Y, NX_PRISM_CHLIST_ITEM_C_ICON_LOCK_W, NX_PRISM_CHLIST_ITEM_C_ICON_LOCK_H);
					ONDK_IMG_StretchBlit(channelList->hLockImg, screen, drawRect);
					lIconX += NX_PRISM_CHLIST_ITEM_C_ICON_CAS_W;
				}
			}

			item_rect.y += item_rect.h;
			item_rect.h = channelList->item_col_h;

			ONDK_FONT_Release(lcnFont);
			ONDK_FONT_Release(focusFont);
			ONDK_FONT_Release(eventNameFont);
			ONDK_FONT_Release(nextEventFont);
		}
		else
		{
			if (channelList->userdata)
			{
				if (pstChData != NULL)
				{
					HxLOG_Debug("pstChData : %s , x(%d), y (%d)\n", pstChData[lDrawCnt].pszChName, item_rect.x, item_rect.y);
					szBuf[0] = '\0';
					HxSTD_sprintf(szBuf, "%03d", pstChData[lDrawCnt].lcn);
					ONDK_FONT_DrawStringAlignAbbr(screen, channelFont, szBuf, item_rect.x + NX_PRISM_CHLIST_ITEM_NUMBER_X,
										item_rect.y,NX_PRISM_CHLIST_ITEM_NUMBER_W, COL(C_T_Gong03), ONDKSTF_TOPLEFT);

					szBuf[0] = '\0';
					HxSTD_sprintf(szBuf, "%s", pstChData[lDrawCnt].pszChName);
					ONDK_FONT_DrawStringAlignAbbr(screen, fontNBold, szBuf, item_rect.x + NX_PRISM_CHLIST_ITEM_NAME_X,
										item_rect.y - 2, NX_PRISM_CHLIST_ITEM_NAME_W, COL(C_T_Whistle_100), ONDKSTF_TOPLEFT);
				}
				else
				{
					HxLOG_Debug("\n[%s.%d] user data is NULL \n",__FUNCTION__,__LINE__);
				}
			}
			else
			{
				HxLOG_Debug("\n[%s.%d] user data is NULL \n",__FUNCTION__,__LINE__);
			}

			item_rect.y += channelList->item_col_h;
		}

		lDrawCnt++;
	}
	ONDK_FONT_Release(font);
	ONDK_FONT_Release(channelFont);
	ONDK_FONT_Release(fontNBold);;

	NOT_USED_PARAM(param1);
	NOT_USED_PARAM(arg);

	return GWM_RET_OK;
}

static ONDK_Result_t	nx_chlist_AddDimImage(NxUiChannelListContents_t *pstContents)
{
	ONDK_Rect_t		stRect = { 0, 0, 0, 0 };

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		return	GWM_RET_SOURCE_NULL;
	}

	// Draw - Mask Image
	stRect = ONDK_Rect(NX_PRISM_CHLIST_DIM_X, NX_PRISM_CHLIST_DIM_Y, NX_PRISM_CHLIST_DIM_W, NX_PRISM_CHLIST_DIM_H);

	ONDK_GWM_COM_Rect_Create(NX_PRISM_CHLIST_DIM_ID, &stRect, 0);
	ONDK_GWM_APP_SetObjectDrawMethod(NX_PRISM_CHLIST_DIM_ID, nx_chlist_DrawDimImage);

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_chlist_DeleteDimImage(NxUiChannelListContents_t *pstContents)
{
	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		return	GWM_RET_SOURCE_NULL;
	}

	ONDK_GWM_Obj_Destroy(NX_PRISM_CHLIST_DIM_ID);

	return	ONDK_RET_OK;
}

static	ONDK_Result_t	nx_chlist_DrawDimImage(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	ONDK_Rect_t			stRect = { 0, 0, 0, 0 };

	stRect.x = area->x1 + rect->x;
	stRect.y = area->y1 + rect->y;
	stRect.w = rect->w;
	stRect.h = rect->h;

	ONDK_GUICOM_VBox(screen, stRect, RES_600_11_LTV_BG02_PNG, RES_600_11_LTV_BG02_PNG, RES_600_11_LTV_BG02_PNG);

	NX_COMMON_NOT_USED_ARGUMENT(screen, area, rect, object);
	NX_COMMON_NOT_USED_PARAM(arg);
	NX_COMMON_NOT_USED_PARAM(bUiTop);

	return	ONDK_RET_OK;
}

static ONDK_Result_t    nx_chlist_DrawChanelGroupList(ONDKSurfaceHandle *screen, ONDK_Region_t *area, ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	/* List variable */
	HINT32          nTotal = 0, nStartIdx = 0, nCurrIdx = 0, nSelectedIdx = 0, nNumOfPage = 0;
	HINT32          nCellHeight = 0, nCount = 0, nOffsetY = 0, lScroll_height = 0;
	LIST_OBJECT     objList = (LIST_OBJECT)object;
	HxVector_t      *pstDataList = NULL;
	NxChList_ChGroupItem_t *pstData = NULL;

	ONDK_Color_t        textFontColor = 0x00000000, bgColor = 0x00000000;
	ONDKFontHandle     *hFontHandle = NULL, *hBoldFontHandle = NULL, *hNormalFontHandle = NULL;
	ONDK_Rect_t         stListRegion = {0, 0, 0, 0};
	HINT32              i = 0, nDataIdx = 0;
	NxUiChannelListContents_t* pstContents = NULL;
	HBOOL bNeedScrollBar = FALSE;

	NOT_USED_PARAM(arg);
	pstContents = nx_chlist_GetContents();

	/* create font handler */
	hBoldFontHandle = ONDK_FONT_CreateSystemFont(eFont_SystemBold, NX_PRISM_FONT_SIZE_20);
	hNormalFontHandle = ONDK_FONT_CreateSystemFont(eFont_SystemNormal, NX_PRISM_FONT_SIZE_20);

	hFontHandle = hNormalFontHandle;
	/* init list coordinate */
	stListRegion.x = area->x1 + rect->x;
	stListRegion.y = area->y1 + rect->y;
	stListRegion.w = rect->w;
	stListRegion.h = rect->h;

	nOffsetY = NX_PRISM_CHLIST_CHANNELGROUP_ITEM_H;
	/* get list data */
	pstDataList = (HxVector_t *)ONDK_GWM_List_GetArrayPtr(objList);
	ONDK_GWM_List_GetProperty (objList, &nTotal, &nStartIdx, &nCurrIdx, &nSelectedIdx, &nNumOfPage);

	if (pstDataList == NULL || nTotal == 0)
	{
		NX_APP_Error("ERROR!!! (pstDataList == NULL) || (nTotal == 0) !!\n");
		return ONDK_RET_FAIL;
	}

	nCellHeight = ONDK_GWM_List_GetCellHeight(ONDK_GWM_Obj_GetObjectID((ONDK_GWM_GuiObject_t)objList));
	/* index setting */
	if (nTotal > nNumOfPage)
	{
		nCount = nNumOfPage;
		bNeedScrollBar = TRUE;
		lScroll_height = rect->h;
	}
	else
	{
		nCount = nTotal;
		bNeedScrollBar = FALSE;
	}

	//Draw  Menu BG Top
	ONDK_DRAW_FillRectElement(screen, stListRegion.x, stListRegion.y, stListRegion.w, NX_PRISM_CHLIST_CHANNELGROUP_BG_TOP_H, COL(C_G_Whistle_10));

	for (i = 0 ; i < nCount ; i++)
	{
		/* init data index */
		nDataIdx = nStartIdx + i;
		if (nDataIdx >= nTotal)
		{
			break;
		}
		pstData = HLIB_VECTOR_ItemAt(pstDataList, nDataIdx);

		/* make cursor data */

		if (nDataIdx == nCurrIdx) /* focus */
		{
			textFontColor = COL(C_T_Whistle_100);
			bgColor = COL(C_G_Base01_100);
			hFontHandle = hBoldFontHandle;
		}
		else
		{
			textFontColor = COL(C_T_Gong04);
			bgColor = COL(C_G_Whistle_10);
			hFontHandle = hNormalFontHandle;
		}

		nx_chlist_DrawCursor(screen, pstData, &stListRegion, nOffsetY, hFontHandle,textFontColor, bgColor,nCellHeight, ((nDataIdx == nCurrIdx)), bUiTop);
		nOffsetY += nCellHeight + 2;
		bgColor = 0x00000000;
	}

	//Draw BG Menu Bottom
	//Draw  Menu BG Top
	ONDK_DRAW_FillRectElement(screen, stListRegion.x, stListRegion.y + nOffsetY, stListRegion.w, NX_PRISM_CHLIST_BG_H - (stListRegion.y + nOffsetY), COL(C_G_Whistle_10));

	ONDK_FONT_Release(hNormalFontHandle);
	ONDK_FONT_Release(hBoldFontHandle);

	return  ONDK_RET_OK;
}

static ONDK_Result_t    nx_chlist_DrawCursor(ONDKSurfaceHandle *screen, NxChList_ChGroupItem_t *pstData, ONDK_Rect_t *stListRegion, HINT32 nOffsetY, ONDKFontHandle *hFontHandle, ONDK_Color_t textFontColor, ONDK_Color_t bgColor, HUINT32 ulCellHeight, HBOOL bIsFocus, int bUiTop)
{
	NxUiChannelListContents_t	*pstContents = NULL;
	ONDK_Rect_t     stRectText = {0,0,0,0};

	pstContents = nx_chlist_GetContents();

	ONDK_DRAW_FillRectElement(screen, stListRegion->x, stListRegion->y + nOffsetY, stListRegion->w, ulCellHeight, bgColor);

	//Draw Text
	stRectText.x = stListRegion->x + NX_PRISM_CHLIST_CHANNELGROUP_ITEM_TEXT_X;
	stRectText.y = stListRegion->y + nOffsetY + NX_PRISM_CHLIST_CHANNELGROUP_ITEM_TEXT_Y;
	stRectText.w = NX_PRISM_CHLIST_CHANNELGROUP_ITEM_TEXT_W;

	ONDK_FONT_DrawStringAlignAbbr(screen, hFontHandle, (char*)pstData->szChannelGroupName, stRectText.x, stRectText.y, stRectText.w, textFontColor, ONDKSTF_LEFT);

	return ONDK_RET_OK;
}

static HUINT32 nx_chlist_GetFocusIndexOfChannelGroup(NxUiChannelListContents_t *pstContents)
{
	NX_ChListGroup_t	eCurGroup = eNxChListGroup_TV;
	HUINT32 	ulIndex = 0, ulLen = 0;
	NxChList_ChGroupItem_t *pstChGroupItem = NULL;

	eCurGroup = pstContents->eChangedGroup;

	/* eNxChListGroup_AlphabetAll ~ eNxChListGroup_AlphabetEtc is included in the eNxChListGroup_ALPHABET Group */
	if ((eCurGroup >= eNxChListGroup_AlphabetAll) && (eCurGroup <= eNxChListGroup_AlphabetEtc))
	{
		eCurGroup = eNxChListGroup_ALPHABET;
	}

	if(eCurGroup >= eNxChListGroup_Satellite)
	{
		eCurGroup = eNxChListGroup_Satellite;
	}

	ulLen = HLIB_VECTOR_Length(pstContents->pstChannelGroupList);

	for(ulIndex = 0; ulIndex < ulLen; ulIndex++)
	{
		pstChGroupItem = HLIB_VECTOR_ItemAt(pstContents->pstChannelGroupList, ulIndex);
		if(pstChGroupItem->eChannelGroupIndex == eCurGroup)
			return ulIndex;
	}

	return ulIndex;

}

static ONDK_Result_t	nx_chlist_InitChannelGroupList(NxUiChannelListContents_t *pstContents)
{
	NxChList_ChGroupItem_t *pstChGroupItem = NULL;
	HUINT32			ulCount = 0;

	if(pstContents->pstChannelGroupList == NULL)
	{
		pstContents->pstChannelGroupList = HLIB_VECTOR_NewEx(NULL, 0, NULL, (void *(*)(void *))DLIB_Free_CB, NULL);
	}

	pstChGroupItem = (NxChList_ChGroupItem_t *)NX_APP_Malloc (sizeof(NxChList_ChGroupItem_t));
	if (pstChGroupItem == NULL)		{ return GWM_RET_OUT_OF_MEMORY; }
	HxSTD_MemSet(pstChGroupItem, 0, sizeof(NxChList_ChGroupItem_t));
	pstChGroupItem->eChannelGroupIndex = eNxChListGroup_ALL;
	NX_PRISM_UTIL_GetResStringIdByChannelListGroupType((NX_ChListGroup_t)eNxChListGroup_ALL,pstChGroupItem->szChannelGroupName);
	HLIB_VECTOR_Add(pstContents->pstChannelGroupList, pstChGroupItem);

	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_MBC);
	if (ulCount >= 0)
	{
		pstChGroupItem = (NxChList_ChGroupItem_t *)NX_APP_Malloc (sizeof(NxChList_ChGroupItem_t));
		if (pstChGroupItem == NULL)		{ return GWM_RET_OUT_OF_MEMORY; }
		HxSTD_MemSet(pstChGroupItem, 0, sizeof(NxChList_ChGroupItem_t));
		pstChGroupItem->eChannelGroupIndex = eNxChListGroup_MBC;
		NX_PRISM_UTIL_GetResStringIdByChannelListGroupType((NX_ChListGroup_t)eNxChListGroup_MBC,pstChGroupItem->szChannelGroupName);
		HLIB_VECTOR_Add(pstContents->pstChannelGroupList, pstChGroupItem);
	}

	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_Movie);
	if (ulCount >= 0)
	{
		pstChGroupItem = (NxChList_ChGroupItem_t *)NX_APP_Malloc (sizeof(NxChList_ChGroupItem_t));
		if (pstChGroupItem == NULL)		{ return GWM_RET_OUT_OF_MEMORY; }
		HxSTD_MemSet(pstChGroupItem, 0, sizeof(NxChList_ChGroupItem_t));
		pstChGroupItem->eChannelGroupIndex = eNxChListGroup_Movie;
		NX_PRISM_UTIL_GetResStringIdByChannelListGroupType((NX_ChListGroup_t)eNxChListGroup_Movie,pstChGroupItem->szChannelGroupName);
		HLIB_VECTOR_Add(pstContents->pstChannelGroupList, pstChGroupItem);
	}

	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_Kids);
	if (ulCount >= 0)
	{
		pstChGroupItem = (NxChList_ChGroupItem_t *)NX_APP_Malloc (sizeof(NxChList_ChGroupItem_t));
		if (pstChGroupItem == NULL)		{ return GWM_RET_OUT_OF_MEMORY; }
		HxSTD_MemSet(pstChGroupItem, 0, sizeof(NxChList_ChGroupItem_t));
		pstChGroupItem->eChannelGroupIndex = eNxChListGroup_Kids;
		NX_PRISM_UTIL_GetResStringIdByChannelListGroupType((NX_ChListGroup_t)eNxChListGroup_Kids,pstChGroupItem->szChannelGroupName);
		HLIB_VECTOR_Add(pstContents->pstChannelGroupList, pstChGroupItem);
	}

	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_Sports);
	if (ulCount >= 0)
	{
		pstChGroupItem = (NxChList_ChGroupItem_t *)NX_APP_Malloc (sizeof(NxChList_ChGroupItem_t));
		if (pstChGroupItem == NULL)		{ return GWM_RET_OUT_OF_MEMORY; }
		HxSTD_MemSet(pstChGroupItem, 0, sizeof(NxChList_ChGroupItem_t));
		pstChGroupItem->eChannelGroupIndex = eNxChListGroup_Sports;
		NX_PRISM_UTIL_GetResStringIdByChannelListGroupType((NX_ChListGroup_t)eNxChListGroup_Sports,pstChGroupItem->szChannelGroupName);
		HLIB_VECTOR_Add(pstContents->pstChannelGroupList, pstChGroupItem);
	}

	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_ALPHABET);
	if (ulCount > 0)
	{
		pstChGroupItem = (NxChList_ChGroupItem_t *)NX_APP_Malloc (sizeof(NxChList_ChGroupItem_t));
		if (pstChGroupItem == NULL)		{ return GWM_RET_OUT_OF_MEMORY; }
		HxSTD_MemSet(pstChGroupItem, 0, sizeof(NxChList_ChGroupItem_t));
		pstChGroupItem->eChannelGroupIndex = eNxChListGroup_ALPHABET;
		NX_PRISM_UTIL_GetResStringIdByChannelListGroupType((NX_ChListGroup_t)eNxChListGroup_ALPHABET,pstChGroupItem->szChannelGroupName);
		HLIB_VECTOR_Add(pstContents->pstChannelGroupList, pstChGroupItem);
	}

	if(pstContents->ulNumAnt > 0)
	{
		pstChGroupItem = (NxChList_ChGroupItem_t *)NX_APP_Malloc (sizeof(NxChList_ChGroupItem_t));
		if (pstChGroupItem == NULL)		{ return GWM_RET_OUT_OF_MEMORY; }
		HxSTD_MemSet(pstChGroupItem, 0, sizeof(NxChList_ChGroupItem_t));
		pstChGroupItem->eChannelGroupIndex = eNxChListGroup_Satellite;
		NX_PRISM_UTIL_GetResStringIdByChannelListGroupType((NX_ChListGroup_t)eNxChListGroup_Satellite,pstChGroupItem->szChannelGroupName);
		HLIB_VECTOR_Add(pstContents->pstChannelGroupList, pstChGroupItem);
	}

	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_HDTV);
	if (ulCount > 0)
	{
		pstChGroupItem = (NxChList_ChGroupItem_t *)NX_APP_Malloc (sizeof(NxChList_ChGroupItem_t));
		if (pstChGroupItem == NULL)		{ return GWM_RET_OUT_OF_MEMORY; }
		HxSTD_MemSet(pstChGroupItem, 0, sizeof(NxChList_ChGroupItem_t));
		pstChGroupItem->eChannelGroupIndex = eNxChListGroup_HDTV;
		NX_PRISM_UTIL_GetResStringIdByChannelListGroupType((NX_ChListGroup_t)eNxChListGroup_HDTV,pstChGroupItem->szChannelGroupName);
		HLIB_VECTOR_Add(pstContents->pstChannelGroupList, pstChGroupItem);
	}

	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_Favorite1);
	if (ulCount > 0)
	{
		pstChGroupItem = (NxChList_ChGroupItem_t *)NX_APP_Malloc (sizeof(NxChList_ChGroupItem_t));
		if (pstChGroupItem == NULL)		{ return GWM_RET_OUT_OF_MEMORY; }
		HxSTD_MemSet(pstChGroupItem, 0, sizeof(NxChList_ChGroupItem_t));
		pstChGroupItem->eChannelGroupIndex = eNxChListGroup_Favorite1;
		NX_PRISM_UTIL_GetResStringIdByChannelListGroupType((NX_ChListGroup_t)eNxChListGroup_Favorite1,pstChGroupItem->szChannelGroupName);
		HLIB_VECTOR_Add(pstContents->pstChannelGroupList, pstChGroupItem);
	}

	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_Favorite2);
	if (ulCount > 0)
	{
		pstChGroupItem = (NxChList_ChGroupItem_t *)NX_APP_Malloc (sizeof(NxChList_ChGroupItem_t));
		if (pstChGroupItem == NULL)		{ return GWM_RET_OUT_OF_MEMORY; }
		HxSTD_MemSet(pstChGroupItem, 0, sizeof(NxChList_ChGroupItem_t));
		pstChGroupItem->eChannelGroupIndex = eNxChListGroup_Favorite2;
		NX_PRISM_UTIL_GetResStringIdByChannelListGroupType((NX_ChListGroup_t)eNxChListGroup_Favorite2,pstChGroupItem->szChannelGroupName);
		HLIB_VECTOR_Add(pstContents->pstChannelGroupList, pstChGroupItem);
	}

	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_Favorite3);
	if (ulCount > 0)
	{
		pstChGroupItem = (NxChList_ChGroupItem_t *)NX_APP_Malloc (sizeof(NxChList_ChGroupItem_t));
		if (pstChGroupItem == NULL)		{ return GWM_RET_OUT_OF_MEMORY; }
		HxSTD_MemSet(pstChGroupItem, 0, sizeof(NxChList_ChGroupItem_t));
		pstChGroupItem->eChannelGroupIndex = eNxChListGroup_Favorite3;
		NX_PRISM_UTIL_GetResStringIdByChannelListGroupType((NX_ChListGroup_t)eNxChListGroup_Favorite3,pstChGroupItem->szChannelGroupName);
		HLIB_VECTOR_Add(pstContents->pstChannelGroupList, pstChGroupItem);
	}

	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_Favorite4);
	if (ulCount > 0)
	{
		pstChGroupItem = (NxChList_ChGroupItem_t *)NX_APP_Malloc (sizeof(NxChList_ChGroupItem_t));
		if (pstChGroupItem == NULL)		{ return GWM_RET_OUT_OF_MEMORY; }
		HxSTD_MemSet(pstChGroupItem, 0, sizeof(NxChList_ChGroupItem_t));
		pstChGroupItem->eChannelGroupIndex = eNxChListGroup_Favorite4;
		NX_PRISM_UTIL_GetResStringIdByChannelListGroupType((NX_ChListGroup_t)eNxChListGroup_Favorite4,pstChGroupItem->szChannelGroupName);
		HLIB_VECTOR_Add(pstContents->pstChannelGroupList, pstChGroupItem);
	}

	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_Favorite5);
	if (ulCount > 0)
	{
		pstChGroupItem = (NxChList_ChGroupItem_t *)NX_APP_Malloc (sizeof(NxChList_ChGroupItem_t));
		if (pstChGroupItem == NULL)		{ return GWM_RET_OUT_OF_MEMORY; }
		HxSTD_MemSet(pstChGroupItem, 0, sizeof(NxChList_ChGroupItem_t));
		pstChGroupItem->eChannelGroupIndex = eNxChListGroup_Favorite5;
		NX_PRISM_UTIL_GetResStringIdByChannelListGroupType((NX_ChListGroup_t)eNxChListGroup_Favorite5,pstChGroupItem->szChannelGroupName);
		HLIB_VECTOR_Add(pstContents->pstChannelGroupList, pstChGroupItem);
	}

	return ONDK_RET_OK;
}

static ONDK_Result_t nx_chlist_CreateChannelGroupUI(NxUiChannelListContents_t *pstContents)
{

	ONDK_Rect_t 	stRect = {0, 0, 0, 0};
	HUINT32			nTotal = 0;

	nTotal = HLIB_VECTOR_Length(pstContents->pstChannelGroupList);
		// Create first menu
	stRect = ONDK_Rect(NX_PRISM_CHLIST_CHANNELGROUP_X, NX_PRISM_CHLIST_CHANNELGROUP_Y, NX_PRISM_CHLIST_CHANNELGROUP_W, NX_PRISM_CHLIST_CHANNELGROUP_H);
	ONDK_GWM_List_Create(NX_PRISM_CHLIST_CHANNELGROUP_ID, &stRect, "CHANNEL GROUP LIST", NX_PRISM_CHLIST_CHANNELGROUP_ITEM_H, nTotal, NX_PRISM_CHLIST_CHANNELGROUP_COUNT, (GWM_LIST_ITEMS)pstContents->pstChannelGroupList);
	ONDK_GWM_APP_SetObjectDrawMethod(NX_PRISM_CHLIST_CHANNELGROUP_ID, (GWM_ObjDraw_t)nx_chlist_DrawChanelGroupList);
	ONDK_GWM_List_SetCircularMove(NX_PRISM_CHLIST_CHANNELGROUP_ID, TRUE);
	ONDK_GWM_Obj_SetFocus(NX_PRISM_CHLIST_CHANNELGROUP_ID);

	return ONDK_RET_OK;
}

static void		nx_chlist_OptionLoadAlphabetGroupInitDataDefault(NxUiChannelListContents_t *pstContents)
{
	HUINT32			ulCount = 0;

	HxSTD_MemSet(&pstContents->stAlphabetOptList, 0x00, sizeof(NX_Util_OPTIONList_t));

	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_AlphabetAll);
	if (ulCount > 0)	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stAlphabetOptList, eNxChListGroup_AlphabetAll);
	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_AlphabetA);
	if (ulCount > 0)	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stAlphabetOptList, eNxChListGroup_AlphabetA);
	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_AlphabetB);
	if (ulCount > 0)	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stAlphabetOptList, eNxChListGroup_AlphabetB);
	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_AlphabetC);
	if (ulCount > 0)	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stAlphabetOptList, eNxChListGroup_AlphabetC);
	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_AlphabetD);
	if (ulCount > 0)	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stAlphabetOptList, eNxChListGroup_AlphabetD);
	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_AlphabetE);
	if (ulCount > 0)	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stAlphabetOptList, eNxChListGroup_AlphabetE);
	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_AlphabetF);
	if (ulCount > 0)	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stAlphabetOptList, eNxChListGroup_AlphabetF);
	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_AlphabetG);
	if (ulCount > 0)	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stAlphabetOptList, eNxChListGroup_AlphabetG);
	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_AlphabetH);
	if (ulCount > 0)	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stAlphabetOptList, eNxChListGroup_AlphabetH);
	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_AlphabetI);
	if (ulCount > 0)	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stAlphabetOptList, eNxChListGroup_AlphabetI);
	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_AlphabetJ);
	if (ulCount > 0)	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stAlphabetOptList, eNxChListGroup_AlphabetJ);
	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_AlphabetK);
	if (ulCount > 0)	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stAlphabetOptList, eNxChListGroup_AlphabetK);
	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_AlphabetL);
	if (ulCount > 0)	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stAlphabetOptList, eNxChListGroup_AlphabetL);
	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_AlphabetM);
	if (ulCount > 0)	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stAlphabetOptList, eNxChListGroup_AlphabetM);
	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_AlphabetN);
	if (ulCount > 0)	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stAlphabetOptList, eNxChListGroup_AlphabetN);
	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_AlphabetO);
	if (ulCount > 0)	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stAlphabetOptList, eNxChListGroup_AlphabetO);
	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_AlphabetP);
	if (ulCount > 0)	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stAlphabetOptList, eNxChListGroup_AlphabetP);
	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_AlphabetQ);
	if (ulCount > 0)	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stAlphabetOptList, eNxChListGroup_AlphabetQ);
	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_AlphabetR);
	if (ulCount > 0)	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stAlphabetOptList, eNxChListGroup_AlphabetR);
	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_AlphabetS);
	if (ulCount > 0)	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stAlphabetOptList, eNxChListGroup_AlphabetS);
	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_AlphabetT);
	if (ulCount > 0)	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stAlphabetOptList, eNxChListGroup_AlphabetT);
	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_AlphabetU);
	if (ulCount > 0)	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stAlphabetOptList, eNxChListGroup_AlphabetU);
	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_AlphabetV);
	if (ulCount > 0)	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stAlphabetOptList, eNxChListGroup_AlphabetV);
	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_AlphabetW);
	if (ulCount > 0)	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stAlphabetOptList, eNxChListGroup_AlphabetW);
	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_AlphabetX);
	if (ulCount > 0)	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stAlphabetOptList, eNxChListGroup_AlphabetX);
	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_AlphabetY);
	if (ulCount > 0)	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stAlphabetOptList, eNxChListGroup_AlphabetY);
	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_AlphabetZ);
	if (ulCount > 0)	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stAlphabetOptList, eNxChListGroup_AlphabetZ);
	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_AlphabetDigit);
	if (ulCount > 0)	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stAlphabetOptList, eNxChListGroup_AlphabetDigit);
	ulCount = NX_CHANNEL_GROUP_GetGroupCount(eNxChListGroup_AlphabetEtc);
	if (ulCount > 0)	NX_PRISM_UTIL_OptionAddItemList(&pstContents->stAlphabetOptList, eNxChListGroup_AlphabetEtc);

}

static void		nx_chlist_CreateOptionChangeAlphabetGroupListPopup(NxUiChannelListContents_t *pstContents)
{
	HINT32		lLineCnt = 0;
	HUINT32		ulTextListNum = 0, ulIndex = 0, ulAttribute = 0, i = 0, ulMaxListNum = 0;
	HUINT8		**pptextList = NULL;
	NX_ChListGroup_t	eCurGroup = eNxChListGroup_TV;
	ONDK_Rect_t	stRect = { 0, 0, 0, 0 }, stListRect = { 0, 0, 0, 0 };


	if (pstContents->stAlphabetOptList.ulOptItemNum == 0 )
	{
		NX_APP_Info("Error!!! Please Check added Option item !!\n");
		return;
	}

	ulTextListNum = pstContents->stAlphabetOptList.ulOptItemNum;
	if (pstContents->szItemTextList == NULL)
	{
		/* Make Option Test List */
		pstContents->szItemTextList = NX_PRISM_UTIL_OptionMakeChannlGroupItemTextList(eNxUtil_OptType_TEXT, &pstContents->stAlphabetOptList);
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
	eCurGroup = pstContents->eChangedGroup;

	for (i = 0; i<pstContents->stAlphabetOptList.ulOptItemNum; i++)
	{
		if (pstContents->stAlphabetOptList.aeOptItemList[i] == (HUINT32)eCurGroup)
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

	NX_PRISM_COMP_ListDlg_SetListPopup(&s_stUiChList_OptContents, ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis()),
							&stRect, (NX_PRISM_UI_DLG_MULTISEL_MSG_LIST_H + NX_PRISM_UI_DLG_MULTISEL_MSG_CELL_MG),
							ulAttribute, ulTextListNum, ulMaxListNum, ulIndex, (HUINT32)pptextList);

	NX_PRISM_COMP_ListDlg_SetFontSize(&s_stUiChList_OptContents, NX_PRISM_FONT_SIZE_22);
	NX_PRISM_COMP_ListDlg_SetTitle(&s_stUiChList_OptContents, ONDK_GetString(RES_MESG_5650_ID));

	if (ulAttribute & eListDlg_OSD_USER)
	{
		NX_PRISM_COMP_ListDlg_SetExternalArea(&s_stUiChList_OptContents, &stListRect);
	}

	/* Create Pop Up */
	ONDK_GWM_APP_Create (ONDK_UPPER_WINDOW, "nx_guide_DisplayListPopup", APP_DEFAULT_POPUP_PRIORITY, nx_chlist_DisplayListPopup, 0, 0, 0, 0);

	//ONDK_GWM_KillTimer(NX_PRISM_CHLIST_EXIT_TIMER_ID);
}

static ONDK_Result_t	nx_chlist_DisplayListPopup(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return	NX_PRISM_COMP_ListDlg_ProtoProc(&s_stUiChList_OptContents, message, hAction, p1, p2, p3);
}

static ONDK_Result_t	nx_chlist_LoadAntennaInfos(NxUiChannelListContents_t *pstContents)
{
	HINT32				i = 0, lRealAntNum = 0, lUserAntCount = 0, lAntIdx = NX_ANT_IDX_NULL;
	HERROR				hError = ERR_FAIL;
	DxAntInfo_t			stAntInfo;

	HxSTD_MemSet(&pstContents->stSatOptList, 0x00, sizeof(NX_Util_OPTIONList_t));

	pstContents->ulNumAnt = 0;
	HxSTD_memset (pstContents->astAntInfo, 0x00, sizeof(DxAntInfo_t) * NX_PRISM_CHLIST_SRCHSAT_FIXED_MAX_ANT_NUM_IN_UI);

	// Init Antenna Value
	for (i = 0; i < NX_PRISM_CHLIST_SRCHSAT_FIXED_MAX_ANT_NUM_IN_UI; i++)
	{
		// Antenna Index
		pstContents->alAntIdxArray[i] = NX_ANT_IDX_NULL;

		// Antenna Info
		pstContents->astAntInfo[i].uid = NX_ANT_IDX_NULL;
		HxSTD_StrCpy(pstContents->astAntInfo[i].satName, (char*)NX_SAT_SATNAME_NONE);
		HxSTD_StrCpy(pstContents->astAntInfo[i].satType, (char*)NX_SAT_SATNAME_NONE);

	}

	// Load Antenna Index
	lRealAntNum = 0;
	lUserAntCount = NX_ANTENNA_GetUserAntCountUDB();
	NX_APP_Debug("[%s:%d] NX_ANTENNA_GetUserAntCountUDB() [%d]\n", __FUNCTION__, __LINE__, lUserAntCount);
	for (i = 0; i < lUserAntCount; i++)
	{
		lAntIdx = NX_ANT_IDX_NULL;
		hError = NX_ANTENNA_GetUserAntIndexUDB((eNxSat_ANTENAA_SLOT1 + i), &lAntIdx);
		if (hError == ERR_OK)
		{
			pstContents->alAntIdxArray[lRealAntNum] = lAntIdx;
			lRealAntNum++;
			NX_APP_Debug("[%s:%d] NX_ANTENNA_GetUserAntIndexUDB()lRealAntNum[%d] = [%d]\n", __FUNCTION__, __LINE__, lRealAntNum, lAntIdx);
		}
		else
		{
			NX_APP_Debug("[%s:%d] NX_ANTENNA_GetUserAntIndexUDB() Not found Antenna [%d]\n", __FUNCTION__, __LINE__, (eNxSat_ANTENAA_SLOT1 + i));
		}
	}

	NX_APP_Debug("[%s:%d] lRealAntNum : [%d]\n", __FUNCTION__, __LINE__, lRealAntNum);

	// Load Antenna Info
	for (i = 0; i < lRealAntNum; i++)
	{
		HxSTD_memset(&stAntInfo, 0x00, sizeof(DxAntInfo_t));
		hError = NX_ANTENNA_GetInfo(pstContents->alAntIdxArray[i], &stAntInfo);
		if ((hError == ERR_OK) && (HxSTD_StrCmp(stAntInfo.satName, (char*)NX_SAT_SATNAME_NONE) != 0))
		{
			pstContents->astAntInfo[i] = stAntInfo;
			NX_PRISM_UTIL_OptionAddItemList(&pstContents->stSatOptList, eNxChListGroup_Satellite | i);
		}
	}
	pstContents->ulNumAnt = lRealAntNum;

	return	ONDK_RET_OK;
}

static void		nx_chlist_CreateOptionSatelliteGroupListPopup(NxUiChannelListContents_t *pstContents)
{
	HINT32		lLineCnt = 0;
	HUINT32		ulTextListNum = 0, ulIndex = 0, ulAttribute = 0, i = 0, ulMaxListNum = 0;
	HUINT8		**pptextList = NULL;
	NX_ChListGroup_t	eCurGroup = eNxChListGroup_TV;
	ONDK_Rect_t	stRect = { 0, 0, 0, 0 }, stListRect = { 0, 0, 0, 0 };

	HxLOG_Print("num of sat(%d)\n\n",pstContents->stSatOptList.ulOptItemNum);
	if (pstContents->stSatOptList.ulOptItemNum == 0 )
	{
		NX_APP_Info("Error!!! Please Check added Option item !!\n");
		return;
	}

	ulTextListNum = pstContents->stSatOptList.ulOptItemNum;
	if (pstContents->szSatItemTextList == NULL)
	{
		/* Make Option Test List */
		pstContents->szSatItemTextList = nx_chlist_OptionMakeSatelliteGroupItemTextList(pstContents, &pstContents->stSatOptList);
		pptextList = pstContents->szSatItemTextList;
		if (pptextList == NULL)
		{
			NX_APP_Info("Error!!! Please Check added Option item !!\n");
			return;
		}
	}
	else
	{
		pptextList = pstContents->szSatItemTextList;
	}

	/* Find Current group index for List Index */
	ulIndex	= 0;
	eCurGroup = pstContents->eChangedGroup;

	for (i = 0; i<pstContents->stSatOptList.ulOptItemNum; i++)
	{
		if (pstContents->stSatOptList.aeOptItemList[i] == (HUINT32)eCurGroup)
		{
			ulIndex = i;
			break;
		}
	}

	lLineCnt = NX_PRISM_UTIL_GetTextLineCount(eFont_SystemNormal, NX_PRISM_FONT_SIZE_22, ONDK_GetString(RES_MESG_6121_ID), NX_PRISM_UI_DLG_MULTISEL_MSG_TEXT_W);

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

	NX_PRISM_COMP_ListDlg_SetListPopup(&s_stUiChList_OptContents, ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis()),
							&stRect, (NX_PRISM_UI_DLG_MULTISEL_MSG_LIST_H + NX_PRISM_UI_DLG_MULTISEL_MSG_CELL_MG),
							ulAttribute, ulTextListNum, ulMaxListNum, ulIndex, (HUINT32)pptextList);

	NX_PRISM_COMP_ListDlg_SetFontSize(&s_stUiChList_OptContents, NX_PRISM_FONT_SIZE_22);
	NX_PRISM_COMP_ListDlg_SetTitle(&s_stUiChList_OptContents, ONDK_GetString(RES_MESG_6121_ID));

	if (ulAttribute & eListDlg_OSD_USER)
	{
		NX_PRISM_COMP_ListDlg_SetExternalArea(&s_stUiChList_OptContents, &stListRect);
	}

	/* Create Pop Up */
	ONDK_GWM_APP_Create (ONDK_UPPER_WINDOW, "nx_guide_DisplayListPopup", APP_DEFAULT_POPUP_PRIORITY, nx_chlist_DisplayListPopup, 0, 0, 0, 0);

	//ONDK_GWM_KillTimer(NX_PRISM_CHLIST_EXIT_TIMER_ID);
}

HUINT8		**nx_chlist_OptionMakeSatelliteGroupItemTextList(NxUiChannelListContents_t *pstContents, NX_Util_OPTIONList_t *pstOptItemList)
{
	HUINT32 		ulIndex = 0, ulArraySize = 0;
	HUINT32 		ulTextLen = 0;
	HUINT8			*pucDstText = NULL;
	HUINT8			**ppucTextArray = NULL;


	ulArraySize = pstOptItemList->ulOptItemNum;
	if (ulArraySize == 0)
	{
		NX_APP_Error(" Error!!! ulItemList list ulItemNum is Zero!!\n");
		return	NULL;
	}

	ppucTextArray = (HUINT8 **)NX_APP_Malloc(sizeof(HUINT8 *) * ulArraySize);
	if (ppucTextArray == NULL)
	{
		return	NULL;
	}

	HxSTD_memset(ppucTextArray, 0, sizeof(HUINT8 *) * ulArraySize);
	for (ulIndex = 0; ulIndex < ulArraySize; ulIndex++)
	{
		// Destination Text
		pucDstText = (HUINT8 *)NX_APP_Malloc (ulTextLen + 1);
		if (pucDstText == NULL)
		{
			for (ulIndex = 0; ulIndex < ulArraySize; ulIndex++)
			{
				if (ppucTextArray[ulIndex] != NULL)
				{
					NX_APP_Free (ppucTextArray[ulIndex]);
				}
			}

			NX_APP_Free (ppucTextArray);
			return	NULL;
		}

		HxSTD_memset(pucDstText, 0x0, ulTextLen + 1);
		pucDstText[0] = '\0';

		HxSTD_StrCpy ((char*)pucDstText, (char*)pstContents->astAntInfo[ulIndex].satName);
		ppucTextArray[ulIndex] = pucDstText;
	}

	return	ppucTextArray;
}

static ONDK_Result_t	nx_chlist_MsgGwmCreate(NxUiChannelListContents_t *pstContents, Handle_t hAct, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32 ulCurIndexChannelGroup = 0;

	HxSTD_MemSet(pstContents, 0, sizeof(NxUiChannelListContents_t));

	pstContents->nCurItemIndex = p1;
	pstContents->nChannelNum = p2;
	pstContents->pstChannels = NX_PRISM_LIVE_GetLiveChannels();
	pstContents->bChangedGroup = FALSE;
	pstContents->eChangedSort = eNxUtil_OptItem_SortNumber;
	pstContents->ePinCodeActiontItem = eNxChlist_OptItem_Max;

	nx_chlist_SetPfSearchCurChannel(pstContents);

	if (pstContents->pstChannels == NULL)
	{
		HxLOG_Error("channel data is NULL!!!\n");
		return	ONDK_RET_FAIL;
	}

	pstContents->eFocusType = eCHLIST_CHANNELLIST;

	//Init the handle for TV and Radio
	pstContents->hTvImg = ONDK_IMG_CreateSelf(RES_212_00_THUMB_TV_PNG);
	pstContents->hRadioImg = ONDK_IMG_CreateSelf(RES_212_00_THUMB_RADIO_PNG);

	//Init Alphabet Opt
	nx_chlist_OptionLoadAlphabetGroupInitDataDefault(pstContents);

	//Load Satellite info
	NX_ANTENNA_LoadAntennaSetting();
	nx_chlist_LoadAntennaInfos(pstContents);

	//Load Channel List
	nx_chlist_OptionActionChangeGroup(pstContents, s_eLastChangedGroup);

	nx_chlist_SetUiObject(pstContents, hAct, p1, p2, p3);

	//Draw Channel Group
	nx_chlist_InitChannelGroupList(pstContents);
	nx_chlist_CreateChannelGroupUI(pstContents);

	//set current index for channel group
	ulCurIndexChannelGroup = nx_chlist_GetFocusIndexOfChannelGroup(pstContents);
	ONDK_GWM_List_SetCurrentIndex(NX_PRISM_CHLIST_CHANNELGROUP_ID, ulCurIndexChannelGroup);

	ONDK_GWM_Obj_SetHide(NX_PRISM_CHLIST_CHANNELGROUP_ID, TRUE);

	nx_chlist_ResetProgrammeTimer(pstContents);
	nx_chlist_ResetExitChListTimer(pstContents);

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_chlist_MsgGwmDestroy(NxUiChannelListContents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_GWM_KillTimer(NX_PRISM_CHLIST_PROGRAMME_TIMER_ID);
	ONDK_GWM_KillTimer(NX_PRISM_CHLIST_EXPIRE_TIMER_ID);

	nx_chlist_DestroyPinCodeDialog();

	ONDK_GWM_Obj_Destroy(NX_PRISM_CHLIST_ID);

	if (pstContents->pstChListData)
	{
		NX_APP_Free(pstContents->pstChListData);
		pstContents->pstChListData = NULL;
	}

	if (pstContents->bChangedGroup && pstContents->pstChannels != NULL)
	{
		// bChangedGroup 가 true 이면, pstContents->pstChannels 는 재할당 받은것이다..
		NX_APP_Free(pstContents->pstChannels);
		pstContents->pstChannels = NULL;
	}

	ONDK_GWM_APP_Destroy(nx_chlist_DisplayListPopup);

	if (pstContents->szItemTextList)
	{
		NX_PRISM_UTIL_OptionDeleteItemTextList(&pstContents->stAlphabetOptList, pstContents->szItemTextList);
		pstContents->szItemTextList = NULL;
	}

	HxSTD_MemSet(&pstContents->stAlphabetOptList, 0x00, sizeof(NX_Util_OPTIONList_t));

	if (pstContents->szSatItemTextList)
	{
		NX_PRISM_UTIL_OptionDeleteItemTextList(&pstContents->stSatOptList, pstContents->szSatItemTextList);
		pstContents->szSatItemTextList= NULL;
	}

	HxSTD_MemSet(&pstContents->stSatOptList, 0x00, sizeof(NX_Util_OPTIONList_t));

	ONDK_IMG_Destroy(pstContents->hTvImg);
	ONDK_IMG_Destroy(pstContents->hRadioImg);

	s_eLastChangedGroup = pstContents->eChangedGroup;

	NX_COMMON_NOT_USED_ARGUMENT(NULL, p1, p2, p3);

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_chlist_MsgGwmKeyDown(NxUiChannelListContents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t	ret = ONDK_RET_FAIL;
	HINT32			lCurItemIndex = 0, lChannelNum = 0;
	NX_Channel_t	*stSelectChannel = NULL;
	NxChList_ChGroupItem_t *pstChGroupItem = NULL;

	nx_chlist_ResetExitChListTimer(pstContents);
	NX_PRISM_UTIL_KeyConv_UiCommon(&p1);
	switch (p1)
	{
		case KEY_GUIDE:
			ONDK_GWM_APP_Destroy(0);
			return ONDK_RET_OK;

		case KEY_ARROWLEFT:
			if(pstContents->eFocusType == eCHLIST_CHANNELLIST)
			{
				pstContents->eFocusType = eCHLIST_CHANNELGROUP;
				ONDK_GWM_Obj_SetHide(NX_PRISM_CHLIST_CHANNELGROUP_ID, FALSE);
				ONDK_GWM_Obj_SetHide(NX_PRISM_CHLIST_OPTION_TEXT_ID, TRUE);
				ONDK_GWM_Obj_SetHide(NX_PRISM_CHLIST_LIST_LINE_ID, TRUE);
				ONDK_GWM_APP_InvalidateObject(NX_PRISM_CHLIST_ID);
				nx_chlist_AddDimImage(pstContents);
			}
			break;

		case KEY_ARROWRIGHT:
			if(pstContents->eFocusType == eCHLIST_CHANNELGROUP)
			{
				pstContents->eFocusType = eCHLIST_CHANNELLIST;
				ONDK_GWM_Obj_SetHide(NX_PRISM_CHLIST_CHANNELGROUP_ID, TRUE);
				ONDK_GWM_Obj_SetHide(NX_PRISM_CHLIST_OPTION_TEXT_ID, FALSE);
				ONDK_GWM_Obj_SetHide(NX_PRISM_CHLIST_LIST_LINE_ID, FALSE);
				ONDK_GWM_APP_InvalidateObject(NX_PRISM_CHLIST_ID);
				nx_chlist_DeleteDimImage(pstContents);
			}
			else
			{
				lCurItemIndex = pstContents->nCurItemIndex;
				lChannelNum = pstContents->nChannelNum;
				ONDK_GWM_APP_Destroy(0);
				ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW,"NX_PRISM_INFOHUB_Proc", APP_BACKGROUND_OSD_PRIORITY, NX_PRISM_INFOHUB_Proc,
										HANDLE_NULL, lCurItemIndex, lChannelNum, 0);
			}
			break;

		case KEY_ARROWUP:
			if(pstContents->eFocusType == eCHLIST_CHANNELGROUP)
			{
				lCurItemIndex = ONDK_GWM_List_GetCurrentIndex(NX_PRISM_CHLIST_CHANNELGROUP_ID);
				lCurItemIndex -= 1;
				if(lCurItemIndex < 0)
				{
					lCurItemIndex = HLIB_VECTOR_Length(pstContents->pstChannelGroupList) - 1;
				}
				ONDK_GWM_List_SetCurrentIndex(NX_PRISM_CHLIST_CHANNELGROUP_ID, lCurItemIndex);
				return ONDK_MESSAGE_BREAK;
			}
			else
			{
				ret = nx_chlist_ActionChannelList(pstContents, p1);
				break;
			}

		case KEY_ARROWDOWN:
			if(pstContents->eFocusType == eCHLIST_CHANNELGROUP)
			{
				lCurItemIndex = ONDK_GWM_List_GetCurrentIndex(NX_PRISM_CHLIST_CHANNELGROUP_ID);
				lCurItemIndex += 1;
				if(lCurItemIndex >= HLIB_VECTOR_Length(pstContents->pstChannelGroupList))
				{
					lCurItemIndex = 0;
				}
				ONDK_GWM_List_SetCurrentIndex(NX_PRISM_CHLIST_CHANNELGROUP_ID, lCurItemIndex);
				return ONDK_MESSAGE_BREAK;
			}
			else
			{
				ret = nx_chlist_ActionChannelList(pstContents, p1);
				break;
			}

		case KEY_OK:
			if(pstContents->eFocusType == eCHLIST_CHANNELGROUP)
			{
				lCurItemIndex = ONDK_GWM_List_GetCurrentIndex(NX_PRISM_CHLIST_CHANNELGROUP_ID);
				pstChGroupItem = HLIB_VECTOR_ItemAt(pstContents->pstChannelGroupList, lCurItemIndex);
				if (pstChGroupItem == NULL)
				{
					NX_APP_Error("Error!!! (pstChGroupItem == NULL) \n");
					return ONDK_MESSAGE_BREAK;
				}

				pstContents->eOptGroup = pstChGroupItem->eChannelGroupIndex;

				if(pstChGroupItem != NULL && pstChGroupItem->eChannelGroupIndex != eNxChListGroup_ALPHABET
							&& pstChGroupItem->eChannelGroupIndex != eNxChListGroup_Satellite)
				{
					nx_chlist_OptionActionChangeGroup(pstContents, pstChGroupItem->eChannelGroupIndex);
					//pstContents->eFocusType = eCHLIST_CHANNELLIST;
				//	ONDK_GWM_Obj_SetHide(NX_PRISM_CHLIST_CHANNELGROUP_ID, TRUE);
					ONDK_GWM_APP_InvalidateObject(NX_PRISM_CHLIST_ID);
					//nx_chlist_DeleteDimImage(pstContents);
					return ONDK_MESSAGE_BREAK;
				}
				else if(pstChGroupItem != NULL && pstChGroupItem->eChannelGroupIndex == eNxChListGroup_ALPHABET)
				{
					nx_chlist_CreateOptionChangeAlphabetGroupListPopup(pstContents);
					return ONDK_MESSAGE_BREAK;
				}
				else if(pstChGroupItem != NULL && pstChGroupItem->eChannelGroupIndex == eNxChListGroup_Satellite)
				{
					if(pstContents->ulNumAnt > 1)
					{
						nx_chlist_CreateOptionSatelliteGroupListPopup(pstContents);
					}
					else if(pstContents->ulNumAnt == 1)
					{
						nx_chlist_SatelliteOptionChange(pstContents, 0);
					}
					return ONDK_MESSAGE_BREAK;
				}
			}
			else
			{
				ret = nx_chlist_ActionChannelList(pstContents, p1);
			}
			break;
		case KEY_CH_PLUS:
		case KEY_CH_MINUS:
			if(pstContents->eFocusType == eCHLIST_CHANNELGROUP)
			{
				return ONDK_MESSAGE_BREAK;
			}
			else
			{
				ret = nx_chlist_ActionChannelList(pstContents, p1);
			}
			break;
		case KEY_EXIT:
		case KEY_BACK:
			ret = nx_chlist_ActionChannelList(pstContents, p1);
			break;

		case KEY_OPT:
			if(pstContents->eFocusType == eCHLIST_CHANNELLIST)
			{
				ONDK_GWM_KillTimer(NX_PRISM_CHLIST_EXPIRE_TIMER_ID);
				if (pstContents->pstChannels)
				{
					if (pstContents->pstChannels == NULL)
					{
						NX_APP_Error("Error!!! pstContents->pstChannels is NULL !!\n");
						return ONDK_RET_FAIL;

					}
					stSelectChannel = &pstContents->pstChannels[pstContents->nCurItemIndex];
					if (stSelectChannel)
					{
						ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, "NX_PRISM_CHANNELLIST_OPT_Proc", APP_DEFAULT_PRIORITY,
									NX_PRISM_CHANNELLIST_OPT_Proc, 0, eNX_UCT_Chlist, stSelectChannel->svcUid, pstContents->eChangedGroup);
					}
					else
					{
						NX_APP_Error("Error!!! Current Channel list Data(%u) was NULL!!!\n", pstContents->nCurItemIndex);
					}
				}
				else
				{
					NX_APP_Error("Error!!! pstContents->pstChannels was NULL!!!\n");
				}
			}
			return	ONDK_MESSAGE_BREAK;

		case KEY_STANDBY:
			ONDK_GWM_APP_Destroy(0);
			return ONDK_MESSAGE_PASS;

		case KEY_SUBTITLE:
		case KEY_AUDIO:
		case KEY_TEXT:
			NX_PRISM_NotAvailable_Show();
			return	ONDK_MESSAGE_BREAK;

		default:
			return ONDK_MESSAGE_PASS;
	}

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	return ret;
}

static ONDK_Result_t	nx_chlist_MsgGwmTimer(NxUiChannelListContents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (p1 == NX_PRISM_CHLIST_PROGRAMME_TIMER_ID)
	{
		// set programme info
		ONDK_GWM_KillTimer(NX_PRISM_CHLIST_PROGRAMME_TIMER_ID);
		nx_chlist_SetChannelListData(pstContents, TRUE);
	}
	else if (p1 == NX_PRISM_CHLIST_EXPIRE_TIMER_ID)
	{
		ONDK_GWM_APP_Destroy(0);
	}

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	return GWM_RET_OK;
}

static ONDK_Result_t	nx_chlist_MsgPopUpOptValueChange(NxUiChannelListContents_t *pstContents, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t			eResult = ONDK_RET_FAIL;
	eNxChlist_OptItem_t 	eCurOptItem = (eNxChlist_OptItem_t)p2;

	switch (eCurOptItem)
	{
		case eNxChlist_OptItem_Group:
			eResult = nx_chlist_OptionActionChangeGroup(pstContents, p3);
			break;
		case eNxChlist_OptItem_Sort:
			eResult = nx_chlist_OptionActionSort(pstContents, p3);
			break;
		case eNxChlist_OptItem_AddFav:
			eResult = nx_chlist_OptionActionAddFavourite(pstContents, p3);
			break;
		case eNxChlist_OptItem_Delete:
			pstContents->ePinCodeActiontItem = eNxChlist_OptItem_Delete;
			nx_chlist_CreatePinCodeDialog();
			break;
		case eNxChlist_OptItem_Lock:
			eResult = nx_chlist_OptionActionLockUnLock(pstContents, p3);
			break;
		case eNxChlist_OptItem_Rename:
			eResult = nx_chlist_OptionActionRename(pstContents, p3);
			break;
		default:
			NX_APP_Info("Not Support Option Item [%d] \n", eCurOptItem);
			break;
	}

	return ONDK_RET_OK;
}

static ONDK_Result_t	nx_chlist_AlphabetOptionChange(NxUiChannelListContents_t *pstContents, HUINT32 ulCurIdx)
{
	NX_ChListGroup_t 		eChGroupType = eNxChListGroup_ALL;

	if (pstContents->stAlphabetOptList.ulOptItemNum == 0)
	{
		NX_APP_Info("Error!!! Please Check Option item was Zero!!\n");
		return	ONDK_MESSAGE_BREAK;
	}

	eChGroupType = (NX_ChListGroup_t)pstContents->stAlphabetOptList.aeOptItemList[ulCurIdx];
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
			nx_chlist_OptionActionChangeGroup(pstContents, eChGroupType);
			//pstContents->eFocusType = eCHLIST_CHANNELLIST;
		//	ONDK_GWM_Obj_SetHide(NX_PRISM_CHLIST_CHANNELGROUP_ID, TRUE);
			ONDK_GWM_APP_InvalidateObject(NX_PRISM_CHLIST_ID);
			//nx_chlist_DeleteDimImage(pstContents);
			break;
		default:
			NX_APP_Info("Error!!! Not Support Sort Item [%d] !!\n", eChGroupType);
			break;
	}

	NX_COMMON_NOT_USED_PARAM(pstContents);
	return ONDK_RET_OK;
}

static ONDK_Result_t	nx_chlist_SatelliteOptionChange(NxUiChannelListContents_t *pstContents, HUINT32 ulCurIdx)
{
	NX_ChListGroup_t 		eChGroupType = eNxChListGroup_ALL;

	if (pstContents->stSatOptList.ulOptItemNum == 0)
	{
		NX_APP_Info("Error!!! Please Check Option item was Zero!!\n");
		return	ONDK_MESSAGE_BREAK;
	}

	eChGroupType = (NX_ChListGroup_t)pstContents->stSatOptList.aeOptItemList[ulCurIdx];
	HxLOG_Print("SAT Id(%d)\n\n", eChGroupType);
	nx_chlist_OptionActionChangeGroup(pstContents, eChGroupType);

	ONDK_GWM_APP_InvalidateObject(NX_PRISM_CHLIST_ID);


	NX_COMMON_NOT_USED_PARAM(pstContents);
	return ONDK_RET_OK;
}

/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/
ONDK_Result_t	NX_PRISM_CHANNELLIST_Proc(HINT32 lMessage, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t				lRet = ONDK_RET_OK;
	NX_UI_CallerType_t 			eCurOptType = eNX_UCT_None;
	NX_ChListGroup_t			eCurChannelGroup = eNxChListGroup_TV;	// Group Item
	NxUiChannelListContents_t	*pstContents = nx_chlist_GetContents();

	ONDK_Print(HxANSI_COLOR_YELLOW("[%s:%d] Message(%d) : %s, handle: %p p1(0x%x) p2(0x%x) p3(0x%x) \n"), __FUNCTION__, __LINE__, lMessage, ONDK_GWM_GwmMsg2String(lMessage), p1, p2, p3);

	switch(lMessage)
	{
		case MSG_GWM_CREATE:
			HxLOG_Debug("\n[%s.%d] MSG_GWM_CREATE IN NX_UI_LIVE_Proc\n\n",__FUNCTION__,__LINE__);
			lRet = nx_chlist_MsgGwmCreate(pstContents, hHandle, p1, p2, p3);
			if (lRet != ONDK_RET_OK)
			{
				//ONDK_GWM_APP_Destroy(0);
			}
			break;

		case MSG_GWM_TIMER:
			NX_APP_Message("MSG_GWM_TIMER\n");
			nx_chlist_MsgGwmTimer(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_APK_MESSAGE:
			break;

		case MSG_GWM_HAPI_MESSAGE:
			lRet = ONDK_MESSAGE_PASS;
			break;

		case MSG_GWM_CLICKED :
			break;

		case MSG_GWM_KEYDOWN:
			lRet = nx_chlist_MsgGwmKeyDown(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_KEYUP:
			break;

		case MSG_GWM_DESTROY:
			lRet = nx_chlist_MsgGwmDestroy(pstContents, p1, p2, p3);
			break;

		case MSG_APP_POPUP_OPT_VALUE_CHANGED:
			NX_APP_Message("MSG_APP_POPUP_OPT_VALUE_CHANGED\n");
			eCurOptType = (NX_UI_CallerType_t)p1;
			if (eCurOptType == eNX_UCT_Chlist)
			{
				nx_chlist_ResetExitChListTimer(pstContents);
				lRet = nx_chlist_MsgPopUpOptValueChange(pstContents, p2, p3);
			}
			break;

		case MSG_CORE_SERVICE_PF_CHANGED_PROGRAMMES:
			// pf update 가 온경우, 갱신...
			NX_APP_Message("received MSG_CORE_SERVICE_PF_CHANGED_PROGRAMMES MSG.\n");
			break;

		case MSG_CORE_SERVICE_EPG_CHANGED_PROGRAMMES:
			NX_APP_Message("received MSG_CORE_SERVICE_EPG_CHANGED_PROGRAMMES MSG.\n");
			break;

		case MSG_APP_SYSTEM_UPDATE_SERVICE:
			NX_APP_Message("MSG_APP_SYSTEM_UPDATE_SERVICE\n");
			/* Reload Channel List */
			nx_chlist_ReloadChannelList(pstContents);
			break;

		case MSG_APP_SYSTEM_RELOAD_SERVICELIST:
			NX_APP_Message("MSG_APP_SYSTEM_UPDATE_SERVICE\n");

			eCurChannelGroup = NX_PRISM_LIVE_GetCurrentChGroupType();
			if (eCurChannelGroup != pstContents->eChangedGroup)
			{
				pstContents->bChangedGroup = TRUE;
				pstContents->eChangedGroup = eCurChannelGroup;
				NX_APP_Info("Change Group List Live Group:(0x%x), list Group:(0x%x) \n", eCurChannelGroup, pstContents->eChangedGroup);
			}

			/* Reload Channel List */
			nx_chlist_ReloadChannelList(pstContents);
			break;

		case MSG_APP_SYSTEM_PIN_OK:
			NX_APP_Message("MSG_APP_SYSTEM_PIN_OK\n");
			lRet = ONDK_RET_OK;
			if (pstContents->ePinCodeActiontItem == eNxChlist_OptItem_Delete)
			{
				pstContents->ePinCodeActiontItem = eNxChlist_OptItem_Max;
				nx_chlist_DestroyPinCodeDialog();
				nx_chlist_OptionActionDelete(pstContents, p3);
				lRet = ONDK_MESSAGE_BREAK;
			}
			else if (pstContents->ePinCodeActiontItem == eNxChlist_OptItem_Lock)
			{
				pstContents->ePinCodeActiontItem = eNxChlist_OptItem_Max;
				nx_chlist_DestroyPinCodeDialog();
				nx_chlist_OptionActionUnLock(pstContents);
				lRet = ONDK_MESSAGE_BREAK;
			}
			else
			{
				NX_APP_Info("UnKnow PIN Process ePinCodeActiontItem(%d)\n", pstContents->ePinCodeActiontItem);
			}
			break;

		case MSG_APP_SYSTEM_PIN_DENIED:
			NX_APP_Message("MSG_APP_SYSTEM_PIN_DENIED\n");
			nx_chlist_DestroyPinCodeDialog();
			pstContents->ePinCodeActiontItem = eNxChlist_OptItem_Max;
			lRet = ONDK_MESSAGE_BREAK;
			break;

		case MSG_APP_KEYBOARD_RESULT:
			NX_APP_Message("MSG_APP_KEYBOARD_RESULT\n");
			nx_chlist_ResetExitChListTimer(pstContents);
			nx_chlist_OptionActionSaveRename(pstContents);
			lRet = ONDK_MESSAGE_BREAK;
			break;

		case MSG_APP_HOMEMENU_UI_LAUNCH:
			NX_APP_Message("MSG_APP_HOMEMENU_UI_LAUNCH\n");
			if (p1 != eNX_UCT_Live)
			{
				ONDK_GWM_APP_Destroy(0);
			}
			break;

		case MSG_APP_SETTING_VALUE_CHANGED:
			NX_APP_Message("MSG_APP_POPUP_LIST_DESTROYED & MSG_APP_SETTING_VALUE_CHANGED\n");
			if (lMessage == MSG_APP_SETTING_VALUE_CHANGED)
			{
				eCurOptType =  (NX_UI_CallerType_t)p1;

				if(pstContents->eOptGroup == eNxChListGroup_ALPHABET)
				{
					nx_chlist_AlphabetOptionChange(pstContents, eCurOptType);
				}
				else if(pstContents->eOptGroup == eNxChListGroup_Satellite)
				{
					nx_chlist_SatelliteOptionChange(pstContents, eCurOptType);
				}
			}
			break;

	}

	if(lRet != ONDK_RET_OK)
	{
		return lRet;
	}

	return ONDK_GWM_ProcessMessageDefault(lMessage, hHandle, p1, p2, p3);
}

HUINT32		NX_PRISM_CHANNELLIST_GetCurrentSort(void)
{
	ONDK_GWM_App_t	pGwmApp = NULL;
	NxUiChannelListContents_t	*pstContents = nx_chlist_GetContents();

	pGwmApp = ONDK_GWM_APP_Get((ONDK_GWM_Callback_t)NX_PRISM_CHANNELLIST_Proc);
	if(pGwmApp == NULL)
	{
		NX_APP_Error("Error!!! NX_PRISM_CHANNELLIST_Proc() was destoryed!!\n");
		return	(HUINT32)eNxUtil_OptItem_None;
	}

	if (pstContents)
	{
		return	(HUINT32)pstContents->eChangedSort;
	}
	else
	{
		return	(HUINT32)eNxUtil_OptItem_None;
	}
}


/******************************************************************************/
#define _________End_Of_File____________________________________________________
/******************************************************************************/


