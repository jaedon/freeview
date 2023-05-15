/***************************************************************************
*     (c)2007-2012 Broadcom Corporation
*
*  This program is the proprietary software of Broadcom Corporation and/or its licensors,
*  and may only be used, duplicated, modified or distributed pursuant to the terms and
*  conditions of a separate, written license agreement executed between you and Broadcom
*  (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
*  no license (express or implied), right to use, or waiver of any kind with respect to the
*  Software, and Broadcom expressly reserves all rights in and to the Software and all
*  intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
*  HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
*  NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
*
*  Except as expressly set forth in the Authorized License,
*
*  1.     This program, including its structure, sequence and organization, constitutes the valuable trade
*  secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
*  and to use this information only in connection with your use of Broadcom integrated circuit products.
*
*  2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
*  AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
*  WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
*  THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
*  OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
*  LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
*  OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
*  USE OR PERFORMANCE OF THE SOFTWARE.
*
*  3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
*  LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
*  EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
*  USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
*  THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
*  ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
*  LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
*  ANY LIMITED REMEDY.
*
* $brcm_Workfile: nexus_base_os.h $
* $brcm_Revision: 7 $
* $brcm_Date: 3/12/12 10:51a $
*
* Revision History:
*
* $brcm_Log: /nexus/base/include/nexus_base_os.h $
* 
* 7   3/12/12 10:51a erickson
* SW7425-1969: back out last checkin
* 
* 5   5/3/11 5:09p erickson
* SW7420-1819: repartition so NFE is a standard feature
* 
* 4   3/16/11 10:11a erickson
* SW7420-1642: add NEXUS_Base_Settings.driverModuleInit and
*  .driverModuleUninit
*
* 3   9/23/10 2:12p erickson
* SW7420-943: refactor for driver/firmware partitioning
*
* 2   9/15/10 3:39p jtna
* SW7420-1004: add synthunk for CORE and PLATFORM
*
* 1   7/14/10 11:59a erickson
* SW7405-4621: split out nexus_base_os.h
*
***************************************************************************/
#ifndef NEXUS_BASE_OS_H
#define NEXUS_BASE_OS_H

/* This is part of the public API. */
#include "bstd.h"
#include "bkni.h"
#include "bkni_multi.h"
#include "nexus_base_types.h"       /* this is also public API */

/**
SW services provided by base which can be used inside and outside nexus.
This is symlinked in nexus/modules/core for use outside nexus.
This file should not contain any nexus internals, including NEXUS_ModuleHandle, ISR context or magnum non-basemodules.
**/

