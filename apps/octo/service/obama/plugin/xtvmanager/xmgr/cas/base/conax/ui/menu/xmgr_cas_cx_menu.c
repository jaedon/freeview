/**
@file			xmgr_cas_cx_menu.c
@brief			CONAX UI 중 CONAX CA Menu 처리.

Description:  \n
Module: AP/CAS/CONAX/UI			 	\n

Copyright (c) 2008 HUMAX Co., Ltd.				\n
All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    **************************/
/*******************************************************************/
#include <octo_common.h>
#include <isosvc.h>

#include <svc_cas.h>
#include <svc_sys.h>
#include <conax_ctrl.h>

#include <mgr_common.h>
#include <mgr_action.h>
#include <mgr_cas.h>
#include <xmgr_cas_util_dslib.h>
#include <xmgr_cas_res_str.h>

#include "../../local_include/_xmgr_cas_cx.h"
#include "../../local_include/_xmgr_cas_cx_util.h"

/*******************************************************************/
/********************      Definitions     *********************/
/*******************************************************************/

#define	D_TEXT_LINE_LENGTH				54
#define	MAX_ELEMENT_TEXT_LENGTH			256
#define CX_SC_CX6_VERSION				0x40
#define	CX_ABOUT_ITEM_TEXT_LENGTH		32


/*******************************************************************/
/********************      Typedef         **************************/
/*******************************************************************/
/*
typedef struct CxSystemItem
{
	XmgrCas_CxCaMenuItem_e	mainBtnId;
	HBOOL					bItemDisable;
	BUS_Callback_t			pfnUiCallback;
	HUINT8					*pszString;
} XmgrCas_CxMenuItem_t;
*/

//typedef enum
//{
//	eCxMenu_STATE_INIT = 0,		/* appl 시작한 상태, menu에서 mmi open한 것이면 mmi open 요청을 함, no ui, no keyblock */
//	eCxMenu_STATE_RUNNING,		/* MMI Data를 받기 시작한 상태, ui 그려진 상태로 key block을 시작한다 */

//	eCxMenu_STATE_MAX
//} XmgrCas_CxMenuState_e;

/*
typedef struct tagXmgrCiMmiUiInfo
{
	SvcCas_MmiEnq_t 				*pstEnqParam;
	SvcCas_MmiMenuList_t 			*pstListParam;
	HUINT8 							*pszTextParam;

	XmgrCas_CiMmiEnqParam_t			stEnqParam;
	XmgrCas_CiMmiType_e 			eMmiType;
	Handle_t						hSession;
	Handle_t						hMmi;
	XmgrCas_CiMmiSessionStatus_e	eSessionStatus;
	DxCAS_MmiSessionType_e			eSessionType;
	HBOOL							bIsHavingData;
} XmgrCas_CiMmiUiInfo_t;

typedef struct tagXmgrCiMmiAppData
{
	HUINT16							usSlotId;
	XmgrCas_CiMmiUiInfo_t			stMenuInfo;
	XmgrCas_CiMmiUiInfo_t			stExtensionInfo;

	XmgrCas_CiMmiCurrentUi_e		eCurrentUi;
	HUINT32							ulUiHandle;
} XmgrCas_CiMmiContext_t;
*/

typedef enum
{
	eCasCx_AboutItem_Interface_Version,		/* Interface Version */
	eCasCx_AboutItem_Card_Number,			/* Card Number */
#if defined(CONFIG_MW_CAS_CONAX_CPM)
	eCasCx_AboutItem_Chip_ID,				/* Chip ID */
#endif
	eCasCx_AboutItem_Session_Number,		/* Number of sessions */
	eCasCx_AboutItem_Language,				/* Language */
	eCasCx_AboutItem_System_ID,				/* CA_SYS_ID */
	eCasCx_AboutItem_Max,
} XmgrCas_CxAboutItem_e;

typedef struct CxAboutMenuItem
{
	XmgrCas_CxAboutItem_e	eItemId;
	HBOOL					bItemDisable;
	HUINT8					szItemName[MGR_CAS_STR_LENGTH_LONG];
	HUINT8					szItemValue[MGR_CAS_STR_LENGTH_LONG];
} XmgrCas_CxAboutMenuItem_t;

typedef enum
{
	eCxAuth_TimeLimitedAccess	= 0,
	eCxAuth_UnlimitedAccess,
	eCxAuth_CreditBasedAccess
} CxAuthType_t;

typedef struct CxMenuSubItemContext
{
	HUINT32				ulTotalNum;

	HUINT8				szTitle[MAX_ELEMENT_TEXT_LENGTH];
	HUINT8				szColumnTitle[MAX_ELEMENT_TEXT_LENGTH];
	CxPurseStatus_t		stPurseStatus;		// tokens , debit
	HERROR				hPinStatus;
	Custom_list_t		stTextListBox;
	HUINT32 			ulFocusIndex;
//	XmgrCas_CxListMenuItem_t	*astListItem;
} XmgrCas_CxUiMenuSubItemContext_t;

typedef struct CxMenuContext {
	HUINT32						ulSlot;
	HUINT16						usMenuDepth;
	HBOOL						bScInserted;
	XmgrCas_CxCaMenuItem_e		eSelectedMenuItem;
	OxMgrCasUiEvtSession_t		stSessionInfo;
	XmgrCas_CxUiSessionStatus_e	eSessionStatus;
	XmgrCas_CxUiMenuSubItemContext_t	stSubItem;
	XmgrCas_CxAboutMenuItem_t	astAboutMenuItem[eCasCx_AboutItem_Max];
} XmgrCas_CxUiMenuContext_t;

typedef struct	tagCXMailItem
{
	HBOOL	bRead;
//	HBOOL	bCheck;
	HINT32	nMailIdx;
	HUINT8	strReceivedDate[6];
	HUINT8	strReceivedTime[9];
//	HINT32	nMailType;
	HUINT8	strMailTxt[CX_MAX_USER_TEXT_LEN + 1]; // this data has normal msg  & attributed msg property, then it must have biggest size.
} XmgrCas_CxUiMailItemData_t;


/*******************************************************************/
/********************      Global Variables     *********************/
/*******************************************************************/


/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
/*
//STATIC XmgrCas_CiMmiContext_t			s_astCiMmi_Context[XMGR_CAS_CI_SLOT_MAX];
STATIC XmgrCas_CiMmiContext_t	s_astCiMmi_Context[XMGR_CAS_CI_SLOT_MAX];
//STATIC HUINT32					s_ulMmiSlotMax = 0;
STATIC HUINT16					s_usSlotIdForDelayedSsClose = 0;
//STATIC XmgrCas_CiMmiState_e				s_eCurrentState = 0;
*/
STATIC XmgrCas_CxUiMenuContext_t	s_stCxUiMenuContext;
STATIC XmgrCas_CxUiMailItemData_t	s_astMailItemData[CX_MAX_USERTEXT_SLOT_NUM];

/********************************************************************/
/********************      Local Functions	*************************/
/********************************************************************/
STATIC XmgrCas_CxUiMenuContext_t *xmgr_cas_CxUiMenuGetContext(void)
{
	return &s_stCxUiMenuContext;
}

STATIC void xmgr_cas_CxUiMenuInitContext(XmgrCas_CxUiMenuContext_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	/**
	  *	p1 : slot id
	  * p2 : sessionTYPE
	  *	p3 : data verison
	**/

	pstContext->ulSlot = (HUINT32)p1;
	pstContext->bScInserted = FALSE;
	pstContext->usMenuDepth = 0;
	pstContext->stSessionInfo.eSessionType		= eDxCAS_SESSIONTYPE_MENU;
	pstContext->stSessionInfo.hSession			= HANDLE_NULL;
	pstContext->stSessionInfo.ulUiHandle		= (HUINT32)xproc_cas_CxUiMenu;
	pstContext->eSessionStatus					= eCX_UI_SESSION_STATUS_Stopped;

	return;
}

STATIC HERROR	xmgr_cas_CxUiMenuIsValidMailMessage(HUINT16 ulDuration)
{
	if (ulDuration == 0)
	{
		return	ERR_OK;
	}
	else
	{
		return	ERR_FAIL;
	}
}

STATIC HCHAR *xmgr_cas_CxUiMenuEncodeString(HCHAR *pszString)
{
	HUINT32		Len = 0;
	HCHAR		*buf = NULL;

	Len = HLIB_STD_URLStrLen(pszString) + 1;
    buf = HLIB_STD_MemCalloc(Len+1);
    HLIB_STD_URLEncode(buf,pszString,Len);

    return buf;
}

STATIC HERROR xmgr_cas_CxUiMenuSetJsonStr(HUINT8* szJsonStr, HUINT8* szStr)
{
	HUINT16 usCnt = 0;
	while(*szStr != '\0')
	{
		if(*szStr == '\'' || *szStr == '\"' || *szStr == '\\')
		{
			szJsonStr[usCnt] = '\\';
			usCnt++;
		}
		szJsonStr[usCnt] = *szStr;
		usCnt++;
		szStr++;
	}

	szJsonStr[usCnt] = '\0';

	return ERR_OK;
}

STATIC HERROR xmgr_cas_CxUiMenuSetCxMenuDisplayed(HBOOL bDisplayed)
{
	HERROR							hErr;
	SvcCas_CtrlParam_t				stParam;
	CX_SetCxDisplayedInParam_t	stInParam;

	stInParam.bDisplayed = bDisplayed;
	stParam.pvIn = (void *)&stInParam;

	stParam.ulControlType = eCACTRL_CX_SetCxMenuDisplayed;
	hErr = SVC_CAS_CtrlSet(eDxCAS_GROUPID_CX, eCACTRL_CX_SetCxMenuDisplayed, &stParam);

	return	hErr;
}

STATIC HERROR xmgr_cas_CxUiMenuCloseCxMenu(XmgrCas_CxUiMenuContext_t *pstContext)
{
	HERROR					hErr = ERR_OK;
	BUS_Result_t			hBusResult = ERR_BUS_NO_ERROR;
	OxMgrCasUiEvtBanner_t	*pstEvent = NULL;

	pstEvent = (OxMgrCasUiEvtBanner_t *)HLIB_STD_MemCalloc(sizeof(OxMgrCasUiEvtBanner_t));

	pstEvent->stSessionInfo.eSessionType = pstContext->stSessionInfo.eSessionType;
	pstEvent->stSessionInfo.ulUiHandle = (HUINT32)xproc_cas_CxUiMenu;
	pstEvent->stSessionInfo.hSession = pstContext->stSessionInfo.hSession;

	/* Display할 String length를 0으로 할 경우 Conax Menu를 close 하도록 한다. */
	pstEvent->stBanner.stString.ulStringLength = 0;

	hBusResult = BUS_PostMessageToOM(NULL, eMEVT_CASUI_TYPE_BANNER_DATA, HANDLE_NULL, (HINT32)eDxCAS_GROUPID_CX, (HINT32)pstContext->ulSlot, (HINT32)pstEvent);
	if(hBusResult != ERR_BUS_NO_ERROR)
	{
		hErr = ERR_FAIL;
	}

	return hErr;
}

STATIC HERROR xmgr_cas_CxUiMenuPostSelectInputData(XmgrCas_CxUiMenuContext_t *pstContext, HUINT16 usSlotId, HUINT32 ulItemIdx)
{
	OxMgrCasUiEvtInput_t	*pstEvent = NULL;
	OxMgrCasUiInput_t		*pstInputData = NULL;
	BUS_Result_t			hBusResult = ERR_BUS_NO_ERROR;
	HERROR					hError = ERR_OK;

	if(pstContext == NULL)
	{
		HxLOG_Error("pstContext is NULL!!()\n");
		return ERR_FAIL;
	}

	pstEvent = (OxMgrCasUiEvtInput_t *)HLIB_STD_MemCalloc(sizeof(OxMgrCasUiEvtInput_t));
	if(pstEvent == NULL)
	{
		return ERR_FAIL;
	}

	HxSTD_MemSet(pstEvent, 0, sizeof(OxMgrCasUiEvtInput_t));

	pstEvent->stSessionInfo.eSessionType = pstContext->stSessionInfo.eSessionType;
	pstEvent->stSessionInfo.ulUiHandle = (HUINT32)xproc_cas_CxUiMenu;
	pstEvent->stSessionInfo.hSession = pstContext->stSessionInfo.hSession;

	pstInputData = &pstEvent->stInputData;

	pstInputData->eInputType = eMgrCasInputType_Selection;
	pstInputData->utInputData.stSelection.ulItemIdx = ulItemIdx;

	hBusResult = BUS_PostMessageToOM(NULL, eMEVT_CASUI_TYPE_INPUT_DATA, HANDLE_NULL, (HINT32)eDxCAS_GROUPID_CX, (HINT32)usSlotId, (HINT32)pstEvent);
	if(hBusResult != ERR_BUS_NO_ERROR)
	{
		HxLOG_Error("\n\n");
		hError = ERR_FAIL;
		goto END_FUNC;
	}

END_FUNC:
	if(hError == ERR_FAIL)
	{
		if(pstEvent != NULL)
		{
			HLIB_STD_MemFree((void *)pstEvent);
		}
	}

	return hError;
}

