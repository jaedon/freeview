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
 * $brcm_Revision: Hydra_Software_Devel/4 $
 * $brcm_Date: 1/26/09 10:33a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/dbg/linuxuser/bdbg_os_priv.c $
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
#include <sys/time.h>
#include <pthread.h>

static struct timeval initTimeStamp;
static pthread_mutex_t g_mutex = PTHREAD_MUTEX_INITIALIZER;

void
BDBG_P_InitializeTimeStamp(void)
{
    int rc;

    rc = gettimeofday(&initTimeStamp, NULL);
    if (rc!=0) {
        BDBG_P_PrintString("### debug: gettimeofday returned %d, ignored", rc);
    }
}

void
BDBG_P_GetTimeStamp(char *timeStamp, int size_t)
{
    struct timeval currentTime;
    int hours, minutes, seconds;
    int milliseconds;
    int rc;

    rc = gettimeofday(&currentTime, NULL);
    if (rc!=0) {
        BDBG_P_PrintString("### debug: gettimeofday returned %d, ignored", rc);
    }

    if (currentTime.tv_usec < initTimeStamp.tv_usec)
    {
        milliseconds = (currentTime.tv_usec - initTimeStamp.tv_usec + 1000000)/1000;
        currentTime.tv_sec--;
    }
    else    {
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
    /* g_mutex is statically initialized */
    return 0;
}

void BDBG_P_OsUninit(void)
{
}

/* NOTE: this function is called from both magnum task and isr context */
void BDBG_P_Lock(void)
{
    int rc;
    rc = pthread_mutex_lock(&g_mutex);
    BDBG_ASSERT(0 == rc);
    return;
}

/* NOTE: this function is called from both magnum task and isr context */
void BDBG_P_Unlock(void)
{
    pthread_mutex_unlock(&g_mutex);
    return;
}

/* End of file */
