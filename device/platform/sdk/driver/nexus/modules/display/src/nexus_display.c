
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
 * $brcm_Workfile: nexus_display.c $
 * $brcm_Revision: 212 $
 * $brcm_Date: 10/10/12 10:32a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/display/src/nexus_display.c $
 * 
 * 212   10/10/12 10:32a erickson
 * SW7408-346: support graphics-only build
 * 
 * 211   9/25/12 12:49p erickson
 * SW7425-3668: clear video decoders intId fields in
 *  NEXUS_Display_DisconnectVideoInput
 * 
 * 210   9/20/12 10:24a gmohile
 * SW7429-240 : Use STG only for vice encoder
 * 
 * 209   9/18/12 10:04a erickson
 * SW7435-344: restore code
 * 
 * 207   8/1/12 2:44p erickson
 * SW7425-3321: fix swapped ulCrcCb and ulCrcCr
 * 
 * 206   7/30/12 11:19a erickson
 * SW7425-3573: add specific error codes fro NEXUS_Display_AddOutput and
 *  RemoveOutput
 * 
 * 205   7/23/12 11:35a vsilyaev
 * SW7420-2328: Use per-thread storage to keep information about the
 *  current client, remove per-module client information
 * 
 * 204   7/12/12 4:03p erickson
 * SW7405-5488: add NEXUS_GetDisplayCapabilities
 * 
 * 203   7/9/12 3:00p erickson
 * SW7435-259: remove unused window param from NEXUS_VideoInput_P_Connect
 * 
 * 202   7/3/12 5:11p bandrews
 * SW7425-717: merge to main
 * 
 * 201   6/28/12 3:35p hongtaoz
 * SW7425-3364: removed reference to the deprecated ulVertFreqMask;
 * 
 * 200   6/26/12 3:57p gmohile
 * SW7231-792 : Merge 720p encode support
 * 
 * 199   6/19/12 12:42p vsilyaev
 * SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
 * 
 * SW7425-717/1   6/4/12 5:59p bandrews
 * SW7425-717: update to fix some bugs and get correct status
 * 
 * 198   5/18/12 12:04p mward
 * SW7435-165: Run-time selection of transcode use cases
 * 
 * 197   5/15/12 3:02p erickson
 * SW7346-792: only resolve NEXUS_DisplayAspectRatio_eAuto when converting
 *  to magnum types to remove ambiguity
 * 
 * 196   4/20/12 11:36a gmohile
 * SW7231-782 : Check if encoder callback is enabled before capturing
 *  frames
 * 
 * 195   4/13/12 5:01p erickson
 * SW7346-737: add vsync isr for HDMI Tx CRC
 *
 * 194   4/11/12 2:51p hongtaoz
 * SW7435-100: added single/dual xcode usage support;
 *
 * 193   4/11/12 11:23a hongtaoz
 * SW7425-2864: apply VDC STG display callback setting properly;
 *
 * 192   3/15/12 2:29p gmohile
 * SW7231-584,SW7231-586: Disable frame drop in nexus for DSP video encode
 *
 * 191   3/14/12 12:25p erickson
 * SW7401-4517: remove old code
 *
 * 190   3/9/12 2:03p gmohile
 * SW7231-584,SW7231-586: Fix go bit interrupt bit mask
 *
 * 189   3/7/12 3:15p gmohile
 * SW7231-586,SW7231-584 : go bit support for raaga encoder
 *
 * 188   3/6/12 5:32p hongtaoz
 * SW7435-50: bringup quad transcoders;
 *
 * 187   2/23/12 2:31p hongtaoz
 * SW7425-2043: mask XUDlib display callback if the STG picture ID doesn't
 *  change;
 *
 * 186   2/23/12 11:18a gmohile
 * SW7425-1473 : synchronize encoder callback settings with isr
 *
 * 185   2/2/12 2:34p gmohile
 * SW7231-587 : Pass original PTS to raaga encoder
 *
 * 184   1/31/12 5:01p gmohile
 * SW7425-1473 : Fix raaga encoder support
 *
 * 183   1/27/12 12:00p erickson
 * SW7425-2139: extend NEXUS_DisplayStatus
 *
 * 182   1/26/12 11:49a mward
 * SW7125-1222:  Set display3DSettings.sourceBuffer in
 *  NEXUS_Display_GetDefaultSettings().
 *
 * 181   1/24/12 10:50a erickson
 * SWNOOS-511: stop VDC isr callbacks at the top of NEXUS_Display_Close
 *
 * 180   1/20/12 1:16p erickson
 * SW7425-2216: default deinterlacing on, assuming 1 MAD block for now
 *
 * 179   1/17/12 10:58a dlwin
 * SW7344-260: Fix for Coverity 36606, FORWARD_NULL issue.
 *
 * 178   1/16/12 6:04p gmohile
 * SW7425-1473 : Merge raaga encoder support
 *
 * 177   1/12/12 12:02p jtna
 * SW7425-1875: merge NEXUS_DisplaySettings.display3DSettings.sourceBuffer
 *
 * 176   12/16/11 7:07p hongtaoz
 * SW7425-1878: fixed a typo for NEXUS_HAS_VIDEO_ENCODER;
 *
 * 175   12/16/11 6:42p hongtaoz
 * SW7425-1878: fixed segfault when stopped user data transcode;
 *
 * 174   12/16/11 5:28p erickson
 * SW7425-1878: merge XUDlib support for video encoder userdata
 *
 * SW7425-1878/6   12/16/11 5:23p ajagmag
 * SW7425-1878: Change prototype of private function to use real type
 *  instead of void *
 *
 * SW7425-1878/5   12/16/11 5:13p ajagmag
 * SW7425-1878: Updated for easier merge
 *
 * SW7425-1878/4   12/16/11 2:10p ajagmag
 * SW7425-1878: Update based on discussions with David E.
 *
 * SW7425-1878/3   12/15/11 7:19a ajagmag
 * SW7425-1878: Bug fixes for user data encode use case
 *
 * SW7425-1878/2   12/13/11 4:12p ajagmag
 * SW7425-1878: Removed stgRate from XUDlib settings.
 *
 * SW7425-1878/1   12/9/11 1:32p ajagmag
 * SW7425-1878: Integrate XUDlib into nexus
 *
 * 173   12/16/11 10:17a erickson
 * SW7425-1877: clarify VideoInput connection logic, add TODO's for future
 *  simplification
 *
 * 172   12/9/11 5:22p bandrews
 * SW7550-772: merge to main
 *
 * SW7550-772/1   12/8/11 8:58p bandrews
 * SW7550-772: implement default timebase usage
 *
 * 171   11/15/11 3:27p erickson
 * SW7425-1747: remove old code
 *
 * 170   10/31/11 7:46p bandrews
 * SW7231-391: merge to main
 *
 * SW7420-2078/3   10/28/11 4:06p bandrews
 * SW7231-391: update timebase to use pointers
 *
 * SW7420-2078/2   10/27/11 7:56p bandrews
 * SW7231-391: merge from main
 *
 * 169   10/27/11 3:32p erickson
 * SW7425-1581: move crcQueueSize to NEXUS_DisplaySettings
 *
 * 168   10/20/11 1:43p erickson
 * SW7420-1992: add private api's for atomically setting multiple video
 *  window settings
 *
 * 167   10/20/11 10:44a erickson
 * SW7425-1546: set display->status.refreshRate when format changes
 *
 * 166   10/19/11 4:59p hongtaoz
 * SW7425-1546: update display refresh rate according to current display
 *  status; set stgIndex correctly for dual;
 *
 * 165   10/19/11 3:10p erickson
 * SW7425-1546: call NEXUS_Display_P_VideoInputDisplayUpdate for display
 *  refresh rate changes, including drop-frame/non-drop-frame changes
 *
 * 164   10/18/11 6:15p jtna
 * SW7425-1531: add helper code to resize video windows when switching
 *  between half-res 3D formats
 *
 * 163   10/13/11 5:27p gmohile
 * SW7420-2006 : Apply changes after add/remove outputs during standby
 *
 * 162   9/29/11 12:41p erickson
 * SW7425-1320: fix prev checkin
 *
 * 161   9/29/11 12:38p erickson
 * SW7425-1320: set vdcDisplayCfg.eMasterTg to eStg0 or eStg1 based on
 *  optional NEXUS_ENCODER_DISPLAY_IDX/ENCODER0/ENCODER1 macros
 *
 * 160   9/20/11 6:51p dlwin
 * SW7231-349: Fix for Coverity: 35291
 *
 * 159   9/14/11 1:08p erickson
 * SW7425-1232: don't have NEXUS_Display_P_SetSettings overwrite SAR
 *  aspect ratio set by NEXUS_Display_SetCustomFormatSettings
 *
 * 158   9/2/11 12:40p erickson
 * SW7420-1995: unregister handles from objdb when doing automatic close
 *
 * 157   8/5/11 4:11p erickson
 * SW7420-1148: change WRN to ERR if display auto-closes window handles.
 *  add comment explaining why this is an error and the app must be fixed.
 *
 * 156   7/28/11 3:58p erickson
 * SW7405-5339: reapply lost fixes from /main/144
 *
 * 155   7/27/11 11:34a mward
 * SW7346-324:  Don't lose outstanding error return code when checking for
 *  error from BVDC_AbortChanges().
 *
 * 154   7/21/11 5:48p jtna
 * SW7346-324: Coverity defect UNCHECKED RETURN
 *
 * 153   7/21/11 4:13p gmohile
 * SW7125-1014 : Rework power management
 *
 * 152   7/19/11 11:14a erickson
 * SW7408-291: move vf filter code to video_output
 *
 * 151   7/8/11 4:13p erickson
 * SW7425-592: add NEXUS_DisplayAspectRatio_eSar for custom display
 *  formats
 *
 * 149   7/7/11 10:57a erickson
 * SW7405-5394: move BVDC_Window_SetMasterFrameRate logic into
 *  NEXUS_VideoWindow_P_ConfigMasterFrameRate, which is called from all
 *  places where it could change.
 *
 * 148   7/7/11 10:12a erickson
 * SW7408-291: remove NEXUS_Display_SetVfFilter impl until it is worked
 *  out
 *
 * 147   7/7/11 10:07a erickson
 * SW7405-5394: NEXUS_DisplayModule_SetAutomaticPictureQuality should only
 *  set ANR when supported. avoid unnecessary errors.
 *
 * 146   6/28/11 9:47a erickson
 * SW7405-5366: add NEXUS_Display_GetStatus
 *
 * 145   6/23/11 3:36p erickson
 * SW7408-291: add NEXUS_Display_SetVfFilter
 *
 * 143   6/16/11 3:41p erickson
 * SW7425-101: use #include bfmt_custom.h instead of copying struct
 *
 * 142   6/7/11 4:56p hongtaoz
 * SW7425-64: don't hack displayRect of custom format; otherwise
 *  fullscreen window size might not be updated when custom format size is
 *  larger than previous;
 *
 * 141   5/27/11 2:42p erickson
 * SW7425-652: add NEXUS_DisplayStgSettings
 *
 * 140   5/4/11 12:00p vsilyaev
 * SW7425-486: Adjust display/canvas width/height for half resolution 3D
 *  formats
 *
 * 139   4/22/11 5:43p vsilyaev
 * SW7425-416: Reworked settings of the display orientation
 *
 * 138   4/22/11 3:13p vsilyaev
 * SW7425-416: Updated 3D display API
 *
 * 137   4/21/11 1:17p vsilyaev
 * SW7425-404: Added API to override orientaion of 2D video format
 *
 * 136   4/20/11 6:11p jtna
 * SW7425-365: fix build warnings for 2.6.37 kernel
 *
 * 135   4/7/11 7:37p hongtaoz
 * SW7425-310: defer custom format change;
 *
 * 134   3/31/11 3:58p gmohile
 * SW7408-260 : Open/close VDC handles during resume/standby
 *
 * 133   3/28/11 3:45p vsilyaev
 * SW7335-1214: Added NEXUS_CallbackHandler framework
 *
 * 132   3/11/11 4:13p erickson
 * SW7346-103: add support for CMP CRC capture
 *
 * 131   3/7/11 11:36a jtna
 * SW7422-221: make use of NEXUS_VideoFormatInfo.isFullRes3d
 *
 * 130   2/25/11 2:52p gmohile
 * SW7408-210 : Deprecate use of NEXUS_POWER_STANDBY
 *
 * 129   2/24/11 1:47p gmohile
 * SW7408-210 : Fix warning
 *
 * 128   2/24/11 12:01p gmohile
 * SW7408-210 : Add BVN standby
 *
 * 127   2/22/11 4:41p erickson
 * SW7425-64: temporarily set ulVertFreqMask, considering dropFrameAllowed
 *
 * 126   2/18/11 2:08p erickson
 * SW7425-64: update NEXUS_DisplayCustomFormatSettings for transcoding
 *  settops
 *
 * 125   2/16/11 4:43p gmohile
 * SW7408-210 : Fix warning
 *
 * 124   2/16/11 10:59a katrep
 * SW7408-210:fixed compiler warning
 *
 * 123   2/15/11 5:33p gmohile
 * SW7408-210 : Merge initial version of module standby
 *
 * SW7408-210/2   2/11/11 5:04p gmohile
 * SW7408-210 : Fix active and pasive standby
 *
 * SW7408-210/1   1/21/11 1:05p gmohile
 * Sw7408-210 : Add standby support
 *
 * 122   1/3/11 11:03a erickson
 * SW3548-1660: switch to BVDC_Display_GetInterruptName
 *
 * 121   12/23/10 12:50p erickson
 * SW7425-34: remove NEXUS_IS_ANALOG_DISPLAY. instead, use
 *  BAVC_VbiPath_eUnknown returned by BVDC_Display_GetVbiPath to determine
 *  if display is VBI capable
 *
 * 120   12/23/10 10:27a erickson
 * SW7550-577: for STB LVDS, nexus display 0 is VDC display 0
 *
 * 119   12/17/10 11:21p hongtaoz
 * SW7425-34: STG display doesn't have VBI block;
 *
 * 118   12/2/10 8:13p spothana
 * SW7420-1177: Adding display rate change callback for DVO output.
 *
 * SW7420-1177/1   11/5/10 3:28p spothana
 * SW7420-1177: Adding display rate change callback for DVO output.
 *
 * 117   12/1/10 4:27p erickson
 * SW7550-577: add NEXUS_DisplayTimingGenerator
 *
 * 116   10/15/10 9:14a erickson
 * SW35230-1761: NEXUS_Display_SetSettings() causes multiple
 *  BINT_CreateCallback
 *
 * 115   7/22/10 10:25a petlee
 * SW35230-661: Update default settings for 35230
 *
 * 114   7/9/10 3:57p bandrews
 * SW7335-794: alignment needs to take place if target changes
 *
 * 113   6/11/10 3:53p petlee
 * SW35230-249: Merging to main
 *
 * SW35230-249/4   6/9/10 4:12p petlee
 * SW35230-249: Minor update
 *
 * SW35230-249/3   6/5/10 5:05p rgreen
 * SW3548-2323: Merge to 35230 branch
 *
 * 112   6/1/10 9:39p bandrews
 * SW3548-2323: unlinked work on 7420 with 35230, input and display only
 *  tuned on latter
 *
 * SW35230-249/2   6/1/10 6:00p petlee
 * SW35230-249: Remove call to ANR as it is not brought up in VDC and
 *  causes run-time error. This is a temp fix.
 *
 * SW35230-249/1   5/30/10 8:20p petlee
 * SW35230-249: Update digital and lvds output for 35230.
 *
 * 111   5/5/10 7:27p bandrews
 * SW3548-2323: updated to apply changes if update mode says so
 *
 * 110   4/14/10 5:29p jtna
 * SW7125-310: Coverity Defect ID:20351 DEADCODE. Condition is not dead if
 *  NEXUS_NUM_VIDEO_WINDOWS>1
 *
 * 109   3/30/10 6:39p bandrews
 * SW3548-2323: proxy code generator doesn't like arrays as parameters to
 *  public APIs
 *
 * 108   3/30/10 4:49p bandrews
 * SW3548-2323: merge to main, need to match header
 *
 * 107   3/29/10 3:33p bandrews
 * SW3548-2323: merge to main
 *
 * SW3548-2323/6   1/7/10 5:35p bandrews
 * merge from latest main
 *
 * 106   3/12/10 4:05p erickson
 * SW3548-2643: added NEXUS_PanelOutput_P_ApplyCurrentStatus
 *
 * 105   1/6/10 4:18p erickson
 * SW7405-3533: move window init code into NEXUS_VideoWindow_P_InitState.
 *  this allows normal and mosaic windows to init to the same defaults.
 *
 * SW3548_2323/5   12/15/09 8:54p bandrews
 * SW3548-2323: updated per bvdc_tune.h
 *
 * SW3548_2323/4   12/15/09 8:50p bandrews
 * SW3548-2323: updated from VDC team's vdc_tune impl
 *
 * SW3548_2323/3   12/15/09 8:32p bandrews
 * SW3548-2323: merge from latest
 *
 * 104   12/9/09 2:23p erickson
 * SW7405-3394: remove unnecessary NEXUS_VideoWindow_P_Teardown, add
 *  better warning for implicit window close
 *
 * 103   11/17/09 3:56p erickson
 * SW3556-852: fix warning
 *
 * 102   11/17/09 3:54p erickson
 * SW3556-852: add NEXUS_DisplayModule_SetAutomaticPictureQuality
 *
 * 101   11/12/09 10:50a erickson
 * SW3548-2616: refactor NEXUS_Display_P_InitGraphics and related code
 *
 * SW3548_2323/2   9/28/09 8:23p bandrews
 * 3548-2323: initial check-in of libtune port
 *
 * SW3548_2323/1   9/22/09 6:50p bandrews
 * SW3548-2323: libTune nexus passthrough
 *
 * 100   9/17/09 9:38a erickson
 * SW7405-3024: introduce NEXUS_DisplayAspectRatio_eAuto so that users can
 *  set 4x3 720p/1080i if desired.
 *
 * 99   9/9/09 1:19p erickson
 * SW7405-2977: allow build with no video decoder
 *
 * 98   8/21/09 3:42p erickson
 * PR57927: convert BDBG_OBJECT_INIT to BKNI_Memset(0) and BDBG_OBJECT_SET
 *  for NEXUS_VideoWindow.
 *
 * 97   8/17/09 2:28p erickson
 * PR55232: backing out last checkin
 *
 * 96   8/12/09 10:07p jrubio
 * PR55232: add 7340/7342
 *
 * 95   8/10/09 10:09a erickson
 * PR56325: only feedback display information to sync-locked inputs
 *
 * 94   8/10/09 8:17a gmohile
 * PR 56400 : Merge legacy vdc support to main line
 *
 * 93   8/4/09 2:07p erickson
 * PR57106: remove hardcoded BAVC_MatrixCoefficients_eDvi_Full_Range_RGB
 *  in favor of NEXUS_PanelOutputSettings.matrixCoefficients with same
 *  default value
 *
 * 92   7/20/09 3:50p erickson
 * PR56932: switch MAD and ANR pixel formats to eUnknown. this allows VDB
 *  to set the optimal format.
 *
 * 91   7/10/09 3:57p erickson
 * PR56558: add NEXUS_DisplayModule_SetVideoDecoderModule to allow faster
 *  start up time
 *
 * 90   6/15/09 5:01p erickson
 * PR56022: deprecated numWindows and numDisplays
 *
 * 89   6/15/09 12:18p erickson
 * PR55928: added NEXUS_DisplaySettings.vecIndex
 *
 * 88   6/12/09 11:13a erickson
 * PR55952: handle default aspect ratio for 1920x1080 display formats in a
 *  consistent way
 *
 * 87   6/8/09 7:05a erickson
 * PR55617: rename NEXUS_P_DisplayAspectRatio_ToMagnum
 *
 * 86   5/20/09 11:29a erickson
 * PR55292: call NEXUS_VideoOutput_P_DestroyLink in
 *  NEXUS_Display_RemoveOutput
 *
 * 85   4/28/09 10:46p bandrews
 * PR54526: Fix format change call info sync; fix static rate mismatch
 *  detection
 *
 * 84   4/6/09 5:59p jgarrett
 * PR 50884: Storing display settings prior to apply changes for VDB
 *
 * 83   3/26/09 9:23a erickson
 * PR51925: allow 3548 to configure main display for VEC output
 *
 * 82   3/23/09 1:44p erickson
 * PR53425: add NEXUS_DisplaySettings.vsyncCallback
 *
 * 81   3/19/09 2:33p erickson
 * PR53151: don't free memory for NEXUS_Display_SetCustomFormatSettings
 *  until NEXUS_Display_Close
 *
 * 80   3/6/09 7:38p jgarrett
 * PR 48984: Always calling ApplyChanges even when VDB fails
 *
 * 79   2/26/09 2:32p jgarrett
 * PR 52464: Adding RTS settings update on display format change
 *
 * 78   2/23/09 3:03p erickson
 * PR52406: fix deep copy in NEXUS_DisplayCustomFormatSettings
 *
 * 77   2/11/09 3:17p erickson
 * PR52032: remove NEXUS_NUM_BYPASS_DISPLAYS
 *
 * 76   2/9/09 12:18p erickson
 * PR51925: add support for secondary SD display on 3548/3556
 *
 * 75   1/26/09 10:04a erickson
 * PR51578: remove unnecessary #if NEXUS_DTV_PLATFORM
 *
 * 74   1/14/09 12:33p erickson
 * PR46300: fix non-DTV
 *
 * 73   1/13/09 6:15p erickson
 * PR46300: added NEXUS_VideoWindow_SetSpecialFeature1Settings
 *
 * 72   1/12/09 12:29p erickson
 * PR50979: rename NEXUS_Display_ConnectVideoInput, clarify comments
 *
 * 71   1/8/09 2:33a erickson
 * PR48984: fix warnings
 *
 * 70   1/6/09 7:46p erickson
 * PR50749: added NEXUS_Display_DisconnectVideoDecoder for tearing down
 *  NEXUS_Display_DriveVideoDecoder
 *
 * 69   1/6/09 11:32a jgarrett
 * PR 48984: Merge dynamic RTS code to main branch
 *
 * 68   12/24/08 1:07p jgarrett
 * PR 50703: Disabling VBI on 7420
 *
 * 67   12/22/08 12:57p nickh
 * PR48963: Temporarily remove vbi components for 7420 bringup
 *
 * PR48984/2   11/26/08 6:19p jgarrett
 * PR 48984: Merging to latest baseline
 *
 * 66   11/24/08 10:15a erickson
 * PR49623: allow vbi encoding on bypass display
 *
 * 65   11/20/08 12:50p erickson
 * PR48944: improve error messages
 *
 * 64   11/17/08 12:28p erickson
 * PR49203: ensure there are no unnecessary unguarded BVDC_ApplyChanges
 *  calls in Nexus
 *
 * PR48984/1   11/20/08 6:47p jgarrett
 * PR 48984: Adding VDB restrictions
 *
 * 63   11/11/08 5:32p erickson
 * PR 48757: default xvYccEnabled to true for DTV
 *
 * 62   11/3/08 3:40p erickson
 * PR46063: added NEXUS_DisplayCustomFormatSettings.interlaced
 *
 * 61   10/29/08 11:38a erickson
 * PR48028: added deringing and dejagging options to
 *  NEXUS_VideoWindowScalerSettings
 *
 * 60   10/22/08 3:49p erickson
 * PR46300: init test feature
 *
 * 59   10/6/08 11:38p erickson
 * PR44801: merge
 *
 * PR44801/1   9/29/08 9:00p bandrews
 * PR44801: Added support for display VEC alignment
 *
 * 58   10/2/08 12:47p erickson
 * PR40799: remove unneccessary Close code
 *
 * 57   9/29/08 9:26a erickson
 * PR46063: don't reinit graphics in _P_SetSettings for custom formats
 *  when not needed
 *
 * 56   9/24/08 12:22p erickson
 * PR46063: init graphics for custom display formats
 *
 * 55   9/23/08 4:40p erickson
 * PR46063: remove unnecessary warning
 *
 * 54   9/22/08 4:28p erickson
 * PR46063: fix platforms that don't have custom format api's
 *
 * 53   9/22/08 9:54a erickson
 * PR46063: support getting format info re: Custom2 inside and outside
 *  nexus
 *
 * 51   9/18/08 11:02a erickson
 * PR47014: update blend equation enums
 *
 * 50   9/17/08 12:29p erickson
 * PR47014: expose compositor blend equation in NEXUS_VideoWindowSettings
 *  and NEXUS_GraphicsSettings
 *
 * 49   9/8/08 3:03p erickson
 * PR46527: intial checkin of test code to route from AVD->M2MC->GFD
 *
 * 48   9/3/08 1:33p erickson
 * PR46254: added NEXUS_VideoWindowSettings.alpha
 *
 * 47   9/2/08 10:27p katrep
 * PR46457: Configure nexus features at the run time based on board strap
 *  options
 *
 * 46   8/21/08 2:17p erickson
 * PR44792: shut down vbi on Close
 *
 * 45   8/19/08 12:22p erickson
 * PR45794: clarify internal function names. add ASSERT's and comments.
 *
 * 44   8/14/08 5:25p katrep
 * PR45674: Fix compiiler warning in kernel mode non debug builds
 *
 * 43   8/13/08 1:25p erickson
 * PR45640: fix bounds for window in lower right corner. a 482/480 fudge
 *  was required.
 *
 * 42   8/8/08 5:33p vishk
 * PR 45390: Nexus VBI reports "No CloseCaption buffer" when VBI not
 *  configured.
 *
 * 41   7/24/08 2:45p erickson
 * PR44744: auto call of NEXUS_VideoInput_Shutdown is needed in some cases
 *
 * 40   7/16/08 3:30p erickson
 * PR44764: default bandwidthEquationParams are now zero
 *
 * 39   7/11/08 10:25a erickson
 * PR44744: fix NEXUS_Display_Close bug
 *
 * 38   7/3/08 4:22p erickson
 * PR44452: NEXUS_Display_Close on the last display should bring the
 *  display module to a clean state. This makes implicit close for a
 *  kernel mode driver possible.
 *
 * 37   6/25/08 11:44a erickson
 * PR39453: moved xvYccEnabled to NEXUS_DisplaySettings
 *
 * 36   6/23/08 10:48a erickson
 * PR43611: impl bypass display on 3548/3556
 *
 * 35   6/20/08 9:21a erickson
 * PR43917: default forceCapture = false for DTV
 *
 * 34   6/18/08 10:04a erickson
 * PR43617: extended NEXUS_VideoWindowSclSettings with bandwidth equation
 *  params
 *
 * 33   6/16/08 12:17p erickson
 * PR43699: Remove VDC Get's from NEXUS GetSettings calls to avoid race
 *  conditions
 *
 * 32   5/23/08 10:49a erickson
 * PR42982: rework PQ Dirty logic to reapply settings on channel change
 *
 * 31   5/23/08 9:14a erickson
 * PR42982: set good default NEXUS_VideoWindowMadSettings
 *
 * 30   5/21/08 11:29a erickson
 * PR42678: fix NEXUS_DisplayAspectRatio_e16x9 default for DTV
 *
 * 29   5/14/08 2:31p erickson
 * PR39257: fix NEXUS_Display_SetSettings for bypass and lvds types
 *
 * 28   5/12/08 1:59p erickson
 * PR42266: set display A/R to 16x9 for all HD formats. the comments on
 *  A/R says that it applies "if applicable".
 *
 * 27   5/5/08 4:48p erickson
 * PR42445: provide API's for better display frame rate control
 *
 * 26   5/5/08 10:20a erickson
 * PR42322: must disconnect input if BVDC_ApplyChanges fails
 *
 * 25   5/1/08 12:57p erickson
 * PR41202: add NEXUS_VideoWindowSettings.forceCapture
 *
 * 24   4/8/08 1:53p erickson
 * PR39453: add 3548 & 3556
 *
 * 23   4/4/08 4:06p erickson
 * PR39257: add NEXUS_DisplayType_eBypass for DTV
 *
 * 22   4/3/08 10:17a erickson
 * PR41122: move picture quality code to 7400
 *
 * 21   4/2/08 2:47p erickson
 * PR40950: move PictureCtrl custom data into function params
 *
 * 20   4/1/08 4:40p erickson
 * PR40606: fix chips with no hdmi output
 *
 * 19   4/1/08 1:46p jgarrett
 * PR 40606: Caching rate information for HDMI
 *
 * 18   3/26/08 3:38p vsilyaev
 * PR 40862: Added code to propagate display framerate to the decoder
 *
 * 17   3/26/08 2:40p erickson
 * PR40950: set custom ANR/DNR data in custom function
 *
 * 16   3/26/08 1:20p vsilyaev
 * PR 40862: Fixed support for 1080p formats
 *
 * 15   3/20/08 3:00p erickson
 * PR40411: add letterBoxDetectionChange and GetLetterBoxStatus
 *
 * 14   3/19/08 11:46a erickson
 * PR34662: 7405 B0 PIP was segfaulting in VDC. Memset of
 *  NEXUS_VideoWindowSettings solved problem.
 *
 * 13   3/3/08 10:34a erickson
 * PR40156: don't apply a window color matrix unless Set
 *
 * 12   2/28/08 10:15a erickson
 * PR36808: fix calls to BVDC_Display_SetTimebase and
 *  BVDC_Window_SetMasterFrameRate
 *
 * 11   2/27/08 10:40a erickson
 * PR39952: don't enable VBI encoding for panel displays
 *
 * 10   2/22/08 4:54p erickson
 * PR39889: fix static analysis warning
 *
 * 9   2/19/08 2:58p erickson
 * PR39736: fix memory leak on failure
 *
 * 8   2/5/08 10:13a erickson
 * PR38679: change private FindWindow
 *
 * 7   2/1/08 5:34p jgarrett
 * PR 39017: Adding HdmiOutput
 *
 * 7   2/1/08 5:25p jgarrett
 * PR 39017: Adding HdmiOutput
 *
 * 6   2/1/08 3:20p erickson
 * PR38679: add vbi support
 *
 * 5   1/31/08 9:55a erickson
 * PR36808: added SyncChannel interface
 *
 * 4   1/23/08 9:22p vobadm
 * PR35457: update docs
 *
 * 3   1/23/08 12:39p erickson
 * PR38919: rename NEXUS_Display_GetWindow to NEXUS_VideoWindow_Open
 *
 * 2   1/21/08 3:50p erickson
 * PR38862: added NEXUS_DisplayType_eLvds which is the default. use
 * DisplayType to select DVO or LVDS.
 *
 * 1   1/18/08 2:20p jgarrett
 * PR 38808: Merging to main branch
 *
 **************************************************************************/
