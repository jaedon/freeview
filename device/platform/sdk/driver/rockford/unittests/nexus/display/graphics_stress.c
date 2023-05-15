/***************************************************************************
*     Copyright (c) 2004-2012, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: graphics_stress.c $
* $brcm_Revision: Hydra_Software_Devel/4 $
* $brcm_Date: 7/31/12 4:54p $
*
*
* Revision History:
*
* $brcm_Log: /rockford/unittests/nexus/display/graphics_stress.c $
* 
* Hydra_Software_Devel/4   7/31/12 4:54p erickson
* SW7429-213: add WRN to clarify nature of stress test
* 
* Hydra_Software_Devel/3   5/14/12 12:08p erickson
* SW7346-824: add test that would have caught bug
* 
* Hydra_Software_Devel/2   1/13/12 8:33p erickson
* SW7425-2145: fix rightViewOffset logic
* 
* Hydra_Software_Devel/1   9/8/11 2:18p erickson
* SW7425-1267: add unittest for GFD
*
***************************************************************************/

#include "nexus_platform.h"
#include "nexus_display.h"
#include "bstd.h"
#include "bkni.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

BDBG_MODULE(graphics_stress);

int main(void)
{
    NEXUS_DisplayHandle display[NEXUS_NUM_DISPLAYS];
    NEXUS_GraphicsSettings graphicsSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_PlatformSettings platformSettings;
    NEXUS_SurfaceHandle framebuffer;
    NEXUS_SurfaceCreateSettings surfaceCreateSettings;
    int rc;
    unsigned loops = 1000;
    unsigned i;

    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    rc = NEXUS_Platform_Init(&platformSettings);
    if (rc) return -1;

    BKNI_Memset(display, 0, sizeof(display));
    NEXUS_Platform_GetConfiguration(&platformConfig);

    NEXUS_Surface_GetDefaultCreateSettings(&surfaceCreateSettings);
    surfaceCreateSettings.pixelFormat = NEXUS_PixelFormat_eA8_R8_G8_B8;
    surfaceCreateSettings.width = 720;
    surfaceCreateSettings.height = 480;
    surfaceCreateSettings.heap = NEXUS_Platform_GetFramebufferHeap(0);
    framebuffer = NEXUS_Surface_Create(&surfaceCreateSettings);
    BDBG_ASSERT(framebuffer);
    
    BDBG_WRN(("this is a stress test. lower-level software may report BDBG_ERR on the randomized settings, but the system should not crash."));

    while (--loops) {
        unsigned displayIndex = rand()%NEXUS_NUM_DISPLAYS;
        
        if (loops % 100 == 0) printf("loops: %d\n", loops);
        switch (rand() % 6) {
        case 0:
            if (!display[displayIndex]) {
                display[displayIndex] = NEXUS_Display_Open(displayIndex, NULL);
                BDBG_ASSERT(display[displayIndex]);
            }
            break;
        case 1:
            if (display[displayIndex]) {
                NEXUS_Display_Close(display[displayIndex]);
                display[displayIndex] = NULL;
            }
            break;
        case 2:
            if (display[displayIndex]) {
                NEXUS_Display_GetGraphicsSettings(display[displayIndex], &graphicsSettings);
                graphicsSettings.enabled = rand()%2;
                graphicsSettings.visible = rand()%2;
                graphicsSettings.antiflutterFilter = rand()%2;
                graphicsSettings.alpha = rand()%0xFF;
                graphicsSettings.zorder = rand() % 4;
                graphicsSettings.chromakeyEnabled = rand()%2;
                graphicsSettings.lowerChromakey = rand();
                graphicsSettings.upperChromakey = rand();
                graphicsSettings.position.x = 0;
                graphicsSettings.position.y = 0;
                graphicsSettings.position.width = 720;
                graphicsSettings.position.height = 480;
                graphicsSettings.clip = graphicsSettings.position;
#if 0
                graphicsSettings.sourceBlendFactor = rand()%NEXUS_CompositorBlendFactor_eMax;
                graphicsSettings.destBlendFactor = rand()%NEXUS_CompositorBlendFactor_eMax;
#endif
                graphicsSettings.constantAlpha = rand()%0xFF;
                graphicsSettings.horizontalFilter = rand()%NEXUS_GraphicsFilterCoeffs_eMax;
                graphicsSettings.verticalFilter = rand()%NEXUS_GraphicsFilterCoeffs_eMax;
/* copied from bvdc_gfxfeedertable_priv.c */
#define BVDC_P_GFX_FIRCOEFF_IDX_START (101)
#define BVDC_P_GFX_FIRCOEFF_IDX_END   (126)
                graphicsSettings.horizontalCoeffIndex = (rand()%2)?0:BVDC_P_GFX_FIRCOEFF_IDX_START+rand()%(BVDC_P_GFX_FIRCOEFF_IDX_END-BVDC_P_GFX_FIRCOEFF_IDX_START+1);
                graphicsSettings.verticalCoeffIndex = (rand()%2)?0:BVDC_P_GFX_FIRCOEFF_IDX_START+rand()%(BVDC_P_GFX_FIRCOEFF_IDX_END-BVDC_P_GFX_FIRCOEFF_IDX_START+1);
                
                if (rand()%2) {
                    graphicsSettings.position.width = 720/2;
                    graphicsSettings.graphics3DSettings.rightViewOffset = rand()%graphicsSettings.position.width;
                }
                else {
                    graphicsSettings.graphics3DSettings.rightViewOffset = 0;
                }
                rc = NEXUS_Display_SetGraphicsSettings(display[displayIndex], &graphicsSettings);
                BDBG_ASSERT(!rc);
            }
            break;
        case 3:
            if (display[displayIndex]) {
                NEXUS_Display_SetGraphicsFramebuffer(display[displayIndex], framebuffer);
                BDBG_ASSERT(!rc);
            }
            break;
        case 4:
            if (display[displayIndex]) {
                NEXUS_GraphicsColorSettings settings;
                NEXUS_Display_GetGraphicsColorSettings(display[displayIndex], &settings);
                settings.contrast = rand();
                settings.saturation = rand();
                settings.hue = rand();
                settings.brightness = rand();
                rc = NEXUS_Display_SetGraphicsColorSettings(display[displayIndex], &settings);
                BDBG_ASSERT(!rc);
            }
            break;
        case 5:
            if (display[displayIndex]) {
                NEXUS_ColorMatrix matrix;
                if (rand()%2) {
                    NEXUS_Display_GetGraphicsColorMatrix(display[displayIndex], &matrix);
                    for (i=0;i<NEXUS_COLOR_MATRIX_COEFF_COUNT;i++) {
                        matrix.coeffMatrix[i] = rand();
                    }
                    matrix.shift = rand();
                    rc = NEXUS_Display_SetGraphicsColorMatrix(display[displayIndex], &matrix);
                    BDBG_ASSERT(!rc);
                }
                else {
                    rc = NEXUS_Display_SetGraphicsColorMatrix(display[displayIndex], NULL);
                    BDBG_ASSERT(!rc);
                }
                BDBG_ASSERT(!rc);
            }
            break;
        }
    }

    for (i=0;i<NEXUS_NUM_DISPLAYS;i++) {
        if (display[i]) {
            NEXUS_Display_Close(display[i]);
            display[i] = NULL;
        }
    }
          
    NEXUS_Surface_Destroy(framebuffer);  
    NEXUS_Platform_Uninit();
    BKNI_Printf("success\n");
    return 0;
}
