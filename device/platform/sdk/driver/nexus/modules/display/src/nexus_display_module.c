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
 * $brcm_Workfile: nexus_display_module.c $
 * $brcm_Revision: 128 $
 * $brcm_Date: 10/12/12 3:12p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/display/src/nexus_display_module.c $
 * 
 * 128   10/12/12 3:12p erickson
 * SW7435-419: change proc to BDBG_LOG to that they are still available
 *  with B_REFSW_DEBUG_LEVEL=err
 * 
 * 127   10/10/12 10:32a erickson
 * SW7408-346: support graphics-only build
 * 
 * 126   10/10/12 10:24a erickson
 * SWDEPRECATED-3811: improve proc
 *
 * 125   9/27/12 1:13p erickson
 * SW7435-362: lock module during init and uninit
 *
 * 124   9/6/12 2:05p erickson
 * SW7346-1015: add NEXUS_DisplayModuleSettings.dacDetection
 *
 * 123   8/2/12 2:45p erickson
 * SW7552-309: for a BVDC_ApplyChanges if HDMI format change in process
 *
 * 122   8/1/12 10:22a erickson
 * SW7552-307: back out change
 *
 * 120   7/23/12 10:20a erickson
 * SW7425-3545: move BVBI_TT_Line array off of stack, defer alloc until
 *  first use
 *
 * 119   5/15/12 3:02p erickson
 * SW7346-792: only resolve NEXUS_DisplayAspectRatio_eAuto when converting
 *  to magnum types to remove ambiguity
 *
 * 118   4/4/12 5:16p hongtaoz
 * SW7435-80: disable VDC DVI input if both HDDVI and HDMI input numbers
 *  are 0;
 *
 * 117   4/3/12 11:13a erickson
 * SW7435-80: Reduced BVDC_Open-time allocations from BMEM based on
 *  nexus_platform_features.h
 *
 * 116   3/29/12 3:40p erickson
 * SW7435-77: add NEXUS_DisplayModuleSettings.vbi.allowVps
 *
 * 115   3/27/12 11:59a erickson
 * SW7435-77: add NEXUS_DisplayModuleSettings.vbi.allowTeletext
 *
 * 114   3/14/12 12:25p erickson
 * SW7401-4517: remove old code
 *
 * 113   3/13/12 4:50p erickson
 * SW7425-2607: use unvarying macros in API for binary compatibility
 *
 * 112   3/6/12 4:50p erickson
 * SW7425-2572: add conversion of BFMT_AspectRatio_eUnknown
 *
 * 111   2/21/12 2:19p erickson
 * SW7425-2130: fix debug warnings
 *
 * 110   11/15/11 3:27p erickson
 * SW7425-1747: remove old code
 *
 * 109   10/31/11 7:49p bandrews
 * SW7231-391: merge to main
 *
 * SW7420-2078/1   10/31/11 7:34p bandrews
 * SW7231-391: added dependency on transport for timebase calls
 *
 * 108   10/6/11 3:42p erickson
 * SW7420-2070: add NEXUS_VBI_ENCODER_QUEUE_SIZE macro to control vbilib
 *  allocations using (NEXUS_VBI_ENCODER_QUEUE_SIZE+1)*NEXUS_NUM_DISPLAYS
 *  formula.
 *
 * 107   7/8/11 4:13p erickson
 * SW7425-592: add NEXUS_DisplayAspectRatio_eSar for custom display
 *  formats
 *
 * 106   5/24/11 4:17p erickson
 * SW7422-280: without a video source, nexus graphics should default to
 *  59.94/29.97/23.976 (i.e. drop frame)
 *
 * 105   3/29/11 3:04p erickson
 * SW7420-1671: NEXUS_DisplayModule_Uninit must acquire lock before doing
 *  cleanup
 *
 * 104   2/25/11 4:37p erickson
 * SW7422-255: add AMOL support
 *
 * 103   1/17/11 9:55p jerrylim
 * SW35125-54: Added 35125 cases for VDB calls
 *
 * 102   12/27/10 11:57a erickson
 * SW7422-2: NEXUS_DisplayModule_Init can call
 *  NEXUS_Heap_SetDisplayHeapSettings per heap
 *
 * 101   12/15/10 5:38p vsilyaev
 * SW7405-4996: Revert StrCmp change
 *
 * SW7405-4996_1/1   12/14/10 10:59a calvinho
 * SW7405-4996: Revert StrCmp change
 *
 * SW7420-1177/1   11/5/10 3:28p spothana
 * SW7420-1177 : Add DVO to display dependency list
 *
 * 99   11/18/10 11:04p spothana
 * SW7420-1078: assign heap index per video window
 *
 * 98   11/18/10 4:27p erickson
 * SW7420-1078: SW7420-1078
 *
 * 97   11/17/10 7:12p vsilyaev
 * SW7405-4996: Use strcmp function for string comparison
 *
 * SW7405-4996/1   11/16/10 10:05a calvinho
 * SW7405-4996: Update string compare function calls
 *
 * 96   11/2/10 2:19p vsilyaev
 * SW7422-64: Added API to set 3D framebuffer
 *
 * 95   10/14/10 11:44a jerrylim
 * SW35230-1746: Custom PQ init to be done in app
 *
 * 94   10/12/10 5:00p spothana
 * SW7420-1157 : Correct the HD and 2HD buffer assignment.
 *
 * 93   10/12/10 11:02a spothana
 * SW7420-1157 : Match the VDC memory usage in the reference software to
 *  memory worksheet
 *
 * 92   9/30/10 2:02p petlee
 * SW35230-1001: Integrate latest backlight dimming changes from VDC
 *
 * 91   9/23/10 4:02p jerrylim
 * SW35230-1332: populated RTS features to VDC context
 *
 * 90   9/22/10 4:01p mward
 * SWDEPRECATED-3811: Eliminate warning for unused strings with DEBUG=n.
 *
 * 89   9/9/10 2:42p jerrylim
 * SW35230-1262: Add PQ customization templates and examples
 *
 * 88   8/26/10 3:25p erickson
 * SWDEPRECATED-3811: add more info to NEXUS_DisplayModule_Print
 *
 * 87   8/26/10 2:22p erickson
 * SWDEPRECATED-3811: add NEXUS_DisplayModule_Print
 *
 * 86   8/19/10 11:37a petlee
 * SW35230-760: Add new flag for disabling FRC upon VDC_Open
 *
 * 85   8/16/10 1:53p erickson
 * SWNOOS-419: Fixed GHS compiler error.
 *
 * 84   8/13/10 1:44p petlee
 * SW35230-1001: Integrate new backlight dimming feature from VDC
 *
 * 83   7/19/10 10:33a petlee
 * SW35230-681: Fix logic for panel reversal
 *
 * 82   7/16/10 3:36p petlee
 * SW35230-681: Integrate new panel reversal feature from VDC
 *
 * 81   7/6/10 10:58a erickson
 * SW7405-4047: implement jail_xvd/vdc/xpt/rap using
 *  BMRC_Monitor_JailByFilename
 *
 * 80   6/28/10 12:09p petlee
 * SW35230-106: Add Close for anv
 *
 * 79   6/11/10 3:53p petlee
 * SW35230-249: Merging to main
 *
 * SW35230-202/SW35230-249/6   6/9/10 3:34p petlee
 * SW35230-249: Merge latest mainline updates.
 *
 * SW35230-202/SW35230-249/5   6/2/10 6:28p petlee
 * SW35230-249: call BANV_Open with tmr
 *
 * SW35230-202/SW35230-249/4   6/2/10 3:54p petlee
 * SW35230-249: Undo previous checkin
 *
 * SW35230-202/SW35230-249/3   6/2/10 3:20p petlee
 * SW35230-249: BANV_Open() now requires a hanlde to timer
 *
 * SW35230-202/SW35230-249/2   6/1/10 11:40a petlee
 * SW35230-249: Revert heap settings.
 *
 * SW35230-202/SW35230-249/1   5/30/10 8:28p petlee
 * SW35230-249: Update heap settings for bringup. This is a temp fix .
 *
 * SW35230-202/1   5/21/10 6:03p petlee
 * SW35230-202: Call
 *  Nexus_DisplayModule_P_GetDefaultPanelOutputInitSettings() for 35230.
 *
 * 78   6/2/10 10:17a erickson
 * SW3548-2962: refactor NEXUS_DisplayModule_SetConfigurationId for settop
 *
 * 77   5/13/10 10:37a erickson
 * SW3548-2921: add pq_disabled=y support
 *
 * 76   3/26/10 2:19p erickson
 * SW7405-3965: refactor mosaic to support hd/sd simul mode
 *
 * 75   3/23/10 3:25p petlee
 * SW35230-106: Merging branch SW35230-106 to mainline
 *
 * SW35230-106a/1   3/1/10 2:34p petlee
 * SW35230-106: Add Open for anv
 *
 * 74   1/8/10 3:16p erickson
 * SW3556-979: add NEXUS_DisplayModuleSettings.vbi booleans to alloc
 *  memory for CGMS-B and gemstar VBI types
 *
 * 73   8/19/09 10:21a erickson
 * PR57747: add BDBG_WRN if clipBase.x or .y are non-zero
 *
 * 72   8/10/09 9:30a erickson
 * PR56400: fix missing return value
 *
 * 71   8/10/09 8:17a gmohile
 * PR 56400 : Merge legacy vdc support to main line
 *
 * 70   8/5/09 10:11a erickson
 * PR51648: set default dacBandGapAdjust values from
 *  BVDC_GetDefaultSettings
 *
 * 69   7/24/09 10:48a erickson
 * PR51648: add NEXUS_DisplayModuleSettings.dacBandGapAdjust[]. move env
 *  variable to Settop API shim.
 *
 * 68   7/22/09 11:40a erickson
 * PR56997: separate AnalogVideoDecoder code
 *
 * 67   7/10/09 3:57p erickson
 * PR56558: add NEXUS_DisplayModule_SetVideoDecoderModule to allow faster
 *  start up time
 *
 * 66   6/12/09 10:50a erickson
 * PR55967: convert NEXUS_DisplayModuleSettings heap settings to integers.
 *  This makes them settable in the application.
 *
 * 65   6/8/09 7:06a erickson
 * PR55617: rename NEXUS_P_DisplayAspectRatio_ToMagnum
 *
 * 64   5/28/09 11:06a erickson
 * PR55483: give friendlier error message if NEXUS_DisplayHeapSettings is
 *  not set
 *
 * 63   5/20/09 3:29p erickson
 * PR54880: keep track of whether VDC heaps were created and only destroy
 *  what was created
 *
 * 62   5/20/09 11:29a erickson
 * PR55292: call NEXUS_VideoOutput_P_DestroyLink in
 *  NEXUS_Display_RemoveOutput
 *
 * 61   5/19/09 10:59a erickson
 * PR54880: fix heap handling. don't assume VideoDecoder will return NULL
 *  heap for default.
 *
 * 60   3/30/09 12:53p erickson
 * PR53453: set vbi module defaults using vbi pi
 *
 * 59   3/27/09 10:31a erickson
 * PR48963: add heap and pipHeap to NEXUS_DisplayModuleSettings. allows
 *  for more platform configurability.
 *
 * 58   3/13/09 10:54a erickson
 * PR52722: improve error message on bad heap settings
 *
 * 57   3/6/09 7:37p jgarrett
 * PR 52722: Handling heap size properly
 *
 * 56   3/2/09 6:10p jgarrett
 * PR 52520: Moving scaler settings to common functions for VDB support
 *
 * 55   2/26/09 12:45p katrep
 * PR48984: Use of string.h cause problems in the kernelmode
 *
 * 54   2/25/09 7:15p nickh
 * PR52525: Implement SW workaround for 7420 3D graphics demos
 *
 * 53   2/23/09 6:40p jgarrett
 * PR 48984: Adding no_vdb env variable
 *
 * 52   2/23/09 10:08a jgarrett
 * PR 52266: Adding memory requirements interface
 *
 * 52   2/23/09 10:04a jgarrett
 * PR 52266: Adding memory requirements interface
 *
 * 52   2/20/09 4:49p jgarrett
 * PR 52266: Adding memory requirements interface
 *
 * 51   2/19/09 12:29p mward
 * PR48984: Compiler warning about unused pEnv.
 *
 * 50   2/13/09 11:40a rgreen
 * PR51991: Add support to modify LVDS_CHx_DATA_N_CFG
 *
 * PR51991/1   2/13/09 10:47a rgreen
 * PR51991: Add support to modify LVDS_CHx_DATA_N_CFG
 *
 * 49   2/12/09 5:34p jgarrett
 * PR 51958: Adding vecSwap option
 *
 * 48   2/12/09 4:06p jgarrett
 * PR 48984: Disabling dynamic RTS for kylin (temporary workaround)
 *
 * 47   2/10/09 11:47a jgarrett
 * PR 48984: Incorporating comments from vdb review
 *
 * 46   1/29/09 10:30a erickson
 * PR51648: added DACx_BAND_GAP environment variable for internal testing
 *
 * 45   1/26/09 11:07a erickson
 * PR51468: global variable naming convention
 *
 * 44   1/14/09 2:57p erickson
 * PR51114: must do error checking of pixel and video format conversions
 *
 * 43   1/6/09 11:32a jgarrett
 * PR 48984: Merge dynamic RTS code to main branch
 *
 * 42   12/18/08 3:23p erickson
 * PR50501: fix DTV
 *
 * 41   12/17/08 11:40p erickson
 * PR50501: refactor HdmiOutput to remove double BVDC_ApplyChanges and 400
 *  msec wait on format change
 *
 * PR48984/3   12/5/08 4:23p jgarrett
 * PR 48984: Updating to latest baseline
 *
 * 40   12/3/08 3:48p nickh
 * PR48963: Add support for 7420 main window allocation from heap1 on
 *  MEMC1
 *
 * PR48984/2   11/20/08 7:00p jgarrett
 * PR 48984: Adding check in apply changes for window layout
 *
 * PR48984/1   11/20/08 6:47p jgarrett
 * PR 48984: Adding VDB support
 *
 * 39   11/4/08 12:38p erickson
 * PR47030: add NEXUS_Display_P_DestroyHeap, refactor so that all VDC
 *  Sources are created by NEXUS_VideoInput_P_CreateLink
 *
 * 38   10/31/08 12:00p erickson
 * PR47030: readd additionalLines
 *
 * 37   10/30/08 9:06p erickson
 * PR47030: added DBG
 *
 * 36   10/30/08 8:55p erickson
 * PR47030: manage VDC heap
 *
 * 35   10/17/08 3:23p erickson
 * PR47030: impl shared analog/digital heap
 *
 * 34   10/16/08 12:39p jtna
 * PR47892: Added GemStar encoding interface
 *
 * 33   10/8/08 12:05a erickson
 * PR45121: set default panel.customLinkSettings
 *
 * 32   9/26/08 5:00p katrep
 * PR47370: hdbuffer format should be NEXUS_VideoFormat_e1080i
 *
 * 31   9/22/08 9:54a erickson
 * PR46063: set default lvds color mode
 *
 * 30   9/12/08 9:14a erickson
 * PR46824: added NEXUS_DisplayModuleSettings.vbi.tteShiftDirMsb2Lsb
 *
 * 29   9/3/08 10:24a erickson
 * PR46457: restore DTV functionaltiy
 *
 * 28   9/2/08 7:48p katrep
 * PR46457: Configure nexus features at the run time based on board strap
 *  options
 *
 * 27   8/22/08 10:07p erickson
 * PR45795: increase # of HD buffers for 3548 failure
 *
 * 26   8/22/08 7:32p erickson
 * PR45795: additional fixes
 *
 * 25   8/22/08 7:06p erickson
 * PR45795: replace enum with 2HD/HD/SD buffer settings. this allows user
 *  to fully configure VDC memory allocation.
 *
 * 24   8/7/08 5:56p jrubio
 * PR45387: make sure we can reduce 7325/7335 to SD Display
 *
 * 23   8/4/08 2:31p erickson
 * PR45271: keep track of AnalogVideoDecoder opens. prevent duplicates.
 *  auto-close when DisplayModule closes.
 *
 * 22   7/30/08 11:54a erickson
 * PR44919: cleaned up VDC buffer count logic for 740x, added SD only
 *  display option support
 *
 * 21   7/17/08 4:18p erickson
 * PR44919: remove B_HAS_LEGACY_VDC code, if NEXUS_NUM_DISPLAYS == 1,
 *  allocated fewer picture buffers
 *
 * 20   7/16/08 11:20a erickson
 * PR44853: clean up -Wstrict-prototypes warnings
 *
 * 19   7/10/08 4:46p erickson
 * PR44598: added LVD link settings to NEXUS_DisplayModuleSettings.panel
 *
 * 18   7/7/08 2:40p erickson
 * PR44619: prevent duplicate NEXUS_PanelOutput_Open calls
 *
 * 17   7/3/08 4:22p erickson
 * PR44452: NEXUS_Display_Close on the last display should bring the
 *  display module to a clean state. This makes implicit close for a
 *  kernel mode driver possible.
 *
 * 16   7/1/08 5:03p erickson
 * PR44391: Add RUL Log Capability to Help VDC Debug
 *
 * 15   6/23/08 5:15p vsilyaev
 * PR 40921: Fixed ADC mapping for scart
 *
 * 14   6/23/08 7:19a erickson
 * PR43914: move BLVD_Open before BVDC_Open
 *
 * 13   5/12/08 4:09p erickson
 * PR42628: free NEXUS_VideoOutput_P_Link when DisplayModule closes
 *
 * 12   5/5/08 4:48p erickson
 * PR42445: provide API's for better display frame rate control
 *
 * 11   5/1/08 3:27p jrubio
 * PR42353: add 7335 support
 *
 * 10   4/8/08 3:18p erickson
 * PR39453: add 3548 & 3556
 *
 * 9   4/8/08 1:53p erickson
 * PR39453: add 3548 & 3556
 *
 * 8   4/3/08 10:17a erickson
 * PR41122: move picture quality code to 7400
 *
 * 7   3/27/08 6:20p vsilyaev
 * PR 40818: Added management of VDC heaps
 *
 * 6   3/27/08 2:29p erickson
 * PR40928: implicit NEXUS_Display_Close with WRN
 *
 * 5   3/26/08 1:20p vsilyaev
 * PR 40862: Fixed support for 1080p formats
 *
 * 4   3/4/08 4:36p jrubio
 * PR40169: adding 7325
 *
 * 3   3/3/08 11:27a erickson
 * PR40168: add BBS VideoTool backdoor to VDC
 *
 * 2   2/1/08 5:25p jgarrett
 * PR 39017: Adding HdmiOutput
 *
 * 1   1/18/08 2:20p jgarrett
 * PR 38808: Merging to main branch
 *
 **************************************************************************/
