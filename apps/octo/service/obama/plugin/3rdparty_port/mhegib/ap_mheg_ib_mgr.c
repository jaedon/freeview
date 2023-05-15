/**
	@file     ap_mheg_ib_mgr.c
	@brief

	Description:

	Module: AP/MHEG			 			\n

	Copyright (c) 2012 HUMAX Co., Ltd.		\n
	All rights reserved.						\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#include <bus.h>
#include <hlib.h>

#include <svc_pipe.h>
#include <svc_cas.h>

#include <mgr_common.h>
#include "ci_ctrl.h"
//#include <pmgr_3rdparty.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/







/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/



/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/



/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/******************* Static Function Prototypes  *************************/
/*******************************************************************/



/*******************************************************************/
/********************	Application Proc	*************************/
/*******************************************************************/

#define _________PROTO_TYPE__________


#define _________LOCAL_FUNC__________


#define _________GLOBAL_FUNC_________


static BUS_Result_t	local_MhegIbManager_Proc(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	//HUINT32			ulActionId = SVC_PIPE_GetActionId(hAction);

	switch (message)
	{
		case eMEVT_BUS_CREATE :
			HxLOG_Info(" eMEVT_BUS_CREATE\n");
			break;

		case eMEVT_BUS_DESTROY :
			HxLOG_Info(" MSG : eMEVT_BUS_DESTROY on App Mheg Manager \n");
			break;

		default:
			break;

	}

	return BUS_ProcessMessageDefault(message, hAction, p1, p2, p3);
}

HERROR XMGR_MhegIbManager_Start(void)
{
	BUS_Result_t	result = ERR_BUS_NO_ERROR;

	HxLOG_Trace();

	result = BUS_MGR_Create("AP_MhegIbManager_Proc", APP_MHEG_MGR_PRIORITY, local_MhegIbManager_Proc, 0, 0, 0, 0);
	if(result == ERR_BUS_NO_ERROR)
	{
		HxLOG_Info(" Start Mheg IB Manger !!! \n");
	}
	else
	{
		HxLOG_Info(" Fail to Start Mheg IB Manger : alreay been here !!! \n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR XMGR_MhegIbManager_RequestFileOrData(	HUINT16	usSessionId,
														HUINT32	nReqType,
														HUINT32	ulReqNameLen,
														HUINT8	*paucReqName)
{
	HERROR									hErr;
	SvcCas_CtrlParam_t							stParam;
	CI_RequestFileOrDataInParam_t				stInParam;

	HxLOG_Trace();

	if(paucReqName == NULL)
	{
		HxLOG_Error(" arg error, paucReqName is NULL\n");
		return ERR_FAIL;
	}

	stParam.ulControlType		= eCACTRL_CI_ReqFileOrData;
	stInParam.usSessionId		= usSessionId;
	stInParam.enReqType		= (CICtrlAmmiRequestType_t)nReqType;
	stInParam.pucReqName		= paucReqName;
	stInParam.ulReqNameLen	= ulReqNameLen;

	stParam.pvIn = (void *)&stInParam;
	hErr = SVC_CAS_CtrlReq(eDxCAS_GROUPID_CAM, eCACTRL_CI_ReqFileOrData, &stParam);

	HxLOG_Trace();

	if(hErr != ERR_OK)
	{
		return ERR_FAIL;
	}

	return ERR_OK;
}


HERROR XMGR_MhegIbManager_ReqAppAbort(HUINT16 usSessionId, HUINT32 nAbortReqCode)
{
	HERROR					hErr;
	SvcCas_CtrlParam_t			stParam;
	CI_RequestAppAbort_t		stInParam;

	stParam.ulControlType		= eCACTRL_CI_ReqAppAbort;
	stInParam.usSessionId		= usSessionId;
	stInParam.enAbortReqCode	= (CICtrlAmmiAbortRequestCode_t)nAbortReqCode;

	stParam.pvIn = (void *)&stInParam;

	HxLOG_Trace();

	hErr = SVC_CAS_CtrlReq(eDxCAS_GROUPID_CAM, eCACTRL_CI_ReqAppAbort, &stParam);
	if(hErr != ERR_OK)
	{
		return ERR_FAIL;
	}

	return ERR_OK;
}

/*******************  End of File ************************/
