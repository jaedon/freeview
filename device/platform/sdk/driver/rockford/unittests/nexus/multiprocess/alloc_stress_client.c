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
 * $brcm_Workfile: alloc_stress_client.c $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 9/17/12 9:34a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/multiprocess/alloc_stress_client.c $
 * 
 * Hydra_Software_Devel/2   9/17/12 9:34a erickson
 * SW7420-1671: don't assert on null handle
 * 
 * Hydra_Software_Devel/1   4/14/11 3:08p erickson
 * SW7420-1671: update tests
 *
 * Hydra_Software_Devel/1   3/23/11 5:03p erickson
 * SW7420-1671: split apps so they can run in kernel mode and user mode
 *
 *****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>
#include "nexus_types.h"
#include "nexus_platform.h"
#include "nexus_graphics2d.h"

BDBG_MODULE(alloc_stress_client);

#define TEST_EXPECTED_FAILURES 0
#define MAX_CLIENTS 100

int main(int argc, char **argv)
{
    NEXUS_Error rc;
    unsigned i;
#define NUM_HANDLES 10
    NEXUS_SurfaceHandle surface[NUM_HANDLES];
    void *allocations[NUM_HANDLES];
    unsigned loops = rand() % 100;
    NEXUS_ClientAuthenticationSettings authSettings;
    unsigned client_index = 0;

    if (argc > 1) {
        client_index = atoi(argv[1]);
    }

    NEXUS_Platform_GetDefaultClientAuthenticationSettings(&authSettings);
    sprintf((char*)authSettings.certificate.data, "testclient%d", client_index);
    authSettings.certificate.length = strlen((const char *)authSettings.certificate.data)+1;
    rc = NEXUS_Platform_AuthenticatedJoin(&authSettings);
    if (rc) {
        fprintf(stderr, "client %d(%d) NEXUS_Platform_Join failed: %d\n", client_index, getpid(), rc);
        return rc;
    }

    BDBG_WRN(("client %d(%d) joined", client_index, getpid()));

    /* brief client stress */
    BKNI_Memset(surface, 0, sizeof(surface));
    BKNI_Memset(allocations, 0, sizeof(allocations));
    for (i=0;i<loops;i++) {
        unsigned index = rand() % NUM_HANDLES;
        switch (rand() % 5) {
        case 0:
            BKNI_Sleep(rand() % 40);
            break;
        case 1:
            if (!surface[index]) {
                NEXUS_SurfaceCreateSettings settings;
                NEXUS_Surface_GetDefaultCreateSettings(&settings);
                settings.width = 10;
                settings.height = 10;
                surface[index] = NEXUS_Surface_Create(&settings);
                /* may be NULL if client is terminated */
            }
            break;
        case 2:
            if (surface[index]) {
                NEXUS_Surface_Destroy(surface[index]);
                surface[index] = NULL;
            }
            break;
        case 3:
            if (!allocations[index]) {
                NEXUS_MemoryAllocationSettings allocSettings;
                /* allocSettings required for untrusted clients */
                NEXUS_Memory_GetDefaultAllocationSettings(&allocSettings);
                rc = NEXUS_Memory_Allocate(rand()%100+1, &allocSettings, &allocations[index]);
                /* may fail if client is terminated */
                BSTD_UNUSED(rc);
            }
            break;
        case 4:
            if (allocations[index]) {
                NEXUS_Memory_Free(allocations[index]);
                allocations[index] = NULL;
            }
            break;
        }
    }
    /* don't close all handles */

    /* uninit sometimes, other times just exit */
    if (rand() % 2) {
        NEXUS_Platform_Uninit();
    }

    return 0;
}
