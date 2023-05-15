/**
	@file     _xmgr_param.h
	@brief    file_name & simple comment.

	Description :			\n
	Module :				\n
	Remarks :				\n
	 Date			Modification				Name\n
	-----------	-------------------------------------	------------------\n
	 2013/04/12		initiated					moonsg\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	__XMGR_PARAM_INT_H__
#define	__XMGR_PARAM_INT_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>

#include <mgr_common.h>
#include <mgr_param.h>

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/

extern HERROR xmgr_param_Init_Base (void);

extern HERROR xmgr_param_SetFactoryDefault_Base(DxFactoryDefault_type_e eDefaultType);

extern HERROR xmgr_param_GetUpdatedList_Base(HUINT8 *pszUpdatedList);

#if defined(CONFIG_PROD_IR4000HD)
	extern HERROR xmgr_param_GetUpdatedList_IR4000HD(HUINT8 *pszUpdatedList);
#else 
	extern HERROR xmgr_param_GetUpdatedList_Base(HUINT8 *pzUpdatedList);
#endif

#endif /* __XMGR_PARAM_INT_H__ */

