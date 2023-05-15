/**
	@file     ap_cas_ir_menu.c
	@brief    file_name & simple comment.

	Description: Application for Navigation Settings Menu\n
	Module: Application(Layer), Menu(module) \n
	Remarks : \n
	 Date			Modification				Name\n
	-----------	-------------------------------------	------------------\n
	 2009/07/20		initiated					mhoh\n

	Copyright (c) 2009 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#if defined(CONFIG_MW_CAS_IRDETO_UCAS)
//******************************************************************************
//	INCLUDE
//******************************************************************************
#include <octo_common.h>


#include <mgr_cas.h>
#include <xmgr_cas_res_str.h>

#include "../local_include/_xmgr_cas_ir.h"
#include "../local_include/_xmgr_cas_ir_adapt.h"
#include "../local_include/_xmgr_cas_ir_util.h"
#include "../local_include/_xmgr_cas_ir_ui.h"

#include <ir_api.h>

#include <ir_ctrl.h>
#include <svc_cas.h>

#include <svc_pipe.h>

#if defined(CONFIG_MW_CAS_IRDETO_UCAS)
#include <iruc_api.h>
#else
#include <s3_service.h>
#include <ir_status.h>
#endif

#include <xmgr_cas_util_dslib.h>

#include <mgr_common.h>

#include <isosvc.h>
#include <_svc_cas_mgr_control.h>

#include <otl.h>
#include <hlib.h>

//******************************************************************************
//	DEFINE
//******************************************************************************

#define	IR_CA_MAILLIST_INDEX_NUM 				20


/* Application에서는 AP_MEM_Free를 사용하여야 하나  MWC_UTIL_ConvSiStringToUtf8()함수에서  SI_MEM_Alloc을 사용하여 level을 맞추려 부듯이 하게 사용함. */
#define STR_MEM_FREE	OxSI_Free

#define		MACRO_GET_VALID_VALUE(a, b) 		((a == -1) ? a : b)

#if defined(CONFIG_MW_CAS_NUM_SC_SLOT)
#define		NUM_SC_SLOT		CONFIG_MW_CAS_NUM_SC_SLOT
#else
#define		NUM_SC_SLOT		1
#endif

#define CAS_IR_MENU_UPDATE_TIMEOUT_ID				1018

#define CAS_IR_MENU_UPDATE_TIMEOUT					1*1000

typedef enum
{
	eCasIrLoaderStatus_ItemInfo_1,
	eCasIrLoaderStatus_Device,
	eCasIrLoaderStatus_Key,
#if defined(CONFIG_MW_CAS_IRDETO_SECURE)
	eCasIrLoaderStatus_CSSN,
#endif
	eCasIrLoaderStatus_L,
	eCasIrLoaderStatus_ItemInfo_2,
	eCasIrLoaderStatus_FwVersion,
	eCasIrLoaderStatus_LoaderVersion,
	eCasIrLoaderStatus_SerialNo,
	eCasIrLoaderStatus_SpecificInfo,
	eCasIrLoaderStatus_Item_Max
} IR_LoaderStatus_Item_t;

typedef enum
{
	eCasIrClientStatus_CCAVersion,
	eCasIrClientStatus_ClientID,
	eCasIrClientStatus_SecureClient,
	eCasIrClientStatus_SecureCoreVersion,
	eCasIrClientStatus_SecureCore,
	eCasIrClientStatus_DownloadStatus,
	eCasIrClientStatus_TMSData,
	eCasIrClientStatus_EMMCount,
	eCasIrClientStatus_ECMCount,
	eCasIrClientStatus_SoftCell,
	eCasIrClientStatus_Version,
	eCasIrClientStatus_Status,
	eCasIrClientStatus_SmartCard,
	eCasIrClientStatus_ExtendInfo,
	eCasIrClientStatus_ItemInfo_1,
	eCasIrClientStatus_SecureChipInfo,
	eCasIrClientStatus_CSSN,
	eCasIrClientStatus_Item_Max
} IR_ClientStatus_Item_t;

typedef enum
{
	eCasIrProductStatus_ProductCount,
	eCasIrProductStatus_SectorNumber,
	eCasIrProductStatus_ProductId,
	eCasIrProductStatus_StartDate,
	eCasIrProductStatus_Duration,
	eCasIrProductStatus_Entitled,
	eCasIrProductStatus_ProductType,
	eCasIrProductStatus_Item_Max
} IR_ProductStatus_Item_t;

typedef enum
{
/* TODO: 관련 enum 값이 어케 연결 되는지 확인 해라. */
	eCasIrSSSItem_EMM_SvcHandle,
	eCasIrSSSItem_EMM_GlobalSvcStatus,
	eCasIrSSSItem_EMM_InternalStatus,
	eCasIrSSSItem_EMM_SmartCard,
	eCasIrSSSItem_EMM_EMMSource,
	eCasIrSSSItem_EMM_EMMPID,
	eCasIrSSSItem_EMM_EmptyLine,
	eCasIrSSSItem_EMM_EMMValue,

/* CCA */
	eCasIrSSSItem_EMM_CCASvcHandle,
	eCasIrSSSItem_EMM_CCASvcStatus,
	eCasIrSSSItem_EMM_CCAEMMMonitorInfo,
/* Smartcard */
	eCasIrSSSItem_EMM_SCSvcHandle,
	eCasIrSSSItem_EMM_SCSvcStatus,
	eCasIrSSSItem_EMM_SCEMMMonitorInfo,

	eCasIrSSSItem_EMM_SubItemMax
} IR_SSS_SUB_EMMItem_t;

typedef enum
{
/* CCA */
	eCasIrSSSItem_IRUC_EMM_CCAEmmService,
	eCasIrSSSItem_IRUC_EMM_CCASvcHandle,
	eCasIrSSSItem_IRUC_EMM_CCASvcStatus,
	eCasIrSSSItem_IRUC_EMM_CCAEMMMonitorInfo,

/* For UI INFORM */
	eCasIrSSSItem_IRUC_EMM_Blank,

/* Smartcard */
	eCasIrSSSItem_IRUC_EMM_SCEmmService,
	eCasIrSSSItem_IRUC_EMM_SCSvcHandle,
	eCasIrSSSItem_IRUC_EMM_SCSvcStatus,
	eCasIrSSSItem_IRUC_EMM_SCEMMMonitorInfo,

	eCasIrSSSItem_IRUC_EMM_SubItemMax
} IR_SSS_SUB_IRUC_EMMItem_t;


typedef struct IrSystemContext {
	HUINT32					ulSlot;
	OxMgrCasUiEvtSession_t	stSessionInfo;
	XmgrCas_IrUiSessionStatus_e	eSessionStatus;
	XmgrCas_IrMenuGroup_e				eActiveMenuGroup;
} IrMenuContext_t;

typedef struct {
	HUINT8							ucNumOfItem;		/* 아이템의 개수 */
	HUINT8							*pszTitle;			/* 타이틀 영역에 표시될 스트링 */
	HUINT8							*pszSubTitle;		/* 서브타이틀 영역에 표시될 스트링 */
	HUINT8							*pszBottom;			/* 하단 영역에 표시된 스트링 */
	OxMgrCasUiMail_t					**ppMailItem;			/* 각 mail item */
}CAS_MmiMailList_t;


//******************************************************************************
//	STATIC VARIABLE
//******************************************************************************

STATIC IrMenuContext_t	s_IrMenuContext;


 //for softcell service status
STATIC IR_SVC_STATUS_INFO_MAIN	*s_pstSvcStatusInfo = NULL;
STATIC HUINT8						s_CASIR_SSS_SubItem_Index = 0;
STATIC HUINT8						s_CASIR_SSS_Item_Count = 0;
STATIC IRUC_SERVICE_STATUS			s_stSvcStatusInfoEach[eIRUC_STATUS_Info_MAX]; // ecm emm ... 나중에  pvr 이나 item 증가하면 배열의 인덱스 늘려야 한다.

STATIC HBOOL						s_bInSoftcellServiceStatusSubItem = FALSE;

STATIC HUINT8 					s_aucMailIdxList[IR_CA_MAILLIST_INDEX_NUM] = {0, };

STATIC HUINT32					s_ulStrHash = 0;

//******************************************************************************
//	GLOBAL VARIABLE
//******************************************************************************



