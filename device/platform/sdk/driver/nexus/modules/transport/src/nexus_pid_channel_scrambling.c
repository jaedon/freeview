/***************************************************************************
 *     (c)2007-2011 Broadcom Corporation
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
 * $brcm_Workfile: nexus_pid_channel_scrambling.c $
 * $brcm_Revision: 14 $
 * $brcm_Date: 4/19/11 2:14p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/transport/7400/src/nexus_pid_channel_scrambling.c $
 * 
 * 14   4/19/11 2:14p erickson
 * SW7335-1168: rework NEXUS_PidChannelScramblingSettings.raveContext to
 *  require it be already started by decoder
 * 
 * 13   4/8/11 3:25p erickson
 * SW7335-1168: fix if pSettings == NULL
 * 
 * 12   3/1/11 10:21a erickson
 * SW7335-1168: add optional
 *  NEXUS_PidChannelScramblingSettings.raveContext
 *
 * 11   2/10/11 12:36p erickson
 * SW7405-5108: added NEXUS_PidChannel_ClearScramblingStatus
 *
 * 10   1/7/10 9:18a erickson
 * SW7550-176: increase size of CDB/ITB for scrambling check. this is
 *  needed for some chips.
 *
 * 9   9/25/09 1:07p erickson
 * SW7405-2903: refactor scrambling check API now that SC_OR_MODE is
 *  enabled
 *
 * 8   8/20/09 9:29a erickson
 * PR57851: hardcode bandHold false on the scrambling check rave context
 *
 * 7   8/6/09 9:43a ahulse
 * PR57125: Add debug message to show which pid is being scramble check
 *  start, stopped
 *
 * 6   8/4/09 11:06a ahulse
 * PR57125: Make sure scramble bits are reset on scrmable check open
 *
 * 5   4/29/09 9:05a erickson
 * PR54619: use #if BXPT_HAS_RAVE_SCRAMBLING_CONTROL
 *
 * 4   7/10/08 4:28p erickson
 * PR44699: enable NEXUS_PidChannel_GetScramblingStatus for 3548 and 3556
 *
 * 3   4/11/08 9:26a erickson
 * PR40079: merge
 *
 * PR40079/1   4/11/08 2:30p rzheng
 * PR40079: implement NEXUS_PidChannel_GetScramblingStatus
 *
 * 2   4/10/08 2:04p erickson
 * PR40079: fix
 *
 * 1   4/10/08 1:58p erickson
 * PR40079: added PidChannel ScramblingCheck
 *
 **************************************************************************/
#include "nexus_transport_module.h"

BDBG_MODULE(nexus_pid_channel_scrambling);

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD) && !defined(CONFIG_JAPAN_SPEC)
#define NEXUS_SCRAMBLINGCHECK_DUMMYREAD_TIMER 10
static void NEXUS_PidChannel_P_DummyReadTimer(void *context);
#endif

void NEXUS_PidChannel_GetDefaultScramblingSettings( NEXUS_PidChannelScramblingSettings *pSettings )
{
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
}

NEXUS_Error NEXUS_PidChannel_StartScramblingCheck( NEXUS_PidChannelHandle pidChannel, const NEXUS_PidChannelScramblingSettings *pSettings )
{
    BDBG_OBJECT_ASSERT(pidChannel, NEXUS_PidChannel);

    if (pidChannel->scramblingCheck.rave) {
        NEXUS_PidChannel_StopScramblingCheck(pidChannel);
    }

    if (pSettings) {
        pidChannel->scramblingCheck.settings = *pSettings;
    }
    else {
        NEXUS_PidChannel_GetDefaultScramblingSettings(&pidChannel->scramblingCheck.settings);
    }

    if ( pidChannel->scramblingCheck.settings.raveContext) {
        if (!pidChannel->scramblingCheck.settings.raveContext->enabled) {
            BDBG_ERR(("external RAVE context must be already started before calling NEXUS_PidChannel_StartScramblingCheck"));
            return BERR_TRACE(NEXUS_NOT_AVAILABLE);
        }
        pidChannel->scramblingCheck.rave = pidChannel->scramblingCheck.settings.raveContext;
    }
    else {
        NEXUS_Error rc;
        NEXUS_RaveSettings raveSettings;
        NEXUS_RaveOpenSettings raveOpenSettings;
        
        /* the nexus code does not try to evacuate the rave buffer. scrambling check is done before any overflow check,
        so it's ok if the rave buffer overflows. this is the minimum amount of memory for rave to put data into a buffer. */
        NEXUS_Rave_GetDefaultOpenSettings_priv(&raveOpenSettings);
        raveOpenSettings.config.Cdb.Length = 4096*2; /* minimum value for XPT PI threshold calculations */
        raveOpenSettings.config.Cdb.Alignment = 4;
        raveOpenSettings.config.Cdb.LittleEndian = true;
        raveOpenSettings.config.Itb.Length = 4096*2;
        raveOpenSettings.config.Itb.Alignment = 4;
        raveOpenSettings.config.Itb.LittleEndian = true;

        pidChannel->scramblingCheck.rave = NEXUS_Rave_Open_priv(&raveOpenSettings);
        if (!pidChannel->scramblingCheck.rave) return BERR_TRACE(NEXUS_UNKNOWN);

        NEXUS_Rave_GetDefaultSettings_priv(&raveSettings);
        raveSettings.pidChannel = pidChannel;
        raveSettings.bandHold = false; /* bandHold doesn't matter for live. but for playback, there's no value in pushing back on playback. so we hardcode to false. */
        rc = NEXUS_Rave_ConfigureAll_priv(pidChannel->scramblingCheck.rave, &raveSettings);
        if (rc) {
            if (!pidChannel->scramblingCheck.settings.raveContext) {
                NEXUS_Rave_Close_priv(pidChannel->scramblingCheck.rave);
            }
            pidChannel->scramblingCheck.rave = NULL;
            return BERR_TRACE(rc);
        }
    
        NEXUS_Rave_Enable_priv(pidChannel->scramblingCheck.rave);
    }

    BDBG_MSG(("Start Scrambling Check for pid=0x%2x ", pidChannel->status.pid ));
    BXPT_Rave_ClearSCRegister(pidChannel->scramblingCheck.rave->raveHandle);

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD) && !defined(CONFIG_JAPAN_SPEC)
    pidChannel->scramblingCheck.dummyreadtimer = NEXUS_ScheduleTimer(NEXUS_SCRAMBLINGCHECK_DUMMYREAD_TIMER, NEXUS_PidChannel_P_DummyReadTimer, pidChannel);
