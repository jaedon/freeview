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
 * $brcm_Workfile: nexus_parser_band.c $
 * $brcm_Revision: 67 $
 * $brcm_Date: 10/5/12 4:57p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/transport/src/nexus_parser_band.c $
 *
 * 67   10/5/12 4:57p bandrews
 * SW7425-4040: return NULL instead of eInvalid
 *
 * 66   10/3/12 5:01p jtna
 * SW7425-3782: refactor nexus tsmf impl
 *
 * 65   8/3/12 7:10p bandrews
 * SW7429-227: add P_SetEnable stub for plats with no parser bands
 *
 * 64   8/2/12 4:22p erickson
 * SW7429-227: call NEXUS_OBJECT_SET for unprotected NEXUS_ParserBand
 *  enums
 *
 * 63   8/1/12 5:47p bandrews
 * SW7420-2085: sob
 *
 * 62   8/1/12 4:48p bandrews
 * SW7420-2085: last update to support platforms without parser bands
 *
 * 61   7/31/12 8:10p bandrews
 * SW7420-2085: one more fix for platforms without parser bands
 *
 * 60   7/31/12 8:05p bandrews
 * SW7420-2085: fix calling of NEXUS_OBJECT_CLASS_MAKE for platforms with
 *  no parser bands
 *
 * 59   7/23/12 11:36a vsilyaev
 * SW7420-2328: Use per-thread storage to keep information about the
 *  current client, remove per-module client information
 *
 * 58   7/6/12 1:02p erickson
 * SW7552-292: fix warning
 *
 * 57   7/6/12 11:46a erickson
 * SW7552-292: fix infinite recursion when using cont_count_ignore debug
 *  env variable
 *
 * 56   6/26/12 3:15p jtna
 * SW7425-3062: add feature to propagate host parserband settings to
 *  demod. deprecate NEXUS_Frontend_Get/SetTransportSettings
 *
 * 55   6/20/12 1:44p erickson
 * SW7420-2085: use NEXUS_OBJECT_SET for parser band
 *
 * 54   6/19/12 12:43p vsilyaev
 * SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
 *
 * 53   6/6/12 7:00p jtna
 * SW7425-2752: fix OOB data routing
 *
 * 52   5/10/12 6:17p jtna
 * SW7425-2752: rework MTSIF-frontend <-> host interface
 *
 * 51   5/9/12 6:50p bandrews
 * SW7550-793: change assert to what we actually say in the comment
 *
 * 50   3/27/12 11:24a erickson
 * SW7435-77: add #if for minimal resource build
 *
 * 49   2/13/12 5:09p gmullen
 * SW7425-2383: Fixed coverity defect
 *
 * 48   2/2/12 8:51p bandrews
 * SW7425-2299: read-only calls (GetSettings/GetStatus) do not reserve the
 *  resource if enum is used
 *
 * 47   1/24/12 10:19a gmullen
 * SW7425-1886: Merged to main
 *
 * SW7425-1886/1   1/19/12 5:44p gmullen
 * SW7425-1886: CC interrupt now goes to app
 *
 * 46   11/8/11 7:57p bandrews
 * SW7231-446: adding stub fns for platforms with no parser bands
 *
 * 45   11/8/11 7:28p bandrews
 * SW7231-446: fix for platforms without parser bands
 *
 * 44   11/7/11 4:00p bandrews
 * SW7231-446: merge to main
 *
 * SW7231-446/1   11/4/11 10:11p bandrews
 * SW7231-446: updates for kernel mode
 *
 * 43   10/31/11 7:48p bandrews
 * SW7231-391: merge to main
 *
 * SW7420-2078/3   10/25/11 7:26p bandrews
 * SW7231-391: fix warnings
 *
 * SW7420-2078/2   10/25/11 5:22p bandrews
 * SW7231-391: update parser band and timebase implementations to use
 *  handles everywhere, even for legacy enum usage
 *
 * SW7420-2078/1   10/11/11 8:26p bandrews
 * SW7231-391: add support for parser band and timebase protected client
 *  usage
 *
 * 42   10/17/11 11:38a gmullen
 * SW7425-1383:  Merged to main
 *
 * SW7425-1383/2   10/14/11 4:09p gmullen
 * SW7425-1383: Updated per David's suggestions
 *
 * SW7425-1383/1   10/11/11 3:45p gmullen
 * SW7425-1383: Added API to return the chip-specific allPass PID channel
 *  number
 *
 * 41   10/6/11 2:04p erickson
 * SW7346-502: add NEXUS_ParserBandSettings.acceptAdapt00
 *
 * 40   7/22/11 11:27a erickson
 * SW7420-2004: enable parser band if NEXUS_ParserBandSettings.cableCard
 *  is not eNone
 *
 * 39   4/15/11 10:35a erickson
 * SW7425-313: add initial MTSIF support
 *
 * 38   4/1/11 4:25p jtna
 * SW7346-119: add 40nm allpass support
 *
 * 37   3/8/11 6:08p vsilyaev
 * SW7422-101: Updated logic of handling continuityCountEnabled, so per
 *  band and per pid settings are combined with logical AND
 *
 * 36   2/24/11 2:32p rjlewis
 * SW7425-89: DSS needs to use binary mode for timestamp generation.
 *
 * 35   2/11/11 10:23a gmohile
 * SW7420-1120 : Fix platform with no parser band
 *
 * 34   2/10/11 11:02a erickson
 * SW7420-1120: refactor NEXUS_PidChannel_Open so it returns separate
 *  handles even if underlying HW pid channel is shared
 *
 * 33   2/9/11 2:18p erickson
 * SW7420-1009: add NEXUS_ParserBand_Acquire and Release
 *
 * 32   10/28/10 5:09p erickson
 * SW7422-20: adapt to new XPT PI (backward compat using magnum and nexus
 *  macros)
 *
 * 31   10/21/10 10:44a erickson
 * SW35230-1563: add NEXUS_ParserBandSettings.teiIgnoreEnabled
 *
 * 30   12/11/09 6:28p mphillip
 * SW7550-112: Convert from specific chips to BXPT_HAS for XC and RS buf
 *
 * 29   12/11/09 6:18p mphillip
 * SW7550-112: Merge 7550 changes to main branch
 *
 * Refsw_7550/1   9/18/09 8:27a nitinb
 * SW7550-29: Getting initial nexus build for 7550
 *
 * 28   12/8/09 2:30p gmohile
 * SW7408-1 : add countCCerror isr function for 7408
 *
 * 27   12/7/09 9:39p randyjew
 * SW7468-6: 7468 to not use BXPT_Rave_SetRSXCDataRate
 *
 * 26   12/1/09 6:31p randyjew
 * SW7468-6: Add 7468 support
 *
 * 26   12/1/09 6:29p randyjew
 * SW7468-6: Add 7468 support
 *
 * 25   11/19/09 4:25p gmohile
 * SW7408-1 : Add 7408 support
 *
 * 24   10/20/09 5:56p jhaberf
 * SW35230-1: BXPT_Remux_SetRSXCDataRate() is not required for the 35230.
 *  #ifdef'd out calls to this function.
 *
 * 23   10/1/09 5:03p erickson
 * SW7405-3087: add playpump cc check, add cc error counting per pid
 *
 * 22   4/20/09 10:53a erickson
 * PR53662: rework XPT power management code to call BXPT_P_CanPowerDown
 *  before powering down core
 *
 * 21   4/17/09 12:31p erickson
 * PR53728: switch to single XPT function for controlling RS and XC data
 *  rates
 *
 * 20   12/17/08 1:58p katrep
 * PR49873: Add support for 7466
 *
 * 19   12/15/08 5:41p jtna
 * PR49927: add NEXUS_ParserBandSettings.lengthError
 *
 * 18   12/11/08 8:05a vsilyaev
 * PR 48908: Changed algorithm to power up/down blocks
 *
 * 17   12/10/08 7:36p vsilyaev
 * PR 48908: Added control of power management
 *
 * 16   9/3/08 7:37p vishk
 * PR 46315: Sample code for PID remapping
 *
 * 15   8/14/08 5:26p katrep
 * PR45674: Fix compiiler warning in kernel mode non debug builds
 *
 * 14   6/25/08 4:30p katrep
 * PR44085: Program the block XPT RS/XC buffer blockout counters and
 *  according to packet size.
 *
 * 13   6/17/08 5:58p gmullen
 * PR43710:
 *
 * 12   6/16/08 4:16p erickson
 * PR43710: make BXPT_ParserAllPassMode one way until XPT PI fixed
 *
 * 11   6/13/08 7:19p erickson
 * PR42973: merge all pass changes
 *
 * PR42973/2   6/3/08 1:17p shyam
 * PR 42973 : Enable 81 Mbps data rates
 *
 * PR42973/1   5/22/08 3:27p shyam
 * PR 42973 : Add allPass and Null PAcket support to Nexus
 *
 * 10   5/22/08 10:24a erickson
 * PR34925: apply cont_count_ignore even if user doesn't call
 *  NEXUS_ParserBand_SetSettings
 *
 * 9   5/9/08 4:51p erickson
 * PR34925: added cont_count_ignore env override for debug
 *
 * 8   4/28/08 4:20p erickson
 * PR42197: fix CASSERT logic
 *
 * 7   4/28/08 11:53a erickson
 * PR42197: remove NEXUS_ParserBand_ePlayback enums
 *
 * 6   4/1/08 4:59p erickson
 * PR41157: fix warning
 *
 * 5   4/1/08 4:48p erickson
 * PR41152: check parserBand bounds before setting interrupts
 *
 * 4   3/27/08 10:37a erickson
 * PR40851: if parser band is DSS, it should override some input band
 *  settings. if not DSS, don't override.
 *
 * PR40851/1   3/27/08 2:54p jerryy
 * PR40851 : Un-necessary input band setting in parser band settings will
 *  cause inputband 0 cannot receive input correctly.
 *
 * 3   3/5/08 9:33a erickson
 * PR40080: more RDB exceptions
 *
 * 2   3/5/08 9:09a erickson
 * PR40080: fix 7401 and 3563 compilation
 *
 * 1   3/4/08 3:31p erickson
 * PR40080: add transport error callbacks
 *
 *****************************************************************/
