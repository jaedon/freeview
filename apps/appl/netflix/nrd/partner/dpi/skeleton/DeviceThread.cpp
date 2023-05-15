/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "DeviceThread.h"

using namespace netflix::device::esplayer;
using namespace netflix;


DeviceThread::DeviceThread(ESPlayerNative& esPlayer, Task task,
                           ThreadConfig *config)
    : Thread(config),
      esPlayer_(esPlayer),
      task_(task)
{
    Start();
    WaitRunning();
}


DeviceThread::~DeviceThread()
{
    Wait(); // don't let the thread outlive the object
}

void DeviceThread::Run()
{
         (esPlayer_.*task_)();
}
