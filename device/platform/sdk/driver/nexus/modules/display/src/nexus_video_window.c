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
 * $brcm_Workfile: nexus_video_window.c $
 * $brcm_Revision: 198 $
 * $brcm_Date: 10/11/12 12:25p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/display/src/nexus_video_window.c $
 * 
 * 198   10/11/12 12:25p vsilyaev
 * SW7435-329: Converted NEXUS_VideoInput to NEXUS_VideoInputHandle and
 *  added object verification
 * 
 * 197   9/17/12 7:30p bandrews
 * SW7420-2356: ensure that window gets cleared settings, even if VDC
 *  window is NULL
 * 
 * 196   8/3/12 7:03p bandrews
 * SW7420-2314: merge to main
 * 
 * 195   8/2/12 4:55p erickson
 * SW7435-276: change default scaleFactorRounding.horizontalTolerance to 3
 *  to avoid 704->720 scaling
 * 
 * SW7420-2314/2   7/23/12 7:49p bandrews
 * SW7420-2314: merge from main
 * 
 * SW7420-2314/1   7/23/12 7:16p bandrews
 * SW7420-2314: add event for window source changing
 * 
 * 194   7/23/12 11:35a vsilyaev
 * SW7420-2328: Use per-thread storage to keep information about the
 *  current client, remove per-module client information
 * 
 * 193   7/9/12 3:00p erickson
 * SW7435-259: remove unused window param from NEXUS_VideoInput_P_Connect
 * 
 * 192   7/6/12 10:58a erickson
 * SW7344-342: fix error recovery if VDC window settings cannot be applied
 *  when VDC window is created
 * 
 * 191   7/3/12 5:11p bandrews
 * SW7425-717: merge to main
 * 
 * 190   6/19/12 12:42p vsilyaev
 * SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
 * 
 * 189   6/15/12 12:45p erickson
 * SW7425-3202: add NEXUS_VideoWindow_SetCompressionSettings
 * 
 * SW7425-717/5   6/11/12 7:40p bandrews
 * SW7425-717: fix bugs, add status to sync channel
 * 
 * SW7425-717/4   6/4/12 6:51p bandrews
 * SW7425-717: Added refresh rate for later; add prototype for
 *  UpdatePhaseDelay_isr
 * 
 * SW7425-717/3   6/4/12 6:35p bandrews
 * SW7425-717: fix alignment checking
 * 
 * SW7425-717/2   6/4/12 6:01p bandrews
 * SW7425-717: fix bugs in phase computation
 * 
 * SW7425-717/1   5/15/12 7:24p bandrews
 * SW7425-717: initial impl
 * 
 * 188   4/12/12 12:58p erickson
 * SW7420-2286: add NEXUS_VideoWindowMadSettings.pqEnhancement, default
 *  eOff
 * 
 * 187   3/15/12 11:41a erickson
 * SW7420-2278: default enable32Pulldown to false
 * 
 * 186   3/14/12 12:25p erickson
 * SW7401-4517: remove old code
 * 
 * 185   1/20/12 1:16p erickson
 * SW7425-2216: default deinterlacing on, assuming 1 MAD block for now
 * 
 * 184   12/21/11 12:47p erickson
 * SW7425-1971: call NEXUS_VideoInput_Shutdown when we call the last
 *  NEXUS_VideoWindow_RemoveInput to clear the copy of the parent's
 *  VDC_Source
 * 
 * 183   11/15/11 3:27p erickson
 * SW7425-1747: remove old code
 * 
 * 182   10/20/11 5:12p erickson
 * SW7420-1992: remove unnecessary priv function
 * 
 * 181   10/20/11 1:43p erickson
 * SW7420-1992: add private api's for atomically setting multiple video
 *  window settings
 *
 * 180   9/8/11 1:06p erickson
 * SW7425-1263: add
 *  NEXUS_CalculateVideoWindowPositionSettings.displayAspectRatio
 *
 * 179   9/6/11 10:41a erickson
 * SW7346-472: clean up 3D right-view buffer returned by
 *  BVDC_Window_GetBuffer
 *
 * 178   9/2/11 12:40p erickson
 * SW7420-1995: unregister handles from objdb when doing automatic close
 *
 * 177   8/8/11 5:22p mward
 * SW7125-1065:  DEADCODE:  otherWindow used only #if
 *  NEXUS_NUM_VIDEO_WINDOWS > 1.
 *
 * 176   7/26/11 3:04p randyjew
 * SW7420-1959:Use NEXUS_NUM_MOSAIC_DECODES to prevent mosaic calls
 *
 * 175   7/18/11 2:01p akam
 * SWDTV-7887: Fixed inaccurate window settings caused by rounding error.
 *
 * 174   7/15/11 11:16a jtna
 * SW7420-1959: defensive programming if clearRect disabled
 *
 * 173   7/14/11 4:35p jtna
 * SW7420-1959: added NEXUS_VideoWindowSettings.clearRect
 *
 * 172   7/12/11 11:03a erickson
 * SW7420-1973: NEXUS_VideoWindow_SetSettings should fail on mosaic parent
 *
 * 171   7/7/11 10:57a erickson
 * SW7405-5394: move BVDC_Window_SetMasterFrameRate logic into
 *  NEXUS_VideoWindow_P_ConfigMasterFrameRate, which is called from all
 *  places where it could change.
 *
 * 170   6/28/11 1:25p erickson
 * SW7405-5366: protect against div-by-zero
 *
 * 169   6/28/11 9:48a erickson
 * SW7405-5366: add NEXUS_VideoWindowStatus.phaseDelay
 *
 * 168   6/27/11 5:02p erickson
 * SW7425-466: backout mosaic zorder workaround. problem solved in
 *  nexus_display_mosaic.c instead.
 *
 * 167   5/26/11 1:42p erickson
 * SW7425-645: update definition of scaleFactorRounding in
 *  NEXUS_VideoWindowSettings
 *
 * 166   5/17/11 1:28p pntruong
 * SW7425-527, SW7425-528: Select the heap index base on window's index
 *  instead of z-order which is configurable by api.
 *
 * 165   5/13/11 9:51a erickson
 * SW7425-525: add NEXUS_VideoWindowSettings.minimumDisplayFormat
 *
 * 164   4/22/11 3:13p vsilyaev
 * SW7425-416: Updated 3D display API
 *
 * 163   4/20/11 6:11p jtna
 * SW7425-365: fix build warnings for 2.6.37 kernel
 *
 * 162   4/19/11 1:54p erickson
 * SW7405-5224: add NEXUS_VideoWindowSettings.preferSyncLock and
 *  NEXUS_VideoWindowStatus.isSyncLocked
 *
 * 161   3/4/11 1:22p erickson
 * SW7125-833: add NEXUS_VideoWindowContentMode_ePanScanWithoutCorrection
 *
 * 160   3/2/11 12:18p erickson
 * SW7420-1560: change NEXUS_CalculateVideoWindowPosition (not backward
 *  compatible) so that NEXUS_VideoWindowSettings param is not [in/out]
 *
 * 159   2/28/11 10:37a katrep
 * SW7405-5146:compiler warning
 *
 * 158   2/28/11 9:29a katrep
 * SW7405-5146:fixed compiler warning in DEBUG=n mode
 *
 * 157   12/28/10 3:57p erickson
 * SW7405-5064: added NEXUS_VideoWindowStatus.outputRect and
 *  NEXUS_VideoWindowSettings.outputRectChanged
 *
 * 156   12/2/10 3:55p erickson
 * SW7420-1227: rework
 *  NEXUS_GetDefaultCalculateVideoWindowPositionSettings
 *
 * 155   11/29/10 1:58p erickson
 * SW7405-715: remove PR 42625 bandwidth restriction. it is not general.
 *  it only applies to some chips with some RTS settings. this should not
 *  be in general purpose code.
 *
 * 154   11/18/10 7:27p spothana
 *  SW7420-1078: Assign heap index per video window and per graphics
 *  buffer.
 *
 * SW7420-1078/2   11/15/10 11:16a spothana
 * SW7420-1078: Correct the assignment for the window heaps.
 *
 * SW7420-1078/1   11/11/10 6:26p spothana
 * SW7420-1078: Assign heap index per video window and per graphics
 *  buffer.
 *
 * 153   11/1/10 10:59a erickson
 * SW7422-43: adapt to BVDC_Window_ReturnBuffer changes
 *
 * 152   10/19/10 5:39p nickh
 * SW7422-10: Allocate the PIP on the main display from MEMC0
 *
 * 151   10/18/10 5:23p petlee
 * SW35230-990: Add flag to indicate where histogram is performed
 *
 * 150   10/14/10 5:08p erickson
 * SW7420-565: init dcTable2
 *
 * 149   10/12/10 10:43a spothana
 *  SW7420-1157 : Match the VDC memory usage in the reference software to
 *  memory worksheet
 *
 * 148   9/10/10 4:35p erickson
 * SW7550-556: added NEXUS_VideoWindowSettings.scaleFactorRounding for
 *  manual control. default to automatic control.
 *
 * 147   8/3/10 5:11p spothana
 * SW7420-732: By default set the DNR/ANR blocks in bypass state rather
 *  than in disable. Toggling between disable and enable states results in
 *  flashing of video.
 *
 * 146   7/6/10 2:03p petlee
 * SW35230-484: Add FRC split screen mode
 *
 * 145   6/1/10 9:39p bandrews
 * SW3548-2323: unlinked work on 7420 with 35230, input and display only
 *  tuned on latter
 *
 * 144   5/13/10 2:06p erickson
 * SW7405-4239: with VDC algorithm improvements, first param to
 *  BVDC_Window_SetScaleFactorRounding should be zero in all cases
 *
 * 143   5/5/10 7:26p bandrews
 * SW3548-2323: updated to use apply changes if update mode says so
 *
 * 142   4/6/10 8:02p bandrews
 * SW7405-4147,SW7468-176,SW7208-12: fix initial call to sync channel so
 *  that it communicates delay, not zero
 *
 * 141   4/1/10 2:59p erickson
 * SW7405-4074: set windowCfg.bDeinterlacerAllocFull so that
 *  NEXUS_VideoWindowGameMode is easier to use across different chips
 *
 * 140   3/30/10 6:39p bandrews
 * SW3548-2323: proxy code generator doesn't like arrays as parameters to
 *  public APIs
 *
 * 139   3/30/10 4:49p bandrews
 * SW3548-2323: merge to main, need to match header
 *
 * 138   3/29/10 3:41p bandrews
 * SW3548-2323: merge to main
 *
 * SW3548-2323/7   3/8/10 8:59p bandrews
 * SW3548-2323: merge from latest
 *
 * 137   3/26/10 2:19p erickson
 * SW7405-3965: refactor mosaic to support hd/sd simul mode
 *
 * 136   3/23/10 11:14a erickson
 * SW7550-325: fix non-linear A/R when transition to/from fullscreen
 *
 * 135   3/3/10 3:00p nickh
 * SW7420-565: Write default dynamic contrast tables
 *
 * 134   2/19/10 3:30p erickson
 * SW7405-3625: fix non-mosaic capable systems
 *
 * 133   2/19/10 1:23p erickson
 * SW7405-3625: add NEXUS_VideoWindowMosaicSettings for backendMosaic
 *  support
 *
 * 132   2/17/10 1:30p erickson
 * SW3548-2776: fix settop
 *
 * 131   2/16/10 4:47p erickson
 * SW3548-2776: add
 *  NEXUS_PictureCtrlContrastStretch.dynamicBacklightScaleFactor
 *
 * 130   2/11/10 1:30p erickson
 * SW7405-3829: change impl to new vdc bShrinkWidth api
 *
 * 129   2/10/10 2:35p erickson
 * SW7405-3829: refactor internal functions to create/destroy VDC window.
 *  NEXUS_VideoWindowMadSettings.shrinkWidth requires window to be
 *  recreated.
 *
 * 128   2/3/10 12:26p erickson
 * SW3548-2727: add NEXUS_VideoWindowSettings.sourceClip
 *
 * BCM3D/3   1/29/10 11:16a erickson
 * SW3548-2727: add NEXUS_VideoWindowSettings.sourceClip
 *
 * 127   2/1/10 7:07p bandrews
 * SW7405-3783: need to notify sync channel of vdc delay on connect,
 *  otherwise it won't ever get the delay
 *
 * SW3548-2323/6   2/1/10 7:04p bandrews
 * SW7405-3783: need to notify sync channel on connect of window delay
 *
 * SW3548-2323/5   1/26/10 4:00p bandrews
 * SW3548-2323: merge from main
 *
 * 126   1/21/10 3:58p erickson
 * SW7405-3783: NEXUS_VideoWindowStatus.delay should be set even if
 *  SyncChannel is not used
 *
 * 125   1/18/10 4:55p petlee
 * SW35230-60: Temporary fix to make Nexus Display build for 35230; Remove
 *  this fix once DTV-VDC team completes port of SW7405-3718.
 *
 * 124   1/11/10 2:02p erickson
 * SW7405-3718: add NEXUS_VideoWindowAfdSettings
 *
 * 123   1/6/10 4:18p erickson
 * SW7405-3533: move window init code into NEXUS_VideoWindow_P_InitState.
 *  this allows normal and mosaic windows to init to the same defaults.
 *
 * SW3548_2323/4   12/15/09 8:50p bandrews
 * SW3548-2323: updated from VDC team's vdc_tune impl
 *
 * SW3548_2323/3   12/15/09 8:36p bandrews
 * SW3548-2323: merge from latest
 *
 * 122   12/9/09 5:00p erickson
 * SW7405-3394: fix non-mosaic
 *
 * 121   12/9/09 3:10p erickson
 * SW7405-3394: refactor mosaic mode API
 *
 * 120   12/3/09 12:08p erickson
 * SW3548-2619: fix warning
 *
 * 119   12/3/09 11:20a erickson
 * SW3548-2619: added NEXUS_VideoWindowSettings.minimumSourceFormat
 *
 * 118   11/19/09 3:48p erickson
 * SW3556-847: increasing delay in NEXUS_VideoWindow_CaptureVideoBuffer
 *
 * 117   11/10/09 5:07p mward
 * SW7400-2574: Only set split screen mode on first window of primary
 *  display also applies to 7400 for most settings.
 *
 * 116   11/10/09 2:16p nickh
 * SW7420-166:  Add support for 3rd display and DUAL_OUTPUT
 *
 * 115   11/10/09 2:39p gkamosa
 * SW3548-2609: Update NEXUS_CalculateVideoWindowPosition to detect cases
 *  where eZoom or eBox AR correction is requested and the source and
 *  display AR are the same.  In these cases, avoid un-necessary
 *  calculations on viewport or clipping as fixed point inaccuracies may
 *  cause the coordinates calculated to be out of bounds, leading to an
 *  unnecessary API failure.
 *
 * 114   11/3/09 5:23p mphillip
 * SW7405-3342: Remove assertion now that NULL settings are allowed
 *
 * 113   11/2/09 10:43a erickson
 * SW3548-2584: allow NULL color matrix for
 *  NEXUS_VideoWindow_SetColorMatrix
 *
 * 112   10/27/09 6:53p mward
 * SW7125-45: Only set split screen mode on first window of primary
 *  display.
 *
 * 111   10/27/09 10:54a erickson
 * SW3548-2577: remove bSmoothScaling. NEXUS_ScalerCaptureBias is
 *  equivalent.
 *
 * 110   10/15/09 4:31p erickson
 * SWDEPRECATED-3881: don't use deprecated struct members. use NEXUS_NUM
 *  defines
 *
 * 109   10/7/09 3:35p erickson
 * SW7405-3140: fix NEXUS_VideoWindowContentMode_ePanScan
 *
 * 108   10/5/09 1:10p erickson
 * SW3548-2527: use colorMatrixAOverride (and B/C) in
 *  NEXUS_VideoWindow_P_SetVdcSettings
 *
 * SW3548_2323/2   9/28/09 8:23p bandrews
 * 3548-2323: initial check-in of libtune port
 *
 * SW3548_2323/1   9/22/09 6:50p bandrews
 * SW3548-2323: libTune nexus passthrough
 *
 * 107   9/22/09 11:16a erickson
 * SW3548-2487: need to check layout when user changes contentMode
 *
 * 106   9/17/09 3:02p erickson
 * SW3548-2348: account for window heaps separately from input heaps
 *
 * 105   9/14/09 12:56p erickson
 * SW3548-2374: add more sourceAspectRatio support including eSar and
 *  eSquarePixel
 *
 * 104   8/26/09 5:58a erickson
 * SW3548-2374: fix negative x/y coordinates in
 *  NEXUS_CalculateVideoWindowPositionSettings.viewport
 *
 * 103   8/26/09 5:18a erickson
 * SW3548-2374: added NEXUS_CalculateVideoWindowPosition
 *
 * 102   8/19/09 11:02a erickson
 * PR57098: added NEXUS_VideoWindowSettings.delay
 *
 * 101   8/19/09 10:24a erickson
 * PR57747: add BDBG_ERR if window position/clipRect/clipBase are bad
 *
 * 100   8/10/09 10:09a erickson
 * PR56325: only feedback display information to sync-locked inputs
 *
 * 99   8/10/09 8:17a gmohile
 * PR 56400 : Merge legacy vdc support to main line
 *
 * 98   7/31/09 11:17a gmohile
 * PR 56512 : Add 7403 support
 *
 * 97   7/30/09 5:17p erickson
 * PR57216: added dejagging and deringing to
 *  NEXUS_VideoWindowSplitScreenSettings
 *
 * 96   7/23/09 4:24p jrubio
 * PR56975: make sure DNR_DEMO_MODE is not turned off when another
 *  Display/Window has it turned on
 *
 * 95   7/20/09 9:16a erickson
 * PR55237: added NEXUS_VideoWindowSettings.allocateFullScreen
 *
 * 94   6/12/09 10:50a erickson
 * PR55967: convert NEXUS_DisplayModuleSettings heap settings to integers.
 *  This makes them settable in the application.
 *
 * 93   5/20/09 3:29p erickson
 * PR54880: keep track of whether VDC heaps were created and only destroy
 *  what was created
 *
 * 92   5/19/09 10:59a erickson
 * PR54880: fix heap handling. don't assume VideoDecoder will return NULL
 *  heap for default.
 *
 * 91   4/23/09 11:28a jgarrett
 * PR 54424: Fixing continuous VDB prints related to content mode
 *  correction
 *
 * 90   4/20/09 4:37p jgarrett
 * PR 54346: Changing ForceZoom to ForceFull, adding better recovery from
 *  invalid app states.
 *
 * 89   4/15/09 11:00a jgarrett
 * PR 51743: Fixing default forceBox state
 *
 * 88   4/14/09 4:50p jgarrett
 * PR 51743: Updating BVDB_GetVideoDisplayMode to new args
 *
 * 87   4/9/09 5:21p katrep
 * PR52197: For panscan vdc aspect ratio should be set to all source.
 *
 * 86   4/9/09 2:20p erickson
 * PR53838: fix settop
 *
 * 85   4/9/09 10:43a erickson
 * PR53838: use VDB's maxFormat if specialFeature1 enabled
 *
 * 84   3/30/09 2:27p erickson
 * PR52520: downgrade WRN to MSG
 *
 * 83   3/27/09 10:31a erickson
 * PR48963: add heap and pipHeap to NEXUS_DisplayModuleSettings. allows
 *  for more platform configurability.
 *
 * 82   3/18/09 10:43a erickson
 * PR52461: set BVDC_Window_SplitScreenSettings.eAnr for all chips
 *
 * 81   3/17/09 3:42p erickson
 * PR52461: removing vdc source anr setting
 *
 * 80   3/10/09 4:30p erickson
 * PR53042: check for NULL on every call to NEXUS_VideoInput_P_Get
 *
 * 79   3/6/09 7:49p jgarrett
 * PR 52849: Fixing pixel limitations
 *
 * 78   3/2/09 6:10p jgarrett
 * PR 52520: Moving scaler settings to common functions for VDB support
 *
 * 77   2/26/09 2:32p jgarrett
 * PR 52166: Following VDB forcePending directly, no app settings allowed
 *
 * 76   2/24/09 6:24p nickh
 * PR52461: Merge 7420 ANR Nexus changes to main branch
 *
 * 7420_mcvp/3   2/24/09 6:20p nickh
 * PR52461: Restrict ANR changes to 7420
 *
 * 7420_mcvp/2   2/13/09 6:01p tdo
 * PR 45785, PR 45789: merge from main branch on 2/13/09
 *
 * 7420_mcvp/1   2/5/09 9:51a syang
 * PR 45785, PR 45789:  get rid of BVDC_Source_Get/SetAnrConfiguration
 *
 * 75   2/12/09 11:23p erickson
 * PR51976: fix NEXUS_VideoWindowSettings.autoMaster logic
 *
 * 74   1/6/09 11:33a jgarrett
 * PR 48984: Merge dynamic RTS code to main branch
 *
 * PR48984/4   12/10/08 11:29a jgarrett
 * PR 48984: Saving window settings immediately for VDB.
 *
 * PR48984/3   12/5/08 4:23p jgarrett
 * PR 48984: Updating to latest baseline
 *
 * 73   11/26/08 2:15p erickson
 * PR49203: allow unguarded BVDC_ApplyChanges for
 *  NEXUS_VideoWindow_SetSyncSettings_priv
 *
 * PR48984/2   11/26/08 6:19p jgarrett
 * PR 48984: Merging to latest baseline
 *
 * 72   11/25/08 11:26a mward
 * PR47739:  Display/Compositor 2 has only one video window.
 *
 * PR48984/1   11/20/08 6:47p jgarrett
 * PR 48984: Adding vdb restrictions
 *
 * 71   11/17/08 12:28p erickson
 * PR49203: ensure there are no unnecessary unguarded BVDC_ApplyChanges
 *  calls in Nexus
 *
 * 70   11/4/08 12:38p erickson
 * PR47030: add NEXUS_Display_P_DestroyHeap, refactor so that all VDC
 *  Sources are created by NEXUS_VideoInput_P_CreateLink
 *
 * 69   10/30/08 8:55p erickson
 * PR47030: use heap handle
 *
 * 68   10/20/08 6:55p bandrews
 * PR46915: Don't assert on NULL vdc window handle
 *
 * 67   10/7/08 10:00p erickson
 * PR47583: added ApplyChanges after BVDC_Window_ReturnBuffer
 *
 * 66   10/6/08 10:51p erickson
 * PR47583: free up memory from inside VDC for
 *  NEXUS_VideoWindow_CaptureVideoBuffer
 *
 * 65   10/6/08 10:21p erickson
 * PR40799: fix link->ref_cnt
 *
 * 64   10/5/08 8:26p erickson
 * PR47583: debug cleanup
 *
 * 63   10/2/08 12:54p erickson
 * PR40799: reorder calls in NEXUS_VideoWindow_AddInput to allow input to
 *  create/destroy BVDC_Source when connected/disconnected
 *
 * 62   9/22/08 9:54a erickson
 * PR46063: support getting format info re: Custom2 inside and outside
 *  nexus
 *
 * 61   9/17/08 12:29p erickson
 * PR47014: expose compositor blend equation in NEXUS_VideoWindowSettings
 *  and NEXUS_GraphicsSettings
 *
 * 60   9/12/08 1:28p erickson
 * PR46813: fix check for window
 *
 * 59   9/8/08 11:40a erickson
 * PR35457: remove bad comment
 *
 * 58   9/3/08 1:33p erickson
 * PR46254: added NEXUS_VideoWindowSettings.alpha
 *
 * 57   9/2/08 7:48p katrep
 * PR46457: Configure nexus features at the run time based on board strap
 *  options
 *
 * 56   8/14/08 5:26p katrep
 * PR45674: Fix compiiler warning in kernel mode non debug builds
 *
 * 55   7/25/08 12:39p erickson
 * PR42625: prevent BVN bandwidth problem when using
 *  BVDC_AspectRatioMode_eUseAllDestination
 *
 * 54   7/3/08 5:08p erickson
 * PR43699: Remove unnecessary VDC Get functions from Nexus to avoid race
 *  conditions
 *
 * 53   7/3/08 4:21p erickson
 * PR44452: closing a window should automatically remove its inputs
 *
 * 52   6/25/08 1:03p erickson
 * PR43785: set BVDC_Window_SetScaleFactorRounding to 0,0 for CRC test
 *
 * 51   6/20/08 9:33a erickson
 * PR43917: defautl bSmoothScaling = false for DTV
 *
 * 50   6/19/08 2:18p bandrews
 * PR42268: Tell sync properly about master frame rate
 *
 * 49   6/17/08 4:21p vsilyaev
 * PR 43102: Fixed warning
 *
 * 48   6/16/08 9:18a erickson
 * PR43102: merge
 *
 * PR43102/1   5/30/08 6:26p gorgon
 * PR43102: PC auto position failed in 1280x1024 with astro.
 *
 * 47   6/13/08 12:34p erickson
 * PR42625: backing out fix for now
 *
 * 46   5/29/08 1:22p erickson
 * PR43126: defer BVDC_ApplyChanges in PQ code while resetting all VDC
 *  settings for a window.
 *
 * 45   5/23/08 11:50a erickson
 * PR42998: add NEXUS_VideoWindowSettings.letterBoxAutoCut
 *
 * 44   5/23/08 10:49a erickson
 * PR42982: rework PQ Dirty logic to reapply settings on channel change
 *
 * 43   5/22/08 11:07a erickson
 * PR42625: hardcode BVDC_Window_Settings.bSmoothScaling to false to avoid
 *  BVN bandwidth problems
 *
 * 42   5/22/08 11:04a erickson
 * PR42625: remove B_HAS_LEGACY_VDC code in preperation for fix
 *
 * 41   5/9/08 12:44p erickson
 * PR39453: add sourcePending callback, set status.sourcePending based on
 *  VDC dirty bits.
 *
 * 40   5/6/08 9:51a erickson
 * PR42214: DTV wants BVDC_Window_SetScaleFactorRounding(0,0)
 *
 * 39   5/5/08 4:48p erickson
 * PR42445: provide API's for better display frame rate control
 *
 * 38   5/1/08 12:57p erickson
 * PR41202: add NEXUS_VideoWindowSettings.forceCapture
 *
 * 37   5/1/08 12:12p erickson
 * PR41202: merge
 *
 * 36   4/30/08 4:42p erickson
 * PR39453: add 3548/3556 impl
 *
 * 35   4/30/08 3:08p erickson
 * PR39453: add 3548/3556 features
 *
 * 34   4/24/08 11:07a erickson
 * PR42071: fix NEXUS_VideoWindow_AddInput and allow for future multi-
 *  module connections
 *
 * 33   4/24/08 10:51a erickson
 * PR42071: only call NEXUS_VideoInput_P_Connect if refcnt == 0
 *
 * 32   4/23/08 4:30p erickson
 * PR42071: fix NEXUS_VideoWindow_P_Teardown
 *
 * 31   4/14/08 9:17a erickson
 * PR39003: move PQ functions below SetZorder for PIP systems
 *
 * 30   4/4/08 10:43a erickson
 * PR41063: opened backdoor to BVDC_Window_SetScalerOutput. if clipBase
 *  width & height are 0, we pass clipRect directly into
 *  BVDC_Window_SetScalerOutput. usage needs to be documented in interface
 *  after it is clarified.
 *
 * 29   4/3/08 1:30p erickson
 * PR40567: merge
 *
 * PR40567/3   4/3/08 5:39p dyzhang
 * PR40567: need to translate to magnum pixel form from nexus one
 *
 * PR40567/2   4/3/08 5:35p dyzhang
 * PR40567: merge from the mainline
 *
 * 27   4/2/08 11:31a erickson
 * PR40198: fix DEBUG=n warning
 *
 * 26   4/2/08 9:16a erickson
 * PR40567: merge
 *
 * PR40567/1   4/2/08 10:35a dyzhang
 * PR40567: add capture/feeder pixel format control
 *
 * 25   4/1/08 3:32p vsilyaev
 * PR 41027: Added use fixed mapping between nexus and VDC windowId's
 *
 * 24   4/1/08 3:19p vsilyaev
 * PR 41027: Added use fixed mapping between nexus and VDC windowId's
 *
 * 23   4/1/08 1:34p erickson
 * PR40411: merge. added lb_event to drive pcinput position algo.
 *
 * 40411_take2/1   4/1/08 6:31p gorgon
 * PR40411: add letterBoxDetectionChange and GetLetterBoxStatus. Use event
 *  to triger lb.
 *
 * 22   3/27/08 6:20p vsilyaev
 * PR 40818: Added management of VDC heaps
 *
 * 21   3/26/08 3:38p vsilyaev
 * PR 40862: Added code to propagate display framerate to the decoder
 *
 * 20   3/20/08 3:00p erickson
 * PR40411: add letterBoxDetectionChange and GetLetterBoxStatus
 *
 * 19   3/11/08 2:10p erickson
 * PR40222: add NEXUS_VideoWindow_GetStatus to report video delay
 *
 * 18   3/3/08 10:34a erickson
 * PR40156: don't apply a window color matrix unless Set
 *
 * 17   3/3/08 9:21a erickson
 * PR40156: fix legacy VDC
 *
 * 16   2/29/08 5:39p erickson
 * PR40156: implement ColorMatrix function
 *
 * 15   2/28/08 8:54p bandrews
 * PR37951: Synclib must use stream progressive, not frame progressive.
 *  Master frame rate status needs to be updated before handing to sync.
 *
 * 14   2/26/08 6:48p bandrews
 * PR37951: Fixed frame rate conversion to BAVC enum
 *
 * 13   2/25/08 9:34p bandrews
 * PR37951: Apply changes must be called in order to install the callback
 *
 * 12   2/21/08 5:48p erickson
 * PR39003: PQ api change
 *
 * 11   2/20/08 11:56a erickson
 * PR39003: all !LEGACY chips now have VDC api changes
 *
 * 10   2/7/08 2:50p erickson
 * PR39003: added #if BCHP_CHIP and comment for VDC api changes
 *
 * 9   2/6/08 4:27p vsilyaev
 * PR 38682: Corrected logic for the 'window->open' flag
 *
 * 8   2/4/08 9:50a erickson
 * PR39003: update code for legacy VDC
 *
 * 7   2/4/08 9:34a erickson
 * PR38679: remove test for input->destination
 *
 * 6   1/31/08 9:55a erickson
 * PR36808: added SyncChannel interface
 *
 * 5   1/25/08 10:10a erickson
 * PR39003: fix 740x which doesn't have recent VDC change
 *
 * 4   1/25/08 9:42a erickson
 * PR39003: adapt to latest VDC
 *
 * 3   1/23/08 9:22p vobadm
 * PR35457: update docs
 *
 * 2   1/23/08 12:39p erickson
 * PR38919: rename NEXUS_Display_GetWindow to NEXUS_VideoWindow_Open
 *
 * 1   1/18/08 2:20p jgarrett
 * PR 38808: Merging to main branch
 *
 * Nexus_Devel/4   1/9/08 12:14p vsilyaev
 * PR 34662: Implemented VideoInput_GetStatus for legacy VDC
 *
 * Nexus_Devel/3   1/8/08 5:32p erickson
 * PR36159: fix 7401
 *
 * Nexus_Devel/2   1/8/08 10:57a erickson
 * PR36159: refactor some 3563 Display API's
 *
 * Nexus_Devel/1   1/3/08 8:43p erickson
 * PR34662: refactor into smaller files
 *
 ***************************************************************************/
