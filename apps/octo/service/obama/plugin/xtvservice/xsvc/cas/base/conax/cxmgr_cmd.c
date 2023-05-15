
/*-------------------------- System Includes --------------------------------*/
#include <svc_pipe.h>
#include <cxmgr_action.h>
#include "conax.h"

/*-------------------------- Local Includes ---------------------------------*/
static CxUiMsgInfo_t	s_stCxFPInfo;

STATIC HERROR local_cxmgr_cmd_MakeDisplayMsg(CxUiMsgInfo_t* pstMsgInfo, CxUiMsgType_t eMsg, HUINT32 ulParam)
{
	switch (eMsg)
	{
	case eCxUi_GroupMr_Ask:
		{
			CxMaturityRatingValue_t* 										stCxUiMrValue;

			stCxUiMrValue = (CxMaturityRatingValue_t *)ulParam;

			pstMsgInfo->eUiMsgType = eMsg;
			pstMsgInfo->mr_ucProgram = stCxUiMrValue->ucProgram;
			pstMsgInfo->mr_ucSmartcard = stCxUiMrValue->ucSmartcard;
		}
		break;
	case eCxUi_GroupPpv_OrderInfo:
		{
			CxOrderInfo_t* 	stCxUiOrder;

			stCxUiOrder = (CxOrderInfo_t *)ulParam;
			pstMsgInfo->eUiMsgType = eMsg;
			pstMsgInfo->or_ulProdRef = stCxUiOrder->ulProdRef;
			pstMsgInfo->or_usPriceUnit = stCxUiOrder->stPrice.usPriceUnit;
			pstMsgInfo->or_ucPriceDecimal = stCxUiOrder->stPrice.ucPriceDecimal;
			MWC_UTIL_DvbStrcpy(pstMsgInfo->or_szLabel, stCxUiOrder->szLabel);
			MWC_UTIL_DvbStrcpy(pstMsgInfo->or_aucCurrency, stCxUiOrder->stPrice.aucCurrency);
		}
		break;

	case eCxUi_GroupPpv_AcceptViewing:
		{
			CxAcceptViewingInfo_t* 	stCxUiAcceptView;

			stCxUiAcceptView = (CxAcceptViewingInfo_t *)ulParam;
			pstMsgInfo->eUiMsgType = eMsg;
			pstMsgInfo->av_ulProdRef = stCxUiAcceptView->ulProdRef;
			pstMsgInfo->av_usMinuteLeft = stCxUiAcceptView->usMinuteLeft;
			MWC_UTIL_DvbStrcpy(pstMsgInfo->av_szLabel, stCxUiAcceptView->szLabel);
		}
		break;

	case eCxUi_GroupPpv_TokenPpv:
		{
			CxTokenDebitInfo_t* stCxUiTokesDebit;

			stCxUiTokesDebit = (CxTokenDebitInfo_t *)ulParam;

			pstMsgInfo->eUiMsgType = eMsg;
			pstMsgInfo->td_usPurseRef = stCxUiTokesDebit->usPurseRef;
			pstMsgInfo->td_ulEventTag = stCxUiTokesDebit->ulEventTag;		// 24 bits
			pstMsgInfo->td_ucInsufficientTokens = stCxUiTokesDebit->ucInsufficientTokens;
			pstMsgInfo->td_ucTokenMode = stCxUiTokesDebit->ucTokenMode;
			pstMsgInfo->td_ulEventCost = stCxUiTokesDebit->ulEventCost;	// 24 bits
			MWC_UTIL_DvbStrcpy(pstMsgInfo->td_szLabel, stCxUiTokesDebit->szLabel);
		}
		break;

#if defined(CONFIG_MW_CAS_CONAX_FINGERPRINT)
	case eCxUi_GroupOther_Fingerprint:
		{
			CxFingerprint_t* 								pstCxUiFingerPrint;

			pstCxUiFingerPrint = (CxFingerprint_t *)ulParam;

			if (pstCxUiFingerPrint != NULL)
			{
				pstMsgInfo->eUiMsgType = eMsg;
				pstMsgInfo->fp_ulTime = pstCxUiFingerPrint->ulTime;
				HxSTD_memcpy(pstMsgInfo->fp_ucCentTime, pstCxUiFingerPrint->ucCentTime, sizeof(pstCxUiFingerPrint->ucCentTime));
				pstMsgInfo->fp_usDuration = pstCxUiFingerPrint->usDuration;
				pstMsgInfo->fp_usPosX = pstCxUiFingerPrint->usPosX;
				pstMsgInfo->fp_usPosY = pstCxUiFingerPrint->usPosY;
				pstMsgInfo->fp_usHeight = pstCxUiFingerPrint->usHeight;
				MWC_UTIL_DvbStrcpy(pstMsgInfo->fp_szText, pstCxUiFingerPrint->szText);
				pstMsgInfo->fp_ucDisplayed = pstCxUiFingerPrint->ucDisplayed;
				pstMsgInfo->fp_bFPFromProtectedData = pstCxUiFingerPrint->bFPFromProtectedData;
				// prevent #232357
				OxCAS_Free(pstCxUiFingerPrint);
			}
		}
		break;
#endif
	default:
		HxLOG_Critical("\n\n");
		break;
	}

	return	ERR_OK;
}

