/**************************************************************
 *	@file		xmgr_cas_ci_ciplusui.c
 *	common interface subcas CI+ system message
 *
 *	http://www.humaxdigital.com
 *
 *	@data			2012/06/27
 *	@author			humax
 *	@brief
 **************************************************************/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>

#include <svc_cas.h>
#include <ci_ctrl.h>

#include <mgr_common.h>
#include <mgr_action.h>
#include <mgr_cas.h>

#include <xmgr_cas_res_str.h>

#include "../local_include/_xmgr_cas_ci.h"


/*******************************************************************/
/********************      Defines         *************************/
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

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

typedef enum
{
	eCI_PLUS_SESSION_STATUS_Stopped,
	eCI_PLUS_SESSION_STATUS_StartReady,
	eCI_PLUS_SESSION_STATUS_Started,

	eCI_PLUS_SESSION_STATUS_Max
} XmgrCas_CiPlugSessionStatus_e;

// 각 Ui Type 각각의 Session Handle을 관리한다. 즉, 서로 다른 Ui Type은 동시에 화면에 뿌려질 수 있고, 동일한 Ui Type은 동시에 출력이 불가능 하다.
// 따라서 필요한 경우 적절히 Ui Type을 추가 하면 된다.
typedef enum
{
	eCI_PLUS_UI_TYPE_Confirm,				// authentication과 관련된 일반적인 메세지.
	eCI_PLUS_UI_TYPE_Banner,				// 상단에 banner 형태로 noti 해주는 메세지. banner는 짧게 화면 상단에 보여지는 것이므로 여러개로 따로 관리하지 않음(필요시 나눠도 됨)
	eCI_PLUS_UI_TYPE_PinRequest,			// Pin Code 요청 관련 메세지.

	eCI_PLUS_UI_TYPE_Max
} XmgrCas_CiPlugUiType_e;

typedef struct
{
	XmgrCas_CiPlugUiType_e			eUiType;
	DxCAS_MmiSessionType_e			eSessionType;
	XmgrCas_CiPlugSessionStatus_e	eSessionStatus;
	Handle_t						hSession;
	BUS_Callback_t					hUi;
	SvcCas_CiPlusCcReportMode_e		eCcReport;
	SvcCas_CiArcErr_e				eArcErrType;
	void				 			*pstEvent;
} XmgrCas_CiPlusUiInfo_t;

typedef struct tagApCiplusUiData
{
	HUINT32							ulSlot;

	HxList_t						*pstUiInfoList;
} XmgrCas_CiPlusUiContext_t;


/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

STATIC XmgrCas_CiPlusUiContext_t		s_astCiplusUiContext[XMGR_CAS_CI_SLOT_MAX];

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Local Functions      ********************/
/*******************************************************************/
#define ____LOCAL_FUNCTION_________________________________________________________________________________________
INLINE XmgrCas_CiPlusUiContext_t *xmgr_cas_CiPlusGetContext(HUINT8 ucSlotNum)
{
	#if defined(CONFIG_DEBUG)
	HxLOG_Assert(ucSlotNum < XMGR_CAS_CI_SLOT_MAX);
	#endif /* defined(CONFIG_DEBUG) */

	return &(s_astCiplusUiContext[ucSlotNum]);
}

#define _____LOCAL_EVENT_PROC______________________________________________________
STATIC BUS_Result_t xmgr_cas_CiPlusInitContext(XmgrCas_CiPlusUiContext_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32						ulSlot = (HUINT32)p1;
	DxCAS_MmiSessionType_e		eSessionType = (DxCAS_MmiSessionType_e)p2;
	HCHAR						*pzProcName = NULL;
	XmgrCas_CiPlusUiInfo_t		*pstUiInfo = NULL;
	//BUS_Callback_t				pfnUiProc = NULL;
	BUS_Result_t				hBusResult = ERR_BUS_NO_ERROR;
	HUINT32						i = 0;

	HxLOG_Info("+++++++++++++++++++++\n");
	HxSTD_MemSet((void *)pstContext, 0x00, sizeof(XmgrCas_CiPlusUiContext_t));

	pstContext->ulSlot = ulSlot;

	for (i=0; i < eCI_PLUS_UI_TYPE_Max; i++)
	{
		pstUiInfo = HLIB_STD_MemCalloc(sizeof(XmgrCas_CiPlusUiInfo_t));
		if(pstUiInfo == NULL)
		{
			HxLOG_Critical("Allocation Error!!!\n\n");
			return ERR_BUS_MESSAGE_PASS;
		}

		pstUiInfo->eUiType = (XmgrCas_CiPlugUiType_e)i;
		pstUiInfo->eSessionType = eSessionType;				// Init 할 때 p2로 받은 것으로 Session Type을 모두 동일하게 만들지만, UI Type 마다 다르게 정의해서 사용해도 됨.
		pstUiInfo->eSessionStatus = eCI_PLUS_SESSION_STATUS_Stopped;
		pstUiInfo->hSession = HANDLE_NULL;
		pstUiInfo->eCcReport = eCiPlus_CcReport_Max;
		pstUiInfo->pstEvent = NULL;

		switch(pstUiInfo->eUiType)
		{
			case eCI_PLUS_UI_TYPE_Confirm:
				pstUiInfo->eSessionType = eDxCAS_SESSIONTYPE_EXTENSION;
				pstUiInfo->hUi = xproc_cas_CiPlusConfirmMsg;
				if(pstUiInfo->hUi != NULL)
				{
					if(BUS_MGR_Get(pstUiInfo->hUi) == NULL)
					{
						pzProcName = "xproc_cas_CiPlusConfirmMsg";
						hBusResult = BUS_MGR_Create(pzProcName, APP_CAS_PRIORITY, pstUiInfo->hUi, 0, 0, 0, 0);
						if(hBusResult != ERR_BUS_NO_ERROR)
						{
							HxLOG_Info("bus result %d\n", hBusResult);
						}
					}
				}
				break;
			case eCI_PLUS_UI_TYPE_Banner:
				pstUiInfo->eSessionType = eDxCAS_SESSIONTYPE_EXTENSION;
				pstUiInfo->hUi = xproc_cas_CiPlusBannerMsg;
				if(pstUiInfo->hUi != NULL)
				{
					if(BUS_MGR_Get(pstUiInfo->hUi) == NULL)
					{
						pzProcName = "xproc_cas_CiPlusBannerMsg";
						hBusResult = BUS_MGR_Create(pzProcName, APP_CAS_PRIORITY, pstUiInfo->hUi, 0, 0, 0, 0);
						if(hBusResult != ERR_BUS_NO_ERROR)
						{
							HxLOG_Info("bus result %d\n", hBusResult);
						}
					}
				}
				break;
			case eCI_PLUS_UI_TYPE_PinRequest:
				pstUiInfo->eSessionType = eDxCAS_SESSIONTYPE_EXTENSION;
				pstUiInfo->hUi= xproc_cas_CiPlusPinMsg;
				if(pstUiInfo->hUi != NULL)
				{
					if(BUS_MGR_Get(pstUiInfo->hUi) == NULL)
					{
						pzProcName = "xproc_cas_CiPlusPinMsg";
						hBusResult = BUS_MGR_Create(pzProcName, APP_CAS_PRIORITY, pstUiInfo->hUi, 0, 0, 0, 0);
						if(hBusResult != ERR_BUS_NO_ERROR)
						{
							HxLOG_Info("bus result %d\n", hBusResult);
						}
					}
				}
				break;
			default:
				HxLOG_Error("Unknown UI Type!!!\n");
				break;
		}

		pstContext->pstUiInfoList = HLIB_LIST_Append(pstContext->pstUiInfoList, (void *)pstUiInfo);
		pstUiInfo = NULL;
	}

	HxLOG_Info("--------------------\n");

	return ERR_BUS_MESSAGE_PASS;
}

