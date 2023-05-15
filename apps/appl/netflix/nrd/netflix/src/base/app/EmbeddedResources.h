/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef EmbeddedResources_h
#define EmbeddedResources_h

#include <string>
#include <map>

#if NRDP_EMBEDDED_RESOURCES_USE_SIZE
#define ADD_RESOURCE(type, path, symbol)                                 \
    if(!resources::sResourceList) resources::sResourceList = new resources::ResourceTable(); \
    resources::sResourceList->insert(std::pair<std::string,resources::ResourceEntry>(std::string(path),resources::ResourceEntry(type, _binary_ ## symbol ## _start, _binary_ ## symbol ## _start + _binary_ ## symbol ## _size)));
#else
#define ADD_RESOURCE(type, path, symbol)                                 \
	if(!resources::sResourceList) resources::sResourceList = new resources::ResourceTable(); \
	resources::sResourceList->insert(std::pair<std::string,resources::ResourceEntry>(std::string(path),resources::ResourceEntry(type, _binary_ ## symbol ## _start, _binary_ ## symbol ## _end)));
#endif

namespace netflix {
namespace resources {

struct ResourceEntry {
    const char *type;
    const char* start;
    const char* end;
    ResourceEntry(const char *type, const char* start, const char* end) : type(type), start(start), end(end) { };
};

typedef std::map<std::string,ResourceEntry> ResourceTable;
typedef std::map<std::string,ResourceEntry>::const_iterator ResourceTableIterator;

extern ResourceTable *sResourceList;

const ResourceEntry *getResourceEntry(const std::string& path);
void destroyResources();
bool hasResources();

}}

#endif
