/******************************************************************************
 *    (c)2008-2010 Broadcom Corporation
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
 * $brcm_Workfile: watchdog.c $
 * $brcm_Revision: 1 $
 * $brcm_Date: 5/6/10 5:27p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/examples/io/watchdog.c $
 * 
 * 1   5/6/10 5:27p erickson
 * SW7400-2743: add watchdog
 *
 * Hydra_Software_Devel/1   4/20/09 10:38a jtna
 * PR54011: added watchdog
 *
 *****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "nexus_platform.h"
#include "nexus_watchdog.h"

int main(void)
{
    unsigned timeout = 30;
    bool status;
    bool started = false;
    NEXUS_Error rc;
    bool done = false;

    rc = NEXUS_Platform_Init(NULL);
    BDBG_ASSERT(!rc);

    rc = NEXUS_Watchdog_SetTimeout(timeout);
    BDBG_ASSERT(!rc);

    while (!done) {
#define BUFSIZE 64
        char buf[BUFSIZE];
        NEXUS_Watchdog_GetLastResetStatus(&status);

        printf(
        "Watchdog Status: \n"
        "  timeout:    %d seconds\n"
        "  started:    %s\n"
        "  last reset: %s\n"
        "\n"
        "Actions:\n"
        "1) set watchdog timeout value\n"
        "2) start or refresh watchdog\n"
        "3) stop watchdog\n"
        "4) exit (will not stop watchdog)\n",
        timeout,
        started?"yes, which means you must refresh within the timeout to avoid reset":"no",
        status?"by watchdog":"not by watchdog"
        );
        fgets(buf, BUFSIZE, stdin);

        switch (atoi(buf)) {
        case 1:
            printf("Enter timeout in seconds:\n");
            fgets(buf, BUFSIZE, stdin);
            timeout = atoi(buf);
            rc = NEXUS_Watchdog_SetTimeout(timeout);
            BDBG_ASSERT(!rc);
            break;
        case 2:
            rc = NEXUS_Watchdog_StartTimer();
            BDBG_ASSERT(!rc);
            started = true;
            break;
        case 3:
            rc = NEXUS_Watchdog_StopTimer();
            BDBG_ASSERT(!rc);
            started = false;
            break;
        case 4:
            done = true;
            break;
        }
    }

    NEXUS_Platform_Uninit();
    return 0;
}