/*
STATIC HERROR xmgr_cas_CxUiMenuPostBannerData(XmgrCas_CxUiMenuContext_t *pstContext, HUINT16 usSlotId, HERROR hStatus)
{
	HERROR					hError = ERR_OK;
	BUS_Result_t			hBusResult = ERR_BUS_NO_ERROR;
	OxMgrCasUiEvtBanner_t	*pstBannerEvent = NULL;

	HINT32					slStream;
	HCHAR					*pzProperties = NULL;
	HUINT32					ulStreamSize = 0;

	pstBannerEvent = (OxMgrCasUiEvtBanner_t *)HLIB_STD_MemCalloc(sizeof(OxMgrCasUiEvtBanner_t));
	if(pstBannerEvent == (OxMgrCasUiEvtBanner_t *)NULL)
	{
		HxLOG_Error("\n");
		hError = ERR_FAIL;
		goto END_FUNC;
	}

	pstBannerEvent->stSessionInfo.eSessionType = pstContext->stSessionInfo.eSessionType;
	pstBannerEvent->stSessionInfo.ulUiHandle = (HUINT32)xproc_cas_CxUiMenu;
	pstBannerEvent->stSessionInfo.hSession = pstContext->stSessionInfo.hSession;

	slStream = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
	HLIB_RWSTREAM_Print(slStream, "[");

	if(pstContext->eSelectedMenuItem == eCasCXMenuItem_Change_Pin)
	{
		switch(hStatus)
		{
			case ERR_OK:
				HLIB_RWSTREAM_Print(slStream, "{\"id\":\"LOC_CAPIN_CHANGED_ID\"}");
				break;
			case CX_ERR_UI_PIN_CHANGE_FAIL:
			case CX_ERR_UI_PIN_LOCKED:
				HLIB_RWSTREAM_Print(slStream, "{\"id\":\"LOC_CAPIN_CHANGE_FAIL_ID\"}");
				break;
			default:
				HxLOG_Critical("ERROR! Check it!!...\n");
				break;
		}
	}
	else if(pstContext->eSelectedMenuItem == eCasCXMenuItem_Maturity_Rating)
	{
		switch(hStatus)
		{
			case ERR_OK:
				// 이 경우는 UI를 따로 출력할 필요 없고, Macturity Rating 변경이 성공적으로 완료 되었으니 해당 내용을 Web에 적용만 하면 됨
				HLIB_RWSTREAM_Print(slStream, "{\"id\":\"LOC_RCS_OK_ID\"}");
				break;
			case CX_ERR_UI_WRONG_PIN:
				// TODO: 번역어 필요
				HLIB_RWSTREAM_Print(slStream, "{\"id\":\"LOC_CAS_CX_MR_PINCODE_WRONGPINCODE_TEXT_ID\"}");
				break;
			case CX_ERR_UI_PIN_LOCKED:
				// 이러한 경우 어떻게 처리해야하는지 Spec에 나와있지 않음. 그냥 Wrong PIN을 받은 것 처럼 해결하면 될 듯 함.
				// 이러한 case에 걸렸을 때 고민하고 적용하도록.
				HxLOG_Critical("ERROR! CX_ERR_UI_PIN_LOCKED... Check it!!...\n");
				break;
			default:
				HxLOG_Critical("ERROR! Check it!!...\n");
				break;
		}
	}

	HLIB_RWSTREAM_Print(slStream, "]");

	pzProperties = (HCHAR*)HLIB_RWSTREAM_CloseWithoutBuf(slStream);
	ulStreamSize = HxSTD_StrLen(pzProperties);

	pstBannerEvent->stBanner.ulAttr = 0;
	pstBannerEvent->stBanner.stString.ulStringLength = ulStreamSize;
	MWC_UTIL_DvbStrncpy(pstBannerEvent->stBanner.stString.szString, pzProperties, ulStreamSize);
	HLIB_STD_MemFree(pzProperties);

	hBusResult = BUS_PostMessageToOM(NULL, eMEVT_CASUI_TYPE_BANNER_DATA, HANDLE_NULL, (HINT32)eDxCAS_GROUPID_CX, (HINT32)usSlotId, (HINT32)pstBannerEvent);
	if(hBusResult != ERR_BUS_NO_ERROR)
	{
		HxLOG_Error("\n\n");
		hError = ERR_FAIL;
		goto END_FUNC;
	}

END_FUNC:
	if(hError == ERR_FAIL)
	{
		if(pstBannerEvent != NULL)
		{
			HLIB_STD_MemFree((void *)pstBannerEvent);
		}
	}

	return hError;
}
*/
STATIC HERROR xmgr_cas_CxUiMenuPostTextData(XmgrCas_CxUiMenuContext_t *pstContext, HUINT16 usSlotId)
{
	HERROR					hError = ERR_OK;
	BUS_Result_t			hBusResult = ERR_BUS_NO_ERROR;
	OxMgrCasUiEvtTextbox_t	*pstTextBoxEvent = NULL;

	HINT32					slStream;
	HCHAR					*pzProperties = NULL;
	HUINT32					ulStreamSize = 0;

	pstTextBoxEvent = (OxMgrCasUiEvtTextbox_t *)HLIB_STD_MemCalloc(sizeof(OxMgrCasUiEvtTextbox_t));
	if(pstTextBoxEvent == (OxMgrCasUiEvtTextbox_t *)NULL)
	{
		HxLOG_Error("\n");
		hError = ERR_FAIL;
		goto END_FUNC;
	}

	pstTextBoxEvent->stSessionInfo.eSessionType = pstContext->stSessionInfo.eSessionType;
	pstTextBoxEvent->stSessionInfo.ulUiHandle = (HUINT32)xproc_cas_CxUiMenu;
	pstTextBoxEvent->stSessionInfo.hSession = pstContext->stSessionInfo.hSession;

	slStream = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);

	if(pstContext->eSelectedMenuItem == eCasCXMenuItem_Change_Pin)
	{
		switch(pstContext->stSubItem.hPinStatus)
		{
			case ERR_OK:
				HLIB_RWSTREAM_Print(slStream, "[{\"action\":\"correct\"}]");
				break;
			case CX_ERR_UI_DIFFERENT_NEW_PIN:
				HxLOG_Error("ERROR! Different new pin is MUST detected at WEB UI...\n");
				break;
			case CX_ERR_UI_WRONG_PIN:
				HLIB_RWSTREAM_Print(slStream, "[{\"action\":\"incorrect\"}]");
				break;
			case CX_ERR_UI_PIN_CHANGE_FAIL:
			case CX_ERR_UI_PIN_LOCKED:
				HLIB_RWSTREAM_Print(slStream, "[{\"action\":\"fail\"}]");
				break;
			default:
				HxLOG_Critical("ERROR! Check it!!...\n");
				break;
		}
	}
	else if(pstContext->eSelectedMenuItem == eCasCXMenuItem_Maturity_Rating)
	{
		switch(pstContext->stSubItem.hPinStatus)
		{
			case ERR_OK:
				// 이 경우는 UI를 따로 출력할 필요 없고, Macturity Rating 변경이 성공적으로 완료 되었으니 해당 내용을 Web에 적용만 하면 됨
				HLIB_RWSTREAM_Print(slStream, "[{\"action\":\"correct\"}]");
				break;
			case CX_ERR_UI_WRONG_PIN:
				HLIB_RWSTREAM_Print(slStream, "[{\"action\":\"incorrect\"}]");
				break;
			case CX_ERR_UI_PIN_LOCKED:
				// 이러한 경우 어떻게 처리해야하는지 Spec에 나와있지 않음. 그냥 Wrong PIN을 받은 것 처럼 해결하면 될 듯 함.
				// 이러한 case에 걸렸을 때 고민하고 적용하도록.
				HxLOG_Critical("ERROR! CX_ERR_UI_PIN_LOCKED... Check it!!...\n");
				HLIB_RWSTREAM_Print(slStream, "[{\"action\":\"incorrect\"}]");
				break;
			default:
				HxLOG_Critical("ERROR! Check it!!...\n");
				break;
		}
	}

	pzProperties = (HCHAR*)HLIB_RWSTREAM_CloseWithoutBuf(slStream);
	ulStreamSize = HxSTD_StrLen(pzProperties);

	pstTextBoxEvent->stTextbox.pszString = (HCHAR *)HLIB_STD_MemCalloc(ulStreamSize+1);
	pstTextBoxEvent->stTextbox.stHeader.ulAttr = 0;
	pstTextBoxEvent->stTextbox.ulStringLength = ulStreamSize;
	MWC_UTIL_DvbStrncpy(pstTextBoxEvent->stTextbox.pszString, pzProperties, ulStreamSize);
	HLIB_STD_MemFree(pzProperties);

	hBusResult = BUS_PostMessageToOM(NULL, eMEVT_CASUI_TYPE_TEXTBOX_DATA, HANDLE_NULL, (HINT32)eDxCAS_GROUPID_CX, (HINT32)usSlotId, (HINT32)pstTextBoxEvent);
	if(hBusResult != ERR_BUS_NO_ERROR)
	{
		HxLOG_Error("\n\n");
		hError = ERR_FAIL;
		goto END_FUNC;
	}

END_FUNC:
	if(hError == ERR_FAIL)
	{
		if(pstTextBoxEvent != NULL)
		{
			HLIB_STD_MemFree((void *)pstTextBoxEvent);
		}
	}

	return hError;
}

STATIC HERROR xmgr_cas_CxUiMenuPostMenuListData(XmgrCas_CxUiMenuContext_t *pstContext, HUINT16 usSlotId, OxMgrCasUIAttribute_e eDefaultAttr)
{
//	SvcCas_MmiMenuList_t 		*pMmiMenuData = NULL;
	HERROR						hError = ERR_OK;
	HUINT8						*pszText = NULL;
	HUINT32						ulStrLen = 0;
	HUINT32						ulMenuItemLoop = 0;
	BUS_Result_t				hBusResult = ERR_BUS_NO_ERROR;
	OxMgrCasUiEvtMenulist_t		*pstEvent = NULL;
	OxMgrCasUiMenulist_t		*pstMenuList = NULL;
	OxMgrCasUiMenuItem_t		*pMenuItem = NULL;
	HINT32						slStream;
	HCHAR						*pzEncodedStr = NULL;
	HCHAR						*pzProperties = NULL;
	HUINT8						*pszListItem = NULL;

	pstEvent = (OxMgrCasUiEvtMenulist_t *)HLIB_STD_MemCalloc(sizeof(OxMgrCasUiEvtMenulist_t));
	if(pstEvent == (OxMgrCasUiEvtMenulist_t *)NULL)
	{
		HxLOG_Error("\n\n");
		hError = ERR_FAIL;
		goto END_FUNC;
	}

	pstEvent->stSessionInfo.eSessionType = pstContext->stSessionInfo.eSessionType;
	pstEvent->stSessionInfo.ulUiHandle = (HUINT32)xproc_cas_CxUiMenu;
	pstEvent->stSessionInfo.hSession = pstContext->stSessionInfo.hSession;

	pstMenuList = &(pstEvent->stMenulist);

	pstMenuList->stHeader.ulAttr = eDefaultAttr;
	pstMenuList->stHeader.ulStartIndex = 0;
	pstMenuList->ulTotalNum = pstContext->stSubItem.ulTotalNum;
	if(pstMenuList->stHeader.ulAttr & eMgrCasUiAttr_Selectable)
	{
		pstMenuList->stHeader.ulFocusIndex = pstContext->stSubItem.ulFocusIndex;
	}
	else
	{
		pstMenuList->stHeader.ulFocusIndex = 0;
	}

	if(pstMenuList->stHeader.ulAttr & eMgrCasUiAttr_Title)
	{
		ulStrLen = SvcCas_UtilStrLen(pstContext->stSubItem.szTitle);
		if(ulStrLen != 0)
		{
			if(ulStrLen >= MGR_CAS_STR_LENGTH_LONG)
			{
				HxLOG_Warning("string length overflow %d\n", ulStrLen);
				ulStrLen = MGR_CAS_STR_LENGTH_LONG-1;
			}

			MWC_UTIL_ConvSiStringToUtf8(NULL, ulStrLen, pstContext->stSubItem.szTitle, &pszText);
			if(pszText != NULL)
			{
				slStream = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
				pzEncodedStr = xmgr_cas_CxUiMenuEncodeString(pszText);
				if(pzEncodedStr != NULL)
				{
					HLIB_RWSTREAM_Print(slStream, "[\"%s\"]", pzEncodedStr);
					HLIB_STD_MemFree(pzEncodedStr);
					pzEncodedStr = NULL;
				}

				pzProperties = (HCHAR*)HLIB_RWSTREAM_CloseWithoutBuf(slStream);

				MWC_UTIL_DvbStrncpy(pstMenuList->stHeader.szTitle, pzProperties, MGR_CAS_STR_LENGTH_LONG);
				HLIB_STD_MemFree(pzProperties);
				HLIB_STD_MemFree(pszText);
			}
		}
	}

	if(pstMenuList->stHeader.ulAttr & eMgrCasUiAttr_Subtitle)
	{
		ulStrLen = SvcCas_UtilStrLen(pstContext->stSubItem.szColumnTitle);
		if(ulStrLen != 0)
		{
			if(ulStrLen >= MGR_CAS_STR_LENGTH_LONG)
			{
				HxLOG_Warning("string length overflow %d\n", ulStrLen);
				ulStrLen = MGR_CAS_STR_LENGTH_LONG-1;
			}

			MWC_UTIL_ConvSiStringToUtf8(NULL, ulStrLen, pstContext->stSubItem.szColumnTitle, &pszText);
			if(pszText != NULL)
			{
				slStream = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
				pzEncodedStr = xmgr_cas_CxUiMenuEncodeString(pszText);
				if(pzEncodedStr != NULL)
				{
					HLIB_RWSTREAM_Print(slStream, "[\"%s\"]", pzEncodedStr);
					HLIB_STD_MemFree(pzEncodedStr);
					pzEncodedStr = NULL;
				}

				pzProperties = (HCHAR*)HLIB_RWSTREAM_CloseWithoutBuf(slStream);

				MWC_UTIL_DvbStrncpy(pstMenuList->stHeader.szSubTitle, pzProperties, MGR_CAS_STR_LENGTH_LONG);
				HLIB_STD_MemFree(pzProperties);
				HLIB_STD_MemFree(pszText);
			}
		}
	}

	HxLOG_Debug("ulTotalNum %d\n", pstMenuList->ulTotalNum);
	pstMenuList->astListItem = (OxMgrCasUiMenuItem_t *)HLIB_STD_MemCalloc(sizeof(OxMgrCasUiMenuItem_t)*(pstMenuList->ulTotalNum));
	if(pstMenuList->astListItem == NULL)
	{
		HxLOG_Error("\n\n");
		hError = ERR_FAIL;
		goto END_FUNC;
	}

	pszListItem = (HUINT8*)OxAP_Malloc(TEXT_SIZE512);
	for(ulMenuItemLoop = 0;ulMenuItemLoop < pstMenuList->ulTotalNum;ulMenuItemLoop++)
	{
		hError = CAS_UTIL_LL_PopFront(&(pstContext->stSubItem.stTextListBox), (void **)&pszListItem);
		HxLOG_Info("[%s]...\n", pszListItem);

		pMenuItem = &pstMenuList->astListItem[ulMenuItemLoop];

		pMenuItem->ulIndex = ulMenuItemLoop;
		pMenuItem->ulAttr = 0;
		pMenuItem->ulType = eMgrCasUiItemType_Str;

		MWC_UTIL_ConvSiStringToUtf8(NULL, SvcCas_UtilStrLen(pszListItem), pszListItem, &pszText);
		if(pszText != NULL)
		{
			pMenuItem->utData.stString.ulStringLength = SvcCas_UtilStrLen(pszText);

			slStream = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
			pzEncodedStr = xmgr_cas_CxUiMenuEncodeString(pszText);
			if(pzEncodedStr != NULL)
			{
				HLIB_RWSTREAM_Print(slStream, "[\"%s\"]", pzEncodedStr);
				HLIB_STD_MemFree(pzEncodedStr);
				pzEncodedStr = NULL;
			}

			pzProperties = (HCHAR*)HLIB_RWSTREAM_CloseWithoutBuf(slStream);

			MWC_UTIL_DvbStrncpy(pMenuItem->utData.stString.szString, pzProperties, MGR_CAS_STR_LENGTH_LONG);
			pMenuItem->utData.stString.szString[MGR_CAS_STR_LENGTH_LONG - 1] = '\0';
			HxLOG_Info("[%s]...\n", pMenuItem->utData.stString.szString);
			HLIB_STD_MemFree(pzProperties);

			HLIB_STD_MemFree(pszText);
		}
		HxLOG_Debug("(%d) %s\n", pMenuItem->ulIndex, pMenuItem->utData.stString.szString);
	}

	hBusResult = BUS_PostMessageToOM(NULL, eMEVT_CASUI_TYPE_MENULIST_DATA, HANDLE_NULL, (HINT32)eDxCAS_GROUPID_CX, (HINT32)usSlotId, (HINT32)pstEvent);
	if(hBusResult != ERR_BUS_NO_ERROR)
	{
		HxLOG_Error("\n\n");
		hError = ERR_FAIL;
		goto END_FUNC;
	}

END_FUNC:
	if(hError == ERR_FAIL)
	{
		if(pstEvent != NULL)
		{
			if(pstMenuList->astListItem != NULL)
			{
				HLIB_STD_MemFree(pstMenuList->astListItem);
			}
			HLIB_STD_MemFree(pstEvent);
		}
	}

	{
		HUINT8	*szItem;
		HERROR	err;
		HINT32	i, numOfListItems = 0;

		numOfListItems	= CAS_UTIL_LL_GetNumOfListItem(&pstContext->stSubItem.stTextListBox);
		for (i = 0 ; i < numOfListItems ; i++)
		{
			err = CAS_UTIL_LL_ReadIndexNthItem(&pstContext->stSubItem.stTextListBox, i, (void **)&szItem);
			if (err != ERR_OK || szItem == NULL)
			{
				continue;
			}

			HLIB_STD_MemFree(szItem);
		}
		CAS_UTIL_LL_InitList(&pstContext->stSubItem.stTextListBox);
	}

	if(pszListItem != NULL)
	{
		HLIB_STD_MemFree(pszListItem);
	}

	return hError;
}

