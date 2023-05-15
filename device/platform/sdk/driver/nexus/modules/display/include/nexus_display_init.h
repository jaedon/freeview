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
 * $brcm_Workfile: nexus_display_init.h $
 * $brcm_Revision: 41 $
 * $brcm_Date: 10/5/12 11:50a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/display/include/nexus_display_init.h $
 * 
 * 41   10/5/12 11:50a erickson
 * SW7435-400: clarify how app can set module settings
 * 
 * 40   9/6/12 2:05p erickson
 * SW7346-1015: add NEXUS_DisplayModuleSettings.dacDetection
 * 
 * 39   5/18/12 10:22a erickson
 * SW7435-165: merge
 * 
 * SW7435-165/3   5/17/12 3:24p mward
 * SW7435-165: encoderTgMapping is an int.
 * 
 * 38   3/29/12 3:40p erickson
 * SW7435-77: add NEXUS_DisplayModuleSettings.vbi.allowVps
 * 
 * 37   3/27/12 11:59a erickson
 * SW7435-77: add NEXUS_DisplayModuleSettings.vbi.allowTeletext
 * 
 * 36   3/13/12 4:50p erickson
 * SW7425-2607: use unvarying macros in API for binary compatibility
 * 
 * 35   11/15/11 3:27p erickson
 * SW7425-1747: remove old code
 * 
 * 34   10/31/11 7:49p bandrews
 * SW7231-391: merge to main
 * 
 * SW7420-2078/1   10/31/11 7:34p bandrews
 * SW7231-391: added dependency on transport module for timebase calls
 * 
 * 33   10/27/11 3:32p erickson
 * SW7425-1581: move crcQueueSize to NEXUS_DisplaySettings
 * 
 * 32   3/11/11 4:13p erickson
 * SW7346-103: add support for CMP CRC capture
 *
 * 31   2/25/11 4:37p erickson
 * SW7422-255: add AMOL support
 *
 * 30   12/9/10 5:16p erickson
 * SW7420-1177: add hdmiDvo
 *
 * 29   11/18/10 4:27p erickson
 * SW7420-1078: SW7420-1078
 *
 * SW7420-1078/2   11/12/10 4:39p spothana
 * SW7420-1078 : Add APIs to modify the display heap parameters.
 *
 * SW7420-1078/1   11/11/10 6:26p spothana
 * SW7420-1078: assign heap index per video window and per graphics frame
 *  buffer
 *
 * 28   8/19/10 2:15p jhaberf
 * SW35230-760: Added ability to disable FRC
 *
 * 27   1/8/10 3:16p erickson
 * SW3556-979: add NEXUS_DisplayModuleSettings.vbi booleans to alloc
 *  memory for CGMS-B and gemstar VBI types
 *
 * 26   10/15/09 4:30p erickson
 * SWDEPRECATED-3881: remove comments on struct members which are
 *  deprecated and are now unused
 *
 * 25   9/1/09 4:23p erickson
 * SWDEPRECATED-2425: update comments
 *
 * 24   7/24/09 10:48a erickson
 * PR51648: add NEXUS_DisplayModuleSettings.dacBandGapAdjust[]. move env
 *  variable to Settop API shim.
 *
 * 23   7/10/09 3:57p erickson
 * PR56558: add NEXUS_DisplayModule_SetVideoDecoderModule to allow faster
 *  start up time
 *
 * 22   6/15/09 5:01p erickson
 * PR56022: deprecated numWindows and numDisplays
 *
 * 21   6/15/09 12:18p erickson
 * PR55928: added NEXUS_DisplaySettings.vecIndex
 *
 * 20   6/12/09 10:50a erickson
 * PR55967: convert NEXUS_DisplayModuleSettings heap settings to integers.
 *  This makes them settable in the application.
 *
 * 19   3/27/09 2:34p erickson
 * PR53453: add ccir656InputBufferSize
 *
 * 18   3/27/09 10:31a erickson
 * PR48963: add heap and pipHeap to NEXUS_DisplayModuleSettings. allows
 *  for more platform configurability.
 *
 * 17   2/20/09 4:49p jgarrett
 * PR 52266: Adding memory requirements interface
 *
 * 16   2/12/09 5:34p jgarrett
 * PR 51958: Adding vecSwap option
 *
 * 15   2/10/09 11:47a jgarrett
 * PR 48984: Incorporating comments from vdb review
 *
 * 14   1/6/09 11:32a jgarrett
 * PR 48984: Merge dynamic RTS code to main branch
 *
 * PR48984/1   11/20/08 6:46p jgarrett
 * PR 48984: Adding VDB support
 *
 * 13   10/30/08 8:54p erickson
 * PR47030: refactor nested structures for reuse with NEXUS_HeapHandle
 *
 * 12   10/24/08 1:38p erickson
 * PR47030: added fullHd/hd/sd buffer settings for digital when
 *  analog/digital memory sharing is enabled
 *
 * 11   9/12/08 9:14a erickson
 * PR46824: added NEXUS_DisplayModuleSettings.vbi.tteShiftDirMsb2Lsb
 *
 * 10   9/5/08 1:16p erickson
 * PR45897: add dynamic backlight options for panels
 *
 * 9   9/2/08 4:56p erickson
 * PR46457: added runtime params for max resources
 *
 * 8   8/22/08 7:06p erickson
 * PR45795: replace enum with 2HD/HD/SD buffer settings. this allows user
 *  to fully configure VDC memory allocation.
 *
 * 7   8/14/08 9:51a erickson
 * PR45647: add NEXUS_PanelOutputLinkMode_eCustom and related settings
 *
 * 6   7/10/08 4:46p erickson
 * PR44598: added LVD link settings to NEXUS_DisplayModuleSettings.panel
 *
 * 5   6/23/08 7:19a erickson
 * PR43914: move BLVD_Open before BVDC_Open
 *
 * 4   5/5/08 4:48p erickson
 * PR42445: provide API's for better display frame rate control
 *
 * 3   2/20/08 1:33p erickson
 * PR39405: added RFM output
 *
 * 2   2/1/08 5:25p jgarrett
 * PR 39017: Adding HdmiOutput
 *
 * 1   1/18/08 2:20p jgarrett
 * PR 38808: Merging to main branch
 *
 * Nexus_Devel/8   11/30/07 10:52a erickson
 * PR35457: api update
 *
 * Nexus_Devel/7   11/8/07 2:42p erickson
 * PR36814: added hdmiInput. only used on 3563, but harmless on other
 * platforms.
 *
 * Nexus_Devel/6   11/6/07 1:04p erickson
 * PR36570: added NEXUS_DisplayModuleBufferType
 *
 * Nexus_Devel/5   9/28/07 11:55a vsilyaev
 * PR 34662: Fixed API after review
 *
 * Nexus_Devel/4   9/26/07 1:43p vsilyaev
 * PR 34662: Implementation of display framebuffer
 *
 * Nexus_Devel/3   9/24/07 12:21p vsilyaev
 * PR 34662: Added connection of window and source
 *
 * Nexus_Devel/2   9/13/07 4:21p vsilyaev
 * PR 34419: Fixed typos
 *
 * Nexus_Devel/1   9/13/07 4:01p vsilyaev
 * PR 34419: Added module operations
 *
 **************************************************************************/
