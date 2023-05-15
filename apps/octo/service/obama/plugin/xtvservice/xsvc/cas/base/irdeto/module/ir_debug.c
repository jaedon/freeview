/*******************************************************************
	File Description
********************************************************************/
/**
	@file	   	ir_debug.c \n
	@brief	main \n

	Description:   \n
	Module: MW/CAS/ IRDETO \n

	Copyright (c) 2010 HUMAX Co., Ltd. \n
	All rights reserved. \n
*/

#include <hlib.h>
#include <pal_pipe.h>
#include <pal_sc.h>

#include <_svc_cas_sub_dev_api.h>

#include "vd_humax.h"

#include "ir_svc.h"
#include "ir_msg_scell.h"
#include "ir_evt.h"
#include "ir_svc.h"
#include "ir_status.h"

#include "ir_nvr.h"
#include "ir_util.h"

#if	defined(IRPLAT_SYS_PVR)
	#include "ir_pvr.h"
	#include <svc_rec.h>
	#include <svc_pb.h>
	#include <svc_meta.h>
#endif

#if defined(CONFIG_MW_CAS_IRDETO_MATURITY_RATING)
#include "s3_maturity_rating.h"
#include "ir_mr.h"
#endif

#include <_svc_cas_mgr_nvram.h>
/********************************************************************
*	Definitions (for this source file)
********************************************************************/






/********************************************************************
*	Global variables
********************************************************************/
extern	IR_SERVICE_INFO			g_IrSvcInfo[IR_HUMAX_MAX_SERVICE];
extern	IR_TP_INFO				g_IrTpInfo[IR_HUMAX_MAX_TP];
extern	IR_DRV_SC_INFO			g_IrDrvScInfo[HUMAX_VD_SC_SLOT_COUNT];


extern HIR_ERR CAS_IR_MAIL_EraseFile(void);
extern HIR_ERR CAS_IR_NVR_Partition_Write( IRNVR_PART_Type ePartType, HUINT32 ulPartOffset, HUINT8 *pucData, HUINT32 ulSize);
extern void CAS_IR_MSG_DRV_DbgCntrMsg(Handle_t hAct, HUINT8 *pData, HUINT16 usLength);
extern HINT32 IR_MV_DBG_ForceEncryptionForAllChannel(HUINT8 ucEncryptFlag);
extern HERROR 		CAS_IR_MGR_NotifyCopyControlEvent(Handle_t hAction, HUINT16 usTypeMask,	HUINT8 ucMcMode, HUINT8 ucDigitalCci, HUINT8 ucAnologCci);


void DBG_IR_CAI_Event(HUINT32 type)
{

	switch(type)
	{
		case CASMGR_CAI_START_SERVICE:
			HLIB_CMD_Printf("[CASACT->IRMGR] CASMGR_CAI_START_SERVICE\n");		break;

		case CASMGR_CAI_STOP_SERVICE:
			HLIB_CMD_Printf("[CASACT->IRMGR] CASMGR_CAI_STOP_SERVICE\n");		break;

		case CASMGR_CAI_SET_PID:
			HLIB_CMD_Printf("[CASACT->IRMGR] CASMGR_CAI_SET_PID\n");				break;

		case CASMGR_CAI_SET_PIDLIST:
			HLIB_CMD_Printf("[CASACT->IRMGR] CASMGR_CAI_SET_PIDLIST\n");			break;

		case CASMGR_CAI_ONE_PID:
			HLIB_CMD_Printf("[CASACT->IRMGR] CASMGR_CAI_ONE_PID\n");				break;

		case CASMGR_CAI_RES_CHANGED:
			HLIB_CMD_Printf("[CASACT->IRMGR] CASMGR_CAI_RES_CHANGED\n");			break;

		case CASMGR_CAI_VIEWTYPE_CHANGED:
			HLIB_CMD_Printf("[CASACT->IRMGR] CASMGR_CAI_VIEWTYPE_CHANGED\n");	break;

		case CASMGR_DD_AV_STATUS:
			HLIB_CMD_Printf("[CASACT->IRMGR] CASMGR_DD_AV_STATUS\n");			break;

		case CASMGR_DD_TUNER_STATUS:
			HLIB_CMD_Printf("[CASACT->IRMGR] CASMGR_DD_TUNER_STATUS\n");			break;

		case CASMGR_CAI_START_TS:
			HLIB_CMD_Printf("[CASACT->IRMGR] CASMGR_CAI_START_TS\n");			break;

		case CASMGR_CAI_STOP_TS:
			HLIB_CMD_Printf("[CASACT->IRMGR] CASMGR_CAI_STOP_TS\n");				break;

		case CASMGR_CAI_START_RECORD:
			HLIB_CMD_Printf("[CASACT->IRMGR] CASMGR_CAI_START_RECORD\n");		break;

		case CASMGR_CAI_STOP_RECORD:
			HLIB_CMD_Printf("[CASACT->IRMGR] CASMGR_CAI_STOP_RECORD\n");			break;

		case CASMGR_CAI_START_PLAYBACK:
			HLIB_CMD_Printf("[CASACT->IRMGR] CASMGR_CAI_START_PLAYBACK\n");		break;

		case CASMGR_CAI_STOP_PLAYBACK:
			HLIB_CMD_Printf("[CASACT->IRMGR] CASMGR_CAI_STOP_PLAYBACK\n");		break;

		case CASMGR_SI_CAT_UPDATE:
			HLIB_CMD_Printf("[CASACT->IRMGR] CASMGR_SI_CAT_UPDATE\n");			break;

		case CASMGR_SI_PMT_UPDATE:
			HLIB_CMD_Printf("[CASACT->IRMGR] CASMGR_SI_PMT_UPDATE\n");			break;

		case CASMGR_SI_SDT_UPDATE:
			HLIB_CMD_Printf("[CASACT->IRMGR] CASMGR_SI_SDT_UPDATE\n");			break;

		case CASMGR_CAINST_SELECTDESELECT:
			HLIB_CMD_Printf("[CASACT->IRMGR] CASMGR_CAINST_SELECTDESELECT\n");	break;

		case CASMGR_CAINST_CHANGED:
			HLIB_CMD_Printf("[CASACT->IRMGR] CASMGR_CAINST_CHANGED\n");			break;
		default:
			break;
	}
}

HUINT32 local_IrDrv_Dbg_PrintScInfo(void)
{

	IR_DRV_SC_INFO 	IrScInfo;

	HLIB_CMD_Printf("\n");

	XSVC_CAS_IR_DRV_GetScInfo(0, &IrScInfo);

	HLIB_CMD_Printf("\n\n==================== SMARTCARD INFO ====================\n");

	HLIB_CMD_Printf("hIRSci= \t0x%X\n", IrScInfo.hIRSci);
	HLIB_CMD_Printf("ResourceID=\t0x%X\n", (int)IrScInfo.ResourceID);
	HLIB_CMD_Printf("ReferenceID=\t0x%X\n", (int)IrScInfo.ReferenceID);
	HLIB_CMD_Printf("SemaphoreID=\t0x%X\n", (int)IrScInfo.SemaphoreID);
	HLIB_CMD_Printf("CardPhysicalStatus=\t%d\n", IrScInfo.CardPhysicalStatus);
	HLIB_CMD_Printf("CardStatus=\t%d\n", IrScInfo.CardStatus);
	HLIB_CMD_Printf("RetryCount=\t%d\n", IrScInfo.RetryCount);
	HLIB_CMD_Printf("Protocol=\t%d\n", IrScInfo.Protocol);
	HLIB_CMD_Printf("AtrLen= \t%d\n", IrScInfo.ulAtrLen);
	HLIB_CMD_Printf("VersionFromATR= \t[%c][%c][%c][%c]\n", IrScInfo.VersionFromATR[0],IrScInfo.VersionFromATR[1], IrScInfo.VersionFromATR[2], IrScInfo.VersionFromATR[3] );
	HLIB_CMD_Printf("szID=\t%s\n", IrScInfo.szScID);
	HLIB_CMD_Printf("bSectors=\t%d\n", IrScInfo.bSectors);
	HLIB_CMD_Printf("eCardForm=\t%d\n", IrScInfo.eCardForm);

	HLIB_CMD_Printf("\n\n____________________________________________________________________\n");

	HLIB_CMD_Printf("Status: %s\n", IrScInfo.stIrScApiInfo.szStatus);

	HLIB_CMD_Printf("Status: %s\n", IrScInfo.stIrScApiInfo.szStatus);
	HLIB_CMD_Printf("Number: %s\n", IrScInfo.stIrScApiInfo.szNumber);
	HLIB_CMD_Printf("Type: %04X\n", IrScInfo.stIrScApiInfo.usType);
	HLIB_CMD_Printf("Version: %s\n", IrScInfo.stIrScApiInfo.szVersion);
	if( IrScInfo.stIrScApiInfo.szVersion[0] >= 0x35 )
	{
		HLIB_CMD_Printf("Build: %02d\n", IrScInfo.stIrScApiInfo.ucBuild);
		HLIB_CMD_Printf("Variant: %02X\n", IrScInfo.stIrScApiInfo.ucVariant);
		HLIB_CMD_Printf("Patch Level: %04X\n", IrScInfo.stIrScApiInfo.usPatchLevel);
		HLIB_CMD_Printf("Owner ID: %04X\n", IrScInfo.stIrScApiInfo.usOwnerID);
	}
	else
	{
		HLIB_CMD_Printf("Build:\n");
		HLIB_CMD_Printf("Variant:\n");
		HLIB_CMD_Printf("Patch Level:\n");
		HLIB_CMD_Printf("Owner ID:\n");
	}

	HLIB_CMD_Printf("Nationality: %s\n", IrScInfo.stIrScApiInfo.szNationality);

#if defined(CONFIG_MW_CAS_IRDETO_MATURITY_RATING)
	{
		HUINT8 	i;

		HLIB_CMD_Printf("==================== MATURITY RATING INFO ====================\n");

		HLIB_CMD_Printf("SC support for MR : %s\n", (IrScInfo.stIrScApiInfo.stMRInfo.bSCSupportMR==0)?"NOT-SUPPORT":"SUPPORT");
		HLIB_CMD_Printf("MR Configuration enabled: %s\n", (IrScInfo.stIrScApiInfo.stMRInfo.bEnabledMREval ==0)?"FALSE": "TRUE");
		HLIB_CMD_Printf("MR Configuration Query result: %s(%d)\n", (IrScInfo.stIrScApiInfo.stMRInfo.eConfigResult ==  IR_MR_CONFIG_OK)? "OK":"ERROR", IrScInfo.stIrScApiInfo.stMRInfo.eConfigResult);

		HLIB_CMD_Printf("_______________________USER INFO ____________________________\n");
		for(i=0;i<IRDETO_MR_MAX_NUMBERS_OF_USER; i++)
		{
			HLIB_CMD_Printf("\tProfile ID: %d,\tLevel: %d, \tPIN: %02X%02X \n",\
				IrScInfo.stIrScApiInfo.stMRInfo.ast_mr_user_profile[i].bProfileID, \
				IrScInfo.stIrScApiInfo.stMRInfo.ast_mr_user_profile[i].bUserAge, \
				IrScInfo.stIrScApiInfo.stMRInfo.ast_mr_user_profile[i].abUserPin[0],IrScInfo.stIrScApiInfo.stMRInfo.ast_mr_user_profile[i].abUserPin[1] );
		}
	}
#endif

#if defined(IRPLAT_SC_QUERY_EXTENDED_PRODUCT_LIST_ID)

	{
		HUINT8 	i, j;
		HUINT8	nProdCnt;

		HLIB_CMD_Printf("\n\n");
		HLIB_CMD_Printf("==================== Extended Product List ====================\n");

		for(i = 0; i < IrScInfo.stIrScApiInfo.nNumberOfSecotrs; i++)
		{
			nProdCnt = IrScInfo.stIrScApiInfo.stExtProductListData[i].stExtProductListSector.bProductCount;

			HLIB_CMD_Printf("===Sector %d ====================\n", IrScInfo.stIrScApiInfo.stExtProductListData[i].stExtProductListSector.bSector);

			HLIB_CMD_Printf("\t bSector: %d, fSectorValid: %d,  wSectorDateCode: 0x%04x, bProductCount: %d\n", \
						IrScInfo.stIrScApiInfo.stExtProductListData[i].stExtProductListSector.bSector, \
						IrScInfo.stIrScApiInfo.stExtProductListData[i].stExtProductListSector.fSectorValid, \
						IrScInfo.stIrScApiInfo.stExtProductListData[i].stExtProductListSector.wSectorDateCode, \
						IrScInfo.stIrScApiInfo.stExtProductListData[i].stExtProductListSector.bProductCount);

			for(j = 0; j < nProdCnt; j++)
			{
				HLIB_CMD_Printf("\t\t wProductId: 0x%04x,  wStartDate: 0x%04x, bDuration: %d\n", \
								IrScInfo.stIrScApiInfo.stExtProductListData[i].p_ExtProductListPorduct[j].wProductId, \
								IrScInfo.stIrScApiInfo.stExtProductListData[i].p_ExtProductListPorduct[j].wStartDate, \
								IrScInfo.stIrScApiInfo.stExtProductListData[i].p_ExtProductListPorduct[j].bDuration);
			}
		}

	}
#endif

	HLIB_CMD_Printf("\n========================== END ==========================\n");

	return 0;
}