STATIC HINT32 local_cxmgr_cmd_FpTest(void *szArgList)
{
	char *aucArg=NULL;
	HUINT8 aucStr[20+1]={0,};

	HxLOG_Print("**********************************************************\n");
	HxLOG_Print("* CONAX FingerPrint CMD...\n");
	HxLOG_Print("* IM: Immediate message...\n");
	HxLOG_Print("* SM: Scheduled message...\n");
	HxLOG_Print("**********************************************************\n\n");

	aucArg = (char *)HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
	if( aucArg == NULL )
	{
		HxLOG_Print("[%s:%d] command : error\n", __FUNCTION__, __LINE__);
		return HxCMD_ERR;
	}
	if( snprintf(aucStr, 20, "%s", aucArg) == 0 )
	{
		HxLOG_Print("[%s:%d] command error\n", __FUNCTION__, __LINE__);
		return HxCMD_ERR;
	}

	HLIB_STD_StrUpper(aucStr);

	if(strcmp(aucStr, "IM") == 0 )	// Immediate FingerPrint Message...
	{
		CxUiMsgInfo_t* 		pstFPInfo		= NULL;
		SvcCas_MsgType_e	eMsgType;
		SvcCas_ShowType_e	eShowType;
		CxFingerprint_t 	*pstFPData;
		HUINT8				pCTemp[3] = {0xFF, 0xFF, 0xFF};

		pstFPData = (CxFingerprint_t *)HLIB_MEM_Malloc(sizeof(CxFingerprint_t));


		pstFPData->ulTime = 0xffffff;
		pstFPData->ucCentTime[0] = ((*pCTemp) >> 3);										// bits(23 - 19) : hour(0-23)
		pstFPData->ucCentTime[1] = (((*pCTemp)&0x07) << 3 | ((*(pCTemp + 1)) >> 5));		// bits(18 - 13) : min(0-59)
		pstFPData->ucCentTime[2] = (((*(pCTemp + 1)) & 0x1F) << 1 | ((*(pCTemp + 2)) >> 7));	// bits(12 -  7) : sec(0-59)
		pstFPData->ucCentTime[3] = ((*(pCTemp + 2)) & 0x7F);								// bits( 6 -  0) : cent(0-99)

		pstFPData->usDuration = 1000;	// 10ÃÊ
		pstFPData->usPosX = 100;
		pstFPData->usPosY = 100;
		pstFPData->usHeight = 10;
		HxSTD_StrNCpy(pstFPData->szText, "TEST123456789", CX_MAX_FINGERPRINT);

		pstFPData->ucDisplayed = FALSE;
		pstFPData->bFPFromProtectedData = TRUE;

		HxLOG_Info("Send the immediate finger-print message...\n");
		pstFPInfo = &s_stCxFPInfo;
		local_cxmgr_cmd_MakeDisplayMsg(pstFPInfo, eCxUi_GroupOther_Fingerprint, (HUINT32)pstFPData);
		eMsgType = eCAS_MSG_TYPE_Command;
		eShowType = eCasShowType_Both;

		svc_cas_MgrSubUpdateCasMessage(CXACT_GetActionHandle(HANDLE_NULL), eMsgType, eDxCAS_GROUPID_CX, (void*)pstFPInfo, eShowType);

		return HxCMD_OK;
	}
	else if(strcmp(aucStr, "SM") == 0 )	// Scheduled FingerPrint Message...
	{
		CxUiMsgInfo_t* 		pstFPInfo		= NULL;
		SvcCas_MsgType_e	eMsgType;
		SvcCas_ShowType_e	eShowType;
		CxFingerprint_t 	*pstFPData;
		HUINT8				pCTemp[3] = {0xFF, 0xFF, 0xFF};

		pstFPData = (CxFingerprint_t *)HLIB_MEM_Malloc(sizeof(CxFingerprint_t));


		pstFPData->ulTime = 0xffffff;
		pstFPData->ucCentTime[0] = ((*pCTemp) >> 3);										// bits(23 - 19) : hour(0-23)
		pstFPData->ucCentTime[1] = (((*pCTemp)&0x07) << 3 | ((*(pCTemp + 1)) >> 5));		// bits(18 - 13) : min(0-59)
		pstFPData->ucCentTime[2] = (((*(pCTemp + 1)) & 0x1F) << 1 | ((*(pCTemp + 2)) >> 7));	// bits(12 -  7) : sec(0-59)
		pstFPData->ucCentTime[3] = ((*(pCTemp + 2)) & 0x7F);								// bits( 6 -  0) : cent(0-99)

		pstFPData->usDuration = 1000;	// 10ÃÊ
		pstFPData->usPosX = 100;
		pstFPData->usPosY = 100;
		pstFPData->usHeight = 10;
		HxSTD_StrNCpy(pstFPData->szText, "TEST123456789", CX_MAX_FINGERPRINT);

		pstFPData->ucDisplayed = FALSE;
		pstFPData->bFPFromProtectedData = TRUE;

		HxLOG_Info("Send the immediate finger-print message...\n");
		pstFPInfo = &s_stCxFPInfo;
		local_cxmgr_cmd_MakeDisplayMsg(pstFPInfo, eCxUi_GroupOther_Fingerprint, (HUINT32)pstFPData);
		eMsgType = eCAS_MSG_TYPE_Command;
		eShowType = eCasShowType_Both;

		svc_cas_MgrSubUpdateCasMessage(CXACT_GetActionHandle(HANDLE_NULL), eMsgType, eDxCAS_GROUPID_CX, (void*)pstFPInfo, eShowType);

		return HxCMD_OK;
	}
	else if(strcmp(aucStr, "POSITION") == 0 )	// Scheduled FingerPrint Message...
	{
		CxUiMsgInfo_t* 		pstFPInfo		= NULL;
		SvcCas_MsgType_e	eMsgType;
		SvcCas_ShowType_e	eShowType;
		CxFingerprint_t 	*pstFPData;
		HUINT8				pCTemp[3] = {0xFF, 0xFF, 0xFF};
		int num;

		pstFPData = (CxFingerprint_t *)HLIB_MEM_Malloc(sizeof(CxFingerprint_t));

		pstFPData->ulTime = 0xffffff;
		pstFPData->ucCentTime[0] = ((*pCTemp) >> 3);										// bits(23 - 19) : hour(0-23)
		pstFPData->ucCentTime[1] = (((*pCTemp)&0x07) << 3 | ((*(pCTemp + 1)) >> 5));		// bits(18 - 13) : min(0-59)
		pstFPData->ucCentTime[2] = (((*(pCTemp + 1)) & 0x1F) << 1 | ((*(pCTemp + 2)) >> 7));	// bits(12 -  7) : sec(0-59)
		pstFPData->ucCentTime[3] = ((*(pCTemp + 2)) & 0x7F);								// bits( 6 -  0) : cent(0-99)

		pstFPData->usDuration = 1000;	// 10ÃÊ

		aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
		if( aucArg != NULL )
		{
			if( sscanf(aucArg, "%d", &num) != 0 )
			{
				pstFPData->usPosX = num;
			}
		}

		aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
		if( aucArg != NULL )
		{
			if( sscanf(aucArg, "%d", &num) != 0 )
			{
				pstFPData->usPosY = num;
			}
		}

		pstFPData->usHeight = 10;
		HxSTD_StrNCpy(pstFPData->szText, "TEST123456789", CX_MAX_FINGERPRINT);

		pstFPData->ucDisplayed = FALSE;
		pstFPData->bFPFromProtectedData = TRUE;

		HxLOG_Info("Send the immediate finger-print message...\n");
		pstFPInfo = &s_stCxFPInfo;
		local_cxmgr_cmd_MakeDisplayMsg(pstFPInfo, eCxUi_GroupOther_Fingerprint, (HUINT32)pstFPData);
		eMsgType = eCAS_MSG_TYPE_Command;
		eShowType = eCasShowType_Both;

		svc_cas_MgrSubUpdateCasMessage(CXACT_GetActionHandle(HANDLE_NULL), eMsgType, eDxCAS_GROUPID_CX, (void*)pstFPInfo, eShowType);

		return HxCMD_OK;
	}
	else
	{
		HxLOG_Print("humandks_cmd [%s:%d]\n", __FUNCTION__, __LINE__);
	}

	return HxCMD_ERR;
}


