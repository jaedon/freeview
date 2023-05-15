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
 * $brcm_Workfile: nexus_remux.c $
 * $brcm_Revision: 43 $
 * $brcm_Date: 9/13/12 12:14p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/transport/src/nexus_remux.c $
 * 
 * 43   9/13/12 12:14p erickson
 * SW7346-1024: use BXPT_RemuxClock_eIbMax
 * 
 * 42   9/13/12 9:55a erickson
 * SW7346-1024: increase max BXPT_RemuxClock_eIb
 * 
 * 41   6/28/12 3:06p jtna
 * SW7468-405: fix NEXUS_OBJECT conversion for NEXUS_NUM_REMUX==0 case
 * 
 * 40   6/19/12 12:44p vsilyaev
 * SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
 * 
 * 39   10/31/11 7:48p bandrews
 * SW7231-391: merge to main
 * 
 * SW7420-2078/1   10/25/11 5:22p bandrews
 * SW7231-391: update parser band and timebase implementations to use
 *  handles everywhere, even for legacy enum usage
 * 
 * 38   8/4/11 9:42a erickson
 * SW7346-394: fix chips w/o remux
 * 
 * 37   8/3/11 2:15p erickson
 * SW7346-394: call BXPT_DirecTvRemux_SetMode based on transportType
 * 
 * 36   2/9/11 2:18p erickson
 * SW7420-1009: support NEXUS_ANY_ID
 *
 * 35   1/26/11 3:40p jrubio
 * SW7420-1237: fix warning
 *
 * 34   1/24/11 9:01a erickson
 * SW7420-1237: fix warning for DTV
 *
 * 33   1/20/11 2:35p erickson
 * SW7420-1237: support live and playback input to remux, allow per-parser
 *  band input maxDataRate
 *
 * 32   7/23/10 4:44p jgarrett
 * SW7420-902: Adding NULL packet insertion and SetSettings on-the-fly.
 *
 * 31   7/2/10 11:00a erickson
 * SW7550-448: wrap BXPT_Remux_SetRSXCDataRate with chip-generic #if
 *
 * 30   6/30/10 1:54p erickson
 * SW7405-3940: fix NEXUS_Remux_RemoveAllPidChannels
 *
 * 29   5/11/10 10:20a vishk
 * SW7405-3940: re-code nexus_remux.c to use internal linked list instead
 *  of an array of size NEXUS_MAX_REMUX_PID_CHANNELS.  Fixed memory leaks.
 *
 * 28   5/10/10 8:41p vishk
 * SW7405-3940: re-code nexus_remux.c to use internal linked list instead
 *  of an array of size NEXUS_MAX_REMUX_PID_CHANNELS
 *
 * 27   2/24/10 10:07a gmullen
 * SW3548-2790: BXPT_Remux_SetPcrJitterAdj now returns a void
 *
 * 26   2/24/10 9:36a mward
 * SW3548-2790: typo.
 *
 * 25   2/23/10 5:42p erickson
 * SW3548-2790: added NEXUS_RemuxSettings.enablePcrJitterAdjust and
 *  .playback
 *
 * 24   12/14/09 5:02p jrubio
 * SW7342-11: fix 7340 remux
 *
 * 23   12/14/09 11:33a jrubio
 * SW7342-11: add Remux for 7340/7342
 *
 * 22   10/20/09 5:56p jhaberf
 * SW35230-1: BXPT_Remux_SetRSXCDataRate() is not required for the 35230.
 *  #ifdef'd out calls to this function.
 *
 * 21   8/20/09 4:29p mward
 * PR55545: Support 7125.
 *
 * 20   6/12/09 3:06p jtna
 * PR55971: register remux as pidchannel consumer
 *
 * 19   5/22/09 8:53a erickson
 * PR55402: fix playback parser band support for remux
 *
 * 18   4/20/09 10:21a erickson
 * PR53728: apply maxDataRate to playback or input band correctly
 *
 * 17   4/17/09 12:31p erickson
 * PR53728: added NEXUS_RemuxSettings.maxDataRate
 *
 * 16   3/28/09 10:03a erickson
 * PR53417: added BXPT_Remux_AddPcrOffset for new remux systems. added
 *  remux pacing support.
 *
 * 15   3/25/09 3:32p erickson
 * PR53417: fix 7420
 *
 * 14   3/24/09 9:43a erickson
 * PR53417: remove allPass setting for some chips. not needed.
 *
 * 13   3/23/09 5:05p erickson
 * PR53417: temp fix for some chips with different remux api's
 *
 * 12   3/23/09 2:46p erickson
 * PR53417: add NEXUS_RemuxSettings.bypass. Swap bypass and allPass.
 *  Clarify usage comments. add NEXUS_RemuxSettings.pcrCorrectionEnabled
 *  and pcrCorrectionOffset.
 *
 * 11   12/17/08 1:57p katrep
 * PR49873: Add support for 7466
 *
 * 10   9/3/08 7:36p vishk
 * PR 46315: Sample code for PID remapping
 *
 * 9   9/2/08 3:48p vishk
 * PR39435 : Add remux capability to Nexus
 *
 * 8   9/2/08 11:45a erickson
 * PR46315: added NEXUS_RemuxSettings.allPass
 *
 * 7   8/14/08 5:26p katrep
 * PR45674: Fix compiiler warning in kernel mode non debug builds
 *
 * 6   4/11/08 9:53a erickson
 * PR41246: convert BDBG_OBJECT_UNSET to BDBG_OBJECT_DESTROY if freeing
 *  memory
 *
 * 5   4/1/08 4:41p erickson
 * PR41152: fix bounds check
 *
 * 4   3/31/08 12:33p erickson
 * PR41073: check result of malloc and fail graciously
 *
 * 3   3/17/08 11:27a erickson
 * PR40350: convert ERR to MSG
 *
 * PR39899/1   3/14/08 8:09p shyam
 * PR 40350 : Remove non error DBG_ERR messages
 *
 * 2   2/22/08 7:22p shyam
 * PR39435: Add remux capability to Nexus
 *
 *****************************************************************************/

