#include <octo_common.h>

#include <pal_sc.h>

#include <db_svc.h> //DbSvc_TsInfo_t

#include <_svc_cas_sub_dev_api.h>

#include "vd_humax.h"
#include "ir_mgr.h"
#include "ir_main.h"

//#include "../include/_svc_cas_err.h"

#include "_svc_cas_mgr_main.h"

#include <util.h> //for api :get 12 bit."

#include <ir_ctrl.h>
/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/






/********************************************************************
*	External variables
********************************************************************/

/********************************************************************
*	External function prototypes
********************************************************************/
#if defined(CONFIG_MW_CAS_IRDETO_SECURE)
extern HERROR CAS_IR_SECURECHIP_SetVirtualCWMode(HUINT8 ulcwMode);
#endif

/********************************************************************
*	Global variables
********************************************************************/
// TODO: temporal type definition. jichoi.

//typedef Handle_t	CAI_HANDLE;
//CAI_HANDLE		g_hIrCai = (CAI_HANDLE)NULL;

//TODO: definitions shall be fixed.
STATIC IR_ScInst_t 	s_IrScInst; //[HUMAX_VD_SC_SLOT_COUNT];
//STATIC IR_Context_t 	s_aIrContext[IR_HUMAX_MAX_SERVICE];
static SvcCas_Context_t			s_astCasIrContext[CAS_MAX_NUM_OF_ACTION];


/********************************************************************
*	Static variables
********************************************************************/


/********************************************************************
*	Static functions Prototypes
*	The following functions will be used as inner functions
********************************************************************/
static HERROR local_cas_ir_Mgr_InitModule(void);
static void local_cas_ir_Mgr_StartService(Handle_t hAction, Handle_t hSvc, SvcCas_ActionType_e eActType);// Action Type 전달 해준다.
static void local_cas_ir_Mgr_StopService(Handle_t hAction, Handle_t hNextSvc);
#if defined (CONNECT_TO_AVAILABLE_INST_WHEN_DISCONNECT_ORG_INST)	// ewlee 20110211
static void local_cas_ir_Mgr_IamSelected(Handle_t hAction, HUINT32 ulCasInstanceId, HBOOL bConnect, HBOOL bCloseSvc);
#else
static void local_cas_ir_Mgr_IamSelected(Handle_t hAction, HUINT32 ulCasInstanceId, HBOOL bConnect);
#endif
static void local_cas_ir_Mgr_SetPID(Handle_t hAction, SvcCas_SetPidInfo_t *pstCaDscInfo);
#if defined(IRPLAT_SYS_USING_TS_API)
static void local_cas_ir_Mgr_StartTS(Handle_t hAction, HUINT16 tsId, HUINT16 onId);
static void local_cas_ir_Mgr_StopTS(Handle_t hAction);
#endif
static void local_cas_ir_Mgr_UpdatePmt(Handle_t hAction, HUINT8 *pucPmt);
static void local_cas_ir_Mgr_UpdateCat(Handle_t hAction, HUINT16 usUniqueId, HUINT8 *pucCat);
#if defined(IRPLAT_FN_PROCESS_SDT)
static void local_cas_ir_Mgr_UpdateSdt(Handle_t hAction, HUINT16 usUniqueId, HUINT8 *pucSdt);
#endif
#if defined(CONFIG_MW_CAS_IRDETO_SOL)
static void local_cas_ir_Mgr_UpdateNit(Handle_t hAction, HUINT16 usUniqueId, HUINT8 *pucNit);
static void local_cas_ir_Mgr_UpdateBat(Handle_t hAction, HUINT16 usUniqueId, HUINT8 *pucBat);
#endif


#define ___LOCAL_FUNCS_FOR_ETC___


HERROR xsvc_cas_IrMgrInit(void)
{
#if defined(CONFIG_MW_CAS_IRDETO_SECURE)
	HINT32	nItemValue;
	HERROR	hErr = ERR_OK;

	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_VIRTUAL_SECURECW_SETTING, (HUINT32 *)&nItemValue, 0);

	if(hErr != ERR_OK)
	{
		HxLOG_Info("Get eDB_PARAM_ITEM_VIRTUAL_SECURECW_SETTING fail! \n");
	}
	CAS_IR_SECURECHIP_SetVirtualCWMode(nItemValue);

	HxLOG_Info("\n\t DB Virtual CW (%d) \n\n",nItemValue);
