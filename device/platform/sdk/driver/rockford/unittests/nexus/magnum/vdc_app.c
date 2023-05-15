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
 * $brcm_Workfile: vdc_app.c $
 * $brcm_Revision: Hydra_Software_Devel/4 $
 * $brcm_Date: 9/29/11 2:47p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/magnum/vdc_app.c $
 * 
 * Hydra_Software_Devel/4   9/29/11 2:47p erickson
 * SW7425-1343: backout last change
 * 
 * Hydra_Software_Devel/2   4/2/09 4:20p shyam
 * PR53323: Need to remove frontend initialization
 * 
 * Hydra_Software_Devel/1   3/19/09 4:58p erickson
 * PR53323: add test app for calling magnum directly from app
 *
 *****************************************************************************/
#include <stdio.h>
#include "nexus_types.h"
#include "nexus_platform.h"
#include "bvdc.h"
#include "priv/nexus_core.h"

/**
This app and Makefile shows how to run a minimal nexus platform
and then open magnum modules directly from the application.

Put ./nexus_platform_features.h into nexus/platform/$PLATFORM/include.
Put ./platform_modules.inc into nexus/platform/$PLATFORM/build.
**/

int main(void)
{
    BVDC_Settings vdcCfg;
    BVDC_Handle vdc;
    BRDC_Handle rdc;
    NEXUS_Error rc;
    NEXUS_PlatformSettings platformSettings;

    printf("starting nexus platform - base and core only\n");
    NEXUS_Platform_GetDefaultSettings(&platformSettings);

    platformSettings.openI2c = false;
    platformSettings.openFrontend = false;
    platformSettings.openFpga=false;
    platformSettings.openOutputs = false;
    rc = NEXUS_Platform_Init(NULL);
    BDBG_ASSERT(!rc);

    printf("starting rdc\n");
    rc = BRDC_Open(&rdc, g_pCoreHandles->chp, g_pCoreHandles->reg, g_pCoreHandles->heap[0], NULL);
    BDBG_ASSERT(!rc);

    BVDC_GetDefaultSettings(&vdcCfg);
    printf("starting vdc\n");
    rc = BVDC_Open(&vdc, g_pCoreHandles->chp, g_pCoreHandles->reg, g_pCoreHandles->heap[0], g_pCoreHandles->bint, rdc, g_pCoreHandles->tmr, &vdcCfg);
    BDBG_ASSERT(!rc);

    printf("shutdown\n");
    BVDC_Close(vdc);
    BRDC_Close(rdc);
    NEXUS_Platform_Uninit();

    printf("success\n");
    return 0;
}

