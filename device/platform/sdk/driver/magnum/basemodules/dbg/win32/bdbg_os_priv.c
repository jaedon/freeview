/***************************************************************************
 *     Copyright (c) 2006-2009, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bdbg_os_priv.c $
 * $brcm_Revision: Hydra_Software_Devel/3 $
 * $brcm_Date: 7/7/09 10:18a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/dbg/win32/bdbg_os_priv.c $
 * 
 * Hydra_Software_Devel/3   7/7/09 10:18a ronchan
 * PR 51415: removed duplicate output functions
 * 
 * Hydra_Software_Devel/2   1/27/09 3:14p ronchan
 * PR 51415: rework bdbg_os_priv interface to not suggest BKNI_EventHandle
 * usage in BDBG_P_Lock, which must lock in both task and isr contexts
 * 
 * Hydra_Software_Devel/1   4/27/06 11:28a vle
 * PR 21065: Redefined BDBG_LOCK and BDBG_UNLOCK to be OS specific
 * functions
 * 
 ***************************************************************************/


#include "bstd.h"
#include "bkni.h"

#include <windows.h>

static CRITICAL_SECTION g_mutex;

void
BDBG_P_InitializeTimeStamp(void)
{
	return;
}

void
BDBG_P_GetTimeStamp(char *timeStamp, int size_t)
{
	*timeStamp='\0';
	return;
}

BERR_Code BDBG_P_OsInit(void)
{
   InitializeCriticalSection(&g_mutex);
   return BERR_SUCCESS;
}

void BDBG_P_OsUninit(void)
{
   DeleteCriticalSection(&g_mutex);
}

void BDBG_P_Lock(void)
{
	EnterCriticalSection(&g_mutex);
	return;
}

void BDBG_P_Unlock(void)
{
	LeaveCriticalSection(&g_mutex);
	return;
}