//******************************************************************************
//	CODE START
//******************************************************************************
STATIC IrMenuContext_t	*xmgr_cas_iruc_menu_GetContext(void);
STATIC HERROR xmgr_cas_iruc_menu_MsgGwmCreate(IrMenuContext_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
STATIC HERROR xmgr_cas_iruc_menu_Destroy(IrMenuContext_t *pstContext);
STATIC BUS_Result_t xmgr_cas_iruc_menu_MsgReqSessionStart(IrMenuContext_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
STATIC BUS_Result_t xmgr_cas_iruc_menu_MsgReqSessionStop(IrMenuContext_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
STATIC BUS_Result_t xmgr_cas_iruc_menu_MsgReqSessionStarted(IrMenuContext_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
STATIC BUS_Result_t xmgr_cas_iruc_menu_MsgReqSessionStopped(IrMenuContext_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);

STATIC HCHAR *xmgr_cas_iruc_menu_EncodeString(HCHAR *string);
STATIC HERROR xmgr_cas_iruc_menu_UpdateData(IrMenuContext_t *pstContext);
STATIC void xmgr_cas_iruc_menu_UpdateCheck(IrMenuContext_t *pstContext);

STATIC HERROR xmgr_cas_iruc_menu_PostMenuListData(IrMenuContext_t *pstContext, HUINT32 ulSlotId, SvcCas_MmiMenuList_t *pMmiMenuData, OxMgrCasUIAttribute_e eDefaultAttr);
STATIC HERROR xmgr_cas_iruc_menu_PostMailListData(IrMenuContext_t *pstContext, HUINT32 ulSlotId, CAS_MmiMailList_t *pMmiMailData, OxMgrCasUIAttribute_e eDefaultAttr);
#if 0
STATIC HERROR xmgr_cas_iruc_menu_PostTextData(IrMenuContext_t *pstContext, HUINT32 ulSlotId, HUINT8 *pszMessage, OxMgrCasUIAttribute_e eDefaultAttr);
#endif

STATIC HERROR xmgr_cas_iruc_menu_SetLoaderStatusParam (IR_LoaderStatus_Item_t eItemId, HUINT8 *pszParam);
STATIC HERROR xmgr_cas_iruc_menu_MakeLoaderStatusMenuListData(SvcCas_MmiMenuList_t * pstMmiList);

STATIC HERROR xmgr_cas_iruc_menu_SetMailMessageParam (HUINT8 ucIdx, HUINT8 ucMailIndex, OxMgrCasUiMail_t *ppMailItem);
STATIC HERROR xmgr_cas_iruc_menu_MakeMailListData(CAS_MmiMailList_t * pstMmiMailList);
STATIC HERROR xmgr_cas_iruc_menu_MsgInputNotifyForMailMessages(IrMenuContext_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);

STATIC HERROR xmgr_cas_iruc_menu_MakeClientStatusMenuListData(SvcCas_MmiMenuList_t * pstMmiList);
STATIC HERROR xmgr_cas_iruc_menu_SetClientStatusParam(HUINT8 ucSlot, IR_ClientStatus_Item_t eItemId, HUINT8 *pszParam);
STATIC HERROR xmgr_cas_iruc_menu_MakeProductStatusMenuListData(SvcCas_MmiMenuList_t * pstMmiList);
#if 0
STATIC HERROR xmgr_cas_iruc_menu_SetProductStatusParam(HUINT8 ucSlot, IR_ProductStatus_Item_t eItemId, HUINT8 *pszParam);
#endif
STATIC HERROR xmgr_cas_iruc_menu_UpdateUCServiceStatusInfomation(void);
STATIC HERROR xmgr_cas_iruc_menu_SetUCServiceStatusParam (HUINT32 eItemId, HUINT8 *pszParam);
STATIC HERROR xmgr_cas_iruc_menu_MakeUCServiceStatusMenuListData(SvcCas_MmiMenuList_t * pstMmiList);
STATIC HERROR xmgr_cas_iruc_menu_MsgInputNotifyForUCServiceStatus(IrMenuContext_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
STATIC HERROR xmgr_cas_iruc_menu_UpdateDataForUCServiceStatusSubMenu(IrMenuContext_t *pstContext);


STATIC HERROR xmgr_cas_iruc_menu_MakeEmmMenuListData(SvcCas_MmiMenuList_t * pstMmiList);
STATIC HERROR xmgr_cas_iruc_menu_SetEmmParam(IR_SSS_SUB_EMMItem_t eItemId, HUINT8 *pszParam);
STATIC HERROR xmgr_cas_iruc_menu_MakeEcmMenuListData(SvcCas_MmiMenuList_t * pstMmiList);



#define	_________________________BODY______________________________



#define _____Local_Menu_Main____________________________________________________

STATIC IrMenuContext_t	*xmgr_cas_iruc_menu_GetContext(void)
{
	return &s_IrMenuContext;
}

STATIC HERROR		xmgr_cas_iruc_menu_MsgGwmCreate(IrMenuContext_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32				ulSlot = (HUINT32)p1;
	DxCAS_MmiSessionType_e	eSessionType = (DxCAS_MmiSessionType_e)p2;
	XmgrCas_IrMenuGroup_e			eIrMenuGrp = (XmgrCas_IrMenuGroup_e)p3;


	HxLOG_Info("xmgr_cas_iruc_menu_MsgGwmCreate() eIrMenuGrp: %d \n", eIrMenuGrp);

	/* param check */
	if(pstContext == NULL)
	{
		HxLOG_Critical("\n\n");

		return ERR_FAIL;
	}

	if(eSessionType != eDxCAS_SESSIONTYPE_MENU)
	{
		HxLOG_Error("\n\n");
		return ERR_FAIL;
	}

	if(eIrMenuGrp <= eCasIrUCMenuGroup_None || eIrMenuGrp >= eCasIrUCMenuGroup_MAX)
	{
		HxLOG_Error("\n\n");
		return ERR_FAIL;
	}



	HxSTD_memset(pstContext, 0, sizeof(IrMenuContext_t));

	pstContext->ulSlot = ulSlot;
	pstContext->stSessionInfo.eSessionType = eSessionType;
	pstContext->eActiveMenuGroup = eIrMenuGrp;
	pstContext->stSessionInfo.ulUiHandle = (HUINT32)xproc_cas_IrUC;
	pstContext->eSessionStatus = eIR_UI_SESSION_STATUS_Stopped;

	HxLOG_Info("xmgr_cas_iruc_menu_MsgGwmCreate() return OK,  pstContext->ulSlot: %d, pstContext->eSessionType: %d, pstContext->eActiveMenuGroup: %d\n", pstContext->ulSlot, pstContext->stSessionInfo.eSessionType, pstContext->eActiveMenuGroup);

	return ERR_OK;
}


STATIC HERROR		xmgr_cas_iruc_menu_Destroy(IrMenuContext_t *pstContext)
{
	if(pstContext->eActiveMenuGroup == eCasIrUCMenuGroup_ServiceStatus)
	{
		xsvc_cas_IrUcStopMonitoring();
	}
	s_ulStrHash = 0;

	return ERR_OK;
}

STATIC HERROR	xmgr_cas_iruc_menu_PostMessageSessionEventToOM(IrMenuContext_t *pstContext, DxCAS_MmiSessionType_e eSessionType, HUINT16 usSlotId, HINT32 eEvent)
{
	HERROR								hError = ERR_OK;
	BUS_Result_t						hResult = ERR_BUS_NO_ERROR;
	OxMgrCasUiEvtSession_t				*pstEvent = NULL;

	pstEvent = HLIB_STD_MemCalloc(sizeof(OxMgrCasUiEvtSession_t));
	if(pstEvent == (OxMgrCasUiEvtSession_t *)NULL)
	{
		HxLOG_Error("\n");
		hError = ERR_FAIL;
		goto END_FUNC;
	}

	//pstEvent->eSessionType = pstContext->eSessionType;
	pstEvent->ulUiHandle = pstContext->stSessionInfo.ulUiHandle;
	pstEvent->hSession = pstContext->stSessionInfo.hSession;
	pstEvent->eSessionType = eSessionType;

	hResult = BUS_PostMessageToOM(NULL, eEvent, HANDLE_NULL, eDxCAS_GROUPID_IR, /*pstContext->usCurSlotId*/usSlotId, (HINT32)pstEvent);
	if(hResult != ERR_BUS_NO_ERROR)
	{
		hError = ERR_FAIL;
		goto END_FUNC;
	}

END_FUNC:
	return hError;
}


STATIC BUS_Result_t xmgr_cas_iruc_menu_MsgReqSessionStart(IrMenuContext_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	OxMgrCasSessionEvent_t 				stSessionEvent;
	DxCAS_MmiSessionType_e			eSessionType;
	HUINT32 							ulSlot;
	Handle_t							hSession;
	XmgrCas_IrMenuGroup_e				eIrMenuGrp;

	HxSTD_memcpy(&stSessionEvent, (OxMgrCasSessionEvent_t*)p3, sizeof(OxMgrCasSessionEvent_t));
	ulSlot = stSessionEvent.ulCasSlotNum;
	eSessionType= stSessionEvent.eSessionType;
	eIrMenuGrp = (XmgrCas_IrMenuGroup_e)stSessionEvent.eCasMenuGroupId;
	hSession = stSessionEvent.hSession;


	HxLOG_Info("SlotNumber %d, SESSIONTYPE %d, SESSIONSTATUS %d, hAction: 0x%x, pstContext->stSessionInfo.hSession: 0x%x\n",
		ulSlot, eSessionType, pstContext->eSessionStatus, hAction, pstContext->stSessionInfo.hSession);
	pstContext->stSessionInfo.hSession = hSession;
	pstContext->eActiveMenuGroup = eIrMenuGrp;

	switch(pstContext->eSessionStatus)
	{
		case eIR_UI_SESSION_STATUS_Stopped:
			pstContext->eSessionStatus = eIR_UI_SESSION_STATUS_Started;
			/* continue */
		case eIR_UI_SESSION_STATUS_Started:
			if(pstContext->stSessionInfo.eSessionType == eSessionType)
			{
				HxLOG_Info("session started already!!\n");
				xmgr_cas_iruc_menu_PostMessageSessionEventToOM(pstContext, eSessionType, ulSlot, eMEVT_CASUI_SESSION_OPENED);
				xmgr_cas_iruc_menu_UpdateData(pstContext);
			}
			break;


		default:
			break;
	}


	return MESSAGE_BREAK;
}


STATIC BUS_Result_t xmgr_cas_iruc_menu_MsgReqSessionStop(IrMenuContext_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	OxMgrCasSessionEvent_t 				stSessionEvent;
	DxCAS_MmiSessionType_e			eSessionType;
	HUINT32 							ulSlotNumber;
	Handle_t							hSession;
	XmgrCas_IrMenuGroup_e				eIrMenuGrp;

	HxSTD_memcpy(&stSessionEvent, (OxMgrCasSessionEvent_t*)p3, sizeof(OxMgrCasSessionEvent_t));
	ulSlotNumber = stSessionEvent.ulCasSlotNum;
	eSessionType= stSessionEvent.eSessionType;
	eIrMenuGrp = (XmgrCas_IrMenuGroup_e)stSessionEvent.eCasMenuGroupId;
	hSession = stSessionEvent.hSession;

	ulSlotNumber = (HUINT32)p1;
	eSessionType = (DxCAS_MmiSessionType_e)p2;

	HxLOG_Info("SlotNumber %d, SESSIONTPYE %d, SESSIONSTATUS %d\n",
		ulSlotNumber, eSessionType, pstContext->eSessionStatus);

	// TODO: shko 여기서 데이터 정리 필요 ?


	return MESSAGE_BREAK;
}

STATIC BUS_Result_t xmgr_cas_iruc_menu_MsgReqSessionStarted(IrMenuContext_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32					ulSlotNumber = 0;
	DxCAS_MmiSessionType_e		eSessionType = eDxCAS_SESSIONTYPE_NONE;

	HxLOG_Info("p1: %d, p2: %d, p3: %d \n", p1, p2, p3);

	ulSlotNumber = (HUINT32)p1;
	eSessionType = (DxCAS_MmiSessionType_e)p2;

	switch(pstContext->eSessionStatus)
	{
		case eIR_UI_SESSION_STATUS_Stopped:
			pstContext->eSessionStatus = eIR_UI_SESSION_STATUS_Started;
			// continue
		case eIR_UI_SESSION_STATUS_Started:
			HxLOG_Info("session status started\n");
			xmgr_cas_iruc_menu_UpdateData(pstContext);
			break;

		default:
			break;
	}


	return MESSAGE_BREAK;
}

STATIC BUS_Result_t xmgr_cas_iruc_menu_MsgReqSessionStopped(IrMenuContext_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32					ulSlotNumber = 0;
	DxCAS_MmiSessionType_e		eSessionType = eDxCAS_SESSIONTYPE_NONE;

	ulSlotNumber = (HUINT32)p1;
	eSessionType = (DxCAS_MmiSessionType_e)p2;

	// TODO: shko 여기서 data 정리해버려.


	return MESSAGE_BREAK;
}

STATIC HCHAR *xmgr_cas_iruc_menu_EncodeString(HCHAR *string)
{
	HUINT32		Len = 0;
	HCHAR		*buf = NULL;

	Len = HLIB_STD_URLStrLen(string) + 1;
	buf = HLIB_STD_MemCalloc(Len+1);
	HLIB_STD_URLEncode(buf,string,Len);

	return buf;
}



STATIC HERROR xmgr_cas_iruc_menu_PostMenuListData(IrMenuContext_t *pstContext, HUINT32 ulSlotId, SvcCas_MmiMenuList_t *pMmiMenuData, OxMgrCasUIAttribute_e eDefaultAttr)
{
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

	//pstEvent = xmgr_cas_ir_menu_GetMenuListEventData();
	pstEvent = (OxMgrCasUiEvtMenulist_t *)HLIB_STD_MemCalloc(sizeof(OxMgrCasUiEvtMenulist_t));
	if(pstEvent == (OxMgrCasUiEvtMenulist_t *)NULL)
	{
		HxLOG_Error("\n\n");
		hError = ERR_FAIL;
		goto END_FUNC;
	}
	pstEvent->stSessionInfo.eSessionType = (HUINT32)pstContext->stSessionInfo.eSessionType;
	pstEvent->stSessionInfo.ulUiHandle = (HUINT32)pstContext->stSessionInfo.ulUiHandle;
	pstEvent->stSessionInfo.hSession = (HUINT32)pstContext->stSessionInfo.hSession;
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
				pzEncodedStr = xmgr_cas_iruc_menu_EncodeString(pszText);
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
				pzEncodedStr = xmgr_cas_iruc_menu_EncodeString(pszText);
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
				pzEncodedStr = xmgr_cas_iruc_menu_EncodeString(pszText);
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
			pzEncodedStr = xmgr_cas_iruc_menu_EncodeString(pszText);
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

	hBusResult = BUS_PostMessageToOM(NULL, eMEVT_CASUI_TYPE_MENULIST_DATA, HANDLE_NULL, (HINT32)eDxCAS_GROUPID_IR, (HINT32)ulSlotId, (HINT32)pstEvent);
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
			if(pstEvent->stMenulist.astListItem != NULL)
			{
				HLIB_STD_MemFree(pstEvent->stMenulist.astListItem);
				pstEvent->stMenulist.astListItem = NULL;
			}
			HLIB_STD_MemFree(pstEvent);
			pstEvent = NULL;
		}
	}

	return hError;
}


STATIC HERROR xmgr_cas_iruc_menu_PostMailListData(IrMenuContext_t *pstContext, HUINT32 ulSlotId, CAS_MmiMailList_t *pMmiMailData, OxMgrCasUIAttribute_e eDefaultAttr)
{
	HERROR						hError = ERR_OK;
	HUINT8						*pszText = NULL;
	HUINT32						ulStrLen = 0;
	HUINT32						ulMailItemLoop = 0;
	BUS_Result_t				hBusResult = ERR_BUS_NO_ERROR;
	OxMgrCasUiEvtMailList_t		*pstEvent = NULL;
	OxMgrCasUiMailList_t		*pstMailList = NULL;
	OxMgrCasUiMail_t			*pMailItem = NULL;
	HINT32					slStream;
	HCHAR					*pzEncodedStr = NULL;
	HCHAR					*pzProperties = NULL;
	HUINT32					ulPropertyLen = 0;

	//pstEvent = xmgr_cas_ir_menu_GetMailListEventData();
	pstEvent = (OxMgrCasUiEvtMailList_t *)HLIB_STD_MemCalloc(sizeof(OxMgrCasUiEvtMailList_t));
	if(pstEvent == (OxMgrCasUiEvtMailList_t *)NULL)
	{
		HxLOG_Error("\n\n");
		hError = ERR_FAIL;
		goto END_FUNC;
	}

	pstEvent->stSessionInfo.eSessionType = (HUINT32)pstContext->stSessionInfo.eSessionType;
	pstEvent->stSessionInfo.ulUiHandle = (HUINT32)pstContext->stSessionInfo.ulUiHandle;
	pstEvent->stSessionInfo.hSession = (HUINT32)pstContext->stSessionInfo.hSession;
	pstMailList = &(pstEvent->stMailList);

	pstMailList->stHeader.ulAttr = eDefaultAttr;
	pstMailList->stHeader.ulFocusIndex = 0;
	pstMailList->stHeader.ulStartIndex = 0;
	pstMailList->ulTotalNum = pMmiMailData->ucNumOfItem;

	if(pMmiMailData->pszTitle != NULL)
	{
		ulStrLen = SvcCas_UtilStrLen(pMmiMailData->pszTitle);
		if(ulStrLen != 0)
		{
			if(ulStrLen >= MGR_CAS_STR_LENGTH_LONG)
			{
				HxLOG_Warning("string length overflow %d\n", ulStrLen);
				ulStrLen = MGR_CAS_STR_LENGTH_LONG-1;
			}

			MWC_UTIL_ConvSiStringToUtf8(NULL, ulStrLen, pMmiMailData->pszTitle, &pszText);
			if(pszText != NULL)
			{
				slStream = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
				pzEncodedStr = xmgr_cas_iruc_menu_EncodeString(pszText);
				if(pzEncodedStr != NULL)
				{
					HLIB_RWSTREAM_Print(slStream, "[\"%s\"]", pzEncodedStr);
					HLIB_STD_MemFree(pzEncodedStr);
					pzEncodedStr = NULL;
				}

				pzProperties = (HCHAR*)HLIB_RWSTREAM_CloseWithoutBuf(slStream);

				MWC_UTIL_DvbStrncpy(pstMailList->stHeader.szTitle, pzProperties, MGR_CAS_STR_LENGTH_LONG);
				HLIB_STD_MemFree(pzProperties);
				pstMailList->stHeader.ulAttr |= eMgrCasUiAttr_Title;

				STR_MEM_FREE(pszText);
			}
		}
	}

	if(pMmiMailData->pszSubTitle != NULL)
	{
		ulStrLen = SvcCas_UtilStrLen(pMmiMailData->pszSubTitle);
		if(ulStrLen != 0)
		{
			if(ulStrLen >= MGR_CAS_STR_LENGTH_LONG)
			{
				HxLOG_Warning("string length overflow %d\n", ulStrLen);
				ulStrLen = MGR_CAS_STR_LENGTH_LONG-1;
			}

			MWC_UTIL_ConvSiStringToUtf8(NULL, ulStrLen, pMmiMailData->pszSubTitle, &pszText);
			if(pszText != NULL)
			{
				slStream = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
				pzEncodedStr = xmgr_cas_iruc_menu_EncodeString(pszText);
				if(pzEncodedStr != NULL)
				{
					HLIB_RWSTREAM_Print(slStream, "[\"%s\"]", pzEncodedStr);
					HLIB_STD_MemFree(pzEncodedStr);
					pzEncodedStr = NULL;
				}

				pzProperties = (HCHAR*)HLIB_RWSTREAM_CloseWithoutBuf(slStream);

				MWC_UTIL_DvbStrncpy(pstMailList->stHeader.szSubTitle, pzProperties, MGR_CAS_STR_LENGTH_LONG);
				HLIB_STD_MemFree(pzProperties);

				pstMailList->stHeader.ulAttr |= eMgrCasUiAttr_Subtitle;
				STR_MEM_FREE(pszText);
			}
		}
	}

	if(pMmiMailData->pszBottom != NULL)
	{
		ulStrLen = SvcCas_UtilStrLen(pMmiMailData->pszBottom);
		if(ulStrLen != 0)
		{
			if(ulStrLen >= MGR_CAS_STR_LENGTH_LONG)
			{
				HxLOG_Warning("string length overflow %d\n", ulStrLen);
				ulStrLen = MGR_CAS_STR_LENGTH_LONG-1;
			}

			MWC_UTIL_ConvSiStringToUtf8(NULL, ulStrLen, pMmiMailData->pszBottom, &pszText);
			if(pszText != NULL)
			{
				slStream = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
				pzEncodedStr = xmgr_cas_iruc_menu_EncodeString(pszText);
				if(pzEncodedStr != NULL)
				{
					HLIB_RWSTREAM_Print(slStream, "[\"%s\"]", pzEncodedStr);
					HLIB_STD_MemFree(pzEncodedStr);
					pzEncodedStr = NULL;
				}

				pzProperties = (HCHAR*)HLIB_RWSTREAM_CloseWithoutBuf(slStream);

				MWC_UTIL_DvbStrncpy(pstMailList->stHeader.szBottom, pzProperties, MGR_CAS_STR_LENGTH_LONG);
				HLIB_STD_MemFree(pzProperties);
				pstMailList->stHeader.ulAttr |= eMgrCasUiAttr_Bottom;
				STR_MEM_FREE(pszText);
			}
		}
	}

	HxLOG_Debug("ulTotalNum %d\n", pstMailList->ulTotalNum);

	if(pstMailList->ulTotalNum > 0)
	{
		pstMailList->astListItem = (OxMgrCasUiMail_t *)HLIB_STD_MemCalloc(sizeof(OxMgrCasUiMail_t)*(pstMailList->ulTotalNum));
		if(pstMailList->astListItem == NULL)
		{
			HxLOG_Error("\n\n");
			hError = ERR_FAIL;
				goto END_FUNC;
		}

		for(ulMailItemLoop = 0;ulMailItemLoop < pstMailList->ulTotalNum;ulMailItemLoop++)
		{
			pMailItem = &pstMailList->astListItem[ulMailItemLoop];

			HxSTD_memcpy(&(pMailItem->stHeader), &(pMmiMailData->ppMailItem[ulMailItemLoop]->stHeader), sizeof(OxMgrCasUiMenuHeader_t));

			MWC_UTIL_ConvSiStringToUtf8(NULL, SvcCas_UtilStrLen(pMmiMailData->ppMailItem[ulMailItemLoop]->pszText), pMmiMailData->ppMailItem[ulMailItemLoop]->pszText, &pszText);
			if(pszText != NULL)
			{
				pMailItem->ulTextLength = SvcCas_UtilStrLen(pszText);

				slStream = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
				pzEncodedStr = xmgr_cas_iruc_menu_EncodeString(pszText);
				if(pzEncodedStr != NULL)
				{
					HLIB_RWSTREAM_Print(slStream, "[\"%s\"]", pzEncodedStr);
					HLIB_STD_MemFree(pzEncodedStr);
					pzEncodedStr = NULL;
				}

				pzProperties = (HCHAR*)HLIB_RWSTREAM_CloseWithoutBuf(slStream);
				ulPropertyLen = HxSTD_StrLen(pzProperties);

				pMailItem->pszText = (HUINT8 *)HLIB_STD_MemCalloc(sizeof(HUINT8) * (ulPropertyLen+1));

				MWC_UTIL_DvbStrncpy(pMailItem->pszText, pzProperties, ulPropertyLen);
				//pMailItem->pszText[ulPropertyLen - 1] = '\0';
				HLIB_STD_MemFree(pzProperties);

				HxLOG_Debug("%s\n", pMailItem->pszText);

				STR_MEM_FREE(pszText);
			}
		}
	}

	hBusResult = BUS_PostMessageToOM(NULL, eMEVT_CASUI_TYPE_MAILLIST_DATA, HANDLE_NULL, (HINT32)eDxCAS_GROUPID_IR, (HINT32)ulSlotId, (HINT32)pstEvent);
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
			if(pstMailList->astListItem != NULL)
				{
				if(pstMailList->astListItem->pszText != NULL)
				{
					HLIB_STD_MemFree(pstMailList->astListItem->pszText);
					pstMailList->astListItem->pszText = NULL;
				}
				HLIB_STD_MemFree(pstMailList->astListItem);
				pstMailList->astListItem = NULL;
			}
			HLIB_STD_MemFree(pstEvent);
			pstEvent = NULL;
		}
	}

	return hError;
}


#if 0 // currently not used.
STATIC HERROR xmgr_cas_iruc_menu_PostTextData(IrMenuContext_t *pstContext, HUINT32 ulSlotId, HUINT8 *pszMessage, OxMgrCasUIAttribute_e eDefaultAttr)
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

	//pstTextBoxEvent = xmgr_cas_ir_menu_GetMenuTextEventData();
	pstTextBoxEvent = (OxMgrCasUiEvtTextbox_t *)HLIB_STD_MemCalloc(sizeof(OxMgrCasUiEvtTextbox_t));
	if(pstTextBoxEvent == (OxMgrCasUiEvtTextbox_t *)NULL)
	{
		HxLOG_Error("\n");
		hError = ERR_FAIL;
		goto END_FUNC;
	}

	MWC_UTIL_ConvSiStringToUtf8(NULL, SvcCas_UtilStrLen(pszMessage), pszMessage, &pszText);
	if(pszText == NULL)
	{
		HxLOG_Warning("\n");
		hError = ERR_FAIL;
		goto END_FUNC;
	}

	ulStringLength = SvcCas_UtilStrLen(pszText);

	pstTextBoxEvent->eSessionType = pstContext->eSessionType;
	pstTextBoxEvent->stTextbox.ulAttr = eDefaultAttr;
	pstTextBoxEvent->stTextbox.ulStringLength = ulStringLength;

	slStream = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_STD_MemAlloc, HLIB_STD_MemFree);
	pzEncodedStr = xmgr_cas_iruc_menu_EncodeString(pszText);
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

	hBusResult = BUS_PostMessageToOM(NULL, eMEVT_CASUI_TYPE_TEXTBOX_DATA, HANDLE_NULL, (HINT32)eDxCAS_GROUPID_IR, (HINT32)ulSlotId, (HINT32)pstTextBoxEvent);
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
#endif

