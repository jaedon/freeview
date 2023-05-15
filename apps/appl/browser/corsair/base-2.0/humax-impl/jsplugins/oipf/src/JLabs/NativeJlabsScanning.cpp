/**************************************************************************************/
/**
 * @file NativeJlabsScanning.cpp
 *
 * jlabsDTTChanne module
 *
 * @author  sbkim(sbkim@humaxdigital.com)
 * @date    2012/03/15
 * @attention   Copyright (c) 2011 Humax - All rights reserved.
 *
 **************************************************************************************
 **/
#if defined OIPF && defined JLABS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "macros.h"
#include "NativeJlabsScanning.h"
#include "NativeJlabsLocalSystem.h"
#include "NativeChannel.h"
#include "oplfavourite.h"


#define	__TRACE()					do{OOIF_LOG_DUMMY("[%s:%d] trace\n", __FUNCTION__, __LINE__);}while(0)
#define	__RETURN_ERROR_IF(cond)		do{if(cond){OOIF_LOG_ERROR("[%s:%d] error \'%s\'\n", __FUNCTION__, __LINE__, #cond);return OOIF_RETURN_ERROR;}}while(0)
#define	__RETURN_NULL_IF(cond)		do{if(cond){OOIF_LOG_WARNING("[%s:%d] null \'%s\'\n", __FUNCTION__, __LINE__, #cond);return OOIF_RETURN_VALUE_NULL;}}while(0)
#define	__RETURN_UNDEFINED_IF(cond)	do{if(cond){OOIF_LOG_WARNING("[%s:%d] undefined \'%s\'\n", __FUNCTION__, __LINE__, #cond);return OOIF_RETURN_VALUE_UNDEFINED;}}while(0)
#define	__RETURN_OK_IF(cond)		do{if(cond)return OOIF_RETURN_ERROR;}while(0)


OOIFReturnCode native_jlabsScanningGetNetworkType(jlabsScanningHandle handle, const char **retval)
{
	stNativeJlabsLocalSystem_ScanSetting_t *pstSetting = (stNativeJlabsLocalSystem_ScanSetting_t*)handle;
	OOIF_LOG_DUMMY("native_jlabsScanningGetNetworkType");
	*retval = (const char*)&pstSetting->aucNetworkType[0];
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsScanningGetFavoriteID(jlabsScanningHandle handle, const char **retval)
{
	FavouriteListHandle *hFavs = NULL;
	ChannelHandle 	*handles = NULL;
	HUINT32			ulNumOfFavs = 0, ulNumOfCh = 0, i = 0, onId = 0;
	stNativeJlabsLocalSystem_ScanSetting_t *pstSetting = (stNativeJlabsLocalSystem_ScanSetting_t*)handle;
	OOIFReturnCode	ret = OOIF_RETURN_VALUE_NULL;

	OOIF_LOG_INFO("pstSetting->aucNetworkType : %s\n",pstSetting->aucNetworkType);
	ulNumOfFavs = (HUINT32)OPL_Favourite_GetCollection(&hFavs);
	if((ulNumOfFavs == 0) || (hFavs == NULL))
	{
		OOIF_LOG_ERROR("ulNumOfFavs : %d\n",ulNumOfFavs);
		OOIF_LOG_ERROR("hFavs : %p\n",hFavs);
		goto funcExit;
	}
	for(i = 0; i < ulNumOfFavs; i++)
	{
		OOIF_LOG_INFO("hFavs[%d] %d\n",i, hFavs[i]);
		ulNumOfCh= OPL_Favourite_GetChannelList(hFavs[i], (OPL_Channel_t**)&handles);
		if((ulNumOfCh == 0) || (handles == NULL))
		{
			OOIF_LOG_ERROR("ulNumOfCh : %d\n",ulNumOfCh);
			OOIF_LOG_ERROR("handles : %p\n",handles);
			continue;
		}
		onId = OPL_Channel_GetOrgNetworkID(handles[0]);
		OOIF_LOG_INFO("onId 0x%x\n", onId);
		if((HxSTD_StrCmp("DTT", (const char*)pstSetting->aucNetworkType) == 0) && (OPL_MACRO_IS_JAPAN_DT_NETWORK(onId)))
		{
			break;
		}
		else if((HxSTD_StrCmp("BS", (const char*)pstSetting->aucNetworkType) == 0) && (OPL_MACRO_IS_JAPAN_BS_NETWORK(onId)))
		{
			break;
		}
		else if((HxSTD_StrCmp("CATV", (const char*)pstSetting->aucNetworkType) == 0) && (OPL_MACRO_IS_JAPAN_CATV_NETWORK(onId)))
		{
			break;
		}
		else
		{
			continue;
		}
	}

	OOIF_LOG_INFO("i : %d\n",i);
	OOIF_LOG_INFO("ulNumOfFavs : %d\n",ulNumOfFavs);
	/*found*/
	if(i < ulNumOfFavs)
	{
		*retval = OPL_Favourite_GetID(hFavs[i]);
		OOIF_LOG_INFO("*retval %s\n\n",*retval);
		ret = OOIF_RETURN_OK;
	}
funcExit:
	if(hFavs != NULL)
	{
		HLIB_STD_MemFree(hFavs);
		hFavs = NULL;
	}
	if(handles != NULL)
	{
		HLIB_STD_MemFree(handles);
		handles = NULL;
	}
	return ret;
}

/* SETTER */
OOIFReturnCode native_jlabsScanningSetFavoriteID(jlabsScanningHandle handle, const char *value)
{
	const char *favId;
	OOIFReturnCode ret;
	FavouriteListHandle hFav = NULL;

	//OOIF_LOG_DUMMY("Setting Scanning.favId to : %s", value);
	if(HxSTD_StrCmp(value,"") !=0)
	{
		OOIF_LOG_ERROR("try to set favId to %s. only null or \"\" can be set!!\n\n", value);
		return OOIF_RETURN_ERROR;
	}
	OOIF_LOG_INFO("same with \"\" \n");
	ret = native_jlabsScanningGetFavoriteID(handle, &favId);
	if(ret == OOIF_RETURN_VALUE_NULL)
	{
		OOIF_LOG_ERROR("favId is already null \n");
		return OOIF_RETURN_OK;
	}
	OOIF_LOG_INFO(" favId is %s\n",favId);
	hFav = OPL_Favourite_FindByID(favId);
	if(hFav == NULL)
	{
		OOIF_LOG_ERROR("cannot find favourite list by %s\n", favId);
		return OOIF_RETURN_ERROR;
	}
	if(OPL_Favourite_Remove(hFav))
	{
		OOIF_LOG_INFO("remove favouriteList suceeded %s\n", favId);
		return OOIF_RETURN_OK;
	}
	OOIF_LOG_ERROR("remove favouriteList failed %s\n", favId);
	return OOIF_RETURN_ERROR;
}

OOIFReturnCode native_jlabsScanningReleaseHandle(jlabsScanningHandle id)
{
	// TODO: Dummy Implementation
	return OOIF_RETURN_OK;
}
#endif