extern HVD_DMX_DEMUX_INFO_STRUCT	s_stIR_DemuxInfo[HMX_VD_DMX_MAX_NUM];
void local_IrDrv_Dbg_PrintTpInfo(void)
{
	HUINT32	i, j, k, l;
	HUINT8	ucIndex = 0xFF;
	HUINT8	aucTemp[128+1];

	HLIB_CMD_Printf("\n\n==================== IRDETO TS INFO ====================\n");

	for( i = 0; i < IR_HUMAX_MAX_TP; i++ )
	{
		if( g_IrTpInfo[i].ucIsAllocated)
		{
			HLIB_CMD_Printf("[%d]hAction : 0x%X\n", i, g_IrTpInfo[i].hAction);
			HLIB_CMD_Printf("[%d]nDmxId : 0x%X\n", i, g_IrTpInfo[i].nDmxId);
			HLIB_CMD_Printf("[%d]usTsUniqueId : 0x%X\n", i, g_IrTpInfo[i].usTsUniqueId);

			for( j = 0; j < CAS_MAX_NUM_OF_SC; j++ )
			{
				HLIB_CMD_Printf("[%d]usSCELL_SvcType : %d\n", i, g_IrTpInfo[i].usSCELL_SvcType[j]);

				if( g_IrTpInfo[i].usSCELL_SvcType[j] == SERVICE_DVB_EMM )
					HLIB_CMD_Printf("\tSERVICE_DVB_EMM\n");
				else
					HLIB_CMD_Printf("\tINVALID TYPE for g_IrTpInfo(0x%X)\n", g_IrTpInfo[i].usSCELL_SvcType[j]);

				HLIB_CMD_Printf("[%d]usSCELL_SvcHandle : 0x%X - ready[%d]\n", i, g_IrTpInfo[i].usSCELL_SvcHandle[j], g_IrTpInfo[i].ucIsSCELL_Ready);
			}
			if (g_IrTpInfo[i].ucIsSCELL_Ready == 0)
				continue;
			/*
			(%03d) %04x %02x %02x %02x %02x %02x %02x
			wEmmCount “%03d”
			wEmmPid “%04x”
			bAddressControl “%02x”
			bScStatus1 “%02x”
			bScStatus2 “%02x”
			abEMMPayload[3] ”%02x %02x %02x”[Global Status, Sector Status, Sector change indicator]
			*/
			for (j = 0; j < CAS_MAX_NUM_OF_SC; j++)
			{
				HLIB_CMD_Printf("[%d]Emm monitioring : hSvc= 0x%X, wRscId = 0x%X\n",i, g_IrTpInfo[i].EmmMonitoring[j].wServiceHandle, (int)g_IrTpInfo[i].EmmMonitoring[j].wResourceId);
				snprintf(aucTemp, 128, "(%03d) %04x %02x %02x %02x %02x %02x %02x %02x",
					(HUINT16)g_IrTpInfo[i].EmmMonitoring[j].wEmmCount,\
					g_IrTpInfo[i].EmmMonitoring[j].wEmmPid,\
					g_IrTpInfo[i].EmmMonitoring[j].bAddressControl,\
					(HUINT16)g_IrTpInfo[i].EmmMonitoring[j].wEmmCount,\
					g_IrTpInfo[i].EmmMonitoring[j].bScStatus1,\
					g_IrTpInfo[i].EmmMonitoring[j].bScStatus2,\
					g_IrTpInfo[i].EmmMonitoring[j].abEmmPayload[0],\
					g_IrTpInfo[i].EmmMonitoring[j].abEmmPayload[1],\
					g_IrTpInfo[i].EmmMonitoring[j].abEmmPayload[2] );
				HLIB_CMD_Printf("\t Display Data = %s\n", aucTemp);
			}
			// For Filter information
			for(ucIndex = 0; ucIndex< HMX_VD_DMX_MAX_NUM; ucIndex++)
			{
				if(s_stIR_DemuxInfo[ucIndex].ResourceID== g_IrTpInfo[i].nDmxId)
					break;
			}

			if(ucIndex != HMX_VD_DMX_MAX_NUM)
			{
				HLIB_CMD_Printf("\t[%d]ResourceID : %d\n", i, (HINT16)s_stIR_DemuxInfo[ucIndex].ResourceID);
				for( j = 0; j < HMX_VD_DMX_MAX_CHANNEL_PER_DEMUX; j++ )
				{
					if( s_stIR_DemuxInfo[ucIndex].RefInfo[j].ReferenceID != VD_REF_ID_ERROR )
					{
						HLIB_CMD_Printf("\t[%d:RefIdx(%d)] ReferenceID : 0x%X\n", i, j, (HUINT16)s_stIR_DemuxInfo[ucIndex].RefInfo[j].ReferenceID);
						HLIB_CMD_Printf("\t[%d:RefIdx(%d)] PID : 0x%X\n", i, j, s_stIR_DemuxInfo[ucIndex].RefInfo[j].PID);
						for( k = 0; k < HMX_VD_DMX_MAX_FILTER_PER_CHANNEL; k++ )
						{
							if( s_stIR_DemuxInfo[ucIndex].RefInfo[j].IR_FilterInfo[k].Allocated == TRUE )
							{
								HLIB_CMD_Printf("\t[%d:RefIdx(%d):FIdx(%d)] RequestID : 0x%X\n", i, j, k, s_stIR_DemuxInfo[ucIndex].RefInfo[j].IR_FilterInfo[k].RequestID);
								HLIB_CMD_Printf("\t\t");
								for( l = 0; l < 16; l++ )
								{
									HLIB_CMD_Printf("%02X ", s_stIR_DemuxInfo[ucIndex].RefInfo[j].IR_FilterInfo[k].FilterValue[l]);
								}
								HLIB_CMD_Printf("\n\t\t");
								for( l = 0; l < 16; l++ )
								{
									HLIB_CMD_Printf("%02X ", s_stIR_DemuxInfo[ucIndex].RefInfo[j].IR_FilterInfo[k].FilterMask[l]);
								}
								HLIB_CMD_Printf("\n");
							}
						}
						HLIB_CMD_Printf("\n");
					}
				}
			}

		}
		else
		{
			HLIB_CMD_Printf("[%d]EMPTY\n", i);
		}

		HLIB_CMD_Printf("----------------------------------------------------------------------\n");
	}
	HLIB_CMD_Printf("\n\n========================== END ==========================\n");
}

/********************************************************************
*	function name :	 INT_IR_ParseMessage_DBG
*	input : HMSG
*	output : None
*	description : CAI와 관련된 message들( set PID/PMT update등... )을 제외한 message들의 debug
********************************************************************/
void CAS_IR_DBG_ParseMessage(HMSG* msg )
{
	HxLOG_Trace();

	switch(msg->usMsgClass)
	{
	case eIR_MSG_CLASS_CAI:
		DBG_IR_CAI_Event( (HUINT32) msg->usParam);
		break;

	case eIR_MSG_CLASS_CAS:
		switch( msg->usParam )
		{
		case IR_CATASK_FROM_SOFTCELL:
			HxLOG_Debug("[SOFTCELL->IR_CATASK]\n");
			break;

		default:
			break;
		}
		break;
	case eIR_MSG_CLASS_APP:
		switch( msg->usParam )
		{
		case CASMGR_APP_MENU_KIND:
			break;
		}
		break;
	case eIR_MSG_CLASS_PAL:
		break;
	default:
		break;
	}
}


HUINT32 local_IrDrv_Dbg_PrintSvcInfo(void)
{

	char		string[128+1];
	HUINT32	 	i, j, k;

	HLIB_CMD_Printf("\n\n==================== IRDETO SERVICE INFO ====================\n");

	for( i = 0; i < IR_HUMAX_MAX_SERVICE; i++ )
	{
		if (g_IrSvcInfo[i].ucIsAllocated)
		{
			HLIB_CMD_Printf("[%d] Handle : irSvc[0x%08X] action[0x%08X] svc[0x%08X]\n", i, g_IrSvcInfo[i].hIrSvc, g_IrSvcInfo[i].hAction, g_IrSvcInfo[i].hSvc);
			HLIB_CMD_Printf("[%d] DemuxID: %d\n", i, g_IrSvcInfo[i].ucDemuxId);
			HLIB_CMD_Printf("[%d]usSCELL_SvcType : %d\n", i, g_IrSvcInfo[i].usSCELL_SvcType);
			HLIB_CMD_Printf("[%d] Is Running with SoftCell = %d, usSCELL_SvcHandle = 0x%X\n", i, g_IrSvcInfo[i].ucIsSelected, g_IrSvcInfo[i].usSCELL_SvcHandle);
#if	defined(IRPLAT_SYS_PVR)
			if (g_IrSvcInfo[i].pstPVRSvcInfo)
			{
				HLIB_CMD_Printf("\tPVR contentId[%d]\n", g_IrSvcInfo[i].pstPVRSvcInfo->ulContentId);
				HLIB_CMD_Printf("\tPVR SoftCELL : Type[%d] Handle [0x%X]\n", g_IrSvcInfo[i].pstPVRSvcInfo->usSCELL_SvcType, g_IrSvcInfo[i].pstPVRSvcInfo->usSCELL_SvcHandle);
			}
#endif

			HxSTD_memset(string, 0, sizeof(string));
			snprintf((char*)string, 128, (char *)"%c%03d-%d", g_IrSvcInfo[i].GlobalStatus.CharSeverity, g_IrSvcInfo[i].GlobalStatus.stStatus.wStatus, g_IrSvcInfo[i].GlobalStatus.stStatus.eSource);
			HLIB_CMD_Printf("[%d] Global Status : %s\n", i, string);
#if	defined(IRPLAT_SYS_PVR)
			if (g_IrSvcInfo[i].pstPVRSvcInfo)
			{
				snprintf((char*)string, 128, (char *)"%c%03d-%d", g_IrSvcInfo[i].pstPVRSvcInfo->GlobalStatus.CharSeverity, g_IrSvcInfo[i].pstPVRSvcInfo->GlobalStatus.stStatus.wStatus, g_IrSvcInfo[i].pstPVRSvcInfo->GlobalStatus.stStatus.eSource);
				HLIB_CMD_Printf("\tPVR Global Status : %s\n", string);
			}
#endif

			HxSTD_memset(string, 0, sizeof(string));
			snprintf((char*)string, 128, (char *)"%c%03d-%d", g_IrSvcInfo[i].InternalStatus.CharSeverity, g_IrSvcInfo[i].InternalStatus.stStatus.wStatus, g_IrSvcInfo[i].InternalStatus.stStatus.eSource);
			HLIB_CMD_Printf("[%d] Internal Status : %s\n", i, string);
#if	defined(IRPLAT_SYS_PVR)
			if (g_IrSvcInfo[i].pstPVRSvcInfo)
			{
				snprintf((char*)string, 128, (char *)"%c%03d-%d", g_IrSvcInfo[i].pstPVRSvcInfo->InternalStatus.CharSeverity, g_IrSvcInfo[i].pstPVRSvcInfo->InternalStatus.stStatus.wStatus, g_IrSvcInfo[i].pstPVRSvcInfo->InternalStatus.stStatus.eSource);
				HLIB_CMD_Printf("\tPVR Internal Status : %s\n", string);
			}
#endif

			HxSTD_memset(string, 0, sizeof(string));
			snprintf((char*)string, 128, (char *)"%c%03d-%d", g_IrSvcInfo[i].EcmRscStatus.CharSeverity, g_IrSvcInfo[i].EcmRscStatus.stStatus.wStatus, g_IrSvcInfo[i].EcmRscStatus.stStatus.eSource);
			HLIB_CMD_Printf("[%d] Ecm Rsc Status : %s\n", i, string);

			HxSTD_memset(string, 0, sizeof(string));
			snprintf((char*)string, 128, (char *)"%c%03d-%d", g_IrSvcInfo[i].DvbDescramblerRscStatus.CharSeverity, g_IrSvcInfo[i].DvbDescramblerRscStatus.stStatus.wStatus, g_IrSvcInfo[i].DvbDescramblerRscStatus.stStatus.eSource);
			HLIB_CMD_Printf("[%d] Descrambler Rsc Status : %s\n", i, string);

			HxSTD_memset(string, 0, sizeof(string));
			snprintf((char*)string, 128, (char *)"%c%03d-%d", g_IrSvcInfo[i].SmartcardRscStatus.CharSeverity, g_IrSvcInfo[i].SmartcardRscStatus.stStatus.wStatus, g_IrSvcInfo[i].SmartcardRscStatus.stStatus.eSource);
			HLIB_CMD_Printf("[%d] Smartcard Rsc Status : %s\n", i, string);

#if defined(CONFIG_MW_CAS_IRDETO_MATURITY_RATING)
			HxSTD_memset(string, 0, sizeof(string));
			snprintf((char*)string, 128, (char *)"%c%03d-%d", g_IrSvcInfo[i].MaturityRatingRscStatus.CharSeverity, g_IrSvcInfo[i].MaturityRatingRscStatus.stStatus.wStatus, g_IrSvcInfo[i].MaturityRatingRscStatus.stStatus.eSource);
			HLIB_CMD_Printf("[%d] Maturity rating Rsc Status : %s\n", i, string);
			HLIB_CMD_Printf("[%d] Maturity rating Rsc ID : %d\n", g_IrSvcInfo[i].MaturityRatingRscStatus.usResourceID);
#endif

			HLIB_CMD_Printf("[%d] eSvcCasKind : %d", i, g_IrSvcInfo[i].eSvcCasKind);
			HLIB_CMD_Printf("\t\t[0:NONE, 1:FTA, 2:IRDETO, 3:OTHER]\n");

			HLIB_CMD_Printf("[%d] PID LIST - %d\n", i, g_IrSvcInfo[i].ucStreamCount);
			for( j = 0; j < g_IrSvcInfo[i].ucStreamCount; j++ )
			{
				HLIB_CMD_Printf("\t\t[PID : 0x%04X - ", g_IrSvcInfo[i].astPidList[j].usPID);
				for (k = 0; k < CAS_MAX_NUM_OF_SC; k++)
				{
					switch(g_IrSvcInfo[i].astPidList[j].ucTypeFromPMT)
					{
						case eEsType_Video:
							HxSTD_memset(string, 0, sizeof(string));
							snprintf((char*)string, 128, (char *)"%c%03d-%d", g_IrSvcInfo[i].astPidList[j].ElemRsc[k].CharSeverity, g_IrSvcInfo[i].astPidList[j].ElemRsc[k].stStatus.wStatus, g_IrSvcInfo[i].astPidList[j].ElemRsc[k].stStatus.eSource);
							HLIB_CMD_Printf("(%s) VIDEO(%d), ", string, g_IrSvcInfo[i].astPidList[j].ucTypeFromPMT);
							break;

						case eEsType_Audio:
							snprintf((char*)string, 128, (char *)"%c%03d-%d", g_IrSvcInfo[i].astPidList[j].ElemRsc[k].CharSeverity, g_IrSvcInfo[i].astPidList[j].ElemRsc[k].stStatus.wStatus, g_IrSvcInfo[i].astPidList[j].ElemRsc[k].stStatus.eSource);
							HLIB_CMD_Printf("(%s) AUDIO(%d), ", string, g_IrSvcInfo[i].astPidList[j].ucTypeFromPMT);
							break;

						case eEsType_Subtitle:
						case eEsType_Teletext:
						case eEsType_Data:
							snprintf((char*)string, 128, (char *)"%c%03d-%d", g_IrSvcInfo[i].astPidList[j].ElemRsc[k].CharSeverity, g_IrSvcInfo[i].astPidList[j].ElemRsc[k].stStatus.wStatus, g_IrSvcInfo[i].astPidList[j].ElemRsc[k].stStatus.eSource);
							HLIB_CMD_Printf("(%s) DATA(%d), ", string, g_IrSvcInfo[i].astPidList[j].ucTypeFromPMT);
							break;

						default:
							snprintf((char*)string, 128, (char *)"%c%03d-%d", g_IrSvcInfo[i].astPidList[j].ElemRsc[k].CharSeverity, g_IrSvcInfo[i].astPidList[j].ElemRsc[k].stStatus.wStatus, g_IrSvcInfo[i].astPidList[j].ElemRsc[k].stStatus.eSource);
							HLIB_CMD_Printf("(%s) UNKNOWN(%d), ", string, g_IrSvcInfo[i].astPidList[j].ucTypeFromPMT);
							break;
					}
				}
				if( g_IrSvcInfo[i].astPidList[j].IsLive )
					HLIB_CMD_Printf("LIVE(Y) ");
				else
					HLIB_CMD_Printf("LIVE(N) ");

				if( g_IrSvcInfo[i].astPidList[j].IsRecord )
					HLIB_CMD_Printf("RECORD(Y) ");
				else
					HLIB_CMD_Printf("RECORD(N) ");

				HLIB_CMD_Printf("\n");
			}

			if(g_IrSvcInfo[i].MacrovisionRscStatus.Valid == TRUE)
			{
				HxSTD_memset(string, 0, sizeof(string));
				snprintf((char*)string, 128, (char *)"%c%03d-%d",\
					g_IrSvcInfo[i].MacrovisionRscStatus.CharSeverity, g_IrSvcInfo[i].MacrovisionRscStatus.stStatus.wStatus, \
					g_IrSvcInfo[i].MacrovisionRscStatus.stStatus.eSource);
				HLIB_CMD_Printf("[%d] Macrovision Rsc Status : %s\n", i, string);
			}

			for (k = 0; k < CAS_MAX_NUM_OF_SC; k++)
			{
				for(j =0; j < MAX_NUM_ECM_MONITOR_PID; j++)
				{
					HLIB_CMD_Printf("[%d] ECM monitor Rsc Status :\n",i );
					if ((g_IrSvcInfo[i].EcmMonitorInfo[k].ausEcmMonitorPID[j] != IRDRV_NULL_PID) && (g_IrSvcInfo[i].EcmMonitorInfo[k].ausEcmMonitorPID[j]))
					{
						HxSTD_memset(string, 0, sizeof(string));

						HLIB_CMD_Printf("\t [%d] hSvc= 0x%X, ecm pid= 0x%X,\n", j, g_IrSvcInfo[i].EcmMonitorInfo[k].astEcmMonitoring[j].wServiceHandle, 	g_IrSvcInfo[i].EcmMonitorInfo[k].astEcmMonitoring[j].wEcmPid);

						snprintf((char*)string, 128, (char *)"DisplayData= (%03d) %04x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x",
							(HUINT16)g_IrSvcInfo[i].EcmMonitorInfo[k].astEcmMonitoring[j].wEcmCount,
							g_IrSvcInfo[i].EcmMonitorInfo[k].astEcmMonitoring[j].wEcmPid,
							g_IrSvcInfo[i].EcmMonitorInfo[k].astEcmMonitoring[j].bPage,
							g_IrSvcInfo[i].EcmMonitorInfo[k].astEcmMonitoring[j].bVersion,
							g_IrSvcInfo[i].EcmMonitorInfo[k].astEcmMonitoring[j].bScStatus1,
							g_IrSvcInfo[i].EcmMonitorInfo[k].astEcmMonitoring[j].bScStatus2,
							g_IrSvcInfo[i].EcmMonitorInfo[k].astEcmMonitoring[j].abScReply[0],
							g_IrSvcInfo[i].EcmMonitorInfo[k].astEcmMonitoring[j].abScReply[1],
							g_IrSvcInfo[i].EcmMonitorInfo[k].astEcmMonitoring[j].abScReply[2],
							g_IrSvcInfo[i].EcmMonitorInfo[k].astEcmMonitoring[j].abScReply[3],
							g_IrSvcInfo[i].EcmMonitorInfo[k].astEcmMonitoring[j].abScReply[4],
							g_IrSvcInfo[i].EcmMonitorInfo[k].astEcmMonitoring[j].abScReply[5] );
						HLIB_CMD_Printf("\t %s\n", string );
					}
					else
					{
						HLIB_CMD_Printf("\t [%d] ]PID NULL\n", j);
						break;
					}
				}
			}
		}
		else
		{
			HLIB_CMD_Printf("[%d]EMPTY\n", i);
		}
		HLIB_CMD_Printf("----------------------------------------------------------------------\n");
	}
	HLIB_CMD_Printf("\n\n========================== END ==========================\n");

	HVD_DBGAPI_DSC_INFO();

	return 0;
}

