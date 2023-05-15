/**************************************************************************************/
/**
 * @file NativeJlabsEmmMessageManager.cpp
 *
 * jlabsEmmMessageManager module
 *
 * @author  sung-hoon Lee(shlee3@humaxdigital.com)
 * @date    2012/03/16
 * @attention   Copyright (c) 2011 Humax - All rights reserved.
 *
 **************************************************************************************
 **/
#if defined OIPF && defined JLABS

#include "NativeJlabsEmmMessageManager.h"
#include "macros.h"
#include "oplmail.h"

#include <oplcasbc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define	__TRACE()					do{OOIF_LOG_DUMMY("[%s:%d] trace\n", __FUNCTION__, __LINE__);}while(0)
#define	__RETURN_ERROR_IF(cond)		do{if(cond){OOIF_LOG_ERROR("[%s:%d] error \'%s\'\n", __FUNCTION__, __LINE__, #cond);return OOIF_RETURN_ERROR;}}while(0)
#define	__RETURN_NULL_IF(cond)		do{if(cond){OOIF_LOG_WARNING("[%s:%d] null \'%s\'\n", __FUNCTION__, __LINE__, #cond);return OOIF_RETURN_VALUE_NULL;}}while(0)
#define	__RETURN_UNDEFINED_IF(cond)	do{if(cond){OOIF_LOG_WARNING("[%s:%d] undefined \'%s\'\n", __FUNCTION__, __LINE__, #cond);return OOIF_RETURN_VALUE_UNDEFINED;}}while(0)
#define	__RETURN_OK_IF(cond)		do{if(cond)return OOIF_RETURN_ERROR;}while(0)


OOIFReturnCode native_jlabsEmmMessageMgrStopAutoDispMsg(int id, int *pResponse)
{
	OOIF_LOG_DUMMY("native_jlabsEmmMessageMgrStopAutoDispMsg");
	if(OPL_CAS_BC_StopAutoDispMsg(id, pResponse) == BPL_STATUS_OK)
	{
		return OOIF_RETURN_OK;
	}
	return OOIF_RETURN_ERROR;
}

OOIFReturnCode native_jlabsEmmMessageMgrGetAllEmmMsg(int *count, jlabsEmmMessageHandle**handles)
{
	*count = OPL_Mail_GetMailListByType( (OPL_MailHandle **)handles, eOPL_MailType_Emm );
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsEmmMessageMgrGetAllSelfMsg(int *count, jlabsEmmMessageHandle**handles)
{
	*count = OPL_Mail_GetMailListByType( (OPL_MailHandle **)handles, eOPL_MailType_Self );
	return OOIF_RETURN_OK;
}


OOIFReturnCode native_jlabsEmmMessageMgrGetAllBoardMsg(int *count, jlabsBoardMessageHandle**handles)
{
	*count= OPL_Mail_GetDummyBoardMsgList((OPL_MailHandle **)handles);
	return OOIF_RETURN_OK;
}

#endif