#include "nexus_base.h"
#include "nexus_display_module.h"
#include "nexus_class_verification.h"
#include "priv/nexus_display_priv.h"
#if NEXUS_HAS_TRANSPORT
#include "priv/nexus_timebase_priv.h"
#endif

BDBG_MODULE(nexus_display);

#define pVideo (&g_NEXUS_DisplayModule_State)
#define B_REFRESH_RATE_10_TO_1000(RATE) (((RATE) == 2397) ? 23976 : (RATE) * 10)

#define NEXUS_DISPLAY_ENCODER_MAX_PICTURE_BUFFERS 16

static NEXUS_Error NEXUS_Display_P_ConfigVecInterrupts( NEXUS_DisplayHandle display, const NEXUS_DisplaySettings *pSettings );
static void nexus_display_p_refresh_rate_event(void *context);

void
NEXUS_Display_GetDefaultSettings(NEXUS_DisplaySettings *pSettings)
{
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    /* normally nexus prefers default values which also happen to be zero. but eAuto was added to the end of the list
    so that the beginning of the list can track with VDC */
    pSettings->timingGenerator = NEXUS_DisplayTimingGenerator_eAuto;
    pSettings->displayType = NEXUS_DisplayType_eAuto;
    pSettings->format = NEXUS_VideoFormat_eNtsc;
    pSettings->aspectRatio = NEXUS_DisplayAspectRatio_eAuto; /* This needs to be eAuto. If the user changes the format to 720p et al. but not
                                                                the aspectRatio, they will likely expect the display a/r to change. */
    pSettings->timebase = NEXUS_Timebase_eInvalid;
    pSettings->vecIndex = -1;
    pSettings->display3DSettings.overrideOrientation = false;
    pSettings->display3DSettings.orientation = NEXUS_VideoOrientation_e2D;
    pSettings->display3DSettings.sourceBuffer = NEXUS_Display3DSourceBuffer_eDefault;
    return;
}

