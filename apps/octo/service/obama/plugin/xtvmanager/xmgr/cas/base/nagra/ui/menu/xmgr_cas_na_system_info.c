//******************************************************************************
//	INCLUDE
//******************************************************************************
#include <octo_common.h>


#include <bus.h>
#include <mgr_cas.h>
#include <xmgr_cas.h>
#include <xmgr_cas_res_str.h>

#include <namgr_main.h>

#include "../../local_include/_xmgr_cas_na_util.h"
#include "../../local_include/_xmgr_cas_na_adapt.h"
#include "../../local_include/_xmgr_cas_na.h"


//******************************************************************************
//	DEFINE
//******************************************************************************
#define _____TIMER_INFO_____________

#define CAS_EXPIRE_UPDATE_TIMER_ID						0x3a01				// ??? how to be determined??
#define CAS_EXPIRE_UPDATE_TIMER_TIME					(10*1000)			// 10sec


typedef struct tag_CAS_NASlot_Context
{
	Handle_t				hSession;
	HUINT32					ulSlot;
	OxMgrCasUiGrid_t		stUiGrid;
} XmgrCas_NaSlot_t;


//******************************************************************************
//	STATIC VARIABLES
//******************************************************************************
STATIC XmgrCas_NaSlot_t 			s_stNaSlotContext;


//******************************************************************************
//	PROTOTYPE FUNCTIONS
//******************************************************************************
STATIC XmgrCas_NaSlot_t	*_xmgr_cas_NaUiSysInfo_GetContext(void);
STATIC HERROR			_xmgr_cas_NaUiSysInfo_SendMenuMessage(XmgrCas_NaSlot_t *pstContext);
STATIC BUS_Result_t		_xmgr_cas_NaUiSysInfo_Setup(XmgrCas_NaSlot_t *pstContext);
STATIC BUS_Result_t		_xmgr_cas_NaUiSysInfo_Update(XmgrCas_NaSlot_t *pstContext);
#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
STATIC HERROR			_xmgr_cas_NaUiSysInfo_CheckActionID(HUINT32 ulActionID);
#endif
STATIC BUS_Result_t		_xmgr_cas_NaUiSysInfo_DestroyUI(XmgrCas_NaSlot_t *pstContext);


#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
extern HUINT32 MGR_ACTION_GetMainActionId(void);
#endif


//******************************************************************************
//	CODE START
//******************************************************************************

STATIC XmgrCas_NaSlot_t	*_xmgr_cas_NaUiSysInfo_GetContext(void)
{
	return &s_stNaSlotContext;
}

STATIC HERROR _xmgr_cas_NaUiSysInfo_SendMenuMessage(XmgrCas_NaSlot_t *pstContext)
{
	OxMgrCasUiEvtGrid_t	*pstEvent;

	if(pstContext->hSession == HANDLE_NULL)
	{
		HxLOG_Error("pstContext->hSession is HANDLE_NULL\n");
		return ERR_FAIL;
	}

	pstEvent = (OxMgrCasUiEvtGrid_t *)HLIB_STD_MemCalloc(sizeof(OxMgrCasUiEvtGrid_t));
	if(pstEvent == NULL)
	{
		HxLOG_Error("HLIB_STD_MemCalloc() error\n");
		return ERR_FAIL;
	}

	pstEvent->stSessionInfo.eSessionType	= eDxCAS_SESSIONTYPE_MENU;
	pstEvent->stSessionInfo.hSession		= pstContext->hSession;
	pstEvent->stSessionInfo.ulUiHandle		= (HUINT32)xproc_cas_NaUiSysInfo;

	HxSTD_memcpy(&pstEvent->stGrid.stHeader, &pstContext->stUiGrid.stHeader, sizeof(OxMgrCasUiGridHeader_t));
	pstEvent->stGrid.ulTotalNum = pstContext->stUiGrid.ulTotalNum;
	pstEvent->stGrid.astGridItem = pstContext->stUiGrid.astGridItem;

	if(BUS_PostMessageToOM(NULL, eMEVT_CASUI_TYPE_GRID_DATA, HANDLE_NULL, (HUINT32)eDxCAS_GROUPID_NA, 0, (HUINT32)pstEvent) != ERR_BUS_NO_ERROR)
	{
		HxLOG_Error("BUS_PostMessageToOM() error\n");
	}

	return ERR_OK;
}

