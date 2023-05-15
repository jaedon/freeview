/******************************************************************************
 *    (c)2011-2012 Broadcom Corporation
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
 * $brcm_Workfile: platform_nexus_init.c $
 * $brcm_Revision: DirectFB_1_4_15_Port/22 $
 * $brcm_Date: 3/7/12 5:26p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /AppLibs/opensource/directfb/src/broadcom_files/private/DirectFB/1.4.15/platform/platform_nexus_init.c $
 *
 * DirectFB_1_4_15_Port/22   3/7/12 5:26p christ
 * SW7231-699: DirectFB-1.4.15: Clean up Broadcom code tree
 *
 * DirectFB_1_4_15_Port/21   3/1/12 7:53p christ
 * SW7420-2273: DirectFB-1.4.15: Use correct Nexus heap for secondary
 *  display frame buffers
 *
 * DirectFB_1_4_15_Port/20   2/29/12 7:07p christ
 * SW7420-2270: DirectFB-1.4.15: Cleanup code prior to release
 *
 * DirectFB_1_4_15_Port/19   2/20/12 6:08p christ
 * SW7420-2255: DirectFB-1.4.15: Graphics 2D open fails on older refsw.
 *
 * DirectFB_1_4_15_Port/18   2/9/12 2:33p christ
 * SW7425-1550: DirectFB: Require integration of Nexus Surface Compositor
 *  into DFB
 *
 * DirectFB_1_4_15_Port/17   1/16/12 1:06p robertwm
 * SW7425-2185: DirectFB: Cannot run unprotected client applications that
 *  access video memory.
 *
 * DirectFB_1_4_15_Port/16   12/15/11 12:38p apandav
 * SW7468-398: DirectFB: Could not build DirectFB 1.4.15 on BCM97468
 *  platform
 *
 * DirectFB_1_4_15_Port/DirectFB-XS_1_4_15_Port/2   1/27/12 7:37p christ
 * SW7231-625: DirectFB-XS: Allow Nexus client only builds.
 *
 * DirectFB_1_4_15_Port/DirectFB-XS_1_4_15_Port/1   12/8/11 3:03p robertwm
 * SW7425-1550: DirectFB: Require integration of Nexus Surface Compositor
 *  into DFB.
 *
 * DirectFB_1_4_15_Port/15   12/7/11 12:04p robertwm
 * SW7425-1911: DirectFB-1.4.15: Remove BCMNEXUS_UNTRUSTED_CLIENT_SUPPORT
 *  from platform code as it is redundant.
 *
 * DirectFB_1_4_15_Port/14   11/25/11 2:53p robertwm
 * SW7125-1167: DirectFB-1.4.15: Cannot compile platform code if 3DTV
 *  support is not available.
 *
 * DirectFB_1_4_15_Port/13   11/22/11 4:58p robertwm
 * SW7425-1550: DirectFB: Require integration of Nexus Surface Compositor
 *  into DFB.
 *
 * DirectFB_1_4_15_Port/12   11/22/11 2:47p christ
 * SW7231-481: DirectFB: Detect incorrectly configured Nexus server.
 *
 * DirectFB_1_4_15_Port/11   11/21/11 5:08p robertwm
 * SW7425-1815: DirectFB-1.4.15: Cannot run DFB in multi-app mode with
 *  Phase 3.5 reference software.
 *
 * DirectFB_1_4_15_Port/10   11/15/11 1:51a christ
 * SW7346-556: DirectFB: Display fails to open on 7346 B1 using Phase 4.5
 *  refsw
 *
 * DirectFB_1_4_15_Port/9   10/20/11 5:56p robertwm
 * SW7231-398: DirectFB-1.4.15: Locking a pre-allocated surface does not
 *  return same virtual address.
 *
 * DirectFB_1_4_15_Port/8   10/20/11 2:14p robertwm
 * SW7425-1156: DirectFB: Nexus Phase 3.5 uses older Nexus Server API,
 *  clients apps must be trusted to access memory..
 *
 * DirectFB_1_4_15_Port/7   10/20/11 12:43p christ
 * SW7425-1156: DirectFB: Nexus Phase 3.5 uses older Nexus Server API,
 *  clients apps must be trusted to access memory.
 *
 * DirectFB_1_4_15_Port/6   10/17/11 5:30p robertwm
 * SW7231-398: DirectFB-1.4.15: Locking a pre-allocated surface does not
 *  return same virtual address.
 *
 * DirectFB_1_4_15_Port/5   10/11/11 4:39p robertwm
 * SW7405-5507: DirectFB-1.4.15: Cannot compile DirectFB Platform code
 *  against Phase 8.0 refsw.
 *
 * DirectFB_1_4_15_Port/4   10/10/11 5:02p christ
 * SW7420-2089: DirectFB: df_andi3d fails to run on 7420
 *
 * DirectFB_1_4_15_Port/3   10/5/11 12:13p robertwm
 * SW7231-382: DirectFB: Support Creating DFB Surfaces for OpenGL
 *  rendering on the VC-4.
 * SW7420-2058: DirectFB: Change DFB Platform code to not rely on
 *  directfb.h header file.
 *
 * DirectFB_1_4_14_Port/2   9/22/11 4:55p christ
 * SW7425-1277: DirectFB: Support untrusted client applications in 1.4.14
 *
 * DirectFB_1_4_14_Port/1   9/22/11 4:43p christ
 * SW7425-1277: DirectFB: Support untrusted client applications in 1.4.14
 *
 *****************************************************************************/
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <dlfcn.h>

#include "platform_display.h"
#include "platform_nexus_init.h"
#include "platform_video_formats.h"
#include "lib/fusion/build.h"

#if NEXUS_HAS_GRAPHICS3D && defined(BCMNEXUS_GFX_HAS_P3D)
#include "nexus_graphics2d_impl.h"
#endif

BDBG_MODULE(nexus_common_init);

static NEXUS_PlatformSettings nexusPlatformSettings;

static bool dfb_p_isNexusPlatformSettingsAcquired = false;
static bool dfb_p_isPlatformInited = false;
#if BCMNEXUS_HAS_IPC
static bool dfb_p_isIPCServerOpened = false;
#endif

#if BCMNEXUS_AUTHENTICATION_KEY
unsigned char authenticationKey[] = {4,4,1,1,7,9,0,6,2,7,0,0}; // Don't tell Debbie !
#endif

#if NEXUS_HAS_GRAPHICS3D && defined(BCMNEXUS_GFX_HAS_V3D)
#include <EGL/egl.h>
static bool  dfb_p_isGraphics3DPoolInit = false;
static void *graphics3dDriverHandle = NULL;
static void (*DFB_Platform_P_BufferGetRequirements)( BEGL_PixmapInfo     *bufferRequirements,
                                                     BEGL_BufferSettings *bufferConstrainedRequirements );
#endif

extern DFB_P_DisplaySettings *dfb_p_displaysettings;

NEXUS_HeapHandle graphics2dHeap = NULL;
NEXUS_HeapHandle graphics2dBoundsHeap = NULL;

int
DFB_Platform_P_GetSharedMemSize( void )
{
    return sizeof(DFB_P_DisplaySettings);
}

bool
DFB_Platform_P_Display_GetDefaultInitSettings( unsigned                         displayIndex,
                                               DFB_PlatformDisplayInitSettings *pSettings )
{
    bool res = false;

    BDBG_MSG_TRACE((">>%s", __FUNCTION__));

    if (displayIndex < NUM_DISPLAYS)
    {
        memset(pSettings, 0, sizeof(*pSettings));
        pSettings->displayType = DFB_PlatformDisplayType_eAuto;
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_SDOUT_PAL)
        pSettings->format      = DFB_PlatformVideoFormat_ePalG;
#elif defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_SDOUT_NTSC_JAPAN)
        pSettings->format      = DFB_PlatformVideoFormat_eNtscJapan;
#else
        pSettings->format      = DFB_PlatformVideoFormat_eNtsc;
#endif
        pSettings->aspectRatio = DFB_PlatformDisplayAspectRatio_eAuto;
        pSettings->timebase    = DFB_PlatformTimebase_e0;
        pSettings->vecIndex    = -1;
        res = true;
    }

    BDBG_MSG_TRACE(("<<%s", __FUNCTION__));
    return res;
}

DFB_PlatformResult
DFB_Platform_P_GetDisplayOutput( DFB_PlatformNexusHandle       displayHandle,
                                 DFB_PlatformDisplayOutputType outputType,
                                 int                           outputIndex,
                                 DFB_PlatformNexusHandle      *pOutputHandle )
{
    BDBG_MSG_TRACE((">>%s", __FUNCTION__));
    *pOutputHandle = DFB_Platform_P_GetDisplayOutputHandle(displayHandle, outputType, outputIndex);
    BDBG_MSG_TRACE(("<<%s", __FUNCTION__));

    return DFB_PLATFORM_OK;
}

