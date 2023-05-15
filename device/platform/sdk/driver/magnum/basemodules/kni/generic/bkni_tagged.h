/***************************************************************************
 *     Copyright (c) 2003, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bkni_tagged.h $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 3/11/03 9:55p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/kernelinterface/generic/bkni_tagged.h $
 * 
 * Hydra_Software_Devel/2   3/11/03 9:55p vsilyaev
 * Fixes for new kni.h
 * 
 * Hydra_Software_Devel/1   3/10/03 8:28p vsilyaev
 * Generic implementation of the tagged kernel interface API.
 * 
 *
 ***************************************************************************/
#ifndef BKNI_TAGGED_H
#define BKNI_TAGGED_H

#include "bkni_ntg.h"

#if BKNI_USE_TAGGED_API

void BKNI_DelayTagged(int microsec, const char *filename, unsigned lineno);
void *BKNI_MallocTagged(size_t size, const char *filename, unsigned lineno);
void BKNI_FreeTagged(void *mem, const char *filename, unsigned lineno);
BERR_Code BKNI_SleepTagged(int millisec, const char *filename, unsigned lineno);
BERR_Code BKNI_CreateEventTagged(BKNI_EventHandle *event, bool initiallySignalled, const char *filename, unsigned lineno);
BERR_Code BKNI_DestroyEventTagged(BKNI_EventHandle event, const char *filename, unsigned lineno);
BERR_Code BKNI_WaitForEventTagged(BKNI_EventHandle event, int timeoutMsec, const char *filename, unsigned lineno);
BERR_Code BKNI_SetEventTagged(BKNI_Event event, const char *filename, unsigned lineno);
BERR_Code BKNI_EnterCriticalSectionTagged(const char *filename, unsigned lineno);
BERR_Code BKNI_LeaveCriticalSectionTagged(const char *filename, unsigned lineno);

#define BKNI_Delay(microsec) BKNI_DelayTagged(microsec, __FILE__, __LINE__)
#define BKNI_Malloc(size) BKNI_MallocTagged(size, __FILE__, __LINE__)
#define BKNI_Free(mem) BKNI_FreeTagged(mem, __FILE__, __LINE__)
#define BKNI_Sleep(millisec) BKNI_SleepTagged(millisec, __FILE__, __LINE__)
#define BKNI_CreateEvent(event, initiallySignalled) BKNI_CreateEventTagged(event, initiallySignalled, __FILE__, __LINE__)
#define BKNI_DestroyEvent(event) BKNI_DestroyEventTagged(event, __FILE__, __LINE__)
#define BKNI_WaitForEvent(event, timeoutMsec) BKNI_WaitForEventTagged(event, timeoutMsec, __FILE__, __LINE__)
#define BKNI_SetEvent(event)  BKNI_SetEventTagged(event, __FILE__, __LINE__) 
#define BKNI_EnterCriticalSection() BKNI_EnterCriticalSectionTagged(__FILE__, __LINE__)
#define BKNI_LeaveCriticalSection() BKNI_LeaveCriticalSectionTagged(__FILE__, __LINE__)

#endif  /* BKNI_USE_TAGGED_API */

#endif /* BKNI_TAGGED_H */