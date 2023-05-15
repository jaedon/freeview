/**
	@file     ap_cas_va_about.c
	@brief    file_name & simple comment.

	Description: for VIACCESS MENU ABOUT UI \n
	Module: Application(Layer), Menu(module), CAS_VA(module) \n
	Remarks : \n
	 Date			Modification				Name\n
	-----------	-------------------------------------	------------------\n
	 2009/07/23		modified					jhlee\n

	Copyright (c) 2009 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <svc_sys.h>
#include <db_param.h>
#include <osd_gfx.h>
//#include <menuui_coor.h>
//#include <settings.h>
//#include <ap_popup_list.h>
#include <svc_cas.h>

#include <mgr_pg.h>
#include "../../local_include/_xmgr_cas_va_util.h"
#include "../../local_include/_xmgr_cas_va.h"
#include "../../local_include/_xmgr_cas_va_ui.h"

#include "_xmgr_cas_va_maturity_rate_coordinate.h"


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

#if defined(CONFIG_DEBUG)
HUINT32	g_CasVa_Mr_debug_level = DBG_ASSERT; // | DBG_TRACE; /* DBG_TRACE, DBG_PRINT, DBG_ASSERT 만 사용! */
#endif

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#if defined(CONFIG_DEBUG)
#define ApCasVaMrPrint(level, message)	(((level) & g_CasVa_Mr_debug_level) ? ((VK_Print("[AP:CAS_VA:MR][%d][%s] ", __LINE__, __FUNCTION__)), (VK_Print message)) : 0)
#define ApCasVaMrError(level, message)	(((level) & g_CasVa_Mr_debug_level) ? ((VK_Print("[AP:CAS_VA:MR][%d][%s] ERROR, ", __LINE__, __FUNCTION__)), (VK_Print message)) : 0)
#define	ApCasVaMrAssert(c) 				((c) ? 0 : (VK_Print("[AP:CAS_VA:MR][%d][%s] ERROR !!\n", __LINE__, __FUNCTION__)))
#else
#define	ApCasVaMrPrint(level, message)
#define	ApCasVaMrError(level, message)
#define	ApCasVaMrAssert(c)
#endif
#define MAX_ELEMENT_TEXT_LENGTH 128

#define	CAS_VA_MR_DEF_COLOR					0x00000000
#define	MR_INPUT_LENGTH						2

/*******************************************************************/
/********************      Struture        *************************/
/*******************************************************************/

typedef enum {
	eCasVaMrItem_ChangeMR,
	eCasVaMrItem_Max,
} CasVaMrItem_t;
typedef struct tag_VaMATURITY_RATING_CONTENT
{
	HUINT32		ulNumOfSubItem, ulMaxNumOfSubItem;
	HUINT8		ucTitle[MAX_ELEMENT_TEXT_LENGTH];

	Handle_t 	hAction;
	HUINT8		ucCurMrValue;
	HUINT8		ucMaxMrValue;
	HUINT8		ucInputMrValue;
	HINT32		lChgValueIdx;

#if defined(CONFIG_MW_SYS_7SEG) && defined(CONFIG_OP_NTVPLUS)
	HBOOL		bDraw7Seg;
#endif
} VaMaturityRatingContent_t;

/*
typedef struct tag_VaMATURITY_RATING_CONTENT
{
	HUINT32		ulCurFocus;
	HUINT32		ulNumOfSubItem, ulMaxNumOfSubItem;
	SettingsLeafSubItem_t	*subItems;

	HUINT8		ucTitle[MAX_ELEMENT_TEXT_LENGTH];

	Handle_t 	hAction;
	HUINT8		ucCurMrValue;
	HUINT8		ucMaxMrValue;
	HUINT8		ucInputMrValue;
	HINT32		lChgValueIdx;

#if defined(CONFIG_MW_SYS_7SEG) && defined(CONFIG_OP_NTVPLUS)
	HBOOL		bDraw7Seg;
#endif
} VaMaturityRatingContent_t;
*/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC VaMaturityRatingContent_t	s_stCasVaMrContent;
//STATIC AP_PinDialogInstance_t		s_stMrPinCodeInstance; // hjlee3