DFB_PlatformResult
DFB_Platform_P_SetDisplayOutput( DFB_PlatformNexusHandle       displayHandle,
                                 DFB_PlatformDisplayOutputType outputType,
                                 int                           outputIndex,
                                 DFB_PlatformNexusHandle       outputHandle )
{
    DFB_PlatformResult res;

    BDBG_MSG_TRACE((">>%s", __FUNCTION__));
    res = DFB_Platform_P_SetDisplayOutputHandle( displayHandle, outputType, outputIndex, outputHandle);
    BDBG_MSG_TRACE(("<<%s", __FUNCTION__));
    return res;
}

DFB_PlatformResult
DFB_Platform_P_AddDisplayOutput( DFB_PlatformNexusHandle       displayHandle,
                                 DFB_PlatformDisplayOutputType outputType,
                                 int                           outputIndex )
{
    DFB_PlatformResult rc = DFB_PLATFORM_UNSUPPORTED;

    BDBG_MSG_TRACE((">>%s", __FUNCTION__));

    /* Hdmi display add is implemented in di_init func */
#if defined(HUMAX_PLATFORM_BASE)
    if (outputType == DFB_PlatformDisplayOutputType_eComponent)
	    rc = DFB_Platform_P_AddDisplayOutput_Component(displayHandle, outputIndex);
#else
    if (outputType == DFB_PlatformDisplayOutputType_eHdmi)
	    rc = DFB_Platform_P_AddDisplayOutput_Hdmi(displayHandle, outputIndex);
    else if (outputType == DFB_PlatformDisplayOutputType_eComponent)
	    rc = DFB_Platform_P_AddDisplayOutput_Component(displayHandle, outputIndex);
#endif
    else if (outputType == DFB_PlatformDisplayOutputType_eComposite)
	    rc = DFB_Platform_P_AddDisplayOutput_Composite(displayHandle, outputIndex);
    else if (outputType == DFB_PlatformDisplayOutputType_eScart)
	    rc = DFB_Platform_P_AddDisplayOutput_Scart(displayHandle, outputIndex);
    else if (outputType == DFB_PlatformDisplayOutputType_eRfm)
	    rc = DFB_Platform_P_AddDisplayOutput_Rfm(displayHandle, outputIndex);
    else if (outputType == DFB_PlatformDisplayOutputType_eSvideo)
	    rc = DFB_Platform_P_AddDisplayOutput_Svideo(displayHandle, outputIndex);

    BDBG_MSG_TRACE(("<<%s", __FUNCTION__));
    return rc;
}

DFB_PlatformResult
DFB_Platform_P_RemoveDisplayOutput( DFB_PlatformNexusHandle       displayHandle,
                                    DFB_PlatformDisplayOutputType outputType,
                                    int                           outputIndex )
{
    DFB_PlatformResult rc = DFB_PLATFORM_UNSUPPORTED;

    BDBG_MSG_TRACE((">>%s", __FUNCTION__));

    if (outputType == DFB_PlatformDisplayOutputType_eHdmi)
        rc = DFB_Platform_P_RemoveDisplayOutput_Hdmi(displayHandle, outputIndex);
    else if (outputType == DFB_PlatformDisplayOutputType_eComponent)
        rc = DFB_Platform_P_RemoveDisplayOutput_Component(displayHandle, outputIndex);
    else if (outputType == DFB_PlatformDisplayOutputType_eComposite)
        rc = DFB_Platform_P_RemoveDisplayOutput_Composite(displayHandle, outputIndex);
    else if (outputType == DFB_PlatformDisplayOutputType_eScart)
        rc = DFB_Platform_P_RemoveDisplayOutput_Scart(displayHandle, outputIndex);
    else if (outputType == DFB_PlatformDisplayOutputType_eRfm)
        rc = DFB_Platform_P_RemoveDisplayOutput_Rfm(displayHandle, outputIndex);
    else if (outputType == DFB_PlatformDisplayOutputType_eSvideo)
        rc = DFB_Platform_P_RemoveDisplayOutput_Svideo(displayHandle, outputIndex);

    BDBG_MSG_TRACE(("<<%s", __FUNCTION__));
    return rc;
}

#define abs(X) ((X)>0?(X):-(X))

#if defined(HUMAX_PLATFORM_BASE)	/* to support 3D surface */
static NEXUS_SurfaceHandle localSurface[2] = {NULL,NULL};
static uint32_t uLocalSurfaceIndex = 0;
#else
static NEXUS_SurfaceHandle localSurface = NULL;
#endif

void
DFB_Platform_P_FreeLocalSurface(void)
{
#if defined(HUMAX_PLATFORM_BASE)	/* to support 3D surface */
	uint32_t i = 0;

	for(i=0; i<2; i++)
	{
		if (localSurface[i] != NULL)
		{
			NEXUS_Surface_Destroy(localSurface[i]);
			localSurface[i] = NULL;
		}
	}
#else
	if (localSurface != NULL)
	{
		NEXUS_Surface_Destroy(localSurface);
		localSurface = NULL;
	}
#endif
}

