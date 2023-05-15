/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef DEVICELIBNATIVE_H_
#define DEVICELIBNATIVE_H_

/** @file DeviceLibNative.h - Reference implementation of the IDeviceLib
 * interface
 *
 * A device partner may use or modify this header and accompanying .cpp file as
 * needed.
 */

#include <nrdbase/tr1.h>
#include <nrdbase/Mutex.h>

#include <nrd/IDeviceLib.h>
#include <nrd/ISystem.h>
#include <nrd/IAudioMixerOutput.h>
#include <nrd/IDrmSystem.h>
#include <nrd/config.h>

#include "ESManagerNative.h"
#include <sstream>

#include <nrddpi/TeeApiCrypto.h>
#include <nrddpi/TeeApiStorage.h>
#include <nrddpi/TeeApiMgmt.h>

#ifndef NRDP_HAS_ESPLAYER
# error "This DPI implements IElementaryStreamPlayer but NrdLib was "    \
    "built for use with IPlaybackDevice. You need to build NrdLib "     \
    "with ESPlayer support (--nrdlib-esplayer)"
#endif

namespace netflix {
namespace device {


// Device-specific implementations other than the reference implementation will
// not necessarily need this structure. The referenceDpiConfig structure stores
// parameter values that the netflix reference implementation may read from the
// command-line to enable automated testing of the reference implementation by
// netflix-internal QA.
//
// The referenceDpiConfig structure stores the reference app's settings for the
// system capability parameters defined in the ISystem interface.  It also
// stores a few reference-app-specific configuration parameters. One instance of
// this structure is allocated by the reference app and the instance persists
// until the DeviceLibNative object is destroyed when the application exits. The
// structure is allocated in an extern c function
// DeviceLibNative.cpp:getDeviceLibOptions() and is available globally via the
// sConfiguration referenceDpiConfig* extern declared below.
//
// The referenceDpiConfig constructor sets the default values for the reference
// app's ISystem capabilities and reference-app-specific configuration
// parameters.
struct referenceDpiConfig {

#if defined(NRDP_HAS_IPV6)
    int64_t ipConnectivityMode;
#endif

    bool mgk; // true if the idFile is for Model Group Key

    std::string idFile; // Path to individualization file
    std::string pubkeyFile; // Path to appboot public key file

    // The device language code. The language code should be in BCP-47 format.
    //
    // Note: The legacy ISO-639-1 two character code or ISO-639-2 three
    //       character code are both legitimate BCP-47 code. See
    //       http://tools.ietf.org/rfc/bcp/bcp47.txt for more details.
    //
    std::string language;

#if defined(BUILD_OPENMAXIL_VIDEO_DECODER) || defined(BUILD_OPENMAXIL_AUDIO_DECODER)
    std::string omxilClock;
#endif
#if defined(BUILD_OPENMAXIL_VIDEO_DECODER)
    std::string omxilVideoDecoder, omxilVideoScheduler;
#endif
#if defined(BUILD_OPENMAXIL_AUDIO_DECODER)
    std::string omxilAudioDecoder;
#endif
#if defined(BUILD_OPENMAXIL_AUDIO_RENDERER)
    std::string omxilAudioRenderer;
#endif
#if defined(BUILD_OPENMAXIL_VIDEO_RENDERER)
    std::string omxilVideoRenderer, omxilVideoGLRenderer;
#endif

    std::string friendlyName; // The device friendly name (DLNA friendly name)

    bool hasPointer;  // "true" if the device has a pointing device connected

    bool hasKeyboard; // "true" if the device has a keyboard connected

    bool hasOnScreenKeyboard;  // "true" if the device provides a built-in
                               // on-screen keyboard for text input

    bool hasSuspend;  // "true" if the device supports suspend mode

    bool hasBackground;  // "true" if the device supports background mode

    bool hasScreensaver; // "true" if the device has a native screen saver

    bool supportFrameRateSeamlessSwitch;

    bool supportDrmPlaybackTransition; // "true" if the device can play
                                       // non-DRMed content first and can make a
                                       // transition to play DRMed content
                                       // seamlessly

    bool supportDrmToDrmTransition;
    
    bool supportMultipleDrmDecryption;

    bool supportLimitedDurationLicenses;

    bool supportSecureStop;

    bool supportDrmStorageDeletion;

    int64_t tcpReceiveBufferSize; // tcp receive buffer in bytes for video/audio
                                  // downloading sockets. 0 means using default.

    int64_t videoBufferPoolSize;  // The size in bytes of the pool of buffers
                                  // used to receive video stream data.

    int64_t audioBufferPoolSize;  // The size in bytes of the pool of buffers used
                                  // to receive audio stream data.

    bool support2DVideoResize;    // "true" if the device can resize the
                                  // video window


    bool support2DVideoResizeAnimation; // "true" if the device can resize the
                                        // video window at least 5 times a second.
    bool supportOnTheFlyAudioSwitch;

    bool supportOnTheFlyAudioCodecSwitch;

    bool supportDolbyVisionComposerControl;

    bool supportDynamicVideoPipelines;

    bool hevcEnabled;

    int64_t minAudioPtsGap;

    // These set the size of the video window in the reference
    // application. Change the values set in the constructor below to change the
    // video window size.
    int64_t videoRendererScreenWidth;
    int64_t videoRendererScreenHeight;

    bool enableSSO;     // "true" if app supports SSO, "false" otherwise
    bool enableSignup;  // "true" if app supports Signup, "false" otherwise
    std::string tokenUrl;
    std::string headerList;
    std::string pinCode;

