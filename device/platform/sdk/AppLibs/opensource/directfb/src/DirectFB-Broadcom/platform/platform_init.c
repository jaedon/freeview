/******************************************************************************
 *    (c)2008-2012 Broadcom Corporation
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
 * $brcm_Workfile: platform_init.c $
 * $brcm_Revision: DirectFB_1_4_15_Port/20 $
 * $brcm_Date: 3/7/12 5:26p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /AppLibs/opensource/directfb/src/broadcom_files/private/DirectFB/1.4.15/platform/platform_init.c $
 *
 * DirectFB_1_4_15_Port/20   3/7/12 5:26p christ
 * SW7231-699: DirectFB-1.4.15: Clean up Broadcom code tree
 *
 * DirectFB_1_4_15_Port/19   3/1/12 7:53p christ
 * SW7420-2273: DirectFB-1.4.15: Use correct Nexus heap for secondary
 *  display frame buffers
 *
 * DirectFB_1_4_15_Port/18   2/29/12 12:35p christ
 * SW7420-2270: DirectFB-1.4.15: Cleanup code prior to release
 *
 * DirectFB_1_4_15_Port/17   2/24/12 10:54p robertwm
 * SW7420-2260: DirectFB: Only close Nexus handles that have been opened
 *  in the DFB platform layer.
 * SW7420-2261: DirectFB-1.4.15: Platform code should not open up the
 *  display, windows or outputs in a slave app.
 * SW7420-2262: DirectFB-1.4.15: DFB_Platform_GetStatus() does not return
 *  correct "master/slave" status.
 *
 * DirectFB_1_4_15_Port/16   2/9/12 2:34p christ
 * SW7425-1550: DirectFB: Require integration of Nexus Surface Compositor
 *  into DFB
 *
 * DirectFB_1_4_15_Port/15   1/16/12 1:05p robertwm
 * SW7425-2185: DirectFB: Cannot run unprotected client applications that
 *  access video memory.
 *
 * DirectFB_1_4_15_Port/DirectFB-XS_1_4_15_Port/1   12/8/11 3:01p robertwm
 * SW7425-1550: DirectFB: Require integration of Nexus Surface Compositor
 *  into DFB.
 *
 * DirectFB_1_4_15_Port/14   11/25/11 2:44p robertwm
 * SW7425-1742: DirectFB: Need to add locking to platform initialisation
 *  data.
 *
 * DirectFB_1_4_15_Port/13   11/21/11 5:07p robertwm
 * SW7425-1815: DirectFB-1.4.15: Cannot run DFB in multi-app mode with
 *  Phase 3.5 reference software.
 *
 * DirectFB_1_4_15_Port/12   11/15/11 1:51a christ
 * SW7346-556: DirectFB: Display fails to open on 7346 B1 using Phase 4.5
 *  refsw
 *
 * DirectFB_1_4_15_Port/11   11/11/11 7:42p robertwm
 * SW7425-1742: DirectFB: Need to add locking to platform initialisation
 *  data.
 *
 * DirectFB_1_4_15_Port/10   10/20/11 5:55p robertwm
 * SW7231-398: DirectFB-1.4.15: Locking a pre-allocated surface does not
 *  return same virtual address.
 *
 * DirectFB_1_4_15_Port/9   10/17/11 5:30p robertwm
 * SW7231-398: DirectFB-1.4.15: Locking a pre-allocated surface does not
 *  return same virtual address.
 *
 * DirectFB_1_4_15_Port/8   10/6/11 4:39p christ
 * SW7425-1420: DirectFB: Access master surface pool handle from slave
 *  application
 *
 * DirectFB_1_4_15_Port/7   10/6/11 4:10p christ
 * SW7425-1420: DirectFB: Access master surface pool handle from slave
 *  application
 *
 * DirectFB_1_4_15_Port/6   10/5/11 12:12p robertwm
 * SW7420-2058: DirectFB: Change DFB Platform code to not rely on
 *  directfb.h header file.
 * SW7420-2065: DirectFB: Need DFB Platform Version Macros for external
 *  applications to query.
 *
 * DirectFB_1_4_14_Port/5   9/22/11 4:43p christ
 * SW7425-1277: DirectFB: Support untrusted client applications in 1.4.14
 *
 * DirectFB_1_4_14_Port/4   9/13/11 4:46p christ
 * SW7425-1277: DirectFB: Support untrusted client applications in 1.4.14
 *
 * DirectFB_1_4_14_Port/3   9/7/11 6:04p robertwm
 * SW7420-2048: DirectFB: Don't display warning messages about resources
 *  already having been opened in DFB platform.
 *
 * DirectFB_1_4_14_Port/2   8/19/11 5:04p christ
 * SW7425-946: DirectFB: Create AppLibs DirectFB 1.4.14 release.
 *
 * DirectFB_1_4_5_Port/7   9/13/11 4:03p robertwm
 * SW7420-2058: DirectFB: Change DFB Platform code to not rely on
 *  directfb.h header file.
 *
 * DirectFB_1_4_5_Port/6   9/6/11 5:35p robertwm
 * SW7420-2048: DirectFB: Don't display warning messages about resources
 *  already having been opened in DFB platform.
 *
 * DirectFB_1_4_5_Port/5   8/26/11 2:59p jackf
 * Added alignment buffering for the structures packed into the shared
 *  memory block.
 * This prevents a structure alignment issue we were seeing.
 *
 * DirectFB_1_4_5_Port/4   8/15/11 12:48p robertwm
 * SW7420-2033: DirectFB: need to expose Nexus VideoWindow handle in dfb
 *  platform code.
 *
 * DirectFB_1_4_14_Port/1   7/28/11 11:21a christ
 * SW7425-946: DirectFB: Create AppLibs DirectFB 1.4.14 release.
 *
 * DirectFB_1_4_5_Port/3   7/8/11 5:00p robertwm
 * SW7630-105: DirectFB-1.4.5: dfb_platform.h has incorrect init settings
 *  for BCC.
 *
 * DirectFB_1_4_5_Port/2   7/7/11 6:44p tross
 * SWBLURAY-26478: Merge in Bluray libinit support.
 *
 * DirectFB_1_4_5_Port/1   6/14/11 5:40p christ
 * SW7425-707: DirectFB: Create DirectFB-1.4.5 Phase 2.0 Release.
 *
 * DirectFB_1_4_Port/53   7/1/11 11:55a tross
 * SWBLURAY-26422: Merge in Bluray libinit support.
 *
 * DirectFB_1_4_Port/53   7/1/11 11:55a tross
 * SWBLURAY-26422: Merge in Bluray libinit support.
 *
 * DirectFB_1_4_Port/52   6/2/11 4:31p robertwm
 * SW7405-5326: DirectFB: Re-factor platform code for new API.
 *
 * DirectFB_1_4_Port/51   5/23/11 1:57p robertwm
 * SW7425-623: DirectFB: Platform code fails to initialise on chips that
 *  have SID and sw_picture_decode is defined.
 *
 * DirectFB_1_4_Port/50   5/17/11 6:49p robertwm
 * SW7405-5326: DirectFB: Re-factor platform code for new API.
 *
 * DirectFB_1_4_Port/49   5/13/11 10:57p robertwm
 * SW7405-5326: DirectFB: Re-factor platform code for new API.
 *
 * DirectFB_1_4_Port/48   3/11/11 5:05p christ
 * SW7468-370: DirectFB: 1.4.1 release fails to compile for platforms
 *  without component output.
 *
 * DirectFB_1_4_Port/47   2/25/11 6:23p apandav
 * SW3520-44: BCM935230: Error while compiling DirectFB_1_4_Port
 *
 * DirectFB_1_4_Port/46   2/22/11 12:00p christ
 * SW7420-1527: DirectFB: Update platform code to get offscreen heap from
 *  Nexus when available
 *
 * DirectFB_1_4_Port/45   2/20/11 7:27p robertwm
 * SW7422-79: Add support for mapping video decoders to andy video source.
 * SW7420-1394: video layer support in DFB.
 * SW7422-275: DirectFB: Enabling video layer causes Segmentation fault.
 *
 * DirectFB_1_4_Port/44   2/18/11 2:38p robertwm
 * SW7468-366: DirectFB: Platforms without component output should not
 *  call component functions.
 *
 * DirectFB_1_4_Port/43   2/4/11 8:39p robertwm
 * SW7400-3000: DirectFB: Support non-DFB master application in
 *  platform_init code.
 * SW7400-3002: DirectFB: Support non-DFB master application in
 *  platform_init code.
 *
 * DirectFB_1_4_Port/42   2/4/11 4:19p robertwm
 * SW35230-2943: DirectFB: Cannot build platform_init code for 35230
 *  platform.
 *
 * DirectFB_1_4_Port/41   2/4/11 3:22p christ
 * SW7420-1458: DirectFB: Rework internal handling of nexus heaps. - fix
 *  compile issue with old nexus.
 *
 * DirectFB_1_4_Port/40   2/3/11 6:10p christ
 * SW7420-1458: DirectFB: Rework internal handling of nexus heaps.
 *
 * DirectFB_1_4_Port/39   2/3/11 6:01p christ
 * SW7420-1458: DirectFB: Rework internal handling of nexus heaps.
 *
 * DirectFB_1_4_Port/38   1/31/11 7:30p robertwm
 * SW7420-1451: DirectFB: platform_init code should get called
 *  automatically by slave applications.
 *
 * DirectFB_1_4_Port/37   1/25/11 8:06p robertwm
 * SW7420-1424: DirectFB: Refactor platform_init code to separate DTV and
 *  STB parts and to support NEXUS heap m'ment.
 *
 * DirectFB_1_4_Port/DirectFB_STEREO_3D_JIRA_SW7422_138/8   1/25/11 7:11p robertwm
 * SW7420-1422: DirectFB: Add support for NEXUS multi-app platform server
 *  in platform init code.
 *
 * DirectFB_1_4_Port/DirectFB_STEREO_3D_JIRA_SW7422_138/7   1/18/11 1:27p christ
 * Fix DTV compile problems.
 *
 * DirectFB_1_4_Port/DirectFB_STEREO_3D_JIRA_SW7422_138/5   1/17/11 5:19p christ
 * Better fix for compile issue.
 *
 * DirectFB_1_4_Port/DirectFB_STEREO_3D_JIRA_SW7422_138/4   1/17/11 4:51p christ
 * Fix build issue with libinit code.
 *
 * DirectFB_1_4_Port/DirectFB_STEREO_3D_JIRA_SW7422_138/3   1/17/11 12:37p christ
 * Reworked platform init code.
 *
 * DirectFB_1_4_Port/DirectFB_STEREO_3D_JIRA_SW7422_138/1   12/13/10 5:23p christ
 * Initial stereo 3d commit
 *
 * DirectFB_1_4_Port/35   12/3/10 8:42p raywu
 * SW7550-629: DirectFB: Integrated ZSP code to DFB packet buffer branch
 *
 * DirectFB_1_4_Port/34   12/2/10 2:43p christ
 * SW7420-1283: DirectFB: Add support for extra Nexus heap on dual MEMC
 *  parts, slight rework to avoid issues for SOCs with only 1 memory
 *  controller (eg 7550).
 *
 * DirectFB_1_4_Port/33   12/1/10 3:47p christ
 * SW7420-1283: DirectFB: Add support for extra Nexus heap on dual MEMC
 *  parts
 *
 * DirectFB_1_4_Port/32   10/20/10 5:11p robertwm
 * SW7420-1195: DirectFB: Move the platform_init shared handles definition
 *  to a header file for portability.
 *
 * DirectFB_1_4_Port/31   10/20/10 4:43p robertwm
 * SW7420-1194: DirectFB: Cannot run application more than once if it was
 *  exited abnormally.
 *
 * DirectFB_1_4_Port/30   10/19/10 1:38p robertwm
 * SW7420-1188: DirectFB: platform_init fails to initialise after
 *  launching an app more than once.
 *
 * DirectFB_1_4_Port/29   10/12/10 4:57p ismailk
 * SW7405-4884: Don't check for "dfb_no_platform_init" env variable in
 *  do_platform_init()
 *
 * DirectFB_1_4_Port/28   9/20/10 9:44p robertwm
 * SW7405-4884: DirectFB: Add shared memory handles to platform_init
 *
 * DirectFB_1_4_Port/27   9/13/10 6:23p robertwm
 * SW7405-4870: DirectFB: Add support for not running the platform_init()
 *  code.
 *
 * DirectFB_1_4_Port/26   8/20/10 3:36p robertwm
 * SW7405-4791: DirectFB: DirectFB-1.4.1 Phase 2.5 fails to build with
 *  Nexus Phase 5.0 reference software.
 *
 * DirectFB_1_4_Port/25   8/11/10 8:09p robertwm
 * SW3556-1178: DirectFB: Seg fault when trying to shut down Penguins on
 *  3556.
 *
 * DirectFB_1_4_Port/24   8/11/10 4:57p robertwm
 * SW35230-957: DirectFB: platform_init fails to build on 35230 with
 *  recent 3D changes.
 *
 * DirectFB_1_4_Port/23   8/11/10 1:57a robertwm
 * SW7405-4250: DirectFB: Screen destination rectangle and source
 *  rectangle not saved across contexts.
 * SW7405-4743: DirectFB: Multiple instances of df_andi.c do not get
 *  displayed.
 *
 * DirectFB_1_4_Port/22   8/6/10 8:04p robertwm
 * SW7405-4724: DirectFB: Support OpenGL ES 1.0 applications.
 *
 * DirectFB_1_4_Port/21   7/20/10 11:29a robertwm
 * SW7420-900: NEXUS_CORE_HEAP_ALIGNMENT has been changed to
 *  NEXUS_LARGEST_CPU_CACHE_LINE.
 *
 * DirectFB_1_4_Port/19   5/5/10 12:49p robertwm
 * SW7405-4295: DirectFB: Reference to NEXUS_NUM_HDMI_OUTPUTS should check
 *  if > 0.
 *
 * DirectFB_1_4_Port/18   4/19/10 7:29p robertwm
 * SW7420-708: DirectFB: Support DirectFB run as a slave.
 *
 * DirectFB_1_4_Port/17   4/9/10 4:54p robertwm
 * SW7405-4183: DirectFB: Add support for Texture Triangles in the
 *  graphics driver.
 *
 * DirectFB_1_4_Port/16   3/4/10 7:07p robertwm
 * SW7468-137: DirectFB: Require CVBS/RFM to be connected to display1
 *  path.
 *
 * DirectFB_1_4_Port/15   2/22/10 11:53a robertwm
 * SW7420-549: Separate heap for graphics doesn't work with 7420.
 *
 * DirectFB_1_4_Port/14   1/5/10 5:42p robertwm
 * SW7405-3711: DirectFB: Require graphics on composite and RFM outputs.
 *
 * DirectFB_1_4_Port/13   1/5/10 11:39a robertwm
 * SW7405-3711: DirectFB: Require graphics on composite and RFM outputs.
 *
 * DirectFB_1_4_Port/12   12/23/09 2:18p robertwm
 * SW7550-51: Support DirectFB on 7550.
 *
 * DirectFB_1_4_Port/SW7550-51/1   12/22/09 3:22p mphillip
 * SW7550-51: Add 7550 build support (on branch)
 *
 * DirectFB_1_4_Port/11   12/17/09 2:31p robertwm
 * SW7335-641: DFB is not working with popen.
 *
 * DirectFB_1_4_Port/10   12/3/09 1:27p robertwm
 * SW7405-3528: BERR_OUT_OF_SYSTEM_MEMORY(0x3) at bgrc_private.c:957.
 *
 * DirectFB_1_4_Port/9   11/27/09 2:05p robertwm
 * SW7405-3502: Poor graphics performance of 1.3.x and 1.4.x compared with
 *  1.0.0
 *
 * DirectFB_1_4_Port/8   10/27/09 6:12p robertwm
 * SW7405-3124: DirectFB: Allow memory architecture (UMA/non-UMA) to be
 *  specified as envar to speed up DFB init.
 * (now allows mem_non_uma=0 to be specified to indicate UMA memory
 *  architecture).
 *
 * DirectFB_1_4_Port/7   10/2/09 12:15p robertwm
 * SW7405-3124: DirectFB: Allow memory architecture (UMA/non-UMA) to be
 *  specified as envar to speed up DFB init.
 *
 * DirectFB_1_4_Port/6   8/31/09 12:41p ismailk
 * SW7405-2954: Uninitialize BDBG() module before closing the module.
 *  Also, don't NEXUS_Platform_Uninit() as the DLNA mediaplayer code lets
 *  nexus connections and handles remain open.
 *
 * DirectFB_1_4_Port/5   8/27/09 4:34p robertwm
 * SW3548-2427: DirectFB: Support 3549 panel types and fix compilation
 *  bugs.
 *
 * DirectFB_1_4_Port/4   8/20/09 12:16p robertwm
 * PR57884: DirectFB: Support 7420 B0 Platform.
 *
 * DirectFB_1_4_Port/3   8/7/09 3:41p robertwm
 * PR56902: Require single graphics memory heap to avoid fragmentation.
 * PR56721: Add HDMI output support.
 *
 * DirectFB_1_4_Port/2   7/10/09 4:22p robertwm
 * PR56721: Add HDMI output to DirectFB.
 *
 * DirectFB_1_4_Port/1   7/8/09 3:19p mphillip
 * PR56658: Add DirectFB 1.4.1 broadcom_files
 *
 * 11   2/25/09 2:33p gmohile
 * PR 25109 : Enable only panel output for DTV platforms
 *
 * 10   11/21/08 8:06p vsilyaev
 * PR 49524: Added graphics2d_sync routine that could be used by different
 *  users of Nexus_Graphics2D
 *
 * 9   11/6/08 9:59a gmohile
 * PR 45243 : Turn off pre-alloc memory for graphics
 *
 * 8   11/5/08 10:26a gmohile
 * PR 45243 : disable destructor
 *
 * 7   11/3/08 1:57p gmohile
 * PR 45243 : Default all DTV to panel output
 *
 * 6   10/24/08 12:42p gmohile
 * PR 46490 : Add hw picture decode support
 *
 * 5   10/14/08 3:53p gmohile
 * PR 45243 : Fix nexus handles
 *
 * 4   9/22/08 11:55a gmohile
 * PR 45243 : Fix 3548 display open
 *
 * 3   9/15/08 11:49a gmohile
 * PR 46734 : Add dual output support
 *
 * 2   9/9/08 1:47p gmohile
 * PR 45243 : Do not add outputs in platform init
 *
 * 1   8/29/08 2:35p gmohile
 * PR 45243 : Do nexus platform init and display open outside directfb
 *
 *****************************************************************************/
