/**************************************************************************************/
/**
 * @file NativeJlabsNetworkService.cpp
 *
 * jlabsDTTChanne module
 *
 * @author 
 * @date    
 * @attention   Copyright (c) 2011 Humax - All rights reserved.
 *
 **************************************************************************************
 **/

#include "NativeJlabsRemoteControlFunction.h"
#include "macros.h"
#include "oplnetservice.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define	__TRACE()					do{OOIF_LOG_DUMMY("[%s:%d] trace\n", __FUNCTION__, __LINE__);}while(0)
#define	__RETURN_ERROR_IF(cond)		do{if(cond){OOIF_LOG_ERROR("[%s:%d] error \'%s\'\n", __FUNCTION__, __LINE__, #cond);return OOIF_RETURN_ERROR;}}while(0)
#define	__RETURN_NULL_IF(cond)		do{if(cond){OOIF_LOG_WARNING("[%s:%d] null \'%s\'\n", __FUNCTION__, __LINE__, #cond);return OOIF_RETURN_VALUE_NULL;}}while(0)
#define	__RETURN_UNDEFINED_IF(cond)	do{if(cond){OOIF_LOG_WARNING("[%s:%d] undefined \'%s\'\n", __FUNCTION__, __LINE__, #cond);return OOIF_RETURN_VALUE_UNDEFINED;}}while(0)
#define	__RETURN_OK_IF(cond)		do{if(cond)return OOIF_RETURN_ERROR;}while(0)


OOIFReturnCode native_jlabsRemoteControlFunctionCreate(jlabsRemoteControlFunctionHandle *id,long int window_identifier, const char *host_uri)
{
	
	return OOIF_RETURN_OK;
}


OOIFReturnCode native_jlabsRemoteControlFunctionGetDeviceHandle(jlabsRemoteControlFunctionHandle identifier, OOIFNumber *retval)
{
    return OOIF_RETURN_OK;

}

OOIFReturnCode native_jlabsRemoteControlFunctionGetDeviceUA(jlabsRemoteControlFunctionHandle identifier, const char * *retval)
{
	return OOIF_RETURN_OK;
}


OOIFReturnCode native_jlabsRemoteControlFunctionServerSideXMLUIListing(jlabsRemoteControlFunctionHandle identifier, const char *UIList, OOIFBoolean advertise, OOIFBoolean *retval)
{
	
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsRemoteControlFunctionSendRemoteMessage(jlabsRemoteControlFunctionHandle identifier, OOIFNumber devHandle, OOIFNumber reqHandle, const char * hearders,  const char * message, OOIFBoolean *retval)
{
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsRemoteControlFunctionSendMulticatNotify(jlabsRemoteControlFunctionHandle identifier, OOIFNumber devHandle, OOIFNumber eventLevel, const char * notifCEHTML, const char friendlyName, const char profileList, OOIFBoolean *retval)
{
	
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsRemoteControlFunctionSendInternalServerError(jlabsRemoteControlFunctionHandle identifier, OOIFNumber devHandle, OOIFNumber reqHandle, OOIFBoolean *retval)
{
	
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsRemoteControlFunctionDropConnection(jlabsRemoteControlFunctionHandle identifier, OOIFNumber devHandle, OOIFBoolean *retval)
{
	
	return OOIF_RETURN_OK;
}


OOIFReturnCode native_jlabsRemoteControlFunctionReleaseHandle(jlabsRemoteControlFunctionHandle identifier)
{

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsRemoteControlFunctionRegisterEventHandler( OPL_NETWORKSERVICE_EventCallback cbFunc)
{
	__TRACE();
	__RETURN_ERROR_IF(cbFunc == NULL);


	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsRemoteControlFunctionUnRegisterEventHandler( OPL_NETWORKSERVICE_EventCallback cbFunc)
{
	__TRACE();
	__RETURN_ERROR_IF(cbFunc == NULL);

	return OOIF_RETURN_OK;
}

