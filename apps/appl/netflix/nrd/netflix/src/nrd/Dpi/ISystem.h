/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

/** @file ISystem.h
 *
 * An interface to system information used by the Netflix
 * application.  Developers will implement this interface to provide
 * capability information for their platform as well as functionality
 * for storing data in both secure and non-secure locations.  The interface
 * also enables developers to expose their device identity through various
 * methods.
 */

#ifndef NRDP_ISYSTEM_H
#define NRDP_ISYSTEM_H

#include <map>
#include <string>
#include <vector>
#include <time.h>

#include "IAudioMixer.h"
#include <nrdbase/NFErr.h>
#include <nrdbase/tr1.h>
#include <nrdbase/Thread.h>

#include <nrdnet/NetCommonDataTypes.h>
#include <nrdase/AseCommonDataTypes.h>

/** @class netflix::device::ISystem
 *  @brief The interface for describing platform capabilities
 */

namespace netflix {

namespace device {

class ISystem : public enable_shared_from_this<ISystem>
{
public:
    /** Device authentication mechanism. Use MODEL_GROUP_KEYS in almost all cases. */
    enum AuthenticationType
    {
        PRESHARED_KEYS,
        WEB,
        X509,
        ECC_LINK,
        NPTICKET,
        MODEL_GROUP_KEYS,
        UNKNOWN
    };

    /** Application States. */
    enum ApplicationState
    {
        BROWSING,
        MEDIA_LOADING,
        MEDIA_UNLOADING,
        BUFFERING,
        PLAYING,
        PAUSED,
        STOPPING
    };

    enum VideoOutputType
    {
        DIGITAL_INTERNAL,
        DIGITAL_MIRACAST,
        DIGITAL_DISPLAY_PORT,
        DIGITAL_DVI,
        DIGITAL_HDMI,
        DIGITAL_EXTERNAL_OTHER,
        ANALOG_COMPOSITE,
        ANALOG_COMPONENT,
        ANALOG_VGA,
        ANALOG_SCART,
        ANALOG_OTHER
    };

    /**
     * Possible HDCP versions.
     */
    enum HDCPVersion
    {
        HDCP_NOT_APPLICABLE = -1,
        HDCP_NOT_ENGAGED    =  0,
        HDCP_1_0            = 10,
        HDCP_1_1,
        HDCP_1_2,
        HDCP_1_3,
        HDCP_1_4,
        HDCP_2_0            = 20,
        HDCP_2_1,
        HDCP_2_2
     };

    /**
     *
     * - videoOutput: The type of video output of active link. See `VideoOutputType` for possible values.
     * - hdcpVersion: The currently engaged HDCP version. See `HDCPVersion` for possible values.
     *
     * You must report the correct HDCP version whenever there is an external
     * link established for devices that have external digital video
     * output links.
     *
     * - For devices with displays such as TVs ( have specified `VideoOutputType` as `DIGITAL_NTERNAL`),
     * return `HDCP_NOT_APPLICABLE` for `hdcpVersion`. In this case, the SDK
     * does not perform any downloadable filtering based on HDCP.
     * - For self displaying devices that have a digital video output, return the engaged HDCP
     * version. In this case, the SDK will perform downloadable filtering as well
     * as make fast playback start decisions based on HDCP version.
     * - For external streaming devices, return currently engaged HDCP
     * version. Based on engaged HDCP version, and each content's
     * resolution/HDCP mapping, SDK can filter out downloadables.
     *
     * When changed, the device should notify the SDK with the
     * `EventDispatcher` callback.
     */
    struct VideoOutputState
    {
        enum VideoOutputType videoOutput;
        enum HDCPVersion hdcpVersion;
    };

    /**
     * Used for logging purposes and device information gathering.
     *
     * - name : A name like "HDMI 1.4a", "HDMI 2.0"
     * - videooutput : Should be one a value in the `VideoOutputType` enum
     * - hdcpVersion : The maxinum HDCP protocol version that device can support. This
     *               could be different from currently engaged HDCP version
     */
    struct VideoOutputInfo
    {
        std::string name;
        enum VideoOutputType videoOutput;
        enum HDCPVersion hdcpVersion;
    };

    /**
     *  Type of audio transition profiles supported by the device.
     */
    enum TransitionProfile
    {
        /**
         * The device does not support changing media volume
         */
        TRANSITION_NONE,
        /**
         * Device can mute or set volume to 0 or 1.0
         */
        TRANSITION_MUTE,
        /**
         * Device can set volume from 0 - 1.0
         */
        TRANSITION_VOLUME,
        /**
         * Device can perform easing
         */
        TRANSITION_EASE
    };

    /**
     * The supported easing functions. `EASE_LINEAR`, `EASE_INCUBIC`, `EASE_OUTCUBIC` are required.
     */
    enum EaseProfile
    {
        EASE_LINEAR,
        EASE_INCUBIC,
        EASE_OUTCUBIC,
        EASE_COUNT
    };