#include "nexus_transport_module.h"
#include "nexus_class_verification.h"
#include "bchp_xpt_fe.h"
#if NEXUS_HAS_TSMF
#include "priv/nexus_tsmf_priv.h"
#endif
#include "priv/nexus_transport_priv.h"

#if BXPT_HAS_XCBUF
#include "bxpt_xcbuf.h"
#endif
#if BXPT_HAS_RSBUF
#include "bxpt_rsbuf.h"
#endif

BDBG_MODULE(nexus_parser_band);

/* when we receive a parser band CC error interrupt, we can look up which pid channel had the error.
this is not exact, but the approximate count is useful. */
void NEXUS_ParserBand_P_CountCcErrors_isr(void)
{
    NEXUS_PidChannelHandle pidChannel;
    unsigned i;

#if NEXUS_PARSER_BAND_CC_CHECK
    uint32_t value[NEXUS_NUM_PID_CHANNELS/32];

    /* read & clear status immediately to maximize chance of catching the next one */
    for (i=0;i<NEXUS_NUM_PID_CHANNELS/32;i++) {
        uint32_t addr = BCHP_XPT_FE_PCC_ERROR0 + (i*4);
        value[i] = BREG_Read32(g_pCoreHandles->reg, addr);
        if (value[i]) {
            BREG_Write32(g_pCoreHandles->reg, addr, 0); /* clear status immediately to maximize chance of catching the next one */
        }
    }

    /* now apply to each pid channel */
    for (pidChannel = BLST_S_FIRST(&pTransport->pidChannels); pidChannel; pidChannel = BLST_S_NEXT(pidChannel, link)) {
        unsigned index = pidChannel->status.pidChannelIndex;
        if ((value[index/32]>>(index%32)) & 0x1) {
            pidChannel->status.continuityCountErrors++;
        }
    }
#else
    bool value[NEXUS_NUM_PID_CHANNELS];
    uint32_t Reg;

    /* read & clear status immediately to maximize chance of catching the next one */
    for (i = 0; i < NEXUS_NUM_PID_CHANNELS; i++)
    {
        uint32_t addr = BCHP_XPT_FULL_PID_PARSER_STATE_CONFIG_0_i_ARRAY_BASE + (i*4);

        Reg = BREG_Read32(g_pCoreHandles->reg, addr);
        value[ i ] = BCHP_GET_FIELD_DATA( Reg, XPT_FULL_PID_PARSER_STATE_CONFIG_0_i, PCC_ERROR ) ? true : false;
        if (value[i])
        {
            Reg &= ~BCHP_MASK( XPT_FULL_PID_PARSER_STATE_CONFIG_0_i, PCC_ERROR );
            BREG_Write32(g_pCoreHandles->reg, addr, Reg); /* clear status immediately to maximize chance of catching the next one */
        }
    }

    /* now apply to each pid channel */
    for (pidChannel = BLST_S_FIRST(&pTransport->pidChannels); pidChannel; pidChannel = BLST_S_NEXT(pidChannel, link))
    {
        unsigned index = pidChannel->status.pidChannelIndex;

        if ( value[index] )
        {
            pidChannel->status.continuityCountErrors++;
        }
    }
#endif
}