/*******************************************************************/
/********************      	Static Prototype	********************/
/*******************************************************************/
#if 0 // not use for octo2.0
// GUI
STATIC inline VaMaturityRatingContent_t *local_cas_va_mr_GetLocalContents(void);
STATIC HINT32		local_cas_va_mr_ConvertItemIdxToObjId (HINT32 nItemIdx);
STATIC HINT32		local_cas_va_mr_ConvertObjIdToItemIdx (HINT32 nObjId);

// Draw
STATIC void			local_cas_va_mr_DrawDefaultUI (VaMaturityRatingContent_t *pContents);
STATIC void			local_cas_va_mr_DrawMRInfo(OSD_Screen_t *pOsdScreen, OSD_Area_t *appArea,
							OSD_Rect_t *objRect, GWM_GuiObject_t object, void *arg, int bUiTop);
#endif
// Launch
STATIC BUS_Result_t	local_cas_va_mr_MaturityRateProc(HINT32 message, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);


// Action
STATIC HERROR		local_cas_va_mr_CheckMrChangingAvailable (VaMaturityRatingContent_t *pContents, HINT32 nObjId);
STATIC HERROR		local_cas_va_mr_CheckMrPincode (HUINT8 ucMrValue);
STATIC BUS_Result_t	local_cas_va_mr_InputToChangeMrValue(VaMaturityRatingContent_t *pContents);

// Init
STATIC HERROR		local_cas_va_mr_InitMrItems(VaMaturityRatingContent_t *pContents);
STATIC HERROR		local_cas_va_mr_InitItem_ChgMaturityRate (VaMaturityRatingContent_t *pContents);
STATIC void			local_cas_va_mr_LoadMRLeveltemValue (VaMaturityRatingContent_t *pContents);

// Unknown
STATIC HERROR	local_cas_va_mr_DestroyContents (VaMaturityRatingContent_t *pContents);

/*******************************************************************/
/********************      	Static Function 	********************/
/*******************************************************************/
#define _________________UI__________________________________


STATIC inline VaMaturityRatingContent_t *local_cas_va_mr_GetLocalContents(void)
{
	return	&s_stCasVaMrContent;
}
#if 0 // not use for octo2.0
STATIC HINT32	local_cas_va_mr_ConvertItemIdxToObjId (HINT32 nItemIdx)
{
	// Item ID에 해당하는 Object ID를 생성
	if ((nItemIdx < 0) || (nItemIdx >= eCasVaMrItem_Max))
	{
		return 0;
	}

	return (eSetting_CasVaMaturityRate | nItemIdx);
}

STATIC HINT32	local_cas_va_mr_ConvertObjIdToItemIdx (HINT32 nObjId)
{
	HINT32	nItemIdx = -1;

	// Object ID에 해당 하는 Item ID 생성
	nItemIdx = (nObjId & ~eSetting_CasVaMaturityRate);

	if ((nItemIdx < 0) || (nItemIdx >= eCasVaMrItem_Max))
	{
		ApCasVaMrError(DBG_ASSERT, ("Invalid value (%d)\n", nItemIdx));
		return -1;
	}

	return nItemIdx;
}


#define	________________Custom_Drawing_Func_________________

STATIC void	local_cas_va_mr_DrawDefaultUI(VaMaturityRatingContent_t *pContents)
{
	OSD_Rect_t		rect = {0, };

	/* Background */
	Settings_DrawBack(FALSE);

	/* Title */
	//LOC_CAS_VA_MATURITY_RATING_MANAGEMENT_TITLE_ID
	MWC_UTIL_DvbStrcpy(pContents->ucTitle, GetStrRsc(LOC_CAS_VA_MATURITY_RATING_MGR_ID));
	Settings_SetWindowTitle(SETTING_GET_AP_DEPTH(eSetting_CasVaMaturityRate), LOC_CAS_VA_MATURITY_RATING_MGR_ID, pContents->ucTitle);

	/* Init sub item objects */
	Settings_InitLeafSubItemsInMenuContents((void *)pContents, (GWM_ObjDraw_t)AP_LNF_DrawLeafSubItemMethod);

	/* Maturity Rating Value */
	OSD_GFX_SetRect(&rect, CAS_VA_MR_ITEM01_X, CAS_VA_MR_ITEM01_Y, CAS_VA_MR_ITEM01_WIDTH, CAS_VA_MR_ITEM01_HEIGHT);
	GWM_Rect_Create(CAS_VA_MR_ITEM12_ID, &rect, CAS_VA_MR_DEF_COLOR);
	GWM_APP_SetObjectDrawMethod(CAS_VA_MR_ITEM12_ID, (GWM_ObjDraw_t)local_cas_va_mr_DrawMRInfo);

	GWM_Obj_SetFocus(pContents->subItems[pContents->ulCurFocus].id);
}