#endif

	HxLOG_Info("\n");

	//step1. CA TASK Init
	HxSTD_memset(&s_IrScInst, 0, sizeof(IR_ScInst_t) );//* HUMAX_VD_SC_SLOT_COUNT);

	/*step2 : IR DRV init */
	/*Create CA TASK and SoftCell Task */
	CAS_IR_DRV_Init();

	//step3. Session manager Init
	local_cas_ir_Mgr_InitModule();

	//step 4. register ctrl list
	CA_IR_RegisterCtrlList();

#if (CONFIG_MW_CAS_NUM_EMBEDDED > 1)
	// multi cas의 경우는 아래 noti를 보내면 안됨.
#else
	CAS_IR_MGR_Notify_InstanceChanged(0, NULL, 0, eCAS_SCM_FLAG_DISABLE);
#endif
	//

#if 0//defined(CONFIG_MW_CAS_IRDETO_SECURE)
	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_VIRTUAL_SECURECW_SETTING, (HUINT32 *)&nItemValue, 0);
	if(hErr != ERR_OK)	HxLOG_Critical("\n\n");

	CAS_IR_SECURECHIP_SetVirtualCWMode(nItemValue);
#endif

	return ERR_OK;
}

static HERROR local_cas_ir_Mgr_InitModule(void)
{
	HERROR nResult = ERR_FAIL;
	SvcCas_CbFuncList_t cbList;
	Handle_t hCasDevice = HANDLE_NULL;

	HxLOG_Info("\r\n");

	HxSTD_memset(&cbList, 0x00, sizeof(SvcCas_CbFuncList_t));
	cbList.pfnCbStartSession = local_cas_ir_Mgr_StartService;
	cbList.pfnCbStopSession = local_cas_ir_Mgr_StopService;
//	cbList.pfnCbPauseSession = NULL; // TODO: shko 고쳐야함
//	cbList.pfnCbResumeSession = NULL;
	cbList.pfnCbSetDSC = local_cas_ir_Mgr_SetPID;
	cbList.pfnCbInstSelected = local_cas_ir_Mgr_IamSelected;

	cbList.pfnCbUpdatePmt = local_cas_ir_Mgr_UpdatePmt;
	cbList.pfnCbUpdateCat = local_cas_ir_Mgr_UpdateCat;
#if defined(IRPLAT_FN_PROCESS_SDT)
	cbList.pfnCbUpdateSdt = local_cas_ir_Mgr_UpdateSdt;
#endif
#if defined(CONFIG_MW_CAS_IRDETO_SOL)
	cbList.pfnCbUpdateNit = local_cas_ir_Mgr_UpdateNit;
	cbList.pfnCbUpdateBat = local_cas_ir_Mgr_UpdateBat;
#endif
// TODO: shko 고쳐야함
	nResult = svc_cas_MgrSubRegisterCb(eCasInst_Embedded, eDxCAS_GROUPID_IR, 0, &cbList, 1, &hCasDevice);
	if (nResult != ERR_OK)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	s_IrScInst.hCasDevice = hCasDevice; // Instance Table Index
	HxLOG_Print("hCasDevice (0x%X)\n", hCasDevice);

	return ERR_OK;
}



static SvcCas_Context_t * local_cas_ir_Mgr_GetContextByAction(Handle_t hAction)
{
	HUINT32 ulActionId = 0;

	if (hAction == HANDLE_NULL)
	{
		HxLOG_Error("handle is null. \n");
		return (SvcCas_Context_t *)NULL;
	}

	ulActionId = svc_cas_SubSvcRmGetActionId(hAction);
	if(ulActionId >= CAS_MAX_NUM_OF_ACTION)
	{
		HxLOG_Error("action id is wrong. \n");
		return (SvcCas_Context_t *)NULL;
	}

	return &s_astCasIrContext[ulActionId];

}


#define ___LOCAL_SESSION_MANAGE_________


static void local_cas_ir_Mgr_GenerateIrSessionHandle(Handle_t in_hAction, Handle_t *out_phIrSessionHandle)
{
	SvcCas_Context_t *pContext;

	/*It can be changed with pure Action ID */
	pContext = local_cas_ir_Mgr_GetContextByAction(in_hAction);

	if(pContext == NULL)
	{
		*out_phIrSessionHandle = (in_hAction & 0xFF000000);
		HxLOG_Critical("\n\n");
		return;
	}

	*out_phIrSessionHandle = (in_hAction & 0xFF000000) | GET_SERVICE_ID(pContext->hSvc) ;
}