STATIC HERROR xmgr_cas_CiPlusGetCcReportInfo(HUINT32 ulSlotId, HUINT32 ulVersion, XmgrCas_CiPlusUiInfo_t *pstUiInfo)
{
	HERROR						hErr = ERR_OK;
	SvcCas_CtrlParam_t			stParam;
	CI_GetCcReportOutParam_t	stCcReport;

	stCcReport.usSlotId		= ulSlotId;
	stCcReport.ulEvtVersion	= ulVersion;
	stParam.pvOut			= &stCcReport;
	stParam.ulControlType	= eCACTRL_CI_GetCcReport;
	hErr = SVC_CAS_CtrlGet(eDxCAS_GROUPID_CAM, eCACTRL_CI_GetCcReport, &stParam);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("\nMW_CAS_CTRL_Get() ERROR!\n");
		return ERR_FAIL;
	}

	HxLOG_Info("usSlotId[%02x], CcReprotType[%02x]\n", stCcReport.usSlotId, stCcReport.pData->eCcReportType);

	pstUiInfo->eCcReport = stCcReport.pData->eCcReportType;
	pstUiInfo->eArcErrType = stCcReport.pData->enArcErrType;

	return ERR_OK;
}

STATIC HCHAR *xmgr_cas_CiPlusEncodeString(HCHAR *string)
{
	HUINT32		Len = 0;
	HCHAR		*buf = NULL;

	Len = HLIB_STD_URLStrLen(string) + 1;
    buf = HLIB_STD_MemCalloc(Len+1);
    HLIB_STD_URLEncode(buf,string,Len);

    return buf;
}

STATIC HERROR xmgr_cas_CiPlusSetHeaderString(XmgrCas_CiPlusUiContext_t *pstContext, XmgrCas_CiPlusUiInfo_t *pstUiInfo)
{
	HUINT8					*pszCamName = NULL;
	HERROR					hErr = ERR_FAIL;
	HINT32					slStream;
	HCHAR					szBuf[MGR_CAS_STR_LENGTH_LONG];
	HCHAR					*pzEncodedStr = NULL;
	HCHAR					*pzProperties = NULL;
	HUINT8					ucSlotId;
	OxMgrCasUiEvtInput_t	*pstEvent = (OxMgrCasUiEvtInput_t *)pstUiInfo->pstEvent;
	OxMgrCasUiInputHeader_t	*pstHeader = &pstEvent->stHeader;
	HUINT32					ulMaxSlotNum = 0;

	// 1. Title String 관련 Setting...
	hErr = SVC_CAS_GetNumOfCISlot(&ulMaxSlotNum);
	hErr = SVC_CAS_GetCamName(pstContext->ulSlot, (HUINT8 **)&pszCamName);
	if (hErr == ERR_OK)
	{
		HxLOG_Info("CAM Name:[%s].\n", pszCamName);

		slStream = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
		HLIB_RWSTREAM_Print(slStream, "[");
		HLIB_RWSTREAM_Print(slStream, "{\"id\":\"LOC_CIPLUS_SLOT_ID\"}");

		// 1-1 UI를 그릴 때 사용할 Slot ID 획득.
		HxSTD_MemSet(szBuf, 0, sizeof(HCHAR)*MGR_CAS_STR_LENGTH_LONG);
		if (ulMaxSlotNum == 2)
		{
			// DI에서는 2Slot인 경우 아래쪽 Slot Id 는 0, 위쪽 Slot Id 는 1로 처리 된다.
			if (pstContext->ulSlot == 0)		// 2slot 중 아랫쪽 slot에 대한 처리
			{
				ucSlotId = 2;
			}
			else if (pstContext->ulSlot == 1)	// 2slot 중 윗쪽 slot에 대한 처리
			{
				ucSlotId = 1;
			}
			else
			{
				HxLOG_Error("ERROR! Wrong Slot NUM!![%d]\n", pstContext->ulSlot);
				ucSlotId = pstContext->ulSlot;
			}
		}
		else if (ulMaxSlotNum == 1)
		{
			ucSlotId = 1;
		}
		else
		{
			HxLOG_Error("ERROR! Wrong MAX CI Slot num!![%d]\n", ulMaxSlotNum);
			ucSlotId = pstContext->ulSlot;
		}

		// 1-2 Slot ID 와 CAM Name 저장
		HxSTD_snprintf(szBuf, MGR_CAS_STR_LENGTH_LONG, " %d : %s", ucSlotId, pszCamName);
		pzEncodedStr = xmgr_cas_CiPlusEncodeString(szBuf);
		if(pzEncodedStr != NULL)
		{
			HLIB_RWSTREAM_Print(slStream, ", \"%s\"", pzEncodedStr);
			HLIB_STD_MemFree(pzEncodedStr);
			pzEncodedStr = NULL;
		}

		HLIB_RWSTREAM_Print(slStream, "]");
		pzProperties = (HCHAR*)HLIB_RWSTREAM_CloseWithoutBuf(slStream);

		HxSTD_MemCopy(pstHeader->szTitle, pzProperties, MGR_CAS_STR_LENGTH_LONG);
		HLIB_STD_MemFree(pzProperties);

		HxLOG_Info("szTitle:[%s].\n", pstHeader->szTitle);
	}
	else
	{
		HxLOG_Error("SVC_CAS_GetCamName() ERROR!!\n");
		HxSTD_MemSet(pstHeader->szTitle, 0, sizeof(HCHAR)*MGR_CAS_STR_LENGTH_LONG);
	}

	// 2. Sub-Title 관련 setting...
	slStream = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
	HLIB_RWSTREAM_Print(slStream, "[");

	switch(pstUiInfo->eCcReport)
	{
		case eCiPlus_CcReport_CanNotLoadHostLicenseConstants:
			HLIB_RWSTREAM_Print(slStream, "{\"id\":\"LOC_CIPLUS_POPUP_TEXT1_ID\"}");
			pstHeader->ulAttr = eMgrCasUiAttr_Subtitle;
			break;
		case eCiPlus_CcReport_HostLicenseConstantsNotSaved:
			HLIB_RWSTREAM_Print(slStream, "{\"id\":\"LOC_CIPLUS_POPUP_TEXT2_ID\"}");
			pstHeader->ulAttr = eMgrCasUiAttr_Subtitle;
			break;
		case eCiPlus_CcReport_VirginCamInsertedAndAuthenticationInProgress:
			HLIB_RWSTREAM_Print(slStream, "{\"id\":\"LOC_CIPLUS_POPUP_TEXT4_ID\"}");
			pstHeader->ulAttr = eMgrCasUiAttr_Subtitle | eMgrCasUiAttr_Title | eMgrCasUiAttr_Timeout;
			pstHeader->ulTimeoutSecond = 30;
			break;
		case eCiPlus_CcReport_AuthenticationFailed:
			if (pstUiInfo->eArcErrType == CI_ARC_ERR_CAM_Cert_Expired)
			{
				HLIB_RWSTREAM_Print(slStream, "{\"id\":\"LOC_CIPLUS_POPUP_TEXT6_ID\"}");
			}
			else
			{
				HLIB_RWSTREAM_Print(slStream, "{\"id\":\"LOC_CIPLUS_POPUP_TEXT15_ID\"");
				HLIB_RWSTREAM_Print(slStream, ", \"in\":[%d]}", pstUiInfo->eArcErrType);
			}
			pstHeader->ulAttr = eMgrCasUiAttr_Subtitle | eMgrCasUiAttr_Title;

			if(pstUiInfo->eArcErrType == CI_ARC_ERR_SAC_Failed || pstUiInfo->eArcErrType == CI_ARC_ERR_CCK_Failed)
			{
				pstHeader->ulAttr |= eMgrCasUiAttr_Timeout;
				pstHeader->ulTimeoutSecond = 5;
			}
			break;
		case eCiPlus_CcReport_OperationFailed:
			HLIB_RWSTREAM_Print(slStream, "{\"id\":\"LOC_CIPLUS_POPUP_TEXT7_ID\"");
			HLIB_RWSTREAM_Print(slStream, ", \"in\":[%d]}", pstUiInfo->eArcErrType);
			pstHeader->ulAttr = eMgrCasUiAttr_Subtitle | eMgrCasUiAttr_Title;
			break;
		case eCiPlus_CcReport_NoResponseFromCam:
			HLIB_RWSTREAM_Print(slStream, "{\"id\":\"LOC_CIPLUS_POPUP_TEXT8_ID\"");
			HLIB_RWSTREAM_Print(slStream, ", \"in\":[%d]}", pstUiInfo->eArcErrType);
			pstHeader->ulAttr = eMgrCasUiAttr_Subtitle | eMgrCasUiAttr_Title;
			break;
#if defined(CONFIG_MW_CI_PLUS_CC_V_2)
		case eCiPlus_CcReport_PinNotVarified:
			HLIB_RWSTREAM_Print(slStream, "{\"id\":\"LOC_CIPLUS_PIN_TEXT5_ID\"}");
			pstHeader->ulAttr = eMgrCasUiAttr_Subtitle | eMgrCasUiAttr_Title | eMgrCasUiAttr_Timeout;
			pstHeader->ulTimeoutSecond = 30;
			break;
		case eCiPlus_CcReport_RequireCicamPin:
			HLIB_RWSTREAM_Print(slStream, "{\"id\":\"LOC_CIPLUS_PIN_TEXT1_ID\"}");
			pstHeader->ulAttr = eMgrCasUiAttr_Subtitle | eMgrCasUiAttr_Title;
			break;
		case eCiPlus_CcReport_PinErrorRetry:
			HLIB_RWSTREAM_Print(slStream, "{\"id\":\"LOC_CIPLUS_PIN_TEXT2_ID\"}");
			pstHeader->ulAttr = eMgrCasUiAttr_Subtitle | eMgrCasUiAttr_Title;
			break;
#endif
		default:
			HxLOG_Error("Incorrect CC Report Type!! Check It!! [%02x]\n", pstUiInfo->eCcReport);
			return ERR_FAIL;
	}

	HLIB_RWSTREAM_Print(slStream, "]");
	pzProperties = (HCHAR*)HLIB_RWSTREAM_CloseWithoutBuf(slStream);

	HxSTD_MemCopy(pstHeader->szSubTitle, pzProperties, MGR_CAS_STR_LENGTH_LONG);
	HLIB_STD_MemFree(pzProperties);

	HxLOG_Info("Message:[%s]\n", pstHeader->szSubTitle);

	return ERR_OK;
}

