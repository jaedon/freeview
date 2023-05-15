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
 * $brcm_Workfile: nexus_transport_capabilities.h $
 * $brcm_Revision: 10 $
 * $brcm_Date: 7/12/12 5:34p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/transport/7400/include/nexus_transport_capabilities.h $
 *
 * 10   7/12/12 5:34p gmullen
 * SW7358-355: Added NEXUS_GetTransportStatus()
 *
 * SW7358-355/1   7/12/12 2:35p gmullen
 * SW7358-355: Added NEXUS_GetTransportStatus()
 *
 * 9   6/28/12 2:03p jtna
 * SW7401-4521: added NEXUS_TransportCapabilities.stcPrescaleMax and
 *  .stcIncrementMax
 *
 * 8   3/15/12 5:11p erickson
 * SW7425-2607: increase for 7435
 *
 * 7   3/13/12 4:42p erickson
 * SW7425-2607: use unvarying macros in API for binary compatibility
 *
 * 6   2/3/12 11:55a erickson
 * SW7405-5598: add NEXUS_TransportCapabilities.numPacketSubs
 *
 * 5   1/19/12 4:00p erickson
 * SW7425-2192: add NEXUS_TransportCapabilities.numTpitPids
 *
 * 4   1/5/12 12:11p erickson
 * SW7425-2099: add
 *  NEXUS_TransportCapabilities.numMessageCapablePidChannels (and fix
 *  vobRemap BCHP_CHIP list)
 *
 * 3   1/23/08 8:37p vobadm
 * PR35457: update docs
 *
 * 2   1/23/08 5:16p erickson
 * PR35457: update docs
 *
 * 1   1/18/08 2:16p jgarrett
 * PR 38808: Merging to main branch
 *
 * Nexus_Devel/4   11/30/07 11:12a erickson
 * PR35457: api update
 *
 * Nexus_Devel/3   11/29/07 2:24p erickson
 * PR35457: doc update for 0.5 release
 *
 **************************************************************************/
#ifndef NEXUS_TRANSPORT_CAPABILITIES_H__
#define NEXUS_TRANSPORT_CAPABILITIES_H__

#ifdef __cplusplus
extern "C" {
#endif

/* macros used in transport API. do not add internal-only macros here. */
#define NEXUS_MAX_PARSER_BANDS 16
#define NEXUS_MAX_PLAYPUMPS    22
/* #if defined(HUMAX_PLATFORM_BASE)
   move NEXUS_MAX_MTSIF to nexus_types.h, merged from URSR14.1 by iglee
#define NEXUS_MAX_MTSIF        2
*/
#define NEXUS_MAX_REMUX        2

/**
Summary:
Query the capabilities of this chip.

Description:
Used in NEXUS_GetTransportCapabilities
**/
typedef struct NEXUS_TransportCapabilities
{
    unsigned numInputBands;
    unsigned numParserBands;
    unsigned numPidChannels;
    unsigned numMessageCapablePidChannels;
    unsigned numPlaypumps;
    unsigned numRecpumps;
    unsigned numTpitPids; /* number of pids that the TPIT indexer can support */
    unsigned numPacketSubs;
    bool packetizer; /* Can the HW packetize ES/PES into TS? */
    bool vobRemap;   /* Can the HW remap VOB audio substream ID's */
    uint32_t stcPrescaleMax; /* Largest prescale setting allowed for STC */
    uint32_t stcIncMax;      /* Largest increment setting allowed for STC */
} NEXUS_TransportCapabilities;

/**
Summary:
Get the chip's transport capabilities.
**/
void NEXUS_GetTransportCapabilities(
    NEXUS_TransportCapabilities *pCapabilities /* [out] */
    );

/**
Summary:
Query the status of the transport module/core.

Description:
Used in NEXUS_GetTransportStatus
**/
typedef struct NEXUS_TransportStatus
{
    struct {
        unsigned totalContexts;
        unsigned totalContextsUsed;
    } rave;
} NEXUS_TransportStatus;

/**
Summary:
Get the status of the transport module and hardware.
**/
void NEXUS_GetTransportStatus(
    NEXUS_TransportStatus *pStatus
    );

#ifdef __cplusplus
}
#endif

#endif
