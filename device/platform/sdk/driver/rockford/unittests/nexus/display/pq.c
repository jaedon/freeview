/***************************************************************************
*     Copyright (c) 2004-2010, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: pq.c $
* $brcm_Revision: Hydra_Software_Devel/22 $
* $brcm_Date: 10/28/10 12:23p $
*
* API Description:
*   API name: Platform
*    Specific APIs to initialze the a board.
*
* Revision History:
*
* $brcm_Log: /rockford/unittests/nexus/display/pq.c $
* 
* Hydra_Software_Devel/22   10/28/10 12:23p mward
* SW7468-6: Use Component on chips with both.  Select valid format for
* Composite.
* 
* Hydra_Software_Devel/21   10/25/10 5:36p randyjew
* SW7468-6: Modifed for chips that do not have Component.
* 
* Hydra_Software_Devel/20   1/11/10 2:00p erickson
* SW7405-3718: added AFD test
*
* Hydra_Software_Devel/19   11/23/09 7:16a erickson
* SW3556-866: add NEXUS_AnalogVideoDecoder_SetColorMatrix
*
* Hydra_Software_Devel/18   11/3/09 3:43p mphillip
* SW7405-3342: Move colorMatrix declaration to allow test to build under
* 7405
*
* Hydra_Software_Devel/17   11/2/09 10:42a erickson
* SW3548-2584: add more color matrix tests
*
* Hydra_Software_Devel/16   10/1/09 11:58a erickson
* SW3548-2512: add NEXUS_VideoWindow_SetColorMatrixNonLinearA/B/C with
* NULL to unset user values
*
* Hydra_Software_Devel/15   7/13/09 10:10a erickson
* PR56759: add NEXUS_PictureCtrl_GetWindowLumaSettings
*
* Hydra_Software_Devel/14   4/17/09 7:14a erickson
* PR54147: add some settop pq tests
*
* Hydra_Software_Devel/13   4/6/09 2:20p erickson
* PR42679: update
*
* Hydra_Software_Devel/12   3/26/09 10:48a erickson
* PR53613: add NEXUS_VideoInput_GetColorMatrix test
*
* Hydra_Software_Devel/11   2/27/09 12:08p erickson
* PR52610: added GetDynamicDefaults functions
*
* Hydra_Software_Devel/10   12/8/08 1:22p erickson
* PR48992: add chroma histogram test
*
* Hydra_Software_Devel/9   12/4/08 9:14a erickson
* PR49901: add NEXUS_VideoWindow_GetCustomAnrData api
*
* Hydra_Software_Devel/8   12/1/08 2:22p erickson
* PR49499: check return codes
*
* Hydra_Software_Devel/7   12/1/08 2:21p erickson
* PR49499: added NEXUS_VideoAdj_SetCustomerSettings test
*
* Hydra_Software_Devel/6   11/26/08 11:07a erickson
* PR42679: update
*
* Hydra_Software_Devel/5   10/30/08 11:52p erickson
* PR48023: refactor scaler functions
*
* Hydra_Software_Devel/4   10/29/08 11:33a erickson
* PR48028: added NEXUS_VideoWindow_SetScalerSettings smoke test
*
* Hydra_Software_Devel/3   10/24/08 1:07p erickson
* PR48249: refactor NEXUS_PictureCtrl_ConfigWindowLumaStatistics
*
* Hydra_Software_Devel/2   10/24/08 12:48p erickson
* PR42679: add new func
*
* Hydra_Software_Devel/1   10/23/08 11:18a erickson
* PR48023: added pq smoke test
*
***************************************************************************/

#include "nexus_platform.h"
#include "nexus_display.h"
#include "nexus_video_input.h"
#if NEXUS_NUM_ANALOG_VIDEO_DECODERS
#include "nexus_composite_input.h"
#include "nexus_svideo_input.h"
#include "nexus_component_input.h"
#include "nexus_pc_input.h"
#include "nexus_analog_video_decoder.h"
#else
#include "nexus_video_decoder.h"
#endif
#include "nexus_picture_ctrl.h"
#include "nexus_video_adj.h"
#if NEXUS_DTV_PLATFORM
#include "nexus_video_window_tnt.h"
#include "nexus_video_window_xvycc.h"
#include "nexus_panel_output.h"
#include "nexus_platform_boardcfg.h"
#include "nexus_custom_video_adjust.h"
#else
#include "nexus_component_output.h"
#endif
#include "nexus_video_window.h"