STATIC HERROR		_xmgr_cas_NaUiScInfoRequestSessionStop(XmgrCas_NaSlot_t *pContext)
{
	HxLOG_Error("Check function[_xmgr_cas_NaUiScInfoRequestSessionStop]...\n");

	// 사용하지 않는 MGR_APPMGR_MODULE_SendUiEvent 불렀던 부분으로 Prevent issue 때문에 수정한 부분
	// 이 부분이 호출 되지 않을 것으로 예상되나, 만약 Project 진행 시 호출 된다면 아래 xmgr_cas_NaUtil_PostMessageSessionEventToOM 함수로 Session Close를 정상적으로 요청하는지 검증 필요
	xmgr_cas_NaUtil_PostMessageSessionEventToOM(eDxCAS_SESSIONTYPE_MENU, xproc_cas_NaUiSysInfo, pContext->hSession, pContext->ulSlot, eMEVT_CASUI_REQ_CLOSE_SESSION);

	return ERR_OK;
}

STATIC BUS_Result_t		_xmgr_cas_NaUiSysInfo_Setup(XmgrCas_NaSlot_t *pstContext)
{
	HxSTD_memset(pstContext, 0, sizeof(XmgrCas_NaSlot_t));

	pstContext->hSession = HANDLE_NULL;
	pstContext->ulSlot = 0; /* Current Slot Number */

	pstContext->stUiGrid.ulTotalNum = eNaSysInfoItem_Item_Max;
	pstContext->stUiGrid.stHeader.ulAttr = 0;
	pstContext->stUiGrid.stHeader.ulNumOfColumn = 2;
	//pstContext->stUiGrid.stHeader.szTitleAlign;
	//pstContext->stUiGrid.stHeader.szTitle;
	//pstContext->stUiGrid.stHeader.szSubTitle;
	//pstContext->stUiGrid.stHeader.szBottom;
	MWC_UTIL_DvbStrncpy(pstContext->stUiGrid.stHeader.szItemAlign, "left|right", MGR_CAS_STR_LENGTH_LONG);
	pstContext->stUiGrid.stHeader.ulFocusType = eMgrCasUiFocusType_Column;
	pstContext->stUiGrid.stHeader.stFocus.ulHorIdx = 0;
	pstContext->stUiGrid.stHeader.stFocus.ulVerIdx = 0;
	pstContext->stUiGrid.stHeader.stStartPos.ulHorIdx = 0;
	pstContext->stUiGrid.stHeader.stStartPos.ulVerIdx = 0;

	/* alloc items */
	pstContext->stUiGrid.astGridItem = (OxMgrCasUiGridItem_t *)OxCAS_Malloc((sizeof(OxMgrCasUiGridItem_t) * pstContext->stUiGrid.ulTotalNum));
	if(pstContext->stUiGrid.astGridItem == NULL)
	{
		HxLOG_Error("OxCAS_Malloc() error\n");
		return ERR_FAIL;
	}

	HxSTD_memset(pstContext->stUiGrid.astGridItem, 0, sizeof(OxMgrCasUiGridItem_t) * (pstContext->stUiGrid.ulTotalNum));

	return ERR_OK;
}


