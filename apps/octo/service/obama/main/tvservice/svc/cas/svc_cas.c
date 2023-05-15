/**
	@file     casmgr_ap_api.c
	@brief    casmgr_ap_api.c (CAS Service)

	Description: Application에서 요청한 Service를 Descramble 한다. \n
	Module: MW/CAS \n
	Remarks :\n

	Copyright (c) 2012 HUMAX Co., Ltd. \n
	All rights reserved. \n
*/

/********************************************************************
* Header Files
********************************************************************/
#define ___01_Header_Files___

#include <octo_common.h>

//#include <mwc_util.h>
#if defined (CONFIG_OP_SES)
#include <pal_sc.h>
#endif

#if defined(CONFIG_SUPPORT_FCC)
#include <svc_pipe.h>
#endif
#include <svc_cas.h>
#include <_svc_cas_mgr_main.h>
#include <_svc_cas_mgr_control.h>
#include <_svc_cas_mgr_instance.h>
#include <_svc_cas_mgr_pal_port.h>
#include <_svc_cas_mgr_svc_port.h>
#include <_svc_cas_mgr_si.h>

#include <psvc_cas.h>

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


/********************************************************************
* Static Functions
* Naming rule : "local"_"CasMgrMain" _ "Function Name"
********************************************************************/
#define ___06_Local_APIs___

/********************************************************************
* Module Internal Functions
* Naming rule : "CasMgrMain" _ "Function Name"
********************************************************************/
#define ___07_Module_Internal_APIs___

HERROR svc_cas_CiGetOpName(HUINT16 usSlotId, HUINT8 **ppszOpName)
{
#if defined(CONFIG_MW_CI)
	if (NULL == ppszOpName)
		return ERR_FAIL;

	if (usSlotId >= svc_cas_PalPortCiGetNumberOfModule())
		return ERR_FAIL;

	return psvc_cas_CiGetOpName(usSlotId, ppszOpName);
#else
	return ERR_FAIL;
#endif
}

/********************************************************************
* Global Public Functions
* Naming rule : "Module Prefix" _ "Function Name"
********************************************************************/
#define ___08_Global_Public_APIs___

HERROR SVC_CAS_Init(PostMsgToMgrCb_t fnPostMsg, PostDataToMgrCb_t fnPostData)
{
	HUINT32				unResult = ERR_FAIL;

// Register Notify Callback
	(void)svc_cas_SvcPortUtilRegisterNotifyCallback(fnPostMsg, fnPostData);

// Register Callback
	psvc_cas_InitProduct();

	unResult = svc_cas_MgrInit();
	if (unResult != ERR_OK)
	{
		HxLOG_Info("MW_CAS_INIT svc_cas_MgrInit FAIL \n");
		return ERR_FAIL;
	}


// DO Init
	psvc_cas_InitModule();

	return ERR_OK;
}

HERROR SVC_CAS_SiRegisterTimeoutCallback(SvcCas_SiGetTimeOutCb_t fnSiTimeout)
{
	return svc_cas_SubSvcSiRegisterGetTableTimeoutCallback(fnSiTimeout);
}

HERROR SVC_CAS_StartService(Handle_t hAction, Handle_t hSvc, SvcCas_ActionType_e eActionType, SvcCas_StartData_t *pstCasStartData)
{
	HUINT32 			unResult = ERR_OK;
	HULONG				ulMsgQId;
	SvcCas_Message_t	stCasMsg;

	HxLOG_Info("ActionHandle(0x%x)\n", hAction);

	if(hAction == HANDLE_NULL)
	{
		HxLOG_Error("Input Handle is Invalid \n");
		return ERR_FAIL;
	}

	/* Message Create */
	HxSTD_memset(&stCasMsg, 0x00, sizeof(SvcCas_Message_t));
	stCasMsg.start.command		= CMD_CAS_START;
	stCasMsg.start.hAction		= hAction;
	stCasMsg.start.hSvc			= hSvc;
	stCasMsg.start.actionType	= eActionType;
	if(pstCasStartData == NULL)
	{
		HxSTD_memset(&stCasMsg.start.stCasStartData, 0, sizeof(SvcCas_StartData_t));
	}
	else
	{
		stCasMsg.start.stCasStartData	= *pstCasStartData;
	}

	ulMsgQId = svc_cas_MgrGetMsgQueueId();

	unResult = VK_MSG_Send(ulMsgQId, &stCasMsg, sizeof(SvcCas_Message_t));
	if(unResult != ERR_OK)
	{
		HxLOG_Error("StartSession Message Seding Failure. \n");
		return ERR_FAIL;
	}

	return ERR_OK;
}