#include "bstd.h"
#include "bkni.h"
#include <stdio.h>
#include <stdlib.h>

BDBG_MODULE(pq);

void print_color_matrix(const char *name, const NEXUS_ColorMatrix *pColorMatrix)
{
    unsigned i;
    BKNI_Printf("Color matrix: %s, shift %d\n", name, pColorMatrix->shift);
    for (i=0;i<NEXUS_COLOR_MATRIX_COEFF_COUNT;i++) {
        BKNI_Printf("0x%08x%s", pColorMatrix->coeffMatrix[i], (i+1)%5 == 0 ? "\n":", ");
    }
    BKNI_Printf("\n");
}

int main(void)
{
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_PlatformSettings platformSettings;
    NEXUS_DisplayHandle display;
    NEXUS_DisplaySettings displayCfg;
    NEXUS_VideoWindowHandle window;
    NEXUS_VideoInput videoInput;
    NEXUS_PictureCtrlContrastStretch contrastStretch;
    int16_t contrastStretchTable[20];
    NEXUS_Error rc;
    NEXUS_VideoWindowMadSettings madSettings;
#if NEXUS_NUM_ANALOG_VIDEO_DECODERS
    NEXUS_AnalogVideoDecoderHandle analogVideoDecoder;
    NEXUS_AnalogVideoDecoderSettings analogVideoDecoderSettings;
#else
    NEXUS_VideoDecoderHandle videoDecoder;
#endif
    NEXUS_ColorMatrix colorMatrix;
#if NEXUS_DTV_PLATFORM
    NEXUS_VideoWindowAnrSettings anrSettings;
#define DATASIZE 1024
    unsigned char data[DATASIZE];
    unsigned sizeReturned;
    NEXUS_VideoAdjI2PValue i2pValue;
    NEXUS_VideoAdj22PulldownValue vadj22PulldownValue;
    NEXUS_VideoAdj32PulldownValue vadj32PulldownValue;
    NEXUS_AttenuationRgbSettings attenuationRgbSettings;
    NEXUS_PictureCtrlDitherSettings ditherSettings;
#if BCHP_VER == BCHP_VER_A0
    NEXUS_PictureCtrlSharpnessValue sharpnessValue;
#endif
    NEXUS_PictureCtrlLumaSettings lumaSettings;
    NEXUS_LumaStatistics lumaStat;
    NEXUS_PictureCtrlChromaSettings chromaSettings;
    NEXUS_ChromaStatistics chromaStat;
    NEXUS_PictureCtrlColorClipSettings colorClipSettings;
    NEXUS_PictureControlColorCorrectionSettings colorCorrectionSettings;
    uint16_t colorTemp;
    int16_t value;
    NEXUS_SharpnessSettings sharpnessSettings;
    unsigned i;
    bool sharpnessEnable;
    int16_t sharpness;
    NEXUS_VideoWindowScalerSettings scalerSettings;
    NEXUS_VideoWindowNonLinearScalerSettings nonLinearScalerSettings;
#endif
    NEXUS_VideoWindowAfdSettings afdSettings;

    /* Bring up all modules for a platform in a default configuration for this platform */
    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);
    NEXUS_Platform_GetConfiguration(&platformConfig);

#if NEXUS_DTV_PLATFORM
    NEXUS_Display_GetDefaultSettings(&displayCfg);
    display = NEXUS_Display_Open(0, &displayCfg);
    NEXUS_Display_AddOutput(display, NEXUS_PanelOutput_GetConnector(platformConfig.outputs.panel[0]));
    NEXUS_BoardCfg_ConfigurePanel(true, true, true);
#else
    NEXUS_Display_GetDefaultSettings(&displayCfg);
    displayCfg.displayType = NEXUS_DisplayType_eAuto;
#if NEXUS_NUM_COMPONENT_OUTPUTS
    displayCfg.format = NEXUS_VideoFormat_e720p;
    display = NEXUS_Display_Open(0, &displayCfg);
    rc = NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
    BDBG_ASSERT(!rc);
#elif NEXUS_NUM_COMPOSITE_OUTPUTS
    displayCfg.format = NEXUS_VideoFormat_eNtsc;
    display = NEXUS_Display_Open(0, &displayCfg);
    rc = NEXUS_Display_AddOutput(display, NEXUS_CompositeOutput_GetConnector(platformConfig.outputs.composite[0]));
    BDBG_ASSERT(!rc);
