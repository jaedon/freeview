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
 * $brcm_Workfile: memset32.c $
 * $brcm_Revision: Hydra_Software_Devel/5 $
 * $brcm_Date: 5/7/10 6:36p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/graphics2d/memset32.c $
 * 
 * Hydra_Software_Devel/5   5/7/10 6:36p mward
 * SW7125-253:  Allow compilation with C++, eliminate "invalid conversion"
 * errors.
 * 
 * Hydra_Software_Devel/4   9/23/09 11:40a erickson
 * SW7405-3062: update test to check memset32 results, invalid cases
 *
 * Hydra_Software_Devel/3   4/6/09 2:20p erickson
 * PR42679: update
 *
 * Hydra_Software_Devel/2   10/23/08 3:59p erickson
 * PR42679: update
 *
 * Hydra_Software_Devel/1   9/12/08 1:00p erickson
 * PR46844: added test
 *
 *****************************************************************************/
/* Nexus example app: show graphics framebuffer with CPU drawing */

#include "nexus_platform.h"
#include "nexus_surface.h"
#include "nexus_graphics2d.h"
#include "nexus_core_utils.h"
#include "bstd.h"
#include "bkni.h"
#include <stdlib.h>
#include <stdio.h>

BDBG_MODULE(memset32);

int main(void)
{
    NEXUS_Graphics2DHandle gfx;
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_Error rc;
    void *temp;
    uint8_t *mem;
    unsigned i;
    unsigned testsize;

    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    rc = NEXUS_Platform_Init(&platformSettings);
    BDBG_ASSERT(!rc);
    NEXUS_Platform_GetConfiguration(&platformConfig);

#define BUFSIZE (20 * 1024)
    rc = NEXUS_Memory_Allocate(BUFSIZE, NULL, &temp);
    BDBG_ASSERT(!rc);
    mem = (uint8_t *)temp;

    gfx = NEXUS_Graphics2D_Open(0, NULL);

    for (testsize = 1024; testsize < BUFSIZE/4; testsize += 1024) {
        printf("testing %d\n", testsize);
        BKNI_Memset(mem, 0xff, BUFSIZE);
        for (i=0;i<BUFSIZE;i++) {
            BDBG_ASSERT(mem[i] == 0xff);
        }
        NEXUS_Memory_FlushCache(mem, BUFSIZE);

        rc = NEXUS_Graphics2D_Memset32(gfx, mem, 0x00000000, testsize);
        BDBG_ASSERT(!rc);

        BKNI_Sleep(500); /* poor man's checkpoint */

        for (i=0;i<testsize*4;i++) {
            BDBG_ASSERT(mem[i] == 0x00);
        }
        for (;i<BUFSIZE;i++) {
            BDBG_ASSERT(mem[i] == 0xff);
        }
    }

    printf("These should fail\n");
    rc = NEXUS_Graphics2D_Memset32(gfx, mem, 0x00, 0);
    BDBG_ASSERT(rc);
    rc = NEXUS_Graphics2D_Memset32(gfx, mem, 0x00, 1);
    BDBG_ASSERT(rc);
    rc = NEXUS_Graphics2D_Memset32(gfx, mem, 0x00, 1023);
    BDBG_ASSERT(rc);
    rc = NEXUS_Graphics2D_Memset32(gfx, mem, 0x00, 1025);
    BDBG_ASSERT(rc);
    rc = NEXUS_Graphics2D_Memset32(gfx, mem, 0x00, (16 * 1024 * 1024) - 1);
    BDBG_ASSERT(rc);
    rc = NEXUS_Graphics2D_Memset32(gfx, mem, 0x00, (16 * 1024 * 1024) + 4096);
    BDBG_ASSERT(rc);

    NEXUS_Graphics2D_Close(gfx);
    NEXUS_Memory_Free(mem);
    NEXUS_Platform_Uninit();

    printf("Success\n");
    return 0;
}