STATIC HERROR	xmgr_cas_CxUiMenuGetCardNumber(HUINT32 ulCardNum, HUINT8 *pszCardNum, HUINT32 ulSize)
{
	HUINT32		ulCardNumHigh, ulCardNumMiddle, ulCardNumLow, ulCardNumChecksum, ulCardNumber;
	HUINT32 	ulTmpInt = 0;

	ulCardNumber = ulCardNum;

	ulCardNumHigh = ulCardNumber/100000000;
	ulCardNumber -= (ulCardNumHigh*100000000);
	ulCardNumMiddle = ulCardNumber/10000;
	ulCardNumber -= (ulCardNumMiddle*10000);
	ulCardNumLow = ulCardNumber;

	// calculate checksum of smart card
	ulTmpInt = xmgr_cas_CxCalcDigit(3, ulCardNumHigh);
	ulTmpInt += xmgr_cas_CxCalcDigit(4, ulCardNumMiddle);
	ulTmpInt += xmgr_cas_CxCalcDigit(4, ulCardNumLow);
	ulCardNumChecksum = (10 - (ulTmpInt%10))%10;

	HxSTD_snprintf(pszCardNum, ulSize, (HUINT8*)"{\"str\":\"%03d %04d %04d - %01d\"}", ulCardNumHigh, ulCardNumMiddle, ulCardNumLow, ulCardNumChecksum);

	return ERR_OK;
}

#if defined(CONFIG_MW_CAS_CONAX_CPM)
STATIC HERROR	xmgr_cas_CxUiMenuGetChipId(HUINT8 *pszChipId, HUINT32 ulSize)
{
	HUINT32 ulChipId = 0, ulPart1, ulPart2, ulPart3;
	HERROR err;

	err = SVC_SYS_GetChipId(&ulChipId);
	ulPart1 = ulChipId / 100000000;
	ulPart2 = (ulChipId / 10000) % 10000;
	ulPart3 = ulChipId % 10000;
	snprintf(pszChipId, ulSize, "{\"str\":\"%03d %04d %04d\"}", ulPart1, ulPart2, ulPart3);

	return err;
}
#endif	// End of defined(CONFIG_MW_CAS_CONAX_CPM)

STATIC BUS_Result_t	xmgr_cas_CxUiMenuGetAboutItems(XmgrCas_CxAboutMenuItem_t *astAboutItem)
{
	HERROR						hError = ERR_OK;
	CxVersionInfo_t				stCxtInfo;
	XmgrCas_CxAboutMenuItem_t	*pstItem = NULL;
	HUINT32	ulTotalNum;
	HINT32	i = 0;

	ulTotalNum = eCasCx_AboutItem_Max;

	HxSTD_memset(&stCxtInfo, 0, sizeof(stCxtInfo));
	hError = xmgr_cas_CxGetVersionInfo(CX_SC_SLOT_0, &stCxtInfo);
	if (hError != ERR_OK)
	{
		hError = xmgr_cas_CxGetVersionInfo(CX_SC_SLOT_0, &stCxtInfo);
		if (hError != ERR_OK)
		{
			// Dialog Destroy : Please Wait
			HxLOG_Print("==[xmgr_cas_CxAboutInitAboutItems]== xmgr_cas_CxGetGetVersionInfo() Error!\n");
			return ERR_FAIL;
		}
	}

	for (i = 0; i < ulTotalNum; i++)
	{
		pstItem = &(astAboutItem[i]);
		if(pstItem != NULL)
		{
			HxSTD_memset(pstItem, 0, sizeof(XmgrCas_CxAboutMenuItem_t));
			pstItem->eItemId = i;
			pstItem->bItemDisable = FALSE;

			memset(pstItem->szItemValue, 0, CX_ABOUT_ITEM_TEXT_LENGTH);

			switch(i)
			{
				case eCasCx_AboutItem_Interface_Version:		/* Interface Version */
					HxSTD_snprintf(pstItem->szItemName, sizeof(pstItem->szItemName), (HUINT8*)"{\"id\":\"LOC_INTERFACE_VERSION_ID\"}");
					HxSTD_snprintf(pstItem->szItemValue, sizeof(pstItem->szItemValue), (HUINT8*)"{\"str\":\"0x%02x\"}", CX_SC_CX6_VERSION);
					break;

				case eCasCx_AboutItem_Card_Number:				/* Card Number(Decimal) - 11 Digits(3+4+4)*/
					HxSTD_snprintf(pstItem->szItemName, sizeof(pstItem->szItemName), (HUINT8*)"{\"id\":\"LOC_CARD_NUMBER_ID\"}");
					xmgr_cas_CxUiMenuGetCardNumber(stCxtInfo.ulCardNumber, pstItem->szItemValue, MGR_CAS_STR_LENGTH_LONG);
					break;

				#if defined(CONFIG_MW_CAS_CONAX_CPM)
				case eCasCx_AboutItem_Chip_ID:					/* Chip ID */
					HxSTD_snprintf(pstItem->szItemName, sizeof(pstItem->szItemName), (HUINT8*)"{\"id\":\"LOC_CHIP_ID_ID\"}");
					xmgr_cas_CxUiMenuGetChipId(pstItem->szItemValue, CX_ABOUT_ITEM_TEXT_LENGTH);
					break;
				#endif	// End of defined(CONFIG_MW_CAS_CONAX_CPM)

				case eCasCx_AboutItem_Session_Number:			/* Session Number(Decimal) */
					HxSTD_snprintf(pstItem->szItemName, sizeof(pstItem->szItemName), (HUINT8*)"{\"id\":\"LOC_NUMBER_OF_SESSIONS_ID\"}");
					HxSTD_snprintf(pstItem->szItemValue, sizeof(pstItem->szItemValue), (HUINT8*)"{\"str\":\"%d\"}", stCxtInfo.nSessionCnt);
					break;

				case eCasCx_AboutItem_Language:					/* Language(Decimal) - 1-3 digits*/
					HxSTD_snprintf(pstItem->szItemName, sizeof(pstItem->szItemName), (HUINT8*)"{\"id\":\"LOC_LANGUAGE_ID\"}");
					HxSTD_snprintf(pstItem->szItemValue, sizeof(pstItem->szItemValue), (HUINT8*)"{\"str\":\"%d\"}", stCxtInfo.nCountryInd);
					break;

				case eCasCx_AboutItem_System_ID:				/* CAS System ID - Hexadecimal */
					HxSTD_snprintf(pstItem->szItemName, sizeof(pstItem->szItemName), (HUINT8*)"{\"id\":\"LOC_CA_SYS_ID_ID\"}");
					HxSTD_snprintf(pstItem->szItemValue, sizeof(pstItem->szItemValue), (HUINT8*)"{\"str\":\"0x%04X\"}", stCxtInfo.nCaSysId);
					break;

				default:
					HxLOG_Critical("\n\n");
					return ERR_FAIL;
			}
		}
	}

	// Dialog Destroy : Please Wait

	return ERR_OK;
}

STATIC HERROR xmgr_cas_CxUiMenuPostConaxRootMenu(XmgrCas_CxUiMenuContext_t *pstContext, HUINT16 usSlotId)
{
	HERROR						hError = ERR_OK;
	HUINT32						ulMenuItemLoop = 0;
	BUS_Result_t				hBusResult = ERR_BUS_NO_ERROR;
	OxMgrCasUiEvtMenulist_t		*pstEvent = NULL;
	OxMgrCasUiMenulist_t		*pstMenuList = NULL;
	OxMgrCasUiMenuItem_t		*pMenuItem = NULL;
	HINT32						slStream;
	HCHAR						*pzEncodedStr = NULL;
	HCHAR						*pzProperties = NULL;
	HUINT8						*pszListItem = NULL;

	pstEvent = (OxMgrCasUiEvtMenulist_t *)HLIB_STD_MemCalloc(sizeof(OxMgrCasUiEvtMenulist_t));
	if(pstEvent == (OxMgrCasUiEvtMenulist_t *)NULL)
	{
		HxLOG_Error("\n\n");
		hError = ERR_FAIL;
		goto END_FUNC;
	}

	pstEvent->stSessionInfo.eSessionType = pstContext->stSessionInfo.eSessionType;
	pstEvent->stSessionInfo.ulUiHandle = (HUINT32)xproc_cas_CxUiMenu;
	pstEvent->stSessionInfo.hSession = pstContext->stSessionInfo.hSession;

	pstMenuList = &(pstEvent->stMenulist);

	pstMenuList->stHeader.ulAttr |= eMgrCasUiAttr_Selectable;
	pstMenuList->stHeader.ulAttr |= eMgrCasUiAttr_Title;
	pstMenuList->stHeader.ulFocusIndex = 0;
	pstMenuList->stHeader.ulStartIndex = 0;
	pstMenuList->ulTotalNum = eCasCXMenuItem_Max - 1;

	// Set Title...
	{
		slStream = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
		HLIB_RWSTREAM_Print(slStream, "[{\"id\":\"LOC_CONDITIONAL_ACCESS_CAP_ID\"}]", pzEncodedStr);
		pzProperties = (HCHAR*)HLIB_RWSTREAM_CloseWithoutBuf(slStream);

		MWC_UTIL_DvbStrncpy(pstMenuList->stHeader.szTitle, pzProperties, MGR_CAS_STR_LENGTH_LONG);
		HLIB_STD_MemFree(pzProperties);
	}

	HxLOG_Debug("ulTotalNum %d\n", pstMenuList->ulTotalNum);
	pstMenuList->astListItem = (OxMgrCasUiMenuItem_t *)HLIB_STD_MemCalloc(sizeof(OxMgrCasUiMenuItem_t)*(pstMenuList->ulTotalNum));
	if(pstMenuList->astListItem == NULL)
	{
		HxLOG_Error("\n\n");
		hError = ERR_FAIL;
		goto END_FUNC;
	}

	for(ulMenuItemLoop = 0;ulMenuItemLoop < pstMenuList->ulTotalNum;ulMenuItemLoop++)
	{
		pMenuItem = &pstMenuList->astListItem[ulMenuItemLoop];

		pMenuItem->ulIndex = ulMenuItemLoop;
		pMenuItem->ulType = eMgrCasUiItemType_Str;
		pMenuItem->ulAttr = 0;		// S/C가 삽입되어 있는 경우에는 모두 enable 이다.

		slStream = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);

		switch(ulMenuItemLoop+1)	// +1을 한 이유: XmgrCas_CxCaMenuItem_e의 0번 값인 eCasCXMenuItem_None에 대해서는 메뉴를 그릴 것이 없기 때문
		{
			case eCasCXMenuItem_Subscription:
				if(pstContext->bScInserted == FALSE)
					pMenuItem->ulAttr |= eMgrCasUiAttr_Disabled;
				HLIB_RWSTREAM_Print(slStream, "[{\"id\":\"LOC_SUBSCRIPTION_STATUS_ID\"}]", pzEncodedStr);
				break;
			case eCasCXMenuItem_Event:
				if(pstContext->bScInserted == FALSE)
					pMenuItem->ulAttr |= eMgrCasUiAttr_Disabled;
				HLIB_RWSTREAM_Print(slStream, "[{\"id\":\"LOC_EVENT_STATUS_ID\"}]", pzEncodedStr);
				break;
			case eCasCXMenuItem_Tokens:
				if(pstContext->bScInserted == FALSE)
					pMenuItem->ulAttr |= eMgrCasUiAttr_Disabled;
				HLIB_RWSTREAM_Print(slStream, "[{\"id\":\"LOC_TOKENS_STATUS_ID\"}]", pzEncodedStr);
				break;
			case eCasCXMenuItem_Change_Pin:
				if(pstContext->bScInserted == FALSE)
					pMenuItem->ulAttr |= eMgrCasUiAttr_Disabled;
				HLIB_RWSTREAM_Print(slStream, "[{\"id\":\"LOC_CHANGE_CA_PIN_ID\"}]", pzEncodedStr);
				break;
			case eCasCXMenuItem_Maturity_Rating:
				if(pstContext->bScInserted == FALSE)
					pMenuItem->ulAttr |= eMgrCasUiAttr_Disabled;
				HLIB_RWSTREAM_Print(slStream, "[{\"id\":\"LOC_MATURITY_RATING_ID\"}]", pzEncodedStr);
				break;
#if defined(CONFIG_MW_CAS_CONAX_TEXT)
			case eCasCXMenuItem_Text_View:
				HLIB_RWSTREAM_Print(slStream, "[{\"id\":\"LOC_MESSAGES_ID\"}]", pzEncodedStr);
				break;
#endif
			case eCasCXMenuItem_About:
				HLIB_RWSTREAM_Print(slStream, "[{\"id\":\"LOC_ABOUT_CONAX_CA_ID\"}]", pzEncodedStr);
				break;
			default:
				HxLOG_Critical("ERROR!! Unknown CX Menu Item...[%d]... Check it!!\n", ulMenuItemLoop+1);
				break;
		}
		pzProperties = (HCHAR*)HLIB_RWSTREAM_CloseWithoutBuf(slStream);

		MWC_UTIL_DvbStrncpy(pMenuItem->utData.stString.szString, pzProperties, MGR_CAS_STR_LENGTH_LONG);
		pMenuItem->utData.stString.ulStringLength = SvcCas_UtilStrLen(pzProperties);
		pMenuItem->utData.stString.szString[MGR_CAS_STR_LENGTH_LONG - 1] = '\0';
		HxLOG_Info("[%s]...\n", pMenuItem->utData.stString.szString);
		HLIB_STD_MemFree(pzProperties);
	}

	hBusResult = BUS_PostMessageToOM(NULL, eMEVT_CASUI_TYPE_MENULIST_DATA, HANDLE_NULL, (HINT32)eDxCAS_GROUPID_CX, (HINT32)usSlotId, (HINT32)pstEvent);
	if(hBusResult != ERR_BUS_NO_ERROR)
	{
		HxLOG_Error("\n\n");
		hError = ERR_FAIL;
		goto END_FUNC;
	}

END_FUNC:
	if(hError == ERR_FAIL)
	{
		if(pstEvent != NULL)
		{
			if(pstMenuList->astListItem != NULL)
			{
				HLIB_STD_MemFree(pstMenuList->astListItem);
			}
			HLIB_STD_MemFree(pstEvent);
		}
	}

	if(pszListItem != NULL)
	{
		HLIB_STD_MemFree(pszListItem);
	}

	return hError;
}


