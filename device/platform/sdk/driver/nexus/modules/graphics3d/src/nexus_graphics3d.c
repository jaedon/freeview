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
 *  secrets of Broadcom, and you shall use all reasonable efforts to protect the confIdiality thereof,
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
 *  LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIdAL, SPECIAL, INDIRECT, OR
 *  EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
 *  USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
 *  THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
 *  ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
 *  LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
 *  ANY LIMITED REMEDY.
 *
 * $brcm_Workfile: nexus_graphics3d.c $
 * $brcm_Revision: 44 $
 * $brcm_Date: 10/26/12 5:25a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/graphics3d/src/nexus_graphics3d.c $
 * 
 * 44   10/26/12 5:25a hauxwell
 * SW7435-452 : Compiler warnings
 * 
 * 43   10/18/12 7:08p hauxwell
 * SWANDROID-76 : [CTS] CtsGraphicsTestCases test failed
 * 
 * 42   9/25/12 5:25p hauxwell
 * SWVC4-429 : BEGL_HW_SCRATCH is vectored to the wrong scratch register
 * 
 * 41   9/25/12 10:04a hauxwell
 * SW7445-47 : add 7445 build support to nexus_graphics3d
 * 
 * 40   9/17/12 1:13p hauxwell
 * SW7435-352 : Nexus interrupt map needs changing to remove programming
 *  of V3D L2
 * 
 * 39   9/7/12 4:05p hauxwell
 * SWVC4-411 : rework nexus_graphics3d
 * 
 * 38   9/7/12 3:39p hauxwell
 * SWVC4-411 : rework nexus_graphics3d
 * 
 * 37   9/5/12 9:48a gsweet
 * SWVC4-408: Allow IDENT registers to be read regardless of acquire state
 * 
 * 36   8/9/12 5:27p erickson
 * SW7425-2734: allow server to limit resources for untrusted clients
 * 
 * 35   7/30/12 5:11p erickson
 * SW7435-279: change module from eLow to eDefault priority
 * 
 * 34   7/18/12 4:21p hauxwell
 * SWVC4-373 : graphics3d prematurely turns the L1 back on if it fails the
 *  Acquire()
 * 
 * 33   7/18/12 9:23a hauxwell
 * SWVC4-358 : graphics3d tryagain process installs multiple callbacks
 * 
 * 32   7/5/12 10:23a katrep
 * SWVC4-356:fix build issue for 7429
 * 
 * 31   7/5/12 12:32p hauxwell
 * SWVC4-356 : power management improvements
 * 
 * 30   6/25/12 3:16p katrep
 * SW7429-1:fix 7429 build issue
 * 
 * 29   6/19/12 12:42p vsilyaev
 * SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
 * 
 * 28   6/12/12 2:17p hauxwell
 * SWVC4-347 : perforce sync
 * 
 * 27   5/28/12 12:07p hauxwell
 * SW7425-3086: expose more internal core registers via nexus API
 * 
 * 26   5/23/12 2:09p hauxwell
 * SW7425-3086: expose more internal core registers via nexus API
 * 
 * 25   5/18/12 8:55a erickson
 * SW7425-3086: extend NEXUS_Graphics3dCommandId, refactor impl
 * 
 * 24   3/6/12 12:12a hauxwell
 * SWVC4-276: PM is not balanced
 * 
 * 23   2/28/12 12:17a hauxwell
 * SW7425-2435: make PM more aggressive
 * 
 * 22   2/24/12 3:36p gmohile
 * SW7425-2435 : Acquire v3d Sram resource
 * 
 * 21   2/16/12 2:18a hauxwell
 * SW7435-36: fix
 * 
 * 20   2/9/12 5:01p mward
 * SW7435-36:  Add 7435.
 * 
 * 19   1/4/12 11:44a dlwin
 * SW7346-625: Address Coverity: 36474, CHECKED_RETURN value.
 * 
 * 18   11/18/11 2:50a hauxwell
 * SWVC4-211: Add SQRSV0 register programming to NEXUS
 * 
 * 17   10/25/11 5:28p katrep
 * SW7429-1:updated for 7429 bringup
 * 
 * 16   9/26/11 4:38p hauxwell
 * SWVC4-119: Big-endian support in V3D
 * 
 * 15   9/22/11 3:00a hauxwell
 * SW7425-1284: add 7425b0 support
 * 
 * 14   8/31/11 10:39a katrep
 * SW7231-317:add suppport for 7231 B0
 * 
 * 13   8/24/11 9:28a erickson
 * SW7346-446: add brcm_Log tag to header file for clearcase checkin
 *  comments
 *
 **************************************************************************/
#include "nexus_graphics3d_module.h"
#include "nexus_graphics3d_impl.h"

#include "priv/nexus_core.h"

#include "bstd.h" /* For BSTD_ENDIAN_BIG define */
#include "bint.h"
#include "bchp_v3d_vpm.h"
#include "bchp_v3d_dbg.h"
#include "bchp_v3d_cle.h"
#include "bchp_v3d_qps.h"
#include "bchp_v3d_ctl.h"
#include "bchp_v3d_gca.h"
#include "bchp_v3d_ptb.h"
#include "bchp_v3d_dbg.h"
#include "bchp_v3d_qps.h"
#include "bchp_v3d_pctr.h"

#if (BCHP_CHIP == 35230)
#include "bchp_v3d_top_gr_bridge.h"
#include "bchp_v3d_pll_ctrl.h"
#include "bchp_hif_cpu_intr1.h"
#elif (BCHP_CHIP == 35233)
#include "bchp_v3d_top_gr_bridge.h"
#include "bchp_int_id_gfx_l2.h"
#include "bchp_hif_cpu_intr1.h"
#elif ((BCHP_CHIP == 7425) && (BCHP_VER >= BCHP_VER_B0))
#include "bchp_v3d_top_gr_bridge.h"
#include "bchp_hif_cpu_intr1.h"
#include "bchp_gfx_gr.h"
#elif (BCHP_CHIP == 7435) || (BCHP_CHIP == 7445)
#include "bchp_v3d_top_gr_bridge.h"
#include "bchp_hif_cpu_intr1.h"
#else
#include "bchp_v3d_pctr.h"
#include "bchp_gfx_gr.h"
#include "bchp_int_id_gfx_l2.h"
#endif

#include "nexus_power_management.h"
#include "nexus_client_resources.h"

/* 35230/3 7425b0 have different interrupt layout for V3D, so they're a special cases */
#if ((BCHP_CHIP==35230) || (BCHP_CHIP==35233))
#define V3D_INTR  BCHP_INT_ID_CREATE(BCHP_V3D_CTL_INTCTL, BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_V3D_CPU_INTR_SHIFT)
#elif (((BCHP_CHIP==7425) && (BCHP_VER>=BCHP_VER_B0)) || (BCHP_CHIP==7435) || (BCHP_CHIP==7445))
/* the L1 */
#define V3D_INTR  (BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_V3D_CPU_INTR_SHIFT+64)
#else
/* Everything else should go through here */
#define V3D_INTR  BCHP_INT_ID_V3D_INTR
#endif

