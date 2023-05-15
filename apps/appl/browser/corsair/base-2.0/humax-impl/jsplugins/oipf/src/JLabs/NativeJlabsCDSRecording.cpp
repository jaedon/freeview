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
#include "NativeJlabsCDSRecording.h"
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

OOIFReturnCode native_jlabsCDSRecordingGetObjectID( jlabsCDSRecordingHandle identifier, const char **retval )
{
	OplDLNAContent *content =  static_cast<OplDLNAContent *>(identifier);
	*retval = content->szCID;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsCDSRecordingGetClass( jlabsCDSRecordingHandle identifier, const char **retval )
{
	OplDLNAContent *content =  static_cast<OplDLNAContent *>(identifier);
	*retval = content->szClass;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsCDSRecordingGetRestricted( jlabsCDSRecordingHandle identifier, OOIFBoolean *retval)
{
	OplDLNAContent *content =  static_cast<OplDLNAContent *>(identifier);
	*retval = (OOIFBoolean)content->bRestricted;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsCDSRecordingGetObjectType( jlabsCDSRecordingHandle identifier, const char **retval )
{
	OplDLNAContent *content =  static_cast<OplDLNAContent *>(identifier);
	*retval = content->szObjectType;
	return OOIF_RETURN_OK;
}
OOIFReturnCode native_jlabsCDSRecordingGetImportURI( jlabsCDSRecordingHandle identifier, const char **retval )
{
	OplDLNAContent *content =  static_cast<OplDLNAContent *>(identifier);
	*retval = content->szImportURI;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsCDSRecordingGetThumbnailURI( jlabsCDSRecordingHandle identifier, const char **retval )
{
	OplDLNAContent *content =  static_cast<OplDLNAContent *>(identifier);
	if ( 0 == strlen(content->szThumbnailURI) )
		*retval = NULL;
	else
		*retval = content->szThumbnailURI;

	return OOIF_RETURN_OK;
}
OOIFReturnCode native_jlabsCDSRecordingGetTitle( jlabsCDSRecordingHandle identifier, const char **retval )
{
	OplDLNAContent *content =  static_cast<OplDLNAContent *>(identifier);
	*retval = content->szTitle;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsCDSRecordingGetDate( jlabsCDSRecordingHandle identifier, const char **retval )
{
	OplDLNAContent *content =  static_cast<OplDLNAContent *>(identifier);
	*retval = content->szDate;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsCDSRecordingGetDuration( jlabsCDSRecordingHandle identifier, const char **retval )
{
	OplDLNAContent *content =  static_cast<OplDLNAContent *>(identifier);
	*retval = content->szDuration;
	return OOIF_RETURN_OK;
}
OOIFReturnCode native_jlabsCDSRecordingGetResolution( jlabsCDSRecordingHandle identifier, const char **retval )
{
	OplDLNAContent *content =  static_cast<OplDLNAContent *>(identifier);
	*retval = content->szResolution;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsCDSRecordingGetDigitalCopyControl( jlabsCDSRecordingHandle identifier, const char **retval )
{
	OplDLNAContent *content =  static_cast<OplDLNAContent *>(identifier);
	*retval = content->szDigitalCopyControl;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsCDSRecordingGetMediaProfile( jlabsCDSRecordingHandle identifier, const char **retval )
{
	OplDLNAContent *content =  static_cast<OplDLNAContent *>(identifier);
	*retval = content->szProfile;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsCDSRecordingGetChildCount( jlabsCDSRecordingHandle identifier, OOIFNumber *retval )
{
	OplDLNAContent *content =  static_cast<OplDLNAContent *>(identifier);
	*retval = (OOIFNumber)content->ulChildCount;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsCDSRecordingGetObjectParentId( jlabsCDSRecordingHandle identifier, const char **retval)
{
	OplDLNAContent *content =  static_cast<OplDLNAContent *>(identifier);
	*retval = content->szParentId;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsCDSRecordingGetGenre( jlabsCDSMetaHandle identifier, int *count, char ***retval)
{
	OplDLNAContent *content = static_cast<OplDLNAContent *>(identifier);

	const char * delim = ",";

	int len_array = 0, i = 0, token_count = 1;
	char *array_buf, *array_ptr, *arg1_tmp_buf, *token, **ret;

	*count = 0;
	*retval = NULL;

	len_array = strlen(content->szGenre);

	if(len_array == 0)
	{
		*count = i;
		*retval = NULL;
		return OOIF_RETURN_VALUE_NULL;
	}

	array_buf = (char *) HLIB_STD_MemAlloc( (len_array + 1) * sizeof(char) );
	strncpy ( array_buf, content->szGenre, len_array);
	array_ptr = array_buf;

	while (*array_ptr != 0)
	{
		if (*array_ptr == *delim)
			token_count++;
		array_ptr++;
	}

	if (token_count > 0)
		ret =(char **) HLIB_STD_MemAlloc(sizeof(char *) * token_count );

	token = strtok_r(array_buf, delim, &arg1_tmp_buf);
	while (token != NULL)
	{
		ret[i] = HLIB_STD_StrDup(token);
		i++;
		token = strtok_r (NULL, delim, &arg1_tmp_buf);
	}

	HLIB_STD_MemFree(array_buf);
	*count = i;
	*retval = ret;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsCDSRecordingGetChannelName( jlabsCDSMetaHandle identifier, const char **retval )
{
	OplDLNAContent *content = static_cast<OplDLNAContent *>(identifier);
	*retval = content->szChannelName;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsCDSRecordingGetChannelNumber( jlabsCDSMetaHandle identifier, const char **retval )
{
	OplDLNAContent *content = static_cast<OplDLNAContent *>(identifier);
	*retval = content->szChannelNr;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsCDSRecordingGetRadioBand( jlabsCDSMetaHandle identifier, const char **retval )
{
	OplDLNAContent *content = static_cast<OplDLNAContent *>(identifier);
	*retval = content->szRadioBand;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsCDSRecordingGetAnalogCopyControl( jlabsCDSMetaHandle identifier, const char **retval )
{
	OplDLNAContent *content = static_cast<OplDLNAContent *>(identifier);
	*retval = content->szAnalogCopyControl;
	return OOIF_RETURN_OK;
}

jlabsCDSMetaHandle native_jlabsCDSRecordingGetMetaData( jlabsCDSRecordingHandle identifier )
{
	if ( NULL == identifier )
	{
		return NULL;
	}

	OplDLNAContent *content =  static_cast<OplDLNAContent *>(identifier);
	return (jlabsCDSMetaHandle)(&content->stMetaData);
}

OOIFReturnCode native_jlabsCDSRecordingFlagUseMetaData( jlabsCDSMetaHandle identifier )
{
	OplDLNAContent *content = static_cast<OplDLNAContent *>(identifier);
	(void )(content);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsCDSRecordingReleaseHandle( jlabsCDSRecordingHandle id )
{
	OplDLNAContent *content =  static_cast<OplDLNAContent *>(id);
	if ( NULL != content )
	{
		delete content;
		content = NULL;
	}
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsCDSRecordingGetParentalRatings(jlabsCDSRecordingHandle identifier, int *count, ParentalRatingHandle **handles)
{

	__TRACE();

	OplDLNAContent      *item=  static_cast<OplDLNAContent *>(identifier);

	int i = 0;
	int	cnt = item->ulPRNum;

#if 0
	if(cnt == 0)
		return OOIF_RETURN_VALUE_UNDEFINED;
#endif

	OPL_ParentalRating_t    *parentalRating;
	ParentalRatingHandle    *myHandles = (ParentalRatingHandle*)HLIB_STD_MemAlloc(sizeof(ParentalRatingHandle) * cnt);

	for(i=0; i<cnt; i++)
	{
		parentalRating = OPL_PCMAN_PRC_AddParentalRating(NULL, NULL, NULL, item->stParentalRating[i].ucParentalRating, 0, NULL);
		myHandles[i] = (ParentalRatingHandle)parentalRating;
	}

	*handles = myHandles;
	*count = cnt;

	return OOIF_RETURN_OK;
}


OOIFReturnCode native_jlabsCDSRecordingGetBookmarks(jlabsCDSRecordingHandle identifier, int *count, BookmarkHandle **handles)
{
	__TRACE();
	OplDLNAContent      *item=  static_cast<OplDLNAContent *>(identifier);
	OPL_Collection_t    *collection = item->bookmarks;

	if (NULL == collection)
	{
		return OOIF_RETURN_VALUE_NULL;
	}

	*handles = (BookmarkHandle *)OPL_Collection_Dump(collection, count, NULL);

	if (*handles == NULL)
	{
		*handles = (BookmarkHandle *)HLIB_STD_MemAlloc(strlen("will be auto-freed"));
		*count = 0;
	}

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsCDSRecordingGetTotalMatchs( jlabsCDSRecordingHandle identifier, int *cnt )
{
	if ( NULL == identifier )
	{
		return OOIF_RETURN_VALUE_NULL;
	}

	OplDLNAContent *content =  static_cast<OplDLNAContent *>(identifier);

	*cnt = content->ulTotalMatches;

	return OOIF_RETURN_OK;
}

#endif
