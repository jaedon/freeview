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
 * $brcm_Workfile: p3d_nexus_proxy.c $
 * $brcm_Revision:
 * $brcm_Date: 11/25/11 2:47p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /AppLibs/opensource/directfb/src/broadcom_files/private/DirectFB/1.4.15/platform/p3d_nexus_proxy.c $
 *
 * DirectFB_1_4_15_Port/2   11/25/11 2:47p robertwm
 * SW7125-1166: DirectFB-1.4.5: Warning in p3d_nexus_proxy.c about hP3d
 *  not initialised.
 *
 * DirectFB_1_4_14_Port/1   7/28/11 11:21a christ
 * SW7425-946: DirectFB: Create AppLibs DirectFB 1.4.14 release.
 *
 * DirectFB_1_4_5_Port/1   6/27/11 6:19p robertwm
 * SW7420-1936: DirectFB: Support OpenGL applications with DirectFB
 *  running in multi-application mode.
 *
 * DirectFB_1_4_Port/2   6/27/11 1:17p robertwm
 * SW7405-5390: DirectFB: PX3D proxy code does not relinquish resources
 *  when app is closed.
 *
 * DirectFB_1_4_Port/1   6/13/11 8:07p scchang
 * SW7420-1936: DirectFB: Support OpenGL application with DirectFB running
 *  in multi-application mode
 *
 *
 ***************************************************************************/

#ifdef NEXUS_MODE_proxy

#include "bstd.h"

#include <signal.h>
#include <sys/ioctl.h>
#include <linux/ioctl.h>
#include <fcntl.h>
#include <sys/mman.h>

#if ((BCHP_CHIP==3548) || (BCHP_CHIP==3556)) && (BCHP_VER < BCHP_VER_B0)
#define P3D_IRQ_WORKAROUND 1
#endif

#include "bchp_common.h"
#if (BCHP_CHIP==3548) || (BCHP_CHIP==3556)
#include "bchp_3548.h"
#include "bchp_hif_cpu_intr1.h"
#if P3D_IRQ_WORKAROUND
#include "bchp_int_id_bvnf_intr2_1.h"
#else
#include "bchp_gfx_l2.h"
#include "bchp_px3d.h"
#include "bchp_int_id_gfx_l2.h"
#endif /* P3D_IRQ_WORKAROUND */
#elif BCHP_CHIP==7400
#include "bchp_7400.h"
#include "bchp_px3d_l2.h"
#include "bchp_hif_cpu_intr1.h"
#include "bchp_int_id_px3d_l2.h"
#elif BCHP_CHIP==7405
#if (BCHP_VER < BCHP_VER_C0)
#error "Unsupported chip revision in graphics3d proxy mode"
#endif
#include "bchp_7405.h"
#include "bchp_hif_cpu_intr1.h"
#include "bchp_px3d_intr2.h"
#elif BCHP_CHIP==7420
#include "bchp_hif_cpu_intr1.h"
#include "bchp_7420.h"
#include "bchp_gfx_l2.h"
#include "bchp_px3d.h"
#include "bchp_int_id_gfx_l2.h"
#elif BCHP_CHIP==7125
#include "bchp_7125.h"
#include "bchp_hif_cpu_intr1.h"
#include "bchp_gfx_l2.h"
#include "bchp_int_id_gfx_l2.h"
#elif (BCHP_CHIP==7342 || BCHP_CHIP==7340)
#if (BCHP_CHIP==7342)
#include "bchp_7342.h"
#else
#include "bchp_7340.h"
#endif
#include "bchp_hif_cpu_intr1.h"
#include "bchp_gfx_l2.h"
#include "bchp_int_id_gfx_l2.h"
#elif BCHP_CHIP==7408
#include "bchp_7408.h"
#include "bchp_hif_cpu_intr1.h"
#include "bchp_gfx_l2.h"
#include "bchp_int_id_gfx_l2.h"
#else
#error "Unsupported chip in graphics3d proxy mode"
#endif
#include "bchp_px3d.h"
#include "bchp_priv.h"

#include "bint.h"
#include "bint_plat.h"
#if (BCHP_CHIP==3548) || (BCHP_CHIP==3556)
#include "bint_3548.h"
#elif BCHP_CHIP==7400
#include "bint_7400.h"
#elif BCHP_CHIP==7405
#include "bint_7405.h"
#elif BCHP_CHIP==7125
#include "bint_7125.h"
#elif BCHP_CHIP==7420
#include "bint_7420.h"
#elif BCHP_CHIP==7342
#include "bint_7342.h"
#elif BCHP_CHIP==7340
#include "bint_7340.h"
#elif BCHP_CHIP==7408
#include "bint_7408.h"
#else
#error "Unsupported chip in graphics3d proxy mode"
#endif

#include "errno.h"
#include "pthread.h"

#include "nexus_base.h"
#include "nexus_memory.h"
#include "nexus_platform.h"
#include "nexus_surface.h"
#include "nexus_driver_ioctl.h"

#include "bp3d.h"
#include "bp3d_private.h"
#include "p3d_nexus_proxy.h"

#include "bstd_defs.h"
#include "berr.h"
#include "bkni.h"
#include "bchp.h"

#if ((BCHP_CHIP==7420) && (BCHP_VER <= BCHP_VER_A1))
#include "bchp_memc_0_1.h"
#include "bchp_memc_1_1.h"
#endif

#if (BCHP_CHIP==7400)
#include "bchp_int_id_px3d_l2.h"
#include "bchp_memc_0.h"
#elif (BCHP_CHIP==3548) || (BCHP_CHIP==3556)
#include "bchp_int_id_gfx_l2.h"
#include "bchp_int_id_bvnf_intr2_1.h"
#include "bchp_memc_0_1.h"
#elif (BCHP_CHIP==7405)
#include "bchp_px3d_intr2.h"
#include "bchp_memc_0_1.h"
#elif (BCHP_CHIP==7336)
#include "bchp_px3d_intr2.h"
#elif (BCHP_CHIP!=7038)
#include "bchp_int_id_gfx_l2.h"
#include "bchp_gfx_l2.h"
#endif

