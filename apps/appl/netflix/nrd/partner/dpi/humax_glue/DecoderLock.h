/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef DEVICE_DECODERLOCK_H
#define DEVICE_DECODERLOCK_H

/* @file DecoderLock.h - Provides static mutexes protecting access to the
 * reference application's implementation-specific codec library. The audio and
 * video decoders call into the same library on different threads. Access to
 * certain library calls must be synchronized.
 */

#include <nrdbase/Mutex.h>

namespace netflix {
namespace device {

/**
 * @class DecoderLock DecoderLock.h
 * @brief A place to share common locks between decoders.
 */
class DecoderLock
{
private:
    static Mutex* mutexForOpenClose_;

public:
    static void   init();
    static void   shutdown();
    static Mutex& mutexForOpenClose();
};

}} // namespace netflix::device

#endif // DEVICE_DECODERLOCK_H