#include "bstd.h"
#include "platform_init.h"
#include "nexus_platform.h"
#include "lib/fusion/build.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>

#include "bkni.h"
#include "bkni_multi.h"
#if FUSION_BUILD_MULTI
#include "platform_ipc_session_simple_client.h"
#endif

BDBG_MODULE(platform_init);

/* Used to indicate whether DFB_Platform_Init() was called successfully or not.
   Each process should call DFB_Platform_Init() to ensure coherency. */
static bool dfb_platform_initialised = false;

/* Indicates whether this is the first ever application/process to
   initialise the dfb platform code. */
static bool dfb_is_master = false;

/* Used to access the shared public handles (this gets set per process) */
static DFB_PlatformSettings  *dfb_platform_shared = NULL;

/* Used to access the shared platform status (this gets set per process) */
static DFB_PlatformStatus    *dfb_platform_status_shared = NULL;

/* Used to access the shared private handles (this gets set per process) */
static DFB_P_DisplaySettings *dfb_p_display_shared = NULL;

/* Used to access the shared private handles (this gets set per process) */
static DFB_P_PlatformSharedData *dfb_p_platform_shared_data = NULL;

/* Not shared across processes, but indicates whether a device was initialised
   in the current process or not. */
static DFB_P_DeviceStatus     dfb_device_status;

#ifdef BCMNEXUS_NSC_SUPPORT
static const bool isXS = true;
#else
static const bool isXS = false;
#endif

#ifdef BCMNEXUS_NIR_SUPPORT
static const bool isNIR = true;
#else
static const bool isNIR = false;
#endif

/********* Private Functions *********/

/* returns false if process is in slave mode
 *         true if process is in master mode */