STATIC BUS_Result_t		_xmgr_cas_NaUiSysInfo_Update(XmgrCas_NaSlot_t *pstContext)
{
	HINT32					i = 0, itemIdx = 0, slStream = 0;
	HBOOL					bDisplay = TRUE;
	OxMgrCasUiGridItem_t	*pSubItem = NULL;
	HUINT8					szParam[MGR_CAS_STR_LENGTH_LONG];
	HCHAR					*pszEncodedStr, *pszProperties;
	HUINT8					*aSysInfo_StrId[eNaSysInfoItem_Item_Max] =	{
#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
														"STR_PROJECT_NAME_ID",
														"STR_SMARTCARD_VERSION_ID",
														"STR_CA_VERSION_ID",
														"STR_SMARTCARD_SERIAL_NUMBER_ID",
#if	defined(CONFIG_MW_CAS_NAGRA_OP_SES)
														"STR_CARD_CREDIT_ID",
#endif
														"STR_CA_SERIAL_NUMBER_ID",
														"STR_CHIPSET_NUID_ID",
														"STR_CHIPSET_TYPE_ID",
														"STR_CHIPSET_REV_ID",
#if	defined(CONFIG_MW_CAS_NAGRA_CA_PRODUCT_ICME) || defined(CONFIG_MW_CAS_NAGRA_CA_PRODUCT_ME) || defined(CONFIG_MW_CAS_NAGRA_CA_PRODUCT_ALME)
														"STR_CSC_INDEX_ID",
#endif
#if (CONFIG_MW_CAS_NUM_SC_SLOT > 1)
														"STR_SMARTCARD_VERSION_ID",
														"STR_SMARTCARD_SERIAL_NUMBER_ID",
#endif
#if defined(CONFIG_MW_CAS_NAGRA_PRM)
														"STR_DVL_VERSION_ID",
														"STR_DVL_CHIPSETTYPE_ID",
#endif
#else
														"LOC_PROJECT_NAME_ID",
														"LOC_SMARTCARD_VERSION_ID",
														"LOC_CA_VERSION_ID",
														"LOC_SMARTCARD_SERIAL_NUMBER_ID",
														"LOC_CA_SERIAL_NUMBER_ID",
														"LOC_CHIPSET_NUID_ID",
														"LOC_CHIPSET_TYPE_ID",
														"LOC_CHIPSET_REV_ID",
#if	defined(CONFIG_MW_CAS_NAGRA_CA_PRODUCT_ICME) || defined(CONFIG_MW_CAS_NAGRA_CA_PRODUCT_ME) || defined(CONFIG_MW_CAS_NAGRA_CA_PRODUCT_ALME)
														"LOC_CSC_INDEX_ID",
#endif
#if (CONFIG_MW_CAS_NUM_SC_SLOT > 1)
														"LOC_SMARTCARD_VERSION_ID",
														"LOC_SMARTCARD_SERIAL_NUMBER_ID",
#endif
#if defined(CONFIG_MW_CAS_NAGRA_PRM)
														"LOC_DVL_VERSION_ID",
#endif
#endif
													};
	xmgr_cas_NaUpdateSystemInfo();

	// check smartcard (TRUE=>occured error~!)
	if (xmgr_cas_NaCheckCardErrorOnMenu())
	{
		bDisplay = FALSE;
		HxLOG_Print("[%s:%d] -> don't display\n", __FUNCTION__, __HxLINE__);
	}

	for(i = 0; i < eNaSysInfoItem_Item_Max; i++)
	{
#if defined(CONFIG_MW_CAS_NAGRA_CA_PRODUCT_ICME)
		if(i == eNaSysInfoItem_iccVersion)
		{
			// if cardless -> remove the item of icc version
			if(xmgr_cas_NaUtil_GetNumberOfIcc() == 0)
			{
				pstContext->stUiGrid.ulTotalNum--;
				continue;
			}
		}
#endif
		pSubItem = &(pstContext->stUiGrid.astGridItem[itemIdx]);
		if (pSubItem == NULL)
		{
			HxLOG_Error("pSubItem ERROR\n");
			return ERR_FAIL;
		}

		pSubItem->ulIndex = itemIdx++;

#if !(CONFIG_MW_CAS_NUM_SC_SLOT > 1)
		if(i == eNaSysInfoItem_Smartcard_Status)
		{
			HxSTD_memset(szParam, 0, MGR_CAS_STR_LENGTH_LONG);

			xmgr_cas_NaGetSystemInfo((XmgrCas_NaSystemInfoItem_e)i, szParam, MGR_CAS_STR_LENGTH_LONG, TRUE);
			slStream = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
			if(slStream == 0)
			{
				HxLOG_Error("slStream is 0\n");
				return ERR_FAIL;
			}

			HLIB_RWSTREAM_Print(slStream, "[");

			if(SvcCas_UtilStrLen(szParam) != 0)
			{
				pszEncodedStr = xmgr_cas_NaUtil_EncodeString(szParam);
				if(pszEncodedStr != NULL)
				{
					HLIB_RWSTREAM_Print(slStream, "{\"id\":\"%s\"}", pszEncodedStr);
					HLIB_STD_MemFree(pszEncodedStr);
					pszEncodedStr = NULL;
				}
			}

			HLIB_RWSTREAM_Print(slStream, "]");
			pszProperties = (HCHAR*)HLIB_RWSTREAM_CloseWithoutBuf(slStream);

			MWC_UTIL_DvbStrncpy(pSubItem->szString, pszProperties, MGR_CAS_STR_LENGTH_LONG);
			pSubItem->ulStringLength = SvcCas_UtilStrLen(pszProperties);

			HLIB_STD_MemFree(pszProperties);
			continue;
		}
#endif
		slStream = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
		if(slStream == 0)
		{
			HxLOG_Error("slStream is 0\n");
			return ERR_FAIL;
		}

		HLIB_RWSTREAM_Print(slStream, "[");
		HLIB_RWSTREAM_Print(slStream, "{\"id\":\"%s\"}", aSysInfo_StrId[i]);

		if (bDisplay)
		{
			HxSTD_memset(szParam, 0, MGR_CAS_STR_LENGTH_LONG);

			//szParam[0] = '|';
			xmgr_cas_NaGetSystemInfo((XmgrCas_NaSystemInfoItem_e)i, szParam, MGR_CAS_STR_LENGTH_LONG, TRUE);
			if(SvcCas_UtilStrLen(szParam) == 0)
			{
				//MWC_UTIL_DvbStrncpy(szParam, "| ", MGR_CAS_STR_LENGTH_LONG);
				szParam[0] = ' ';
			}

			HxLOG_Print("[%s:%d] [%d][%s]\n", __FUNCTION__, __HxLINE__, i, szParam);

			pszEncodedStr = xmgr_cas_NaUtil_EncodeString(szParam);
			if(pszEncodedStr != NULL)
			{
				HLIB_RWSTREAM_Print(slStream, ", \"%s\"", pszEncodedStr);
				HLIB_STD_MemFree(pszEncodedStr);
				pszEncodedStr = NULL;
			}
		}

		HLIB_RWSTREAM_Print(slStream, "]");
		pszProperties = (HCHAR*)HLIB_RWSTREAM_CloseWithoutBuf(slStream);

		MWC_UTIL_DvbStrncpy(pSubItem->szString, pszProperties, MGR_CAS_STR_LENGTH_LONG);
		pSubItem->ulStringLength = SvcCas_UtilStrLen(pszProperties);

		HLIB_STD_MemFree(pszProperties);
	}

	return ERR_OK;
}