#if NEXUS_NUM_PARSER_BANDS

static NEXUS_Error NEXUS_ParserBand_P_SetInterrupts(NEXUS_ParserBandHandle parserBand, const NEXUS_ParserBandSettings *pSettings);

static int NEXUS_ParserBand_P_NextAvailable(void)
{
    int next = -1;
    unsigned i = 0;

    for (i = 0; i < NEXUS_NUM_PARSER_BANDS; i++)
    {
        if (!pTransport->parserBand[i].acquired)
        {
            next = i;
            break;
        }
    }

    return next;
}

static NEXUS_ParserBandHandle NEXUS_ParserBand_P_ResolveAcquire(NEXUS_ParserBand band, bool acquire)
{
    NEXUS_ParserBandHandle out = NULL;

    if (band != NEXUS_ParserBand_eInvalid)
    {
        if ((unsigned)band < (unsigned)NEXUS_ParserBand_eMax)
        {
            unsigned index = band - NEXUS_ParserBand_e0; /* assumes continuous enums */

            if (index < NEXUS_NUM_PARSER_BANDS)
            {
                /* enum variant */
                if (acquire)
                {
#if BDBG_DEBUG_BUILD
                    if (!pTransport->parserBand[index].acquired)
                    {
                        BDBG_MSG(("Allocating parser band %u to unprotected client", index));
                        /* increment ref_cnt. it will never be decremented. */
                        NEXUS_OBJECT_SET(NEXUS_ParserBand, &pTransport->parserBand[index]);
                    }
#endif
                    pTransport->parserBand[index].acquired = true;
                }
                out = &pTransport->parserBand[index];
            }
            else
            {
                BERR_TRACE(NEXUS_INVALID_PARAMETER);
            }
        }
        else if ((unsigned)band > (unsigned)NEXUS_ParserBand_eMax)
        {
            /* pointer variant */
            out = (NEXUS_ParserBandHandle)band;
            BDBG_OBJECT_ASSERT(out, NEXUS_ParserBand);
        }
        else /*if (band == NEXUS_ParserBand_eMax)*/
        {
            BERR_TRACE(NEXUS_INVALID_PARAMETER);
        }
    }
    /* else == eInvalid -> return NULL */

    return out;
}

NEXUS_ParserBandHandle NEXUS_ParserBand_Resolve_priv(NEXUS_ParserBand band)
{
    return NEXUS_ParserBand_P_ResolveAcquire(band, true);
}

void NEXUS_ParserBand_P_GetSettings(NEXUS_ParserBandHandle band, NEXUS_ParserBandSettings *pSettings)
{
    BDBG_OBJECT_ASSERT(band, NEXUS_ParserBand);
    if (pSettings)
    {
        *pSettings = band->settings;
    }
}

