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
* $brcm_Workfile: nexus_platform_settings.c $
* $brcm_Revision: 107 $
* $brcm_Date: 10/29/12 3:33p $
*
* API Description:
*   API name: Platform
*   Functions that should be compiled both in linux kernel and linux user/proxy environment
*
* Revision History:
*
* $brcm_Log: /nexus/platforms/common/src/nexus_platform_settings.c $
* 
* 107   10/29/12 3:33p jgarrett
* SW7425-4157: Adding audio_logs_enabled=y
* 
* 106   10/5/12 11:53a erickson
* SW7435-400: remove unused code
* 
* 105   8/24/12 4:18p vishk
* SW7358-345: Merge changes to mainline. Change the channel indexing to
*  include spi core type.
* 
* 104   8/20/12 1:57p bselva
* SW7358-400: Add support for 7218 platform
* 
* 103   8/1/12 11:12a erickson
* SW7435-279: add NEXUS_PlatformSettings.graphics2DModuleSettings
* 
* 102   7/30/12 12:22p erickson
* SW7435-179: fix 7400 memc[2] support
* 
* 101   7/25/12 8:41a erickson
* SW7435-179: rework heap algorithm to pair with bmem regions, not MEMC
*  regions. this allows for multiple, discontinuous bmem regions in each
*  MEMC region.
* 
* 100   7/13/12 11:57a erickson
* SW7552-303: use OS-reported max_dcache_line_size instead of nexus macro
* 
* 99   7/5/12 5:27p bandrews
* SW7231-876: fixed warnings
* 
* 98   6/11/12 12:51p erickson
* SW7425-3028: rework supportedCodecs[] to that array index is actually
*  NEXUS_VideoCodec
* 
* 97   5/14/12 5:46p erickson
* SW7435-179: concat adjacent bmem regions
* 
* 96   5/14/12 3:46p mward
* SW7231-748: fix #includes
* 
* 95   5/14/12 2:42p erickson
* SW7435-179: add boolean for optional heap creation, fail if non-
*  optional heap can't be created
* 
* 94   3/27/12 4:20p erickson
* SW7425-2607: allow binary compatible platform api's
* 
* 93   3/15/12 9:21a erickson
* SW7425-2607: use NEXUS_MAX_XVD_DEVICES
* 
* 92   3/14/12 3:12p erickson
* SW7425-2607: use NEXUS_MAX_MEMC
* 
* 91   3/1/12 5:57p vishk
* SW7231-68: Setting the default for openSpi to false for backward
*  compatibility.
* 
* 90   2/24/12 6:13p vishk
* SW7231-68: add SPI support on top of I2C for 3461 interface
* 
* 89   2/21/12 2:19p erickson
* SW7425-2130: fix debug warnings
*
* 88   1/26/12 1:24p katrep
* SW7422-2:compiler warnig in DEBUG=n builds
*
* 87   11/18/11 1:49p mward
* SW7422-445:  Compiler warning for initialized i.
*
* 86   11/16/11 12:55p erickson
* SW7422-445: read host memory in NEXUS_Platform_GetDefaultSettings so
*  that platform code can adapt to various OS memory configurations
*
* 85   11/15/11 3:19p erickson
* SW7425-1747: remove old code
*
* 84   11/3/11 6:53p jgarrett
* SW7422-400: Enabling audio debug logs on 40nm APE chips
*
* 83   10/20/11 10:09a erickson
* SW7422-2: print memory type string instead of integer
*
* 82   9/19/11 11:47a jrubio
* SW7335-1356: 7335/7325 using 2618 kernel
*
* 81   9/6/11 2:43p erickson
* SWNOOS-482: pass through file module settings in user mode
*
* 80   9/6/11 9:44a erickson
* SW7425-1231: if NEXUS_Platform_GetDefaultSettings is called more than
*  once, just return the first values
*
* 79   8/26/11 2:51p erickson
* SW7422-2: don't create heaps for non-existant regions
*
* 78   8/12/11 4:39p mward
* SW7425-1060: On 7400 MIPS is also restricted to only MEMC0.
*
* 77   8/8/11 12:28p erickson
* SW7425-1060: revise exceptions to bmem restrictions. general case needs
*  to strictly follow linux bmem. code specific exceptions using MIPS
*  type when possible.
*
* 76   8/8/11 5:57p xhuang
* SW7552-95: Add uppder 256M memory access for 7552
*
* 75   8/2/11 2:53p bselva
* SW7358-85: Added the support to have upper 256 MB available for BMEM
*
* 74   7/11/11 11:42a erickson
* SWDTV-7208: remove DTV exception
*
* 73   6/1/11 10:03a erickson
* SWDTV-6928: fix NEXUS_Platform_P_SetCoreModuleSettings debug code
*
* 72   5/25/11 9:54a erickson
* SW7346-214: add security module settings to NEXUS_PlatformSettings
*
* 71   5/19/11 3:49p erickson
* SW7422-445: call NEXUS_Platform_P_ReadMemcConfig from
*  NEXUS_Platform_GetDefaultSettings so that heap defaults can be based
*  on board strapping option
*
* 70   5/17/11 2:15p jhaberf
* SWDTV-6928: Merge of 35233 platform code to the main
*
* SWDTV-6928/1   5/4/11 6:25p mstefan
* SWDTV-6928: merge to main
*
* 66   4/7/11 7:53p katrep
* SW7405-5229:add support multipe heaps
*
* 65   4/7/11 4:03p spothana
* SW7420-1734 : Use MEMC1 regions in the nexus even if OS doesn't provide
*  any information.
*
* 64   4/6/11 2:16p mward
* SW7125-899:  Added NEXUS_Platform_P_GetDefaultSettings() for 7125.
*
* 63   4/1/11 4:31p jtna
* SW7346-119: fix botched merge
*
* 61   4/1/11 4:25p erickson
* SW7420-1734: rework heap calc algorithm so that whole memc regions can
*  be skipped if not made available by the OS, and that heap[0] is
*  aliased if not available
*
* 60   3/7/11 12:16p erickson
* SW7420-1123: set default NEXUS_MemoryType to eFull
*
* 59   3/4/11 11:23a erickson
* SW7400-3006: rework algo for OS region to limit nexus region
*
* 58   2/25/11 2:52p gmohile
* SW7408-210 : Deprecate use of NEXUS_POWER_STANDBY
*
* 57   2/17/11 4:24p gmohile
* SW7408-210 : Merge PM 2.0 support to mainline
*
* 56   2/15/11 11:08a agin
* SWNOOS-441:  Support UCOS.
*
* 55   1/19/11 5:08p erickson
* SW7422-220: allow heap[] array to be sparse
*
* 54   1/19/11 12:03p erickson
* SW7420-1123: default heaps to NEXUS_MemoryType_eFull
*
* 53   1/7/11 5:52p mward
* SW7422-2:  Use NEXUS_Platform_P_GetDefaultSettings() for 7400.
*
* 52   1/4/11 9:15a erickson
* SW7422-2: add chip
*
* 51   12/30/10 2:44p erickson
* SW7422-2: extend BCHP_CHIP list
*
* 50   12/27/10 2:37p erickson
* SW7422-2: create implicit heaps for remainder of unused memory
*
* 49   12/27/10 1:42p erickson
* SW7422-2: add NEXUS_Platform_P_GetDefaultSettings
*
* 48   12/27/10 12:01p erickson
* SW7422-2: add NEXUS_Platform_P_GetDefaultSettings (7425 only for now)
*
* 47   12/27/10 11:12a erickson
* SW7422-2: modify 7425 defaults
*
* 46   12/27/10 10:40a erickson
* SW7422-2: improve debug, added temp 7425 settings
*
* 45   12/22/10 3:09p erickson
* SW7422-2: don't create zero-sized default heaps
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
* 41   11/19/10 11:29a spothana
* SW7420-1078: Making changes to  get frame buffer API to follow nexus
*  coding protocol.
*
* 40   11/18/10 7:11p spothana
* SW7420-1078: Get frame buffer heap index per display for each platform.
*
* SW7420-1078/2   11/12/10 5:38p spothana
* SW7420-1078: Get frame buffer heap index per display for each platform.
*
* 39   10/26/10 10:40p agin
* SWNOOS-429:  Changed NO_OS to NO_OS_DIAGS.
*
* 38   9/30/10 11:24a vsilyaev
* SW7422-65: User per AVD core heap sizes
*
* 37   9/27/10 1:33a agin
* SWNOOS-425:  Remove 97125 from the list of boards for openFrontend =
*  false for NO_OS.
*
* 36   8/13/10 12:11a agin
* SWNOOS-419:  Add 97420 to list of boards for openFrontend = false for
*  NO_OS.
*
* 35   8/4/10 10:57a VISHK
* SW7335-813: Add smartcard module settings to the Platform settings.
*
* 34   4/22/10 6:23p mward
* SW7405-3832: switch to NEXUS_PLATFORM macro
*
* 33   4/21/10 9:29a erickson
* SW7420-703: pass NEXUS_SurfaceModuleSettings through
*  NEXUS_PlatformSettings
*
* 32   4/14/10 6:44p jtna
* SW7125-323: Coverity Defect ID:20438 RESOURCE_LEAK
*
* 31   4/1/10 3:22p rjain
* SW7550-309: merging back to main
*
* SW7550-74/2   2/22/10 2:14p nitinb
* SW7550-237: Enabled VC1 and AVS for 7550
*
* SW7550-74/1   1/14/10 11:14a chengli
* SW7550-74 : disable H.264 AVC for 97530 CRB
*
* 30   3/10/10 3:01p erickson
* SW7405-3965: deprecrate NEXUS_VideoDecoderModuleSettings.numDecodes,
*  etc.
*
* 29   2/25/10 9:41a erickson
* SW7420-549: use MEMC defaults to enforce minimum alignment, device-
*  access requirements
*
* 28   2/23/10 2:33p vishk
* SW7335-681: Debug module used before being initialized in
*  NEXUS_Platform_Init().
*
* 27   1/6/10 2:50p garetht
* SW7420-534: Port 7043 demo software to 93380sms (7420 + 3380) platform
*
* 26   12/14/09 7:24p agin
* SWNOOS-385:  Support 7125 for no-os.
*
* 25   12/10/09 8:24p mphillip
* SW7550-112: Merge 7550 changes to main branch
*
* Refsw_7550/2   12/3/09 2:20a nitinb
* SW7550-29: Enabled H.264 codec for 7550
*
* Refsw_7550/1   11/17/09 5:49a nitinb
* SW7550-29: Configuring video decoder setting for 7550
*
* 24   11/10/09 1:37p erickson
* SWDEPRECATED-3725: add WRN if cannot create implict heaps for all
*  MEMC's
*
* 23   10/15/09 4:32p erickson
* SWDEPRECATED-3881: remove default values for unused members
*
* 22   8/31/09 2:01p jgarrett
* SW7405-2928: Adding audio capture settings
*
* 21   8/26/09 12:02p mward
* SW7125-4: Adding 97019 platform.
*
* 20   7/31/09 9:40a gmohile
* PR 56512 : Merge 7401/03 support to mainline
*
* 19   7/13/09 10:11a agin
* PR56698: For no-os, don't set openFrontend to true for cable boards.
*
* 18   5/22/09 11:35a vishk
* PR 54842: rockford/unittests/nexus/general/boot should test
*  NEXUS_PlatformSettings options
*
* 17   4/21/09 10:53a erickson
* PR54395: added NULL check
*
* 16   3/30/09 2:55p erickson
* PR53426: extend nexus multi-heap platform code to allow -1 (remainder)
*  in any place in the heap list
*
* 15   3/19/09 4:59p erickson
* PR53323: add more #if so that more modules can be optionally removed
*
* 14   3/17/09 1:57p erickson
* PR53327: fix memory leak on failure path
*
* 13   3/9/09 2:33p erickson
* PR50748: add multi-heap support
*
* 12   1/28/09 6:27p vsilyaev
* PR 51392: Moved magnum initialization from Nexus base to the platform.
*  Initialize only magnum layer inside of
*  NEXUS_Platform_GetDefaultSettings
*
* 11   1/26/09 12:36p erickson
* PR51468: global variable naming convention
*
* 8   9/2/08 7:48p katrep
* PR46457: Configure nexus features at the run time based on board strap
*  options
*
* 7   8/22/08 7:07p erickson
* PR45795: add buffer counts to DisplayModuleSettings
*
* 6   7/30/08 11:19a erickson
* PR44919: add NEXUS_DISPLAY_SD_ONLY option
*
* 5   7/28/08 3:23p erickson
* PR45197: add NEXUS_VIDEO_DECODER_SD_ONLY option
*
* 4   7/23/08 5:13p erickson
* PR44598: use nexus_platform_features to default video decoder heap size
*
* 3   7/23/08 2:16p erickson
* PR44598: NEXUS_Platform_GetDefaultSettings must call NEXUS_Base_Init so
*  that GetDefaultSettings can use Magnum
*
* 2   7/22/08 3:03p erickson
* PR44598: move some module settings into NEXUS_PlatformSettings to allow
*  app to customize init.
*
* 1   7/1/08 6:28p vsilyaev
* PR 44452: Location of NEXUS_Platform_GetDefaultSettings
*
***************************************************************************/
#include "nexus_platform_priv.h"
#include "nexus_types.h"
#include "nexus_base.h"
#include "priv/nexus_core.h"
#include "nexus_platform.h"
#if NEXUS_HAS_FILE
#include "nexus_file_init.h"
#endif

