/***************************************************************************
*     (c)2004-2012 Broadcom Corporation
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
* $brcm_Workfile: nexus_stc_channel_priv.h $
* $brcm_Revision: 22 $
* $brcm_Date: 6/27/12 6:25p $
*
* API Description:
*  Internal timebase APIs.  Primarily used by audio and video decoders.
*
* Revision History:
*
* $brcm_Log: /nexus/modules/transport/7400/include/priv/nexus_stc_channel_priv.h $
* 
* 22   6/27/12 6:25p bandrews
* SW7346-826: merge to main
* 
* SW7346-826/3   6/20/12 9:09p bandrews
* SW7346-826: merge from main
* 
* 21   6/19/12 12:43p vsilyaev
* SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
* 
* SW7346-826/2   6/20/12 8:39p bandrews
* SW7346-826: clean up FIFO watchdog code
* 
* SW7346-826/1   6/14/12 8:50p bandrews
* SW7346-826: add video non-consumption condition detection and flush
*  resolution
* 
* 20   6/1/12 4:18p vsilyaev
* SW7420-2312: Added reference counting of the StcChannel
* 
* 19   5/14/12 7:41p bandrews
* SW7425-2264: merge to main
* 
* SW7425-2264/2   5/7/12 7:31p bandrews
* SW7425-2264: fix build errors
* 
* SW7425-2264/1   5/4/12 9:50p bandrews
* SW7425-2264: first stab at low latency TSM impl
* 
* 18   3/2/12 4:56p bandrews
* SW7425-2276: merge to main
* 
* SW7425-2276/1   2/23/12 9:08p bandrews
* SW7425-2276: initial impl of gap detection
* 
* 17   12/13/11 9:44p bandrews
* SW7425-1772: merge to main
* 
* SW7425-1772/2   12/12/11 10:01p bandrews
* SW7425-1772: add priority queue for decoder stc management
* 
* SW7425-1772/1   11/17/11 6:31p bandrews
* SW7425-1772: 6 channel support hack
* 
* 16   10/31/11 7:46p bandrews
* SW7231-391: merge to main
* 
* SW7420-2078/1   10/25/11 5:22p bandrews
* SW7231-391: update parser band and timebase implementations to use
*  handles everywhere, even for legacy enum usage
* 
* 15   8/12/11 4:16p jtna
* SW7425-1070: merge video primer ptsStcDiffCorretion feature
* 
* 14   6/10/11 3:54p vsilyaev
* SW7425-654: Added triggerNumber to the
*  NEXUS_StcChannelNonRealtimeSettings
* 
* 13   6/8/11 2:20p vsilyaev
* SW7425-654: Updated to match new PI
* 
* 12   6/7/11 3:46p vsilyaev
* SW7425-654: Added API to support non-realtime transcode
* 
* 11   10/26/09 12:26p erickson
* SW7405-3287: add support for sw-based pcr offset for mosaic playback
*  TSM support
*
* 10   7/30/09 4:47p erickson
* PR54880: add NEXUS_StcChannel_GetSerialStc_priv
*
* 9   5/18/09 4:24p erickson
* PR54880: allow StcChannels with different PCR_OFFSET contexts and STC's
*
* 8   11/19/08 9:47a erickson
* PR49212: merge ASTM overrides
*
* PR49212/1   11/18/08 7:45p bandrews
* PR49212: ASTM overrides user settings, but it should not destroy them
*  when it does so
*
* 7   11/18/08 12:57p erickson
* PR48155: extend tsm deadlock algo to check more specific case
*
* 6   11/18/08 11:54a erickson
* PR48155: add decoder fifo watchdog logic to recover from TSM deadlocks
*
* 5   7/23/08 3:01p jgarrett
* PR 39452: Merging to main branch
*
* 4   6/26/08 1:57p erickson
* PR43541: add NEXUS_StcChannel_GetIndex_priv to get the index of the
*  PCR_OFFSET block
*
* Nexus_Audio_Descriptors/1   5/15/08 4:40p jgarrett
* PR 42360: Adding third audio decode
*
* 3   3/25/08 5:25p bandrews
* PR40090: Added compile flag for ASTM
*
* 2   3/24/08 3:16p bandrews
* PR40090: API changes
*
* 1   1/18/08 2:16p jgarrett
* PR 38808: Merging to main branch
*
* Nexus_Devel/14   11/30/07 1:18p erickson
* PR34925: rename to nexus_stc_channel.h
*
* Nexus_Devel/13   11/16/07 9:34a erickson
* PR34925: added #include
*
* Nexus_Devel/12   11/9/07 3:18p erickson
* PR36814: added guard bands
*
* Nexus_Devel/11   9/28/07 4:24p jgarrett
* PR 35002: Adding EnableOffset equivalent
*
* Nexus_Devel/10   9/24/07 11:16a erickson
* PR34925: update
*
* Nexus_Devel/9   9/24/07 10:09a erickson
* PR34925: update
*
* Nexus_Devel/8   9/24/07 9:28a erickson
* PR34926: added required _priv suffix
*
* Nexus_Devel/7   9/21/07 5:03p erickson
* PR34926: added calls to StcChannel from VideoDecoder
*
* Nexus_Devel/6   9/17/07 4:12p erickson
* PR34925: added timebase and stcchannel
*
* Nexus_Devel/5   9/14/07 4:15p erickson
* PR34925: update
*
* Nexus_Devel/4   8/24/07 12:02p jgarrett
* PR 34254: Adding comment blocks
*
***************************************************************************/
#ifndef NEXUS_STCCHANNEL_PRIV_H__
#define NEXUS_STCCHANNEL_PRIV_H__

