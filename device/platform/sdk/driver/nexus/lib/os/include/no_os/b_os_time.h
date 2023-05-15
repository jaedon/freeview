/***************************************************************************
*     (c)2004-2011 Broadcom Corporation
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
* $brcm_Workfile: b_os_time.h $
* $brcm_Revision: 5 $
* $brcm_Date: 5/12/11 6:03p $
*
* Description:
*   API name: OS
*    Library routines for OS abstraction
*
* Revision History:
*
* $brcm_Log: /nexus/lib/os/include/no-os/b_os_time.h $
* 
* 5   5/12/11 6:03p agin
* SWNOOS-458:  Fix compiler warnings.
* 
* 4   7/12/09 8:15p agin
* PR56698: Create timeval2 data structure.
* 
* 3   5/20/09 4:46p agin
* PR53502: SDE does not need sys\time.h at this moment.
* 
* 2   5/20/09 3:02p agin
* PR53502: Support both GHS and SDE.
* 
* 1   6/12/08 4:55p agin
* PR43579: Check in changes for 20080530 BCM97325 non-os release.
* 
***************************************************************************/
#ifndef B_OS_TIME_H__
#define B_OS_TIME_H__

#if defined(GHS)
#include <time.h>
#elif defined(MIPS_SDE)
/*#include <sys/time.h>*/
#endif

#ifdef __cplusplus
extern "C" {
#endif

struct timeval2 {
	long	tv_sec;		/* seconds */
	long	tv_usec;	/* microseconds */
/*	unsigned long ticks; */
};

typedef struct timeval2 B_Time;

#define B_TIME_ZERO {0,0}

#ifdef __cplusplus
}
#endif

#endif /* #ifndef B_OS_TIME_H__ */

