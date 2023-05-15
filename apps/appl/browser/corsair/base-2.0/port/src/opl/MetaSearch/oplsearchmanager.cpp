#include "oplsearchmanager.h"

#include <hlib.h>
#include <apk.h>

bool OplSearchManager::instanceFlag = false;
OplSearchManager* OplSearchManager::manager = NULL;
LIST OplSearchManager::map;
OPL_SearchListener_t OplSearchManager::onSearch;

OplSearchManager* OplSearchManager::getInstance() {
    if (!instanceFlag) {
        manager = new OplSearchManager();
        instanceFlag = true;
        return manager;
    }
    else {
        return manager;
    }
}

OPL_HANDLE OplSearchManager::createSearch(OPL_SearchTarget_e target, OPL_HANDLE searchmanager) {
    OplMetadataSearchHandle *search = new OplMetadataSearchHandle(target);

    HxLOG_Trace();

	search->setSearchManager( searchmanager );
    map.insert(map.end(), (OPL_HANDLE)search);
    return (OPL_HANDLE)search;
}

void OplSearchManager::removeSearch(OPL_HANDLE handle) {
    OplMetadataSearchHandle *search = static_cast<OplMetadataSearchHandle*>(handle);

    HxLOG_Trace();

    if (hasSearch(search)) {
        map.remove(handle);
        delete search;
    }
}

bool OplSearchManager::hasSearch(OplMetadataSearchHandle *search) {
    LIST::iterator i;

    for (i = map.begin(); i != map.end(); ++i) {
        if (*i == (OPL_HANDLE)search) {
            return TRUE;
        }
    }

    return FALSE;
}

void OplSearchManager::addEventListener(OPL_HANDLE handle, OPL_SearchListener_t onSearch) {
    OplMetadataSearchHandle* search = (OplMetadataSearchHandle*)handle;

    HxLOG_Trace();

    if (search) {
        this->onSearch = onSearch;
		switch (search->getTarget()) {
			case eOPL_SEARCH_TARGET_SCHEDULED:
			case eOPL_SEARCH_TARGET_ONDEMAND:
				APK_META_QUERY_SetNotifier((ApkMetaQHandle_t)search->result->context, (APK_META_QUERY_Notifier_t)onMetadataSearchEvent, (void*)search);
				break;
			default:
				break;
		}
    }
}

void OplSearchManager::executeQuery(OPL_HANDLE handle) {
    OplMetadataSearchHandle* search = (OplMetadataSearchHandle*)handle;

    HxLOG_Trace();

    if (search) {
    	APK_META_SEARCH_Exec(search->query->getQueryString(), (APK_META_Getter_t)onSmartSearchEvent, (OPL_HANDLE)search);
    }
}

void OplSearchManager::onMetadataSearchEvent(OPL_HANDLE handle, HINT32 event, const HINT32 *args, void *userdata) {
    OplMetadataSearchHandle *search = (OplMetadataSearchHandle*)userdata;
    OPL_SearchState_e state;

    if (!hasSearch(search) || search->result->context != handle) {
        HxLOG_Error("[%s:%d] hasSearch() error!\n", __HxFILE__, __HxLINE__);
        return;
    }

    search->result->update();

    switch(event) {
    case eQUERY_EVENT_MORE_DATA:
        state = eOPL_SEARCH_STATE_MORE_DATA;
        break;

    case eQUERY_EVENT_FINISHED:
        state = eOPL_SEARCH_STATE_FINISHED;
        break;

    case eQUERY_EVENT_INVALID:
        state = eOPL_SEARCH_STATE_INVALID;
        break;

    case eQUERY_EVENT_NEW_VERSION:
        state = eOPL_SEARCH_STATE_UPDATED;
        break;
    default:
        state = eOPL_SEARCH_STATE_UNKNOWN;
        return;
    }

    if (onSearch) {
        onSearch((OPL_HANDLE)search, state);
    }
}

HBOOL OplSearchManager::onSmartSearchEvent(HINT32 id, HINT32 remainingJob, const char *typeName, HxVector_t *list, void *userdata) {

    OplMetadataSearchHandle *search = (OplMetadataSearchHandle*)userdata;
    search->result->update(typeName, list);

    if (onSearch) {
        onSearch((OPL_HANDLE)search, eOPL_SEARCH_STATE_FINISHED);
    }

    if (remainingJob == 0)	{
        return TRUE;
    }

    return FALSE;
}

OplSearchManager::~OplSearchManager() {
    instanceFlag = false;
}