STATIC HERROR xmgr_cas_CxUiMenuPostAboutConaxListData(XmgrCas_CxUiMenuContext_t *pstContext, HUINT16 usSlotId)
{
	HERROR						hError = ERR_OK;
	HUINT8						*pszText = NULL;
	HUINT32						ulMenuItemLoop = 0;
	BUS_Result_t				hBusResult = ERR_BUS_NO_ERROR;
	OxMgrCasUiEvtMenulist_t		*pstEvent = NULL;
	OxMgrCasUiMenulist_t		*pstMenuList = NULL;
	OxMgrCasUiMenuItem_t		*pMenuItem = NULL;
	HINT32						slStream;
	HCHAR						*pzEncodedStr = NULL;
	HCHAR						*pzProperties = NULL;
	HUINT8						*pszListItem = NULL;

	hError = xmgr_cas_CxUiMenuGetAboutItems(pstContext->astAboutMenuItem);

	pstEvent = (OxMgrCasUiEvtMenulist_t *)HLIB_STD_MemCalloc(sizeof(OxMgrCasUiEvtMenulist_t));
	if(pstEvent == (OxMgrCasUiEvtMenulist_t *)NULL)
	{
		HxLOG_Error("\n\n");
		hError = ERR_FAIL;
		goto END_FUNC;
	}

	pstEvent->stSessionInfo.eSessionType = pstContext->stSessionInfo.eSessionType;
	pstEvent->stSessionInfo.ulUiHandle = (HUINT32)xproc_cas_CxUiMenu;
	pstEvent->stSessionInfo.hSession = pstContext->stSessionInfo.hSession;

	pstMenuList = &(pstEvent->stMenulist);

	pstMenuList->stHeader.ulAttr |= eMgrCasUiAttr_Title;
	pstMenuList->stHeader.ulFocusIndex = 0;
	pstMenuList->stHeader.ulStartIndex = 0;
	pstMenuList->ulTotalNum = eCasCx_AboutItem_Max;

	// Set Title...
	{
		slStream = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
		HLIB_RWSTREAM_Print(slStream, "[{\"id\":\"LOC_ABOUT_CONAX_CA_ID\"}]", pzEncodedStr);
		pzProperties = (HCHAR*)HLIB_RWSTREAM_CloseWithoutBuf(slStream);

		MWC_UTIL_DvbStrncpy(pstMenuList->stHeader.szTitle, pzProperties, MGR_CAS_STR_LENGTH_LONG);
		HLIB_STD_MemFree(pzProperties);
	}

	HxLOG_Debug("ulTotalNum %d\n", pstMenuList->ulTotalNum);
	pstMenuList->astListItem = (OxMgrCasUiMenuItem_t *)HLIB_STD_MemCalloc(sizeof(OxMgrCasUiMenuItem_t)*(pstMenuList->ulTotalNum));
	if(pstMenuList->astListItem == NULL)
	{
		HxLOG_Error("\n\n");
		hError = ERR_FAIL;
		goto END_FUNC;
	}

	pszListItem = (HUINT8*)OxAP_Malloc(TEXT_SIZE512);
	for(ulMenuItemLoop = 0;ulMenuItemLoop < pstMenuList->ulTotalNum;ulMenuItemLoop++)
	{
		HxSTD_snprintf(pszListItem, TEXT_SIZE512, (HUINT8*)"%s,%s", (HUINT8*)pstContext->astAboutMenuItem[ulMenuItemLoop].szItemName, (HUINT8*)pstContext->astAboutMenuItem[ulMenuItemLoop].szItemValue);
		HxLOG_Info("[%s]...\n", pszListItem);

		pMenuItem = &pstMenuList->astListItem[ulMenuItemLoop];

		pMenuItem->ulIndex = ulMenuItemLoop;
		pMenuItem->ulAttr = 0;
		pMenuItem->ulType = eMgrCasUiItemType_Str;

		MWC_UTIL_ConvSiStringToUtf8(NULL, SvcCas_UtilStrLen(pszListItem), pszListItem, &pszText);
		if(pszText != NULL)
		{
			pMenuItem->utData.stString.ulStringLength = SvcCas_UtilStrLen(pszText);

			slStream = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
			pzEncodedStr = xmgr_cas_CxUiMenuEncodeString(pszText);
			if(pzEncodedStr != NULL)
			{
				HLIB_RWSTREAM_Print(slStream, "[\"%s\"]", pzEncodedStr);
				HLIB_STD_MemFree(pzEncodedStr);
				pzEncodedStr = NULL;
			}

			pzProperties = (HCHAR*)HLIB_RWSTREAM_CloseWithoutBuf(slStream);

			MWC_UTIL_DvbStrncpy(pMenuItem->utData.stString.szString, pzProperties, MGR_CAS_STR_LENGTH_LONG);
			pMenuItem->utData.stString.szString[MGR_CAS_STR_LENGTH_LONG - 1] = '\0';
			HxLOG_Info("[%s]...\n", pMenuItem->utData.stString.szString);
			HLIB_STD_MemFree(pzProperties);

			HLIB_STD_MemFree(pszText);
		}
		HxLOG_Debug("(%d) %s\n", pMenuItem->ulIndex, pMenuItem->utData.stString.szString);
	}

	hBusResult = BUS_PostMessageToOM(NULL, eMEVT_CASUI_TYPE_MENULIST_DATA, HANDLE_NULL, (HINT32)eDxCAS_GROUPID_CX, (HINT32)usSlotId, (HINT32)pstEvent);
	if(hBusResult != ERR_BUS_NO_ERROR)
	{
		HxLOG_Error("\n\n");
		hError = ERR_FAIL;
		goto END_FUNC;
	}

END_FUNC:
	if(hError == ERR_FAIL)
	{
		if(pstEvent != NULL)
		{
			if(pstMenuList->astListItem != NULL)
			{
				HLIB_STD_MemFree(pstMenuList->astListItem);
			}
			HLIB_STD_MemFree(pstEvent);
		}
	}

	if(pszListItem != NULL)
	{
		HLIB_STD_MemFree(pszListItem);
	}

	return hError;
}

STATIC HUINT32	xmgr_cas_CxUiMenuGetPincodeFromString(HUINT8 *szPincode)
{
	HUINT8		*p;
	HUINT32 	nPinCode;

	for (nPinCode = 0, p = szPincode; *p ; p++)
	{
		nPinCode *= 10;
		nPinCode += ((*p)-'0');
	}

	return nPinCode;
}

STATIC HERROR	xmgr_cas_CxUiMenuChangeSmartCardPin(HUINT8* szOldPincode, HUINT8* szNewPincode)
{
	HERROR		hPinStatus = ERR_OK;
	SvcCas_CtrlParam_t			stParam;
	CX_ChangeScPinInParam_t		stInParam;

	if ((szOldPincode == NULL) || (szNewPincode == NULL))
	{
		return	ERR_FAIL;
	}

	stParam.ulControlType = eCACTRL_CX_ChangeScPin;
	stInParam.nScSlot = CX_SC_SLOT_0;
	stInParam.pucOldPin= szOldPincode;
	stInParam.pucNewPin = szNewPincode;
	stParam.pvIn = (void *)&stInParam;
	hPinStatus = SVC_CAS_CtrlSet(eDxCAS_GROUPID_CX, eCACTRL_CX_ChangeScPin, &stParam);
	HxLOG_Print("hPinStatus[%x] \n", hPinStatus);

#if defined(CONFIG_MW_CAS_CONAX_MR_RESERVATION)
	if (hPinStatus == ERR_OK)
	{
		xmgr_cas_CxOperateCachedPin(szNewPincode);
	}
#endif

	return	hPinStatus;
}

STATIC HERROR	xmgr_cas_CxUiMenuGetDebitData(XmgrCas_CxUiMenuSubItemContext_t *pstContents)
{
	CxStatusList_t *pstStatusList = NULL, *pstStatusListTmp = NULL;
	HERROR		hError = ERR_FAIL;
	HUINT8		*szText = NULL, szTemp[TEXT_SIZE128] = {0,}, szStr1[MAX_ELEMENT_TEXT_LENGTH] = {0,};
	HxDATETIME_Date_t	stDate;
	HxDATETIME_Time_t	stTime;
	HBOOL		bFound = FALSE;

	HxSTD_StrNCpy(pstContents->szTitle, "[{\"id\":\"LOC_PURSE_DEBIT_STATUS_ID\"}]", MAX_ELEMENT_TEXT_LENGTH);
	HxSTD_StrNCpy(pstContents->szColumnTitle,
		"[{\"id\":\"LOC_EVENT_ID\"},{\"id\":\"LOC_TOKENS_ID\"},{\"id\":\"LOC_DATE_ID\"}]", MAX_ELEMENT_TEXT_LENGTH);

	CAS_UTIL_LL_InitList(&pstContents->stTextListBox);

	// To make 'Purse: GoldPurse' ('Purse: <Label>')
	// Get Status Info - Tokens(Purse)
	pstStatusList = xmgr_cas_CxReqStatusInfo(CX_SC_SLOT_0, eCxCaStatus_AllPurse, 0);
	if (pstStatusList == NULL)
	{
		// Retry Get Status Info - Tokens(Purse)
		pstStatusList = (CxStatusList_t*)xmgr_cas_CxReqStatusInfo(CX_SC_SLOT_0, eCxCaStatus_AllPurse, 0);
		if (pstStatusList == NULL)
		{
			HxLOG_Print("==[xmgr_cas_CxStatusListLoadItem_TokensData]== xmgr_cas_CxReqStatusInfo() Empty Data .\n");
			return ERR_FAIL;
		}
	}

	pstStatusListTmp = pstStatusList;
	while (pstStatusListTmp)
	{
		if(pstContents->stPurseStatus.usPurseRef == pstStatusListTmp->stStatus.stPurse.usPurseRef)
		{
			bFound = TRUE;
			xmgr_cas_CxUiMenuSetJsonStr(szStr1, pstStatusListTmp->stStatus.stPurse.szLabel);
			HxSTD_snprintf((char*)szTemp, TEXT_SIZE128, (char*)"[{\"id\":\"LOC_PURSE_ID\",\"str\":\": %s\"}]", szStr1);

			szText = (HUINT8*)OxAP_Malloc(SvcCas_UtilStrLen(szTemp) + 1);
			MWC_UTIL_DvbStrcpy(szText, szTemp);

			hError = CAS_UTIL_LL_AddItem(&(pstContents->stTextListBox), szText);
			if(hError != ERR_OK)
			{
				/* 시스템에 의한 메모리 조작이 아니면 절대 발생하지 않을 듯, Linux only */
				HxLOG_Critical("\n\n");
				return ERR_FAIL;
			}

			pstContents->ulTotalNum++;
			szText = NULL;

			break;
		}

		pstStatusListTmp = pstStatusListTmp->pstNext;
	}

	if(bFound == FALSE)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	// Get Data
	// Get Status Info - Purse Debit
	pstStatusList = xmgr_cas_CxReqStatusInfo(CX_SC_SLOT_0, eCxCaStatus_Debit, pstContents->stPurseStatus.usPurseRef);
	if (pstStatusList == NULL)
	{
		// Get Status Info - Purse Debit
		pstStatusList = xmgr_cas_CxReqStatusInfo(CX_SC_SLOT_0, eCxCaStatus_Debit, pstContents->stPurseStatus.usPurseRef);
		if (pstStatusList == NULL)
		{
			HxLOG_Print("==[xmgr_cas_CxStatusListLoadItem_DebitData]== xmgr_cas_CxReqStatusInfo() Empty Data .\n");
			return ERR_FAIL;
		}
	}

	pstStatusListTmp = pstStatusList;

	/*
		============ CONAX Spec ============
		- Support a minimun of 100 entries.
	*/

	// make String
	while (pstStatusListTmp)
	{
		stDate.usYear = pstStatusListTmp->stStatus.stDebit.stLastAccessTime.usYear;
		stDate.ucMonth = pstStatusListTmp->stStatus.stDebit.stLastAccessTime.ucMonth;
		stDate.ucDay = pstStatusListTmp->stStatus.stDebit.stLastAccessTime.ucDay;
		stTime.ucHour = pstStatusListTmp->stStatus.stDebit.stLastAccessTime.ucHour;
		stTime.ucMinute = pstStatusListTmp->stStatus.stDebit.stLastAccessTime.ucMinute;

		xmgr_cas_CxUiMenuSetJsonStr(szStr1, pstStatusListTmp->stStatus.stDebit.szLabel);
		HxSTD_snprintf((char*)szTemp, TEXT_SIZE128, (char*)"[{\"str\":\"%s\"},{\"str\":\"%d\"},{\"str\":\"%02d.%02d.%04d %02d:%02d\"}]",
						 szStr1, pstStatusListTmp->stStatus.stDebit.ulConsumedTokens,
						stDate.ucDay, stDate.ucMonth, stDate.usYear, stTime.ucHour, stTime.ucMinute);

		szText = (HUINT8*)OxAP_Malloc(SvcCas_UtilStrLen(szTemp) + 1);
		MWC_UTIL_DvbStrcpy(szText, szTemp);

		/* linked list에 배열의 원소를 하나하나 리스트로 포인팅 시킨다. */
		hError = CAS_UTIL_LL_AddItem(&(pstContents->stTextListBox), szText);
		if(hError != ERR_OK)
		{
			/* 시스템에 의한 메모리 조작이 아니면 절대 발생하지 않을 듯, Linux only */
			HxLOG_Critical("\n\n");
			return ERR_FAIL;
		}
		pstContents->ulTotalNum++;

		szText = NULL;

		pstStatusListTmp = pstStatusListTmp->pstNext;
	}

	return ERR_OK;
}

