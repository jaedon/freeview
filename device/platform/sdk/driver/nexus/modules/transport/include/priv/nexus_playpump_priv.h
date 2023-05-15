/***************************************************************************
 *     (c)2010-2012 Broadcom Corporation
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
 * $brcm_Workfile: nexus_playpump_priv.h $
 * $brcm_Revision: 5 $
 * $brcm_Date: 7/27/12 2:14p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/transport/include/priv/nexus_playpump_priv.h $
 * 
 * 5   7/27/12 2:14p jgarrett
 * SW7425-3281: Merge to main branch
 * 
 * 4   7/17/12 4:44p jgarrett
 * SW7425-3281: Incorporating code review feedback
 * 
 * SW7425-3281/1   7/18/12 9:18a jgarrett
 * SW7425-3281: Branching for 7231 release
 * 
 * 4   7/17/12 4:44p jgarrett
 * SW7425-3281: Incorporating code review feedback
 * 
 * 3   12/28/10 5:44p vsilyaev
 * SW7425-39: Added Mux specific interfaces
 * 
 * 2   12/23/10 3:25p vsilyaev
 * SW7425-39: Start/stop playpump inside mux
 * 
 * 1   12/16/10 6:04p vsilyaev
 * SW7425-39: Added playpump private API
 * 
 **************************************************************************/
#ifndef NEXUS_PLAYPUMP_PRIV_H__
#define NEXUS_PLAYPUMP_PRIV_H__

#include "nexus_types.h"
#include "nexus_playpump.h"

#include "bpvrlib_feed.h"

#ifdef __cplusplus
extern "C" {
#endif

NEXUS_Error NEXUS_Playpump_AddExtendedOffsetEntries_priv(
        NEXUS_PlaypumpHandle playpump, /* instance of NEXUS_Playpump */
		const BPVRlib_Feed_ExtendedOffsetEntry *entries, /* pointer to array of BPVRlib_Feed_ExtendedOffsetEntries */ 
		size_t count, /* number of elements in the array */ 
		size_t *nconsumed/* out pointer to return number of consumed elements */
		);

NEXUS_Error NEXUS_Playpump_GetCompleted_priv(
        NEXUS_PlaypumpHandle playpump, /* instance of NEXUS_Playpump */
		size_t *ncompleted	/* out pointer to return number of completed elements */
		);

NEXUS_Error NEXUS_Playpump_StartMuxInput_priv(
        NEXUS_PlaypumpHandle playpump /* instance of NEXUS_Playpump */
        );

NEXUS_PidChannelHandle NEXUS_Playpump_OpenPidChannel_priv(
    NEXUS_PlaypumpHandle playpump,
    uint16_t src_pid,                   /* source substream */
    uint16_t dst_pid,                   /* destination substream */
    const NEXUS_PlaypumpOpenPidChannelSettings *pSettings 
    );

#if defined(B_HAS_ASF)
void* NEXUS_Playpump_GetAsfHandle_priv(NEXUS_PlaypumpHandle handle);
#endif

#ifdef __cplusplus
}
#endif

#endif
