/**
	@file     mgr_param.c
	@brief    .

	Description:
	Module: MGR/PARAM			 	\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#include <db_param.h>

#include <svc_sys.h>

#include <bus.h>
#include <mgr_common.h>

#include <mgr_param.h>
#include <pmgr_param.h>

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
/********************      API Functions   *************************/
/*******************************************************************/

/********************************************************************/
/********************	Application Proc.	*************************/
/********************************************************************/

HERROR MGR_PARAM_Init (void)
{
	return pmgr_param_Init ();
}

HERROR	MGR_PARAM_SetSysLanguage(HxLANG_Id_e eLangId)
{
	HERROR	 hErr;
	HCHAR	*pszLanguage;

	// 3-1. Convert HxLANG_Id_e to iso code
	pszLanguage = (HCHAR *)HLIB_LANG_IdTo639(eLangId);
	if (NULL == pszLanguage)
		return ERR_FAIL;

	// 4. Set Loader Language
	hErr = SVC_SYS_SetLanguage((HUINT8 *)pszLanguage);

	return hErr;
}

HERROR	MGR_PARAM_SetFactoryDefault(DxFactoryDefault_type_e eDefaultType)
{
	return pmgr_param_SetFactoryDefault (eDefaultType);
}

HERROR	MGR_PARAM_GetUpdatedList(HUINT8 *pzUpdatedList)
{
	return pmgr_param_GetUpdatedList(pzUpdatedList);
}


