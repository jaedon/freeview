/***************************************************************************
 *     (c)2007-2012 Broadcom Corporation
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
 * $brcm_Workfile: nexus_display_module.h $
 * $brcm_Revision: 133 $
 * $brcm_Date: 10/1/12 3:49p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/display/src/nexus_display_module.h $
 * 
 * 133   10/1/12 3:49p katrep
 * SW7445-1:add 7445 support
 * 
 * 132   8/1/12 10:22a erickson
 * SW7552-307: back out change
 * 
 * 130   7/24/12 12:36p vsilyaev
 * SW7425-3544: Use static allocation for large data structures
 * 
 * 129   7/23/12 10:20a erickson
 * SW7425-3545: move BVBI_TT_Line array off of stack, defer alloc until
 *  first use
 * 
 * 128   7/23/12 10:08a erickson
 * SW7425-3545: fix indenting
 * 
 * 127   7/3/12 5:21p bandrews
 * SW7425-717: added proto for display alignment
 * 
 * 126   6/19/12 12:42p vsilyaev
 * SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
 * 
 * 125   5/15/12 3:02p erickson
 * SW7346-792: only resolve NEXUS_DisplayAspectRatio_eAuto when converting
 *  to magnum types to remove ambiguity
 * 
 * 124   5/14/12 12:30p erickson
 * SW7346-824: cache graphics settings if internal window not created
 * 
 * 123   4/20/12 11:36a gmohile
 * SW7231-782 : Check if encoder callback is enabled before capturing
 *  frames
 * 
 * 122   4/13/12 5:01p erickson
 * SW7346-737: add vsync isr for HDMI Tx CRC
 * 
 * 121   3/15/12 2:29p gmohile
 * SW7231-584,SW7231-586: Disable frame drop in nexus for DSP video encode
 * 
 * 120   3/14/12 12:25p erickson
 * SW7401-4517: remove old code
 * 
 * 119   1/31/12 5:01p gmohile
 * SW7425-1473 : Fix raaga encoder support
 * 
 * 118   1/16/12 6:04p gmohile
 * SW7425-1473 : Merge raaga encoder support
 * 
 * 117   12/16/11 5:28p erickson
 * SW7425-1878: merge XUDlib support for video encoder userdata
 * 
 * SW7425-1878/2   12/16/11 2:12p ajagmag
 * SW7425-1878: Update based on discussions with David E.
 * 
 * SW7425-1878/1   12/9/11 1:32p ajagmag
 * SW7425-1878: Integrate XUDlib into nexus
 * 
 * 116   11/15/11 3:27p erickson
 * SW7425-1747: remove old code
 * 
 * 115   10/19/11 4:59p hongtaoz
 * SW7425-1546: update display refresh rate according to current display
 *  status; set stgIndex correctly for dual;
 * 
 * 114   10/19/11 3:10p erickson
 * SW7425-1546: call NEXUS_Display_P_VideoInputDisplayUpdate for display
 *  refresh rate changes, including drop-frame/non-drop-frame changes
 *
 * 113   10/6/11 3:42p erickson
 * SW7420-2070: add NEXUS_VBI_ENCODER_QUEUE_SIZE macro to control vbilib
 *  allocations using (NEXUS_VBI_ENCODER_QUEUE_SIZE+1)*NEXUS_NUM_DISPLAYS
 *  formula.
 *
 * 112   7/21/11 4:13p gmohile
 * SW7125-1014 : Rework power management
 *
 * 111   7/8/11 1:17p erickson
 * SW7346-306: VideoImageInput must set eInterruptRefreshRate with display
 *  format
 *
 * 110   6/28/11 9:47a erickson
 * SW7405-5366: add NEXUS_Display_GetStatus
 *
 * 109   5/27/11 2:42p erickson
 * SW7425-652: add NEXUS_DisplayStgSettings
 *
 * 108   4/22/11 3:13p vsilyaev
 * SW7425-416: Updated 3D display API
 *
 * 107   4/21/11 1:17p vsilyaev
 * SW7425-404: Added API to override orientaion of 2D video format
 *
 * 106   3/31/11 3:58p gmohile
 * SW7408-260 : Open/close VDC handles during resume/standby
 *
 * 105   3/28/11 3:45p vsilyaev
 * SW7335-1214: Added NEXUS_CallbackHandler framework
 *
 * 104   3/11/11 4:13p erickson
 * SW7346-103: add support for CMP CRC capture
 *
 * 103   2/25/11 4:37p erickson
 * SW7422-255: add AMOL support
 *
 * 102   2/16/11 10:14a gmohile
 * SW7408-210 : Fix kernel mode build
 *
 * 101   12/23/10 12:50p erickson
 * SW7425-34: remove NEXUS_IS_ANALOG_DISPLAY. instead, use
 *  BAVC_VbiPath_eUnknown returned by BVDC_Display_GetVbiPath to determine
 *  if display is VBI capable
 *
 * 100   12/2/10 8:14p spothana
 * SW7420-1177: Adding nexus hdmi dvo module.
 *
 * SW7420-1177/3   11/5/10 3:28p spothana
 * SW7420-1177: Update nexus hdmi dvo.
 *
 * SW7420-1177/2   11/5/10 2:25p spothana
 * SW7420-1177: Merging from latest
 *
 * 99   11/2/10 2:19p vsilyaev
 * SW7422-64: Added API to set 3D framebuffer
 *
 * 98   10/22/10 12:11p erickson
 * SW7422-69: remove condition
 *
 * 97   10/22/10 12:09p erickson
 * SW7422-69: move hdmi input connection to 7400 for 7422 support
 *
 * 96   9/30/10 10:15a mbatchel
 * SW35230-1452: Add VDEC tuning API.
 *
 * 95   9/9/10 12:33p petlee
 * SW35230-1262: Add PQ customization templates and examples
 *
 * 94   9/3/10 5:08p mbatchel
 * SW35230-878, SW35230-1206: Add progressive-cadence tuning parameters.
 *
 * 93   8/13/10 4:04p petlee
 * SW35230-1001: Add tunig file for backlight dimming
 *
 * 92   8/5/10 4:29p petlee
 * SW35230-764: Integrate raw VBI capture
 *
 * 91   7/15/10 8:26a mbatchel
 * SW35230-577: Add VAFE and VINDECO to NEXUS Tuning extension.
 *
 * 90   7/8/10 1:22p mbatchel
 * SW35230-577: Add tnt shim.
 *
 * 89   7/8/10 11:48a petlee
 * SW35230-577: Add headers for the new Tuning Shim Nexus Extension
 *
 * 88   6/24/10 11:49a petlee
 * SW35230-408: Add header for Nexus_AonAdc Interface
 *
 * 87   6/2/10 8:30p bandrews
 * SW3548-2323: share nexus_video_window_tune.h; create others for 35230
 *
 * 86   5/20/10 5:03p mphillip
 * SW7335-666: Enable CGMSA and CGMSB to work together
 *
 * 85   5/13/10 10:37a erickson
 * SW3548-2921: add pq_disabled=y support
 *
 * 84   4/23/10 5:33p petlee
 * SW35230-183: Add the new Nexus_AdcConfiguration Interface
 *
 * 83   3/30/10 4:39p bandrews
 * SW3548-2323: merge tuning api to main
 *
 * 82   3/26/10 2:19p erickson
 * SW7405-3965: refactor mosaic to support hd/sd simul mode
 *
 * 81   3/23/10 3:25p petlee
 * SW35230-106: Merging branch SW35230-106 to mainline
 *
 * 80   3/17/10 2:52p mward
 * SW7125-230:  Add 7125 to NEXUS_HAS_PEP.
 *
 * 79   3/12/10 4:05p erickson
 * SW3548-2643: added NEXUS_PanelOutput_P_ApplyCurrentStatus
 *
 * SW35230-106a/3   3/8/10 3:29p petlee
 * SW35230-106: Add #if for 35230 specific variable
 *
 * SW35230-106a/2   2/25/10 3:08p petlee
 * SW35230-106: Remove obsolete comment
 *
 * SW35230-106a/1   2/25/10 3:02p petlee
 * SW35230-106: Add new handle for the ANV PI
 *
 * 78   2/23/10 5:14p petlee
 * SW35230-18: Add check for BCHP_CHIP == 35230
 *
 * 77   2/16/10 4:47p erickson
 * SW3548-2776: add
 *  NEXUS_PictureCtrlContrastStretch.dynamicBacklightScaleFactor
 *
 * 76   2/2/10 5:37p erickson
 * SWGIGGS2S-254: revise dynamic backlight wiring. PanelOutput must store
 *  scalingFactor and scalingShift.
 *  NEXUS_PictureCtrl_SetCustomContrastStretch must clear. have display
 *  instance store the panel handle for easier and safer lookup.
 *
 * 75   1/11/10 9:44a erickson
 * SW3556-979: rework VPS, CGMS, CGMS-B and WSS encoding. instead of
 *  trying to get into the VBIlib encoder queue, we just intercept the
 *  queue in the VEC isr. this removes the BKNI_Sleep and possible Set
 *  failure.
 *
 * 74   12/20/09 4:59p randyjew
 * SW7468-6:Add 7468 support
 *
 * 73   11/12/09 10:50a erickson
 * SW3548-2616: refactor NEXUS_Display_P_InitGraphics and related code
 *
 * 72   11/3/09 2:12p mward
 * SW7125-51: Enable TNT for 7125 in Nexus.
 *
 * 71   9/17/09 4:10p erickson
 * SW7405-3033: back out last change. fix datatype instead.
 *
 * 70   9/9/09 1:19p erickson
 * SW7405-2977: allow build with no video decoder
 *
 * 69   8/12/09 10:09p jrubio
 * PR55232: add 7342/7340
 *
 * 68   8/10/09 8:17a gmohile
 * PR 56400 : Merge legacy vdc support to main line
 *
 * 67   8/6/09 2:38p erickson
 * PR56417: removed NEXUS_GraphicsSettings.gammaCorrection. feature should
 *  not be used.
 *
 * 66   7/23/09 10:50a erickson
 * PR56969: fix DTV. all settops will expose the hddvi interface. no dtv
 *  chips will expose it. this could change in the future.
 *
 * 65   7/23/09 10:39a erickson
 * PR56969: add hddvi input
 *
 * 64   7/22/09 11:40a erickson
 * PR56997: separate AnalogVideoDecoder code
 *
 * 63   7/21/09 10:20a jrubio
 * PR56906: add TNT for 7335/7325
 *
 * 62   7/16/09 11:50a erickson
 * PR56417: added NEXUS_GraphicsSettings.gammaCorrection
 *
 * 61   6/24/09 3:42p erickson
 * PR56233: enable PEP for 7335 and 7325
 *
 * 60   6/8/09 7:06a erickson
 * PR55617: rename NEXUS_P_DisplayAspectRatio_ToMagnum
 *
 * 59   5/20/09 3:29p erickson
 * PR54880: keep track of whether VDC heaps were created and only destroy
 *  what was created
 *
 * 58   5/20/09 11:29a erickson
 * PR55292: call NEXUS_VideoOutput_P_DestroyLink in
 *  NEXUS_Display_RemoveOutput
 *
 * 57   5/19/09 10:59a erickson
 * PR54880: fix heap handling. don't assume VideoDecoder will return NULL
 *  heap for default.
 *
 * 56   4/24/09 4:20p jgarrett
 * PR 53506: Adapting to new VDB OSD structure, adding specialfeature to
 *  graphics
 *
 * 55   4/8/09 4:31p vsilyaev
 * PR 54016: Added support for precompiled header
 *
 * 54   4/7/09 11:42a erickson
 * PR53623: change #define to NEXUS_NUM_656_OUTPUTS
 *
 * 53   4/1/09 12:16p erickson
 * PR53623: merge 656 output support
 *
 * PR53623/1   3/31/09 10:07a mward
 * PR 53623: 656 output support.
 *
 * 52   3/23/09 1:44p erickson
 * PR53425: add NEXUS_DisplaySettings.vsyncCallback
 *
 * 51   3/4/09 1:03p jgarrett
 * PR 52618: Adding forcePcFormat option to HdmiInput
 *
 * 50   3/2/09 6:10p jgarrett
 * PR 52520: Moving scaler settings to common functions for VDB support
 *
 * 49   2/26/09 2:32p jgarrett
 * PR 52464: Adding RTS settings update on display format change
 *
 * 48   2/11/09 3:17p erickson
 * PR52032: remove NEXUS_NUM_BYPASS_DISPLAYS
 *
 * 47   2/9/09 3:15p erickson
 * PR50703: remove 7420 exception
 *
 * 46   1/26/09 11:07a erickson
 * PR51468: global variable naming convention
 *
 * 45   1/14/09 12:33p erickson
 * PR46300: fix non-DTV
 *
 * 44   1/13/09 6:15p erickson
 * PR46300: added NEXUS_VideoWindow_SetSpecialFeature1Settings
 *
 * 43   1/6/09 11:32a jgarrett
 * PR 48984: Merge dynamic RTS code to main branch
 *
 * 42   12/24/08 12:40p jgarrett
 * PR 50703: Disabling VBI for 7420
 *
 * 41   12/17/08 11:40p erickson
 * PR50501: refactor HdmiOutput to remove double BVDC_ApplyChanges and 400
 *  msec wait on format change
 *
 * PR48984/3   12/5/08 4:23p jgarrett
 * PR 48984: Updating to latest baseline
 *
 * 40   11/28/08 1:04p nickh
 * PR48963: Add 7420 PEP and TNT support
 *
 * PR48984/2   11/26/08 6:19p jgarrett
 * PR 48984: Merging to latest baseline
 *
 * 39   11/24/08 10:15a erickson
 * PR49623: allow vbi encoding on bypass display
 *
 * PR48984/1   11/20/08 6:46p jgarrett
 * PR 48984: Adding VDB support
 *
 * 38   11/4/08 12:38p erickson
 * PR47030: add NEXUS_Display_P_DestroyHeap, refactor so that all VDC
 *  Sources are created by NEXUS_VideoInput_P_CreateLink
 *
 * 37   10/30/08 8:55p erickson
 * PR47030: manage VDC heap
 *
 * 36   10/22/08 11:40a erickson
 * PR48216: allow display to compile with no HdmiInput module
 *
 * 35   9/22/08 12:10p erickson
 * PR47190: added NEXUS_PictureCtrl_SetColorClipSettings
 *
 * 34   9/19/08 5:19p erickson
 * PR46063: added NEXUS_Display_SetCustomFormatSettings
 *
 * 33   9/5/08 1:16p erickson
 * PR45897: add dynamic backlight options for panels
 *
 * 32   8/22/08 7:06p erickson
 * PR45795: replace enum with 2HD/HD/SD buffer settings. this allows user
 *  to fully configure VDC memory allocation.
 *
 * 31   8/21/08 3:02p erickson
 * PR45959: add sample extension
 *
 * 30   8/4/08 2:31p erickson
 * PR45271: keep track of AnalogVideoDecoder opens. prevent duplicates.
 *  auto-close when DisplayModule closes.
 *
 * 29   7/7/08 2:40p erickson
 * PR44619: prevent duplicate NEXUS_PanelOutput_Open calls
 *
 * 28   7/3/08 5:07p erickson
 * PR43699: Remove unnecessary VDC Get functions from Nexus to avoid race
 *  conditions
 *
 * 27   6/23/08 5:15p vsilyaev
 * PR 40921: Fixed ADC mapping for scart
 *
 * 26   6/23/08 7:19a erickson
 * PR43914: move BLVD_Open before BVDC_Open
 *
 * 25   6/19/08 2:32p vsilyaev
 * PR 40921: There is no component output on the 93556 platform
 *
 * 24   6/11/08 12:46p vsilyaev
 * PR 43491: Added support for CCIR-656 input
 *
 * 23   5/21/08 10:33a erickson
 * PR42678: rename NEXUS_DvoOutput to NEXUS_PanelOutput
 *
 * 22   5/12/08 4:09p erickson
 * PR42628: free NEXUS_VideoOutput_P_Link when DisplayModule closes
 *
 * 21   5/8/08 3:08p erickson
 * PR42487: merge, with fix
 *
 * PR42487/1   5/7/08 3:04p gorgon
 * PR42487:Support PC input HW auto raster in 3548
 *
 * 20   4/30/08 3:08p erickson
 * PR39453: add 3548/3556 features
 *
 * 19   4/30/08 11:14a erickson
 * PR41371: add CGMS-B support
 *
 * 18   4/11/08 3:37p vsilyaev
 * PR 40921: Added SCART input
 *
 * 17   4/11/08 11:06a vsilyaev
 * PR 40410: Fixed naming typo
 *
 * 16   4/8/08 3:18p erickson
 * PR39453: add 3548 & 3556
 *
 * 15   4/8/08 1:53p erickson
 * PR39453: add 3548 & 3556
 *
 * 14   4/7/08 4:10p erickson
 * PR32888: added NEXUS_CRC_CAPTURE
 *
 * 13   4/4/08 4:36p erickson
 * PR39257: default NEXUS_NUM_BYPASS_DISPLAYS to 0
 *
 * 12   4/4/08 4:06p erickson
 * PR39257: add NEXUS_DisplayType_eBypass for DTV
 *
 * 11   4/3/08 10:17a erickson
 * PR41122: move picture quality code to 7400
 *
 * 10   4/2/08 3:16p erickson
 * PR40950: 740x not ready for PQ yet
 *
 * 9   4/2/08 2:47p erickson
 * PR40950: move PictureCtrl custom data into function params
 *
 * 8   4/1/08 1:46p jgarrett
 * PR 40606: Caching rate information for HDMI
 *
 * 7   3/27/08 6:20p vsilyaev
 * PR 40818: Added management of VDC heaps
 *
 * 6   3/26/08 11:44p vsilyaev
 * PR 40410: Added more video settings for HDMI Input interface
 *
 * 5   3/24/08 5:35p vsilyaev
 * PR 40755: Added implementation for colorkeying
 *
 * 4   3/21/08 12:17p vsilyaev
 * PR 39986: Added VideoImageInput
 *
 * 3   2/1/08 3:20p erickson
 * PR38679: add vbi support
 *
 * 2   1/31/08 9:55a erickson
 * PR36808: added SyncChannel interface
 *
 * 1   1/18/08 2:20p jgarrett
 * PR 38808: Merging to main branch
 *
 **************************************************************************/