    /**
     * @brief Converts a transition profile enum to a string
     *
     * @param t transition profile to convert
     * @return String equivalent of the transition profile
     */
    static std::string transitionProfileToString(TransitionProfile t) {
        switch (t) {
        case TRANSITION_NONE:   return "TRANSITION_NONE";
        case TRANSITION_MUTE:   return "TRANSITION_MUTE";
        case TRANSITION_VOLUME: return "TRANSITION_VOLUME";
        case TRANSITION_EASE:   return "TRANSITION_EASE";
        default:                return "unknown";
        }
    }

    /**
     * @brief Converts an ease profile enum to a string
     *
     * @param e ease profile to convert
     * @return String equivalent to the ease profile
     */
    static std::string easeProfileToString(EaseProfile e) {
        switch (e) {
        case EASE_LINEAR:       return "EASE_LINEAR";
        case EASE_INCUBIC:      return "EASE_IN_CUBIC";
        case EASE_OUTCUBIC:     return "EASE_OUT_CUBIC";
        default:                return "unknown";
        }
    }

    /**
     * System capability structure
     */
    struct Capability
    {
        /**
         * Constructor with suitable defaults
         */
        Capability() :
            hasPointer                         ( false ),
            hasTouchSupport                    ( false ),
            hasKeyboard                        ( false ),
            hasSuspend                         ( false ),
            hasBackground                      ( false ),
            hasOnScreenKeyboard                ( false ),
            hasScreensaver                     ( false ),
            supportFrameRateSeamlessSwitch     ( true  ),
            support2DVideoResize               ( true  ),
            support2DVideoResizeAnimation      ( false ),
            supportDrmPlaybackTransition       ( true  ),
            supportDrmToDrmTransition          ( true ),
            supportMultipleDrmDecryption       ( true  ),
            supportLimitedDurationLicenses     ( true  ),
            supportSecureStop                  ( true  ),
            supportDrmStorageDeletion          ( true  ),
            supportUiSound                     ( true  ),
            supportOnTheFlyAudioSwitch         ( true  ),
            supportOnTheFlyAudioCodecSwitch    ( false ),
            supportDolbyVisionComposerControl  ( false ),
            supportDynamicVideoPipelines       ( false ),
            supportPlayerAudioTransition       ( TRANSITION_EASE ),
            tcpReceiveBufferSize               ( 0     ),
            videoBufferPoolSize                ( 0     ),
            audioBufferPoolSize                ( 0     ),
            displayColorDepth                  ( 8     ),
            minAudioPtsGap                     ( 0     )
        {
        }

        /**
         * Specifies if the device has a pointing device connected
         */
        bool hasPointer;

        /**
         * Specifies if the device has a touchpad device connected
         */
        bool hasTouchSupport;

        /**
         * Specifies if the device has a keyboard connected
         */
        bool hasKeyboard;

        /**
         * Specifies if the device supports app suspend mode.
         */
        bool hasSuspend;

        /**
         * Specifies if the device supports app background mode.
         */
        bool hasBackground;

        /**
         * Specifies if the device provides a built-in
         *  on-screen keyboard for text input
         */
        bool hasOnScreenKeyboard;

        /**
         * Specifies if the device has a native screen saver
         */
        bool hasScreensaver;

        /**
         * Specifies if device can take streams with different framerates and play
         * and display them without re-initializing the underlying media pipeline,
         * which may cause visual artifacts like flickering.
         */
        bool supportFrameRateSeamlessSwitch;

        /**
         *  Specifies if the device resize video prior to the start of
         * playback. Must set to `true`.
         *
         * 1. The video window can be resized to an arbitrarily small width and
         * height, down to 1/8th of the original resolution.
         * 2. The resized video window can be positioned without alignment
         * restrictions. The top left corner of the window can be positioned at
         * an odd pixel offset from the left and from the top, for example.
         * 3. The window resize completes in less than 0.5 seconds.
         * 4. Video should be displayed at the correct aspect ratio in as large
         * an area as possible within the specified rectangle.  That means the
         * platform should put black bars where needed instea of cropping.
         *
         * When this is Specifies, the SDK positions
         * the window with `IPlaybackDevice::setVideoWindow()` or
         * `IPlaybackGroup::setVideoWindow()`, with a specified video window rectangle.
         * This window will:
         *
         * 1. Be given with coordinates corresponding to the full screen
         * display resolution
         *
         * 2. Always lie entirely within the video display area. There will
         * be no partially visible display window rectangles.
         *
         */
        bool support2DVideoResize;

