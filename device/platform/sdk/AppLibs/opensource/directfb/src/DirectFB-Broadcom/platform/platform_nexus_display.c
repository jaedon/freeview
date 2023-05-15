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
 * $brcm_Workfile: platform_nexus_display.c $
 * $brcm_Revision: DirectFB_1_4_15_Port/13 $
 * $brcm_Date: 3/7/12 5:26p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /AppLibs/opensource/directfb/src/broadcom_files/private/DirectFB/1.4.15/platform/platform_nexus_display.c $
 * 
 * DirectFB_1_4_15_Port/13   3/7/12 5:26p christ
 * SW7231-699: DirectFB-1.4.15: Clean up Broadcom code tree
 * 
 * DirectFB_1_4_15_Port/12   3/1/12 1:06p christ
 * SW7552-208: DirectFB: CVBS can't output 576i due to DFB identify
 *  vertical scaling incorrectly on 7552 platform.
 *
 * DirectFB_1_4_15_Port/11   2/29/12 7:11p christ
 * SW7420-2270: DirectFB-1.4.15: Cleanup code prior to release
 *
 * DirectFB_1_4_15_Port/10   2/29/12 3:37p apandav
 * SW7552-208: DirectFB: CVBS can't output 576i due to DFB identify
 *  vertical scaling incorrectly on 7552 platform.
 *
 * DirectFB_1_4_15_Port/9   2/13/12 3:46p mnelis
 * SW7552-208: DirectFB: CVBS can't output 576i due to DFB identify
 *  vertical scaling incorrectly on 7552 platform.
 *
 * DirectFB_1_4_15_Port/8   2/9/12 2:33p christ
 * SW7425-1550: DirectFB: Require integration of Nexus Surface Compositor
 *  into DFB
 *
 * DirectFB_1_4_15_Port/DirectFB-XS_1_4_15_Port/1   12/8/11 3:02p robertwm
 * SW7425-1550: DirectFB: Require integration of Nexus Surface Compositor
 *  into DFB.
 *
 * DirectFB_1_4_15_Port/7   12/15/11 12:52p apandav
 * SW7468-398: DirectFB: Could not build DirectFB 1.4.15 on BCM97468
 *  platform
 *
 * DirectFB_1_4_15_Port/6   12/7/11 2:13p robertwm
 * SW7125-1167: DirectFB-1.4.15: Cannot compile platform code if 3DTV
 *  support is not available.
 *
 * DirectFB_1_4_15_Port/5   11/25/11 2:42p robertwm
 * SW7125-1167: DirectFB-1.4.15: Cannot compile platform code if 3DTV
 *  support is not available.
 *
 * DirectFB_1_4_15_Port/4   11/22/11 4:59p robertwm
 * SW7425-1550: DirectFB: Require integration of Nexus Surface Compositor
 *  into DFB.
 *
 * DirectFB_1_4_15_Port/3   11/11/11 4:19p christ
 * SW7425-1738: DirectFB: Black screen of death when running in release
 *  mode
 *
 * DirectFB_1_4_15_Port/2   11/11/11 2:50p christ
 * SW7425-1737: DirectFB: Vertical scaler not initialised correctly
 *
 * DirectFB_1_4_15_Port/1   11/8/11 3:35p christ
 * SW7425-1703: DirectFB: Move NEXUS_Display calls into platform layer
 *
 *****************************************************************************/
#include "platform_display.h"

#include <stdlib.h>

#include "bkni.h"
#include "bkni_multi.h"

#include "bchp_common.h"
#include "bchp_gfd_0.h"

#ifdef BCHP_GFD_1_REG_START
    #include "bchp_gfd_1.h"
#endif

BDBG_MODULE(platform_nexus_display);

/**
Helper Macros
**/

// Copy rectangle contents from src to dest
#define SET_RECT(dest,src) { (dest).x = (src).x;          \
                             (dest).y = (src).y;          \
                             (dest).width = (src).width;  \
                             (dest).height = (src).height; }

// Create 32bit ARGB value from 4 components
#define PIXEL_ARGB(a,r,g,b)    ( ((a) << 24) | \
                                 ((r) << 16) | \
                                 ((g) << 8)  | \
                                 (b) )

// Split 32bit colour into 4 components
#define ARGB_COLOUR(colour,a,r,g,b) { (a) = ((colour) >> 24) & 0xff;    \
                                      (r) = ((colour) >> 16) & 0xff;    \
                                      (g) = ((colour) >>  8) & 0xff;    \
                                      (b) = ((colour)      ) & 0xff; }


DFB_P_DisplaySettings *dfb_p_displaysettings;

void
DFB_Platform_P_DFBFormatToDisplaySize( DFB_PlatformVideoFormat format,
                                       DFB_PlatformDimension  *pSize )
{
    NEXUS_VideoFormatInfo info;

    BDBG_ASSERT(pSize);

    if (format < DFB_PlatformVideoFormat_eMax)
    {
        NEXUS_VideoFormat_GetInfo(DFB_Platform_P_DFBToNexusVideoFormat(format), &info);
        pSize->w = info.width;
        pSize->h = info.height;
    }
}

NEXUS_VideoOutput
DFB_Platform_P_DisplayOutputToVideoOutput( DFB_PlatformNexusHandle       outputHandle,
                                           DFB_PlatformDisplayOutputType outputType )
{
    NEXUS_VideoOutput nexusOutput;

    BDBG_ASSERT(outputHandle);

    switch (outputType)
    {
#if NEXUS_NUM_HDMI_OUTPUTS
        case DFB_PlatformDisplayOutputType_eHdmi:
            nexusOutput = NEXUS_HdmiOutput_GetVideoConnector(outputHandle);
            break;
#endif

#if NEXUS_NUM_COMPONENT_OUTPUTS
        case DFB_PlatformDisplayOutputType_eComponent:
            nexusOutput = NEXUS_ComponentOutput_GetConnector(outputHandle);
            break;
#endif

#if NEXUS_NUM_COMPOSITE_OUTPUTS
        case DFB_PlatformDisplayOutputType_eComposite:
            nexusOutput = NEXUS_CompositeOutput_GetConnector(outputHandle);
            break;
#endif

#if NEXUS_NUM_SVIDEO_OUTPUTS
        case DFB_PlatformDisplayOutputType_eSvideo:
            nexusOutput = NEXUS_SvideoOutput_GetConnector(outputHandle);
            break;
#endif

#if NEXUS_NUM_SCART_OUTPUTS
        case DFB_PlatformDisplayOutputType_eScart:
            nexusOutput = NEXUS_ScartInput_GetVideoOutputConnector(outputHandle);
            break;
#endif

#if NEXUS_NUM_RFM_OUTPUTS
        case DFB_PlatformDisplayOutputType_eRfm:
            nexusOutput = NEXUS_Rfm_GetVideoConnector(outputHandle);
            break;
#endif

        default:
            BDBG_ERR(("%s: Invalid output type %d!\n", __FUNCTION__, outputType));
            nexusOutput = NULL;
    }
    return nexusOutput;
}

bool
DFB_Platform_P_CheckGfxVScl( int displayIndex )
{
    DFB_PlatformStatus platformStatus;
    DFB_PlatformResult res;
    bool ret = false;

    res = DFB_Platform_P_GetStatus(&platformStatus);

    if (platformStatus.isMaster)
    {
        if(0 == displayIndex)
        {
#ifdef BCHP_GFD_0_HW_CONFIGURATION /* use HW_CONFIGURATION to detect the feature */
            uint32_t value;
            NEXUS_Platform_ReadRegister(BCHP_GFD_0_HW_CONFIGURATION, &value);

            if ((value & BCHP_GFD_0_HW_CONFIGURATION_VSCL_MASK) >> BCHP_GFD_0_HW_CONFIGURATION_VSCL_SHIFT)
                    ret = true;
#else

#if (defined(BCMNEXUS_GFD0_HAS_VERTICAL_SCALER))
            ret = true;
#endif
#endif/*BCHP_GFD_0_HW_CONFIGURATION*/
        }
        else /*disp Index  != 0*/
        {
#ifdef BCHP_GFD_1_HW_CONFIGURATION /* use HW_CONFIGURATION to detect the feature */

/* Ugly work around for some chips without proper RDB register code generation (7552).
 * This assumes that the GFD0 and GFD1 config registers have the same layout
 * which is bound to bit us in the rear at some point! */
#ifndef BCHP_GFD_1_HW_CONFIGURATION_VSCL_MASK
#define BCHP_GFD_1_HW_CONFIGURATION_VSCL_MASK BCHP_GFD_0_HW_CONFIGURATION_VSCL_MASK
#endif
#ifndef BCHP_GFD_1_HW_CONFIGURATION_VSCL_SHIFT
#define BCHP_GFD_1_HW_CONFIGURATION_VSCL_SHIFT BCHP_GFD_0_HW_CONFIGURATION_VSCL_SHIFT
#endif
            uint32_t value;

            NEXUS_Platform_ReadRegister(BCHP_GFD_1_HW_CONFIGURATION, &value);

            if ((value & BCHP_GFD_1_HW_CONFIGURATION_VSCL_MASK) >> BCHP_GFD_1_HW_CONFIGURATION_VSCL_SHIFT)
                    ret = true;
#else
#if (defined(BCMNEXUS_GFD1_HAS_VERTICAL_SCALER))
            ret = true;
#endif
#endif/*BCHP_GFD_1_HW_CONFIGURATION*/
        }

    }

    return ret;
}

