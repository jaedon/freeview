
#include <pal_sc.h>

#include "ir_platform.h"

//#if defined(CONFIG_MW_CAS_IRDETO_UCAS)
//#include "UniversalClient_Common_SPI.h"
//#include "UniversalClient_Common_API.h"
//#include "iruc_drv.h"
//#include "iruc_api.h"
//#endif
//#if defined(CONFIG_MW_CAS_IRDETO_SCELL)
//#include "ir_drv.h"
//#endif

#include <_svc_cas_sub_dev_api.h>

#include "vd_humax.h"

#include "ir_api.h"
#include "ir_evt.h"
#include "ir_svc.h"



#define	IrEvtDump		HxLOG_Info  //임시 방편이니 추후 변경 필요


#define	MAX_IR_EVT_BUFFER			16
#define GET_IR_SERVERITY_CHAR_FROM_STATUSCODE(statuscode)	(char) (statuscode>>24)


#if defined(CONFIG_MW_CAS_IRDETO_SCELL)
static HUINT32			s_abwEvtStatus[IR_HUMAX_MAX_SERVICE]; //this roll is substituting to state machine functionality in viva.
static HMSG				s_aHMsgLast[IR_HUMAX_MAX_SERVICE];
#endif

#if defined(CONFIG_MW_CAS_IRDETO_UCAS)
static HUINT32 			s_abwUCEvtStatus[IR_HUMAX_MAX_SERVICE]; //this roll is substituting to state machine functionality in viva.
static HMSG				s_aUCHMsgLast[IR_HUMAX_MAX_SERVICE];
#endif

static IREVT_Data		_stIrEvtData[MAX_IR_EVT_BUFFER];
static HINT32			_nIrEvtDataIndex = 0;

// api for debug message
#ifdef CONFIG_DEBUG // remove warning
static char *local_cas_ir_EVT_LongToBinary(HUINT32 i)
{
	static char s[32 + 1] = { '0', };
	int count = 32;

	do
	{
		s[--count] = '0' + (char) (i & 1);
		i = i >> 1;
	} while (count);

	return s;
}
#endif

static HUINT32 *	local_cas_ir_EVT_GetEvtStatus (Handle_t hAction)
{
	HUINT8	ucIdx;
	HUINT16 usType;

	usType = xsvc_cas_IrGetSvcType((void *)hAction);
	if (usType == IR_CAS_TYPE_SCELL)
	{
#if defined(CONFIG_MW_CAS_IRDETO_SCELL)
		ucIdx = CAS_IR_DRV_GetIrSvcInfoIDXbyActHandle(hAction);
		if(ucIdx >= IR_HUMAX_MAX_SERVICE)
		{
			HxLOG_Assert(0);
			return NULL;
		}
		return &s_abwEvtStatus[ucIdx];
#endif
	}
	else if (usType == IR_CAS_TYPE_UC)
	{
#if defined(CONFIG_MW_CAS_IRDETO_UCAS)
		ucIdx = CAS_IRUC_GetIrSvcInfoIDXbyActHandle(hAction);
		if(ucIdx >= IRUC_HUMAX_MAX_SERVICE)
		{
			HxLOG_Assert(0);
			return NULL;
		}

		return &s_abwUCEvtStatus[ucIdx];
#endif
	}
	return NULL;
}

static HMSG *	local_cas_ir_EVT_GetEvtMsg (Handle_t hAction)
{
	HUINT8	ucIdx;
	HUINT16	usType;

	usType = xsvc_cas_IrGetSvcType((void *)hAction);
	if (usType == IR_CAS_TYPE_SCELL)
	{
#if defined(CONFIG_MW_CAS_IRDETO_SCELL)
		ucIdx = CAS_IR_DRV_GetIrSvcInfoIDXbyActHandle(hAction);
		if(ucIdx >= IR_HUMAX_MAX_SERVICE)
		{
			HxLOG_Assert(0);
			return NULL;
		}

		return &s_aHMsgLast[ucIdx];
#endif
	}
	else if (usType == IR_CAS_TYPE_UC)
	{
#if defined(CONFIG_MW_CAS_IRDETO_UCAS)
		ucIdx = CAS_IRUC_GetIrSvcInfoIDXbyActHandle(hAction);
		if(ucIdx >= IRUC_HUMAX_MAX_SERVICE)
		{
			HxLOG_Assert(0);
			return NULL;
		}

		return &s_aUCHMsgLast[ucIdx];
#endif
	}
	return NULL;
}

#if defined(CONFIG_MW_CAS_IRDETO_COMPONENT_LEVEL_SCRAMBLE)
SvcCas_ShowType_e local_cas_ir_EVT_GetAVState(Handle_t hAction)
{
	HINT32				i, j;
	IR_SERVICE_INFO		*pstIrSvcInfo;
	SvcCas_ShowType_e		eAVShow = eCasShowType_None;

	HxLOG_Info("local_cas_ir_EVT_GetAVState (+) hAction = 0x%0x\n", hAction);

	pstIrSvcInfo = CAS_IR_DRV_GetIrSvcInfobyActHandle(hAction);

	if(pstIrSvcInfo == NULL)
	{
		HxLOG_Error("local_cas_ir_EVT_GetAVState (-) NULL err eAVShow = 0x%0x\n", eAVShow);
		return eAVShow;
	}

	for(i = 0; i < pstIrSvcInfo->ucStreamCount; i++)
	{
		for(j = 0; j < CAS_MAX_NUM_OF_SC; j++)
		{
			if(
			  	(pstIrSvcInfo->astPidList[i].ElemRsc[j].CharSeverity == 'D') &&
				(	(pstIrSvcInfo->astPidList[i].ElemRsc[j].stStatus.wStatus == 29) ||
			    	(pstIrSvcInfo->astPidList[i].ElemRsc[j].stStatus.wStatus == 100) ||
			    	(pstIrSvcInfo->astPidList[i].ElemRsc[j].stStatus.wStatus == 105)
				)
			)
			{
				if(pstIrSvcInfo->astPidList[i].ucTypeFromPlay == eEsType_Video)
				{
					eAVShow |= eCasShowType_VideoOnly;

				}
				else if(pstIrSvcInfo->astPidList[i].ucTypeFromPlay == eEsType_Audio)
				{
					eAVShow |= eCasShowType_AudioOnly;
				}
			}
		}
	}

	HxLOG_Info("local_cas_ir_EVT_GetAVState (-) eAVShow = 0x%0x\n", eAVShow);

	return	eAVShow;
}
#endif

