/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  xmgr_spec.h
	@brief

	Description: main main procedure 관련 내용들을 분류한 모듈이다. \n
	Module:
	Remarks : 										\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	__XMGR_SPEC_H__
#define	__XMGR_SPEC_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
//#include <isosvc.h>
//#include <stb_params.h>
//#include <linkedlist.h>

/*******************************************************************/
// MW
/*******************************************************************/
//#include <svc_pipe.h>
//#include <svc_resources.h>

/*******************************************************************/
// DB
/*******************************************************************/
//#include <db_svc.h>


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
// CONFIG_OP_FREESAT		>>>
typedef enum
{
	eFreesatModeFreesat = 0,
	eFreesatModeCommonDvb
} XmgrFsatMode_e;

// CONFIG_OP_FREESAT		<<<

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

// CONFIG_OP_FREESAT		>>>
HERROR	XMGR_SPEC_GetOperationMode_Freesat (XmgrFsatMode_e *peMode);
HERROR	XMGR_SPEC_SetOperationMode_Freesat (XmgrFsatMode_e eMode);
#if 0
HERROR	XMGR_SPEC_GetNumOfFreesatModeSvcInDB (XsvcSi_SiModeFsat_e eFsatMode, HINT32 *pnCount);
HERROR	XMGR_SPEC_GetSvcAttr_Freesat (DbSvc_Attr_t *attr);
#endif
HERROR	XMGR_SPEC_ProcessAdultService_Freesat(HUINT32 ulFsatAdultGrpId);
// CONFIG_OP_FREESAT		<<<

#endif	/*	!__XMGR_SPEC_H__	*/