BDBG_MODULE(nexus_platform_settings);

static void NEXUS_Platform_P_LimitMemcRegions(NEXUS_PlatformMemory *pMemory);

void NEXUS_Platform_GetDefaultSettings( NEXUS_PlatformSettings *pSettings )
{
    static bool set = false;
    static NEXUS_PlatformSettings defaultSettings;
    NEXUS_Error errCode;
    unsigned i;
    NEXUS_PlatformMemory *pMemory = &g_platformMemory;

    if (set) {
        /* if NEXUS_Platform_GetDefaultSettings is called more than once, just return the first values */
        *pSettings = defaultSettings;
        return;
    }

    errCode = NEXUS_Platform_P_Magnum_Init();
    if(errCode!=BERR_SUCCESS) {
        return;
    }

    BDBG_ASSERT(NULL != pSettings);
    BKNI_Memset(pSettings, 0, sizeof(*pSettings)); /* don't call BKNI_Memset prior to initializing magnum */

    pSettings->cachedMemory = true; /* Default to cached memory */

    pSettings->openI2c = true;
    #if (NEXUS_PLATFORM==97019)
    pSettings->openFpga = false;
    #else
    pSettings->openFpga = true;
    #endif
    #if defined(DIAGS) && ((BCM_BOARD==97420) || (BCM_BOARD==97456) || (BCM_BOARD==97459))
    pSettings->openFrontend = false;
    #else
    pSettings->openFrontend = true;
    #endif
    pSettings->openOutputs = true;
    pSettings->checkPlatformType = true;
    if (NEXUS_GetEnv("nexus_binary_compat")) {
        pSettings->checkPlatformType = false;
    }

#if NEXUS_HAS_TRANSPORT
    NEXUS_TransportModule_GetDefaultSettings(&pSettings->transportModuleSettings);
#endif
#if NEXUS_HAS_DISPLAY
    NEXUS_DisplayModule_GetDefaultSettings(&pSettings->displayModuleSettings);

    /* Should Not be defined for 7405/7325/7335 and variants,its done on platfrom_core.c
       based on board strap options */
#if (NEXUS_DISPLAY_NUM_SD_BUFFERS || NEXUS_DISPLAY_NUM_HD_BUFFERS || NEXUS_DISPLAY_NUM_FULL_HD_BUFFERS)
    pSettings->displayModuleSettings.fullHdBuffers.count = NEXUS_DISPLAY_NUM_FULL_HD_BUFFERS;
    pSettings->displayModuleSettings.hdBuffers.count = NEXUS_DISPLAY_NUM_HD_BUFFERS;
    pSettings->displayModuleSettings.sdBuffers.count = NEXUS_DISPLAY_NUM_SD_BUFFERS;
#endif
#endif


#if NEXUS_HAS_VIDEO_DECODER
    NEXUS_VideoDecoderModule_GetDefaultSettings(&pSettings->videoDecoderModuleSettings);
#if NEXUS_VIDEO_DECODER_GENERAL_HEAP_SIZE
    /* this method for setting default heap sizes is deprecated. instead, each platform's nexus_platform_$(NEXUS_PLATFORM).c sets default. */
    pSettings->videoDecoderModuleSettings.heapSize[0].general = NEXUS_VIDEO_DECODER_GENERAL_HEAP_SIZE;
    pSettings->videoDecoderModuleSettings.heapSize[0].secure = NEXUS_VIDEO_DECODER_SECURE_HEAP_SIZE;
    pSettings->videoDecoderModuleSettings.heapSize[0].picture = NEXUS_VIDEO_DECODER_PICTURE_HEAP_SIZE;
#endif
#endif

#if BCHP_CHIP==7550
#if NEXUS_BOARD_7530_CRB
    pSettings->videoDecoderModuleSettings.supportedCodecs[NEXUS_VideoCodec_eH264] = NEXUS_VideoCodec_eNone;
    pSettings->videoDecoderModuleSettings.supportedCodecs[NEXUS_VideoCodec_eVc1] = NEXUS_VideoCodec_eNone;
    pSettings->videoDecoderModuleSettings.supportedCodecs[NEXUS_VideoCodec_eAvs] = NEXUS_VideoCodec_eNone;
#endif
#endif

#if NEXUS_HAS_AUDIO
    NEXUS_AudioModule_GetDefaultSettings(&pSettings->audioModuleSettings);
    #if NEXUS_NUM_AUDIO_DSP
    if ( NEXUS_GetEnv("audio_logs_enabled") )
    {
        pSettings->audioModuleSettings.dspDebugSettings.typeSettings[NEXUS_AudioDspDebugType_eUartMessage].enabled = true;
        pSettings->audioModuleSettings.dspDebugSettings.typeSettings[NEXUS_AudioDspDebugType_eDramMessage].enabled = true;
        pSettings->audioModuleSettings.dspDebugSettings.typeSettings[NEXUS_AudioDspDebugType_eCoreDump].enabled = true;
    }
    else
    {
        if ( NEXUS_GetEnv("audio_uart_file") )
        {
            pSettings->audioModuleSettings.dspDebugSettings.typeSettings[NEXUS_AudioDspDebugType_eUartMessage].enabled = true;
        }
        if ( NEXUS_GetEnv("audio_debug_file") )
        {
            pSettings->audioModuleSettings.dspDebugSettings.typeSettings[NEXUS_AudioDspDebugType_eDramMessage].enabled = true;
        }
        if ( NEXUS_GetEnv("audio_core_file") )
        {
            pSettings->audioModuleSettings.dspDebugSettings.typeSettings[NEXUS_AudioDspDebugType_eCoreDump].enabled = true;
        }
    }
    #endif
#endif

#if NEXUS_HAS_SURFACE
    NEXUS_SurfaceModule_GetDefaultSettings(&pSettings->surfacePlatformSettings);
#endif

#if NEXUS_HAS_SMARTCARD
    NEXUS_SmartcardModule_GetDefaultSettings(&pSettings->smartCardSettings);
#endif

#if NEXUS_HAS_VIDEO_ENCODER
    NEXUS_VideoEncoderModule_GetDefaultSettings(&pSettings->videoEncoderSettings);
#endif

#if NEXUS_HAS_SECURITY
    NEXUS_SecurityModule_GetDefaultSettings(&pSettings->securitySettings);
#endif

#if NEXUS_HAS_GRAPHICS2D
    NEXUS_Graphics2DModule_GetDefaultSettings(&pSettings->graphics2DModuleSettings);
#endif

#if NEXUS_HAS_FILE
    {
        NEXUS_FileModuleSettings fileModuleSettings;
        NEXUS_FileModule_GetDefaultSettings(&fileModuleSettings);
        pSettings->fileModuleSettings.workerThreads = fileModuleSettings.workerThreads;
        BDBG_CASSERT(sizeof(pSettings->fileModuleSettings.schedulerSettings) == sizeof(fileModuleSettings.schedulerSettings));
        BKNI_Memcpy(pSettings->fileModuleSettings.schedulerSettings, fileModuleSettings.schedulerSettings, sizeof(pSettings->fileModuleSettings.schedulerSettings));
    }
#endif

#if NEXUS_POWER_MANAGEMENT
    NEXUS_Platform_GetStandbySettings(&pSettings->standbySettings);
#endif

    for (i=0;i<NEXUS_MAX_HEAPS;i++) {
        pSettings->heap[i].memoryType = NEXUS_MemoryType_eFull;
    }

    /* learn the MEMC configuration on this board and store it to g_platformMemory.
    this requires temporary bringup of OS mem and register mapping.
    must unmap. it's possible that an app calls NEXUS_Platform_GetDefaultSettings and then exits. can't leak an mmap.
    */
    BKNI_Memset(pMemory, 0, sizeof(*pMemory));
    errCode = NEXUS_Platform_P_InitOSMem();
    if (errCode) {
        BERR_TRACE(errCode);
    }
    else {
        BREG_Handle reg = NEXUS_Platform_P_MapRegisters();
        if (!reg) {
            BERR_TRACE(NEXUS_UNKNOWN);
        }
        else {
            /* pSettings is not used in newer implementations of NEXUS_Platform_P_ReadMemcConfig, but older code may modify it */
            NEXUS_Platform_P_ReadMemcConfig(reg, pMemory, pSettings);
            NEXUS_Platform_P_UnmapRegisters(reg);
        }
        errCode = NEXUS_Platform_P_CalcSubMemc(pMemory);
        if (errCode) {
            errCode = BERR_TRACE(errCode);
            /* fall through */
        }
        errCode = NEXUS_Platform_P_GetHostMemory(pMemory);
        if (errCode) {
            errCode = BERR_TRACE(errCode);
            /* fall through */
        }
        NEXUS_Platform_P_LimitMemcRegions(pMemory);
        NEXUS_Platform_P_UninitOSMem();
    }

    /* get platform-specific settings. this can use g_platformMemory to customize settings based on memory layout and kernel usage. */
    NEXUS_Platform_P_GetDefaultSettings(pSettings);

    defaultSettings = *pSettings;
    set = true;

    return;
}