HIR_ERR local_cas_ir_EVT_UpdateSvcStatus(IREVT_SVC_STATE state, Handle_t hAction , HMSG *hMsg)
{
	IREVT_Data	*pEvtData;
	HUINT32 ulStatusCode;
	HUINT32 ulAxiCodeIdx;
	HUINT32	*pStatus;
	HMSG	*pMsg;

	SvcCas_ShowType_e		eShowType = eCasShowType_Both;
	SvcCas_MsgType_e		eCasMsgType = eCAS_MSG_TYPE_Fail;

	HxLOG_Info("state = 0x%X, hAction = 0x%X, hMsg = 0x%X\n", state, hAction, (HUINT32)hMsg );

	pStatus	= local_cas_ir_EVT_GetEvtStatus(hAction);
	pMsg	= local_cas_ir_EVT_GetEvtMsg(hAction);
	if (pStatus == NULL || pMsg == NULL)
	{
		HxLOG_Critical("\n\n");
		return HIR_NOT_OK;
	}

	switch(state)
	{
		case IREVTBW_SVC_INIT:
			HxLOG_Info("IREVTBW_SVC_INIT\n");
			*pStatus = IREVTBW_SVC_INIT;
			break;

		case IREVTBW_SVC_OPENED:
			HxLOG_Info("IREVTBW_SVC_OPENED\n");
			*pStatus |= IREVTBW_SVC_OPENED;
			break;

		case IREVTBW_SVC_DEFINED:
			HxLOG_Info("IREVTBW_SVC_DEFINED\n");
			*pStatus |= IREVTBW_SVC_DEFINED;
			break;

		case IREVTBW_SVC_SDT_RECEIVED:
			HxLOG_Info("IREVTBW_SVC_SDT_RECEIVED\n");
			*pStatus |= IREVTBW_SVC_SDT_RECEIVED;
			break;

		case IREVTBW_SVC_GLOBAL_STATUS:
			HxLOG_Info("IREVTBW_SVC_GLOBAL_STATUS\n");
			*pMsg = *hMsg;
			*pStatus |= IREVTBW_SVC_GLOBAL_STATUS;
			break;

		default:
			break;
	}

	HxLOG_Print("IREVTBW_SVC_TRIGGER =%d,  s_abwEvtStatus= %d\n", IREVTBW_SVC_TRIGGER, *pStatus );

	if( IREVTBW_SVC_TRIGGER >  *pStatus )
	{
#if defined(CONFIG_DEBUG)
		HxLOG_Info("not yet triggered c[%s]\n", local_cas_ir_EVT_LongToBinary(*pStatus) );
		HxLOG_Info("not yet triggered s[%s]\n", local_cas_ir_EVT_LongToBinary(IREVTBW_SVC_TRIGGER)  );
#endif
		return 	HIR_OK;
	}

	/*HEREBY, svc status message all  trigger event */
	HxLOG_Info("Event Trigger\n");

	_nIrEvtDataIndex = (_nIrEvtDataIndex + 1) % MAX_IR_EVT_BUFFER;
	pEvtData = &_stIrEvtData[_nIrEvtDataIndex];

	HxSTD_memset(pEvtData, 0x00, sizeof(IREVT_Data) );

	//AXI CODE index or hMsg.usParam = ucMailIndex;
	ulAxiCodeIdx = pMsg->usParam;
	// severity char & resource & error code.
	ulStatusCode = pMsg->ulParam;

	HxLOG_Info("state= %X, hAction = 0x%X, ulAxiCodeIdx=0x%X\n", state, hAction, ulAxiCodeIdx);

#if defined(CONFIG_MW_CAS_IRDETO_UCAS)
	if (ulAxiCodeIdx == IRUC_STATUS_MSG_NONE)
		return HIR_OK;

	if (ulAxiCodeIdx <= IRUC_STATUS_MSG_E102)
#else
	// TODO: I-055-4 에 대한 Trace 필요하다.
	if( 'D' == GET_IR_SERVERITY_CHAR_FROM_STATUSCODE(ulStatusCode) )
#endif
	{
		HxLOG_Print("Do show display. code (0x%X)\n", ulAxiCodeIdx);
		eCasMsgType = eCAS_MSG_TYPE_Ok;
	}
	else
	{
		HxLOG_Print("Do not display Error. code (0x%X)\n", ulAxiCodeIdx);
		eCasMsgType = eCAS_MSG_TYPE_Fail;
		//	screen blank if cas error occurred.
		//	need test more...
		//	audio만 play... Irdeto에서 Audio가 FTA이면 그냥 PLay하라는 말이 이능...
#if defined(CONFIG_OP_DIGITURK)
		// Service Component Based Scrambled test중에 Video는 descrmable 가능하고 Audio는 불가능한 경우
		// Error message를 display하면서 Video가 나와야 함.
		// 마찬가지로 둘중 한나가 FTA인 경우에도 나와야 함.
		eShowType = eCasShowType_Both;
#else
		eShowType = eCasShowType_AudioOnly;
#endif
	}

#if defined(CONFIG_MW_CAS_IRDETO_COMPONENT_LEVEL_SCRAMBLE)
	eShowType = local_cas_ir_EVT_GetAVState(hAction);
#endif

	if (ulAxiCodeIdx == AXI_STATUSCODE_IDX_E04_4 || ulAxiCodeIdx == AXI_STATUSCODE_IDX_E05_4 || ulAxiCodeIdx == AXI_STATUSCODE_IDX_E06_4 || ulAxiCodeIdx == AXI_STATUSCODE_IDX_I07_4)
	{
		// ALMAJD 에서 FTA block 하는데 smart card 없는데 audio 가 나와서 smart card error 는 audio 도 안나오도록 한다.
		eShowType = eCasShowType_None;
	}

	pEvtData->eIrEvtClass = pMsg->usMsgClass;
	pEvtData->ulAxiCodeIdx = ulAxiCodeIdx;

	HxLOG_Info("hAct= 0x%X, CasMsgType=%d, eShowType=%d\n", hAction, eCasMsgType, eShowType);

#if defined(CONFIG_DEBUG)
	//print debug.
	local_cas_ir_EVT_DEBUG_PrintEvtClass(pEvtData->eIrEvtClass);
#endif

	if(ERR_OK != svc_cas_MgrSubUpdateCasMessage( hAction, eCasMsgType, eDxCAS_GROUPID_IR, (void*)pEvtData,	eShowType) )
	{
		HxLOG_Print("send message failed\n");
		return HIR_NOT_OK;
	}

	if(eCasMsgType == eCAS_MSG_TYPE_Ok)
	{
		*pStatus = IREVTBW_SVC_CAS_OK;
	}
	else
	{
		*pStatus = IREVTBW_SVC_CAS_FAIL;
	}

	return HIR_OK;
}

