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
* $brcm_Workfile: nexus_platform_init.h $
* $brcm_Revision: 30 $
* $brcm_Date: 10/9/12 4:08p $
*
* API Description:
*   API name: Platform
*    Specific APIs to initialize the a board.
*
* Revision History:
*
* $brcm_Log: /nexus/platforms/common/include/nexus_platform_init.h $
* 
* 30   10/9/12 4:08p erickson
* SW7425-3782: provide stub NEXUS_FrontendHandle if needed
* 
* 29   10/5/12 11:53a erickson
* SW7435-400: remove unused code
* 
* 28   8/31/12 10:00a erickson
* SW7435-328: remove NEXUS_PlatformSettings.openSpi. everyone should open
*  their own SPI handle as needed.
* 
* 27   8/27/12 3:26p erickson
* SW7425-1747: remove unused code
* 
* 26   8/1/12 11:12a erickson
* SW7435-279: add NEXUS_PlatformSettings.graphics2DModuleSettings
* 
* 25   7/25/12 8:40a erickson
* SW7435-179: update comments for NEXUS_PlatformSettings.heap[]
* 
* 24   5/14/12 2:42p erickson
* SW7435-179: add boolean for optional heap creation, fail if non-
*  optional heap can't be created
* 
* 23   4/24/12 10:27a erickson
* SW7420-507: change to #if NEXUS_HAS_TRANSCODER
* 
* 22   3/27/12 4:20p erickson
* SW7425-2607: allow binary compatible platform api's
* 
* 21   2/24/12 5:50p vishk
* SW7231-68: add SPI support on top of I2C for 3461 interface
* 
* 20   12/15/11 7:45p rgreen
* SW7425-1140: Use NEXUS_HAS_CEC instead of NEXUS_NUM_CEC to
*  enable/disable CEC support
* 
* 19   12/15/11 4:26p vsilyaev
* SW7425-1140: Merge NEXUS_Cec module support into main-line
* 
* SW7425-1140/1   12/9/11 5:11p vle
* SW7425-1140: add NEXUS Cec module
* 
* 18   10/13/11 11:01p hongtaoz
* SW7425-1349: added audio dummy outputs;
* 
* 15   9/6/11 10:05a erickson
* SWNOOS-482: add NEXUS_FileModuleSettings.schedulerSettings[] per-thread
*  array
* 
* 14   8/10/11 3:03p erickson
* SW7420-1123: move NEXUS_Platform_Uninit to nexus_platform_client.h so
*  clients don't have to #include nexus_platform.h
* 
* 13   5/25/11 9:54a erickson
* SW7346-214: add security module settings to NEXUS_PlatformSettings
* 
* 12   5/17/11 1:06p jhaberf
* SWDTV-6928: Merge of 35233 platform code to the main
* 
* 11   5/12/11 4:12p erickson
* SW7422-2: clarify comments on NEXUS_PlatformSettings.heap[]
* 
* 10   4/20/11 7:48p vsilyaev
* SW7425-394: Allow application to set number of I/O worker threads that
*  are serving I/O requests
* 
* 9   4/1/11 4:26p jtna
* SW7346-119: add 40nm allpass support
* 
* 8   2/17/11 4:24p gmohile
* SW7408-210 : Merge PM 2.0 support to mainline
* 
* SW7408-210/1   1/12/11 1:58p gmohile
* SW7408-210 : Add standby support
* 
* 7   12/22/10 2:29p erickson
* SW7422-2: add support for multiple physical address regions within a
*  single MEMC
*
* 6   12/20/10 4:26p erickson
* SW7422-2: rework nexus_platform_core, add board-specific file and API,
*  mmap after determining heaps
*
* 5   12/13/10 7:02p vsilyaev
* SW7425-40: Added system initialization for the StreamMux and
*  VideoEncoder modules
*
* 4   12/7/10 5:47p erickson
* SW7420-1148: refactor kernel mode proxy for process isolation
*
* 3   12/2/10 8:15p spothana
* SW7420-1177: Adding nexus hdmi dvo module.
*
* SW7420-1177/1   11/1/10 8:57p spothana
* SW7420-1177: Adding nexus hdmi dvo module.
*
* 2   9/15/10 4:51p jtna
* SW7420-1004: moved Join() to nexus_platform_init.h as well
*
* 1   9/15/10 3:41p jtna
* SW7420-1004: add synthunk for CORE and PLATFORM
*
***************************************************************************/
#ifndef NEXUS_PLATFORM_INIT_H__
#define NEXUS_PLATFORM_INIT_H__

