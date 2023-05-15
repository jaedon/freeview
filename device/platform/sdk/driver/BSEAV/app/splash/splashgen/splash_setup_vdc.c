/***************************************************************************
 *     (c)2002-2009 Broadcom Corporation
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
 * $brcm_Workfile: splash_setup_vdc.c $
 * $brcm_Revision: 33 $
 * $brcm_Date: 10/31/12 6:53p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/splash/splashgen/splash_setup_vdc.c $
 * 
 * 33   10/31/12 6:53p mward
 * SW7435-114:  Back out dual surface support.  Move to branch pending CFE
 * support.
 * 
 * 32   10/5/12 6:32p bselva
 * SW7360-38: Add splash support
 * 
 * 31   9/28/12 4:26p mward
 * SW7435-114:  RTS for 7435 in 1u4t mode requires feeding SD and HD
 * displays from separate MEMCs.
 * 
 * 30   9/4/12 4:39p jessem
 * SW7425-3872: NULLed hMem1 for 7425 to reflect reassignment of GFD0 and
 * GFD1 to MEMC0.
 *
 * 29   7/23/12 5:05p mward
 * SW7125-1306:  Fix run on chips without GFD vertical upscale.
 *
 * 28   4/19/12 1:46p mward
 * SW7435-114:  Add support for 7435.
 *
 * 27   4/19/12 1:10p jessem
 * SW7425-2828: Reduced console clutter.
 *
 * 26   4/18/12 3:23p jessem
 * SW7425-2828: Removed extraneous variable.
 *
 * 25   4/18/12 2:47p jessem
 * SW7425-2828: Corrected method on obtaining heap. Added run-time options
 * to set the display format and to disable the 2nd display. Backed-out
 * of creating a separate surface for the the 2nd display.
 *
 * 24   4/9/12 4:43p jessem
 * SW7425-2828: Corrected GFD handle used with 2nd display when obtaining
 * RDC scratch regs.
 *
 * 23   4/9/12 11:25a jessem
 * SW7425-2828: Removed hardcoded RDC scratch registers and use
 * BVDC_Test_Source_GetGfdScratchRegisters() instead. Also, updated the
 * implementation of the splash surface for the 2nd display.
 *
 * 22   12/20/11 4:50p randyjew
 * SW7208-176: Change Hdmi Output Format default to RGB from YCrCb.
 *
 * 21   8/16/11 5:20p jessem
 * SW7425-1122: Merge SW7552-48 changes to main line.
 *
 * 20   8/8/11 11:16p nickh
 * SW7425-878: Add 7425 splash support
 *
 * 19   2/24/11 5:09p gmohile
 * SW7408-239 : Add 7408 splash support
 *
 * 18   4/30/10 4:51p randyjew
 * SW7468-6: Add SPLASH_NUM_XXX_OUTPUTS flags instead of
 * NEXUS_NUM_XXX_OUTPUTS.
 *
 * 17   4/16/10 6:10p nitinb
 * SW7550-284: Do not configure composite 1 and S-Video outputs for 7550
 * splash
 *
 * 16   6/26/09 12:45p shyam
 * PR54436 : Add 1080i 50 Hz support
 *
 * 15   5/15/09 5:06p shyam
 * PR54436 : Add 720p50Hz and other HD format support
 *
 * 14   5/15/09 3:32p shyam
 * PR54436 : Add HD support
 *
 * 13   5/14/09 5:39p shyam
 * PR52592 : Add support for ARGB8888 surfaces
 *
 * 12   4/8/09 4:28p shyam
 * PR52386 : Add support for DTV platforms
 *
 * 4   4/8/09 12:44p shyam
 * PR52386 : Port splash to nexus Base Build system
 *
 * 3   12/24/08 5:03p katrep
 * PR50711: Add splash support for 7405/7335/7325 and settop chips
 *
 * 2   10/13/08 10:04a erickson
 * PR46184: merge
 *
 * PR46184/1   10/10/08 3:49p chengs
 * PR46184: BSUR_Surface_Create() add a new parameter.
 *
 * 1   9/29/08 11:34a erickson
 * PR46184: added splash app
 *
 * 11   11/2/07 12:59p shyam
 * PR 34874 : Add support for Composite 1
 *
 * 10   11/2/07 12:54p shyam
 * PR 34874 : Add support for Composite 1
 *
 * 9   10/19/07 4:09p shyam
 * PR 30741 : Add support for 576p+PAL
 *
 * 8   10/8/07 7:27p shyam
 * PR 30741 : Add support for No HDMI
 *
 * 7   9/7/07 4:43p shyam
 * PR 30741 : Cleaned up LEGACY_VDC code and assert on VDC errors
 *
 * 6   8/9/07 6:18p shyam
 * PR 33658 : Add PAL support
 *
 * 5   8/9/07 3:10p shyam
 * PR 33858 : Got 480p and dual compositor  working on 7403
 *
 * 4   7/31/07 4:01p shyam
 * PR 33658 : Port splash to 97403 platform
 *
 * 3   7/18/07 12:35p shyam
 * PR 30741 : Add support for 480p on HDMI and NTSC on composite
 *
 * 2   6/29/07 2:09p shyam
 * PR 30741 : Added back HDMI
 *
 * 1   5/14/07 6:50p shyam
 * PR 30741 : Add reference support for generic portable splash
 *
 ***************************************************************************/