#include "nexus_transport_module.h"
BDBG_MODULE(nexus_remux);

#if NEXUS_NUM_REMUX
#if B_HAS_DSS
#include "bxpt_directv_remux.h"
#endif

static NEXUS_Error NEXUS_Remux_P_Start(NEXUS_RemuxHandle handle);
static void NEXUS_Remux_P_Stop(NEXUS_RemuxHandle handle);

void NEXUS_Remux_GetDefaultSettings( NEXUS_RemuxSettings *pSettings )
{
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    pSettings->insertNullPackets = true;
}

/* local storage for instance management */
static NEXUS_RemuxHandle g_remux[NEXUS_NUM_REMUX];

NEXUS_RemuxHandle NEXUS_Remux_Open( unsigned index, const NEXUS_RemuxSettings *pSettings )
{
    BERR_Code rc;
    NEXUS_RemuxSettings settings;
    NEXUS_RemuxHandle rmx;
    unsigned numRmxChannels;
    BXPT_Remux_ChannelSettings rmxChannelSettings;

    if (!pSettings) {
        NEXUS_Remux_GetDefaultSettings(&settings);
        pSettings = &settings;
    }

    if (index == NEXUS_ANY_ID) {
        for (index=0;index<NEXUS_NUM_REMUX;index++) {
            if (!g_remux[index]) break;
        }
        if (index == NEXUS_NUM_REMUX) {
            rc = BERR_TRACE(NEXUS_NOT_AVAILABLE);
            return NULL;
        }
    }

    if (index >= NEXUS_NUM_REMUX) {
        rc = BERR_TRACE(NEXUS_INVALID_PARAMETER);
        return NULL;
    }

    rmx = (NEXUS_RemuxHandle)BKNI_Malloc(sizeof(*rmx));
    if (!rmx) {
        rc=BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);
        return NULL;
    }
    NEXUS_OBJECT_INIT(NEXUS_Remux, rmx);
    rmx->settings = *pSettings;

    /* Open the Remux Transport block */
    rc = BXPT_Remux_GetTotalChannels(pTransport->xpt, &numRmxChannels);
    if(index >= numRmxChannels)
    {
        rc=BERR_TRACE(NEXUS_INVALID_PARAMETER);
        goto error;
    }

    rc = BXPT_Remux_GetChannelDefaultSettings(pTransport->xpt, index, &rmxChannelSettings);
    if (rc != BERR_SUCCESS) { rc = BERR_TRACE(rc); goto error; }

    /* Map nexus params to XPT params */
    if( pSettings->outputClock <= NEXUS_RemuxClock_e27Mhz_VCXO_A) {
        rmxChannelSettings.OutputClock = pSettings->outputClock;
    }
    else {
        rmxChannelSettings.OutputClock = pSettings->remuxClockIBSrc+BXPT_RemuxClock_eIb0;
    }
    if (rmxChannelSettings.OutputClock > BXPT_RemuxClock_eIbMax) {
        rc = BERR_TRACE(NEXUS_INVALID_PARAMETER);
        goto error;
    }
    rmxChannelSettings.ParallelEn = pSettings->parallelOutput;
    rmxChannelSettings.InvertClk = pSettings->invertClock;
    rmxChannelSettings.InvertSync = pSettings->invertSync;
    rmxChannelSettings.ByteSync = pSettings->byteSync;
    rmxChannelSettings.InsertNullPackets = pSettings->insertNullPackets;
    rc = BXPT_Remux_OpenChannel(pTransport->xpt, &rmx->xptRmx, index, &rmxChannelSettings);
    if (rc != BERR_SUCCESS) { rc = BERR_TRACE(rc); goto error; }

    rmx->index = index;

