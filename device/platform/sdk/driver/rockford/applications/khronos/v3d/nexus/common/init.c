/******************************************************************************
 *    (c)2008-2010 Broadcom Corporation
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
 * $brcm_Workfile: init.c $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 3/17/11 4:23p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/applications/opengles_v3d/v3d/nexus/common/init.c $
 * 
 * Hydra_Software_Devel/2   3/17/11 4:23p erickson
 * SWVC4-133: merge
 * 
 * hauxwell_35230_v3d/6   12/13/10 10:26a gsweet
 * Fixed runtime warnings when display is larger than NTSC
 * 
 * hauxwell_35230_v3d/5   12/10/10 11:01a hills
 * SW35230-2542: Fixed console warning in cube example
 * 
 * hauxwell_35230_v3d/4   12/9/10 4:14p hills
 * Added filter settings.
 * 
 * hauxwell_35230_v3d/3   12/9/10 9:09a hills
 * Fixed compile warnings on 35230
 * 
 * hauxwell_35230_v3d/2   12/8/10 1:53p gsweet
 * Fix compile warning
 * 
 * hauxwell_35230_v3d/1   12/8/10 1:24p gsweet
 * Added HDMI support to V3D examples. Moved platform/display init into
 * common code.
 * 
 *
 *****************************************************************************/

#include "init.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef NEXUS_DTV_PLATFORM
#include "nexus_platform_boardcfg.h"
#endif

#define UNUSED(X) (void)X

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef SINGLE_PROCESS
eInitResult InitPlatform(void)
{
   NEXUS_Error                         err = NEXUS_NOT_SUPPORTED;
   NEXUS_PlatformSettings              platform_settings;
   NEXUS_PlatformConfiguration         platform_config;

   NEXUS_Platform_GetDefaultSettings(&platform_settings);

#if NEXUS_DTV_PLATFORM
   {
      char * b552 = getenv("panel_type");

      if (b552 && strcmp("B552", b552) == 0)
      {
         platform_settings.displayModuleSettings.panel.lvds.dvoLinkMode = NEXUS_PanelOutputLinkMode_eDualChannel1;
         platform_settings.displayModuleSettings.panel.lvds.lvdsColorMode = NEXUS_LvdsColorMode_e8Bit;
      }
      else
      {
         platform_settings.displayModuleSettings.panel.lvds.dvoLinkMode     = NEXUS_PanelOutputLinkMode_eQuad;
         platform_settings.displayModuleSettings.panel.lvds.lvdsColorMode   = NEXUS_LvdsColorMode_e10Bit;
      }
   }
#endif /* NEXUS_DTV_PLATFORM */

   platform_settings.openFrontend = false;

   err = NEXUS_Platform_Init(&platform_settings);
   if (err != NEXUS_SUCCESS)
   {
      printf("NEXUS_Platform_Init() failed\n");
      return eInitFailed;
   }

   NEXUS_Platform_GetConfiguration(&platform_config);

   return eInitSuccess;
}

#endif /* SINGLE_PROCESS */

#ifndef SINGLE_PROCESS
eInitResult InitPlatform(void)
{
   NEXUS_Error err = NEXUS_NOT_SUPPORTED;

   /* We are multiprocess mode Nexus, so join with the existing instance */
   err = NEXUS_Platform_Join();

   if (err != NEXUS_SUCCESS)
   {
      printf("Failed to join an existing server\n");
      return eInitFailed;
   }

   return eInitSuccess;
}
#endif /* !SINGLE_PROCESS */