HERROR SVC_CAS_StopService(Handle_t hAction, Handle_t hNextSvc, ApiSyncMode_t ulSyncMode)
{
	HUINT32 			unResult = ERR_OK;
	HULONG			ulMsgQId;
	SvcCas_Message_t	stCasMsg;

	HxLOG_Info("ActionHandle(0x%x)\n", hAction);

	if(hAction == HANDLE_NULL)
	{
		HxLOG_Error("Input Handle is Invalid \n");
		return ERR_FAIL;
	}

	/* Message Create */
	HxSTD_memset(&stCasMsg, 0x00, sizeof(SvcCas_Message_t));
	stCasMsg.stop.command	= CMD_CAS_STOP;
	stCasMsg.stop.hAction	= hAction;
	stCasMsg.stop.hNextSvc	= hNextSvc;
	stCasMsg.stop.syncMode	= ulSyncMode;

	ulMsgQId = svc_cas_MgrGetMsgQueueId();

	unResult = VK_MSG_Send(ulMsgQId, &stCasMsg, sizeof(SvcCas_Message_t));
	if(unResult != ERR_OK)
	{
		HxLOG_Error("StartSession Message Seding Failure. \n");
		return ERR_FAIL;
	}

	if(ulSyncMode == eSyncMode)
	{
		VK_SEM_Get(svc_cas_MgrGetCasSyncSemaId());
	}

	return ERR_OK;
}

HERROR SVC_CAS_SetRecPid(Handle_t hAction, SvcCas_ActionType_e eActType, SvcCas_SetPidInfo_t *pstCaDscInfo)
{

	HUINT32 nResult = 0;
	HUINT32 idx = 0;
	SvcCas_Message_t casMsg;
	HUINT32			ulMsgQId;
#if defined(CONFIG_IRDETO_HMX_PVR) || defined(CONFIG_MW_CAS_VIACCESS) || defined(CONFIG_MW_CAS_CONAX) || defined(CONFIG_MW_CAS_NAGRA) || defined(CONFIG_MW_CAS_VERIMATRIX)
	HUINT32		ulActionId = 0;
#endif

	HxLOG_Info("ActionHandle(0x%x)\n", hAction);

	if(hAction == HANDLE_NULL)
	{
		HxLOG_Error("Input Handle is Invalid \n");
		return ERR_FAIL;
	}

	HxSTD_memset(&casMsg, 0x00, sizeof(SvcCas_Message_t));
	casMsg.setPid.command = CMD_CAS_SETPID;
	casMsg.setPid.hAction = hAction;
	casMsg.setPid.actionType = eActType;

	HxLOG_Print("num of pid (%d)\n", pstCaDscInfo->ulPidNum);

#if defined(CONFIG_IRDETO_HMX_PVR) || defined(CONFIG_MW_CAS_VIACCESS) || defined(CONFIG_MW_CAS_CONAX) || defined(CONFIG_MW_CAS_NAGRA) || defined(CONFIG_MW_CAS_VERIMATRIX)
	// Record 에 대한 pid 를 정의한다.
	ulActionId = svc_cas_SubSvcRmGetActionId(hAction);
	if (pstCaDscInfo->ulPidNum == 0 && (ulActionId >= eActionId_REC_FIRST && ulActionId <= eActionId_REC_LAST))
	{
		HINT32	i = 0;
		SvcCas_PmtInfo_t	*pstPmtInfo = NULL;

		pstPmtInfo = svc_cas_MgrSubGetPmtInfo(hAction);
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
						casMsg.setPid.stEsPidInfo.stPidInfo[idx].esType = pstPmtInfo->stElementInfo[i].eEsType;
						casMsg.setPid.stEsPidInfo.stPidInfo[idx].usPid = pstPmtInfo->stElementInfo[i].usPid;
						idx++;
						break;
					default:
						break;
				}
			}
			casMsg.setPid.stEsPidInfo.ulPidNum = idx;

			ulMsgQId = svc_cas_MgrGetMsgQueueId();

			nResult = VK_MSG_Send(ulMsgQId, &casMsg, sizeof(SvcCas_Message_t));
			if(nResult != ERR_OK)
			{
				HxLOG_Error("Message Send Failure. \n");
				return ERR_FAIL;
			}

			return ERR_OK;
		}
	}
