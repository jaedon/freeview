/**
	@file     mgr_tplist.c
	@brief

	Description :			\n
	Module :				\n
	Remarks :				\n
	 Date			Modification				Name\n
	-----------	-------------------------------------	------------------\n
	 2013/04/16		init							\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/


/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

#include <string.h>

#include <db_svc.h>
#include <db_param.h>

#include <svc_rec.h>
#include <svc_pb.h>
#include <svc_meta.h>

#include <mgr_appmgr.h>

#include <mgr_action.h>
#include <mgr_live.h>
#include <mgr_spec.h>
#include <mgr_action.h>
#include <pmgr_spec.h>

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
/********************      Functions            ********************/
/*******************************************************************/

HERROR	MGR_SPEC_Init (void)
{
	return pmgr_spec_Init ();
}

HERROR MGR_SPEC_GetCountryCode (HxCountry_e *peCountryId)
{
	HERROR			hErr;
	HxCountry_e		eCountryId = eCountryID_NONE;

	if (NULL == peCountryId)		{ return ERR_FAIL; }

	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_COUNTRY_ID, (HUINT32 *)&eCountryId, 0);
	if (ERR_OK != hErr)
	{
		HxLOG_Warning("DB_PARAM_GetItem(eDB_PARAM_ITEM_COUNTRY_ID) Error!!\n");
	}
	HxLOG_Print("eCountryId = 0x%x\n", eCountryId);

	if(FALSE == HLIB_COUNTRY_IsValidCountryId(eCountryId))
	{
		*peCountryId = eCountryID_NONE;
	}
	else
	{
		*peCountryId = eCountryId;
	}
	HxLOG_Print("eCountryId = 0x%x (eCountryID_NONE = 0x%x)\n", eCountryId, eCountryID_NONE);

	return ERR_OK;
}

HERROR MGR_SPEC_GetOperatorCode (DxOperator_e *peOperator)
{
	HERROR		hErr;

	if (NULL == peOperator)		{ return ERR_FAIL; }

	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_OPERATOR_TYPE, (HUINT32 *)peOperator, 0);
	if (ERR_OK != hErr)
	{
		HxLOG_Warning("DB_PARAM_GetItem(eDB_PARAM_ITEM_OPERATOR_TYPE) Error!!\n");
	}

	return hErr;
}

/* end of file */