eInitResult InitPlatformAndDefaultDisplay(NEXUS_DISPLAYHANDLE *handle, float *aspect, uint32_t w, uint32_t h)
{
   eInitResult res = InitPlatform();

   if (res == eInitSuccess)
   {
#ifdef SINGLE_PROCESS
      NEXUS_GraphicsSettings  graphics_settings;

      /* We are the primary process, so open the display */
      *handle = OpenDisplay(0, w, h);
      InitPanelOutput(*handle);
      InitCompositeOutput(*handle, w, h);
      InitComponentOutput(*handle);
      InitHDMIOutput(*handle);

      if (*handle != 0)
      {
         NEXUS_Display_GetGraphicsSettings(*handle, &graphics_settings);
         *aspect = (float)graphics_settings.position.width / graphics_settings.position.height;
      }
      else
         *aspect = (float)w / (float)h;
#else
      *aspect = (float)w / (float)h;
      *handle = 0;
#endif
   }

   return res;
}

void TermPlatform(NEXUS_DISPLAYHANDLE handle)
{
   /* Close the Nexus display */
#ifdef SINGLE_PROCESS
   if (handle != 0)
   {
#if NEXUS_NUM_HDMI_OUTPUTS && !NEXUS_DTV_PLATFORM
      NEXUS_HdmiOutputSettings      hdmiSettings;
      NEXUS_PlatformConfiguration   platform_config;
      NEXUS_Platform_GetConfiguration(&platform_config);

      NEXUS_HdmiOutput_GetSettings(platform_config.outputs.hdmi[0], &hdmiSettings);
      hdmiSettings.hotplugCallback.callback = 0;
      hdmiSettings.hotplugCallback.context = 0;
      hdmiSettings.hotplugCallback.param = 0;
      NEXUS_HdmiOutput_SetSettings(platform_config.outputs.hdmi[0], &hdmiSettings);
#endif
      NEXUS_Display_Close(handle);
   }

   NEXUS_Platform_Uninit();
#else
   UNUSED(handle);
#endif
}

/* If format == 0, w & h are used to determine requested video format */
NEXUS_DisplayHandle OpenDisplay(NEXUS_VideoFormat format, uint32_t w, uint32_t h)
{
   NEXUS_DisplayHandle     display = NULL;
   NEXUS_DisplaySettings   display_settings;
   NEXUS_GraphicsSettings  graphics_settings;

   /* Bring up display */
   NEXUS_Display_GetDefaultSettings(&display_settings);

   if (format != 0)
   {
      display_settings.format = format;
   }
#if !NEXUS_DTV_PLATFORM
   else
   {
      if (w <= 720 && h <= 480)
         display_settings.format = NEXUS_VideoFormat_eNtsc;
      else if (w <= 1280 && h <= 720)
         display_settings.format = NEXUS_VideoFormat_e720p;
      else
         display_settings.format = NEXUS_VideoFormat_e1080p;
   }
#else
   UNUSED(w);
   UNUSED(h);
#endif

   display = NEXUS_Display_Open(0, &display_settings);
   if (!display)
      printf("NEXUS_Display_Open() failed\n");

   NEXUS_Display_GetGraphicsSettings(display, &graphics_settings);
   graphics_settings.horizontalFilter = NEXUS_GraphicsFilterCoeffs_eBilinear;
   graphics_settings.verticalFilter = NEXUS_GraphicsFilterCoeffs_eBilinear;

   /* Disable blend with video plane */
   graphics_settings.sourceBlendFactor = NEXUS_CompositorBlendFactor_eOne;
   graphics_settings.destBlendFactor   = NEXUS_CompositorBlendFactor_eZero;

   NEXUS_Display_SetGraphicsSettings(display, &graphics_settings);

   return display;
}

void InitPanelOutput(NEXUS_DisplayHandle display)
{
#if NEXUS_NUM_PANEL_OUTPUTS

   NEXUS_PlatformConfiguration   platform_config;

   NEXUS_Platform_GetConfiguration(&platform_config);

   if (platform_config.outputs.panel[0])
   {
      char * b552 = getenv("panel_type");
      if (b552 && strcmp("B552", b552) == 0)
      {
         NEXUS_PanelOutputSettings     panelOutputSettings;
         NEXUS_PanelOutput_GetSettings(platform_config.outputs.panel[0], &panelOutputSettings);
         panelOutputSettings.frameRateMultiplier = 1;
         NEXUS_PanelOutput_SetSettings(platform_config.outputs.panel[0], &panelOutputSettings);
      }

      NEXUS_Display_AddOutput(display, NEXUS_PanelOutput_GetConnector(platform_config.outputs.panel[0]));

      NEXUS_BoardCfg_ConfigurePanel(true, true, true);
   }
#else
   UNUSED(display);
#endif
}