        /** Specifies if the device can:
         *
         * 1. Smoothly animate the resizing of the video window from one
         * size to another. This means the video window can be resized and
         * respositioned from any arbitrarily sized rectangle to another at
         * least 5 times per second.
         * 2. Resize without any visual artifacts or interruption of
         * video and audio playback.
         * 3. Set supportVideoResizeDuringPlayback to also be true.  In other words, if
         * supportVideoResizeDuringPlayback is false,
         * supportVideoResizeAnimation must also be false.
         *
         * When the device sets supportVideoResizeAnimation is true the SDK will
         * position the window via IPlaybackDevice::setVideoWindow or
         * IPlaybackGroup::setVideoWindow, the specified video window rectangle
         *
         * 1) Will be given in coordinates corresponding to the full screen
         * display resolution
         *
         * 2) Will always lie entirely within the video display area. There will
         * be no partially visible display window rectangles.
         *
         * ( CE Partners: This capability is not for CE devices like TV, BD and
         * STB. For CE devices, it should be set as false)
         */
        bool support2DVideoResizeAnimation;

        /**
         * Specifies if the device can start video playback with Non-DRM content and switch
         * to DRM-protected content seamlessly. (aka, McQueen) Must set to `true`.
         *
         * When set to 'true', Netflix downloads and plays non-DRM content to speed up
         * perceived loading times. Netflix fetches the license for the content and then
         * transitions to DRM-protected content on-the-fly.
         */
        bool supportDrmPlaybackTransition;

        /**
         * Specifies if the device can seamlessly playback while there are chagnes in DRMs. With this flag true, content
         * with different licenses can be played seamlessly. For example, suppose A, B and C have different licenses
         * each. With this flag true, A -> B -> C -> B -> A -> .... (arbitrary order) playback is supported. This is
         * essential capability for features like segmented playlist between different title or per-resolution-key with
         * playready 2.5.
         */
        bool supportDrmToDrmTransition;
        
        /**
         * Specifies if the device can support multiple DRM sessions concurrently.
         * If supported, the device can fetch multiple DRM licenses and decrypt content simultaneously.
         */
        bool supportMultipleDrmDecryption;

        /**
         * Specifies if the device supports limited duration DRM licenses, which are short-period licenses for DRM-protected content.
         */
        bool supportLimitedDurationLicenses;

        /**
         * Specifies if the device supports secure stop, which provides cryptographic proof that content playback for a title has stopped.
         */
        bool supportSecureStop;

        /**

         * Specifies if the device can delete the DRM secure store.
         *
         * return true only when below 4 DPIs are supported at IDrmSystem
         * - deleteDrmStore
         * - deleteKeyStore
         * - getDrmStoreHash
         * - getKeyStoreHash
         */
        bool supportDrmStorageDeletion;

        /**
         * Specifies if the device can render UI sound. Must set to `true`.
         */
        bool supportUiSound;

        /**
         * Specifies if the device can switch to another audio track
         * on-the-fly if the audio codecs are the same.
         */
        bool supportOnTheFlyAudioSwitch;

        /**
         * Specifies if the device can switch to another audio codec on-the-fly.
         */
        bool supportOnTheFlyAudioCodecSwitch;

        /**
         * Specifies if the device can turn on and off Dolby Vision Enhancement
         * Layer composition. Use this as a helper feature to verify
         * that HDR is correctly consumed.
         */
        bool supportDolbyVisionComposerControl;

        /**
         * Specifies if the device supports dynamic video pipelines.
         *
         * There are two types of video pipelines.
         *
         * - Static Pipeline - These pipelines reserve resources at the start of playback and
         * cannot release them without restarting the decoder. For example,
         * if the decoder initializes with a 1080p maximum resolution, it uses
         * the same resources even if playing back 480p. The extra resources ARE NOT
         * available to a second decoder.
         * - Dynamic Pipeline - These pipelines can release resources during playback
         * if the bitstream changes. For example, if the decoder initializes with a 1080p maximum
         * resolution, it can release resources when switching to 480p.
         * These extra resources are available to a second decoder
        */
        bool supportDynamicVideoPipelines;

        /**
         * Sets the type of transition the media player supports as specified by the `TransitionProfile`enum.
         *
         */
        TransitionProfile supportPlayerAudioTransition;

        /**
         *  TCP receive buffer in bytes for
         *  video and audio downloading sockets. Set to `0` to use the default value.
         */
        int32_t tcpReceiveBufferSize;

        /**
         * The size in bytes of the buffer pool
         * used to receive video stream data.
         */
        int64_t videoBufferPoolSize;

        /**
         * The size in bytes of the buffer pool used to receive audio stream data.
         */
        int64_t audioBufferPoolSize;

        /**
         * The list of video content profiles that are currently available
         */
        std::vector<ContentProfile> videoProfiles;

        /**
         * The list of audio content profiles that are selectable to users.
         */
        std::vector<ContentProfile> audioProfiles;

        /**
         * The list of audio content profiles that the device will decode.
         */
        std::vector<ContentProfile> audioDecodeProfiles;

        /**
         * The list of audio eases supported.
         */
        std::vector<EaseProfile> audioEaseProfiles;

        /**
         * Indicate the color precision (bit depth) of
         * the built-in display.  When the device supports
         * decode color precision that is different than
         * the display's precision, indicate the actual
         * display precision here. For example, if the
         * device can decode 10-bit color, but downsamples
         * and dithers to render on an 8-bit display, you
         * should specify 8-bit precision here.
         */
        int displayColorDepth;