STATIC HERROR	xmgr_cas_CxUiMenuGetCreditData(XmgrCas_CxUiMenuSubItemContext_t *pstContents)
{
	CxStatusList_t *pstStatusList = NULL, *pstStatusListTmp = NULL;
	HERROR		hError = ERR_FAIL;
	HUINT8		*szText = NULL, szTemp[TEXT_SIZE128] = {0,}, szStr1[MAX_ELEMENT_TEXT_LENGTH] = {0,};
	HBOOL		bFound = FALSE;

	HxSTD_StrNCpy(pstContents->szTitle, "[{\"id\":\"LOC_PURSE_CREDIT_STATUS_ID\"}]", MAX_ELEMENT_TEXT_LENGTH);
	HxSTD_StrNCpy(pstContents->szColumnTitle,
		"[{\"id\":\"LOC_LABEL_ID\"},{\"id\":\"LOC_TOKENS_ID\"}]", MAX_ELEMENT_TEXT_LENGTH);

	CAS_UTIL_LL_InitList(&pstContents->stTextListBox);

	// To make 'Purse: GoldPurse' ('Purse: <Label>')
	// Get Status Info - Tokens(Purse)
	pstStatusList = xmgr_cas_CxReqStatusInfo(CX_SC_SLOT_0, eCxCaStatus_AllPurse, 0);
	if (pstStatusList == NULL)
	{
		// Retry Get Status Info - Tokens(Purse)
		pstStatusList = (CxStatusList_t*)xmgr_cas_CxReqStatusInfo(CX_SC_SLOT_0, eCxCaStatus_AllPurse, 0);
		if (pstStatusList == NULL)
		{
			HxLOG_Print("==[xmgr_cas_CxStatusListLoadItem_TokensData]== xmgr_cas_CxReqStatusInfo() Empty Data .\n");
			return ERR_FAIL;
		}
	}

	HxLOG_Info("Purse Ref to search:[%d]...\n", pstContents->stPurseStatus.usPurseRef);
	pstStatusListTmp = pstStatusList;
	while (pstStatusListTmp)
	{
		HxLOG_Info("Purse Ref of pstStatusListTmp:[%d]...\n", pstStatusListTmp->stStatus.stPurse.usPurseRef);
		if(pstContents->stPurseStatus.usPurseRef == pstStatusListTmp->stStatus.stPurse.usPurseRef)
		{
			HxLOG_Trace();
			bFound = TRUE;
			HxLOG_Info("szLabel:[%s]...\n", pstStatusListTmp->stStatus.stPurse.szLabel);
			xmgr_cas_CxUiMenuSetJsonStr(szStr1, pstStatusListTmp->stStatus.stPurse.szLabel);
			HxLOG_Info("szStr1:[%s]...\n", pstStatusListTmp->stStatus.stPurse.szLabel);
			HxSTD_snprintf((char*)szTemp, TEXT_SIZE128, (char*)"[{\"id\":\"LOC_PURSE_ID\",\"str\":\": %s\"}]", szStr1);

			szText = (HUINT8*)OxAP_Malloc(SvcCas_UtilStrLen(szTemp) + 1);
			MWC_UTIL_DvbStrcpy(szText, szTemp);

			hError = CAS_UTIL_LL_AddItem(&(pstContents->stTextListBox), szText);
			if(hError != ERR_OK)
			{
				/* 시스템에 의한 메모리 조작이 아니면 절대 발생하지 않을 듯, Linux only */
				HxLOG_Critical("\n\n");
				return ERR_FAIL;
			}

			pstContents->ulTotalNum++;

			szText = NULL;

			break;
		}
		pstStatusListTmp = pstStatusListTmp->pstNext;
	}

	if(bFound == FALSE)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	// Get Status Info - Purse Credit
	pstStatusList = xmgr_cas_CxReqStatusInfo(CX_SC_SLOT_0, eCxCaStatus_Credit, pstContents->stPurseStatus.usPurseRef);
	if (pstStatusList == NULL)
	{
		// Retry Get Status Info - Purse Credit
		pstStatusList = xmgr_cas_CxReqStatusInfo(CX_SC_SLOT_0, eCxCaStatus_Credit, pstContents->stPurseStatus.usPurseRef);
	}

	pstStatusListTmp = pstStatusList;

	/*
		============ CONAX Spec ============
		- Support a minimun of 20 entries.
	*/

	// make String
	while (pstStatusListTmp)
	{
		xmgr_cas_CxUiMenuSetJsonStr(szStr1, pstStatusListTmp->stStatus.stCredit.szLabel);
		HxSTD_snprintf((HUINT8*)szTemp, sizeof(szTemp), (HUINT8*)"[{\"str\":\"%s\"},{\"str\":\"%d\"}]",
			szStr1, pstStatusListTmp->stStatus.stCredit.ulCreditTokens);

		szText = (HUINT8*)OxAP_Malloc(SvcCas_UtilStrLen(szTemp) + 1);
		MWC_UTIL_DvbStrcpy(szText, szTemp);

		/* linked list에 배열의 원소를 하나하나 리스트로 포인팅 시킨다. */
		hError = CAS_UTIL_LL_AddItem(&(pstContents->stTextListBox), szText);
		if(hError != ERR_OK)
		{
			/* 시스템에 의한 메모리 조작이 아니면 절대 발생하지 않을 듯, Linux only */
			HxLOG_Critical("\n\n");
			return ERR_FAIL;
		}
		pstContents->ulTotalNum++;

		szText = NULL;

		pstStatusListTmp = pstStatusListTmp->pstNext;
	}

	return ERR_OK;
}


STATIC HERROR	xmgr_cas_CxUiMenuGetSubscriptionData(XmgrCas_CxUiMenuSubItemContext_t *pstContents)
{
	CxStatusList_t *pstStatusList = NULL, *pstStatusListTmp = NULL;
	CxStatusList_t 	stList;
	HERROR		hError = ERR_FAIL;
	HUINT8		*szText = NULL, szStartDate[TEXT_SIZE64] = {0,}, szEndDate[TEXT_SIZE64] = {0,};
	HUINT8		*pucEntitlement;
	HUINT8		szTemp[MAX_ELEMENT_TEXT_LENGTH] = {0,}, szStr1[MAX_ELEMENT_TEXT_LENGTH] = {0,};
	HxDATETIME_Date_t	stStartDate, stEndDate;

	pstStatusList = &stList;
	HxLOG_Trace();

	HxSTD_memset(pstContents, 0, sizeof(XmgrCas_CxUiMenuSubItemContext_t));
	pstContents->ulTotalNum = 0;
	HxSTD_StrNCpy(pstContents->szTitle, "[{\"id\":\"LOC_SUBSCRIPTION_STATUS_ID\"}]", MAX_ELEMENT_TEXT_LENGTH);
	HxSTD_StrNCpy(pstContents->szColumnTitle,
		"[{\"id\":\"LOC_CAS_IR_SC_SSS_NAME_ITEM_ID\"},{\"id\":\"LOC_CAS_IR_SC_UC_PRODUCT_STATUS_STARTDATE_ID\"},{\"id\":\"LOC_END_DATE_ID\"}]", MAX_ELEMENT_TEXT_LENGTH);

	// Get Status Info - Subscription
	pstStatusList = xmgr_cas_CxReqStatusInfo(CX_SC_SLOT_0, eCxCaStatus_AllSubscription, 0);
	if (pstStatusList == NULL)
	{
		// Retry Get Status Info - Subscription
		pstStatusList = xmgr_cas_CxReqStatusInfo(CX_SC_SLOT_0, eCxCaStatus_AllSubscription, 0); //CX_SC_STATUS_REF);
		if (pstStatusList == NULL)
		{
			HxLOG_Print("==[xmgr_cas_CxStatusListLoadItem_SubscriptionData]== xmgr_cas_CxReqStatusInfo() Empty Data .\n");
			return ERR_FAIL;
		}
	}

	pstStatusListTmp = pstStatusList;
	HxLOG_Print("\t=====================================\n");
	HxLOG_Print("\t Name         Start Date / End Date \n");

	/*
		============ CONAX Spec ============
		- Four octer_str_byte : Displayed in hex nataion
		- Subscription_ref : Not be displayed
		- Supoort a minimun of 40 entries.
	*/

	CAS_UTIL_LL_InitList(&pstContents->stTextListBox);

	// make String
	while (pstStatusListTmp)
	{
		/* __________________________________________________________________*/
		// Label
		xmgr_cas_CxUiMenuSetJsonStr(szStr1, pstStatusListTmp->stStatus.stSubscription.szLabel);

		// Start Date
		stStartDate.usYear = pstStatusListTmp->stStatus.stSubscription.stStartDate[0].usYear;
		stStartDate.ucMonth = pstStatusListTmp->stStatus.stSubscription.stStartDate[0].ucMonth;
		stStartDate.ucDay = pstStatusListTmp->stStatus.stSubscription.stStartDate[0].ucDay;

		// Get Start Date Month String
		hError = xmgr_cas_CxGetMonthString(stStartDate.ucMonth, szStartDate);
		if( hError != ERR_OK)
		{
			HxLOG_Critical("\n\n");
		}

		// End Date
		stEndDate.usYear = pstStatusListTmp->stStatus.stSubscription.stEndDate[0].usYear;
		stEndDate.ucMonth = pstStatusListTmp->stStatus.stSubscription.stEndDate[0].ucMonth;
		stEndDate.ucDay = pstStatusListTmp->stStatus.stSubscription.stEndDate[0].ucDay;

		// Get End Date Month String
		hError = xmgr_cas_CxGetMonthString(stEndDate.ucMonth, szEndDate);
		if( hError != ERR_OK)
		{
			HxLOG_Critical("\n\n");
		}

		// Entitlement 1
		pucEntitlement = pstStatusListTmp->stStatus.stSubscription.szEntitlemnet[0];

		// 1ST Info
		HxSTD_snprintf(szTemp, sizeof(szTemp), (HUINT8*)"[{\"str\":\"%s\"},{\"str\":\"%02d %s %04d\"},{\"str\":\"%02d %s %04d\"},{\"str\":\"%02X%02X%02X%02X\"}]",
				szStr1, stStartDate.ucDay, szStartDate, stStartDate.usYear, stEndDate.ucDay, szEndDate, stEndDate.usYear,
				pucEntitlement[0], pucEntitlement[1], pucEntitlement[2], pucEntitlement[3]);
		szText = (HUINT8*)OxAP_Malloc(SvcCas_UtilStrLen(szTemp) + 1);
		MWC_UTIL_DvbStrcpy(szText, szTemp);

		HxLOG_Info("usSubscriptionRef: [%d]...\n", pstStatusListTmp->stStatus.stSubscription.usSubscriptionRef);
		HxLOG_Print("1\t %s|%02d %s %04d|%02d %s %04d|%02X%02X%02X%02X\n", szStr1, stStartDate.ucDay, szStartDate, stStartDate.usYear, stEndDate.ucDay, szEndDate, stEndDate.usYear,
				pucEntitlement[0], pucEntitlement[1], pucEntitlement[2], pucEntitlement[3]);
		/* linked list에 배열의 원소를 하나하나 리스트로 포인팅 시킨다. */
		hError = CAS_UTIL_LL_AddItem(&(pstContents->stTextListBox), szText);
		if(hError != ERR_OK)
		{
			/* 시스템에 의한 메모리 조작이 아니면 절대 발생하지 않을 듯, Linux only */
			HxLOG_Critical("\n\n");
			return ERR_FAIL;
		}
		pstContents->ulTotalNum++;

		/* __________________________________________________________________*/
		szText = NULL;

		/* __________________________________________________________________*/
		// Start Date
		stStartDate.usYear = pstStatusListTmp->stStatus.stSubscription.stStartDate[1].usYear;
		stStartDate.ucMonth = pstStatusListTmp->stStatus.stSubscription.stStartDate[1].ucMonth;
		stStartDate.ucDay = pstStatusListTmp->stStatus.stSubscription.stStartDate[1].ucDay;

		// Get Start Date Month String
		hError = xmgr_cas_CxGetMonthString(stStartDate.ucMonth, szStartDate);
		if( hError != ERR_OK)
		{
			HxLOG_Critical("\n\n");
		}

		// End Date
		stEndDate.usYear = pstStatusListTmp->stStatus.stSubscription.stEndDate[1].usYear;
		stEndDate.ucMonth = pstStatusListTmp->stStatus.stSubscription.stEndDate[1].ucMonth;
		stEndDate.ucDay = pstStatusListTmp->stStatus.stSubscription.stEndDate[1].ucDay;

		// Get End Date Month String
		hError = xmgr_cas_CxGetMonthString(stEndDate.ucMonth, szEndDate);
		if( hError != ERR_OK)
		{
			HxLOG_Critical("\n\n");
		}

		// Entitlement 2
		pucEntitlement = pstStatusListTmp->stStatus.stSubscription.szEntitlemnet[1];

		// 2nd Info
		HxSTD_snprintf(szTemp, sizeof(szTemp), (HUINT8*)"[{\"str\":\"%s\"},{\"str\":\"%02d %s %04d\"},{\"str\":\"%02d %s %04d\"},{\"str\":\"%02X%02X%02X%02X\"}]",
				(HUINT8*)"  ", stStartDate.ucDay, szStartDate, stStartDate.usYear, stEndDate.ucDay, szEndDate, stEndDate.usYear,
				pucEntitlement[0], pucEntitlement[1], pucEntitlement[2], pucEntitlement[3]);

		szText = (HUINT8*)OxAP_Malloc(SvcCas_UtilStrLen(szTemp) + 1);
		MWC_UTIL_DvbStrcpy(szText, szTemp);
		HxLOG_Info("[%s]...\n", szText);

		HxLOG_Print("\t %s|%02d %s %04d|%02d %s %04d|%02X%02X%02X%02X\n", (HUINT8*)"  ", stStartDate.ucDay, szStartDate, stStartDate.usYear, stEndDate.ucDay, szEndDate, stEndDate.usYear,
				pucEntitlement[0], pucEntitlement[1], pucEntitlement[2], pucEntitlement[3]);
		/* linked list에 배열의 원소를 하나하나 리스트로 포인팅 시킨다. */
		hError = CAS_UTIL_LL_AddItem(&(pstContents->stTextListBox), szText);
		if(hError != ERR_OK)
		{
			/* 시스템에 의한 메모리 조작이 아니면 절대 발생하지 않을 듯, Linux only */
			HxLOG_Critical("\n\n");
			return ERR_FAIL;
		}
		pstContents->ulTotalNum++;
		/* __________________________________________________________________*/

		szText = NULL;

		pstStatusListTmp = pstStatusListTmp->pstNext;
	}

	return ERR_OK;
}


