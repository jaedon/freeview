/*******************************************************************
	File Description
********************************************************************/
/**
	@file	   	ir_status.c \n
	@brief	main \n

	Description:   \n
	Module: MW/CAS/ IRDETO \n

	Copyright (c) 2010 HUMAX Co., Ltd. \n
	All rights reserved. \n
*/
#include <pal_pipe.h>
#include <pal_sc.h>

#include "s3_vd.h"
#include "s3_prod_if_msg.h"
#include "s3_cadrv.h"

#include <svc_cas.h>

#include <_svc_cas_sub_dev_api.h>

#include "vd_humax.h"
#include "ir_svc.h"
#include "ir_msg_scell.h"
#include "ir_api.h"
#include "ir_evt.h"

#include "ir_status.h"
#include "ir_util.h"

#if defined(CONFIG_OP_DIGITURK)
#include "s3_tm_msg.h"
#endif

#if defined(IRPLAT_FN_COPY_CONTROL)
#include "s3_vd_macrovision.h"
#include "s3_vd_copycontrol.h"
#endif

#if	defined(IRPLAT_SYS_PVR)
	#include "ir_pvr.h"
	#include <svc_rec.h>
	#include <svc_pb.h>
	#include <svc_meta.h>
#endif
#if	defined(CONFIG_IRDETO_HMX_PVR) || defined(CONFIG_MW_CAS_IRDETO_PPV)
//	#include <mwc_util.h>
#endif

#if defined(CONFIG_MW_CAS_IRDETO_MATURITY_RATING)
	#include "ir_mr.h"
#endif

#if defined(CONFIG_DEBUG)&& !defined(WIN32)
	#include "ir_debug.h"
	#define DEBUG_SVC_STATUS
#endif

#include "ir_fta_block.h"
#if	defined(CONFIG_IRDETO_HMX_PVR)
#include "_svc_cas_sub_api.h"
#endif
#include "ir_mgr.h"
/********************************************************************
*	Definitions (for this source file)
********************************************************************/






/********************************************************************
*	Macro Definitions
********************************************************************/
#define	_______INTERNAL_STATUS_FUNCTION_______
#define GetIRState(a, b)		\
								a.ucValid = b.Valid; 				\
								a.CharSeverity = b.CharSeverity;	\
								a.eSource = b.stStatus.eSource; 	\
								a.usStatus = b.stStatus.wStatus; 	\
								a.eSeverity = b.stStatus.eSeverity


#define GetIRRscState(a, b)		\
								a.CharSeverity = b.CharSeverity;	\
								a.usRscID = b.usResourceID;			\
								a.eSource = b.stStatus.eSource; 	\
								a.usStatus = b.stStatus.wStatus;	\
								a.eSeverity = b.stStatus.eSeverity

#define	TOGGLE(a)	((a == 0) ? 1 : 0)


/********************************************************************
*	Static variables
********************************************************************/
// ewlee 20100915
#if defined(CONFIG_MW_CAS_IRDETO_PPV)
static HBOOL	s_bIsNeedToGetPurchasedEventList = FALSE;
#endif

#if (CONFIG_MW_CAS_NUM_SC_SLOT > 1)
HUINT32			g_I07_4_Flagfor2SC = FALSE;
HUINT32			g_I07_4_SvcNum = 0;
#endif

/********************************************************************
*	Global variables
********************************************************************/
#ifdef	_____ECM_EMM_MONITORING_____
HUINT16	s_usSCELLforMonitor = 0;
HUINT8	s_ucTypeForMonitor = IR_MONITOR_STOP;
	#ifdef __WA_EMM_MONITOR_CLOSE_ERR__
		HUINT32	s_EmmMonitorCount[CAS_MAX_NUM_OF_SC];
	#endif
#endif

/********************************************************************
*   extern variable
********************************************************************/
extern	SOFTCELL_VERSION_DATA		ClientVersionData[];
extern	SOFTCELL_VERSION_DATA		ComponentsVersionData[];

extern	IR_SERVICE_INFO			g_IrSvcInfo[IR_HUMAX_MAX_SERVICE];
extern	IR_TP_INFO				g_IrTpInfo[IR_HUMAX_MAX_TP];
extern	IR_DRV_SC_INFO			g_IrDrvScInfo[HUMAX_VD_SC_SLOT_COUNT];

#if	defined(IRPLAT_SYS_PVR)
extern ia_result IR_MACROVISION_GetRecordMode(HUINT16 svcInfoIndex, HUINT8* pRecordMode);
extern HUINT32 HUMAX_VD_ExtStorage_DelayWrite(vd_ref_id wRefId, ia_word32 ulStart, ia_word16 ulLen, void *pucData);
#endif

#if defined(IRPLAT_FN_COPY_CONTROL)
extern ia_result IR_MACROVISION_GetCopyControlInfo(HUINT16 svcInfoIndex, HUINT16 *pusTypeMask, HUINT8 *pucMcMode, HUINT8 *pucDigitalCci, HUINT8 *pucAnologCci);
#endif
#if defined(CONFIG_MW_CAS_IRDETO_HOMINGCHANNEL) || defined(CONFIG_MW_CAS_IRDETO_RECOVERY_RESCAN_HOME)
extern HVD_Result  HUMAX_VD_SC_GetCardPhysicalStatus(HUINT8 ucSlotID, HBOOL *pSCInserted);
#endif


#if defined(IR_INST_CHANGE_BY_CAS_WORK_WELL)
static HBOOL		ir_desc_well = TRUE;

void		CAS_IR_STAUS_SetIrdetoDescState(HBOOL irDescState);
HBOOL	CAS_IR_STATUS_IsCamAvailable(void);
HBOOL	CAS_IR_STATUS_IsIrdetoStatusError(ia_status_st stStatus);

extern HUINT32 CAS_IR_MGR_Notify_InstanceChanged(HUINT16 usSlotId, HUINT16 *pusCaSysId, HUINT8 ucNumCaSysId, HBOOL bAvailable);
extern HERROR CASINST_GetCamState(HUINT16 usSlotId, SvcCas_CamState_e *eCamState);
#endif

extern AXI_STATUSCODE_IDX	CAS_IR_DRV_GetErrorCodeIndex(ia_status_st *pStatus);

#if defined(IRPLAT_SC_QUERY_EXTENDED_PRODUCT_LIST_ID)
HIR_ERR CAS_IR_DRV_QueryExtendedProductList(HUINT32 ulScRscId, ia_bool bAnySector, HUINT8 ucSectorCount, HUINT8* pucSectorList);
#endif

/********************************************************************************
// jichoi: currently, following api is used for ir api and debug 2009.11.20
********************************************************************************/
HUINT8 CAS_IR_DRV_GetNumberOfServices(IR_SVC_STATUS_INFO_MAIN **stSvcStatusMain)
{
	HUINT8	ucCount = 0;
	HUINT8	ucIndex = 0;
	HUINT8	ucEMMsvcInstance = 1;
	HUINT8	ucECMsvcInstance = 1;
	HUINT8	i, j;
#if	defined(IRPLAT_SYS_PVR)
	HUINT8	ucPVRsvcInstance = 1;
#endif

	IR_SVC_STATUS_INFO_MAIN *pWork;

	HxLOG_Info("\n");

	for (i = 0; i < IR_HUMAX_MAX_TP; i++)
	{
		for (j = 0; j < CAS_MAX_NUM_OF_SC; j++)
		{
			HxLOG_Info(" g_IrTpInfo[%d].usSCELL_SvcHandle[%d] : (%d) \n", i, j, g_IrTpInfo[i].usSCELL_SvcHandle[j]);

#ifdef	CONFIG_DEBUG
			if ( g_IrTpInfo[i].ucIsSCELL_Ready && g_IrTpInfo[i].ucIsAllocated )// && (g_IrTpInfo[i].usSCELL_SvcHandle[j] != NULL))
			HxLOG_Print("TP Info  : Ready [%d] Alloc [%d]\n", g_IrTpInfo[i].ucIsSCELL_Ready, g_IrTpInfo[i].ucIsAllocated);
#endif
			//if ( g_IrTpInfo[i].ucIsSCELL_Ready && g_IrTpInfo[i].ucIsAllocated)
			if ( g_IrTpInfo[i].ucIsSCELL_Ready && g_IrTpInfo[i].ucIsAllocated  && g_IrTpInfo[i].usSCELL_SvcHandle[j] != INVALID_SOFTCELL_SVCHANDLE)
			{
				ucCount++;
				HxLOG_Info(" ucCount: (%d)   tp,i : (%d)   sc,j : (%d) \n", ucCount, i, j);
			}

		}
	}
	for (i = 0; i < IR_HUMAX_MAX_SERVICE; i++)
	{
		if ( g_IrSvcInfo[i].ucIsSCELLReady && g_IrSvcInfo[i].ucIsAllocated)
		{
			ucCount++;
#if	defined(IRPLAT_SYS_PVR)
			//	We don't need descramble service in Playback mode
			if (g_IrSvcInfo[i].eIrSvcActionType == IR_SVC_PLAYBACK)
				ucCount--;

			if (g_IrSvcInfo[i].pstPVRSvcInfo)
			{
				if (g_IrSvcInfo[i].pstPVRSvcInfo->ucIsSCELLReady)
					ucCount++;
			}
#endif
		}
	}

	ucIndex = 0;
	if( ucCount !=0 )
	{
		*stSvcStatusMain = (IR_SVC_STATUS_INFO_MAIN *)OxCAS_Malloc(sizeof(IR_SVC_STATUS_INFO_MAIN) * ucCount);
		if (*stSvcStatusMain == NULL)
		{
			HxLOG_Print(" Error : Mem Alloc Error\n");
			ucCount = 0;
			return ucCount;
		}
		pWork = *stSvcStatusMain;

		for( j = 0; j < CAS_MAX_NUM_OF_SC; j++ )
		{
			for( i = 0; i < IR_HUMAX_MAX_TP; i++ )
			{
				//if (g_IrTpInfo[i].ucIsSCELL_Ready && g_IrTpInfo[i].ucIsAllocated)
				if ( g_IrTpInfo[i].ucIsSCELL_Ready && g_IrTpInfo[i].ucIsAllocated  && g_IrTpInfo[i].usSCELL_SvcHandle[j] != INVALID_SOFTCELL_SVCHANDLE)
				{
					pWork[ucIndex].hAction			= g_IrTpInfo[i].hAction;
					pWork[ucIndex].eSvcType			= g_IrTpInfo[i].usSCELL_SvcType[j];
					HxLOG_Info(" g_IrTpInfo[i].usSCELL_SvcType[j]: (%d), g_IrTpInfo[i].usSCELL_SvcHandle[j]: (%d), ucEMMsvcInstance: (%d), tp,i : (%d)   sc,j : (%d) \n", g_IrTpInfo[i].usSCELL_SvcType[j], g_IrTpInfo[i].usSCELL_SvcHandle[j], ucEMMsvcInstance, i, j);
					pWork[ucIndex].usSCellSvcHandle	= g_IrTpInfo[i].usSCELL_SvcHandle[j];
					pWork[ucIndex].ucInstanceCount	= ucEMMsvcInstance;
					ucEMMsvcInstance++;
					ucIndex++;
				}
			}
		}

		for( i = 0; i < IR_HUMAX_MAX_SERVICE; i++ )
		{
#if	defined(IRPLAT_PVR_DRM)
			if (g_IrSvcInfo[i].ucIsSCELLReady && g_IrSvcInfo[i].ucIsAllocated)
#elif defined(IRPLAT_PVR_PHASE12)
			if (g_IrSvcInfo[i].ucIsSCELLReady && g_IrSvcInfo[i].ucIsAllocated)
			//if (g_IrSvcInfo[i].ucIsSCELLReady && g_IrSvcInfo[i].ucIsAllocated && g_IrSvcInfo[i].usSCELL_SvcHandle != INVALID_SOFTCELL_SVCHANDLE)
#else		// PVR Define이 빠진 일반적인 상황에서의 조건.
			if (g_IrSvcInfo[i].ucIsSCELLReady && g_IrSvcInfo[i].ucIsAllocated && g_IrSvcInfo[i].usSCELL_SvcHandle != INVALID_SOFTCELL_SVCHANDLE)
#endif
			{
				pWork[ucIndex].hAction			= g_IrSvcInfo[i].hAction;
				pWork[ucIndex].eSvcType			= g_IrSvcInfo[i].usSCELL_SvcType;
				pWork[ucIndex].usSCellSvcHandle	= g_IrSvcInfo[i].usSCELL_SvcHandle;
				pWork[ucIndex].ucInstanceCount	= ucECMsvcInstance;
				ucIndex++;
				ucECMsvcInstance++;

				HxLOG_Info(" g_IrSvcInfo[i].usSCELL_SvcType: (%d), g_IrSvcInfo[i].usSCELL_SvcHandle: (%d), ucECMsvcInstance: (%d), svc,i : (%d)\n", g_IrSvcInfo[i].usSCELL_SvcType, g_IrSvcInfo[i].usSCELL_SvcHandle, ucECMsvcInstance, i);

#if	defined(IRPLAT_SYS_PVR)
				//	we don't need descrambler in playback service
				if (g_IrSvcInfo[i].eIrSvcActionType == IR_SVC_PLAYBACK)
				{
					ucIndex--;
					ucECMsvcInstance--;
				}

				if (g_IrSvcInfo[i].pstPVRSvcInfo)
				{
					if (g_IrSvcInfo[i].pstPVRSvcInfo->ucIsSCELLReady)
					{
						pWork[ucIndex].hAction			= g_IrSvcInfo[i].hAction;
						pWork[ucIndex].eSvcType			= g_IrSvcInfo[i].pstPVRSvcInfo->usSCELL_SvcType;
						pWork[ucIndex].usSCellSvcHandle	= g_IrSvcInfo[i].pstPVRSvcInfo->usSCELL_SvcHandle;
						pWork[ucIndex].ucInstanceCount	= ucPVRsvcInstance;

#if	defined(IRPLAT_PVR_PHASE12)
						if (g_IrSvcInfo[i].eCasActionType == eCAS_ActionType_Tsr_Play)
						{
							pWork[ucIndex].eSvcType		= SERVICE_PVR_TSR_PLAYBACK;
						}
						else if (g_IrSvcInfo[i].eCasActionType == eCAS_ActionType_Tsr_Rec)
						{
							pWork[ucIndex].eSvcType		= SERVICE_PVR_TSR_RECORD;
						}
#endif
						ucIndex++;
						ucPVRsvcInstance++;
					}
				}
#endif
			}
		}
	}

	HxLOG_Info(" ucCount: %d \n", ucCount);
	return ucCount;
}


