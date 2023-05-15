/***************************************************************************
 *     Copyright (c) 2006-2009, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bdbg_os_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 1/26/09 10:33a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/dbg/bdbg_os_priv.h $
 * 
 * Hydra_Software_Devel/2   1/26/09 10:33a erickson
 * PR51415: rework bdbg_os_priv interface to not suggest BKNI_EventHandle
 * usage in BDBG_P_Lock, which must lock in both task and isr contexts
 *
 * Hydra_Software_Devel/1   4/27/06 10:10a vle
 * PR 21065: Redefined BDBG_LOCK and BDBG_UNLOCK to be OS specific
 * functions.
 *
 ***************************************************************************/


#ifndef BDBG_OS_PRIV_H__
#define BDBG_OS_PRIV_H__

#ifdef __cplusplus
extern "C" {
#endif

/* The functions below are OS specific and must exist for OS using BDBG module  */

/****************************************************************
Summary:
Initialize the OS-specific layer in the DBG basemodule
****************************************************************/
BERR_Code BDBG_P_OsInit(void);

/****************************************************************
Summary:
Uninitialize the OS-specific layer in the DBG basemodule
****************************************************************/
void BDBG_P_OsUninit(void);

/****************************************************************
Summary:
Lock BDBG module from multiple accesses.

Description:
DBG requires that BDBG_P_Lock be implemented per OS.
BDBG_P_Lock must work in both task and isr modes.
Therefore, BDBG_P_Lock cannot be implemented by calling BKNI_AcquireMutex or BKNI_EnterCriticalSection which only work in task mode.

Your OS's implementation of BDBG_P_Lock will likely look similar to your OS's implementation of BKNI_EnterCriticalSection.
For example, in linux user mode, BDBG_P_Lock acquries a pthread mutex.
For another example, in linux kernel mode, BDBG_P_Lock acquires a spin lock.
****************************************************************/
void BDBG_P_Lock(void);


/****************************************************************
Summary:
Release BDBG module from a BDBG_P_Lock state.
****************************************************************/
void BDBG_P_Unlock(void);


/****************************************************************
Summary:
This function initializes the initial timestamp when BDBG module
is initialize. This initial timestamp will be used as reference
for determining the run time thus far.
****************************************************************/
void BDBG_P_InitializeTimeStamp(void);


/****************************************************************
Summary:
This function calculates the run time and convert it into a
more readable form so that it can be attach to debug messages.
*****************************************************************/
void BDBG_P_GetTimeStamp(
    char *timeStamp,    /* size of the string containing the formatted timestamp */
    int size_t          /* the formatted timestamp to attach to debug messages */
    );


#ifdef __cplusplus
}
#endif

#endif
