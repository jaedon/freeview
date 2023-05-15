/**
	@file     _svc_cas_mgr_main.c
	@brief    _svc_cas_mgr_main.c (CAS Service)

	Description:
	Module:  \n
	Remarks :\n

	Copyright (c) 2012 HUMAX Co., Ltd. \n
	All rights reserved. \n
*/

/********************************************************************
* Header Files
********************************************************************/
#define ___01_Header_Files___

#include <octo_common.h>

#include <pal_ci.h>

#include <svc_cas.h>

#include <_svc_cas_mgr_common.h>
#include <_svc_cas_mgr_main.h>
#include <_svc_cas_mgr_svc_port.h>
#include <_svc_cas_mgr_pal_port.h>
#include <_svc_cas_mgr_debug.h>

#include <_svc_cas_mgr_instance.h>
#include <_svc_cas_mgr_si.h>
#include <_svc_cas_mgr_scenario.h>

#include <_svc_cas_sub_svc_api.h>

#include <psvc_cas.h>


/* �Ʒ� �κ��� layer�� �µ��� �����Ǿ�� �� �κ�... */

#if defined(CONFIG_MW_CAS_NAGRA_OP_UPC)
#include <namgr_main.h>
#endif




/********************************************************************
* For Debugging
********************************************************************/
#define ___02_Debugging___


/********************************************************************
* Local Definitions : #define / enum / struct / union
********************************************************************/
#define ___03_Local_Definitions___


/********************************************************************
* Extern Variables / Function Prototypes
********************************************************************/
#define ___04_Extern_Vars_Func_Prototypes___

/********************************************************************
* Static Variables / Function Prototypes
********************************************************************/
#define ___05_Local_Vars_Func_Prototypes___

STATIC HULONG			s_ulCasMgrMsgQId;
STATIC HULONG			s_ulCasMgrTaskId;

STATIC HULONG			s_ulCasMgrSyncSema;						/* stop session ó���ÿ��� ��� */

//Action Context
STATIC SvcCas_Context_t s_stCasActionContext[CAS_MAX_NUM_OF_ACTION];

STATIC void svc_cas_MgrUpdateVideoStateCb(HUINT32 ulDevId, SvcCas_VideoDevEvent_e eEvt);
STATIC void svc_cas_MgrUpdateAudioStateCb(SvcCas_AudioDevEvent_e eEvt);
STATIC HERROR svc_cas_MgrSetPid(Handle_t hAction, SvcCas_ActionType_e eActType, SvcCas_SetPidInfo_t *pstCaDscInfo);

#if defined(CONFIG_DEBUG)
STATIC const HINT8 *svc_cas_MgrMakeStrChType(SvcCas_ChannelType_e eType);
STATIC const HINT8 *svc_cas_MgrMakeStrCasMsgType(SvcCas_MsgType_e cmd);
STATIC const HINT8 *svc_cas_MgrMakeStrCasId(DxCAS_GroupId_e cmd);
STATIC const HINT8 *svc_cas_MgrMakeStrCasInstanceType(SvcCas_InstanceType_e eType);
STATIC const HINT8 *svc_cas_MgrMakeStrCamEvtType(SvcCas_DevEvtType_e eType);
STATIC const HINT8 *svc_cas_MgrMakeStrSiTblState(SvcCas_SiTableState_e eState);
// TODO: swlee3 should be deleted
#if defined(CONFIG_MW_CAS_NAGRA_OP_SES_CAKTEST)
STATIC void svc_cas_PrintMgrContextInfo(HUINT32 i, SvcCas_Context_t	*pstContext, HBOOL bVideoDecoding, HUINT32 ulCaInstanceId, DxCAS_GroupId_e eCasGroupId);
#endif
#if defined(CONFIG_MW_CI)
STATIC const HINT8 *svc_cas_MgrMakeStrCamState(SvcCas_CamState_e eState);
#endif
STATIC const HINT8 *svc_cas_MgrMakeStrActionType(SvcCas_ActionType_e cmd);
STATIC HERROR svc_cas_MgrPrintPmtInfo(SvcCas_PmtInfo_t *pstPmtInfo);
STATIC void svc_cas_MgrPrintActionContextList(void);
#endif

/********************************************************************
* Static Functions
* Naming rule : "local"_"CasMgrMain" _ "Function Name"
********************************************************************/
#define ___06_Local_APIs___

STATIC HERROR svc_cas_MgrInitActionContextByActionId(HUINT32 ulActionId)
{
	HUINT32 i;

	if(ulActionId >= CAS_MAX_NUM_OF_ACTION)
	{
		HxLOG_Assert(0);
		return ERR_FAIL;
	}

	svc_cas_SiFreePmtInfo(&s_stCasActionContext[ulActionId].stPmtInfo);

	VK_memset32(&s_stCasActionContext[ulActionId], 0, sizeof(SvcCas_Context_t));

	// ���Ͽ��� SvcCas_Context_t ��� �߿��� 0 �� �ƴ� ������ �ʱ�ȭ�� �Ǵ� �͵鸸 ó���� �ֵ��� �Ѵ�.
	s_stCasActionContext[ulActionId].bConnect = CAS_INST_DISCONNECT;
	s_stCasActionContext[ulActionId].state = eCAS_State_Idle;

// Action�� ����.
	s_stCasActionContext[ulActionId].ulActionId = HANDLE_NULL;
	s_stCasActionContext[ulActionId].hAction = HANDLE_NULL;
	s_stCasActionContext[ulActionId].eActType = eCAS_ActionType_None;
	s_stCasActionContext[ulActionId].eSiSpec = eSIxSPEC_DVB;
	s_stCasActionContext[ulActionId].eTextEnc = eSIxTEXT_ENC_DEFAULT;

//serviced�� ����
	s_stCasActionContext[ulActionId].hSvc = HANDLE_NULL;
	s_stCasActionContext[ulActionId].hNextSvc = HANDLE_NULL;
	s_stCasActionContext[ulActionId].eChType = eCasChannel_Unknown;

	for(i=0; i<CAS_MAX_NUM_OF_ES; i++)
	{
		s_stCasActionContext[ulActionId].stCaDscInfo.stPidInfo[i].usPid = PID_NULL;
	}

	s_stCasActionContext[ulActionId].ulDemuxId = RxRSCID_NULL;

	s_stCasActionContext[ulActionId].ulCasInstanceId = HANDLE_NULL;

/* SI */
	s_stCasActionContext[ulActionId].ulPatFilterId = CAS_FILTER_ID_NULL;
	s_stCasActionContext[ulActionId].ePatState	= eCASSI_Table_Waiting;

	s_stCasActionContext[ulActionId].usPmtPid = PID_NULL;
	s_stCasActionContext[ulActionId].ulPmtFilterId = CAS_FILTER_ID_NULL;
	s_stCasActionContext[ulActionId].ePmtState = eCASSI_Table_Waiting;

#if defined (CAS_SUPPORT_CAT)
	s_stCasActionContext[ulActionId].ulCatFilterId = CAS_FILTER_ID_NULL;
	s_stCasActionContext[ulActionId].eCatState = eCASSI_Table_Waiting;

#endif

#if defined (CAS_SUPPORT_SDT)
	s_stCasActionContext[ulActionId].ulSdtFilterId = CAS_FILTER_ID_NULL;
	s_stCasActionContext[ulActionId].eSdtState = eCASSI_Table_Waiting;
#endif


#if defined(CAS_SUPPORT_NIT)
	s_stCasActionContext[ulActionId].ulNitFilterId = CAS_FILTER_ID_NULL;
	s_stCasActionContext[ulActionId].eNitState = eCASSI_Table_Waiting;
#endif

#if defined(CAS_SUPPORT_BAT)
	s_stCasActionContext[ulActionId].ulBatFilterId = CAS_FILTER_ID_NULL;
	s_stCasActionContext[ulActionId].eBatState = eCASSI_Table_Waiting;
#endif

#if defined(CAS_SUPPORT_EIT)
	s_stCasActionContext[ulActionId].ulEitFilterId = CAS_FILTER_ID_NULL;
	s_stCasActionContext[ulActionId].eEitState = eCASSI_Table_Waiting;
#endif

#if defined(CAS_SUPPORT_TDT)
	s_stCasActionContext[ulActionId].ulTdtFilterId = CAS_FILTER_ID_NULL;
	s_stCasActionContext[ulActionId].eTdtState = eCASSI_Table_Waiting;
#endif

	return ERR_OK;
}

STATIC HERROR svc_cas_MgrInitActionContext(void)
{
	int idx = 0;

	for(idx=0; idx<CAS_MAX_NUM_OF_ACTION; idx++)
	{
		svc_cas_MgrInitActionContextByActionId(idx);
	}

	return ERR_OK;
}

STATIC HERROR svc_cas_MgrInitFileSystem(void)
{
	if (HLIB_DIR_IsExist(CAS_JFFS2_PATH) != TRUE)
	{
		if (HLIB_DIR_Make(CAS_JFFS2_PATH) != ERR_OK)
		{
			HxLOG_Error("error occurred in HLIB_DIR_Make\n");
			return ERR_FAIL;
		}
#if defined(CONFIG_DROP_ROOT_PRIVILEGES)
		/* obama�� access �����ϵ��� file permission ���� */
		HLIB_EXTRA_Chmod(CAS_JFFS2_PATH, 0700);

		{
			HBOOL bRoot = FALSE;

			if(HLIB_EXTRA_AmIRoot(&bRoot) == ERR_OK)
			{
				if(bRoot == TRUE)
				{
					if(HLIB_EXTRA_Chown(CAS_JFFS2_PATH, "obama", "obama") != ERR_OK)
					{
						HxLOG_Error("ERROR In [HLIB_EXTRA_Chown]. [%s]\n", CAS_JFFS2_PATH);
					}
				}
			}
		}
#endif
	}

	return ERR_OK;
}

STATIC HERROR svc_cas_MgrInitModule(void)
{
	HERROR	hErr;

	svc_cas_MgrInitActionContext();

	hErr = svc_cas_InstanceInitModule();
	if(hErr != ERR_OK)
	{
		HxLOG_Error("instance sub module init Error !!!\n");
		return ERR_FAIL;
	}

	hErr = psvc_cas_ScenarioInit();
	if(hErr != ERR_OK)
	{
		HxLOG_Error("CasMgrDefScenario_InitModule() error\n");
		return ERR_FAIL;
	}

	hErr = svc_cas_SiInitModule();
	if(hErr != ERR_OK)
	{
		HxLOG_Error("Si sub module init Error !!!\n");
		return ERR_FAIL;
	}

	hErr = svc_cas_MgrInitFileSystem();
	if(hErr != ERR_OK)
	{
		HxLOG_Error("svc_cas_MgrInitFileSystem() Error !!!\n");
		return ERR_FAIL;
	}

	hErr = svc_cas_PalPortVideoRegisterEventCallback(svc_cas_MgrUpdateVideoStateCb);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("svc_cas_PalPortVideoRegisterEventCallback() Error !!!\n");
		return ERR_FAIL;
	}

	hErr = svc_cas_PalPortAudioRegisterEventCallback(svc_cas_MgrUpdateAudioStateCb);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("svc_cas_PalPortAudioRegisterEventCallback() Error !!!\n");
		return ERR_FAIL;
	}

#if 0
	// DRM Init
	local_casmain_InitDrm();
#endif

	return ERR_OK;
}

HERROR svc_cas_MgrSubCasInit(void)
{
	return psvc_cas_MgrSubCasInit();
}

SvcCas_Context_t *svc_cas_MgrGetActionContextByActionId(HUINT32 ulActionId)
{
	if(ulActionId >= CAS_MAX_NUM_OF_ACTION)
	{
		HxLOG_Assert(0);
		return NULL;
	}

	return &s_stCasActionContext[ulActionId];
}

SvcCas_Context_t *svc_cas_MgrGetActionContextByAction(Handle_t hAction)
{
	HUINT32 ulActionId;

	ulActionId = svc_cas_SvcPortRmGetActionId(hAction);

	if(ulActionId >= CAS_MAX_NUM_OF_ACTION)
	{
		HxLOG_Error("ulActionId >= CAS_MAX_NUM_OF_ACTION, ulActionId: %d \n", ulActionId);
		return NULL;
	}

	return svc_cas_MgrGetActionContextByActionId(ulActionId);
}

SIxTextEncoding_e svc_cas_MgrGetTextEncoding(HUINT32 ulActionId)
{
	SvcCas_Context_t *pstContext = svc_cas_MgrGetActionContextByActionId(ulActionId);

	if (NULL == pstContext)
		return eSIxTEXT_ENC_DEFAULT;

	return pstContext->eTextEnc;
}

SIxSpec_e svc_cas_MgrGetSiSpec(HUINT32 ulActionId)
{
	SvcCas_Context_t *pstContext = svc_cas_MgrGetActionContextByActionId(ulActionId);

	if (NULL == pstContext)
		return eSIxSPEC_DVB;

	return pstContext->eSiSpec;
}


STATIC HERROR svc_cas_MgrGetChannelType(SvcCas_Context_t *pstCasContext, SvcCas_ChannelType_e *eChType)
{
	HUINT32				i;
	SvcCas_ChannelType_e	etype = eCasChannel_FTA;

	if(pstCasContext->stPmtInfo.bProgramCaFlag == TRUE)
	{
		etype = eCasChannel_Scramble;
	}
	else
	{
		for(i=0; i < pstCasContext->stPmtInfo.ulNumOfElement; i++)
		{
			if(pstCasContext->stPmtInfo.stElementInfo[i].bElementCaFlag == TRUE)
			{
				etype = eCasChannel_Scramble;
				break;
			}
		}
	}



	/*	PMT ���� CA_descriptor �� �����ϴ��� PES ������ Scramble ���θ� Ȯ���Ͽ� PES �� Scramble �Ǿ� ���� ������ FTA �� �����ϰ� ó����.
			�ߵ��� ��� ���� FTA �����ӿ��� PMT �� CA Descriptor �� �����ϴ� ��찡 ����. */
	/*	Thanks! ��Ʈ�� �߰��� */
	/*	�ߵ� �̿� ���� �� ���� �׽�Ʈ������ �ش� ���񽺵��� �����Ѵ�.
		��ü������ ������ �ϵ� DEMUX�� �ش� ����� �������� ���� ��� �ٸ� �ַ���� �ִ��� ����� �ʿ��� �κ��̴�. (����� ������?) */
#if defined(CONFIG_OP_MIDDLE_EAST) || defined(CONFIG_OP_VTC) || defined(CONFIG_OP_DIGITURK) || defined (CONFIG_OP_SES)
	{
		HERROR 	hErr = ERR_FAIL;
		HBOOL	bIsSupportScrambleBitChecking = FALSE;
		SvcCas_DevDmxTsState_e eState;

		if(etype != eCasChannel_FTA)
		{
			bIsSupportScrambleBitChecking = svc_cas_DevDmxIsSupportScrambleBitChecking((HUINT32)pstCasContext->ulDemuxId);
			if (bIsSupportScrambleBitChecking == TRUE)
			{

				hErr = svc_cas_DevDmxCheckScrambleBitByPid((HUINT32)pstCasContext->ulDemuxId, &eState);
				HxLOG_Info("pstCasContext->ulDemuxId: %d, hErr: %d, eState: %d, pstCasContext->state: %d \n", pstCasContext->ulDemuxId, hErr, eState, pstCasContext->state);
				if (hErr == ERR_OK && eState == eCAS_DMX_TSSTATE_CLEAN && pstCasContext->state != eCAS_State_CasOK)
				{
					HxLOG_Info("PES is not scrambled, \n");
					etype = eCasChannel_FTA;
				}
				else
				{
					HxLOG_Info("PES is also scrambled, \n");
				}
			}
		}
	}
#endif




	*eChType = etype;

	return ERR_OK;
}

STATIC void svc_cas_MgrUpdateVideoStateCb(HUINT32 ulDevId, SvcCas_VideoDevEvent_e eEvt)
{
	HUINT32 				nResult = ERR_OK;
	SvcCas_Message_t 		casMsg;


	HxLOG_Print("[%s:%d] evt (%d) \n",__FUNCTION__,__LINE__,eEvt);

	/* Message Create */
	HxSTD_memset(&casMsg, 0x00, sizeof(SvcCas_Message_t));
	casMsg.simple.command = CMD_CAS_CHECKSTREAM;
	casMsg.simple.param = eEvt;
	casMsg.simple.version = eCAS_NOTI_TYPE_VIDEO;

	nResult = VK_MSG_Send(svc_cas_MgrGetMsgQueueId(), &casMsg, sizeof(SvcCas_Message_t));
	if(nResult != ERR_OK)
	{
		HxLOG_Error("CMD_CAS_CHECKSTREAM/eCAS_NOTI_TYPE_VIDEO Message Seding Failure. \n");
		return;
	}

	return;
}

STATIC void svc_cas_MgrUpdateAudioStateCb(SvcCas_AudioDevEvent_e eEvt)
{
	HUINT32 			nResult = ERR_OK;
	SvcCas_Message_t 	casMsg;

	HxLOG_Print("[%s:%d] evt (%d) \n",__FUNCTION__,__LINE__,eEvt);

	/* Message Create */
	HxSTD_memset(&casMsg, 0x00, sizeof(SvcCas_Message_t));
	casMsg.simple.command = CMD_CAS_CHECKSTREAM;
	casMsg.simple.param = eEvt;
	casMsg.simple.version = eCAS_NOTI_TYPE_AUDIO;

	nResult = VK_MSG_Send(svc_cas_MgrGetMsgQueueId(), &casMsg, sizeof(SvcCas_Message_t));
	if(nResult != ERR_OK)
	{
		HxLOG_Error("CMD_CAS_CHECKSTREAM/eCAS_NOTI_TYPE_AUDIO Message Seding Failure. \n");
		return;
	}

	return;
}

STATIC HERROR svc_cas_MgrFillCasActionContext(Handle_t hAction, Handle_t hSvc, SvcCas_ActionType_e actType, SvcCas_StartData_t *pstCasStartData, SvcCas_Context_t *pstCasContext)
{
	HERROR				hErr;
	HUINT32 				nResult = ERR_OK;
	HUINT32				ulActionId;
	HUINT16				usUniqueId;
	HUINT16				usTsId;
	HUINT16				usOnId;
	SvcCas_DbSvcInfo_t	stSvcInfo;

	ulActionId = svc_cas_SvcPortRmGetActionId(hAction);
	hErr = svc_cas_MgrInitActionContextByActionId(ulActionId);
	if(hErr != ERR_OK)
	{
		HxLOG_Critical("Critical Eror!!!\n");
		return ERR_FAIL;
	}

	pstCasContext->hSvc				= hSvc;
	pstCasContext->hAction			= hAction;
	pstCasContext->eActType			= actType;
	pstCasContext->ulActionId		= ulActionId;
	pstCasContext->eSiSpec			= pstCasStartData->eSiSpec;
	pstCasContext->eTextEnc			= pstCasStartData->eTextEnc;
	pstCasContext->stCasStartData	= *pstCasStartData;

	hErr = svc_cas_SvcPortRmGetResourceId(pstCasContext->ulActionId, eCAS_RM_ITEM_DEMUX, &pstCasContext->ulDemuxId);
	if(hErr != ERR_OK)
	{
		HxLOG_Critical("Critical Eror!!!\n");
		return ERR_FAIL;
	}


	/*
			start �� context update�� action ������ ����� �޸� �Ѵ�.
			live : db_svc����
			pvr : mw pvr����...���Ŀ��� mwc �� ���� �Ǿ�� ��.
	*/
	switch(pstCasContext->eActType)
	{
	case eCAS_ActionType_Live:
	case eCAS_ActionType_Data:
	case eCAS_ActionType_Recording:
	case eCAS_ActionType_Tsr_Rec:

		/*
		*	DB Access�� MGR������ �ϵ��� �Ѵ�.
		*	Sub Cas ���� DB Access �� Sync ������ �߻� ����.
		*/
		nResult = svc_cas_SvcPortDbGetTsInfoByServiceHandle(hSvc, &usUniqueId, &usTsId, &usOnId);
		if(ERR_OK !=nResult)
		{
			HxLOG_Critical("Critical Eror!!!\n");
		}

		nResult = svc_cas_SvcPortDbGetSvcInfoByServiceHandle(hSvc, &stSvcInfo);
		if(ERR_OK!=nResult)
		{
			HxLOG_Critical("Critical Eror!!!\n");
		}

		pstCasContext->usOrgNetId	= usOnId;
		pstCasContext->usTsId		= usTsId;
		pstCasContext->usSvcId		= stSvcInfo.usSvcId;
		pstCasContext->usPmtPid		= stSvcInfo.usPmtPid;
		pstCasContext->usUniqueId = usUniqueId;

		#if defined(CONFIG_MW_CAS_NAGRA_OP_UPC)
		if (stSvcInfo->eCasMgrCasType == eCAS_SUB_TYPE_FTA)
		{
			HUINT16 usScAlarm;
			MW_CA_NA_GetCurrScStatus(0, &usScAlarm);
			if (usScAlarm != CA_ALARM_SMARTCARD_REMOVED)
			{
				svc_cas_SvcPortUtilPostMsgToAp(eSEVT_CAS_OK, hAction, eDxCAS_GROUPID_NONE, 0, 0);
			}
		}
		#endif

		break;

#if	defined(CONFIG_MW_MM_PVR)
	case eCAS_ActionType_Playback:
	case eCAS_ActionType_Descramble_a_File:
	case eCAS_ActionType_Tsr_Play:
		{
			SvcCas_PvrSvcInfo_t	stCasPvrSvcInfo;

			nResult = svc_cas_SvcPortPvrGetService(hAction, &stCasPvrSvcInfo);
			if(nResult != ERR_OK)
			{
				HxLOG_Error("svc_cas_SvcPortPvrGetService ret nResult:0x%X\n",nResult);
				return ERR_FAIL;
			}

			pstCasContext->usOrgNetId	= stCasPvrSvcInfo.usOnId;
			pstCasContext->usTsId		= stCasPvrSvcInfo.usTsId;
			pstCasContext->usSvcId		= stCasPvrSvcInfo.usSvcId;
			pstCasContext->usPmtPid		= stCasPvrSvcInfo.usPmtPid;
			pstCasContext->usUniqueId	= svc_cas_SvcPortDbGetAllUniqueId();
		}
		break;
#endif

	default:
		HxLOG_Error("pContext->eActType is unknown : 0x%X\n", pstCasContext->eActType);
		break;

	}

	HxLOG_Info("OnId(0x%x), TsId(0x%x), ServiceID(0x%x), PmtPID(0x%x) \n", pstCasContext->usOrgNetId, pstCasContext->usTsId, pstCasContext->usSvcId, pstCasContext->usPmtPid);

	return ERR_OK;
}