#include "nexus_display_module.h"
#include "priv/nexus_surface_priv.h"

BDBG_MODULE(nexus_video_window);

#define BDBG_MSG_TRACE(X)

static const uint32_t aIreTable[NEXUS_DC_TABLE_COLS - 1] =  {
     64, 100, 152, 239, 327, 414, 502, 590, 677, 765, 852, 940, 956, 972, 988, 1004
};


static const uint32_t alDCTable1[NEXUS_DC_TABLE_ROWS * NEXUS_DC_TABLE_COLS] = {
    /* Used for settop-box */
     64, 64, 100, 190, 382, 550, 658, 735, 799, 826, 874, 916, 940, 940, 940, 940, 940,
     75, 64, 100, 190, 382, 550, 658, 735, 799, 826, 874, 916, 940, 940, 940, 940, 940,
    124, 64,  99, 170, 310, 442, 560, 650, 722, 790, 840, 890, 940, 940, 940, 940, 940,
    198, 64,  97, 160, 260, 406, 511, 601, 680, 753, 820, 880, 940, 940, 940, 940, 940,
    275, 64,  94, 150, 235, 360, 460, 560, 650, 735, 815, 880, 940, 940, 940, 940, 940,
    349, 64,  90, 140, 215, 328, 430, 531, 629, 725, 800, 870, 940, 940, 940, 940, 940,
    425, 64,  85, 130, 200, 305, 418, 515, 624, 718, 795, 870, 940, 940, 940, 940, 940,
    497, 64,  80, 120, 200, 292, 409, 508, 619, 711, 790, 866, 940, 940, 940, 940, 940,
    572, 64,  80, 120, 200, 290, 395, 491, 602, 700, 783, 863, 940, 940, 940, 940, 940,
    652, 64,  80, 120, 200, 290, 388, 481, 581, 684, 773, 860, 940, 940, 940, 940, 940,
    723, 64,  80, 120, 200, 288, 384, 473, 569, 676, 768, 856, 940, 940, 940, 940, 940,
    799, 64,  80, 120, 200, 282, 378, 469, 563, 661, 743, 840, 940, 940, 940, 940, 940,
    875, 64,  80, 120, 200, 277, 375, 465, 558, 659, 742, 835, 940, 940, 940, 940, 940,
    924, 64,  80, 120, 200, 275, 372, 463, 555, 650, 742, 834, 940, 940, 940, 940, 940,
    940, 64,  80, 120, 200, 275, 372, 463, 555, 650, 742, 834, 940, 940, 940, 940, 940
};