#define CLIENT_TIMEOUT_MS 200
#define ABSTRACT_HW

/* If the register is available to program endianness, assume we are swappable */
#if defined(BCHP_V3D_CTL_ENDSWP)
#define V3D_HAS_ENDIAN_CONTROL
#endif

BDBG_MODULE(nexus_graphics3d);

NEXUS_ModuleHandle NEXUS_P_Graphics3dModule;


typedef struct graphics3d_state
{
   uint32_t                   commandMap[NEXUS_Graphics3dCommandId_eMax];
   NEXUS_Graphics3dHandle     mutexOwner_isr;
   NEXUS_TimerHandle          timer;
   NEXUS_Graphics3dHandle     timerGfx;
   bool                       setEndian;
   uint32_t                   endianValue;
   BINT_CallbackHandle        bintCallback;

   BLST_Q_HEAD(tryAgainList, NEXUS_Graphics3d) tryAgainList;

   /* used for the timeout activity_watcher */
   NEXUS_ThreadHandle         sleepThread;
   bool                       terminate;
   BKNI_EventHandle           cancelTimeout;
   bool                       turnedOn;

} graphics3d_state;

static graphics3d_state gTheOne;

static void InitCommandMap(void);
static void InterruptCallback_isr(void *context, int param);
static void TryAcquireAgain(void *context);
static void TurnOn(NEXUS_Graphics3dHandle gfx);

/******************
 * Module functions
 ******************/

void NEXUS_Graphics3dModule_GetDefaultSettings(NEXUS_Graphics3dModuleSettings __attribute__ ((unused)) *pSettings)
{
   BDBG_ENTER(NEXUS_Graphics3dModule_GetDefaultSettings);
   BDBG_LEAVE(NEXUS_Graphics3dModule_GetDefaultSettings);
}

static void activity_watcher(__attribute__ ((unused)) void * p)
{
   /* this is controlled from NEXUS_Graphics3dModule_Init() */
   while(!gTheOne.terminate)
   {
      BERR_Code err;
      err = BKNI_WaitForEvent(gTheOne.cancelTimeout, 3 * 1000 /* 3 second timeout */);
      /* only timeouts at the end of 3 seconds, wont ordinarily go via here */
      if (err == BERR_TIMEOUT)
      {
         if (__sync_bool_compare_and_swap(&gTheOne.turnedOn, true, false))
         {
            /* timed out, check if the bin/render threads are at the end */
            uint32_t CT0CA, CT1CA, CT0EA, CT1EA;
            CT0CA = BREG_Read32( g_pCoreHandles->reg, BCHP_V3D_CLE_CT0CA );
            CT1CA = BREG_Read32( g_pCoreHandles->reg, BCHP_V3D_CLE_CT1CA );
            CT0EA = BREG_Read32( g_pCoreHandles->reg, BCHP_V3D_CLE_CT0EA );
            CT1EA = BREG_Read32( g_pCoreHandles->reg, BCHP_V3D_CLE_CT1EA );
            if (CT0CA == CT0EA && CT1CA == CT1EA)
            {
               BDBG_MSG(("V3D : PowerDown"));
               /* if the timeout fires, no register access has occurred in 30 seconds, power down
                  *can't be called int a critical section* */
               NEXUS_PowerManagement_SetCoreState(NEXUS_PowerManagementCore_eGraphics3D, false);
               NEXUS_PowerManagement_SetCoreState(NEXUS_PowerManagementCore_eGraphics3DMemory, false);
            }
            else
            {
               /* if the device has moved, keep it switched on */
               (void)__sync_lock_test_and_set(&gTheOne.turnedOn, true);
            }
         }
      }
   }
}