HIR_ERR local_cas_ir_EVT_UpdateIRDStatus(Handle_t hAction , HMSG *hMsg)
{
	IREVT_Data	*pEvtData;
	SvcCas_MsgType_e		eMsgType;
	SvcCas_ShowType_e		eShowType;

	HxLOG_Info("hAction = 0x%X, hMsg = 0x%X\n", hAction, (HUINT32)hMsg );

	eMsgType = eCAS_MSG_TYPE_Command;
	eShowType = eCasShowType_Both;

	_nIrEvtDataIndex = (_nIrEvtDataIndex + 1) % MAX_IR_EVT_BUFFER;
	pEvtData = &_stIrEvtData[_nIrEvtDataIndex];

	if(pEvtData == NULL)
		return HIR_OK;

	HxSTD_memset(pEvtData, 0x00, sizeof(IREVT_Data) );

	/* convert HMSG --> Irdeto event */
	/* Later, its direct conversion is needed. */

#if defined(CONFIG_DEBUG)
	//print debug.
	local_cas_ir_EVT_DEBUG_PrintEvtClass(hMsg->usMsgClass);
#endif

	switch(hMsg->usMsgClass)
	{
		case IREVT_CLASS_DECODER_MSG_NORMAL_MAIL:
		case IREVT_CLASS_DECODER_MSG_FORCED_DISPLAY:
			HxLOG_Print("local_cas_ir_EVT_UpdateIRDStatus(IREVT_CLASS_DECODER_MSG_FORCED_DISPLAY) ---- mail idx = 0x%X\n", hMsg->usParam );
			pEvtData->ExtData.ucMail_Index= (HUINT8) hMsg->usParam;
			break;

		case IREVT_CLASS_DECODER_MSG_FORCED_DOWNLOAD:
		case IREVT_CLASS_DECODER_MSG_DOWNLOAD_ALLOWED_ENABLE:
		case IREVT_CLASS_DECODER_MSG_DOWNLOAD_ALLOWED_DISABLE:
			break;

		case IREVT_CLASS_DECODER_MSG_ECM_FINGERPRINT:
			HxLOG_Print("local_cas_ir_EVT_UpdateIRDStatus(IREVT_CLASS_DECODER_MSG_ECM_FINGERPRINT) ---- ecm finger print, slot id = 0x%X\n", hMsg->usParam );
#if defined(CONFIG_MW_CAS_IRDETO_SCELL)
			XSVC_CAS_IR_DRV_CopyScInfo(hMsg->usParam, pEvtData->ExtData.aucHashedCode);
#endif
			break;

		case IREVT_CLASS_DECODER_MSG_CHANGE_PINCODE:
			pEvtData->ExtData.ulPinCodeBCDType=  hMsg->usParam;
			break;

		case IREVT_CLASS_DECODER_MSG_FORCE_TUNE:
			pEvtData->ExtData.stForceTuneParam.usNetworkId =hMsg->usParam;
			pEvtData->ExtData.stForceTuneParam.usTsId = (hMsg->ulParam >> 16 )&0xFFFF;
			pEvtData->ExtData.stForceTuneParam.usSvcId = (hMsg->ulParam )&0xFFFF;
			break;

		case IREVT_CLASS_DECODER_MSG_RESET_CH_ORDER:
			pEvtData->ExtData.stForceTuneParam.usBouquetId =hMsg->usParam;
			break;

		case IREVT_CLASS_DECODER_MSG_RESET_IRD:
		case IREVT_CLASS_DECODER_MSG_RESCAN:
		case IREVT_CLASS_DECODER_MSG_RESCAN_FROM_HOME:
		case IREVT_CLASS_DECODER_MSG_RECONNECT:
			break;

#if defined(IRPLAT_FN_DDEMM_USER_PAYLOAD_FOR_DIGITURK)
		case IREVT_CLASS_DECODER_MSG_CA_VIRTUAL_BILL:
			pEvtData->ExtData.stVirtualBillData.usLength = hMsg->usParam;
			pEvtData->ExtData.stVirtualBillData.pucBillData = (HUINT8 *)hMsg->ulParam;
			break;
		case IREVT_CLASS_DECODER_MSG_CA_DM_PARAMETERS:
			if(((void *)(hMsg->ulParam) != NULL) && (hMsg->usParam != 0))
			{
				VK_MEM_Memcpy((void *)&pEvtData->ExtData.stDMParameters, (void *)hMsg->ulParam, hMsg->usParam);
				OxCAS_Free((void *)hMsg->ulParam);
			}
			break;
		case IREVT_CLASS_DECODER_MSG_CA_FEATURE_BITMAP:
			pEvtData->ExtData.ulCaFeatureBitmap = hMsg->ulParam;
			break;
		case IREVT_CLASS_DECODER_MSG_CA_IPPV_IP_AND_PHONE_NO:
			if(((void *)(hMsg->ulParam) != NULL) && (hMsg->usParam != 0))
			{
				VK_MEM_Memcpy((void *)&pEvtData->ExtData.stIppvCallbackInfo, (void *)hMsg->ulParam, hMsg->usParam);
				OxCAS_Free((void *)hMsg->ulParam);
			}
			break;
		case IREVT_CLASS_DECODER_MSG_CA_IPPV_DEBIT_LIMIT_THRESHOLD:
			pEvtData->ExtData.ucThreshold = (HUINT8)hMsg->ulParam;
			break;
		case IREVT_CLASS_DECODER_MSG_CA_IPPV_SCHEDULE:
			if(((void *)(hMsg->ulParam) != NULL) && (hMsg->usParam != 0))
			{
				VK_MEM_Memcpy((void *)&pEvtData->ExtData.stIppvSchedule, (void *)hMsg->ulParam, hMsg->usParam);
				OxCAS_Free((void *)hMsg->ulParam);
			}
			break;
		case IREVT_CLASS_DECODER_MSG_CA_REPORT_STATUS:
			if(((void *)(hMsg->ulParam) != NULL) && (hMsg->usParam != 0))
			{
				VK_MEM_Memcpy((void *)&pEvtData->ExtData.stDecorderReportStatus, (void *)hMsg->ulParam, hMsg->usParam);
				OxCAS_Free((void *)hMsg->ulParam);
			}
			break;
		case IREVT_CLASS_DECODER_MSG_CA_SET_DOWNLOAD_TYPE:
			if(((void *)(hMsg->ulParam) != NULL) && (hMsg->usParam != 0))
			{
				VK_MEM_Memcpy((void *)&pEvtData->ExtData.stDownloadType, (void *)hMsg->ulParam, hMsg->usParam);
				OxCAS_Free((void *)hMsg->ulParam);
			}
			break;

		case IREVT_CLASS_DECODER_MSG_EPG_DM_SETUP:
			if(((void *)(hMsg->ulParam) != NULL) && (hMsg->usParam != 0))
			{
				VK_MEM_Memcpy((void *)&pEvtData->ExtData.stDMStatus, (void *)hMsg->ulParam, hMsg->usParam);
				OxCAS_Free((void *)hMsg->ulParam);
			}
			break;
		case IREVT_CLASS_DECODER_MSG_EPG_FORCED_STANDBY:
			break;
		case IREVT_CLASS_DECODER_MSG_EPG_DISPLAY_CARD_NUMBER:
			if(((void *)(hMsg->ulParam) != NULL) && (hMsg->usParam != 0))
			{
				VK_MEM_Memcpy((void *)&pEvtData->ExtData.stDisplayCardNo, (void *)hMsg->ulParam, hMsg->usParam);
				OxCAS_Free((void *)hMsg->ulParam);
			}
			break;
		case IREVT_CLASS_DECODER_MSG_EPG_REMOTE_TAG:
			if(((void *)(hMsg->ulParam) != NULL) && (hMsg->usParam != 0))
			{
				VK_MEM_Memcpy((void *)&pEvtData->ExtData.stRemoteTagging, (void *)hMsg->ulParam, hMsg->usParam);
				OxCAS_Free((void *)hMsg->ulParam);
			}
			break;
		case IREVT_CLASS_DECODER_MSG_EPG_REMOTE_RECORD:
			if(((void *)(hMsg->ulParam) != NULL) && (hMsg->usParam != 0))
			{
				VK_MEM_Memcpy((void *)&pEvtData->ExtData.stRemoteRecording, (void *)hMsg->ulParam, hMsg->usParam);
				OxCAS_Free((void *)hMsg->ulParam);
			}
			break;
		case IREVT_CLASS_DECODER_MSG_EPG_DEL_PVOD_EVENT:
			if(((void *)(hMsg->ulParam) != NULL) && (hMsg->usParam != 0))
			{
				VK_MEM_Memcpy((void *)&pEvtData->ExtData.stDelPushVodEvent, (void *)hMsg->ulParam, hMsg->usParam);
				OxCAS_Free((void *)hMsg->ulParam);
			}
			break;
		case IREVT_CLASS_DECODER_MSG_EPG_CLEAR_3PA_DATA:
			if(((void *)(hMsg->ulParam) != NULL) && (hMsg->usParam != 0))
			{
				VK_MEM_Memcpy((void *)&pEvtData->ExtData.stClear3PaData, (void *)hMsg->ulParam, hMsg->usParam);
				OxCAS_Free((void *)hMsg->ulParam);
			}
			break;
		case IREVT_CLASS_DECODER_MSG_EPG_COLOUR_DISPLAY:
			if(((void *)(hMsg->ulParam) != NULL) && (hMsg->usParam != 0))
			{
				VK_MEM_Memcpy((void *)&pEvtData->ExtData.stColourDisplayData, (void *)hMsg->ulParam, hMsg->usParam);
				OxCAS_Free((void *)hMsg->ulParam);
			}
			break;
		case IREVT_CLASS_DECODER_MSG_EPG_CS_PHONE_NO:
			if(((void *)(hMsg->ulParam) != NULL) && (hMsg->usParam != 0))
			{
				VK_MEM_Memcpy((void *)&pEvtData->ExtData.stCSPhoneNo, (void *)hMsg->ulParam, hMsg->usParam);
				OxCAS_Free((void *)hMsg->ulParam);
			}
			break;
		case IREVT_CLASS_DECODER_MSG_EPG_PPV_PHONE_NO:
			if(((void *)(hMsg->ulParam) != NULL) && (hMsg->usParam != 0))
			{
				VK_MEM_Memcpy((void *)&pEvtData->ExtData.stPpvPhoneNo, (void *)hMsg->ulParam, hMsg->usParam);
				OxCAS_Free((void *)hMsg->ulParam);
			}
			break;
		case IREVT_CLASS_DECODER_MSG_EPG_UPDATE_HDD:
			break;
		case IREVT_CLASS_DECODER_MSG_EPG_FORMAT_HDD:
			break;
		case IREVT_CLASS_DECODER_MSG_EPG_EPG_FEATURES:
			pEvtData->ExtData.ulEpgFeatureBitmap = hMsg->ulParam;
			break;
		case IREVT_CLASS_DECODER_MSG_EPG_USER_FOLDER_SETTINGS:
			pEvtData->ExtData.usUserFolderSize = hMsg->ulParam;
			break;

		case IREVT_CLASS_DECODER_MSG_EPG_SET_OTHER_MODE:
			if(((void *)(hMsg->ulParam) != NULL) && (hMsg->usParam != 0))
			{
				VK_MEM_Memcpy((void *)&pEvtData->ExtData.stSetOtherMode, (void *)hMsg->ulParam, hMsg->usParam);
				OxCAS_Free((void *)hMsg->ulParam);
			}
			break;
		case IREVT_CLASS_DECODER_MSG_EPG_EASY_SMS_DISPLAY:
			if(((void *)(hMsg->ulParam) != NULL) && (hMsg->usParam != 0))
			{
				VK_MEM_Memcpy((void *)&pEvtData->ExtData.stEasySmsDisplayData, (void *)hMsg->ulParam, hMsg->usParam);
				OxCAS_Free((void *)hMsg->ulParam);
			}
			break;
#if 0
		case IREVT_CLASS_DECODER_MSG_EPG_REMOTE_TAG_DS265:
			break;
#endif
		case IREVT_CLASS_DECODER_MSG_EPG_DIGIAKTIF:
			if(((void *)(hMsg->ulParam) != NULL) && (hMsg->usParam != 0))
			{
				VK_MEM_Memcpy((void *)&pEvtData->ExtData.stDigiaktifTagging, (void *)hMsg->ulParam, hMsg->usParam);
				OxCAS_Free((void *)hMsg->ulParam);
			}
			break;
		case IREVT_CLASS_DECODER_MSG_EPG_SET_FTA_LIMIT:
			if(((void *)(hMsg->ulParam) != NULL) && (hMsg->usParam != 0))			{
				VK_MEM_Memcpy((void *)&pEvtData->ExtData.stFtaLimit, (void *)hMsg->ulParam, hMsg->usParam);
				OxCAS_Free((void *)hMsg->ulParam);
			}
			break;
#endif

#if defined(IRPLAT_FN_DDEMM_USER_PAYLOAD_FOR_UPC)
		case IREVT_CLASS_DECODER_MSG_EPG_CS_PHONE_NO:
			if(((void *)(hMsg->ulParam) != NULL) && (hMsg->usParam != 0))
			{
				VK_MEM_Memcpy((void *)&pEvtData->ExtData.stCSPhoneNo, (void *)hMsg->ulParam, hMsg->usParam);
				OxCAS_Free((void *)hMsg->ulParam);
			}
			break;

		case IREVT_CLASS_DECODER_MSG_USAGE_ID:
			if( hMsg->usParam != 0 )
			{
				pEvtData->ExtData.ucUsageId = hMsg->ulParam;
			}
			break;
#endif
		//following are to send just msg class.
		case IREVT_CLASS_SC_NATIONALITY_CHANGED_BY_EMM:
		case IREVT_CLASS_ECM_MONITOR_UPDATE:
		case IREVT_CLASS_EMM_MONITOR_UPDATE:
#if defined(CONFIG_MW_CAS_IRDETO_PREVIEW)
		case IREVT_CLASS_ECM_MONITOR_PREVIEW_TIME:
#endif

#if	defined(IRPLAT_SYS_PVR)
		case IREVT_CLASS_DECODER_MSG_PVR:

#endif
#if defined(CONFIG_MW_CAS_IRDETO_SOL)
		case IREVT_CLASS_SOL_STATUS_CHANGED:
#endif
		case IREVT_CLASS_FTA_BLOCK_STATUS_CHANGED:

#if defined(CONFIG_MW_CAS_IRDETO_HOMINGCHANNEL)
		case IREVT_CLASS_HOMING_CH_UPDATE:
#endif
#if	defined(CONFIG_MW_CAS_IRDETO_MATURITY_RATING)
		case IREVT_CLASS_MR_PIN_STAUS_UPDATE:
		case IREVT_CLASS_MR_PF_STAUS_UPDATE:
#endif
			pEvtData->ExtData.ulErrIndex= hMsg->usParam;

			break;

#if	defined(CONFIG_MW_CAS_IRDETO_MATURITY_RATING)
		case IREVT_CLASS_MR_SC_CAPABLITY_UPDATE:
			break;
#endif
#if 0
		case IREVT_CLASS_SOFTCELL_MENU_NEED_TO_UPDATE:
			break;
#endif
		default :
			HxLOG_Print("Not yet defined(0x%X)\n", hMsg->usMsgClass);
			HxLOG_Critical("\n\n");
			return HIR_NOT_OK;
	}

	pEvtData->eIrEvtClass = hMsg->usMsgClass;

	svc_cas_MgrSubUpdateCasMessage( hAction, \
								eMsgType, \
								eDxCAS_GROUPID_IR, \
								(void *)pEvtData, \
								eShowType);

	return HIR_OK;
}