STATIC HERROR xmgr_cas_iruc_menu_UpdateData(IrMenuContext_t *pstContext)
{
	HERROR				hError = ERR_FAIL;
	HUINT32				ulMenuItemLoop = 0;
	SvcCas_MmiMenuList_t		*pstMmiList = NULL;
	CAS_MmiMailList_t		*pstMmiMailList = NULL;
	HUINT8				*pstMmmiText = NULL;
	HUINT16				*pusComponentLength = NULL;
	HUINT8				**ppszComponentsString = NULL;

	switch (pstContext->eActiveMenuGroup)
	{
		case eCasIrUCMenuGroup_ServiceStatus:
		{
			//xmgr_cas_iruc_menu_UpdateUCServiceStatusInfomation();

			pstMmiList = (SvcCas_MmiMenuList_t *)HLIB_STD_MemCalloc(sizeof(SvcCas_MmiMenuList_t));

			if(pstMmiList == NULL)
			{
				HxLOG_Error("\n\n");
				hError = ERR_FAIL;
				goto END_FUNC;
			}

			hError = xmgr_cas_iruc_menu_MakeUCServiceStatusMenuListData(pstMmiList);
			if(hError != ERR_OK)
			{
				goto END_FUNC;
			}

			hError = xmgr_cas_iruc_menu_PostMenuListData(pstContext, (HUINT32)pstContext->ulSlot, pstMmiList, 0);
			if(hError != ERR_OK)
			{
				HxLOG_Error("\n\n");
				goto END_FUNC;
			}

		}
		break;
		case eCasIrUCMenuGroup_LoaderStatus:
			{
				pstMmiList = (SvcCas_MmiMenuList_t *)HLIB_STD_MemCalloc(sizeof(SvcCas_MmiMenuList_t));

				if(pstMmiList == NULL)
				{
					HxLOG_Error("\n\n");
					hError = ERR_FAIL;
					goto END_FUNC;
				}

				hError = xmgr_cas_iruc_menu_MakeLoaderStatusMenuListData(pstMmiList);
				if(hError != ERR_OK)
				{
					HxLOG_Error("\n\n");
					goto END_FUNC;
				}

				hError = xmgr_cas_iruc_menu_PostMenuListData(pstContext, (HUINT32)pstContext->ulSlot, pstMmiList, 0);
				if(hError != ERR_OK)
				{
					HxLOG_Error("\n\n");
					goto END_FUNC;
				}
			}
			break;
		case eCasIrUCMenuGroup_ProductStatus:
			/* 어케 만들지 고민 해보자. */
			{
				HxLOG_Error("eCasIrUCMenuGroup_ProductStatus\n");
				pstMmiList = (SvcCas_MmiMenuList_t *)HLIB_STD_MemCalloc(sizeof(SvcCas_MmiMenuList_t));

				if(pstMmiList == NULL)
				{
					HxLOG_Error("\n\n");
					hError = ERR_FAIL;
					goto END_FUNC;
				}

				hError = xmgr_cas_iruc_menu_MakeProductStatusMenuListData(pstMmiList);
				if(hError != ERR_OK)
				{
					HxLOG_Error("\n\n");
					goto END_FUNC;
				}

				hError = xmgr_cas_iruc_menu_PostMenuListData(pstContext, (HUINT32)pstContext->ulSlot, pstMmiList, 0);
				if(hError != ERR_OK)
				{
					HxLOG_Error("\n\n");
					goto END_FUNC;
				}
			}
			break;
		case eCasIrUCMenuGroup_ClientStatus:
			{
				pstMmiList = (SvcCas_MmiMenuList_t *)HLIB_STD_MemCalloc(sizeof(SvcCas_MmiMenuList_t));

				if(pstMmiList == NULL)
				{
					HxLOG_Error("\n\n");
					hError = ERR_FAIL;
					goto END_FUNC;
				}

				hError = xmgr_cas_iruc_menu_MakeClientStatusMenuListData(pstMmiList);
				if(hError != ERR_OK)
				{
					HxLOG_Error("\n\n");
					goto END_FUNC;
				}

				hError = xmgr_cas_iruc_menu_PostMenuListData(pstContext, (HUINT32)pstContext->ulSlot, pstMmiList, 0);
				if(hError != ERR_OK)
				{
					HxLOG_Error("\n\n");
					goto END_FUNC;
				}
			}

			break;

		case eCasIrUCMenuGroup_MailMessages:
			pstMmiMailList = (CAS_MmiMailList_t *)HLIB_STD_MemCalloc(sizeof(CAS_MmiMailList_t));

			if(pstMmiMailList == NULL)
			{
				HxLOG_Error("\n\n");
				hError = ERR_FAIL;
				goto END_FUNC;
			}

			hError = xmgr_cas_iruc_menu_MakeMailListData(pstMmiMailList);
			if(hError != ERR_OK)
			{
				HxLOG_Error("\n\n");
				goto END_FUNC;
			}

			hError = xmgr_cas_iruc_menu_PostMailListData(pstContext, (HUINT32)pstContext->ulSlot, pstMmiMailList, 0);
			if(hError != ERR_OK)
			{
				HxLOG_Error("\n\n");
				goto END_FUNC;
			}
			break;

		case eCasIrUCMenuGroup_None:
		case eCasIrUCMenuGroup_MAX:
		default:
			HxLOG_Error("\n\n");
			hError = ERR_FAIL;
			goto END_FUNC;



	}

END_FUNC:

	if(pstMmiList != NULL)
	{
		if(pstMmiList->pszTitle != NULL)
		{
			HLIB_STD_MemFree(pstMmiList->pszTitle);
			pstMmiList->pszTitle = NULL;
		}

		if(pstMmiList->pszSubTitle != NULL)
		{
			HLIB_STD_MemFree(pstMmiList->pszSubTitle);
			pstMmiList->pszSubTitle = NULL;
		}

		if(pstMmiList->pszBottom != NULL)
		{
			HLIB_STD_MemFree(pstMmiList->pszBottom);
			pstMmiList->pszBottom = NULL;
		}

		if(pstMmiList->ppszItem != NULL)
		{
			for(ulMenuItemLoop = 0; ulMenuItemLoop < pstMmiList->ulNumOfItem; ulMenuItemLoop++)
			{
				if(pstMmiList->ppszItem[ulMenuItemLoop] != NULL)
				{
					HLIB_STD_MemFree(pstMmiList->ppszItem[ulMenuItemLoop]);
					pstMmiList->ppszItem[ulMenuItemLoop] = NULL;
				}
			}
			HLIB_STD_MemFree(pstMmiList->ppszItem);
			pstMmiList->ppszItem = NULL;
		}

		HLIB_STD_MemFree(pstMmiList);
		pstMmiList = NULL;
	}

	if(pstMmiMailList != NULL)
	{
		if(pstMmiMailList->pszTitle != NULL)
		{
			HLIB_STD_MemFree(pstMmiMailList->pszTitle);
			pstMmiMailList->pszTitle = NULL;
		}

		if(pstMmiMailList->pszSubTitle != NULL)
		{
			HLIB_STD_MemFree(pstMmiMailList->pszSubTitle);
			pstMmiMailList->pszSubTitle = NULL;
		}

		if(pstMmiMailList->pszBottom != NULL)
		{
			HLIB_STD_MemFree(pstMmiMailList->pszBottom);
			pstMmiMailList->pszBottom = NULL;
		}

		if(&(pstMmiMailList->ppMailItem) != NULL)
		{
			for(ulMenuItemLoop = 0; ulMenuItemLoop < pstMmiMailList->ucNumOfItem; ulMenuItemLoop++)
			{
				if(pstMmiMailList->ppMailItem[ulMenuItemLoop] != NULL)
				{
					if(pstMmiMailList->ppMailItem[ulMenuItemLoop]->pszText != NULL)
					{
						HLIB_STD_MemFree(pstMmiMailList->ppMailItem[ulMenuItemLoop]->pszText);
						pstMmiMailList->ppMailItem[ulMenuItemLoop]->pszText = NULL;
					}
					HLIB_STD_MemFree(pstMmiMailList->ppMailItem[ulMenuItemLoop]);
					pstMmiMailList->ppMailItem[ulMenuItemLoop] = NULL;
				}
			}
			HLIB_STD_MemFree(pstMmiMailList->ppMailItem);
			(pstMmiMailList->ppMailItem) = NULL;
		}

		HLIB_STD_MemFree(pstMmiMailList);
		pstMmiMailList = NULL;
	}

	if(pstMmmiText != NULL)
	{
		HLIB_STD_MemFree(pstMmmiText);
		pstMmmiText = NULL;
	}

	if(pusComponentLength != NULL)
	{
		HLIB_STD_MemFree(pusComponentLength);
		pusComponentLength = NULL;
	}

	if(ppszComponentsString != NULL)
	{
		HLIB_STD_MemFree(ppszComponentsString);
		ppszComponentsString = NULL;
	}

	return hError;
}

STATIC void xmgr_cas_iruc_menu_UpdateCheck(IrMenuContext_t *pstContext)
{
	if(pstContext->eActiveMenuGroup != eCasIrUCMenuGroup_LoaderStatus)
	{
		if(pstContext->eActiveMenuGroup == eCasIrUCMenuGroup_ServiceStatus && s_bInSoftcellServiceStatusSubItem == TRUE)
		{
			xmgr_cas_iruc_menu_UpdateDataForUCServiceStatusSubMenu(pstContext);
		}
		else if ((pstContext->eActiveMenuGroup == eCasIrUCMenuGroup_MailMessages)||(pstContext->eActiveMenuGroup == eCasIrUCMenuGroup_ClientStatus))
		{
			xmgr_cas_iruc_menu_UpdateData(pstContext);
		}
	}

}

#define	_________________________Client_status______________________________

STATIC HERROR 	xmgr_cas_iruc_menu_SetClientStatusParam(HUINT8 ucSlot, IR_ClientStatus_Item_t eItemId, HUINT8 *pszParam)
{
	SvcCas_CtrlParam_t					stIrCtrlParamForClient;
	IRUC_CLIENT_STATUS  					pClientStatus;
	IR_GetClientInfoOutParam_t				stClientInfo;
	HUINT8								szbuf[TEXT_SIZE256] = {0, };
	HERROR 								hErr = ERR_OK;

/* TODO: hjlee3
   우선 나중에 IR 부분 걷어 내자 */

	stIrCtrlParamForClient.ulControlType	= eCACTRL_IRUC_ClientStatus;
	stIrCtrlParamForClient.pvOut			= &stClientInfo;
	stClientInfo.ucScSlotId 				= ucSlot;
	stClientInfo.pstIrApiClientInfo 			= &pClientStatus;

	SVC_CAS_CtrlGet(eDxCAS_GROUPID_IR, eCACTRL_IRUC_ClientStatus, &stIrCtrlParamForClient);
	HxLOG_Info("address of pszParam: %x \n", pszParam);

	switch(eItemId)
	{
		case eCasIrClientStatus_CCAVersion:
			HxLOG_Info(" pClientStatus.ucVersion: %s \n", pClientStatus.ucVersion);
			MWC_UTIL_DvbStrcpy(pszParam, pClientStatus.ucVersion);
			break;
		case eCasIrClientStatus_ClientID:
			HxLOG_Info(" pClientStatus.ucClientID: %s \n", pClientStatus.ucClientID);
			MWC_UTIL_DvbStrcpy(pszParam, pClientStatus.ucClientID);
			break;

		case eCasIrClientStatus_SecureClient:
			HxLOG_Info(" pClientStatus.ucSerialNumber: %s \n", pClientStatus.ucSerialNumber);
			snprintf((char *)szbuf, TEXT_SIZE64, "%s, %04x, %s", pClientStatus.ucSerialNumber, pClientStatus.nCASystemID, pClientStatus.ucNationality);
			MWC_UTIL_DvbStrcpy(pszParam, szbuf);
			break;

		case eCasIrClientStatus_SecureCoreVersion:
			HxLOG_Info(" pClientStatus.ucSecureCoreVersion: %s \n", pClientStatus.ucSecureCoreVersion);
			MWC_UTIL_DvbStrcpy(pszParam, pClientStatus.ucSecureCoreVersion);
			break;

		case eCasIrClientStatus_SecureCore:
			HxLOG_Info(" pClientStatus.ucSecureCoreLoadStatus: %s \n", pClientStatus.ucSecureCoreLoadStatus);
			MWC_UTIL_DvbStrcpy(pszParam, pClientStatus.ucSecureCoreLoadStatus);
			break;

		case eCasIrClientStatus_DownloadStatus:
			HxLOG_Info(" pClientStatus.packagesDownloadProgressInfo: %s \n", pClientStatus.packagesDownloadProgressInfo);
			MWC_UTIL_DvbStrcpy(pszParam, pClientStatus.packagesDownloadProgressInfo);
			break;

		case eCasIrClientStatus_TMSData:
			HxLOG_Info(" pClientStatus.ucTmsData: %02X\n", pClientStatus.ucTmsData);
			MWC_UTIL_DvbStrcpy(pszParam, pClientStatus.ucTmsData);
			break;

		case eCasIrClientStatus_EMMCount:
			HxLOG_Info(" pClientStatus. nEmmCount: %d \n", pClientStatus.nEmmCount);
			snprintf((char *)szbuf, TEXT_SIZE64, "%d", pClientStatus.nEmmCount);
			MWC_UTIL_DvbStrcpy(pszParam, szbuf);
			break;

		case eCasIrClientStatus_ECMCount:
			HxLOG_Info(" pClientStatus. nEcmCount: %d \n", pClientStatus.nEcmCount);
			snprintf((char *)szbuf, TEXT_SIZE64, "%d", pClientStatus.nEcmCount);
			MWC_UTIL_DvbStrcpy(pszParam, szbuf);
			break;

		case eCasIrClientStatus_SoftCell:
			HxLOG_Info(" pClientStatus. softcell: %s \n", "DVB Client");
			snprintf((char *)szbuf, TEXT_SIZE256, "%s", "DVB Client");
			MWC_UTIL_DvbStrcpy(pszParam, szbuf);
			break;

		case eCasIrClientStatus_Version:
			HxLOG_Info(" pClientStatus. buildInfo: %s \n", pClientStatus.ucBuildInfo);
			snprintf((char *)szbuf, TEXT_SIZE256, "%s", pClientStatus.ucBuildInfo);
			MWC_UTIL_DvbStrcpy(pszParam, szbuf);
			break;

		case eCasIrClientStatus_Status:
			HxLOG_Info(" pClientStatus.ucSmartCardStatus: %s \n", pClientStatus.ucSmartCardStatus);
			MWC_UTIL_DvbStrcpy(pszParam, pClientStatus.ucSmartCardStatus);
			break;

		case eCasIrClientStatus_SmartCard:
			HxLOG_Info(" pClientStatus.ucSmartCardStatus: %s, %04x, %d.%d, %s, 0x%X\n", pClientStatus.ucScserialNumber, pClientStatus.nsmartcardType,pClientStatus.ucmajorVersion,pClientStatus.ucminorVersion,pClientStatus.ucSmartCardNationality,pClientStatus.nSmartCardCASystemID);
			snprintf((char *)szbuf, TEXT_SIZE256, "%s, %04x, %d.%d, %s, %04x", pClientStatus.ucScserialNumber, pClientStatus.nsmartcardType, pClientStatus.ucmajorVersion, pClientStatus.ucminorVersion, pClientStatus.ucSmartCardNationality, pClientStatus.nSmartCardCASystemID);
			MWC_UTIL_DvbStrcpy(pszParam, szbuf);
			break;

		case eCasIrClientStatus_ExtendInfo:
			HxLOG_Info(" pClientStatus.extendinfo: %d, %d, %04x, %04x \n", pClientStatus.ucbuild, pClientStatus.ucvariant,pClientStatus.npatchLevel,pClientStatus.nownerId);
			snprintf((char *)szbuf, TEXT_SIZE256, "%d, %d, %04x, %04x", pClientStatus.ucbuild, pClientStatus.ucvariant,pClientStatus.npatchLevel,pClientStatus.nownerId);
			MWC_UTIL_DvbStrcpy(pszParam, szbuf);
			break;

		case eCasIrClientStatus_ItemInfo_1:
		case eCasIrClientStatus_SecureChipInfo:
			HxLOG_Info("eCasIrClientStatus_ItemInfo_1\n");
			snprintf((char*)szbuf, TEXT_SIZE64, " ");
			MWC_UTIL_DvbStrncpy(pszParam, szbuf, TEXT_SIZE64);
			break;

		case eCasIrClientStatus_CSSN:
			HxLOG_Info(" pClientStatus.cssn: %02X%02X%02X%02X, %d, %d \n",
						pClientStatus.ucCSSN[0],
						pClientStatus.ucCSSN[1],
						pClientStatus.ucCSSN[2],
						pClientStatus.ucCSSN[3],
						pClientStatus.nLockID,pClientStatus.ucSecureType);
			snprintf((char *)szbuf, TEXT_SIZE256, "%02X%02X%02X%02X, %d, %d",
						pClientStatus.ucCSSN[0],
						pClientStatus.ucCSSN[1],
						pClientStatus.ucCSSN[2],
						pClientStatus.ucCSSN[3],
						pClientStatus.nLockID,pClientStatus.ucSecureType);
			MWC_UTIL_DvbStrcpy(pszParam, szbuf);
			break;

		default:
			HxLOG_Error("[xmgr_cas_iruc_menu_SetClientStatusParam] pSubItem ERROR\n");
			hErr = ERR_FAIL;

	}

	if(HxSTD_StrLen(pszParam)==0)
	{
		//빈 string으로 줄 수 맞춰서 올려주는게 web ap이 편하단다.
		snprintf((char*)szbuf, TEXT_SIZE64, " ");
		MWC_UTIL_DvbStrncpy(pszParam, szbuf, TEXT_SIZE64);
	}

	HxLOG_Print("[xmgr_cas_iruc_menu_SetClientStatusParam] pszParam: [%s]\n", pszParam);
	return hErr;
}

