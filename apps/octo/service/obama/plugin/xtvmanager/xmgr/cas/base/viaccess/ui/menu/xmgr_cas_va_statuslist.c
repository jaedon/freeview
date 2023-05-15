
/**
	@file     ap_cas_va_statuslist.c
	@brief    file_name & simple comment.

	Description: for VIACCESS MENU Status List UI \n
	Module: Application(Layer), Menu(module), CAS_VA(module) \n
	Remarks : \n
	 Date			Modification				Name\n
	-----------	-------------------------------------	------------------\n
	 2009/07/24		modified					jhlee\n

	Copyright (c) 2009 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/


/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <db_svc.h>
#include <db_param.h>
#include <svc_sys.h>
#include <svc_cas.h>
#include <isosvc.h>
#include <osd_gfx.h>
#include <osd_font.h>

#include <xmgr_cas_util_dslib.h>
//#include <app_manager_cas.h> // TODO: hjlee3, just for build
#include <mgr_pg.h>
#include <mgr_cas.h>

#include "../../local_include/_xmgr_cas_va_util.h"
#include "../../local_include/_xmgr_cas_va_ui.h"
#include "../../local_include/_xmgr_cas_va_sc.h"
#include "_xmgr_cas_va_menu_coordinate.h"
#include "va_ui_adapt.h"
#include "mgr_action.h"
#include <xmgr_cas_res_str.h>


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

#if defined(CONFIG_DEBUG)
HUINT32	g_CasVaStatusList_Level = DBG_ASSERT;
HUINT32	g_CasVaStatusList_DbgColor = 0;
#endif




/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

#if defined(CONFIG_DEBUG)
#define	ApCasVaStatusListPrint(level, message)	(((level) & g_CasVaStatusList_Level) ? (VK_Print("[01;%dm", g_CasVaStatusList_DbgColor), VK_Print("==[App:Viaccess Status]=="), VK_Print message, VK_Print("[00m")) : 0)
#define	ApCasVaStatusListAssert(c) 				((c) ? 0 : (VK_Print("\n ==[Viaccess Status]== List assertion on %s: line%d\n\n", __FILE__, __LINE__)))
#define	ApCasVaStatusListData(level, message)	(((level) & g_CasVaStatusList_Level) ? ( VK_Print message) : 0)
#else
#define	ApCasVaStatusListPrint(level, message)
#define	ApCasVaStatusListAssert(c)
#define	ApCasVaStatusListData(level, message)
#endif


#define	CAS_VA_STATUSLIST_DEF_COLOR					0x00000000
#define	CAS_VA_STATUSLIST_TRANSPARENCY				100
#define	CAS_VA_STATUSLIST_ITEM_MAX					10
#define	CAS_VA_MAX_ITEM_STRLEN						64

#define	D_TEXT_LINE_LENGTH							54


/*******************************************************************/
/********************      Struture        *************************/
/*******************************************************************/

typedef struct
{							/* If a date is before 1/1/1980, viaccess  */
    HUINT16    uiYear;		/* transforms it in 1/1/1980.              */
    HUINT8     uiMonth;		/* If after 12/31/2107,                    */
    HUINT8     uiDay;		/* it transforms it in 12/31/2107...       */
} VA_UI_Date_t;

typedef struct
{
    VA_UI_Date_t	stMin;   /* minimum is 1/1/1980  */
    VA_UI_Date_t	stMax;   /* maximum is 12/31/2107 */
} VA_UI_DateRange_t;

typedef	enum
{
	eRightsOrigin,
	eRightsClassList,
	eRightsThemeList,
	eRightsPrebookedList,
} VA_UI_RightsType_t;

typedef struct tag_StatusList_content
{
	HUINT32			ulTotalNum;
	HUINT8				szTitle[MGR_CAS_STR_LENGTH_LONG];
	HUINT8				szColumnTitle[MGR_CAS_STR_LENGTH_LONG];
	HINT32				lCasVaCAItemID;
	Custom_list_t		stTextListBox;
	BUS_Callback_t		pfnCallerCallback;
} StatusList_Content_t;

#if 0 // not use for octo2.0
typedef struct tag_StatusList_content
{
	/* for drawing application window */
	HUINT32					ulCurFocus;
	HUINT32					ulNumOfSubItem, ulMaxNumOfSubItem;
	SettingsLeafSubItem_t	*subItems;

	/* running info for app */
	HUINT8					szTitle[MAX_ELEMENT_TEXT_LENGTH];
	HINT32					lCasVaCAItemID;
	VA_UI_RightsType_t		eRightsType;

	/* List Item */
	HBOOL					bDrawCursor;
	Custom_list_t			stTextListBox;
	GWM_ObjDraw_t			objDrawFunc;

	/* Op Message TextBox */
	HUINT8					*pszOpMessage;

	/* Dialog*/
	AP_DialogInstance_t		stDialogInstance;

#if defined(CONFIG_MW_SYS_7SEG) && defined(CONFIG_OP_NTVPLUS)
	HBOOL					bDraw7Seg;
#endif
} StatusList_Content_t;
#endif

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

STATIC StatusList_Content_t			s_CasVaStatusListContents;

STATIC HINT32						s_uiSelectOpListIdx = -1;
STATIC VA_MENU_OP_LIST				s_stOpList;
STATIC VA_MENU_CLASS_CONSULTATION	s_stOpClass;
STATIC VA_MENU_THEME_CONSULTATION	s_stOpTheme;
STATIC VA_MENU_PREBOOK_CONSULTATION	s_stOpPrebook;
STATIC VA_MENU_OP_MESSAGE			*s_pstOpMsg;
STATIC VA_MENU_STB_INFO				s_stStbInfo;
STATIC VA_MENU_SMARTCARD_INFO		s_stScInfo;
#ifdef CONFIG_MW_CAS_VIACCESS_PVR
STATIC VA_MENU_PLAYBACK_INFO		s_stPbInfo;
#endif
STATIC VA_UI_RightsType_t			s_eCurrentRightsType;

/*******************************************************************/
/********************      	Static Prototype	********************/
/*******************************************************************/

STATIC BUS_Result_t		local_va_statuslist_DestroyUI(StatusList_Content_t *pstContents);
STATIC BUS_Result_t		local_va_statuslist_MsgGwmListSelected(StatusList_Content_t *pstContents, HINT32 lIndex, HBOOL bRight);
#if 0 // not use for octo2.0
STATIC HERROR			local_va_statuslist_DrawDefaultUI(StatusList_Content_t *pstContents);
STATIC HERROR			local_va_statuslist_DrawMenuTitle(StatusList_Content_t *pstContents);
STATIC HERROR			local_va_statuslist_DrawListItem_Title(StatusList_Content_t *pstContents);
STATIC BUS_Result_t		local_va_statuslist_DrawListItem(OSD_Screen_t *screen, OSD_Area_t *area, OSD_Rect_t *rect, GWM_GuiObject_t object, void *arg, int param1);
STATIC BUS_Result_t		local_va_statuslist_DrawList_RightsTitle(OSD_Screen_t *screen, OSD_Area_t *area, OSD_Rect_t *rect, GWM_GuiObject_t object, void *arg, int param1);
STATIC HERROR			local_va_statuslist_DrawListItem_RightsSum(OSD_Screen_t * pixmap, OSD_Font_t * font, HINT32 lx, HINT32 ly, HINT32 lxOffset, HUINT8 * szString, OSD_Color_t color);
STATIC HERROR			local_va_statuslist_DrawListItem_StbInfos(OSD_Screen_t *pixmap, OSD_Font_t *font, HINT32 lx, HINT32 ly, HINT32 lxOffset, HUINT8 *szString, OSD_Color_t color);
STATIC HERROR			local_va_statuslist_DrawListItem_Common(OSD_Screen_t *pixmap, OSD_Font_t *font, HINT32 lx, HINT32 ly, HINT32 lxOffset, HUINT8 *szString, OSD_Color_t color);
#endif
STATIC BUS_Result_t		local_va_statuslist_InitStatusList(StatusList_Content_t *pstContents, HINT32 lItemObjID);
STATIC HERROR			local_va_statuslist_InitListTitle(StatusList_Content_t *pstContents);
STATIC void				local_va_statuslist_InitListItems(StatusList_Content_t *pstContents);
STATIC HERROR			local_va_statuslist_LoadItemData(StatusList_Content_t *pstContents);
STATIC HERROR			local_va_statuslist_LoadItem_StbInfoData(StatusList_Content_t *pstContents);
STATIC HERROR			local_va_statuslist_LoadItem_RightsData(StatusList_Content_t *pstContents, HINT32 lIndex);
STATIC HERROR			local_va_statuslist_LoadItem_SmartCardInfoData(StatusList_Content_t *pstContents);
STATIC HERROR			local_va_statuslist_LoadItem_OperatorMsgData(StatusList_Content_t *pstContents);
STATIC HERROR			local_va_statuslist_ResetLists(StatusList_Content_t *pstContents);
STATIC HERROR			local_va_statuslist_FreeItemData(StatusList_Content_t *pstContents);
#ifdef CONFIG_MW_CAS_VIACCESS_PVR
STATIC HERROR			local_va_statuslist_DrawListItem_PvrPlaybackInfo (OSD_Screen_t *pixmap, OSD_Font_t *font, HINT32 lx, HINT32 ly, HINT32 lxOffset, HUINT8 *szString, OSD_Color_t color);
STATIC HERROR			local_va_statuslist_GetPlaybackSvcName(HUINT8 *pSvcName);
STATIC HERROR			local_va_statuslist_LoadItem_PvrPlaybackInfoData(StatusList_Content_t *pstContents);
#endif

extern void			AP_CAS_VA_GetClassMarkStr(VA_UI_CLASS_LIST *pClassList, HUINT8 *pStr);

/*******************************************************************/
/********************      	Static Function 	********************/
/*******************************************************************/
#define _________________UI__________________________________


STATIC StatusList_Content_t	*local_va_statuslist_GetLocalContents(void)
{
	return	&s_CasVaStatusListContents;
}

STATIC BUS_Result_t		local_va_statuslist_DestroyUI(StatusList_Content_t *pstContents)
{
#if 0 // not use for octo2.0
	/* free mainitem */
	if(pstContents->subItems != NULL)
	{
		OxAP_Free(pstContents->subItems);
		pstContents->subItems = NULL;
	}
#endif

	return ERR_OK;
}


#define	________________Action_Func________________________
#if 0 // hjlee3
STATIC AP_PinDialogInstance_t		s_stUnlockConsultationInstance;
#endif
STATIC BUS_Result_t	local_cas_va_UnlockConsultationProc(HINT32 message, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
#if 0 // not use for octo2.0
	s_stUnlockConsultationInstance.parentAppCallback = xmgr_cas_VaStatusListProc;
	s_stUnlockConsultationInstance.ulDlgAttributes = (eCasVa_PinCheckEnterCA_InDlg)eVA_PinCheckEnter_UnlockConsultation;

	return AP_CAS_VA_CheckPinDialog_Proc(&s_stUnlockConsultationInstance, message, handle, p1, p2, p3);
#endif
	return ERR_OK;
}

STATIC BUS_Result_t local_va_statuslist_MsgGwmListSelected(StatusList_Content_t *pstContents, HINT32 lIndex, HBOOL bRight)
{
#if 0 // not use for octo2.0
	HUINT8		*szTmp = NULL;
	HERROR		hError = ERR_FAIL;
	HUINT8		szStr[CAS_VA_MAX_ITEM_STRLEN];
	HINT32		lRef;

	if (pstContents->lCasVaCAItemID == eCasVAMenuItem_Rights)
	{
		switch (pstContents->eRightsType)
		{
			case eRightsOrigin:
				if ( bRight == TRUE )
				{
					// Init Data
					local_va_statuslist_InitListTitle(pstContents);
					local_va_statuslist_FreeItemData(pstContents);
					local_va_statuslist_ResetLists(pstContents);

					GWM_Obj_Destroy (CAS_VA_STATUSLIST_LIST_ID);
					GWM_Obj_Destroy (CAS_VA_STATUSLIST_TEXTLIST_TITLE01_ID);
					GWM_Obj_Destroy (CAS_VA_STATUSLIST_TEXTLIST_TITLE02_ID);
					GWM_Obj_Destroy (CAS_VA_STATUSLIST_TEXTLIST_TITLE03_ID);

					local_va_statuslist_InitListItems(pstContents);

					pstContents->lCasVaCAItemID = eCasVAMenuItem_Rights;
					pstContents->eRightsType = eRightsClassList; // show "Class List"

					local_va_statuslist_LoadItem_RightsData (pstContents, s_uiSelectOpListIdx);
					local_va_statuslist_DrawDefaultUI (pstContents);
				}
				break;

			case eRightsClassList:
				if ( bRight == TRUE )
				{
					// Init Data
					local_va_statuslist_InitListTitle(pstContents);
					local_va_statuslist_FreeItemData(pstContents);
					local_va_statuslist_ResetLists(pstContents);

					GWM_Obj_Destroy (CAS_VA_STATUSLIST_LIST_ID);
					GWM_Obj_Destroy (CAS_VA_STATUSLIST_TEXTLIST_TITLE01_ID);
					GWM_Obj_Destroy (CAS_VA_STATUSLIST_TEXTLIST_TITLE02_ID);
					GWM_Obj_Destroy (CAS_VA_STATUSLIST_TEXTLIST_TITLE03_ID);

					pstContents->lCasVaCAItemID = eCasVAMenuItem_Rights;
					pstContents->eRightsType = eRightsThemeList; // show "Theme/Level List"

					local_va_statuslist_LoadItem_RightsData (pstContents, s_uiSelectOpListIdx);
					local_va_statuslist_DrawDefaultUI (pstContents);
				}
				break;

			case eRightsThemeList:
				{
					// Init Data
					local_va_statuslist_InitListTitle(pstContents);
					local_va_statuslist_FreeItemData(pstContents);
					local_va_statuslist_ResetLists(pstContents);

					GWM_Obj_Destroy (CAS_VA_STATUSLIST_LIST_ID);
					GWM_Obj_Destroy (CAS_VA_STATUSLIST_TEXTLIST_TITLE01_ID);
					GWM_Obj_Destroy (CAS_VA_STATUSLIST_TEXTLIST_TITLE02_ID);
					GWM_Obj_Destroy (CAS_VA_STATUSLIST_TEXTLIST_TITLE03_ID);

					pstContents->lCasVaCAItemID = eCasVAMenuItem_Rights;
					pstContents->eRightsType = (bRight ? eRightsPrebookedList : eRightsClassList);

					local_va_statuslist_LoadItem_RightsData (pstContents, s_uiSelectOpListIdx);
					local_va_statuslist_DrawDefaultUI (pstContents);
				}
				break;

			case eRightsPrebookedList:
				if ( bRight == FALSE )
				{
					// Init Data
					local_va_statuslist_InitListTitle(pstContents);
					local_va_statuslist_FreeItemData(pstContents);
					local_va_statuslist_ResetLists(pstContents);

					GWM_Obj_Destroy (CAS_VA_STATUSLIST_LIST_ID);
					GWM_Obj_Destroy (CAS_VA_STATUSLIST_TEXTLIST_TITLE01_ID);
					GWM_Obj_Destroy (CAS_VA_STATUSLIST_TEXTLIST_TITLE02_ID);
					GWM_Obj_Destroy (CAS_VA_STATUSLIST_TEXTLIST_TITLE03_ID);

					pstContents->lCasVaCAItemID = eCasVAMenuItem_Rights;
					pstContents->eRightsType = eRightsThemeList; // show "Theme/Level List"

					local_va_statuslist_LoadItem_RightsData (pstContents, s_uiSelectOpListIdx);
					local_va_statuslist_DrawDefaultUI (pstContents);
				}
				break;

			default:
				break;
		}
	}
#endif
	return	ERR_BUS_NO_ERROR;
}


