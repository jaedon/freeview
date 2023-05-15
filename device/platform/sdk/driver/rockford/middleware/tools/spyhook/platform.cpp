/*=============================================================================
Copyright (c) 2012 Broadcom Europe Limited.
All rights reserved.

Project  :  PPP
Module   :  MMM

FILE DESCRIPTION
DESC
=============================================================================*/

#include "platform.h"

#ifndef WIN32

#include <pthread.h>

void plCreateMutex(MutexHandle *handle)
{
   pthread_mutex_init(handle, NULL);
}

void plDestroyMutex(MutexHandle *handle)
{
   pthread_mutex_destroy(handle);
}

void plLockMutex(MutexHandle *handle)
{
   pthread_mutex_lock(handle);
}

void plUnlockMutex(MutexHandle *handle)
{
   pthread_mutex_unlock(handle);
}

unsigned int plGetTimeNowMs()
{
   static unsigned int sTimeBase = 0;

   unsigned int nowMs;

   struct timespec now;

   clock_gettime(CLOCK_REALTIME, &now);

   nowMs = now.tv_sec * 1000;
   nowMs += now.tv_nsec / 1000000;

   if (sTimeBase == 0)
      sTimeBase = nowMs;

   return nowMs - sTimeBase;
}

void plGetAccurateTime(unsigned int *secs, unsigned int *nanosecs)
{
   struct timespec   now;
   clock_gettime(CLOCK_REALTIME, &now);

   *secs = now.tv_sec;
   *nanosecs = now.tv_nsec;
}

#else

#include <windows.h>

void plCreateMutex(MutexHandle *handle)
{
   *handle = CreateMutex(0, FALSE, NULL);
}

void plDestroyMutex(MutexHandle *handle)
{
   CloseHandle(*handle);
}

void plLockMutex(MutexHandle *handle)
{
   WaitForSingleObject(*handle, INFINITE);
}

void plUnlockMutex(MutexHandle *handle)
{
   ReleaseMutex(*handle);
}

unsigned int plGetTimeNowMs()
{
   return timeGetTime();
}

void plGetAccurateTime(unsigned int *secs, unsigned int *nanosecs)
{
   unsigned int msec = timeGetTime();

   *secs = msec / 1000;
   *nanosecs = 1000 * (msec - (*secs * 1000));
}


#endif