void CAS_IR_Dbg_ConvertResourceToString(HUINT32 source)
{
	switch(source)
	{
	case RESOURCE_ECM_SOURCE: HLIB_CMD_Printf("RESOURCE_ECM_SOURCE\n"); break;
	case RESOURCE_EMM_SOURCE: HLIB_CMD_Printf("RESOURCE_EMM_SOURCE\n"); break;
	case RESOURCE_DVB_DESCRAMBLER: HLIB_CMD_Printf("RESOURCE_DVB_DESCRAMBLER\n"); break;
	case RESOURCE_SMARTCARD: HLIB_CMD_Printf("RESOURCE_SMARTCARD\n"); break;
	case RESOURCE_IPTV_STB_SOCKET: HLIB_CMD_Printf("RESOURCE_IPTV_STB_SOCKET\n"); break;
	case RESOURCE_IPTV_HEADEND_SOCKET: HLIB_CMD_Printf("RESOURCE_IPTV_HEADEND_SOCKET\n"); break;
	case RESOURCE_IPTV_MROUTER_ADDRESS: HLIB_CMD_Printf("RESOURCE_IPTV_MROUTER_ADDRESS\n"); break;
	case RESOURCE_CC_SOURCE: HLIB_CMD_Printf("RESOURCE_CC_SOURCE\n"); break;
	case RESOURCE_MONITORING: HLIB_CMD_Printf("RESOURCE_MONITORING\n"); break;
	case RESOURCE_CC_DECRYPTOR: HLIB_CMD_Printf("RESOURCE_CC_DECRYPTOR\n"); break;
	case RESOURCE_REGISTRATION_HANDLE: HLIB_CMD_Printf("RESOURCE_REGISTRATION_HANDLE\n"); break;
	case RESOURCE_SESSION_MANAGEMENT: HLIB_CMD_Printf("RESOURCE_SESSION_MANAGEMENT\n"); break;
	case RESOURCE_SESSION_MANAGEMENT_DRM: HLIB_CMD_Printf("RESOURCE_SESSION_MANAGEMENT_DRM\n"); break;
	case RESOURCE_CRYPTO_OPERATION: HLIB_CMD_Printf("RESOURCE_CRYPTO_OPERATION\n"); break;
	case RESOURCE_MACROVISION: HLIB_CMD_Printf("RESOURCE_MACROVISION\n"); break;
	case RESOURCE_STB_MAC_ADDRESS: HLIB_CMD_Printf("RESOURCE_STB_MAC_ADDRESS\n"); break;
	case RESOURCE_DVB_OOB: HLIB_CMD_Printf("RESOURCE_DVB_OOB\n"); break;
	case RESOURCE_GLOBAL_SOURCE: HLIB_CMD_Printf("RESOURCE_GLOBAL_SOURCE\n"); break;
	case RESOURCE_INTERNAL_SOURCE: HLIB_CMD_Printf("RESOURCE_INTERNAL_SOURCE\n"); break;
	case RESOURCE_ELEM_STREAM_SOURCE: HLIB_CMD_Printf("RESOURCE_ELEM_STREAM_SOURCE\n"); break;
	case RESOURCE_MATURITY_RATING: HLIB_CMD_Printf("RESOURCE_MATURITY_RATING\n"); break;

// only resource types shall be descripted here. 	case RESOURCE_ERROR: HLIB_CMD_Printf("RESOURCE_ERROR\n"); break;
	default: HLIB_CMD_Printf("UNKNOWN RESOURCE(source=%d)\n", source);break;

	}

}


HUINT32 local_Ir_Drv_Dbg_PrintVersionInfo(void)
{
	HUINT32	i;
	HUINT16	usClientLength;
	HUINT8	*pClientString;
	HUINT16	usCount;
	HUINT16	*pusComponentLength = NULL;
	HUINT8	**pComponentsString = NULL;

	HLIB_CMD_Printf("\n\n==================== CLIENT VERSION INFO ====================\n");

	xsvc_cas_IrGetClientVersion(&usClientLength, &pClientString);
	HLIB_CMD_Printf("[usClientLength = %d, @pClientString = 0x%X\n", usClientLength, (HINT32 )pClientString);
	if( usClientLength )
	{
		HLIB_CMD_Printf("%s\n", pClientString);
		OxCAS_Free(pClientString);
	}

	HLIB_CMD_Printf("\n==================== COMPONENTS VERSION INFO ====================\n");

	xsvc_cas_IrGetComponentsVersion(&usCount, &pusComponentLength, &pComponentsString);
	if( usCount !=0 )
	{
		for( i = 0; i < usCount; i++ )
		{
			if( pusComponentLength[i] )
			{
				HLIB_CMD_Printf("%s\n", pComponentsString[i]);
				OxCAS_Free(pComponentsString[i]);
			}
		}

		if( pComponentsString )
		{
			OxCAS_Free(pComponentsString);
		}

		if( pusComponentLength )
		{
			OxCAS_Free(pusComponentLength);
		}
	}
	else	// prevent fix (added)
	{
		if (pComponentsString)
			OxCAS_Free(pComponentsString);	// count가 0인 경우, null일테지만...

		if (pusComponentLength)
			OxCAS_Free(pusComponentLength);	// count가 0인 경우, null일테지만...
	}
	// the end of fix

	HLIB_CMD_Printf("\n========================== END ==========================\n");

	return 0;
}