#ifdef __cplusplus
extern "C"
{
#endif

/**
Summary:
Handle for a thread.

Description:

See Also:
NEXUS_Thread_Create
**/
typedef struct NEXUS_Thread *NEXUS_ThreadHandle;

/**
Summary:
Settings used for creating a thread

Description:

See Also:
NEXUS_Thread_GetDefaultSettings
NEXUS_Thread_Create
**/
typedef struct NEXUS_ThreadSettings
{
    unsigned priority;  /* 0=highest, 100=lowest */
    size_t  stackSize; /* In Bytes, may be rounded up to OS minimum */
} NEXUS_ThreadSettings;

/**
Summary:
Get default settings for the structure.

Description:
This is required in order to make application code resilient to the addition of new strucutre members in the future.

See Also:
NEXUS_Thread_Create
**/
void NEXUS_Thread_GetDefaultSettings(
    NEXUS_ThreadSettings *pSettings    /* [out] Default Settings for OS */
    );

/**
Summary:
Create a thread.

Description:

See Also:
NEXUS_Thread_Destroy
NEXUS_Thread_GetDefaultSettings
**/
NEXUS_ThreadHandle NEXUS_Thread_Create(
    const char *pThreadName,                 /* Thread Name, optional */
    void (*pThreadFunc)(void *),             /* Thread Main Routine */
    void *pContext,                          /* Context provided to callback */
    const NEXUS_ThreadSettings *pSettings    /* Thread Settings */
    );

/**
Summary:
Destroy a thread after its function has exited.

Description:
This does not cancel the execution of the pThreadFunc. pThreadFunc must exit on its own, then
NEXUS_Thread_Destroy can clean up the resources.

See Also:
NEXUS_Thread_Create
**/
void NEXUS_Thread_Destroy(
    NEXUS_ThreadHandle thread /* Thread Handle, returned from NEXUS_Thread_Create() */
    );

/**
Summary:
Activate gathering of run-time software profile information.
Profiling support shall be enabled at the compile time.
**/
NEXUS_Error NEXUS_Profile_Start(void);

/**
Summary:
Finishes gathering of run-time software profile information and prints report.
**/
void NEXUS_Profile_Stop(
    const char *name /* title of the profile report */
    );

/**
Summary:
Mark thread that could originate profiling samples
**/
void NEXUS_Profile_MarkThread(
    const char *name /* thread name */
    );

/**
Summary:
Flush the data cache for an address range.

Description:
This function will flush and invalidate the address range provided
from the data cache.  It is guaranteed that at least the address range
provided will be flushed, however larger amounts may be flushed depending
on the CPU and underlying OS primitives available.
****************************************************************************/
void NEXUS_FlushCache(
    const void *address, /* cached address to flush */
    size_t size /* size in bytes to flush */
    );

/**
Summary:
Returns the value for an environment variable.

Description:
The implementation of this function varies by OS.
For Linux user mode, this will be environment variables set in the shell.
For Linux kernel mode, it could be insmod parameters.

Returns:
A null-terminated string for the environment variable.
NULL means that it does not exist.

See Also:
NEXUS_SetEnv
*/
const char *NEXUS_GetEnv(
    const char *name
    );

/**
Summary:
Sets the value for an environment variable.

Description:
If value is NULL, the internal state for the given name will be cleared.

See Also:
NEXUS_GetEnv
*/
void NEXUS_SetEnv(
    const char *name,
    const char *value
    );


/**
Summary:
Convert a null-terminated ASCII string to an integer.

Description:
Nexus wrapper for C89,Posix.1 atoi(3) function
*/
int NEXUS_atoi(const char *str);

/**
Summary:
Standard string functions
**/
int NEXUS_P_Base_StrCmp(const char *str1, const char *str2);
int b_strlen(const char *s);
char *b_strncpy(char *dest, const char *src, int n);

/**
Summary:
Settings passed into NEXUS_Base_Init

Description:
NEXUS_Base_Settings must be defined in nexus_base_os.h because NEXUS_Base_GetDefaultSettings is OS-specific.
**/
typedef struct NEXUS_Base_Settings
{
    NEXUS_ThreadSettings threadSettings[NEXUS_ModulePriority_eMax];

    /* callbacks for per-module driver registration */
    NEXUS_Error (*driverModuleInit)(void *context, NEXUS_ModuleHandle module, const char *name, const NEXUS_ModuleSettings *pSettings);
    void (*driverModuleUninit)(void *context, NEXUS_ModuleHandle module, const char *name, const NEXUS_ModuleSettings *pSettings);
    void *procContext;
} NEXUS_Base_Settings;

/**
Summary:
Get default settings for the structure.

Description:
The implementation of NEXUS_Base_GetDefaultSettings is OS-specific so that threading can be customized.
**/
void NEXUS_Base_GetDefaultSettings(
    NEXUS_Base_Settings *pSettings /* [out] Default Settings */
    );

/**
Summary:
Used for profiling
**/
const char *NEXUS_P_Base_Os_GetTaskNameFromStack(const unsigned long *stack);

/**
Summary:
Used for profiling
**/
void NEXUS_P_Base_Os_MarkThread(const char *name);

/**
NEXUS_P_Base_Scheduler_Status is used by the internal and external scheduler interface.
**/
typedef struct NEXUS_P_Base_Scheduler_Status {
    bool exit; /* scheduler has to exit */
    bool idle; /* there is no activite in the scheduler */
    unsigned timeout; /* timer's timeout (miliseconds) */
} NEXUS_P_Base_Scheduler_Status;

#ifdef __cplusplus
}
#endif

#endif /* !defined NEXUS_BASE_OS_H */