void InitComponentOutput(NEXUS_DisplayHandle display)
{
#if NEXUS_NUM_COMPONENT_OUTPUTS && !NEXUS_DTV_PLATFORM

   NEXUS_PlatformConfiguration   platform_config;
   NEXUS_Platform_GetConfiguration(&platform_config);

   if (platform_config.outputs.component[0])
      NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platform_config.outputs.component[0]));
#else
   UNUSED(display);
#endif
}

void InitCompositeOutput(NEXUS_DisplayHandle display, uint32_t w, uint32_t h)
{
#if NEXUS_NUM_COMPOSITE_OUTPUTS && !NEXUS_DTV_PLATFORM

   NEXUS_PlatformConfiguration   platform_config;

   if (w <= 720 && h <=480)
   {
      NEXUS_Platform_GetConfiguration(&platform_config);

      if (platform_config.outputs.composite[0])
         NEXUS_Display_AddOutput(display, NEXUS_CompositeOutput_GetConnector(platform_config.outputs.composite[0]));
   }
#else
   UNUSED(display);
   UNUSED(w);
   UNUSED(h);
#endif
}

#if NEXUS_NUM_HDMI_OUTPUTS && !NEXUS_DTV_PLATFORM
static void hotplug_callback(void *pParam, int iParam)
{
   NEXUS_HdmiOutputStatus status;
   NEXUS_HdmiOutputHandle hdmi = pParam;
   NEXUS_DisplayHandle display = (NEXUS_DisplayHandle)iParam;

   NEXUS_HdmiOutput_GetStatus(hdmi, &status);
   fprintf(stderr, "HDMI hotplug event: %s\n", status.connected?"connected":"not connected");

   /* the app can choose to switch to the preferred format, but it's not required. */
   if (status.connected)
   {
      NEXUS_DisplaySettings displaySettings;
      NEXUS_Display_GetSettings(display, &displaySettings);
      if (!status.videoFormatSupported[displaySettings.format])
      {
         fprintf(stderr, "\nCurrent format not supported by attached monitor. Switching to preferred format %d\n", status.preferredVideoFormat);
         displaySettings.format = status.preferredVideoFormat;
         NEXUS_Display_SetSettings(display, &displaySettings);
      }
   }
}
#endif

void InitHDMIOutput(NEXUS_DisplayHandle display)
{
#if NEXUS_NUM_HDMI_OUTPUTS && !NEXUS_DTV_PLATFORM

   NEXUS_HdmiOutputSettings      hdmiSettings;
   NEXUS_PlatformConfiguration   platform_config;
   NEXUS_Platform_GetConfiguration(&platform_config);

   if (platform_config.outputs.hdmi[0])
   {
      NEXUS_Display_AddOutput(display, NEXUS_HdmiOutput_GetVideoConnector(platform_config.outputs.hdmi[0]));
      /* Install hotplug callback -- video only for now */
      NEXUS_HdmiOutput_GetSettings(platform_config.outputs.hdmi[0], &hdmiSettings);
      hdmiSettings.hotplugCallback.callback = hotplug_callback;
      hdmiSettings.hotplugCallback.context = platform_config.outputs.hdmi[0];
      hdmiSettings.hotplugCallback.param = (int)display;
      NEXUS_HdmiOutput_SetSettings(platform_config.outputs.hdmi[0], &hdmiSettings);

      /* Force a hotplug to switch to a supported format if necessary */
      hotplug_callback(platform_config.outputs.hdmi[0], (int)display);
   }
#else
   UNUSED(display);
#endif
}

#ifdef __cplusplus
}
#endif
