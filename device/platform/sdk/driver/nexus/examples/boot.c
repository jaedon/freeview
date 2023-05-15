/******************************************************************************
 *    (c)2008-2012 Broadcom Corporation
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
 * $brcm_Workfile: boot.c $
 * $brcm_Revision: 8 $
 * $brcm_Date: 3/27/12 3:47p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/examples/boot.c $
 * 
 * 8   3/27/12 3:47p erickson
 * SW7231-639: add chipRevision to print out
 *
 * 7   11/15/11 3:21p erickson
 * SW7425-1747: remove old code
 *
 * 6   2/16/11 3:06p jhaberf
 * SW35230-161: defined NEXUS_PlatformStatus only for non-DTV platforms
 *
 * 5   2/15/11 3:59p erickson
 * SW7420-1491: add NEXUS_Platform_GetReleaseVersion
 *
 * 4   10/27/10 3:18p erickson
 * SW7422-69: fix dtv
 *
 * 3   6/24/10 11:51a erickson
 * SW7400-2806: add NEXUS_Platform_GetStatus
 *
 * 2   7/18/08 11:31a jgarrett
 * PR 44953: Removing -Wstrict-prototypes warnings
 *
 *****************************************************************************/
#include <stdio.h>
#include "bstd.h"
#include "nexus_types.h"
#include "nexus_platform.h"

BDBG_MODULE(boot);

int main(void)
{
    NEXUS_Error rc;

    while (1) {
        NEXUS_PlatformStatus platformStatus;
        char versionString[256];

        /* NOTE: can't use KNI or DBG before NEXUS_Platform_Init or after NEXUS_Platform_Uninit. Nexus/magnum stack must be up. */
        fprintf(stderr, "Ready to initialize\n");

        rc = NEXUS_Platform_Init(NULL);
        if ( rc ) {
            fprintf(stderr, "Unable to initialize nexus: %d\n", rc);
            return -1;
        }

        rc = NEXUS_Platform_GetStatus(&platformStatus);
        BDBG_ASSERT(!rc);
        NEXUS_Platform_GetReleaseVersion(versionString, 256);

        BDBG_WRN(("Initialization of BCM%04x %c%c Nexus, release %s, complete. Press ENTER to uninit.",
            platformStatus.chipId,
            ((platformStatus.chipRevision>>4)&0xF) + 'A' - 1,
            ((platformStatus.chipRevision)&0xF) + '0',
            versionString));
        getchar();

        NEXUS_Platform_Uninit();

        fprintf(stderr, "Uninit complete. Press ENTER to init again.\n");
        getchar();
    }

    return 0;
}

