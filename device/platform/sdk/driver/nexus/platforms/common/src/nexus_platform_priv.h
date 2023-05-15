/***************************************************************************
*     (c)2004-2014 Broadcom Corporation
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
* $brcm_Workfile: nexus_platform_priv.h $
* $brcm_Revision: PROD_HUMAX_URSR/1 $
* $brcm_Date: 7/4/14 6:49p $
*
* API Description:
*   API name: Platform (private)
*    Private APIs to initialize the a board.
*
* Revision History:
*
* $brcm_Log: /nexus/platforms/common/src/nexus_platform_priv.h $
* 
* PROD_HUMAX_URSR/1   7/4/14 6:49p scho
* SW7425-4944: Preserve wakeup status during S3 cold boot
* 
* 81   10/16/12 10:28a erickson
* SW7425-1792: add core to module list, remove unused code
* 
* 80   10/12/12 11:57a erickson
* SW7425-1792: don't use client on NEXUS_Platform_ReleaseObject
* 
* 79   10/8/12 2:30p vsilyaev
* SW7425-1792: Allow application to control life-cycle of objects
* 
* 78   9/25/12 9:47a erickson
* SW7435-364: monitor for runaway L1 interrupts
* 
* 77   7/26/12 4:05p gmohile
* SW7425-3519,SW7425-3520,SW7425-3077: Add frontend standby
* 
* 76   7/25/12 8:41a erickson
* SW7435-179: rework heap algorithm to pair with bmem regions, not MEMC
*  regions. this allows for multiple, discontinuous bmem regions in each
*  MEMC region.
* 
* 75   7/13/12 11:57a erickson
* SW7552-303: use OS-reported max_dcache_line_size instead of nexus macro
* 
* 74   6/19/12 10:56a erickson
* SW7435-238: core module requires full MEMC base address and offset, not
*  bmem-limited, for MRC to protect upper and lower memory
* 
* 73   6/15/12 3:41p chengs
* SW7584-34: Add 7584 support.
* 
* 72   5/23/12 2:29p bselva
* SW7360-19: Add support for 7360 platform
* 
* 71   5/9/12 3:35p erickson
* SW7231-748: fix #includes
* 
* 70   3/15/12 9:21a erickson
* SW7425-2607: use NEXUS_MAX_XVD_DEVICES
* 
* 69   3/14/12 3:12p erickson
* SW7425-2607: use NEXUS_MAX_MEMC
* 
* 68   3/6/12 1:27p mward
* SW7435-7:  Add 7435 to the BMIPS5000_40NM=1.
* 
* 67   1/24/12 2:44p vsilyaev
* SW7425-2258: Added Nexus video decoder extension to decode MJPEG
* 
* 66   12/22/11 1:26p katrep
* SW7429-1:add kernelmode support
* 
* 65   12/15/11 4:26p vsilyaev
* SW7425-1140: Merge NEXUS_Cec module support into main-line
* 
* SW7425-1140/1   12/9/11 5:11p vle
* SW7425-1140: add NEXUS Cec module
* 
* 64   11/29/11 11:38a erickson
* SW7420-2129: remove unused function
* 
* 63   11/16/11 12:55p erickson
* SW7422-445: read host memory in NEXUS_Platform_GetDefaultSettings so
*  that platform code can adapt to various OS memory configurations
* 
* 62   11/15/11 3:19p erickson
* SW7425-1747: remove old code
* 
* 61   8/8/11 12:25p erickson
* SW7425-1060: add BMIPS3300
* 
* 60   7/21/11 4:14p gmohile
* SW7125-1014 : Rework power management
* 
* 59   6/3/11 9:07a erickson
* SW7422-445: remove unused macro
* 
* 58   6/2/11 1:24p erickson
* SW7420-1819: add NEXUS_Platform_P_TerminateProcess prototype
* 
* 57   5/19/11 3:49p erickson
* SW7422-445: call NEXUS_Platform_P_ReadMemcConfig from
*  NEXUS_Platform_GetDefaultSettings so that heap defaults can be based
*  on board strapping option
* 
* 56   5/17/11 2:15p jhaberf
* SWDTV-6928: Merge of 35233 platform code to the main
* 
* SWDTV-6928/1   5/4/11 6:25p mstefan
* SWDTV-6928: merge to main
* 
* 55   5/3/11 5:08p erickson
* SW7420-1819: repartition so NFE is a standard feature
* 
* 54   4/8/11 11:00a erickson
* SW7420-1671: always call NEXUS_Platform_P_InitServer
* 
* 53   3/16/11 10:27a erickson
* SW7420-1642: use NEXUS_Base_Settings.driverModuleInit instead of
*  NEXUS_Platform_P_InitDone
*
* 52   2/28/11 3:11p erickson
* SW7420-1123: refactor server init/uninit for simpler, cleaner app
*  init/uninit
*
* 51   2/25/11 2:52p gmohile
* SW7408-210 : Deprecate use of NEXUS_POWER_STANDBY
*
* 50   2/24/11 12:02p gmohile
* SW7408-210 : Add BVN standby
*
* 49   2/17/11 4:24p gmohile
* SW7408-210 : Merge PM 2.0 support to mainline
*
* 48   1/19/11 12:04p erickson
* SW7420-1123: default NEXUS_MODULE_SELF in nexus_platform_module.h as
*  normal
*
* 47   12/30/10 1:19p erickson
* SW7422-2: update macros
*
* 46   12/28/10 2:43p erickson
* SW7422-2: define BMIPS5500 in nexus_platform_priv.h based on BCHP_CHIP
*  list
*
* 45   12/27/10 12:01p erickson
* SW7422-2: add NEXUS_Platform_P_GetDefaultSettings (7425 only for now)
*
* 44   12/22/10 2:28p erickson
* SW7422-2: add support for multiple physical address regions within a
*  single MEMC
*
* 43   12/20/10 4:26p erickson
* SW7422-2: rework nexus_platform_core, add board-specific file and API,
*  mmap after determining heaps
*
* 42   12/13/10 7:02p vsilyaev
* SW7425-40: Added system initialization for the StreamMux and
*  VideoEncoder modules
*
* 41   12/3/10 2:51p spothana
* SW7420-1177: Update nexus hdmi dvo.
*
* SW7420-1177/1   11/5/10 3:26p spothana
* SW7420-1177: Update nexus hdmi dvo.
*
* 40   12/3/10 10:02a gmohile
* SW7408-190 : Use conditional compile for frontend
*
* 39   11/22/10 5:32p mward
* SW7125-737: NEXUS_Platform_P_FrontendType(),
*  NEXUS_Platform_P_ShutdownCM(), NEXUS_Platform_P_CMPowerIsOn().
*
* 38   11/19/10 11:29a spothana
* SW7420-1078: Making changes to  get frame buffer API to follow nexus
*  coding protocol.
*
* 37   11/18/10 7:09p spothana
* SW7420-1078: Add get frame buffer heap index
*
* SW7420-1078/1   11/12/10 5:42p spothana
* SW7420-1078: Add get frame buffer heap index
*
* 36   11/12/10 10:29a rgreen
* SW7422-10: Merge Changes
*
* SW7422-10/1   11/10/10 2:13p rgreen
* SW7422-10: Add nexus platform support for hdmiInput
*
* 35   11/10/10 5:01p mward
* SW7125-729: Add NEXUS_Platform_P_IsHostFrontendPinmux() to check the
*  state of I2C control.
*
* 34   10/29/10 5:25p nickh
* SW7422-85: Add support for sharing MEMC1 with Linux
*
* 33   9/24/10 1:30p gskerl
* SW7468-102: Merged back to /main branch
*
* BRUTUS_97468_IBC_PM_DEMO/1   8/26/10 2:47p gskerl
* SW7468-102: Declare g_activeModuleList as extern for 7468
*
* 32   9/17/10 5:56p jtna
* SW7420-972: make use of BCHP_PWR-flavor of power management
*
* 31   8/21/10 2:35p gskerl
* SW7125-237: Corrected typo (missing #)
*
* 30   8/21/10 2:30p gskerl
* SW7125-237: Add more modules to NEXUS_PlatformHandles struct for 7125
*  and 7468
*
* 29   7/30/10 12:19p erickson
* SW3548-2887: add common NEXUS_Platform_P_ConfigureGisbTimeout()
*
* 28   6/24/10 7:17p gskerl
* SW7125-403: Added conditional compilation directives around Active
*  Standby code.
*
* 27   6/23/10 7:08p gskerl
* SW7125-403: Merge Active Standby changes to main branch
*
* 26   6/11/10 3:53p mward
* SW7125-35: Add NEXUS_Platform_P_HostFrontendPinmux() to switch BSC_M3
*  between host and BNM.
*
* 25   4/6/10 3:09p rjain
* SW7550-228: picking up latest PM changes for 7550
*
* 24   3/8/10 4:25p erickson
* SW7405-3986: rename macro to NEXUS_POWER_STANDBY
*
* 23   2/4/10 5:37p jgarrett
* SW7408-57: Refactoring soft audio startup of TP1
*
* 22   1/28/10 4:36p erickson
* SW7420-532: refactor nexus platform standby for linux 2.6.31
*
* SW7420-70/1   1/18/10 5:02p erickson
* SW7420-70: refactor nexus_platform_standby api
*
* 21   1/15/10 6:02p jgarrett
* SW7408-17: Adding backtrace support for TP1
*
* 20   12/29/09 12:51p erickson
* SW7550-146: allow security module to be initialized after the transport
*  module if HW requires it
*
* 19   11/13/09 5:00p erickson
* SW7325-250: add nexus_platform_standby support
*
* 18   11/6/09 9:50a erickson
* SW7405-3350: move surface module handle to NEXUS_PlatformHandles
*
* 17   10/22/09 11:53a erickson
* SW7405-3277: update comments
*
* 16   8/25/09 12:24p erickson
* SW7420-228: rework platform memory mapping to allow for multiple MEMC's
*  on 7420
*
* PR55501/1   8/14/09 7:36p bandrews
* PR55501: kernel mode support for 7420 with memc1 mappable from user
*
* 15   7/2/09 10:46a erickson
* PR56558: multithreaded module initialization
*
* 14   6/23/09 11:59p mphillip
* PR54926: Add a hook to allow customization at the end of
*  NEXUS_Platform_Init
*
* 13   5/12/09 4:33p erickson
* PR52109: added call to NEXUS_Platform_P_UninitInterrupts
*
* 12   5/12/09 4:18p erickson
* PR52109: move g_NEXUS_platformModule for more general use
*
* 11   5/11/09 4:02p jtna
* PR54515: make transport module handle available globally
*
* 10   4/9/09 11:42a jrubio
* PR52188: add support for 2nd Receiver
*
* 9   3/9/09 2:33p erickson
* PR50748: add multi-heap support
*
* 8   1/28/09 6:27p vsilyaev
* PR 51392: Moved magnum initialization from Nexus base to the platform.
*  Initialize only magnum layer inside of
*  NEXUS_Platform_GetDefaultSettings
*
* 7   1/26/09 12:08p erickson
* PR51468: global variable naming convention
*
* 6   9/23/08 4:48p jrubio
* PR46457: need to remove const from the Init function
*
* 5   9/23/08 4:16p erickson
* PR46950: add NEXUS_Platform_P_AtomicUpdateCallback_isr
*
* 4   9/2/08 7:48p katrep
* PR46457: Configure nexus features at the run time based on board strap
*  options
*
* 3   7/23/08 2:16p erickson
* PR44598: NEXUS_Platform_GetDefaultSettings must call NEXUS_Base_Init so
*  that GetDefaultSettings can use Magnum
*
* 2   2/1/08 5:00p vsilyaev
* PR 38682: Fixed prototype for functions without argument
*
* 1   1/18/08 2:17p jgarrett
* PR 38808: Merging to main branch
*
* Nexus_Devel/8   11/8/07 9:17a erickson
* PR36802: use standard NEXUS_HAS_MODULE defines, impl Platform_Uninit
*
* Nexus_Devel/7   11/5/07 2:12p erickson
* PR36725: add NEXUS_PlatformConfiguration, share more code with 97401
*
* Nexus_Devel/6   10/10/07 11:29a jgarrett
* PR 35551: Adding platform internal handle container
*
* Nexus_Devel/5   9/27/07 10:11p jgarrett
* PR 35002: Adding OS interrupt reset
*
* Nexus_Devel/4   9/26/07 5:36p vsilyaev
* PR 34662: Added platform configuration
*
* Nexus_Devel/3   9/24/07 1:00p jgarrett
* PR 35002: Adding core APIs
*
* Nexus_Devel/2   9/21/07 6:26p jgarrett
* PR 35002: Successful 97400 build
*
* Nexus_Devel/1   9/19/07 6:32p jgarrett
* PR 35002: Adding initial 97400 platform
*
***************************************************************************/
#ifndef NEXUS_PLATFORM_PRIV_H__
#define NEXUS_PLATFORM_PRIV_H__