static bool
DFB_Platform_P_InitSharedMemory( size_t shMemSz )
{
#if FUSION_BUILD_MULTI
   char * client_name = "dfb";

    BSTD_UNUSED(shMemSz);
    int rc = simple_client_init(client_name);
    if(rc < 0) {
        BDBG_MSG_TRACE(("Could not connect to IPC server. DFB master can ignore this message."));
        return true;
    }
    else {
        return false;
    }
#else
    (void)shMemSz;
#endif
    return true;
}

/*
* This function will create the shared memory and give you the
* void pointer to that memory
*/
static void *
DFB_Platform_P_OpenSharedMemory( size_t shMemSz,
                                 bool bCreate )
{
    char    *shm;
#if FUSION_BUILD_MULTI
    shm = calloc(1, shMemSz);
    if (bCreate) {
        DFB_PlatformResult rc = DFB_PLATFORM_OK ;
        memset(shm, 0, shMemSz);
        rc = DFB_Platform_P_StartIpcServer();

        if(rc) {
            BDBG_ERR(("Failed to start platform ipc server"));
            return NULL;
        }
    }
    else {
        DFB_PlatformResult res;
        DFB_PlatformSettings *pSettings = (DFB_PlatformSettings  *)   ((uint8_t *)shm );
        /*Check received platform settings are valid*/
        while(pSettings->platformSettings.platforSettingsValid != (unsigned int)PLATFORM_SETTINGS_VALID) {
            res = brc_client_get_settings_client(simple_client_state.rca_client,(DFB_PlatformShared *) shm);
            if(res) {
                BDBG_ERR(("Failed to get shared settings from ipc server"));
                free (shm);
                return NULL;
            }
            usleep(100000);
        }
    }
#else
    (void)bCreate;
    shm = (char*)calloc(1,shMemSz);
#endif /* FUSION_BUILD_MULTI */

    BDBG_MSG(("Returning Shared Memory Address:%p\n",shm));

    return shm;
}

static void
DFB_Platform_P_CloseSharedMemory( void * shMem )
{

#if FUSION_BUILD_MULTI
    if(dfb_is_master) {
        DFB_Platform_P_StopIpcServer();
    }
#endif /* FUSION_BUILD_MULTI */
    free(shMem);

}

static DFB_PlatformResult
DFB_Platform_P_SetupSharedMemory( void )
{
    int   shareMemSize = (sizeof(DFB_PlatformSettings)      + 3) +
                         (sizeof(DFB_PlatformStatus)        + 3) +
                         (DFB_Platform_P_GetSharedMemSize() + 3) +
                         (sizeof(DFB_P_PlatformSharedData)  + 3);

    void *pShareMem    = NULL;

    if (dfb_platform_shared == NULL)
    {
        /* If the shared memory has already been initialised, then it means
        we are a slave application. */
        dfb_is_master = DFB_Platform_P_InitSharedMemory(shareMemSize);
        pShareMem = DFB_Platform_P_OpenSharedMemory(shareMemSize, dfb_is_master);

        if (!pShareMem)
        {
            BDBG_ERR(("%s: Setting Up Shared Memory Failed\n", __FUNCTION__));
            return DFB_PLATFORM_NOSHAREDMEMORY;
        }

        dfb_platform_shared         = (DFB_PlatformSettings  *)   ((uint8_t *)pShareMem );
        dfb_platform_status_shared  = (DFB_PlatformStatus    *)   ((uint8_t *)dfb_platform_shared        + ((sizeof(DFB_PlatformSettings)     + 3) & ~3) );
        dfb_p_display_shared        = (DFB_P_DisplaySettings *)   ((uint8_t *)dfb_platform_status_shared + ((sizeof(DFB_PlatformStatus  )     + 3) & ~3) );
        dfb_p_platform_shared_data  = (DFB_P_PlatformSharedData *)((uint8_t *)dfb_p_display_shared       + ((sizeof(DFB_P_DisplaySettings)    + 3) & ~3) );
    }
    return DFB_PLATFORM_OK;
}

DFB_PlatformResult
DFB_Platform_P_GetShared(DFB_PlatformShared *shm)
{
    DFB_PlatformResult res = DFB_PLATFORM_OK;

    int   shareMemSize = (sizeof(DFB_PlatformSettings)      + 3) +
                         (sizeof(DFB_PlatformStatus)        + 3) +
                         (DFB_Platform_P_GetSharedMemSize() + 3) +
                         (sizeof(DFB_P_PlatformSharedData)  + 3);

    if(!shm)
    {
        BDBG_ERR(("%s() Invalid parameter!", __FUNCTION__));
        res = DFB_PLATFORM_INIT;
    }
    else
    {
        memcpy(shm, dfb_platform_shared, shareMemSize );
        shm->dfb_platform_status_shared.isMaster = !dfb_is_master;

    }
    return res;
}

DFB_PlatformResult
DFB_Platform_P_GetStatus( DFB_PlatformStatus *pStatus )
{
    DFB_PlatformResult res;

    res = DFB_Platform_P_SetupSharedMemory();

    if (res != DFB_PLATFORM_OK) {
        BDBG_ERR(("%s() failed to setup shared memory!", __FUNCTION__));
    }
    else {
        dfb_platform_status_shared->isXS     = isXS;
        dfb_platform_status_shared->isNIR    = isNIR;
        dfb_platform_status_shared->isMaster = dfb_is_master;
        memcpy(pStatus, dfb_platform_status_shared, sizeof(*pStatus));
    }

    return res;
}

DFB_PlatformResult
DFB_Platform_P_GetSettings( DFB_PlatformSettings *pSettings )
{
    DFB_PlatformResult res;
    if(!dfb_is_master)
    {
        res = brc_client_get_settings_client(simple_client_state.rca_client, (DFB_PlatformShared *)dfb_platform_shared);
        if(res)
        {
            BDBG_ERR(("Failed to get shared settings from ipc server"));
            return DFB_PLATFORM_FAILURE;
        }
    }
    memcpy(pSettings, dfb_platform_shared, sizeof(DFB_PlatformSettings));
    return DFB_PLATFORM_OK;
}

unsigned long
DFB_Platform_P_GetGraphicsHeapSize( void )
{
    unsigned long heapSize = DEFAULT_GFX_HEAP_SIZE;
    char *pValue;
    char invalid = '\0';
    char *pInvalid = &invalid;

    pValue = getenv("gfx_heap_size");
    if (pValue != NULL)
    {
        heapSize = strtoul(pValue, &pInvalid, 0);
        if ((heapSize == 0) || (*pInvalid != '\0'))
        {
            BDBG_WRN(("Invalid \"gfx_heap_size\" value specified - defaulting to %ld bytes.\n", heapSize));
            heapSize = DEFAULT_GFX_HEAP_SIZE;
        }
    }

    if (heapSize % 4096)
    {
        heapSize -= (heapSize % 4096);
        BDBG_WRN(("Invalid \"gfx_heap_size\" value specified - must be 4k aligned rounding down to %ld bytes.\n", heapSize));
    }

    return heapSize;
}

DFB_PlatformNexusHandle
DFB_Platform_P_GetGraphicsHeap( void *pMemory )
{
    bool isMaster = dfb_platform_status_shared->isMaster || dfb_platform_shared->trustUnauthenticatedClients;

    return DFB_Platform_P_GetHeap(pMemory, isMaster);
}

/** Returns -1 if no display found with given handle */
int
DFB_Platform_P_GetDisplayIndex( DFB_PlatformNexusHandle displayHandle )
{
    int displayIndex;

    for (displayIndex = 0; displayIndex < NUM_DISPLAYS; displayIndex ++)
    {
        if (dfb_p_display_shared->display[displayIndex].displayHandle == displayHandle)
            break;
    }

    if (displayIndex >= NUM_DISPLAYS)
        displayIndex = -1;

    return displayIndex;
}

DFB_PlatformNexusHandle
DFB_Platform_P_GetDisplayHandle( int displayIndex)
{
    DFB_PlatformNexusHandle handle = NULL;

    if (displayIndex < NUM_DISPLAYS)
        handle = dfb_p_display_shared->display[displayIndex].displayHandle;

    return handle;
}

DFB_PlatformResult
DFB_Platform_P_GetDisplayOutputStatus( DFB_PlatformDisplayOutputType outputType,
                                       int                           outputIndex)
{
    int                        outputSlot;
    DFB_P_DisplayOutputStatus *pOutputStatus;

    pOutputStatus = &dfb_p_display_shared->output[0];

    for (outputSlot = 0; outputSlot < DFB_PLATFORM_NUM_DISPLAY_OUTPUTS_MAX; outputSlot++)
    {
        if (pOutputStatus->outputType   == outputType &&
            pOutputStatus->outputIndex  == outputIndex)
        {
            return DFB_PLATFORM_BUSY;
        }
        pOutputStatus++;
    }

    return DFB_PLATFORM_OK;
}

DFB_PlatformNexusHandle
DFB_Platform_P_GetDisplayOutputHandle( DFB_PlatformNexusHandle       displayHandle,
                                       DFB_PlatformDisplayOutputType outputType,
                                       int                           outputIndex )
{
    int                        outputSlot;
    int                        displayIndex;
    DFB_PlatformNexusHandle    outputHandle = NULL;
    DFB_P_DisplayOutputStatus *pOutputStatus;

    displayIndex = DFB_Platform_P_GetDisplayIndex(displayHandle);

    if (displayIndex >= 0 && displayIndex < NUM_DISPLAYS)
    {
        pOutputStatus = &dfb_p_display_shared->output[0];

        for (outputSlot = 0; outputSlot < DFB_PLATFORM_NUM_DISPLAY_OUTPUTS_MAX; outputSlot++)
        {
            if (pOutputStatus->displayIndex == displayIndex &&
                pOutputStatus->outputType   == outputType   &&
                pOutputStatus->outputIndex  == outputIndex)
            {
                outputHandle = pOutputStatus->outputHandle;
                break;
            }
            pOutputStatus++;
        }
    }
    return outputHandle;
}