#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
STATIC HERROR			_xmgr_cas_NaUiSysInfo_CheckActionID(HUINT32 ulActionID)
{
	HERROR bCheck = ERR_OK;

	if (ulActionID != MGR_ACTION_GetMainActionId())
	{
		HxLOG_Error("received, but [%x] isn't main actiod id!\n", ulActionID);
		bCheck = ERR_FAIL;
	}

	return bCheck;
}
#endif

STATIC BUS_Result_t		_xmgr_cas_NaUiSysInfo_DestroyUI(XmgrCas_NaSlot_t *pstContext)
{
	HxLOG_Print("[_xmgr_cas_NaUiSysInfo_DestroyUI]\n");

#if 0	// om_cas_mmi에서 free가 이루어진다.
	if (pstContext->astSubItems != NULL)
	{
		OxAP_Free(pstContext->astSubItems);
		pstContext->astSubItems = NULL;
	}
#endif

	return ERR_OK;
}

STATIC BUS_Result_t _xmgr_cas_NaUiScInfoMsgInputNotify(XmgrCas_NaSlot_t *pstContext, HINT32 message, HINT32 p1, HINT32 p2, HINT32 p3)
{
//	HERROR 					hError = ERR_FAIL;
	OxMgrCasInputType_e 	eInputType = (OxMgrCasInputType_e)p2;
	OxMgrCasUiInputNotify_t	*pstCasInputData = (OxMgrCasUiInputNotify_t *)p3;

	HxLOG_Print("[_xmgr_cas_NaUiScInfoMsgInputNotify] InputType[%d]", eInputType);

	if(eInputType == eMgrCasInputType_Etc)
	{
		if(pstCasInputData->stEtc.eMgrCasEtcType == eMgrCasEtc_Previous)
		{
			HxLOG_Print("[_xmgr_cas_NaUiScInfoMsgInputNotify] : eMgrCasEtc_Previous ");
			_xmgr_cas_NaUiScInfoRequestSessionStop(pstContext);
		}
		else if(pstCasInputData->stEtc.eMgrCasEtcType == eMgrCasEtc_Exit)
		{
			HxLOG_Print("[_xmgr_cas_NaUiScInfoMsgInputNotify] : eMgrCasEtc_Exit ");
			_xmgr_cas_NaUiScInfoRequestSessionStop(pstContext);
		}

	}

	return ERR_BUS_MESSAGE_BREAK;
}