DFB_PlatformResult
DFB_Platform_P_SetDisplaySettings( DFB_PlatformNexusHandle         displayHandle,
                                   DFB_Platform_P_DisplaySettings *pSettings )
{
    DFB_PlatformResult             res;
    NEXUS_Error           rc;
    NEXUS_DisplaySettings displaySettings;
    int                   displayIndex;

#if NEXUS_NUM_HDMI_OUTPUTS
    NEXUS_HdmiOutputVendorSpecificInfoFrame iFrame;
    NEXUS_HdmiOutputStatus hdmiStatus;
    NEXUS_VideoFormat hdmiFormat;
    NEXUS_HdmiOutputHandle hdmi;
#endif

    BDBG_MSG_TRACE((">>%s", __FUNCTION__));

    BDBG_ASSERT(pSettings);

    displayIndex = DFB_Platform_P_GetDisplayIndex(displayHandle);
    if (displayIndex < 0)
    {
        res = DFB_PLATFORM_INVARG;
    }
#if (BCMNEXUS_VEC_HAS_1080P_60HZ_SUPPORT == 0)
    else if ((pSettings->format == DFB_PlatformVideoFormat_e1080p60hz) || (pSettings->format == DFB_PlatformVideoFormat_e1080p50hz))
    {
        BDBG_ERR(("%s: Hardware cannot support 1080p50/60Hz output!\n", __FUNCTION__));
        res = DFB_PLATFORM_INVARG;
    }
#endif
    else
    {
        NEXUS_Display_GetSettings(displayHandle, &displaySettings);
        displaySettings.format      = DFB_Platform_P_DFBToNexusVideoFormat(pSettings->format);
        displaySettings.aspectRatio = DFB_Platform_P_DFBToNexusDisplayAspectRatio(pSettings->aspectRatio);
        displaySettings.background  = DFB_Platform_P_DFBToNexusColor(pSettings->background);

        /* vsyncCallback.callback can be NULL to remove the callback so we use -1 to denote that we don't want to
           modify it. */
        if (pSettings->vsyncCallback.callback != (DFB_PlatformCallback)-1)
            memcpy(&displaySettings.vsyncCallback, &pSettings->vsyncCallback, sizeof(pSettings->vsyncCallback));

#if NEXUS_NUM_HDMI_OUTPUTS
        hdmi = DFB_Platform_P_GetDisplayOutputHandle(displayHandle, DFB_PlatformDisplayOutputType_eHdmi, displayIndex);

        if (hdmi) {
            // check that the TV supports the mode we are setting.
            NEXUS_HdmiOutput_GetStatus(hdmi,&hdmiStatus);

            BDBG_MSG_TRACE(("Checking TV supported stereoscopic modes 0x%x - nexus video format %d (format %d)",
                            hdmiStatus.hdmi3DFormatsSupported[DFB_Platform_P_DFBToNexusVideoFormat(pSettings->format)],
                            DFB_Platform_P_DFBToNexusVideoFormat(pSettings->format),
                            pSettings->format));
            NEXUS_HdmiOutput_GetVendorSpecificInfoFrame(hdmi,&iFrame);

            /* Nexus feature / bug. EDID 3D information isn't copied into the 3d version of the display VIC lookup table for 3D modes */
            if (pSettings->format == DFB_PlatformVideoFormat_e720p_3DOU_AS)
                hdmiFormat = NEXUS_VideoFormat_e720p;
            else if (pSettings->format == DFB_PlatformVideoFormat_e720p50hz_3DOU_AS)
                hdmiFormat = NEXUS_VideoFormat_e720p50hz;
            else if (pSettings->format == DFB_PlatformVideoFormat_e720p30hz_3DOU_AS)
                hdmiFormat = NEXUS_VideoFormat_e720p30hz;
            else if (pSettings->format == DFB_PlatformVideoFormat_e720p24hz_3DOU_AS)
                hdmiFormat = NEXUS_VideoFormat_e720p24hz;
            else if (pSettings->format == DFB_PlatformVideoFormat_e1080p24hz_3DOU_AS)
                hdmiFormat = NEXUS_VideoFormat_e1080p24hz;
            else if (pSettings->format == DFB_PlatformVideoFormat_e1080p30hz_3DOU_AS)
                hdmiFormat = NEXUS_VideoFormat_e1080p30hz;
            else
                hdmiFormat = DFB_Platform_P_DFBToNexusVideoFormat(pSettings->format);

            if (hdmiFormat != DFB_Platform_P_DFBToNexusVideoFormat(pSettings->format))
                BDBG_MSG_TRACE(("Checking TV supported stereoscopic modes 0x%x (nexus=%d) (Using 2D VIC) - nexus video format %d (format %d)",
                            hdmiStatus.hdmi3DFormatsSupported[hdmiFormat],hdmiFormat,
                            DFB_Platform_P_DFBToNexusVideoFormat(pSettings->format),
                            pSettings->format));

            switch (pSettings->stereoMode)
            {

                case DFB_PlatformStereoscopicMode_e2d:
                    BDBG_MSG_TRACE(("Trying to set non stereoscopic mode - 2D"));
#if BCMNEXUS_VDC_SUPPORT_3D_VIDEO && BCMNEXUS_DISPLAY_HAS_3DTV_SUPPORT
                    displaySettings.display3DSettings.overrideOrientation = false;
                    displaySettings.display3DSettings.orientation = NEXUS_VideoOrientation_e2D;
#if defined(HUMAX_PLATFORM_BASE)
		    if(displayIndex == 0)
		    	displaySettings.timingGenerator = NEXUS_DisplayTimingGenerator_eHdmiDvo;
		    else
			displaySettings.timingGenerator = NEXUS_DisplayTimingGenerator_eAuto;
#else
                    displaySettings.timingGenerator = NEXUS_DisplayTimingGenerator_eAuto;
#endif
#endif
                    iFrame.hdmiVideoFormat = NEXUS_HdmiVendorSpecificInfoFrame_HDMIVideoFormat_eNone;
                    BDBG_MSG_TRACE(("Using Non stereoscopic mode - 2D"));
                    break;

                case DFB_PlatformStereoscopicMode_eSideBySideHalf:
                    BDBG_MSG_TRACE(("Trying to set stereoscopic mode - Side by Side - Half"));
                    iFrame.hdmiVideoFormat = NEXUS_HdmiVendorSpecificInfoFrame_HDMIVideoFormat_e3DFormat;
                    iFrame.hdmi3DStructure = NEXUS_HdmiVendorSpecificInfoFrame_3DStructure_eSidexSideHalf;
                    if (!(hdmiStatus.hdmi3DFormatsSupported[hdmiFormat] & NEXUS_HdmiOutput_3DStructure_SideBySideHalfHorizontal))
                        return DFB_PLATFORM_UNSUPPORTED;
                    BDBG_MSG_TRACE(("Using stereoscopic mode - Side by Side - Half"));
#if BCMNEXUS_VDC_SUPPORT_3D_VIDEO && BCMNEXUS_DISPLAY_HAS_3DTV_SUPPORT
                    displaySettings.display3DSettings.orientation = NEXUS_VideoOrientation_e3D_LeftRight;
                    displaySettings.display3DSettings.overrideOrientation = true;
#endif
                    break;

                case DFB_PlatformStereoscopicMode_eTopBottom:
                    BDBG_MSG_TRACE(("Trying to set stereoscopic mode - Top Bottom - Half"));
                    if (!(hdmiStatus.hdmi3DFormatsSupported[hdmiFormat] & NEXUS_HdmiOutput_3DStructure_TopAndBottom))
                        return DFB_PLATFORM_UNSUPPORTED;
                    iFrame.hdmiVideoFormat = NEXUS_HdmiVendorSpecificInfoFrame_HDMIVideoFormat_e3DFormat;
                    iFrame.hdmi3DStructure = NEXUS_HdmiVendorSpecificInfoFrame_3DStructure_eTopAndBottom;
                    BDBG_MSG_TRACE(("Using stereoscopic mode - Top Bottom - Half"));
#if BCMNEXUS_VDC_SUPPORT_3D_VIDEO && BCMNEXUS_DISPLAY_HAS_3DTV_SUPPORT
                    displaySettings.display3DSettings.orientation = NEXUS_VideoOrientation_e3D_OverUnder;
                    displaySettings.display3DSettings.overrideOrientation = true;
#endif
                    break;

#if BCMNEXUS_VDC_SUPPORT_3D_VIDEO && BCMNEXUS_DISPLAY_HAS_3DTV_SUPPORT
                case DFB_PlatformStereoscopicMode_eFramePacking:
                    BDBG_MSG_TRACE(("Trying to set stereoscopic mode - Frame packed"));
                    iFrame.hdmiVideoFormat = NEXUS_HdmiVendorSpecificInfoFrame_HDMIVideoFormat_e3DFormat;
                    iFrame.hdmi3DStructure = NEXUS_HdmiVendorSpecificInfoFrame_3DStructure_eFramePacking;
                    if (!(hdmiStatus.hdmi3DFormatsSupported[hdmiFormat] & NEXUS_HdmiOutput_3DStructure_FramePacking))
                        return DFB_PLATFORM_UNSUPPORTED;
                    /* If frame packed we need to make sure we are in the correct 3D mode not 2D version. */
                    /* This relies on the ordering of the Nexus video formats (3D formats are at the end of the enum). */
                    if (pSettings->format < DFB_PlatformVideoFormat_e720p_3DOU_AS)
                    {
                        BDBG_MSG_TRACE(("HDMI 3D Mode and Nexus Display mode not compatible"));
                        return DFB_PLATFORM_INVARG;
                    }

                    BDBG_MSG_TRACE(("Using stereoscopic mode - Frame packed (Updating the display timing generator settings)"));
                    displaySettings.timingGenerator = NEXUS_DisplayTimingGenerator_eHdmiDvo;
                    break;

                case DFB_PlatformStereoscopicMode_eSideBySideFull:
                    BDBG_MSG_TRACE(("Trying to set stereoscopic mode - Side by Side - Full"));
                    iFrame.hdmiVideoFormat = NEXUS_HdmiVendorSpecificInfoFrame_HDMIVideoFormat_e3DFormat;
                    iFrame.hdmi3DStructure = NEXUS_HdmiVendorSpecificInfoFrame_3DStructure_eSidexSideFull;
                    if (!(hdmiStatus.hdmi3DFormatsSupported[hdmiFormat] & NEXUS_HdmiOutput_3DStructure_SideBySideFull))
                        return DFB_PLATFORM_UNSUPPORTED;
                    BDBG_MSG_TRACE(("Using stereoscopic mode - Side by Side - Full"));
                    break;
#endif
                default:
                    BDBG_ERR(("%s() Unknown stereoscopic framing mode %d\n", __FUNCTION__, pSettings->stereoMode));
                    return DFB_PLATFORM_INVARG;
            }
        }
#endif /* NEXUS_NUM_HDMI_OUTPUTS */

        rc = NEXUS_Display_SetSettings(displayHandle, &displaySettings);

        if (rc == NEXUS_SUCCESS)
        {
            memcpy(&dfb_p_displaysettings->display[displayIndex].displaySettings, pSettings, sizeof(*pSettings));
            DFB_Platform_P_DFBFormatToDisplaySize(pSettings->format, &dfb_p_displaysettings->display[displayIndex].displaySize);

            res = DFB_PLATFORM_OK;

#if NEXUS_NUM_HDMI_OUTPUTS
            if (hdmi)
            {
                BDBG_MSG_TRACE(("Now sending HDMI vendor specific info frame for 3D support"));
                rc = NEXUS_HdmiOutput_SetVendorSpecificInfoFrame(hdmi,&iFrame);

                if (rc != NEXUS_SUCCESS)
                {
                    BDBG_ERR(("%s() Failed to send HDMI vendor specific info frame for 3D mode support\n",__FUNCTION__));
                    res = DFB_PLATFORM_FAILURE;
                }
            }
#endif
        }
        else
        {
            BDBG_ERR(("%s() Failed to set Nexus display settings!\n",__FUNCTION__));
            res = DFB_PLATFORM_FAILURE;
        }
    }
    BDBG_MSG_TRACE(("<<%s", __FUNCTION__));
    return res;
}