NEXUS_Error NEXUS_P_Display_GetMagnumVideoFormatInfo(NEXUS_DisplayHandle display, NEXUS_VideoFormat videoFormat, BFMT_VideoInfo *pInfo)
{
    BFMT_VideoFmt formatVdc;

    if (videoFormat == NEXUS_VideoFormat_eCustom2) {
        if (display->customFormatInfo) {
            *pInfo = *display->customFormatInfo;
            return 0;
        }
        else {
            return BERR_TRACE(NEXUS_INVALID_PARAMETER);
        }
    }
    else {
        NEXUS_Error rc;
        rc = NEXUS_P_VideoFormat_ToMagnum(videoFormat, &formatVdc);
        if (rc) {return BERR_TRACE(rc);}
        rc = BFMT_GetVideoFormatInfo( formatVdc, pInfo );
        if (rc) {return BERR_TRACE(rc);}
    }
    return 0;
}

#if NEXUS_NUM_DISPLAYS > 1
NEXUS_Error NEXUS_Display_P_Align( NEXUS_DisplayHandle display, NEXUS_DisplayHandle target )
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    BERR_Code mrc = BERR_SUCCESS;
    BVDC_Display_AlignmentSettings sAlignSettings;
    BVDC_Display_Handle hTargetDisplay;

    BDBG_OBJECT_ASSERT(display, NEXUS_Display);

    BVDC_Display_GetAlignment(display->displayVdc, &hTargetDisplay, &sAlignSettings);

    if (target)
    {
        sAlignSettings.eDirection = BVDC_AlignmentDirection_eAuto;
        hTargetDisplay = target->displayVdc;
        BDBG_MSG(("enable display %p alignment to display %p",  display->displayVdc, hTargetDisplay));
    }
    else
    {
        hTargetDisplay = NULL;
        BDBG_MSG(("disable display %p alignment",  display->displayVdc));
    }

    mrc = BVDC_Display_SetAlignment(display->displayVdc, hTargetDisplay, &sAlignSettings);
    if (mrc!=BERR_SUCCESS) { rc = BERR_TRACE(mrc);goto end;}

end:

    return rc;
}
#endif

/*
Summary:
Returns bits 'e'..'b' from word 'w',

Example:
    B_GET_BITS(0xDE,7,4)==0x0D
    B_GET_BITS(0xDE,3,0)=0x0E
*/
#define B_GET_BITS(w,e,b)  (((w)>>(b))&(((unsigned)(-1))>>((sizeof(unsigned))*8-(e+1-b))))

static BERR_Code
NEXUS_Display_P_SetSettings(NEXUS_DisplayHandle display, const NEXUS_DisplaySettings *pSettings, bool force)
{
    BERR_Code rc;
    unsigned i;

    force = force || pVideo->lastUpdateFailed;

    /* NOTE: display->cfg are the old settings. pSettings are the new settings. always apply pSettings, not display->cfg. */

    if (force ||
                (display->cfg.format!=pSettings->format) ||  /* changes in the format */
                (display->cfg.display3DSettings.overrideOrientation != pSettings->display3DSettings.overrideOrientation) ||  /* changes in the overrideOrientation */
                (pSettings->display3DSettings.overrideOrientation && (display->cfg.display3DSettings.orientation != pSettings->display3DSettings.orientation)) /* changes in the orientation */
                ) {
        BFMT_VideoInfo video_format_info;
        NEXUS_Rect newDisplayRect = {0,0,0,0};

        rc = NEXUS_P_Display_GetMagnumVideoFormatInfo(display, pSettings->format, &video_format_info);
        if (rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc); goto err_format; }

        newDisplayRect.width = video_format_info.ulDigitalWidth;
        if (pSettings->displayType == NEXUS_DisplayType_eBypass) {
            newDisplayRect.height = video_format_info.ulHeight;
        }
        else {
            newDisplayRect.height = video_format_info.ulDigitalHeight;
        }
        if(pSettings->display3DSettings.overrideOrientation) {
            if(pSettings->display3DSettings.orientation == NEXUS_VideoOrientation_e3D_LeftRight) {
                newDisplayRect.width /= 2;
            } else if(pSettings->display3DSettings.orientation == NEXUS_VideoOrientation_e3D_OverUnder) {
                newDisplayRect.height /= 2;
            }
        }

        /* if the display format dimensions change, we need to disable graphics. the current graphics will not look right. */
        if (!NEXUS_P_Display_RectEqual(&display->displayRect, &newDisplayRect)) {
            display->displayRect = newDisplayRect;
            NEXUS_Display_P_ResetGraphics(display); /* destroys graphics and resets the graphics settings with current displayRect */
        }

        if (pSettings->format != NEXUS_VideoFormat_eCustom2) {
            rc = BVDC_Display_SetVideoFormat(display->displayVdc, video_format_info.eVideoFmt);
            if (rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc);goto err_setformat;}
        }

        display->status.refreshRate = B_REFRESH_RATE_10_TO_1000(video_format_info.ulVertFreq);
        display->formatChanged = true;
    }

    if (force || pSettings->display3DSettings.overrideOrientation != display->cfg.display3DSettings.overrideOrientation ||
        pSettings->display3DSettings.orientation != display->cfg.display3DSettings.orientation)
    {
        rc = BVDC_Display_SetOrientation(
            display->displayVdc,
            NEXUS_P_VideoOrientation_ToMagnum(pSettings->display3DSettings.overrideOrientation ? pSettings->display3DSettings.orientation : NEXUS_VideoOrientation_e2D) /* if overrideOrientation is false, then we need to default back to 2D */
            );
        if(rc!=BERR_SUCCESS) {rc = BERR_TRACE(rc);goto err_setorientation;}
    }

    if (force || pSettings->display3DSettings.sourceBuffer != display->cfg.display3DSettings.sourceBuffer) {
        rc = BVDC_Display_Set3dSourceBufferSelect(display->displayVdc, (BVDC_3dSourceBufferSelect)pSettings->display3DSettings.sourceBuffer);
        if(rc!=BERR_SUCCESS) {rc = BERR_TRACE(rc);goto err_setorientation;}
    }

    /* SW7335-794 alignment needs to be done if forced, format changes, or target changes */
    if (force || display->cfg.format!=pSettings->format || display->cfg.alignmentTarget != pSettings->alignmentTarget) {
#if NEXUS_NUM_DISPLAYS > 1
        rc = NEXUS_Display_P_Align(display, pSettings->alignmentTarget);
        if (rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc);goto err_setformat;}
#endif
    }

#if NEXUS_HAS_TRANSPORT
    if(force || display->cfg.timebase!=pSettings->timebase) {
        NEXUS_TimebaseHandle timebase = NULL;

        NEXUS_Module_Lock(g_NEXUS_DisplayModule_State.modules.transport);
        timebase = NEXUS_Timebase_Resolve_priv(pSettings->timebase);
        if (timebase)
        {
            unsigned timebaseIndex;
            rc = NEXUS_Timebase_GetIndex_priv(timebase, &timebaseIndex);
            if (!rc)
            {
                rc = BVDC_Display_SetTimebase(display->displayVdc, BAVC_Timebase_e0 + timebaseIndex);
            }
        }
        else
        {
            rc = NEXUS_INVALID_PARAMETER;
        }
        NEXUS_Module_Unlock(g_NEXUS_DisplayModule_State.modules.transport);
        if (rc) { rc = BERR_TRACE(rc); goto err_settimebase; }
    }
#endif

    if(force || display->cfg.aspectRatio != pSettings->aspectRatio || display->cfg.format!=pSettings->format) {
        if (pSettings->aspectRatio == NEXUS_DisplayAspectRatio_eSar) {
            /* this is only valid for NEXUS_Display_SetCustomFormatSettings and was set there. if it must be supported for NEXUS_Display_SetSettings, move
            that VDC call here. */
            if (display->cfg.format != NEXUS_VideoFormat_eCustom2) {
                rc = BERR_TRACE(NEXUS_INVALID_PARAMETER);
                goto err_aspectratio;
            }
        }
        else {
            BFMT_AspectRatio aspectRatioVdc;
            rc = NEXUS_P_DisplayAspectRatio_ToMagnum(pSettings->aspectRatio, pSettings->format, &aspectRatioVdc);
            if (rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc);goto err_aspectratio;}
            rc = BVDC_Display_SetAspectRatio(display->displayVdc, aspectRatioVdc);
            if (rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc);goto err_setaspectratio;}
        }
    }

    if(force || display->cfg.background != pSettings->background) {
        rc = BVDC_Compositor_SetBackgroundColor(display->compositor, B_GET_BITS(pSettings->background, 23, 16), B_GET_BITS(pSettings->background, 15, 8), B_GET_BITS(pSettings->background, 7, 0));
        if (rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc);goto err_background;}
    }

    if(force || display->cfg.xvYccEnabled != pSettings->xvYccEnabled) {
        rc = BVDC_Display_SetHdmiXvYcc(display->displayVdc, pSettings->xvYccEnabled);
        if (rc) rc=BERR_TRACE(rc);
    }

    for(i=0;i<sizeof(display->windows)/sizeof(display->windows[0]);i++) {
        NEXUS_VideoWindowHandle window = &display->windows[i];
        rc = NEXUS_VideoWindow_P_ConfigMasterFrameRate(window, pSettings, &window->cfg);
        if (rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc);goto err_master;}
        BKNI_EnterCriticalSection();
        NEXUS_VideoWindow_P_UpdatePhaseDelay_isr(window, display->status.refreshRate);
        BKNI_LeaveCriticalSection();
    }

    rc = NEXUS_Display_P_ConfigVecInterrupts(display, pSettings);
    if (rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc);goto err_configvecinterrupts;}

    return BERR_SUCCESS;