#if BXPT_HAS_REMUX_PCR_OFFSET
    rc = BXPT_Remux_SetBypassMode(rmx->xptRmx, pSettings->bypass);
    if (rc != BERR_SUCCESS) { rc = BERR_TRACE(rc); goto error; }

    /* No allPass setting required on this chip. Just attach an allPass pid channel and all data will flow out. */

    rc = BXPT_Remux_AddPcrOffset(rmx->xptRmx, pSettings->pcrCorrectionEnabled, pSettings->pcrCorrectionOffset);
    if (rc != BERR_SUCCESS) { rc = BERR_TRACE(rc); goto error; }

     BXPT_Remux_SetPcrJitterAdj(rmx->xptRmx, true, pSettings->enablePcrJitterAdjust);
#else
    rc = BXPT_Remux_SetBypassMode(rmx->xptRmx, pSettings->bypass);
    if (rc != BERR_SUCCESS) { rc = BERR_TRACE(rc); goto error; }

    rc = BXPT_Remux_SetAllPassMode(rmx->xptRmx, pSettings->allPass);
    if (rc != BERR_SUCCESS) { rc = BERR_TRACE(rc); goto error; }

    rc = BXPT_Remux_SetPcrCorrection(rmx->xptRmx, pSettings->pcrCorrectionEnabled, pSettings->pcrCorrectionOffset);
    if (rc != BERR_SUCCESS) { rc = BERR_TRACE(rc); goto error; }

    rc = BXPT_Remux_ConfigPacing(rmx->xptRmx, pSettings->pacing?BXPT_PacingControl_eStart:BXPT_PacingControl_eStop);
    if (rc != BERR_SUCCESS) { rc = BERR_TRACE(rc); goto error; }

    rc = BXPT_Remux_SetPacingErrorBound(rmx->xptRmx, pSettings->pacingMaxError);
    if (rc != BERR_SUCCESS) { rc = BERR_TRACE(rc); goto error; }
#endif

    BLST_S_INIT(&rmx->pid_list);

    g_remux[index] = rmx;
    return rmx;

error:
    if (rmx->xptRmx) {
        BXPT_Remux_CloseChannel(rmx->xptRmx);
    }
    BKNI_Free(rmx);
    return NULL;
}

static void NEXUS_Remux_P_Finalizer( NEXUS_RemuxHandle handle )
{
    NEXUS_OBJECT_ASSERT(NEXUS_Remux, handle);
    NEXUS_Remux_RemoveAllPidChannels(handle);
    BXPT_Remux_CloseChannel(handle->xptRmx);

    g_remux[handle->index] = NULL;
    NEXUS_OBJECT_DESTROY(NEXUS_Remux, handle);
    BKNI_Free(handle);
    return;
}

NEXUS_OBJECT_CLASS_MAKE(NEXUS_Remux, NEXUS_Remux_Close);


void NEXUS_Remux_GetDefaultParserBandwidth(NEXUS_RemuxParserBandwidth *pSettings)
{
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    pSettings->parserBand = NEXUS_ParserBand_eInvalid; /* force client to set, unless playback */
    pSettings->maxDataRate = 25000000; /* 25Mbps*/
}

