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
* $brcm_Workfile: nexus_platform.c $
* $brcm_Revision: SW7435-433/PROD_HUMAX_URSR/1 $
* $brcm_Date: 7/4/14 6:49p $
*
* API Description:
*   API name: Platform
*    Specific APIs to initialize the a board.
*
* Revision History:
*
* $brcm_Log: /nexus/platforms/common/src/nexus_platform.c $
*
* SW7435-433/PROD_HUMAX_URSR/1   7/4/14 6:49p scho
* SW7425-4944: Preserve wakeup status during S3 cold boot
*
* SW7435-433/1   12/4/12 4:10p mward
* SW7425-4254 : Re-enable dma standby
*
* 252   10/25/12 2:14p gmohile
* SW7435-453 : Add core standby
*
* 251   10/25/12 2:05p erickson
* SW7425-1792: fix NEXUS_BASE_ONLY
*
* 250   10/18/12 5:32p gmohile
* SW7425-3945 : Include bchp headers
*
* 249   10/16/12 1:33p erickson
* SW7435-375: transport and dma do not depend on security
*
* 248   10/16/12 10:28a erickson
* SW7425-1792: add core to module list, remove unused code
*
* 247   10/12/12 11:57a erickson
* SW7425-1792: don't use client on NEXUS_Platform_ReleaseObject
*
* 246   10/12/12 11:30a erickson
* SW7408-346: separate test for NEXUS_HAS_VIDEO_DECODER and DISPLAY
*
* 245   10/11/12 5:51p gmohile
* SW7425-3945 : Acquire BINT resource before enabling L1 interrupts
*
* 244   10/8/12 2:30p vsilyaev
* SW7425-1792: Allow application to control life-cycle of objects
*
* 243   9/28/12 11:20a erickson
* SW7435-362: stop scheduler threads before uninitializing modules. this
*  ensures no lock is held when it is destroyed.
*
* 242   9/28/12 8:45a erickson
* SW7435-344: convert spi/gpio handle to index in module settings. only
*  open spi/gpio when led/keypad is actually opened.
*
* 241   9/25/12 11:28a erickson
* SW7435-362: fix last checkin
*
* 240   9/24/12 11:57a erickson
* SW7435-362: lock module during init and uninit
*
* 239   9/21/12 1:40p randyjew
* SW7435-344:  Remove SPI Close Callbacks
*
* 238   9/18/12 2:24p gmohile
* SW7231-1000 : Always leave m2m dma clocks enabled
*
* 237   9/17/12 3:04p erickson
* SW7435-344: add NEXUS_OBJECT_REGISTER/UNREGISTER to syncthunk for
*  usermode server
*
* 236   9/17/12 10:47a erickson
* SW7435-344: improve recovery of partial init
*
* 235   9/5/12 6:08p jtna
* SW7425-3782: rework host and demod TSMF impl. both cases now go through
*  the host API
*
* 234   8/24/12 4:23p vishk
* SW7358-345: Merge changes to mainline. Change the channel indexing to
*  include spi core type.
*
* 233   8/1/12 4:00p erickson
* SW7425-2734: allow server to limit resources for untrusted clients
*
* 232   8/1/12 11:12a erickson
* SW7435-279: add NEXUS_PlatformSettings.graphics2DModuleSettings
*
* 231   7/27/12 2:14p jgarrett
* SW7425-3281: Merge to main branch
*
* 230   7/26/12 4:05p gmohile
* SW7425-3519,SW7425-3520,SW7425-3077: Add frontend standby
*
* 229   7/26/12 2:24p erickson
* SW7420-2328: do not use BERR_TRACE when nexus not initialized
*
* SW7425-3281/4   7/18/12 2:06p jgarrett
* SW7425-3281: Moving calls to WMDRM init routines before InitServer
*
* SW7425-3281/3   7/18/12 9:40a jgarrett
* SW7425-3281: Merging platform changes for 7231 release
*
* 228   7/6/12 2:04p erickson
* SW7420-2078: reorder NEXUS_Platform_P_UninitServer and
*  NEXUS_Platform_P_Shutdown
*
* 227   6/22/12 4:46p erickson
* SW7420-2078: move NEXUS_Platform_P_InitServer to correct place
*
* 226   6/22/12 11:49a erickson
* SW7420-2078: must uninit server after closing frontend
*
* 225   6/20/12 4:21p erickson
* SW7420-2078: remove acquire/release functions for frontend/i2c. must
*  initserver before opening frontend.
*
* SW7425-3281/2   7/8/12 4:55p piyushg
* SW7425-3281: Added support for playready licensce acquisition and Nexus
*  fileio
*
* SW7425-3281/1   6/20/12 6:07p piyushg
* SW7425-3281: Add "wmdrmpd", "wmdrmpd_core" and "wmdrmpd_io" modules to
*  nexus platform.
*
* 224   5/30/12 6:31p agin
* SWNOOS-540:  Fixed compiler error with SDE.
*
* 223   5/15/12 12:01p jtna
* SW7425-3013: default MTSIF to enabled and don't ignore app's request to
*  disable
*
* 222   5/9/12 3:35p erickson
* SW7231-748: fix #includes
*
* 221   5/9/12 3:19p erickson
* SW7231-748: verify platform lock held
*
* 220   4/27/12 10:58a gmohile
* SW7231-783 : Fix build error
*
* 219   4/20/12 3:34p gmohile
* SW7231-783 : Enable proper module locking for standby
*
* 218   3/27/12 4:55p erickson
* SW7425-2607: protect against pSettings = NULL, fix 65nm
*
* 217   3/27/12 4:20p erickson
* SW7425-2607: allow binary compatible platform api's
*
* 216   3/27/12 3:47p erickson
* SW7231-639: read registers instead of using BCHP_GetChipInfo and
*  BCHP_CHIP
*
* 215   3/19/12 6:29p erickson
* SW7231-639: fix NEXUS_PlatformStatus.chipRevision
*
* 214   3/14/12 11:54a erickson
* SW7401-4517: move BCHP_CHIP macro from API to implementation
*
* 213   3/12/12 4:41p gkamosa
* SW7425-2543: Merge support for 3383 SMS platform
*
* Centaurus_Phase_1.0/2   3/6/12 6:03p gkamosa
* SW7425-2337: Merge branch to tip
*
* 212   2/29/12 6:51p gmohile
* SW7425-2289 : Add cec standby
*
* 211   2/14/12 1:32p erickson
* SW7231-639: add NEXUS_PlatformStatus.familyId
*
* 210   2/1/12 10:38a erickson
* SW7425-2231: fix kernel mode procfs support
*
* 209   1/27/12 5:00p gmohile
* SW7425-1708 : Add gpio standby
*
* 208   1/24/12 3:50p vsilyaev
* SW7425-2112: Added missing connections
*
* 207   1/24/12 3:32p vsilyaev
* SW7425-2258: Added transport dependency
*
* 206   1/24/12 2:44p vsilyaev
* SW7425-2258: Added Nexus video decoder extension to decode MJPEG
*
* 205   1/16/12 6:06p gmohile
* SW7425-1473 : Merge raaga encoder support
*
* 204   1/11/12 5:23p gmohile
* SW7425-1708 : Init i2c module before hdmi
*
* Centaurus_Phase_1.0/1   2/6/12 6:17p jmiddelk
* SW7425-2337: Adding support for 93383sms
*
* 203   12/15/11 4:26p vsilyaev
* SW7425-1140: Merge NEXUS_Cec module support into main-line
*
* SW7425-1140/1   12/9/11 5:11p vle
* SW7425-1140: add NEXUS Cec module
*
* 202   12/2/11 6:32p jtna
* SW7425-1708: add i2c s3 standby power management
*
* 201   11/23/11 4:03p jtna
* SW7425-1708: add s3 power management for security
*
* 200   11/22/11 5:51p jtna
* SW7425-1708: add s3 power management for rfm
*
* 199   11/18/11 4:23p jtna
* SW7425-1708: add s3 power management for pwm
*
* 198   11/15/11 4:04p erickson
* SW7425-1747: remove old code
*
* 197   11/11/11 7:07p jtna
* SW7425-1708: added NEXUS_UartModule_Standby_priv
*
* 196   11/11/11 10:55a erickson
* SW7231-317: fix NEXUS_BASE_ONLY, clean up warnings, simplify
*  NEXUS_PlatformConfiguration if no inputs/outputs are defined
*
* 195   11/10/11 6:00p jtna
* SW7425-1709: move NEXUS_DmaModule_Standby_priv prototype to separate,
*  private header file
*
* 194   11/8/11 6:00p jtna
* SW7425-1709: make use of BMMD_Standby/Resume
*
* 193   11/1/11 2:51p erickson
* SW7231-391: fix naming, warnings
*
* 192   11/1/11 11:21a erickson
* SW7231-391: add macro to test if nexus class exists
*
* 191   10/31/11 7:49p bandrews
* SW7231-391: merge to main
*
* SW7420-2078/5   10/31/11 7:35p bandrews
* SW7231-391: added dependency from display to transport for timebase
*  calls
*
* 190   10/31/11 5:54p bandrews
* SW7420-2078: merge to main
*
* SW7420-2078/4   10/18/11 7:02p bandrews
* SW7231-391: remove mention of global_handle attribute
*
* SW7420-2078/3   10/11/11 8:23p bandrews
* SW7420-2078: must untune when releasing platform
*
* SW7420-2078/2   10/3/11 7:30p bandrews
* SW7420-2078: forgot to include class verification header
*
* SW7420-2078/1   9/30/11 7:32p bandrews
* SW7420-2078: added
*
* 189   10/24/11 9:47a erickson
* SW7420-1992: fix warning
*
* 188   10/20/11 1:56p erickson
* SW7420-1992: embed NEXUS_VideoWindowSettings in
*  NEXUS_SurfaceCompositorDisplaySettings
*
* 187   10/19/11 1:11p gmohile
* SW7420-2103 : Uhf module should ont be locks in active standby
*
* 184   9/14/11 2:31p erickson
* SW7405-5478: add MSG to identify each module as it uninits
*
* 183   9/7/11 4:04p gmohile
* SW7425-533 : Add VCE standby support
*
* 182   9/6/11 2:43p erickson
* SWNOOS-482: pass through file module settings in user mode
*
* 181   8/24/11 9:13a erickson
* SW7420-1993: add input_router
*
* 180   8/23/11 4:33p erickson
* SW7420-1992: rename to surface_compositor
*
* 179   8/12/11 3:23p gmohile
* SW7425-533 : add hdmi out standby
*
* 178   8/12/11 11:13a gmohile
* SW7125-1014 : Ir input should use the active scheduler
*
* 177   8/10/11 1:38p erickson
* SWSECURITY-28: add nsk2hdi module option
*
* 176   8/4/11 10:20a gmohile
* SW7425-692 : Add hdmi input standby
*
* 175   7/26/11 1:33p erickson
* SW7420-1992: add surface_compositor module and examples
*
* 174   7/21/11 5:02p gmohile
* SW7125-1014 : Fix graphics standby include file
*
* 173   7/21/11 4:14p gmohile
* SW7125-1014 : Rework power management
*
*
* 169   6/20/11 5:41p jgarrett
* SW7425-409: Merging VP6 support to main branch
*
* SW7425-409/2   6/13/11 7:21p jgarrett
* SW7425-409: Changing module startup order to handle video decoder being
*  dependent on audio
*
* SW7425-409/1   6/6/11 7:16p jgarrett
* SW7425-409: Adding DSP video decoder extensions for VDE
*
* 168   5/25/11 9:54a erickson
* SW7346-214: add security module settings to NEXUS_PlatformSettings
*
* 167   5/17/11 2:15p jhaberf
* SWDTV-6928: Merge of 35233 platform code to the main
*
* SWDTV-6928/1   5/4/11 6:25p mstefan
* SWDTV-6928: merge to main
*
* 164   4/16/11 2:14p nickh
* SW7425-313: Enable MTSIF for platforms that support it
*
* 163   4/14/11 4:05p hongtaoz
* SW7425-344, SW7125-769: clean uninit platform frontend; frontend uninit
*  is unconditional since power management might opened some resources
*  independent of frontend open or not;
*
* 162   4/13/11 11:30a hongtaoz
* SW7425-344: uninit frontend module only if it's opened at platform
*  init;
*
* 161   4/8/11 11:02a erickson
* SW7420-1671: always call NEXUS_Platform_P_InitServer
*
* 160   4/7/11 6:15p vsilyaev
* SW7425-232: Updated file mux module
*
* 159   4/7/11 5:25p erickson
* SW7420-1671: try to exit failed NEXUS_Platform_Init cleanly
*
* 158   4/7/11 1:56p gmohile
* SW7420-1773 : Resume nexus to full power before uninit
*
* 157   4/6/11 3:09p erickson
* SW7420-1671: try to exit failed NEXUS_Platform_Init cleanly
*
* 156   4/4/11 6:18p vsilyaev
* SW7405-5221: Added option to forward debug output to FIFO
*
* 155   4/1/11 4:23p jtna
* SW7346-119: add 40nm allpass support
*
* 154   3/16/11 10:27a erickson
* SW7420-1642: use NEXUS_Base_Settings.driverModuleInit instead of
*  NEXUS_Platform_P_InitDone
*
* 153   3/9/11 1:19p erickson
* SW7405-3282: reduce NEXUS_Platform_Init stack usage
*
* 152   3/8/11 9:15a jrubio
* SW7325-843: make sure pwm var is not defined unless NEXUS_HAS_PWM is
*  support, fixes NEXUS_BASE_ONLY
*
* 151   2/28/11 3:11p erickson
* SW7420-1123: refactor server init/uninit for simpler, cleaner app
*  init/uninit
*
* 150   2/28/11 2:37p erickson
* SW7420-1123: fix indenting & whitespace
*
* 149   2/28/11 12:32p erickson
* SW7335-1188: use NEXUS_HAS_<MODULE> for deciding to init/uninit a
*  module
*
* 148   2/25/11 2:52p gmohile
* SW7408-210 : Deprecate use of NEXUS_POWER_STANDBY
*
* 147   2/17/11 4:24p gmohile
* SW7408-210 : Merge PM 2.0 support to mainline
*
* 146   2/17/11 11:53a hongtaoz
* SW7425-9: add security support for hdmi_input;
*
* 146   2/17/11 11:51a hongtaoz
* SW7425-9: add security support for hdmi_input;
*
* 145   2/16/11 4:39p randyjew
* SW7344-25:Add 7344 spi frontpanel support using Aon gpios
*
* 144   1/21/11 11:33a mphillip
* SW7420-1403: Error out if Graphics3D fails to initialize
*
* 143   1/17/11 4:46p vsilyaev
* SW7425-65: Added StcChannel to the videoEncoder
*
* 142   1/4/11 1:53p erickson
* SW7422-2: improve NEXUS_Platform_Init error recovery
*
* 141   12/26/10 4:17p vsilyaev
* SW7425-40: Fixed shutdown sequence
*
* 140   12/20/10 4:29p erickson
* SW7422-2: rework nexus_platform_core, add board-specific file and API,
*  mmap after determining heaps
*
* 139   12/17/10 4:39p jtna
* SW7422-155: init picture decoder
*
* 138   12/13/10 7:02p vsilyaev
* SW7425-40: Added system initialization for the StreamMux and
*  VideoEncoder modules
*
* 137   12/7/10 5:47p erickson
* SW7420-1148: refactor kernel mode proxy for process isolation
*
* 136   12/2/10 7:41p spothana
* SW7420-1177: Adding nexus hdmi dvo module.
*
* SW7420-1177/2   11/5/10 3:23p spothana
* SW7420-1177: Update nexus hdmi dvo.
*
* SW7420-1177/1   11/1/10 8:36p spothana
* SW7420-1177: Adding nexus hdmi dvo module.
*
* 135   11/12/10 10:30a rgreen
* SW7422-10: Merge Changes
*
* SW7422-10/1   11/10/10 2:13p rgreen
* SW7422-10: Add nexus platform support for hdmiInput
*
* 134   10/26/10 11:33a jtna
* SW7420-887: added temp_monitor
*
* SW7420-887/2   10/21/10 7:23p spothana
* SW7420-887: Correct the coding conventions
*
* SW7420-887/1   10/20/10 7:50p spothana
* SW7420-887: Add temperature module.
*
* 133   10/13/10 1:37p vsilyaev
* SW7550-496: Initialiaze surface prior to the audio module
*
* 132   10/12/10 3:25p spothana
* SW7420-1157 : Match the VDC memory usage in the reference software to
*  memory worksheet
*
* 131   10/12/10 3:13p erickson
* SW7420-1135: add simple decoder
*
* 130   9/29/10 9:48a erickson
* SW7420-1123: consolidate IPC server header file for now
*
* 129   9/29/10 9:33a erickson
* SW7420-1123: add linux usermode IPC
*
* 128   9/24/10 3:31p jtna
* SW7420-887: added touchpad module
*
* 127   9/24/10 1:07p nickh
* SW7422-10: Add main and PIP memory settings for 7422
*
* 126   9/23/10 2:14p erickson
* SW7420-943: add NFE reverse jump wrappers
*
* 125   9/8/10 11:53a vsilyaev
* SW7468-129: Added video decoder on ZSP
*
* 124   8/26/10 4:23p jgarrett
* SW7408-52: Fixing up 7405 soft audio support
*
* 123   8/20/10 12:35p erickson
* SW7405-3832: add support for non-numeric NEXUS_PLATFORM defines.
*
* 122   8/5/10 8:32p VISHK
* SW7335-813: Add smartcard module settings to the Platform settings.
*
* 121   8/4/10 5:01p spothana
* SW7420-886: Add support for 97410 UMA mode using MEMC0 only.
*
* 120   8/4/10 10:55a VISHK
* SW7335-813: Add smartcard module settings to the Platform settings.
*
* 119   8/3/10 5:45p gskerl
* SW7125-237: Fixed compile error; moved NEXUS_Platform_JoinStandby()
*  from nexus_platform.c to nexus_platform_standby.c
*
* 118   8/3/10 2:46p gskerl
* SW7125-237: Fixed compile error; only declare
*  NEXUS_Platform_JoinStandby() if NEXUS_POWER_STANDBY is true
*
* 117   8/2/10 6:44p gskerl
* SW7125-237: Added NEXUS_PlatformJoinStandby API for multiprocess Nexus
*  in Active Standby mode
*
* 116   7/19/10 4:17p jgarrett
* SW7550-496: Adding surface module handle to audio initialization for
*  ZSP graphics.
*
* SW7550-496/1   7/16/10 5:50p nitinb
* SW7550-496: For ZSP graphics extension, surface module need to be
*  initialized before audio module
*
* 115   6/24/10 11:52a erickson
* SW7400-2806: add NEXUS_Platform_GetStatus
*
* 114   4/21/10 9:30a erickson
* SW7420-703: pass NEXUS_SurfaceModuleSettings through
*  NEXUS_PlatformSettings
*
* 113   4/19/10 3:44p nickh
* SW7420-709: Add 7420DBS support and remove BCHP_CHIP
*
* 112   4/14/10 6:40p jtna
* SW7125-322: Coverity Defect ID:21269 DEADCODE. videoDecoderThread is
*  not null if MULTITHREADED_INIT=1
*
* 111   4/9/10 4:47p katrep
* SW7405-4089:Fixed segfault in 32UMA mode
*
* 110   4/1/10 2:48p rjain
* SW7550-309: merging back to main
*
* SW7550-74/1   1/14/10 11:17a chengli
* SW7550-74 : smart card freq should be 24MHz for ARIB
*
* 109   3/19/10 10:12a erickson
* SW7405-3832: create standard method for specific
*  NEXUS_PLATFORM_VERSION. use tagged NEXUS_Platform_Init to enforce
*  binary compatibility. consolidate NEXUS_Platform_GetReleaseVersion
*  using standard macros.
*
* 108   2/4/10 5:37p jgarrett
* SW7408-57: Refactoring soft audio startup of TP1
*
* 107   2/4/10 3:02p gmohile
* SW7408-1 : Disable DMA L1 interrupt for 7408 during shutdown
*
* 106   1/21/10 6:35p garetht
* SW7420-553: Add platform for 93380vms
*
* 105   1/15/10 6:02p jgarrett
* SW7408-17: Adding backtrace support for TP1
*
* 104   12/30/09 2:53p nitinb
* SW7550-146: Correcting the conditions for
*  NEXUS_TRANSPORT_BEFORE_SECURITY
*
* 103   12/29/09 12:51p erickson
* SW7550-146: allow security module to be initialized after the transport
*  module if HW requires it
*
* 102   12/23/09 1:10p rjain
* SW7550-146: For 7550, first init transport module, then security
*
* 101   12/15/09 4:30p erickson
* SW7400-2629: add NEXUS_Platform_ReadRegister and
*  NEXUS_Platform_WriteRegister
*
* 100   12/1/09 5:37p katrep
* SW7335-627:user proper defines for conditional code for NEXUS_BASE_ONLY
*
* 99   11/30/09 3:13p jrubio
* SW7335-627: fixed for NEXUS_BASE_ONLY
*
* 98   11/18/09 1:31p gmohile
* SW7408-1 : Fix warning
*
* 97   11/12/09 3:37p gmohile
* SW7408-1 : Update 7408 support
*
* 96   11/12/09 2:26p gmohile
* SW7405-3395 : Fix use of NEXUS_FPGA_SUPPORT
*
* 95   11/6/09 9:50a erickson
* SW7405-3350: move surface module handle to NEXUS_PlatformHandles
*
* 94   11/6/09 9:48a erickson
* SWDEPRECATED-3881: fix code for platforms with < 3 displays
*
* 93   11/4/09 1:59p mward
* SWDEPRECATED-3881: Support 3 display operation.
*
* 92   10/22/09 2:00p erickson
* SW7405-3267: move NEXUS_KEYPAD_GPIO to nexus_platform_features.h
*
* 91   10/16/09 1:42p erickson
* SWDEPRECATED-3881: default NEXUS_PlatformConfiguration values based on
*  #defines
*
* 90   9/17/09 5:20p mward
* SW7400-2529: Coverity Defect ID:18807 DEADCODE audioThread is only used
*  if MULTITHREADED_INIT.
*
* 89   8/28/09 4:00p jgarrett
* SW7405-2928: Adding multichannel capture support
*
* 88   8/25/09 6:44p jrubio
* SW7340-10: fix GPIO setting for SPI Frontpanel
*
* 87   8/19/09 1:34p jrubio
* PR55882: add 7340/7342
*
* 86   8/5/09 4:39p katrep
* PR56569: add nexus platfrom get sw version api
*
* 85   8/5/09 12:04p katrep
* PR56569: platfrom sofftware release version
*
* 84   7/29/09 4:23p nickh
* PR56017: Add 7420B0 support for display heap settings
*
* 83   7/10/09 4:34p erickson
* PR56558: use NEXUS_DisplayModule_SetVideoDecoderModule to speed up init
*
* 82   7/9/09 10:00a erickson
* PR56558: no multi-threaded module init for linux kernel mode
*
* 81   7/2/09 10:46a erickson
* PR56558: multithreaded module initialization
*
* 80   7/2/09 10:43a erickson
* PR56558: make NEXUS_Platform_InitFrontend a public function, callable
*  after NEXUS_Platform_Init if NEXUS_PlatformSettings.openFrontend =
*  false.
*
* 79   6/23/09 11:59p mphillip
* PR54926: Add a hook to allow customization at the end of
*  NEXUS_Platform_Init
*
* 78   6/15/09 12:08a nickh
* PR55967: Fix compile error
*
* 77   6/12/09 10:51a erickson
* PR55967: convert NEXUS_DisplayModuleSettings heap settings to integers.
*  This makes them settable in the application.
*
* 76   6/5/09 7:29p rjlewis
* PR55287: missing initializer.
*
* 75   5/22/09 11:39a vishk
* PR 54842: rockford/unittests/nexus/general/boot should test
*  NEXUS_PlatformSettings options
*
* 74   5/22/09 11:33a vishk
* PR 54842: rockford/unittests/nexus/general/boot should test
*  NEXUS_PlatformSettings options
*
* 73   5/13/09 4:52p jtna
* PR54809: guard against unpaired init and uninits
*
* 72   5/12/09 4:33p erickson
* PR52109: added call to NEXUS_Platform_P_UninitInterrupts
*
* 71   5/12/09 4:18p erickson
* PR52109: move g_NEXUS_platformModule for more general use
*
* 70   5/11/09 4:02p jtna
* PR54515: make transport module handle available globally
*
* 69   4/27/09 6:18p jrubio
* PR44849: fix splash, add NEXUS_BASE_ONLY support
*
* 68   4/21/09 11:46a vishk
* PR 54381: Nexus smartcard module setting not configurable
*
* 67   4/20/09 3:22p vishk
* PR 54381: Nexus smartcard module setting not configurable
*
* 66   4/9/09 11:46a jrubio
* PR52188: add support for 2nd Receiver
*
* 65   4/8/09 4:31p vsilyaev
* PR 54016: Added support for precompiled header
*
* 64   3/30/09 10:36a erickson
* PR50748: support the secure heap using module settings handle
*
* 63   3/27/09 10:33a erickson
* PR48963: set 7420 heaps for main and pip
*
* 62   3/20/09 2:54p katrep
* PR53323: Fixed typo for NEXUS_HAS_IR_INPUT
*
* 61   3/19/09 4:59p erickson
* PR53323: add more #if so that more modules can be optionally removed
*
* 60   3/9/09 2:33p erickson
* PR50748: add multi-heap support
*
* 59   1/30/09 3:52p katrep
* PR50567: Disabled UHF input for 97466
*
* 58   1/28/09 6:27p vsilyaev
* PR 51392: Moved magnum initialization from Nexus base to the platform.
*  Initialize only magnum layer inside of
*  NEXUS_Platform_GetDefaultSettings
*
* 57   1/26/09 12:35p erickson
* PR51468: global variable naming convention
*
* 56   1/22/09 3:40p shyam
* PR50142 : Add Transcoder Uninit to the platform uninit function
*
* 55   12/30/08 2:34p jgarrett
* PR 50409: Adding DVB-CI for 97325
*
* 54   12/24/08 12:40p jgarrett
* PR 50703: Allowing build without security module
*
* 53   12/21/08 6:25p nickh
* PR50605: Fix compile errors when running without Security
*
* 52   12/19/08 11:07a erickson
* PR35457: give stronger ERR message when NEXUS_Platform_Init fails
*
* 51   12/17/08 1:48p katrep
* PR49875: Add support for 7466
*
* 50   12/3/08 3:06p erickson
* PR48963: remove stripeWidth setting. no longer needed for Graphics2D
*  init.
*
* 49   11/3/08 4:49p katrep
* PR48628: Add PWM to 7405 build
*
* 48   10/9/08 10:15a erickson
* PR47328: moved NEXUS_Platform_GetStreamerInputBand to frontend.c
*
* 47   10/3/08 11:41a shyam
* PR 40732 : Remove nexus_platform.c warnings
*
* 46   9/24/08 6:58p shyam
* PR40732 : First working transcoder DMA version
*
* 45   9/23/08 9:16a jgarrett
* PR 46513: Merging 97459 support to mainline
*
* 44   9/8/08 7:34p shyam
* PR40732 : Reorganize 97043 platform specific into seperate files
* PR40732 : Add PCI interrupt support
*
* 43   9/2/08 10:27p katrep
* PR46457: Configure nexus features at the run time based on board strap
*  options
*
* 42   9/2/08 4:45p erickson
* PR46457: added NEXUS_Platform_GetSettings
*
* 41   8/22/08 7:23p erickson
* PR45795: remove maxOutputFormat from config
*
* 40   8/5/08 9:06a erickson
* PR45422: transport requires security for DivX DRM
*
* 39   7/31/08 5:19p vsilyaev
* PR 45137: Initial support for multi application mode
*
* 38   7/23/08 2:16p erickson
* PR44598: NEXUS_Platform_GetDefaultSettings must call NEXUS_Base_Init so
*  that GetDefaultSettings can use Magnum
*
* 37   7/22/08 3:03p erickson
* PR44598: move some module settings into NEXUS_PlatformSettings to allow
*  app to customize init.
*
* 36   7/18/08 10:21a erickson
* PR44919: move VideoDecoder heap customization into
*  nexus_platform_features.h
*
* PR44830/2   9/17/08 4:41p anilmm
* PR44830:  Replaced BCM_BOARD macro
*
* PR44830/1   7/18/08 11:05a anilmm
* PR44830: Add support for 97459
*
* 35   7/11/08 5:10p jtna
* PR42924: Add channel change measurements functionality
*
* 34   7/11/08 12:45p mphillip
* PR40869: Initialize Graphics3D module
*
* 33   7/7/08 10:20a jrubio
* PR42353: change secure heap size for 7325/7335 same as 7405
*
* 32   7/3/08 4:50p erickson
* PR44586: NEXUS_DmaModule_Init must require the security module handle
*
* 31   7/3/08 1:14p mphillip
* PR40869: 97400 decoder heap values
*
* 30   7/2/08 10:23a vsilyaev
* PR 44452: Added file to keep NEXUS_Platform_GetDefaultSettings
*
* 29   5/16/08 7:40p vishk
* PR 40034: Develop Nexus Smartcard Interface/Module.
* PR 40016: bsettop_smartcard: Develop SettopAPI-to-Nexus shim layer
*
* 28   5/5/08 4:55p jrubio
* PR42439: you must calculate your heap sizes for video decoder, each
*  platform is different
*
* 27   5/2/08 2:02p jrubio
* PR42353: update features for 7335
*
* 26   5/1/08 3:31p jrubio
* PR42353: add 7335 support
*
* 25   4/29/08 6:01p jrubio
* PR42336: add 7325 conditional for vdc heap allocation
*
* 24   4/24/08 12:59p erickson
* PR42113: add InputCapture
*
* 23   4/18/08 4:55p vsilyaev
* PR 41868: Added security API to playpump and recpump
*
* 22   4/11/08 12:36p erickson
* PR41695: added Graphics2D stripeWidth setting
*
* 21   4/8/08 10:40a vsilyaev
* PR 41083: Added bprofile support
*
* 20   4/2/08 5:51p shyam
* PR39899: Add 7043 Transcoder support
*
* 19   3/27/08 8:25p bandrews
* PR40090: corrected astm display module to video decoder module
*
* 18   3/27/08 2:43p vsilyaev
* PR 40818: Added settings to allocate shared heap
*
* 17   3/25/08 7:41p vsilyaev
* PR 40862: Added supported video output formats
*
* 16   3/24/08 10:27p bandrews
* PR40090: astm integration
*
* 15   3/24/08 9:59a erickson
* PR40788: don't fail Platform Init if frontend init fails
*
* 13   3/4/08 5:43p jrubio
* PR40169: add n 7325
*
* 12   3/4/08 4:30p jgarrett
* PR 39405: Adding RFM audio support
*
* PR39899/3   3/14/08 6:19p shyam
* PR 39899 : Fix compiler warnings
*
* PR39899/2   3/12/08 6:21p shyam
* PR39899 : Merge in good nodes from main
*
* PR39899/1   2/27/08 5:53p shyam
* PR 39899 : Integrate 7043 XCD into Nexus
*
* 11   2/26/08 5:42p jgarrett
* PR 39017: Adding security handle to hdmi
*
* 10   2/20/08 1:45p erickson
* PR39405: added RFM
*
* 9   2/7/08 11:02a jgarrett
* PR 39270: Specifying per-chip heap sizes
*
* 8   2/5/08 2:30p jgarrett
* PR 39017: Adding HdmiOutput to Audio connection
*
* 7   2/4/08 11:42a erickson
* PR36808: added SyncChannel
*
* 6   2/1/08 5:34p jgarrett
* PR 39017: Adding HdmiOutput
*
* 6   2/1/08 5:25p jgarrett
* PR 39017: Adding HdmiOutput
*
* 5   1/30/08 4:25p vsilyaev
* PR 38682: Fixed warnings in release build
*
* 4   1/24/08 4:38p vsilyaev
* PR 38682: Improved kernel/proxy environment
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
* Nexus_Devel/38   1/16/08 3:26p vsilyaev
* PR 38682: Fixed condition initialization of PVR modules
*
* Nexus_Devel/37   12/18/07 1:49p jgarrett
* PR 37837: Fixing DMA shutdown
*
* Nexus_Devel/36   12/7/07 11:54a jgarrett
* PR 37837: Removing security-dma linkage
*
* Nexus_Devel/35   12/6/07 11:37a jgarrett
* PR 37931: Making UART conditional
*
* Nexus_Devel/34   12/3/07 9:28p jgarrett
* PR 37837: Adding DMA
*
* Nexus_Devel/33   11/29/07 2:54p erickson
* PR35457: pass transport handle in Audio module Settings
*
* Nexus_Devel/32   11/21/07 11:05a erickson
* PR37423: add io modules
*
* Nexus_Devel/31   11/15/07 5:28p erickson
* PR37137: added security module
*
* Nexus_Devel/30   11/15/07 12:34p rrlee
* PR37125: Add calls to UninitCore and UninitOS in shutdown
*
* Nexus_Devel/29   11/8/07 6:30p jgarrett
* PR 36774: Setting streamer input band to 5 for 7405
*
* Nexus_Devel/28   11/8/07 2:47p vsilyaev
* PR 36788: PVR record module
*
* Nexus_Devel/27   11/8/07 9:17a erickson
* PR36802: use standard NEXUS_HAS_MODULE defines, impl Platform_Uninit
*
* Nexus_Devel/26   11/5/07 2:12p erickson
* PR36725: add NEXUS_PlatformConfiguration, share more code with 97401
*
* Nexus_Devel/25   10/19/07 9:39a erickson
* PR36275: added graphics2d
*
* Nexus_Devel/24   10/18/07 10:39a erickson
* PR36152: added NEXUS_Platform_GetStreamerInputBand
*
* Nexus_Devel/23   10/15/07 5:11p vsilyaev
* PR 35824: Added playback module
*
* Nexus_Devel/22   10/10/07 5:14p vsilyaev
* PR 35824: Really added file module
*
* Nexus_Devel/21   10/10/07 5:11p vsilyaev
* PR 35824: Added file module
*
* Nexus_Devel/20   10/10/07 11:34a jgarrett
* PR 35551: Adding platform handles
*
* Nexus_Devel/19   10/8/07 10:24a erickson
* PR35551: adding Uninit
*
* Nexus_Devel/18   10/5/07 5:51p jgarrett
* PR 35551: Adding frontend
*
* Nexus_Devel/17   10/5/07 1:42p jgarrett
* PR 35744: Adding i2c module
*
* Nexus_Devel/16   9/28/07 4:24p jgarrett
* PR 35002: Adding debug
*
* Nexus_Devel/15   9/28/07 11:57a vsilyaev
* PR 34662: Fixed API after review
*
* Nexus_Devel/14   9/28/07 11:32a erickson
* PR34926: rename videodecoder to video_decoder
*
* Nexus_Devel/13   9/28/07 11:06a erickson
* PR34926: move transport handle to videoDecoderSettings.transport
*
* Nexus_Devel/12   9/28/07 10:05a erickson
* PR34925: updated module settings structure names
*
* Nexus_Devel/11   9/27/07 7:51p jgarrett
* PR 34954: Adding audio
*
* Nexus_Devel/10   9/26/07 5:36p vsilyaev
* PR 34662: Added platform configuration
*
* Nexus_Devel/9   9/26/07 1:45p vsilyaev
* PR 34662: Implementation of display framebuffer
*
* Nexus_Devel/8   9/25/07 1:31p vsilyaev
* PR 34662: Added display module
*
* Nexus_Devel/7   9/25/07 12:31p erickson
* PR34926: fix Unint
*
* Nexus_Devel/6   9/25/07 12:27p erickson
* PR34926: added VideoDecoder
*
* Nexus_Devel/5   9/25/07 9:55a erickson
* PR34925: return 0 on successful Init
*
* Nexus_Devel/4   9/24/07 3:21p erickson
* PR34925: added Transport init
*
* Nexus_Devel/3   9/24/07 1:00p jgarrett
* PR 35002: Migrating core code for better 97401/97400 sharing
*
* Nexus_Devel/2   9/21/07 6:26p jgarrett
* PR 35002: Successful 97400 build
*
* Nexus_Devel/1   9/19/07 6:32p jgarrett
* PR 35002: Adding initial 97400 platform
*
***************************************************************************/