STATIC HINT32 local_cxmgr_cmd_UiTest(void *szArgList)
{
	char *aucArg=NULL;
	HUINT8 aucStr[20+1]={0,};

	HxLOG_Print("**********************************************************\n");
	HxLOG_Print("* CONAX UI CMD...\n");
	HxLOG_Print("* SS: Subscription Status...\n");
	HxLOG_Print("* ES: Event Status...\n");
	HxLOG_Print("**********************************************************\n\n");

	aucArg = (char *)HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
	if( aucArg == NULL )
	{
		HxLOG_Print("[%s:%d] command : error\n", __FUNCTION__, __LINE__);
		return HxCMD_ERR;
	}
	if( snprintf(aucStr, 20, "%s", aucArg) == 0 )
	{
		HxLOG_Print("[%s:%d] command error\n", __FUNCTION__, __LINE__);
		return HxCMD_ERR;
	}

	HLIB_STD_StrUpper(aucStr);

	if(strcmp(aucStr, "SS") == 0 )	// Subscription Status...
	{
		CxStatusList_t* pstList;
		CxStatus_t* pstStatus;
		HxLOG_Info("GetStatusInfo...\n");
		pstList = CXAPI_UiGetStatusInfo(0, eCxCaStatus_AllSubscription, 0);

		if(pstList == NULL)
		{
			HxLOG_Info("S/C Read Error...\n");
		}
		else
		{
			do
			{
				pstStatus = &(pstList->stStatus);
				HxLOG_Info("====================\n");
				HxLOG_Info("[%s][Start Date: %04d. %02d. %02d][End Date: %02d. %02d. %02d]\n",
					pstStatus->stSubscription.szLabel,
					pstStatus->stSubscription.stStartDate[0].usYear,
					pstStatus->stSubscription.stStartDate[0].ucMonth,
					pstStatus->stSubscription.stStartDate[0].ucDay,
					pstStatus->stSubscription.stEndDate[0].usYear,
					pstStatus->stSubscription.stEndDate[0].ucMonth,
					pstStatus->stSubscription.stEndDate[0].ucDay
					);
				HxLOG_Info("====================\n");

				if(pstList->pstNext != NULL)
				{
					pstList = pstList->pstNext;
				}
				else
				{
					HxLOG_Info("NO MORE LIST...\n");
					break;
				}
			}while(1);
		}

		return HxCMD_OK;
	}
	else if(strcmp(aucStr, "ES") == 0 )	// Subscription Status...
	{
		CxStatusList_t* pstList;
		CxStatus_t* pstStatus;
		HxLOG_Info("GetStatusInfo...\n");
		pstList = CXAPI_UiGetStatusInfo(0, eCxCaStatus_AllPpvEvent, 0);

		if(pstList == NULL)
		{
			HxLOG_Info("S/C Read Error...\n");
		}
		else
		{
			do
			{
				pstStatus = &(pstList->stStatus);
				HxLOG_Info("====================\n");
				HxLOG_Info("[%s][Start Date: %04d. %02d. %02d][End Date: %02d. %02d. %02d]\n",
					pstStatus->stEvent.szLabel,
					pstStatus->stEvent.stStartDate.usYear,
					pstStatus->stEvent.stStartDate.ucMonth,
					pstStatus->stEvent.stStartDate.ucDay,
					pstStatus->stEvent.stEndDate.usYear,
					pstStatus->stEvent.stEndDate.ucMonth,
					pstStatus->stEvent.stEndDate.ucDay
					);
				HxLOG_Info("====================\n");

				if(pstList->pstNext != NULL)
				{
					pstList = pstList->pstNext;
				}
				else
				{
					HxLOG_Info("NO MORE LIST...\n");
					break;
				}
			}while(1);
		}

		return HxCMD_OK;
	}
	else
	{
		HxLOG_Print("humandks_cmd [%s:%d]\n", __FUNCTION__, __LINE__);
	}

	return HxCMD_ERR;
}

HINT32 CXCMD_Execute(void *szArgList)
{
	char *aucArg=NULL;
	HUINT8 aucStr[20+1]={0,};

	HxLOG_Print("**********************************************************\n");
	HxLOG_Print("* CONAX CMD...\n");
	HxLOG_Print("**********************************************************\n\n");

	/* 1st argument */
	aucArg = (char *)HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
	if( aucArg == NULL )
	{
		HxLOG_Print("[%s:%d] command : error\n", __FUNCTION__, __LINE__);
		return HxCMD_ERR;
	}
	if( snprintf(aucStr, 20, "%s", aucArg) == 0 )
	{
		HxLOG_Print("[%s:%d] command error\n", __FUNCTION__, __LINE__);
		return HxCMD_ERR;
	}

	HLIB_STD_StrUpper(aucStr);

	if(strcmp(aucStr, "UI") == 0 )
	{
		return local_cxmgr_cmd_UiTest(szArgList);
	}
	else if(strcmp(aucStr, "FP") == 0 )
	{
		return local_cxmgr_cmd_FpTest(szArgList);
	}
	else
	{
		HxLOG_Print("humandks_cmd [%s:%d]\n", __FUNCTION__, __LINE__);
	}

	return HxCMD_ERR;
}