#endif

	for(idx = 0; idx < pstCaDscInfo->ulPidNum; idx++)
	{
		casMsg.setPid.stEsPidInfo.stPidInfo[idx].esType = pstCaDscInfo->stPidInfo[idx].esType;
		casMsg.setPid.stEsPidInfo.stPidInfo[idx].usPid = pstCaDscInfo->stPidInfo[idx].usPid;
	}
	casMsg.setPid.stEsPidInfo.ulPidNum = pstCaDscInfo->ulPidNum;

	ulMsgQId = svc_cas_MgrGetMsgQueueId();

	nResult = VK_MSG_Send(ulMsgQId, &casMsg, sizeof(SvcCas_Message_t));
	if(nResult != ERR_OK)
	{
		HxLOG_Error("Message Send Failure. \n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR SVC_CAS_SetDataPid(Handle_t hAction, SvcCas_SetPidInfo_t *pstCaDscInfo)
{
	HxLOG_Critical("Dummy Function. Should be implemented \n");
	return ERR_OK;
}

HERROR SVC_CAS_SetPid(	Handle_t hAction,
							SvcCas_ActionType_e eActType,
							HUINT16 usVideoPid,
							HUINT16 usAudioPid,
							HUINT16 usAuxPid,
							HUINT16 usDolbyPid,
							HUINT16 usTeletext_Super_Pid,
							HUINT16 usSubtitle_CC_Pid )
{
	HUINT32 			nResult = 0;
	HUINT32			ulMsgQId;
	SvcCas_Message_t	stCasMsg;

#if defined(CONFIG_MW_CAS_IRDETO_UCAS)
	HUINT32				i=0, ulAddPidNum=0;
	SvcCas_PmtInfo_t	*pstPmtInfo = NULL;
#endif

	HxLOG_Info("hAction(%x), vid(%x), aud(%x), aux(%x), dolby(%x), ttx_super(%x), sub_cc(%x)\n",
		hAction, usVideoPid, usAudioPid, usAuxPid, usDolbyPid, usTeletext_Super_Pid, usSubtitle_CC_Pid);

	if(hAction == HANDLE_NULL)
	{
		HxLOG_Error("Input Handle is Invalid \n");
		return ERR_FAIL;
	}

	HxSTD_memset(&stCasMsg, 0x00, sizeof(SvcCas_Message_t));
	stCasMsg.setPid.command = CMD_CAS_SETPID;
	stCasMsg.setPid.hAction= hAction;
	stCasMsg.setPid.actionType = eActType;

	stCasMsg.setPid.stEsPidInfo.ulPidNum = CAS_MAX_NUM_OF_LIVE_PID;
	stCasMsg.setPid.stEsPidInfo.stPidInfo[0].esType = eEsType_Video;
	stCasMsg.setPid.stEsPidInfo.stPidInfo[0].usPid = usVideoPid;
	stCasMsg.setPid.stEsPidInfo.stPidInfo[1].esType = eEsType_Audio;
	stCasMsg.setPid.stEsPidInfo.stPidInfo[1].usPid = usAudioPid;
	stCasMsg.setPid.stEsPidInfo.stPidInfo[2].esType = eEsType_Audio;
	stCasMsg.setPid.stEsPidInfo.stPidInfo[2].usPid = usAuxPid;
	stCasMsg.setPid.stEsPidInfo.stPidInfo[3].esType = eEsType_Audio;
	stCasMsg.setPid.stEsPidInfo.stPidInfo[3].usPid = usDolbyPid;
	stCasMsg.setPid.stEsPidInfo.stPidInfo[4].esType = eEsType_Subtitle;
	stCasMsg.setPid.stEsPidInfo.stPidInfo[4].usPid = usSubtitle_CC_Pid;
	stCasMsg.setPid.stEsPidInfo.stPidInfo[5].esType = eEsType_Teletext;
	stCasMsg.setPid.stEsPidInfo.stPidInfo[5].usPid = usTeletext_Super_Pid;

#if defined(CONFIG_MW_CAS_IRDETO_UCAS)
	ulAddPidNum = stCasMsg.setPid.stEsPidInfo.ulPidNum;
	HxLOG_Info("Alreay add on ulAddPidNum [%d]\n", ulAddPidNum);
	pstPmtInfo = svc_cas_MgrSubGetPmtInfo(hAction);
	if (pstPmtInfo != NULL)
	{
		for (i = 0; i < pstPmtInfo->ulNumOfElement; i++)
		{
			switch (pstPmtInfo->stElementInfo[i].eEsType)
			{
				case	eEsType_Audio:
					HxLOG_Info("svc_cas_MgrSubGetPmtInfo() eEsType_Audio element PID[0x%x],usAudioPid :[0x%x],usAuxPid :[0x%x],usDolbyPid :[0x%x]\n", pstPmtInfo->stElementInfo[i].usPid, usAudioPid, usAuxPid, usDolbyPid);
					if ((pstPmtInfo->stElementInfo[i].usPid != usAudioPid) && (pstPmtInfo->stElementInfo[i].usPid != usAuxPid) && (pstPmtInfo->stElementInfo[i].usPid != usDolbyPid))
					{
						stCasMsg.setPid.stEsPidInfo.stPidInfo[ulAddPidNum].esType = eEsType_Audio;
						stCasMsg.setPid.stEsPidInfo.stPidInfo[ulAddPidNum].usPid = pstPmtInfo->stElementInfo[i].usPid;
						ulAddPidNum += 1;
						HxLOG_Info("+ ADD Audio PID[0x%x]\n", pstPmtInfo->stElementInfo[i].usPid);
					}
					break;
				default:
					break;
			}
		}
		HxLOG_Info("Total ulAddPidNum [%d]\n", ulAddPidNum);
		stCasMsg.setPid.stEsPidInfo.ulPidNum = ulAddPidNum;
	}
#endif

	ulMsgQId = svc_cas_MgrGetMsgQueueId();

	nResult = VK_MSG_Send(ulMsgQId, &stCasMsg, sizeof(SvcCas_Message_t));
	if(nResult != ERR_OK)
	{
		HxLOG_Error("Message Send Failure. \n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR SVC_CAS_GetChannelType(Handle_t hAction, SvcCas_ChannelType_e *pType)
{
	SvcCas_Context_t						*pstCasContext = NULL;

	if(pType == NULL)
	{
		return ERR_FAIL;
	}

	pstCasContext = svc_cas_MgrGetActionContextByAction(hAction);

	if (pstCasContext == NULL)
	{
		HxLOG_Error("pstCasContext is NULL\n");
		return ERR_FAIL;
	}

	if (CasMgrSI_DefineChannelType(pstCasContext, pType) != ERR_OK)
	{
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR	SVC_CAS_GetNumOfSCSlot(HUINT32 *ulMaxSlot)
{
#if defined (CONFIG_OP_SES)
	#if (CONFIG_MW_CAS_NUM_SC_SLOT > 0)
	*ulMaxSlot = PAL_SC_GetNumberOfModule();
	#else
	*ulMaxSlot = 0;
	#endif
#else
	HxLOG_Critical("Dummy Function. Should be implemented \n");
#endif

	return ERR_OK;
}

HBOOL	SVC_CAS_CheckSmartCardValidity(DxCAS_GroupId_e eCasId)
{
	return svc_cas_InstanceCheckSmartCardValidityByCasId(eCasId);
}

HERROR	SVC_CAS_GetSCCasSysIdList (HUINT32 *pulNumCaSysId, HUINT16 **ppusCaSysIdList)
{
	HxLOG_Critical("Dummy Function. Should be implemented \n");
	return ERR_OK;
}

HERROR	SVC_CAS_FreeSCCasSysIdList (HUINT16 *pusCaSysIdList)
{
	HxLOG_Critical("Dummy Function. Should be implemented \n");
	return ERR_OK;
}

#if defined (CONFIG_OP_SES)
HERROR	SVC_CAS_GetCaSystemIdByActionId (HUINT32 ulActionId, HUINT32 *pulCnt, HUINT16 *pusCaSystemID)
{
	HINT32			ulCnt = 0;
	SvcCas_PmtInfo_t 	*pstPmtInfo;
	SvcCas_Context_t *pContext = NULL;

	if(ulActionId >= CAS_MAX_NUM_OF_ACTION)
	{
		return ERR_FAIL;
	}

	pContext = svc_cas_MgrGetActionContextByActionId(ulActionId);
	if(pContext == NULL || pContext->state == eCAS_State_Idle)
	{
		return ERR_FAIL;
	}

	pstPmtInfo = &pContext->stPmtInfo;
	if(pstPmtInfo == NULL)
	{
		HxLOG_Error(" \n");
		return ERR_FAIL;
	}

	*pulCnt = pstPmtInfo->ulNumOfProgramCaDescriptor;
	if(*pulCnt > CAS_MAX_NUM_OF_CAPID)
	{
		*pulCnt = CAS_MAX_NUM_OF_CAPID;
	}

	for(ulCnt = 0 ; ulCnt < *pulCnt ; ulCnt++)
	{
		pusCaSystemID[ulCnt] = pstPmtInfo->stProgramCaDescriptor[ulCnt].usCaSysId;
	}

	return ERR_OK;
}
#endif

#define __________CI___________

#if defined(CONFIG_MW_CI)
HERROR SVC_CAS_GetNumOfCISlot(HUINT32 *ulMaxSlot)
{
	*ulMaxSlot = svc_cas_PalPortCiGetNumberOfModule();

	return ERR_OK;
}

HERROR SVC_CAS_GetMaxNumOfCISlot(HUINT32 *ulMaxSlot)
{
	*ulMaxSlot = svc_cas_PalPortCiGetMaxNumOfCISlot();

	return ERR_OK;
}

HERROR SVC_CAS_GetCamState(HUINT16 usSlotId, HUINT32 *pCamState)
{
	if (NULL == pCamState)
		return ERR_FAIL;

	return svc_cas_InstanceGetCamState(usSlotId, pCamState);
}

// for cam name display
HERROR SVC_CAS_GetCamName(HUINT16 usSlotId, HUINT8 **ppszCamInfo)
{
	if (NULL == ppszCamInfo)
		return ERR_FAIL;

	return svc_cas_InstanceGetCamName(usSlotId, ppszCamInfo);
}

HERROR SVC_CAS_CiGetOpName(HUINT16 usSlotId, HUINT8 **ppszOpName)
{
	if (NULL == ppszOpName)
		return ERR_FAIL;

	return svc_cas_InstanceGetOpName(usSlotId, ppszOpName);
}
#endif



HERROR SVC_CAS_GetUiData(HUINT8 *pucUiData, DxCAS_UiDataType_e eType)
{
	HxLOG_Debug("eType: %d \n", eType);

	return psvc_cas_GetUiData(pucUiData, eType);
}




#define __________CONTROL___________

HERROR SVC_CAS_CtrlCall(DxCAS_GroupId_e eCasId, HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	return svc_cas_CtrlCall(eCasId, ulControlType, pstParam);
}

HERROR SVC_CAS_CtrlGet(DxCAS_GroupId_e eCasId, HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	return svc_cas_CtrlGet(eCasId, ulControlType, pstParam);
}

HERROR SVC_CAS_CtrlSet(DxCAS_GroupId_e eCasId, HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	return svc_cas_CtrlSet(eCasId, ulControlType, pstParam);
}

HERROR SVC_CAS_CtrlReq(DxCAS_GroupId_e eCasId, HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	return svc_cas_CtrlReq(eCasId, ulControlType, pstParam);
}

HERROR SVC_CAS_CtrlNoti(DxCAS_GroupId_e eCasId, HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	return svc_cas_CtrlNoti(eCasId, ulControlType, pstParam);
}

/********************************************************************
* For Debugging
********************************************************************/
#define ___09_Debugging_APIs___

#if defined(CONFIG_DEBUG)

#if defined(CONFIG_MW_CAS_IRDETO)
extern void CMD_Register_MW_CAS_IRDRV_Debug(HxCMD_Handle_t hCasIrCmdHandle);
#endif
#if defined(CONFIG_MW_CAS_NAGRA)
extern void CMD_Register_SVC_CAS_NagraCmd(HxCMD_Handle_t hCasIrCmdHandle);
#endif
#if defined(CONFIG_MW_CAS_CONAX)
extern void CMD_Register_SVC_CAS_ConaxCoreCmd(HxCMD_Handle_t hCasCxCmdHandle);
#endif
void SVC_CAS_CmdInit(HxCMD_Handle_t hCmdHandle)
{
	svc_cas_MgrCmd_Init(hCmdHandle);

#if defined(CONFIG_MW_CI)
#endif

#if defined(CONFIG_MW_CAS_NAGRA)
	CMD_Register_SVC_CAS_NagraCmd(hCmdHandle);
#endif

#if defined(CONFIG_MW_CAS_IRDETO_SCELL)
	CMD_Register_MW_CAS_IRDRV_Debug(hCmdHandle);
#endif

#if defined(CONFIG_MW_CAS_IRDETO_UCAS)
	CMD_Register_SVC_CAS_IrdetoCmd(hCmdHandle);
#endif

#if defined(CONFIG_MW_CAS_CONAX)
	CMD_Register_SVC_CAS_ConaxCoreCmd(hCmdHandle);
#endif

#if defined(CONFIG_MW_CAS_SATSA)
#endif

}

#endif

/*********************** End of File ******************************/