/********************************************************************************
********************************************************************************/
HUINT8 CAS_IR_DRV_GetInformationOfElementary(HUINT16 usHandle, HUINT32 ulResourceId, IR_ELEM_STATE **stSvcStatusInfoElement)
{
	HUINT8	i, ucCount = 0;

	IR_SERVICE_INFO		*pstIrSvcInfo;
	IR_ELEM_STATE		*pWork;

	pstIrSvcInfo = CAS_IR_DRV_FindIrSvcInfoBySCELLHandle(usHandle);
	if (pstIrSvcInfo == NULL)
	{
		HxLOG_Print("not matched service info(0x%X)\n", usHandle );
		return 0;
	}

	ucCount = pstIrSvcInfo->ucStreamCount;
	if( ucCount )
	{
		*stSvcStatusInfoElement = (IR_ELEM_STATE *)OxCAS_Malloc(sizeof(IR_ELEM_STATE) * ucCount);
		if (*stSvcStatusInfoElement == NULL)
		{
			HxLOG_Print(": Mem Alloc Error\n");
			ucCount = 0;
			return ucCount;
		}
		pWork = *stSvcStatusInfoElement;

		for( i = 0; i < ucCount; i++ )
		{
			pWork[i].usEsPID		= pstIrSvcInfo->astPidList[i].usPID;
			pWork[i].usEcmOrEmmPID	= pstIrSvcInfo->astPidList[i].ElemRsc[ulResourceId].usEcmPID;
			pWork[i].CharSeverity	= pstIrSvcInfo->astPidList[i].ElemRsc[ulResourceId].CharSeverity;
			pWork[i].eSource		= pstIrSvcInfo->astPidList[i].ElemRsc[ulResourceId].stStatus.eSource;
			pWork[i].usStatus		= pstIrSvcInfo->astPidList[i].ElemRsc[ulResourceId].stStatus.wStatus;
			pWork[i].eSeverity		= pstIrSvcInfo->astPidList[i].ElemRsc[ulResourceId].stStatus.eSeverity;

			#if 0 //debug
				HxLOG_Print("\t\t(%d)usEsPID \t\t= 0x%x\n", i, pWork[i].usEsPID);
				HxLOG_Print("\t\t(%d)usEcmOrEmmPID \t= 0x%x\n", i, pWork[i].usEcmOrEmmPID);
				HxLOG_Print("\t\t(%d)CharSeverity\t\t= %d\n", i, pWork[i].CharSeverity);
				HxLOG_Print("\t\t(%d)eSource \t\t= %d\n", i, pWork[i].eSource);
				HxLOG_Print("\t\t(%d)usStatus \t\t= %d\n", i, pWork[i].usStatus);
				HxLOG_Print("\t\t(%d)eSeverity \t\t= %d\n\n", i, pWork[i].eSeverity);
			#endif
		}
	}

	HxLOG_Print("ucCount= %d\n", ucCount);

	return ucCount;
}

/********************************************************************************
********************************************************************************/
HIR_ERR CAS_IR_DRV_GetInformationOfEachService(HUINT32 eSvcType, HUINT16 usHandle, IR_SVC_STATUS_INFO_EACH *pstSvcStatusInfoEach)
{
	HUINT8	i, j;
//	IR_TP_INFO			*pstIrTpInfo;
	IR_SERVICE_INFO		*pstIrSvcInfo;

	HxLOG_Info("\n");

	if( pstSvcStatusInfoEach == NULL )
		return HIR_NOT_OK;

	if (eSvcType == SERVICE_DVB_EMM)
	{
//		pstIrTpInfo = CAS_IR_DRV_FindIrTpInfoBySCELL(usHandle);
//		if (pstIrTpInfo == NULL)
//		{
HxLOG_Info("[CAS_IR_DRV_GetInformationOfEachService] pstIrTpInfo is NULL!!!!!!!!!\n");
//			return HIR_OK;
//		}

		for ( i = 0; i < IR_HUMAX_MAX_TP; i++ )
		{
			for ( j = 0; j < CAS_MAX_NUM_OF_SC; j++)
			{
				if (g_IrTpInfo[i].ucIsAllocated &&  g_IrTpInfo[i].ucIsSCELL_Ready && (g_IrTpInfo[i].usSCELL_SvcHandle[j] == usHandle))
				{
					pstSvcStatusInfoEach->eSvcType = eSvcType;
					pstSvcStatusInfoEach->usHandle = usHandle;

					// GLOBAL STATUS
					GetIRState(pstSvcStatusInfoEach->GlobalStatus, g_IrTpInfo[i].GlobalStatus[j]);

					// INTERNAL STATUS
					GetIRState(pstSvcStatusInfoEach->InternalStatus, g_IrTpInfo[i].InternalStatus[j]);

					// EMM RSC STATUS
					GetIRRscState(pstSvcStatusInfoEach->EcmOrEmmRscStatus, g_IrTpInfo[i].EmmRscStatus[j]);
					pstSvcStatusInfoEach->EcmOrEmmRscStatus.usEmmPID = g_IrTpInfo[i].EmmRscStatus[j].usEmmPID;

					// SMARTCARD RSC STATUS
					GetIRRscState(pstSvcStatusInfoEach->SmartcardRscStatus, g_IrTpInfo[i].SmartcardRscStatus[j]);

					// EMM MONITORING
					HxSTD_memcpy((HUINT8 *)&(pstSvcStatusInfoEach->EmmMonitoring), (HUINT8 *)&(g_IrTpInfo[i].EmmMonitoring[j]), sizeof(IR_EMM_MON_STATE));
					HxLOG_Info("i:(%d) j:(%d) GLOBAL STATUS info:(%d) (%d) (%d)\n", i, j, pstSvcStatusInfoEach->GlobalStatus.eSeverity, pstSvcStatusInfoEach->GlobalStatus.eSource, pstSvcStatusInfoEach->GlobalStatus.usStatus);
				}
			}
		}
	}
	else
	{
		switch (eSvcType)
		{
			case	SERVICE_DVB_DESCRAMBLE:
				pstIrSvcInfo = CAS_IR_DRV_FindIrSvcInfoBySCELLHandle(usHandle);
				if (pstIrSvcInfo)
				{
					pstSvcStatusInfoEach->eSvcType = eSvcType;
					pstSvcStatusInfoEach->usHandle = usHandle;

					// GLOBAL STATUS
					GetIRState(pstSvcStatusInfoEach->GlobalStatus, pstIrSvcInfo->GlobalStatus);

					// INTERNAL STATUS
					GetIRState(pstSvcStatusInfoEach->InternalStatus, pstIrSvcInfo->InternalStatus);

					// DVB DESCRAMBLER RSC STATUS
					GetIRRscState(pstSvcStatusInfoEach->DvbDescramblerRscStatus, pstIrSvcInfo->DvbDescramblerRscStatus);

					// ECM RSC STATUS
					GetIRRscState(pstSvcStatusInfoEach->EcmOrEmmRscStatus, pstIrSvcInfo->EcmRscStatus);

					// SMARTCARD RSC STATUS
					GetIRRscState(pstSvcStatusInfoEach->SmartcardRscStatus, pstIrSvcInfo->SmartcardRscStatus);

					// MACROVISION RSC STATUS
					GetIRRscState(pstSvcStatusInfoEach->MacrovisionRscStatus, pstIrSvcInfo->MacrovisionRscStatus);

					// ECM MONITORING
					for (j = 0; j < CAS_MAX_NUM_OF_SC; j++)
					{
						for (i = 0; i < MAX_NUM_ECM_MONITOR_PID; i++)
						{
							pstSvcStatusInfoEach->EcmPID[j][i] = IRDRV_NULL_PID;
							HxSTD_memset((HUINT8 *)&(pstSvcStatusInfoEach->EcmMonitoring[j][i]), 0x00, sizeof(IR_ECM_MON_STATE));
						}
					}
					for (j = 0; j < CAS_MAX_NUM_OF_SC; j++)
					{
						for (i = 0; i < MAX_NUM_ECM_MONITOR_PID; i++)
						{
							if (pstIrSvcInfo->EcmMonitorInfo[j].ausEcmMonitorPID[i] == IRDRV_NULL_PID)
							{
								break;
							}

							pstSvcStatusInfoEach->EcmPID[j][i] = pstIrSvcInfo->EcmMonitorInfo[j].ausEcmMonitorPID[i];
							HxSTD_memcpy((HUINT8 *)&(pstSvcStatusInfoEach->EcmMonitoring[j][i]), (HUINT8 *)&(pstIrSvcInfo->EcmMonitorInfo[j].astEcmMonitoring[i]), sizeof(msg_dvb_ecm_monitor_data_st));
						}
					}
				}
				break;

#if	defined(IRPLAT_SYS_PVR)
	#if	defined(IRPLAT_PVR_PHASE12)
			case	SERVICE_PVR_RECORD:
			case	SERVICE_PVR_TSR_RECORD:
				{
					IR_PVRSERVICE_INFO	*pstPVRSvcInfo;

					pstPVRSvcInfo = CAS_IR_Drv_FindPVRSvcInfoBySCELL(usHandle);
					if (pstPVRSvcInfo)
					{
						pstSvcStatusInfoEach->eSvcType = eSvcType;
						pstSvcStatusInfoEach->usHandle = usHandle;

						// GLOBAL STATUS
						GetIRState(pstSvcStatusInfoEach->GlobalStatus, pstPVRSvcInfo->GlobalStatus);

						// INTERNAL STATUS
						GetIRState(pstSvcStatusInfoEach->InternalStatus, pstPVRSvcInfo->InternalStatus);

						// Session Ref
						pstSvcStatusInfoEach->recordId = pstPVRSvcInfo->ulContentId;

						// Session Management Status
						GetIRRscState(pstSvcStatusInfoEach->SessionMngStatus, pstPVRSvcInfo->SessionManageRscStatus);

						// Session Sub Status
						GetIRRscState(pstSvcStatusInfoEach->SessionSubStatus, pstPVRSvcInfo->SessionSubStatus);

						// Crypto Operatoin Status
						GetIRRscState(pstSvcStatusInfoEach->CryptoOPStatus, pstPVRSvcInfo->CryptoOperationRscStatus);
					}
				}
				break;

			case	SERVICE_PVR_PLAYBACK:
			case	SERVICE_PVR_TSR_PLAYBACK:
				{
					IR_PVRSERVICE_INFO	*pstPVRSvcInfo;

					pstPVRSvcInfo = CAS_IR_Drv_FindPVRSvcInfoBySCELL(usHandle);
					if (pstPVRSvcInfo)
					{
						pstSvcStatusInfoEach->eSvcType = eSvcType;
						pstSvcStatusInfoEach->usHandle = usHandle;

						// GLOBAL STATUS
						GetIRState(pstSvcStatusInfoEach->GlobalStatus, pstPVRSvcInfo->GlobalStatus);

						// INTERNAL STATUS
						GetIRState(pstSvcStatusInfoEach->InternalStatus, pstPVRSvcInfo->InternalStatus);

						// Session Ref
						pstSvcStatusInfoEach->recordId = pstPVRSvcInfo->ulContentId;

						// Session Management Status
						GetIRRscState(pstSvcStatusInfoEach->SessionMngStatus, pstPVRSvcInfo->SessionManageRscStatus);

						// Session Sub Status
						GetIRRscState(pstSvcStatusInfoEach->SessionSubStatus, pstPVRSvcInfo->SessionSubStatus);

						// Crypto Operatoin Status
						GetIRRscState(pstSvcStatusInfoEach->CryptoOPStatus, pstPVRSvcInfo->CryptoOperationRscStatus);
					}
				}
				break;
		#endif
		#if	defined(IRPLAT_PVR_DRM)
			case	SERVICE_PVR_DRM:
				{
					IR_PVRSERVICE_INFO	*pstPVRSvcInfo;

					pstPVRSvcInfo = CAS_IR_Drv_FindPVRSvcInfoBySCELL(usHandle);
					if (pstPVRSvcInfo)
					{
						pstSvcStatusInfoEach->eSvcType = eSvcType;
						pstSvcStatusInfoEach->usHandle = usHandle;

						// GLOBAL STATUS
						GetIRState(pstSvcStatusInfoEach->GlobalStatus, pstPVRSvcInfo->GlobalStatus);

						// INTERNAL STATUS
						GetIRState(pstSvcStatusInfoEach->InternalStatus, pstPVRSvcInfo->InternalStatus);

						// Session Ref
						pstSvcStatusInfoEach->recordId = pstPVRSvcInfo->ulContentId;

							//	Contents Right (Content Right Id, RightRecId, Seq Rec Id)
							pstSvcStatusInfoEach->ulCRID = pstPVRSvcInfo->stPVRDrmInfo.stDrmRightRecInfo.ulCRID;
							pstSvcStatusInfoEach->ulRightsRecId = pstPVRSvcInfo->stPVRDrmInfo.stDrmBasicInfo.ulRightRecId;
							pstSvcStatusInfoEach->ulSeqRecId = pstPVRSvcInfo->stPVRDrmInfo.stDrmBasicInfo.ulSeqRecId;

						// Session Management Status
						GetIRRscState(pstSvcStatusInfoEach->SessionMngStatus, pstPVRSvcInfo->SessionManageRscStatus);

						// Session Sub Status
						GetIRRscState(pstSvcStatusInfoEach->SessionSubStatus, pstPVRSvcInfo->SessionSubStatus);

						// Crypto Operatoin Status
						GetIRRscState(pstSvcStatusInfoEach->CryptoOPStatus, pstPVRSvcInfo->CryptoOperationRscStatus);

						//	Copy Control
						GetIRRscState(pstSvcStatusInfoEach->MacrovisionRscStatus, pstPVRSvcInfo->MacrovisionRscStatus);
					}
				}
				break;
		#endif
#endif
		}
	}

 	return HIR_OK;
}