#include "nexus_platform_module.h"
#include "nexus_platform.h"
#include "nexus_platform_features.h"
#include "nexus_memory.h"
#include "priv/nexus_core.h"
#include "nexus_core_init.h"
#if NEXUS_POWER_MANAGEMENT
#include "nexus_platform_standby.h"
#endif

#ifdef NEXUS_FPGA_SUPPORT
#include "bfpga.h"
#endif

#if NEXUS_HAS_SPI_FRONTPANEL
#include "nexus_gpio.h"
#include "nexus_spi.h"
#endif

#ifndef NEXUS_NUM_XVD_DEVICES
/* NEXUS_NUM_XVD_DEVICES is used throughout platform code; this provides backward compatibility */
#define NEXUS_NUM_XVD_DEVICES NEXUS_MAX_XVD_DEVICES
#endif

#include "blst_queue.h"
#include "blst_slist.h"

/* CPU type, TODO: read mips 4380/5000 from sysfs
internally see http://www.sj.broadcom.com/projects/MIPS3300/. */
#if BCHP_CHIP==7422 || BCHP_CHIP==7425 || BCHP_CHIP==7435 || BCHP_CHIP==7429
 #define BMIPS5000_40NM 1
#elif BCHP_CHIP==7420
 #define BMIPS5000_65NM 1