        /*
         * minimum pts gap that device can handle. In case of segmented
         * playlist, where audio samples can be overlapped, sdk removes
         * overlapped audio samples, and can cause gap in pts. If device has a
         * limitation in handling small pts gap, and require minimum gap,
         * specify it here in "milisecond"
         */
        uint32_t minAudioPtsGap;
    };

    /**
     * EventDispatcher Interface that defines callbacks for important capability change events.
     */
    class EventDispatcher
    {
    public:
        virtual ~EventDispatcher() {}

        /** Called when the system capabilities are changed. */
        virtual void capabilityChanged() = 0;

        /** Called when the video output resolution is changed. */
        virtual void videoOutputResolutionChanged() = 0;

        /** Called when the screensaver state is changed. */
        virtual void screensaverStateChanged() = 0;

        /** Called when the device language setting is changed. */
        virtual void languageChanged() = 0;

        /** Called when the view mode is changed. */
        virtual void viewModeChanged() = 0;

        /** Called when the device's output volume has changed or the device has
         * been muted or unmuted. */
        virtual void outputVolumeChanged() = 0;

        /** Called when the device Network setting is changed. */
        virtual void networkChanged() = 0;

        /** Called when HDMI plugin status is changed, such as hot plugged in/out. */
        virtual void videoOutputStatusChanged() = 0;

        /**
         * A new command is received. The syntax and semantics of the command
         * string is not defined as part of the NRDP DPI.
         *
         * @param[in] command the command string received.
         */
        virtual void commandReceived(const std::string &command) = 0;
    };

    virtual ~ISystem() {}

    /**
     * Initialization. An event-dispatcher interface is provided for the device
     * specific implementation to report events whenever needed.
     *
     * It's the device implementor's decision on what needs to be done inside
     * this function.
     *
     * @param[in] eventDispatcher the event-dispatcher interface.
     */
    virtual void init(shared_ptr<EventDispatcher> eventDispatcher) = 0;

    /**
     * Termination. It's the device implementor's decision on what needs to be
     * done inside this function.
     */
    virtual void shutdown() = 0;

    /**
     * Returns true if the screensaver is on.
     * For devices without screensaver support, always return false.
     * For devices with screensaver support, see also the hasScreenSaver capability.
     *
     * @return true if the screensaver is on.
     */
    virtual bool isScreensaverOn() const = 0;

    /**
     * Suspend. The device implementor must initiate a transfer to suspend state.
     * Once suspend state has been initiated the platform must call NrdApplication::setSuspended.
     *
     */
    virtual void requestSuspend(const std::string &/*reason*/) { }

    /**
     * Store an arbitrary set of key/value pairs in non-modifiable and
     * encrypted persistent storage. This replaces any previous set of
     * data in secure persistent storage. Upon failure, this method
     * should simply return.
     *
     * @param[in] data the data to store.
     */
    virtual void storeEncrypted(const std::map<std::string, std::string>& data) = 0;

    /**
     * Load the set of key/value pairs from non-modifiable and
     * encrypted persistent storage. Upon failure, this method should
     * return an empty map.
     */
    virtual std::map<std::string, std::string> loadEncrypted() = 0;

    /**
     * Synchronously write the secure storage.  This function is called at
     * critical times during run time as well as when the application is
     * torn down.
     */
    virtual void flushEncrypted() = 0;

    /**
     * Called to query the device why the application was terminated or was killed.
     * This method will be called when a crash has been detected.  If the partner
     * wishes to not use Breakpad or Netflix crash reporting a full report by the
     * partner can be provided here instead.
     *
     * Example 1:
     *      The device runs out of physical memory and the Linux OOM killer chooses
     *      Netflix.  The kernel issues SIGKILL to Netflix.  The device application
     *      manager on the waitpid() for Netflix sees that Netflix is terminated
     *      and parses /var/sys/kern.log to see the OOM killed SIGKILLed Netflix.
     *      The application manager then persists a termination reason in some
     *      special area.  The next time Netflix runs this method is called to
     *      retrieve the reason.
     *
     * Example 2:
     *      The Netflix application times out transitioning to suspend or resuming
     *      from suspend.  The application issues SIGKILL to Netflix. The application
     *      manager then persists a termination reason in some
     *      special area.  The next time Netflix runs this method is called to
     *      retrieve the reason.
     *
     * Example 3:
     *      The Netflix application makes an incorrect memory access and SIGSEGV
     *      causes the application to crash.  The partner crash reporting system
     *      persists a crash log and when this emthod is called, returns the report
     *      in a Netflix parsable format.
     */
    virtual Variant getReport() = 0;

    /**
     * Called to query the device uptime in seconds.
     *
     * @param[out] uptime Seconds since device was powered on.
     * @param[out] ontime
     *         Seconds since device transitioned from a user perceived "off" state to "on" state.
     *         If user has not transitioned the device to "on" state and
     *         Netflix is launched (ex: launch to suspend) this is the same
     *         value as uptime.
     *
     */
    virtual void getUptime(uint32_t & uptime, uint32_t & ontime) = 0;

