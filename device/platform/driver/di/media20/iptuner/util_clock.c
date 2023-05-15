/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:		$Workfile:   util_md5.c  $
 * Version:			$Revision:   1.0  $
 * Original Author:		SungYong Jeon $
 * Current Author:	$Author: junsy@humaxdigital.com $
 * Date:			$Date: Fri June 05 10:20:48 KST 2009 $
 * File Description:	not so strong MD5 hashing
 * Module: 			          
 * Remarks:		
 */

/*******************************************************************/ 	
/* Copyright (c) 2009 HUMAX Co., Ltd. 								*/
/* All rights reserved.												*/
/*******************************************************************/


/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/
/* Start Including Header Files */
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>

#include "di_iptuner_common.h"
#include "di_iptuner_config.h"
#include "util_clock.h"
#include "vkernel.h"
/* End Including Headers */
/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
/* Start #define */
/* End #define */

/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
/* Start typedef */
/* End typedef */


/*******************************************************************/
/************************ global variables *************************/
/*******************************************************************/
/* Start global variable */

/* End global variable */


/*******************************************************************/
/************************ static variables *************************/
/*******************************************************************/
/* Start static variable */
/* End static variable */


/*******************************************************************/
/************************ static funtions **************************/
/*******************************************************************/
#include "nexus_base.h"

HULONG UTIL_GetClockMsec (void)
{
	NEXUS_Time nexus_time;
	unsigned long   msTick;

	NEXUS_Time_Get(&nexus_time);

#if defined (CONFIG_DEVICE_MODE_KERNEL)
#if (NEXUS_VERSION >= 1290)
	msTick = nexus_time;
#else
	msTick = (nexus_time.seconds*1000 + nexus_time.nanoseconds/1000000);
#endif
#else
	msTick = (nexus_time.tv_sec*1000 + nexus_time.tv_nsec/1000000);
#endif

	return msTick-(msTick%100);
}
HULONG UTIL_GetClock (void)
{
#if 0
	return (HLONG) clock();
#else
	return (HULONG) time(NULL);
#endif
}

long UTIL_GetTimevalDiff(const struct timeval *current, const struct timeval *past)
{
	return 1000*(current->tv_sec - past->tv_sec) + (current->tv_usec - past->tv_usec)/1000;
}

void UTIL_GetCurrentTimeval(struct timeval *time)
{
	int nret;

	nret = gettimeofday(time, NULL);
	if (nret!=0) {
		;
	}
	return;
}