#elif BCHP_CHIP==7344 || BCHP_CHIP==7346 || BCHP_CHIP==7231 || BCHP_CHIP==7584
 #define BMIPS4380_40NM 1
#elif BCHP_CHIP==7401 || BCHP_CHIP==7403 || BCHP_CHIP==7550 || BCHP_CHIP==7552 || BCHP_CHIP==7358 || BCHP_CHIP==7360
 #define BMIPS3300 1
#endif

typedef enum NEXUS_PlatformStandbyLockMode {
    NEXUS_PlatformStandbyLockMode_eNone,         /* Module does not need to be locked in any standby state */
    NEXUS_PlatformStandbyLockMode_ePassiveOnly,  /* Module is locked only in passive state. Unlocked otherwise */
    NEXUS_PlatformStandbyLockMode_eAlways,       /* Module is always locked in any standby state */    
    NEXUS_PlatformStandbyLockMode_eMax
} NEXUS_PlatformStandbyLockMode;

typedef struct NEXUS_Platform_P_ModuleInfo {
    BLST_Q_ENTRY(NEXUS_Platform_P_ModuleInfo) link;
    NEXUS_ModuleHandle module;
    NEXUS_PlatformStandbyLockMode lock_mode; 
    bool powerdown, locked;  
    NEXUS_Error (*standby)(bool, const NEXUS_StandbySettings *); 
    void (*uninit)(void);
} NEXUS_Platform_P_ModuleInfo;