void
DFB_Platform_P_HDMI_Hotplug_Callback( void *pParam,
                                      int   iParam )
{
#if NEXUS_NUM_HDMI_OUTPUTS
    NEXUS_HdmiOutputStatus         status;
    NEXUS_HdmiOutputHandle         hdmi    = pParam;
    DFB_PlatformNexusHandle        display = (DFB_PlatformNexusHandle)iParam;
#if NEXUS_NUM_COMPONENT_OUTPUTS
    DFB_PlatformNexusHandle        componentHandle;
#endif
    DFB_Platform_P_DisplaySettings displaySettings;
    DFB_PlatformVideoFormat        preferredVideoFormat;

    BDBG_MSG_TRACE((">>%s", __FUNCTION__));

    BDBG_ASSERT(display);
    BDBG_ASSERT(hdmi);

    DFB_Platform_P_GetDisplaySettings(display, &displaySettings);
    NEXUS_HdmiOutput_GetStatus(hdmi, &status);
    BDBG_WRN(("HDMI hotplug event: %s", status.connected?"connected":"not connected"));

    /* the app can choose to switch to the preferred format, but it's not required. */
    if ( status.connected )
    {
        if ( !status.videoFormatSupported[DFB_Platform_P_DFBToNexusVideoFormat(displaySettings.format)] )
        {
            preferredVideoFormat = DFB_Platform_P_NexusToDFBVideoFormat(status.preferredVideoFormat);

            BDBG_WRN(("\nCurrent format not supported by attached monitor. Switching to preferred format %d\n", preferredVideoFormat));
#if NEXUS_NUM_COMPONENT_OUTPUTS
            if (DFB_Platform_P_GetDisplayOutputStatus(DFB_PlatformDisplayOutputType_eComponent, 0) == DFB_PLATFORM_BUSY)
            {
                if (DFB_Platform_P_GetDisplayOutput(display, DFB_PlatformDisplayOutputType_eComponent, 0, &componentHandle) == DFB_PLATFORM_OK)
                {
                    if (componentHandle != NULL && (preferredVideoFormat >= DFB_PlatformVideoFormat_eVesa640x480p60hz))
                    {
                        BDBG_WRN(("Removing Component output as it cannot support this video format.\n"));
                        DFB_Platform_P_RemoveDisplayOutput(display, DFB_PlatformDisplayOutputType_eComponent, 0);
                    }
                }
            }
#endif
            displaySettings.format = preferredVideoFormat;
            DFB_Platform_P_SetDisplaySettings(display, &displaySettings);
        }
    }
#if NEXUS_NUM_COMPONENT_OUTPUTS
    else if (DFB_Platform_P_GetDisplayOutputStatus(DFB_PlatformDisplayOutputType_eComponent, 0) == DFB_PLATFORM_OK)
    {
        if (DFB_Platform_P_GetDisplayOutput(display, DFB_PlatformDisplayOutputType_eComponent, 0, &componentHandle) == DFB_PLATFORM_OK)
        {
            if (componentHandle == NULL && displaySettings.format < DFB_PlatformVideoFormat_eVesa640x480p60hz)
            {
                BDBG_MSG(("Adding Component output as video format is supported.\n"));
                DFB_Platform_P_AddDisplayOutput(display, DFB_PlatformDisplayOutputType_eComponent, 0);
            }
        }
    }
#endif
    BDBG_MSG_TRACE(("<<%s", __FUNCTION__));
#else
    BSTD_UNUSED(pParam);
    BSTD_UNUSED(iParam);
#endif
}

DFB_PlatformResult
DFB_Platform_P_HdmiHotplugHandler( DFB_PlatformNexusHandle displayHandle,
                                   DFB_PlatformNexusHandle hdmiHandle )
{
    DFB_Platform_P_HDMI_Hotplug_Callback(hdmiHandle, (int)displayHandle);
    return DFB_PLATFORM_OK;
}

DFB_PlatformResult
DFB_Platform_P_AddDisplayOutput_Hdmi( DFB_PlatformNexusHandle displayHandle,
                                      int                     outputIndex )
{
    DFB_PlatformResult          dfbRet = DFB_PLATFORM_UNSUPPORTED;
#if NEXUS_NUM_HDMI_OUTPUTS
    DFB_PlatformNexusHandle     outputHandle;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_HdmiOutputSettings    hdmiSettings;

    dfbRet = DFB_PLATFORM_FAILURE;
    if (outputIndex < NEXUS_NUM_HDMI_OUTPUTS)
    {
        outputHandle = DFB_Platform_P_GetDisplayOutputHandle(displayHandle, DFB_PlatformDisplayOutputType_eHdmi, outputIndex);

        /* Check to make sure output is not already connected... */
        if (outputHandle == NULL)
        {
            NEXUS_Platform_GetConfiguration(&platformConfig);

            outputHandle = platformConfig.outputs.hdmi[outputIndex];

            if (NEXUS_Display_AddOutput(displayHandle,
                                        DFB_Platform_P_DisplayOutputToVideoOutput(outputHandle, DFB_PlatformDisplayOutputType_eHdmi)) == NEXUS_SUCCESS)
            {

#if (defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_DIRECTFB_MASTER) && defined(CONFIG_JAPAN_SPEC))
                dfbRet = DFB_Platform_P_SetDisplayOutputHandle( displayHandle, DFB_PlatformDisplayOutputType_eHdmi, outputIndex, outputHandle);

                BDBG_MSG(("%s: Add HDMI%d Display%d Output (res=%d)\n", __FUNCTION__, outputIndex, DFB_Platform_P_GetDisplayIndex(displayHandle), dfbRet));
#else
                /* Install hotplug callback -- video only for now */
                NEXUS_HdmiOutput_GetSettings(outputHandle, &hdmiSettings);
                hdmiSettings.hotplugCallback.callback = DFB_Platform_P_HDMI_Hotplug_Callback;
                hdmiSettings.hotplugCallback.context = outputHandle;
                hdmiSettings.hotplugCallback.param = (int)displayHandle;
                NEXUS_HdmiOutput_SetSettings(outputHandle, &hdmiSettings);

                dfbRet = DFB_Platform_P_SetDisplayOutputHandle( displayHandle, DFB_PlatformDisplayOutputType_eHdmi, outputIndex, outputHandle);

                BDBG_MSG(("%s: Add HDMI%d Display%d Output (res=%d)\n", __FUNCTION__, outputIndex, DFB_Platform_P_GetDisplayIndex(displayHandle), dfbRet));

                /* Force a hotplug to switch to preferred format */
                DFB_Platform_P_HDMI_Hotplug_Callback(outputHandle, (int)displayHandle);
#endif
            }
        }
    }
#else
    BSTD_UNUSED(displayHandle);
    BSTD_UNUSED(outputIndex);
#endif
    return dfbRet;
}