void NEXUS_Platform_GetSettings( NEXUS_PlatformSettings *pSettings )
{
    BDBG_ASSERT(NULL != pSettings);
    *pSettings = g_NEXUS_platformSettings;
}

#ifdef BDBG_DEBUG_BUILD /* used in BDBG_ERR and BDBG_MSG */
static const char *b_memory_type_str(unsigned memoryType)
{
    if (memoryType == NEXUS_MemoryType_eFull) {
        return "eFull";
    }
    else if (memoryType == NEXUS_MemoryType_eDriver) {
        return "eDriver";
    }
    else if (memoryType == NEXUS_MemoryType_eApplication) {
        return "eApplication";
    }
    else if (memoryType == NEXUS_MemoryType_eDeviceOnly) {
        return "eDeviceOnly";
    }
    else {
        static char buf[16]; /* assumed that result is only used immediately */
        BKNI_Snprintf(buf, 16, "%#x", memoryType);
        return buf;
    }
}
#endif

/* function name is misleading.
this function adjust bmem regions, not memc regions.
it dynamically creates regions the OS leaves out.
it assigns bmem regions to MEMC regions. if not assignment is possible, the bmem region is zeroed out. */
static void NEXUS_Platform_P_LimitMemcRegions(NEXUS_PlatformMemory *pMemory)
{
    unsigned i, j, k;

    /* add bmem regions that OS has left out.
    in linux 2.6.37 and beyond, linux should report bmem regions for all memory and this code is not needed. */
#if BMIPS3300
    if (pMemory->memc[0].length > 0x20000000) {
        unsigned availableBmem = NEXUS_MAX_HEAPS;
        for (i=0;i<NEXUS_MAX_HEAPS;i++) {
            if (pMemory->osRegion[i].length &&
                pMemory->osRegion[i].base >= 0x20000000 &&
                pMemory->osRegion[i].base < 0x40000000) break;
            if (!pMemory->osRegion[i].length && availableBmem == NEXUS_MAX_HEAPS) {
                availableBmem = i;
            }
        }
        if (i == NEXUS_MAX_HEAPS && availableBmem != NEXUS_MAX_HEAPS) {
            BDBG_MSG(("dynamically creating bmem.%d because OS is not reporting it", availableBmem));
            pMemory->osRegion[availableBmem].base = 0x20000000;
            pMemory->osRegion[availableBmem].length = pMemory->memc[0].length - 0x10000000;
        }
    }
#elif BCHP_CHIP == 7405 || BCHP_CHIP == 7420 || BCHP_CHIP == 7400 || BCHP_CHIP==7335 || BCHP_CHIP==7325
    if (pMemory->memc[1].length) {
        unsigned availableBmem = NEXUS_MAX_HEAPS;
        for (i=0;i<NEXUS_MAX_HEAPS;i++) {
            if (pMemory->osRegion[i].length &&
                pMemory->osRegion[i].base >= 0x60000000) break;
            if (!pMemory->osRegion[i].length && availableBmem == NEXUS_MAX_HEAPS) {
                availableBmem = i;
            }
        }
        if (i == NEXUS_MAX_HEAPS && availableBmem != NEXUS_MAX_HEAPS) {
            BDBG_MSG(("dynamically creating bmem.%d because OS is not reporting it", availableBmem));
            pMemory->osRegion[availableBmem].base = 0x60000000;
            pMemory->osRegion[availableBmem].length = pMemory->memc[1].length;
        }
    }
#endif
#if BCHP_CHIP == 7400
    if (pMemory->memc[2].length) {
        unsigned availableBmem = NEXUS_MAX_HEAPS;
        for (i=0;i<NEXUS_MAX_HEAPS;i++) {
            if (pMemory->osRegion[i].length &&
                pMemory->osRegion[i].base >= 0x70000000) break;
            if (!pMemory->osRegion[i].length && availableBmem == NEXUS_MAX_HEAPS) {
                availableBmem = i;
            }
        }
        if (i == NEXUS_MAX_HEAPS && availableBmem != NEXUS_MAX_HEAPS) {
            BDBG_MSG(("dynamically creating bmem.%d because OS is not reporting it", availableBmem));
            pMemory->osRegion[availableBmem].base = 0x70000000;
            pMemory->osRegion[availableBmem].length = pMemory->memc[2].length;
        }
    }
#endif
    
    /* dump all OS regions and all nexus regions for debug */
    for (i=0;i<NEXUS_MAX_HEAPS;i++) {
        if (pMemory->osRegion[i].length) {
            BDBG_MSG(("os bmem.%d offset %#x, size %u", i, pMemory->osRegion[i].base, pMemory->osRegion[i].length));
        }
    }

    /* determine MEMC index/subIndex for each bmem region */
    for (i=0;i<NEXUS_MAX_HEAPS;i++) {
        bool done = false;
        for (j=0;j<NEXUS_MAX_MEMC && !done;j++) {
            for (k=0;k<NEXUS_NUM_MEMC_REGIONS;k++) {
                if (pMemory->osRegion[i].base >= pMemory->memc[j].region[k].base &&
                    pMemory->osRegion[i].base < pMemory->memc[j].region[k].base + pMemory->memc[j].region[k].length)
                {
                    pMemory->osRegion[i].memcIndex = j;
                    pMemory->osRegion[i].subIndex = k;
                    done = true; break; /* double break */
                }    
            }
            if (done) break;
        }
        if (j == NEXUS_MAX_MEMC) {
            BDBG_ERR(("unable to assign bmem.%d offset %#x to MEMC region", i, pMemory->osRegion[i].base));
            pMemory->osRegion[i].length = 0;
        }
    }
}