#include "nexus_base.h"
#include "nexus_display_module.h"
#include "priv/nexus_core.h"
#include "bkni.h"

BDBG_MODULE(nexus_display_module);

NEXUS_ModuleHandle g_NEXUS_displayModuleHandle = NULL;
NEXUS_DisplayModule_State g_NEXUS_DisplayModule_State;

void
NEXUS_DisplayModule_GetDefaultSettings(NEXUS_DisplayModuleSettings *pSettings)
{
    BVDC_Settings vdcCfg;
    BVBI_Settings vbiCfg;
    unsigned i;

    BDBG_ASSERT(pSettings);
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));

    BVDC_GetDefaultSettings(&vdcCfg);
    pSettings->vecSwap = vdcCfg.bVecSwap;
    pSettings->dropFrame = true; /* without a video source, nexus graphics should default to 59.94/29.97/23.976 (i.e. drop frame) */

    /* verify API macros aren't below nexus_platform_features.h */
    BDBG_CASSERT(NEXUS_MAX_DISPLAYS >= NEXUS_NUM_DISPLAYS);
    BDBG_CASSERT(NEXUS_MAX_VIDEO_WINDOWS >= NEXUS_NUM_VIDEO_WINDOWS);

    BDBG_CASSERT(BVDC_MAX_DACS == NEXUS_MAX_VIDEO_DACS);
    for (i=0;i<NEXUS_MAX_VIDEO_DACS;i++) {
        pSettings->dacBandGapAdjust[i] = vdcCfg.aulDacBandGapAdjust[i];
    }
    BDBG_CASSERT(NEXUS_VideoDacDetection_eOn == (NEXUS_VideoDacDetection)BVDC_Mode_eOn);
    pSettings->dacDetection = vdcCfg.eDacDetection;

    BVBI_GetDefaultSettings(&vbiCfg);
    pSettings->vbi.tteShiftDirMsb2Lsb = vbiCfg.tteShiftDirMsb2Lsb;
    pSettings->vbi.ccir656InputBufferSize = vbiCfg.in656bufferSize;