STATIC HERROR xmgr_cas_iruc_menu_MakeClientStatusMenuListData(SvcCas_MmiMenuList_t * pstMmiList)
{
	HUINT32				ulMenuItemLoop = 0;
	HERROR				hErr = ERR_FAIL;
	HUINT8				ucNumSc = NUM_SC_SLOT;
	HUINT8				ulScItemLoop = 0;
	HUINT32				ulMenuItemCount = 0;

	pstMmiList->ulNumOfItem = eCasIrClientStatus_Item_Max * ucNumSc;

	pstMmiList->ppszItem = (HUINT8 **)HLIB_STD_MemCalloc(sizeof(HUINT8*) * pstMmiList->ulNumOfItem);

	if(pstMmiList->ppszItem == NULL)
	{
		hErr = ERR_FAIL;
		HxLOG_Critical("\n\n");
		goto END_FUNC;
	}

	for(ulScItemLoop = 0; ulScItemLoop < ucNumSc; ulScItemLoop++)
	{
		for(ulMenuItemLoop = 0; ulMenuItemLoop < eCasIrClientStatus_Item_Max; ulMenuItemLoop++)
		{
			pstMmiList->ppszItem[ulMenuItemCount] = (HUINT8 *)HLIB_STD_MemCalloc(MGR_CAS_STR_LENGTH_LONG);

			if(pstMmiList->ppszItem[ulMenuItemCount] != NULL)
			{
				hErr = xmgr_cas_iruc_menu_SetClientStatusParam(ulScItemLoop, ulMenuItemLoop, pstMmiList->ppszItem[ulMenuItemCount]);
				if(hErr == ERR_FAIL)
				{
					HxLOG_Critical("\n\n");
				}

				HxLOG_Print("Item[%d], pstMmiList->ppszItem[ulMenuItemLoop]: [%s]\n", ulMenuItemLoop, pstMmiList->ppszItem[ulMenuItemCount]);
			}
			else
			{
				HxLOG_Error("pstMmiList->ppszItem[%d] == NULL !!\n", ulMenuItemLoop);
				hErr = ERR_FAIL;
				goto END_FUNC;
			}

			ulMenuItemCount++;

		}
	}

END_FUNC:
	return hErr;

}


#define	_________________________Product_status______________________________

#if 0
STATIC HERROR 	xmgr_cas_iruc_menu_SetProductStatusParam(HUINT8 ucSlot, IR_ProductStatus_Item_t eItemId, HUINT8 *pszParam)
{
	SvcCas_CtrlParam_t					stIrCtrlParamForProduct;
	IRUC_PRODUCT_STATUS   				pProductStatus;
	IR_GetProductInfoOutParam_t			stProductInfo;
	HUINT8								szbuf[TEXT_SIZE256] = {0, };
	HERROR 								hErr = ERR_OK;

	stIrCtrlParamForProduct.ulControlType		= eCACTRL_IRUC_ProductStatus;
	stIrCtrlParamForProduct.pvOut			= &stProductInfo;
	stProductInfo.ucScSlotId 					= ucSlot;
	stProductInfo.pstIrApiProductInfo 			= &pProductStatus;

	SVC_CAS_CtrlGet(eDxCAS_GROUPID_IR, eCACTRL_IRUC_ProductStatus, &stIrCtrlParamForProduct);
	HxLOG_Info("address of pszParam: %x \n", pszParam);

	switch(eItemId)
	{
		case eCasIrProductStatus_ProductCount:
			HxLOG_Info(" pProductStatus.id: %d \n", pProductStatus.id);
			MWC_UTIL_DvbStrcpy(pszParam, pProductStatus.id);
			break;

		case eCasIrProductStatus_SectorNumber:
			HxLOG_Info(" pProductStatus.ucSectorNumber: %d \n", pProductStatus.id);
			MWC_UTIL_DvbStrcpy(pszParam, pProductStatus.id);
			break;

		case eCasIrProductStatus_ProductId:
			HxLOG_Info(" pProductStatus.aucProductID: %d \n", pProductStatus.aucProductID);
			MWC_UTIL_DvbStrcpy(pszParam, pProductStatus.aucProductID);
			break;

		case eCasIrProductStatus_StartDate:
			HxLOG_Info(" pProductStatus.ulStartDate: %d \n", pProductStatus.ulStartDate);
			MWC_UTIL_DvbStrcpy(pszParam, pProductStatus.ulStartDate);
			break;

		case eCasIrProductStatus_Duration:
			HxLOG_Error(" pProductStatus.ulDuration: %d \n", pProductStatus.ulStartDate);
			MWC_UTIL_DvbStrcpy(pszParam, pProductStatus.ulStartDate);
			break;

		case eCasIrProductStatus_Entitled:
			HxLOG_Info(" pProductStatus.bEntitled: %d \n", pProductStatus.bEntitled);
			MWC_UTIL_DvbStrcpy(pszParam, pProductStatus.bEntitled);
			break;

		case eCasIrProductStatus_ProductType:
			HxLOG_Info(" pProductStatus.usProductType: %d \n", pProductStatus.usProductType);
			MWC_UTIL_DvbStrcpy(pszParam, pProductStatus.usProductType);
			break;

		default:
			HxLOG_Error("[xmgr_cas_iruc_menu_SetProductStatusParam] pSubItem ERROR\n");
			hErr = ERR_FAIL;

	}

	if(HxSTD_StrLen(pszParam)==0)
	{
		//빈 string으로 줄 수 맞춰서 올려주는게 web ap이 편하단다.
		snprintf((char*)szbuf, TEXT_SIZE64, " ");
		MWC_UTIL_DvbStrncpy(pszParam, szbuf, TEXT_SIZE64);
	}

	HxLOG_Print("[xmgr_cas_iruc_menu_SetProductStatusParam] pszParam: [%s]\n", pszParam);
	return hErr;
}
#endif

STATIC HERROR xmgr_cas_iruc_menu_MakeProductStatusMenuListData(SvcCas_MmiMenuList_t * pstMmiList)
{
	SvcCas_CtrlParam_t				stIrCtrlParamForProduct;
	IR_GetProductInfoOutParam_t 		stProductInfo;
	HERROR							hErr = ERR_OK;
	HxDATETIME_Date_t				stUcBaseDate = {2000, 1, 1};
	HINT32 							stUcBaseJulian = 0;

	// uc 이므로 우선 slot num 은 무시하고 하나로 처리.
	if (pstMmiList == NULL)
	{
		hErr = ERR_FAIL;
		HxLOG_Critical(" pstMmiList is NULL!!!!\n",__FUNCTION__,__LINE__);
		goto END_FUNC;
	}

	HxSTD_MemSet(&stProductInfo, 0, sizeof(IR_GetProductInfoOutParam_t));

	stIrCtrlParamForProduct.ulControlType		= eCACTRL_IRUC_ProductStatus;
	stIrCtrlParamForProduct.pvOut			= &stProductInfo;
	stProductInfo.ucScSlotId 					= 0; // uc 이므로일단 한개만 .. 처리..

	SVC_CAS_CtrlGet(eDxCAS_GROUPID_IR, eCACTRL_IRUC_ProductStatus, &stIrCtrlParamForProduct);

	if (stProductInfo.productCnt != 0 && stProductInfo.pstIrApiProductInfo != NULL)
	{
		int i = 0;
		IRUC_PRODUCT_STATUS		*ptrProductList = stProductInfo.pstIrApiProductInfo;

		pstMmiList->ppszItem = (HUINT8 **)HLIB_STD_MemCalloc(sizeof(HUINT8*) * stProductInfo.productCnt);
		pstMmiList->ulNumOfItem = stProductInfo.productCnt;

		HLIB_DATETIME_ConvertDateToJulian(&stUcBaseDate, &stUcBaseJulian);
		stUcBaseJulian--;

		for ( i = 0 ; i < stProductInfo.productCnt; i ++)
		{
			HxDATETIME_Date_t		stUcItemStartDate = {0,};
			pstMmiList->ppszItem[i] = (HUINT8 *)HLIB_STD_MemCalloc(MGR_CAS_STR_LENGTH_LONG);
			HLIB_DATETIME_ConvertJulianToDate((stUcBaseJulian + ptrProductList[i].ulStartDate), &stUcItemStartDate);

			HxSTD_snprintf(pstMmiList->ppszItem[i],  MGR_CAS_STR_LENGTH_LONG,  "%d %02X%02X %d/%02d/%02d %u %d %d",
							ptrProductList[i].ucSectorNumber,
							ptrProductList[i].aucProductID[0], ptrProductList[i].aucProductID[1],
							stUcItemStartDate.usYear, stUcItemStartDate.ucMonth, stUcItemStartDate.ucDay,
							ptrProductList[i].ulDuration, ptrProductList[i].bEntitled, ptrProductList[i].usProductType);
		}
		HLIB_MEM_Free(ptrProductList);
	}
	else
	{
		HxLOG_Debug(" product status info is 0 \n");

		pstMmiList->ulNumOfItem = 1;
		pstMmiList->ppszItem = (HUINT8 **)HLIB_STD_MemCalloc(sizeof(HUINT8*) * pstMmiList->ulNumOfItem);
		if(pstMmiList->ppszItem == NULL)
		{
			hErr = ERR_FAIL;
			HxLOG_Critical("\n\n");
			goto END_FUNC;
		}

		pstMmiList->ppszItem[0] = (HUINT8 *)HLIB_STD_MemCalloc(MGR_CAS_STR_LENGTH_LONG);
		snprintf((char *)pstMmiList->ppszItem[0], MGR_CAS_STR_LENGTH_LONG, "NULL");

		hErr = ERR_OK;
		goto END_FUNC;
	}

END_FUNC:
	return hErr;
}


#define	_________________________Loader_status______________________________