HUINT32 local_Ir_Drv_Dbg_PrintLiveIaStatus(void)
{
	HUINT32	i, j, k, m;
#if defined(IRPLAT_FN_COPY_CONTROL)
	HUINT32	l;
#endif

	HUINT8	ucNumOfService = 0;
	IR_SVC_STATUS_INFO_MAIN	*pstSvcStatusInfo = NULL;

	HUINT8	ucNumOfElem = 0;
	IR_ELEM_STATE	*pstSvcStatusInfoElem = NULL;

	HLIB_CMD_Printf("---------------------------------SERVICE STATUS---------------------------------\n");
	HLIB_CMD_Printf("\n\n\tDVB Client Services\n\n\n");
	HLIB_CMD_Printf("\tHandle\tName\tSource TS\tInstance\tDescription\n");
	ucNumOfService = CAS_IR_DRV_GetNumberOfServices(&pstSvcStatusInfo);
	if( ucNumOfService )
	{
		if( pstSvcStatusInfo != NULL )
		{
			for( i = 0; i < ucNumOfService; i++ )
			{
				HLIB_CMD_Printf("\t0x%03X\t", pstSvcStatusInfo[i].usSCellSvcHandle);
				switch(pstSvcStatusInfo[i].eSvcType)
				{
					case SERVICE_DVB_DESCRAMBLE:  	HLIB_CMD_Printf("View\t"); break;
					case SERVICE_DVB_EMM: HLIB_CMD_Printf("EMM\t"); break;
					default: HLIB_CMD_Printf("???\t"); break;
				}

				switch(pstSvcStatusInfo[i].eSvcType)
				{
					case SERVICE_DVB_DESCRAMBLE:  	HLIB_CMD_Printf("\tDVB Descramble Service\n"); break;
					case SERVICE_DVB_EMM: HLIB_CMD_Printf("\tDVB EMM Service\n"); break;
					default: HLIB_CMD_Printf("\t???\n"); break;
				}
			}
			HLIB_CMD_Printf("--------------------------------------------------------------------------------\n\n");

			for( i = 0; i < ucNumOfService; i++ )
			{
				IR_SVC_STATUS_INFO_EACH stSvcStatusInfoEach = {0, };	//prevent fix

				HLIB_CMD_Printf("---------------------------------HANDLE : 0x%03X---------------------------------\n", pstSvcStatusInfo[i].usSCellSvcHandle);

				if( CAS_IR_DRV_GetInformationOfEachService(pstSvcStatusInfo[i].eSvcType, pstSvcStatusInfo[i].usSCellSvcHandle, &stSvcStatusInfoEach) == HIR_OK )
				{
					switch(pstSvcStatusInfo[i].eSvcType)
					{
						case SERVICE_DVB_DESCRAMBLE:  	HLIB_CMD_Printf("DVB Descramble Service\n\n"); break;
						case SERVICE_DVB_EMM: HLIB_CMD_Printf("DVB EMM Service\n\n"); break;
						default: HLIB_CMD_Printf("what ???\n\n"); break;
					}

					HLIB_CMD_Printf("Service Handle : \t\t0x%03X\n", pstSvcStatusInfo[i].usSCellSvcHandle);
					HLIB_CMD_Printf("Global Service Status : \t%c%03d-%d \n",
						stSvcStatusInfoEach.GlobalStatus.CharSeverity, stSvcStatusInfoEach.GlobalStatus.usStatus, stSvcStatusInfoEach.GlobalStatus.eSource);
					HLIB_CMD_Printf("Internal Status : \t\t\t\t%c%03d-%d\n",
						stSvcStatusInfoEach.InternalStatus.CharSeverity, stSvcStatusInfoEach.InternalStatus.usStatus, stSvcStatusInfoEach.InternalStatus.eSource);

					if( pstSvcStatusInfo[i].eSvcType == SERVICE_DVB_DESCRAMBLE )
					{
						HLIB_CMD_Printf("ECM Source : \t\t\t0x%04X\t\t%c%03d-%d\n",
							stSvcStatusInfoEach.EcmOrEmmRscStatus.usRscID,
							stSvcStatusInfoEach.EcmOrEmmRscStatus.CharSeverity, stSvcStatusInfoEach.EcmOrEmmRscStatus.usStatus, stSvcStatusInfoEach.EcmOrEmmRscStatus.eSource);

						for (m = 0; m < CAS_MAX_NUM_OF_SC; m++)
						{
							ucNumOfElem = CAS_IR_DRV_GetInformationOfElementary(pstSvcStatusInfo[i].usSCellSvcHandle, m, &pstSvcStatusInfoElem);
							if( ucNumOfElem )
							{
								for( j = 0; j < ucNumOfElem; j++ )
								{
									HLIB_CMD_Printf("\tES:0x%04X\t\tECM:0x%04X\t%c%03d-%d\n", pstSvcStatusInfoElem[j].usEsPID, pstSvcStatusInfoElem[j].usEcmOrEmmPID,
										pstSvcStatusInfoElem[j].CharSeverity, pstSvcStatusInfoElem[j].usStatus, pstSvcStatusInfoElem[j].eSource);
								}
							}
							if (pstSvcStatusInfoElem)
							{
								OxCAS_Free(pstSvcStatusInfoElem);
								pstSvcStatusInfoElem = NULL;
							}
						}
						HLIB_CMD_Printf("DVB Descramble : \t\t0x%04X\t\t%c%03d-%d\n",
							stSvcStatusInfoEach.DvbDescramblerRscStatus.usRscID,
							stSvcStatusInfoEach.DvbDescramblerRscStatus.CharSeverity, stSvcStatusInfoEach.DvbDescramblerRscStatus.usStatus, stSvcStatusInfoEach.DvbDescramblerRscStatus.eSource);
						HLIB_CMD_Printf("Smart Card : \t\t\t0x%04X\t\t%c%03d-%d\n",
							stSvcStatusInfoEach.SmartcardRscStatus.usRscID,
							stSvcStatusInfoEach.SmartcardRscStatus.CharSeverity, stSvcStatusInfoEach.SmartcardRscStatus.usStatus, stSvcStatusInfoEach.SmartcardRscStatus.eSource);
#if defined(IRPLAT_FN_COPY_CONTROL) /* name is changed SUPPORT_MACROVISION */
						HLIB_CMD_Printf("Macrovision : \t\t\t0x%04X\t\t%c%03d-%d\n",
							stSvcStatusInfoEach.MacrovisionRscStatus.usRscID,
							stSvcStatusInfoEach.MacrovisionRscStatus.CharSeverity, stSvcStatusInfoEach.MacrovisionRscStatus.usStatus, stSvcStatusInfoEach.MacrovisionRscStatus.eSource);

						HLIB_CMD_Printf("Monitoring : \n");
						for (m = 0; m < CAS_MAX_NUM_OF_SC; m++)
						{
							for (l = 0; l < MAX_NUM_ECM_MONITOR_PID; l++)
							{
								if (stSvcStatusInfoEach.EcmPID[m][l] != IRDRV_NULL_PID)
								{
									HLIB_CMD_Printf("ECM : %04X %02X %02X %02X %02X ", stSvcStatusInfoEach.EcmMonitoring[m][l].wEcmPid, stSvcStatusInfoEach.EcmMonitoring[m][l].bPage,
										stSvcStatusInfoEach.EcmMonitoring[m][l].bVersion, stSvcStatusInfoEach.EcmMonitoring[m][l].bScStatus1, stSvcStatusInfoEach.EcmMonitoring[m][l].bScStatus2);
									for( k = 0; k < 6; k++ )
									{
										HLIB_CMD_Printf("%02X ", stSvcStatusInfoEach.EcmMonitoring[m][l].abScReply[k]);
									}
								}
							}
						}
#endif

						HLIB_CMD_Printf("\n");

					}
					else if( pstSvcStatusInfo[i].eSvcType == SERVICE_DVB_EMM )
					{
						HLIB_CMD_Printf("Smart Card : \t\t\t0x%04X\t\t%c%03d-%d\n",
							stSvcStatusInfoEach.SmartcardRscStatus.usRscID,
							stSvcStatusInfoEach.SmartcardRscStatus.CharSeverity, stSvcStatusInfoEach.SmartcardRscStatus.usStatus, stSvcStatusInfoEach.SmartcardRscStatus.eSource);
						HLIB_CMD_Printf("EMM Source : \t\t\t0x%04X\t\t%c%03d-%d\n",
							stSvcStatusInfoEach.EcmOrEmmRscStatus.usRscID,
							stSvcStatusInfoEach.EcmOrEmmRscStatus.CharSeverity, stSvcStatusInfoEach.EcmOrEmmRscStatus.usStatus, stSvcStatusInfoEach.EcmOrEmmRscStatus.eSource);
						HLIB_CMD_Printf("\t\tEMM PID : 0x%04X\n", stSvcStatusInfoEach.EcmOrEmmRscStatus.usEmmPID);

						HLIB_CMD_Printf("Monitoring : \n");

						HLIB_CMD_Printf("EMM : %02X %02X %02X ", stSvcStatusInfoEach.EmmMonitoring.bAddressControl,
							stSvcStatusInfoEach.EmmMonitoring.bScStatus1, stSvcStatusInfoEach.EmmMonitoring.bScStatus2);

						for( k = 0; k < 3; k++ )
						{
							HLIB_CMD_Printf("%02X ", stSvcStatusInfoEach.EmmMonitoring.abEmmPayload[k]);
						}
						HLIB_CMD_Printf("\n");
					}
				}
				else
				{
					HLIB_CMD_Printf("\tGet Info Fail!!\n");
				}

				HLIB_CMD_Printf("--------------------------------------------------------------------------------\n");
			}
			OxCAS_Free(pstSvcStatusInfo);
		}
	}
	else
	{
		HLIB_CMD_Printf("\n\n No registered service\n");
	}

	return 0;
}



void CAS_IR_DBG_STATUS(HUINT32 eSvcType, ia_word16 wServiceHandle, HUINT32 Source, ia_status_st status)
{
	char CharSeverity;

	switch(status.eSeverity)
	{
	case STATUS_FATAL:		CharSeverity = 'F';			break;
	case STATUS_ERROR:		CharSeverity = 'E';			break;
	case STATUS_WARNING:	CharSeverity = 'W';			break;
	case STATUS_INFO:		CharSeverity = 'I';			break;
	case STATUS_INFO_DND:	CharSeverity = 'D';			break;
	default:				CharSeverity = 'U';			break;
	}

	switch(eSvcType)
	{
	case SERVICE_DVB_DESCRAMBLE: HxLOG_Debug("\tSERVICE_DVB_DESCRMABLE(%d) ", wServiceHandle); break;
	case SERVICE_DVB_EMM: HxLOG_Debug("\tSERVICE_DVB_EMM(%d) ", wServiceHandle); break;
	case SERVICE_PVR_RECORD: HxLOG_Debug("\tSERVICE_PVR_RECORD(%d) ", wServiceHandle); break;
	case SERVICE_PVR_PLAYBACK: HxLOG_Debug("\tSERVICE_PVR_PLAYBACK(%d) ", wServiceHandle); break;
	case SERVICE_PVR_DRM: HxLOG_Debug("\tSERVICE_PVR_DRM(%d) ", wServiceHandle); break;

	default : HxLOG_Debug("\tUNKNOWN(svc type = 0x%X)", eSvcType); break;
	}

	switch(Source)
	{
	case RESOURCE_ECM_SOURCE: HxLOG_Debug("RESOURCE_ECM_SOURCE"); break;
	case RESOURCE_EMM_SOURCE: HxLOG_Debug("RESOURCE_EMM_SOURCE"); break;
	case RESOURCE_DVB_DESCRAMBLER: HxLOG_Debug("RESOURCE_DVB_DESCRAMBLER"); break;
	case RESOURCE_SMARTCARD: HxLOG_Debug("RESOURCE_SMARTCARD"); break;
	case RESOURCE_IPTV_STB_SOCKET: HxLOG_Debug("RESOURCE_IPTV_STB_SOCKET"); break;
	case RESOURCE_IPTV_HEADEND_SOCKET: HxLOG_Debug("RESOURCE_IPTV_HEADEND_SOCKET"); break;
	case RESOURCE_IPTV_MROUTER_ADDRESS: HxLOG_Debug("RESOURCE_IPTV_MROUTER_ADDRESS"); break;
	case RESOURCE_CC_SOURCE: HxLOG_Debug("RESOURCE_CC_SOURCE"); break;
	case RESOURCE_MONITORING: HxLOG_Debug("RESOURCE_MONITORING"); break;
	case RESOURCE_CC_DECRYPTOR: HxLOG_Debug("RESOURCE_CC_DECRYPTOR"); break;
	case RESOURCE_REGISTRATION_HANDLE: HxLOG_Debug("RESOURCE_REGISTRATION_HANDLE"); break;
	case RESOURCE_SESSION_MANAGEMENT: HxLOG_Debug("RESOURCE_SESSION_MANAGEMENT"); break;
	case RESOURCE_SESSION_MANAGEMENT_DRM: HxLOG_Debug("RESOURCE_SESSION_MANAGEMENT_DRM"); break;
	case RESOURCE_CRYPTO_OPERATION: HxLOG_Debug("RESOURCE_CRYPTO_OPERATION"); break;
	case RESOURCE_MACROVISION: HxLOG_Debug("RESOURCE_MACROVISION"); break;
	case RESOURCE_STB_MAC_ADDRESS: HxLOG_Debug("RESOURCE_STB_MAC_ADDRESS"); break;
	case RESOURCE_DVB_OOB: HxLOG_Debug("RESOURCE_DVB_OOB"); break;
	case RESOURCE_GLOBAL_SOURCE: HxLOG_Debug("RESOURCE_GLOBAL_SOURCE"); break;
	case RESOURCE_INTERNAL_SOURCE: HxLOG_Debug("RESOURCE_INTERNAL_SOURCE"); break;
	case RESOURCE_ELEM_STREAM_SOURCE: HxLOG_Debug("RESOURCE_ELEM_STREAM_SOURCE"); break;
	case RESOURCE_ERROR: HxLOG_Debug("RESOURCE_ERROR"); break;
	case RESOURCE_MATURITY_RATING: HxLOG_Debug("RESOURCE_MATURITY_RATING"); break;
	}

	HxLOG_Debug(" : %c%03d-%d(", CharSeverity, status.wStatus, status.eSource);

	switch(status.eSource)
	{
	case SOURCE_UNKNOWN:					/* Use only for initialisation and error-checking. */
		HxLOG_Debug("SOURCE_UNKNOWN");
		break;
	case SOURCE_SOFTCELL:						/* Core components. */
		HxLOG_Debug("SOURCE_SOFTCELL");
		break;
	case SOURCE_SERVICES:						/* Core service and modules components. */
		HxLOG_Debug("SOURCE_SERVICES");
		break;
	case SOURCE_SMARTCARD:						/* One of the components of the smartcard security device.*/
		HxLOG_Debug("SOURCE_SMARTCARD");
		break;
	case SOURCE_DVB_EMM_SERVICE:				/* Status of the DVB EMM service. */
		HxLOG_Debug("SOURCE_DVB_EMM_SERVICE");
		break;
	case SOURCE_MOD_EMM_SOURCE:				/* Status of the EMM source (CAT) module. */
		HxLOG_Debug("SOURCE_MOD_EMM_SOURCE");
		break;
	case SOURCE_MOD_DVB_EMM_FILTER:			/* Status of the DVB EMM filter module. */
		HxLOG_Debug("SOURCE_MOD_DVB_EMM_FILTER");
		break;
	case SOURCE_DVB_DESCRAMBLE_SERVICE:	/* Status of the DVB DESCRAMBLE service. */
		HxLOG_Debug("SOURCE_DVB_DESCRAMBLE_SERVICE");
		break;
	case SOURCE_MOD_ECM_SOURCE:				/* Status of the ECM source (PMT) module. */
		HxLOG_Debug("SOURCE_MOD_ECM_SOURCE");
		break;
	case SOURCE_MOD_DVB_DESCRAMBLER:		/* Status of the DVB Descrambler module. */
		HxLOG_Debug("SOURCE_MOD_DVB_DESCRAMBLER");
		break;
	case SOURCE_MOD_MONITORING:				/* Status of monitoring module. */
		HxLOG_Debug("SOURCE_MOD_MONITORING");
		break;
	case SOURCE_IPPV:							/* IPPV smartcard option. */
		HxLOG_Debug("SOURCE_IPPV");
		break;
	case SOURCE_SMARTCARD_MARRIAGE:			/* Status of smartcard marriage option. */
		HxLOG_Debug("SOURCE_SMARTCARD_MARRIAGE");
		break;
	case SOURCE_IPTV_REGISTRATION_EMM:		/* Status of the IPTV Registration and EMM service. */
		HxLOG_Debug("SOURCE_IPTV_REGISTRATION_EMM");
		break;
	case SOURCE_MOD_IPTV_STB_SOCKET:		/* Status of the IPTV STB Socket module. */
		HxLOG_Debug("SOURCE_MOD_IPTV_STB_SOCKET");
		break;
	case SOURCE_MOD_IPTV_HEADEND_SOCKET:	/* Status of the IPTV HeadEnd Socket module. */
		HxLOG_Debug("SOURCE_MOD_IPTV_HEADEND_SOCKET");
		break;
	case SOURCE_MOD_IPTV_MROUTER_ADDRESS:	/* Status of the IPTV MRouter Address module. */
		HxLOG_Debug("SOURCE_MOD_IPTV_MROUTER_ADDRESS");
		break;
	case SOURCE_MOD_CC_EMM:					/* Status of cc emm filter module in operator service. */
		HxLOG_Debug("SOURCE_MOD_CC_EMM");
		break;
	case SOURCE_MOD_CC_SOURCE:				/* Status of cc source in cc services. */
		HxLOG_Debug("SOURCE_MOD_CC_SOURCE");
		break;
	case SOURCE_CC_OPERATOR_SERVICE:		/* Status of cc operator service. */
		HxLOG_Debug("SOURCE_CC_OPERATOR_SERVICE");
		break;
	case SOURCE_MOD_CC_ECM:					/* Status of cc ecm parser module in operator service. */
		HxLOG_Debug("SOURCE_MOD_CC_ECM");
		break;
	case SOURCE_CC_DESCRAMBLER_SERVICE:	/* Status of cc descrambler service. */
		HxLOG_Debug("SOURCE_CC_DESCRAMBLER_SERVICE");
		break;
	case SOURCE_MOD_CC_DESCRAM:				/* Status of cc descrambler module. */
		HxLOG_Debug("SOURCE_MOD_CC_DESCRAM");
		break;
	case SOURCE_MOD_CC_DECRYPTOR:			/* Status of cc decryptor source module. */
		HxLOG_Debug("SOURCE_MOD_CC_DECRYPTOR");
		break;
	case SOURCE_MOD_REGISTRATION_HANDLE:	/* Status of the IPTV Registration service handle. */
		HxLOG_Debug("SOURCE_MOD_REGISTRATION_HANDLE");
		break;
	case SOURCE_IPTV_DESCRAMBLE_VOD_SERVICE:	/* Status of the IPTV Descramble VOD service. */
		HxLOG_Debug("SOURCE_IPTV_DESCRAMBLE_VOD_SERVICE");
		break;
	case SOURCE_PVR_RECORD_SERVICE:			/* Status of PVR Record service. */
		HxLOG_Debug("SOURCE_PVR_RECORD_SERVICE");
		break;
	case SOURCE_PVR_PLAYBACK_SERVICE:		/* Status of PVR Playback service. */
		HxLOG_Debug("SOURCE_PVR_PLAYBACK_SERVICE");
		break;
	case SOURCE_MOD_SESSION_MANAGEMENT:	/* Status of Session Management resource module. */
		HxLOG_Debug("SOURCE_MOD_SESSION_MANAGEMENT");
		break;
	case SOURCE_MOD_SESSION_MANAGEMENT_DRM:
		HxLOG_Debug("SOURCE_MOD_SESSION_MANAGEMENT_DRM");
		break;
	case SOURCE_MOD_CRYPTO_OPERATION:		/* Status of Crypto Operation resource module. */
		HxLOG_Debug("SOURCE_MOD_CRYPTO_OPERATION");
		break;
	case SOURCE_MOD_MACROVISION:				/* Status of Macrovision resource module. */
		HxLOG_Debug("SOURCE_MOD_MACROVISION");
		break;
	case SOURCE_APPLICATION:					/* To be used by the application to report non-SoftCell status. */
		HxLOG_Debug("SOURCE_APPLICATION");
		break;
	case SOURCE_MOD_STB_MAC_ADDRESS:		/* Status of STB MAC address module. */
		HxLOG_Debug("SOURCE_MOD_STB_MAC_ADDRESS");
		break;
	case SOURCE_MOD_DVB_OOB:					/* Status of the DVB OOB resource module. */
		HxLOG_Debug("SOURCE_MOD_DVB_OOB");
		break;
	case SOURCE_CUSTOM_CA:                /* status of CustomCA module */
		HxLOG_Debug("SOURCE_CUSTOM_CA");
		break;
	case SOURCE_MOD_MATURITY_RATING:
		HxLOG_Debug("SOURCE_MOD_MATURITY_RATING");
		break;
	case SOURCE_PVR_DRM_SERVICE:
		HxLOG_Debug("SOURCE_PVR_DRM_SERVICE");
		break;
	default:
		HxLOG_Debug("CAS_IR_DBG_STATUS: UNKNOWN SOURCE: status.eSource= %d\n", status.eSource);
		break;
	}

	HxLOG_Debug(")\n");
}



