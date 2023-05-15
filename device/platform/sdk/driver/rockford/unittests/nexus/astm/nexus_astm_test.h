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
 * $brcm_Workfile: nexus_astm_test.h $
 * $brcm_Revision: Hydra_Software_Devel/3 $
 * $brcm_Date: 12/19/08 1:25p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/astm/nexus_astm_test.h $
 * 
 * Hydra_Software_Devel/3   12/19/08 1:25p bandrews
 * PR50561: updated test to match code
 * 
 * Hydra_Software_Devel/2   12/12/08 6:28p bandrews
 * PR50079: update test env
 * 
 * Hydra_Software_Devel/1   12/8/08 2:47p bandrews
 * PR50079: Create ASTM unit test framework
 **************************************************************************/
#ifndef NEXUS_ASTM_TEST_H__
#define NEXUS_ASTM_TEST_H__

#include "nexus_astm.h"
#include "nexus_video_decoder.h"
#include "nexus_audio_decoder.h"
#include "nexus_stc_channel.h"
#include "nexus_timebase.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef enum NEXUS_StcChannelTsmMode
{
    NEXUS_StcChannelTsmMode_eStcMaster = 0, /* STC is seeded with either the video or audio PTS, depending on which decoder makes an STC request first */
    NEXUS_StcChannelTsmMode_eVideoMaster, /* STC is seeded with the video PTS */
    NEXUS_StcChannelTsmMode_eAudioMaster, /* STC is seeded with the audio PTS */
    NEXUS_StcChannelTsmMode_eOutputMaster, /* No tsm is performed.  Output clock pulls data through decoder.  Also called VSYNC mode for video. */
    NEXUS_StcChannelTsmMode_eMax
} NEXUS_StcChannelTsmMode;

typedef struct NEXUS_StcChannelAstmSettings
{
    bool enabled;
    NEXUS_StcChannelMode mode;
    NEXUS_StcChannelTsmMode tsmMode;
    unsigned int syncLimit;
} NEXUS_StcChannelAstmSettings;

typedef enum NEXUS_TimebaseClockCoupling
{
    NEXUS_TimebaseClockCoupling_eInputClock, /* PCR, VSYNC, HSYNC, etc. */
    NEXUS_TimebaseClockCoupling_eInternalClock, /* internal XTAL */
    NEXUS_TimebaseClockCoupling_eMax /* enum terminator */
} NEXUS_TimebaseClockCoupling;

typedef struct NEXUS_TimebaseAstmSettings
{
    bool enabled; /* is astm enabled */
    NEXUS_TimebaseClockCoupling clockCoupling; /* lock to input or xtal */
    NEXUS_Callback pcrReceived_isr; /* Notify Astm when the a PCR arrives. */
    void * callbackContext; /* user context passed in isr callbacks */
} NEXUS_TimebaseAstmSettings;

typedef struct NEXUS_VideoDecoderAstmSettings
{
	bool enableAstm; /* Astm will enable/disable astm mode */
	unsigned int syncLimit; /* limit outside of which decoder will not apply astm mode */
	bool enableTsm; /* Astm will enable/disable tsm. */
    bool enablePlayback; /* Astm will enable/disable playback. */
    NEXUS_Callback tsmPass_isr; /* Notify Astm when TSM passes. */
    NEXUS_Callback tsmFail_isr; /* Notify Astm when TSM fails. */
    NEXUS_Callback tsmLog_isr; /* Notify Astm when TSM log buffer fills */
    NEXUS_Callback lifecycle_isr; /* Notify Astm when we have started/stopped */
    NEXUS_Callback watchdog_isr; /* Notify Astm when a watchdog occurs */
    void *callbackContext; /* user context passed callback_isr */
} NEXUS_VideoDecoderAstmSettings;

typedef struct NEXUS_VideoDecoderAstmStatus
{
	bool started;
	uint32_t pts;
	struct
	{
		unsigned int address;
		unsigned int size;
	} tsmLog;
} NEXUS_VideoDecoderAstmStatus;

typedef struct NEXUS_AudioDecoderAstmSettings
{
	bool enableAstm; /* Astm will enable/disable astm mode */
	unsigned int syncLimit; /* limit outside of which decoder will not apply astm mode */
	bool enableTsm; /* Astm will enable/disable tsm. */
    bool enablePlayback; /* Astm will enable/disable playback. */
    NEXUS_Callback tsmPass_isr; /* Notify Astm when TSM passes. */
    NEXUS_Callback tsmFail_isr; /* Notify Astm when TSM fails. */
    NEXUS_Callback tsmLog_isr; /* Notify Astm when TSM log buffer fills */
    NEXUS_Callback lifecycle_isr; /* Notify Astm when decoder is started/stopped */
    NEXUS_Callback watchdog_isr; /* Notify Astm when a watchdog occurs */
    void * callbackContext; /* user context passed in isr callbacks */
} NEXUS_AudioDecoderAstmSettings;