DFB_PlatformResult
DFB_Platform_P_SetDisplayOutputHandle( DFB_PlatformNexusHandle       displayHandle,
                                       DFB_PlatformDisplayOutputType outputType,
                                       int                           outputIndex,
                                       DFB_PlatformNexusHandle       outputHandle )
{
    DFB_PlatformResult         res = DFB_PLATFORM_FAILURE;
    int                        outputSlot;
    int                        displayIndex;
    DFB_P_DisplayOutputStatus *pOutputStatus;

    displayIndex = DFB_Platform_P_GetDisplayIndex(displayHandle);

    if (displayIndex >= 0 && displayIndex < NUM_DISPLAYS)
    {
        pOutputStatus = &dfb_p_display_shared->output[0];

        for (outputSlot = 0; outputSlot < DFB_PLATFORM_NUM_DISPLAY_OUTPUTS_MAX; outputSlot++)
        {
            /* If the output Handle is NULL, then we need to find an already registered slot
               and then remove it. */
            if (outputHandle == NULL)
            {
                if (pOutputStatus->outputType   == outputType &&
                    pOutputStatus->outputIndex  == outputIndex &&
                    pOutputStatus->displayIndex == displayIndex)
                {
                    memset(pOutputStatus, 0, sizeof(*pOutputStatus));
                    res = DFB_PLATFORM_OK;
                    break;
                }
            }
            /* If we are adding a handle, then we need to find a spare slot... */
            else
            {
                if (pOutputStatus->outputHandle == NULL)
                {
                    pOutputStatus->outputHandle = outputHandle;
                    pOutputStatus->outputType   = outputType;
                    pOutputStatus->outputIndex  = outputIndex;
                    pOutputStatus->displayIndex = displayIndex;
                    res = DFB_PLATFORM_OK;
                    break;
                }
            }
            pOutputStatus++;
        }
    }
    return res;
}

/** Returns -1 if no display window found with given handle */
int
DFB_Platform_P_GetDisplayWindowSlot( DFB_PlatformNexusHandle windowHandle )
{
    int windowSlot;

    for (windowSlot = 0; windowSlot < DFB_PLATFORM_NUM_DISPLAY_WINDOWS_MAX; windowSlot++)
    {
        if (dfb_p_display_shared->window[windowSlot].windowHandle == windowHandle)
            break;
    }

    if (windowSlot >= DFB_PLATFORM_NUM_DISPLAY_WINDOWS_MAX)
        windowSlot = -1;

    return windowSlot;
}

DFB_PlatformNexusHandle
DFB_Platform_P_GetDisplayWindowHandle( DFB_PlatformNexusHandle displayHandle,
                                       int                     windowIndex )
{
    int                        displayIndex;
    int                        windowSlot;
    DFB_PlatformNexusHandle    windowHandle = NULL;
    DFB_P_DisplayWindowStatus *pWindowStatus;

    displayIndex = DFB_Platform_P_GetDisplayIndex(displayHandle);

    if (displayIndex >= 0 && displayIndex < NUM_DISPLAYS)
    {
        pWindowStatus = &dfb_p_display_shared->window[0];

        for (windowSlot = 0; windowSlot < DFB_PLATFORM_NUM_DISPLAY_WINDOWS_MAX; windowSlot++)
        {
            if (pWindowStatus->displayIndex == displayIndex && pWindowStatus->windowIndex  == windowIndex)
            {
                windowHandle = pWindowStatus->windowHandle;
                break;
            }
            pWindowStatus++;
        }
    }
    return windowHandle;
}

DFB_PlatformColor
DFB_Platform_P_NexusToDFB_PlatformColor( uint32_t color )
{
    DFB_PlatformColor retColor = {color>>24, (color&0xff0000>>16), (color&0xff00)>>8, color&0xff};
    return retColor;
}

uint32_t
DFB_Platform_P_DFBToNexusColor( DFB_PlatformColor color )
{
    uint32_t retColor = (color.a << 24) | (color.r << 16) | (color.g <<8) |color.b;
    return retColor;
}

NEXUS_VideoFormat
DFB_Platform_P_DFBToNexusVideoFormat( DFB_PlatformVideoFormat format )
{
    return DFB_Platform_P_DFBToNexusVideoFormatTable[format];
}

DFB_PlatformVideoFormat
DFB_Platform_P_NexusToDFBVideoFormat( NEXUS_VideoFormat format )
{
    return DFB_Platform_P_NexusToDFBVideoFormatTable[format];
}

#if NEXUS_HAS_DISPLAY
NEXUS_DisplayType
DFB_Platform_P_DFBToNexusDisplayType( DFB_PlatformDisplayType type )
{
    return DFB_Platform_P_DFBToNexusDisplayTypeTable[type];
}

DFB_PlatformDisplayType
DFB_Platform_P_NexusToDFBDisplayType( NEXUS_DisplayType type )
{
    return DFB_Platform_P_NexusToDFBDisplayTypeTable[type];
}

NEXUS_Timebase
DFB_Platform_P_DFBToNexusTimebase( DFB_PlatformTimebase timebase)
{
    return DFB_Platform_P_DFBToNexusTimebaseTable[timebase];
}
#endif

NEXUS_AspectRatio
DFB_Platform_P_DFBToNexusDisplayAspectRatio( DFB_PlatformDisplayAspectRatio ratio )
{
    return DFB_Platform_P_DFBToNexusDisplayAspectRatioTable[ratio];
}

DFB_PlatformDisplayAspectRatio
DFB_Platform_P_NexusToDFBDisplayAspectRatio( NEXUS_AspectRatio ratio )
{
    return DFB_Platform_P_NexusToDFBDisplayAspectRatioTable[ratio];
}

/********* Public Functions *********/

/* Get the default DFB platform settings */
void
DFB_Platform_GetDefaultSettings_tagged( DFB_PlatformClientType clientType,
                                        DFB_PlatformSettings  *pSettings,
                                        unsigned               versionCheck,
                                        unsigned               structSizeCheck )
{
    DFB_PlatformResult res;
    unsigned  i;

    BDBG_MSG_TRACE((">>%s", __FUNCTION__));

    /* Check for binary compatibility */
    if (versionCheck != DFB_PLATFORM_GET_VERSION()) {
        BDBG_ERR(("DFB_Platform_GetDefaultSettings() failed with version mismatch (dfb=%d.%d, caller=%d.%d). Please recompile application and/or DirectFB.",
            (DFB_PLATFORM_GET_VERSION() & DFB_PLATFORM_P_VERSION_MAJ_MASK) >> DFB_PLATFORM_P_VERSION_MAJ_SHIFT,
            (DFB_PLATFORM_GET_VERSION() & DFB_PLATFORM_P_VERSION_MIN_MASK) >> DFB_PLATFORM_P_VERSION_MIN_SHIFT,
            (versionCheck                 & DFB_PLATFORM_P_VERSION_MAJ_MASK) >> DFB_PLATFORM_P_VERSION_MAJ_SHIFT,
            (versionCheck                 & DFB_PLATFORM_P_VERSION_MIN_MASK) >> DFB_PLATFORM_P_VERSION_MIN_SHIFT));
        BDBG_ASSERT(false);
    }

    if (structSizeCheck != DFB_PLATFORM_P_GET_STRUCT_SIZE()) {
        BDBG_ERR(("DFB_Platform_GetDefaultSettings() failed with struct size mismatch (dfb=%d, caller=%d). Please recompile application and/or DirectFB.",
            DFB_PLATFORM_P_GET_STRUCT_SIZE(), structSizeCheck));
        BDBG_ASSERT(false);
    }

    memset(pSettings, 0, sizeof(*pSettings));

    res = DFB_Platform_P_SetupSharedMemory();

    if (res != DFB_PLATFORM_OK) {
        BDBG_ERR(("DFB_Platform_GetDefaultSettings() failed to setup shared memory!"));
        BDBG_ASSERT(false);
    }

    /* Setup the default settings depending on whether the client application has already initialised
       or joined Nexus first and whether it is a master/slave Nexus process/app. */
    if (clientType == DFB_PlatformClientType_eMasterNexusInitialized)
    {
        /* Nexus already initialised, so DFB Platform code doesn't need to do it. */
        pSettings->initOrJoinNexus = false;
    }
    else if (clientType == DFB_PlatformClientType_eMasterNexusUninitialized)
    {
        /* Nexus not already initialised, so DFB Platform code needs to do it. */
        pSettings->initOrJoinNexus = true;
    }
    else if (clientType == DFB_PlatformClientType_eSlaveNexusInitialized)
    {
        /* Nexus already joined, so DFB Platform code doesn't need to do it. */
        pSettings->initOrJoinNexus = false;
    }
    else if (clientType == DFB_PlatformClientType_eSlaveNexusUninitialized)
    {
        /* Nexus not already joined, so DFB Platform code needs to do it. */
        pSettings->initOrJoinNexus = true;
    }
    else
    {
        BDBG_ERR(("DFB_Platform_GetDefaultSettings() Invalid client type %d!\n", clientType));
    }

    /* If we have already initialised the shared memory, then it means we are definitely a slave. */
    pSettings->slave = !dfb_is_master;

    pSettings->allowUnauthenticatedClients = true;
    pSettings->trustUnauthenticatedClients = false;

    if (getenv("dfb_slave") && !strcmp(getenv("dfb_slave"), "y"))
        pSettings->slave = true;

    if (dfb_is_master)
    {
        /* If we are a DFB master application, then get the default settings */
        DFB_Platform_P_GetDefaultSettings(pSettings);
    }
    else
    {
        /* If we are not a DFB master application, then copy the platform settings from the shared memory. */
        memcpy(&pSettings->platformSettings, &dfb_platform_shared->platformSettings, sizeof(dfb_platform_shared->platformSettings));

        /* Also copy over the server-side settings from shared memory */
        pSettings->allowUnauthenticatedClients = dfb_platform_shared->allowUnauthenticatedClients;
        pSettings->trustUnauthenticatedClients = dfb_platform_shared->trustUnauthenticatedClients;

        /* and copy over the heap handles... */
        pSettings->displayHeapHandle          = dfb_platform_shared->displayHeapHandle;
        pSettings->secondaryDisplayHeapHandle = dfb_platform_shared->secondaryDisplayHeapHandle;
        pSettings->offscreenHeapHandle        = dfb_platform_shared->offscreenHeapHandle;
        pSettings->paletteHeapHandle          = dfb_platform_shared->paletteHeapHandle;
        pSettings->videoHeapHandle            = dfb_platform_shared->videoHeapHandle;
        pSettings->pictureDecoderHeapHandle   = dfb_platform_shared->pictureDecoderHeapHandle;
    }

    if (dfb_platform_shared->graphics2d.handle == NULL)
        pSettings->graphics2d.init = DFB_Platform_P_Graphics2D_GetDefaultInitSettings(&pSettings->graphics2d.initSettings);

    if (dfb_platform_shared->graphics3d.handle == NULL)
        pSettings->graphics3d.init = DFB_Platform_P_Graphics3D_GetDefaultInitSettings(&pSettings->graphics3d.initSettings);

    if (dfb_platform_shared->pictureDecoder.handle == NULL)
        pSettings->pictureDecoder.init = DFB_Platform_P_PictureDecoder_GetDefaultInitSettings(&pSettings->pictureDecoder.initSettings);

    /* Initialise the displays if we are a master */
    if (dfb_is_master)
    {
        for (i = 0; i < DFB_PLATFORM_NUM_DISPLAYS_MAX; i++)
        {
            if (dfb_platform_shared->display[i].handle == NULL)
                pSettings->display[i].init = DFB_Platform_P_Display_GetDefaultInitSettings( i, &pSettings->display[i].initSettings );
        }

        /* Initialise the display windows */
        for (i = 0; i < DFB_PLATFORM_NUM_DISPLAY_WINDOWS_MAX; i++)
        {
            if (dfb_platform_shared->displayWindow[i].handle == NULL)
                pSettings->displayWindow[i].init = DFB_Platform_P_DisplayWindow_GetDefaultInitSettings( i, &pSettings->displayWindow[i].initSettings );
        }

        /* Initialise the display outputs */
        for (i = 0; i < DFB_PLATFORM_NUM_DISPLAY_OUTPUTS_MAX; i++)
        {
            if (dfb_platform_shared->displayOutput[i].handle == NULL)
                pSettings->displayOutput[i].init = DFB_Platform_P_DisplayOutput_GetDefaultInitSettings( i, &pSettings->displayOutput[i].initSettings );
        }
    }

    BDBG_MSG_TRACE(("<<%s", __FUNCTION__));
}