/***************************************************************************
 * Container for platform handles
 ***************************************************************************/
typedef struct NEXUS_PlatformHandles
{    
    NEXUS_PlatformConfiguration config;
    
    NEXUS_ModuleHandle core;
    NEXUS_ModuleHandle videoDecoder;
    NEXUS_ModuleHandle audio;
    NEXUS_ModuleHandle transport;
    NEXUS_ModuleHandle display;
    NEXUS_ModuleHandle file;
    NEXUS_ModuleHandle playback;
    NEXUS_ModuleHandle surface;
    NEXUS_ModuleHandle hdmiOutput;
    NEXUS_ModuleHandle hdmiDvo;        
    NEXUS_ModuleHandle hdmiInput;
    NEXUS_ModuleHandle cec;
    NEXUS_ModuleHandle rfm;
    NEXUS_ModuleHandle frontend;
    NEXUS_ModuleHandle security;
    NEXUS_ModuleHandle dma;        
    NEXUS_ModuleHandle i2c;
    NEXUS_ModuleHandle gpio;
    NEXUS_ModuleHandle pwm;
    NEXUS_ModuleHandle videoEncoder;    

#if NEXUS_HAS_PICTURE_DECODER
    NEXUS_ModuleHandle pictureDecoder;
#endif
    
#ifdef NEXUS_FPGA_SUPPORT
    BFPGA_Handle fpgaHandle;
#endif
    
    BLST_Q_HEAD(handle_head, NEXUS_Platform_P_ModuleInfo) handles;
} NEXUS_PlatformHandles;