#if defined(CONFIG_MW_CAS_IRDETO_MATURITY_RATING)
void	INT_IR_ParseMRMessage_DBG(msg_mr_user_profile_reply_st *pst_mr_user_profile_reply)
{
	msg_mr_user_profile_reply_st reply;

	HxSTD_memcpy(&reply, pst_mr_user_profile_reply, sizeof(msg_mr_user_profile_reply_st) );

	switch(reply.eResult)
	{
		case MR_USER_PROFILE_OK:
			HLIB_CMD_Printf("MSG_MATURITY_RATING_USER_PROFILE_REPLY: MR_USER_PROFILE_OK\n");
			break;
		case MR_USER_PROFILE_CARD_ERROR:
			HLIB_CMD_Printf("MSG_MATURITY_RATING_USER_PROFILE_REPLY: MR_USER_PROFILE_CARD_ERROR\n");
			break;
		case MR_USER_PROFILE_CARD_MR_INCAPABLE:
			HLIB_CMD_Printf("MSG_MATURITY_RATING_USER_PROFILE_REPLY: MR_USER_PROFILE_SUBSCRIBER_PIN_WRONG\n");
			break;
		case MR_USER_PROFILE_SUBSCRIBER_PIN_WRONG:
			HLIB_CMD_Printf("MSG_MATURITY_RATING_USER_PROFILE_REPLY: MR_USER_PROFILE_SUBSCRIBER_PIN_WRONG\n");
			break;
		case MR_USER_PROFILE_SUBSCRIBER_PIN_BLOCK:
			HLIB_CMD_Printf("MSG_MATURITY_RATING_USER_PROFILE_REPLY: MR_USER_PROFILE_SUBSCRIBER_PIN_BLOCK\n");
			break;
		case MR_USER_PROFILE_ACCESS_NOT_ALLOWED:
			HLIB_CMD_Printf("MSG_MATURITY_RATING_USER_PROFILE_REPLY: MR_USER_PROFILE_USER_ID_OUT_OF_RANGE\n");
			break;
		case MR_USER_PROFILE_USER_ID_OUT_OF_RANGE:
			HLIB_CMD_Printf("MSG_MATURITY_RATING_USER_PROFILE_REPLY: MR_USER_PROFILE_USER_ID_OUT_OF_RANGE\n");
			break;
		case MR_USER_PROFILE_USER_NOT_EXIST:
			HLIB_CMD_Printf("MSG_MATURITY_RATING_USER_PROFILE_REPLY: MR_USER_PROFILE_USER_NOT_EXIST(index=%d)\n",  reply.stUserProfile.bProfileID);
			break;
		case MR_USER_PROFILE_USER_AGE_OUT_OF_RANGE:
			HLIB_CMD_Printf("MSG_MATURITY_RATING_USER_PROFILE_REPLY: MR_USER_PROFILE_USER_AGE_OUT_OF_RANGE\n");
			break;
		default :
			HLIB_CMD_Printf("MSG_MATURITY_RATING_USER_PROFILE_REPLY: The Result(%d) is not defined in document.\n", reply.eResult);
			break;
	}

}


#endif

#if defined(IRPLAT_TEST_VERIFYPLATFORM)
extern int StartPlatformValidator();
#endif
void local_IrDrv_Dbg_CmdTool(HUINT32 cmd, HUINT32 par1, HUINT32 par2, HUINT32 par3)
{
	ia_result result;

	HLIB_CMD_Printf("[IR_DBGTOOL]");
	switch(cmd)
	{
		case IR_DBGTOOL_SCELL_INFO:
			HLIB_CMD_Printf("[SCELL] MSG_CLIENT_VERSION_QUERY\t:");
			result = SCELL_Message(MSG_CLIENT_VERSION_QUERY, 0, 0, NULL);
			HLIB_CMD_Printf("[SCELL] MSG_COMPONENTS_VERSION_QUERY\t:");
			result = SCELL_Message(MSG_COMPONENTS_VERSION_QUERY, 0, 0, NULL);
			HLIB_CMD_Printf("0x%X\n", result);
			break;
#if defined(IRPLAT_TEST_VERIFYPLATFORM)
		case IR_DBGTOOL_VERITY_PLATFORM:

			StartPlatformValidator();
			break;
#endif

#if defined(CONFIG_MW_CAS_IRDETO_MATURITY_RATING)
		case IR_DBGTOOL_MR_USER_SET_PF:
			HLIB_CMD_Printf("[MR][SET PF] \n");
			{
				HUINT8 aucPIN[2];
				msg_mr_user_profile_data_st stpf;

				aucPIN[0] = (par1 >> 8) & 0xFF;
				aucPIN[1] = par1 & 0xFF;

				stpf.bProfileID = par2;
				stpf.bUserAge = par3;

				XSVC_CAS_IR_DRV_MR_UserProfileQuery(0, aucPIN, IA_MR_USER_PROFILE_SET , &stpf);
			}
			break;
		case IR_DBGTOOL_MR_USER_READ_PF:
			HLIB_CMD_Printf("[MR][READ] PF\n");
			{
				HUINT8 aucPIN[2];
				msg_mr_user_profile_data_st stpf;

				aucPIN[0] = (par1 >> 8) & 0xFF;
				aucPIN[1] = par1 & 0xFF;

				stpf.bProfileID = par2;
				XSVC_CAS_IR_DRV_MR_UserProfileQuery(0, aucPIN, IA_MR_USER_PROFILE_READ , &stpf );
			}
			break;

		case IR_DBGTOOL_MR_USER_DELETE_PF:
			HLIB_CMD_Printf("[MR][DELETE]\n");
			{
				HUINT8 aucPIN[2];
				msg_mr_user_profile_data_st stpf;

				aucPIN[0] = (par1 >> 8) & 0xFF;
				aucPIN[1] = par1 & 0xFF;

				stpf.bProfileID = par2;

				XSVC_CAS_IR_DRV_MR_UserProfileQuery(0, aucPIN, IA_MR_USER_PROFILE_DELETE, &stpf);
			}
			break;

		case IR_DBGTOOL_MR_INPUT_PIN:
			{
				HUINT8 aucPIN[2];

				aucPIN[0] = (par1 >> 8) & 0xFF;
				aucPIN[1] = par1 & 0xFF;

				HLIB_CMD_Printf("[MR][Input PIN(%02X%02X) \n", aucPIN[0], aucPIN[1]);

				/* 1st play handle used for debuuging parameter. */
				XSVC_CAS_IR_DRV_MR_UpdateResource(	g_IrSvcInfo[0].hIrSvc , aucPIN);
			}
			break;
		case IR_DBGTOOL_MR_CONFIG_READ:
			{
				HUINT8 aucPIN[2];

				aucPIN[0] = (par1 >> 8) & 0xFF;
				aucPIN[1] = par1 & 0xFF;

				HLIB_CMD_Printf("[MR][CONF][READ]\n");

				XSVC_CAS_IR_DRV_MR_ConfigurationQuery(0, aucPIN, IA_MR_CONFIG_READ, par2);
			}
			break;
		case IR_DBGTOOL_MR_CONFIG_SET:
			{
				HUINT8 aucPIN[2];

				aucPIN[0] = (par1 >> 8) & 0xFF;
				aucPIN[1] = par1 & 0xFF;

				HLIB_CMD_Printf("[MR][CONF][SET][%s]\n", (par2 == 0)?"DISABLE":  (par2 == 1)?"ENABLE":"WHAT ????" );

				XSVC_CAS_IR_DRV_MR_ConfigurationQuery(0, aucPIN, IA_MR_CONFIG_UPDATE, par2);
			}
			break;
		case IR_DBGTOOL_MR_SC_READ_STATUS:
			{
				HLIB_CMD_Printf("[MR][SC][READ]\n");

				//par1 is smc resource ID
				XSVC_CAS_IR_DRV_MR_CapabilityQuery(par1);
			}
			break;
		case IR_DBGTOOL_MR_USER_READ_ALL_PF:
			{
				HUINT8 aucPIN[2], aucDecPIN[4];

				aucPIN[0] = (par1 >> 8) & 0xFF;
				aucPIN[1] = par1 & 0xFF;

				aucDecPIN[0] = HLIB_MATH_Bcd2Dec((aucPIN[0] & 0xF0 )>>4 );
				aucDecPIN[1] = HLIB_MATH_Bcd2Dec((aucPIN[0] & 0x0F ));
				aucDecPIN[2] = HLIB_MATH_Bcd2Dec((aucPIN[1] & 0xF0 ) >> 4 );
				aucDecPIN[3] = HLIB_MATH_Bcd2Dec((aucPIN[1] & 0x0F ));

				HLIB_CMD_Printf("[MR][IR_DBGTOOL_MR_USER_READ_ALL_PF][%d][%d][%d][%d]\n", aucDecPIN[0], aucDecPIN[1], aucDecPIN[2], aucDecPIN[3]);

				xsvc_cas_IrMrReadAllUserPF(g_IrSvcInfo[0].hIrSvc, aucDecPIN);
			}
			break;
#endif
	}

}

void local_IrDrv_Dbg_CmdTool(HUINT32 cmd, HUINT32 par1, HUINT32 par2, HUINT32 par3);

#if 0
extern	HUINT32	IrCaDrvDebugLevel;
extern	HUINT32	g_IrApi_Level;
extern	HUINT32	g_IrSvc_Level;
extern	HUINT32	IrEvtDebugLevel;
extern	HUINT32	g_IrMsg_Level;
extern	HUINT32	IRNvrDebugLevel;
extern	HUINT32	g_IrMgr_Level;
extern	HUINT32	IR3OSDrvDebugLevel;
extern	HUINT32	IR3_SUPDebugLevel;
extern	HUINT32	IR3VDApiDebugLevel;
extern	HUINT32	VdDmxDebugLevel;
extern	HUINT32	IR3VDDscDebugLevel;
extern	HUINT32	VDExtStoDebugLevel;
extern	HUINT32	IR3VDLoaderDebugLevel;
extern	HUINT32	VdCCDebugLevel;
extern	HUINT32	VdScDebugLevel;
extern	HUINT32	IR3VDPStorageDebugLevel;
extern	HUINT32	IR3VDTimerDebugLevel;
extern	HUINT32	g_IrStatus_Level;

static void local_cmd_DbgCasIrdeto(unsigned long ulMode, char *pucSubModule, unsigned long ulLevel);

static CMD_TABLE_ITEM_t	s_IrDrvDbgInfo =
{
		"Irdeto",
		local_cmd_DbgCasIrdeto,
		{"irapi", "irmgr", "irdrv", "cadrv",  NULL},
		NULL
};