NEXUS_ModuleHandle NEXUS_Graphics3dModule_Init(const NEXUS_Graphics3dModuleSettings __attribute__ ((unused)) *pSettings)
{
   NEXUS_ModuleSettings moduleSettings;
   unsigned int ident0, ident1, ident2;
   bool         defaultBigEndian = false;
   bool         endianSwappable = false;
   bool         wantBigEndian = false;
   char         bcg_version[3];
   NEXUS_ThreadSettings thread_settings;
   int          rc;

   BDBG_ENTER(NEXUS_Graphics3dModule_Init);

   BDBG_ASSERT(!NEXUS_P_Graphics3dModule);

#if BSTD_CPU_ENDIAN == BSTD_ENDIAN_BIG
   wantBigEndian = true;
#endif
   /* init global module handle */
   NEXUS_Module_GetDefaultSettings(&moduleSettings);
   moduleSettings.priority = NEXUS_ModulePriority_eDefault;
   NEXUS_P_Graphics3dModule = NEXUS_Module_Create("graphics3d", &moduleSettings);
   if (!NEXUS_P_Graphics3dModule)
   {
      BDBG_ERR(("Unable to create graphics3d module handle"));
      return NULL;
   }

   BKNI_Memset(&gTheOne, 0, sizeof(gTheOne));

   /* setup the sleep watcher thread */
   BKNI_CreateEvent(&gTheOne.cancelTimeout);
   gTheOne.terminate = false;
   gTheOne.turnedOn = false;
   NEXUS_Thread_GetDefaultSettings(&thread_settings);
   gTheOne.sleepThread = NEXUS_Thread_Create("v3d_sleeper",
                                             activity_watcher,
                                             NULL,
                                             &thread_settings);

   gTheOne.setEndian = false;
   gTheOne.endianValue = 0;

   /* Power on */
   NEXUS_PowerManagement_SetCoreState(NEXUS_PowerManagementCore_eGraphics3DMemory, true);
   NEXUS_PowerManagement_SetCoreState(NEXUS_PowerManagementCore_eGraphics3D, true);

   /* Ensure the command map is populated */
   InitCommandMap();

   ident0 = BREG_Read32( g_pCoreHandles->reg, BCHP_V3D_CTL_IDENT0 );
   ident1 = BREG_Read32( g_pCoreHandles->reg, BCHP_V3D_CTL_IDENT1 );
   ident2 = BREG_Read32( g_pCoreHandles->reg, BCHP_V3D_CTL_IDENT2 );

   bcg_version[0] = ((ident0 >> 24) == 0x2) ? 'A' : 'B';
   bcg_version[1] = (ident1 & 0xF) + '0';
   bcg_version[2] = '\0';

   BDBG_MSG(("V3D - %s", bcg_version));
   BDBG_MSG(("Technology Version 0x%2.2x", ident0 >> 24));
   BDBG_MSG(("Revision 0x%2.2x", ident1 & 0xF));

   if (((ident0 >> 24) == 0x2) && ((ident1 & 0xF) == 0))
   {
      BDBG_MSG(("Number of slices 0x%2.2x", (ident1 >> 4) & 0xF));
      BDBG_MSG(("QPUs per slice 0x%2.2x", (ident1 >> 8) & 0xF));
      BDBG_MSG(("TMUs per slice 0x%2.2x", (ident1 >> 12) & 0xF));
   }
   else
   {
      BDBG_MSG(("Number of slices 0x%2.2x", (ident1 >> 4) & 0xF));
      BDBG_MSG(("QPUs per slice 0x%2.2x", (ident1 >> 8) & 0xF));
      BDBG_MSG(("TMUs per slice 0x%2.2x", (ident1 >> 12) & 0xF));
      BDBG_MSG(("Num semaphores 0x%2.2x", (ident1 >> 16) & 0xFF));
      BDBG_MSG(("HDR available 0x%2.2x", (ident1 >> 24) & 0xF));
      BDBG_MSG(("V3D_VPM_MEMSIZE 0x%2.2x", (ident1 >> 28) & 0xF));
      BDBG_MSG(("V3D_VRI_MEMSIZE 0x%2.2x", ident2 & 0xF));
      BDBG_MSG(("V3D_TLB_L2SZ 0x%2.2x", (ident2 >> 4) & 0xF));
      BDBG_MSG(("V3D_TLB_DBLBUF 0x%2.2x", (ident2 >> 8) & 0xF));

      defaultBigEndian = ((ident2 >> 20) & 0x1) != 0;
      endianSwappable = ((ident2 >> 21) & 0x1) != 0;

      BDBG_MSG(("Default endian %s", defaultBigEndian ? "BIG" : "LITTLE"));
      BDBG_MSG(("Endian swappable %d", endianSwappable));
   }

   /* Init the V3D - with default values */
   BREG_Write32( g_pCoreHandles->reg, BCHP_V3D_GCA_LOW_PRI_ID, 0x00800000 );
   BREG_Write32( g_pCoreHandles->reg, BCHP_V3D_GCA_CACHE_ID, 0x00010002 );

   BREG_Write32( g_pCoreHandles->reg, BCHP_V3D_GCA_CACHE_SWAP_CTRL, 0 );
   BREG_Write32( g_pCoreHandles->reg, BCHP_V3D_GCA_CACHE_SWAP_CTRL_2, 0 );

   BREG_Write32( g_pCoreHandles->reg, BCHP_V3D_GCA_MEM_PROTECT_0, 0 );
   BREG_Write32( g_pCoreHandles->reg, BCHP_V3D_GCA_MEM_PROTECT_1, 0 );
   BREG_Write32( g_pCoreHandles->reg, BCHP_V3D_GCA_MEM_PROTECT_2, 0 );
   BREG_Write32( g_pCoreHandles->reg, BCHP_V3D_GCA_MEM_PROTECT_3, 0 );
   BREG_Write32( g_pCoreHandles->reg, BCHP_V3D_GCA_MEM_PROTECT_4, 0 );
   BREG_Write32( g_pCoreHandles->reg, BCHP_V3D_GCA_MEM_PROTECT_5, 0 );
   BREG_Write32( g_pCoreHandles->reg, BCHP_V3D_GCA_MEM_PROTECT_6, 0 );
   BREG_Write32( g_pCoreHandles->reg, BCHP_V3D_GCA_MEM_PROTECT_7, 0 );

   /* Setup the endian handling (when it's available) */
   /* coverity[dead_error_condition] */
   if (wantBigEndian)
   {
      gTheOne.endianValue = 1;

      if (!defaultBigEndian && !endianSwappable)
         BDBG_MSG(("THIS V3D VARIANT DOES NOT SUPPORT BIG-ENDIAN"));
#ifdef V3D_HAS_ENDIAN_CONTROL
      else if (endianSwappable)
      {
         BDBG_MSG(("SWITCHING TO BIG-ENDIAN"));
         gTheOne.setEndian = true;
         BREG_Write32( g_pCoreHandles->reg, BCHP_V3D_CTL_ENDSWP, gTheOne.endianValue );
      }
#endif
   }
   else
   {
      gTheOne.endianValue = 0;

      /* coverity[dead_error_condition] */
      if (defaultBigEndian && !endianSwappable)
         BDBG_MSG(("THIS V3D VARIANT DOES NOT SUPPORT LITTLE-ENDIAN"));
#ifdef V3D_HAS_ENDIAN_CONTROL
      else if (endianSwappable)
      {
         gTheOne.setEndian = true;
         BREG_Write32( g_pCoreHandles->reg, BCHP_V3D_CTL_ENDSWP, gTheOne.endianValue );
      }
#endif
   }

   /* clear the PM counts */
   BREG_Write32( g_pCoreHandles->reg, BCHP_V3D_GCA_PM_CTRL, 1 );
   BREG_Write32( g_pCoreHandles->reg, BCHP_V3D_GCA_PM_CTRL, 0 );

   BLST_Q_INIT(&gTheOne.tryAgainList);

   /* Turn off core until it is Acquired (dynamic case) */
   NEXUS_PowerManagement_SetCoreState(NEXUS_PowerManagementCore_eGraphics3D, false);
   NEXUS_PowerManagement_SetCoreState(NEXUS_PowerManagementCore_eGraphics3DMemory, false);

#if (((BCHP_CHIP==7425) && (BCHP_VER>=BCHP_VER_B0)) || (BCHP_CHIP==7435) || (BCHP_CHIP==7445))
   /* make sure the L1 is connected */
   rc = NEXUS_Core_ConnectInterrupt(V3D_INTR, InterruptCallback_isr, NULL, 0);
#else
   rc = BINT_CreateCallback(&gTheOne.bintCallback, g_pCoreHandles->bint,
                           V3D_INTR, InterruptCallback_isr, NULL, 0);
#endif
   if (rc)
   {
      rc = BERR_TRACE(rc);
      goto error;
   }

   BDBG_LEAVE(NEXUS_Graphics3dModule_Init);

   return NEXUS_P_Graphics3dModule;

error:
    /* TODO: clean up */
    return NULL;
}

void NEXUS_Graphics3dModule_Uninit()
{
   BDBG_ENTER(NEXUS_Graphics3dModule_Uninit);

   /* tell the thread to exit */
   gTheOne.terminate = true;
   BKNI_SetEvent(gTheOne.cancelTimeout);
   if (__sync_bool_compare_and_swap(&gTheOne.turnedOn, true, false))
   {
      BDBG_MSG(("V3D : PowerDown"));
      /* if the timeout fires, no register access has occurred in 30 seconds, power down
         *can't be called int a critical section* */
      NEXUS_PowerManagement_SetCoreState(NEXUS_PowerManagementCore_eGraphics3D, false);
      NEXUS_PowerManagement_SetCoreState(NEXUS_PowerManagementCore_eGraphics3DMemory, false);
   }
   NEXUS_Thread_Destroy(gTheOne.sleepThread);
   BKNI_DestroyEvent(gTheOne.cancelTimeout);
#if (((BCHP_CHIP==7425) && (BCHP_VER>=BCHP_VER_B0)) || (BCHP_CHIP==7435) || (BCHP_CHIP==7445))
   /* make sure the L1 is disconnected */
   NEXUS_Core_DisconnectInterrupt(V3D_INTR);
#else
   BINT_DestroyCallback(gTheOne.bintCallback);
#endif

   NEXUS_Module_Destroy(NEXUS_P_Graphics3dModule);
   NEXUS_P_Graphics3dModule = NULL;

   BDBG_LEAVE(NEXUS_Graphics3dModule_Uninit);
}