/* See, It don't need mem allocation for hMsg */
HINT32 CAS_IR_EVT_SendEvent(Handle_t hAction, HMSG *hMsg)
{
	HxLOG_Info("\n");
#if defined(CONFIG_DEBUG)
	local_cas_ir_EVT_DEBUG_PrintEvtClass(hMsg->usMsgClass);
#endif
	switch(hMsg->usMsgClass)
	{
	case IREVT_CLASS_SVC_DESCRAMBLE_START:
		local_cas_ir_EVT_UpdateSvcStatus(IREVTBW_SVC_INIT, hAction, hMsg);
		break;
	case IREVT_CLASS_SVC_DESCRAMBLE_OPEN:
		local_cas_ir_EVT_UpdateSvcStatus(IREVTBW_SVC_OPENED, hAction, hMsg);
		break;
	case IREVT_CLASS_SVC_DESCRAMBLE_DEFINED:
		local_cas_ir_EVT_UpdateSvcStatus(IREVTBW_SVC_DEFINED, hAction, hMsg);
		break;

	case IREVT_CLASS_CAS_DESCRAMBLED_WELL:  //E29 - D29-4
	case IREVT_CLASS_CAS_FTA_SERVICE: //D100-8
	case IREVT_CLASS_CAS_ERROR_FOR_SERVICE:
	case IREVT_CLASS_CAS_ERROR_FOR_GLOBAL:
		local_cas_ir_EVT_UpdateSvcStatus(IREVTBW_SVC_GLOBAL_STATUS, hAction, hMsg);
		break;

	case IREVT_CLASS_DECODER_MSG_NORMAL_MAIL:
	case IREVT_CLASS_DECODER_MSG_FORCED_DISPLAY:

	case IREVT_CLASS_DECODER_MSG_FORCED_DOWNLOAD:
	case IREVT_CLASS_DECODER_MSG_DOWNLOAD_ALLOWED_ENABLE:
	case IREVT_CLASS_DECODER_MSG_DOWNLOAD_ALLOWED_DISABLE:
	case IREVT_CLASS_DECODER_MSG_CHANGE_PINCODE:
	case IREVT_CLASS_DECODER_MSG_FORCE_TUNE:
	case IREVT_CLASS_DECODER_MSG_RESET_IRD:
	case IREVT_CLASS_DECODER_MSG_RESET_CH_ORDER:
	case IREVT_CLASS_DECODER_MSG_RESCAN:
	case IREVT_CLASS_DECODER_MSG_RESCAN_FROM_HOME:
	case IREVT_CLASS_DECODER_MSG_RECONNECT:
	case IREVT_CLASS_DECODER_MSG_ECM_FINGERPRINT:
	case IREVT_CLASS_DECODER_MSG_PVR:
#if defined(IRPLAT_FN_DDEMM_USER_PAYLOAD_FOR_DIGITURK)
	case IREVT_CLASS_DECODER_MSG_CA_VIRTUAL_BILL:
	case IREVT_CLASS_DECODER_MSG_CA_DM_PARAMETERS:
	case IREVT_CLASS_DECODER_MSG_CA_FEATURE_BITMAP:
	case IREVT_CLASS_DECODER_MSG_CA_IPPV_IP_AND_PHONE_NO:
	case IREVT_CLASS_DECODER_MSG_CA_IPPV_DEBIT_LIMIT_THRESHOLD:
	case IREVT_CLASS_DECODER_MSG_CA_IPPV_SCHEDULE:
	case IREVT_CLASS_DECODER_MSG_CA_REPORT_STATUS:
	case IREVT_CLASS_DECODER_MSG_CA_SET_DOWNLOAD_TYPE:

	case IREVT_CLASS_DECODER_MSG_EPG_DM_SETUP:
	case IREVT_CLASS_DECODER_MSG_EPG_FORCED_STANDBY:
	case IREVT_CLASS_DECODER_MSG_EPG_DISPLAY_CARD_NUMBER:
	case IREVT_CLASS_DECODER_MSG_EPG_REMOTE_TAG:
	case IREVT_CLASS_DECODER_MSG_EPG_REMOTE_RECORD:
	case IREVT_CLASS_DECODER_MSG_EPG_DEL_PVOD_EVENT:
	case IREVT_CLASS_DECODER_MSG_EPG_CLEAR_3PA_DATA:
	case IREVT_CLASS_DECODER_MSG_EPG_COLOUR_DISPLAY:
	case IREVT_CLASS_DECODER_MSG_EPG_CS_PHONE_NO:
	case IREVT_CLASS_DECODER_MSG_EPG_PPV_PHONE_NO:
	case IREVT_CLASS_DECODER_MSG_EPG_UPDATE_HDD:
	case IREVT_CLASS_DECODER_MSG_EPG_FORMAT_HDD:
	case IREVT_CLASS_DECODER_MSG_EPG_EPG_FEATURES:
	case IREVT_CLASS_DECODER_MSG_EPG_USER_FOLDER_SETTINGS:

	case IREVT_CLASS_DECODER_MSG_EPG_SET_OTHER_MODE:
	case IREVT_CLASS_DECODER_MSG_EPG_EASY_SMS_DISPLAY:
	case IREVT_CLASS_DECODER_MSG_EPG_REMOTE_TAG_DS265:
	case IREVT_CLASS_DECODER_MSG_EPG_DIGIAKTIF:
	case IREVT_CLASS_DECODER_MSG_EPG_SET_FTA_LIMIT:
#endif
#if defined(IRPLAT_FN_DDEMM_USER_PAYLOAD_FOR_UPC)
	case IREVT_CLASS_DECODER_MSG_EPG_CS_PHONE_NO:
	case IREVT_CLASS_DECODER_MSG_USAGE_ID:
#endif
	case IREVT_CLASS_ECM_MONITOR_UPDATE:
	case IREVT_CLASS_EMM_MONITOR_UPDATE:
#if defined(CONFIG_MW_CAS_IRDETO_PREVIEW)
	case IREVT_CLASS_ECM_MONITOR_PREVIEW_TIME:
#endif

	case IREVT_CLASS_SC_NATIONALITY_CHANGED_BY_EMM:
#if defined(CONFIG_MW_CAS_IRDETO_HOMINGCHANNEL)
	case IREVT_CLASS_HOMING_CH_UPDATE:
#endif

		local_cas_ir_EVT_UpdateIRDStatus( hAction, hMsg );
		break;

#if defined(CONFIG_MW_CAS_IRDETO_SOL)
	case IREVT_CLASS_SOL_STATUS_CHANGED:
		HxLOG_Print("SendEvttoExtApp  IREVT_CLASS_SOL_STATUS_CHANGED\n");
		local_cas_ir_EVT_UpdateIRDStatus(hAction, hMsg);
		break;
#endif

	case IREVT_CLASS_FTA_BLOCK_STATUS_CHANGED:
		HxLOG_Print("SendEvttoExtApp  IREVT_CLASS_FTA_BLOCK_STATUS_CHANGED\n");
		local_cas_ir_EVT_UpdateIRDStatus(hAction, hMsg);
		break;

#if defined(CONFIG_MW_CAS_IRDETO_MATURITY_RATING) && !defined(CONFIG_MW_CAS_IRDETO_UCAS)
	case IREVT_CLASS_MR_CONFIG_UPDATE:
		HxLOG_Print("SendEvttoExtApp  IREVT_CLASS_MR_CONFIG_UPDATE(result= %d)\n", hMsg->usParam);
		/* if hMsg->usParam doesn't have  0 , it shall be processed as error , valid range is 0 to 5 */
		//IR_SendEvtToApp(IR_EVT_MRConfigStatus_Changed, hMsg->usParam, 0);
		break;
	case IREVT_CLASS_MR_PIN_STAUS_UPDATE:
		HxLOG_Print("SendEvttoExtApp  IREVT_CLASS_MR_PIN_STAUS_UPDATE(NEED PIN:%s)\n",(hMsg->usParam>1)? "YES":"NO");
		local_cas_ir_EVT_UpdateIRDStatus( hAction, hMsg );
		break;
	case IREVT_CLASS_MR_PF_STAUS_UPDATE:
		HxLOG_Print("SendEvttoExtApp  IREVT_CLASS_MR_PF_STAUS_UPDATE(result= %d)\n", hMsg->usParam);
		/* if hMsg->usParam doesn't have  0 or 7, it shall be processed as error  */
		local_cas_ir_EVT_UpdateIRDStatus( hAction, hMsg );
		break;
	case IREVT_CLASS_MR_SC_CAPABLITY_UPDATE:
		HxLOG_Print("SendEvttoExtApp  IREVT_CLASS_MR_SC_CAPABLITY_UPDATE\n");
		local_cas_ir_EVT_UpdateIRDStatus( hAction, hMsg );
		break;
#endif
	default:
		HxLOG_Print("CAS_IR_EVT_SendEvent: Msg class(0x%04X) is not defined!!!\n",hMsg->usMsgClass );
		break;
	}

	return ERR_OK;
}