typedef struct NEXUS_AudioDecoderAstmStatus 
{
	bool started;
	uint32_t pts;
	struct
	{
		unsigned int address;
		unsigned int size;
	} tsmLog;
} NEXUS_AudioDecoderAstmStatus;

struct NEXUS_AstmDecoderStatus
{
	bool started;
    uint32_t pts;
    struct
    {
        unsigned int address;
        unsigned int size;
    } tsmLog;
};

typedef void (*NEXUS_Astm_DecoderStatusAccessor_isr)(void * decoder, struct NEXUS_AstmDecoderStatus * status);

typedef void * BASTMlib_Presenter_Handle;

/* Used to collect information to process callbacks from around the system */
struct NEXUS_AstmContext
{
    NEXUS_AstmHandle astm;
    NEXUS_StcChannelHandle stc;
    bool started;
    void * decoder;
#ifdef BDBG_DEBUG_BUILD
    const char * decoderName;
#endif
    NEXUS_Astm_DecoderStatusAccessor_isr getStatus_isr;
    BASTMlib_Presenter_Handle presenter;
    BKNI_EventHandle watchdogEvent;
    NEXUS_EventCallbackHandle watchdogEventHandler;
    BKNI_EventHandle lifecycleEvent;
    NEXUS_EventCallbackHandle lifecycleEventHandler;
};

struct TestData
{
	struct
	{
		NEXUS_VideoDecoderAstmSettings settings;
		NEXUS_VideoDecoderAstmStatus status;
	} video;
	struct
	{
		NEXUS_AudioDecoderAstmSettings settings;
		NEXUS_AudioDecoderAstmStatus status;
	}audio[2];
	struct
	{
		NEXUS_StcChannelAstmSettings astmSettings;
		NEXUS_StcChannelSettings settings;
		uint32_t stc;
	} stcChannel;
	struct
	{
		NEXUS_TimebaseAstmSettings settings;
		NEXUS_TimebaseStatus status;
	} timebase;
	struct
	{
		NEXUS_PidChannelStatus status;
	} pidChannel;
};

extern struct TestData gTestData;

NEXUS_Error NEXUS_Astm_P_ApplyStcSource(NEXUS_AstmHandle astm, NEXUS_AstmStcSource stcSource, void * stcMaster);

NEXUS_Error NEXUS_Astm_P_ApplyClockCoupling(NEXUS_AstmHandle astm, NEXUS_AstmClockCoupling clockCoupling);

NEXUS_Error NEXUS_Astm_P_ApplyPresentationRateControl(NEXUS_AstmHandle astm, NEXUS_AstmPresentationRateControl presentationRateControl);

NEXUS_StcChannelTsmMode NEXUS_Astm_P_ComputeStcChannelTsmMode(
    NEXUS_AstmHandle astm,
    NEXUS_AstmStcSource stcSource,
    void * stcMaster,
    NEXUS_AstmPresentationRateControl presentationRateControl
);

NEXUS_Error NEXUS_Astm_P_ApplyStcChannelTsmMode(
    NEXUS_AstmHandle astm,
    NEXUS_StcChannelTsmMode tsmMode
);

BERR_Code NEXUS_Astmlib_P_PresentationStateChange(void * pvParm1, int iParm2);

BERR_Code NEXUS_Astmlib_P_ClockCouplingStateChange(void * pvParm1, int iParm2);

void NEXUS_Astm_P_GetAudioStatus_isr(void * decoder, struct NEXUS_AstmDecoderStatus * status);

void NEXUS_Astm_P_GetVideoStatus_isr(void * decoder, struct NEXUS_AstmDecoderStatus * status);

void NEXUS_Astm_P_TsmCallback_isr(struct NEXUS_AstmContext * context, bool pass);

void NEXUS_Astm_P_DecoderTsmPassCallback_isr(void * context, int param);

void NEXUS_Astm_P_DecoderTsmFailCallback_isr(void * context, int param);

void NEXUS_Astm_P_DecoderTsmLogCallback_isr(void * context, int param);

void NEXUS_Astm_P_TimebasePcrReceivedCallback_isr(void * context, int param);

void NEXUS_Astm_P_DisconnectVideoDecoder(NEXUS_AstmHandle astm);

NEXUS_Error NEXUS_Astm_P_ConnectVideoDecoder(NEXUS_AstmHandle astm, const NEXUS_AstmSettings * pSettings);

NEXUS_Error NEXUS_Astm_P_DisconnectAudioDecoder(NEXUS_AstmHandle astm, unsigned int index);

NEXUS_Error NEXUS_Astm_P_ConnectAudioDecoder(NEXUS_AstmHandle astm, unsigned int index, const NEXUS_AstmSettings * pSettings);

void NEXUS_Astm_P_DisconnectTransport(NEXUS_AstmHandle astm);

NEXUS_Error NEXUS_Astm_P_ConnectTransport(NEXUS_AstmHandle astm, const NEXUS_AstmSettings * pSettings);

#ifdef __cplusplus
}
#endif

#endif