BDBG_MODULE(p3d_nexus_proxy);

#if (BCHP_CHIP == 7420)
#define P3D_MEMC1_DISPLAY                     1
#endif
#define REMAP_SURFACES_WITHOUT_IOCTL          1
#define NATIVE_NUM_CACHED_INITWINDOW_SURFACES 10

typedef struct stored_structs {
    void                *memory;
    BSUR_Surface_Handle  hSur;
    int                  width;
    int                  height;
    uint32_t             offset;
    NEXUS_PixelFormat    pixelFormat;
} stored_structs_t;

static BMEM_Handle g_p3d_local_hMem = NULL;
static BMEM_Handle g_p3d_memc0_hMem = NULL;
#if P3D_MEMC1_DISPLAY
static BMEM_Handle g_p3d_memc1_hMem = NULL;
#endif
BMEM_ModuleHandle  g_p3d_hMemModule = NULL;
static BMEM_Handle g_p3d_graphics_hMem = NULL;
static BINT_Handle g_p3d_hInt = NULL;
static BREG_Handle g_p3d_hReg = NULL;
static BP3D_Handle g_p3d_hP3d = NULL;
static void *      g_p3d_MemAddr = NULL;

static const BINT_P_IntMap px3d_bint_FAKE[] =
{
#if P3D_IRQ_WORKAROUND
    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_BVNF_CPU_INTR_1_SHIFT, BCHP_BVNF_INTR2_1_R5F_STATUS, 0, "BVNF1"},
#else
#if (BCHP_CHIP==7400)
    { BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_PX3D_CPU_INTR_SHIFT+32, BCHP_PX3D_L2_CPU_STATUS, 0, "PX3D"},
#elif (BCHP_CHIP==3548) || (BCHP_CHIP==3556)
    { BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_GFX_CPU_INTR_SHIFT+32, BCHP_GFX_L2_CPU_STATUS, 0, "GFX"},
#elif (BCHP_CHIP==7405)
    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_PX3D_CPU_INTR_SHIFT, BCHP_PX3D_INTR2_R5F_STATUS, 0, "PX3D"},
#elif (BCHP_CHIP==7125) || (BCHP_CHIP==7420) || (BCHP_CHIP==7342) || (BCHP_CHIP==7340) || (BCHP_CHIP==7408)
    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_GFX_CPU_INTR_SHIFT, BCHP_GFX_L2_CPU_STATUS, 0, "GFX"},
#else
#error "Unsupported chip in graphics3d proxy mode"
#endif
#endif /* P3D_IRQ_WORKAROUND */
    { -1, 0, 0, NULL}
};
static int g_p3d_int_thread_done = 0;
static int g_p3d_int_thread_started = 0;
static pthread_t int_thread;
static pthread_attr_t int_thread_attr;
static void *b_3d_int_thread(void *data);

static stored_structs_t created_initwindow_surfaces[NATIVE_NUM_CACHED_INITWINDOW_SURFACES+1] = {{NULL,NULL,0,0,0,0}};

/* imports from the Nexus proxy layer */
extern int proxy_ioctl_fd;


/************************************************************************/
/* Miniature fake BCHP layer for P3D only (implemented in graphics3d.c) */
/************************************************************************/
/* Miniature fake BCHP layer for P3D only */
BERR_Code BCHP_OpenFakeChip( BCHP_Handle *phChip,
                             BREG_Handle  hRegister)
{
    BCHP_P_Context fakeChip = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};
    BSTD_UNUSED(hRegister);
    *phChip = &fakeChip;
    return BERR_SUCCESS;
}

/* Note that this shim version of BCHP_GetFeature only returns support for 3D. */
BERR_Code BCHP_GetFeature( const BCHP_Handle   hChip,
                           const BCHP_Feature  eFeature,
                           void               *pFeatureValue )
{
    BSTD_UNUSED(hChip);
    switch (eFeature) {
    case BCHP_Feature_e3DGraphicsCapable:
        *(bool *)pFeatureValue = true;
        break;
    default:
        BDBG_ERR(("%s() Fake BCHP layer has been asked for feature %d", __FUNCTION__, eFeature));
        *(bool *)pFeatureValue = false;
        break;
    }
    return BERR_SUCCESS;
}

static BPXL_Format P3D_P_Nexus2pxl( NEXUS_PixelFormat format )
{
    switch (format)
    {
    case NEXUS_PixelFormat_eR5_G6_B5:
        return BPXL_eR5_G6_B5;
    case NEXUS_PixelFormat_eA8_R8_G8_B8:
        return BPXL_eA8_R8_G8_B8;
    case NEXUS_PixelFormat_eA1_R5_G5_B5:
        return BPXL_eA1_R5_G5_B5;
    case NEXUS_PixelFormat_eA4_R4_G4_B4:
        return BPXL_eA4_R4_G4_B4;
    case NEXUS_PixelFormat_eY08_Cb8_Y18_Cr8:
        return BPXL_eY08_Cb8_Y18_Cr8;
    case NEXUS_PixelFormat_eY18_Cr8_Y08_Cb8:
        return BPXL_eY18_Cr8_Y08_Cb8;
    default:
        BDBG_ERR(("%s() unsupported format %08x!", __FUNCTION__, format));
        return 0;
    }
}

/************************************************************************/
/* Miniature fake BINT layer for P3D only                               */
/************************************************************************/
static void P3D_P_SetInt( BREG_Handle regHandle,
                          uint32_t    baseAddr,
                          int         shift )
{
    BDBG_MSG(("P3D_P_SetInt(%08x,%d)", baseAddr, shift));
    BSTD_UNUSED(regHandle);
    BSTD_UNUSED(baseAddr);
    BSTD_UNUSED(shift);
}