STATIC HERROR			xmgr_cas_iruc_menu_SetLoaderStatusParam (IR_LoaderStatus_Item_t eItemId, HUINT8 *pszParam)
{
	HUINT8					szbuf[TEXT_SIZE64] = {0, };
	HUINT8					ucTmp1, ucTmp2, ucTmp3;
//	HINT16					sTmp1, sTmp2;
	HUINT32 					ulSerialNo;
#if defined(CONFIG_MW_CAS_IRDETO_SECURE)
	HUINT32					cssnTmp;
#endif
	HERROR					hErr = ERR_OK;

	SvcCas_CtrlParam_t						stIrCtrlParam;
	SvcCas_CtrlParam_t						stIrCtrlParamForStatus;
	IR_LOADER_GetStatusOutParam_t	stGetStatusInfoParam;
	IR_LOADER_GetManuCodeOutParam_t		stGetManuCodeParam;
	IR_LOADER_GetHwCodeOutParam_t		stGetHwCodeParam;
	IR_LOADER_GetLoadSqeNumOutParam_t	stGetSeqNumParam;
	IR_LOADER_GetLoaderMajOutParam_t		stGetMajorParam;
	IR_LOADER_GetLoaderTypeOutParam_t		stGetTypeParam;
	IR_LOADER_GetLoaderMinorOutParam_t	stGetMinorParam;
	IR_LOADER_GetSerialNumOutParam_t		stGetSerialNumParam;

	HxLOG_Info("address of pszParam: %x \n", pszParam);

	switch(eItemId)
	{
		 //빈 string으로 줄 수 맞춰서 올려주는게 web ap이 편하단다.
		case eCasIrLoaderStatus_ItemInfo_1:
		case eCasIrLoaderStatus_ItemInfo_2:
			snprintf((char*)szbuf, TEXT_SIZE64, " ");
			MWC_UTIL_DvbStrncpy(pszParam, szbuf, TEXT_SIZE64);
			break;

		case eCasIrLoaderStatus_Device:
			stIrCtrlParam.ulControlType			= eCACTRL_IR_LOADER_GetManuCode;
			stIrCtrlParam.pvOut				= &stGetManuCodeParam;
			svc_cas_CtrlGet(eDxCAS_GROUPID_IR, eCACTRL_IR_LOADER_GetManuCode, &stIrCtrlParam);
			ucTmp1 = stGetManuCodeParam.usManCode;

			stIrCtrlParam.ulControlType			= eCACTRL_IR_LOADER_GetHWCode;
			stIrCtrlParam.pvOut				= &stGetHwCodeParam;
			svc_cas_CtrlGet(eDxCAS_GROUPID_IR, eCACTRL_IR_LOADER_GetHWCode, &stIrCtrlParam);
			ucTmp2 = stGetHwCodeParam.usHwCode;

			stIrCtrlParamForStatus.ulControlType		= eCACTRL_IR_LOADER_GetStatusInfo;
			stIrCtrlParamForStatus.pvOut			= &stGetStatusInfoParam;
			svc_cas_CtrlGet(eDxCAS_GROUPID_IR, eCACTRL_IR_LOADER_GetStatusInfo, &stIrCtrlParamForStatus);

			snprintf((char*)szbuf, TEXT_SIZE64, "%d, %d, %d", ucTmp1, ucTmp2, MACRO_GET_VALID_VALUE(stGetStatusInfoParam.stIRStatus.s16VariantValid, stGetStatusInfoParam.stIRStatus.u16Variant));
			MWC_UTIL_DvbStrncpy(pszParam, szbuf, TEXT_SIZE64);
			break;

		case eCasIrLoaderStatus_Key:
			stIrCtrlParamForStatus.ulControlType		= eCACTRL_IR_LOADER_GetStatusInfo;
			stIrCtrlParamForStatus.pvOut			= &stGetStatusInfoParam;
			svc_cas_CtrlGet(eDxCAS_GROUPID_IR, eCACTRL_IR_LOADER_GetStatusInfo, &stIrCtrlParamForStatus);

			snprintf((char*)szbuf, TEXT_SIZE64, "%d, %d, %d", MACRO_GET_VALID_VALUE(stGetStatusInfoParam.stIRStatus.s16SysIDValid, (HINT16)stGetStatusInfoParam.stIRStatus.u16SysID),
													MACRO_GET_VALID_VALUE(stGetStatusInfoParam.stIRStatus.s16KeyVerValid, stGetStatusInfoParam.stIRStatus.u16KeyVer),
													MACRO_GET_VALID_VALUE(stGetStatusInfoParam.stIRStatus.s16SigVerValid, stGetStatusInfoParam.stIRStatus.u16SigVer));
			MWC_UTIL_DvbStrncpy(pszParam, szbuf, TEXT_SIZE64);
			break;

#if defined(CONFIG_MW_CAS_IRDETO_SECURE)
		case eCasIrLoaderStatus_CSSN:
			stIrCtrlParam.ulControlType 		= eCACTRL_IR_LOADER_GetCssn;
			stIrCtrlParam.pvOut 			= &cssnTmp;
			svc_cas_CtrlGet(eDxCAS_GROUPID_IR, eCACTRL_IR_LOADER_GetCssn, &stIrCtrlParam);
			snprintf((char*)szbuf, TEXT_SIZE64, "%010d", cssnTmp);
			MWC_UTIL_DvbStrncpy(pszParam, szbuf, TEXT_SIZE64);
			break;
#endif

		case eCasIrLoaderStatus_L:
			stIrCtrlParam.ulControlType			= eCACTRL_IR_LOADER_GetLoadSeqNum;
			stIrCtrlParam.pvOut				= &stGetSeqNumParam;
			svc_cas_CtrlGet(eDxCAS_GROUPID_IR, eCACTRL_IR_LOADER_GetLoadSeqNum, &stIrCtrlParam);
			ucTmp1 = stGetSeqNumParam.usLoadSeqNum;

			snprintf((char*)szbuf, TEXT_SIZE64, "%d", ucTmp1);
			MWC_UTIL_DvbStrncpy(pszParam, szbuf, TEXT_SIZE64);
			break;
		case eCasIrLoaderStatus_FwVersion:
			snprintf((char*)szbuf, TEXT_SIZE64, "MVFSIR 1.02.00");
			MWC_UTIL_DvbStrncpy(pszParam, szbuf, TEXT_SIZE64);
			break;

		case eCasIrLoaderStatus_LoaderVersion:
			stIrCtrlParam.ulControlType			= eCACTRL_IR_LOADER_GetLoaderMajor;
			stIrCtrlParam.pvOut				= &stGetMajorParam;
			svc_cas_CtrlGet(eDxCAS_GROUPID_IR, eCACTRL_IR_LOADER_GetLoaderMajor, &stIrCtrlParam);
			ucTmp1 = stGetMajorParam.ucLoaderMajor;

			stIrCtrlParam.ulControlType			= eCACTRL_IR_LOADER_GetLoaderMinor;
			stIrCtrlParam.pvOut				= &stGetMinorParam;
			svc_cas_CtrlGet(eDxCAS_GROUPID_IR, eCACTRL_IR_LOADER_GetLoaderMinor, &stIrCtrlParam);
			ucTmp2 = stGetMinorParam.ucLoaderMinor;

			stIrCtrlParam.ulControlType			= eCACTRL_IR_LOADER_GetLoaderType;
			stIrCtrlParam.pvOut				= &stGetTypeParam;
			svc_cas_CtrlGet(eDxCAS_GROUPID_IR, eCACTRL_IR_LOADER_GetLoaderType, &stIrCtrlParam);
			ucTmp3 = stGetTypeParam.ucLoaderType;

			snprintf((char*)szbuf, TEXT_SIZE64, "%d.%d.%d", ucTmp1, ucTmp2, ucTmp3);
			MWC_UTIL_DvbStrncpy(pszParam, szbuf, TEXT_SIZE64);
			break;

		case eCasIrLoaderStatus_SerialNo:
			stIrCtrlParam.ulControlType			= eCACTRL_IR_LOADER_GetSerialNum;
			stIrCtrlParam.pvOut				= &stGetSerialNumParam;
			svc_cas_CtrlGet(eDxCAS_GROUPID_IR, eCACTRL_IR_LOADER_GetSerialNum, &stIrCtrlParam);
			ulSerialNo = stGetSerialNumParam.unSerialNum;

			snprintf((char*)szbuf, TEXT_SIZE64, "%u", ulSerialNo);
			MWC_UTIL_DvbStrncpy(pszParam, szbuf, TEXT_SIZE64);
			break;

		case eCasIrLoaderStatus_SpecificInfo:
			snprintf((char*)szbuf, TEXT_SIZE64, "Specific Info");
			MWC_UTIL_DvbStrncpy(pszParam, szbuf, TEXT_SIZE64);
			break;

		default:
			HxLOG_Error("pSubItem ERROR\n");
			hErr = ERR_FAIL;
	}



	HxLOG_Print("[xmgr_cas_iruc_menu_SetLoaderStatusParam] pszParam: [%s]\n", pszParam);

	return hErr;
}


STATIC HERROR xmgr_cas_iruc_menu_MakeLoaderStatusMenuListData(SvcCas_MmiMenuList_t * pstMmiList)
{
	HUINT32				ulMenuItemLoop = 0;
	HERROR				hErr = ERR_FAIL;


	pstMmiList->ulNumOfItem = eCasIrLoaderStatus_Item_Max;

	pstMmiList->ppszItem = (HUINT8 **)HLIB_STD_MemCalloc(sizeof(HUINT8*) * pstMmiList->ulNumOfItem);

	if(pstMmiList->ppszItem == NULL)
	{
		hErr = ERR_FAIL;
		HxLOG_Critical("\n\n");
		goto END_FUNC;
	}

	for(ulMenuItemLoop = 0; ulMenuItemLoop < pstMmiList->ulNumOfItem; ulMenuItemLoop++)
	{
		pstMmiList->ppszItem[ulMenuItemLoop] = (HUINT8 *)HLIB_STD_MemCalloc(MGR_CAS_STR_LENGTH_LONG);

		if(pstMmiList->ppszItem[ulMenuItemLoop] != NULL)
		{
			hErr = xmgr_cas_iruc_menu_SetLoaderStatusParam(ulMenuItemLoop, pstMmiList->ppszItem[ulMenuItemLoop]);
			if(hErr == ERR_FAIL)
			{
				HxLOG_Critical("\n\n");
			}

			HxLOG_Print("[%s] [%d] Item[%d], pstMmiList->ppszItem[ulMenuItemLoop]: [%s]\n", __FUNCTION__, __LINE__, ulMenuItemLoop, pstMmiList->ppszItem[ulMenuItemLoop]);
		}
		else
		{
			HxLOG_Error("pstMmiList->ppszItem[%d] == NULL !!\n", ulMenuItemLoop);
			hErr = ERR_FAIL;
			goto END_FUNC;
		}

	}

END_FUNC:
	return hErr;


}


#define	_________________________Mail_messages______________________________

STATIC HERROR			xmgr_cas_iruc_menu_SetMailMessageParam (HUINT8 ucIdx, HUINT8 ucMailIndex, OxMgrCasUiMail_t *pMailItem)
{
	HERROR				hErr = ERR_FAIL;
	MAILBOX_TABLE 		MailInfo;
	SvcCas_CtrlParam_t		stIrCtrlParam;
	IR_GetMailBoxOutParam_t	stMailBoxParam;
	IR_GetMailStateOutParam_t	stMailStateParam;

	HxLOG_Info("ucIdx: %d, ucMailIndex: %d \n", ucIdx, ucMailIndex);

	stIrCtrlParam.ulControlType = eCACTRL_IR_GetMailBox;
	stIrCtrlParam.pvOut 	= &stMailBoxParam;
	stMailBoxParam.ucMailIndex = ucMailIndex;
	stMailBoxParam.pstMailBox	= &MailInfo;
	hErr = SVC_CAS_CtrlGet(eDxCAS_GROUPID_IR, eCACTRL_IR_GetMailBox, &stIrCtrlParam);

	if(hErr == ERR_FAIL)
	{
		goto END_FUNC;
	}

	if(TRUE != MailInfo.bAttributeMsg)
	{
		if(MailInfo.msg_length >= MAX_NORMAL_TEXT_LENGTH)
		{
			MailInfo.msg_length = MAX_NORMAL_TEXT_LENGTH-1;
		}
	}
	else
	{
		if(MailInfo.msg_length >= MAX_ATTRIBUTED_TEXT_LENGTH)
		{
			MailInfo.msg_length = MAX_ATTRIBUTED_TEXT_LENGTH-1;
		}
	}
	pMailItem->pszText = (HUINT8 *)HLIB_STD_MemCalloc(sizeof(HUINT8) * (MailInfo.msg_length+1));
	MWC_UTIL_DvbStrncpy(pMailItem->pszText, MailInfo.text, MailInfo.msg_length);
	pMailItem->ulTextLength = MailInfo.msg_length;
//	pMailItem->pszText[pMailItem->ulTextLength-1] = '\0';

#if 0

	/** < Attribute */
	HUINT32 				ulBgColor;
	HUINT32 				ulFgColor;
	HUINT32 				ulProgress;

	/** < String */
	HCHAR					szTitle[MGR_CAS_STR_LENGTH_LONG];		/** < 타이틀 영역에 표시될 스트링 */
	HCHAR					szSubTitle[MGR_CAS_STR_LENGTH_LONG]; 	/** < 서브타이틀 영역에 표시될 스트링 */
	HCHAR					szBottom[MGR_CAS_STR_LENGTH_LONG];		/** < 하단 영역에 표시된 스트링 */
#endif

	HxSTD_memset(&(pMailItem->stHeader), 0x00, sizeof(OxMgrCasUiMailHeader_t));

	HxLOG_Info("MailInfo.bAttributeMsg: %d \n", MailInfo.bAttributeMsg);
	pMailItem->stHeader.bIsAttributed = MailInfo.bAttributeMsg;

	pMailItem->stHeader.nMailIdx = ucIdx;
	snprintf(pMailItem->stHeader.strReceivedDate, MGR_CAS_MAIL_RECEIVED_DATE_LENGTH, "[\"%02d/%02d\"]", MailInfo.add_date.ucDay, MailInfo.add_date.ucMonth);
	snprintf(pMailItem->stHeader.strReceivedTime, MGR_CAS_MAIL_RECEIVED_TIME_LENGTH, "[\"%02d:%02d\"]", MailInfo.add_time.ucHour, MailInfo.add_time.ucMinute);
	HxLOG_Info("MailInfo.add_date.ucDay:%02d, MailInfo.add_date.ucMonth:%02d, pMailItem->stHeader.strReceivedDate: %s \n", MailInfo.add_date.ucDay, MailInfo.add_date.ucMonth, pMailItem->stHeader.strReceivedDate);
	HxLOG_Info("MailInfo.add_time.ucHour:%02d, MailInfo.add_time.ucMinute:%02d, pMailItem->stHeader.strReceivedTime: %s \n", MailInfo.add_time.ucHour, MailInfo.add_time.ucMinute, pMailItem->stHeader.strReceivedTime);
	pMailItem->stHeader.nMailType	= MailInfo.Atrb_Display.ucMsg_Type;
	pMailItem->stHeader.ulMailIconType = MailInfo.ucType;
	pMailItem->stHeader.bUseIcon = TRUE;

	HxLOG_Info("MailInfo.Atrb_Display.bFlashing: %d \n", MailInfo.Atrb_Display.bFlashing);
	if(MailInfo.Atrb_Display.bFlashing)
	{
		pMailItem->stHeader.ulAttr |= eMgrCasUiAttr_Flashing;
	}

	HxLOG_Info("MailInfo.Atrb_Display.bNormal: %d \n", MailInfo.Atrb_Display.bNormal);
	if(MailInfo.Atrb_Display.bNormal)
	{
		pMailItem->stHeader.ulAttr |= eMgrCasUiAttr_Title;
		// 모든 String 관리는 Web에서 처리하는 것으로 약속됨에 따라 AP_CasResStr_GetStrRsc() 함수를 더이상 사용하지 말아야 함.
		// 따라서 LOC ID를 Web으로 전송 후 Web에서 그에 맞는 string을 찾아 출력하도록 함.
		HxSTD_snprintf(pMailItem->stHeader.szTitle, MGR_CAS_STR_LENGTH_LONG-1, "[{\"id\":\"LOC_MAIL_MESSAGES_ID\"}]");
		pMailItem->stHeader.szTitle [MGR_CAS_STR_LENGTH_LONG-1] = '\0';

	}
	else
	{
		pMailItem->stHeader.ulAttr |= eMgrCasUiAttr_SetPosition;
		pMailItem->stHeader.ulAttr |= eMgrCasUiAttr_Multiline;
#if !defined(USE_WEB_UI_COORDINATE)
		pMailItem->stHeader.ulHeight = (CONFIG_MWC_OSD_VERTICAL*MailInfo.Atrb_Display.ucCoverage) / 100;
		pMailItem->stHeader.ulWidth = (CONFIG_MWC_OSD_HORIZONTAL* MailInfo.Atrb_Display.ucCoverage) / 100;
		pMailItem->stHeader.ulPosX = (CONFIG_MWC_OSD_HORIZONTAL - pMailItem->stHeader.ulWidth) / 2;
		pMailItem->stHeader.ulPosY = (CONFIG_MWC_OSD_VERTICAL - pMailItem->stHeader.ulHeight) / 2;
#else
		pMailItem->stHeader.ulCoverage = MailInfo.Atrb_Display.ucCoverage;
#endif

	}

	pMailItem->stHeader.ulTimeoutSecond = MailInfo.Atrb_Display.unDuration;


	stIrCtrlParam.ulControlType = eCACTRL_IR_GetMailReadState;
	stIrCtrlParam.pvOut 	= &stMailStateParam;
	stMailStateParam.ucMailIndex = ucMailIndex;
	SVC_CAS_CtrlGet(eDxCAS_GROUPID_IR, eCACTRL_IR_GetMailReadState, &stIrCtrlParam);
	HxLOG_Info("pMailItem->stHeader.bRead: %d \n", pMailItem->stHeader.bRead);
	pMailItem->stHeader.bRead = stMailStateParam.bReadMail;

END_FUNC:

	return hErr;
}


