/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef __ESMANAGERERROR_H__
#define __ESMANAGERERROR_H__

#include <nrdbase/NFError.h>

namespace netflix {
namespace device {

/* @file DrmError.h - Defines an NFError class that wraps around PlayReady error
 * codes.  Allows PlayReady error codes to be passed as netfliex NFErrors of
 * type NFErr_DRMFailed.  The file does not implement a DPI interface.
 */

class ESManagerError : public NFError
{
public:
    ESManagerError(NFErrorCode errorCode,
                   uint32_t deviceErrorCode,
                   const std::string& deviceErrorString);
    virtual ~ESManagerError();

    virtual std::string toString() const;
    virtual Variant toVariant() const;

private:
    uint32_t m_deviceErrorCode;
    std::string m_deviceErrorString;

};

}}

#endif // __DRM_ERROR_H__
