/*******************************************************************
	File Description
********************************************************************/
/**
	@file	   	ir_main.c \n
	@brief	main \n

	Description:   \n
	Module: MW/CAS/ IRDETO \n

	Copyright (c) 2010 HUMAX Co., Ltd. \n
	All rights reserved. \n
*/

#include <pal_sc.h>

#include <_svc_cas_sub_dev_api.h>

#include "vd_humax.h"

#include "ir_svc.h"
#include "ir_msg.h"
#include "ir_main.h"
#include "os_drv.h"
#include "ir_si.h"
#include "ir_msg_scell.h"
#include "ir_status.h"



/********************************************************************
*	Definitions (for this source file)
********************************************************************/

#if defined(CONFIG_DEBUG)
#include "ir_debug.h"
#define DEBUG_SVC_STATUS

#endif






#if defined(IRPLAT_SYS_PROCESS_MUTEX)
static unsigned long	s_IrProcessMutex;
#define IR_MUTEX_GET()			VK_SEM_Get(s_IrProcessMutex)
#define IR_MUTEX_RELEASE()		VK_SEM_Release(s_IrProcessMutex)
#else
#define IR_MUTEX_GET()
#define IR_MUTEX_RELEASE()
#endif

#define IR_LDR_INIT_RETRY		10

/********************************************************************
*	Static variables
********************************************************************/
static unsigned long s_ulCaTaskId, s_ulCaTaskQId;


void CAS_IR_DRV_ProcessMsg(HMSG *msg);

/********************************************************************
*	Extern variables
********************************************************************/
extern	IR_SERVICE_INFO		g_IrSvcInfo[];
extern	IR_TP_INFO			g_IrTpInfo[];
extern	IR_DRV_SC_INFO		g_IrDrvScInfo[];

extern 	HUINT8 s_bIsNeedToGetPurchasedEventList;

extern	ia_result HUMAX_ResetCard( vd_ref_id wRefId );
extern	HUINT32 HUMAX_VD_PersistentStorage_DelayWrite(vd_ref_id wRefId, HUINT16 usStart, HUINT16 usLen, HUINT8 *pucData);

#if	defined(IRPLAT_SYS_PVR)
extern HUINT32 HUMAX_VD_ExtStorage_DelayWrite(vd_ref_id wRefId, ia_word32 ulStart, ia_word16 ulLen, void *pucData);
#endif

#ifdef	IRPLAT_SYS_SC_EXTRACT_MSG
extern	void	HUMAX_ExtractCard(HUINT8 ucSlotId);
#endif

/******************************************************************************************************/
//      Function : void local_cas_ir_CaTask() -> main task in CAS
/******************************************************************************************************/
static void local_cas_ir_CaTask( void *pParam)
{
    HUINT32 result;
    HMSG	msg;

	HxLOG_Info("\n");

	result = CAS_IR_SoftCellInit();
	if(HIR_OK != result)
	{
		HxLOG_Critical("\n\n");
		while(1);
	}

	while (1)
	{
		result = VK_MSG_Receive( s_ulCaTaskQId, &msg, sizeof(HMSG) );
		if (result != 0)
		{
			HxLOG_Print("VK_MSG_Receive ERROR!\n");
			continue;
		}
		IR_MUTEX_GET();
		CAS_IR_DRV_ProcessMsg(&msg);
		IR_MUTEX_RELEASE();
	}
}