void NEXUS_ParserBand_GetSettings(NEXUS_ParserBand band, NEXUS_ParserBandSettings *pSettings)
{
    NEXUS_ParserBandHandle handle;

    handle = NEXUS_ParserBand_P_ResolveAcquire(band, false);

    if (handle)
    {
        NEXUS_ParserBand_P_GetSettings(handle, pSettings);
    }
    else
    {
        BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }
}

NEXUS_Error NEXUS_ParserBand_P_SetSettings(NEXUS_ParserBandHandle parserBand, const NEXUS_ParserBandSettings *pSettings)
{
    BERR_Code rc;
    BXPT_ParserConfig parserCfg;
    unsigned prevMaxDataRate = 0;
    unsigned bandHwIndex;

    BDBG_OBJECT_ASSERT(parserBand, NEXUS_ParserBand);

    bandHwIndex = parserBand->hwIndex;

#if !NEXUS_PARSER_BAND_CC_CHECK
    if(pSettings->continuityCountEnabled != parserBand->settings.continuityCountEnabled && parserBand->pidChannels) {
        BDBG_WRN(("%#lx:continuityCountEnabled wouldn't get applied to aleady opened pids", bandHwIndex));
    }
#endif


    rc = BXPT_GetParserConfig(pTransport->xpt, bandHwIndex, &parserCfg);
    if (rc) {return BERR_TRACE(rc);}

    switch (pSettings->sourceType) {
    case NEXUS_ParserBandSourceType_eInputBand:
#if NEXUS_HAS_TSMF
    case NEXUS_ParserBandSourceType_eTsmf: /* TSMF source requires inputband to be configured properly */
#endif
        rc = BXPT_SetParserDataSource(pTransport->xpt, bandHwIndex, BXPT_DataSource_eInputBand, pSettings->sourceTypeSettings.inputBand);
        if (rc) {return BERR_TRACE(rc);}

        rc = NEXUS_InputBand_P_SetTransportType(pSettings->sourceTypeSettings.inputBand, pSettings->transportType);
        if (rc) {return BERR_TRACE(rc);}
        break;

#if NEXUS_NUM_REMUX
    case NEXUS_ParserBandSourceType_eRemux:
        rc = BXPT_SetParserDataSource(pTransport->xpt, bandHwIndex, BXPT_DataSource_eRemuxFeedback, pSettings->sourceTypeSettings.remux->index);
        if (rc) {return BERR_TRACE(rc);}
        break;
#endif

#if NEXUS_NUM_MTSIF
    case NEXUS_ParserBandSourceType_eMtsif:
        break; /* keep going with the rest of the function. the host PB settings still need to be set */
#endif

    default:
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }

#if NEXUS_PARSER_BAND_CC_CHECK
    parserCfg.ContCountIgnore = !pSettings->continuityCountEnabled;
    if (NEXUS_GetEnv("cont_count_ignore")) {
        parserCfg.ContCountIgnore = true;
    }
#endif
    parserCfg.ErrorInputIgnore = pSettings->teiIgnoreEnabled;
    parserCfg.AcceptNulls = pSettings->acceptNullPackets;
    parserCfg.TsMode = NEXUS_IS_DSS_MODE(pSettings->transportType) ? BXPT_ParserTimestampMode_eBinary : BXPT_ParserTimestampMode_eMod300;
    parserCfg.AcceptAdapt00 = pSettings->acceptAdapt00;

    rc = BXPT_SetParserConfig(pTransport->xpt, bandHwIndex, &parserCfg);
    if (rc) {return BERR_TRACE(rc);}

#if B_HAS_DSS
    rc = BXPT_DirecTv_SetParserBandMode(pTransport->xpt, bandHwIndex, NEXUS_IS_DSS_MODE(pSettings->transportType) ? BXPT_ParserMode_eDirecTv:BXPT_ParserMode_eMpeg);
    if (rc) {return BERR_TRACE(rc);}
#endif

    rc = NEXUS_ParserBand_P_SetInterrupts(parserBand, pSettings);
    if (rc) {return BERR_TRACE(rc);}

    prevMaxDataRate = parserBand->settings.maxDataRate;
    parserBand->settings = *pSettings;

#if NEXUS_HAS_TSMF
    if (pSettings->sourceType==NEXUS_ParserBandSourceType_eTsmf)
    {
        /* now that we know which PB, complete the IB -> TSMF -> PB chain */
        NEXUS_Tsmf_SetOutput_priv(parserBand);
    }
#endif

#if (!NEXUS_HAS_LEGACY_XPT) /* 40nm platforms */
    if (prevMaxDataRate != pSettings->maxDataRate) {
        BDBG_WRN(("maxDataRate can only be changed via NEXUS_TransportModuleSettings"));
        parserBand->settings.maxDataRate = prevMaxDataRate;
    }
#endif

    /* BXPT_ParserAllPassMode(false) has a side effect of restoring default RS/XC DataRate values. Therefore the order
    of the following calls is important. */
    rc = BXPT_ParserAllPassMode(pTransport->xpt, bandHwIndex, pSettings->allPass);
    if (rc) {return BERR_TRACE(rc);}

#if BXPT_HAS_XCBUF
    if(!pSettings->allPass)
    {
        rc = BXPT_Rave_SetRSXCDataRate( pTransport->rave[0].channel, BXPT_ParserType_eIb, bandHwIndex, pSettings->maxDataRate, NEXUS_IS_DSS_MODE(pSettings->transportType)?130:188);
        if (rc) {return BERR_TRACE(rc);}
    }
#endif

    /* settings.cableCard can cause enable to change */
    NEXUS_ParserBand_P_SetEnable(parserBand);

    return rc;
}