#endif

    return 0;
}

void NEXUS_PidChannel_StopScramblingCheck( NEXUS_PidChannelHandle pidChannel )
{
    BDBG_OBJECT_ASSERT(pidChannel, NEXUS_PidChannel);
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD) && !defined(CONFIG_JAPAN_SPEC)
    if (pidChannel->scramblingCheck.dummyreadtimer) {
        NEXUS_CancelTimer(pidChannel->scramblingCheck.dummyreadtimer);
        pidChannel->scramblingCheck.dummyreadtimer = NULL;
    }
#endif
    if (pidChannel->scramblingCheck.rave) {
        BDBG_MSG(("Stop Scrambling Check for pid=0x%2x ", pidChannel->status.pid ));
        if (!pidChannel->scramblingCheck.settings.raveContext) {
            NEXUS_Rave_Close_priv(pidChannel->scramblingCheck.rave);
        }
        pidChannel->scramblingCheck.rave = NULL;
    }
}

NEXUS_Error NEXUS_PidChannel_GetScramblingStatus( NEXUS_PidChannelHandle pidChannel, NEXUS_PidChannelScramblingStatus *pStatus )
{
    unsigned avScramble = 0;
    BXPT_Rave_ScrambleCtrl scrambleCtrl;
    NEXUS_Error rc;

    BDBG_OBJECT_ASSERT(pidChannel, NEXUS_PidChannel);

    BKNI_Memset(pStatus, 0, sizeof(*pStatus));

    if (!pidChannel->scramblingCheck.rave) {
        /* must start first */
        return BERR_TRACE(NEXUS_NOT_AVAILABLE);
    }

    rc = BXPT_Rave_GetScramblingCtrl(pidChannel->scramblingCheck.rave->raveHandle, &scrambleCtrl);
    if (rc) return BERR_TRACE(rc);

    if (pidChannel->scramblingCheck.settings.pusiOnly) {
        if (scrambleCtrl.PusiValid) {
            avScramble = scrambleCtrl.Pusi;
            pStatus->pidExists = true;
        }
        else {
            pStatus->pidExists = false;
        }
    }
    else {
        if (scrambleCtrl.AllValid) {
            avScramble = scrambleCtrl.ScAll;
            pStatus->pidExists = true;
        }
        else {
            pStatus->pidExists = false;
        }
    }

    if (pStatus->pidExists) {
        /* by default, XPT PI programs RAVE HW to have sticky state (aka SC_OR_MODE), so we don't have to remember */
        pStatus->scrambled = avScramble;
    }

    return 0;
}

void NEXUS_PidChannel_ClearScramblingStatus( NEXUS_PidChannelHandle pidChannel )
{
    BDBG_OBJECT_ASSERT(pidChannel, NEXUS_PidChannel);
    if (pidChannel->scramblingCheck.rave) {
        BXPT_Rave_ClearSCRegister(pidChannel->scramblingCheck.rave->raveHandle);
    }
}

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD) && !defined(CONFIG_JAPAN_SPEC)
static void NEXUS_PidChannel_P_DummyReadTimer(void *context)
{
    NEXUS_PidChannelHandle pidChannel = context;
    BXPT_Rave_ContextPtrs ptr;

    pidChannel->scramblingCheck.dummyreadtimer = NULL;

    if (pidChannel->scramblingCheck.rave) {
        BXPT_Rave_CheckBuffer(pidChannel->scramblingCheck.rave->raveHandle, &ptr);
        BXPT_Rave_UpdateReadOffset(pidChannel->scramblingCheck.rave->raveHandle, ptr.Cdb.ByteCount + ptr.Cdb.WrapByteCount, ptr.Itb.ByteCount + ptr.Itb.WrapByteCount);
        pidChannel->scramblingCheck.dummyreadtimer = NEXUS_ScheduleTimer(NEXUS_SCRAMBLINGCHECK_DUMMYREAD_TIMER, NEXUS_PidChannel_P_DummyReadTimer, pidChannel);
    }
}
#else
static void NEXUS_PidChannel_P_DummyReadTimer(void *context)
{
	return;
}
#endif