#include <stdio.h>

/* base modules */
#include "bstd.h"           /* standard types */
#include "berr.h"           /* error code */

/* common utilities */
#include "brdc.h"
#include "bfmt.h"

/* porting interfaces */
#include "bvdc.h"
#include "bvdc_test.h"

#ifndef SPLASH_NOHDM
#include "bhdm_edid.h"
#endif

#if NEXUS_DTV_PLATFORM
/* porting nexus */
#include "nexus_platform_boardcfg.h"
#endif

#include "splash_vdc_rulgen.h"
#include "bsplash_board.h"
#include "nexus_platform_features.h"

BDBG_MODULE("splash_setup_vdc");

#ifdef LEGACY_VDC
#define BVDC_Compositor_Create(hVdc,phCmp,CompId,Settings) \
                            BVDC_Compositor_Create(hVdc,phCmp,CompId)
#define BVDC_Display_Create(hCmp,hDsp, DspId, Settings) \
                            BVDC_Display_Create(hCmp,hDsp)
#define BVDC_Source_Create(hVdc,phGfx,GfxId,Settings) \
                            BVDC_Source_Create(hVdc,phGfx,GfxId)
#define BVDC_Window_Create(hCmp,phGWin,WinId,hGfx,Settings) \
                            BVDC_Window_Create(hCmp,phGWin,hGfx)
#endif

#ifndef SPLASH_NOHDM
/* prototypes */
BERR_Code ActivateHdmi(BVDC_Handle hVDC, BHDM_Handle hHDM, BVDC_Display_Handle hDisplay);
void   DeactivateHdmi(BVDC_Handle hVDC, BHDM_Handle hHDM, BVDC_Display_Handle hDisplay);

#ifndef SPLASH_HDMI_OUTPUTFORMAT_YCrCb
#define SPLASH_HDMI_OUTPUTFORMAT_YCrCb 0  /*RGB default*/
#endif


#endif

#define Constant_Freq_Mode  (1)

/* Adjust control word so we achieve 160Hz
   Fv = W * 27e6 / (2^16 * (Period + 1))
   Let Period = 255 and solve for W ==> W=99 or 0x63 */
#define PWM_PERIOD              (0xff)
#define PWM_DUTY_CYCLE          (70)   /* 70% in percentage */
#define PWM_FREQ                (160)  /* 160 Hz */
#define PWM_CTRL_WORD \
    (((uint32_t)PWM_FREQ * (PWM_PERIOD+1) * 0x10000) / 27000000)

BERR_Code splash_setup_gfxwin2
    ( BVDC_Handle   hVdc,
      ModeHandles  *pState );

/***************************************************************************
 * This function sets up the display with graphics and does ONE
 * ApplyChanges call. After this point, all register writes and RUL
 * updates should be intercepted and dumped.
 */