void NEXUS_Graphics3d_GetDefaultOpenSettings(NEXUS_Graphics3dOpenSettings *pSettings)
{
   BDBG_ENTER(NEXUS_Graphics3d_GetDefaultOpenSettings);

   BKNI_Memset(pSettings, 0, sizeof(*pSettings));
   NEXUS_CallbackDesc_Init(&pSettings->controlCallback);
   NEXUS_CallbackDesc_Init(&pSettings->tryAgainCallback);

   BDBG_LEAVE(NEXUS_Graphics3d_GetDefaultOpenSettings);
}

void NEXUS_Graphics3d_GetDefaultAcquireSettings(NEXUS_Graphics3dAcquireSettings *pSettings)
{
   BDBG_ENTER(NEXUS_Graphics3d_GetDefaultAcquireSettings);

   BKNI_Memset(pSettings, 0, sizeof(*pSettings));

   BDBG_LEAVE(NEXUS_Graphics3d_GetDefaultAcquireSettings);
}

void NEXUS_Graphics3d_GetDefaultCommand(NEXUS_Graphics3dCommand *pCommand)
{
   BDBG_ENTER(NEXUS_Graphics3d_GetDefaultCommandSettings);

   BKNI_Memset(pCommand, 0, sizeof(*pCommand));

   BDBG_LEAVE(NEXUS_Graphics3d_GetDefaultCommandSettings);
}

NEXUS_Graphics3dHandle NEXUS_Graphics3d_Open(const NEXUS_Graphics3dOpenSettings *pSettings)
{
   int rc;
   NEXUS_Graphics3dHandle         gfx;

   BDBG_ENTER(NEXUS_Graphics3d_Open);

    rc = NEXUS_CLIENT_RESOURCES_ACQUIRE(graphics3d,Count,NEXUS_ANY_ID);
    if (rc) { rc = BERR_TRACE(rc); return NULL; }
    
   gfx = BKNI_Malloc(sizeof(*gfx));
   if (!gfx)
   {
      NEXUS_CLIENT_RESOURCES_RELEASE(graphics3d,Count,NEXUS_ANY_ID);
      BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);
      return NULL;
   }
   NEXUS_OBJECT_INIT(NEXUS_Graphics3d, gfx);

   gfx->magic = MAGIC;
   gfx->openSettings = *pSettings;

   gfx->tryAgainHandler = NEXUS_TaskCallback_Create(gfx, NULL);
   if (!gfx->tryAgainHandler)
   {
      BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);
      goto error;
   }

   gfx->mainISR = NEXUS_IsrCallback_Create(gfx, NULL);
   if (!gfx->mainISR)
   {
      BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);
      goto error;
   }

   if (pSettings->tryAgainCallback.callback)
      NEXUS_TaskCallback_Set(gfx->tryAgainHandler, &pSettings->tryAgainCallback);

   if (pSettings->controlCallback.callback)
      NEXUS_IsrCallback_Set(gfx->mainISR, &pSettings->controlCallback);
   BDBG_LEAVE(NEXUS_Graphics3d_Open);

   return gfx;
error:
   BDBG_LEAVE(NEXUS_Graphics3d_Open);

   NEXUS_Graphics3d_Close(gfx);
   return NULL;
}

#if 0 /* Useful for debugging */
static void DumpQueue(void)
{
   NEXUS_Graphics3dHandle gfx;
   BKNI_Printf("Q = ");
   for (gfx = BLST_Q_FIRST(&gTheOne.tryAgainList); gfx != NULL; gfx = BLST_Q_NEXT(gfx, node))
   {
      BKNI_Printf("%p, ", gfx);
   }
   BKNI_Printf("\n");
}
#endif

static void StripFromTryAgainList(NEXUS_Graphics3dHandle gfx)
{
   NEXUS_Graphics3dHandle search;

   /* Strip gfx from the tryAgainList */
   for (search = BLST_Q_FIRST(&gTheOne.tryAgainList); search != NULL; )
   {
      NEXUS_Graphics3dHandle next = BLST_Q_NEXT(search, node);
      if (search == gfx)
      {
         BLST_Q_REMOVE(&gTheOne.tryAgainList, search, node);
         break;
      }
      search = next;
   }
}

static void FireNextWaiting(void)
{
    /* Fire the next waiting tryAgain callback */
    NEXUS_Graphics3dHandle gfx;
    gfx = BLST_Q_FIRST(&gTheOne.tryAgainList);
    if (gfx)
    {
        NEXUS_OBJECT_ASSERT(NEXUS_Graphics3d, gfx);
        BLST_Q_REMOVE_HEAD(&gTheOne.tryAgainList, node);

        TryAcquireAgain(gfx);
    }
}

static void NEXUS_Graphics3d_P_Finalizer(NEXUS_Graphics3dHandle gfx)
{
   BDBG_ENTER(NEXUS_Graphics3d_Close);
   NEXUS_OBJECT_ASSERT(NEXUS_Graphics3d, gfx);

   if (gTheOne.timer != NULL && gfx == gTheOne.timerGfx)
   {
       NEXUS_CancelTimer(gTheOne.timer);
       gTheOne.timer = NULL;
   }

   if (gfx->mainISR != NULL) 
   {
      NEXUS_IsrCallback_Destroy(gfx->mainISR);
      gfx->mainISR = NULL;
   }

   if (gfx->tryAgainHandler != NULL)
   {
      NEXUS_TaskCallback_Destroy(gfx->tryAgainHandler);
      gfx->tryAgainHandler = NULL;
   }
   /* Strip gfx from the tryAgainList */
   StripFromTryAgainList(gfx);

   if (gTheOne.mutexOwner_isr == gfx)
   {
      gfx->refCount = 1; /* Ensure it releases */
      NEXUS_Graphics3d_Release(gfx);
   }

   FireNextWaiting();

   NEXUS_CLIENT_RESOURCES_RELEASE(graphics3d,Count,NEXUS_ANY_ID);
   NEXUS_OBJECT_DESTROY(NEXUS_Graphics3d, gfx);
   BKNI_Free(gfx);

   BDBG_LEAVE(NEXUS_Graphics3d_Close);
   return;
}

NEXUS_OBJECT_CLASS_MAKE(NEXUS_Graphics3d, NEXUS_Graphics3d_Close);

static void AppendTryAgain(NEXUS_Graphics3dHandle gfx)
{
   BLST_Q_INSERT_TAIL(&gTheOne.tryAgainList, gfx, node);
}

