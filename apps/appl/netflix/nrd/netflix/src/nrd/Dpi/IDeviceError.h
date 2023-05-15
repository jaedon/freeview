/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef __IDEVICE_ERROR_H__
#define __IDEVICE_ERROR_H__

#include <nrdbase/Variant.h>
#include <nrdbase/NFError.h>

namespace netflix {
namespace device {
namespace esplayer {


/** @file IDeviceError.h The header file for Device Error DPI.
 *
 */

/** @class netflix::device::IDeviceError.h

Stream player status codes. */
enum Status {
    OK,

    /**
     * There are no available media samples (access units) to deliver to the
     * device.
     */
    NO_AVAILABLE_SAMPLES,

    /**
     * The device is not currently able to accept a media sample (access unit)
     * from the SDK.
     */
    NO_AVAILABLE_BUFFERS,

    /**
     * All samples (access units) have been delivered.  There are no more
     * samples remaining in the presentation for this streams.
     */
    END_OF_STREAM,

    /**
     * There is an error retrieving media samples for the player.
     */
    STREAM_ERROR,

    /**
     * The device's playback pipeline encounters an unrecoverable decoding
     * error.
     */
    DECODE_ERROR,

    /**
     * The player received media attributes that it cannot support. May be
     * raised at stream player initialization time or upon stream switch.
     */
    UNEXPECTED_MEDIA_ATTRIBUTES,

    /**
     * Error initializing a new elementary stream player. If the error is due to
     * an unsupported media type, UNEXPECTED_MEDIA ATTRIBUTES should be
     * specified instead of this more general error.
     */
    INITIALIZATION_ERROR,

    /**
     * An unspecified error was occured. It is discouraged to use this
     * "unspecified error" to create IDeviceError. If should be used only when
     * you can not find any other error category that your error situation fit.
     * In that case, you should provide very specific deviceSpecificErrorCode
     * and extra information
     */
    ERROR,

    /***
     *  A DPI function whose implementation is optional was called by the
     *  system, and is not currently being implemented by this device.
     */
    NOT_IMPLEMENTED,

    /**
     * When initializing a new elementary stream player for MVC playback, the
     * SDK provides IPlaybackDevice::createStreamPlayer with an
     * IESPlayerCallback object that by default delivers base and dependent view
     * NALUs interleaved in a single sample buffer. The SDK can also provide
     * base and dependent view NALUs in separate buffers, however.  If the
     * createStreamPlayer implementation returns SPLIT_MVC_REQUIRED, the SDK
     * will call createStreamPlayer again and provide an IESPlayerCallback
     * object that delivers base and dependent NALUs in separate buffers.  The
     * mFormat3D of the VideoAttributes passed to createStreamPlayer specifies
     * whether the callback will provide MVC NALUs combined in one buffer or
     * with seperate base and dependent view buffers.
     */
    SPLIT_MVC_REQUIRED,

    /**
     * An encrypted sample has been fed with an unknown keyID specified.
     */
    UNKNOWN_KEY_ERROR,

    /**
     * An error was encountered decrypting a sample.
     */
    DECRYPTION_ERROR,

    /**
     * An error was caught in SDK code, for example, in the SDK's
     * IESPlayerCallback implementation.
     */
    INTERNAL_ERROR,

    /**
     * An error was encountered in ISampleWriter. ISampleWriter is the DPI that
     * handles stream transfer from NRDP to device. This error can happen when
     * initializing ISampleWriter fails, or writing data to the device falils.
     *
     */
    SAMPLE_WRITER_ERROR,

    /**
     * A HDCP related error was encountered while device is delivering video
     * data to renderer.
     *
     */
    HDCP_ERROR,

    /**
     * An error was encountered when initializing underlying DRM
     * SYSTEM. e.g. failed to initialize PlayReady moduel
     *
     */
    DRMSYSTEM_INITIALIZATION_ERROR = 100,

    /**
     * An error was encountered when generating challenge
     *
     */
    DRMSYSTEM_CHALLENGE_ERROR,

    /**
     * An error was encountered while retriveing license or storing license
     *
     */
    DRMSYSTEM_LICENSE_ERROR,

    /**
     * An error was encountered while handling secure stop
     *
     */
    DRMSYSTEM_SECURESTOP_ERROR,

    /**
     * An error was encountered while handling key store
     *
     */
    DRMSYSTEM_KEYSTORE_ERROR,

    /**
     * An error was encountered while handling drm store
     *
     */
    DRMSYSTEM_DRMSTORE_ERROR,

    /**
     *
     *
     */
    UNCATEGORIZED_ERROR = 1000

};

}}}// namespace netflix::device::esplayer

namespace netflix {
namespace device {

class IDeviceError : public NFError
{
public:
    /**
     * Create a device error object.
     *
     * @param[in] categoryCode status code that current error can be categorized
     *            into. Below 4 status codes are NOT ALLOWED to be used for error
     *            object since below 4 status codes are not for error situation
     *            esplayer::OK
     *            esplayer::NO_AVAILABLE_SAMPLES
     *            esplayer::NO_AVAILABLE_BUFFERS
     *            esplayer::END_OF_STREAM
     *
     * @param[in] deviceSpecificErrorCode error code from the device. This error
     *            code can be different from device to device. This is very
     *            important information for partners to debug the issue
     *            happening in the field. When error happens in the field,
     *            Netflix can provide this deviceSpecificErrorCode to the
     *            partners so that partners can debug issue easily.
     *
     * @param[in] deviceSpecificErrorInfo This can hold extra information of
     *            error. By using Variant, it is expandable.
     *
     * @param[in] enabledStackTracing when true, it captures stack trace when
     *            this error object is created.
     *
     */
    IDeviceError(esplayer::Status categoryCode,
                 ullong deviceSpecificErrorCode,
                 const Variant& deviceSpecificErrorInfo,
                 bool enableStackTracing = false);
    virtual ~IDeviceError();

    virtual esplayer::Status getCategoryCode();
    virtual ullong getDeviceSpecificErrorCode();
    virtual std::string getDeviceSpecificErrorCodeStr();
    virtual Variant& getDeviceSpecificErrorInfo();
    virtual std::string getDeviceSpecificErrorInfoStr();

    virtual std::string toString() const;
    virtual Variant toVariant() const;

    IDeviceError &operator=(const IDeviceError &rhs);

private:
    esplayer::Status  m_categoryCode;
    ullong            m_deviceSpecificErrorCode;
    Variant     m_deviceSpecificErrorInfo;

    static const std::string m_categoryCodeKey;
    static const std::string m_deviceSpecificErrorCodeKey;
    static const std::string m_deviceSpecificErrorInfoKey;
    static const std::string m_deviceErrorDescriptionKey;
    static const std::string m_deviceStackTraceKey;

    static const std::string& getCategoryCodeKey();
    static const std::string& getDeviceSpecificErrorCodeKey();
    static const std::string& getDeviceSpecificErrorInfoKey();
    static const std::string& getDeviceStackTraceKey();
};

}} // namespace netflix::device

#endif