#if (!BCMNEXUS_VDC_SUPPORT_3D_VIDEO || !BCMNEXUS_DISPLAY_HAS_3DTV_SUPPORT) || (defined(HUMAX_PLATFORM_BASE))	/* to support 3D surface */
#if defined(HUMAX_PLATFORM_BASE)	/* to support 3D surface */
DFB_PlatformResult DFB_Platform_P_BlitComposite3DFramebuffer(
    DFB_PlatformNexusHandle graphics2dHandle,
    DFB_PlatformNexusHandle display,
    DFB_Platform_P_DisplayGraphicsFramebuffer3D *frameBuffer3D,
    bool stereoLayer,
    int z)
{
    DFB_PlatformResult           res;
    NEXUS_SurfaceCreateSettings  createSurfaceSettings;
    NEXUS_SurfaceCreateSettings  localSurfaceSettings;
    NEXUS_Rect                   inputRect;
    NEXUS_Rect                   outputRect;
    NEXUS_Error                  rc      = NEXUS_SUCCESS;
    int                          scaledZ = z;

    outputRect.width = 0;
    outputRect.height = 0;

    NEXUS_Surface_GetCreateSettings(frameBuffer3D->main,&createSurfaceSettings);

    if (frameBuffer3D->orientation == DFB_Platform_P_VideoOrientation_e3D_OverUnder)
    {
        createSurfaceSettings.width = createSurfaceSettings.width;
        createSurfaceSettings.height = createSurfaceSettings.height/2;
    }
    else if (frameBuffer3D->orientation == DFB_Platform_P_VideoOrientation_e3D_LeftRight)
    {
        createSurfaceSettings.width = createSurfaceSettings.width/2;
        createSurfaceSettings.height = createSurfaceSettings.height;
    }

	if(uLocalSurfaceIndex>1) uLocalSurfaceIndex = 0;

    if (localSurface[uLocalSurfaceIndex] != NULL)
    {
        NEXUS_Surface_GetCreateSettings(localSurface[uLocalSurfaceIndex],&localSurfaceSettings);

        if(memcmp( &createSurfaceSettings, &localSurfaceSettings, sizeof(NEXUS_SurfaceCreateSettings)))
        {
             NEXUS_Surface_Destroy(localSurface[uLocalSurfaceIndex]);
             localSurface[uLocalSurfaceIndex] = NULL;
        }
    }

    if (localSurface[uLocalSurfaceIndex] == NULL)
        localSurface[uLocalSurfaceIndex] = NEXUS_Surface_Create(&createSurfaceSettings);

    if (frameBuffer3D->orientation == DFB_Platform_P_VideoOrientation_e3D_OverUnder)
    {
        scaledZ = z;

        outputRect.width = createSurfaceSettings.width;
        outputRect.height = createSurfaceSettings.height;
    }
    else if (frameBuffer3D->orientation == DFB_Platform_P_VideoOrientation_e3D_LeftRight)
    {
        scaledZ = z * createSurfaceSettings.width / (int)(createSurfaceSettings.width/2);

        outputRect.width = createSurfaceSettings.width;
        outputRect.height = createSurfaceSettings.height;
    }

    inputRect.x = 0;
    inputRect.y = 0;
    if (frameBuffer3D->orientation == DFB_Platform_P_VideoOrientation_e3D_OverUnder){
        inputRect.width = createSurfaceSettings.width;
        inputRect.height = createSurfaceSettings.height*2;
    }
    else if (frameBuffer3D->orientation == DFB_Platform_P_VideoOrientation_e3D_LeftRight){
        inputRect.width = createSurfaceSettings.width*2;
        inputRect.height = createSurfaceSettings.height;
    }
    else{
        inputRect.width = createSurfaceSettings.width;
        inputRect.height = createSurfaceSettings.height;
    }

    outputRect.x = 0;
    outputRect.y = 0;

    if (!stereoLayer) // we only have one source image in this mode
        frameBuffer3D->right = frameBuffer3D->main;

    rc = NEXUS_Graphics2D_FastBlit( graphics2dHandle,
                                    localSurface[uLocalSurfaceIndex], &outputRect,
                                    frameBuffer3D->main, &inputRect,
                                    NEXUS_BlitColorOp_eCopySource,
                                    NEXUS_BlitAlphaOp_eCopySource,
                                    0);
    if (rc != NEXUS_SUCCESS)
        BDBG_ERR(("%s Failed to blit data (top/left)\n",__FUNCTION__));

    frameBuffer3D->main = localSurface[uLocalSurfaceIndex];

    frameBuffer3D->orientation = DFB_Platform_P_VideoOrientation_e2D;
    frameBuffer3D->right = NULL;

    res = DFB_Platform_P_SetDisplayGraphicsFramebuffer3D(display,frameBuffer3D, NULL, NULL, false);
    uLocalSurfaceIndex++;

    return res;
}
#else
DFB_PlatformResult DFB_Platform_P_BlitComposite3DFramebuffer(
    DFB_PlatformNexusHandle graphics2dHandle,
    DFB_PlatformNexusHandle display,
    DFB_Platform_P_DisplayGraphicsFramebuffer3D *frameBuffer3D,
    bool stereoLayer,
    int z)
{
    DFB_PlatformResult           res;
    NEXUS_SurfaceCreateSettings  createSurfaceSettings;
    NEXUS_SurfaceCreateSettings  localSurfaceSettings;
    NEXUS_Rect                   inputRect;
    NEXUS_Rect                   outputRect;
    NEXUS_Error                  rc      = NEXUS_SUCCESS;
    int                          scaledZ = z;

    outputRect.width = 0;
    outputRect.height = 0;

    NEXUS_Surface_GetCreateSettings(frameBuffer3D->main,&createSurfaceSettings);

    if (localSurface != NULL)
    {
        NEXUS_Surface_GetCreateSettings(localSurface,&localSurfaceSettings);

        if(memcmp( &createSurfaceSettings, &localSurfaceSettings, sizeof(NEXUS_SurfaceCreateSettings)))
        {
             NEXUS_Surface_Destroy(localSurface);
             localSurface = NULL;
        }
    }

    if (localSurface == NULL)
        localSurface = NEXUS_Surface_Create(&createSurfaceSettings);

    if (frameBuffer3D->orientation == DFB_Platform_P_VideoOrientation_e3D_OverUnder)
    {
        scaledZ = z;

        outputRect.width = createSurfaceSettings.width - abs(scaledZ);
        outputRect.height = createSurfaceSettings.height/2;
    }
    else if (frameBuffer3D->orientation == DFB_Platform_P_VideoOrientation_e3D_LeftRight)
    {
        scaledZ = z * createSurfaceSettings.width / (int)(createSurfaceSettings.width/2);

        outputRect.width = (int)(createSurfaceSettings.width/2) - abs(z);
        outputRect.height = createSurfaceSettings.height;
    }

    inputRect.x = (scaledZ < 0) ? -scaledZ : 0;
    inputRect.y = 0;
    inputRect.width = createSurfaceSettings.width - abs(scaledZ);
    inputRect.height = createSurfaceSettings.height;

    outputRect.x = (scaledZ > 0) ? scaledZ : 0;
    outputRect.y = 0;

    BDBG_MSG_TRACE(("(T/L) Input x:%d y:%d - width:%d height:%d",
                    inputRect.x,
                    inputRect.y,
                    inputRect.width,
                    inputRect.height));

    BDBG_MSG_TRACE(("(T/L) Output x:%d y:%d - width:%d height:%d",
                    outputRect.x,
                    outputRect.y,
                    outputRect.width,
                    outputRect.height));


    if (!stereoLayer) // we only have one source image in this mode
        frameBuffer3D->right = frameBuffer3D->main;

    rc = NEXUS_Graphics2D_FastBlit( graphics2dHandle,
                                    localSurface, &outputRect,
                                    frameBuffer3D->main, &inputRect,
                                    NEXUS_BlitColorOp_eCopySource,
                                    NEXUS_BlitAlphaOp_eCopySource,
                                    0);
    if (rc != NEXUS_SUCCESS)
        BDBG_ERR(("%s Failed to blit data (top/left)\n",__FUNCTION__));

    inputRect.x = (scaledZ > 0) ? scaledZ : 0;

    if (frameBuffer3D->orientation == DFB_Platform_P_VideoOrientation_e3D_OverUnder)
    {
        outputRect.x = (z < 0) ? -z : 0;
        outputRect.y = createSurfaceSettings.height/2;
    }

    else if (frameBuffer3D->orientation == DFB_Platform_P_VideoOrientation_e3D_LeftRight)
        outputRect.x = (int)(createSurfaceSettings.width/2) - abs(z);

    BDBG_MSG_TRACE(("(B/R) Input x:%d y:%d - width:%d height:%d",
                    inputRect.x, inputRect.y,
                    inputRect.width, inputRect.height));

    BDBG_MSG_TRACE(("(B/R) Output x:%d y:%d - width:%d height:%d",
                    outputRect.x, outputRect.y,
                    outputRect.width, outputRect.height));

    rc = NEXUS_Graphics2D_FastBlit( graphics2dHandle,
                                    localSurface, &outputRect,
                                    frameBuffer3D->right, &inputRect,
                                    NEXUS_BlitColorOp_eCopySource,
                                    NEXUS_BlitAlphaOp_eCopySource,
                                    0);
    if (rc != NEXUS_SUCCESS)
        BDBG_ERR(("%s: Failed to blit data (bottom/right)\n",__FUNCTION__));

    frameBuffer3D->main = localSurface;

    frameBuffer3D->orientation = DFB_Platform_P_VideoOrientation_e2D;
    frameBuffer3D->right = NULL;

    res = DFB_Platform_P_SetDisplayGraphicsFramebuffer3D(display,frameBuffer3D, NULL, NULL, false);

    return res;
}
#endif // #if defined(HUMAX_PLATFORM_BASE)	/* to support 3D surface */
#endif

DFB_PlatformNexusHandle
DFB_Platform_P_GetHeap( void *pMemory,
                        bool  isMaster )
{
    NEXUS_MemoryStatus      status;
    DFB_PlatformNexusHandle handle = NULL;

    if ( isMaster )
    {
        NEXUS_PlatformConfiguration platformConfig;

        NEXUS_Platform_GetConfiguration(&platformConfig);

        for (int i=0;i<NEXUS_MAX_HEAPS;i++) {
            NEXUS_HeapHandle heap = platformConfig.heap[i];

            if (!heap)
                continue;

            if (NEXUS_Heap_GetStatus(heap, &status) == NEXUS_SUCCESS)
            {
                BDBG_MSG(("%s: Heap[%d]: handle=%p, memcIndex=%d, range=<%p-%p>, size=%d, offset=0x%08x, type=%d (pMem=%p)", __FUNCTION__,
                          i, (void *)heap, status.memcIndex, status.addr, (void *)((uint32_t)status.addr+status.size-1),
                          status.size, status.offset, status.memoryType, pMemory));

                if (pMemory >= status.addr && pMemory < (void *)((uint32_t)status.addr + status.size))
                {
                    handle = heap;
                    break;
                }
            }
        }
    }
    else
    {
#if BCMNEXUS_HAS_IPC
        NEXUS_ClientConfiguration clientConfig;

        NEXUS_Platform_GetClientConfiguration(&clientConfig);

        for (int i=0;i<NEXUS_MAX_HEAPS;i++) {
            NEXUS_HeapHandle heap = clientConfig.heap[i];

            if (!heap)
                continue;

            if (NEXUS_Heap_GetStatus(heap, &status) == NEXUS_SUCCESS)
            {
                BDBG_MSG(("%s: Heap[%d]: handle=%p, memcIndex=%d, range=<%p-%p>, size=%d, offset=0x%08x, type=%d (pMem=%p)", __FUNCTION__,
                          i, (void *)heap, status.memcIndex, status.addr, (void *)((uint32_t)status.addr+status.size-1),
                          status.size, status.offset, status.memoryType, pMemory));

                if (pMemory >= status.addr && pMemory < (void *)((uint32_t)status.addr + status.size))
                {
                    handle = heap;
                    break;
                }
            }
        }
#endif /*BCMNEXUS_HAS_IPC*/
    }
    return handle;
}

void
DFB_Platform_P_DisplayHeaps( DFB_PlatformSettings *pSettings )
{
    bool isMaster;

    BDBG_MSG(("%s() We are %s app (%strusted slave mode)", __FUNCTION__,
              pSettings->slave? "Slave" : "Master" ,
              pSettings->trustUnauthenticatedClients ? "" : "un"));

    isMaster = !pSettings->slave;
#if !BCMNEXUS_NSC_SUPPORT
    isMaster = isMaster || pSettings->trustUnauthenticatedClients;
#endif
    DFB_Platform_P_GetHeap(NULL, isMaster);
}