NEXUS_Error NEXUS_Graphics3d_Acquire(NEXUS_Graphics3dHandle gfx, 
                                     const NEXUS_Graphics3dAcquireSettings *pSettings)
{
#if !(((BCHP_CHIP==7425) && (BCHP_VER>=BCHP_VER_B0)) || (BCHP_CHIP==7435) || (BCHP_CHIP==7445))
   BERR_Code    err;
#endif

   BDBG_OBJECT_ASSERT(gfx, NEXUS_Graphics3d);

   BDBG_ENTER(NEXUS_Graphics3d_Acquire);

   if (pSettings == NULL)
      return NEXUS_INVALID_PARAMETER;

   /* cancel the timeout prior to modifying the power state */
   BKNI_SetEvent(gTheOne.cancelTimeout);
   if (__sync_bool_compare_and_swap(&gTheOne.turnedOn, false, true))
   {
      BDBG_MSG(("V3D : PowerUp"));
      /* HW is turned off and we are just about to use it, turn it on
         *can't be called int a critical section* */
      NEXUS_PowerManagement_SetCoreState(NEXUS_PowerManagementCore_eGraphics3D, true);
      NEXUS_PowerManagement_SetCoreState(NEXUS_PowerManagementCore_eGraphics3DMemory, true);
   }

   /* Kill the timer that is checking for broken clients */
   if (gTheOne.timer != NULL && gTheOne.timerGfx == gfx)
   {
      NEXUS_CancelTimer(gTheOne.timer);
      gTheOne.timer = NULL;
      gTheOne.timerGfx = NULL;
   }

   /* Only one client at a time */
   if (__sync_fetch_and_and(&gTheOne.mutexOwner_isr, 0xFFFFFFFF))
   {
      if (gTheOne.mutexOwner_isr == gfx)
      {
         /* Optimization for single-process model :
            If this client currently has the lock, and there are no clients in the queue, we will
            give it a reference counted lock */
         if (BLST_Q_EMPTY(&gTheOne.tryAgainList))
         {
            gfx->refCount++;
            return NEXUS_SUCCESS;
         }
      }

      /* We can't get the acquire lock yet, so add us to the queue */
      AppendTryAgain(gfx);
      return NEXUS_NOT_AVAILABLE;
   }

   /* Must test and set before enabling the interrupt, otherwise we have a race. */
   (void)__sync_lock_test_and_set(&gTheOne.mutexOwner_isr, gfx);

#if (((BCHP_CHIP==7425) && (BCHP_VER>=BCHP_VER_B0)) || (BCHP_CHIP==7435) || (BCHP_CHIP==7445))
   /* make sure the L1 is enabled */
   NEXUS_Core_EnableInterrupt(V3D_INTR);
#else
   if (BINT_ClearCallback(gTheOne.bintCallback) == BERR_SUCCESS)
   {
#if (!((BCHP_CHIP==35230) || (BCHP_CHIP==35233)))
      err = BINT_EnableCallback(gTheOne.bintCallback);
      if (err)
      {
         err=BERR_TRACE(err);
         return err;
      }
#endif
   }
#endif

   gfx->refCount++;
   gfx->acqSettings = *pSettings;

   TurnOn(gfx);

   BDBG_LEAVE(NEXUS_Graphics3d_Acquire);

   return NEXUS_SUCCESS;
}

NEXUS_Error NEXUS_Graphics3d_Release(NEXUS_Graphics3dHandle gfx)
{
   NEXUS_Error res;

   BDBG_OBJECT_ASSERT(gfx, NEXUS_Graphics3d);

   BDBG_ENTER(NEXUS_Graphics3d_Release);

   gfx->refCount--;
   if (gfx->refCount == 0)
   {
#if (((BCHP_CHIP==7425) && (BCHP_VER>=BCHP_VER_B0)) || (BCHP_CHIP==7435) || (BCHP_CHIP==7445))
      NEXUS_Core_DisableInterrupt(V3D_INTR);
#else
      BINT_DisableCallback(gTheOne.bintCallback);
#endif

      (void)__sync_lock_test_and_set(&gTheOne.mutexOwner_isr, NULL);

      /* Fire the next waiting tryAgain callback */
      FireNextWaiting();

      res = NEXUS_SUCCESS;
   }
   else if (gfx->refCount > 0)
   {
      res = NEXUS_NOT_AVAILABLE;
   }
   else if (gfx->refCount < 0)
   {
      res = NEXUS_INVALID_PARAMETER;
   }

   BDBG_LEAVE(NEXUS_Graphics3d_Release);

   return res;
}

NEXUS_Error NEXUS_Graphics3d_GetStatus(NEXUS_Graphics3dHandle gfx, NEXUS_Graphics3dCommandId statusId, NEXUS_Graphics3dStatus *pStatus)
{
   uint32_t res = 0;

   /* Reads can 'occasionally' happen outside of an acquire due to timing issues, and are non-destructive, 
      so we will allow this, but return 0, since we don't have the core.
      BDBG_OBJECT_ASSERT(gfx, NEXUS_Graphics3d);
   */
   BDBG_ENTER(NEXUS_Graphics3d_Status);

   /* We do allow reads of the IDENT registers when not acquired */
   if (gfx->magic == MAGIC && (gTheOne.mutexOwner_isr == gfx || 
                               statusId == NEXUS_Graphics3dCommandId_eIDENT0 || 
                               statusId == NEXUS_Graphics3dCommandId_eIDENT1 || 
                               statusId == NEXUS_Graphics3dCommandId_eIDENT2 || 
                               statusId == NEXUS_Graphics3dCommandId_eIDENT3))
   {
      if (statusId > NEXUS_Graphics3dCommandId_eNO_COMMAND && statusId < NEXUS_Graphics3dCommandId_eMax)
      {
         if (__sync_bool_compare_and_swap(&gTheOne.turnedOn, false, true))
         {
            BDBG_MSG(("V3D : PowerUp"));
            /* cancel the timeout prior to modifying the power state */
            BKNI_SetEvent(gTheOne.cancelTimeout);
            /* HW is turned off and we are just about to use it, turn it on
               *can't be called int a critical section* */
            NEXUS_PowerManagement_SetCoreState(NEXUS_PowerManagementCore_eGraphics3D, true);
            NEXUS_PowerManagement_SetCoreState(NEXUS_PowerManagementCore_eGraphics3DMemory, true);
         }

         res = BREG_Read32(g_pCoreHandles->reg, gTheOne.commandMap[statusId]);
      }
      else
      {
         BDBG_WRN(("NEXUS_Graphics3d_Status ARG OUT OF BOUNDS\n"));
         return BERR_TRACE(NEXUS_INVALID_PARAMETER);
      }
   }

   BDBG_LEAVE(NEXUS_Graphics3d_Status);

   if (pStatus != NULL)
      pStatus->status = res;

   return NEXUS_SUCCESS;
}

