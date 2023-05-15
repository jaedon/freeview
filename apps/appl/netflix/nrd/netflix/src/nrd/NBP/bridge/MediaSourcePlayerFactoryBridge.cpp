/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "MediaSourcePlayerFactoryBridge.h"

#include <nrdbase/Log.h>

#include "MediaSourcePlayerBridge.h"

using namespace netflix;
using namespace netflix::mediacontrol;


//////////
// Methods
//////////

#define MethodList(Method)                      \
    Method(createPlayer)                        \
    Method(destroyPlayer)

MethodTable(MediaSourcePlayerFactoryBridge, MethodList);

//////////
// MediaSourcePlayerFactoryBridge
//////////
MediaSourcePlayerFactoryBridge::MediaSourcePlayerFactoryBridge()
    : NfObject("mediasourceplayerfactory", NULL, 0,
               Methods::MediaSourcePlayerFactoryBridge::methods,
               Methods::MediaSourcePlayerFactoryBridge::MethodCount)
{
}

MediaSourcePlayerFactoryBridge::~MediaSourcePlayerFactoryBridge()
{
}

//////////
// shutdown
//////////
void MediaSourcePlayerFactoryBridge::shutdown()
{
    // call shutdown on all players
    std::vector<shared_ptr<MediaSourcePlayerBridge> >::iterator it;
    for (it = m_players.begin(); it != m_players.end(); ++it)
        (*it)->shutdown();
}

//////////
// findPlayer
//////////
shared_ptr<IMediaSourcePlayer> MediaSourcePlayerFactoryBridge::findPlayer(
    const std::string &path)
{
    NTRACE(TRACE_MSPBRIDGE, "findPlayer %s", path.c_str());

    // walk our player bridges looking for path
    std::vector<shared_ptr<MediaSourcePlayerBridge> >::iterator it;
    for (it = m_players.begin(); it != m_players.end(); ++it)
    {
        if ((*it)->name() == path)
        {
            NTRACE(TRACE_MSPBRIDGE, "Found MediaSourcePlayerBridge %s", path.c_str());
            return (*it)->player();
        }
    }

    Log::warn(TRACE_MSPBRIDGE, "Unable to find player: %s", path.c_str());
    return shared_ptr<IMediaSourcePlayer>();
}

//////////
//
// invoke
//
//////////
#define READ_VAR(type, name)                    \
    args.mapValue<type>(name, &ok);             \
    do {                                        \
        if (!ok) {                              \
            invalidArgumentError(method, name); \
            sendEvent("result", result);        \
            return false;                       \
        }                                       \
    } while (0)

Variant MediaSourcePlayerFactoryBridge::invoke(int method, const Variant &args)
{
    Variant result;

    // make sure we pass back the index of the caller
    const std::string index = args.mapValue<std::string>("idx");
    result["idx"] = index;
    result["success"] = false; // assume failure for now

    switch (method)
    {
#define FORWARD_METHOD(METHODNAME, ...)                            \
        case Methods::MediaSourcePlayerFactoryBridge::METHODNAME:  \
            return METHODNAME(result, method, args);

        // Forward all the methods to the same named function
        MethodList(FORWARD_METHOD);
    }

    // should never get here, but pass to default handler
    return NfObject::invoke(method, args);
}

//////////
// createPlayer
//////////
bool MediaSourcePlayerFactoryBridge::createPlayer(
    Variant &result, int method, const Variant &args)
{
    bool ok;

    // get the path of the new player
    const std::string name = READ_VAR(std::string, "name");

    NTRACE(TRACE_MSPBRIDGE, "Factory::createPlayer %s", name.c_str());

    // check to see if we already have a player created
    shared_ptr<MediaSourcePlayerBridge> player;
    std::vector<shared_ptr<MediaSourcePlayerBridge> >::const_iterator it;
    for (it = m_players.begin(); it != m_players.end(); ++it)
    {
        if ((*it)->name() == name)
        {
            NTRACE(TRACE_MSPBRIDGE, "Found existing MediaSourcePlayerBridge %s",
                   name.c_str());
            player = *it;
            break;
        }
    }

    if (!player.get())
    {
        NTRACE(TRACE_MSPBRIDGE, "Creating new MediaSourcePlayerBridge %s", name.c_str());

        // create a new player bridge
        player.reset(new MediaSourcePlayerBridge(name));
        player->setParent(shared_from_this());

        // add it to our list
        m_players.push_back(player);
    }

    // sync properties
    player->sync();

    result["success"] = true;
    sendEvent("result", result);

    return true;
}

//////////
// destroyPlayer
//////////
bool MediaSourcePlayerFactoryBridge::destroyPlayer(
    Variant &result, int method, const Variant &args)
{
    bool ok;

    // get the path of to delete
    const std::string name = READ_VAR(std::string, "name");

    NTRACE(TRACE_MSPBRIDGE, "Destroying MediaSourcePlayerBridge %s", name.c_str());

    // find the player with that path
    bool found = false;
    std::vector<shared_ptr<MediaSourcePlayerBridge> >::iterator it;
    for (it = m_players.begin(); it != m_players.end(); ++it)
    {
        if ((*it)->name() == name)
        {
            // delete this player
            (*it)->shutdown();
            m_players.erase(it);
            found = true;
            break;
        }
    }

    result["success"] = found;
    sendEvent("result", result);
    return true;
}
