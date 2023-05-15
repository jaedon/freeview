/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "MdxNetworkManager.h"
#include <nrdbase/ScopedMutex.h>
#include "MdxConfiguration.h"
#include "MdxLog.h"
#include <nrd/NrdApplication.h>

using namespace std;
using namespace netflix;
using namespace netflix::device;
using namespace netflix::mdx;

bool interfaceConnected(const ISystem::NetworkInterface &iface)
{
    // CE devices may not be able to detect internetConnected, so
    // isConnected is true if linkConnected is connected and
    // internetConnected is unknown or connected
    return ((iface.linkConnected == CONN_STATE_CONNECTED) &&
            (iface.internetConnected == CONN_STATE_UNKNOWN || iface.internetConnected == CONN_STATE_CONNECTED));
}

bool isValidMdxInterface(const ISystem::NetworkInterface &iface)
{
    // if mdx::Configuration has specified an interface name, only allow the use
    // of that interface
    const string &configuredMdxInterface = netflix::mdx::Configuration::getInterface();
    if(configuredMdxInterface != "default")
    {
        if(configuredMdxInterface != iface.name)
        {
            MDXTRACE("MdxNetworkManager: Skipping %s", iface.name.c_str());
            // not allowed or configured
            return false;
        }
        else
        {
            MDXTRACE("MdxNetworkManager: Found %s", iface.name.c_str());
            return true;
        }
    }

    // not allowed on mobile or other type interfaces
    if((iface.physicalLayerType != PHYSLAYER_TYPE_WIFI) && (iface.physicalLayerType != PHYSLAYER_TYPE_WIRED))
    {
        MDXTRACE("MdxNetworkManager: %s is not WIFI or WIRED", iface.name.c_str());
        // not the right type
        return false;
    }

    vector<string> ignoredAddresses;
    vector<string>::const_iterator ignoredIter;
    ignoredAddresses.push_back("0.0.0.0");
    ignoredAddresses.push_back("169.254.");
    ignoredAddresses.push_back("127.0.0.1");

    // Ensure the IP address is valid
    for(ignoredIter = ignoredAddresses.begin(); ignoredIter != ignoredAddresses.end(); ++ignoredIter)
    {
        if(!iface.ipAddress.length() ||
           iface.ipAddress.compare(0, ignoredIter->length(), *ignoredIter) == 0)
        {
            MDXTRACE("MdxNetworkManager: skipping %s with ip address: '%s'", iface.name.c_str(), iface.ipAddress.c_str());
            // not a good IP address
            return false;
        }
    }

    return true;
}

MdxNetworkManager::MdxNetworkManager()
    : mMutex(MDXNETWORKMANAGER_MUTEX, "MdxNetworkManager")
{
}

MdxNetworkManager::~MdxNetworkManager()
{
    stop();
}

MdxErrorCode MdxNetworkManager::start(shared_ptr<IMdx> mdx)
{
    ScopedMutex lock(mMutex);
    NrdApplication *app = nrdApp();

    if (!app || !mdx.get()) {
        return MdxError_BadParameter;
    }

    if(mMdx.get()) {
        // duplicate start?
        return MdxError_BadCall;
    }

    mMdx = mdx;

    // add this listener so it can be called when the network changes
    app->addSystemListener(shared_from_this());

    if(findInterface(mChosenInterface))
    {
        notifyChanged(mChosenInterface);
    }

    return MdxError_OK;
}

void MdxNetworkManager::stop()
{
    ScopedMutex lock(mMutex);

    mMdx.reset();
}