NEXUS_Error NEXUS_Remux_SetParserBandwidth( NEXUS_RemuxHandle handle,const NEXUS_RemuxParserBandwidth *pSettings)
{
    BERR_Code rc=BERR_SUCCESS;
#if BXPT_HAS_RSBUF && BXPT_HAS_XCBUF
    unsigned index,parserNum;
    BXPT_ParserType parserType;
    NEXUS_TransportType transportType;
#endif

    BDBG_OBJECT_ASSERT(handle, NEXUS_Remux);
    BDBG_ASSERT(pSettings);
#if BXPT_HAS_RSBUF && BXPT_HAS_XCBUF
    if (pSettings->maxDataRate){
        if(pSettings->playpump){
            NEXUS_PlaypumpSettings playpumpSettings;
            NEXUS_PlaypumpStatus playpumpStatus;
            NEXUS_Playpump_GetSettings(pSettings->playpump,&playpumpSettings);
            NEXUS_Playpump_GetStatus(pSettings->playpump,&playpumpStatus);
            index = playpumpStatus.index + NEXUS_NUM_PARSER_BANDS;
            parserNum = playpumpStatus.index;
            parserType = BXPT_ParserType_ePb;
            transportType = playpumpSettings.transportType;
        }
        else{
            NEXUS_ParserBandHandle parserBand;
            NEXUS_ParserBandSettings parserBandSettings;
            parserType = BXPT_ParserType_eIb;
            parserBand = NEXUS_ParserBand_Resolve_priv(pSettings->parserBand);
            if (!parserBand) return BERR_TRACE(NEXUS_INVALID_PARAMETER);
            parserNum = index = parserBand->hwIndex;
            NEXUS_ParserBand_P_GetSettings(parserBand, &parserBandSettings);
            transportType = parserBandSettings.transportType;
        }

        BKNI_Memcpy(&handle->remuxParserBandwidth[index],pSettings,sizeof(*pSettings));

        rc = BXPT_Remux_SetRSXCDataRate( handle->xptRmx,parserNum,parserType,pSettings->maxDataRate ,
                                     NEXUS_IS_DSS_MODE(transportType)?130:188);
        if (rc) return BERR_TRACE(rc);
    }
#else
     /* Do not call BXPT_Remux_SetRSXCDataRate() for chips that do not have RS and XC buffers */
#endif
    handle->parserBandCount++;
    if(handle->parserBandCount > 2){
        BDBG_WRN(("********* Please consult your FAE for bandwidth requirement if number of parser bands added to the remux is greater than 2 *************"));
    }
    return rc;
}


NEXUS_Error NEXUS_Remux_AddPidChannel( NEXUS_RemuxHandle handle, NEXUS_PidChannelHandle pidChannel )
{
    BERR_Code rc;
    NEXUS_Remux_P_PidChannel *pid;
    unsigned parserIndex;
    bool firstPid;
    
    BDBG_OBJECT_ASSERT(handle, NEXUS_Remux);
    BDBG_OBJECT_ASSERT(pidChannel, NEXUS_PidChannel);
    
    pid = BLST_S_FIRST(&handle->pid_list);
    firstPid = !pid;
    if (!firstPid) {
        /* all pid channels must match on certain attributes */
        if (pid->pidChn->status.transportType != pidChannel->status.transportType) {
            return BERR_TRACE(NEXUS_INVALID_PARAMETER);
        }
    }    

    pid = BKNI_Malloc(sizeof(NEXUS_Remux_P_PidChannel));
    if ( NULL == pid )
    {
       return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
    }
    pid->pidChn = pidChannel;
    
    BLST_S_DICT_ADD(&handle->pid_list, pid, NEXUS_Remux_P_PidChannel, pidChn, link, err_duplicate);

#if B_HAS_DSS
    if (firstPid) {
        rc = BXPT_DirecTvRemux_SetMode(handle->xptRmx, NEXUS_IS_DSS_MODE(pidChannel->status.transportType)?BXPT_RemuxMode_eDirecTv:BXPT_RemuxMode_eMpeg);
        if (rc) { rc = BERR_TRACE(rc); goto err_setmode;}
    }
#endif

    rc = BXPT_Remux_AddPidChannelToRemux( handle->xptRmx, handle->index==0?BXPT_RemuxInput_eA:BXPT_RemuxInput_eB, pidChannel->status.pidChannelIndex);
    if (rc) { rc = BERR_TRACE(rc); goto err_addpid;}

    parserIndex = pidChannel->status.playback?(pidChannel->status.playbackIndex + NEXUS_NUM_PARSER_BANDS):pidChannel->parserBand->hwIndex;

    handle->pidChannelCount[parserIndex]++;

    if (handle->started) {
        NEXUS_PidChannel_ConsumerStarted(pidChannel);
    }

    return 0;

err_addpid:
#if B_HAS_DSS
err_setmode:
#endif
    BLST_S_DICT_REMOVE(&handle->pid_list, pid, pidChannel, NEXUS_Remux_P_PidChannel, pidChn, link);
    BKNI_Free(pid);
    return rc;
    
err_duplicate:
    BKNI_Free(pid);
    return BERR_TRACE(NEXUS_INVALID_PARAMETER);
}

