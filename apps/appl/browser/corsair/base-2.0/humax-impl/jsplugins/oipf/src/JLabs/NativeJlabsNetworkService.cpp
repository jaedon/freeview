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

#include "NativeJlabsNetworkService.h"
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

OOIFReturnCode native_jlabsNetworkServiceGetServiceName(jlabsNetworkServiceHandle identifier, const char **retval)
{
	OplNetworkService *networkserviceObj = static_cast<OplNetworkService*>(identifier);
    *retval = networkserviceObj->getServiceName();
    return OOIF_RETURN_OK;

}
OOIFReturnCode native_jlabsNetworkServiceGetTargetName(jlabsNetworkServiceHandle identifier, const char **retval)
{
	OplNetworkService *networkserviceObj = static_cast<OplNetworkService*>(identifier);
    *retval = networkserviceObj->getTargetName();
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsNetworkServiceSetTargetName(jlabsNetworkServiceHandle identifier, const char *value)
{
	OplNetworkService *networkserviceObj = static_cast<OplNetworkService*>(identifier);
	char *p = (char *) value;
	OOIFBoolean retval = networkserviceObj->setTargetName(p);
	if ( TRUE != retval  )
	{
		return OOIF_RETURN_ERROR;
	}
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsNetworkServiceGet(jlabsNetworkServiceHandle identifier, const char *name, const char **retval)
{
	__TRACE();
	__RETURN_ERROR_IF(name == NULL);
	__RETURN_ERROR_IF(retval == NULL);

	OplNetworkService *networkserviceObj = static_cast<OplNetworkService*>(identifier);
	char *ret = networkserviceObj->getCmd((char *)name);
	*retval = ret;

	return OOIF_RETURN_OK;
}
OOIFReturnCode native_jlabsNetworkServiceSet(jlabsNetworkServiceHandle identifier, const char *name, const char *value, const char **retval)
{
	__TRACE();
	__RETURN_ERROR_IF(name == NULL);
	__RETURN_ERROR_IF(value == NULL);
	__RETURN_ERROR_IF(retval == NULL);
	
	OplNetworkService *networkserviceObj = static_cast<OplNetworkService*>(identifier);
	char *p1 = (char *)name;
	char *p2 = (char *)value;
	 networkserviceObj->setCmd(p1, p2);
	return OOIF_RETURN_OK;
}
OOIFReturnCode native_jlabsNetworkServiceRestart(jlabsNetworkServiceHandle identifier, OOIFNumber *retval)
{
	__TRACE();
	__RETURN_ERROR_IF(retval == NULL);

	OplNetworkService *networkserviceObj = static_cast<OplNetworkService*>(identifier);
	*retval =  networkserviceObj->restart();
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsNetworkServiceStop(jlabsNetworkServiceHandle identifier, OOIFNumber *retval)
{
	__TRACE();
	__RETURN_ERROR_IF(retval == NULL);

	OplNetworkService *networkserviceObj = static_cast<OplNetworkService*>(identifier);
	*retval =  networkserviceObj->stop() ;
	return OOIF_RETURN_OK;
}
OOIFReturnCode native_jlabsNetworkServiceReleaseHandle(jlabsNetworkServiceHandle identifier)
{
	__TRACE();
	__RETURN_ERROR_IF(identifier == NULL);

	OplNetworkService *networkserviceObj = static_cast<OplNetworkService*>(identifier);
	if ( networkserviceObj )
	{
		delete networkserviceObj;
	}
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsNetworkServiceGetSvcIndex( jlabsNetworkServiceHandle identifier, OOIFNumber *retval )
{
	__TRACE();
	__RETURN_ERROR_IF(retval == NULL);

	OplNetworkService *networkserviceObj = static_cast<OplNetworkService*>(identifier);
	*retval =  networkserviceObj->getServiceIndex();

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsNetworkServiceGetTargetIndex( jlabsNetworkServiceHandle identifier, OOIFNumber *retval )
{
	__TRACE();
	__RETURN_ERROR_IF(retval == NULL);

	OplNetworkService *networkserviceObj = static_cast<OplNetworkService*>(identifier);
	*retval =  networkserviceObj->getTargetIndex();

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsNetworkServiceRegisterEventHandler( OPL_NETWORKSERVICE_EventCallback cbFunc)
{
	__TRACE();
	__RETURN_ERROR_IF(cbFunc == NULL);

	OplNetworkService::registerEventListener( cbFunc );

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsNetworkServiceUnRegisterEventHandler( OPL_NETWORKSERVICE_EventCallback cbFunc)
{
	__TRACE();
	__RETURN_ERROR_IF(cbFunc == NULL);

	OplNetworkService::unregisterEventListener( cbFunc );

	return OOIF_RETURN_OK;
}

