/***************************************************************************
 *     (c)2012 Broadcom Corporation
 *
 *  This program is the proprietary software of Broadcom Corporation and/or its licensors,
 *  and may only be used, duplicated, modified or distributed pursuant to the terms and
 *  conditions of a separate, written license agreement executed between you and Broadcom
 *  (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
 *  no license (express or implied), right to use, or waiver of any kind with respect to the
 *  Software, and Broadcom expressly reserves all rights in and to the Software and all
 *  intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
 *  HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
 *  NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
 *
 *  Except as expressly set forth in the Authorized License,
 *
 *  1.     This program, including its structure, sequence and organization, constitutes the valuable trade
 *  secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
 *  and to use this information only in connection with your use of Broadcom integrated circuit products.
 *
 *  2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
 *  AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
 *  WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
 *  THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
 *  OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
 *  LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
 *  OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
 *  USE OR PERFORMANCE OF THE SOFTWARE.
 *
 *  3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
 *  LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
 *  EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
 *  USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
 *  THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
 *  ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
 *  LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
 *  ANY LIMITED REMEDY.
 *
 * $brcm_Workfile: astm_test.c $
 * $brcm_Revision: Hydra_Software_Devel/6 $
 * $brcm_Date: 9/6/12 6:08p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/astm/astm_test.c $
 * 
 * Hydra_Software_Devel/6   9/6/12 6:08p erickson
 * SW7435-349: add standard header
 * 
 **************************************************************************/

#if NEXUS_HAS_ASTM
#include "nexus_platform.h"
#include "nexus_pid_channel.h"
#include "nexus_video_decoder.h"
#include "nexus_stc_channel.h"
#include "nexus_audio_decoder.h"
#include "nexus_astm.h"
#include "nexus_astm_test.h"
#include "bstd.h"
#include "bkni.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#if NEXUS_NUM_DISPLAYS > 1
#define HDSD_SIMUL 1
#endif

int main(void)
{
    NEXUS_PlatformSettings platformSettings;
    NEXUS_AstmSettings astmSettings;
    NEXUS_AstmHandle astm;
    unsigned int i = 0;
    
	printf("NEXUS ASTM test application\n");
	BKNI_Memset(&gTestData, 0, sizeof(struct TestData));

	printf("Platform init\n");
    /* Bring up all modules for a platform in a default configuration for this platform */
    NEXUS_Platform_GetDefaultSettings(&platformSettings); 
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings); 
    
	printf("ASTM create\n");
    /* create an astm instance and connect */
    NEXUS_Astm_GetDefaultSettings(&astmSettings);
    astmSettings.stcChannel = (NEXUS_StcChannelHandle)1;
    astmSettings.videoDecoder = (NEXUS_VideoDecoderHandle)1;
    astmSettings.audioDecoder[0] = (NEXUS_AudioDecoderHandle)2;
    astmSettings.audioDecoder[1] = (NEXUS_AudioDecoderHandle)3;
    astmSettings.stcMaster = (NEXUS_AudioDecoderHandle)2; /* audio master is preferred */
    astm = NEXUS_Astm_Create(&astmSettings);

	/* begin test */

	printf("hit enter to begin test\n");
	getchar();

	printf("ASTM start\n");
	gTestData.audio[0].status.started = true;
	BKNI_EnterCriticalSection();
	gTestData.audio[0].settings.lifecycle_isr(gTestData.audio[0].settings.callbackContext, 0);
	BKNI_LeaveCriticalSection();

	printf("hit enter to send audio pts errors\n");
	getchar();

	printf("Generating audio pts errors\n");
	for (i = 0; i < 50; i++)
	{
		BKNI_EnterCriticalSection();
		gTestData.audio[0].settings.tsmFail_isr(gTestData.audio[0].settings.callbackContext, 0);
		BKNI_LeaveCriticalSection();
	}

	printf("hit enter to generate more pts errors\n");
	getchar();

	printf("Generating audio pts errors\n");
	for (i = 0; i < 50; i++)
	{
		BKNI_EnterCriticalSection();
		gTestData.audio[0].settings.tsmFail_isr(gTestData.audio[0].settings.callbackContext, 0);
		BKNI_LeaveCriticalSection();
	}

	printf("hit enter to stop\n");
	getchar();

	printf("Stopping astm\n");
	gTestData.audio[0].status.started = false;
	BKNI_EnterCriticalSection();
	gTestData.audio[0].settings.lifecycle_isr(gTestData.audio[0].settings.callbackContext, 0);
	BKNI_LeaveCriticalSection();

	printf("hit enter to destroy\n");
	getchar();

	NEXUS_Astm_Destroy(astm);

    return 0;
}

#include "astm_test_env.c"
#endif /* NEXUS_HAS_ASTM */