NEXUS_Error NEXUS_Remux_RemovePidChannel( NEXUS_RemuxHandle handle, NEXUS_PidChannelHandle pidChannel )
{
    BERR_Code rc;
    NEXUS_Remux_P_PidChannel *pid;
    unsigned parserIndex;

    BDBG_OBJECT_ASSERT(handle, NEXUS_Remux);
    BDBG_OBJECT_ASSERT(pidChannel, NEXUS_PidChannel);

    parserIndex = pidChannel->status.playback?(pidChannel->status.playbackIndex + NEXUS_NUM_PARSER_BANDS):pidChannel->parserBand->hwIndex;

    BLST_S_DICT_REMOVE(&handle->pid_list, pid, pidChannel, NEXUS_Remux_P_PidChannel, pidChn, link);
    if(!pid) { rc = BERR_TRACE(NEXUS_INVALID_PARAMETER); goto err_pid;}
    BKNI_Free(pid);

    BDBG_ASSERT(handle->pidChannelCount[parserIndex]);
    handle->pidChannelCount[parserIndex]--;

    rc = BXPT_Remux_RemovePidChannelFromRemux( handle->xptRmx, handle->index==0?BXPT_RemuxInput_eA:BXPT_RemuxInput_eB, pidChannel->status.pidChannelIndex);
    if (rc) return BERR_TRACE(rc);

    if (!handle->pidChannelCount[parserIndex]) {
        handle->parserBandCount--;
#if BXPT_HAS_RSBUF && BXPT_HAS_XCBUF
        if(handle->remuxParserBandwidth[parserIndex].maxDataRate){
            unsigned parserNum;
            BXPT_ParserType parserType;
            NEXUS_TransportType transportType;

            if(pidChannel->status.playback){
               parserNum = pidChannel->status.playbackIndex;
               parserType = BXPT_ParserType_ePb;
            }
            else{
                parserNum = (unsigned)pidChannel->parserBand->hwIndex;
                parserType = BXPT_ParserType_eIb;
            }
            transportType = pidChannel->status.transportType;
            rc = BXPT_Remux_SetRSXCDataRate( handle->xptRmx,parserType, parserNum,25000000,NEXUS_IS_DSS_MODE(transportType)?130:188);
        if (rc) {rc = BERR_TRACE(rc);} /* fall through */

    }
#else
    /* Do not call BXPT_Remux_SetRSXCDataRate() for chips that do not have RS and XC buffers */
#endif
    }
    return 0;
err_pid:
    return rc;
}

void NEXUS_Remux_RemoveAllPidChannels( NEXUS_RemuxHandle handle )
{
    BERR_Code rc;
    NEXUS_Remux_P_PidChannel *pid;
#if BXPT_HAS_RSBUF && BXPT_HAS_XCBUF
    unsigned i,parserNum;
    BXPT_ParserType parserType;
    NEXUS_TransportType transportType;
#endif

    BDBG_OBJECT_ASSERT(handle, NEXUS_Remux);

    while(NULL!=(pid=BLST_S_FIRST(&handle->pid_list))) {
        NEXUS_Error rc;
        rc = NEXUS_Remux_RemovePidChannel(handle, pid->pidChn);
        if (rc) break; /* avoid infinite loop */
    }

    handle->parserBandCount=0;
    BKNI_Memset(handle->pidChannelCount,0,sizeof(handle->pidChannelCount));

#if BXPT_HAS_RSBUF && BXPT_HAS_XCBUF
    for(i=0;i<NEXUS_NUM_REMUX_PARSER_INPUTS;i++){
        if(handle->remuxParserBandwidth[i].maxDataRate){
            if(handle->remuxParserBandwidth[i].playpump){
                NEXUS_PlaypumpSettings playpumpSettings;
                NEXUS_PlaypumpStatus playpumpStatus;
                NEXUS_Playpump_GetSettings(handle->remuxParserBandwidth[i].playpump,&playpumpSettings);
                NEXUS_Playpump_GetStatus(handle->remuxParserBandwidth[i].playpump,&playpumpStatus);
                parserNum = playpumpStatus.index;
                parserType = BXPT_ParserType_ePb;
                transportType = playpumpSettings.transportType;
            }
            else{
                NEXUS_ParserBandHandle parserBand;
                NEXUS_ParserBandSettings parserBandSettings;

                parserType = BXPT_ParserType_eIb;
                parserBand = NEXUS_ParserBand_Resolve_priv(handle->remuxParserBandwidth[i].parserBand);
                if (!parserBand) return;/* can't do anything useful if this happens */

                parserNum = parserBand->hwIndex;
                NEXUS_ParserBand_P_GetSettings(parserBand, &parserBandSettings);
                transportType = parserBandSettings.transportType;
            }

            rc = BXPT_Remux_SetRSXCDataRate( handle->xptRmx,parserType, parserNum, 25000000,NEXUS_IS_DSS_MODE(transportType)?130:188);
            if (rc) {rc = BERR_TRACE(rc);} /* fall through */
            BKNI_Memset(&handle->remuxParserBandwidth[i],0,sizeof(handle->remuxParserBandwidth[i]));
        }
    }
#else
    BSTD_UNUSED(rc);
    /* Do not call BXPT_Remux_SetRSXCDataRate() for chips that do not have RS and XC buffers */
#endif
   return;
}

