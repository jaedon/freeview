/*
  FILE : stub_os.c
  PURPOSE: This file is a stub for linking tests.
*/
#include "va_os.h"

INT VA_OS_InitializeSemaphore(tVA_OS_Semaphore * const pSemaphore, UINT32 uiValue )
{
  return 0;
}

INT VA_OS_DeleteSemaphore(tVA_OS_Semaphore * const pSemaphore)
{
  return 0;
}

INT VA_OS_AcquireSemaphore(tVA_OS_Semaphore * const pSemaphore, UINT32 uiDuration )
{
  return 0;
}


INT VA_OS_ReleaseSemaphore(tVA_OS_Semaphore * const pSemaphore )
{
  return 0;
}

INT VA_OS_InitializeMutex(tVA_OS_Mutex * const pMutex)
{
  return 0;
}

INT VA_OS_DeleteMutex(tVA_OS_Mutex * const pMutex)
{
  return 0;
}

INT VA_OS_LockMutex(tVA_OS_Mutex * const pMutex)
{
  return 0;
}

INT VA_OS_UnlockMutex(tVA_OS_Mutex * const pMutex)
{
  return 0;
}

void* VA_OS_Alloc ( UINT32 uiSize)
{
  return NULL;
}

void VA_OS_Free(void * ptr)
{
  return;
}

INT VA_OS_Printf(const char *pFormat, ...)
{
  return 0;
}

INT VA_OS_Getc(UINT32 uiDuration)
{
  return 0;
}

void VA_OS_Sleep( UINT32 uiDuration )
{
  return;
}

DWORD VA_OS_StartPeriodicCall( UINT32 uiDurationPeriod,tVA_OS_PeriodicCallFct pfPeriodicCallFunction,
                               DWORD dwViaccessData )
{
  return 0;
}

INT VA_OS_StopPeriodicCall( DWORD dwHandle )
{
  return 0;
}

void VA_OS_GetTime(tVA_OS_Time *pTime)
{
  return;
}

void VA_OS_TimeToTm(const tVA_OS_Time  *pTime, tVA_OS_Tm  *pTm )
{
  return;
}

void VA_OS_TmToTime(const tVA_OS_Tm *pTm, tVA_OS_Time *pTime)
{
  return;
}

void VA_OS_GetRunTime(tVA_OS_RunTime *pRunTime)
{
  return;
}

/* End of File */