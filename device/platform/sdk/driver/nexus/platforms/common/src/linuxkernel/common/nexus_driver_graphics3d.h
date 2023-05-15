/***************************************************************************
 *     (c)2010 Broadcom Corporation
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
 * $brcm_Workfile: nexus_driver_graphics3d.h $
 * $brcm_Revision: 1 $
 * $brcm_Date: 6/10/10 4:10p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/platforms/97400/src/linuxkernel/nexus_driver_graphics3d.h $
 * 
 * 1   6/10/10 4:10p mphillip
 * SW7405-4091: Move nexus_driver 3D hooks to separate file for NFE
 * 
  ************************************************************/
#ifndef NEXUS_DRIVER_GRAPHICS3D_H__
#define NEXUS_DRIVER_GRAPHICS3D_H__

#ifdef NEXUS_HAS_3D
#include "bstd.h"
#include "bkni.h"

typedef struct PROXY_NEXUS_GetSurfaceOffset {
    void *surface;
    uint32_t offset;
    int width;
    int height;
    int pitch;
    int pixel_format;
} PROXY_NEXUS_GetSurfaceOffset;

typedef struct PROXY_NEXUS_Hook3dInterrupt {
    unsigned which_interrupt;
} PROXY_NEXUS_Hook3dInterrupt;

typedef struct PROXY_NEXUS_WaitFor3dInterrupt {
    bool was_fired;
} PROXY_NEXUS_WaitFor3dInterrupt;

typedef struct PROXY_NEXUS_Unhook3dInterrupt {
    unsigned which_interrupt;
} PROXY_NEXUS_Unhook3dInterrupt;

typedef struct PROXY_NEXUS_3dPowerManagement {
    bool pm_state;
} PROXY_NEXUS_3dPowerManagement;

void nexus_driver_3d_get_surface_offset(PROXY_NEXUS_GetSurfaceOffset *data);
BERR_Code nexus_driver_3d_hook_interrupt(PROXY_NEXUS_Hook3dInterrupt *data);
void nexus_driver_3d_wait_for_interrupt(PROXY_NEXUS_WaitFor3dInterrupt *data);
void nexus_driver_3d_unhook_interrupt(void);
void nexus_driver_3d_power_management(PROXY_NEXUS_3dPowerManagement *data);
void nexus_driver_3d_irq_isr(void *pvParam1,int iParam2 );
#endif /* NEXUS_HAS_3D */

#endif /* NEXUS_DRIVER_GRAPHICS3D_H__ */



