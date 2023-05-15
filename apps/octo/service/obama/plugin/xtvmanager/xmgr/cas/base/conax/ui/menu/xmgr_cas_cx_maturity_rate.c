/**
	@file     ap_cas_cx_about.c
	@brief    file_name & simple comment.

	Description: for CONAX MENU ABOUT UI \n
	Module: Application(Layer), Menu(module), CAS_CX(module) \n
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
#include <pal_video.h>
#include <svc_sys.h>
#include <db_param.h>
#include <osd_gfx.h>
#include <svc_cas.h>
#include <conax.h>
#include <cx_common.h>

#include "../../local_include/_xmgr_cas_cx.h"
#include "../../local_include/_xmgr_cas_cx_util.h"
#include <xmgr_cas_res_str.h>

#include <app_manager_cas.h>
#include <mgr_pg.h>
#include <mgr_cas.h>

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

STATIC HUINT8	s_szStrText[TEXT_SIZE128];


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

#define	CAS_CX_MR_DEF_COLOR					0x00000000

/*******************************************************************/
/********************      Struture        *************************/
/*******************************************************************/
typedef enum
{
	eCasCxMRItem_ChangeMR,
	eCasCxMRItem_CardLevel,
	eCasCxMRItem_Description,
	eCasCxMRItem_DescGeneral,
	eCasCxMRItem_DescParental,
	eCasCxMRItem_DescOver18,
	eCasCxMRItem_DescErotic,
	eCasCxMRItem_MAX,
} XmgrCas_CxMrItem_e;


#if 1
typedef struct CxMatRateItem
{
	XmgrCas_CxMrItem_e	eItemId;
	HUINT8			szItemValue[MGR_CAS_STR_LENGTH_LONG];
} XmgrCas_CxMrItem_t;

typedef struct tag_MATURITY_RATING_CONTENT
{
	HUINT32					ulTotalNum;
	HUINT8					szTitle[MAX_ELEMENT_TEXT_LENGTH];
	Handle_t 				hAction;
	HUINT8					ucCurMatRatLevel;
	HINT32					lChgValueIdx;
	BUS_Callback_t			pfnCallerCallback;
	XmgrCas_CxMrItem_t		astListItem[eCasCxMRItem_MAX];
} XmgrCas_CxMrContent_t;

#else
typedef struct tag_MATURITY_RATING_CONTENT
{
	HUINT32		ulCurFocus;
	HUINT32		ulNumOfSubItem, ulMaxNumOfSubItem;
	XmgrCas_CxSettingsLeafSubItem_t	*subItems;

	HUINT8		ucTitle[MAX_ELEMENT_TEXT_LENGTH];

	Handle_t 	hAction;
	HUINT8		ucCurMatRatLevel;
	HINT32		lChgValueIdx;
} XmgrCas_CxMrContent_t;
#endif

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC XmgrCas_CxMrContent_t	s_CasCxMRContent;
//STATIC ListDialog_Content_t	  	s_PopupListCasCxMRContent;		/**< Popup Contents. */
STATIC AP_PinDialogInstance_t	s_MrPinCodeInstance;


/*******************************************************************/
/********************      	Static Prototype	********************/
/*******************************************************************/

#if 0 // not use for octo2.0
STATIC HINT32		xmgr_cas_CxMrConvertItemIdxToObjId(HINT32 nItemIdx);
STATIC HINT32		xmgr_cas_CxMrConvertObjIdToItemIdx(HINT32 nObjId);
STATIC HUINT32		xmgr_cas_CxMrConvertItemIdxToMenuItemId(HINT32 nItemIdx);
STATIC void			xmgr_cas_CxMrFindNextFocus(XmgrCas_CxMrContent_t	*pContents, HBOOL bUp);

STATIC HERROR		xmgr_cas_CxMrGetMatutiryRateStr(HUINT8 *pszStr, HUINT8 ucCurMatRatLevel);

STATIC HERROR		xmgr_cas_CxMrDestroyContents (XmgrCas_CxMrContent_t *pContents);

// Draw
STATIC void			xmgr_cas_CxMrDrawDefaultUI(XmgrCas_CxMrContent_t *pContents);
STATIC void			xmgr_cas_CxMrDrawMRInfo(OSD_Screen_t *pOsdScreen, OSD_Area_t *appArea,
							OSD_Rect_t *objRect, GWM_GuiObject_t object, void *arg, int bUiTop);

