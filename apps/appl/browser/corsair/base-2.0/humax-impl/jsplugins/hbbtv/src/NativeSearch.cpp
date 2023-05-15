// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#if defined(OIPF) || defined(HBBTV_1_2_1)
#include "NativeSearch.h"

#include "macros.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <oplsearchmanager.h>
#include <oplmetadatasearchhandle.h>
#include <oplqueryhandle.h>
#include <oplsearchresultshandle.h>

#define	SEARCHMANAGER_TRACE	OOIF_LOG_DEBUG("\tSearchManager::%s(%d)\n", __FUNCTION__, __LINE__)

searchManagerListener	*g_onMetaSearch;
static void	searchmanager_onSearchEvent (OPL_HANDLE search, OPL_SearchState_e oplState) 
{
    SearchManagerEventInfo info;
    OplMetadataSearchHandle *searchhandle = static_cast<OplMetadataSearchHandle*>(search);

    SEARCHMANAGER_TRACE;

    switch (oplState) {
    case eOPL_SEARCH_STATE_FINISHED:
        info.metadata_search_data.state = 0;
        info.metadata_search_data.search = searchhandle;
        // Search has finished
        if (g_onMetaSearch)
        {
            g_onMetaSearch(SEARCH_MANAGER_EVENT_METADATA_SEARCH, &info, searchhandle->getSearchManager());
        }
        break;

    case eOPL_SEARCH_STATE_MORE_DATA:
        info.metadata_search_data.state = 1;
        info.metadata_search_data.search = searchhandle;
        // More search results are available
        if (g_onMetaSearch)
        {
            g_onMetaSearch(SEARCH_MANAGER_EVENT_METADATA_SEARCH, &info, searchhandle->getSearchManager());
        }
        break;

    case eOPL_SEARCH_STATE_INVALID:
        info.metadata_search_data.state = 2;
        info.metadata_search_data.search = searchhandle;
        // The data returned by the search is no longer valid
        if (g_onMetaSearch) g_onMetaSearch(SEARCH_MANAGER_EVENT_METADATA_SEARCH, &info, searchhandle->getSearchManager());
        break;

    case eOPL_SEARCH_STATE_UPDATED:
#ifdef OIPF
        info.metadata_update_data.action = 1;
        info.metadata_update_data.info = reinterpret_cast<unsigned>(searchhandle);
        info.metadata_update_data.object = NULL;
        info.metadata_update_data.type = SEARCH_PROGRAMME_ITEM;

        //"A new version of metadata is available"
        if (g_onMetaSearch)
        {
            g_onMetaSearch(SEARCH_MANAGER_EVENT_METADATA_UPDATE, &info, searchhandle->getSearchManager());
        }
        break;
#endif
    default:
        break;
    }
}

// ---------------------------------------------------------------------------
//                                                               SEARCHMANAGER
// ---------------------------------------------------------------------------
OOIFReturnCode native_SearchManagerSetListener(searchManagerListener listener)
{
    g_onMetaSearch = listener;
    return OOIF_RETURN_OK;
}

