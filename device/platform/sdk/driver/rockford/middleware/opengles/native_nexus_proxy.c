/***************************************************************************
 *     Copyright (c) 2008-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: native_nexus_proxy.c $
 * $brcm_Revision: Hydra_Software_Devel/26 $
 * $brcm_Date: 5/15/12 12:52p $
 *
 * Module Description: 
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/middleware/opengles/native_nexus_proxy.c $
 * 
 * Hydra_Software_Devel/26   5/15/12 12:52p nissen
 * SW7125-1283: Fixed build error.
 * 
 * Hydra_Software_Devel/25   5/11/12 2:12p nissen
 * SW7125-1283: Fixed P3D memory leak when closing.
 * 
 * Hydra_Software_Devel/24   2/15/12 4:54p mward
 * SW7425-2218: removed AvsLock/Unlock and added get handle.
 * 
 * Hydra_Software_Devel/23   2/2/12 12:24p mward
 * SW7425-2216: Adding initializers for for AVS Lock/Unlock members of
 * fakeChip.
 * 
 * Hydra_Software_Devel/22   1/17/12 8:16p nissen
 * SW7435-30: Added support for using planes instead of BSUR surfaces.
 * 
 * Hydra_Software_Devel/21   8/22/11 11:15a mward
 * SW7346-117: Added support for a chip specific stand-by mode function.
 * 
 * Hydra_Software_Devel/20   3/11/11 1:26p gmohile
 * SW7408-217 : Add 7408 support
 * 
 * Hydra_Software_Devel/19   3/1/11 8:04p mphillip
 * SW7420-1442: Update to handle boards with memc1 for the main
 * framebuffer
 * 
 * Hydra_Software_Devel/18   2/11/11 2:35p mward
 * SW7408-212 : Add support for avs settings
 * 
 * Hydra_Software_Devel/17   10/21/10 4:48p mphillip
 * SW7125-670: Move bchp shim out of graphics3d for proxy mode
 * 
 * Hydra_Software_Devel/16   8/6/10 12:31p nissen
 * SW7125-545: Fixed warnings for Nexus function calls.
 * 
 * Hydra_Software_Devel/15   7/2/10 7:19p jrubio
 * SW7342-224: fix typo with 7340
 * 
 * Hydra_Software_Devel/14   7/2/10 7:18p jrubio
 * SW7342-224: fix typo with 7340
 * 
 * Hydra_Software_Devel/13   7/2/10 5:33p jrubio
 * SW7342-224: Add 7340/7342 Support
 * 
 * Hydra_Software_Devel/12   10/27/09 6:55p mphillip
 * SW7420-413: Add support for 7420
 * 
 * Hydra_Software_Devel/11   10/27/09 11:01a mward
 * SW7125-41: 7125 interrupt support for 3D proxy
 * 
 * Hydra_Software_Devel/10   8/26/09 3:23p nissen
 * SW7405-1990: Added pixel format check when comparing surfaces.
 * 
 * Hydra_Software_Devel/9   7/8/09 10:39a jtna
 * PR55968: 3d graphics power management
 * 
 * Hydra_Software_Devel/8   1/13/09 1:43p mphillip
 * PR50961: 7405 interrupt support for 3D proxy
 * 
 * Hydra_Software_Devel/7   1/9/09 2:37p mphillip
 * PR49825: Use correct interrupt for 35xx B0
 * 
 * Hydra_Software_Devel/6   11/20/08 3:31p mphillip
 * PR48697: Fix typo
 * 
 * Hydra_Software_Devel/5   11/11/08 11:49a mphillip
 * PR48697: 7400-specific changes to nexus 3d kernel layer
 * 
 * Hydra_Software_Devel/4   11/4/08 3:58p mphillip
 * PR47441: Adjust to new BSUR_Surface_Create parameters
 * 
 * Hydra_Software_Devel/3   11/3/08 3:18p mphillip
 * PR47441: Follow the native_P_BlitSurface API change
 * 
 * Hydra_Software_Devel/2   10/31/08 6:19p mphillip
 * PR47441: 3d kernel proxy implementation
 * 
 * Hydra_Software_Devel/1   10/30/08 2:47p nissen
 * PR 47441: Initial version.
 * 
 ***************************************************************************/

#include "bstd.h"
#include "native_private.h"
#include "native_nexus.h"

#include <sys/ioctl.h>
#include <linux/ioctl.h>
#include <fcntl.h>
#include <sys/mman.h>

#include "nexus_base.h"
#include "nexus_memory.h"

#if ((BCHP_CHIP==3548) || (BCHP_CHIP==3556)) && (BCHP_VER < BCHP_VER_B0)
#define P3D_IRQ_WORKAROUND 1
#endif

#include "bmem.h"
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

#include "pthread.h"

#include "nexus_driver_ioctl.h"

BDBG_MODULE(p3d_proxy);

#if (BCHP_CHIP == 7420)
#define P3D_MEMC1_DISPLAY 1
#endif

/* imports from the Nexus proxy layer */
extern int proxy_ioctl_fd;

