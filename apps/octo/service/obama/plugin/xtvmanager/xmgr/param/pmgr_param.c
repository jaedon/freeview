/**
	@file     	pmgr_param.c
	@brief

	Description:  			\n
	Module: 					 					\n
	Remarks : 										\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#include <bus.h>
#include <mgr_common.h>
#include <mgr_appmgr.h>
#include <mgr_param.h>
#include <pmgr_param.h>
#include "./local_include/_xmgr_param.h"

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/
HERROR pmgr_param_Init (void)
{
	return xmgr_param_Init_Base ();
}

HERROR pmgr_param_SetFactoryDefault (DxFactoryDefault_type_e eDefaultType)
{
	return xmgr_param_SetFactoryDefault_Base (eDefaultType);
}

HERROR pmgr_param_GetUpdatedList(HUINT8 *pzUpdatedList)
{

#if defined(CONFIG_PROD_IR4000HD)
	 return xmgr_param_GetUpdatedList_IR4000HD(pzUpdatedList);
#else
	 return xmgr_param_GetUpdatedList_Base(pzUpdatedList);
#endif
}
/*********************** End of File ******************************/