static void local_cas_ir_Mgr_StartService(Handle_t hAction, Handle_t hSvc, SvcCas_ActionType_e eActType)	// Action Type 전달 해준다.
{
	CAS_CAI_IF_msgPID *pMsg;
	HUINT32		i;

	HxLOG_Info("hAction=0x%X, Svc=0x%X, eActType=%d)\n", hAction, hSvc, eActType);

	pMsg = (CAS_CAI_IF_msgPID *)OxCAS_Malloc(sizeof(CAS_CAI_IF_msgPID));
	if (pMsg == NULL)
	{
		HxLOG_Critical("\n\n");
		return;
	}

	local_cas_ir_Mgr_GenerateIrSessionHandle(hAction, &(pMsg->hIrSvc) );
	pMsg->hAct= hAction;
	pMsg->hSvc= hSvc;
	pMsg->eActType= eActType;

	for(i=0; i<eIR_COMPONENT_MAX; i++)
	{
		pMsg->usPID_list[i] = IRDRV_NULL_PID;
	}

#if defined(IR_INST_CHANGE_BY_CAS_WORK_WELL)
	CAS_IR_STAUS_SetIrdetoDescState(TRUE);
#endif

	CAS_IR_SendMessage(eIR_MSG_CLASS_CAI, CASMGR_CAI_START_SERVICE, (void *)pMsg);

}

static void local_cas_ir_Mgr_StopService(Handle_t hAction, Handle_t hNextSvc)
{
	HUINT32 i;
	CAS_CAI_IF_msgPID *pPidMsg;
	CAS_CAI_IF_msgTS *pTsMsg;
	DbSvc_TsInfo_t stTsInfo;


	HxLOG_Info("hAction=0x%X, hNextSvc=0x%X)\n", hAction, hNextSvc);

	pPidMsg = (CAS_CAI_IF_msgPID *)OxCAS_Malloc(sizeof(CAS_CAI_IF_msgPID));
	if (pPidMsg == NULL)
	{
		HxLOG_Critical("\n\n");
		return;
	}

 	local_cas_ir_Mgr_GenerateIrSessionHandle(hAction, &(pPidMsg->hIrSvc) );

	/*Stop TS */
	// TP 전환 혹은 service 완전 stop시 EMM filtering stop
	// 1.service 완전 stop (menu 진입, standby 진입 등)
	if( hNextSvc == (Handle_t) HANDLE_NULL)
	{
#if 0
		Handle_t hSvc;
		DbSvc_TsInfo_t stTsInfo;
		HUINT16			usTsUniqueId = 0xFFFF;
		if (MGR_ACTION_GetMasterSvcHandle(hAction, &hSvc) == ERR_OK)
		{
			if (DB_SVC_GetTsInfoByServiceHandle(hSvc, &stTsInfo) == ERR_OK)
			{
				usTsUniqueId = stTsInfo.usUniqueId;
			}
		}
		if (usTsUniqueId != 0xFFFF)
			pPidMsg->usTsUniqueId = usTsUniqueId;
		else
			pPidMsg->usTsUniqueId = INVALID_TS_UNIQUE_ID;
#endif
		pPidMsg->usTsUniqueId= INVALID_TS_UNIQUE_ID;
		pPidMsg->hSvc	= HANDLE_NULL;
	}
	else if (DB_SVC_GetTsInfoByServiceHandle(hNextSvc, &stTsInfo) == ERR_OK)
	{
		pPidMsg->usTsUniqueId = stTsInfo.usUniqueId;
		pPidMsg->hSvc	= HANDLE_NULL; /* no meaning */
	}
	else
	{
		// TODO: DB에 기술되지 않은 서비스, Ir_drv로 일단 넣는다.
		pPidMsg->usTsUniqueId= INVALID_TS_UNIQUE_ID;
		pPidMsg->hSvc	= HANDLE_NULL;
		// jichoi HxLOG_Critical("\n\n");
	}

	/*Stop Service */
	for(i=0; i<eIR_COMPONENT_MAX; i++)
	{
		pPidMsg->usPID_list[i] = IRDRV_NULL_PID;
	}

	CAS_IR_SendMessage(eIR_MSG_CLASS_CAI, CASMGR_CAI_STOP_SERVICE, (void *)pPidMsg);

	// tp management.
	pTsMsg = (CAS_CAI_IF_msgTS *)OxCAS_Malloc(sizeof(CAS_CAI_IF_msgTS));

	if( hNextSvc == (Handle_t) HANDLE_NULL)
	{
		pTsMsg->hAction = hAction;
		pTsMsg->usTsUniqueId = INVALID_TS_UNIQUE_ID;
	}
	else
	{
		if (DB_SVC_GetTsInfoByServiceHandle(hNextSvc, &stTsInfo) == ERR_OK)
		{
			pTsMsg->hAction = hAction;
			pTsMsg->usTsUniqueId = stTsInfo.usUniqueId;
		}
		else
		{
			HxLOG_Print("DB_SVC_GetTsInfoByServiceHandle, Cannot find TsInfo ");

			// TODO: 일단 현재의 TP를 cut 한다.
			pTsMsg->hAction = hAction;
			pTsMsg->usTsUniqueId = INVALID_TS_UNIQUE_ID;
		}
	}
	CAS_IR_SendMessage(eIR_MSG_CLASS_CAI, CASMGR_CAI_STOP_TS, (void *)pTsMsg);
}