DFB_PlatformResult
DFB_Platform_GetStatus( DFB_PlatformStatus *pStatus )
{
    DFB_PlatformResult res;

    BDBG_MSG_TRACE((">>%s", __FUNCTION__));

    BDBG_ASSERT(pStatus);

    res = DFB_Platform_P_GetStatus(pStatus);

    BDBG_MSG_TRACE(("<<%s", __FUNCTION__));
    return res;
}

DFB_PlatformResult
DFB_Platform_GetSettings( DFB_PlatformSettings *pSettings )
{
    DFB_PlatformResult res;
    DFB_PlatformStatus platformStatus;

    BDBG_MSG_TRACE((">>%s", __FUNCTION__));

    BDBG_ASSERT(pSettings);

    res = DFB_Platform_P_GetStatus(&platformStatus);

    if (res == DFB_PLATFORM_OK)
    {
        if (!platformStatus.initOrJoinNexus)
        {
            BDBG_ERR(("%s: DFB Platform not initialised. Please ensure DFB_Platform_Init() is called first.\n", __FUNCTION__));
            res = DFB_PLATFORM_INIT;
        }
        else
        {
            res = DFB_Platform_P_GetSettings(pSettings);
        }
    }

    BDBG_MSG_TRACE(("<<%s", __FUNCTION__));
    return res;
}

void
DFB_Platform_GetVersion( unsigned *pMajor,
                         unsigned *pMinor )
{

    BDBG_MSG_TRACE((">>%s", __FUNCTION__));

    BDBG_ASSERT(pMajor);
    BDBG_ASSERT(pMinor);

    *pMajor = (DFB_PLATFORM_GET_VERSION() & DFB_PLATFORM_P_VERSION_MAJ_MASK) >> DFB_PLATFORM_P_VERSION_MAJ_SHIFT;
    *pMinor = (DFB_PLATFORM_GET_VERSION() & DFB_PLATFORM_P_VERSION_MIN_MASK) >> DFB_PLATFORM_P_VERSION_MIN_SHIFT;

    BDBG_MSG_TRACE(("<<%s", __FUNCTION__));
}

DFB_PlatformResult
DFB_Platform_GetDisplaySettings( DFB_PlatformNexusHandle displayHandle, DFB_PlatformDisplaySettings *pSettings)
{
    DFB_PlatformResult             res;
    DFB_Platform_P_DisplaySettings displaySettings;

    BDBG_MSG_TRACE((">>%s", __FUNCTION__));

    BDBG_ASSERT(pSettings);

    if (!dfb_platform_initialised)
    {
        BDBG_ERR(("%s: DFB Platform not initialised. Please ensure DFB_Platform_Init() is called first.\n", __FUNCTION__));
        res = DFB_PLATFORM_INIT;
    }
    else
    {
        if (DFB_Platform_P_GetDisplayIndex(displayHandle) == -1)
        {
            BDBG_ERR(("%s: Invalid display handle %p!\n", __FUNCTION__, (void *)displayHandle));
            res = DFB_PLATFORM_INVARG;
        }
        else
        {
            res = DFB_Platform_P_GetDisplaySettings(displayHandle, &displaySettings);
            if (res != DFB_PLATFORM_OK)
                BDBG_ERR(("%s: Cannot get private display settings (res=%d)!\n", __FUNCTION__, res));
            else
                memcpy(pSettings, &displaySettings, sizeof(*pSettings));
        }
    }

    BDBG_MSG_TRACE(("<<%s", __FUNCTION__));
    return res;
}

DFB_PlatformResult
DFB_Platform_SetDisplaySettings( DFB_PlatformNexusHandle displayHandle, DFB_PlatformDisplaySettings *pSettings)
{
    DFB_PlatformResult             res;
    DFB_Platform_P_DisplaySettings displaySettings;

    BDBG_MSG_TRACE((">>%s", __FUNCTION__));

    BDBG_ASSERT(pSettings);

    if (!dfb_platform_initialised)
    {
        BDBG_ERR(("%s: DFB Platform not initialised. Please ensure DFB_Platform_Init() is called first.\n", __FUNCTION__));
        res = DFB_PLATFORM_INIT;
    }
    else
    {
        if (DFB_Platform_P_GetDisplayIndex(displayHandle) == -1)
        {
            BDBG_ERR(("%s: Invalid display handle %p!\n", __FUNCTION__, (void *)displayHandle));
            res = DFB_PLATFORM_INVARG;
        }
        else
        {
            memset(&displaySettings, 0, sizeof(displaySettings));
            memcpy(&displaySettings, pSettings, sizeof(*pSettings));
            displaySettings.vsyncCallback.callback = (DFB_PlatformCallback)-1; /* Signal don't set */

            res = DFB_Platform_P_SetDisplaySettings( displayHandle, &displaySettings );
            if (res != DFB_PLATFORM_OK)
                BDBG_ERR(("%s: Cannot set private display settings (res=%d)!\n", __FUNCTION__, res));
        }
    }
    BDBG_MSG_TRACE(("<<%s", __FUNCTION__));
    return res;
}

DFB_PlatformResult
DFB_Platform_GetDisplayWindow( DFB_PlatformNexusHandle       displayHandle,
                               int                           windowIndex,
                               DFB_PlatformNexusHandle      *pHandle )
{
    DFB_PlatformResult res = DFB_PLATFORM_OK;
    int       displayIndex;

    BDBG_MSG_TRACE((">>%s", __FUNCTION__));

    BDBG_ASSERT(displayHandle);
    BDBG_ASSERT(pHandle);

    if (!dfb_platform_initialised)
    {
        BDBG_ERR(("%s: DFB Platform not initialised. Please ensure DFB_Platform_Init() is called first.\n", __FUNCTION__));
        res = DFB_PLATFORM_INIT;
    }
    else
    {
        displayIndex = DFB_Platform_P_GetDisplayIndex(displayHandle);

        if (displayIndex == -1)
        {
            BDBG_ERR(("%s: Invalid display handle %p!\n", __FUNCTION__, (void *)displayHandle));
            res = DFB_PLATFORM_INVARG;
        }
        else
        {
            *pHandle = DFB_Platform_P_GetDisplayWindowHandle(displayHandle, windowIndex);

            if (*pHandle == NULL)
            {
                BDBG_ERR(("%s: Cannot get window handle for display %d and window index %d (res=%d)!\n", __FUNCTION__, displayIndex, windowIndex));
                res = DFB_PLATFORM_FAILURE;
            }
        }
    }

    BDBG_MSG_TRACE(("<<%s", __FUNCTION__));
    return res;

}

DFB_PlatformResult
DFB_Platform_AddDisplayOutput( DFB_PlatformNexusHandle       displayHandle,
                               DFB_PlatformDisplayOutputType outputType,
                               int                           outputIndex )
{
    DFB_PlatformResult      res;

    BDBG_MSG_TRACE((">>%s", __FUNCTION__));

    BDBG_ASSERT(displayHandle);

    if (!dfb_platform_initialised)
    {
        BDBG_ERR(("%s: DFB Platform not initialised. Please ensure DFB_Platform_Init() is called first.\n", __FUNCTION__));
        res = DFB_PLATFORM_INIT;
    }
    else
    {
        if (DFB_Platform_P_GetDisplayIndex(displayHandle) == -1)
        {
            BDBG_ERR(("%s: Invalid display handle %p!\n", __FUNCTION__, (void *)displayHandle));
            res = DFB_PLATFORM_INVARG;
        }
        else
        {
            if (outputType >= DFB_PlatformDisplayOutputType_eMax)
            {
                BDBG_ERR(("%s: Invalid output connector type %d!\n", __FUNCTION__, outputType));
                res = DFB_PLATFORM_INVARG;
            }
            else
            {
                res = DFB_Platform_P_GetDisplayOutputStatus(outputType, outputIndex);

                if (res == DFB_PLATFORM_OK)
                {
                    res = DFB_Platform_P_AddDisplayOutput(displayHandle, outputType, outputIndex);
                    if (res != DFB_PLATFORM_OK)
                        BDBG_ERR(("%s: Cannot add output of type %d and index %d to display (res=%d)!\n", __FUNCTION__,
                                   outputType, outputIndex, res));
                }
                else
                    BDBG_ERR(("%s: Output handle for output of type %d and index %d busy! (res=%d)!\n", __FUNCTION__,
                               outputType, outputIndex, res));
            }
        }
    }

    BDBG_MSG_TRACE(("<<%s", __FUNCTION__));
    return res;
}

