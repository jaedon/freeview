/**
@file			xmgr_cas_mmi.c
@brief			ci & embedded cas�� mmi�� �̿��� ��츦 ���� procedure.

Description:  \n
Module: AP/CAS/MMI			 	\n

Copyright (c) 2008 HUMAX Co., Ltd.				\n
All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    **************************/
/*******************************************************************/
#include <octo_common.h>
#include <isosvc.h>

#include <svc_cas.h>
#include <ci_ctrl.h>

#include <mgr_common.h>
#include <mgr_action.h>
#include <mgr_cas.h>


#include "../local_include/_xmgr_cas_ci.h"

/*******************************************************************/
/********************      Definitions     *********************/
/*******************************************************************/

#if defined(CONFIG_DEBUG)
#define	LOG					HxLOG_Print
#define INFO_LOG			HxLOG_Info
#define	ERR_LOG				HxLOG_Error
#define MSG_LOG				HxLOG_Message
#define	FuncENTER			HxLOG_Info("(+)Enter!!\n");
#define	FuncLEAVE(X)		HxLOG_Info("(-)Leave -- (0x%X)\n",X);
#define	DELAY_LOG(X)		OCTO_NULL_PRINT
#define PAUSE				OCTO_NULL_PRINT
#else /* defined(CONFIG_DEBUG) */
#define	LOG					OCTO_NULL_PRINT
#define INFO_LOG			OCTO_NULL_PRINT
#define	ERR_LOG				OCTO_NULL_PRINT
#define MSG_LOG				OCTO_NULL_PRINT
#define	FuncENTER			OCTO_NULL_PRINT
#define	FuncLEAVE(X)		OCTO_NULL_PRINT
#define	DELAY_LOG(X)		OCTO_NULL_PRINT
#define PAUSE				OCTO_NULL_PRINT
#endif /* defined(CONFIG_DEBUG) */

#define MMI_EXP_TIMER_ID			(104)
#define XMGR_CAS_CI_MMI_RESET_LISTBOX_INDEX(listID)		 { \
													GWM_List_SetCurrentIndex(listID,0);		\
													GWM_List_SetStartIndex(listID, 0); 		\
												 }

#define INVALID_SLOT_ID							(0xffff)

//#define XMGR_CI_MMI_MAX_SLOT_NUM						(PAL_MAX_CISLOT_NUMBER)

#if 0	// �Ʒ� �ּ� ���� ��Ȳ�� �߻� ��Ű�� CAM�� �����ؾ� �� ��� if defined�� ��� ��� �ϵ���!
	/* MMI Open�� ������ ���� CAM�� ����Ͽ� �߰� */
	/* Irdeto(Euro) CAM �߿� CAS resource�� Open�Ϸ� �Ǳ� ���� MMI Open�� �õ��ϸ� ������ ���� ��찡 �߻� */
	#define XMGR_CI_MMI_SUPPORT_TIME_OUT
#endif
#define XMGR_CI_MMI_APPL_CLOSE_INDICATOR				(0x20091211)
#define XMGR_CI_MMI_TIMEOUT_TIMER_ID					(0x04280145)
#define XMGR_CI_MMI_TIMEOUT_TIMER_LABS					(10 * 1000)

/* Cancel�� ����Ű�� ���۽� �������� �ϴ� CAM(��: Neotion CI+ ķ)�� �־�, timer�� ����� ����Ű�� block�Ѵ� - �̷� ���� ķ�� ���� �־� �Ϲ������� �����ϴ� ���� �ٶ����� */
#define XMGR_CI_MMI_IGNORE_REPEAT_KEY
#define XMGR_CI_MMI_IGNORE_REPEAT_KEY_TIMER_ID			(0x05051735)
#define XMGR_CI_MMI_IGNORE_REPEAT_KEY_TIMER_LABS		(500)		// 500 ms


/* Application������ AP_MEM_Free�� ����Ͽ��� �ϳ�  MWC_UTIL_ConvSiStringToUtf8()�Լ�����  SI_MEM_Alloc�� ����Ͽ� level�� ���߷� �ε��� �ϰ� �����. */
#define STR_MEM_FREE	OxSI_Free

/*******************************************************************/
/********************      Typedef         **************************/
/*******************************************************************/

typedef enum
{
	eMMI_STATE_INIT = 0,		/* appl ������ ����, menu���� mmi open�� ���̸� mmi open ��û�� ��, no ui, no keyblock */
	eMMI_STATE_RUNNING,		/* MMI Data�� �ޱ� ������ ����, ui �׷��� ���·� key block�� �����Ѵ� */

	eMMI_STATE_MAX
} XmgrCas_CiMmiState_e;

typedef enum
{
	eCI_MMI_SESSION_STATUS_Stopped,
	eCI_MMI_SESSION_STATUS_StartReady,
	eCI_MMI_SESSION_STATUS_Started,

	eCI_MMI_SESSION_STATUS_Max
} XmgrCas_CiMmiSessionStatus_e;


typedef enum
{
	eCI_MMI_CURRENT_UI_Extension,
	eCI_MMI_CURRENT_UI_Menu,

	eCI_MMI_CURRENT_UI_Max
} XmgrCas_CiMmiCurrentUi_e;

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

/*******************************************************************/
/********************      Global Variables     *********************/
/*******************************************************************/


/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
//STATIC XmgrCas_CiMmiContext_t			s_astCiMmi_Context[XMGR_CAS_CI_SLOT_MAX];
STATIC XmgrCas_CiMmiContext_t	s_astCiMmi_Context[XMGR_CAS_CI_SLOT_MAX];
//STATIC HUINT32					s_ulMmiSlotMax = 0;
STATIC HUINT16					s_usSlotIdForDelayedSsClose = 0;
//STATIC XmgrCas_CiMmiState_e				s_eCurrentState = 0;


/********************************************************************/
/********************      Local Functions	*************************/
/********************************************************************/
STATIC XmgrCas_CiMmiContext_t *xmgr_cas_CiMmiGetContext(HUINT32 ulSlot)
{
	if (XMGR_CAS_CI_SLOT_MAX <= ulSlot)
		return NULL;

	return &(s_astCiMmi_Context[ulSlot]);
}

STATIC HCHAR *xmgr_cas_CiMmiEncodeString(HCHAR *pszString)
{
	HUINT32		Len = 0;
	HCHAR		*buf = NULL;

	Len = HLIB_STD_URLStrLen(pszString) + 1;
    buf = HLIB_STD_MemCalloc(Len+1);
    HLIB_STD_URLEncode(buf,pszString,Len);

    return buf;
}

STATIC HERROR xmgr_cas_CiMmiSetMmiParam(XmgrCas_CiMmiContext_t *pstContext, XmgrCas_CiMmiType_e eMmiType, SvcCas_MmiData_t *pMmiData)
{
	XmgrCas_CiMmiUiInfo_t	*pstUiInfo = NULL;
	void 					*pvMmiData = pMmiData->pvMmiData;

	switch(pstContext->eCurrentUi)
	{
		case eCI_MMI_CURRENT_UI_Menu:
			pstUiInfo = &pstContext->stMenuInfo;
			break;
		case eCI_MMI_CURRENT_UI_Extension:
			pstUiInfo = &pstContext->stExtensionInfo;
			break;
		default:
			// �������� �߸� �Ǿ� �ִ� �������, ����ó���� Extention UI�ζ� ����ϵ��� ��.
			HxLOG_Error("ERROR - wrong UI Type!!\n");
			pstContext->eCurrentUi = eCI_MMI_CURRENT_UI_Extension;
			pstUiInfo = &pstContext->stExtensionInfo;
			return ERR_FAIL;
	}

	pstUiInfo->eMmiType = eMmiType;

	switch(pstUiInfo->eMmiType)
	{
		case eCiMmiType_Text:
			pstUiInfo->pszTextParam = (HUINT8 *)pvMmiData;
			// �Ʒ��ʿ��� Param ������ �ǰ����� Ȥ�� �𸣴� ��� ���� ������� NULL ó���� �� ��.
			pstUiInfo->pstEnqParam = NULL;
			pstUiInfo->pstListParam = NULL;
			break;
		case eCiMmiType_Menu:
		case eCiMmiType_List:
			pstUiInfo->pstListParam = (SvcCas_MmiMenuList_t *)pvMmiData;
			pstUiInfo->pstEnqParam = NULL;
			pstUiInfo->pszTextParam = NULL;
			break;

		case eCiMmiType_Enq:
			pstUiInfo->pstEnqParam = (SvcCas_MmiEnq_t *)pvMmiData;
			pstUiInfo->pstListParam = NULL;
			pstUiInfo->pszTextParam = NULL;
			break;
		default:
			HxLOG_Error("ERROR - sequence error!! check it!!\n");
			break;
	}

	pstUiInfo->bIsHavingData = TRUE;

	// Menu �� ���� �� ��쿡 Session Open �� hMmi�� CAM�� ���� �Ҵ� ���� �ʴ� ��찡 �־� hMmi NULL�� ��� ���� �Ҵ� ��.
	if(pstUiInfo->hMmi == HANDLE_NULL)
	{
		pstUiInfo->hMmi = pMmiData->hMmi;
	}
	return ERR_OK;
}

