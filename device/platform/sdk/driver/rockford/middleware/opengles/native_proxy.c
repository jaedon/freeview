/***************************************************************************
 *     Copyright (c) 2007-2008, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: native_proxy.c $
 * $brcm_Revision: Hydra_Software_Devel/6 $
 * $brcm_Date: 11/5/08 12:25p $
 *
 * Module Description: Native entry point functions for the proxy driver.
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/middleware/opengles/native_proxy.c $
 * 
 * Hydra_Software_Devel/6   11/5/08 12:25p mphillip
 * PR48687: Stub the new native_P_Blit functions to enable building.
 * 
 * Hydra_Software_Devel/5   10/22/08 8:10p nissen
 * PR 47303: Added support for calling new SUR surface create function.
 * 
 * Hydra_Software_Devel/4   1/7/08 4:35p mphillip
 * PR35854: Clean up two warnins
 * 
 * Hydra_Software_Devel/3   12/18/07 7:46p mphillip
 * PR35854: Create user-space bchp shim for p3d's query
 * 
 * Hydra_Software_Devel/2   12/6/07 9:14p mphillip
 * PR35854: Initial commit of proxy bindings for opengles
 * 
 * 
 ***************************************************************************/

#include "native_private.h"
#include "native_proxy.h"

#include <sys/ioctl.h>
#include <linux/ioctl.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "bmem.h"
#include "bchp_common.h"
#include "bchp_7400.h"
#include "bchp_priv.h"
#include "p3ddrv_ioctl.h"

#include "bsettop_base_mem.h"

BDBG_MODULE(p3d_proxy);

/* Temporary: copy this struct from api/src/magnum/bsettop_impl.h due to 
 * proxy include issues */
#include "blst_queue.h"
#include "bkni.h"
struct bsurface {
    bgraphics_t graphics; /* TODO: this is a poor design decision. there's no reason why a block of
        memory should be tied to a particular graphics engine. please reconsider when refactoring
        the API in the future. */
    bsurface_memory mem;
    bsurface_create_settings create_settings;
    bsurface_settings settings;

    bool destroy; /* If true, then destroy after fill. */
    bsettop_rect fill_rect; /* If the surface was moved or hidden, or if it has an
                            alpha value, then we must fill. fill_rect width &&
                            height will be nonzero. */

    /* Magnum data */
    BSUR_Surface_Handle surface;
    BSUR_Palette_Handle palette;
    unsigned long *palette_mem;
    int palette_num;

    /* Double or triple buffered framebuffer */
    BSUR_Surface_Handle surface1;
    bsurface_memory mem1;
    BSUR_Surface_Handle surface2;
    bsurface_memory mem2;

    /* Implicitly externally allocated memory */
    bsurface_memory allocatedSurfaceMem;
    bsurface_memory allocatedSurfaceMem1;
    bsurface_memory allocatedSurfaceMem2;

    BLST_Q_ENTRY(bsurface) link; /* see bgraphics.surfaces */
};

struct bgraphics {
    bsurface_create_settings framebuffer_settings;
    bgraphics_settings settings;
    bdisplay_t display;
    bsurface_t sync_surface; /* used for bsurface_sync if async_blits == true */
    struct b_base_graphics *base;
    struct bsurface *frame_buffer;
    bool double_buffered;
    bool initialized;
    bool no_wait_after_graphics_sync; /* used for not waiting after triple buffered flip */
    BKNI_EventHandle isr_event; /* convert GRC isr's to task time callbacks */
    BGRC_Handle grc;

    /* list of non-framebuffer surfaces which may need to be blitted at bgraphics_sync time */
    BLST_Q_HEAD(surfaceshead, bsurface) surfaces;
};

#include "bsur_private.h"

#include "bint_7400.h"
#include "bint.h"
#include "bint_plat.h"
#include "bchp_px3d_l2.h"
#include "bchp_hif_cpu_intr1.h"

#include "pthread.h"

static int g_ioctl_fd = -1;
static const BINT_P_IntMap px3d_bint_7400A0[] =
{
    { BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_PX3D_CPU_INTR_SHIFT +32,            BCHP_PX3D_L2_CPU_STATUS,               0,                   "PX3D"},
    { -1, 0, 0, NULL}
};
static BINT_Settings p3d_int_settings;

