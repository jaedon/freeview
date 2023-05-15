/**
	@file     casmgr_ap_null_api.c
	@brief    casmgr_ap_null_api.c (CAS Service)

	Description: \n
	Module: MW/CAS \n
	Remarks :\n

	Copyright (c) 2012 HUMAX Co., Ltd. \n
	All rights reserved. \n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>

#include <svc_cas.h>


#ifdef CONFIG_MW_CI
#include <ci.h>
#include <ci_mgr.h>
#include <pal_ci.h>
#endif

#ifdef CONFIG_MW_CI_PLUS
#include <ci_plus.h>
#endif

/********************************************************************
* Header Files :  헤더 파일은 가능한 .c 에서 include 할 것 !
********************************************************************/
#define ___01_Header_Files___


/********************************************************************
* Extern Variables / Function Prototypes
********************************************************************/
#define ___02_Extern_Vars_Func_Prototypes___


/********************************************************************
* Module Internal Definitions : #define / enum / struct / union
********************************************************************/
#define ___03_Module_Internal_Definitions___




/********************************************************************
* Module Internal Function Prototypes
********************************************************************/
#define ___04_Module_Internal_Func_Prototypes___


/********************************************************************
* Global Public Definitions : #define / enum / struct / union
********************************************************************/
#define ___03_Global_Public_Definitions___

STATIC PostMsgToMgrCb_t		s_fnPostMsgToMgr = NULL;
STATIC PostDataToMgrCb_t	s_fnPostDataToMgr = NULL;

#define _____PUBLIC_APIs_______________________________________________________________
HERROR SVC_CAS_Init(PostMsgToMgrCb_t fnPostMsg, PostDataToMgrCb_t fnPostData)
{
	s_fnPostMsgToMgr = fnPostMsg;
	s_fnPostDataToMgr = fnPostData;

	return ERR_OK;
}

HERROR SVC_CAS_StartService(Handle_t hAction, Handle_t hSvc, SvcCas_ActionType_e eActionType, SvcCas_StartData_t *pstCasStartData)
{
	if (s_fnPostMsgToMgr)
	{
		(void)(* s_fnPostMsgToMgr)(eSEVT_CAS_START, hAction, HANDLE_NULL, eDxCAS_GROUPID_NONE, 0);
	}

	return ERR_OK;
}

HERROR SVC_CAS_StopService(Handle_t hAction, Handle_t hNextSvc, ApiSyncMode_t ulSyncMode)
{
	if (s_fnPostMsgToMgr)
	{
		(void)(* s_fnPostMsgToMgr)(eSEVT_CAS_STOPPED, hAction, HANDLE_NULL, eDxCAS_GROUPID_NONE, 0);
	}

	return ERR_OK;
}

HERROR SVC_CAS_SetRecPid(Handle_t hAction, SvcCas_ActionType_e eActType, SvcCas_SetPidInfo_t *pstCaDscInfo)
{
	return ERR_OK;
}

HERROR SVC_CAS_SetDataPid(Handle_t hAction, SvcCas_SetPidInfo_t *pstCaDscInfo)
{
	return ERR_OK;
}

HERROR SVC_CAS_SetPid(
							Handle_t hAction,
							SvcCas_ActionType_e eActType,
							HUINT16 usVideoPid,
							HUINT16 usAudioPid,
							HUINT16 usAuxPid,
							HUINT16 usDolbyPid,
							HUINT16 usTeletextPid,
							HUINT16 usSubtitlePid
							)
{
	return ERR_OK;
}

HERROR SVC_CAS_GetChannelType(Handle_t hAction, SvcCas_ChannelType_e *pType)
{
	/* FTA 모델의 경우, Unknown으로 처리하는 경우 문제가 될 수 있어, default FTA처리, 20141103  */
	if(NULL != pType)
	{
		*pType = eCasChannel_FTA;
	}

	return ERR_OK;
}

HERROR SVC_CAS_SiRegisterTimeoutCallback(SvcCas_SiGetTimeOutCb_t fnSiTimeout)
{
	return ERR_OK;
}

/********************************************************************
* Global Public Function Prototypes
********************************************************************/

#if defined(CONFIG_DEBUG)
void SVC_CAS_PrintMmiList(HUINT16 usSlotId)
{
	return;
}

HERROR SVC_CAS_PrintCaSysId(Handle_t hAction)
{
	return ERR_OK;
}

void SVC_CAS_PrintInfoAll(void)
{
	return;
}

void SVC_CAS_PrintCaDeviceInfo(void)
{
	return;
}

void SVC_CAS_PrintCaInstTable(void)
{
	return;
}

void SVC_CAS_PrintCaInstTableById(HUINT32 ulCaInstanceId)
{
	return;
}

void SVC_CAS_PrintContextList(void)
{
	return;
}

void SVC_CAS_PrintContextByActionId(HUINT32 ulActionId)
{
	return;
}

void SVC_CAS_PrintCatInfo(HUINT32 ulActionId)
{
	return;
}

void SVC_CAS_PrintPmtInfo(HUINT32 ulActionId)
{
	return;
}

void SVC_CAS_PrintMmiStatus(void)
{
	return;
}

void SVC_CAS_PrintRawTable(void)
{
	return;
}


void MW_CAS_DBG_ReleaseSyncSema(void)
{
	return;
}
#endif

#define ___04_Global_Public_Func_Prototypes___

/*********************** End of File ******************************/