/******************************************************************************************************/
/******************************************************************************************************/
static	void local_cas_ir_Drv_ProcessMsgFromCAI(HMSG *msg)
{
	HxLOG_Info("\n");

	if( msg->ulParam == (HUINT32)NULL )
	{
		HxLOG_Error("ulParams' value has Null\n");
		return ;
	}

	switch(msg->usParam)
	{
	case CASMGR_CAI_START_SERVICE:
		CAS_IR_SVC_StartService((CAS_CAI_IF_msgPID *)(msg->ulParam));
		break;

	case CASMGR_CAI_STOP_SERVICE:
		CAS_IR_SVC_StopService((CAS_CAI_IF_msgPID *)(msg->ulParam));
		break;

	case CASMGR_SI_PMT_UPDATE:
		CAS_IR_SI_UpdatePMT((CAS_CAI_IF_msgPMT *)(msg->ulParam));
#if defined(CONFIG_MW_CAS_IRDETO_SOL)
		CAS_IR_SOL_UpdatePMT();
#endif
		break;

	case CASMGR_CAI_SET_PID:
		CAS_IR_SVC_SetPID((CAS_CAI_IF_msgPIDList *)(msg->ulParam));
		break;


#if defined(IRPLAT_SYS_USING_TS_API)
	case CASMGR_CAI_START_TS:
		CAS_IR_SVC_StartTS((CAS_CAI_IF_msgTS *)(msg->ulParam));
		break;
#endif

	case CASMGR_CAI_STOP_TS:
		CAS_IR_SVC_StopTS((CAS_CAI_IF_msgTS *)(msg->ulParam));
		break;

#if defined(IRPLAT_SYS_PVR)
	case CASMGR_IR_SETUP_RECORD:
		CAS_IR_PVR_SetupRecord((CAS_CAI_IF_msgPVR *)(msg->ulParam));
		break;

	case CASMGR_IR_SETUP_PLAYBACK:
		CAS_IR_PVR_SetupPlayback((CAS_CAI_IF_msgPVR *)(msg->ulParam));
		break;
#endif
	case CASMGR_SI_CAT_UPDATE:
		CAS_IR_SI_UpdateCAT((CAS_CAI_IF_msgCAT *)(msg->ulParam));
		break;

#if defined(IRPLAT_FN_PROCESS_SDT)
	case CASMGR_SI_SDT_UPDATE:
		CAS_IR_SI_UpdateSDT((CAS_CAI_IF_msgSDT *)(msg->ulParam));
		break;
#endif
	case CASMGR_CAINST_SELECTDESELECT:
	{
		CAS_CAI_IF_msgCAINST_SELECT		*pMsgSelect;

		pMsgSelect = (CAS_CAI_IF_msgCAINST_SELECT *)(msg->ulParam);

		if( 0 != pMsgSelect->IsSelect ) // Value 1 is selected.
		{
			CAS_IR_SVC_IamSelected(pMsgSelect );
		}
		else /* if pre-comp.def "IRPLAT_SYS_1CAS1SC" is declared, below condition must not be done */
		{
			CAS_IR_SVC_IamDeSelected(pMsgSelect);
		}
		break;
	}
#if defined(CONFIG_MW_CAS_IRDETO_SOL)
	case CASMGR_SI_NIT_UPDATE:
		CAS_IR_SI_UpdateNIT((CAS_CAI_IF_msgNIT *)(msg->ulParam));
		break;

	case CASMGR_SI_BAT_UPDATE:
		CAS_IR_SI_UpdateBAT((CAS_CAI_IF_msgBAT *)(msg->ulParam));
		break;
#endif

#if 0 //removed stuffs in octo system., in my opinion, followings are to be considered later..
	case CASMGR_CAI_SET_PIDLIST:
	case CASMGR_CAINST_CHANGED:
	case CASMGR_CAI_ONE_PID:
	case CASMGR_CAI_RES_CHANGED:
	case CASMGR_CAI_VIEWTYPE_CHANGED:
	case CASMGR_DD_AV_STATUS:
	case CASMGR_DD_TUNER_STATUS:
#endif

	default:
		HxLOG_Print("NOT Processed.(msg type = 0x%X)\n", msg->usParam);
		break;
	}
}