STATIC HERROR xmgr_cas_iruc_menu_MakeMailListData(CAS_MmiMailList_t * pstMmiMailList)
{
	HUINT32				ulValidMailIndexLoop = 0;
	HERROR				hErr = ERR_FAIL;
	MAILBOX_TABLE 		MailInfo;
	HINT32 					i;
	HUINT8 					ucCnt;
	HUINT8 					aucVaildMailIdxList[IR_CA_MAILLIST_INDEX_NUM] = {0, };
	SvcCas_CtrlParam_t		stIrCtrlParam;
	IR_GetMailListOutParam_t	stGetMailListParam;
	IR_GetMailBoxOutParam_t	stMailBoxParam;
	IR_GetCheckMailOutParam_t	stCheckMailParam;

#if defined(CONFIG_DEBUG)
	HxLOG_Info("========================= mail list ===========\n");
	CAS_IR_MSG_DRV_DbgMailBox();
	HxLOG_Info("==========================================\n");
#endif

	pstMmiMailList->ucNumOfItem = 0;

	stIrCtrlParam.ulControlType = eCACTRL_IR_GetMailList;
	stIrCtrlParam.pvOut 	= &stGetMailListParam;
	stGetMailListParam.paucMailIndexList = s_aucMailIdxList;
	hErr = SVC_CAS_CtrlGet(eDxCAS_GROUPID_IR, eCACTRL_IR_GetMailList, &stIrCtrlParam);
	ucCnt = stGetMailListParam.ucNumberOfSavedMail;

	for (i=0 ; i< ucCnt ; i++)
	{
		stIrCtrlParam.ulControlType = eCACTRL_IR_GetMailBox;
		stIrCtrlParam.pvOut 	= &stMailBoxParam;
		stMailBoxParam.ucMailIndex = s_aucMailIdxList[i];
		stMailBoxParam.pstMailBox	= &MailInfo;
		SVC_CAS_CtrlGet(eDxCAS_GROUPID_IR, eCACTRL_IR_GetMailBox, &stIrCtrlParam);

		stIrCtrlParam.ulControlType = eCACTRL_IR_CheckValidOfMail;
		stIrCtrlParam.pvOut		= &stCheckMailParam;
		stCheckMailParam.ucMailIndex = s_aucMailIdxList[i];
		SVC_CAS_CtrlGet(eDxCAS_GROUPID_IR, eCACTRL_IR_CheckValidOfMail, &stIrCtrlParam);

		HxLOG_Info("stCheckMailParam.ucMailIndex: %d, valid: %d \n", stCheckMailParam.ucMailIndex, stCheckMailParam.bCheckMail);
		if (stCheckMailParam.bCheckMail == TRUE)
		{
			aucVaildMailIdxList[pstMmiMailList->ucNumOfItem] = s_aucMailIdxList[i];
			pstMmiMailList->ucNumOfItem++;
		}
	}

	if(pstMmiMailList->ucNumOfItem == 0)
	{
		hErr = ERR_OK;
		HxLOG_Debug("pstMmiMailList->ucNumOfItem: %d \n\n", pstMmiMailList->ucNumOfItem);
		goto END_FUNC;
	}

	pstMmiMailList->ppMailItem = (OxMgrCasUiMail_t **)HLIB_STD_MemCalloc(sizeof(OxMgrCasUiMail_t*) * (pstMmiMailList->ucNumOfItem));

	if(pstMmiMailList->ppMailItem == NULL)
	{
		hErr = ERR_FAIL;
		HxLOG_Critical("\n\n");
		goto END_FUNC;
	}

	for(ulValidMailIndexLoop = 0; ulValidMailIndexLoop < pstMmiMailList->ucNumOfItem; ulValidMailIndexLoop++)
	{
		pstMmiMailList->ppMailItem[ulValidMailIndexLoop] = (OxMgrCasUiMail_t *)HLIB_STD_MemCalloc(sizeof(OxMgrCasUiMail_t));

		if(pstMmiMailList->ppMailItem[ulValidMailIndexLoop] != NULL)
		{
			//pstMmiMailList->ppMailItem[ulValidMailIndexLoop] = (OxMgrCasUiMail_t *)HLIB_STD_MemCalloc(sizeof(OxMgrCasUiMail_t));
			hErr = xmgr_cas_iruc_menu_SetMailMessageParam(ulValidMailIndexLoop, aucVaildMailIdxList[ulValidMailIndexLoop], pstMmiMailList->ppMailItem[ulValidMailIndexLoop]);
			if(hErr == ERR_FAIL)
			{
				HxLOG_Critical("\n\n");
				break;
			}

		}
		else
		{
			HxLOG_Error("pstMmiMailList->ppMailItem[ulValidMailIndexLoop][%d] == NULL !!\n", ulValidMailIndexLoop);
			hErr = ERR_FAIL;
			goto END_FUNC;
		}
	}


END_FUNC:
	return hErr;


}

STATIC HERROR xmgr_cas_iruc_menu_MsgInputNotifyForMailMessages(IrMenuContext_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32							ulSlotNumber = 0;
	DxCAS_MmiSessionType_e				eSessionType = eDxCAS_SESSIONTYPE_NONE;
	OxMgrCasUiUserInputNotify_t		*pstUserInputNotify = NULL;
	Handle_t						hSession;
	HUINT32 					ulUiHandle;


	ulSlotNumber = (HUINT32)p1;
	eSessionType = (DxCAS_MmiSessionType_e)p2;
	pstUserInputNotify = (OxMgrCasUiUserInputNotify_t *)p3;

	if (eSessionType != eDxCAS_SESSIONTYPE_MENU)
	{
		return ERR_FAIL;
	}

	hSession = pstUserInputNotify->hSession;
	ulUiHandle = pstUserInputNotify->ulUiHandle;
	HxLOG_Debug("hSession: 0x%x, uiHandle: 0x%x \n", hSession, ulUiHandle);

	HxLOG_Info("InputType[%d]\n", pstUserInputNotify->eInputType);

	switch(pstUserInputNotify->eInputType)
	{
		case eMgrCasInputType_Button:
			HxLOG_Info("eMgrCasInputType_Button\n");
			// nothing to do
			break;

		case eMgrCasInputType_Number:
			HxLOG_Info("eMgrCasInputType_Number, ulNumber(%d), pszNumber(%s), Length(%d)\n", pstUserInputNotify->utData.stNumber.ulNumber, pstUserInputNotify->utData.stNumber.pszNumber, pstUserInputNotify->utData.stNumber.ulNumberLength);
			//pstUserInputNotify->utData.stNumber.pszNumber 에 해당하는 mail 을 읽음 표시.
			//pstUserInputNotify->utData.stNumber.ulNumber 는 web ap에서 무조건 0으로 내려주게 coding 되어.... 도대체 왜 ???
			{
				SvcCas_CtrlParam_t		stIrCtrlParam;
				IR_GetCheckMailOutParam_t	stCheckMailParam;
				HUINT8					mailIdx;
				HUINT8					mailIdxTemp;
				SvcCas_CtrlParam_t			stInParam;
				IR_SetMailStateInParam_t		stReadMail;

				mailIdxTemp = atoi(pstUserInputNotify->utData.stNumber.pszNumber);
				mailIdx = s_aucMailIdxList[mailIdxTemp];

				HxLOG_Info("mailIdx: %d\n", mailIdx);

				stIrCtrlParam.ulControlType = eCACTRL_IR_CheckValidOfMail;
				stIrCtrlParam.pvOut 	= &stCheckMailParam;
				stCheckMailParam.ucMailIndex = mailIdx;
				SVC_CAS_CtrlGet(eDxCAS_GROUPID_IR, eCACTRL_IR_CheckValidOfMail, &stIrCtrlParam);
				HxLOG_Info("stCheckMailParam.bCheckMail: %d \n", stCheckMailParam.bCheckMail);
				if (stCheckMailParam.bCheckMail == TRUE)
				{
					stInParam.ulControlType		= eCACTRL_IR_SetMailReadState;
					stInParam.pvIn				= &stReadMail;
					stReadMail.ucMailIndex = mailIdx;
					stReadMail.bReadMail = TRUE;

					SVC_CAS_CtrlSet(eDxCAS_GROUPID_IR, eCACTRL_IR_SetMailReadState, &stInParam);

				}

			}

			xmgr_cas_iruc_menu_UpdateData(pstContext);
			break;

		case eMgrCasInputType_String:
			HxLOG_Info("eMgrCasInputType_String\n");
			// nothing to do
			break;

		case eMgrCasInputType_Selection:
			HxLOG_Info("eMgrCasInputType_Selection, pstUserInputNotify->utData.stSelection.ulItemIdx: %d\n", pstUserInputNotify->utData.stSelection.ulItemIdx);
			// pstUserInputNotify->utData.stSelection.ulItemIdx에 해당하는 mail을 지운다.

			{
				SvcCas_CtrlParam_t		stIrCtrlParam;
				IR_GetCheckMailOutParam_t	stCheckMailParam;
				HUINT8					mailIdx;
				SvcCas_CtrlParam_t			stInParam;
				IR_DeleteMailInParam_t		stDeleteMail;


				mailIdx = s_aucMailIdxList[pstUserInputNotify->utData.stSelection.ulItemIdx];

				HxLOG_Info("mailIdx: %d\n", mailIdx);

				stIrCtrlParam.ulControlType = eCACTRL_IR_CheckValidOfMail;
				stIrCtrlParam.pvOut 	= &stCheckMailParam;
				stCheckMailParam.ucMailIndex = mailIdx;
				SVC_CAS_CtrlGet(eDxCAS_GROUPID_IR, eCACTRL_IR_CheckValidOfMail, &stIrCtrlParam);
				HxLOG_Info("stCheckMailParam.bCheckMail: %d \n", stCheckMailParam.bCheckMail);
				if (stCheckMailParam.bCheckMail == TRUE)
				{
					stInParam.ulControlType		= eCACTRL_IR_DeleteMail;
					stInParam.pvIn				= &stDeleteMail;
					stDeleteMail.ucMailIndex = mailIdx;

					SVC_CAS_CtrlCall(eDxCAS_GROUPID_IR, eCACTRL_IR_DeleteMail, &stInParam);


				}

			}


			xmgr_cas_iruc_menu_UpdateData(pstContext);
			break;

		case eMgrCasInputType_Etc:
			HxLOG_Info("eMgrCasInputType_Etc\n");
			// nothing to do
			break;

		default:
			break;
	}


	return ERR_OK;
}


#define	_________________________Softcell_service_status______________________________
STATIC HERROR xmgr_cas_iruc_menu_UpdateUCServiceStatusInfomation(void)
{
	HERROR				hErr;
	HUINT8				uServiceCount = 0;

	HxLOG_Info("\n");

	if (s_pstSvcStatusInfo)
	{
		CA_MEM_Free(s_pstSvcStatusInfo);
		s_pstSvcStatusInfo = NULL;
	}

	hErr = xsvc_cas_IrUcSvcStatusGetNumberOfServices(&s_pstSvcStatusInfo, &uServiceCount);
	HxLOG_Info("uServiceCount: (%d)\n", uServiceCount);
	if( hErr == HIR_OK )
	{
		s_CASIR_SSS_Item_Count = uServiceCount;
	}
	else
	{
		s_CASIR_SSS_Item_Count = 0;
	}
	HxLOG_Info("s_CASIR_SSS_Item_Count = %d\n", s_CASIR_SSS_Item_Count);



	return ERR_OK;


}

STATIC HERROR			xmgr_cas_iruc_menu_SetUCServiceStatusParam (HUINT32 itemIdx, HUINT8 *pszParam)
{
	HERROR					hErr = ERR_OK;
	HUINT32					ucTunerId;

	HxLOG_Info("address of pszParam: %x \n", pszParam);

	/* Handle */
	// s_pstSvcStatusInfo[eItemId].usSCellSvcHandle

	/* Name */
	// eItemId
	// SERVICE_DVB_DESCRAMBLE
	// SERVICE_DVB_EMM
	// SERVICE_PVR_RECORD
	// SERVICE_PVR_PLAYBACK
	// SERVICE_PVR_DRM
	// SERVICE_PVR_TSR_PLAYBACK
	// SERVICE_PVR_TSR_RECORD

	/* Source TS */
	SVC_PIPE_GetResourceId(SVC_PIPE_GetActionId(s_pstSvcStatusInfo[itemIdx].hAction), eRxRSCTYPE_TUNER_GROUP, (HUINT32 *)&ucTunerId);

	/* Instance */
	//s_pstSvcStatusInfo[eItemId].ucInstanceCount

	/* Description */  // 이것은 name과 동일.
	HxLOG_Info("s_pstSvcStatusInfo[eItemId].usSCellSvcHandle: 0x%03X, eItemId: %d, ucTunerId: %d, s_pstSvcStatusInfo[eItemId].ucInstanceCount: %d\n", s_pstSvcStatusInfo[itemIdx].usSCellSvcHandle, s_pstSvcStatusInfo[itemIdx], ucTunerId, s_pstSvcStatusInfo[itemIdx].ucInstanceCount);
	snprintf((char *)pszParam, MGR_CAS_STR_LENGTH_LONG, "0x%03X:%d:%d:%d", s_pstSvcStatusInfo[itemIdx].usSCellSvcHandle, s_pstSvcStatusInfo[itemIdx].eSvcType, ucTunerId, s_pstSvcStatusInfo[itemIdx].ucInstanceCount);


	HxLOG_Print("pszParam: [%s]\n", pszParam);

	return hErr;
}


STATIC HERROR xmgr_cas_iruc_menu_MakeUCServiceStatusMenuListData(SvcCas_MmiMenuList_t * pstMmiList)
{
	HUINT32				ulMenuItemLoop = 0;
	HERROR				hErr = ERR_FAIL;
	HUINT32				ulStrHash = 0;
	HUINT8				*pStr = NULL;
	HUINT32				ulStrLength = 0;


	xmgr_cas_iruc_menu_UpdateUCServiceStatusInfomation();

	pstMmiList->ulNumOfItem = s_CASIR_SSS_Item_Count;

	pStr = HLIB_STD_MemCalloc(sizeof(HUINT8) * 1024);
	if(pStr == NULL)
	{
		hErr = ERR_FAIL;
		HxLOG_Critical("\n\n");
		goto END_FUNC;
	}

	if(pstMmiList->ulNumOfItem == 0)
	{
		pstMmiList->ulNumOfItem = 1;

		pstMmiList->ppszItem = (HUINT8 **)HLIB_STD_MemCalloc(sizeof(HUINT8*) * pstMmiList->ulNumOfItem);

		if(pstMmiList->ppszItem == NULL)
		{
			hErr = ERR_FAIL;
			HxLOG_Critical("\n\n");
			goto END_FUNC;
		}

		// 빈칸으로 보낸다.
		pstMmiList->ppszItem[0] = (HUINT8 *)HLIB_STD_MemCalloc(MGR_CAS_STR_LENGTH_LONG);
		snprintf((char *)pstMmiList->ppszItem[0], MGR_CAS_STR_LENGTH_LONG, "NULL");
		MWC_UTIL_DvbStrcat(pStr, pstMmiList->ppszItem[0]);

		hErr = ERR_OK;
		goto END_FUNC;
	}

	pstMmiList->ppszItem = (HUINT8 **)HLIB_STD_MemCalloc(sizeof(HUINT8*) * pstMmiList->ulNumOfItem);

	if(pstMmiList->ppszItem == NULL)
	{
		hErr = ERR_FAIL;
		HxLOG_Critical("\n\n");
		goto END_FUNC;
	}

	HxLOG_Info("pstMmiList->ulNumOfItem: %d \n", pstMmiList->ulNumOfItem);


	for(ulMenuItemLoop = 0; ulMenuItemLoop < pstMmiList->ulNumOfItem; ulMenuItemLoop++)
	{
		pstMmiList->ppszItem[ulMenuItemLoop] = (HUINT8 *)HLIB_STD_MemCalloc(MGR_CAS_STR_LENGTH_LONG);

		if(pstMmiList->ppszItem[ulMenuItemLoop] != NULL)
		{
			HxLOG_Info("ulMenuItemLoop: %d, s_pstSvcStatusInfo[ulMenuItemLoop].eSvcType: %d \n", ulMenuItemLoop, s_pstSvcStatusInfo[ulMenuItemLoop].eSvcType);
			hErr = xmgr_cas_iruc_menu_SetUCServiceStatusParam(ulMenuItemLoop, pstMmiList->ppszItem[ulMenuItemLoop]);
			if(hErr == ERR_FAIL)
			{
				HxLOG_Critical("\n\n");
			}
			else
			{
				ulStrLength += SvcCas_UtilStrLen(pstMmiList->ppszItem[ulMenuItemLoop]);
				if(ulStrLength < 1024)
				{
					MWC_UTIL_DvbStrcat(pStr, pstMmiList->ppszItem[ulMenuItemLoop]);
				}
			}

			HxLOG_Print("[%s] [%d] Item[%d], pstMmiList->ppszItem[ulMenuItemLoop]: [%s]\n", __FUNCTION__, __LINE__, ulMenuItemLoop, pstMmiList->ppszItem[ulMenuItemLoop]);
		}
		else
		{
			HxLOG_Error("pstMmiList->ppszItem[%d] == NULL !!\n", ulMenuItemLoop);
			hErr = ERR_FAIL;
			goto END_FUNC;
		}

	}





END_FUNC:


	if(pStr != NULL)
	{
		ulStrHash = HLIB_HASH_StrHash(pStr);
		HLIB_STD_MemFree(pStr);
		pStr = NULL;
	}

	if(s_ulStrHash == ulStrHash)
	{
		HxLOG_Info("The same data. do not send. \n");
		return ERR_FAIL;
	}
	else
	{
		s_ulStrHash = ulStrHash;
	}


	return hErr;


}

