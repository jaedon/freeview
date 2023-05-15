/******************************************************************************
 *    (c)2011-2012 Broadcom Corporation
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
 * $brcm_Workfile: nexus_audio_equalizer.c $
 * $brcm_Revision: 1 $
 * $brcm_Date: 7/30/12 4:36p $
 * 
 * Module Description:
 * 
 * Revision History:
 * 
 * $brcm_Log: /nexus/modules/audio/src/rap_v3/nexus_audio_equalizer.c $
 * 
 * 1   7/30/12 4:36p jgarrett
 * SW7435-266: Switching to unified audio module structure
 * 
 * 7   6/19/12 12:41p vsilyaev
 * SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
 * 
 * 6   5/31/12 10:34a jgarrett
 * SW7425-3133: Adding audio_equalizer_disabled option
 * 
 * 5   2/22/12 3:31p jgarrett
 * SW7405-5003: Removing BKNI_Memcmp
 * 
 * 4   9/19/11 5:34p jgarrett
 * SW7231-372: Coverity CID 35386
 * 
 * 3   9/15/11 3:26p jgarrett
 * SWDTV-8673: Adding checks for changing subwoofer/subsonic filters on
 *  the fly
 * 
 * 2   9/8/11 8:45a jgarrett
 * SWDTV-6627: Adding nexus EQ support
 * 
 * 1   8/18/11 5:51p jgarrett
 * SWDTV-6306: Merge DTV APE changes to main branch
 * 
 * Nexus_APE_Integration/1   7/1/11 5:50p jgarrett
 * SWDTV-6306: Integrated to latest 7422 baseline and nexus audio
 *  directory structure
 * 
 * 1   4/11/11 5:52p jgarrett
 * SWDTV-6306: Adding preliminary Nexus APE support for DTV
 * 
 *****************************************************************************/
#include "nexus_audio_module.h"

BDBG_MODULE(nexus_audio_equalizer);

typedef struct NEXUS_AudioEqualizerOutputNode
{
    BLST_S_ENTRY(NEXUS_AudioEqualizerOutputNode) node;
    NEXUS_AudioOutput output;           /* Output attached to this equalizer */
} NEXUS_AudioEqualizerOutputNode;

/* You can have at most one of each type in this interface.  All types except cascaded PEQ are 1 stage.  Cascated PEQ is up to NEXUS_MAX_CASCADED_PARAMETRIC_EQ stages.  Subwoofer/Subsonic are two additional. */
#define NEXUS_AUDIO_EQUALIZER_MAX_STAGES ((2+NEXUS_AUDIO_EQUALIZER_MAX_ADDITIONAL_MODE) + NEXUS_MAX_CASCADED_PARAMETRIC_EQ + 2)
#define EQ_STAGE_ID_INVALID (0xffffffff)

typedef struct NEXUS_AudioEqualizer
{
    NEXUS_OBJECT(NEXUS_AudioEqualizer);
    void *pDummy;
} NEXUS_AudioEqualizer;

void NEXUS_AudioEqualizer_GetDefaultSettings(
    NEXUS_AudioEqualizerSettings *pSettings    /* [out] Default Settings */
    )
{
    BSTD_UNUSED(pSettings);
}

NEXUS_AudioEqualizerHandle NEXUS_AudioEqualizer_Open(
    unsigned index,
    const NEXUS_AudioEqualizerSettings *pSettings   /* Pass NULL for defaults */
    )
{
    BSTD_UNUSED(index);
    BSTD_UNUSED(pSettings);
    (void)BERR_TRACE(BERR_NOT_SUPPORTED);
    return NULL;
}


static void NEXUS_AudioEqualizer_P_Finalizer(
    NEXUS_AudioEqualizerHandle handle
    )
{
    BSTD_UNUSED(handle);
}

NEXUS_OBJECT_CLASS_MAKE(NEXUS_AudioEqualizer, NEXUS_AudioEqualizer_Close);

void NEXUS_AudioEqualizer_GetSettings(
    NEXUS_AudioEqualizerHandle handle,
    NEXUS_AudioEqualizerSettings *pSettings    /* [out] Current Settings */
    )
{
    BSTD_UNUSED(handle);
    BSTD_UNUSED(pSettings);
}

NEXUS_Error NEXUS_AudioEqualizer_SetSettings(
    NEXUS_AudioEqualizerHandle handle,
    const NEXUS_AudioEqualizerSettings *pSettings
    )
{
    BSTD_UNUSED(handle);
    BSTD_UNUSED(pSettings);
    return BERR_TRACE(BERR_NOT_SUPPORTED);
}

NEXUS_Error NEXUS_AudioOutput_SetEqualizer(
    NEXUS_AudioOutput output,
    NEXUS_AudioEqualizerHandle equalizer    /* Pass NULL to remove any equalizer connected to this output */
    )
{
    BSTD_UNUSED(output);
    BSTD_UNUSED(equalizer);
    return BERR_TRACE(BERR_NOT_SUPPORTED);
}