OOIFReturnCode native_SearchManagerCreate(SearchManagerHandle *id, long int window_identifier, const char * host_uri)
{
	static int ids = 0;
	*id = (void*)ids;
	ids++;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_SearchManagerCreateSearch(SearchManagerHandle id, OOIFNumber searchTarget, MetadataSearchHandle *retval)
{
    OPL_SearchTarget_e	target;
    OPL_HANDLE search;

    OplSearchManager *searchManager = OplSearchManager::getInstance();

    SEARCHMANAGER_TRACE;
    OOIF_LOG_DEBUG("\tSearchManager::%s(%d) ---------- searchTarget = %d\n", __FUNCTION__, __LINE__, searchTarget);

    if (searchTarget == 1) {
        target = eOPL_SEARCH_TARGET_SCHEDULED;
    }
    else if (searchTarget == 2) {
        target = eOPL_SEARCH_TARGET_ONDEMAND;
    }
    else if (searchTarget == 3) {
        target = eOPL_SEARCH_TARGET_PROGRAMME;
    }
    else if (searchTarget == 4) {
        target = eOPL_SEARCH_TARGET_CHANNEL;
    }
    else if (searchTarget == 5) {
        target = eOPL_SEARCH_TARGET_FILE;
    }
    else if (searchTarget == 6) {
        target = eOPL_SEARCH_TARGET_RECORDING;
    }
    else {
        return OOIF_RETURN_ERROR;
    }

    search = searchManager->createSearch(target, id);

    if (search == NULL)
        return OOIF_RETURN_ERROR;

    searchManager->addEventListener(search, searchmanager_onSearchEvent);

    *retval = static_cast<MetadataSearchHandle>(search);
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_SearchManagerReleaseHandle(SearchManagerHandle id)
{
	OOIF_LOG_DEBUG("Relased SearchManager instance: %ld\n", reinterpret_cast<long>(id));
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_SearchManagerOnListenerAccessed(SearchManagerHandle id, const char* eventName)
{
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_MetadataSearchReleaseHandle(MetadataSearchHandle id)
{
    SEARCHMANAGER_TRACE;

    OplSearchManager *searchManager = OplSearchManager::getInstance();
    searchManager->removeSearch(static_cast<OPL_HANDLE>(id));

	return OOIF_RETURN_OK;
}

#ifdef OIPF
OOIFReturnCode native_SearchManagerGuideDaysAvailable(SearchManagerHandle id, OOIFNumber *retval)
{
    SEARCHMANAGER_TRACE;
    *retval = 7;	// TODO:

	return OOIF_RETURN_OK;
}
#endif // OIPF

// ---------------------------------------------------------------------------
//                                                                       QUERY
// ---------------------------------------------------------------------------
OOIFReturnCode native_QueryReleaseHandle(QueryHandle id)
{
    SEARCHMANAGER_TRACE;

    OplQueryHandle* query = static_cast<OplQueryHandle*>(id);
    delete query;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_QueryAnd(QueryHandle q1, QueryHandle q2, QueryHandle *retval)
{
    SEARCHMANAGER_TRACE;

    OplQueryHandle* query1 = static_cast<OplQueryHandle*>(q1);
    OplQueryHandle* query2 = static_cast<OplQueryHandle*>(q2);
    query1->_and(query2);
    *retval = (QueryHandle)query1;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_QueryOr(QueryHandle q1, QueryHandle q2, QueryHandle *retval)
{
    SEARCHMANAGER_TRACE;

    OplQueryHandle* query1 = static_cast<OplQueryHandle*>(q1);
    OplQueryHandle* query2 = static_cast<OplQueryHandle*>(q2);
    query1->_or(query2);
    *retval = (QueryHandle)query1;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_QueryNot(QueryHandle q, QueryHandle *retval)
{
    SEARCHMANAGER_TRACE;

    OplQueryHandle* query = static_cast<OplQueryHandle*>(q);
    query->_not();
    *retval = (QueryHandle)query;

	return OOIF_RETURN_OK;
}

// ---------------------------------------------------------------------------
//                                                              METADATASEARCH
// ---------------------------------------------------------------------------
#if defined(HMX_WEBUI)
OOIFReturnCode native_SearchGetId(MetadataSearchHandle identifier, OOIFNumber *retval) {
    SEARCHMANAGER_TRACE;

    *retval = reinterpret_cast<unsigned>(identifier);
    return OOIF_RETURN_OK;
}
#endif

OOIFReturnCode native_SearchGetSearchTarget(MetadataSearchHandle identifier, OOIFNumber *retval)
{
    SEARCHMANAGER_TRACE;

    OplMetadataSearchHandle *search = static_cast<OplMetadataSearchHandle*>(identifier);
    OPL_SearchTarget_e t = search->getTarget();

    switch(t) {
    case eOPL_SEARCH_TARGET_SCHEDULED:
        *retval = 1;
        break;
    case eOPL_SEARCH_TARGET_ONDEMAND:
		*retval = 2;
        break;
    case eOPL_SEARCH_TARGET_PROGRAMME:
        *retval = 3;
        break;
    case eOPL_SEARCH_TARGET_CHANNEL:
        *retval = 4;
        break;
    case eOPL_SEARCH_TARGET_FILE:
        *retval = 5;
        break;
    case eOPL_SEARCH_TARGET_RECORDING:
        *retval = 6;
        break;

    default:
        return OOIF_RETURN_ERROR;
    }

    OOIF_LOG_DEBUG("\tmetadataSearch::%s(%d) ---------- searchTarget = %d\n", __FUNCTION__, __LINE__, *retval);

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_SearchCreateQuery(MetadataSearchHandle identifier, const char *field, OOIFNumber comparison, const char *value, QueryHandle *retval) {
    SEARCHMANAGER_TRACE;

    OplMetadataSearchHandle *search = static_cast<OplMetadataSearchHandle*>(identifier);
	const char *tmp = strstr(field, ".");
	OplQueryHandle *query = NULL;

	if(tmp)
    	query = search->createQuery(++tmp, (int)comparison, value);
	else
    	query = search->createQuery(field, (int)comparison, value);

    *retval = (QueryHandle)query;

    return OOIF_RETURN_OK;
}

OOIFReturnCode native_SearchAddChannelConstraint(MetadataSearchHandle identifier, ChannelHandle channel)
{
    SEARCHMANAGER_TRACE;

    OplMetadataSearchHandle *search = static_cast<OplMetadataSearchHandle*>(identifier);

    if (NULL != channel) {
        search->addChannelConstraint((OPL_Channel_t)channel);
    }
    else {
        search->clearChannelConstraint();
    }

    return OOIF_RETURN_OK;
}

OOIFReturnCode native_SearchSetQueryF(MetadataSearchHandle identifier, QueryHandle q)
{
    SEARCHMANAGER_TRACE;

    OplSearchManager *searchManager = OplSearchManager::getInstance();
    OplMetadataSearchHandle *search = static_cast<OplMetadataSearchHandle*>(identifier);
    OplQueryHandle *query = static_cast<OplQueryHandle*>(q);
    search->setQuery(query);

	if (query->getQueryString()) {
    	searchManager->executeQuery((OPL_HANDLE)search);
	}

	return OOIF_RETURN_OK;
}

#ifdef OIPF
OOIFReturnCode native_SearchGetQuery(MetadataSearchHandle identifier, QueryHandle *retval)
{
    SEARCHMANAGER_TRACE;

    OplMetadataSearchHandle *search = static_cast<OplMetadataSearchHandle*>(identifier);
    QueryHandle query = search->getQuery();

    if (query == NULL) {
        return OOIF_RETURN_VALUE_NULL;
    }

    *retval = query;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_SearchSetQuery(MetadataSearchHandle identifier, QueryHandle q) 
{
    SEARCHMANAGER_TRACE;

    OplMetadataSearchHandle *search = static_cast<OplMetadataSearchHandle*>(identifier);
    OplQueryHandle *query = static_cast<OplQueryHandle*>(q);
    search->setQuery(query);

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_SearchOrderBy(MetadataSearchHandle identifier, const char *field, OOIFBoolean ascending)
{
    SEARCHMANAGER_TRACE;

    OplMetadataSearchHandle *search = static_cast<OplMetadataSearchHandle*>(identifier);
    search->orderBy(field, ascending ? OPL_TRUE : OPL_FALSE);

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_SearchAddCurrentRatingConstraint(MetadataSearchHandle identifier)
{
    SEARCHMANAGER_TRACE;

    // TODO:
    OplMetadataSearchHandle *search = static_cast<OplMetadataSearchHandle*>(identifier);
    search->addRatingConstraint(NULL, 0);

    return OOIF_RETURN_ERROR;
}

OOIFReturnCode native_SearchAddRatingConstraint(MetadataSearchHandle identifier, ParentalRatingSchemeHandle scheme, OOIFBoolean has_threshold, OOIFNumber threshold)
{
    SEARCHMANAGER_TRACE;

    // TODO:
    OplMetadataSearchHandle *search = static_cast<OplMetadataSearchHandle*>(identifier);
    search->addRatingConstraint(NULL, 0);

    return OOIF_RETURN_ERROR;
}
#endif // OIPF

#ifdef HBBTV_1_2_1
OOIFReturnCode native_SearchFindProgrammesFromStream(MetadataSearchHandle identifier, ChannelHandle channel, OOIFBoolean has_startTime, OOIFNumber startTime)
{
    int			length;
    char		value[40];

    SEARCHMANAGER_TRACE;

    OplMetadataSearchHandle *search = static_cast<OplMetadataSearchHandle*>(identifier);
    search->abort();

    snprintf(value, 40, "%d", static_cast<int>(startTime));

    OplQueryHandle *query = new OplQueryHandle("endTime", eOPL_QUERY_GT_OR_EQ/*>=*/, value);
    search->setQuery(query);
    delete query;
    search->addChannelConstraint((OPL_Channel_t)channel);
    length = 1;

    if (length <= 2) {
        time_t	currTime = (int)time(NULL);
        time_t	start    = static_cast<time_t>(startTime);

        if (currTime - 10 <= start && start <= currTime + 10) {
            search->addTargetConstraint("P/F");
        }
    }
	return OOIF_RETURN_OK;
}
#else
OOIFReturnCode native_SearchFindProgrammesFromStream(MetadataSearchHandle identifier, ChannelHandle channel, OOIFBoolean has_startTime, OOIFNumber startTime, OOIFNumber count, OOIFBoolean *retval)
{
    int			length;
    char		value[40];

    SEARCHMANAGER_TRACE;

    OplMetadataSearchHandle *search = static_cast<OplMetadataSearchHandle*>(identifier);
    search->abort();

    snprintf(value, 40, "%d", static_cast<int>(startTime));

    OplQueryHandle *query = new OplQueryHandle("endTime", eOPL_QUERY_GT_OR_EQ/*>=*/, value);
    search->setQuery(query);
    delete query;
    search->addChannelConstraint((OPL_Channel_t)channel);
    length = static_cast<int>(count);

    if (length <= 2) {
        time_t	currTime = (int)time(NULL);
        time_t	start    = static_cast<time_t>(startTime);

        if (currTime - 10 <= start && start <= currTime + 10) {
            search->addTargetConstraint("P/F");
        }
    }

    *retval = TRUE;
	return OOIF_RETURN_OK;
}
#endif

OOIFReturnCode native_SearchGetResult(MetadataSearchHandle identifier, SearchResultsHandle *retval)
{
    SEARCHMANAGER_TRACE;

    OplMetadataSearchHandle *search = static_cast<OplMetadataSearchHandle*>(identifier);
    OplSearchResultsHandle *result = search->getResults();
    *retval = static_cast<SearchResultsHandle>(result);

	return OOIF_RETURN_OK;
}

// ---------------------------------------------------------------------------
//                                                               SEARCHRESULTS
// ---------------------------------------------------------------------------
OOIFReturnCode native_SearchResultsGetLength(SearchResultsHandle identifier, OOIFNumber *retval)
{
    SEARCHMANAGER_TRACE;

    OplSearchResultsHandle *result = static_cast<OplSearchResultsHandle*>(identifier);
    *retval = static_cast<OOIFNumber>(result->length);

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_SearchResultsGetOffset(SearchResultsHandle identifier, OOIFNumber *retval)
{
    SEARCHMANAGER_TRACE;

    OplSearchResultsHandle *result = static_cast<OplSearchResultsHandle*>(identifier);
    *retval = static_cast<OOIFNumber>(result->offset);

	return OOIF_RETURN_OK;
}
OOIFReturnCode native_SearchResultsGetTotalSize(SearchResultsHandle identifier, OOIFNumber *retval)
{
    SEARCHMANAGER_TRACE;

    OplSearchResultsHandle *result = static_cast<OplSearchResultsHandle*>(identifier);
    *retval = static_cast<OOIFNumber>(result->getTotalSize());

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_SearchResultsGetResults(SearchResultsHandle identifier, OOIFNumber offset, OOIFNumber count, OOIFBoolean *retval)
{
    SEARCHMANAGER_TRACE;

    OplSearchResultsHandle *result = static_cast<OplSearchResultsHandle*>(identifier);

    *retval = FALSE;

    if (result) {
        result->offset = static_cast<int>(offset);
        result->count = static_cast<int>(count);

        switch (result->target) {
        case eOPL_SEARCH_TARGET_SCHEDULED:
        case eOPL_SEARCH_TARGET_ONDEMAND:
            if (result->search()) {
                *retval = result->update() ? TRUE : FALSE;
            }
            break;
		case eOPL_SEARCH_TARGET_PROGRAMME:
        case eOPL_SEARCH_TARGET_CHANNEL:
        case eOPL_SEARCH_TARGET_FILE:
        case eOPL_SEARCH_TARGET_RECORDING:
            break;
        default:
            break;
        }
    }

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_SearchResultsAbort(SearchResultsHandle identifier)
{
    SEARCHMANAGER_TRACE;

    OplSearchResultsHandle *result = static_cast<OplSearchResultsHandle *>(identifier);
    result->abort();

	return OOIF_RETURN_OK;
}

#ifdef OIPF
OOIFReturnCode native_SearchResultsUpdate(SearchResultsHandle identifier)
{
    SEARCHMANAGER_TRACE;

    OplSearchResultsHandle *result = static_cast<OplSearchResultsHandle*>(identifier);
	switch (result->target) {
    case eOPL_SEARCH_TARGET_SCHEDULED:
    case eOPL_SEARCH_TARGET_ONDEMAND:
    	result->update();
		break;
	case eOPL_SEARCH_TARGET_PROGRAMME:
    case eOPL_SEARCH_TARGET_CHANNEL:
    case eOPL_SEARCH_TARGET_FILE:
    case eOPL_SEARCH_TARGET_RECORDING:
       	break;
	default:
		break;
	}

	return OOIF_RETURN_OK;

}
#endif // OIPF

OOIFReturnCode native_SearchResultsItem(SearchResultsHandle identifier, OOIFNumber offset, SearchItemType *type, void **r) {
    OplSearchResultsHandle *result = static_cast<OplSearchResultsHandle *>(identifier);
    int index;

    SEARCHMANAGER_TRACE;

    if (result == NULL) {
        return OOIF_RETURN_VALUE_NULL;
    }

    index = static_cast<int>(offset);

    if (result->items == NULL) {
        return OOIF_RETURN_VALUE_NULL;
    }

    if (result->length <= index) {
        return OOIF_RETURN_VALUE_NULL;
    }

    void* orign = (void*)OPL_Collection_ItemAt(result->items, index);

    switch (result->target) {
    case eOPL_SEARCH_TARGET_SCHEDULED:
    case eOPL_SEARCH_TARGET_PROGRAMME:
		*type = SEARCH_PROGRAMME_ITEM;
        *r = (void *)OPL_Programme_Clone((OPL_ProgrammeHandle)orign);
        break;
    case eOPL_SEARCH_TARGET_CHANNEL:
		*type = SEARCH_CHANNEL_ITEM;
		*r = (void *)OPL_Channel_Clone((OPL_Channel_t)orign);
        break;
    case eOPL_SEARCH_TARGET_FILE:
        *type   = SEARCH_FILE_ITEM;
        *r = orign;
        break;
    case eOPL_SEARCH_TARGET_RECORDING:
        *type   = SEARCH_RECORDING_ITEM;
        *r = (void *)OPL_Recording_Clone((OPL_Recording_t*)orign);
        break;
    default:
        return OOIF_RETURN_VALUE_NULL;
    }

    if (*r == NULL)
        return OOIF_RETURN_VALUE_NULL;

	return OOIF_RETURN_OK;
}

OOIFReturnCode native_SearchResultsReleaseHandle(SearchResultsHandle id) {
    SEARCHMANAGER_TRACE;

    OplSearchResultsHandle* result = static_cast<OplSearchResultsHandle *>(id);
    result->release();

	return OOIF_RETURN_OK;
}
#endif // OIPF || HBBTV_1_2_1

#ifdef	HMX_WEBUI
OOIFReturnCode native_SearchAddConstraint(MetadataSearchHandle identifier, const char *field, const char *value) {
    SEARCHMANAGER_TRACE;

    if (strcmp(field, "target") == 0) {
        OplMetadataSearchHandle *search = static_cast<OplMetadataSearchHandle*>(identifier);
        search->addTargetConstraint(value);
    }

    return OOIF_RETURN_OK;
}
#endif