/********************************************************************************
********************************************************************************/
HIR_ERR CAS_IR_DRV_SendServiceDefineOfEcmMonitor(HUINT16 ushSCELL, HUINT8 bStart)
{
	HINT32		i, j;

	IR_SERVICE_INFO		*pstIrSvcInfo;

	HxLOG_Info("SCELLHandle (%x) bStart (%x)\n", ushSCELL, bStart);

	pstIrSvcInfo = CAS_IR_DRV_FindIrSvcInfoBySCELLHandle(ushSCELL);
	if (pstIrSvcInfo == NULL)
	{
		HxLOG_Print("no matched SoftCELL handle!!\n");
		return HIR_NOT_OK;
	}

	for (j = 0; j < CAS_MAX_NUM_OF_SC; j++)
	{
		for (i = 0; i < MAX_NUM_ECM_MONITOR_PID; i++)
		{
			pstIrSvcInfo->EcmMonitorInfo[j].ausEcmMonitorPID[i] = IRDRV_NULL_PID;
			HxSTD_memset(&(pstIrSvcInfo->EcmMonitorInfo[j].astEcmMonitoring[i]), 0, sizeof(msg_dvb_ecm_monitor_data_st));
		}
	}

	return CAS_IR_SVC_DEFINE_ECMMONITOR(ushSCELL, bStart);
}

/********************************************************************************
********************************************************************************/
HIR_ERR CAS_IR_DRV_SendServiceDefineOfEmmMonitor(HUINT16 ushSCELL, HUINT8 bStart)
{
	IR_TP_INFO	*pstIrTpInfo;
	HUINT32	i;

	HxLOG_Info("usSCELLHandle(%x) bStart(%x)\n", ushSCELL, bStart);
	pstIrTpInfo = CAS_IR_DRV_FindIrTpInfoBySCELL(ushSCELL);
	if (pstIrTpInfo == NULL)
	{
		HxLOG_Print("no matched SoftCELL handle!!!\n");
		return HIR_NOT_OK;
	}

	for (i = 0; i < CAS_MAX_NUM_OF_SC; i++)
	{
		HxSTD_memset(&(pstIrTpInfo->EmmMonitoring[i]), 0, sizeof(msg_dvb_emm_monitor_data_st));

		#ifdef __WA_EMM_MONITOR_CLOSE_ERR__
			s_EmmMonitorCount[i] = 0;
		#endif
	}

	return CAS_IR_SVC_DEFINE_EMMMONITOR(ushSCELL, bStart);
}

/********************************************************************************
********************************************************************************/
HIR_ERR CAS_IR_DRV_GetGlobalStatusOfService(HUINT32 unPlayHandle, IR_STATE *pGlobalStatus)
{
	IR_SERVICE_INFO		*pstIrSvcInfo;

	HxLOG_Info("\n");
	pstIrSvcInfo = CAS_IR_DRV_FindIrSvcInfo(unPlayHandle);
	if (pstIrSvcInfo)
	{
		GetIRState((*pGlobalStatus), pstIrSvcInfo->GlobalStatus);
		return HIR_OK;
	}

	return HIR_NOT_OK;
}


/********************************************************************************
********************************************************************************/
void CAS_IR_DRV_UpdateStatus_SC(HUINT32 eSvcType, ia_word16 wServiceHandle, HUINT32	Source, HUINT32 usRscId, HUINT32 usEcmOrEmmPID, ia_status_st status)
{
	HUINT32 i;
	HMSG		msg={0,};
	HUINT8	CharSeverity = SEVERITY_TO_CHAR(status.eSeverity);

	HxLOG_Info("\n");

	HxLOG_Print("\n\n~~~~~~~~~ %c%03d-%d ~~~~~~~~\n\n", CharSeverity, status.wStatus, status.eSource);
	/*Condition 1. Normal SC was in there */
	if(Is_D00_4(status)  )
	{
#if defined(IRPLAT_FN_QUERY_VENDOR_ID)
		/* It has highest priority in octo sys. */
		if(HIR_OK != CAS_IR_DRV_QueryVendorID(usRscId) )
		{
			HxLOG_Print("Query vendor id error\n") );
		}
#endif

		if(HIR_OK != CAS_IR_DRV_QueryScData(usRscId) )
		{
			HxLOG_Print("Query SC data error\n");
		}


		if(HIR_OK != CAS_IR_DRV_AnyProductQuery(usRscId) )
		{
			HxLOG_Print("Query to check any product in SC: error\n");
		}
#if !defined(CONFIG_OP_ALMAJD)	// Al Majd 는 head end 에서 EMM 을 냈을 때만 eeprom 에 저장한다.
#if defined(CONFIG_MW_CAS_IRDETO_HOMINGCHANNEL) || defined(CONFIG_MW_CAS_IRDETO_RECOVERY_RESCAN_HOME)
		if(HIR_OK != CAS_IR_DRV_HomingChannelQuery(usRscId) )
		{
			HxLOG_Print("Query to homming channel by SC: error\n");
		}
#endif
#endif
#if defined(CONFIG_MW_CAS_IRDETO_MATURITY_RATING)
		if(HIR_OK != CAS_IR_DRV_MR_CapabilityQuery(usRscId) )
		{
			HxLOG_Print("Query to homming channel by SC: error\n");
		}
#endif
// ewlee 20100915
#if defined(CONFIG_MW_CAS_IRDETO_PPV)
		if (CAS_IR_DRV_GetFlagToGetPurchasedEventList() == TRUE)
		{
			CASMGR_UpdateCasEtcEvent(0, CAS_IR_MGR_GetCasInstance(), eDxCAS_GROUPID_IR, eSEVT_CAS_SC_INSERTED, usRscId, eDxCAS_GROUPID_IR, 0);
			CAS_IR_DRV_SetFlagToGetPurchasedEventList(FALSE);
		}
#endif

#if (CONFIG_MW_CAS_NUM_SC_SLOT > 1)
		if(g_I07_4_Flagfor2SC)
		{
			g_IrSvcInfo[g_I07_4_SvcNum].GlobalStatus.stStatus.wStatus = status.wStatus;
			g_IrSvcInfo[g_I07_4_SvcNum].GlobalStatus.stStatus.eSeverity = status.eSeverity;
			g_I07_4_Flagfor2SC = FALSE;
		}
#endif

#if defined(CONFIG_MW_CAS_IRDETO_FTA_BLOCK)
		CAS_IR_FTA_BLOCK_update_Status(FTA_BLOCK_UPDATE_IRCARD_STATUS_OK);
#endif
	}
#if (CONFIG_MW_CAS_NUM_SC_SLOT > 1)
	else if( Is_I07_4(status) )
	{
		HBOOL	bSendMsg = TRUE;
		HxSTD_memset(&(g_IrDrvScInfo[usRscId].stIrScApiInfo), 0, sizeof(IR_API_SC_INFO));

		/* 현재 서비스 상태가 AV 출력되는 상태 (디스크램블 or FTA) 인지 확인 */
		for( i = 0; i < IR_HUMAX_MAX_SERVICE; i++ )
		{
			/* Allocate 되어있는 서비스의 상태가 AV 출력 상태면 I07-4 메세지를 보내지 않는다. */
			if((g_IrSvcInfo[i].ucIsAllocated == TRUE) && ( g_IrSvcInfo[i].GlobalStatus.stStatus.eSeverity == STATUS_INFO_DND ))
			{
				HxLOG_Print("STATUS_INFO_DND: not send I07-4! \n");
				bSendMsg = FALSE;
			}
		}

		/* 서비스 상태가 AV 출력되는 상태가 아닐때만 I07-4 EVENT 전달 */
		if( bSendMsg )
		{
			msg.usMsgClass = IREVT_CLASS_CAS_ERROR_FOR_GLOBAL;
			msg.usParam = (HUINT16)CAS_IR_DRV_GetErrorCodeIndex(&status);
			msg.ulParam = MAKE_IR_STATUSCODE(CharSeverity, status.wStatus, status.eSource);
			msg.unInfo.aulL[0] = usRscId;

			for( i = 0; i < IR_HUMAX_MAX_SERVICE; i++ )
			{
				if( g_IrSvcInfo[i].hIrSvc != HANDLE_NULL )
				{
					CAS_IR_EVT_SendEvent(g_IrSvcInfo[i].hAction, &msg);

					/* 메세지를 보냈으면 Allocate 되어 있는 서비스의 상태를 I07-4로 바꿈 */
					if ((g_IrDrvScInfo[TOGGLE(usRscId)].CardStatus != VD_SC_CARD_OUT) && (g_IrSvcInfo[i].ucIsAllocated == TRUE))
					{
						HxLOG_Print("\n alloc (%d) select (%d) ready (%d) \n", g_IrSvcInfo[i].ucIsAllocated, g_IrSvcInfo[i].ucIsSelected, g_IrSvcInfo[i].ucIsSCELLReady);
						HxLOG_Print("Send I07-4! \n\n");
						g_I07_4_Flagfor2SC = TRUE;
						g_I07_4_SvcNum = i;
					}
				}
			}
		}
	}
	else if( Is_E04_4(status) )
#else
	/*Condition 2.	SC was out from box. */
	else if( Is_E04_4(status) || Is_I07_4(status))
#endif
	{
#if (CONFIG_MW_CAS_NUM_EMBEDDED > 1)
		if( Is_E04_4(status) )
		{
			CAS_IR_MGR_Notify_InstanceChanged(usRscId, NULL, 0 , FALSE);
			svc_cas_DevScNotifyScValidity(g_IrDrvScInfo[usRscId].hIRSci, eCAS_SCM_FLAG_DISABLE);
			g_IrDrvScInfo[usRscId].CardPhysicalStatus = eCAS_SCI_SC_NOT_PRESENT;
		}
#endif
		HxSTD_memset(&(g_IrDrvScInfo[usRscId].stIrScApiInfo), 0, sizeof(IR_API_SC_INFO));

#if (CONFIG_MW_CAS_NUM_SC_SLOT > 1)
		if (g_IrDrvScInfo[TOGGLE(usRscId)].CardStatus == VD_SC_CARD_OUT)
#endif
		{
			msg.usMsgClass = IREVT_CLASS_CAS_ERROR_FOR_GLOBAL/*IREVT_CLASS_SC_NATIONALITY_CHANGED_BY_SC_OUT*/;
			msg.usParam = (HUINT16)CAS_IR_DRV_GetErrorCodeIndex(&status);
			msg.ulParam = MAKE_IR_STATUSCODE(CharSeverity, status.wStatus, status.eSource);
			msg.unInfo.aulL[0] = usRscId;

#if defined(CONFIG_MW_CAS_IRDETO_SOL)
			CAS_IR_SOL_update_SC_Status(SOL_SC_UPDATE_IRCARD_STATUS, FALSE);
#endif
			for( i = 0; i < IR_HUMAX_MAX_SERVICE; i++ )
			{
				if( g_IrSvcInfo[i].hIrSvc != HANDLE_NULL )
				{
					CAS_IR_EVT_SendEvent(g_IrSvcInfo[i].hAction, &msg);
				}
			}
		}
	}
#if (CONFIG_MW_CAS_NUM_EMBEDDED > 1)
	else if( Is_E05_4(status) ||  Is_E06_4(status))
	{
		CAS_IR_MGR_Notify_InstanceChanged(usRscId, NULL, 0 , FALSE);
		svc_cas_DevScNotifyScValidity(g_IrDrvScInfo[usRscId].hIRSci, eCAS_SCM_FLAG_DISABLE);
		g_IrDrvScInfo[usRscId].CardPhysicalStatus = eCAS_SCI_SC_NOT_PRESENT;

		msg.usMsgClass = IREVT_CLASS_CAS_ERROR_FOR_GLOBAL;
		msg.usParam = (HUINT16)CAS_IR_DRV_GetErrorCodeIndex(&status);
		msg.ulParam = MAKE_IR_STATUSCODE(CharSeverity, status.wStatus, status.eSource);
		msg.unInfo.aulL[0] = usRscId;

		for( i = 0; i < IR_HUMAX_MAX_SERVICE; i++ )
		{
			if( g_IrSvcInfo[i].hIrSvc != HANDLE_NULL )
			{
				CAS_IR_EVT_SendEvent(g_IrSvcInfo[i].hAction, &msg);
			}
		}
	}
