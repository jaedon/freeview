/**************************************************************************************/
/**
 * @file oplcasbc.c
 *
 * oplcasbc
 *
 * @author  Sunghoon Lee(shlee3@humaxdigital.com)
 * @date    2012/08/13
 * @attention   Copyright (c) 2011 Humax - All rights reserved.
 *
 **************************************************************************************
 **/
#include "oplcasbc.h"

#include "oapi.h"
#include "apk.h"
#include <hlib.h>

#include <stdlib.h>
#include <string.h>

/******************************************************************
	Define
******************************************************************/
/******************************************************************
	Typedef
******************************************************************/
/******************************************************************
	Static variables
******************************************************************/

/******************************************************************
	Static function prototypes
******************************************************************/

/******************************************************************
	local functions
******************************************************************/

BPLStatus	OPL_CAS_BC_ReleaseAutoDispMsg(OPL_AutoDispMsg_t *pDispMsg)
{
	if(APK_CAS_BC_ReleaseAutoDispMsg((APKS_CASBC_BsAutoMsgData_t *) pDispMsg) == ERR_OK)
	{
		return BPL_STATUS_OK;
	}
	return BPL_STATUS_ERROR;
}
BPLStatus	OPL_CAS_BC_GetAutoDispMsgId(OPL_AutoDispMsg_t *pDispMsg, HUINT32 *pMsgId)
{
	if(APK_CAS_BC_GetAutoDispMsgId((APKS_CASBC_BsAutoMsgData_t *) pDispMsg, pMsgId) == ERR_OK)
	{
		return BPL_STATUS_OK;
	}
	return BPL_STATUS_ERROR;
}
BPLStatus	OPL_CAS_BC_GetAutoDispMsgType(OPL_AutoDispMsg_t *pDispMsg, HUINT32 *pMsgType)
{
	if(APK_CAS_BC_GetAutoDispMsgType((APKS_CASBC_BsAutoMsgData_t *) pDispMsg, pMsgType) == ERR_OK)
	{
		return BPL_STATUS_OK;
	}
	return BPL_STATUS_ERROR;
}

BPLStatus	OPL_CAS_BC_StopAutoDispMsg(HINT32 msgId, HINT32 *pResponse)
{
	if(APK_CAS_BC_StopAutoDispMsg(msgId, pResponse) == ERR_OK)
	{
		return BPL_STATUS_OK;
	}
	return BPL_STATUS_ERROR;
}

