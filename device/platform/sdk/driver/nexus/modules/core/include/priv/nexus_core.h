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
* $brcm_Workfile: nexus_core.h $
* $brcm_Revision: 45 $
* $brcm_Date: 10/8/12 1:03p $
*
* Revision History:
*
* $brcm_Log: /nexus/modules/core/include/priv/nexus_core.h $
* 
* 45   10/8/12 1:03p erickson
* SW7425-3782: add NEXUS_FrontendConnector for frontend -> transport
*  connections
* 
* 44   1/30/12 2:12p jtna
* SW7231-630: add nmi, remove vga as wakeup device
* 
* 43   1/27/12 4:40p jtna
* SW7425-1708: add frontend standby option
* 
* 42   1/23/12 4:34p erickson
* SW7435-27: rearrange so nexus_core_convert.c has no public functions
* 
* 41   1/4/12 3:20p erickson
* SW7420-1318: save pointer to BINT_P_IntMap for later lookup
* 
* 40   12/22/11 5:42p vsilyaev
* SW7425-2039: Added framework for controllable reporting of BTRC results
* 
* 39   12/19/11 10:04a erickson
* SW7425-1795: add function to validate cpu accessibility of heaps and
*  addresses
* 
* 38   12/14/11 4:52p jgarrett
* SW7425-1950: Consolidating audio codec conversion routines into core
*  module
* 
* 37   10/7/11 10:40a erickson
* SW7425-1183: merge
* 
* SW7425-1183/1   9/16/11 12:43p gmohile
* SW7425-1183 : Add transport packet wakeup support
* 
* 36   8/23/11 11:36a erickson
* SW7231-345: merge
* 
* SW7231-345/1   8/23/11 11:26a gmohile
* SW7231-345 : Add Deep Sleep (S3) standby mode
* 
* 35   7/21/11 3:00p bandrews
* SW7125-1014: move from pub to priv, only platform code uses it
* 
* SW7125-1014/1   7/20/11 4:37p gmohile
* SW7125-1014 : Rework power management
* 
* 34   1/19/11 5:02p erickson
* SW7422-220: add heap index for better debug info
*
* 33   12/9/10 8:04p vsilyaev
* SW7425-40: Added new types for the video encoder
*
* 32   10/25/10 7:02p vsilyaev
* SW7422-64: Added 3D video format and orientation
*
* 31   9/23/10 2:13p erickson
* SW7420-943: refactor for driver/firmware partitioning
*
* 30   7/14/10 10:30a erickson
* SW7405-4621: all chips now share core/CHIP/include. make
*  nexus_core_features.h chip-generic.
*
* 29   6/23/10 12:10p erickson
* SW7550-453: add NEXUS_Heap_GetHeapHandle
*
* 28   6/11/10 2:31p erickson
* SW3556-1124: add NEXUS_P_RefreshRate_FromFrameRate. removed unneeded
*  function.
*
* 27   2/26/10 2:15p erickson
* SW7325-655: use BMEM_Heap_FreeCached to remove side-effect based
*  NEXUS_Core_P_AddressToHeap
*
* 26   8/25/09 12:11p erickson
* SW7420-228: add NEXUS_MemoryDescriptor
*
* 25   7/23/09 10:36a erickson
* PR57010: move MRC handles from core impl to private API
*
* 24   6/8/09 7:06a erickson
* PR55617: add NEXUS_P_AspectRatio_ToMagnum
*
* 23   5/4/09 3:40p erickson
* PR53373: merge HDMI 1.3a support
*
* PR53373_DeepColor/1   4/16/09 3:58p vle
* PR53373: Add HDMI 1.3a support, deep color, to Brutus/nexus
*
* 22   3/9/09 2:32p erickson
* PR50748: remove nexus multi-heap management
*
* 21   2/23/09 12:13p erickson
* PR52370: reduce compilation dependencies by removing bvdc.h from
*  nexus_core.h
*
* 20   1/26/09 1:54p erickson
* PR51468: global variable naming convention
*
* 19   1/8/09 9:37p erickson
* PR50757: added NEXUS_P_FrameRate_ToMagnum
*
* 18   11/19/08 10:06a erickson
* PR48764: merge in macrovision status
*
* PR48764/1   11/10/08 4:39p chengli
* PR48764 : Add API NEXUS_P_MacrovisionType_FromMagnum
*
* 17   10/30/08 8:49p erickson
* PR47030: added NEXUS_HeapHandle and related functions
*
* 16   10/17/08 12:23p erickson
* PR47030: fix misspelling
*
* 15   9/22/08 9:46a erickson
* PR46063: added NEXUS_P_VideoFormat_SetInfo for custom panel formats
*
* 14   7/3/08 6:27p vsilyaev
* PR 42470: Added stub for BTRC_DECLARE_MODULE
*
* 13   6/30/08 1:44p erickson
* PR44165: allow no NEXUS_CORE_NUM_SECURE_HEAPS define
*
* 12   6/20/08 2:10p erickson
* PR43859: merge
*
* PR43859/1   6/20/08 1:10p dyzhang
* PR43859: add NEXUS_P_VertFreq_FromMagnum to convert bfmt format infor
*  vertical frequency to nexus framerate enum
*
* 11   5/16/08 12:04p erickson
* PR41730: expose NEXUS_Core_P_AddressToHeap in private API
*
* 10   5/8/08 2:48p erickson
* PR42438: derive NEXUS_VideoInputStatus.format for digital decode using
*  common function
*
* 9   5/6/08 1:56p vsilyaev
* PR 42470: Make BTRC_XXX macro always avaliable
*
* 8   5/2/08 10:53a erickson
* PR41314: add SECAM variants
*
* 7   3/27/08 2:43p vsilyaev
* PR 40818: Added settings to allocate shared heap
*
* 6   3/26/08 11:44p vsilyaev
* PR 40410: Added more video settings for HDMI Input interface
*
* 5   3/26/08 12:51p erickson
* PR40567: added pixel format functions
*
* PR40567/1   3/26/08 6:22p dyzhang
* PR40567: add NEXUS_P_PixelFormat_FromMagnum and
*  NEXUS_P_PixelFormat_ToMagnum
*
* 4   3/21/08 11:10a erickson
* PR40744: added NEXUS_P_FrameRate_FromMagnum
*
* 3   1/23/08 8:37p vobadm
* PR35457: update docs
*
* 2   1/23/08 5:16p erickson
* PR35457: update docs
*
* 1   1/18/08 2:12p jgarrett
* PR 38808: Merging to main branch
*
* Nexus_Devel/13   1/10/08 9:56a erickson
* PR36814: BAVC_Colorspace doesn't exist for 7401
*
* Nexus_Devel/12   1/9/08 12:29p erickson
* PR36814: added NEXUS_P_ColorSpace_FromMagnum
*
* Nexus_Devel/11   1/9/08 10:43a erickson
* PR38470: add NEXUS_P_AspectRatio_FromMagnum
*
* Nexus_Devel/10   12/28/07 5:05p erickson
* PR38470: add NEXUS_VideoFormat_GetInfo to public API in
* nexus_core_utils.h
*
* Nexus_Devel/9   11/13/07 5:31p erickson
* PR35457: documentation update
*
* Nexus_Devel/8   9/21/07 6:26p jgarrett
* PR 35002: Adding interrupt param for BINT_Isr
*
* Nexus_Devel/7   9/18/07 5:42p jgarrett
* PR 35002: Adding nexus_core_features.h
*
* Nexus_Devel/6   9/13/07 4:23p vsilyaev
* PR 34419: Fixed name of initialization header file
*
* Nexus_Devel/5   9/13/07 2:37p vsilyaev
* PR 34419: Updated for new directory layout
*
* Nexus_Devel/4   9/10/07 5:34p vsilyaev
* PR 34419: Added array of memory heaps
*
* Nexus_Devel/3   9/5/07 2:47p vsilyaev
* PR 34419: Added interrupt interface
*
* Nexus_Devel/2   8/31/07 6:02p vsilyaev
* PR 34419: Added configuration structure for NEXUS_Base
*
* Nexus_Devel/1   8/28/07 12:46p vsilyaev
* PR 34254: Split system/board functionality
*
***************************************************************************/
#ifndef NEXUS_CORE_H
#define NEXUS_CORE_H

