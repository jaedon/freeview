/***************************************************************************
 *     Copyright (c) 2006-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bdbg_os_priv.c $
 * $brcm_Revision: Hydra_Software_Devel/9 $
 * $brcm_Date: 2/21/12 8:44a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/dbg/linuxkernel/bdbg_os_priv.c $
 * 
 * Hydra_Software_Devel/9   2/21/12 8:44a erickson
 * SW7346-703: add linux 3.3 support
 * 
 * Hydra_Software_Devel/8   4/19/11 5:51p nickh
 * SW7425-365: Add 2.6.37 kernel support
 * 
 * Hydra_Software_Devel/7   9/11/09 9:54a erickson
 * SW7420-228: switch to autoconf.h
 *
 * Hydra_Software_Devel/6   1/26/09 10:33a erickson
 * PR51415: rework bdbg_os_priv interface to not suggest BKNI_EventHandle
 * usage in BDBG_P_Lock, which must lock in both task and isr contexts
 *
 * Hydra_Software_Devel/5   2/12/08 10:27a jgarrett
 * PR 36078: Initializing spinlock
 *
 * Hydra_Software_Devel/4   6/19/07 11:40a jgarrett
 * PR 31935: Updating for 2.6.18
 *
 * Hydra_Software_Devel/3   10/6/06 10:18a jgarrett
 * PR 21708: Adding include for 2.6.
 *
 * Hydra_Software_Devel/2   10/5/06 11:56a jgarrett
 * PR 21708: Adding spin_locks where locking is required.
 *
 * Hydra_Software_Devel/1   4/27/06 10:28a vle
 * PR 21065: Redefined BDBG_LOCK and BDBG_UNLOCK to be OS specific
 * functions
 *
 ***************************************************************************/

#include "bstd.h"
#include "bkni.h"
#include <linux/version.h>
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 37)
#include <generated/autoconf.h>
#else
#include <linux/autoconf.h>
#endif
#include <linux/time.h>
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
#include <linux/hardirq.h>
#else
#include <linux/tqueue.h>
#include <asm/smplock.h>
#endif

static struct timeval initTimeStamp;
static spinlock_t g_lock = __SPIN_LOCK_UNLOCKED(bdbg_os_priv.lock);
static unsigned long g_lockFlags;

void
BDBG_P_InitializeTimeStamp(void)
{
    do_gettimeofday(&initTimeStamp);
}

void
BDBG_P_GetTimeStamp(char *timeStamp, int size_t)
{
    struct timeval currentTime;
    int hours, minutes, seconds;
    int milliseconds;
    int rc;

    do_gettimeofday(&currentTime);

    if (currentTime.tv_usec < initTimeStamp.tv_usec)
    {
        milliseconds = (currentTime.tv_usec - initTimeStamp.tv_usec + 1000000)/1000;
        currentTime.tv_sec--;
    }
    else{
        milliseconds = (currentTime.tv_usec - initTimeStamp.tv_usec)/1000;
    }

    /* Calculate the time   */
    hours = (currentTime.tv_sec - initTimeStamp.tv_sec)/3600;
    minutes = (((currentTime.tv_sec - initTimeStamp.tv_sec)/60))%60;
    seconds = (currentTime.tv_sec - initTimeStamp.tv_sec)%60;

    /* print the formatted time including the milliseconds  */
    rc = BKNI_Snprintf(timeStamp, size_t, "%02u:%02u:%02u.%03u", hours, minutes, seconds, milliseconds);
    return;

}

BERR_Code BDBG_P_OsInit(void)
{
    /* g_lock is statically initialized */
    return 0;
}

void BDBG_P_OsUninit(void)
{
}

/* Must protect against recursive entries with isr's and/or tasklets here */
/* This code should really all be in BKNI... and not here... */

/*******
 * WARNING: There may be a deadlock possibility using recursive spinlocks
 * if critical sections are implemented with them as well.  Because of this,
 * it is recommended that critical sections are implemented with tasklet_disable
 * and tasklet_enable rather than spin_lock_bh().
*******/
void BDBG_P_Lock(void)
{
    spin_lock_irqsave(&g_lock, g_lockFlags);
    return;
}

void BDBG_P_Unlock(void)
{
    spin_unlock_irqrestore(&g_lock, g_lockFlags);
    return;
}

/* End of file */