#if NEXUS_HAS_DYNAMIC_RTS
    {
        const char *pEnv;
    if ( NULL == NEXUS_GetEnv("no_dynamic_rts") )
    {
        pSettings->handleDynamicRts = true;     /* Expect dynamic RTS by default */
    }
    pEnv = NEXUS_GetEnv("bvn_usage");
    if ( pEnv )
    {
        if ( !BKNI_Memcmp((const void*)pEnv, (const void*)"config", 6) )
        {
            pSettings->configurationId = NEXUS_atoi(pEnv+6);
            BDBG_WRN(("Using BVN Configuration ID %u", pSettings->configurationId));
        }
    }
    else
    {
        pEnv = NEXUS_GetEnv("bvn_config");
        if ( pEnv )
        {
            pSettings->configurationId = NEXUS_atoi(pEnv);
            BDBG_WRN(("Using BVN Configuration ID %u", pSettings->configurationId));
        }
    }
    }
#endif

    /* default formats are based on typical usage. if you require a different use, please modify this in your platform or application. */
    pSettings->primaryDisplayHeapIndex = 0;
    pSettings->fullHdBuffers.format = NEXUS_VideoFormat_e1080p30hz;
    pSettings->fullHdBuffers.pixelFormat = NEXUS_PixelFormat_eY18_Cb8_Y08_Cr8;
    pSettings->hdBuffers.format = NEXUS_VideoFormat_e1080i;
    pSettings->hdBuffers.pixelFormat = NEXUS_PixelFormat_eY18_Cb8_Y08_Cr8;
    pSettings->sdBuffers.format = NEXUS_VideoFormat_ePalG;
    pSettings->sdBuffers.pixelFormat = NEXUS_PixelFormat_eY18_Cb8_Y08_Cr8;

    for(i=0;i<NEXUS_MAX_HEAPS;i++ )
    {
      pSettings->displayHeapSettings[i].fullHdBuffers.format = NEXUS_VideoFormat_e1080p30hz;
      pSettings->displayHeapSettings[i].fullHdBuffers.pixelFormat = NEXUS_PixelFormat_eY18_Cb8_Y08_Cr8;
      pSettings->displayHeapSettings[i].hdBuffers.format = NEXUS_VideoFormat_e1080i;
      pSettings->displayHeapSettings[i].hdBuffers.pixelFormat = NEXUS_PixelFormat_eY18_Cb8_Y08_Cr8;
      pSettings->displayHeapSettings[i].sdBuffers.format = NEXUS_VideoFormat_ePalG;
      pSettings->displayHeapSettings[i].sdBuffers.pixelFormat = NEXUS_PixelFormat_eY18_Cb8_Y08_Cr8;
    }

    return;
}