#if defined (CONNECT_TO_AVAILABLE_INST_WHEN_DISCONNECT_ORG_INST)	// ewlee 20110211
static void local_cas_ir_Mgr_IamSelected(Handle_t hAction, HUINT32 ulCasInstanceId, HBOOL bConnect, HBOOL bCloseSvc)
#else
static void local_cas_ir_Mgr_IamSelected(Handle_t hAction, HUINT32 ulCasInstanceId, HBOOL bConnect)
#endif
{
	CAS_CAI_IF_msgCAINST_SELECT	*pMsg;

	HxLOG_Info("hAction=0x%X, ulCasInstanceId=0x%X, bConnect=%d)\n", hAction, ulCasInstanceId, bConnect);

	pMsg = (CAS_CAI_IF_msgCAINST_SELECT *)OxCAS_Malloc(sizeof(CAS_CAI_IF_msgCAINST_SELECT));
	if (pMsg == NULL)
	{
		HxLOG_Critical("\n\n");
		return;
	}

  	local_cas_ir_Mgr_GenerateIrSessionHandle(hAction, &(pMsg->hIrSvc) );

	pMsg->ulCasInstanceId = ulCasInstanceId;
	pMsg->IsSelect = bConnect;
	#if defined (CONNECT_TO_AVAILABLE_INST_WHEN_DISCONNECT_ORG_INST)	// ewlee 20110211
	pMsg->CloseFlag = bCloseSvc;
	#else
	pMsg->CloseFlag = FALSE;
	#endif

	CAS_IR_SendMessage(eIR_MSG_CLASS_CAI, CASMGR_CAINST_SELECTDESELECT, (void *)pMsg);

}

static void local_cas_ir_Mgr_SetPID(Handle_t hAction, SvcCas_SetPidInfo_t *pstCaDscInfo)
{
	HUINT8	i;
	CAS_CAI_IF_msgPIDList *pMsg;

	HxLOG_Info("hAction=0x%X, pstCaDscInfo=0x%X\n", hAction, (HUINT32) pstCaDscInfo );

#if defined(CONFIG_DEBUG)
	HxLOG_Print("pstCaDscInfo->ulPidNum = %d\n", pstCaDscInfo->ulPidNum);

	for(i=0; i<pstCaDscInfo->ulPidNum; i++)
	{
		HxLOG_Print("ES type= %d, PID= 0x%X\n", pstCaDscInfo->stPidInfo[i].esType, pstCaDscInfo->stPidInfo[i].usPid);
	}
#endif

	pMsg = (CAS_CAI_IF_msgPIDList *)OxCAS_Malloc(sizeof(CAS_CAI_IF_msgPIDList));
	if (pMsg == NULL)
	{
		HxLOG_Critical("\n\n");
		return;
	}

  	local_cas_ir_Mgr_GenerateIrSessionHandle(hAction, &(pMsg->hIrSvc) );

	for( i = 0; i < IR_SVC_MAX_PID; i++ )
	{
		pMsg->astPidInfo[i].ucType = eEsType_None;
		pMsg->astPidInfo[i].usPID = IRDRV_NULL_PID;
	}

	pMsg->ulPidInfoCnt = pstCaDscInfo->ulPidNum;

	for( i=0; i<pstCaDscInfo->ulPidNum ; i++)
	{
		pMsg->astPidInfo[i].usPID = pstCaDscInfo->stPidInfo[i].usPid;
		pMsg->astPidInfo[i].ucType= pstCaDscInfo->stPidInfo[i].esType;
	}

	CAS_IR_SendMessage(eIR_MSG_CLASS_CAI, CASMGR_CAI_SET_PID, (void *)pMsg);

}