#ifndef NEXUS_DISPLAY_MODULE_H__
#define NEXUS_DISPLAY_MODULE_H__

#include "nexus_base.h"
#include "nexus_platform_features.h"

#define B_HAS_VBI   1
#define B_HAS_VBI_ENCODE   1
#ifndef NEXUS_VBI_ENCODER_QUEUE_SIZE
/* override NEXUS_VBI_ENCODER_QUEUE_SIZE in nexus_platform_features.h */
#define NEXUS_VBI_ENCODER_QUEUE_SIZE 32
#endif

/**
PEP is picture enhancement processor
    PEP has a CAB (color adjustment block) and LAB (luma adjustment block)
TNT is <>
    TNT adjusts sharpness
CSC is color space convertor
    CSC adjusts hue, saturation, brightness
    All chips have a CSC
**/
#if BCHP_CHIP == 7400 || BCHP_CHIP == 7420 || BCHP_CHIP == 7335 || BCHP_CHIP == 7325 || \
BCHP_CHIP==7342 || BCHP_CHIP==7340 || BCHP_CHIP==7468 || BCHP_CHIP ==7125 || BCHP_CHIP ==7445
#define NEXUS_HAS_PEP 1
#endif
#if BCHP_CHIP == 7400 || BCHP_CHIP == 7420 || BCHP_CHIP == 7335 || BCHP_CHIP ==7325 || BCHP_CHIP ==7125 \
|| BCHP_CHIP ==7445
#define NEXUS_HAS_TNT 1
#endif
#if BCHP_CHIP == 3548 || BCHP_CHIP == 3556 || BCHP_CHIP == 35230
#define NEXUS_HAS_CM3D 1
#endif
#if BCHP_CHIP == 3548 || BCHP_CHIP == 3556
#define NEXUS_HAS_HW_AUTO_RASTER 1
#endif


