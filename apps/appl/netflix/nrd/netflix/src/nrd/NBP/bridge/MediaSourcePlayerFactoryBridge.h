/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef __MEDIASOURCEPLAYER_FACTORY_BRIDGE_H__
#define __MEDIASOURCEPLAYER_FACTORY_BRIDGE_H__

#include <nrd/NfObject.h>

namespace netflix {

namespace mediacontrol {
class IMediaSourcePlayer;
}

class MediaSourcePlayerBridge;

class MediaSourcePlayerFactoryBridge : public NfObject
{
public:
    MediaSourcePlayerFactoryBridge();
    ~MediaSourcePlayerFactoryBridge();

    void shutdown();

    shared_ptr<mediacontrol::IMediaSourcePlayer> findPlayer(const std::string &path);

public:
    virtual Variant invoke(int method, const Variant &data);

private:
    bool createPlayer(Variant &result, int method, const Variant &args);
    bool destroyPlayer(Variant &result, int method, const Variant &args);

    std::vector<shared_ptr<MediaSourcePlayerBridge> > m_players;
};

}

#endif // __MEDIASOURCEPLAYER_FACTORY_BRIDGE_H__
