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
 * $brcm_Workfile: pwm.c $
 * $brcm_Revision: Hydra_Software_Devel/4 $
 * $brcm_Date: 6/16/10 1:26p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/pwm/pwm.c $
 * 
 * Hydra_Software_Devel/4   6/16/10 1:26p erickson
 * SW7405-4444: refactor NEXUS_Pwm_OpenChannel for kernel mode support
 *
 * Hydra_Software_Devel/3   5/7/10 6:37p mward
 * SW7125-253:  Allow compilation with C++, eliminate "invalid conversion"
 * errors.
 *
 * Hydra_Software_Devel/2   6/1/09 9:23a erickson
 * PR42679: fix for DTV
 *
 * Hydra_Software_Devel/1   1/27/09 10:52a erickson
 * PR42679: added pwm unit test
 *
 *****************************************************************************/
#include "nexus_platform.h"
#include "nexus_pwm.h"

#include "bstd.h"
#include "bkni.h"
#include <stdlib.h>
#include <stdio.h>

int main(void)
{
    NEXUS_PwmChannelHandle pwm, pwm2;
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_PwmChannelSettings pwmSettings;
    uint16_t word;
    NEXUS_Error rc;

    /* Bring up all modules for a platform in a default configuration for this platform */
    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openOutputs = false; /* for DTV systems, opening the panel output causes the pwm to be opened. disable that for this test. */
    NEXUS_Platform_Init(&platformSettings);
    NEXUS_Platform_GetConfiguration(&platformConfig);

#if NEXUS_DTV_PLATFORM
    NEXUS_Pwm_GetDefaultChannelSettings(NEXUS_PwmChannelNumber_e0, &pwmSettings);
    pwm = NEXUS_Pwm_OpenChannel(&pwmSettings);

    NEXUS_Pwm_GetControlWord(pwm, &word);
    rc = NEXUS_Pwm_SetControlWord(pwm, word);
    BDBG_ASSERT(!rc);

    NEXUS_Pwm_CloseChannel(pwm);

    pwm = NEXUS_Pwm_OpenChannel(&pwmSettings);
    BDBG_ASSERT(pwm);
    printf("next call should fail\n");
    pwm2 = NEXUS_Pwm_OpenChannel(&pwmSettings);
    BDBG_ASSERT(!pwm2);
    NEXUS_Pwm_CloseChannel(pwm);
#else
    NEXUS_Pwm_GetDefaultChannelSettings(&pwmSettings);
    pwm = NEXUS_Pwm_OpenChannel(0, &pwmSettings);

    NEXUS_Pwm_GetControlWord(pwm, &word);
    rc = NEXUS_Pwm_SetControlWord(pwm, word);
    BDBG_ASSERT(!rc);

    NEXUS_Pwm_CloseChannel(pwm);

    pwm = NEXUS_Pwm_OpenChannel(0, &pwmSettings);
    BDBG_ASSERT(pwm);
    printf("next call should fail\n");
    pwm2 = NEXUS_Pwm_OpenChannel(0, &pwmSettings);
    BDBG_ASSERT(!pwm2);

    NEXUS_Pwm_CloseChannel(pwm);
#endif


    NEXUS_Platform_Uninit();

    printf("success\n");
    return 0;
}

