/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef I_MEDIA_SOURCE_APPENDABLE_H
#define I_MEDIA_SOURCE_APPENDABLE_H

#include "nrdase/AseBuffer.h"

#include <map>

namespace netflix {
namespace mediacontrol {

typedef std::map<std::string,uint32_t> StreamAttributesType;
typedef shared_ptr<StreamAttributesType> StreamAttributesPtr;

class NrdpMediaSourceBuffer;
typedef shared_ptr<NrdpMediaSourceBuffer> NrdpMediaSourceBufferPtr;

/** @class MediaSourceAppendable
 *  Base class for objects that can be appended to an NrdpMediaSourceBuffer
 *
 *  Methods of this class should be called only from the HRM thread
 */
class MediaSourceAppendable
{
public:
    typedef shared_ptr<MediaSourceAppendable> MediaSourceAppendablePtr;

    /** Provide the source buffer and indicate that forwarding can start */
    bool forward( NrdpMediaSourceBufferPtr pMediaSourceBuffer );

    /** Cancel forwarding in progress */
    void cancelForwarding();

protected:

    MediaSourceAppendable();

    virtual ~MediaSourceAppendable() {}

    /** Called by subclass when new data is available */
    void newBlocksAvailable();

    /** Called by subclass to forward a block */
    void forwardBlock( ase::AseBufferPtr pData );

    /** Implemented by subclasses - must forward all available blocks
     *  and return true if forwarding is complete
     */
    virtual bool forwardBlocks() = 0;

protected:

    NrdpMediaSourceBufferPtr    mMediaSourceBufferPtr;

    bool                        mForwardingComplete;

    bool                        mForwardingCancelled;
};

typedef MediaSourceAppendable::MediaSourceAppendablePtr MediaSourceAppendablePtr;

inline MediaSourceAppendable::MediaSourceAppendable()
    : mForwardingComplete( false )
    , mForwardingCancelled( false )
{
}

}}

#endif
