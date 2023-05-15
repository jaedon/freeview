/**************************************************************************************/
/**
 * @file NativeJlabsCDSRecording.cpp
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

#include "NativeJlabsDMS.h"
#include "opldmp.h"

#include "macros.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define	__TRACE()					do{OOIF_LOG_DUMMY("[%s:%d] trace\n", __FUNCTION__, __LINE__);}while(0)
#define	__RETURN_ERROR_IF(cond)		do{if(cond){OOIF_LOG_ERROR("[%s:%d] error \'%s\'\n", __FUNCTION__, __LINE__, #cond);return OOIF_RETURN_ERROR;}}while(0)
#define	__RETURN_NULL_IF(cond)		do{if(cond){OOIF_LOG_WARNING("[%s:%d] null \'%s\'\n", __FUNCTION__, __LINE__, #cond);return OOIF_RETURN_VALUE_NULL;}}while(0)
#define	__RETURN_UNDEFINED_IF(cond)	do{if(cond){OOIF_LOG_WARNING("[%s:%d] undefined \'%s\'\n", __FUNCTION__, __LINE__, #cond);return OOIF_RETURN_VALUE_UNDEFINED;}}while(0)
#define	__RETURN_OK_IF(cond)		do{if(cond)return OOIF_RETURN_ERROR;}while(0)

OOIFReturnCode native_jlabsDMSDeviceGetDeviceHandle( jlabsDMSDeviceHandle identifier, const char **retval)
{
	OplDMSDevice *dmsDevice =  static_cast<OplDMSDevice *>(identifier);
	*retval = dmsDevice->getUDN();
	//*retval = dmsDevice->getDeviceHandle();
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsDMSDeviceGetUDN( jlabsDMSDeviceHandle identifier, const char **retval )
{
	OplDMSDevice *dmsDevice =  static_cast<OplDMSDevice *>(identifier);
	*retval = dmsDevice->getUDN();
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsDMSDeviceGetFriendlyName( jlabsDMSDeviceHandle identifier, const char **retval )
{
	OplDMSDevice *dmsDevice =  static_cast<OplDMSDevice *>(identifier);
	*retval = dmsDevice->getFriendlyName();
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsDMSDeviceGetIpAddress( jlabsDMSDeviceHandle identifier, const char **retval )
{
	OplDMSDevice *dmsDevice =  static_cast<OplDMSDevice *>(identifier);
	*retval = dmsDevice->getIpAddress();
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsDMSDeviceMACAddress( jlabsDMSDeviceHandle identifier, const char **retval )
{
	OplDMSDevice *dmsDevice =  static_cast<OplDMSDevice *>(identifier);
	*retval = dmsDevice->getMACAddress();
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsDMSDeviceGetTotalStorageSize( jlabsDMSDeviceHandle identifier, const char **retval )
{
	OplDMSDevice *dmsDevice =  static_cast<OplDMSDevice *>(identifier);
	*retval = dmsDevice->getTotalSize();
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsDMSDeviceGetSupportedSortCAP(jlabsDMSDeviceHandle identifier, int *count, char ***retval)
{
	OplDMSDevice *dmsDevice =  static_cast<OplDMSDevice *>(identifier);
	int i, nCapCnt;
	char **get, **ret;

	get = dmsDevice->getSortCAP(&nCapCnt);
	ret = (char**)HLIB_STD_MemAlloc(sizeof(char*)*nCapCnt);

	for(i=0; i<nCapCnt; i++)
		ret[i] = HLIB_STD_StrDup(get[i]);

	*retval = ret;
	*count = nCapCnt;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsDMSDeviceGetSupportedSearchCAP(jlabsDMSDeviceHandle identifier, int *count, char ***retval)
{
	OplDMSDevice *dmsDevice =  static_cast<OplDMSDevice *>(identifier);

	int i, nCapCnt;
	char **get, **ret;

	get = dmsDevice->getSearchCAP(&nCapCnt);
	ret = (char**)HLIB_STD_MemAlloc(sizeof(char*)*nCapCnt);

	for(i=0; i<nCapCnt; i++)
		ret[i] = HLIB_STD_StrDup(get[i]);

	*retval = ret;
	*count = nCapCnt;


	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsDMSDeviceGetSupportedDlnaCAP(jlabsDMSDeviceHandle identifier, int *count, char ***retval)
{
	OplDMSDevice *dmsDevice =  static_cast<OplDMSDevice *>(identifier);

	int i, nCapCnt;
	char **get, **ret;

	get = dmsDevice->getDlnaCAP(&nCapCnt);
	ret = (char**)HLIB_STD_MemAlloc(sizeof(char*)*nCapCnt);

	for(i=0; i<nCapCnt; i++)
		ret[i] = HLIB_STD_StrDup(get[i]);

	*retval = ret;
	*count = nCapCnt;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsDMSDeviceGetSupportedJlabsCAP(jlabsDMSDeviceHandle identifier, int *count, char ***retval)
{
	OplDMSDevice *dmsDevice =  static_cast<OplDMSDevice *>(identifier);
	*retval = dmsDevice->getJlabsCAP(count);
	int i, nCapCnt;
	char **get, **ret;

	get = dmsDevice->getJlabsCAP(&nCapCnt);
	ret = (char**)HLIB_STD_MemAlloc(sizeof(char*)*nCapCnt);

	for(i=0; i<nCapCnt; i++)
		ret[i] = HLIB_STD_StrDup(get[i]);

	*retval = ret;
	*count = nCapCnt;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsDMSDeviceGetRecordDestinations(jlabsDMSDeviceHandle identifier, OOIFBoolean *retval )
{
	BPLStatus Err;
	int 	i, count;
	char 	**Caps = NULL;

	bool 	bIsJlabRec = false;

	OplDMSDevice *dmsDevice =  static_cast<OplDMSDevice *>(identifier);
	*retval = FALSE;

	Caps = dmsDevice->getJlabsCAP(&count);
	if(count > 0 && Caps!= NULL)
	{
		for(i=0; i<count; i++)
		{
			if(!strcmp(Caps[i], "upload_rec_sink-1.0"))
			{
				bIsJlabRec = true;
			}
		}
	}

	if(bIsJlabRec || dmsDevice->bIsSupportSrs)
		*retval = TRUE;

	if(*retval == TRUE)
		Err = dmsDevice->getRecordDestination(dmsDevice->m_szUDN);
	else
		return OOIF_RETURN_OK;

	if(Err !=  BPL_STATUS_OK)
		*retval = FALSE;
	else
		*retval = TRUE;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsDMSDeviceContentSearch(jlabsDMSDeviceHandle identifier, jlabsContentSearchHandle *retval)
{
	OplDMSDevice *dmsDevice =  static_cast<OplDMSDevice *>(identifier);
	OOIF_LOG_DUMMY("native_jlabsDMSDeviceContentSearch called");
	(void )(dmsDevice);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsDMSDeviceRegisterEventHandler( void (cbFunc)(int event, int size, void **data) )
{
	OplDMSDevice::registerEventListener( cbFunc );
	return OOIF_RETURN_OK;
}


OOIFReturnCode native_jlabsDMSDeviceUnRegisterEventHandler( void )
{
	OplDMSDevice::unRegisterEventListener();
	return OOIF_RETURN_OK;
}

/* Methods */
OOIFReturnCode native_jlabsDMSDeviceReleaseHandle( jlabsDMSDeviceHandle id )
{
	OplDMSDevice *dmsDevice =  static_cast<OplDMSDevice *>(id);
	if ( NULL != dmsDevice )
	{
		delete dmsDevice;
		dmsDevice = NULL;
	}
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsDMSDeviceBrowseDirectChildren( jlabsDMSDeviceHandle identifier, char *cid, char *sort, int index, int request, OOIFBoolean *retval )
{
	BPLStatus Err;
	OplDMSDevice *dmsDevice =  static_cast<OplDMSDevice *>(identifier);
	Err = dmsDevice->browsingChildItem( (const char *)cid, sort, index, request);

	if(Err !=  BPL_STATUS_OK)
		*retval = FALSE;
	else
		*retval = TRUE;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsDMSgetAvailableStorageSize( jlabsDMSDeviceHandle identifier, char *dst, OOIFBoolean *retval)
{
	BPLStatus Err;

	OplDMSDevice *dmsDevice =  static_cast<OplDMSDevice *>(identifier);
	Err = dmsDevice->getAvailableStorageSize( (const char *)dst);

	if(Err !=  BPL_STATUS_OK)
		*retval = FALSE;
	else
		*retval = TRUE;

	return OOIF_RETURN_OK;
}

OOIFReturnCode _native_jlabsDMSgetAvailableStorageSize( void *pItem, double *pTotal, double *pAvailabe, char **dst )
{
	Opl_DmsStorage_t *item = (Opl_DmsStorage_t *)pItem;

	if(item == NULL)
		return OOIF_RETURN_ERROR;

	*pTotal = item->Total;
	*pAvailabe = item->Available;
	*dst = item->pDstId;

	return OOIF_RETURN_OK;
}
#endif