static int g_native_breg_fd = -1;
static BINT_Handle g_p3d_hint;
static BMEM_Handle g_p3d_local_h_mem = NULL;
static BMEM_Handle g_p3d_sur_h_mem = NULL;
#if P3D_MEMC1_DISPLAY
static BMEM_Handle g_p3d_memc1_h_mem = NULL;
#endif

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
static BINT_Settings p3d_int_settings;

static int g_p3d_int_thread_done = 0;
static int g_p3d_int_thread_started = 0;
static pthread_t int_thread;
static pthread_attr_t int_thread_attr;
static void *b_3d_int_thread(void *data);

/************************************************************************/
/* Miniature fake BCHP layer for P3D only (implemented in graphics3d.c) */
/************************************************************************/

/* Graphics3D is being built in proxy mode, which requires a shim layer and 
 * a few duplicated PI modules.  This replaces p3ddrv in settopapi for 7400.
 */

/* Miniature fake BCHP layer for P3D only */
BCHP_P_Context fakeChip = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};
BERR_Code BCHP_OpenFakeChip(BCHP_Handle *phChip, BREG_Handle hRegister) {
    BSTD_UNUSED(hRegister);
    *phChip = &fakeChip;
    return BERR_SUCCESS;
}

/* Note that this shim version of BCHP_GetFeature only returns support for 3D. */
BERR_Code BCHP_GetFeature(const BCHP_Handle hChip, const BCHP_Feature eFeature, void *pFeatureValue) {
    BSTD_UNUSED(hChip);
    switch (eFeature) {
    case BCHP_Feature_e3DGraphicsCapable:
        *(bool *)pFeatureValue = true;
        break;
    default:
        BDBG_ERR(("Fake BCHP layer has been asked for feature %d",eFeature));
        *(bool *)pFeatureValue = false;
        break;
    }
    return BERR_SUCCESS;
}

static
BPXL_Format p3d_proxy_nexus2pxl(NEXUS_PixelFormat nexus_format)
{
    switch (nexus_format)
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
        BDBG_ERR(("p3d_proxy_nexus2pxl: unsupported format %08x!",nexus_format));
        return 0;
    }
}

/************************************************************************/
/* Miniature fake BINT layer for P3D only                               */
/************************************************************************/
static void P3D_P_SetInt( BREG_Handle regHandle, uint32_t baseAddr, int shift )
{
    BDBG_MSG(("P3D_P_SetInt(%08x,%d)",baseAddr,shift));
    BSTD_UNUSED(regHandle);
    BSTD_UNUSED(baseAddr);
    BSTD_UNUSED(shift);
}

static void P3D_P_ClearInt( BREG_Handle regHandle, uint32_t baseAddr, int shift )
{
    BDBG_MSG(("P3D_P_ClearInt(%08x,%d)",baseAddr,shift));
    BSTD_UNUSED(regHandle);
    BSTD_UNUSED(baseAddr);
    BSTD_UNUSED(shift);
}

static void P3D_P_SetMask( BREG_Handle regHandle, uint32_t baseAddr, int shift )
{
    BDBG_MSG(("P3D_P_SetMask(%08x,%d)",baseAddr,shift));
    BSTD_UNUSED(regHandle);
    BSTD_UNUSED(baseAddr);
    BSTD_UNUSED(shift);
}

static void P3D_P_ClearMask( BREG_Handle regHandle, uint32_t baseAddr, int shift )
{
    BDBG_MSG(("P3D_P_ClearMask(%08x,%d)",baseAddr,shift));
    BSTD_UNUSED(regHandle);
    BSTD_UNUSED(baseAddr);
    BSTD_UNUSED(shift);
}

static uint32_t P3D_P_ReadMask( BREG_Handle regHandle, uint32_t baseAddr )
{
    uint32_t rc;
    BDBG_MSG(("P3D_P_ReadMask(%08x)",baseAddr));
    BSTD_UNUSED(regHandle);
    switch (baseAddr) {
    case 0x01200000:		/* 7400 */
        rc = 0x00000008;	/* PX3D_2_INTR */
        break;
    case 0x00c50000:
        rc = 0x00000100;
        break;
    case 0x00303100:
        rc = 0x00a00500;
        break;
    case 0x00129200:	/* 7405 (7413) */
        rc = 0x00000004;
        break;
    case 0x00483000: /* 7125 && 7420 */
        rc = 0x00000008;
        break;
    default:
        BDBG_ERR(("P3D_P_ReadMask: unrecognized base address 0x%08x",baseAddr));
        rc = 0;
    }
    return rc;
}

static uint32_t P3D_P_ReadStatus( BREG_Handle regHandle, uint32_t baseAddr )
{
    uint32_t rc;
    BDBG_MSG(("P3D_P_ReadStatus(%08x)",baseAddr));
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
    case 0x00483000: /* 7125 && 7420 */
        rc = 0x00000008;
        break;
    default:
        BDBG_ERR(("P3D_P_ReadMask: unrecognized base address 0x%08x",baseAddr));
        rc = 0;
    }
    return rc;
}

