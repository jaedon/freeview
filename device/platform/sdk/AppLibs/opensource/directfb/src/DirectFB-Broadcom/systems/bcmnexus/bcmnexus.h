/***************************************************************************
 *    (c)2005-2012 Broadcom Corporation
 *
 * This program is the proprietary software of Broadcom Corporation and/or its licensors,
 * and may only be used, duplicated, modified or distributed pursuant to the terms and
 * conditions of a separate, written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
 * no license (express or implied), right to use, or waiver of any kind with respect to the
 * Software, and Broadcom expressly reserves all rights in and to the Software and all
 * intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
 * HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
 * NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
 *
 * Except as expressly set forth in the Authorized License,
 *
 * 1.     This program, including its structure, sequence and organization, constitutes the valuable trade
 * secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of Broadcom integrated circuit products.
 *
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
 * AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
 * WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
 * THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
 * OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
 * LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
 * OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
 * USE OR PERFORMANCE OF THE SOFTWARE.
 *
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
 * LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
 * EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
 * USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
 * ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
 * LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
 * ANY LIMITED REMEDY.
 *
 * $brcm_Workfile: bcmnexus.h $
 * $brcm_Revision: DirectFB_1_4_15_Port/7 $
 * $brcm_Date: 3/1/12 7:58p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /AppLibs/opensource/directfb/src/broadcom_files/private/DirectFB/1.4.15/systems/bcmnexus/bcmnexus.h $
 *
 * DirectFB_1_4_15_Port/7   3/1/12 7:58p christ
 * SW7420-2273: DirectFB-1.4.15: Use correct Nexus heap for secondary
 *  display frame buffers
 *
 * DirectFB_1_4_15_Port/6   2/9/12 2:33p christ
 * SW7425-1550: DirectFB: Require integration of Nexus Surface Compositor
 *  into DFB
 *
 * DirectFB_1_4_15_Port/DirectFB-XS_1_4_15_Port/2   1/27/12 7:02p christ
 * SW7231-625: DirectFB-XS: Allow Nexus client only builds
 *
 * DirectFB_1_4_15_Port/5   1/10/12 12:34p christ
 * SW7231-580: DirectFB: Graphics card version checking / code cleanup
 *
 * DirectFB_1_4_15_Port/DirectFB-XS_1_4_15_Port/1   12/8/11 3:34p robertwm
 * SW7425-1550: DirectFB: Require integration of Nexus Surface Compositor
 *  into DFB.
 *
 * DirectFB_1_4_15_Port/4   12/6/11 4:13p apandav
 * SW7405-5561: DirectFB: DirectFB build fails due to Robust Mutex defines
 *  on BCM97405
 *
 * DirectFB_1_4_15_Port/3   11/30/11 7:48p scchang
 * SW7420-2158: DirectFB: Consider setting pthread mutex robust attribute
 *
 * DirectFB_1_4_15_Port/2   10/5/11 12:18p robertwm
 * SW7231-382: DirectFB: Support Creating DFB Surfaces for OpenGL
 *  rendering on the VC-4.
 *
 * DirectFB_1_4_14_Port/1   7/28/11 11:23a christ
 * SW7425-946: DirectFB: Create AppLibs DirectFB 1.4.14 release.
 *
 * DirectFB_1_4_5_Port/3   7/11/11 5:30p robertwm
 * SW7231-292: DirectFB-1.4.5: Ensure that outputs are not connected to
 *  the display twice.
 *
 * DirectFB_1_4_5_Port/2   7/7/11 6:45p tross
 * SWBLURAY-26478: Merge in Bluray libinit support.
 *
 * DirectFB_1_4_5_Port/1   6/14/11 5:05p christ
 * SW7425-707: DirectFB: Create DirectFB-1.4.5 Phase 2.0 Release.
 *
 * DirectFB_1_4_Port/28   6/10/11 5:17p christ
 * SW7422-138: DirecftFB: Add support for stereoscopic 3D graphics
 *  display.
 *
 * DirectFB_1_4_Port/DirectFB_1_4_1_BD_Integration/3   4/7/11 8:18p tross
 * Merge from latest.
 *
 * DirectFB_1_4_Port/DirectFB_1_4_1_BD_Integration/2   3/28/11 11:03a tross
 * Merge from DirectFB 1.4.1 Phase 3.01 label.
 *
 * DirectFB_1_4_Port/27   5/17/11 6:51p robertwm
 * SW7405-5326: DirectFB: Re-factor platform code for new API.
 *
 * DirectFB_1_4_Port/26   3/30/11 6:20p christ
 * SW7425-254: DirectFB: Improve performance by replacing linux-fusion
 *  calls with pthreads
 *
 * DirectFB_1_4_Port/25   3/17/11 12:42p robertwm
 * SW7422-378: DirectFB: Destroying a Palettised surface in a client
 *  application causes NEXUS assertion during free.
 * SW7422-383: DirectFB: Need to serialise access to SID to prevent
 *  assertions.
 *
 * DirectFB_1_4_Port/24   3/1/11 8:49p robertwm
 * SW7420-1559: DirectFB: NEXUS surface assertion in multi-app mode when
 *  closing down a DFB client.
 *
 * DirectFB_1_4_Port/DirectFB_1_4_1_BD_Integration/1   3/2/11 6:14p tross
 * Merge in Bluray support.
 *
 * DirectFB_1_4_Port/23   2/8/11 12:44p robertwm
 * SW7420-1467: DirectFB: Multiple apps should NOT share NEXUS Display
 *  handle.
 *
 * DirectFB_1_4_Port/22   2/3/11 6:04p christ
 * SW7420-1458: DirectFB: Rework internal handling of nexus heaps.
 *
 * DirectFB_1_4_Port/20   1/19/11 12:25p scchang
 * SW7420-1394: video layer support in DFB
 *
 * DirectFB_1_4_Port/19   12/3/10 8:39p raywu
 * SW7550-629: DirectFB: Integrated ZSP code to DFB packet buffer branch
 *
 * DirectFB_1_4_Port/18   12/1/10 3:47p christ
 * SW7420-1283: DirectFB: Add support for extra Nexus heap on dual MEMC
 *  parts
 *
 * DirectFB_1_4_Port/17   8/20/10 3:38p robertwm
 * SW7405-4791: DirectFB: DirectFB-1.4.1 Phase 2.5 fails to build with
 *  Nexus Phase 5.0 reference software.
 *
 * DirectFB_1_4_Port/16   8/11/10 2:01a robertwm
 * SW7405-4250: DirectFB: Screen destination rectangle and source
 *  rectangle not saved across contexts.
 * SW7405-4743: DirectFB: Multiple instances of df_andi.c do not get
 *  displayed.
 *
 * DirectFB_1_4_Port/15   7/22/10 4:17p robertwm
 * SW7405-4649: DirectFB: Optimise surface locking and wait idle syncing.
 *
 * DirectFB_1_4_Port/14   4/15/10 10:37p robertwm
 * SW7405-4214: DirectFB: Multi-Process DirectFB fails to run.
 *
 * DirectFB_1_4_Port/13   4/9/10 4:24p robertwm
 * SW7405-4180: DirectFB: Require independent setting of graphics
 *  resolution on multiple output paths.
 *
 * DirectFB_1_4_Port/12   3/2/10 10:41a robertwm
 * SW3556-1069: DirectFB: Run-time dynamic linking between gfxdriver and
 *  system drivers not working.
 *
 * DirectFB_1_4_Port/11   1/25/10 10:23a robertwm
 * SW7405-3827: DirectFB: system module not including
 *  bcmnexus_gfx_defines.h.
 *
 * DirectFB_1_4_Port/10   1/18/10 8:10p robertwm
 * SW7420-551: DirectFB: Add support for Screen "SetPowerMode()" API Call.
 *
 * DirectFB_1_4_Port/9   1/14/10 6:53p robertwm
 * SW3556-996: DirectFB: Implement ImageProvider for DirectFB-1.4.1 using
 *  SID.
 *
 * DirectFB_1_4_Port/8   1/4/10 5:59p robertwm
 * SW7420-523: DirectFB: Require 1080p resolution support.
 * SW7405-3701: DirectFB: Different video and graphics resolutions.
 *
 * DirectFB_1_4_Port/7   12/23/09 2:22p robertwm
 * SW7550-51: Support DirectFB on 7550.
 *
 * DirectFB_1_4_Port/SW7550-51/1   12/22/09 3:22p mphillip
 * SW7550-51: Add 7550 build support (on branch)
 *
 * DirectFB_1_4_Port/6   12/16/09 9:45p robertwm
 * SW7400-2643: DirectFB: Add support for setting/getting video modes.
 *
 * DirectFB_1_4_Port/5   11/16/09 1:27p robertwm
 * SW7405-3399: WaitForSync() function is unimplemented.
 * (Fixed graphics freeze issue).
 *
 * DirectFB_1_4_Port/4   11/13/09 7:10p robertwm
 * SW7405-3399: WaitForSync() function is unimplemented.
 *
 * DirectFB_1_4_Port/3   8/27/09 4:36p robertwm
 * SW3548-2427: DirectFB: Support 3549 panel types and fix compilation
 *  bugs.
 *
 * DirectFB_1_4_Port/2   8/21/09 11:12a robertwm
 * PR57917: DirectFB: testman and testrun don't run anymore in 1.4.1
 *  (SaWMan)
 *
 * DirectFB_1_4_Port/1   7/8/09 3:19p mphillip
 * PR56658: Add DirectFB 1.4.1 broadcom_files
 *
 * 9   11/3/08 1:57p gmohile
 * PR 45243 : Default all DTV to panel output
 *
 * 8   10/24/08 12:42p gmohile
 * PR 46490 : Add hw picture decode support
 *
 * 7   10/14/08 3:53p gmohile
 * PR 45243 : Fix nexus handles
 *
 * 6   9/15/08 11:49a gmohile
 * PR 46734 : Add dual output support
 *
 * 5   8/29/08 2:34p gmohile
 * PR 45243 : Do nexus platform init and display open outside directfb
 *
 * 4   8/27/08 4:26p gmohile
 * PR 45561 : Remove surface LUT
 *
 * 3   8/21/08 1:49p gmohile
 * PR 25109 : Fix compile error
 *
 * 2   7/31/08 3:30p ttrammel
 * PR44951: Update DirectFB for 2.6.18-5.0 and 93549
 *
 * $bcmnexus_Workfile: bcmnexus.h $
 * $bcmnexus_Revision: $
 * $bcmnexus_Date: $
 *
 * [File Description:]
 *
 * Revision History:
 *
 ***************************************************************************/