BERR_Code  splash_vdc_setup(
    BCHP_Handle         hChp,
    BREG_Handle         hReg,
    BINT_Handle         hInt,
    BTMR_Handle         hTmr,
    ModeHandles        *pState
    )
{
    BERR_Code               eErr;
    BVDC_Handle             hVdc;
#ifndef SPLASH_NOHDM
    BHDM_Handle             hHdm;
    BHDM_Settings           HDMSettings;
#endif
    BVDC_Compositor_Handle  hCompositor0;
    BVDC_Display_Handle     hDisplay0;
    BFMT_VideoInfo          stVideoInfo;
    BSUR_Surface_Handle     hSurface;
    BVDC_Source_Handle      hGfxSource;
    BVDC_Window_Handle      hGfxWindow;
#if 0
    BLVD_Settings           stLvdSettings;
    BPWM_ChannelSettings    stPwmChannelsettings;
#endif
#if NEXUS_DTV_PLATFORM
    BVDC_Display_Settings   cfg_display;
#endif
    BAVC_Gfx_Picture pic;

    /* create 565 surface */
    TestError( BSUR_Surface_Create(
#if (BCHP_CHIP == 7420) || ((BCHP_CHIP==7435) && !NEXUS_PLATFORM_7435_BOX_TYPE_1u4t)
		pState->hMem1,
#else
		pState->hMem0,
#endif
        pState->ulGfxWidth, pState->ulGfxHeight,
        0, NULL, SPLASH_SURFACE_PXL_TYPE, NULL, 0, NULL, &hSurface ),
        "ERROR:BSUR_Surface_Create" );

    pState->hSurface = hSurface;

    /* open VDC */
    eErr = BVDC_Open(&pState->hVdc,
                hChp,
                hReg,
#if (BCHP_CHIP == 7420)
                pState->hMem1,
#else
				pState->hMem0,
#endif
                hInt,
                pState->hRdc,
                hTmr,
                NULL);
    hVdc = pState->hVdc;

#ifndef SPLASH_NOHDM
    eErr = BHDM_GetDefaultSettings(&HDMSettings);
#if (BCHP_CHIP != 7360)
  HDMSettings.hTMR = hTmr;
#endif

    eErr = BHDM_Open(&pState->hHdm,
            hChp,
            hReg,
            hInt,
            pState->hRegI2c,
            &HDMSettings);
    hHdm = pState->hHdm;
#endif


    /* Enable PWM and LVDS */
#if 0
    printf("------------------ENABLE_REG_DUMP------------------");
    /* LVDS Display Module */
    BLVD_GetDefaultSettings(&stLvdSettings);
    stLvdSettings.bFifoMasterMode = true;
    /* Different for WXGA vs FHD */
#if 0
    stLvdSettings.eOutputFormat = BLVD_OutputFormat_eDualLvdsMode_Link2_Link1;
#else
    stLvdSettings.eOutputFormat = BLVD_OutputFormat_eSingleLvdsMode_Link1;
#endif
    BLVD_Open(&pState->hLvd,hChp,hReg,hInt,
        &stLvdSettings);
    BLVD_PowerLvdsPort(pState->hLvd, true) ;

    /* PWM Module */
    BPWM_Open(&pState->hPwm, hChp, hReg, NULL);
    BPWM_GetChannelDefaultSettings(pState->hPwm, 0, &stPwmChannelsettings);
    stPwmChannelsettings.openDrainb = false;
    stPwmChannelsettings.FreqMode   = Constant_Freq_Mode;
    BPWM_OpenChannel(pState->hPwm, &pState->hPwmChn, 0, &stPwmChannelsettings);
    BPWM_SetControlWord(pState->hPwmChn, PWM_CTRL_WORD);
    BPWM_SetPeriodInterval(pState->hPwmChn, PWM_PERIOD);
    BPWM_SetOnInterval(pState->hPwmChn, PWM_DUTY_CYCLE*PWM_PERIOD/100);
    BPWM_Start(pState->hPwmChn);

    /* Config Panel MISC control*/
    NEXUS_BoardCfg_ConfigurePanel(true, true, true);
#endif

#ifndef SPLASH_NODISP_MAIN
    /***************************
     * Display configuration 0
     */

    /* Create a compositor handle from our hVdc handle.  The number of
     * compositors is chip dependent.  Each compositor is responsible for output
     * a video signal or BVB (input timing to vec).  In case we need to do
     * HD/SD, then we'll need the second compositior. */
    TestError( BVDC_Compositor_Create( hVdc, &pState->hCompositor0,
        BVDC_CompositorId_eCompositor0, NULL), "ERROR: BVDC_Compositor_Create" );
    hCompositor0 = pState->hCompositor0;

    /* Create display handle, where the input to the VEC comes from
     * compositor. Note that,
     * only the secondary path can display all 6 Dacs. */
#if NEXUS_DTV_PLATFORM
    BVDC_Display_GetDefaultSettings(BVDC_DisplayId_eDisplay2, &cfg_display);
    cfg_display.eMasterTg = BVDC_DisplayTg_eDviDtg;
    eErr = BVDC_Display_Create(hCompositor0,&pState->hDisplay0,
        BVDC_DisplayId_eDisplay2,
        &cfg_display);
#else
    eErr = BVDC_Display_Create(hCompositor0,&pState->hDisplay0,
        BVDC_DisplayId_eAuto, NULL);
#endif
    if (eErr != BERR_SUCCESS)
    {
        BDBG_ERR(("BVDC_Display_Create failed"));
        goto done;
    }
    hDisplay0 = pState->hDisplay0;

    /* Set display format */
    TestError( BVDC_Display_SetVideoFormat( hDisplay0, pState->eDisplay0Fmt),
        "ERROR: BVDC_Display_SetVideoFormat" );

    /* Set the background color to blue */
#if NEXUS_DTV_PLATFORM
    TestError( BVDC_Compositor_SetBackgroundColor( hCompositor0,
        0x00, 0x00, 0x00 ),
        "ERROR: BVDC_Compositor_SetBackgroundColor" );
#else
    TestError( BVDC_Compositor_SetBackgroundColor( hCompositor0,
        0x00, 0x00, 0x80 ),
        "ERROR: BVDC_Compositor_SetBackgroundColor" );
#endif
#if NEXUS_DTV_PLATFORM
    TestError( BVDC_Display_SetHdmiConfiguration( hDisplay0,
        BVDC_Hdmi_0,BAVC_MatrixCoefficients_eDvi_Full_Range_RGB),
        "ERROR: BVDC_Display_SetVideoFormat" );
#else
    /* set DAC configurations for specific display format */
    /* Dac setup is specified in bsplash_board.h */
    switch (pState->eDisplay0Fmt)
    {
    case BFMT_VideoFmt_eNTSC:
    case BFMT_VideoFmt_eNTSC_J:
    case BFMT_VideoFmt_ePAL_G: /* Heopfully this is what Jason wants me to do */
    case BFMT_VideoFmt_ePAL_H:
    case BFMT_VideoFmt_ePAL_I:
    case BFMT_VideoFmt_ePAL_K:
        TestError( BVDC_Display_SetDacConfiguration( hDisplay0,
            BRCM_DAC_COMPOSITE_0, BVDC_DacOutput_eComposite),
            "ERROR: BVDC_Display_SetDacConfiguration" );
#if SPLASH_NUM_COMPOSITE_OUTPUTS > 1
        TestError( BVDC_Display_SetDacConfiguration( hDisplay0,
            BRCM_DAC_COMPOSITE_1, BVDC_DacOutput_eComposite),
            "ERROR: BVDC_Display_SetDacConfiguration" );
#endif
#if SPLASH_NUM_SVIDEO_OUTPUTS
        TestError( BVDC_Display_SetDacConfiguration( hDisplay0,
            BRCM_DAC_SVIDEO_CHROMA, BVDC_DacOutput_eSVideo_Chroma),
            "ERROR: BVDC_Display_SetDacConfiguration" );
        TestError( BVDC_Display_SetDacConfiguration( hDisplay0,
            BRCM_DAC_SVIDEO_LUMA, BVDC_DacOutput_eSVideo_Luma),
            "ERROR: BVDC_Display_SetDacConfiguration" );
#endif

#if SPLASH_NUM_COMPONENT_OUTPUTS
        TestError( BVDC_Display_SetDacConfiguration( hDisplay0,
            BRCM_DAC_PR, BVDC_DacOutput_ePr),
            "ERROR: BVDC_Display_SetDacConfiguration" );
        TestError( BVDC_Display_SetDacConfiguration( hDisplay0,
            BRCM_DAC_Y, BVDC_DacOutput_eY),
            "ERROR: BVDC_Display_SetDacConfiguration" );
        TestError( BVDC_Display_SetDacConfiguration( hDisplay0,
            BRCM_DAC_PB, BVDC_DacOutput_ePb),
            "ERROR: BVDC_Display_SetDacConfiguration" );
#endif
        break;

    case BFMT_VideoFmt_e576p_50Hz:
    case BFMT_VideoFmt_e480p:
    case BFMT_VideoFmt_e720p:
    case BFMT_VideoFmt_e720p_50Hz :
    case BFMT_VideoFmt_e1080i:
    case BFMT_VideoFmt_e1080i_50Hz:
    case BFMT_VideoFmt_e1080p_50Hz :
#if SPLASH_NUM_COMPONENT_OUTPUTS
        /* DAC3/4/5 = Pr/Y/Pb */
        TestError( BVDC_Display_SetDacConfiguration( hDisplay0,
            BRCM_DAC_PR, BVDC_DacOutput_ePr),
            "ERROR: BVDC_Display_SetDacConfiguration" );
        TestError( BVDC_Display_SetDacConfiguration( hDisplay0,
            BRCM_DAC_Y, BVDC_DacOutput_eY),
            "ERROR: BVDC_Display_SetDacConfiguration" );
        TestError( BVDC_Display_SetDacConfiguration( hDisplay0,
            BRCM_DAC_PB, BVDC_DacOutput_ePb),
            "ERROR: BVDC_Display_SetDacConfiguration" );
#endif
        break;
    default:
        break;
    }
#endif

    /* determine size of display */
    TestError( BFMT_GetVideoFormatInfo( pState->eDisplay0Fmt,
        &stVideoInfo ), "ERROR:BFMT_GetVideoFormatInfo" );

    /***************************
     * Graphics source
     */

    /* create a graphics source handle */
    TestError( BVDC_Source_Create( hVdc, &pState->hGfxSource0,
        BAVC_SourceId_eGfx0, NULL), "ERROR: BVDC_Source_Create" );
    hGfxSource = pState->hGfxSource0;

	/* obtain RDC scratch registers */
	BVDC_Test_Source_GetGfdScratchRegisters(pState->hGfxSource0,
		&pState->ulDisp0GfdScrReg0, &pState->ulDisp0GfdScrReg1);

    /* specify the source surface */
    BKNI_Memset(&pic, 0, sizeof(pic));
    pic.hSurface = hSurface;
    TestError( BVDC_Source_SetSurface( hGfxSource, &pic ),
        "ERROR: BVDC_Source_SetSurface" );

    /* create a window handle */
    TestError( BVDC_Window_Create( hCompositor0,
        &pState->hGfxWindow0, BVDC_WindowId_eAuto,
        pState->hGfxSource0, NULL ), "ERROR:BVDC_Window_Create" );
    hGfxWindow = pState->hGfxWindow0;

    BDBG_MSG(("set destination size to match display stVideoInfo(%d,%d) %d,%d %d,%d",
        stVideoInfo.ulWidth, stVideoInfo.ulHeight,
        (stVideoInfo.ulWidth - pState->ulGfxWidth)/2,
        (pState->ulGfxHeight-stVideoInfo.ulHeight)/2,
        pState->ulGfxWidth, pState->ulGfxHeight ));
#if (NEXUS_HAS_GFD_VERTICAL_UPSCALE)
	/* set destination size to match display */
    TestError( BVDC_Window_SetDstRect( hGfxWindow,
        0,
        0,
        stVideoInfo.ulWidth, stVideoInfo.ulHeight ),
        "ERROR:BVDC_Window_SetDstRect" );
#else
    /* set destination size to match display */
    TestError( BVDC_Window_SetDstRect( hGfxWindow,
        (stVideoInfo.ulWidth - pState->ulGfxWidth)/2,
        (stVideoInfo.ulHeight - pState->ulGfxHeight)/2,
        pState->ulGfxWidth, pState->ulGfxHeight ),
        "ERROR:BVDC_Window_SetDstRect" );
#endif
    /* set the output of the scaler */
    /* TODO: if this line is missing the appropriate defaults are
       not calculated and an assertion failure occurs */
#if (NEXUS_HAS_GFD_VERTICAL_UPSCALE)
    TestError( BVDC_Window_SetScalerOutput( hGfxWindow,
        0, 0, stVideoInfo.ulWidth, stVideoInfo.ulHeight ),
        "ERROR:BVDC_Window_SetScalerOutput" );
#else
    TestError( BVDC_Window_SetScalerOutput( hGfxWindow,
        0, 0, pState->ulGfxWidth, pState->ulGfxHeight ),
        "ERROR:BVDC_Window_SetScalerOutput" );
#endif
    /* set order to front */
    TestError( BVDC_Window_SetZOrder( hGfxWindow, 1),
        "ERROR:BVDC_Window_SetZOrder" );

    /* enable visibility */
    TestError( BVDC_Window_SetVisibility( hGfxWindow, true),
        "ERROR:BVDC_Window_SetVisibility" );

#endif

    if(pState->bNoDisplay1 == false)
    {
        splash_setup_gfxwin2(hVdc, pState) ;
    }

    /***************************
     * Apply Changes
     */
    TestError( BVDC_ApplyChanges(hVdc), "ERROR:BVDC_ApplyChanges" );

#ifndef SPLASH_NOHDM
    /***************************
     * Add HDMI
     */

    eErr = ActivateHdmi(hVdc, hHdm, hDisplay0);
    if( eErr != BERR_SUCCESS)
        BDBG_ERR(("Error ActivateHDMI\n"));
#endif

#if 0 /* Example comment out shyam */
    /* apply changes */
    TestError( BVDC_ApplyChanges(hVdc),
        "ERROR:BVDC_ApplyChanges" );
#endif

done:
    /* return status */
    BDBG_ASSERT(!eErr) ;
    return eErr;
}