STATIC HERROR	xmgr_cas_CxUiMenuGetEventData(XmgrCas_CxUiMenuSubItemContext_t *pstContents)
{
	CxStatusList_t *pstStatusList = NULL, *pstStatusListTmp = NULL;
	CxStatusList_t 	stList;
	HERROR		hError = ERR_FAIL;
	HUINT8		*szText = NULL, szStartDate[TEXT_SIZE64] = {0,}, szEndDate[TEXT_SIZE64] = {0,};
	HUINT8		szTemp[MAX_ELEMENT_TEXT_LENGTH] = {0,}, szStr1[MAX_ELEMENT_TEXT_LENGTH] = {0,}, szStr2[MAX_ELEMENT_TEXT_LENGTH] = {0,};
	HxDATETIME_Date_t	stStartDate, stEndDate;
	HxDATETIME_Time_t	stSTTime, stEDTime;

	HxLOG_Print("[Enter]\r\n");

	pstStatusList = &stList;
	// Dialog Create : Please Wait

	HxSTD_memset(pstContents, 0, sizeof(XmgrCas_CxUiMenuSubItemContext_t));
	pstContents->ulTotalNum = 0;
	HxSTD_StrNCpy(pstContents->szTitle, "[{\"id\":\"LOC_EVENT_STATUS_ID\"}]", MAX_ELEMENT_TEXT_LENGTH);
	HxSTD_StrNCpy(pstContents->szColumnTitle,
		"[{\"id\":\"LOC_CAS_IR_SC_SSS_NAME_ITEM_ID\"},{\"id\":\"LOC_CAS_IR_SC_UC_PRODUCT_STATUS_STARTDATE_ID\"},{\"id\":\"LOC_END_DATE_ID\"},{\"id\":\"LOC_MIN_OR_CREDIT_LEFT_ID\"}]"
		, MAX_ELEMENT_TEXT_LENGTH);

	// Get Status Info - Event
	pstStatusList = xmgr_cas_CxReqStatusInfo(CX_SC_SLOT_0, eCxCaStatus_AllPpvEvent, 0);
	if (pstStatusList == NULL)
	{
		// Retry Get Status Info - Event
		pstStatusList = xmgr_cas_CxReqStatusInfo(CX_SC_SLOT_0, eCxCaStatus_AllPpvEvent, 0);
		if (pstStatusList == NULL)
		{
			HxLOG_Print("==[xmgr_cas_CxStatusListLoadItem_EventData]== xmgr_cas_CxReqStatusInfo() Empty Data .\n");
			return ERR_FAIL;
		}
	}

	pstStatusListTmp = pstStatusList;

	/*
		============ CONAX Spec ============
		- auth_type : 0x00(Time Limited Access), 0x01(Unlimited Access), 0x02(Crdeit base Access)
		- Support a minimun of 100 entries.
	*/

	CAS_UTIL_LL_InitList(&pstContents->stTextListBox);

	// make String
	while (pstStatusListTmp)
	{
		// Label
		xmgr_cas_CxUiMenuSetJsonStr(szStr1, pstStatusListTmp->stStatus.stEvent.szLabel);

		// Start Date
		stStartDate.usYear = pstStatusListTmp->stStatus.stEvent.stStartDate.usYear;
		stStartDate.ucMonth = pstStatusListTmp->stStatus.stEvent.stStartDate.ucMonth;
		stStartDate.ucDay = pstStatusListTmp->stStatus.stEvent.stStartDate.ucDay;
		stSTTime.ucHour = pstStatusListTmp->stStatus.stEvent.stStartDate.ucHour;
		stSTTime.ucMinute = pstStatusListTmp->stStatus.stEvent.stStartDate.ucMinute;
		// Get Start Date Month String
		//resResult = AP_UTIL_GetMonthString(0, stStartDate.ucMonth, &szStartDate);
		hError = xmgr_cas_CxGetMonthString(stStartDate.ucMonth, szStartDate);
		if( hError != ERR_OK)
		{
			HxLOG_Critical("\n\n");
		}

		/* ____________________________________________________________________ */
		// End Date
		stEndDate.usYear = pstStatusListTmp->stStatus.stEvent.stEndDate.usYear;
		stEndDate.ucMonth = pstStatusListTmp->stStatus.stEvent.stEndDate.ucMonth;
		stEndDate.ucDay = pstStatusListTmp->stStatus.stEvent.stEndDate.ucDay;
		stEDTime.ucHour = pstStatusListTmp->stStatus.stEvent.stEndDate.ucHour;
		stEDTime.ucMinute = pstStatusListTmp->stStatus.stEvent.stEndDate.ucMinute;

		// Get End Date Month String
		//resResult = AP_UTIL_GetMonthString(0, stEndDate.ucMonth, &szEndDate);
		hError = xmgr_cas_CxGetMonthString(stEndDate.ucMonth, szEndDate);
		if( hError != ERR_OK)
		{
			HxLOG_Critical("\n\n");
		}

		// Minutes of Credits Left
		if (pstStatusListTmp->stStatus.stEvent.ucAuthType == eCxAuth_TimeLimitedAccess)
		{
			HxSTD_snprintf(szTemp, sizeof(szTemp),
				(HUINT8*)"[{\"str\":\"%s\"},{\"str\":\"%02d %s %04d %02d:%02d\"},{\"str\":\"%02d %s %04d %02d:%02d\"},{\"str\":\"%d \",\"id\":\"LOC_REC_MINLEFT_ID\"}]",
				szStr1,stStartDate.ucDay, szStartDate, stStartDate.usYear, stSTTime.ucHour, stSTTime.ucMinute,
				stEndDate.ucDay, szEndDate, stEndDate.usYear, stEDTime.ucHour, stEDTime.ucMinute, pstStatusListTmp->stStatus.stEvent.usLeft);
		}
		else if (pstStatusListTmp->stStatus.stEvent.ucAuthType == eCxAuth_CreditBasedAccess)
		{
			// TODO: [kkk] Temporal LOC ID 'LOC_CREDIT_ID' should be changed.
			HxSTD_snprintf(szStr2, sizeof(szStr2),
				(HUINT8*)"[{\"str\":\"%s\"},{\"str\":\"%02d %s %04d %02d:%02d\"},{\"str\":\"%02d %s %04d %02d:%02d\"},{\"str\":\"%d \",\"id\":\"LOC_CREDIT_ID\"}]",
				szStr1, stStartDate.ucDay, szStartDate, stStartDate.usYear, stSTTime.ucHour, stSTTime.ucMinute,
				stEndDate.ucDay, szEndDate, stEndDate.usYear, stEDTime.ucHour, stEDTime.ucMinute, pstStatusListTmp->stStatus.stEvent.usLeft);
		}
		else
		{
			// Unlimited Access
			HxSTD_snprintf(szTemp, sizeof(szTemp),
				(HUINT8*)"[{\"str\":\"%s\"},{\"str\":\"%02d %s %04d %02d:%02d\"},{\"str\":\"%02d %s %04d %02d:%02d\"},{\"str\":\" \"}]",
				szStr1,stStartDate.ucDay, szStartDate, stStartDate.usYear, stSTTime.ucHour, stSTTime.ucMinute,
				stEndDate.ucDay, szEndDate, stEndDate.usYear, stEDTime.ucHour, stEDTime.ucMinute);
		}

		szText = (HUINT8*)OxAP_Malloc(SvcCas_UtilStrLen(szTemp) + 1);
		MWC_UTIL_DvbStrcpy(szText, szTemp);
		/* linked list에 배열의 원소를 하나하나 리스트로 포인팅 시킨다. */
		hError = CAS_UTIL_LL_AddItem(&(pstContents->stTextListBox), szText);
		if(hError != ERR_OK)
		{
			/* 시스템에 의한 메모리 조작이 아니면 절대 발생하지 않을 듯, Linux only */
			HxLOG_Critical("\n\n");
			return ERR_FAIL;
		}
		pstContents->ulTotalNum++;

		szText = NULL;

		pstStatusListTmp = pstStatusListTmp->pstNext;
	}

	return ERR_OK;
}


STATIC HERROR	xmgr_cas_CxUiMenuGetTokenData(XmgrCas_CxUiMenuSubItemContext_t *pstContents)
{
	CxStatusList_t *pstStatusList = NULL, *pstStatusListTmp = NULL;
	CxStatusList_t 	stList;
	HERROR		hError = ERR_FAIL;

	HUINT8		*szText = NULL;
	HUINT8		szTemp[TEXT_SIZE128] = {0,}, szStr1[TEXT_SIZE128] = {0,};
	HUINT32 	ulBalance = 0;

	HxLOG_Print("[Enter]\r\n");
	// Dialog Create : Please Wait

	pstStatusList = &stList;
	HxLOG_Trace();

	HxSTD_memset(pstContents, 0, sizeof(XmgrCas_CxUiMenuSubItemContext_t));
	pstContents->ulTotalNum = 0;
	pstContents->ulFocusIndex = 0;
	HxSTD_StrNCpy(pstContents->szTitle, "[{\"id\":\"LOC_TOKENS_STATUS_ID\"}]", MAX_ELEMENT_TEXT_LENGTH);
	HxSTD_StrNCpy(pstContents->szColumnTitle,
		"[{\"id\":\"LOC_PURSE_ID\"},{\"id\":\"LOC_BALANCE_ID\"}]"
		, MAX_ELEMENT_TEXT_LENGTH);

	// Get Status Info - Tokens(Purse)
	pstStatusList = xmgr_cas_CxReqStatusInfo(CX_SC_SLOT_0, eCxCaStatus_AllPurse, 0);
	if (pstStatusList == NULL)
	{
		// Retry Get Status Info - Tokens(Purse)
		pstStatusList = (CxStatusList_t*)xmgr_cas_CxReqStatusInfo(CX_SC_SLOT_0, eCxCaStatus_AllPurse, 0);
		if (pstStatusList == NULL)
		{
			HxLOG_Print("==[xmgr_cas_CxStatusListLoadItem_TokensData]== xmgr_cas_CxReqStatusInfo() Empty Data .\n");
			return ERR_FAIL;
		}
	}

	pstStatusListTmp = pstStatusList;

	/*
		============ CONAX Spec ============
		- Purse_status : Menu above displays two PURSE_STATUS parameters.
		- Select a purse by highlighting it. and then display the purse's credit or debit status.
	*/

	CAS_UTIL_LL_InitList(&pstContents->stTextListBox);

	// make String
	while (pstStatusListTmp)
	{
		// Label
		ulBalance = pstStatusListTmp->stStatus.stPurse.ulBalance;

		if (ulBalance)
		{
			xmgr_cas_CxUiMenuSetJsonStr(szStr1, pstStatusListTmp->stStatus.stPurse.szLabel);
		}

		if (ulBalance < 1000)
		{
			HxSTD_snprintf((HUINT8*)szTemp, sizeof(szTemp), (HUINT8*)"[{\"str\":\"%s\"},{\"str\":\"%d \",\"id\":\"LOC_TOKENS_ID\"},{\"str\":\"%d\"}]",
							szStr1, ulBalance, pstStatusListTmp->stStatus.stPurse.usPurseRef);
		}
		else if (ulBalance < 1000000)
		{
			HxSTD_snprintf((HUINT8*)szTemp, sizeof(szTemp), (HUINT8*)"[{\"str\":\"%s\"},{\"str\":\"%d,%03d \",\"id\":\"LOC_TOKENS_ID\"},{\"str\":\"%d\"}]",
							szStr1, ulBalance/1000, ulBalance%1000, pstStatusListTmp->stStatus.stPurse.usPurseRef);
		}
		else
		{
			HxSTD_snprintf((HUINT8*)szTemp, sizeof(szTemp), (HUINT8*)"[{\"str\":\"%s\"},{\"str\":\"%d,%03d,%03d \",\"id\":\"LOC_TOKENS_ID\"},{\"str\":\"%d\"}]",
							szStr1, ulBalance/1000000, (ulBalance/1000)%1000, ulBalance%1000, pstStatusListTmp->stStatus.stPurse.usPurseRef);
		}

		szText = (HUINT8*)OxAP_Malloc(SvcCas_UtilStrLen(szTemp) + 1);
		MWC_UTIL_DvbStrcpy(szText, szTemp);

		/* linked list에 배열의 원소를 하나하나 리스트로 포인팅 시킨다. */
		hError = CAS_UTIL_LL_AddItem(&(pstContents->stTextListBox), szText);
		if(hError != ERR_OK)
		{
			/* 시스템에 의한 메모리 조작이 아니면 절대 발생하지 않을 듯, Linux only */
			HxLOG_Critical("\n\n");
			return ERR_FAIL;
		}

		pstContents->ulTotalNum++;

		szText = NULL;

		pstStatusListTmp = pstStatusListTmp->pstNext;
	}

	return ERR_OK;
}

STATIC HERROR	xmgr_cas_CxUiMenuGetMaturityRatingData(XmgrCas_CxUiMenuSubItemContext_t *pstContents)
{
	HUINT8		*szText = NULL;
	HUINT8		ucMRValue;
	HERROR		hError = ERR_FAIL;
	HUINT8		szStr[TEXT_SIZE64] = {0,};

	HxLOG_Print("[Enter]\r\n");

	HxSTD_memset(pstContents, 0, sizeof(XmgrCas_CxUiMenuSubItemContext_t));
	HxSTD_StrNCpy(pstContents->szTitle, "[{\"id\":\"LOC_MATURITY_RATING_ID\"}]", MAX_ELEMENT_TEXT_LENGTH);

	/* Get MR value */
	hError = xmgr_cas_CxGetCurMatRatLevel(CX_SC_SLOT_0, &ucMRValue);
	if(hError != ERR_OK)
	{
		HxLOG_Error("xmgr_cas_CxGetCurMatRatLevel() FUNCTION ERROR!!\n");
	}
	HxLOG_Info("Current Maturity Rating: [%d]...\n", ucMRValue);

	switch(ucMRValue)
	{
		case 0x01: // G - General audience
			pstContents->ulFocusIndex = 0;
			break;
		case 0x02: // PG - With parental guidance
			pstContents->ulFocusIndex = 1;
			break;
		case 0x04: // A - Over 18
			pstContents->ulFocusIndex = 2;
			break;
		case 0x08: // X - Erotiv
			pstContents->ulFocusIndex = 3;
			break;
		default:
			HxLOG_Error("ERROR...\n");
			pstContents->ulFocusIndex = 0;
			break;
	}

	CAS_UTIL_LL_InitList(&pstContents->stTextListBox);

	HxSTD_snprintf(szStr, sizeof(szStr), (HUINT8*)"[{\"id\":\"LOC_G_ID\"}]");
	szText = (HUINT8*)OxAP_Malloc(SvcCas_UtilStrLen(szStr) + 1);
	MWC_UTIL_DvbStrcpy(szText, szStr);
	hError = CAS_UTIL_LL_AddItem(&(pstContents->stTextListBox), szText);

	HxSTD_snprintf(szStr, sizeof(szStr), (HUINT8*)"[{\"id\":\"LOC_PG_ID\"}]");
	szText = (HUINT8*)OxAP_Malloc(SvcCas_UtilStrLen(szStr) + 1);
	MWC_UTIL_DvbStrcpy(szText, szStr);
	hError = CAS_UTIL_LL_AddItem(&(pstContents->stTextListBox), szText);

	HxSTD_snprintf(szStr, sizeof(szStr), (HUINT8*)"[{\"id\":\"LOC_A_ID\"}]");
	szText = (HUINT8*)OxAP_Malloc(SvcCas_UtilStrLen(szStr) + 1);
	MWC_UTIL_DvbStrcpy(szText, szStr);
	hError = CAS_UTIL_LL_AddItem(&(pstContents->stTextListBox), szText);

	HxSTD_snprintf(szStr, sizeof(szStr), (HUINT8*)"[{\"id\":\"LOC_X_ID\"}]");
	szText = (HUINT8*)OxAP_Malloc(SvcCas_UtilStrLen(szStr) + 1);
	MWC_UTIL_DvbStrcpy(szText, szStr);
	hError = CAS_UTIL_LL_AddItem(&(pstContents->stTextListBox), szText);

	pstContents->ulTotalNum = 4;

	return ERR_OK;
}


