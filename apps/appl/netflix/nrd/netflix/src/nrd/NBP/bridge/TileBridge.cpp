/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "TileBridge.h"

#include <nrdbase/Configuration.h>
#include <integration/Tile.h>
#include <integration/Splash.h>

using namespace netflix;

#define TilePropertyList(Property)              \
    Property(errorCodes, NfObject::ReadOnly)    \
    Property(version, NfObject::ReadOnly)

PropertyTable(TileBridge, TilePropertyList, properties)

#define MethodList(Method)                      \
    Method(setSplash)                           \
    Method(setTiles)                            \
    Method(validateSplash)                      \
    Method(validateTiles)

MethodTable(TileBridge, MethodList)

TileBridge::TileBridge()
: NfObject("tile", Properties::TileBridge::properties, Properties::TileBridge::PropertyCount,
           Methods::TileBridge::methods, Methods::TileBridge::MethodCount),
    mIntegrationWriter(new IntegrationWriter(Configuration::ilDirectory(), Configuration::ilSecret())),
    mIntegrationWriterMutex(INTEGRATIONWRITER_MUTEX, "IntegrationWriter::mutex")
{
}

void TileBridge::init()
{
    startEventListener();
}

bool TileBridge::getProperty(int property, Variant *data) const
{
    switch (property) {
    case Properties::TileBridge::version:
        *data = mIntegrationWriter->version();
        return true;
    case Properties::TileBridge::errorCodes: {
#define DEFINE_ERROR_CODE(code) (*data)[#code] = IntegrationWriter::code;
#include <nrd/IntegrationWriterErrorCodesInternal.h>
#undef DEFINE_ERROR_CODE
        return true; }
    }

    return false;
}

Variant TileBridge::invoke(int method, const Variant &var)
{
    Variant ret;
    ret["id"] = var.mapValue<int>("id");

    switch (method) {
    case Methods::TileBridge::setTiles: {
        const Variant::StringMapIterator it = var.find("tiles");
        if (it == var.stringMapEnd()) {
            invalidArgumentError(method, "tiles");
            return false;
        }
        std::string error;
        IntegrationWriter::ErrorCode code = IntegrationWriter::Success;
        const bool success = setTiles(it->second, &code, &error);
        if (!success)
            ret["error"] = error;
        ret["errorCode"] = code;
        ret["success"] = success;
        sendEvent("setTiles", ret);
        break; }
    case Methods::TileBridge::setSplash: {
        const Variant::StringMapIterator it = var.find("splashes");
        if (it == var.stringMapEnd()) {
            invalidArgumentError(method, "splashes");
            return false;
        }
        std::string error;
        IntegrationWriter::ErrorCode code = IntegrationWriter::Success;
        const bool success = setSplashes(it->second, &code, &error);
        if (!success)
            ret["error"] = error;
        ret["success"] = success;
        ret["errorCode"] = code;
        sendEvent("setSplash", ret);
        break; }
    case Methods::TileBridge::validateSplash:
    case Methods::TileBridge::validateTiles: {
        std::string error;
        IntegrationWriter::ErrorCode code = IntegrationWriter::Success;
        {
            ScopedMutex lock(mIntegrationWriterMutex);
            if (method == Methods::TileBridge::validateTiles) {
                ret["success"] = mIntegrationWriter->validateTiles(&code, &error);
            } else {
                ret["success"] = mIntegrationWriter->validateSplashes(&code, &error);
            }
        }
        ret["errorCode"] = code;
        if (!error.empty())
            ret["error"] = error;
        sendEvent("validate", ret);
        break; }
    default:
        return NfObject::invoke(method, var);
    }
    return true;
}

bool TileBridge::setTiles(const Variant &data, IntegrationWriter::ErrorCode *code, std::string *error)
{
    if (code)
        *code = IntegrationWriter::Success;
    ScopedMutex lock(mIntegrationWriterMutex);
    if (data.isNull()) {
        return mIntegrationWriter->clearTiles(code, error);
    }
    std::vector<TileGroup> groups;

    const std::vector<Variant> groupVar = data.mapValue<std::vector<Variant> >("groups");
    groups.resize(groupVar.size());
    size_t i = 0;
    for (std::vector<Variant>::const_iterator it = groupVar.begin(); it != groupVar.end(); ++it, ++i) {
        TileGroup &group = groups[i];
        group.title = it->mapValue<std::string>("title");

        const std::vector<Variant> tilesVar = it->mapValue<std::vector<Variant> >("tiles");
        std::vector<Tile> &tiles = group.tiles;
        tiles.resize(tilesVar.size());

        size_t j = 0;
        for (std::vector<Variant>::const_iterator tit = tilesVar.begin(); tit != tilesVar.end(); ++tit, ++j) {
            assert(j < tiles.size());
            Tile &tile = tiles[j];
            tile.url = tit->mapValue<std::string>("url");
            tile.width = tit->mapValue<int>("width");
            tile.height = tit->mapValue<int>("height");
            tile.title = tit->mapValue<std::string>("title");
            tile.description = tit->mapValue<std::string>("description");
            tile.shortText = tit->mapValue<std::string>("shortText");
            tile.deepLink = tit->mapValue<std::string>("deepLink");
            tile.expiry = static_cast<time_t>(tit->mapValue<ullong>("expiry"));
        }
    }
    return mIntegrationWriter->write(groups, code, error);
}

bool TileBridge::setSplashes(const Variant &data, IntegrationWriter::ErrorCode *code, std::string *error)
{
    if (code)
        *code = IntegrationWriter::Success;
    ScopedMutex lock(mIntegrationWriterMutex);
    if (data.isNull()) {
        return mIntegrationWriter->clearSplashes(code, error);
    }
    std::vector<Splash> splashes;
    splashes.resize(data.size());

    int i = 0;
    for (std::vector<Variant>::const_iterator it = data.arrayBegin(); it != data.arrayEnd(); ++it, ++i) {
        Splash &splash = splashes[i];
        splash.width = it->mapValue<int>("width");
        splash.height = it->mapValue<int>("height");
        splash.data = it->mapValue<DataBuffer>("data").toVector();
    }
    return mIntegrationWriter->write(splashes, code, error);
}

void TileBridge::event(ApplicationEvent::Type type, const Variant &)
{
    if (type == ApplicationEvent::Event_FactoryReset) {
        mIntegrationWriter->clearSplashes(0);
        mIntegrationWriter->clearTiles(0);
    }
}
