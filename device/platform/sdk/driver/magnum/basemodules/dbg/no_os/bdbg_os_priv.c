/***************************************************************************
 *     Copyright (c) 2006-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bdbg_os_priv.c $
 * $brcm_Revision: Hydra_Software_Devel/7 $
 * $brcm_Date: 5/12/11 6:02p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/dbg/no-os/bdbg_os_priv.c $
 * 
 * Hydra_Software_Devel/7   5/12/11 6:02p agin
 * SWNOOS-458:  Fix compiler warnings.
 * 
 * Hydra_Software_Devel/6   10/27/10 4:40p agin
 * SWNOOS-429:  Remove unnecessary include.
 * 
 * Hydra_Software_Devel/5   10/24/10 8:37p agin
 * SWNOOS-429:  Remove unnecessary define.
 * 
 * Hydra_Software_Devel/4   5/24/10 1:30p enavarro
 * SW35230-123: include nexus header only for nexus-based no-os
 * 
 * Hydra_Software_Devel/3   7/10/09 3:09p agin
 * PR56698: Comment out calls to BKNI_AcquireMutex and BKNI_ReleaseMutex.
 * 
 * Hydra_Software_Devel/2   2/28/09 9:48p agin
 * PR52583: Fixed BDBG_P_GetTimeStamp.
 * 
 * Hydra_Software_Devel/1   2/16/09 1:57p agin
 * PR52128: New file for no-os.
 * 
 ***************************************************************************/
#include "bstd.h"
#include "bkni.h"
#include "bkni_multi.h"

/* platform spefic type for timestamps */
typedef struct vxworks_timeval {
    uint32_t	tv_sec;     /* seconds */
    uint32_t	tv_usec;    /* microseconds */
}vxworks_timeval;

static vxworks_timeval initTimeStamp;

static void
BDBG_P_GetTime(vxworks_timeval *pTime)
{
	#if 1
	BSTD_UNUSED(pTime);
	#else
	int rc;
	struct timespec vxworks_time;

	rc = clock_gettime(CLOCK_REALTIME, &vxworks_time);
	if (rc!=0) {
		BDBG_P_PrintString("### debug: clock_gettime returned %d, ignored", rc);
	}
	else {
		pTime->tv_sec  = vxworks_time.tv_sec;
		pTime->tv_usec = vxworks_time.tv_nsec / 1000;	/* convert to usec */
		if (pTime->tv_usec >= 1000000)	 /* if this isn't right, things go bad downstream */
		{
			BDBG_P_PrintString("!!! Assert (time->tv_usec >= 1000000) Failed at %s:%d\n", __FILE__, __LINE__);
    	BKNI_Fail();
    }
	}
	#endif
	return;
}

void
BDBG_P_InitializeTimeStamp(void)
{
	BDBG_P_GetTime(&initTimeStamp);
}

void
BDBG_P_GetTimeStamp(char *timeStamp, int size_t)
{
	#if 0
  vxworks_timeval currentTime;
	int hours, minutes, seconds;
 	int milliseconds;
	int rc;
	
	BDBG_P_GetTime(&currentTime);
	
	if (currentTime.tv_usec < initTimeStamp.tv_usec)
	{
		milliseconds = (currentTime.tv_usec - initTimeStamp.tv_usec + 1000000)/1000;
		currentTime.tv_sec--;
	}
	else	{
		milliseconds = (currentTime.tv_usec - initTimeStamp.tv_usec)/1000;
	}
	
	/* Calculate the time	*/
	hours = (currentTime.tv_sec - initTimeStamp.tv_sec)/3600;
	minutes = (((currentTime.tv_sec - initTimeStamp.tv_sec)/60))%60;
	seconds = (currentTime.tv_sec - initTimeStamp.tv_sec)%60;
		
	/* print the formatted time including the milliseconds	*/
	rc = BKNI_Snprintf(timeStamp, size_t, "%02u:%02u:%02u.%03u", hours, minutes, seconds, milliseconds);
        #else
	BKNI_Snprintf(timeStamp, size_t, "%02u:%02u:%02u.%03u", 0, 0, 0, 0);
        #endif
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

void BDBG_P_Lock(void)
{
/*	BKNI_AcquireMutex(mutex); */
	return;
}

void BDBG_P_Unlock(void)
{
/*	BKNI_ReleaseMutex(mutex);  */
	return;
}

/* End of file */