#define pVideo (&g_NEXUS_DisplayModule_State)
static NEXUS_Error NEXUS_VideoWindow_P_ApplySplitScreenSettings(NEXUS_VideoWindowHandle window);

static void
NEXUS_VideoWindow_P_LboxCallbackFunc_isr(void *pvParm1, int iParm2, const BVDC_BoxDetectInfo *pBoxDetectInfo)
{
    NEXUS_VideoWindowHandle window = pvParm1;
    bool setevent = false;
    uint32_t  ulOverflowHeight=0;

    BDBG_OBJECT_ASSERT(window, NEXUS_VideoWindow);
    BSTD_UNUSED(iParm2);

    /* for now, magnum maps straight to nexus. */
    BDBG_CASSERT(sizeof(*pBoxDetectInfo) == sizeof(window->letterBoxStatus));

    /* Set event for pc auto position if result changed */
    if ((window->letterBoxStatus.whiteBoxLeft  != pBoxDetectInfo->ulWhiteBoxLeft  )||
        (window->letterBoxStatus.whiteBoxTop   != pBoxDetectInfo->ulWhiteBoxTop   )||
        (window->letterBoxStatus.whiteBoxWidth != pBoxDetectInfo->ulWhiteBoxWidth )||
        (window->letterBoxStatus.whiteBoxHeight!= pBoxDetectInfo->ulWhiteBoxHeight))
    {
        /* don't set event until after memcpy */
        /* LB only have 10 bits to indicate the height.*/
        if((int32_t)(pBoxDetectInfo->ulWhiteBoxHeight) < 177)
        {
            ulOverflowHeight = 1024 + (int32_t)(pBoxDetectInfo->ulWhiteBoxHeight);
        }
        else
        {
            ulOverflowHeight = pBoxDetectInfo->ulWhiteBoxHeight;
        }
        if((pBoxDetectInfo->ulWhiteBoxWidth <= 1920) &&
            (ulOverflowHeight <= 1200))
        {
            setevent = true;
        }
    }

    BKNI_Memcpy(&window->letterBoxStatus, pBoxDetectInfo, sizeof(*pBoxDetectInfo));
    NEXUS_IsrCallback_Fire_isr(window->letterBoxDetectionCallback);
    if (setevent) {
        window->letterBoxStatus.whiteBoxHeight = ulOverflowHeight;
        BKNI_SetEvent(window->lb_event);
    }
}

static BERR_Code NEXUS_VideoWindow_P_RecreateWindow(NEXUS_VideoWindowHandle window)
{
    BERR_Code rc;
    NEXUS_VideoWindow_P_DestroyVdcWindow(window);
    rc = NEXUS_VideoWindow_P_CreateVdcWindow(window, &window->cfg);
    if (rc) return BERR_TRACE(rc);
    rc = NEXUS_VideoWindow_P_SetVdcSettings(window, &window->cfg, true);
    if (rc) return BERR_TRACE(rc);
    return 0;
}

#define IS_DIGITAL_SOURCE(window) ((window)->input && (window)->input->type == NEXUS_VideoInputType_eDecoder)

static BERR_Code NEXUS_VideoWindow_P_TestSyncLock(NEXUS_VideoWindowHandle window)
{
    unsigned j;
    NEXUS_VideoWindowStatus status;
    BERR_Code rc;

    rc = NEXUS_VideoWindow_GetStatus(window, &status);
    if (rc) return BERR_TRACE(rc);
    if (status.isSyncLocked) return 0;

    if (!IS_DIGITAL_SOURCE(window)) {
        BDBG_ERR(("preferSyncLock only applies to digital sources"));
        return BERR_TRACE(NEXUS_NOT_SUPPORTED);
    }

    /* if there's another digital window on this display, destroy & recreate it. then this window will become sync-locked. */
    /* if there's another digital window for this source on another display, destroy & recreate it. then this window will become sync-locked. */
    for (j=0;j<NEXUS_NUM_DISPLAYS;j++) {
        unsigned i;
        NEXUS_DisplayHandle d = pVideo->displays[j];
        if (!d) continue;
        for (i=0;i<NEXUS_NUM_VIDEO_WINDOWS;i++) {
            NEXUS_VideoWindowHandle w = &d->windows[i];
            if (!w->open || w == window) continue; /* skip closed or same window */

            /* if the other window shares the same source or
            another digital source on the same display, then it contends for sync lock. */
            if (w->input == window->input || (d == window->display && IS_DIGITAL_SOURCE(w))) {
                if (w->cfg.preferSyncLock) {
                    BDBG_WRN(("two windows have preferSyncLock = true"));
                    break;
                }
                else {
                    NEXUS_VideoWindow_P_RecreateWindow(w);
                }
                break;
            }
        }
    }
    return 0;
}