bool
DFB_Platform_P_Graphics2D_GetDefaultInitSettings( DFB_PlatformGraphics2DInitSettings *pSettings )
{
    bool res;

    BDBG_MSG_TRACE((">>%s", __FUNCTION__));

#if NEXUS_HAS_GRAPHICS2D || NEXUS_HAS_ZSP_GRAPHICS
    memset(pSettings, 0, sizeof(DFB_PlatformGraphics2DInitSettings));

#ifdef NEXUS_GRAPHICS2D_QUEUED
    pSettings->maxOperations = 2048;
#endif
    res = true;
#else
    BSTD_UNUSED(pSettings);
    res = false;
#endif
    BDBG_MSG_TRACE(("<<%s", __FUNCTION__));
    return res;

}

DFB_PlatformResult
DFB_Platform_P_Graphics2D_Init( DFB_PlatformGraphics2DInitSettings *pSettings,
                                DFB_PlatformNexusHandle            *pHandle )
{
    DFB_PlatformResult res = DFB_PLATFORM_UNSUPPORTED;

    BDBG_MSG_TRACE((">>%s", __FUNCTION__));

    BDBG_ASSERT(pHandle);

#if NEXUS_HAS_GRAPHICS2D || NEXUS_HAS_ZSP_GRAPHICS
    unsigned int index = 0;
    NEXUS_Graphics2DOpenSettings graphicsSettings;

    BDBG_ASSERT(pSettings);

    NEXUS_Graphics2D_GetDefaultOpenSettings(&graphicsSettings);

#ifdef NEXUS_GRAPHICS2D_QUEUED
    graphicsSettings.maxOperations = pSettings->maxOperations;
#else
    graphicsSettings.preAllocPacketMemory = false;
    graphicsSettings.maxOperations = 0;
#endif

#ifdef BCMNEXUS_GFX_PACKET_FIFO_SUPPORTED
    graphicsSettings.packetFifoThreshold = 0;
#endif

#if BCMNEXUS_NSC_SUPPORT
    index = NEXUS_ANY_ID;
#endif

    if (graphics2dBoundsHeap)
        graphicsSettings.boundsHeap = graphics2dBoundsHeap;

    if (graphics2dHeap)
        graphicsSettings.heap = graphics2dHeap;

    *pHandle = NEXUS_Graphics2D_Open(index, &graphicsSettings );
    if (*pHandle)
    {
        BDBG_MSG(("%s: Graphics2D Handle=%p\n", __FUNCTION__, *pHandle));
        res = DFB_PLATFORM_OK;
    }
    else
    {
        BDBG_ERR(("%s: Cannot open graphics2d!\n", __FUNCTION__));
        res = DFB_PLATFORM_FAILURE;
    }
#else
    BSTD_UNUSED(pSettings);
    *pHandle = NULL;
#endif
    BDBG_MSG_TRACE(("<<%s", __FUNCTION__));
    return res;
}

void
DFB_Platform_P_Graphics2D_Uninit( DFB_PlatformNexusHandle handle )
{
    BDBG_MSG_TRACE((">>%s", __FUNCTION__));

    BDBG_ASSERT(handle);

#if NEXUS_HAS_GRAPHICS2D || NEXUS_HAS_ZSP_GRAPHICS
    NEXUS_Graphics2D_Close(handle);
#endif

    BDBG_MSG_TRACE(("<<%s", __FUNCTION__));
}

bool
DFB_Platform_P_Graphics3D_GetDefaultInitSettings( DFB_PlatformGraphics3DInitSettings *pSettings )
{
    bool res;

    BDBG_MSG_TRACE((">>%s", __FUNCTION__));

#if NEXUS_HAS_GRAPHICS3D && defined(BCMNEXUS_GFX_HAS_P3D)
    memset(pSettings, 0, sizeof(DFB_PlatformGraphics3DInitSettings));
    res = true;
#else
    BSTD_UNUSED(pSettings);
    res = false;
#endif
    BDBG_MSG_TRACE(("<<%s", __FUNCTION__));
    return res;
}

DFB_PlatformResult
DFB_Platform_P_Graphics3D_Init( DFB_PlatformGraphics3DInitSettings *pSettings,
                                DFB_PlatformNexusHandle            *pHandle )
{
    DFB_PlatformResult res = DFB_PLATFORM_UNSUPPORTED;

    BDBG_MSG_TRACE((">>%s", __FUNCTION__));

    BDBG_ASSERT(pHandle);

#if NEXUS_HAS_GRAPHICS3D && defined(BCMNEXUS_GFX_HAS_P3D)
    BDBG_ASSERT(pSettings);

    /* NEXUS_Graphics3D_Open doesn't return back a valid pointer, so don't set the handle
       to it! */
    *pHandle = (DFB_PlatformNexusHandle)0xDEADBEEF;
#if !NEXUS_GRAPHICS3D_MODE_PROXY
    NEXUS_Graphics3D_Open(0, NULL);
#endif
    BDBG_MSG(("%s: Graphics3D Handle=0x%08x\n", __FUNCTION__, *pHandle));
    res = DFB_PLATFORM_OK;
#else
    BSTD_UNUSED(pSettings);
    *pHandle = NULL;
#endif
    BDBG_MSG_TRACE(("<<%s", __FUNCTION__));
    return res;
}

void
DFB_Platform_P_Graphics3D_Uninit( DFB_PlatformNexusHandle handle )
{
    BDBG_MSG_TRACE((">>%s", __FUNCTION__));

    BDBG_ASSERT(handle);

#if NEXUS_HAS_GRAPHICS3D && defined(BCMNEXUS_GFX_HAS_P3D) && !NEXUS_GRAPHICS3D_MODE_PROXY
    NEXUS_Graphics3D_Close(handle);
#endif

    BDBG_MSG_TRACE(("<<%s", __FUNCTION__));
}

#if NEXUS_HAS_GRAPHICS3D && defined(BCMNEXUS_GFX_HAS_P3D)
BP3D_Handle DFB_Platform_P_Graphics3D_GetP3D(void)
{
    BDBG_MSG_TRACE((">>%s", __FUNCTION__));
#if NEXUS_HAS_GRAPHICS3D && defined(BCMNEXUS_GFX_HAS_P3D)
#if NEXUS_GRAPHICS3D_MODE_PROXY
     return Nexus_Proxy_P3D_GetP3d();
#else
     return NEXUS_Graphics3d_GetP3d_priv();
#endif
#endif
    BDBG_MSG_TRACE(("<<%s", __FUNCTION__));
}

BMEM_Handle DFB_Platform_P_Graphics3D_GetBMEM(void)
{
    BDBG_MSG_TRACE((">>%s", __FUNCTION__));
#if NEXUS_HAS_GRAPHICS3D && defined(BCMNEXUS_GFX_HAS_P3D)
#if NEXUS_GRAPHICS3D_MODE_PROXY
     return Nexus_Proxy_P3D_GetMEM();
#else
     return g_pCoreHandles->heap[0];
#endif
#endif
    BDBG_MSG_TRACE(("<<%s", __FUNCTION__));
}

void DFB_Platform_P_Graphics3D_GetBSUR( DFB_PlatformNexusHandle hSurface,
                                        BSUR_Surface_Handle    *phSur )
{
    BDBG_MSG_TRACE((">>%s", __FUNCTION__));
#if NEXUS_HAS_GRAPHICS3D && defined(BCMNEXUS_GFX_HAS_P3D)
    BDBG_ASSERT(hSurface);
    BDBG_ASSERT(phSur);
#if NEXUS_GRAPHICS3D_MODE_PROXY
    {
     int width, height;
     Nexus_Proxy_P3D_GetSurface(hSurface, phSur, &width, &height);
    }
#else
     NEXUS_Module_Lock( g_NEXUS_graphics2DData.settings.surface );
     *phSur = NEXUS_Surface_GetSurface_priv( hSurface );
     NEXUS_Module_Unlock( g_NEXUS_graphics2DData.settings.surface );
#endif
#endif
    BDBG_MSG_TRACE(("<<%s", __FUNCTION__));
}

void DFB_Platform_P_Graphics3D_OpenP3D( BP3D_Handle *phP3d )
{
    BDBG_MSG_TRACE((">>%s", __FUNCTION__));
#if NEXUS_HAS_GRAPHICS3D && defined(BCMNEXUS_GFX_HAS_P3D)
#if NEXUS_GRAPHICS3D_MODE_PROXY
    {
    BMEM_Handle             hMem;
    Nexus_Proxy_P3D_Open(&hMem, phP3d);
    }
#else
    *phP3d = NEXUS_Graphics3d_GetP3d_priv();
#endif
#endif
    BDBG_MSG_TRACE(("<<%s", __FUNCTION__));
}

