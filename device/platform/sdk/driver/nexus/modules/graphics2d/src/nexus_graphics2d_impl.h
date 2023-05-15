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
 * $brcm_Workfile: nexus_graphics2d_impl.h $
 * $brcm_Revision: 21 $
 * $brcm_Date: 7/18/12 3:48p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/graphics2d/src/nexus_graphics2d_impl.h $
 * 
 * 21   7/18/12 3:48p erickson
 * SW7425-3433: simplify dynamic PM for better performance
 * 
 * 20   6/19/12 12:42p vsilyaev
 * SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
 * 
 * 19   1/13/12 5:30p jtna
 * SW7425-1708: rework graphics2d standby power mgmt
 * 
 * 18   1/5/12 12:28p erickson
 * SW7435-30: do not call surface module private API
 * 
 * 17   12/19/11 10:04a erickson
 * SW7425-1795: allow NULL packet buffer heap. validate driver-side CPU
 *  accessibility at GetPacketBuffer.
 * 
 * 16   8/24/11 10:09a erickson
 * SW7420-1894: add checkpoint watchdog if packetFifoThreshold is non-
 *  zero. helps debug apps that aren't calling checkpoint.
 * 
 * 15   5/24/11 4:03p erickson
 * SW7346-149: support multiplexing of packet and function-based blit by
 *  using two GRC packet contexts within a single NEXUS_Graphics2D context
 * 
 * 14   5/24/11 3:13p erickson
 * SW7420-1200: reduce cpu util on blit
 * 
 * 13   5/23/11 4:50p erickson
 * SW7420-1200: refactor grclib for optimal nexus use
 * 
 * 12   10/28/10 5:48p jtna
 * SW7405-4810: add checkpoint-based gfx2d power management
 * 
 * 11   8/16/10 11:08a erickson
 * SW7405-3671: add packet blit API. switch to packet-based GRC
 *  implementation.
 *
 * 10   6/18/10 4:30p erickson
 * SW7405-4508: virtualize NEXUS_Graphics2D_Open so that each user can
 *  have their own blitter and their own checkpoint callback
 *
 * 9   8/12/09 1:34p vsilyaev
 * PR 57629: Moved large structures from stack to permanent memory
 *  storage, this decreased stack consumption from 514+ to 136 bytes. It
 *  should also improve cache locality
 *
 * 8   8/12/09 12:52p vsilyaev
 * PR 57629: Cache results of NEXUS_GRCLIB_PENDTYPE
 *
 * 7   8/10/09 6:02p vsilyaev
 * PR 57552: Use tagged tailquee to manage temporary surfaces, traverse
 *  list only after user called Checkpoint function
 *
 * 6   8/10/09 12:57p vsilyaev
 * PR 57469: Return proper error code when queue is full and try to handle
 *  condition when BGRC_WaitForOperationsComplete returns out of memory
 *  error
 *
 * 5   6/24/09 1:37p erickson
 * PR55968: added power management
 *
 * 4   5/13/09 9:34a erickson
 * PR51077: use BGRC_WaitForOperationsComplete instead of dummy blit for
 *  checkpoint
 *
 * 3   3/11/09 9:20a erickson
 * PR53076: support NEXUS_Graphics2D_DestripeToSurface to an RGB surface
 *
 * 2   1/26/09 9:49a erickson
 * PR51393: add support for async non-GRClib blits including Destripe and
 *  Memset32
 *
 * 1   2/25/08 2:59p erickson
 * PR39786: added NEXUS_Graphics2D_Destripe
 *
 **************************************************************************/
#include "nexus_graphics2d_module.h"
#include "nexus_graphics2d_init.h"
#include "priv/nexus_graphics2d_standby_priv.h"

#include "bgrclib_packet.h"
#include "bgrc.h"
#include "bgrc_packet.h"
#include "blst_squeue.h"

#define NEXUS_GRAPHICS2D_MAX_CONTEXTS 32
struct NEXUS_Graphics2DEngine {
    BGRC_Handle grc;
    BLST_S_HEAD(NEXUS_Graphics2DEngineContexts, NEXUS_Graphics2D) contexts;
    BKNI_EventHandle advanceEvent;
    NEXUS_EventCallbackHandle advanceEventCallback;
    unsigned allocatedContexts;
    BGRC_Packet_ContextStatus contextStatus[NEXUS_GRAPHICS2D_MAX_CONTEXTS];
    struct {
        NEXUS_HeapHandle heap;
        BGRC_Settings grcSettings;
        bool s3Standby;
    } standby;
};

struct NEXUS_Graphics2D {
    NEXUS_OBJECT(NEXUS_Graphics2D);
    unsigned index;

    /* magnum */
    BGRC_Handle grc;
    BGRClib_Handle grclib;
    BGRC_PacketContext_Handle functionContext; /* context used for functions */
    BGRC_PacketContext_Handle packetContext; /* context used for packet blit */
    BLST_S_ENTRY(NEXUS_Graphics2D) link; /* for NEXUS_Graphics2DEngine.contexts */

    NEXUS_Graphics2DOpenSettings openSettings;
    NEXUS_Graphics2DSettings settings;
    NEXUS_TaskCallbackHandle checkpoint;
    NEXUS_TaskCallbackHandle packetSpaceAvailable;
    unsigned packetSpaceAvailableCount;
    unsigned checkpointCount;
    struct {
        unsigned counter;
        NEXUS_TimerHandle timer;
    } checkpointWatchdog;
    bool verifyPacketHeap;

    struct {
        BGRClib_BlitParams blitParams;
        BGRClib_BlitColorKeyParams colorKeyParams;
        BGRClib_BlitMatrixParams matrixParams;
        BGRClib_BlitPatternParams patternParams;
        BGRClib_BlitScalingControlParams scalingControlParams;
    } blitData;

    struct {
        BGRC_PacketContext_CreateSettings packetContextSettings;
    } standby;
    NEXUS_BaseObject baseObject;
};

/* used for blit and destripe, so it can't be static */
extern const int32_t g_NEXUS_ai32_Matrix_YCbCrtoRGB[20];

/* global data. */
struct Graphics2DData {
    NEXUS_Graphics2DModuleSettings settings;
};
extern struct Graphics2DData g_NEXUS_graphics2DData;

NEXUS_OBJECT_CLASS_DECLARE(NEXUS_Graphics2D);


