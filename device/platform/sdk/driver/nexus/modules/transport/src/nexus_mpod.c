/******************************************************************************
 *    (c)2008-2012 Broadcom Corporation
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
 * $brcm_Workfile: nexus_mpod.c $
 * $brcm_Revision: 22 $
 * $brcm_Date: 5/24/12 2:44p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/transport/7400/src/nexus_mpod.c $
 * 
 * 22   5/24/12 2:44p gmullen
 * SW7425-3108: Preserve byteSync default from BXPT
 * 
 * 21   3/27/12 5:15p bandrews
 * SW7425-2531: check for playpump vs. parser band before dereferencing
 *  parser band
 * 
 * 20   10/31/11 7:47p bandrews
 * SW7231-391: merge to main
 * 
 * SW7420-2078/1   10/25/11 5:22p bandrews
 * SW7231-391: update parser band and timebase implementations to use
 *  handles everywhere, even for legacy enum usage
 * 
 * 19   10/13/11 3:32p erickson
 * SW7420-1120: improve code for platforms without parser bands
 * 
 * 18   4/19/11 10:48a erickson
 * SW7125-908: rework mpod clock delay setting
 * 
 * 17   12/16/10 6:07p vsilyaev
 * SW7425-39: Added playpump private interface
 * 
 * 16   10/28/10 5:09p erickson
 * SW7422-20: adapt to new XPT PI (backward compat using magnum and nexus
 *  macros)
 * 
 * 15   9/20/10 6:10p mphillip
 * SW7420-1044: Add support for new mpod API
 * 
 * 14   12/11/09 3:31p erickson
 * SW7408-1: convert to standard XPT define
 *
 * 13   12/1/09 6:31p randyjew
 * SW7468-6: Add 7468 support
 *
 * 13   12/1/09 6:29p randyjew
 * SW7468-6: Add 7468 support
 *
 * 12   11/19/09 4:25p gmohile
 * SW7408-1 : Add 7408 support
 *
 * 11   10/9/09 10:41a erickson
 * SW7325-605: add NEXUS_MpodOpenSettings.clockNotRunning
 *
 * 10   9/22/09 1:03p erickson
 * SW7325-605: add NEXUS_MpodOpenSettings.outputInvertSync. update
 *  comments.
 *
 * 9   7/31/09 2:36p gmohile
 * PR 56512 : Add 7401/03 support
 *
 * 8   7/6/09 10:18a erickson
 * PR56566: fix memory leak in NEXUS_MpodInput_Close
 *
 * 7   2/24/09 5:27p anilmm
 * PR52416:  parser band allpass setting not updated in shadow parser band
 *  config setting
 *
 * 6   9/23/08 9:16a jgarrett
 * PR 46513: Merging 97459 support to mainline
 *
 * PR44830/2   9/15/08 5:16p anilmm
 * PR44830:  Adjust for nexus naming convention
 *
 * PR44830/1   7/28/08 7:35p anilmm
 * PR44830: Add output interface control settings to NEXUS_Mpod_Open
 *
 * 5   8/14/08 5:26p katrep
 * PR45674: Fix compiiler warning in kernel mode non debug builds
 *
 * 4   7/7/08 4:44p erickson
 * PR44486: added NEXUS_MpodOpenSettings.pod2ChipMClkMuxSelect
 *
 * 3   6/18/08 1:28p erickson
 * PR43730: fix warning, remove unneeded handle in GetDefaultSettings
 *
 * 2   5/21/08 3:09p shyam
 * PR 42103 : Make Input handle a Nexus DBG object
 * PR 42103 : Fix the missing PbBand initialization
 *
 * 1   5/20/08 7:46p shyam
 * PR 42103 : Add transport MPOD support
 *
 *****************************************************************************/

#include "nexus_transport_module.h"
#include "nexus_mpod.h"

BDBG_MODULE(nexus_mpod);

#if BXPT_HAS_MPOD_SCARD_SUPPORT

#include "bxpt_mpod.h"
#include "nexus_playpump_impl.h"

BDBG_OBJECT_ID(NEXUS_Mpod);
BDBG_OBJECT_ID(NEXUS_MpodInput);

/**
TODO: suggestions on rework:
- require a NEXUS_MpodInput_Open, even more spod. this would allow us to remove the parserBand/playpump from OpenSettings.
- remove NEXUS_MpodBandType and unions. just do: if playpump == NULL, use parserBand.
**/

struct NEXUS_Mpod
{
    BDBG_OBJECT(NEXUS_Mpod)
    NEXUS_MpodOpenSettings openSettings;
};