void DFB_Platform_P_Graphics3D_CloseP3D( BP3D_Handle hP3d )
{
    BDBG_MSG_TRACE((">>%s", __FUNCTION__));
#if NEXUS_HAS_GRAPHICS3D && defined(BCMNEXUS_GFX_HAS_P3D) && NEXUS_GRAPHICS3D_MODE_PROXY
    Nexus_Proxy_P3D_Close(hP3d);
#else
    (void)hP3d;
#endif
    BDBG_MSG_TRACE(("<<%s", __FUNCTION__));
}
#endif

DFB_PlatformResult
DFB_Platform_P_Graphics3D_PoolInit( DFB_Platform_P_PoolHandle *pPoolHandle )
{
    DFB_PlatformResult  ret;
#if NEXUS_HAS_GRAPHICS3D && defined(BCMNEXUS_GFX_HAS_V3D)
    char               *error;
    char               *libgfx3ddriv = DIRECTFB_PREFIX "/lib/" DIRECTFB_GFX_3D_DRIV_LIB;
    BEGL_DriverInterfaces beglDriverInterfaces;
    static void        (*DFB_Platform_P_GetDefaultDriverInterfaces)( BEGL_DriverInterfaces *driverInterfaces );

    BDBG_MSG_TRACE((">>%s", __FUNCTION__));

    BDBG_ASSERT(pPoolHandle != NULL);

    ret = DFB_PLATFORM_OK;

    if (dfb_p_isGraphics3DPoolInit == false)
    {
        BDBG_MSG(("Trying to dlopen library %s...", libgfx3ddriv));
        graphics3dDriverHandle = dlopen( libgfx3ddriv, RTLD_NOW );

        if (!graphics3dDriverHandle)
        {
            BDBG_ERR(("%s: Unable to dlopen `%s'!", __FUNCTION__, libgfx3ddriv ));
            BDBG_ERR(("%s: '%s' required for DirectFB / OpenGL applications (otherwise you can ignore this error)", __FUNCTION__, libgfx3ddriv ));
            return DFB_PLATFORM_INIT;
        }
        else
        {
            dlerror();    /* Clear any existing error */

            *(void **) (&DFB_Platform_P_GetDefaultDriverInterfaces) = dlsym(graphics3dDriverHandle, "BEGL_GetDefaultDriverInterfaces");
            if ((error = dlerror()) != NULL || (DFB_Platform_P_GetDefaultDriverInterfaces == NULL))
            {
                BDBG_ERR(("%s: Failure tying to find \"BEGL_GetDefaultDriverInterfaces\" function (%s)", __FUNCTION__, error));
                ret = DFB_PLATFORM_FAILURE;
            }
            else
            {
                (*DFB_Platform_P_GetDefaultDriverInterfaces) (&beglDriverInterfaces);
                *(void **) (&DFB_Platform_P_BufferGetRequirements) = beglDriverInterfaces.displayCallbacks.BufferGetRequirements;
            }

        }

        if (ret == DFB_PLATFORM_OK)
        {
            *pPoolHandle = (DFB_Platform_P_PoolHandle)0xFACEF00D;
            dfb_p_isGraphics3DPoolInit = true;
        }
    }
    BDBG_MSG_TRACE(("<<%s", __FUNCTION__));
#else
    (void)pPoolHandle;

    ret = DFB_PLATFORM_UNSUPPORTED;
#endif
    return ret;
}

void
DFB_Platform_P_Graphics3D_PoolUninit( DFB_Platform_P_PoolHandle poolHandle )
{
    (void)poolHandle;
#if NEXUS_HAS_GRAPHICS3D && defined(BCMNEXUS_GFX_HAS_V3D)
    BDBG_MSG_TRACE((">>%s", __FUNCTION__));
    if (dfb_p_isGraphics3DPoolInit == true)
    {
        if (graphics3dDriverHandle)
        {
            dlclose(graphics3dDriverHandle);
            graphics3dDriverHandle = NULL;
        }
        dfb_p_isGraphics3DPoolInit = false;
    }
    BDBG_MSG_TRACE(("<<%s", __FUNCTION__));
#endif
}

DFB_PlatformResult
DFB_Platform_P_Graphics3D_GetBufferSize( DFB_Platform_P_PoolHandle poolHandle,
                                         int                       width,
                                         int                       height,
                                         DFB_PlatformPixelFormat   pixelformat,
                                         int                      *pitch,
                                         int                      *size )
{
    DFB_PlatformResult ret = DFB_PLATFORM_UNSUPPORTED;

    BDBG_ASSERT(pitch != NULL);
    BDBG_ASSERT(size  != NULL);

    (void)poolHandle;

#if NEXUS_HAS_GRAPHICS3D && defined(BCMNEXUS_GFX_HAS_V3D)
    BDBG_MSG_TRACE((">>%s", __FUNCTION__));
    ret = DFB_PLATFORM_INIT;
    if (dfb_p_isGraphics3DPoolInit == true)
    {
        BEGL_PixmapInfo     bufferRequirements;
        BEGL_BufferSettings bufferConstrainedRequirements;

        if (DFB_Platform_P_BufferGetRequirements)
        {
            memset(&bufferRequirements, 0, sizeof(bufferRequirements));
            bufferRequirements.width = width;
            bufferRequirements.height = height;

            ret = DFB_PLATFORM_OK;

            switch (pixelformat)
            {
            case DFB_PLATFORM_PIXELFORMAT_ABGR:
                bufferRequirements.format = BEGL_BufferFormat_eA8B8G8R8;
                break;
            case DFB_PLATFORM_PIXELFORMAT_RGB32:
                bufferRequirements.format = BEGL_BufferFormat_eX8B8G8R8;
                break;
            case DFB_PLATFORM_PIXELFORMAT_RGB16:
                bufferRequirements.format = BEGL_BufferFormat_eR5G6B5;
                break;
            case DFB_PLATFORM_PIXELFORMAT_UYVY:
                bufferRequirements.format = BEGL_BufferFormat_eYUV422_Texture;
                break;
            default:
                BDBG_WRN(("%s: Cannot support platform pixel format %d!\n", __FUNCTION__, pixelformat));
                ret = DFB_PLATFORM_UNSUPPORTED;
            }

            if (ret == DFB_PLATFORM_OK)
            {
                (*DFB_Platform_P_BufferGetRequirements)( &bufferRequirements, &bufferConstrainedRequirements);

                *pitch = bufferConstrainedRequirements.pitchBytes;
                *size  = bufferConstrainedRequirements.totalByteSize;
                BDBG_MSG(("%s: Buffer %dx%d, platform pixel format=%d, size=%d, pitch=%d", __FUNCTION__,
                          width, height, pixelformat, *size, *pitch));
            }
        }
    }
    BDBG_MSG_TRACE(("<<%s", __FUNCTION__));
#else
    (void)width;
    (void)height;
    (void)pixelformat;
#endif
    return ret;
}

bool
DFB_Platform_P_PictureDecoder_GetDefaultInitSettings( DFB_PlatformPictureDecoderInitSettings *pSettings )
{
    BDBG_MSG_TRACE((">>%s", __FUNCTION__));

#if NEXUS_HAS_PICTURE_DECODER
    memset(pSettings, 0, sizeof(DFB_PlatformPictureDecoderInitSettings));
    pSettings->multiScanBufferSize =  1*1024*1024;
    pSettings->bufferSize          =  1*1024*1024;
    return true;
#else
    BSTD_UNUSED(pSettings);
#endif
    BDBG_MSG_TRACE(("<<%s", __FUNCTION__));
    return false;
}

DFB_PlatformResult
DFB_Platform_P_PictureDecoder_Init( DFB_PlatformPictureDecoderInitSettings *pSettings,
                                    DFB_PlatformNexusHandle                *pHandle )
{
    DFB_PlatformResult res = DFB_PLATFORM_UNSUPPORTED;

    BDBG_MSG_TRACE((">>%s", __FUNCTION__));

    BDBG_ASSERT(pHandle);

#if NEXUS_HAS_PICTURE_DECODER
    NEXUS_PictureDecoderOpenSettings decoderSettings;

    NEXUS_PictureDecoder_GetDefaultOpenSettings(&decoderSettings);
    decoderSettings.heap = graphics2dHeap;
    decoderSettings.bufferSize = pSettings->bufferSize;
    decoderSettings.multiScanBufferSize = pSettings->multiScanBufferSize;
    *pHandle = NEXUS_PictureDecoder_Open(0, &decoderSettings);
    if (*pHandle)
    {
        BDBG_MSG(("%s: Picture Decoder Handle=%p\n", __FUNCTION__, *pHandle));
        res = DFB_PLATFORM_OK;
    }
    else
    {
        BDBG_ERR(("%s: Cannot open picture decoder!\n", __FUNCTION__));
        res = DFB_PLATFORM_FAILURE;
    }
#else
    BSTD_UNUSED(pSettings);
    *pHandle = NULL;
#endif
    BDBG_MSG_TRACE(("<<%s", __FUNCTION__));
    return res;
}