// Launch
STATIC void			xmgr_cas_CxMrCreatePopupList(XmgrCas_CxMrContent_t *pContents, HINT32 ulObjId);
STATIC BUS_Result_t	xmgr_cas_CxMrDisplayListPopup(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
STATIC HERROR		xmgr_cas_CxMrDestroyListPopup(void);
STATIC inline ListDialog_Content_t *xmgr_cas_CxMrGetPopupListContents(void);
#endif
// GUI
STATIC inline XmgrCas_CxMrContent_t *xmgr_cas_CxMrGetLocalContents(void);

STATIC HUINT8		xmgr_cas_CxMrConvertItemValueIdxToMRValue(HINT32 lItemValueIdx);
STATIC HINT32		xmgr_cas_CxMrConvertMRValueToItemValueIdx(HUINT8	ucMRValue);

STATIC BUS_Result_t	xproc_cas_CxMrPinCode(HINT32 message, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);

// Action
STATIC void			xmgr_cas_CxMrProcessClicked(XmgrCas_CxMrContent_t *pContents, HINT32 nObjId);
STATIC HERROR		xmgr_cas_CxMrUpdateSubItem(XmgrCas_CxMrContent_t *pContents, HINT32 nObjId);
STATIC HERROR		xmgr_cas_CxMrChangedMRLevelValue(XmgrCas_CxMrContent_t *pContents);
STATIC void			xmgr_cas_CxMrChangeSubItemValue(HINT32 nObjId, HINT32 ulCurIdx);
STATIC HERROR		xmgr_cas_CxMrChangedPopupValue(XmgrCas_CxMrContent_t *pContents, HINT32 lCurIdx, HINT32 p2, HINT32 p3);
STATIC HERROR		xmgr_cas_CxMrCheckMRPincode(HUINT8 ucMatRateLevel);

// Init
STATIC HERROR		xmgr_cas_CxMrInitMRItems(XmgrCas_CxMrContent_t *pContents,HINT32 p1);
STATIC HERROR		xmgr_cas_CxMrInitItemChgMaturityRate(XmgrCas_CxMrContent_t *pContents);
STATIC void			xmgr_cas_CxMrLoadMRLeveltemValue(XmgrCas_CxMrContent_t *pContents);

/*******************************************************************/
/********************      	Static Function 	********************/
/*******************************************************************/
#define _________________UI__________________________________

STATIC inline XmgrCas_CxMrContent_t *xmgr_cas_CxMrGetLocalContents(void)
{
	return	&s_CasCxMRContent;
}

// TODO: To remove
#if 0 // not use for octo2.0
STATIC inline ListDialog_Content_t *xmgr_cas_CxMrGetPopupListContents(void)
{
	return	&s_PopupListCasCxMRContent;
}



STATIC HINT32	xmgr_cas_CxMrConvertItemIdxToObjId(HINT32 nItemIdx)
{
	// Item ID에 해당하는 Object ID를 생성
	if ((nItemIdx < 0) || (nItemIdx >= eCasCxMRItem_MAX))
	{
		return 0;
	}

	return (eSetting_CasCxMaturityRate | nItemIdx);
}


STATIC HINT32	xmgr_cas_CxMrConvertObjIdToItemIdx(HINT32 nObjId)
{
	HINT32	nItemIdx = -1;

	// Object ID에 해당 하는 Item ID 생성
	nItemIdx = (nObjId & ~eSetting_CasCxMaturityRate);

	if ((nItemIdx < 0) || (nItemIdx >= eCasCxMRItem_MAX))
	{
		HxLOG_Critical("\n\n");
		return -1;
	}

	return nItemIdx;
}


STATIC HUINT32	xmgr_cas_CxMrConvertItemIdxToMenuItemId(HINT32 nItemIdx)
{
	// Item ID에 해당 하는 Item Value ID
	switch (nItemIdx)
	{
		case eCasCxMRItem_ChangeMR:
			return MENUITEM_ID_CASCX_MATURITY_RATE;

		default:
			HxLOG_Critical("\n\n");
			return (HUINT32)-1;
	}
}
#endif

STATIC HUINT8	xmgr_cas_CxMrConvertItemValueIdxToMRValue(HINT32 lItemValueIdx)
{
	// Item Value 에 해당 하는 CONAX MR Value
	switch (lItemValueIdx)
	{
		case eCasCx_MR_G:
			return	CX_MATURITY_RATE_G;

		case eCasCx_MR_PG:
			return	CX_MATURITY_RATE_PG;

		case eCasCx_MR_A:
			return	CX_MATURITY_RATE_A;

		case eCasCx_MR_X:
			return	CX_MATURITY_RATE_X;

		default:
			HxLOG_Critical("\n\n");
			return	CX_MATURITY_RATE_G;
	}
}


STATIC HINT32	xmgr_cas_CxMrConvertMRValueToItemValueIdx(HUINT8 ucMRValue)
{
	// CONAX MR Value 값에 해당하는 Item Value
	switch (ucMRValue)
	{
		case CX_MATURITY_RATE_G:
			return	eCasCx_MR_G;

		case CX_MATURITY_RATE_PG:
			return	eCasCx_MR_PG;

		case CX_MATURITY_RATE_A:
			return	eCasCx_MR_A;

		case CX_MATURITY_RATE_X:
			return	eCasCx_MR_X;

		default:
			HxLOG_Critical("\n\n");
			return	eCasCx_MR_G;
	}
}
STATIC HINT32	xmgr_cas_CxMrConvertMRValueToString(HUINT8	ucMRValue)
{
	// CONAX MR Value 값에 해당하는 Item Value
	switch (ucMRValue)
	{
		case CX_MATURITY_RATE_G:
			return	STR_G_ID;

		case CX_MATURITY_RATE_PG:
			return	STR_G_ID;

		case CX_MATURITY_RATE_A:
			return	STR_G_ID;

		case CX_MATURITY_RATE_X:
			return	STR_G_ID;

		default:
			HxLOG_Critical("\n\n");
			return	STR_G_ID;
	}
}
#if 0 // not use for octo2.0
STATIC void	xmgr_cas_CxMrFindNextFocus(XmgrCas_CxMrContent_t *pContents, HBOOL bUp)
{
	HUINT32		nSubItems;
	HUINT32		i;
	XmgrCas_CxSettingsLeafSubItem_t *pSubItem = NULL;

	nSubItems = AP_Contents_CountSubitem((void *)pContents);

	for ( i = 0; i < nSubItems; i++ )
	{
		if (bUp == TRUE)
		{
			pSubItem = AP_Contents_GetSubitem((void *)pContents, (nSubItems - 1) - i);
		}
		else
		{
			pSubItem = AP_Contents_GetSubitem((void *)pContents, i);
		}

		if ( pSubItem == NULL || pSubItem->bDisable == TRUE )
		{
			continue;
		}

		GWM_Obj_SetFocus(pSubItem->id);
		break;
	}
}


STATIC HERROR	xmgr_cas_CxMrDestroyContents (XmgrCas_CxMrContent_t *pContents)
{
	if (pContents == NULL)
	{
		return ERR_FAIL;
	}

	AP_Contents_FreeSubitemArray ((void *)pContents);
	HxSTD_memset (pContents, 0, sizeof(XmgrCas_CxMrContent_t));

	return ERR_OK;
}

#endif
#define	________________Custom_Drawing_Func_________________
// TODO: To remove
#if 0 // not use for octo2.0
STATIC void	xmgr_cas_CxMrDrawDefaultUI(XmgrCas_CxMrContent_t *pContents)
{
	OSD_Rect_t		rect = {0, };

	/* Background */
	Settings_DrawBack(FALSE);

	/* Title */
	MWC_UTIL_DvbStrcpy(pContents->ucTitle, AP_CasResStr_GetStrRsc(LOC_CAS_CX_MR_TITLE_ID));
	Settings_SetWindowTitle(SETTING_GET_AP_DEPTH(eSetting_CasCxMaturityRate), LOC_CAS_CX_MR_TITLE_ID, pContents->ucTitle);

	/* Init sub item objects */
	Settings_InitLeafSubItemsInMenuContents((void *)pContents, (GWM_ObjDraw_t)AP_LNF_DrawLeafSubItemMethod);

	/* Maturity Rating Value */


	OSD_GFX_SetRect(&rect, CAS_CX_MR_ITEM01_X, CAS_CX_MR_ITEM01_Y, CAS_CX_MR_ITEM01_WIDTH, CAS_CX_MR_ITEM01_HEIGHT);
	GWM_Rect_Create(CAS_CX_MR_ITEM12_ID, &rect, 0x00000000);
	GWM_APP_SetObjectDrawMethod(CAS_CX_MR_ITEM12_ID, (GWM_ObjDraw_t)xmgr_cas_CxMrDrawMRInfo);

	GWM_Obj_SetFocus(pContents->subItems[pContents->ulCurFocus].id);
}


STATIC void	xmgr_cas_CxMrDrawMRInfo(OSD_Screen_t *pOsdScreen, OSD_Area_t *appArea,
							OSD_Rect_t *objRect, GWM_GuiObject_t object, void *arg, int bUiTop)
{
	HUINT8			szSting[TEXT_SIZE64] = {0,};
	OSD_Rect_t		rect = {0, };
	XmgrCas_CxMrContent_t	*pContents = NULL;


	XMGR_CAS_CX_PARAM_UNUSED(pOsdScreen);
	XMGR_CAS_CX_PARAM_UNUSED(appArea);
	XMGR_CAS_CX_PARAM_WARRING_REMOVE(objRect, object, arg, bUiTop);

	HxSTD_memset(s_szStrText, 0, TEXT_SIZE128);

	pContents = xmgr_cas_CxMrGetLocalContents();

	/* Card Level Info */
	xmgr_cas_CxGetMatutiryRateStr(szSting, pContents->ucCurMatRatLevel);

	OSD_GFX_SetRect(&rect, CAS_CX_MR_ITEM01_X, CAS_CX_MR_ITEM01_Y, 	CAS_CX_MR_ITEM01_WIDTH, CAS_CX_MR_ITEM01_HEIGHT);
	MWC_UTIL_DvbUtf8Sprintf(s_szStrText, (HUINT8*)"%s %s", (HUINT8 *)AP_CasResStr_GetStrRsc(LOC_CAS_CX_MR_CARDLEVEL_ITEM_ID), (HUINT8 *)szSting);
	GWM_Text_Create(CAS_CX_MR_ITEM01_ID, &rect, (HUINT8*)s_szStrText);

	GWM_Text_ChangeColor(CAS_CX_MR_ITEM01_ID, COL(C_T_Help_Wzd), CAS_CX_MR_DEF_COLOR);
	GWM_Text_SetSize(CAS_CX_MR_ITEM01_ID, FONT_SIZE_M);
	GWM_Text_SetAlign(CAS_CX_MR_ITEM01_ID, TEXT_ALIGN_LEFT);

	/* Available Settings : */
	OSD_GFX_SetRect(&rect, CAS_CX_MR_ITEM03_X, CAS_CX_MR_ITEM03_Y, 	CAS_CX_MR_ITEM03_WIDTH, CAS_CX_MR_ITEM03_HEIGHT);
	GWM_Text_Create(CAS_CX_MR_ITEM03_ID, &rect, (HUINT8*)AP_CasResStr_GetStrRsc(LOC_CAS_CX_MR_AVAILABLE_SETTING_ITEM_ID));
	GWM_Text_ChangeColor(CAS_CX_MR_ITEM03_ID, COL(C_T_Help_Wzd), CAS_CX_MR_DEF_COLOR);
	GWM_Text_SetSize(CAS_CX_MR_ITEM03_ID, FONT_SIZE_M);
	GWM_Text_SetAlign(CAS_CX_MR_ITEM03_ID, TEXT_ALIGN_LEFT);

	/* G - Card Level Info */
	OSD_GFX_SetRect(&rect, CAS_CX_MR_ITEM04_X, CAS_CX_MR_ITEM04_Y, 	CAS_CX_MR_ITEM04_WIDTH, CAS_CX_MR_ITEM04_HEIGHT);
	GWM_Text_Create(CAS_CX_MR_ITEM04_ID, &rect, (HUINT8*)AP_CasResStr_GetStrRsc(LOC_CAS_CX_MR_G_ITEM_ID));
	GWM_Text_ChangeColor(CAS_CX_MR_ITEM04_ID, COL(C_T_Help_Wzd), CAS_CX_MR_DEF_COLOR);
	GWM_Text_SetSize(CAS_CX_MR_ITEM04_ID, FONT_SIZE_M);
	GWM_Text_SetAlign(CAS_CX_MR_ITEM04_ID, TEXT_ALIGN_LEFT);

	/* G - Card Level Info  - Content */
	OSD_GFX_SetRect(&rect, CAS_CX_MR_ITEM08_X, CAS_CX_MR_ITEM08_Y, 	CAS_CX_MR_ITEM08_WIDTH, CAS_CX_MR_ITEM08_HEIGHT);
	GWM_Text_Create(CAS_CX_MR_ITEM08_ID, &rect, (HUINT8*)AP_CasResStr_GetStrRsc(LOC_CAS_CX_MR_G_TEXT_ID));
	GWM_Text_ChangeColor(CAS_CX_MR_ITEM08_ID, COL(C_T_Info_Wzd), CAS_CX_MR_DEF_COLOR);
	GWM_Text_SetSize(CAS_CX_MR_ITEM08_ID, FONT_SIZE_M);
	GWM_Text_SetAlign(CAS_CX_MR_ITEM08_ID, TEXT_ALIGN_LEFT);

	/* PG - Card Level Info */
	OSD_GFX_SetRect(&rect, CAS_CX_MR_ITEM05_X, CAS_CX_MR_ITEM05_Y, 	CAS_CX_MR_ITEM05_WIDTH, CAS_CX_MR_ITEM05_HEIGHT);
	GWM_Text_Create(CAS_CX_MR_ITEM05_ID, &rect, (HUINT8*)AP_CasResStr_GetStrRsc(LOC_CAS_CX_MR_PG_ITEM_ID));
	GWM_Text_ChangeColor(CAS_CX_MR_ITEM05_ID, COL(C_T_Help_Wzd), CAS_CX_MR_DEF_COLOR);
	GWM_Text_SetSize(CAS_CX_MR_ITEM05_ID, FONT_SIZE_M);
	GWM_Text_SetAlign(CAS_CX_MR_ITEM05_ID, TEXT_ALIGN_LEFT);

	/* PG - Card Level Info - Content */
	OSD_GFX_SetRect(&rect, CAS_CX_MR_ITEM09_X, CAS_CX_MR_ITEM09_Y, 	CAS_CX_MR_ITEM09_WIDTH, CAS_CX_MR_ITEM09_HEIGHT);
	GWM_Text_Create(CAS_CX_MR_ITEM09_ID, &rect, (HUINT8*)AP_CasResStr_GetStrRsc(LOC_CAS_CX_MR_PG_TEXT_ID));
	GWM_Text_ChangeColor(CAS_CX_MR_ITEM09_ID, COL(C_T_Info_Wzd), CAS_CX_MR_DEF_COLOR);
	GWM_Text_SetSize(CAS_CX_MR_ITEM09_ID, FONT_SIZE_M);
	GWM_Text_SetAlign(CAS_CX_MR_ITEM09_ID, TEXT_ALIGN_LEFT);

	/* A - Card Level Info */
	OSD_GFX_SetRect(&rect, CAS_CX_MR_ITEM06_X, CAS_CX_MR_ITEM06_Y, 	CAS_CX_MR_ITEM06_WIDTH, CAS_CX_MR_ITEM06_HEIGHT);
	GWM_Text_Create(CAS_CX_MR_ITEM06_ID, &rect, (HUINT8*)AP_CasResStr_GetStrRsc(LOC_CAS_CX_MR_A_ITEM_ID));
	GWM_Text_ChangeColor(CAS_CX_MR_ITEM06_ID, COL(C_T_Help_Wzd), CAS_CX_MR_DEF_COLOR);
	GWM_Text_SetSize(CAS_CX_MR_ITEM06_ID, FONT_SIZE_M);
	GWM_Text_SetAlign(CAS_CX_MR_ITEM06_ID, TEXT_ALIGN_LEFT);

	/* A - Card Level Info - CONTENT */
	OSD_GFX_SetRect(&rect, CAS_CX_MR_ITEM10_X, CAS_CX_MR_ITEM10_Y, 	CAS_CX_MR_ITEM10_WIDTH, CAS_CX_MR_ITEM10_HEIGHT);
	GWM_Text_Create(CAS_CX_MR_ITEM10_ID, &rect, (HUINT8*)AP_CasResStr_GetStrRsc(LOC_CAS_CX_MR_A_TEXT_ID));
	GWM_Text_ChangeColor(CAS_CX_MR_ITEM10_ID, COL(C_T_Info_Wzd), CAS_CX_MR_DEF_COLOR);
	GWM_Text_SetSize(CAS_CX_MR_ITEM10_ID, FONT_SIZE_M);
	GWM_Text_SetAlign(CAS_CX_MR_ITEM10_ID, TEXT_ALIGN_LEFT);

	/* X - Card Level Info */
	OSD_GFX_SetRect(&rect, CAS_CX_MR_ITEM07_X, CAS_CX_MR_ITEM07_Y, 	CAS_CX_MR_ITEM07_WIDTH, CAS_CX_MR_ITEM07_HEIGHT);
	GWM_Text_Create(CAS_CX_MR_ITEM07_ID, &rect, (HUINT8*)AP_CasResStr_GetStrRsc(LOC_CAS_CX_MR_X_ITEM_ID));
	GWM_Text_ChangeColor(CAS_CX_MR_ITEM07_ID, COL(C_T_Help_Wzd), CAS_CX_MR_DEF_COLOR);
	GWM_Text_SetSize(CAS_CX_MR_ITEM07_ID, FONT_SIZE_M);
	GWM_Text_SetAlign(CAS_CX_MR_ITEM07_ID, TEXT_ALIGN_LEFT);

	/* X - Card Level Info - Content */
	OSD_GFX_SetRect(&rect, CAS_CX_MR_ITEM11_X, CAS_CX_MR_ITEM11_Y, 	CAS_CX_MR_ITEM11_WIDTH, CAS_CX_MR_ITEM11_HEIGHT);
	GWM_Text_Create(CAS_CX_MR_ITEM11_ID, &rect, (HUINT8*)AP_CasResStr_GetStrRsc(LOC_CAS_CX_MR_X_TEXT_ID));
	GWM_Text_ChangeColor(CAS_CX_MR_ITEM11_ID, COL(C_T_Info_Wzd), CAS_CX_MR_DEF_COLOR);
	GWM_Text_SetSize(CAS_CX_MR_ITEM11_ID, FONT_SIZE_M);
	GWM_Text_SetAlign(CAS_CX_MR_ITEM11_ID, TEXT_ALIGN_LEFT);
}
#endif

#define	________________Launch_Popups_______________________
// TODO: To remove
#if 0 // not use for octo2.0

STATIC void	xmgr_cas_CxMrCreatePopupList(XmgrCas_CxMrContent_t *pContents, HINT32 ulObjId)
{
	HUINT32		nTextList;
	HUINT32		ulIndex;
	HUINT8		**pptextList;
	OSD_Rect_t	stRect;
	XmgrCas_CxSettingsLeafSubItem_t *psubItem;

	/* Get subitem matched nObjId */
	psubItem = AP_Contents_GetSubitemByObjId ((void *)pContents, ulObjId);
	if (psubItem == NULL)
	{
		return;
	}

	pptextList	= psubItem->itemTextList;
	nTextList	= psubItem->numOfItemTextList;
	ulIndex		= psubItem->idxOfTextList;

	OSD_GFX_SetRect(&stRect, LEAF_APP_POPUP_X, LEAF_APP_POPUP_Y, LEAF_APP_POPUP_WIDTH, LEAF_APP_POPUP_HEIGHT);

	AP_ListDlg_SetListPopup (xmgr_cas_CxMrGetPopupListContents(),
							BUS_MGR_GetAppCallback(BUS_MGR_GetThis()),
							&stRect,
							eListDlg_BG_4LeafApp,
							nTextList,
							ulIndex,
							(HUINT32)pptextList);

	/* Create Pop Up */
	BUS_MGR_Create ("xmgr_cas_CxMrDisplayListPopup", APP_DEFAULT_PRIORITY,
					xmgr_cas_CxMrDisplayListPopup, HANDLE_NULL, 0, 0, 0);
}


STATIC BUS_Result_t	xmgr_cas_CxMrDisplayListPopup(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return AP_ListDlg_ProtoProc (&s_PopupListCasCxMRContent, message, hAction, p1, p2, p3);
}


STATIC HERROR	xmgr_cas_CxMrDestroyListPopup(void)
{
	return BUS_MGR_Destroy(xmgr_cas_CxMrDisplayListPopup);
}
#endif

/* pin code proc */
STATIC BUS_Result_t	xproc_cas_CxMrPinCode(HINT32 message, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return xproc_cas_CxCheckPinDialog(&s_MrPinCodeInstance, message, handle, p1, p2, p3);
}


#define	________________Action_Func__________________________

STATIC void	xmgr_cas_CxMrProcessClicked(XmgrCas_CxMrContent_t *pContents, HINT32 nObjId)
{
	HINT32		lItemIdx=0;

	//lItemIdx = xmgr_cas_CxMrConvertObjIdToItemIdx(nObjId);
	switch (lItemIdx)
	{
		case eCasCxMRItem_ChangeMR:
			//xmgr_cas_CxMrCreatePopupList(pContents, nObjId);
			break;

		default :
			HxLOG_Critical("\n\n");
			break;
	}
}


STATIC HERROR	xmgr_cas_CxMrChangedMRLevelValue(XmgrCas_CxMrContent_t *pContents)
{
	HINT32		lItemIdx=0;
	HINT32		nObjId = 0;

	//nObjId = GWM_GetFocusedObjectID();
	//lItemIdx = xmgr_cas_CxMrConvertObjIdToItemIdx(nObjId);

	switch (lItemIdx)
	{
		case eCasCxMRItem_ChangeMR:
			xmgr_cas_CxMrChangeSubItemValue(nObjId, pContents->lChgValueIdx);
			xmgr_cas_CxMrUpdateSubItem(pContents, nObjId);

			//GWM_APP_InvalidateObject(CAS_CX_MR_ITEM12_ID);
			break;

		default:
			HxLOG_Critical("\n\n");
			return ERR_FAIL;
	}

	return ERR_OK;
}


STATIC void	xmgr_cas_CxMrChangeSubItemValue(HINT32 nObjId, HINT32 ulCurIdx)
{
	//SettingsLeafSubItem_t	*pSubItem = NULL;
	//HUINT32 				ulMenuItemId = 0;

	// TODO: To get data...
#if 0 // not use for octo2.0
	pSubItem = AP_Contents_GetSubitemByObjId((void *)xmgr_cas_CxMrGetLocalContents(), nObjId);

	/* Index 변경. */
	pSubItem->idxOfTextList = ulCurIdx;

	/* Update UI */
	MWC_UTIL_DvbStrcpy(pSubItem->subItemText, pSubItem->itemTextList[pSubItem->idxOfTextList]);

	/* Update subItem value */
	ulMenuItemId = xmgr_cas_CxMrConvertItemIdxToMenuItemId(xmgr_cas_CxMrConvertObjIdToItemIdx(nObjId));
	AP_MenuItem_GetValueFromIndex(ulMenuItemId, ulCurIdx, &(pSubItem->subItemValue));
#endif
}


STATIC HERROR	xmgr_cas_CxMrChangedPopupValue(XmgrCas_CxMrContent_t *pContents, HINT32 lCurIdx, HINT32 p2, HINT32 p3)
{
	HINT32		lItemIdx;
	//HINT32		nObjId = 0;
	//HUINT8		ucItemValue;

	XMGR_CAS_CX_PARAM_UNUSED(p2);
	XMGR_CAS_CX_PARAM_UNUSED(p3);

	//nObjId = GWM_GetFocusedObjectID();
	#if 0 // not use for octo2.0
	lItemIdx = xmgr_cas_CxMrConvertObjIdToItemIdx(nObjId);
	#endif

	switch (lItemIdx)
	{
		case eCasCxMRItem_ChangeMR:
			// Chek CAS Pin code, when if u change MR Leverl
			#if 0 // not use for octo2.0
			pContents->lChgValueIdx = lCurIdx;
			ucItemValue = xmgr_cas_CxMrConvertItemValueIdxToMRValue(lCurIdx);
			if (pContents->ucCurMatRatLevel != ucItemValue)
			{
				xmgr_cas_CxMrCheckMRPincode(ucItemValue);
			}
			#endif
			break;

		default:
			HxLOG_Critical("\n\n");
			return ERR_FAIL;
	}

	return ERR_OK;
}



STATIC HERROR	xmgr_cas_CxMrCheckMRPincode(HUINT8 ucMatRateLevel)
{
	XmgrCas_CxPinCheckStepInDlg_e	ePinCheckStep;
	XmgrCas_CxMrContent_t	*pContents = NULL;

#if 0 // not use for octo2.0
	AP_PinDialog_SetDialog(&s_MrPinCodeInstance,
	BUS_MGR_GetAppCallback(BUS_MGR_GetThis ()),
		(DLG_TYPE_TITLE|DLG_TYPE_MESSAGE|DLG_TIMEOUT_NOTIMEOUT),
		(HUINT8 *)AP_CasResStr_GetStrRsc(LOC_CAS_CX_MR_PINCODE_TITLE_ID),
		NULL);
#endif

	pContents = xmgr_cas_CxMrGetLocalContents();
	if(pContents == NULL)
	{
		HxLOG_Critical("\n\n");
		return	ERR_FAIL;
	}

	ePinCheckStep = eCX_PinCheckStep_InDlg_InputPincode;
	s_MrPinCodeInstance.parentAppCallback = xproc_cas_CxMaturityRating;
	s_MrPinCodeInstance.ulDlgAttributes = (XmgrCas_CxPinCheckEnterCaInDlg_e)eCX_PinCheckEnter_MR_CHG;
	#if 0 // not use for octo2.0
	BUS_MGR_Create("xproc_cas_CxMrPinCode", APP_DIALOGBOX_PRIORITY, xproc_cas_CxMrPinCode,
				0, (HINT32)ePinCheckStep, (HINT32)ucMatRateLevel, 0);
	#endif

	return ERR_OK;
}


STATIC HERROR	xmgr_cas_CxMrUpdateSubItem(XmgrCas_CxMrContent_t *pContents, HINT32 nObjId)
{
	HINT32		lItemIdx = 0;
	HUINT8		ucItemValue = 0;
	//SettingsLeafSubItem_t 	*pSubItem = NULL;
#if 0 // not use for octo2.0
	pSubItem = AP_Contents_GetSubitemByObjId(pContents, nObjId);
	lItemIdx = xmgr_cas_CxMrConvertObjIdToItemIdx(nObjId);
	ucItemValue = xmgr_cas_CxMrConvertItemValueIdxToMRValue(pSubItem->subItemValue);
#endif
	switch (lItemIdx)
	{
		case eCasCxMRItem_ChangeMR:
			if (pContents->ucCurMatRatLevel != ucItemValue)
			{
				pContents->ucCurMatRatLevel = ucItemValue;
			}
			break;

		default:
			HxLOG_Critical("\n\n");
			return ERR_FAIL;
	}

	return ERR_OK;
}


#define	________________Init_Func____________________________

STATIC HERROR	xmgr_cas_CxMrInitMRItems(XmgrCas_CxMrContent_t *pContents,HINT32 p1)
{
	HERROR		hError;

	/* param check */
	if (pContents == NULL)
	{
		HxLOG_Critical("\n\n");
		return	ERR_FAIL;
	}

	pContents->pfnCallerCallback = (BUS_Callback_t)p1;
	pContents->ulTotalNum = eCasCxMRItem_MAX;

	/* Title */
	MWC_UTIL_DvbStrcpy(pContents->szTitle, AP_CasResStr_GetStrRsc(STR_MATURITY_RATING_ID));

	/* Get Maturiy Rating Value */
	xmgr_cas_CxMrLoadMRLeveltemValue(pContents);
#if 0 // not use for octo2.0
	/* Init the sub items */
	hError = AP_Contents_InitSubitemArray ((void *)pContents);
	if (hError != ERR_OK)
	{
		HxLOG_Critical("\n\n");
		return	hError;
	}
#endif

	/* Make SubItems */
	xmgr_cas_CxMrInitItemChgMaturityRate(pContents);

	return	ERR_OK;
}


STATIC void	xmgr_cas_CxMrLoadMRLeveltemValue(XmgrCas_CxMrContent_t *pContents)
{
	HUINT8		ucMRValue;

	/* Get MR value */
	xmgr_cas_CxGetCurMatRatLevel(CX_SC_SLOT_0, &ucMRValue);

	pContents->ucCurMatRatLevel = ucMRValue;
	pContents->lChgValueIdx = xmgr_cas_CxMrConvertMRValueToItemValueIdx(ucMRValue);
}


STATIC HERROR	xmgr_cas_CxMrInitItemChgMaturityRate(XmgrCas_CxMrContent_t *pContents)
{
	//HERROR		hError = ERR_FAIL;
	//HUINT32		ulNumOfItems = 0;
	HUINT32		lMRItemIdx = 0;
	//HUINT8		**ppListOfItem = NULL;
	//SettingsLeafSubItem_t	subItem = {0, };

	/* Copy main text */
	MWC_UTIL_DvbStrcpy(pContents->astListItem[eCasCxMRItem_ChangeMR].szItemValue,AP_CasResStr_GetStrRsc(STR_CHANGE_MATURITY_RATING_ID));

	// Maturity Rating Item name Idx
	lMRItemIdx = xmgr_cas_CxMrConvertMRValueToString(pContents->ucCurMatRatLevel);

	HxSTD_snprintf(pContents->astListItem[eCasCxMRItem_CardLevel].szItemValue,MGR_CAS_STR_LENGTH_LONG, "%s %s", AP_CasResStr_GetStrRsc(STR_CHANGE_MATURITY_RATING_ID),AP_CasResStr_GetStrRsc(lMRItemIdx));

// TODO: Multi language 고려해야함

	MWC_UTIL_DvbStrcpy(pContents->astListItem[eCasCxMRItem_Description].szItemValue,AP_CasResStr_GetStrRsc(STR_AVAILABLE_SETTINGS_ID));
	HxSTD_snprintf(pContents->astListItem[eCasCxMRItem_DescGeneral].szItemValue,MGR_CAS_STR_LENGTH_LONG, "%s %s", AP_CasResStr_GetStrRsc(STR_G_ID),AP_CasResStr_GetStrRsc(STR_GENERAL_AUDIENCE_ID));
	HxSTD_snprintf(pContents->astListItem[eCasCxMRItem_DescParental].szItemValue,MGR_CAS_STR_LENGTH_LONG, "%s %s", AP_CasResStr_GetStrRsc(STR_PG_ID),AP_CasResStr_GetStrRsc(STR_GENERAL_AUDIENCE_ID));
	HxSTD_snprintf(pContents->astListItem[eCasCxMRItem_DescOver18].szItemValue,MGR_CAS_STR_LENGTH_LONG, "%s %s", AP_CasResStr_GetStrRsc(STR_A_ID),AP_CasResStr_GetStrRsc(STR_OVER_18_ID));
	HxSTD_snprintf(pContents->astListItem[eCasCxMRItem_DescErotic].szItemValue,MGR_CAS_STR_LENGTH_LONG, "%s %s", AP_CasResStr_GetStrRsc(STR_X_ID),AP_CasResStr_GetStrRsc(STR_EROTIC_ID));

	return ERR_OK;

#if 0 // not use for octo2.0
	/* get number of MENUITEM_ID_CASCX_MATURITY_RATE SubItems */
	hError = AP_Menuitem_CountSubitems (MENUITEM_ID_CASCX_MATURITY_RATE, &ulNumOfItems);
	if((hError != ERR_OK) || (ulNumOfItems == 0))
	{
		HxLOG_Critical("\n\n");
		return	hError;
	}

	/* List를 만든다. */
	ppListOfItem = AP_MenuItem_MakeTextListFromMenuItemId(MENUITEM_ID_CASCX_MATURITY_RATE);
	if ( ppListOfItem == NULL )
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	subItem.numOfItemTextList = ulNumOfItems;
	subItem.itemTextList = ppListOfItem;

	// Maturity Rating Itemv value Idx
	lMRItemIdx = xmgr_cas_CxMrConvertMRValueToItemValueIdx(pContents->ucCurMatRatLevel);

	/* List의 Index를 얻어온다. */
	hError = AP_MenuItem_GetIndexFromValue(MENUITEM_ID_CASCX_MATURITY_RATE, lMRItemIdx, &subItem.idxOfTextList);
	if (hError != ERR_OK)
	{
		subItem.idxOfTextList = 0;
		subItem.subItemValue = 0;
	}
	else
	{
		subItem.subItemValue = lMRItemIdx;
	}
	MWC_UTIL_DvbStrcpy(subItem.subItemText, subItem.itemTextList[subItem.idxOfTextList]);

	/* Append the subitem to the menu contents. */
	AP_Contents_AppendSubitem ((void *)pContents, &subItem);
#endif

}

STATIC HERROR xmgr_cas_CxMrSendData(XmgrCas_CxMrContent_t *pstContext)
{
	HERROR					hErr = ERR_OK;
	XmgrCas_CxMrItem_t			*pstItem;
	OxMgrCasUiEvtGrid_t		stEvent;
	OxMgrCasUiGrid_t		*pstGridList;
	OxMgrCasUiGridItem_t	*pstGridItem;
	HUINT32					ulStrLen, i;

	HxSTD_memset(&stEvent, 0, sizeof(OxMgrCasUiEvtGrid_t));
	stEvent.ulUiHandle = (HUINT32)pstContext->pfnCallerCallback;

	pstGridList = &stEvent.stGrid;

	pstGridList->stHeader.ulNumOfColumn = 2;
	pstGridList->stHeader.ulAttr = 0;
	MWC_UTIL_DvbStrncpy(pstGridList->stHeader.szItemAlign, "left|right", MGR_CAS_STR_LENGTH_LONG);
	pstGridList->stHeader.ulFocusType = eMgrCasUiFocusType_Column;
	pstGridList->stHeader.stFocus.ulHorIdx = 0;
	pstGridList->stHeader.stFocus.ulVerIdx = 0;
	pstGridList->stHeader.stStartPos.ulHorIdx = 0;
	pstGridList->stHeader.stStartPos.ulVerIdx = 0;

	ulStrLen = SvcCas_UtilStrLen(pstContext->szTitle);

	if(ulStrLen != 0)
	{
		MWC_UTIL_DvbStrncpy(pstGridList->stHeader.szTitle, pstContext->szTitle, MGR_CAS_STR_LENGTH_LONG);
		pstGridList->stHeader.ulAttr |= eMgrCasUiAttr_Title;
		HxLOG_Print("Menu Title [%s]\n", pstGridList->stHeader.szTitle);
	}
#if 0
	ulStrLen = SvcCas_UtilStrLen(pstContext->szSubTitle);
	if(ulStrLen != 0)
	{
		MWC_UTIL_DvbStrncpy(pstGridList->stHeader.szSubTitle, pstContext->szSubTitle, MGR_CAS_STR_LENGTH_LONG);
		pstGridList->stHeader.ulAttr |= eMgrCasUiAttr_Subtitle;
		HxLOG_Print("Sub Title [%s]\n", pstGridList->stHeader.szSubTitle);
	}
#endif

	pstGridList->ulTotalNum = pstContext->ulTotalNum;
	HxLOG_Print("Item Count[%d]\n", pstGridList->ulTotalNum);

	pstGridList->astGridItem = (OxMgrCasUiGridItem_t *)OxAP_Malloc(sizeof(OxMgrCasUiGridItem_t) * (pstGridList->ulTotalNum));
	if(pstGridList->astGridItem == NULL)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	HxSTD_memset(pstGridList->astGridItem, 0, sizeof(OxMgrCasUiGridItem_t) * (pstGridList->ulTotalNum));

	for (i = 0; i < pstGridList->ulTotalNum; i++)
	{
		pstGridItem = &pstGridList->astGridItem[i];
		pstItem = &pstContext->astListItem[i];

		pstGridItem->ulIndex = i;
		pstGridItem->ulAttr = 0;

		HxSTD_snprintf((char *)pstGridItem->szString, MGR_CAS_STR_LENGTH_LONG, "%s", pstItem->szItemValue);

		pstGridItem->szString[MGR_CAS_STR_LENGTH_LONG - 1] = '\0';
	 	pstGridItem->ulStringLength = SvcCas_UtilStrLen(pstGridItem->szString);
		HxLOG_Print("Item [%d][%s]\n", i, pstGridItem->szString);
	}

	hErr = MGR_APPMGR_MODULE_SendUiEvent (eDxCAS_GROUPID_CX, 0/*ulSlotId*/, EVT_CASUI_GRID, &stEvent);
	if(hErr != ERR_OK)
	{
		HxLOG_Critical("\n\n");
	}

	if(pstGridList->astGridItem != NULL)
	{
		OxAP_Free(pstGridList->astGridItem);
	}
	return hErr;
}
STATIC HERROR		xmgr_cas_CxMrUiRequestSessionStop(XmgrCas_CxMrContent_t *pContext)
{
	OxMgrCasUiEvtReqSessionStop_t stEvent;

	stEvent.ulSessionType = (HUINT32)eMgrCasUiSession_Menu;
	if(MGR_APPMGR_MODULE_SendUiEvent (eDxCAS_GROUPID_CX, 0, eMEVT_CASUI_REQ_CLOSE_SESSION, &stEvent) != ERR_OK)
	{
		HxLOG_Error("\n");
	}

	return ERR_OK;
}
STATIC BUS_Result_t xmgr_cas_CxMrUiMsgInputNotify(XmgrCas_CxMrContent_t *pstContext, HINT32 message, HINT32 p1, HINT32 p2, HINT32 p3)
{
	OxMgrCasInputType_e 	eInputType = (OxMgrCasInputType_e)p2;
	OxMgrCasUiInputNotify_t	*pstCasInputData = (OxMgrCasUiInputNotify_t *)p3;

	HxLOG_Print("[local_cas_cx_ui_about_MsgInputNotify] InputType[%d]", eInputType);

	if(eInputType == eMgrCasInputType_Etc)
	{
		if(pstCasInputData->stEtc.eMgrCasEtcType == eMgrCasEtc_Previous)
		{
			HxLOG_Print("[local_cas_cx_ui_about_MsgInputNotify] : eMgrCasEtc_Previous ");
			xmgr_cas_CxMrUiRequestSessionStop(pstContext);
		}
		else if(pstCasInputData->stEtc.eMgrCasEtcType == eMgrCasEtc_Exit)
		{
			HxLOG_Print("[local_cas_cx_ui_about_MsgInputNotify] : eMgrCasEtc_Exit ");
			xmgr_cas_CxMrUiRequestSessionStop(pstContext);
		}

	}

	return ERR_BUS_MESSAGE_BREAK;
}
#define	________________Global_Function_______________________

/*******************************************************************/
/********************      	Global Function 	********************/
/*******************************************************************/

BUS_Result_t	xproc_cas_CxMaturityRating(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR				hError = ERR_FAIL;
	BUS_Result_t 		gwmResult = ERR_FAIL;
	XmgrCas_CxMrContent_t	*pContents = NULL;

	pContents = xmgr_cas_CxMrGetLocalContents();

	switch ( message )
	{
		case eMEVT_BUS_CREATE :
			/* Initialiaze Items */
			hError = xmgr_cas_CxMrInitMRItems(pContents,p1);
			if(ERR_OK!=hError)			HxLOG_Critical("Critical Eror!!!\n");

			xmgr_cas_CxMrSendData(pContents);

			#if 0 // not use for octo2.0
			/* set menu timer, for TV products */
			BUS_SetTimer(MENU_TIMEOUT_ID, MENU_TIMEOUT);
			#endif
			break;
#if 0
		/* apply changed value in pop-up list to sub item */
		case MSG_APP_SETTING_VALUE_CHANGED:
			HxLOG_Info("MSG_APP_SETTING_VALUE_CHANGED!\n");
			xmgr_cas_CxMrChangedPopupValue(pContents, p1, p2, p3);
			return MESSAGE_BREAK;
#endif

		/* MR Change Pin code OK*/
		case eMEVT_PG_PIN_CHECK_OK:
			xmgr_cas_CxMrChangedMRLevelValue(pContents);
			break;

		case eMEVT_PG_PIN_CHECK_DENIED:
			break;

#if 0
		case MSG_DLG_INPUTED_KEYDOWN:
			BUS_ResetTimer(MENU_TIMEOUT_ID);
			return MESSAGE_BREAK;
#endif

		case MSG_OAPI_CASUI_SESSION_START:
			break;

		case MSG_OAPI_CASUI_SESSION_STOP:
			break;

		case MSG_OAPI_CASUI_INPUT_NOTIFY:
			gwmResult = xmgr_cas_CxMrUiMsgInputNotify(pContents, message, p1, p2, p3);
			if(gwmResult != ERR_OK)
			{
				return gwmResult;
			}
			break;

		case eMEVT_BUS_DESTROY:
			// TODO: Destory 하면 죽는다....
			#if 0
			/* kill pincode dialog, if launched */
			if (BUS_MGR_Get(xproc_cas_CxMaturityRating) != NULL)
			{
				//BUS_MGR_Destroy(xproc_cas_CxMrPinCode);
			}
			BUS_MGR_Destroy(0);
			#endif

			/* kill timer */
			//BUS_KillTimer(MENU_TIMEOUT_ID);




			#if 0 // not use for octo2.0
			//BUS_MGR_Destroy(xproc_cas_CxMrPinCode);

			/* Destroy Contents */
			xmgr_cas_CxMrDestroyContents(pContents);

			/* pop-up list */
			xmgr_cas_CxMrDestroyListPopup();

			//AP_SPEC_ClearMenuPanelString();
			#endif
			break;

		default:
			break;

		// TODO: Blocking...
		#if 0 // not use for octo2.0

		case MSG_GWM_KEYUP:
			AP_KeyConv_UiCommon(&p1);
			break;

		case MSG_GWM_KEYDOWN:
			HxLOG_Info("MSG_GWM_KEYDOWN! - p2(0x%x)\n",p2);
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

					BUS_MGR_Destroy(NULL);
					return MESSAGE_BREAK;

				case KEY_ARROWLEFT:
				case KEY_BACK:
					BUS_MGR_Destroy(NULL);
					BUS_MGR_Create((char*)"xproc_cas_CxSmartCardMenu", (APP_DEFAULT_PRIORITY | UISTATE_MENU), (GWM_Callback_t)xproc_cas_CxSmartCardMenu, 0, 0, 0, 0);
					return MESSAGE_BREAK;
			}
			break;

		case MSG_GWM_SCROLLUP:
			HxLOG_Info("MSG_GWM_SCROLLUP!\n");
			xmgr_cas_CxMrFindNextFocus(pContents, TRUE);
			return MESSAGE_BREAK;

		case MSG_GWM_SCROLLDOWN:
			HxLOG_Info("MSG_GWM_SCROLLDOWN!\n");
			xmgr_cas_CxMrFindNextFocus(pContents, FALSE);
			return MESSAGE_BREAK;

		case MSG_GWM_TIMER:
			if (p1 == MENU_TIMEOUT_ID)
			{
				/* reset Title depth */
				Settings_ClearWindowTitle();

				BUS_MGR_Destroy(0);
				return MESSAGE_BREAK;
			}
			break;
		case eSEVT_CAS_SC_REMOVED:
			HxLOG_Print(" SC Event : eSEVT_CAS_SC_REMOVED!\n");
			BUS_SendMessage(xproc_cas_CxMaturityRating, MSG_GWM_KEYDOWN, hAction, KEY_MENU, 0, 0);
			break;
#endif
		// TODO: Blocking...
		#if 0 // not use for octo2.0

		/* popup list를 보여 준다 */
		case MSG_GWM_CLICKED:
		case MSG_GWM_SCROLLRIGHT:
			HxLOG_Info("MSG_GWM_CLICKED or MSG_GWM_SCROLLRIGHT!\n");
			xmgr_cas_CxMrProcessClicked(pContents, p1);
			return MESSAGE_BREAK;
		#endif


	}

	return BUS_ProcessMessageDefault(message, hAction, p1, p2, p3);
}

// End of file

