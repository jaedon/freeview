/***************************************************************************
 *     (c)2007-2012 Broadcom Corporation
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
 * $brcm_Workfile: nexus_video_decoder_primer.h $
 * $brcm_Revision: 5 $
 * $brcm_Date: 6/8/12 2:15p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/video_decoder/7400/include/nexus_video_decoder_primer.h $
 * 
 * 5   6/8/12 2:15p erickson
 * SW7425-3170: promote NEXUS_VideoDecoderPrimerHandle to a tracked
 *  handle, refactor API with backward compatible macros
 * 
 * 4   8/12/11 4:21p jtna
 * SW7425-1070: merge video primer ptsStcDiffCorrection feature
 * 
 * 3   8/23/10 2:44p erickson
 * SW7405-4723: add NEXUS_VideoDecoder_SetPrimerSettings
 * 
 * 2   10/12/09 9:20a erickson
 * SWDEPRECATED-4001: add usage comments to NEXUS_VideoDecoder_StartPrimer
 *
 * 1   7/15/09 1:59p erickson
 * PR56522: add NEXUS_VideoDecoderPrimer api
 *
 **************************************************************************/
#ifndef NEXUS_VIDEO_DECODER_PRIMER_H__
#define NEXUS_VIDEO_DECODER_PRIMER_H__

#include "nexus_video_decoder.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
Summary:
Handle for video decoder primer returned by NEXUS_VideoDecoderPrimer_Open
**/
typedef struct NEXUS_VideoDecoderPrimer *NEXUS_VideoDecoderPrimerHandle;

/**
Summary:
Start processing a pid channel for fast channel change using NEXUS_VideoDecoderPrimer_StopPrimerAndStartDecode
**/
NEXUS_VideoDecoderPrimerHandle NEXUS_VideoDecoderPrimer_Open( /* attr{destructor=NEXUS_VideoDecoderPrimer_Close} */
    NEXUS_VideoDecoderHandle videoDecoder
    );

/**
Summary:
**/
void NEXUS_VideoDecoderPrimer_Close(
    NEXUS_VideoDecoderPrimerHandle primer
    );

/**
Summary:
Tell primer to start processing data.
**/
NEXUS_Error NEXUS_VideoDecoderPrimer_Start(
    NEXUS_VideoDecoderPrimerHandle primer,
    const NEXUS_VideoDecoderStartSettings *pStartSettings
    );

/**
Summary:
Tell primer to stop processing data.

Description:
Primers are automatically stopped if you call NEXUS_VideoDecoderPrimer_StopPrimerAndStartDecode.
**/
void NEXUS_VideoDecoderPrimer_Stop(
    NEXUS_VideoDecoderPrimerHandle primer
    );

/**
Summary:
Start decode using a primed pid channel.

Description:
This is mutually exclusive with NEXUS_VideoDecoder_Start.
Call NEXUS_VideoDecoder_Stop, just like a non-primed channel.
After stopping decode, you must call NEXUS_VideoDecoderPrimer_Start to reactivate the primer.
**/
NEXUS_Error NEXUS_VideoDecoderPrimer_StopPrimerAndStartDecode(
    NEXUS_VideoDecoderPrimerHandle primer,
    NEXUS_VideoDecoderHandle videoDecoder
    );

/**
Summary:
Video decoder primer settings.

Description:
These settings can be changed before or after NEXUS_VideoDecoderPrimer_Start.

When decode is started with NEXUS_VideoDecoderPrimer_StopPrimerAndStartDecode, Nexus will calculate the difference
between the current STC and the PTS of all random access points (RAP) currently in the buffer. Nexus will then 
select a single RAP from which to start decode. This decision affects the momentary behavior immediately after 
decode is started, while a STC/PTS discrepancy exists. 

pastTolerance and futureTolerance control the above decision in the following manner:

- If there are no RAPs that satisfy either tolerance, then the larger of the two tolerances is taken as a preference.
If pastTolerance > futureTolerance, then Nexus will select the nearest past RAP (STC > PTS).
If pastTolerance < futureTolerance, then Nexus will select the nearest future RAP (STC < PTS)
If pastTolerance == futureTolerance, then Nexus will select the nearest RAP from the current STC.

- If there are one or more RAPs that satisfy one or both tolerances, then Nexus will select the nearest RAP from 
the current STC that satisfies a tolerance.
**/
typedef struct NEXUS_VideoDecoderPrimerSettings
{
    unsigned pastTolerance;   /* time (in milliseconds) between the current STC and the past, within which 
                                 Nexus will look for a RAP to start decode from */
    unsigned futureTolerance; /* time (in milliseconds) between the current STC and the future, within which 
                                 Nexus will look for a RAP to start decode from */

    bool ptsStcDiffCorrectionEnabled; /* If true, then correct the PTS-STC difference when starting decode, 
                                         by applying an additional PTS offset to the video decoder.
                                         If not using sync_channel, this may worsen A/V sync.
                                         If using sync_channel, this may worsen end-to-end system delay. */
} NEXUS_VideoDecoderPrimerSettings;


/**
Summary:
Get the current NEXUS_VideoDecoderPrimerSettings from the primer.
**/
void NEXUS_VideoDecoderPrimer_GetSettings(
    NEXUS_VideoDecoderPrimerHandle primer,
    NEXUS_VideoDecoderPrimerSettings *pSettings /* [out] */
    );

/**
Summary:
Set the current NEXUS_VideoDecoderPrimerSettings from the primer.
**/
NEXUS_Error NEXUS_VideoDecoderPrimer_SetSettings(
    NEXUS_VideoDecoderPrimerHandle primer,
    const NEXUS_VideoDecoderPrimerSettings *pSettings
    );

/* deprecated */
#define NEXUS_VideoDecoder_OpenPrimer(VIDEODECODER) NEXUS_VideoDecoderPrimer_Open(VIDEODECODER)
#define NEXUS_VideoDecoder_ClosePrimer(VIDEODECODER,PRIMER) NEXUS_VideoDecoderPrimer_Close(PRIMER)
#define NEXUS_VideoDecoder_StartPrimer(VIDEODECODER,PRIMER,PSETTINGS) NEXUS_VideoDecoderPrimer_Start(PRIMER,PSETTINGS)
#define NEXUS_VideoDecoder_StopPrimer(VIDEODECODER,PRIMER) NEXUS_VideoDecoderPrimer_Stop(PRIMER)
#define NEXUS_VideoDecoder_GetPrimerSettings(PRIMER,PSETTINGS) NEXUS_VideoDecoderPrimer_GetSettings(PRIMER,PSETTINGS)
#define NEXUS_VideoDecoder_SetPrimerSettings(PRIMER,PSETTINGS) NEXUS_VideoDecoderPrimer_SetSettings(PRIMER,PSETTINGS)
#define NEXUS_VideoDecoder_StartDecodeWithPrimer(VIDEODECODER,PRIMER) NEXUS_VideoDecoderPrimer_StopPrimerAndStartDecode(PRIMER,VIDEODECODER)

#ifdef __cplusplus
}
#endif

#endif