void
DFB_Platform_P_PictureDecoder_Uninit( DFB_PlatformNexusHandle handle )
{
    BDBG_MSG_TRACE((">>%s", __FUNCTION__));

    BDBG_ASSERT(handle);

#ifdef NEXUS_HAS_PICTURE_DECODER
    NEXUS_PictureDecoder_Close(handle);
#endif

    BDBG_MSG_TRACE(("<<%s", __FUNCTION__));
}


/* ******************************************************************************************** */


DFB_PlatformResult
DFB_Platform_P_GetDisplaySize( int                    displayIndex,
                               DFB_PlatformDimension *pSize )
{
    if (displayIndex < NUM_DISPLAYS)
    {
        *pSize = dfb_p_displaysettings->display[displayIndex].displaySize;
        return DFB_PLATFORM_OK;
    }
    else
    {
        return DFB_PLATFORM_INVARG;
    }
}

DFB_PlatformResult
DFB_Platform_P_GetDisplaySettings( DFB_PlatformNexusHandle         displayHandle,
                                   DFB_Platform_P_DisplaySettings *pSettings)
{
    DFB_PlatformResult res = DFB_PLATFORM_OK;
    int       displayIndex;

    BDBG_MSG_TRACE((">>%s", __FUNCTION__));

    BDBG_ASSERT(pSettings);

    displayIndex = DFB_Platform_P_GetDisplayIndex(displayHandle);

    if (displayIndex >= 0)
        memcpy(pSettings, &dfb_p_displaysettings->display[displayIndex].displaySettings, sizeof(*pSettings));
    else
        res = DFB_PLATFORM_INVARG;

    BDBG_MSG_TRACE(("<<%s", __FUNCTION__));
    return res;
}

/* ******************************************************************************************** */


#ifdef BCMNEXUS_VIDEO_DECODER_MFD_AVD_MAPPING

NEXUS_HeapHandle
DFB_Platform_P_GetVideoGraphicsHeap( unsigned mfdIndex )
{
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_PlatformSettings platformSettings;
    unsigned int mfdHeap;
    unsigned int targetMemc;
    NEXUS_HeapHandle heap = NULL;
    NEXUS_MemoryStatus heapStatus;
    unsigned int i;

    NEXUS_Platform_GetSettings(&platformSettings);

    /* Note that MFDx corresponds to NEXUS_VideoDecoder_Open(x) */
    mfdHeap = platformSettings.videoDecoderModuleSettings.avdHeapIndex[platformSettings.videoDecoderModuleSettings.avdMapping[mfdIndex]];
    NEXUS_Platform_GetConfiguration(&platformConfig);

    if (platformConfig.heap[mfdHeap])
        NEXUS_Heap_GetStatus(platformConfig.heap[mfdHeap], &heapStatus);
    else
        return platformConfig.heap[platformSettings.videoDecoderModuleSettings.avdHeapIndex[0]]; // Not ideal but stops things going bang

    targetMemc = heapStatus.memcIndex;

    /* We prefer NEXUS_MEMCx_GRAPHICS_HEAP because it is defined to be NEXUS_MemoryType_eApplication and as large as possible,
       we just don't know if it's MFDx-accessible. */
#ifdef NEXUS_MEMC0_GRAPHICS_HEAP
    if (!heap && platformConfig.heap[NEXUS_MEMC0_GRAPHICS_HEAP])
    {
        NEXUS_Heap_GetStatus(platformConfig.heap[NEXUS_MEMC0_GRAPHICS_HEAP], &heapStatus);

        if ( (heapStatus.memcIndex == targetMemc) && (heapStatus.memoryType & NEXUS_MemoryType_eApplication) )
            heap = platformConfig.heap[NEXUS_MEMC0_GRAPHICS_HEAP];
    }
#endif

#ifdef NEXUS_MEMC1_GRAPHICS_HEAP
    if (!heap && platformConfig.heap[NEXUS_MEMC1_GRAPHICS_HEAP])
    {
        NEXUS_Heap_GetStatus(platformConfig.heap[NEXUS_MEMC1_GRAPHICS_HEAP], &heapStatus);

        if ( (heapStatus.memcIndex == targetMemc) && (heapStatus.memoryType & NEXUS_MemoryType_eApplication) )
            heap = platformConfig.heap[NEXUS_MEMC1_GRAPHICS_HEAP];
    }
#endif

    /* If that doesn't work, find any heap that meets the requirements. hopefully it's not a NEXUS_MEMCx_PICTURE_BUFFER_HEAP
       because that will collide with XVD/VDC spreadsheet calculations. */
    if (!heap) {
        for (i=0;i<NEXUS_MAX_HEAPS;i++)
        {
            if (platformConfig.heap[i])
            {
                NEXUS_Heap_GetStatus(platformConfig.heap[i], &heapStatus);
                if ( (heapStatus.memcIndex == targetMemc) && (heapStatus.memoryType & NEXUS_MemoryType_eApplication) )
                {
                    heap = platformConfig.heap[i];
                    break;
                }
            }
        }
    }

    /* If that doesn't work, we need to modify the default platform settings */
    if (!heap)
        BDBG_ERR(("Unable to find Nexus heap for Video Graphics"));

    return heap;

}
#endif