DFB_PlatformResult
DFB_Platform_P_RemoveDisplayOutput_Hdmi( DFB_PlatformNexusHandle displayHandle,
                                         int                     outputIndex )
{
    DFB_PlatformResult      dfbRet = DFB_PLATFORM_UNSUPPORTED;
#if NEXUS_NUM_HDMI_OUTPUTS
    DFB_PlatformNexusHandle outputHandle;

    dfbRet = DFB_PLATFORM_FAILURE;
    if (outputIndex < NEXUS_NUM_HDMI_OUTPUTS)
    {
        outputHandle = DFB_Platform_P_GetDisplayOutputHandle(displayHandle, DFB_PlatformDisplayOutputType_eHdmi, outputIndex);

        /* Ensure that the output is connected... */
        if (outputHandle)
        {
            if (NEXUS_Display_RemoveOutput(displayHandle, DFB_Platform_P_DisplayOutputToVideoOutput(outputHandle, DFB_PlatformDisplayOutputType_eHdmi)) == NEXUS_SUCCESS)
            {
                dfbRet = DFB_Platform_P_SetDisplayOutputHandle( displayHandle, DFB_PlatformDisplayOutputType_eHdmi, outputIndex, NULL);
                BDBG_MSG(("%s: Remove HDMI%d Display%d Output (res=%d)\n", __FUNCTION__,
                           outputIndex, DFB_Platform_P_GetDisplayIndex(displayHandle), dfbRet));
            }
        }
    }
#else
    BSTD_UNUSED(displayHandle);
    BSTD_UNUSED(outputIndex);
#endif
    return dfbRet;
}

DFB_PlatformResult
DFB_Platform_P_AddDisplayOutput_Component( DFB_PlatformNexusHandle displayHandle,
                                           int                     outputIndex )
{
    DFB_PlatformResult          dfbRet = DFB_PLATFORM_UNSUPPORTED;
#if NEXUS_NUM_COMPONENT_OUTPUTS
    DFB_PlatformNexusHandle     outputHandle;
    NEXUS_PlatformConfiguration platformConfig;

    dfbRet = DFB_PLATFORM_FAILURE;
    if (outputIndex < NEXUS_NUM_COMPONENT_OUTPUTS)
    {
        outputHandle = DFB_Platform_P_GetDisplayOutputHandle(displayHandle, DFB_PlatformDisplayOutputType_eComponent, outputIndex);

        /* Check to make sure output is not already connected... */
        if (outputHandle == NULL)
        {
            NEXUS_Platform_GetConfiguration(&platformConfig);

            outputHandle = platformConfig.outputs.component[outputIndex];
            if (NEXUS_Display_AddOutput(displayHandle, DFB_Platform_P_DisplayOutputToVideoOutput(outputHandle, DFB_PlatformDisplayOutputType_eComponent)) == NEXUS_SUCCESS)
            {
                dfbRet = DFB_Platform_P_SetDisplayOutputHandle( displayHandle, DFB_PlatformDisplayOutputType_eComponent, outputIndex, outputHandle);
                BDBG_MSG(("%s: Add Component%d Display%d Output (res=%d)\n", __FUNCTION__, outputIndex, DFB_Platform_P_GetDisplayIndex(displayHandle), dfbRet));
            }
        }
    }
#else
    BSTD_UNUSED(displayHandle);
    BSTD_UNUSED(outputIndex);
#endif
    return dfbRet;
}

DFB_PlatformResult
DFB_Platform_P_RemoveDisplayOutput_Component( DFB_PlatformNexusHandle displayHandle,
                                              int                     outputIndex )
{
    DFB_PlatformResult      dfbRet = DFB_PLATFORM_UNSUPPORTED;
#if NEXUS_NUM_COMPONENT_OUTPUTS
    DFB_PlatformNexusHandle outputHandle;

    dfbRet = DFB_PLATFORM_FAILURE;
    if (outputIndex < NEXUS_NUM_COMPONENT_OUTPUTS)
    {
        outputHandle = DFB_Platform_P_GetDisplayOutputHandle(displayHandle, DFB_PlatformDisplayOutputType_eComponent, outputIndex);

        /* Ensure that the output is connected... */
        if (outputHandle)
        {
            if (NEXUS_Display_RemoveOutput(displayHandle, DFB_Platform_P_DisplayOutputToVideoOutput(outputHandle, DFB_PlatformDisplayOutputType_eComponent)) == NEXUS_SUCCESS)
            {
                dfbRet = DFB_Platform_P_SetDisplayOutputHandle( displayHandle, DFB_PlatformDisplayOutputType_eComponent, outputIndex, NULL);
                BDBG_MSG(("%s: Remove Component%d Display%d Output (res=%d)\n", __FUNCTION__, outputIndex, DFB_Platform_P_GetDisplayIndex(displayHandle), dfbRet));
            }
        }
    }
#else
    BSTD_UNUSED(displayHandle);
    BSTD_UNUSED(outputIndex);
#endif
    return dfbRet;
}

DFB_PlatformResult
DFB_Platform_P_AddDisplayOutput_Composite( DFB_PlatformNexusHandle displayHandle,
                                           int                     outputIndex )
{
    DFB_PlatformResult          dfbRet = DFB_PLATFORM_UNSUPPORTED;
#if NEXUS_NUM_COMPOSITE_OUTPUTS
    DFB_PlatformNexusHandle     outputHandle;
    NEXUS_PlatformConfiguration platformConfig;

    dfbRet = DFB_PLATFORM_FAILURE;

    if (outputIndex < NEXUS_NUM_COMPOSITE_OUTPUTS)
    {
        outputHandle = DFB_Platform_P_GetDisplayOutputHandle(displayHandle, DFB_PlatformDisplayOutputType_eComposite, outputIndex);

        /* Check to make sure output is not already connected... */
        if (outputHandle == NULL)
        {
            NEXUS_Platform_GetConfiguration(&platformConfig);
            outputHandle = platformConfig.outputs.composite[outputIndex];
            if (NEXUS_Display_AddOutput(displayHandle, DFB_Platform_P_DisplayOutputToVideoOutput(outputHandle, DFB_PlatformDisplayOutputType_eComposite)) == NEXUS_SUCCESS)
            {
                dfbRet = DFB_Platform_P_SetDisplayOutputHandle( displayHandle, DFB_PlatformDisplayOutputType_eComposite, outputIndex, outputHandle);
                BDBG_MSG(("%s: Add Composite%d Display%d Output (res=%d)\n", __FUNCTION__, outputIndex, DFB_Platform_P_GetDisplayIndex(displayHandle), dfbRet));
            }
        }
    }
#else
    BSTD_UNUSED(displayHandle);
    BSTD_UNUSED(outputIndex);
#endif
    return dfbRet;
}

DFB_PlatformResult
DFB_Platform_P_RemoveDisplayOutput_Composite( DFB_PlatformNexusHandle displayHandle,
                                              int                     outputIndex )
{
    DFB_PlatformResult      dfbRet = DFB_PLATFORM_UNSUPPORTED;
#if NEXUS_NUM_COMPOSITE_OUTPUTS
    DFB_PlatformNexusHandle outputHandle;

    dfbRet = DFB_PLATFORM_FAILURE;
    if (outputIndex < NEXUS_NUM_COMPOSITE_OUTPUTS)
    {
        outputHandle = DFB_Platform_P_GetDisplayOutputHandle(displayHandle, DFB_PlatformDisplayOutputType_eComposite, outputIndex);

        /* Ensure that the output is connected... */
        if (outputHandle)
        {
            if (NEXUS_Display_RemoveOutput(displayHandle, DFB_Platform_P_DisplayOutputToVideoOutput(outputHandle, DFB_PlatformDisplayOutputType_eComposite)) == NEXUS_SUCCESS)
            {
                dfbRet = DFB_Platform_P_SetDisplayOutputHandle(displayHandle, DFB_PlatformDisplayOutputType_eComposite, outputIndex, NULL);
                BDBG_MSG(("%s: Remove Composite%d Display%d Output (res=%d)\n", __FUNCTION__, outputIndex, DFB_Platform_P_GetDisplayIndex(displayHandle), dfbRet));
            }
        }
    }
#else
    BSTD_UNUSED(displayHandle);
    BSTD_UNUSED(outputIndex);
#endif
    return dfbRet;
}