static void P3D_P_ClearInt( BREG_Handle regHandle,
                            uint32_t    baseAddr,
                            int         shift )
{
    BDBG_MSG(("P3D_P_ClearInt(%08x,%d)", baseAddr, shift));
    BSTD_UNUSED(regHandle);
    BSTD_UNUSED(baseAddr);
    BSTD_UNUSED(shift);
}

static void P3D_P_SetMask( BREG_Handle regHandle,
                           uint32_t    baseAddr,
                           int         shift )
{
    BDBG_MSG(("P3D_P_SetMask(%08x,%d)", baseAddr, shift));
    BSTD_UNUSED(regHandle);
    BSTD_UNUSED(baseAddr);
    BSTD_UNUSED(shift);
}

static void P3D_P_ClearMask( BREG_Handle regHandle,
                             uint32_t    baseAddr,
                             int         shift )
{
    BDBG_MSG(("P3D_P_ClearMask(%08x,%d)", baseAddr, shift));
    BSTD_UNUSED(regHandle);
    BSTD_UNUSED(baseAddr);
    BSTD_UNUSED(shift);
}

static uint32_t P3D_P_ReadMask( BREG_Handle regHandle,
                                uint32_t    baseAddr )
{
    uint32_t rc;

    BDBG_MSG(("P3D_P_ReadMask(%08x)", baseAddr));
    BSTD_UNUSED(regHandle);
    switch (baseAddr) {
    case 0x01200000:        /* 7400 */
        rc = 0x00000008;    /* PX3D_2_INTR */
        break;
    case 0x00c50000:
        rc = 0x00000100;
        break;
    case 0x00303100:
        rc = 0x00a00500;
        break;
    case 0x00129200:        /* 7405 (7413) */
        rc = 0x00000004;
        break;
    case 0x00483000:        /* 7125 && 7420 */
        rc = 0x00000008;
        break;
    default:
        BDBG_ERR(("%s() unrecognized base address 0x%08x", __FUNCTION__, baseAddr));
        rc = 0;
    }
    return rc;
}

static uint32_t P3D_P_ReadStatus( BREG_Handle regHandle,
                                  uint32_t    baseAddr )
{
    uint32_t rc;

    BDBG_MSG(("P3D_P_ReadStatus(%08x)", baseAddr));
    BSTD_UNUSED(regHandle);
    switch (baseAddr) {
    case 0x01200000:
        rc = 0x00000008;
        break;
    case 0x00c50000:
        rc = 0x00000100;
        break;
    case 0x00303100:
        rc = 0x00a00500;
        break;
    case 0x00129200:
        rc = 0x00000004;
        break;
    case 0x00483000:        /* 7125 && 7420 */
        rc = 0x00000008;
        break;
    default:
        BDBG_ERR(("%s() unrecognized base address 0x%08x", __FUNCTION__, baseAddr));
        rc = 0;
    }
    return rc;
}

/* interrupt monitoring thread */
static void *b_3d_int_thread(void *data)
{
    int rc;

    BSTD_UNUSED(data);

    g_p3d_int_thread_started = true;
    if (proxy_ioctl_fd != -1) {
        PROXY_NEXUS_Hook3dInterrupt data;

#if P3D_IRQ_WORKAROUND
        data.which_interrupt = BCHP_INT_ID_BVNF_INTR2_1_RDC_TRIG_10_INTR;
#else
#if (BCHP_CHIP==7400)
        data.which_interrupt = BCHP_INT_ID_PX3D_2_INTR;
#elif (BCHP_CHIP==3548) || (BCHP_CHIP==3556)
        data.which_interrupt = BCHP_INT_ID_PX3D_2_INTR;
#elif (BCHP_CHIP==7405)
        data.which_interrupt = BCHP_INT_ID_CREATE(BCHP_PX3D_INTR2_R5F_STATUS, BCHP_PX3D_INTR2_R5F_STATUS_PX3D_2_INTR_SHIFT);
#elif (BCHP_CHIP==7420) || (BCHP_CHIP==7125) || (BCHP_CHIP==7340) || (BCHP_CHIP==7342) || (BCHP_CHIP==7408)
        data.which_interrupt = BCHP_INT_ID_CREATE(BCHP_GFX_L2_CPU_STATUS, BCHP_GFX_L2_CPU_STATUS_PX3D_2_INTR_SHIFT);
#else
#error "Unsupported chip in graphics3d proxy mode"
#endif
#endif /* P3D_IRQ_WORKAROUND */
        rc = ioctl(proxy_ioctl_fd, IOCTL_PROXY_NEXUS_Hook3dInterrupt, &data);
        if (rc!=0) {BERR_TRACE(NEXUS_OS_ERROR);}
    } else {
        BDBG_ERR(("%s() proxy_ioctl_fd seems to be uninitialized!!!", __FUNCTION__));
    }

    while (!g_p3d_int_thread_done) {
        pthread_testcancel();
        if (proxy_ioctl_fd != -1) {
            PROXY_NEXUS_WaitFor3dInterrupt data;

            data.was_fired = 0;
            if (ioctl(proxy_ioctl_fd, IOCTL_PROXY_NEXUS_WaitFor3dInterrupt, &data)) {
                /* this is normal on shutdown */
                BDBG_WRN(("b_3d_int_thread exiting"));
                break;
            }

            if (data.was_fired) {

                BKNI_EnterCriticalSection();
#if P3D_IRQ_WORKAROUND
                BINT_Isr(g_p3d_hInt, BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_BVNF_CPU_INTR_1_SHIFT);
#else
#if (BCHP_CHIP==7400)
                BINT_Isr(g_p3d_hInt, BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_PX3D_CPU_INTR_SHIFT + 32);
#elif (BCHP_CHIP==3548) || (BCHP_CHIP==3556)
                BINT_Isr(g_p3d_hInt, BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_GFX_CPU_INTR_SHIFT + 32);
#elif (BCHP_CHIP==7405)
                BINT_Isr(g_p3d_hInt, BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_PX3D_CPU_INTR_SHIFT);
#elif (BCHP_CHIP==7125) || (BCHP_CHIP==7420) || (BCHP_CHIP==7342) || (BCHP_CHIP==7340) || (BCHP_CHIP==7408)
                BINT_Isr(g_p3d_hInt, BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_GFX_CPU_INTR_SHIFT);
#else
#error "Unsupported chip in graphics3d proxy mode"
#endif
#endif /* P3D_IRQ_WORKAROUND */
                BKNI_LeaveCriticalSection();
            }

        }
    }
    if (proxy_ioctl_fd != -1) {
        PROXY_NEXUS_Unhook3dInterrupt data;

#if (BCHP_CHIP==7408)
        data.which_interrupt = BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_BVNF_1_CPU_INTR_SHIFT;
#else
        data.which_interrupt = BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_BVNF_CPU_INTR_1_SHIFT;
#endif
        rc = ioctl(proxy_ioctl_fd, IOCTL_PROXY_NEXUS_Unhook3dInterrupt, &data);
        if (rc!=0) {BERR_TRACE(NEXUS_OS_ERROR);}
    }
    return NULL;
}

