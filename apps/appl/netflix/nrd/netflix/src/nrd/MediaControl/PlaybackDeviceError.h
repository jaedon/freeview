/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef __PLAYBACK_DEVICE_ERROR_H__
#define __PLAYBACK_DEVICE_ERROR_H__

#include <nrdbase/NFError.h>

namespace netflix {
namespace device {

//
// Wrap a device error into an NFError
//
class PlaybackDeviceError : public NFError
{
public:
    PlaybackDeviceError(int32_t iespStatusCode, ullong deviceSpecificErrorCode,
                        const std::string& deviceSpecificErrorMsg);
    virtual ~PlaybackDeviceError();

    virtual std::string toString() const;
    virtual Variant toVariant() const;

    static const std::string& getIespStatusCodeKey();
    static const std::string& getDeviceSpecificErrorCodeKey();
    static const std::string& getDeviceSpecificErrorMsgKey();

private:
    int32_t m_iespStatusCode;

    // Stores the 64-bit device specific error code as a string
    // expressing the value as hex, i.e. "0x80f0...".
    std::string m_deviceSpecificErrorCode;
    std::string m_deviceSpecificErrorMsg;

    static const std::string m_iespStatusCodeKey;
    static const std::string m_deviceSpecificErrorCodeKey;
    static const std::string m_deviceSpecificErrorMsgKey;
};

}} // namespace netflix::device


#endif // __PLAYBACK_DEVICE_ERROR_H__
