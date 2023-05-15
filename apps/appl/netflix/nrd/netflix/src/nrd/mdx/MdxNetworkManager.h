/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef __MDXNETWORKMANAGER_H__

#include <nrd/ISystem.h>
#include <nrdbase/Mutex.h>
#include "IMdx.h"
#include "MdxError.h"

namespace netflix
{
namespace mdx
{
class MdxNetworkManager : public netflix::device::ISystem::EventDispatcher, public enable_shared_from_this<MdxNetworkManager>
{
public:
    MdxNetworkManager();
    ~MdxNetworkManager();

    MdxErrorCode start(shared_ptr<IMdx> mdx);
    void stop();

    // from ISystem EventDispatcher
    void capabilityChanged(){}
    void videoOutputResolutionChanged(){}
    void screensaverStateChanged(){}
    void languageChanged(){}
    void networkChanged();
    void outputVolumeChanged();
    void videoOutputStatusChanged(){}
    void commandReceived(const std::string &) {}
    void viewModeChanged(){}

protected:

    struct WatchedInterface
    {
        WatchedInterface() : mConnected(false) {}

        std::string mInterfaceName;
        std::string mIpAddress;
        std::string mSSID;
        bool        mConnected;
    };

    bool findInterface(WatchedInterface &found);
    void notifyChanged(const WatchedInterface &iface);

    shared_ptr<IMdx> mMdx;
    WatchedInterface mChosenInterface;
    Mutex      mMutex;
};

}} // namespace netflix::mdx

#endif // __MDXNETWORKLISTENER_H__