struct NEXUS_MpodInput
{
    BDBG_OBJECT(NEXUS_MpodInput)
    BXPT_ParserType type;
    unsigned number;
    NEXUS_ParserBandHandle parserBand;
    NEXUS_MpodInputSettings inputSettings;
};


/* Summary : Gets Open Settings */
void NEXUS_Mpod_GetDefaultOpenSettings(NEXUS_MpodOpenSettings* pOpenSettings)
{
    BDBG_ASSERT(pOpenSettings);
    BKNI_Memset(pOpenSettings, 0, sizeof(NEXUS_MpodOpenSettings));

    /* The few settings where BXPT's defaults will differ from 0 */
    pOpenSettings->byteSync = true;
}

/* Summary : Opens/inits the device */
NEXUS_MpodHandle NEXUS_Mpod_Open(
    unsigned int index,
    const NEXUS_MpodOpenSettings *pOpenSettings
)
{
    BERR_Code rc;
    NEXUS_MpodOpenSettings openSettings;
    NEXUS_MpodHandle mpod;
    BXPT_Mpod_Config config;
    NEXUS_PlaypumpHandle playpump;
    NEXUS_ParserBandHandle parserBand;

    BSTD_UNUSED(index);
    if (!pOpenSettings) {
        NEXUS_Mpod_GetDefaultOpenSettings(&openSettings);
        pOpenSettings = &openSettings;
    }

    mpod = (NEXUS_MpodHandle)BKNI_Malloc(sizeof(*mpod));
    if (!mpod) {
        rc=BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);
        return NULL;
    }
    BKNI_Memset(mpod, 0, sizeof(*mpod));
    BDBG_OBJECT_SET(mpod, NEXUS_Mpod);
    mpod->openSettings = *pOpenSettings;
    BXPT_Mpod_GetDefaultConfig( pTransport->xpt, &config );
#if BXPT_HAS_MPOD_SCARD_SUPPORT
    if (pOpenSettings->bandType == NEXUS_MpodBandType_ePlaypump)
    {
        playpump = pOpenSettings->band.playpump;
        parserBand = NULL;
    }
    else
    {
        playpump = NULL;
        /* returns NULL on platforms with no parser bands */
        parserBand = NEXUS_ParserBand_Resolve_priv(pOpenSettings->band.parserBand);
        if (!parserBand)
        {
            BERR_TRACE(NEXUS_INVALID_PARAMETER);
            goto error;
        }
    }

    config.SmodeEn = (pOpenSettings->mpodMode==NEXUS_MpodMode_eSpod) ;
    config.BandEn = pOpenSettings->enableBandAssociation;
    config.PbBand = playpump != NULL;
    config.BandNo = playpump ? playpump->index : (parserBand ? parserBand->hwIndex : (unsigned char)-1);
#endif
    config.ByteSync = pOpenSettings->byteSync;
    if (pOpenSettings->clockDelay) {
        config.NshiftClk = 1;
        config.ClkDelay = pOpenSettings->clockDelay;
    }
    else {
        config.NshiftClk = 0;
        config.ClkDelay = 0;
    }
    config.InvertClk = pOpenSettings->invertClock;
    config.ClkNrun = pOpenSettings->clockNotRunning;
#if NEXUS_DTV_PLATFORM == 1
    config.Pod2ChipMClkMuxSelect = pOpenSettings->pod2ChipMClkMuxSelect;
#endif
    config.OutputInvertSync = pOpenSettings->outputInvertSync;
    rc = BXPT_Mpod_Init( pTransport->xpt, &config );
    if(rc != BERR_SUCCESS)
    {
        rc=BERR_TRACE(rc);
        goto error;
    }

    /* Successful return */
    return mpod;

error:
    if (mpod) BKNI_Free(mpod);
    return NULL;
}

void NEXUS_Mpod_Close(NEXUS_MpodHandle mpod)
{
    BDBG_OBJECT_ASSERT(mpod, NEXUS_Mpod);

    BXPT_Mpod_Shutdown(pTransport->xpt);
    BDBG_OBJECT_DESTROY(mpod, NEXUS_Mpod);
    BKNI_Free(mpod);
}