void
DFB_Platform_P_SetHeapHandles( DFB_PlatformSettings *pSettings )
{
    NEXUS_PlatformSettings      platformSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_MemoryStatus          memoryStatus;
    NEXUS_Error                 rc;

    BDBG_MSG_TRACE((">>%s", __FUNCTION__));

    NEXUS_Platform_GetSettings(&platformSettings);
    NEXUS_Platform_GetConfiguration(&platformConfig);

    // In this case the heaps have already been configured correctly
    // for the system by Nexus. All we need to do is to ask nicely

    // unauth clients can't access any heap except the offscreen heap
#if !BCMNEXUS_NSC_SUPPORT
    if ( pSettings->trustUnauthenticatedClients || !pSettings->slave)
#else
    if (0)
#endif
    {

        if (pSettings->displayHeapHandle == NULL)
            pSettings->displayHeapHandle = NEXUS_Platform_GetFramebufferHeap(0);

        BDBG_ASSERT(pSettings->displayHeapHandle);

        rc = NEXUS_Heap_GetStatus(pSettings->displayHeapHandle, &memoryStatus);

        if (rc == NEXUS_SUCCESS)
            BDBG_WRN(("Display heap: handle=%p size %dkB - free %dkB - largest block %dkB",
                        pSettings->displayHeapHandle, memoryStatus.size/1024,memoryStatus.free/1024,memoryStatus.largestFreeBlock/1024));

        if (pSettings->offscreenHeapHandle == NULL)
        {
            NEXUS_HeapHandle offscreen = NEXUS_Platform_GetFramebufferHeap(NEXUS_OFFSCREEN_SURFACE);

            if (offscreen == NULL)
            {
                BDBG_WRN(("Failed to get offscreen heap details from Nexus looking for alternative ..."));
                offscreen = platformConfig.heap[2]; // 40nm SOC default Application heap

                if (offscreen == NULL) // lets just use the same as the main heap
                    offscreen = pSettings->displayHeapHandle;
            }
            pSettings->offscreenHeapHandle = offscreen;
        }

        rc= NEXUS_Heap_GetStatus(pSettings->offscreenHeapHandle, &memoryStatus);

        if (rc == NEXUS_SUCCESS)
            BDBG_WRN(("Offscreen heap: handle=%p size %dkB - free %dkB - largest block %dkB",
                        pSettings->offscreenHeapHandle, memoryStatus.size/1024,memoryStatus.free/1024,memoryStatus.largestFreeBlock/1024));

        if (pSettings->secondaryDisplayHeapHandle == NULL)
        {
            pSettings->secondaryDisplayHeapHandle = NEXUS_Platform_GetFramebufferHeap(1);
            if (pSettings->secondaryDisplayHeapHandle == NULL)
                pSettings->secondaryDisplayHeapHandle =  pSettings->displayHeapHandle;
        }


    }
    else // Nexus slave app - we need a different Nexus API to get our heap memory.
    {

        NEXUS_ClientConfiguration clientConfig;

        NEXUS_Platform_GetClientConfiguration(&clientConfig);

#if !BCMNEXUS_NSC_SUPPORT
        if (pSettings->trustUnauthenticatedClients)
        {
            pSettings->displayHeapHandle          = clientConfig.heap[0];
            pSettings->offscreenHeapHandle        = clientConfig.heap[1];
            pSettings->videoHeapHandle            = clientConfig.heap[2];
            pSettings->paletteHeapHandle          = clientConfig.heap[3];
            pSettings->pictureDecoderHeapHandle   = clientConfig.heap[3];
            pSettings->secondaryDisplayHeapHandle = clientConfig.heap[4];

            rc = NEXUS_Heap_GetStatus(pSettings->displayHeapHandle, &memoryStatus);

            if (rc == NEXUS_SUCCESS)
                BDBG_WRN(("Slave Display heap: size %dkB - free %dkB - largest block %dkB",memoryStatus.size/1024,memoryStatus.free/1024,memoryStatus.largestFreeBlock/1024));
        }
        else // Untrusted, we only have offscreen memory
        {
#if !BCMNEXUS_PROTECTED_CLIENT_SUPPORT
            DFB_PlatformSettings dfbPlatformSettings;

            DFB_Platform_P_GetSettings(&dfbPlatformSettings);
            pSettings->offscreenHeapHandle = dfbPlatformSettings.offscreenHeapHandle;
#else
            pSettings->offscreenHeapHandle = clientConfig.heap[0];
#endif
            pSettings->displayHeapHandle   = NULL;
        }

#else   /* BCMNEXUS_NSC_SUPPORT */
        {
            int i;
            NEXUS_HeapHandle heap;

#if BCMNEXUS_NSC_SUPPORT == BCMNEXUS_NSC_TUNNELLED_MODE
            /* Tunnelled mode we are a protected slave with access to all heaps
               so Nexus platform heap functions will work */
            heap = NEXUS_Platform_GetFramebufferHeap(NEXUS_OFFSCREEN_SURFACE);
            graphics2dHeap = graphics2dBoundsHeap = NULL;

#else
            /* In multi buffered mode we are untrusted so only have 1 heap
               the offscreen heap, but heap no. is different from master so can't
               use the nexus get frame buffer heap functions */
            heap = clientConfig.heap[0];
            graphics2dHeap = clientConfig.heap[NEXUS_MAX_HEAPS-1];
            graphics2dBoundsHeap = heap;
#endif

            // Set all of our heaps to be the same.
            pSettings->displayHeapHandle        =
            pSettings->offscreenHeapHandle      =
            pSettings->videoHeapHandle          =
            pSettings->paletteHeapHandle        =
            pSettings->pictureDecoderHeapHandle = heap;

            for (i = 0 ; i < NEXUS_MAX_HEAPS ; ++i)
            {
                if (clientConfig.heap[i] != NULL)
                {
                    rc = NEXUS_Heap_GetStatus(clientConfig.heap[i], &memoryStatus);
                    if (rc == NEXUS_SUCCESS )
                        BDBG_ERR(("Heap[%d]: type=0x%x, handle=%p, size %dkB - free %dkB - largest block %dkB - base %p",
                                  i, memoryStatus.memoryType, (void *)heap, memoryStatus.size/1024, memoryStatus.free/1024, memoryStatus.largestFreeBlock/1024, memoryStatus.addr));
                }
            }
        }
#endif  /* BCMNEXUS_NSC_SUPPORT */

        if (pSettings->offscreenHeapHandle != NULL)
        {
            rc = NEXUS_Heap_GetStatus(pSettings->offscreenHeapHandle, &memoryStatus);

            if (rc == NEXUS_SUCCESS)
                BDBG_WRN(("Slave Offscreen heap: size %dkB - free %dkB - largest block %dkB",memoryStatus.size/1024,memoryStatus.free/1024,memoryStatus.largestFreeBlock/1024));
        }
        else
            BDBG_WRN(("Failed to get valid slave offscreen heap"));
    }

    // Always on memc0 for reg access in nexus gfx 2d apparently.
    if (pSettings->paletteHeapHandle == NULL)
        pSettings->paletteHeapHandle = platformConfig.heap[0];

    if (pSettings->videoHeapHandle == NULL)
        if ( pSettings->trustUnauthenticatedClients || !pSettings->slave) // not for untrusted slave apps atm
#ifdef BCMNEXUS_VIDEO_DECODER_MFD_AVD_MAPPING
            pSettings->videoHeapHandle = DFB_Platform_P_GetVideoGraphicsHeap(0);
#else
            pSettings->videoHeapHandle = platformConfig.heap[platformSettings.videoDecoderModuleSettings.avdHeapIndex[0]];
#endif

#ifdef NEXUS_HAS_PICTURE_DECODER
    if (pSettings->pictureDecoderHeapHandle == NULL)
        pSettings->pictureDecoderHeapHandle = platformConfig.heap[0];
#endif

    BDBG_MSG_TRACE(("<<%s", __FUNCTION__));
}

void DFB_Platform_P_GetDefaultSettings( DFB_PlatformSettings *pSettings )
{
    int                    heap;

    BDBG_MSG_TRACE((">>%s", __FUNCTION__));

#if BCMNEXUS_NSC_SUPPORT
    dfb_p_isNexusPlatformSettingsAcquired = true;
    pSettings->slave = true;
#endif

    if (dfb_p_isNexusPlatformSettingsAcquired == false)
    {
        /* If Nexus has already been initialised externally, then just get the current settings */
#ifndef BCMNEXUS_CLIENT_LIBRARY
        if (pSettings->initOrJoinNexus == true)
            NEXUS_Platform_GetDefaultSettings(&nexusPlatformSettings);
        else
#endif
            NEXUS_Platform_GetSettings(&nexusPlatformSettings);

        dfb_p_isNexusPlatformSettingsAcquired = true;
    }

    pSettings->platformSettings.openI2c       = nexusPlatformSettings.openI2c;
    pSettings->platformSettings.openFpga      = nexusPlatformSettings.openFpga;
    pSettings->platformSettings.openFrontend  = nexusPlatformSettings.openFrontend;
    pSettings->platformSettings.pipPbpEnabled = false;
    pSettings->platformSettings.startPanelPwm = false;

    for (heap = 0; heap < DFB_PLATFORM_MAX_HEAPS, heap < NEXUS_MAX_HEAPS; heap++)
    {
        pSettings->platformSettings.heap[heap].memcIndex    = nexusPlatformSettings.heap[heap].memcIndex;
        pSettings->platformSettings.heap[heap].subIndex     = nexusPlatformSettings.heap[heap].subIndex;
        pSettings->platformSettings.heap[heap].size         = nexusPlatformSettings.heap[heap].size;
        pSettings->platformSettings.heap[heap].alignment    = nexusPlatformSettings.heap[heap].alignment;
        pSettings->platformSettings.heap[heap].guardBanding = nexusPlatformSettings.heap[heap].guardBanding;
        pSettings->platformSettings.heap[heap].memoryType   = nexusPlatformSettings.heap[heap].memoryType;
    }

    BDBG_MSG_TRACE(("<<%s", __FUNCTION__));
}