BERR_Code splash_setup_gfxwin2
	( BVDC_Handle   hVdc,
      ModeHandles  *pState )
{
    BERR_Code               eErr;
#if NEXUS_DTV_PLATFORM
    BSTD_UNUSED(hVdc);
    BSTD_UNUSED(pState);
#else
    BFMT_VideoInfo          stVideoInfo;
    uint32_t xStart, xEnd, yStart, yEnd ;
    BAVC_Gfx_Picture pic;

    BDBG_MSG(("Create Compositor 2"));
    /* Create a compositor handle from our hVdc handle.  The number of
     * compositors is chip dependent.  Each compositor is responsible for output
     * a video signal or BVB (input timing to vec).  In case we need to do
     * HD/SD, then we'll need the second compositior. */
    TestError( BVDC_Compositor_Create( hVdc, &pState->hCompositor1,
        BVDC_CompositorId_eCompositor1, NULL),
        "ERROR: BVDC_Compositor_Create" );

    BDBG_MSG(("Create Display 2"));
    /* Create display handle, where the input to the VEC comes from
     * compositor. Note that,
     * only the secondary path can display all 6 Dacs. */
    TestError( BVDC_Display_Create( pState->hCompositor1, &pState->hDisplay1,
        BVDC_DisplayId_eAuto, NULL),
        "ERROR: BVDC_Display_Create" );

    BDBG_MSG(("Set display format"));
    /* Set display format */
    TestError( BVDC_Display_SetVideoFormat( pState->hDisplay1, pState->eDisplay1Fmt),
        "ERROR: BVDC_Display_SetVideoFormat" );

    BDBG_MSG(("Set the background color to blue"));
    /* Set the background color to blue */
    TestError( BVDC_Compositor_SetBackgroundColor( pState->hCompositor1,
        0x00, 0x00, 0x80 ),
        "ERROR: BVDC_Compositor_SetBackgroundColor" );

    switch (pState->eDisplay1Fmt)
    {
    case BFMT_VideoFmt_eNTSC:
    case BFMT_VideoFmt_eNTSC_J:
    case BFMT_VideoFmt_ePAL_G: /* Heopfully this is what Jason wants me to do */
    case BFMT_VideoFmt_ePAL_H:
    case BFMT_VideoFmt_ePAL_I:
    case BFMT_VideoFmt_ePAL_K:
        BDBG_MSG(("Set DACs"));
        TestError( BVDC_Display_SetDacConfiguration( pState->hDisplay1,
            BRCM_DAC_COMPOSITE_0, BVDC_DacOutput_eComposite),
            "ERROR: BVDC_Display_SetDacConfiguration" );
#if SPLASH_NUM_COMPOSITE_OUTPUTS > 1
        TestError( BVDC_Display_SetDacConfiguration( pState->hDisplay1,
            BRCM_DAC_COMPOSITE_1, BVDC_DacOutput_eComposite),
            "ERROR: BVDC_Display_SetDacConfiguration" );
#endif

#if SPLASH_NUM_SVIDEO_OUTPUTS
        TestError( BVDC_Display_SetDacConfiguration( pState->hDisplay1,
            BRCM_DAC_SVIDEO_CHROMA, BVDC_DacOutput_eSVideo_Chroma),
            "ERROR: BVDC_Display_SetDacConfiguration" );
        TestError( BVDC_Display_SetDacConfiguration( pState->hDisplay1,
            BRCM_DAC_SVIDEO_LUMA, BVDC_DacOutput_eSVideo_Luma),
            "ERROR: BVDC_Display_SetDacConfiguration" );
#endif
        break;
    default:
        BDBG_ERR(("Bad format value for display 2"));
        break;
    }

    /* determine size of display */
    TestError( BFMT_GetVideoFormatInfo( pState->eDisplay1Fmt,
        &stVideoInfo ),
        "ERROR:BFMT_GetVideoFormatInfo" );

    /***************************
     * Graphics source
     */

    BDBG_MSG(("create a graphics source handle"));
    /* create a graphics source handle */
    TestError( BVDC_Source_Create( hVdc, &pState->hGfxSource1,
        BAVC_SourceId_eGfx1, NULL),
        "ERROR: BVDC_Source_Create" );

	/* obtain RDC scratch registers */
	BVDC_Test_Source_GetGfdScratchRegisters(pState->hGfxSource1,
		&pState->ulDisp1GfdScrReg0, &pState->ulDisp1GfdScrReg1);

    /* specify the source surface */
    BKNI_Memset(&pic, 0, sizeof(pic));
    pic.hSurface = pState->hSurface;
    TestError( BVDC_Source_SetSurface( pState->hGfxSource1, &pic ),
        "ERROR: BVDC_Source_SetSurface" );

    BDBG_MSG(("create a window handle "));
    /* create a window handle */
    TestError( BVDC_Window_Create( pState->hCompositor1,
        &pState->hGfxWindow1, BVDC_WindowId_eAuto,
        pState->hGfxSource1, NULL ),
        "ERROR:BVDC_Window_Create" );

    BDBG_MSG(("pState->ulGfxWidth = %d, pState->ulGfxHeight = %d,"
		" stVideoInfo.ulWidth = %d, stVideoInfo.ulHeight = %d",
		pState->ulGfxWidth, pState->ulGfxHeight, stVideoInfo.ulWidth, stVideoInfo.ulHeight));
    xStart = (pState->ulGfxWidth >= stVideoInfo.ulWidth) ? (pState->ulGfxWidth - stVideoInfo.ulWidth)/2 : (stVideoInfo.ulWidth-pState->ulGfxWidth)/2 ;
    xEnd = (pState->ulGfxWidth >= stVideoInfo.ulWidth) ? stVideoInfo.ulWidth : pState->ulGfxWidth ;
    yStart = (pState->ulGfxHeight >= stVideoInfo.ulHeight) ? (pState->ulGfxHeight - stVideoInfo.ulHeight)/2 : (stVideoInfo.ulHeight-pState->ulGfxHeight)/2 ;
    yEnd = (pState->ulGfxHeight >= stVideoInfo.ulHeight) ? stVideoInfo.ulHeight : pState->ulGfxHeight ;
    BDBG_MSG(("Set Dest Rect Params  %d,%d %d,%d", xStart, yStart, xEnd, yEnd));

    /* set destination size to match display */
    TestError( BVDC_Window_SetDstRect( pState->hGfxWindow1,
        xStart, yStart, xEnd, yEnd ),
        "ERROR:BVDC_Window_SetDstRect" );

    BDBG_MSG(("Set Scalar output"));
    TestError( BVDC_Window_SetScalerOutput( pState->hGfxWindow1,
        0, 0, xEnd, yEnd ),
        "ERROR:BVDC_Window_SetScalerOutput" );

    BDBG_MSG(("Set ZOrder"));
    /* set order to front */
    TestError( BVDC_Window_SetZOrder( pState->hGfxWindow1, 1),
        "ERROR:BVDC_Window_SetZOrder" );

    BDBG_MSG(("Set Visibility"));
    /* enable visibility */
    TestError( BVDC_Window_SetVisibility( pState->hGfxWindow1, true),
        "ERROR:BVDC_Window_SetVisibility" );

    BDBG_MSG(("Second Gfx window opened."));
done:
#endif
    /* return status */
    return eErr;
}