STATIC HERROR	xmgr_cas_CiMmiRequestMMICloseToMW(unsigned long hMmi)
{
	HERROR					hErr;
	SvcCas_CtrlParam_t		stParam;
	CI_MmiCloseInParam_t 	stInParamClose;

	if(hMmi == HANDLE_NULL)
	{
		HxLOG_Info("MMI is (maybe) already Closed...\n");
		return ERR_OK;
	}

	stInParamClose.hMmi 	= hMmi;
	stParam.pvIn			= &stInParamClose;
	stParam.ulControlType	= eCACTRL_CI_MMI_Close;

	hErr = SVC_CAS_CtrlCall(eDxCAS_GROUPID_CAM, eCACTRL_CI_MMI_Close, &stParam);
	if(hErr != ERR_OK)
	{
		ERR_LOG("Critical Error!!!\n");
	}

	return hErr;
}

STATIC HERROR	xmgr_cas_CiMmiPostSessionEventToOM(XmgrCas_CiMmiUiInfo_t *pstUiInfo, HUINT16 usSlotId, HINT32 eEvent)
{
	BUS_Result_t						hResult = ERR_BUS_NO_ERROR;
	OxMgrCasUiEvtSession_t				*pstEvent = NULL;

	pstEvent = HLIB_STD_MemCalloc(sizeof(OxMgrCasUiEvtSession_t));
	if(pstEvent == (OxMgrCasUiEvtSession_t *)NULL)
	{
		HxLOG_Error("\n");
		return ERR_FAIL;
	}

	pstEvent->ulUiHandle = (HUINT32)xproc_cas_CiMmi;
	pstEvent->eSessionType = pstUiInfo->eSessionType;
	pstEvent->hSession = pstUiInfo->hSession;

	hResult = BUS_PostMessageToOM(NULL, eEvent, HANDLE_NULL, eDxCAS_GROUPID_CAM, (HINT32)usSlotId, (HINT32)pstEvent);
	if(hResult != ERR_BUS_NO_ERROR)
	{
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC HERROR xmgr_cas_CiMmiPostTextData(XmgrCas_CiMmiUiInfo_t *pstUiInfo, HUINT16 usSlotId)
{
	HERROR					hError = ERR_OK;
	HUINT8					*pszText = NULL;
	HUINT32					ulStringLength = 0;
	BUS_Result_t			hBusResult = ERR_BUS_NO_ERROR;
	OxMgrCasUiEvtTextbox_t	*pstTextBoxEvent = NULL;

	HINT32					slStream;
	HCHAR					*pzEncodedStr = NULL;
	HCHAR					*pzProperties = NULL;
	HUINT32					ulStreamSize = 0;

	if(pstUiInfo->pszTextParam == NULL)
	{
		HxLOG_Assert(FALSE);
		hError = ERR_FAIL;
		goto END_FUNC;
	}

	pstTextBoxEvent = (OxMgrCasUiEvtTextbox_t *)HLIB_STD_MemCalloc(sizeof(OxMgrCasUiEvtTextbox_t));
	if(pstTextBoxEvent == (OxMgrCasUiEvtTextbox_t *)NULL)
	{
		HxLOG_Error("\n");
		hError = ERR_FAIL;
		goto END_FUNC;
	}

	MWC_UTIL_ConvSiStringToUtf8(NULL, SvcCas_UtilStrLen(pstUiInfo->pszTextParam), pstUiInfo->pszTextParam, &pszText);
	if(pszText == NULL)
	{
		HxLOG_Warning("\n");
		hError = ERR_FAIL;
		goto END_FUNC;
	}

	ulStringLength = SvcCas_UtilStrLen(pszText);

	pstTextBoxEvent->stSessionInfo.eSessionType = pstUiInfo->eSessionType;
	pstTextBoxEvent->stSessionInfo.ulUiHandle = (HUINT32)xproc_cas_CiMmi;
	pstTextBoxEvent->stSessionInfo.hSession = pstUiInfo->hSession;

	pstTextBoxEvent->stTextbox.stHeader.ulAttr = 0;
	pstTextBoxEvent->stTextbox.ulStringLength = ulStringLength;

	slStream = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
	pzEncodedStr = xmgr_cas_CiMmiEncodeString(pszText);
	if(pzEncodedStr != NULL)
	{
		HLIB_RWSTREAM_Print(slStream, "[\"%s\"]", pzEncodedStr);
		HLIB_STD_MemFree(pzEncodedStr);
		pzEncodedStr = NULL;
	}

	pzProperties = (HCHAR*)HLIB_RWSTREAM_CloseWithoutBuf(slStream);
	ulStreamSize = HxSTD_StrLen(pzProperties);

	pstTextBoxEvent->stTextbox.pszString = (HCHAR *)HLIB_STD_MemCalloc(ulStreamSize+1);
	MWC_UTIL_DvbStrncpy(pstTextBoxEvent->stTextbox.pszString, pzProperties, ulStreamSize);
	HLIB_STD_MemFree(pzProperties);

	hBusResult = BUS_PostMessageToOM(NULL, eMEVT_CASUI_TYPE_TEXTBOX_DATA, HANDLE_NULL, (HINT32)eDxCAS_GROUPID_CAM, (HINT32)usSlotId, (HINT32)pstTextBoxEvent);
	if(hBusResult != ERR_BUS_NO_ERROR)
	{
		HxLOG_Error("\n\n");
		hError = ERR_FAIL;
		goto END_FUNC;
	}

END_FUNC:
	if(pszText != NULL)
	{
		OxSI_Free(pszText);
	}

	if(hError == ERR_FAIL)
	{
		if(pstTextBoxEvent != NULL)
		{
			HLIB_STD_MemFree((void *)pstTextBoxEvent);
		}
	}

	return hError;
}

STATIC HERROR xmgr_cas_CiMmiPostMenuListData(XmgrCas_CiMmiUiInfo_t *pstUiInfo, HUINT16 usSlotId, OxMgrCasUIAttribute_e eDefaultAttr)
{
	SvcCas_MmiMenuList_t 		*pMmiMenuData = NULL;
	HERROR						hError = ERR_OK;
	HUINT8						*pszText = NULL;
	HUINT32						ulStrLen = 0;
	HUINT32						ulMenuItemLoop = 0;
	BUS_Result_t				hBusResult = ERR_BUS_NO_ERROR;
	OxMgrCasUiEvtMenulist_t		*pstEvent = NULL;
	OxMgrCasUiMenulist_t		*pstMenuList = NULL;
	OxMgrCasUiMenuItem_t		*pMenuItem = NULL;
	HINT32					slStream;
	HCHAR					*pzEncodedStr = NULL;
	HCHAR					*pzProperties = NULL;

	pstEvent = (OxMgrCasUiEvtMenulist_t *)HLIB_STD_MemCalloc(sizeof(OxMgrCasUiEvtMenulist_t));
	if(pstEvent == (OxMgrCasUiEvtMenulist_t *)NULL)
	{
		HxLOG_Error("\n\n");
		hError = ERR_FAIL;
		goto END_FUNC;
	}

	pMmiMenuData = pstUiInfo->pstListParam;

	pstEvent->stSessionInfo.eSessionType = pstUiInfo->eSessionType;
	pstEvent->stSessionInfo.ulUiHandle = (HUINT32)xproc_cas_CiMmi;
	pstEvent->stSessionInfo.hSession = pstUiInfo->hSession;

	pstMenuList = &(pstEvent->stMenulist);

	pstMenuList->stHeader.ulAttr = eDefaultAttr;
	pstMenuList->stHeader.ulFocusIndex = 0;
	pstMenuList->stHeader.ulStartIndex = 0;
	pstMenuList->ulTotalNum = pMmiMenuData->ulNumOfItem;

	if(pMmiMenuData->pszTitle != NULL)
	{
		ulStrLen = SvcCas_UtilStrLen(pMmiMenuData->pszTitle);
		if(ulStrLen != 0)
		{
			if(ulStrLen >= MGR_CAS_STR_LENGTH_LONG)
			{
				HxLOG_Warning("string length overflow %d\n", ulStrLen);
				ulStrLen = MGR_CAS_STR_LENGTH_LONG-1;
			}

			MWC_UTIL_ConvSiStringToUtf8(NULL, ulStrLen, pMmiMenuData->pszTitle, &pszText);
			if(pszText != NULL)
			{
				slStream = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
				pzEncodedStr = xmgr_cas_CiMmiEncodeString(pszText);
				if(pzEncodedStr != NULL)
				{
					HLIB_RWSTREAM_Print(slStream, "[\"%s\"]", pzEncodedStr);
					HLIB_STD_MemFree(pzEncodedStr);
					pzEncodedStr = NULL;
				}

				pzProperties = (HCHAR*)HLIB_RWSTREAM_CloseWithoutBuf(slStream);

				MWC_UTIL_DvbStrncpy(pstMenuList->stHeader.szTitle, pzProperties, MGR_CAS_STR_LENGTH_LONG);
				HLIB_STD_MemFree(pzProperties);
				pstMenuList->stHeader.ulAttr |= eMgrCasUiAttr_Title;

				STR_MEM_FREE(pszText);
			}
		}
	}

	if(pMmiMenuData->pszSubTitle != NULL)
	{
		ulStrLen = SvcCas_UtilStrLen(pMmiMenuData->pszSubTitle);
		if(ulStrLen != 0)
		{
			if(ulStrLen >= MGR_CAS_STR_LENGTH_LONG)
			{
				HxLOG_Warning("string length overflow %d\n", ulStrLen);
				ulStrLen = MGR_CAS_STR_LENGTH_LONG-1;
			}

			MWC_UTIL_ConvSiStringToUtf8(NULL, ulStrLen, pMmiMenuData->pszSubTitle, &pszText);
			if(pszText != NULL)
			{
				slStream = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
				pzEncodedStr = xmgr_cas_CiMmiEncodeString(pszText);
				if(pzEncodedStr != NULL)
				{
					HLIB_RWSTREAM_Print(slStream, "[\"%s\"]", pzEncodedStr);
					HLIB_STD_MemFree(pzEncodedStr);
					pzEncodedStr = NULL;
				}

				pzProperties = (HCHAR*)HLIB_RWSTREAM_CloseWithoutBuf(slStream);

				MWC_UTIL_DvbStrncpy(pstMenuList->stHeader.szSubTitle, pzProperties, MGR_CAS_STR_LENGTH_LONG);
				HLIB_STD_MemFree(pzProperties);

				pstMenuList->stHeader.ulAttr |= eMgrCasUiAttr_Subtitle;
				STR_MEM_FREE(pszText);
			}
		}
	}

	if(pMmiMenuData->pszBottom != NULL)
	{
		ulStrLen = SvcCas_UtilStrLen(pMmiMenuData->pszBottom);
		if(ulStrLen != 0)
		{
			if(ulStrLen >= MGR_CAS_STR_LENGTH_LONG)
			{
				HxLOG_Warning("string length overflow %d\n", ulStrLen);
				ulStrLen = MGR_CAS_STR_LENGTH_LONG-1;
			}

			MWC_UTIL_ConvSiStringToUtf8(NULL, ulStrLen, pMmiMenuData->pszBottom, &pszText);
			if(pszText != NULL)
			{
				slStream = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
				pzEncodedStr = xmgr_cas_CiMmiEncodeString(pszText);
				if(pzEncodedStr != NULL)
				{
					HLIB_RWSTREAM_Print(slStream, "[\"%s\"]", pzEncodedStr);
					HLIB_STD_MemFree(pzEncodedStr);
					pzEncodedStr = NULL;
				}

				pzProperties = (HCHAR*)HLIB_RWSTREAM_CloseWithoutBuf(slStream);

				MWC_UTIL_DvbStrncpy(pstMenuList->stHeader.szBottom, pzProperties, MGR_CAS_STR_LENGTH_LONG);
				HLIB_STD_MemFree(pzProperties);
				pstMenuList->stHeader.ulAttr |= eMgrCasUiAttr_Bottom;
				STR_MEM_FREE(pszText);
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

	for(ulMenuItemLoop = 0;ulMenuItemLoop < pstMenuList->ulTotalNum;ulMenuItemLoop++)
	{
		pMenuItem = &pstMenuList->astListItem[ulMenuItemLoop];

		pMenuItem->ulIndex = ulMenuItemLoop;
		pMenuItem->ulAttr = 0;
		pMenuItem->ulType = eMgrCasUiItemType_Str;

		MWC_UTIL_ConvSiStringToUtf8(NULL, SvcCas_UtilStrLen(pMmiMenuData->ppszItem[ulMenuItemLoop]), pMmiMenuData->ppszItem[ulMenuItemLoop], &pszText);
		if(pszText != NULL)
		{
			pMenuItem->utData.stString.ulStringLength = SvcCas_UtilStrLen(pszText);

			slStream = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
			pzEncodedStr = xmgr_cas_CiMmiEncodeString(pszText);
			if(pzEncodedStr != NULL)
			{
				HLIB_RWSTREAM_Print(slStream, "[\"%s\"]", pzEncodedStr);
				HLIB_STD_MemFree(pzEncodedStr);
				pzEncodedStr = NULL;
			}

			pzProperties = (HCHAR*)HLIB_RWSTREAM_CloseWithoutBuf(slStream);

			MWC_UTIL_DvbStrncpy(pMenuItem->utData.stString.szString, pzProperties, MGR_CAS_STR_LENGTH_LONG);
			pMenuItem->utData.stString.szString[MGR_CAS_STR_LENGTH_LONG - 1] = '\0';
			HLIB_STD_MemFree(pzProperties);

			STR_MEM_FREE(pszText);
		}
		HxLOG_Debug("(%d) %s\n", pMenuItem->ulIndex, pMenuItem->utData.stString.szString);
	}

	hBusResult = BUS_PostMessageToOM(NULL, eMEVT_CASUI_TYPE_MENULIST_DATA, HANDLE_NULL, (HINT32)eDxCAS_GROUPID_CAM, (HINT32)usSlotId, (HINT32)pstEvent);
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

	return hError;
}

STATIC HERROR xmgr_cas_CiMmiPostEnqData(XmgrCas_CiMmiUiInfo_t *pstUiInfo, HUINT16 usSlotId)
{
	SvcCas_MmiEnq_t 		*pMmiEnqData = NULL;
	HERROR					hError = ERR_OK;
	HUINT8					*pszText = NULL;
	BUS_Result_t			hBusResult = ERR_BUS_NO_ERROR;
	OxMgrCasUiEvtInput_t	*pstInputEvent = NULL;
	OxMgrCasUiInputNumber_t	*pstNumber = NULL;
	HINT32					slStream;
	HCHAR					*pzEncodedStr = NULL;
	HCHAR					*pzProperties = NULL;

	HxLOG_Info("xmgr_cas_CiMmiPostEnqData()\n");

	pstInputEvent = (OxMgrCasUiEvtInput_t *)HLIB_STD_MemCalloc(sizeof(OxMgrCasUiEvtInput_t));
	if(pstInputEvent == NULL)
	{
		return ERR_FAIL;
	}

	pMmiEnqData = pstUiInfo->pstEnqParam;

	pstInputEvent->stSessionInfo.eSessionType = pstUiInfo->eSessionType;
	pstInputEvent->stSessionInfo.ulUiHandle = (HUINT32)xproc_cas_CiMmi;
	pstInputEvent->stSessionInfo.hSession = pstUiInfo->hSession;

	if(pMmiEnqData->pszPrompt != NULL)
	{
		MWC_UTIL_ConvSiStringToUtf8(NULL, SvcCas_UtilStrLen(pMmiEnqData->pszPrompt), pMmiEnqData->pszPrompt, &pszText);
		if(pszText == NULL)
		{
			HxLOG_Warning("\n");
		}
		else
		{
			slStream = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
			pzEncodedStr = xmgr_cas_CiMmiEncodeString(pszText);
			if(pzEncodedStr != NULL)
			{
				HLIB_RWSTREAM_Print(slStream, "[\"%s\"]", pzEncodedStr);
				HLIB_STD_MemFree(pzEncodedStr);
				pzEncodedStr = NULL;
			}

			pzProperties = (HCHAR*)HLIB_RWSTREAM_CloseWithoutBuf(slStream);

			MWC_UTIL_DvbStrncpy(pstInputEvent->stHeader.szSubTitle, pzProperties, MGR_CAS_STR_LENGTH_LONG);
			HLIB_STD_MemFree(pzProperties);
			pstInputEvent->stHeader.ulAttr |= eMgrCasUiAttr_Subtitle;
			STR_MEM_FREE(pszText);
		}
	}

	pstInputEvent->stInputData.eInputType = eMgrCasInputType_Number;
	pstNumber = (OxMgrCasUiInputNumber_t *)&pstInputEvent->stInputData.utInputData;

	if(pMmiEnqData->ucBlindOnOff)
	{
		pstNumber->ulAttr = eMgrCasUiAttr_HiddenInput;
	}
	else
	{
		pstNumber->ulAttr = 0;
	}

	pstNumber->ulNumberLength = pMmiEnqData->ucAnswerLen;
	if(pstNumber->ulNumberLength != 0)
	{
		HxSTD_MemSet(pstNumber->pszNumber, 0x00, sizeof(MGR_CASINPUT_NUMBER_MAX_LENGTH));
		if(pstNumber->ulNumberLength == 0xff)
		{
			HxLOG_Info("Answer Length is variable...\n");
		}
		else if(pstNumber->ulNumberLength >= MGR_CASINPUT_NUMBER_MAX_LENGTH)
		{
			HxLOG_Warning("\n");
			pstNumber->ulNumberLength = MGR_CASINPUT_NUMBER_MAX_LENGTH-1;
		}
	}

	hBusResult = BUS_PostMessageToOM(NULL, eMEVT_CASUI_TYPE_INPUT_DATA, HANDLE_NULL, (HINT32)eDxCAS_GROUPID_CAM, (HINT32)usSlotId, (HINT32)pstInputEvent);
	if(hBusResult != ERR_BUS_NO_ERROR)
	{
		HxLOG_Error("\n\n");
		hError = ERR_FAIL;
		goto END_FUNC;
	}

END_FUNC:
	if(hError == ERR_FAIL)
	{
		if(pstInputEvent != NULL)
		{
			HLIB_STD_MemFree(pstInputEvent);
		}
	}

	return hError;
}

STATIC HERROR xmgr_cas_CiMmiPostUiData(XmgrCas_CiMmiContext_t *pstContext)
{
	XmgrCas_CiMmiUiInfo_t	*pstUiInfo = NULL;

	switch(pstContext->eCurrentUi)
	{
		case eCI_MMI_CURRENT_UI_Menu:
			pstUiInfo = &pstContext->stMenuInfo;
			break;
		case eCI_MMI_CURRENT_UI_Extension:
			pstUiInfo = &pstContext->stExtensionInfo;
			break;
		default:
			HxLOG_Error("ERROR - wrong UI Type!!\n");
			return ERR_FAIL;
	}

	if(pstUiInfo->bIsHavingData == FALSE)
	{
		HxLOG_Info("no data... \n");
		return ERR_OK;
	}

	switch (pstUiInfo->eMmiType)
	{
		case eCiMmiType_Text:
		{
			if(xmgr_cas_CiMmiPostTextData(pstUiInfo, pstContext->usSlotId) != ERR_OK)
			{
				HxLOG_Error("\n\n");
			}

			break;
		}

		case eCiMmiType_Menu:
		{
			if(xmgr_cas_CiMmiPostMenuListData(pstUiInfo, pstContext->usSlotId, eMgrCasUiAttr_Selectable) != ERR_OK)
			{
				HxLOG_Error("\n\n");
			}
			#if defined(CONFIG_DEBUG)
			{
				int i;
				HxLOG_Print("[XMGR_CI_MMI_MENU] \n");
				HxLOG_Print("\t [%s] \n", pstUiInfo->pstListParam->pszTitle);
				HxLOG_Print("\t [%s] \n", pstUiInfo->pstListParam->pszSubTitle);
				for (i = 0; i < pstUiInfo->pstListParam->ulNumOfItem; i++)
				{
					HxLOG_Print("\t\t [%02d:%s] \n", i+1, pstUiInfo->pstListParam->ppszItem[i]);
				}
				HxLOG_Print("\t [%s] \n", pstUiInfo->pstListParam->pszBottom);
			}
			#endif
			break;
		}

		case eCiMmiType_List:
		{
			if(xmgr_cas_CiMmiPostMenuListData(pstUiInfo, pstContext->usSlotId, 0) != ERR_OK)
			{
				HxLOG_Error("\n\n");
			}
			#if defined(CONFIG_DEBUG)
			{
				int i;
				HxLOG_Print("[XMGR_CI_MMI_LIST] \n");
				HxLOG_Print("\t [%s] \n", pstUiInfo->pstListParam->pszTitle);
				HxLOG_Print("\t [%s] \n", pstUiInfo->pstListParam->pszSubTitle);
				for (i = 0; i < pstUiInfo->pstListParam->ulNumOfItem; i++)
				{
					HxLOG_Print("\t\t [%02d:%s] \n", i+1, pstUiInfo->pstListParam->ppszItem[i]);
				}
				HxLOG_Print("\t [%s] \n", pstUiInfo->pstListParam->pszBottom);
			}
			#endif
			break;
		}

		case eCiMmiType_Enq:
		{
			if(xmgr_cas_CiMmiPostEnqData(pstUiInfo, pstContext->usSlotId) != ERR_OK)
			{
				HxLOG_Error("\n\n");
			}
			break;
		}

		default:
			HxLOG_Error("\n\n");
			return ERR_FAIL;
	}

	pstUiInfo->bIsHavingData = FALSE;
	return ERR_OK;
}


STATIC HERROR xmgr_cas_CiMmiUpdateMmiData(XmgrCas_CiMmiContext_t *pstContext)
{
	XmgrCas_CiMmiUiInfo_t	*pstUiInfo = NULL;

	switch(pstContext->eCurrentUi)
	{
		case eCI_MMI_CURRENT_UI_Menu:
			pstUiInfo = &pstContext->stMenuInfo;
			break;
		case eCI_MMI_CURRENT_UI_Extension:
			pstUiInfo = &pstContext->stExtensionInfo;
			break;
		default:
			// �������� �߸� �Ǿ� �ִ� �������, ����ó���� Extention UI�ζ� ����ϵ��� ��.
			HxLOG_Error("ERROR - wrong UI Type!!\n");
			pstContext->eCurrentUi = eCI_MMI_CURRENT_UI_Extension;
			pstUiInfo = &pstContext->stExtensionInfo;
			return ERR_FAIL;
	}

	HxLOG_Info("pstUiInfo->eSessionStatus[%d] (eSessionStatus is should be 2...)\n", pstUiInfo->eSessionStatus);

	if(pstUiInfo->hSession == HANDLE_NULL)
	{
		xmgr_cas_CiMmiPostSessionEventToOM(pstUiInfo, pstContext->usSlotId, eMEVT_CASUI_REQ_OPEN_SESSION);
		pstUiInfo->eSessionStatus = eCI_MMI_SESSION_STATUS_StartReady;
	}
	else
	{
		xmgr_cas_CiMmiPostUiData(pstContext);
	}

	return ERR_OK;
}

STATIC HERROR xmgr_cas_CiMmiOpenMMI(XmgrCas_CiMmiContext_t *pstContext, HUINT32 ulSlotNumber, DxCAS_MmiSessionType_e eSessionType)
{
	HERROR		hError = ERR_OK;

	SvcCas_CtrlParam_t		stParam;
	CI_MmiOpenInParam_t		stMmiOpen;

	HxLOG_Trace();

	stMmiOpen.usSlotId 		= ulSlotNumber;
	stParam.pvIn			= &stMmiOpen;
	stParam.ulControlType	= eCACTRL_CI_MMI_Open;

	/* request from  Menu */
	hError = SVC_CAS_CtrlCall(eDxCAS_GROUPID_CAM, eCACTRL_CI_MMI_Open, &stParam);

	FuncLEAVE(0);
	return hError;
}

STATIC BUS_Result_t xmgr_cas_CiMmiMmiDataProc(XmgrCas_CiMmiContext_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR 					hError = ERR_FAIL;
	MgrAction_Type_e		eActionType;
	XmgrCas_CiMmiUiInfo_t	*pstUiInfo = NULL;
	HUINT16 				usCurSlotId = (HUINT16)p1;
	HUINT32 				ulVersion = (HUINT32)p3;
	SvcCas_MmiData_t 		*pMmiData = NULL;
	SvcCas_CtrlParam_t		stParam;
	CI_GetDataInParam_t 	stInParam;
	CI_GetDataOutParam_t	stOutParam;

	HxLOG_Trace();

	switch(message)
	{
		case eMEVT_CAS_MMI_OPEN:
		{
			HxLOG_Info("Message : eMEVT_CAS_MMI_OPEN Handle (0x%x), p1(0x%x), p2(0x%x), p3(0x%x) \n", hAction, p1, p2, p3);
			// 1. ���� �����ִ� UI Info�� ȹ��.
			if(pstContext->eCurrentUi == eCI_MMI_CURRENT_UI_Menu)
			{
				pstUiInfo = &pstContext->stMenuInfo;
			}
			else // Menu�� �������� ���� ��쿡�� �ƹ��͵� �ȿ��� �ְų� eCI_MMI_CURRENT_UI_Extension�� �����ִ� ����̴�. �ƹ��͵� �� ���� ���� ���� pop up MMI �� (Ȯ�� �ʿ�)
			{
				pstUiInfo = &pstContext->stExtensionInfo;
			}

			// �̹� �ش� slot & eSession Type���� Session�� Open �Ǿ� �ִµ�, �ٽ� open�� �õ��ϴ� ��� �̹Ƿ� �Ʒ� Assert�� �ɸ��� ��ƾ Ȯ���� �� ��.
// TODO:
//			if(pstUiInfo->hSession != HANDLE_NULL)
//			{
//				HxLOG_Assert(FALSE);
//			}

			// 2. hMmi �� ȹ�� (hMmi �� CAM �� nego �� usSsNo�� ��Ÿ��)
			stInParam.ulDataVer		= ulVersion;
			stInParam.usSlotId		= usCurSlotId;
			stOutParam.ppMmiData 	= &pMmiData;
			stParam.pvIn			= &stInParam;
			stParam.pvOut			= &stOutParam;
			stParam.ulControlType	= eCACTRL_CI_MMI_GetData;
			hError = SVC_CAS_CtrlGet(eDxCAS_GROUPID_CAM, eCACTRL_CI_MMI_GetData, &stParam);
			if(hError != ERR_OK)
			{
				HxLOG_Error("\n\n");
				return MESSAGE_BREAK;
			}

#if defined(CONFIG_DEBUG)
			if((pstUiInfo->hMmi != HANDLE_NULL) && (pstUiInfo->hMmi != (Handle_t)pMmiData->hMmi))
			{
				// HANDLE_NULL�� �ƴ� ���°�, ������ �Ҵ� ���� hMmi ���̶� �ٸ� ������ Open �� ��û�ϸ� sequence error�� �ִ�. �Ʒ� log ��½� ����� �ʿ� ��.
				HxLOG_Error("Sequence Error... Check it!! \n");
			}
#endif
			// Menu�� ��� Web App���� Open req�� �ް� �̿����� �������� Opened �� �÷��� ��Ȳ�ε�, hMmi�� �Ҵ���� ���� �����̹Ƿ� �ϴ� ���� ���� hMmi�� �缳�� (���� ������ �ִٸ� �� ���� �α׸� ��� �� ����)
			pstUiInfo->hMmi = (Handle_t)pMmiData->hMmi;

			if(pstUiInfo->hSession != HANDLE_NULL)
			{
				HxLOG_Warning("Session is already opened... [%08x] \n", pstUiInfo->hSession);
				return MESSAGE_BREAK;
			}

			eActionType = MGR_ACTION_GetType(MGR_ACTION_GetMainActionId());

			HxLOG_Info("\n\n\nMMI Open ST[%d], AT[%d]\n\n\n\n", pstUiInfo->eSessionType, eActionType);

			/* Live, Playback, Chase Playback, TSR Playback�� ������ ���¿��� MMI�� display���� �ʴ´�. */
			/* Search ���̶�� �ϴ��� Operator Profile ���� ���� ������ UI ����� �Ǿ�� �Ѵ�.		*/
 			if((eActionType != eViewType_WTV) && (eActionType != eViewType_PB) && (eActionType != eViewType_TSR_PB) && (eActionType != eViewType_CHASE_PB)
				&& (BUS_MGR_Get(xproc_cas_CiOpSearchOperatorProfile) == NULL))
			{
				HxLOG_Info("There is not on view mode, then MMI should be closed. Action type [%d]\n", eActionType);

				hError = xmgr_cas_CiMmiRequestMMICloseToMW(pstUiInfo->hMmi);
				if(hError != ERR_OK)
				{
					ERR_LOG("Critical Error!!!\n");
				}

				return MESSAGE_BREAK;
			}

			pstContext->eCurrentUi = eCI_MMI_CURRENT_UI_Extension;

			xmgr_cas_CiMmiPostSessionEventToOM(pstUiInfo, pstContext->usSlotId, eMEVT_CASUI_REQ_OPEN_SESSION);
			pstUiInfo->eSessionStatus = eCI_MMI_SESSION_STATUS_StartReady;

			return MESSAGE_BREAK;
		}

		case eMEVT_CAS_MMI_TEXT:
			HxLOG_Info("Message : eMEVT_CAS_MMI_TEXT Handle (0x%x), p1(0x%x), p2(0x%x), p3(0x%x) \n", hAction, p1, p2, p3);

			stInParam.ulDataVer		= ulVersion;
			stInParam.usSlotId		= usCurSlotId;
			stOutParam.ppMmiData 	= &pMmiData;
			stParam.pvIn			= &stInParam;
			stParam.pvOut			= &stOutParam;
			stParam.ulControlType	= eCACTRL_CI_MMI_GetData;
			hError = SVC_CAS_CtrlGet(eDxCAS_GROUPID_CAM, eCACTRL_CI_MMI_GetData, &stParam);
			if(hError != ERR_OK)
			{
				HxLOG_Error("\n\n");
				break;
			}

			hError = xmgr_cas_CiMmiSetMmiParam(pstContext, eCiMmiType_Text, pMmiData);
			hError = xmgr_cas_CiMmiUpdateMmiData(pstContext);
			if(ERR_OK != hError)
				HxLOG_Error("Critical Error!!!\n");

			return MESSAGE_BREAK;

		case eMEVT_CAS_MMI_ENQ:
			HxLOG_Info("Message : eMEVT_CAS_MMI_ENQ Handle (0x%x), p1(0x%x), p2(0x%x), p3(0x%x) \n", hAction, p1, p2, p3);

//			pstContext->bEnqComplete = FALSE;

			stInParam.ulDataVer		= ulVersion;
			stInParam.usSlotId		= usCurSlotId;
			stOutParam.ppMmiData 	= &pMmiData;
			stParam.pvIn			= &stInParam;
			stParam.pvOut			= &stOutParam;
			stParam.ulControlType	= eCACTRL_CI_MMI_GetData;
			hError = SVC_CAS_CtrlGet(eDxCAS_GROUPID_CAM, eCACTRL_CI_MMI_GetData, &stParam);
			if(hError != ERR_OK)
			{
				HxLOG_Error("\n\n");
				break;
			}

//			pstContext->pstEnqParam = (SvcCas_MmiEnq_t *)pMmiData->pvMmiData;
			hError = xmgr_cas_CiMmiSetMmiParam(pstContext, eCiMmiType_Enq, pMmiData);
			hError = xmgr_cas_CiMmiUpdateMmiData(pstContext);
			if(ERR_OK != hError)
				HxLOG_Error("Critical Error!!!\n");

			return MESSAGE_BREAK;

		case eMEVT_CAS_MMI_MENU:
			HxLOG_Info("Message : eMEVT_CAS_MMI_MENU Handle (0x%x), p1(0x%x), p2(0x%x), p3(0x%x) \n", hAction, p1, p2, p3);

			stInParam.ulDataVer		= ulVersion;
			stInParam.usSlotId		= usCurSlotId;
			stOutParam.ppMmiData 	= &pMmiData;
			stParam.pvIn			= &stInParam;
			stParam.pvOut			= &stOutParam;
			stParam.ulControlType	= eCACTRL_CI_MMI_GetData;
			hError = SVC_CAS_CtrlGet(eDxCAS_GROUPID_CAM, eCACTRL_CI_MMI_GetData, &stParam);
			if(hError != ERR_OK)
			{
				HxLOG_Error("\n\n");
				break;
			}

			hError = xmgr_cas_CiMmiSetMmiParam(pstContext, eCiMmiType_Menu, pMmiData);
			hError = xmgr_cas_CiMmiUpdateMmiData(pstContext);
			return MESSAGE_BREAK;

		case eMEVT_CAS_MMI_LIST:
			HxLOG_Info("Message : eMEVT_CAS_MMI_LIST Handle (0x%x), p1(0x%x), p2(0x%x), p3(0x%x) \n", hAction, p1, p2, p3);

			stInParam.ulDataVer		= ulVersion;
			stInParam.usSlotId		= usCurSlotId;
			stOutParam.ppMmiData 	= &pMmiData;
			stParam.pvIn			= &stInParam;
			stParam.pvOut			= &stOutParam;
			stParam.ulControlType	= eCACTRL_CI_MMI_GetData;
			hError = SVC_CAS_CtrlGet(eDxCAS_GROUPID_CAM, eCACTRL_CI_MMI_GetData, &stParam);
			if(hError != ERR_OK)
			{
				HxLOG_Error("\n\n");
				break;
			}

			hError = xmgr_cas_CiMmiSetMmiParam(pstContext, eCiMmiType_List, pMmiData);
			hError = xmgr_cas_CiMmiUpdateMmiData(pstContext);
			return MESSAGE_BREAK;

		case eMEVT_CAS_MMI_CLOSE:
			HxLOG_Info("Message : eMEVT_CAS_MMI_CLOSE Handle (0x%x), p1(0x%x), p2(0x%x), p3(0x%x) \n", hAction, p1, p2, p3);
			if(pstContext->eCurrentUi == eCI_MMI_CURRENT_UI_Menu)
			{
				pstUiInfo = &pstContext->stMenuInfo;
			}
			else if(pstContext->eCurrentUi == eCI_MMI_CURRENT_UI_Extension)
			{
				pstUiInfo = &pstContext->stExtensionInfo;
			}
			else
			{
				HxLOG_Warning("No UI Info...\n");
				return MESSAGE_BREAK;
			}

			stInParam.ulDataVer		= ulVersion;
			stInParam.usSlotId		= usCurSlotId;
			stOutParam.ppMmiData 	= &pMmiData;
			stParam.pvIn			= &stInParam;
			stParam.pvOut			= &stOutParam;
			stParam.ulControlType	= eCACTRL_CI_MMI_GetData;
			hError = SVC_CAS_CtrlGet(eDxCAS_GROUPID_CAM, eCACTRL_CI_MMI_GetData, &stParam);
			if(hError != ERR_OK)
			{
				HxLOG_Error("\n\n");
				break;
			}

			/*
				pMmiData->ulMmiDataSize : Close Timer (ms)
				�ش� ���� setting �Ǿ� �ִٸ� �ش� �ð� ���Ŀ� MMI close �� �϶�� �ǹ� ��.
			*/
			if(pMmiData->ulMmiDataSize != 0)
			{
				//data size is time for close timer
				HxLOG_Info("Message : eMEVT_CAS_MMI_CLOSE - set close timer (%d secs) \n", pMmiData->ulMmiDataSize);
				BUS_SetTimer(MMI_EXP_TIMER_ID, pMmiData->ulMmiDataSize * 1000);
				s_usSlotIdForDelayedSsClose = usCurSlotId;
			}
			else if (pstUiInfo->hSession == HANDLE_NULL)
			{
				// �ش� UI�� ���� Session�� �̹� ���� �ִٸ� CAM�ʿ� MMI close ���۸� ����
				HxLOG_Info("Session is already stopped.\n");
				hError = xmgr_cas_CiMmiRequestMMICloseToMW(pMmiData->hMmi);
				if(hError != ERR_OK)
				{
					HxLOG_Error("Error!!!\n");
				}
			}
			else
			{
				hError = xmgr_cas_CiMmiRequestMMICloseToMW(pMmiData->hMmi);
				if(hError != ERR_OK)
				{
					HxLOG_Error("Error!!!\n");
				}
				xmgr_cas_CiMmiPostSessionEventToOM(pstUiInfo, pstContext->usSlotId, eMEVT_CASUI_REQ_CLOSE_SESSION);
			}

			pstUiInfo->hMmi 			= HANDLE_NULL;
			pstUiInfo->hSession 		= HANDLE_NULL;
			pstUiInfo->eSessionStatus	= eCI_MMI_SESSION_STATUS_Stopped;
			pstUiInfo->eMmiType 		= eCiMmiType_Max;
			pstUiInfo->pstEnqParam		= NULL;
			pstUiInfo->pstListParam		= NULL;
			pstUiInfo->pszTextParam		= NULL;
			pstContext->eCurrentUi 		= eCI_MMI_CURRENT_UI_Max;

			return MESSAGE_BREAK;

		default:
			HxLOG_Info("Message : Invalid messge (0x%x) \n", message);
			break;
	}

	FuncLEAVE(0);

	return ERR_BUS_NO_ERROR;
}

STATIC BUS_Result_t xmgr_cas_CiMmiMsgOpenSession(XmgrCas_CiMmiContext_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	OxMgrCasSessionEvent_t		*pstSession = (OxMgrCasSessionEvent_t *)p3;

	HxLOG_Trace();

	switch(pstSession->eSessionType)
	{
		case eDxCAS_SESSIONTYPE_MENU:
			if(pstContext->eCurrentUi == eCI_MMI_CURRENT_UI_Extension)
			{
				// MMI�� slot���� extension���� ���ȵ�, menu�� ���ȵ� CAM Ư���� UI 1���� ������ ������. ���� �ٸ� Type�� UI�� �����ִٸ�, session�� close �ϴ� �۾��� �ʿ�.
				if((pstContext->stExtensionInfo.hSession != HANDLE_NULL) && (pstContext->stExtensionInfo.eSessionStatus != eCI_MMI_SESSION_STATUS_Stopped))
				{
					xmgr_cas_CiMmiPostSessionEventToOM(&pstContext->stExtensionInfo, pstContext->usSlotId, eMEVT_CASUI_REQ_CLOSE_SESSION);
				}
				pstContext->eCurrentUi = eCI_MMI_CURRENT_UI_Max;
			}

			// �̹� �ش� slot & eSession Type���� Session�� Open �Ǿ� �ִµ�, �ٽ� open�� �õ��ϴ� ��� �̹Ƿ� �Ʒ� Assert�� �ɸ��� ��ƾ Ȯ���� �� ��.
// TODO:
//			if(pstContext->stMenuInfo.hSession != HANDLE_NULL)
//			{
//				HxLOG_Assert(FALSE);
//			}

			pstContext->eCurrentUi = eCI_MMI_CURRENT_UI_Menu;
			pstContext->stMenuInfo.eSessionStatus = eCI_MMI_SESSION_STATUS_Started;
			pstContext->stMenuInfo.hSession = pstSession->hSession;

			xmgr_cas_CiMmiPostSessionEventToOM(&pstContext->stMenuInfo, pstContext->usSlotId, eMEVT_CASUI_SESSION_OPENED);
			xmgr_cas_CiMmiOpenMMI(pstContext, pstSession->ulCasSlotNum, pstSession->eSessionType);
			break;

		case eDxCAS_SESSIONTYPE_EXTENSION:
			if(pstContext->eCurrentUi == eCI_MMI_CURRENT_UI_Menu)
			{
				// MMI�� slot���� extension���� ���ȵ�, menu�� ���ȵ� CAM Ư���� UI 1���� ������ ������. ���� �ٸ� Type�� UI�� �����ִٸ�, session�� close �ϴ� �۾��� �ʿ�.
				if((pstContext->stMenuInfo.hSession != HANDLE_NULL) && (pstContext->stMenuInfo.eSessionStatus != eCI_MMI_SESSION_STATUS_Stopped))
				{
					xmgr_cas_CiMmiPostSessionEventToOM(&pstContext->stMenuInfo, pstContext->usSlotId, eMEVT_CASUI_REQ_CLOSE_SESSION);
				}
				pstContext->eCurrentUi = eCI_MMI_CURRENT_UI_Max;
			}

			// �̹� �ش� slot & eSession Type���� Session�� Open �Ǿ� �ִµ�, �ٽ� open�� �õ��ϴ� ��� �̹Ƿ� �Ʒ� Assert�� �ɸ��� ��ƾ Ȯ���� �� ��.
// TODO:
//			if(pstContext->stExtensionInfo.hSession != HANDLE_NULL)
//			{
//				HxLOG_Assert(FALSE);
//			}

			pstContext->eCurrentUi = eCI_MMI_CURRENT_UI_Extension;
			pstContext->stExtensionInfo.eSessionStatus = eCI_MMI_SESSION_STATUS_Started;
			pstContext->stExtensionInfo.hSession = pstSession->hSession;

			xmgr_cas_CiMmiPostSessionEventToOM(&pstContext->stExtensionInfo, pstContext->usSlotId, eMEVT_CASUI_SESSION_OPENED);
			break;
		default:
			break;
	}

	xmgr_cas_CiMmiPostUiData(pstContext);

	FuncLEAVE(0);

	return MESSAGE_BREAK;
}

STATIC BUS_Result_t xmgr_cas_CiMmiMsgCloseSession(XmgrCas_CiMmiContext_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	OxMgrCasSessionEvent_t	*pstSessionEvent = (OxMgrCasSessionEvent_t *)p3;
	XmgrCas_CiMmiUiInfo_t	*pstUiInfo = NULL;
	HERROR					hErr;

	HxLOG_Trace();

	switch(pstSessionEvent->eSessionType)
	{
		case eDxCAS_SESSIONTYPE_MENU:
			pstUiInfo = &pstContext->stMenuInfo;
			break;
		case eDxCAS_SESSIONTYPE_EXTENSION:
			pstUiInfo = &pstContext->stExtensionInfo;
			break;
		default:
			HxLOG_Critical("ERROR - wrong UI Type!!\n");
			return ERR_FAIL;
	}

	pstUiInfo->eSessionStatus = eCI_MMI_SESSION_STATUS_Stopped;
	pstUiInfo->hSession = HANDLE_NULL;
	hErr = xmgr_cas_CiMmiRequestMMICloseToMW(pstUiInfo->hMmi);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("Error!!!\n");
	}

	pstUiInfo->hMmi = HANDLE_NULL;
	pstUiInfo->pstEnqParam = NULL;
	pstUiInfo->pstListParam = NULL;
	pstUiInfo->pszTextParam = NULL;
	pstUiInfo->eMmiType = eCiMmiType_Max;
	pstContext->eCurrentUi = eCI_MMI_CURRENT_UI_Max;

	FuncLEAVE(0);

	return MESSAGE_BREAK;
}

STATIC BUS_Result_t xmgr_cas_CiMmiMsgInputNotify(XmgrCas_CiMmiContext_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR 							hError = ERR_FAIL;
	HUINT32							ulSlotNumber = 0;
	DxCAS_MmiSessionType_e			eSessionType = eDxCAS_SESSIONTYPE_NONE;
	OxMgrCasUiUserInputNotify_t		*pstUserInputNotify = NULL;
	SvcCas_CtrlParam_t				stParam;
	CI_MmiAnswerInParam_t			stInParamAnswer;
	CI_MmiSelectInParam_t			stInParamSelect;
	XmgrCas_CiMmiUiInfo_t			*pstUiInfo = NULL;

	HxLOG_Trace();

	ulSlotNumber = (HUINT32)p1;
	eSessionType = (DxCAS_MmiSessionType_e)p2;
	pstUserInputNotify = (OxMgrCasUiUserInputNotify_t *)p3;

	switch(pstContext->eCurrentUi)
	{
		case eCI_MMI_CURRENT_UI_Menu:
			pstUiInfo = &pstContext->stMenuInfo;
			break;
		case eCI_MMI_CURRENT_UI_Extension:
			pstUiInfo = &pstContext->stExtensionInfo;
			break;
		default:
			HxLOG_Error("ERROR - wrong UI Type!!\n");
			return ERR_FAIL;
	}

	if (eSessionType != eDxCAS_SESSIONTYPE_EXTENSION && eSessionType != eDxCAS_SESSIONTYPE_MENU)
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
					if(pstUiInfo->hMmi == HANDLE_NULL)
					{
						HxLOG_Error("\n");
						xmgr_cas_CiMmiPostSessionEventToOM(pstUiInfo, ulSlotNumber, eMEVT_CASUI_REQ_CLOSE_SESSION);
					}
					else
					{
						if(pstUiInfo->eMmiType == eCiMmiType_Enq)
						{
							HxLOG_Info("eMgrCasEtc_Previous, MmiAnswer, Enq Cancel\n");
							stInParamAnswer.hMmi			= pstUiInfo->hMmi;
							stInParamAnswer.pucString		= NULL;
							stInParamAnswer.usStringLength	= 0;

							stParam.pvIn					= &stInParamAnswer;
							stParam.ulControlType			= eCACTRL_CI_MMI_Answer;

							hError = SVC_CAS_CtrlCall(eDxCAS_GROUPID_CAM, eCACTRL_CI_MMI_Answer, &stParam);
							if(hError != ERR_OK)
							{
								ERR_LOG("Critical Error!!!\n");
							}
						}
						else
						{
							HxLOG_Info("eMgrCasEtc_Previous, MmiSelect Finished\n");

							stInParamSelect.hMmi			= pstUiInfo->hMmi;
							stInParamSelect.ucItemNumber	= 0;
							stParam.pvIn 					= &stInParamSelect;
							stParam.ulControlType			= eCACTRL_CI_MMI_Select;

							hError = SVC_CAS_CtrlCall(eDxCAS_GROUPID_CAM, eCACTRL_CI_MMI_Select, &stParam);
							if(hError != ERR_OK)
							{
								ERR_LOG("Critical Error!!!\n");
							}
						}
					}
					break;

				case eMgrCasEtc_Exit:
					HxLOG_Info("eMgrCasEtc_Exit\n");
					/*
						2009.04.09. GiGaCam Issue, Close�� Cam�� ������ ���� �� ����.
						���� �����ϱ� ���ؼ� �߰�.
						�� Ű�� ���ؼ��� ���� ���Ḧ �����ص� ������ �� ����.
					*/
					hError = xmgr_cas_CiMmiRequestMMICloseToMW(pstUiInfo->hMmi);
					if(hError != ERR_OK)
					{
						ERR_LOG("Critical Error!!!\n");
					}
					xmgr_cas_CiMmiPostSessionEventToOM(pstUiInfo, ulSlotNumber, eMEVT_CASUI_REQ_CLOSE_SESSION);
					break;

				default:
					break;
			}
			return ERR_BUS_MESSAGE_BREAK;
			break;

		case eMgrCasInputType_Number:
			HxLOG_Info("eMgrCasInputType_Number\n");
			if(pstUiInfo->hMmi == HANDLE_NULL)
			{
				ERR_LOG("\n");
				return MESSAGE_BREAK;
			}

			HxLOG_Info("[%s:%d] pszNumber(%s), Length(%d) \n", pstUserInputNotify->utData.stNumber.pszNumber, pstUserInputNotify->utData.stNumber.ulNumberLength);

			stInParamAnswer.hMmi			= pstUiInfo->hMmi;
			stInParamAnswer.pucString		= pstUserInputNotify->utData.stNumber.pszNumber;
			stInParamAnswer.usStringLength	= pstUserInputNotify->utData.stNumber.ulNumberLength;

			stParam.pvIn					= &stInParamAnswer;
			stParam.ulControlType			= eCACTRL_CI_MMI_Answer;

			hError = SVC_CAS_CtrlCall(eDxCAS_GROUPID_CAM, eCACTRL_CI_MMI_Answer, &stParam);
			if(hError != ERR_OK)
			{
				ERR_LOG("Critical Error!!!\n");
			}

			HxLOG_Info("[%s:%d] InputComplete, MmiAnswer Finished\n");
			return ERR_BUS_MESSAGE_BREAK;
			break;

		case eMgrCasInputType_Selection:
			switch(pstUiInfo->eMmiType)
			{
				case eCiMmiType_Text:
					break;

				case eCiMmiType_Menu:
					if(pstUiInfo->hMmi == HANDLE_NULL)
					{
						ERR_LOG("\n");
						return MESSAGE_BREAK;
					}

					stInParamSelect.hMmi			= pstUiInfo->hMmi;
					stInParamSelect.ucItemNumber	= pstUserInputNotify->utData.stSelection.ulItemIdx+1;
					stParam.pvIn 					= &stInParamSelect;
					stParam.ulControlType			= eCACTRL_CI_MMI_Select;
					hError = SVC_CAS_CtrlCall(eDxCAS_GROUPID_CAM, eCACTRL_CI_MMI_Select, &stParam);
					if(hError != ERR_OK)
					{
						ERR_LOG("Critical Error!!!\n");
					}

					HxLOG_Info("Menu, MmiSelect Finished\n");
					break;

				case eCiMmiType_List:
					/*
						List Type�� List Select�� '0' �� ������.
					*/
					stInParamSelect.hMmi			= pstUiInfo->hMmi;
					stInParamSelect.ucItemNumber	= 0;
					stParam.pvIn 					= &stInParamSelect;
					stParam.ulControlType			= eCACTRL_CI_MMI_Select;
					hError = SVC_CAS_CtrlCall(eDxCAS_GROUPID_CAM, eCACTRL_CI_MMI_Select, &stParam);
					if(hError != ERR_OK)
					{
						ERR_LOG("Critical Error!!!\n");
					}

					HxLOG_Info("List, MmiSelect Finished\n");
					break;

				case eCiMmiType_Enq:
					break;

				default:
					break;
			}
			break;

		default:
			break;
	}

	FuncLEAVE(0);

	return ERR_BUS_MESSAGE_BREAK;
}

/*
STATIC BUS_Result_t xmgr_cas_CiMmiMSG_GWM_DESTROY(XmgrCas_CiMmiContext_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR hErr;
	HxLOG_Info("curSlotId(%d) hMmi(0x%X) bNeedSsClose(%d) ", s_usCurSlotId,
																//(HUINT32)pstContext->astMmiStatus[pstContext->usCurSlotId].hMmi,
																(HUINT32)pstContext->stMmiStatus.hMmi,
																pstContext->bNeedSsClose);

	if (xmgr_cas_CiMmiisValidSlotNum(s_usCurSlotId) != TRUE)
	{
		// �̷� ��Ȳ�� �߻��ϸ� ����� �ؾ� �� !!
		HxLOG_Error("\n\n");
	}
	else
	{
		//if(pstContext->astMmiStatus[pstContext->usCurSlotId].hMmi == HANDLE_NULL)
		if(pstContext->stMmiStatus.hMmi == HANDLE_NULL)
		{
			// �̷� ��Ȳ�� �߻��ϸ� ����� �ؾ� �� !!
			HxLOG_Error("\n\n");
		}
		else if (pstContext->bNeedSsClose != FALSE)
		{
			hErr = xmgr_cas_CiMmiRequestMMICloseToMW(pstContext->stMmiStatus.hMmi);
			if(hErr != ERR_OK)
			{
				ERR_LOG("Critical Error!!!\n");
			}
		}
	}

	return ERR_BUS_NO_ERROR;
}
*/

STATIC void xmgr_cas_CiMmiInitContext(XmgrCas_CiMmiContext_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	/**
	  *	p1 : slot id
	  * p2 : sessionTYPE
	  *	p3 : data verison
	**/
	HUINT16 				usParamSlotId = (HUINT16)p1;

	pstContext->stExtensionInfo.hMmi 			= HANDLE_NULL;
	pstContext->stExtensionInfo.eSessionType	= eDxCAS_SESSIONTYPE_EXTENSION;
	pstContext->stExtensionInfo.hSession 		= HANDLE_NULL;
	pstContext->stExtensionInfo.eSessionStatus 	= eCI_MMI_SESSION_STATUS_Stopped;
	pstContext->stExtensionInfo.eMmiType 		= eCiMmiType_Max;

	pstContext->stMenuInfo.hMmi 				= HANDLE_NULL;
	pstContext->stMenuInfo.eSessionType 		= eDxCAS_SESSIONTYPE_MENU;
	pstContext->stMenuInfo.hSession 			= HANDLE_NULL;
	pstContext->stMenuInfo.eSessionStatus 		= eCI_MMI_SESSION_STATUS_Stopped;
	pstContext->stMenuInfo.eMmiType 			= eCiMmiType_Max;

	pstContext->ulUiHandle = (HUINT32)xproc_cas_CiMmi;
	pstContext->usSlotId = usParamSlotId;

	pstContext->eCurrentUi = eCI_MMI_CURRENT_UI_Max;

	return;
}

STATIC void xmgr_cas_CiMmiClearContext(XmgrCas_CiMmiContext_t *pstContext)
{
	if(pstContext->stMenuInfo.hSession != HANDLE_NULL)
	{
		xmgr_cas_CiMmiPostSessionEventToOM(&pstContext->stMenuInfo, pstContext->usSlotId, eMEVT_CASUI_REQ_CLOSE_SESSION);
	}

	if(pstContext->stExtensionInfo.hSession != HANDLE_NULL)
	{
		xmgr_cas_CiMmiPostSessionEventToOM(&pstContext->stExtensionInfo, pstContext->usSlotId, eMEVT_CASUI_REQ_CLOSE_SESSION);
	}


	pstContext->stMenuInfo.hMmi 			= HANDLE_NULL;
	pstContext->stMenuInfo.hSession 		= HANDLE_NULL;
	pstContext->stMenuInfo.eSessionStatus	= eCI_MMI_SESSION_STATUS_Stopped;
	pstContext->stMenuInfo.eMmiType 		= eCiMmiType_Max;
	pstContext->stMenuInfo.pstEnqParam		= NULL;
	pstContext->stMenuInfo.pstListParam		= NULL;
	pstContext->stMenuInfo.pszTextParam		= NULL;

	pstContext->stExtensionInfo.hMmi 			= HANDLE_NULL;
	pstContext->stExtensionInfo.hSession 		= HANDLE_NULL;
	pstContext->stExtensionInfo.eSessionStatus	= eCI_MMI_SESSION_STATUS_Stopped;
	pstContext->stExtensionInfo.eMmiType 		= eCiMmiType_Max;
	pstContext->stExtensionInfo.pstEnqParam		= NULL;
	pstContext->stExtensionInfo.pstListParam	= NULL;
	pstContext->stExtensionInfo.pszTextParam	= NULL;

	pstContext->eCurrentUi 		= eCI_MMI_CURRENT_UI_Max;

	return;
}

BUS_Result_t xproc_cas_CiMmi(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t			hBusResult = MESSAGE_PASS;
	XmgrCas_CiMmiContext_t	*pstContext = NULL;
	HUINT32 				ulMaxSlotNumber = 0, i = 0, ulSlot = 0;
	OxMgrCasSessionEvent_t	*pstSessionEvent = NULL;
	XmgrCas_CiMmiUiInfo_t	*pstUiInfo = NULL;

	switch(message)
	{
		case eMEVT_BUS_CREATE:
			HxLOG_Info("Message : eMEVT_BUS_CREATE Handle (0x%x), p1(0x%x), p2(0x%x), p3(0x%x) \n", hAction, p1, p2, p3);
			BUS_MGR_SetMgrName(BUS_MGR_GetThis(), (HINT8 *)__FUNCTION__);
			SVC_CAS_GetNumOfCISlot(&ulMaxSlotNumber);
			// Init context
			/* �տ��� alloc�� pstContext->astMmiStatus�� invalid�ϰ� ����� block ��Ŵ. */

			for(i = 0;i < ulMaxSlotNumber;i++)
			{
				pstContext = xmgr_cas_CiMmiGetContext(i);
				xmgr_cas_CiMmiInitContext(pstContext, message, hAction, (HUINT32)i, p2, p3);
			}
			hBusResult = MESSAGE_BREAK;
			break;

		/**
		  * 		MMI Data message
		**/
		case eMEVT_CAS_MMI_OPEN:
		case eMEVT_CAS_MMI_TEXT:
		case eMEVT_CAS_MMI_ENQ:
		case eMEVT_CAS_MMI_MENU:
		case eMEVT_CAS_MMI_LIST:
		case eMEVT_CAS_MMI_CLOSE:
			ulSlot = p1;
			pstContext = xmgr_cas_CiMmiGetContext(ulSlot);
			xmgr_cas_CiMmiMmiDataProc(pstContext, message, hAction, p1, p2, p3);
			hBusResult = MESSAGE_BREAK;
			break;

		case eMEVT_CAS_CAM_REMOVED :
			HxLOG_Info("eMEVT_CAS_CAM_REMOVED...\n");
			ulSlot = (HUINT32)p1;
			pstContext = xmgr_cas_CiMmiGetContext(ulSlot);
			xmgr_cas_CiMmiClearContext(pstContext);
			hBusResult = MESSAGE_BREAK;
			break;

		case eMEVT_BUS_TIMER:
			pstContext = xmgr_cas_CiMmiGetContext(s_usSlotIdForDelayedSsClose);
			HxLOG_Info("Message : eMEVT_BUS_TIMER Handle (0x%x), p1(0x%x), p2(0x%x), p3(0x%x) \n", hAction, p1, p2, p3);

			switch(pstContext->eCurrentUi)
			{
				case eCI_MMI_CURRENT_UI_Menu:
					pstUiInfo = &pstContext->stMenuInfo;
					break;
				case eCI_MMI_CURRENT_UI_Extension:
					pstUiInfo = &pstContext->stExtensionInfo;
					break;
				default:
					HxLOG_Error("ERROR - wrong UI Type!!\n");
					if (p1 == MMI_EXP_TIMER_ID)
					{
						return MESSAGE_BREAK;
					}
					break;
			}

			switch(p1)
			{
				case MMI_EXP_TIMER_ID:
					if (xmgr_cas_CiMmiRequestMMICloseToMW(pstUiInfo->hMmi) != ERR_OK)
						ERR_LOG("Critical Error!!!\n");

					xmgr_cas_CiMmiPostSessionEventToOM(pstUiInfo, pstContext->usSlotId, eMEVT_CASUI_REQ_CLOSE_SESSION);
					hBusResult = MESSAGE_BREAK;
					break;

				default:
					break;
			}
			break;

		case eOEVT_CAS_CASUI_APP_OPEN_SESSION:
			pstSessionEvent = (OxMgrCasSessionEvent_t *)p3;
			pstContext = xmgr_cas_CiMmiGetContext(pstSessionEvent->ulCasSlotNum);
			xmgr_cas_CiMmiMsgOpenSession(pstContext, message, hAction, p1, p2, p3);
			hBusResult = MESSAGE_BREAK;
			break;

		case eOEVT_CAS_CASUI_APP_CLOSE_SESSION:
			pstSessionEvent = (OxMgrCasSessionEvent_t *)p3;
			pstContext = xmgr_cas_CiMmiGetContext(pstSessionEvent->ulCasSlotNum);
			xmgr_cas_CiMmiMsgCloseSession(pstContext, message, hAction, p1, p2, p3);
			hBusResult = MESSAGE_BREAK;
			break;

		case eOEVT_CAS_CASUI_APP_USERINPUTDATA:
			ulSlot = p1;
			pstContext = xmgr_cas_CiMmiGetContext(ulSlot);
			hBusResult = xmgr_cas_CiMmiMsgInputNotify(pstContext, message, hAction, p1, p2, p3);
			break;

		case eMEVT_BUS_DESTROY:
			ulSlot = p1;
//			pstContext = xmgr_cas_CiMmiGetContext(ulSlot);
			HxLOG_Info("Message : eMEVT_BUS_DESTROY, Handle (0x%x), p1(0x%x), p2(0x%x), p3(0x%x)\n", hAction, p1, p2, p3);
//			xmgr_cas_CiMmiMSG_GWM_DESTROY(pstContext, message, hAction, p1, p2, p3);
			break;

		case eMEVT_BUS_READY_SHUTDOWN:
			return ERR_BUS_MESSAGE_PASS;

		default:
			break;
	}

	if(hBusResult != MESSAGE_BREAK)
	{
		hBusResult = BUS_ProcessMessageDefault(message, hAction, p1, p2, p3);
	}

	return hBusResult;
}

/* EOF */