#include "nexus_base.h"
#include "nexus_platform_priv.h"
#include "b_objdb.h"
#include "nexus_power_management.h"
#ifndef NEXUS_BASE_ONLY
#include "nexus_class_verification.h"
#endif

#ifdef NEXUS_FPGA_SUPPORT
#include "nexus_platform_fpga.h"
#endif
#if NEXUS_HAS_I2C
#include "nexus_i2c_init.h"
#include "priv/nexus_i2c_standby_priv.h"
#endif
#if NEXUS_HAS_TRANSPORT
#include "nexus_transport_init.h"
#include "nexus_input_band.h"
#include "priv/nexus_transport_standby_priv.h"
#endif
#if NEXUS_HAS_VIDEO_DECODER
#include "nexus_video_decoder_init.h"
#include "priv/nexus_video_decoder_standby_priv.h"
#endif
#if NEXUS_HAS_DISPLAY
#include "nexus_display_init.h"
#include "nexus_video_input.h"
#include "nexus_video_output.h"
#include "priv/nexus_display_standby_priv.h"
#endif
#if NEXUS_HAS_SURFACE
#include "nexus_surface_init.h"
#endif
#if NEXUS_HAS_GRAPHICS2D
#include "nexus_graphics2d_init.h"
#include "priv/nexus_graphics2d_standby_priv.h"
#endif
#ifdef NEXUS_HAS_GRAPHICS3D
#include "nexus_graphics3d_init.h"
#endif
#if NEXUS_HAS_FRONTEND
#include "nexus_frontend_init.h"
#endif
#if NEXUS_HAS_SECURITY
#include "nexus_security_init.h"
#include "priv/nexus_security_standby_priv.h"
#endif
#if NEXUS_HAS_AUDIO
#include "nexus_audio_init.h"
#include "nexus_audio_input.h"
#include "nexus_audio_output.h"
#include "priv/nexus_audio_standby_priv.h"
#endif
#if NEXUS_HAS_FILE
#include "nexus_file_init.h"
#endif
#if NEXUS_HAS_PLAYBACK
#include "nexus_playback_init.h"
#endif
#if NEXUS_HAS_RECORD
#include "nexus_record_init.h"
#endif
#if NEXUS_HAS_DVB_CI
#include "nexus_dvb_ci_init.h"
#endif
#if NEXUS_HAS_PCMCIA
#include "nexus_pcmcia_init.h"
#endif
#if NEXUS_HAS_IR_INPUT
#include "nexus_ir_input_init.h"
#endif
#if NEXUS_HAS_LED
#include "nexus_led_init.h"
#endif
#if NEXUS_HAS_KEYPAD
#include "nexus_keypad_init.h"
#endif
#if NEXUS_HAS_IR_BLASTER
#include "nexus_ir_blaster_init.h"
#endif
#if NEXUS_HAS_INPUT_CAPTURE
#include "nexus_input_capture_init.h"
#endif
#if NEXUS_HAS_UHF_INPUT
#include "nexus_uhf_input_init.h"
#include "priv/nexus_uhf_input_standby_priv.h"
#endif
#if NEXUS_HAS_GPIO
#include "nexus_gpio_init.h"
#include "priv/nexus_gpio_standby_priv.h"
#endif
#if NEXUS_HAS_SPI
#include "nexus_spi_init.h"
#endif
#if NEXUS_HAS_SPI_FRONTPANEL
#include "nexus_spi.h"
#include "nexus_gpio.h"
#endif
#if NEXUS_HAS_UART
#include "nexus_uart_init.h"
#include "priv/nexus_uart_standby_priv.h"
#endif
#if NEXUS_HAS_SMARTCARD
#include "nexus_smartcard_init.h"
#include "priv/nexus_smartcard_standby_priv.h"
#endif
#if NEXUS_HAS_DMA
#include "nexus_dma_init.h"
#include "priv/nexus_dma_standby_priv.h"
#endif
#if NEXUS_HAS_HDMI_OUTPUT
#include "nexus_hdmi_output_init.h"
#include "priv/nexus_hdmi_output_standby_priv.h"
#endif
#if NEXUS_HAS_CEC
#include "nexus_cec_init.h"
#include "priv/nexus_cec_standby_priv.h"
#endif
#if NEXUS_HAS_HDMI_DVO
#include "nexus_hdmi_dvo_init.h"
#endif
#if NEXUS_HAS_HDMI_INPUT
#include "nexus_hdmi_input_init.h"
#include "priv/nexus_hdmi_input_standby_priv.h"
#endif
#if NEXUS_HAS_PICTURE_DECODER
#include "nexus_picture_decoder_init.h"
#include "priv/nexus_picture_decoder_standby_priv.h"
#endif
#if NEXUS_HAS_SYNC_CHANNEL
#include "nexus_sync_channel_init.h"
#endif
#if NEXUS_HAS_ASTM
#include "nexus_astm_init.h"
#endif
#if NEXUS_HAS_RFM
#include "nexus_rfm_init.h"
#include "priv/nexus_rfm_standby_priv.h"
#endif
#include "nexus_core_utils.h"
#if NEXUS_HAS_PWM
#include "nexus_pwm_init.h"
#include "nexus_pwm.h"
#include "priv/nexus_pwm_standby_priv.h"
#endif
#ifdef NEXUS_HAS_TOUCHPAD
#include "nexus_touchpad_init.h"
#endif
#if NEXUS_HAS_SIMPLE_DECODER
#include "nexus_simple_decoder_init.h"
#endif
#if NEXUS_HAS_SURFACE_COMPOSITOR
#include "nexus_surface_compositor_init.h"
#endif
#if NEXUS_HAS_INPUT_ROUTER
#include "nexus_input_router_init.h"
#endif
#if NEXUS_HAS_TEMP_MONITOR
#include "nexus_temp_monitor_init.h"
#include "priv/nexus_temp_monitor_standby_priv.h"
#endif
#if NEXUS_HAS_VIDEO_ENCODER
#include "nexus_video_encoder_init.h"
#include "priv/nexus_video_encoder_standby_priv.h"
#endif
#if NEXUS_HAS_STREAM_MUX
#include "nexus_stream_mux_init.h"
#endif
#if NEXUS_HAS_FILE_MUX
#include "nexus_file_mux_init.h"
#endif
#if NEXUS_HAS_NSK2HDI
#include "nexus_nsk2hdi_init.h"
#endif
#if NEXUS_BASE_OS_linuxkernel
#include "nexus_generic_driver_impl.h"
#endif
#if NEXUS_HAS_WMDRMPD
#include "nexus_wmdrmpd_init.h"
#endif
#if NEXUS_HAS_WMDRMPD_CORE
#include "nexus_wmdrmpd_core_init.h"
#endif
#if NEXUS_HAS_WMDRMPD_IO
#include "nexus_wmdrmpd_io_init.h"
#endif