static BINT_Handle g_p3d_hint;
static BMEM_Handle g_p3d_local_h_mem = NULL;
static BMEM_Handle g_p3d_sur_h_mem = NULL;

static pthread_t int_thread;
static pthread_attr_t int_thread_attr;
static bool int_thread_done = true;
static void *b_int_thread(void *data);

/* Miniature fake BCHP layer for P3D only */
BCHP_P_Context fakeChip = {NULL, NULL, NULL, NULL};
BERR_Code BCHP_OpenFake7400(BCHP_Handle *phChip, BREG_Handle hRegister) {
	BSTD_UNUSED(hRegister);
	*phChip = &fakeChip;
	return BERR_SUCCESS;
}

/* Note that this shim version of BCHP_GetFeature only returns support for 3D. */
BERR_Code BCHP_GetFeature(const BCHP_Handle hChip, const BCHP_Feature eFeature, void *pFeatureValue) {
	BSTD_UNUSED(hChip);
	if (eFeature == BCHP_Feature_e3DGraphicsCapable) {
		*(bool *)pFeatureValue = true;
	} else {
		*(bool *)pFeatureValue = false;
	}
	return BERR_SUCCESS;
}

static int pixel_format_remap(int old_format)
{
    int rv;
    switch (old_format){
    case  0: rv = BPXL_eR5_G6_B5; break;
    case  1: rv = BPXL_eA1_R5_G5_B5; break;
    case  2: rv = BPXL_eR5_G5_B5_A1; break;
    case  3: rv = BPXL_eA4_R4_G4_B4; break;
    case  4: rv = BPXL_eR4_G4_B4_A4; break;
    case  5: rv = BPXL_eA8_R8_G8_B8; break;
    case  6: rv = BPXL_eP8; break;
    case  7: rv = BPXL_eA8_P8; break;
    case  8: rv = BPXL_eZ16; break;
    case  9: rv = BPXL_eA8; break;
    case 10: rv = BPXL_eY08_Cb8_Y18_Cr8; break;
    case 11: rv = BPXL_eCr8_Y18_Cb8_Y08; break;
    case 12: rv = BPXL_eZ16; break;
    case 13: rv = BPXL_eP2; break;
    case 14: rv = BPXL_eP4; break;
    default:
        rv = old_format;
        BDBG_ERR(("Unrecognized pixel format: %d",old_format));
    }
    return rv;
}