#endif

	g_IrDrvScInfo[usRscId].stIrScApiInfo.stStatus.eSource = status.eSource;
	g_IrDrvScInfo[usRscId].stIrScApiInfo.stStatus.usStatus = status.wStatus;
	g_IrDrvScInfo[usRscId].stIrScApiInfo.stStatus.eSeverity = status.eSeverity;

	snprintf((char*)g_IrDrvScInfo[usRscId].stIrScApiInfo.szStatus, IR_SC_INFO_STATUS_LENGTH - 1, "%c%03d-%d", CharSeverity, status.wStatus, status.eSource);

#if 0 // by jichoi.
	msg.usMsgClass = IREVT_CLASS_CAS_ERROR_FOR_GLOBAL;
	msg.usParam = (HUINT16)CAS_IR_DRV_GetErrorCodeIndex(&status);
	msg.ulParam = MAKE_IR_STATUSCODE(CharSeverity, status.wStatus, status.eSource);
	CAS_IR_EVT_SendEvent(g_IrSvcInfo[i].hAction, &msg);
#endif

}

/********************************************************************************
********************************************************************************/
void	CAS_IR_DRV_UpdateStatus(HUINT32 eSvcType, ia_word16 wServiceHandle, HUINT32	Source, HUINT32 usEsPIDorRscID, HUINT32 usEcmOrEmmPID, ia_status_st status, HUINT32 ulResourceID)
{
	HMSG		msg = {0, };
	HINT32		i, j;
	HUINT8		CharSeverity;
	IR_STATUS_INFO	statusInfo;
	IR_SERVICE_INFO	*pstIrSvcInfo;
#if defined(CONFIG_MW_CAS_IRDETO_COMPONENT_LEVEL_SCRAMBLE)
	static HBOOL		s_bStateChange = FALSE;
#endif
#if defined(IRPLAT_PVR_PHASE12)
	HUINT8		recordMode;
#endif

#if defined(DEBUG_SVC_STATUS)&& !defined(WIN32)
	CAS_IR_DBG_STATUS(eSvcType, wServiceHandle, Source, status);
#endif

	CharSeverity = SEVERITY_TO_CHAR(status.eSeverity);
	statusInfo.CharSeverity = CharSeverity;
	statusInfo.usResourceID = usEsPIDorRscID;
	statusInfo.stStatus.eSource = status.eSource;
	statusInfo.stStatus.wStatus = status.wStatus;
	statusInfo.stStatus.eSeverity = status.eSeverity;
	statusInfo.Valid = TRUE;

	/**** wServiceHandle이 0xFFFF인 것은 smartcard status나 msg_error처럼 global한 것들. ****/
	if( wServiceHandle == INVALID_SOFTCELL_SVCHANDLE )
	{
		if( Source == RESOURCE_SMARTCARD )
		{
			CAS_IR_DRV_UpdateStatus_SC( eSvcType,  wServiceHandle,	Source,  usEsPIDorRscID,  usEcmOrEmmPID,  status);
		}
		else if( Source == RESOURCE_ERROR )
		{
			HxLOG_Print("invalid svc handle - RESOURCE_ERROR\n");
		}
		else
		{
			HxLOG_Print("invalid svc handle, Must be checked Source(0x%x)!!!!\n", Source);
			HxLOG_Critical("\n\n");
		}
		return;
	}

	switch (eSvcType)
	{
		case SERVICE_DVB_DESCRAMBLE:
			pstIrSvcInfo = CAS_IR_DRV_FindIrSvcInfoBySCELLHandle(wServiceHandle);
			if (pstIrSvcInfo)
			{
#if defined(IR_SVC_CLOSE_USE_NULL_PMT)
			if( pstIrSvcInfo->hSvc != HANDLE_NULL )
#endif
				switch (Source)
				{
					case	RESOURCE_GLOBAL_SOURCE:
						HxLOG_Print("\n\nBEFORE : %c%03d-%d\n\n", pstIrSvcInfo->GlobalStatus.stStatus.eSeverity, pstIrSvcInfo->GlobalStatus.stStatus.wStatus, pstIrSvcInfo->GlobalStatus.stStatus.eSource);
#if defined(CONFIG_MW_CAS_IRDETO_COMPONENT_LEVEL_SCRAMBLE)
						if( (pstIrSvcInfo->GlobalStatus.stStatus.wStatus != status.wStatus) || (pstIrSvcInfo->GlobalStatus.stStatus.eSeverity != status.eSeverity) || (s_bStateChange == TRUE))
#else
						if( (pstIrSvcInfo->GlobalStatus.stStatus.wStatus != status.wStatus) || (pstIrSvcInfo->GlobalStatus.stStatus.eSeverity != status.eSeverity) )
#endif
						{
							HxLOG_Print("\n\n***** BEFORE : %c%03d-%d => %c%03d-%d *****\n\n",pstIrSvcInfo->GlobalStatus.stStatus.eSeverity, pstIrSvcInfo->GlobalStatus.stStatus.wStatus, pstIrSvcInfo->GlobalStatus.stStatus.eSource, CharSeverity, status.wStatus, status.eSource);


#if	defined(IRPLAT_SYS_PVR)
	#if	defined(IRPLAT_PVR_DRM)
							if (Is_D100_8(status))
							{
								HxLOG_Print("FTA Service --> Clear or Free Service...\n");
								if (pstIrSvcInfo->eIrSvcActionType == IR_SVC_RECORD_BASE)
								{
									HxLOG_Info("Recordbase Service is descrambled!! start Irdeto PVR DRM Service!!!\n");
									CAS_IR_DRV_Setup_Record(pstIrSvcInfo->hIrSvc, IR_RECORD_TYPE_DRM, 0);
								}
							}
	#endif
#endif

#if defined(CONFIG_OP_ALMAJD)
#if defined(CONFIG_MW_CAS_IRDETO_FTA_BLOCK)
							CAS_IR_FTA_BLOCK_update_Status(FTA_BLOCK_UPDATE_IR_STATUS_OK);
#endif
#endif
							/*	Correct CW is being set by descrambler related with targetted scrambled onemore es of svc */
							if(Is_D29_4(status))
							{
								if(pstIrSvcInfo->usSCELL_SvcHandle == wServiceHandle )
								{
									msg.usMsgClass = IREVT_CLASS_CAS_DESCRAMBLED_WELL;
									HxLOG_Info(" need to get error code  1st\n");
									msg.usParam = (HUINT16)CAS_IR_DRV_GetErrorCodeIndex(&(pstIrSvcInfo->GlobalStatus.stStatus) );
									msg.ulParam= (HUINT32)MAKE_IR_STATUSCODE(CharSeverity, pstIrSvcInfo->GlobalStatus.stStatus.wStatus, pstIrSvcInfo->GlobalStatus.stStatus.eSource);

									CAS_IR_EVT_SendEvent(pstIrSvcInfo->hAction, &msg);

#if defined(CONFIG_MW_CAS_IRDETO_PREVIEW)
									if(pstIrSvcInfo->eIrSvcActionType == IR_SVC_LIVE)
									{// Start ECM Monitor for Preview
										CAS_IR_DRV_SendServiceDefineOfEcmMonitor(pstIrSvcInfo->usSCELL_SvcHandle, 1);
									}
#endif

#if defined(IRPLAT_SYS_SAVE_SMARTCARD_NUMBER)  && defined (CONFIG_OP_ALMAJD)
									//	AlMajd 는 scrambled channel 이 정상적으로 play 되었을 때 smartcard number 를 저장하여 두었다가  E30-4 가 발생 하였을 때 smartcard unpaire 에 의한 것인지? key 를 못 받은 것인지를 구분한다.
									HUMAX_VdSc_SaveSmartcardNumber ();
#endif
								}
#if defined(IRPLAT_SYS_PVR)		//	descramble ok --> start pvr service.
#if	defined(IRPLAT_PVR_PHASE12)
								if (pstIrSvcInfo->eIrSvcActionType == IR_SVC_RECORD_BASE)
								{
									HxLOG_Print("Recordbase Service is descrambled!! start Irdeto PVR Service!!\n");
									IR_MACROVISION_GetRecordMode(CAS_IR_DRV_GetIrSvcIndexFromPtr(pstIrSvcInfo), &recordMode);
									CAS_IR_DRV_Setup_Record(pstIrSvcInfo->hIrSvc, recordMode, 0);
								}
#endif
#if	defined(IRPLAT_PVR_DRM)
								if (pstIrSvcInfo->eIrSvcActionType == IR_SVC_RECORD_BASE)
								{
									HxLOG_Info("Recordbase Service is descrambled!! start Irdeto PVR DRM Service!!!\n");
									CAS_IR_DRV_Setup_Record(pstIrSvcInfo->hIrSvc, IR_RECORD_TYPE_DRM, 0);
								}
#endif
#endif

#if defined(IR_INST_CHANGE_BY_CAS_WORK_WELL)
								CAS_IR_STAUS_SetIrdetoDescState(TRUE);
#endif

#if	defined(CONFIG_IRDETO_HMX_PVR)
								//	Descramble 확인시 DRM OK를 내려준다.
								svc_cas_MgrSubUpdateCasEtcEvent(pstIrSvcInfo->hAction, CAS_IR_MGR_GetCasInstance(), eDxCAS_GROUPID_IR, eSEVT_CAS_DRM_NONE, 0, 0, 0);
#endif
							}
#if defined(CONFIG_MW_CAS_IRDETO_MATURITY_RATING)
							/* it's for making off waiting_lock state of ext-app state
							In addition, if E136, 137, or E138 occured, we can assume the card has no problem regarding product and etcs.
							only maturity rating's flaged on, then we could insert the maturity rating state in ext app. */
							else if(Is_E136orE137_4(status ) )
							{
								msg.usMsgClass = IREVT_CLASS_CAS_DESCRAMBLED_WELL;
								HxLOG_Info(" need to get error code  2nd\n");
								msg.usParam = (HUINT16)CAS_IR_DRV_GetErrorCodeIndex(&status);
								msg.ulParam = (HUINT32)MAKE_IR_STATUSCODE(CharSeverity, status.wStatus, status.eSource);
								CAS_IR_EVT_SendEvent(pstIrSvcInfo->hAction, &msg);
							}
#endif
							/*	The Case against: Correct CW is being set by descrambler related with targetted scrambled onemore es of svc */
							else
							{
#if defined(IR_INST_CHANGE_BY_CAS_WORK_WELL)
								if(CAS_IR_STATUS_IsIrdetoStatusError(status))
								{
									// Request Instance Change!
									IrStatusData(DBG_PRINT, ("\n\t Request Instance Change!!!\n\n"));
									CAS_IR_STAUS_SetIrdetoDescState(FALSE);
									CAS_IR_MGR_Notify_InstanceChanged(0, NULL, 1, FALSE);
								}
								else
								{
									CAS_IR_STAUS_SetIrdetoDescState(TRUE);
								}
#endif								// I034-9는 descramble확인 중....이므로 display해야하는 error로 생각하지 말자...
								if( !Is_I34_9(status) )
								{
								#if (CONFIG_MW_CAS_NUM_SC_SLOT > 1)
									/* 2SC에서 SLOT0에만 Card가 있는 경우 서비스 전환시 (Card가 있음에도 불구하고) E04-4에러가 발생하여 다음과 같이 WA함. */
									/* Root Cause를 찾으면 하기 코드 삭제할 것. */
									if( Is_E04_4(status) )
									{
										if((g_IrDrvScInfo[0].CardStatus != VD_SC_CARD_IN) && (g_IrDrvScInfo[1].CardStatus != VD_SC_CARD_IN))
										{
											msg.usMsgClass = IREVT_CLASS_CAS_ERROR_FOR_SERVICE;
											msg.usParam = (HUINT16)CAS_IR_DRV_GetErrorCodeIndex(&status);
											msg.ulParam = (HUINT32)MAKE_IR_STATUSCODE(CharSeverity, status.wStatus, status.eSource);

											CAS_IR_EVT_SendEvent(pstIrSvcInfo->hAction, &msg);
										}
									}
									else
								#endif
									{
										msg.usMsgClass = IREVT_CLASS_CAS_ERROR_FOR_SERVICE;
										HxLOG_Info(" need to get error code  3rd\n");
										msg.usParam = (HUINT16)CAS_IR_DRV_GetErrorCodeIndex(&status);
										msg.ulParam = (HUINT32)MAKE_IR_STATUSCODE(CharSeverity, status.wStatus, status.eSource);

										CAS_IR_EVT_SendEvent(pstIrSvcInfo->hAction, &msg);
									}
								}
							}
#if defined(CONFIG_MW_CAS_IRDETO_COMPONENT_LEVEL_SCRAMBLE)
							s_bStateChange = FALSE;
#endif
						}

						pstIrSvcInfo->GlobalStatus = statusInfo;
						break;

					case	RESOURCE_INTERNAL_SOURCE:
						pstIrSvcInfo->InternalStatus = statusInfo;
						break;

					case	RESOURCE_ECM_SOURCE:
						pstIrSvcInfo->EcmRscStatus = statusInfo;
						break;

					case	RESOURCE_ELEM_STREAM_SOURCE:
						for( j = 0; j < pstIrSvcInfo->ucStreamCount; j++ )
						{
					#if defined(CONFIG_MW_CAS_IRDETO_COMPONENT_LEVEL_SCRAMBLE)
							if (ulResourceID < CAS_MAX_NUM_OF_SC)
							{
							//ycgo
								if( (pstIrSvcInfo->astPidList[j].ElemRsc[ulResourceID].CharSeverity != CharSeverity) ||
									(pstIrSvcInfo->astPidList[j].ElemRsc[ulResourceID].usEcmPID != (HUINT16)usEcmOrEmmPID) ||
									(pstIrSvcInfo->astPidList[j].ElemRsc[ulResourceID].stStatus.eSource != status.eSource) ||
									(pstIrSvcInfo->astPidList[j].ElemRsc[ulResourceID].stStatus.wStatus != status.wStatus) ||
									(pstIrSvcInfo->astPidList[j].ElemRsc[ulResourceID].stStatus.eSeverity != status.eSeverity)
								)
								{
									if( pstIrSvcInfo->astPidList[j].usPID == usEsPIDorRscID )
									{
										pstIrSvcInfo->astPidList[j].ElemRsc[ulResourceID].CharSeverity			= CharSeverity;
										pstIrSvcInfo->astPidList[j].ElemRsc[ulResourceID].usEcmPID				= (HUINT16)usEcmOrEmmPID;
										pstIrSvcInfo->astPidList[j].ElemRsc[ulResourceID].stStatus.eSource		= status.eSource;
										pstIrSvcInfo->astPidList[j].ElemRsc[ulResourceID].stStatus.wStatus		= status.wStatus;
										pstIrSvcInfo->astPidList[j].ElemRsc[ulResourceID].stStatus.eSeverity	= status.eSeverity;
										pstIrSvcInfo->astPidList[j].ElemRsc[ulResourceID].Valid = TRUE;
										break;
									}
									s_bStateChange = TRUE;
								}
							}
					#else
							if( pstIrSvcInfo->astPidList[j].usPID == usEsPIDorRscID )
							{
								pstIrSvcInfo->astPidList[j].ElemRsc[ulResourceID].CharSeverity		= CharSeverity;
								pstIrSvcInfo->astPidList[j].ElemRsc[ulResourceID].usEcmPID			= (HUINT16)usEcmOrEmmPID;
								pstIrSvcInfo->astPidList[j].ElemRsc[ulResourceID].stStatus.eSource	= status.eSource;
								pstIrSvcInfo->astPidList[j].ElemRsc[ulResourceID].stStatus.wStatus	= status.wStatus;
								pstIrSvcInfo->astPidList[j].ElemRsc[ulResourceID].stStatus.eSeverity	= status.eSeverity;
								pstIrSvcInfo->astPidList[j].ElemRsc[ulResourceID].Valid = TRUE;
								break;
							}
					#endif
						}

						if( j == pstIrSvcInfo->ucStreamCount )
							HxLOG_Print(" unknown usEsPIDorRscID(0x%X).\n", usEsPIDorRscID);
						break;

					case	RESOURCE_DVB_DESCRAMBLER:
						pstIrSvcInfo->DvbDescramblerRscStatus = statusInfo;
						break;

					case	RESOURCE_SMARTCARD:
						pstIrSvcInfo->SmartcardRscStatus = statusInfo;
						break;

#if defined(IRPLAT_FN_COPY_CONTROL)
					case	RESOURCE_MACROVISION:
					{
						HUINT16 usCCTypeMask;
						HUINT8	ucMcMode, ucDigitalCci, ucAnalogCci;

						pstIrSvcInfo->MacrovisionRscStatus = statusInfo;
#if defined(CONFIG_OP_DIGITURK)
						usCCTypeMask = (HUINT16)((0xFF000000 & usEcmOrEmmPID) >> 24);
						ucMcMode = (HUINT8)((0x00FF0000 & usEcmOrEmmPID) >> 16);
						ucDigitalCci = (HUINT8)((0x0000FF00 & usEcmOrEmmPID) >> 8);
						ucAnalogCci = (HUINT8)(0x000000FF & usEcmOrEmmPID);
						HxLOG_Error("Descramble[type 0x%04x], MC[%d], DT[%d], AN[%d]\n", usCCTypeMask, ucMcMode, ucDigitalCci, ucAnalogCci);
						CAS_IR_MGR_NotifyCopyControlEvent(pstIrSvcInfo->hAction, usCCTypeMask, ucMcMode, ucDigitalCci, ucAnalogCci);
#else

						IR_MACROVISION_GetCopyControlInfo(CAS_IR_DRV_GetIrSvcIndexFromPtr(pstIrSvcInfo), &usCCTypeMask, &ucMcMode, &ucDigitalCci, &ucAnalogCci);
						CAS_IR_MGR_NotifyCopyControlEvent(pstIrSvcInfo->hAction, usCCTypeMask, ucMcMode, ucDigitalCci, ucAnalogCci);
#endif
					}
						break;
#else
					case	RESOURCE_MACROVISION:
						pstIrSvcInfo->MacrovisionRscStatus = statusInfo;
						break;
#endif

					case	RESOURCE_MONITORING:
//						if ((status.eSeverity == STATUS_INFO_DND) && (status.wStatus == 100))
//							pstIrSvcInfo->ucIsMonitoring = 1;
						break;

#if defined(CONFIG_MW_CAS_IRDETO_MATURITY_RATING)
					case	RESOURCE_MATURITY_RATING:
						pstIrSvcInfo->MaturityRatingRscStatus = statusInfo;
						break;
#endif
					default:
						HxLOG_Print("Invalid Resource Type 0x%x\n", Source);
						break;
				}
			}

			break;

		case	SERVICE_DVB_EMM:
			for (i = 0; i < IR_HUMAX_MAX_TP; i++)
			{
				if ( g_IrTpInfo[i].ucIsAllocated &&  g_IrTpInfo[i].ucIsSCELL_Ready )
				{
					for( j = 0; j < CAS_MAX_NUM_OF_SC; j++ )
					{
						if ( g_IrTpInfo[i].usSCELL_SvcHandle[j] == wServiceHandle )
						{
							switch (Source)
							{
								case	RESOURCE_GLOBAL_SOURCE:
									g_IrTpInfo[i].GlobalStatus[j] = statusInfo;
									break;
								case	RESOURCE_INTERNAL_SOURCE:
									g_IrTpInfo[i].InternalStatus[j] = statusInfo;
									break;
								case	RESOURCE_EMM_SOURCE:
									g_IrTpInfo[i].EmmRscStatus[j].CharSeverity		= CharSeverity;
									g_IrTpInfo[i].EmmRscStatus[j].usResourceID		= (HUINT16)usEsPIDorRscID;
									g_IrTpInfo[i].EmmRscStatus[j].usEmmPID			= (HUINT16)usEcmOrEmmPID;
									g_IrTpInfo[i].EmmRscStatus[j].stStatus.eSource	= status.eSource;
									g_IrTpInfo[i].EmmRscStatus[j].stStatus.wStatus	= status.wStatus;
									g_IrTpInfo[i].EmmRscStatus[j].stStatus.eSeverity= status.eSeverity;
									g_IrTpInfo[i].EmmRscStatus[j].Valid				= TRUE;
									break;
								case	RESOURCE_SMARTCARD:
									g_IrTpInfo[i].SmartcardRscStatus[j] = statusInfo;
									break;
								case	RESOURCE_MONITORING:
			//						if ((status.eSeverity == STATUS_INFO_DND) && (status.wStatus == 100))
			//							pstIrTpInfo->ucIsMonitoring = 1;
									break;
							}
						}
					}
				}
			}
			break;
/*	아래와 같은 방법으로 update를 하게 되면, 같은 정보로 덮어 버릴 수 있음. 추후에 CAS_IR_DRV_FindIrTpInfoBySCELL() 에서 index도 return 받는 방식으로 변경해도 무관.
			pstIrTpInfo = CAS_IR_DRV_FindIrTpInfoBySCELL(wServiceHandle);
			if (pstIrTpInfo)
			{
				for( j = 0; j < CAS_MAX_NUM_OF_SC; j++ )
				{
					switch (Source)
					{
						case	RESOURCE_GLOBAL_SOURCE:
							pstIrTpInfo->GlobalStatus[j] = statusInfo;
							break;
						case	RESOURCE_INTERNAL_SOURCE:
							pstIrTpInfo->InternalStatus[j] = statusInfo;
							break;
						case	RESOURCE_EMM_SOURCE:
							pstIrTpInfo->EmmRscStatus[j].CharSeverity		= CharSeverity;
							pstIrTpInfo->EmmRscStatus[j].usResourceID		= (HUINT16)usEsPIDorRscID;
							pstIrTpInfo->EmmRscStatus[j].usEmmPID			= (HUINT16)usEcmOrEmmPID;
							pstIrTpInfo->EmmRscStatus[j].stStatus.eSource	= status.eSource;
							pstIrTpInfo->EmmRscStatus[j].stStatus.wStatus	= status.wStatus;
							pstIrTpInfo->EmmRscStatus[j].stStatus.eSeverity= status.eSeverity;
							pstIrTpInfo->EmmRscStatus[j].Valid				= TRUE;
							break;
						case	RESOURCE_SMARTCARD:
							pstIrTpInfo->SmartcardRscStatus[j] = statusInfo;
							break;
						case	RESOURCE_MONITORING:
	//						if ((status.eSeverity == STATUS_INFO_DND) && (status.wStatus == 100))
	//							pstIrTpInfo->ucIsMonitoring = 1;
							break;
					}
				}
			}
			break;
*/
#if	defined(IRPLAT_SYS_PVR)
	#if	defined(IRPLAT_PVR_PHASE12)
		case	SERVICE_PVR_RECORD:
			{
				IR_PVRSERVICE_INFO	*pstPVRSvcInfo;

				pstPVRSvcInfo = CAS_IR_Drv_FindPVRSvcInfoBySCELL(wServiceHandle);
				if (pstPVRSvcInfo)
				{
					switch (Source)
					{
						case	RESOURCE_GLOBAL_SOURCE:
							{
								HUINT16 	usCodeIndex;

								pstPVRSvcInfo->GlobalStatus = statusInfo;
//	Contents Id는 중복되는 경우가 있음.
//	PVRInfo PTR로 찾는것이 맞음.
#if	0
								pstIrSvcInfo = local_cas_ir_Drv_FindIrSvcInfoByContentsId(pstPVRSvcInfo->ulContentId);
#else
								pstIrSvcInfo = CAS_IR_DRV_FindIrSvcInfoByPvrInfo((void*)pstPVRSvcInfo);
#endif
								if (pstIrSvcInfo)
								{
									HxLOG_Info(" need to get error code  4th\n");
									usCodeIndex = CAS_IR_DRV_GetErrorCodeIndex(&status);
									if (status.eSeverity != STATUS_INFO_DND)
										CAS_IR_DRV_Setup_Record(pstIrSvcInfo->hIrSvc, IR_RECORD_TYPE_NOMSK, usCodeIndex);
									else if (usCodeIndex == AXI_STATUSCODE_IDX_D100_27)
										CAS_IR_DRV_Setup_Record(pstIrSvcInfo->hIrSvc, IR_RECORD_TYPE_ENCRYPTION, usCodeIndex);
								}
							}
							break;
						case	RESOURCE_INTERNAL_SOURCE:
							pstPVRSvcInfo->InternalStatus = statusInfo;
							break;
						case	RESOURCE_SESSION_MANAGEMENT:
							pstPVRSvcInfo->SessionManageRscStatus = statusInfo;
							break;
						case	RESOURCE_CRYPTO_OPERATION:
							pstPVRSvcInfo->CryptoOperationRscStatus = statusInfo;
							break;
						case	RESOURCE_SESSION_SUB:
							pstPVRSvcInfo->SessionSubStatus = statusInfo;
							break;
						case	RESOURCE_SMARTCARD:
							break;
#if defined(IRPLAT_FN_COPY_CONTROL)
						case	RESOURCE_MACROVISION:
							break;
#endif
						default:
							HxLOG_Print("SERVICE_RECORD : Invalid Resource Type = %x\n", Source);
							break;
					}

					HxLOG_Print("\t\t\t~~~~~~~~~ %c%03d-%d ~~~~~~~~\n\n", CharSeverity, status.wStatus, status.eSource);
				}
			}
			break;

		case	SERVICE_PVR_PLAYBACK:
			{
				IR_PVRSERVICE_INFO		*pstPVRSvcInfo;

				pstPVRSvcInfo = CAS_IR_Drv_FindPVRSvcInfoBySCELL(wServiceHandle);
				if (pstPVRSvcInfo)
				{
					switch (Source)
					{
						case	RESOURCE_GLOBAL_SOURCE:
							{
								HUINT16 	usCodeIndex;
								HUINT32		ulStatus;

								ulStatus = MAKE_IR_STATUSCODE(CharSeverity, status.wStatus, status.eSource);
								pstPVRSvcInfo->GlobalStatus = statusInfo;

//	Contents Id는 중복되는 경우가 있음.
//	PVRInfo PTR로 찾는것이 맞음.
#if	0
								pstIrSvcInfo = local_cas_ir_Drv_FindIrSvcInfoByContentsId(pstPVRSvcInfo->ulContentId);
#else
								pstIrSvcInfo = CAS_IR_DRV_FindIrSvcInfoByPvrInfo((void*)pstPVRSvcInfo);
#endif
								if (pstIrSvcInfo)
								{
									HxLOG_Info(" need to get error code  5th\n");
									usCodeIndex = CAS_IR_DRV_GetErrorCodeIndex(&status);
									if (usCodeIndex == AXI_STATUSCODE_IDX_D100_28)
										CAS_IR_DRV_Setup_Playback(pstIrSvcInfo->hIrSvc, IR_RECORD_TYPE_ENCRYPTION, usCodeIndex, ulStatus);
									else if (status.eSeverity != STATUS_INFO_DND)
										CAS_IR_DRV_Setup_Playback(pstIrSvcInfo->hIrSvc, IR_RECORD_TYPE_NOMSK, usCodeIndex, ulStatus);
								}
							}
							break;
						case	RESOURCE_INTERNAL_SOURCE:
							pstPVRSvcInfo->InternalStatus = statusInfo;
							break;
						case	RESOURCE_SESSION_MANAGEMENT:
							pstPVRSvcInfo->SessionManageRscStatus = statusInfo;
							break;
						case	RESOURCE_CRYPTO_OPERATION:
							pstPVRSvcInfo->CryptoOperationRscStatus = statusInfo;
							break;
						case	RESOURCE_SESSION_SUB:
							pstPVRSvcInfo->SessionSubStatus = statusInfo;
							break;
						case	RESOURCE_SMARTCARD:
							break;
						default:
							HxLOG_Print("[IR_DRV] ERROR - SERVICE_PLAYBACK : Invalid Resource Type = 0x%X\n", Source);
							break;
					}
					HxLOG_Print("\t\t\t~~~~~~~~~ %c%03d-%d ~~~~~~~~\n\n", CharSeverity, status.wStatus, status.eSource);
				}
			}
			break;
#endif
	#if	defined(IRPLAT_PVR_DRM)
		case	SERVICE_PVR_DRM:
			{
				IR_PVRSERVICE_INFO		*pstPVRSvcInfo;

				pstPVRSvcInfo = CAS_IR_Drv_FindPVRSvcInfoBySCELL(wServiceHandle);
				if (pstPVRSvcInfo)
				{
					switch (Source)
					{
						case	RESOURCE_GLOBAL_SOURCE:
							{
								HUINT16 	usCodeIndex;

								pstPVRSvcInfo->GlobalStatus = statusInfo;
//	Contents Id는 중복되는 경우가 있음.
//	PVRInfo PTR로 찾는것이 맞음.
#if	0
								pstIrSvcInfo = local_cas_ir_Drv_FindIrSvcInfoByContentsId(pstPVRSvcInfo->ulContentId);
#else
								pstIrSvcInfo = CAS_IR_DRV_FindIrSvcInfoByPvrInfo(pstPVRSvcInfo);
#endif
								if (pstIrSvcInfo)
								{
									HxLOG_Info(" need to get error code  4th\n");
									usCodeIndex = CAS_IR_DRV_GetErrorCodeIndex(&status);
									if (pstIrSvcInfo->eIrSvcActionType == IR_SVC_PLAYBACK)
									{
										HUINT32		ulStatus;

										ulStatus = MAKE_IR_STATUSCODE(CharSeverity, status.wStatus, status.eSource);
										if ((status.eSeverity == STATUS_ERROR) || (status.eSeverity == STATUS_FATAL))
											CAS_IR_DRV_Setup_Playback(pstIrSvcInfo->hIrSvc, IR_RECORD_TYPE_DRM_ERROR, usCodeIndex, ulStatus);
										else
											CAS_IR_DRV_Setup_Playback(pstIrSvcInfo->hIrSvc, IR_RECORD_TYPE_DRM, usCodeIndex, ulStatus);
									} else
									{
										if ((status.eSeverity == STATUS_ERROR) || (status.eSeverity == STATUS_FATAL))
											CAS_IR_DRV_Setup_Record(pstIrSvcInfo->hIrSvc, IR_RECORD_TYPE_DRM_ERROR, usCodeIndex);
										else
										{
											CAS_IR_DRV_Setup_Record(pstIrSvcInfo->hIrSvc, IR_RECORD_TYPE_DRM, usCodeIndex);
										}
									}
#if	0
									if (status.eSeverity != STATUS_INFO_DND)
										CAS_IR_DRV_Setup_Record(pstIrSvcInfo->hIrSvc, IR_RECORD_TYPE_NOMSK, usCodeIndex);
									else if (usCodeIndex == AXI_STATUSCODE_IDX_D100_27)
										CAS_IR_DRV_Setup_Record(pstIrSvcInfo->hIrSvc, IR_RECORD_TYPE_ENCRYPTION, usCodeIndex);
#endif
								}
							}
							break;
						case	RESOURCE_INTERNAL_SOURCE:
							pstPVRSvcInfo->InternalStatus = statusInfo;
							break;
						case	RESOURCE_SESSION_MANAGEMENT_DRM:
							pstPVRSvcInfo->SessionManageRscStatus = statusInfo;
							break;
						case	RESOURCE_CRYPTO_OPERATION:
							pstPVRSvcInfo->CryptoOperationRscStatus = statusInfo;
							break;
						case	RESOURCE_SESSION_SUB:
							pstPVRSvcInfo->SessionSubStatus = statusInfo;
							break;
						case	RESOURCE_SMARTCARD:
							break;
#if defined(IRPLAT_FN_COPY_CONTROL)
						case	RESOURCE_MACROVISION:/*RESOURCE_COPY_CONTROL*/
						{
							HUINT16	usCCTypeMask;
							HUINT8	ucMcMode, ucDigitalCci, ucAnalogCci;

							pstPVRSvcInfo->MacrovisionRscStatus = statusInfo;
							pstIrSvcInfo = CAS_IR_DRV_FindIrSvcInfoByPvrInfo(pstPVRSvcInfo);

							usCCTypeMask = (HUINT16)((0xFF000000 & usEcmOrEmmPID) >> 24);
							ucMcMode = (HUINT8)((0x00FF0000 & usEcmOrEmmPID) >> 16);
							ucDigitalCci = (HUINT8)((0x0000FF00 & usEcmOrEmmPID) >> 8);
							ucAnalogCci = (HUINT8)(0x000000FF & usEcmOrEmmPID);
							HxLOG_Error("DRM Playback Copy Control[type 0x%04x], MC[%d], DT[%d], AN[%d]\n", usCCTypeMask, ucMcMode, ucDigitalCci, ucAnalogCci);
							CAS_IR_MGR_NotifyCopyControlEvent(pstIrSvcInfo->hAction, usCCTypeMask, ucMcMode, ucDigitalCci, ucAnalogCci);
						}
						break;
#endif

							break;
#if	0		//	CA_DRV에서 해당 값 입력함.
						case	RESOURCE_DRM_RIGHT:
							pstPVRSvcInfo->ulCRID = ulResourceID;
							pstPVRSvcInfo->ulRightRecId = usEsPIDorRscID;
							pstPVRSvcInfo->ulSeqRecId = usEcmOrEmmPID;

							//	해당 정보 Write를 CAS보다는 AP에서 하는게 좋음.
							//	PB할 경우에는 CAS에서 하고Record는 왜 AP에서 하냐 라고 하면
							//	할말이 없는데...
							//	여튼 Write는 ap_record_irdeto.c에 존재함.
							//	MV_NONE or CLEAR받으면 우선 처리.
							//	가라 코드임..
							break;
#endif
						default:
							HxLOG_Print("[IR_DRV] ERROR - SERVICE_PVR_DRM : Invalid Resource Type = 0x%X\n", Source);
							break;
					}
				}
				HxLOG_Print("\t\t\t~~~~~~~~~ %c%03d-%d ~~~~~~~~\n\n", CharSeverity, status.wStatus, status.eSource);
			}
			break;
	#endif
#endif

		default:
			HxLOG_Print("Not Supported Service TYPE [%x]\n", eSvcType);
			break;
	}
}