NEXUS_Error NEXUS_Graphics3d_SendCommand(NEXUS_Graphics3dHandle gfx, const NEXUS_Graphics3dCommand *pCommand)
{
   BDBG_OBJECT_ASSERT(gfx, NEXUS_Graphics3d);

   BDBG_ENTER(NEXUS_Graphics3d_Command);

   if (pCommand == NULL)
      return BERR_TRACE(NEXUS_INVALID_PARAMETER);

   if (pCommand->commandId > NEXUS_Graphics3dCommandId_eNO_COMMAND && pCommand->commandId < NEXUS_Graphics3dCommandId_eMax)
   {
      if (__sync_bool_compare_and_swap(&gTheOne.turnedOn, false, true))
      {
         BDBG_MSG(("V3D : PowerUp"));
         /* cancel the timeout prior to modifying the power state */
         BKNI_SetEvent(gTheOne.cancelTimeout);
         /* HW is turned off and we are just about to use it, turn it on
            *can't be called int a critical section* */
         NEXUS_PowerManagement_SetCoreState(NEXUS_PowerManagementCore_eGraphics3D, true);
         NEXUS_PowerManagement_SetCoreState(NEXUS_PowerManagementCore_eGraphics3DMemory, true);
      }

      if (pCommand->commandId == NEXUS_Graphics3dCommandId_eINTDONE)
      {
#if (((BCHP_CHIP==7425) && (BCHP_VER>=BCHP_VER_B0)) || (BCHP_CHIP==7435) || (BCHP_CHIP==7445))
          if (gfx == gTheOne.mutexOwner_isr)
          {
             NEXUS_Core_EnableInterrupt(V3D_INTR);
          }
#endif
      }
      else
      {
         BREG_Write32(g_pCoreHandles->reg, gTheOne.commandMap[pCommand->commandId], pCommand->data);
      }
   }
   else
   {
      BDBG_WRN(("NEXUS_Graphics3d_Command ARG OUT OF BOUNDS\n"));
      return BERR_TRACE(NEXUS_INVALID_PARAMETER);
   }

   BDBG_LEAVE(NEXUS_Graphics3d_Command);

   return NEXUS_SUCCESS;
}

static void InterruptCallback_isr(void *context, int param)
{
   NEXUS_Graphics3dHandle gfx = gTheOne.mutexOwner_isr;

   BDBG_ENTER(InterruptCallback);

   BSTD_UNUSED(context);
   BSTD_UNUSED(param);

   /* Fire off the isr callback */
#if (((BCHP_CHIP==7425) && (BCHP_VER>=BCHP_VER_B0)) || (BCHP_CHIP==7435) || (BCHP_CHIP==7445))
   NEXUS_Core_DisableInterrupt_isr(V3D_INTR);
#endif

   if (gfx && gfx->mainISR != NULL)
   {
      NEXUS_IsrCallback_Fire_isr(gfx->mainISR);
   }
   else
      BDBG_WRN(("UNVECTORED IRQ\n"));

   BDBG_LEAVE(InterruptCallback);
}

static void TimerCallback(void *context)
{
   NEXUS_Graphics3dHandle gfx = (NEXUS_Graphics3dHandle)context;

   BDBG_ENTER(TimerCallback);

   /* allow NEXUS to re-add another timer callback */
   gTheOne.timer = NULL;

   /* Put us back in the waiting list */
   if (gfx != NULL && gfx->magic == MAGIC)
      AppendTryAgain(gfx);

   /* Fire the next waiting tryAgain callback */
   FireNextWaiting();
   BDBG_LEAVE(TimerCallback);
}

static void TryAcquireAgain(void *context)
{
   NEXUS_Graphics3dHandle gfx = (NEXUS_Graphics3dHandle)context;

   BDBG_ENTER(TryAcquireAgain);

   /* Fire off the try again callback */
   if (gfx != NULL && gfx->magic == MAGIC && gfx->tryAgainHandler != NULL)
   {
      NEXUS_TaskCallback_Fire(gfx->tryAgainHandler);
   }
   else
      BDBG_ERR(("Losing a TryAgain callback"));

   /* Start a timer to catch the case where we fire this callback, but the client dies before
      being able to send the next acquire. In that case we would never receive another acquire,
      so if this timer triggers before we cancel it in acquire, that's the case we hit. We will
      simply fire the next queued client at that point. */
   gTheOne.timerGfx = gfx;

   /* NEXUS scheduler cannot have more than one timer pending, state machine gets in a mess */
   if (gTheOne.timer == NULL)
   {
      gTheOne.timer = NEXUS_ScheduleTimer(CLIENT_TIMEOUT_MS, TimerCallback, gfx);
      BDBG_ASSERT(gTheOne.timer);
   }

   BDBG_LEAVE(TryAcquireAgain);
}


