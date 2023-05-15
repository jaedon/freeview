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
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 12/30/09 6:13p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/dbg/bkernel/bdbg_os_priv.c $
 * 
 * Hydra_Software_Devel/2   12/30/09 6:13p vsilyaev
 * SW7408-17: Reduced amout of debug output
 * 
 * Hydra_Software_Devel/1   11/20/09 5:24p jgarrett
 * SW7408-17: Adding bkernel
 * 
 * Hydra_Software_Devel/4   1/26/09 10:33a erickson
 * PR51415: rework bdbg_os_priv interface to not suggest BKNI_EventHandle
 * usage in BDBG_P_Lock, which must lock in both task and isr contexts
 *
 * Hydra_Software_Devel/3   11/27/07 10:29a jgarrett
 * PR 37550: Coverity fixes
 *
 * Hydra_Software_Devel/2   10/4/06 3:21p jgarrett
 * PR 24626: Converting lock routines to use internal mutex.  BKNI mutexes
 * are not appropriate as this may be called from critical section or isr
 * code.
 *
 * Hydra_Software_Devel/1   4/27/06 10:22a vle
 * PR 21065: Redefined BDBG_LOCK and BDBG_UNLOCK to be OS specific
 * functions.
 *
 ***************************************************************************/

#include "bstd.h"
#include "bkni.h"
#include "bkernel.h"

void
BDBG_P_InitializeTimeStamp(void)
{
}

void
BDBG_P_GetTimeStamp(char *timeStamp, int size)
{
    struct timeval currentTime;

    gettimeofday(&currentTime, NULL);

    BKNI_Snprintf(timeStamp, size, "%3u.%03u", currentTime.tv_sec, currentTime.tv_usec/1000);
    return;

}

BERR_Code BDBG_P_OsInit(void)
{
    return 0;
}

void BDBG_P_OsUninit(void)
{
}

/* NOTE: this function is called from both magnum task and isr context */
void BDBG_P_Lock(void)
{
}

/* NOTE: this function is called from both magnum task and isr context */
void BDBG_P_Unlock(void)
{
}

/* End of file */