STATIC void	local_cas_va_mr_DrawMRInfo(OSD_Screen_t *pOsdScreen, OSD_Area_t *appArea,
							OSD_Rect_t *objRect, GWM_GuiObject_t object, void *arg, int bUiTop)
{
	HUINT8			*pszString = NULL, *szStrText = NULL;
	OSD_Rect_t		rect = {0, };
	VaMaturityRatingContent_t	*pContents = NULL;


	pszString = GetTextBuffer(TEXT_SIZE64);
	szStrText = GetTextBuffer(TEXT_SIZE128);

	AP_CAS_VA_PARAM_UNUSED(pOsdScreen);
	AP_CAS_VA_PARAM_UNUSED(appArea);
	AP_CAS_VA_PARAM_WARRING_REMOVE(objRect, object, arg, bUiTop);

	pContents = local_cas_va_mr_GetLocalContents();

	/* Card Level Info */
	MWC_UTIL_DvbUtf8Sprintf (pszString, (HUINT8*)"%d", pContents->ucMaxMrValue);

	OSD_GFX_SetRect(&rect, CAS_VA_MR_ITEM01_X, CAS_VA_MR_ITEM01_Y, CAS_VA_MR_ITEM01_WIDTH, CAS_VA_MR_ITEM01_HEIGHT);
	MWC_UTIL_DvbUtf8Sprintf(szStrText, (HUINT8*)"%s %s", (HUINT8 *)GetStrRsc(LOC_CAS_VA_MATURITY_MAX_RATING_VALUE_ID), (HUINT8 *)pszString);
	GWM_Text_Create(CAS_VA_MR_ITEM01_ID, &rect, (HUINT8*)szStrText);
	GWM_Text_ChangeColor(CAS_VA_MR_ITEM01_ID, COL(C_T_Cursor), CAS_VA_MR_DEF_COLOR);
	GWM_Text_SetSize(CAS_VA_MR_ITEM01_ID, FONT_SIZE_M);
	GWM_Text_SetAlign(CAS_VA_MR_ITEM01_ID, TEXT_ALIGN_LEFT);
}

#endif

#define	________________Launch_Popups_______________________

STATIC BUS_Result_t	local_cas_va_mr_MaturityRateProc(HINT32 message, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
#if 0 // hjlee3
	return AP_CAS_VA_CheckPinDialog_Proc(&s_stMrPinCodeInstance, message, handle, p1, p2, p3);
#endif
}

#define	________________Action_Func__________________________

STATIC HERROR	local_cas_va_mr_CheckMrChangingAvailable (VaMaturityRatingContent_t *pContents, HINT32 nObjId)
{
//	SettingsLeafSubItem_t	*pstSubItem = NULL;
	HUINT8					*szString = NULL, ch = 0;
	UNIXTIME				ulRsvUnixtime = 0;
	HINT32					nItemIdx = 0;
	HUINT8					ucMrValueModified = 0;
#if 0 // not use for octo2.0
	nItemIdx = local_cas_va_mr_ConvertObjIdToItemIdx (nObjId);

	if (nItemIdx != eCasVaMrItem_ChangeMR)
	{
		return ERR_FAIL;
	}

	pstSubItem = AP_Contents_GetSubitemByObjId ((void*)pContents, nObjId);
	szString = GWM_Input_GetTextFromID (nObjId);
	if (szString == NULL)
	{
		return ERR_FAIL;
	}

	ch = szString[0];	ucMrValueModified = atoi(&ch) * 10;
	ch = szString[1];	ucMrValueModified += atoi(&ch);

	pContents->ucInputMrValue = ucMrValueModified; // save the input value

	if((ucMrValueModified > pContents->ucMaxMrValue) || (ucMrValueModified == pContents->ucCurMrValue))
	{
		return ERR_FAIL;
	}

	/* Changing of maturity rating is available. Check ca pincode */
	local_cas_va_mr_CheckMrPincode (ucMrValueModified);
#endif
}