STATIC HERROR xmgr_cas_iruc_menu_MsgInputNotifyForUCServiceStatus(IrMenuContext_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32							ulSlotNumber = 0;
	DxCAS_MmiSessionType_e				eSessionType = eDxCAS_SESSIONTYPE_NONE;
	OxMgrCasUiUserInputNotify_t		*pstUserInputNotify = NULL;
	Handle_t 						hSession;
	HUINT32						ulUiHandle;


	ulSlotNumber = (HUINT32)p1;
	eSessionType = (DxCAS_MmiSessionType_e)p2;
	pstUserInputNotify = (OxMgrCasUiUserInputNotify_t *)p3;

	if (eSessionType != eDxCAS_SESSIONTYPE_MENU)
	{
		return ERR_FAIL;
	}

	hSession = pstUserInputNotify->hSession;
	ulUiHandle = pstUserInputNotify->ulUiHandle;

	HxLOG_Debug("hSession: 0x%x, uiHandle: 0x%x \n", hSession, ulUiHandle);


	HxLOG_Info("InputType[%d]\n", pstUserInputNotify->eInputType);

	switch(pstUserInputNotify->eInputType)
	{
		case eMgrCasInputType_Button:
			HxLOG_Info("eMgrCasInputType_Button\n");
			// nothing to do
			break;

		case eMgrCasInputType_Number:
			HxLOG_Info("eMgrCasInputType_Number\n");
			// nothing to do
			break;

		case eMgrCasInputType_String:
			HxLOG_Info("eMgrCasInputType_String\n");
			// nothing to do
			break;

		case eMgrCasInputType_Selection:
			HxLOG_Info("eMgrCasInputType_Selection\n");

			if(s_bInSoftcellServiceStatusSubItem == FALSE)
			{
				s_bInSoftcellServiceStatusSubItem = TRUE;

				if(pstUserInputNotify->utData.stSelection.ulItemIdx >= s_CASIR_SSS_Item_Count)
				{
					HxLOG_Info("s_CASIR_SSS_Item_Count:  %d\n", s_CASIR_SSS_Item_Count);
					return ERR_FAIL;
				}
				s_CASIR_SSS_SubItem_Index = pstUserInputNotify->utData.stSelection.ulItemIdx;

				xsvc_cas_IrUcStopMonitoring();
				/* hjlee3 선택 된 menu의 data를 update 하는 것으로 보인다. */
				xmgr_cas_iruc_menu_UpdateDataForUCServiceStatusSubMenu(pstContext);
			}

			break;

		case eMgrCasInputType_Etc:
			switch(pstUserInputNotify->utData.stEtc.eMgrCasEtcType)
			{
				case eMgrCasEtc_Previous:
					HxLOG_Info("eMgrCasEtc_Previous\n");

					xsvc_cas_IrUcStopMonitoring();

					if(s_bInSoftcellServiceStatusSubItem == FALSE)
					{
						HxLOG_Info("s_bInSoftcellServiceStatusSubItem == FALSE\n");
						//BUS_MGR_Destroy(NULL);
						 // TODO: shko data 정리하고 종료?
						xmgr_cas_iruc_menu_PostMessageSessionEventToOM(pstContext, eSessionType, ulSlotNumber, eMEVT_CASUI_REQ_CLOSE_SESSION);
					}
					else
					{
						s_bInSoftcellServiceStatusSubItem = FALSE;

						HxLOG_Info("s_bInSoftcellServiceStatusSubItem == TRUE\n");

						s_CASIR_SSS_SubItem_Index = 0;

						xmgr_cas_iruc_menu_UpdateData(pstContext);
					}
					break;

				case eMgrCasEtc_Exit:
					HxLOG_Info("eMgrCasEtc_Exit\n");

					//BUS_MGR_Destroy(NULL);
					 // TODO: shko data 정리하고 종료?
					xmgr_cas_iruc_menu_PostMessageSessionEventToOM(pstContext, eSessionType, ulSlotNumber, eMEVT_CASUI_REQ_CLOSE_SESSION);
					break;

				default:
					break;
			}
			break;


		default:
			break;
	}


	return ERR_OK;
}

// TODO: shko 이것은 xmgr_cas_iruc_menu_UpdateData 에 합쳐도 무방.
STATIC HERROR xmgr_cas_iruc_menu_UpdateDataForUCServiceStatusSubMenu(IrMenuContext_t *pstContext)
{
	HERROR				hError = ERR_FAIL;
	HUINT32				ulMenuItemLoop = 0;
	SvcCas_MmiMenuList_t		*pstMmiList = NULL;
	CAS_MmiMailList_t		*pstMmiMailList = NULL;
	HUINT8				*pstMmmiText = NULL;
	HUINT16				*pusComponentLength = NULL;
	HUINT8				**ppszComponentsString = NULL;



	HxLOG_Info("s_pstSvcStatusInfo[s_CASIR_SSS_SubItem_Index].eSvcType: %d \n", s_CASIR_SSS_SubItem_Index, s_pstSvcStatusInfo[s_CASIR_SSS_SubItem_Index].eSvcType);
	switch (s_pstSvcStatusInfo[s_CASIR_SSS_SubItem_Index].eSvcType)
	{

/* hjlee3 선택 된 메뉴 아이템에 의해서 관련 데이타 만드는 것으로 보인다. */
		case SERVICE_DVB_DESCRAMBLE:
			HxLOG_Info("SERVICE_DVB_DESCRAMBLE\n");

			xsvc_cas_IrUcStartMonitoring(s_pstSvcStatusInfo[s_CASIR_SSS_SubItem_Index].usSCellSvcHandle, IR_MONITOR_ECM);
			xsvc_cas_IrUcSvcStatusGetInformationOfEachService(s_pstSvcStatusInfo[s_CASIR_SSS_SubItem_Index].eSvcType, s_pstSvcStatusInfo[s_CASIR_SSS_SubItem_Index].usSCellSvcHandle,  &s_stSvcStatusInfoEach[eIRUC_STATUS_Info_ECM], eIRUC_STATUS_Info_ECM);
			{
	HxLOG_Debug("\n======================== Get Each Sevice Inform =====================\n");
	HxLOG_Debug("svcType : 0x%X,  usHandle : 0x%x,  monirotMessage : %s \n",
		s_stSvcStatusInfoEach[eIRUC_STATUS_Info_ECM].eSvcType,
		s_stSvcStatusInfoEach[eIRUC_STATUS_Info_ECM].hUC_SVC,
		s_stSvcStatusInfoEach[eIRUC_STATUS_Info_ECM].aucMonitormessage);
	HxLOG_Debug("========================== Each Service END ==========================\n");
			}

			{
				pstMmiList = (SvcCas_MmiMenuList_t *)HLIB_STD_MemCalloc(sizeof(SvcCas_MmiMenuList_t));

				if(pstMmiList == NULL)
				{
					HxLOG_Error("\n\n");
					hError = ERR_FAIL;
					goto END_FUNC;
				}

				hError = xmgr_cas_iruc_menu_MakeEcmMenuListData(pstMmiList);
				if(hError != ERR_OK)
				{
					HxLOG_Error("\n\n");
					goto END_FUNC;
				}

/* 메뉴 관련 데이타를 OM에 전달 한다. OM 이 모냐 모에 약자지 ??
   어케 사용하는지 확인 필요 하다. */
				hError = xmgr_cas_iruc_menu_PostMenuListData(pstContext, (HUINT32)pstContext->ulSlot, pstMmiList, 0);
				if(hError != ERR_OK)
				{
					HxLOG_Error("\n\n");
					goto END_FUNC;
				}
			}

			break;

		case SERVICE_DVB_EMM:
			HxLOG_Info("SERVICE_DVB_EMM\n");

			xsvc_cas_IrUcStartMonitoring(s_pstSvcStatusInfo[s_CASIR_SSS_SubItem_Index].usSCellSvcHandle, IR_MONITOR_EMM);
			xsvc_cas_IrUcSvcStatusGetInformationOfEachService(s_pstSvcStatusInfo[s_CASIR_SSS_SubItem_Index].eSvcType, s_pstSvcStatusInfo[s_CASIR_SSS_SubItem_Index].usSCellSvcHandle, &s_stSvcStatusInfoEach[eIRUC_STATUS_Info_CCA], eIRUC_STATUS_Info_CCA);
			xsvc_cas_IrUcSvcStatusGetInformationOfEachService(s_pstSvcStatusInfo[s_CASIR_SSS_SubItem_Index].eSvcType, s_pstSvcStatusInfo[s_CASIR_SSS_SubItem_Index].usSCellSvcHandle, &s_stSvcStatusInfoEach[eIRUC_STATUS_Info_SOFTCELL], eIRUC_STATUS_Info_SOFTCELL);
			{
				pstMmiList = (SvcCas_MmiMenuList_t *)HLIB_STD_MemCalloc(sizeof(SvcCas_MmiMenuList_t));

				if(pstMmiList == NULL)
				{
					HxLOG_Error("\n\n");
					hError = ERR_FAIL;
					goto END_FUNC;
				}

				hError = xmgr_cas_iruc_menu_MakeEmmMenuListData(pstMmiList);
				if(hError != ERR_OK)
				{
					HxLOG_Error("\n\n");
					goto END_FUNC;
				}

				hError = xmgr_cas_iruc_menu_PostMenuListData(pstContext, (HUINT32)pstContext->ulSlot, pstMmiList, 0);
				if(hError != ERR_OK)
				{
					HxLOG_Error("\n\n");
					goto END_FUNC;
				}
			}

			break;

// TODO: shko 아래는 아직 구현 안함.
		case SERVICE_PVR_RECORD:
			HxLOG_Info("SERVICE_PVR_RECORD\n");
			break;

		case SERVICE_PVR_PLAYBACK:
			HxLOG_Info("SERVICE_PVR_PLAYBACK\n");
			break;

		case SERVICE_PVR_DRM:
			HxLOG_Info("SERVICE_PVR_DRM\n");
			break;

#if defined(S3_MODIFIED_BY_HUMAX)
		case SERVICE_PVR_TSR_RECORD:
			HxLOG_Info("SERVICE_PVR_TSR_RECORD\n");
			break;

		case SERVICE_PVR_TSR_PLAYBACK:
			HxLOG_Info("SERVICE_PVR_TSR_PLAYBACK\n");
			break;
#endif

		default:
			HxLOG_Error("\n");
			hError = ERR_FAIL;
			goto END_FUNC;



	}

END_FUNC:

	if(pstMmiList != NULL)
	{
		if(pstMmiList->pszTitle != NULL)
		{
			HLIB_STD_MemFree(pstMmiList->pszTitle);
			pstMmiList->pszTitle = NULL;
		}

		if(pstMmiList->pszSubTitle != NULL)
		{
			HLIB_STD_MemFree(pstMmiList->pszSubTitle);
			pstMmiList->pszSubTitle = NULL;
		}

		if(pstMmiList->pszBottom != NULL)
		{
			HLIB_STD_MemFree(pstMmiList->pszBottom);
			pstMmiList->pszBottom = NULL;
		}

		if(pstMmiList->ppszItem != NULL)
		{
			for(ulMenuItemLoop = 0; ulMenuItemLoop < pstMmiList->ulNumOfItem; ulMenuItemLoop++)
			{
				if(pstMmiList->ppszItem[ulMenuItemLoop] != NULL)
				{
					HLIB_STD_MemFree(pstMmiList->ppszItem[ulMenuItemLoop]);
					pstMmiList->ppszItem[ulMenuItemLoop] = NULL;
				}
			}
			HLIB_STD_MemFree(pstMmiList->ppszItem);
			pstMmiList->ppszItem = NULL;
		}

		HLIB_STD_MemFree(pstMmiList);
		pstMmiList = NULL;
	}

	if(pstMmiMailList != NULL)
	{
		if(pstMmiMailList->pszTitle != NULL)
		{
			HLIB_STD_MemFree(pstMmiMailList->pszTitle);
			pstMmiMailList->pszTitle = NULL;
		}

		if(pstMmiMailList->pszSubTitle != NULL)
		{
			HLIB_STD_MemFree(pstMmiMailList->pszSubTitle);
			pstMmiMailList->pszSubTitle = NULL;
		}

		if(pstMmiMailList->pszBottom != NULL)
		{
			HLIB_STD_MemFree(pstMmiMailList->pszBottom);
			pstMmiMailList->pszBottom = NULL;
		}

		if(&(pstMmiMailList->ppMailItem) != NULL)
		{
			for(ulMenuItemLoop = 0; ulMenuItemLoop < pstMmiMailList->ucNumOfItem; ulMenuItemLoop++)
			{
				if(pstMmiMailList->ppMailItem[ulMenuItemLoop] != NULL)
				{
					if(pstMmiMailList->ppMailItem[ulMenuItemLoop]->pszText != NULL)
					{
						HLIB_STD_MemFree(pstMmiMailList->ppMailItem[ulMenuItemLoop]->pszText);
						pstMmiMailList->ppMailItem[ulMenuItemLoop]->pszText = NULL;
					}
					HLIB_STD_MemFree(pstMmiMailList->ppMailItem[ulMenuItemLoop]);
					pstMmiMailList->ppMailItem[ulMenuItemLoop] = NULL;
				}
			}
			HLIB_STD_MemFree(&(pstMmiMailList->ppMailItem));
			(pstMmiMailList->ppMailItem) = NULL;
		}

		HLIB_STD_MemFree(pstMmiMailList);
		pstMmiMailList = NULL;
	}

	if(pstMmmiText != NULL)
	{
		HLIB_STD_MemFree(pstMmmiText);
		pstMmmiText = NULL;
	}

	if(pusComponentLength != NULL)
	{
		HLIB_STD_MemFree(pusComponentLength);
		pusComponentLength = NULL;
	}

	if(ppszComponentsString != NULL)
	{
		HLIB_STD_MemFree(ppszComponentsString);
		ppszComponentsString = NULL;
	}

	return hError;
}


#define	_________________________EMM____________________________________________

STATIC HERROR xmgr_cas_iruc_menu_MakeEmmMenuListData(SvcCas_MmiMenuList_t * pstMmiList)
{
	HUINT32				ulMenuItemLoop = 0;
	HERROR				hErr = ERR_FAIL;
	HUINT32				ulMenuItemCount = 0;

	pstMmiList->ulNumOfItem = eCasIrSSSItem_IRUC_EMM_SubItemMax;

	pstMmiList->ppszItem = (HUINT8 **)HLIB_STD_MemCalloc(sizeof(HUINT8*) * pstMmiList->ulNumOfItem);

	if(pstMmiList->ppszItem == NULL)
	{
		hErr = ERR_FAIL;
		HxLOG_Critical("\n\n");
		goto END_FUNC;
	}

	for(ulMenuItemLoop = 0; ulMenuItemLoop < eCasIrSSSItem_IRUC_EMM_SubItemMax; ulMenuItemLoop++)
	{
		pstMmiList->ppszItem[ulMenuItemLoop] = (HUINT8 *)HLIB_STD_MemCalloc(MGR_CAS_STR_LENGTH_LONG);

		if(pstMmiList->ppszItem[ulMenuItemLoop] != NULL)
		{
			hErr = xmgr_cas_iruc_menu_SetEmmParam(ulMenuItemLoop, pstMmiList->ppszItem[ulMenuItemLoop]);
			if(hErr == ERR_FAIL)
			{
				HxLOG_Critical("\n\n");
			}

			HxLOG_Print("Item[%d], pstMmiList->ppszItem[ulMenuItemLoop]: [%s]\n", ulMenuItemLoop, pstMmiList->ppszItem[ulMenuItemCount]);
		}
		else
		{
			HxLOG_Error("pstMmiList->ppszItem[%d] == NULL !!\n", ulMenuItemLoop);
			hErr = ERR_FAIL;
			goto END_FUNC;
		}


	}

END_FUNC:
	return hErr;


}

