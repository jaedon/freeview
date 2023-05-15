/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "EmbeddedResources.h"
#include <nrdbase/Log.h>

namespace netflix {
namespace resources {

ResourceTable *sResourceList = 0;

const ResourceEntry *getResourceEntry(const std::string& path)
{
    if(!sResourceList)
        return NULL;

    if (path.empty()) {
        Log::error(TRACE_RESOURCES, "(getResourceEntry) Called with an empty path!");
        return NULL;
    }

    ResourceTableIterator it;

    it = sResourceList->find(path);
    if (it == sResourceList->end()) {
        std::string fixedPath;
        const char *cstr = path.c_str();
        size_t last = 0;
        for(size_t i = 0; i < path.length()-1; ) {
            if(cstr[i] == '/' && cstr[i+1] == '/') {
                fixedPath.append(cstr+last, (i-last)+1);
                while(cstr[++i] == '/');
                last = i;
            } else {
                ++i;
            }
        }
        if(last != path.length())
            fixedPath.append(cstr+last);
        if(!fixedPath.empty())
            it = sResourceList->find(fixedPath);
    }

    if (it == sResourceList->end()) {
        Log::info(TRACE_RESOURCES, "(getResourceEntry) %s not found", path.c_str());
        return NULL;
    }
    else {
        NTRACE(TRACE_RESOURCES, "(getResourceEntry) %s found", path.c_str());
        return &it->second;
    }
}

bool hasResources()
{
    return sResourceList && !sResourceList->empty();
}

void destroyResources()
{
    delete sResourceList;
    sResourceList = 0;
}


}}