err_configvecinterrupts:
err_master:
err_background:
#if NEXUS_HAS_TRANSPORT
err_settimebase:
#endif
err_aspectratio:
err_setaspectratio:
err_setorientation:
err_setformat:
err_format:
    return rc;
}

#if NEXUS_HAS_VIDEO_ENCODER && NEXUS_NUM_DSP_VIDEO_ENCODERS
static void NEXUS_Display_GetCaptureBuffer_isr(NEXUS_DisplayHandle display)
{
    BVDC_Test_Window_CapturedImage capture;
    NEXUS_DisplayCapturedImage image;
    NEXUS_Error rc;
    bool drop=false;
    NEXUS_Display_P_Image *pImage;

    if(!display->windows[0].vdcState.window) 
	return;

    BVDC_Test_Window_GetBuffer_isr(display->windows[0].vdcState.window, &capture);
    BDBG_MSG(("GET %#x", (uint32_t)capture.pvBufAddr));
    if(capture.pvBufAddr) {
	if(display->encoder.framesEnqueued%display->encoder.dropRate == 0) {
	    image.pvaddress = capture.pvBufAddr;
	    image.width = capture.ulWidth;
	    image.height = capture.ulHeight;
	    image.polarity = capture.eCapturePolarity;
	    image.origPts = capture.ulOrigPTS;
	    NEXUS_P_LookupFrameRate(display->status.refreshRate , &image.framerate);
	    rc = display->encoder.enqueueCb_isr(display->encoder.context, &image);
	    if(rc == BERR_SUCCESS) {
		pImage = BLST_SQ_FIRST(&display->encoder.free);
		BDBG_ASSERT(pImage);
		pImage->addr = capture.pvBufAddr;
		BLST_SQ_REMOVE_HEAD(&display->encoder.free, link);
		BLST_SQ_INSERT_TAIL(&display->encoder.queued, pImage, link);
	    }
	} else {
	    drop=true;
	}
	if(drop || rc) {
	    BDBG_MSG(("RET1 %#x", (uint32_t)capture.pvBufAddr));
	    BVDC_Test_Window_ReturnBuffer_isr(display->windows[0].vdcState.window, &capture);

	}
	display->encoder.framesEnqueued++;
    }else {
	if(!BLST_SQ_FIRST(&display->encoder.free))
	    BDBG_WRN(("%d out of %d Buffers in use", NEXUS_DISPLAY_ENCODER_MAX_PICTURE_BUFFERS, NEXUS_DISPLAY_ENCODER_MAX_PICTURE_BUFFERS));
    }
}

static void NEXUS_Display_ReturnCaptureBuffer_isr(NEXUS_DisplayHandle display)
{
    BVDC_Test_Window_CapturedImage capture;
    NEXUS_DisplayCapturedImage image;
    NEXUS_Error rc;
    NEXUS_Display_P_Image *pImage;

    /* Query the encoder for all possible buffers that can be returned */
    for(;;) {
	rc = display->encoder.dequeueCb_isr(display->encoder.context, &image);

	if(image.pvaddress)  {
	    capture.pvBufAddr = image.pvaddress;
	    BDBG_MSG(("RET2 %#x", (uint32_t)capture.pvBufAddr));
	    BVDC_Test_Window_ReturnBuffer_isr(display->windows[0].vdcState.window, &capture);

	    for(pImage = BLST_SQ_FIRST(&display->encoder.queued); pImage;  pImage= BLST_SQ_NEXT(pImage, link)) {
		if(pImage->addr == capture.pvBufAddr)
		    break;
	    }
	    if(pImage) {
		BLST_SQ_REMOVE(&display->encoder.queued,pImage , NEXUS_Display_P_Image, link);
		BLST_SQ_INSERT_TAIL(&display->encoder.free, pImage, link);
	    } else {
		/* This should never happen. We should always find the return buffer in our list if dequeue_isr returns one*/
		BDBG_ASSERT(0);
	    }

	} else {
	    break;
	}
    }
}
#endif

static void NEXUS_DisplayCb_isr(void *pParam, int iParam, void *pCbData)
{
    NEXUS_DisplayHandle display;
    BAVC_VdcDisplay_Info *pVdcRateInfo;
    BVDC_Display_CallbackData *pCallbackData = pCbData;
    BDBG_ASSERT(NULL != pParam);
    BDBG_ASSERT(NULL != pCbData);
    BSTD_UNUSED(iParam);

    display = pParam;
    BDBG_OBJECT_ASSERT(display, NEXUS_Display);

    BDBG_MSG(("NEXUS_DisplayCb_isr %p", display));
    pVdcRateInfo = &(pCallbackData->sDisplayInfo);

#if NEXUS_HAS_VIDEO_ENCODER && !NEXUS_NUM_DSP_VIDEO_ENCODERS
    if (display->encodeUserData && pCallbackData->stMask.bStgPictureId)
    {
        BXUDlib_DisplayInterruptHandler_isr(display->hXud, pCallbackData);
    }
#endif

    if (pCallbackData->stMask.bRateChange) {
#if NEXUS_NUM_HDMI_OUTPUTS
        display->hdmi.rateInfo = *pVdcRateInfo;
        display->hdmi.rateInfoValid = true;

        if ( NULL != display->hdmi.rateChangeCb_isr )
        {
            BDBG_MSG(("Propagating rate change to HDMI"));
            display->hdmi.rateChangeCb_isr(display, display->hdmi.pCbParam);
        }
        else
        {
            BDBG_MSG(("Not Propagating rate change to HDMI"));
        }
#endif
#if NEXUS_NUM_HDMI_DVO
        display->hdmiDvo.rateInfo = *pVdcRateInfo;
        display->hdmiDvo.rateInfoValid = true;

        if ( NULL != display->hdmiDvo.rateChangeCb_isr )
        {
            BDBG_MSG(("Propagating rate change to HDMI DVO"));
            display->hdmiDvo.rateChangeCb_isr(display, display->hdmiDvo.pCbParam);
        }
        else
        {
            BDBG_MSG(("Not Propagating rate change to HDMI DVO"));
        }
#endif
        display->status.refreshRate = B_REFRESH_RATE_10_TO_1000(pVdcRateInfo->ulVertRefreshRate);

        BDBG_MSG(("display refresh rate change to %d", display->status.refreshRate));
        BKNI_SetEvent(display->refreshRate.event);
    }
#if NEXUS_NUM_HDMI_OUTPUTS
    if (display->hdmi.vsync_isr) {
        /* general purpose per-vsync isr. one use is CRC capture. */
        (display->hdmi.vsync_isr)(display->hdmi.pCbParam);
    }
#endif

    if (pCallbackData->stMask.bCrc && display->crc.queue) {
        NEXUS_DisplayCrcData *pData = &display->crc.queue[display->crc.wptr];
        pData->cmp.luma = pCallbackData->ulCrcLuma;
#if defined(HUMAX_PLATFORM_BASE)
		pData->cmp.cb = pCallbackData->ulCrcCr;
        pData->cmp.cr = pCallbackData->ulCrcCb;
#else
        pData->cmp.cb = pCallbackData->ulCrcCb;
        pData->cmp.cr = pCallbackData->ulCrcCr;
#endif		
        if (++display->crc.wptr == display->crc.size) {
            display->crc.wptr = 0;
        }
        if (display->crc.wptr == display->crc.rptr) {
            BDBG_WRN(("Display%d CMP CRC overflow", display->index));
        }
    }

#if NEXUS_HAS_VIDEO_ENCODER && NEXUS_NUM_DSP_VIDEO_ENCODERS
    if (pCallbackData->stMask.bPerVsync && display->encoder.callbackEnabled) {
        NEXUS_Display_GetCaptureBuffer_isr(display);
        NEXUS_Display_ReturnCaptureBuffer_isr(display);
    }
#endif
}

/* nexus_p_install_display_cb is called on Open and SetSettings.
it is not evenly paired with nexus_p_uninstall_display_cb */
static BERR_Code nexus_p_install_display_cb(NEXUS_DisplayHandle display)
{
    BVDC_Display_CallbackSettings settings;
    BERR_Code rc;

    BVDC_Display_GetCallbackSettings(display->displayVdc, &settings);
    settings.stMask.bRateChange = 1;
    settings.stMask.bCrc = (display->cfg.crcQueueSize != 0);
    settings.stMask.bPerVsync = 1; /* needed for HDMI crc */

#if NEXUS_HAS_VIDEO_ENCODER && !NEXUS_NUM_DSP_VIDEO_ENCODERS
    if( display->encodeUserData )
    {
        settings.stMask.bStgPictureId = 1;
        BDBG_MSG(("Display %p enables STG display callback.", display->index));
    }
#endif

    rc = BVDC_Display_SetCallbackSettings(display->displayVdc, &settings);
    if (rc) return BERR_TRACE(rc);

    if (display->crc.size != display->cfg.crcQueueSize) {
        void *new_ptr = NULL, *old_ptr;

        /* defer the free until after critical section */
        old_ptr = display->crc.queue;
        /* queue size of 1 is treated same as 0 because it can't hold anything */
        if (display->cfg.crcQueueSize > 1) {
            new_ptr = BKNI_Malloc(display->cfg.crcQueueSize * sizeof(NEXUS_DisplayCrcData));
            if (!new_ptr) {
                return BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);
            }
        }

        /* must synchronize with ISR, so set state in CS */
        BKNI_EnterCriticalSection();
        display->crc.queue = new_ptr;
        display->crc.size = display->cfg.crcQueueSize>1?display->cfg.crcQueueSize:0;
        display->crc.wptr = display->crc.rptr = 0; /* flush */
        BKNI_LeaveCriticalSection();

        if (old_ptr) {
            BKNI_Free(old_ptr);
        }
    }

    rc = BVDC_Display_InstallCallback(display->displayVdc, (BVDC_CallbackFunc_isr)NEXUS_DisplayCb_isr, display, 0);
    if (rc) return BERR_TRACE(rc);

    return 0;
}

static void nexus_p_uninstall_display_cb(NEXUS_DisplayHandle display)
{
    BVDC_Display_CallbackSettings settings;
    BERR_Code rc;

    BVDC_Display_GetCallbackSettings(display->displayVdc, &settings);
    settings.stMask.bRateChange = 0;
    settings.stMask.bCrc = 0;
    rc = BVDC_Display_SetCallbackSettings(display->displayVdc, &settings);
    if (rc) rc = BERR_TRACE(rc);

    rc = BVDC_Display_InstallCallback(display->displayVdc, (BVDC_CallbackFunc_isr)NULL, NULL, 0);
    if (rc) rc = BERR_TRACE(rc);

    if (display->crc.queue) {
        BKNI_Free(display->crc.queue);
        BKNI_EnterCriticalSection();
        display->crc.queue = NULL;
        display->crc.size = 0;
        BKNI_LeaveCriticalSection();
    }
}

static BERR_Code
NEXUS_Display_P_Open(NEXUS_DisplayHandle display, unsigned displayIndex, const NEXUS_DisplaySettings *pSettings)
{
    BERR_Code rc=BERR_SUCCESS;
    BVDC_DisplayId vdcDisplayId;

    if (pSettings->displayType == NEXUS_DisplayType_eDvo || pSettings->displayType == NEXUS_DisplayType_eLvds) {
        BVDC_Display_Settings vdcDisplayCfg;

        if (displayIndex != 0) {
            BDBG_ERR(("invalid dvo display"));
            goto err_compositor;
        }
        BDBG_MSG(("BVDC_Compositor_Create dvo"));
        rc = BVDC_Compositor_Create(pVideo->vdc, &display->compositor, BVDC_CompositorId_eCompositor0, NULL);
        if (rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc);goto err_compositor;}
        BDBG_MSG(("BVDC_Display_Create dvo"));
        vdcDisplayId = BVDC_DisplayId_eDisplay0;
        BVDC_Display_GetDefaultSettings(vdcDisplayId, &vdcDisplayCfg);
        vdcDisplayCfg.eMasterTg = BVDC_DisplayTg_eDviDtg;
        rc = BVDC_Display_Create(display->compositor, &display->displayVdc, vdcDisplayId, &vdcDisplayCfg);
        if (rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc);goto err_display;}
    }
    else if (pSettings->displayType == NEXUS_DisplayType_eBypass) {
        BVDC_Display_Settings vdcDisplayCfg;

        BDBG_MSG(("BVDC_Compositor_Create %d", BVDC_CompositorId_eCompositor0 + displayIndex));
        rc = BVDC_Compositor_Create(pVideo->vdc, &display->compositor, BVDC_CompositorId_eCompositor0 + displayIndex, NULL);
        if (rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc);goto err_compositor;}

        BDBG_MSG(("BVDC_Display_Create"));
        BVDC_Display_GetDefaultSettings(BVDC_DisplayId_eDisplay2, &vdcDisplayCfg);
        vdcDisplayCfg.eMasterTg = BVDC_DisplayTg_ePrimIt;
        rc = BVDC_Display_Create(display->compositor, &display->displayVdc, BVDC_DisplayId_eDisplay0, &vdcDisplayCfg);
        if (rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc);goto err_display;}
    }
    else {
        BVDC_Display_Settings vdcDisplayCfg;

        unsigned vecIndex;
        NEXUS_VideoFormatInfo formatInfo;
        if (pSettings->vecIndex == -1) {
            if (displayIndex < 2 && g_NEXUS_DisplayModule_State.moduleSettings.vecSwap) {
                vecIndex = 1-displayIndex;
            }
            else {
                vecIndex = displayIndex;
            }
        }
        else {
            vecIndex = (unsigned)pSettings->vecIndex;
        }
        BDBG_MSG(("BVDC_Compositor_Create %d", BVDC_CompositorId_eCompositor0+displayIndex));
        rc = BVDC_Compositor_Create(pVideo->vdc, &display->compositor, BVDC_CompositorId_eCompositor0+displayIndex, NULL);
        if (rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc);goto err_compositor;}

        BDBG_MSG(("BVDC_Display_Create %d", BVDC_DisplayId_eDisplay0+vecIndex));
        BVDC_Display_GetDefaultSettings(BVDC_DisplayId_eDisplay0+vecIndex, &vdcDisplayCfg);
        if (pSettings->timingGenerator < NEXUS_DisplayTimingGenerator_eAuto) {
            BDBG_CASSERT(NEXUS_DisplayTimingGenerator_eEncoder == (NEXUS_DisplayTimingGenerator)BVDC_DisplayTg_eStg);
            vdcDisplayCfg.eMasterTg = (BVDC_DisplayTg)pSettings->timingGenerator;
        }

