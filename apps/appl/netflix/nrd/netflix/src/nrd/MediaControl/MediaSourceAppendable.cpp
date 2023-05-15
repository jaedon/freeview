/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include <nrdbase/Assert.h>
#include "NrdpMediaSourceBuffer.h"

using namespace netflix;
using namespace mediacontrol;


bool MediaSourceAppendable::forward( NrdpMediaSourceBufferPtr pMediaSourceBuffer )
{
    ASSERT( !mForwardingComplete, "mForwardingComplete" );
    ASSERT( !mForwardingCancelled, "mForwardingCancelled" );
    ASSERT( pMediaSourceBuffer, "pMediaSourceBuffer" );

    mMediaSourceBufferPtr = pMediaSourceBuffer;

    mForwardingComplete = forwardBlocks();

    return mForwardingComplete;
}

void MediaSourceAppendable::cancelForwarding()
{
    ASSERT( !mForwardingComplete, "mForwardingComplete" );
    ASSERT( !mForwardingCancelled, "mForwardingCancelled" );

    mForwardingCancelled = true;

    mMediaSourceBufferPtr.reset();
}

void MediaSourceAppendable::newBlocksAvailable()
{
    ASSERT( !mForwardingComplete, "mForwardingComplete" );

    if ( mMediaSourceBufferPtr && !mForwardingCancelled )
        mMediaSourceBufferPtr->indicateBlocksAvailable();

}
void MediaSourceAppendable::forwardBlock( ase::AseBufferPtr pData )
{
    ASSERT( !mForwardingComplete, "mForwardingComplete" );
    ASSERT( !mForwardingCancelled, "mForwardingCancelled" );
    ASSERT( mMediaSourceBufferPtr, "mMediaSourceBufferPtr" );

    mMediaSourceBufferPtr->provideBlock( pData );
}