#ifndef NEXUS_DISPLAY_INIT_H__
#define NEXUS_DISPLAY_INIT_H__

#include "nexus_memory.h"
#include "nexus_display_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
Summary:
The Display module's down modules
**/
typedef struct NEXUS_DisplayModuleDependencies
{
    NEXUS_ModuleHandle videoDecoder;
    NEXUS_ModuleHandle surface;
    NEXUS_ModuleHandle hdmiInput;
    NEXUS_ModuleHandle hdmiDvo;
    NEXUS_ModuleHandle hdmiOutput;
    NEXUS_ModuleHandle rfm;
    NEXUS_ModuleHandle pwm; /* needed for panel backlight control */
    NEXUS_ModuleHandle transport;
} NEXUS_DisplayModuleDependencies;

typedef enum NEXUS_VideoDacDetection
{
    NEXUS_VideoDacDetection_eAuto,
    NEXUS_VideoDacDetection_eOff,
    NEXUS_VideoDacDetection_eOn,
    NEXUS_VideoDacDetection_eMax
} NEXUS_VideoDacDetection;

/**
Summary:
Settings used in NEXUS_DisplayModule_Init
**/
typedef struct NEXUS_DisplayModuleSettings
{
    NEXUS_DisplayModuleDependencies modules;

    /* These determine the creation of a Display Module heap (i.e. the internal VDC heap) from device heap[0].
       This is used if NEXUS_VideoWindowSettings.heap or NEXUS_VideoInputSettings.heap is set.
       If you are always using per-VideoWindow and per-VideoInput heaps, then buffers should be all zeros. */
    NEXUS_DisplayBufferTypeSettings fullHdBuffers; /* double-sized HD buffers used for 1080p output */
    NEXUS_DisplayBufferTypeSettings hdBuffers;     /* HD buffers used for 1080i or 720p output */
    NEXUS_DisplayBufferTypeSettings sdBuffers;     /* SD buffers used for 480i/576i output */


    /* displayHeapSettings[] will default to all zero.
    If the platform code detects this is all zero, it will set its defaults.
    If the display module code detects this is all zero, it will use the legacy settings. */
    NEXUS_DisplayHeapSettings displayHeapSettings[NEXUS_MAX_HEAPS];
    unsigned videoWindowHeapIndex[NEXUS_MAX_DISPLAYS][NEXUS_MAX_VIDEO_WINDOWS]; /* Set the heap index per video window */
    unsigned primaryDisplayHeapIndex;        /* The heap given to BVDC_Open for general use.
                                                This is usually the heap index for videoWindowHeapIndex[0][0] i.e HD Main Video window */
    bool dropFrame;                      /* If NEXUS_DisplaySettings.frameRateMaster is not set and NEXUS_VideoWindowSettings.autoMaster is
                                            not set, then this boolean determines if a VEC is drop-frame or non-drop-frame. */

    uint32_t dacBandGapAdjust[NEXUS_MAX_VIDEO_DACS];  /*Adjustment to the video TDAC and QDAC bandgap setting.
                                            The default value is correct for most chipsets. However, there are
                                            some production runs that require an adjustment for correct amplitude,
                                            depends on the particular fab line that manufactured the chip. */
    NEXUS_VideoDacDetection dacDetection;
    
    struct {
        bool allowTeletext;              /* allocate extra memory for teletext VBI */
        bool allowVps;                   /* allocate extra memory for vps VBI */
        bool allowGemStar;               /* allocate extra memory for gemstar VBI */
        bool allowCgmsB;                 /* allocate extra memory for CGMS-B VBI */
        bool allowAmol;                  /* allocate extra memory for AMOL VBI */
        bool tteShiftDirMsb2Lsb;         /* If true, teletext encoder shift direction will be set to MSBToLSB. Otherwise, it
                                            will be set to LSBToMSB. The default value is FALSE. */
        unsigned ccir656InputBufferSize; /* Size of 656 VBI buffer in bytes. The default value is 0. */
    } vbi;

    bool vecSwap;                       /* Default = true.  If you require more than three SD outputs with concurrent HD output, you may need to set this to false.
                                           This is used only if NEXUS_DisplaySettings.vecIndex is -1. */
    bool handleDynamicRts;              /* Default = true on DTV platforms.  Set to false if you want to manage dynamic RTS in the application instead.
                                           You will need to set the system into NEXUS_VideoInputResumeMode_eNow to handle dynamic RTS. See
                                           NEXUS_VideoInput_SetResumeMode for more details. */
    unsigned configurationId;           /* The BVN configuration # from the RTS spreadsheet.  Currently only applicable to DTV chipsets.
                                           If this value is not set, the application will be responsible for handling source pending callbacks
                                           and making the necessary adjustments to settings and RTS values. */
    bool disableFrc;                    /* Default = false. Set to true to disable loading and running of FRC. */
    int encoderTgMapping[NEXUS_MAX_VIDEO_ENCODERS]; /* index of STG to use for video encoder. 
                                           array index is NEXUS_VideoEncoder index. use -1 for no mapping. */
} NEXUS_DisplayModuleSettings;