#include "nexus_platform_features.h"
#include "priv/nexus_core_features.h"
#include "nexus_core_init.h"
#include "nexus_core_utils.h"
#include "nexus_types.h"
#include "nexus_video_types.h"
#include "nexus_memory.h"

/* NOTE: try to keep this list of header files generic. we don't want lots of
system-wide compilation dependencies. */
#include "bint.h"
#include "bint_plat.h"
#include "bchp.h"
#include "breg_mem.h"
#include "btmr.h"
#include "bmem.h"
#include "bavc.h"
#include "bavc_hdmi.h"
#include "bpxl.h"
#include "bmrc_monitor.h"

#if B_HAS_TRC
#include "btrc.h"
void NEXUS_Core_Btrc_Report(const BTRC_Module *module, const char *moduleName);
#define NEXUS_BTRC_REPORT(module)  NEXUS_Core_Btrc_Report(BTRC_MODULE_HANDLE(module),#module)
#else
#define BTRC_MODULE_HANDLE(x)
#define BTRC_MODULE(x,y) extern int btrc_unused
#define BTRC_MODULE_DECLARE(x) extern int btrc_unused
#define NEXUS_BTRC_REPORT(L1_ISR) (void)0
#define BTRC_Module_Report(x)
#define BTRC_Module_Reset(x)
#define BTRC_TRACE(x, y)
#endif

