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
 * $brcm_Workfile: stress_server.c $
 * $brcm_Revision: Hydra_Software_Devel/13 $
 * $brcm_Date: 8/16/12 4:36p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/multiprocess/stress_server.c $
 * 
 * Hydra_Software_Devel/13   8/16/12 4:36p erickson
 * SW7435-303: use NEXUS_Platform_GetClientStatus to verify client
 * functionality
 * 
 * Hydra_Software_Devel/12   11/4/11 3:31p erickson
 * SW7420-1671: kill clients
 * 
 * Hydra_Software_Devel/11   10/7/11 3:55p erickson
 * SW7420-1148: update
 * 
 * Hydra_Software_Devel/10   9/7/11 5:31p erickson
 * SW7420-1148: must set server to allow unauthenticated clients
 * 
 * Hydra_Software_Devel/9   4/14/11 3:08p erickson
 * SW7420-1671: update tests
 *
 * Hydra_Software_Devel/8   4/8/11 2:20p erickson
 * SW7420-1671: don't require that clients exit before removing server
 *
 * Hydra_Software_Devel/7   3/30/11 12:40p erickson
 * SW7420-1671: tweak WRN
 *
 * Hydra_Software_Devel/6   3/25/11 9:32a erickson
 * SW7420-1671: set
 * NEXUS_PlatformStartServerSettings.trustUnauthenticatedClients
 *
 * Hydra_Software_Devel/5   3/23/11 5:03p erickson
 * SW7420-1671: split apps so they can run in kernel mode and user mode
 *
 * Hydra_Software_Devel/4   3/22/11 1:28p erickson
 * SW7420-1671: add more stress: have client open handles, leave some
 * open, sometimes uninit and sometimes terminate abnormally
 *
 * Hydra_Software_Devel/3   3/21/11 1:15p erickson
 * SW7420-1671: increase clients and default loops, decrease sleep time
 *
 * Hydra_Software_Devel/2   3/21/11 10:19a erickson
 * SW7420-1671: fix #define
 *
 * Hydra_Software_Devel/1   3/18/11 5:33p erickson
 * SW7420-1671: add join_stress
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
#include <assert.h>
#include <time.h>
#include "nexus_types.h"
#include "nexus_platform.h"

BDBG_MODULE(stress_server);

/* copied from bcm_driver.c */
#define BRCM_MAX_CLIENTS 16

struct {
    pid_t pid;
    NEXUS_ClientHandle client;
} g_client[BRCM_MAX_CLIENTS];

/*
the stress_server launches a variety of clients to provide multiprocess client/server stress tests.
usage:

    // params
    nexus stress_server [APP NUM_MAIN_LOOPS NUM_SUB_LOOPS]

    // runs with default params
    nexus stress_server

    // launch a specific client
    nexus stress_server join_stress_client
    nexus stress_server alloc_stress_client

    // launch a specific client with a specific # of loops
    nexus stress_server myclient 100 10
    
during the stress test, you will see lots of error messages on the console. these come from the clients.
the key is: does the server survive and print "success" in the end?
*/