#if !NEXUS_NUM_DSP_VIDEO_ENCODERS
        /* encoder timing generator override */
        if (pSettings->timingGenerator == NEXUS_DisplayTimingGenerator_eAuto || pSettings->timingGenerator == NEXUS_DisplayTimingGenerator_eEncoder) {
#ifdef NEXUS_ENCODER_DISPLAY_IDX
            if (display->index == NEXUS_ENCODER_DISPLAY_IDX) {
                vdcDisplayCfg.eMasterTg = BVDC_DisplayTg_eStg0;
            }
#endif
#ifdef NEXUS_ENCODER0_DISPLAY_IDX
            if (display->index == NEXUS_ENCODER0_DISPLAY_IDX) {
                vdcDisplayCfg.eMasterTg = g_NEXUS_DisplayModule_State.moduleSettings.encoderTgMapping[0] > -1 ?
                    BVDC_DisplayTg_eStg0 + g_NEXUS_DisplayModule_State.moduleSettings.encoderTgMapping[0] : 
                    BVDC_DisplayTg_eStg0;
            }
#endif
#ifdef NEXUS_ENCODER1_DISPLAY_IDX
            if (display->index == NEXUS_ENCODER1_DISPLAY_IDX) {
                vdcDisplayCfg.eMasterTg = g_NEXUS_DisplayModule_State.moduleSettings.encoderTgMapping[1] > -1 ?
                    BVDC_DisplayTg_eStg0 + g_NEXUS_DisplayModule_State.moduleSettings.encoderTgMapping[1] : 
                    BVDC_DisplayTg_eStg1;
            }
#endif
#ifdef NEXUS_ENCODER2_DISPLAY_IDX
            if (display->index == NEXUS_ENCODER2_DISPLAY_IDX) {
                vdcDisplayCfg.eMasterTg = g_NEXUS_DisplayModule_State.moduleSettings.encoderTgMapping[2] > -1 ?
                    BVDC_DisplayTg_eStg0 + g_NEXUS_DisplayModule_State.moduleSettings.encoderTgMapping[2] :
                    BVDC_DisplayTg_eStg2;
            }
#endif
#ifdef NEXUS_ENCODER3_DISPLAY_IDX
            if (display->index == NEXUS_ENCODER3_DISPLAY_IDX) {
                vdcDisplayCfg.eMasterTg = g_NEXUS_DisplayModule_State.moduleSettings.encoderTgMapping[3] > -1 ?
                    BVDC_DisplayTg_eStg0 + g_NEXUS_DisplayModule_State.moduleSettings.encoderTgMapping[3] :
                    BVDC_DisplayTg_eStg3;
            }
#endif
            display->stgIndex = vdcDisplayCfg.eMasterTg - BVDC_DisplayTg_eStg0;
        }
#endif
        NEXUS_VideoFormat_GetInfo(pSettings->format, &formatInfo);
        if (formatInfo.isFullRes3d && pSettings->timingGenerator!=NEXUS_DisplayTimingGenerator_eHdmiDvo) {
            BDBG_WRN(("3D output display format selected without HDMI master mode (NEXUS_DisplayTimingGenerator_eHdmiDvo)"));
        }
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_ARIB_VIDEO)
			vdcDisplayCfg.bArib480p = true;
#endif
        rc = BVDC_Display_Create(display->compositor, &display->displayVdc, BVDC_DisplayId_eDisplay0+vecIndex, &vdcDisplayCfg);
        if (rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc);goto err_display;}
    }
    return rc;

err_display:
    BVDC_Compositor_Destroy(display->compositor);
err_compositor:
    {
        BERR_Code erc = BVDC_AbortChanges(pVideo->vdc);
        if (erc!=BERR_SUCCESS) {rc=BERR_TRACE(erc);}
    }
    return rc;
}


NEXUS_DisplayHandle
NEXUS_Display_Open(unsigned displayIndex,const NEXUS_DisplaySettings *pSettings)
{
    NEXUS_DisplayHandle  display;
    BERR_Code rc;
    unsigned i;
    NEXUS_DisplaySettings defaultSettings;

    BDBG_MSG(("NEXUS_Display_Open: %u", displayIndex));

    if (displayIndex >= NEXUS_NUM_DISPLAYS) {
        rc = BERR_TRACE(NEXUS_INVALID_PARAMETER);
        return NULL;
    }

    if(pSettings==NULL) {
        NEXUS_Display_GetDefaultSettings(&defaultSettings);
        pSettings = &defaultSettings;
    }
    if(displayIndex>=sizeof(pVideo->displays)/sizeof(pVideo->displays[0])) {
        rc = BERR_TRACE(BERR_INVALID_PARAMETER);
        goto err_settings;
    }
    display = BKNI_Malloc(sizeof(*display));
    if(!display) {
        rc = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        goto err_alloc;
    }
    NEXUS_OBJECT_INIT(NEXUS_Display, display);
    display->cfg = *pSettings;
    display->index = displayIndex;
    BLST_D_INIT(&display->outputs);
    display->vsyncCallback.isrCallback = NEXUS_IsrCallback_Create(display, NULL);
    if (!display->vsyncCallback.isrCallback) {rc = BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY); goto err_createisrcallback;}
    rc = BKNI_CreateEvent(&display->refreshRate.event);
    if (rc) {rc = BERR_TRACE(rc); goto err_createrefreshrateevent;}
    display->refreshRate.handler = NEXUS_RegisterEvent(display->refreshRate.event, nexus_display_p_refresh_rate_event, display);
    if (!display->refreshRate.handler) {rc = BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY); goto err_registerrefreshrate;}

    if (display->cfg.timingGenerator == NEXUS_DisplayTimingGenerator_eEncoder) {
        display->stgSettings.enabled = true;
    }

    /* By default, there are no restrictions on graphics height/width/bpp.  Dynamic RTS may enforce this later. */
    display->restrictions.gfxWidth = display->restrictions.gfxHeight = display->restrictions.gfxBpp = 0xffffffff;

#if NEXUS_NUM_HDMI_OUTPUTS
    BDBG_ASSERT(NULL == display->hdmi.rateChangeCb_isr);
    NEXUS_CallbackHandler_Init(display->hdmi.outputNotifyDisplay,NEXUS_VideoOutput_P_SetHdmiSettings, display );
    display->hdmi.outputNotify = NULL;
#endif

    rc = NEXUS_Display_P_Open(display, displayIndex, pSettings);
    if (rc) {rc = BERR_TRACE(rc); goto err_open;}

    rc = nexus_p_install_display_cb(display);
    if (rc) {rc = BERR_TRACE(rc); goto err_install_display_cb;}

    for(i=0;i<sizeof(display->windows)/sizeof(display->windows[0]);i++) {
        NEXUS_VideoWindowHandle window = &display->windows[i];
        NEXUS_VideoWindow_P_InitState(window, i, display);
    }

    NEXUS_Display_P_InitGraphics(display);

    rc = NEXUS_Display_P_SetSettings(display, pSettings, true);
    if (rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc);goto err_display_set;}

    /* now that display->displayRect is set, we can set default window positions */
    for(i=0;i<sizeof(display->windows)/sizeof(display->windows[0]);i++) {
        NEXUS_VideoWindowHandle window = &display->windows[i];
        window->cfg.position = display->displayRect;
        window->cfg.clipBase = display->displayRect;
        if (pSettings->displayType == NEXUS_DisplayType_eBypass) {
            window->cfg.position.height = 482;
        }
        /* coverity[dead_error_condition] */
        if(i>0) {
            window->cfg.position.x = display->displayRect.width/2;
            window->cfg.position.y = 0;
            window->cfg.position.width /=2;
            window->cfg.position.height /=2;
        }
    }

    BDBG_MSG(("First Display VDC ApplyChanges [display=%p]", display));
    if(pVideo->updateMode != NEXUS_DisplayUpdateMode_eAuto) {rc=BERR_TRACE(NEXUS_NOT_SUPPORTED);}
    rc = BVDC_ApplyChanges(pVideo->vdc);
    if (rc!=BERR_SUCCESS) {rc = BERR_TRACE(rc);goto err_applychanges;}

    rc = NEXUS_Display_P_ConnectVbi(display);
    if (rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc);goto err_vbi;}

#if NEXUS_HAS_VIDEO_ENCODER
#if !NEXUS_NUM_DSP_VIDEO_ENCODERS
    BXUDlib_Create(&display->hXud, NULL);
#else
    /*Initialize the capture buffer pointer cache */
    {
    NEXUS_Display_P_Image *image;

    BLST_SQ_INIT(&display->encoder.free);
    BLST_SQ_INIT(&display->encoder.queued);
    for(i=0; i<NEXUS_DISPLAY_ENCODER_MAX_PICTURE_BUFFERS; i++){
        image = BKNI_Malloc(sizeof(*image));
        BLST_SQ_INSERT_TAIL(&display->encoder.free, image, link);
    }
    display->encoder.framesEnqueued = 0;
    }
#endif
#endif
    pVideo->displays[display->index] = display;

    return display;

err_vbi:
err_applychanges:
err_display_set:
    NEXUS_Display_P_UninitGraphics(display);
    nexus_p_uninstall_display_cb(display);
err_install_display_cb:
    /* TODO: NEXUS_Display_P_Close */
    rc = BVDC_Display_Destroy(display->displayVdc);
    if (rc!=BERR_SUCCESS) {rc = BERR_TRACE(rc);}
    rc = BVDC_Compositor_Destroy(display->compositor);
    if (rc!=BERR_SUCCESS) {rc = BERR_TRACE(rc);}
    rc = BVDC_AbortChanges(pVideo->vdc);
    if (rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc);}
    rc = BVDC_ApplyChanges(pVideo->vdc); /* an Apply is needed after the Destroy */
    if (rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc);}
err_open:
    NEXUS_UnregisterEvent(display->refreshRate.handler);
err_registerrefreshrate:
    BKNI_DestroyEvent(display->refreshRate.event);
err_createrefreshrateevent:
    NEXUS_IsrCallback_Destroy(display->vsyncCallback.isrCallback);
err_createisrcallback:
    NEXUS_OBJECT_DESTROY(NEXUS_Display, display);
    BKNI_Free(display);
err_alloc:
err_settings:
    return NULL;
}

static void
NEXUS_Display_P_Release(NEXUS_DisplayHandle display)
{
    unsigned i;
    for(i=0;i<sizeof(display->windows)/sizeof(display->windows[0]);i++) {
        NEXUS_VideoWindowHandle window = &display->windows[i];

        if (window->open) {
            BDBG_WRN(("NEXUS_Display_Close is automatically closing window %p. Application must explicitly close the handle.", window));
            NEXUS_OBJECT_UNREGISTER(NEXUS_VideoWindow, window, Close);
            NEXUS_VideoWindow_Close(window);
        }
    }
    return;
}