    /**
     * Called to notify when the system is registered. This method may
     * be called multiple times with the same value--it is not only
     * called when the registered state has changed.
     *
     * @param[in] registered true if registered, false if not.
     */
    virtual void setRegistered(bool registered) = 0;

    /**
     * Return the device ESN.
     *
     * @return the ESN.
     */
    virtual const std::string getEsn() const = 0;

    /**
     * Gets the DeviceModel string.
     * The DeviceModel is a partner specific string to describe the current device
     *
     * @param[out] deviceModel the DeviceModel string
     * @return true if a DeviceModel is available
     */
    virtual bool getDeviceModel(std::string &deviceModel) const = 0;

    /**
     * Return the device language code. The language code should be in BCP-47
     * format.
     *
     * Note: The legacy ISO-639-1 two character code or ISO-639-2 three
     *       character code are both legitimate BCP-47 code. See
     *       http://tools.ietf.org/rfc/bcp/bcp47.txt for more details.
     *
     * @return the language code.
     */
    virtual const std::string getLanguage() const = 0;

    /**
     * Returns the System's capabilities
     *
     * @return capability the description of the system's capability.
     */
    virtual Capability getCapability() const = 0;

    /**
     * Return the number of times this application has been through Netflix's
     * the certification process.  This number should be monotonically
     * increasing.
     *
     * @return the number of certification passes.
     */
    virtual uint32_t getCertificationVersion() const = 0;

    /**
     * Return the device software version.
     *
     * @return the software version.
     */
    virtual const std::string getSoftwareVersion() const = 0;

    /**
     * Return the device friendly name (DLNA friendly name)
     *
     * @return the friendly name.
     */
    virtual const std::string getFriendlyName() const = 0;

    /**
     * Return the current time from the DRM subsystem.
     *
     * @return the DRM subsystem time.
     */

    /**
     * Return the authentication type used by the system.
     *
     * @return the authentication type.
     */
    virtual AuthenticationType getAuthenticationType() const = 0;

    /**
     * Return the video output resolution. For example: if the video output is set to NTSC,
     * the video output resolution should be 720x480; if the video output is set to 1080p
     * HD, the video output resolution should be 1920x1080.
     *
     * @param[out] width the width of video output plane in pixels.
     * @param[out] height the height of video output plane in pixels.
     */
    virtual void getVideoOutputResolution(uint32_t& width, uint32_t& height) const = 0;

    /**
     * Notify about the state of application for the device's information.
     *
     * @param[in] the new application state.
     */
    virtual void notifyApplicationState(ApplicationState state) = 0;

    /** !!!! DOCUMENT
     */
    virtual std::map<std::string, std::string> getStartupLogTags() = 0;

    /** This function instructs the DPI to reseed the SSL entropy. */
    virtual void reseedSslEntropy() = 0;

    /** Get the platform preferred IP connectivity mode. */
    virtual IpConnectivityMode getIpConnectivityMode() = 0;

    /**
     * Return the device current view mode. This function provides a standard way for
     * the device bridge to get the device current view mode.
     *
     * @return the current view mode.
     */
    virtual ViewModeType getCurrentViewMode() const = 0;

    /**
     * Return the device available view modes. This function provides a standard
     * way for the device bridge to get the device available view modes.
     *
     * @return the available view modes.
     */
    virtual std::vector<ViewModeType> getAvailableViewModes() const = 0;

    /**
     * Set the device view mode. This function provides a standard way for the
     * device bridge to set the view mode. Since it could take several seconds
     * for device to switch view mode, device must call
     * DeviceBridge::viewModeChanged() when the view mode switching is complete.
     * DeviceBridge::viewModeChanged() will in turn send an event to the
     * listening UI.
     *
     * @param[in] view mode to set.
     */
    virtual void setViewMode(ViewModeType& viewMode) = 0;

    /**
     * Returns the type of volume control that is implemented by the device.
     *
     * @return TV_VOLUME_CONTROL or NETFLIX_APP_VOLUME_CONTROL.
     *
     * A return value of TV_VOLUME_CONTROL means either:
     *
     * (1) The device itself physically renders the audio (i.e. the device has
     * speakers). It implements the netflix ISystem volume control calls so as
     * to control the global audio volume rendered by the device. For example,
     * if the device is a TV, the netflix ISystem volume control methods adjust
     * the TV's volume in the same way that the TV's remote control does. When
     * the TV's volume is adjusted by the ISystem volume control methods, the
     * TV's volume indication UI (e.g. bars representing volume level
     * temporarily drawn across the bottome of the screen) is shown.
     *
     * or
     *
     * (2) The external device (external meaning non-TV device such as a set-top
     * box or bluray player) is connected to a TV by a means which enables
     * control of the TV's volume as described in (1) (e.g. CEC). The device
     * implements the ISystem volume control calls via this capability.
     *
     * A return value of NETFLIX_APP_VOLUME_CONTROL means that the device neither
     * physically renders the audio nor can the device directly control the
     * global audio output level of the device that is physically rendering the
     * audio. The device can, however, scale the audio output level of the
     * netflix application or allow the netflix application to scale the overall
     * output of the device. The ISystem volume control methods are implemented
     * to control the netflix application's audio output level or overall output
     * level of the device. Returning this type from getVolumeControlType
     * conveys that the ultimate volume rendered by the loudspeakers is affected
     * by a downstream volume setting that is external to the device not
     * controllable from the netflix app.
     */
    virtual VolumeControlType getVolumeControlType() = 0;

