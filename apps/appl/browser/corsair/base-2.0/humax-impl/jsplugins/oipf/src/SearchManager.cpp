// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#if defined(OIPF) || defined(HBBTV_1_2_1)
#include "SearchManager.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jsplugin.h"

#include "macros.h"

#include "Exception.h"
#include "MetadataSearch.h"
#include "ChannelConfig.h"
#include "MetadataSearchEvent.h"
#include "MetadataUpdateEvent.h"

#ifdef HBBTV_SECURITY
#include "NativeSecurity.h"
#endif // HBBTV_SECURITY

using namespace Ooif;

list<SearchManager*> SearchManager::instances;

SearchManager::SearchManager(SearchManagerHandle i) : WrapperObject<SearchManager>(i)
{
	classType = CLASS_SEARCHMANAGER;
	instances.push_back(this);
}

SearchManager::~SearchManager()
{
	instances.remove(this);
}

int SearchManager::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;
#if defined(HMX_SECURITY)
	NATIVE_SECURITY_CHECK(obj);
#endif
#ifdef OIPF
	NATIVE_GET_NUMBER("guideDaysAvailable", native_SearchManagerGuideDaysAvailable(identifier, &RETVAL), JSP_GET_VALUE);
#endif // OIPF

#ifdef HBBTV_SECURITY
	GET_GUARDED_SECURITY_CHECK("createSearch", CHECK_BROADCAST);
	GET_GUARDED_SECURITY_CHECK("getChannelConfig", CHECK_BROADCAST);
#endif // HBBTV_SECURITY
	GET_FUNCTION(createSearch, obj, "n", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(getChannelConfig, obj, "", JSP_GET_VALUE_CACHE);
#ifdef OIPF
	INTRINSIC_EVENT_GET(MetadataUpdate);
#endif // OIPF
	INTRINSIC_EVENT_GET(MetadataSearch);

	return EventTarget::getter(obj, name, result);
}

int SearchManager::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;

#ifdef OIPF
	SET_READONLY("guideDaysAvailable");
#endif // OIPF

#ifdef OIPF
	INTRINSIC_EVENT(MetadataUpdate);
#endif // OIPF
	INTRINSIC_EVENT(MetadataSearch);

	return JSP_PUT_NOTFOUND;
}

//static
int SearchManager::createSearch(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
                                int argc, jsplugin_value *argv, jsplugin_value *result)
{
#ifdef HBBTV_SECURITY
	CALL_SECURITY_CHECK(CHECK_BROADCAST, result);
#endif // HBBTV_SECURITY
	CHECK_ARGUMENT_COUNT(1);
	SearchManager *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, SearchManager);
#ifdef HBBTV_1_2_1
	OOIFNumber searchTarget = NUMBER_VALUE(argv[0]);

	if (searchTarget != 1)
		THROW_WRONG_ARG_ERR(this_obj, result, JSP_CALL_EXCEPTION);
#endif // HBBTV_1_2_1

	MetadataSearchHandle id;
	NATIVE_CALL(native_SearchManagerCreateSearch(instance->getIdentifier(), NUMBER_VALUE(argv[0]), &id));
	RETURN_OBJECT(this_obj, new MetadataSearch(id), result, JSP_CALL_VALUE);
}

//static
int SearchManager::getChannelConfig(jsplugin_obj *this_obj, jsplugin_obj *function_obj,
                                    int argc, jsplugin_value *argv, jsplugin_value *result)
{
#ifdef HBBTV_SECURITY
	CALL_SECURITY_CHECK(CHECK_BROADCAST, result);
#endif // HBBTV_SECURITY
	CHECK_ARGUMENT_COUNT(0);

	RETURN_OBJECT(this_obj, new ChannelConfig(DEFAULT), result, JSP_CALL_VALUE);
}

//virtual
OOIFReturnCode SearchManager::checkListenerAccessAllowed(const char* eventName)
{
#ifdef HBBTV_SECURITY
	LISTENER_ACCESS_GUARDED_SECURITY_CHECK("MetadataSearch", CHECK_BROADCAST);
#endif // HBBTV_SECURITY
	return native_SearchManagerOnListenerAccessed(identifier, eventName);
}

//static
SearchManager* SearchManager::GetInstance(SearchManagerHandle id)
{
	for (list<SearchManager*>::iterator it=instances.begin(); it!=instances.end(); it++)
	{
		if ((*it)->getIdentifier() == id)
			return *it;
	}
	return NULL;
}

//static
void SearchManager::onMetadataEvent(SearchManagerEvent event, SearchManagerEventInfo *info, SearchManagerHandle id)
{
	SearchManager* searchManager = GetInstance(id);
	if (!searchManager)
	{
		OOIF_LOG_WARNING("Unknown identifier received");
		return;
	}

	Event *e = NULL;
	switch (event)
	{
	case SEARCH_MANAGER_EVENT_METADATA_SEARCH:
	{
		e = new MetadataSearchEvent(searchManager->getHost(),
		                            info->metadata_search_data.state,
		                            info->metadata_search_data.search);
		break;
	}
#ifdef OIPF
	case SEARCH_MANAGER_EVENT_METADATA_UPDATE:
	{
		e = new MetadataUpdateEvent(searchManager->getHost(),
		                            info->metadata_update_data.action,
		                            info->metadata_update_data.info,
		                            info->metadata_update_data.object,
		                            info->metadata_update_data.type);
		break;
	}
#endif //OIPF
	default:
		OOIF_LOG_WARNING("Unknown event received");
		return;
	}
	searchManager->dispatchEvent(e);
}

//static
void SearchManager::releaseNative(SearchManagerHandle id)
{
	ENTRY;
	NATIVE_WARN_NOK(native_SearchManagerReleaseHandle(id));
}
#endif //OIPF || HBBTV_1_2_1