#ifndef __CORE__BCMNEXUS_H__
#define __CORE__BCMNEXUS_H__

#include <dfb_types.h>
#include "platform_init.h"
#include "bcmnexus_gfx_defines.h"
#include "bcmnexus_sys_defines.h"
#include "nexus_surface.h"
#ifdef NEXUS_HAS_PICTURE_DECODER
#include "nexus_picture_decoder.h"
#endif
#include <core/surface_pool.h>
#include <gfx/convert.h>

/* If updating any of the APIs or making any radical changes please rev the major version number.
   Any functional changes or bug fixes please upadte the minor version number */

#define BCMNEXUS_SYS_MAJOR_VERSION 5
#define BCMNEXUS_SYS_MINOR_VERSION 1

/* Macros to get/set fields in the shared DFBBCMNEXUS structure */
#define SET_DISPLAY_NUM_GFX_LAYERS( BCMNEXUS_PTR, val, id)      (BCMNEXUS_PTR->displays[id].numGfxLayers = val)
#define SET_DISPLAY_GFX_LAYER_ID(   BCMNEXUS_PTR, val, idx, id) (BCMNEXUS_PTR->displays[id].gfxLayerIds[idx] = val)
#define SET_DISPLAY_VID_LAYER_ID(   BCMNEXUS_PTR, val, idx, id) (BCMNEXUS_PTR->displays[id].vidLayerIds[idx] = val)
#define SET_DISPLAY_HND(            BCMNEXUS_PTR, val, id)      (BCMNEXUS_PTR->displays[id].hDisplay = val)
#define SET_DISPLAY_VSYNC_CB(       BCMNEXUS_PTR, val, id)      (BCMNEXUS_PTR->displays[id].vsyncCallback = val)