NEXUS_Error NEXUS_ParserBand_SetSettings(NEXUS_ParserBand band, const NEXUS_ParserBandSettings *pSettings)
{
    NEXUS_ParserBandHandle handle = NULL;

    handle = NEXUS_ParserBand_Resolve_priv(band);

    if (handle)
    {
        return NEXUS_ParserBand_P_SetSettings(handle, pSettings);
    }
    else
    {
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }
}

/*
Enable the parser or input band if there is one pid channel open
*/
void NEXUS_ParserBand_P_SetEnable(NEXUS_ParserBandHandle parserBand)
{
    BERR_Code rc;
    bool enabled;

    BDBG_OBJECT_ASSERT(parserBand, NEXUS_ParserBand);

    /* TODO: replace with enabled = (parserBand->_n_baseObject.ref_cnt > 1) and remove parserBand->refcnt */
    enabled = parserBand->refcnt?true:false;

    if (parserBand->settings.sourceType==NEXUS_ParserBandSourceType_eMtsif) {
        /* MTSIF connection does not require host PB to be enabled, because the MTSIF RX on the host
           feeds directly into the input buffer (even though the connection is often pictured as
           MTSIF RX feeding to the host PB)

           it may be desirable to always disable the host PB, in order to avoid a host IB plus a MTSIF
           connection feeding to the same PB */
        enabled = false;
    }

    /* cableCard forces a parser band on */
    if (!enabled && parserBand->settings.cableCard != NEXUS_CableCardType_eNone) {
        enabled = true;
    }

    if(enabled && !parserBand->enabled) {
        NEXUS_Transport_P_IncPowerDown(true);
    }
#if NEXUS_PARSER_BAND_CC_CHECK
    if (NEXUS_GetEnv("cont_count_ignore")) {
        unsigned bandHwIndex = parserBand->hwIndex;
        BXPT_ParserConfig parserCfg;
        /* SetSettings is not required to be called by the user, so set ContCountIgnore directly */
        rc = BXPT_GetParserConfig(pTransport->xpt, bandHwIndex, &parserCfg);
        if (!rc) {
            parserCfg.ContCountIgnore = true;
            (void)BXPT_SetParserConfig(pTransport->xpt, bandHwIndex, &parserCfg);
        }
    }
#endif

    /* make sure the refcnt didn't get decremented below zero */
    BDBG_ASSERT(parserBand->refcnt >= 0);
    rc = BXPT_SetParserEnable(pTransport->xpt, parserBand->hwIndex, enabled);
    if (rc) {rc=BERR_TRACE(rc);}

    if(!enabled && parserBand->enabled) {
        NEXUS_Transport_P_IncPowerDown(false);
    }
    parserBand->enabled=enabled;

#if !NEXUS_NUM_RAVE_CONTEXTS
    {
        int refcnt = 0;
        int i;
        NEXUS_InputBand inputBand = parserBand->inputBand;
        for (i=0;i<NEXUS_NUM_PARSER_BANDS;i++) {
            if (pTransport->parserBand[i].inputBand == inputBand) {
                refcnt += pTransport->parserBand[i].refcnt;
            }
        }
        enabled = refcnt?true:false;
        if(enabled && !pTransport->inputBand[inputBand].enabled) {
            NEXUS_Transport_P_IncPowerDown(true);
        }
        rc = BXPT_EnableOutputBufferBand(pTransport->xpt, inputBand, enabled);
        if (rc) {BERR_TRACE(rc);}
        if(!enabled && pTransport->inputBand[inputBand].enabled) {
            NEXUS_Transport_P_IncPowerDown(false);
        }
        pTransport->inputBand[inputBand].enabled = enabled;
    }
#endif
}

static void NEXUS_ParserBand_P_CCError_isr(void *context, int param)
{
    NEXUS_ParserBandHandle handle = context;
    BSTD_UNUSED(param);
    NEXUS_ParserBand_P_CountCcErrors_isr();
    NEXUS_IsrCallback_Fire_isr(handle->ccErrorCallback);
}

static void NEXUS_ParserBand_P_TEIError_isr(void *context, int param)
{
    NEXUS_ParserBandHandle handle = context;
    BSTD_UNUSED(param);
    NEXUS_IsrCallback_Fire_isr(handle->teiErrorCallback);
}

static void NEXUS_ParserBand_P_LengthError_isr(void *context, int param)
{
    NEXUS_ParserBandHandle handle = context;
    BSTD_UNUSED(param);
    NEXUS_IsrCallback_Fire_isr(handle->lengthErrorCallback);
}

void NEXUS_ParserBand_P_TEIError_priv_isr(NEXUS_ParserBand parserBand)
{
    NEXUS_ParserBand_P_TEIError_isr(NEXUS_ParserBand_Resolve_priv(parserBand), 0);
}

void NEXUS_ParserBand_P_LengthError_priv_isr(NEXUS_ParserBand parserBand)
{
    NEXUS_ParserBand_P_LengthError_isr(NEXUS_ParserBand_Resolve_priv(parserBand), 0);
}