static void
NEXUS_Display_P_Finalizer(NEXUS_DisplayHandle display)
{
    BERR_Code rc;
    unsigned i;

    NEXUS_OBJECT_ASSERT(NEXUS_Display, display);

    if (display->index >= sizeof(pVideo->displays)/sizeof(pVideo->displays[0])) {
        /* adding runtime check for static analysis */
        return;
    }

    /* stop all callbacks from coming into display module */
    nexus_p_uninstall_display_cb(display);
    display->cfg.vsyncCallback.callback = NULL; /* force the INT callbacks to be destroyed */
    (void)NEXUS_Display_P_ConfigVecInterrupts(display, &display->cfg);
    NEXUS_UnregisterEvent(display->refreshRate.handler);

    NEXUS_Display_RemoveAllOutputs(display);

#if NEXUS_HAS_VIDEO_ENCODER && !NEXUS_NUM_DSP_VIDEO_ENCODERS
    BXUDlib_Destroy(display->hXud);
#endif

#if B_HAS_VBI
    if (display->vbi.settings.vbiSource) {
        NEXUS_DisplayVbiSettings vbiSettings = display->vbi.settings;
        vbiSettings.vbiSource = NULL;
        NEXUS_Display_SetVbiSettings(display, &vbiSettings);
    }
#endif

    /* if we close the last display, then shutdown all cached VideoInput's. this allows implicit close in kernel mode implementations to work. */
    for (i=0;i<sizeof(pVideo->displays)/sizeof(pVideo->displays[0]);i++) {
        if (pVideo->displays[i] && pVideo->displays[i] != display) break;
    }
    if (i == sizeof(pVideo->displays)/sizeof(pVideo->displays[0])) {
        NEXUS_VideoInput_P_Link *inputLink;

        while(NULL!=(inputLink=BLST_S_FIRST(&pVideo->inputs))) {
            BDBG_ASSERT(inputLink->input);
            NEXUS_VideoInput_Shutdown(inputLink->input);
        }
    }

    for(i=0;i<sizeof(display->windows)/sizeof(display->windows[0]);i++) {
        NEXUS_VideoWindowHandle window = &display->windows[i];

        if (window->adjContext.customDnrData) {
            BKNI_Free(window->adjContext.customDnrData);
        }
        if (window->adjContext.customAnrData) {
            BKNI_Free(window->adjContext.customAnrData);
        }
        if (window->picContext.customContrastStretchData) {
            BKNI_Free(window->picContext.customContrastStretchData);
        }
        /* If the user calls NEXUS_VideoWindow_Close after this, BDBG_OBJECT will assert. */
        BDBG_OBJECT_DESTROY(window, NEXUS_VideoWindow);
    }

    NEXUS_Display_P_DisconnectVbi(display);

    NEXUS_Display_P_UninitGraphics(display);

    rc = BVDC_Display_Destroy(display->displayVdc);
    if (rc!=BERR_SUCCESS) {rc = BERR_TRACE(rc);}
    rc = BVDC_Compositor_Destroy(display->compositor);
    if (rc!=BERR_SUCCESS) {rc = BERR_TRACE(rc);}

    if(pVideo->updateMode != NEXUS_DisplayUpdateMode_eAuto) {rc=BERR_TRACE(NEXUS_NOT_SUPPORTED);}
    rc = BVDC_ApplyChanges(pVideo->vdc);
    if (rc!=BERR_SUCCESS) {rc = BERR_TRACE(rc);}

    BKNI_DestroyEvent(display->refreshRate.event);
    NEXUS_IsrCallback_Destroy(display->vsyncCallback.isrCallback);

    if (display->customFormatInfo) {
        if (display->customFormatInfo->pCustomInfo) {
            BKNI_Free(display->customFormatInfo->pCustomInfo->pDvoRmTbl1);
            BKNI_Free(display->customFormatInfo->pCustomInfo->pDvoRmTbl0);
            BKNI_Free(display->customFormatInfo->pCustomInfo->pDvoMicrocodeTbl);
            BKNI_Free(display->customFormatInfo->pCustomInfo);
        }
        BKNI_Free(display->customFormatInfo);
        display->customFormatInfo = NULL;
    }
#if NEXUS_NUM_HDMI_OUTPUTS
    NEXUS_CallbackHandler_Shutdown(display->hdmi.outputNotifyDisplay);
#endif
#if NEXUS_HAS_VIDEO_ENCODER && NEXUS_NUM_DSP_VIDEO_ENCODERS
    {
    NEXUS_Display_P_Image *image;
    while(NULL != (image=BLST_SQ_FIRST(&display->encoder.free))){
        BLST_SQ_REMOVE_HEAD(&display->encoder.free, link);
        BKNI_Free(image);
    }
    while(NULL != (image=BLST_SQ_FIRST(&display->encoder.queued))){
        BLST_SQ_REMOVE_HEAD(&display->encoder.queued, link);
        BKNI_Free(image);
    }
    }
#endif

    pVideo->displays[display->index] = NULL;

    NEXUS_OBJECT_DESTROY(NEXUS_Display, display);
    BKNI_Free(display);

    return;
}

NEXUS_OBJECT_CLASS_MAKE_WITH_RELEASE(NEXUS_Display, NEXUS_Display_Close);

NEXUS_Error
NEXUS_Display_AddOutput(NEXUS_DisplayHandle display, NEXUS_VideoOutput output)
{
    BERR_Code rc, cleanup_rc;
    NEXUS_VideoOutput_P_Link *link;

    BDBG_OBJECT_ASSERT(display, NEXUS_Display);
    BDBG_OBJECT_ASSERT(output, NEXUS_VideoOutput);
    BDBG_ASSERT(output->source);
    if(output->destination) {
        BDBG_ERR(("This output is already connected to a display."));
        rc = BERR_TRACE(NEXUS_DISPLAY_ADD_OUTPUT_ALREADY_CONNECTED);
        goto err_already_connected;
    }
    link = NEXUS_P_VideoOutput_Link(output);
    if(!link) {
        rc = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY); /* unable to create link */
        goto err_link;
    }
    rc = link->iface.connect(output->source, display);
    if(rc!=BERR_SUCCESS) {
        BDBG_ERR(("Unable to connect this output to the display. There may be an conflict in outputs or this output is not compatible with this displayType or format."));
        rc = BERR_TRACE(rc);
        goto err_connect;
    }

    rc = NEXUS_Display_P_ApplyChanges();
    if (rc) {rc = BERR_TRACE(rc); goto err_apply;}

    link->display = display;
    BLST_D_INSERT_HEAD(&display->outputs, link, link);
    return BERR_SUCCESS;

err_apply:
    cleanup_rc = link->iface.disconnect(output->source, display);
    if(cleanup_rc!=BERR_SUCCESS) { cleanup_rc = BERR_TRACE(cleanup_rc); }
err_connect:
    output->destination = NULL; /* unlink */
    cleanup_rc = BVDC_AbortChanges(pVideo->vdc);
    if(cleanup_rc!=BERR_SUCCESS) { cleanup_rc = BERR_TRACE(cleanup_rc); }
err_link:
err_already_connected:
    return rc;
}

NEXUS_Error
NEXUS_Display_RemoveOutput( NEXUS_DisplayHandle display, NEXUS_VideoOutput output)
{
    BERR_Code rc;
    NEXUS_VideoOutput_P_Link *link;

    BDBG_OBJECT_ASSERT(display, NEXUS_Display);
    BDBG_OBJECT_ASSERT(output, NEXUS_VideoOutput);
    BDBG_ASSERT(output->source);
    if(output->destination == NULL) {
#if defined(HUMAX_PLATFORM_BASE)
		return BERR_SUCCESS;
#else
        rc = BERR_TRACE(NEXUS_DISPLAY_REMOVE_OUTPUT_NOT_CONNECTED);
        goto err_already_disconnected;
#endif
    }
    link = output->destination;
    BDBG_OBJECT_ASSERT(link, NEXUS_VideoOutput_P_Link);
    if(link->display!=display) {
        BDBG_ERR(("This output is not connected to this display."));
        rc = BERR_TRACE(NEXUS_DISPLAY_REMOVE_OUTPUT_WRONG_CONNECTION);
        goto err_already_disconnected;
    }
    BLST_D_REMOVE(&display->outputs, link, link); /* remove it from list right the way */
    link->display = NULL;
    rc = link->iface.disconnect(output->source, display);
    if(rc!=BERR_SUCCESS) {
        /* we are going to leak the link here */
        rc = BERR_TRACE(rc);
        goto err_disconnect;
    }
    output->destination = NULL;

    /* deallocate the link too */
    NEXUS_VideoOutput_P_DestroyLink(link);

    rc = NEXUS_Display_P_ApplyChanges();
    if (rc) { rc = BERR_TRACE(rc); goto err_apply; }

    return BERR_SUCCESS;
err_apply:
err_disconnect:
    {
        BERR_Code rc = BVDC_AbortChanges(pVideo->vdc); /* preserve original rc */
        if (rc!=BERR_SUCCESS) {rc = BERR_TRACE(rc);}
    }
err_already_disconnected:
    return rc;
}

void
NEXUS_Display_RemoveAllOutputs(NEXUS_DisplayHandle display)
{
    NEXUS_VideoOutput_P_Link *link;
    NEXUS_Error rc;
    BDBG_OBJECT_ASSERT(display, NEXUS_Display);

    while(NULL!=(link= BLST_D_FIRST(&display->outputs))) {
        BDBG_ASSERT(link->display == display);
        rc = NEXUS_Display_RemoveOutput( display, link->output);
        if(rc!=BERR_SUCCESS) {
            rc = BERR_TRACE(rc);
            break; /* exit from the loop */
        }
    }
    return;
}

static bool
NEXUS_P_Display_RectEqual3d(const NEXUS_Rect *winRect, const NEXUS_Rect *dispRect, NEXUS_VideoOrientation orientation)
{
    BDBG_ASSERT(winRect);
    BDBG_ASSERT(dispRect);

    if (orientation==NEXUS_VideoOrientation_e3D_LeftRight) {
        return winRect->width == dispRect->width/2 && winRect->height == dispRect->height;
    }
    else if (orientation==NEXUS_VideoOrientation_e3D_OverUnder) {
        return winRect->width == dispRect->width && winRect->height == dispRect->height/2;
    }

    return false;
}

void
NEXUS_Display_GetSettings(NEXUS_DisplayHandle display, NEXUS_DisplaySettings *pSettings)
{
    BDBG_OBJECT_ASSERT(display, NEXUS_Display);
    BDBG_ASSERT(pSettings);
    *pSettings = display->cfg;
    return;
}

NEXUS_Error
NEXUS_Display_SetSettings(NEXUS_DisplayHandle display, const NEXUS_DisplaySettings *pSettings)
{
    BERR_Code rc;
    NEXUS_VideoOutput_P_Link *output, *next;
    NEXUS_Rect prevDisplayRect;
    bool formatChanged = false;

    BDBG_OBJECT_ASSERT(display, NEXUS_Display);
    BDBG_ASSERT(pSettings);

    prevDisplayRect = display->displayRect;
    if(pSettings->format != display->cfg.format) {
        formatChanged = true;

        NEXUS_Display_P_DisableVbi(display);

        if ( !NEXUS_P_VideoFormat_IsSd(pSettings->format) )
        {
            for(output=BLST_D_FIRST(&display->outputs);output;output=next)
            {
                 next = BLST_D_NEXT(output, link);
                 if( output->sdOnly )
                 {
                     /* remove unsupported outputs */
                     rc = NEXUS_Display_RemoveOutput(display, output->output);
                     if(rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc);}
                }
            }
       }
    }
    rc = NEXUS_Display_P_SetSettings(display, pSettings, false);
    if(rc!=BERR_SUCCESS) { goto err_setsettings; }

    if ((pSettings->format!=display->cfg.format) ||
        (pSettings->display3DSettings.overrideOrientation != display->cfg.display3DSettings.overrideOrientation) ||
        (pSettings->display3DSettings.orientation != display->cfg.display3DSettings.orientation))
    {
        unsigned i;
        for(i=0;i<sizeof(display->windows)/sizeof(display->windows[0]);i++) {
            NEXUS_VideoWindowHandle window = &display->windows[i];
            /* resize window */
            if (NEXUS_P_Display_RectEqual(&window->cfg.position, &prevDisplayRect) || /* preserve fullscreen */
                (window->cfg.position.x + window->cfg.position.width > display->displayRect.width) || /* window will exceed bounds of new display */
                (window->cfg.position.y + window->cfg.position.height > display->displayRect.height+2) || /* The +2 on height is needed to account for NTSC as 482 or 480 */
                NEXUS_P_Display_RectEqual3d(&window->cfg.position, &prevDisplayRect,
                    pSettings->display3DSettings.overrideOrientation?display->cfg.display3DSettings.orientation:NEXUS_VideoOrientation_e2D)) /* preserve fullscreen for halfres 3D formats on 40nm BVN */
            {
                /* the simple solution is just to make the window full screen instead of letting VDC fail.
                likely the app will want to reposition the window if the display size changes. */
                window->cfg.position = display->displayRect; /* set to fullscreen */
                if(window->vdcState.window) {
                    rc = NEXUS_VideoWindow_P_SetVdcSettings(window, &window->cfg, true);
                    if (rc!=BERR_SUCCESS) {rc = BERR_TRACE(rc);goto err_windowsettings;}
                }
            }
            if(window->input) {
                NEXUS_Display_P_VideoInputDisplayUpdate(window->input, window, pSettings);
            }
        }
    }

    {
        BFMT_AspectRatio beforeAR, afterAR;
        rc = NEXUS_P_DisplayAspectRatio_ToMagnum(display->cfg.aspectRatio, display->cfg.format, &beforeAR);
        if (!rc) {
            rc = NEXUS_P_DisplayAspectRatio_ToMagnum(pSettings->aspectRatio, pSettings->format, &afterAR);
        }
        if ( !rc && ((pSettings->format != display->cfg.format) || (beforeAR != afterAR)) )
        {
            for( output=BLST_D_FIRST(&display->outputs); NULL != output; output=BLST_D_NEXT(output, link) )
            {
                 if( output->iface.formatChange )
                 {
                     /* Update format to outputs that require knowledge (e.g. HDMI) */
                     rc = output->iface.formatChange(output->output->source, display, pSettings->format, pSettings->aspectRatio);
                     if(rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc);}
                 }
            }
        }
    }
    display->cfg = *pSettings;

    rc = nexus_p_install_display_cb(display);
    if (rc) {rc = BERR_TRACE(rc); goto err_install_display_cb;}

    rc = NEXUS_Display_P_ApplyChanges();
    if (rc) {rc = BERR_TRACE(rc);goto err_applychanges;}

    rc = NEXUS_Display_P_EnableVbi(display, pSettings->format);
    if (rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc); goto err_vbi; }

    /* PR:54526 must get sync notification after window has been configured */
    if (formatChanged)
    {
        unsigned int i;
        for(i=0;i<sizeof(display->windows)/sizeof(display->windows[0]);i++) {
            NEXUS_VideoWindowHandle window = &display->windows[i];

            if (window->syncSettings.formatCallback_isr) {
                BKNI_EnterCriticalSection();
                (*window->syncSettings.formatCallback_isr)(window->syncSettings.callbackContext, 0);
                BKNI_LeaveCriticalSection();
            }
        }
    }

err_vbi: /* ignore VBI errors */

    return BERR_SUCCESS;

err_applychanges:
err_install_display_cb:
err_setsettings:
err_windowsettings:
    {
        BERR_Code rc = BVDC_AbortChanges(pVideo->vdc);
        if (rc!=BERR_SUCCESS) {rc = BERR_TRACE(rc);}
    }
    return rc;
}

NEXUS_Error
NEXUS_DisplayModule_SetUpdateMode(NEXUS_DisplayUpdateMode updateMode)
{
    return NEXUS_DisplayModule_SetUpdateMode_priv(updateMode, NULL);
}

