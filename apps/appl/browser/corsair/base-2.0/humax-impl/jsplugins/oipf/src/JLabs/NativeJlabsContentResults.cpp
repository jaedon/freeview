/**************************************************************************************/
/**
 * @file NativeJlabsContentsResults.cpp
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

#include "NativeJlabsContentResults.h"
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

#define JLABS_CONTENTSEARCH_TRACE     OOIF_LOG_DEBUG("\tjlabsContentResults::%s(%d)\n", __FUNCTION__, __LINE__)

OOIFReturnCode native_jlabsContentResultsGetLength(jlabsContentResultsHandle identifier, OOIFNumber *retval)
{
	JLABS_CONTENTSEARCH_TRACE;

#if 0
	OPL_JlabsContentSearchResults_t *result = static_cast<OPL_JlabsContentSearchResults_t*>(identifier);

	if (result)
		*retval = static_cast<OOIFNumber>(result->length);
	else
#endif
	*retval = 0;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsContentResultsGetOffset(jlabsContentResultsHandle identifier, OOIFNumber *retval)
{
	JLABS_CONTENTSEARCH_TRACE;

#if 0
	OPL_JlabsContentSearchResults_t *result = static_cast<OPL_JlabsContentSearchResults_t*>(identifier);

	if (result)
		*retval = static_cast<OOIFNumber>(result->offset);
	else
#endif
	*retval = 0;

	return OOIF_RETURN_OK;
}
OOIFReturnCode native_jlabsContentResultsGetTotalSize(jlabsContentResultsHandle identifier, OOIFNumber *retval)
{
	JLABS_CONTENTSEARCH_TRACE;

#if 0
	OPL_JlabsContentSearchResults_t *result = static_cast<OPL_JlabsContentSearchResults_t*>(identifier);

	if (result)
		*retval = static_cast<OOIFNumber>(result->totalSize);
	else
#endif
	*retval = 0;

	return OOIF_RETURN_OK;
}


OOIFReturnCode native_jlabsContentResultsGetResults(jlabsContentResultsHandle identifier, OOIFNumber offset, OOIFNumber count, OOIFBoolean *retval)
{
	JLABS_CONTENTSEARCH_TRACE;

	OPL_JlabsContentSearchResults_t *result = static_cast<OPL_JlabsContentSearchResults_t*>(identifier);

	*retval = FALSE;

	if (result)
	{
#if 0
		OPL_BOOL  success = OPL_METADATASEARCH_Search(result->search);
		*retval = success ? TRUE : FALSE;

		if (OPL_METADATASEARCH_Search(result->search))
		{
			*retval = OPL_METADATASEARCH_UpdateResults(result) ? TRUE : FALSE;
		}
#endif

		result->offset = static_cast<int>(offset);
	}
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsContentResultsAbort(jlabsContentResultsHandle identifier)
{
	JLABS_CONTENTSEARCH_TRACE;

	OPL_JlabsContentSearchResults_t *result = static_cast<OPL_JlabsContentSearchResults_t*>(identifier);
	(void )(result);

#if 0
	if (result)
		OPL_METADATASEARCH_AbortResults(result);
#endif

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsContentResultsUpdate(jlabsContentResultsHandle identifier)
{
	JLABS_CONTENTSEARCH_TRACE;

	OPL_JlabsContentSearchResults_t *result = static_cast<OPL_JlabsContentSearchResults_t*>(identifier);
	(void )(result);
#if 0
	if (result)
	{
		/*OPL_BOOL success = */OPL_METADATASEARCH_UpdateResults(result);
	}
#endif

	return OOIF_RETURN_OK;

}

OOIFReturnCode native_jlabsContentResultsItem(jlabsContentResultsHandle identifier, OOIFNumber offset, void **result)
{
	JLABS_CONTENTSEARCH_TRACE;

	OPL_JlabsContentSearchResults_t *self = static_cast<OPL_JlabsContentSearchResults_t*>(identifier);
	int                 index;

	if (self == NULL)
	{
		return OOIF_RETURN_VALUE_NULL;
	}

	index  = static_cast<int>(offset);

	if (self->items == NULL)
		return OOIF_RETURN_VALUE_NULL;

	if (self->totalSize <= self->offset + index)
		return OOIF_RETURN_VALUE_NULL;

	if (self->length <= index)
		return OOIF_RETURN_VALUE_NULL;

#if 0
	switch (OPL_METADATASEARCH_GetTarget(self->search))
	{
		case eOPL_SEARCH_TARGET_SCHEDULED:
			*type   = SEARCH_PROGRAMME_ITEM;
			*result = (void *)OPL_Programme_Clone((OPL_ProgrammeHandle)OPL_Collection_ItemAt(self->items, index));
			if (*result == NULL)
				return OOIF_RETURN_VALUE_NULL;
			break;

		default:
			// only schedule....
			return OOIF_RETURN_VALUE_NULL;
	}
#endif

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsContentResultsReleaseHandle(jlabsContentResultsHandle id)
{
	JLABS_CONTENTSEARCH_TRACE;

	OPL_JlabsContentSearchResults_t *result = static_cast<OPL_JlabsContentSearchResults_t*>(id);

	(void )(result);
#if 0
	if (result)
		OPL_METADATASEARCH_ReleaseResults(result);
#endif

	return OOIF_RETURN_OK;
}



#if 0
OOIFReturnCode native_jlabsContentResultsGetlength(OOIFNumber *retval)
{
	*retval = 1;
	return OOIF_RETURN_OK;
}
OOIFReturnCode native_jlabsContentResultsGetoffset(OOIFNumber *retval)
{
	*retval = 2;
	return OOIF_RETURN_OK;
}
OOIFReturnCode native_jlabsContentResultsGettotalSize( OOIFNumber *retval)
{
	*retval = 3;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsContentResultsItem(OOIFNumber number, jlabsCDSRecordingHandle *retval)
{
	OOIF_LOG_DUMMY("native_jlabsContentResultsItem!");
	jlabsCDSRecordingHandle myHandles = (jlabsCDSRecordingHandle)HLIB_STD_MemAlloc(sizeof(jlabsCDSRecordingHandle));
	*retval = (void*)myHandles;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsContentResultsGetResults( OOIFBoolean *retval)
{
	// TODO: Dummy Implementation
	OOIF_LOG_DUMMY("native_jlabsContentResultsGetResults");
	*retval = FALSE;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsContentResultsabort()
{
	// TODO: Dummy Implementation
	OOIF_LOG_DUMMY("native_jlabsContentResultsabort");

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsContentResultsupdate()
{
	// TODO: Dummy Implementation
	OOIF_LOG_DUMMY("native_jlabsContentResultsupdate");

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsContentResultsReleaseHandle(jlabsContentResultsHandle identifier)
{
	return OOIF_RETURN_OK;
}
#endif

#endif
