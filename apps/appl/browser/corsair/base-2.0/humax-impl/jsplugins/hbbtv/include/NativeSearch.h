/* -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#if defined(OIPF) || defined(HBBTV_1_2_1)
#ifndef __NATIVE_SEARCH
#define __NATIVE_SEARCH

#include "NativeOOIFTypes.h"
#include "NativeChannel.h"
#include "NativeParentalControlManager.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * \defgroup Metadata Metadata
 * @{
 */


/**
   Opaque pointer to an oipfSearchManager instance in the native platform
*/
typedef void *SearchManagerHandle;

/* A CODHandle is the same regardless of which COD* class an object belongs */
typedef void *QueryHandle;
typedef void *MetadataSearchHandle;
typedef void *SearchResultsHandle;

/*------------------------------------------
  SearchManager
  ------------------------------------------*/

typedef enum {
	SEARCH_PROGRAMME_ITEM,
    SEARCH_CHANNEL_ITEM,
    SEARCH_FILE_ITEM,
    SEARCH_RECORDING_ITEM
} SearchItemType;

/**
   SearchManager event types.
*/
typedef enum {
	/** This event corresponds to the onMetadataSearch event defined in DAE1.2 7.12.5 / DAE2.1 7.12.1.2 */
	SEARCH_MANAGER_EVENT_METADATA_SEARCH = 0
#ifdef OIPF
	,
	/** This event corresponds to the onMetadataUpdate event defined in DAE1.2 7.12.6 / DAE2.1 7.12.1.2 */
	SEARCH_MANAGER_EVENT_METADATA_UPDATE = 1
#endif /* OIPF */
} SearchManagerEvent;

/**
	Union used to pass event context info in an event callback. What
	attribute to use is decided by the event type defined by the
	#SearchManagerEvent.
*/
typedef union
{
	/** used for SEARCH_MANAGER_EVENT_METADATA_SEARCH as described in DAE1.2 7.12.5 / DAE2.1 7.12.1.2 */
	struct {
		OOIFNumber state;
		MetadataSearchHandle search;
	} metadata_search_data;
#ifdef OIPF
	/** used for SEARCH_MANAGER_EVENT_METADATA_UPDATE as described in DAE1.1 7.12.6 / DAE2.1 7.12.1.2 */
	struct {
		OOIFNumber action;
		OOIFNumber info;
		void* object;
		SearchItemType type;
	} metadata_update_data;
#endif /* OIPF */
} SearchManagerEventInfo;

/**
   The callback function that the native platform should call to
   dispatch a SearchManager related event. The OOIF plugin will call the
   registered listeners on the all SearchManager instances that the
   reference point out.
   \param event The event type
   \param info Event context info
   \param id SearchManager instance identifier
   @see native_SearchManagerSetListener
 */
typedef void (searchManagerListener)(SearchManagerEvent event, SearchManagerEventInfo *info, SearchManagerHandle id);

/**
   Registers new listener on SearchManager related events.
   \param listener The listener
   \return OOIF_RETURN_OK
 */
OOIFReturnCode native_SearchManagerSetListener(searchManagerListener listener);

/**
   Creates a new SearchManager instance in the native platform. The
   returned handle will be used as unique identifier for the
   SearchManager instance in the OOIF plugin.
   \param id Reference to the new instance
   \param window_identifier The identifier of the window the create
   call originates from
   \param host_uri The host URI of the application the create call
   originates from
   \return OOIF_RETURN_OK, or an OOIF error return code
*/
OOIFReturnCode native_SearchManagerCreate(SearchManagerHandle *id, long int window_identifier, const char * host_uri);

/**
   Called when the SearchManager instance is destroyed in the OOIF plugin.
   \param id SearchManager instance identifier
   \return OOIF_RETURN_OK
 */
OOIFReturnCode native_SearchManagerReleaseHandle(SearchManagerHandle id);