BERR_Code
NEXUS_P_DisplayAspectRatio_ToMagnum(NEXUS_DisplayAspectRatio aspectRatio, NEXUS_VideoFormat format, BFMT_AspectRatio *maspectRatio)
{
    switch (aspectRatio) {
    case NEXUS_DisplayAspectRatio_eAuto:
        {
        NEXUS_VideoFormatInfo formatInfo;
        NEXUS_VideoFormat_GetInfo(format, &formatInfo);
        *maspectRatio = (formatInfo.width > 720) ? BFMT_AspectRatio_e16_9 : BFMT_AspectRatio_e4_3;
        }
        break;
    case NEXUS_DisplayAspectRatio_e4x3:
        *maspectRatio = BFMT_AspectRatio_e4_3;
        break;
    case NEXUS_DisplayAspectRatio_e16x9:
        *maspectRatio = BFMT_AspectRatio_e16_9;
        break;
    case NEXUS_DisplayAspectRatio_eSar:
        *maspectRatio = BFMT_AspectRatio_eSAR;
        break;
    default:
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }
    return 0;
}

bool
NEXUS_P_Display_RectEqual(const NEXUS_Rect *r1,  const NEXUS_Rect *r2)
{
    BDBG_ASSERT(r1);
    BDBG_ASSERT(r2);
    return r1->x == r2->x && r1->width == r2->width && r1->y == r2->y && r1->height == r2->height;
}

static const NEXUS_Rect NEXUS_P_EmptyRect = {0,0,0,0};

NEXUS_Error NEXUS_Display_P_GetScalerRect(const NEXUS_VideoWindowSettings *pSettings, NEXUS_Rect *pRect)
{
    /* if the user set clipBase.x and .y, they probably don't understand the API. put out this warning so we can catch problems. */
    if (pSettings->clipBase.x || pSettings->clipBase.y) {
        BDBG_WRN(("clipBase.x and .y are unused but clipBase was set to %d,%d,%d,%d.", pSettings->clipBase.x, pSettings->clipBase.y, pSettings->clipBase.width, pSettings->clipBase.height));
    }

    if (NEXUS_P_Display_RectEqual(&NEXUS_P_EmptyRect, &pSettings->clipRect)) {
        /* no clipping */
        pRect->x = pRect->y = 0;
        pRect->width = pSettings->position.width;
        pRect->height = pSettings->position.height;
    }
    else if (pSettings->clipBase.width == 0 && pSettings->clipBase.height == 0) {
        /* if clipBase width & height are 0, pass clipRect directly into BVDC_Window_SetScalerOutput. This avoids
        the clipBase/clipRect transformation. for now, this is an undocumented feature until its usage can be clarified. */
        *pRect = pSettings->clipRect;
    }
    else {
        unsigned x,y, width, height;
        if(pSettings->clipRect.x < 0 ||  pSettings->clipRect.y < 0  ||
           pSettings->clipRect.width == 0 || pSettings->clipRect.height == 0 ||
           pSettings->clipBase.width == 0 || pSettings->clipBase.height == 0 ||
           pSettings->clipRect.width+pSettings->clipRect.x > pSettings->clipBase.width || pSettings->clipRect.height+pSettings->clipRect.y > pSettings->clipBase.height) {
            pRect->x = pRect->y = 0;
            pRect->width = pSettings->position.width;
            pRect->height = pSettings->position.height;
            return BERR_TRACE(BERR_INVALID_PARAMETER);
        }
        x = (((unsigned)pSettings->position.width)*pSettings->clipRect.x)/pSettings->clipRect.width;
        y = (((unsigned)pSettings->position.height)*pSettings->clipRect.y)/pSettings->clipRect.height;
        width = (((unsigned)pSettings->position.width)*pSettings->clipBase.width)/pSettings->clipRect.width;
        height = (((unsigned)pSettings->position.height)*pSettings->clipBase.height)/pSettings->clipRect.height;

        pRect->x = x;
        pRect->y = y;
        pRect->width = width;
        pRect->height = height;
    }

    return NEXUS_SUCCESS;
}

#ifdef BRDC_USE_CAPTURE_BUFFER
#include <stdio.h>
#include "brdc_dbg.h"
void NEXUS_Display_P_RdcReadCapture(void *data)
{
    BRDC_Handle rdc = (BRDC_Handle)data;
    int total;
    uint8_t mem[4096];
    static FILE *file = NULL;
    static int filesize = 0;
    const char *filename = NEXUS_GetEnv("capture_ruls");

    BDBG_ASSERT(filename);
    if (!file) {
        static int filecnt = 0;
        char buf[256];
        BKNI_Snprintf(buf, 256, "%s%d", filename, filecnt++);
        file = fopen(buf, "w+");
    }

    do {
        BKNI_EnterCriticalSection();
        BRDC_DBG_ReadCapture_isr(rdc, mem, 4096, &total);
        BKNI_LeaveCriticalSection();
        if (total) {
            fwrite(mem, total, 1, file);
            filesize += total;
        }
   } while (total);

   if (filesize >= 100*1024*1024) {
       fclose(file);
       file = NULL;
       filesize = 0;
       /* on the next write, a new file will be opened. */
   }

    NEXUS_ScheduleTimer(1000, NEXUS_Display_P_RdcReadCapture, rdc);
}
#endif

void NEXUS_DisplayModule_Print(void)
{
    #if !BDBG_NO_MSG
    unsigned i;
    NEXUS_VideoInput_P_Link *input;

    BDBG_LOG(("DisplayModule:"));
    for (i=0;i<NEXUS_NUM_DISPLAYS;i++) {
        unsigned j;
        NEXUS_VideoOutput_P_Link *output;
        NEXUS_DisplayHandle display = g_NEXUS_DisplayModule_State.displays[i];
        NEXUS_VideoFormatInfo formatInfo;

        if (!display) continue;

        NEXUS_VideoFormat_GetInfo(display->cfg.format, &formatInfo);

        BDBG_LOG(("display %d: format=%dx%d%c(%d) %d.%02dhz", i,
            display->displayRect.width,display->displayRect.height,formatInfo.interlaced?'i':'p', display->cfg.format,
            display->status.refreshRate/1000, display->status.refreshRate%1000));
        BDBG_LOG(("  graphics: %s fb=%p %dx%d pixelFormat=%d", display->graphics.windowVdc?"enabled":"disabled", display->graphics.frameBuffer3D.main,
            display->graphics.frameBufferWidth, display->graphics.frameBufferHeight, display->graphics.frameBufferPixelFormat));

        for (j=0;j<NEXUS_NUM_VIDEO_WINDOWS;j++) {
            NEXUS_VideoWindowHandle window = &display->windows[j];
            if (!window->open) continue;
            BDBG_LOG(("  window %d: visible=%d, position=%d,%d,%d,%d, NEXUS_VideoInput=%p",
                window->windowId,
                window->cfg.visible,
                window->cfg.position.x, window->cfg.position.y, window->cfg.position.width, window->cfg.position.height,
                window->input));
        }

        for (output=BLST_D_FIRST(&display->outputs);output;output=BLST_D_NEXT(output, link)) {
            static const char *str[] = {"eComposite", "eSvideo", "eComponent", "ePanel", "eRfm", "eHdmi", "eHdmiDvo", "eScartInput", "eCcir656"};
            BDBG_LOG(("  output %p: %s", output, str[output->output->type]));
        }
    }

    for (input=BLST_S_FIRST(&g_NEXUS_DisplayModule_State.inputs);input;input=BLST_S_NEXT(input, link)) {
        static const char *str[] = {"eDecoder", "eCcir656", "eAnalogVideoDecoder", "ePc", "eComposite", "eSvideo", "eComponent", "eIfd", "eHdmi", "eImage", "eScartInput", "eHdDvi"};

        char buf[32];
        if (input->id <= BAVC_SourceId_eMpegMax) {
            BKNI_Snprintf(buf, 32, "eMpeg%d", input->id);
        }
        else if (input->id <= BAVC_SourceId_eGfxMax) {
            BKNI_Snprintf(buf, 32, "eGfx%d", input->id-BAVC_SourceId_eMpegMax);
        }
        else if (input->id <= BAVC_SourceId_eVfdMax) {
            BKNI_Snprintf(buf, 32, "eVfd%d", input->id-BAVC_SourceId_eGfxMax);
        }
        else {
            BKNI_Snprintf(buf, 32, "BAVC_SourceId %d", input->id);
        }
        BDBG_LOG(("NEXUS_VideoInput %p: link=%p, %s, %s, ref_cnt=%d",
            input->input, input, str[input->input->type], buf, input->ref_cnt));
    }
    #endif

}

