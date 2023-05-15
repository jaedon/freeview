/***************************************************************************
 *     (c)2007-2008 Broadcom Corporation
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
 * $brcm_Workfile: nexus_astm_test_extensions.h $
 * $brcm_Revision: Hydra_Software_Devel/3 $
 * $brcm_Date: 12/19/08 1:25p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/astm/nexus_astm_test_extensions.h $
 * 
 * Hydra_Software_Devel/3   12/19/08 1:25p bandrews
 * PR50561: updated test to match code
 * 
 * Hydra_Software_Devel/2   12/12/08 6:17p bandrews
 * PR50079: Updated test framework
 * 
 * Hydra_Software_Devel/1   12/8/08 2:47p bandrews
 * PR50079: Create ASTM unit test framework
 **************************************************************************/
#ifndef NEXUS_ASTM_TEST_EXTENSIONS_H__
#define NEXUS_ASTM_TEST_EXTENSIONS_H__

#ifdef __GNUC__
#warning "Compiling nexus_astm.c with test extensions"
#else
#pragma message("Compiling nexus_astm.c with test extensions")
#endif

/* expose statics */
#define static 

/* override dependencies */
#define NEXUS_PidChannel_GetStatus NEXUS_PidChannel_GetStatus_test
NEXUS_Error NEXUS_PidChannel_GetStatus_test(
    NEXUS_PidChannelHandle pidChannel,
    NEXUS_PidChannelStatus *pStatus /* [out] */
    );

#define NEXUS_AudioDecoder_GetAstmSettings_priv NEXUS_AudioDecoder_GetAstmSettings_priv_test
void NEXUS_AudioDecoder_GetAstmSettings_priv_test(NEXUS_AudioDecoderHandle audioDecoder, NEXUS_AudioDecoderAstmSettings * pAstmSettings );

#define NEXUS_AudioDecoder_SetAstmSettings_priv NEXUS_AudioDecoder_SetAstmSettings_priv_test
NEXUS_Error NEXUS_AudioDecoder_SetAstmSettings_priv_test( NEXUS_AudioDecoderHandle audioDecoder, const NEXUS_AudioDecoderAstmSettings * pAstmSettings );

#define NEXUS_AudioDecoder_GetAstmStatus_isr NEXUS_AudioDecoder_GetAstmStatus_isr_test
NEXUS_Error NEXUS_AudioDecoder_GetAstmStatus_isr_test( NEXUS_AudioDecoderHandle audioDecoder, NEXUS_AudioDecoderAstmStatus * pAstmStatus );

#define NEXUS_StcChannel_GetAstmSettings_priv NEXUS_StcChannel_GetAstmSettings_priv_test
void NEXUS_StcChannel_GetAstmSettings_priv_test(
    NEXUS_StcChannelHandle stcChannel,
    NEXUS_StcChannelAstmSettings * pAstmSettings  /* [out] */
);

#define NEXUS_StcChannel_GetSettings NEXUS_StcChannel_GetSettings_test
void NEXUS_StcChannel_GetSettings_test(NEXUS_StcChannelHandle stcChannel, NEXUS_StcChannelSettings *pSettings);

#define NEXUS_StcChannel_GetStc_isr NEXUS_StcChannel_GetStc_isr_test
void NEXUS_StcChannel_GetStc_isr_test(NEXUS_StcChannelHandle stcChannel, uint32_t *pStc);

#define NEXUS_StcChannel_SetAstmSettings_priv NEXUS_StcChannel_SetAstmSettings_priv_test
NEXUS_Error NEXUS_StcChannel_SetAstmSettings_priv_test(
    NEXUS_StcChannelHandle stcChannel,
    const NEXUS_StcChannelAstmSettings * pAstmSettings
);

#define NEXUS_Timebase_GetAstmSettings_priv NEXUS_Timebase_GetAstmSettings_priv_test
void NEXUS_Timebase_GetAstmSettings_priv_test(
    NEXUS_Timebase timebase,
    NEXUS_TimebaseAstmSettings * pAstmSettings  /* [out] */
);

#define NEXUS_Timebase_GetStatus_isr NEXUS_Timebase_GetStatus_isr_test
NEXUS_Error NEXUS_Timebase_GetStatus_isr_test(NEXUS_Timebase timebase, NEXUS_TimebaseStatus *pStatus);

#define NEXUS_Timebase_SetAstmSettings_priv NEXUS_Timebase_SetAstmSettings_priv_test
NEXUS_Error NEXUS_Timebase_SetAstmSettings_priv_test(
    NEXUS_Timebase timebase,
    const NEXUS_TimebaseAstmSettings * pAstmSettings
);

#define NEXUS_VideoDecoder_GetAstmSettings_priv NEXUS_VideoDecoder_GetAstmSettings_priv_test
void NEXUS_VideoDecoder_GetAstmSettings_priv_test(NEXUS_VideoDecoderHandle videoDecoder, NEXUS_VideoDecoderAstmSettings * pAstmSettings);

#define NEXUS_VideoDecoder_GetAstmStatus_isr NEXUS_VideoDecoder_GetAstmStatus_isr_test
NEXUS_Error NEXUS_VideoDecoder_GetAstmStatus_isr_test(NEXUS_VideoDecoderHandle videoDecoder, NEXUS_VideoDecoderAstmStatus *pAstmStatus);

#define NEXUS_VideoDecoder_SetAstmSettings_priv NEXUS_VideoDecoder_SetAstmSettings_priv_test
NEXUS_Error NEXUS_VideoDecoder_SetAstmSettings_priv_test(NEXUS_VideoDecoderHandle videoDecoder, const NEXUS_VideoDecoderAstmSettings *pAstmSettings);

#endif

