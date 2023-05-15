/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

/**
 * @file ISecureStop.h
 *
 *
 */
#ifndef ISECURESTOP_H_
#define ISECURESTOP_H_

#include <vector>

#include <nrdbase/tr1.h>
#include <nrdbase/NFErr.h>

namespace netflix {
namespace device {

/**
 * @class ISecureStop IDrm.h
 * @brief A secure stop is a token from the DRM subsystem that
 *        provides proof the DRM context associated with a decryption
 *        session has been destroyed. Not all DRM subsystems support
 *        the notion of a secure stop (e.g. Janus and PlayReady do
 *        not).
 */
class ISecureStop
{
public:
    /** Destructor. */
    virtual ~ISecureStop(){}

    /**
     * Returns the secure stop data.
     *
     * @return the secure stop data.
     * @sa getRawDataSize()
     */
    virtual unsigned char *getRawData() = 0;

    /**
     * Returns the secure stop data size.
     *
     * @return secure stop data size in bytes.
     * @sa getRawData()
     */
    virtual size_t getRawDataSize() = 0;

    /**
     * Pull the session ID associated with this secure stop. The
     * session ID is defined by the DRM subsystem and must uniquely
     * identify a decryption session's DRM context.
     *
     * @param[out] sessionID the session ID container.
     */
    virtual void getSessionID(/*out*/std::vector<unsigned char>& sessionID) = 0;
};

}} // namespace netflix::device
#endif