STATIC BUS_Result_t _xmgr_cas_NaUiSysInfo_MsgOpenSession(XmgrCas_NaSlot_t *pstContext, HINT32 message, HINT32 p1, HINT32 p2, HINT32 p3)
{
	OxMgrCasSessionEvent_t	*pstSession = (OxMgrCasSessionEvent_t *)p3;

	pstContext->hSession = pstSession->hSession;

	xmgr_cas_NaUtil_PostMessageSessionEventToOM(eDxCAS_SESSIONTYPE_MENU, xproc_cas_NaUiSysInfo, pstSession->hSession, pstSession->ulCasSlotNum, eMEVT_CASUI_SESSION_OPENED);

	_xmgr_cas_NaUiSysInfo_Update(pstContext);
	_xmgr_cas_NaUiSysInfo_SendMenuMessage(pstContext);

	return ERR_BUS_MESSAGE_BREAK;
}

STATIC BUS_Result_t _xmgr_cas_NaUiSysInfo_MsgCloseSession(XmgrCas_NaSlot_t *pstContext, HINT32 message, HINT32 p1, HINT32 p2, HINT32 p3)
{
	pstContext->hSession = HANDLE_NULL;

	// TODO: free item

	return BUS_MGR_Destroy(NULL);
}

#if defined(CONFIG_DEBUG)
STATIC HUINT8 *_xmgr_cas_NaUiScInfoGetMessageName(HINT32 message)
{
	switch(message)
	{
		ENUM_TO_STR(eMEVT_CAS_OK);
		ENUM_TO_STR(eMEVT_CAS_FAIL);
		ENUM_TO_STR(eMEVT_CAS_CMD);
		ENUM_TO_STR(eMEVT_CAS_CHECK);

//		ENUM_TO_STR(MSG_APP_UPDATE_SMARTCARD_INFO);
		ENUM_TO_STR(eMEVT_XMGR_CAS_NA_APS_MESSAGE_UPDATED);
		ENUM_TO_STR(eMEVT_CAS_SYSINFO_UPDATED);

		ENUM_TO_STR(eMEVT_CAS_SC_REMOVED);
		ENUM_TO_STR(eMEVT_CAS_SC_INSERTED);

		//ENUM_TO_STR(MSG_OAPI_CASUI_CLOSE);
		ENUM_TO_STR(eOEVT_CAS_CASUI_APP_OPEN_SESSION);
		ENUM_TO_STR(eOEVT_CAS_CASUI_APP_CLOSE_SESSION);
//		ENUM_TO_STR(MSG_OAPI_CASUI_INPUT_NOTIFY);
//		ENUM_TO_STR(MSG_OAPI_CASUI_CHANGE_LANGUAGE);

		default:
			return NULL;
	}
}
#endif