/***************************************************************************/
void native_P_InitDisplay( 
    NativeDisplay display,
    BMEM_Handle *phMem,
    BP3D_Handle *phP3d )
{
    int rc;
    p3d_mem_request_t mem_request;
    void *k2u;
    unsigned long u2k;

    BERR_Code brc;
    BMEM_Settings settings;
    BMEM_Heap_Settings hsettings;
    unsigned long mem_offset;
    BMEM_ModuleHandle hmod;
    BMEM_Handle hmem;

    BCHP_Handle hchip;
    BREG_Handle hreg;
    void *reg_addr;
    int reg_fd;
    const BINT_Settings *int_settings;
    BINT_Handle hint;
    BP3D_Handle hP3d;
    
    BSTD_UNUSED( display );

    /* setup the memory handle
     * This calls into the kernel driver to allocate a block, converts the pointer to user space, 
     * and then creates a heap from the resulting memory block. */
    g_ioctl_fd = open("/dev/p3ddrv", O_RDWR);
    if (g_ioctl_fd < 0) {
        BDBG_ERR(("Unable to open p3d kernel device"));
        *phMem = NULL;
        *phP3d = NULL;
        return;
    }

    /* "[32M] should be enough for everyone" -- Actually, this can be changed as needed.
     * This is allocating a block of device memory in kernel space, translating it to user 
     * space, and then the block is set up as a user-space heap in BMEM.  This is the 
     * heap that P3D, OpenGL ES, and GFX3D should be using. */
    mem_request.size = 32 * 1024 * 1024;
    rc = ioctl(g_ioctl_fd, P3D_IOCTL_MEMORY_REQUEST, &mem_request);
    if (rc!=0) {
        BDBG_ERR(("p3d ioctl returned an error on memory request"));
    }

    k2u = b_kernel2user((unsigned long)mem_request.ptr,false);
    u2k = b_user2kernel(k2u);
    mem_offset = u2k; /* save the physical address we should have received */
    mem_offset = ~0xA0000000 & (uint32_t)mem_request.ptr;
    mem_request.ptr = b_kernel2user((unsigned long)mem_request.ptr,false);

    brc = BMEM_GetDefaultSettings(&settings);
    brc = BMEM_Open(&hmod, &settings);

    hsettings.uiAlignment = 6;
    hsettings.eSafetyConfig = BMEM_SafetyConfig_eFastest;
    hsettings.eBookKeeping = BMEM_BOOKKEEPING_SYSTEM;
    hsettings.pCachedAddress = NULL;
    hsettings.flush = NULL;
    hsettings.flush_isr = NULL;
    brc = BMEM_Heap_Create(hmod, mem_request.ptr, mem_offset, mem_request.size, &hsettings, &hmem);
    g_p3d_local_h_mem = hmem;
    *phMem = hmem;

    brc = BMEM_Heap_Create(hmod, b_sys_mem.addr, b_sys_mem.offset, b_sys_mem.size, &hsettings, &hmem);
    g_p3d_sur_h_mem = hmem; 

    /* now set up the p3d handle */
    reg_fd = open("/dev/mem", O_RDWR|O_SYNC);
    reg_addr = mmap(0, BCHP_PX3D_REG_END+4, PROT_READ|PROT_WRITE, MAP_SHARED, reg_fd, BCHP_PHYSICAL_OFFSET);
    if (reg_addr == MAP_FAILED) {
        BDBG_ERR(("Failed to map registers"));
    }
    BREG_Open(&hreg, reg_addr, BCHP_PHYSICAL_OFFSET);
    /* Open the fake 7400 BCHP layer, as the real one does a reset on the cores */
    BCHP_OpenFake7400(&hchip, hreg);
    
    /* set up our own BINT layer */
    int_settings = BINT_7400_GetSettings();
    p3d_int_settings.name = int_settings->name;
    p3d_int_settings.pClearInt = int_settings->pClearInt;
    p3d_int_settings.pClearMask = int_settings->pClearMask;
    p3d_int_settings.pReadMask = int_settings->pReadMask;
    p3d_int_settings.pReadStatus = int_settings->pReadStatus;
    p3d_int_settings.pSetInt = int_settings->pSetInt;
    p3d_int_settings.pSetMask = int_settings->pSetMask;
    p3d_int_settings.pIntMap = px3d_bint_7400A0;
    brc = BINT_Open(&hint, hreg, &p3d_int_settings);
    g_p3d_hint = hint; 
    
    brc = BP3D_Open(&hP3d, hchip, hreg, hmem, hint);
    *phP3d = hP3d;
    
    /* and not knowing where else to do this, set up the interrupt monitoring thread here */
    int_thread_done = false;

    pthread_attr_init(&int_thread_attr);
    pthread_attr_setstacksize(&int_thread_attr, 16*1024);

    if (pthread_create(&int_thread, &int_thread_attr, b_int_thread, NULL)) {
        BDBG_ERR(("Unable to create interrupt monitoring thread!"));
    }

}

typedef struct stored_structs {
	bsurface_t surface;
	void *memory;
	BSUR_Surface_Handle hsur;
	int width;
	int height;
} stored_structs_t;

#define NATIVE_NUM_CACHED_INITWINDOW_SURFACES 10
static stored_structs_t created_initwindow_surfaces[NATIVE_NUM_CACHED_INITWINDOW_SURFACES+1] = {{NULL,NULL,NULL,0,0}};

