/******************************************************************************
 
    Copyright
    This code is strictly confidential and the receiver is obliged to use it 
    exclusively for his or her own purposes. No part of Viaccess code may be 
    reproduced or transmitted in any form or by any means, electronic or 
    mechanical, including photocopying, recording, or by any information storage 
    and retrieval system, without permission in writing from Viaccess. 
    The information in this code is subject to change without notice. Viaccess 
    does not warrant that this code is error free. If you find any problems 
    with this code or wish to make comments, please report them to Viaccess.
 
    Trademarks 
    Viaccess is a registered trademark of Viaccess ® in France and/or other 
    countries. All other product and company names mentioned herein are the 
    trademarks of their respective owners.
    Viaccess may hold patents, patent applications, trademarks, copyrights 
    or other intellectual property rights over the code hereafter. Unless 
    expressly specified otherwise in a Viaccess written license agreement, the
    delivery of this code does not imply the concession of any license over 
    these patents, trademarks, copyrights or other intellectual property.
 
******************************************************************************/

/*
    $Revision: 5833 $
    $Date: 2007-05-10 17:56:04 +0200 (jeu., 10 mai 2007) $
*/



#ifndef _VA_OS_H_
#define _VA_OS_H_

#include "va_types.h"
#include "va_errors.h"
#include "va_setup.h"


#define kVA_OS_NBMAX_PERIODIC_CALLS            1
#define kVA_OS_WAIT_FOREVER                    0xFFFFFFFF
#define kVA_OS_NO_WAIT                         0


typedef DWORD tVA_OS_Semaphore[kVA_SETUP_SEMAPHORE_SIZE_IN_DWORD];
typedef DWORD tVA_OS_Mutex[kVA_SETUP_MUTEX_SIZE_IN_DWORD];
typedef INT (*tVA_OS_PeriodicCallFct)(DWORD dwViaccessData);


INT VA_OS_InitializeSemaphore(
    tVA_OS_Semaphore * const pSemaphore, UINT32 uiValue );

INT VA_OS_DeleteSemaphore(
    tVA_OS_Semaphore * const pSemaphore);

INT VA_OS_AcquireSemaphore(
    tVA_OS_Semaphore * const pSemaphore,
    UINT32 uiDuration );

INT VA_OS_ReleaseSemaphore(
    tVA_OS_Semaphore * const pSemaphore );

INT VA_OS_InitializeMutex(tVA_OS_Mutex * const pMutex);

INT VA_OS_DeleteMutex(tVA_OS_Mutex * const pMutex);

INT VA_OS_LockMutex(tVA_OS_Mutex * const pMutex);

INT VA_OS_UnlockMutex(tVA_OS_Mutex * const pMutex);

void *VA_OS_Alloc ( UINT32 uiSize);

void VA_OS_Free(void * ptr);

INT VA_OS_Printf(const char *pFormat, ...);

INT VA_OS_Getc(UINT32 uiDuration);

void VA_OS_Sleep( UINT32 uiDuration );

DWORD VA_OS_StartPeriodicCall( 
    UINT32 uiDurationPeriod,
    tVA_OS_PeriodicCallFct pfPeriodicCallFunction,
    DWORD dwViaccessData );

INT VA_OS_StopPeriodicCall( DWORD dwHandle );


typedef UINT32 tVA_OS_Time;

typedef struct
{
    UINT32 uiSec ;
    UINT32 uiMin ;
    UINT32 uiHour ;
    UINT32 uiMonthDay ;
    UINT32 uiMonth ;
    UINT32 uiYear ;
    UINT32 uiWeekDay ;
    UINT32 uiYearDay ;

} tVA_OS_Tm;


typedef struct
{
    UINT32 uiMilliSec ;
    UINT32 uiMicroSec ;

} tVA_OS_RunTime ;



void VA_OS_GetTime(tVA_OS_Time *pTime);

void VA_OS_TimeToTm(
    const tVA_OS_Time  *pTime, tVA_OS_Tm  *pTm );

void VA_OS_TmToTime(
    const tVA_OS_Tm *pTm, tVA_OS_Time *pTime);

void VA_OS_GetRunTime(tVA_OS_RunTime *pRunTime);

#endif /* _VA_OS_H_ */