#endif	
#endif
    window = NEXUS_VideoWindow_Open(display, 0);

#if NEXUS_NUM_ANALOG_VIDEO_DECODERS
    analogVideoDecoder = NEXUS_AnalogVideoDecoder_Open(0, NULL);
    videoInput = NEXUS_AnalogVideoDecoder_GetConnector(analogVideoDecoder);
    NEXUS_AnalogVideoDecoder_GetDefaultSettingsForVideoInput(NEXUS_CompositeInput_GetConnector(platformConfig.inputs.composite[0]), &analogVideoDecoderSettings);
    rc = NEXUS_AnalogVideoDecoder_SetSettings(analogVideoDecoder, &analogVideoDecoderSettings);
    BDBG_ASSERT(!rc);
#else
    videoDecoder = NEXUS_VideoDecoder_Open(0, NULL);
    videoInput = NEXUS_VideoDecoder_GetConnector(videoDecoder);
#endif
    rc = NEXUS_VideoWindow_AddInput(window, videoInput);
    BDBG_ASSERT(!rc);

    /* step through every pq function to make sure it doesn't fail */

#if NEXUS_DTV_PLATFORM
    /* print some subset of MAD dynamic defaults */
    NEXUS_VideoWindow_GetDynamicDefaultMadSettings(window, &madSettings);
    BKNI_Printf("MAD defaults: %d %d %d %d %d\n",
        madSettings.deinterlace,
        madSettings.enable32Pulldown,
        madSettings.enable22Pulldown,
        madSettings.gameMode,
        madSettings.pixelFormat);
    NEXUS_VideoAdj_GetDynamicDefaultI2PValue(window, &i2pValue);
    BKNI_Printf("MAD I2P defaults: %d %d %d %d %d %d %d\n",
        i2pValue.tmMode,
        i2pValue.smMode,
        i2pValue.enableQmK,
        i2pValue.enableQmL,
        i2pValue.enableQmM,
        i2pValue.enable3548MotionSpacialAlgorithm,
        i2pValue.enable3548MotionTemporalAlgorithm);
    NEXUS_VideoAdj_GetDynamicDefault22PulldownValue(window, &vadj22PulldownValue);
    BKNI_Printf("MAD 22 pulldown defaults: %d %d %d %d\n",
        vadj22PulldownValue.madItFieldMatchRatio,
        vadj22PulldownValue.madItFieldEnterLockLevel,
        vadj22PulldownValue.madItFieldCtrlLockSatLevel,
        vadj22PulldownValue.madBwvCtrlLumaThreshold);
    NEXUS_VideoAdj_GetDynamicDefault32PulldownValue(window, &vadj32PulldownValue);
    BKNI_Printf("MAD 32 pulldown defaults: %d %d %d %d\n",
        vadj32PulldownValue.madItFieldPhaseMatchThresh,
        vadj32PulldownValue.madItFieldEnterLockLevel,
        vadj32PulldownValue.madItFieldCtrlRepfVetoLevel,
        vadj32PulldownValue.madItFieldCtrlRepfPixCorrectLevel,
        vadj32PulldownValue.madItFieldCtrlLockSatLevel,
        vadj32PulldownValue.madItFieldCtrlBadEditLevel,
        vadj32PulldownValue.madBwvCtrlLumaThreshold);
#endif

    BKNI_Printf("MAD\n");
    NEXUS_VideoWindow_GetMadSettings(window, &madSettings);
    madSettings.deinterlace = true;
    rc = NEXUS_VideoWindow_SetMadSettings(window, &madSettings);
    BDBG_ASSERT(!rc);
    madSettings.deinterlace = false;
    rc = NEXUS_VideoWindow_SetMadSettings(window, &madSettings);
    BDBG_ASSERT(!rc);

    BKNI_Printf("AFD\n");
    NEXUS_VideoWindow_GetAfdSettings(window, &afdSettings);
    afdSettings.mode = NEXUS_AfdMode_eStream;
    rc = NEXUS_VideoWindow_SetAfdSettings(window, &afdSettings);
    BDBG_ASSERT(!rc);
    afdSettings.mode = NEXUS_AfdMode_eUser;
    afdSettings.userValue = 0xff;
    rc = NEXUS_VideoWindow_SetAfdSettings(window, &afdSettings);
    BDBG_ASSERT(!rc);
    afdSettings.mode = NEXUS_AfdMode_eDisabled;
    rc = NEXUS_VideoWindow_SetAfdSettings(window, &afdSettings);
    BDBG_ASSERT(!rc);