NEXUS_Error NEXUS_DisplayModule_SetUpdateMode_priv( NEXUS_DisplayUpdateMode updateMode, NEXUS_DisplayUpdateMode *pPrevUpdateMode )
{
    NEXUS_Error rc;

    NEXUS_ASSERT_MODULE();
    if (pPrevUpdateMode) {
        *pPrevUpdateMode = pVideo->updateMode;
    }
    switch(updateMode) {
    case NEXUS_DisplayUpdateMode_eManual:
        pVideo->updateMode = updateMode;
        break;
   case NEXUS_DisplayUpdateMode_eAuto:
        pVideo->updateMode = updateMode;
        /* keep going */
   case NEXUS_DisplayUpdateMode_eNow:
        /* Before applying changes, check for any dynamic RTS changes */
        rc = NEXUS_Vdb_P_CheckLayout();
        (void)BERR_TRACE(rc);
        rc = BVDC_ApplyChanges(pVideo->vdc);
        if (rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc); goto err_applychanges; }
        pVideo->lastUpdateFailed = false;
    }
    return NEXUS_SUCCESS;

err_applychanges:
    pVideo->lastUpdateFailed = true;
    {
        BERR_Code rc = BVDC_AbortChanges(pVideo->vdc);
        if (rc!=BERR_SUCCESS) {rc = BERR_TRACE(rc);}
    }
    return rc;
}

static void
NEXUS_Display_P_DecoderDataReady_isr(void *context, const BAVC_MFD_Picture *picture)
{
    BSTD_UNUSED(context);
    BSTD_UNUSED(picture);
    return;
}

#if NEXUS_HAS_VIDEO_ENCODER && !NEXUS_NUM_DSP_VIDEO_ENCODERS
NEXUS_Error
NEXUS_DisplayModule_SetUserDataEncodeMode_priv(NEXUS_DisplayHandle display, bool encodeUserData, BXUDlib_Settings *userDataEncodeSettings )
{
    NEXUS_Error rc = BERR_SUCCESS;
    unsigned i;

    BXUDlib_Settings *pXudSettings = (BXUDlib_Settings *)userDataEncodeSettings;

    display->encodeUserData = encodeUserData;
    if(encodeUserData) {
        display->userDataEncodeSettings = *pXudSettings;
        BXUDlib_SetSettings(display->hXud, &display->userDataEncodeSettings);
    }

    for(i=0;i<sizeof(display->windows)/sizeof(display->windows[0]);i++) {
        NEXUS_VideoWindowHandle window = &display->windows[i];
        if(window->input) {
            NEXUS_Display_P_VideoInputDisplayUpdate(window->input, window, &display->cfg);
        }
    }

    rc = nexus_p_install_display_cb(display);
    if (rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc); goto err_installcb; }
    rc = NEXUS_Display_P_ApplyChanges();
    if (rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc); goto err_applychanges; }

err_installcb:
err_applychanges:

    return rc;
}
#endif

NEXUS_Error NEXUS_Display_ConnectVideoInput( NEXUS_DisplayHandle display, NEXUS_VideoInput input )
{
#if NEXUS_HAS_VIDEO_DECODER
    BERR_Code rc;
    NEXUS_VideoDecoderDisplayConnection decoderConnect;

    BDBG_OBJECT_ASSERT(display, NEXUS_Display);
    BDBG_OBJECT_ASSERT(input, NEXUS_VideoInput);
    if (input->type != NEXUS_VideoInputType_eDecoder) {
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }
    /* similar to NEXUS_VideoInput_P_Connect, but uses BVDC_Display_GetInterruptName for interrupts.
    also lacks protection against duplicate connections, etc. app must be written right. */
    if (!pVideo->modules.videoDecoder) {
        return BERR_TRACE(NEXUS_NOT_SUPPORTED);
    }

    NEXUS_Module_Lock(pVideo->modules.videoDecoder);
    NEXUS_VideoDecoder_GetDisplayConnection_priv(input->source, &decoderConnect);
    NEXUS_Module_Unlock(pVideo->modules.videoDecoder);

    rc = BVDC_Display_GetInterruptName(display->displayVdc, BAVC_Polarity_eTopField, &decoderConnect.top.intId);
    if (rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc); goto err_intr_name;}
    rc = BVDC_Display_GetInterruptName(display->displayVdc, BAVC_Polarity_eBotField, &decoderConnect.bottom.intId);
    if (rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc); goto err_intr_name;}
    rc = BVDC_Display_GetInterruptName(display->displayVdc, BAVC_Polarity_eFrame, &decoderConnect.frame.intId);
    if (rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc); goto err_intr_name;}
    decoderConnect.callbackContext = NULL;
    decoderConnect.dataReadyCallback_isr = NEXUS_Display_P_DecoderDataReady_isr;

    NEXUS_Module_Lock(pVideo->modules.videoDecoder);
    rc = NEXUS_VideoDecoder_SetDisplayConnection_priv(input->source, &decoderConnect);
    NEXUS_Module_Unlock(pVideo->modules.videoDecoder);
    if (rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc); goto err_connect;}

    return rc;

err_connect:
err_intr_name:
    return rc;
#else
    BDBG_OBJECT_ASSERT(display, NEXUS_Display);
    BDBG_OBJECT_ASSERT(input, NEXUS_VideoInput);
    BSTD_UNUSED(NEXUS_Display_P_DecoderDataReady_isr);
    return BERR_TRACE(NEXUS_NOT_SUPPORTED);
#endif
}

void NEXUS_Display_DisconnectVideoInput( NEXUS_DisplayHandle display, NEXUS_VideoInput input )
{
#if NEXUS_HAS_VIDEO_DECODER
    BERR_Code rc;
    NEXUS_VideoDecoderDisplayConnection decoderConnect;

    BDBG_OBJECT_ASSERT(display, NEXUS_Display);
    BDBG_OBJECT_ASSERT(input, NEXUS_VideoInput);
    if (input->type != NEXUS_VideoInputType_eDecoder) {
        BERR_TRACE(NEXUS_INVALID_PARAMETER);
        return;
    }
    if (!pVideo->modules.videoDecoder) {
        BERR_TRACE(NEXUS_NOT_SUPPORTED);
        return;
    }

    NEXUS_Module_Lock(pVideo->modules.videoDecoder);
    NEXUS_VideoDecoder_GetDisplayConnection_priv(input->source, &decoderConnect);
    NEXUS_Module_Unlock(pVideo->modules.videoDecoder);

    decoderConnect.callbackContext = NULL;
    decoderConnect.dataReadyCallback_isr = NULL;
    decoderConnect.top.intId = decoderConnect.bottom.intId = decoderConnect.frame.intId = 0;

    NEXUS_Module_Lock(pVideo->modules.videoDecoder);
    rc = NEXUS_VideoDecoder_SetDisplayConnection_priv(input->source, &decoderConnect);
    NEXUS_Module_Unlock(pVideo->modules.videoDecoder);
    if (rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc); }
#else
    BDBG_OBJECT_ASSERT(display, NEXUS_Display);
    BDBG_OBJECT_ASSERT(input, NEXUS_VideoInput);
#endif
}

static void NEXUS_Display_P_VecInterrupt_isr(void *context, int param)
{
    BSTD_UNUSED(param);
    NEXUS_IsrCallback_Fire_isr(((NEXUS_DisplayHandle)context)->vsyncCallback.isrCallback);
}

static NEXUS_Error NEXUS_Display_P_ConfigVecInterrupts( NEXUS_DisplayHandle display, const NEXUS_DisplaySettings *pSettings )
{
    NEXUS_IsrCallback_Set(display->vsyncCallback.isrCallback, &pSettings->vsyncCallback);

    if (!pSettings->vsyncCallback.callback) {
        unsigned i;
        for (i=0;i<3;i++) {
            if (display->vsyncCallback.intCallback[i]) {
                BINT_DestroyCallback(display->vsyncCallback.intCallback[i]);
                display->vsyncCallback.intCallback[i] = NULL;
            }
        }
    }
    else {
        unsigned i;
        for (i=0;i<3;i++) {
            if (!display->vsyncCallback.intCallback[i]) {
                BERR_Code rc;
                BINT_Id intId;
                BAVC_Polarity pol = i==0?BAVC_Polarity_eTopField:i==1?BAVC_Polarity_eBotField:BAVC_Polarity_eFrame;

                rc = BVDC_Display_GetInterruptName(display->displayVdc, pol, &intId);
                if (rc) return BERR_TRACE(rc);

                rc = BINT_CreateCallback(&display->vsyncCallback.intCallback[i], g_pCoreHandles->bint, intId, NEXUS_Display_P_VecInterrupt_isr, display, 0);
                if (rc) return BERR_TRACE(rc);
                rc = BINT_EnableCallback(display->vsyncCallback.intCallback[i]);
                if (rc) return BERR_TRACE(rc);
            }
        }
    }

    return 0;
}

#include "bfmt_custom.h"

void NEXUS_Display_GetDefaultCustomFormatSettings(NEXUS_DisplayCustomFormatSettings *pSettings)
{
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
}

NEXUS_Error NEXUS_Display_SetCustomFormatSettings( NEXUS_DisplayHandle display, NEXUS_VideoFormat format, const NEXUS_DisplayCustomFormatSettings *pSettings )
{
    BERR_Code rc;
    BFMT_AspectRatio magnumAspectRatio;
    BFMT_VideoFmt magnumVideoFormat;
    NEXUS_DisplaySettings settings = display->cfg;
    NEXUS_VideoFormatInfo nexusVideoFormatInfo;

    if (format != NEXUS_VideoFormat_eCustom2) {
        return BERR_TRACE(NEXUS_NOT_SUPPORTED);
    }

    BDBG_OBJECT_ASSERT(display, NEXUS_Display);
    if(pVideo->updateMode != NEXUS_DisplayUpdateMode_eAuto) {rc=BERR_TRACE(NEXUS_NOT_SUPPORTED);}

    if (!pSettings) {
        return BERR_TRACE(NEXUS_NOT_SUPPORTED);
    }

    /* allocate memory. VDC expects the custom BFMT_VideoInfo to persist outside of VDC. */
    if (!display->customFormatInfo) {
        /* allocate the data structure and all its linked substructures */
        display->customFormatInfo = BKNI_Malloc(sizeof(BFMT_VideoInfo));
        display->customFormatInfo->pCustomInfo = NULL; /* no custom info */
        /* this memory is freed in NEXUS_Display_Close */
    }

    rc = NEXUS_P_DisplayAspectRatio_ToMagnum(pSettings->aspectRatio, format, &magnumAspectRatio);
    if (rc) return BERR_TRACE(rc);

    rc = NEXUS_P_VideoFormat_ToMagnum(format, &magnumVideoFormat);
    if (rc) return BERR_TRACE(rc);

    display->customFormatInfo->eVideoFmt = magnumVideoFormat;
    display->customFormatInfo->ulWidth = pSettings->width;
    display->customFormatInfo->ulHeight = pSettings->height;
    display->customFormatInfo->ulDigitalWidth = pSettings->width;
    display->customFormatInfo->ulDigitalHeight = pSettings->height;
    display->customFormatInfo->ulScanWidth = pSettings->width;
    display->customFormatInfo->ulScanHeight = pSettings->height;
    display->customFormatInfo->ulVertFreq = pSettings->refreshRate/10; /* convert from 1/1000 to 1/100 */

    display->customFormatInfo->ulPxlFreq = pSettings->width * pSettings->height * (pSettings->refreshRate/10) * 1000000;
    display->customFormatInfo->bInterlaced = pSettings->interlaced;
    display->customFormatInfo->eAspectRatio = magnumAspectRatio;
    display->customFormatInfo->pchFormatStr = "custom_format";
    /* all other members are "don't care" */

    rc = BVDC_Display_SetCustomVideoFormat(display->displayVdc, display->customFormatInfo);
    if (rc) return BERR_TRACE(rc);

    if (pSettings->aspectRatio == NEXUS_DisplayAspectRatio_eSar) {
        rc = BVDC_Display_SetSampleAspectRatio(display->displayVdc, pSettings->sampleAspectRatio.x, pSettings->sampleAspectRatio.y);
        if (rc) return BERR_TRACE(rc);
    }

    /* change this setting, as if SetSettings was called. */
    display->cfg.format = format;
    display->cfg.aspectRatio = pSettings->aspectRatio;

    BKNI_Memset(&nexusVideoFormatInfo, 0, sizeof(nexusVideoFormatInfo));
    nexusVideoFormatInfo.width = display->customFormatInfo->ulWidth;
    nexusVideoFormatInfo.height = display->customFormatInfo->ulHeight;
    nexusVideoFormatInfo.digitalWidth = display->customFormatInfo->ulDigitalWidth;
    nexusVideoFormatInfo.digitalHeight = display->customFormatInfo->ulDigitalHeight;
    nexusVideoFormatInfo.scanWidth = display->customFormatInfo->ulScanWidth;
    nexusVideoFormatInfo.scanHeight = display->customFormatInfo->ulScanHeight;
    nexusVideoFormatInfo.topActive = display->customFormatInfo->ulTopActive;
    nexusVideoFormatInfo.topMaxVbiPassThru = display->customFormatInfo->ulTopMaxVbiPassThru;
    nexusVideoFormatInfo.bottomMaxVbiPassThru = display->customFormatInfo->ulBotMaxVbiPassThru;
    nexusVideoFormatInfo.verticalFreq = display->customFormatInfo->ulVertFreq;
    nexusVideoFormatInfo.interlaced = display->customFormatInfo->bInterlaced;
    nexusVideoFormatInfo.aspectRatio = display->customFormatInfo->eAspectRatio;
    nexusVideoFormatInfo.pixelFreq = display->customFormatInfo->ulPxlFreq;
    NEXUS_P_VideoFormat_SetInfo(format, &nexusVideoFormatInfo);

    /* don't set the new format display rect here; otherwise, fullscreen video window won't be resized later; */
    display->cfg.format = NEXUS_VideoFormat_eNtsc; /* force a change -> eCustom2 */
    settings.format = format;
    return NEXUS_Display_SetSettings(display, &settings);
}