#define NEXUS_HAS_MOSAIC_MODE 1

#include "nexus_display_thunks.h"
#include "nexus_display.h"
#include "nexus_display_init.h"
#include "priv/nexus_display_standby_priv.h"
#include "priv/nexus_display_priv.h"
#include "nexus_display_vbi.h"
#include "nexus_video_window.h"
#include "nexus_video_window_tune.h"
#include "nexus_video_input_crc.h"
#include "priv/nexus_video_window_priv.h"
#include "priv/nexus_core.h"
#include "nexus_core_utils.h"
#include "nexus_picture_ctrl.h"
#include "nexus_video_adj.h"
#if NEXUS_HAS_CM3D
#include "nexus_video_window_cm3d.h"
#include "nexus_video_window_xvycc.h"
#endif
#include "nexus_display_extensions.h"

#if BCHP_CHIP == 3563
#include "nexus_display_3563.h"
#elif BCHP_CHIP == 3548 || BCHP_CHIP == 3556
#include "nexus_display_3548.h"
#elif BCHP_CHIP == 35230
#include "nexus_display_tune.h"
#include "nexus_video_input_tune.h"
#include "nexus_display_35230.h"
#endif

#if NEXUS_HAS_MOSAIC_MODE
    #include "nexus_mosaic_display.h"