/***************************************************************************/
/* PRIVATE */
/***************************************************************************/
NEXUS_SurfaceHandle native_P_CreateNexusSurface( 
    BSUR_Surface_Handle hSurface,
    NEXUS_Rect *pRect )
{
    NEXUS_SurfaceCreateSettings surfaceCreateSettings;
    NEXUS_SurfaceHandle surfaceHandle;
    BPXL_Format format;
    uint32_t ulWidth = 0;
    uint32_t ulHeight = 0;
    uint32_t ulPitch = 0;
    void *pvMemory;
    void *pvCached = NULL;
    uint32_t offset;

    BSUR_Surface_GetFormat( hSurface, &format );
    BSUR_Surface_GetDimensions( hSurface, &ulWidth, &ulHeight );
    BSUR_Surface_GetAddress( hSurface, &pvMemory, &ulPitch );

    /* over-ride the above lookup with an address in the nexus address space */
    BMEM_ConvertAddressToOffset( g_p3d_sur_h_mem, pvMemory, &offset);
    pvCached = NEXUS_OffsetToCachedAddr(offset);

    NEXUS_Surface_GetDefaultCreateSettings( &surfaceCreateSettings );
    surfaceCreateSettings.width = ulWidth;
    surfaceCreateSettings.height = ulHeight;
    surfaceCreateSettings.pitch = ulPitch;
    surfaceCreateSettings.pMemory = pvCached;

    switch( format )
    {
    case BPXL_eR5_G6_B5:    surfaceCreateSettings.pixelFormat = NEXUS_PixelFormat_eR5_G6_B5; break;
    case BPXL_eA8_R8_G8_B8: surfaceCreateSettings.pixelFormat = NEXUS_PixelFormat_eA8_R8_G8_B8; break;
    case BPXL_eA1_R5_G5_B5: surfaceCreateSettings.pixelFormat = NEXUS_PixelFormat_eA1_R5_G5_B5; break;
    case BPXL_eA4_R4_G4_B4: surfaceCreateSettings.pixelFormat = NEXUS_PixelFormat_eA4_R4_G4_B4; break;
    default:                surfaceCreateSettings.pixelFormat = NEXUS_PixelFormat_eY08_Cb8_Y18_Cr8;
    }

    surfaceHandle = NEXUS_Surface_Create( &surfaceCreateSettings );
    if( surfaceHandle == 0 ) {
        BDBG_ERR(("native_P_CreateNexusSurface: error creating nexus surface!"));
        return 0;
    }

    pRect->width = (uint16_t) ulWidth;
    pRect->height = (uint16_t) ulHeight;

    return surfaceHandle;
}

/***************************************************************************/
void native_P_DestroyNexusSurface( 
    NEXUS_SurfaceHandle surfaceHandle )
{
    if( surfaceHandle )
        NEXUS_Surface_Destroy( surfaceHandle );
}

/***************************************************************************/
BPXL_Format native_P_ConvertFormat_M2MCtoPXL( uint32_t format )
{
	switch( format )
	{
	case BM2MC_PACKET_PixelFormat_eA8_R8_G8_B8:     return BPXL_eA8_R8_G8_B8;
	case BM2MC_PACKET_PixelFormat_eR5_G6_B5:        return BPXL_eR5_G6_B5;
	case BM2MC_PACKET_PixelFormat_eA1_R5_G5_B5:     return BPXL_eA1_R5_G5_B5;
	case BM2MC_PACKET_PixelFormat_eA4_R4_G4_B4:     return BPXL_eA4_R4_G4_B4;
	case BM2MC_PACKET_PixelFormat_eY18_Cr8_Y08_Cb8: return BPXL_eY18_Cr8_Y08_Cb8;
	case BM2MC_PACKET_PixelFormat_eY08_Cb8_Y18_Cr8: return BPXL_eY08_Cb8_Y18_Cr8;
	case BM2MC_PACKET_PixelFormat_eL8:              return BPXL_eL8;
	case BM2MC_PACKET_PixelFormat_eA8:              return BPXL_eA8;
	case BM2MC_PACKET_PixelFormat_eL8_A8:           return BPXL_eL8_A8;
	default:                                        return BPXL_eZ16;
	}
}

BP3D_Handle g_hP3d = 0;