/***************************************************************************
Summary:
Initialize any OS-specifics including memory mapping and interrupt handling.
***************************************************************************/
NEXUS_Error NEXUS_Platform_P_InitOS(void);
NEXUS_Error NEXUS_Platform_P_UninitOS(void);
void NEXUS_Platform_P_MonitorOS(void);

/***************************************************************************
Summary:
Initialize OS-specific memory mapping.
This should not be called if you are also calling NEXUS_Platform_P_InitOS.
***************************************************************************/
NEXUS_Error NEXUS_Platform_P_InitOSMem(void);
void NEXUS_Platform_P_UninitOSMem(void);

/**
Summary:
Initialize core module

Description:
This calls into nexus_platform_core.c, which initializes the nexus_core module.
From here, calls into platform-specific code is made. See NEXUS_Platform_P_InitBoard.
**/
NEXUS_Error NEXUS_Platform_P_InitCore(
    NEXUS_PlatformSettings *pSettings
    );

/**
Summary:
Uninitialize the core module
**/
void NEXUS_Platform_P_UninitCore(void);

/**
Summary:
Init board-specific state. See nexus_platform_$(NEXUS_PLATFORM).c.
**/
NEXUS_Error NEXUS_Platform_P_InitBoard(void);

/**
Summary:
Uninit board-specific state. See nexus_platform_$(NEXUS_PLATFORM).c.
**/
void NEXUS_Platform_P_UninitBoard(void);

/**
Summary:
Memory layout of the board for each MEMC and each physical addressing region

Description:
These structure members are arranged to be backward compatible with NEXUS_Platform_P_GetHostMemory, which is implemented per OS.
**/
typedef struct NEXUS_PlatformMemory
{
    /* DDR sizes detected on the board, per MEMC, per physical addressing region */
    struct {
        unsigned length; /* no MEMC if 0 */
        struct {
            /* actual memory */
            unsigned original_base; /* physical address. 0 is valid. */
            unsigned original_length; /* no region if 0 */
            
            unsigned base; /* physical address. 0 is valid. */
            unsigned length; /* no region if 0 */
        } region[NEXUS_NUM_MEMC_REGIONS];
    } memc[NEXUS_MAX_MEMC];

    /* OS's report of usable memory by physical address */
    struct {
        unsigned base; /* physical address */
        unsigned length; /* no region if 0 */
        
        /* correlation of bmem to MEMC */
        unsigned memcIndex; 
        unsigned subIndex;
    } osRegion[NEXUS_MAX_HEAPS];
    
    unsigned max_dcache_line_size; /* reported by OS. BMEM alignment must be >= this to avoid cache coherency bugs. */
} NEXUS_PlatformMemory;