static BERR_Code P3D_Open(
        BP3D_Handle *phP3d,
        BCHP_Handle hChip,
        BREG_Handle hRegister,
        BMEM_Handle hMemory,
        BINT_Handle hInterrupt )
{
        BP3D_Handle hP3d = 0;
        BERR_Code err = BERR_SUCCESS;

        BDBG_ENTER(P3D_Open);
        BDBG_ASSERT( phP3d );
        BDBG_ASSERT( hChip );
        BDBG_ASSERT( hRegister );
        BDBG_ASSERT( hMemory );
        BDBG_ASSERT( hInterrupt );

#ifdef BP3D_P_ISR_TIMER
        BTMR_GetDefaultSettings( &dsettings );
        BTMR_Open( &hTmr, hChip, hRegister, hInterrupt, &dsettings );

        BTMR_GetDefaultTimerSettings( &tsettings );
        BTMR_CreateTimer( hTmr, &hTimer, &tsettings );
#endif

#if ((BCHP_CHIP==3548) && (BCHP_VER == BCHP_VER_A0)) || \
        ((BCHP_CHIP==3556) && (BCHP_VER == BCHP_VER_A0))
        BREG_Write32( hRegister, BCHP_MEMC_0_1_CLIENT_INFO_25, 0x4000 /*BCHP_MEMC_0_1_CLIENT_INFO_25_RR_EN_ENABLED*/ );
#endif

#if ((BCHP_CHIP==7420) && (BCHP_VER <= BCHP_VER_A1))
{
        /* Switch GFD from MEMC1 to MEMC0 */
        uint32_t ulMemClient0 = BREG_Read32( hRegister, BCHP_MEMC_0_1_CLIENT_INFO_27 );
        uint32_t ulMemClient1 = BREG_Read32( hRegister, BCHP_MEMC_1_1_CLIENT_INFO_27 );

        if( ulMemClient0 < ulMemClient1 )
        {
                BREG_Write32( hRegister, BCHP_MEMC_0_1_CLIENT_INFO_27, ulMemClient1 );
                BREG_Write32( hRegister, BCHP_MEMC_1_1_CLIENT_INFO_27, ulMemClient0 );

                ulMemClient0 = BREG_Read32( hRegister, BCHP_MEMC_0_1_CLIENT_INFO_28 );
                ulMemClient1 = BREG_Read32( hRegister, BCHP_MEMC_1_1_CLIENT_INFO_28 );
                BREG_Write32( hRegister, BCHP_MEMC_0_1_CLIENT_INFO_28, ulMemClient1 );
                BREG_Write32( hRegister, BCHP_MEMC_1_1_CLIENT_INFO_28, ulMemClient0 );

                ulMemClient0 = BREG_Read32( hRegister, BCHP_MEMC_0_1_CLIENT_INFO_29 );
                ulMemClient1 = BREG_Read32( hRegister, BCHP_MEMC_1_1_CLIENT_INFO_29 );
                BREG_Write32( hRegister, BCHP_MEMC_0_1_CLIENT_INFO_29, ulMemClient1 );
                BREG_Write32( hRegister, BCHP_MEMC_1_1_CLIENT_INFO_29, ulMemClient0 );
        }
}
#endif

        /* allocate memory for private data */
        hP3d = (BP3D_Handle) BKNI_Malloc( sizeof (BP3D_P_Handle) );
        if( hP3d == 0 )
        {
                err = BERR_OUT_OF_SYSTEM_MEMORY;
                goto fail;
        }

        /* clear main data structure */
        BKNI_Memset( (void *) hP3d, 0, sizeof (BP3D_P_Handle) );

        /* create interrupt callback */
#if ((BCHP_CHIP==3548) && (BCHP_VER == BCHP_VER_A0)) || \
        ((BCHP_CHIP==3556) && (BCHP_VER == BCHP_VER_A0))
        err = BINT_CreateCallback( &hP3d->hInterruptCallback, hInterrupt,
                BCHP_INT_ID_BVNF_INTR2_1_RDC_TRIG_10_INTR, &BP3D_P_Render_Isr, hP3d, 0 );
#elif (BCHP_CHIP==7405) || (BCHP_CHIP==7336)
        err = BINT_CreateCallback( &hP3d->hInterruptCallback, hInterrupt, BCHP_INT_ID_CREATE(BCHP_PX3D_INTR2_R5F_STATUS, BCHP_PX3D_INTR2_R5F_STATUS_PX3D_2_INTR_SHIFT), &BP3D_P_Render_Isr, hP3d, 0 );
#else
        err = BINT_CreateCallback( &hP3d->hInterruptCallback, hInterrupt,
                BCHP_INT_ID_PX3D_2_INTR, &BP3D_P_Render_Isr, hP3d, 0 );
#endif
        if( err != BERR_SUCCESS )
                goto fail;

        /* clear interrupt callback */
        err = BINT_ClearCallback( hP3d->hInterruptCallback );
        if( err != BERR_SUCCESS )
                goto fail;

        /* enable interrupt callback */
        err = BINT_EnableCallback( hP3d->hInterruptCallback );
        if( err != BERR_SUCCESS )
                goto fail;

#if (BCHP_CHIP==7405) || (BCHP_CHIP==3548) || (BCHP_CHIP==3556)
        /* PX3D Fix: Store memory client */
        hP3d->ulMemClientInfoReg = BREG_Read32( hRegister, BP3D_P_MEMC_0_1_CLIENT_INFO() ) |
                BP3D_P_MEMC_0_1_CLIENT_INFO(_BO_VAL_MASK) | BP3D_P_MEMC_0_1_CLIENT_INFO(_RR_EN_MASK);
        BREG_Write32( hRegister, BP3D_P_MEMC_0_1_CLIENT_INFO(), hP3d->ulMemClientInfoReg );
#endif

        /* allocate device memory for intermediate multi-render buffer */
        hP3d->pvTmpBuffer = BMEM_AllocAligned( hMemory, BP3D_P_TILE_WIDTH * BP3D_P_TILE_HEIGHT *
                (BPXL_BITS_PER_PIXEL(BPXL_eA8_R8_G8_B8) / 8 + BPXL_BITS_PER_PIXEL(BPXL_eZ16) / 8), 2, 0 );
        if( hP3d->pvTmpBuffer == NULL )
        {
                err = BERR_OUT_OF_DEVICE_MEMORY;
                goto fail;
        }

        /* convert intermediate buffer address to offset */
        err = BMEM_ConvertAddressToOffset( hMemory, hP3d->pvTmpBuffer, &hP3d->TmpColorBuffer.ulOffset );
        if( err != BERR_SUCCESS )
                goto fail;

        /* set intermediate buffer data */
        hP3d->TmpColorBuffer.eFormat = BPXL_eA8_R8_G8_B8;
        hP3d->TmpColorBuffer.ulBpp = BPXL_BITS_PER_PIXEL(hP3d->TmpColorBuffer.eFormat) / 8;
        hP3d->TmpColorBuffer.ulPitch = BP3D_P_TILE_WIDTH * hP3d->TmpColorBuffer.ulBpp;

        hP3d->TmpDepthBuffer.ulOffset = hP3d->TmpColorBuffer.ulOffset + hP3d->TmpColorBuffer.ulPitch * BP3D_P_TILE_HEIGHT;
        hP3d->TmpDepthBuffer.eFormat = BPXL_eZ16;
        hP3d->TmpDepthBuffer.ulBpp = BPXL_BITS_PER_PIXEL(hP3d->TmpDepthBuffer.eFormat) / 8;
        hP3d->TmpDepthBuffer.ulPitch = BP3D_P_TILE_WIDTH * hP3d->TmpDepthBuffer.ulBpp;

        /* set private data */
        hP3d->hChip = hChip;
        hP3d->hRegister = hRegister;
        hP3d->hMemory = hMemory;
        hP3d->hInterrupt = hInterrupt;

        /* set return handle */
        *phP3d = hP3d;

        BDBG_LEAVE(P3D_Open);
        return BERR_SUCCESS;

fail:
        if( hP3d )
        {
                if( hP3d->pvTmpBuffer )
                        BMEM_Free( hMemory, hP3d->pvTmpBuffer );

                if( hP3d->hInterruptCallback )
                        BINT_DestroyCallback( hP3d->hInterruptCallback );

                BKNI_Free( (void *) hP3d );
        }

        BDBG_LEAVE(P3D_Open);
        return BERR_TRACE(err);
}