#if defined (CONFIG_MW_CAS_NAGRA)
#if defined(CONFIG_MW_CAS_NAGRA_OP_MEDIASET)  || defined(CONFIG_MW_CAS_NAGRA_OP_SPAIN_TDT)
STATIC void svc_cas_MgrStopNitFilter4Nagra(SvcCas_Context_t *pstCasContext, Handle_t hNextSvc)
{
	HERROR		hErr;
	HUINT16		usUniqueId;

	HxLOG_Info("hNextSvc = 0x%x\n", hNextSvc);

	if (hNextSvc != HANDLE_NULL)
	{
		DbSvc_TsInfo_t	tsInfo;

		hErr = svc_cas_SvcPortDbGetTsInfoByServiceHandle(hNextSvc, &usUniqueId, NULL, NULL);
		HxLOG_Info("pstCasContext->usUniqueId = %d, tsInfo.usUniqueId = %d\n", pstCasContext->usUniqueId, usUniqueId);
		if (hErr == ERR_OK)
		{
			if (pstCasContext->usUniqueId != usUniqueId)
			{
				MW_CA_NA_NotifyTPChanged(TRUE);
				pstCasContext->bNotTpChanged = FALSE;

				hErr = svc_cas_SiStopNitFilter(pstCasContext);
				if (hErr == ERR_OK)
				{
					hErr = svc_cas_SiFreeNitRaw(pstCasContext);
					HxLOG_Info("Nit filter stop&free\n");
				}
				else
				{
					HxLOG_Error("svc_cas_SiStopNitFilter error..\n");
				}
			}
			else
			{
				MW_CA_NA_NotifyTPChanged(FALSE);
				pstCasContext->bNotTpChanged = TRUE;
			}
		}
		else
		{
			MW_CA_NA_NotifyTPChanged(FALSE);
			pstCasContext->bNotTpChanged = TRUE;
			HxLOG_Error("svc_cas_SvcPortDbGetTsInfoByServiceHandle error..\n");
		}
	}
	else
	{
		MW_CA_NA_NotifyTPChanged(TRUE);
		pstCasContext->bNotTpChanged = FALSE;

		hErr = svc_cas_SiStopNitFilter(pstCasContext);
		if (hErr == ERR_OK)
		{
			hErr = svc_cas_SiFreeNitRaw(pstCasContext);
			HxLOG_Info("Nit filter stop&free\n");
		}
		else
		{
			HxLOG_Error("svc_cas_SiStopNitFilter error..\n");
		}
	}
}
#endif
#endif

#if defined(CONFIG_MW_CAS_IRDETO_UCAS)
STATIC HERROR svc_cas_MgrFastSetPidInfo(SvcCas_Context_t *pstCasContext)
{
	HERROR	hErr = ERR_FAIL;
	HUINT32 i = 0, idx = 0;
	SvcCas_SetPidInfo_t 	stCasPid;
	SvcCas_ActionType_e 	eActType = eCAS_ActionType_None;
	SvcCas_ChannelType_e 	eCasType = 0;
	SvcCas_SetPidInfo_t		stEsPidInfo;
	SvcCas_PmtInfo_t		*pstPmtInfo = NULL;

	HxSTD_MemSet(&stCasPid, 0, sizeof(SvcCas_SetPidInfo_t));
	HxSTD_memset(&stEsPidInfo, 0x00, sizeof(SvcCas_SetPidInfo_t));

	/*
		IRDETO UC ������ CAS���� PMT�� Update �� �޾Ƽ� ó���ϱ� ���� SET PID�� ���ؼ� ����� PID ��
		�ռ� �����ؾ� �Ѵ�. �׷��� �̷��� ����. �� ���� ��� ������ �����ϱ�!!
	*/
	if (pstCasContext->eActType == eCAS_ActionType_Tsr_Rec)
	{
		eActType = eCAS_ActionType_Tsr_Rec;
	}
	else if (pstCasContext->eActType == eCAS_ActionType_Recording)
	{
		eActType = eCAS_ActionType_Recording;
	}
	else
	{
		eActType = eCAS_ActionType_None;
	}

	if (eActType != eCAS_ActionType_None)
	{
		hErr = SVC_CAS_GetChannelType(pstCasContext->hAction, &eCasType);
		if (hErr == ERR_OK && eCasType != eCasChannel_FTA)
		{
			pstPmtInfo = svc_cas_MgrSubGetPmtInfo(pstCasContext->hAction);
			if (pstPmtInfo != NULL)
			{
				for (i = 0; i < pstPmtInfo->ulNumOfElement; i++)
				{
					switch (pstPmtInfo->stElementInfo[i].eEsType)
					{
						case	eEsType_Audio:
						case	eEsType_Data:
						case	eEsType_Video:
						case	eEsType_Subtitle:
						case	eEsType_Teletext:
							stEsPidInfo.stPidInfo[idx].esType = pstPmtInfo->stElementInfo[i].eEsType;
							stEsPidInfo.stPidInfo[idx].usPid = pstPmtInfo->stElementInfo[i].usPid;
							idx++;
							break;
						default:
							break;
					}
				}
				stEsPidInfo.ulPidNum = idx;
			}

			hErr = svc_cas_MgrSetPid(pstCasContext->hAction, eActType, &stEsPidInfo);
			if (hErr != ERR_OK)	HxLOG_Critical("Critical Eror!!!\n");
		}
	}
	else
	{
		return	ERR_FAIL;
	}

	return	ERR_OK;
}
#endif

STATIC HERROR svc_cas_MgrClearSubCasTryFlag(SvcCas_DevInstance_t *pstCasInst, HUINT32 ulCaInstanceId)
{
	HUINT32			i;
	SvcCas_Context_t	*pstCasContext;

	if(pstCasInst == NULL)
	{
		return ERR_FAIL;
	}

	for(i = 0; i < pstCasInst->ulCurRefCnt ; i++)
	{
		pstCasContext = svc_cas_MgrGetActionContextByActionId(pstCasInst->aulActionId[i]);
		if(pstCasContext == NULL)
		{
			continue;
		}

		pstCasContext->abTryCasInst[ulCaInstanceId] = FALSE;
	}

	return ERR_OK;
}

HERROR svc_cas_MgrDisconnectService(HUINT32 ulStopActionId, HBOOL bNotifyCasStop)
{
	HERROR			hErr;
	SvcCas_Context_t	*pstStopCasContext;

	if(ulStopActionId >= CAS_MAX_NUM_OF_ACTION)
	{
		/* cas instance�� �������� ���� ���񽺿� ���ؼ� stop�� ��� �ü��� �ִ�.. �ؼ� ERR_OK*/
		HxLOG_Info("ulStopActionId = %d, bNotifyCasStop = %d\n", ulStopActionId, bNotifyCasStop);
		return ERR_OK;
	}

	HxLOG_Info("ulStopActionId = %d, bNotifyCasStop = %d\n", ulStopActionId, bNotifyCasStop);

/* �Ʒ� stop ������ �������� ����~~!!
	��? --> sub cas���� disconnect�� �ް� stop�� �������� ������� �����ϰ� �Ǿ��ִ�..
		������ STOP ������ �̻��� ������� ����..
*/
	pstStopCasContext = svc_cas_MgrGetActionContextByActionId(ulStopActionId);
	if(pstStopCasContext == NULL)
	{
		HxLOG_Error("pstStopCasContext is NULL\n");
		return ERR_FAIL;
	}

/*	sub cas�� �������� ���� ���� sub CAS stop�� �˷� �ش�.
	������ �Ǿ����� disconnet -> stop -> instance ������ �Ѵ�.
*/
	if(pstStopCasContext->state >= eCAS_State_subCasSelected)
	{

		/* Sub Cas �� Connect�� CAS���Ը� disconnect�� �˸���.
		*/
		hErr = svc_cas_InstanceManageCasInstance(pstStopCasContext, CAS_INST_DISCONNECT);
		if(hErr != ERR_OK)
		{
			HxLOG_Error("svc_cas_InstanceManageCasInstance's ret : 0x%X\n");
			return ERR_FAIL;
		}

		/* Sub Cas�鿡�� stop session�� �˸���.
		*/
		if(bNotifyCasStop == TRUE)
		{
			hErr = svc_cas_InstanceStopSession(pstStopCasContext);
			if(hErr != ERR_OK)
			{
				HxLOG_Error("svc_cas_InstanceStopSession's ret : 0x%X\n");
				return ERR_FAIL;
			}
		}

		/* ������ ��û ���̴� Action Instance�� ���� �Ѵ�.
		*/
		hErr = svc_cas_InstanceDisconnectCasInstance(pstStopCasContext);
		if(hErr != ERR_OK)
		{
			HxLOG_Error("svc_cas_InstanceDisconnectCasInstance() error \n");
			return ERR_FAIL;
		}
	}
	else
	{
		/* Sub Cas�鿡�� stop session�� �˸���.
		*/
		if(bNotifyCasStop == TRUE)
		{
			hErr = svc_cas_InstanceStopSession(pstStopCasContext);
			if(hErr != ERR_OK)
			{
				HxLOG_Error("svc_cas_InstanceStopSession's ret : 0x%X\n");
				return ERR_FAIL;
			}
		}
	}

	// CAS scenario �켱������ ���� disconnect�� ���� CAS stop�� subcas���� �����ϰ� �ִ� context�� �ʱ�ȭ ���� ���� �� �ִ�.
	pstStopCasContext->state = bNotifyCasStop ? eCAS_State_Idle : eCAS_State_subCasNotSelected;
	return ERR_OK;
}

STATIC HERROR svc_cas_MgrConnectSevcie(SvcCas_Context_t	*pstCasContext, HUINT32 ulCasInstId, HUINT32 ulStopActionId)
{
	HERROR		hErr;

	HxLOG_Trace();
	/* ã���� ���� ���� ���� CAS OK�� �������� �Ѵ�. -> ��� �ٲٰ� ������ �켱 ���� table�� ������ �� �̰��� define�� �ְų� �������� ������ ����~
		ulStopActionId �� �ִ� ���� �켱 ���� ������ ����� �ϴ� action
	*/
	if(ulCasInstId == HANDLE_NULL)
	{
		svc_cas_SvcPortUtilPostMsgToAp(eSEVT_CAS_OK, pstCasContext->hAction, eDxCAS_GROUPID_NONE, 0, 0);

		/* ci plus�� PVR �ó�����. */
		#if defined(CONFIG_MW_CI_PLUS) && defined (CONFIG_MW_MM_PVR)
		svc_cas_SvcPortUtilPostMsgToAp(eSEVT_CAS_DRM_NONE, pstCasContext->hAction, 0, 0, 0);
		#endif

		#if	defined(CONFIG_MW_CAS_IRDETO_PVR)
		//	FTA�� ��� Digital MacroVision Ȥ�� DRMó�� ����.
		svc_cas_SvcPortUtilPostMsgToAp(eSEVT_CAS_DMV_NONE, pstCasContext->hAction, 0, 0, 0);
		#elif defined(CONFIG_IRDETO_HMX_PVR)
		svc_cas_SvcPortUtilPostMsgToAp(eSEVT_CAS_DMV_NONE, pstCasContext->hAction, 0, 0, 0);
		#endif

		pstCasContext->state = eCAS_State_subCasNotSelected;
	}
	else
	{
		/*	�켱 ���� ������ ����� �ϴ� action
		*/
		if(ulStopActionId != HANDLE_NULL)
		{
			svc_cas_MgrDisconnectService(ulStopActionId, FALSE);
		}

		/*	���õ� Cas inst�� �����Ѵ�.
		*/
		hErr = svc_cas_InstanceConnectCasInstance(pstCasContext, ulCasInstId);
		if(hErr != ERR_OK)
		{
			HxLOG_Error("svc_cas_InstanceConnectCasInstance() error \n");
			return ERR_FAIL;
		}

		/*	���õ� sub cas���� select�Ȱ��� �˸���.
		*/
		hErr = svc_cas_InstanceManageCasInstance(pstCasContext, CAS_INST_CONNECT);
		if(hErr != ERR_OK)
		{
			HxLOG_Error("svc_cas_InstanceManageCasInstance(CAS_INST_CONNECT) error \n");
			return ERR_FAIL;
		}

		pstCasContext->state = eCAS_State_subCasSelected;
	}

	return ERR_OK;
}

#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
HBOOL svc_cas_MgrGetSystemIdFoundByInstance(SvcCas_Context_t *pstCasContext, HUINT32 ulInstanceId)
{
	HUINT32		ulSlotIdx = 0;
	HUINT32		ulCaDescIdx = 0;
	HUINT32		ulCaSysIdIdx = 0;
	HBOOL		bSysIdFound = FALSE;
	SvcCas_DevInstance_t	*pCaInst = NULL;
	HUINT16		usCasIdMask = 0xFFFF;

	if(pstCasContext == NULL)
	{
		return FALSE;
	}

	pCaInst = svc_cas_InstanceGetCasDevInst(ulInstanceId);

	if(pCaInst)
	{
		/* compare descriptor & caInst sys id */
		if(pstCasContext->stPmtInfo.bProgramCaFlag == TRUE)
		{
			HxLOG_Info("\t\t + number of program ca descriptor (%d)  \n", pstCasContext->stPmtInfo.ulNumOfProgramCaDescriptor);
			for (ulCaDescIdx = 0; ulCaDescIdx < pstCasContext->stPmtInfo.ulNumOfProgramCaDescriptor; ulCaDescIdx++)
			{
				for (ulSlotIdx = 0; ulSlotIdx < CAS_MAX_NUM_OF_SLOT; ulSlotIdx++)
				{
					for (ulCaSysIdIdx = 0; ulCaSysIdIdx < pCaInst->ulNumCaSysId[ulSlotIdx]; ulCaSysIdIdx++)
					{
						SvcCas_CaDescriptor_t	*pstProgramCaDescriptor = &pstCasContext->stPmtInfo.stProgramCaDescriptor[ulCaDescIdx];
						HUINT16 				usInstCaSysId = pCaInst->ausCaSysId[ulSlotIdx][ulCaSysIdIdx];

						HxLOG_Info("\t\t\t + Instance ca sys id(0x%x), pmt global ca sys id(0x%x) \n", usInstCaSysId, pstCasContext->stPmtInfo.stProgramCaDescriptor[ulCaDescIdx].usCaSysId);

						if ((usInstCaSysId & usCasIdMask) == (pstProgramCaDescriptor->usCaSysId & usCasIdMask))
						{
							bSysIdFound = TRUE;
						}
					}
				}
			}
		}
	}

	return bSysIdFound;
}
#endif