DFB_PlatformResult
DFB_Platform_P_AddDisplayOutput_Svideo( DFB_PlatformNexusHandle displayHandle,
                                        int                     outputIndex )
{
    DFB_PlatformResult          dfbRet = DFB_PLATFORM_UNSUPPORTED;
#if NEXUS_NUM_SVIDEO_OUTPUTS
    DFB_PlatformNexusHandle     outputHandle;
    NEXUS_PlatformConfiguration platformConfig;

    dfbRet = DFB_PLATFORM_FAILURE;
    if (outputIndex < NEXUS_NUM_SVIDEO_OUTPUTS)
    {
        outputHandle = DFB_Platform_P_GetDisplayOutputHandle(displayHandle, DFB_PlatformDisplayOutputType_eSvideo, outputIndex);

        /* Check to make sure output is not already connected... */
        if (outputHandle == NULL)
        {
            NEXUS_Platform_GetConfiguration(&platformConfig);

            outputHandle = platformConfig.outputs.svideo[outputIndex];
            if (NEXUS_Display_AddOutput(displayHandle, DFB_Platform_P_DisplayOutputToVideoOutput(outputHandle, DFB_PlatformDisplayOutputType_eSvideo)) == NEXUS_SUCCESS)
            {
                dfbRet = DFB_Platform_P_SetDisplayOutputHandle( displayHandle, DFB_PlatformDisplayOutputType_eSvideo, outputIndex, outputHandle);
                BDBG_MSG(("%s: Add Svideo%d Display%d Output (res=%d)\n", __FUNCTION__, outputIndex, DFB_Platform_P_GetDisplayIndex(displayHandle), dfbRet));
            }
        }
    }

#else
    BSTD_UNUSED(displayHandle);
    BSTD_UNUSED(outputIndex);
#endif
    return dfbRet;
}

DFB_PlatformResult
DFB_Platform_P_RemoveDisplayOutput_Svideo( DFB_PlatformNexusHandle displayHandle,
                                           int                     outputIndex )
{
    DFB_PlatformResult      dfbRet = DFB_PLATFORM_UNSUPPORTED;
#if NEXUS_NUM_SVIDEO_OUTPUTS
    DFB_PlatformNexusHandle outputHandle;

    dfbRet = DFB_PLATFORM_FAILURE;

    if (outputIndex < NEXUS_NUM_SVIDEO_OUTPUTS)
    {
        outputHandle = DFB_Platform_P_GetDisplayOutputHandle(displayHandle, DFB_PlatformDisplayOutputType_eSvideo, outputIndex);

        /* Ensure that the output is connected... */
        if (outputHandle)
        {
            if (NEXUS_Display_RemoveOutput(displayHandle, DFB_Platform_P_DisplayOutputToVideoOutput(outputHandle, DFB_PlatformDisplayOutputType_eSvideo)) == NEXUS_SUCCESS)
            {
                dfbRet = DFB_Platform_P_SetDisplayOutputHandle( displayHandle, DFB_PlatformDisplayOutputType_eSvideo, outputIndex, NULL);
                BDBG_MSG(("%s: Remove Svideo%d Display%d Output (res=%d)\n", __FUNCTION__, outputIndex, DFB_Platform_P_GetDisplayIndex(displayHandle), dfbRet));
            }
        }
    }

#else
    BSTD_UNUSED(displayHandle);
    BSTD_UNUSED(outputIndex);
#endif
    return dfbRet;
}

DFB_PlatformResult
DFB_Platform_P_AddDisplayOutput_Scart( DFB_PlatformNexusHandle displayHandle,
                                       int                     outputIndex )
{
    DFB_PlatformResult          dfbRet = DFB_PLATFORM_UNSUPPORTED;
#if NEXUS_NUM_SCART_OUTPUTS
    DFB_PlatformNexusHandle     outputHandle;
    NEXUS_PlatformConfiguration platformConfig;

    dfbRet = DFB_PLATFORM_FAILURE;

    if (outputIndex < NEXUS_NUM_SCART_OUTPUTS)
    {
        outputHandle = DFB_Platform_P_GetDisplayOutputHandle(displayHandle, DFB_PlatformDisplayOutputType_eScart, outputIndex);

        /* Check to make sure output is not already connected... */
        if (outputHandle == NULL)
        {
            NEXUS_Platform_GetConfiguration(&platformConfig);

            outputHandle = platformConfig.outputs.scart[outputIndex];
            if (NEXUS_Display_AddOutput(displayHandle, DFB_Platform_P_DisplayOutputToVideoOutput(outputHandle, DFB_PlatformDisplayOutputType_eScart)) == NEXUS_SUCCESS)
            {
                dfbRet = DFB_Platform_P_SetDisplayOutputHandle( displayHandle, DFB_PlatformDisplayOutputType_eScart, outputIndex, outputHandle);
                BDBG_MSG(("%s: Add Scart%d Display%d Output (res=%d)\n", __FUNCTION__, outputIndex, DFB_Platform_P_GetDisplayIndex(displayHandle), dfbRet));
            }
        }
    }
#else
    BSTD_UNUSED(displayHandle);
    BSTD_UNUSED(outputIndex);
#endif
    return dfbRet;
}

DFB_PlatformResult
DFB_Platform_P_RemoveDisplayOutput_Scart( DFB_PlatformNexusHandle displayHandle,
                                          int                     outputIndex )
{
    DFB_PlatformResult      dfbRet = DFB_PLATFORM_UNSUPPORTED;
#if NEXUS_NUM_SCART_OUTPUTS
    DFB_PlatformNexusHandle outputHandle;

    dfbRet = DFB_PLATFORM_FAILURE;

    if (outputIndex < NEXUS_NUM_SCART_OUTPUTS)
    {
        outputHandle = DFB_Platform_P_GetDisplayOutputHandle(displayHandle, DFB_PlatformDisplayOutputType_eScart, outputIndex);

        /* Ensure that the output is connected... */
        if (outputHandle)
        {
            if (NEXUS_Display_RemoveOutput(displayHandle, DFB_Platform_P_DisplayOutputToVideoOutput(outputHandle, DFB_PlatformDisplayOutputType_eScart)) == NEXUS_SUCCESS)
            {
                dfbRet = DFB_Platform_P_SetDisplayOutputHandle( displayHandle, DFB_PlatformDisplayOutputType_eScart, outputIndex, NULL);
                BDBG_MSG(("%s: Remove Scart%d Display%d Output (res=%d)\n", __FUNCTION__, outputIndex, DFB_Platform_P_GetDisplayIndex(displayHandle), dfbRet));
            }

        }
    }
#else
    BSTD_UNUSED(displayHandle);
    BSTD_UNUSED(outputIndex);
#endif
    return dfbRet;
}

DFB_PlatformResult
DFB_Platform_P_AddDisplayOutput_Rfm( DFB_PlatformNexusHandle displayHandle,
                                     int                     outputIndex )
{
    DFB_PlatformResult          dfbRet = DFB_PLATFORM_UNSUPPORTED;
#if NEXUS_NUM_RFM_OUTPUTS
    DFB_PlatformNexusHandle     outputHandle;
    NEXUS_PlatformConfiguration platformConfig;

    dfbRet = DFB_PLATFORM_FAILURE;

    if (outputIndex < NEXUS_NUM_RFM_OUTPUTS)
    {
        outputHandle = DFB_Platform_P_GetDisplayOutputHandle(displayHandle, DFB_PlatformDisplayOutputType_eRfm, outputIndex);

        /* Check to make sure output is not already connected... */
        if (outputHandle == NULL)
        {
            NEXUS_Platform_GetConfiguration(&platformConfig);

            outputHandle = platformConfig.outputs.rfm[outputIndex];
            if (NEXUS_Display_AddOutput(displayHandle, DFB_Platform_P_DisplayOutputToVideoOutput(outputHandle, DFB_PlatformDisplayOutputType_eRfm)) == NEXUS_SUCCESS)
            {
                dfbRet = DFB_Platform_P_SetDisplayOutputHandle( displayHandle, DFB_PlatformDisplayOutputType_eRfm, outputIndex, outputHandle);
                BDBG_MSG(("%s: Add RFM%d Display%d Output (res=%d)\n", __FUNCTION__, outputIndex, DFB_Platform_P_GetDisplayIndex(displayHandle), dfbRet));
            }
        }
    }
#else
    BSTD_UNUSED(displayHandle);
    BSTD_UNUSED(outputIndex);
#endif
    return dfbRet;
}

DFB_PlatformResult
DFB_Platform_P_RemoveDisplayOutput_Rfm( DFB_PlatformNexusHandle displayHandle,
                                        int                     outputIndex )
{
    DFB_PlatformResult      dfbRet = DFB_PLATFORM_UNSUPPORTED;
#if NEXUS_NUM_RFM_OUTPUTS
    DFB_PlatformNexusHandle outputHandle;

    dfbRet = DFB_PLATFORM_FAILURE;
    if (outputIndex < NEXUS_NUM_RFM_OUTPUTS)
    {
        outputHandle = DFB_Platform_P_GetDisplayOutputHandle(displayHandle, DFB_PlatformDisplayOutputType_eRfm, outputIndex);

        /* Ensure that the output is connected... */
        if (outputHandle)
        {
            if (NEXUS_Display_RemoveOutput(displayHandle, DFB_Platform_P_DisplayOutputToVideoOutput(outputHandle, DFB_PlatformDisplayOutputType_eRfm)) == NEXUS_SUCCESS)
            {
                dfbRet = DFB_Platform_P_SetDisplayOutputHandle( displayHandle, DFB_PlatformDisplayOutputType_eRfm, outputIndex, NULL);
                BDBG_MSG(("%s: Remove RFM%d Display%d Output (res=%d)\n", __FUNCTION__, outputIndex, DFB_Platform_P_GetDisplayIndex(displayHandle), dfbRet));
            }
        }
    }
#else
    BSTD_UNUSED(displayHandle);
    BSTD_UNUSED(outputIndex);
#endif
    return dfbRet;
}