/********************************************************************
*	function name :	 CAS_IR_DRV_AnyProductQuery
*	input :
*	output :
*	description :
********************************************************************/
HUINT32 CAS_IR_DRV_AnyProductQuery(HUINT32 resourceId)
{
	ia_result	ret;
	msg_any_product_query_st *pAnyProductQuery;

	pAnyProductQuery = (msg_any_product_query_st *)OxCAS_Malloc(sizeof(msg_any_product_query_st));
	pAnyProductQuery->wResourceId   = resourceId;
	pAnyProductQuery->bGracePeriod  = 0;
	pAnyProductQuery->fKeepTrack    = IA_TRUE; // TODO: Must be to be checked.
	pAnyProductQuery->fAnySector    = IA_TRUE;
	pAnyProductQuery->bSectorCount  = 0;

	HxLOG_Print("[SCELL_Message] MSG_ANY_PRODUCT_QUERY\n");
	ret = SCELL_Message(MSG_ANY_PRODUCT_QUERY, 0, sizeof(msg_any_product_query_st), pAnyProductQuery);
	g_IrDrvScInfo[resourceId].bProductInit = IA_FAIL;

	if(IA_SUCCESS != ret)
	{
		return HIR_NOT_OK;
	}

	return HIR_OK;
}

/********************************************************************************
********************************************************************************/
HIR_ERR CAS_IR_DRV_QueryScData(HUINT32 ulScRscId)
{
	ia_result	ret;
	msg_smartcard_query_data_st	*psmartcard_query_data;

	psmartcard_query_data = (msg_smartcard_query_data_st *)OxCAS_Malloc(sizeof(msg_smartcard_query_data_st));
	psmartcard_query_data->wResourceId = ulScRscId;
	HxLOG_Print("[SCELL_Message] MSG_SMARTCARD_DATA_QUERY: \t");
	ret = SCELL_Message(MSG_SMARTCARD_DATA_QUERY, 0, sizeof(msg_smartcard_query_data_st), (void *)psmartcard_query_data);
	HxLOG_Print("%s\n", (IA_SUCCESS != ret) ? "FAIL" :"OK" );

	if(IA_SUCCESS != ret)
	{
		return HIR_NOT_OK;
	}
	return HIR_OK;
}