/******************************************************************************/
/**
*   \fn     xproc_cas_NaUiSmartCardInfo(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
*   \brief  system status procedure
*
*   @param[in]  gwm msg/action handler
*   @return     BUS_ProcessMessageDefault(message, hAction, p1, p2, p3)
*   @ingroup    SYSTEM STATUS
*/
/******************************************************************************/
BUS_Result_t		xproc_cas_NaUiSysInfo(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
	HUINT32				ulActionId;
#endif
	XmgrCas_NaSlot_t	*pstContext = _xmgr_cas_NaUiSysInfo_GetContext();
#if 0		// not used
	HUINT32				nResult = ERR_FAIL;
#endif

#ifdef CONFIG_DEBUG
	HUINT8 *pMsg = _xmgr_cas_NaUiScInfoGetMessageName(message);
	if(pMsg != NULL)
	{
		HxLOG_Print("message(%s), hAct(%x), p1(%x), p2(%x), p3(%x)\n", pMsg, hAction, p1, p2, p3);
	}
#endif

	switch(message)
	{
		case eMEVT_BUS_CREATE:
			BUS_MGR_SetMgrName(BUS_MGR_GetThis(), "xproc_cas_NaUiSysInfo");
			_xmgr_cas_NaUiSysInfo_Setup(pstContext);
#if 0//defined(CONFIG_MW_CAS_NAGRA_OP_SES)
// AP와 확인 필요한 상태라 우선 막음 by mslee
			/* Set expiration update timer */
			BUS_SetTimer(CAS_EXPIRE_UPDATE_TIMER_ID, CAS_EXPIRE_UPDATE_TIMER_TIME);
#endif
			break;

#if defined(CONFIG_OP_SES)
		/* standby에서도 떠 있어야 하므로 그냥 PASS
		    stanby mode진입후 바로 live로 전환시 해당 proc이 distory되는 문제로인해 live 전환시 msg 처리 오류 발생.
		*/
		case eMEVT_BUS_READY_SHUTDOWN:
			return MESSAGE_PASS;
#endif

//		case MSG_APP_UPDATE_SMARTCARD_INFO :
/*
	아래 조건으로 인해 menu가 display되어 있는 상태에서 SC status가 update되지 않는 경우(upside down) 발생함.
	아래 조건이 필요한 경우 추가 확인 필요.
*/
#if 0//!defined(CONFIG_MW_CAS_NAGRA_OP_TIVU) && !defined(CONFIG_MW_CAS_NAGRA_OP_MEDIASET)
			if (xmgr_cas_NaPincodeCheckSCRemoved() == FALSE)
				break;
#endif

#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
		case eMEVT_XMGR_CAS_NA_APS_MESSAGE_UPDATED :
#endif
		case eMEVT_CAS_SYSINFO_UPDATED :
			HxLOG_Print(" %x\n", message);

			if (p1 == eDxCAS_GROUPID_NA)
			{
#if (CONFIG_MW_CAS_NUM_SC_SLOT > 1)
				// slot 별 in/out event를 update
				xmgr_cas_NaUpdateSystemInfo();
#endif
				_xmgr_cas_NaUiSysInfo_Update(pstContext);
				_xmgr_cas_NaUiSysInfo_SendMenuMessage(pstContext);
			}
			break;

		case eMEVT_CAS_CHECK :
		case eMEVT_CAS_FAIL :
			HxLOG_Print(" eSEVT_CAS_FAIL\n");
#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
			ulActionId = SVC_PIPE_GetActionId(hAction);
			if (_xmgr_cas_NaUiSysInfo_CheckActionID(ulActionId) == ERR_OK)
			{
				return MESSAGE_BREAK;
			}
#endif
			break;

		case eOEVT_CAS_CASUI_APP_OPEN_SESSION:
			_xmgr_cas_NaUiSysInfo_MsgOpenSession(pstContext, message, p1, p2, p3);
			break;

		case eOEVT_CAS_CASUI_APP_CLOSE_SESSION:
			_xmgr_cas_NaUiSysInfo_MsgCloseSession(pstContext, message, p1, p2, p3);
			break;
#if 0
		case MSG_OAPI_CASUI_INPUT_NOTIFY:
			nResult = _xmgr_cas_NaUiScInfoMsgInputNotify(pstContext, message, p1, p2, p3);
			if(nResult != ERR_OK)
			{
				return nResult;
			}
			break;

		case MSG_OAPI_CASUI_CHANGE_LANGUAGE:
			break;
#endif
		case eMEVT_BUS_TIMER:
#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
			if( p1 == CAS_EXPIRE_UPDATE_TIMER_ID )
			{
				// Query HD+ Expire date
#if 0 //mslee
				xmgr_cas_NaUpdateCasMessage_Aps(eCAS_NA_UI_APS_UPDATE_PRV);
#endif
				// update cas information
				_xmgr_cas_NaUiSysInfo_Update(pstContext);
				_xmgr_cas_NaUiSysInfo_SendMenuMessage(pstContext);

				// reset timer
				BUS_ResetTimer(CAS_EXPIRE_UPDATE_TIMER_ID);
			}
#endif
			return MESSAGE_BREAK;

		case eMEVT_BUS_DESTROY:
			/* destroy UI */
			_xmgr_cas_NaUiSysInfo_DestroyUI(pstContext);

#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
			BUS_KillTimer(CAS_EXPIRE_UPDATE_TIMER_ID);
#endif

#if 0 //mslee #if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
			xmgr_cas_NaUpdateCasMessage_Aps(eCAS_NA_UI_APS_UPDATE_PRV);
#endif
			break;

		default:
			break;
	}

	return BUS_ProcessMessageDefault(message, hAction, p1, p2, p3);
}