void
DFB_Platform_P_GetDefaultDisplayGraphicsFramebuffer3D( DFB_Platform_P_DisplayGraphicsFramebuffer3D *pFrameBuffer3D )
{
#if BCMNEXUS_DISPLAY_HAS_3DTV_SUPPORT
    NEXUS_GraphicsFramebuffer3D nexusFrameBuffer3D;

    BDBG_ASSERT(pFrameBuffer3D);

    NEXUS_Graphics_GetDefaultFramebuffer3D(&nexusFrameBuffer3D);

    pFrameBuffer3D->orientation = nexusFrameBuffer3D.orientation;
    pFrameBuffer3D->main        = nexusFrameBuffer3D.main;
    pFrameBuffer3D->alpha       = nexusFrameBuffer3D.alpha;
    pFrameBuffer3D->right       = nexusFrameBuffer3D.right;
    pFrameBuffer3D->rightAlpha  = nexusFrameBuffer3D.rightAlpha;
    pFrameBuffer3D->alphaW0     = nexusFrameBuffer3D.alphaW0;
    pFrameBuffer3D->alphaW1     = nexusFrameBuffer3D.alphaW1;
#else
    memset(pFrameBuffer3D, 0, sizeof(*pFrameBuffer3D));
    pFrameBuffer3D->orientation = DFB_Platform_P_VideoOrientation_e2D;
    pFrameBuffer3D->main        = NULL;
#endif
}

DFB_PlatformResult
DFB_Platform_P_SetDisplayGraphicsFramebuffer3D(       DFB_PlatformNexusHandle                      display,
                                                const DFB_Platform_P_DisplayGraphicsFramebuffer3D *pFrameBuffer3D,
                                                const DFB_PlatformRect *pLeftUpdateRect,
                                                const DFB_PlatformRect *pRightUpdateRect,
                                                bool  singleBuffered)
{
    NEXUS_Error                 err;
    BSTD_UNUSED(pLeftUpdateRect);
    BSTD_UNUSED(pRightUpdateRect);
    BSTD_UNUSED(singleBuffered);

#if BCMNEXUS_DISPLAY_HAS_3DTV_SUPPORT
    NEXUS_GraphicsFramebuffer3D nexusFrameBuffer3D;

    BDBG_ASSERT(pFrameBuffer3D);

    memset(&nexusFrameBuffer3D, 0, sizeof(nexusFrameBuffer3D));
    nexusFrameBuffer3D.orientation = pFrameBuffer3D->orientation;
    nexusFrameBuffer3D.main        = pFrameBuffer3D->main;
    nexusFrameBuffer3D.alpha       = pFrameBuffer3D->alpha;
    nexusFrameBuffer3D.right       = pFrameBuffer3D->right;
    nexusFrameBuffer3D.rightAlpha  = pFrameBuffer3D->rightAlpha;
    nexusFrameBuffer3D.alphaW0     = pFrameBuffer3D->alphaW0;
    nexusFrameBuffer3D.alphaW1     = pFrameBuffer3D->alphaW1;

    err = NEXUS_Display_SetGraphicsFramebuffer3D(display, &nexusFrameBuffer3D);
#else
    BDBG_ASSERT(pFrameBuffer3D);

    err = NEXUS_Display_SetGraphicsFramebuffer(display, pFrameBuffer3D->main);
#endif
    return (err == NEXUS_SUCCESS) ? DFB_PLATFORM_OK : DFB_PLATFORM_FAILURE;
}


/**
Summary:
Get current graphics compositing settings.
**/
DFB_PlatformResult DFB_Platform_P_GetGraphicsSettings(
    DFB_PlatformNexusHandle display,
    DFB_Platform_P_GraphicsSettings *pSettings /* [out] */
    )
{
    NEXUS_GraphicsSettings nexusSettings;

    BDBG_MSG_TRACE((">>%s", __FUNCTION__));

    NEXUS_Display_GetGraphicsSettings(display,&nexusSettings);

    pSettings->enabled = nexusSettings.enabled;
    pSettings->alpha   = nexusSettings.alpha;
    pSettings->zorder  = nexusSettings.zorder;

    pSettings->chromakeyEnabled = nexusSettings.chromakeyEnabled;

    ARGB_COLOUR(nexusSettings.lowerChromakey,
                pSettings->lowerChromakey.a,
                pSettings->lowerChromakey.r,
                pSettings->lowerChromakey.g,
                pSettings->lowerChromakey.b);

    ARGB_COLOUR(nexusSettings.upperChromakey,
                pSettings->upperChromakey.a,
                pSettings->upperChromakey.r,
                pSettings->upperChromakey.g,
                pSettings->upperChromakey.b);

    SET_RECT(pSettings->position,nexusSettings.position);
    SET_RECT(pSettings->clip,nexusSettings.clip);

    pSettings->frameBufferCallback.callback = nexusSettings.frameBufferCallback.callback;
    pSettings->frameBufferCallback.context  = nexusSettings.frameBufferCallback.context;
    pSettings->frameBufferCallback.param    = nexusSettings.frameBufferCallback.param;

    switch ( nexusSettings.sourceBlendFactor)
    {
        case NEXUS_CompositorBlendFactor_eZero:
            pSettings->sourceBlendFactor = DFB_PlatformBlendFactor_eZero;
            break;
        case NEXUS_CompositorBlendFactor_eOne:
            pSettings->sourceBlendFactor = DFB_PlatformBlendFactor_eOne;
            break;
        case NEXUS_CompositorBlendFactor_eSourceAlpha:
            pSettings->sourceBlendFactor = DFB_PlatformBlendFactor_eSourceAlpha;
            break;
        case NEXUS_CompositorBlendFactor_eInverseSourceAlpha:
            pSettings->sourceBlendFactor = DFB_PlatformBlendFactor_eInverseSourceAlpha;
            break;
        case NEXUS_CompositorBlendFactor_eConstantAlpha:
            pSettings->sourceBlendFactor = DFB_PlatformBlendFactor_eConstantAlpha;
            break;
        case NEXUS_CompositorBlendFactor_eInverseConstantAlpha:
            pSettings->sourceBlendFactor = DFB_PlatformBlendFactor_eInverseConstantAlpha;
            break;
        default:
            return DFB_PLATFORM_UNSUPPORTED;
    }


    switch ( nexusSettings.destBlendFactor)
    {
        case NEXUS_CompositorBlendFactor_eZero:
            pSettings->destBlendFactor = DFB_PlatformBlendFactor_eZero;
            break;
        case NEXUS_CompositorBlendFactor_eOne:
            pSettings->destBlendFactor = DFB_PlatformBlendFactor_eOne;
            break;
        case NEXUS_CompositorBlendFactor_eSourceAlpha:
            pSettings->destBlendFactor = DFB_PlatformBlendFactor_eSourceAlpha;
            break;
        case NEXUS_CompositorBlendFactor_eInverseSourceAlpha:
            pSettings->destBlendFactor = DFB_PlatformBlendFactor_eInverseSourceAlpha;
            break;
        case NEXUS_CompositorBlendFactor_eConstantAlpha:
            pSettings->destBlendFactor = DFB_PlatformBlendFactor_eConstantAlpha;
            break;
        case NEXUS_CompositorBlendFactor_eInverseConstantAlpha:
            pSettings->destBlendFactor = DFB_PlatformBlendFactor_eInverseConstantAlpha;
            break;
        default:
            return DFB_PLATFORM_UNSUPPORTED;
    }

    nexusSettings.constantAlpha = pSettings->constantAlpha;

    switch( nexusSettings.horizontalFilter )
    {
        case NEXUS_GraphicsFilterCoeffs_ePointSample:
            pSettings->horizontalFilter = DFB_PlatformFilterCoeffs_ePointSample;
            break;
        case NEXUS_GraphicsFilterCoeffs_eBilinear:
            pSettings->horizontalFilter = DFB_PlatformFilterCoeffs_eBilinear;
            break;
        case NEXUS_GraphicsFilterCoeffs_eAnisotropic:
            pSettings->horizontalFilter = DFB_PlatformFilterCoeffs_eAnisotropic;
            break;
        default:
            return DFB_PLATFORM_UNSUPPORTED;
    }

#if defined(BCMNEXUS_GFD0_HAS_VERTICAL_SCALER) || defined(BCMNEXUS_GFD1_HAS_VERTICAL_SCALER)
    switch( nexusSettings.verticalFilter )
    {
        case NEXUS_GraphicsFilterCoeffs_ePointSample:
            pSettings->verticalFilter = DFB_PlatformFilterCoeffs_ePointSample;
            break;
        case NEXUS_GraphicsFilterCoeffs_eBilinear:
            pSettings->verticalFilter = DFB_PlatformFilterCoeffs_eBilinear;
            break;
        case NEXUS_GraphicsFilterCoeffs_eAnisotropic:
            pSettings->verticalFilter = DFB_PlatformFilterCoeffs_eAnisotropic;
            break;
        default:
            return DFB_PLATFORM_UNSUPPORTED;
    }
#endif

#if BCMNEXUS_DISPLAY_HAS_3DTV_SUPPORT
    pSettings->stereoOffset = nexusSettings.graphics3DSettings.rightViewOffset;
#else
    pSettings->stereoOffset = 0;
#endif

    return DFB_PLATFORM_OK;
}

