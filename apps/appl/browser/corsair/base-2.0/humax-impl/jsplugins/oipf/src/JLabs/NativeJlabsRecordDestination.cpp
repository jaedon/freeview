/**************************************************************************************/
/**
 * @file NativeJlabsDestination.cpp
 *
 * jlabsDTTChanne module
 *
 * @author 
 * @date    
 * @attention   Copyright (c) 2011 Humax - All rights reserved.
 *
 **************************************************************************************
 **/

#if defined OIPF && defined JLABS
#include "NativeJlabsRecordDestination.h"
#include "macros.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "opldmp.h"

#define	__TRACE()					do{OOIF_LOG_DUMMY("[%s:%d] trace\n", __FUNCTION__, __LINE__);}while(0)
#define	__RETURN_ERROR_IF(cond)		do{if(cond){OOIF_LOG_ERROR("[%s:%d] error \'%s\'\n", __FUNCTION__, __LINE__, #cond);return OOIF_RETURN_ERROR;}}while(0)
#define	__RETURN_NULL_IF(cond)		do{if(cond){OOIF_LOG_WARNING("[%s:%d] null \'%s\'\n", __FUNCTION__, __LINE__, #cond);return OOIF_RETURN_VALUE_NULL;}}while(0)
#define	__RETURN_UNDEFINED_IF(cond)	do{if(cond){OOIF_LOG_WARNING("[%s:%d] undefined \'%s\'\n", __FUNCTION__, __LINE__, #cond);return OOIF_RETURN_VALUE_UNDEFINED;}}while(0)
#define	__RETURN_OK_IF(cond)		do{if(cond)return OOIF_RETURN_ERROR;}while(0)

OOIFReturnCode native_jlabsRecordDestinationId(jlabsRecordDestinationHandle identifier,  const char **retval )
{
	OplDLNADestination *content =  static_cast<OplDLNADestination *>(identifier);
	*retval = content->szId;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsRecordDestinationName(jlabsRecordDestinationHandle identifier, const char **retval )
{
	OplDLNADestination *content =  static_cast<OplDLNADestination *>(identifier);
	*retval = content->szName;
	return OOIF_RETURN_OK;
}
#endif