STATIC BUS_Result_t local_cas_va_mr_InputToChangeMrValue(VaMaturityRatingContent_t *pContents)
{
#if 0 // not use for octo2.0
	SettingsLeafSubItem_t	*pstSubItem = NULL;

	HINT32		lItemIdx;
	HINT32		nObjId = 0;

	nObjId = GWM_GetFocusedObjectID();
	lItemIdx = local_cas_va_mr_ConvertObjIdToItemIdx(nObjId);

	pstSubItem = AP_Contents_GetSubitemByObjId ((void *)pContents, nObjId);
	if (pstSubItem == NULL)
	{
		return ERR_TARGET_NULL;
	}

	snprintf (pstSubItem->itemString, sizeof(pstSubItem->itemString), "%02d", pContents->ucCurMrValue);

	GWM_Obj_Destroy(nObjId);

	GWM_Input_Create(nObjId, &(pstSubItem->rect), pstSubItem->mainItemText, MR_INPUT_LENGTH, pstSubItem->itemString);
	GWM_Input_SetStringMode(nObjId, eGUI_INPUTOBJ_MODE_Normal);
	GWM_APP_SetObjectDrawMethod(nObjId, AP_LNF_DrawLeafInputMethod);
	GWM_Obj_SetFocus(nObjId);
#endif
	return ERR_OK;
}


STATIC HERROR	local_cas_va_mr_CheckMrPincode (HUINT8 ucMrValue)
{
#if 0 // not use for octo2.0
	eCasVa_PinCheckStep_InDlg	ePinCheckStep;
	VaMaturityRatingContent_t	*pContents = NULL;

	pContents = local_cas_va_mr_GetLocalContents();
	if ( pContents == NULL )
	{
		return	ERR_FAIL;
	}

	ePinCheckStep = eVA_PinCheckStep_InDlg_InputPincode;
	s_stMrPinCodeInstance.parentAppCallback = xmgr_cas_VaMaturityRatingProc;
	s_stMrPinCodeInstance.ulDlgAttributes = (eCasVa_PinCheckEnterCA_InDlg)eVA_PinCheckEnter_ModifyMaturityRate;

	BUS_MGR_Create("local_cas_va_mr_MaturityRateProc", APP_DIALOGBOX_PRIORITY, local_cas_va_mr_MaturityRateProc,
				0, (HINT32)ePinCheckStep, (HUINT32)ucMrValue, 0);
#endif
}



#define	________________Init_Func____________________________

STATIC HERROR	local_cas_va_mr_InitMrItems(VaMaturityRatingContent_t *pContents)
{
	HERROR		hError;
#if 0 // not use for octo2.0
	/* param check */
	if (pContents == NULL)
	{
		ApCasVaMrError(DBG_ASSERT, ("Invalid param\n"));
		return	ERR_FAIL;
	}

	/* Get Maturiy Rating Value */
	local_cas_va_mr_LoadMRLeveltemValue(pContents);

	/* Init the sub items */
	hError = AP_Contents_InitSubitemArray ((void *)pContents); // using only one item of ten items.
	if (hError != ERR_OK)
	{
		ApCasVaMrError(DBG_ASSERT, ("AP_Contents_InitSubitemArray() return %x\n", hError));
		return	hError;
	}

	/* Make SubItems */
	local_cas_va_mr_InitItem_ChgMaturityRate(pContents);
#endif
	return	ERR_OK;
}

STATIC void	local_cas_va_mr_LoadMRLeveltemValue (VaMaturityRatingContent_t *pContents)
{
#if 0 // not use for octo2.0
	HINT32 nResult;
	HUINT8 ucCurMr, ucMaxMr;

	AP_CAS_VA_GetMaturityRate(&ucCurMr, &ucMaxMr, &nResult);

	pContents->ucCurMrValue = ucCurMr;
	pContents->ucMaxMrValue = ucMaxMr;
	pContents->ucInputMrValue = 0;
	pContents->lChgValueIdx = 3;
#endif
}