HIR_ERR CAS_IR_DRV_QueryScNationality (HUINT32 ulScRscId)
{
	ia_result	ret;
	msg_smartcard_query_data_st	*psmartcard_query_data;

	psmartcard_query_data = (msg_smartcard_query_data_st *)CA_MEM_Alloc(sizeof(msg_smartcard_query_data_st));
	psmartcard_query_data->wResourceId = ulScRscId;
	HxLOG_Info("[SCELL_Message] MSG_NATIONALITY_QUERY: \t");
	ret = SCELL_Message(MSG_NATIONALITY_QUERY, 0, sizeof(msg_smartcard_query_data_st), (void *)psmartcard_query_data);
	HxLOG_Info("%s\n", (IA_SUCCESS != ret) ? "FAIL" :"OK" );

	if(IA_SUCCESS != ret)
	{
		return HIR_NOT_OK;
	}
	return HIR_OK;
}

HIR_ERR CAS_IR_DRV_RegionQuery(HUINT32 ulScRscId, HUINT8 ucSector)
{
	ia_result	ret;
	msg_region_query_st	*pregion_query_data;

	pregion_query_data = (msg_region_query_st *)OxCAS_Malloc(sizeof(msg_region_query_st));
	pregion_query_data->wResourceId = ulScRscId;
	pregion_query_data->bSector = ucSector;
	HxLOG_Print("[SCELL_Message] MSG_SMARTCARD_REGION_QUERY: \t");
	ret = SCELL_Message(MSG_REGION_QUERY, 0, sizeof(msg_region_query_st), (void *)pregion_query_data);
	HxLOG_Print("%s\n", (IA_SUCCESS != ret) ? "FAIL" :"OK" );

	if(IA_SUCCESS != ret)
	{
		return HIR_NOT_OK;
	}
	return HIR_OK;
}

/********************************************************************************
********************************************************************************/
HUINT32 CAS_IR_DRV_HomingChannelQuery(HUINT32 resourceId)
{
	ia_result	ret;
	msg_smartcard_query_data_st	*scQueryData;

	scQueryData = (msg_smartcard_query_data_st *)OxCAS_Malloc(sizeof(msg_smartcard_query_data_st));
	scQueryData->wResourceId = resourceId;

	HxLOG_Print("[SCELL_Message] MSG_SMARTCARD_HOMING_CH_QUERY\n");
	ret = SCELL_Message(MSG_SMARTCARD_HOMING_CH_QUERY, 0, sizeof(msg_smartcard_query_data_st), (void *)scQueryData);

	if(IA_SUCCESS != ret)
	{
		return HIR_NOT_OK;
	}

	return HIR_OK;
}