    /**
     * @brief Returns the current volume
     *
     * Called by the SDK to get the device's current volume.
     * This is independent of whether the audio output is muted or not.
     * For example, if the device is not muted and getVolume() returns 0.5,
     * if the device is then muted getVolume() should still return 0.5.
     * If the device is then un-muted, getVolume() returns 0.5.
     *
     * 0 represents no audio. 1.0 represents maximum volume.
     * The linearity of the value is device dependent.
     * For instance, 0.5 could be interpreted on the device as linear or
     * logarithmic. This would either be a 6 dB drop from maximum or 1/2
     * the dynamic range in dB from maximum which would be a 72 dB drop on a
     * 24-bit output.
     *
     * @return the current volume setting ranging from 0 to 1.0.
     */
    virtual double getVolume() = 0;

    /**
     *
     * Called by the SDK to set the device's current volume setting.
     * If the audio output is muted when setVolume is called, it should
     * remain muted after setVolume returns.
     *
     * 0 represents no audio. 1.0 represents maximum volume.
     * The linearity of the value is device dependent.
     * For instance, .5 could be interpreted on the device as linear or
     * logarithmic. This would either be a 6 dB drop from maximum or 1/2
     * the dynamic range in dB from maximum which would be a 72 dB drop on a
     * 24-bit output.
     *
     * @param[in] volume: the volume setting (from 0 - 1.0)
     */
    virtual void setVolume(double volume) = 0;

    /**
     * For devices whose volume can only be set to discrete levels, when the SDK
     * wishes to change the volume by the smallest amount possible, it needs to
     * know what value corresponds to the distance to the next level.  For
     * example, if the current volume is 0.5, the next louder discrete volume
     * possible on the device is at 0.55, and the next quieter discrete volume is
     * at 0.45, getCurrentMinimumVolumeStep() would return 0.05. In the situation
     * where the difference to the next louder step does not equal the difference
     * to the next quieter step, the larger of the two should be returned.
     *
     *  @return true if the device is muted, false otherwise.
     */
    virtual double getCurrentMinimumVolumeStep() = 0;

    /**
     * @brief Returns true or false if device is currently muted
     * @return true if device is currently muted, false otherwise
     */
    virtual bool isMuted() = 0;

    /**
     * If the device is placed into the mute state, audio output is silent.
     * Mute does not affect the volume setting controlled by getVolume() and
     * setVolume().  Mute can be viewed as a switch that enables or disables
     * audio output irrespective of the volume setting.
     *
     * @param[in] muteSetting: if muteSetting is true, audio output should be
     * silenced. If false, audio output should resume at the volume as would be
     * returned by getVolume().
     */
    virtual void setMute(bool muteSetting) = 0;

    /**
     * @brief Get signatures of each in-process shared object.
     * @return Variant of signatures
     */
    virtual Variant getSignatures() = 0;

    /**
     *  Get the size of the encrypted persistent storage in bytes.
     */
    virtual ullong getSecureStoreSize() = 0;

    /** Network interface configuration information */
    struct NetworkInterface
    {
        NetworkInterface()
            : physicalLayerType(PHYSLAYER_TYPE_MOBILE),
              physicalLayerSubtype(PHYSLAYER_SUBTYPE_UNKNOWN),
              wirelessChannel(0),
              mobileCountryCode(0),
              mobileNetworkCode(0),
              isDefault(false),
              linkConnected(CONN_STATE_UNKNOWN),
              internetConnected(CONN_STATE_UNKNOWN)
        {
        }

        std::string name; // name of the interface, e.g. "eth0"
        PhysicalLayerType    physicalLayerType;    // e.g. mobile, gsm, wired, etc.
        PhysicalLayerSubtype physicalLayerSubtype; // e.g. 802_11a, 10 mbps
                                                   // ethernet,etc.
        std::string ipAddress; // current IPv4 address
        std::vector<std::string> ipv6Addresses; // list of current IPv6
                                                // addresses

        std::string macAddress; // hardware address for WIFI or wired.
        std::string ssid;       // if connected via wifi, the wireless SSID
        int wirelessChannel;    // if connected via wifi, the wireless channel