STATIC HERROR	local_cas_va_mr_InitItem_ChgMaturityRate(VaMaturityRatingContent_t *pContents)
{
	HERROR		hError = ERR_FAIL;
#if 0 // not use for octo2.0
	SettingsLeafSubItem_t	subItem = {0, };

	/* Set SubItem ID */
	subItem.id = local_cas_va_mr_ConvertItemIdxToObjId(eCasVaMrItem_ChangeMR);

	/* Copy main text */
	MWC_UTIL_DvbStrcpy(subItem.mainItemText, GetStrRsc(LOC_CAS_VA_MATURITY_RATING_CHANGE_ID));

	subItem.numOfItemTextList = 0;
	subItem.itemTextList = NULL;
	subItem.idxOfTextList = 0;
	subItem.subItemValue = 0;

	MWC_UTIL_DvbUtf8Sprintf((HUINT8*)subItem.subItemText, (const HUINT8*)"%02d", pContents->ucCurMrValue);

	/* Append the subitem to the menu contents. */
	AP_Contents_AppendSubitem ((void *)pContents, &subItem);
#endif
	return ERR_OK;
}


#define	________________Global_Function_______________________

/*******************************************************************/
/********************      	Global Function 	********************/
/*******************************************************************/

BUS_Result_t	xmgr_cas_VaMaturityRatingProc(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR				hError = ERR_FAIL;
	BUS_Result_t		gwmResult = ERR_FAIL;
	VaMaturityRatingContent_t *pContents = NULL;
	HINT32				nObjId = 0;
#if 0 // not use for octo2.0
	pContents = local_cas_va_mr_GetLocalContents();

	switch ( message )
	{
		case eMEVT_BUS_CREATE :
			ApCasVaMrPrint(DBG_TRACE, ("eMEVT_BUS_CREATE!\n"));

			/* Set Area! */
			GWM_APP_SetAppArea (CAS_VA_MR_WINDOW_X, CAS_VA_MR_WINDOW_Y, CAS_VA_MR_WINDOW_WIDTH, CAS_VA_MR_WINDOW_HEIGHT);

			/* Initialiaze Items */
			hError = local_cas_va_mr_InitMrItems(pContents);
			ApCasVaMrAssert(hError == ERR_OK);

			/* Draw UI */
			local_cas_va_mr_DrawDefaultUI(pContents);

			/* set menu timer, for TV products */
			BUS_SetTimer(MENU_TIMEOUT_ID, MENU_TIMEOUT);

			/* set BOX LCD */
#if defined(CONFIG_MW_SYS_7SEG) && defined(CONFIG_OP_NTVPLUS)
			//AP_SPEC_DisplayMenuPanelString(pContents->ucTitle, eDxPANEL_ALIGN_CENTER);
#else
			SVC_SYS_DisplayPanelString (ePANEL_STR_PRIORITY_MENU, pContents->ucTitle, eDxPANEL_ALIGN_CENTER);
#endif
			break;

		/* popup list를 보여 준다 */
		case MSG_GWM_CLICKED:
		case MSG_GWM_SCROLLRIGHT:
			ApCasVaMrPrint(DBG_TRACE, ("MSG_GWM_CLICKED or MSG_GWM_SCROLLRIGHT!\n"));
			local_cas_va_mr_InputToChangeMrValue(pContents);
			return MESSAGE_BREAK;

		/* apply changed value in pop-up list to sub item */
		case MSG_APP_SETTING_VALUE_CHANGED:
			ApCasVaMrPrint(DBG_TRACE, ("MSG_APP_SETTING_VALUE_CHANGED!\n"));
			return MESSAGE_BREAK;

		/* user's input number */
		case MSG_GWM_INPUTCOMPLETE:
			ApCasVaMrPrint(DBG_TRACE, ("MSG_GWM_INPUTCOMPLETE!\n"));

			// Agree MR과 Change MR이 같은 Proc을 사용. Agree MR 창 종료...
			if(BUS_MGR_Get(local_cas_va_mr_MaturityRateProc) != NULL)
				BUS_MGR_Destroy(local_cas_va_mr_MaturityRateProc);

			local_cas_va_mr_CheckMrChangingAvailable(pContents, p1);
			return MESSAGE_BREAK;

		/* MR Change Pin code OK*/
		case eMEVT_PG_PIN_CHECK_OK:
		case eMEVT_PG_PIN_CHECK_DENIED:
			{
				ApCasVaMrPrint(DBG_TRACE, ("eMEVT_PG_PIN_CHECK_OK or eMEVT_PG_PIN_CHECK_DENIED!\n"));

				if(s_stMrPinCodeInstance.ulDlgAttributes == eVA_PinCheckEnter_ModifyMaturityRate)
				{
					HINT32 nLocId;

					BUS_MGR_Destroy(0);
					BUS_MGR_Create((char*)"xmgr_cas_VaSmartCardMenuProc", APP_DEFAULT_PRIORITY, (GWM_Callback_t)xmgr_cas_VaSmartCardMenuProc, 0, 0, 0, 0);

#if defined(CONFIG_OP_NORDIG_BOXER)
					if (message == eMEVT_PG_PIN_CHECK_OK)
					{
						xmgr_cas_VaShowMessageBoxByLoc(LOC_CAS_VA_MSG_MR_CHANGED_ID);
					}
					else
					{
						xmgr_cas_VaAdaptShowBoxerScLockMsg();
					}
#else
					nLocId = (message == eMEVT_PG_PIN_CHECK_OK ? LOC_CAS_VA_MSG_MR_CHANGED_ID : LOC_CAS_VA_ERR_SC_LOCKED_ID);
					xmgr_cas_VaShowMessageBoxByLoc(nLocId);
#endif

					return MESSAGE_BREAK;
				}

				if(s_stMrPinCodeInstance.ulDlgAttributes == eVA_PinCheckEnter_AgreeMaturityRate)
				{
					BUS_MGR_Destroy(0);

					if(message == eMEVT_PG_PIN_CHECK_DENIED)
					{
#if defined(CONFIG_OP_NORDIG_BOXER)
						xmgr_cas_VaAdaptShowBoxerScLockMsg();
#else
						xmgr_cas_VaShowMessageBoxByLoc(LOC_CAS_VA_ERR_SC_LOCKED_ID);
#endif
					}

					return MESSAGE_BREAK;
				}
			}

		case MSG_DLG_INPUTED_KEYDOWN:
			GWM_ResetTimer(MENU_TIMEOUT_ID);
			return MESSAGE_BREAK;

		case MSG_GWM_KEYUP:
			AP_KeyConv_UiCommon(&p1);
			break;

		case MSG_GWM_KEYDOWN:
			ApCasVaMrPrint(DBG_TRACE, ("MSG_GWM_KEYDOWN! - p2(0x%x)\n",p2));
			GWM_ResetTimer(MENU_TIMEOUT_ID);

			AP_KeyConv_UiCommon(&p1);

			gwmResult = Settings_CheckKeyPass(p1, BUS_MGR_GetAppCallback(BUS_MGR_GetThis()));
			if (gwmResult != ERR_BUS_NO_ERROR)
			{
				return	gwmResult;
			}

			switch(p1)
			{
				case KEY_EXIT:
				case KEY_MENU:
					/* reset Title depth */
					Settings_ClearWindowTitle();
#if defined(CONFIG_MW_SYS_7SEG) && defined(CONFIG_OP_NTVPLUS)
					pContents->bDraw7Seg = TRUE;
#endif
					BUS_MGR_Destroy(NULL);
					return MESSAGE_BREAK;

				case KEY_ARROWLEFT:
					nObjId = GWM_GetFocusedObjectID();
					if (GUIOBJ_BUTTON != GWM_Obj_GetObjectType(nObjId))
					{
						break;
					}

				case KEY_BACK:
					BUS_MGR_Destroy(0);
					BUS_MGR_Create((char*)"xmgr_cas_VaSmartCardMenuProc", APP_DEFAULT_PRIORITY, (GWM_Callback_t)xmgr_cas_VaSmartCardMenuProc, 0, 0, 0, 0);
					return MESSAGE_BREAK;
			}
			break;

		case MSG_GWM_SCROLLUP:
			ApCasVaMrPrint(DBG_TRACE, ("MSG_GWM_SCROLLUP!\n"));
			//local_cx_mr_FindNextFocus(pContents, TRUE);
			return MESSAGE_BREAK;

		case MSG_GWM_SCROLLDOWN:
			ApCasVaMrPrint(DBG_TRACE, ("MSG_GWM_SCROLLDOWN!\n"));
			return MESSAGE_BREAK;

		case MSG_GWM_TIMER:
			if (p1 == MENU_TIMEOUT_ID)
			{
				/* reset Title depth */
				Settings_ClearWindowTitle();
#if defined(CONFIG_MW_SYS_7SEG) && defined(CONFIG_OP_NTVPLUS)
				pContents->bDraw7Seg = TRUE;
#endif
				BUS_MGR_Destroy(0);
				return MESSAGE_BREAK;
			}
			break;

		case eSEVT_CAS_SC_REMOVED:
			ApCasVaMrPrint(DBG_PRINT, (" SC Event : eSEVT_CAS_SC_REMOVED!\n"));
			BUS_MGR_Destroy(NULL);
			BUS_MGR_Create((char*)"AP_System_Proc", APP_DEFAULT_PRIORITY, (GWM_Callback_t)AP_System_Proc, 0, 0, 0, 0);
			break;

		case eMEVT_BUS_DESTROY:
			{
#if defined(CONFIG_MW_SYS_7SEG) && defined(CONFIG_OP_NTVPLUS)
				HBOOL bDraw7Seg = pContents->bDraw7Seg;
#endif
				/* kill pincode dialog, if launched */
				BUS_MGR_Destroy(local_cas_va_mr_MaturityRateProc);

				/* setting 값을 저장한다. */
				//local_cx_mr_SetMRLeveltemValue(pContents);

				/* Destroy Contents */
				local_cas_va_mr_DestroyContents(pContents);

				/* kill timer */
				BUS_KillTimer(MENU_TIMEOUT_ID);

#if defined(CONFIG_MW_SYS_7SEG) && defined(CONFIG_OP_NTVPLUS)
				if (bDraw7Seg == TRUE)
				{
					/* clear front PANEL */
					//AP_SPEC_ClearMenuPanelString();
				}
#else

				SVC_SYS_ClearPanelString(ePANEL_STR_PRIORITY_MENU);
#endif
			}
			break;

		default:
			break;
	}
#endif
	return BUS_ProcessMessageDefault(message, hAction, p1, p2, p3);
}