BERR_Code
NEXUS_VideoWindow_P_SetVdcSettings(NEXUS_VideoWindowHandle window, const NEXUS_VideoWindowSettings *settings, bool force)
{
    BERR_Code rc;
    BVDC_Window_Handle windowVdc = window->vdcState.window;
    bool callSync = false;
    NEXUS_DisplayUpdateMode saveUpdateMode;
    bool isFullScreen;
    BVDC_Window_Status vdcStatus;

    /* first, see if we have to destroy and recreate windows for synclock preferences */
    if (settings->preferSyncLock) {
        NEXUS_VideoWindow_P_TestSyncLock(window);
    }

    /* We've about to make a lot of VDC changes. We do not want ApplyChanges called. This will temporarily mask it.
    Be sure to restore it at the end, even in a failure case. */
    saveUpdateMode = g_NEXUS_DisplayModule_State.updateMode;
    g_NEXUS_DisplayModule_State.updateMode = NEXUS_DisplayUpdateMode_eManual;

    BDBG_ASSERT(window->vdcState.window);

    force = force || pVideo->lastUpdateFailed;

    if(force || settings->autoMaster != window->cfg.autoMaster) {
        rc = NEXUS_VideoWindow_P_ConfigMasterFrameRate(window, &window->display->cfg, settings);
        if (rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc); goto err_windowCfg;}
        callSync = true;
    }

    #if NEXUS_HAS_DYNAMIC_RTS
    /* The user setting for force capture has no effect on systems with dynamic RTS.  VDB gives us this info. */
    if(force || window->restrictions.forceCapture != window->syncStatus.forcedCaptureEnabled)
    {
        rc = BVDC_Window_SetForceCapture(windowVdc, window->restrictions.forceCapture);
        if (rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc); goto err_windowCfg;}
        window->syncStatus.forcedCaptureEnabled = window->restrictions.forceCapture;
        callSync = true;
    }
    #else
    if(force || settings->forceCapture != window->cfg.forceCapture ||
       settings->forceCapture != window->syncStatus.forcedCaptureEnabled)
    {
        /* Be aware that SyncChannel requires force capture for smooth transitions. */
        rc = BVDC_Window_SetForceCapture(windowVdc, settings->forceCapture);
        if (rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc); goto err_windowCfg;}
        window->syncStatus.forcedCaptureEnabled = settings->forceCapture;
        callSync = true;
    }
    #endif

    if(force || settings->pixelFormat != window->cfg.pixelFormat || settings->pixelFormat != window->pixelFormat ||
       !NEXUS_Vdb_P_PixelFormatValid(window->pixelFormat, window->restrictions.maxFormat))
    {
        BPXL_Format pixelFormat;
        if (settings->pixelFormat != NEXUS_PixelFormat_eUnknown) {
            NEXUS_PixelFormat nexusFormat = settings->pixelFormat;

            if ( !NEXUS_Vdb_P_PixelFormatValid(settings->pixelFormat, window->restrictions.maxFormat) )
            {
                BDBG_WRN(("Window pixel format is not supported in the current usage mode.  Reducing window pixel format: %d -> %d.",
                    settings->pixelFormat, window->restrictions.maxFormat));
                nexusFormat = window->restrictions.maxFormat;
            }

            rc = NEXUS_P_PixelFormat_ToMagnum(nexusFormat, &pixelFormat);
            if (rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc);goto err_windowCfg;}

            rc = BVDC_Window_SetPixelFormat(windowVdc, pixelFormat);
            if (rc!=BERR_SUCCESS) {rc = BERR_TRACE(rc);goto err_windowCfg;}
            window->pixelFormat = nexusFormat;
        }
        else if ( window->restrictions.maxFormat != NEXUS_PixelFormat_eUnknown )
        {
            rc = NEXUS_P_PixelFormat_ToMagnum(window->restrictions.maxFormat, &pixelFormat);
            if (rc!=BERR_SUCCESS) {rc = BERR_TRACE(rc);goto err_windowCfg;}

            rc = BVDC_Window_SetPixelFormat(windowVdc, pixelFormat);
            if (rc!=BERR_SUCCESS) {rc = BERR_TRACE(rc);goto err_windowCfg;}
            window->pixelFormat = window->restrictions.maxFormat;
        }
    }

    if(force || settings->visible != window->cfg.visible) {
        rc = BVDC_Window_SetVisibility(windowVdc, settings->visible);
        if (rc!=BERR_SUCCESS) {rc = BERR_TRACE(rc);goto err_windowCfg;}
        window->syncStatus.visible = settings->visible;
        callSync = true;
        window->layoutChanged = true;
    }

    if(force || settings->zorder != window->cfg.zorder) {
        rc = BVDC_Window_SetZOrder(windowVdc, settings->zorder);
        if (rc!=BERR_SUCCESS) {rc = BERR_TRACE(rc);goto err_windowCfg;}
        window->layoutChanged = true;
    }

    if(force || settings->alpha != window->cfg.alpha ||
                settings->sourceBlendFactor != window->cfg.sourceBlendFactor ||
                settings->destBlendFactor != window->cfg.destBlendFactor ||
                settings->constantAlpha != window->cfg.constantAlpha)
    {
        rc = BVDC_Window_SetAlpha(windowVdc, settings->alpha);
        if (rc!=BERR_SUCCESS) {rc = BERR_TRACE(rc);goto err_windowCfg;}

        BDBG_CASSERT(NEXUS_CompositorBlendFactor_eMax-1 == BVDC_BlendFactor_eOneMinusConstantAlpha);
        rc = BVDC_Window_SetBlendFactor( windowVdc, settings->sourceBlendFactor, settings->destBlendFactor, settings->constantAlpha);
        if (rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc); goto err_windowCfg;}
    }

    /* internally call SetSettings functions which may have been deferred because the VDC window didn't exist */
    if(force){
        NEXUS_VideoAdj_P_ApplySetSettings(window);
        NEXUS_PictureCtrl_P_ApplySetSettings(window);
        if (window->afdSet) {
            rc = NEXUS_VideoWindow_SetAfdSettings(window, &window->afdSettings);
            if (rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc); goto err_windowCfg;}
        }
    }

    if (force ||
        settings->letterBoxDetect != window->cfg.letterBoxDetect ||
        settings->letterBoxAutoCut != window->cfg.letterBoxAutoCut)
    {
        if (settings->letterBoxDetect == true) {
            rc = BVDC_Window_EnableBoxDetect(windowVdc, NEXUS_VideoWindow_P_LboxCallbackFunc_isr, (void *)window, 0, settings->letterBoxAutoCut);
            if (rc!=BERR_SUCCESS) {rc = BERR_TRACE(rc);goto err_windowCfg;}
        }
        else {
            rc = BVDC_Window_DisableBoxDetect(windowVdc);
            if (rc!=BERR_SUCCESS) {rc = BERR_TRACE(rc);goto err_windowCfg;}
        }
    }

    if (force || settings->delay != window->cfg.delay) {
        /* NOTE: BVDC_Window_SetDelayOffset is also set in NEXUS_VideoWindow_SetSyncSettings_priv */
        rc = BVDC_Window_SetDelayOffset(windowVdc, settings->delay + window->syncSettings.delay);
        if (rc) {return BERR_TRACE(rc);}
    }

    /* Always validate content mode when dynamic RTS is active.  Some usage modes override this setting. */
    if (force || g_NEXUS_DisplayModule_State.moduleSettings.handleDynamicRts || settings->contentMode != window->cfg.contentMode ||
        BKNI_Memcmp(&settings->scaleFactorRounding, &window->cfg.scaleFactorRounding, sizeof(settings->scaleFactorRounding)))
    {
        BVDC_PanScanType panScanType = BVDC_PanScanType_eDisable;
        BVDC_AspectRatioMode aspectRatioMode;

        if (force || settings->contentMode != window->cfg.contentMode) {
            /* if the user changed layout, we need to recompute */
            window->layoutChanged = true;
        }

        rc = BVDC_Window_SetScaleFactorRounding( windowVdc,
            settings->scaleFactorRounding.enabled?settings->scaleFactorRounding.horizontalTolerance:0,
            settings->scaleFactorRounding.enabled?settings->scaleFactorRounding.verticalTolerance:0);
        if (rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc); goto err_windowCfg; }

        switch(settings->contentMode) {
        case NEXUS_VideoWindowContentMode_ePanScan:
            panScanType = BVDC_PanScanType_eStream;
            aspectRatioMode = BVDC_AspectRatioMode_eUseAllDestination;
            break;
        case NEXUS_VideoWindowContentMode_ePanScanWithoutCorrection:
            panScanType = BVDC_PanScanType_eStream;
            aspectRatioMode = BVDC_AspectRatioMode_eBypass;
            break;
        case NEXUS_VideoWindowContentMode_eZoom:
            aspectRatioMode = BVDC_AspectRatioMode_eUseAllDestination;
            break;
        case NEXUS_VideoWindowContentMode_eBox:
            aspectRatioMode = BVDC_AspectRatioMode_eUseAllSource;
            break;
        default:
        case NEXUS_VideoWindowContentMode_eFullNonLinear:
        case NEXUS_VideoWindowContentMode_eFull:
            aspectRatioMode = BVDC_AspectRatioMode_eBypass;
            break;
        }

        if ( window->restrictions.forceBox && aspectRatioMode != BVDC_AspectRatioMode_eUseAllSource )
        {
            BDBG_WRN(("Current usage mode requires letter/pillarbox output.  Setting window content mode to NEXUS_VideoWindowContentMode_eBox."));
            aspectRatioMode = BVDC_AspectRatioMode_eUseAllSource;
        }
        else if ( window->restrictions.forceFull && aspectRatioMode != BVDC_AspectRatioMode_eBypass )
        {
            BDBG_WRN(("Current usage mode requires full output mode.  Setting window content mode to NEXUS_VideoWindowContentMode_eFull."));
            aspectRatioMode = BVDC_AspectRatioMode_eBypass;
        }

        BDBG_MSG(("window %p pan scan %d aspect ratio %d",windowVdc,panScanType,aspectRatioMode));
        rc = BVDC_Window_SetPanScanType(windowVdc, panScanType);
        if (rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc); goto err_windowCfg; }

        rc = BVDC_Window_SetAspectRatioMode(windowVdc, aspectRatioMode);
        if (rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc); goto err_windowCfg; }
    }

    isFullScreen = settings->position.x == 0 &&
                 settings->position.y == 0 &&
                 window->display->displayRect.width == settings->position.width &&
                 window->display->displayRect.height == settings->position.height;

    if (force ||
        !NEXUS_P_Display_RectEqual(&window->cfg.position, &settings->position) ||
        settings->contentMode != window->cfg.contentMode)
    {
        unsigned scalerWidth = (isFullScreen && (settings->contentMode==NEXUS_VideoWindowContentMode_eFullNonLinear)) ? settings->position.width/2 : 0;
        rc = BVDC_Window_SetNonLinearScl(windowVdc, scalerWidth, 0);
        if (rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc); goto err_windowCfg; }
    }
    if(force || !NEXUS_P_Display_RectEqual(&window->cfg.position, &settings->position) ||
            !NEXUS_P_Display_RectEqual(&window->cfg.clipBase, &settings->clipBase) ||
            !NEXUS_P_Display_RectEqual(&window->cfg.clipRect, &settings->clipRect)
            )
    {
        NEXUS_Rect scalerRect;

        callSync = true;
        window->layoutChanged = true;
        window->syncStatus.fullScreen = isFullScreen;

        rc = BVDC_Window_SetDstRect(windowVdc, settings->position.x, settings->position.y, settings->position.width, settings->position.height);
        if (rc!=BERR_SUCCESS) {rc = BERR_TRACE(rc);goto err_windowDimensions;}

        rc = NEXUS_Display_P_GetScalerRect(settings, &scalerRect);
        if (rc!=BERR_SUCCESS) {rc = BERR_TRACE(rc);goto err_windowDimensions;}

        BDBG_MSG(("NEXUS_VideoWindow_P_SetVdcSettings:%#lx scaler output [%u:%u x %u:%u]", (unsigned long)window, scalerRect.x, scalerRect.y, scalerRect.width, scalerRect.height));
        rc = BVDC_Window_SetScalerOutput(windowVdc, scalerRect.x, scalerRect.y, scalerRect.width, scalerRect.height);
        if (rc!=BERR_SUCCESS) {rc = BERR_TRACE(rc);goto err_windowDimensions;}
    }

    if(force ||
       window->cfg.sourceClip.left != settings->sourceClip.left ||
       window->cfg.sourceClip.right != settings->sourceClip.right ||
       window->cfg.sourceClip.top != settings->sourceClip.top ||
       window->cfg.sourceClip.bottom != settings->sourceClip.bottom )
    {
        rc = BVDC_Window_SetSrcClip(windowVdc, settings->sourceClip.left, settings->sourceClip.right, settings->sourceClip.top, settings->sourceClip.bottom);
        if (rc!=BERR_SUCCESS) {rc = BERR_TRACE(rc);goto err_windowDimensions;}
    }

    if (force && window->colorMatrixSet) {
        rc = BVDC_Window_SetColorMatrix(windowVdc, window->colorMatrixOverride, window->colorMatrix.coeffMatrix, window->colorMatrix.shift);
        if (rc!=BERR_SUCCESS) {rc = BERR_TRACE(rc);goto err_windowCfg;}
    }
#if NEXUS_HAS_CM3D
    if (force && window->colorMatrixASet) {
        rc = BVDC_Window_SetColorMatrixNonLinearA(windowVdc, window->colorMatrixAOverride, window->colorMatrixA.coeffMatrix, window->colorMatrixA.shift);
        if (rc!=BERR_SUCCESS) {rc = BERR_TRACE(rc);goto err_windowCfg;}
    }
    if (force && window->colorMatrixBSet) {
        rc = BVDC_Window_SetColorMatrixNonLinearB(windowVdc, window->colorMatrixBOverride, window->colorMatrixB.coeffMatrix, window->colorMatrixB.shift);
        if (rc!=BERR_SUCCESS) {rc = BERR_TRACE(rc);goto err_windowCfg;}
    }
    if (force && window->colorMatrixCSet) {
        rc = BVDC_Window_SetColorMatrixNonLinearC(windowVdc, window->colorMatrixCOverride, window->colorMatrixC.coeffMatrix, window->colorMatrixC.shift);
        if (rc!=BERR_SUCCESS) {rc = BERR_TRACE(rc);goto err_windowCfg;}
    }
#endif
#if NEXUS_NUM_MOSAIC_DECODES
    if (force || window->cfg.clearRect.enabled != settings->clearRect.enabled ||
        window->cfg.clearRect.position.x != settings->clearRect.position.x ||
        window->cfg.clearRect.position.y != settings->clearRect.position.y ||
        window->cfg.clearRect.position.width != settings->clearRect.position.width ||
        window->cfg.clearRect.position.height != settings->clearRect.position.height ||
        window->cfg.clearRect.color != settings->clearRect.color)
    {
        BVDC_MosaicConfiguration mosaicCfg;
        BKNI_Memset(&mosaicCfg, 0, sizeof(mosaicCfg));
        if (!settings->clearRect.enabled) {
            /* do the minimum if disabling */
            BVDC_Window_SetMosaicConfiguration(windowVdc, false, &mosaicCfg);
        }
        else {
            BVDC_Rect clearRect;
            bool mosaicMode;
            mosaicCfg.bVideoInMosaics = false;
            mosaicCfg.bClearRectByMaskColor = true;
            mosaicCfg.ulClearRectAlpha = (settings->clearRect.color >> 24) & 0xff;
            mosaicCfg.ulMaskColorRed = (settings->clearRect.color >> 16) & 0xff;
            mosaicCfg.ulMaskColorGreen = (settings->clearRect.color >> 8) & 0xff;
            mosaicCfg.ulMaskColorBlue = (settings->clearRect.color >> 0) & 0xff;
            BVDC_Window_SetMosaicConfiguration(windowVdc, settings->clearRect.enabled, &mosaicCfg);

            clearRect.lLeft = settings->clearRect.position.x;
            clearRect.lTop = settings->clearRect.position.y;
            clearRect.ulWidth = settings->clearRect.position.width;
            clearRect.ulHeight = settings->clearRect.position.height;
            BVDC_Window_SetMosaicDstRects(windowVdc, 1, &clearRect);

            mosaicMode = settings->clearRect.enabled;
            BVDC_Window_SetMosaicRectsVisibility(windowVdc, 1, &mosaicMode);
        }
    }
#endif

    if (force ||
        window->cfg.window3DSettings.rightViewOffset != settings->window3DSettings.rightViewOffset) {
        rc = BVDC_Window_SetDstRightRect(windowVdc, settings->window3DSettings.rightViewOffset);
        if (rc!=BERR_SUCCESS) {rc = BERR_TRACE(rc);goto err_windowCfg;}
    }

    /* this call forces the status to be valid after creation has completed, necessary for sync */
    rc = BVDC_Window_GetStatus(window->vdcState.window, &vdcStatus);
    if (rc) return BERR_TRACE(rc);

    if (force || window->status.isSyncLocked != vdcStatus.bSyncLock)
    {
        window->status.isSyncLocked = vdcStatus.bSyncLock;
        callSync = true;
    }

    if (callSync && window->syncSettings.stateChangeCallback_isr) {
        BKNI_EnterCriticalSection();
        (*window->syncSettings.stateChangeCallback_isr)(window->syncSettings.callbackContext, 0);
        BKNI_LeaveCriticalSection();
    }

    g_NEXUS_DisplayModule_State.updateMode = saveUpdateMode;
    return BERR_SUCCESS;

err_windowCfg:
    g_NEXUS_DisplayModule_State.updateMode = saveUpdateMode;
    return rc;

err_windowDimensions:
    BDBG_ERR(("Invalid window %d dimensions: position(%d,%d,%d,%d), clipRect(%d,%d,%d,%d), clipBase(%d,%d,%d,%d)",
        window->index,
        settings->position.x,settings->position.y,settings->position.width,settings->position.height,
        settings->clipRect.x,settings->clipRect.y,settings->clipRect.width,settings->clipRect.height,
        settings->clipBase.x,settings->clipBase.y,settings->clipBase.width,settings->clipBase.height));
    g_NEXUS_DisplayModule_State.updateMode = saveUpdateMode;
    return rc;
}

#define PHASE_TOLERANCE 50

void NEXUS_VideoWindow_P_UpdatePhaseDelay_isr(NEXUS_VideoWindowHandle window, unsigned refreshRate)
{
    window->status.phaseDelay = 0; /* set to zero here, if refresh rate is ready, this will get updated to real value */
    if (refreshRate)
    {
        int vsyncDelayInUs;
        int vsyncDelay;

        vsyncDelay = window->status.delay;
        vsyncDelayInUs = vsyncDelay * (1000000*1000/refreshRate);
        window->status.phaseDelay = window->phaseDelay - vsyncDelayInUs; /* subtract out 'delay' portion */
    }
}

