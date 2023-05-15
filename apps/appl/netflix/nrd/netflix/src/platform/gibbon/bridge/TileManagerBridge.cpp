/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "TileManagerBridge.h"
#include "GibbonConfiguration.h"

#include <iterator>

using namespace netflix::gibbon;
using namespace netflix;

#define TileManagerBridgePropertyList(Property)      \
    Property(tiles, NfObject::ReadOnly)

PropertyTable(TileManagerBridge, TileManagerBridgePropertyList, properties)

#define TileManagerBridgeMethodList(Method)     \
    Method(init)                                \
    Method(splash)

MethodTable(TileManagerBridge, TileManagerBridgeMethodList);

TileManagerBridge::TileManagerBridge()
    : NfObject("tilemanager", Properties::TileManagerBridge::properties,
               Properties::TileManagerBridge::PropertyCount,
               Methods::TileManagerBridge::methods,
               Methods::TileManagerBridge::MethodCount), mMutex(ZERO_MUTEX, "TileManagerBridge::Mutex")
{
}

bool TileManagerBridge::getProperty(int index, Variant *result) const
{
    switch(index) {
    case Properties::TileManagerBridge::tiles: {
        const TileManager::Tiles t = tiles();
        *result = Variant();
        Variant groups;
        for(std::vector<TileGroup>::const_iterator git = t.groups.begin(); git != t.groups.end(); ++git) {
            Variant group;
            group["title"] = git->title;

            const std::vector<Tile> &tiles = git->tiles;
            for(std::vector<Tile>::const_iterator tit = tiles.begin(); tit != tiles.end(); ++tit) {
                Variant tile;
                tile["url"] = tit->url;
                tile["title"] = tit->title;
                tile["description"] = tit->description;
                tile["shortText"] = tit->shortText;
                tile["deepLink"] = tit->deepLink;
                tile["height"] = tit->height;
                tile["width"] = tit->width;
                tile["expiry"] = tit->expiry;
                group["tiles"].push_back(tile);
            }
            groups.push_back(group);
        }
        *result = Variant("groups", groups);
        (*result)["state"] = static_cast<int>(t.status);
        break; }
    default:
        return false;
    }
    return true;
}

Variant TileManagerBridge::invoke(int method, const Variant &data)
{
    bool ok;
    switch(method) {
    case Methods::TileManagerBridge::init: {
        TileManager::start(Configuration::ilDirectory(), Configuration::ilSecret());
        break; }
    case Methods::TileManagerBridge::splash: {
        const int id = data.mapValue<int>("id", &ok);
        if(!ok) {
            invalidArgumentError(method, "id");
            return false;
        }

        Variant result;
        result["id"] = id;
        {
            std::vector<Splash> splashes = Splash::read(Configuration::ilDirectory(), Configuration::ilSecret());
            bool got_width, got_height;
            const int width = data.mapValue<int>("width", &got_width),
                      height = data.mapValue<int>("height", &got_height);
            if(got_width && got_height) {
                std::vector<Splash> bestSplashes;
                std::vector<Splash>::const_iterator it = Splash::findBestSplash(splashes, width, height);
                if(it != splashes.end())
                    bestSplashes.push_back(*it);
                splashes = bestSplashes;
            }
            for(std::vector<Splash>::const_iterator it = splashes.begin(); it != splashes.end(); ++it) {
                Variant splash;
                splash["data"] = it->data;
                splash["width"] = it->width;
                splash["height"] = it->height;
                result["splash"].push_back(splash);
            }
        }
        sendEvent("splash", result);
        break; }
    default:
        return false;
    }
    return Variant();
}

void TileManagerBridge::tilesChanged()
{
    propertyUpdated(Properties::TileManagerBridge::tiles);
}

void TileManagerBridge::error(const std::string &msg)
{
    Log::error(TRACE_LOG, "TileManagerBridge::error(%s)", msg.c_str());
}
