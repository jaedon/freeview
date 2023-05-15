#include "oplmetadatasearchhandle.h"

#include <hlib.h>
#include <apk.h>

HxTREE_t* OplMetadataSearchHandle::map = NULL;

OplMetadataSearchHandle::OplMetadataSearchHandle(OPL_SearchTarget_e target) {
    APK_META_QUERY_Target_e eTarget = eQUERY_TARGET_UNKNOWN;

    this->query = NULL;

    switch (target) {
    case eOPL_SEARCH_TARGET_SCHEDULED: eTarget = eQUERY_TARGET_EPG; break;
    case eOPL_SEARCH_TARGET_ONDEMAND:  eTarget = eQUERY_TARGET_VOD; break;
    case eOPL_SEARCH_TARGET_PROGRAMME:
    case eOPL_SEARCH_TARGET_CHANNEL:
    case eOPL_SEARCH_TARGET_FILE:
    case eOPL_SEARCH_TARGET_RECORDING:
        break;
    default:
      HxLOG_Error("[%s:%d] Unknown Target(%d)!!\n", __HxFILE__, __HxLINE__, target);
    }

    ApkMetaQHandle_t context = APK_META_QUERY_New(eTarget);

    if (context == NULL) {
        delete this;
        HxLOG_Error("[%s:%d] Out of memory!\n", __HxFILE__, __HxLINE__);
    }
    else {
        OplSearchResultsHandle *result = new OplSearchResultsHandle(target, (OPL_HANDLE)context);
        this->result = result;
    }
}

OplMetadataSearchHandle::~OplMetadataSearchHandle() {
    delete this->result;
}

OPL_SearchTarget_e OplMetadataSearchHandle::getTarget() {
    if (this->result->target) {
        return this->result->target;
    }

    return eOPL_SEARCH_TARGET_UNKNOWN;
}

OplQueryHandle *OplMetadataSearchHandle::createQuery(const char *field, int comparison, const char *value) {

    // field == queryString
    if (comparison == (int)-1) {
        return new OplQueryHandle(field);
    }
    else {
        return new OplQueryHandle(field, comparison, value);
    }
}

OPL_HANDLE OplMetadataSearchHandle::getQuery() {
    HxLOG_Trace();

    if (this->result->context) {
        return (OPL_HANDLE)this->query;
        // return (OPL_HANDLE)APK_META_QUERY_GetQuery((ApkMetaQHandle_t)this->result->context);
    }

    return NULL;
}

int OplMetadataSearchHandle::getQueryType() {
    return this->query->type;
}

void OplMetadataSearchHandle::setQuery(OplQueryHandle *query) {
    HxLOG_Trace();

    this->query = query;

    if (this->result->context && !this->query->getQueryString()) {
        APK_META_QUERY_SetQuery((ApkMetaQHandle_t)this->result->context, (const APK_QUERY_t)query->handle);
    }
}

void OplMetadataSearchHandle::addRatingConstraint(void *scheme, int threshold) {
    HxLOG_Error("[%s:%d] NOT IMPLEMENTED YET.....\n", __HxFILE__, __HxLINE__);
}

void OplMetadataSearchHandle::clearRatingConstraint() {
    HxLOG_Error("[%s:%d] NOT IMPLEMENTED YET.....\n", __HxFILE__, __HxLINE__);
}

void OplMetadataSearchHandle::addTargetConstraint(const char *target) {
    HxLOG_Trace();

    if (this->result->context) {
        if (strcmp(target, "P/F") == 0)
            APK_META_QUERY_SetFilter((ApkMetaQHandle_t)this->result->context, eQUERY_FILTER_PF);
        else if (strcmp(target, "schedule") == 0)
            APK_META_QUERY_SetFilter((ApkMetaQHandle_t)this->result->context, eQUERY_FILTER_SCH);
		else if ( strcmp(target, "ipepg") == 0)
			APK_META_QUERY_SetFilter((ApkMetaQHandle_t)this->result->context, eQUERY_FILTER_IP);
		else if ( strcmp(target, "rfepg") == 0)
			APK_META_QUERY_SetFilter((ApkMetaQHandle_t)this->result->context, eQUERY_FILTER_RF);
        else
            APK_META_QUERY_SetFilter((ApkMetaQHandle_t)this->result->context, eQUERY_FILTER_ALL);
    }
}

void OplMetadataSearchHandle::addChannelConstraint(OPL_Channel_t channel) {
    HxLOG_Trace();

    if (this->result->context && channel) {
#ifdef CONFIG_FUNC_EPG_USE_SVCUID
        APK_META_QUERY_AddSvcConstraint((ApkMetaQHandle_t)this->result->context, (HINT32)OPL_Channel_GetUniqueID(channel));
#else
        APK_META_QUERY_AddSvcConstraint((ApkMetaQHandle_t)this->result->context, OPL_Channel_GetOrgNetworkID (channel), OPL_Channel_GetTransponderID (channel), OPL_Channel_GetServiceID (channel));
#endif
    }
}

void OplMetadataSearchHandle::clearChannelConstraint() {
    HxLOG_Trace();

    if (this->result->context) {
        APK_META_QUERY_ClearSvcConstraint((ApkMetaQHandle_t)this->result->context);
    }
}

void OplMetadataSearchHandle::orderBy(const char *field, OPL_BOOL ascending) {
    HxLOG_Trace();
    HxLOG_Assert(this);

    if (this->result->context) {
        APK_META_QUERY_OrderBy((ApkMetaQHandle_t)this->result->context, field, ascending ? TRUE : FALSE);
    }
}

OplSearchResultsHandle *OplMetadataSearchHandle::getResults() {
    HxLOG_Trace();
    return this->result;
}

void OplMetadataSearchHandle::abort() {
    HxLOG_Trace();

    if (this->result->context == NULL) {
        return;
    }

    (void) APK_META_QUERY_Abort((ApkMetaQHandle_t)this->result->context);
}

void OplMetadataSearchHandle::setSearchManager(OPL_HANDLE searchmanager){
    HxLOG_Trace();

 	this->searchmanager = searchmanager;
}

OPL_HANDLE OplMetadataSearchHandle::getSearchManager( void ){
    HxLOG_Trace();

 	return this->searchmanager;
}