static void NEXUS_VideoWindow_P_DelayCallback_isr(void *data, int iParm2, void * pvVdcData)
{
    NEXUS_VideoWindowHandle window = (NEXUS_VideoWindowHandle)data;
    const BVDC_Window_CallbackData * pCbData = (const BVDC_Window_CallbackData *)pvVdcData;
    const BVDC_Window_CallbackMask * pMask = &pCbData->stMask;

    BSTD_UNUSED(iParm2);

    /* record the status for GetStatus calls */
    BDBG_OBJECT_ASSERT(window, NEXUS_VideoWindow);
    
    if (pMask->bVsyncDelay)
    {
        window->status.delay = pCbData->ulVsyncDelay;
        window->syncStatus.delayValid = true;
    }

    if (pMask->bDriftDelay)
    {
        window->phaseDelay = pCbData->ulDriftDelay; /* total delay in microseconds */
        NEXUS_VideoWindow_P_UpdatePhaseDelay_isr(window, window->display->status.refreshRate);
    }
    
    if (pMask->bRectAdjust)
    {
        if (window->status.outputRect.x != pCbData->stOutputRect.lLeft ||
            window->status.outputRect.y != pCbData->stOutputRect.lTop ||
            window->status.outputRect.width != pCbData->stOutputRect.ulWidth ||
            window->status.outputRect.height != pCbData->stOutputRect.ulHeight)
        {
            window->status.outputRect.x = pCbData->stOutputRect.lLeft;
            window->status.outputRect.y = pCbData->stOutputRect.lTop;
            window->status.outputRect.width = pCbData->stOutputRect.ulWidth;
            window->status.outputRect.height = pCbData->stOutputRect.ulHeight;
            NEXUS_IsrCallback_Fire_isr(window->outputRectChangedCallback);
        }
    }

    if ((pMask->bVsyncDelay || pMask->bDriftDelay) && window->syncSettings.delayCallback_isr) {
        (*window->syncSettings.delayCallback_isr)(window->syncSettings.callbackContext, window->status.delay);
    }
}

BERR_Code
NEXUS_VideoWindow_P_CreateVdcWindow(NEXUS_VideoWindowHandle window, const NEXUS_VideoWindowSettings *cfg)
{
    BERR_Code rc;
    NEXUS_DisplayHandle display;
    BVDC_Window_Settings windowCfg;
    NEXUS_VideoInput_P_Link *link;
    BVDC_Window_CallbackSettings wcbs;

    BDBG_OBJECT_ASSERT(window->input, NEXUS_VideoInput);
    link = window->input->destination;
    BDBG_OBJECT_ASSERT(link, NEXUS_VideoInput_P_Link);
    BDBG_ASSERT(link->sourceVdc);
    BDBG_OBJECT_ASSERT(window->display, NEXUS_Display);
    display = window->display;
    BDBG_ASSERT(cfg);

    BDBG_MSG((">%s(%d) window: %ux%u display=%#x window=%#x", link->id>=BAVC_SourceId_eHdDvi0?"avc/hdmi": link->id==BAVC_SourceId_eVdec0?"analog": link->id==BAVC_SourceId_e656In0?"656": "other(mpeg?)", (int)link->id, cfg->position.width, cfg->position.height, (unsigned)display, (unsigned)window));

    rc = BVDC_Window_GetDefaultSettings(window->windowId, &windowCfg);
    if (rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc); goto err_window;}

    /*
     * This is for the cases where the application is setting the per window heap using Set Window Settings API
     */
    if (cfg->heap && cfg->heap != pVideo->heap) {
        window->vdcHeap = windowCfg.hHeap = NEXUS_Display_P_CreateHeap(cfg->heap);

    }
    else
    {
        unsigned windowHeapIndex = pVideo->moduleSettings.videoWindowHeapIndex[window->display->index][window->index];
        BDBG_MSG(("window zorder %d display index %d windowheapindex %d display main heap %d",
            window->index,window->display->index,windowHeapIndex,pVideo->moduleSettings.primaryDisplayHeapIndex ));
        if(pVideo->moduleSettings.primaryDisplayHeapIndex != windowHeapIndex)
        {
           window->vdcHeap= windowCfg.hHeap = NEXUS_Display_P_CreateHeap(g_pCoreHandles->nexusHeap[windowHeapIndex]);
        }
    }
    windowCfg.bAllocFullScreen = cfg->allocateFullScreen;

    if (cfg->minimumSourceFormat != NEXUS_VideoFormat_eUnknown) {
        BFMT_VideoFmt fmt;
        rc = NEXUS_P_VideoFormat_ToMagnum(cfg->minimumSourceFormat, &fmt);
        if (rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc); goto err_window;}
        windowCfg.pMinSrcFmt = BFMT_GetVideoFormatInfoPtr(fmt);
    }
    if (cfg->minimumDisplayFormat != NEXUS_VideoFormat_eUnknown) {
        BFMT_VideoFmt fmt;
        rc = NEXUS_P_VideoFormat_ToMagnum(cfg->minimumDisplayFormat, &fmt);
        if (rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc); goto err_window;}
        windowCfg.pMinDspFmt = BFMT_GetVideoFormatInfoPtr(fmt);
    }

    /* This causes MAD to allocate 5 fields. This allows apps to be written using NEXUS_VideoWindowGameMode_e5Fields_ForceSpatial instead of having
    to dynamically learn how many buffers are in use in order to use game mode. */
    windowCfg.bDeinterlacerAllocFull = true;

    rc = BVDC_Window_Create( display->compositor, &window->vdcState.window,
                             (display->index < 2) ? window->windowId : BVDC_WindowId_eAuto,
                             link->sourceVdc, &windowCfg);
    if (rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc); goto err_window;}
    BDBG_ASSERT(window->vdcState.window);

    rc = BVDC_Window_GetCallbackSettings(window->vdcState.window, &wcbs);
    if (rc) {rc = BERR_TRACE(rc); goto err_setcallbacksettings;}
    wcbs.stMask.bDriftDelay = true;
    wcbs.ulLipSyncTolerance = 1000; /* usec. hardcoded threshold for bDriftDelay. */
#ifndef B_HAS_LEGACY_VDC
    wcbs.stMask.bGameModeDelay = true;
#endif
    wcbs.stMask.bVsyncDelay = true;
    rc = BVDC_Window_SetCallbackSettings(window->vdcState.window, &wcbs);
    if (rc) {rc = BERR_TRACE(rc); goto err_setcallbacksettings;}

    rc = BVDC_Window_InstallCallback(window->vdcState.window, NEXUS_VideoWindow_P_DelayCallback_isr, window, 0);
    if (rc) {
        rc = BERR_TRACE(rc);
        goto err_postcreate;
    }

    /* Do not apply any Nexus settings to the window here. See NEXUS_VideoWindow_P_SetVdcSettings for that code. */

    BDBG_MSG(("<window:%#x", window->vdcState.window));
    return BERR_SUCCESS;

err_setcallbacksettings:
err_postcreate:
    BVDC_Window_Destroy(window->vdcState.window);
    window->vdcState.window = NULL;
err_window:
    return rc;
}


void
NEXUS_VideoWindow_P_DestroyVdcWindow(NEXUS_VideoWindowHandle window)
{
    BERR_Code rc;

    BDBG_ASSERT(window->vdcState.window);
    rc = BVDC_Window_Destroy(window->vdcState.window);
    if (rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc); }
    window->vdcState.window = NULL;

    /* may need to throw SetMasterFrameRate to other window */
    (void)NEXUS_VideoWindow_P_ConfigMasterFrameRate(window, &window->display->cfg, &window->cfg);

    if(g_NEXUS_DisplayModule_State.updateMode != NEXUS_DisplayUpdateMode_eAuto) { rc = BERR_TRACE(NEXUS_NOT_SUPPORTED);}
    rc = BVDC_ApplyChanges(pVideo->vdc);
    if (rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc); }
    return;
}

void
NEXUS_VideoWindow_GetSettings(NEXUS_VideoWindowHandle window, NEXUS_VideoWindowSettings *settings)
{
    BDBG_OBJECT_ASSERT(window, NEXUS_VideoWindow);
    BDBG_ASSERT(settings);
    *settings = window->cfg;
    return;
}

NEXUS_Error
NEXUS_VideoWindow_AddInput(NEXUS_VideoWindowHandle window, NEXUS_VideoInput input)
{
    NEXUS_Error rc;
    NEXUS_VideoInput_P_Link *link;

    BDBG_OBJECT_ASSERT(window, NEXUS_VideoWindow);

    NEXUS_OBJECT_ASSERT(NEXUS_VideoInput, input);

    /* verify there is no existing connection for this window */
    if (window->input!=NULL) {
        rc = BERR_TRACE(NEXUS_NOT_SUPPORTED); goto err_sanity_check;
    }

#if NEXUS_NUM_MOSAIC_DECODES
    if (window->mosaic.parent) {
        rc = NEXUS_VideoWindow_P_AddMosaicInput(window, input);
        if (rc) {rc = BERR_TRACE(rc); goto err_sanity_check;}
        /* mosaics can now be connected as a normal input/input, but do not create the VDC window */
    }
#endif

    link = NEXUS_VideoInput_P_Get(input);
    if (!link) {
        /* The only case where NEXUS_VideoInput_P_Get can fail is if this NEXUS_VideoInput is
        connected to another module. As of now, that case doesn't exist, but this check would be needed. */
        rc = BERR_TRACE(NEXUS_NOT_SUPPORTED); goto err_link;
    }

    /* we must connect before calling setup. this allows inputs to create/destroy their VDC source
    on connect/disconnect instead of requiring them to always be created. */
    if (link->ref_cnt == 0) {
        rc = NEXUS_VideoInput_P_Connect(input);
        if(rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc);goto err_connect;}
    }
    link->ref_cnt++;
    window->input = input;
    BDBG_ASSERT(input->destination == link);

    if (link->sync.connectionChangedCallback_isr)
    {
        BKNI_EnterCriticalSection();
        link->sync.connectionChangedCallback_isr(link->sync.callbackContext, 0);
        BKNI_LeaveCriticalSection();
    }

#if NEXUS_NUM_MOSAIC_DECODES
    if (!window->mosaic.parent)
#endif
    {
        rc = NEXUS_VideoWindow_P_CreateVdcWindow(window, &window->cfg);
        if(rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc);goto err_setup;}
        BDBG_ASSERT(window->vdcState.window);

        /* now that the window is created, we can apply settings */
        rc = NEXUS_VideoWindow_P_SetVdcSettings(window, &window->cfg, true);
        if (rc!=BERR_SUCCESS) {rc = BERR_TRACE(rc);goto err_setsettings;}

        rc = NEXUS_Display_P_ApplyChanges();
        if (rc!=BERR_SUCCESS) {rc = BERR_TRACE(rc);goto err_applychanges;}
    }

    BKNI_EnterCriticalSection();
    NEXUS_VideoInput_P_TriggerSourceChanged_isr(link, false);
    BKNI_LeaveCriticalSection();

    /* update display settings on the input side */
    BDBG_ASSERT(window->display);
    NEXUS_Display_P_VideoInputDisplayUpdate(input, window, &window->display->cfg);

#if NEXUS_NUM_MOSAIC_DECODES
    if (window->mosaic.parent) {
        rc = NEXUS_VideoWindow_P_ApplyMosaic(window->mosaic.parent);
        if (rc) {rc = BERR_TRACE(rc);} /* fall through */
    }
#endif
    NEXUS_OBJECT_ACQUIRE(window, NEXUS_VideoInput, input);

    return NEXUS_SUCCESS;

err_applychanges:
err_setsettings:
    NEXUS_VideoWindow_P_DestroyVdcWindow(window);
err_setup:
    BDBG_ASSERT(link->ref_cnt>0);
    if(--link->ref_cnt==0) {
        NEXUS_VideoInput_P_Disconnect(window->input);
    }
err_connect:
    {
        BERR_Code rc = BVDC_AbortChanges(pVideo->vdc);
        if (rc!=BERR_SUCCESS) {rc = BERR_TRACE(rc);}
    }
    window->input = NULL;
err_link:
err_sanity_check:
    return rc;
}

NEXUS_Error
NEXUS_VideoWindow_RemoveInput(NEXUS_VideoWindowHandle window, NEXUS_VideoInput input)
{
    NEXUS_VideoInput_P_Link *link;

    BDBG_OBJECT_ASSERT(window, NEXUS_VideoWindow);
    BDBG_OBJECT_ASSERT(input, NEXUS_VideoInput);
    if(window->input!=input) { return BERR_TRACE(NEXUS_INVALID_PARAMETER); }

    link = NEXUS_VideoInput_P_Get(window->input);
    if (!link) {
        BDBG_ERR(("NEXUS_VideoWindow_RemoveInput:%#lx invalid input %#lx", (unsigned long)window, (unsigned long)window->input));
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }

#if NEXUS_NUM_MOSAIC_DECODES
    if (window->mosaic.parent) {
        NEXUS_VideoWindow_P_RemoveMosaicInput(window, input);
    }
    else 
#endif
    {
        NEXUS_VideoWindow_P_DestroyVdcWindow(window);
    }

    if (link->sync.connectionChangedCallback_isr)
    {
        BKNI_EnterCriticalSection();
        link->sync.connectionChangedCallback_isr(link->sync.callbackContext, 0);
        BKNI_LeaveCriticalSection();
    }

    BDBG_ASSERT(link->ref_cnt>0);
    if (--link->ref_cnt==0) {
        NEXUS_VideoInput_P_Disconnect(window->input);
        
#if NEXUS_NUM_MOSAIC_DECODES
        /* because each mosaic's link copies the VDC source of the parent, we must shutdown the mosaic on 
        the last disconnect to clear the copy. */
        if (window->mosaic.parent) {
            NEXUS_VideoInput_Shutdown(input);
        }
#endif
    }
    
    window->input = NULL;

    if (window->vdcHeap) {
        NEXUS_Display_P_DestroyHeap(window->vdcHeap);
        window->vdcHeap = NULL;
    }
    NEXUS_OBJECT_RELEASE(window, NEXUS_VideoInput, input);

    return NEXUS_SUCCESS;
}

void
NEXUS_VideoWindow_RemoveAllInputs(NEXUS_VideoWindowHandle window)
{
    NEXUS_Error rc;

    BDBG_OBJECT_ASSERT(window, NEXUS_VideoWindow);

    if(window->input) {
        rc = NEXUS_VideoWindow_RemoveInput(window, window->input);
        if (rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc);}
    }
    return;
}