#if NEXUS_DTV_PLATFORM
    NEXUS_VideoWindow_GetAnrSettings(window, &anrSettings);
    anrSettings.anr.mode = NEXUS_VideoWindowFilterMode_eEnable;
    rc = NEXUS_VideoWindow_SetAnrSettings(window, &anrSettings);
    BDBG_ASSERT(!rc);
    rc = NEXUS_VideoWindow_SetCustomAnrSettings(window, &anrSettings, data, DATASIZE);
    BDBG_ASSERT(!rc);
    rc = NEXUS_VideoWindow_GetCustomAnrData(window, data, DATASIZE, &sizeReturned);
    BDBG_ASSERT(!rc);
    BDBG_ASSERT(sizeReturned == DATASIZE);
    BDBG_WRN(("Expecting failure..."));
    rc = NEXUS_VideoWindow_GetCustomAnrData(window, data, DATASIZE-1, &sizeReturned);
    BDBG_ASSERT(rc); /* should fail */
    BDBG_ASSERT(sizeReturned == DATASIZE-1);
    anrSettings.anr.mode = NEXUS_VideoWindowFilterMode_eDisable;
    rc = NEXUS_VideoWindow_SetAnrSettings(window, &anrSettings);
    BDBG_ASSERT(!rc);
#endif

#if NEXUS_DTV_PLATFORM
    rc = NEXUS_VideoWindow_SetBrightness(window, 0);
    BDBG_ASSERT(!rc);
    NEXUS_VideoWindow_GetBrightness(window, &value);
    BDBG_ASSERT(value == 0);
    rc = NEXUS_VideoWindow_SetBrightness(window, 1000);
    BDBG_ASSERT(!rc);
    NEXUS_VideoWindow_GetBrightness(window, &value);
    BDBG_ASSERT(value == 1000);
    rc = NEXUS_VideoWindow_SetBrightness(window, 0);
    BDBG_ASSERT(!rc);

    rc = NEXUS_VideoWindow_SetHue(window, 0);
    BDBG_ASSERT(!rc);
    NEXUS_VideoWindow_GetHue(window, &value);
    BDBG_ASSERT(value == 0);
    rc = NEXUS_VideoWindow_SetHue(window, 1000);
    BDBG_ASSERT(!rc);
    NEXUS_VideoWindow_GetHue(window, &value);
    BDBG_ASSERT(value == 1000);
    rc = NEXUS_VideoWindow_SetHue(window, 0);
    BDBG_ASSERT(!rc);

    rc = NEXUS_VideoWindow_SetSaturation(window, 0);
    BDBG_ASSERT(!rc);
    NEXUS_VideoWindow_GetSaturation(window, &value);
    BDBG_ASSERT(value == 0);
    rc = NEXUS_VideoWindow_SetSaturation(window, 1000);
    BDBG_ASSERT(!rc);
    NEXUS_VideoWindow_GetSaturation(window, &value);
    BDBG_ASSERT(value == 1000);
    rc = NEXUS_VideoWindow_SetSaturation(window, 0);
    BDBG_ASSERT(!rc);

    rc = NEXUS_VideoWindow_SetContrast(window, 0);
    BDBG_ASSERT(!rc);
    NEXUS_VideoWindow_GetContrast(window, &value);
    BDBG_ASSERT(value == 0);
    rc = NEXUS_VideoWindow_SetContrast(window, 1000);
    BDBG_ASSERT(!rc);
    NEXUS_VideoWindow_GetContrast(window, &value);
    BDBG_ASSERT(value == 1000);
    rc = NEXUS_VideoWindow_SetContrast(window, 0);
    BDBG_ASSERT(!rc);

    NEXUS_VideoWindow_GetAttenuationRgb(window, &attenuationRgbSettings);
    rc = NEXUS_VideoWindow_SetAttenuationRgb(window, &attenuationRgbSettings);
    BDBG_ASSERT(!rc);

    NEXUS_PictureCtrl_GetDitherSettings(window, &ditherSettings);
    rc = NEXUS_PictureCtrl_SetDitherSettings(window, &ditherSettings);
    BDBG_ASSERT(!rc);
