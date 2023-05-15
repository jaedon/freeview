/***************************************************************************
*     (c)2004-2012 Broadcom Corporation
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
* $brcm_Workfile: nexus_platform.h $
* $brcm_Revision: 68 $
* $brcm_Date: 6/20/12 4:21p $
*
* API Description:
*   API name: Platform
*    Specific APIs to initialize the a board.
*
* Revision History:
*
* $brcm_Log: /nexus/platforms/97400/include/nexus_platform.h $
* 
* 68   6/20/12 4:21p erickson
* SW7420-2078: remove acquire/release functions for frontend/i2c. must
*  initserver before opening frontend.
* 
* 67   5/15/12 11:04a erickson
* SWDEPRECATED-3716: fix issue with NEXUS_BASE_ONLY Build
* 
* 66   3/29/12 5:05p erickson
* SW7435-65: add comments for NEXUS_OFFSCREEN_SURFACE
* 
* 65   3/27/12 4:20p erickson
* SW7425-2607: allow binary compatible platform api's
* 
* 64   3/14/12 11:58a erickson
* SW7425-1747: remove unused code
* 
* 63   2/24/12 5:45p vishk
* SW7231-68: add SPI support on top of I2C for 3461 interface
* 
* 62   2/14/12 1:32p erickson
* SW7231-639: add NEXUS_PlatformStatus.familyId
* 
* 61   12/15/11 7:46p rgreen
* SW7425-1140: Use NEXUS_HAS_CEC instead of NEXUS_NUM_CEC to
*  enable/disable CEC support
* 
* 60   12/15/11 4:26p vsilyaev
* SW7425-1140: Merge NEXUS_Cec module support into main-line
* 
* SW7425-1140/1   12/14/11 2:11p vle
* SW7425-1140: add NEXUS Cec module
* 
* 59   11/11/11 10:55a erickson
* SW7231-317: fix NEXUS_BASE_ONLY, clean up warnings, simplify
*  NEXUS_PlatformConfiguration if no inputs/outputs are defined
* 
* 58   10/31/11 5:51p bandrews
* SW7420-2078: merge to main
* 
* SW7420-2078/2   10/18/11 7:01p bandrews
* SW7231-391: remove global_handle attribute
* 
* SW7420-2078/1   9/30/11 7:23p bandrews
* SW7420-2078: added
* 
* 57   10/13/11 11:01p hongtaoz
* SW7425-1349: added audio dummy outputs;
* 
* 56   9/19/11 4:33p erickson
* SWDEPRECATED-2425: improve comment
* 
* 55   8/18/11 5:50p jgarrett
* SWDTV-6306: Merge DTV APE changes to main branch
* 
* Nexus_APE_Integration/1   7/8/11 6:40p jgarrett
* SWDTV-6760: Adding I2sMultiOutput
* 
* 54   5/17/11 1:06p jhaberf
* SWDTV-6928: Merge of 35233 platform code to the main
* 
* SWDTV-6928/1   5/4/11 6:29p mstefan
* SWDTV-6928: merge to main
* 
* 53   3/4/11 11:27a erickson
* SW7400-3006: add NEXUS_Platform_CreateHeap
*
* 52   2/22/11 4:16p gmohile
* SW7408-210 : Fix kernelmode standby
*
* 51   2/18/11 2:28p erickson
* SW7420-1078: add NEXUS_OFFSCREEN_SURFACE for
*  NEXUS_Platform_GetFramebufferHeap
*
* 50   2/15/11 3:24p erickson
* SW7420-1491: use attr{local=true} to get rid of code duplication
*
* 49   1/28/11 1:57p jgarrett
* SW7125-769: Adding nexus frontend handle stub for platforms without
*  frontend support
*
* 48   1/26/11 1:52p jgarrett
* SW7125-769: Merge to main branch
*
* SW7125-769/1   1/24/11 11:30a mward
* SW7125-769:  Add NEXUS_Platform_OpenFrontend().
*
*
* 47   1/21/11 3:10p erickson
* SW7420-1078: modify comment
*
* 46   12/7/10 5:47p erickson
* SW7420-1148: refactor kernel mode proxy for process isolation
*
* 45   12/2/10 8:15p spothana
* SW7420-1177: Adding nexus hdmi dvo module.
*
* SW7420-1177/1   11/1/10 8:33p spothana
* SW7420-1177: Adding nexus hdmi dvo module.
*
* 44   11/19/10 11:29a spothana
* SW7420-1078: Making changes to  get frame buffer API to follow nexus
*  coding protocol.
*
* 43   11/18/10 7:10p spothana
* SW7420-1078: Get frame buffer heap index per display for each platform.
*
* SW7420-1078/1   11/12/10 5:38p spothana
* SW7420-1078: Get frame buffer heap index per display for each platform.
*
* 42   9/29/10 9:47a erickson
* SW7420-1123: consolidate IPC server header file for now
*
* 41   9/15/10 4:51p jtna
* SW7420-1004: moved Join() to nexus_platform_init.h as well
*
* 40   9/15/10 3:46p jtna
* SW7420-1004: add synthunk for CORE and PLATFORM
*
* 39   8/20/10 12:35p erickson
* SW7405-3832: add support for non-numeric NEXUS_PLATFORM defines.
*
* 38   8/4/10 10:51a VISHK
* SW7335-813: Add smartcard module settings to the Platform settings.
*
* 37   7/14/10 12:27p erickson
* SW7405-4621: add #include for app backward-compat
*
* 36   7/14/10 10:32a erickson
* SW7405-4621: remove nexus_base.h
*
* 35   6/24/10 11:52a erickson
* SW7400-2806: add NEXUS_Platform_GetStatus
*
* 34   4/21/10 9:29a erickson
* SW7420-703: pass NEXUS_SurfaceModuleSettings through
*  NEXUS_PlatformSettings
*
* 33   4/9/10 4:48p katrep
* SW7405-4089:Fixed segfault in 32UMA mode
*
* 32   3/19/10 10:12a erickson
* SW7405-3832: create standard method for specific
*  NEXUS_PLATFORM_VERSION. use tagged NEXUS_Platform_Init to enforce
*  binary compatibility. consolidate NEXUS_Platform_GetReleaseVersion
*  using standard macros.
*
* 31   3/9/10 2:17p ismailk
* SW7420-618: Include nexus_base.h for the 93380sms build.
*
* 30   1/6/10 2:56p garetht
* SW7420-534: Port 7043 demo software to 93380sms (7420 + 3380) platform
*
* 29   12/15/09 4:30p erickson
* SW7400-2629: add NEXUS_Platform_ReadRegister and
*  NEXUS_Platform_WriteRegister
*
* 28   11/30/09 3:14p jrubio
* SW7335-627: fixed for NEXUS_BASE_ONLY
*
* 27   11/12/09 1:14p erickson
* SWDEPRECATED-2425: improve comments
*
* 26   10/19/09 6:28p garetht
* SW7420-386:  Rebase 93380sms codebase to 97420 Phase 3.0
*
* 24   10/15/09 4:31p erickson
* SWDEPRECATED-3881: add new members to NEXUS_PlatformConfiguration
*
* 23   8/28/09 4:00p jgarrett
* SW7405-2928: Adding multichannel capture support
*
* 22   8/5/09 4:37p katrep
* PR56569: Add nexus platfromm get version api
*
* 21   7/2/09 10:43a erickson
* PR56558: make NEXUS_Platform_InitFrontend a public function, callable
*  after NEXUS_Platform_Init if NEXUS_PlatformSettings.openFrontend =
*  false.
*
* 20   5/22/09 11:30a vishk
* PR 54842: rockford/unittests/nexus/general/boot should test
*  NEXUS_PlatformSettings options
*
* 19   5/14/09 6:13p katrep
* PR52109: Thunk layer build error in the kernelmode builds
*
* 18   4/7/09 11:42a erickson
* PR53623: change #define to NEXUS_NUM_656_OUTPUTS
*
* 17   4/1/09 12:15p erickson
* PR53623: merge 656 output support
*
* PR53623/1   3/31/09 12:44p mward
* PR 53623: 656 output support.
*
* 16   3/26/09 11:27a erickson
* PR53323: added NEXUS_BASE_ONLY option
*
* 15   3/20/09 1:08p erickson
* PR53323: fix #if
*
* 14   3/19/09 4:59p erickson
* PR53323: add more #if so that more modules can be optionally removed
*
* 13   3/9/09 2:33p erickson
* PR50748: add multi-heap support
*
* 12   9/24/08 11:19a erickson
* PR46457: modified usage notes on NEXUS_Platform_GetSettings
*
* 11   9/2/08 4:45p erickson
* PR46457: added NEXUS_Platform_GetSettings
*
* 10   8/22/08 7:06p erickson
* PR45795: remove maxOutputFormat. it's available with
*  NEXUS_DisplayModule_GetSettings.
*
* 9   7/31/08 5:19p vsilyaev
* PR 45137: Initial support for multi application mode
*
* 8   7/22/08 3:03p erickson
* PR44598: move some module settings into NEXUS_PlatformSettings to allow
*  app to customize init.
*
* 7   3/27/08 2:43p vsilyaev
* PR 40818: Added settings to allocate shared heap
*
* 6   3/25/08 7:41p vsilyaev
* PR 40862: Added supported video output formats
*
* 5   2/20/08 1:45p erickson
* PR39405: added rfm
*
* 4   2/1/08 5:25p jgarrett
* PR 39017: adding HdmiOutput
*
* 3   1/23/08 9:21p vobadm
* PR35457: update docs
*
* 2   1/23/08 3:11p vsilyaev
* PR 38682: Changed NEXUS_Platform_GetStreamerInputBand to follow nexus
*  rules on return types
*
* 1   1/18/08 2:17p jgarrett
* PR 38808: Merging to main branch
*
* Nexus_Devel/10   12/6/07 2:47p jgarrett
* PR 37971: Adding I2sOutput
*
* Nexus_Devel/9   11/6/07 4:22p jgarrett
* PR 34954: Migrating to audio connector model
*
* Nexus_Devel/8   11/5/07 4:34p erickson
* PR36725: remove bootOnly
*
* Nexus_Devel/7   11/5/07 2:12p erickson
* PR36725: add NEXUS_PlatformConfiguration, share more code with 97401
*
* Nexus_Devel/6   10/29/07 1:46p ptimariu
* PR35002: add extern "C"
*
* Nexus_Devel/5   10/18/07 10:39a erickson
* PR36152: added NEXUS_Platform_GetStreamerInputBand
*
* Nexus_Devel/4   9/26/07 5:36p vsilyaev
* PR 34662: Added platform configuration
*
* Nexus_Devel/3   9/24/07 3:43p jgarrett
* PR 35002: Adding platform_features
*
* Nexus_Devel/2   9/21/07 6:26p jgarrett
* PR 35002: Complete 97400 build
*
* Nexus_Devel/1   9/19/07 6:32p jgarrett
* PR 35002: Adding initial 97400 platform
*
***************************************************************************/
#ifndef NEXUS_PLATFORM_H__
#define NEXUS_PLATFORM_H__

