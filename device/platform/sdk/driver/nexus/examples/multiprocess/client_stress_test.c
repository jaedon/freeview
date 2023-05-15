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
 * $brcm_Workfile: client_stress_test.c $
 * $brcm_Revision: 3 $
 * $brcm_Date: 9/7/11 12:55p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/examples/multiprocess/client_stress_test.c $
 * 
 * 3   9/7/11 12:55p erickson
 * SW7420-1009: use NEXUS_ANY_ID for NEXUS_Graphics2D_Open
 * 
 * 2   1/21/11 3:11p erickson
 * SW7420-1123: use NEXUS_MAX_HEAPS for all client/server heap
 *  communication
 *
 * 1   12/7/10 5:47p erickson
 * SW7420-1148: refactor kernel mode proxy for process isolation
 *
 *****************************************************************************/

/*
This is an untrusted client.
We cannot #include nexus_platform.h, nexus_display.h
*/
#include "nexus_platform_client.h"
#include "nexus_surface.h"
#include "nexus_graphics2d.h"
#include "nexus_memory.h"
#include "nexus_base_mmap.h"
#include "bstd.h"
#include "bkni.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>

BDBG_MODULE(client_stress_test);

static void complete(void *data, int unused)
{
    BSTD_UNUSED(unused);
    BKNI_SetEvent((BKNI_EventHandle)data);
}

static void checkpoint(NEXUS_Graphics2DHandle gfx, BKNI_EventHandle event)
{
    int rc;
    rc = NEXUS_Graphics2D_Checkpoint(gfx, NULL);
    if (rc == NEXUS_GRAPHICS2D_QUEUED) {
        BKNI_WaitForEvent(event, 0xffffffff);
    }
}

int main(void)
{
    NEXUS_SurfaceHandle surface;
    NEXUS_SurfaceMemory mem;
    NEXUS_SurfaceCreateSettings createSettings;
    NEXUS_Graphics2DHandle gfx;
    NEXUS_Graphics2DSettings gfxSettings;
    NEXUS_Graphics2DFillSettings fillSettings;
    BKNI_EventHandle event;
    NEXUS_ClientConfiguration clientConfig;
    int i;
    NEXUS_Error rc;
    NEXUS_ClientAuthenticationSettings authSettings;

    printf("Test 1: join should fail if server requires authentication\n");
    rc = NEXUS_Platform_AuthenticatedJoin(NULL);
    assert(rc);


    printf("Test 2: join should fail with wrong cert\n");
    NEXUS_Platform_GetDefaultClientAuthenticationSettings(&authSettings);
    strcpy((char*)authSettings.certificate.data, "wrong");
    authSettings.certificate.length = 5;
    rc = NEXUS_Platform_AuthenticatedJoin(&authSettings);
    assert(rc);


    /* Now join should work */
    NEXUS_Platform_GetDefaultClientAuthenticationSettings(&authSettings);
    {
    /* simplistic string-based id for authentication. */
    char cert[] = "client_blit1234";
    strcpy((char*)authSettings.certificate.data, cert);
    authSettings.certificate.length = strlen(cert)+1;
    }
    rc = NEXUS_Platform_AuthenticatedJoin(&authSettings);
    assert(!rc);

    NEXUS_Platform_GetClientConfiguration(&clientConfig);
    /* print client heaps */
    for (i=0;i<NEXUS_MAX_HEAPS;i++) {
        NEXUS_MemoryStatus status;
        NEXUS_HeapHandle heap;

        heap = clientConfig.heap[i];
        if (!heap) break;
        rc = NEXUS_Heap_GetStatus(heap, &status);
        BDBG_ASSERT(!rc);
        printf("client heap[%d]: MEMC %d, offset %d (%#x), size %d (%#x), alignment %d\n",
            i, status.memcIndex, status.offset, status.offset, status.size, status.size, status.alignment);
    }

    /* Test 3: param validation on a variety of calls. These tests
    should be tried one at a time. Some may make the client crash but none should not crash the server. */
    NEXUS_Surface_GetDefaultCreateSettings(NULL);
    NEXUS_Surface_Create(NULL);
    NEXUS_Surface_GetDefaultCreateSettings((void*)0x12341234);
#if 0
    /* this will make the client crash. it should not make the server cras. */
    NEXUS_Surface_Create((void*)0x12341234);
#endif

    /* Test 4: a variety of non-pointer param validation on NEXUS_Surface_Create.
    This should not make the client or server crash */
    for (i=0;i<20;i++) {
        NEXUS_Surface_GetDefaultCreateSettings(&createSettings);
        createSettings.pixelFormat = NEXUS_PixelFormat_eMax + rand()%10000;
        createSettings.width = rand()%10000;
        createSettings.height = rand()%10000;
        surface = NEXUS_Surface_Create(&createSettings);
        BDBG_ASSERT(!surface);
    }

    NEXUS_Surface_GetDefaultCreateSettings(&createSettings);
    createSettings.width = 720;
    createSettings.height = 480;
    surface = NEXUS_Surface_Create(&createSettings);
    NEXUS_Surface_GetMemory(surface, &mem);

    BKNI_CreateEvent(&event);

    gfx = NEXUS_Graphics2D_Open(NEXUS_ANY_ID, NULL);
    NEXUS_Graphics2D_GetSettings(gfx, &gfxSettings);
    gfxSettings.checkpointCallback.callback = complete;
    gfxSettings.checkpointCallback.context = event;
    NEXUS_Graphics2D_SetSettings(gfx, &gfxSettings);

    NEXUS_Graphics2D_GetDefaultFillSettings(&fillSettings);
    fillSettings.surface = NULL;
    fillSettings.rect.width = rand();
    fillSettings.rect.height = rand();
    fillSettings.color = rand();
    rc = NEXUS_Graphics2D_Fill(gfx, &fillSettings);
    BDBG_ASSERT(rc);

    for (i=0;i<20;i++) {
        NEXUS_Graphics2D_GetDefaultFillSettings(&fillSettings);
        fillSettings.surface = (void*)rand();
        if (fillSettings.surface == surface) continue; /* just in case we get lucky */
        fillSettings.rect.width = rand();
        fillSettings.rect.height = rand();
        fillSettings.color = rand();
        rc = NEXUS_Graphics2D_Fill(gfx, &fillSettings);
        BDBG_ASSERT(rc);
    }

    BDBG_WRN(("success"));
    return 0;
}
