/***************************************************************************
 *     (c)2008-2012 Broadcom Corporation
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
 * $brcm_Workfile: nexus_graphics3d.c $
 * $brcm_Revision: 17 $
 * $brcm_Date: 6/19/12 6:46p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/graphics3d/7400/src/nexus_graphics3d.c $
 * 
 * 17   6/19/12 6:46p vsilyaev
 * SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
 * 
 * 16   4/25/11 3:24p mward
 * SW7125-905:  NEXUS_PowerManagement_SetCoreState()  not needed in module
 *  init.
 * 
 * 15   10/21/10 4:47p mphillip
 * SW7125-670: Move bchp shim out of graphics3d for proxy mode
 * 
 * 14   8/26/10 5:09p mward
 * SW7405-4433: merge BCHP_PWR.  Init new members of BCHP_P_Context in the
 *  fakeChip.
 * 
 * 13   7/2/10 6:36p jrubio
 * SW7342-224: fix typo for 7340
 * 
 * 12   7/2/10 5:34p jrubio
 * SW7342-224: Add 7340/7342 Support
 * 
 * 11   10/5/09 2:22p mward
 * SW7125-4: Add support for 7125.
 * 
 * 10   7/8/09 10:37a jtna
 * PR55968: 3d graphics power management
 * 
 * 9   2/12/09 4:36p jrubio
 * PR51629: add 7336
 * 
 * 8   1/12/09 4:53p katrep
 * PR50472: Add support for 7405 C0
 * 
 * 7   11/28/08 1:12p nickh
 * PR48963: Add 7420 3D support
 * 
 * 6   11/10/08 4:40p mphillip
 * PR48697: Add 7400 support
 * 
 * 5   11/4/08 3:28p mphillip
 * PR47441: Proxy mode fake chip support
 * 
 * 4   7/18/08 1:13p mphillip
 * PR40869: Fix compile warnings, remove unnecessary assertion
 * 
 * 3   7/11/08 6:43p mphillip
 * PR40869: Correct the name of the module handle variable
 * 
 * 2   7/10/08 4:02p mphillip
 * PR40869: Update for header checkin
 * 
 * 1   7/9/08 4:55p mphillip
 * PR40869: Add graphics3d module back
 * 
 *
 **************************************************************************/
#include "nexus_graphics3d_module.h"
#include "nexus_power_management.h"
#if !NEXUS_GRAPHICS3D_MODE_PROXY
/* This is only needed for the user-space version of Nexus_Graphics3D
 * Proxy mode creates its own handles.
 */
#include "priv/nexus_core.h"
#endif

#include "bp3d.h"

BDBG_MODULE(nexus_graphics3d);

NEXUS_ModuleHandle NEXUS_P_Graphics3dModule;
struct {
    NEXUS_Graphics3dModuleSettings settings;
    BP3D_Handle p3d;
} g_graphics3d = {{0},NULL};

typedef struct NEXUS_Graphics3D {
    NEXUS_OBJECT(NEXUS_Graphics3D);
} NEXUS_Graphics3D;

static  NEXUS_Graphics3D g_NEXUS_Graphics3D_dummy;

static NEXUS_Graphics3DHandle g_NEXUS_graphics3DHandle = &g_NEXUS_Graphics3D_dummy;

static BERR_Code NEXUS_Graphics3d_P_InitP3d(void);
static void NEXUS_Graphics3d_P_UninitP3d(void);

/****************************************
* Module functions
***************/

void NEXUS_Graphics3dModule_GetDefaultSettings(NEXUS_Graphics3dModuleSettings *pSettings)
{
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
}

NEXUS_ModuleHandle NEXUS_Graphics3dModule_Init(const NEXUS_Graphics3dModuleSettings *pSettings)
{
    NEXUS_ModuleSettings moduleSettings;
    NEXUS_Graphics3dModuleSettings defaultSettings;

    BDBG_ASSERT(!NEXUS_P_Graphics3dModule);

    if (!pSettings) {
        NEXUS_Graphics3dModule_GetDefaultSettings(&defaultSettings);
        pSettings = &defaultSettings;
    }

    /* init global module handle */
    NEXUS_Module_GetDefaultSettings(&moduleSettings);
    moduleSettings.priority = NEXUS_ModulePriority_eLow;
    NEXUS_P_Graphics3dModule = NEXUS_Module_Create("graphics3d", &moduleSettings);
    if (!NEXUS_P_Graphics3dModule) {
        BDBG_ERR(("Unable to create graphics3d module handle"));
        return NULL;
    }

    g_graphics3d.settings = *pSettings;

    return NEXUS_P_Graphics3dModule;
}

void NEXUS_Graphics3dModule_Uninit(void)
{
    NEXUS_Module_Destroy(NEXUS_P_Graphics3dModule);
    NEXUS_P_Graphics3dModule = NULL;
}

NEXUS_Graphics3DHandle NEXUS_Graphics3D_Open(unsigned index, const NEXUS_Graphics3DOpenSettings *pSettings)
{
    NEXUS_Graphics3DOpenSettings openSettings;
    BSTD_UNUSED(index);

    if (pSettings==NULL) {
        NEXUS_Graphics3D_GetDefaultOpenSettings(&openSettings);
        pSettings = &openSettings;
    }

    NEXUS_PowerManagement_SetCoreState(NEXUS_PowerManagementCore_eGraphics3D, true);
    NEXUS_Graphics3d_P_InitP3d();
    NEXUS_OBJECT_SET(NEXUS_Graphics3D, g_NEXUS_graphics3DHandle);
    return g_NEXUS_graphics3DHandle;
}

static void NEXUS_Graphics3D_P_Finalizer(NEXUS_Graphics3DHandle gfx)
{
    BSTD_UNUSED(gfx);
    NEXUS_OBJECT_ASSERT(NEXUS_Graphics3D, gfx);
    NEXUS_Graphics3d_P_UninitP3d();
    NEXUS_PowerManagement_SetCoreState(NEXUS_PowerManagementCore_eGraphics3D, false);
    NEXUS_OBJECT_UNSET(NEXUS_Graphics3D, gfx);
}

NEXUS_OBJECT_CLASS_MAKE(NEXUS_Graphics3D, NEXUS_Graphics3D_Close);

void NEXUS_Graphics3D_GetDefaultOpenSettings(NEXUS_Graphics3DOpenSettings *pSettings)
{
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
}

#if NEXUS_GRAPHICS3D_MODE_PROXY
static BERR_Code NEXUS_Graphics3d_P_InitP3d(void)
{
    BERR_Code rc = BERR_SUCCESS;

    return rc;
}

void NEXUS_Graphics3d_P_UninitP3d(void)
{
}
#else
/* Graphics3D is being built for user-space.  No special case is needed to 
 * handle passing interrupts back.
 */
static BERR_Code NEXUS_Graphics3d_P_InitP3d(void)
{
    BERR_Code rc;

    rc = BP3D_Open(&g_graphics3d.p3d, g_pCoreHandles->chp, g_pCoreHandles->reg, g_pCoreHandles->heap[0], g_pCoreHandles->bint);
    if (rc) {BDBG_ERR(("Unable to create p3d handle for graphics3d")); return BERR_TRACE(rc);}

    return rc;
}

void NEXUS_Graphics3d_P_UninitP3d(void)
{
    BP3D_Close(g_graphics3d.p3d);
}
#endif

BP3D_Handle NEXUS_Graphics3d_GetP3d_priv(void)
{
    return g_graphics3d.p3d;
}