#include "nexus_types.h"
#include "nexus_stc_channel.h"
#include "nexus_timebase_priv.h"
#include "bavc.h"
#include "bavc_xpt.h"

#ifdef __cplusplus
extern "C" {
#endif

#if NEXUS_HAS_ASTM
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

typedef struct NEXUS_StcChannelAstmStatus
{
    NEXUS_TimebaseHandle timebase;
} NEXUS_StcChannelAstmStatus;
#endif /* NEXUS_HAS_ASTM */

typedef enum NEXUS_StcChannelDecoderType
{
    NEXUS_StcChannelDecoderType_eVideo0,    /* Video Decoder */
    NEXUS_StcChannelDecoderType_eAudio0,    /* Primary Audio (i.e. Decode) */
    NEXUS_StcChannelDecoderType_eAudio1,    /* Secondary Audio (i.e. Passthrough) */
    NEXUS_StcChannelDecoderType_eAudio2,    /* Tertiary Audio (i.e. Audio Descriptor Decode) */
    NEXUS_StcChannelDecoderType_eAudio3,    /* Quaternary Audio (i.e. multichannel decode or passthrough, limited priority/privelege) */
    NEXUS_StcChannelDecoderType_eAudio4,    /* Quinary Audio (i.e. multichannel decode or passthrough, limited priority/privelege) */
    NEXUS_StcChannelDecoderType_eAudio5,    /* Senary Audio (i.e. multichannel decode or passthrough, limited priority/privelege) */
    NEXUS_StcChannelDecoderType_eMax
} NEXUS_StcChannelDecoderType;

typedef BERR_Code (*NEXUS_StcChannel_GetPtsCallback)(void *pContext, BAVC_PTSInfo *pPTSInfo);
typedef BERR_Code (*NEXUS_StcChannel_GetCdbLevelCallback)(void *pContext, unsigned *pCdbLevel);
typedef BERR_Code (*NEXUS_StcChannel_StcValidCallback)(void *pContext);
typedef BERR_Code (*NEXUS_StcChannel_SetPcrOffset)(void *pContext, uint32_t pcrOffset);
typedef BERR_Code (*NEXUS_StcChannel_GetPcrOffset)(void *pContext, uint32_t *pPcrOffset);

typedef struct NEXUS_StcChannelCallbacks
{
    NEXUS_StcChannel_GetPtsCallback       getPts_isr;         /* Required */
    NEXUS_StcChannel_GetCdbLevelCallback  getCdbLevel_isr;    /* Required */
    NEXUS_StcChannel_StcValidCallback     stcValid_isr;       /* The STC has been marked valid. Required if decoder invalidates the STC during playback mode. */
    NEXUS_StcChannel_SetPcrOffset         setPcrOffset_isr;   /* Optional. If set, then don't set the Serial STC but call this function with the needed PCR_OFFSET. */
    NEXUS_StcChannel_GetPcrOffset         getPcrOffset_isr;   /* Optional. If set, then call this function to get the offset to add w/ the Serial STC. */
    void                                 *pDevContext;        /* Context for all callbacks. */
} NEXUS_StcChannelCallbacks;

void NEXUS_StcChannel_GetDefaultCallbacks_priv(
    NEXUS_StcChannelCallbacks *callbacks /* [out] */
    );

/*
When a decoder (audio or video) receives a StcChannel, it must call NEXUS_StcChannel_SetCallbacks_priv and provide
the isr callbacks. These are needed by StcChannel to perform basic lipsync. This is usually called when starting decode.
Call NEXUS_StcChannel_ClearCallbacks_priv when stopping decode.
*/
BERR_Code NEXUS_StcChannel_SetCallbacks_priv(
    NEXUS_StcChannelHandle handle,
    NEXUS_StcChannelDecoderType type,               /* Type of decoder */
    unsigned priority,
    const NEXUS_StcChannelCallbacks *pCallbacks     /* StcChannel ISR callbacks */
    );

/*
Disconnect callbacks set up by NEXUS_StcChannel_SetCallbacks_priv, usually called when stopping decode.
*/
void NEXUS_StcChannel_ClearCallbacks_priv(
    NEXUS_StcChannelHandle handle,
    NEXUS_StcChannelDecoderType type                /* Type of decoder */
    );

/*
Enable Output to a specified PID Channel
*/
BERR_Code NEXUS_StcChannel_EnablePidChannel_priv(
    NEXUS_StcChannelHandle handle,
    NEXUS_PidChannelHandle pidChannel
    );

/*
Disable Output to a specified PID Channel
*/
BERR_Code NEXUS_StcChannel_DisablePidChannel_priv(
    NEXUS_StcChannelHandle handle,
    NEXUS_PidChannelHandle pidChannel
    );

/*
Get the index of the STC block (not PCR_OFFSET context) used by this StcChannel
*/
void NEXUS_StcChannel_GetIndex_priv(
    NEXUS_StcChannelHandle handle,
    unsigned *pIndex /* [out] */
    );

/*
When a decoder receives a "RequestStc" interrupt (also called "FirstPts" interrupt), it must
call this StcChannel function.
*/
BERR_Code NEXUS_StcChannel_RequestStc_isr(
    NEXUS_StcChannelHandle handle,      /* StcChannel Handle */
    NEXUS_StcChannelDecoderType type,   /* Decoder Type */
    const struct BAVC_PTSInfo *pPts
    );

/*
When a decoder receives a "PtsError" interrupt , it must
call this StcChannel function.
*/
BERR_Code NEXUS_StcChannel_PtsError_isr(
    NEXUS_StcChannelHandle handle,      /* StcChannel handle */
    NEXUS_StcChannelDecoderType type,   /* Decoder Type */
    const struct BAVC_PTSInfo *pPts     /* last PTS from the stream, in 45KHz units */
    );

/**
Get the current STC for this StcChannel
**/
void NEXUS_StcChannel_GetStc_isr(
    NEXUS_StcChannelHandle stcChannel,
    uint32_t *pStc /* [out] */
    );

NEXUS_Error NEXUS_StcChannel_SetStc_isr(
    NEXUS_StcChannelHandle stcChannel,
    uint32_t stc
    );

#if NEXUS_HAS_ASTM
void NEXUS_StcChannel_GetAstmSettings_priv(
    NEXUS_StcChannelHandle stcChannel,
    NEXUS_StcChannelAstmSettings * pAstmSettings  /* [out] */
    );

NEXUS_Error NEXUS_StcChannel_SetAstmSettings_priv(
    NEXUS_StcChannelHandle stcChannel,
    const NEXUS_StcChannelAstmSettings * pAstmSettings
    );
    
NEXUS_Error NEXUS_StcChannel_GetAstmStatus_priv(
    NEXUS_StcChannelHandle stcChannel,
    NEXUS_StcChannelAstmStatus * pAstmStatus /* [out] */
    );
#endif /* NEXUS_HAS_ASTM */

/*
Watchdog processing.
Each decoder must report whether it believes it is hung.
This is needed to avoid a TSM deadlock.
One flavor of TSM deadlock is when one decoder's fifo fills while waiting for a PTS to mature, causing band hold. Then, other decoder's fifo drains because of the band hold. Decode is stuck.
*/
void NEXUS_StcChannel_ReportDecoderHang_priv(
    NEXUS_StcChannelHandle handle,
    NEXUS_StcChannelDecoderType type,   /* Decoder Type */
    bool hung,          /* set to true if the decoder believes it is hung */
    unsigned percentFull,
    bool *pShouldFlush,  /* [out] if true, decoder should flush then call NEXUS_StcChannel_ReportFlush_priv */
    bool *pShouldZeroFill /* [out] if true, decoder should zero fill on next underflow */
    );

void NEXUS_StcChannel_ReportFlush_priv(
    NEXUS_StcChannelHandle handle,
    NEXUS_StcChannelDecoderType type   /* Decoder Type */
    );

/**
Summary:
Get the current Serial STC value (PCR_OFFSET STC without the offset).
**/
void NEXUS_StcChannel_GetSerialStc_priv(
    NEXUS_StcChannelHandle handle,
    uint32_t *pStc                                /* [out] Current Stc */
    );

/**
Summary:
Set pcr offset context into acquire mode for pcr_offset entry generation
**/
void NEXUS_StcChannel_SetPcrOffsetContextAcquireMode_priv(
    NEXUS_StcChannelHandle stcChannel
    );

/*
Summary:
Type of event that coult increment the STC 
*/
typedef enum NEXUS_StcChannelTriggerMode {
    NEXUS_StcChannelTriggerMode_eTimebase,            /* Increment STC by timebase */
    NEXUS_StcChannelTriggerMode_eExternalTrig,        /* Increment by external trigger source. */
    NEXUS_StcChannelTriggerMode_eSoftIncrement,       /* Increment when STC_INC_TRIG register is written */
    NEXUS_StcChannelTriggerMode_eMax
} NEXUS_StcChannelTriggerMode;


/*
Summary: 
Configuration of non-realtime mode of STC channel
*/
typedef struct NEXUS_StcChannelNonRealtimeSettings {
    NEXUS_StcChannelTriggerMode triggerMode; /* Event that will increment the STC */
    uint64_t stcIncrement; /* used when triggerMode == NEXUS_StcChannel_TriggerMode_eExternalTrig */
    unsigned externalTrigger;/* Identifies which external trigger is used when TriggerMode == _eExternalTrig */
} NEXUS_StcChannelNonRealtimeSettings;

void NEXUS_StcChannel_GetDefaultNonRealtimeSettings_priv (
    NEXUS_StcChannelNonRealtimeSettings *pSettings
        );

NEXUS_Error NEXUS_StcChannel_SetNonRealtimeConfig_priv(
    NEXUS_StcChannelHandle handle,
    NEXUS_StcChannelNonRealtimeSettings *pSettings
    );

/**
Summary: 
Get address of the soft increment register. 
**/
void NEXUS_StcChannel_GetSoftIncrementRegOffset_priv(
    NEXUS_StcChannelHandle handle,
    BAVC_Xpt_StcSoftIncRegisters *regMap
    );


typedef struct NEXUS_StcChannelLowLatencySettings
{
    bool enabled;
} NEXUS_StcChannelLowLatencySettings;

void NEXUS_StcChannel_GetDefaultLowLatencySettings_priv(
    NEXUS_StcChannelLowLatencySettings * pSettings
    );

NEXUS_Error NEXUS_StcChannel_SetLowLatencySettings_priv(
    NEXUS_StcChannelHandle stcChannel,
    NEXUS_StcChannelDecoderType decoder,
    NEXUS_StcChannelLowLatencySettings * pSettings
    );


void NEXUS_StcChannel_GetRate_priv(NEXUS_StcChannelHandle stcChannel, unsigned * increment, unsigned * prescale);
bool NEXUS_StcChannel_IsFrozen_priv(NEXUS_StcChannelHandle stcChannel);

NEXUS_OBJECT_CLASS_DECLARE(NEXUS_StcChannel);

#ifdef __cplusplus
}
#endif

#endif