#endif

#if NEXUS_NUM_ANALOG_VIDEO_DECODERS
    #include "nexus_analog_video_decoder.h"
    #include "nexus_composite_input.h"
    #include "nexus_component_input.h"
    #include "nexus_svideo_input.h"
    #include "nexus_pc_input.h"
#endif

#include "nexus_video_output.h"
#include "nexus_video_input.h"
#include "priv/nexus_video_input_priv.h"
#include "nexus_video_input_vbi.h"
#include "nexus_component_output.h"
#include "nexus_svideo_output.h"
#include "nexus_composite_output.h"
#include "nexus_ccir656_output.h"
#include "nexus_video_output.h"
#include "nexus_video_image_input.h"
#include "nexus_video_hdmi_input.h"
/* all settops expose the HD_DVI interface, even if HW does not support */
#include "nexus_hddvi_input.h"

#if NEXUS_NUM_SCART_INPUTS
#include "nexus_scart_input.h"
#endif
#if defined(NEXUS_NUM_656_INPUTS)
/* due to way how thunk layer works, if header file present in the module
include directory, it should be included here, and .C file with implementation must be compiled */
#include "nexus_ccir656_input.h"
#endif

#include "blst_list.h"
#include "bvdc.h"
#include "bvdc_test.h"
#if B_HAS_VBI
#include "bvbi.h"
#include "bvbilib.h"
#endif
#include "priv/nexus_core_video.h"
#if NEXUS_HAS_VIDEO_DECODER
#include "priv/nexus_video_decoder_priv.h"
#endif