STATIC HERROR	xmgr_cas_CxUiMenuGetMailMessageData(XmgrCas_CxUiMenuSubItemContext_t *pstContents)
{
	HERROR						hError = ERR_FAIL;
	HINT32 						i;
	HUINT32 					ulCnt = 0, ulNumMsg;
	HUINT8						szTemp[TEXT_SIZE512] = {0,};
	HUINT8						*szText = NULL;
	CxUserTextList_t 			stList;
	XmgrCas_CxUiMailItemData_t	*pMailItemData = NULL;

	HxLOG_Info("xmgr_cas_CxUiMenuGetMailMessageData() +\n");

	HxSTD_memset(pstContents, 0, sizeof(XmgrCas_CxUiMenuSubItemContext_t));
	pstContents->ulTotalNum = 0;
	HxSTD_StrNCpy(pstContents->szTitle, "[{\"id\":\"LOC_MESSAGES_ID\"}]", MAX_ELEMENT_TEXT_LENGTH);

	for( i = 0; i < CX_MAX_USERTEXT_SLOT_NUM; i++ )
	{
		HxSTD_memset(&s_astMailItemData[i], 0, sizeof(XmgrCas_CxUiMailItemData_t));
	}

	hError = xmgr_cas_CxGetUserText(&ulNumMsg, &stList);
	if ((hError != ERR_OK) || (ulNumMsg < 1))
	{
		HxLOG_Critical("\n\n");
		pstContents->ulTotalNum = 0;
		return	ERR_FAIL;
	}

	HxLOG_Print("Mail Load Count = %d\n", ulNumMsg);
	ulCnt = 0;
	for (i=0 ; i< ulNumMsg ; i++)
	{
		hError = xmgr_cas_CxUiMenuIsValidMailMessage(stList.pstUserText[i].usDuration);
		if (hError == ERR_OK && (strlen(stList.pstUserText[i].szText) > 0))
		{
			if (pMailItemData == NULL)
			{
				pMailItemData = (XmgrCas_CxUiMailItemData_t*)OxAP_Malloc(sizeof(XmgrCas_CxUiMailItemData_t));
			}
			if (pMailItemData == NULL)
			{
				HxLOG_Critical("\n\n");
				continue;
			}

			HxSTD_memset(pMailItemData, 0, sizeof(XmgrCas_CxUiMailItemData_t));

			pMailItemData->bRead		= stList.pstUserText[i].ucDisplayed;
			pMailItemData->nMailIdx		= stList.pstUserText[i].ucSeqNo;
			HxSTD_memcpy(pMailItemData->strMailTxt, stList.pstUserText[i].szText, CX_MAX_USER_TEXT_LEN);
			pMailItemData->strMailTxt[CX_MAX_USER_TEXT_LEN] = '\0';

			// prevent fix
			snprintf((char*)pMailItemData->strReceivedDate, sizeof(pMailItemData->strReceivedDate), "%02d/%02d", stList.pstUserText[i].stStartDataTime.stDate.ucDay, stList.pstUserText[i].stStartDataTime.stDate.ucMonth);
			snprintf((char*)pMailItemData->strReceivedTime, sizeof(pMailItemData->strReceivedTime), "%02d:%02d",
				stList.pstUserText[i].stStartDataTime.stTime.ucHour, stList.pstUserText[i].stStartDataTime.stTime.ucMinute);

			if(pMailItemData->bRead == TRUE)
			{
				HxSTD_snprintf((HUINT8*)szTemp, sizeof(szTemp), (HUINT8*)"[{\"img\":\"619_SET_Mail_Read_N.png\"},{\"str\":\"%s\"},{\"str\":\"%s\"},{\"str\":\"%s\"},{\"str\":\"%d\"}]",
																	pMailItemData->strReceivedDate,
																	pMailItemData->strReceivedTime,
																	pMailItemData->strMailTxt,
																	pMailItemData->nMailIdx);
			}
			else
			{
				HxSTD_snprintf((HUINT8*)szTemp, sizeof(szTemp), (HUINT8*)"[{\"img\":\"619_SET_Mail_UnRead_N.png\"},{\"str\":\"%s\"},{\"str\":\"%s\"},{\"str\":\"%s\"},{\"str\":\"%d\"}]",
																	pMailItemData->strReceivedDate,
																	pMailItemData->strReceivedTime,
																	pMailItemData->strMailTxt,
																	pMailItemData->nMailIdx);
			}

			szText = (HUINT8*)OxAP_Malloc(SvcCas_UtilStrLen(szTemp) + 1);
			MWC_UTIL_DvbStrcpy(szText, szTemp);

			hError = CAS_UTIL_LL_AddItem(&(pstContents->stTextListBox), szText);
			if (hError != ERR_OK)
			{
				HxLOG_Critical("\n\n");
			}

			HxSTD_memcpy(&s_astMailItemData[ulCnt], pMailItemData, sizeof(XmgrCas_CxUiMailItemData_t));
			if (pMailItemData != NULL)
			{
				OxAP_Free(pMailItemData);
				pMailItemData = NULL;
			}

			ulCnt++;
		}
	}

	pstContents->ulTotalNum = ulCnt;
	HxLOG_Print("Mail Count = %d\n", ulCnt);

	return	ERR_OK;
}

/*
STATIC HERROR	xmgr_cas_CxUiMenuGetMessagesData(XmgrCas_CxUiMenuSubItemContext_t *pstContents)
{
	HERROR		hError = ERR_FAIL;
	HUINT8		*szText = NULL;
	HUINT8		szTemp[TEXT_SIZE128] = {0,}, szStr1[TEXT_SIZE128] = {0,};
	HUINT32 	ulNumMsg, i, ulStrlen;
	CxUserTextList_t pstList;


	HxLOG_Print("[Enter]\r\n");

	pstContents->ulTotalNum = 0;
	HxSTD_StrNCpy(pstContents->szTitle, "Messages", MAX_ELEMENT_TEXT_LENGTH);

	CAS_UTIL_LL_InitList(&pstContents->stTextListBox);

	// Dialog Create : Please Wait

	hError = xmgr_cas_CxGetUserText(&ulNumMsg, &pstList);
	if ((hError != ERR_OK) || (ulNumMsg < 1))
	{
		HxLOG_Critical("\n\n");
		return	ERR_FAIL;
	}

	for (i = 0; i<ulNumMsg; i++)
	{
		ulStrlen = 0;
		ulStrlen = SvcCas_UtilStrLen(pstList.pstUserText[i].szText);
		if (ulStrlen)
		{
			int idx = 0;

			while (idx < ulStrlen)
			{
				memcpy(szStr1, pstList.pstUserText[i].szText + idx, D_TEXT_LINE_LENGTH);
				szStr1[D_TEXT_LINE_LENGTH] = 0;

				if (idx == 0)
					MWC_UTIL_DvbUtf8Sprintf(szTemp, (HUINT8*)"%d|%s", i, szStr1);	// indexed from 0 to 15
				else
					MWC_UTIL_DvbUtf8Sprintf(szTemp, (HUINT8*)" |%s", szStr1);

				idx += D_TEXT_LINE_LENGTH;

				szText = (HUINT8*)OxAP_Malloc(SvcCas_UtilStrLen(szTemp) + 1);
				MWC_UTIL_DvbStrcpy(szText, szTemp);

				// linked list에 배열의 원소를 하나하나 리스트로 포인팅 시킨다.
				hError = CAS_UTIL_LL_AddItem(&(pstContents->stTextListBox), szText);
				if (hError != ERR_OK)
				{
					// 시스템에 의한 메모리 조작이 아니면 절대 발생하지 않을 듯, Linux only
					HxLOG_Critical("\n\n");
				}
				pstContents->ulTotalNum++;
			}
		}
	}

	return ERR_OK;
}
*/
STATIC HERROR	xmgr_cas_CxUiMenuPostMessageSessionEventToOM(XmgrCas_CxUiMenuContext_t *pstContext, HUINT32 ulSlotNumber, HINT32 eEvent)
{
	HERROR								hError = ERR_OK;
	BUS_Result_t						hResult = ERR_BUS_NO_ERROR;
	OxMgrCasUiEvtSession_t				*pstEvent = NULL;

	pstEvent = (OxMgrCasUiEvtSession_t *)HLIB_STD_MemCalloc(sizeof(OxMgrCasUiEvtSession_t));

	pstEvent->eSessionType = pstContext->stSessionInfo.eSessionType;
	pstEvent->ulUiHandle = (HUINT32)xproc_cas_CxUiMenu;
	HxLOG_Info("UiProc: [0x%08x]...\n", pstEvent->ulUiHandle);
	pstEvent->hSession = pstContext->stSessionInfo.hSession;

	hResult = BUS_PostMessageToOM(NULL, eEvent, HANDLE_NULL, eDxCAS_GROUPID_CX, ulSlotNumber, (HINT32)pstEvent);
	if(hResult != ERR_BUS_NO_ERROR)
	{
		hError = ERR_FAIL;
	}

	return hError;
}

STATIC BUS_Result_t xmgr_cas_CxUiMenuOpenSession(XmgrCas_CxUiMenuContext_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	OxMgrCasSessionEvent_t		*pstSession = (OxMgrCasSessionEvent_t *)p3;

	pstContext->eSessionStatus = eCX_UI_SESSION_STATUS_Started;
	pstContext->stSessionInfo.hSession = pstSession->hSession;

	xmgr_cas_CxUiMenuPostMessageSessionEventToOM(pstContext, pstSession->ulCasSlotNum, eMEVT_CASUI_SESSION_OPENED);

	return MESSAGE_BREAK;
}

STATIC BUS_Result_t xmgr_cas_CxUiMenuCloseSession(XmgrCas_CxUiMenuContext_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	pstContext->stSessionInfo.eSessionType = eDxCAS_SESSIONTYPE_NONE;
	pstContext->eSessionStatus = eCX_UI_SESSION_STATUS_Stopped;

	return MESSAGE_BREAK;
}

STATIC HERROR xmgr_cas_CxUiMenuSelectMailMessage(XmgrCas_CxUiMenuContext_t *pstContext, OxMgrCasUiUserInputNotify_t *pstUserInputNotify)
{
	HERROR	hError = ERR_FAIL;

	hError = xmgr_cas_CxSetUserTextDisplayed(pstUserInputNotify->utData.stSelection.ulItemIdx, eCxUTDisplayCntOnce);
	if(hError != ERR_OK)
	{
		HxLOG_Error("xmgr_cas_CxSetUserTextDisplayed() FUNCTION ERROR!!\n");
	}

	return hError;
}

STATIC HERROR xmgr_cas_CxUiMenuSelectMaturityRating(XmgrCas_CxUiMenuContext_t *pstContext, OxMgrCasUiUserInputNotify_t *pstUserInputNotify)
{
	HERROR	hError = ERR_FAIL;

	if(pstUserInputNotify->utData.stSelection.ulDataLength > 0)
	{
		pstContext->stSubItem.hPinStatus = xmgr_cas_CxChangeMaturityRating(CX_SC_SLOT_0, pstUserInputNotify->utData.stSelection.ulItemIdx, pstUserInputNotify->utData.stSelection.aucData);
		HxLOG_Info("hPinStatus: [%d]...\n", pstContext->stSubItem.hPinStatus);

		hError = xmgr_cas_CxUiMenuPostTextData(pstContext, CX_SC_SLOT_0);
		if(hError != ERR_OK)
		{
			HxLOG_Error("xmgr_cas_CxUiMenuPostTextData() FUNCTION ERROR!!\n");
		}
	}
	else
	{
		HxLOG_Critical("Error!!\n");
	}

	return ERR_OK;
}

STATIC HERROR xmgr_cas_CxUiMenuSelectTokens(XmgrCas_CxUiMenuContext_t *pstContext, OxMgrCasUiUserInputNotify_t *pstUserInputNotify)
{
	HUINT8 		aucStr[MGR_CAS_STR_LENGTH_SHORT]={0,};
	HERROR		hError = ERR_FAIL;

	HxSTD_memset(&pstContext->stSubItem, 0, sizeof(XmgrCas_CxUiMenuSubItemContext_t));

	pstContext->stSubItem.stPurseStatus.usPurseRef = (HUINT16)pstUserInputNotify->utData.stSelection.ulItemIdx;
	HxSTD_memcpy(aucStr, pstUserInputNotify->utData.stSelection.aucData, MGR_CAS_STR_LENGTH_SHORT);

	if(pstUserInputNotify->utData.stSelection.ulDataLength > 0)
	{
		HLIB_STD_StrUpper(aucStr);

		if(strcmp(aucStr, "DEBIT") == 0 )	// Debit
		{
			HxLOG_Info("Prepare debit submenu...\n");

			hError = xmgr_cas_CxUiMenuGetDebitData(&pstContext->stSubItem);
			if(hError != ERR_OK)
			{
				HxLOG_Error("xmgr_cas_CxUiMenuGetDebitData() FUNCTION ERROR!!\n");
			}
		}
		else if(strcmp(aucStr, "CREDIT") == 0 )	// Credit
		{
			HxLOG_Info("Prepare credit submenu...\n");

			hError = xmgr_cas_CxUiMenuGetCreditData(&pstContext->stSubItem);
			if(hError != ERR_OK)
			{
				HxLOG_Error("xmgr_cas_CxUiMenuGetCreditData() FUNCTION ERROR!!\n");
			}
		}
		else
		{
			//ERROR
			HxLOG_Critical("ERROR!!!\n");
			return ERR_FAIL;
		}

		hError = xmgr_cas_CxUiMenuPostMenuListData(pstContext, CX_SC_SLOT_0, 0);
		if(hError != ERR_OK)
		{
			HxLOG_Error("xmgr_cas_CxUiMenuPostMenuListData() FUNCTION ERROR!!\n");
		}
	}
	else
	{
		HxLOG_Critical("Error!!\n");
	}

	pstContext->usMenuDepth++;

	return ERR_OK;
}