static NEXUS_Error NEXUS_Remux_P_Start(NEXUS_RemuxHandle handle)
{
    BERR_Code rc;
    NEXUS_Remux_P_PidChannel *pid;

    BDBG_OBJECT_ASSERT(handle, NEXUS_Remux);

    if (handle->started) {
        BDBG_ERR(("Remux already started"));
        return BERR_TRACE(NEXUS_NOT_SUPPORTED);
    }

    BDBG_MSG(("NEXUS_Remux_P_Start %p", handle));
    rc = BXPT_Remux_DoRemux(handle->xptRmx, true);
    if(rc != BERR_SUCCESS) {
        return BERR_TRACE(rc);
    }

    for ( pid = BLST_S_FIRST(&handle->pid_list); pid; pid = BLST_S_NEXT(pid, link) )
    {
        NEXUS_PidChannel_ConsumerStarted(pid->pidChn);
    }

    handle->started = true;
    return NEXUS_SUCCESS;
}

NEXUS_Error NEXUS_Remux_Start(NEXUS_RemuxHandle handle)
{
    NEXUS_Error rc;
    rc = NEXUS_Remux_P_Start(handle);
    if(rc!=NEXUS_SUCCESS) {
        return BERR_TRACE(rc);
    }
    return NEXUS_SUCCESS;
}

static void NEXUS_Remux_P_Stop( NEXUS_RemuxHandle handle )
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_Remux);
    BDBG_MSG(("NEXUS_Remux_Stop %p", handle));
    if (handle->started) {
        BERR_Code rc;
        rc = BXPT_Remux_DoRemux(handle->xptRmx, false);
        if(rc != BERR_SUCCESS) {rc = BERR_TRACE(rc);}
        handle->started = false;
    }
}

void NEXUS_Remux_Stop( NEXUS_RemuxHandle handle )
{
    NEXUS_Remux_P_Stop(handle);
    return;
}