#define GET_DISPLAY_NUM_GFX_LAYERS( BCMNEXUS_PTR, id)           (BCMNEXUS_PTR->displays[id].numGfxLayers)
#define GET_DISPLAY_GFX_LAYER_ID(   BCMNEXUS_PTR, idx, id)      (BCMNEXUS_PTR->displays[id].gfxLayerIds[idx])
#define GET_DISPLAY_VID_LAYER_ID(   BCMNEXUS_PTR, idx, id)      (BCMNEXUS_PTR->displays[id].vidLayerIds[idx])
#define GET_DISPLAY_HND(            BCMNEXUS_PTR, id)           (BCMNEXUS_PTR->displays[id].hDisplay)
#define GET_DISPLAY_VSYNC_MUTEX(    BCMNEXUS_PTR, id)           (BCMNEXUS_PTR->displays[id].vsyncMutex)
#define GET_DISPLAY_VSYNC_SEM(      BCMNEXUS_PTR, id)           (BCMNEXUS_PTR->displays[id].vsyncSem)
#define GET_DISPLAY_VSYNC_CB(       BCMNEXUS_PTR, id)           (BCMNEXUS_PTR->displays[id].vsyncCallback)

#if NEXUS_HAS_PICTURE_DECODER
#define SET_PICTURE_DECODER_HND(    BCMNEXUS_PTR, val)          (BCMNEXUS_PTR->pictureDecoder = val)
#define GET_PICTURE_DECODER_HND(    BCMNEXUS_PTR)               (BCMNEXUS_PTR->pictureDecoder)
#define SET_PICTURE_DECODER_LOCK(   BCMNEXUS_PTR, val)          (BCMNEXUS_PTR->pictureDecoderLock = val)
#define GET_PICTURE_DECODER_LOCK(   BCMNEXUS_PTR)               (BCMNEXUS_PTR->pictureDecoderLock)
#endif

