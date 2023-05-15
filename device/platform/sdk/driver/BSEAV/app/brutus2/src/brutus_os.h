/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: brutus_os.h $
* $brcm_Revision: 2 $
* $brcm_Date: 2/28/12 5:03p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/brutus_os.h $
* 
* 2   2/28/12 5:03p tokushig
* SW7405-5581: add header comments
* 
* 
*********************************************************************************/

#ifndef BRUTUSOS_H__
#define BRUTUSOS_H__

#include "b_os_lib.h"

#ifdef __cplusplus
extern "C" {
#endif


extern B_ThreadHandle       gSchedulerThread;
extern B_MutexHandle        gLock;
extern B_SchedulerHandle    gScheduler;

/*
    This mutex class is based on the b_os mutex type but implements it such
    that the mutex is locked when this object is constructed, and unlocked
    when this object is freed.  It is a convenience class used to lock/unlock
    a given mutex based on the scope of a local variable - 'break' or 'return'
    will still trigger the mutex unlock based on scope rules.
 
    For example:
    void test(int * pInt)
    {
        CScopedMutex(_myMutex);
 
        if (NULL == pInt)
            return;
 
        *pInt = 1;
    }
 
    _myMutex will be locked at the start of test() and unlocked automatically
    when the function returns (regardless of whether pInt points to a valid
    int or is NULL).
*/
class CScopedMutex
{
public:
    CScopedMutex(B_MutexHandle mutex);
    ~CScopedMutex(void);
protected:
    B_MutexHandle _mutex;
};

#ifdef __cplusplus
}
#endif

#endif /* BRUTUSOS_H__ */