#if defined(CONFIG_MW_CAS_NAGRA_OP_SES_CAKTEST)
HERROR svc_cas_MgrSearchNotDesCasContextNConnect(HUINT32 ulCaInstanceId, DxCAS_GroupId_e eCasGroupId)
{
	HUINT32				i;
	HUINT32 			ulSearchedCasInstId = HANDLE_NULL;
	HUINT32				ulStopActionId = HANDLE_NULL;
	SvcCas_Context_t	*pstContext = NULL;
	HBOOL				   bVideoDecoding = FALSE;

	svc_cas_DevVideoIsRunning(0, &bVideoDecoding);

	HxLOG_Trace();
	HxLOG_Info("** ulCaInstanceId(%d), eCasGroupId(%s)\n", ulCaInstanceId, eCasGroupId == eDxCAS_GROUPID_CAM ? "eDxCAS_GROUPID_CAM" : "ECAS");

	for(i = 0; i < CAS_MAX_NUM_OF_ACTION; i++)
	{
		pstContext = svc_cas_MgrGetActionContextByActionId(i);
		if((pstContext == NULL) || (pstContext->bPmtUpdated == FALSE))
		{
			continue;
		}

#if defined (CONFIG_DEBUG)
		svc_cas_PrintMgrContextInfo(i, pstContext, bVideoDecoding, ulCaInstanceId, eCasGroupId);
#endif

		if( ((pstContext->bConnect == CAS_INST_CONNECT) && (pstContext->state == eCAS_State_CasFail))
			||	(pstContext->state == eCAS_State_Idle)
			||	(pstContext->state == eCAS_State_subCasNotSelected))
		{
			svc_cas_ScenarioSearchCasInstance(pstContext, ulCaInstanceId, &ulSearchedCasInstId, &ulStopActionId, FALSE);

			HxLOG_Info("\n======================= N Connect : Case 1 =======================\n");
			HxLOG_Info("++ ulCasInstId = %d, ulStopActionId = %d\n", ulSearchedCasInstId, ulStopActionId);

			// ���Ѿ��� instance�� connect �Ǿ� �ִ� ���¿��� �� instance�� ���ԵǾ��� �� disconnect�� ���� �ʴ´�.
			// TODO: scenario�� ������ ���̳� �ϴ��� �Ʒ��� ���� ������ �д�.
			if( (pstContext->ulCasInstanceId != HANDLE_NULL)
				&& (pstContext->ulCasInstanceId != ulSearchedCasInstId)
				&& (ulStopActionId == HANDLE_NULL ))
			{
				SvcCas_DevInstance_t *pstCasInst;

				HxLOG_Warning("W/A : Disconnect action...\n");

				pstCasInst = svc_cas_InstanceGetCasDevInst(pstContext->ulCasInstanceId);
				if(pstCasInst != NULL)
				{
					HUINT32 j;
					HUINT32 ulCurRefCnt;
					HUINT32 aulActionId[CAS_MAX_NUM_OF_ACTION];

					ulCurRefCnt = pstCasInst->ulCurRefCnt;
					for(j = 0; j < ulCurRefCnt; j++)
						aulActionId[j] = pstCasInst->aulActionId[j];

					for(j = 0; j < ulCurRefCnt; j++)
						svc_cas_MgrDisconnectService(aulActionId[j], FALSE);
				}
			}

			if(svc_cas_MgrConnectSevcie(pstContext, ulSearchedCasInstId, ulStopActionId) != ERR_OK)
			{
				HxLOG_Info("svc_cas_MgrConnectSevcie() error \n");
			}
		}
		// CAM�� ���ԵǾ� �ִµ� systemId�� ��ġ�ؼ� casinstid�� �����ϰ� state�� ok�� �Ȱ������ ������ ���� descramble�� �ȵǴ� ���¿���
		// ECAS�� ������ ��� ECAS�� ������ �������� �ʾ� ���⼭ �߰���.
		else if(eCasGroupId != eDxCAS_GROUPID_CAM /* ecas�ΰ�� */
				&& pstContext->bConnect == CAS_INST_CONNECT
				&& (pstContext->state == eCAS_State_CasOK/* || pstContext->state == eCAS_State_subCasNotSelected*/)
				&& pstContext->eChType == eCasChannel_Scramble
				&& bVideoDecoding == FALSE)
		{
			svc_cas_ScenarioSearchCasInstance(pstContext, ulCaInstanceId, &ulSearchedCasInstId, &ulStopActionId, FALSE);

			HxLOG_Info("\n======================= N Connect : Case 2 =======================\n");
			HxLOG_Info("++ ulCasInstId = %d, ulStopActionId = %d\n", ulSearchedCasInstId, ulStopActionId);

			// ���Ѿ��� instance�� connect �Ǿ� �ִ� ���¿��� �� instance�� ���ԵǾ��� �� disconnect�� ���� �ʴ´�.
			// TODO: scenario�� ������ ���̳� �ϴ��� �Ʒ��� ���� ������ �д�.
			if( (pstContext->ulCasInstanceId != HANDLE_NULL)
				&& (pstContext->ulCasInstanceId != ulSearchedCasInstId)
				&& (ulStopActionId == HANDLE_NULL ))
			{
				SvcCas_DevInstance_t *pstCasInst;

				HxLOG_Warning("W/A : Disconnect action...\n");

				pstCasInst = svc_cas_InstanceGetCasDevInst(pstContext->ulCasInstanceId);
				if(pstCasInst != NULL)
				{
					HUINT32 j;
					HUINT32 ulCurRefCnt;
					HUINT32 aulActionId[CAS_MAX_NUM_OF_ACTION];

					ulCurRefCnt = pstCasInst->ulCurRefCnt;
					for(j = 0; j < ulCurRefCnt; j++)
						aulActionId[j] = pstCasInst->aulActionId[j];

					for(j = 0; j < ulCurRefCnt; j++)
						svc_cas_MgrDisconnectService(aulActionId[j], FALSE);
				}
			}

			if(svc_cas_MgrConnectSevcie(pstContext, ulSearchedCasInstId, ulStopActionId) != ERR_OK)
			{
				HxLOG_Info("svc_cas_MgrConnectSevcie() error \n");
			}

			// ECAS�� ���� ��쿡�� ���� �߻�, ECAS�� ���ؼ� instance pmt update�� �ٽ� ���ش�.
			if(svc_cas_InstanceUpdatePmt(pstContext) != ERR_OK)
			{
				HxLOG_Info("svc_cas_InstanceUpdatePmt() error \n");
			}
		}
		/* ecas or cam�� �������� ���� ���¿��� channel change�� �̷������ ecas or cam�� ���Խ� live�� ������ �ʴ� ���� */
		else if(pstContext->bConnect == CAS_INST_DISCONNECT
				&& pstContext->state == eCAS_State_CasFail
				&& pstContext->bPmtUpdated == TRUE
				&& pstContext->eChType == eCasChannel_Scramble)
		{
			HUINT32 j = 0;
			SvcCas_DevInstance_t	*pstCasInstance = NULL;

			svc_cas_ScenarioSearchCasInstance(pstContext, ulCaInstanceId, &ulSearchedCasInstId, &ulStopActionId, FALSE);

			HxLOG_Info("\n======================= N Connect : Case 3 =======================\n");
			HxLOG_Info("ulCasInstId = %d, ulStopActionId = %d\n", ulSearchedCasInstId, ulStopActionId);

			// ���Ѿ��� instance�� connect �Ǿ� �ִ� ���¿��� �� instance�� ���ԵǾ��� �� disconnect�� ���� �ʴ´�.
			// TODO: scenario�� ������ ���̳� �ϴ��� �Ʒ��� ���� ������ �д�.
			if( (pstContext->ulCasInstanceId != HANDLE_NULL)
				&& (pstContext->ulCasInstanceId != ulSearchedCasInstId)
				&& (ulStopActionId == HANDLE_NULL ))
			{
				SvcCas_DevInstance_t *pstCasInst;

				HxLOG_Warning("W/A : Disconnect action...\n");

				pstCasInst = svc_cas_InstanceGetCasDevInst(pstContext->ulCasInstanceId);
				if(pstCasInst != NULL)
				{
					HUINT32 j;
					HUINT32 ulCurRefCnt;
					HUINT32 aulActionId[CAS_MAX_NUM_OF_ACTION];

					ulCurRefCnt = pstCasInst->ulCurRefCnt;
					for(j = 0; j < ulCurRefCnt; j++)
						aulActionId[j] = pstCasInst->aulActionId[j];

					for(j = 0; j < ulCurRefCnt; j++)
						svc_cas_MgrDisconnectService(aulActionId[j], FALSE);
				}
			}

			// device�� �����ϴ��� �ٽ� check�Ѵ�.
			for(j=0; j<CAS_MAX_NUM_OF_INSTANCE; j++)
			{
				pstCasInstance = svc_cas_InstanceGetCasDevInst(j);
				if(pstCasInstance == NULL)
				{
					continue;
				}

				HxLOG_Debug("\t [%d] InputCasInstType(%s), eCasInstType(%s), eCamState(%s)\n",
									j,
									eCasGroupId == eDxCAS_GROUPID_CAM ? "eDxCAS_GROUPID_CAM" : "eDxCAS_GROUPID_NA",
									pstCasInstance->eCasInstType == eCasInst_Embedded ? "eCasInst_Embedded" : "eCasInst_CI",
									pstCasInstance->eCamState == eCAS_CAM_STATE_Removed?"eCAS_CAM_STATE_Removed":"Non eCAS_CAM_STATE_Removed");

				// ECAS
				if(eCasGroupId != eDxCAS_GROUPID_CAM && pstCasInstance->eCasInstType == eCasInst_Embedded)
				{
					// only sc�� ���Ե� ��쿡�� check.
					if(svc_cas_InstanceIsAdaptedSCInserted(j, 0) == TRUE)
					{
						HxLOG_Debug("ECAS : svc_cas_MgrUpdatePmt ==> restart\n");
						//svc_cas_MgrUpdatePmt(pstContext->hAction, pstContext->ulPmtFilterId, pstContext->ePmtState);

						if(svc_cas_MgrConnectSevcie(pstContext, ulSearchedCasInstId, ulStopActionId) != ERR_OK)
						{
							HxLOG_Info("svc_cas_MgrConnectSevcie() error \n");
						}

						if(svc_cas_InstanceUpdatePmt(pstContext) != ERR_OK)
						{
							// ECAS�� ���� ��쿡�� ���� �߻�, ECAS�� ���ؼ� instance pmt update�� �ٽ� ���ش�.
							HxLOG_Info("svc_cas_InstanceUpdatePmt() error \n");
						}
						break;
					}
				}
				// CAM
				else if(eCasGroupId == eDxCAS_GROUPID_CAM && pstCasInstance->eCasInstType == eCasInst_CI && pstCasInstance->eCamState != eCAS_CAM_STATE_Removed)
				{
					if(svc_cas_MgrConnectSevcie(pstContext, ulSearchedCasInstId, ulStopActionId) != ERR_OK)
					{
						HxLOG_Info("svc_cas_MgrConnectSevcie() error \n");
					}
					break;
				}
			}
		}
	}

	#if defined(CONFIG_MW_CI) && defined(CONFIG_MW_CI_USE_EXTERNAL_CONTROLLER)
	/*	CI�� ������ �ȵǾ��� �� ~ path�� ������ �ֵ��� �Ѵ�.
		�ϳ��� ������ �Ǿ� ������ ci mgr�� ���ؼ� ����ǵ��� �Ѵ�.
	*/
	HxLOG_Info("svc_cas_InstanceFindNotConnectPathCiNConnect start() check device\n");
	svc_cas_InstanceFindNotConnectPathCiNConnect(ulSearchedCasInstId);
	#endif

	return ERR_OK;
}
#else
HERROR svc_cas_MgrSearchNotDesCasContextNConnect(HUINT32 ulCaInstanceId)
{
	HUINT32				i;
	HUINT32 			ulSearchedCasInstId = HANDLE_NULL;
	HUINT32				ulStopActionId = HANDLE_NULL;
	SvcCas_Context_t	*pstContext = NULL;
	HxLOG_Trace();

	for(i = 0; i < CAS_MAX_NUM_OF_ACTION; i++)
	{
		pstContext = svc_cas_MgrGetActionContextByActionId(i);
		if((pstContext == NULL) || (pstContext->bPmtUpdated == FALSE))
		{
			continue;
		}

		HxLOG_Info("[CURRENT] hAction = 0x%x, bConnect = %d, ulCasInstanceId = %d, state = %d\n", pstContext->hAction, pstContext->bConnect, pstContext->ulCasInstanceId, pstContext->state);

		if( ((pstContext->bConnect == CAS_INST_CONNECT) && (pstContext->state == eCAS_State_CasFail))
			|| (pstContext->state <= eCAS_State_subCasNotSelected)
			|| (pstContext->ulCasInstanceId == ulCaInstanceId) )
		{
			svc_cas_ScenarioSearchCasInstance(pstContext, ulCaInstanceId, &ulSearchedCasInstId, &ulStopActionId, FALSE);
			HxLOG_Error(HxANSI_COLOR_YELLOW("[CAS INSTANCE][DEV] hAction = 0x%x, ulSearchedCasInstId = %d, ulStopActionId = %d")"\n", pstContext->hAction, ulSearchedCasInstId, ulStopActionId);

			if(ulSearchedCasInstId == HANDLE_NULL || (pstContext->ulCasInstanceId == ulSearchedCasInstId) )
			{
				HxLOG_Info("ulSearchedCasInstId (%d), pstContext->ulCasInstanceId (%d)\n", ulSearchedCasInstId, pstContext->ulCasInstanceId);
				continue;
			}

			// ���Ѿ��� instance�� connect �Ǿ� �ִ� ���¿��� �� instance�� ���ԵǾ��� �� disconnect�� ���� �ʴ´�.
			// TODO: scenario�� ������ ���̳� �ϴ��� �Ʒ��� ���� ������ �д�.
			if( (pstContext->ulCasInstanceId != HANDLE_NULL)
				&& (pstContext->ulCasInstanceId != ulSearchedCasInstId)
				&& (ulStopActionId == HANDLE_NULL ))
			{
				SvcCas_DevInstance_t *pstCasInst;

				HxLOG_Warning("W/A : Disconnect action...\n");

				pstCasInst = svc_cas_InstanceGetCasDevInst(pstContext->ulCasInstanceId);
				if(pstCasInst != NULL)
				{
					HUINT32 j;
					HUINT32 ulCurRefCnt;
					HUINT32 aulActionId[CAS_MAX_NUM_OF_ACTION];

					ulCurRefCnt = pstCasInst->ulCurRefCnt;
					for(j = 0; j < ulCurRefCnt; j++)
						aulActionId[j] = pstCasInst->aulActionId[j];

					for(j = 0; j < ulCurRefCnt; j++)
						svc_cas_MgrDisconnectService(aulActionId[j], FALSE);
				}
			}

			if( (ulSearchedCasInstId == HANDLE_NULL)
				|| (pstContext->ulCasInstanceId != ulSearchedCasInstId)
				|| (ulStopActionId != HANDLE_NULL) )
			{
				if(svc_cas_MgrConnectSevcie(pstContext, ulSearchedCasInstId, ulStopActionId) != ERR_OK)
				{
					HxLOG_Info("svc_cas_MgrConnectSevcie() error \n");
				}
			}
		}
	}

	#if defined(CONFIG_MW_CI) && defined(CONFIG_MW_CI_USE_EXTERNAL_CONTROLLER)
	/*	CI�� ������ �ȵǾ��� �� ~ path�� ������ �ֵ��� �Ѵ�.
		�ϳ��� ������ �Ǿ� ������ ci mgr�� ���ؼ� ����ǵ��� �Ѵ�.
	*/
	HxLOG_Info("svc_cas_InstanceFindNotConnectPathCiNConnect start() check device\n");
	svc_cas_InstanceFindNotConnectPathCiNConnect(ulSearchedCasInstId);
	#endif

	return ERR_OK;
}
#endif

STATIC HERROR svc_cas_MgrStartSiFilter(SvcCas_Context_t *pstCasContext)
{
	HERROR	hErr;

/* PMT filter start
	PMT�� ���� �ʰ� NIT, SDT other �����θ� ��ġ�� �Ϸ��ϴ� ��� pmtpid = null �� �� �ְ�
	�̷��� pmt filter start�� ���ϸ鼭 CAS ������ ���������� �̷������ �ʰ�, descramble�� �ȵǴ� ���� �߻�
	pmtpid = null �ΰ�� PAT filter�� �ɾ �ٽ� �������� ����
*/
	if(pstCasContext->usPmtPid == PID_NULL || pstCasContext->usPmtPid == 0)
	{
		hErr = svc_cas_SiStartPatFilter(pstCasContext);
		if(hErr != ERR_OK)
		{
			HxLOG_Error("Pat filter start failed \n");
		}
	}
	else
	{
		hErr = svc_cas_SiStartPmtFilter(pstCasContext);
		if(hErr != ERR_OK)
		{
			HxLOG_Error("Pmt filter start failed \n");
		}
	}

/* CAT filter start
*/
#if defined (CAS_SUPPORT_CAT)
	hErr = svc_cas_SiStartCatFilter(pstCasContext);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("Cat filter hasn't been started\n");
	}
#endif

#if defined (CAS_SUPPORT_SDT)
	// Sdt Filter Start
	// --> <TODO_CI_PLUS> : SDT�� ���� �Ŀ� Host Service Shunning �� ���� ������ ĳ���ϰ� �� ĳ�̵� ������ 7�ϵ��� �����ϸ� �� ���ȿ��� ���� ���͸� �� �ʿ䰡 ������ ������ �� !!
	hErr = svc_cas_SiStartSdtFilter(pstCasContext);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("Sdt filter hasn't been started \n");
	}
#endif

#if defined(CAS_SUPPORT_NIT)
	{
		HBOOL		bNitStart = TRUE;

	#if defined(CONFIG_MW_CAS_NAGRA_OP_MEDIASET)  || defined(CONFIG_MW_CAS_NAGRA_OP_SPAIN_TDT)
		if(pstCasContext->bNotTpChanged == TRUE)
		{
			bNitStart = FALSE;
		}
		else
		{
			pstCasContext->bNotTpChanged = TRUE;
		}
		#if defined(CONFIG_MW_CAS_NAGRA_OP_MEDIASET)
		MW_CA_NA_NotifySvcChanged(pstCasContext->hSvc, pstCasContext->hAction);
		#endif
	#endif

		if(bNitStart == TRUE)
		{
			hErr = svc_cas_SiStartNitFilter(pstCasContext);
			if(hErr != ERR_OK)
			{
				HxLOG_Error("Nit filter hasn't been started\n");
			}
		}
	}
#endif

#if defined(CAS_SUPPORT_BAT)
	{
		{
			hErr = svc_cas_SiStartBatFilter(pstCasContext);
			if(hErr != ERR_OK)
			{
				HxLOG_Error("Bat filter hasn't been started\n");
			}
		}
	}
#endif

#if defined(CAS_SUPPORT_EIT)
	hErr = svc_cas_SiStartEitFilter(pstCasContext);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("Eit filter hasn't been started\n");
	}
#endif

#if defined(CAS_SUPPORT_TDT)
	hErr = svc_cas_SiStartTdtFilter(pstCasContext);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("Tdt filter hasn't been started\n");
	}
#endif

	return ERR_OK;
}

STATIC HERROR svc_cas_MgrStopSiFilter(SvcCas_Context_t *pstCasContext)
{
	HERROR		hErr;

	hErr = svc_cas_SiStopPatFilter(pstCasContext);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("svc_cas_SiStopPatFilter's  \n");
	}
	pstCasContext->bPatUpdated = FALSE;

	hErr = svc_cas_SiStopPmtFilter(pstCasContext);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("svc_cas_SiStopPmtFilter's  \n");
	}
	pstCasContext->bPmtUpdated = FALSE;

#if defined (CAS_SUPPORT_CAT)
	hErr = svc_cas_SiStopCatFilter(pstCasContext);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("svc_cas_SiStopCatFilter's  \n");
	}
	pstCasContext->bCatUpdated = FALSE;
#endif

#if defined (CAS_SUPPORT_SDT)
	hErr = svc_cas_SiStopSdtFilter(pstCasContext);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("svc_cas_SiStopSdtFilter's  \n");
	}

	pstCasContext->bSdtUpdated = FALSE;
#endif

#if defined (CAS_SUPPORT_NIT)
	#if defined (CONFIG_MW_CAS_NAGRA)
		#if defined(CONFIG_MW_CAS_NAGRA_OP_MEDIASET)  || defined(CONFIG_MW_CAS_NAGRA_OP_SPAIN_TDT)
		svc_cas_MgrStopNitFilter4Nagra(pstCasContext, hNextSvc);
		#endif
	#else
	{
		{
			hErr = svc_cas_SiStopNitFilter(pstCasContext);
			if(hErr != ERR_OK)
			{
				HxLOG_Error("svc_cas_SiStopNitFilter's  \n");
			}
			svc_cas_SiFreeNitRaw(pstCasContext);
			pstCasContext->bNitUpdated = FALSE;
		}
	}
	#endif
#endif

#if defined (CAS_SUPPORT_BAT)
	{
		{
			hErr = svc_cas_SiStopBatFilter(pstCasContext);
			if(hErr != ERR_OK)
			{
				HxLOG_Error("svc_cas_SiStopBatFilter's  \n");
			}
			svc_cas_SiFreeBatRaw(pstCasContext);
			pstCasContext->bBatUpdated = FALSE;
		}
	}
#endif

#if defined (CAS_SUPPORT_EIT)
	hErr = svc_cas_SiStopEitFilter(pstCasContext);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("svc_cas_SiStopEitFilter's  \n");
	}
	svc_cas_SiFreeEitRaw(pstCasContext);
	pstCasContext->bEitUpdated = FALSE;
#endif

#if defined (CAS_SUPPORT_TDT)
	hErr = svc_cas_SiStopTdtFilter(pstCasContext);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("svc_cas_SiStopTdtFilter's  \n");
	}
	pstCasContext->bTdtUpdated = FALSE;
#endif

	return ERR_OK;
}

STATIC HERROR svc_cas_MgrStartService(Handle_t hAction, Handle_t hSvc, SvcCas_ActionType_e actType, SvcCas_StartData_t *pstCasStartData)
{
	HERROR			hErr;
	SvcCas_Context_t 	*pstCasContext = NULL;
#if defined(CONFIG_DEBUG)
	HxLOG_Info("ActionHandle(0x%x), SvcHandle(0x%x), ActType(%s) \n",
						hAction, hSvc, svc_cas_MgrMakeStrActionType(actType));
#endif
/* check action version */
	hErr = svc_cas_SvcPortRmCheckIfLatestActionHandle(hAction);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("Version\n");
		return ERR_FAIL;
	}

/* get Cas Action Context */
	pstCasContext = svc_cas_MgrGetActionContextByAction(hAction);
	if (hAction == HANDLE_NULL || pstCasContext == NULL)
	{
		HxLOG_Error("Failed get CasActionContext\n");
		return ERR_FAIL;
	}

/* fill Cas Action Context */
	hErr = svc_cas_MgrFillCasActionContext(hAction, hSvc, actType, pstCasStartData, pstCasContext);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("Failed Init Cas Action Context\n");
		return ERR_FAIL;
	}

/* start SI Filter */
	hErr = svc_cas_MgrStartSiFilter(pstCasContext);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("Failed start si filter\n");
		return ERR_FAIL;
	}

/* ��ϵ� ��� sub cas���� notify. */
	hErr = svc_cas_InstanceStartSession(pstCasContext);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("svc_cas_InstanceStartSession() error \n");
		return ERR_FAIL;
	}

/* State Change. */
	pstCasContext->state = eCAS_State_Started;

	return ERR_OK;
}

STATIC HERROR svc_cas_MgrStopService(Handle_t hAction, Handle_t hNextSvc, ApiSyncMode_t syncMode)
{
	HERROR			hErr;
	SvcCas_Context_t 	*pstCasContext = NULL;

	HxLOG_Info("hAction(0x%x), hNextSvc(0x%x) \n", hAction, hNextSvc);

/* check action version */
	hErr = svc_cas_SvcPortRmCheckIfLatestActionHandle(hAction);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("Version\n");
		goto Return;
	}

/* get Cas Action Context */
	pstCasContext = svc_cas_MgrGetActionContextByAction(hAction);
	if (hAction == HANDLE_NULL || pstCasContext == NULL)
	{
		HxLOG_Error("Failed get CasActionContext\n");
		hErr = ERR_FAIL;
		goto Return;
	}

	#if defined(CONFIG_DEBUG)
	HxLOG_Info("pstCasContext->eActType = %s \n", svc_cas_MgrMakeStrActionType(pstCasContext->eActType));
	#endif