DFB_PlatformResult
DFB_Platform_P_StartServer(DFB_PlatformSettings  *pSettings)
{
#if BCMNEXUS_HAS_IPC
    NEXUS_PlatformStartServerSettings   serverSettings;
    NEXUS_Error                         rc;
#if BCMNEXUS_AUTHENTICATION_KEY
    NEXUS_ClientSettings                clientSettings;
#endif

    NEXUS_Platform_GetDefaultStartServerSettings(&serverSettings);

    serverSettings.allowUnauthenticatedClients = pSettings->allowUnauthenticatedClients;

    if (pSettings->trustUnauthenticatedClients)
    {
        NEXUS_PlatformConfiguration nexusPlatformConfig;
        NEXUS_HeapHandle * pHeap = NULL;

        NEXUS_Platform_GetConfiguration(&nexusPlatformConfig);

#if !BCMNEXUS_AUTHENTICATION_KEY

#if BCMNEXUS_PROTECTED_CLIENT_SUPPORT
        {
            int i;

            /* If we are to trust unauthenticated clients, then we enter unprotected mode which provides full access to the nexus heaps. */
        serverSettings.unauthenticatedConfiguration.mode = NEXUS_ClientMode_eUnprotected;
        pHeap = serverSettings.unauthenticatedConfiguration.heap;

            for (i = 0; i < NEXUS_MAX_HEAPS; i++) {
                pHeap[i] = nexusPlatformConfig.heap[i];
                BDBG_MSG(("%s: heap[%d] handle=%p", __FUNCTION__, i, (void *)pHeap[i]));
            }

            pHeap = NULL; /* Prevent further settings of heaps */
        }
#endif

#else // !BCMNEXUS_AUTHENTICATION_KEY

        NEXUS_Platform_GetDefaultClientSettings(&clientSettings);
#if BCMNEXUS_PROTECTED_CLIENT_SUPPORT
        serverSettings.unauthenticatedConfiguration.mode = NEXUS_ClientMode_eUntrusted; // Need to authenticate to get memory
        clientSettings.configuration.mode                = NEXUS_ClientMode_eUnprotected;
#endif
        pHeap = clientSettings.configuration.heap;

#endif // !BCMNEXUS_AUTHENTICATION_KEY

        if (pHeap != NULL)
        {
            pHeap[0] =  NEXUS_Platform_GetFramebufferHeap(0);
            pHeap[1] =  NEXUS_Platform_GetFramebufferHeap(NEXUS_OFFSCREEN_SURFACE);
#ifdef BCMNEXUS_VIDEO_DECODER_MFD_AVD_MAPPING
            pHeap[2] =  DFB_Platform_P_GetVideoGraphicsHeap(0);
#endif
            pHeap[3] =  nexusPlatformConfig.heap[0]; // slightly risky ??
            pHeap[4] =  NEXUS_Platform_GetFramebufferHeap(1);
        }
    }
    else
    {
#if BCMNEXUS_PROTECTED_CLIENT_SUPPORT
        serverSettings.unauthenticatedConfiguration.mode = NEXUS_ClientMode_eUntrusted;
#endif

#if !BCMNEXUS_AUTHENTICATION_KEY
#if BCMNEXUS_PROTECTED_CLIENT_SUPPORT
        serverSettings.unauthenticatedConfiguration.heap[0] =  NEXUS_Platform_GetFramebufferHeap(NEXUS_OFFSCREEN_SURFACE);

        if (serverSettings.unauthenticatedConfiguration.heap[0] == NULL)
        {
            // Sticky situation, there doesn't appear to be a valid offscreen heap.
            BDBG_ERR(("*** NEXUS heap configuration is broken, no valid offscreen graphics heap. ***"));
            return DFB_PLATFORM_FAILURE;

        }

    BDBG_WRN(("%s: Setting unauth heap to %p",__FUNCTION__,serverSettings.unauthenticatedConfiguration.heap[0]));
#endif

#else

#if BCMNEXUS_PROTECTED_CLIENT_SUPPORT
        serverSettings.unauthenticatedConfiguration.heap[0] =  NULL; // have to authenticate to get access to heap
        clientSettings.configuration.mode                   =  NEXUS_ClientMode_eUntrusted;
#endif
        clientSettings.configuration.heap[0]                =  NEXUS_Platform_GetFramebufferHeap(NEXUS_OFFSCREEN_SURFACE);
#endif
    }

    /* Work around for 7425 Nexus Ph 3.5  */
    /* Slave applications have to be trusted to access memory */
#if !BCMNEXUS_PROTECTED_CLIENT_SUPPORT
#if BCMNEXUS_ALLOW_TRUSTED_CLIENTS_TO_CRASH_SUPPORT
    serverSettings.allowTrustedClientsToCrash = false;
#endif
    serverSettings.allowUnauthenticatedClients = true;
#if BCMNEXUS_TRUST_UNAUTHENTICATED_CLIENTS_SUPPORT
    serverSettings.trustUnauthenticatedClients = true;
#endif

#if BCMNEXUS_DEFAULT_UNTRUSTED_HEAP_SUPPORT
    serverSettings.defaultUntrustedHeap =  NEXUS_Platform_GetFramebufferHeap(NEXUS_OFFSCREEN_SURFACE);
#endif
    pSettings->trustUnauthenticatedClients = true;
#endif

    BDBG_WRN(("%s: Nexus server client settings - allow unauth %c - trust unauth %c",__FUNCTION__,
              pSettings->allowUnauthenticatedClients ? 'y' : 'n',
              pSettings->trustUnauthenticatedClients ? 'y' : 'n'));

    rc = NEXUS_Platform_StartServer(&serverSettings);
    BDBG_ASSERT(!rc);

    dfb_p_isIPCServerOpened = true;

#if BCMNEXUS_AUTHENTICATION_KEY
    memcpy(clientSettings.authentication.certificate.data,authenticationKey,sizeof(authenticationKey));
    clientSettings.authentication.certificate.length = sizeof(authenticationKey);

    NEXUS_Platform_RegisterClient(&clientSettings);
#endif

#else /* BCMNEXUS_HAS_IPC */
    (void)pSettings;
#endif /* BCMNEXUS_HAS_IPC */

    return DFB_PLATFORM_OK;
}

DFB_PlatformResult
DFB_Platform_P_Init( DFB_PlatformSettings  *pSettings,
                     DFB_P_DisplaySettings *pPrivSettings )
{
    int          heap;
    NEXUS_Error  rc;

    BDBG_MSG_TRACE((">>%s", __FUNCTION__));

    dfb_p_displaysettings = pPrivSettings;

    /* Do we need to initialise/join NEXUS? */
    if (pSettings->initOrJoinNexus)
    {
        /* The slave would initialize the audio/video/window/etc nexus resources */
        if (pSettings->slave)
        {
#if BCMNEXUS_AUTHENTICATION_KEY
            NEXUS_ClientAuthenticationSettings authSettings;

            NEXUS_Platform_GetDefaultClientAuthenticationSettings(&authSettings);
            memcpy(authSettings.certificate.data,authenticationKey,sizeof(authenticationKey));
            authSettings.certificate.length = sizeof(authenticationKey);
            rc = NEXUS_Platform_AuthenticatedJoin(&authSettings);
#else
            rc = NEXUS_Platform_Join();
#endif

            if(rc != NEXUS_SUCCESS)
            {
                BDBG_ERR(("%s: Failed to join to Nexus master process",__FUNCTION__));
#if BCMNEXUS_NSC_SUPPORT
                BDBG_ERR(("DirectFB is being used in XS mode, do you have a running Nexus master application with Surface Compositor support?"));
#endif
                rc=BERR_TRACE(rc);
                return DFB_PLATFORM_INIT;
            }

            BDBG_MSG(("%s: Nexus Joined\n", __FUNCTION__));
        }
        else
        {
            if (dfb_p_isNexusPlatformSettingsAcquired == false)
            {
#ifndef BCMNEXUS_CLIENT_LIBRARY
                NEXUS_Platform_GetDefaultSettings(&nexusPlatformSettings);
#else
                NEXUS_Platform_GetSettings(&nexusPlatformSettings);
#endif
                dfb_p_isNexusPlatformSettingsAcquired = true;
            }

            nexusPlatformSettings.openI2c      = pSettings->platformSettings.openI2c;
            nexusPlatformSettings.openFpga     = pSettings->platformSettings.openFpga;
            nexusPlatformSettings.openFrontend = pSettings->platformSettings.openFrontend;

            for (heap = 0; heap < DFB_PLATFORM_MAX_HEAPS, heap < NEXUS_MAX_HEAPS; heap++)
            {
                nexusPlatformSettings.heap[heap].memcIndex    = pSettings->platformSettings.heap[heap].memcIndex;
                nexusPlatformSettings.heap[heap].subIndex     = pSettings->platformSettings.heap[heap].subIndex;
                nexusPlatformSettings.heap[heap].size         = pSettings->platformSettings.heap[heap].size;
                nexusPlatformSettings.heap[heap].alignment    = pSettings->platformSettings.heap[heap].alignment;
                nexusPlatformSettings.heap[heap].guardBanding = pSettings->platformSettings.heap[heap].guardBanding;
                nexusPlatformSettings.heap[heap].memoryType   = pSettings->platformSettings.heap[heap].memoryType;
            }

#if defined(HUMAX_PLATFORM_BASE)&& defined(CONFIG_BCM_CAS_MULTI2)
			nexusPlatformSettings.securitySettings.enableMulti2Key = true;
#endif
            rc = NEXUS_Platform_Init(&nexusPlatformSettings);
            if (rc != NEXUS_SUCCESS)
            {
                BDBG_ERR(("%s: Failed to initialise to Nexus",__FUNCTION__));
                rc=BERR_TRACE(rc);
                return DFB_PLATFORM_INIT;
            }

            BDBG_MSG(("%s: NEXUS Initialised", __FUNCTION__));

#if FUSION_BUILD_MULTI
            if (DFB_Platform_P_StartServer(pSettings) != DFB_PLATFORM_OK)
            {
                BDBG_ERR(("Failed to start Nexus Server, multi app will not work!!"));
                return DFB_PLATFORM_INIT;
            }
#endif
        }
        dfb_p_isPlatformInited = true;
    }
    BDBG_MSG_TRACE(("<<%s", __FUNCTION__));
    return DFB_PLATFORM_OK;
}

DFB_PlatformResult
DFB_Platform_P_Uninit( void )
{
    BDBG_MSG_TRACE(("<<%s", __FUNCTION__));

#if BCMNEXUS_HAS_IPC
    if (dfb_p_isIPCServerOpened)
    {
        NEXUS_Platform_StopServer();
        dfb_p_isIPCServerOpened = false;
    }
#endif
    if (dfb_p_isPlatformInited)
    {
        NEXUS_Platform_Uninit();
        dfb_p_isNexusPlatformSettingsAcquired = false;
        dfb_p_isPlatformInited = false;
    }

    BDBG_MSG_TRACE((">>%s", __FUNCTION__));
    return DFB_PLATFORM_OK;
}