#include "priv/nexus_core_standby_priv.h"

#if NEXUS_POWER_MANAGEMENT
#include "bchp_pwr.h"
#include "bchp_pwr_resources.h"
#endif

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_TEMP_IR4000HD)
#include "bchp_clkgen.h"
#include "bchp_gio.h"
#endif

BDBG_MODULE(nexus_platform);

NEXUS_PlatformHandles g_NEXUS_platformHandles;
NEXUS_PlatformSettings g_NEXUS_platformSettings;   /* saved platfrom settings */
NEXUS_ModuleHandle g_NEXUS_platformModule = NULL;
static NEXUS_HeapHandle g_secureHeap = NULL;
static char g_platformSwVersion[100]; /* keep off the static to prevent blowout */

#ifdef NEXUS_BASE_OS_linuxuser
/* We can speed up init slightly by loading FW from multiple threads. */
#define MULTITHREADED_INIT 1
#endif

static NEXUS_Platform_P_ModuleInfo * NEXUS_Platform_P_AddModule(NEXUS_ModuleHandle module_handle,
                                NEXUS_PlatformStandbyLockMode lock_mode,
                                void (*uninit)(void),
                                NEXUS_Error (*standby)(bool, const NEXUS_StandbySettings *))
{
    NEXUS_Platform_P_ModuleInfo *module_info;

    if(!module_handle)
        return NULL;

    module_info = BKNI_Malloc(sizeof(*module_info));
    if(!module_info)
        return NULL;

    BKNI_Memset(module_info, 0, sizeof(*module_info));
    module_info->module = module_handle;
    module_info->lock_mode = lock_mode;
    module_info->locked = false;
    module_info->powerdown = false;
    module_info->uninit = uninit;
    module_info->standby = standby;
    BLST_Q_INSERT_HEAD(&g_NEXUS_platformHandles.handles, module_info, link);

    return module_info;
}

