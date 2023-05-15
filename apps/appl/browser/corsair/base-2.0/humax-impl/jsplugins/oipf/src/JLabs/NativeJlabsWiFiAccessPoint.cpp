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

#include "NativeJlabsWiFiAccessPoint.h"
#include "macros.h"
#include "oplnetservice.h"
#include "oplnetwork.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define	__TRACE()					do{OOIF_LOG_DUMMY("[%s:%d] trace\n", __FUNCTION__, __LINE__);}while(0)
#define	__RETURN_ERROR_IF(cond)		do{if(cond){OOIF_LOG_ERROR("[%s:%d] error \'%s\'\n", __FUNCTION__, __LINE__, #cond);return OOIF_RETURN_ERROR;}}while(0)
#define	__RETURN_NULL_IF(cond)		do{if(cond){OOIF_LOG_WARNING("[%s:%d] null \'%s\'\n", __FUNCTION__, __LINE__, #cond);return OOIF_RETURN_VALUE_NULL;}}while(0)
#define	__RETURN_UNDEFINED_IF(cond)	do{if(cond){OOIF_LOG_WARNING("[%s:%d] undefined \'%s\'\n", __FUNCTION__, __LINE__, #cond);return OOIF_RETURN_VALUE_UNDEFINED;}}while(0)
#define	__RETURN_OK_IF(cond)		do{if(cond)return OOIF_RETURN_ERROR;}while(0)

OOIFReturnCode native_jlabsWiFiAccessPointGetEssid(void * identifier, const char **retval)
{
	OplNetworkAccessPoint *apObj = static_cast<OplNetworkAccessPoint *>(identifier);

	*retval = apObj->szESSID;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsWiFiAccessPointGetMacAddress(void * identifier, const char **retval)
{
	OplNetworkAccessPoint *apObj = static_cast<OplNetworkAccessPoint *>(identifier);

	*retval = apObj->szMacAddr;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsWiFiAccessPointGetSecureTypeN(void * identifier, OOIFNumber *retval)
{
	OplNetworkAccessPoint *apObj = static_cast<OplNetworkAccessPoint *>(identifier);
	*retval = OPL_NetworkWiFi_ConvertToSecurityInt( apObj->nSecureType);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsWiFiAccessPointGetAuthenTypeN(void * identifier, OOIFNumber *retval)
{
	OplNetworkAccessPoint *apObj = static_cast<OplNetworkAccessPoint *>(identifier);
	*retval = OPL_NetworkWiFi_ConvertToAuthenInt( apObj->nAuthenType );
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsWiFiAccessPointGetEncryptTypeN(void * identifier, OOIFNumber *retval)
{
	OplNetworkAccessPoint *apObj = static_cast<OplNetworkAccessPoint *>(identifier);
	*retval = OPL_NetworkWiFi_ConvertToEncryptInt( apObj->nEncryptType );
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsWiFiAccessPointGetSecureType(void * identifier, const char **retval)
{
	OplNetworkAccessPoint *apObj = static_cast<OplNetworkAccessPoint *>(identifier);
	*retval = OPL_NetworkWiFi_ConvertToSecuritySTR( apObj->nSecureType);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsWiFiAccessPointGetAuthenType(void * identifier, const char **retval)
{
	OplNetworkAccessPoint *apObj = static_cast<OplNetworkAccessPoint *>(identifier);
	*retval = OPL_NetworkWiFi_ConvertToAuthenSTR( apObj->nAuthenType );
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsWiFiAccessPointGetEncryptType(void * identifier, const char **retval)
{
	OplNetworkAccessPoint *apObj = static_cast<OplNetworkAccessPoint *>(identifier);
	*retval = OPL_NetworkWiFi_ConvertToEncryptSTR( apObj->nEncryptType );
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsWiFiAccessPointGetMode(void * identifier, OOIFNumber *retval)
{
	OplNetworkAccessPoint *apObj = static_cast<OplNetworkAccessPoint *>(identifier);
	*retval = OPL_NetworkWiFi_ConvertToModeInt( apObj->nOpMode );
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsWiFiAccessPointGetWaveStrength(void * identifier, OOIFNumber *retval)
{
	OplNetworkAccessPoint *apObj = static_cast<OplNetworkAccessPoint *>(identifier);
	*retval = apObj->nWaveStrength;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsWiFiAccessPointGetConnectSpeed(void * identifier, OOIFNumber *retval)
{
	OplNetworkAccessPoint *apObj = static_cast<OplNetworkAccessPoint *>(identifier);
	*retval = apObj->nConnectSpeed;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsWiFiAccessPointReleaseHandle(void * identifier)
{
	OplNetworkAccessPoint *apObj = static_cast<OplNetworkAccessPoint *>(identifier);
	if ( apObj )
	{
		delete apObj;
		apObj = NULL;
	}
	return OOIF_RETURN_OK;
}



