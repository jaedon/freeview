
/**
	@file     ap_cas_va_menu.c
	@brief    file_name & simple comment.

	Description: for VIACCESS MENU UI \n
	Module: Application(Layer), Menu(module), VIACCESS_CX(module) \n
	Remarks : \n
	 Date			Modification				Name\n
	-----------	-------------------------------------	------------------\n
	 2009/07/22		modified					jhlee\n

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

//#include <gwm.h>
//#include <menuui_coor.h>
//#include <settings.h>
//#include <ap_menu_contents.h>
//#include <app_manager_cas.h>// TODO: hjlee3, just for build
#include <mgr_pg.h>
#include <mgr_cas.h>

#include "../../local_include/_xmgr_cas_va.h"
#include "../../local_include/_xmgr_cas_va_util.h"
#include "../../local_include/_xmgr_cas_va_ui.h"
#include <xmgr_cas_res_str.h>
#include "_xmgr_cas_va_menu_coordinate.h"
#include "va_ui_adapt.h"

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

#if defined(CONFIG_DEBUG)
HUINT32	g_CasVaSc_Level 	= DBG_ASSERT;
HUINT32	g_CasVaSc_DbgColor 	= 0;
#endif


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#if defined(CONFIG_DEBUG)
#define CasVaScPrint(level, message)	(((level) & g_CasVaSc_Level) ? (VK_Print("[01;%dm", g_CasVaSc_DbgColor), VK_Print("[App:Viaccess Menu status]"), VK_Print message, VK_Print("[00m")) : 0)
#define CasVaScAssert(c) 				((c) ? 0 : (VK_Print("\nViaccess Menu assertion on %s: line%d\n\n", __FILE__, __LINE__)))
#define CasVaScData(level, message)		(((level) & g_CasVaSc_Level) ? (VK_Print message) : 0)
#else
#define CasVaScPrint(level, message)
#define CasVaScAssert(c)
#define CasVaScData(level, message)
#endif

/*******************************************************************/
/********************      Struture        *************************/
/*******************************************************************/
typedef struct VaMenuItem
{
	CasViaccessCAMenu_Item_t	mainBtnId;
	HBOOL					bItemDisable;
	BUS_Callback_t			pfnUiCallback;
	HUINT8					*pszString;
}VaMenuItem_t;

typedef struct VaMenuContext {

	HUINT8				szTitle[MGR_CAS_STR_LENGTH_LONG];
	HUINT8				szSubTitle[MGR_CAS_STR_LENGTH_LONG];
	CasViaccessCAMenu_Item_t	eActiveItem;
	HUINT32 			ulTotalNum;
	VaMenuItem_t		astListItem[eCasVAMenuItem_Max];
} VaMenuContext_t;

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

STATIC eCasVA_PinChangeStep_InDlg	s_VaPinChangeStep_InServiceMenu;
STATIC VaMenuContext_t		s_CasVaCAMenuContent;
/* TODO:
   ui pincode Dlg  æÓƒ… ¥Î√§«“∞Õ¿Œ∞° ??? hjlee3
   AP_PinDialogInstance_t --> HUINT32 */
//STATIC AP_PinDialogInstance_t 		s_PinCodeInstance;
STATIC HUINT32 	s_PinCodeInstance;
/*******************************************************************/
/********************      	Static Prototype	********************/
/*******************************************************************/