        // Set these fields if PhysicalLayerType is NET_TYPE_MOBILE,
        // NET_TYPE_GSM, or NET_TYPE_CDMA.
        std::string    mobileCarrier;     // Unique mobile carrier name.
        uint32_t       mobileCountryCode; // Mobile country code.
        uint32_t       mobileNetworkCode; // Mobile network code.

        bool isDefault; // true if this interface is used to send to the default
                        // gateway
        enum ConnectionState linkConnected; // able to communicate with wireless
                                            // access point for wireless, or if
                                            // ethernet is connected
        enum ConnectionState internetConnected; // able to communicate with
                                                // internet endpoint

        std::string additionalInfo;  // Additional device-dependent info that
                                     // should be logged.  The string may
                                     // contain JSON-formatted network setting
                                     // info, for example.
   };

    /**
     * Returns a list of NetworkInterfaces.
     */
    virtual std::vector<struct NetworkInterface> getNetworkInterfaces() = 0;

    /**
     * Returns a list of IP addresses as strings that will be used as DNS servers.
     */
    virtual std::vector<std::string> getDNSList() = 0;


    /*
     * Returns all supported digital output whether it is active or not.
     */
    virtual std::vector<struct VideoOutputInfo> getSupportedVideoOutput() = 0;

    /**
     * Returns all "active" digital output link status.
     *
     * When there is no link established, return empty vector
     *
     * When changed, device should notify via event mechanism defined by
     * EventDispather.
     */
    virtual std::vector<struct VideoOutputState> getActiveVideoOutput() = 0;

    /**
     * Returns Display panel dimension in cm. This is different from output
     * resolution. This is to get panel's physical size. Physical panel size can
     * be used to optimize UI and etc.  When unknown, return 0 for both width
     * and height.
     *
     * This does not have event notification mechanism. For self displaying
     * devices like DTV, pannel size will not be changed, and value retrieved by
     * first call of this DPI will hold valid. For external streaming devices
     * like STB, when HDMI is connected, this DPI can be called to get valid
     * value instead of sending event.
     */
    virtual void getDisplaySize(uint32_t& widthInCm, uint32_t& heightInCm) = 0;

    /**
     * returns Variant for requested key. This DPI is to collect arbitrary
     * information. When partner implementation does not have information for
     * key, it can return empty Variant.
     */
    virtual Variant getSystemValue(const std::string& key)
    {
        NRDP_UNUSED(key);
        Variant map;
        return map;
    }

    /*
     * ***************************
     * Dual Video Pipeline Support
     * ***************************
     *
     * This multi video pipeline capabilities will be added to ISystem for 15.1.
     * For future revisions, I think we need to refactor ISystem.h for
     * modularization such as, INetworkSystem, ISecuritySystem,
     * ICapabilitySystem and so on.
     *
     */

    /*
     * This is enumeration to describe decoder's maximum capability. We could
     * have used ContentProfile, but ContentProfile describe not only codec
     * capability but also packaging and drm. By providing maximum codec
     * capability, DPI provides information to NrdJs so that NrdJs can select
     * correct stream profiles to play. If mapping between below enum and
     * ContentProfile is bothersome, we can provide helper function at bridge,
     * or use ContentProfile with good explanation for partner porting.
     */
    enum CodecProfile
    {
        CODEC_NOT_AVAILABLE = 0,

        /*
         * H264 decoder profile list
         */
        AVC_MAIN_L30,
        AVC_MAIN_L31,
        AVC_MAIN_L40,

        /*
        AVC_SHPL30,
        AVC_SHPL31,
        AVC_SHPL40,
        */

        /*
         * HEVC decoder profile list - no need for main profile since main10
         * include main capability
         */
        HEVC_MAIN10_L20,
        HEVC_MAIN10_L21,
        HEVC_MAIN10_L30,
        HEVC_MAIN10_L31,
        HEVC_MAIN10_L40,
        HEVC_MAIN10_L41,
        HEVC_MAIN10_L50,
        HEVC_MAIN10_L51,
    };

    enum ZOrderCapability
    {
        ZORDER_NOT_AVAILABLE = 0, // no zorder is possible or undefined composition when layered
        ZORDER_ANY,               // can be layered any any zorder is possible for this video pipeline
        ZORDER_TOP_ONLY,          // can only be layered on top-most layer
        ZORDER_BOTTOM_ONLY        // can only be layered on bottom-most layer
    };

    /*
     * If below structure is passed as an input argument for getRemainingVideoPipelineCapability,
     * only pipelineId, maxHEVCCodecProfile, maxAVCCodecProfile needs to be specified.
     */
    struct VideoPipelineCapability
    {
        VideoPipelineCapability()
            : pipelineId(0), maxHEVCCodecProfile(CODEC_NOT_AVAILABLE), maxAVCCodecProfile(CODEC_NOT_AVAILABLE),
              isDrmCapable(false), isFullPictureQuality(false), zOrder(ZORDER_NOT_AVAILABLE), isFullFeaturedPipeline(false)
        {}
        uint32_t pipelineId;              // identifier for pipeline
        CodecProfile maxHEVCCodecProfile; // maximum HEVC decoder profile/level supported in this pipeline
        CodecProfile maxAVCCodecProfile;  // maximum H264 decoder profile/level supported in this pipeline
        bool isDrmCapable;                // DRM content can be played in this pipeline
        bool isFullPictureQuality;        // have complete Picture Quality module in this pipeline
        ZOrderCapability zOrder;          // ZOrdering capability
        bool isFullFeaturedPipeline;      // if this is false, check above fields individually
    };