#include "nexus_types.h"
#include "nexus_base_os.h"
#include "nexus_memory.h"
#include "nexus_platform_features.h"
#include "nexus_platform_extint.h"
#include "nexus_platform_init.h"
#include "nexus_platform_standby.h"
#include "nexus_platform_server.h"

#ifdef __cplusplus
extern "C"
{
#endif

/***************************************************************************
Summary:
Get settings that were passed into NEXUS_Platform_Init along with modifications
made by Nexus. Be aware this is a different meaning of GetSettings than the
standard definition.

Description:
For some settings, 0 means that Nexus should select a default. NEXUS_Platform_GetSettings will
not return that 0 but will return the default that was chosen. In this sense, this
function works more like GetStatus.
***************************************************************************/
void NEXUS_Platform_GetSettings(
    NEXUS_PlatformSettings *pSettings /* [out] */
    );

/* generic max is useful for binary compatibility between varying platforms */
#define NEXUS_MAX_CONFIG_HANDLES 5

/***************************************************************************
Summary:
Broadcom reference platform configuration

Description:
The broadcom reference platforms will open board-specific handles
and place the handles in this structure for use by the application.
***************************************************************************/
typedef struct NEXUS_PlatformConfiguration {
#if NEXUS_HAS_I2C
    NEXUS_I2cHandle i2c[NEXUS_MAX_CONFIG_HANDLES];
#endif
#if NEXUS_HAS_FRONTEND && NEXUS_MAX_FRONTENDS
    NEXUS_FrontendHandle frontend[NEXUS_MAX_FRONTENDS];
#endif

    struct {
#if NEXUS_HAS_DISPLAY
        NEXUS_ComponentOutputHandle component[NEXUS_MAX_CONFIG_HANDLES];
        NEXUS_CompositeOutputHandle composite[NEXUS_MAX_CONFIG_HANDLES];
        NEXUS_SvideoOutputHandle svideo[NEXUS_MAX_CONFIG_HANDLES];
        NEXUS_Ccir656OutputHandle ccir656[NEXUS_MAX_CONFIG_HANDLES];
#endif
#if NEXUS_HAS_RFM && NEXUS_NUM_RFM_OUTPUTS
        NEXUS_RfmHandle rfm[NEXUS_NUM_RFM_OUTPUTS];
#endif
#if NEXUS_HAS_AUDIO
#if NEXUS_NUM_AUDIO_DACS
        NEXUS_AudioDacHandle audioDacs[NEXUS_NUM_AUDIO_DACS];
#endif
#if NEXUS_NUM_AUDIO_DUMMY_OUTPUTS
        NEXUS_AudioDummyOutputHandle audioDummy[NEXUS_NUM_AUDIO_DUMMY_OUTPUTS];
#endif
#if NEXUS_NUM_SPDIF_OUTPUTS
        NEXUS_SpdifOutputHandle spdif[NEXUS_NUM_SPDIF_OUTPUTS];
#endif
#if NEXUS_NUM_I2S_OUTPUTS
        NEXUS_I2sOutputHandle i2s[NEXUS_NUM_I2S_OUTPUTS];
#endif
#if NEXUS_NUM_I2S_MULTI_OUTPUTS
        NEXUS_I2sMultiOutputHandle i2sMulti[NEXUS_NUM_I2S_MULTI_OUTPUTS];
#endif
#endif
#if NEXUS_HAS_HDMI_OUTPUT && NEXUS_NUM_HDMI_OUTPUTS
        NEXUS_HdmiOutputHandle hdmi[NEXUS_NUM_HDMI_OUTPUTS];
#endif
#if NEXUS_HAS_HDMI_DVO && NEXUS_NUM_HDMI_DVO
        NEXUS_HdmiDvoHandle hdmiDvo[NEXUS_NUM_HDMI_DVO];
#endif
#if NEXUS_HAS_CEC && NEXUS_NUM_CEC
        NEXUS_CecHandle cec[NEXUS_NUM_CEC];
#endif
        int dummy; /* always have dummy to avoid warnings if there are no inputs */
    } outputs;

    struct {
#if NEXUS_NUM_COMPONENT_INPUTS
        NEXUS_ComponentInputHandle component[NEXUS_NUM_COMPONENT_INPUTS];
    #if NEXUS_NUM_ANALOG_AUDIO_DECODERS
        NEXUS_AnalogAudioInputHandle componentAudio[NEXUS_NUM_COMPONENT_INPUTS];
    #endif
#endif
#if NEXUS_NUM_SVIDEO_INPUTS
        NEXUS_SvideoInputHandle svideo[NEXUS_NUM_SVIDEO_INPUTS];
    #if NEXUS_NUM_ANALOG_AUDIO_DECODERS
        NEXUS_AnalogAudioInputHandle svideoAudio[NEXUS_NUM_SVIDEO_INPUTS];
    #endif
#endif
#if NEXUS_NUM_COMPOSITE_INPUTS
        NEXUS_CompositeInputHandle composite[NEXUS_NUM_COMPOSITE_INPUTS];
    #if NEXUS_NUM_ANALOG_AUDIO_DECODERS
        NEXUS_AnalogAudioInputHandle compositeAudio[NEXUS_NUM_COMPOSITE_INPUTS];
    #endif
#endif
#if NEXUS_NUM_PC_INPUTS
        NEXUS_PcInputHandle pc[NEXUS_NUM_PC_INPUTS];
    #if NEXUS_NUM_ANALOG_AUDIO_DECODERS
        NEXUS_AnalogAudioInputHandle pcAudio[NEXUS_NUM_PC_INPUTS];
    #endif
#endif
#if NEXUS_NUM_SCART_INPUTS
        NEXUS_ScartInputHandle scart[NEXUS_NUM_SCART_INPUTS];
    #if NEXUS_NUM_ANALOG_AUDIO_DECODERS
        NEXUS_AnalogAudioInputHandle scartAudio[NEXUS_NUM_SCART_INPUTS];
    #endif
#endif
        int dummy; /* always have dummy to avoid warnings if there are no inputs */
    } inputs;

    struct {
        NEXUS_VideoFormat maxOutputFormat; /* maximum supported video output format */
        NEXUS_VideoFormat maxOutputFormatSd; /* maximum supported standard definition video output format */
    } video;

#if NEXUS_HAS_DISPLAY
    bool supportedDisplay[NEXUS_MAX_DISPLAYS]; /* report which displays can be opened */
#endif
    unsigned numWindowsPerDisplay; /* 1 = main only, 2 = PIP-capable */

#if NEXUS_HAS_VIDEO_DECODER
    bool supportedDecoder[NEXUS_MAX_VIDEO_DECODERS]; /* This refers to regular decoders, not mosaic decoders. */
#endif

    NEXUS_HeapHandle heap[NEXUS_MAX_HEAPS];
#if NEXUS_HAS_SPI
    NEXUS_SpiHandle spi[NEXUS_MAX_CONFIG_HANDLES];
#endif
} NEXUS_PlatformConfiguration;

/***************************************************************************
Summary:
Get configured handles from the platform layer

Description:
The reference nexus platform code will initialize board-specific handles
based upon settings provided to NEXUS_Platform_Init().  Those handles
may be retrieved by an application via this funciton.

See Also:
NEXUS_Platform_Init
***************************************************************************/
void NEXUS_Platform_GetConfiguration(
    NEXUS_PlatformConfiguration *pConfiguration /* [out] */
    );


/***************************************************************************
Summary:
Get the input band for a streamer.

Description:
The assignment of streamer input to input band depends on board layout and possibly FPGA routing.
If FPGA configuration is required for your platform to achieve this routing, you must set openI2c
and openFpga to be true in NEXUS_PlatformSettings.
 ***************************************************************************/
NEXUS_Error NEXUS_Platform_GetStreamerInputBand(
    unsigned index, /* index of the streamer input */
    NEXUS_InputBand *pInputBand /* [out] */
    );

/***************************************************************************
Summary:
Initialize the frontend if NEXUS_PlatformSettings.openFrontend = false.

Description:
If NEXUS_PlatformSettings.openFrontend = true (which is default), then NEXUS_Platform_Init
will call NEXUS_Platform_InitFrontend automatically. For faster system boot time, you can
set openFrontend = false and then call NEXUS_Platform_InitFrontend after the system has achieved
some baseline state (e.g. after the app has put graphics on the screen).
***************************************************************************/
NEXUS_Error NEXUS_Platform_InitFrontend(void);

/***************************************************************************
Summary:
Initialize a particular frontend.

Description:
This will create and initialize the specified frontend.
Use NEXUS_Platform_UninitFrontend to uninit the frontend along with any others
that have been created.
***************************************************************************/
NEXUS_FrontendHandle NEXUS_Platform_OpenFrontend(
    unsigned id /* platform assigned ID for this frontend. See NEXUS_FrontendUserParameters.id.
                   See nexus_platform_frontend.c for ID assignment and/or see
                   nexus_platform_features.h for possible platform-specific macros.
                */
    );

/***************************************************************************
Summary:
Uninitialize all frontends.

Description:
This will uninit whatever was created by NEXUS_Platform_OpenFrontend,
NEXUS_Platform_InitFrontend, or NEXUS_Platform_Init.
NEXUS_Platform_UninitFrontend is called automatically by NEXUS_Platform_Uninit.
***************************************************************************/
void NEXUS_Platform_UninitFrontend(void);

/***************************************************************************
Summary:
Get the current reference software release version.
***************************************************************************/
void NEXUS_Platform_GetReleaseVersion( /* attr{local=true} */
    char *pVersionString, /* [out] returns a NULL-terminated string <= size */
    unsigned size /* maximum size data written to pVersionString, including null terminator */
    );

/***************************************************************************
Summary:
Platform status and general information

Description:
Examples of familyId and chipId are:

    familyId  chipId List
    0x7429    0x7429,0x7428,0x7241,0x7242
    0x7231    0x7231,0x7230,0x7229
    
***************************************************************************/
typedef struct NEXUS_PlatformStatus
{
    uint16_t chipId;       /* main chip, in hex, e.g. 0x7425. This ID is also called the product ID or bondout ID. */
    uint16_t chipRevision; /* revision of the main chip, in hex, e.g. 0x0010 = A0, 0x0021 = B1 */
    uint32_t familyId;     /* the family of this chip. this usually corresponds to the BCHP_CHIP used to compile the SW.
                              SW compiled for the familyId can often run on all chipId's within that family. */
} NEXUS_PlatformStatus;

/***************************************************************************
Summary:
Get platform status
***************************************************************************/
NEXUS_Error NEXUS_Platform_GetStatus(
    NEXUS_PlatformStatus *pStatus /* [out] */
    );

/***************************************************************************
Summary:
Read a register

Description:
This function is provided for special cases and is not generally supported. Indiscriminate use will result in system failure.
It requires the application to know the register map, usually by including RDB header files.

Nexus will add the memory-mapped base pointer for register space. This means you can use RDB register addresses directly.
***************************************************************************/
NEXUS_Error NEXUS_Platform_ReadRegister(
    uint32_t address,
    uint32_t *pValue /* [out] */
    );

/***************************************************************************
Summary:
Write a register

Description:
This function is provided for special cases and is not generally supported. Indiscriminate use will result in system failure.
It requires the application to know the register map, usually by including RDB header files.
***************************************************************************/
void NEXUS_Platform_WriteRegister(
    uint32_t address,
    uint32_t value
    );

/***************************************************************************
Summary:
Returns heap that is accessible by GFD HW block for the specified display.
If you use this heap in your framebuffer's NEXUS_SurfaceCreateSettings.heap, the GFD will be able to read from it.

Description:
If you pass NEXUS_OFFSCREEN_SURFACE, the function will return a default heap for off-screen surface allocation.
This is provided as a convenience for multi-platform graphics applications.
The heap will be aligned correctly for VC4 (3D graphics).
The heap will also have no guard banding, for optimal performance.
***************************************************************************/
NEXUS_HeapHandle NEXUS_Platform_GetFramebufferHeap(
    unsigned displayIndex
    );
#define NEXUS_OFFSCREEN_SURFACE ((unsigned)-1)


/**
Summary:
Settings for NEXUS_Platform_CreateHeap
**/
typedef struct NEXUS_PlatformCreateHeapSettings
{
    unsigned offset; /* physical address */
    unsigned size; /* in bytes */
    NEXUS_MemoryType memoryType; /* requested memory mapping */
    unsigned alignment; /* required alignment (in bytes) of allocations in this region */
    bool locked; /* if true, nexus is not allowed to allocate from this heap */
    void *userAddress; /* if set, use this instead of doing an internal mmap for NEXUS_MemoryType_eApplication. */
} NEXUS_PlatformCreateHeapSettings;

/**
Summary:
Get default settings for NEXUS_Platform_CreateHeap
**/
void NEXUS_Platform_GetDefaultCreateHeapSettings(
    NEXUS_PlatformCreateHeapSettings *pSettings /* [out] */
    );

/**
Summary:
Create a heap at runtime which is outside of all existing nexus heaps.

Description:
A system may have memory which nexus is not given access to at NEXUS_Platform_Init.
NEXUS_Platform_CreateHeap can create a heap on that memory.
There must be no overlap in offset or virtual address space with existing heaps.
**/
NEXUS_HeapHandle NEXUS_Platform_CreateHeap(
    const NEXUS_PlatformCreateHeapSettings *pSettings
    );

/**
Summary:
Destroy a heap created by NEXUS_Platform_CreateHeap
**/
void NEXUS_Platform_DestroyHeap(
    NEXUS_HeapHandle heap
    );

#if defined(HUMAX_PLATFORM_BASE)
/**
Summary: HUMAX_PLATFORM_BASE
It's added by HUMAX. 

Description:
Read the platform registers.
**/
void NEXUS_Platform_Registers_Read_Access(
    unsigned long Offset, 
    unsigned long *Value
    );

/**
Summary: HUMAX_PLATFORM_BASE
It's added by HUMAX. 

Description:
Write the platform registers.
**/
void NEXUS_Platform_Registers_Write_Access(
    unsigned long Offset, 
    unsigned long Value
    );
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* #ifndef NEXUS_PLATFORM_H__ */