/**
Summary:
Set graphics compositing settings.
**/
DFB_PlatformResult DFB_Platform_P_SetGraphicsSettings(
    DFB_PlatformNexusHandle display,
    const DFB_Platform_P_GraphicsSettings *pSettings
    )
{
    NEXUS_Error rc;
    NEXUS_GraphicsSettings nexusSettings;

    BDBG_MSG_TRACE((">>%s", __FUNCTION__));

    NEXUS_Display_GetGraphicsSettings(display,&nexusSettings);

    nexusSettings.enabled = pSettings->enabled;
#ifdef BCMNEXUS_DISPLAY_GRAPHICS_HAS_VISIBLE_FLAG
    nexusSettings.visible = pSettings->enabled;
#endif
    nexusSettings.alpha   = pSettings->alpha;
    nexusSettings.zorder  = pSettings->zorder;

    nexusSettings.chromakeyEnabled = pSettings->chromakeyEnabled;
    nexusSettings.lowerChromakey = PIXEL_ARGB(pSettings->lowerChromakey.a,
                                              pSettings->lowerChromakey.r,
                                              pSettings->lowerChromakey.g,
                                              pSettings->lowerChromakey.b);

    nexusSettings.upperChromakey = PIXEL_ARGB(pSettings->upperChromakey.a,
                                              pSettings->upperChromakey.r,
                                              pSettings->upperChromakey.g,
                                              pSettings->upperChromakey.b);

    SET_RECT(nexusSettings.position,pSettings->position);
    SET_RECT(nexusSettings.clip,pSettings->clip);

    nexusSettings.frameBufferCallback.callback = pSettings->frameBufferCallback.callback;
    nexusSettings.frameBufferCallback.context  = pSettings->frameBufferCallback.context;
    nexusSettings.frameBufferCallback.param    = pSettings->frameBufferCallback.param;

    switch ( pSettings->sourceBlendFactor)
    {
        case DFB_PlatformBlendFactor_eZero:
            nexusSettings.sourceBlendFactor = NEXUS_CompositorBlendFactor_eZero;
            break;
        case DFB_PlatformBlendFactor_eOne:
            nexusSettings.sourceBlendFactor = NEXUS_CompositorBlendFactor_eOne;
            break;
        case DFB_PlatformBlendFactor_eSourceAlpha:
            nexusSettings.sourceBlendFactor = NEXUS_CompositorBlendFactor_eSourceAlpha;
            break;
        case DFB_PlatformBlendFactor_eInverseSourceAlpha:
            nexusSettings.sourceBlendFactor = NEXUS_CompositorBlendFactor_eInverseSourceAlpha;
            break;
        case DFB_PlatformBlendFactor_eConstantAlpha:
            nexusSettings.sourceBlendFactor = NEXUS_CompositorBlendFactor_eConstantAlpha;
            break;
        case DFB_PlatformBlendFactor_eInverseConstantAlpha:
            nexusSettings.sourceBlendFactor = NEXUS_CompositorBlendFactor_eInverseConstantAlpha;
            break;
        default:
            return DFB_PLATFORM_UNSUPPORTED;
    }

    switch ( pSettings->destBlendFactor)
    {
        case DFB_PlatformBlendFactor_eZero:
            nexusSettings.destBlendFactor = NEXUS_CompositorBlendFactor_eZero;
            break;
        case DFB_PlatformBlendFactor_eOne:
            nexusSettings.destBlendFactor = NEXUS_CompositorBlendFactor_eOne;
            break;
        case DFB_PlatformBlendFactor_eSourceAlpha:
            nexusSettings.destBlendFactor = NEXUS_CompositorBlendFactor_eSourceAlpha;
            break;
        case DFB_PlatformBlendFactor_eInverseSourceAlpha:
            nexusSettings.destBlendFactor = NEXUS_CompositorBlendFactor_eInverseSourceAlpha;
            break;
        case DFB_PlatformBlendFactor_eConstantAlpha:
            nexusSettings.destBlendFactor = NEXUS_CompositorBlendFactor_eConstantAlpha;
            break;
        case DFB_PlatformBlendFactor_eInverseConstantAlpha:
            nexusSettings.destBlendFactor = NEXUS_CompositorBlendFactor_eInverseConstantAlpha;
            break;
        default:
            return DFB_PLATFORM_UNSUPPORTED;
    }

    nexusSettings.constantAlpha = pSettings->constantAlpha;

    switch(pSettings->horizontalFilter)
    {
        case DFB_PlatformFilterCoeffs_ePointSample:
            nexusSettings.horizontalFilter =NEXUS_GraphicsFilterCoeffs_ePointSample;
            break;
        case DFB_PlatformFilterCoeffs_eBilinear:
            nexusSettings.horizontalFilter =NEXUS_GraphicsFilterCoeffs_eBilinear;
            break;
        case DFB_PlatformFilterCoeffs_eAnisotropic:
            nexusSettings.horizontalFilter =NEXUS_GraphicsFilterCoeffs_eAnisotropic;
            break;
        default:
            return DFB_PLATFORM_UNSUPPORTED;
    }

#if defined(BCMNEXUS_GFD0_HAS_VERTICAL_SCALER) || defined(BCMNEXUS_GFD1_HAS_VERTICAL_SCALER)
    switch(pSettings->verticalFilter)
    {
        case DFB_PlatformFilterCoeffs_ePointSample:
            nexusSettings.verticalFilter =NEXUS_GraphicsFilterCoeffs_ePointSample;
            break;
        case DFB_PlatformFilterCoeffs_eBilinear:
            nexusSettings.verticalFilter =NEXUS_GraphicsFilterCoeffs_eBilinear;
            break;
        case DFB_PlatformFilterCoeffs_eAnisotropic:
            nexusSettings.verticalFilter =NEXUS_GraphicsFilterCoeffs_eAnisotropic;
            break;
        default:
            return DFB_PLATFORM_UNSUPPORTED;
    }
#endif

#if BCMNEXUS_DISPLAY_HAS_3DTV_SUPPORT
    nexusSettings.graphics3DSettings.rightViewOffset = pSettings->stereoOffset;
#endif
    rc = NEXUS_Display_SetGraphicsSettings(display,&nexusSettings);

    BDBG_MSG_TRACE(("<<%s", __FUNCTION__));

    return (rc == NEXUS_SUCCESS) ? DFB_PLATFORM_OK : DFB_PLATFORM_FAILURE;
}

bool
DFB_Platform_P_DisplayWindow_GetDefaultInitSettings( unsigned                               windowSlot,
                                                     DFB_PlatformDisplayWindowInitSettings *pSettings )
{
    bool init = false;

    BDBG_MSG_TRACE((">>%s", __FUNCTION__));

    if (windowSlot < DFB_PLATFORM_NUM_DISPLAY_WINDOWS_MAX)
    {
        memset(pSettings, 0, sizeof(*pSettings));

        switch (windowSlot)
        {
            /* Set window 0 to primary display */
            case 0:
                pSettings->displayIndex = 0;
                pSettings->windowIndex  = 0;
                init = true;
                break;
#if (NUM_DISPLAYS > 1)
            /* Set window 1 to secondary display */
            case 1:
                pSettings->displayIndex = 1;
                pSettings->windowIndex  = 0;
                init = true;
                break;
#endif
#if defined (HUMAX_PLATFORM_BASE)
            case 2:
                pSettings->displayIndex = 0;
                pSettings->windowIndex  = 1;
                init = true;
                break;

            case 3:
                pSettings->displayIndex = 1;
                pSettings->windowIndex  = 1;
                init = true;
                break;
#endif
            default:
                break;
        }
    }
    BDBG_MSG_TRACE(("<<%s", __FUNCTION__));
    return init;
}

