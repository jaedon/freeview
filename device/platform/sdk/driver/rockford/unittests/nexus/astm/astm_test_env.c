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
 * $brcm_Workfile: astm_test_env.c $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 9/6/12 6:08p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/astm/astm_test_env.c $
 * 
 * Hydra_Software_Devel/2   9/6/12 6:08p erickson
 * SW7435-349: add standard header
 * 
 **************************************************************************/

struct TestData gTestData;

#ifdef NEXUS_PidChannel_GetStatus
#undef NEXUS_PidChannel_GetStatus
#endif
NEXUS_Error NEXUS_PidChannel_GetStatus_test(
    NEXUS_PidChannelHandle pidChannel,
    NEXUS_PidChannelStatus *pStatus /* [out] */
    )
{
	BSTD_UNUSED(pidChannel);
	printf("NEXUS_PidChannel_GetStatus_test\n");
	*pStatus = gTestData.pidChannel.status;
	return 0;
}

#ifdef NEXUS_AudioDecoder_GetAstmSettings_priv
#undef NEXUS_AudioDecoder_GetAstmSettings_priv
#endif
void NEXUS_AudioDecoder_GetAstmSettings_priv_test(NEXUS_AudioDecoderHandle audioDecoder, NEXUS_AudioDecoderAstmSettings * pAstmSettings )
{
	BSTD_UNUSED(audioDecoder);
	printf("NEXUS_AudioDecoder_GetAstmSettings_priv_test\n");
	*pAstmSettings = gTestData.audio[(unsigned)audioDecoder - 2].settings;
}

#ifdef NEXUS_AudioDecoder_SetAstmSettings_priv
#undef NEXUS_AudioDecoder_SetAstmSettings_priv
#endif
NEXUS_Error NEXUS_AudioDecoder_SetAstmSettings_priv_test( NEXUS_AudioDecoderHandle audioDecoder, const NEXUS_AudioDecoderAstmSettings * pAstmSettings )
{
	BSTD_UNUSED(audioDecoder);
	printf("NEXUS_AudioDecoder_SetAstmSettings_priv_test\n");
	printf("enableAstm = %s\n", pAstmSettings->enableAstm ? "true" : "false");
	printf("enableTsm = %s\n", pAstmSettings->enableTsm ? "true" : "false");
	printf("enablePlayback = %s\n", pAstmSettings->enablePlayback ? "true" : "false");
	printf("syncLimit = %u\n", pAstmSettings->syncLimit);
	gTestData.audio[(unsigned)audioDecoder - 2].settings = *pAstmSettings;
	return 0;
}

#ifdef NEXUS_AudioDecoder_GetAstmStatus_isr
#undef NEXUS_AudioDecoder_GetAstmStatus_isr
#endif
NEXUS_Error NEXUS_AudioDecoder_GetAstmStatus_isr_test( NEXUS_AudioDecoderHandle audioDecoder, NEXUS_AudioDecoderAstmStatus * pAstmStatus )
{
	BSTD_UNUSED(audioDecoder);
	printf("NEXUS_AudioDecoder_GetAstmStatus_isr_test\n");
	*pAstmStatus = gTestData.audio[(unsigned)audioDecoder - 2].status;
	return 0;
}

#ifdef NEXUS_StcChannel_GetAstmSettings_priv
#undef NEXUS_StcChannel_GetAstmSettings_priv
#endif
void NEXUS_StcChannel_GetAstmSettings_priv_test(
    NEXUS_StcChannelHandle stcChannel,
    NEXUS_StcChannelAstmSettings * pAstmSettings  /* [out] */
)
{
	BSTD_UNUSED(stcChannel);
	printf("NEXUS_StcChannel_GetAstmSettings_priv_test\n");
	*pAstmSettings = gTestData.stcChannel.astmSettings;
}

#ifdef NEXUS_StcChannel_SetAstmSettings_priv
#undef NEXUS_StcChannel_SetAstmSettings_priv
#endif
NEXUS_Error NEXUS_StcChannel_SetAstmSettings_priv_test(
    NEXUS_StcChannelHandle stcChannel,
    const NEXUS_StcChannelAstmSettings * pAstmSettings
)
{
	BSTD_UNUSED(stcChannel);
	printf("NEXUS_StcChannel_SetAstmSettings_priv_test\n");
	printf("enabled = %s\n", pAstmSettings->enabled ? "true" : "false");
	printf("mode = %d\n", pAstmSettings->mode);
	printf("tsmMode = %d\n", pAstmSettings->tsmMode);
	printf("syncLimit = %u\n", pAstmSettings->syncLimit);
	gTestData.stcChannel.astmSettings = *pAstmSettings;
	return 0;
}