static NEXUS_Error NEXUS_ParserBand_P_SetInterrupts(NEXUS_ParserBandHandle parserBand, const NEXUS_ParserBandSettings *pSettings)
{
    BERR_Code rc;

    BDBG_OBJECT_ASSERT(parserBand, NEXUS_ParserBand);

    if (pSettings->ccError.callback) {
        if (!parserBand->ccErrorInt) {
            BDBG_WRN(("create cc callback %d", parserBand));
            rc = BINT_CreateCallback(&parserBand->ccErrorInt, g_pCoreHandles->bint, BXPT_INT_ID_PARSER_CONTINUITY_ERROR(parserBand->hwIndex), NEXUS_ParserBand_P_CCError_isr, parserBand, 0);
            if (rc) return BERR_TRACE(rc);
            rc = BINT_EnableCallback(parserBand->ccErrorInt);
            if (rc) return BERR_TRACE(rc);
        }
    }
    else if (parserBand->ccErrorInt) {
        BINT_DisableCallback(parserBand->ccErrorInt);
        BINT_DestroyCallback(parserBand->ccErrorInt);
        parserBand->ccErrorInt = NULL;
    }

    if (pSettings->teiError.callback) {
        if (!parserBand->teiErrorInt) {
            BDBG_WRN(("create tei callback %d", parserBand));
            rc = BINT_CreateCallback(&parserBand->teiErrorInt, g_pCoreHandles->bint, BXPT_INT_ID_PARSER_TRANSPORT_ERROR(parserBand->hwIndex), NEXUS_ParserBand_P_TEIError_isr, parserBand, 0);
            if (rc) return BERR_TRACE(rc);
            rc = BINT_EnableCallback(parserBand->teiErrorInt);
            if (rc) return BERR_TRACE(rc);
        }
    }
    else if (parserBand->teiErrorInt) {
        BINT_DisableCallback(parserBand->teiErrorInt);
        BINT_DestroyCallback(parserBand->teiErrorInt);
        parserBand->teiErrorInt = NULL;
    }

    if (pSettings->lengthError.callback) {
        if (!parserBand->lengthErrorInt) {
            BDBG_WRN(("create length callback %d", parserBand));
            rc = BINT_CreateCallback(&parserBand->lengthErrorInt, g_pCoreHandles->bint, BXPT_INT_ID_PARSER_LENGTH_ERROR(parserBand->hwIndex), NEXUS_ParserBand_P_LengthError_isr, parserBand, 0);
            if (rc) return BERR_TRACE(rc);
            rc = BINT_EnableCallback(parserBand->lengthErrorInt);
            if (rc) return BERR_TRACE(rc);
        }
    }
    else if (parserBand->lengthErrorInt) {
        BINT_DisableCallback(parserBand->lengthErrorInt);
        BINT_DestroyCallback(parserBand->lengthErrorInt);
        parserBand->lengthErrorInt = NULL;
    }

    NEXUS_IsrCallback_Set(parserBand->ccErrorCallback, &pSettings->ccError );
    NEXUS_IsrCallback_Set(parserBand->teiErrorCallback, &pSettings->teiError);
    NEXUS_IsrCallback_Set(parserBand->lengthErrorCallback, &pSettings->lengthError);

    return 0;
}

static NEXUS_ParserBandHandle NEXUS_ParserBand_P_Open(unsigned index)
{
    NEXUS_ParserBandHandle handle = NULL;

    if (index == NEXUS_ANY_ID)
    {
        int next = NEXUS_ParserBand_P_NextAvailable();
        if (next == -1)
        {
            BERR_TRACE(NEXUS_NOT_AVAILABLE);
            goto end;
        }
        else
        {
            index = (unsigned)next;
        }
    }

    if (index >= NEXUS_NUM_PARSER_BANDS)
    {
        BERR_TRACE(NEXUS_INVALID_PARAMETER);
        goto end;
    }

    if (!pTransport->parserBand[index].acquired)
    {
        pTransport->parserBand[index].acquired = true;
        /* BDBG_MSG(("Allocating parser band %u to client %p", index, b_objdb_get_client())); */
        handle = &pTransport->parserBand[index];
        NEXUS_OBJECT_SET(NEXUS_ParserBand, handle);
    }
    else
    {
        BERR_TRACE(NEXUS_NOT_AVAILABLE);
    }

end:
    return handle;
}

NEXUS_ParserBand NEXUS_ParserBand_Open(unsigned index)
{
    NEXUS_ParserBandHandle handle = NULL;

    handle = NEXUS_ParserBand_P_Open(index);

    /* here we cast to the enum, but it is really a pointer */
    return handle?(NEXUS_ParserBand)handle:(NEXUS_ParserBand)NULL;
}

static void NEXUS_ParserBand_P_Finalizer(NEXUS_ParserBandHandle band)
{
    NEXUS_OBJECT_ASSERT(NEXUS_ParserBand, band);

    /* this should disable, if no other references */
    NEXUS_ParserBand_P_SetEnable(band);

    /* this may unregister enum variant usage, if that usage was done
    in an unprotected client *after* a protected client already acquired
    the resource */
    band->acquired = false;
    NEXUS_OBJECT_UNSET(NEXUS_ParserBand, band);

    /* BDBG_MSG(("Client %p releasing parser band %d", b_objdb_get_client(), band->hwIndex)); */
}

static NEXUS_OBJECT_CLASS_MAKE(NEXUS_ParserBand, NEXUS_ParserBand_P_Close);

