/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef __DRM_ERROR_H__
#define __DRM_ERROR_H__

#include <nrdbase/NFError.h>

namespace netflix {
namespace device {

/* @file DrmError.h - Defines an NFError class that wraps around PlayReady error
 * codes.  Allows PlayReady error codes to be passed as netfliex NFErrors of
 * type NFErr_DRMFailed.  The file does not implement a DPI interface.
 */

class DrmError : public NFError
{
public:
    DrmError(uint32_t errorCode);
    virtual ~DrmError();

    virtual std::string toString() const;
    virtual Variant toVariant() const;

    static const std::string getDrmErrorCodeKey();


private:
    uint32_t m_drmCode;
    static const std::string m_drmErrorCodeKey;

};

}}

#endif // __DRM_ERROR_H__
