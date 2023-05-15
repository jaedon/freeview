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
 * $brcm_Workfile: bad_join.c $
 * $brcm_Revision: Hydra_Software_Devel/3 $
 * $brcm_Date: 11/18/11 1:22p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/multiprocess/bad_join.c $
 * 
 * Hydra_Software_Devel/3   11/18/11 1:22p erickson
 * SW7420-1148: randomize
 * 
 * Hydra_Software_Devel/2   10/25/11 2:21p erickson
 * SW7420-2107: add unittest that would have caught the failure
 *
 * Hydra_Software_Devel/1   4/14/11 3:08p erickson
 * SW7420-1671: update tests
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

BDBG_MODULE(bad_join);

int main(int argc, char **argv)
{
    unsigned loops = 10;
    int rc;
    pid_t pid;
    bool kernelmode_test = false;
    
    srand(time(NULL));
    
    if (argc > 1 && !strcmp(argv[1], "-k")) {
        kernelmode_test = true;
    }    

    while (--loops) {
        if (rand()%2) {
            BDBG_WRN(("BEGIN FAILURE1"));
            /* process cannot join when no server is initialized */
            rc = NEXUS_Platform_Join();
            assert(rc);
            BDBG_WRN(("END FAILURE1"));
        }

        fprintf(stderr, "server init\n");
        rc = NEXUS_Platform_Init(NULL);
        assert(!rc);

        if (rand()%2) {
            BDBG_WRN(("BEGIN FAILURE2"));
            /* cannot init twice */
            rc = NEXUS_Platform_Init(NULL);
            BDBG_ASSERT(rc);
            BDBG_WRN(("END FAILURE2"));
        }

        if (rand()%2) {
            /* process cannot join when it is the server */
            BDBG_WRN(("BEGIN FAILURE3"));
            rc = NEXUS_Platform_Join();
            BDBG_ASSERT(rc);
            BDBG_WRN(("END FAILURE3"));
        }

        NEXUS_Platform_Uninit();

        /* make sure we can come up and down */
        rc = NEXUS_Platform_Init(NULL);
        BDBG_ASSERT(!rc);

        if (kernelmode_test) {
            /* cannot fork and Join. you can fork/exec/Join, but this app doesn't test that. */
            pid = fork();
            if (!pid) {
                fprintf(stderr, "BEGIN FAILURE3\n");
                rc = NEXUS_Platform_Join();
                assert(rc); /* should fail */
                fprintf(stderr, "END FAILURE3\n");
                return 0;
            }
            else {
                int status;
                rc = waitpid(pid, &status, 0);
                BDBG_ASSERT(rc == pid);
                BDBG_ASSERT(WIFEXITED(status));
            }
        }
        
        NEXUS_Platform_Uninit();
        
        if (kernelmode_test) {
            /* you can Init/Uninit/Join (i.e. the server becomes a client, provided there is another server that starts). */
            int start_pipe[2];
            int stop_pipe[2];
            unsigned n;
            
            rc = pipe(start_pipe);
            assert(!rc);
            rc = pipe(stop_pipe);
            assert(!rc);
            
            pid = fork();
            if (!pid) {
                NEXUS_PlatformStartServerSettings serverSettings;
                
                close(start_pipe[0]);
                close(stop_pipe[1]);
                
                rc = NEXUS_Platform_Init(NULL);
                BDBG_ASSERT(!rc);
                
                NEXUS_Platform_GetDefaultStartServerSettings(&serverSettings);
                serverSettings.allowUnauthenticatedClients = true;
                serverSettings.unauthenticatedConfiguration.mode = NEXUS_ClientMode_eProtected;
                rc = NEXUS_Platform_StartServer(&serverSettings);
                BDBG_ASSERT(!rc);
                
                write(start_pipe[1], &n, sizeof(n));
                read(stop_pipe[0], &n, sizeof(n));
                
                NEXUS_Platform_StopServer();
                NEXUS_Platform_Uninit();
                return 0;
            }
            else {
                int status;
                
                close(start_pipe[1]);
                close(stop_pipe[0]);
                
                read(start_pipe[0], &n, sizeof(n));
                
                rc = NEXUS_Platform_Join();
                BDBG_ASSERT(!rc);
                NEXUS_Platform_Uninit();
                
                write(stop_pipe[1], &n, sizeof(n));
                
                rc = waitpid(pid, &status, 0);
                BDBG_ASSERT(rc == pid);
                BDBG_ASSERT(WIFEXITED(status));
            }
        }
    }

    printf("success\n");
    return 0;
}