static void NEXUS_Platform_P_UninitModules(void)
{
    NEXUS_Platform_P_ModuleInfo *module_info;

    while(NULL != (module_info=BLST_Q_FIRST(&g_NEXUS_platformHandles.handles))) {
        BDBG_MSG(("uninit %s", NEXUS_Module_GetName(module_info->module)));
        if( module_info->uninit ) {
            module_info->uninit();
        }
        else if (module_info->module != g_NEXUS_platformHandles.core) {
            BDBG_WRN(("Uninit function not provided for module %s", NEXUS_Module_GetName(module_info->module)));
        }
        BLST_Q_REMOVE_HEAD(&g_NEXUS_platformHandles.handles, link);
        BKNI_Free(module_info);
    }
}

#if NEXUS_HAS_VIDEO_DECODER
static void NEXUS_Platform_P_InitVideoDecoder(void *context)
{
    NEXUS_VideoDecoderModuleSettings videoDecoderSettings;
    BSTD_UNUSED(context);
    BDBG_MSG((">VIDEO_DECODER"));
    videoDecoderSettings = g_NEXUS_platformSettings.videoDecoderModuleSettings;
    BDBG_ASSERT(g_NEXUS_platformHandles.transport);
    videoDecoderSettings.transport = g_NEXUS_platformHandles.transport;
    videoDecoderSettings.secureHeap = g_secureHeap;
#if NEXUS_NUM_ZSP_VIDEO_DECODERS || NEXUS_NUM_DSP_VIDEO_DECODERS
    BDBG_ASSERT(g_NEXUS_platformHandles.audio);
    videoDecoderSettings.audio = g_NEXUS_platformHandles.audio;
#endif
#if NEXUS_NUM_SID_VIDEO_DECODERS
    BDBG_ASSERT(g_NEXUS_platformHandles.pictureDecoder);
    videoDecoderSettings.pictureDecoder = g_NEXUS_platformHandles.pictureDecoder;
#endif
    g_NEXUS_platformHandles.videoDecoder = NEXUS_VideoDecoderModule_Init(&videoDecoderSettings);
    if (!g_NEXUS_platformHandles.videoDecoder) {
        BDBG_ERR(("Unable to init VideoDecoder"));
    }
}
#endif

#if NEXUS_HAS_AUDIO
#if NEXUS_HAS_SOFT_AUDIO
static uint32_t NEXUS_Platform_P_AudioReadReserved(
    void *pUnused1,
    int unused2,
    uint32_t physicalAddress)
{
    BSTD_UNUSED(pUnused1);
    BSTD_UNUSED(unused2);
    return NEXUS_Platform_P_ReadReserved(physicalAddress);
}
static void NEXUS_Platform_P_AudioWriteReserved(
    void *pUnused1,
    int unused2,
    uint32_t physicalAddress,
    uint32_t value)
{
    BSTD_UNUSED(pUnused1);
    BSTD_UNUSED(unused2);
    NEXUS_Platform_P_WriteReserved(physicalAddress, value);
}
static uint32_t NEXUS_Platform_P_AudioReadCoreReg(
    void *pUnused1,
    int unused2,
    uint32_t physicalAddress)
{
    BSTD_UNUSED(pUnused1);
    BSTD_UNUSED(unused2);
    return NEXUS_Platform_P_ReadCoreReg(physicalAddress);
}
static void NEXUS_Platform_P_AudioWriteCoreReg(
    void *pUnused1,
    int unused2,
    uint32_t physicalAddress,
    uint32_t value)
{
    BSTD_UNUSED(pUnused1);
    BSTD_UNUSED(unused2);
    NEXUS_Platform_P_WriteCoreReg(physicalAddress, value);
}
static uint32_t NEXUS_Platform_P_AudioReadCmtControl(
    void *pUnused1,
    int unused2)
{
    BSTD_UNUSED(pUnused1);
    BSTD_UNUSED(unused2);
    return NEXUS_Platform_P_ReadCmtControl();
}
static void NEXUS_Platform_P_AudioWriteCmtControl(
    void *pUnused1,
    int unused2,
    uint32_t value)
{
    BSTD_UNUSED(pUnused1);
    BSTD_UNUSED(unused2);
    NEXUS_Platform_P_WriteCmtControl(value);
}
#endif

void NEXUS_Platform_P_InitAudio(void *context)
{
    NEXUS_AudioModuleSettings audioSettings;

    BSTD_UNUSED(context);
    BDBG_MSG((">AUDIO"));
    audioSettings = g_NEXUS_platformSettings.audioModuleSettings;
    BDBG_ASSERT(g_NEXUS_platformHandles.transport);
    audioSettings.modules.transport = g_NEXUS_platformHandles.transport;
    BDBG_ASSERT(g_NEXUS_platformHandles.surface);
    audioSettings.modules.surface = g_NEXUS_platformHandles.surface;
    #if NEXUS_HAS_HDMI_OUTPUT
    BDBG_ASSERT(g_NEXUS_platformHandles.hdmiOutput);
    audioSettings.modules.hdmiOutput = g_NEXUS_platformHandles.hdmiOutput;
    #endif
    #if NEXUS_HAS_HDMI_INPUT
    BDBG_ASSERT(g_NEXUS_platformHandles.hdmiInput);
    audioSettings.modules.hdmiInput = g_NEXUS_platformHandles.hdmiInput;
    #endif
    #if NEXUS_HAS_RFM
    BDBG_ASSERT(g_NEXUS_platformHandles.rfm);
    audioSettings.modules.rfm = g_NEXUS_platformHandles.rfm;
    #endif
    #if NEXUS_HAS_SOFT_AUDIO
    audioSettings.readReserved = NEXUS_Platform_P_AudioReadReserved;
    audioSettings.writeReserved = NEXUS_Platform_P_AudioWriteReserved;
    audioSettings.readCoreRegister = NEXUS_Platform_P_AudioReadCoreReg;
    audioSettings.writeCoreRegister = NEXUS_Platform_P_AudioWriteCoreReg;
    audioSettings.readCmtControl = NEXUS_Platform_P_AudioReadCmtControl;
    audioSettings.writeCmtControl = NEXUS_Platform_P_AudioWriteCmtControl;
    #endif
    /* this is something used by the DTV... for now ignore the "DTV centric guarding schema" */
    #if NEXUS_HAS_FRONTEND
        audioSettings.modules.frontend = g_NEXUS_platformHandles.frontend;
    #endif

#if defined(HUMAX_PLATFORM_BASE)
	/*
	solution of MS11 test failed
		1) Dialog normalization test
		2) Loudness matching test.
	also see volume control in P_AUDIO_SetVolume()
	*/
	audioSettings.loudnessMode = NEXUS_AudioLoudnessEquivalenceMode_eEbuR128;
#endif

    g_NEXUS_platformHandles.audio = NEXUS_AudioModule_Init(&audioSettings);
    if ( !g_NEXUS_platformHandles.audio )
    {
        BDBG_ERR(("Unable to init audio"));
    } else {
        NEXUS_Platform_P_AddModule(g_NEXUS_platformHandles.audio, NEXUS_PlatformStandbyLockMode_eAlways, NEXUS_AudioModule_Uninit, NEXUS_AudioModule_Standby_priv);
    }
}
#endif


#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_TEMP_IR4000HD)
/* IR4000HD W/A  for ethernet link down issue between initial HW and some routers 2015.07.13*/
#define HW_REVISION_MASK 0x3800000
#define HW_REVISION_SHIFT 23
#define HW_REVISION_1_0  0  /* board revision 1.0 */
#define HW_REVISION_0_6  2  /* board revision 0.6 */
void NEXUS_Platform_P_ClkPllModify(void)
{
	uint32_t hwrevision = 0;
	uint32_t regDataWrite = 0;

	hwrevision = BREG_Read32(g_pCoreHandles->reg, BCHP_GIO_DATA_LO);
	hwrevision = (hwrevision &  HW_REVISION_MASK)>>HW_REVISION_SHIFT;
	BDBG_ERR(("[IR4000HD] HW checked. %x", hwrevision));

	if(hwrevision == HW_REVISION_1_0 || hwrevision == HW_REVISION_0_6)
	{
		BDBG_ERR(("[IR4000HD] Ethernet link down problem W/A by adjusting PLL."));
		BDBG_ERR(("[IR4000HD] Board Revision %s.", (hwrevision == HW_REVISION_1_0)? "1.0":"0.6"));

		regDataWrite = BREG_Read32(g_pCoreHandles->reg, BCHP_CLKGEN_PLL_SYS1_PLL_GAIN);
		regDataWrite &= ~(BCHP_MASK(CLKGEN_PLL_SYS1_PLL_GAIN, LOOP_GAIN_PROPORTIONAL_IN_PHASE));
		regDataWrite &= ~(BCHP_MASK(CLKGEN_PLL_SYS1_PLL_GAIN, LOOP_GAIN_INTEGRATOR_IN_PHASE));

		regDataWrite |= BCHP_FIELD_DATA(CLKGEN_PLL_SYS1_PLL_GAIN, LOOP_GAIN_PROPORTIONAL_IN_PHASE, 0xc);
		regDataWrite |= BCHP_FIELD_DATA(CLKGEN_PLL_SYS1_PLL_GAIN, LOOP_GAIN_INTEGRATOR_IN_PHASE, 0x04);
		BREG_Write32(g_pCoreHandles->reg, BCHP_CLKGEN_PLL_SYS1_PLL_GAIN, regDataWrite);
	}
}
#endif

/***************************************************************************
Summary:
    Initialize Nexus
Description:
    This will initialize all board-specifics and then proceed to
    initialize the nexus modules above it.  This is the main entry point
    for all applications to start Nexus.
See Also:
    NEXUS_Platform_Uninit
 ***************************************************************************/