#if defined(IRPLAT_FN_QUERY_VENDOR_ID)
/********************************************************************************
********************************************************************************/
HIR_ERR CAS_IR_DRV_QueryVendorID(HUINT32 resourceId)
{
	ia_result	ret;
	msg_vendor_id_query_st *pVendorIDQuery;

	HxLOG_Info("\n");

	pVendorIDQuery = (msg_vendor_id_query_st *)OxCAS_Malloc(sizeof(msg_vendor_id_query_st));
	pVendorIDQuery->wResourceId   = resourceId;

	ret = SCELL_Message(MSG_SMARTCARD_VENDOR_ID_QUERY, 0, sizeof(msg_vendor_id_query_st), pVendorIDQuery);

	if(IA_SUCCESS != ret)
	{
		return HIR_NOT_OK;
	}

	return HIR_OK;
}
#endif

#if defined(IR_INST_CHANGE_BY_CAS_WORK_WELL)
HBOOL	CAS_IR_STATUS_IsCamAvailable(void)
{
	HUINT32	ulState;

	CASINST_GetCamState(0, &ulState);
	if (ulState == 0)
		return FALSE;
	else
		return TRUE;
}

void	CAS_IR_STAUS_SetIrdetoDescState(HBOOL irDescState)
{
	ir_desc_well = irDescState;

	return;
}

HBOOL	CAS_IR_STAUS_IsIrdetoDescrambleWell(void)
{
	return ir_desc_well;
}

HBOOL	CAS_IR_STATUS_IsIrdetoStatusError(ia_status_st stStatus)
{
	if(CAS_IR_STATUS_IsCamAvailable() == FALSE)
	{
		// CI - CAM 없음.
		HxLOG_Info("NO CAM ~ \n");
		return FALSE;
	}

	if ((stStatus.eSeverity == STATUS_ERROR) && (stStatus.eSource == 9) && \
		(stStatus.wStatus == 106) )
	{
		HxLOG_Info("E106-9 Error !!! \n");
		return TRUE;
	}
	else if ((stStatus.eSeverity == STATUS_ERROR) && (stStatus.eSource == 4) && \
		(stStatus.wStatus == 16) )
	{
		HxLOG_Info("E16-4 Error !!! \n");
		return TRUE;
	}
	else if ((stStatus.eSeverity == STATUS_ERROR) && (stStatus.eSource == 4) && \
		(stStatus.wStatus == 19) )
	{
		HxLOG_Info("E19-4 Error !!! \n");
		return TRUE;
	}
	// 필요한 IR Error Status를 추가.

	else
	{
		HxLOG_Info("NO Specific Error~ \n");
		return FALSE;
	}
}

#endif

/********************************************************************************
********************************************************************************/
#if defined(CONFIG_MW_CAS_IRDETO_HOMINGCHANNEL) || defined(CONFIG_MW_CAS_IRDETO_RECOVERY_RESCAN_HOME)
HUINT32 CAS_IR_DRV_GetHomeChannelInfo(HUINT8 ucSlotID, IR_SC_HOME_CHANNEL_INFO* pHomeChannel)
{
	HVD_Result hErr;
	HBOOL bInserted;

	HxLOG_Info("\n");

	if( (ucSlotID != 0) || (pHomeChannel == NULL ))
		return HIR_NOT_OK;

	hErr = HUMAX_VD_SC_GetCardPhysicalStatus(ucSlotID, &bInserted);
	if((hErr != eHVD_OK) || (bInserted != TRUE))
		return HIR_NOT_OK;

	HxSTD_memcpy(pHomeChannel, &(g_IrDrvScInfo[ucSlotID].stIrScApiInfo.stHomingInfo), sizeof(IR_SC_HOME_CHANNEL_INFO) );

	return HIR_OK;
}

HUINT32 xsvc_cas_IrGetHomeChannelInfo(IR_SC_HOME_CHANNEL_INFO *pHomeChannel)
{
	/* support only slot id =0, and signgle slot platform */
	HUINT32	ret;
	IR_API_SC_INFO stIrScInfo;

	HxLOG_Info("\n");

	ret = CAS_IR_DRV_GetHomeChannelInfo(0, pHomeChannel);
	if(ret != HIR_OK)
	{
		HxLOG_Print("[API_IR] ERROR - GetHomeChannelInfo\n");
		return HIR_NOT_OK;
	}

	HxLOG_Print("[CAS_IR_GetHomeChannelInfo]dwFreq=%d, dwSymbolRate=%d\n",
		pHomeChannel->dwFreq, pHomeChannel->dwSymbolRate);

	return HIR_OK;
}

#endif


#if defined(CONFIG_OP_DIGITURK)
HIR_ERR xsvc_cas_IrDrvRemoteChannelQuery(HUINT32 ulScRscId, HUINT8* pucBuf, HUINT32 ulLength)
{
	msg_remote_channel_message_st	stRemoteChannelMessage;
	ia_result						rtVal;
	HUINT8* 						pucMessage	= NULL;
	HUINT8*							pucStart	= NULL;
	HUINT8							ucMsgLen	= (sizeof(msg_remote_channel_message_st) + (sizeof(HUINT8) * ulLength));

	pucMessage = (HUINT8 *)CA_MEM_Alloc(ucMsgLen);
	pucStart = pucMessage;	// pointer 시작점 저장
	HxSTD_memset(pucMessage, 0x00, sizeof(HUINT8) * ulLength);

	HxSTD_memset(&stRemoteChannelMessage, 0x00, sizeof(msg_remote_channel_message_st));
	stRemoteChannelMessage.wResourceId = ulScRscId;
	stRemoteChannelMessage.bLength = (HUINT8)ulLength;
	HxSTD_memcpy(pucMessage, &stRemoteChannelMessage, sizeof(msg_remote_channel_message_st));

	pucMessage += sizeof(msg_remote_channel_message_st);
	HxSTD_memcpy(pucMessage, pucBuf, sizeof(HUINT8) * ulLength);

	HxLOG_Info("[SCELL_Message] MSG_REMOTE_MESSAGE_QUERY\n");

	pucMessage = pucStart;	// 저장한 시작점 pointer로
	rtVal = SCELL_Message(MSG_REMOTE_MESSAGE_QUERY, 0, ucMsgLen, (void*)pucMessage);

	if (rtVal != IA_SUCCESS)
	{
		return HIR_NOT_OK;
	}

	return HIR_OK;
}

HIR_ERR xsvc_cas_IrDrvQueryProductCheck(HUINT32 ulScRscId, HUINT8 ucGracePeriod, ia_bool bKeepTrack,
																		HUINT8 ucProductCount, HUINT32* pstProduct)
{
	ia_result	ret;
	msg_product_check_query_st stProductCheckQuery;

	msg_product_check_query_product_st *stProduct;
	HUINT8 		*pDataToSend;

	pDataToSend = (HUINT8 *)CA_MEM_Alloc(sizeof(msg_product_check_query_st) + (sizeof(msg_product_check_query_product_st)*ucProductCount));
	stProductCheckQuery.wResourceId   = ulScRscId;
	stProductCheckQuery.bGracePeriod  = ucGracePeriod;
	stProductCheckQuery.fKeepTrack    = bKeepTrack;
	stProductCheckQuery.bProductCount  = ucProductCount;

	HxSTD_memcpy(pDataToSend, &stProductCheckQuery, sizeof(msg_product_check_query_st));
	if(pstProduct)
	{
		HxSTD_memcpy(pDataToSend + sizeof(msg_product_check_query_st), pstProduct, (sizeof(msg_product_check_query_product_st)*ucProductCount));
	}
	else
	{
		HxLOG_Info("[%s:%d] pstProduct is NULL\n",__FILE__, __LINE__);
		CA_MEM_Free(pDataToSend);

		return HIR_NOT_OK;
	}

	stProduct = (msg_product_check_query_product_st *)pstProduct;

	HxLOG_Info("[%s:%d] sector = %d, anysector = %d, product = %d, 0x%x \n",__FILE__, __LINE__,
							stProduct->bSector,stProduct->fAnySector, stProduct->wProduct,stProduct->wProduct);
	HxLOG_Info("[%s:%d] size of msg_product_check_query_product_st = %d \n",__FILE__, __LINE__,
																		sizeof(msg_product_check_query_product_st));
	HxLOG_Info("[SCELL_Message] MSG_PRODUCT_LIST_QUERY\n");
	ret = SCELL_Message(MSG_PRODUCT_CHECK_QUERY, 0, sizeof(msg_product_check_query_st) + (sizeof(msg_product_check_query_product_st)*ucProductCount), pDataToSend);

	if(IA_SUCCESS != ret)
	{
		return HIR_NOT_OK;
	}

	return HIR_OK;
}

HIR_ERR xsvc_cas_IrDrvQueryTmsData(HUINT32 ulScRscId)
{
	ia_result ret;
	msg_smartcard_tms_query_data_st *pstScData;

	pstScData = (msg_smartcard_tms_query_data_st *)CA_MEM_Alloc(sizeof(msg_smartcard_tms_query_data_st));
	pstScData->wResourceId = ulScRscId;

	HxLOG_Info("[SCELL_Message] MSG_SMARTCARD_TMS_DATA_QUERY\n");
	ret = SCELL_Message(MSG_SMARTCARD_TMS_DATA_QUERY, 0, sizeof(msg_smartcard_tms_query_data_st), (void *)pstScData);

	if(IA_SUCCESS != ret)
	{
		return HIR_NOT_OK;
	}

	return HIR_OK;
}
#endif

/********************************************************************************
********************************************************************************/
HIR_ERR xsvc_cas_IrDrvQueryProductList(HUINT32 ulScRscId, HUINT8 ucGracePeriod, ia_bool bKeepTrack,
	ia_bool bAnySector, HUINT8 ucSectorCount, HUINT8* pucSectorList)
{
	ia_result	ret;
	msg_product_list_query_st stAnyProductQuery;
	HUINT8 *pDataToSend;

	pDataToSend = (HUINT8 *)OxCAS_Malloc(sizeof(msg_any_product_query_st) + ucSectorCount);
	stAnyProductQuery.wResourceId   = ulScRscId;
	stAnyProductQuery.bGracePeriod  = ucGracePeriod;
	stAnyProductQuery.fKeepTrack    = bKeepTrack;
	stAnyProductQuery.fAnySector    = bAnySector;
	stAnyProductQuery.bSectorCount  = ucSectorCount;

	HxSTD_memcpy(pDataToSend, &stAnyProductQuery, sizeof(msg_any_product_query_st));
	if(pucSectorList)
		HxSTD_memcpy(pDataToSend + sizeof(msg_any_product_query_st), pucSectorList, ucSectorCount);

	HxLOG_Print("[SCELL_Message] MSG_ANY_PRODUCT_QUERY\n");
	ret = SCELL_Message(MSG_PRODUCT_LIST_QUERY, 0, sizeof(msg_any_product_query_st) + ucSectorCount, pDataToSend);

	if(IA_SUCCESS != ret)
	{
		return HIR_NOT_OK;
	}

	return HIR_OK;
}


/********************************************************************************
********************************************************************************/
#if defined(IRPLAT_SC_QUERY_EXTENDED_PRODUCT_LIST_ID)
HIR_ERR CAS_IR_DRV_QueryExtendedProductList(HUINT32 ulScRscId, ia_bool bAnySector, HUINT8 ucSectorCount, HUINT8* pucSectorList)
{
	ia_result	ret;
	msg_extended_product_list_query_st stExtendedProductQuery;
	HUINT8 *pDataToSend;

	pDataToSend = (HUINT8 *)CA_MEM_Alloc(sizeof(msg_extended_product_list_query_st) + ucSectorCount);
	stExtendedProductQuery.wResourceId   = ulScRscId;
	stExtendedProductQuery.fAnySector    = bAnySector;
	stExtendedProductQuery.bSectorCount  = ucSectorCount;

	HxSTD_memcpy(pDataToSend, &stExtendedProductQuery, sizeof(msg_extended_product_list_query_st));
	if(pucSectorList)
		HxSTD_memcpy(pDataToSend + sizeof(msg_extended_product_list_query_st), pucSectorList, ucSectorCount);

	HxLOG_Info("[SCELL_Message] MSG_EXTENDED_PRODUCT_LIST_QUERY\n");
	ret = SCELL_Message(MSG_EXTENDED_PRODUCT_LIST_QUERY, 0, sizeof(msg_extended_product_list_query_st) + ucSectorCount, pDataToSend);

	if(IA_SUCCESS != ret)
	{
		return HIR_NOT_OK;
	}

	return HIR_OK;
}
#endif

/********************************************************************************
********************************************************************************/
HIR_ERR xsvc_cas_IrDrvQueryUserData(HUINT32 ulScRscId)
{
	ia_result ret;
	msg_smartcard_query_data_st *pstScData;

	pstScData = (msg_smartcard_query_data_st *)OxCAS_Malloc(sizeof(msg_smartcard_query_data_st));
	pstScData->wResourceId = ulScRscId;

	HxLOG_Print("[SCELL_Message] MSG_ANY_PRODUCT_QUERY\n");
	ret = SCELL_Message(MSG_SMARTCARD_USER_DATA_QUERY, 0, sizeof(msg_smartcard_query_data_st), (void *)pstScData);

	if(IA_SUCCESS != ret)
	{
		return HIR_NOT_OK;
	}

	return HIR_OK;
}