#if defined(IRPLAT_SYS_USING_TS_API)
static void local_cas_ir_Mgr_StartTS(Handle_t hAction, HUINT16 tsId, HUINT16 onId)
{
	CAS_CAI_IF_msgTS 	*pMsg;
	Handle_t			hPlay;
	HUINT32				result;

	HxLOG_Info("hAction=0x%X, tsId= 0x%04X, onId=0x%04X\n", hAction, tsId, onId) );

}

static void local_cas_ir_Mgr_StopTS(Handle_t hAction)
{
	CAS_CAI_IF_msgTS *pMsg;

	HxLOG_Info("hAction=0x%X\n", hAction) );

	pMsg = (CAS_CAI_IF_msgTS *)OxCAS_Malloc(sizeof(CAS_CAI_IF_msgTS));
	if (pMsg == NULL)
	{
		HxLOG_Print("[IR_CAI_StopTS] memory allocation failed !!\n");
		return;
	}

	pMsg->hAction = hAction;

	CAS_IR_SendMessage(eIR_MSG_CLASS_CAI, CASMGR_CAI_STOP_TS, (void *)pMsg);

}
#endif

static void local_cas_ir_Mgr_UpdatePmt(Handle_t hAction, HUINT8 *pucPmt)
{
	CAS_CAI_IF_msgPMT *pMsg;
	HUINT16			usPmtLen;

	usPmtLen = get12bit(pucPmt + 1) + 3;

	HxLOG_Info("hAction=0x%X, pucPmt= 0x%X, usPmtLen=%d\n", hAction, (HUINT32) pucPmt, usPmtLen);

	pMsg = (CAS_CAI_IF_msgPMT *)OxCAS_Malloc(sizeof(CAS_CAI_IF_msgPMT));
	if (pMsg == NULL)
	{
		HxLOG_Critical("\n\n");
		return;
	}

 	local_cas_ir_Mgr_GenerateIrSessionHandle(hAction, &(pMsg->hIrSvc) );

	pMsg->usLength = usPmtLen;

	// Message받아서 처리하는 부분에서 FREE하기.. !!!!
	pMsg->pRawData = (HUINT8 *)OxCAS_Malloc(usPmtLen);
	if (pMsg->pRawData == NULL)
	{
		HxLOG_Critical("\n\n");
		OxCAS_Free(pMsg);
		pMsg = NULL;
		return;
	}
	else
	{
		HxSTD_memcpy(pMsg->pRawData, pucPmt, usPmtLen);
		CAS_IR_SendMessage(eIR_MSG_CLASS_CAI, CASMGR_SI_PMT_UPDATE, (void *)pMsg);
	}
}
static void local_cas_ir_Mgr_UpdateCat(Handle_t hAction, HUINT16 usUniqueId, HUINT8 *pucCat)
{
	CAS_CAI_IF_msgCAT *pMsg;
	HUINT16 	usCatLen;

	usCatLen = get12bit(pucCat + 1) + 3;

	HxLOG_Info("hAction=0x%X, usUniqueId= 0x%X,pucCat=%X\n", hAction, usUniqueId,(HUINT32) pucCat );

	pMsg = (CAS_CAI_IF_msgCAT *)OxCAS_Malloc(sizeof(CAS_CAI_IF_msgCAT));
	if (pMsg == NULL)
	{
		HxLOG_Critical("\n\n");
		return;
	}

	pMsg->hAction= hAction;
	pMsg->usTsUniqueId = usUniqueId;
	pMsg->usLength = usCatLen;

	// Message받아서 처리하는 부분에서 FREE하기.. !!!!
	pMsg->pRawData = (HUINT8 *)OxCAS_Malloc(usCatLen);
	if (pMsg->pRawData == NULL)
	{
		HxLOG_Critical("\n\n");
		OxCAS_Free(pMsg);
		pMsg = NULL;
		return;
	}

	HxSTD_memcpy(pMsg->pRawData, pucCat, usCatLen);
	CAS_IR_SendMessage(eIR_MSG_CLASS_CAI, CASMGR_SI_CAT_UPDATE, (void *)pMsg);

}