/***************************************************************************/
void native_P_InitWindow( 
    NativeWindow window,
    BSUR_Surface_Handle *phSurface,
    int *piWidth,
    int *piHeight )
{
    bsurface_t bsurface = (bsurface_t) window;
    bsurface_settings bsettings;
    p3d_sur_request_t sur_request;
    bsurface_memory sur_memory;
    void *k2u;
    unsigned long u2k;
    int rc;
    int ix;

    bsurface_get( bsurface, &bsettings );

    sur_request.bsurface = bsurface;
    rc = ioctl(g_ioctl_fd, P3D_IOCTL_SURFACE_REQUEST, &sur_request);
    if (rc!=0) {
        BDBG_ERR(("p3d ioctl returned an error on surface request"));
    }
    
    bsurface_get_memory(bsurface, &sur_memory);

    for (ix=0; ix < NATIVE_NUM_CACHED_INITWINDOW_SURFACES; ix++) {
    	if (!created_initwindow_surfaces[ix].memory) {
    		break;
    	} else if (created_initwindow_surfaces[ix].memory == sur_memory.buffer) {
    		*phSurface = created_initwindow_surfaces[ix].hsur;
    		*piWidth = created_initwindow_surfaces[ix].width;
    		*piHeight = created_initwindow_surfaces[ix].height;
    	    return;
    	}
    }
    
    u2k = b_user2kernel(sur_memory.buffer);
    k2u = b_kernel2user(u2k,false);
    sur_request.pixel_format = pixel_format_remap(sur_request.pixel_format);
#ifdef BSUR_SURFACE_CREATE_HAS_SETTINGS
    BSUR_Surface_Create(g_p3d_sur_h_mem, sur_request.width, sur_request.height, sur_memory.pitch, k2u, sur_request.pixel_format, sur_request.palette, BSUR_CONSTRAINT_ADDRESS_PIXEL_ALIGNED, 0, phSurface);
#else
    BSUR_Surface_Create(g_p3d_sur_h_mem, sur_request.width, sur_request.height, sur_memory.pitch, k2u, sur_request.pixel_format, sur_request.palette, BSUR_CONSTRAINT_ADDRESS_PIXEL_ALIGNED, phSurface);
#endif

    *piWidth = bsettings.position.width;
    *piHeight = bsettings.position.height;
    if (ix < NATIVE_NUM_CACHED_INITWINDOW_SURFACES) {
    	created_initwindow_surfaces[ix].memory = sur_memory.buffer;
    	created_initwindow_surfaces[ix].hsur = *phSurface;
    	created_initwindow_surfaces[ix].width = *piWidth;
    	created_initwindow_surfaces[ix].height = *piHeight;
    }
}

#define NATIVE_NUM_CACHED_INITPIXMAP_SURFACES 25
static stored_structs_t created_initpixmap_surfaces[NATIVE_NUM_CACHED_INITPIXMAP_SURFACES+1] = {{NULL,NULL,NULL,0,0}};

/***************************************************************************/
void native_P_InitPixmap( 
    NativePixmap pixmap,
    BSUR_Surface_Handle *phSurface,
    int *piWidth,
    int *piHeight )
{
    bsurface_t bsurface = (bsurface_t) pixmap;
    bsurface_settings bsettings;
    p3d_sur_request_t sur_request;
    bsurface_memory sur_memory;
    void *k2u;
    unsigned long u2k;
    int rc;
    int ix;

    bsurface_get( bsurface, &bsettings );
    sur_request.bsurface = bsurface;
    rc = ioctl(g_ioctl_fd, P3D_IOCTL_SURFACE_REQUEST, &sur_request);
    if (rc!=0) {
        BDBG_ERR(("p3d ioctl returned an error on surface request"));
    }
    
    bsurface_get_memory(bsurface, &sur_memory);
    for (ix=0; ix < NATIVE_NUM_CACHED_INITPIXMAP_SURFACES; ix++) {
    	if (!created_initpixmap_surfaces[ix].memory) {
    		break;
    	} else if (created_initpixmap_surfaces[ix].memory == sur_memory.buffer) {
    		*phSurface = created_initpixmap_surfaces[ix].hsur;
    		*piWidth = created_initpixmap_surfaces[ix].width;
    		*piHeight = created_initpixmap_surfaces[ix].height;
		return;
    	}
    }

    u2k = b_user2kernel(sur_memory.buffer);
    k2u = b_kernel2user(u2k,false);
    sur_request.pixel_format = pixel_format_remap(sur_request.pixel_format);
#ifdef BSUR_SURFACE_CREATE_HAS_SETTINGS
    BSUR_Surface_Create(g_p3d_sur_h_mem, sur_request.width, sur_request.height, sur_memory.pitch, k2u, sur_request.pixel_format, sur_request.palette, BSUR_CONSTRAINT_ADDRESS_PIXEL_ALIGNED, 0, phSurface);
#else
    BSUR_Surface_Create(g_p3d_sur_h_mem, sur_request.width, sur_request.height, sur_memory.pitch, k2u, sur_request.pixel_format, sur_request.palette, BSUR_CONSTRAINT_ADDRESS_PIXEL_ALIGNED, phSurface);
#endif

    *piWidth = bsettings.position.width;
    *piHeight = bsettings.position.height;
    if (ix < NATIVE_NUM_CACHED_INITPIXMAP_SURFACES) {
    	created_initpixmap_surfaces[ix].memory = sur_memory.buffer;
    	created_initpixmap_surfaces[ix].hsur = *phSurface;
    	created_initpixmap_surfaces[ix].width = *piWidth;
    	created_initpixmap_surfaces[ix].height = *piHeight;
    }
}

