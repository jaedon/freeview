/**
@file				_svc_cas_mgr_debug.h
@brief				_svc_cas_mgr_debug.h (CAS Service)

Description:  	Cas Service Module에서 사용되는 Debug Information, Print, String 등을 하나로 모아서 관리한다. \n
				이렇게 함으로써 이후 엔지니어들이 쉽게 Debug 정보를 사용 및 관리 할 수 있도록 운영한다. \n
Module: MW/CAS \n
Remarks :\n

Copyright (c) 2008 HUMAX Co., Ltd. \n
All rights reserved. \n
*/

#ifndef	__SVC_CAS_MGR_DEBUG_H__
#define	__SVC_CAS_MGR_DEBUG_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>

#include <svc_cas.h>

#include <_svc_cas_mgr_common.h>
#include <_svc_cas_mgr_instance.h>

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/


/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/

HERROR svc_cas_DbgInitModule(void);

const HINT8 *svc_cas_DbgMakeStrActionType(SvcCas_ActionType_e cmd);
const HINT8 *svc_cas_DbgMakeStrChType(SvcCas_ChannelType_e eType);
const HINT8 *svc_cas_DbgMakeStrContextState(SvcCas_State_e eState);
const HINT8 *svc_cas_DbgMakeStrMmiType(SvcCas_MmiDataType_e eType);
const HINT8 *svc_cas_DbgMakeStrCasEvtType(SvcCas_EvtType_e eType);
const HINT8 *svc_cas_DbgMakeStrSiTblState(SvcCas_SiTableState_e eState);
const HINT8 *svc_cas_DbgMakeStrMatchLevel(SvcCas_MatchLevel_e eType);
const HINT8 *svc_cas_DbgMakeStrChType(SvcCas_ChannelType_e eType);
const HINT8 *svc_cas_DbgMakeStrCaiEvtType(SvcCas_DevEvtType_e eType);
const HINT8 *svc_cas_DbgMakeStrCaiEvtType(SvcCas_DevEvtType_e eType);
const HINT8 *svc_cas_DbgMakeStrCbNotiType(SvcCas_InstNotiType_e eType);
const HINT8 *svc_cas_DbgMakeStrCaiType(SvcCas_InstanceType_e eType);
const HINT8 *svc_cas_DbgMakeStrCamState(SvcCas_CamState_e eState);
const HINT8 *svc_cas_DbgMakeStrEsType(EsType_t eType);
const HINT8 *svc_cas_DbgMakeStrDescrambleState(SvcCas_DscState_e cmd);
const HINT8 *svc_cas_DbgMakeStrContextState(SvcCas_State_e eState);
const HINT8 *svc_cas_DbgMakeStrCaSysId(SvcCas_CaSysIdSpecifier_e cmd);
const HINT8 *svc_cas_DbgMakeStrCasCmd(SvcCas_Command_e cmd);
const HINT8 *svc_cas_DbgMakeStrCasId(DxCAS_GroupId_e cmd);
const HINT8 *svc_cas_DbgMakeStrCasMsgType(SvcCas_MsgType_e cmd);
const HINT8 *svc_cas_DbgMakeStrCasShowType(SvcCas_ShowType_e cmd);

HERROR svc_cas_DbgPrintPmtInfo(SvcCas_PmtInfo_t *pstPmtInfo);
#if 0
HERROR svc_cas_DbgPrintPmtInfoByActionId(HUINT32 ulActionId);
#endif

HERROR svc_cas_DbgPrintPmtElementInfo(SvcCas_PmtInfo_t *pstPmtInfo);

HERROR svc_cas_DbgPrintActionContextByActionId(HUINT32 ulActionId);
HERROR svc_cas_DbgPrintActionContextList(void);

HERROR svc_cas_DbgPrintCaInstTableById(HUINT32 ulCaInstanceId);
HERROR svc_cas_DbgPrintCaInstTable(void);

#if 0
void svc_cas_DbgPrintPidInfo(SvcCas_SetPidInfo_t *pstCaDscInfo);
#endif
void svc_cas_DbgPrintMmiStatus(SvcCas_MmiData_t *pData);

HERROR svc_cas_DbgPrintCatInfo(SvcCas_CatInfo_t *pstCatInfo);



#endif /* !__SVC_CAS_MGR_DEBUG_H__ */