/**
Summary:
Opens an input channel to Mpod
**/
NEXUS_MpodInputHandle NEXUS_MpodInput_Open(
    NEXUS_MpodHandle mpod,
    const NEXUS_MpodInputSettings *pInputSettings
    )
{
    BERR_Code brc ;
    NEXUS_MpodInputSettings inputSettings;
    NEXUS_MpodInputHandle mpodInput ;
    NEXUS_PlaypumpHandle playpump;
    
    BDBG_OBJECT_ASSERT(mpod, NEXUS_Mpod);

    if (!pInputSettings) 
    {
        NEXUS_MpodInput_GetDefaultSettings(&inputSettings);
        pInputSettings = &inputSettings;
    }

    playpump = pInputSettings->bandType == NEXUS_MpodBandType_ePlaypump ? 
        pInputSettings->band.playpump : NULL;

#if !NEXUS_NUM_PARSER_BANDS
    if (!playpump) {
        BERR_TRACE(NEXUS_NOT_SUPPORTED);
        return NULL;
    }
#endif

    mpodInput = (NEXUS_MpodInputHandle)BKNI_Malloc(sizeof(*mpodInput));
    if (NULL == mpodInput)
    {
        BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);
        return NULL;
    }
    BKNI_Memset(mpodInput, 0, sizeof(*mpodInput));
    BDBG_OBJECT_SET(mpodInput, NEXUS_MpodInput);

    mpodInput->inputSettings = *pInputSettings ;

    if (playpump)
    {
        mpodInput->type = BXPT_ParserType_ePb;
        mpodInput->number = playpump->index;
    }
    else
    {
        mpodInput->parserBand = NEXUS_ParserBand_Resolve_priv(pInputSettings->band.parserBand);
        if (!mpodInput->parserBand)
        {
            BERR_TRACE(NEXUS_INVALID_PARAMETER);
            goto error_openinput;
        }
        
        mpodInput->type = BXPT_ParserType_eIb;
        mpodInput->number = mpodInput->parserBand->hwIndex;

        brc = BXPT_SetParserEnable(pTransport->xpt, mpodInput->parserBand->hwIndex, true);
        if (brc != BERR_SUCCESS)
        {
            BERR_TRACE(brc);
            goto error_openinput;
        }
    }

    if (pInputSettings->mpodPreFilter)
    {
        brc = BXPT_Mpod_RouteToMpodPidFiltered(
            pTransport->xpt,
            mpodInput->type,
            mpodInput->number,
            true, /* bool MpodPidFilter */
            false, /* bool ContinuityCountCheck */
            true /* bool Enable */
            );
    }
    else
    {
        brc = BXPT_Mpod_RouteToMpod(
                pTransport->xpt,
                mpodInput->type,
                mpodInput->number,
                true);
    }
    
    if (brc != BERR_SUCCESS)
    {
        BERR_TRACE(brc);
        goto error_openinput;
    }

    /* TODO: is this here rather than grouped above because we want to ensure routing 
    above succeeds before changing these internal xpt settings? */
    if (playpump)
    {
        playpump->settings.allPass = pInputSettings->mpodPreFilter ? false : true;
    }
    else
    {
#if NEXUS_NUM_PARSER_BANDS
        mpodInput->parserBand->settings.allPass = pInputSettings->mpodPreFilter ? false : true;
        mpodInput->parserBand->settings.cableCard = NEXUS_CableCardType_eMCard;
#endif
    }
    
    return mpodInput ;

error_openinput :
    BDBG_OBJECT_DESTROY(mpodInput, NEXUS_MpodInput);
    BKNI_Free(mpodInput);
    return NULL ;
}

void NEXUS_MpodInput_Close( NEXUS_MpodInputHandle mpodInput )
{
    BERR_Code brc;

    BDBG_OBJECT_ASSERT(mpodInput, NEXUS_MpodInput);

    if (mpodInput->inputSettings.mpodPreFilter)
    {
        brc = BXPT_Mpod_RouteToMpodPidFiltered(
            pTransport->xpt,
            mpodInput->type,
            mpodInput->number,
            false, /* bool MpodPidFilter */
            true, /* bool ContinuityCountCheck */
            false /* bool Enable */
            );
    }
    else
    {
        brc = BXPT_Mpod_RouteToMpod(
                pTransport->xpt,
                mpodInput->type,
                mpodInput->number,
                false);
    }
    
    if (brc != BERR_SUCCESS)
    {
        BERR_TRACE(brc);
    }

    if (mpodInput->type == BXPT_ParserType_ePb)
    {
        mpodInput->inputSettings.band.playpump->settings.allPass = false;
    }
    else
    {
#if NEXUS_NUM_PARSER_BANDS
        mpodInput->parserBand->settings.allPass = false;
        mpodInput->parserBand->settings.cableCard = NEXUS_CableCardType_eNone;
#endif
    }

    BDBG_OBJECT_DESTROY(mpodInput, NEXUS_MpodInput);
    BKNI_Free(mpodInput);
}

void NEXUS_MpodInput_GetDefaultSettings(
    NEXUS_MpodInputSettings *pMpodDefInputSettings /* [out] */
    )
{
    BDBG_ASSERT(pMpodDefInputSettings);
    BKNI_Memset(pMpodDefInputSettings, 0 , sizeof(*pMpodDefInputSettings));
}