void NEXUS_ParserBand_Close(NEXUS_ParserBand parserBand)
{
    NEXUS_ParserBandHandle handle = NULL;

    handle = NEXUS_ParserBand_Resolve_priv(parserBand);

    if (handle)
    {
        NEXUS_ParserBand_P_Close(handle);
    }
    else
    {
        BDBG_ERR(("You may be attempting to close the enum variant of this resource.  Please ensure you are passing the resource returned when you called open."));
        BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }
}

static bool is_mtsif(unsigned i, NEXUS_FrontendConnectorHandle connector, NEXUS_TsmfHandle *tsmf)
{
    const NEXUS_ParserBandSettings *ps = &pTransport->parserBand[i].settings;
    *tsmf = NULL;
    if (!pTransport->parserBand[i].acquired) {
        return false;
    }

    /* if connector is NULL, match any */
    if (ps->sourceType == NEXUS_ParserBandSourceType_eMtsif && (ps->sourceTypeSettings.mtsif == connector || !connector)) {
        return true;
    }
#if NEXUS_HAS_TSMF
    if (ps->sourceType == NEXUS_ParserBandSourceType_eTsmf && ps->sourceTypeSettings.tsmf) {
        NEXUS_TsmfSettings settings;
        NEXUS_Tsmf_GetSettings(ps->sourceTypeSettings.tsmf, &settings);
        if (settings.sourceType == NEXUS_TsmfSourceType_eMtsif && (settings.sourceTypeSettings.mtsif==connector || !connector)) {
            *tsmf = ps->sourceTypeSettings.tsmf;
            return true;
        }
    }
#endif
    return false;
}

void NEXUS_ParserBand_GetMtsifConnections_priv( NEXUS_FrontendConnectorHandle connector, struct NEXUS_MtsifParserBandSettings *pSettings, unsigned numEntries, unsigned *pNumReturned)
{
    unsigned i, total = 0;
    NEXUS_ASSERT_MODULE();
    for (i=0;i < NEXUS_NUM_PARSER_BANDS && total < numEntries;i++) {
        NEXUS_TsmfHandle tsmf;
        if (is_mtsif(i, connector, &tsmf)) {
            BKNI_Memset(&pSettings[total], 0, sizeof(pSettings[total]));
            pSettings[total].index = i;
            pSettings[total].settings = pTransport->parserBand[i].settings;
#if defined(NEXUS_HAS_TSMF) && defined(NEXUS_FRONTEND_45216)
            if (tsmf) {
                NEXUS_Tsmf_ReadSettings_priv(tsmf, &pSettings[total]);
            }
#endif
            total++;
        }
    }
    *pNumReturned = total;
}

void NEXUS_ParserBand_P_MtsifErrorStatus_priv(unsigned lengthError, unsigned transportError)
{
    unsigned i;
    NEXUS_ASSERT_MODULE();
    for (i=0;(lengthError || transportError) && i<NEXUS_NUM_PARSER_BANDS;i++) {
        NEXUS_TsmfHandle tsmf;
        if (!is_mtsif(i, NULL, &tsmf)) continue;
        if ((lengthError & 1) || (transportError & 1)) {
            BKNI_EnterCriticalSection();
            if (lengthError & 1) {
                NEXUS_ParserBand_P_LengthError_isr(NEXUS_ParserBand_Resolve_priv(i), 0);
            }
            if (transportError & 1) {
                NEXUS_ParserBand_P_TEIError_isr(NEXUS_ParserBand_Resolve_priv(i), 0);
            }
            BKNI_LeaveCriticalSection();
        }
        lengthError >>= 1;
        transportError >>= 1;
    }
}

#else /* NEXUS_NUM_PARSER_BANDS */

NEXUS_ParserBandHandle NEXUS_ParserBand_Resolve_priv(NEXUS_ParserBand band)
{
    BSTD_UNUSED(band);
    BERR_TRACE(NEXUS_NOT_SUPPORTED);
    return NULL;
}

void NEXUS_ParserBand_P_GetSettings(NEXUS_ParserBandHandle band, NEXUS_ParserBandSettings *pSettings)
{
    BSTD_UNUSED(band);
    BSTD_UNUSED(pSettings);
    BERR_TRACE(NEXUS_NOT_SUPPORTED);
}

void NEXUS_ParserBand_GetSettings(NEXUS_ParserBand parserBand, NEXUS_ParserBandSettings *pSettings)
{
    BSTD_UNUSED(parserBand);
    BSTD_UNUSED(pSettings);
    BDBG_WRN(("Parser Band not enabled on this chipset"));
    return;
}

NEXUS_Error NEXUS_ParserBand_SetSettings(NEXUS_ParserBand parserBand, const NEXUS_ParserBandSettings *pSettings)
{
    BSTD_UNUSED(parserBand);
    BSTD_UNUSED(pSettings);
    BDBG_WRN(("Parser Band not enabled on this chipset"));
    BERR_TRACE(NEXUS_NOT_SUPPORTED);
    return 0;
}

NEXUS_ParserBand NEXUS_ParserBand_Open(unsigned index)
{
    BSTD_UNUSED(index);
    BERR_TRACE(NEXUS_NOT_SUPPORTED);
    return NEXUS_ParserBand_eInvalid;
}

static void NEXUS_ParserBand_P_Finalizer(NEXUS_ParserBandHandle band)
{
    BSTD_UNUSED(band);
    BERR_TRACE(NEXUS_NOT_SUPPORTED);
}