/***************************************************************************/
/* PUBLIC */
/***************************************************************************/
void native_P_InitDisplay( 
    NativeDisplay display,
    BMEM_Handle *phMem,
    BP3D_Handle *phP3d )
{
    BREG_Handle hreg;
    void *reg_addr;
    int reg_fd;
    BCHP_Handle hchip;

    /* support to declare a BMEM handle */
    BERR_Code brc;
    BMEM_Settings mem_settings;
    BMEM_Heap_Settings heap_settings;
    BMEM_ModuleHandle hmod;
    BMEM_Handle hmem;
    NEXUS_Error nrc;

    /* parameters for the 3d proxy local heap */
    void *local_mem_addr = NULL;
    unsigned long local_mem_offset = 0;
    unsigned long local_mem_size = 0;
    /* parameters for the uncached mmap heap */
    int global_mem_fd;
    void *global_mem_addr = NULL;
    unsigned long global_mem_offset = 0;
    unsigned long global_mem_size = 0;

    /* support to declare BINT */
    const BINT_Settings *int_settings;
    BINT_Handle hint;

    BSTD_UNUSED( display );

    /* Set up a user-mode P3D.  This requires the following handles:
     * BCHP, BREG, BMEM, BINT */

    /* Configure a BMEM handle */
    /* 32 MB is the default size for the local p3d-specific heap.
     * *ALL* OpenGL/PX3D allocations should occur on here, so that 
     * they can be de-referenced in both user and kernel space.
     * 
     * This value may need to be adjusted based on the application's 
     * specific use.  32M is adequate for bTV and the other demos. */
#if (BCHP_CHIP==3548) || (BCHP_CHIP==3556) || (BCHP_CHIP==7405) || (BCHP_CHIP==7420) || (BCHP_CHIP==7125) || (BCHP_CHIP==7342) || (BCHP_CHIP==7340) || (BCHP_CHIP==7408)
    local_mem_size = 32*1024*1024;
#elif BCHP_CHIP==7400
    local_mem_size = 16*1024*1024;
#else
#error "Unsupported chip in graphics3d proxy mode"
#endif
    nrc = NEXUS_Memory_Allocate( local_mem_size, NULL, &local_mem_addr );
    if (nrc) {
        BDBG_ERR(("NEXUS_Memory_Allocate failed: %d",nrc));
    }
    if (!local_mem_addr) {
        BDBG_ERR(("Failed to allocate memory"));
    }

    /* Set up the data for the local heap, based on the allocation */
    local_mem_offset = NEXUS_AddrToOffset(local_mem_addr);

    /* Heap creation settings */
    brc = BMEM_GetDefaultSettings(&mem_settings);
    brc = BMEM_Open(&hmod, &mem_settings);

#if P3D_MEMC1_DISPLAY
    heap_settings.uiAlignment = 8;
#else
    heap_settings.uiAlignment = 6;
#endif
    heap_settings.eSafetyConfig = BMEM_SafetyConfig_eFastest;
    heap_settings.eBookKeeping = BMEM_BOOKKEEPING_SYSTEM;
    heap_settings.pCachedAddress = NULL;
    heap_settings.flush = NULL;
    heap_settings.flush_isr = NULL;

    /* Create an uncached mmap */
    global_mem_fd = open("/dev/mem", O_RDWR|O_SYNC);
    global_mem_addr = mmap(0,0x0C000000,PROT_READ|PROT_WRITE,MAP_SHARED,global_mem_fd,0x04000000);
    global_mem_offset = 0x04000000;
    global_mem_size = 0x0C000000;

    /* ...and create a heap address space.  This heap should NEVER be 
     * allocated from, as it is shared with kernel space.  It is used 
     * solely to remap uncached pointers to memory used and accessible 
     * in both user and kernel space. */
    brc = BMEM_Heap_Create(hmod, global_mem_addr, global_mem_offset, global_mem_size, &heap_settings, &hmem);
    g_p3d_sur_h_mem = hmem;

#if P3D_MEMC1_DISPLAY
    global_mem_addr = mmap(0,64*1024*1024,PROT_READ|PROT_WRITE,MAP_SHARED,global_mem_fd,0x60000000);
    brc = BMEM_Heap_Create(hmod, global_mem_addr, 0x60000000, 64*1024*1024, &heap_settings, &hmem);
    g_p3d_memc1_h_mem = hmem;
#endif

    /* get a pointer in the uncached address space to the memory we allocated earlier,
     * then create a heap off of it */
    BMEM_ConvertOffsetToAddress( g_p3d_sur_h_mem, local_mem_offset, &local_mem_addr);
    brc = BMEM_Heap_Create(hmod, local_mem_addr, local_mem_offset, local_mem_size, &heap_settings, &hmem);
    g_p3d_local_h_mem = hmem;
    *phMem = hmem;

    /* Configure a BREG handle */
    reg_fd = open("/dev/mem", O_RDWR|O_SYNC);
#if (BCHP_CHIP==3548) || (BCHP_CHIP==3556)
    reg_addr = mmap(0, BCHP_PX3D_TIME_STATUS+4, PROT_READ|PROT_WRITE, MAP_SHARED, reg_fd, BCHP_PHYSICAL_OFFSET);
#elif BCHP_CHIP==7400 || (BCHP_CHIP==7125) || (BCHP_CHIP==7340) || (BCHP_CHIP==7342) || (BCHP_CHIP==7408)
    reg_addr = mmap(0, BCHP_PX3D_REG_END+4, PROT_READ|PROT_WRITE, MAP_SHARED, reg_fd, BCHP_PHYSICAL_OFFSET);
#elif BCHP_CHIP==7405
    reg_addr = mmap(0, BCHP_MEM_DMA_SECURE_REG_END+4, PROT_READ|PROT_WRITE, MAP_SHARED, reg_fd, BCHP_PHYSICAL_OFFSET);
#elif BCHP_CHIP==7420
    reg_addr = mmap(0, BCHP_REGISTER_END, PROT_READ|PROT_WRITE, MAP_SHARED, reg_fd, BCHP_PHYSICAL_OFFSET);
#else
#error "Unsupported chip in graphics3d proxy mode"
#endif
    if (reg_addr == MAP_FAILED) {
        BDBG_ERR(("Failed to map registers"));
    }
    BREG_Open(&hreg, reg_addr, BCHP_PHYSICAL_OFFSET);
    g_native_breg_fd = reg_fd;

    /* Configure a (faked) BCHP handle.  This is to allow p3d to do a 
     * check to see if the chip supports 3D.  The shim functions are 
     * in graphics3d.c
     * 
     * This is faked, because the full BCHP_OpenXXXX() functions reset 
     * many of the cores.
     */
    BCHP_OpenFakeChip(&hchip, hreg);

    /* set up our own BINT layer */
#if (BCHP_CHIP==3548) || (BCHP_CHIP==3556)
    int_settings = BINT_3548_GetSettings();
#elif BCHP_CHIP==7400
    int_settings = BINT_7400_GetSettings();
#elif BCHP_CHIP==7405
    int_settings = BINT_7405_GetSettings();
#elif BCHP_CHIP==7125
    int_settings = BINT_7125_GetSettings();
#elif BCHP_CHIP==7340
    int_settings = BINT_7340_GetSettings();
#elif BCHP_CHIP==7342
    int_settings = BINT_7342_GetSettings();
#elif BCHP_CHIP==7420
    int_settings = BINT_7420_GetSettings();
#elif BCHP_CHIP==7408
    int_settings = BINT_7408_GetSettings();
#else
#error "Unsupported chip in graphics3d proxy mode"
#endif
    p3d_int_settings.name = int_settings->name;

    /* Override the writes.  These shim functions are earlier in this file. */
    p3d_int_settings.pReadMask = P3D_P_ReadMask;
    p3d_int_settings.pReadStatus = P3D_P_ReadStatus;
    p3d_int_settings.pClearInt = P3D_P_ClearInt;
    p3d_int_settings.pClearMask = P3D_P_ClearMask;
    p3d_int_settings.pSetInt = P3D_P_SetInt;
    p3d_int_settings.pSetMask = P3D_P_SetMask;
    p3d_int_settings.pIntMap = px3d_bint_FAKE;

    brc = BINT_Open(&hint, hreg, &p3d_int_settings);
    g_p3d_hint = hint; 

    if (proxy_ioctl_fd != -1) {
        int rc;
        PROXY_NEXUS_3dPowerManagement data;
 
        data.pm_state = true;
        rc = ioctl(proxy_ioctl_fd, IOCTL_PROXY_NEXUS_3dPowerManagement, &data);
        if(rc!=0) {BERR_TRACE(NEXUS_OS_ERROR);}
    }

    /* and finally, use these to create a p3d handle */
    if( g_hP3d == 0 )
        brc = BP3D_Open(&g_hP3d, hchip, hreg, g_p3d_local_h_mem, hint);
    *phP3d = g_hP3d;
}