void MdxNetworkManager::networkChanged()
{
    ScopedMutex lock(mMutex);
    shared_ptr<netflix::device::ISystem> system = nrdApp()->getSystem();

    if(!system)
    {
        return;
    }

    std::vector<struct ISystem::NetworkInterface> ifMap = system->getNetworkInterfaces();
    std::vector<struct ISystem::NetworkInterface>::const_iterator iter = ifMap.begin();

    for(; iter != ifMap.end(); ++iter)
    {
        if(iter->name == mChosenInterface.mInterfaceName)
        {
            bool isConnected = interfaceConnected(*iter);

            if(iter->ipAddress != mChosenInterface.mIpAddress ||
               isConnected     != mChosenInterface.mConnected ||
               iter->ssid      != mChosenInterface.mSSID)
            {
                MDXTRACE("Mdx interface %s changed", iter->name.c_str());

                mChosenInterface.mInterfaceName = iter->name;
                mChosenInterface.mIpAddress     = iter->ipAddress;
                mChosenInterface.mSSID          = iter->ssid;
                mChosenInterface.mConnected     = isConnected;

                notifyChanged(mChosenInterface);
            }
            break;
        }
    }

    // if the interface we were using is disconnected, try and find another one
    if(!mChosenInterface.mConnected)
    {
        // try and find a new interface
        if(findInterface(mChosenInterface))
        {
            // if we managed to find a valid one that is connected, change to that one
            if(mChosenInterface.mConnected) {
                notifyChanged(mChosenInterface);
            }
        }
    }
}

// go through all the interfaces and watch any valid MDX interfaces
bool MdxNetworkManager::findInterface(WatchedInterface &found)
{
    ScopedMutex lock(mMutex);

    shared_ptr<netflix::device::ISystem> system = nrdApp()->getSystem();
    if (!system)
    {
        return false;
    }

    const string &configuredMdxInterface = netflix::mdx::Configuration::getInterface();
    NRDP_UNUSED(configuredMdxInterface);

    MDXTRACE("MdxNetworkManager: looking for interface %s", configuredMdxInterface.c_str());

    std::vector<struct ISystem::NetworkInterface> ifMap = system->getNetworkInterfaces();
    std::vector<struct ISystem::NetworkInterface>::const_iterator iter = ifMap.begin();
    std::vector<struct ISystem::NetworkInterface> validInterfaces;

    for(; iter != ifMap.end(); ++iter)
    {
        if(isValidMdxInterface(*iter))
        {
            bool isConnected = interfaceConnected(*iter);
            if(isConnected)
            {
                MDXTRACE("MdxNetworkManager: %s is valid", iter->name.c_str());
                validInterfaces.push_back(*iter);
            }
            else
            {
                MDXTRACE("MdxNetworkManager: %s not connected", iter->name.c_str());
            }
        }
    }

    if(!validInterfaces.size())
    {
        return false;
    }

    if(validInterfaces.size() > 1)
    {
        iter = validInterfaces.begin();

        for(; iter != validInterfaces.end(); ++iter)
        {
            if(iter->isDefault)
            {
                MDXTRACE("MdxNetworkManager: found interface with default route, %s with IP address %s",
                         iter->name.c_str(),
                         iter->ipAddress.c_str());

                found.mInterfaceName = iter->name;
                found.mIpAddress     = iter->ipAddress;
                found.mSSID          = iter->ssid;
                found.mConnected     = true;
                return true;
            }
        }
    }

    // no interface with default route found, so just return the first valid one
    iter = validInterfaces.begin();

    MDXTRACE("MdxNetworkManager: found interface %s with IP address %s",
             iter->name.c_str(),
             iter->ipAddress.c_str());

    found.mInterfaceName = iter->name;
    found.mIpAddress     = iter->ipAddress;
    found.mSSID          = iter->ssid;
    found.mConnected     = true;
    return true;
}

void MdxNetworkManager::notifyChanged(const WatchedInterface &iface)
{
    MDXTRACE("MDX interface %s changed: ipAddress = %s, ssid = %s, connected = %s",
             iface.mInterfaceName.c_str(),
             iface.mIpAddress.c_str(),
             iface.mSSID.c_str(),
             iface.mConnected ? "true" : "false");

    mMdx->mdxAddInterfaceName(iface.mInterfaceName);
    mMdx->InterfaceChanged(iface.mInterfaceName,
                           iface.mConnected,
                           iface.mIpAddress,
                           iface.mSSID);
}

void MdxNetworkManager::outputVolumeChanged()
{

}