DFB_PlatformResult
DFB_Platform_GetDisplayOutput( DFB_PlatformNexusHandle       displayHandle,
                               DFB_PlatformDisplayOutputType outputType,
                               int                           outputIndex,
                               DFB_PlatformNexusHandle      *pHandle )
{
    DFB_PlatformResult res;

    BDBG_MSG_TRACE((">>%s", __FUNCTION__));

    BDBG_ASSERT(displayHandle);
    BDBG_ASSERT(pHandle);

    if (!dfb_platform_initialised)
    {
        BDBG_ERR(("%s: DFB Platform not initialised. Please ensure DFB_Platform_Init() is called first.\n", __FUNCTION__));
        res = DFB_PLATFORM_INIT;
    }
    else
    {
        if (DFB_Platform_P_GetDisplayIndex(displayHandle) == -1)
        {
            BDBG_ERR(("%s: Invalid display handle %p!\n", __FUNCTION__, (void *)displayHandle));
            res = DFB_PLATFORM_INVARG;
        }
        else
        {
            if (outputType >= DFB_PlatformDisplayOutputType_eMax)
            {
                BDBG_ERR(("%s: Invalid output connector type %d!\n", __FUNCTION__, outputType));
                res = DFB_PLATFORM_INVARG;
            }
            else
            {
                res = DFB_Platform_P_GetDisplayOutput(displayHandle, outputType, outputIndex, pHandle);
                if (res != DFB_PLATFORM_OK)
                    BDBG_ERR(("%s: Cannot get output handle for output of type %d and index %d (res=%d)!\n", __FUNCTION__,
                               outputType, outputIndex, res));
            }
        }
    }

    BDBG_MSG_TRACE(("<<%s", __FUNCTION__));
    return res;

}

DFB_PlatformResult
DFB_Platform_SetDisplayOutput( DFB_PlatformNexusHandle       displayHandle,
                               DFB_PlatformDisplayOutputType outputType,
                               int                           outputIndex,
                               DFB_PlatformNexusHandle       handle )
{
    DFB_PlatformResult res;

    BDBG_MSG_TRACE((">>%s", __FUNCTION__));

    BDBG_ASSERT(displayHandle);

    if (!dfb_platform_initialised)
    {
        BDBG_ERR(("%s: DFB Platform not initialised. Please ensure DFB_Platform_Init() is called first.\n", __FUNCTION__));
        res = DFB_PLATFORM_INIT;
    }
    else
    {
        if (DFB_Platform_P_GetDisplayIndex(displayHandle) == -1)
        {
            BDBG_ERR(("%s: Invalid display handle %p!\n", __FUNCTION__, (void *)displayHandle));
            res = DFB_PLATFORM_INVARG;
        }
        else
        {
            if (outputType >= DFB_PlatformDisplayOutputType_eMax)
            {
                BDBG_ERR(("%s: Invalid output connector type %d!\n", __FUNCTION__, outputType));
                res = DFB_PLATFORM_INVARG;
            }
            else
            {
                res = DFB_Platform_P_SetDisplayOutput(displayHandle, outputType, outputIndex, handle);
                if (res != DFB_PLATFORM_OK)
                    BDBG_ERR(("%s: Cannot set output handle %p for output of type %d and index %d (res=%d)!\n", __FUNCTION__,
                               (void *)handle, outputType, outputIndex, res));
            }
        }
    }

    BDBG_MSG_TRACE(("<<%s", __FUNCTION__));
    return res;
}

DFB_PlatformResult
DFB_Platform_RemoveDisplayOutput( DFB_PlatformNexusHandle       displayHandle,
                                  DFB_PlatformDisplayOutputType outputType,
                                  int                           outputIndex )
{
    DFB_PlatformResult      res;

    BDBG_MSG_TRACE((">>%s", __FUNCTION__));

    BDBG_ASSERT(displayHandle);

    if (!dfb_platform_initialised)
    {
        BDBG_ERR(("%s: DFB Platform not initialised. Please ensure DFB_Platform_Init() is called first.\n", __FUNCTION__));
        res = DFB_PLATFORM_INIT;
    }
    else
    {
        if (DFB_Platform_P_GetDisplayIndex(displayHandle) == -1)
        {
            BDBG_ERR(("%s: Invalid display handle %p!\n", __FUNCTION__, (void *)displayHandle));
            res = DFB_PLATFORM_INVARG;
        }
        else
        {
            if (outputType >= DFB_PlatformDisplayOutputType_eMax)
            {
                BDBG_ERR(("%s: Invalid output connector type %d!\n", __FUNCTION__, outputType));
                res = DFB_PLATFORM_INVARG;
            }
            else
            {
                res =  DFB_Platform_P_GetDisplayOutputStatus(outputType, outputIndex);
                if (res != DFB_PLATFORM_BUSY)
                {
                    BDBG_ERR(("%s: Output of type %d and index %d has already been removed!\n", __FUNCTION__, outputType, outputIndex));
                    res = DFB_PLATFORM_FAILURE;
                }
                else
                {
                    res = DFB_Platform_P_RemoveDisplayOutput(displayHandle, outputType, outputIndex);
                    if (res != DFB_PLATFORM_OK)
                        BDBG_ERR(("%s: Cannot remove output of type %d and index %d from display (res=%d)!\n", __FUNCTION__,
                                    outputType, outputIndex, res));
                }
            }
        }
    }

    BDBG_MSG_TRACE(("<<%s", __FUNCTION__));
    return res;
}

DFB_PlatformResult
DFB_Platform_RemoveDisplayOutputs( DFB_PlatformNexusHandle displayHandle )
{
    DFB_PlatformResult            res = DFB_PLATFORM_OK;
    int                           outputSlot;
    int                           outputIndex;
    int                           displayIndex;
    DFB_PlatformDisplayOutputType outputType;
    DFB_P_DisplayOutputStatus    *pOutputStatus;


    BDBG_MSG_TRACE((">>%s", __FUNCTION__));

    BDBG_ASSERT(displayHandle);

    if (!dfb_platform_initialised)
    {
        BDBG_ERR(("%s: DFB Platform not initialised. Please ensure DFB_Platform_Init() is called first.\n", __FUNCTION__));
        res = DFB_PLATFORM_INIT;
    }
    else
    {
        displayIndex = DFB_Platform_P_GetDisplayIndex(displayHandle);

        if (displayIndex == -1)
        {
            BDBG_ERR(("%s: Invalid display handle %p!\n", __FUNCTION__, (void *)displayHandle));
            res = DFB_PLATFORM_INVARG;
        }
        else
        {
            pOutputStatus = &dfb_p_display_shared->output[0];

            for (outputSlot = 0; outputSlot < DFB_PLATFORM_NUM_DISPLAY_OUTPUTS_MAX; outputSlot++)
            {
                if (pOutputStatus->outputHandle && (displayIndex == pOutputStatus->displayIndex))
                {
                    outputType  = pOutputStatus->outputType;
                    outputIndex = pOutputStatus->outputIndex;

                    res = DFB_Platform_P_RemoveDisplayOutput(displayHandle, outputType, outputIndex);
                    if (res != DFB_PLATFORM_OK)
                    {
                        BDBG_ERR(("%s: Cannot remove output of type %d and index %d from display%d (res=%d)!\n", __FUNCTION__,
                                    outputType, outputIndex, displayIndex, res));
                        break;
                    }
                }
                pOutputStatus++;
            }
        }
     }

    BDBG_MSG_TRACE(("<<%s", __FUNCTION__));
    return res;
}

DFB_PlatformResult
DFB_Platform_HdmiHotplugHandler( DFB_PlatformNexusHandle displayHandle,
                                 DFB_PlatformNexusHandle hdmiHandle )
{
    DFB_PlatformResult res;

    BDBG_MSG_TRACE((">>%s", __FUNCTION__));

    BDBG_ASSERT(displayHandle);
    BDBG_ASSERT(hdmiHandle);

    if (!dfb_platform_initialised)
    {
        BDBG_ERR(("%s: DFB Platform not initialised. Please ensure DFB_Platform_Init() is called first.\n", __FUNCTION__));
        res = DFB_PLATFORM_INIT;
    }
    else
    {
         if (DFB_Platform_P_GetDisplayIndex(displayHandle) == -1)
        {
            BDBG_ERR(("%s: Invalid display handle %p!\n", __FUNCTION__, (void *)displayHandle));
            res = DFB_PLATFORM_INVARG;
        }
        else
        {
            res = DFB_Platform_P_HdmiHotplugHandler(displayHandle, hdmiHandle);
            if (res != DFB_PLATFORM_OK)
                BDBG_ERR(("%s: Problem encountered setting HDMI hotplug handler (res=%d)!\n", __FUNCTION__, res));
        }
     }

    BDBG_MSG_TRACE(("<<%s", __FUNCTION__));
    return res;
}