/***************************************************************************/
void native_P_TermDisplay( 
	NativeDisplay display )
{
    BSTD_UNUSED( display );
    if( g_hP3d )
        BP3D_Close(g_hP3d);
    g_hP3d = 0;
}

/***************************************************************************/
typedef struct stored_structs {
    void *memory;
    BSUR_Surface_Handle hsur;
    int width;
    int height;
    uint32_t offset;
    NEXUS_PixelFormat pixelFormat;
} stored_structs_t;

#define NATIVE_NUM_CACHED_INITWINDOW_SURFACES 10
static stored_structs_t created_initwindow_surfaces[NATIVE_NUM_CACHED_INITWINDOW_SURFACES+1] = {{NULL,NULL,0,0,0,0}};

/* this triggers whether IOCTL_PROXY_NEXUS_GetSurfaceOffset is used to 
 * gain information about the surface before remapping it for use, or 
 * whether Nexus calls are used.  Some use cases are marginally faster one way, 
 * some are the other.  This define allows both cases to persist for
 * benchmarking.
 */
#define REMAP_SURFACES_WITHOUT_IOCTL 1
/***************************************************************************/
void native_P_InitWindow( 
    NativeWindow window,
    BSUR_Surface_Handle *phSurface,
    unsigned int *puiFormat,
    unsigned int *puiOffset,
    void **ppvMemory,
    int *piPitch,
    int *piWidth,
    int *piHeight )
{
#if REMAP_SURFACES_WITHOUT_IOCTL
    NEXUS_SurfaceHandle sur = (NEXUS_SurfaceHandle) window;
    NEXUS_SurfaceMemory sur_memory;
    NEXUS_SurfaceCreateSettings sur_settings;
    BSUR_Surface_Settings bsurface_settings;
    BM2MC_PACKET_Plane plane;
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

    NEXUS_Surface_GetMemory(sur, &sur_memory);
    NEXUS_Surface_GetCreateSettings(sur, &sur_settings);
    NEXUS_Surface_InitPlane( sur, &plane );

    for (ix=0; ix < NATIVE_NUM_CACHED_INITWINDOW_SURFACES; ix++) {
        if (!created_initwindow_surfaces[ix].memory) {
            break;
        } else if ((created_initwindow_surfaces[ix].memory == sur_memory.buffer) && 
                   (created_initwindow_surfaces[ix].pixelFormat == sur_settings.pixelFormat)) {
            *phSurface = created_initwindow_surfaces[ix].hsur;
            *puiFormat = created_initwindow_surfaces[ix].pixelFormat;
            *puiOffset = created_initwindow_surfaces[ix].offset;
            *ppvMemory = sur_memory.buffer;
            *piPitch = sur_memory.pitch;
            *piWidth = created_initwindow_surfaces[ix].width;
            *piHeight = created_initwindow_surfaces[ix].height;
            return;
        }
    }

    *puiFormat = native_P_ConvertFormat_M2MCtoPXL(plane.format);
    *puiOffset = plane.address;
    *ppvMemory = sur_memory.buffer;
    *piPitch = sur_memory.pitch;
    *piWidth = sur_settings.width;
    *piHeight = sur_settings.height;
#if P3D_MEMC1_DISPLAY
    BMEM_Heap_ConvertOffsetToAddress(g_p3d_memc1_h_mem,NEXUS_AddrToOffset(sur_memory.buffer),&sur_addr);
#else
    BMEM_Heap_ConvertOffsetToAddress(g_p3d_local_h_mem,NEXUS_AddrToOffset(sur_memory.buffer),&sur_addr);
#endif
    BSUR_Surface_GetDefaultSettings(&bsurface_settings);
    BSUR_Surface_Create(g_p3d_local_h_mem, sur_settings.width, sur_settings.height, sur_memory.pitch, sur_addr, p3d_proxy_nexus2pxl(sur_settings.pixelFormat), NULL, BSUR_CONSTRAINT_ADDRESS_PIXEL_ALIGNED, &bsurface_settings, phSurface);

    if (ix < NATIVE_NUM_CACHED_INITWINDOW_SURFACES) {
        created_initwindow_surfaces[ix].memory = sur_memory.buffer;
        created_initwindow_surfaces[ix].hsur = *phSurface;
        created_initwindow_surfaces[ix].width = *piWidth;
        created_initwindow_surfaces[ix].height = *piHeight;
        created_initwindow_surfaces[ix].pixelFormat = sur_settings.pixelFormat;
    } else {
        BDBG_ERR(("Out of cached pixmaps!  Please increase the value of NATIVE_NUM_CACHED_INITWINDOW_SURFACES at %s:%d",__FILE__,__LINE__));
    }
#else
    NEXUS_SurfaceHandle sur = (NEXUS_SurfaceHandle) window;

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
        brc = BMEM_Heap_ConvertOffsetToAddress(g_p3d_local_h_mem,data.offset,&sur_addr);

        for (ix=0; ix < NATIVE_NUM_CACHED_INITWINDOW_SURFACES; ix++) {
            if (!created_initwindow_surfaces[ix].offset) {
                break;
            } else if (created_initwindow_surfaces[ix].offset == data.offset) {
                *phSurface = created_initwindow_surfaces[ix].hsur;
                *piWidth = created_initwindow_surfaces[ix].width;
                *piHeight = created_initwindow_surfaces[ix].height;
                return;
            }
        }

        *piWidth = data.width;
        *piHeight = data.height;
        BSUR_Surface_GetDefaultSettings(&bsurface_settings);
        BSUR_Surface_Create(g_p3d_local_h_mem, data.width, data.height, data.pitch, sur_addr, p3d_proxy_nexus2pxl(data.pixel_format), NULL, BSUR_CONSTRAINT_ADDRESS_PIXEL_ALIGNED, &bsurface_settings, phSurface);

        if (ix < NATIVE_NUM_CACHED_INITWINDOW_SURFACES) {
            created_initwindow_surfaces[ix].offset = data.offset;
            created_initwindow_surfaces[ix].hsur = *phSurface;
            created_initwindow_surfaces[ix].width = *piWidth;
            created_initwindow_surfaces[ix].height = *piHeight;
        } else {
            BDBG_ERR(("Out of cached pixmaps!  Please increase the value of NATIVE_NUM_CACHED_INITWINDOW_SURFACES at %s:%d",__FILE__,__LINE__));
        }
    }