STATIC HERROR xmgr_cas_CiPlusSetBannerString(XmgrCas_CiPlusUiInfo_t *pstUiInfo, OxMgrCasUiEvtBanner_t *pstEvent)
{
	HCHAR		*pzProperties = NULL;
	HINT32		slStream;

	slStream = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
	HLIB_RWSTREAM_Print(slStream, "[");

	switch(pstUiInfo->eCcReport)
	{
		case eCiPlus_CcReport_AuthenticationSuccess:
			HLIB_RWSTREAM_Print(slStream, "{\"id\":\"LOC_CIPLUS_POPUP_TEXT5_ID\"}");
			pstEvent->stBanner.ulTimeoutSecond = 3; // 3 sec
			pstEvent->stBanner.ulAttr = eMgrCasUiAttr_Timeout;
			break;
#if defined(CONFIG_MW_CI_PLUS_CC_V_2)
		case eCiPlus_CcReport_PinError:
			HLIB_RWSTREAM_Print(slStream, "{\"id\":\"LOC_CIPLUS_PIN_TEXT3_ID\"}");
			pstEvent->stBanner.ulTimeoutSecond = 3; // 3 sec
			pstEvent->stBanner.ulAttr = eMgrCasUiAttr_Timeout;
			break;
		case eCiPlus_CcReport_PinVarified:
			HLIB_RWSTREAM_Print(slStream, "{\"id\":\"LOC_CIPLUS_PIN_TEXT4_ID\"}");
			pstEvent->stBanner.ulTimeoutSecond = 3; // 3 sec
			pstEvent->stBanner.ulAttr = eMgrCasUiAttr_Timeout;
			break;
#endif
		default:
			HxLOG_Error("Incorrect CC Report Type!! Check It!! [%02x]\n", pstUiInfo->eCcReport);
			return ERR_FAIL;
	}

	HLIB_RWSTREAM_Print(slStream, "]");
	pzProperties = (HCHAR*)HLIB_RWSTREAM_CloseWithoutBuf(slStream);

	HxSTD_MemCopy(pstEvent->stBanner.stString.szString, pzProperties, MGR_CAS_STR_LENGTH_LONG);
	pstEvent->stBanner.stString.ulStringLength = SvcCas_UtilStrLen(pstEvent->stBanner.stString.szString);
	HLIB_STD_MemFree(pzProperties);

	HxLOG_Info("Message:[%s], Length:[%d].\n", pstEvent->stBanner.stString.szString, pstEvent->stBanner.stString.ulStringLength);

	return ERR_OK;
}

