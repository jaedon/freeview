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
 * $brcm_Workfile: nexus_graphics3d_impl.h $
 * $brcm_Revision: 8 $
 * $brcm_Date: 9/17/12 1:14p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/graphics3d/src/nexus_graphics3d_impl.h $
 * 
 * 8   9/17/12 1:14p hauxwell
 * SW7435-352 : Nexus interrupt map needs changing to remove programming
 *  of V3D L2
 * 
 * 7   9/7/12 4:05p hauxwell
 * SWVC4-411 : rework nexus_graphics3d
 * 
 * 6   9/7/12 3:38p hauxwell
 * SWVC4-411 : rework nexus_graphics3d
 * 
 * 5   6/19/12 12:42p vsilyaev
 * SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
 * 
 * 4   5/18/12 8:55a erickson
 * SW7425-3086: extend NEXUS_Graphics3dCommandId, refactor impl
 * 
 * 3   5/19/11 9:13a hauxwell
 * SWDTV-7032: OpenGL performance for 35233 is worse than 35230
 * 
 * 3   5/19/11 9:09a hauxwell
 * SWDTV-7032: OpenGL performance for 35233 is worse than 35230
 * 
 * 2   3/17/11 4:23p erickson
 * SWVC4-133: merge
 * 
 * gsweet_multi_proc/1   3/17/11 2:10p gsweet
 * SWVC4-133: Multiprocess V3D support
 * 
 * 1   1/24/11 7:10p hauxwell
 * SWVC4-111: merge to mainline
 * 
 * hauxwell_35230_v3d/1   12/6/10 11:51a gsweet
 * SWVC4-101: Allow abstract platform support for V3D driver.
 * 
 *
 **************************************************************************/
#include "nexus_graphics3d_module.h"
#include "priv/nexus_core.h"

#include "bint.h"
#include "blst_queue.h"

#define MAGIC  0x3D5EED3D

BDBG_OBJECT_ID_DECLARE(NEXUS_Graphics3d);

struct NEXUS_Graphics3d {
    NEXUS_OBJECT(NEXUS_Graphics3d);
    BLST_Q_ENTRY(NEXUS_Graphics3d)     node;
    uint32_t                           magic;
    int32_t                            refCount;
    NEXUS_Graphics3dOpenSettings       openSettings;
    NEXUS_Graphics3dAcquireSettings    acqSettings;
    NEXUS_IsrCallbackHandle            mainISR;

    NEXUS_TaskCallbackHandle           tryAgainHandler;
};