/**
 * Called when an event listener is added, removed, or accessed from Javascript
 * on a SearchManager Object.  OOIF will handle it automatically, so no action
 * is needed from the integration layer. Nonetheless, this callback gives the
 * opportunity to prevent the listener from being added, removed, or accessed,
 * by returning an error code. This may be used to implement security checks,
 * as defined in section HbbTV 11.1, or capability checks as defined in DAE 7
 * and DAE 9.3
 * \param id Reference to the SearchManager instance to which an event listener
 * is being added, removed, or accessed.
 * \param eventName the name of the event
 * \return OOIF_RETURN_OK, or OOIF_RETURN_SECURITY_ERROR, or OOIF_RETURN_TYPE_ERROR
 */
OOIFReturnCode native_SearchManagerOnListenerAccessed(SearchManagerHandle id, const char* eventName);

#ifdef OIPF
/**
   Gets OIPF property "guideDaysAvailable" as defined in DAE1.1 7.12.1.2 / DAE2.1 7.12.1.3.
   \param id SearchManager instance identifier
   \param retval guide days available
   \return OOIF_RETURN_OK
*/
OOIFReturnCode native_SearchManagerGuideDaysAvailable(SearchManagerHandle id, OOIFNumber *retval);
#endif /* OIPF */

/**
   This function corresponds to the OIPF function "createSearch(...)" as defined in DAE1.1 7.12.1.2 / DAE2.1 7.12.1.3.
   \param id SearchManager instance identifier
   \param searchTarget The metadata that should be searched
   \param retval Handle to MetadataSearch object
   \return OOIF_RETURN_OK
*/
OOIFReturnCode native_SearchManagerCreateSearch(SearchManagerHandle id, OOIFNumber searchTarget, MetadataSearchHandle *retval);

/*------------------------------------------
  Query
  ------------------------------------------*/

/**
   Called when the Query instance is destroyed in the OOIF plugin.
   \param id Query instance identifier
   \return OOIF_RETURN_OK
 */
OOIFReturnCode native_QueryReleaseHandle(QueryHandle id);

/**
   This function corresponds to the OIPF function "and(...)" as defined in DAE1.1 7.12.3.2 / DAE2.1 7.12.3.1.
   \param q1 The first predicate for the AND operation
   \param q1 The second predicate for the AND operation
   \param retval The result of the AND operation
   \return OOIF_RETURN_OK
*/
OOIFReturnCode native_QueryAnd(QueryHandle q1, QueryHandle q2, QueryHandle *retval);

/**
   This function corresponds to the OIPF function "or(...)" as defined in DAE1.1 7.12.3.2 / DAE2.1 7.12.3.1.
   \param q1 The first predicate for the OR operation
   \param q1 The second predicate for the OR operation
   \param retval The result of the OR operation
   \return OOIF_RETURN_OK
*/
OOIFReturnCode native_QueryOr(QueryHandle q1, QueryHandle q2, QueryHandle *retval);

/**
   This function corresponds to the OIPF function "not(...)" as defined in DAE1.1 7.12.3.2 / DAE2.1 7.12.3.1.
   \param q The predicate for the NOT operation
   \param retval The result of the NOT operation
   \return OOIF_RETURN_OK
*/
OOIFReturnCode native_QueryNot(QueryHandle q, QueryHandle *retval);

/*------------------------------------------
  MetadataSearch
  ------------------------------------------*/

/**
   Called when the MetadataSearch instance is destroyed in the OOIF plugin.
   \param id MetadataSearch instance identifier
   \return OOIF_RETURN_OK
 */
OOIFReturnCode native_MetadataSearchReleaseHandle(MetadataSearchHandle id);

/**
   Gets OIPF property "searchTarget" as defined in DAE 7.12.2.1.
   \param identifier MetadataSearch instance identifier
   \param retval search target value
   \return OOIF_RETURN_OK
*/
OOIFReturnCode native_SearchGetSearchTarget(MetadataSearchHandle identifier, OOIFNumber *retval);

/**
   Gets OIPF property "result" as defined in DAE 7.12.2.1.
   \param identifier The Metadata search handle
   \param retval The search result object
*/
OOIFReturnCode native_SearchGetResult(MetadataSearchHandle identifier, SearchResultsHandle *retval);