STATIC HERROR xmgr_cas_CiPlusPostUiEventToOM(XmgrCas_CiPlusUiInfo_t *pstUiInfo, HUINT32 ulSlot)
{
	HINT32 message = 0;
	HERROR hErr = ERR_FAIL;
	OxMgrCasUiEvtInput_t *pstInputEvent;
	OxMgrCasUiEvtBanner_t *pstBannerEvent;

	/* fix for prevent #172593 */
	if(NULL == pstUiInfo)
	{
		HxLOG_Error("pstUiInfo is NULL\n");
		return ERR_FAIL;
	}
	else
	{
		if(pstUiInfo->pstEvent == NULL)
		{
			HxLOG_Error("pstUiInfo->pstEvent is NULL\n");
			return ERR_FAIL;
		}
	}

	switch(pstUiInfo->eUiType)
	{
		case eCI_PLUS_UI_TYPE_Confirm:
			pstInputEvent = (OxMgrCasUiEvtInput_t *)pstUiInfo->pstEvent;
			pstInputEvent->stSessionInfo.eSessionType = eDxCAS_SESSIONTYPE_EXTENSION;
			pstInputEvent->stSessionInfo.hSession = pstUiInfo->hSession;
			pstInputEvent->stSessionInfo.ulUiHandle = (HUINT32)pstUiInfo->hUi;
			message = eMEVT_CASUI_TYPE_INPUT_EX_DATA;
			break;
		case eCI_PLUS_UI_TYPE_Banner:
			pstBannerEvent = (OxMgrCasUiEvtBanner_t *)pstUiInfo->pstEvent;
			pstBannerEvent->stSessionInfo.eSessionType = eDxCAS_SESSIONTYPE_EXTENSION;
			pstBannerEvent->stSessionInfo.hSession = pstUiInfo->hSession;
			pstBannerEvent->stSessionInfo.ulUiHandle = (HUINT32)pstUiInfo->hUi;
			message = eMEVT_CASUI_TYPE_BANNER_DATA;
			break;
		case eCI_PLUS_UI_TYPE_PinRequest:
			pstInputEvent = (OxMgrCasUiEvtInput_t *)pstUiInfo->pstEvent;
			pstInputEvent->stSessionInfo.eSessionType = eDxCAS_SESSIONTYPE_EXTENSION;
			pstInputEvent->stSessionInfo.hSession = pstUiInfo->hSession;
			pstInputEvent->stSessionInfo.ulUiHandle = (HUINT32)pstUiInfo->hUi;
			message = eMEVT_CASUI_TYPE_INPUT_EX_DATA;
			break;
		default:
			HxLOG_Error("Unknown Ui Type!!!\n");
			return ERR_FAIL;
	}
	hErr = BUS_PostMessageToOM(NULL, message, HANDLE_NULL, eDxCAS_GROUPID_CAM, (HINT32)ulSlot, (HINT32)pstUiInfo->pstEvent);

	if(hErr == ERR_OK)
	{
		// pstUiInfo->pstEvent는 om 에서 메시지를 받고 처리한 후 mem free 를 보장해준다.
		pstUiInfo->pstEvent = NULL;
	}
	else
	{
		HxLOG_Error("Msg Error\n");
		HLIB_STD_MemFree(pstUiInfo->pstEvent);
		pstUiInfo->pstEvent = NULL;
	}

	return hErr;
}