#ifdef NEXUS_StcChannel_GetSettings
#undef NEXUS_StcChannel_GetSettings
#endif
void NEXUS_StcChannel_GetSettings_test(NEXUS_StcChannelHandle stcChannel, NEXUS_StcChannelSettings *pSettings)
{
	BSTD_UNUSED(stcChannel);
	printf("NEXUS_StcChannel_GetSettings_test\n");
	*pSettings = gTestData.stcChannel.settings;
}

#ifdef NEXUS_StcChannel_GetStc_isr
#undef NEXUS_StcChannel_GetStc_isr
#endif
void NEXUS_StcChannel_GetStc_isr_test(NEXUS_StcChannelHandle stcChannel, uint32_t *pStc)
{
	BSTD_UNUSED(stcChannel);
	printf("NEXUS_StcChannel_GetStc_isr_test\n");
	*pStc = gTestData.stcChannel.stc;
}

#ifdef NEXUS_Timebase_GetAstmSettings_priv
#undef NEXUS_Timebase_GetAstmSettings_priv
#endif
void NEXUS_Timebase_GetAstmSettings_priv_test(
    NEXUS_Timebase timebase,
    NEXUS_TimebaseAstmSettings * pAstmSettings  /* [out] */
)
{
	BSTD_UNUSED(timebase);
	printf("NEXUS_Timebase_GetAstmSettings_priv_test\n");
	*pAstmSettings = gTestData.timebase.settings;
}

#ifdef NEXUS_Timebase_SetAstmSettings_priv
#undef NEXUS_Timebase_SetAstmSettings_priv
#endif
NEXUS_Error NEXUS_Timebase_SetAstmSettings_priv_test(
    NEXUS_Timebase timebase,
    const NEXUS_TimebaseAstmSettings * pAstmSettings
)
{
	BSTD_UNUSED(timebase);
	printf("NEXUS_Timebase_SetAstmSettings_priv_test\n");
	printf("enabled = %s\n", pAstmSettings->enabled ? "true" : "false");
	printf("clockCoupling = %d\n", pAstmSettings->clockCoupling);
	gTestData.timebase.settings = *pAstmSettings;
	return 0;
}

#ifdef NEXUS_Timebase_GetStatus_isr
#undef NEXUS_Timebase_GetStatus_isr
#endif
NEXUS_Error NEXUS_Timebase_GetStatus_isr_test(NEXUS_Timebase timebase, NEXUS_TimebaseStatus *pStatus)
{
	BSTD_UNUSED(timebase);
	printf("NEXUS_Timebase_GetStatus_isr_test\n");
	*pStatus = gTestData.timebase.status;
	return 0;
}

#ifdef NEXUS_VideoDecoder_GetAstmSettings_priv
#undef NEXUS_VideoDecoder_GetAstmSettings_priv
#endif
void NEXUS_VideoDecoder_GetAstmSettings_priv_test(NEXUS_VideoDecoderHandle videoDecoder, NEXUS_VideoDecoderAstmSettings * pAstmSettings)
{
	BSTD_UNUSED(videoDecoder);
	printf("NEXUS_VideoDecoder_GetAstmSettings_priv_test\n");
	*pAstmSettings = gTestData.video.settings;
}

#ifdef NEXUS_VideoDecoder_SetAstmSettings_priv
#undef NEXUS_VideoDecoder_SetAstmSettings_priv
#endif
NEXUS_Error NEXUS_VideoDecoder_SetAstmSettings_priv_test(NEXUS_VideoDecoderHandle videoDecoder, const NEXUS_VideoDecoderAstmSettings *pAstmSettings)
{
	BSTD_UNUSED(videoDecoder);
	printf("NEXUS_VideoDecoder_SetAstmSettings_priv_test\n");
	printf("enableAstm = %s\n", pAstmSettings->enableAstm ? "true" : "false");
	printf("enableTsm = %s\n", pAstmSettings->enableTsm ? "true" : "false");
	printf("enablePlayback = %s\n", pAstmSettings->enablePlayback ? "true" : "false");
	printf("syncLimit = %u\n", pAstmSettings->syncLimit);
	gTestData.video.settings = *pAstmSettings;
	return 0;
}

#ifdef NEXUS_VideoDecoder_GetAstmStatus_isr
#undef NEXUS_VideoDecoder_GetAstmStatus_isr
#endif
NEXUS_Error NEXUS_VideoDecoder_GetAstmStatus_isr_test(NEXUS_VideoDecoderHandle videoDecoder, NEXUS_VideoDecoderAstmStatus *pAstmStatus)
{
	BSTD_UNUSED(videoDecoder);
	printf("NEXUS_VideoDecoder_GetAstmStatus_isr_test\n");
	*pAstmStatus = gTestData.video.status;
	return 0;
}