static NEXUS_OBJECT_CLASS_MAKE(NEXUS_ParserBand, NEXUS_ParserBand_P_Close);

void NEXUS_ParserBand_Close(NEXUS_ParserBand parserBand)
{
    BSTD_UNUSED(parserBand);
    BERR_TRACE(NEXUS_NOT_SUPPORTED);
    BSTD_UNUSED(&NEXUS_ParserBand_P_Close);
}

void NEXUS_ParserBand_P_SetEnable(NEXUS_ParserBandHandle parserBand)
{
    BSTD_UNUSED(parserBand);
    BDBG_WRN(("Parser Band not enabled on this chipset"));
    return;
}

void NEXUS_ParserBand_GetMtsifConnections_priv( NEXUS_FrontendConnectorHandle connector, struct NEXUS_MtsifParserBandSettings *pSettings, unsigned numEntries, unsigned *pNumReturned)
{
    BSTD_UNUSED(connector);
    BSTD_UNUSED(pSettings);
    BSTD_UNUSED(numEntries);
    BSTD_UNUSED(pNumReturned);
}

void NEXUS_ParserBand_P_MtsifErrorStatus_priv(unsigned lengthError, unsigned transportError)
{
    BSTD_UNUSED(lengthError);
    BSTD_UNUSED(transportError);
}

#endif /* NEXUS_NUM_PARSER_BANDS */

void NEXUS_GetParserBandMapping( NEXUS_ParserBandMapping *pParserMap )
{
#if BXPT_HAS_PARSER_REMAPPING
    unsigned i;
    BXPT_ParserBandMapping xptMap;

    BKNI_Memset(pParserMap, 0, sizeof(*pParserMap));
    BXPT_GetParserMapping(pTransport->xpt, &xptMap);

#if NEXUS_NUM_PARSER_BANDS
    /* the number of bands may be configured differently */
    for (i=0;i<NEXUS_NUM_PARSER_BANDS && i<BXPT_NUM_PID_PARSERS;i++) {
        pParserMap->frontend[i].virtualParserBandNum = xptMap.FrontEnd[i].VirtualParserBandNum;
        pParserMap->frontend[i].virtualParserIsPlayback = xptMap.FrontEnd[i].VirtualParserIsPlayback;
    }
#endif
#if NEXUS_NUM_PLAYPUMPS
    for (i=0;i<NEXUS_NUM_PLAYPUMPS && i<BXPT_NUM_PLAYBACKS;i++) {
        pParserMap->playback[i].virtualParserBandNum = xptMap.Playback[i].VirtualParserBandNum;
        pParserMap->playback[i].virtualParserIsPlayback = xptMap.Playback[i].VirtualParserIsPlayback;
    }
#endif
#else
    BKNI_Memset(pParserMap, 0, sizeof(*pParserMap));
    BERR_TRACE(NEXUS_NOT_SUPPORTED);
#endif
}

NEXUS_Error NEXUS_SetParserBandMapping( const NEXUS_ParserBandMapping *pParserMap )
{
#if BXPT_HAS_PARSER_REMAPPING
    /* TODO: the meaning of playback remapping in nexus has not been worked out. */
    unsigned i;
    BXPT_ParserBandMapping xptMap;
    BERR_Code rc;

    BXPT_GetParserMapping(pTransport->xpt, &xptMap);

    /* the number of bands may be configured differently */
#if NEXUS_NUM_PARSER_BANDS
    for (i=0;i<NEXUS_NUM_PARSER_BANDS && i<BXPT_NUM_PID_PARSERS;i++) {
        xptMap.FrontEnd[i].VirtualParserBandNum = pParserMap->frontend[i].virtualParserBandNum;
        xptMap.FrontEnd[i].VirtualParserIsPlayback = pParserMap->frontend[i].virtualParserIsPlayback;
    }
#endif
#if NEXUS_NUM_PLAYPUMPS
    for (i=0;i<NEXUS_NUM_PLAYPUMPS && i<BXPT_NUM_PLAYBACKS;i++) {
        xptMap.Playback[i].VirtualParserBandNum = pParserMap->playback[i].virtualParserBandNum;
        xptMap.Playback[i].VirtualParserIsPlayback = pParserMap->playback[i].virtualParserIsPlayback;
    }
#endif

    rc = BXPT_SetParserMapping(pTransport->xpt, &xptMap);
    if (rc) return BERR_TRACE(rc);

    return 0;
#else
    BSTD_UNUSED(pParserMap);
    return BERR_TRACE(NEXUS_NOT_SUPPORTED);
#endif
}

NEXUS_Error NEXUS_ParserBand_GetAllPassPidChannelIndex(
    NEXUS_ParserBand parserBand,
    unsigned *pHwPidChannel
    )
{
#if NEXUS_NUM_PARSER_BANDS
    NEXUS_ParserBandHandle handle = NULL;

    handle = NEXUS_ParserBand_P_ResolveAcquire(parserBand, false);
    if (handle)
    {
        /* On the front-end parsers, the PID channel number is the parser band number. There's little risk that it'll change. */
        *pHwPidChannel = BXPT_GET_IB_PARSER_ALLPASS_CHNL( handle->hwIndex );
    }
    else
    {
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }

    return 0;
#else
    BSTD_UNUSED(parserBand);
    BSTD_UNUSED(pHwPidChannel);
    return BERR_TRACE(NEXUS_NOT_SUPPORTED);
#endif
}