NEXUS_Error NEXUS_Platform_Init_tagged( const NEXUS_PlatformSettings *pSettings, unsigned platformCheck, unsigned versionCheck, unsigned structSizeCheck )
{
    NEXUS_Error errCode;
    NEXUS_Base_Settings baseSettings;
    NEXUS_ThreadHandle videoDecoderThread = NULL;
#if MULTITHREADED_INIT
    NEXUS_ThreadHandle audioThread = NULL;
#endif
#if NEXUS_HAS_TRANSPORT
    NEXUS_TransportModuleSettings transportSettings;
#endif
#if NEXUS_HAS_FRONTEND
    NEXUS_FrontendModuleSettings frontendSettings;
#endif
#if NEXUS_HAS_PLAYBACK
    NEXUS_PlaybackModuleSettings playbackSettings;
#endif
#if NEXUS_HAS_RECORD
    NEXUS_RecordModuleSettings recordSettings;
#endif
#if NEXUS_HAS_GRAPHICS2D
    NEXUS_Graphics2DModuleSettings graphics2DSettings;
#endif
#if NEXUS_HAS_DMA
    NEXUS_DmaModuleSettings dmaSettings;
#endif
#if NEXUS_HAS_HDMI_OUTPUT
    NEXUS_HdmiOutputModuleSettings hdmiSettings;
#endif
#if NEXUS_HAS_HDMI_INPUT
    NEXUS_HdmiInputModuleSettings hdmiInputSettings;
#endif
#if NEXUS_HAS_CEC
    NEXUS_CecModuleSettings cecSettings;
#endif
#if NEXUS_HAS_PICTURE_DECODER
    NEXUS_PictureDecoderModuleSettings pictureDecoderSettings;
#endif
#if NEXUS_HAS_SYNC_CHANNEL
    NEXUS_SyncChannelModuleSettings syncChannelSettings;
#endif
#if NEXUS_HAS_ASTM
    NEXUS_AstmModuleSettings astmSettings;
#endif
#if NEXUS_HAS_DVB_CI
    NEXUS_DvbCiModuleSettings dvbCiSettings;
#endif
#if NEXUS_HAS_PCMCIA
    NEXUS_PcmCiaModuleSettings pcmCiaSettings;
#endif
#if NEXUS_HAS_LED
    NEXUS_LedModuleSettings ledSettings;
#endif
#if NEXUS_HAS_KEYPAD
    NEXUS_KeypadModuleSettings keypadSettings;
#endif
#if NEXUS_HAS_SMARTCARD
    NEXUS_SmartcardModuleSettings smartCardSettings;
#endif
#if NEXUS_HAS_STREAM_MUX
    NEXUS_StreamMuxModuleSettings streamMuxModuleSettings;
#endif
#if NEXUS_HAS_NSK2HDI
    NEXUS_Nsk2hdiModuleSettings nsk2hdiSettings;
#endif
#if NEXUS_HAS_WMDRMPD
    NEXUS_ModuleHandle wmdrmpd;
    NEXUS_WmDrmPdModuleSettings wmdrmpdSettings;
#endif
#if NEXUS_HAS_WMDRMPD_CORE
    NEXUS_ModuleHandle wmdrmpdCore;
    NEXUS_WmDrmPdCoreModuleSettings wmdrmpdcoreSettings;
#endif
#if NEXUS_HAS_WMDRMPD_IO
    NEXUS_ModuleHandle wmdrmpdIo;
#endif

    NEXUS_ModuleHandle handle;
    unsigned i;

    BSTD_UNUSED(handle); /* in case it is unused */
    NEXUS_Platform_GetReleaseVersion(g_platformSwVersion,100);
    BDBG_WRN(("Release %s",g_platformSwVersion));

#if !NEXUS_PLATFORM_NON_NUMERIC
    if ((!pSettings || pSettings->checkPlatformType) && platformCheck != NEXUS_PLATFORM) {
        /* This code also ensures that NEXUS_PLATFORM is numeric, not alpha-numeric */
        BDBG_ERR(("NEXUS_Platform_Init failed with platform mismatch (nexus=%d, caller=%d). The application must be recompiled for this platform.",
            NEXUS_PLATFORM, platformCheck));
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }
#else
    BSTD_UNUSED(platformCheck);
#endif

    if ((!pSettings || pSettings->checkPlatformType) && versionCheck != NEXUS_P_GET_VERSION(NEXUS_PLATFORM)) {
        BDBG_ERR(("NEXUS_Platform_Init failed with version mismatch (nexus=%d.%d, caller=%d.%d). Please recompile application and/or nexus.",
            NEXUS_P_GET_VERSION(NEXUS_PLATFORM) / NEXUS_PLATFORM_VERSION_UNITS,
            NEXUS_P_GET_VERSION(NEXUS_PLATFORM) % NEXUS_PLATFORM_VERSION_UNITS,
            versionCheck / NEXUS_PLATFORM_VERSION_UNITS,
            versionCheck % NEXUS_PLATFORM_VERSION_UNITS));
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }

    if (structSizeCheck != NEXUS_P_GET_STRUCT_SIZES()) {
        BDBG_ERR(("NEXUS_Platform failed with struct size mismatch (nexus=%d, caller=%d). Please recompile application and/or nexus.",
            NEXUS_P_GET_STRUCT_SIZES(), structSizeCheck));
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }

    if (g_NEXUS_platformModule != NULL) {
        BDBG_ERR(("Already initialized"));
        return BERR_TRACE(NEXUS_UNKNOWN);
    }

    /* Create defaults if no settings were provided */
    if ( NULL == pSettings )
    {
        NEXUS_Platform_GetDefaultSettings(&g_NEXUS_platformSettings);
    }
    else
    {
        g_NEXUS_platformSettings=*pSettings;
    }

    BKNI_Memset(&g_NEXUS_platformHandles, 0, sizeof(g_NEXUS_platformHandles));
    BLST_Q_INIT(&g_NEXUS_platformHandles.handles);

#if NEXUS_HAS_DISPLAY
    for (i=0;i<NEXUS_NUM_DISPLAYS;i++) {
        g_NEXUS_platformHandles.config.supportedDisplay[i] = NEXUS_NUM_DISPLAYS>i;
    }
    g_NEXUS_platformHandles.config.numWindowsPerDisplay = NEXUS_NUM_VIDEO_WINDOWS;
#endif

#if NEXUS_HAS_VIDEO_DECODER
    for (i=0;i<NEXUS_NUM_VIDEO_DECODERS;i++) {
        g_NEXUS_platformHandles.config.supportedDecoder[i] = NEXUS_NUM_VIDEO_DECODERS>i;
    }
#endif

    errCode = NEXUS_Platform_P_Magnum_Init();
    if ( errCode!=BERR_SUCCESS ) { errCode=BERR_TRACE(errCode); goto err_magnum; }

    BDBG_MSG((">BASE"));
    NEXUS_Base_GetDefaultSettings(&baseSettings);
#if NEXUS_BASE_OS_linuxkernel
    /* callback per module create to generic driver for /proc registration */
    baseSettings.driverModuleInit = nexus_driver_module_init_enum_cb;
    baseSettings.driverModuleUninit = nexus_driver_module_uninit_enum_cb;
#endif
    errCode = NEXUS_Base_Init(&baseSettings);
    if ( errCode!=BERR_SUCCESS ) { errCode=BERR_TRACE(errCode); goto err_base; }

    g_NEXUS_platformModule = NEXUS_Module_Create("platform", NULL);
    if ( !g_NEXUS_platformModule ) { errCode=BERR_TRACE(errCode); goto err_plaform_module; }
    NEXUS_LockModule();

    BDBG_MSG((">OS"));
    errCode = NEXUS_Platform_P_InitOS();
    if ( errCode!=BERR_SUCCESS ) { errCode=BERR_TRACE(errCode); goto err_os; }

    /* Init the core modules */
    BDBG_MSG((">CORE"));
    errCode = NEXUS_Platform_P_InitCore(&g_NEXUS_platformSettings);
    if ( errCode!=BERR_SUCCESS ) { errCode=BERR_TRACE(errCode); goto err_core; }

    NEXUS_Platform_P_AddModule(g_NEXUS_platformHandles.core, NEXUS_PlatformStandbyLockMode_ePassiveOnly, NULL, NEXUS_CoreModule_Standby_priv);

    for (i=0;i<NEXUS_MAX_HEAPS;i++) {
        if (g_pCoreHandles->nexusHeap[i]) {
            NEXUS_MemoryStatus status;
            NEXUS_Error rc;
            rc = NEXUS_Heap_GetStatus(g_pCoreHandles->nexusHeap[i], &status);
            if (!rc && status.memoryType == NEXUS_MemoryType_eSecure) {
                BDBG_WRN(("Heap %d has been marked as the secure heap", i));
                g_secureHeap = g_pCoreHandles->nexusHeap[i];
                break;
            }
        }
    }

    if(NEXUS_GetEnv("profile_init")) {
        NEXUS_Profile_Start();
    }

    /* do board-specific configuration after Core is brought up */
    BDBG_MSG((">INIT_BOARD"));
    errCode = NEXUS_Platform_P_InitBoard();
    if (errCode) {errCode = BERR_TRACE(errCode); goto err_initboard;}

    BDBG_MSG((">PINMUX"));
    /* Init pinmuxes and vcxo control */
    errCode = NEXUS_Platform_P_InitPinmux();
    if ( errCode!=BERR_SUCCESS ) { errCode=BERR_TRACE(errCode); goto err_postboard; }

    BDBG_MSG((">VCXO"));
    errCode = NEXUS_Platform_P_InitVcxo();
    if ( errCode!=BERR_SUCCESS ) { errCode=BERR_TRACE(errCode); goto err_postboard; }

    /* We acquire the BINT resource here , becuse there may be pending L1, which may fire before the modules
       are initialized. ALso, in case of weak mask interurpts, we need to clear the L2. So acquire BINT and
       release after all modules have been initialized */

#ifdef BCHP_PWR_RESOURCE_BINT_OPEN
    BCHP_PWR_AcquireResource(g_NEXUS_pCoreHandles->chp, BCHP_PWR_RESOURCE_BINT_OPEN);
#endif

    /* Start Interrupts */
    BDBG_MSG((">InitInterrupts"));
    errCode = NEXUS_Platform_P_InitInterrupts();
    if ( errCode!=BERR_SUCCESS ) { errCode=BERR_TRACE(errCode); goto err_postboard; }

    /* First bring up the minimum modules necessary to launch the module-init threads. */

/* For some chips, the security module must be initialized before transport.
For some, transport must be initialized before security. For others, it can be in either order.
This requirement comes from hardware wiring for reset or security.
*/
#if BCHP_CHIP == 7550 || BCHP_CHIP == 7408
#define NEXUS_TRANSPORT_BEFORE_SECURITY 1
#endif

#if !NEXUS_TRANSPORT_BEFORE_SECURITY
#if NEXUS_HAS_SECURITY
    BDBG_MSG((">SECURITY"));
    g_NEXUS_platformHandles.security = NEXUS_SecurityModule_Init(&g_NEXUS_platformSettings.securitySettings);
    if (!g_NEXUS_platformHandles.security) {
        BDBG_ERR(("Unable to init security"));
        errCode = BERR_TRACE(BERR_NOT_SUPPORTED);
        goto err;
    }
    else {
        NEXUS_Platform_P_AddModule(g_NEXUS_platformHandles.security, NEXUS_PlatformStandbyLockMode_ePassiveOnly, NEXUS_SecurityModule_Uninit, NEXUS_SecurityModule_Standby_priv);
    }
#endif
#endif /* NEXUS_TRANSPORT_BEFORE_SECURITY */

#if NEXUS_HAS_DMA
    BDBG_MSG((">DMA"));
    NEXUS_DmaModule_GetDefaultSettings(&dmaSettings);
    g_NEXUS_platformHandles.dma = NEXUS_DmaModule_Init(&dmaSettings);
    if (!g_NEXUS_platformHandles.dma) {
        BDBG_ERR(("Unable to init Dma"));
        errCode = BERR_TRACE(BERR_NOT_SUPPORTED);
        goto err;
    }
    else {
        NEXUS_Platform_P_AddModule(g_NEXUS_platformHandles.dma, NEXUS_PlatformStandbyLockMode_ePassiveOnly, NEXUS_DmaModule_Uninit, NEXUS_DmaModule_Standby_priv);
    }
#endif
#if NEXUS_HAS_TRANSPORT
    BDBG_MSG((">TRANSPORT"));
    NEXUS_TransportModule_GetDefaultSettings(&transportSettings);
    transportSettings = g_NEXUS_platformSettings.transportModuleSettings;
    transportSettings.dma = g_NEXUS_platformHandles.dma;
    transportSettings.secureHeap = g_secureHeap;
    g_NEXUS_platformHandles.transport = NEXUS_TransportModule_Init(&transportSettings);
    if (!g_NEXUS_platformHandles.transport) {
        BDBG_ERR(("Unable to init Transport"));
        errCode = BERR_TRACE(NEXUS_NOT_SUPPORTED);
        goto err;
    } else {
        NEXUS_Platform_P_AddModule(g_NEXUS_platformHandles.transport, NEXUS_PlatformStandbyLockMode_ePassiveOnly, NEXUS_TransportModule_Uninit, NEXUS_TransportModule_Standby_priv);
    }
#endif

#if NEXUS_TRANSPORT_BEFORE_SECURITY
#if NEXUS_HAS_SECURITY
    BDBG_MSG((">SECURITY"));
    g_NEXUS_platformHandles.security = NEXUS_SecurityModule_Init(&g_NEXUS_platformSettings.securitySettings);
    if (!g_NEXUS_platformHandles.security) {
        BDBG_ERR(("Unable to init security"));
        errCode = BERR_TRACE(BERR_NOT_SUPPORTED);
        goto err;
    }
    else {
        NEXUS_Platform_P_AddModule(g_NEXUS_platformHandles.security, NEXUS_PlatformStandbyLockMode_ePassiveOnly, NEXUS_SecurityModule_Uninit, NEXUS_SecurityModule_Standby_priv);
    }
#endif
#endif /* NEXUS_TRANSPORT_BEFORE_SECURITY */

#if NEXUS_HAS_NSK2HDI
    BDBG_MSG((">NSK2HDI"));
    NEXUS_Nsk2hdiModule_GetDefaultSettings(&nsk2hdiSettings);
    nsk2hdiSettings.securityModule = g_NEXUS_platformHandles.security;
    nsk2hdiSettings.transportModule = g_NEXUS_platformHandles.transport;
    handle = NEXUS_Nsk2hdiModule_Init(&nsk2hdiSettings);
    if ( !handle )
    {
        BDBG_ERR(("Unable to init nsk2hdi"));
        errCode = BERR_TRACE(BERR_NOT_SUPPORTED);
        goto err;
    }
    else {
        NEXUS_Platform_P_AddModule(handle, NEXUS_PlatformStandbyLockMode_ePassiveOnly, NEXUS_Nsk2hdiModule_Uninit, NULL);
    }
#endif

#if NEXUS_HAS_I2C
    /* Init I2C */
    BDBG_MSG((">I2C"));
    g_NEXUS_platformHandles.i2c = NEXUS_I2cModule_Init(NULL);
    if ( !g_NEXUS_platformHandles.i2c ) {
    BDBG_ERR(("Unable to init I2C"));
        errCode=BERR_TRACE(errCode);
        goto err;
    }
    else {
        NEXUS_Platform_P_AddModule(g_NEXUS_platformHandles.i2c, NEXUS_PlatformStandbyLockMode_eAlways, NEXUS_I2cModule_Uninit, NEXUS_I2cModule_Standby_priv);
    }
#endif

#if NEXUS_HAS_HDMI_OUTPUT
    BDBG_MSG((">HDMI_OUTPUT"));
    NEXUS_HdmiOutputModule_GetDefaultSettings(&hdmiSettings);
#if NEXUS_HAS_SECURITY
    hdmiSettings.modules.security = g_NEXUS_platformHandles.security;
#endif
    g_NEXUS_platformHandles.hdmiOutput = NEXUS_HdmiOutputModule_Init(&hdmiSettings);
    if ( !g_NEXUS_platformHandles.hdmiOutput) {
        BDBG_ERR(("Unable to init HdmiOutput"));
        errCode = BERR_TRACE(NEXUS_NOT_SUPPORTED);
        goto err;
    } else {
        NEXUS_Platform_P_AddModule(g_NEXUS_platformHandles.hdmiOutput, NEXUS_PlatformStandbyLockMode_eAlways, NEXUS_HdmiOutputModule_Uninit, NEXUS_HdmiOutputModule_Standby_priv);
    }
#endif

#if NEXUS_HAS_CEC
    BDBG_MSG((">CEC"));
    NEXUS_CecModule_GetDefaultSettings(&cecSettings);
    g_NEXUS_platformHandles.cec = NEXUS_CecModule_Init(&cecSettings);
    if ( !g_NEXUS_platformHandles.cec) {
        BDBG_ERR(("Unable to init CEC"));
        errCode = BERR_TRACE(NEXUS_NOT_SUPPORTED);
        goto err;
    } else {
        NEXUS_Platform_P_AddModule(g_NEXUS_platformHandles.cec, NEXUS_PlatformStandbyLockMode_eAlways, NEXUS_CecModule_Uninit, NEXUS_CecModule_Standby_priv);
    }
#endif


#if NEXUS_HAS_HDMI_DVO
    BDBG_MSG((">HDMI_DVO"));
    g_NEXUS_platformHandles.hdmiDvo = NEXUS_HdmiDvoModule_Init(NULL);
    if (!g_NEXUS_platformHandles.hdmiDvo) {
        BDBG_ERR(("Unable to init HdmiDvo"));
        errCode = BERR_TRACE(NEXUS_NOT_SUPPORTED);
        goto err;
    } else {
        NEXUS_Platform_P_AddModule(g_NEXUS_platformHandles.hdmiDvo, NEXUS_PlatformStandbyLockMode_eAlways, NEXUS_HdmiDvoModule_Uninit, NULL);
    }
#endif

#if NEXUS_HAS_HDMI_INPUT
    BDBG_MSG((">HDMI_INPUT"));
    NEXUS_HdmiInputModule_GetDefaultSettings(&hdmiInputSettings);
#if NEXUS_HAS_SECURITY
    hdmiInputSettings.modules.security = g_NEXUS_platformHandles.security;
#endif
    g_NEXUS_platformHandles.hdmiInput = NEXUS_HdmiInputModule_Init(&hdmiInputSettings);
    if (!g_NEXUS_platformHandles.hdmiInput)
    {
        BDBG_ERR(("Unable to init hdmiInput"));
        errCode = BERR_TRACE(BERR_NOT_SUPPORTED);
        goto err;
    } else {
        NEXUS_Platform_P_AddModule(g_NEXUS_platformHandles.hdmiInput, NEXUS_PlatformStandbyLockMode_eAlways, NEXUS_HdmiInputModule_Uninit, NEXUS_HdmiInputModule_Standby_priv);
    }
#endif



#if NEXUS_HAS_RFM
    BDBG_MSG((">RFM"));
    g_NEXUS_platformHandles.rfm = NEXUS_RfmModule_Init(NULL);
    if (!g_NEXUS_platformHandles.rfm) {
        BDBG_ERR(("Unable to init Rfm"));
        errCode = BERR_TRACE(NEXUS_NOT_SUPPORTED);
        goto err;
    } else {
        NEXUS_Platform_P_AddModule(g_NEXUS_platformHandles.rfm, NEXUS_PlatformStandbyLockMode_eAlways, NEXUS_RfmModule_Uninit, NEXUS_RfmModule_Standby_priv);
    }
#endif

#if NEXUS_HAS_SURFACE
    BDBG_MSG((">SURFACE"));
    g_NEXUS_platformHandles.surface = NEXUS_SurfaceModule_Init(&g_NEXUS_platformSettings.surfacePlatformSettings);
    if (!g_NEXUS_platformHandles.surface) {
        BDBG_ERR(("Unable to init Surface"));
        errCode = BERR_TRACE(NEXUS_NOT_SUPPORTED);
        goto err;
    } else {
        NEXUS_Platform_P_AddModule(g_NEXUS_platformHandles.surface, NEXUS_PlatformStandbyLockMode_eNone, NEXUS_SurfaceModule_Uninit, NULL);
    }
#endif


    /* Now that our init threads are launched, we can bring up modules in any order. */
#if NEXUS_HAS_SPI
    BDBG_MSG((">SPI"));
    handle = NEXUS_SpiModule_Init(NULL);
    if ( !handle ) {
        BDBG_ERR(("Unable to init Spi"));
        errCode=BERR_TRACE(errCode);
        goto err;
    } else {
        NEXUS_Platform_P_AddModule(handle, NEXUS_PlatformStandbyLockMode_eNone, NEXUS_SpiModule_Uninit, NULL);
    }
#endif

    /* Other IO modules */
#if NEXUS_HAS_IR_INPUT
    BDBG_MSG((">IR_INPUT"));
    handle = NEXUS_IrInputModule_Init(NULL);
    if ( !handle ) {
        BDBG_ERR(("Unable to init Ir Input"));
        errCode=BERR_TRACE(errCode);
        goto err;
    } else {
        NEXUS_Platform_P_AddModule(handle, NEXUS_PlatformStandbyLockMode_eNone, NEXUS_IrInputModule_Uninit, NULL);
    }
#endif

#if NEXUS_HAS_INPUT_CAPTURE
    BDBG_MSG((">INPUT_CAPTURE"));
    handle = NEXUS_InputCaptureModule_Init(NULL);
    if ( !handle ) {
        BDBG_ERR(("Unable to init Input Capture"));
        errCode=BERR_TRACE(errCode);
        goto err;
    } else {
        NEXUS_Platform_P_AddModule(handle, NEXUS_PlatformStandbyLockMode_eAlways, NEXUS_InputCaptureModule_Uninit, NULL);
    }
#endif

#if NEXUS_HAS_IR_BLASTER
    BDBG_MSG((">IR_BLASTER"));
    handle = NEXUS_IrBlasterModule_Init(NULL);
    if ( !handle ) {
        BDBG_ERR(("Unable to init Ir Blaster"));
        errCode=BERR_TRACE(errCode);
        goto err;
    } else {
        NEXUS_Platform_P_AddModule(handle, NEXUS_PlatformStandbyLockMode_eAlways, NEXUS_IrBlasterModule_Uninit, NULL);
    }
#endif

#if NEXUS_HAS_UHF_INPUT
    BDBG_MSG((">UHF_INPUT"));
    handle = NEXUS_UhfInputModule_Init(NULL);
    if ( !handle ) {
        BDBG_ERR(("Unable to init Uhf Input"));
        errCode=BERR_TRACE(errCode);
        goto err;
    } else {
        NEXUS_Platform_P_AddModule(handle, NEXUS_PlatformStandbyLockMode_ePassiveOnly, NEXUS_UhfInputModule_Uninit, NEXUS_UhfInputModule_Standby_priv);
    }
#endif

#if NEXUS_HAS_GPIO
    BDBG_MSG((">GPIO"));
    g_NEXUS_platformHandles.gpio = NEXUS_GpioModule_Init(NULL);
    if ( !g_NEXUS_platformHandles.gpio ) {
        BDBG_ERR(("Unable to init Gpio"));
        errCode=BERR_TRACE(errCode);
        goto err;
    } else {
        NEXUS_Platform_P_AddModule(g_NEXUS_platformHandles.gpio, NEXUS_PlatformStandbyLockMode_ePassiveOnly, NEXUS_GpioModule_Uninit, NEXUS_GpioModule_Standby_priv);
    }
#endif

#if NEXUS_HAS_LED
    BDBG_MSG((">LED"));
    NEXUS_LedModule_GetDefaultSettings(&ledSettings);
#if NEXUS_HAS_SPI_FRONTPANEL
    ledSettings.spi.valid = true;
    ledSettings.spi.index = NEXUS_SPI_INDEX(Upg,0);
#endif
    handle = NEXUS_LedModule_Init(&ledSettings);
    if ( !handle ) {
        BDBG_ERR(("Unable to init LED"));
        errCode=BERR_TRACE(errCode);
        goto err;
    } else {
        NEXUS_Platform_P_AddModule(handle, NEXUS_PlatformStandbyLockMode_eNone, NEXUS_LedModule_Uninit, NULL);
    }
#endif

#if NEXUS_HAS_KEYPAD
    NEXUS_KeypadModule_GetDefaultSettings(&keypadSettings);

#if NEXUS_HAS_SPI_FRONTPANEL
    keypadSettings.spi.valid = true;
    keypadSettings.spi.index = NEXUS_SPI_INDEX(Upg,1);
    keypadSettings.gpio.valid = true;
#ifdef NEXUS_KEYPAD_AON_GPIO
    keypadSettings.gpio.type = NEXUS_GpioType_eAonStandard;
    keypadSettings.gpio.pin = NEXUS_KEYPAD_AON_GPIO; /* defined in nexus_platform_features.h */
#else
    keypadSettings.gpio.type = NEXUS_GpioType_eStandard;
    keypadSettings.gpio.pin = NEXUS_KEYPAD_GPIO; /* defined in nexus_platform_features.h */
#endif
#endif

    BDBG_MSG((">KEYPAD"));
    handle = NEXUS_KeypadModule_Init(&keypadSettings);
    if ( !handle ) {
        BDBG_ERR(("Unable to init Keypad"));
        errCode=BERR_TRACE(errCode);
        goto err;
    } else {
        NEXUS_Platform_P_AddModule(handle, NEXUS_PlatformStandbyLockMode_eNone, NEXUS_KeypadModule_Uninit, NULL);
    }
#endif

#if NEXUS_HAS_UART
    BDBG_MSG((">UART"));
    handle = NEXUS_UartModule_Init(NULL);
    if (!handle) {
        BDBG_ERR(("Unable to init Uart"));
        errCode=BERR_TRACE(errCode);
        goto err;
    } else {
        NEXUS_Platform_P_AddModule(handle, NEXUS_PlatformStandbyLockMode_eAlways, NEXUS_UartModule_Uninit, NEXUS_UartModule_Standby_priv);
    }
#endif

#if NEXUS_HAS_TOUCHPAD
    BDBG_ERR((">NEXUS_TOUCHPAD_SUPPORT"));
    handle = NEXUS_TouchpadModule_Init(NULL);
    if ( !handle ) {
        BDBG_ERR(("Unable to init Touchpad"));
        errCode=BERR_TRACE(errCode);
        goto err;
    } else {
        NEXUS_Platform_P_AddModule(handle, NEXUS_PlatformStandbyLockMode_eAlways, NEXUS_TouchpadModule_Uninit, NULL);
    }
#endif
#if NEXUS_HAS_FRONTEND
    BDBG_MSG((">FRONTEND"));
    NEXUS_FrontendModule_GetDefaultSettings(&frontendSettings);
    frontendSettings.i2cModule = g_NEXUS_platformHandles.i2c;
    frontendSettings.transport = g_NEXUS_platformHandles.transport;
    g_NEXUS_platformHandles.frontend = NEXUS_FrontendModule_Init(&frontendSettings);
    if ( !g_NEXUS_platformHandles.frontend )
    {
        BDBG_ERR(("Unable to init frontend"));
        errCode = BERR_TRACE(NEXUS_NOT_SUPPORTED);
        goto err;
    } else {
    #if defined(HUMAX_PLATFORM_BASE)&& defined(CONFIG_EXT_DEMOD_POWER_CONTROL)

	#else
        NEXUS_Platform_P_AddModule(g_NEXUS_platformHandles.frontend, NEXUS_PlatformStandbyLockMode_ePassiveOnly, NEXUS_FrontendModule_Uninit, NEXUS_Platform_P_StandbyFrontend);
	#endif
    }
#endif


#if NEXUS_HAS_AUDIO
    /* audio requires transport, hdmiOutput and rfm. so we can launch as early as here. */
#if MULTITHREADED_INIT && !NEXUS_NUM_ZSP_VIDEO_DECODERS && !NEXUS_NUM_DSP_VIDEO_DECODERS
    audioThread = NEXUS_Thread_Create("audio_init", NEXUS_Platform_P_InitAudio, NULL, NULL);
#else
    NEXUS_Platform_P_InitAudio(NULL);
    if (!g_NEXUS_platformHandles.audio) {
        errCode = BERR_TRACE(NEXUS_NOT_SUPPORTED);
        goto err;
    }
#endif
#endif

#if NEXUS_HAS_PICTURE_DECODER
    BDBG_MSG((">PICTURE_DECODER"));
    NEXUS_PictureDecoderModule_GetDefaultSettings(&pictureDecoderSettings);
    pictureDecoderSettings.surface = g_NEXUS_platformHandles.surface;
    pictureDecoderSettings.transport = g_NEXUS_platformHandles.transport;
    g_NEXUS_platformHandles.pictureDecoder = NEXUS_PictureDecoderModule_Init(&pictureDecoderSettings);
    if (!g_NEXUS_platformHandles.pictureDecoder) {
        BDBG_ERR(("Unable to init PictureDecoder"));
        errCode = BERR_TRACE(BERR_NOT_SUPPORTED);
        goto err;
    } else {
        NEXUS_Platform_P_AddModule(g_NEXUS_platformHandles.pictureDecoder, NEXUS_PlatformStandbyLockMode_eAlways, NEXUS_PictureDecoderModule_Uninit, NEXUS_PictureDecoderModule_Standby_priv);
    }
#endif

#if NEXUS_HAS_VIDEO_DECODER
    /* video decoder requires transport and secure heap. so we can launch as early as here. */
#if MULTITHREADED_INIT
    videoDecoderThread = NEXUS_Thread_Create("video_decoder_init", NEXUS_Platform_P_InitVideoDecoder, NULL, NULL);
#else
    NEXUS_Platform_P_InitVideoDecoder(NULL);
    if (!g_NEXUS_platformHandles.videoDecoder) {
        errCode = BERR_TRACE(NEXUS_NOT_SUPPORTED);
        goto err;
    }
#endif
#endif

#if NEXUS_HAS_DVB_CI
    BDBG_MSG((">DVB-CI"));
    NEXUS_DvbCiModule_GetDefaultSettings(&dvbCiSettings);
    dvbCiSettings.modules.gpio = g_NEXUS_platformHandles.gpio;
    handle = NEXUS_DvbCiModule_Init(&dvbCiSettings);
    if ( !handle )
    {
        BDBG_ERR(("Unable to init DVB-CI module"));
        errCode = BERR_TRACE(BERR_NOT_SUPPORTED);
        goto err;
    } else {
        NEXUS_Platform_P_AddModule(handle, NEXUS_PlatformStandbyLockMode_ePassiveOnly, NEXUS_DvbCiModule_Uninit, NULL);
    }
#endif

#if NEXUS_HAS_PCMCIA
    BDBG_MSG(("PCMCIA"));
    NEXUS_PcmCiaModule_GetDefaultSettings(&pcmCiaSettings);
    pcmCiaSettings.modules.gpio = g_NEXUS_platformHandles.gpio;
    handle = NEXUS_PcmCiaModule_Init(&pcmCiaSettings);
    if ( !handle )
    {
        BDBG_ERR(("Unable to init PCMCIA module"));
        errCode = BERR_TRACE(BERR_NOT_SUPPORTED);
        goto err;
    } else {
        NEXUS_Platform_P_AddModule(handle, NEXUS_PlatformStandbyLockMode_eAlways, NEXUS_PcmCiaModule_Uninit, NULL);
    }
#else
    BSTD_UNUSED(g_NEXUS_platformHandles.gpio);
#endif


#if NEXUS_HAS_GRAPHICS2D
    BDBG_MSG((">GRAPHICS2D"));
    graphics2DSettings = g_NEXUS_platformSettings.graphics2DModuleSettings;
    graphics2DSettings.surface = g_NEXUS_platformHandles.surface;
    handle = NEXUS_Graphics2DModule_Init(&graphics2DSettings);
    if (!handle) {
        BDBG_ERR(("Unable to init Graphics2D"));
        errCode = BERR_TRACE(NEXUS_NOT_SUPPORTED);
        goto err;
    } else {
        NEXUS_Platform_P_AddModule(handle, NEXUS_PlatformStandbyLockMode_eAlways, NEXUS_Graphics2DModule_Uninit, NEXUS_Graphics2DModule_Standby_priv);
    }
#endif

#if NEXUS_HAS_SMARTCARD
    BDBG_MSG((">SMARTCARD"));
    if(!pSettings) {
        NEXUS_SmartcardModule_GetDefaultSettings(&smartCardSettings);
        smartCardSettings.clockSource= NEXUS_SmartcardClockSource_eInternalClock;
#if NEXUS_BOARD_7530_CRB
        smartCardSettings.clockFrequency = 24000000;
#else
        smartCardSettings.clockFrequency = 27000000;
#endif
        handle = NEXUS_SmartcardModule_Init(&smartCardSettings);
    }
    else {
        handle = NEXUS_SmartcardModule_Init(&g_NEXUS_platformSettings.smartCardSettings);
    }
    if (!handle) {
        BDBG_ERR(("Unable to init SmartCard"));
        errCode = BERR_TRACE(NEXUS_NOT_SUPPORTED);
        goto err;
    } else {
        NEXUS_Platform_P_AddModule(handle, NEXUS_PlatformStandbyLockMode_ePassiveOnly, NEXUS_SmartcardModule_Uninit, NEXUS_SmartcardModule_Standby_priv);
    }
#endif

#if NEXUS_HAS_PWM
    BDBG_MSG((">PWM"));
    g_NEXUS_platformHandles.pwm = NEXUS_PwmModule_Init(NULL);
    if (!g_NEXUS_platformHandles.pwm) {
        BDBG_ERR(("NEXUS_PwmModule_Init failed\n"));
        errCode = BERR_TRACE(BERR_NOT_SUPPORTED);
        goto err;
    }
    else {
        NEXUS_Platform_P_AddModule(g_NEXUS_platformHandles.pwm, NEXUS_PlatformStandbyLockMode_eAlways, NEXUS_PwmModule_Uninit, NEXUS_PwmModule_Standby_priv);
    }
#endif

#if NEXUS_HAS_TEMP_MONITOR
    BDBG_MSG((">TEMP_MONITOR"));
    handle = NEXUS_TempMonitorModule_Init(NULL);
    if (!handle) {
        BDBG_ERR(("NEXUS_TempMonitorModule_Init failed\n"));
        errCode = BERR_TRACE(BERR_NOT_SUPPORTED);
        goto err;
    } else {
        NEXUS_Platform_P_AddModule(handle, NEXUS_PlatformStandbyLockMode_eAlways, NEXUS_TempMonitorModule_Uninit, NULL);
    }
#endif

#if NEXUS_HAS_FILE
    BDBG_MSG((">FILE"));
    {
        NEXUS_FileModuleSettings fileModuleSettings;
        NEXUS_FileModule_GetDefaultSettings(&fileModuleSettings);
        fileModuleSettings.workerThreads = g_NEXUS_platformSettings.fileModuleSettings.workerThreads;
        BKNI_Memcpy(fileModuleSettings.schedulerSettings, g_NEXUS_platformSettings.fileModuleSettings.schedulerSettings, sizeof(fileModuleSettings.schedulerSettings));
        g_NEXUS_platformHandles.file = NEXUS_FileModule_Init(&fileModuleSettings);
    }
    if (!g_NEXUS_platformHandles.file) {
        BDBG_ERR(("Unable to init File"));
        errCode = BERR_TRACE(NEXUS_NOT_SUPPORTED);
        goto err;
    } else {
        NEXUS_Platform_P_AddModule(g_NEXUS_platformHandles.file, NEXUS_PlatformStandbyLockMode_eNone, NEXUS_FileModule_Uninit, NULL);
    }
#endif

#if NEXUS_HAS_GRAPHICS3D
    BDBG_MSG((">GRAPHICS3D"));
    handle = NEXUS_Graphics3dModule_Init(NULL);
    if ( !handle ) {
        BDBG_ERR(("Unable to initialize 3D graphics"));
        errCode = BERR_TRACE(NEXUS_NOT_SUPPORTED);
        goto err;
    } else {
        NEXUS_Platform_P_AddModule(handle, NEXUS_PlatformStandbyLockMode_eAlways, NEXUS_Graphics3dModule_Uninit, NULL);
    }
#endif

#if NEXUS_HAS_DISPLAY
    BDBG_MSG((">DISPLAY"));
    {
        /* NEXUS_DisplayModuleSettings is large (600+ bytes) so we malloc to avoid stack blowout */
        NEXUS_DisplayModuleSettings *pDisplaySettings = BKNI_Malloc(sizeof(*pDisplaySettings));
        if (!pDisplaySettings) {
            errCode = BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);
            goto err;
        }
        *pDisplaySettings = g_NEXUS_platformSettings.displayModuleSettings;
        pDisplaySettings->modules.surface = g_NEXUS_platformHandles.surface;
        /* don't set pDisplaySettings->modules.videoDecoder here */
    #if NEXUS_HAS_HDMI_OUTPUT
        pDisplaySettings->modules.hdmiOutput = g_NEXUS_platformHandles.hdmiOutput;
    #endif
    #if NEXUS_HAS_HDMI_DVO
        pDisplaySettings->modules.hdmiDvo = g_NEXUS_platformHandles.hdmiDvo;
    #endif
    #if NEXUS_HAS_HDMI_INPUT
        pDisplaySettings->modules.hdmiInput = g_NEXUS_platformHandles.hdmiInput;
    #endif
    #if NEXUS_HAS_RFM
        pDisplaySettings->modules.rfm = g_NEXUS_platformHandles.rfm;
    #endif
    #if NEXUS_HAS_PWM
        pDisplaySettings->modules.pwm = g_NEXUS_platformHandles.pwm;
    #endif
        pDisplaySettings->modules.transport = g_NEXUS_platformHandles.transport;
        g_NEXUS_platformHandles.display = NEXUS_DisplayModule_Init(pDisplaySettings);
        BKNI_Free(pDisplaySettings);
        if (!g_NEXUS_platformHandles.display) {
            BDBG_ERR(("Unable to init Display"));
            errCode = BERR_TRACE(NEXUS_NOT_SUPPORTED);
            goto err;
        }
    }