STATIC HERROR 	xmgr_cas_iruc_menu_SetEmmParam(IR_SSS_SUB_EMMItem_t eItemId, HUINT8 *pszParam)
{

	HERROR 							hErr = ERR_OK;

	HxLOG_Info("address of pszParam: %x \n", pszParam);

	switch(eItemId)
	{

		case eCasIrSSSItem_IRUC_EMM_CCAEmmService:
		case eCasIrSSSItem_IRUC_EMM_Blank:
		case eCasIrSSSItem_IRUC_EMM_SCEmmService:
			snprintf((char *)pszParam, MGR_CAS_STR_LENGTH_LONG, " ");
			break;

		case eCasIrSSSItem_IRUC_EMM_CCASvcHandle:
			snprintf((char *)pszParam, MGR_CAS_STR_LENGTH_LONG, "0x%04X", s_stSvcStatusInfoEach[eIRUC_STATUS_Info_CCA].hUC_SVC);
			break;

		case eCasIrSSSItem_IRUC_EMM_CCASvcStatus:
			snprintf((char *)pszParam, MGR_CAS_STR_LENGTH_LONG, "%s",s_stSvcStatusInfoEach[eIRUC_STATUS_Info_CCA].aucSVCStatusString);
			break;

		case eCasIrSSSItem_IRUC_EMM_CCAEMMMonitorInfo:
			if(s_stSvcStatusInfoEach[eIRUC_STATUS_Info_CCA].bMonitoringValid==TRUE)
			{
				snprintf((char *)pszParam, MGR_CAS_STR_LENGTH_LONG,"%s ",s_stSvcStatusInfoEach[eIRUC_STATUS_Info_CCA].aucMonitormessage);
			}
			else
			{
				snprintf((char *)pszParam, MGR_CAS_STR_LENGTH_LONG," ");
			}
			break;

		case eCasIrSSSItem_IRUC_EMM_SCSvcHandle:
			snprintf((char *)pszParam, MGR_CAS_STR_LENGTH_LONG, "0x%04X", s_stSvcStatusInfoEach[eIRUC_STATUS_Info_SOFTCELL].hUC_SVC);
			break;

		case eCasIrSSSItem_IRUC_EMM_SCSvcStatus:
			snprintf((char *)pszParam, MGR_CAS_STR_LENGTH_LONG, "%s",s_stSvcStatusInfoEach[eIRUC_STATUS_Info_SOFTCELL].aucSVCStatusString);
			break;

		case eCasIrSSSItem_IRUC_EMM_SCEMMMonitorInfo:
			if(s_stSvcStatusInfoEach[eIRUC_STATUS_Info_SOFTCELL].bMonitoringValid==TRUE)
			{
				snprintf((char *)pszParam, MGR_CAS_STR_LENGTH_LONG,"%s ",s_stSvcStatusInfoEach[eIRUC_STATUS_Info_SOFTCELL].aucMonitormessage);
			}
			else
			{
				snprintf((char *)pszParam, MGR_CAS_STR_LENGTH_LONG," ");
			}
			break;

		default:
			HxLOG_Error("pSubItem ERROR\n");
			hErr = ERR_FAIL;

	}


	HxLOG_Print("pszParam: [%s]\n", pszParam);

	return hErr;
}


#define	_________________________ECM____________________________________________

#define BUFF_SIZE 128

STATIC HERROR xmgr_cas_iruc_menu_MakeEcmMenuListData(SvcCas_MmiMenuList_t * pstMmiList)
{
/* SoftCell */
	/* Service Handle */
	/* Global Service Status */
	/* Internal Status */

	/*  ECM Source */ // NUM_SC_SLOT
		/* ES Info */
		//	ucNumOfElem = (HUINT8)xsvc_cas_IrSVCSTATUS_GetInformationOfElement(s_pstSvcStatusInfo[s_CASIR_SSS_SubItem_Index].usSCellSvcHandle, j, &pstSvcStatusInfoElem);

	/* Macrovision - Copy Control*/
	/*  Monitoring */	/* ECM INFO */ // NUM_SC_SLOT*MAX_NUM_ECM_MONITOR_PID

/* UC */
	/* Descramble Service : secure Clent Name
	   	 Service Handle : internal Service Handle
		 Service Status : Service Status
		     ES: [espid] , ECM: [ECM PID]
			 ES: [espid] , ECM: [ECM PID]
		 Monitoring : ECM Monitoring info */

	Custom_list_t	stTextListBox;

	HUINT8			buf1[BUFF_SIZE] = {0,};
	HUINT8			buf[BUFF_SIZE] = {0,};
	HUINT8			*pszListItem = NULL;
	HUINT8			*szText = NULL;
	HINT32			i, j;
//	IR_ELEM_STATE	*pstSvcStatusInfoElem = NULL;
	HERROR			hError = ERR_FAIL;
	HUINT32				ulMenuItemLoop = 0;

	CAS_UTIL_LL_InitList(&stTextListBox);

	/* Descramble Service */
	if(s_stSvcStatusInfoEach[s_CASIR_SSS_SubItem_Index].bCCA==TRUE)
	{
		snprintf((char *)buf1, BUFF_SIZE, "Cloaked CA");
	}
	else
	{
		snprintf((char *)buf1, BUFF_SIZE, "Smart Card");
	}

//	snprintf((char*)buf, BUFF_SIZE, "%s|%s", "Descramble Service ", buf1);
	szText =  (HUINT8*)OxAP_Malloc(SvcCas_UtilStrLen(buf1) + 1);
	MWC_UTIL_DvbStrcpy(szText, buf1);
	hError = CAS_UTIL_LL_AddItem(&(stTextListBox), szText);
	if (hError != ERR_OK)
	{
		HxLOG_Error("CAS_UTIL_LL_AddItem() fail!\n");
		goto END_FUNC;
	}

	/* Service Handle */
	snprintf((char *)buf1, BUFF_SIZE, "0x%04X", s_stSvcStatusInfoEach[eIRUC_STATUS_Info_ECM].hUC_SVC);
	szText = (HUINT8*)OxAP_Malloc(SvcCas_UtilStrLen(buf1) + 1);
	MWC_UTIL_DvbStrcpy(szText, buf1);
	HxLOG_Info("szText: %s \n", szText);
	hError = CAS_UTIL_LL_AddItem(&(stTextListBox), szText);
	if (hError != ERR_OK)
	{
		HxLOG_Error("CAS_UTIL_LL_AddItem() fail!\n");
		goto END_FUNC;
	}

	/* Service Status  */
	snprintf((char *)buf1, BUFF_SIZE, "%s",s_stSvcStatusInfoEach[eIRUC_STATUS_Info_ECM].aucSVCStatusString);

	szText = (HUINT8*)OxAP_Malloc(SvcCas_UtilStrLen(buf1) + 1);
	MWC_UTIL_DvbStrcpy(szText, buf1);
	HxLOG_Info("szText: %s \n", szText);
	hError = CAS_UTIL_LL_AddItem(&stTextListBox, szText);
	if (hError != ERR_OK)
	{
		HxLOG_Error("CAS_UTIL_LL_AddItem() fail!\n");
		goto END_FUNC;
	}


	/*	ECM Source */
	for (j = 0; j < NUM_SC_SLOT; j++)
	{
		/* ES Info */
		//ucNumOfElem = (HUINT8)xsvc_cas_IrSvcStatusGetInformationOfElement(s_pstSvcStatusInfo[s_CASIR_SSS_SubItem_Index].usSCellSvcHandle, j, &pstSvcStatusInfoElem);
		if(s_stSvcStatusInfoEach[eIRUC_STATUS_Info_ECM].ucStreamCnt)
		{
			HUINT32	ulStreamIndex = 0;
			for(ulStreamIndex = 0; ulStreamIndex<s_stSvcStatusInfoEach[eIRUC_STATUS_Info_ECM].ucStreamCnt; ulStreamIndex++)
			{
				if(s_stSvcStatusInfoEach[eIRUC_STATUS_Info_ECM].stStreamStatus[ulStreamIndex].ucComponentCnt)
				{
					for( i = 0; i < s_stSvcStatusInfoEach[eIRUC_STATUS_Info_ECM].stStreamStatus[ulStreamIndex].ucComponentCnt; i++ )
					{

/* TODO : hjlee3,
          usEcmOrEmmPID를 어케 얻어 올 것인가
          해당 pid는 filer set 할시에 알수 잇는데....거기서 data를 가지고 오기엔 애매하다....
          SoftCell 에서는 status 정보로 주는데 UC는 어디서 얻어 오는지 모르겟다.
          아래 monitoring 정보에서 얻어오는 수박에 없어 보인다.*/
						snprintf((char*)buf, BUFF_SIZE, "%s: [0x%04X] %s: [0x%04X], %s", "ES", s_stSvcStatusInfoEach[eIRUC_STATUS_Info_ECM].stStreamStatus[ulStreamIndex].EsPidList[i],
							"ECM", s_stSvcStatusInfoEach[eIRUC_STATUS_Info_ECM].stStreamStatus[ulStreamIndex].EcmEmmPid, s_stSvcStatusInfoEach[eIRUC_STATUS_Info_ECM].stStreamStatus[ulStreamIndex].aucStatusString);

						szText = (HUINT8*)OxAP_Malloc(SvcCas_UtilStrLen(buf) + 1);
						MWC_UTIL_DvbStrcpy(szText, buf);
						HxLOG_Info("szText: %s \n", szText);
						hError = CAS_UTIL_LL_AddItem(&stTextListBox, szText);
						if (hError != ERR_OK)
						{
							HxLOG_Error("CAS_UTIL_LL_AddItem() fail!\n");
							goto END_FUNC;
						}
					}
				}
			}
		}
	}

	/* ECM Monitoring INFO */
	if(s_stSvcStatusInfoEach[eIRUC_STATUS_Info_ECM].bMonitoringValid==TRUE)
	{
		for (j = 0; j < NUM_SC_SLOT; j++)
		{
			snprintf((char *)buf, BUFF_SIZE,"Monitoring : %s ",s_stSvcStatusInfoEach[eIRUC_STATUS_Info_ECM].aucMonitormessage);
			szText = (HUINT8*)OxAP_Malloc(SvcCas_UtilStrLen(buf) + 1);
			MWC_UTIL_DvbStrcpy(szText, buf);
			HxLOG_Info("szText: %s \n", szText);
			hError = CAS_UTIL_LL_AddItem(&stTextListBox, szText);
			if (hError != ERR_OK)
			{
				HxLOG_Error("CAS_UTIL_LL_AddItem() fail!\n");
				goto END_FUNC;
			}
		}
	}
	// make mmi data
	pstMmiList->ulNumOfItem = CAS_UTIL_LL_GetNumOfListItem(&stTextListBox);

	pstMmiList->ppszItem = (HUINT8 **)HLIB_STD_MemCalloc(sizeof(HUINT8*) * pstMmiList->ulNumOfItem);

	if(pstMmiList->ppszItem == NULL)
	{
		hError = ERR_FAIL;
		HxLOG_Critical("\n\n");
		goto END_FUNC;
	}

	pszListItem = (HUINT8*)OxAP_Malloc(BUFF_SIZE + 1);
	for(ulMenuItemLoop = 0; ulMenuItemLoop < pstMmiList->ulNumOfItem; ulMenuItemLoop++)
	{
		pstMmiList->ppszItem[ulMenuItemLoop] = (HUINT8 *)HLIB_STD_MemCalloc(MGR_CAS_STR_LENGTH_LONG);

		if(pstMmiList->ppszItem[ulMenuItemLoop] == NULL)
		{
			hError = ERR_FAIL;
			HxLOG_Critical("\n\n");
			goto END_FUNC;
		}

		hError = CAS_UTIL_LL_PopFront(&stTextListBox, (void **)&pszListItem);
		if(hError == ERR_OK)
		{
			MWC_UTIL_DvbStrcpy(pstMmiList->ppszItem[ulMenuItemLoop], pszListItem);
			HxLOG_Info("pstMmiList->ppszItem[%d]: %s \n", ulMenuItemLoop, pstMmiList->ppszItem[ulMenuItemLoop]);
		}
	}

END_FUNC:

	{
		HUINT8	*szItem;
		HERROR	err;
		HINT32			numOfListItems = 0;

		numOfListItems	= CAS_UTIL_LL_GetNumOfListItem(&stTextListBox);
		for (i = 0 ; i < numOfListItems ; i++)
		{
			err = CAS_UTIL_LL_ReadIndexNthItem(&stTextListBox, i, (void **)&szItem);
			if (err != ERR_OK || szItem == NULL)
			{
				continue;
			}

			OxAP_Free(szItem);
		}
		CAS_UTIL_LL_InitList(&stTextListBox);

		if(pszListItem != NULL)
		{
			OxAP_Free(pszListItem);
		}
	}

	return hError;

}


#define	____XPROC_CAS_IR_Menu________________________________________________________________

BUS_Result_t		xproc_cas_IrUC(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t		eBusResult = ERR_BUS_NO_ERROR;
	IrMenuContext_t	*pstContext = NULL;
	HERROR			hErr = ERR_FAIL;

	HxLOG_Info("xproc_cas_IrUC message(0x%x), hAction(0x%x)\n", message, hAction);

	pstContext = xmgr_cas_iruc_menu_GetContext();

	switch(message)
	{
		case eMEVT_BUS_CREATE:
			HxLOG_Print("xproc_cas_IrUC - Create \n");
			BUS_MGR_SetMgrName(BUS_MGR_GetThis(), "xproc_cas_IrUC");
			HxLOG_Debug("hAction: 0x%x \n", hAction);
			hErr = xmgr_cas_iruc_menu_MsgGwmCreate(pstContext, message, hAction, p1, p2, p3);
			if(hErr == ERR_FAIL)
			{
				HxLOG_Print("xproc_cas_IrUC - Create fail!   destroy!!!\n");
				BUS_MGR_Destroy(NULL);
			}

			if((pstContext->eActiveMenuGroup == eCasIrUCMenuGroup_ServiceStatus)||(pstContext->eActiveMenuGroup == eCasIrUCMenuGroup_ClientStatus))
			{
				BUS_SetTimer(CAS_IR_MENU_UPDATE_TIMEOUT_ID, CAS_IR_MENU_UPDATE_TIMEOUT);
			}
			s_ulStrHash = 0;
			break;

		case eMEVT_BUS_DESTROY:
			HxLOG_Print("xproc_cas_IrUC - Destroy \n");
			xmgr_cas_iruc_menu_Destroy(pstContext);
			BUS_KillTimer(CAS_IR_MENU_UPDATE_TIMEOUT_ID);
			break;

		case eOEVT_CAS_CASUI_APP_OPEN_SESSION:
			HxLOG_Info("receive eOEVT_CAS_CASUI_APP_OPEN_SESSION \n");
			if(pstContext != NULL)
			{
				HxLOG_Debug("hAction: 0x%x \n", hAction);
				eBusResult = xmgr_cas_iruc_menu_MsgReqSessionStart(pstContext, message, hAction, p1, p2, p3);
			}
			break;

		case eOEVT_CAS_CASUI_APP_CLOSE_SESSION:
			HxLOG_Info("receive eOEVT_CAS_CASUI_APP_CLOSE_SESSION \n");
			if(pstContext != NULL)
			{
				eBusResult = xmgr_cas_iruc_menu_MsgReqSessionStop(pstContext, message, hAction, p1, p2, p3);
				BUS_MGR_Destroy(NULL);
			}
			break;

		case eOEVT_CAS_CASUI_APP_USERINPUTDATA:
			HxLOG_Info("receive eOEVT_CAS_CASUI_APP_USERINPUTDATA \n");
			if(pstContext->eActiveMenuGroup == eCasIrUCMenuGroup_ServiceStatus)
			{
				s_ulStrHash = 0;
				eBusResult = xmgr_cas_iruc_menu_MsgInputNotifyForUCServiceStatus(pstContext, message, hAction, p1, p2, p3);
			}
			else 	if(pstContext->eActiveMenuGroup == eCasIrUCMenuGroup_MailMessages)
			{
				eBusResult = xmgr_cas_iruc_menu_MsgInputNotifyForMailMessages(pstContext, message, hAction, p1, p2, p3);
			}
			break;

		case eMEVT_BUS_TIMER:
			HxLOG_Info("receive eMEVT_BUS_TIMER \n");
			if(p1 == CAS_IR_MENU_UPDATE_TIMEOUT_ID)
			{
				xmgr_cas_iruc_menu_UpdateCheck(pstContext);
			}
			return MESSAGE_BREAK;

		case eMEVT_BUS_IRDETO_UPDATE_UI:
			HxLOG_Print("msg (0x%x), p1 (0x%x)\n", message, p1);
			xmgr_cas_iruc_menu_UpdateCheck(pstContext);
			break;

		case eMEVT_CAS_OK:
		case eMEVT_CAS_FAIL:
		case eMEVT_CAS_CMD:
		case eMEVT_CAS_CHECK:
			HxLOG_Print("msg (0x%x), p1 (0x%x)\n", message, p1);
			if (p1 == eDxCAS_GROUPID_IR)
			{
				xmgr_cas_iruc_menu_UpdateCheck(pstContext);
			}
			break;

		default:
			break;
	}



	return BUS_ProcessMessageDefault(message, hAction, p1, p2, p3);
}
#endif
/* end of file */