#if NEXUS_HAS_GRAPHICS2D
#define SET_GRAPHICS2D_HND(         BCMNEXUS_PTR, val)          (BCMNEXUS_PTR->hGfx2D = val)
#define SET_GRAPHICS2D_SYNC(        BCMNEXUS_PTR, val)          (BCMNEXUS_PTR->gfx2DSync = val)
#define GET_GRAPHICS2D_HND(         BCMNEXUS_PTR)               (BCMNEXUS_PTR->hGfx2D)
#define GET_GRAPHICS2D_SYNC(        BCMNEXUS_PTR)               (BCMNEXUS_PTR->gfx2DSync)
#endif

#define SET_POOL_HND(               BCMNEXUS_PTR, val)          (BCMNEXUS_PTR->pool = val)
#define GET_POOL_HND(               BCMNEXUS_PTR)               (BCMNEXUS_PTR->pool)

#define SET_GFX3D_POOL_HND(         BCMNEXUS_PTR, val)          (BCMNEXUS_PTR->gfx3dPool = val)
#define GET_GFX3D_POOL_HND(         BCMNEXUS_PTR)               (BCMNEXUS_PTR->gfx3dPool)

#define SET_DISPLAY_HEAP_HND(       BCMNEXUS_PTR, val)          (BCMNEXUS_PTR->displayHeap = val)
#define GET_DISPLAY_HEAP_HND(       BCMNEXUS_PTR)               (BCMNEXUS_PTR->displayHeap)

#define SET_2ND_DISPLAY_HEAP_HND(   BCMNEXUS_PTR, val)          (BCMNEXUS_PTR->secondaryDisplayHeap = val)
#define GET_2ND_DISPLAY_HEAP_HND(   BCMNEXUS_PTR)               (BCMNEXUS_PTR->secondaryDisplayHeap)


