/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef I_MEDIA_FRAGMENT_H
#define I_MEDIA_FRAGMENT_H

#include <nrdase/AseBuffer.h>
#include <nrdase/AseTimeStamp.h>
#include <nrdbase/AseErrorCodes.h>
#include <nrdnet/AseTimeVal.h>

namespace netflix {
namespace ase {
class IMediaFragment
{
public:
    virtual ~IMediaFragment() {}

    /** Get the time at which this request should be completed */
    virtual AseTimeVal getCompletionDeadline() const = 0;

    /**
     * TODO: re-think of the role of IMediaFragment for updating buffer status
     */
    virtual bool isBufferAvailable() = 0;
    virtual void bufferFull() = 0;

    /** Handle media data
     * @param pBuffer [in] Buffer containing some media data
     * @param sequence [in] the sequence number of the incoming buffer
     * @return: negative error codes; 1 if the request is completed
     */
    virtual int provideResponseBody( AseBufferPtr const& pBuffer, int sequence, AseTimeVal& abandontime ) = 0;

    /** Handle a failure of an MEDIA request
     * @param code [in] The MEDIA failure code
     */
    virtual void provideFailureCode( AseErrorCode errCode ) = 0;

public:
    class BufferObj
    {
        public:
            BufferObj( AseBufferPtr const& pBuffer, int sequence ) :
                mAseBufferPtr(pBuffer), mSequence(sequence)
        {
        }

            BufferObj() :
                mAseBufferPtr( AseBufferPtr() ), mSequence(-1)
        {
        }

            AseBufferPtr mAseBufferPtr;
            int mSequence;
    };
};

}}
#endif