STATIC HERROR	local_cas_va_mr_DestroyContents (VaMaturityRatingContent_t *pContents)
{
#if 0 // not use for octo2.0
	if (pContents == NULL)
	{
		return ERR_FAIL;
	}

	AP_Contents_FreeSubitemArray ((void *)pContents);
	HxSTD_memset (pContents, 0, sizeof(VaMaturityRatingContent_t));
#endif
	return ERR_OK;
}

STATIC HERROR local_cas_va_AgreeMaturityRate(HUINT32 uiAcsId, HUINT32 uiHandle)
{
	eCasVa_PinCheckStep_InDlg	ePinCheckStep;
	VaMaturityRatingContent_t	*pContents = NULL;

	pContents = local_cas_va_mr_GetLocalContents();
	if ( pContents == NULL )
	{
		return	ERR_FAIL;
	}

	ePinCheckStep = eVA_PinCheckStep_InDlg_InputPincode;
#if 0 // hjlee3
	s_stMrPinCodeInstance.parentAppCallback = AP_CAS_VA_MainProc;
	s_stMrPinCodeInstance.ulDlgAttributes = (eCasVa_PinCheckEnterCA_InDlg)eVA_PinCheckEnter_AgreeMaturityRate;
#endif

	BUS_MGR_Create("local_cas_va_mr_MaturityRateProc", APP_CAS_PRIORITY, local_cas_va_mr_MaturityRateProc,
				0, (HINT32)ePinCheckStep, (HINT32)uiAcsId, (HINT32)uiHandle);
	return ERR_OK;
}

HERROR AP_CAS_VA_AgreeMaturityRateDestory()
{
#if 0 // not use for octo2.0
	BUS_MGR_Destroy(local_cas_va_mr_MaturityRateProc);
#endif
	return ERR_OK;
}

HERROR AP_CAS_VA_AgreeMaturityRate(HUINT32 uiAcsId, HUINT32 uiHandle)
{
	// 이미 maturity rating 처리 중이라면 새로온 이벤트는 무시한다. (안그러면 입력하던 pin code 가 지워질것..)
	if(BUS_MGR_Get(local_cas_va_mr_MaturityRateProc) != NULL)
		return ERR_OK;

	return local_cas_va_AgreeMaturityRate(uiAcsId, uiHandle);
}

// End of file