#endif
}

#define NATIVE_NUM_CACHED_INITPIXMAP_SURFACES 50
static stored_structs_t created_initpixmap_surfaces[NATIVE_NUM_CACHED_INITPIXMAP_SURFACES+1] = {{NULL,NULL,0,0,0,0}};
/***************************************************************************/
void native_P_InitPixmap( 
    NativePixmap pixmap,
    BSUR_Surface_Handle *phSurface,
    unsigned int *puiFormat,
    unsigned int *puiOffset,
    void **ppvMemory,
    int *piPitch,
    int *piWidth,
    int *piHeight )
{
#if REMAP_SURFACES_WITHOUT_IOCTL
    NEXUS_SurfaceHandle sur = (NEXUS_SurfaceHandle) pixmap;
    NEXUS_SurfaceMemory sur_memory;
    NEXUS_SurfaceCreateSettings sur_settings;
    BSUR_Surface_Settings bsurface_settings;
    BM2MC_PACKET_Plane plane;
    int ix;
    void *sur_addr = NULL;

    NEXUS_Surface_GetMemory(sur, &sur_memory);
    NEXUS_Surface_GetCreateSettings(sur, &sur_settings);
    NEXUS_Surface_InitPlane( sur, &plane );

    for (ix=0; ix < NATIVE_NUM_CACHED_INITPIXMAP_SURFACES; ix++) {
        if (!created_initpixmap_surfaces[ix].memory) {
            break;
        } else if ((created_initpixmap_surfaces[ix].memory == sur_memory.buffer) && 
                   (created_initpixmap_surfaces[ix].pixelFormat == sur_settings.pixelFormat)) {
            *phSurface = created_initpixmap_surfaces[ix].hsur;
            *puiFormat = created_initwindow_surfaces[ix].pixelFormat;
            *puiOffset = created_initwindow_surfaces[ix].offset;
            *ppvMemory = sur_memory.buffer;
            *piPitch = sur_memory.pitch;
            *piWidth = created_initpixmap_surfaces[ix].width;
            *piHeight = created_initpixmap_surfaces[ix].height;
            return;
        }
    }

    *puiFormat = native_P_ConvertFormat_M2MCtoPXL(plane.format);
    *puiOffset = plane.address;
    *ppvMemory = sur_memory.buffer;
    *piPitch = sur_memory.pitch;
    *piWidth = sur_settings.width;
    *piHeight = sur_settings.height;
    BMEM_Heap_ConvertOffsetToAddress(g_p3d_local_h_mem,NEXUS_AddrToOffset(sur_memory.buffer),&sur_addr);
    BSUR_Surface_GetDefaultSettings(&bsurface_settings);
    BSUR_Surface_Create(g_p3d_local_h_mem, sur_settings.width, sur_settings.height, sur_memory.pitch, sur_addr, p3d_proxy_nexus2pxl(sur_settings.pixelFormat), NULL, BSUR_CONSTRAINT_ADDRESS_PIXEL_ALIGNED, &bsurface_settings, phSurface);

    if (ix < NATIVE_NUM_CACHED_INITPIXMAP_SURFACES) {
        created_initpixmap_surfaces[ix].memory = sur_memory.buffer;
        created_initpixmap_surfaces[ix].hsur = *phSurface;
        created_initpixmap_surfaces[ix].width = *piWidth;
        created_initpixmap_surfaces[ix].height = *piHeight;
        created_initpixmap_surfaces[ix].pixelFormat = sur_settings.pixelFormat;
    } else {
        BDBG_ERR(("Out of cached pixmaps!  Please increase the value of NATIVE_NUM_CACHED_INITPIXMAP_SURFACES at %s:%d",__FILE__,__LINE__));
    }
#else
    NEXUS_SurfaceHandle sur = (NEXUS_SurfaceHandle) window;

    if (proxy_ioctl_fd != -1) {
        int rc;
        BERR_Code brc;
        PROXY_NEXUS_GetSurfaceOffset data;
        int ix;
        void *sur_addr = NULL;

        data.surface = sur;
        rc = ioctl(proxy_ioctl_fd, IOCTL_PROXY_NEXUS_GetSurfaceOffset, &data);
        if(rc!=0) {BERR_TRACE(NEXUS_OS_ERROR);}
        brc = BMEM_Heap_ConvertOffsetToAddress(g_p3d_local_h_mem,data.offset,&sur_addr);

        for (ix=0; ix < NATIVE_NUM_CACHED_INITPIXMAP_SURFACES; ix++) {
            if (!created_initpixmap_surfaces[ix].offset) {
                break;
            } else if (created_initpixmap_surfaces[ix].offset == data.offset) {
                *phSurface = created_initpixmap_surfaces[ix].hsur;
                *piWidth = created_initpixmap_surfaces[ix].width;
                *piHeight = created_initpixmap_surfaces[ix].height;
                return;
            }
        }

        *piWidth = data.width;
        *piHeight = data.height;
        BSUR_Surface_GetDefaultSettings(&bsurface_settings);
        BSUR_Surface_Create(g_p3d_local_h_mem, data.width, data.height, data.pitch, sur_addr, p3d_proxy_nexus2pxl(data.pixel_format), NULL, BSUR_CONSTRAINT_ADDRESS_PIXEL_ALIGNED, &bsurface_settings, phSurface);

        if (ix < NATIVE_NUM_CACHED_INITPIXMAP_SURFACES) {
            created_initpixmap_surfaces[ix].offset = data.offset;
            created_initpixmap_surfaces[ix].hsur = *phSurface;
            created_initpixmap_surfaces[ix].width = *piWidth;
            created_initpixmap_surfaces[ix].height = *piHeight;
        } else {
            BDBG_ERR(("Out of cached pixmaps!  Please increase the value of NATIVE_NUM_CACHED_INITPIXMAP_SURFACES at %s:%d",__FILE__,__LINE__));
        }
    }
#endif
}

