/******************************************************************************
 *    (c)2008-2011 Broadcom Corporation
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
 * $brcm_Workfile: simple_server.c $
 * $brcm_Revision: Hydra_Software_Devel/7 $
 * $brcm_Date: 2/16/12 11:09a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/multiprocess/simple_server.c $
 * 
 * Hydra_Software_Devel/7   2/16/12 11:09a mward
 * SW7420-1148:  Compiler warning for unused variable "unprotected".
 * 
 * Hydra_Software_Devel/5   11/18/11 1:22p erickson
 * SW7420-1148: add -timeout, add -mode, set unauthenticated heap
 * 
 * Hydra_Software_Devel/4   10/12/11 12:25p erickson
 * SW7420-1148: api change
 * 
 * Hydra_Software_Devel/3   9/7/11 5:31p erickson
 * SW7420-1148: must set server to allow unauthenticated clients
 * 
 * Hydra_Software_Devel/2   8/8/11 5:03p erickson
 * SW7420-1689: add -secure and -timeout X option
 * 
 * Hydra_Software_Devel/1   7/29/11 12:06p erickson
 * SW7420-1991: add multiprocess tests for nelem and nelem_out attributes
 * 
 *****************************************************************************/
#include "bstd.h"
#include "nexus_platform.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

BDBG_MODULE(boot);

int main(int argc, char **argv)
{
    NEXUS_Error rc;
    int curarg = 1;
    unsigned timeout = 2 * 60; /* 2 min */
    NEXUS_PlatformStartServerSettings serverSettings;
    NEXUS_ClientMode defaultClientMode = NEXUS_ClientMode_eProtected;
    
    while (curarg < argc) {
        if (!strcmp(argv[curarg], "-mode") && curarg+1 < argc) {
            ++curarg;
            if (!strcmp("untrusted", argv[curarg])) {
                defaultClientMode = NEXUS_ClientMode_eUntrusted;
            }
            else if (!strcmp("protected", argv[curarg])) {
                defaultClientMode = NEXUS_ClientMode_eProtected;
            }
            else if (!strcmp("unprotected", argv[curarg])) {
                defaultClientMode = NEXUS_ClientMode_eUnprotected;
            }
        }
        else if (!strcmp(argv[curarg], "-timeout") && curarg+1 < argc) {
            timeout = atoi(argv[++curarg]);
        }
        curarg++;
    }
    
    rc = NEXUS_Platform_Init(NULL);
    if (rc) return -1;
    
    NEXUS_Platform_GetDefaultStartServerSettings(&serverSettings);
    serverSettings.allowUnauthenticatedClients = true; /* allow, but don't trust by default */
    serverSettings.unauthenticatedConfiguration.mode = defaultClientMode;
    serverSettings.unauthenticatedConfiguration.heap[0] = NEXUS_Platform_GetFramebufferHeap(NEXUS_OFFSCREEN_SURFACE);
    rc = NEXUS_Platform_StartServer(&serverSettings);
    if (rc) return -1;
    
    if (timeout) {
        BKNI_Sleep(timeout * 1000);
    }
    else {
        printf("Press ENTER to shutdown server\n");
        getchar();
    }
    
    NEXUS_Platform_StopServer();
    NEXUS_Platform_Uninit();

    return 0;
}
