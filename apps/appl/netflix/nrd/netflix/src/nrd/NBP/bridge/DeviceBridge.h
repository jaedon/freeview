/*
 *(c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */


#ifndef DEVICEBRIDGE_H
#define DEVICEBRIDGE_H

#include <nrdbase/Mutex.h>
#include <nrdbase/tr1.h>

#include <nrd/ISystem.h>
#include <nrd/NfObject.h>

namespace netflix {

class DeviceBridge : public NfObject
{
public:
    DeviceBridge();
    ~DeviceBridge();

    void init();
    void updateSDKVersion();
    void updateNetworkProperty();

    virtual bool getProperty(int index, Variant *result) const;
    virtual bool setProperty(int index, const Variant &value);
    virtual Variant invoke(int method, const Variant &args);

private:
    class EventDispatcher : public device::ISystem::EventDispatcher
    {
    public:
        EventDispatcher(const shared_ptr<DeviceBridge> &bridge)
            : mBridge(bridge)
        {}
        virtual void capabilityChanged();
        virtual void videoOutputResolutionChanged();
        virtual void screensaverStateChanged();
        virtual void languageChanged();
        virtual void networkChanged();
        virtual void commandReceived(const std::string &command);
        virtual void viewModeChanged();
        virtual void outputVolumeChanged();
        virtual void videoOutputStatusChanged();
    private:
        weak_ptr<DeviceBridge> mBridge;
    };

    shared_ptr<EventDispatcher> mEventDispatcher;

    Variant mStartupTags;
};
}

#endif // DEVICEBRIDGE_H
