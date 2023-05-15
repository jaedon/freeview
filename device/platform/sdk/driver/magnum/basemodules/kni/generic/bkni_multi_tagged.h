/***************************************************************************
 *     Copyright (c) 2003, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bkni_multi_tagged.h $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 3/11/03 9:55p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/kernelinterface/generic/bkni_multi_tagged.h $
 * 
 * Hydra_Software_Devel/2   3/11/03 9:55p vsilyaev
 * Migrate to the new bkni.h file.
 * 
 * Hydra_Software_Devel/1   3/10/03 8:27p vsilyaev
 * Generic implementation of the tagged kernel interface API.
 * 
 *
 ***************************************************************************/
#ifndef BKNI_MULTI_TAGGED_H
#define BKNI_MULTI_TAGGED_H

#include "bkni_multi_ntg.h"

#if BKNI_USE_TAGGED_API

BERR_Code BKNI_CreateMutexTagged(BKNI_MutexHandle *mutex, const char *filename, unsigned lineno);
BERR_Code BKNI_DestroyMutexTagged(BKNI_MutexHandle mutex, const char *filename, unsigned lineno);
BERR_Code BKNI_AcquireMutexTagged(BKNI_MutexHandle mutex, int timeoutMillisec, const char *filename, unsigned lineno);
BERR_Code BKNI_ReleaseMutexTagged(BKNI_MutexHandle mutex, const char *filename, unsigned lineno);
BERR_Code BKNI_CreateTaskTagged(BKNI_TaskHandle task,BKNI_TaskFunction task_fn, BKNI_TaskPriority priority, void *data, const char *filename, unsigned lineno);
BERR_Code BKNI_DestroyTaskTagged(BKNI_TaskHandle task, int *returnCode, const char *filename, unsigned lineno);

#define BKNI_CreateMutex(mutex) BKNI_CreateMutexTagged(mutex, __FILE__, __LINE__)
#define BKNI_DestroyMutex(mutex) BKNI_DestroyMutexTagged(mutex, __FILE__, __LINE__) 
#define BKNI_AcquireMutex(mutex, timeoutMillisec) BKNI_AcquireMutexTagged(mutex, timeoutMillisec, __FILE__, __LINE__) 
#define BKNI_ReleaseMutex(mutex) BKNI_ReleaseMutexTagged(mutex, __FILE__, __LINE__)
#define BKNI_CreateTask(task, task_fn, priority, data) BKNI_CreateTaskTagged(task, task_fn, priority, data, __FILE__, __LINE__)
#define BKNI_DestroyTask(task, returnCode) BKNI_DestroyTaskTagged(task, returnCode, __FILE__, __LINE__)

#endif /* BKNI_USE_TAGGED_API */

#endif /* BKNI_MULTI_TAGGED_H */