NEXUS_Error
NEXUS_VideoWindow_SetSettings(NEXUS_VideoWindowHandle window, const NEXUS_VideoWindowSettings *settings)
{
    BERR_Code rc = BERR_SUCCESS;

    BDBG_OBJECT_ASSERT(window, NEXUS_VideoWindow);
    BDBG_OBJECT_ASSERT(window->display, NEXUS_Display);
    BDBG_ASSERT(settings);

#if NEXUS_NUM_MOSAIC_DECODES
    /* don't allow this if a mosaic parent */
    if (BLST_S_FIRST(&window->mosaic.children)) {
        return BERR_TRACE(NEXUS_NOT_AVAILABLE);
    }
#endif

    if (window->vdcState.window)
    {
        NEXUS_VideoWindowSettings oldSettings;
        rc = NEXUS_VideoWindow_P_SetVdcSettings(window, settings, false);
        if (rc!=BERR_SUCCESS) {rc = BERR_TRACE(rc);goto err_setsettings;}

        /* We must save the settings here for VDB to read them in ApplyChanges.  If ApplyChanges fails, we will roll back. */
        oldSettings = window->cfg;
        window->cfg = *settings;

        rc = NEXUS_Display_P_ApplyChanges();
        if (rc!=BERR_SUCCESS)
        {
            rc = BERR_TRACE(rc);
            window->cfg = oldSettings;
            goto err_applychanges;
        }
    }
    else
    {
        /* Just store the settings */
        window->cfg = *settings;
    }

    if (window->letterBoxDetectionCallback) {
        NEXUS_IsrCallback_Set(window->letterBoxDetectionCallback, &settings->letterBoxDetectionChange);
    }
    if (window->outputRectChangedCallback) {
        NEXUS_IsrCallback_Set(window->outputRectChangedCallback, &settings->outputRectChanged);
    }

#if NEXUS_NUM_MOSAIC_DECODES
    if (window->mosaic.parent) {
        NEXUS_VideoWindow_P_ApplyMosaic(window->mosaic.parent);
    }
#endif
    return rc;
err_applychanges:
err_setsettings:
    {
        BERR_Code rc = BVDC_AbortChanges(pVideo->vdc);
        if (rc!=BERR_SUCCESS) {rc = BERR_TRACE(rc);}
    }
    return rc;
}

NEXUS_SurfaceHandle NEXUS_VideoWindow_CaptureVideoBuffer( NEXUS_VideoWindowHandle window )
{
    BVDC_Window_Handle  windowVDC;
    BVDC_Window_CapturedImage captureBuffer;
    BERR_Code rc = BERR_SUCCESS;
    NEXUS_SurfaceHandle surface;
    unsigned tries = 5;

    BDBG_ENTER(NEXUS_VideoWindow_CaptureVideoBuffer);

    BDBG_OBJECT_ASSERT(window, NEXUS_VideoWindow);
    windowVDC = window->vdcState.window;

    if (NULL == windowVDC) /* window not connected */
    {
        BDBG_ERR(("Window not connnected"));
        BDBG_LEAVE(NEXUS_VideoWindow_CaptureVideoBuffer);
        rc = BERR_TRACE(NEXUS_NOT_SUPPORTED);
        return NULL;
    }

    if (window->captureBuffer) {
        BDBG_ERR(("must return currently captured surface before capturing another"));
        return NULL;
    }

    /* set capture buffers used, should apply changes to take effect */
    rc = BVDC_Window_SetUserCaptureBufferCount(windowVDC, 1);
    if (rc) {rc=BERR_TRACE(rc);return NULL;}

    if(g_NEXUS_DisplayModule_State.updateMode != NEXUS_DisplayUpdateMode_eAuto) { rc = BERR_TRACE(NEXUS_NOT_SUPPORTED);}
    rc = BVDC_ApplyChanges(g_NEXUS_DisplayModule_State.vdc);
    if (rc) {rc=BERR_TRACE(rc);return NULL;}

    /* now that we've allocated a user capture buffer inside VDC, we must succeed or goto error_free_buffer to free that buffer. */

    /* Get the video buffer from VDC.
       Note that BVDC_Window_GetBuffer will set captureBuffer.hCaptureBuffer even if rc is non-zero.
       This is atypical behavior but must be accounted for. */
    while (tries--) {
        rc = BVDC_Window_GetBuffer(windowVDC, &captureBuffer);
        if (rc == BVDC_ERR_NO_AVAIL_CAPTURE_BUFFER) {
            /* Only a buffer at certain postion can be returned and it must have valid picture captured on it. Without this, tearing occurs.
            This tries just a few times with a slight delay. */
            BKNI_Delay(10 * 1000);
        }
        else {
            break;
        }
    }
    if ((rc != BERR_SUCCESS) || (captureBuffer.hCaptureBuffer == NULL))
    {
        rc = BERR_TRACE(rc);
        goto error_free_buffer;
    }

    /* store for later return to VDC. a new nexus API is needed to return it to the user. */
    window->hRCaptureBuffer = captureBuffer.hRCaptureBuffer;

    NEXUS_Module_Lock(g_NEXUS_DisplayModule_State.modules.surface);
    surface = NEXUS_Surface_CreateFromMagnum_priv(captureBuffer.hCaptureBuffer);
    NEXUS_Module_Unlock(g_NEXUS_DisplayModule_State.modules.surface);
    if(!surface) {
        BDBG_ERR(("unknown surface. was this a video capture buffer?"));
        goto error_free_buffer;
    }

    /* Save the buffer in the window private context */
    window->captureBuffer = surface;

    BDBG_LEAVE(NEXUS_VideoWindow_CaptureVideoBuffer);

    return surface;

error_free_buffer:
    if (captureBuffer.hCaptureBuffer != NULL) {
        rc = BVDC_Window_ReturnBuffer(windowVDC, &captureBuffer);
        if (rc) {rc = BERR_TRACE(rc);} /* fall through */
        window->hRCaptureBuffer = NULL;
        rc = BVDC_ApplyChanges(g_NEXUS_DisplayModule_State.vdc);
        if (rc) {rc = BERR_TRACE(rc);} /* fall through */
    }
    rc = BVDC_Window_SetUserCaptureBufferCount(windowVDC, 0);
    if (rc) {rc = BERR_TRACE(rc);} /* fall through */
    rc = BVDC_ApplyChanges(g_NEXUS_DisplayModule_State.vdc);
    if (rc) {rc = BERR_TRACE(rc);} /* fall through */
    return NULL;

}   /* NEXUS_VideoWindow_CaptureVideoBuffer() */

NEXUS_Error NEXUS_VideoWindow_ReleaseVideoBuffer( NEXUS_VideoWindowHandle window, NEXUS_SurfaceHandle surface )
{
    BVDC_Window_Handle  windowVDC;
    BERR_Code rc = BERR_SUCCESS;
    BSUR_Surface_Handle magnumSurface;

    BDBG_ENTER(NEXUS_VideoWindow_ReleaseVideoBuffer);

    BDBG_OBJECT_ASSERT(window, NEXUS_VideoWindow);
    windowVDC = window->vdcState.window;
    BDBG_ASSERT(surface);

    if(surface != window->captureBuffer) {
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }

    if (NULL == windowVDC) /* window not connected */
    {
        BDBG_ERR(("Window not connnected"));
        BDBG_LEAVE(NEXUS_VideoWindow_ReleaseVideoBuffer);
        return BERR_TRACE(NEXUS_UNKNOWN);
    }

    NEXUS_Module_Lock(g_NEXUS_DisplayModule_State.modules.surface);
    magnumSurface = NEXUS_Surface_GetSurface_priv(surface);
    NEXUS_Surface_ReleaseSurface_priv(surface);
    NEXUS_Module_Unlock(g_NEXUS_DisplayModule_State.modules.surface);
    BDBG_ASSERT(magnumSurface);
    window->captureBuffer = NULL;

    {
        BVDC_Window_CapturedImage captureBuffer;
        BKNI_Memset(&captureBuffer, 0, sizeof(captureBuffer));
        captureBuffer.hCaptureBuffer = magnumSurface;
        captureBuffer.hRCaptureBuffer = window->hRCaptureBuffer;
        rc = BVDC_Window_ReturnBuffer(windowVDC, &captureBuffer);
        if (rc) return BERR_TRACE(rc);
        window->hRCaptureBuffer = NULL;
    }
    if(g_NEXUS_DisplayModule_State.updateMode != NEXUS_DisplayUpdateMode_eAuto) { rc = BERR_TRACE(NEXUS_NOT_SUPPORTED);}
    rc = BVDC_ApplyChanges(g_NEXUS_DisplayModule_State.vdc);
    if (rc) return BERR_TRACE(rc);
    rc = BVDC_Window_SetUserCaptureBufferCount(windowVDC, 0);
    if (rc) return BERR_TRACE(rc);
    rc = BVDC_ApplyChanges(g_NEXUS_DisplayModule_State.vdc);
    if (rc) return BERR_TRACE(rc);

    return NEXUS_SUCCESS;
}

void
NEXUS_VideoWindow_GetSplitScreenSettings(
    NEXUS_VideoWindowHandle window,
    NEXUS_VideoWindowSplitScreenSettings *pSettings)
{
    *pSettings = window->splitScreenSettings;
}

NEXUS_Error
NEXUS_VideoWindow_SetSplitScreenSettings(
    NEXUS_VideoWindowHandle window,
    const NEXUS_VideoWindowSplitScreenSettings *pSettings
    )
{
    NEXUS_Error rc;
    BDBG_OBJECT_ASSERT(window, NEXUS_VideoWindow);
    BDBG_ASSERT(pSettings);

    /* save for later */
    if (!window->vdcState.window) {
        window->splitScreenSettings = *pSettings;
        return NEXUS_SUCCESS;
    }

    window->splitScreenSettings = *pSettings;
    rc = NEXUS_VideoWindow_P_ApplySplitScreenSettings(window);
    if (rc) return BERR_TRACE(rc);

    rc = NEXUS_Display_P_ApplyChanges();
    if (rc) return BERR_TRACE(rc);

    return 0;
}

static NEXUS_Error NEXUS_VideoWindow_P_ApplySplitScreenSettings(NEXUS_VideoWindowHandle window)
{
    BERR_Code rc;
    BVDC_Window_Handle windowVDC;
    NEXUS_VideoInput_P_Link *pLink;
    BVDC_Window_SplitScreenSettings winSettings;
    BVDC_Source_SplitScreenSettings srcSettings;
    NEXUS_VideoWindowSplitScreenSettings *pSettings = &window->splitScreenSettings;
    int i =0,j=0;

    BDBG_ENTER(NEXUS_VideoWindow_SetSplitScreenSettings);

    windowVDC = window->vdcState.window;
    BDBG_ASSERT(windowVDC);
    pLink = NEXUS_VideoInput_P_Get(window->input);

    /* Check for 1st window of primary display (PEP_WINDOW restriction in BVDC_Window_SetSplitScreenMode) */
    if ((window->display->index == 0) && (window->windowId == BVDC_WindowId_eVideo0))
    {
        (void)BVDC_Window_GetSplitScreenMode(windowVDC, &winSettings);
        winSettings.eHue = pSettings->hue;
        winSettings.eContrast = pSettings->contrast;
        winSettings.eBrightness = pSettings->brightness;
        winSettings.eColorTemp = pSettings->colorTemp;

        winSettings.eAutoFlesh = pSettings->autoFlesh;
        winSettings.eSharpness = pSettings->sharpness;
        winSettings.eBlueBoost = pSettings->blueBoost;
        winSettings.eGreenBoost = pSettings->greenBoost;
        winSettings.eBlueStretch = pSettings->blueStretch;
        winSettings.eCms = pSettings->cms;
        winSettings.eContrastStretch = pSettings->contrastStretch;
        winSettings.eCm3d = pSettings->cm3d;
        winSettings.eAnr = pSettings->anr;
        winSettings.eDeJagging = pSettings->dejagging;
        winSettings.eDeRinging = pSettings->deringing;
        rc = BVDC_Window_SetSplitScreenMode(windowVDC, &winSettings);
        if (rc) return BERR_TRACE(rc);
    }

    if (pLink && pLink->sourceVdc) {
        (void)BVDC_Source_GetSplitScreenMode(pLink->sourceVdc, &srcSettings);

        srcSettings.eDnr = BVDC_SplitScreenMode_eDisable;

        for (i=0;i<NEXUS_NUM_DISPLAYS && srcSettings.eDnr==BVDC_SplitScreenMode_eDisable;i++) {
            for (j=0;j<NEXUS_NUM_VIDEO_WINDOWS;j++) {
                if (pVideo->displays[i] && pVideo->displays[i]->windows[j].input == pLink->input) {
                    if (pVideo->displays[i]->windows[j].splitScreenSettings.dnr) {
                        srcSettings.eDnr = pVideo->displays[i]->windows[j].splitScreenSettings.dnr;
                        BDBG_MSG(("display %d window %d DNR set to %d",i, j, srcSettings.eDnr));
                        break;
                    }
                }
            }
        }

        rc = BVDC_Source_SetSplitScreenMode(pLink->sourceVdc, &srcSettings);
        if (rc) return BERR_TRACE(rc);
    }
    /* don't ApplyChanges here */
    return NEXUS_SUCCESS;
}

NEXUS_VideoWindowHandle
NEXUS_VideoWindow_Open(NEXUS_DisplayHandle display, unsigned windowIndex)
{
    BERR_Code rc;
    NEXUS_VideoWindowHandle window;
    BDBG_OBJECT_ASSERT(display, NEXUS_Display);
    if(windowIndex>=sizeof(display->windows)/sizeof(display->windows[0])) {
        rc = BERR_TRACE(BERR_NOT_SUPPORTED);
        return NULL;
    }
    window = &display->windows[windowIndex];

    if (window->open) {
        BDBG_ERR(("window already open"));
        return NULL;
    }
    NEXUS_OBJECT_SET(NEXUS_VideoWindow, window);
    window->open = true;
    window->letterBoxDetectionCallback = NEXUS_IsrCallback_Create(window, NULL);
    window->outputRectChangedCallback = NEXUS_IsrCallback_Create(window, NULL);
    window->index = windowIndex; /* have explicit window->index instead of VDC's windowId for dereferencing arrays, etc. */
    BDBG_CASSERT(BVDC_WindowId_eVideo0 == 0);
    BDBG_CASSERT(BVDC_WindowId_eVideo1 == 1);
    window->windowId = windowIndex + BVDC_WindowId_eVideo0;
    /* By default, there are no restrictions on capabilities.  Dynamic RTS may enforce this later. */
    window->restrictions.mnrBnrPermitted = window->restrictions.dcrPermitted = window->restrictions.anrPermitted = window->restrictions.madPermitted = true;
    window->restrictions.madFieldStore = 0xffffffff;
    window->restrictions.anrMaxFormat = window->restrictions.madMaxFormat = window->restrictions.maxFormat = NEXUS_PixelFormat_eUnknown;
    window->restrictions.forceCapture = false;
    window->restrictions.forceBox = window->restrictions.forceFull = false;
    window->rtsId = (unsigned)-1; /* Invalid */

    if (BKNI_CreateEvent(&window->lb_event)) {
        BDBG_ERR(("window lb event fault"));
        return NULL;
    }

#if NEXUS_NUM_MOSAIC_DECODES
    BLST_S_INIT(&window->mosaic.children);
#endif

    return window;
}

