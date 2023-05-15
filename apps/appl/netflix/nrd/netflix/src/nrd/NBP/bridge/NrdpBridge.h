/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef NRDPBRIDGE_H
#define NRDPBRIDGE_H

#include <nrd/NfObject.h>
#include <nrd/Request.h>
#include <nrd/config.h>

namespace netflix {
class NrdApplication;

class MediaRequestBridge;
class AudioBridge;
class DeviceBridge;
class InstrumentationBridge;
class LogBridge;
#ifdef NRDP_ASE
class PlayerBridge;
#endif
class MediaSourcePlayerFactoryBridge;
class SecureStopBridge;
class DrmSystemBridge;
class StorageBridge;
class TextToSpeechBridge;
#ifdef NRDP_HAS_PARTNER_INTEGRATION
class TileBridge;
#endif
class WebCryptoBridge;
class WebSocketBridge;
class NetworkBridge;
class SystemBridge;
#ifdef NRDP_MDX_ENABLED
class MdxBridge;
#endif

namespace mediacontrol {
class NrdpMediaSourceManager;
}

class NrdpBridge : public NfObject
{
public:
    NrdpBridge();

    void init();
    void shutdown();

    virtual unsigned requestFlags(const Request &request) const;
    virtual bool setProperty(int index, const Variant &value);
    virtual bool getProperty(int index, Variant *variant) const;
    virtual Variant invoke(int method, const Variant &data);

    virtual void event(ApplicationEvent::Type type, const Variant &data);

    void trustStoreUpdated();

    shared_ptr<mediacontrol::NrdpMediaSourceManager> mediaSourceManager;

    shared_ptr<AudioBridge> audioBridge;
    shared_ptr<DeviceBridge> deviceBridge;
#ifdef NRDP_ASE
    shared_ptr<PlayerBridge> playerBridge;
#endif
    shared_ptr<LogBridge> logBridge;
    shared_ptr<MediaRequestBridge> mediaRequestBridge;
    shared_ptr<MediaSourcePlayerFactoryBridge> mediaSourcePlayerFactoryBridge;
    shared_ptr<SecureStopBridge> secureStopBridge;
    shared_ptr<DrmSystemBridge> drmSystemBridge;
    shared_ptr<StorageBridge> storageBridge;
    shared_ptr<InstrumentationBridge> instrumentationBridge;
    shared_ptr<TextToSpeechBridge> textToSpeechBridge;
    shared_ptr<WebCryptoBridge> webCryptoBridge;
    shared_ptr<WebSocketBridge> webSocketBridge;
    shared_ptr<NetworkBridge> networkBridge;
    shared_ptr<SystemBridge> systemBridge;
#ifdef NRDP_MDX_ENABLED
    shared_ptr<MdxBridge> mdxBridge;
#endif
#ifdef NRDP_HAS_PARTNER_INTEGRATION
    shared_ptr<TileBridge> tileBridge;
#endif
#ifdef BUILD_QA
    shared_ptr<NfObject> qaBridge;
#endif
private:
    void setSuspended(bool suspend, const Variant &data);
    std::string mSuspendReason;
    Time mSuspendTime;
};
}
#endif