int main(int argc, char **argv)
{
    /* default number of loops set so test runs in <= 3 minutes */
    unsigned loop0 = 5;
    unsigned loop1 = 250;
    NEXUS_Error rc;
    const char *client_name = "join_stress_client";
    unsigned total_joins = 0;

    srand(time(NULL));

    if (argc > 1) {
        client_name = argv[1];
    }
    if (argc > 2) {
        loop0 = atoi(argv[2]);
    }
    if (argc > 3) {
        loop1 = atoi(argv[3]);
    }

    while (loop0--) {
        unsigned loop = loop1;
        NEXUS_PlatformStartServerSettings serverSettings;
        NEXUS_ClientSettings clientSettings;
        NEXUS_PlatformConfiguration platformConfig;
        unsigned i;

        fprintf(stderr, "server init\n");
        rc = NEXUS_Platform_Init(NULL);
        if (rc) {
            fprintf(stderr, "NEXUS_Platform_Init failed: %d\n", rc);
            return -1;
        }
        NEXUS_Platform_GetConfiguration(&platformConfig);

#if 0
/* if you run this test in a loop, this type of termination is helpful.
   run as follows: while [ 1 ]; do nexus join_stress_server || break; done
*/
        if (rand() % 6 == 0) {
            return 0;
        }
        if (rand() % 6 == 0) {
            NEXUS_Platform_Uninit();
            return 0;
        }
#endif

        NEXUS_Platform_GetDefaultStartServerSettings(&serverSettings);
        serverSettings.allowUnauthenticatedClients = true;
        serverSettings.unauthenticatedConfiguration.mode = NEXUS_ClientMode_eProtected;
        serverSettings.unauthenticatedConfiguration.heap[0] = platformConfig.heap[0];
        rc = NEXUS_Platform_StartServer(&serverSettings);
        BDBG_ASSERT(!rc);

        for (i=0;i<BRCM_MAX_CLIENTS;i++) {
            NEXUS_Platform_GetDefaultClientSettings(&clientSettings);
            sprintf((char*)clientSettings.authentication.certificate.data, "testclient%d", i);
            clientSettings.authentication.certificate.length = strlen((const char *)clientSettings.authentication.certificate.data)+1;
            clientSettings.configuration.mode = NEXUS_ClientMode_eProtected;
            clientSettings.configuration.heap[0] = platformConfig.heap[0];
            g_client[i].client = NEXUS_Platform_RegisterClient(&clientSettings);
            BDBG_ASSERT(!rc);
        }

        BDBG_WRN(("%d client loops", loop));
        while (loop--) {
            unsigned client = rand() % BRCM_MAX_CLIENTS;
            switch (rand() % 4) {
            case 0:
                /* fork a new client */
                if (!g_client[client].pid) {
                    char client_num[10];
                    unsigned pid;

                    sprintf(client_num, "%d", client);
                    pid = fork();
                    if (pid > 0) {
                        /* server */
                        BDBG_WRN(("client %d: fork %d", client, pid));
                        g_client[client].pid = pid;
                    }
                    else if (pid == 0) {
                        /* exec client so that memory is cleared */
                        rc = execlp(client_name, client_name, client_num, NULL);
                        /* will not return unless there's an error */
                        printf("execlp %s failure: %d %d\n", argv[0], rc, errno);
                        return -1;
                    }
                    else {
                        BDBG_ERR(("fork error: %d %d", rc, errno));
                    }
                }
                break;
            case 1:
                /* kill a new client */
                if (g_client[client].pid) {
                    NEXUS_ClientStatus status;
                    
                    BDBG_WRN(("client %d: kill/wait %d", client, g_client[client].pid));
                    rc = kill(g_client[client].pid, 9);
                    BDBG_ASSERT(!rc);
                    rc = waitpid(g_client[client].pid, 0, 0);
                    BDBG_ASSERT(rc == (unsigned)g_client[client].pid);
                    g_client[client].pid = 0;
                    
                    rc = NEXUS_Platform_GetClientStatus(g_client[client].client, &status);
                    BDBG_ASSERT(!rc);
                    total_joins += status.numJoins;
                }
                break;
#if EXPECT_CHILDREN_TO_TERMINATE
            case 2:
                /* wait for child to exit. no child should run indefinitely. */
                if (g_client[client].pid) {
                    BDBG_WRN(("client %d: clean wait for %d", client, g_client[client].pid));
                    rc = waitpid(g_client[client].pid, 0, 0);
                    BDBG_ASSERT(rc == (unsigned)g_client[client].pid);
                    g_client[client].pid = 0;
                }
                break;
#endif
            case 3:
                BKNI_Sleep(rand() % 50);
                break;
            }
        }

#define WAIT_FOR_CHILDREN 1
#if WAIT_FOR_CHILDREN
        /* clean wait for all children before stopping */
        for (i=0;i<BRCM_MAX_CLIENTS;i++) {
            if (g_client[i].pid) {
                BDBG_WRN(("client %d: terminal clean wait for %d", i, g_client[i].pid));
#if !EXPECT_CHILDREN_TO_TERMINATE
                rc = kill(g_client[i].pid, 9);
                BDBG_ASSERT(!rc);
#endif
                rc = waitpid(g_client[i].pid, 0, 0);
                BDBG_ASSERT(rc == (unsigned)g_client[i].pid);
                g_client[i].pid = 0;
            }
        }
#endif
        
        fprintf(stderr, "server uninit\n");
        for (i=0;i<BRCM_MAX_CLIENTS;i++) {
            NEXUS_Platform_UnregisterClient(g_client[i].client);
        }
        NEXUS_Platform_StopServer();
        
#if !WAIT_FOR_CHILDREN
        /* kill and wait for all children after stopping. if the server has 
        already stopped, the client sockets may be stuck, so the kill is required. */
        for (i=0;i<BRCM_MAX_CLIENTS;i++) {
            if (g_client[i].pid) {
                BDBG_WRN(("client %d: terminal kill/wait for %d", i, g_client[i].pid));
                rc = kill(g_client[i].pid, 9);
                BDBG_ASSERT(!rc);
                rc = waitpid(g_client[i].pid, 0, 0);
                BDBG_ASSERT(rc == (unsigned)g_client[i].pid);
                g_client[i].pid = 0;
            }
        }
#endif
        
        NEXUS_Platform_Uninit();
        printf("%d joins\n", total_joins);
    }

    printf("%d total joins\n", total_joins);
    /* if total_joins falls below this, something has likely gone wrong with the client/server connection */
    BDBG_ASSERT(total_joins >= 50);
    printf("success\n");
    return 0;
}