/******************************************************************************************************/
//      Function : void CAS_Init()
/******************************************************************************************************/
int		CAS_IR_DRV_Init(void)
{
	HUINT8 i=0;
	HUINT8 j=0;

	HxSTD_memset(g_IrSvcInfo, 0, sizeof(IR_SERVICE_INFO) * IR_HUMAX_MAX_SERVICE);
	HxSTD_memset(g_IrTpInfo,  0, sizeof(IR_TP_INFO) * IR_HUMAX_MAX_TP);


	for (i = 0; i < IR_HUMAX_MAX_TP; i++)
	{
		for (j = 0; j < CAS_MAX_NUM_OF_SC; j++)
		{
			g_IrTpInfo[i].hAction = HANDLE_NULL;
			g_IrTpInfo[i].usSCELL_SvcHandle[j] = INVALID_SOFTCELL_SVCHANDLE;
		}
	}

	for( i = 0; i < IR_HUMAX_MAX_SERVICE; i++ )
	{
		g_IrSvcInfo[i].hIrSvc = HANDLE_NULL;
		g_IrSvcInfo[i].hAction = HANDLE_NULL;
		g_IrSvcInfo[i].hSvc = HANDLE_NULL;
		g_IrSvcInfo[i].usSCELL_SvcHandle = INVALID_SOFTCELL_SVCHANDLE;
	}

#if !defined(CONFIG_PROD_HD5500T)
	/* 생산 중 Irdeto Key Read Error 발생하여 LOADER Init Retry code 추가함. */
	i = 0;
	while(ERR_OK != xsvc_cas_IrLoaderInit() )
	{
		HxLOG_Error("read loader info error!!\n");
		HxLOG_Critical("\n\n");

		VK_TASK_Sleep(300);

		if(i++ > IR_LDR_INIT_RETRY)
		{
			HxLOG_Error("\n\t ERROR ir loader init timeout!!\n\n");
			break;
		}
	}
#endif
	CAS_IR_MSG_DRV_Initialise();
	CAS_IR_CATASK_Init();

#if defined(CONFIG_MW_CAS_IRDETO_SOL)
	CAS_IR_SOL_initialize();
#endif

#if defined(IR_INST_CHANGE_BY_CAS_WORK_WELL)
	CAS_IR_STAUS_SetIrdetoDescState(TRUE);
#endif

	return 0;
}

/********************************************************************************
********************************************************************************/
HIR_ERR	CAS_IR_SoftCellInit(void)
{
	ia_result result = IA_SUCCESS;

	HxLOG_Info("\n");

	result = SCELL_Initialise();
	if( result != IA_SUCCESS )
	{
		HxLOG_Critical("\n\n");
		return HIR_NOT_OK;
	}

	HxLOG_Print("MSG_CLIENT_VERSION_QUERY\n");
	result = SCELL_Message(MSG_CLIENT_VERSION_QUERY, 0, 0, NULL);
	HxLOG_Print("MSG_COMPONENTS_VERSION_QUERY\n");
	result = SCELL_Message(MSG_COMPONENTS_VERSION_QUERY, 0, 0, NULL);

	if(result != IA_SUCCESS)
	{
		HxLOG_Critical("\n\n");
		return HIR_NOT_OK;
	}

	return HIR_OK;
}


/********************************************************************************
********************************************************************************/
void CAS_IR_CATASK_Init(void)
{

	HUINT32		result;

	HxLOG_Info("Irdeto manufactureer  ca task\n");

	result = VK_TASK_Create(local_cas_ir_CaTask, \
							HMXIR3_TASK_PRIORITY, \
							HMXIR3_TASK_STACK_SIZE, \
							"HMXIR3_CA_T",\
							NULL, \
							&s_ulCaTaskId, \
							0);
	if( result != VK_OK )
	{
		HxLOG_Error("create task fail \n");
		HxLOG_Critical("\n\n");
		return;
	}

	result = VK_MSG_Create(HMXIR3_TASK_MSGQ_SIZE, sizeof(HMSG), "HMXIR3_CA_Q", &s_ulCaTaskQId, VK_SUSPENDTYPE_FIFO );
	if ( result != VK_OK )
	{
		HxLOG_Error("create queue fail \n");
		HxLOG_Critical("\n\n");
		return;
	}

#if defined(IRPLAT_SYS_PROCESS_MUTEX)
	result = VK_SEM_Create(&s_IrProcessMutex, "s_IrProcessMutex", VK_SUSPENDTYPE_FIFO);
	if( result != VK_OK )
	{
		HxLOG_Print("ir create mutex failed. (0x%X)\n", result);
		HxLOG_Critical("\n\n");
		return ;
	}
#endif

	result = VK_TASK_Start( s_ulCaTaskId );
	if ( result != VK_OK )
	{
		HxLOG_Print("failure :start ca task\n");
		HxLOG_Critical("\n\n");
		return;
	}

	return;
}


/********************************************************************************
********************************************************************************/
void CAS_IR_SendMessage(HUINT16 msg_channel, HUINT16 msg_type, void* pMsg)
{
	HMSG		msg;

	HxLOG_Info(": CH(0x%X) : TYPE(0x%X)\n", msg_channel, msg_type );

	msg.usMsgClass	= msg_channel;
	msg.usParam		= msg_type;
	msg.ulParam		= (HUINT32)pMsg;

	VK_MSG_Send( s_ulCaTaskQId, &msg, sizeof(HMSG) );
}