/* State Check. Idle State �̿��� State�� ��� ó�� ����.
	 ���� ä�� ��ȯ�� �̹� stop�� ���� �ٽ� stop�Ǵ� normal�� ��Ȳ�̹Ƿ� ERR_OK �����ϵ��� �� (����μ� ERR_FAIL �����ص� ���� ó���ϴ� �� ����)
*/
	if(pstCasContext->state == eCAS_State_Idle)
	{
		#if defined (CONFIG_MW_CAS_NAGRA)
		#if defined(CONFIG_MW_CAS_NAGRA_OP_MEDIASET)  || defined(CONFIG_MW_CAS_NAGRA_OP_SPAIN_TDT)
		svc_cas_MgrStopNitFilter4Nagra(pstCasContext);
		#endif
		#endif
		goto Return;
	}

	/*
		����. action handle�� version�� start�� stop �� �ٸ���. stop�� start�� ���ؼ� higher version�� ������.
	*/
	pstCasContext->hAction = hAction;				/* version update�� �� stop requested */
	pstCasContext->hNextSvc = hNextSvc;			/* channel �������� ���� stop�� next service handle�� �����Ѵ�. �̰����� tp ���� ���θ� Ȯ�� ����. */

/* stop si filter
*/
	hErr = svc_cas_MgrStopSiFilter(pstCasContext);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("It's Failed to stop si filter\n");
	}

/* �ش� ���񽺿� ���õ� resource�� �����Ѵ�.
	1. ���õ� CAS�� disconnect���� �˸���.
	2. ���񽺰� stop�� �Ǿ����� ��� CAS���� �˸���.
	3. ���� �����  cas inst�� ������ ���´�.
	4. ���� ���¸� eCAS_State_Idle�� �ٲ۴�.
*/
	hErr = svc_cas_MgrDisconnectService(pstCasContext->ulActionId, TRUE);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("It's failed to disconnect service error, pstCasContext->ulActionId = %d\n", pstCasContext->ulActionId);
	}

Return:

	if(syncMode == eSyncMode)
	{
		VK_SEM_Release(s_ulCasMgrSyncSema);
	}
	else
	{
		/* �ش� Action�� ���� ���� �� GWM���� Event ������. */
		svc_cas_SvcPortUtilPostMsgToAp(eSEVT_CAS_STOPPED, hAction, 0, 0, 0);
	}

	return hErr;
}

STATIC HERROR svc_cas_MgrUpdatePmtInfo(SvcCas_Context_t	*pstCasContext, HUINT32 ulFilterId, SvcCas_SiTableState_e eSiTableState)
{
	HERROR		hErr;
	HUINT16		usDbPmtPid = PID_NULL;

	/*
			start session�ÿ� ���� ���� pmt pid�� service db's pmt pid�� �ٸ� ���.
			pmt update�� sync.
	*/
	if(	(pstCasContext->eActType == eCAS_ActionType_Live) ||
		(pstCasContext->eActType == eCAS_ActionType_Recording) ||
		(pstCasContext->eActType == eCAS_ActionType_Tsr_Rec) ||
		(pstCasContext->eActType == eCAS_ActionType_Data))
	{
		hErr = svc_cas_SvcPortDbGetPmtPidbyServiceHandle(pstCasContext->hSvc, &usDbPmtPid);
		if(hErr != ERR_OK)
		{
			HxLOG_Critical("Critical Eror!!!\n");
		}
	}
#if defined (CONFIG_MW_MM_PVR)
	else if(	(pstCasContext->eActType == eCAS_ActionType_Playback) ||
			(pstCasContext->eActType == eCAS_ActionType_Descramble_a_File) ||
			(pstCasContext->eActType == eCAS_ActionType_Tsr_Play))
	{
		SvcCas_PvrSvcInfo_t		stCasPvrSvcInfo;

		hErr = svc_cas_SvcPortPvrGetService(pstCasContext->hAction, &stCasPvrSvcInfo);
		if(hErr != ERR_OK)
		{
			HxLOG_Error("~~~ \n");
			return ERR_FAIL;
		}

		usDbPmtPid = stCasPvrSvcInfo.usPmtPid;
	}
#endif
	if(pstCasContext->usPmtPid != usDbPmtPid)
	{
		HxLOG_Error("PmtPid is different, ActContext(0x%x), in Db(0x%x) \n", pstCasContext->usPmtPid, usDbPmtPid);
		return ERR_FAIL;
	}

	if(pstCasContext->ulPmtFilterId != ulFilterId)
	{
		HxLOG_Error("pmt filter id is different with stored id.\n");
		return ERR_FAIL;
	}

/* pmt table state update */
	svc_cas_SiUpdateTableState(pstCasContext, eSI_TableType_PMT, eSiTableState);
	if(eSiTableState != eCASSI_Table_Received)
	{
		HxLOG_Error("pmt table state is wrong.\n");
		return ERR_FAIL;
	}

/* pmt info update */
	hErr = svc_cas_SiUpdatePmtInfo(pstCasContext, &pstCasContext->stPmtInfo);
	if(hErr != ERR_OK)
	{
		HxLOG_Critical("Critical Eror!!!\n");
	}

#if defined(CONFIG_MW_CAS_IRDETO_UCAS)
	hErr = svc_cas_MgrFastSetPidInfo(pstCasContext);
	if(hErr != ERR_OK)
	{
		HxLOG_Debug("Only support Recording Action\n");
	}
#endif

/*	2009.05.19. current next indicator�� zero�� ��� ó������ �ʵ��� ����.
	2009.06.11. Neotion �ǰ��� CI�� ��쿣 current next indicator�� 0�� ��쿡�� ķ�� �ڵ鸵�ϵ��� ca_pmt �� ķ�� �����ϴ� ���� �´ٶ�� ���ε� �ϴ� �� �ǰ��� ����.
*/
	if(pstCasContext->stPmtInfo.bCurNextInd == 0)
	{
		HxLOG_Print("pmt current next indicator is zero. \n");
		return ERR_FAIL;
	}

/* state update  */
	pstCasContext->bPmtUpdated = TRUE;
	svc_cas_MgrGetChannelType(pstCasContext, &pstCasContext->eChType);


	HxLOG_Print("\n\n --- Channel type [%s] : %s --- \n\n\n", svc_cas_DbgMakeStrChType(pstCasContext->eChType), svc_cas_DbgMakeStrContextState(pstCasContext->state)); // �߿� �α��̹Ƿ� �׽� ��� ����.


	return ERR_OK;
}

STATIC void svc_cas_MgrReconnectService(SvcCas_Context_t *pstCasContext, HUINT32 ulCasInstId)
{
	HINT32	i;
	HxLOG_Trace();

	if(ulCasInstId == HANDLE_NULL)
		return;

	if(pstCasContext->eActType != eCAS_ActionType_Live)
		return;

	for(i = 0 ; i < CAS_MAX_NUM_OF_ACTION ; i++)
	{
		if(&s_stCasActionContext[i] == pstCasContext)
			continue;

		if(s_stCasActionContext[i].hSvc != pstCasContext->hSvc)
			continue;

		if(s_stCasActionContext[i].state != eCAS_State_subCasNotSelected)
			continue;

		switch(s_stCasActionContext[i].eActType)
		{
			case eCAS_ActionType_Recording:
			case eCAS_ActionType_Tsr_Rec:
				if(svc_cas_MgrConnectSevcie(&s_stCasActionContext[i], ulCasInstId, HANDLE_NULL) != ERR_OK)
				{
					HxLOG_Error("svc_cas_MgrConnectSevcie() error, hAct(0x%x)\n", s_stCasActionContext[i].hAction);
				}
				break;

			default:
				break;
		}
	}
}

STATIC HERROR svc_cas_MgrUpdatePmt(Handle_t hAction, HUINT32 ulFilterId, SvcCas_SiTableState_e eSiTableState)
{
	HERROR			hErr;
	HUINT32			ulCasInstId = HANDLE_NULL;
	HUINT32			ulStopActionId = HANDLE_NULL;
	SvcCas_Context_t	*pstCasContext = NULL;


	HxLOG_Trace();

/* check action version */
	hErr = svc_cas_SvcPortRmCheckIfLatestActionHandle(hAction);
	if(hErr != ERR_OK)
	{
		// ���� ä�� ��ȯ�� normal�� ��Ȳ�̹Ƿ� ERR_OK �����ϵ��� �� (����μ� ERR_FAIL �����ص� ���� ó���ϴ� �� ����)
		HxLOG_Info("action handle wrong\n");
		return ERR_OK; // return ERR_FAIL;
	}

/* get Cas Action Context */
	pstCasContext = svc_cas_MgrGetActionContextByAction(hAction);
	if (pstCasContext == NULL)
	{
		HxLOG_Error("action context is null\n");
		return ERR_FAIL;
	}

/* check state */
	if(pstCasContext->state == eCAS_State_Idle)
	{
		HxLOG_Error("State is Idle, so we can not try to this command \n");
		return ERR_FAIL;
	}

/* update pmt info */
	hErr = svc_cas_MgrUpdatePmtInfo(pstCasContext, ulFilterId, eSiTableState);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("svc_cas_MgrUpdatePmtInfo() failed");
		return ERR_FAIL;
	}

/* Cas instance�� ã�´�.
	������ ��ϵ� �켱���� table�� cb�� �̿��Ͽ� cas instance�� ã�� ����.
	��)	CAS ���� ���� FTA�� ������ CAS �ó������� ���� ������ �� ���� �� �� ���� �ִ�.
		Scramble Channel�� �翬�� ������ Ca system id�� �����ϴ� ���� �켱 ������ ���� ���� ���̴�.
*/
	hErr = svc_cas_ScenarioSearchCasInstance(pstCasContext, HANDLE_NULL, &ulCasInstId, &ulStopActionId, TRUE);
	if(hErr == ERR_FAIL)
	{
		HxLOG_Error("svc_cas_ScenarioSearchCasInstance() fail, return! \n");
	}

	if(pstCasContext->bConnect == TRUE && pstCasContext->state == eCAS_State_CasOK && pstCasContext->ulCasInstanceId == ulCasInstId)
	{
		HxLOG_Info("PMT is updated. Selected sub CAS is same with before. Do not need to connect sub CAS again...\n");
	}
	else
	{
// TODO: swlee3 should be deleted
#if 0//defined(CONFIG_MW_CAS) && defined(CONFIG_MW_CAS_NAGRA) && defined(CONFIG_MW_CAS_NAGRA_OP_SES)// && defined(CONFIG_MW_CAS_NAGRA_OP_SES_CAKTEST)
		// nagra�� ��� ���� ä���� nagra�� �ƴ� �ٸ� CAS�� ��� ������ �޽����� ǥ���ؾ� ��. �׸��� �ٸ� �޽��� ��� ������ �߻��Ͽ� �߰������� �Լ� �߰�
		// ����� icord-mini�� ulCasInstId == HANDLE_NULL �ΰ�� ulCasInstId�� ���� ������.
		if(pstCasContext->eChType != eCasChannel_FTA && ulCasInstId == HANDLE_NULL)
		{
			svc_cas_PostProcessAfterScenarioSearchInstance(&ulCasInstId);
		}
#endif

		// �߿��� ������ �� �Ѹ����� �Ѵ�.
		HxLOG_Error(HxANSI_COLOR_YELLOW("[CAS INSTANCE][PMT] hAction = 0x%x, ulCasInstId = %d, ulStopActionId = %d")"\n", pstCasContext->hAction, ulCasInstId, ulStopActionId);

/* ã���� Cas instance�� action context�� ���� �� ���� �� sub cas���� notify��.
	�켱�������� �и� ulStopActionId�� stop�Ѵ�.
*/
		hErr = svc_cas_MgrConnectSevcie(pstCasContext, ulCasInstId, ulStopActionId);
		if(hErr != ERR_OK)
		{
			HxLOG_Error("svc_cas_MgrConnectSevcie() error \n");
			return ERR_FAIL;
		}

		/*	�켱������ ���� disconnect�� service�� �ٽ� ������ �ش�.
			subcas���� �Ѵٸ� casmgr�� sync�� ���� ���� ���ɼ��� �ִ� ������ ���� �۾��Ǿ� �ִ�. (resource ���� ��).*/
		svc_cas_MgrReconnectService(pstCasContext, ulCasInstId);

#if defined(CONFIG_MW_CI) && defined(CONFIG_MW_CI_USE_EXTERNAL_CONTROLLER)
		/*	CI�� ������ �ȵǾ��� �� ~ path�� ������ �ֵ��� �Ѵ�.
			�ϳ��� ������ �Ǿ� ������ ci mgr�� ���ؼ� ����ǵ��� �Ѵ�.
		*/
		HxLOG_Info("svc_cas_InstanceFindNotConnectPathCiNConnect start() pmt \n");
		svc_cas_InstanceFindNotConnectPathCiNConnect(ulCasInstId);
#endif
	}
/*	Record Action�� PMT ������ ����Ͽ� SetPid�� �����ϴµ�,
	MGR Action���� SetRecPid�� ������ �ؿ��� PMT ������ ������Ʈ �Ǿ� ���� ������ SetPid�� ƨ���.
	PMT ������Ʈ �� SetPid�� ��ü������ ������ �� �ֵ��� �Ѵ�.

	�ϴ�, CONAX�� SetPid�� �ΰ��ϰ� �����ϱ⿡ CONAX�� ���Ƶΰ�, �������� ���ؼ��� �۾��ϸ鼭 Ǯ���.
*/
#if defined(CONFIG_MW_CAS_CONAX) || defined(CONFIG_MW_CAS_VERIMATRIX)
	switch(pstCasContext->eActType)
	{
		case eCAS_ActionType_Recording:
		case eCAS_ActionType_Tsr_Rec:
			{
				SvcCas_SetPidInfo_t stSetPid;
				stSetPid.ulPidNum = 0;
				if(SVC_CAS_SetRecPid(hAction, pstCasContext->eActType, &stSetPid) != ERR_OK)
				{
					HxLOG_Error("SVC_CAS_SetRecPid() error \n");
				}
			}
			break;

		default:
			break;
	}
#endif

/* ��ϵ� ��� sub cas���� pmt update notify. */
	hErr = svc_cas_InstanceUpdatePmt(pstCasContext);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("CasMgrInstatce_UpdatePmt() error \n");
		return ERR_FAIL;
	}


	return ERR_OK;
}

STATIC HERROR svc_cas_MgrCompareSetPidInfo(SvcCas_Context_t *pstContext, SvcCas_SetPidInfo_t *pstCaDscInfo, HBOOL *pbIsChanged)
{
	HUINT32 i, j;
	SvcCas_SetPidInfo_t *pInfo = NULL;
	HBOOL bFindEsType = FALSE;

	HxLOG_Info(" \n");

	if(pstContext == NULL || pstCaDscInfo == NULL || pbIsChanged == NULL)
	{
		HxLOG_Error("arg error \n");
		return ERR_FAIL;
	}

	pInfo = &pstContext->stCaDscInfo;
	if(pInfo->ulPidNum != pstCaDscInfo->ulPidNum)
	{
		HxLOG_Info("pInfo->ulPidNum (%d) != pstCaDscInfo->ulPidNum (%d)\n", pInfo->ulPidNum, pstCaDscInfo->ulPidNum);

		*pbIsChanged = TRUE;
		return ERR_OK;
	}

	for(i = 0; i<pInfo->ulPidNum; i++)
	{
		bFindEsType = FALSE;
		for(j = 0; j<pstCaDscInfo->ulPidNum; j++)
		{
			if(pInfo->stPidInfo[i].esType == pstCaDscInfo->stPidInfo[j].esType)
			{
				if( (pstCaDscInfo->stPidInfo[j].usPid != PID_CURRENT) && (pInfo->stPidInfo[i].usPid != pstCaDscInfo->stPidInfo[j].usPid) )
				{
					HxLOG_Info("[%d].usPid %x != [%d].usPid %x\n", i, pInfo->stPidInfo[i].usPid, j, pstCaDscInfo->stPidInfo[j].usPid);

					*pbIsChanged = TRUE;
					return ERR_OK;
				}
				bFindEsType = TRUE;
			}
		}

		if(bFindEsType != TRUE)
		{
			HxLOG_Info("bFindEsType != TRUE\n");

			*pbIsChanged = TRUE;
			return ERR_OK;
		}
	}

	*pbIsChanged = FALSE;

	return ERR_OK;
}

HERROR svc_cas_MgrUpdateSetPidInfo(SvcCas_Context_t *pContext, SvcCas_ActionType_e eActType, SvcCas_SetPidInfo_t *pstCaDscInfo)
{
	HUINT32 i=0, ulPidNum = 0;
	SvcCas_SetPidInfo_t *pInfo = NULL;

	if(pContext == NULL || pstCaDscInfo == NULL)
	{
		return ERR_FAIL;
	}
#if defined(CONFIG_DEBUG)
	HxLOG_Print("action id (%d), action type (%s), pid num (%d)\n",
		pContext->ulActionId, svc_cas_MgrMakeStrActionType(eActType), pstCaDscInfo->ulPidNum);
#endif
	pInfo = &pContext->stCaDscInfo;

	/* Update�� ���� Action Type �̶�� �������� ó�� ���� �ϴ�. */

	if(pContext->eActType != eActType)
	{
		HxLOG_Error("different action type\n");
		return ERR_FAIL;
	}

	if( 		(eActType == eCAS_ActionType_Live)
		||	(eActType == eCAS_ActionType_Playback)
#ifdef CONFIG_MW_CAS_VIACCESS
		// tsr play�� playback action ������ ���� �� �ѵ�...�ϴ� cas define
		||	(eActType == eCAS_ActionType_Tsr_Play)
#endif
		)
	{
		if(pstCaDscInfo->ulPidNum != CAS_MAX_NUM_OF_LIVE_PID)
		{
			HxLOG_Error("num of pid[%d]. \n", pstCaDscInfo->ulPidNum);
			//return ERR_FAIL;
		}

#if defined(CONFIG_MW_CAS_IRDETO_UCAS)
		ulPidNum = pstCaDscInfo->ulPidNum;
#else
		ulPidNum = CAS_MAX_NUM_OF_LIVE_PID;
#endif
		for(i = 0; i<ulPidNum; i++)
		{
			pInfo->stPidInfo[i].esType = pstCaDscInfo->stPidInfo[i].esType;
			if(pstCaDscInfo->stPidInfo[i].usPid != PID_CURRENT)
			{
				pInfo->stPidInfo[i].usPid = pstCaDscInfo->stPidInfo[i].usPid;
			}
		}
		pInfo->ulPidNum = pstCaDscInfo->ulPidNum;
	}
	else
	{
		/* memcpy..���� */
		// 2. Pid ������ Context�� Update�Ѵ�.
		HxSTD_memset(&pContext->stCaDscInfo, 0x00, sizeof(SvcCas_SetPidInfo_t) * 1);
		HxSTD_memcpy(&pContext->stCaDscInfo, pstCaDscInfo, sizeof(SvcCas_SetPidInfo_t) * 1);
	}

	return ERR_OK;
}

STATIC HERROR svc_cas_MgrSetPid(Handle_t hAction, SvcCas_ActionType_e eActType, SvcCas_SetPidInfo_t *pstCaDscInfo)
{
	HBOOL			bIsUpdated;
	HERROR			hErr;
	SvcCas_Context_t	*pstCasContext = NULL;

/* check action version */
	hErr = svc_cas_SvcPortRmCheckIfLatestActionHandle(hAction);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("Version\n");
		return ERR_FAIL;
	}

/* get Cas Action Context */
	pstCasContext = svc_cas_MgrGetActionContextByAction(hAction);
	if (hAction == HANDLE_NULL || pstCasContext == NULL)
	{
		HxLOG_Error("Failed get CasActionContext\n");
		return ERR_FAIL;
	}

/* check state & filter id */
	if(pstCasContext->state == eCAS_State_Idle)
	{
		HxLOG_Error("State is Idle, so we can not try to this command \n");
		return ERR_FAIL;
	}

/* ���� Pid ������ �޶��� ���� �ִ��� Ȯ���Ѵ�.
	���� PID�� ������ �ƹ��͵� ���ص� �ȴ�~
*/
	hErr = svc_cas_MgrCompareSetPidInfo(pstCasContext, pstCaDscInfo, &bIsUpdated);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("svc_cas_MgrCompareSetPidInfo's error\n");
		return ERR_FAIL;
	}

	if(bIsUpdated != TRUE)
	{
		HxLOG_Print("Pid Not Changed !! \n");
		return ERR_OK;
	}

/* update set pid info*/
	hErr = svc_cas_MgrUpdateSetPidInfo(pstCasContext, eActType, pstCaDscInfo);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("svc_cas_MgrUpdateSetPidInfo's error\n");
		return ERR_FAIL;
	}

/* set pid sub cas�� ����.	*/
	hErr = svc_cas_InstanceSetDeScramble(pstCasContext);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("svc_cas_InstanceSetDeScramble's error\n");
		return ERR_FAIL;
	}

