/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  _svc_cas_mgr_control.h		\n
	@brief	  cas module header file		\n

	Description: cas module. \n
	Module: MW/CAS									 					\n

	Copyright (c) 2012 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	__SVC_CAS_MGR_CONTROL_H__
#define	__SVC_CAS_MGR_CONTROL_H__

/********************************************************************
* Header Files :  헤더 파일은 가능한 .c 에서 include 할 것 !
********************************************************************/
#define ___01_Header_Files___
#include <octo_common.h>
#include <svc_cas.h>
//#include <_svc_cas_mgr_common.h>


/********************************************************************
* Extern Variables / Function Prototypes
********************************************************************/
#define ___02_Extern_Vars_Func_Prototypes___


/********************************************************************
* Global Public Definitions : #define / enum / struct / union
********************************************************************/
#define ___03_Global_Public_Definitions___


/********************************************************************
* Global Public Function Prototypes
********************************************************************/
#define ___04_Global_Public_Func_Prototypes___

HERROR svc_cas_CtrlInit(DxCAS_GroupId_e eCasId);
HERROR svc_cas_CtrlRegister(DxCAS_GroupId_e eCasId, HUINT32 ulNum, SvcCas_Ctrl_t *pstCasCtrlList);
HERROR svc_cas_CtrlCall(DxCAS_GroupId_e eCasId, HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
HERROR svc_cas_CtrlGet(DxCAS_GroupId_e eCasId, HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
HERROR svc_cas_CtrlSet(DxCAS_GroupId_e eCasId, HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
HERROR svc_cas_CtrlReq(DxCAS_GroupId_e eCasId, HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
HERROR svc_cas_CtrlNoti(DxCAS_GroupId_e eCasId, HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);


#endif /* !__SVC_CAS_MGR_CONTROL_H__ */