BERR_Code  close_mode(
    ModeHandles            *pState
    )
{
    BERR_Code  eErr = BERR_SUCCESS;

	BSUR_Surface_Destroy(pState->hSurface) ;
#ifndef SPLASH_NOHDM
	BHDM_Close(pState->hHdm) ;
#endif

#ifndef SPLASH_NODISP_MAIN
    TestError( BVDC_Window_Destroy(pState->hGfxWindow0),
            "BVDC_Window_Destroy");

    TestError( BVDC_ApplyChanges(pState->hVdc),
            "BVDC_ApplyChanges");

    TestError( BVDC_Source_Destroy(pState->hGfxSource0),
            "BVDC_Source_Destroy");

    TestError( BVDC_Display_Destroy(pState->hDisplay0),
            "BVDC_Display_Destroy");

    TestError( BVDC_Compositor_Destroy(pState->hCompositor0),
            "BVDC_Compositor_Destroy");

    TestError( BVDC_ApplyChanges(pState->hVdc),
            "BVDC_ApplyChanges");
#endif

    if(!pState->bNoDisplay1)
    {
        TestError( BVDC_Window_Destroy(pState->hGfxWindow1),
                "BVDC_Window_Destroy");

        TestError( BVDC_ApplyChanges(pState->hVdc),
                "BVDC_ApplyChanges");

        TestError( BVDC_Source_Destroy(pState->hGfxSource1),
                "BVDC_Source_Destroy");

        TestError( BVDC_Display_Destroy(pState->hDisplay1),
                "BVDC_Display_Destroy");

        TestError( BVDC_Compositor_Destroy(pState->hCompositor1),
                "BVDC_Compositor_Destroy");

        TestError( BVDC_ApplyChanges(pState->hVdc),
                "BVDC_ApplyChanges");
    }

    TestError( BVDC_Close(pState->hVdc),
            "BVDC_Close");

done:
    return eErr;
}