#define SET_OFFSCREEN_HEAP_HND(     BCMNEXUS_PTR, val)          (BCMNEXUS_PTR->offscreenHeap = val)
#define GET_OFFSCREEN_HEAP_HND(     BCMNEXUS_PTR)               (BCMNEXUS_PTR->offscreenHeap)

#define SET_PALETTE_HEAP_HND(       BCMNEXUS_PTR, val)          (BCMNEXUS_PTR->paletteHeap = val)
#define GET_PALETTE_HEAP_HND(       BCMNEXUS_PTR)               (BCMNEXUS_PTR->paletteHeap)

#define SET_CALL_HANDLER(           BCMNEXUS_PTR, val)          (BCMNEXUS_PTR->call = val)
#define GET_CALL_HANDLER(           BCMNEXUS_PTR)               (BCMNEXUS_PTR->call)
#define SET_VIDEO_HEAP_HND(         BCMNEXUS_PTR, val)          (BCMNEXUS_PTR->videoHeap = val)
#define GET_VIDEO_HEAP_HND(         BCMNEXUS_PTR)               (BCMNEXUS_PTR->videoHeap)

/*** Definitions ***/
#define PRIMARY_DISPLAY_ID          (DSCID_PRIMARY)
#define SECONDARY_DISPLAY_ID        (PRIMARY_DISPLAY_ID + 1)
#define TERTIARY_DISPLAY_ID         (SECONDARY_DISPLAY_ID + 1)

#define PRIMARY_GFX_LAYER_INDEX     (0)
#define SECONDARY_GFX_LAYER_INDEX   (PRIMARY_GFX_LAYER_INDEX + 1)

#define PRIMARY_VID_LAYER_INDEX     (0)
#define SECONDARY_VID_LAYER_INDEX   (PRIMARY_VID_LAYER_INDEX + 1)

#ifdef EOWNERDEAD
#define PTHREAD_ROBUST_MUTEX_LOCK(mutex) \
do { \
     int err; \
     err = pthread_mutex_lock(mutex); \
     if (err == EOWNERDEAD) \
     { \
          pthread_mutex_consistent(mutex); \
          D_ERROR("Thread(%p) pthread_mutex_consistent() completed!", (void *)pthread_self()); \
          pthread_mutex_unlock(mutex); \
          D_ERROR("Thread(%p) pthread_mutex_unlock() completed!", (void *)pthread_self()); \
          pthread_mutex_lock(mutex); \
          D_ERROR("Thread(%p) pthread_mutex_lock() completed!", (void *)pthread_self()); \
     } else if (err == ENOTRECOVERABLE) \
     { \
          D_ERROR("Thread(%p) mutex shall be in unusable state!", (void *)pthread_self()); \
     } \
} while (0)

#else
#define PTHREAD_ROBUST_MUTEX_LOCK(mutex) pthread_mutex_lock(mutex)

#endif /*EOWNERDEAD*/

typedef enum {
     BCM_POOL_MEMORY_ALLOCATE,
     BCM_POOL_MEMORY_FREE,
     BCM_POOL_SURFACE_CREATE,
     BCM_POOL_SURFACE_DESTROY,
     BCM_LAYER_SET_GFX_FRAMEBUFFER,
     BCM_LAYER_SET_GFX_SETTINGS,
     BCM_SCREEN_SET_DISPLAY_SETTINGS,
     BCM_SCREEN_ENABLE_DISPLAY_OUTPUT,
} eBcmCall;

/* These structures are shared amongst the different processes that interface with DFB */
typedef struct {
    int                        numGfxLayers;
    DFBDisplayLayerID          gfxLayerIds[NUM_GFX_LAYERS_PER_DISPLAY];
    DFBDisplayLayerID          vidLayerIds[NUM_VID_LAYERS_PER_DISPLAY];
    DFB_PlatformNexusHandle    hDisplay;
    DFBDimension               size;
    pthread_mutex_t            vsyncMutex;
    pthread_cond_t             vsyncSem;
    void                     (*vsyncCallback) (void *context, int param);
} BCMNEXUS_Display;