/***************************************************************************/
void native_P_CopySurface( 
    NativeDisplay display,
    BSUR_Surface_Handle hDstSurface,
    BSUR_Surface_Handle hSrcSurface )
{
    NEXUS_Graphics2DHandle gfx = (NEXUS_Graphics2DHandle) display;

    NEXUS_Rect dstRect = { 0, 0, 0, 0 };
    NEXUS_Rect srcRect = { 0, 0, 0, 0 };

    NEXUS_SurfaceHandle dstSurface = native_P_CreateNexusSurface( hDstSurface, &dstRect );
    NEXUS_SurfaceHandle srcSurface = native_P_CreateNexusSurface( hSrcSurface, &srcRect );

    if( dstSurface && srcSurface ) {
        NEXUS_Error nrc;
        nrc = NEXUS_Graphics2D_FastBlit( gfx, dstSurface, &dstRect, srcSurface, &srcRect, 
            NEXUS_BlitColorOp_eCopySource, NEXUS_BlitAlphaOp_eCopySource, 0 );
        if (nrc) {
            BDBG_ERR(("native_P_CopySurface: error nrc: %d copying surface (probably creating a mipmap)",nrc));
        }
    }

    if( srcSurface )
        native_P_DestroyNexusSurface( srcSurface );
    if( dstSurface )
        native_P_DestroyNexusSurface( dstSurface );
}

