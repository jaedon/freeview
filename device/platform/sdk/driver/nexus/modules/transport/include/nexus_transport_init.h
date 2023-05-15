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
 * $brcm_Workfile: nexus_transport_init.h $
 * $brcm_Revision: 22 $
 * $brcm_Date: 10/16/12 1:34p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/transport/include/nexus_transport_init.h $
 * 
 * 22   10/16/12 1:34p erickson
 * SW7435-375: transport does not depend on security
 * 
 * 21   10/8/12 10:00a erickson
 * SW7346-119: clarify that maxDataRate is in bits per second
 * 
 * 20   10/5/12 11:50a erickson
 * SW7435-400: clarify how app can set module settings
 * 
 * 19   3/14/12 11:53a erickson
 * SW7401-4517: move BCHP_CHIP macro from API to implementation
 * 
 * 18   3/13/12 4:42p erickson
 * SW7425-2607: use unvarying macros in API for binary compatibility
 * 
 * 17   3/5/12 2:49p vsilyaev
 * SW7425-2549, SW7425-2399: Improved handling of playback peak rate
 * 
 * 16   2/16/12 2:41p vsilyaev
 * SW7425-2399: Added peakDataRate for the playback(playpump) channels
 * 
 * 15   2/9/12 11:56a jtna
 * SW7425-2248: add
 *  NEXUS_TransportModuleSettings.clientEnabled.parserBand[].mpodRs
 * 
 * 14   12/20/11 1:40p gmullen
 * SW7358-176: Added per-client XC and RS allocation control
 * 
 * SW7358-176/1   12/20/11 1:32p gmullen
 * SW7358-176: Added per-client XC and RS allocation control
 * 
 * 13   4/15/11 10:35a erickson
 * SW7425-313: add initial MTSIF support
 *
 * 12   4/5/11 11:39a jtna
 * SW7346-119: wrap new code with #if NEXUS_NUM_PARSER_BANDS
 *
 * 11   4/1/11 4:24p jtna
 * SW7346-119: add 40nm allpass support
 *
 * 10   1/17/11 9:31a erickson
 * SW7425-68: add NEXUS_TransportModuleSettings.mainHeapIndex
 *
 * 9   7/14/10 10:33a erickson
 * SW7405-4621: replace nexus_base.h with nexus_types.h
 *
 * 8   2/17/10 10:13a erickson
 * SW7335-675: added NEXUS_TransportModuleSettings.tpit settings for
 *  idleEventTimeout and recordEventTimeout
 *
 * 7   2/3/10 1:18p erickson
 * SW7408-1: NEXUS_TRANSPORT_BEFORE_SECURITY for 7408
 *
 * 6   12/29/09 12:51p erickson
 * SW7550-146: allow security module to be initialized after the transport
 *  module if HW requires it
 *
 * 5   10/19/09 11:40a erickson
 * SW7400-2559: added NEXUS_TransportModuleSettings.dssSaveMptFlag
 *
 * 4   3/30/09 10:35a erickson
 * PR50748: support the secure heap using module settings handle
 *
 * 3   8/5/08 9:05a erickson
 * PR45422: transport module needs security handle to support DivX DRM
 *
 * 2   4/18/08 4:03p vsilyaev
 * PR 41868: Added security API to playpump and recpump
 *
 * 1   1/18/08 2:19p jgarrett
 * PR 38808: Merging to main branch
 *
 * Nexus_Devel/5   10/3/07 9:50a erickson
 * PR34925: coding convention
 *
 * Nexus_Devel/4   9/28/07 10:00a erickson
 * PR34925: 0.1 prerel review
 *
 * Nexus_Devel/3   9/24/07 2:36p erickson
 * PR34925: added Message
 *
 * Nexus_Devel/2   9/14/07 3:27p erickson
 * PR34925: initial compilation
 *
 * Nexus_Devel/1   9/14/07 12:29p erickson
 * PR34925: update
 *
 **************************************************************************/
#ifndef NEXUS_TRANSPORT_INIT_H__
#define NEXUS_TRANSPORT_INIT_H__

#include "nexus_types.h"
#include "nexus_transport_capabilities.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct NEXUS_TransportModuleSettings
{
    unsigned mainHeapIndex;      /* Heap index used by XPT to allocate most buffers including RAVE CDB/ITB, playback, record and message buffers. */
    NEXUS_ModuleHandle dma;      /* Optional. Dma module is used for security.
                                    This handle can be set after NEXUS_TransportModule_Init using NEXUS_TransportModule_SetDmaModule. */
    NEXUS_HeapHandle secureHeap; /* Optional. if set, transport will allocate all RAVE contexts from this heap. */

    bool dssSaveMptFlag; /* if true, the DSS MPT flag will be put in the front of the reconstructed message.
                            this is a global setting. */

    /* global TPIT filter settings. see NEXUS_RecpumpSettings.tpit for per-record TPIT settings. */
    struct {
        unsigned idleEventTimeout;   /* Number of 108 MHz clocks before triggering a record event timeout. See NEXUS_RecpumpSettings.tpit.idleEventEnabled. */
        unsigned recordEventTimeout; /* Number of 108 MHz clocks before triggering a record packet timeout. See NEXUS_RecpumpSettings.tpit.recordEventEnabled. */
    } tpit;

    struct {
        unsigned parserBand[NEXUS_MAX_PARSER_BANDS]; /* maximum data rate in units of bits per second */
        unsigned playback[NEXUS_MAX_PLAYPUMPS]; /* maximum data rate in units of bits per second */
    } maxDataRate; /* for 40nm, this replaces NEXUS_ParserBandSettings.maxDataRate */

    struct {
        bool enabled;
        unsigned rxInterfaceWidth; /* width of MTSIF bus. supported values are 1, 2, 4 and 8 bits. */
    } mtsif[NEXUS_MAX_MTSIF];

    /* Describes which transport components will receive data from each input and playback parser. Applications can reduce the
       amount of memory required by setting unused paths to false. */
    struct {
        struct {
            bool rave; /* decode/record */
            bool message;
            bool remux[NEXUS_MAX_REMUX];
            bool mpodRs; /* if true, data is routed to MPOD RS buffer first, then to MPOD (if enabled). this affects RTS.
                            if false, data is routed directly to MPOD (if enabled) */
        } parserBand[NEXUS_MAX_PARSER_BANDS];
        struct {
            bool rave; /* decode/record */
            bool message;
            bool remux[NEXUS_MAX_REMUX];
        } playback[NEXUS_MAX_PLAYPUMPS];
    } clientEnabled;
} NEXUS_TransportModuleSettings;

void NEXUS_TransportModule_GetDefaultSettings(
    NEXUS_TransportModuleSettings *pSettings /* [out] */
    );

/**
Summary:
Initialize the transport module

Description:
This function is called by NEXUS_Platform_Init, not by applications.
If you want to modify these settings from your application, you can do this 
through NEXUS_PlatformSettings as follows:

    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.transportModuleSettings.xxx = xxx;
    NEXUS_Platform_Init(&platformSettings);

**/
NEXUS_ModuleHandle NEXUS_TransportModule_Init(
    const NEXUS_TransportModuleSettings *pSettings
    );

void NEXUS_TransportModule_Uninit(void);

#ifdef __cplusplus
}
#endif

#endif