/* state update */
	if(pstCasContext->state == eCAS_State_Started)
	{
		pstCasContext->state = eCAS_State_SetPid;
	}

	return ERR_OK;
}

STATIC HERROR svc_cas_MgrUpdatePat(Handle_t hAction, HUINT32 ulFilterId, SvcCas_SiTableState_e eSiTableState)
{
	HERROR			hErr;
	SvcCas_Context_t 	*pstCasContext = NULL;

/* check action version */
	hErr = svc_cas_SvcPortRmCheckIfLatestActionHandle(hAction);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("Version\n");
		return ERR_FAIL;
	}

/* get Cas Action Context */
	pstCasContext = svc_cas_MgrGetActionContextByAction(hAction);
	if (hAction == HANDLE_NULL || pstCasContext == NULL)
	{
		HxLOG_Error("Failed get CasActionContext\n");
		return ERR_FAIL;
	}

	HxLOG_Info("action id(%d), filter id (%d), table state (%d) \n", pstCasContext->ulActionId, ulFilterId, eSiTableState);

/* check state & filter id */
	if(pstCasContext->state == eCAS_State_Idle)
	{
		HxLOG_Error("State is Idle, so we can not try to this command \n");
		return ERR_FAIL;
	}

	if(pstCasContext->ulPatFilterId != ulFilterId)
	{
		HxLOG_Error("pat filter id is different with stored id.\n");
		return ERR_FAIL;
	}

/* update pat info */
	svc_cas_SiUpdateTableState(pstCasContext, eSI_TableType_PAT, eSiTableState);
	if(eSiTableState != eCASSI_Table_Received)
	{
		HxLOG_Error("pmt table state is wrong.\n");
		return ERR_FAIL;
	}

	hErr = svc_cas_SiUpdatePatInfo(pstCasContext);
	if(hErr != ERR_OK)
	{
		HxLOG_Critical("Critical Eror!!!\n");
		return ERR_FAIL;
	}

/* 	start pmt filter
	PMT PID�� ������ �״� PMT�� �ɾ��. */
	hErr = svc_cas_SiStartPmtFilter(pstCasContext);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("svc_cas_SiStartPatFilter() \n");
	}

	return ERR_OK;
}

STATIC HERROR svc_cas_MgrCheckCasDevice(SvcCas_CheckDeviceMsg_t *pParam)
{
	HERROR					hErr;
#if defined(CONFIG_DEBUG)
	HxLOG_Info("%s \n", svc_cas_MgrMakeStrCamEvtType(pParam->eEvtType) );
#endif
/*
	ci :
		1. cam inserted �� cas info�� update�� �Ǹ� host�� support ca system id�� �����Ѵ�.
		2. cam removed �� host�� null ca system id�� �����Ѵ�.
			- ���� �ȿö� �´�. ��? Ȯ���ʿ�..
			- �̰� ������ svc_cas_InstanceUpdateCamState() ���� stop�� �ϰ� �������. �Ѥ�;

	embedded cas :
		1. smart card inserted �� registration�� �Ϸ�Ǹ� host�� support ca system id�� �����Ѵ�.
		2. smart card removed �� ��� host�� null ca system id�� �����Ѵ�.
*/
	if(pParam->eEvtType == eCAS_DEV_EVT_CasInfo)
	{
		HBOOL					bIsInserted = FALSE;
		HINT32					msg = eSEVT_CAS_SC_REMOVED;
		SvcCas_DevInstance_t 		*pstCasInst = NULL;

		hErr = svc_cas_InstanceUpdateCasInfo(pParam);
		if(hErr != ERR_OK)
		{
			HxLOG_Error("update instance table error.\n");
			return ERR_FAIL;
		}

		pstCasInst = svc_cas_InstanceGetCasDevInst(pParam->ulCaInstanceId);
		if(pstCasInst == NULL)
		{
			HxLOG_Error("instance id is wrong.\n");
			return ERR_FAIL;
		}

		if(pstCasInst->eCasInstType == eCasInst_Embedded)
		{
			bIsInserted = svc_cas_InstanceIsAdaptedSCInserted(pParam->ulCaInstanceId, pParam->usSlotId);
			if(bIsInserted == TRUE)
			{
				msg = eSEVT_CAS_SC_INSERTED;
			}
#if 0
			else
			{
				// SC �� ���ŵǾ��� �� �ش� SC ����� Service�� ���Ͽ� disconnect ����
				HUINT32 i;
				HUINT32 ulCurRefCnt;
				HUINT32 aulActionId[CAS_MAX_NUM_OF_ACTION];

				ulCurRefCnt = pstCasInst->ulCurRefCnt;

				for(i = 0; i < ulCurRefCnt; i++)
				{
					aulActionId[i] = pstCasInst->aulActionId[i];
				}
				for(i = 0; i < ulCurRefCnt; i++)
				{
					svc_cas_MgrDisconnectService(aulActionId[i], FALSE);
				}
			}
#endif
			svc_cas_SvcPortUtilPostMsgToAp(msg, 0, pParam->usSlotId, pstCasInst->eCasId, 0);
		}
		else if(pstCasInst->eCasInstType == eCasInst_CI)
		{
			if(pParam->utData.stCasInfo.ulNumCaSysId == 0)
			{
				HUINT32 i;
				HUINT32 ulCurRefCnt;
				HUINT32 aulActionId[CAS_MAX_NUM_OF_ACTION];

				ulCurRefCnt = pstCasInst->ulCurRefCnt;

				for(i = 0; i < ulCurRefCnt; i++)
				{
					aulActionId[i] = pstCasInst->aulActionId[i];
				}
				for(i = 0; i < ulCurRefCnt; i++)
				{
					svc_cas_MgrDisconnectService(aulActionId[i], FALSE);
				}
			}
		}

		/*	instance�� �߻��� ��� new instance�� descramble�� ������ action context�� ã�Ƽ� mapping�Ѵ�.
			instance�� ���ŵǾ��� ������  action context�� ã�Ƽ� dis-connect ���� �ʴ´�.
			-> sub cas���� �ش� ���¸� ��� �˰� �;� �Ҽ��� �ִ�.

			1. �켱 ��Ǯ���� �༮�� �߿��� �ش� CAS �� ���� �ߴ� ����� ���� �߾��ٴ� ����� �����.
				��? CAS������ ���� �Ǿ����� ��� �ٽ� �õ��� ���� �ϱ� ���÷�~
			2. ������ ��Ǯ���� �༮�� ã�Ƽ� ������ �õ��Ѵ�.
		*/
		// �ش� CAS �� ���� �ߴ� ����� ���� �߾��ٴ� ����� �����.
		svc_cas_MgrClearSubCasTryFlag(pstCasInst, pParam->ulCaInstanceId);

		//������ ��Ǯ���� �༮�� ã�Ƽ� ������ �õ��Ѵ�.
// TODO: swlee3 should be deleted
#if defined(CONFIG_MW_CAS_NAGRA_OP_SES_CAKTEST)
		HxLOG_Info("eCasGroupId = %s\n",pParam->eCasGroupId == eDxCAS_GROUPID_CAM ? "eDxCAS_GROUPID_CAM" : "ECAS");
		svc_cas_MgrSearchNotDesCasContextNConnect(pParam->ulCaInstanceId, pParam->eCasGroupId);
#else
		svc_cas_MgrSearchNotDesCasContextNConnect(pParam->ulCaInstanceId);
#endif
	}
#if defined(CONFIG_MW_CI)
	else if(pParam->eEvtType == eCAS_DEV_EVT_CamState)
	{
		hErr = svc_cas_InstanceUpdateCamState(pParam);
		if(hErr != ERR_OK)
		{
			return ERR_FAIL;
		}

		if(pParam->utData.eCamState == eCAS_CAM_STATE_Removed)
		{
			svc_cas_SvcPortUtilPostMsgToAp(eSEVT_CAS_CAM_REMOVED, 0, pParam->usSlotId, eDxCAS_GROUPID_CAM, 0);
		}
		else if(pParam->utData.eCamState == eCAS_CAM_STATE_Inserted)
		{
			svc_cas_SvcPortUtilPostMsgToAp(eSEVT_CAS_CAM_INSERTED, 0, pParam->usSlotId, eDxCAS_GROUPID_CAM, 0);
		}
		else if(pParam->utData.eCamState == eCAS_CAM_STATE_Initialized)
		{
			svc_cas_SvcPortUtilPostMsgToAp(eSEVT_CAS_CAM_INITIALIZED, 0, pParam->usSlotId, eDxCAS_GROUPID_CAM, 0);
		}
#if defined(CONFIG_DEBUG)
		HxLOG_Info("SlotId(%d), (%s) \n", pParam->usSlotId, svc_cas_MgrMakeStrCamState(pParam->utData.eCamState));
#endif
	}
	else if(pParam->eEvtType == eCAS_DEV_EVT_CamAppInfo)
	{
		HxLOG_Info("SlotId(%d), CamName (%s) \n", pParam->usSlotId, pParam->utData.stCasCiAppInfo.szText);

		hErr = svc_cas_InstanceUpdateCamAppInfo(pParam);
		if(hErr != ERR_OK)
		{
			return ERR_FAIL;
		}
	}
#endif
	else if(pParam->eEvtType == eCAS_DEV_EVT_ScState)
	{
		// TODO
	}
	else if(pParam->eEvtType == eCAS_DEV_EVT_ScInfo)
	{
		// TODO
	}
	else
	{
		HxLOG_Error("unknown event type.\n");
	}

	return ERR_OK;
}

// TODO: swlee3 should be deleted
#if defined(CONFIG_MW_CAS_NAGRA_OP_SES_CAKTEST)
STATIC HBOOL svc_cas_MgrUpdateSubCasMisMatchMsg(SvcCas_DevInstance_t	*pstCasInst, DxCAS_GroupId_e eCasId)
{
	HBOOL bMisMatchFlag = FALSE;

	if(pstCasInst == NULL)
	{
		HxLOG_Error("pstCasInst is NULL.\n");
		return bMisMatchFlag;
	}

	if( (pstCasInst->eCasInstType == eCasInst_Embedded) && (pstCasInst->eCasId != eCasId) )
	{
		HxLOG_Info("** ECAS :  Cas mis match <  pstCasInst->eCasId(%d),  pParam->eCasId(%d)\n", pstCasInst->eCasId, eCasId);
		bMisMatchFlag = TRUE;
	}
	else if ((pstCasInst->eCasInstType == eCasInst_CI) && (pstCasInst->eCasId != eCasId))
	{
		HxLOG_Info("** CAM :  Cas mis match <  pstCasInst->eCasId(%d),  pParam->eCasId(%d)\n", pstCasInst->eCasId, eCasId);
		bMisMatchFlag = TRUE;
	}

	return bMisMatchFlag;
}
#endif

STATIC HERROR svc_cas_MgrUpdateSubCasMsg(SvcCas_CheckSubCasMsg_t *pParam)
{
	SvcCas_Context_t		*pstCasContext = NULL;
	SvcCas_DevInstance_t	*pstCasInst = NULL;

/* check argument
*/
	HxLOG_Trace();

	if (pParam == NULL)
	{
		HxLOG_Error("pParam == NULL\n");
		return ERR_FAIL;
	}

	if (pParam->eMsgType >= eCAS_MSG_TYPE_Max || pParam->eMsgType == eCAS_MSG_TYPE_None)
	{
		HxLOG_Error("pParam->eMsgType (%d) error\n", pParam->eMsgType);
		return ERR_FAIL;
	}
#if defined(CONFIG_DEBUG)
	HxLOG_Print("param : hAction(0x%08x), (%s), (%s), ShowType(%d)\n",	pParam->hAction,
																	svc_cas_MgrMakeStrCasId(pParam->eCasId),
																	svc_cas_MgrMakeStrCasMsgType(pParam->eMsgType),
																	pParam->eShowType);
#endif
	pstCasContext = svc_cas_MgrGetActionContextByAction(pParam->hAction);
	if(pstCasContext == NULL)
	{
		HxLOG_Error("pstCasContext == NULL\n");
		return ERR_OK;
	}

// TODO: swlee3 should be deleted
#if defined(CONFIG_MW_CAS_NAGRA_OP_SES_CAKTEST)
// SES �� BDC�� eMsgType==eCAS_MSG_TYPE_Command�� ulCasInstanceId�� ������� display �����ؾ� ��
	if(pParam->eMsgType == eCAS_MSG_TYPE_Command)
	{
		svc_cas_SvcPortUtilPostMsgToAp(eSEVT_CAS_CMD, pParam->hAction, pParam->eCasId, (HINT32)pParam->pMsg, pParam->eShowType);
		return ERR_OK;
	}

	HxLOG_Print("pstCasContext->bPmtUpdated: %d, pstCasContext->eChType: %d\n",	pstCasContext->bPmtUpdated, pstCasContext->eChType);

	if(pstCasContext->bPmtUpdated == TRUE)
	{
		if(pstCasContext->eChType == eCasChannel_FTA || pstCasContext->eChType == eCasChannel_Unknown)
		{
			// FTA �� ��� �̸鼭 CAS OK, Check Event�� �÷� ������
			switch(pParam->eMsgType)
			{
				case eCAS_MSG_TYPE_Check:
				case eCAS_MSG_TYPE_Ok:
					break;

				default:
#if defined(CONFIG_DEBUG)
					HxLOG_Info(" channel type (%s) \n", svc_cas_MgrMakeStrChType(pstCasContext->eChType));
#endif
					return ERR_OK;
			}
		}
	}
	else
	{
		HxLOG_Error("\n");
		return ERR_FAIL;
	}
#endif

	if (pstCasContext->ulCasInstanceId == HANDLE_NULL)
	{
// TODO: swlee3 should be deleted
#if defined(CONFIG_MW_CAS_NAGRA_OP_SES_CAKTEST)
		/* smart card�� ���� ���¿����� �޽��� ó���� �ʿ��� */
		if(pParam->eCasId == eDxCAS_GROUPID_NA)
		{
			if (pParam->eMsgType == eCAS_MSG_TYPE_Ok)
			{
				pstCasContext->state = eCAS_State_CasOK;
				pstCasContext->bSendCasMsg2Ap  = TRUE;
				svc_cas_SvcPortUtilPostMsgToAp(eSEVT_CAS_OK, pParam->hAction, pParam->eCasId, 0, 0);
				return ERR_OK;
			}
			else if (pParam->eMsgType == eCAS_MSG_TYPE_Check)
			{
				svc_cas_SvcPortUtilPostMsgToAp(eSEVT_CAS_CHECK, pParam->hAction, pParam->eCasId, (HINT32)pParam->pMsg, pParam->eShowType);
				return ERR_OK;
			}
			else if (pParam->eMsgType == eCAS_MSG_TYPE_Fail)
			{
				goto SEND_CAS_FAIL_MSG;
			}
		}
//#else	// TODO: swlee3 should be deleted
		// �� ��쿡�� CAS scenario�� �����ؾ� �� �� ������ ulCasInstanceId�� NULL�ε� OK / CHECK�� �� ���� �ֳ�...?
		if (pParam->eMsgType == eCAS_MSG_TYPE_Fail)
		{
			goto _CHECK_CAS_SCENARIO_;
		}
		else if(pParam->eMsgType == eCAS_MSG_TYPE_Command)
		{
			svc_cas_SvcPortUtilPostMsgToAp(eSEVT_CAS_CMD, pParam->hAction, pParam->eCasId, (HINT32)pParam->pMsg, pParam->eShowType);
			return ERR_OK;
		}
#endif
		// �ش� case�� ������ ���� �ó����� ������ ��������. �⺻������ channel ��û�� �̷�� �� ��� FTA/CAS ������� instance�� �Ҵ��� �Ǿ� �־�� �Ѵ�.
		HxLOG_Info("pstCasContext->ulCasInstanceId HANDLE_NULL\n");
		return ERR_FAIL;
	}

	pstCasInst = svc_cas_InstanceGetCasDevInst(pstCasContext->ulCasInstanceId);
	if(pstCasInst == NULL)
	{
		HxLOG_Error("cannot find CasInst with pstCasContext->ulCasInstanceId: %d \n", pstCasContext->ulCasInstanceId);
		return ERR_FAIL;
	}
#if defined(CONFIG_DEBUG)
	HxLOG_Print("(%s)\n",	svc_cas_MgrMakeStrCasInstanceType(pstCasInst->eCasInstType));
#endif

// TODO: swlee3 should be deleted
#if !defined(CONFIG_MW_CAS_NAGRA_OP_SES_CAKTEST)
/* command�� ��쿡�� ���� check���� �ʰ� �ٷ� app�� �޽��� ������.
	���� EMM���� ���� �޽��� �ǰڴ�. �����ϰ� �������� ������ ���� �޽�����..
*/
	if(pParam->eMsgType == eCAS_MSG_TYPE_Command)
	{
		// TODO: postData�� ��ü. Ȯ���ʿ�. ������ �����. �� sub cas�� evtData �� Ʋ����, evtData �ȿ� void *pData �� �ִ� ��쵵 �ִ�.
		svc_cas_SvcPortUtilPostMsgToAp(eSEVT_CAS_CMD, pParam->hAction, pParam->eCasId, (HINT32)pParam->pMsg, pParam->eShowType);
		return ERR_OK;
	}

/* PMT�� ���� �ʾ����� ok, fail, check�޽��� ó�� ���� �ʴ´�. ��? PMT �𸣴µ� ���� CAS�� ���� �����߰ڴ°�?
	eCAS_MSG_TYPE_Command�� ���񽺿� ���� ���� �޽��� �̱� ������(EMM�� �˾Ƶ� �Ǵ� �༮��..) ������ ������
	�� 3���� ���񽺿� ����� �༮���̴�. (Ecm�� ���谡 �ִ�.) �ؼ� ������ �ȵȴ�.
*/

	HxLOG_Print("pstCasContext->bPmtUpdated: %d, pstCasContext->eChType: %d\n",	pstCasContext->bPmtUpdated, pstCasContext->eChType);

	if(pstCasContext->bPmtUpdated == TRUE)
	{
		if(pstCasContext->eChType == eCasChannel_FTA || pstCasContext->eChType == eCasChannel_Unknown)
		{
			// FTA �� ��� �̸鼭 CAS OK, Check Event�� �÷� ������
			switch(pParam->eMsgType)
			{
				case eCAS_MSG_TYPE_Check:
				case eCAS_MSG_TYPE_Ok:
					break;

				default:
#if defined(CONFIG_DEBUG)
					HxLOG_Info(" channel type (%s) \n", svc_cas_MgrMakeStrChType(pstCasContext->eChType));
#endif
					return ERR_OK;
			}
		}
	}
	else
	{
		HxLOG_Error("\n");
		return ERR_FAIL;
	}
#endif

// TODO: swlee3 should be deleted
#if defined(CONFIG_MW_CAS_NAGRA_OP_SES_CAKTEST)
	if(svc_cas_MgrUpdateSubCasMisMatchMsg(pstCasInst, pParam->eCasId) == TRUE)
	{
		HxLOG_Info("** Cas mis match Msg : Return\n");
		return ERR_OK;
	}
#endif

_CHECK_CAS_SCENARIO_:
	if(pParam->eMsgType == eCAS_MSG_TYPE_Ok)
	{
		pstCasContext->state = eCAS_State_CasOK;

		/*	CI�� ȭ�� show time�� ���̱� ���� ���� OK�� �ö� ���� ������ OK�� bSendCasMsg2Ap�� TRUE�� ���� �ʴ´�.
			������ CI�� �׳� MMI��.
		*/
#if 0
		if(pstCasInst->eCasInstType == eCasInst_Embedded)
		{
			pstCasContext->bSendCasMsg2Ap  = TRUE;
		}
#endif
		svc_cas_SvcPortUtilPostMsgToAp(eSEVT_CAS_OK, pParam->hAction, pParam->eCasId, 0, 0);
// TODO: swlee3 should be deleted
#if defined(CONFIG_MW_CAS_NAGRA_OP_SES_CAKTEST)
		return ERR_OK;
#endif
	}
	else if(pParam->eMsgType == eCAS_MSG_TYPE_Check)
	{
		// TODO: postData�� ��ü. Ȯ���ʿ�. ������ �����. �� sub cas�� evtData �� Ʋ����, evtData �ȿ� void *pData �� �ִ� ��쵵 �ִ�.
		svc_cas_SvcPortUtilPostMsgToAp(eSEVT_CAS_CHECK, pParam->hAction, pParam->eCasId, (HINT32)pParam->pMsg, pParam->eShowType);
// TODO: swlee3 should be deleted
#if defined(CONFIG_MW_CAS_NAGRA_OP_SES_CAKTEST)
		return ERR_OK;
#endif
	}

// TODO: swlee3 should be deleted
#if defined(CONFIG_MW_CAS_NAGRA_OP_SES_CAKTEST)
	if(pParam->eMsgType == eCAS_MSG_TYPE_Fail)
#else
	else if(pParam->eMsgType == eCAS_MSG_TYPE_Fail)
#endif
	{
		/* 	eCAS_MSG_TYPE_Fail�� �޴� �ٰ� ������ fail �޽����� ap�� ������ �ʴ´�.
				�ٸ� CAS�� ������ �ִ��� Ȯ�� �ؾ߱� ������ �ٸ� CAS�� �õ��� ���� fail �޽����� ������.
				-> ó�� �޴� eCAS_State_CasOK, eCAS_MSG_TYPE_Fail�� �������� ������ �ִ��� ������ �Ǵ��Ѵ�.
				-> �߰��� ������ �������� ����(eCAS_MSG_TYPE_Fail) �ٽ� �õ����� �ʴ´�. �װ� ������ bSendCasMsg2Ap ������ ����.
				-> �õ��� ���� ���� �༮�� ������ ã�Ƽ� �õ��� ����.
				-> ���� ��� CAS�� �õ��� ������ ���� ���� �켱������ CAS�� �����ϵ��� �Ѵ�. -> �ش� CAS�� error msg�� ������ �Ѵ�. �Ѥ�;
		*/
#if 0
		if(pstCasContext->bSendCasMsg2Ap == TRUE)
		{
			goto SEND_CAS_FAIL_MSG;
		}
		else
#endif
		{
			HUINT32		ulNumOfCasInst;
			HUINT32		ulSearchedCasInstId = HANDLE_NULL;
			HUINT32		ulStopActionId = HANDLE_NULL;
			HERROR		hErr;

			/* Sub Cas�� �� ���� ���� �ٷ� Fail �޽����� ������. */
			ulNumOfCasInst = svc_cas_InstanceGetNumOfCasDevInst();
			if(ulNumOfCasInst == 1)
			{
				goto SEND_CAS_FAIL_MSG;
			}
// TODO: swlee3 should be deleted
#if defined(CONFIG_MW_CAS_NAGRA_OP_SES_CAKTEST)
			/*
				���� CI�� ���� ���ε� CAS FAIL MSG�� �ö�� ��� ���� �߻�. CAS + CI ���ΰ�� ������. ex) CI�� desceamble ���ε� cas�� �����ϸ� live�� stop�Ǵ� ����߻�.
				Fail Msg�� �ö�ö�°�� casInstType�� casId�� ��ġ���� ������ �ϴ� return����.
				( search�� �ٽ��ϰԵǸ� ��û���� ������ ������ �߻������� )
			*/
			if(svc_cas_MgrUpdateSubCasMisMatchMsg(pstCasInst, pParam->eCasId) == TRUE)
			{
				HxLOG_Info("** Cas mis match Msg : Return\n");
				return ERR_OK;
			}
			else
#endif
			{
			// �õ��� ���� ���� �༮�� �ִ��� ã�� ����.
			svc_cas_ScenarioSearchCasInstance(pstCasContext, HANDLE_NULL, &ulSearchedCasInstId, &ulStopActionId, FALSE);
			}

			HxLOG_Error(HxANSI_COLOR_YELLOW("[CAS INSTANCE][MSG] hAction = 0x%x, ulCasInstId = %d, ulStopActionId = %d")"\n", pstCasContext->hAction, ulSearchedCasInstId, ulStopActionId);

			// ���� �ɸ��� �켱 ������ ���� ���� ���� ã�Ƽ� �׳༮�� fail �޽����� �������� �Ѵ�.
			if(ulSearchedCasInstId == HANDLE_NULL)
			{
				// ��� Cas Inst���� �켱 ������ ���� ���� ���� ã�´�.
				hErr = svc_cas_ScenarioSearchCasInstance(pstCasContext, HANDLE_NULL, &ulSearchedCasInstId, &ulStopActionId, TRUE);
				HxLOG_Error(HxANSI_COLOR_YELLOW("[CAS INSTANCE][MSG] hAction = 0x%x, ulCasInstId = %d, ulStopActionId = %d")"\n", pstCasContext->hAction, ulSearchedCasInstId, ulStopActionId);
				if(hErr != ERR_OK || ulSearchedCasInstId == HANDLE_NULL)
				{
					HxLOG_Info("svc_cas_ScenarioSearchCasInstance fail, or ulSearchedCasInstId==NULL \n");
					goto SEND_CAS_FAIL_MSG;
				}

				if(pstCasInst->eCasId != pParam->eCasId)
				{
					HxLOG_Info("ignore message, selecte instance(0x%x), message instance(0x%x)\n", pstCasInst->eCasId, pParam->eCasId);
					return ERR_OK;
				}

				HxLOG_Info("send CAS message...(0x%x)\n", pParam->eCasId);
				goto SEND_CAS_FAIL_MSG;
#if 0
				//������ ���õ� ���ϰ� �켱 ���� ���� ���� �༮�ϰ� ������ ��.. �ٷ� fail�� �������� �Ѵ�.
				if(pstCasContext->ulCasInstanceId == ulSearchedCasInstId)
				{
					HxLOG_Info("pstCasContext->ulCasInstanceId == ulSearchedCasInstId \n");
					goto SEND_CAS_FAIL_MSG;
				}
//#else	// TODO: swlee3 should be deleted
				if(pstCasInst != NULL)
				{
					if(pstCasInst->eCasId == pParam->eCasId)
					{
						HxLOG_Info("pstCasInst->eCasId == pParam->eCasId\n");
						goto SEND_CAS_FAIL_MSG;
					}
				}
#endif
				// ������ ���� CAS �޽����� AP�� ������ ������.
//				pstCasContext->bSendCasMsg2Ap = TRUE;
			}

			if(pstCasInst == NULL)
			{
				HxLOG_Info("pstCasInst is NULL ====>  \n");
				goto SEND_CAS_FAIL_MSG;
			}

			/*	������ sub cas�� ����� ���� ���� �� �ٽ� �����Ѵ�.
				live�� TSR���� ���� ���߰� �ٽ� �����ϵ��� �Ѵ�.
			*/
			if(pstCasContext->eActType == eCAS_ActionType_Live)
			{
				HUINT32			i;
				HUINT32 			ulCurRefCnt;
				HUINT32 			aulActionId[CAS_MAX_NUM_OF_ACTION];
				SvcCas_Context_t	*pstTryCasContext;

				ulCurRefCnt = pstCasInst->ulCurRefCnt;

				for(i = 0; i < ulCurRefCnt; i++)
				{
					aulActionId[i] = pstCasInst->aulActionId[i];
				}

				for(i = 0; i < ulCurRefCnt; i++)
				{
					svc_cas_MgrDisconnectService(aulActionId[i], FALSE);
				}

				for(i = 0; i < ulCurRefCnt; i++)
				{
					pstTryCasContext = svc_cas_MgrGetActionContextByActionId(aulActionId[i]);

					// �õ� �� ���� �༮�� �ִ�. or ��� ���� �ϰ� �켱 ������ ���� ���� �༮�� fail �޽����� ������ ���� �ٽ� �õ��Ѵ�..
					hErr = svc_cas_MgrConnectSevcie(pstTryCasContext, ulSearchedCasInstId, ulStopActionId);
					if(hErr != ERR_OK)
					{
						HxLOG_Info("svc_cas_MgrConnectSevcie() error \n");
					}

					//�켱 ���������� �з��� stop�� �ѹ��� stop�ϸ� �Ǳ� ������ �ٷ� HANDLE_NULL�� �ٲ۴�.
					ulStopActionId = HANDLE_NULL;
				}
			}
			else
			{
				svc_cas_MgrDisconnectService(pstCasContext->ulActionId, FALSE);

				// �õ� �� ���� �༮�� �ִ�. or ��� ���� �ϰ� �켱 ������ ���� ���� �༮�� fail �޽����� ������ ���� �ٽ� �õ��Ѵ�..
				hErr = svc_cas_MgrConnectSevcie(pstCasContext, ulSearchedCasInstId, ulStopActionId);
				if(hErr != ERR_OK)
				{
					HxLOG_Info("svc_cas_MgrConnectSevcie() error \n");
				}
			}

			#if defined(CONFIG_MW_CI) && defined(CONFIG_MW_CI_USE_EXTERNAL_CONTROLLER)
			/*	CI�� ������ �ȵǾ��� �� ~ path�� ������ �ֵ��� �Ѵ�.
				�ϳ��� ������ �Ǿ� ������ ci mgr�� ���ؼ� ����ǵ��� �Ѵ�.
			*/
			HxLOG_Info("svc_cas_InstanceFindNotConnectPathCiNConnect start() update msg\n");
			svc_cas_InstanceFindNotConnectPathCiNConnect(ulSearchedCasInstId);
			#endif
		}
	}

	return ERR_OK;

SEND_CAS_FAIL_MSG:
	pstCasContext->state				= eCAS_State_CasFail;
//	pstCasContext->bSendCasMsg2Ap	= TRUE;
	// TODO: postData�� ��ü. Ȯ���ʿ�. ������ �����. �� sub cas�� evtData �� Ʋ����, evtData �ȿ� void *pData �� �ִ� ��쵵 �ִ�.
	svc_cas_SvcPortUtilPostMsgToAp(eSEVT_CAS_FAIL, pParam->hAction, pParam->eCasId, (HINT32)pParam->pMsg, pParam->eShowType);
	return ERR_OK;
}