NEXUS_Error NEXUS_Remux_SetSettings(NEXUS_RemuxHandle handle,const NEXUS_RemuxSettings *pSettings)
{
    bool remux_restart_required=false;
    BERR_Code rc;
    BDBG_OBJECT_ASSERT(handle, NEXUS_Remux);
    BDBG_ASSERT(pSettings);

    if(handle->started)
    {
      NEXUS_Remux_P_Stop(handle);
      remux_restart_required=true;
    }

    #if BXPT_HAS_REMUX_PCR_OFFSET
    if(pSettings->bypass != handle->settings.bypass){
    rc = BXPT_Remux_SetBypassMode(handle->xptRmx, pSettings->bypass);
    if (rc != BERR_SUCCESS) { rc = BERR_TRACE(rc); goto error; }
    handle->settings.bypass = pSettings->bypass ;
    }
    /* No allPass setting required on this chip. Just attach an allPass pid channel and all data will flow out. */
    if(pSettings->pcrCorrectionEnabled!=handle->settings.pcrCorrectionEnabled
       || pSettings->pcrCorrectionOffset!= handle->settings.pcrCorrectionOffset){
    rc = BXPT_Remux_AddPcrOffset(handle->xptRmx, pSettings->pcrCorrectionEnabled, pSettings->pcrCorrectionOffset);
    if (rc != BERR_SUCCESS) { rc = BERR_TRACE(rc); goto error; }
    handle->settings.pcrCorrectionEnabled=pSettings->pcrCorrectionEnabled;
    }
    if(pSettings->enablePcrJitterAdjust!=handle->settings.enablePcrJitterAdjust){
     BXPT_Remux_SetPcrJitterAdj(handle->xptRmx, true, pSettings->enablePcrJitterAdjust);
     handle->settings.enablePcrJitterAdjust = pSettings->enablePcrJitterAdjust;
    }
    #else

    if(pSettings->bypass != handle->settings.bypass){
    rc = BXPT_Remux_SetBypassMode(handle->xptRmx, pSettings->bypass);
    if (rc != BERR_SUCCESS) { rc = BERR_TRACE(rc); goto error; }
    handle->settings.bypass = pSettings->bypass ;
    }

    if(pSettings->allPass!=handle->settings.allPass){
    rc = BXPT_Remux_SetAllPassMode(handle->xptRmx, pSettings->allPass);
    if (rc != BERR_SUCCESS) { rc = BERR_TRACE(rc); goto error; }
    handle->settings.allPass = pSettings->allPass;
    }

    if(pSettings->pcrCorrectionEnabled!=handle->settings.pcrCorrectionEnabled
       || pSettings->pcrCorrectionOffset!= handle->settings.pcrCorrectionOffset){
    rc = BXPT_Remux_SetPcrCorrection(handle->xptRmx, pSettings->pcrCorrectionEnabled, pSettings->pcrCorrectionOffset);
    if (rc != BERR_SUCCESS) { rc = BERR_TRACE(rc); goto error; }
    handle->settings.pcrCorrectionEnabled = pSettings->pcrCorrectionEnabled;
    handle->settings.pcrCorrectionOffset = pSettings->pcrCorrectionOffset;
    }

    if(pSettings->pacing!=handle->settings.pacing){
    rc = BXPT_Remux_ConfigPacing(handle->xptRmx, pSettings->pacing?BXPT_PacingControl_eStart:BXPT_PacingControl_eStop);
    if (rc != BERR_SUCCESS) { rc = BERR_TRACE(rc); goto error; }
    handle->settings.pacing = pSettings->pacing;
    }

    if(pSettings->pacingMaxError!=handle->settings.pacingMaxError){
    rc = BXPT_Remux_SetPacingErrorBound(handle->xptRmx, pSettings->pacingMaxError);
    if (rc != BERR_SUCCESS) { rc = BERR_TRACE(rc); goto error; }
    handle->settings.pacingMaxError = pSettings->pacingMaxError;
    }
    #endif

    if(remux_restart_required)
    {
      rc = NEXUS_Remux_P_Start(handle);
      if (rc != BERR_SUCCESS) { rc = BERR_TRACE(rc); goto error; }
    }

    return NEXUS_SUCCESS;
error:
     return rc;
}

void NEXUS_Remux_GetSettings(NEXUS_RemuxHandle handle,NEXUS_RemuxSettings *pSettings)
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_Remux);
    BDBG_ASSERT(pSettings);
    *pSettings = handle->settings;
    return;
}

#if defined(HUMAX_PLATFORM_BASE) 
NEXUS_Error NEXUS_Remux_GetAllPassStartPIDChannel(unsigned int *pAllPassStartPIDChannel)
{
	BERR_Code err = 0;
	unsigned int allPassStartPIDChannel = 0;

	err = BXPT_GetAllPassStartPIDChannel(&allPassStartPIDChannel);
	if(err != 0)
	{
		return -1;
	}

	*pAllPassStartPIDChannel = allPassStartPIDChannel;

	return 0;
}
#else /*don't delete to prevent errors when HUMAX_PLATFORM_BASE not defined*/
NEXUS_Error NEXUS_Remux_GetAllPassStartPIDChannel(unsigned int *pAllPassStartPIDChannel)
{
    BSTD_UNUSED(pAllPassStartPIDChannel);
    BDBG_WRN(("Not supported"));
    BERR_TRACE(BERR_NOT_SUPPORTED);
	return 0;
}
#endif

#else

void NEXUS_Remux_GetDefaultSettings(
    NEXUS_RemuxSettings *pSettings /* [out] */
    )
{
    BSTD_UNUSED(pSettings);
    BDBG_WRN(("Remux not enabled on this chipset"));
    BERR_TRACE(BERR_NOT_SUPPORTED);
    return;
}


NEXUS_RemuxHandle NEXUS_Remux_Open(
    unsigned index,
    const NEXUS_RemuxSettings *pSettings
    )
{
    BSTD_UNUSED(index);
    BSTD_UNUSED(pSettings);
    BDBG_WRN(("Remux not enabled on this chipset"));
    BERR_TRACE(BERR_NOT_SUPPORTED);
    return NULL;
}