#define _____STATUS_SCREEN_____
HIR_ERR xsvc_cas_IrGetClientVersionString(HUINT8 *pucClientVersion)
{
	HxLOG_Info("\n");

	if( ClientVersionData[0].usCount )
	{
		if( ClientVersionData[0].VersionData.wBuildInfoLength <= 1 )
		{
			snprintf((char *)pucClientVersion, 256, (char *)"%s Version : %d.%d.%d", (char *)(ClientVersionData[0].pucNameString), ClientVersionData[0].VersionData.bMajor,
										ClientVersionData[0].VersionData.bMinor, (HUINT32)ClientVersionData[0].VersionData.wSubVersion);
		}
		else
		{
			snprintf((char *)pucClientVersion, 256, (char *)"%s : %d.%d.%d, %s", (char *)(ClientVersionData[0].pucNameString), ClientVersionData[0].VersionData.bMajor,
										ClientVersionData[0].VersionData.bMinor, (HUINT32)ClientVersionData[0].VersionData.wSubVersion,
										(char *)(ClientVersionData[0].pucBuildInfoString));
		}

		HxLOG_Print("[%s]\n", pucClientVersion);

		return HIR_OK;
	}
	else
	{
		HxLOG_Error("Cannot get ClientVersion\n");

		return HIR_NOT_OK;
	}
}

HIR_ERR xsvc_cas_IrGetClientVersion(HUINT16 *pusLength, HUINT8 **ppString)
{
	HxLOG_Info("\n");

	if( ClientVersionData[0].usCount )
	{
		*pusLength = (HUINT16)(strlen((char *)(ClientVersionData[0].pucNameString)) + strlen((char *)(ClientVersionData[0].pucBuildInfoString))
			+ 30);  // TODO: <-- what's that?

		HxLOG_Print("Version string : length = %d\n", *pusLength);

		*ppString = OxCAS_Malloc(*pusLength);
		HxSTD_memset(*ppString, 0, *pusLength);
		if( ClientVersionData[0].VersionData.wBuildInfoLength <= 1 )
		{
			snprintf((char *)*ppString, (HINT32)*pusLength, (char *)"%s Version : %d.%d.%d", (char *)(ClientVersionData[0].pucNameString), ClientVersionData[0].VersionData.bMajor,
										ClientVersionData[0].VersionData.bMinor, (HUINT32)ClientVersionData[0].VersionData.wSubVersion);
		}
		else
		{
			snprintf((char *)*ppString, (HINT32)*pusLength, (char *)"%s : %d.%d.%d, %s", (char *)(ClientVersionData[0].pucNameString), ClientVersionData[0].VersionData.bMajor,
										ClientVersionData[0].VersionData.bMinor, (HUINT32)ClientVersionData[0].VersionData.wSubVersion,
										(char *)(ClientVersionData[0].pucBuildInfoString));
		}

		HxLOG_Print("[%s]\n", *ppString);

		return HIR_OK;
	}
	else
	{
		HxLOG_Error("Cannot get ClientVersion\n");

		*pusLength = 0;
		return HIR_NOT_OK;
	}
}



#define _____SOFTCELL_SERVICE_STATUS_____
/********************************************************************
 Description: SoftCell에서 처리하고 있는 service들의 개수와 그 개수 만큼의 service information을 얻어내는 함수.
 		   여기서 service는 우리가 흔히 생각하는 channel의 개념이 아니고,
 		   SoftCell이 처리하는 하나의 단위. 즉, EMM도 하나의 EMM service가 된다.

			enum
			{
					// DVB services. //
				SERVICE_DVB_DESCRAMBLE = 1,
				SERVICE_DVB_EMM,
					// IPTV services. //
				SERVICE_IPTV_REGISTRATION_EMM,
				SERVICE_IPTV_DESCRAMBLE_VOD,
					// Cyphercast Services. //
				SERVICE_CC_DESCRAMBLER,
				SERVICE_CC_OPERATOR,
				SERVICE_PVR_RECORD,
				SERVICE_PVR_PLAYBACK
			};
 ********************************************************************/
#ifndef WIN32
HIR_ERR xsvc_cas_IrSvcStatusGetNumberOfServices(IR_SVC_STATUS_INFO_MAIN **stSvcStatusMain, HUINT8 *pucNumberOfServices)
{
	HxLOG_Info("\n");

	*pucNumberOfServices =  CAS_IR_DRV_GetNumberOfServices(stSvcStatusMain);

	return HIR_OK;
}

/********************************************************************
 Description: 각 service들의 세부적인 정보를 얻어내는 함수.
 		   DVB Client Integration Guide에 있는 service status menu를 그리는 spec에 맞게 맞추어진 정보임.
 ********************************************************************/
HIR_ERR xsvc_cas_IrSvcStatusGetInformationOfEachService(HUINT32 eSvcType, HUINT16 usHandle, IR_SVC_STATUS_INFO_EACH *stSvcStatusEach)
{
	HxLOG_Info(" eSvcType: (%d)\n", eSvcType);

	if( HIR_OK != CAS_IR_DRV_GetInformationOfEachService(eSvcType, usHandle, stSvcStatusEach) )
	{
		return HIR_NOT_OK;
	}

	return HIR_OK;
}


/**********************************************************************************
*         Not used Function for 2010.05.06
***********************************************************************************/
HIR_ERR  xsvc_cas_IrDrvGetGlobalStatusOfService(HUINT32 unPlayHandle, IR_STATE *pGlobalStatus)
{
	HxLOG_Info("\n");

	if( HIR_OK != CAS_IR_DRV_GetGlobalStatusOfService(unPlayHandle, pGlobalStatus) )
	{
		return HIR_NOT_OK;
	}
	return HIR_OK;
}

/********************************************************************
 Description: Element stream들의 정보를 얻어내는 함수.
 		   DVB Client Integration Guide에 있는 service status menu를 그리는 spec에 맞게 맞추어진 정보임.
 ********************************************************************/
#if	defined(IRPLAT_PVR_DRM) && defined(IRPLAT_SYS_PVR)
HIR_ERR	xsvc_cas_IrSvcStatusGetPlaybackDrmInfo(Handle_t hAction, IR_PVRDRM_INFO *pstPvrDrmInfo)
{
	IR_SERVICE_INFO	*pstSvcInfo;

	pstSvcInfo = CAS_IR_DRV_FindIrSvcInfoByActionId(hAction);
	if (pstSvcInfo)
	{
		if (pstSvcInfo->pstPVRSvcInfo)
		{
			memcpy(pstPvrDrmInfo, &pstSvcInfo->pstPVRSvcInfo->stPVRDrmInfo, sizeof(IR_PVRDRM_INFO));
			return HIR_OK;
		}
		return HIR_NOT_OK;
	}

	return HIR_NOT_OK;
}
#endif

HIR_ERR	xsvc_cas_IrSvcStatusGetGlobalStatus(Handle_t hAction, IR_STATE *pGlobalStatus)
{
#if defined(IRPLAT_PVR_DRM)
	IR_SERVICE_INFO	*pstIrSvcInfo;

	pstIrSvcInfo = CAS_IR_DRV_FindIrSvcInfoByActionId(hAction);
	if (pstIrSvcInfo)
	{
		GetIRState((*pGlobalStatus), pstIrSvcInfo->GlobalStatus);
		return HIR_OK;
	}
#endif
	return HIR_NOT_OK;

}

HUINT8 xsvc_cas_IrSvcStatusGetInformationOfElement(HUINT16 usHandle, HUINT32 ulResourceId, IR_ELEM_STATE **stSvcStatusElement)
{
	HxLOG_Info("\n");

	return CAS_IR_DRV_GetInformationOfElementary(usHandle, ulResourceId, stSvcStatusElement);
}




#define _____COMPONENTS_STATUS_____
HIR_ERR	xsvc_cas_IrComponentsCount (HUINT16 *pusCount)
{
	if (pusCount == NULL)
	{
		return HIR_INVALID_PARAMETER;
	}

	*pusCount	= ComponentsVersionData[0].usCount;

	return HIR_OK;
}



HIR_ERR	xsvc_cas_IrComponentsGetName (HUINT16 usIndex, HUINT8 *szName)
{
	SOFTCELL_VERSION_DATA *pData;

#define BUFF_SIZE 128

	if (szName == NULL)
	{
		return HIR_INVALID_PARAMETER;
	}

	if (usIndex >= ComponentsVersionData[0].usCount)
	{
		return HIR_INVALID_PARAMETER;
	}

	pData	= &ComponentsVersionData[usIndex];
	if (pData->usCount == 0 || pData->pucNameString == NULL)
	{
		return HIR_NOT_OK;
	}

	strncpy(szName, pData->pucNameString, BUFF_SIZE);

	return HIR_OK;
}

HIR_ERR	xsvc_cas_IrComponentsGetVersion (HUINT16 usIndex, HUINT8 *pucMajor, HUINT8 *pucMinor, HUINT32 *pulSubVer)
{
	SOFTCELL_VERSION_DATA *pData;

	if (pucMajor == NULL || pucMinor == NULL || pulSubVer == NULL)
	{
		return HIR_INVALID_PARAMETER;
	}

	if (usIndex >= ComponentsVersionData[0].usCount)
	{
		return HIR_INVALID_PARAMETER;
	}

	pData	= &ComponentsVersionData[usIndex];
	if (pData->usCount == 0)
	{
		return HIR_NOT_OK;
	}

	*pucMajor	= (HUINT8)pData->VersionData.bMajor;
	*pucMinor	= (HUINT8)pData->VersionData.bMinor;
	*pulSubVer	= (HUINT32)pData->VersionData.wSubVersion;

	return HIR_OK;
}


/********************************************************************
*	function name :	 xsvc_cas_IrGetComponentsVersion
*	input : ..
*	output :
*	description :
********************************************************************/
// TODO: Change paratermeters & method of gathering data.
HIR_ERR xsvc_cas_IrGetComponentsVersion(HUINT16	*pusCount, HUINT16 **pusLength, HUINT8 ***pString)
{
	HUINT8	i;

	HxLOG_Info("\n");

	if( 0 !=  ComponentsVersionData[0].usCount )
	{
		*pusCount = ComponentsVersionData[0].usCount;
		*pusLength = (HUINT16 *)OxCAS_Malloc(sizeof(HUINT16) * ComponentsVersionData[0].usCount);
		*pString = (HUINT8 **)OxCAS_Malloc(sizeof(HUINT32) * ComponentsVersionData[0].usCount);
		for( i = 0; i < ComponentsVersionData[0].usCount; i++ )
		{
			(*pusLength)[i] = strlen((char *)(ComponentsVersionData[i].pucNameString)) + strlen((char *)(ComponentsVersionData[i].pucBuildInfoString)) + 25;

			HxLOG_Print("[API_IR] GetComponentsVersion[%d] : length = %d\n", i, (*pusLength)[i] );

			(*pString)[i] = (HUINT8 *)OxCAS_Malloc((*pusLength)[i]);
			HxSTD_memset((*pString)[i], 0, (*pusLength)[i]);

			if( ComponentsVersionData[i].usCount )
			{
				if( ComponentsVersionData[i].VersionData.wBuildInfoLength <= 1 )
				{
					snprintf((char *)((*pString)[i]), (HINT32)(*pusLength)[i], "%s Version : %d.%d.%d",\
						ComponentsVersionData[i].pucNameString, \
						ComponentsVersionData[i].VersionData.bMajor,\
						ComponentsVersionData[i].VersionData.bMinor, \
						(HUINT32)ComponentsVersionData[i].VersionData.wSubVersion);
				}
				else
				{
					snprintf((char *)((*pString)[i]), (HINT32)(*pusLength)[i], "%s Version : %d.%d.%d, %s", \
						ComponentsVersionData[i].pucNameString, \
						ComponentsVersionData[i].VersionData.bMajor,\
						ComponentsVersionData[i].VersionData.bMinor, \
						(HUINT32)ComponentsVersionData[i].VersionData.wSubVersion,\
						ComponentsVersionData[i].pucBuildInfoString);
				}

				HxLOG_Print("[API_IR] GetClientVersion : [%s]\n", (*pString)[i] );
			}
		}
		return HIR_OK;
	}
	else
	{
		*pusCount = 0;
		return HIR_NOT_OK;
	}
}

HIR_ERR	xsvc_cas_IrStartMonitoring(HUINT16 ushSCELL, HUINT8 ucType)
{
	HxLOG_Info("\n");

	if( s_ucTypeForMonitor == IR_MONITOR_STOP )
	{
		switch(ucType)
		{
			case IR_MONITOR_ECM:
				CAS_IR_DRV_SendServiceDefineOfEcmMonitor(ushSCELL, 1);
				s_usSCELLforMonitor = ushSCELL;
				s_ucTypeForMonitor = ucType;
				break;

			case IR_MONITOR_EMM:
				CAS_IR_DRV_SendServiceDefineOfEmmMonitor(ushSCELL, 1);
				s_usSCELLforMonitor = ushSCELL;
				s_ucTypeForMonitor = ucType;
				break;
		}
	}
	return HIR_OK;
}

HIR_ERR	xsvc_cas_IrStopMonitoring(void)
{
	HxLOG_Info("\n");

	if( s_ucTypeForMonitor != IR_MONITOR_STOP )
	{
		switch(s_ucTypeForMonitor)
		{
			case IR_MONITOR_ECM:
				CAS_IR_DRV_SendServiceDefineOfEcmMonitor(s_usSCELLforMonitor, 0);
				break;

			case IR_MONITOR_EMM:
				CAS_IR_DRV_SendServiceDefineOfEmmMonitor(s_usSCELLforMonitor, 0);
				break;
		}

		s_ucTypeForMonitor = IR_MONITOR_STOP;
	}
	return HIR_OK;
}
#endif