static void local_cmd_DbgCasIrdeto(unsigned long ulMode, char *pucSubModule, unsigned long ulLevel)
{
	switch(ulMode)
	{
		case CMDPROC_PRINT_STATE :

			if(IS_TABLEITEM(pucSubModule, "ALL"))
			{
				HLIB_CMD_Printf("	[CAS][IRDETO]	irApi	(%04x)\n", g_IrApi_Level);
				HLIB_CMD_Printf("	[CAS][IRDETO]   irMgr   (%04x)\n", g_IrMgr_Level);
				HLIB_CMD_Printf("	[CAS][IRDETO]   irSvc   (%04x)\n", g_IrSvc_Level);
				HLIB_CMD_Printf("	[CAS][IRDETO]   irEvt   (%04x)\n",IrEvtDebugLevel);
				HLIB_CMD_Printf("	[CAS][IRDETO]   caDrv   (%04x)\n", IrCaDrvDebugLevel);
				HLIB_CMD_Printf("	[CAS][IRDETO]   vdDsc   (%04x)\n", IR3VDDscDebugLevel);
				HLIB_CMD_Printf("	[CAS][IRDETO]   vdSc    (%04x)\n", VdScDebugLevel);
			} else
			{
				if (IS_TABLEITEM(pucSubModule, "IRAPI"))
					HLIB_CMD_Printf("	[CAS][IRDETO]	irApi	(%04x)\n", g_IrApi_Level);
				if (IS_TABLEITEM(pucSubModule, "IRMGR"))
					HLIB_CMD_Printf("	[CAS][IRDETO]   irMgr   (%04x)\n", g_IrMgr_Level);
				if (IS_TABLEITEM(pucSubModule, "IRSVC"))
					HLIB_CMD_Printf("	[CAS][IRDETO]   irSvc   (%04x)\n", g_IrSvc_Level);
				if (IS_TABLEITEM(pucSubModule, "IREVT"))
					HLIB_CMD_Printf("	[CAS][IRDETO]   irEvt   (%04x)\n", IrEvtDebugLevel);
				if (IS_TABLEITEM(pucSubModule, "IRSTS"))
					HLIB_CMD_Printf("	[CAS][IRDETO]   irStatus   (%04x)\n", g_IrStatus_Level);
				if (IS_TABLEITEM(pucSubModule, "CADRV"))
					HLIB_CMD_Printf("	[CAS][IRDETO]   caDrv   (%04x)\n", IrCaDrvDebugLevel);
				if (IS_TABLEITEM(pucSubModule, "VDDSC"))
					HLIB_CMD_Printf("	[CAS][IRDETO]   vdDsc   (%04x)\n", IR3VDDscDebugLevel);
				if (IS_TABLEITEM(pucSubModule, "VDSC"))
					HLIB_CMD_Printf("	[CAS][IRDETO]   vdSc    (%04x)\n", VdScDebugLevel);
			}
			break;

		case CMDPROC_SET_STATE :
			if (IS_TABLEITEM(pucSubModule, "IRDBG"))
				HLIB_CMD_Printf("	[CAS][IRDETO]	irDbg	(%04x)\n", (g_IrDebug_Level = ulLevel));
			if (IS_TABLEITEM(pucSubModule, "IRAPI"))
				HLIB_CMD_Printf("	[CAS][IRDETO]	irApi	(%04x)\n", (g_IrApi_Level = ulLevel));
			if (IS_TABLEITEM(pucSubModule, "IRMGR"))
				HLIB_CMD_Printf("	[CAS][IRDETO]   irMgr   (%04x)\n", (g_IrMgr_Level = ulLevel));
			if (IS_TABLEITEM(pucSubModule, "IRSVC"))
				HLIB_CMD_Printf("	[CAS][IRDETO]   irSvc   (%04x)\n", (g_IrSvc_Level = ulLevel));
			if (IS_TABLEITEM(pucSubModule, "IREVT"))
				HLIB_CMD_Printf("	[CAS][IRDETO]	irEvt	(%04x)\n", (IrEvtDebugLevel = ulLevel));
			if (IS_TABLEITEM(pucSubModule, "IRSTS"))
				HLIB_CMD_Printf("	[CAS][IRDETO]	irSts	(%04x)\n", (g_IrStatus_Level = ulLevel));
			if (IS_TABLEITEM(pucSubModule, "CADRV"))
				HLIB_CMD_Printf("	[CAS][IRDETO]   caDrv   (%04x)\n", (IrCaDrvDebugLevel = ulLevel));
			if (IS_TABLEITEM(pucSubModule, "VDDSC"))
				HLIB_CMD_Printf("	[CAS][IRDETO]   vdDsc   (%04x)\n", (IR3VDDscDebugLevel = ulLevel));
			if (IS_TABLEITEM(pucSubModule, "VDSC"))
				HLIB_CMD_Printf("	[CAS][IRDETO]   vdSc    (%04x)\n", (VdScDebugLevel = ulLevel));
			break;

		default :

			break;
	}
	return;

}
#endif

static int local_cmd_IrDrvCommand(void *szArgList)
{
	char *aucArg=NULL;
	HUINT8 aucStr[20+1]={0,};

	/* 1st argument */
	aucArg = (char *)HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
	if( aucArg == NULL )
		return HxCMD_ERR;
	if( snprintf(aucStr, 20, "%s", aucArg) == 0 )
		return HxCMD_ERR;

	HLIB_STD_StrUpper(aucStr);

	if (strcmp(aucStr, "SVC") == 0)
	{
		local_IrDrv_Dbg_PrintSvcInfo();
		return HxCMD_OK;
	}
	else if (strcmp(aucStr, "TP") == 0)
	{
		local_IrDrv_Dbg_PrintTpInfo();
		return HxCMD_OK;
	}
	else if(strcmp(aucStr, "SC") == 0)
	{
		local_IrDrv_Dbg_PrintScInfo();
	}
	else if(strcmp(aucStr, "SCELL") == 0)
	{
		local_IrDrv_Dbg_CmdTool(IR_DBGTOOL_SCELL_INFO, 0 ,0 ,0);
	}
	else if(strcmp(aucStr, "MAIL") == 0 )
	{
		aucArg = (char *)HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
		if(aucArg == NULL)
		{
			return HxCMD_ERR;
		}
		if( snprintf(aucStr, 20, "%s", aucArg) == 0  )
		{
			return HxCMD_ERR;
		}
		HLIB_STD_StrUpper(aucStr);

		if(strcmp(aucStr, "INFO") == 0)
		{
			CAS_IR_MSG_DRV_DbgMailBox();
		}
		else if(strcmp(aucStr, "ERASE") == 0)
		{
			CAS_IR_MAIL_EraseFile();
		}
		else
		{
			HLIB_CMD_Printf("choose info or erase\n");
		}
	}
	else if(strcmp(aucStr, "ERASE") == 0 )
	{
		aucArg = (char *)HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
		if(aucArg == NULL)
		{
			HLIB_CMD_Printf("choose [ca] or [ir] or [loader]\n");
			return HxCMD_ERR;
		}
		if( snprintf(aucStr, 20, "%s", aucArg) == 0  )
		{
			HLIB_CMD_Printf("aucArg error \n");
			return HxCMD_ERR;
		}
		HLIB_STD_StrUpper(aucStr);

		if(strcmp(aucStr, "CA") == 0)
		{
			HUINT8 aTemp[IR_PARTITION_CA_SIZE] = {0x00,};
			if (HIR_OK == CAS_IR_NVR_Partition_Write(IRNVR_PART_CA, 0, aTemp, IR_PARTITION_CA_SIZE ) )
			{
				HLIB_CMD_Printf("Erase CA partition :ok\n");
			}
			else
			{
				HLIB_CMD_Printf("Erase CA partition :failed\n");
			}
		}
		else if(strcmp(aucStr, "IR") == 0)
		{
			HUINT8 aTemp[IR_PARTITION_IRDETO_SIZE] = {0x00,};
			if (HIR_OK == CAS_IR_NVR_Partition_Write(IRNVR_PART_IRDETO, 0, aTemp, IR_PARTITION_IRDETO_SIZE ) )
			{
				HLIB_CMD_Printf("Erase irdeto partition :ok\n");
			}
			else
			{
				HLIB_CMD_Printf("Erase irdeto  partition :failed\n");
			}
		}
		else if(strcmp(aucStr, "LOADER") == 0)
		{
			HUINT8 aTemp[IR_PARTITION_LOADER_SIZE] = {0x00,};
			if (HIR_OK == CAS_IR_NVR_Partition_Write(IRNVR_PART_LOADER, 0, aTemp, IR_PARTITION_LOADER_SIZE ) )
			{
				HLIB_CMD_Printf("Erase Loader partition :ok\n");
			}
			else
			{
				HLIB_CMD_Printf("Erase Loader partition :failed\n");
			}
		}
		else
		{
			HLIB_CMD_Printf("choose [ca] or [ir] or [loader]\n");
		}
	}
	else if(strcmp(aucStr, "READ") == 0 )
	{
		aucArg = (char *)HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
		if(aucArg == NULL)
		{
			HLIB_CMD_Printf("choose [ca] or [ir] or [loader]\n");
			return HxCMD_ERR;
		}
		if( snprintf(aucStr, 20, "%s", aucArg) == 0  )
		{
			HLIB_CMD_Printf("aucArg error \n");
			return HxCMD_ERR;
		}
		HLIB_STD_StrUpper(aucStr);

		if(strcmp(aucStr, "CA") == 0)
		{
			HUINT8 aTemp[IR_PARTITION_CA_SIZE] = {0x00,};
			HUINT32	i = 0;
			if (HIR_OK == CAS_IR_NVR_Partition_Read(IRNVR_PART_CA, 0, aTemp, IR_PARTITION_CA_SIZE ) )
			{
				HLIB_CMD_Printf("read CA partition :ok\n");
				for(i = 0; i < IR_PARTITION_CA_SIZE ; i++)
				{
					if(i%16 == 0)
					{
						HLIB_CMD_Printf("\n");
					}

					HLIB_CMD_Printf("%02x ",  aTemp[i]);

				}
				HLIB_CMD_Printf("\n");

			}
			else
			{
				HLIB_CMD_Printf("read CA partition :failed\n");
			}
		}
		else if(strcmp(aucStr, "IR") == 0)
		{
			HUINT8 aTemp[IR_PARTITION_IRDETO_SIZE] = {0x00,};
			HUINT32	i = 0;
			if (HIR_OK == CAS_IR_NVR_Partition_Read(IRNVR_PART_IRDETO, 0, aTemp, IR_PARTITION_IRDETO_SIZE ) )
			{
				HLIB_CMD_Printf("read irdeto partition :ok\n");
				for(i = 0; i < IR_PARTITION_IRDETO_SIZE ; i++)
				{
					if(i%16 == 0)
					{
						HLIB_CMD_Printf("\n");
					}

					HLIB_CMD_Printf("%02x ",  aTemp[i]);
				}
				HLIB_CMD_Printf("\n");


			}
			else
			{
				HLIB_CMD_Printf("read irdeto  partition :failed\n");
			}
		}
		else if(strcmp(aucStr, "LOADER") == 0)
		{
			HUINT8 aTemp[IR_PARTITION_LOADER_SIZE] = {0x00,};
			HUINT32	i=0;
			if (HIR_OK == CAS_IR_NVR_Partition_Read(IRNVR_PART_LOADER, 0, aTemp, IR_PARTITION_LOADER_SIZE ) )
			{
				HLIB_CMD_Printf("read Loader partition :ok\n");
				for(i = 0; i < IR_PARTITION_LOADER_SIZE ; i++)
				{

					if(i%16 == 0)
					{
						HLIB_CMD_Printf("\n");
					}
					HLIB_CMD_Printf("%02x ",  aTemp[i]);
				}
				HLIB_CMD_Printf("\n");


			}
			else
			{
				HLIB_CMD_Printf("read Loader partition :failed\n");
			}
		}
		else
		{
			HLIB_CMD_Printf("choose [ca] or [ir] or [loader]\n");
		}
	}
	else if (strcmp(aucStr, "DMX") == 0)
	{
		HVD_DMX_PrintDemuxInfo();
		return HxCMD_OK;
	}
	else if(strcmp(aucStr, "ETC") == 0)
	{
		HUINT32	cmd = 0,arg1=0,arg2=0,arg3=0;

		aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList );
		if( aucArg != NULL ) sscanf(aucArg, "%d", &cmd) ;

		aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList );
		if( aucArg != NULL ) sscanf(aucArg, "%d", &arg1) ;

		aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList );
		if( aucArg != NULL ) sscanf(aucArg, "%d", &arg2) ;

		aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList );
		if( aucArg != NULL ) sscanf(aucArg, "%d", &arg3) ;

		local_IrDrv_Dbg_CmdTool(cmd, arg1,arg2,arg3);

		return HxCMD_OK;
	}
#if defined(IRPLAT_FN_COPY_CONTROL)
	else if(strcmp(aucStr, "COPY") == 0)
	{
		HUINT32 cmd;

		aucArg = CMD_NextArg( (char **)&szArgList );
		if( aucArg != NULL ) sscanf(aucArg, "%d", &cmd) ;

		switch(cmd)
		{
			case 0:
				CAS_IR_MGR_NotifyCopyControlEvent(g_IrSvcInfo[0].hAction, 0x07, 0, 0, 0);
				break;

			case 1:
				CAS_IR_MGR_NotifyCopyControlEvent(g_IrSvcInfo[0].hAction, 0x07, 1, 1, 1);
				break;

			case 2:
				CAS_IR_MGR_NotifyCopyControlEvent(g_IrSvcInfo[0].hAction, 0x07, 2, 2, 2);
				break;

			case 3:
				CAS_IR_MGR_NotifyCopyControlEvent(g_IrSvcInfo[0].hAction, 0x07, 3, 3, 3);
				break;

		}
	}