static void InitCommandMap(void)
{
   /* Initialize the command map (once only) */
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_eNO_COMMAND]         = 0;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_eL2CACTL]            = BCHP_V3D_CTL_L2CACTL;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_eSLCACTL]            = BCHP_V3D_CTL_SLCACTL;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_eINTCTL]             = BCHP_V3D_CTL_INTCTL;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_eINTENA]             = BCHP_V3D_CTL_INTENA;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_eINTDIS]             = BCHP_V3D_CTL_INTDIS;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_eCT0CS]              = BCHP_V3D_CLE_CT0CS;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_eCT1CS]              = BCHP_V3D_CLE_CT1CS;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_eCT0EA]              = BCHP_V3D_CLE_CT0EA;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_eCT1EA]              = BCHP_V3D_CLE_CT1EA;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_eCT0CA]              = BCHP_V3D_CLE_CT0CA;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_eCT1CA]              = BCHP_V3D_CLE_CT1CA;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_ePCS]                = BCHP_V3D_CLE_PCS;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_eBFC]                = BCHP_V3D_CLE_BFC;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_eRFC]                = BCHP_V3D_CLE_RFC;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_eBPOA]               = BCHP_V3D_PTB_BPOA;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_eBPOS]               = BCHP_V3D_PTB_BPOS;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_eCACHE_CTRL]         = BCHP_V3D_GCA_CACHE_CTRL;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_ePCTRC]              = BCHP_V3D_PCTR_PCTRC;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_ePCTRE]              = BCHP_V3D_PCTR_PCTRE;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_ePCTR0]              = BCHP_V3D_PCTR_PCTR0;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_ePCTRS0]             = BCHP_V3D_PCTR_PCTRS0;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_ePCTR1]              = BCHP_V3D_PCTR_PCTR1;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_ePCTRS1]             = BCHP_V3D_PCTR_PCTRS1;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_ePCTR2]              = BCHP_V3D_PCTR_PCTR2;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_ePCTRS2]             = BCHP_V3D_PCTR_PCTRS2;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_ePCTR3]              = BCHP_V3D_PCTR_PCTR3;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_ePCTRS3]             = BCHP_V3D_PCTR_PCTRS3;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_ePCTR4]              = BCHP_V3D_PCTR_PCTR4;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_ePCTRS4]             = BCHP_V3D_PCTR_PCTRS4;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_ePCTR5]              = BCHP_V3D_PCTR_PCTR5;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_ePCTRS5]             = BCHP_V3D_PCTR_PCTRS5;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_ePCTR6]              = BCHP_V3D_PCTR_PCTR6;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_ePCTRS6]             = BCHP_V3D_PCTR_PCTRS6;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_ePCTR7]              = BCHP_V3D_PCTR_PCTR7;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_ePCTRS7]             = BCHP_V3D_PCTR_PCTRS7;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_ePCTR8]              = BCHP_V3D_PCTR_PCTR8;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_ePCTRS8]             = BCHP_V3D_PCTR_PCTRS8;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_ePCTR9]              = BCHP_V3D_PCTR_PCTR9;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_ePCTRS9]             = BCHP_V3D_PCTR_PCTRS9;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_ePCTR10]             = BCHP_V3D_PCTR_PCTR10;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_ePCTRS10]            = BCHP_V3D_PCTR_PCTRS10;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_ePCTR11]             = BCHP_V3D_PCTR_PCTR11;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_ePCTRS11]            = BCHP_V3D_PCTR_PCTRS11;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_ePCTR12]             = BCHP_V3D_PCTR_PCTR12;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_ePCTRS12]            = BCHP_V3D_PCTR_PCTRS12;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_ePCTR13]             = BCHP_V3D_PCTR_PCTR13;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_ePCTRS13]            = BCHP_V3D_PCTR_PCTRS13;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_ePCTR14]             = BCHP_V3D_PCTR_PCTR14;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_ePCTRS14]            = BCHP_V3D_PCTR_PCTRS14;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_ePCTR15]             = BCHP_V3D_PCTR_PCTR15;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_ePCTRS15]            = BCHP_V3D_PCTR_PCTRS15;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_eGCA_PM_SEL]         = BCHP_V3D_GCA_PM_CTRL;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_eGCA_BW_CNT]         = BCHP_V3D_GCA_V3D_BW_CNT;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_eGCA_BW_MEM_CNT]     = BCHP_V3D_GCA_MEM_BW_CNT;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_eIDENT0]             = BCHP_V3D_CTL_IDENT0;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_eIDENT1]             = BCHP_V3D_CTL_IDENT1;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_eIDENT2]             = BCHP_V3D_CTL_IDENT2;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_eSQRSV0]             = BCHP_V3D_QPS_SQRSV0;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_eIDENT3]             = BCHP_V3D_CTL_IDENT3;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_eSQRSV1]             = BCHP_V3D_QPS_SQRSV1;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_eSQCNTL]             = BCHP_V3D_QPS_SQCNTL;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_eSQCSTAT]            = BCHP_V3D_QPS_SQCSTAT;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_eSRQPC]              = BCHP_V3D_QPS_SRQPC;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_eSRQUA]              = BCHP_V3D_QPS_SRQUA;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_eSRQUL]              = BCHP_V3D_QPS_SRQUL;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_eSRQCS]              = BCHP_V3D_QPS_SRQCS;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_eSCRATCH]            = BCHP_V3D_CTL_SCRATCH;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_eENDSWP]             = BCHP_V3D_CTL_ENDSWP;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_eVPACNTL]            = BCHP_V3D_VPM_VPACNTL;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_eVPMBASE]            = BCHP_V3D_VPM_VPMBASE;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_eBPCA]               = BCHP_V3D_PTB_BPCA;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_eBPCS]               = BCHP_V3D_PTB_BPCS;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_eBXCF]               = BCHP_V3D_PTB_BXCF;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_eCACHE_ID]           = BCHP_V3D_GCA_CACHE_ID;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_eCACHE_SWAP_CTRL]    = BCHP_V3D_GCA_CACHE_SWAP_CTRL;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_eCACHE_SWAP_CTRL_2]  = BCHP_V3D_GCA_CACHE_SWAP_CTRL_2;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_eCACHE_STATUS]       = BCHP_V3D_GCA_CACHE_STATUS;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_ePM_CTRL]            = BCHP_V3D_GCA_PM_CTRL;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_eV3D_BW_CNT]         = BCHP_V3D_GCA_V3D_BW_CNT;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_eMEM_BW_CNT]         = BCHP_V3D_GCA_MEM_BW_CNT;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_eLOW_PRI_ID]         = BCHP_V3D_GCA_LOW_PRI_ID;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_eMEM_PROTECT_0]      = BCHP_V3D_GCA_MEM_PROTECT_0;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_eMEM_PROTECT_1]      = BCHP_V3D_GCA_MEM_PROTECT_1;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_eMEM_PROTECT_2]      = BCHP_V3D_GCA_MEM_PROTECT_2;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_eMEM_PROTECT_3]      = BCHP_V3D_GCA_MEM_PROTECT_3;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_eMEM_PROTECT_4]      = BCHP_V3D_GCA_MEM_PROTECT_4;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_eMEM_PROTECT_5]      = BCHP_V3D_GCA_MEM_PROTECT_5;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_eMEM_PROTECT_6]      = BCHP_V3D_GCA_MEM_PROTECT_6;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_eMEM_PROTECT_7]      = BCHP_V3D_GCA_MEM_PROTECT_7;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_eMEM_PROTECT_STATUS] = BCHP_V3D_GCA_MEM_PROTECT_STATUS;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_eAXI_BRIDGE_STATUS]  = BCHP_V3D_GCA_AXI_BRIDGE_STATUS;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_eSCRATCH_0]          = BCHP_V3D_GCA_SCRATCH_0;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_eSAFE_SHUTDOWN]      = BCHP_V3D_GCA_SAFE_SHUTDOWN;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_eSAFE_SHUTDOWN_ACK]  = BCHP_V3D_GCA_SAFE_SHUTDOWN_ACK;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_eDBCFG]              = BCHP_V3D_DBG_DBCFG;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_eDBSCS]              = BCHP_V3D_DBG_DBSCS;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_eDBSCFG]             = BCHP_V3D_DBG_DBSCFG;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_eDBSSR]              = BCHP_V3D_DBG_DBSSR;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_eDBSDR0]             = BCHP_V3D_DBG_DBSDR0;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_eDBSDR1]             = BCHP_V3D_DBG_DBSDR1;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_eDBSDR2]             = BCHP_V3D_DBG_DBSDR2;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_eDBSDR3]             = BCHP_V3D_DBG_DBSDR3;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_eDBQRUN]             = BCHP_V3D_DBG_DBQRUN;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_eDBQHLT]             = BCHP_V3D_DBG_DBQHLT;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_eDBQSTP]             = BCHP_V3D_DBG_DBQSTP;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_eDBQITE]             = BCHP_V3D_DBG_DBQITE;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_eDBQITC]             = BCHP_V3D_DBG_DBQITC;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_eDBQGHC]             = BCHP_V3D_DBG_DBQGHC;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_eDBQGHG]             = BCHP_V3D_DBG_DBQGHG;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_eDBQGHH]             = BCHP_V3D_DBG_DBQGHH;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_eDBGE]               = BCHP_V3D_DBG_DBGE;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_eFDBGO]              = BCHP_V3D_DBG_FDBG0;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_eFDBGB]              = BCHP_V3D_DBG_FDBGB;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_eFDBGR]              = BCHP_V3D_DBG_FDBGR;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_eFDBGS]              = BCHP_V3D_DBG_FDBGS;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_eERRSTAT]            = BCHP_V3D_DBG_ERRSTAT;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_eCT00RA0]            = BCHP_V3D_CLE_CT00RA0;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_eCT01RA0]            = BCHP_V3D_CLE_CT01RA0;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_eCT0LC]              = BCHP_V3D_CLE_CT0LC;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_eCT1LC]              = BCHP_V3D_CLE_CT1LC;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_eCT0PC]              = BCHP_V3D_CLE_CT0PC;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_eCT1PC]              = BCHP_V3D_CLE_CT1PC;
#if (BCHP_CHIP==35233 || ((BCHP_CHIP==7425) && (BCHP_VER>=BCHP_VER_B0)) || BCHP_CHIP==7435) || (BCHP_CHIP==7445)
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_eBRIDGE_REVISION]    = BCHP_V3D_TOP_GR_BRIDGE_REVISION;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_eBRIDGE_CTRL]        = BCHP_V3D_TOP_GR_BRIDGE_CTRL;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_eBRIDGE_SW_INIT_0]   = BCHP_V3D_TOP_GR_BRIDGE_SW_INIT_0;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_eBRIDGE_SW_INIT_1]   = BCHP_V3D_TOP_GR_BRIDGE_SW_INIT_1;
#else
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_eBRIDGE_REVISION]    = BCHP_GFX_GR_REVISION;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_eBRIDGE_CTRL]        = BCHP_GFX_GR_CTRL;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_eBRIDGE_SW_INIT_0]   = BCHP_GFX_GR_SW_INIT_0;
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_eBRIDGE_SW_INIT_1]   = BCHP_GFX_GR_SW_INIT_1;
#endif
   gTheOne.commandMap[NEXUS_Graphics3dCommandId_eINTDONE]            = 0; /* dummy command */
}