#if defined(IRPLAT_FN_PROCESS_SDT)
static void local_cas_ir_Mgr_UpdateSdt(Handle_t hAction, HUINT16 usUniqueId, HUINT8 *pucSdt)
{
	CAS_CAI_IF_msgSDT *pMsg;
	HUINT16 	usSdtLen;

	usSdtLen = get12bit(pucSdt + 1) + 3;

	HxLOG_Info("hAction=0x%X, usUniqueId= 0x%X,pucSdt=0x%X\n", hAction, usUniqueId,(HUINT32) pucSdt ) );

	pMsg = (CAS_CAI_IF_msgSDT *)OxCAS_Malloc(sizeof(CAS_CAI_IF_msgSDT));
	if (pMsg == NULL)
	{
		HxLOG_Critical("\n\n");
		return;
	}

 	local_cas_ir_Mgr_GenerateIrSessionHandle(hAction, &(pMsg->hIrSvc) );
	pMsg->hAction= hAction;
	pMsg->usTsUniqueId = usUniqueId;
	pMsg->usLength = usSdtLen;

	HxLOG_Print("usSdtLen = %d\n", usSdtLen) );

	// Message받아서 처리하는 부분에서 FREE하기.. !!!!
	pMsg->pRawData = (HUINT8 *)OxCAS_Malloc(usSdtLen);
	if (pMsg->pRawData == NULL)
	{
		HxLOG_Critical("\n\n");
		OxCAS_Free(pMsg);
		pMsg = NULL;
		return;
	}

	HxSTD_memcpy(pMsg->pRawData, pucSdt, usSdtLen);
	CAS_IR_SendMessage(eIR_MSG_CLASS_CAI, CASMGR_SI_SDT_UPDATE, (void *)pMsg);

}
#endif

#if defined(CONFIG_MW_CAS_IRDETO_SOL)
/********************************************************************
*	function name :
*	input : void(None)
*	output : None
*	description :
********************************************************************/
static void local_cas_ir_Mgr_UpdateNit(Handle_t hAction, HUINT16 usUniqueId, HUINT8 *pucNit)
{
	CAS_CAI_IF_msgNIT *pMsg;
	HUINT16 	usNitLen;
#if defined(CAS_SUPPORT_NIT_MULTI)
	HUINT8				ucLastSecNum;
	HINT32				i;
	HUINT8				*ucTemp;
	HUINT32 			ulTotalLen = 0;

	ucTemp			= pucNit;

	ucLastSecNum	= get8bit(ucTemp + 7);
	ucLastSecNum++;

	for(i = 0; i < ucLastSecNum; i++)
	{
		usNitLen = get12bit(ucTemp + 1) + 3;
		ucTemp += usNitLen;
		ulTotalLen += usNitLen;
	}

	usNitLen = ulTotalLen;
#else
	usNitLen = get12bit(pucNit + 1) + 3;
#endif

	HxLOG_Info("hAction=0x%X, usUniqueId= 0x%X,pucNit=%X\n", hAction, usUniqueId,(HUINT32) pucNit ) );

	pMsg = (CAS_CAI_IF_msgNIT *)OxCAS_Malloc(sizeof(CAS_CAI_IF_msgNIT));
	if (pMsg == NULL)
	{
		HxLOG_Critical("\n\n");
		return;
	}

	pMsg->hAction= hAction;
	pMsg->usTsUniqueId = usUniqueId;
	pMsg->usLength = usNitLen;

	// Message받아서 처리하는 부분에서 FREE하기.. !!!!
	pMsg->pRawData = (HUINT8 *)OxCAS_Malloc(usNitLen);
	if (pMsg->pRawData == NULL)
	{
		HxLOG_Critical("\n\n");
		OxCAS_Free(pMsg);
		pMsg = NULL;
		return;
	}

	HxSTD_memcpy(pMsg->pRawData, pucNit, usNitLen);
	CAS_IR_SendMessage(eIR_MSG_CLASS_CAI, CASMGR_SI_NIT_UPDATE, (void *)pMsg);

}