    /*
     * Returns the maximum amount of video pipelines on the device. For example, declaring 2 means you support dual video pipelines.
     */
    virtual uint32_t maxVideoPipelines()
    {
        // TO BE IMPLEMENTED
        return 2;
    }

    /*
     * get individual video pipeline capability
     *
     * param[out] list of video pipeline capabilities when those are not used
     * simultaneously. This is inherent capability of each video pipeline
     *
     * Typical scenario : For example, suppose that there is a device that has two video
     * pipelines, one with both HEVC_MAIN10_L51 and H264_MAIN_L41 codec capable,
     * drm capable, zordering any, full picture quality and the other with
     * H264_MAIN_L41 pipeline, drm capable, zordering any, sub picture
     * quality, then this API will return lists of two pipeline as below
     *
     * {
     *   pipelineNum:1,
     *   maxHEVCCodecProfile: HEVC_MAIN10_L51
     *   maxH264CodecProfile: H264_MAIN_L41 // decoder in pipeline 1 can handle both HEVC and H264
     *   isDrmCapable: true
     *   isFullPictureQuality : true
     *   zOrder : ZORDERING_ANY
     * }
     * {
     *   pipelineNum: 2,
     *   maxHEVCCodecProfile: CODEC_NOT_AVAILABLE
     *   maxH264CodecProfile: H264_MAIN_L41 // decoder in pipeline 2 can handle only H264
     *   isDrmCapable: true
     *   isFullPictureQuality : false
     *   zOrder : ZORDERING_ANY
     * }
     *
     */
    virtual std::vector<struct VideoPipelineCapability> getVideoPipelineCapabilities()
    {
        return std::vector<struct VideoPipelineCapability>();
    }

    /*
     * get remaining video pipeline capability/property.
     *
     * API takes the list of all video capabilities/properties of video
     * pipelines currently(hypothetically) in use, and give back a single video
     * pipeline capability/property
     *
     * * return:
     *
     * param[in]: const std::vector<struct VideoPipelineProperty>& list of video
     *            pipeline properties currently in use, or hypothetical can be in use
     *
     * param[out]: video pipeline property left when above input video pipeline properties are in use.
     *
     * For example, suppose that device have "n + 2" pipeline. If application
     * provide "n" number of video pipeline properties as input that are
     * currently(or hypothetically) in use, then, API always get you next single
     * video pipeline property that can be used. If application use it also, now
     * device is running "n + 1" video pipeline. Next, if application provide
     * "n + 1" video pipeline properties, device will return the last 1 video
     * pipeline property.
     *
     * Typical scenario : For example, suppose that there is a device that has two video
     * pipelines, one with both HEVC_MAIN10_L51 and H264_MAIN_L41 codec capable,
     * drm capable, zordering any, full picture quality and the other with
     * H264_MAIN_L41 pipeline, drm capable, zordering any, sub picture
     * quality. If single video of max 1080p HEVC (HEVC_MAIN10_L40), DRMed video is currenlty or
     * hypothetically played at its first video pipeline, then, input for this
     * API will be like
     *
     * {
     *  pipelineNum:1,
     *  maxHEVCCodecProfile: HEVC_MAIN10_L40
     *  maxH264CodecProfile: CODEC_NOT_AVAILABLE
     *  isDrmCapable: true
     *  isFullPictureQuality : true
     *  zOrder : ZORDERING_ANY
     * }
     *
     *  then, device can return for remaining capability of
     *  {
     *   pipelineNum: 2,
     *   maxHEVCCodecProfile: HEVC_MAIN10_L40 // first pipeline is using half of HEVC power already
     *   maxH264CodecProfile: H264_MAIN_41
     *   isDrmCapable: true
     *   isFullPictureQuality : false
     *   zOrder : ZORDERING_ANY
     *  }
     *
     */

    virtual NFErr getRemainingVideoPipelineCapability(
        const std::vector<struct VideoPipelineCapability>& pipelineInUse,
        struct VideoPipelineCapability& pipelineToUse)
    {
        NRDP_UNUSED(pipelineInUse);
        NRDP_UNUSED(pipelineToUse);
        return NFErr_NotImplemented;
    }

    /**
     * This function is used for testing only and is when called disables the compositing of
     * the Dolby Vision enhancement layer onto the base layer.
     *
     * param[in] if true, disable EL composing. if false, enable EL composing
     */
    virtual void disableDolbyVisionELComposing(bool){};

};

}} // namespace netflix::device

#endif

