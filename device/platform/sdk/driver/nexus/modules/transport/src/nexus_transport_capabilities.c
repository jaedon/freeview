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
 * $brcm_Workfile: nexus_transport_capabilities.c $
 * $brcm_Revision: 8 $
 * $brcm_Date: 7/12/12 5:34p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/transport/7400/src/nexus_transport_capabilities.c $
 * 
 * 8   7/12/12 5:34p gmullen
 * SW7358-355: Added NEXUS_GetTransportStatus()
 * 
 * SW7358-355/1   7/12/12 2:35p gmullen
 * SW7358-355: Added NEXUS_GetTransportStatus()
 * 
 * 7   6/28/12 2:03p jtna
 * SW7401-4521: added NEXUS_TransportCapabilities.stcPrescaleMax and
 *  .stcIncrementMax
 * 
 * 6   3/14/12 11:39a erickson
 * SW7401-4517: remove old code
 * 
 * 5   2/3/12 11:55a erickson
 * SW7405-5598: add NEXUS_TransportCapabilities.numPacketSubs
 * 
 * 4   1/19/12 4:00p erickson
 * SW7425-2192: add NEXUS_TransportCapabilities.numTpitPids
 * 
 * 3   1/12/12 2:58p bandrews
 * SW7425-2099: only use XPT MSG CAP PID CH define if HAS_MESG_BUFFERS is
 *  defined
 * 
 * 2   1/5/12 12:12p erickson
 * SW7425-2099: add
 *  NEXUS_TransportCapabilities.numMessageCapablePidChannels (and fix
 *  vobRemap BCHP_CHIP list)
 * 
 * 1   1/18/08 2:20p jgarrett
 * PR 38808: Merging to main branch
 * 
 * Nexus_Devel/1   9/27/07 2:00p erickson
 * PR34925: added impl to complete public API. temp.
 * 
 **************************************************************************/
#include "nexus_transport_module.h"

void NEXUS_GetTransportCapabilities(NEXUS_TransportCapabilities *pCap)
{
    BKNI_Memset(pCap, 0, sizeof(*pCap));
    pCap->numInputBands = NEXUS_NUM_INPUT_BANDS;
    pCap->numParserBands = NEXUS_NUM_PARSER_BANDS;
    pCap->numPidChannels = NEXUS_NUM_PID_CHANNELS;
#ifdef BXPT_HAS_MESG_BUFFERS
    pCap->numMessageCapablePidChannels = BXPT_NUM_MESSAGE_CAPABLE_PID_CHANNELS;
#else
    /* TODO: should this be set to zero or max for platforms with only SW
    message support */
    pCap->numMessageCapablePidChannels = 0;
#endif
    pCap->numPlaypumps = NEXUS_NUM_PLAYPUMPS;
    pCap->numRecpumps = NEXUS_NUM_RECPUMPS;
    pCap->packetizer = true;
    pCap->vobRemap = true;
    pCap->numTpitPids = BXPT_NUM_TPIT_PIDS;
    pCap->numPacketSubs= BXPT_NUM_PACKETSUBS;
    
#ifdef BXPT_HAS_32BIT_PRESCALE
    pCap->stcPrescaleMax = 0xFFFFFFFF;
    pCap->stcIncMax = 0xFFFFFFFF;
#else
    pCap->stcPrescaleMax = 0xFF;
    pCap->stcIncMax = 0xFFFFFF;
#endif
}

void NEXUS_GetTransportStatus(
    NEXUS_TransportStatus *pStatus
    )
{
    BXPT_Rave_Status raveStatus;

    BDBG_ASSERT(pStatus);

    BKNI_Memset(pStatus, 0, sizeof(*pStatus));
    BXPT_Rave_GetStatus( pTransport->rave[0].channel, &raveStatus );
    pStatus->rave.totalContexts = raveStatus.SupportedContexts;
    pStatus->rave.totalContextsUsed = raveStatus.AllocatedContexts;
}