NEXUS_ModuleHandle
NEXUS_DisplayModule_Init(const NEXUS_DisplayModuleSettings *pSettings)
{
    NEXUS_ModuleSettings moduleSettings;
    BERR_Code rc;
    BVDC_Settings vdcCfg;
    NEXUS_DisplayModule_State *state = &g_NEXUS_DisplayModule_State;
    BPXL_Format mPixelFormat;
    BFMT_VideoFmt mVideoFormat;
    unsigned i;

    if(!pSettings) {
        rc = BERR_TRACE(BERR_INVALID_PARAMETER);
        goto err_settings;
    }

    if ( pSettings->handleDynamicRts && pSettings->configurationId == 0 )
    {
        BDBG_ERR(("Configuration ID not specified for dynamic RTS."));
        BDBG_ERR(("Export bvn_usage=configN where N=1 for WXGA, 2 for Full-HD, etc."));
        BDBG_ERR(("Please see the usage modes spreadsheet for more information."));
        BDBG_ASSERT(pSettings->configurationId != 0);
        goto err_settings;
    }

    BKNI_Memset(state->vdcHeapMap, 0, sizeof(state->vdcHeapMap));
    state->moduleSettings = *pSettings;
    state->pqDisabled = (bool)NEXUS_GetEnv("pq_disabled");

    if (pSettings->modules.surface == NULL) {
        rc = BERR_TRACE(BERR_INVALID_PARAMETER);
        goto err_settings;
    }
#if NEXUS_NUM_HDMI_INPUTS
    if (pSettings->modules.hdmiInput == NULL) {
        rc = BERR_TRACE(BERR_INVALID_PARAMETER);
        goto err_settings;
    }
#endif
#if NEXUS_HAS_TRANSPORT
    if (pSettings->modules.transport == NULL) {
        rc = BERR_TRACE(BERR_INVALID_PARAMETER);
        goto err_settings;
    }
#endif

    NEXUS_Module_GetDefaultSettings(&moduleSettings);
    /* default priority */
    moduleSettings.dbgPrint = NEXUS_DisplayModule_Print;
    moduleSettings.dbgModules = "nexus_display_module";
    g_NEXUS_displayModuleHandle = NEXUS_Module_Create("display", &moduleSettings);
    if(!g_NEXUS_displayModuleHandle) { rc = BERR_TRACE(BERR_OS_ERROR); goto err_module; }
    NEXUS_LockModule();

    if (pSettings->modules.videoDecoder) {
        NEXUS_UseModule(pSettings->modules.videoDecoder);
    }
    NEXUS_UseModule(pSettings->modules.surface);
#if NEXUS_NUM_HDMI_INPUTS
    NEXUS_UseModule(pSettings->modules.hdmiInput);
#endif
#if NEXUS_NUM_HDMI_OUTPUTS
    NEXUS_UseModule(pSettings->modules.hdmiOutput);
#endif
#if NEXUS_NUM_HDMI_DVO
    NEXUS_UseModule(pSettings->modules.hdmiDvo);
#endif
#if NEXUS_HAS_TRANSPORT
    NEXUS_UseModule(pSettings->modules.transport);
#endif
    BLST_S_INIT(&state->inputs);
    state->updateMode = NEXUS_DisplayUpdateMode_eAuto;
    state->lastUpdateFailed = false;
    state->modules = pSettings->modules;
    for(i=0;i<sizeof(state->displays)/sizeof(state->displays[0]);i++) {
        state->displays[i] = NULL;
    }

    /* get heap settings from NEXUS_DisplayModuleSettings and apply to individual nexus heaps */
    for (i=0;i<NEXUS_MAX_HEAPS;i++) {
        NEXUS_HeapHandle heap = g_pCoreHandles->nexusHeap[i];
        if (heap) {
            NEXUS_Heap_SetDisplayHeapSettings(heap, &pSettings->displayHeapSettings[i]);
        }
    }

    rc = BRDC_Open(&state->rdc, g_pCoreHandles->chp, g_pCoreHandles->reg, g_pCoreHandles->heap[0], NULL);
    if(rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc); goto err_rdc; }

#ifdef BRDC_USE_CAPTURE_BUFFER
    if (NEXUS_GetEnv("capture_ruls")) {
        NEXUS_ScheduleTimer(1000, NEXUS_Display_P_RdcReadCapture, state->rdc);
    }
#endif

    /* open VDC */
    BVDC_GetDefaultSettings(&vdcCfg);

#if (BCHP_CHIP == 35230 || BCHP_CHIP == 35125 || BCHP_CHIP == 35121 || BCHP_CHIP == 35330)
    /* In DTV chips, VDC modules need information about global features of the RTS
     * configuration which is currently being used. To allow this VDB must be
     * initalize prior to VDC open so that each VDC module can correctly get
     * configured in VDC Open based on the features that the current RTS
     * supports */
    rc = NEXUS_Vdb_P_Init();
    if ( rc )
    {
        (void) BERR_TRACE(rc);
        goto err_vdb;
    }

    rc = NEXUS_vdb_P_GetRtsConfigFeatures(&vdcCfg.stRtsConfigFeatures);
    if ( rc )
    {
        (void) BERR_TRACE(rc);
        goto err_vdb;
    }
