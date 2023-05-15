/**
@file				_svc_cas_mgr_sc.h
@brief			_svc_cas_mgr_sc.h (CAS Service)

Description:  \n
Module: MW/CAS \n
Remarks :\n

Copyright (c) 2008 HUMAX Co., Ltd. \n
All rights reserved. \n
*/

#ifndef	__CASMGR_SC_H__
#define	__CASMGR_SC_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#include <svc_cas.h>

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

HERROR svc_cas_ScInitModule(void);
HERROR svc_cas_ScSendCmd2MgrTask(DxCAS_GroupId_e eGroupId, HUINT32 ulCommand, HUINT32 ulMsgKind, HUINT8* pDataToSend);


#endif /* !__CASMGR_SC_H__ */