/***************************************************************************/
/* PUBLIC */
/***************************************************************************/
#define MEM_HEAP0_SIZE 192*1024*1024
#define MEM_HEAP1_SIZE  64*1024*1024
#define MEM_HEAP2_SIZE 192*1024*1024
#define MEM_HEAP3_SIZE 256*1024*1024
#define MEM_HEAP0_OFFSET  0x04000000
#define MEM_HEAP1_OFFSET  0x60000000
#define MEM_HEAP2_OFFSET  0x64000000
#define MEM_HEAP3_OFFSET  0x20000000

BP3D_Handle Nexus_Proxy_P3D_GetP3d(void)
{
    return g_p3d_hP3d;
}

BMEM_Handle Nexus_Proxy_P3D_GetMEM(void)
{
    return g_p3d_local_hMem;
}

void Nexus_Proxy_P3D_Close( BP3D_Handle hP3d )
{
    int rc;

    /* Close the thread if started */
    if (g_p3d_int_thread_started)
    {
        BDBG_MSG(("%s: About to cancel thread...", __FUNCTION__));
        g_p3d_int_thread_done = 1;
        pthread_cancel(int_thread);
        pthread_kill(int_thread, SIGHUP);
        pthread_join(int_thread, NULL);
        BDBG_MSG(("%s: Thread cancelled", __FUNCTION__));
    }

    BP3D_Close(hP3d);

    if (proxy_ioctl_fd != -1) {
        PROXY_NEXUS_3dPowerManagement data;

        data.pm_state = false;
        rc = ioctl(proxy_ioctl_fd, IOCTL_PROXY_NEXUS_3dPowerManagement, &data);
        if(rc!=0) {BERR_TRACE(NEXUS_OS_ERROR);}
    }

    BINT_Close(g_p3d_hInt);
    BREG_Close(g_p3d_hReg);
    NEXUS_Memory_Free(g_p3d_MemAddr);
}