#if NEXUS_HAS_VIDEO_ENCODER && !NEXUS_NUM_DSP_VIDEO_ENCODERS
#include "bxudlib.h"
#endif

#include "nexus_display_impl.h"
#include "nexus_video_window_impl.h"
#include "nexus_video_output_impl.h"
#include "nexus_video_input_impl.h"
#include "nexus_vdb_impl.h"
#if NEXUS_NUM_ANALOG_VIDEO_DECODERS
#include "nexus_analog_video_decoder_impl.h"
#endif

#include "blst_squeue.h"
#if NEXUS_NUM_HDMI_OUTPUTS
#include "nexus_hdmi_output.h"
#endif


#ifdef __cplusplus
extern "C" {
#endif

typedef struct NEXUS_DisplayModule_State {
    BVDC_Handle vdc;
    BRDC_Handle rdc;
    NEXUS_DisplayModuleSettings moduleSettings;
    NEXUS_HeapHandle heap; /* main heap used in BVDC_Open */
    NEXUS_DisplayModuleDependencies modules;
    NEXUS_DisplayHandle displays[NEXUS_NUM_DISPLAYS];
    BLST_S_HEAD(NEXUS_DisplayModule_P_Inputs, NEXUS_VideoInput_P_Link) inputs;
    NEXUS_DisplayUpdateMode updateMode;
    bool lastUpdateFailed;

    /* mapping of nexus to vdc heap */
#define MAX_VDC_HEAPS 4
    struct {
        NEXUS_HeapHandle nexusHeap;
        BVDC_Heap_Handle vdcHeap;
        NEXUS_DisplayHeapSettings settings;
        unsigned refcnt;
    } vdcHeapMap[MAX_VDC_HEAPS];

    struct {
#if NEXUS_NUM_COMPONENT_OUTPUTS
        struct NEXUS_ComponentOutput component[NEXUS_NUM_COMPONENT_OUTPUTS];
#endif
#if NEXUS_NUM_COMPOSITE_OUTPUTS
        struct NEXUS_CompositeOutput composite[NEXUS_NUM_COMPOSITE_OUTPUTS];
#endif
#if NEXUS_NUM_SVIDEO_OUTPUTS
        struct NEXUS_SvideoOutput svideo[NEXUS_NUM_SVIDEO_OUTPUTS];
#endif
#if NEXUS_NUM_656_OUTPUTS
        struct NEXUS_Ccir656Output ccir656[NEXUS_NUM_656_OUTPUTS];
#endif
        int dummy;
    } outputs;
#if B_HAS_VBI
    BVBI_Handle vbi;
    BVBIlib_Handle vbilib;
    BVBIlib_List_Handle vbilist;
    BVBI_TT_Line *ttLines; /* avoided large stack */
#endif

#if NEXUS_NUM_HDMI_OUTPUTS
    /* Some HdmiOutput changes must be deferred until after the master ApplyChanges. This stores that state. */
    struct {
        NEXUS_DisplayHandle display;
        void *hdmiOutput;
        BAVC_MatrixCoefficients colorimetry;
        NEXUS_VideoFormat format;
        NEXUS_DisplayAspectRatio aspectRatio;
    } hdmiOutput;
#endif

#if NEXUS_NUM_HDMI_DVO
    /* Some HdmiDvo changes must be deferred until after the master ApplyChanges. This stores that state. */
    struct {
        NEXUS_DisplayHandle display;
        void *hdmiDvo;
        BAVC_MatrixCoefficients colorimetry;
        NEXUS_VideoFormat format;
        NEXUS_DisplayAspectRatio aspectRatio;
    } hdmiDvo;
#endif

#if NEXUS_NUM_MOSAIC_DECODES
/* NEXUS_NUM_MOSAIC_DECODE_SETS corresponds to the # of AVD cores that can be put into mosaic mode. */
#define NEXUS_NUM_MOSAIC_DECODE_SETS 2
    struct {
        NEXUS_VideoInputObject input;
        unsigned parentIndex; /* cache the lookup of the videodecoder's parentIndex */
    } mosaicInput[NEXUS_NUM_MOSAIC_DECODE_SETS];
#endif

    bool pqDisabled; /* export pq_disabled=y provides a divide-and-conquer method of isolating PQ-related problems. */

    /* instead of allocating large data structures on the stack, they are allocated  in the static data (it's safe since module is serialized)
       data is kept in union with entry per function and we use 'ccokie' to verify that union was not stoled */
    union {
#if NEXUS_NUM_HDMI_OUTPUTS
        struct {
            BERR_Code (*cookie)(void *, NEXUS_DisplayHandle , NEXUS_VideoFormat , NEXUS_DisplayAspectRatio );
            NEXUS_HdmiOutputStatus hdmiOutputStatus;
        } NEXUS_VideoOutput_P_SetHdmiFormat;
#endif
        unsigned unused;
    } functionData;
} NEXUS_DisplayModule_State;

struct NEXUS_DisplayGraphics {
    BVDC_Source_Handle source;
    BVDC_Window_Handle windowVdc;/* it indicates  that graphics is active, e.g. it both enabled in settings and frambuffer was assigned */
    BSUR_Surface_Handle queuedSurface; /* surface queued for display in hardware */
    NEXUS_GraphicsFramebuffer3D frameBuffer3D;
    uint16_t frameBufferWidth; /* width of the frame buffer */
    uint16_t frameBufferHeight; /* height of the frame buffer  */
    NEXUS_PixelFormat frameBufferPixelFormat; /* pixelFormat of the frame buffer */
    NEXUS_IsrCallbackHandle frameBufferCallback;
    NEXUS_GraphicsSettings cfg;
    NEXUS_ColorMatrix colorMatrix;
    bool colorMatrixSet;
    unsigned validCount;
    NEXUS_GraphicsColorSettings colorSettings;
};

typedef struct NEXUS_Display_P_Image {    
    BLST_SQ_ENTRY(NEXUS_Display_P_Image) link;
    void *addr;
} NEXUS_Display_P_Image;
BLST_SQ_HEAD(NEXUS_Display_P_ImageQueue, NEXUS_Display_P_Image );

struct NEXUS_Display {
    NEXUS_OBJECT(NEXUS_Display);
    BVDC_Compositor_Handle compositor;
    BVDC_Display_Handle displayVdc;
    BFMT_VideoInfo *customFormatInfo;
#if B_HAS_VBI_ENCODE
    struct {
        BVBI_Encode_Handle enc_core; /* must be NULL if not created */
        BVBIlib_Encode_Handle enc; /* this is VBI encode handle destination for VBI data */
        BINT_CallbackHandle tf_isr, bf_isr;
        bool progressive; /* need fast way in isr to know if display is progressive and modify tf/bf_isr field parameter */
        NEXUS_DisplayVbiSettings settings;
        BVBI_AMOL_Type amolType;

        struct {
            bool wssSet;
            uint16_t wssData;

            bool cgmsTopSet;
            bool cgmsBottomSet;
            uint32_t cgmsData;

            bool cgmsBTopSet;
            bool cgmsBBottomSet;
            BVBI_CGMSB_Datum cgmsBData;

            bool vpsSet;
            BVBI_VPSData vpsData;
        } pending;
    } vbi;
#endif
    unsigned index;
    NEXUS_DisplaySettings cfg;
    NEXUS_DisplayStatus status;
    NEXUS_PictureCtrlColorClipSettings colorClipSettings;
    NEXUS_DisplayStgSettings stgSettings;
    unsigned stgIndex;

    struct NEXUS_VideoWindow windows[NEXUS_NUM_VIDEO_WINDOWS];
    struct NEXUS_DisplayGraphics graphics;
    BLST_D_HEAD(NEXUS_Display_P_Outputs, NEXUS_VideoOutput_P_Link) outputs;
    NEXUS_Rect displayRect;
#if NEXUS_NUM_HDMI_OUTPUTS
    struct
    {
        NEXUS_CallbackHandler outputNotifyDisplay;
        /* NEXUS_HdmiOutputHandle */ void *outputNotify;
        void (*rateChangeCb_isr)(NEXUS_DisplayHandle display, void *pParam);
        void (*vsync_isr)(void *pParam);
        void *pCbParam;
        BAVC_VdcDisplay_Info rateInfo;
        bool rateInfoValid;
    } hdmi;
#endif

#if NEXUS_NUM_HDMI_DVO
    struct
    {
        void (*rateChangeCb_isr)(NEXUS_DisplayHandle display, void *pParam);
        void *pCbParam;
        BAVC_VdcDisplay_Info rateInfo;
        bool rateInfoValid;
    } hdmiDvo;
#endif

    struct
    {
        unsigned gfxWidth, gfxHeight, gfxBpp;
        uint32_t specialFeature1;
    } restrictions;
    bool formatChanged;
    struct {
        BINT_CallbackHandle intCallback[3]; /* top, bot, frame */
        NEXUS_IsrCallbackHandle isrCallback;
    } vsyncCallback;
    struct {
        NEXUS_DisplayCrcData *queue;
        unsigned size; /* num entries, not num bytes */
        unsigned rptr, wptr;
    } crc;
    struct {
        BKNI_EventHandle event;
        NEXUS_EventCallbackHandle handler;
    } refreshRate;

#if NEXUS_HAS_VIDEO_ENCODER 
#if !NEXUS_NUM_DSP_VIDEO_ENCODERS
    BXUDlib_Handle hXud;
    bool encodeUserData;
    BXUDlib_Settings userDataEncodeSettings;
#else
    struct {    
        NEXUS_Error (*enqueueCb_isr)(void * context, NEXUS_DisplayCapturedImage *image);
        NEXUS_Error (*dequeueCb_isr)(void * context, NEXUS_DisplayCapturedImage *image);
        void *context;  
        unsigned framesEnqueued;
        unsigned dropRate;
        uint32_t *buf;  
        struct NEXUS_Display_P_ImageQueue free;
        struct NEXUS_Display_P_ImageQueue queued;
        bool callbackEnabled;
    } encoder;
#endif
#endif
};

/*
Display module global variables
**/
extern NEXUS_DisplayModule_State g_NEXUS_DisplayModule_State;
extern NEXUS_ModuleHandle g_NEXUS_displayModuleHandle;

/**
Conversion and helper functions
**/
BERR_Code NEXUS_P_DisplayAspectRatio_ToMagnum(NEXUS_DisplayAspectRatio aspectRatio, NEXUS_VideoFormat videoFormat, BFMT_AspectRatio *maspectRatio);
bool NEXUS_P_Display_RectEqual(const NEXUS_Rect *r1,  const NEXUS_Rect *r2);

BERR_Code NEXUS_Display_P_InitGraphics(NEXUS_DisplayHandle display);
void NEXUS_Display_P_UninitGraphics(NEXUS_DisplayHandle display);
void NEXUS_Display_P_ResetGraphics(NEXUS_DisplayHandle display);
void NEXUS_Display_P_DestroyGraphicsSource(NEXUS_DisplayHandle display);

NEXUS_Error NEXUS_Display_P_GetScalerRect(const NEXUS_VideoWindowSettings  *pSettings, NEXUS_Rect *pRect);

/* Performs apply changes after checking updateMode */
NEXUS_Error NEXUS_Display_P_ApplyChanges(void);

/**
Display VBI functions
**/
BERR_Code NEXUS_Display_P_ConnectVbi(NEXUS_DisplayHandle display);
void NEXUS_Display_P_DisconnectVbi(NEXUS_DisplayHandle display);
void NEXUS_Display_P_DisableVbi(NEXUS_DisplayHandle display);
BERR_Code NEXUS_Display_P_EnableVbi(NEXUS_DisplayHandle display, NEXUS_VideoFormat format);
BERR_Code NEXUS_Display_P_VbiData_isr(NEXUS_DisplayHandle display, BVBI_Field_Handle vbiData);
BVDC_Heap_Handle NEXUS_Display_P_CreateHeap(NEXUS_HeapHandle heap);
void NEXUS_Display_P_DestroyHeap(BVDC_Heap_Handle vdcHeap);

/* Find the window that this input is connected to */
NEXUS_VideoWindowHandle NEXUS_Display_P_FindWindow(NEXUS_VideoInput input);

/* Create VideoInputLink for the videoImage input */
NEXUS_VideoInput_P_Link *NEXUS_VideoImageInput_P_OpenInput(NEXUS_VideoInput input);
void NEXUS_VideoImageInput_P_UpdateDisplayInformation(NEXUS_VideoImageInputHandle imageInput, const NEXUS_DisplaySettings *pSettings);

#if NEXUS_NUM_DISPLAYS > 1
NEXUS_Error NEXUS_Display_P_Align( NEXUS_DisplayHandle display, NEXUS_DisplayHandle target );
#endif

NEXUS_OBJECT_CLASS_DECLARE(NEXUS_Display);
NEXUS_OBJECT_CLASS_DECLARE(NEXUS_HdDviInput);
NEXUS_OBJECT_CLASS_DECLARE(NEXUS_VideoImageInput);
NEXUS_OBJECT_CLASS_DECLARE(NEXUS_Ccir656Input);

#define NEXUS_MODULE_SELF g_NEXUS_displayModuleHandle

#ifdef __cplusplus
}
#endif

#endif /* NEXUS_DISPLAY_MODULE_H__ */