DFB_PlatformResult
DFB_Platform_P_DisplayWindow_Init( DFB_PlatformDisplayWindowInitSettings *pSettings,
                                   DFB_PlatformNexusHandle               *pHandle )
{
    DFB_PlatformResult               res = DFB_PLATFORM_FAILURE;
    NEXUS_DisplayHandle     displayHandle;
    DFB_PlatformNexusHandle windowHandle;

    BDBG_MSG_TRACE((">>%s", __FUNCTION__));

    BDBG_ASSERT(pSettings);
    BDBG_ASSERT(pHandle);

    displayHandle = (NEXUS_DisplayHandle)DFB_Platform_P_GetDisplayHandle(pSettings->displayIndex);

    if (displayHandle)
    {
       windowHandle = NEXUS_VideoWindow_Open(displayHandle, pSettings->windowIndex);
       if (windowHandle == NULL)
       {
           BDBG_ERR(("%s: Cannot open display%d window%d!\n", __FUNCTION__, pSettings->displayIndex, pSettings->windowIndex));
       }
       else
       {
           BDBG_MSG(("%s: Display %d window%d handle=%p\n", __FUNCTION__, pSettings->displayIndex, pSettings->windowIndex, windowHandle));
           *pHandle = windowHandle;
           res = DFB_PLATFORM_OK;
       }
    }
    else
    {
        BDBG_ERR(("%s: Display%d not already opened!\n", __FUNCTION__, pSettings->displayIndex));
    }

    BDBG_MSG_TRACE(("<<%s", __FUNCTION__));
    return res;
}

DFB_PlatformResult
DFB_Platform_P_DisplayOutput_Init(DFB_PlatformDisplayOutputInitSettings *pSettings,
                                  DFB_PlatformNexusHandle               *pHandle )
{
    DFB_PlatformResult               res;
    DFB_PlatformNexusHandle displayHandle;

    BDBG_MSG_TRACE((">>%s", __FUNCTION__));

    BDBG_ASSERT(pSettings);
    BDBG_ASSERT(pHandle);

    displayHandle = DFB_Platform_P_GetDisplayHandle(pSettings->displayIndex);

    if (displayHandle)
    {
        res = DFB_Platform_P_AddDisplayOutput(displayHandle, pSettings->outputType, pSettings->outputIndex);
        if (res == DFB_PLATFORM_OK)
        {
            *pHandle = DFB_Platform_P_GetDisplayOutputHandle(displayHandle, pSettings->outputType, pSettings->outputIndex);
            BDBG_MSG(("%s: Display Output%d (type=%d) Handle=%p\n", __FUNCTION__, pSettings->outputIndex, pSettings->outputType, *pHandle));
        }
    }
    else
    {
        BDBG_ERR(("%s: Cannot open display output%d (type=%d)!\n", __FUNCTION__, pSettings->outputIndex,  pSettings->outputType));
        res = DFB_PLATFORM_FAILURE;
        *pHandle = NULL;
    }

    BDBG_MSG_TRACE(("<<%s", __FUNCTION__));
    return res;
}

DFB_PlatformResult
DFB_Platform_P_Display_Init( unsigned                         displayIndex,
                             DFB_PlatformDisplayInitSettings *pSettings,
                             DFB_PlatformNexusHandle         *pHandle)
{
    DFB_PlatformResult             res;
    NEXUS_DisplaySettings displayCfg;
    NEXUS_Error           rc;

    BDBG_MSG_TRACE((">>%s", __FUNCTION__));

    BDBG_ASSERT(pSettings);
    BDBG_ASSERT(pHandle);

    NEXUS_Display_GetDefaultSettings(&displayCfg);

    if (pSettings->format != DFB_PlatformVideoFormat_eUnknown)
    {
        displayCfg.displayType  = DFB_Platform_P_DFBToNexusDisplayType(pSettings->displayType);
        displayCfg.vecIndex     = pSettings->vecIndex;
        displayCfg.format       = DFB_Platform_P_DFBToNexusVideoFormat(pSettings->format);
        displayCfg.aspectRatio  = DFB_Platform_P_DFBToNexusDisplayAspectRatio(pSettings->aspectRatio);
        displayCfg.timebase     = DFB_Platform_P_DFBToNexusTimebase(pSettings->timebase);
        displayCfg.background   = DFB_Platform_P_DFBToNexusColor(pSettings->background);
        displayCfg.xvYccEnabled = pSettings->xvYccEnabled;
#if defined(HUMAX_PLATFORM_BASE)
	if(displayIndex == 0)
		displayCfg.timingGenerator = NEXUS_DisplayTimingGenerator_eHdmiDvo;
	else
		displayCfg.timingGenerator = NEXUS_DisplayTimingGenerator_eAuto;
#endif
   }

    *pHandle = NEXUS_Display_Open(displayIndex, &displayCfg);

    if (*pHandle == NULL)
    {
        BDBG_ERR(("%s: Cannot open display%d!\n", __FUNCTION__, displayIndex));
        rc=BERR_TRACE(NEXUS_NOT_SUPPORTED);
        res = DFB_PLATFORM_FAILURE;
    }
    else
    {
        BDBG_MSG(("%s: Display%d Handle=%p\n", __FUNCTION__, displayIndex, *pHandle));
        dfb_p_displaysettings->display[displayIndex].displayHandle               = *pHandle;
        dfb_p_displaysettings->display[displayIndex].displaySettings.format      = pSettings->format;
        dfb_p_displaysettings->display[displayIndex].displaySettings.aspectRatio = pSettings->aspectRatio;
        dfb_p_displaysettings->display[displayIndex].displaySettings.background  = pSettings->background;
        DFB_Platform_P_DFBFormatToDisplaySize(pSettings->format, &dfb_p_displaysettings->display[displayIndex].displaySize);
        res = DFB_PLATFORM_OK;
    }

    BDBG_MSG_TRACE(("<<%s", __FUNCTION__));
    return res;
}

void
DFB_Platform_P_Display_Uninit( DFB_PlatformNexusHandle handle)
{
    int displayIndex;

    BDBG_MSG_TRACE((">>%s", __FUNCTION__));

    BDBG_ASSERT(handle);

#if !BCMNEXUS_VDC_SUPPORT_3D_VIDEO || !BCMNEXUS_DISPLAY_HAS_3DTV_SUPPORT
    DFB_Platform_P_FreeLocalSurface();
#endif

    displayIndex = DFB_Platform_P_GetDisplayIndex(handle);
    NEXUS_Display_RemoveAllOutputs(handle);
    NEXUS_Display_Close(handle);
    dfb_p_displaysettings->display[displayIndex].displayHandle = NULL;

    BDBG_MSG_TRACE(("<<%s", __FUNCTION__));
}

void
DFB_Platform_P_DisplayWindow_Uninit( DFB_PlatformNexusHandle handle)
{
    int windowSlot;

    BDBG_MSG_TRACE((">>%s", __FUNCTION__));

    BDBG_ASSERT(handle);

    NEXUS_VideoWindow_Close(handle);

    windowSlot = DFB_Platform_P_GetDisplayWindowSlot(handle);

    dfb_p_displaysettings->window[windowSlot].windowHandle = NULL;

    BDBG_MSG_TRACE(("<<%s", __FUNCTION__));
}

bool
DFB_Platform_P_DisplayOutput_GetDefaultInitSettings( unsigned                               outputSlot,
                                                     DFB_PlatformDisplayOutputInitSettings *pSettings )
{
    bool init = false;

    BDBG_MSG_TRACE((">>%s", __FUNCTION__));

    if (outputSlot < DFB_PLATFORM_NUM_DISPLAY_OUTPUTS_MAX)
    {
        memset(pSettings, 0, sizeof(*pSettings));

        switch (outputSlot)
        {
            /* Set HDMI 0 to primary output */
            case 0:
#if NEXUS_NUM_HDMI_OUTPUTS
                pSettings->outputType = DFB_PlatformDisplayOutputType_eHdmi;
                pSettings->outputIndex = 0;     /* HDMI 0 */
                pSettings->displayIndex = 0;    /* Primary display */
                init = true;
#endif
                break;
            /* Set Component 0 to primary output */
            case 1:
#if NEXUS_NUM_COMPONENT_OUTPUTS
                pSettings->outputType = DFB_PlatformDisplayOutputType_eComponent;
                pSettings->outputIndex = 0;     /* Component 0 */
                pSettings->displayIndex = 0;    /* Primary display */
                init = true;
#endif
                break;

            /* Set Composite 0 to primary output if "hdsd_mode" envar is set to "0",
               otherwise set to secondary output */
            case 2:
#if NEXUS_NUM_COMPOSITE_OUTPUTS && (NUM_DISPLAYS > 1)
                pSettings->outputType = DFB_PlatformDisplayOutputType_eComposite;
                pSettings->outputIndex = 0;     /* Composite 0 */
                pSettings->displayIndex = 1;    /* Secondary display */

                if (getenv("hdsd_mode") != NULL && strcmp(getenv("hdsd_mode"), "0") == 0)
                {
                    pSettings->displayIndex = 0;/* Primary display */
                }
                init = true;
#endif
                break;

            /* Set RFM 0 to primary output if "hdsd_mode" envar is set to "0",
               otherwise set to secondary output */
            case 3:
#if NEXUS_NUM_RFM_OUTPUTS && (NUM_DISPLAYS > 1)
                pSettings->outputType = DFB_PlatformDisplayOutputType_eRfm;
                pSettings->outputIndex = 0;     /* RFM 0 */
                pSettings->displayIndex = 1;    /* Secondary display */

                if (getenv("hdsd_mode") != NULL && strcmp(getenv("hdsd_mode"), "0") == 0)
                {
                    pSettings->displayIndex = 0;/* Primary display */
                }
                init = true;
#endif
                break;

            default:
                break;
        }
    }

    BDBG_MSG_TRACE(("<<%s", __FUNCTION__));
    return init;
}