/**
   This function corresponds to the OIPF function "setQuery(...)" as defined in DAE 7.12.2.2
   \param identifier The Metadata search handle
   \param query as defined in DAE 7.12.2.2
   \return OOIF_RETURN_OK
*/
OOIFReturnCode native_SearchSetQueryF(MetadataSearchHandle identifier, QueryHandle query);

#ifdef OIPF
/**
   Sets OIPF property "query" as defined in DAE 7.12.2.1.
   \param identifier The Metadata search handle
   \param query The query object to be set
   \return OOIF_RETURN_OK
*/
OOIFReturnCode native_SearchSetQuery(MetadataSearchHandle identifier, QueryHandle query);

/**
   Gets OIPF property "query" as defined in DAE 7.12.2.1.
   \param identifier The Metadata search handle
   \param retval the query object
   \return OOIF_RETURN_OK
*/
OOIFReturnCode native_SearchGetQuery(MetadataSearchHandle identifier, QueryHandle *retval);

/**
   This function corresponds to the OIPF function "orderBy(...)" as defined in DAE 7.12.2.2
   \param identifier The Metadata search handle
   \param field the name of the field by which results SHOULD be sorted. Might be NULL, this
	corresponds to JavaScript's null.
   \param ascending Flag indicating whether the results SHOULD be returned in
	ascending or descending order
   \return OOIF_RETURN_OK
*/
OOIFReturnCode native_SearchOrderBy(MetadataSearchHandle identifier, const char *field, OOIFBoolean ascending);

/**
   This function corresponds to the OIPF function "addRatingConstraint(...)" as defined in DAE 7.12.2.2
   \param identifier The Metadata search handle
   \param The parental rating scheme upon which the constraint SHALL be
	based. Value equal to 0 means that scheme is NULL
   \param has_threshold Indicates whether threshold parameter is null in the JavaScript environment
   \param threshold The threshold above which results SHALL NOT be returned
   \return OOIF_RETURN_OK
*/
OOIFReturnCode native_SearchAddRatingConstraint(MetadataSearchHandle identifier, ParentalRatingSchemeHandle scheme, OOIFBoolean has_threshold, OOIFNumber threshold);

/**
   This function corresponds to the OIPF function "addCurrentRatingConstraint(...)" as defined in DAE 7.12.2.2.
   \param identifier The Metadata search handle
   \return OOIF_RETURN_OK
*/
OOIFReturnCode native_SearchAddCurrentRatingConstraint(MetadataSearchHandle identifier);

/**
   This function corresponds to the OIPF function "addChannelConstraint(...)" as defined in DAE 7.12.2.2.
   \param identifier The Metadata search handle
   \param channel The channel from which results SHALL be returned,
	channel equal to 0 means that channel is NULL
   \return OOIF_RETURN_OK
*/

#endif // OIPF
OOIFReturnCode native_SearchAddChannelConstraint(MetadataSearchHandle identifier, ChannelHandle channel);

/**
   This function corresponds to the OIPF property "createQuery(...)" as defined in DAE 7.12.2.2.
   \param field The name of the field to compare
   \param comparison The type of comparison
   \param value The value to check
   \param retval handle to the query instance
   \return OOIF_RETURN_OK
*/
OOIFReturnCode native_SearchCreateQuery(MetadataSearchHandle identifier, const char *field, OOIFNumber comparison, const char *value, QueryHandle *retval);

#ifdef HBBTV_1_2_1
/**
   This function corresponds to the OIPF property "findProgrammesFromStream(...)" as defined in DAE 7.12.2.2.
   \param identifier The Metadata search handle
   \param channel The channel for which programme information should be found
   \param has_startTime Indicates whether startTime parameter is null in the JavaScript environment
   \param startTime The start of the time period for which results should be returned
	measured in seconds since midnight (GMT) on 1/1/1970.
	Otherwise, results are available immediately
   \return OOIF_RETURN_OK
*/
OOIFReturnCode native_SearchFindProgrammesFromStream(MetadataSearchHandle identifier, ChannelHandle channel, OOIFBoolean has_startTime, OOIFNumber startTime);
#else
/**
   This function corresponds to the OIPF property "findProgrammesFromStream(...)" as defined in DAE 7.12.2.2.
   \param identifier The Metadata search handle
   \param channel The channel for which programme information should be found
   \param has_startTime Indicates whether startTime parameter is null in the JavaScript environment
   \param startTime The start of the time period for which results should be returned
	measured in seconds since midnight (GMT) on 1/1/1970.
   \param count The number of programmes for which information should be returned
   \param retval If false, then results are not available until the notification events have been returned.
	Otherwise, results are available immediately
   \return OOIF_RETURN_OK
*/
OOIFReturnCode native_SearchFindProgrammesFromStream(MetadataSearchHandle identifier, ChannelHandle channel, OOIFBoolean has_startTime, OOIFNumber startTime, OOIFNumber count, OOIFBoolean *retval);
#endif