#ifdef __cplusplus
extern "C"
{
#endif

/*=************************
This private Core interface provides Magnum basemodules to Nexus modules.
They are accesible using the global variable g_pCoreHandles.

Core also provides an api for L1 interrupt management.
**************************/

typedef struct NEXUS_Core_MagnumHandles
{
    BREG_Handle         reg;
    BCHP_Handle         chp;
    BTMR_Handle         tmr;
    BINT_Handle         bint;
    const BINT_P_IntMap *bint_map;
    BMEM_ModuleHandle   mem;
    BMEM_Heap_Handle    heap[NEXUS_MAX_HEAPS]; /* magnum heap. each magnum heap is owned by exactly one nexus heap. */
    NEXUS_HeapHandle    nexusHeap[NEXUS_MAX_HEAPS]; /* nexus heap. each nexus heap has exactly one magnum heap. */
    struct {
        BMRC_Monitor_Handle rmm;
        BMRC_Handle mrc;
        BMEM_MonitorInterface mem_monitor;
    } memc[NEXUS_MAX_HEAPS]; /* index of this array is memc index, which must be < NEXUS_MAX_HEAPS */
} NEXUS_Core_MagnumHandles;

/**
Summary:
Global variable for obtaining Core handles

Description:
This is usable after init
**/
extern const NEXUS_Core_MagnumHandles *g_NEXUS_pCoreHandles;

#define g_pCoreHandles g_NEXUS_pCoreHandles


/***************************************************************************
Summary:
Standby mode used in NEXUS_StandbySettings
***************************************************************************/
typedef enum NEXUS_StandbyMode
{
    NEXUS_StandbyMode_eOn,          /* Normal mode of operation. Also known as S0 mode. */
    NEXUS_StandbyMode_eActive,      /* Frontend and transport modules are running. All other modules are put to sleep.
                       The same wakeup devices as ePassive are available. 
                       The application cannot put the MIPS to sleep in this mode. Also known as S1 mode*/    
    NEXUS_StandbyMode_ePassive,     /* Low power mode where the AV cores are clock gated. Code remains resident.
                       IrInput, UhfInput, HdmiOutput (CEC), Gpio and Keypad are available to be configured as wakeup devices. 
                       Application must call OS to put the MIPS to sleep. Also known as S2 mode. */
    NEXUS_StandbyMode_eDeepSleep,   /* Lowest power setting. Entire chip is power gated except for AON block. Code remains resident.
                       IrInput, UhfInput, HdmiOutput (CEC), Gpio and Keypad are available to be configured as wakeup devices. 
                       Application must call OS to put the MIPS to sleep. Also known as S3 mode. */
    NEXUS_StandbyMode_eMax
} NEXUS_StandbyMode;

/***************************************************************************
Summary:
Settings used for module standby api
***************************************************************************/
typedef struct NEXUS_StandbySettings
{
    NEXUS_StandbyMode mode;
    struct {    
        bool ir;
        bool uhf;
        bool keypad;
        bool gpio;
        bool nmi;
        bool cec; 
        bool transport;
        unsigned timeout; /* in seconds */
    } wakeupSettings;  
    bool openFrontend; /* If true, NEXUS_Platform_SetStandbySettings will initialize the frontend. */
} NEXUS_StandbySettings;

/***************************************************************************
Summary:
Enable an L1 interrupt.

Description:
This function will enable a level 1 (L1) interrupt.

Returns:
BERR_SUCCESS - The interrupt has been enabled.
BERR_INVALID_PARAMETER - An invalid parameter was passed.
BERR_OS_ERROR - An OS error occurred.

****************************************************************************/
BERR_Code NEXUS_Core_EnableInterrupt(
    unsigned irqNum
    );

/***************************************************************************
Summary:
Enable an L1 interrupt.

Description:
This function will enable a level 1 (L1) interrupt.

Returns:
BERR_SUCCESS - The interrupt has been enabled.
BERR_INVALID_PARAMETER - An invalid parameter was passed.
BERR_OS_ERROR - An OS error occurred.

****************************************************************************/
BERR_Code NEXUS_Core_EnableInterrupt_isr(
    unsigned irqNum
    );

/***************************************************************************
Summary:
Disable an L1 interrupt.

Description:
This function will disable a level 1 (L1) interrupt.

****************************************************************************/
void NEXUS_Core_DisableInterrupt(
    unsigned irqNum
    );

/***************************************************************************
Summary:
Disable an L1 interrupt.

Description:
This function will disable a level 1 (L1) interrupt.

Returns:
BERR_SUCCESS - The interrupt has been disabled.
BERR_INVALID_PARAMETER - An invalid parameter was passed.
BERR_OS_ERROR - An OS error occurred.

****************************************************************************/
void NEXUS_Core_DisableInterrupt_isr(
    unsigned irqNum
    );

/***************************************************************************
Summary:
Connect an L1 interrupt.

Description:
This function will register a callback function for a level 1 (L1)
interrupt.

Returns:
BERR_SUCCESS - The interrupt has been disabled.
BERR_INVALID_PARAMETER - An invalid parameter was passed.
BERR_OS_ERROR - An OS error occurred.

****************************************************************************/
BERR_Code NEXUS_Core_ConnectInterrupt(
    unsigned irqNum,
    NEXUS_Core_InterruptFunction pIsrFunc,
    void *pFuncParam,
    int iFuncParam
    );

/***************************************************************************
Summary:
Disconnect an L1 interrupt.

Description:
This function will un-register a callback function for a level 1 (L1)
interrupt.

****************************************************************************/
void NEXUS_Core_DisconnectInterrupt(
    unsigned irqNum
    );

/**
Summary:
Common Nexus-to-Magnum conversion functions
**/
NEXUS_Error       NEXUS_P_TransportType_ToMagnum(NEXUS_TransportType transportType, BAVC_StreamType *streamType);
NEXUS_Error       NEXUS_P_VideoFormat_ToMagnum(NEXUS_VideoFormat format, BFMT_VideoFmt *mformat);
NEXUS_VideoFormat NEXUS_P_VideoFormat_FromMagnum(BFMT_VideoFmt format);
NEXUS_VideoFormat NEXUS_P_VideoFormat_FromInfo(unsigned height, unsigned frameRate, bool interlaced);
NEXUS_AspectRatio NEXUS_P_AspectRatio_FromMagnum(BFMT_AspectRatio eAspectRatio);
BFMT_AspectRatio  NEXUS_P_AspectRatio_ToMagnum(NEXUS_AspectRatio aspectRatio);
BFMT_Orientation NEXUS_P_VideoOrientation_ToMagnum(NEXUS_VideoOrientation orientation);
NEXUS_VideoOrientation NEXUS_P_VideoOrientation_FromMagnum(BFMT_Orientation orientation);
BAVC_VideoCompressionStd NEXUS_P_VideoCodec_ToMagnum(NEXUS_VideoCodec codec, NEXUS_TransportType transportType);
NEXUS_VideoCodec NEXUS_P_VideoCodec_FromMagnum(BAVC_VideoCompressionStd std);
BAVC_AudioCompressionStd NEXUS_P_AudioCodec_ToMagnum(NEXUS_AudioCodec codec);
NEXUS_AudioCodec NEXUS_P_AudioCodec_FromMagnum(BAVC_AudioCompressionStd std);

#if BCHP_CHIP != 7401
NEXUS_ColorSpace  NEXUS_P_ColorSpace_FromMagnum(BAVC_Colorspace colorSpace);
BAVC_Colorspace NEXUS_P_ColorSpace_ToMagnum(NEXUS_ColorSpace colorSpace);
#endif
NEXUS_HdmiColorDepth NEXUS_P_HdmiColorDepth_FromMagnum(BAVC_HDMI_BitsPerPixel eBitsPerPixel);
BAVC_HDMI_BitsPerPixel NEXUS_P_HdmiColorDepth_ToMagnum(NEXUS_HdmiColorDepth colorDepth);

NEXUS_VideoFrameRate NEXUS_P_FrameRate_FromMagnum(BAVC_FrameRateCode magnumFramerate);
NEXUS_Error NEXUS_P_FrameRate_ToMagnum(NEXUS_VideoFrameRate frameRate, BAVC_FrameRateCode *pMagnumFramerate);
void NEXUS_P_LookupFrameRate( unsigned frameRateInteger, NEXUS_VideoFrameRate *pNexusFrameRate );
unsigned NEXUS_P_RefreshRate_FromFrameRate(NEXUS_VideoFrameRate frameRate);
NEXUS_PixelFormat NEXUS_P_PixelFormat_FromMagnum(BPXL_Format magnumPixelFormat);
NEXUS_Error       NEXUS_P_PixelFormat_ToMagnum(NEXUS_PixelFormat nexusPixelFormat, BPXL_Format *magnumPixelFormat);

typedef struct NEXUS_PixelFormatInfo {
    /* NOTE: do not re-order these members. they are initialized statically and your compiler may not warn you. */
    bool rgb;
    bool palette; /* NOTE: All palette pixel formats are ARGB8888. */
    bool alpha_per_pixel;
    uint8_t bpp; /* bits per pixel */
    BPXL_Format magnum;
    NEXUS_PixelFormat nexus;
} NEXUS_PixelFormatInfo;

const NEXUS_PixelFormatInfo *NEXUS_PixelFormat_GetInfo(NEXUS_PixelFormat pixelFormat);

/**
Summary:
Easy Nexus lookup functions which can't be public because of coding convention.
See NEXUS_VideoFormat_GetInfo for public version.
**/
bool NEXUS_P_VideoFormat_IsSd(NEXUS_VideoFormat format);
bool NEXUS_P_VideoFormat_IsInterlaced(NEXUS_VideoFormat format);

NEXUS_Error NEXUS_Memory_P_ConvertAlignment(
    unsigned alignment,
    unsigned *pAlignmentExponent /* [out] */
    );

/**
Summary:
Retrieve the heap for a cached address.
This function always has a second purpose of converting to an uncached address
**/
BMEM_Heap_Handle NEXUS_Core_P_AddressToHeap(
    void *pAddress,          /* cached address */
    void **ppUncachedAddress /* [out] optionally, uncached address. passing NULL is allowed. */
    );

/**
Summary:
Set information about custom formats so that NEXUS_VideoFormat_GetInfo can return it.

Description:
See NEXUS_Display_SetCustomFormatSettings for the custom video format API.
This function only helps nexus state, not magnum state.
For now, only one custom video format is stored.
**/
void NEXUS_P_VideoFormat_SetInfo(
    NEXUS_VideoFormat customVideoFormat,
    const NEXUS_VideoFormatInfo *pVideoFmtInfo    /* [out] */
    );

/***************************************************************************
Summary:
Get the magnum MEM heap from the Nexus heap
***************************************************************************/
BMEM_Heap_Handle NEXUS_Heap_GetMemHandle(
    NEXUS_HeapHandle heap
    );

/***************************************************************************
Summary:
Get the Nexus heap from the magnum MEM heap
***************************************************************************/
NEXUS_HeapHandle NEXUS_Heap_GetHeapHandle(
    BMEM_Heap_Handle mem
    );

/***************************************************************************
Summary:
Get default heap settings for a memory controller

Description:
The implementation of this function will have chip-specific information.
***************************************************************************/
void NEXUS_Heap_GetDefaultMemcSettings(
    unsigned memcIndex,
    NEXUS_Core_MemoryRegion *pSettings /* [out] */
    );

/***************************************************************************
Summary:
Create a heap
***************************************************************************/
NEXUS_HeapHandle NEXUS_Heap_Create(
    unsigned index, /* have to pass in the index because the heap[] array is sparse, so we need the debug information of where it's going */
    BMEM_ModuleHandle mem,
    const NEXUS_Core_MemoryRegion *pSettings
    );

/***************************************************************************
Summary:
Destroy a heap
***************************************************************************/
void NEXUS_Heap_Destroy(
    NEXUS_HeapHandle heap
    );
    
/* returns true if heap has driver mmapping (cached and uncached) */
bool NEXUS_P_CpuAccessibleHeap(
    NEXUS_HeapHandle heap
    );

/* returns true if address is CPU accessible in this execution context (not fake) */
bool NEXUS_P_CpuAccessibleAddress(
    void *address /* cached or uncached address */
    );
    
NEXUS_OBJECT_CLASS_DECLARE(NEXUS_FrontendConnector);

#ifdef __cplusplus
}
#endif

#endif /* NEXUS_CORE_H */