/**
Summary:
Read board strapping option to determine MEMC configuration.

Description:
Sets pMemory->memc[].length.
May modify pSettings (platform settings) based on those strapping options.
Called from NEXUS_Platform_P_InitCore.
**/
NEXUS_Error NEXUS_Platform_P_ReadMemcConfig(
    BREG_Handle hReg,
    NEXUS_PlatformMemory *pMemory, /* [out] */
    NEXUS_PlatformSettings *pSettings /* [out] */
    );

void NEXUS_Platform_P_GetDefaultSettings(
    NEXUS_PlatformSettings *pSettings
    );
    
/**
Summary:
Map register space and return BREG handle
**/
BREG_Handle NEXUS_Platform_P_MapRegisters(void);

/**
Summary:
Unmap register space
**/
void NEXUS_Platform_P_UnmapRegisters(BREG_Handle reg);


/**
Summary:
Convert a physical address into a MEMC and offset into that MEMC
**/
NEXUS_Error NEXUS_Platform_P_ConvertFromPhysicalAddress(
    const NEXUS_PlatformMemory *pMemory,
    unsigned offset, /* physical address */
    unsigned *pMemc, /* MEMC index */
    unsigned *pMemcOffset /* physical offset in DDR for this MEMC */
    );

/***************************************************************************
Summary:
    Map physical memory into virtual space

Description:
Returns:
    Valid address on success, NULL for failure.
See Also:
    NEXUS_Platform_P_UnmapMemory
 ***************************************************************************/
void *NEXUS_Platform_P_MapMemory(
    unsigned long offset,
    unsigned long length,
    bool cached
    );

/***************************************************************************
Summary:
    Unmap a virtual address mapped by NEXUS_Platform_P_MapMemory
 ***************************************************************************/
void NEXUS_Platform_P_UnmapMemory(
    void *pMem,
    unsigned long length
    );


/**
Summary:
Get the memory regions that the OS says can be used by nexus/magnum
**/
NEXUS_Error NEXUS_Platform_P_GetHostMemory(NEXUS_PlatformMemory *pMemory);
NEXUS_Error NEXUS_Platform_P_CalcSubMemc(NEXUS_PlatformMemory *pMemory);

/***************************************************************************
Summary:
Read reserved memory
***************************************************************************/
uint32_t NEXUS_Platform_P_ReadReserved(
    uint32_t physicalAddress
    );

/***************************************************************************
Summary:
Write reserved memory
***************************************************************************/
void NEXUS_Platform_P_WriteReserved(
    uint32_t physicalAddress,
    uint32_t value
    );

/***************************************************************************
Summary:
Read core register
***************************************************************************/
uint32_t NEXUS_Platform_P_ReadCoreReg(
    uint32_t offset
    );

/***************************************************************************
Summary:
Write core register
***************************************************************************/
void NEXUS_Platform_P_WriteCoreReg(
    uint32_t offset,
    uint32_t value
    );

/***************************************************************************
Summary:
Read CMT Control Register
***************************************************************************/
uint32_t NEXUS_Platform_P_ReadCmtControl(void);

/***************************************************************************
Summary:
Write CMT Control Register
***************************************************************************/
void NEXUS_Platform_P_WriteCmtControl(
    uint32_t value
    );

/***************************************************************************
Summary:
    Reset any pending L1 interrupts
 ***************************************************************************/
void NEXUS_Platform_P_ResetInterrupts(void);

/***************************************************************************
Summary:
    Connect an L1 interrupt to the OS
 ***************************************************************************/