/***************************************************************************/
void native_P_BlitSurface( 
    EGL_P_Context *pContext,
    NativeDisplay display,
    BSUR_Surface_Handle hDstSurface,
    int iDstX, int iDstY, int iDstWidth, int iDstHeight,
    BSUR_Surface_Handle hSrcSurface,
    int iSrcX, int iSrcY, int iSrcWidth, int iSrcHeight,
    float fScaleX, float fScaleY,
    float fPhaseX, float fPhaseY,
    uint32_t ulColor,
    GLboolean bSame )
{
    BSTD_UNUSED(pContext);
    BSTD_UNUSED(display);
    BSTD_UNUSED(hDstSurface);
    BSTD_UNUSED(iDstX);
    BSTD_UNUSED(iDstY);
    BSTD_UNUSED(iDstWidth);
    BSTD_UNUSED(iDstHeight);
    BSTD_UNUSED(hSrcSurface);
    BSTD_UNUSED(iSrcX);
    BSTD_UNUSED(iSrcY);
    BSTD_UNUSED(iSrcWidth);
    BSTD_UNUSED(iSrcHeight);
    BSTD_UNUSED(fScaleX);
    BSTD_UNUSED(fScaleY);
    BSTD_UNUSED(fPhaseX);
    BSTD_UNUSED(fPhaseY);
    BSTD_UNUSED(ulColor);
    BSTD_UNUSED(bSame);

}

void native_P_BlitWait( 
    EGL_P_Context *pContext,
    NativeDisplay display,
    BSUR_Surface_Handle hSurface )
{
    BSTD_UNUSED(pContext);
    BSTD_UNUSED(display);
    BSTD_UNUSED(hSurface);
}

/* interrupt monitoring thread */
static int g_3d_irq_calls = 0;
static void *b_3d_int_thread(void *data)
{
    BSTD_UNUSED(data);

    g_p3d_int_thread_started = true;
    if (proxy_ioctl_fd != -1) {
        int rc;
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
        if(rc!=0) {BERR_TRACE(NEXUS_OS_ERROR);}
    } else {
        BDBG_ERR(("b_3d_int_thread: =====> proxy_ioctl_fd seems to be uninitialized!!!"));
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
            g_3d_irq_calls++;
            if (data.was_fired) {
                BKNI_EnterCriticalSection();
#if P3D_IRQ_WORKAROUND
                BINT_Isr(g_p3d_hint, BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_BVNF_CPU_INTR_1_SHIFT);
#else
#if (BCHP_CHIP==7400)
                BINT_Isr(g_p3d_hint, BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_PX3D_CPU_INTR_SHIFT + 32);
#elif (BCHP_CHIP==3548) || (BCHP_CHIP==3556)
                BINT_Isr(g_p3d_hint, BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_GFX_CPU_INTR_SHIFT + 32);
#elif (BCHP_CHIP==7405)
                BINT_Isr(g_p3d_hint, BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_PX3D_CPU_INTR_SHIFT);
#elif (BCHP_CHIP==7125) || (BCHP_CHIP==7420) || (BCHP_CHIP==7342) || (BCHP_CHIP==7340) || (BCHP_CHIP==7408)
                BINT_Isr(g_p3d_hint, BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_GFX_CPU_INTR_SHIFT);
#else
#error "Unsupported chip in graphics3d proxy mode"
#endif
#endif /* P3D_IRQ_WORKAROUND */
                BKNI_LeaveCriticalSection();
            }

        }
    }
    if (proxy_ioctl_fd != -1) {
        int rc;
        PROXY_NEXUS_Unhook3dInterrupt data;
#if (BCHP_CHIP==7408)
	data.which_interrupt = BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_BVNF_1_CPU_INTR_SHIFT;
#else
        data.which_interrupt = BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_BVNF_CPU_INTR_1_SHIFT;
#endif
        rc = ioctl(proxy_ioctl_fd, IOCTL_PROXY_NEXUS_Unhook3dInterrupt, &data);
        if(rc!=0) {BERR_TRACE(NEXUS_OS_ERROR);}
    }
    return NULL;
}

/* End of File */
