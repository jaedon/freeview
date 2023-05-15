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
 * $brcm_Workfile: register_client.c $
 * $brcm_Revision: 1 $
 * $brcm_Date: 9/20/11 10:15a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/examples/multiprocess/register_client.c $
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

BDBG_MODULE(register_client);

const char *modestr[] = {"untrusted", "protected", "unprotected", "invalid"};

int main(int argc, char **argv)
{
    int rc;
    NEXUS_ClientAuthenticationSettings auth;
    NEXUS_ClientConfiguration clientConfig;
    unsigned i;
    unsigned id = 0;
    
    if (argc > 1) {
        id = atoi(argv[1]); /* passed by register_client_server */
    }
    
    if (id) {
        NEXUS_Platform_GetDefaultClientAuthenticationSettings(&auth);
        auth.certificate.length = snprintf((char*)auth.certificate.data, 256, "client%d", id);
        rc = NEXUS_Platform_AuthenticatedJoin(&auth);
        if (rc) return -1;
        
        NEXUS_Platform_GetClientConfiguration(&clientConfig);
        
        printf("%s client joined with certificate '%s'\n", modestr[clientConfig.mode], auth.certificate.data);
    }
    else {
        rc = NEXUS_Platform_AuthenticatedJoin(NULL);
        if (rc) return -1;
        
        NEXUS_Platform_GetClientConfiguration(&clientConfig);
        
        printf("%s unauthenticated client joined\n", modestr[clientConfig.mode]);
    }
    
    /* print client heaps */
    for (i=0;i<NEXUS_MAX_HEAPS;i++) {
        NEXUS_MemoryStatus status;
        NEXUS_HeapHandle heap;

        heap = clientConfig.heap[i];
        if (!heap) continue;
        rc = NEXUS_Heap_GetStatus(heap, &status);
        BDBG_ASSERT(!rc);
        printf("client heap[%d]: MEMC %d, offset %#x, addr %p, size %d (%#x), alignment %d\n",
            i, status.memcIndex, status.offset, status.addr, status.size, status.size, status.alignment);
    }

    printf("Press ENTER to exit client\n");
    getchar();
        
    NEXUS_Platform_Uninit();
    
    return 0;
}
