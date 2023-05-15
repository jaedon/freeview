/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "SocketizerLog.h"

using namespace netflix::socketizer;

FILE* Log::sFile;
Mutex Log::sFileMutex;
bool Log::sEnabled = false;

void Log::initialize(FILE* f)
{
    MutexLocker locker(sFileMutex);
    sFile = f;
}

void Log::enable(bool en)
{
    MutexLocker locker(sFileMutex);
    sEnabled = en;
}