/* ------------------------------------------
  SearchResults
  ------------------------------------------*/

/**
   Called when the SearchResults instance is destroyed in the OOIF plugin.
   \param id SearchResults instance identifier
   \return OOIF_RETURN_OK
 */
OOIFReturnCode native_SearchResultsReleaseHandle(SearchResultsHandle id);

/**
   Gets OIPF property "length" as defined in DAE 7.12.4.1.
   \param identifier the handle of search result object
   \param retval the number
   \return OOIF_RETURN_OK
*/
OOIFReturnCode native_SearchResultsGetLength(SearchResultsHandle identifier, OOIFNumber *retval);

/**
   Gets OIPF property "offset" as defined in DAE 7.12.4.1.
   \param identifier the handle of search result object
   \param retval the offset
   \return OOIF_RETURN_OK
*/
OOIFReturnCode native_SearchResultsGetOffset(SearchResultsHandle identifier, OOIFNumber *retval);

/**
   Gets OIPF property "totalSize" as defined in DAE 7.12.4.1.
   \param identifier the handle of search result object
   \param the total number of items
   \retval OOIF_RETURN_OK or OOIF_RETURN_VALUE_UNDEFINED
*/
OOIFReturnCode native_SearchResultsGetTotalSize(SearchResultsHandle identifier, OOIFNumber *retval);

/**
   This function corresponds to the OIPF function "item(...)" as defined in DAE 7.12.4.2.
   \param identifier the handle of search result object
   \param offset The index into the result set
   \param type Defines the search type (Channel or Programme)
   \param result desired item
   \retval OOIF_RETURN_OK or OOIF_RETURN_VALUE_UNDEFINED
*/
OOIFReturnCode native_SearchResultsItem(SearchResultsHandle identifier, OOIFNumber offset, SearchItemType *type, void **result);

/**
   This function corresponds to the OIPF function "getResults(...)" as defined in DAE 7.12.4.2.
   \param identifier The handle of search result object
   \param offset The number of items at the start of the result set to be skipped before data is retrieved.
   \param count The number of results to retrieve
   \param retval defines if results are available immediately  or not
   \return OOIF_RETURN_OK
*/
OOIFReturnCode native_SearchResultsGetResults(SearchResultsHandle identifier, OOIFNumber offset, OOIFNumber count, OOIFBoolean *retval);

/**
   This function corresponds to the OIPF function "abort(...)" as defined in DAE 7.12.4.2.
   \param identifier The handle of search result object
   \return OOIF_RETURN_OK
*/
OOIFReturnCode native_SearchResultsAbort(SearchResultsHandle identifier);

#ifdef OIPF
/**
   This function corresponds to the OIPF function "update(...)" as defined in DAE 7.12.4.2.
   \param identifier The handle of search result object
   \return OOIF_RETURN_OK
*/
OOIFReturnCode native_SearchResultsUpdate(SearchResultsHandle identifier);
#endif /* OIPF */

#ifdef	HMX_WEBUI
OOIFReturnCode native_SearchAddConstraint(MetadataSearchHandle identifier, const char *field, const char *value);

/**
   Gets OIPF property "id" as defined in DAE1.1 7.12.2.1.
   \param identifier MetadataSearch instance identifier
   \param retval the id
   \return OOIF_RETURN_OK
*/
OOIFReturnCode native_SearchGetId(MetadataSearchHandle identifier, OOIFNumber *retval);
#endif
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __NATIVE_SEARCH */
#endif /* OIPF || HBBTV_1_2_1 */