static void TurnOn( NEXUS_Graphics3dHandle gfx )
{
   unsigned int ct0ca, ct0ea;
   unsigned int ct1ca, ct1ea;

#if 0
   /* turn on the PLL's, power up */
   r = BREG_Read32( g_pCoreHandles->reg, V3D_PLL_CTRL_PLL_6CH_CTRL_OUT_CH0 );
   BREG_Write32( g_pCoreHandles->reg, V3D_PLL_CTRL_PLL_6CH_CTRL_OUT_CH0, r & ~0x10 );      /* clear bit 8 for CH power up */
   r = BREG_Read32( g_pCoreHandles->reg, V3D_PLL_CTRL_PLL_6CH_CTRL_OUT_CH1 );
   BREG_Write32( g_pCoreHandles->reg, V3D_PLL_CTRL_PLL_6CH_CTRL_OUT_CH1, r & ~0x10 );      /* clear bit 8 for CH power up */
   r = BREG_Read32( g_pCoreHandles->reg, V3D_PLL_CTRL_PLL_6CH_CTRL_OUT_CH2 );
   BREG_Write32( g_pCoreHandles->reg, V3D_PLL_CTRL_PLL_6CH_CTRL_OUT_CH2, r & ~0x10 );      /* clear bit 8 for CH power up */
   r = BREG_Read32( g_pCoreHandles->reg, V3D_PLL_CTRL_PLL_6CH_CTRL_OUT_CH3 );
   BREG_Write32( g_pCoreHandles->reg, V3D_PLL_CTRL_PLL_6CH_CTRL_OUT_CH3, r & ~0x10 );      /* clear bit 8 for CH power up */
   r = BREG_Read32( g_pCoreHandles->reg, V3D_PLL_CTRL_PLL_6CH_CTRL_OUT_CH4 );
   BREG_Write32( g_pCoreHandles->reg, V3D_PLL_CTRL_PLL_6CH_CTRL_OUT_CH4, r & ~0x10 );      /* clear bit 8 for CH power up */
   r = BREG_Read32( g_pCoreHandles->reg, V3D_PLL_CTRL_PLL_6CH_CTRL_OUT_CH5 );
   BREG_Write32( g_pCoreHandles->reg, V3D_PLL_CTRL_PLL_6CH_CTRL_OUT_CH5, r & ~0x10 );      /* clear bit 8 for CH power up */
#endif

   ct0ca = BREG_Read32( g_pCoreHandles->reg, BCHP_V3D_CLE_CT0CA );
   ct0ea = BREG_Read32( g_pCoreHandles->reg, BCHP_V3D_CLE_CT0EA );
   ct1ca = BREG_Read32( g_pCoreHandles->reg, BCHP_V3D_CLE_CT1CA );
   ct1ea = BREG_Read32( g_pCoreHandles->reg, BCHP_V3D_CLE_CT1EA );
   if ( ct0ca != ct0ea )
      BDBG_ERR(("RESET when ct0ca != ct0ea (%p, %p)", ct0ca, ct0ea));
   if ( ct1ca != ct1ea )
      BDBG_ERR(("RESET when ct1ca != ct1ea (%p, %p)", ct1ca, ct1ea));

   /* need to reset, VPM can only be reprogrammed after a reset */
#if (BCHP_CHIP==35230)
   BREG_Write32( g_pCoreHandles->reg, BCHP_V3D_TOP_GR_BRIDGE_SW_RESET_0, 7 );      /* v3d + clk_216 + clk_108 */
   BREG_Write32( g_pCoreHandles->reg, BCHP_V3D_TOP_GR_BRIDGE_SW_RESET_0, 0 );      /* deassert to make run again */
#elif (BCHP_CHIP==35233 || ((BCHP_CHIP==7425) && (BCHP_VER>=BCHP_VER_B0)) || BCHP_CHIP==7435 || (BCHP_CHIP==7445))
   BREG_Write32(g_pCoreHandles->reg, BCHP_V3D_TOP_GR_BRIDGE_SW_INIT_0, BCHP_FIELD_ENUM(V3D_TOP_GR_BRIDGE_SW_INIT_0, V3D_CLK_108_SW_INIT, ASSERT) );
   BREG_Write32(g_pCoreHandles->reg, BCHP_V3D_TOP_GR_BRIDGE_SW_INIT_0, BCHP_FIELD_ENUM(V3D_TOP_GR_BRIDGE_SW_INIT_0, V3D_CLK_108_SW_INIT, DEASSERT) );
#elif ((BCHP_VER == BCHP_VER_A0) || (BCHP_CHIP==7231) || (BCHP_CHIP==7584)) && (BCHP_CHIP!=7429)
   BREG_Write32(g_pCoreHandles->reg, BCHP_GFX_GR_SW_INIT_0, BCHP_FIELD_ENUM(GFX_GR_SW_INIT_0, V3D_CLK_108_SW_INIT, ASSERT) );
   BREG_Write32(g_pCoreHandles->reg, BCHP_GFX_GR_SW_INIT_0, BCHP_FIELD_ENUM(GFX_GR_SW_INIT_0, V3D_CLK_108_SW_INIT, DEASSERT) );
#else
   BREG_Write32(g_pCoreHandles->reg, BCHP_GFX_GR_SW_INIT_0, BCHP_FIELD_ENUM(GFX_GR_SW_INIT_0, V3D_TOP_CLK_108_SW_INIT, ASSERT) );
   BREG_Write32(g_pCoreHandles->reg, BCHP_GFX_GR_SW_INIT_0, BCHP_FIELD_ENUM(GFX_GR_SW_INIT_0, V3D_TOP_CLK_108_SW_INIT, DEASSERT) );
#endif

#ifdef V3D_HAS_ENDIAN_CONTROL
  if (gTheOne.setEndian)
  {
     /* ENDSWP is reset after the reset, so ensure it's correctly set again */
     BREG_Write32( g_pCoreHandles->reg, BCHP_V3D_CTL_ENDSWP, gTheOne.endianValue );
  }
#endif

   BREG_Write32( g_pCoreHandles->reg, BCHP_V3D_VPM_VPMBASE, gfx->acqSettings.userVertexPipeMemory );
}
