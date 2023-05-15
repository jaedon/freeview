/***************************************************************************
 *     (c)2007-2010 Broadcom Corporation
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
 * $brcm_Workfile: nexus_audio_init.h $
 * $brcm_Revision: 4 $
 * $brcm_Date: 7/30/10 4:13p $
 *
 * Module Description:
 *      Audio Initialization Routines
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/soft_audio/7408/include/nexus_audio_init.h $
 * 
 * 4   7/30/10 4:13p jgarrett
 * SW7405-4621: replace nexus_base.h with nexus_types.h
 * 
 * 3   7/20/10 2:16p jgarrett
 * SW7550-496: Adding surface handle
 * 
 * 2   2/4/10 5:37p jgarrett
 * SW7408-57: Refactoring soft audio startup of TP1
 * 
 * 1   12/8/09 11:08a jgarrett
 * SW7408-17: Adding soft audio
 * 
 * 11   8/28/09 4:00p jgarrett
 * SW7405-2928: Adding multichannel capture support
 * 
 * 10   7/14/09 1:46p jgarrett
 * PR 54367: adding capture buffer size option
 * 
 * 9   4/6/09 5:25p jgarrett
 * PR 53393: Adding memory configuration options
 * 
 * 8   4/2/09 2:24p jgarrett
 * PR 52729: Adding HBR option for SPDIF and allowing the platform to
 *  configure whether HBR is enabled
 * 
 * 7   11/12/08 3:48p jgarrett
 * PR 48920: Adding a global independent delay flag that affects all
 *  channels
 * 
 * 6   9/25/08 5:07p jgarrett
 * PR 46055: Refactoring equalizer API
 * 
 * 5   8/8/08 6:59p jgarrett
 * PR 45171: Adding Frontend handle for AASD
 * 
 * 4   7/23/08 3:01p jgarrett
 * PR 40367: Removing runtime codec booleans
 * 
 * 3   2/20/08 1:36p erickson
 * PR39405: added RFM output
 * 
 * 2   2/5/08 1:49p jgarrett
 * PR 39017: Adding HdmiOutput
 * 
 * 1   1/18/08 2:18p jgarrett
 * PR 38808: Merging to main branch
 * 
 * Nexus_Devel/5   1/17/08 12:23p jgarrett
 * PR 38753: Adding HDMI Input handle
 * 
 * Nexus_Devel/4   1/8/08 7:01p jgarrett
 * PR 38535: Adding equalizer settings, touching up post-processing names
 * 
 * Nexus_Devel/3   11/29/07 2:24p erickson
 * PR35457: doc update for 0.5 release
 * 
 * Nexus_Devel/2   11/9/07 9:58a jgarrett
 * PR 36774: Adding encode and post-processing options to init
 * 
 * Nexus_Devel/1   9/27/07 7:52p jgarrett
 * PR 34954: Adding audio module init
 * 
 **************************************************************************/
#ifndef NEXUS_AUDIO_INIT_H__
#define NEXUS_AUDIO_INIT_H__

#include "nexus_types.h"
#include "nexus_audio_output.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct NEXUS_AudioModuleSettings
{
    struct 
    {
        NEXUS_ModuleHandle transport;
        NEXUS_ModuleHandle hdmiInput;   /* Only required for platforms that support HDMI input */
        NEXUS_ModuleHandle hdmiOutput;  /* Only required for platforms that support HDMI output */
        NEXUS_ModuleHandle rfm;         /* Only required for platforms that support RFM */
        NEXUS_ModuleHandle frontend;    /* Only required for platforms that support RfAudioDecoder */
        NEXUS_ModuleHandle surface;     /* Only required for platforms that support graphics on the audio DSP */
    } modules;

    bool watchdogEnabled;
    bool independentDelay;              /* If true, independent delay is enabled on all channels */
    bool hbrEnabled;                    /* If true, high bitrate audio passthrough is enabled for direct AC3+ passthrough.  
                                           This does increase memory requirements. */
    int maximumProcessingBranches;      /* Set this to the maximum number of active post processing branches active on a single channel.  Default=0. */
    unsigned audioCaptureFifoSize;      /* Audio capture fifo size in bytes (per-channel).  On some systems, this must be set only at initialization time. */
    unsigned audioCaptureChannels;      /* Default = 2 for stereo.  To capture 5.1 content use 6. */

    uint32_t (*readReserved)(void *pParam1, int param2, uint32_t physicalAddress);              /* Read 4 bytes of data starting at physicalAddress */
    void (*writeReserved)(void *pParam1, int param2, uint32_t physicalAddress, uint32_t data);  /* Write 4 bytes of data starting at physicalAddress */
    uint32_t (*readCoreRegister)(void *pParam1, int param2, uint32_t offset);                   /* Read 4 bytes of data from the core register offset specified */
    void (*writeCoreRegister)(void *pParam1, int param2, uint32_t offset, uint32_t data);       /* Write 4 bytes of data to the core register offset specified */
    uint32_t (*readCmtControl)(void *pParam1, int param2);                                      /* Read the CMT control register (CP0 22 sel 2) */
    void (*writeCmtControl)(void *pParam1, int param2, uint32_t data);                          /* Write the CMT control register (CP0 22 sel 2) */
    void *pReservedParam1;
    int reservedParam2;
} NEXUS_AudioModuleSettings;

void NEXUS_AudioModule_GetDefaultSettings(
    NEXUS_AudioModuleSettings *pSettings    /* [out] */
    );

NEXUS_ModuleHandle NEXUS_AudioModule_Init(
    const NEXUS_AudioModuleSettings *pSettings  /* NULL will use default settings */
    );

void NEXUS_AudioModule_Uninit(void);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef NEXUS_AUDIO_INIT_H__ */