STATIC BUS_Result_t					local_va_camenu_MsgGwmClickedChgPincode (VaMenuContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
STATIC BUS_Result_t					local_va_camenu_PinCodeProc (HINT32 message, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
STATIC BUS_Result_t					local_va_camenu_MsgGwmPinDenied (VaMenuContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
STATIC BUS_Result_t					local_va_camenu_MsgGwmPinOK (VaMenuContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
STATIC HERROR						local_va_camenu_DrawDefaultUI (VaMenuContext_t *pstContents);
STATIC HERROR						local_va_camenu_InitItem_Rights (VaMenuContext_t *pstContents);
STATIC HERROR						local_va_camenu_InitItem_OperatorMSG (VaMenuContext_t *pstContents);
STATIC HERROR						local_va_camenu_InitItem_StbInfo (VaMenuContext_t *pstContents);
STATIC HERROR						local_va_camenu_InitItem_SmartcardInfo (VaMenuContext_t *pstContents);
STATIC HERROR						local_va_camenu_InitItem_ChangePin (VaMenuContext_t *pstContents);
STATIC HERROR						local_va_camenu_InitItem_MaturityRating (VaMenuContext_t *pstContents);
STATIC HERROR						local_va_camenu_InitCAMenuItems(VaMenuContext_t *pstContents);
STATIC HERROR						local_va_camenu_DestroyUI (VaMenuContext_t *pstContents);
STATIC inline VaMenuContext_t	*local_va_camenu_GetContents(void);
STATIC inline HINT32				local_va_camenu_GetDefaultFocus(void);
STATIC HINT32						local_va_camenu_ConvertObjIdToItemIdx(HINT32 nObjId);
STATIC HINT32						local_va_camenu_ConvertItemIdxToObjId(HINT32 nItemId);

/*******************************************************************/
/********************      	Static Function 	********************/
/*******************************************************************/
STATIC HINT32	CasVaStbInfo_StrId[eCasVa_StbInfoItem_Max] =
{
					STR_VIACCESS_MANUAFACTURER_IDENTIFIER_ID,
					STR_VIACCESS_MODEL_IDENTIFIER_ID,
					STR_TERMINAL_SERIAL_NUMBER_ID,
					STR_HARDWARE_VERSION_ID,
					STR_BOOT_LOADER_VERSION_ID,
					STR_SOFTWARE_VERSION_ID,
					STR_STB_IDENTIFIER_ID,
#ifdef CONFIG_MW_CAS_VIACCESS_PVR
					STR_MESG_1096_ID,
#endif
					STR_UA_ID,
					STR_MESG_1097_ID,
#ifdef CONFIG_MW_CAS_VIACCESS_PURPLE_BOX
					STR_TERMINAL_IDENTIFIER_ID,
#endif
					STR_CAMLOCK_VALUE_ID,
};



#define _________________UI__________________________________

STATIC HERROR		local_va_camenu_DestroyUI(VaMenuContext_t *pstContents)
{
#if 0 // not use for octo2.0
	/* if dynamic allocated data are, free */
	AP_NaviCntxt_FreeMainItemArray(pstContents);
#endif
	return ERR_OK;
}

STATIC inline VaMenuContext_t	*local_va_camenu_GetContents(void)
{
	return &s_CasVaCAMenuContent;
}

STATIC inline HINT32	local_va_camenu_GetDefaultFocus(void)
{
	return	eCasVAMenuItem_Rights;
}
#if 0 // not use for octo2.0
STATIC HINT32	local_va_camenu_ConvertObjIdToItemIdx(HINT32 nObjId)
{
	HINT32	nItemId = 0;

	nItemId = (nObjId & ~eSetting_CasVaSCInfo);

	if ((nItemId < 0) || (nItemId >= eCasVAMenuItem_Max))
	{
		return -1;
	}

	return nItemId;
}

STATIC HINT32	local_va_camenu_ConvertItemIdxToObjId(HINT32 nItemId)
{
	if ((nItemId < 0) || (nItemId >= eCasVAMenuItem_Max))
	{
		return 0;
	}

	return (eSetting_CasVaSCInfo | nItemId);
}
#endif
STATIC BUS_Result_t	local_va_camenu_MsgGwmClickedChgPincode(VaMenuContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
#if 0 // not use for octo2.0
	AP_CAS_VA_PARAM_WARRING_REMOVE(pstContents, p1, p2, p3);

	if (p1 == local_va_camenu_ConvertItemIdxToObjId(eCasVAMenuItem_Change_Pin))
	{
		AP_PinDialog_SetDialog(&s_PinCodeInstance,
			BUS_MGR_GetAppCallback(BUS_MGR_GetThis ()), (DLG_TYPE_TITLE | DLG_TIMEOUT_NOTIMEOUT),
			(HUINT8 *)AP_CasResStr_GetStrRsc(LOC_RESPONSE_MSG_PINCODE_ID), NULL);

		// menuø° ¿««— ¡¯¿‘ -> «ˆ¿Á pin ¿‘∑¬ ªÛ≈¬∑Œ Ω√¿€
		s_VaPinChangeStep_InServiceMenu = eVA_PinChangeStep_InDlg_InputCurPincode;
		BUS_MGR_Create("local_va_camenu_PinCodeProc", APP_DIALOGBOX_PRIORITY, local_va_camenu_PinCodeProc, 0, s_VaPinChangeStep_InServiceMenu, 0, 0);
	}
#endif
	return ERR_BUS_NO_ERROR;
}

STATIC BUS_Result_t	local_va_camenu_MsgGwmPinOK(VaMenuContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
#if 0 // not use for octo2.0
	AP_CAS_VA_PARAM_WARRING_REMOVE(pstContents, p1, p2, p3);

	// next step¿∏∑Œ ¡¯«‡
	s_VaPinChangeStep_InServiceMenu = s_VaPinChangeStep_InServiceMenu + 1;

	if (s_VaPinChangeStep_InServiceMenu < eVA_PinChangeStep_InDlg_MAX)
	{
		BUS_MGR_Create("local_va_camenu_PinCodeProc", APP_DIALOGBOX_PRIORITY, local_va_camenu_PinCodeProc,
			0, s_VaPinChangeStep_InServiceMenu, 0, 0);
	}
	else
	{	// ∏µŒ ¡¯«‡µ» ªÛ≈¬ (øœ∑·µ«æ˙¥Ÿ¥¬ confirm popup ∂ÁøÏ±‚ ø©±‚º≠ « ø‰? ∫∏∑˘)
		s_VaPinChangeStep_InServiceMenu = eVA_PinChangeStep_InDlg_InputCurPincode;
	}
#endif
	return ERR_BUS_NO_ERROR;
}

STATIC BUS_Result_t	local_va_camenu_MsgGwmPinDenied(VaMenuContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
#if 0 // not use for octo2.0
	HINT32				nObjId;

	AP_CAS_VA_PARAM_WARRING_REMOVE(pstContents, p1, p2, p3);

	if (p1 != eVA_PinChangeError_Normal)
	{
		// get object id
		nObjId = GWM_Obj_GetObjectID(GWM_Obj_GetFocusedObject());
		local_va_camenu_MsgGwmClickedChgPincode(pstContents, nObjId, p2, p3);
	}
#endif
	return ERR_BUS_NO_ERROR;
}

STATIC BUS_Result_t	local_va_camenu_PinCodeProc(HINT32 message, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
#if 0 // TODO ≥™¡ﬂø° « ø‰ «—¡ˆ ªÏ∆Ï∫∏¿⁄ hjlee3
	return AP_CAS_VA_PinDialog_Proc(&s_PinCodeInstance, message, handle, p1, p2, p3);
#endif
}
#if 0 // not use for octo2.0
STATIC HERROR		local_va_camenu_DrawDefaultUI(VaMenuContext_t *pstContents)
{

	BUS_Result_t	resError = ERR_FAIL;

	/* set, draw background */
	Settings_DrawBack(FALSE);

	/* title */
	MWC_UTIL_DvbStrcpy (pstContents->title, AP_CasResStr_GetStrRsc(LOC_CONDITIONAL_ACCESS_TITLE_ID));
	Settings_SetWindowTitle (SETTING_GET_AP_DEPTH(eSetting_CasVaSCInfo), LOC_CONDITIONAL_ACCESS_TITLE_ID, pstContents->title);

	/* init mainItems */
	Settings_InitNaviMainItems (pstContents->mainItems, pstContents->ulNumOfMainItem, (GWM_ObjDraw_t)AP_LNF_DrawNaviMainItemMethod);

	/* init sub items */
	Settings_InitNaviSubItemList (pstContents->mainItems, pstContents->nCurFocus, TRUE);

	/* set focus */
	resError = GWM_Obj_SetFocus (pstContents->mainItems[pstContents->nCurFocus].mainBtnId);
	if (resError != ERR_BUS_NO_ERROR)
	{
		/*
			±‚æÔ«ﬂ¥¯ focus∞° ¡∂∞«¿« ¿««ÿ ¿ÃπÃ disableµ«æÓ ¿÷¥¬ ∞ÊøÏ ø¿µø¿€«—¥Ÿ.
			¿˝¥Î ¡∂∞«ø° ¿««ÿ disable µ«¡ˆ æ ¥¬ æ∆¿Ã≈€¿ª default∑Œ ∞Ò∂Ûæﬂ «—¥Ÿ.
			∏∏æ‡ √÷º“ «œ≥™¿« æ∆¿Ã≈€µµ enableµ» ≥‡ºÆ¿Ã æ¯¥Ÿ∏È æ∆øπ ¿≠ depthø°º≠ ∏¯µÈæÓø¿∞‘ ∏∑æ∆æﬂ «—¥Ÿ.
		*/
		pstContents->nCurFocus = local_va_camenu_GetDefaultFocus();

		/* init sub items */
		Settings_InitNaviSubItemList(pstContents->mainItems, pstContents->nCurFocus, TRUE);

		/* re-set focus */
		GWM_Obj_SetFocus(pstContents->mainItems[pstContents->nCurFocus].mainBtnId);
	}
	return ERR_OK;
}
#endif
STATIC HERROR	local_va_camenu_InitItem_Rights (VaMenuContext_t *pstContents)
{
	VaMenuItem_t	*pstItem;
	pstItem = &pstContents->astListItem[eCasVAMenuItem_Rights];

	// ∏µÁ String ∞¸∏Æ¥¬ Webø°º≠ √≥∏Æ«œ¥¬ ∞Õ¿∏∑Œ æ‡º”µ ø° µ˚∂Û AP_CasResStr_GetStrRsc() «‘ºˆ∏¶ ¥ı¿ÃªÛ ªÁøÎ«œ¡ˆ ∏ªæ∆æﬂ «‘.
	// µ˚∂Ûº≠ LOC ID∏¶ Web¿∏∑Œ ¿¸º€ »ƒ Webø°º≠ ±◊ø° ∏¬¥¬ string¿ª √£æ∆ √‚∑¬«œµµ∑œ «‘.
	todo
	pstItem->pszString = AP_CasResStr_GetStrRsc(STR_RIGHTS_CONSULTATION_ID);

	pstItem->mainBtnId = eCasVAMenuItem_Rights;
	pstItem->bItemDisable = FALSE;
	pstItem->pfnUiCallback = xmgr_cas_VaStatusListProc;


#if 0 // not use for octo2.0
	SettingsNaviMainItem_t		stMainItem = {0, };

	AP_NaviCntxt_InitNaviMainItem (&stMainItem);

	/* Set the object ID */
	stMainItem.mainBtnId = local_va_camenu_ConvertItemIdxToObjId (eCasVAMenuItem_Rights);

	/* set title, Loc Id */
	stMainItem.nTitleLocId = STR_RIGHTS_CONSULTATION_ID;
	MWC_UTIL_DvbStrcpy(stMainItem.title, AP_CasResStr_GetStrRsc(STR_RIGHTS_CONSULTATION_ID));
	MWC_UTIL_DvbStrcpy(stMainItem.appName, (HUINT8*)"AP_CAS_VA_Rights_Proc");

	stMainItem.appToLaunch = xmgr_cas_VaStatusListProc;

	/* used appParam3 parameter, because devied Item id for used one app proc .*/
	stMainItem.appParam3 = eCasVAMenuItem_Rights;

	/* Register the main item to the contents */
	AP_NaviCntxt_AppendMainItem (pstContents, &stMainItem);
#endif
	return ERR_OK;
}

STATIC HERROR	local_va_camenu_InitItem_OperatorMSG (VaMenuContext_t *pstContents)
{
	VaMenuItem_t	*pstItem;
	pstItem = &pstContents->astListItem[eCasVAMenuItem_Operator_Msg];

	// ∏µÁ String ∞¸∏Æ¥¬ Webø°º≠ √≥∏Æ«œ¥¬ ∞Õ¿∏∑Œ æ‡º”µ ø° µ˚∂Û AP_CasResStr_GetStrRsc() «‘ºˆ∏¶ ¥ı¿ÃªÛ ªÁøÎ«œ¡ˆ ∏ªæ∆æﬂ «‘.
	// µ˚∂Ûº≠ LOC ID∏¶ Web¿∏∑Œ ¿¸º€ »ƒ Webø°º≠ ±◊ø° ∏¬¥¬ string¿ª √£æ∆ √‚∑¬«œµµ∑œ «‘.
	todo
	pstItem->pszString = AP_CasResStr_GetStrRsc(STR_OPERATOR_MESSAGE_CONSULTATION_ID);

	pstItem->mainBtnId = eCasVAMenuItem_Operator_Msg;
	pstItem->bItemDisable = FALSE;
	pstItem->pfnUiCallback = xmgr_cas_VaStatusListProc;


#if 0 // not use for octo2.0
	SettingsNaviMainItem_t	stMainItem = {0, };

	AP_NaviCntxt_InitNaviMainItem (&stMainItem);

	/* Set the object ID */
	stMainItem.mainBtnId = local_va_camenu_ConvertItemIdxToObjId(eCasVAMenuItem_Operator_Msg);

	/* set title, Loc Id */

	stMainItem.nTitleLocId = STR_OPERATOR_MESSAGE_CONSULTATION_ID;
	MWC_UTIL_DvbStrcpy(stMainItem.title, AP_CasResStr_GetStrRsc(STR_OPERATOR_MESSAGE_CONSULTATION_ID));
	MWC_UTIL_DvbStrcpy (stMainItem.appName, (HUINT8*)"AP_CAS_VA_OperatorMsg_Proc");

	stMainItem.appToLaunch = xmgr_cas_VaStatusListProc;

	/* used appParam3 parameter, because devied Item id for used one app proc .*/
	stMainItem.appParam3 = eCasVAMenuItem_Operator_Msg;

	/* Register the main item to the contents */
	AP_NaviCntxt_AppendMainItem (pstContents, &stMainItem);
#endif
	return ERR_OK;
}


STATIC HERROR	local_va_camenu_InitItem_StbInfo(VaMenuContext_t *pstContents)
{
	VaMenuItem_t	*pstItem;
	pstItem = &pstContents->astListItem[eCasVAMenuItem_Stb_Info];

	// ∏µÁ String ∞¸∏Æ¥¬ Webø°º≠ √≥∏Æ«œ¥¬ ∞Õ¿∏∑Œ æ‡º”µ ø° µ˚∂Û AP_CasResStr_GetStrRsc() «‘ºˆ∏¶ ¥ı¿ÃªÛ ªÁøÎ«œ¡ˆ ∏ªæ∆æﬂ «‘.
	// µ˚∂Ûº≠ LOC ID∏¶ Web¿∏∑Œ ¿¸º€ »ƒ Webø°º≠ ±◊ø° ∏¬¥¬ string¿ª √£æ∆ √‚∑¬«œµµ∑œ «‘.
	todo
	pstItem->pszString = AP_CasResStr_GetStrRsc(STR_STB_INFORMATION_ID);

	pstItem->mainBtnId = eCasVAMenuItem_Stb_Info;
	pstItem->bItemDisable = FALSE;
	pstItem->pfnUiCallback = xmgr_cas_VaStatusListProc;
#if 0 // not use for octo2.0
	SettingsNaviMainItem_t		stMainItem = {0, };

	AP_NaviCntxt_InitNaviMainItem (&stMainItem);

	/* Set the object ID */
	stMainItem.mainBtnId = local_va_camenu_ConvertItemIdxToObjId(eCasVAMenuItem_Stb_Info);

	/* set title, Loc Id */
	stMainItem.nTitleLocId = STR_STB_INFORMATION_ID;
	MWC_UTIL_DvbStrcpy (stMainItem.title, AP_CasResStr_GetStrRsc(STR_STB_INFORMATION_ID));
	MWC_UTIL_DvbStrcpy (stMainItem.appName, (HUINT8*)"AP_CAS_VA_StbInfomation_Proc");

	stMainItem.appToLaunch = xmgr_cas_VaStatusListProc;

	/* used appParam3 parameter, because devied Item id for used one app proc .*/
	stMainItem.appParam3 = eCasVAMenuItem_Stb_Info;

	/* Register the main item to the contents */
	AP_NaviCntxt_AppendMainItem (pstContents, &stMainItem);
#endif
	return ERR_OK;
}


STATIC HERROR	local_va_camenu_InitItem_SmartcardInfo (VaMenuContext_t *pstContents)
{
	VaMenuItem_t	*pstItem;
	pstItem = &pstContents->astListItem[eCasVAMenuItem_Smartcard_Info];

	// ∏µÁ String ∞¸∏Æ¥¬ Webø°º≠ √≥∏Æ«œ¥¬ ∞Õ¿∏∑Œ æ‡º”µ ø° µ˚∂Û AP_CasResStr_GetStrRsc() «‘ºˆ∏¶ ¥ı¿ÃªÛ ªÁøÎ«œ¡ˆ ∏ªæ∆æﬂ «‘.
	// µ˚∂Ûº≠ LOC ID∏¶ Web¿∏∑Œ ¿¸º€ »ƒ Webø°º≠ ±◊ø° ∏¬¥¬ string¿ª √£æ∆ √‚∑¬«œµµ∑œ «‘.
	todo
	pstItem->pszString = AP_CasResStr_GetStrRsc(STR_SMARTCARD_INFORMATION_ID);

	pstItem->mainBtnId = eCasVAMenuItem_Smartcard_Info;
	pstItem->bItemDisable = FALSE;
	pstItem->pfnUiCallback = xmgr_cas_VaStatusListProc;

#if 0 // not use for octo2.0
	SettingsNaviMainItem_t		stMainItem = {0, };

	AP_NaviCntxt_InitNaviMainItem (&stMainItem);

	/* Set the object ID */
	stMainItem.mainBtnId = local_va_camenu_ConvertItemIdxToObjId(eCasVAMenuItem_Smartcard_Info);

	/* set title, Loc Id */
	stMainItem.nTitleLocId = STR_SMARTCARD_INFORMATION_ID;
	MWC_UTIL_DvbStrcpy (stMainItem.title, AP_CasResStr_GetStrRsc(STR_SMARTCARD_INFORMATION_ID));
	MWC_UTIL_DvbStrcpy (stMainItem.appName, (HUINT8*)"AP_CAS_VA_SmartCardInfo_Proc");

	stMainItem.appToLaunch = xmgr_cas_VaStatusListProc;

	/* used appParam3 parameter, because devied Item id for used one app proc .*/
	stMainItem.appParam3 = eCasVAMenuItem_Smartcard_Info;

	/* Register the main item to the contents */
	AP_NaviCntxt_AppendMainItem (pstContents, &stMainItem);
#endif
	return ERR_OK;
}

#ifdef CONFIG_MW_CAS_VIACCESS_PVR
STATIC HERROR	local_va_camenu_InitItem_PvrPlaybackInfo (VaMenuContext_t *pstContents)
{
#if 0 // ysmoon_20130812
	SettingsNaviMainItem_t		stMainItem = {0, };

	AP_NaviCntxt_InitNaviMainItem (&stMainItem);

	/* Set the object ID */
	stMainItem.mainBtnId = local_va_camenu_ConvertItemIdxToObjId(eCasVAMenuItem_PvrPlayback_Info);

	/* set title, Loc Id */
	stMainItem.nTitleLocId = LOC_CAS_VA_PLAYBACK_INFO_ID;
	MWC_UTIL_DvbStrcpy (stMainItem.title, AP_CasResStr_GetStrRsc(LOC_CAS_VA_PLAYBACK_INFO_ID));
	MWC_UTIL_DvbStrcpy (stMainItem.appName, (HUINT8*)"AP_CAS_VA_PvrPlaybackInfo_Proc");

	stMainItem.appToLaunch = xmgr_cas_VaStatusListProc;

	/* used appParam3 parameter, because devied Item id for used one app proc .*/
	stMainItem.appParam3 = eCasVAMenuItem_PvrPlayback_Info;

	/* Register the main item to the contents */
	AP_NaviCntxt_AppendMainItem (pstContents, &stMainItem);
#endif

	return ERR_OK;
}
#endif

STATIC HERROR	local_va_camenu_InitItem_ChangePin(VaMenuContext_t *pstContents)
{
	VaMenuItem_t	*pstItem;
	pstItem = &pstContents->astListItem[eCasVAMenuItem_Change_Pin];

	// ∏µÁ String ∞¸∏Æ¥¬ Webø°º≠ √≥∏Æ«œ¥¬ ∞Õ¿∏∑Œ æ‡º”µ ø° µ˚∂Û AP_CasResStr_GetStrRsc() «‘ºˆ∏¶ ¥ı¿ÃªÛ ªÁøÎ«œ¡ˆ ∏ªæ∆æﬂ «‘.
	// µ˚∂Ûº≠ LOC ID∏¶ Web¿∏∑Œ ¿¸º€ »ƒ Webø°º≠ ±◊ø° ∏¬¥¬ string¿ª √£æ∆ √‚∑¬«œµµ∑œ «‘.
	todo
	pstItem->pszString = AP_CasResStr_GetStrRsc(STR_PIN_CODE_MODIFICATION_ID);

	pstItem->mainBtnId = eCasVAMenuItem_Change_Pin;
	pstItem->bItemDisable = FALSE;
	pstItem->pfnUiCallback = NULL;

#if 0 // not use for octo2.0
	SettingsNaviMainItem_t stMainItem = {0, };

	AP_NaviCntxt_InitNaviMainItem (&stMainItem);

	/* Set the object ID */
	stMainItem.mainBtnId = local_va_camenu_ConvertItemIdxToObjId(eCasVAMenuItem_Change_Pin);

	/* set title, Loc Id */
	stMainItem.nTitleLocId = STR_PIN_CODE_MODIFICATION_ID;
	MWC_UTIL_DvbStrcpy (stMainItem.title, AP_CasResStr_GetStrRsc(STR_PIN_CODE_MODIFICATION_ID));
	MWC_UTIL_DvbStrcpy (stMainItem.appName, (HUINT8*)"AP_CAS_VA_ChangeCaPin_Proc");

	stMainItem.appToLaunch = NULL;

	/* Register the main item to the contents */
	AP_NaviCntxt_AppendMainItem (pstContents, &stMainItem);
#endif
	return ERR_OK;
}

STATIC HERROR	local_va_camenu_InitItem_MaturityRating(VaMenuContext_t *pstContents)
{
	VaMenuItem_t	*pstItem;
	pstItem = &pstContents->astListItem[eCasVAMenuItem_Maturity_Rating];

	// ∏µÁ String ∞¸∏Æ¥¬ Webø°º≠ √≥∏Æ«œ¥¬ ∞Õ¿∏∑Œ æ‡º”µ ø° µ˚∂Û AP_CasResStr_GetStrRsc() «‘ºˆ∏¶ ¥ı¿ÃªÛ ªÁøÎ«œ¡ˆ ∏ªæ∆æﬂ «‘.
	// µ˚∂Ûº≠ LOC ID∏¶ Web¿∏∑Œ ¿¸º€ »ƒ Webø°º≠ ±◊ø° ∏¬¥¬ string¿ª √£æ∆ √‚∑¬«œµµ∑œ «‘.
	todo
	pstItem->pszString = AP_CasResStr_GetStrRsc(STR_MATURITY_RATING_MANAGEMENT_ID);

	pstItem->mainBtnId = eCasVAMenuItem_Maturity_Rating;
	pstItem->bItemDisable = FALSE;
	pstItem->pfnUiCallback = xmgr_cas_VaMaturityRatingProc;

#if 0 // not use for octo2.0
	SettingsNaviMainItem_t		stMainItem = {0, };

	AP_NaviCntxt_InitNaviMainItem (&stMainItem);

	/* Set the object ID */
	stMainItem.mainBtnId = local_va_camenu_ConvertItemIdxToObjId(eCasVAMenuItem_Maturity_Rating);

	/* set title, Loc Id */
	stMainItem.nTitleLocId = STR_MATURITY_RATING_MANAGEMENT_ID;
	MWC_UTIL_DvbStrcpy (stMainItem.title, AP_CasResStr_GetStrRsc(STR_MATURITY_RATING_MANAGEMENT_ID));
	MWC_UTIL_DvbStrcpy (stMainItem.appName, (HUINT8*)"xmgr_cas_VaMaturityRatingProc");

	stMainItem.appToLaunch = xmgr_cas_VaMaturityRatingProc;

	/* Register the main item to the contents */
	AP_NaviCntxt_AppendMainItem (pstContents, &stMainItem);
#endif
	return ERR_OK;
}

STATIC HERROR	local_va_camenu_InitCAMenuItems (VaMenuContext_t *pstContents)
{

	HERROR		hError = ERR_FAIL;

	CasVaScPrint(DBG_TRACE, ("local_va_camenu_InitSystemItems() +\n"));

	/* param check */
	if (pstContents == NULL)
	{
		CasVaScAssert(0);
		return	ERR_FAIL;
	}
	#if 0 // not use for octo2.0
	/* alloc mainItems, mainItem has each infos of sub menu to launch */
	hError = AP_NaviCntxt_InitMainItemArray (pstContents);
	if (hError != ERR_OK)
	{
		CasVaScAssert(0);
		return	hError;
	}
	/* settings default focus¥¬ ¡¶¿œ √π æ∆¿Ã≈€ */
	pstContents->nDefaultFocusIdx = local_va_camenu_GetDefaultFocus();
	pstContents->pConvFunc = local_va_camenu_ConvertObjIdToItemIdx;

	#endif

	local_va_camenu_InitItem_Rights (pstContents);
	local_va_camenu_InitItem_OperatorMSG (pstContents);
	local_va_camenu_InitItem_StbInfo (pstContents);
	local_va_camenu_InitItem_SmartcardInfo (pstContents);
#ifdef CONFIG_MW_CAS_VIACCESS_PVR
	local_va_camenu_InitItem_PvrPlaybackInfo (pstContents);
#endif
	local_va_camenu_InitItem_ChangePin (pstContents);
	local_va_camenu_InitItem_MaturityRating (pstContents);

	CasVaScPrint (DBG_TRACE, ("local_va_camenu_InitSystemItems() -\n"));
	return	ERR_OK;
}

STATIC HERROR		local_cas_va_ui_camenu_RequestSessionStop(VaMenuContext_t *pContext)
{
#if 0 // not use new viaccess sw
	OxMgrCasUiEvtReqSessionStop_t stEvent;

	stEvent.ulSessionType = (HUINT32)eMgrCasUiSession_Menu;
	if(MGR_CAS_SendUiEvent (eDxCAS_GROUPID_VA, 0, eMEVT_CASUI_REQ_CLOSE_SESSION, &stEvent) != ERR_OK)
	{
		HxLOG_Error("\n");
	}
#endif
	return ERR_OK;
}

STATIC BUS_Result_t local_cas_va_ui_camenu_MsgInputNotify(VaMenuContext_t *pstContext, HINT32 message, HINT32 p1, HINT32 p2, HINT32 p3)
{
	OxMgrCasInputType_e 	eInputType = (OxMgrCasInputType_e)p2;
	OxMgrCasUiInputNotify_t	*pstCasInputData = (OxMgrCasUiInputNotify_t *)p3;
	BUS_Callback_t			pfnUiCallback;

	//HxLOG_Print("[local_cas_cx_ui_camenu_MsgInputNotify] InputType[%d]", eInputType);
	if(pstContext->eActiveItem == eCasVAMenuItem_Max)
	{
		if(eInputType == eMgrCasInputType_Etc)
		{
			if(pstCasInputData->stEtc.eMgrCasEtcType == eMgrCasEtc_Previous)
			{
				//HxLOG_Print("[local_cas_cx_ui_camenu_MsgInputNotify] : eMgrCasEtc_Previous ");
				local_cas_va_ui_camenu_RequestSessionStop(pstContext);
			}
			else if(pstCasInputData->stEtc.eMgrCasEtcType == eMgrCasEtc_Exit)
			{
				//HxLOG_Print("[local_cas_cx_ui_camenu_MsgInputNotify] : eMgrCasEtc_Exit ");
				local_cas_va_ui_camenu_RequestSessionStop(pstContext);
			}

		}
		else if(eInputType == eMgrCasInputType_Selection)
		{
			HxLOG_Print("[local_cas_va_ui_camenu_MsgInputNotify] [%s] index:[%d]\n", "eMgrCasInputType_Selection", pstCasInputData->stSelection.ulItemIdx);
			if(pstCasInputData->stSelection.ulItemIdx >= eCasVAMenuItem_Max)
				return ERR_BUS_OUT_OF_RANGE;

			pfnUiCallback = pstContext->astListItem[pstCasInputData->stSelection.ulItemIdx].pfnUiCallback;
			if(pfnUiCallback != NULL)
			{
				BUS_MGR_Create(NULL, APP_CAS_PRIORITY, pfnUiCallback, 0, (HINT32)xmgr_cas_VaSmartCardMenuProc/*p1*/, p2, pstContext->eActiveItem);

				pstContext->eActiveItem = pstCasInputData->stSelection.ulItemIdx;
			}
		}
		else if(eInputType == eMgrCasInputType_Number)
		{
			HxLOG_Print("What to do?\n");
		}
	}
	else
	{
		if (BUS_MGR_Get(pstContext->astListItem[pstContext->eActiveItem].pfnUiCallback) != NULL)
		{
			HxLOG_Print("[local_cas_va_ui_camenu_MsgInputNotify] Send Message\n");
			BUS_SendMessage(pstContext->astListItem[pstContext->eActiveItem].pfnUiCallback, message, 0, p1, p2, p3);
		}
	}
	return ERR_BUS_MESSAGE_BREAK;
}
STATIC HERROR local_va_system_SendMenuListData(VaMenuContext_t *pstContext)
{
	HERROR					hErr = ERR_OK;
#if 0 // not use  hjlee3
	VaMenuItem_t			*pstItem;
	OxMgrCasUiEvtMenulist_t	stEvent;
	OxMgrCasUiMenulist_t	*pstMenuList;
	HUINT32					ulStrLen, i;

	HxSTD_memset(&stEvent, 0, sizeof(OxMgrCasUiEvtMenulist_t));
	stEvent.ulUiHandle = (HUINT32)xmgr_cas_VaSmartCardMenuProc;

	pstMenuList = &stEvent.stMenulist;

	pstMenuList->stHeader.ulAttr = 0;
	ulStrLen = SvcCas_UtilStrLen(pstContext->szTitle);

	if(ulStrLen != 0)
	{
		MWC_UTIL_DvbStrncpy(pstMenuList->stHeader.szTitle, pstContext->szTitle, MGR_CAS_STR_LENGTH_LONG);
		pstMenuList->stHeader.ulAttr |= eMgrCasUiAttr_Title;
		HxLOG_Print("Menu Title [%s]\n", pstMenuList->stHeader.szTitle);
	}

	ulStrLen = SvcCas_UtilStrLen(pstContext->szSubTitle);
	if(ulStrLen != 0)
	{
		MWC_UTIL_DvbStrncpy(pstMenuList->stHeader.szSubTitle, pstContext->szSubTitle, MGR_CAS_STR_LENGTH_LONG);
		pstMenuList->stHeader.ulAttr |= eMgrCasUiAttr_Subtitle;
		HxLOG_Print("Sub Title [%s]\n", pstMenuList->stHeader.szSubTitle);
	}

	pstMenuList->stHeader.ulAttr |= eMgrCasUiAttr_SetPosition;
	pstMenuList->stHeader.ulStartIndex = 0;
	if(pstContext->eActiveItem != eCasVAMenuItem_Max)
	{
		pstMenuList->stHeader.ulFocusIndex = pstContext->eActiveItem ;
	}
	else
	{
		pstMenuList->stHeader.ulFocusIndex = 0;
	}

	pstMenuList->ulTotalNum = pstContext->ulTotalNum;

	pstMenuList->astListItem = (OxMgrCasUiMenuItem_t *)OxAP_Malloc(sizeof(OxMgrCasUiMenuItem_t) * (pstMenuList->ulTotalNum));
	if(pstMenuList->astListItem == NULL)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}
	HxSTD_memset(pstMenuList->astListItem, 0, sizeof(OxMgrCasUiMenuItem_t) * (pstMenuList->ulTotalNum));

	for (i = 0; i < pstMenuList->ulTotalNum; i++)
	{
		OxMgrCasUiMenuItem_t *pMenuItem = &pstMenuList->astListItem[i];
		pstItem = &pstContext->astListItem[i];


		//pMenuItem->ulIndex = i;
		pMenuItem->ulIndex = pstItem->mainBtnId;
		pMenuItem->ulAttr = 0;
		pMenuItem->ulType = eMgrCasUiItemType_Str;

		pMenuItem->utData.stString.ulStringLength = SvcCas_UtilStrLen(pstItem->pszString);
		MWC_UTIL_DvbStrncpy(pMenuItem->utData.stString.szString, pstItem->pszString, MGR_CAS_STR_LENGTH_LONG);
		pMenuItem->utData.stString.szString[MGR_CAS_STR_LENGTH_LONG - 1] = '\0';
		HxLOG_Print("Item [%d][%s]\n", i, pMenuItem->utData.stString.szString);
	}

	hErr = MGR_CAS_SendUiEvent (eDxCAS_GROUPID_VA, 0/*ulSlotId*/, EVT_CASUI_MENULIST, &stEvent);
	if(hErr != ERR_OK)
	{
		HxLOG_Critical("\n\n");
	}

	if(pstMenuList->astListItem != NULL)
	{
		OxAP_Free(pstMenuList->astListItem);
	}
#endif
	return hErr;
}


BUS_Result_t	xmgr_cas_VaSmartCardMenuProc(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t			busResult = ERR_BUS_NO_ERROR;
	VaMenuContext_t	*pstContents = NULL;
	CAS_VA_SC_State_t		stScState;
#if defined(CONFIG_OP_NORDIG_BOXER)
	HUINT8		*pszStr1 = NULL;
	HUINT32		ulBoxerModelId = 152;	//temp..

	//error code ¡§∏Æ« ø‰..
	const HUINT32	ulBoxerErrorCode_SC_FAIL 				= 2;	// refer eBoxer_ErrCode_02_SC_FAIL
	const HUINT32	ulBoxerErrorCode_SC_NOT_ACCEPTED 	= 7;	// refer eBoxer_ErrCode_07_SC_NOT_ACCEPTED
#endif

	CasVaScPrint(DBG_TRACE, ("Message : (0x%x)\n", message));

	/* get running param */
	pstContents = local_va_camenu_GetContents();

#if defined(CONFIG_OP_NORDIG_BOXER)
	ulBoxerModelId = xmgr_cas_VaAdaptGetBoxerModel_ID();
	pszStr1 = GetTextBuffer(256);
	VK_memset32(pszStr1, 0, 256);
#endif

	switch(message)
	{
		case eMEVT_BUS_CREATE:
			xmgr_cas_VaGetSmartcardState(&stScState);
			if(stScState.nState == VA_UI_SMARTCARD_EXTRACTED)
			{
#if 0
				BUS_SendMessage (xmgr_cas_VaSmartCardMenuProc, MSG_APP_KEYBOARD_KEYDOWN, hAction, KEY_BACK, 0, 0);
#endif
				xmgr_cas_VaShowMessageBoxByLoc(STR_MESG_1135_ID);
				break;
			}
			else if(stScState.nState == VA_UI_SMARTCARD_FAILED)
			{
#if 0
				BUS_SendMessage (xmgr_cas_VaSmartCardMenuProc, MSG_APP_KEYBOARD_KEYDOWN, hAction, KEY_BACK, 0, 0);
#endif
#if defined(CONFIG_OP_NORDIG_BOXER)
				// ∏µÁ String ∞¸∏Æ¥¬ Webø°º≠ √≥∏Æ«œ¥¬ ∞Õ¿∏∑Œ æ‡º”µ ø° µ˚∂Û AP_CasResStr_GetStrRsc() «‘ºˆ∏¶ ¥ı¿ÃªÛ ªÁøÎ«œ¡ˆ ∏ªæ∆æﬂ «‘.
				// µ˚∂Ûº≠ LOC ID∏¶ Web¿∏∑Œ ¿¸º€ »ƒ Webø°º≠ ±◊ø° ∏¬¥¬ string¿ª √£æ∆ √‚∑¬«œµµ∑œ «‘.
				todo
				MWC_UTIL_DvbUtf8Sprintf(pszStr1, "%s\nid : %03d %03d", AP_CasResStr_GetStrRsc(STR_MESG_2009_ID), ulBoxerModelId, ulBoxerErrorCode_SC_FAIL);
				xmgr_cas_VaShowMessageBox(pszStr1);
#else
				xmgr_cas_VaShowMessageBoxByLoc(STR_SMARTCARD_ERROR_01_ID);
#endif
				break;
			}
			else if(stScState.nState == VA_UI_SMARTCARD_NOT_ACCEPTED)
			{
#if 0
				BUS_SendMessage (xmgr_cas_VaSmartCardMenuProc, MSG_APP_KEYBOARD_KEYDOWN, hAction, KEY_BACK, 0, 0);
#endif
#if defined(CONFIG_OP_NORDIG_BOXER)
				// ∏µÁ String ∞¸∏Æ¥¬ Webø°º≠ √≥∏Æ«œ¥¬ ∞Õ¿∏∑Œ æ‡º”µ ø° µ˚∂Û AP_CasResStr_GetStrRsc() «‘ºˆ∏¶ ¥ı¿ÃªÛ ªÁøÎ«œ¡ˆ ∏ªæ∆æﬂ «‘.
				// µ˚∂Ûº≠ LOC ID∏¶ Web¿∏∑Œ ¿¸º€ »ƒ Webø°º≠ ±◊ø° ∏¬¥¬ string¿ª √£æ∆ √‚∑¬«œµµ∑œ «‘.
				todo
				MWC_UTIL_DvbUtf8Sprintf(pszStr1, "%s\nid : %03d %03d", AP_CasResStr_GetStrRsc(STR_MESG_2013_ID), ulBoxerModelId, ulBoxerErrorCode_SC_NOT_ACCEPTED);
				xmgr_cas_VaShowMessageBox(pszStr1);
#else
				xmgr_cas_VaShowMessageBoxByLoc(STR_INVALID_SMARTCARD_01_ID);
#endif
				break;
			}

			/* init window objects */
			local_va_camenu_InitCAMenuItems (pstContents);

			local_va_system_SendMenuListData(pstContents);

			/* draw window */
			//local_va_camenu_DrawDefaultUI (pstContents);

		#if 0 // not use for octo2.0
			/* set menu timer, for TV products */
			BUS_SetTimer (MENU_TIMEOUT_ID, MENU_TIMEOUT);
		#endif

			/* set front pannel */
#if defined(CONFIG_MW_SYS_7SEG) && defined(CONFIG_OP_NTVPLUS)
			//AP_SPEC_DisplayMenuPanelString(pstContents->szTitle, eDxPANEL_ALIGN_CENTER);
#else
			SVC_SYS_DisplayPanelString (ePANEL_STR_PRIORITY_MENU, pstContents->szTitle, eDxPANEL_ALIGN_CENTER);
#endif
			break;

#if 0 // ysmoon_20130812
		case MSG_OAPI_CASUI_SESSION_START:
			break;

		case MSG_OAPI_CASUI_SESSION_STOP:
			break;

		case MSG_OAPI_CASUI_INPUT_NOTIFY:
			busResult = local_cas_va_ui_camenu_MsgInputNotify(pstContents, message, p1, p2, p3);
			if(busResult != ERR_OK)
			{
				return busResult;
			}
			break;
#endif


		/* Pin code change ∞¸∑√ √≥∏Æ */
		case eMEVT_PG_PIN_CHECK_OK:
			busResult = local_va_camenu_MsgGwmPinOK (pstContents, p1, p2, p3);
			break;

		case eMEVT_PG_PIN_CHECK_DENIED:
			if(p1 == eVA_PinChangeError_ScLocked)
			{
#if defined(CONFIG_OP_NORDIG_BOXER)
				xmgr_cas_VaAdaptShowBoxerScLockMsg();
#else
				xmgr_cas_VaShowMessageBoxByLoc(STR_MESG_1141_ID);
#endif
			}
			else
			{
				busResult = local_va_camenu_MsgGwmPinDenied (pstContents, p1, p2, p3);
			}
			break;

		case eSEVT_CAS_SC_REMOVED:
			CasVaScPrint (DBG_PRINT, (" SC Event : eSEVT_CAS_SC_REMOVED!\n"));
#if 0 // TODO :  æÓƒ… ¥Î√º «“ ∞Õ¿Œ∞° ?  hjlee3
			BUS_SendMessage (xmgr_cas_VaSmartCardMenuProc, MSG_APP_KEYBOARD_KEYDOWN, hAction, KEY_BACK, 0, 0);
#endif
			break;

		case eMEVT_BUS_DESTROY:
			{
#if defined(CONFIG_MW_SYS_7SEG) && defined(CONFIG_OP_NTVPLUS)
				HBOOL bDraw7Seg = pstContents->bDraw7Seg;
#endif
				/* Smartcard ∞¸∑√ ø°∑Ø ∏ﬁΩ√¡ˆ √≥∏Æ */
				AP_CAS_VA_HideMessageBox();
				BUS_MGR_Destroy (local_va_camenu_PinCodeProc);

				/* destroy UI */
				local_va_camenu_DestroyUI (pstContents);

#if 0
	TODO : MENU_TIMEOUT_ID æÓƒ… ¥Î√º «“ ∞Õ¿Œ∞° ?  hjlee3
kill timer
				BUS_KillTimer (MENU_TIMEOUT_ID);
#endif

#if defined(CONFIG_MW_SYS_7SEG) && defined(CONFIG_OP_NTVPLUS)
				if (bDraw7Seg == TRUE)
				{
					/* clear front PANEL */
					//AP_SPEC_ClearMenuPanelString();
				}
#else
				/* clear front PANEL */
				SVC_SYS_ClearPanelString (ePANEL_STR_PRIORITY_MENU);
#endif
			}
			break;
#if 0 // not use for octo2.0
		case MSG_GWM_SCROLLRIGHT:
		case MSG_GWM_CLICKED:
			{
				SettingsNaviMainItem_t mainItem;
				if (pstContents->mainItems[pstContents->nCurFocus].appToLaunch != NULL)
				{
					HxSTD_memcpy(&mainItem, &(pstContents->mainItems[pstContents->nCurFocus]), sizeof(SettingsNaviMainItem_t));
					BUS_MGR_Destroy (NULL);
					BUS_MGR_Create((char*)mainItem.appName, APP_DEFAULT_PRIORITY, mainItem.appToLaunch,
							mainItem.hAction, mainItem.appParam1, mainItem.appParam2, mainItem.appParam3);
				}
				else // pincode
				{
					local_va_camenu_MsgGwmClickedChgPincode(pstContents, p1, p2, p3);
				}
			}
			return MESSAGE_BREAK;

		case MSG_GWM_KEYUP:
			AP_KeyConv_UiCommon(&p1);
			break;

		case MSG_GWM_KEYDOWN:
			/* settings¿Ã ¿¸π›¿˚¿∏∑Œ √≥∏Æ«ÿæﬂ «“ ≈∞µÈ */
			AP_KeyConv_UiCommon(&p1);

			GWM_ResetTimer(MENU_TIMEOUT_ID);
			busResult = Settings_CheckKeyPass(p1, BUS_MGR_GetAppCallback(BUS_MGR_GetThis()));
			if (busResult != ERR_OK)
			{
				return	busResult;
			}

			switch (p1)
			{
				case KEY_EXIT:
				case KEY_MENU:
					/* reset Title depth */
					Settings_ClearWindowTitle();

					/* ªÛ¿ß ∏ﬁ¥∫∑Œ ø√∂Û∞• ∂ß focus∏¶ ±‚æÔ«œ¡ˆ æ ¥¬ ∞Õ¿Ã ∆Ì«“ ºˆµµ ¿÷¥Ÿ. */
					Settings_InitFocus(pstContents);
#if defined(CONFIG_MW_SYS_7SEG) && defined(CONFIG_OP_NTVPLUS)
					pstContents->bDraw7Seg = TRUE;
#endif
					BUS_MGR_Destroy(NULL);
					return MESSAGE_BREAK;

				case KEY_ARROWLEFT:
				case KEY_BACK:
					BUS_MGR_Destroy(NULL);

					/* to upper depth */
					BUS_MGR_Create ("AP_System_Proc", APP_DEFAULT_PRIORITY, (GWM_Callback_t)AP_System_Proc, 0, 0, 0, 0);
					return MESSAGE_BREAK;
			}
			break;

		case MSG_GWM_OBJ_FOCUS_CHANGED:
			Settings_Navi_ReflectFocus (pstContents, p1, FALSE, FindFocus_Current);
			return MESSAGE_BREAK;

		case MSG_GWM_SCROLLUP:
			/* wrap around */
			Settings_Navi_ReflectFocus (pstContents, p1, TRUE, FindFocus_Up);
			return MESSAGE_BREAK;

		case MSG_GWM_SCROLLDOWN:
			/* wrap around */
			Settings_Navi_ReflectFocus (pstContents, p1, TRUE, FindFocus_Down);
			return MESSAGE_BREAK;

		case MSG_GWM_TIMER:
			if (p1 == MENU_TIMEOUT_ID)
			{
				/* reset Title depth */
				Settings_ClearWindowTitle();
#if defined(CONFIG_MW_SYS_7SEG) && defined(CONFIG_OP_NTVPLUS)
				pstContents->bDraw7Seg = TRUE;
#endif
				BUS_MGR_Destroy(0);
			}
			return MESSAGE_BREAK;

#endif


		default:
			break;
	}
	CasVaScPrint(DBG_TRACE, ("AP_System_Proc handle(0x%x)\n", hAction));

	return BUS_ProcessMessageDefault (message, hAction, p1, p2, p3);
}