void Nexus_Proxy_P3D_Open( BMEM_Handle *phMem,
                           BP3D_Handle *phP3d )
{
    BMEM_Handle hMem;
    BCHP_Handle hChip;
    BINT_Handle hInt;
    BP3D_Handle hP3d = NULL;

    BERR_Code brc;
    NEXUS_Error rc;

    /* support to declare a BMEM handle */
    BMEM_Settings memSettings;
    BMEM_Heap_Settings memHeapSettings;

    /* parameters for the P3D proxy local heap */
    unsigned long localMemSize = 0;
    void *localMemAddr = NULL;
    unsigned long localMemOffset = 0;

    /* parameters for the uncached mmap heap */
    int fd;
    void *mapAddress = NULL;

    /* support to declare BINT */
    const BINT_Settings *intSettings;
    BINT_Settings p3dIntSettings;


    /* Set up a user-mode P3D.  This requires the following handles: BCHP, BREG, BMEM, BINT */

    /* Configure a BMEM handle */
    /* 32 MB is the default size for the local p3d-specific heap.
     * *ALL* OpenGL/PX3D allocations should occur on here, so that
     * they can be de-referenced in both user and kernel space.
     *
     * This value may need to be adjusted based on the application's specific use. */
#if (BCHP_CHIP==3548) || (BCHP_CHIP==3556) || (BCHP_CHIP==7405) || (BCHP_CHIP==7420) || (BCHP_CHIP==7125) || (BCHP_CHIP==7342) || (BCHP_CHIP==7340) || (BCHP_CHIP==7408)
    localMemSize = 32*1024*1024;
#elif BCHP_CHIP==7400
    localMemSize = 16*1024*1024;
#else
#error "Unsupported chip in graphics3d proxy mode"
#endif

    rc = NEXUS_Memory_Allocate(localMemSize, NULL, &g_p3d_MemAddr);
    if (rc) {
        BDBG_ERR(("%s() NEXUS_Memory_Allocate failed: %d", __FUNCTION__, rc));
    }
    if (!g_p3d_MemAddr) {
        BDBG_ERR(("%s() Failed to allocate memory", __FUNCTION__));
    }
    /* Set up the data for the local heap, based on the allocation */
    localMemOffset = NEXUS_AddrToOffset(g_p3d_MemAddr);
    BDBG_MSG(("%s() allocat memory from nexus address(%p), offset(%p), size(%lu)", __FUNCTION__,
              g_p3d_MemAddr, localMemOffset, localMemSize));

    /* Heap creation settings */
    BMEM_GetDefaultSettings(&memSettings);
    brc = BMEM_Open(&g_p3d_hMemModule, &memSettings);
    if (brc) {
        BDBG_ERR(("%s() BMEM_Open failed: %d", __FUNCTION__, brc));
    }

#if P3D_MEMC1_DISPLAY
    memHeapSettings.uiAlignment    = 7;
#else
    memHeapSettings.uiAlignment    = 6;
#endif
    memHeapSettings.eSafetyConfig  = BMEM_SafetyConfig_eTrack;/*BMEM_SafetyConfig_eFastest*/
    memHeapSettings.eBookKeeping   = BMEM_BOOKKEEPING_SYSTEM;
    memHeapSettings.pCachedAddress = NULL;
    memHeapSettings.flush          = NULL;
    memHeapSettings.flush_isr      = NULL;

    /* Create an uncached mmap */
    fd = open("/dev/mem", O_RDWR|O_SYNC);
    mapAddress = mmap(0, MEM_HEAP0_SIZE, PROT_READ|PROT_WRITE,MAP_SHARED, fd, MEM_HEAP0_OFFSET);
    brc = BMEM_Heap_Create(g_p3d_hMemModule, mapAddress, MEM_HEAP0_OFFSET, MEM_HEAP0_SIZE, &memHeapSettings, &g_p3d_memc0_hMem);
    if (brc) {
        BDBG_ERR(("%s() BMEM_Heap_Create on MEMC0 failed: %d", __FUNCTION__, brc));
    }
    BDBG_MSG(("%s() address(%p), handle(%p)", __FUNCTION__, mapAddress, g_p3d_memc0_hMem));

#if P3D_MEMC1_DISPLAY
    mapAddress = mmap(0, MEM_HEAP1_SIZE, PROT_READ|PROT_WRITE,MAP_SHARED, fd, MEM_HEAP1_OFFSET);
    brc = BMEM_Heap_Create(g_p3d_hMemModule, mapAddress, MEM_HEAP1_OFFSET, MEM_HEAP1_SIZE, &memHeapSettings, &g_p3d_memc1_hMem);
    if (brc) {
        BDBG_ERR(("%s() BMEM_Heap_Create on MEMC1 failed: %d", __FUNCTION__, brc));
    }
    BDBG_MSG(("%s() address(%p), handle(%p)", __FUNCTION__, mapAddress, g_p3d_memc1_hMem));

    mapAddress = mmap(0, MEM_HEAP2_SIZE, PROT_READ|PROT_WRITE,MAP_SHARED, fd, MEM_HEAP2_OFFSET);
    brc = BMEM_Heap_Create(g_p3d_hMemModule, mapAddress, MEM_HEAP2_OFFSET, MEM_HEAP2_SIZE, &memHeapSettings, &g_p3d_graphics_hMem);
    if (brc) {
        BDBG_ERR(("%s() BMEM_Heap_Create on MEMC1 failed: %d", __FUNCTION__, brc));
    }
    BDBG_MSG(("%s() address(%p), handle(%p)", __FUNCTION__, mapAddress, g_p3d_graphics_hMem));
#endif

    if (g_p3d_graphics_hMem == NULL)
        g_p3d_graphics_hMem = g_p3d_memc0_hMem;

    /* get a pointer in the uncached address space to the memory we allocated earlier,
     * then create a heap off of it */
    localMemAddr = g_p3d_MemAddr;
    BMEM_ConvertOffsetToAddress(g_p3d_memc0_hMem, localMemOffset, &localMemAddr);
    brc = BMEM_Heap_Create(g_p3d_hMemModule, localMemAddr, localMemOffset, localMemSize, &memHeapSettings, &hMem);
    if (brc) {
        BDBG_ERR(("%s() BMEM_Heap_Create on MEMC0 failed: %d", __FUNCTION__, brc));
    }
    BDBG_MSG(("%s() address(%p), offset(%p), size(%lu), handle(%p)", __FUNCTION__, localMemAddr, localMemOffset, localMemSize, hMem));
    g_p3d_local_hMem = hMem;

    *phMem = hMem;

    /* Configure a BREG handle */
    fd = open("/dev/mem", O_RDWR|O_SYNC);
#if (BCHP_CHIP==3548) || (BCHP_CHIP==3556)
    mapAddress = mmap(0, BCHP_PX3D_TIME_STATUS+4, PROT_READ|PROT_WRITE, MAP_SHARED, fd, BCHP_PHYSICAL_OFFSET);
#elif BCHP_CHIP==7400 || (BCHP_CHIP==7125) || (BCHP_CHIP==7340) || (BCHP_CHIP==7342) || (BCHP_CHIP==7408)
    mapAddress = mmap(0, BCHP_PX3D_REG_END+4, PROT_READ|PROT_WRITE, MAP_SHARED, fd, BCHP_PHYSICAL_OFFSET);
#elif BCHP_CHIP==7405
    mapAddress = mmap(0, BCHP_MEM_DMA_SECURE_REG_END+4, PROT_READ|PROT_WRITE, MAP_SHARED, fd, BCHP_PHYSICAL_OFFSET);
#elif BCHP_CHIP==7420
    mapAddress = mmap(0, BCHP_REGISTER_END, PROT_READ|PROT_WRITE, MAP_SHARED, fd, BCHP_PHYSICAL_OFFSET);
#else
#error "Unsupported chip in graphics3d proxy mode"
#endif
    if (mapAddress == MAP_FAILED) {
        BDBG_ERR(("Failed to map registers"));
    }
    BREG_Open(&g_p3d_hReg, mapAddress, BCHP_PHYSICAL_OFFSET);

    /* Configure a (faked) BCHP handle.
     * This is to allow p3d to do a check to see if the chip supports 3D.
     * The shim functions are in graphics3d.c
     *
     * This is faked, because the full BCHP_OpenXXXX() functions reset many of the cores. */
    BCHP_OpenFakeChip(&hChip, g_p3d_hReg);

    /* set up our own BINT layer */
#if (BCHP_CHIP==3548) || (BCHP_CHIP==3556)
    intSettings = BINT_3548_GetSettings();
#elif BCHP_CHIP==7400
    intSettings = BINT_7400_GetSettings();
#elif BCHP_CHIP==7405
    intSettings = BINT_7405_GetSettings();
#elif BCHP_CHIP==7125
    intSettings = BINT_7125_GetSettings();
#elif BCHP_CHIP==7340
    intSettings = BINT_7340_GetSettings();
#elif BCHP_CHIP==7342
    intSettings = BINT_7342_GetSettings();
#elif BCHP_CHIP==7420
    intSettings = BINT_7420_GetSettings();
#elif BCHP_CHIP==7408
    intSettings = BINT_7408_GetSettings();
#else
#error "Unsupported chip in graphics3d proxy mode"
#endif
    p3dIntSettings.name        = intSettings->name;
    /* Override the writes.  These shim functions are earlier in this file. */
    p3dIntSettings.pReadMask   = P3D_P_ReadMask;
    p3dIntSettings.pReadStatus = P3D_P_ReadStatus;
    p3dIntSettings.pClearInt   = P3D_P_ClearInt;
    p3dIntSettings.pClearMask  = P3D_P_ClearMask;
    p3dIntSettings.pSetInt     = P3D_P_SetInt;
    p3dIntSettings.pSetMask    = P3D_P_SetMask;
    p3dIntSettings.pIntMap     = px3d_bint_FAKE;

    brc = BINT_Open(&hInt, g_p3d_hReg, &p3dIntSettings);
    if (brc) {
        BDBG_ERR(("%s() BINT_Open failed: %d", __FUNCTION__, brc));
    }
    g_p3d_hInt = hInt;

    if (proxy_ioctl_fd != -1) {
        int rc;
        PROXY_NEXUS_3dPowerManagement data;

        data.pm_state = true;
        rc = ioctl(proxy_ioctl_fd, IOCTL_PROXY_NEXUS_3dPowerManagement, &data);
        if(rc!=0) {BERR_TRACE(NEXUS_OS_ERROR);}
    }
    else
    {
        BDBG_ERR(("%s: Proxy not opened!!!\n", __FUNCTION__));
    }

    /* and finally, use these to create a p3d handle */
    brc = P3D_Open(&hP3d, hChip, g_p3d_hReg, g_p3d_local_hMem, hInt);
    if (brc) {
        BDBG_ERR(("%s() P3D_Open failed: %d", __FUNCTION__, brc));
    }
    g_p3d_hP3d = hP3d;

    *phP3d = hP3d;
}

