/***************************************************************************
 *     (c)2010-2011 Broadcom Corporation
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
 * $brcm_Workfile: nexus_driver_graphics3d.c $
 * $brcm_Revision: 3 $
 * $brcm_Date: 7/11/11 11:42a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/platforms/97400/src/linuxkernel/nexus_driver_graphics3d.c $
 * 
 * 3   7/11/11 11:42a erickson
 * SWDTV-7208: remove unnecessary #include
 * 
 * 2   6/28/10 12:13p mphillip
 * SW7405-4091: Fix a missed rename.
 * 
 * 1   6/10/10 4:10p mphillip
 * SW7405-4091: Move nexus_driver 3D hooks to separate file for NFE
 * 
  ************************************************************/
#ifdef NEXUS_HAS_3D
#include "bkni.h"
#include "bint.h"

#include "priv/nexus_core.h"

#include "nexus_surface.h"
#include "priv/nexus_surface_priv.h"
#include "nexus_power_management.h"
#include "nexus_driver_graphics3d.h"

BDBG_MODULE(nexus_driver_graphics3d);

static BKNI_EventHandle g_3d_hInterruptEvent = NULL;
static BINT_CallbackHandle g_3d_hInterruptCallback = NULL;



void nexus_driver_3d_get_surface_offset(PROXY_NEXUS_GetSurfaceOffset *data)
{
    NEXUS_SurfaceMemory sur_memory;
    NEXUS_SurfaceCreateSettings sur_settings;

    NEXUS_Surface_GetMemory(data->surface, &sur_memory);
    NEXUS_Surface_GetCreateSettings(data->surface, &sur_settings);
    data->offset = NEXUS_AddrToOffset(sur_memory.buffer);
    data->width = sur_settings.width;
    data->height = sur_settings.height;
    data->pitch = sur_memory.pitch;
    data->pixel_format = sur_settings.pixelFormat;
}

BERR_Code nexus_driver_3d_hook_interrupt(PROXY_NEXUS_Hook3dInterrupt *data)
{
    BERR_Code rc = BERR_SUCCESS;

    rc = BKNI_CreateEvent( &g_3d_hInterruptEvent );
    if( rc != BERR_SUCCESS ) {
        BDBG_MSG(("Failed to create 3d event"));
        return rc;
    }
    BKNI_ResetEvent(g_3d_hInterruptEvent);
    rc = BINT_CreateCallback( &g_3d_hInterruptCallback, g_pCoreHandles->bint,
            data->which_interrupt, &nexus_driver_3d_irq_isr, NULL, 0 );
    if( rc != BERR_SUCCESS ) {
        BDBG_ERR(("Failed to register callback for 3d event"));
        return rc;
    }
    rc = BINT_ClearCallback( g_3d_hInterruptCallback );
    if( rc != BERR_SUCCESS ) {
        BDBG_ERR(("Failed to clear callback for 3d event"));
        return rc;
    }
    rc = BINT_EnableCallback( g_3d_hInterruptCallback );
    if( rc != BERR_SUCCESS ) {
        BDBG_ERR(("Failed to enable callback for 3d event"));
        return rc;
    }
    return rc;
}

void nexus_driver_3d_wait_for_interrupt(PROXY_NEXUS_WaitFor3dInterrupt *data)
{
    BERR_Code rc = BERR_SUCCESS;
    data->was_fired = false;
    rc = BKNI_WaitForEvent( g_3d_hInterruptEvent, 30000 );
    if (rc == BERR_SUCCESS) {
        data->was_fired = true;
    }
}

void nexus_driver_3d_unhook_interrupt(void)
{
    if( g_3d_hInterruptCallback)
        BINT_DestroyCallback( g_3d_hInterruptCallback );
    g_3d_hInterruptCallback = NULL;
    if( g_3d_hInterruptEvent )
        BKNI_DestroyEvent( g_3d_hInterruptEvent );
    g_3d_hInterruptEvent = NULL;
}

void nexus_driver_3d_power_management(PROXY_NEXUS_3dPowerManagement *data)
{
    NEXUS_PowerManagement_SetCoreState(NEXUS_PowerManagementCore_eGraphics3D, data->pm_state);
}

void nexus_driver_3d_irq_isr(
    void *pvParam1,
    int iParam2 )
{
    BSTD_UNUSED(pvParam1);
    BSTD_UNUSED(iParam2);
    BKNI_SetEvent( g_3d_hInterruptEvent );
}
#endif /* NEXUS_HAS_3D */