#include "nexus_types.h"
#include "nexus_base_os.h"
#include "nexus_memory.h"
#include "nexus_platform_features.h"
#include "nexus_platform_extint.h"
#include "nexus_platform_version.h"
#include "nexus_platform_standby.h"
#if NEXUS_HAS_SURFACE
#include "nexus_surface_init.h"
#endif
#if NEXUS_HAS_TRANSPORT
#include "nexus_transport_init.h"
#endif
#if NEXUS_HAS_DISPLAY
#include "nexus_display_init.h"
#endif
#if NEXUS_HAS_AUDIO
#include "nexus_audio_init.h"
#include "nexus_audio_dac.h"
#include "nexus_spdif_output.h"
#if NEXUS_NUM_AUDIO_DUMMY_OUTPUTS
#include "nexus_audio_dummy_output.h"
#endif
#if NEXUS_NUM_I2S_OUTPUTS
#include "nexus_i2s_output.h"
#endif
#endif
#if NEXUS_HAS_VIDEO_DECODER
#include "nexus_video_decoder_init.h"
#endif
#if NEXUS_HAS_I2C
#include "nexus_i2c.h"
#endif
#if NEXUS_HAS_SPI
#include "nexus_spi.h"
#endif
#if NEXUS_HAS_FRONTEND
#include "nexus_frontend.h"
#else
typedef void *NEXUS_FrontendHandle; /* stub */
#endif
#if NEXUS_HAS_DISPLAY
#include "nexus_component_output.h"
#include "nexus_composite_output.h"
#include "nexus_svideo_output.h"
#include "nexus_ccir656_output.h"
#endif
#if NEXUS_HAS_RFM
#include "nexus_rfm.h"
#endif
#if NEXUS_HAS_HDMI_OUTPUT
#include "nexus_hdmi_output.h"
#endif
#if NEXUS_HAS_HDMI_DVO
#include "nexus_hdmi_dvo.h"
#endif
#if NEXUS_HAS_CEC
#include "nexus_cec.h"
#endif
#if NEXUS_HAS_SMARTCARD
#include "nexus_smartcard_init.h"
#endif
#if NEXUS_HAS_VIDEO_ENCODER
#include "nexus_video_encoder_init.h"
#endif
#if NEXUS_HAS_SECURITY
#include "nexus_security_init.h"
#endif
#if NEXUS_HAS_GRAPHICS2D
#include "nexus_graphics2d_init.h"
#endif

/* The following macros are for internal use, but are included in this public header file so that
we can provide a tagged NEXUS_Platform_Init to ensure binary compatibility because the nexus binary
and the application binary. */
#define NEXUS_PLATFORM_VERSION_UNITS (256)
#define NEXUS_PLATFORM_VERSION(maj,min) ((maj)*NEXUS_PLATFORM_VERSION_UNITS + (min))
#define NEXUS_P_GET_VERSION_P(p) NEXUS_PLATFORM_ ## p
#define NEXUS_P_GET_VERSION(p)  NEXUS_P_GET_VERSION_P(p)
#define NEXUS_P_GET_STRUCT_SIZES() (sizeof(NEXUS_PlatformSettings) + sizeof(NEXUS_PlatformConfiguration))