#ifndef SPLASH_NOHDM
BERR_Code ActivateHdmi(BVDC_Handle hVDC, BHDM_Handle hHDM, BVDC_Display_Handle hDisplay)
{
    BERR_Code  eErr = BERR_SUCCESS;
    BFMT_VideoFmt           vidfmt;
    BHDM_Settings           hdmiSettings;
    const BFMT_VideoInfo*   vidinfo;
    bool                    hasHdmiSupport;

    BHDM_EDID_RxVendorSpecificDB    vsdb;

    /* Get Current Display format */
    TestError(BVDC_Display_GetVideoFormat(hDisplay, &vidfmt),
        "BVDC_Display_GetVideoFormat") ;

    /* Get video info */
    vidinfo = BFMT_GetVideoFormatInfoPtr(vidfmt);

    /* Get Current Settings */
    TestError(BHDM_GetHdmiSettings(hHDM, &hdmiSettings),
        "BHDM_GetHdmiSettings");

    /* Set the video format */
    hdmiSettings.eInputVideoFmt = vidfmt;

    if (hdmiSettings.eOutputPort == BHDM_OutputPort_eHDMI)
    {
        BHDM_EDID_IsRxDeviceHdmi(hHDM, &vsdb, &hasHdmiSupport);

        if (SPLASH_HDMI_OUTPUTFORMAT_YCrCb /*hasHdmiSupport*/)
        {
            hdmiSettings.eOutputFormat = BHDM_OutputFormat_eHDMIMode;
            hdmiSettings.eAspectRatio = vidinfo->eAspectRatio;

            /* Audio settings (for later) */
            /*
                set hdmi audio()
                BAUD_GetClockSamplingRate(GetBAUD(), ??, &sampleRate);
                hdmiSettings.eAudioSamplingRate = sampleRate;
                hdmiSettings.AudioBits = BAVC_AudioBits_e16;
            */
        }
        else
        {
            /* Configure for DVI mode */
            hdmiSettings.eOutputFormat = BHDM_OutputFormat_eDVIMode;
        }
    }

    TestError(BHDM_GetColorimetry(hdmiSettings.eOutputFormat, vidfmt, &hdmiSettings.eColorimetry),
        "BHDM_GetColorimetry");

    TestError(BVDC_Display_SetHdmiConfiguration(hDisplay, BVDC_Hdmi_0, hdmiSettings.eColorimetry),
        "BVDC_Display_SetHdmiConfiguration");

    TestError(BVDC_ApplyChanges(hVDC),
        "BVDC_ApplyChanges");

    TestError(BHDM_EnableDisplay(hHDM, &hdmiSettings),
        "BHDM_EnableDisplay");

    TestError(BVDC_Display_InstallCallback(hDisplay,
                    (BVDC_CallbackFunc_isr)BHDM_AudioVideoRateChangeCB_isr,
                    hHDM,
                    BHDM_Callback_Type_eVideoChange),
        "BVDC_Display_InstallCallback");

#if 0 /* Example comment out shyam */
    TestError(BVDC_ApplyChanges(hVDC),
        "BVDC_ApplyChanges");
#endif

done:
    return eErr;
}

/* Activate HDMI to match the current video format */
void DeactivateHdmi(BVDC_Handle hVDC, BHDM_Handle hHDM, BVDC_Display_Handle hDisplay)
{
    /*
        uninstall audio callback (later)
    */

    BVDC_Display_InstallCallback(hDisplay, NULL, NULL, 0) ;

    (void) BVDC_ApplyChanges(hVDC);

    (void) BHDM_DisableDisplay(hHDM);

}
#endif

/* End of File */