typedef struct  {

        /* Setting the IR or keypad timeout to 0 will result in the driver generating both a KEYPRESS and KEYRELEASED
           event each time a button/key is pressed.  This is no longer the default bahviour.  Instead, when a key is
           pressed only a KEYPRESS event will be generated.  If the key is released, then a KEYRELEASED event will
           be generated after the timeout specified below. This is the default behaviour and mimicks the way
           events are generated for HID devices such as a keyboard or mouse. */
    unsigned int  bcmnexus_ir_timeout;           /* IR KEYRELEASED event generation timeout in ms */
    unsigned int  bcmnexus_key_timeout;          /* KEYPAD KEYRELEASED event generation timeout in ms */

        /* Setup the IR and Keypad repeat filter time (in ms).  Key presses received less than this value will
           be treated as repeats. */
    unsigned int  bcmnexus_ir_repeat_time;       /* IR repeat time in ms */
    unsigned int  bcmnexus_key_repeat_time;      /* KEYPAD repeat time in ms */

    unsigned int  bcmnexus_key_debounce;         /* Specify the KEYPAD debounce count */
    unsigned int  bcmnexus_ir_repeat_skip;       /* Specify the number of initial IR repeats to skip/dismiss */
    unsigned int  bcmnexus_key_repeat_skip;      /* Specify the number of initial KEYPAD repeats to skip/dismiss */
    char         *bcmnexus_ir_protocol;          /* Specify IR protocol */
    char         *bcmnexus_ir_keycodes;          /* Specify IR keycodes module*/

    bool          sw_picture_decode;

    char         *res;                           /* alternative representation
                                                    of screen size */

    /*platforms with no vertical GFD scalers and memory BW limitations, to get 1080p output, FB needs to be customized using ibuffer config option */
    struct {
        int                   width;            /* Intermediate buffer width*/
        int                   height;           /* Intermediate buffer height*/
    } ibuffer;

} BCMNEXUS_Options;

typedef struct  {
    BCMNEXUS_Display           displays[NUM_DISPLAYS+1];/* Handles to primary, secondary displays and video layer display */
    BCMNEXUS_Options           options;                 /* Command line options specific to Broadcom driver */
#if NEXUS_HAS_GRAPHICS2D
    NEXUS_Graphics2DHandle     hGfx2D;                  /* Graphics 2D/blitter handle */
    bool                       gfx2DSync;               /* Flag to indicate whether we need to issue checkpoint/sync */
#endif
#ifdef NEXUS_HAS_PICTURE_DECODER
    NEXUS_PictureDecoderHandle pictureDecoder;          /* Picture decoder handle*/
    pthread_mutex_t            pictureDecoderLock;      /* Picture decoder global access lock */
#endif
    FusionCall                 call;
    CoreSurfacePool           *pool;                    /* 'virtual' pool, to link NEXUS <> DFB surfaces */
    CoreSurfacePool           *gfx3dPool;               /* 'virtual' pool, to link NEXUS <> DFB surfaces */
    NEXUS_HeapHandle           displayHeap;             /* Index of which Nexus Heap to use for Graphics Surfaces */
    NEXUS_HeapHandle           secondaryDisplayHeap;    /* Index of which Nexus Heap to use for Graphics Surfaces on secondary display*/
    NEXUS_HeapHandle           offscreenHeap;           /* Index of Nexus Heap to use for offscreen Graphics Surfaces */
    NEXUS_HeapHandle           paletteHeap;             /* Index of  Nexus Heap to use for pallettes*/
    NEXUS_HeapHandle           videoHeap;               /* Index of  Nexus Heap to use for video based graphics surfaces*/
    int                        heapSize;                /* Total useable size of the display heap */
} DFBBCMNEXUS;

extern CoreDFB *dfb_bcmnexus_core;                      /* Local for each process that joins DirectFB */

#endif