#if defined (CAS_SUPPORT_SDT)
STATIC HERROR local_CasMain_UpdateSDT(Handle_t hAction, HUINT16 usUniqueId, HUINT32 ulFilterId, SvcCas_SiTableState_e eSiTableState)
{
	HERROR			hErr;
	SvcCas_Context_t	*pContext = NULL;

	HxLOG_Info(" action handle (0x%08x)\n", hAction);

	pContext = svc_cas_MgrGetActionContextByAction(hAction);
	if(pContext == NULL)
	{
		HxLOG_Critical("\n");
		return ERR_FAIL;
	}
#if defined(CONFIG_DEBUG)
	HxLOG_Info("hAction(0x%x), (%s) \n", pContext->hAction, svc_cas_MgrMakeStrSiTblState(eSiTableState));
#endif
	if (pContext->ulSdtFilterId == CAS_FILTER_ID_NULL)
	{
		HxLOG_Critical("\n");
		return ERR_FAIL;
	}

	// Filter Id check.
	if(pContext->ulSdtFilterId != ulFilterId)
	{
		HxLOG_Error("Sdt filter id is different with stored id.\n");
		return ERR_FAIL;
	}

	pContext->bSdtUpdated = TRUE;
	pContext->eSdtState = eSiTableState;

	hErr = svc_cas_SiUpdateSdtRawData(pContext);
	if(hErr != ERR_OK)
	{
		HxLOG_Critical("\n");
		return ERR_FAIL;
	}

	hErr = svc_cas_InstanceUpdateSdt(pContext);
	if(hErr != ERR_OK)
	{
		HxLOG_Critical("\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}
#endif // CAS_SUPPORT_SDT

#if defined (CAS_SUPPORT_CAT)
HERROR svc_cas_MgrUpdateCAT(Handle_t hAction, HUINT32 ulFilterId, SvcCas_SiTableState_e eSiTableState)
{
	SvcCas_Context_t *pContext = NULL;
	HUINT32 nResult = 0;

	HxLOG_Print("[%s:%d] \n",__FUNCTION__, __LINE__);

	if (hAction == HANDLE_NULL)
	{
		HxLOG_Error("action handle is null\n");
		return ERR_FAIL;
	}

	nResult = svc_cas_SvcPortRmCheckIfLatestActionHandle(hAction);
	if(nResult != ERR_OK)
	{
		HxLOG_Print("[%s:%d] action version.\n",__FUNCTION__, __LINE__);
		return ERR_OK; // return ERR_FAIL;
	}

	pContext = svc_cas_MgrGetActionContextByAction(hAction);
	if (pContext == NULL)
	{
		HxLOG_Error("action context is null\n");
		return ERR_FAIL;
	}

	HxLOG_Info("action id(%d), filter id (%d), table state (%d) \n",
		pContext->ulActionId, ulFilterId, eSiTableState);

	if(pContext->state == eCAS_State_Idle)
	{
		HxLOG_Error("State is Idle, so we can not try to this command \n");
		return ERR_FAIL;
	}

	if(pContext->ulCatFilterId != ulFilterId)
	{
		HxLOG_Error("cat filter id is different with stored id.\n");
		return ERR_FAIL;
	}


	svc_cas_SiUpdateTableState(pContext, eSI_TableType_CAT, eSiTableState);
	if(eSiTableState != eCASSI_Table_Received)
	{
		HxLOG_Error("cat table state is wrong.\n");
		return ERR_FAIL;
	}

	nResult = svc_cas_SiUpdateCatInfo(pContext);
	if(ERR_OK!=nResult)			HxLOG_Critical("Critical Eror!!!\n");



	nResult = svc_cas_InstanceUpdateCat(pContext);
	if(nResult != ERR_OK)
	{
		HxLOG_Error("CasMgrInstatce_UpdatePmt() error \n");
		return ERR_FAIL;
	}

	return ERR_OK;

}
#endif

STATIC HERROR svc_cas_MgrCheckMmi(SvcCas_CheckMmiMsg_t *pstCheckMmi)
{
	switch(pstCheckMmi->eDataType)
	{
	case eCasMmiType_Open:
		svc_cas_SvcPortUtilPostMsgToAp(eSEVT_CAS_MMI_OPEN, (Handle_t)NULL, pstCheckMmi->usSlotId, eDxCAS_GROUPID_CAM, pstCheckMmi->ulMmiVer);
		break;
	case eCasMmiType_Close:
		svc_cas_SvcPortUtilPostMsgToAp(eSEVT_CAS_MMI_CLOSE, (Handle_t)NULL, pstCheckMmi->usSlotId, eDxCAS_GROUPID_CAM, pstCheckMmi->ulMmiVer);
		break;
	case eCasMmiType_Menu:
		svc_cas_SvcPortUtilPostMsgToAp(eSEVT_CAS_MMI_MENU, (Handle_t)NULL, pstCheckMmi->usSlotId, eDxCAS_GROUPID_CAM, pstCheckMmi->ulMmiVer);
		break;
	case eCasMmiType_List:
		svc_cas_SvcPortUtilPostMsgToAp(eSEVT_CAS_MMI_LIST, (Handle_t)NULL, pstCheckMmi->usSlotId, eDxCAS_GROUPID_CAM, pstCheckMmi->ulMmiVer);
		break;
	case eCasMmiType_Text:
		svc_cas_SvcPortUtilPostMsgToAp(eSEVT_CAS_MMI_TEXT, (Handle_t)NULL, pstCheckMmi->usSlotId, eDxCAS_GROUPID_CAM, pstCheckMmi->ulMmiVer);
		break;
	case eCasMmiType_Enq:
		svc_cas_SvcPortUtilPostMsgToAp(eSEVT_CAS_MMI_ENQ, (Handle_t)NULL, pstCheckMmi->usSlotId, eDxCAS_GROUPID_CAM, pstCheckMmi->ulMmiVer);
		break;

	case eCasMmiType_None:
	default:
		HxLOG_Error("wrong mmitype(%d) \n", pstCheckMmi->eDataType);
		break;
	}

	return ERR_OK;
}

STATIC HERROR svc_cas_MgrGetDrmNotAvailableMessage(DxCopyrightControl_CasCcType_e eDrmType, HUINT32 *pulMsg, HINT32 *pnParam1)
{
	HERROR		hErr = ERR_FAIL;

	if (pulMsg == NULL)
	{
		return ERR_FAIL;
	}

	switch(eDrmType)
	{
	case eDxCopyrightControl_CasCcType_CiPlus:
		*pulMsg = eSEVT_CAS_DRM_UNKNOWN;
		if (pnParam1) *pnParam1 = eDxCopyrightControl_CasCcType_CiPlus;
		hErr = ERR_OK;
		break;
	case eDxCopyrightControl_CasCcType_NagraSes:
	case eDxCopyrightControl_CasCcType_Irdeto:
	case eDxCopyrightControl_CasCcType_Viaccess:
	case eDxCopyrightControl_CasCcType_Conax:
		*pulMsg = eSEVT_CAS_DRM_NONE;
		hErr = ERR_OK;
		break;
	default:
		break;
	}

	return hErr;
}

HERROR svc_cas_MgrUpdateDrmInfo(SvcCas_CheckDrmInfo_t *pParam)
{
	HINT32				p1 = 0;
	HUINT32				ulMsg;
	HERROR				hErr;
	SvcCas_Context_t		*pContext = NULL;
	SvcCas_DevInstance_t	*pCaInst = NULL;

	if(pParam == NULL)
	{
		HxLOG_Error("\n");
		return ERR_FAIL;
	}

	pContext = svc_cas_MgrGetActionContextByAction(pParam->hAction);
	if (pContext == NULL)
	{
		HxLOG_Error("action handle is invalid. hAction(0x%08x \n", pParam->hAction);
		return ERR_FAIL;
	}

	if(pContext->ulCasInstanceId != pParam->ulCaInstIdx)
	{
		HxLOG_Error("ca instance id is different with context's instance. so, message canceled. \n");
		return ERR_OK;
	}

	pCaInst = svc_cas_InstanceGetCasDevInst(pContext->ulCasInstanceId);
	if(pCaInst == NULL)
	{
		HxLOG_Error("instance table id is wrong. \n");
		return ERR_FAIL;
	}

	if (pParam->pData == NULL)
	{
		HxLOG_Print(" DRM Info. is not available. \n");
#if defined(CONFIG_MW_CAS_NAGRA_OP_SES_CAKTEST)
		// cak7 - CAK-MER-MetadataCopyCtrl test �� �����ִ� �ڵ�
#if defined(CONFIG_DEBUG)
		HxLOG_Print(">>> no copy control is activated \n");
		HxLOG_Print("[METADATA] eTrickModeCtrl(0)SvcBlock(0) DigiCpProtect(0) AnaCpProtect(0) eStorageRight(0) bStorageEncrypt(0) eCompVidCtrl(0) bTransCtrl(0)) bBlucomCtrl(0)\n");
#else
		MW_CA_NA_PrintForRelease(">>> no copy control is activated \n");
		MW_CA_NA_PrintForRelease("[METADATA] eTrickModeCtrl(0)SvcBlock(0) DigiCpProtect(0) AnaCpProtect(0) eStorageRight(0) bStorageEncrypt(0) eCompVidCtrl(0) bTransCtrl(0)) bBlucomCtrl(0)\n");
#endif
#endif
		hErr = svc_cas_MgrGetDrmNotAvailableMessage(pParam->eDrmType, &ulMsg, &p1);
		if (hErr != ERR_OK)
		{
			HxLOG_Error("svc_cas_MgrGetDrmNotAvailableMessage() fail \n");
			return ERR_FAIL;
		}

		svc_cas_SvcPortUtilPostMsgToAp(ulMsg, pParam->hAction, p1, 0, 0);
	}
	else
	{
#if 0//defined(CONFIG_MW_CAS_NAGRA_OP_SES)
		// cak7 - CAK-MER-MetadataCopyCtrl test �� �����ִ� �ڵ�
		HxLOG_Print(" [METADATA] eTrickModeCtrl(0)SvcBlock(0) DigiCpProtect(0) AnaCpProtect(0) eStorageRight(0) bStorageEncrypt(0) eCompVidCtrl(0) bTransCtrl(0)) bBlucomCtrl(0)\n");
		hErr = svc_cas_MgrGetDrmNotAvailableMessage(pParam->eDrmType, &ulMsg, &p1);
		if (hErr != ERR_OK)
		{
			HxLOG_Error("svc_cas_MgrGetDrmNotAvailableMessage() fail \n");
			return ERR_FAIL;
		}
		svc_cas_SvcPortUtilPostMsgToAp(ulMsg, pParam->hAction, p1, 0, 0);
#else
		ulMsg = eSEVT_CAS_DRM_SIGNALLED;
		p1 = (HINT32)pParam->pData;

		svc_cas_SvcPortUtilPostDataToAp(ulMsg, pParam->hAction, 0, 0, (void*)p1, sizeof(DxCopyrightControl_t));
#endif
	}



	return ERR_OK;
}

STATIC HERROR svc_cas_MgrNotifyRecordOperation(SvcCas_NotifyRecordOperationMsg_t *pstRecOperation)
{
//	svc_cas_SvcPortUtilPostMsgToAp(EVT_CAS_REC, pstRecOperation->hAction, pstRecOperation->eCasId, pstRecOperation->ucRecStatus, pstRecOperation->usSlotId);
	switch(pstRecOperation->eCasId)
	{
		case eDxCAS_GROUPID_CAM:
			if(pstRecOperation->ucRecStatus == 0)		// TODO: ucRecStatus�� enum���� ó���ؾ� �ϰ� �ٲ�� ���� �κе� ���� ���� �ʿ�.
			{
				svc_cas_SvcPortUtilPostMsgToAp(eSEVT_CAS_CIPLUS_REC_PAUSE, (Handle_t)NULL, pstRecOperation->usSlotId, eDxCAS_GROUPID_CAM, pstRecOperation->hAction);
			}
			else if(pstRecOperation->ucRecStatus == 1)
			{
				svc_cas_SvcPortUtilPostMsgToAp(eSEVT_CAS_CIPLUS_REC_START, (Handle_t)NULL, pstRecOperation->usSlotId, eDxCAS_GROUPID_CAM, pstRecOperation->hAction);
			}
			break;

		default:
			HxLOG_Error("Wrong CAS ID (%d) \n", pstRecOperation->eCasId);
			return ERR_FAIL;
			break;
	}

	return ERR_OK;
}

STATIC HERROR svc_cas_MgrCheckStream(SvcCas_SimpleMsg_t *pstMsg)
{
#if defined(CONFIG_MW_CI)
	HINT32 i;

	// CAM�� descrambling ���� ���δ� AV decoding ���ηθ� üũ�ϰ� �ִ�.
	if(pstMsg->version != eCAS_NOTI_TYPE_VIDEO || pstMsg->param != eVIDEO_EVENT_FRAME_START)
	{
		HxLOG_Debug("version(%d), param(%d)\n", pstMsg->version, pstMsg->param);
		return ERR_OK;
	}

	for(i = 0 ; i < CAS_MAX_NUM_OF_ACTION ; i++)
	{
		if(s_stCasActionContext[i].state < eCAS_State_subCasSelected)
		{
			continue;
		}

		if(s_stCasActionContext[i].eActType == eCAS_ActionType_Live || s_stCasActionContext[i].eActType == eCAS_ActionType_Tsr_Rec)
		{
			s_stCasActionContext[i].state = eCAS_State_CasOK;

			// AV state�� underrun���� decoding���� �ٲ����. CI������ �̹� CAS_OK �����̳� �ٽ� �����д�.
			svc_cas_SvcPortUtilPostMsgToAp(eSEVT_CAS_OK, s_stCasActionContext[i].hAction, eDxCAS_GROUPID_CAM, 0, 0);
		}
	}
#endif

	return ERR_OK;
}

STATIC void svc_cas_MgrHandleMsg(SvcCas_Message_t *casMsg)
{
	HERROR hErr = ERR_OK;
#if defined(CONFIG_DEBUG)
	HxLOG_Info("Command : [%s] \n", svc_cas_MgrCmd_MakeStrCasCmd(casMsg->command));
#endif
	switch (casMsg->command)
	{
		case CMD_CAS_START:
			hErr = svc_cas_MgrStartService(casMsg->start.hAction, casMsg->start.hSvc, casMsg->start.actionType, &casMsg->start.stCasStartData);
			if(hErr != ERR_OK)	HxLOG_Critical("Critical Eror!!!\n");
			break;

		case CMD_CAS_SETPID:
			hErr = svc_cas_MgrSetPid(casMsg->setPid.hAction, casMsg->setPid.actionType, &casMsg->setPid.stEsPidInfo);
			if(hErr != ERR_OK)	HxLOG_Critical("Critical Eror!!!\n");
			break;

		case CMD_CAS_STOP:
			hErr = svc_cas_MgrStopService(casMsg->stop.hAction, casMsg->stop.hNextSvc, casMsg->stop.syncMode);
			if(hErr != ERR_OK)	HxLOG_Critical("Critical Eror!!!\n");
			break;

		case CMD_CAS_CHECKDEVICE:
			hErr = svc_cas_MgrCheckCasDevice(&casMsg->checkDev);
			if(hErr != ERR_OK)	HxLOG_Critical("Critical Eror!!!\n");
			break;

		case CMD_CAS_CHECKEVENT:
			hErr = svc_cas_MgrUpdateSubCasMsg(&casMsg->checkMsg);
			if(hErr != ERR_OK)	HxLOG_Critical("Critical Eror!!!\n");
			break;
/* SI */
		case CMD_CAS_CHECKPAT:
			hErr = svc_cas_MgrUpdatePat(casMsg->checkSiTbl.hAction, casMsg->checkSiTbl.ulFilterId, casMsg->checkSiTbl.eSiTableState);
			if(hErr != ERR_OK)	HxLOG_Critical("Critical Eror!!!\n");
			break;

		case CMD_CAS_CHECKPMT:
			hErr = svc_cas_MgrUpdatePmt(casMsg->checkSiTbl.hAction, casMsg->checkSiTbl.ulFilterId, casMsg->checkSiTbl.eSiTableState);
			if(hErr != ERR_OK)	HxLOG_Critical("Critical Eror!!!\n");
			break;

		#if defined(CAS_SUPPORT_CAT)
		case CMD_CAS_CHECKCAT:
			hErr = svc_cas_MgrUpdateCAT(casMsg->checkSiTbl.hAction, casMsg->checkSiTbl.ulFilterId, casMsg->checkSiTbl.eSiTableState);
			if(hErr != ERR_OK)	HxLOG_Critical("Critical Eror!!!\n");
			break;
		#endif

		#if defined(CAS_SUPPORT_SDT)
		case CMD_CAS_CHECKSDT:
			hErr = local_CasMain_UpdateSDT(casMsg->checkSiTbl.hAction, casMsg->checkSiTbl.usUniqueId, casMsg->checkSiTbl.ulFilterId, casMsg->checkSiTbl.eSiTableState);
			if(hErr != ERR_OK)	HxLOG_Critical("Critical Eror!!!\n");
			break;
		#endif

		#if defined(CAS_SUPPORT_NIT)
		case CMD_CAS_CHECKNIT:
			break;
		#endif

		#if defined(CAS_SUPPORT_EIT)
		case CMD_CAS_CHECKEIT:
			break;
		#endif

		#if defined(CAS_SUPPORT_TDT)
		case CMD_CAS_CHECKTDT:
			break;
		#endif

		#if defined(CAS_SUPPORT_BAT)
		case CMD_CAS_CHECKBAT:
			break;
		#endif

/* MMI */
		case CMD_CAS_CHECKMMI:
			hErr = svc_cas_MgrCheckMmi(&casMsg->checkMmi);
			if(hErr != ERR_OK)	HxLOG_Critical("Critical Eror!!!\n");
			break;

/* STREAM */
		case CMD_CAS_CHECKSTREAM:
			hErr = svc_cas_MgrCheckStream(&casMsg->simple);
			if(hErr != ERR_OK)	HxLOG_Critical("Critical Eror!!!\n");
			break;

/* DRM */
		case CMD_CAS_CHECKDRM:
			svc_cas_MgrUpdateDrmInfo(&casMsg->checkDrm);
			break;

/* PPV */
		case CMD_CAS_SC_PPVREPLY:
			break;

/* SC MSG */
		case CMD_CAS_SC_PINCODEREPLY:
			break;

		case CMD_CAS_SC_UDATAREADREPLY:
			break;

		case CMD_CAS_ETCEVENT:
			break;

/* Recording */
		case CMD_CAS_NOTIREC:
			hErr = svc_cas_MgrNotifyRecordOperation(&casMsg->notifyRecOperation);
			if(hErr != ERR_OK)	HxLOG_Critical("Critical Eror!!!\n");
			break;

		default :
			HxLOG_Info("Default Message\n");
			break;

	}
}

STATIC void svc_cas_MgrTask(void *v)
{
	HUINT32 			nResult = 0;
	HERROR			hErr = ERR_FAIL;
	SvcCas_Message_t 	stCasMsg;

	hErr = svc_cas_MgrInitModule();
	if(hErr != ERR_OK)
	{
		HxLOG_Critical("svc_cas_MgrInitModule() Error !!!\n");
		return;
	}

	hErr = svc_cas_MgrSubCasInit();
	if(hErr != ERR_OK)
	{
		HxLOG_Critical("sub Cas init Error !!!\n");
		return;
	}

	for(;;)
	{
		nResult = VK_MSG_Receive (s_ulCasMgrMsgQId, &stCasMsg, sizeof(SvcCas_Message_t));
		if(nResult == ERR_OK && stCasMsg.command < CMD_CAS_MAX)
		{
			svc_cas_MgrHandleMsg(&stCasMsg);
		}
		else
		{
			HxLOG_Info("GetMessage Error !!! : 0x%X\n", nResult);
			continue;
		}
	}

	return;
}


/********************************************************************
* Module Internal Functions
* Naming rule : "CasMgrMain" _ "Function Name"
********************************************************************/
#define ___07_Module_Internal_APIs___

HERROR svc_cas_MgrInit(void)
{
	HUINT32		unResult;

// Create Task
	unResult = (HUINT32)VK_TASK_Create (svc_cas_MgrTask,
									MW_CAS_TASK_PRIORITY,
									MW_CAS_TASK_STACK_SIZE,
									"tCASIx",
									NULL,
									&s_ulCasMgrTaskId, 0);

	if (unResult != ERR_OK)
	{
		HxLOG_Error("TASK_CREATE FAIL \n");
		return ERR_FAIL;
	}

// Create Message Queue
	unResult = (HUINT32)VK_MSG_Create(MW_CAS_MSGQ_SIZE, sizeof(SvcCas_Message_t), "CasMsgQ", (unsigned long *)&s_ulCasMgrMsgQId, VK_SUSPENDTYPE_PRIORITY);
	if (unResult != ERR_OK)
	{
		HxLOG_Error("MSG_CREATE FAIL \n");
		return ERR_FAIL;
	}

// Create Semaphore
	unResult = VK_SEM_Create(&s_ulCasMgrSyncSema,"cas_cmd_sem", VK_SUSPENDTYPE_PRIORITY);
	if (unResult != ERR_OK)
	{
		HxLOG_Error("Creation Error: s_ulCasMgrSyncSema sema \n");
		return ERR_FAIL;
	}
	else
	{
		VK_SEM_Get(s_ulCasMgrSyncSema); /* Get �س��� ������ sync API ������ �ȵ�. */
	}

// task start
	VK_TASK_Start(s_ulCasMgrTaskId);

	return ERR_OK;
}


HULONG svc_cas_MgrGetMsgQueueId(void)
{
	return s_ulCasMgrMsgQId;
}

HULONG svc_cas_MgrGetCasSyncSemaId(void)
{
	return s_ulCasMgrSyncSema;
}

HERROR svc_cas_MgrGetRequestedCamIdByActionId(HUINT32 ulActionId, HUINT8 *pucCamId)
{
	SvcCas_Context_t	*pstCasContext;

	if(pucCamId == NULL)
	{
		return ERR_FAIL;
	}

	pstCasContext = svc_cas_MgrGetActionContextByActionId(ulActionId);
	if(pstCasContext == NULL)
	{
		return ERR_FAIL;
	}

	if(pstCasContext->stCasStartData.eCasGroupId == eDxCAS_GROUPID_CAM)
	{
		HxSTD_memcpy(pucCamId, pstCasContext->stCasStartData.uData.stCasCamStartData.aucCAMID, 8);
		return ERR_OK;
	}

	return ERR_FAIL;
}

/********************************************************************
* For Debugging
********************************************************************/
#define ___08_Debugging_APIs___

#if defined(CONFIG_DEBUG)
#if 0
STATIC const HINT8 *svc_cas_MgrMakeStrCasShowType(SvcCas_ShowType_e cmd)
{
	switch (cmd)
	{
		ENUM_TO_STR(eCasShowType_None);
		ENUM_TO_STR(eCasShowType_AudioOnly);
		ENUM_TO_STR(eCasShowType_VideoOnly);
		ENUM_TO_STR(eCasShowType_Both);
		ENUM_TO_STR(eCasShowType_Max);
	}
	return ("Unknown");
}
#endif

STATIC const HINT8 *svc_cas_MgrMakeStrCasMsgType(SvcCas_MsgType_e cmd)
{
	switch (cmd)
	{
		ENUM_TO_STR(eCAS_MSG_TYPE_None);
		ENUM_TO_STR(eCAS_MSG_TYPE_Ok);
		ENUM_TO_STR(eCAS_MSG_TYPE_Fail);
		ENUM_TO_STR(eCAS_MSG_TYPE_Command);
		ENUM_TO_STR(eCAS_MSG_TYPE_Check);
		ENUM_TO_STR(eCAS_MSG_TYPE_Max);
	}
	return ("Unknown");
}

STATIC const HINT8 *svc_cas_MgrMakeStrCasId(DxCAS_GroupId_e cmd)
{
	switch (cmd)
	{
		ENUM_TO_STR(eDxCAS_GROUPID_NONE);
		ENUM_TO_STR(eDxCAS_GROUPID_MG);
		ENUM_TO_STR(eDxCAS_GROUPID_VA);
		ENUM_TO_STR(eDxCAS_GROUPID_IR);
		ENUM_TO_STR(eDxCAS_GROUPID_CX);
		ENUM_TO_STR(eDxCAS_GROUPID_CR);
		ENUM_TO_STR(eDxCAS_GROUPID_NA);
		ENUM_TO_STR(eDxCAS_GROUPID_ND);
		ENUM_TO_STR(eDxCAS_GROUPID_SATSA);
		ENUM_TO_STR(eDxCAS_GROUPID_COMMON);
		ENUM_TO_STR(eDxCAS_GROUPID_CAM);
		default: return "unknown";

	}
	return ("Unknown");
}


STATIC const HINT8 *svc_cas_MgrMakeStrCasInstanceType(SvcCas_InstanceType_e eType)
{
	switch (eType)
	{
		ENUM_TO_STR(eCasInst_None);
		ENUM_TO_STR(eCasInst_Embedded);
		ENUM_TO_STR(eCasInst_CI);
		ENUM_TO_STR(eCasInst_Max);
	}
	return ("Unknown");
}

STATIC const HINT8 *svc_cas_MgrMakeStrCamEvtType(SvcCas_DevEvtType_e eType)
{
	switch (eType)
	{
		ENUM_TO_STR(eCAS_DEV_EVT_CamState);
		ENUM_TO_STR(eCAS_DEV_EVT_CamAppInfo);
		ENUM_TO_STR(eCAS_DEV_EVT_CamCasInfo);
		ENUM_TO_STR(eCAS_DEV_EVT_CamPlusInfo);
		ENUM_TO_STR(eCAS_DEV_EVT_ScState);
		ENUM_TO_STR(eCAS_DEV_EVT_ScInfo);
		ENUM_TO_STR(eCAS_DEV_EVT_CasInfo);
	}
	return ("Unknown");
}

STATIC const HINT8 *svc_cas_MgrMakeStrSiTblState(SvcCas_SiTableState_e eState)
{
	switch (eState)
	{
		ENUM_TO_STR(eCASSI_Table_Skipped);
		ENUM_TO_STR(eCASSI_Table_Waiting);
		ENUM_TO_STR(eCASSI_Table_Received);
		ENUM_TO_STR(eCASSI_Table_Timeout);
		ENUM_TO_STR(eCASSI_Table_Running);
		ENUM_TO_STR(eCASSI_Table_Fail);
	}
	return ("Unknown");
}

STATIC const HINT8 * svc_cas_MgrMakeStrContextState(SvcCas_State_e eState)
{
	switch (eState)
	{
		ENUM_TO_STR(eCAS_State_Idle);
		ENUM_TO_STR(eCAS_State_Started);
		ENUM_TO_STR(eCAS_State_SetPid);
		ENUM_TO_STR(eCAS_State_subCasNotSelected);
		ENUM_TO_STR(eCAS_State_subCasSelected);
		ENUM_TO_STR(eCAS_State_CasOK);
		ENUM_TO_STR(eCAS_State_CasFail);
		ENUM_TO_STR(eCAS_State_Max);
	}

	return ("Unknown");
}

STATIC const HINT8 *svc_cas_MgrMakeStrChType(SvcCas_ChannelType_e eType)
{
	switch (eType)
	{
		ENUM_TO_STR(eCasChannel_Unknown);
		ENUM_TO_STR(eCasChannel_FTA);
		ENUM_TO_STR(eCasChannel_Scramble);
	}
	return ("Unknown");
}

STATIC const HINT8 *svc_cas_MgrMakeStrActionType(SvcCas_ActionType_e cmd)
{
	switch (cmd)
	{
		ENUM_TO_STR(eCAS_ActionType_None);
		ENUM_TO_STR(eCAS_ActionType_Live);
		ENUM_TO_STR(eCAS_ActionType_Playback);
		ENUM_TO_STR(eCAS_ActionType_Recording);
		ENUM_TO_STR(eCAS_ActionType_Tsr_Play);
		ENUM_TO_STR(eCAS_ActionType_Tsr_Rec);
		ENUM_TO_STR(eCAS_ActionType_Descramble_a_File);
		ENUM_TO_STR(eCAS_ActionType_Data);
		ENUM_TO_STR(eCAS_ActionType_EngineeringTS);
		ENUM_TO_STR(eCAS_ActionType_RfVod);
		ENUM_TO_STR(eCAS_ActionType_Max);
	}
	return ("Unknown");
}

STATIC const HINT8 *svc_cas_MgrMakeStrEsType(EsType_t eType)
{
	switch (eType)
	{
		ENUM_TO_STR(eEsType_None);
		ENUM_TO_STR(eEsType_Video);
		ENUM_TO_STR(eEsType_Audio);
		ENUM_TO_STR(eEsType_PCR);
		ENUM_TO_STR(eEsType_Subtitle);
		ENUM_TO_STR(eEsType_Teletext);
		ENUM_TO_STR(eEsType_ClosedCaption);
		ENUM_TO_STR(eEsType_SuperImpose);
		ENUM_TO_STR(eEsType_Data);
		ENUM_TO_STR(eEsType_Unknown);
		ENUM_TO_STR(eEsType_Reserved);
		ENUM_TO_STR(eEsType_All);
		ENUM_TO_STR(eEstype_Max);
	}

	return ("Unknown");
}

#if defined(CONFIG_MW_CI)
STATIC const HINT8 *svc_cas_MgrMakeStrCamState(SvcCas_CamState_e eState)
{
	switch (eState)
	{
		ENUM_TO_STR(eCAS_CAM_STATE_Removed);
		ENUM_TO_STR(eCAS_CAM_STATE_Inserted);
		ENUM_TO_STR(eCAS_CAM_STATE_Initializing);
		ENUM_TO_STR(eCAS_CAM_STATE_Initialized);
	}
	return ("Unknown");
}
#endif

STATIC HERROR svc_cas_MgrPrintPmtElementInfo(SvcCas_PmtInfo_t *pstPmtInfo)
{
	int i, j;
	HUINT32 ulEsCnt = 0;
	SvcCas_PmtElement_t *pElem = NULL;

	if(pstPmtInfo == NULL)
	{
		return ERR_FAIL;
	}

	ulEsCnt = pstPmtInfo->ulNumOfElement;

	HLIB_CMD_Printf("\nwe have (%d) pmt element information \n", ulEsCnt);

	for(i=0; i<ulEsCnt; i++)
	{
		pElem = &pstPmtInfo->stElementInfo[i];
		HLIB_CMD_Printf("\n(%d) - element ----------\n\n", i+1);
		HLIB_CMD_Printf("StreamType			(0x%x) \n", pElem->ucStreamType);
		HLIB_CMD_Printf("EsType				(%s) \n", svc_cas_MgrMakeStrEsType(pElem->eEsType));
		HLIB_CMD_Printf("Pid				(0x%x) \n", pElem->usPid);
		HLIB_CMD_Printf("IsCurrent			(%d) \n", pElem->bIsCurrent);
		HLIB_CMD_Printf("Flag of element ca flag		(%d) \n", pElem->bElementCaFlag);
		HLIB_CMD_Printf("number of element ca descriptor	(%d) \n", pElem->ulNumOfCaDescriptor);

		for(j=0; j<pElem->ulNumOfCaDescriptor; j++)
		{
			HLIB_CMD_Printf("    [%d] -ca sys id		(0x%x)\n", j, pElem->stElementCaDescriptor[j].usCaSysId);
			HLIB_CMD_Printf("    [%d]- ca pid		(0x%x)\n", j, pElem->stElementCaDescriptor[j].usCaPid);
		}
	}

	return ERR_OK;
}

STATIC HERROR svc_cas_MgrPrintPmtInfo(SvcCas_PmtInfo_t *pstPmtInfo)
{
	int i = 0;

	if(pstPmtInfo == NULL)
	{
		HLIB_CMD_Printf(" \n");
		return ERR_FAIL;
	}

	HLIB_CMD_Printf("\n=============== pmt info ===============\n\n");

	HLIB_CMD_Printf("section state			(%d) \n", pstPmtInfo->ulSectionState);
	HLIB_CMD_Printf("service id			(0x%x) \n", pstPmtInfo->usServiceId);
	HLIB_CMD_Printf("pcr pid				(0x%x) \n", pstPmtInfo->usPcrPid);
	HLIB_CMD_Printf("version number			(%d) \n", pstPmtInfo->ucVerNum);
	HLIB_CMD_Printf("current next indicator		(%d) \n", pstPmtInfo->bCurNextInd);

	HLIB_CMD_Printf("crc value			(0x%x) \n", pstPmtInfo->ulCrc);
	HLIB_CMD_Printf("flag of global ca descriptor	(%d) \n", pstPmtInfo->bProgramCaFlag);
	HLIB_CMD_Printf("number of global ca descriptor	(%d) \n", pstPmtInfo->ulNumOfProgramCaDescriptor);

	for(i=0; i<pstPmtInfo->ulNumOfProgramCaDescriptor; i++)
	{
		HLIB_CMD_Printf("global ca descriptor[%d], ca sys id (0x%x), ca pid (0x%x) \n", i, pstPmtInfo->stProgramCaDescriptor[i].usCaSysId, pstPmtInfo->stProgramCaDescriptor[i].usCaPid);
	}

	svc_cas_MgrPrintPmtElementInfo(pstPmtInfo);

	HLIB_CMD_Printf("\n============ end of pmt info ===========\n");

	return ERR_OK;
}

STATIC void svc_cas_MgrPrintInfoAll(void)
{
	svc_cas_MgrPrintActionContextList();
	svc_cas_InstancePrintCaInstTable();
	svc_cas_ScenarioPrintPriorityTable();

#if defined(CONFIG_MW_CI)
	//ycgo �� �κ� ���� �Ұ� .. -> �ٷ� pal �θ��� �ȵȴ�.
	PAL_CI_PrintTsPathAll();
	PAL_CI_PrintDemuxPath();
	PAL_CI_PrintCIStatus();
#endif
	return;
}

STATIC void svc_cas_MgrPrintCaInstTableById(HUINT32 ulCaInstanceId)
{
	svc_cas_InstancePrintCaInstTableById(ulCaInstanceId);
	return;
}

STATIC void svc_cas_MgrPrintCaInstTable(void)
{
	svc_cas_InstancePrintCaInstTable();
	return;
}

// TODO: swlee3 should be deleted
#if defined(CONFIG_MW_CAS_NAGRA_OP_SES_CAKTEST)
STATIC void svc_cas_PrintMgrContextInfo(HUINT32 i, SvcCas_Context_t	*pstContext, HBOOL bVideoDecoding, HUINT32 ulCaInstanceId, DxCAS_GroupId_e eCasGroupId)
{
	SvcCas_DevInstance_t *pstCasInstTmp = NULL;

	if(pstContext == NULL)
	{
		return;
	}

	HxLOG_Info("Current video status(%d)\n", bVideoDecoding);
	HxLOG_Info("\n[%d] hAction(0x%x)\n\teCasGroupId(%s)\n\tulCaInstanceId(%d)\n\tulCasInstanceId(%d)\n\tbConnect(%d)\n\tstate(%d)\n\teActType(%d)\n\tbPmtUpdated(%d)\n\teChType(%d)\n\tusCurCaSysId(0x%x)\n\tbDscStarted(%d)\n\tabTryCasInst[0] = %d, abTryCasInst[1] = %d\n\tulPmtFilterId(0x%x)\n\tusPmtPid(0x%x)\n\tbPmtUpdated(%d)\n\tePmtState(%d)\n",
		i,
		pstContext->hAction,
		eCasGroupId == eDxCAS_GROUPID_CAM ? "eDxCAS_GROUPID_CAM" : "ECAS",
		ulCaInstanceId,
		pstContext->ulCasInstanceId,
		pstContext->bConnect,
		pstContext->state,
		pstContext->eActType,
		pstContext->bPmtUpdated,
		pstContext->eChType,
		pstContext->usCurCaSysId,
		pstContext->bDscStarted,
		pstContext->abTryCasInst[0],
		pstContext->abTryCasInst[1],
		pstContext->ulPmtFilterId,
		pstContext->usPmtPid,
		pstContext->bPmtUpdated,
		pstContext->ePmtState);

	HxLOG_Info("\tCaDscInfo : ulPidNum(%d)\n", pstContext->stCaDscInfo.ulPidNum);
	HxLOG_Info("\tStarted(%d)\n", pstContext->bStarted);
	HxLOG_Info("\teCasGroupId(0x%x)\n", pstContext->stCasStartData.eCasGroupId);
	HxLOG_Info("\t------------------------------------------\n");
	HxLOG_Info("\tulCasInstanceId[%d]\n", pstContext->ulCasInstanceId);

	pstCasInstTmp = svc_cas_InstanceGetCasDevInst(pstContext->ulCasInstanceId);
	if(pstCasInstTmp)
	{
		HxLOG_Info("\tpstCasInstTmp : ok\n");
		HxLOG_Info("\t\teCasInstType(%d)\n", pstCasInstTmp->eCasInstType);
		HxLOG_Info("\t\teCasId(0x%x)\n", pstCasInstTmp->eCasId);
		HxLOG_Info("\t\tulSlot(%d)\n", pstCasInstTmp->ulSlot);
		HxLOG_Info("\t\tbAllocated(%d)\n", pstCasInstTmp->bAllocated);
		HxLOG_Info("\t\tbUsed(%d)\n", pstCasInstTmp->bUsed);
		HxLOG_Info("\t\teLastNotify(%d)\n", pstCasInstTmp->eLastNotify);
		HxLOG_Info("\t\tulNumOfSlot(%d)\n", pstCasInstTmp->ulNumOfSlot);
		HxLOG_Info("\t\tbHaveDevice[1] = %d\n", pstCasInstTmp->bHaveDevice[1]);
		HxLOG_Info("\t\tbHaveDevice[2] = %d\n", pstCasInstTmp->bHaveDevice[2]);
		HxLOG_Info("\t\tulNumOfMaxSvc(%d)\n", pstCasInstTmp->ulNumOfMaxSvc);
		HxLOG_Info("\t\tulCurRefCnt(%d)\n", pstCasInstTmp->ulCurRefCnt);
		HxLOG_Info("\t\taulActionId[0](0x%x)\n", pstCasInstTmp->aulActionId[0]);
		HxLOG_Info("\t\taulActionId[1](0x%x)\n", pstCasInstTmp->aulActionId[1]);
		HxLOG_Info("\t\teCAM :  CamState(%d)\n", pstCasInstTmp->eCamState);
		HxLOG_Info("\t\tCAM :  eCiAppType(0x%x)\n", pstCasInstTmp->stCamAppInfo.eCiAppType);
		HxLOG_Info("\t\tCAM :  usManufacturer(%d)\n", pstCasInstTmp->stCamAppInfo.usManufacturer);
		HxLOG_Info("\t\tCAM :  usManufacturerCode(%d)\n", pstCasInstTmp->stCamAppInfo.usManufacturerCode);
		HxLOG_Info("\t\tCAM :  szText(%s)\n", pstCasInstTmp->stCamAppInfo.szText);
		HxLOG_Info("\t\tCAM :  ucIsInitialized(%d)\n", pstCasInstTmp->stCamAppInfo.ucIsInitialized);
	}
	else
	{
		HxLOG_Info("\tpstCasInstTmp : is not ok\n");
	}
}
#endif

STATIC void svc_cas_MgrPrintActionContextByActionId(HUINT32 ulActionId)
{
	int idx2 = 0;
	SvcCas_Context_t *pContext = NULL;

	if(ulActionId >= CAS_MAX_NUM_OF_ACTION)
	{
		return;
	}

	pContext = svc_cas_MgrGetActionContextByActionId(ulActionId);
	if(pContext == NULL || pContext->state == eCAS_State_Idle)
	{
		HLIB_CMD_Printf("%d-th context state is (%s) \n", ulActionId, svc_cas_MgrMakeStrContextState(pContext->state));
		return;
	}

	HLIB_CMD_Printf ("\n----------- print action context [Id:%d] --------------- \n", ulActionId);
	HLIB_CMD_Printf ("\n");

	HLIB_CMD_Printf ("Context Number		: %d\n", ulActionId);
	HLIB_CMD_Printf ("state			: %s\n", svc_cas_MgrMakeStrContextState(pContext->state));
	HLIB_CMD_Printf ("Action Id		: %d\n", pContext->ulActionId);
	HLIB_CMD_Printf ("Action Handle		: 0x%x\n", pContext->hAction);
	HLIB_CMD_Printf ("Service Handle		: 0x%x\n", pContext->hSvc);
	HLIB_CMD_Printf ("usOrgNetId		: 0x%x\n", pContext->usOrgNetId);
	HLIB_CMD_Printf ("usTsId			: 0x%x\n", pContext->usTsId);
	HLIB_CMD_Printf ("usSvcId			: 0x%x\n", pContext->usSvcId);
	HLIB_CMD_Printf ("usUniqueId		: 0x%x\n", pContext->usUniqueId);
	HLIB_CMD_Printf ("Channel Type 		: %s\n", svc_cas_MgrMakeStrChType(pContext->eChType));
	HLIB_CMD_Printf ("Action Type		: %s\n", svc_cas_MgrMakeStrActionType(pContext->eActType));
	HLIB_CMD_Printf ("\nEsPid Num		: %d\n", pContext->stCaDscInfo.ulPidNum);

	for(idx2 = 0; idx2 < pContext->stCaDscInfo.ulPidNum; idx2++)
	{
		HLIB_CMD_Printf("PidInfo[%d], 		(%s) (0x%x)\n",
			idx2, svc_cas_MgrMakeStrEsType(pContext->stCaDscInfo.stPidInfo[idx2].esType), pContext->stCaDscInfo.stPidInfo[idx2].usPid);
	}

	HLIB_CMD_Printf ("\nCurrent CaSysId		: 0x%x\n", pContext->usCurCaSysId);
	HLIB_CMD_Printf ("Current InstanceId	: %d\n", pContext->ulCasInstanceId);

	HLIB_CMD_Printf ("bStarted 		: %d\n", pContext->bStarted);
	HLIB_CMD_Printf ("bDscStarted		: %d\n", pContext->bDscStarted);
	HLIB_CMD_Printf ("bPatUpdated		: %d\n", pContext->bPatUpdated);
	HLIB_CMD_Printf ("bPmtUpdated		: %d\n", pContext->bPmtUpdated);
#if defined(CAS_SUPPORT_CAT)
	HLIB_CMD_Printf ("bCatUpdated		: %d\n", pContext->bCatUpdated);
#endif
	HLIB_CMD_Printf ("bConnect 		: %d\n", pContext->bConnect);

	HLIB_CMD_Printf ("Pmt State		: %s\n", svc_cas_MgrMakeStrSiTblState(pContext->ePmtState));
#if defined(CAS_SUPPORT_CAT)
	HLIB_CMD_Printf ("Cat State		: %s\n", svc_cas_MgrMakeStrSiTblState(pContext->eCatState));
#endif
	HLIB_CMD_Printf ("Pmt Pid			: 0x%x\n", pContext->usPmtPid);
	HLIB_CMD_Printf ("Pmt FilterId 		: %d\n", pContext->ulPmtFilterId);

	svc_cas_MgrPrintPmtInfo(&pContext->stPmtInfo);

	HLIB_CMD_Printf ("\n");
	HLIB_CMD_Printf ("\n------------------------------------------------------ \n");

	return;
}

STATIC void svc_cas_MgrPrintActionContextList(void)
{
	int 				idx =0;

	HLIB_CMD_Printf ("\n========================= print action context for all ============================= \n");

	for(idx =0; idx<CAS_MAX_NUM_OF_ACTION; idx++)
	{
		svc_cas_MgrPrintActionContextByActionId(idx);
	}

	HLIB_CMD_Printf ("\n===================== end of print action context for all ========================== \n");

	return;
}

STATIC void svc_cas_MgrPrintRawTable(void)
{
	svc_cas_SiPrintRawTable();

	return;
}

STATIC void svc_cas_MgrPrintPmtInfoByActionId(HUINT32 ulActionId)
{
	SvcCas_Context_t *pContext = NULL;

	if(ulActionId >= NUM_MAX_ACTION)
	{
		return;
	}

	pContext = svc_cas_MgrGetActionContextByActionId(ulActionId);
	if(pContext == NULL)
	{
		return;
	}

	svc_cas_MgrPrintPmtInfo(&pContext->stPmtInfo);

	return;
}

#if defined(CAS_SUPPORT_CAT)
STATIC HERROR svc_cas_MgrPrintCatInfo(SvcCas_CatInfo_t *pstCatInfo)
{
	int i = 0;

	if(pstCatInfo == NULL)
	{
		HLIB_CMD_Printf(" \n");
		return ERR_FAIL;
	}

	HLIB_CMD_Printf("\n\t=============== cat info ===============\n\n");

	HLIB_CMD_Printf("\tnumber of ca desc 		(%d) \n", pstCatInfo->ulDescNum);

	for(i=0; i<pstCatInfo->ulDescNum; i++)
	{
		HLIB_CMD_Printf("\t\tca descriptor[%d], ca sys id (0x%x), ca pid (0x%x) \n",
			i, pstCatInfo->stDesc[i].usCaSysId, pstCatInfo->stDesc[i].usCaPid);
	}

	HLIB_CMD_Printf("\n\t============ end of cat info ===========\n");

	return ERR_OK;
}

STATIC void svc_cas_MgrPrintCatInfoByActionId(HUINT32 ulActionId)
{
	SvcCas_Context_t *pContext = NULL;

	if(ulActionId >= NUM_MAX_ACTION)
	{
		return;
	}

	pContext = svc_cas_MgrGetActionContextByActionId(ulActionId);
	if(pContext == NULL)
	{
		return;
	}

	svc_cas_MgrPrintCatInfo(&pContext->stCatInfo);

	return;
}
#endif

STATIC int svc_cas_MgrCasCommand(void *szArgList)
{
	char		*aucArg=NULL;
	HUINT8		 aucStr[20];//, aucArg2[20], aucArg3[20];
	HUINT32		 p2 = 0;

	/* 1st argument */
	aucArg = (char *)HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
	if( aucArg == NULL )
	{
		return HxCMD_ERR;
	}
	if(snprintf(aucStr, 20, "%s", aucArg) == 0 )
	{
		return HxCMD_ERR;
	}

	HLIB_STD_StrUpper(aucStr);

	if (strcmp(aucStr, "INFO") == 0)
	{
		svc_cas_MgrPrintInfoAll();
	}
	if (strcmp(aucStr, "PATH") == 0)
	{
		HUINT32				ulActionId;
		STATIC HINT32		nSlotId;

		/* 2nd argument : Action ID */
		aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList );
		if( aucArg == NULL )
		{
			return HxCMD_ERR;
		}
		if( sscanf(aucStr, "%d", &p2) == 0 )
		{
			return HxCMD_ERR;
		}
		ulActionId = p2;

		/* 3nd argument : CI Slot ID */
		aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList );
		if( aucArg == NULL )
		{
			return HxCMD_ERR;
		}
		if( sscanf(aucStr, "%d", &p2) == 0 )
		{
			return HxCMD_ERR;
		}
		nSlotId = p2;

		if(nSlotId > 2)
		{
			nSlotId = -1;
		}

		HLIB_CMD_Printf ("ulActionId = %d, nSlotId = %d\n", ulActionId, nSlotId);
#if defined(CONFIG_MW_CI)
		svc_cas_DevCiPlugInCi (ulActionId, nSlotId);
#endif
	}
	if (strcmp(aucStr, "TSPATH") == 0)
	{
		HUINT32			ulSlotId;
		HUINT16			usSlotId;
		HUINT32			bPassThrough;
		SvcCas_DevCiPath_e	ePath = eCAS_DEV_CI_BYPASS;

		/* 2nd argument */
		aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList );
		if( aucArg == NULL )
		{
			return HxCMD_ERR;
		}
		if( sscanf(aucStr, "%d", &p2) == 0 )
		{
			return HxCMD_ERR;
		}
		ulSlotId = p2;
		usSlotId = (HUINT16)ulSlotId;

		/* 3nd argument */
		aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList );
		if( aucArg == NULL )
		{
			return HxCMD_ERR;
		}
		if( sscanf(aucStr, "%d", &p2) == 0 )
		{
			return HxCMD_ERR;
		}
		bPassThrough = p2;

		if(bPassThrough == TRUE)
		{
			ePath = eCAS_DEV_CI_PASSTHROUGH;
		}

		HLIB_CMD_Printf("usSlotId = %d, ePath = %d\n", usSlotId, ePath);
#if defined(CONFIG_MW_CI)
		svc_cas_DevCiSetTsPath(usSlotId, ePath);
#endif

	}
	else if(strcmp(aucStr, "INST") == 0)
	{
		/* 2nd argument */
		aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList );
		if( aucArg != NULL )
		{
			if( sscanf(aucArg, "%d", &p2) == 0 )
			{
				return HxCMD_ERR;
			}

			svc_cas_MgrPrintCaInstTableById(p2);
		}
		else
		{
			svc_cas_MgrPrintCaInstTable();
		}
	}
	else if(strcmp(aucStr, "ACT") == 0)
	{
		/* 2nd argument */
		aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList );
		if( aucArg != NULL )
		{
			if( sscanf(aucArg, "%d", &p2) == 0 )
			{
				return HxCMD_ERR;
			}

			svc_cas_MgrPrintActionContextByActionId(p2);
		}
		else
		{
			svc_cas_MgrPrintActionContextList();
		}
	}
	else if (strcmp(aucStr, "RAWTBL") == 0)
	{
		svc_cas_MgrPrintRawTable();
	}
	else if (strcmp(aucStr, "PMT") == 0)
	{
		/* 2nd argument */
		aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList );
		if( aucArg != NULL )
		{
			if( sscanf(aucArg, "%d", &p2) == 0 )
			{
				return HxCMD_ERR;
			}
		}

		svc_cas_MgrPrintPmtInfoByActionId(p2);
	}