/********************************************************************************
********************************************************************************/
void CAS_IR_DRV_ProcessMsg(HMSG *msg)
{
	CAS_IR_BASE_MSG 	*pMsgIR;
	HUINT32 			result;

	HxLOG_Info(" MsgClass (%d) MsgParam (%d) \n", msg->usMsgClass, msg->usParam);
#if defined(DEBUG_SVC_STATUS) && !defined(WIN32)
	CAS_IR_DBG_ParseMessage(msg);
#endif
	switch ( msg->usMsgClass )
	{
		// CHANNEL FROM CAI - start servcie/set PID.. 등.. mw core에서 MXE->CAI를 통해 넘어오는 message들..
		case eIR_MSG_CLASS_CAI:
			local_cas_ir_Drv_ProcessMsgFromCAI(msg);
			break;

		// CHANNEL FROM CAS - SoftCell에서 보내주는 msg나 CA TASK가 자체적으로 필요해서 사용하는 message들..
		case eIR_MSG_CLASS_CAS:
			if( msg->ulParam == (HUINT32)NULL )
				break;

			pMsgIR = (CAS_IR_BASE_MSG *)msg->ulParam;

			switch(msg->usParam)
			{
				case IR_CATASK_CARD_IN:
					HUMAX_ResetCard(pMsgIR->msg.SC.ReferenceID);
					// ewlee 20100914
					#if defined(CONFIG_MW_CAS_IRDETO_PPV)
					CAS_IR_DRV_SetFlagToGetPurchasedEventList(TRUE);
				//	CASMGR_UpdateCasEtcEvent(0, CAS_IR_MGR_GetCasInstance(), eDxCAS_GROUPID_IR, eSEVT_CAS_SC_INSERTED, pMsgIR->msg.SC.ucSlotID, eDxCAS_GROUPID_IR, 0);
					#endif
					break;

				case IR_CATASK_CARD_OUT:
#ifdef	IRPLAT_SYS_SC_EXTRACT_MSG
					HUMAX_ExtractCard(pMsgIR->msg.SC.ucSlotID);
#endif

#if defined(CONFIG_OP_GREECE_NOVA) || defined(CONFIG_MW_CAS_IRDETO_PPV)
					//1 To Do 반드시 여기에서 SC Removed msg를 보내야 하는지 다시 한번 확인하자!
					//CASMGR_UpdateCasEtcEvent(0, CAS_IR_MGR_GetCasInstance(), eDxCAS_GROUPID_IR, eSEVT_CAS_SC_REMOVED, pMsgIR->msg.SC.ucSlotID, eDxCAS_GROUPID_IR, 0);
					#endif
					break;

				case IR_CATASK_NVRAM_WRITE:
					result = HUMAX_VD_PersistentStorage_DelayWrite(pMsgIR->msg.NVRAM.RefId, (HUINT16)pMsgIR->msg.NVRAM.ulStart, (HUINT16)pMsgIR->msg.NVRAM.ulLen, pMsgIR->msg.NVRAM.pucData);
					if (result != HIR_OK)
					{
						HxLOG_Print("IR_CATASK_NVRAM_WRITE err occured.\n");
					}
					if( NULL != pMsgIR->msg.NVRAM.pucData )
					{
						OxCAS_Free(pMsgIR->msg.NVRAM.pucData);
					}

					break;

#if defined(IRPLAT_SYS_PVR)
				case IR_CATASK_FILE_WRITE:
					result = HUMAX_VD_ExtStorage_DelayWrite(pMsgIR->msg.FILE.RefId, pMsgIR->msg.FILE.ulStart, (ia_word16)pMsgIR->msg.FILE.ulLen, pMsgIR->msg.FILE.pucData);
					if (result == HIR_OK)
					{
						if( pMsgIR->msg.NVRAM.pucData )
						{
							OxCAS_Free(pMsgIR->msg.FILE.pucData);
						}
					}
					break;
#endif

				case IR_CATASK_FROM_SOFTCELL:
					CAS_IR_SoftcellMsgProc(pMsgIR);
					break;
			}

			break;

		default:
			break;
	}

	/* followings are added, 2008-08-11 */
	if((void*)(msg->ulParam) != NULL)
	{
		OxCAS_Free((void*)(msg->ulParam));
	}
}