#endif
	else if (strcmp(aucStr, "ERR") == 0)
	{
		HMSG		msg = {0, };
		HUINT32		interval = 0;

		aucArg = (HCHAR *)HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
		if( aucArg == NULL )
			return HxCMD_ERR;
		if( sscanf(aucArg, "%d", &interval) == 0 )
			return HxCMD_ERR;

		msg.usMsgClass = IREVT_CLASS_CAS_ERROR_FOR_SERVICE;
		msg.usParam = AXI_STATUSCODE_IDX_E33_4;
		msg.ulParam = 0;//(HUINT32)MAKE_IR_STATUSCODE(CharSeverity, status.wStatus, status.eSource);
		CAS_IR_EVT_SendEvent(g_IrSvcInfo[0].hAction, &msg);

		VK_TASK_Sleep(interval);

		msg.usMsgClass = IREVT_CLASS_CAS_ERROR_FOR_SERVICE;
		msg.usParam = AXI_STATUSCODE_IDX_I07_4;
		msg.ulParam = 0;//(HUINT32)MAKE_IR_STATUSCODE(CharSeverity, status.wStatus, status.eSource);
		CAS_IR_EVT_SendEvent(g_IrSvcInfo[0].hAction, &msg);

		VK_TASK_Sleep(interval);

		msg.usMsgClass = IREVT_CLASS_CAS_ERROR_FOR_SERVICE;
		msg.usParam = AXI_STATUSCODE_IDX_E101_3;
		msg.ulParam = 0;//(HUINT32)MAKE_IR_STATUSCODE(CharSeverity, status.wStatus, status.eSource);
		CAS_IR_EVT_SendEvent(g_IrSvcInfo[0].hAction, &msg);


		return HxCMD_OK;
	}

	return HxCMD_ERR;
}

static	int	local_cmd_IrMVCommand(void *szArgList)
{
	char 	*aucArg=NULL;
	int		nEncryption;

	/* 1st argument */
	aucArg = (char *)HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
	if( aucArg == NULL )
		return HxCMD_ERR;
	if( sscanf(aucArg, "%d", &nEncryption) == 0 )
		return HxCMD_ERR;

#if defined(IRPLAT_SYS_PVR)
	switch (nEncryption)
	{
		case	1:
			IR_MV_DBG_ForceEncryptionForAllChannel(1);
			break;
		case	2:
			IR_MV_DBG_ForceEncryptionForAllChannel(2);
			break;
		default:
			IR_MV_DBG_ForceEncryptionForAllChannel(0);
			break;
	}
#else
	HLIB_CMD_Printf("not supported in zapper!!\n");
#endif

	return HxCMD_OK;
}

#if defined(IRPLAT_FN_DDEMM_USER_PAYLOAD_FOR_DIGITURK)
HUINT8 ITO_DDEMM_VIRTUAL_BILL[] =
{
	0x60, 0x01,0x00,0x03,0x09,0x00 ,0x01 ,0x00,0x17 ,0x3C ,0x00 ,0x00,0x01 ,0x01 ,0x00,0x10 ,0x30,0x31,
		  0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x30,0x31,0x32,0x33,0x34,0x35
};

HUINT8 ITO_DDEMM_DM_PARAMETERS[] =
{
	0x60, 0x01 ,0x00,0x03 ,0x09 ,0x00 ,0x01 ,0x00,0x49 ,0x3C ,0x00 ,0x00,0x01 ,0x02 ,0x00,0x42
	,0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x30,0x31,0x32,0x33,0x34,0x35
	,0x0A,0x0B,0x0C,0x0D ,0x00,0x28 ,0x00,0xD3 /*(Yearly at 06.19)*/ ,0x10,0x06
	,0x41,0x42,0x43,0x44,0x45 ,0x46,0x47,0x48,0x49,0x4A ,0x4B,0x4C,0x4D,0x4E,0x4F
	,0x50,0x51,0x52,0x53,0x00 ,0x61,0x62,0x63,0x64,0x65 ,0x66,0x67,0x68,0x69,0x6A
	,0x6B,0x6C,0x6D,0x6E,0x6F ,0x00,0x00,0x00,0x00,0x00
};


HUINT8 ITO_DDEMM_FEATURE_BITMAP[] =
{
	0x60, 0x01 ,0x00,0x03 ,0x09 ,0x00 ,0x01 ,0x00,0x0B ,0x3C ,0x00 ,0x00,0x01 ,0x03 ,0x00,0x04 ,0x00,0x00,0x00,0x01
};

HUINT8 ITO_DDEMM_IPPV_IP_AND_PHONE_NO[] =
{
	0x60, 0x01 ,0x00,0x03 ,0x09 ,0x00 ,0x01 ,0x00,0x1D ,0x3C ,0x00 ,0x00,0x01 ,0x04 ,0x00,0x16
	,0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x30,0x31,0x32,0x33,0x34,0x00 ,0x0A,0x0B,0x0C,0x0D ,0x00,0x28
};

HUINT8 ITO_DDEMM_IPPV_DEBIT_LIMIT_THRESHOLD[] =
{
	0x60, 0x01 ,0x00,0x03 ,0x09 ,0x00 ,0x01 ,0x00,0x08 ,0x3C ,0x00 ,0x00,0x01 ,0x05 ,0x00,0x01 ,0x40
};

HUINT8 ITO_DDEMM_IPPV_SCHEDULE[] =
{
	0x60, 0x01 ,0x00,0x03 ,0x09 ,0x00 ,0x01 ,0x00,0x0C ,0x3C ,0x00 ,0x00,0x01 ,0x06 ,0x00,0x05 ,0x01 ,0x00,0xD3 /*(Yearly at 06.19)*/ ,0x10,0x06
};

HUINT8 ITO_DDEMM_REPORT_STATUS[] =
{
	0x60, 0x01 ,0x00,0x03 ,0x09 ,0x00 ,0x01 ,0x00,0x46 ,0x3C ,0x00 ,0x00,0x01 ,0x07 ,0x00,0x3F ,0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x30,0x31,0x32,0x33,0x34,0x00
	,0x0A,0x0B,0x0C,0x0D ,0x00,0x28 ,0x41,0x42,0x43,0x44,0x45 ,0x46,0x47,0x48,0x49,0x4A ,0x4B,0x4C,0x4D,0x4E,0x4F ,0x50,0x51,0x52,0x53,0x00 ,0x61,0x62,0x63,0x64,0x65
	,0x66,0x67,0x68,0x69,0x6A ,0x6B,0x6C,0x6D,0x6E,0x6F ,0x00,0x00,0x00,0x00,0x00 ,0x00
};

HUINT8 ITO_DDEMM_SET_DOWNLOAD_TYPE[] =
{
	0x60, 0x01 ,0x00,0x03 ,0x09 ,0x00 ,0x01 ,0x00,0x09 ,0x3C ,0x00 ,0x00,0x01 ,0x08 ,0x00,0x02 ,0x01 ,0x00
};

HUINT8 ITO_DDEMM_SET_OTHEOR_ENABLE[] =
{
	0x60, 0x01 ,0x00,0x03 ,0x09 ,0x00 ,0x01 ,0x00,0x09 ,0x3C ,0x00 ,0x00,0x02 ,0x50 ,0x00,0x0b ,0x01
	/*,'1', '0', 'A', '3', '3', '7', '0', '7',*/
	,'F', 'F', 'F', 'F', 'F', 'F', 'F', 'F',
	0x00, 0x00
};

HUINT8 ITO_DDEMM_SET_OTHEOR_DISABLE[] =
{
	0x60, 0x01 ,0x00,0x03 ,0x09 ,0x00 ,0x01 ,0x00,0x09 ,0x3C ,0x00 ,0x00,0x02 ,0x50 ,0x00,0x0b ,0x00
	/*,'1', '0', 'A', '3', '3', '7', '0', '7',*/
	,'F', 'F', 'F', 'F', 'F', 'F', 'F', 'F',
	0x00, 0x00
};

HUINT8 ITO_DDEMM_SET_FTA_LIMIT_ENABLE[] =
{
	0x60, 0x01 ,0x00,0x03 ,0x09 ,0x00 ,0x01 ,0x00,0x09 ,0x3C ,0x00 ,0x00,0x02 ,0x54 ,0x00,0x0b ,0x01
	/*,'1', '0', 'A', '3', '3', '7', '0', '7',*/
	,'F', 'F', 'F', 'F', 'F', 'F', 'F', 'F',
	0x3C, 0x00
};

HUINT8 ITO_DDEMM_SET_FTA_LIMIT_DISABLE[] =
{
	0x60, 0x01 ,0x00,0x03 ,0x09 ,0x00 ,0x01 ,0x00,0x09 ,0x3C ,0x00 ,0x00,0x02 ,0x54 ,0x00,0x0b ,0x00
	/*,'1', '0', 'A', '3', '3', '7', '0', '7',*/
	,'F', 'F', 'F', 'F', 'F', 'F', 'F', 'F',
	0x00, 0x00

};

HUINT8 ITO_DDEMM_RESET_PIN[] =
{
	0x40, 0x00 ,0x00,0x00
};

HUINT8 ITO_DDEMM_RECOVERY_RESET[] =
{
	0x50, 0x00
};

HUINT8 ITO_DDEMM_RECOVERY_RESET_CH_ORDER[] =
{
	0x50, 0x10, 0x00, 0X80
};

HUINT8 ITO_DDEMM_RECOVERY_RESCAN[] =
{
	0x50, 0x20,
};

HUINT8 ITO_DDEMM_RECOVERY_RESCAN_HOMING_CH[] =
{
	0x50, 0x30,
};

HUINT8 ITO_DDEMM_RECOVERY_RECONNECT[] =
{
	0x50, 0x40,
};

HUINT8 ITO_DDEMM_RECOVERY_RESET_INST_PIN[] =
{
	0x50, 0x60, 0x00, 0x00
};

HUINT8 ITO_DDEMM_CS_PHONE_NO[] =
{
	0x60, 0x01 ,0x00,0x03 ,0x09 ,0x00 ,0x01 ,0x00,0x1D ,0x3C ,0x00 ,0x00,0x02 ,0x0a ,0x00,0x10
	,0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x30,0x31,0x32,0x33,0x34,0x00
};

HUINT8 ITO_DDEMM_PPV_PHONE_NO[] =
{
	0x60, 0x01 ,0x00,0x03 ,0x09 ,0x00 ,0x01 ,0x00,0x1D ,0x3C ,0x00 ,0x00,0x02 ,0x0b ,0x00,0x10
	,0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x30,0x31,0x32,0x33,0x34,0x00
};

HUINT8 ITO_DDEMM_DISPLAY_CARD_NO[] =
{
	0x60, 0x01 ,0x00,0x03 ,0x09 ,0x00 ,0x01 ,0x00,0x0B ,0x3C ,0x00 ,0x00,0x02 ,0x03 ,0x00,0x05 ,0x00,0xC8 ,0x00,0x50 ,0x0A
};

HUINT8 ITO_DDEMM_COLOUR_DISPLAY_NO[] =
{
	0x60, 0x01 ,0x00,0x03 ,0x09 ,0x00 ,0x01 ,0x00,0x0B ,0x3C ,0x00 ,0x00,0x02 ,0x09 ,0x01,0x09 ,0x00,0xC8 ,0x00,0x50 ,0x01
	, 0x00, 0x00, 0x00, 0x10, 0xFF, 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r'
	,'s', 't', 'u', 'v', 'w', 'x', 'y', 'z', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q'
	, 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', 'a', 'b', 'c', 'd', 'e'
	, 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r' ,'s', 't', 'u', 'v', 'w', 'x', 'y', 'z', 'A', 'B', 'C', 'D'
	, 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q' , 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '0', '1', '2'
	, '3', '4', '5', '6', '7', '8', '9', '0',  'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r'
	,'s', 't', 'u', 'v', 'w', 'x', 'y', 'z', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q'
	, 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', 'a', 'b', 'c', 'd', 'e'
	, 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r' ,'s', 't', 'u', 'v', 'w', 'x', 'y', 'z', 'A', 'B', 'C', 'D'
	, 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q' , 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '0', '1', '2'
	, '3', '4', '5', '6', '7', '8', '9', '0', '8', '9', '0'
};

HUINT8 ITO_DDEMM_EASY_SMS_NO[] =
{
	0x60, 0x01 ,0x00,0x03 ,0x09 ,0x00 ,0x01 ,0x00,0x0B ,0x3C ,0x00 ,0x00,0x02 ,0x51 ,0x01,0x0E ,0x00,0xC8 ,0x00,0x50 ,0x01
	, 'D', 'K', '3', '\0', '\0', '\0', '\0', '\0'
	, 0x0A, 0xFF, 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r'
	,'s', 't', 'u', 'v', 'w', 'x', 'y', 'z', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q'
	, 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', 'a', 'b', 'c', 'd', 'e'
	, 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r' ,'s', 't', 'u', 'v', 'w', 'x', 'y', 'z', 'A', 'B', 'C', 'D'
	, 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q' , 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '0', '1', '2'
	, '3', '4', '5', '6', '7', '8', '9', '0',  'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r'
	,'s', 't', 'u', 'v', 'w', 'x', 'y', 'z', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q'
	, 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', 'a', 'b', 'c', 'd', 'e'
	, 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r' ,'s', 't', 'u', 'v', 'w', 'x', 'y', 'z', 'A', 'B', 'C', 'D'
	, 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q' , 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '0', '1', '2'
	, '3', '4', '5', '6', '7', '8', '9', '0', '8', '9', '0'
};

HUINT8 ITO_DDEMM_REMOTE_TAGGING[] =
{
	0x60, 0x01 ,0x00,0x03 ,0x09 ,0x00 ,0x01 ,0x00,0x0B ,0x3C ,0x00 ,0x00,0x02 ,0x05 ,0x00,0x6e ,0x00,0x16 ,0x00,0x00,0x00,0x00
	,0x00,0x02 ,0x00,0x00,0x00,0x00 ,0x00,0x00,0x00,0x00 ,0x00,0x00,0x00,0x00, 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j',
	'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r' ,'s', 't', 'u', 'v', 'w', 'x', 'y', 'z', 'A', 'B', 'C', 0x00,
	'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r' ,'s', 't', 'u', 'v', 'w', 'x', 'y',
	'z', 'A', 'B', 'C', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r' ,'s', 't', 'u',
	'v', 'w', 'x', 'y', 'z', 'A', 'B', 'C', 'D', 0x00
};