#if defined(CAS_SUPPORT_CAT)
	else if (strcmp(aucStr, "CAT") == 0)
	{
		/* 2nd argument */
		aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList );
		if( aucArg != NULL )
		{
			if( sscanf(aucArg, "%d", &p2) == 0 )
			{
				return HxCMD_ERR;
			}
		}

		svc_cas_MgrPrintCatInfoByActionId(p2);
	}
#endif

	return HxCMD_OK;
}

const HINT8 * svc_cas_MgrCmd_MakeStrCasCmd(SvcCas_Command_e cmd)
{
	switch (cmd)
	{
		ENUM_TO_STR(CMD_CAS_START);
		ENUM_TO_STR(CMD_CAS_STOP);

		ENUM_TO_STR(CMD_CAS_SETPID);
		ENUM_TO_STR(CMD_CAS_CHECKDEVICE);
		ENUM_TO_STR(CMD_CAS_CHECKEVENT);

		ENUM_TO_STR(CMD_CAS_CHECKPAT);
		ENUM_TO_STR(CMD_CAS_CHECKPMT);
		ENUM_TO_STR(CMD_CAS_CHECKCAT);
		ENUM_TO_STR(CMD_CAS_CHECKSDT);
		ENUM_TO_STR(CMD_CAS_CHECKNIT);
		ENUM_TO_STR(CMD_CAS_CHECKBAT);
		ENUM_TO_STR(CMD_CAS_CHECKEIT);
		ENUM_TO_STR(CMD_CAS_CHECKTDT);

		ENUM_TO_STR(CMD_CAS_CHECKSTREAM);
		ENUM_TO_STR(CMD_CAS_CHECKDRM);
		ENUM_TO_STR(CMD_CAS_ETCEVENT);
		ENUM_TO_STR(CMD_CAS_CHECKMMI);

		ENUM_TO_STR(CMD_CAS_SC_PPVREPLY);
		ENUM_TO_STR(CMD_CAS_SC_PINCODEREPLY);
		ENUM_TO_STR(CMD_CAS_SC_UDATAREADREPLY);

		ENUM_TO_STR(CMD_CAS_NOTIREC);

		ENUM_TO_STR(CMD_CAS_MULTISCREEN);

		ENUM_TO_STR(CMD_CAS_MAX);
	}

	return ("Unknown");
}

void svc_cas_MgrCmd_Init(HxCMD_Handle_t hCmdHandle)
{

	HLIB_CMD_RegisterWord(hCmdHandle, svc_cas_MgrCasCommand,
						"mgr",						/* keyword */
						"mw cas debug api call",	/* help   */
						"mgr info | act <id> | inst <id> | mmiopen <slot> | mmisel <slot><num> | mmians <slot><num> | mmiclose <slot> | mmiinfo | rawtbl | sdt ");	/* usage  */
}
#endif

/*********************** End of File ******************************/