/**
create heaps based on NEXUS_PlatformSettings.heap[] requests (which are based on MEMC index/subindex) and allowed bmem regions (stored in NEXUS_PlatformMemory.osRegion[]
**/
NEXUS_Error NEXUS_Platform_P_SetCoreModuleSettings(const NEXUS_PlatformSettings *pSettings, const NEXUS_PlatformMemory *pMemory, NEXUS_Core_Settings *pCoreSettings)
{
    NEXUS_Error rc = 0;
    unsigned i, j;
    unsigned totalUsed[NEXUS_MAX_HEAPS];
    unsigned heapOffset[NEXUS_MAX_HEAPS];
    int bmemIndex[NEXUS_MAX_HEAPS]; /* bmem index for each NEXUS_PlatformSettings.heap[] */

    for (i=0;i<NEXUS_MAX_HEAPS;i++) {
        totalUsed[i] = heapOffset[i] = 0;
        bmemIndex[i] = -1;
    }

    /* step 1: assign bmemIndex[] and calculate the remainder per bmem region */
    for (i=0;i<NEXUS_MAX_HEAPS;i++) {
        int size = pSettings->heap[i].size;

        if (!size) {
            continue;
        }

        BDBG_MSG(("request heap[%d]: MEMC%d/%d, size %d, %s", i, pSettings->heap[i].memcIndex, pSettings->heap[i].subIndex,
            pSettings->heap[i].size, b_memory_type_str(pSettings->heap[i].memoryType)));
            
        if (size == -1) {
            continue;
        }
        
        /* find the first bmem in this MEMC region that is not already used up.
        there is a dynamic mapping from NEXUS_PlatformSettings.heap[] to bmem region based on their order */
        for (j=0;j<NEXUS_MAX_HEAPS;j++) {
            BDBG_ASSERT(totalUsed[j] <= pMemory->osRegion[j].length);
            if (totalUsed[j] == pMemory->osRegion[j].length) {
                continue;
            }
            if (pSettings->heap[i].memcIndex == pMemory->osRegion[j].memcIndex &&
                pSettings->heap[i].subIndex == pMemory->osRegion[j].subIndex)
            {
                bmemIndex[i] = j;
                break;
            }
        }
        if (j == NEXUS_MAX_HEAPS) {
            if (pSettings->heap[i].optional) {
                /* can't create this heap */
                continue;
            }
            else {
                BDBG_ERR(("No bmem found for heap[%d]: MEMC%d/%d, size %d, %s", i, pSettings->heap[i].memcIndex, pSettings->heap[i].subIndex,
                    pSettings->heap[i].size, b_memory_type_str(pSettings->heap[i].memoryType)));
                return BERR_TRACE(NEXUS_INVALID_PARAMETER);
            }
        }
        
        if (totalUsed[bmemIndex[i]] + size > pMemory->osRegion[bmemIndex[i]].length) {
            rc = BERR_TRACE(NEXUS_INVALID_PARAMETER);    
            goto cannot_create_heap;    
        }
        totalUsed[bmemIndex[i]] += size;
    }

    /* step 2: allocate the heaps, including determining the size=-1 heaps */
    for (i=0;i<NEXUS_MAX_HEAPS;i++) {
        int size;

        size = pSettings->heap[i].size;
        if (!size) {
            continue;
        }

        if (size == -1) {
            /* find bmem for the remainder heap */
            for (j=0;j<NEXUS_MAX_HEAPS;j++) {
                BDBG_ASSERT(totalUsed[j] <= pMemory->osRegion[j].length);
                if (totalUsed[j] == pMemory->osRegion[j].length) {
                    continue;
                }
                if (pSettings->heap[i].memcIndex == pMemory->osRegion[j].memcIndex &&
                    pSettings->heap[i].subIndex == pMemory->osRegion[j].subIndex)
                {
                    bmemIndex[i] = j;
                    break;
                }
            }
            if (j == NEXUS_MAX_HEAPS) {
                /* we cannot create a heap for this region */
                if (pSettings->heap[i].optional)
                    continue;
                rc = BERR_TRACE(NEXUS_INVALID_PARAMETER);    
                goto cannot_create_heap;    
            }
            size = pMemory->osRegion[j].length - totalUsed[bmemIndex[i]];
            totalUsed[bmemIndex[i]] += size;
        }
        else {
            if (bmemIndex[i] == -1) {
                BDBG_ASSERT(pSettings->heap[i].optional);
                /* fixed size optional heap that couldn't be created */
                continue;
            }
        }
        BDBG_ASSERT(bmemIndex[i] >= 0 && bmemIndex[i] < NEXUS_MAX_HEAPS);
        BDBG_ASSERT(size > 0);

        NEXUS_Heap_GetDefaultMemcSettings(pSettings->heap[i].memcIndex, &pCoreSettings->heapRegion[i]);
        /* if OS reports greater cache line size than BMEM's default, then increase */
        if (pMemory->max_dcache_line_size > pCoreSettings->heapRegion[i].alignment) {
            pCoreSettings->heapRegion[i].alignment = pMemory->max_dcache_line_size;
        }
        /* make sure user setting doesn't give access that GetDefault doesn't allow
        TODO: this could be a bitwise operation, but it isn't needed now. */
        if (pCoreSettings->heapRegion[i].memoryType) {
            pCoreSettings->heapRegion[i].memoryType = pSettings->heap[i].memoryType;
        }

        /* make sure user setting isn't less than minimum requirement based on cache line size. */
        if (pSettings->heap[i].alignment > pCoreSettings->heapRegion[i].alignment) {
            pCoreSettings->heapRegion[i].alignment = pSettings->heap[i].alignment;
        }
        pCoreSettings->heapRegion[i].length = size;
        pCoreSettings->heapRegion[i].offset = pMemory->osRegion[bmemIndex[i]].base + heapOffset[bmemIndex[i]];

        heapOffset[bmemIndex[i]] += size;
    }

#if BDBG_DEBUG_BUILD
    for (i=0;i<NEXUS_MAX_HEAPS;i++) {
        if (!pCoreSettings->heapRegion[i].length) continue;
        BDBG_MSG(("creating heap[%d]: MEMC%d, offset %#x, size %u, %s",
            i,
            pCoreSettings->heapRegion[i].memcIndex,
            pCoreSettings->heapRegion[i].offset,
            pCoreSettings->heapRegion[i].length,
            b_memory_type_str(pCoreSettings->heapRegion[i].memoryType)));
    }
#endif

    return rc;
    
cannot_create_heap:
    /* assumes i was left at heap of interest */
    BDBG_ERR(("cannot create heap[%d]: MEMC%d/%d, size %d, %s", i, pSettings->heap[i].memcIndex, pSettings->heap[i].subIndex,
        pSettings->heap[i].size, b_memory_type_str(pSettings->heap[i].memoryType)));
    BDBG_ERR(("run with msg_modules=nexus_platform_settings for more config info"));
    return BERR_TRACE(rc);    
}

/*
 * Since nexus platform settings file is common for multiple platforms and
 * each of these platforms have individual nexus_platform_$(NEXUS_PLATFORM) files, this API would call into
 * a private API in the nexus_platform core
 */
NEXUS_HeapHandle NEXUS_Platform_GetFramebufferHeap(unsigned displayIndex)
{
  return NEXUS_Platform_P_GetFramebufferHeap(displayIndex);
}