NEXUS_Error NEXUS_Platform_P_ConnectInterrupt(
    unsigned irqNum,
    NEXUS_Core_InterruptFunction pIsrFunc,
    void *pFuncParam,
    int iFuncParam
    );

/***************************************************************************
Summary:
    Disconnect an L1 interrupt from the OS
 ***************************************************************************/
void NEXUS_Platform_P_DisconnectInterrupt(
    unsigned irqNum
    );

/***************************************************************************
Summary:
    enable an L1 interrupt
 ***************************************************************************/
NEXUS_Error NEXUS_Platform_P_EnableInterrupt(
    unsigned irqNum
    );

/***************************************************************************
Summary:
    Enable an L1 interrupt
 ***************************************************************************/
NEXUS_Error NEXUS_Platform_P_EnableInterrupt_isr(
    unsigned irqNum
    );

/***************************************************************************
Summary:
    Disable an L1 interrupt
 ***************************************************************************/
void NEXUS_Platform_P_DisableInterrupt(
    unsigned irqNum
    );

/***************************************************************************
Summary:
    Disable an L1 interrupt in isr context
 ***************************************************************************/
void NEXUS_Platform_P_DisableInterrupt_isr(
    unsigned irqNum
    );

/***************************************************************************
Summary:
Configure pin-muxes in a platform-specific way
 ***************************************************************************/
NEXUS_Error NEXUS_Platform_P_InitPinmux(void);

/***************************************************************************
Summary:
Powers off internal cable modem (BNM)
 ***************************************************************************/
NEXUS_Error NEXUS_Platform_P_ShutdownCM(void);

/***************************************************************************
Summary:
Returns chip number of the frontend
 ***************************************************************************/
#if NEXUS_HAS_FRONTEND
unsigned NEXUS_Platform_P_FrontendType(NEXUS_FrontendHandle h);
#endif

/***************************************************************************
Summary:
Returns power-on status of internal cable modem
 ***************************************************************************/
bool NEXUS_Platform_P_CMPowerIsOn(void);

/***************************************************************************
Summary:
Switch BSC_M3 and external interrupts for frontends on it to host control if host==true,
to BNM control otherwise.
 ***************************************************************************/
NEXUS_Error NEXUS_Platform_P_HostFrontendPinmux(bool host);

/***************************************************************************
Summary:
True if pin-muxes for host control of BSC_M3 and the external
frontend(s) on that bus on 97125 platform.  False if BNM control.
 ***************************************************************************/
bool NEXUS_Platform_P_IsHostFrontendPinmux(void);

/***************************************************************************
Summary:
Configure VCXO in a platform-specific way
 ***************************************************************************/
NEXUS_Error NEXUS_Platform_P_InitVcxo(void);

/***************************************************************************
Summary:
    Enable L1 interrupts for a platform
 ***************************************************************************/
NEXUS_Error NEXUS_Platform_P_InitInterrupts(void);
void NEXUS_Platform_P_UninitInterrupts(void);

/***************************************************************************
Summary:
    Post-initialization board configuration
 ***************************************************************************/
NEXUS_Error NEXUS_Platform_P_Config(
    const NEXUS_PlatformSettings *pSettings
    );

/***************************************************************************
Summary:
    Shutdown for NEXUS_Platform_P_Config
 ***************************************************************************/
void NEXUS_Platform_P_Shutdown(void);

/***************************************************************************
Summary:
Atomic update

Description:
This is a callback from the REG basemodule. The nexus_platform_os.c implementation
can chose to synchronize with the OS for any register which is shared between magnum
and the OS. Operation should be:

    uint32_t value;
    lock();
    value = *CONVERT_TO_ADDR(reg);
    value &= ~mask;
    value |= value;
    *CONVERT_TO_ADDR(reg) = value;
    unlock();

 ***************************************************************************/