/********************************************************************
*	function name :
*	input : void(None)
*	output : None
*	description :
********************************************************************/
static void local_cas_ir_Mgr_UpdateBat(Handle_t hAction, HUINT16 usUniqueId, HUINT8 *pucBat)
{
	CAS_CAI_IF_msgBAT *pMsg;
	HUINT16 	usBatLen;

	usBatLen = get12bit(pucBat + 1) + 3;

	HxLOG_Info("hAction=0x%X, usUniqueId= 0x%X,pucBat=%X\n", hAction, usUniqueId,(HUINT32) pucBat ) );

	pMsg = (CAS_CAI_IF_msgBAT *)OxCAS_Malloc(sizeof(CAS_CAI_IF_msgBAT));
	if (pMsg == NULL)
	{
		HxLOG_Critical("\n\n");
		return;
	}

	pMsg->hAction= hAction;
	pMsg->usTsUniqueId = usUniqueId;
	pMsg->usLength = usBatLen;

	// Message받아서 처리하는 부분에서 FREE하기.. !!!!
	pMsg->pRawData = (HUINT8 *)OxCAS_Malloc(usBatLen);
	if (pMsg->pRawData == NULL)
	{
		HxLOG_Critical("\n\n");
		OxCAS_Free(pMsg);
		pMsg = NULL;
		return;
	}

	HxSTD_memcpy(pMsg->pRawData, pucBat, usBatLen);
	CAS_IR_SendMessage(eIR_MSG_CLASS_CAI, CASMGR_SI_BAT_UPDATE, (void *)pMsg);

}
#endif

#define ___API_FOR_IR_DRV______________________________________________________________________________

/********************************************************************
*	function name :	 CAS_IR_MGR_Notify_InstanceChanged
*	input : void(None)
*	output : None
*	description : blah blah blah
********************************************************************/
HUINT32 CAS_IR_MGR_Notify_InstanceChanged(HUINT16 usSlotId, HUINT16 *pusCaSysId, HUINT8 ucNumCaSysId, HBOOL bAvailable)
{
#if (CONFIG_MW_CAS_NUM_EMBEDDED > 1)
	HUINT32 i;
#endif
	HUINT32	ret;

	HxLOG_Info("ucNumCaSysId=0x%d\n", ucNumCaSysId);

#if (CONFIG_MW_CAS_NUM_EMBEDDED <= 1)
	s_IrScInst.usSlotId= usSlotId;
  #if defined(IR_SC_IN_OUT_NOTI_TO_CASMGR)
	s_IrScInst.usNumCaSysId= ucNumCaSysId;	/* Cas Mgr에서는 이 값이 0이면 sc 빠진 것으로 처리한다. 이를 고려하기 위함. */
  #else
	s_IrScInst.usNumCaSysId= 1;
  #endif  // #if defined(IR_SC_IN_OUT_NOTI_TO_CASMGR)
	s_IrScInst.ausCaSysId[0] = 0x600;
	bAvailable = TRUE;
#else
	s_IrScInst.usSlotId= usSlotId;
	s_IrScInst.usNumCaSysId= ucNumCaSysId;

	if(pusCaSysId == NULL || ucNumCaSysId == 0)
	{//card removed
		for(i=0; i<MAX_SC_SECTOR_COUNT; i++ )
		{
			s_IrScInst.ausCaSysId[i] = 0 ;
		}
//		bAvailable = FALSE;
	}
	else	//card inserted
	{
		for(i=0; i<ucNumCaSysId; i++ )
		{
			// TODO: CA instance  버그 수정 전 가지 마스킹 사용 할 것
			#if 0 //good
			s_IrScInst.ausCaSysId[i] = pusCaSysId[i] ;
			#else// bad
			s_IrScInst.ausCaSysId[i] = pusCaSysId[i] & 0xFF00 ;
			#endif
		}
//		bAvailable = TRUE;
	}
#endif
	ret = svc_cas_MgrSubUpdateCasInfo(	s_IrScInst.hCasDevice,
								eDxCAS_GROUPID_IR,
								s_IrScInst.usSlotId,
								s_IrScInst.ausCaSysId,
								s_IrScInst.usNumCaSysId,
								IR_HUMAX_MAX_SERVICE,
								 bAvailable
								);

	if(ret != ERR_OK)
	{
		HxLOG_Print("\tERROR CAS_IR_MGR_Notify_InstanceChanged ret=0x%x\n", ret);
		return ret;
	}

	return ERR_OK;
}


Handle_t CAS_IR_MGR_GetCasInstance(void)
{
	return s_IrScInst.hCasDevice;
}


SvcCas_Context_t * CAS_IR_MGR_GetContextByAction(Handle_t hAction)
{
	return local_cas_ir_Mgr_GetContextByAction(hAction);
}