#endif

    vdcCfg.bVecSwap = pSettings->vecSwap;

    /*
     * For platforms with single VDC heap,check for the  HD and SD buffers count.
     * If all the buffer counts are zero, then the platform uses multiple VDC heaps.
     */
    if(pSettings->fullHdBuffers.count || pSettings->hdBuffers.count || pSettings->sdBuffers.count ||
       pSettings->fullHdBuffers.pipCount || pSettings->hdBuffers.pipCount || pSettings->sdBuffers.pipCount){
        (void)NEXUS_P_PixelFormat_ToMagnum(pSettings->fullHdBuffers.pixelFormat, &mPixelFormat);
        (void)NEXUS_P_VideoFormat_ToMagnum(pSettings->fullHdBuffers.format, &mVideoFormat);
        vdcCfg.stHeapSettings.ulBufferCnt_2HD = pSettings->fullHdBuffers.count;
        vdcCfg.stHeapSettings.ulBufferCnt_2HD_Pip = pSettings->fullHdBuffers.pipCount;
        vdcCfg.stHeapSettings.ePixelFormat_2HD = mPixelFormat;
        vdcCfg.stHeapSettings.eBufferFormat_2HD = mVideoFormat;
        vdcCfg.stHeapSettings.ulAdditionalLines_2HD = pSettings->fullHdBuffers.additionalLines;

        (void)NEXUS_P_PixelFormat_ToMagnum(pSettings->hdBuffers.pixelFormat, &mPixelFormat);
        (void)NEXUS_P_VideoFormat_ToMagnum(pSettings->hdBuffers.format, &mVideoFormat);
        vdcCfg.stHeapSettings.ulBufferCnt_HD = pSettings->hdBuffers.count;
        vdcCfg.stHeapSettings.ulBufferCnt_HD_Pip = pSettings->hdBuffers.pipCount;
        vdcCfg.stHeapSettings.ePixelFormat_HD = mPixelFormat;
        vdcCfg.stHeapSettings.eBufferFormat_HD = mVideoFormat;
        vdcCfg.stHeapSettings.ulAdditionalLines_HD = pSettings->hdBuffers.additionalLines;

        /* set default format based on hdBuffer format */
        vdcCfg.eVideoFormat = mVideoFormat;

        (void)NEXUS_P_PixelFormat_ToMagnum(pSettings->sdBuffers.pixelFormat, &mPixelFormat);
        (void)NEXUS_P_VideoFormat_ToMagnum(pSettings->sdBuffers.format, &mVideoFormat);
        vdcCfg.stHeapSettings.ulBufferCnt_SD = pSettings->sdBuffers.count;
        vdcCfg.stHeapSettings.ulBufferCnt_SD_Pip = pSettings->sdBuffers.pipCount;
        vdcCfg.stHeapSettings.ePixelFormat_SD = mPixelFormat;
        vdcCfg.stHeapSettings.eBufferFormat_SD = mVideoFormat;
        vdcCfg.stHeapSettings.ulAdditionalLines_SD = pSettings->sdBuffers.additionalLines;
    }
    else{

        unsigned j, heapIndex = pSettings->primaryDisplayHeapIndex;

        for (i=0;i<NEXUS_NUM_DISPLAYS;i++) {
            for(j=0;j<NEXUS_NUM_VIDEO_WINDOWS;j++){
                if (pSettings->videoWindowHeapIndex[i][j] >= NEXUS_MAX_HEAPS || !g_pCoreHandles->nexusHeap[pSettings->videoWindowHeapIndex[i][j]]) {
                    rc = BERR_TRACE(NEXUS_INVALID_PARAMETER);
                    goto err_vdc;
                }
            }
        }

        (void)NEXUS_P_PixelFormat_ToMagnum(pSettings->displayHeapSettings[heapIndex].fullHdBuffers.pixelFormat, &mPixelFormat);
        (void)NEXUS_P_VideoFormat_ToMagnum(pSettings->displayHeapSettings[heapIndex].fullHdBuffers.format, &mVideoFormat);
        vdcCfg.stHeapSettings.ulBufferCnt_2HD = pSettings->displayHeapSettings[heapIndex].fullHdBuffers.count;
        vdcCfg.stHeapSettings.ulBufferCnt_2HD_Pip = pSettings->displayHeapSettings[heapIndex].fullHdBuffers.pipCount;
        vdcCfg.stHeapSettings.ePixelFormat_2HD = mPixelFormat;
        vdcCfg.stHeapSettings.eBufferFormat_2HD = mVideoFormat;
        vdcCfg.stHeapSettings.ulAdditionalLines_2HD = pSettings->displayHeapSettings[heapIndex].fullHdBuffers.additionalLines;
        (void)NEXUS_P_PixelFormat_ToMagnum(pSettings->displayHeapSettings[heapIndex].hdBuffers.pixelFormat, &mPixelFormat);
        (void)NEXUS_P_VideoFormat_ToMagnum(pSettings->displayHeapSettings[heapIndex].hdBuffers.format, &mVideoFormat);
        vdcCfg.stHeapSettings.ulBufferCnt_HD = pSettings->displayHeapSettings[heapIndex].hdBuffers.count;
        vdcCfg.stHeapSettings.ulBufferCnt_HD_Pip =pSettings->displayHeapSettings[heapIndex].hdBuffers.pipCount;
        vdcCfg.stHeapSettings.ePixelFormat_HD = mPixelFormat;
        vdcCfg.stHeapSettings.eBufferFormat_HD = mVideoFormat;
        vdcCfg.stHeapSettings.ulAdditionalLines_HD =pSettings->displayHeapSettings[heapIndex].hdBuffers.additionalLines;

        /* set default format based on hdBuffer format */
        vdcCfg.eVideoFormat = mVideoFormat;

        (void)NEXUS_P_PixelFormat_ToMagnum(pSettings->displayHeapSettings[heapIndex].sdBuffers.pixelFormat, &mPixelFormat);
        (void)NEXUS_P_VideoFormat_ToMagnum(pSettings->displayHeapSettings[heapIndex].sdBuffers.format, &mVideoFormat);
        vdcCfg.stHeapSettings.ulBufferCnt_SD = pSettings->displayHeapSettings[heapIndex].sdBuffers.count;
        vdcCfg.stHeapSettings.ulBufferCnt_SD_Pip = pSettings->displayHeapSettings[heapIndex].sdBuffers.pipCount;
        vdcCfg.stHeapSettings.ePixelFormat_SD = mPixelFormat;
        vdcCfg.stHeapSettings.eBufferFormat_SD = mVideoFormat;
        vdcCfg.stHeapSettings.ulAdditionalLines_SD = pSettings->displayHeapSettings[heapIndex].sdBuffers.additionalLines;
    }

    BDBG_MSG(("Creating main VDC heap: SD %u, HD %u, 2HD %u, SD_PIP %u, HD_PIP %u, 2HD_PIP %u ",
              vdcCfg.stHeapSettings.ulBufferCnt_SD, vdcCfg.stHeapSettings.ulBufferCnt_HD,
              vdcCfg.stHeapSettings.ulBufferCnt_2HD,vdcCfg.stHeapSettings.ulBufferCnt_SD_Pip,
              vdcCfg.stHeapSettings.ulBufferCnt_HD_Pip,vdcCfg.stHeapSettings.ulBufferCnt_2HD_Pip));

    vdcCfg.eDisplayFrameRate = pSettings->dropFrame ? BAVC_FrameRateCode_e59_94 : BAVC_FrameRateCode_e60;

    BDBG_CASSERT(BVDC_MAX_DACS == NEXUS_MAX_VIDEO_DACS);
    for (i=0;i<NEXUS_MAX_VIDEO_DACS;i++) {
        vdcCfg.aulDacBandGapAdjust[i] = pSettings->dacBandGapAdjust[i];
    }
    vdcCfg.eDacDetection = pSettings->dacDetection;

    if (pSettings->primaryDisplayHeapIndex >= NEXUS_MAX_HEAPS || !g_pCoreHandles->nexusHeap[pSettings->primaryDisplayHeapIndex]) {
        rc = BERR_TRACE(NEXUS_INVALID_PARAMETER);
        goto err_vdc;
    }

    state->heap = g_pCoreHandles->nexusHeap[pSettings->primaryDisplayHeapIndex];

    if ( NEXUS_GetEnv("jail_vdc") ) {
        NEXUS_MemoryStatus memStatus;
        (void)NEXUS_Heap_GetStatus(state->heap, &memStatus);
        BMRC_Monitor_JailByFilename(g_pCoreHandles->memc[memStatus.memcIndex].rmm, "bvdc_");
        BMRC_Monitor_JailUpdate(g_pCoreHandles->memc[memStatus.memcIndex].rmm);
    }

#if !NEXUS_NUM_MOSAIC_DECODES
    vdcCfg.bDisableMosaic = true;
#endif
#if !NEXUS_NUM_656_INPUTS
    vdcCfg.bDisable656Input = true;
#endif
#if !NEXUS_NUM_HDDVI_INPUTS && !NEXUS_NUM_HDMI_INPUTS
    vdcCfg.bDisableHddviInput = true;
#endif

    rc = BVDC_Open(&state->vdc, g_pCoreHandles->chp, g_pCoreHandles->reg, NEXUS_Heap_GetMemHandle(state->heap), g_pCoreHandles->bint, state->rdc, g_pCoreHandles->tmr, &vdcCfg);
    if(rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc); goto err_vdc; }

    NEXUS_VideoOutputs_P_Init();

