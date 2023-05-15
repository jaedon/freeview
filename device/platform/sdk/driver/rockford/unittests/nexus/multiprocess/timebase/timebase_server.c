/******************************************************************************
 *    (c)2010-2011 Broadcom Corporation
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
 * $brcm_Workfile: timebase_server.c $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 10/11/11 8:53p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/multiprocess/timebase/timebase_server.c $
 * 
 * Hydra_Software_Devel/1   10/11/11 8:53p bandrews
 * SW7231-391: parser band and timebase unit tests
 *****************************************************************************/
#include "nexus_types.h"
#include "nexus_platform.h"
#include "nexus_platform_server.h"
#include "nexus_timebase.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include "bstd.h"
#include "bkni.h"

BDBG_MODULE(timebase_server);

int main(int argc, char **argv)  
{
    NEXUS_Error rc;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_PlatformStartServerSettings serverSettings;
    int use_timebase_0 = 0;

    if (argc > 1)
    {
        use_timebase_0 = atoi(argv[1]);
    }

    rc = NEXUS_Platform_Init(NULL);
    if (rc) return -1;

    NEXUS_Platform_GetConfiguration(&platformConfig);

    if (use_timebase_0)
    {
        NEXUS_TimebaseSettings settings;
        printf("Server acquiring timebase 0...");
        NEXUS_Timebase_GetSettings(NEXUS_Timebase_e0, &settings);
        settings.sourceType = NEXUS_TimebaseSourceType_eFreeRun;
        NEXUS_Timebase_SetSettings(NEXUS_Timebase_e0, &settings);
        printf("PASS\n");
    }
    else
    {
        printf("Server not acquiring any timebases\n");
    }
    

    NEXUS_Platform_GetDefaultStartServerSettings(&serverSettings);
    serverSettings.allowUnauthenticatedClients = true; /* client is written this way */
    serverSettings.unauthenticatedConfiguration.mode = NEXUS_ClientMode_eProtected;
    serverSettings.unauthenticatedConfiguration.heap[0] = platformConfig.heap[0]; /* for purposes of example, allow access to main heap */
    rc = NEXUS_Platform_StartServer(&serverSettings);
    BDBG_ASSERT(!rc);

    printf("Hit enter to stop the server\n");
    getchar();
    
    NEXUS_Platform_Uninit();

    return 0;
}