#define	________________Drawing_Func________________________
#if 0 // not use for octo2.0
STATIC HERROR	local_va_statuslist_DrawDefaultUI(StatusList_Content_t *pstContents)
{
	HINT32		lNumOfList = 0;
	OSD_Rect_t		rect = {0, };
	int		lListBoxHeight = 0;
	int		lStatusListItemMax = 0;
	int		lStatusListBoxY = 0;

	/* set, draw background */
	Settings_DrawBack(FALSE);

	/* Menu Title */
	local_va_statuslist_DrawMenuTitle(pstContents);

	/* List Title */
	if ( pstContents->lCasVaCAItemID == eCasVAMenuItem_Rights ) // List title
	{
		local_va_statuslist_DrawListItem_Title (pstContents);
		lListBoxHeight = (CAS_VA_STATUSLIST_LIST_BOX_HEIGHT - CAS_VA_STATUSLIST_ITEM_LINESPACE);
		lStatusListBoxY = (CAS_VA_STATUSLIST_LIST_BOX_Y + CAS_VA_STATUSLIST_ITEM_LINESPACE);
		lStatusListItemMax = CAS_VA_STATUSLIST_ITEM_MAX - 1; // 9 ∞≥
	}
	else // Non list title
	{
#if defined(CONFIG_OP_NORDIG_BOXER)
		// ui ªÛ¿∏∑Œ¥¬ max 9∞≥∞° «—∞Ë¿Œµ•, ø÷ 10∞≥∑Œ «ÿ ≥ı¿∫¡ˆ »Æ¿Œ « ø‰
		lListBoxHeight = (CAS_VA_STATUSLIST_LIST_BOX_HEIGHT - CAS_VA_STATUSLIST_ITEM_LINESPACE);
		lStatusListBoxY = (CAS_VA_STATUSLIST_LIST_BOX_Y + CAS_VA_STATUSLIST_ITEM_LINESPACE);
		lStatusListItemMax = CAS_VA_STATUSLIST_ITEM_MAX - 1; // 9 ∞≥
#else
		lListBoxHeight = CAS_VA_STATUSLIST_LIST_BOX_HEIGHT;
		lStatusListBoxY = CAS_VA_STATUSLIST_LIST_BOX_Y;
		lStatusListItemMax = CAS_VA_STATUSLIST_ITEM_MAX; // 10 ∞≥
#endif
	}

	/* List Box */
	OSD_GFX_SetRect (&rect, CAS_VA_STATUSLIST_LIST_BOX_X, lStatusListBoxY, CAS_VA_STATUSLIST_LIST_BOX_WIDTH, lListBoxHeight);
	GWM_Rect_Create (CAS_VA_STATUSLIST_LIST_BOX_ID, &rect, CAS_VA_STATUSLIST_DEF_COLOR);
	GWM_APP_SetObjectDrawMethod (CAS_VA_STATUSLIST_LIST_BOX_ID, AP_LNF_DrawBoxD);

	if ( pstContents->lCasVaCAItemID != eCasVAMenuItem_Operator_Msg )
	{
		/* List Box Item */
		lNumOfList = UTIL_LL_GetNumOfListItem(&(pstContents->stTextListBox));
		OSD_GFX_SetRect(&rect, CAS_VA_STATUSLIST_LIST_X, lStatusListBoxY, CAS_VA_STATUSLIST_LIST_WIDTH, lListBoxHeight);
		GWM_List_Create(CAS_VA_STATUSLIST_LIST_ID, &rect, "statusList", CAS_VA_STATUSLIST_LIST_ITEM_HEIGHT, lNumOfList, lStatusListItemMax, (HUINT32)&(pstContents->stTextListBox));
	}
	else
	{
		// Last Op Message √‚∑¬øÎ Text Box Item
		OSD_GFX_SetRect(&rect, CAS_VA_STATUSLIST_LIST_X + 30, CAS_VA_STATUSLIST_LIST_BOX_Y + 30, CAS_VA_STATUSLIST_LIST_WIDTH - 30, lListBoxHeight - 30);
		GWM_Text_Create(CAS_VA_STATUSLIST_LIST_ID, &rect, pstContents->pszOpMessage);
		GWM_Text_SetSize(CAS_VA_STATUSLIST_LIST_ID, FONT_SIZE_M);
		GWM_Text_ChangeColor(CAS_VA_STATUSLIST_LIST_ID, COL(C_T_Enable), 0);
	}

	GWM_APP_SetObjectDrawMethod(CAS_VA_STATUSLIST_LIST_ID, pstContents->objDrawFunc);

	/* Set Foucs */
	GWM_Obj_SetFocus(CAS_VA_STATUSLIST_LIST_ID);

	return ERR_OK;
}


STATIC HERROR	local_va_statuslist_DrawMenuTitle(StatusList_Content_t *pstContents)
{

	switch (pstContents->lCasVaCAItemID)
	{
		case eCasVAMenuItem_Rights:
			if (pstContents->eRightsType == eRightsOrigin)
			{
				MWC_UTIL_DvbStrcpy (pstContents->szTitle, GetStrRsc(LOC_CAS_VA_RIGHTS_CONSULT_ID));
				Settings_SetWindowTitle(SETTING_GET_AP_DEPTH(eSetting_CasVaStatusList), LOC_CAS_VA_RIGHTS_CONSULT_ID, pstContents->szTitle);
			}
			else if (pstContents->eRightsType == eRightsClassList
				|| pstContents->eRightsType == eRightsThemeList
				|| pstContents->eRightsType == eRightsPrebookedList)
			{
				MWC_UTIL_DvbStrcpy (pstContents->szTitle, GetStrRsc(LOC_CAS_VA_RIGHTS_SUMMARY_ID));
				Settings_SetWindowTitle(SETTING_GET_AP_DEPTH(eSetting_CasVaStatusList), LOC_CAS_VA_RIGHTS_SUMMARY_ID, pstContents->szTitle);
			}
			break;

		case eCasVAMenuItem_Operator_Msg:
			MWC_UTIL_DvbStrcpy (pstContents->szTitle, GetStrRsc(LOC_CAS_VA_OP_MSG_CONSULT_ID));
			Settings_SetWindowTitle (SETTING_GET_AP_DEPTH(eSetting_CasVaStatusList), LOC_CAS_VA_OP_MSG_CONSULT_ID, pstContents->szTitle);
			break;

		case eCasVAMenuItem_Stb_Info:
			MWC_UTIL_DvbStrcpy (pstContents->szTitle, GetStrRsc(LOC_CAS_VA_STB_INFO_ID));
			Settings_SetWindowTitle (SETTING_GET_AP_DEPTH(eSetting_CasVaStatusList), LOC_CAS_VA_STB_INFO_ID, pstContents->szTitle);
			break;

		case eCasVAMenuItem_Smartcard_Info:
			MWC_UTIL_DvbStrcpy (pstContents->szTitle, GetStrRsc(LOC_CAS_VA_SC_INFO_ID));
			Settings_SetWindowTitle (SETTING_GET_AP_DEPTH(eSetting_CasVaStatusList), LOC_CAS_VA_SC_INFO_ID, pstContents->szTitle);
			break;
#ifdef CONFIG_MW_CAS_VIACCESS_PVR
		case eCasVAMenuItem_PvrPlayback_Info:
			MWC_UTIL_DvbStrcpy (pstContents->szTitle, GetStrRsc(LOC_CAS_VA_PLAYBACK_INFO_ID));
			Settings_SetWindowTitle (SETTING_GET_AP_DEPTH(eSetting_CasVaStatusList), LOC_CAS_VA_PLAYBACK_INFO_ID, pstContents->szTitle);
			break;
#endif
		default:
			ApCasVaStatusListAssert(0);
			return ERR_FAIL;
	}

	return ERR_OK;
}