/***************************************************************************/
void native_P_CopySurface( 
    NativeDisplay display,
    BSUR_Surface_Handle hDstSurface,
    BSUR_Surface_Handle hSrcSurface )
{
    bgraphics_t bgraphics = (bgraphics_t) display;
    p3d_sur_copy_t sur_copy;
    int rc;

    sur_copy.bgraphics = bgraphics;
    /* fill up src */
    sur_copy.src_addr = (void *)b_user2kernel(hSrcSurface->pvAddress);
    sur_copy.src_width = hSrcSurface->ulWidth;
    sur_copy.src_height = hSrcSurface->ulHeight;
    sur_copy.src_pitch = hSrcSurface->ulPitch;
    sur_copy.src_constraint = hSrcSurface->ulConstraintMask ;
    sur_copy.src_uid = hSrcSurface->ulID;
    sur_copy.src_format = hSrcSurface->eFormat;
    sur_copy.src_palette = hSrcSurface->hPalette;
    sur_copy.src_driver_owned = hSrcSurface->bDriverOwned;
    /* fill up dest */
    sur_copy.dst_addr = (void *)b_user2kernel(hDstSurface->pvAddress);
    sur_copy.dst_width = hDstSurface->ulWidth;
    sur_copy.dst_height = hDstSurface->ulHeight;
    sur_copy.dst_pitch = hDstSurface->ulPitch;
    sur_copy.dst_constraint = hDstSurface->ulConstraintMask;
    sur_copy.dst_uid = hDstSurface->ulID;
    sur_copy.dst_format = hDstSurface->eFormat;
    sur_copy.dst_palette = hDstSurface->hPalette;
    sur_copy.dst_driver_owned = hDstSurface->bDriverOwned;
    
    BDBG_MSG(("native_P_CopySurface: src_addr: %p, dst_addr: %p",sur_copy.src_addr,sur_copy.dst_addr));

    rc = ioctl(g_ioctl_fd, P3D_IOCTL_SURFACE_COPY, &sur_copy);
    if (rc!=0) {
        BDBG_ERR(("p3d ioctl returned an error on surface copy"));
    }
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

/* p3d interrupt monitoring thread */
void *b_int_thread(void *data)
{
    BSTD_UNUSED(data);

    while (!int_thread_done) {
 
        p3d_int_request_t int_request;
        int_request.enable = true;

        pthread_testcancel();

        if (ioctl(g_ioctl_fd, P3D_IOCTL_CHECK_FOR_INTERRUPT, &int_request)) {
            /* this is normal on shutdown */
            BDBG_WRN(("b_int_thread exiting"));
            break;
        }
        if (int_request.was_fired) {
            BKNI_EnterCriticalSection();
            BINT_Isr(g_p3d_hint, BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_PX3D_CPU_INTR_SHIFT + 32);
            BKNI_LeaveCriticalSection();
        }

    }
    return NULL;
}