/* An external application can call this function (via DFB_Platform_Init()) to pass information about handles, heaps, etc. to DFB */
DFB_PlatformResult
DFB_Platform_Init_tagged( DFB_PlatformSettings *pSettings,
                          unsigned              versionCheck,
                          unsigned              structSizeCheck )
{
    DFB_PlatformResult   err = DFB_PLATFORM_OK;
    int                  displayIndex;
    int                  windowSlot;
    int                  outputSlot;

    BDBG_MSG_TRACE((">>%s", __FUNCTION__));

    BDBG_Init();

    /* Check for binary compatibility */
    if (versionCheck != DFB_PLATFORM_GET_VERSION()) {
        BDBG_ERR(("DFB_Platform_Init() failed with version mismatch (dfb=%d.%d, caller=%d.%d). Please recompile application and/or DirectFB.",
            (DFB_PLATFORM_GET_VERSION() & DFB_PLATFORM_P_VERSION_MAJ_MASK) >> DFB_PLATFORM_P_VERSION_MAJ_SHIFT,
            (DFB_PLATFORM_GET_VERSION() & DFB_PLATFORM_P_VERSION_MIN_MASK) >> DFB_PLATFORM_P_VERSION_MIN_SHIFT,
            (versionCheck                 & DFB_PLATFORM_P_VERSION_MAJ_MASK) >> DFB_PLATFORM_P_VERSION_MAJ_SHIFT,
            (versionCheck                 & DFB_PLATFORM_P_VERSION_MIN_MASK) >> DFB_PLATFORM_P_VERSION_MIN_SHIFT));
        return DFB_PLATFORM_VERSIONMISMATCH;
    }

    if (structSizeCheck != DFB_PLATFORM_P_GET_STRUCT_SIZE()) {
        BDBG_ERR(("DFB_Platform_Init() failed with struct size mismatch (dfb=%d, caller=%d). Please recompile application and/or DirectFB.",
            DFB_PLATFORM_P_GET_STRUCT_SIZE(), structSizeCheck));
        return DFB_PLATFORM_INVARG;
    }

    /* If the client hasn't specified any settings, then this is also an error! */
    if (pSettings == NULL)
    {
        BDBG_ERR(("DFB_Platform_Init() failed because no settings were passed!"));
        return DFB_PLATFORM_INVARG;
    }

    /* If we have already initialised the platform code in the same process, then no need to do it again! */
    if (dfb_platform_initialised == true)
    {
        BDBG_WRN(("DFB_Platform_Init(): DFB Platform already initialised!"));
        return DFB_PLATFORM_OK;
    }

    BDBG_WRN(("Broadcom Corporation DirectFB%s Platform Layer Version %d.%d",
              (isXS ? "-XS" : ""), DFB_PLATFORM_VERSION_MAJOR, DFB_PLATFORM_VERSION_MINOR));

    err = DFB_Platform_P_SetupSharedMemory();

    if (err != DFB_PLATFORM_OK) {
        BDBG_ERR(("DFB_Platform_Init() failed to setup shared memory!"));
        return err;
    }

    /* Ensure per process device status is cleared */
    memset(&dfb_device_status, 0, sizeof(dfb_device_status));

     /* Perform initialisation */
    err = DFB_Platform_P_Init(pSettings, dfb_p_display_shared);

    if (err != DFB_PLATFORM_OK) {
        BDBG_ERR(("DFB_Platform_Init() failed to initialise private settings!"));

         return err;
    }

    /* Always ensure that the heap handles are initialised */
    DFB_Platform_P_SetHeapHandles(pSettings);

    /* Display the heaps */
    DFB_Platform_P_DisplayHeaps(pSettings);

    /* Setup the displays */
    for (displayIndex = 0; displayIndex < NUM_DISPLAYS; displayIndex++)
    {
        /* Should we initialise the display? */
        if (pSettings->display[displayIndex].init)
        {
#if !BCMNEXUS_NSC_SUPPORT
            /* Check to make sure that another process hasn't already opened the display */
            if (dfb_p_display_shared->display[displayIndex].displayHandle != NULL)
            {
                BDBG_WRN(("DFB_Platform_Init() Display module already opened!\n"));
                continue;
            }
#endif

            /* Do we need to actually open the display? */
            if (pSettings->display[displayIndex].handle == NULL)
            {
                BDBG_MSG(("DFB_Platform_Init() Opening display%d...\n", displayIndex));
                err = DFB_Platform_P_Display_Init(displayIndex, &pSettings->display[displayIndex].initSettings, &pSettings->display[displayIndex].handle);

                /* Indicate that the display was opened by this process... */
                dfb_device_status.IsDisplayOpened[displayIndex] = (err == DFB_PLATFORM_OK);
            }

            if (err == DFB_PLATFORM_OK)
            {
                if (pSettings->display[displayIndex].handle == NULL)
                {
                    BDBG_ERR(("DFB_Platform_Init() Cannot open display %d!\n", displayIndex));
                    err = DFB_PLATFORM_FAILURE;
                    goto err_init;
                }
                else
                {
                    DFB_PlatformVideoFormat format;

                    /* Save the display state if it is really open */
                    memcpy(&dfb_platform_shared->display[displayIndex], &pSettings->display[displayIndex], sizeof(pSettings->display[0]));

                    format = pSettings->display[displayIndex].initSettings.format;

                    /* Also ensure that the display shared state is consistent */
                    dfb_p_display_shared->display[displayIndex].displayHandle               = pSettings->display[displayIndex].handle;
                    dfb_p_display_shared->display[displayIndex].displaySettings.format      = format;
                    dfb_p_display_shared->display[displayIndex].displaySettings.aspectRatio = pSettings->display[displayIndex].initSettings.aspectRatio;
                    memcpy(&dfb_p_display_shared->display[displayIndex].displaySettings.background,
                           &pSettings->display[displayIndex].initSettings.background,
                           sizeof(dfb_p_display_shared->display[displayIndex].displaySettings.background));

                    DFB_Platform_P_DFBFormatToDisplaySize(format, &dfb_p_display_shared->display[displayIndex].displaySize);
                }
            }
        }
    }

    /* Setup the display windows */
    for (windowSlot = 0; windowSlot < DFB_PLATFORM_NUM_DISPLAY_WINDOWS_MAX; windowSlot++)
    {
        /* Should we initialise the output? */
        if (pSettings->displayWindow[windowSlot].init)
        {
            /* Check to make sure that another process hasn't already init the display window */
            if (dfb_platform_shared->displayWindow[windowSlot].handle != NULL)
            {
                BDBG_WRN(("DFB_Platform_Init() Display Window slot %d already opened!\n", windowSlot));
                continue;
            }

            /* Do we need to actually init the display window? */
            if (pSettings->displayWindow[windowSlot].handle == NULL)
            {
                BDBG_MSG(("DFB_Platform_Init() Opening display window slot%d...\n", windowSlot));
                err = DFB_Platform_P_DisplayWindow_Init(&pSettings->displayWindow[windowSlot].initSettings, &pSettings->displayWindow[windowSlot].handle);

                /* Indicate that the window was opened by this process... */
                dfb_device_status.IsWindowOpened[windowSlot] = (err == DFB_PLATFORM_OK);
            }

            if (err == DFB_PLATFORM_OK)
            {
                if (pSettings->displayWindow[windowSlot].handle == NULL)
                {
                    BDBG_ERR(("DFB_Platform_Init() Cannot open display window slot %d!\n", windowSlot));
                    err = DFB_PLATFORM_FAILURE;
                    goto err_init;
                }
                else
                {
                    /* Save the display window state if it is really open */
                    memcpy(&dfb_platform_shared->displayWindow[windowSlot], &pSettings->displayWindow[windowSlot], sizeof(pSettings->displayWindow[0]));

                    /* Also ensure that the display window shared state is consistent */
                    dfb_p_display_shared->window[windowSlot].windowHandle = pSettings->displayWindow[windowSlot].handle;
                    dfb_p_display_shared->window[windowSlot].windowIndex  = pSettings->displayWindow[windowSlot].initSettings.windowIndex;
                    dfb_p_display_shared->window[windowSlot].displayIndex = pSettings->displayWindow[windowSlot].initSettings.displayIndex;
                }
            }
        }
    }

    /* Setup the display outputs */
    for (outputSlot = 0; outputSlot < DFB_PLATFORM_NUM_DISPLAY_OUTPUTS_MAX; outputSlot++)
    {
        /* Should we initialise the output? */
        if (pSettings->displayOutput[outputSlot].init)
        {
            /* Check to make sure that another process hasn't already init the output */
            if (dfb_p_display_shared->output[outputSlot].outputHandle != NULL)
            {
                BDBG_WRN(("DFB_Platform_Init() Output slot %d already opened!\n", outputSlot));
                continue;
            }

            /* Do we need to actually init the output? */
            if (pSettings->displayOutput[outputSlot].handle == NULL)
            {
                BDBG_MSG(("DFB_Platform_Init() Opening output slot%d...\n", outputSlot));
                err = DFB_Platform_P_DisplayOutput_Init(&pSettings->displayOutput[outputSlot].initSettings, &pSettings->displayOutput[outputSlot].handle);
            }

            if (err == DFB_PLATFORM_OK)
            {
                if (pSettings->displayOutput[outputSlot].handle == NULL)
                {
                    BDBG_ERR(("DFB_Platform_Init() Cannot open output slot %d!\n", outputSlot));
                    err = DFB_PLATFORM_FAILURE;
                    goto err_init;
                }
                else
                {
                    /* Save the output state if it is really open */
                    memcpy(&dfb_platform_shared->displayOutput[outputSlot], &pSettings->displayOutput[outputSlot], sizeof(pSettings->displayOutput[0]));

                    /* Also ensure that the output shared state is consistent */
                    dfb_p_display_shared->output[outputSlot].outputHandle = pSettings->displayOutput[outputSlot].handle;
                    dfb_p_display_shared->output[outputSlot].outputType   = pSettings->displayOutput[outputSlot].initSettings.outputType;
                    dfb_p_display_shared->output[outputSlot].outputIndex  = pSettings->displayOutput[outputSlot].initSettings.outputIndex;
                    dfb_p_display_shared->output[outputSlot].displayIndex = pSettings->displayOutput[outputSlot].initSettings.displayIndex;
                }
            }
        }
    }

    /* Should we initialise the graphics 2D module? */
    if (pSettings->graphics2d.init == true)
    {
#if !BCMNEXUS_NSC_SUPPORT
        /* Check to make sure that another process hasn't already opened the graphics2d module */
        if (dfb_platform_shared->graphics2d.handle != NULL)
        {
            BDBG_WRN(("DFB_Platform_Init() Graphics2d module already opened!\n"));
        }
        else
#endif
        {
            /* Do we need to actually open the graphics2d module? */
            if (pSettings->graphics2d.handle == NULL)
            {
                BDBG_MSG(("DFB_Platform_Init() Opening graphics2d module...\n"));
                err = DFB_Platform_P_Graphics2D_Init(&pSettings->graphics2d.initSettings, &pSettings->graphics2d.handle);

                /* Indicate successfull open of the graphics2d device in the current process... */
                dfb_device_status.IsGraphics2DOpened = (err == DFB_PLATFORM_OK);
            }

            if (err == DFB_PLATFORM_OK)
            {
                if (pSettings->graphics2d.handle == NULL)
                {
                    BDBG_ERR(("DFB_Platform_Init() Cannot open graphics2d!\n"));
                    err = DFB_PLATFORM_FAILURE;
                    goto err_init;
                }
                else
                {
                    /* Save the graphics2d module state if it is really open */
                    memcpy(&dfb_platform_shared->graphics2d, &pSettings->graphics2d, sizeof(pSettings->graphics2d));
                }
            }
        }
    }

    /* Should we initialise the graphics 3D module? */
    if (pSettings->graphics3d.init == true)
    {
        /* Check to make sure that another process hasn't already opened the graphics3d module */
        if (dfb_platform_shared->graphics3d.handle != NULL)
        {
            BDBG_WRN(("DFB_Platform_Init() Graphics3d module already opened!\n"));
        }
        else
        {
            /* Do we need to actually open the graphics3d module? */
            if (pSettings->graphics3d.handle == NULL)
            {
                BDBG_MSG(("DFB_Platform_Init() Opening graphics3d module...\n"));
                err = DFB_Platform_P_Graphics3D_Init(&pSettings->graphics3d.initSettings, &pSettings->graphics3d.handle);

                /* Indicate successfull open of the graphics3d device in the current process... */
                dfb_device_status.IsGraphics3DOpened = (err == DFB_PLATFORM_OK);
            }

            if (err == DFB_PLATFORM_OK)
            {
                if (pSettings->graphics3d.handle == NULL)
                {
                    BDBG_ERR(("DFB_Platform_Init() Cannot open graphics3d!\n"));
                    err = DFB_PLATFORM_FAILURE;
                    goto err_init;
                }
                else
                {
                    /* Save the graphics3d module state if it is really open */
                    memcpy(&dfb_platform_shared->graphics3d, &pSettings->graphics3d, sizeof(pSettings->graphics3d));
                }
            }
        }
    }

    /* Should we initialise the picture decoder module? */
    if (pSettings->pictureDecoder.init == true && !getenv("sw_picture_decode"))
    {
        /* Check to make sure that another process hasn't already opened the picture decoder module */
        if (dfb_platform_shared->pictureDecoder.handle != NULL)
        {
            BDBG_WRN(("DFB_Platform_Init() Picture Decoder module already opened!\n"));
        }
        else
        {
            /* Do we need to actually open the picture decoder module? */
            if (pSettings->pictureDecoder.handle == NULL)
            {
                BDBG_MSG(("DFB_Platform_Init() Opening picture decoder module...\n"));
                err = DFB_Platform_P_PictureDecoder_Init(&pSettings->pictureDecoder.initSettings, &pSettings->pictureDecoder.handle);

                /* Indicate successfull open of the picture decoder device in the current process... */
                dfb_device_status.IsPictureDecoderOpened = (err == DFB_PLATFORM_OK);
            }

            if (err == DFB_PLATFORM_OK)
            {
                if (pSettings->pictureDecoder.handle == NULL)
                {
                    BDBG_ERR(("DFB_Platform_Init() Cannot open picture decoder!\n"));
                    err = DFB_PLATFORM_FAILURE;
                    goto err_init;
                }
                else
                {
                    /* Save the picture decoder module state if it is really open */
                    memcpy(&dfb_platform_shared->pictureDecoder, &pSettings->pictureDecoder, sizeof(pSettings->pictureDecoder));
                }
            }
        }
    }


    /* Only copy all settings over to shared memory if we are the master process */
    if (dfb_is_master)
    {
        BDBG_MSG(("DFB_Platform_Init() Copying platform settings to shared memory...\n"));
        pSettings->platformSettings.platforSettingsValid =(unsigned int) PLATFORM_SETTINGS_VALID;
        *dfb_platform_shared = *pSettings;
        dfb_platform_status_shared->isMaster = true;
    }
    else
    {
        dfb_platform_status_shared->isMaster = false;
    }

    /* Indicate that we have at this point successfully initialised the DFB Platform code */
    dfb_platform_initialised = true;                        /* Local process state */
    dfb_platform_status_shared->initOrJoinNexus = true;     /* Global process state */

     goto err_ok;

err_init: 
    DFB_Platform_P_CloseSharedMemory(dfb_platform_shared);
err_ok:
    BDBG_MSG_TRACE(("<<%s", __FUNCTION__));
    return err;
}

