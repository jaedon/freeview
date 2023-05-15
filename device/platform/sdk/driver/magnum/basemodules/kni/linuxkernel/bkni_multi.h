/***************************************************************************
 *     Copyright (c) 2003-2009, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bkni_multi.h $
 * $brcm_Revision: Hydra_Software_Devel/6 $
 * $brcm_Date: 4/13/09 3:51p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/kni/linuxkernel/bkni_multi.h $
 * 
 * Hydra_Software_Devel/6   4/13/09 3:51p jtna
 * PR53480: PR53480: BKNI_TRACK_MALLOCS logic for linuxkernel
 * 
 * Hydra_Software_Devel/PR53480/1   4/8/09 12:08p jtna
 * PR53480: BKNI_TRACK_MALLOCS logic for linuxkernel
 * 
 * Hydra_Software_Devel/5   1/30/09 9:17p vsilyaev
 * PR 42495: Using matching types for the line number
 * 
 * Hydra_Software_Devel/4   4/8/03 4:47p erickson
 * BKNI_ResetEvent now returns void
 * Split BKNI_AcquireMutex into Acquire and TryAcquire
 * 
 * Hydra_Software_Devel/3   4/3/03 6:13p erickson
 * some pre 0.9 api rework
 * 
 * Hydra_Software_Devel/2   3/20/03 3:25p erickson
 * added linuxuser
 * 
 * Hydra_Software_Devel/1   3/12/03 3:50p erickson
 * updated tests and got linuxkernel working with them
 ***************************************************************************/
#ifndef BKNI_MULTI_H__
#define BKNI_MULTI_H__

#include "bkni.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct BKNI_MutexObj *BKNI_MutexHandle;

#if BKNI_TRACK_MALLOCS
#define BKNI_CreateMutex(mutex) BKNI_CreateMutex_tagged(mutex, __FILE__, __LINE__)
BERR_Code BKNI_CreateMutex_tagged(BKNI_MutexHandle *mutex, const char *file, int line);

#define BKNI_DestroyMutex(mutex) BKNI_DestroyMutex_tagged(mutex, __FILE__, __LINE__)
void BKNI_DestroyMutex_tagged(BKNI_MutexHandle mutex, const char *file, int line);
#else
BERR_Code BKNI_CreateMutex(BKNI_MutexHandle *mutex);
void BKNI_DestroyMutex(BKNI_MutexHandle mutex);
#endif

#define BKNI_AcquireMutex(MUTEX) BKNI_AcquireMutex_tagged(MUTEX,__FILE__,__LINE__)
#define BKNI_TryAcquireMutex(MUTEX) BKNI_TryAcquireMutex_tagged(MUTEX,__FILE__,__LINE__)
#define BKNI_ReleaseMutex(MUTEX) BKNI_ReleaseMutex_tagged(MUTEX,__FILE__,__LINE__)

BERR_Code BKNI_AcquireMutex_tagged(BKNI_MutexHandle mutex, const char *file, unsigned line);
BERR_Code BKNI_TryAcquireMutex_tagged(BKNI_MutexHandle mutex, const char *file, unsigned line);
void BKNI_ReleaseMutex_tagged(BKNI_MutexHandle mutex, const char *file, unsigned line);

#ifdef __cplusplus
}
#endif

#endif /* BKNI_MULTI_H__ */