#ifdef __cplusplus
extern "C"
{
#endif

/***************************************************************************
Summary:
Run time configuration for the File Module
***************************************************************************/
#ifndef NEXUS_FILE_MAX_IOWORKERS
#define NEXUS_FILE_MAX_IOWORKERS 4
#endif
typedef struct NEXUS_FileModulePlatformSettings {
    unsigned workerThreads; /* number of the I/O worker threads  instaniated in the File module */
    NEXUS_ThreadSettings schedulerSettings[NEXUS_FILE_MAX_IOWORKERS];
} NEXUS_FileModulePlatformSettings;


/***************************************************************************
Summary:
Broadcom reference platform settings

Description:
The typical Broadcom settop/DTV reference platform has the following configurable features:
1) A set of I2C channels
2) An FPGA for routing transport to input bands
3) A series of frontend daughter card slots
4) A set of video and audio outputs with DAC settings which vary per platform

NEXUS_PlatformSettings allows NEXUS_Platform_Init to open these handles and configure
the devices according to typical defaults.
Your application can choose to disable this and open the handles itself.

If you allow Platform to open the handles, you cannot re-open the same handles in your application.
Instead, you must call NEXUS_Platform_GetConfiguration to retrieve the handles opened by NEXUS_Platform_Init.
***************************************************************************/
typedef struct NEXUS_PlatformSettings
{
    bool cachedMemory;      /* If true, the data cache will be enabled for driver memory clients that wish to use it.
                               If false, the cache will be disabled */
    bool openI2c;           /* If true, NEXUS_Platform_Init will open the I2C channels for this platform. */
    bool openFpga;          /* If true, NEXUS_Platform_Init will open the FPGA. This is needed for some frontends. */
    bool openFrontend;      /* If true, NEXUS_Platform_Init will scan the reference board for frontend devices. */
    bool openOutputs;       /* If true, NEXUS_Platform_Init will open video and audio outputs and configure the DAC's. */
    bool openInputs;        /* If true, NEXUS_Platform_Init will open analog video inputs configure the ADC's. */
    bool checkPlatformType; /* If true, NEXUS_Platform_Init will verify nexus and app were compiled for the platform and version.
                               You can set false if your nexus platforms are binary compatibile. */

    /* User-customized heap management.
    NEXUS_Platform_GetDefaultSettings populates the heap[] array for the reference board. It can be modified by the application.
    You can retrieve the NEXUS_HeapHandle's from NEXUS_PlatformConfiguration.heap[].
    Nexus creates heaps according to OS-reported bmem regions. 
    The heap[] array is sparse. The only required heap is heap[0].
    */
    struct {
        unsigned memcIndex; /* memory controller (MEMC) index. */
        unsigned subIndex;  /* addressing region within the MEMC. non-zero only for MEMC's with discontinuous physical 
                               address (for example, register hole). */
        int size;           /* If size is >0, a fixed sized heap is created.
                               If size is -1, all remaining memory in a matching bmem region is allocated to that heap.
                               If size is 0 (default), no heap is created. */
        unsigned alignment; /* Minimum alignment of allocations in this heap. */
        bool guardBanding;  /* If true, add debug guard bands around each allocation.
                               Only applies to CPU-accessible heaps.
                               Set to false if your heap size calculations are precise with no extra space. */
        unsigned memoryType; /* see NEXUS_MEMORY_TYPE bitmasks and NEXUS_MemoryType macros in nexus_types.h */
        bool optional;      /* if true, then allow nexus to init even if memory for this heap cannot be found */
    } heap[NEXUS_MAX_HEAPS];

    /* The reference platform exposes some module settings here for application configuration.
    Customer boards and applications may use this, or may implement their own custom platform code. */
#if NEXUS_HAS_TRANSPORT
    NEXUS_TransportModuleSettings transportModuleSettings;
#endif
#if NEXUS_HAS_DISPLAY
    NEXUS_DisplayModuleSettings displayModuleSettings;
#endif
#if NEXUS_HAS_AUDIO
    NEXUS_AudioModuleSettings audioModuleSettings;
#endif
#if NEXUS_HAS_VIDEO_DECODER
    NEXUS_VideoDecoderModuleSettings videoDecoderModuleSettings;
#endif
#if NEXUS_HAS_SURFACE
    NEXUS_SurfaceModuleSettings surfacePlatformSettings;
#endif
#if NEXUS_HAS_SMARTCARD
    NEXUS_SmartcardModuleSettings smartCardSettings;
#endif
#if NEXUS_HAS_VIDEO_ENCODER
    NEXUS_VideoEncoderModuleSettings videoEncoderSettings;
#endif
#if NEXUS_HAS_SECURITY
    NEXUS_SecurityModuleSettings securitySettings;
#endif
#if NEXUS_HAS_GRAPHICS2D
    NEXUS_Graphics2DModuleSettings graphics2DModuleSettings;
#endif
    NEXUS_PlatformStandbySettings standbySettings;

    NEXUS_FileModulePlatformSettings fileModuleSettings;
} NEXUS_PlatformSettings;


/***************************************************************************
Summary:
Get default settings to pass into NEXUS_Platform_Init.
***************************************************************************/
void NEXUS_Platform_GetDefaultSettings(
    NEXUS_PlatformSettings *pSettings /* [out] */
    );


/***************************************************************************
Summary:
NEXUS_Platform_Init will initialize Nexus

Description:
NEXUS_Platform_Init will initialize all board-specifics and then proceed to
initialize the nexus modules above it.  This is the main entry point
for all applications to start Nexus.

Nexus is a singleton. You cannot call NEXUS_Platform_Init a second time unless
you have first called NEXUS_Platform_Uninit.

Do not call NEXUS_Platform_Init_tagged directly. Instead, call NEXUS_Platform_Init.
This will perform basic version checking to make sure you have a properly configured system.

See Also:
NEXUS_Platform_Uninit
 ***************************************************************************/
NEXUS_Error NEXUS_Platform_Init_tagged(
    const NEXUS_PlatformSettings *pSettings,     /* attr{null_allowed=y} Pass NULL for defaults */
    unsigned platformCheck,                      /* set by NEXUS_Platform_Init macro. only tested if NEXUS_PlatformSettings.checkPlatformType is true. */
    unsigned versionCheck,                       /* set by NEXUS_Platform_Init macro. only tested if NEXUS_PlatformSettings.checkPlatformType is true.  */
    unsigned structSizeCheck                     /* set by NEXUS_Platform_Init macro */
    );

#if NEXUS_PLATFORM_NON_NUMERIC
#define NEXUS_Platform_Init(PSETTINGS) \
    NEXUS_Platform_Init_tagged((PSETTINGS), 0, NEXUS_P_GET_VERSION(NEXUS_PLATFORM), NEXUS_P_GET_STRUCT_SIZES())
#else
#define NEXUS_Platform_Init(PSETTINGS) \
    NEXUS_Platform_Init_tagged((PSETTINGS), NEXUS_PLATFORM, NEXUS_P_GET_VERSION(NEXUS_PLATFORM), NEXUS_P_GET_STRUCT_SIZES())
#endif

/***************************************************************************
Summary:
Uninitialize Nexus

Description:
The user is responsible for closing handles before calling NEXUS_Platform_Uninit.
Some handles can be automatically closed, but some cannot. It is best to explicitly
close all handles that you have opened.

After calling NEXUS_Platform_Uninit, you can all NEXUS_Platform_Init to bring up Nexus
again.
***************************************************************************/
/*
void NEXUS_Platform_Uninit(void) is defined in nexus_platform_client.h, which is included by nexus_platform.h
*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* #ifndef NEXUS_PLATFORM_INIT_H__ */