#endif

    BKNI_Printf("contrast stretch\n");
    NEXUS_PictureCtrl_GetContrastStretch(window, &contrastStretch);
    rc = NEXUS_PictureCtrl_SetContrastStretch(window, &contrastStretch);
    BDBG_ASSERT(!rc);
    rc = NEXUS_PictureCtrl_SetCustomContrastStretch(window, &contrastStretch, contrastStretchTable, 20);
    BDBG_ASSERT(!rc);

#if NEXUS_DTV_PLATFORM && BCHP_VER == BCHP_VER_A0
    NEXUS_PictureCtrl_GetSharpnessValue(window, &sharpnessValue);
    rc = NEXUS_PictureCtrl_SetSharpnessValue(window, &sharpnessValue);
    BDBG_ASSERT(!rc);
#endif

#if NEXUS_DTV_PLATFORM
    printf("NEXUS_PictureCtrl_GetWindowLumaStatistics\n");
    NEXUS_PictureCtrl_GetDefaultLumaSettings(&lumaSettings);
    NEXUS_PictureCtrl_GetWindowLumaSettings(window, &lumaSettings);
    printf("NEXUS_PictureCtrl_GetWindowLumaSettings %d,%d,%d,%d\n",
        lumaSettings.region.left, lumaSettings.region.right, lumaSettings.region.top, lumaSettings.region.bottom);
    rc = NEXUS_PictureCtrl_ConfigWindowLumaStatistics(window, &lumaSettings);
    BDBG_ASSERT(!rc);
    rc = NEXUS_PictureCtrl_GetWindowLumaStatistics(window, &lumaStat);
    BDBG_ASSERT(!rc);

    NEXUS_PictureCtrl_GetDefaultLumaSettings(&lumaSettings);
    lumaSettings.region.left = 10;
    lumaSettings.region.right = 10;
    lumaSettings.region.top = 10;
    lumaSettings.region.bottom = 10;
    rc = NEXUS_PictureCtrl_ConfigWindowLumaStatistics(window, &lumaSettings);
    BDBG_ASSERT(!rc);
    rc = NEXUS_PictureCtrl_GetWindowLumaStatistics(window, &lumaStat);
    BDBG_ASSERT(!rc);

    printf("NEXUS_PictureCtrl_GetWindowChromaStatistics\n");
    NEXUS_PictureCtrl_GetDefaultChromaSettings(&chromaSettings);
    rc = NEXUS_PictureCtrl_ConfigWindowChromaStatistics(window, &chromaSettings);
    BDBG_ASSERT(!rc);
    rc = NEXUS_PictureCtrl_GetWindowChromaStatistics(window, &chromaStat);
    BDBG_ASSERT(!rc);

    NEXUS_PictureCtrl_GetDefaultChromaSettings(&chromaSettings);
    chromaSettings.region.left = 10;
    chromaSettings.region.right = 10;
    chromaSettings.region.top = 10;
    chromaSettings.region.bottom = 10;
    rc = NEXUS_PictureCtrl_ConfigWindowChromaStatistics(window, &chromaSettings);
    BDBG_ASSERT(!rc);
    rc = NEXUS_PictureCtrl_GetWindowChromaStatistics(window, &chromaStat);
    BDBG_ASSERT(!rc);

    printf("NEXUS_PictureCtrl_SetColorCorrectionTable\n");
    NEXUS_PictureCtrl_GetColorCorrectionTable(display, &colorCorrectionSettings);
    rc = NEXUS_PictureCtrl_SetColorCorrectionTable(display, &colorCorrectionSettings);
    BDBG_ASSERT(!rc);

    printf("NEXUS_PictureCtrl_SetColorClipSettings\n");
    NEXUS_PictureCtrl_GetColorClipSettings(display, &colorClipSettings);
    rc = NEXUS_PictureCtrl_SetColorClipSettings(display, &colorClipSettings);
    BDBG_ASSERT(!rc);

    printf("NEXUS_VideoWindow_SetColorTemp\n");
    NEXUS_VideoWindow_GetColorTemp(window, &colorTemp);
    rc = NEXUS_VideoWindow_SetColorTemp(window, colorTemp);
    BDBG_ASSERT(!rc);

    printf("NEXUS_VideoWindow_SetColorMatrixNonLinearA/B/C\n");
    rc = NEXUS_VideoWindow_GetColorMatrixNonLinearA(window, &colorMatrix);
    BDBG_ASSERT(!rc);
    rc = NEXUS_VideoWindow_SetColorMatrixNonLinearA(window, &colorMatrix);
    BDBG_ASSERT(!rc);
    rc = NEXUS_VideoWindow_SetColorMatrixNonLinearA(window, NULL);
    BDBG_ASSERT(!rc);
    rc = NEXUS_VideoWindow_GetColorMatrixNonLinearA(window, &colorMatrix);
    BDBG_ASSERT(!rc);

    rc = NEXUS_VideoWindow_GetColorMatrixNonLinearB(window, &colorMatrix);
    BDBG_ASSERT(!rc);
    rc = NEXUS_VideoWindow_SetColorMatrixNonLinearB(window, &colorMatrix);
    BDBG_ASSERT(!rc);
    rc = NEXUS_VideoWindow_SetColorMatrixNonLinearB(window, NULL);
    BDBG_ASSERT(!rc);
    rc = NEXUS_VideoWindow_GetColorMatrixNonLinearB(window, &colorMatrix);
    BDBG_ASSERT(!rc);

    rc = NEXUS_VideoWindow_GetColorMatrixNonLinearC(window, &colorMatrix);
    BDBG_ASSERT(!rc);
    rc = NEXUS_VideoWindow_SetColorMatrixNonLinearC(window, &colorMatrix);
    BDBG_ASSERT(!rc);
    rc = NEXUS_VideoWindow_SetColorMatrixNonLinearC(window, NULL);
    BDBG_ASSERT(!rc);
    rc = NEXUS_VideoWindow_GetColorMatrixNonLinearC(window, &colorMatrix);
    BDBG_ASSERT(!rc);

    printf("NEXUS_VideoWindow_SetSharpness\n");
    NEXUS_VideoWindow_GetSharpness(window, &sharpnessEnable, &sharpness);
    rc = NEXUS_VideoWindow_SetSharpness(window, sharpnessEnable, sharpness);
    BDBG_ASSERT(!rc);

    printf("NEXUS_VideoWindow_SetSharpnessConfig\n");
    NEXUS_VideoWindow_GetSharpnessConfig(window, &sharpnessSettings);
    rc = NEXUS_VideoWindow_SetSharpnessConfig(window, &sharpnessSettings);
    BDBG_ASSERT(!rc);

    for (i=0;i<NEXUS_CM3D_MAX_OVERLAPPED_REGIONS;i++) {
        NEXUS_Cm3dOverlappedRegion region;
        NEXUS_VideoWindow_GetCm3dOverlappedRegion(window, &region, i);
#if 0
/* TODO: need valid values */
        rc = NEXUS_VideoWindow_SetCm3dOverlappedRegion(window, &region, i);
        BDBG_ASSERT(!rc);
#endif
    }

    for (i=0;i<NEXUS_CM3D_MAX_REGIONS;i++) {
        NEXUS_Cm3dRegion region;
        NEXUS_VideoWindow_GetCm3dRegion(window, &region, i);
#if 0
/* TODO: need valid values */
        rc = NEXUS_VideoWindow_SetCm3dRegion(window, &region, i);
        BDBG_ASSERT(!rc);
#endif
    }

    printf("NEXUS_VideoInput_GetColorMatrix\n");
    NEXUS_VideoInput_GetColorMatrix(videoInput, &colorMatrix);
    print_color_matrix("VideoInput", &colorMatrix);
    rc = NEXUS_VideoInput_SetColorMatrix(videoInput, &colorMatrix);
    BDBG_ASSERT(!rc);
    rc = NEXUS_VideoInput_SetColorMatrix(videoInput, NULL);
    BDBG_ASSERT(!rc);

    printf("NEXUS_AnalogVideoDecoder_SetColorMatrix\n");
    NEXUS_AnalogVideoDecoder_GetColorMatrix(analogVideoDecoder, &colorMatrix);
    print_color_matrix("AnalogVideoDecoder", &colorMatrix);
    rc = NEXUS_AnalogVideoDecoder_SetColorMatrix(analogVideoDecoder, &colorMatrix);
    BDBG_ASSERT(!rc);
    rc = NEXUS_AnalogVideoDecoder_SetColorMatrix(analogVideoDecoder, NULL);
    BDBG_ASSERT(!rc);

    printf("NEXUS_PanelOutput_SetColorMatrix\n");
    NEXUS_PanelOutput_GetColorMatrix(platformConfig.outputs.panel[0], &colorMatrix);
    rc = NEXUS_PanelOutput_SetColorMatrix(platformConfig.outputs.panel[0], &colorMatrix);
    BDBG_ASSERT(!rc);
    rc = NEXUS_PanelOutput_SetColorMatrix(platformConfig.outputs.panel[0], NULL);
    BDBG_ASSERT(!rc);

    printf("NEXUS_PanelOutput_SetAttenuationRgb\n");
    NEXUS_PanelOutput_GetAttenuationRgb(platformConfig.outputs.panel[0], &attenuationRgbSettings);
    rc = NEXUS_PanelOutput_SetAttenuationRgb(platformConfig.outputs.panel[0], &attenuationRgbSettings);
    BDBG_ASSERT(!rc);

    printf("NEXUS_VideoWindow_SetNonLinearScalerSettings\n");
    NEXUS_VideoWindow_GetNonLinearScalerSettings(window, &nonLinearScalerSettings);
    nonLinearScalerSettings.nonLinearScaling = true;
    nonLinearScalerSettings.nonLinearSourceWidth = 720;
    nonLinearScalerSettings.nonLinearScalerOutWidth = 480;
    rc = NEXUS_VideoWindow_SetNonLinearScalerSettings(window, &nonLinearScalerSettings);
    BDBG_ASSERT(!rc);
    nonLinearScalerSettings.nonLinearScaling = false;
    rc = NEXUS_VideoWindow_SetNonLinearScalerSettings(window, &nonLinearScalerSettings);
    BDBG_ASSERT(!rc);

    printf("NEXUS_VideoWindow_SetScalerSettings\n");
    NEXUS_VideoWindow_GetScalerSettings(window, &scalerSettings);
    rc = NEXUS_VideoWindow_SetScalerSettings(window, &scalerSettings);
    scalerSettings.verticalDejagging = true;
    scalerSettings.horizontalLumaDeringing = true;
    scalerSettings.verticalLumaDeringing = true;
    scalerSettings.horizontalChromaDeringing = true;
    scalerSettings.verticalChromaDeringing = true;
    rc = NEXUS_VideoWindow_SetScalerSettings(window, &scalerSettings);
    BDBG_ASSERT(!rc);
    scalerSettings.verticalDejagging = false;
    scalerSettings.horizontalLumaDeringing = false;
    scalerSettings.verticalLumaDeringing = false;
    scalerSettings.horizontalChromaDeringing = false;
    scalerSettings.verticalChromaDeringing = false;
    rc = NEXUS_VideoWindow_SetScalerSettings(window, &scalerSettings);
    BDBG_ASSERT(!rc);

    printf("Custom VideoAdj settings\n");
    {
    NEXUS_VideoAdjCustomSettings customSettings;

    NEXUS_VideoAdj_GetCustomerSettings( window, NEXUS_VideoAdjCustomSettingsType_eI2PValue, &customSettings);
    rc = NEXUS_VideoAdj_SetCustomerSettings( window, NEXUS_VideoAdjCustomSettingsType_eI2PValue, &customSettings);
    BDBG_ASSERT(!rc);

    NEXUS_VideoAdj_GetCustomerSettings( window, NEXUS_VideoAdjCustomSettingsType_e22PulldownValue, &customSettings);
    rc = NEXUS_VideoAdj_SetCustomerSettings( window, NEXUS_VideoAdjCustomSettingsType_e22PulldownValue, &customSettings);
    BDBG_ASSERT(!rc);

    NEXUS_VideoAdj_GetCustomerSettings( window, NEXUS_VideoAdjCustomSettingsType_e32PulldownValue, &customSettings);
    rc = NEXUS_VideoAdj_SetCustomerSettings( window, NEXUS_VideoAdjCustomSettingsType_e32PulldownValue, &customSettings);
    BDBG_ASSERT(!rc);
    }
#else
    printf("NEXUS_VideoWindow_SetColorMatrix\n");
    NEXUS_VideoWindow_GetColorMatrix(window, &colorMatrix);
    rc = NEXUS_VideoWindow_SetColorMatrix(window, &colorMatrix);
    BDBG_ASSERT(!rc);
    rc = NEXUS_VideoWindow_SetColorMatrix(window, NULL);
    BDBG_ASSERT(!rc);
    NEXUS_VideoWindow_GetColorMatrix(window, &colorMatrix);
#endif

    return 0;
}