#if B_HAS_VBI
    {
        BVBIlib_List_Settings vbilistsettings;
        BVBI_Settings vbiSettings;

        BVBI_GetDefaultSettings(&vbiSettings);
        vbiSettings.tteShiftDirMsb2Lsb = pSettings->vbi.tteShiftDirMsb2Lsb;
        vbiSettings.in656bufferSize = pSettings->vbi.ccir656InputBufferSize;
        BDBG_MSG(("BVBI_Open"));
        rc = BVBI_Open(&state->vbi, g_pCoreHandles->chp, g_pCoreHandles->reg, g_pCoreHandles->heap[0], &vbiSettings);
        if(rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc); goto err_vbi; }

        rc = BVBIlib_Open (&state->vbilib, state->vbi);
        if(rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc); goto err_vbilib; }

        BVBIlib_List_GetDefaultSettings(&vbilistsettings);
        vbilistsettings.bAllowTeletext = pSettings->vbi.allowTeletext;
        vbilistsettings.bAllowVPS = pSettings->vbi.allowVps;
        vbilistsettings.bAllowGemstar = pSettings->vbi.allowGemStar;
        vbilistsettings.bAllowCgmsB = pSettings->vbi.allowCgmsB;
        vbilistsettings.bAllowAmol = pSettings->vbi.allowAmol;
        rc = BVBIlib_List_Create(&state->vbilist, state->vbi,
            (NEXUS_VBI_ENCODER_QUEUE_SIZE+1)*NEXUS_NUM_DISPLAYS, /* number of entries.
                The +1 is because of vbilib internal resource management.
                The *NEXUS_NUM_DISPLAYS is for each BVBIlib_Encode_Create */
            &vbilistsettings);
        if(rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc); goto err_vbilist; }
    }
#endif

#if !(BCHP_CHIP == 35230 || BCHP_CHIP == 35125 || BCHP_CHIP == 35121 || BCHP_CHIP == 35330)
    rc = NEXUS_Vdb_P_Init();
    if ( rc )
    {
        (void)BERR_TRACE(rc);
        goto err_vdb;
    }
#endif

#if NEXUS_NUM_MOSAIC_DECODES
    for (i=0;i<NEXUS_NUM_MOSAIC_DECODE_SETS;i++) {
        NEXUS_VIDEO_INPUT_INIT(&state->mosaicInput[i].input, NEXUS_VideoInputType_eDecoder, NULL);
    }
#endif

    NEXUS_UnlockModule();
    return g_NEXUS_displayModuleHandle;

err_vdb:
#if B_HAS_VBI
    BVBIlib_List_Destroy(state->vbilist);
err_vbilist:
    BVBIlib_Close(state->vbilib);
err_vbilib:
    BVBI_Close(state->vbi);
err_vbi:
#endif
    BVDC_Close(state->vdc);
err_vdc:
    BRDC_Close(state->rdc);
err_rdc:
    NEXUS_UnlockModule();
    NEXUS_Module_Destroy(g_NEXUS_displayModuleHandle);
err_module:
err_settings:
    return NULL;
}

void NEXUS_DisplayModule_GetSettings( NEXUS_DisplayModuleSettings *pSettings )
{
    *pSettings = g_NEXUS_DisplayModule_State.moduleSettings;
}

void NEXUS_DisplayModule_Uninit(void)
{
    NEXUS_DisplayModule_State *state = &g_NEXUS_DisplayModule_State;
    BERR_Code rc;
    unsigned i;

    /* auto cleanup must run with module lock held */
    NEXUS_LockModule();
    for (i=0;i<sizeof(state->displays)/sizeof(state->displays[0]);i++) {
        if (state->displays[i]) {
            BDBG_WRN(("Implicit NEXUS_Display_Close(%d) called", i));
            NEXUS_Display_Close(state->displays[i]);
        }
    }
#if NEXUS_NUM_ANALOG_VIDEO_DECODERS
    NEXUS_AnalogVideoDecoder_P_AutoClose();
#endif

    /* closing all displays should destroy all inputs and outputs */
    BDBG_ASSERT(!BLST_S_FIRST(&state->inputs));

#if B_HAS_VBI
    BVBIlib_List_Destroy(state->vbilist);
    BVBIlib_Close(state->vbilib);
    BVBI_Close(state->vbi);
    if (g_NEXUS_DisplayModule_State.ttLines) {
        BKNI_Free(g_NEXUS_DisplayModule_State.ttLines);
        g_NEXUS_DisplayModule_State.ttLines = NULL;
    }
#endif

#if NEXUS_DISPLAY_EXTENSION_PQ_CUSTOMIZATION
    rc = NEXUS_CustomPq_Uninit();
    if (rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc);}
#endif

    rc=BVDC_Close(state->vdc);
    if (rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc); }
    rc=BRDC_Close(state->rdc);
    if (rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc); }

    NEXUS_Vdb_P_Uninit();

    for (i=0;i<MAX_VDC_HEAPS;i++) {
        if (state->vdcHeapMap[i].refcnt) {
            BDBG_ERR(("VDC heap %d not destroyed. Internal state problem.", i));
        }
    }

    NEXUS_UnlockModule();
    NEXUS_Module_Destroy(g_NEXUS_displayModuleHandle);
    return;
}

NEXUS_Error NEXUS_Display_P_ApplyChanges(void)
{
    if (g_NEXUS_DisplayModule_State.updateMode==NEXUS_DisplayUpdateMode_eAuto
#if NEXUS_NUM_HDMI_OUTPUTS
        || g_NEXUS_DisplayModule_State.hdmiOutput.display
#endif
        )
    {
        BERR_Code rc;
        rc = NEXUS_Vdb_P_CheckLayout();
        if ( rc )
        {
            (void)BERR_TRACE(rc);
        }
        rc = BVDC_ApplyChanges(g_NEXUS_DisplayModule_State.vdc);
        if (rc) return BERR_TRACE(rc);

#if NEXUS_NUM_HDMI_OUTPUTS
        if (g_NEXUS_DisplayModule_State.hdmiOutput.display) {
            NEXUS_VideoOutput_P_PostSetHdmiFormat();
        }
#endif
    }
    return 0;
}

void NEXUS_Display_P_DestroyHeap(BVDC_Heap_Handle vdcHeap)
{
    NEXUS_DisplayModule_State *state = &g_NEXUS_DisplayModule_State;
    unsigned i;

    for (i=0;i<MAX_VDC_HEAPS;i++) {
        if (state->vdcHeapMap[i].vdcHeap == vdcHeap) {
            BDBG_ASSERT(state->vdcHeapMap[i].refcnt);
            if (--state->vdcHeapMap[i].refcnt == 0) {
                BVDC_Heap_Destroy(state->vdcHeapMap[i].vdcHeap);
                state->vdcHeapMap[i].vdcHeap = NULL;
                state->vdcHeapMap[i].nexusHeap = NULL;
            }
            return;
        }
    }
    BDBG_ERR(("Couldn't find heap"));
    BDBG_ASSERT(0);
}