#endif


#if MULTITHREADED_INIT
    /* audio is needed for sync, so join here */
    if (audioThread) {
        NEXUS_Thread_Destroy(audioThread);
        audioThread = NULL;
        if (!g_NEXUS_platformHandles.audio) {
            errCode = BERR_TRACE(NEXUS_NOT_SUPPORTED);
            goto err;
        }
    }
#endif

#if NEXUS_HAS_SYNC_CHANNEL
    BDBG_MSG((">SYNC_CHANNEL"));
    NEXUS_SyncChannelModule_GetDefaultSettings(&syncChannelSettings);
    syncChannelSettings.modules.display = g_NEXUS_platformHandles.display;
    syncChannelSettings.modules.audio = g_NEXUS_platformHandles.audio;
    handle = NEXUS_SyncChannelModule_Init(&syncChannelSettings);
    if (!handle) {
        BDBG_ERR(("Unable to init SyncChannel"));
        errCode = BERR_TRACE(NEXUS_NOT_SUPPORTED);
        goto err;
    } else {
        NEXUS_Platform_P_AddModule(handle, NEXUS_PlatformStandbyLockMode_eNone, NEXUS_SyncChannelModule_Uninit, NULL);
    }
#endif

    /* videoDecoder is needed for astm, so join here */
    /* coverity[dead_error_condition] */
    if (videoDecoderThread) {
        NEXUS_Thread_Destroy(videoDecoderThread);
        videoDecoderThread = NULL;
        if (!g_NEXUS_platformHandles.videoDecoder) {
            errCode = BERR_TRACE(NEXUS_NOT_SUPPORTED);
            goto err;
        }
    }

