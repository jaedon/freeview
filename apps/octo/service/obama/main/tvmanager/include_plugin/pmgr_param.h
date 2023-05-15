/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  pmgr_param.h
	@brief

	Description:			\n
	Module: 			 	\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	___PMGR_PARAM_H___
#define	___PMGR_PARAM_H___


/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#include <mgr_param.h>

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Plug-In Functions    ********************/
/*******************************************************************/

HERROR	pmgr_param_Init (void);
HERROR	pmgr_param_SetFactoryDefault(DxFactoryDefault_type_e eDefaultType);
HERROR 	pmgr_param_GetUpdatedList(HUINT8 *pzUpdatedList);

#endif /* !___PMGR_PARAM_H___ */