STATIC HERROR	local_va_statuslist_DrawListItem_Title(StatusList_Content_t *pstContents)
{
	HUINT8		szTitle[TEXT_SIZE128];
	OSD_Rect_t	rect = {0, }, rcTmp = {0, };

	switch (pstContents->lCasVaCAItemID)
	{
		case eCasVAMenuItem_Rights: // «Â¿Á±Ó¡ˆ¥¬eCasVAMenuItem_Rights ∏∏ «ÿ¥ÁªÁ«◊ ¿÷¿Ω
			/* Draw - List Title */
			if ( pstContents->eRightsType == eRightsOrigin )
			{
				OSD_GFX_SetRect (&rect, CAS_VA_STATUSLIST_TITLE_X, CAS_VA_STATUSLIST_TITLE_Y,
									CAS_VA_STATUSLIST_LIST_TITLE_WIDTH, CAS_VA_STATUSLIST_TITLE_HEIGHT);
				GWM_Text_Create (CAS_VA_STATUSLIST_TEXTLIST_TITLE01_ID, &rect, (HUINT8*)GetStrRsc(LOC_CAS_VA_SELECT_OP_LIST_ID));
				GWM_Text_ChangeColor (CAS_VA_STATUSLIST_TEXTLIST_TITLE01_ID, COL(C_T_Subtitle01), CAS_VA_STATUSLIST_DEF_COLOR);
				GWM_Text_SetSize (CAS_VA_STATUSLIST_TEXTLIST_TITLE01_ID, FONT_SIZE_M);
				GWM_Text_SetAlign (CAS_VA_STATUSLIST_TEXTLIST_TITLE01_ID, TEXT_ALIGN_LEFT);
			}
			else if ( pstContents->eRightsType == eRightsClassList)
			{
				// Class List Button
				OSD_GFX_SetRect(&rect, CAS_VA_STATUSLIST_LIST_BOX_X + CAS_VA_STATUSLIST_TEXT_MARGIN, CAS_VA_STATUSLIST_TITLE_Y,
					CAS_VA_STATUSLIST_LIST_BOX_TITLE_BUTTON_WIDTH, CAS_VA_STATUSLIST_TITLE_HEIGHT);
				GWM_Button_Create(CAS_VA_STATUSLIST_TEXTLIST_TITLE01_ID, &rect, (HUINT8*)GetStrRsc(LOC_CAS_VA_CLASS_LIST_ID));
				GWM_Button_SetStatus (CAS_VA_STATUSLIST_TEXTLIST_TITLE01_ID, GUIOBJ_PUSHED);
				GWM_APP_SetObjectDrawMethod (CAS_VA_STATUSLIST_TEXTLIST_TITLE01_ID, (GWM_ObjDraw_t)local_va_statuslist_DrawList_RightsTitle);

				// Theme List Button
				OSD_GFX_CopyRect(&rcTmp, &rect);
				OSD_GFX_SetRect(&rect, rcTmp.x + rcTmp.w, rcTmp.y, ((CAS_VA_STATUSLIST_LIST_BOX_WIDTH)/3), CAS_VA_STATUSLIST_TITLE_HEIGHT);
				GWM_Button_Create(CAS_VA_STATUSLIST_TEXTLIST_TITLE02_ID, &rect, (HUINT8*)GetStrRsc(LOC_CAS_VA_THEME_LEVEL_LIST_ID));
				GWM_Button_SetStatus (CAS_VA_STATUSLIST_TEXTLIST_TITLE02_ID, GUIOBJ_NORMAL);
				GWM_APP_SetObjectDrawMethod (CAS_VA_STATUSLIST_TEXTLIST_TITLE02_ID, (GWM_ObjDraw_t)local_va_statuslist_DrawList_RightsTitle);

				// Prebooked List Button
				OSD_GFX_CopyRect(&rcTmp, &rect);
				OSD_GFX_SetRect(&rect, rcTmp.x + rcTmp.w, rcTmp.y, ((CAS_VA_STATUSLIST_LIST_BOX_WIDTH)/3), CAS_VA_STATUSLIST_TITLE_HEIGHT);
				GWM_Button_Create(CAS_VA_STATUSLIST_TEXTLIST_TITLE03_ID, &rect, (HUINT8*)GetStrRsc(LOC_CAS_VA_PREBOOKED_LIST_ID));
				GWM_Button_SetStatus (CAS_VA_STATUSLIST_TEXTLIST_TITLE03_ID, GUIOBJ_NORMAL);
				GWM_APP_SetObjectDrawMethod (CAS_VA_STATUSLIST_TEXTLIST_TITLE03_ID, (GWM_ObjDraw_t)local_va_statuslist_DrawList_RightsTitle);
			}
			else if (pstContents->eRightsType == eRightsThemeList)
			{
				// Class List Button
				OSD_GFX_SetRect(&rect, CAS_VA_STATUSLIST_LIST_BOX_X + CAS_VA_STATUSLIST_TEXT_MARGIN, CAS_VA_STATUSLIST_TITLE_Y,
					CAS_VA_STATUSLIST_LIST_BOX_TITLE_BUTTON_WIDTH, CAS_VA_STATUSLIST_TITLE_HEIGHT);
				GWM_Button_Create(CAS_VA_STATUSLIST_TEXTLIST_TITLE01_ID, &rect, (HUINT8*)GetStrRsc(LOC_CAS_VA_CLASS_LIST_ID));
				GWM_Button_SetStatus (CAS_VA_STATUSLIST_TEXTLIST_TITLE01_ID, GUIOBJ_NORMAL);
				GWM_APP_SetObjectDrawMethod (CAS_VA_STATUSLIST_TEXTLIST_TITLE01_ID, (GWM_ObjDraw_t)local_va_statuslist_DrawList_RightsTitle);

				// Theme List Button
				OSD_GFX_CopyRect(&rcTmp, &rect);
				OSD_GFX_SetRect(&rect, rcTmp.x + rcTmp.w, rcTmp.y, ((CAS_VA_STATUSLIST_LIST_BOX_WIDTH)/3), CAS_VA_STATUSLIST_TITLE_HEIGHT);
				GWM_Button_Create(CAS_VA_STATUSLIST_TEXTLIST_TITLE02_ID, &rect, (HUINT8*)GetStrRsc(LOC_CAS_VA_THEME_LEVEL_LIST_ID));
				GWM_Button_SetStatus (CAS_VA_STATUSLIST_TEXTLIST_TITLE02_ID, GUIOBJ_PUSHED);
				GWM_APP_SetObjectDrawMethod (CAS_VA_STATUSLIST_TEXTLIST_TITLE02_ID, (GWM_ObjDraw_t)local_va_statuslist_DrawList_RightsTitle);

				// Prebooked List Button
				OSD_GFX_CopyRect(&rcTmp, &rect);
				OSD_GFX_SetRect(&rect, rcTmp.x + rcTmp.w, rcTmp.y, ((CAS_VA_STATUSLIST_LIST_BOX_WIDTH)/3), CAS_VA_STATUSLIST_TITLE_HEIGHT);
				GWM_Button_Create(CAS_VA_STATUSLIST_TEXTLIST_TITLE03_ID, &rect, (HUINT8*)GetStrRsc(LOC_CAS_VA_PREBOOKED_LIST_ID));
				GWM_Button_SetStatus (CAS_VA_STATUSLIST_TEXTLIST_TITLE03_ID, GUIOBJ_NORMAL);
				GWM_APP_SetObjectDrawMethod (CAS_VA_STATUSLIST_TEXTLIST_TITLE03_ID, (GWM_ObjDraw_t)local_va_statuslist_DrawList_RightsTitle);
			}
			else if (pstContents->eRightsType == eRightsPrebookedList)
			{
				// Class List Button
				OSD_GFX_SetRect(&rect, CAS_VA_STATUSLIST_LIST_BOX_X + CAS_VA_STATUSLIST_TEXT_MARGIN, CAS_VA_STATUSLIST_TITLE_Y,
					CAS_VA_STATUSLIST_LIST_BOX_TITLE_BUTTON_WIDTH, CAS_VA_STATUSLIST_TITLE_HEIGHT);
				GWM_Button_Create(CAS_VA_STATUSLIST_TEXTLIST_TITLE01_ID, &rect, (HUINT8*)GetStrRsc(LOC_CAS_VA_CLASS_LIST_ID));
				GWM_Button_SetStatus (CAS_VA_STATUSLIST_TEXTLIST_TITLE01_ID, GUIOBJ_NORMAL);
				GWM_APP_SetObjectDrawMethod (CAS_VA_STATUSLIST_TEXTLIST_TITLE01_ID, (GWM_ObjDraw_t)local_va_statuslist_DrawList_RightsTitle);

				// Theme List Button
				OSD_GFX_CopyRect(&rcTmp, &rect);
				OSD_GFX_SetRect(&rect, rcTmp.x + rcTmp.w, rcTmp.y, ((CAS_VA_STATUSLIST_LIST_BOX_WIDTH)/3), CAS_VA_STATUSLIST_TITLE_HEIGHT);
				GWM_Button_Create(CAS_VA_STATUSLIST_TEXTLIST_TITLE02_ID, &rect, (HUINT8*)GetStrRsc(LOC_CAS_VA_THEME_LEVEL_LIST_ID));
				GWM_Button_SetStatus (CAS_VA_STATUSLIST_TEXTLIST_TITLE02_ID, GUIOBJ_NORMAL);
				GWM_APP_SetObjectDrawMethod (CAS_VA_STATUSLIST_TEXTLIST_TITLE02_ID, (GWM_ObjDraw_t)local_va_statuslist_DrawList_RightsTitle);

				// Prebooked List Button
				OSD_GFX_CopyRect(&rcTmp, &rect);
				OSD_GFX_SetRect(&rect, rcTmp.x + rcTmp.w, rcTmp.y, ((CAS_VA_STATUSLIST_LIST_BOX_WIDTH)/3), CAS_VA_STATUSLIST_TITLE_HEIGHT);
				GWM_Button_Create(CAS_VA_STATUSLIST_TEXTLIST_TITLE03_ID, &rect, (HUINT8*)GetStrRsc(LOC_CAS_VA_PREBOOKED_LIST_ID));
				GWM_Button_SetStatus (CAS_VA_STATUSLIST_TEXTLIST_TITLE03_ID, GUIOBJ_PUSHED);
				GWM_APP_SetObjectDrawMethod (CAS_VA_STATUSLIST_TEXTLIST_TITLE03_ID, (GWM_ObjDraw_t)local_va_statuslist_DrawList_RightsTitle);
			}
			break;

		default:
			ApCasVaStatusListAssert(0);
			return ERR_FAIL;
	}


	return ERR_OK;
}



STATIC BUS_Result_t local_va_statuslist_DrawList_RightsTitle (OSD_Screen_t *screen, OSD_Area_t *area,
							OSD_Rect_t *rect, GWM_GuiObject_t object, void *arg, int param1)
{
	BUTTON_OBJECT	btn = (BUTTON_OBJECT)arg;
	HINT32			x = 0, y = 0, w = 0, h = 0, fh = 0;
	HUINT32 		status;
	HINT32			fontSize = 0;
	OSD_Font_t		*pFont = NULL;
	char			*str = NULL;
	OSD_Color_t 	txtCol = COLOR_WHITE;

	int				lLeftStockImg, lCenterStockImg, lRightStockImg;

	OSD_Rect_t			rc;
	OSD_StockImage_t	leftImg, centImg, rightImg, arrowImg;

	StatusList_Content_t	*pstContents = NULL;

	x = area->x1 + rect->x;
	y = area->y1 + rect->y;
	y -= CAS_VA_STATUSLIST_TITLE_BUTTON_MARGIN_Y;

	/* width, height¥¬ cursor¿« ≥Ù¿Ã, ≥ ∫Ò¿” */
	w = rect->w - 4;
	h = rect->h;

	pstContents = local_va_statuslist_GetLocalContents();

	fontSize = FONT_SIZE_M;
	status = GWM_Button_GetStatus(btn);

	if (status & GUIOBJ_PUSHED)
	{
		lLeftStockImg = e111_1_00_Cursor_S1_1L;
		lCenterStockImg = e111_1_00_Cursor_S1_2C;
		lRightStockImg = e111_1_00_Cursor_S1_3R;
	}
	else
	{
		lLeftStockImg = e113_1_00_Cursor_D1_H_1L;
		lCenterStockImg = e113_1_00_Cursor_D1_H_2C;
		lRightStockImg = e113_1_00_Cursor_D1_H_3R;
		x -= 2;
	}

	/* active, front most */
	if (!AP_StockImg_GetImage (lLeftStockImg, &leftImg)
		&& !AP_StockImg_GetImage (lCenterStockImg, &centImg)
		&& !AP_StockImg_GetImage (lRightStockImg, &rightImg)
		&& !AP_StockImg_GetImage (e111_1_00_Cursor_2R_Arrow, &arrowImg))
	{
		/* button feedback (just adding y offset)*/
		txtCol = COL(C_T_Cursor);
		fontSize = FONT_SIZE_M;

#if defined(CONFIG_GWM_O2_ANIMATION)
#if 0
		GWM_O2AniWrap_BlendStockFrame(screen, BUS_MGR_GetAppCallback(BUS_MGR_GetThis()), GWM_Obj_GetObjectID(arg),
					x, y, x + w, y + h,
					&leftImg, &centImg, &rightImg, 100 - FRAME_IMAGE_DEFAULT_TRANS);
#else
		/*
			comment : swlee3
			Rights Summaryø°º≠ Class / Theme / Prebook∏ﬁ¥∫
			Up/Down KeyΩ√ ¿ÃªÛ«ˆªÛ¿∏∑Œ O2 ANIMATION πÃªÁøÎ
		*/
		OSD_GFX_BlendStockFrame(screen, x, y, x + w, y + h,
					&leftImg, &centImg, &rightImg, 100 - FRAME_IMAGE_DEFAULT_TRANS);
#endif
#else
		OSD_GFX_BlendStockFrame(screen, x, y, x + w, y + h,
					&leftImg, &centImg, &rightImg, 100 - FRAME_IMAGE_DEFAULT_TRANS);
#endif
	}

	pFont = OSD_FONT_Get(eFont_SystemNormal);
	str = GWM_Button_GetTitlePtr(btn);
	if ((pFont != NULL) && (str != NULL) && (!OSD_FONT_SetSize(pFont, fontSize)))
	{
		AP_UTIL_GetFontRectHeight(pFont, &fh);
		y = (y + 8) + ((CAS_VA_STATUSLIST_TEXT_HEIGHT - fh) >> 1); 	/* left-middle aligned */
		OSD_GFX_DrawStringWithAbbr(screen, pFont, x+CAS_VA_STATUSLIST_TITLE_BUTTON_TEXT_X, y, w, str, txtCol);
	}

	OSD_FONT_Release (pFont);

	return ERR_OK;
}


