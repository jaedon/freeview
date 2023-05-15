/***************************************************************************
 *     (c)2011 Broadcom Corporation
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
 * $brcm_Workfile: register_server.c $
 * $brcm_Revision: 1 $
 * $brcm_Date: 9/20/11 10:15a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/examples/multiprocess/register_server.c $
 * 
 * 1   9/20/11 10:15a erickson
 * SW7420-1148: add example of authenticated and unauthenticated clients
 * 
 **************************************************************************/

#include "nexus_platform.h"
#include "bstd.h"
#include "bkni.h"
#include "bkni_multi.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <errno.h>
#include <time.h>

BDBG_MODULE(register_server);

/* server */
int main(void)
{
    NEXUS_Error rc;
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_PlatformStartServerSettings serverSettings;
    
    srand(time(NULL));
    
    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    rc = NEXUS_Platform_Init(&platformSettings);
    if (rc) return -1;
    
    NEXUS_Platform_GetConfiguration(&platformConfig);
    
    NEXUS_Platform_GetDefaultStartServerSettings(&serverSettings);
    serverSettings.allowUnauthenticatedClients = true;
    serverSettings.unauthenticatedConfiguration.mode = NEXUS_ClientMode_eUntrusted;
    serverSettings.unauthenticatedConfiguration.heap[2] = platformConfig.heap[0]; /* doesn't have to be 0-->0 mapping */
    rc = NEXUS_Platform_StartServer(&serverSettings);
    BDBG_ASSERT(!rc);
    
    while (1) {
        unsigned id;
        unsigned pid;
        NEXUS_ClientHandle client;
        
        if (rand() % 2) {
            NEXUS_ClientSettings clientSettings;
            
            id = rand();
            printf("Press ENTER to fork authenticated client %d\n", id);
            getchar();
            
            /* register a new client*/
            NEXUS_Platform_GetDefaultClientSettings(&clientSettings);
            clientSettings.authentication.certificate.length = snprintf((char*)clientSettings.authentication.certificate.data, 256, "client%d", id);
            clientSettings.configuration.mode = NEXUS_ClientMode_eProtected;
            clientSettings.configuration.heap[0] = platformConfig.heap[0];
            clientSettings.configuration.heap[1] = platformConfig.heap[1];
            client = NEXUS_Platform_RegisterClient(&clientSettings);
            BDBG_ASSERT(client);
        }
        else {
            id = 0;
            printf("Press ENTER to fork unauthenticated client\n");
            getchar();
            
            client = NULL;
        }
        
        pid = fork();
        if (!pid) {
            char client_num[64];
            const char *client_name = "register_client";
            snprintf(client_num, 64, "%d", id);
            rc = execlp(client_name, client_name, client_num, NULL);
            /* will not return unless there's an error */
            printf("execlp failure: %d %d\n", rc, errno);
            return -1;
        }
        else if (pid != 0) {
            /* wait for client to exit */
            waitpid(pid, 0, 0);
        }
        else {
            return -1;
        }
        
        if (client) {
            NEXUS_Platform_UnregisterClient(client);
        }
    }
    
    NEXUS_Platform_Uninit();
    return 0;
}