void NEXUS_Platform_P_AtomicUpdateCallback_isr(
    void *callbackContext,
    uint32_t reg,
    uint32_t mask,
    uint32_t value
    );

/***************************************************************************
Summary:
    This function initializes magnum sub-system that allows to call magnum functtions.
Note:
   This function could be called multiped times, but should initialize magnum only once
 ***************************************************************************/
NEXUS_Error NEXUS_Platform_P_Magnum_Init(void);

/***************************************************************************
Summary:
    This function un-initializes magnum sub-system.
 ***************************************************************************/
void NEXUS_Platform_P_Magnum_Uninit(void);

/***************************************************************************
Summary:
Common function to map user-accessible Platform settings to internal Core settings.
 ***************************************************************************/
NEXUS_Error NEXUS_Platform_P_SetCoreModuleSettings(
    const NEXUS_PlatformSettings *pSettings, /* settings based in by the user */
    const NEXUS_PlatformMemory *pMemory,
    NEXUS_Core_Settings *pCoreSettings /* [out] this structure should already be initialized. this function will populate the heapRegion[] array. */
    );

/***************************************************************************
Summary:
Configure GISB timeout checking
 ***************************************************************************/
void NEXUS_Platform_P_ConfigureGisbTimeout(void);

/**
Internal standby functions
**/
#if NEXUS_POWER_MANAGEMENT
typedef struct NEXUS_PlatformStandbyState {
    NEXUS_PlatformStandbySettings settings;
    bool locked;
} NEXUS_PlatformStandbyState;

extern NEXUS_PlatformStandbyState g_standbyState;

void NEXUS_Platform_P_ResetWakeupDevices(const NEXUS_PlatformStandbySettings *pSettings);
void NEXUS_Platform_P_SetWakeupDevices(const NEXUS_PlatformStandbySettings *pSettings);
void NEXUS_Platform_P_GetStandbyStatus(NEXUS_PlatformStandbyStatus *pStatus);
#endif

/***************************************************************************
Summary:
Based on the RTS settings for each platform, framebuffer for each display
could be placed on any heaps. This API shall return the heap handle
for each frame buffer.
 ***************************************************************************/
NEXUS_HeapHandle NEXUS_Platform_P_GetFramebufferHeap(unsigned displayIndex);

NEXUS_Error NEXUS_Platform_P_InitServer(void);
void NEXUS_Platform_P_UninitServer(void);

void NEXUS_Platform_P_TerminateProcess(unsigned pid);

/***************************************************************************
Summary:
    Initialize Nexus
Description:
    This will initialize all board-specifics
See Also:
 ***************************************************************************/
NEXUS_Error NEXUS_Platform_P_Init( const NEXUS_PlatformSettings *pSettings,
                                           unsigned platformCheck,
                                           unsigned versionCheck, 
                                           unsigned structSizeCheck );

/***************************************************************************
Summary:
Private function for frontend standby/resume
 ***************************************************************************/
NEXUS_Error NEXUS_Platform_P_StandbyFrontend(
    bool enabled, 
    const NEXUS_StandbySettings *pSettings
    );

/***************************************************************************
Summary:
Private function to set standby settings, with option ot reset wakeup status. 
 ***************************************************************************/
NEXUS_Error NEXUS_Platform_P_SetStandbySettings( 
    const NEXUS_PlatformStandbySettings *pSettings,
    bool resetWakeupStatus
    );

/***************************************************************************
Summary:
Global variables for platform state
 ***************************************************************************/
extern NEXUS_PlatformHandles g_NEXUS_platformHandles;
extern NEXUS_PlatformSettings g_NEXUS_platformSettings;
extern NEXUS_PlatformMemory g_platformMemory;


NEXUS_Error NEXUS_Platform_P_AcquireObject(const struct b_objdb_client *client, const NEXUS_InterfaceName *type, void *object);
void NEXUS_Platform_P_ReleaseObject(const NEXUS_InterfaceName *type, void *object);


#endif /* #ifndef NEXUS_PLATFORM_PRIV_H__ */