STATIC HERROR xmgr_cas_CxUiMenuSelectSubMenu(XmgrCas_CxUiMenuContext_t *pstContext, OxMgrCasUiUserInputNotify_t	 *pstUserInputNotify)
{
	HERROR		hError = ERR_FAIL;

	switch(pstUserInputNotify->utData.stSelection.ulItemIdx + 1)	// eCasCXMenuItem_None은 실제 화면에 나타나지 않는 내용이고, ItemIdx가 0이면 Subscription Status를 나타내기 때문에 '+1'
	{
		case eCasCXMenuItem_None:
			HxLOG_Error("eCasCXMenuItem_None... Do Nothing...\n");
			break;
		case eCasCXMenuItem_Subscription:
		{
			HxLOG_Info("eCasCXMenuItem_Subscription...\n");
			pstContext->eSelectedMenuItem = eCasCXMenuItem_Subscription;

			hError = xmgr_cas_CxUiMenuGetSubscriptionData(&pstContext->stSubItem);
			if(hError != ERR_OK)
			{
				HxLOG_Error("xmgr_cas_CxUiMenuGetSubscriptionData() FUNCTION ERROR!!\n");
			}
			hError = xmgr_cas_CxUiMenuPostMenuListData(pstContext, CX_SC_SLOT_0, eMgrCasUiAttr_Title|eMgrCasUiAttr_Subtitle);
			if(hError != ERR_OK)
			{
				HxLOG_Error("xmgr_cas_CxUiMenuPostMenuListData() FUNCTION ERROR!!\n");
			}
			break;
		}
		case eCasCXMenuItem_Event:
		{
			HxLOG_Info("eCasCXMenuItem_Event...\n");
			pstContext->eSelectedMenuItem = eCasCXMenuItem_Event;

			hError = xmgr_cas_CxUiMenuGetEventData(&pstContext->stSubItem);
			if(hError != ERR_OK)
			{
				HxLOG_Error("xmgr_cas_CxUiMenuGetEventData() FUNCTION ERROR!!\n");
			}
			hError = xmgr_cas_CxUiMenuPostMenuListData(pstContext, CX_SC_SLOT_0, eMgrCasUiAttr_Title|eMgrCasUiAttr_Subtitle);
			if(hError != ERR_OK)
			{
				HxLOG_Error("xmgr_cas_CxUiMenuPostMenuListData() FUNCTION ERROR!!\n");
			}
			break;
		}
		case eCasCXMenuItem_Tokens:
		{
			HxLOG_Info("eCasCXMenuItem_Tokens...\n");
			pstContext->eSelectedMenuItem = eCasCXMenuItem_Tokens;

			hError = xmgr_cas_CxUiMenuGetTokenData(&pstContext->stSubItem);
			if(hError != ERR_OK)
			{
				HxLOG_Error("xmgr_cas_CxUiMenuGetTokenData() FUNCTION ERROR!!\n");
			}
			hError = xmgr_cas_CxUiMenuPostMenuListData(pstContext, CX_SC_SLOT_0, eMgrCasUiAttr_Title|eMgrCasUiAttr_Subtitle|eMgrCasUiAttr_Selectable);
			if(hError != ERR_OK)
			{
				HxLOG_Error("xmgr_cas_CxUiMenuPostMenuListData() FUNCTION ERROR!!\n");
			}
			break;
		}
		case eCasCXMenuItem_Change_Pin:
		{
			HxLOG_Info("eCasCXMenuItem_Change_Pin...\n");
			pstContext->eSelectedMenuItem = eCasCXMenuItem_Change_Pin;

			HxSTD_memset(&pstContext->stSubItem, 0, sizeof(XmgrCas_CxUiMenuSubItemContext_t));
			pstContext->stSubItem.ulTotalNum = 0;
			HxSTD_StrNCpy(pstContext->stSubItem.szTitle, "{\"id\":\"LOC_CHANGE_CA_PIN_ID\"}", MAX_ELEMENT_TEXT_LENGTH);

			hError = xmgr_cas_CxUiMenuPostMenuListData(pstContext, CX_SC_SLOT_0, eMgrCasUiAttr_Title);
			if(hError != ERR_OK)
			{
				HxLOG_Error("xmgr_cas_CxUiMenuPostMenuListData() FUNCTION ERROR!!\n");
			}

			break;
		}
		case eCasCXMenuItem_Maturity_Rating:
		{
			HxLOG_Info("eCasCXMenuItem_Maturity_Rating...\n");
			pstContext->eSelectedMenuItem = eCasCXMenuItem_Maturity_Rating;

			hError = xmgr_cas_CxUiMenuGetMaturityRatingData(&pstContext->stSubItem);

			hError = xmgr_cas_CxUiMenuPostMenuListData(pstContext, CX_SC_SLOT_0, eMgrCasUiAttr_Title|eMgrCasUiAttr_Selectable);
			if(hError != ERR_OK)
			{
				HxLOG_Error("xmgr_cas_CxUiMenuPostMenuListData() FUNCTION ERROR!!\n");
			}
			break;
		}
#if defined(CONFIG_MW_CAS_CONAX_TEXT)
		case eCasCXMenuItem_Text_View:
		{
			HxLOG_Info("eCasCXMenuItem_Text_View...\n");
			pstContext->eSelectedMenuItem = eCasCXMenuItem_Text_View;

			hError = xmgr_cas_CxUiMenuGetMailMessageData(&pstContext->stSubItem);
			if(hError != ERR_OK)
			{
				HxLOG_Error("xmgr_cas_CxUiMenuGetMailMessageData() FUNCTION ERROR!!\n");
			}
			hError = xmgr_cas_CxUiMenuPostMenuListData(pstContext, CX_SC_SLOT_0, eMgrCasUiAttr_Title|eMgrCasUiAttr_Selectable);
			if(hError != ERR_OK)
			{
				HxLOG_Error("xmgr_cas_CxUiMenuPostMenuListData() FUNCTION ERROR!!\n");
			}
			break;
		}
#endif
		case eCasCXMenuItem_About:
		{
			HxLOG_Info("eCasCXMenuItem_About...\n");
			pstContext->eSelectedMenuItem = eCasCXMenuItem_About;
			hError = xmgr_cas_CxUiMenuPostAboutConaxListData(pstContext, CX_SC_SLOT_0);
			break;
		}
		case eCasCXMenuItem_Max:
			break;
		default:
			break;
	}

	pstContext->usMenuDepth++;

	return ERR_OK;
}



STATIC BUS_Result_t xmgr_cas_CxUiMenuInputNotify(XmgrCas_CxUiMenuContext_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR 							hError = ERR_FAIL;
	HUINT32							ulSlotNumber = 0;
	DxCAS_MmiSessionType_e			eSessionType = eDxCAS_SESSIONTYPE_NONE;
	OxMgrCasUiUserInputNotify_t		*pstUserInputNotify = NULL;

	HxLOG_Trace();

	ulSlotNumber = (HUINT32)p1;
	eSessionType = (DxCAS_MmiSessionType_e)p2;
	pstUserInputNotify = (OxMgrCasUiUserInputNotify_t *)p3;


	if (eSessionType != eDxCAS_SESSIONTYPE_MENU)
	{
		return MESSAGE_PASS;
	}

	switch(pstUserInputNotify->eInputType)
	{
		case eMgrCasInputType_Etc:
			switch(pstUserInputNotify->utData.stEtc.eMgrCasEtcType)
			{
				case eMgrCasEtc_Previous:
					HxLOG_Info("eMgrCasEtc_Previous\n");

					if(pstContext->usMenuDepth == 2)
					{
						OxMgrCasUiUserInputNotify_t		stUserInputNotify;

						if(pstContext->eSelectedMenuItem != eCasCXMenuItem_Tokens)
						{
							HxLOG_Critical("ERROR! If the menu depth is '2', the selected menu should be eCasCXMenuItem_Tokens...\n");
							return MESSAGE_PASS;
						}

						stUserInputNotify.utData.stSelection.ulItemIdx = pstContext->eSelectedMenuItem - 1;

						// 결국 Depth 1을 들어가는 것 이므로, Select SubMenu와 동일한 루틴을 타면 됨.
						pstContext->usMenuDepth = 0;
						// Enter 1st Sub Menu...
						hError = xmgr_cas_CxUiMenuSelectSubMenu(pstContext, &stUserInputNotify);
					}
					else if(pstContext->usMenuDepth == 1)
					{
						pstContext->eSelectedMenuItem = eCasCXMenuItem_None;
						pstContext->usMenuDepth = 0;
						hError = xmgr_cas_CxUiMenuPostConaxRootMenu(pstContext, CX_SC_SLOT_0);
						if(hError != ERR_OK)
						{
							HxLOG_Error("ERROR! xmgr_cas_CxUiMenuPostConaxRootMenu()...\n");
						}
					}
					else
					{
						HxLOG_Critical("ERROR! Check sequence...\n");
					}
					break;

				case eMgrCasEtc_Exit:
					HxLOG_Info("eMgrCasEtc_Exit\n");
					pstContext->usMenuDepth = 0;
					pstContext->eSelectedMenuItem = eCasCXMenuItem_None;
					break;

				default:
					break;
			}
			break;

		case eMgrCasInputType_Number:
		{
			HxLOG_Info("eMgrCasInputType_Number\n");

			if(pstContext->eSelectedMenuItem == eCasCXMenuItem_Change_Pin)
			{
				if(pstUserInputNotify->utData.stNumber.ulNumberLength == 8)
				{
					pstContext->stSubItem.hPinStatus = xmgr_cas_CxUiMenuChangeSmartCardPin(&pstUserInputNotify->utData.stNumber.pszNumber[0], &pstUserInputNotify->utData.stNumber.pszNumber[4]);
					hError = xmgr_cas_CxUiMenuPostTextData(pstContext, CX_SC_SLOT_0);
					//UI 시나리오 상 pin change correct 일 경우 depth가 자동으로 main으로 한단계 올라간다.
					if(pstContext->stSubItem.hPinStatus == ERR_OK)
					{
						pstContext->eSelectedMenuItem = eCasCXMenuItem_None;
						pstContext->usMenuDepth--;
					}
				}
				else
				{
					HxLOG_Error("ERROR! change pin length should be '8'!!\n");
				}
			}
			else
			{
				HxLOG_Error("ERROR - Number input is only used by change pin. Check it...\n");
			}
			break;
		}

		case eMgrCasInputType_Selection:
			if(pstContext->eSelectedMenuItem == eCasCXMenuItem_None)
			{
				// Enter 1st Sub Menu...
				hError = xmgr_cas_CxUiMenuSelectSubMenu(pstContext, pstUserInputNotify);
			}
			else if(pstContext->eSelectedMenuItem == eCasCXMenuItem_Tokens)
			{
				hError = xmgr_cas_CxUiMenuSelectTokens(pstContext, pstUserInputNotify);
			}
			else if(pstContext->eSelectedMenuItem == eCasCXMenuItem_Maturity_Rating)
			{
				hError = xmgr_cas_CxUiMenuSelectMaturityRating(pstContext, pstUserInputNotify);
			}
			else if(pstContext->eSelectedMenuItem == eCasCXMenuItem_Text_View)
			{
				hError = xmgr_cas_CxUiMenuSelectMailMessage(pstContext, pstUserInputNotify);
			}
			else
			{
				HxLOG_Error("ERROR!! Check the sequence!!!\n");
			}

			HxLOG_Info("Menu, MmiSelect Finished\n");
			break;

		default:
			break;
	}

	NOT_USED_PARAM(hError);
	NOT_USED_PARAM(ulSlotNumber);

	return ERR_BUS_MESSAGE_BREAK;
}

BUS_Result_t xproc_cas_CxUiMenu(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t				hBusResult = MESSAGE_PASS;
	HERROR						hError = ERR_OK;
	XmgrCas_CxUiMenuContext_t	*pstContext = xmgr_cas_CxUiMenuGetContext();

	switch(message)
	{
		case eMEVT_BUS_CREATE:
			HxLOG_Info("Message : eMEVT_BUS_CREATE Handle (0x%x), p1(0x%x), p2(0x%x), p3(0x%x) \n", hAction, p1, p2, p3);
			BUS_MGR_SetMgrName(BUS_MGR_GetThis(), (HINT8 *)__FUNCTION__);
			xmgr_cas_CxUiMenuInitContext(pstContext, message, hAction, p1, p2, p3);

			hBusResult = MESSAGE_BREAK;
			break;

		case eMEVT_CAS_SC_INSERTED:
			HxLOG_Info("Message : eMEVT_CAS_SC_INSERTED Handle (0x%x), p1(0x%x), p2(0x%x), p3(0x%x) \n", hAction, p1, p2, p3);
			pstContext->bScInserted = TRUE;
			if(pstContext->eSelectedMenuItem == eCasCXMenuItem_About && pstContext->usMenuDepth == 1)
			{
				hError = xmgr_cas_CxUiMenuPostAboutConaxListData(pstContext, CX_SC_SLOT_0);
			}
			else if(pstContext->eSelectedMenuItem == eCasCXMenuItem_None && pstContext->usMenuDepth == 0)
			{
				hError = xmgr_cas_CxUiMenuPostConaxRootMenu(pstContext, CX_SC_SLOT_0);
				if(hError != ERR_OK)
				{
					HxLOG_Error("ERROR! xmgr_cas_CxUiMenuPostConaxRootMenu()...\n");
				}
			}
			break;

		case eMEVT_CAS_SC_REMOVED:
			HxLOG_Info("Message : eMEVT_CAS_SC_REMOVED Handle (0x%x), p1(0x%x), p2(0x%x), p3(0x%x) \n", hAction, p1, p2, p3);
			pstContext->bScInserted = FALSE;
			if(pstContext->eSelectedMenuItem == eCasCXMenuItem_About && pstContext->usMenuDepth == 1)
			{
				hError = xmgr_cas_CxUiMenuPostAboutConaxListData(pstContext, CX_SC_SLOT_0);
			}
			else if(pstContext->eSelectedMenuItem == eCasCXMenuItem_None && pstContext->usMenuDepth == 0)
			{
				hError = xmgr_cas_CxUiMenuPostConaxRootMenu(pstContext, CX_SC_SLOT_0);
				if(hError != ERR_OK)
				{
					HxLOG_Error("ERROR! xmgr_cas_CxUiMenuPostConaxRootMenu()...\n");
				}
			}
			else
			{
				hError = xmgr_cas_CxUiMenuCloseCxMenu(pstContext);
				if(hError != ERR_OK)
				{
					HxLOG_Error("ERROR! xmgr_cas_CxUiMenuCloseCxMenu()...\n");
				}
			}
			break;

		case eOEVT_CAS_CASUI_APP_OPEN_SESSION:
			HxLOG_Info("eOEVT_CAS_CASUI_APP_OPEN_SESSION...\n");
			if(pstContext != NULL)
			{
				hBusResult = xmgr_cas_CxUiMenuOpenSession(pstContext, message, hAction, p1, p2, p3);
			}
			// Session이 Open되고 난 후에 CX About 데이터를 올려준다.
			pstContext->usMenuDepth = 0;
			hError = xmgr_cas_CxUiMenuPostConaxRootMenu(pstContext, CX_SC_SLOT_0);
			if(hError != ERR_OK)
			{
				HxLOG_Error("ERROR! xmgr_cas_CxUiMenuPostConaxRootMenu()...\n");
			}

			//cxcore쪽에 cxmenu가 열린 상태를 알려준다. (User Text 동작등을 위해... Menu가 열려있는동안 User Text data가 올라오면 안됨.)
			hError = xmgr_cas_CxUiMenuSetCxMenuDisplayed(TRUE);
			if(hError != ERR_OK)
			{
				HxLOG_Error("ERROR! xmgr_cas_CxUiMenuSetCxMenuDisplayed()...\n");
			}
			break;

		case eOEVT_CAS_CASUI_APP_CLOSE_SESSION:
			HxLOG_Info("eOEVT_CAS_CASUI_APP_CLOSE_SESSION...\n");

			//cxcore쪽에 cxmenu가 닫힌 상태를 알려준다. (User Text 동작등을 위해... Menu가 열려있는동안 User Text data가 올라오면 안됨.)
			hError = xmgr_cas_CxUiMenuSetCxMenuDisplayed(FALSE);
			if(hError != ERR_OK)
			{
				HxLOG_Error("ERROR! xmgr_cas_CxUiMenuSetCxMenuDisplayed()...\n");
			}
/*			if(pstContext != NULL)
			{
				xmgr_cas_CiUiMsgCloseSession(pstContext, message, hAction, p1, p2, p3);
			}
*/
			break;

		case eOEVT_CAS_CASUI_APP_USERINPUTDATA:
			hBusResult = xmgr_cas_CxUiMenuInputNotify(pstContext, message, hAction, p1, p2, p3);
			HxLOG_Info("receive eOEVT_CAS_CASUI_APP_USERINPUTDATA \n");
			break;

		case eMEVT_BUS_DESTROY:
			HxLOG_Info("Message : eMEVT_BUS_DESTROY, Handle (0x%x), p1(0x%x), p2(0x%x), p3(0x%x)\n", hAction, p1, p2, p3);
			break;

		case eMEVT_BUS_READY_SHUTDOWN:
			return ERR_BUS_MESSAGE_PASS;
/*
		case eMEVT_CAS_FAIL:
		{
			CxUiMsgInfo_t* pstMsgInfo = (CxUiMsgInfo_t*)p2;
			HxLOG_Info("eMEVT_CAS_FAIL...\n");
			switch(pstMsgInfo->eUiMsgType)
			{
				case eCxUi_GroupSc_NoCard:
					HxLOG_Info("S/C is NOT inserted...\n");
					pstContext->bScInserted = FALSE;
					break;
				case eCxUi_GroupSc_Inserted:
					HxLOG_Info("S/C is inserted...\n");
					pstContext->bScInserted = TRUE;
					break;
			}
			break;
		}
*/
		default:
			break;
	}

	if(hBusResult != MESSAGE_BREAK)
	{
		hBusResult = BUS_ProcessMessageDefault(message, hAction, p1, p2, p3);
	}

	return hBusResult;
}

