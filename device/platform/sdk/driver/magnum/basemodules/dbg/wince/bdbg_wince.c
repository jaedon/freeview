/***************************************************************************
 *     Copyright (c) 2005-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bdbg_wince.c $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 11/23/10 12:21p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /brcm_wince/97400/Private/BMSIPTV/magnum/basemodules/dbg/wince/bdbg_wince.c $
 * 
 * Hydra_Software_Devel/2   11/23/10 12:21p katrep
 * SW7405-3705:add os init functions
 * 
 * Hydra_Software_Devel/1   11/10/10 6:04p katrep
 * SW7405-3705:add wince dir
 * 
 * PROD_WinCE60/3   12/11/09 6:23p ptimariu
 * SW7405-3458: client 2.0 nexus 6.5 sync-up
 * 
 * PROD_WinCE60/2   1/22/09 7:55a rrlee
 * PR51161: Add support for BKNI_ASSERT_ISR_CONTEXT macro
 * 
 * PROD_WinCE50_7401/2   5/16/06 4:23p rrlee
 * PR21405: Merge Linux changes
 * 
 ***************************************************************************/
 
#include <windows.h>
#include "bstd.h"
#include "bkni.h"
#include "bkni_multi.h"

static DWORD s_initCount = 0;
void
BDBG_P_InitializeTimeStamp(void)
{
    s_initCount = GetTickCount();
}

void
BDBG_P_GetTimeStamp(char *timeStamp, int size_t)
{
	DWORD tickCount;
	int hours, minutes, seconds, milliseconds;

    tickCount = GetTickCount() - s_initCount;
    milliseconds = tickCount % 1000;
    seconds = tickCount / 1000;

	
	/* Calculate the time	*/
	hours = seconds/3600;
	minutes = (seconds/60)%60;
	seconds %= 60;
		
	/* print the formatted time including the milliseconds	*/
	BKNI_Snprintf(timeStamp, size_t, "%02u:%02u:%02u.%03u", hours, minutes, seconds, milliseconds);
	return;
}

// Modifications to BDBG data structures mus be done under lock 
// compatible with BKNI _isr code rules. Can't be BKNI_Mutex!
static BOOL s_Initialized=FALSE;
static CRITICAL_SECTION s_DbgLock;


BERR_Code BDBG_P_OsInit(void)
{
    /* g_mutex is statically initialized */
    return 0;
}

void BDBG_P_OsUninit(void)
{
}


void BDBG_P_Lock(BKNI_MutexHandle mutex)
{
    if(!s_Initialized)
    {
        InitializeCriticalSection(&s_DbgLock);
        s_Initialized=TRUE;
    }
    EnterCriticalSection(&s_DbgLock);
}

void BDBG_P_Unlock(BKNI_MutexHandle mutex)
{
    LeaveCriticalSection(&s_DbgLock);
}
/* End of file */
