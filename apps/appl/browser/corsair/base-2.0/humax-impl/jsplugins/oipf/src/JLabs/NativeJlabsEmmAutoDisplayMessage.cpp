/**************************************************************************************/
/**
 * @file NativeJlabsEmmAutoDisplayMessage.cpp
 *
 * JlabsEmmAutoDisplayMessage module
 *
 * @author  sung-hoon Lee(shlee3@humaxdigital.com)
 * @date    2012/03/16
 * @attention   Copyright (c) 2011 Humax - All rights reserved.
 *
 **************************************************************************************
 **/
#if defined OIPF && defined JLABS


#include "NativeJlabsEmmAutoDisplayMessage.h"
#include "macros.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <oplcasbc.h>

#define	__TRACE()					do{OOIF_LOG_DUMMY("[%s:%d] trace\n", __FUNCTION__, __LINE__);}while(0)
#define	__RETURN_ERROR_IF(cond)		do{if(cond){OOIF_LOG_ERROR("[%s:%d] error \'%s\'\n", __FUNCTION__, __LINE__, #cond);return OOIF_RETURN_ERROR;}}while(0)
#define	__RETURN_NULL_IF(cond)		do{if(cond){OOIF_LOG_WARNING("[%s:%d] null \'%s\'\n", __FUNCTION__, __LINE__, #cond);return OOIF_RETURN_VALUE_NULL;}}while(0)
#define	__RETURN_UNDEFINED_IF(cond)	do{if(cond){OOIF_LOG_WARNING("[%s:%d] undefined \'%s\'\n", __FUNCTION__, __LINE__, #cond);return OOIF_RETURN_VALUE_UNDEFINED;}}while(0)
#define	__RETURN_OK_IF(cond)		do{if(cond)return OOIF_RETURN_ERROR;}while(0)


OOIFReturnCode native_jlabsEmmAutoDisplayMessageGetId(jlabsEmmAutoDisplayMessageHandle id, OOIFNumber *retval)
{
	HUINT32 msgId = 0;
	OOIF_LOG_DUMMY("native_jlabsEmmAutoDisplayMessageGetId");
	if((id != NULL) && (retval != NULL))
	{
		if(OPL_CAS_BC_GetAutoDispMsgId((OPL_AutoDispMsg_t *)id,&msgId) == BPL_STATUS_OK)
		{
			*retval = static_cast<OOIFNumber>(msgId);
			return OOIF_RETURN_OK;
		}
	}
	*retval = static_cast<OOIFNumber>(msgId);
	return OOIF_RETURN_VALUE_NULL;
}

OOIFReturnCode native_jlabsEmmAutoDisplayMessageGetAutoDisplayType(jlabsEmmAutoDisplayMessageHandle id, OOIFNumber *retval)
{
	HUINT32 msgType = 0;
	OOIF_LOG_DUMMY("native_jlabsEmmAutoDisplayMessageGetAutoDisplayType");
	if((id != NULL) && (retval != NULL))
	{
		if(OPL_CAS_BC_GetAutoDispMsgType((OPL_AutoDispMsg_t *)id, &msgType) == BPL_STATUS_OK)
		{
			*retval = static_cast<OOIFNumber>(msgType);
			return OOIF_RETURN_OK;
		}
	}
	*retval = static_cast<OOIFNumber>(msgType);
	return OOIF_RETURN_VALUE_NULL;
}

OOIFReturnCode native_jlabsEmmAutoDisplayMessageReleaseHandle(jlabsEmmAutoDisplayMessageHandle id)
{
	OOIF_LOG_DUMMY("native_jlabsEmmAutoDisplayMessageReleaseHandle");
	if(id != NULL)
	{
		OPL_CAS_BC_ReleaseAutoDispMsg((OPL_AutoDispMsg_t *)id);
	}
	return OOIF_RETURN_OK;
}

#endif