/**
Summary:
Get defaults before calling NEXUS_DisplayModule_Init
**/
void NEXUS_DisplayModule_GetDefaultSettings(
    NEXUS_DisplayModuleSettings *pSettings /* [out] */
    );

/**
Summary:
Initialize the Display module

Description:
This function is called by NEXUS_Platform_Init, not by applications.
If you want to modify these settings from your application, you can do this 
through NEXUS_PlatformSettings as follows:

    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.displayModuleSettings.xxx = xxx;
    NEXUS_Platform_Init(&platformSettings);

**/
NEXUS_ModuleHandle NEXUS_DisplayModule_Init(
    const NEXUS_DisplayModuleSettings *pSettings
    );

/**
Summary:
Uninitialize the Display module
**/
void NEXUS_DisplayModule_Uninit(void);

/**
Summary:
Get the settings that were used in NEXUS_DisplayModule_Init.

Description:
These cannot be changed without calling NEXUS_DisplayModule_Uninit then NEXUS_DisplayModule_Init.
This is for informational purposes.
**/
void NEXUS_DisplayModule_GetSettings(
    NEXUS_DisplayModuleSettings *pSettings /* [out] */
    );

#define NEXUS_DISPLAY_WINDOW_MAIN (0x1)
#define NEXUS_DISPLAY_WINDOW_PIP  (0x2)
#define NEXUS_DISPLAY_WINDOW_MONITOR (0x4)

#define NEXUS_DISPLAY_INPUT_DIGITAL (0x1000)
#define NEXUS_DISPLAY_INPUT_ANALOG  (0x2000)

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
    );

/**
Summary:
Set the VideoDecoder module dependency

Description:
This allows for faster system boot time. The Display module and VideoDecoder module can init separately, then the link can be made after both are initialized.
**/
void NEXUS_DisplayModule_SetVideoDecoderModule(
    NEXUS_ModuleHandle videoDecoder /* Set to NULL or to the VideoDecoder module */
    );

#ifdef __cplusplus
}
#endif

#endif /* NEXUS_DISPLAY_INIT_H__ */
