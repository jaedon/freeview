/**************************************************************************************/
/**
 * @file NativeJlabsEmmMessage.cpp
 *
 * jlabsEmmMessage module
 *
 * @author  sung-hoon Lee(shlee3@humaxdigital.com)
 * @date    2012/03/16
 * @attention   Copyright (c) 2011 Humax - All rights reserved.
 *
 **************************************************************************************
 **/
#if defined OIPF && defined JLABS

#include "NativeJlabsEmmMessage.h"
#include "macros.h"

#include "oplmail.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define	__TRACE()					do{OOIF_LOG_DUMMY("[%s:%d] trace\n", __FUNCTION__, __LINE__);}while(0)
#define	__RETURN_ERROR_IF(cond)		do{if(cond){OOIF_LOG_ERROR("[%s:%d] error \'%s\'\n", __FUNCTION__, __LINE__, #cond);return OOIF_RETURN_ERROR;}}while(0)
#define	__RETURN_NULL_IF(cond)		do{if(cond){OOIF_LOG_WARNING("[%s:%d] null \'%s\'\n", __FUNCTION__, __LINE__, #cond);return OOIF_RETURN_VALUE_NULL;}}while(0)
#define	__RETURN_UNDEFINED_IF(cond)	do{if(cond){OOIF_LOG_WARNING("[%s:%d] undefined \'%s\'\n", __FUNCTION__, __LINE__, #cond);return OOIF_RETURN_VALUE_UNDEFINED;}}while(0)
#define	__RETURN_OK_IF(cond)		do{if(cond)return OOIF_RETURN_ERROR;}while(0)


// ro Integer msgId
OOIFReturnCode native_jlabsEmmMessageGetMsgId(jlabsEmmMessageHandle id, OOIFNumber *retval)
{
	*retval = OPL_Mail_GetMsgId( (OPL_MailHandle)id );
	return OOIF_RETURN_OK;
}

// ro Integer msgType
OOIFReturnCode native_jlabsEmmMessageGetMsgType(jlabsEmmMessageHandle id, OOIFNumber *retval)
{
	*retval = OPL_Mail_GetMsgType( (OPL_MailHandle)id );
	return OOIF_RETURN_OK;
}


// ro String networkTypeName
OOIFReturnCode native_jlabsEmmMessageGetNetworkTypeName(jlabsEmmMessageHandle id, const char **retval)
{
	*retval = OPL_Mail_GetNetworkTypeName( (OPL_MailHandle)id );	
	return OOIF_RETURN_OK;
}

// ro String date
OOIFReturnCode native_jlabsEmmMessageGetDate(jlabsEmmMessageHandle id, const char **retval)
{
	*retval = OPL_Mail_GetDate( (OPL_MailHandle)id );	
	return OOIF_RETURN_OK;
}

// ro String msgBody
OOIFReturnCode native_jlabsEmmMessageGetMsgBody(jlabsEmmMessageHandle id, const char **retval)
{
	*retval = OPL_Mail_GetMsgBody( (OPL_MailHandle)id );	
	return OOIF_RETURN_OK;
}

// ro Boolean readDoneFlag
OOIFReturnCode native_jlabsEmmMessageGetReadDoneFlag(jlabsEmmMessageHandle id, OOIFBoolean *retval)
{
	*retval = OPL_Mail_GetReadDoneFlag( (OPL_MailHandle)id );	
	return OOIF_RETURN_OK;
}

// ro String msgTitle
OOIFReturnCode native_jlabsEmmMessageGetMsgTitle(jlabsEmmMessageHandle id, const char **retval)
{
	*retval = OPL_Mail_GetMsgTitle( (OPL_MailHandle)id );	
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsEmmMessageReleaseHandle(jlabsEmmMessageHandle id)
{
	OPL_Mail_Delete( id );
	return OOIF_RETURN_OK;
}


#endif