void NEXUS_MpodInput_GetSettings(
    NEXUS_MpodInputHandle mpodInput,
    NEXUS_MpodInputSettings *pMpodInputSettings /* [out] */
    )
{
    BDBG_OBJECT_ASSERT(mpodInput, NEXUS_MpodInput);
    *pMpodInputSettings = mpodInput->inputSettings ;
}

NEXUS_Error NEXUS_MpodInput_SetSettings(
    NEXUS_MpodInputHandle mpodInput,
    const NEXUS_MpodInputSettings *pMpodInputSettings
    )
{
    NEXUS_Error rc = NEXUS_SUCCESS ;
    BERR_Code brc ;

    BDBG_OBJECT_ASSERT(mpodInput, NEXUS_MpodInput);
    BDBG_ASSERT(pMpodInputSettings);

    /* All Pass */
    if(mpodInput->inputSettings.allPass != pMpodInputSettings->allPass)
    {
        if (pMpodInputSettings->allPass)
        {
            /* TODO: this looks like it assumes parser band input.
            Shouldn't it check if playpump or not? */
            rc = BXPT_ParserAllPassMode(
                    pTransport->xpt,
                    mpodInput->number,
                    pMpodInputSettings->allPass);
            if (rc != BERR_SUCCESS)
                    return BERR_TRACE(rc);
        }
        brc = BXPT_Mpod_AllPass(
                pTransport->xpt,
                mpodInput->type,
                mpodInput->number,
                pMpodInputSettings->allPass);
        if (brc != BERR_SUCCESS)
            rc = BERR_TRACE(brc);
    }

    mpodInput->inputSettings = *pMpodInputSettings ;

    return rc ;
}

#else

void NEXUS_Mpod_GetDefaultOpenSettings(NEXUS_MpodOpenSettings* pOpenSettings)
{
    BSTD_UNUSED(pOpenSettings);
    BDBG_WRN(("Mpod not enabled on this chipset"));
    BERR_TRACE(BERR_NOT_SUPPORTED);
    return;
}

NEXUS_MpodHandle NEXUS_Mpod_Open(unsigned int index, const NEXUS_MpodOpenSettings *pOpenSettings)
{
    BSTD_UNUSED(index);
    BSTD_UNUSED(pOpenSettings);
    BDBG_WRN(("Mpod not enabled on this chipset"));
    BERR_TRACE(BERR_NOT_SUPPORTED);
    return NULL;
}

void NEXUS_Mpod_Close(NEXUS_MpodHandle mpod)
{
    BSTD_UNUSED(mpod);
    BDBG_WRN(("Mpod not enabled on this chipset"));
    BERR_TRACE(BERR_NOT_SUPPORTED);
    return;
}

NEXUS_MpodInputHandle NEXUS_MpodInput_Open(NEXUS_MpodHandle mpod, const NEXUS_MpodInputSettings *pInputSettings)
{
    BSTD_UNUSED(mpod);
    BSTD_UNUSED(pInputSettings);
    BDBG_WRN(("Mpod not enabled on this chipset"));
    BERR_TRACE(BERR_NOT_SUPPORTED);
    return NULL;
}

void NEXUS_MpodInput_Close( NEXUS_MpodInputHandle mpodInput )
{
    BSTD_UNUSED(mpodInput);
    BDBG_WRN(("Mpod not enabled on this chipset"));
    BERR_TRACE(BERR_NOT_SUPPORTED);
    return;
}

void NEXUS_MpodInput_GetDefaultSettings(NEXUS_MpodInputSettings *pMpodDefInputSettings)
{
    BSTD_UNUSED(pMpodDefInputSettings);
    BDBG_WRN(("Mpod not enabled on this chipset"));
    BERR_TRACE(BERR_NOT_SUPPORTED);
    return;
}

void NEXUS_MpodInput_GetSettings(NEXUS_MpodInputHandle mpodInput, NEXUS_MpodInputSettings *pMpodInputSettings)
{
    BSTD_UNUSED(mpodInput);
    BSTD_UNUSED(pMpodInputSettings);
    BDBG_WRN(("Mpod not enabled on this chipset"));
    BERR_TRACE(BERR_NOT_SUPPORTED);
    return;
}

NEXUS_Error NEXUS_MpodInput_SetSettings(NEXUS_MpodInputHandle mpodInput, const NEXUS_MpodInputSettings *pMpodInputSettings)
{
    BSTD_UNUSED(mpodInput);
    BSTD_UNUSED(pMpodInputSettings);
    BDBG_WRN(("Mpod not enabled on this chipset"));
    BERR_TRACE(BERR_NOT_SUPPORTED);
    return 0;
}

#endif
