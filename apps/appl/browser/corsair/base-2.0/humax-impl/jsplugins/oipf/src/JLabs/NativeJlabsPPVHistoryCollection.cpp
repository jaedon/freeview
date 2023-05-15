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

#include "macros.h"
#include "hxstdlib.h"
#include "oplppvhistory.h"
#include "NativeJlabsPPVHistoryCollection.h"

#define	__TRACE()					do{OOIF_LOG_DUMMY("[%s:%d] trace\n", __FUNCTION__, __LINE__);}while(0)
#define	__RETURN_ERROR_IF(cond)		do{if(cond){OOIF_LOG_ERROR("[%s:%d] error \'%s\'\n", __FUNCTION__, __LINE__, #cond);return OOIF_RETURN_ERROR;}}while(0)
#define	__RETURN_NULL_IF(cond)		do{if(cond){OOIF_LOG_WARNING("[%s:%d] null \'%s\'\n", __FUNCTION__, __LINE__, #cond);return OOIF_RETURN_VALUE_NULL;}}while(0)
#define	__RETURN_UNDEFINED_IF(cond)	do{if(cond){OOIF_LOG_WARNING("[%s:%d] undefined \'%s\'\n", __FUNCTION__, __LINE__, #cond);return OOIF_RETURN_VALUE_UNDEFINED;}}while(0)
#define	__RETURN_OK_IF(cond)		do{if(cond)return OOIF_RETURN_ERROR;}while(0)

/* getter read only */
uploadPPVViewlogDataListener *gUploadPPVViewlogData = NULL;

static void onUploadPPVViewlogData(int result)
{
	gUploadPPVViewlogData(result);
}
OOIFReturnCode native_jlabsPPVHistoryCollectionGetTotalPurchasedFee(OOIFNumber *retval)
{
	OPL_PPVHistoryHandle *myHandles = NULL;
	HINT32 numOfHandle = 0, i =0;
	HUINT32 totalFee = 0;
	OOIF_LOG_DUMMY("native_jlabsPPVHistoryCollectionGetTotalPurchasedFee");
	numOfHandle = OPL_PPVHistory_GetPPVHistoryList(&myHandles);
	if(numOfHandle != 0 && myHandles != NULL)
	{
		for(i = 0; i< numOfHandle; i++)
		{
			totalFee += OPL_PPVHistory_GetPPVFee(myHandles[i]);
		}
	}
	else
	{
		totalFee = 0;
	}

	if(myHandles != NULL)
	{
		for(i = 0; i< numOfHandle; i++)
		{
			OPL_PPVHistory_Delete(myHandles[i]);
		}
		HLIB_STD_MemFree(myHandles);
	}
	*retval = static_cast<OOIFNumber>(totalFee);
	return OOIF_RETURN_OK;
}


/* funciton */
OOIFReturnCode native_jlabsPPVHistoryCollectionPPVViewlogData(OOIFNumber*retval)
{
	HERROR hErr = ERR_FAIL;
	HINT32	uploadResult = -1;
	OOIF_LOG_DUMMY("native_jlabsPPVHistoryCollectionPPVViewlogData");

	hErr = OPL_PPVHistory_UploadPPVViewlogData(&uploadResult);
	if((hErr == ERR_OK) && (uploadResult == 0))
	{
		*retval = 0;
		OPL_PPVHistory_SetUploadPPVViewlogDataListener(onUploadPPVViewlogData);
	}
	else
	{
		*retval = -1;
	}
	return OOIF_RETURN_OK;
}
OOIFReturnCode native_jlabsPPVHistoryCollectionDeleteAllPPVPrivateData(OOIFNumber*retval)
{
	HERROR hErr = ERR_FAIL;
	OOIF_LOG_DUMMY("native_jlabsPPVHistoryCollectionDeleteAllPPVPrivateData");
	hErr= OPL_PPVHistory_Remove();
	if(hErr == ERR_OK)
	{
		*retval = 0;
	}
	else
	{
		*retval = -1;
	}

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsPPVHistoryCollectionCancelPPVViewlogDataUpload(OOIFNumber*retval)
{
	HERROR hErr = ERR_FAIL;
	HINT32	cancelResult = -1;
	OOIF_LOG_DUMMY("native_jlabsPPVHistoryCollectionCancelPPVViewlogDataUpload");
	hErr= OPL_PPVHistory_CancelPPVViewlogDataUpload(&cancelResult);
	if((hErr == ERR_OK) && (cancelResult == 0))
	{
		*retval = 0;
	}
	else
	{
		*retval = -1;
	}
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsPPVHistoryCollectionSetListener(uploadPPVViewlogDataListener listener)
{
	gUploadPPVViewlogData = listener;
	return OOIF_RETURN_OK;
}


#endif // #if defined OIPF && defined JLABS
