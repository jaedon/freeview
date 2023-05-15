/**************************************************************************************/
/**
 * @file NativeJlabsDTTChannel.cpp
 *
 * jlabsDTTChanne module
 *
 * @author  Seo-Lim Park(slpark@humaxdigital.com)
 * @date    2012/03/15
 * @attention   Copyright (c) 2011 Humax - All rights reserved.
 *
 **************************************************************************************
 **/
#if defined OIPF && defined JLABS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hxstdlib.h"
#include "macros.h"
#include "NativeJlabsPPVHistory.h"

#define	MAX_BUF_LEN 				32
#define	MAX_EVENT_DATA_LEN		1024
#define	__TRACE()					do{OOIF_LOG_DUMMY("[%s:%d] trace\n", __FUNCTION__, __LINE__);}while(0)
#define	__RETURN_ERROR_IF(cond)		do{if(cond){OOIF_LOG_ERROR("[%s:%d] error \'%s\'\n", __FUNCTION__, __LINE__, #cond);return OOIF_RETURN_ERROR;}}while(0)
#define	__RETURN_NULL_IF(cond)		do{if(cond){OOIF_LOG_WARNING("[%s:%d] null \'%s\'\n", __FUNCTION__, __LINE__, #cond);return OOIF_RETURN_VALUE_NULL;}}while(0)
#define	__RETURN_UNDEFINED_IF(cond)	do{if(cond){OOIF_LOG_WARNING("[%s:%d] undefined \'%s\'\n", __FUNCTION__, __LINE__, #cond);return OOIF_RETURN_VALUE_UNDEFINED;}}while(0)
#define	__RETURN_OK_IF(cond)		do{if(cond)return OOIF_RETURN_ERROR;}while(0)

/* GETTER */
OOIFReturnCode native_jlabsPPVHistroyGetFee(jlabsPPVHistoryHandle identifier, const char **retval)
{
	static char acBuf[MAX_BUF_LEN];
	OOIF_LOG_DUMMY("native_jlabsPPVHistroyGetFee");
	HxSTD_MemSet(acBuf, 0x00, MAX_BUF_LEN);
	HxSTD_snprintf(acBuf, MAX_BUF_LEN-1, "%d",OPL_PPVHistory_GetPPVFee(identifier));
	*retval = acBuf;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsPPVHistroyGetEventData(jlabsPPVHistoryHandle identifier, const char **retval)
{
	static char acBuf[MAX_EVENT_DATA_LEN];
	HERROR hErr = ERR_FAIL;
	OOIF_LOG_DUMMY("native_jlabsPPVHistroyGetEventData");
	HxSTD_MemSet(acBuf, 0x00, MAX_BUF_LEN);
	hErr = OPL_PPVHistory_GetEventData(identifier, (HCHAR*)acBuf, MAX_EVENT_DATA_LEN-1);
	if(hErr != ERR_OK)
	{
		return OOIF_RETURN_ERROR;
	}
	*retval = acBuf;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsPPVHistroyGetDate(jlabsPPVHistoryHandle identifier, OOIFNumber *retval)
{
	OOIF_LOG_DUMMY("native_jlabsPPVHistroyGetDate");
	*retval = OPL_PPVHistory_GetPurchaseDate(identifier);
	 return OOIF_RETURN_OK;
}

/*** read only ***/
OOIFReturnCode native_jlabsPPVHistroyGetCaSystemId(jlabsPPVHistoryHandle identifier, const char **retval)
{
	static char acBuf[MAX_BUF_LEN];
	HERROR hErr = ERR_FAIL;
	OOIF_LOG_DUMMY("native_jlabsPPVHistroyGetCaSystemId");
	HxSTD_MemSet(acBuf, 0x00, MAX_BUF_LEN);
	hErr = OPL_PPVHistory_GetSystemId(identifier, (HCHAR*)acBuf, MAX_BUF_LEN-1);
	if(hErr != ERR_OK)
	{
		return OOIF_RETURN_ERROR;
	}
	*retval = acBuf;
	return OOIF_RETURN_OK;
}

/* SETTER */
OOIFReturnCode native_jlabsPPVHistroySetFee(jlabsPPVHistoryHandle identifier, const char *value)
{
	// TODO: Dummy Implementation
	OOIF_LOG_DUMMY("Setting PPVHistroy.fee to : %s", value);
	return OOIF_RETURN_NOT_SUPPORTED;
}

OOIFReturnCode native_jlabsPPVHistroySetEventData(jlabsPPVHistoryHandle identifier, const char *value)
{
	// TODO: Dummy Implementation
	OOIF_LOG_DUMMY("Setting PPVHistroy.eventData to : %s", value);
	return OOIF_RETURN_NOT_SUPPORTED;
}

OOIFReturnCode native_jlabsPPVHistroySetDate(jlabsPPVHistoryHandle identifier, OOIFNumber value)
{
	// TODO: Dummy Implementation
	OOIF_LOG_DUMMY("Setting PPVHistroy.Date to : %f", value);
	return OOIF_RETURN_NOT_SUPPORTED;
}

// String get(String key)
OOIFReturnCode native_jlabsPPVHistoryGet(const char *name, const char **retval)
{
	OOIF_LOG_DUMMY("Called PPVHistory.get with %s", name);
	*retval = "Dummy Native Text";
	return OOIF_RETURN_NOT_SUPPORTED;
}

// void set(String key, String value);
OOIFReturnCode native_jlabsPPVHistorySet(const char *name, const char *value)
{
	OOIF_LOG_DUMMY("Called PPVHistory.set with %s, %s", name, value);
	return OOIF_RETURN_NOT_SUPPORTED;
}

OOIFReturnCode native_jlabsPPVHistroyReleaseHandle(jlabsPPVHistoryHandle id)
{
	OOIF_LOG_DUMMY("native_jlabsPPVHistroyReleaseHandle");
	if(id != NULL)
	{
		OPL_PPVHistory_Delete(id);
	}
	return OOIF_RETURN_OK;
}

#endif // #if defined OIPF && defined JLABS