    referenceDpiConfig() :
#if defined(NRDP_HAS_IPV6)
        // If not specified via command line, ipconnectivityMode should be selected by
        // 1. QAbridge
        // 2. streamingConfig
        //
        // This is priotized only when valid value is provided by command line option
        // default is 0 to enable this only when command line option is set
        ipConnectivityMode(0),
#endif
        mgk(true), idFile(""), pubkeyFile(""),
        language("en"),
        friendlyName("Friendly Netflix Device"),
        hasPointer(true), hasKeyboard(true),
        hasOnScreenKeyboard(false), hasSuspend(true), hasBackground(true),
        hasScreensaver(false),
        supportFrameRateSeamlessSwitch(true),
        supportDrmPlaybackTransition(true),
        supportDrmToDrmTransition(true),
        supportMultipleDrmDecryption(true),
        supportLimitedDurationLicenses(true),
        supportSecureStop(true),
        supportDrmStorageDeletion(true),
        tcpReceiveBufferSize(128 * 1024),
        videoBufferPoolSize(m_kVideoPoolSize),
        audioBufferPoolSize(m_kAudioPoolSize),
        support2DVideoResize(true),
        support2DVideoResizeAnimation(true),
        supportOnTheFlyAudioSwitch(true),
        supportOnTheFlyAudioCodecSwitch(false),
        supportDolbyVisionComposerControl(true),
        supportDynamicVideoPipelines(false),
#ifdef BUILD_VANGUARDHEVC_VIDEO_DECODER
        hevcEnabled(true),
#else
        hevcEnabled(false),
#endif
        minAudioPtsGap(0),
        videoRendererScreenWidth(1280),
        videoRendererScreenHeight(720),
        enableSSO(false),
        enableSignup(false),
        tokenUrl(""),
        headerList(""),
        pinCode("")
    { };

    std::string toString() const {
        std::stringstream data;
        data << "Individualization file: " << idFile
             << "\nAppBoot Public Key file: " << pubkeyFile
             << "\nLanguage: " << language
             << "\nFriendly Name: " << friendlyName
             << "\n\nHas Pointer: " << hasPointer
             << "\nHas Keyboard: " << hasKeyboard
             << "\nHas Suspend: " << hasSuspend
             << "\nHas Background: " << hasBackground
             << "\nHas On-Screen Keyboard: " << hasOnScreenKeyboard
             << "\nSupports 2D video window resize: " << support2DVideoResize
             << "\nSupports 2D video window resize animation: " << support2DVideoResizeAnimation
             << "\nSupports non-DRM to DRM playback transition:" << supportDrmPlaybackTransition
             << "\nSupports DRM to DRM playback transition:" << supportDrmToDrmTransition
             << "\nSupports DolbyVision Composer Control:" << supportDolbyVisionComposerControl
             << "\nSupports DynamicVideoPipelines:" << supportDynamicVideoPipelines
             << "\nVideo Buffer Pool Size: " << videoBufferPoolSize
             << "\nAudio Buffer Pool Size: " << audioBufferPoolSize
             << "\nVideo renderer screen size: " << videoRendererScreenWidth << "x" << videoRendererScreenHeight;

        return data.str();
    }

    // The size configuration for buffer pools. The optimal size configuration is
    // device specific. The settings here are just for device vendor reference.
    // Please refer to https://nrd.netflix.com/display/DOC40/AV+Buffer+Requirements
    // for detailed requirements for audio and video buffer pool sizes in different
    // configurations.


    // Temporarily increased video and audio buffer for dual video
    static const uint32_t m_kAudioPoolSize = 2252*1024;
#if defined(BUILD_VANGUARDHEVC_VIDEO_DECODER)
    static const uint32_t m_kVideoPoolSize = 90*1024*1024; //increased buffer size for 4K content requirements
#else
    static const uint32_t m_kVideoPoolSize = 32*1024*1024;
#endif
};


// Used to allow netflix-internal QA to run the netflix reference application
// with command-line options. Device-specific implementations won't necessarily
// need this.
//
// In the netflix reference application, this extern points to an object that is
// allocated in extern c function DeviceLibNative.cpp:getDeviceLibOptions() and
// lives for the lifetime of the application.
//
extern referenceDpiConfig* sConfiguration;

class FileSystem;


class DeviceLibNative : public IDeviceLib
{
public:
    DeviceLibNative();
    virtual shared_ptr<IDrmSystem> getDrmSystem();
    virtual shared_ptr<ISystem> getSystem();
    virtual shared_ptr<IWebCrypto> getWebCrypto();
    virtual shared_ptr<esplayer::IElementaryStreamManager> getESManager();
    virtual shared_ptr<IBufferManager> getBufferManager();
    virtual shared_ptr<IAudioMixer> getAudioMixer();
    virtual shared_ptr<ITextToSpeech> getTextToSpeech();

    virtual ~DeviceLibNative();

private:
    Mutex mutex_;
    shared_ptr<IDrmSystem> theDrmSystem_;
    shared_ptr<FileSystem> theSystem_;
    shared_ptr<IWebCrypto> theWebCrypto_;
    shared_ptr<esplayer::ESManagerNative> theElementaryStreamManager_;
    shared_ptr<IBufferManager> theBufferManager_;
    shared_ptr<IAudioMixer> theMixer_;
    shared_ptr<IAudioMixerOutput> theMixerOutput_;
    shared_ptr<TeeApiCrypto> teeCryptoPtr_;
    shared_ptr<TeeApiStorage> teeStoragePtr_;
    shared_ptr<TeeApiMgmt> teeMgmtPtr_;
    shared_ptr<ITextToSpeech> textToSpeechPtr_;
};

}}

#endif /* DEVICELIBNATIVE_H_ */