DFB_PlatformResult
DFB_Platform_Uninit( void )
{
    DFB_PlatformResult res;
    int       displayIndex;
    int       windowSlot;

    BDBG_MSG_TRACE((">>%s", __FUNCTION__));

    if (!dfb_platform_initialised)
    {
        BDBG_WRN(("%s(): DFB Platform already uninitialised.\n", __FUNCTION__));
        res = DFB_PLATFORM_OK;
    }
    else
    {
        if (dfb_device_status.IsGraphics3DOpened)
        {
            DFB_Platform_P_Graphics3D_Uninit(dfb_platform_shared->graphics3d.handle);
            dfb_platform_shared->graphics3d.handle = NULL;
            dfb_device_status.IsGraphics3DOpened  = false;
        }

        if (dfb_device_status.IsGraphics2DOpened)
        {
            DFB_Platform_P_Graphics2D_Uninit(dfb_platform_shared->graphics2d.handle);
            dfb_platform_shared->graphics2d.handle = NULL;
            dfb_device_status.IsGraphics2DOpened  = false;
        }

        if (dfb_device_status.IsPictureDecoderOpened)
        {
            DFB_Platform_P_PictureDecoder_Uninit(dfb_platform_shared->pictureDecoder.handle);
            dfb_platform_shared->pictureDecoder.handle = NULL;
            dfb_device_status.IsPictureDecoderOpened  = false;
        }

        for (windowSlot = 0; windowSlot < DFB_PLATFORM_NUM_DISPLAY_WINDOWS_MAX; windowSlot++)
        {
            if (dfb_device_status.IsWindowOpened[windowSlot])
            {
                DFB_Platform_P_DisplayWindow_Uninit(dfb_platform_shared->displayWindow[windowSlot].handle);
                dfb_platform_shared->displayWindow[windowSlot].handle = NULL;
                dfb_device_status.IsWindowOpened[windowSlot] = false;
            }
        }

        for (displayIndex = 0; displayIndex < NUM_DISPLAYS; displayIndex++)
        {
            if (dfb_device_status.IsDisplayOpened[displayIndex])
            {
                DFB_Platform_RemoveDisplayOutputs(dfb_platform_shared->display[displayIndex].handle);

                DFB_Platform_P_Display_Uninit(dfb_platform_shared->display[displayIndex].handle);
                dfb_platform_shared->display[displayIndex].handle = NULL;
                dfb_device_status.IsDisplayOpened[displayIndex]  = false;
            }
        }

        res = DFB_Platform_P_Uninit();

        if (res != DFB_PLATFORM_OK)
        {
            BDBG_ERR(("%s: Cannot uninitialise platform (res=%d)!\n", __FUNCTION__, res));
        }
        else
        {
            if (dfb_is_master)
            {
                dfb_platform_status_shared->initOrJoinNexus = false;
            }
#if FUSION_BUILD_MULTI
            else
            {
                simple_client_shutdown();
            }
#endif
            BDBG_MSG(("%s: Destroying The Shared Memory\n", __FUNCTION__));
            DFB_Platform_P_CloseSharedMemory(dfb_platform_shared);
            dfb_platform_shared = NULL;
            dfb_platform_initialised = false;
        }
    }

    BDBG_MSG_TRACE(("<<%s", __FUNCTION__));

    BDBG_Uninit();
    return res;
}

DFB_PlatformResult DFB_Platform_P_GetSharedSettings(DFB_PlatformSharedSetting setting, void* pValue)
{

    DFB_PlatformResult res = DFB_PLATFORM_UNSUPPORTED;

    if (!dfb_platform_initialised)
    {
        BDBG_ERR(("%s: DFB Platform not initialised. Please ensure DFB_Platform_Init() is called first.\n", __FUNCTION__));
        res = DFB_PLATFORM_INIT;
    }
    else if (setting < DFB_PlatformSharedSetting_eMax )
    {
        switch (setting)
        {
            case DFB_PlatformSharedSetting_eSurfacePool:
                *(unsigned int*)pValue = dfb_p_platform_shared_data->surfacePool;
                BDBG_MSG_TRACE(("%s(%d) Returning value 0x%x\n",__FUNCTION__,__LINE__,pValue));
                res = DFB_PLATFORM_OK;
                break;
            case DFB_PlatformSharedSetting_e3DGraphicsPool:
                *(unsigned int*)pValue = dfb_p_platform_shared_data->graphics3DPool;
                BDBG_MSG_TRACE(("%s(%d) Returning value 0x%x\n",__FUNCTION__,__LINE__,pValue));
                res = DFB_PLATFORM_OK;
                break;
            default:
                break;
        }
    }

    return res;
}

DFB_PlatformResult DFB_Platform_P_SetSharedSettings(DFB_PlatformSharedSetting setting, void* pValue)
{

    DFB_PlatformResult res = DFB_PLATFORM_UNSUPPORTED;

    /* Have to be master application to set data */
    if (!dfb_is_master)
        return DFB_PLATFORM_ACCESSDENIED;

    if (!dfb_platform_initialised)
    {
        BDBG_ERR(("%s: DFB Platform not initialised. Please ensure DFB_Platform_Init() is called first.\n", __FUNCTION__));
        res = DFB_PLATFORM_INIT;
    }
    else if (setting < DFB_PlatformSharedSetting_eMax )
    {
        switch (setting)
        {
            case DFB_PlatformSharedSetting_eSurfacePool:
                BDBG_MSG_TRACE(("%s(%d) Saving value 0x%x\n",__FUNCTION__,__LINE__,*(unsigned int*)pValue));
                dfb_p_platform_shared_data->surfacePool = *(unsigned int*)pValue;
                res = DFB_PLATFORM_OK;
                break;
            case DFB_PlatformSharedSetting_e3DGraphicsPool:
                BDBG_MSG_TRACE(("%s(%d) Saving value 0x%x\n",__FUNCTION__,__LINE__,*(unsigned int*)pValue));
                dfb_p_platform_shared_data->graphics3DPool = *(unsigned int*)pValue;
                res = DFB_PLATFORM_OK;
                break;
            default:
                break;
        }
    }

    return res;
}