static void NEXUS_Remux_P_Finalizer( NEXUS_RemuxHandle handle )
{
    BSTD_UNUSED(handle);
    BDBG_WRN(("Remux not enabled on this chipset"));
    BERR_TRACE(BERR_NOT_SUPPORTED);
    return;
}

NEXUS_OBJECT_CLASS_MAKE(NEXUS_Remux, NEXUS_Remux_Close);

void NEXUS_Remux_GetDefaultParserBandwidth(NEXUS_RemuxParserBandwidth *pSettings)
{
    BSTD_UNUSED(pSettings);
    BDBG_WRN(("Remux not enabled on this chipset"));
    BERR_TRACE(BERR_NOT_SUPPORTED);
    return;
}

NEXUS_Error NEXUS_Remux_SetParserBandwidth( NEXUS_RemuxHandle handle,const NEXUS_RemuxParserBandwidth *pSettings)
{
    BSTD_UNUSED(handle);
    BSTD_UNUSED(pSettings);
    BDBG_WRN(("Remux not enabled on this chipset"));
    BERR_TRACE(BERR_NOT_SUPPORTED);
    return 0;
}
/*
Summary:
Add a pid channel to the remux output
*/
NEXUS_Error NEXUS_Remux_AddPidChannel(
    NEXUS_RemuxHandle handle,
    NEXUS_PidChannelHandle pidChannel
    )
{
    BSTD_UNUSED(handle);
    BSTD_UNUSED(pidChannel);
    BDBG_WRN(("Remux not enabled on this chipset"));
    BERR_TRACE(BERR_NOT_SUPPORTED);
    return 0;
}

NEXUS_Error NEXUS_Remux_RemovePidChannel(
    NEXUS_RemuxHandle handle,
    NEXUS_PidChannelHandle pidChannel
    )
{
    BSTD_UNUSED(handle);
    BSTD_UNUSED(pidChannel);
    BDBG_WRN(("Remux not enabled on this chipset"));
    BERR_TRACE(BERR_NOT_SUPPORTED);
    return 0;
}

void NEXUS_Remux_RemoveAllPidChannels( NEXUS_RemuxHandle handle )
{
    BSTD_UNUSED(handle);
    BDBG_WRN(("Remux not enabled on this chipset"));
    BERR_TRACE(BERR_NOT_SUPPORTED);
    return;
}

NEXUS_Error NEXUS_Remux_Start(
    NEXUS_RemuxHandle handle
    )
{
    BSTD_UNUSED(handle);
    BDBG_WRN(("Remux not enabled on this chipset"));
    BERR_TRACE(BERR_NOT_SUPPORTED);
    return 0;
}

void NEXUS_Remux_Stop( NEXUS_RemuxHandle handle )
{
    BSTD_UNUSED(handle);
    BDBG_WRN(("Remux not enabled on this chipset"));
    BERR_TRACE(BERR_NOT_SUPPORTED);
    return;
}


NEXUS_Error NEXUS_Remux_SetSettings(NEXUS_RemuxHandle handle,const NEXUS_RemuxSettings *pSettings)
{
    BSTD_UNUSED(handle);
    BSTD_UNUSED(pSettings);
    BDBG_WRN(("Remux not enabled on this chipset"));
    BERR_TRACE(BERR_NOT_SUPPORTED);
    return 0;
}

void NEXUS_Remux_GetSettings(NEXUS_RemuxHandle handle,NEXUS_RemuxSettings *pSettings)
{
    BSTD_UNUSED(handle);
    BSTD_UNUSED(pSettings);
    BDBG_WRN(("Remux not enabled on this chipset"));
    BERR_TRACE(BERR_NOT_SUPPORTED);
    return;
}

#if defined(HUMAX_PLATFORM_BASE) 
NEXUS_Error NEXUS_Remux_GetAllPassStartPIDChannel(unsigned int *pAllPassStartPIDChannel)
{
    BSTD_UNUSED(pAllPassStartPIDChannel);
    BDBG_WRN(("Remux not enabled on this chipset"));
    BERR_TRACE(BERR_NOT_SUPPORTED);
	return 0;
}
#else /*don't delete to prevent errors when HUMAX_PLATFORM_BASE not defined*/
NEXUS_Error NEXUS_Remux_GetAllPassStartPIDChannel(unsigned int *pAllPassStartPIDChannel)
{
    BSTD_UNUSED(pAllPassStartPIDChannel);
    BDBG_WRN(("Remux not enabled on this chipset"));
    BERR_TRACE(BERR_NOT_SUPPORTED);
	return 0;
}
#endif

#endif /* NEXUS_NUM_REMUX */