HUINT8 ITO_DDEMM_REMOTE_RECORDING[] =
{
	0x60, 0x01 ,0x00,0x03 ,0x09 ,0x00 ,0x01 ,0x00,0x0B ,0x3C ,0x00 ,0x00,0x02 ,0x06 ,0x00,0x78 ,0x00,0x7e ,0x51,0xA4
	,0x1e,0xE8 ,0x00,0x00,0x00,0x00 ,0x00,0x00 ,0x00,0x00 ,0x00,0x00,0x00,0x0A ,0x00,0x00,0x00,0x0A ,'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j',
	'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r' ,'s', 't', 'u', 'v', 'w', 'x', 'y', 'z', 'A', 'B', 'C', 0x00,
	'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r' ,'s', 't', 'u', 'v', 'w', 'x', 'y',
	'z', 'A', 'B', 'C', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r' ,'s', 't', 'u',
	'v', 'w', 'x', 'y', 'z', 'A', 'B', 'C', 'D', 0x00, 0x00, 0x00,0x05, 0x00,0x00,0x00,0x00 ,0x00
};

static int local_cmd_IrDrvCommandForDDEMM(void *szArgList)
{
	char *aucArg=NULL;
	HUINT32 ulCmd;

	/* 1st argument */
	aucArg = (char *)HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
	if( aucArg == NULL )
		return HxCMD_ERR;
	if( sscanf(aucArg, "%d", &ulCmd) == 0 )
		return HxCMD_ERR;

	switch(ulCmd)
	{
		case 0:
			HLIB_CMD_Printf("ITO_DDEMM_VIRTUAL_BILL\n");
			CAS_IR_MSG_DRV_DbgCntrMsg(0, ITO_DDEMM_VIRTUAL_BILL, sizeof(ITO_DDEMM_VIRTUAL_BILL));
			break;
		case 1:
			HLIB_CMD_Printf("ITO_DDEMM_DM_PARAMETERS\n");
			CAS_IR_MSG_DRV_DbgCntrMsg(0, ITO_DDEMM_DM_PARAMETERS, sizeof(ITO_DDEMM_DM_PARAMETERS));
			break;
		case 2:
			HLIB_CMD_Printf("ITO_DDEMM_FEATURE_BITMAP\n");
			CAS_IR_MSG_DRV_DbgCntrMsg(0, ITO_DDEMM_FEATURE_BITMAP, sizeof(ITO_DDEMM_FEATURE_BITMAP));
			break;
		case 3:
			HLIB_CMD_Printf("ITO_DDEMM_IPPV_IP_AND_PHONE_NO\n");
			CAS_IR_MSG_DRV_DbgCntrMsg(0, ITO_DDEMM_IPPV_IP_AND_PHONE_NO, sizeof(ITO_DDEMM_IPPV_IP_AND_PHONE_NO));
			break;
		case 4:
			HLIB_CMD_Printf("ITO_DDEMM_IPPV_DEBIT_LIMIT_THRESHOLD\n");
			CAS_IR_MSG_DRV_DbgCntrMsg(0, ITO_DDEMM_IPPV_DEBIT_LIMIT_THRESHOLD, sizeof(ITO_DDEMM_IPPV_DEBIT_LIMIT_THRESHOLD));
			break;
		case 5:
			HLIB_CMD_Printf("ITO_DDEMM_IPPV_SCHEDULE\n");
			CAS_IR_MSG_DRV_DbgCntrMsg(0, ITO_DDEMM_IPPV_SCHEDULE, sizeof(ITO_DDEMM_IPPV_SCHEDULE));
			break;
		case 6:
			HLIB_CMD_Printf("ITO_DDEMM_REPORT_STATUS\n");
			CAS_IR_MSG_DRV_DbgCntrMsg(0, ITO_DDEMM_REPORT_STATUS, sizeof(ITO_DDEMM_REPORT_STATUS));
			break;
		case 7:
			HLIB_CMD_Printf("ITO_DDEMM_SET_DOWNLOAD_TYPE\n");
			CAS_IR_MSG_DRV_DbgCntrMsg(0, ITO_DDEMM_SET_DOWNLOAD_TYPE, sizeof(ITO_DDEMM_SET_DOWNLOAD_TYPE));
			break;
		case 8:
			HLIB_CMD_Printf("ITO_DDEMM_RESET_PIN\n");
			CAS_IR_MSG_DRV_DbgCntrMsg(0, ITO_DDEMM_RESET_PIN, sizeof(ITO_DDEMM_RESET_PIN));
			break;
		case 9:
			HLIB_CMD_Printf("ITO_DDEMM_RECOVERY_RESET\n");
			CAS_IR_MSG_DRV_DbgCntrMsg(0, ITO_DDEMM_RECOVERY_RESET, sizeof(ITO_DDEMM_RECOVERY_RESET));
			break;
		case 10:
			HLIB_CMD_Printf("ITO_DDEMM_RECOVERY_RESET_CH_ORDER\n");
			CAS_IR_MSG_DRV_DbgCntrMsg(0, ITO_DDEMM_RECOVERY_RESET_CH_ORDER, sizeof(ITO_DDEMM_RECOVERY_RESET_CH_ORDER));
			break;
		case 11:
			HLIB_CMD_Printf("ITO_DDEMM_RECOVERY_RESCAN\n");
			CAS_IR_MSG_DRV_DbgCntrMsg(0, ITO_DDEMM_RECOVERY_RESCAN, sizeof(ITO_DDEMM_RECOVERY_RESCAN));
			break;
		case 12:
			HLIB_CMD_Printf("ITO_DDEMM_RECOVERY_RESCAN_HOMING_CH\n");
			CAS_IR_MSG_DRV_DbgCntrMsg(0, ITO_DDEMM_RECOVERY_RESCAN_HOMING_CH, sizeof(ITO_DDEMM_RECOVERY_RESCAN_HOMING_CH));
			break;
		case 13:
			HLIB_CMD_Printf("ITO_DDEMM_RECOVERY_RECONNECT\n");
			CAS_IR_MSG_DRV_DbgCntrMsg(0, ITO_DDEMM_RECOVERY_RECONNECT, sizeof(ITO_DDEMM_RECOVERY_RECONNECT));
			break;
		case 14:
			HLIB_CMD_Printf("ITO_DDEMM_RECOVERY_RESET_INST_PIN\n");
			CAS_IR_MSG_DRV_DbgCntrMsg(0, ITO_DDEMM_RECOVERY_RESET_INST_PIN, sizeof(ITO_DDEMM_RECOVERY_RESET_INST_PIN));
			break;
		case 15:
			HLIB_CMD_Printf("ITO_DDEMM_SET_OTHEOR_ENABLE\n");
			CAS_IR_MSG_DRV_DbgCntrMsg(0, ITO_DDEMM_SET_OTHEOR_ENABLE, sizeof(ITO_DDEMM_SET_OTHEOR_ENABLE));
			break;
		case 16:
			HLIB_CMD_Printf("ITO_DDEMM_SET_OTHEOR_DISABLE\n");
			CAS_IR_MSG_DRV_DbgCntrMsg(0, ITO_DDEMM_SET_OTHEOR_DISABLE, sizeof(ITO_DDEMM_SET_OTHEOR_DISABLE));
			break;
		case 17:
			HLIB_CMD_Printf("ITO_DDEMM_CS_PHONE_NO\n");
			CAS_IR_MSG_DRV_DbgCntrMsg(0, ITO_DDEMM_CS_PHONE_NO, sizeof(ITO_DDEMM_CS_PHONE_NO));
			break;
		case 18:
			HLIB_CMD_Printf("ITO_DDEMM_PPV_PHONE_NO\n");
			CAS_IR_MSG_DRV_DbgCntrMsg(0, ITO_DDEMM_PPV_PHONE_NO, sizeof(ITO_DDEMM_PPV_PHONE_NO));
			break;
		case 19:
			HLIB_CMD_Printf("ITO_DDEMM_SET_FTA_LIMIT_ENABLE\n");
			CAS_IR_MSG_DRV_DbgCntrMsg(0, ITO_DDEMM_SET_FTA_LIMIT_ENABLE, sizeof(ITO_DDEMM_SET_FTA_LIMIT_ENABLE));
			break;
		case 20:
			HLIB_CMD_Printf("ITO_DDEMM_SET_FTA_LIMIT_DISABLE\n");
			CAS_IR_MSG_DRV_DbgCntrMsg(0, ITO_DDEMM_SET_FTA_LIMIT_DISABLE, sizeof(ITO_DDEMM_SET_FTA_LIMIT_DISABLE));
			break;
		case 21:
			HLIB_CMD_Printf("ITO_DDEMM_DISPLAY_CARD_NO\n");
			CAS_IR_MSG_DRV_DbgCntrMsg(0, ITO_DDEMM_DISPLAY_CARD_NO, sizeof(ITO_DDEMM_DISPLAY_CARD_NO));
			break;
		case 22:
			HLIB_CMD_Printf("ITO_DDEMM_COLOUR_DISPLAY_NO\n");
			CAS_IR_MSG_DRV_DbgCntrMsg(0, ITO_DDEMM_COLOUR_DISPLAY_NO, sizeof(ITO_DDEMM_COLOUR_DISPLAY_NO));
			break;
		case 23:
			HLIB_CMD_Printf("ITO_DDEMM_EASY_SMS_NO\n");
			CAS_IR_MSG_DRV_DbgCntrMsg(0, ITO_DDEMM_EASY_SMS_NO, sizeof(ITO_DDEMM_EASY_SMS_NO));
			break;
		case 24:
			HLIB_CMD_Printf("ITO_DDEMM_REMOTE_TAGGING\n");
			CAS_IR_MSG_DRV_DbgCntrMsg(0, ITO_DDEMM_REMOTE_TAGGING, sizeof(ITO_DDEMM_REMOTE_TAGGING));
			break;

		case 25:
			HLIB_CMD_Printf("ITO_DDEMM_REMOTE_RECORDING\n");
			CAS_IR_MSG_DRV_DbgCntrMsg(0, ITO_DDEMM_REMOTE_RECORDING, sizeof(ITO_DDEMM_REMOTE_RECORDING));
			break;
		default:
			break;
	}

	return HxCMD_OK;
}
#endif

#if defined(IRPLAT_SYS_SAVE_SMARTCARD_NUMBER)
static	int	local_cmd_IrSaveSCNum(void *szArgList)
{
	HUMAX_VdSc_SaveSmartcardNumber();

	return CMD_OK;
}
#endif

#if defined (CONFIG_OP_ALMAJD)
static	int	local_cmd_IrSetRGCode(void *szArgList)
{
	char 		*aucArg=NULL;
	HUINT16 	usNewRegion = 0;
	HUINT16 	usOldRegion = 0;
	HBOOL		bHex = FALSE;
	HINT32		nIndex = 0;

	aucArg = (char *)CMD_NextArg((char **)&szArgList);
	if( aucArg == NULL )
		return CMD_ERR;

	usNewRegion = 0;
	if (aucArg[0] == '0')
	{
		if (aucArg[1] == '\0')
		{
			nIndex = 1;
		}
		else if (aucArg[1] == 'x' || aucArg[1] == 'X')
		{
			bHex = TRUE;
			nIndex = 2;
		}
		else
		{
			return CMD_ERR;
		}
	}

	while (aucArg[nIndex] != '\0')
	{
		if (bHex == TRUE)
		{
			usNewRegion *= 16;
		}
		else
		{
			usNewRegion *= 10;
		}

		if (aucArg[nIndex] >= '0' && aucArg[nIndex] <= '9')
		{
			usNewRegion += aucArg[nIndex] - '0';
		}
		else if (bHex == TRUE && (aucArg[nIndex] >= 'a' && aucArg[nIndex] <= 'f'))
		{
			usNewRegion += aucArg[nIndex] - 'a' + 10;
		}
		else if (bHex == TRUE && (aucArg[nIndex] >= 'A' && aucArg[nIndex] <= 'F'))
		{
			usNewRegion += aucArg[nIndex] - 'A' + 10;
		}
		else
		{
			break;
		}

		nIndex++;
	}

	CAS_IR_NVR_ReadRegionCode (&usOldRegion, 2);
	HLIB_CMD_Printf ("Change region code : from 0x%04X to 0x%04X\n", usOldRegion, usNewRegion);
	CAS_IR_NVR_SaveRegionCode (&usNewRegion, 2);

	return CMD_OK;
}
#endif

void CMD_Register_MW_CAS_IRDRV_Debug(HxCMD_Handle_t hCasIrCmdHandle)
{


	HLIB_CMD_RegisterWord(hCasIrCmdHandle,
						local_cmd_IrDrvCommand,
						"ir",						/* keyword */
						"irdeto drv debug api",	/* help   */
						"ir svc | tp |sc |scell |mail |erase |read | dmx | etc | copy | err");	/* usage  */

	HLIB_CMD_RegisterWord(hCasIrCmdHandle,
						local_cmd_IrMVCommand,
						"irmv",
						"irdeto forced macrovision test",
						"0:original, 1:encryption, 2:forbidden");

#if defined(IRPLAT_FN_DDEMM_USER_PAYLOAD_FOR_DIGITURK)
	HLIB_CMD_RegisterWord(
						hCasIrCmdHandle,
						local_cmd_IrDrvCommandForDDEMM,
						"irddemm",
						"irdeto digiturk DDEMM test",
						"0:Virtual bill, 1:DM Parameter, 2:Feature Bitmap, 3: IPPV IP & Phone No., \n 			\
						4:Debit Limit Threshold, 5:IPPV Schedule, 6:Report Status 7:Download Type \n 			\
						8:PIN Reset, 9:Reset IRD, 10:Reset Order, 11:Rescan, 12:Rescan Home, 13:Reconnect \n 	\
						14:Reset Inst Pin, 15:Set OhterMode Enable, 16:Set OtherMode Disable \n					\
						17:Set CS Phone No. 18: Set PPV Phone No. 19:Set FTA Limit Enable, 20:Set FTA Limit Disable, \n					\
						21:Display Card No. 22: Colour Display, 23: Easy SMS, 24: Remote Tagging\n					\
						25: Remote Recording");
#endif
#if defined(IRPLAT_SYS_SAVE_SMARTCARD_NUMBER)
	HLIB_CMD_RegisterWord(
						hCasIrCmdHandle,
						local_cmd_IrSaveSCNum,
						"savescnum",
						"Save smart card number to EEPROM",
						"");
#endif
#if defined (CONFIG_OP_ALMAJD)
	HLIB_CMD_RegisterWord(
						hCasIrCmdHandle,
						local_cmd_IrSetRGCode,
						"setrgcode",
						"Set region code on EEPROM",
						"");
#endif
	return;
}