BVDC_Heap_Handle NEXUS_Display_P_CreateHeap(NEXUS_HeapHandle heap)
{
    BVDC_Settings vdcCfg;
    BERR_Code rc;
    BPXL_Format mPixelFormat;
    BFMT_VideoFmt mVideoFormat;
    NEXUS_DisplayHeapSettings displayHeapSettings;
    int i, openSlot = -1;
    NEXUS_DisplayModule_State *state = &g_NEXUS_DisplayModule_State;

    NEXUS_Heap_GetDisplayHeapSettings(heap, &displayHeapSettings);

    if (!displayHeapSettings.fullHdBuffers.pixelFormat &&
        !displayHeapSettings.hdBuffers.pixelFormat &&
        !displayHeapSettings.sdBuffers.pixelFormat)
    {
        BDBG_ERR(("NEXUS_DisplayHeapSettings have not been set for heap %p", heap));
        return NULL;
    }

    for (i=0;i<MAX_VDC_HEAPS;i++) {
        if (state->vdcHeapMap[i].nexusHeap == heap) {
            BDBG_ASSERT(state->vdcHeapMap[i].vdcHeap);
            BDBG_ASSERT(state->vdcHeapMap[i].refcnt);
            BDBG_MSG(("Reusing VDC heap %d", i));
            state->vdcHeapMap[i].refcnt++;
            return state->vdcHeapMap[i].vdcHeap;
        }
        else if (openSlot == -1 && !state->vdcHeapMap[i].nexusHeap) {
            openSlot = i;
            BDBG_ASSERT(!state->vdcHeapMap[i].vdcHeap);
            BDBG_ASSERT(!state->vdcHeapMap[i].refcnt);
        }
    }
    if (openSlot == -1) {
        BDBG_ERR(("Increase MAX_VDC_HEAPS"));
        return NULL;
    }

    BVDC_GetDefaultSettings(&vdcCfg);

    rc = NEXUS_P_PixelFormat_ToMagnum(displayHeapSettings.fullHdBuffers.pixelFormat, &mPixelFormat);
    if (rc) {rc = BERR_TRACE(rc);goto error;}
    rc = NEXUS_P_VideoFormat_ToMagnum(displayHeapSettings.fullHdBuffers.format, &mVideoFormat);
    if (rc) {rc = BERR_TRACE(rc);goto error;}
    vdcCfg.stHeapSettings.ePixelFormat_2HD = mPixelFormat;
    vdcCfg.stHeapSettings.eBufferFormat_2HD = mVideoFormat;
    vdcCfg.stHeapSettings.ulBufferCnt_2HD = displayHeapSettings.fullHdBuffers.count;
    vdcCfg.stHeapSettings.ulBufferCnt_2HD_Pip = displayHeapSettings.fullHdBuffers.pipCount;
    vdcCfg.stHeapSettings.ulAdditionalLines_2HD = displayHeapSettings.fullHdBuffers.additionalLines;

    rc = NEXUS_P_PixelFormat_ToMagnum(displayHeapSettings.hdBuffers.pixelFormat, &mPixelFormat);
    if (rc) {rc = BERR_TRACE(rc);goto error;}
    rc = NEXUS_P_VideoFormat_ToMagnum(displayHeapSettings.hdBuffers.format, &mVideoFormat);
    if (rc) {rc = BERR_TRACE(rc);goto error;}
    vdcCfg.stHeapSettings.ePixelFormat_HD = mPixelFormat;
    vdcCfg.stHeapSettings.eBufferFormat_HD = mVideoFormat;
    vdcCfg.stHeapSettings.ulBufferCnt_HD = displayHeapSettings.hdBuffers.count;
    vdcCfg.stHeapSettings.ulBufferCnt_HD_Pip = displayHeapSettings.hdBuffers.pipCount;
    vdcCfg.stHeapSettings.ulAdditionalLines_HD = displayHeapSettings.hdBuffers.additionalLines;

    rc = NEXUS_P_PixelFormat_ToMagnum(displayHeapSettings.sdBuffers.pixelFormat, &mPixelFormat);
    if (rc) {rc = BERR_TRACE(rc);goto error;}
    rc = NEXUS_P_VideoFormat_ToMagnum(displayHeapSettings.sdBuffers.format, &mVideoFormat);
    if (rc) {rc = BERR_TRACE(rc);goto error;}
    vdcCfg.stHeapSettings.ePixelFormat_SD = mPixelFormat;
    vdcCfg.stHeapSettings.eBufferFormat_SD = mVideoFormat;
    vdcCfg.stHeapSettings.ulBufferCnt_SD = displayHeapSettings.sdBuffers.count;
    vdcCfg.stHeapSettings.ulBufferCnt_SD_Pip = displayHeapSettings.sdBuffers.pipCount;
    vdcCfg.stHeapSettings.ulAdditionalLines_SD = displayHeapSettings.sdBuffers.additionalLines;

    BDBG_MSG(("Creating VDC heap %d: SD %u, HD %u, 2HD %u, SD_PIP %u, HD_PIP %u, 2HD_PIP %u",
              openSlot, vdcCfg.stHeapSettings.ulBufferCnt_SD, vdcCfg.stHeapSettings.ulBufferCnt_HD,
              vdcCfg.stHeapSettings.ulBufferCnt_2HD,vdcCfg.stHeapSettings.ulBufferCnt_SD_Pip,
              vdcCfg.stHeapSettings.ulBufferCnt_HD_Pip,vdcCfg.stHeapSettings.ulBufferCnt_2HD_Pip));

    rc = BVDC_Heap_Create(state->vdc, &state->vdcHeapMap[openSlot].vdcHeap,
        NEXUS_Heap_GetMemHandle(heap), &vdcCfg.stHeapSettings);
    if (rc) {rc = BERR_TRACE(rc);goto error;}

    state->vdcHeapMap[openSlot].settings = displayHeapSettings;
    state->vdcHeapMap[openSlot].nexusHeap = heap;
    state->vdcHeapMap[openSlot].refcnt = 1;

    return state->vdcHeapMap[openSlot].vdcHeap;

error:
    BDBG_ERR(("Unable to create a display heap for this operation. User must call NEXUS_Heap_SetDisplayHeapSettings with correct settings beforehand."));
    return NULL;
}

/**************************
BBS VideoTool backdoor

These functions provide the BBS VideoTool raw access to VDC, REG, RDC and MEM along with a mutex which protects them.
This is not a public API and is subject to change.
**************************/

void BVideoTool_LockDisplay(void)
{
    NEXUS_Module_Lock(g_NEXUS_displayModuleHandle);
}

void BVideoTool_UnlockDisplay(void)
{
    NEXUS_Module_Unlock(g_NEXUS_displayModuleHandle);
}

typedef struct BVideoToolDisplayHandles {
    BVDC_Handle vdc;
    BRDC_Handle rdc;
    BREG_Handle reg;
    BMEM_Heap_Handle heap;
} BVideoToolDisplayHandles;

void BVideoTool_GetDisplayHandles(BVideoToolDisplayHandles *pDisplayHandles)
{
    pDisplayHandles->vdc = g_NEXUS_DisplayModule_State.vdc;
    pDisplayHandles->rdc = g_NEXUS_DisplayModule_State.rdc;
    pDisplayHandles->reg = g_pCoreHandles->reg;
    pDisplayHandles->heap = g_pCoreHandles->heap[g_NEXUS_DisplayModule_State.moduleSettings.primaryDisplayHeapIndex];
}

/**
Summary:
Get the settings that were used in NEXUS_DisplayModule_Init.

Description:
These cannot be changed without calling NEXUS_DisplayModule_Uninit then NEXUS_DisplayModule_Init.
This is for informational purposes.
**/
NEXUS_Error NEXUS_DisplayModule_GetMemorySettings(
    unsigned configurationId,                           /* Configuration ID */
    uint32_t mask,                                      /* Must contain at least one window and at least one input */
    NEXUS_DisplayBufferTypeSettings *pFullHdBuffers,    /* [out] Full HD buffer requirements */
    NEXUS_DisplayBufferTypeSettings *pHdBuffers,        /* [out] HD buffer requirements */
    NEXUS_DisplayBufferTypeSettings *pSdBuffers,        /* [out] SD buffer requirements */
    unsigned *pHeapSize                                 /* [out] Heap size in bytes */
    )
{
    #if NEXUS_HAS_DYNAMIC_RTS
    return NEXUS_Vdb_P_GetMemorySettings(
        configurationId,
        mask,
        pFullHdBuffers,
        pHdBuffers,
        pSdBuffers,
        pHeapSize);
    #else
    BSTD_UNUSED(configurationId);
    BSTD_UNUSED(mask);
    BSTD_UNUSED(pFullHdBuffers);
    BSTD_UNUSED(pHdBuffers);
    BSTD_UNUSED(pSdBuffers);
    BSTD_UNUSED(pHeapSize);
    return BERR_TRACE(BERR_NOT_SUPPORTED);
    #endif
}

void NEXUS_DisplayModule_SetVideoDecoderModule( NEXUS_ModuleHandle videoDecoder )
{
    g_NEXUS_DisplayModule_State.modules.videoDecoder = videoDecoder;
}

NEXUS_Error NEXUS_DisplayModule_SetConfigurationId( unsigned configurationId )
{
    BSTD_UNUSED(configurationId);
    return BERR_TRACE(NEXUS_NOT_SUPPORTED);
}