NEXUS_Error NEXUS_DisplayModule_SetAutomaticPictureQuality(void)
{
    /* no-op. we use to turn on de-interlacing, but that is now done with default values. */
    return 0;
}

NEXUS_Error NEXUS_Display_GetCrcData( NEXUS_DisplayHandle display, NEXUS_DisplayCrcData *pData, unsigned numEntries, unsigned *pNumEntriesReturned )
{
    BDBG_OBJECT_ASSERT(display, NEXUS_Display);
    *pNumEntriesReturned = 0;
    /* Coverity: 36606, FORWARD_NULL */
    if (pData == NULL)
    {
        BERR_TRACE(NEXUS_INVALID_PARAMETER);
        return 0;
    }
    /* no critical section needed for this type of producer/consumer */
    while (*pNumEntriesReturned < numEntries && display->crc.wptr != display->crc.rptr && display->crc.queue) {
        pData[*pNumEntriesReturned] = display->crc.queue[display->crc.rptr];
        (*pNumEntriesReturned)++;
        if (++display->crc.rptr == display->crc.size) {
            display->crc.rptr = 0;
        }
    }
    return 0;
}

NEXUS_Error NEXUS_DisplayModule_Standby_priv(bool enabled, const NEXUS_StandbySettings *pSettings)
{
#if NEXUS_POWER_MANAGEMENT
    unsigned i;
    BERR_Code rc=0;
    NEXUS_VideoInput_P_Link *input_link;

    BSTD_UNUSED(pSettings);

    if(!enabled) {
    rc = BRDC_Resume(pVideo->rdc);
    if (rc) { rc = BERR_TRACE(rc); goto err; }
    rc = BVDC_Resume(pVideo->vdc);
    if (rc) { rc = BERR_TRACE(rc); goto err; }
#if B_HAS_VBI
    rc = BVBI_Resume(pVideo->vbi);
    if (rc) { rc = BERR_TRACE(rc); goto err; }
#endif
    }

    for (i=0;i<sizeof(pVideo->displays)/sizeof(pVideo->displays[0]);i++) {
        NEXUS_DisplayHandle display = pVideo->displays[i];
        NEXUS_VideoOutput_P_Link *link;
        unsigned j;

        if(!display) {
            continue;
        }

        /* Open BVDC_Display and BVDC_Compositor */
        if(!enabled) {
            rc = NEXUS_Display_P_Open(display, display->index, &display->cfg);
            if (rc) {rc = BERR_TRACE(rc); goto err_apply;}

            rc = nexus_p_install_display_cb(display);
            if (rc) {rc = BERR_TRACE(rc); goto err_apply;}

            rc = NEXUS_Display_P_SetSettings(display, &display->cfg, true);
            if (rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc);goto err_apply;}

            rc = NEXUS_Display_P_ConnectVbi(display);
            if (rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc);goto err_apply;}

            NEXUS_Display_SetGraphicsSettings(display, &display->graphics.cfg);
        }

        /* Disconnect Inputs */
        for(j=0; j<sizeof(display->windows)/sizeof(display->windows[0]); j++) {
            NEXUS_VideoWindowHandle window = &display->windows[j];

            if(!window) {
            continue;
            }

            if(enabled) {
            if(window->input) {
                window->standby = window->input;
                rc = NEXUS_VideoWindow_RemoveInput(window, window->input);
            }
            } else {
            if(window->standby) {
                rc = NEXUS_VideoWindow_AddInput(window, window->standby);
                window->standby = NULL;
            }
            }
            if (rc) { rc = BERR_TRACE(rc); goto err; }
        }

        /* Disconnect Outputs */
        for(link=BLST_D_FIRST(&display->outputs);link!=NULL;link=BLST_D_NEXT(link, link)) {
            if(enabled) {
            rc = link->iface.disconnect(link->output->source, display);
            } else {
            rc = link->iface.connect(link->output->source, display);
            }
            if (rc) { rc = BERR_TRACE(rc); goto err_apply; }
        }

    rc = NEXUS_Display_P_ApplyChanges();
    if(rc) { rc = BERR_TRACE(rc); goto err_apply; }

        /* Close BVDC_Display and BVDC_Compositor */
        if(enabled) {
            NEXUS_Display_P_DisconnectVbi(display);

            /* Dont uninit graphics. Just destroy source and window */
            NEXUS_Display_P_DestroyGraphicsSource(display);

            nexus_p_uninstall_display_cb(display);

            display->cfg.vsyncCallback.callback = NULL; /* force the INT callbacks to be destroyed */
            (void)NEXUS_Display_P_ConfigVecInterrupts(display, &display->cfg);

            rc = BVDC_Display_Destroy(display->displayVdc);
            if (rc!=BERR_SUCCESS) {rc = BERR_TRACE(rc); goto err_apply;}
            rc = BVDC_Compositor_Destroy(display->compositor);
            if (rc!=BERR_SUCCESS) {rc = BERR_TRACE(rc); goto err_apply;}
        }
    }

    /*Shutdown all inputs */
    if(enabled) {
        while(1) {
            input_link = BLST_S_FIRST(&pVideo->inputs);
            if(!input_link)
            break;
            NEXUS_VideoInput_Shutdown(input_link->input);
        }
    }

    rc = NEXUS_Display_P_ApplyChanges();
    if(rc) { rc = BERR_TRACE(rc); goto err_apply; }

    if(enabled) {
#if B_HAS_VBI
    rc = BVBI_Standby(pVideo->vbi, NULL);
    if (rc) { rc = BERR_TRACE(rc); goto err; }
#endif
    rc = BVDC_Standby(pVideo->vdc, NULL);
    if (rc) { rc = BERR_TRACE(rc); goto err; }
    rc = BRDC_Standby(pVideo->rdc, NULL);
    if (rc) { rc = BERR_TRACE(rc); goto err; }
    }

    return BERR_SUCCESS;

err_apply:
    /* SW7231-349, Coverity: 35291 */
    rc = BVDC_AbortChanges(pVideo->vdc);
    rc = BERR_TRACE(rc);
err:
    return rc;
#else
    BSTD_UNUSED(enabled);
    BSTD_UNUSED(pSettings);
#endif
    return NEXUS_SUCCESS;
}

void NEXUS_Display_GetStgSettings( NEXUS_DisplayHandle display, NEXUS_DisplayStgSettings *pSettings )
{
    BDBG_OBJECT_ASSERT(display, NEXUS_Display);
    *pSettings = display->stgSettings;
}

NEXUS_Error NEXUS_Display_SetStgSettings( NEXUS_DisplayHandle display, const NEXUS_DisplayStgSettings *pSettings )
{
    BVDC_Display_StgSettings vdcSettings;
    BERR_Code rc;
    bool enabled;

    BDBG_OBJECT_ASSERT(display, NEXUS_Display);

    BKNI_Memset(&vdcSettings, 0, sizeof(vdcSettings)); /* no GetDefault */
    vdcSettings.bNonRealTime = pSettings->nonRealTime;
    if (display->cfg.timingGenerator == NEXUS_DisplayTimingGenerator_eEncoder) {
        enabled = true;
    }
    else {
        enabled = pSettings->enabled;
    }
    rc = BVDC_Display_SetStgConfiguration(display->displayVdc, enabled, &vdcSettings);
    if (rc) return BERR_TRACE(rc);

    display->stgSettings = *pSettings;

    return 0;
}

NEXUS_Error NEXUS_Display_GetStatus( NEXUS_DisplayHandle display, NEXUS_DisplayStatus *pStatus )
{
    uint32_t numWindows, numGraphics;
    BDBG_OBJECT_ASSERT(display, NEXUS_Display);
    *pStatus = display->status;
    (void)BVDC_Compositor_GetMaxWindowCount(display->compositor, &numWindows, &numGraphics);
    pStatus->numWindows = numWindows;
    pStatus->graphicsSupported = (numGraphics != 0);
    return 0;
}

static void nexus_display_p_refresh_rate_event(void *context)
{
    NEXUS_DisplayHandle display = context;
    unsigned i;
    for(i=0;i<sizeof(display->windows)/sizeof(display->windows[0]);i++) {
        NEXUS_VideoWindowHandle window = &display->windows[i];
        BKNI_EnterCriticalSection();
        NEXUS_VideoWindow_P_UpdatePhaseDelay_isr(window, display->status.refreshRate);
        BKNI_LeaveCriticalSection();
        if(window->input) {
            NEXUS_Display_P_VideoInputDisplayUpdate(window->input, window, &display->cfg);
        }
    }
}

#if NEXUS_HAS_VIDEO_ENCODER && NEXUS_NUM_DSP_VIDEO_ENCODERS
NEXUS_Error NEXUS_Display_SetEncoderCallback_priv(NEXUS_DisplayHandle display, NEXUS_DisplayEncoderSettings *pSettings)
{
    BERR_Code rc = NEXUS_SUCCESS;
    unsigned encodeRate;
    BVDC_Test_Window_CapturedImage capture;
    NEXUS_Display_P_Image *pImage;

    if(pSettings &&
       pSettings->enqueueCb_isr && pSettings->dequeueCb_isr) {
    encodeRate = NEXUS_P_RefreshRate_FromFrameRate(pSettings->encodeRate);
    BKNI_EnterCriticalSection();
    display->encoder.enqueueCb_isr = pSettings->enqueueCb_isr;
    display->encoder.dequeueCb_isr = pSettings->dequeueCb_isr;
    display->encoder.context = pSettings->context;
    display->encoder.framesEnqueued = 0;
#if 1
    display->encoder.dropRate = ((display->status.refreshRate/1000)+1) / (encodeRate/1000);
#else
    display->encoder.dropRate = 1; /* No drop ins nexus. DSP firmware will handle frame drops */
#endif
    BKNI_LeaveCriticalSection();
    rc = BVDC_Window_SetUserCaptureBufferCount(display->windows[0].vdcState.window, NEXUS_DISPLAY_ENCODER_MAX_PICTURE_BUFFERS);
    if (rc) return BERR_TRACE(rc);
    rc = BVDC_ApplyChanges(pVideo->vdc);
    if (rc) return BERR_TRACE(rc);
    rc = BVDC_Display_SetArtificticalVsync(display->displayVdc, true, pSettings->extIntAddress, 1<<pSettings->extIntBitNum);
    if (rc) return BERR_TRACE(rc);
    rc = BVDC_ApplyChanges(pVideo->vdc);
    if (rc) return BERR_TRACE(rc);
    } else {
    rc = BVDC_Display_SetArtificticalVsync(display->displayVdc, false, 0, 0);
    if (rc) {rc = BERR_TRACE(rc);}
    rc = BVDC_ApplyChanges(pVideo->vdc);
    if (rc) {rc = BERR_TRACE(rc);}
    /* Reclaim all buffers that encoder might be holding */
    while(NULL != (pImage=BLST_SQ_FIRST(&display->encoder.queued))){
        capture.pvBufAddr = (void*)pImage->addr;
        BVDC_Test_Window_ReturnBuffer_isr(display->windows[0].vdcState.window, &capture);
        BLST_SQ_REMOVE_HEAD(&display->encoder.queued, link);
        BLST_SQ_INSERT_TAIL(&display->encoder.free, pImage, link);
    }
    rc = BVDC_Window_SetUserCaptureBufferCount(display->windows[0].vdcState.window, 0);
    if (rc!=BERR_SUCCESS) {rc = BERR_TRACE(rc);}
    rc = BVDC_ApplyChanges(pVideo->vdc);
    if (rc!=BERR_SUCCESS) {rc = BERR_TRACE(rc);}
    BKNI_EnterCriticalSection();
    display->encoder.framesEnqueued = 0;
    display->encoder.enqueueCb_isr = NULL;
    display->encoder.dequeueCb_isr = NULL;
    display->encoder.context = NULL;
    BKNI_LeaveCriticalSection();
    }

    return rc;
}

NEXUS_Error NEXUS_Display_EnableEncoderCallback_priv(NEXUS_DisplayHandle display)
{
    BERR_Code rc = NEXUS_SUCCESS;
    BVDC_Display_CallbackSettings settings;

    BVDC_Display_GetCallbackSettings(display->displayVdc, &settings);
    settings.stMask.bPerVsync = 1;
    rc = BVDC_Display_SetCallbackSettings(display->displayVdc, &settings);
    if (rc) return BERR_TRACE(rc);
    rc = BVDC_ApplyChanges(pVideo->vdc);
    if (rc) return BERR_TRACE(rc);

    BKNI_EnterCriticalSection();
    display->encoder.callbackEnabled = true;
    BKNI_LeaveCriticalSection();

    return rc;
}

NEXUS_Error NEXUS_Display_DisableEncoderCallback_priv(NEXUS_DisplayHandle display)
{
    BERR_Code rc = NEXUS_SUCCESS;
    BVDC_Display_CallbackSettings settings;
    
    BKNI_EnterCriticalSection();
    display->encoder.callbackEnabled = false;
    BKNI_LeaveCriticalSection();

    BVDC_Display_GetCallbackSettings(display->displayVdc, &settings);
    settings.stMask.bPerVsync = 0;
    rc = BVDC_Display_SetCallbackSettings(display->displayVdc, &settings);
    if (rc) return BERR_TRACE(rc);
    rc = BVDC_ApplyChanges(pVideo->vdc);
    if (rc!=BERR_SUCCESS) {rc = BERR_TRACE(rc);}

    return rc;
}
#endif

void NEXUS_GetDisplayCapabilities( NEXUS_DisplayCapabilities *pCapabilities )
{
    BVDC_Display_Capabilities vdcCap;
    int rc;
    
    BKNI_Memset(pCapabilities, 0, sizeof(*pCapabilities));
    
    rc = BVDC_Display_GetCapabilities(NULL /* generic capabilities */, &vdcCap);
    if (!rc) {
        unsigned i;
        for (i=0;i<BFMT_VideoFmt_eMaxCount;i++) {
            pCapabilities->displayFormatSupported[NEXUS_P_VideoFormat_FromMagnum(i)] = vdcCap.pfIsVidfmtSupported(i);
        }
    }
}