/* this triggers whether IOCTL_PROXY_NEXUS_GetSurfaceOffset is used to
 * gain information about the surface before remapping it for use, or
 * whether Nexus calls are used.  Some use cases are marginally faster one way,
 * some are the other.  This define allows both cases to persist for
 * benchmarking.
 */
/***************************************************************************/
void Nexus_Proxy_P3D_GetSurface( NEXUS_SurfaceHandle  hSurface,
                                 BSUR_Surface_Handle *phSur,
                                 int                 *piWidth,
                                 int                 *piHeight )
{
    BERR_Code brc;
    BMEM_Heap_Handle hCurMem;
    BSUR_Surface_Settings bsurface_settings;
#if REMAP_SURFACES_WITHOUT_IOCTL
    NEXUS_SurfaceMemory sur_memory;
    NEXUS_SurfaceCreateSettings sur_settings;
    int ix;
    void *sur_addr = NULL;

    if (!g_p3d_int_thread_started) {
        /* don't bother starting this thread until we have a display */
        g_p3d_int_thread_done = 0;

        pthread_attr_init(&int_thread_attr);
        pthread_attr_setstacksize(&int_thread_attr, 16*1024);

        if (pthread_create(&int_thread, &int_thread_attr, b_3d_int_thread, NULL)) {
            BDBG_MSG(("Unable to create interrupt monitoring thread!"));
        }
    }

    NEXUS_Surface_GetMemory(hSurface, &sur_memory);
    NEXUS_Surface_GetCreateSettings(hSurface, &sur_settings);

    for (ix=0; ix < NATIVE_NUM_CACHED_INITWINDOW_SURFACES; ix++) {
        if (!created_initwindow_surfaces[ix].memory) {
            /*printf("%s(): surfaces[%d].memory arrary is empty\n", __FUNCTION__, ix);*/
            break;
        } else if ((created_initwindow_surfaces[ix].memory == sur_memory.buffer) &&
                   (created_initwindow_surfaces[ix].pixelFormat == sur_settings.pixelFormat)) {
            /*printf("%s(): find the associated surfaces[%d].memory\n", __FUNCTION__, ix);*/
            *phSur    = created_initwindow_surfaces[ix].hSur;
            *piWidth  = created_initwindow_surfaces[ix].width;
            *piHeight = created_initwindow_surfaces[ix].height;
            return;
        }
    }

    *piWidth  = sur_settings.width;
    *piHeight = sur_settings.height;
#if P3D_MEMC1_DISPLAY
    if (NEXUS_AddrToOffset(sur_memory.buffer) >= MEM_HEAP2_OFFSET)
        hCurMem = g_p3d_graphics_hMem;
    else if (NEXUS_AddrToOffset(sur_memory.buffer) >= MEM_HEAP1_OFFSET)
        hCurMem = g_p3d_memc1_hMem;
    else
        hCurMem = g_p3d_local_hMem;
#else
    hCurMem = g_p3d_memc0_hMem;
#endif
    BMEM_Heap_ConvertOffsetToAddress(hCurMem, NEXUS_AddrToOffset(sur_memory.buffer), &sur_addr);
    BSUR_Surface_GetDefaultSettings(&bsurface_settings);
    brc = BSUR_Surface_Create(hCurMem, sur_settings.width, sur_settings.height, sur_memory.pitch, sur_addr, P3D_P_Nexus2pxl(sur_settings.pixelFormat), NULL, BSUR_CONSTRAINT_ADDRESS_PIXEL_ALIGNED, &bsurface_settings, phSur);
    if (brc)
        BDBG_ERR(("%s() create surface error=%d", __FUNCTION__, brc));


    if (ix < NATIVE_NUM_CACHED_INITWINDOW_SURFACES) {
        /*printf("%s(): save to surfaces[%d].memory\n", __FUNCTION__, ix);*/
        created_initwindow_surfaces[ix].memory = sur_memory.buffer;
        created_initwindow_surfaces[ix].hSur   = *phSur;
        created_initwindow_surfaces[ix].width  = *piWidth;
        created_initwindow_surfaces[ix].height = *piHeight;
        created_initwindow_surfaces[ix].pixelFormat = sur_settings.pixelFormat;
    } else {
        BDBG_ERR(("Out of cached pixmaps!  Please increase the value of NATIVE_NUM_CACHED_INITWINDOW_SURFACES at %s:%d",__FILE__,__LINE__));
    }
#else

    if (!g_p3d_int_thread_started) {
        /* don't bother starting this thread until we have a display */
        g_p3d_int_thread_done = 0;

        pthread_attr_init(&int_thread_attr);
        pthread_attr_setstacksize(&int_thread_attr, 16*1024);

        if (pthread_create(&int_thread, &int_thread_attr, b_3d_int_thread, NULL)) {
            BDBG_MSG(("Unable to create interrupt monitoring thread!"));
        }
    }

    if (proxy_ioctl_fd != -1) {
        int rc;
        BERR_Code brc;
        PROXY_NEXUS_GetSurfaceOffset data;
        int ix;
        void *sur_addr = NULL;

        data.surface = sur;
        rc = ioctl(proxy_ioctl_fd, IOCTL_PROXY_NEXUS_GetSurfaceOffset, &data);
        if(rc!=0) {BERR_TRACE(NEXUS_OS_ERROR);}
        brc = BMEM_Heap_ConvertOffsetToAddress(g_p3d_local_hMem, data.offset, &sur_addr);

        for (ix=0; ix < NATIVE_NUM_CACHED_INITWINDOW_SURFACES; ix++) {
            if (!created_initwindow_surfaces[ix].offset) {
                break;
            } else if (created_initwindow_surfaces[ix].offset == data.offset) {
                *phSur    = created_initwindow_surfaces[ix].hSur;
                *piWidth  = created_initwindow_surfaces[ix].width;
                *piHeight = created_initwindow_surfaces[ix].height;
                return;
            }
        }

        *piWidth  = data.width;
        *piHeight = data.height;
        BSUR_Surface_GetDefaultSettings(&bsurface_settings);
        BSUR_Surface_Create(g_p3d_local_hMem, data.width, data.height, data.pitch, sur_addr, P3D_P_Nexus2pxl(data.pixel_format), NULL, BSUR_CONSTRAINT_ADDRESS_PIXEL_ALIGNED, &bsurface_settings, phSur);

        if (ix < NATIVE_NUM_CACHED_INITWINDOW_SURFACES) {
            created_initwindow_surfaces[ix].offset = data.offset;
            created_initwindow_surfaces[ix].hSur   = *phSur;
            created_initwindow_surfaces[ix].width  = *piWidth;
            created_initwindow_surfaces[ix].height = *piHeight;
        } else {
            BDBG_ERR(("Out of cached pixmaps!  Please increase the value of NATIVE_NUM_CACHED_INITWINDOW_SURFACES at %s:%d",__FILE__,__LINE__));
        }
    }
#endif
}
#else
void dummy(void)
{
}
#endif