STATIC BUS_Result_t local_va_statuslist_DrawListItem(OSD_Screen_t *screen, OSD_Area_t *area,
							OSD_Rect_t *rect, GWM_GuiObject_t object, void *arg, int param1)
{
	HINT32		i = 0;
	HINT32		x1 = 0, y1 = 0, x2 = 0, y2 = 0;
	HINT32		x = 0, y = 0;

	HINT32		listId=0, lCellHeight=0;
	HINT32		lastIdx=0;

	HINT32		lCursor_x_off=0, lCursor_y_off=0;
	HINT32		lText_x_off=0, lText_y_off=0;

	HINT32		lAllPage = 0, lTotal=0, lStart=0, lCurr=0, lSelect=0;
	HINT32		lScrollBall_height = 0, lScroll_y_offset = 0;

	HUINT32		ulItem_addr = 0;
	HINT32		lFontRectHeight = 0;
	HUINT32		ulfontTextColor = 0;
	HUINT32		ulfontTextSize = 0;

	HUINT8		*szTmp = NULL;
	HBOOL		bFocused = FALSE;
	HBOOL		bNeedScrollBar = FALSE;
	HERROR		resResult = ERR_FAIL;
	HINT32		lStatuslistItemMax = 0;
	HINT32		lStatuslistScrollbarHeight = 0;

	LIST_OBJECT	obj = (LIST_OBJECT)object;

	OSD_Font_t			*font;

	OSD_StockImage_t 	LeftImg, CenterImg, RightImg, ArrowImg;
	OSD_StockImage_t 	stImgTop, stImgMid, stImgBot;

	StatusList_Content_t	*pstContents = NULL;

	AP_CAS_VA_PARAM_UNUSED(arg);

	/* make coordinate */
	x1 = area->x1+rect->x;
	y1 = area->y1+rect->y;
	x2 = area->x1+rect->x+rect->w;
	y2 = area->y1+rect->y+rect->h;

	/* set properties */
	ulItem_addr = GWM_List_GetArrayPtr(obj);
	GWM_List_GetProperty(obj, &lTotal, &lStart, &lCurr, &lSelect);

	/* no items to draw, just return */
	if ((ulItem_addr == 0) || (lTotal == 0))
	{
		/* dont need to draw */
		return ERR_OK;
	}

	font = OSD_FONT_Get(eFont_SystemNormal);
	if (font == NULL)
	{
		/* cant draw */
		return ERR_OK;
	}

	pstContents = local_va_statuslist_GetLocalContents();

	listId = GWM_Obj_GetObjectID(object);
	lCellHeight = GWM_List_GetCellHeight(listId);

	lCursor_x_off = CAS_VA_STATUSLIST_CURSOR_X;
	lCursor_y_off = CAS_VA_STATUSLIST_CURSOR_Y;

	lText_x_off = CAS_VA_STATUSLIST_TEXT_CONTENT_X;
	lText_y_off = CAS_VA_STATUSLIST_TEXT_FIRST_Y;

	if ( pstContents->lCasVaCAItemID == eCasVAMenuItem_Rights )
	{
		lStatuslistItemMax = CAS_VA_STATUSLIST_ITEM_MAX - 1; // 9 ∞≥
		lStatuslistScrollbarHeight = CAS_VA_STATUSLIST_LIST_SCROLL_HEIGHT - CAS_VA_STATUSLIST_ITEM_LINESPACE;
	}
	else
	{
#if defined(CONFIG_OP_NORDIG_BOXER)
		// ui ªÛ¿∏∑Œ¥¬ max 9∞≥∞° «—∞Ë¿Œµ•, ø÷ 10∞≥∑Œ «ÿ ≥ı¿∫¡ˆ »Æ¿Œ « ø‰
		lStatuslistItemMax = CAS_VA_STATUSLIST_ITEM_MAX - 1; // 9 ∞≥
		lStatuslistScrollbarHeight = CAS_VA_STATUSLIST_LIST_SCROLL_HEIGHT - CAS_VA_STATUSLIST_ITEM_LINESPACE;
#else
		lStatuslistItemMax = CAS_VA_STATUSLIST_ITEM_MAX; // 10 ∞≥
		lStatuslistScrollbarHeight = CAS_VA_STATUSLIST_LIST_SCROLL_HEIGHT;
#endif
	}

	if (lStatuslistItemMax < lTotal)
	{
		lastIdx = lStatuslistItemMax;
		bNeedScrollBar = TRUE;
	}
	else
	{
		lastIdx = lTotal;
	}

	/* «◊ªÛ current index ∞° ¡¶¿œ √π æ∆¿Ã≈€¿Ã µ… ºˆ ¿÷µµ∑œ «—¥Ÿ. */
	for (i = 0; i < lastIdx; i++)
	{
		bFocused = FALSE;

		resResult = UTIL_LL_ReadIndexNthItem(&(pstContents->stTextListBox), lStart+i, (void**)&szTmp);
		if (resResult != ERR_OK)
		{
			continue;
		}

		if (param1 > 0)
		{
			if ((i + lStart == lCurr) && (pstContents->bDrawCursor == TRUE)) //==GUIOBJ_HILIGHTED)
			{
				bFocused = TRUE;
 				ulfontTextColor = COL (C_T_Cursor);
 				ulfontTextSize = FONT_SIZE_L;

				if ((!AP_StockImg_GetImage(e111_1_00_Cursor_S1_1L, &LeftImg))
					&& (!AP_StockImg_GetImage(e111_1_00_Cursor_S1_2C, &CenterImg))
					&& (!AP_StockImg_GetImage(e111_1_00_Cursor_S1_3R, &RightImg)))
				{
#if defined(CONFIG_GWM_O2_ANIMATION)
					GWM_O2AniWrap_BlendStockFrame(screen,BUS_MGR_GetAppCallback(BUS_MGR_GetThis()), GWM_Obj_GetObjectID(object),
							 x1+lCursor_x_off, y1+lCursor_y_off+i*CAS_VA_STATUSLIST_LIST_ITEM_HEIGHT,
#else
					OSD_GFX_BlendStockFrame(screen, x1+lCursor_x_off, y1+lCursor_y_off+i*CAS_VA_STATUSLIST_LIST_ITEM_HEIGHT,
#endif /* #if defined(CONFIG_GWM_O2_ANIMATION) */
						x1+lCursor_x_off+CAS_VA_STATUSLIST_CURSOR_WIDTH, y1+lCursor_y_off+i*CAS_VA_STATUSLIST_LIST_ITEM_HEIGHT+CAS_VA_STATUSLIST_CURSOR_HEIGHT,
						&LeftImg, &CenterImg, &RightImg, CAS_VA_STATUSLIST_TRANSPARENCY);
				}

				if (pstContents->lCasVaCAItemID == eCasVAMenuItem_Rights && pstContents->eRightsType == eRightsOrigin)
				{
					if (!AP_StockImg_GetImage (e111_1_00_Cursor_2R_Arrow, &ArrowImg))
					{
						OSD_Rect_t	rc;
						OSD_GFX_SetRect(&rc, x1+CAS_VA_STATUSLIST_TEXT_CONTENT_WIDTH, y1+30+i*CAS_VA_STATUSLIST_LIST_ITEM_HEIGHT, ArrowImg.width, ArrowImg.height);
#if defined(CONFIG_GWM_O2_ANIMATION)
						GWM_O2AniWrap_FB_Blend(&ArrowImg,  BUS_MGR_GetAppCallback(BUS_MGR_GetThis()), GWM_Obj_GetObjectID(arg), ArrowImg.width, ArrowImg.height, screen, &rc);
#else
						OSD_GFX_FB_Blend(ArrowImg.hBuffer, ArrowImg.width, ArrowImg.height, screen, &rc);
#endif
					}
				}
			}
			else //if (focus==GUIOBJ_DISABLED)
			{
				ulfontTextColor = COL (C_T_Enable);
				if (pstContents->lCasVaCAItemID == eCasVAMenuItem_Stb_Info)
				{
					ulfontTextSize = FONT_SIZE_S;
				}
				else
				{
					ulfontTextSize = FONT_SIZE_M;
				}
			}
		}
		else
		{
 			ulfontTextColor = COL (C_T_Dialog_MSG);

			if ((i + lStart == lCurr) && (pstContents->bDrawCursor == TRUE)) //==GUIOBJ_HILIGHTED)
			{
				bFocused = TRUE;
 				ulfontTextSize = FONT_SIZE_L;

				if ((!AP_StockImg_GetImage(e111_1_00_Cursor_S1_H_1L, &LeftImg))
					&& (!AP_StockImg_GetImage(e111_1_00_Cursor_S1_H_2C, &CenterImg))
					&& (!AP_StockImg_GetImage(e111_1_00_Cursor_S1_H_3R, &RightImg)))
				{
#if defined(CONFIG_GWM_O2_ANIMATION)
					GWM_O2AniWrap_BlendStockFrame(screen,BUS_MGR_GetAppCallback(BUS_MGR_GetThis()), GWM_Obj_GetObjectID(object),
							 x1+lCursor_x_off, y1+lCursor_y_off+i*CAS_VA_STATUSLIST_LIST_ITEM_HEIGHT,
#else
					OSD_GFX_BlendStockFrame(screen, x1+lCursor_x_off, y1+lCursor_y_off+i*CAS_VA_STATUSLIST_LIST_ITEM_HEIGHT,
#endif /* #if defined(CONFIG_GWM_O2_ANIMATION) */
						x1+lCursor_x_off+CAS_VA_STATUSLIST_CURSOR_WIDTH, y1+lCursor_y_off+i*CAS_VA_STATUSLIST_LIST_ITEM_HEIGHT+CAS_VA_STATUSLIST_CURSOR_HEIGHT,
						&LeftImg, &CenterImg, &RightImg, CAS_VA_STATUSLIST_TRANSPARENCY);
				}

				ulfontTextColor = COL (C_T_Cursor);
			}
			else //if (focus==GUIOBJ_DISABLED)
			{
				ulfontTextSize = FONT_SIZE_M;
			}
		}

		/* Draw Text Msg */
		if (szTmp != NULL)
		{
			if (OSD_FONT_SetSize(font, ulfontTextSize))
			{
				OSD_FONT_Release(font);
				ApCasVaStatusListAssert(0);
				ApCasVaStatusListPrint(DBG_PRINT, ("==[local_va_statuslist_DrawListItem]== fail to set a font size.\n"));
				return ERR_FAIL;
			}

			OSD_FONT_GetFontHeight(font, &lFontRectHeight);
			y = y1 + ((CAS_VA_STATUSLIST_TEXT_HEIGHT - lFontRectHeight)>>1) + lText_y_off;		/* y-axis mid-aligned */

			//OSD_GFX_DrawStringWithAbbr(screen, font, x1+lText_x_off, y, CAS_VA_STATUSLIST_TEXT_CONTENT_WIDTH, (char*)szTmp, ulfontTextColor);
			switch (pstContents->lCasVaCAItemID)
			{
				case eCasVAMenuItem_Rights:
					if ( pstContents->eRightsType == eRightsOrigin )
					{
						local_va_statuslist_DrawListItem_Common (screen, font, x1 + CAS_VA_STATUSLIST_RIGHTS_X_MARGIN, y, lText_x_off, szTmp, ulfontTextColor);
					}
					else if ( pstContents->eRightsType == eRightsPrebookedList )
					{
						local_va_statuslist_DrawListItem_Common (screen, font, x1, y, lText_x_off, szTmp, ulfontTextColor);
					}
					else // eRightsClassList or eRightsThemeList
					{
						local_va_statuslist_DrawListItem_RightsSum (screen, font, x1, y, lText_x_off, szTmp, ulfontTextColor);
					}
					break;

				case eCasVAMenuItem_Operator_Msg:
					if ( (lStart+i) == 0)
					{
						ulfontTextColor = COL (C_T_Subtitle01);
						local_va_statuslist_DrawListItem_Common (screen, font, x1, y, lText_x_off, szTmp, ulfontTextColor);
					}
					else
					{
						local_va_statuslist_DrawListItem_Common (screen, font, x1, y, lText_x_off, szTmp, ulfontTextColor);
					}
					break;

				case eCasVAMenuItem_Stb_Info:
#if 0
					if ( lStart % 2 == CAS_VA_STATUSLIST_ITEM_EVEN_LOCATION )
					{
						if ( i % 2 == CAS_VA_STATUSLIST_ITEM_ODD_LOCATION )
						{
							ulfontTextColor = COL (C_T_Subtitle01);
							x1 += CAS_VA_STATUSLIST_ITEM_VALUE_X;
							local_va_statuslist_DrawListItem_Common (screen, font, x1, y, lText_x_off, szTmp, ulfontTextColor);
							x1 -= CAS_VA_STATUSLIST_ITEM_VALUE_X;
						}
						else
						{
							local_va_statuslist_DrawListItem_Common (screen, font, x1, y, lText_x_off, szTmp, ulfontTextColor);
						}
					}
					else
					{
						if ( i % 2 == CAS_VA_STATUSLIST_ITEM_EVEN_LOCATION )
						{
							ulfontTextColor = COL (C_T_Subtitle01);
							x1 += CAS_VA_STATUSLIST_ITEM_VALUE_X;
							local_va_statuslist_DrawListItem_Common (screen, font, x1, y, lText_x_off, szTmp, ulfontTextColor);
							x1 -= CAS_VA_STATUSLIST_ITEM_VALUE_X;
						}
						else
						{
							local_va_statuslist_DrawListItem_Common (screen, font, x1, y, lText_x_off, szTmp, ulfontTextColor);
						}
					}
#endif
					local_va_statuslist_DrawListItem_StbInfos (screen, font, x1, y, lText_x_off, szTmp, ulfontTextColor);
					break;

				case eCasVAMenuItem_Smartcard_Info:
					if ( (lStart+i) == eCasVa_SmartCardInfoItem_UAofScPermitted || (lStart+i) >= VA_UI_SC_AUTOPAIRING_DEFAULT_ITEM_NUM )
					{
						ulfontTextColor = COL (C_T_Subtitle01);
						x1 += CAS_VA_STATUSLIST_ITEM_VALUE_X;
						local_va_statuslist_DrawListItem_Common (screen, font, x1, y, lText_x_off, szTmp, ulfontTextColor);
						x1 -= CAS_VA_STATUSLIST_ITEM_VALUE_X;
					}
					else
					{
						local_va_statuslist_DrawListItem_Common (screen, font, x1, y, lText_x_off, szTmp, ulfontTextColor);
					}
					break;
#ifdef CONFIG_MW_CAS_VIACCESS_PVR
				case eCasVAMenuItem_PvrPlayback_Info:
					if ( (lStart+i) == 0)
					{
						ulfontTextColor = COL (C_T_Subtitle01);
						local_va_statuslist_DrawListItem_Common(screen, font, x1, y, lText_x_off, szTmp, ulfontTextColor);
					}
					else
					{
						local_va_statuslist_DrawListItem_PvrPlaybackInfo(screen, font, x1, y, lText_x_off, szTmp, ulfontTextColor);
					}
					break;
#endif
				default:
					ApCasVaStatusListAssert(0);
					break;
			}
 		}

 		lText_y_off+=lCellHeight;
	}

	OSD_FONT_Release(font);

	/* Draw scroll ball */
	if (bNeedScrollBar == TRUE)
	{
		HINT32	nScrollBall_step = 0, nHeight = 0, tmpTotal = lTotal, dividedStep = 0;

		lAllPage = (lTotal / lStatuslistItemMax);
		lAllPage += (lTotal % lStatuslistItemMax) ? 1 : 0;

		lScrollBall_height = lStatuslistScrollbarHeight / lAllPage;
		if (lScrollBall_height < CAS_VA_STATUSLIST_LIST_SCROLLBALL_H)
		{
			lScrollBall_height = CAS_VA_STATUSLIST_LIST_SCROLLBALL_H;
		}

		nHeight = (lStatuslistScrollbarHeight - lScrollBall_height);
		for (i = 2; tmpTotal > nHeight; i++)
		{
			tmpTotal = lTotal / i;
		} //while (ulTotal > (460 - lScrollBall_height));

		dividedStep = i - 1;

		nScrollBall_step = nHeight / (tmpTotal-1);
		lScroll_y_offset = (lCurr / dividedStep) * nScrollBall_step;
		lScrollBall_height += (nHeight - ((tmpTotal-1) * nScrollBall_step));

		x = x1 + CAS_VA_STATUSLIST_LIST_SCROLL_X;
		y = y1 + CAS_VA_STATUSLIST_LIST_SCROLL_Y;

		/* BG */
		if ((!AP_StockImg_GetImage (e166_1_00_Scroll_B1_1T, &stImgTop))
			&& (!AP_StockImg_GetImage (e166_1_00_Scroll_B1_2M, &stImgMid))
			&& (!AP_StockImg_GetImage (e166_1_00_Scroll_B1_3B, &stImgBot)))
		{
			OSD_GFX_BlendStockVFrame (screen, x, y + lScroll_y_offset, x + stImgTop.width - 1, y + lScroll_y_offset + lScrollBall_height,
									&stImgTop, &stImgMid, &stImgBot, 100);
		}
	}

	return ERR_OK;
}


STATIC HERROR	local_va_statuslist_DrawListItem_Common (OSD_Screen_t *pixmap, OSD_Font_t *font, HINT32 lx, HINT32 ly, HINT32 lxOffset, HUINT8 *szString, OSD_Color_t color)
{
	HINT32	lText_x_off = lxOffset;
	HUINT8	szStr[TEXT_SIZE512] = {0,};
	MWC_UTIL_DvbStrcpy ((HUINT8*)szStr, (const HUINT8*)szString);
	OSD_GFX_DrawStringWithAbbr (pixmap, font, lx+lText_x_off, ly, CAS_VA_STATUSLIST_LIST_BOX_WIDTH, (char*)szStr, color);
	return ERR_OK;
}


STATIC HERROR	local_va_statuslist_DrawListItem_RightsSum (OSD_Screen_t *pixmap, OSD_Font_t *font, HINT32 lx, HINT32 ly, HINT32 lxOffset, HUINT8 *szString, OSD_Color_t color)
{
	HINT32		lText_x_off;
	HUINT8		szStr[3][CAS_VA_MAX_ITEM_STRLEN];

#if 1
	TEXT_DRAW_INFO_t	stTextDrawInfo;
	OSD_Rect_t			stTextRect;
#endif

	lText_x_off = lxOffset;

	AP_CAS_VA_GetWordFromString(szStr[0], szString, 0);
	AP_CAS_VA_GetWordFromString(szStr[1], szString, 1);
	AP_CAS_VA_GetWordFromString(szStr[2], szString, 2);

#if 1
	stTextDrawInfo.ulDrawMode = TEXTMODE_FORCE_FIT;
	stTextDrawInfo.ulFontSize = FONT_SIZE_M;
	stTextDrawInfo.ulTextColor = (HUINT32)color;
	stTextDrawInfo.ulXAlign = TEXT_ALIGN_LEFT;
	stTextDrawInfo.ulYAlign = TEXT_ALIGN_TOP;

	stTextRect.x = lx+lText_x_off;
	stTextRect.y = ly;
	stTextRect.w = CAS_VA_STATUSLIST_RIGHTS_TEXT_WIDTH02;
	stTextRect.h = CAS_VA_STATUSLIST_ITEM_LINESPACE;

	OSD_GFX_DrawStringWithAbbr(pixmap, font, lx+lText_x_off, ly, CAS_VA_STATUSLIST_RIGHTS_TEXT_WIDTH01, (char*)szStr[0], color);

	lText_x_off += (CAS_VA_STATUSLIST_RIGHTS_TEXT_WIDTH01 + CAS_VA_STATUSLIST_TEXT_MARGIN);
	stTextRect.x += lText_x_off;
	OSD_GFX_DrawStringWithAbbr(pixmap, font, lx+lText_x_off, ly, CAS_VA_STATUSLIST_RIGHTS_TEXT_WIDTH01, (char*)szStr[1], color);

	stTextRect.x += (CAS_VA_STATUSLIST_RIGHTS_TEXT_WIDTH01 + CAS_VA_STATUSLIST_TEXT_MARGIN);
	AP_UTIL_DrawFormattedText (pixmap, font, &stTextRect, &stTextDrawInfo, (HUINT8*)szStr[2]);
#else
	OSD_GFX_DrawStringWithAbbr(pixmap, font, lx+lText_x_off, ly, CAS_VA_STATUSLIST_RIGHTS_TEXT_WITH01, (char*)szStr[0], color);
	lText_x_off += (CAS_VA_STATUSLIST_RIGHTS_TEXT_WITH01 + CAS_VA_STATUSLIST_TEXT_MARGIN) - 70;
	OSD_GFX_DrawStringWithAbbr(pixmap, font, lx+lText_x_off, ly, CAS_VA_STATUSLIST_RIGHTS_TEXT_WITH02, (char*)szStr[1], color);
	lText_x_off += (CAS_VA_STATUSLIST_RIGHTS_TEXT_WITH02 + CAS_VA_STATUSLIST_TEXT_MARGIN) - 70;
	OSD_GFX_DrawStringWithAbbr(pixmap, font, lx+lText_x_off, ly, CAS_VA_STATUSLIST_RIGHTS_TEXT_WITH03, (char*)szStr[2], color);
#endif

	return ERR_OK;
}


STATIC HERROR	local_va_statuslist_DrawListItem_StbInfos (OSD_Screen_t *pixmap, OSD_Font_t *font, HINT32 lx, HINT32 ly, HINT32 lxOffset, HUINT8 *szString, OSD_Color_t color)
{
	HINT32		lText_x_off;
	HUINT8		szStr[2][128];

#if 1
	OSD_Rect_t			stTextRect;
	TEXT_DRAW_INFO_t	stTextDrawInfo;
#endif
	lText_x_off = lxOffset;

	AP_CAS_VA_GetWordFromString(szStr[0], szString, 0);
	AP_CAS_VA_GetWordFromString(szStr[1], szString, 1);

#if 1
	stTextDrawInfo.ulDrawMode = TEXTMODE_FORCE_FIT;
	stTextDrawInfo.ulFontSize = FONT_SIZE_XS;
	stTextDrawInfo.ulTextColor = (HUINT32)color;
	stTextDrawInfo.ulXAlign = TEXT_ALIGN_LEFT;
	stTextDrawInfo.ulYAlign = TEXT_ALIGN_TOP;

	stTextRect.x = lx+lText_x_off;
	stTextRect.y = ly;
	stTextRect.w = CAS_VA_STATUSLIST_LIST_WIDTH_SUB;
	stTextRect.h = CAS_VA_STATUSLIST_ITEM_LINESPACE;
	AP_UTIL_DrawFormattedText (pixmap, font, &stTextRect, &stTextDrawInfo, (HUINT8*)szStr[0]);

	lText_x_off += (CAS_VA_STATUSLIST_LIST_WIDTH_SUB - 40 + CAS_VA_STATUSLIST_TEXT_MARGIN);
	stTextDrawInfo.ulXAlign = TEXT_ALIGN_RIGHT;

	stTextRect.x += lText_x_off;
	stTextRect.w += 60;
	AP_UTIL_DrawFormattedText (pixmap, font, &stTextRect, &stTextDrawInfo, (HUINT8*)szStr[1]);
#else
	OSD_GFX_DrawStringWithAbbr(pixmap, font, lx+lText_x_off, ly, CAS_VA_STATUSLIST_LIST_WIDTH_SUB, (char*)szStr[0], color);
	lText_x_off += (CAS_VA_STATUSLIST_LIST_WIDTH_SUB + CAS_VA_STATUSLIST_TEXT_MARGIN);
	OSD_GFX_DrawStringWithAbbr(pixmap, font, lx+lText_x_off, ly, CAS_VA_STATUSLIST_LIST_WIDTH_SUB, (char*)szStr[1], color);
#endif

	return ERR_OK;
}

#ifdef CONFIG_MW_CAS_VIACCESS_PVR
STATIC HERROR	local_va_statuslist_DrawListItem_PvrPlaybackInfo (OSD_Screen_t *pixmap, OSD_Font_t *font, HINT32 lx, HINT32 ly, HINT32 lxOffset, HUINT8 *szString, OSD_Color_t color)
{
	HINT32		lText_x_off;
	HUINT8		szStr[2][128];
	OSD_Rect_t			stTextRect;
	TEXT_DRAW_INFO_t	stTextDrawInfo;

	lText_x_off = lxOffset;

	AP_CAS_VA_GetWordFromString(szStr[0], szString, 0);
	AP_CAS_VA_GetWordFromString(szStr[1], szString, 1);

	stTextDrawInfo.ulDrawMode = TEXTMODE_FORCE_FIT;
	stTextDrawInfo.ulFontSize = FONT_SIZE_M;
	stTextDrawInfo.ulTextColor = (HUINT32)color;
	stTextDrawInfo.ulXAlign = TEXT_ALIGN_LEFT;
	stTextDrawInfo.ulYAlign = TEXT_ALIGN_TOP;

	stTextRect.x = lx+lText_x_off;
	stTextRect.y = ly;
	stTextRect.w = CAS_VA_STATUSLIST_LIST_WIDTH_SUB;
	stTextRect.h = CAS_VA_STATUSLIST_ITEM_LINESPACE;
	AP_UTIL_DrawFormattedText (pixmap, font, &stTextRect, &stTextDrawInfo, (HUINT8*)szStr[0]);

	lText_x_off += (CAS_VA_STATUSLIST_LIST_WIDTH_SUB - 40 + CAS_VA_STATUSLIST_TEXT_MARGIN);

	stTextRect.x += lText_x_off;
	stTextRect.w += 60;
	AP_UTIL_DrawFormattedText (pixmap, font, &stTextRect, &stTextDrawInfo, (HUINT8*)szStr[1]);
}
#endif
#endif

STATIC BUS_Result_t	local_va_statuslist_InitStatusList(StatusList_Content_t *pstContents, HINT32 lItemObjID)
{
#if 0 // not use for octo2.0
	pstContents->lCasVaCAItemID = lItemObjID;

	local_va_statuslist_InitListItems(pstContents);
	local_va_statuslist_LoadItemData(pstContents);
#endif
	return ERR_OK;
}


STATIC HERROR	local_va_statuslist_InitListTitle(StatusList_Content_t *pstContents)
{
#if 0 // not use for octo2.0
	HUINT8	szTitle[TEXT_SIZE128];
	switch (pstContents->lCasVaCAItemID)
	{
		case eCasVAMenuItem_Rights:
			MWC_UTIL_DvbUtf8Sprintf (szTitle, (HUINT8*)"%s", (HUINT8*)GetStrRsc(LOC_CAS_VA_SELECT_OP_LIST_ID));
			break;

		default:
			ApCasVaStatusListAssert(0);
			return ERR_FAIL;
	}
#endif
	return ERR_OK;
}


STATIC void	local_va_statuslist_InitListItems(StatusList_Content_t *pstContents)
{
#if 0 // not use for octo2.0
	pstContents->ulCurFocus = 0;
	pstContents->ulNumOfSubItem = 0;
	pstContents->eRightsType = eRightsOrigin;

	UTIL_LL_InitList(&(pstContents->stTextListBox));

	pstContents->pszOpMessage = NULL;
#endif
}


STATIC HERROR	local_va_statuslist_ResetLists(StatusList_Content_t *pstContents)
{
#if 0 // not use for octo2.0
	/* tmp move listµÈ¿∫ current grpø° ¥Î«ÿº≠∏∏ ªÁøÎµ«π«∑Œ
		¿ÃπÃ curList∏¶ editøœ∑·∑Œ ∫∏∞Ì list∏∏ √ ±‚»≠ «œ∞Ì, content¥¬ ¡ˆøÏ∏È æ»µ»¥Ÿ. */

	UTIL_LL_SetListEmpty(&(pstContents->stTextListBox), (ll_deleteitem_method)NULL);
#endif
	return ERR_OK;
}


STATIC HERROR	local_va_statuslist_LoadItemData(StatusList_Content_t *pstContents)
{
	HERROR		hError = ERR_FAIL;

	switch (pstContents->lCasVaCAItemID)
	{
		case eCasVAMenuItem_Rights:
			xmgr_cas_VaGetOpList(&s_stOpList);
			hError = local_va_statuslist_LoadItem_RightsData(pstContents, 0);
			break;

		case eCasVAMenuItem_Operator_Msg:
			xmgr_cas_VaGetOpLastMessage(&s_pstOpMsg);
			hError = local_va_statuslist_LoadItem_OperatorMsgData(pstContents);
			break;

		case eCasVAMenuItem_Stb_Info:
			xmgr_cas_VaGetStbInfo(&s_stStbInfo);
			hError = local_va_statuslist_LoadItem_StbInfoData(pstContents);
			break;

		case eCasVAMenuItem_Smartcard_Info:
			xmgr_cas_VaGetSmartcardInfo(&s_stScInfo);
			hError = local_va_statuslist_LoadItem_SmartCardInfoData(pstContents);
			break;

#ifdef CONFIG_MW_CAS_VIACCESS_PVR
		case eCasVAMenuItem_PvrPlayback_Info:
			local_va_statuslist_GetPlaybackSvcName(&s_stPbInfo.szSvcName);
			xmgr_cas_VaGetPlaybackInfo(&s_stPbInfo.stInfo);
			hError = local_va_statuslist_LoadItem_PvrPlaybackInfoData(pstContents);
			break;
#endif

		default:
			ApCasVaStatusListAssert(0);
			return ERR_FAIL;
	}
    return ERR_OK;
}

STATIC HERROR	local_va_statuslist_LoadItem_RightsData(StatusList_Content_t *pstContents, HINT32 lIndex)
{
#if 0 // not use for octo2.0
	HERROR		hError = ERR_FAIL;
	HUINT8		szTemp[TEXT_SIZE128], szStr1[TEXT_SIZE128], szStr2[TEXT_SIZE128], szStr3[TEXT_SIZE128];
	HUINT8		*pszText = NULL, *szStrOperator = NULL;
	HUINT32 	i, ulStrlen, ulEachItemValue, ulCnt;
	HINT32		nRet = VA_UI_NO_MODE;

	VA_MENU_CLASS_CONSULTATION stClass;
	VA_MENU_THEME_CONSULTATION stTheme;
	VA_MENU_PREBOOK_CONSULTATION stPrebook;

	// Choose Draw Cursor
	if ( pstContents->eRightsType == eRightsOrigin )
	{
		pstContents->bDrawCursor = TRUE;
	}
	else // eRightsClassList or eRightsThemeList
	{
		pstContents->bDrawCursor = FALSE;
	}

	// List Item Draw Function
	pstContents->objDrawFunc = local_va_statuslist_DrawListItem;

	s_eCurrentRightsType = pstContents->eRightsType;

	switch (pstContents->eRightsType)
	{
		case eRightsOrigin:
			{
				for ( i = 1; i < s_stOpList.uiOpNum; i++)
				{
					MWC_UTIL_DvbUtf8Sprintf(szTemp, (HUINT8*)"%s", s_stOpList.pOpList[i].szLable);

					pszText = (HUINT8*)OxAP_Malloc(SvcCas_UtilStrLen(szTemp) + 1);
					MWC_UTIL_DvbStrcpy (pszText, szTemp);
					hError = CAS_UTIL_LL_AddItem (&(pstContents->stTextListBox), pszText);
					if (hError != ERR_OK)
					{
						ApCasVaStatusListAssert(0);
					}
				}
			}
			break;

		case eRightsClassList:
			{
				AP_CAS_VA_GetOpClass(&s_stOpList.pOpList[s_uiSelectOpListIdx], &stClass, &nRet);

				if(nRet == VA_UI_ERR_CONSULTATION_LOCKED)
				{
					BUS_MGR_Create("local_cas_va_UnlockConsultationProc", APP_DIALOGBOX_PRIORITY, local_cas_va_UnlockConsultationProc,
						0, (HINT32)eVA_PinCheckStep_InDlg_InputPincode, 0, 0);
				}

				MWC_UTIL_DvbStrcpy ((HUINT8*)szStr1, (const HUINT8*)GetStrRsc(LOC_CAS_VA_START_DATE_ID));
				MWC_UTIL_DvbStrcpy ((HUINT8*)szStr2, (const HUINT8*)GetStrRsc(LOC_CAS_VA_EXPIRY_DATE_ID));
				MWC_UTIL_DvbStrcpy ((HUINT8*)szStr3, (const HUINT8*)GetStrRsc(LOC_CAS_VA_CLASSES_ID));

				//MWC_UTIL_DvbUtf8Sprintf (szTemp, (HUINT8*)"%s|%s|%s", szStr1, szStr2, szStr3);
				snprintf(szTemp, TEXT_SIZE128, (HUINT8*)"%s|%s|%s", szStr1, szStr2, szStr3);

				pszText = (HUINT8*)OxAP_Malloc(SvcCas_UtilStrLen(szTemp) + 1);
				MWC_UTIL_DvbStrcpy (pszText, szTemp);
				hError = CAS_UTIL_LL_AddItem (&(pstContents->stTextListBox), pszText);
				if (hError != ERR_OK)
				{
					ApCasVaStatusListAssert(0);
				}

				/* 2nd blank item */
				MWC_UTIL_DvbUtf8Sprintf(szTemp, (HUINT8*)"%s|%s|%s", (HUINT8*)" ", (HUINT8*)" ", (HUINT8*)" ");

				pszText = (HUINT8*)OxAP_Malloc(SvcCas_UtilStrLen(szTemp) + 1);
				MWC_UTIL_DvbStrcpy (pszText, szTemp);
				hError = CAS_UTIL_LL_AddItem (&(pstContents->stTextListBox), pszText);
				if (hError != ERR_OK)
				{
					ApCasVaStatusListAssert(0);
				}

				for ( i = 0; i < stClass.uiNum; i++)
				{
					// start date
					MWC_UTIL_DvbStrcpy(szStr1, stClass.pList[i].szStartDate);

					// expiry date
					MWC_UTIL_DvbStrcpy(szStr2, stClass.pList[i].szEndDate);

					// classes
					AP_CAS_VA_GetClassMarkStr(&stClass.pList[i], szStr3);

					// all
					//MWC_UTIL_DvbUtf8Sprintf (szTemp, (HUINT8*)"%s|%s|%s", szStr1, szStr2, szStr3);
					snprintf(szTemp, TEXT_SIZE128, (HUINT8*)"%s|%s|%s", szStr1, szStr2, szStr3);

					// add item
					pszText = (HUINT8*)OxAP_Malloc(SvcCas_UtilStrLen(szTemp) + 1);
					MWC_UTIL_DvbStrcpy (pszText, szTemp);
					hError = CAS_UTIL_LL_AddItem (&(pstContents->stTextListBox), pszText);
					if (hError != ERR_OK)
					{
						ApCasVaStatusListAssert(0);
					}
				}

				if(stClass.pList)
				{
					OxAP_Free(stClass.pList);
					stClass.pList = NULL;
				}
			}

			break;

		case eRightsThemeList:
			{
				AP_CAS_VA_GetOpTheme(&s_stOpList.pOpList[s_uiSelectOpListIdx], &stTheme, &nRet);

				if(nRet == VA_UI_ERR_CONSULTATION_LOCKED)
				{
					BUS_MGR_Create("local_cas_va_UnlockConsultationProc", APP_DIALOGBOX_PRIORITY, local_cas_va_UnlockConsultationProc,
						0, (HINT32)eVA_PinCheckStep_InDlg_InputPincode, 0, 0);
				}

				/* 1st title item */
				MWC_UTIL_DvbStrcpy ((HUINT8*)szStr1, (const HUINT8*)GetStrRsc(LOC_CAS_VA_START_DATE_ID));
				MWC_UTIL_DvbStrcpy ((HUINT8*)szStr2, (const HUINT8*)GetStrRsc(LOC_CAS_VA_EXPIRY_DATE_ID));
				MWC_UTIL_DvbStrcpy ((HUINT8*)szStr3, (const HUINT8*)GetStrRsc(LOC_CAS_VA_THEME_LEVEL_ID));

				//MWC_UTIL_DvbUtf8Sprintf (szTemp, (HUINT8*)"%s|%s|%s", szStr1, szStr2, szStr3);
				snprintf(szTemp, TEXT_SIZE128, (HUINT8*)"%s|%s|%s", szStr1, szStr2, szStr3);

				pszText = (HUINT8*)OxAP_Malloc(SvcCas_UtilStrLen(szTemp) + 1);
				MWC_UTIL_DvbStrcpy (pszText, szTemp);
				hError = CAS_UTIL_LL_AddItem (&(pstContents->stTextListBox), pszText);
				if (hError != ERR_OK)
				{
					ApCasVaStatusListAssert(0);
				}

				/* 2nd blank item */
				MWC_UTIL_DvbUtf8Sprintf(szTemp, (HUINT8*)"%s|%s|%s", (HUINT8*)" ", (HUINT8*)" ", (HUINT8*)" ");

				pszText = (HUINT8*)OxAP_Malloc(SvcCas_UtilStrLen(szTemp) + 1);
				MWC_UTIL_DvbStrcpy (pszText, szTemp);
				hError = CAS_UTIL_LL_AddItem (&(pstContents->stTextListBox), pszText);
				if (hError != ERR_OK)
				{
					ApCasVaStatusListAssert(0);
				}

				for (i = 0 ; i < stTheme.uiNum ; i++)
				{
					// start date
					MWC_UTIL_DvbStrcpy(szStr1, stTheme.pList[i].szStartDate);

					// expiry date
					MWC_UTIL_DvbStrcpy(szStr2, stTheme.pList[i].szEndDate);

					// theme/level
					MWC_UTIL_DvbUtf8Sprintf (szStr3, (HUINT8*)"[Th %d / Lv %d]", stTheme.pList[i].ucTheme, stTheme.pList[i].ucLevel); // GetStrSrc (...)

					// all
					//MWC_UTIL_DvbUtf8Sprintf (szTemp, (HUINT8*)"%s|%s|%s", szStr1, szStr2, szStr3);
					snprintf(szTemp, TEXT_SIZE128, (HUINT8*)"%s|%s|%s", szStr1, szStr2, szStr3);

					// add item
					pszText = (HUINT8*)OxAP_Malloc(SvcCas_UtilStrLen(szTemp) + 1);
					MWC_UTIL_DvbStrcpy (pszText, szTemp);
					hError = CAS_UTIL_LL_AddItem (&(pstContents->stTextListBox), pszText);
					if (hError != ERR_OK)
					{
						ApCasVaStatusListAssert(0);
					}
				}

				if(stTheme.pList)
				{
					OxAP_Free(stTheme.pList);
					stTheme.pList = NULL;
				}
			}
			break;


		case eRightsPrebookedList:
			{
				AP_CAS_VA_GetOpPrebook(&s_stOpList.pOpList[s_uiSelectOpListIdx], &stPrebook, &nRet);

				if(nRet == VA_UI_ERR_CONSULTATION_LOCKED)
				{
					BUS_MGR_Create("local_cas_va_UnlockConsultationProc", APP_DIALOGBOX_PRIORITY, local_cas_va_UnlockConsultationProc,
						0, (HINT32)eVA_PinCheckStep_InDlg_InputPincode, 0, 0);
				}

				/* 1st title item */
				MWC_UTIL_DvbStrcpy ((HUINT8*)szStr1, (const HUINT8*)GetStrRsc(LOC_CAS_VA_PROGRAMME_NUMBER_ID));
				MWC_UTIL_DvbUtf8Sprintf (szTemp, (HUINT8*)"%s", szStr1, szStr2, szStr3);

				pszText = (HUINT8*)OxAP_Malloc(SvcCas_UtilStrLen(szTemp) + 1);
				MWC_UTIL_DvbStrcpy (pszText, szTemp);
				hError = CAS_UTIL_LL_AddItem (&(pstContents->stTextListBox), pszText);
				if (hError != ERR_OK)
				{
					ApCasVaStatusListAssert(0);
				}

				/* 2nd blank item */
				MWC_UTIL_DvbUtf8Sprintf(szTemp, (HUINT8*)"%s", (HUINT8*)" ");

				pszText = (HUINT8*)OxAP_Malloc(SvcCas_UtilStrLen(szTemp) + 1);
				MWC_UTIL_DvbStrcpy (pszText, szTemp);
				hError = CAS_UTIL_LL_AddItem (&(pstContents->stTextListBox), pszText);
				if (hError != ERR_OK)
				{
					ApCasVaStatusListAssert(0);
				}

				for ( i = 0 ; i < stPrebook.uiNum ; i++)
				{
					// prebooked list ( min ~ max )
					if (stPrebook.pList[i].ulMin == stPrebook.pList[i].ulMax)
					{
						MWC_UTIL_DvbUtf8Sprintf (szStr1, (HUINT8*)"[%d]", stPrebook.pList[i].ulMin);
					}
					else
					{
						MWC_UTIL_DvbUtf8Sprintf(szStr1, (HUINT8*)"[%d - %d]", stPrebook.pList[i].ulMin, stPrebook.pList[i].ulMax);
					}

					// all
					MWC_UTIL_DvbUtf8Sprintf (szTemp, (HUINT8*)"%s", szStr1);

					// add item
					pszText = (HUINT8*)OxAP_Malloc(SvcCas_UtilStrLen(szTemp) + 1);
					MWC_UTIL_DvbStrcpy (pszText, szTemp);
					hError = CAS_UTIL_LL_AddItem (&(pstContents->stTextListBox), pszText);
					if (hError != ERR_OK)
					{
						ApCasVaStatusListAssert(0);
					}
				}

				if(stPrebook.pList)
				{
					OxAP_Free(stPrebook.pList);
					stPrebook.pList = NULL;
				}
			}
			break;

		default:
			break;
	}
#endif
	return ERR_OK;
}


STATIC HERROR	local_va_statuslist_LoadItem_StbInfoData(StatusList_Content_t *pstContents)
{
#if 0 // not use for octo2.0
	HERROR		hError = ERR_FAIL;
	HUINT8		*pszText = NULL;
	HUINT8		szTemp[TEXT_SIZE128];
	HUINT32 	i;

	// Choose Draw Cursor
	pstContents->bDrawCursor = FALSE;

	// List Item Draw Function
	pstContents->objDrawFunc = local_va_statuslist_DrawListItem;

	// Item value's location is even rows
	for (i = 0; i < eCasVa_StbInfoItem_Max; i++)
	{
		switch ( i )
		{
			case	eCasVa_StbInfoItem_VaManuf_ID:
				MWC_UTIL_DvbUtf8Sprintf(szTemp, (HUINT8*)"%s|%d", GetStrRsc(CasVaStbInfo_StrId[i]), s_stStbInfo.uiManufId);
				break;
			case	eCasVa_StbInfoItem_VaModel_ID:
				MWC_UTIL_DvbUtf8Sprintf(szTemp, (HUINT8*)"%s|%d", GetStrRsc(CasVaStbInfo_StrId[i]), s_stStbInfo.uiModelId);
				break;
			case	eCasVa_StbInfoItem_Teminal_SN:
				MWC_UTIL_DvbUtf8Sprintf(szTemp, (HUINT8*)"%s|%s", GetStrRsc(CasVaStbInfo_StrId[i]), s_stStbInfo.pszTerminalSn);
				break;
			case	eCasVa_StbInfoItem_HW_Version:
				MWC_UTIL_DvbUtf8Sprintf(szTemp, (HUINT8*)"%s|%s", GetStrRsc(CasVaStbInfo_StrId[i]), s_stStbInfo.pszHwVer);
				break;
			case	eCasVa_StbInfoItem_Bootloader_Version:
				MWC_UTIL_DvbUtf8Sprintf(szTemp, (HUINT8*)"%s|%s", GetStrRsc(CasVaStbInfo_StrId[i]), s_stStbInfo.pszBlVer);
				break;
			case	eCasVa_StbInfoItem_SW_Version:
				MWC_UTIL_DvbUtf8Sprintf(szTemp, (HUINT8*)"%s|%s", GetStrRsc(CasVaStbInfo_StrId[i]), s_stStbInfo.pszSwVer);
				break;
			case	eCasVa_StbInfoItem_STB_ID:
				MWC_UTIL_DvbUtf8Sprintf(szTemp, (HUINT8*)"%s|%s", GetStrRsc(CasVaStbInfo_StrId[i]), s_stStbInfo.pszStbId);
				break;
#ifdef CONFIG_MW_CAS_VIACCESS_PVR
			case	eCasVa_StbInfoItem_HDD_SN:
				MWC_UTIL_DvbUtf8Sprintf(szTemp, (HUINT8*)"%s|%s", GetStrRsc(CasVaStbInfo_StrId[i]), s_stStbInfo.pszHddSn);
				break;
#endif
			case	eCasVa_StbInfoItem_UA:
				MWC_UTIL_DvbUtf8Sprintf(szTemp, (HUINT8*)"%s|%s", GetStrRsc(CasVaStbInfo_StrId[i]), s_stStbInfo.pszUA);
				break;
			case	eCasVa_StbInfoItem_ACS_Version:
				MWC_UTIL_DvbUtf8Sprintf(szTemp, (HUINT8*)"%s|%s", GetStrRsc(CasVaStbInfo_StrId[i]), s_stStbInfo.pszAcsVer);
				break;
#ifdef CONFIG_MW_CAS_VIACCESS_PURPLE_BOX
			case	eCasVa_StbInfoItem_Terminal_ID:
				MWC_UTIL_DvbUtf8Sprintf(szTemp, (HUINT8*)"%s|%s", GetStrRsc(CasVaStbInfo_StrId[i]), s_stStbInfo.pszTerminalId);
				break;
#endif
			case	eCasVa_StbInfoItem_CAMLock_Value:
				MWC_UTIL_DvbUtf8Sprintf(szTemp, (HUINT8*)"%s|%s", GetStrRsc(CasVaStbInfo_StrId[i]), s_stStbInfo.pszCamlock);
				break;
			default:
				break;
		}

		pszText = (HUINT8*)OxAP_Malloc(SvcCas_UtilStrLen(szTemp) + 1);
		MWC_UTIL_DvbStrcpy(pszText, szTemp);

		/* linked list ø° πËø≠¿« ø¯º“∏¶ «œ≥™«œ≥™ ∏ÆΩ∫∆Æ∑Œ ∆˜¿Œ∆√ Ω√≈≤¥Ÿ. */
		hError = CAS_UTIL_LL_AddItem(&(pstContents->stTextListBox), pszText);
		if (hError != ERR_OK)
		{
			/* Ω√Ω∫≈€ø° ¿««— ∏ﬁ∏∏Æ ¡∂¿€¿Ã æ∆¥œ∏È ¿˝¥Î πﬂª˝«œ¡ˆ æ ¿ª µÌ, Linux only */
			ApCasVaStatusListAssert(0);
		}
	}
#endif
	return ERR_OK;
}


STATIC HERROR	local_va_statuslist_LoadItem_OperatorMsgData(StatusList_Content_t *pstContents)
{
#if 0 // not use for octo2.0
	HUINT8		szTemp[TEXT_SIZE512], szConvOpMsg[VA_UI_OP_MSG_SIZE] = {0,};
	HUINT8		*pszText = NULL;
	HUINT32 	uiConvLen = 0;


	pstContents->objDrawFunc = AP_LNF_PlainMultilineText_Draw;

	if(s_pstOpMsg != NULL)
	{
		AP_CAS_VA_GetConvertOpMessage(s_pstOpMsg, szConvOpMsg, &uiConvLen);
	}

	MWC_UTIL_DvbUtf8Sprintf(szTemp, "%s\n \n%s", GetStrRsc(LOC_CAS_VA_OP_LAST_MSG_RECV_ID), szConvOpMsg);

	pszText = (HUINT8*)OxAP_Malloc(SvcCas_UtilStrLen(szTemp) + 1);
	MWC_UTIL_DvbStrcpy(pszText, szTemp);

	pstContents->pszOpMessage = pszText;
#endif
	return ERR_OK;
}



STATIC HERROR	local_va_statuslist_LoadItem_SmartCardInfoData(StatusList_Content_t *pstContents)
{
#if 0 // not use for octo2.0
	HERROR		hError = ERR_FAIL;
	HUINT8		*pszText = NULL;
	HUINT8		szTemp[TEXT_SIZE128], szStr1[TEXT_SIZE128];
	HUINT32 	i, ulStrlen, ulEachItemValue;

	// Choose Draw Cursor
	pstContents->bDrawCursor = FALSE;

	// List Item Draw Function
	pstContents->objDrawFunc = local_va_statuslist_DrawListItem;

	// Item value's location is even rows

	// Title : Pairing Infomation
	MWC_UTIL_DvbStrcpy ((HUINT8*)szStr1, (const HUINT8*)GetStrRsc(LOC_CAS_VA_SC_INFO_PAIR_INFO_ID));
	szStr1[D_TEXT_LINE_LENGTH] = 0;
	MWC_UTIL_DvbUtf8Sprintf(szTemp, (HUINT8*)"%s", szStr1);

	pszText = (HUINT8*)OxAP_Malloc(SvcCas_UtilStrLen(szTemp) + 1);
	MWC_UTIL_DvbStrcpy(pszText, szTemp);

	hError = CAS_UTIL_LL_AddItem(&(pstContents->stTextListBox), pszText);
	if (hError != ERR_OK)
	{
		ApCasVaStatusListAssert(0);
	}

	if (s_stScInfo.uiPairMode == VA_UI_AUTOPAIRING_MODE)
	{
		MWC_UTIL_DvbStrcpy ((HUINT8*)szStr1, (const HUINT8*)GetStrRsc(LOC_CAS_VA_SC_INFO_PAIR_MODE_ID));
		MWC_UTIL_DvbUtf8Sprintf (szTemp, (HUINT8*)"%s : [ %s ]", szStr1, GetStrRsc(LOC_CAS_VA_AUTOPAIRING_ID));
		pszText = (HUINT8*)OxAP_Malloc(SvcCas_UtilStrLen(szTemp) + 1);
		MWC_UTIL_DvbStrcpy (pszText, szTemp);
		hError = CAS_UTIL_LL_AddItem(&(pstContents->stTextListBox), pszText);
		if (hError != ERR_OK)
		{
			ApCasVaStatusListAssert(0);
		}

		for (i=0; i < eCasVa_SmartCardInfoItem_Max; i++)
		{
			switch ( i )
			{
				case	eCasVa_SmartCardInfoItem_FirstBlank:
				case	eCasVa_SmartCardInfoItem_SecondBlank:
					MWC_UTIL_DvbStrcpy((HUINT8 *)szStr1, (const HUINT8 *)"");
					szStr1[D_TEXT_LINE_LENGTH] = 0;
					MWC_UTIL_DvbUtf8Sprintf(szTemp, (HUINT8*)"%s", szStr1);
					break;

				case 	eCasVa_SmartCardInfoItem_AutoParingMode:
					MWC_UTIL_DvbStrcpy((HUINT8*)szStr1, (const HUINT8*)GetStrRsc(LOC_CAS_VA_AUTOPAIRING_MODE_ID));
					szStr1[D_TEXT_LINE_LENGTH] = 0;
					MWC_UTIL_DvbUtf8Sprintf(szTemp, (HUINT8*)"%s :", szStr1);
					break;

				case	eCasVa_SmartCardInfoItem_NumOfScPermiited:
					MWC_UTIL_DvbStrcpy((HUINT8*)szStr1, (const HUINT8*)GetStrRsc(LOC_CAS_VA_AUTOPAIRING_NUM_OF_SC_ID));
					szStr1[D_TEXT_LINE_LENGTH] = 0;
					MWC_UTIL_DvbUtf8Sprintf(szTemp, (HUINT8*)"%s : [ %d ]", szStr1, s_stScInfo.uiPairNum);
					break;

				case	eCasVa_SmartCardInfoItem_UAofScPermitted:
					MWC_UTIL_DvbStrcpy((HUINT8*)szStr1, (const HUINT8*)GetStrRsc(LOC_CAS_VA_UA_OF_PERMITTED_SC_ID));
					szStr1[D_TEXT_LINE_LENGTH] = 0;
					MWC_UTIL_DvbUtf8Sprintf(szTemp, (HUINT8*)"%s :", szStr1);
					break;

				default:
					break;
			}

			pszText = (HUINT8*)OxAP_Malloc(SvcCas_UtilStrLen(szTemp) + 1);
			MWC_UTIL_DvbStrcpy(pszText, szTemp);

			hError = CAS_UTIL_LL_AddItem(&(pstContents->stTextListBox), pszText);
			if (hError != ERR_OK)
			{
				ApCasVaStatusListAssert(0);
			}
		}

		// add UA of permitted smartcards
		for (i = 0; i < s_stScInfo.uiUaNum; i++)
		{
			MWC_UTIL_DvbUtf8Sprintf (szTemp, (HUINT8*)"[ %d ]", s_stScInfo.pUaList[i]);
			pszText = (HUINT8*)OxAP_Malloc(SvcCas_UtilStrLen(szTemp) + 1);
			MWC_UTIL_DvbStrcpy (pszText, szTemp);

			hError = CAS_UTIL_LL_AddItem (&(pstContents->stTextListBox), pszText);
			if (hError != ERR_OK)
			{
				ApCasVaStatusListAssert(0);
			}
		}
	}
	else
	{
		MWC_UTIL_DvbStrcpy ((HUINT8*)szStr1, (const HUINT8*)GetStrRsc(LOC_CAS_VA_SC_INFO_PAIR_MODE_ID));
		MWC_UTIL_DvbUtf8Sprintf (szTemp, (HUINT8*)"%s", szStr1);
		pszText = (HUINT8*)OxAP_Malloc(SvcCas_UtilStrLen(szTemp) + 1);
		MWC_UTIL_DvbStrcpy (pszText, szTemp);

		hError = CAS_UTIL_LL_AddItem(&(pstContents->stTextListBox), pszText);
		if (hError != ERR_OK)
		{
			ApCasVaStatusListAssert(0);
		}
	}
#endif
	return ERR_OK;
}


STATIC HERROR	local_va_statuslist_FreeItemData(StatusList_Content_t *pstContents)
{
#if 0 // not use for octo2.0
	HINT32		i, lNumOfList = 0;
	HUINT8		*szTmp = NULL;
	HERROR		hError = ERR_FAIL;

	lNumOfList = UTIL_LL_GetNumOfListItem(&(pstContents->stTextListBox));

	for (i = 0; i < lNumOfList; i++)
	{
		hError = UTIL_LL_ReadIndexNthItem(&(pstContents->stTextListBox), i, (void**)&szTmp);
		if (hError != ERR_OK)
		{
			ApCasVaStatusListAssert(0);
		}
		else
		{
			OxAP_Free(szTmp);
		}
	}

	if(pstContents->pszOpMessage)
		OxAP_Free(pstContents->pszOpMessage);
#endif
	return ERR_OK;
}

#ifdef CONFIG_MW_CAS_VIACCESS_PVR
STATIC HERROR	local_va_statuslist_GetPlaybackSvcName(HUINT8 *pSvcName)
{
	HUINT32 ulActionId;
	MgrPb_Info_t stPbInfo;
	SvcMeta_Record_t stRecInfo;
	HERROR	hError;

	ulActionId = MGR_ACTION_GetMainActionId();

	hError = MGR_PLAYBACK_GetPlaybackInfo(ulActionId, &stPbInfo);
	if(hError == ERR_FAIL)
	{
		pSvcName[0] = 0;
		return hError;
	}

	hError = SVC_META_GetRecordInfo(stPbInfo.ulContentId, &stRecInfo);
	if(hError == ERR_FAIL)
	{
		pSvcName[0] = 0;
		return hError;
	}

	VK_memset32(pSvcName, 0, sizeof(TEXT_SIZE128));

	pSvcName[0] = 0x15;
	MWC_UTIL_DvbStrncpy(pSvcName + 1, stRecInfo.aucDisplayEvtName, TEXT_SIZE128 - 2);

	return ERR_OK;
}

STATIC HERROR	local_va_statuslist_LoadItem_PvrPlaybackInfoData(StatusList_Content_t *pstContents)
{
	HERROR		hError = ERR_FAIL;
	HUINT8		*pszText = NULL;
	HUINT8		szTemp[TEXT_SIZE256], szStr1[TEXT_SIZE256];
	HUINT32 	i, ulEachItemValue, uiDateLen;
	int idx = 0;

#if 0// ysmoon_20130812
	// Choose Draw Cursor
	pstContents->bDrawCursor = FALSE;

	// List Item Draw Function
	pstContents->objDrawFunc = local_va_statuslist_DrawListItem;
#endif
	// playback ªÛ≈¬∞° æ∆¥“ ∞ÊøÏ æ∆π´∞Õµµ «•Ω√«œ¡ˆ æ ∞Ì ¡æ∑·...
	if(MGR_ACTION_GetType(MGR_ACTION_GetMainActionId()) != eViewType_PB)
	{
		return ERR_OK;
	}

	for(i = 0 ; i < eCasVa_PvrPlaybackInfoItem_Max ; i++)
	{
		switch(i)
		{
			case eCasVa_PvrPlaybackInfoItem_RecordDate:
				uiDateLen = sizeof(s_stPbInfo.stInfo.szRecordDate);
				MWC_UTIL_DvbStrncpy(szTemp, s_stPbInfo.stInfo.szRecordDate, uiDateLen);
				szTemp[uiDateLen] = 0;

				MWC_UTIL_DvbUtf8Sprintf (szStr1, (HUINT8*)"%s %s", s_stPbInfo.szSvcName, szTemp);
				MWC_UTIL_DvbUtf8Sprintf (szTemp, (HUINT8*)"%s", szStr1);
				break;
#if 0// ysmoon_20130812
			case eCasVa_PvrPlaybackInfoItem_FirstPbDate:
				MWC_UTIL_DvbUtf8Sprintf ((HUINT8*)szStr1, (HUINT8 *)"   %s :|%s", (const HUINT8*)GetStrRsc(LOC_CAS_VA_FIRST_PLAYBACK_DATE_ID), s_stPbInfo.stInfo.szFirstPlaybackDate);
				szStr1[D_TEXT_LINE_LENGTH] = 0;
				MWC_UTIL_DvbUtf8Sprintf (szTemp, (HUINT8*)"%s", szStr1);
				break;

			case eCasVa_PvrPlaybackInfoItem_NumOfPb:
				MWC_UTIL_DvbUtf8Sprintf ((HUINT8*)szStr1, (HUINT8 *)"   %s :|%d", (const HUINT8*)GetStrRsc(LOC_CAS_VA_NUMBER_OF_PLAYBACK_ID), s_stPbInfo.stInfo.ulPlaybackCounter);
				szStr1[D_TEXT_LINE_LENGTH] = 0;
				MWC_UTIL_DvbUtf8Sprintf (szTemp, (HUINT8*)"%s", szStr1);
				break;
#endif
			default:
				break;
		}

		pszText = (HUINT8*)OxAP_Malloc (SvcCas_UtilStrLen(szTemp) + 1);
		MWC_UTIL_DvbStrcpy (pszText, szTemp);

		hError = CAS_UTIL_LL_AddItem (&(pstContents->stTextListBox), pszText);
		if (hError != ERR_OK)
		{
			ApCasVaStatusListAssert(0);
		}
	}

	return ERR_OK;
}
#endif


#define	________________Global_Function_______________________

/*******************************************************************/
/********************      	Global Function 	********************/
/*******************************************************************/


BUS_Result_t	xmgr_cas_VaStatusListProc (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t			eBusResult = ERR_BUS_NO_ERROR;
	StatusList_Content_t	*pstContents = local_va_statuslist_GetLocalContents();
	HINT32					lCurIdx = 0, lNumOfList = 0;

	switch (message)
	{
		case eMEVT_BUS_CREATE:
			/* init */
			local_va_statuslist_InitStatusList (pstContents, p3);

			/* set front panel */
#if defined(CONFIG_MW_SYS_7SEG) && defined(CONFIG_OP_NTVPLUS)
			//AP_SPEC_DisplayMenuPanelString(pstContents->szTitle, eDxPANEL_ALIGN_CENTER);
#else
			SVC_SYS_DisplayPanelString (ePANEL_STR_PRIORITY_MENU, pstContents->szTitle, eDxPANEL_ALIGN_CENTER);
#endif


			#if 0 // not use for octo2.0

			/* set app area */
			GWM_APP_SetAppArea (CAS_VA_STATUSLIST_APP_WINDOW_X, CAS_VA_STATUSLIST_APP_WINDOW_Y,
							CAS_VA_STATUSLIST_APP_WINDOW_WIDTH, CAS_VA_STATUSLIST_APP_WINDOW_HEIGHT);

			/* init UI object */
			//local_va_statuslist_DrawDefaultUI (pstContents);

			/* set menu timer, for TV products */
			BUS_SetTimer(MENU_TIMEOUT_ID, MENU_TIMEOUT);

			#endif
			break;

		case eMEVT_BUS_TIMER:
			#if 0 // not use for octo2.0

			if (p1 == MENU_TIMEOUT_ID)
			{
				/* reset Title depth */
				Settings_ClearWindowTitle();
				#if defined(CONFIG_MW_SYS_7SEG) && defined(CONFIG_OP_NTVPLUS)
					pstContents->bDraw7Seg = TRUE;
				#endif
				BUS_MGR_Destroy(0);
			}
			#endif
			return MESSAGE_BREAK;

		case eSEVT_CAS_SC_REMOVED:
			ApCasVaStatusListPrint(DBG_PRINT, (" SC Event : eSEVT_CAS_SC_REMOVED!\n"));
			BUS_MGR_Destroy(NULL);
			#if 0 // not use for octo2.0
			BUS_MGR_Create((char*)"AP_System_Proc", APP_DEFAULT_PRIORITY, (BUS_Callback_t)AP_System_Proc, 0, 0, 0, 0);
			#endif
			return MESSAGE_BREAK;

		case eMEVT_PG_PIN_CHECK_OK:
			ApCasVaStatusListPrint(DBG_PRINT, ("eMEVT_PG_PIN_CHECK_OK!\n"));
			BUS_MGR_Destroy(local_cas_va_UnlockConsultationProc);
			local_va_statuslist_LoadItem_RightsData(pstContents, s_uiSelectOpListIdx);
			return MESSAGE_BREAK;

		case eMEVT_PG_PIN_CHECK_DENIED:
			ApCasVaStatusListPrint(DBG_PRINT, ("eMEVT_PG_PIN_CHECK_DENIED!\n"));
			BUS_MGR_Destroy(local_cas_va_UnlockConsultationProc);
			BUS_MGR_Destroy(xmgr_cas_VaStatusListProc);

#if defined(CONFIG_OP_NORDIG_BOXER)
			xmgr_cas_VaAdaptShowBoxerScLockMsg();
#else
			xmgr_cas_VaShowMessageBoxByLoc(STR_MESG_1141_ID);
#endif
		#if 0 // not use for octo2.0
			BUS_MGR_Create((char*)"AP_System_Proc", APP_DEFAULT_PRIORITY, (BUS_Callback_t)AP_System_Proc, 0, 0, 0, 0);
		#endif
			return MESSAGE_BREAK;

		case eMEVT_BUS_DESTROY:
			{
#if defined(CONFIG_MW_SYS_7SEG) && defined(CONFIG_OP_NTVPLUS)
				HBOOL bDraw7Seg = pstContents->bDraw7Seg;
#endif
				/* destroy UI */
				local_va_statuslist_FreeItemData(pstContents);
				local_va_statuslist_ResetLists(pstContents);
				local_va_statuslist_DestroyUI(pstContents);

				if(s_stOpList.pOpList)
				{
					OxAP_Free(s_stOpList.pOpList);
					s_stOpList.pOpList = NULL;
				}

/* kill timer */
#if 0 // hjlee3
				BUS_KillTimer(MENU_TIMEOUT_ID);
#endif

#if defined(CONFIG_MW_SYS_7SEG) && defined(CONFIG_OP_NTVPLUS)
				if (bDraw7Seg == TRUE)
				{
					/* clear front PANEL */
					//AP_SPEC_ClearMenuPanelString();
				}
#else
				/* clear front PANEL */
				SVC_SYS_ClearPanelString(ePANEL_STR_PRIORITY_MENU);
#endif
			}
			break;

		default:
			break;

#if 0 // not use for octo2.0
		case MSG_DLG_INPUTED_KEYDOWN:
			BUS_ResetTimer (MENU_TIMEOUT_ID);
			return MESSAGE_BREAK;

		case MSG_GWM_LIST_CHANGED:
			GWM_APP_InvalidateObject (CAS_VA_STATUSLIST_LIST_ID);
			return MESSAGE_BREAK;

		case MSG_GWM_LIST_SELECTED:
			//local_va_statuslist_MsgGwmListSelected (pstContents, p2);
			return MESSAGE_BREAK;

		case MSG_GWM_KEYUP:
			AP_KeyConv_UiCommon(&p1);
			break;

		case MSG_GWM_KEYDOWN:
			AP_KeyConv_UiCommon(&p1);
			BUS_ResetTimer(MENU_TIMEOUT_ID);

			eBusResult = Settings_CheckKeyPass (p1, BUS_MGR_GetAppCallback(BUS_MGR_GetThis()));
			if (eBusResult != ERR_BUS_NO_ERROR)
			{
				return eBusResult;
			}

			switch (p1)
			{
				case KEY_EXIT:
				case KEY_MENU:
					/* reset Title depth */
					Settings_ClearWindowTitle();
#if defined(CONFIG_MW_SYS_7SEG) && defined(CONFIG_OP_NTVPLUS)
					pstContents->bDraw7Seg = TRUE;
#endif
					BUS_MGR_Destroy(NULL);
					return MESSAGE_BREAK;

				//case KEY_OK:		/* ¥‹º¯»˜ »≠∏È¿ª ∫∏¥¬ ∞Õ¿Ãπ«∑Œ */
				case KEY_BACK:
					BUS_MGR_Destroy(NULL);
					BUS_MGR_Create("xmgr_cas_VaSmartCardMenuProc", APP_DEFAULT_PRIORITY, (BUS_Callback_t)xmgr_cas_VaSmartCardMenuProc, 0, 0, 0, 0);
					return MESSAGE_BREAK;

				case KEY_ARROWLEFT:
					{
						if (pstContents->lCasVaCAItemID == eCasVAMenuItem_Rights &&
							( pstContents->eRightsType == eRightsPrebookedList || pstContents->eRightsType == eRightsThemeList ))
						{
							HINT32	lCurIdx = 0, lNumOfList = 0;
							if (pstContents->lCasVaCAItemID == eCasVAMenuItem_Rights)
							{
								lCurIdx = GWM_List_GetCurrentIndex (CAS_VA_STATUSLIST_LIST_ID);
								local_va_statuslist_MsgGwmListSelected (pstContents, lCurIdx, FALSE);
							}
							return MESSAGE_BREAK;
						}
						else
						{
							/* to upper */
							BUS_MGR_Destroy(NULL);
							BUS_MGR_Create("xmgr_cas_VaSmartCardMenuProc", APP_DEFAULT_PRIORITY, (BUS_Callback_t)xmgr_cas_VaSmartCardMenuProc, 0, 0, 0, 0);
							return MESSAGE_BREAK;
						}
					}

				case KEY_OK:
				case KEY_ARROWRIGHT:
					{
						if(p1 == KEY_OK && pstContents->lCasVaCAItemID == eCasVAMenuItem_Rights)
						{
							if((pstContents->eRightsType == eRightsOrigin) && (s_stOpList.uiOpNum > 0))
							{
								s_uiSelectOpListIdx = GWM_List_GetCurrentIndex (CAS_VA_STATUSLIST_LIST_ID);
								s_uiSelectOpListIdx += 1;

								ApCasVaStatusListPrint(DBG_PRINT, ("select OP Index (%d) / [%s / %d]!\n",
									s_uiSelectOpListIdx, s_stOpList.pOpList[s_uiSelectOpListIdx].szLable, s_stOpList.pOpList[s_uiSelectOpListIdx].ulSoid));
							}
							else
							{
								return MESSAGE_BREAK;
							}
						}

						if (pstContents->lCasVaCAItemID == eCasVAMenuItem_Rights)
						{
							if(p1 == KEY_ARROWRIGHT && pstContents->eRightsType == eRightsOrigin)
							{
								return MESSAGE_BREAK;
							}

							lCurIdx = GWM_List_GetCurrentIndex (CAS_VA_STATUSLIST_LIST_ID);
							local_va_statuslist_MsgGwmListSelected (pstContents, lCurIdx, TRUE);
						}

						return MESSAGE_BREAK;
					}
					break;

#if defined(CONFIG_OP_NORDIG_BOXER)
				// STB info ø°º≠¥¬ arrow up/down¿ª page up/down¿∏∑Œ mapping
				case KEY_ARROWUP:
				case KEY_ARROWDOWN:
					if (pstContents->lCasVaCAItemID != eCasVAMenuItem_Stb_Info)
					{
						break;
					}
					/* fall through */
				case KEY_CH_PLUS:
				case KEY_CH_MINUS:
					// «ˆ¿Á focus∞° list obj¿Ã∞Ì page∞° 1∆‰¿Ã¡ˆ ¿ÃªÛ¿Ã∂Û∏È page up/down¿∏∑Œ ∫Ø∞Ê
					// æ∆¥œ∂Û∏È not available
					if (GUIOBJ_LIST == GWM_Obj_GetObjectType(GWM_GetFocusedObjectID()))
					{
						HUINT32 ulListTotalItemNum, ulListNumOfPage;

						GWM_List_GetTotalItem(GWM_GetFocusedObjectID(), &ulListTotalItemNum);
						ulListNumOfPage = GWM_List_GetNumOfPage(GWM_GetFocusedObjectID());

						if (ulListNumOfPage < ulListTotalItemNum)	// 1page ¿ÃªÛ¿Ã∏È page∑Œ ∫Ø»Ø
						{
							if ((p1 == KEY_CH_PLUS) || (p1 == KEY_ARROWUP))
							{
								p1 = KEY_PAGEUP;
							}
							else
							{
								p1 = KEY_PAGEDOWN;
							}
							break;
						}
					}
					/* launch NOT Available application */
					AP_NotAvailable_Show();
					return MESSAGE_BREAK;
#endif

				default:
					break;
			}
			break;


#endif
	}
	return BUS_ProcessMessageDefault(message, hAction, p1, p2, p3);
}


// End of file