#if NEXUS_HAS_VIDEO_DECODER
    NEXUS_DisplayModule_SetVideoDecoderModule(g_NEXUS_platformHandles.videoDecoder);
    /* Video Decoder and Display need to go in this order */
    NEXUS_Platform_P_AddModule(g_NEXUS_platformHandles.videoDecoder, NEXUS_PlatformStandbyLockMode_eAlways, NEXUS_VideoDecoderModule_Uninit, NEXUS_VideoDecoderModule_Standby_priv);
#endif
#if NEXUS_HAS_DISPLAY
    NEXUS_Platform_P_AddModule(g_NEXUS_platformHandles.display, NEXUS_PlatformStandbyLockMode_eAlways, NEXUS_DisplayModule_Uninit, NEXUS_DisplayModule_Standby_priv);
#endif

#if NEXUS_HAS_VIDEO_ENCODER
    BDBG_MSG((">VIDEO_ENCODER"));
    g_NEXUS_platformSettings.videoEncoderSettings.display = g_NEXUS_platformHandles.display;
    g_NEXUS_platformSettings.videoEncoderSettings.transport = g_NEXUS_platformHandles.transport;
    g_NEXUS_platformSettings.videoEncoderSettings.audio = g_NEXUS_platformHandles.audio;
    g_NEXUS_platformHandles.videoEncoder = NEXUS_VideoEncoderModule_Init(&g_NEXUS_platformSettings.videoEncoderSettings);
    if(!g_NEXUS_platformHandles.videoEncoder) {
        BDBG_ERR(("NEXUS_VideoEncoderModule_Init failed\n"));
        errCode = BERR_TRACE(BERR_NOT_SUPPORTED);
        goto err;
    } else {
        NEXUS_Platform_P_AddModule(g_NEXUS_platformHandles.videoEncoder, NEXUS_PlatformStandbyLockMode_eAlways, NEXUS_VideoEncoderModule_Uninit, NEXUS_VideoEncoderModule_Standby_priv);
    }
#endif

#if NEXUS_HAS_STREAM_MUX
    BDBG_MSG((">STREAM_MUX"));
    NEXUS_StreamMuxModule_GetDefaultSettings(&streamMuxModuleSettings);
    streamMuxModuleSettings.transport = g_NEXUS_platformHandles.transport;
    streamMuxModuleSettings.audio = g_NEXUS_platformHandles.audio;
    streamMuxModuleSettings.videoEncoder = g_NEXUS_platformHandles.videoEncoder;
    handle = NEXUS_StreamMuxModule_Init(&streamMuxModuleSettings);
    if(!handle) {
        BDBG_ERR(("NEXUS_StreamMuxModule_Init: failed\n"));
        errCode = BERR_TRACE(BERR_NOT_SUPPORTED);
        goto err;
    } else {
        NEXUS_Platform_P_AddModule(handle, NEXUS_PlatformStandbyLockMode_eNone, NEXUS_StreamMuxModule_Uninit, NULL);
    }
#endif

#if NEXUS_HAS_ASTM
    BDBG_MSG((">ASTM"));
    NEXUS_AstmModule_GetDefaultSettings(&astmSettings);
    astmSettings.modules.videoDecoder = g_NEXUS_platformHandles.videoDecoder;
    astmSettings.modules.audio = g_NEXUS_platformHandles.audio;
    astmSettings.modules.transport = g_NEXUS_platformHandles.transport;
    handle = NEXUS_AstmModule_Init(&astmSettings);
    if (!handle) {
        BDBG_ERR(("Unable to init Astm"));
        errCode = BERR_TRACE(NEXUS_NOT_SUPPORTED);
        goto err;
    } else {
        NEXUS_Platform_P_AddModule(handle, NEXUS_PlatformStandbyLockMode_eNone, NEXUS_AstmModule_Uninit, NULL);
    }
#endif

#if NEXUS_HAS_PLAYBACK
    BDBG_MSG((">PLAYBACK"));
    NEXUS_PlaybackModule_GetDefaultSettings(&playbackSettings);
    playbackSettings.modules.file = g_NEXUS_platformHandles.file;
    playbackSettings.modules.audioDecoder = g_NEXUS_platformHandles.audio;
    playbackSettings.modules.videoDecoder = g_NEXUS_platformHandles.videoDecoder;
    playbackSettings.modules.playpump = g_NEXUS_platformHandles.transport;
    g_NEXUS_platformHandles.playback = NEXUS_PlaybackModule_Init(&playbackSettings);
    if (!g_NEXUS_platformHandles.playback) {
        BDBG_ERR(("Unable to init playback"));
        errCode = BERR_TRACE(NEXUS_NOT_SUPPORTED);
        goto err;
    } else {
        NEXUS_Platform_P_AddModule(g_NEXUS_platformHandles.playback, NEXUS_PlatformStandbyLockMode_eNone, NEXUS_PlaybackModule_Uninit, NULL);
    }
#endif

#if NEXUS_HAS_RECORD
    BDBG_MSG((">RECORD"));
    NEXUS_RecordModule_GetDefaultSettings(&recordSettings);
    recordSettings.modules.file = g_NEXUS_platformHandles.file;
    recordSettings.modules.recpump = g_NEXUS_platformHandles.transport;
#if NEXUS_HAS_PLAYBACK
    recordSettings.modules.playback = g_NEXUS_platformHandles.playback;
#endif
    handle = NEXUS_RecordModule_Init(&recordSettings);
    if (!handle) {
        BDBG_ERR(("Unable to init record"));
        errCode = BERR_TRACE(NEXUS_NOT_SUPPORTED);
        goto err;
    } else {
        NEXUS_Platform_P_AddModule(handle, NEXUS_PlatformStandbyLockMode_eNone, NEXUS_RecordModule_Uninit, NULL);
    }
#endif

#if NEXUS_HAS_FILE_MUX
    BDBG_MSG((">FILE_MUX"));
    handle = NEXUS_FileMuxModule_Init(NULL);
    if (!handle) {
        BDBG_ERR(("Unable to init File Mux"));
        errCode = BERR_TRACE(NEXUS_NOT_SUPPORTED);
        goto err;
    } else {
        NEXUS_Platform_P_AddModule(handle, NEXUS_PlatformStandbyLockMode_eNone, NEXUS_FileMuxModule_Uninit, NULL);
    }
#endif

#if NEXUS_HAS_SIMPLE_DECODER
    BDBG_MSG((">SIMPLE_DECODER"));
    handle = NEXUS_SimpleDecoderModule_Init(NULL);
    if (!handle) {
        BDBG_ERR(("Unable to init Simple Decoder"));
        errCode = BERR_TRACE(NEXUS_NOT_SUPPORTED);
        goto err;
    } else {
        NEXUS_Platform_P_AddModule(handle, NEXUS_PlatformStandbyLockMode_eNone, NEXUS_SimpleDecoderModule_Uninit, NULL);
    }
#endif

#if NEXUS_HAS_SURFACE_COMPOSITOR
    {
    NEXUS_SurfaceCompositorModuleSettings settings;
    BDBG_MSG((">SURFACE_COMPOSITOR"));
    settings.modules.display = g_NEXUS_platformHandles.display;
    handle = NEXUS_SurfaceCompositorModule_Init(&settings);
    }
    if (!handle) {
        BDBG_ERR(("Unable to init SurfaceCompositor"));
        errCode = BERR_TRACE(NEXUS_NOT_SUPPORTED);
        goto err;
    } else {
        NEXUS_Platform_P_AddModule(handle, NEXUS_PlatformStandbyLockMode_eNone, NEXUS_SurfaceCompositorModule_Uninit, NULL);
    }
#endif

#if NEXUS_HAS_INPUT_ROUTER
    BDBG_MSG((">INPUT_ROUTER"));
    handle = NEXUS_InputRouterModule_Init(NULL);
    if (!handle) {
        BDBG_ERR(("Unable to init InputRouter"));
        errCode = BERR_TRACE(NEXUS_NOT_SUPPORTED);
        goto err;
    } else {
        NEXUS_Platform_P_AddModule(handle, NEXUS_PlatformStandbyLockMode_eNone, NEXUS_InputRouterModule_Uninit, NULL);
    }
#endif

#if NEXUS_HAS_WMDRMPD_IO
    BDBG_MSG((">WMDRMPD_IO"));
    wmdrmpdIo = NEXUS_WmDrmPdIoModule_Init(NULL);
    if (!wmdrmpdIo) {
        BDBG_ERR(("Unable to init wmdrmpdIo"));
        errCode = BERR_TRACE(NEXUS_NOT_SUPPORTED);
        goto err;
    } else {
        NEXUS_Platform_P_AddModule(wmdrmpdIo, NEXUS_PlatformStandbyLockMode_eNone, NEXUS_WmDrmPdIoModule_Uninit, NULL);
    }