static void
NEXUS_VideoWindow_P_Release(NEXUS_VideoWindowHandle window)
{
    if (!window->open) {
        BDBG_ERR(("invalid window"));
    }
    window->open = false;
    NEXUS_VideoWindow_RemoveAllInputs(window);
#if NEXUS_NUM_MOSAIC_DECODES
    if (window->mosaic.parent==NULL) {
        NEXUS_VideoWindowHandle mosaicChild;
        while ((mosaicChild = BLST_S_FIRST(&window->mosaic.children))) {
            BDBG_WRN(("NEXUS_VideoWindow_Close is automatically closing mosaic window %p", mosaicChild));
            NEXUS_OBJECT_UNREGISTER(NEXUS_VideoWindow, mosaicChild, Close);
            NEXUS_VideoWindow_Close(mosaicChild);
        }
    }
#endif
    return;
}

static void
NEXUS_VideoWindow_P_Finalizer(NEXUS_VideoWindowHandle window)
{
    NEXUS_OBJECT_ASSERT(NEXUS_VideoWindow, window);

#if NEXUS_NUM_MOSAIC_DECODES
    if (window->mosaic.parent) {
        BLST_S_REMOVE(&window->mosaic.parent->mosaic.children, window, NEXUS_VideoWindow, mosaic.link);
        BDBG_OBJECT_DESTROY(window, NEXUS_VideoWindow);
        BKNI_Free(window);
    }
    else
#endif
    {
        NEXUS_IsrCallback_Destroy(window->letterBoxDetectionCallback);
        NEXUS_IsrCallback_Destroy(window->outputRectChangedCallback);
        BKNI_DestroyEvent(window->lb_event);
    }
}

NEXUS_OBJECT_CLASS_MAKE_WITH_RELEASE(NEXUS_VideoWindow, NEXUS_VideoWindow_Close);

void NEXUS_VideoWindow_GetSyncSettings_priv( NEXUS_VideoWindowHandle window, NEXUS_VideoWindowSyncSettings *pSyncSettings )
{
    BDBG_OBJECT_ASSERT(window, NEXUS_VideoWindow);
    NEXUS_ASSERT_MODULE();
    *pSyncSettings = window->syncSettings;
}

NEXUS_Error NEXUS_VideoWindow_SetSyncSettings_priv( NEXUS_VideoWindowHandle window, const NEXUS_VideoWindowSyncSettings *pSyncSettings )
{
    BERR_Code rc;
    BVDC_Window_Handle windowVdc = window->vdcState.window;
    bool hookingUp = false;

    BDBG_OBJECT_ASSERT(window, NEXUS_VideoWindow);
    NEXUS_ASSERT_MODULE();

    hookingUp = (window->syncSettings.stateChangeCallback_isr == NULL &&
        pSyncSettings->stateChangeCallback_isr != NULL);
    
    window->syncSettings = *pSyncSettings;
    
    if (windowVdc)
    {
        rc = BVDC_Window_SetDelayOffset(windowVdc, window->cfg.delay + window->syncSettings.delay);
        if (rc) {return BERR_TRACE(rc);}

#if NEXUS_NUM_DISPLAYS > 1
        rc = NEXUS_Display_P_Align(window->display, window->display->cfg.alignmentTarget);
        if (rc) return BERR_TRACE(rc);
#endif

        rc = BVDC_ApplyChanges(g_NEXUS_DisplayModule_State.vdc);
        if (rc) {return BERR_TRACE(rc);}

        /* fire some callbacks to set SyncChannel's initial state */
        if (hookingUp) {
            BKNI_EnterCriticalSection();
            if (window->syncSettings.stateChangeCallback_isr) {
                (*window->syncSettings.stateChangeCallback_isr)(window->syncSettings.callbackContext, 0);
            }
            if (window->syncSettings.formatCallback_isr) {
                (*window->syncSettings.formatCallback_isr)(window->syncSettings.callbackContext, 0);
            }
            if (window->syncSettings.delayCallback_isr) {
                (*window->syncSettings.delayCallback_isr)(window->syncSettings.callbackContext, window->status.delay);
            }
            BKNI_LeaveCriticalSection();
        }
    }
    else
    {
        BDBG_MSG(("SetSyncSettings called with NULL VDC window"));
    }

    return 0;
}

NEXUS_Error NEXUS_VideoWindow_GetSyncStatus_isr( NEXUS_VideoWindowHandle window, NEXUS_VideoWindowSyncStatus *pSyncStatus )
{
    BFMT_VideoInfo video_format_info;
    unsigned long ulFrameRate;
    NEXUS_Error rc;

    BDBG_OBJECT_ASSERT(window, NEXUS_VideoWindow);

    /* translate display status on demand */
    rc = NEXUS_P_Display_GetMagnumVideoFormatInfo(window->display, window->display->cfg.format, &video_format_info);
    if (rc) {return BERR_TRACE(rc);}
    /* master frame rate sync status is set in ConfigMasterFrameRate */
    window->syncStatus.height = video_format_info.ulDigitalHeight;
    window->syncStatus.interlaced = video_format_info.bInterlaced;
    window->syncStatus.delay = window->status.delay;
    window->syncStatus.phaseDelay = window->status.phaseDelay;
    window->syncStatus.syncLocked = window->status.isSyncLocked;
    window->syncStatus.aligned = window->display->cfg.alignmentTarget && window->display->cfg.alignmentTarget != window->display;
    window->syncStatus.refreshRate = window->display->status.refreshRate;
    ulFrameRate = video_format_info.bInterlaced ? video_format_info.ulVertFreq / 2 : video_format_info.ulVertFreq;
    switch (ulFrameRate) {
    case 2397: window->syncStatus.frameRate = BAVC_FrameRateCode_e23_976; break;
    case 2400: window->syncStatus.frameRate = BAVC_FrameRateCode_e24; break;
    case 2500: window->syncStatus.frameRate = BAVC_FrameRateCode_e25; break;
    case 2997: window->syncStatus.frameRate = BAVC_FrameRateCode_e29_97; break;
    case 3000: window->syncStatus.frameRate = BAVC_FrameRateCode_e30; break;
    case 5000: window->syncStatus.frameRate = BAVC_FrameRateCode_e50; break;
    case 5994: window->syncStatus.frameRate = BAVC_FrameRateCode_e59_94; break;
    case 6000: window->syncStatus.frameRate = BAVC_FrameRateCode_e60; break;
    default: window->syncStatus.frameRate = BAVC_FrameRateCode_eUnknown; break;
    }

    *pSyncStatus = window->syncStatus;

    return 0;
}

void NEXUS_VideoWindow_GetColorMatrix( NEXUS_VideoWindowHandle window, NEXUS_ColorMatrix *pSettings)
{
    BDBG_OBJECT_ASSERT(window, NEXUS_VideoWindow);
    *pSettings = window->colorMatrix;
}

NEXUS_Error NEXUS_VideoWindow_SetColorMatrix( NEXUS_VideoWindowHandle window, const NEXUS_ColorMatrix *pSettings )
{
    BDBG_OBJECT_ASSERT(window, NEXUS_VideoWindow);

    if (pSettings) {
        window->colorMatrix = *pSettings;
    }
    window->colorMatrixSet = true;
    window->colorMatrixOverride = pSettings != NULL;
    if (window->vdcState.window) {
        BERR_Code rc;
        rc = BVDC_Window_SetColorMatrix(window->vdcState.window, window->colorMatrixOverride, window->colorMatrix.coeffMatrix, window->colorMatrix.shift);
        if (rc) return BERR_TRACE(rc);

        rc = NEXUS_Display_P_ApplyChanges();
        if (rc) return BERR_TRACE(rc);
    }
    return 0;
}

NEXUS_Error NEXUS_VideoWindow_GetStatus( NEXUS_VideoWindowHandle window, NEXUS_VideoWindowStatus *pStatus)
{
    BDBG_OBJECT_ASSERT(window, NEXUS_VideoWindow);

    /* window->status is incrementally updated from various isr callbacks from VDC, so we
    start with this copy */
    *pStatus = window->status;

    /* now query immediate status */
    if (window->vdcState.window) {
        BVDC_Window_Status vdcStatus;
        BERR_Code rc;
        rc = BVDC_Window_GetStatus(window->vdcState.window, &vdcStatus);
        if (rc) return BERR_TRACE(rc);
        pStatus->isSyncLocked = vdcStatus.bSyncLock;
    }
    else {
        pStatus->isSyncLocked = false;
    }

    return 0;
}


NEXUS_Error NEXUS_VideoWindow_GetLetterBoxStatus( NEXUS_VideoWindowHandle window, NEXUS_VideoWindowLetterBoxStatus *pStatus )
{
    *pStatus = window->letterBoxStatus;
    return 0;
}

void NEXUS_GetDefaultCalculateVideoWindowPositionSettings( NEXUS_CalculateVideoWindowPositionSettings *pSettings )
{
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    pSettings->manualAspectRatioCorrection = NEXUS_VideoWindowContentMode_eFull;
    pSettings->sourceAspectRatio = NEXUS_AspectRatio_e4x3;
    pSettings->viewport.width = 1920;
    pSettings->viewport.height = 1080;
    pSettings->displayAspectRatio = NEXUS_DisplayAspectRatio_eAuto;
}

NEXUS_Error NEXUS_CalculateVideoWindowPosition( const NEXUS_CalculateVideoWindowPositionSettings *pPosition,
    const NEXUS_VideoWindowSettings *pInputWindowSettings, NEXUS_VideoWindowSettings *pWindowSettings)
{
    unsigned clip;
    NEXUS_Rect viewport = pPosition->viewport; /* may need to modify for manual a/r correction */
    unsigned verticalClipping = pPosition->verticalClipping; /* may need to modify for manual a/r correction */
    unsigned horizontalClipping = pPosition->horizontalClipping; /* may need to modify for manual a/r correction */
    unsigned displayAr, sourceAr; /* aspect ratio in 1/100th's units (e.g. 4:3 = 4/3 = 133 */
    unsigned aspectNumerator, aspectDenominator;

    *pWindowSettings = *pInputWindowSettings;

    if (!pPosition->displayWidth || !pPosition->displayHeight) {
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }

    /* store AR as width/height in 1/100ths.
    This has the advantage of already being in units of a horizontal percentage for clipping-based zoom. */
    switch (pPosition->displayAspectRatio) {
    case NEXUS_DisplayAspectRatio_eAuto:
        displayAr = pPosition->displayWidth * 100 / pPosition->displayHeight;
        if (!displayAr) return BERR_TRACE(NEXUS_INVALID_PARAMETER);
        break;
    case NEXUS_DisplayAspectRatio_e4x3:
        displayAr = 4 * 100 / 3;
        break;
    case NEXUS_DisplayAspectRatio_e16x9:
        displayAr = 16 * 100 / 9;
        break;
    default:
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }

    switch (pPosition->sourceAspectRatio) {
    case NEXUS_AspectRatio_e4x3:
        sourceAr = 400 / 3;
        aspectNumerator = 4;
        aspectDenominator = 3;
        break;
    case NEXUS_AspectRatio_e16x9:
        sourceAr = 1600 / 9;
        aspectNumerator = 16;
        aspectDenominator = 9;
        break;
    case NEXUS_AspectRatio_eSquarePixel:
        /* square pixel is the same as SAR 1:1 */
        if (pPosition->sourceHeight) {
            sourceAr = pPosition->sourceWidth * 100 / pPosition->sourceHeight;
            aspectNumerator = pPosition->sourceWidth;
            aspectDenominator = pPosition->sourceHeight;
        }
        else {
            return BERR_TRACE(NEXUS_INVALID_PARAMETER);
        }
        break;
    case NEXUS_AspectRatio_e221x1:
        sourceAr = 221;
        aspectNumerator = 221;
        aspectDenominator = 100;
        break;
    case NEXUS_AspectRatio_e15x9:
        sourceAr = 1500 / 9;
        aspectNumerator = 15;
        aspectDenominator = 9;
        break;
    case NEXUS_AspectRatio_eSar:
        if (pPosition->sampleAspectRatio.x && pPosition->sampleAspectRatio.y && pPosition->sourceWidth && pPosition->sourceHeight) {
            sourceAr = pPosition->sourceWidth * 100 * pPosition->sampleAspectRatio.x / pPosition->sampleAspectRatio.y / pPosition->sourceHeight;
            aspectNumerator = pPosition->sourceWidth * pPosition->sampleAspectRatio.x;
            aspectDenominator = pPosition->sourceHeight * pPosition->sampleAspectRatio.y;
        }
        else {
            return BERR_TRACE(NEXUS_INVALID_PARAMETER);
        }
        break;
    default:
        sourceAr = 0;
        aspectNumerator = 0;
        aspectDenominator = 1;
        break;
    }

    /**
    Manual aspect ratio correction involves either changes in the viewport or in clipping.
    **/
    switch (pPosition->manualAspectRatioCorrection) {
    case NEXUS_VideoWindowContentMode_eBox:
        if (!sourceAr) {
            return BERR_TRACE(NEXUS_INVALID_PARAMETER);
        }

        /* if display and source ARs are equal, no letter/pillar boxing required */
        if(displayAr != sourceAr)
        {
            if (displayAr > sourceAr) {
                /* pillar box */
                unsigned w = viewport.height * aspectNumerator / aspectDenominator;
                viewport.x += (viewport.width - w) / 2;
                viewport.width = w;
            }
            else {
                /* letter box */
                unsigned h = viewport.width * aspectDenominator / aspectNumerator;
                viewport.y += (viewport.height - h) / 2;
                viewport.height = h;
            }
        }
        pWindowSettings->contentMode = NEXUS_VideoWindowContentMode_eFull;
        break;
    case NEXUS_VideoWindowContentMode_eZoom:
        if (!sourceAr) {
            return BERR_TRACE(NEXUS_INVALID_PARAMETER);
        }

        /* if display and source ARs are equal, no adjustment required */
        if(displayAr != sourceAr)
        {
            if (displayAr > sourceAr) {
                /* vertical clipping - convert to height/width, then do the 1/100ths based math */
                verticalClipping += (100 * 100 / sourceAr) - (100 * 100 / displayAr);
            }
            else {
                /* horizontal clipping - units of sourceAr & displayAr are ready for direct math */
                horizontalClipping += sourceAr - displayAr;
            }
        }
        pWindowSettings->contentMode = NEXUS_VideoWindowContentMode_eFull;
        break;
    case NEXUS_VideoWindowContentMode_eFull:
        pWindowSettings->contentMode = NEXUS_VideoWindowContentMode_eFull;
        break;
    case NEXUS_VideoWindowContentMode_eFullNonLinear:
    case NEXUS_VideoWindowContentMode_ePanScan:
    case NEXUS_VideoWindowContentMode_ePanScanWithoutCorrection:
        /* only auto a/r correction supported for these modes */
        return BERR_TRACE(NEXUS_NOT_SUPPORTED);
    default:
        /* invalid value */
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }


    /* use display coordinates as clipBase */
    pWindowSettings->clipBase.x = 0; /* unused */
    pWindowSettings->clipBase.y = 0; /* unused */
    pWindowSettings->clipBase.width = pPosition->displayWidth;
    pWindowSettings->clipBase.height = pPosition->displayHeight;

    /* start off with no clipping */
    pWindowSettings->clipRect = pWindowSettings->clipBase;

    /* apply user's clipping */
    clip = pWindowSettings->clipBase.width * horizontalClipping / 10000;
    pWindowSettings->clipRect.x += clip/2;
    pWindowSettings->clipRect.width -= clip;
    clip = pWindowSettings->clipBase.height * verticalClipping / 10000;
    pWindowSettings->clipRect.y += clip/2;
    pWindowSettings->clipRect.height -= clip;

    /* apply h/v position while maintaining the viewport and not changing scaling.
    let x/y go negative for now. */
    pWindowSettings->clipRect.x -= pPosition->horizontalPosition;
    pWindowSettings->clipRect.y -= pPosition->verticalPosition;

    /* convert viewport to window position, making adjustments to clipping as needed */
    pWindowSettings->position = viewport;

    /* check if offscreen */
    if (viewport.x >= (int)pPosition->displayWidth ||
        viewport.x + viewport.width <= 0 ||
        viewport.y >= (int)pPosition->displayHeight ||
        viewport.y + viewport.height <= 0)
    {
        pWindowSettings->position = pWindowSettings->clipRect = pWindowSettings->clipBase;
        pWindowSettings->position.width = pWindowSettings->position.height = 0;
    }
    else {
        if (viewport.x < 0) {
            int temp;
            pWindowSettings->position.x = 0;
            pWindowSettings->position.width += viewport.x;

            temp = (-viewport.x) * pWindowSettings->clipBase.width / viewport.width;
            pWindowSettings->clipRect.x += temp;
            pWindowSettings->clipRect.width -= temp;
        }
        if (viewport.y < 0) {
            int temp;
            pWindowSettings->position.y = 0;
            pWindowSettings->position.height += viewport.y;

            temp = (-viewport.y) * pWindowSettings->clipBase.height / viewport.height;
            pWindowSettings->clipRect.y += temp;
            pWindowSettings->clipRect.height -= temp;
        }
        if (viewport.x + viewport.width > (int)pPosition->displayWidth) {
            pWindowSettings->position.width -= viewport.x + viewport.width - pPosition->displayWidth;
            pWindowSettings->clipRect.width -= (viewport.width - pWindowSettings->position.width) * pWindowSettings->clipBase.width / viewport.width;
        }
        if (viewport.y + viewport.height > (int)pPosition->displayHeight) {
            pWindowSettings->position.height -= viewport.y + viewport.height - pPosition->displayHeight;
            pWindowSettings->clipRect.height -= (viewport.height - pWindowSettings->position.height) * pWindowSettings->clipBase.height / viewport.height;
        }
    }

    BDBG_MSG_TRACE(("NEXUS_CalculateVideoWindowPosition %d,%d,%d,%d; %d,%d,%d,%d; %d,%d,%d,%d",
        pWindowSettings->position.x, pWindowSettings->position.y, pWindowSettings->position.width, pWindowSettings->position.height,
        pWindowSettings->clipRect.x, pWindowSettings->clipRect.y, pWindowSettings->clipRect.width, pWindowSettings->clipRect.height,
        pWindowSettings->clipBase.x, pWindowSettings->clipBase.y, pWindowSettings->clipBase.width, pWindowSettings->clipBase.height));
    /* verify that our math is right */
    BDBG_ASSERT(pWindowSettings->position.x >= 0);
    BDBG_ASSERT(pWindowSettings->position.y >= 0);
    BDBG_ASSERT(pWindowSettings->position.width <= pPosition->displayWidth);
    BDBG_ASSERT(pWindowSettings->position.height <= pPosition->displayHeight);

    return 0;
}

