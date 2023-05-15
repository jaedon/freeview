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
 * $brcm_Workfile: examples_lib.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 2/19/09 3:30p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/playback/examples_lib.h $
 * 
 * Hydra_Software_Devel/1   2/19/09 3:30p erickson
 * PR51841: move examples_lib out of nexus/examples
 * 
 * 5   7/18/08 11:31a jgarrett
 * PR 44953: Removing strict-prototype warnings
 * 
 * 4   6/19/08 10:46a erickson
 * PR42678: switch to output_type=panel|component
 *
 * 3   5/1/08 10:13a erickson
 * PR42349: add b_get_time for better example status
 *
 * 2   4/24/08 11:04a jrubio
 * PR42054: add DSS support for tuner ast
 *
 *****************************************************************************/
/*
A collection of functions to make the example apps smaller and easier to understand.
These are not NEXUS api's and are subject to change.
*/

#ifndef EXAMPLES_LIB_H__
#define EXAMPLES_LIB_H__

#include "nexus_pid_channel.h"
#include "nexus_playpump.h"
#if NEXUS_HAS_PLAYBACK
#include "nexus_playback.h"
#endif
#include "nexus_video_types.h"
#include "nexus_audio_types.h"
#include "nexus_stc_channel.h"

extern NEXUS_StcChannelHandle g_stcChannel;

typedef struct ExampleDecodeSettings {
    NEXUS_ParserBand parserBand;
    NEXUS_TransportType transportType;
    NEXUS_PlaypumpHandle playpump; /* if set, open playpump pid channels */
#if NEXUS_HAS_PLAYBACK
    NEXUS_PlaybackHandle playback; /* if set, open playback pid channels */
#endif
    uint16_t videoPid;
    NEXUS_VideoCodec videoCodec;
    uint16_t audioPid;
    NEXUS_AudioCodec audioCodec;
    uint16_t pcrPid;
} ExampleDecodeSettings;

/* Single decode of audio/video. */
void ExampleDecode_GetDefaultSettings(ExampleDecodeSettings *pSettings);

/* Init the example decode engine */
NEXUS_Error InitExampleDecode(void);

/* Start a decode */
NEXUS_Error StartExampleDecode(ExampleDecodeSettings *pSettings);

/* Stop a decode */
void StopExampleDecode(void);

/* Uninit the example decode engine */
void UninitExampleDecode(void);

/* Dump status */
void PrintExampleDecodeStatus(void);

/* Get time in milliseconds */
unsigned b_get_time(void);

/* Returns true if system configured for DTV panel output */
bool is_panel_output(void);

#endif