#endif

#if NEXUS_HAS_WMDRMPD_CORE
    BDBG_MSG((">WMDRMPD_CORE"));
    NEXUS_WmDrmPdCoreModule_GetDefaultSettings(&wmdrmpdcoreSettings);
    wmdrmpdcoreSettings.modules.wmdrmpdIo = wmdrmpdIo;
    wmdrmpdCore = NEXUS_WmDrmPdCoreModule_Init(&wmdrmpdcoreSettings);
    if (!wmdrmpdCore) {
        BDBG_ERR(("Unable to init wmdrmpdCore"));
        errCode = BERR_TRACE(NEXUS_NOT_SUPPORTED);
        goto err;
    } else {
        NEXUS_Platform_P_AddModule(wmdrmpdCore, NEXUS_PlatformStandbyLockMode_eNone, NEXUS_WmDrmPdCoreModule_Uninit, NULL);
    }
#endif

#if NEXUS_HAS_WMDRMPD
    BDBG_MSG((">WMDRMPD"));
    NEXUS_WmDrmPdModule_GetDefaultSettings(&wmdrmpdSettings);
    wmdrmpdSettings.modules.wmdrmpdCore = wmdrmpdCore;
    wmdrmpdSettings.modules.wmdrmpdIo = wmdrmpdIo;
    wmdrmpd = NEXUS_WmDrmPdModule_Init(&wmdrmpdSettings);
    if (!wmdrmpd) {
        BDBG_ERR(("Unable to init wmdrmpd"));
        errCode = BERR_TRACE(NEXUS_NOT_SUPPORTED);
        goto err;
    } else {
        NEXUS_Platform_P_AddModule(wmdrmpd, NEXUS_PlatformStandbyLockMode_eNone, NEXUS_WmDrmPdModule_Uninit, NULL);
    }
#endif

    /* all modules are up, so we can bring up IPC server before opening interfaces. */
    NEXUS_Platform_P_InitServer();

    BDBG_MSG((">CONFIG"));
    errCode = NEXUS_Platform_P_Config(&g_NEXUS_platformSettings);
    if ( errCode!=BERR_SUCCESS ) { errCode = BERR_TRACE(errCode); goto err_config; }

#if NEXUS_HAS_FRONTEND
    if (g_NEXUS_platformSettings.openFrontend) {
        BDBG_MSG((">FRONTEND CONFIG"));
        errCode = NEXUS_Platform_InitFrontend();
        if ( errCode!=BERR_SUCCESS ) {
            BDBG_ERR(("Unable to init frontend. Allowing Nexus to still come up."));
        }
    }
#endif

#ifdef BCHP_PWR_RESOURCE_BINT_OPEN
    BCHP_PWR_ReleaseResource(g_NEXUS_pCoreHandles->chp, BCHP_PWR_RESOURCE_BINT_OPEN);
#endif

#if NEXUS_POWER_MANAGEMENT
    /* We could be initializing after S3 cold boot, in which case we want to reset the wake up status.*/
    NEXUS_Platform_P_SetStandbySettings(&g_NEXUS_platformSettings.standbySettings, false);
#endif

    /* Success */
    if(NEXUS_GetEnv("profile_init")) {
        NEXUS_Profile_Stop("NEXUS_Platform_Init");
    }

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_TEMP_IR4000HD)
	/* IR4000HD W/A */
	NEXUS_Platform_P_ClkPllModify();
#endif

    BDBG_WRN(("initialized"));
    NEXUS_UnlockModule();
    return NEXUS_SUCCESS;

err_config:
    NEXUS_Platform_P_UninitServer();
err:
    NEXUS_Platform_P_UninitModules();
    NEXUS_Platform_P_UninitInterrupts();
err_postboard:
    NEXUS_Platform_P_UninitBoard();
err_initboard:
    NEXUS_Platform_P_UninitCore();
err_core:
    NEXUS_Platform_P_UninitOS();
err_os:
    NEXUS_UnlockModule();
    NEXUS_Module_Destroy(g_NEXUS_platformModule);
    g_NEXUS_platformModule = NULL;
err_plaform_module:
    NEXUS_Base_Uninit();
err_base:
    NEXUS_Platform_P_Magnum_Uninit();
err_magnum:
    BDBG_ASSERT(errCode); /* if we've taken this path, it's only because of failure */
    BDBG_ERR(("NEXUS_Platform_Init has failed. The system is not usable. Handles returned by NEXUS_Platform_GetConfiguration are likely to be invalid."));
    return errCode;
}

/***************************************************************************
Summary:
    Uninitialize Nexus
See Also:
    NEXUS_Platform_Init
 ***************************************************************************/
void NEXUS_Platform_Uninit(void)
{
    if (g_NEXUS_platformModule == NULL) {
        /* nexus is already uninitialized. nothing to do.
        do not use BERR or BDBG here. */
        return;
    }
    NEXUS_LockModule();

#if NEXUS_POWER_MANAGEMENT
    g_NEXUS_platformSettings.standbySettings.mode = NEXUS_PlatformStandbyMode_eOn;
    NEXUS_Platform_SetStandbySettings(&g_NEXUS_platformSettings.standbySettings);
#endif
#if NEXUS_HAS_FRONTEND
    /* shutdown whatever platform opened */
    NEXUS_Platform_UninitFrontend();
#endif

    /* close handles opened by NEXUS_Platform_P_Config (not in objdb) */
    NEXUS_Platform_P_Shutdown();

    /* close clients, close handles in objdb */
    NEXUS_Platform_P_UninitServer();

    NEXUS_UnlockModule();
    NEXUS_Base_Stop();
    NEXUS_LockModule();

#ifdef BCHP_PWR_RESOURCE_BINT_OPEN
    BCHP_PWR_AcquireResource(g_NEXUS_pCoreHandles->chp, BCHP_PWR_RESOURCE_BINT_OPEN);
#endif
    NEXUS_Platform_P_UninitModules();

    NEXUS_Platform_P_UninitInterrupts();
#ifdef BCHP_PWR_RESOURCE_BINT_OPEN
    BCHP_PWR_ReleaseResource(g_NEXUS_pCoreHandles->chp, BCHP_PWR_RESOURCE_BINT_OPEN);
#endif

    BDBG_MSG(("<UNINIT_BOARD"));
    NEXUS_Platform_P_UninitBoard();
    BDBG_MSG(("<CORE"));
    NEXUS_Platform_P_UninitCore();
    BDBG_MSG(("<OS"));
    NEXUS_Platform_P_UninitOS();

    NEXUS_UnlockModule();
    NEXUS_Module_Destroy(g_NEXUS_platformModule);
    g_NEXUS_platformModule = NULL;

    BDBG_MSG(("<BASE"));
    NEXUS_Base_Uninit();
    BDBG_MSG(("<MAGNUM"));
    NEXUS_Platform_P_Magnum_Uninit();

    return;
}

void NEXUS_Platform_GetConfiguration(NEXUS_PlatformConfiguration *pConfiguration)
{
    /* verify platform module lock is enabled */
    NEXUS_ASSERT_MODULE();
    *pConfiguration = g_NEXUS_platformHandles.config;
    BDBG_ASSERT(sizeof(pConfiguration->heap) == sizeof(g_pCoreHandles->nexusHeap));
    BKNI_Memcpy(pConfiguration->heap, g_pCoreHandles->nexusHeap, sizeof(pConfiguration->heap));
}

NEXUS_Error NEXUS_Platform_ReadRegister( uint32_t address, uint32_t *pValue )
{
    *pValue = BREG_Read32(g_pCoreHandles->reg, address);
    return 0;
}

void NEXUS_Platform_WriteRegister( uint32_t address, uint32_t value )
{
    BREG_Write32(g_pCoreHandles->reg, address, value);
}

#include "bchp_sun_top_ctrl.h"
NEXUS_Error NEXUS_Platform_GetStatus( NEXUS_PlatformStatus *pStatus )
{
    uint32_t chipId;
    BDBG_ASSERT(g_NEXUS_platformModule); /* make sure NEXUS_Platform_Init was called */

    BKNI_Memset(pStatus, 0, sizeof(*pStatus));

#ifdef BCHP_SUN_TOP_CTRL_PRODUCT_ID
    /* 40nm */
    chipId = BREG_Read32(g_pCoreHandles->reg, BCHP_SUN_TOP_CTRL_PRODUCT_ID);
    pStatus->chipRevision = chipId & 0xFFFF;
    pStatus->chipId = chipId >> 16;
#else
    /* 65nm */
    chipId = BREG_Read32(g_pCoreHandles->reg, BCHP_SUN_TOP_CTRL_PROD_REVISION);
    pStatus->chipRevision = chipId & 0xFFFF;
    pStatus->chipId = chipId >> 16;
#endif
#ifdef BCHP_SUN_TOP_CTRL_CHIP_FAMILY_ID
    pStatus->familyId = BREG_Read32(g_pCoreHandles->reg, BCHP_SUN_TOP_CTRL_CHIP_FAMILY_ID);
    if (!pStatus->chipId) {
        /* if PRODUCT_ID not burned in OTP, use FAMILY_ID */
        pStatus->chipRevision = pStatus->familyId & 0xFFFF;
        pStatus->chipId = pStatus->familyId >> 16;
    }
    pStatus->familyId = pStatus->familyId >> 16;
#else
    pStatus->familyId = pStatus->chipId;
#endif

    pStatus->chipRevision += 0x10; /* adapt to previous method of reporting chip revision: 10 = A0, 21 = B1, etc. */

    return 0;
}

#if B_REFSW_FIRMWARE
/* The following mappings simplify the rev jump */
NEXUS_Error
NEXUS_Platform_P_EnableInterrupt( unsigned irqNum)
{
    NEXUS_Error rc;
    BKNI_EnterCriticalSection();
    rc = NEXUS_Platform_P_EnableInterrupt_isr(irqNum);
    BKNI_LeaveCriticalSection();
    return rc;
}
void
NEXUS_Platform_P_DisableInterrupt( unsigned irqNum)
{
    BKNI_EnterCriticalSection();
    NEXUS_Platform_P_DisableInterrupt_isr(irqNum);
    BKNI_LeaveCriticalSection();
    return;
}
NEXUS_Error
NEXUS_Platform_P_Magnum_Init(void)
{
    /* nothing to do here, magnum already initialized when driver loaded */
    return BERR_SUCCESS;
}
void
NEXUS_Platform_P_Magnum_Uninit(void)
{
    return;
}
#endif

#ifndef NEXUS_BASE_ONLY
/* implementation of attr{shutdown=NEXUS_VideoInput_Shutdown} */
void nexus_driver_shutdown_NEXUS_VideoInput_Shutdown(void *object)
{
#if NEXUS_HAS_DISPLAY
    NEXUS_VideoInput_Shutdown(object);
#else
    BSTD_UNUSED(object);
#endif
}

/* implementation of attr{shutdown=NEXUS_VideoOutput_Shutdown} */
void nexus_driver_shutdown_NEXUS_VideoOutput_Shutdown(void *object)
{
#if NEXUS_HAS_DISPLAY
    NEXUS_VideoOutput_Shutdown(object);
#else
    BSTD_UNUSED(object);
#endif
}

/* implementation of attr{shutdown=NEXUS_AudioInput_Shutdown} */
void nexus_driver_shutdown_NEXUS_AudioInput_Shutdown(void *object)
{
#if NEXUS_HAS_AUDIO
    NEXUS_AudioInput_Shutdown(object);
#else
    BSTD_UNUSED(object);
#endif
}

/* implementation of attr{shutdown=NEXUS_AudioOutput_Shutdown} */
void nexus_driver_shutdown_NEXUS_AudioOutput_Shutdown(void *object)
{
#if NEXUS_HAS_AUDIO
    NEXUS_AudioOutput_Shutdown(object);
#else
    BSTD_UNUSED(object);
#endif
}
#endif

void NEXUS_Platform_GetDefaultClientConfiguration(NEXUS_ClientConfiguration *pConfig)
{
    BKNI_Memset(pConfig, 0, sizeof(*pConfig));
    pConfig->mode = NEXUS_ClientMode_eProtected;
    pConfig->resources.dma.total = 4;
    pConfig->resources.graphics2d.total = 2;
    pConfig->resources.graphics3d.total = 2;
    pConfig->resources.pictureDecoder.total = 2;
    pConfig->resources.playpump.total = 2;
    pConfig->resources.recpump.total = 2;
    pConfig->resources.simpleAudioPlayback.total = 2;
    pConfig->resources.stcChannel.total = 2;
    pConfig->resources.surface.total = 100;
}

void NEXUS_Platform_GetDefaultStartServerSettings( NEXUS_PlatformStartServerSettings *pSettings )
{
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    NEXUS_Platform_GetDefaultClientConfiguration(&pSettings->unauthenticatedConfiguration);
}

void NEXUS_Platform_GetDefaultClientSettings( NEXUS_ClientSettings *pSettings )
{
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    NEXUS_Platform_GetDefaultClientConfiguration(&pSettings->configuration);
}

void NEXUS_Platform_GetDefaultInterfaceName(NEXUS_InterfaceName *type)
{
    BKNI_Memset(type, 0, sizeof(*type));
    return;
}

NEXUS_Error NEXUS_Platform_P_AcquireObject(const struct b_objdb_client *client, const NEXUS_InterfaceName *type, void *object)
{
    const NEXUS_Platform_P_ModuleInfo *module_info;
    for (module_info = BLST_Q_FIRST(&g_NEXUS_platformHandles.handles); module_info; module_info = BLST_Q_NEXT(module_info, link)) {
        NEXUS_BaseObject  *base_object = b_objdb_find_object_in_module_and_lock(module_info->module, client, type->name, object);
        if(base_object) {
            NEXUS_BaseObject_P_Acquire_Tagged(NULL, object, base_object->descriptor, NULL, __FILE__, __LINE__);
            NEXUS_Module_Unlock(module_info->module);
            return NEXUS_SUCCESS;
        }
    }
    return BERR_TRACE(NEXUS_INVALID_PARAMETER);
}

void NEXUS_Platform_P_ReleaseObject(const NEXUS_InterfaceName *type, void *object)
{
    const NEXUS_Platform_P_ModuleInfo *module_info;
    for (module_info = BLST_Q_FIRST(&g_NEXUS_platformHandles.handles); module_info; module_info = BLST_Q_NEXT(module_info, link)) {
        NEXUS_BaseObject  *base_object = b_objdb_find_object_in_module_and_lock(module_info->module, NULL, type->name, object);
        if(base_object) {
            NEXUS_BaseObject_P_Release_Tagged(NULL, object, base_object->descriptor, NULL, __FILE__, __LINE__);
            NEXUS_Module_Unlock(module_info->module);
            return ;
        }
    }
    (void)BERR_TRACE(NEXUS_INVALID_PARAMETER);
    return ;
}