STATIC HERROR xmgr_cas_CiPlusPostSessionEventToOM(XmgrCas_CiPlusUiInfo_t *pstUiInfo, HUINT16 usSlotId, HINT32 eEvent)
{
	BUS_Result_t						hResult = ERR_BUS_NO_ERROR;
	OxMgrCasUiEvtSession_t				*pstEvent = NULL;

	pstEvent = HLIB_STD_MemCalloc(sizeof(OxMgrCasUiEvtSession_t));
	if(pstEvent == (OxMgrCasUiEvtSession_t *)NULL)
	{
		HxLOG_Error("\n");
		return ERR_FAIL;
	}

	pstEvent->eSessionType = pstUiInfo->eSessionType;
	pstEvent->ulUiHandle = (HUINT32)pstUiInfo->hUi;
	pstEvent->hSession = pstUiInfo->hSession;

	hResult = BUS_PostMessageToOM(NULL, eEvent, HANDLE_NULL, eDxCAS_GROUPID_CAM, usSlotId, (HINT32)pstEvent);
	if(hResult != ERR_BUS_NO_ERROR)
	{
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC HERROR xmgr_cas_CiPlusSetConfirmMessage(XmgrCas_CiPlusUiContext_t *pstContext, XmgrCas_CiPlusUiInfo_t *pstUiInfo)
{
	OxMgrCasUiEvtInput_t	*pstEvent = NULL;
	OxMgrCasUiInput_t		*pstInputData = NULL;
	HCHAR					*pzProperties = NULL;
	HINT32					slStream;

	if(pstContext == NULL)
	{
		HxLOG_Error("pstContext is NULL!!()\n");
		return ERR_FAIL;
	}

	pstUiInfo->eUiType = eCI_PLUS_UI_TYPE_Confirm;

	pstEvent = (OxMgrCasUiEvtInput_t *)HLIB_STD_MemCalloc(sizeof(OxMgrCasUiEvtInput_t));
	if(pstEvent == NULL)
	{
		return ERR_FAIL;
	}

	HxSTD_MemSet(pstEvent, 0, sizeof(OxMgrCasUiEvtInput_t));
	pstUiInfo->pstEvent = pstEvent;

	pstEvent->stSessionInfo.eSessionType = pstUiInfo->eSessionType;
	pstInputData = &pstEvent->stInputData;

	xmgr_cas_CiPlusSetHeaderString(pstContext, pstUiInfo);

	pstInputData->eInputType = eMgrCasInputType_Button;
	pstInputData->utInputData.stBtn.ulAttr = 0;
	pstInputData->utInputData.stBtn.eMgrCasBtnType = eMgrCasBtn_1st;

	slStream = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
	HLIB_RWSTREAM_Print(slStream, "[");
	HLIB_RWSTREAM_Print(slStream, "{\"id\":\"LOC_CLOSE_ID\"}");
	HLIB_RWSTREAM_Print(slStream, "]");
	pzProperties = (HCHAR*)HLIB_RWSTREAM_CloseWithoutBuf(slStream);

	HxSTD_MemCopy(pstInputData->utInputData.stBtn.aszButtonName[0], pzProperties, MGR_CAS_STR_LENGTH_SHORT);
	HLIB_STD_MemFree(pzProperties);

	HxLOG_Info("aszButtonName:[%s].\n", pstInputData->utInputData.stBtn.aszButtonName[0]);

	return ERR_OK;
}

// Banner 형태로 출력되는 Message를 Appkit쪽에 올려주는 함수
STATIC HERROR xmgr_cas_CiPlusSetBannerMessage(XmgrCas_CiPlusUiContext_t *pstContext, XmgrCas_CiPlusUiInfo_t *pstUiInfo)
{
	OxMgrCasUiEvtBanner_t	*pstEvent = (OxMgrCasUiEvtBanner_t *)pstUiInfo->pstEvent;
	HERROR					hErr = ERR_OK;

	HxLOG_Info("xmgr_cas_CiPlusPostResponseMessage()\n");

	pstUiInfo->eUiType = eCI_PLUS_UI_TYPE_Banner;

	if(pstContext == NULL)
	{
		HxLOG_Error("pstContext is NULL!!()\n");
		return ERR_FAIL;
	}

	if (pstEvent != NULL)
	{
		HxLOG_Warning("pstEvent is not NULL... The data of pstUiInfo has not consumed yet...\n" );
		HLIB_STD_MemFree(pstEvent);
	}

	pstEvent = (OxMgrCasUiEvtBanner_t *)HLIB_STD_MemCalloc(sizeof(OxMgrCasUiEvtBanner_t));
	if(pstEvent == NULL)
	{
		return ERR_FAIL;
	}

	HxSTD_MemSet(pstEvent, 0, sizeof(OxMgrCasUiEvtBanner_t));

	pstEvent->stSessionInfo.eSessionType = pstUiInfo->eSessionType;

	hErr = xmgr_cas_CiPlusSetBannerString(pstUiInfo, pstEvent);

	return hErr;
}

#if defined(CONFIG_MW_CI_PLUS_CC_V_2)
STATIC HERROR xmgr_cas_CiPlusSetPinRequestMessage(XmgrCas_CiPlusUiContext_t *pstContext, XmgrCas_CiPlusUiInfo_t *pstUiInfo)
{
	OxMgrCasUiEvtInput_t	*pstEvent = NULL;
	HERROR					hErr = ERR_OK;
	OxMgrCasUiInput_t		*pstInputData = NULL;

	HxLOG_Info("xmgr_cas_CiPlusPostResponseMessage()\n");

	pstUiInfo->eUiType = eCI_PLUS_UI_TYPE_PinRequest;

	if(pstContext == NULL)
	{
		HxLOG_Error("pstContext is NULL!!()\n");
		return ERR_FAIL;
	}

	if (pstEvent != NULL)
	{
		HxLOG_Warning("pstEvent is not NULL... The data of pstUiInfo has not consumed yet...\n" );
		HLIB_STD_MemFree(pstEvent);
	}

	pstEvent = (OxMgrCasUiEvtInput_t *)HLIB_STD_MemCalloc(sizeof(OxMgrCasUiEvtInput_t));

	if(pstEvent == (OxMgrCasUiEvtInput_t *)NULL)
	{
		return ERR_FAIL;
	}

	HxSTD_MemSet(pstEvent, 0, sizeof(OxMgrCasUiEvtInput_t));
	pstUiInfo->pstEvent = pstEvent;

	pstEvent->stSessionInfo.eSessionType = pstUiInfo->eSessionType = eDxCAS_SESSIONTYPE_EXTENSION;
	pstInputData = &pstEvent->stInputData;

	xmgr_cas_CiPlusSetHeaderString(pstContext, pstUiInfo);

	pstInputData->eInputType = eMgrCasInputType_Number;
	pstInputData->utInputData.stNumber.ulAttr = eMgrCasUiAttr_HiddenInput ;
	pstInputData->utInputData.stNumber.ulNumberLength = 4; // PIN은 모두 4자리로 처리 함. 4자리가 아닐 경우 추후 변경 필요.
	HxSTD_MemSet(pstInputData->utInputData.stNumber.pszNumber, 0x00, sizeof(MGR_CASINPUT_NUMBER_MAX_LENGTH));

	return hErr;
}
#endif


STATIC HERROR xmgr_cas_CiPlusSetUiMessage(XmgrCas_CiPlusUiContext_t *pstContext, XmgrCas_CiPlusUiInfo_t	 *pstUiInfo)
{
	HERROR 					hErr = ERR_OK;

	switch (pstUiInfo->eCcReport)
	{
		case eCiPlus_CcReport_CanNotLoadHostLicenseConstants:
		case eCiPlus_CcReport_HostLicenseConstantsNotSaved:
		case eCiPlus_CcReport_VirginCamInsertedAndAuthenticationInProgress:
		case eCiPlus_CcReport_AuthenticationFailed:
		case eCiPlus_CcReport_OperationFailed:
		case eCiPlus_CcReport_NoResponseFromCam:
			hErr = xmgr_cas_CiPlusSetConfirmMessage(pstContext, pstUiInfo);
			break;
		case eCiPlus_CcReport_AuthenticationSuccess:
			hErr = xmgr_cas_CiPlusSetBannerMessage(pstContext, pstUiInfo);
			break;
#if defined(CONFIG_MW_CI_PLUS_V_1_3)
		case eCiPlus_CcReport_PinNotVarified:
			hErr = xmgr_cas_CiPlusSetConfirmMessage(pstContext, pstUiInfo);
			break;
		case eCiPlus_CcReport_PinError:
		case eCiPlus_CcReport_PinVarified:
			hErr = xmgr_cas_CiPlusSetBannerMessage(pstContext, pstUiInfo);
			break;
		case eCiPlus_CcReport_RequireCicamPin:
		case eCiPlus_CcReport_PinErrorRetry:
			hErr = xmgr_cas_CiPlusSetPinRequestMessage(pstContext, pstUiInfo);
			break;
#endif
		default:
			break;
	}

	return hErr;
}

STATIC XmgrCas_CiPlusUiInfo_t *xmgr_cas_CiPlusGetUiInfoByUiType(HxList_t *pstUiInfoList, XmgrCas_CiPlugUiType_e eUiType)
{
	HxList_t					*pstUiInfoItem = NULL;
	XmgrCas_CiPlusUiInfo_t		*pstUiInfo = NULL;

	pstUiInfoItem = pstUiInfoList;

	while(pstUiInfoItem != NULL)
	{
		pstUiInfo = (XmgrCas_CiPlusUiInfo_t *)HLIB_LIST_Data(pstUiInfoItem);
		if(pstUiInfo != NULL)
		{
			if(pstUiInfo->eUiType == eUiType)
			{
				break;
			}
		}

		pstUiInfoItem = pstUiInfoItem->next;
		pstUiInfo = NULL;
	}

	return pstUiInfo;
}

STATIC BUS_Result_t xmgr_cas_CiPlusCiplusDataProc (XmgrCas_CiPlusUiContext_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t				hErr = ERR_FAIL;
	HUINT32						ulSlotId = (HUINT32)p1;
	XmgrCas_CiCamStatus_e		eCiCamStatus = p2;
	HUINT32 					ulVersion = (HUINT32)p3;
	XmgrCas_CiPlusUiInfo_t		stUiInfo;
	XmgrCas_CiPlusUiInfo_t		*pstUiInfo = NULL;

	HxLOG_Trace();
	if( eCiCamStatus == eCiCas_CamRemoved )
	{
		HxLOG_Warning("The CAM status is eCiCas_CamRemoved. If this message is shown, check the CI+ UI routine. And don't display the CI+ Message.\n");
		return MESSAGE_BREAK;
	}

	HxSTD_MemSet(&stUiInfo, 0, sizeof(XmgrCas_CiPlusUiInfo_t));
	stUiInfo.pstEvent = NULL;

	hErr = xmgr_cas_CiPlusGetCcReportInfo(ulSlotId, ulVersion, &stUiInfo);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("xmgr_cas_CiPlusGetCcReportInfo() Func ERROR.\n");
		return MESSAGE_BREAK;
	}

	hErr = xmgr_cas_CiPlusSetUiMessage(pstContext, &stUiInfo);

	// Set 완료 한 stUiInfo 값을 context에 저장
	pstUiInfo = xmgr_cas_CiPlusGetUiInfoByUiType(pstContext->pstUiInfoList, stUiInfo.eUiType);
	/* fix for prevent #172593 */
	if(NULL == pstUiInfo)
	{
		HxLOG_Error("pstUiInfo is NULL\n");
		return MESSAGE_BREAK;
	}

	pstUiInfo->eCcReport = stUiInfo.eCcReport;
	pstUiInfo->eArcErrType = stUiInfo.eArcErrType;
	pstUiInfo->pstEvent = stUiInfo.pstEvent;

	// 보낼 수 있는 상황이면 바로 보내고 아닐 경우에는 open session...
	if(pstUiInfo->hSession != HANDLE_NULL)
	{
		HxLOG_Info("pstUiInfo->eSessionStatus[%d] (eSessionStatus is should be 2...)\n", pstUiInfo->eSessionStatus);
		hErr = xmgr_cas_CiPlusPostUiEventToOM(pstUiInfo, pstContext->ulSlot);
	}
	else
	{
		HxLOG_Warning("pstUiInfo->eSessionStatus[%d] (eSessionStatus is should not be 2...)\n", pstUiInfo->eSessionStatus);
		pstUiInfo->eSessionStatus = eCI_PLUS_SESSION_STATUS_StartReady;
		hErr = xmgr_cas_CiPlusPostSessionEventToOM(pstUiInfo, pstContext->ulSlot, eMEVT_CASUI_REQ_OPEN_SESSION);
	}

	return MESSAGE_BREAK;
}

STATIC void xmgr_cas_CiPlusFreeUiInfoItem(XmgrCas_CiPlusUiInfo_t	*pstUiInfo)
{
	if(pstUiInfo!= NULL)
	{
		if(pstUiInfo->pstEvent != NULL)
		{
			HLIB_STD_MemFree(pstUiInfo->pstEvent);
			pstUiInfo->pstEvent = NULL;
		}

		pstUiInfo->eSessionStatus = eCI_PLUS_SESSION_STATUS_Stopped;
		pstUiInfo->hSession = HANDLE_NULL;
		pstUiInfo->eCcReport = eCiPlus_CcReport_Max;
		pstUiInfo->eArcErrType = CI_ARC_ERR_None;
	}
}

STATIC BUS_Result_t xmgr_cas_CiPlusMsgInputNotify(XmgrCas_CiPlusUiContext_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32							ulSlotNumber = (HUINT32)p1;
	XmgrCas_CiPlugUiType_e			eUiType = (XmgrCas_CiPlugUiType_e)p2;
	OxMgrCasUiUserInputNotify_t		*pstUserInputNotify = (OxMgrCasUiUserInputNotify_t *)p3;
	XmgrCas_CiPlusUiInfo_t			*pstUiInfo;

	HxLOG_Trace();

	pstUiInfo = xmgr_cas_CiPlusGetUiInfoByUiType(pstContext->pstUiInfoList, eUiType);

	switch(pstUiInfo->eSessionStatus)
	{
		case eCI_PLUS_SESSION_STATUS_Stopped:
		case eCI_PLUS_SESSION_STATUS_StartReady:
			HxLOG_Assert(FALSE);
			break;
		case eCI_PLUS_SESSION_STATUS_Started:
			break;
		default:
			break;
	}

	switch(pstUserInputNotify->eInputType)
	{
		case eMgrCasInputType_Etc:
			switch(pstUserInputNotify->utData.stEtc.eMgrCasEtcType)
			{
				case eMgrCasEtc_Previous:
					LOG("eMgrCasEtc_Previous...\n");
					break;

				case eMgrCasEtc_Exit:
					LOG("eMgrCasEtc_Exit...\n");
					break;

				default:
					break;
			}
			break;

		case eMgrCasInputType_Number:
			LOG("eMgrCasInputType_Number...\n");
#if defined(CONFIG_MW_CI_PLUS_CC_V_2)
			if(pstUiInfo->eCcReport == eCiPlus_CcReport_RequireCicamPin || pstUiInfo->eCcReport == eCiPlus_CcReport_PinErrorRetry)
			{
				SvcCas_CtrlParam_t					stParam;
				CI_SetPinByUserInParam_t			stInParam;

				LOG("pszNumber(%s), Length(%d)...\n", pstUserInputNotify->utData.stNumber.pszNumber, pstUserInputNotify->utData.stNumber.ulNumberLength);

				stInParam.usSlotId = (HUINT16)ulSlotNumber;
				stInParam.unPinLength = pstUserInputNotify->utData.stNumber.ulNumberLength;
				stInParam.pucPinData = pstUserInputNotify->utData.stNumber.pszNumber;

				stParam.pvIn					= &stInParam;
				stParam.ulControlType			= eCACTRL_CI_SetPinCode;

				if(SVC_CAS_CtrlSet(eDxCAS_GROUPID_CAM, eCACTRL_CI_SetPinCode, &stParam) != ERR_OK)
				{
					ERR_LOG("Critical Eror!!!\n");
				}

				LOG("InputComplete, CI+ Answer Finished...\n");

				break;
			}
#endif
			break;

		case eMgrCasInputType_Selection:
			LOG("eMgrCasInputType_Selection...\n");
			break;

		default:
			break;
	}

	FuncLEAVE(0);

	return ERR_BUS_MESSAGE_BREAK;
}

STATIC BUS_Result_t xmgr_cas_CiPlusMsgOpenSession(XmgrCas_CiPlusUiContext_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	OxMgrCasSessionEvent_t	*pstSession = (OxMgrCasSessionEvent_t *)p3;
	XmgrCas_CiPlugUiType_e	eUiType = (XmgrCas_CiPlugUiType_e)p2;
	XmgrCas_CiPlusUiInfo_t	*pstUiInfo;


	pstUiInfo = xmgr_cas_CiPlusGetUiInfoByUiType(pstContext->pstUiInfoList, eUiType);
	/* fix for prevent #172593 */
	if(NULL == pstUiInfo)
	{
		HxLOG_Error("pstUiInfo is NULL\n");
		return ERR_BUS_MESSAGE_PASS;
	}

	pstUiInfo->hSession = pstSession->hSession;

	HxLOG_Info("Rcved Session Type:[%d], Slot Num:[%d], hSession:[%d], ui handle:[%d]...\n"
		, pstSession->eSessionType, pstSession->ulCasSlotNum, pstSession->hSession, pstSession->ulUiHandle);
	HxLOG_Info("Session Status: [%d] ( <- it should be '1')...\n", pstUiInfo->eSessionStatus);

	xmgr_cas_CiPlusPostSessionEventToOM(pstUiInfo, pstContext->ulSlot, eMEVT_CASUI_SESSION_OPENED);
	pstUiInfo->eSessionStatus = eCI_PLUS_SESSION_STATUS_Started;

	if (pstUiInfo->pstEvent != NULL)
	{
		if (xmgr_cas_CiPlusPostUiEventToOM(pstUiInfo, pstContext->ulSlot) != ERR_OK)
		{
			HxLOG_Critical("xmgr_cas_CiPlusPostUiEventToOM() error\n");
		}
	}

	return ERR_BUS_MESSAGE_PASS;
}

STATIC BUS_Result_t xmgr_cas_CiPlusMsgCloseSession(XmgrCas_CiPlusUiContext_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	XmgrCas_CiPlugUiType_e	eUiType = (XmgrCas_CiPlugUiType_e)p2;
	XmgrCas_CiPlusUiInfo_t	*pstUiInfo;

	// TODO: session closed 는 유지하지 않고, UI를 닫으려면 length를 0으로 올림. 만약 close session이 불렸는데도 UI 가 남아 있다면 length를 0으로 올려주는 작업 필요.
	pstUiInfo = xmgr_cas_CiPlusGetUiInfoByUiType(pstContext->pstUiInfoList, eUiType);

	xmgr_cas_CiPlusFreeUiInfoItem(pstUiInfo);

	return MESSAGE_BREAK;
}


#define _____XMGR_CAS_CI_UI_PUBLIC_FUNCTION______________________________________________________
/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/

BUS_Result_t	xproc_cas_CiPlusConfirmMsg(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t 				eBusRes = ERR_BUS_NO_ERROR;
	XmgrCas_CiPlusUiContext_t	*pstContext = NULL;
	HUINT32						ulSlotNumber = 0;
	OxMgrCasSessionEvent_t 		*pstSessionEvent = NULL;

	switch(message)
	{
		case eMEVT_BUS_CREATE :
			HxLOG_Info("eMEVT_BUS_CREATE... \n");
			BUS_MGR_SetMgrName(BUS_MGR_GetThis(), (HINT8 *)__FUNCTION__);
			break;

		case eOEVT_CAS_CASUI_APP_OPEN_SESSION:
			pstSessionEvent = (OxMgrCasSessionEvent_t *)p3;
			HxLOG_Info("eOEVT_CAS_CASUI_APP_OPEN_SESSION slot %d\n", pstSessionEvent->ulCasSlotNum);
			pstContext = xmgr_cas_CiPlusGetContext(pstSessionEvent->ulCasSlotNum);
			if(pstContext != NULL)
			{
				// p2에서는 Session Type이 넘어오나 아래 함수에서는 쓰지 않고 UI Type이 필요하므로 p2로 넘기도록 함.
				eBusRes = xmgr_cas_CiPlusMsgOpenSession(pstContext, message, hAction, p1, (HINT32)eCI_PLUS_UI_TYPE_Confirm, p3);
			}
			break;

		case eOEVT_CAS_CASUI_APP_CLOSE_SESSION:
			pstSessionEvent = (OxMgrCasSessionEvent_t *)p3;
			HxLOG_Info("eOEVT_CAS_CASUI_APP_CLOSE_SESSION slot %d\n", pstSessionEvent->ulCasSlotNum);
			pstContext = xmgr_cas_CiPlusGetContext(pstSessionEvent->ulCasSlotNum);
			if(pstContext != NULL)
			{
				// p2에서는 Session Type이 넘어오나 아래 함수에서는 쓰지 않고 UI Type이 필요하므로 p2로 넘기도록 함.
				eBusRes = xmgr_cas_CiPlusMsgCloseSession(pstContext, message, hAction, p1, (HINT32)eCI_PLUS_UI_TYPE_Confirm, p3);
			}
			break;

		case eOEVT_CAS_CASUI_APP_USERINPUTDATA:
			ulSlotNumber = (HUINT32)p1;
			HxLOG_Info("eOEVT_CAS_CASUI_APP_USERINPUTDATA slot %d\n", ulSlotNumber);
			pstContext = xmgr_cas_CiPlusGetContext(ulSlotNumber);
			if(pstContext != NULL)
			{
				eBusRes = xmgr_cas_CiPlusMsgInputNotify(pstContext, message, hAction, p1, (HINT32)eCI_PLUS_UI_TYPE_Confirm, p3);
			}
			break;

		case eMEVT_BUS_DESTROY :
			break;

		default:
			break;
	}

	if(eBusRes != ERR_BUS_NO_ERROR)
	{
		eBusRes = BUS_ProcessMessageDefault(message, hAction, p1, p2, p3);
	}

	return eBusRes;
}

BUS_Result_t	xproc_cas_CiPlusBannerMsg(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t 				eBusRes = ERR_BUS_NO_ERROR;
	XmgrCas_CiPlusUiContext_t	*pstContext = NULL;
	HUINT32						ulSlotNumber = 0;
	OxMgrCasSessionEvent_t 		*pstSessionEvent = NULL;

	switch(message)
	{
		case eMEVT_BUS_CREATE :
			HxLOG_Info("eMEVT_BUS_CREATE... \n");
			BUS_MGR_SetMgrName(BUS_MGR_GetThis(), (HINT8 *)__FUNCTION__);
			break;

		case eOEVT_CAS_CASUI_APP_OPEN_SESSION:
			pstSessionEvent = (OxMgrCasSessionEvent_t *)p3;
			HxLOG_Info("eOEVT_CAS_CASUI_APP_OPEN_SESSION slot %d\n", pstSessionEvent->ulCasSlotNum);
			pstContext = xmgr_cas_CiPlusGetContext(pstSessionEvent->ulCasSlotNum);
			if(pstContext != NULL)
			{
				// p2에서는 Session Type이 넘어오나 아래 함수에서는 쓰지 않고 UI Type이 필요하므로 p2로 넘기도록 함.
				eBusRes = xmgr_cas_CiPlusMsgOpenSession(pstContext, message, hAction, p1, (HINT32)eCI_PLUS_UI_TYPE_Banner, p3);
			}
			break;

		case eOEVT_CAS_CASUI_APP_CLOSE_SESSION:
			pstSessionEvent = (OxMgrCasSessionEvent_t *)p3;
			HxLOG_Info("eOEVT_CAS_CASUI_APP_CLOSE_SESSION slot %d\n", pstSessionEvent->ulCasSlotNum);
			pstContext = xmgr_cas_CiPlusGetContext(pstSessionEvent->ulCasSlotNum);
			if(pstContext != NULL)
			{
				// p2에서는 Session Type이 넘어오나 아래 함수에서는 쓰지 않고 UI Type이 필요하므로 p2로 넘기도록 함.
				eBusRes = xmgr_cas_CiPlusMsgCloseSession(pstContext, message, hAction, p1, (HINT32)eCI_PLUS_UI_TYPE_Banner, p3);
			}
			break;

		case eOEVT_CAS_CASUI_APP_USERINPUTDATA:
			ulSlotNumber = (HUINT32)p1;
			HxLOG_Info("eOEVT_CAS_CASUI_APP_USERINPUTDATA slot %d\n", ulSlotNumber);
			pstContext = xmgr_cas_CiPlusGetContext(ulSlotNumber);
			if(pstContext != NULL)
			{
				eBusRes = xmgr_cas_CiPlusMsgInputNotify(pstContext, message, hAction, p1, (HINT32)eCI_PLUS_UI_TYPE_Banner, p3);
			}
			break;

		case eMEVT_BUS_DESTROY :
			break;

		default:
			break;
	}

	if(eBusRes != ERR_BUS_NO_ERROR)
	{
		eBusRes = BUS_ProcessMessageDefault(message, hAction, p1, p2, p3);
	}

	return eBusRes;
}

BUS_Result_t	xproc_cas_CiPlusPinMsg(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t 				eBusRes = ERR_BUS_NO_ERROR;
	XmgrCas_CiPlusUiContext_t	*pstContext = NULL;
	HUINT32						ulSlotNumber = 0;
	OxMgrCasSessionEvent_t 		*pstSessionEvent = NULL;

	switch(message)
	{
		case eMEVT_BUS_CREATE :
			HxLOG_Info("eMEVT_BUS_CREATE... \n");
			BUS_MGR_SetMgrName(BUS_MGR_GetThis(), (HINT8 *)__FUNCTION__);
			break;

		case eOEVT_CAS_CASUI_APP_OPEN_SESSION:
			pstSessionEvent = (OxMgrCasSessionEvent_t *)p3;
			HxLOG_Info("eOEVT_CAS_CASUI_APP_OPEN_SESSION slot %d\n", pstSessionEvent->ulCasSlotNum);
			pstContext = xmgr_cas_CiPlusGetContext(pstSessionEvent->ulCasSlotNum);
			if(pstContext != NULL)
			{
				// p2에서는 Session Type이 넘어오나 아래 함수에서는 쓰지 않고 UI Type이 필요하므로 p2로 넘기도록 함.
				eBusRes = xmgr_cas_CiPlusMsgOpenSession(pstContext, message, hAction, p1, (HINT32)eCI_PLUS_UI_TYPE_PinRequest, p3);
			}
			break;

		case eOEVT_CAS_CASUI_APP_CLOSE_SESSION:
			pstSessionEvent = (OxMgrCasSessionEvent_t *)p3;
			HxLOG_Info("eOEVT_CAS_CASUI_APP_CLOSE_SESSION slot %d\n", pstSessionEvent->ulCasSlotNum);
			pstContext = xmgr_cas_CiPlusGetContext(pstSessionEvent->ulCasSlotNum);
			if(pstContext != NULL)
			{
				// p2에서는 Session Type이 넘어오나 아래 함수에서는 쓰지 않고 UI Type이 필요하므로 p2로 넘기도록 함.
				eBusRes = xmgr_cas_CiPlusMsgCloseSession(pstContext, message, hAction, p1, (HINT32)eCI_PLUS_UI_TYPE_PinRequest, p3);
			}
			break;

		case eOEVT_CAS_CASUI_APP_USERINPUTDATA:
			ulSlotNumber = (HUINT32)p1;
			HxLOG_Info("eOEVT_CAS_CASUI_APP_USERINPUTDATA slot %d\n", ulSlotNumber);
			pstContext = xmgr_cas_CiPlusGetContext(ulSlotNumber);
			if(pstContext != NULL)
			{
				eBusRes = xmgr_cas_CiPlusMsgInputNotify(pstContext, message, hAction, p1, (HINT32)eCI_PLUS_UI_TYPE_PinRequest, p3);
			}
			break;

		case eMEVT_BUS_DESTROY :
			break;

		default:
			break;
	}

	if(eBusRes != ERR_BUS_NO_ERROR)
	{
		eBusRes = BUS_ProcessMessageDefault(message, hAction, p1, p2, p3);
	}

	return eBusRes;
}


BUS_Result_t	xproc_cas_CiPlusSysMsg(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t 		eBusRes = ERR_BUS_NO_ERROR;
	XmgrCas_CiPlusUiContext_t	*pstContext = NULL;
	HUINT32				ulSlotNumber = 0;

	switch(message)
	{
		case eMEVT_BUS_CREATE :
		{
			HUINT32 i = 0;
			HxLOG_Info("eMEVT_BUS_CREATE... \n");
			BUS_MGR_SetMgrName(BUS_MGR_GetThis(), (HINT8 *)__FUNCTION__);
			for(i=0; i<XMGR_CAS_CI_SLOT_MAX; i++)
			{
				pstContext = xmgr_cas_CiPlusGetContext(i);
				if(pstContext != NULL)
				{
					eBusRes = xmgr_cas_CiPlusInitContext(pstContext, message, hAction, (HINT32)i, p2, p3);
				}
				else
				{
					HxLOG_Critical("Get CI+ UI Context FAIL!!\n");
				}
			}
			break;
		}
		case eMEVT_CAS_CIPLUS_CC_REPORT:
			ulSlotNumber = (HUINT32)p1;
			HxLOG_Info("eMEVT_CAS_CIPLUS_CC_REPORT slot %d\n", ulSlotNumber);
			pstContext = xmgr_cas_CiPlusGetContext(ulSlotNumber);
			if(pstContext != NULL)
			{
				eBusRes = xmgr_cas_CiPlusCiplusDataProc(pstContext, message, hAction, p1, p2, p3);
			}
			break;
/*
		case eOEVT_CAS_CASUI_APP_OPEN_SESSION:
			ulSlotNumber = (HUINT32)p1;
			HxLOG_Info("eOEVT_CAS_CASUI_APP_OPEN_SESSION slot %d\n", ulSlotNumber);
			pstContext = xmgr_cas_CiPlusGetContext(ulSlotNumber);
			if(pstContext != NULL)
			{
				eBusRes = xmgr_cas_CiPlusAppReqSessionStart(pstContext, message, hAction, p1, p2, p3);
			}
			break;

		case eOEVT_CAS_CASUI_APP_CLOSE_SESSION:
			ulSlotNumber = (HUINT32)p1;
			HxLOG_Info("eOEVT_CAS_CASUI_APP_CLOSE_SESSION slot %d\n", ulSlotNumber);
			pstContext = xmgr_cas_CiPlusGetContext(ulSlotNumber);
			if(pstContext != NULL)
			{
				eBusRes = xmgr_cas_CiPlusAppReqSessionStop(pstContext, message, hAction, p1, p2, p3);
			}
			break;

		case eOEVT_CAS_CASUI_APP_USERINPUTDATA:
			ulSlotNumber = (HUINT32)p1;
			HxLOG_Info("eOEVT_CAS_CASUI_APP_USERINPUTDATA slot %d\n", ulSlotNumber);
			pstContext = xmgr_cas_CiPlusGetContext(ulSlotNumber);
			if(pstContext != NULL)
			{
				eBusRes = xmgr_cas_CiPlusMsgInputNotify(pstContext, message, hAction, p1, p2, p3);
			}
			break;
*/
		case eMEVT_CAS_CAM_REMOVED :
			// TODO: CAM이 제거 되었을 경우 열려 있는 Session을 모두 close하는 루틴 추가.
			break;
		case eMEVT_BUS_DESTROY :
			break;

		case eMEVT_BUS_READY_SHUTDOWN:
			return ERR_BUS_MESSAGE_PASS;

		default:
			break;
	}

	if(eBusRes != ERR_BUS_NO_ERROR)
	{
		eBusRes = BUS_ProcessMessageDefault(message, hAction, p1, p2, p3);
	}

	return eBusRes;
}

/* EOF */

