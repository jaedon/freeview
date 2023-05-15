/******************************************************************************
 *    (c)2008 Broadcom Corporation
 *
 * This program is the proprietary software of Broadcom Corporation and/or its licensors,
 * and may only be used, duplicated, modified or distributed pursuant to the terms and
 * conditions of a separate, written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
 * no license (express or implied), right to use, or waiver of any kind with respect to the
 * Software, and Broadcom expressly reserves all rights in and to the Software and all
 * intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
 * HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
 * NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
 *
 * Except as expressly set forth in the Authorized License,
 *
 * 1.     This program, including its structure, sequence and organization, constitutes the valuable trade
 * secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of Broadcom integrated circuit products.
 *
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
 * AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
 * WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
 * THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
 * OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
 * LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
 * OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
 * USE OR PERFORMANCE OF THE SOFTWARE.
 *
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
 * LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
 * EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
 * USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
 * ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
 * LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
 * ANY LIMITED REMEDY.
 *
 * $brcm_Workfile: settimeofday.c $
 * $brcm_Revision: Hydra_Software_Devel/3 $
 * $brcm_Date: 11/3/08 10:37a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/general/settimeofday.c $
 * 
 * Hydra_Software_Devel/3   11/3/08 10:37a erickson
 * PR42679: fix test, add comment
 *
 * Hydra_Software_Devel/2   10/23/08 3:59p erickson
 * PR42679: update
 *
 * Hydra_Software_Devel/1   7/9/08 12:05p erickson
 * PR44643: add settimeofday test
 *
 *****************************************************************************/
#include "nexus_platform.h"
#include <sys/time.h>

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>
#include "bstd.h"
#include "bkni.h"

bool done = false;
BKNI_EventHandle event;
int count = 0;

/**
This test verifies that KNI uses CLOCK_MONOTONIC, not calendar time.
**/

void *thread_func(void *data)
{
    BSTD_UNUSED(data);
    while (!done) {
        int rc = BKNI_WaitForEvent(event, 3000);
        printf("BKNI_WaitForEvent done: %d\n", rc);
        count++;
    }
    return NULL;
}

int main(void)
{
    struct timeval tv;
    pthread_t thread;
    unsigned loops = 100; /* do not change this without recalculating target */

    /* Bring up all modules for a platform in a default configuraiton for this platform */
    NEXUS_Platform_Init(NULL);
    BKNI_CreateEvent(&event);
    pthread_create(&thread, NULL, thread_func, NULL);

    while (--loops) {
        tv.tv_sec = rand() % 10000000;
        printf("Setting new time: %ld seconds\n", (long)tv.tv_sec);
        settimeofday(&tv, NULL);

        BKNI_Sleep(1000);
    }
    done = true;
    pthread_join(thread, NULL);
    BKNI_DestroyEvent(event);

    printf("count is %d. if KNI is resilient to settimeofday, count should be 34 (+/-1).\n", count);
    BDBG_ASSERT(count >= 33 && count <= 35);

    NEXUS_Platform_Uninit();

    return 0;
}