void NEXUS_VideoWindow_P_InitState(NEXUS_VideoWindowHandle window, unsigned index, NEXUS_DisplayHandle display)
{
    BKNI_Memset(window, 0, sizeof(*window));
    BDBG_OBJECT_SET(window, NEXUS_VideoWindow);
    window->open = false;
    window->display = display;
    /* TODO: when nexus can know the number of deinterlacers, we can default more on. for now, we assume one. */
    if (index == 0 && window->display->index == 0) {
        window->adjContext.stMadSettings.deinterlace = true;
        window->adjContext.bMadSet = true;
    }
    window->adjContext.stMadSettings.pixelFormat = NEXUS_PixelFormat_eUnknown; /* eUnknown allows VDB to set the optimal format */
    window->adjContext.stMadSettings.pqEnhancement = NEXUS_MadPqEnhancement_eOff; /* eOff means MAD stays on for 480i->480i and 1080i->1080i which prevents audio glitch because of lipsync adjustment. */
    window->adjContext.stAnrSettings.pixelFormat = NEXUS_PixelFormat_eUnknown; /* eUnknown allows VDB to set the optimal format */
    /* default to bypassing filtering. Disabling and Enabling causes flashes, but enabling and bypassing does not*/
    window->adjContext.stAnrSettings.anr.mode = NEXUS_VideoWindowFilterMode_eBypass;
    window->adjContext.stAnrSettings.anr.level = -(BVDC_QP_ADJUST_STEPS-1); /* minimum filter value*/
    window->adjContext.stDnrSettings.bnr.mode = NEXUS_VideoWindowFilterMode_eBypass;
    window->adjContext.stDnrSettings.bnr.level = -(BVDC_QP_ADJUST_STEPS-1);
    window->adjContext.stDnrSettings.dcr.mode = NEXUS_VideoWindowFilterMode_eBypass;
    window->adjContext.stDnrSettings.dcr.level =  -(BVDC_QP_ADJUST_STEPS-1);
    window->adjContext.stDnrSettings.mnr.mode = NEXUS_VideoWindowFilterMode_eBypass;
    window->adjContext.stDnrSettings.mnr.level = -(BVDC_QP_ADJUST_STEPS-1);
    window->adjContext.stSclSettings.horizontalLumaDeringing = true;
    window->adjContext.stSclSettings.verticalLumaDeringing = true;
    window->adjContext.stSclSettings.horizontalChromaDeringing = true;
    window->adjContext.stSclSettings.verticalChromaDeringing = true;
#if NEXUS_DISPLAY_DEINTERLACER_HALFBITS_PER_PIXEL
    window->adjContext.compressionSettings.mad.enabled = (NEXUS_DISPLAY_DEINTERLACER_HALFBITS_PER_PIXEL > 0);
    window->adjContext.compressionSettings.mad.compressionAmount = NEXUS_DISPLAY_DEINTERLACER_HALFBITS_PER_PIXEL;
#endif
#ifdef NEXUS_DYNAMIC_BACKLIGHT_SCALE_FACTOR
    window->picContext.stCustomContrast.dynamicBacklightScaleFactor = NEXUS_DYNAMIC_BACKLIGHT_SCALE_FACTOR; /* full scaling */
#endif
    window->cfg.autoMaster = (index == 0);
    window->cfg.alpha = 0xFF;
    window->cfg.contentMode =  NEXUS_VideoWindowContentMode_eFull;
    window->cfg.sourceBlendFactor = NEXUS_CompositorBlendFactor_eSourceAlpha;
    window->cfg.destBlendFactor = NEXUS_CompositorBlendFactor_eInverseSourceAlpha;
    window->cfg.constantAlpha = 0xFF;
    window->cfg.visible = true;
    window->cfg.zorder = index;
    window->cfg.scaleFactorRounding.enabled = true;
    window->cfg.scaleFactorRounding.horizontalTolerance = 3; /* only scale if >3%. for instance, 704 will not be scaled to 720. */
#if NEXUS_CRC_CAPTURE
    window->cfg.scaleFactorRounding.verticalTolerance = 0;
#else
    window->cfg.scaleFactorRounding.verticalTolerance = 3; /* only scale if >3%. for instance, 480 will not be scaled to 482. */
#endif

    BKNI_Memcpy(&window->picContext.stCustomContrast.dcTable1[0], &alDCTable1[0], sizeof(window->picContext.stCustomContrast.dcTable1));
    BKNI_Memcpy(&window->picContext.stCustomContrast.dcTable2[0], &alDCTable1[0], sizeof(window->picContext.stCustomContrast.dcTable2));
    BKNI_Memcpy(&window->picContext.stCustomContrast.ireTable[0], &aIreTable[0], sizeof(window->picContext.stCustomContrast.ireTable));

    /* We default forceCapture on for set-tops because it's usually needed for HD/SD VEC sync and all usage modes allow it.
    We don't default forceCapture on for DTV because it can't be used in some usage modes. */
    window->cfg.forceCapture = true;
#if NEXUS_NUM_MOSAIC_DECODES
    window->mosaic.mosaicSettings.backendMosaic.clipRect.width = 1920;
    window->mosaic.mosaicSettings.backendMosaic.clipRect.width = 1080;
    window->mosaic.mosaicSettings.backendMosaic.clipBase = window->mosaic.mosaicSettings.backendMosaic.clipRect;
#endif
}

void NEXUS_VideoWindow_GetAfdSettings( NEXUS_VideoWindowHandle window, NEXUS_VideoWindowAfdSettings *pSettings )
{
    BDBG_OBJECT_ASSERT(window, NEXUS_VideoWindow);
    *pSettings = window->afdSettings;
}

NEXUS_Error NEXUS_VideoWindow_SetAfdSettings( NEXUS_VideoWindowHandle window, const NEXUS_VideoWindowAfdSettings *pSettings )
{
    BDBG_OBJECT_ASSERT(window, NEXUS_VideoWindow);

    window->afdSettings = *pSettings;
    window->afdSet = true;

    if (window->vdcState.window) {
        BERR_Code rc;

        BDBG_CASSERT(NEXUS_AfdMode_eUser == (NEXUS_AfdMode)BVDC_AfdMode_eUser);
        BDBG_CASSERT(NEXUS_AfdClip_eOptionalLevel2 == (NEXUS_AfdClip)BVDC_AfdClip_eOptionalLevel2);
        BDBG_CASSERT(sizeof(NEXUS_VideoWindowAfdSettings) == sizeof(BVDC_AfdSettings));
        rc = BVDC_Window_SetAfdSettings(window->vdcState.window, (const BVDC_AfdSettings *)(const NEXUS_VideoWindowAfdSettings *)&window->afdSettings);
        if (rc) return BERR_TRACE(rc);

        rc = NEXUS_Display_P_ApplyChanges();
        if (rc) return BERR_TRACE(rc);
    }

    return 0;
}

/**
set this window's "SetMasterFrameRate" setting.
if true, we may need to set another window false. if false, we may need to set another window true.
this function must be called from all places in nexus where these conditions may change.
**/
NEXUS_Error NEXUS_VideoWindow_P_ConfigMasterFrameRate(NEXUS_VideoWindowHandle window, const NEXUS_DisplaySettings *pDisplaySettings, const NEXUS_VideoWindowSettings *pWindowsettings)
{
    BERR_Code rc;
    bool masterFrameRate;

    /* pDisplaySettings->frameRateMaster takes precedence */
    masterFrameRate = (pDisplaySettings->frameRateMaster ? pDisplaySettings->frameRateMaster == window->input : pWindowsettings->autoMaster);

    /* if vdc window destroyed, we can throw SetMasterFrameRate to the other window as well */
    masterFrameRate = masterFrameRate && window->vdcState.window;

#if 0
    /* TODO: masterFrameRate could be forced false if this window is not sync-locked. but that info is currently not available immediately after window creation. */
    if (masterFrameRate) {
        BVDC_Window_Status vdcStatus;
        rc = BVDC_Window_GetStatus(window->vdcState.window, &vdcStatus);
        if (rc) return BERR_TRACE(rc);
        masterFrameRate = masterFrameRate && vdcStatus.bSyncLock;
        BDBG_MSG_TRACE(("synclock %d", vdcStatus.bSyncLock));
    }
#endif

    BDBG_MSG_TRACE(("setting SetMasterFrameRate(%d, %d -> %d) based on %d %d %d", window->index, window->vdcState.masterFrameRate, masterFrameRate,
        pDisplaySettings->frameRateMaster, pWindowsettings->autoMaster, window->vdcState.window));

    if (masterFrameRate != window->vdcState.masterFrameRate) {
#if NEXUS_NUM_VIDEO_WINDOWS > 1
        NEXUS_VideoWindowHandle otherWindow = NULL;

        /* only one window per display can have SetMasterFrameRate(true). main overrides pip. */
        otherWindow = &window->display->windows[1 - window->index];
        if (!otherWindow->open) otherWindow = NULL;
#endif

        if (masterFrameRate) {
            /* find any conflict and turn off SetMasterFrameRate on another window */
#if NEXUS_NUM_VIDEO_WINDOWS > 1
            if (otherWindow && otherWindow->vdcState.window && otherWindow->vdcState.masterFrameRate) {
                if (otherWindow->index == 1) {
                    rc = BVDC_Window_SetMasterFrameRate(otherWindow->vdcState.window, false);
                    if (rc) return BERR_TRACE(rc);
                    otherWindow->vdcState.masterFrameRate = false;
                }
                else {
                    /* don't allow pip to override main */
                    masterFrameRate = false;
                }
            }
#endif

            rc = BVDC_Window_SetMasterFrameRate(window->vdcState.window, masterFrameRate);
            if (rc) return BERR_TRACE(rc);
        }
        else {
            if (window->vdcState.window) {
                rc = BVDC_Window_SetMasterFrameRate(window->vdcState.window, false);
                if (rc) return BERR_TRACE(rc);
            }

            /* throw SetMasterFrameRate to other window if it wants it. it's ok if it's sync-slip. */
#if NEXUS_NUM_VIDEO_WINDOWS > 1
            if (otherWindow && otherWindow->vdcState.window && otherWindow->cfg.autoMaster && !otherWindow->vdcState.masterFrameRate) {
                rc = BVDC_Window_SetMasterFrameRate(otherWindow->vdcState.window, true);
                if (rc) return BERR_TRACE(rc);
                otherWindow->vdcState.masterFrameRate = true;
            }
#endif
        }
        window->vdcState.masterFrameRate = masterFrameRate;
    }

    window->syncStatus.masterFrameRateEnabled = masterFrameRate;

    return 0;
}

NEXUS_Error NEXUS_VideoWindow_SetSettings_priv( NEXUS_VideoWindowHandle handle, const NEXUS_VideoWindowSettings *pSettings )
{
    NEXUS_ASSERT_MODULE();
    /* call _impl version */
    return NEXUS_VideoWindow_SetSettings(handle, pSettings);
}