#if defined(CONFIG_DEBUG)
void local_cas_ir_EVT_DEBUG_PrintEvtClass(HUINT16 msg_class)
{
	if(msg_class < IREVT_CLASS_DECODER_MSG_ENHANCED_TEXT)
	{
		HxLOG_Print("IREVT_UPDATE: SVC : ");
	}
	else
	{
		HxLOG_Print("IREVT_UPDATE: IRD : ");
	}

	switch(msg_class)
	{

	case IREVT_CLASS_SVC_DESCRAMBLE_START  :
		HxLOG_Print("IREVT_CLASS_SVC_DESCRAMBLE_START  \n");	break;
	case IREVT_CLASS_SVC_DESCRAMBLE_OPEN:
		HxLOG_Print("IREVT_CLASS_SVC_DESCRAMBLE_OPEN\n"); break;
	case IREVT_CLASS_SVC_DESCRAMBLE_DEFINED:
		HxLOG_Print("IREVT_CLASS_SVC_DESCRAMBLE_DEFINED\n"); break;
// Scrambel/Descramble 관련
	case IREVT_CLASS_CAS_ERROR_FOR_SERVICE:
		HxLOG_Print("IREVT_CLASS_CAS_ERROR_FOR_SERVICE\n"); break;
	case IREVT_CLASS_CAS_ERROR_FOR_GLOBAL:
		HxLOG_Print("IREVT_CLASS_CAS_ERROR_FOR_GLOBAL\n"); break;
	case IREVT_CLASS_CAS_DESCRAMBLED_WELL:
		HxLOG_Print("IREVT_CLASS_CAS_DESCRAMBLED_WELL\n"); break;
	case IREVT_CLASS_CAS_FTA_SERVICE:
		HxLOG_Print("IREVT_CLASS_CAS_FTA_SERVICE\n"); break;

	case IREVT_CLASS_DECODER_MSG_ENHANCED_TEXT:
		HxLOG_Print("IREVT_CLASS_DECODER_MSG_ENHANCED_TEXT\n"); break;
	case IREVT_CLASS_DECODER_MSG_ECM_FINGERPRINT:
		HxLOG_Print("IREVT_CLASS_DECODER_MSG_ECM_FINGERPRINT\n"); break;
	case IREVT_CLASS_DECODER_MSG_EMM_FINGERPRINT:
		HxLOG_Print("IREVT_CLASS_DECODER_MSG_EMM_FINGERPRINT\n"); break;
	case IREVT_CLASS_DECODER_MSG_FORCED_DOWNLOAD:
		HxLOG_Print("IREVT_CLASS_DECODER_MSG_FORCED_DOWNLOAD\n"); break;
	case IREVT_CLASS_DECODER_MSG_DOWNLOAD_ALLOWED_ENABLE:
		HxLOG_Print("IREVT_CLASS_DECODER_MSG_DOWNLOAD_ALLOWED_ENABLE\n"); break;
	case IREVT_CLASS_DECODER_MSG_DOWNLOAD_ALLOWED_DISABLE:
		HxLOG_Print("IREVT_CLASS_DECODER_MSG_DOWNLOAD_ALLOWED_DISABLE\n"); break;

	case IREVT_CLASS_DECODER_MSG_CHANGE_PINCODE:
		HxLOG_Print("IREVT_CLASS_DECODER_MSG_CHANGE_PINCODE\n"); break;
	case IREVT_CLASS_DECODER_MSG_NORMAL_MAIL:
		HxLOG_Print("IREVT_CLASS_DECODER_MSG_NORMAL_MAIL\n"); break;
	case IREVT_CLASS_DECODER_MSG_FORCED_DISPLAY:
		HxLOG_Print("IREVT_CLASS_DECODER_MSG_FORCED_DISPLAY\n"); break;
	case IREVT_CLASS_DECODER_MSG_MENU_UPDATE:
		HxLOG_Print("IREVT_CLASS_DECODER_MSG_MENU_UPDATE\n"); break;
	case IREVT_CLASS_DECODER_MSG_MENU_STATE:
		HxLOG_Print("IREVT_CLASS_DECODER_MSG_MENU_STATE\n"); break;
	case IREVT_CLASS_DECODER_MSG_FORCE_TUNE:
		HxLOG_Print("IREVT_CLASS_DECODER_MSG_FORCE_TUNE\n"); break;
	case IREVT_CLASS_DECODER_MSG_RESET_IRD:
		HxLOG_Print("IREVT_CLASS_DECODER_MSG_RESET_IRD\n"); break;
	case IREVT_CLASS_DECODER_MSG_RESET_CH_ORDER:
		HxLOG_Print("IREVT_CLASS_DECODER_MSG_RESET_CH_ORDER\n"); break;
	case IREVT_CLASS_DECODER_MSG_RESCAN:
		HxLOG_Print("IREVT_CLASS_DECODER_MSG_RESCAN\n"); break;
	case IREVT_CLASS_DECODER_MSG_RESCAN_FROM_HOME:
		HxLOG_Print("IREVT_CLASS_DECODER_MSG_RESCAN_FROM_HOME\n"); break;
	case IREVT_CLASS_DECODER_MSG_RECONNECT:
		HxLOG_Print("IREVT_CLASS_DECODER_MSG_RECONNECT\n"); break;
#if defined(IRPLAT_FN_DDEMM_USER_PAYLOAD_FOR_DIGITURK)
	case IREVT_CLASS_DECODER_MSG_CA_VIRTUAL_BILL:
		HxLOG_Print("IREVT_CLASS_DECODER_MSG_CA_VIRTUAL_BILL\n");
		break;
	case IREVT_CLASS_DECODER_MSG_CA_DM_PARAMETERS:
		HxLOG_Print("IREVT_CLASS_DECODER_MSG_CA_DM_PARAMETERS\n");
		break;
	case IREVT_CLASS_DECODER_MSG_CA_FEATURE_BITMAP:
		HxLOG_Print("IREVT_CLASS_DECODER_MSG_CA_FEATURE_BITMAP\n");
		break;
	case IREVT_CLASS_DECODER_MSG_CA_IPPV_IP_AND_PHONE_NO:
		HxLOG_Print("IREVT_CLASS_DECODER_MSG_CA_IPPV_IP_AND_PHONE_NO\n");
		break;
	case IREVT_CLASS_DECODER_MSG_CA_IPPV_DEBIT_LIMIT_THRESHOLD:
		HxLOG_Print("IREVT_CLASS_DECODER_MSG_CA_IPPV_DEBIT_LIMIT_THRESHOLD\n");
		break;
	case IREVT_CLASS_DECODER_MSG_CA_IPPV_SCHEDULE:
		HxLOG_Print("IREVT_CLASS_DECODER_MSG_CA_IPPV_SCHEDULE\n");
		break;
	case IREVT_CLASS_DECODER_MSG_CA_REPORT_STATUS:
		HxLOG_Print("IREVT_CLASS_DECODER_MSG_CA_REPORT_STATUS\n");
		break;
	case IREVT_CLASS_DECODER_MSG_CA_SET_DOWNLOAD_TYPE:
		HxLOG_Print("IREVT_CLASS_DECODER_MSG_CA_SET_DOWNLOAD_TYPE\n");
		break;

	case IREVT_CLASS_DECODER_MSG_EPG_DM_SETUP:
		HxLOG_Print("IREVT_CLASS_DECODER_MSG_CA_SET_DOWNLOAD_TYPE\n");
		break;
	case IREVT_CLASS_DECODER_MSG_EPG_FORCED_STANDBY:
		HxLOG_Print("IREVT_CLASS_DECODER_MSG_EPG_FORCED_STANDBY\n");
		break;
	case IREVT_CLASS_DECODER_MSG_EPG_DISPLAY_CARD_NUMBER:
		HxLOG_Print("IREVT_CLASS_DECODER_MSG_EPG_DISPLAY_CARD_NUMBER\n");
		break;
	case IREVT_CLASS_DECODER_MSG_EPG_REMOTE_TAG:
		HxLOG_Print("IREVT_CLASS_DECODER_MSG_EPG_REMOTE_TAG\n");
		break;
	case IREVT_CLASS_DECODER_MSG_EPG_REMOTE_RECORD:
		HxLOG_Print("IREVT_CLASS_DECODER_MSG_EPG_REMOTE_RECORD\n");
		break;
	case IREVT_CLASS_DECODER_MSG_EPG_DEL_PVOD_EVENT:
		HxLOG_Print("IREVT_CLASS_DECODER_MSG_EPG_DEL_PVOD_EVENT\n");
		break;
	case IREVT_CLASS_DECODER_MSG_EPG_CLEAR_3PA_DATA:
		HxLOG_Print("IREVT_CLASS_DECODER_MSG_EPG_CLEAR_3PA_DATA\n");
		break;
	case IREVT_CLASS_DECODER_MSG_EPG_COLOUR_DISPLAY:
		HxLOG_Print("IREVT_CLASS_DECODER_MSG_EPG_COLOUR_DISPLAY\n");
		break;
	case IREVT_CLASS_DECODER_MSG_EPG_CS_PHONE_NO:
		HxLOG_Print("IREVT_CLASS_DECODER_MSG_EPG_CS_PHONE_NO\n");
		break;
	case IREVT_CLASS_DECODER_MSG_EPG_PPV_PHONE_NO:
		HxLOG_Print("IREVT_CLASS_DECODER_MSG_EPG_PPV_PHONE_NO\n");
		break;
	case IREVT_CLASS_DECODER_MSG_EPG_UPDATE_HDD:
		HxLOG_Print("IREVT_CLASS_DECODER_MSG_EPG_UPDATE_HDD\n");
		break;
	case IREVT_CLASS_DECODER_MSG_EPG_FORMAT_HDD:
		HxLOG_Print("IREVT_CLASS_DECODER_MSG_EPG_FORMAT_HDD\n");
		break;
	case IREVT_CLASS_DECODER_MSG_EPG_EPG_FEATURES:
		HxLOG_Print("IREVT_CLASS_DECODER_MSG_EPG_EPG_FEATURES\n");
		break;
	case IREVT_CLASS_DECODER_MSG_EPG_USER_FOLDER_SETTINGS:
		HxLOG_Print("IREVT_CLASS_DECODER_MSG_EPG_USER_FOLDER_SETTINGS\n");
		break;

	case IREVT_CLASS_DECODER_MSG_EPG_SET_OTHER_MODE:
		HxLOG_Print("IREVT_CLASS_DECODER_MSG_EPG_SET_OTHER_MODE\n");
		break;
	case IREVT_CLASS_DECODER_MSG_EPG_EASY_SMS_DISPLAY:
		HxLOG_Print("IREVT_CLASS_DECODER_MSG_EPG_EASY_SMS_DISPLAY\n");
		break;
	case IREVT_CLASS_DECODER_MSG_EPG_REMOTE_TAG_DS265:
		HxLOG_Print("IREVT_CLASS_DECODER_MSG_EPG_REMOTE_TAG_DS265\n");
		break;
	case IREVT_CLASS_DECODER_MSG_EPG_DIGIAKTIF:
		HxLOG_Print("IREVT_CLASS_DECODER_MSG_EPG_DIGIAKTIF\n");
		break;
	case IREVT_CLASS_DECODER_MSG_EPG_SET_FTA_LIMIT:
		HxLOG_Print("IREVT_CLASS_DECODER_MSG_EPG_SET_FTA_LIMIT\n");
		break;
#endif
#if defined(IRPLAT_FN_DDEMM_USER_PAYLOAD_FOR_UPC)
	case IREVT_CLASS_DECODER_MSG_EPG_CS_PHONE_NO:
		HxLOG_Print("IREVT_CLASS_DECODER_MSG_EPG_CS_PHONE_NO\n");
		break;
	case IREVT_CLASS_DECODER_MSG_USAGE_ID:
		HxLOG_Print("IREVT_CLASS_DECODER_MSG_USAGE_ID\n");
		break;
#endif
	case IREVT_CLASS_DECODER_MSG_PVR:
		HxLOG_Print("IREVT_CLASS_DECODER_MSG_PVR\n"); break;

	case IREVT_CLASS_SOFTCELL_MENU_NEED_TO_UPDATE :
		HxLOG_Print("IREVT_CLASS_SOFTCELL_MENU_NEED_TO_UPDATE\n"); break;
	case IREVT_CLASS_SMARTCARD_MENU_NEED_TO_UPDATE:
		HxLOG_Print("IREVT_CLASS_SMARTCARD_MENU_NEED_TO_UPDATE\n"); break;

	case IREVT_CLASS_SC_NATIONALITY_CHANGED_BY_EMM :
		HxLOG_Print("IREVT_CLASS_SC_NATIONALITY_CHANGED_BY_EMM\n"); break;
	case IREVT_CLASS_SC_NATIONALITY_CHANGED_BY_SC_OUT:
		HxLOG_Print("IREVT_CLASS_SC_NATIONALITY_CHANGED_BY_SC_OUT\n"); break;

	case IREVT_CLASS_SOL_STATUS_CHANGED :
		HxLOG_Print("IREVT_CLASS_SOL_STATUS_CHANGED\n"); break;

#if defined(CONFIG_MW_CAS_IRDETO_MATURITY_RATING)
	case IREVT_CLASS_MR_CONFIG_UPDATE :
		HxLOG_Print("IREVT_CLASS_MR_CONFIG_UPDATE\n"); break;
	case IREVT_CLASS_MR_PIN_STAUS_UPDATE:// to triggering pin window regarding Maturity rating
		HxLOG_Print("IREVT_CLASS_MR_PIN_STAUS_UPDATE\n"); break;// to triggering pin window regarding Maturity rating
	case IREVT_CLASS_MR_PF_STAUS_UPDATE: // to triggering pin window regarding Maturity rating
		HxLOG_Print("IREVT_CLASS_MR_PF_STAUS_UPDATE\n"); break; // to triggering pin window regarding Maturity rating
	case IREVT_CLASS_MR_SC_CAPABLITY_UPDATE:
		HxLOG_Print("IREVT_CLASS_MR_SC_CAPABLITY_UPDATE\n"); break;
#endif

	case IREVT_CLASS_ECM_MONITOR_UPDATE:
		HxLOG_Print("IREVT_CLASS_ECM_MONITOR_UPDATE\n"); break;
	case IREVT_CLASS_EMM_MONITOR_UPDATE:
		HxLOG_Print("IREVT_CLASS_EMM_MONITOR_UPDATE\n"); break;
	default:
		HxLOG_Print("Unkown Irdeto Event message (0x%X)\n",msg_class );
		break;
	}

}
#endif

