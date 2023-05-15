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
 * $brcm_Workfile: nexus_video_input.c $
 * $brcm_Revision: 155 $
 * $brcm_Date: 10/11/12 12:25p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/display/src/nexus_video_input.c $
 * 
 * 155   10/11/12 12:25p vsilyaev
 * SW7435-329: Converted NEXUS_VideoInput to NEXUS_VideoInputHandle and
 *  added object verification
 * 
 * 154   10/10/12 10:32a erickson
 * SW7408-346: support graphics-only build
 * 
 * 153   9/11/12 1:49p ahulse
 * SW7425-2172: Update image input to use MFD SetSurface support
 * 
 * 152   8/7/12 12:55p gmohile
 * SW7425-3668 : Disconnect BVN interrupt to video decoder
 * 
 * 151   8/3/12 7:03p bandrews
 * SW7420-2314: merge to main
 * 
 * SW7420-2314/2   7/23/12 7:45p bandrews
 * SW7420-2314: merge from main
 * 
 * SW7420-2314/1   7/23/12 7:16p bandrews
 * SW7420-2314: add event for window source changing
 * 
 * 150   7/13/12 3:39p erickson
 * SW7425-3107: refactor cc/tt/gs capture to prevent isr race-condition,
 *  prevent cpu-reordering failure, and to reduce code duplication
 * 
 * 149   7/9/12 3:00p erickson
 * SW7435-259: remove unused window param from NEXUS_VideoInput_P_Connect
 * 
 * 148   6/11/12 1:06p erickson
 * SW7346-815: SW7346-815
 * 
 * 147   6/6/12 11:55a hongtaoz
 * SW7435-7: added the 4th MFD input for source callback;
 * 
 * 146   3/23/12 12:07p jtna
 * SW7346-725: use NEXUS_VideoInput_P_GetExisting() to avoid creating VDC
 *  source when not needed
 * 
 * 145   3/14/12 12:25p erickson
 * SW7401-4517: remove old code
 * 
 * 144   2/13/12 6:28p hongtaoz
 * SW7425-2386: fixing coverity defect;
 * 
 * 143   1/31/12 5:01p gmohile
 * SW7425-1473 : Fix raaga encoder support
 * 
 * 142   12/21/11 12:48p erickson
 * SW7425-1971: improve debug, remove unused code
 * 
 * 141   12/16/11 5:28p erickson
 * SW7425-1878: merge XUDlib support for video encoder userdata
 * 
 * SW7425-1878/5   12/16/11 7:55p ajagmag
 * SW7425-1878: simple cleanup
 * 
 * SW7425-1878/4   12/16/11 5:13p ajagmag
 * SW7425-1878: Updated for easier merge
 * 
 * 140   12/16/11 10:17a erickson
 * SW7425-1877: clarify VideoInput connection logic, add TODO's for future
 *  simplification
 * 
 * SW7425-1878/3   12/16/11 2:11p ajagmag
 * SW7425-1878: Update based on discussions with David E.
 * 
 * SW7425-1878/2   12/15/11 10:31a ajagmag
 * SW7425-1878: changes for user data encode use case
 * 
 * SW7425-1878/1   12/15/11 7:20a ajagmag
 * SW7425-1878: Bug fixes for user data encode use case
 * 
 * 139   10/20/11 10:44a erickson
 * SW7425-1546: set display->status.refreshRate when format changes
 * 
 * 138   10/19/11 5:00p hongtaoz
 * SW7425-1546: update display refresh rate according to current display
 *  status; set stgIndex correctly for dual;
 * 
 * 137   10/18/11 6:15p jtna
 * SW7425-1531: add helper code to resize video windows when switching
 *  between half-res 3D formats
 * 
 * 136   7/8/11 1:17p erickson
 * SW7346-306: VideoImageInput must set eInterruptRefreshRate with display
 *  format
 * 
 * 135   6/29/11 10:43a erickson
 * SW7425-466: clean up on failed NEXUS_VideoWindow_AddInput
 * 
 * 134   6/23/11 2:52p hongtaoz
 * SW7425-466: add 3x decodes support;
 * 
 * 133   6/14/11 3:11p vsilyaev
 * SW7425-654: Propagate STG/trigger index from display to the video
 *  decoder
 * 
 * 132   5/16/11 5:38p jtna
 * SW7422-446: add correct fix
 * 
 * 131   5/16/11 5:33p jtna
 * SW7422-446: pass in correct setting to BVDC_Source_SetOrientation()
 * 
 * 130   4/22/11 3:13p vsilyaev
 * SW7425-416: Updated 3D display API
 * 
 * 129   12/10/10 5:06p erickson
 * SW35230-2209: add NEXUS_VideoDecoderSettings.scanMode
 * 
 * 128   8/24/10 4:27p erickson
 * SW3548-2795: ForcePending should set sourcePending status to true
 *
 * 127   8/24/10 8:40a erickson
 * SW35230-1059: fix uninitialized data caused by NEXUS_VideoInputSettings
 *  change. use a GetDefaultSettings instead of static initialization.
 *
 * 126   7/8/10 9:30a erickson
 * SW3548-3021: don't call VDB when in NEXUS_VideoInputResumeMode_eManual.
 *  VDC doesn't need new RTS applied until
 *  NEXUS_VideoInputResumeMode_eNow. this removes unnecessary error
 *  messages and state machine churn.
 *
 * 125   6/11/10 2:30p erickson
 * SW3556-1124: check pDirty->bFrameRate. derive nexus frameRate and
 *  refreshRate from eFrameRateCode, not pFmtInfo.
 *
 * 124   6/1/10 9:39p bandrews
 * SW3548-2323: unlinked work on 7420 with 35230, input and display only
 *  tuned on latter
 *
 * 123   5/5/10 7:26p bandrews
 * SW3548-2323: updated to use apply changes if update mode says so
 *
 * 122   3/30/10 6:39p bandrews
 * SW3548-2323: proxy code generator doesn't like arrays as parameters to
 *  public APIs
 *
 * 121   3/30/10 4:49p bandrews
 * SW3548-2323: merge to main, need to match header
 *
 * 120   3/29/10 3:39p bandrews
 * SW3548-2323: merge to main
 *
 * SW3548-2323/6   3/8/10 3:54p bandrews
 * SW3548-2323: merge from latest
 *
 * 119   3/26/10 2:19p erickson
 * SW7405-3965: refactor mosaic to support hd/sd simul mode
 *
 * 118   3/23/10 3:25p petlee
 * SW35230-106: Merging branch SW35230-106 to mainline
 *
 * SW35230-106a/9   3/22/10 1:20p petlee
 * SW35230-106: Minor updates according to code review
 *
 * SW35230-106a/8   3/9/10 2:57p petlee
 * SW35230-106: Minor cleanups
 *
 * SW35230-106a/6   3/9/10 2:00p petlee
 * SW35230-106: Update the re-work done for VideoInput.
 *
 * SW35230-106a/5   2/26/10 5:46p petlee
 * SW35230-106: Wrap g_VideoInputState with #if NEXUS_DTV_PLATFORM > 2
 *
 * SW35230-106a/4   2/26/10 5:31p petlee
 * SW35230-106: Add check for existing sourceVdc before creating a VDC
 *  handle
 *
 * SW35230-106a/3   2/26/10 3:12p petlee
 * SW35230-106: Move destruction of sourceVdc to VideoInput_Shutdown()
 *
 * SW35230-106a/2   2/26/10 10:33a petlee
 * SW35230-106: Handle cases where sourceVdc is used when it has not been
 *  created yet. Public APIs should store state and set it later once
 *  sourceVdc gets created.
 *
 * SW35230-106a/1   2/25/10 3:01p petlee
 * SW35230-106: Update to accomodate new requirements in 35230 h/w where
 *  three analog video decoders are routed through two ports, and the two
 *  ports + hdmi input are routed through two hd_dvi blocks for routing to
 *  the BVN.
 *
 * 117   2/22/10 12:50p erickson
 * SW7405-3625: update backendMosaic impl
 *
 * 116   2/19/10 3:30p erickson
 * SW7405-3625: fix non-mosaic capable systems
 *
 * 115   2/19/10 1:23p erickson
 * SW7405-3625: add NEXUS_VideoWindowMosaicSettings for backendMosaic
 *  support
 *
 * SW3548_2323/5   12/15/09 8:54p bandrews
 * SW3548-2323: updated per bvdc_tune.h
 *
 * SW3548_2323/4   12/15/09 8:50p bandrews
 * SW3548-2323: updated from VDC team's vdc_tune impl
 *
 * SW3548_2323/3   12/15/09 8:38p bandrews
 * SW3548-2323: merge from latest
 *
 * 114   12/9/09 3:10p erickson
 * SW7405-3394: refactor mosaic mode API
 *
 * 113   11/2/09 10:43a erickson
 * SW3548-2584: allow NULL color matrix for
 *  NEXUS_VideoInput_SetColorMatrix
 *
 * 112   10/15/09 4:31p erickson
 * SWDEPRECATED-3881: don't use deprecated struct members. use NEXUS_NUM
 *  defines
 *
 * SW3548_2323/2   9/28/09 8:23p bandrews
 * 3548-2323: initial check-in of libtune port
 *
 * SW3548_2323/1   9/22/09 6:50p bandrews
 * SW3548-2323: libTune nexus passthrough
 *
 * 111   9/21/09 2:53p erickson
 * SW3548-2475: clarify meaning and use of VDC source callback params re:
 *  format change
 *
 * 110   9/9/09 1:20p erickson
 * SW7405-2977: allow build with no video decoder
 *
 * 109   8/28/09 9:36a gmohile
 * SW7403-796 : Add legacy vdc support
 *
 * 108   8/19/09 3:39p erickson
 * PR57822: set NEXUS_VideoInputStatus.videoPresent to true if digital
 *  picture not muted
 *
 * 107   8/10/09 10:09a erickson
 * PR56325: only feedback display information to sync-locked inputs
 *
 * 106   8/10/09 8:17a gmohile
 * PR 56400 : Merge legacy vdc support to main line
 *
 * 105   8/6/09 2:23p erickson
 * PR57387: add BDBG_ERR to catch BFMT_VideoFmt/NEXUS_VideoFormat
 *  mismatches in the future
 *
 * 104   8/3/09 8:57a erickson
 * PR57220: added new framerates to switch
 *
 * 103   7/31/09 11:17a gmohile
 * PR 56512 : Add 7403 support
 *
 * 102   7/23/09 10:39a erickson
 * PR56969: add hddvi input
 *
 * 101   7/10/09 3:58p erickson
 * PR56558: add NEXUS_DisplayModule_SetVideoDecoderModule to allow faster
 *  start up time
 *
 * 100   7/8/09 2:50p erickson
 * PR56634: disable DNR in the BVDC_Source when the VideoInput is
 *  disconnect from the last Nexus VideoWindow
 *
 * 99   6/15/09 5:01p erickson
 * PR56022: deprecated numWindows and numDisplays
 *
 * 98   5/20/09 3:29p erickson
 * PR54880: keep track of whether VDC heaps were created and only destroy
 *  what was created
 *
 * 97   5/19/09 10:59a erickson
 * PR54880: fix heap handling. don't assume VideoDecoder will return NULL
 *  heap for default.
 *
 * 96   4/24/09 11:58a erickson
 * PR54572: replace calls to NEXUS_VideoInput_P_Get with input-
 *  >destination. we can't recreate a NEXUS_VideoInput_P_Link as a result
 *  of an internal event.
 *
 * 95   4/20/09 5:06p jgarrett
 * PR 54346: Adding better recovery from invalid app states.
 *
 * 94   4/6/09 11:49a erickson
 * PR53137: don't tear down vbi until NEXUS_VideoInput_P_DestroyLink.
 *  NEXUS_VideoInput_Shutdown may not actually tear down input.
 *
 * 93   4/2/09 9:38a jgarrett
 * PR 53654: Adding option to suspend dynamic RTS for a source
 *
 * 92   3/30/09 11:17a erickson
 * PR48963: use NEXUS_VideoDecoder_GetHeap_priv
 *
 * 91   3/26/09 10:49a erickson
 * PR53613: allow NEXUS_VideoInput_GetColorMatrix to read from VDC is user
 *  has not set the NEXUS_ColorMatrix
 *
 * 90   3/23/09 1:43p erickson
 * PR48984: destroy checkFormatChanged event and event handler
 *
 * 89   3/20/09 1:59p jgarrett
 * PR 53465: Adding numeric refresh rate
 *
 * 88   3/10/09 4:30p erickson
 * PR53042: check for NULL on every call to NEXUS_VideoInput_P_Get
 *
 * 87   3/6/09 7:39p jgarrett
 * PR 52600: Bailing out of resume of VDB fails
 *
 * 86   3/4/09 1:03p jgarrett
 * PR 52618: Adding forcePcFormat option to HdmiInput
 *
 * 85   2/10/09 11:47a jgarrett
 * PR 48984: Incorporating comments from vdb review
 *
 * 84   1/30/09 12:02p vsilyaev
 * PR 51741: Don't free unallocated pointers
 *
 * 83   1/9/09 2:38a erickson
 * PR50502: defer source pending callback until the video has an active
 *  picture so that there's a valid format
 *
 * 82   1/6/09 11:49a jgarrett
 * PR 48984: Merging dynamic RTS code to main branch
 *
 * 81   12/23/08 6:03p vsilyaev
 * PR 50432: Added options for the closed caption decoder
 *
 * 80   12/16/08 11:50a jgarrett
 * PR 50002: Adding NEXUS_VideoInput_ForcePending
 *
 * PR48984/3   12/10/08 11:29a jgarrett
 * PR 48984: Merge to latest baseline
 *
 * 79   12/9/08 10:35a erickson
 * PR35457: clarify BDBG_ERR
 *
 * 78   12/8/08 5:04p erickson
 * PR50058: rework internal VideoInput vbi accounting, don't store refcnt
 *
 * PR48984/2   11/26/08 6:19p jgarrett
 * PR 48984: Merging to latest baseline
 *
 * 77   11/21/08 12:53p erickson
 * PR43413: added vps decode
 *
 * PR48984/1   11/20/08 6:47p jgarrett
 * PR 48984: Adding vdb hooks for auto source pending
 *
 * 76   11/17/08 12:28p erickson
 * PR49203: ensure there are no unnecessary unguarded BVDC_ApplyChanges
 *  calls in Nexus
 *
 * 75   11/6/08 5:22p erickson
 * PR47030: rename NEXUS_VideoInput_P_DestroyLink
 *
 * 74   11/4/08 4:24p erickson
 * PR47030: pass VideoDecoder heap when creating link
 *
 * 73   11/4/08 12:38p erickson
 * PR47030: add NEXUS_Display_P_DestroyHeap, refactor so that all VDC
 *  Sources are created by NEXUS_VideoInput_P_CreateLink
 *
 * 72   11/3/08 2:27p erickson
 * PR47030: add HdmiInput heap
 *
 * 71   10/31/08 12:15a erickson
 * PR47392: use value of 0xFFFF for no WSS data
 *
 * 70   10/30/08 8:56p erickson
 * PR47030: use NEXUS_HeapHandle for per-window and per-source heaps
 *
 * 69   10/29/08 12:15p erickson
 * PR48373: set defaults based on bvbi.c defaults
 *
 * 68   10/29/08 11:55a erickson
 * PR47895: merge
 *
 * PR46000/2   10/28/08 9:16p mlei
 * PR47895: fire soure pending callback when analog video goes stable.
 *
 * 67   10/24/08 1:38p erickson
 * PR47030: added fullHd/hd/sd buffer settings for digital when
 *  analog/digital memory sharing is enabled
 *
 * 66   10/24/08 11:16a erickson
 * PR48006: memset link for easier init, protect against NULL pFmtInfo
 *
 * 65   10/17/08 3:23p erickson
 * PR47030: impl shared analog/digital heap
 *
 * 64   10/17/08 12:24p erickson
 * PR47030: impl shared analog/digital heap
 *
 * 63   10/16/08 5:25p erickson
 * PR48006: call NEXUS_AnalogVideoDecoder_P_SetVbi with VDC source
 *  callback format
 *
 * 62   10/16/08 12:32p erickson
 * PR47070: fix gemStar naming convention
 *
 * 61   10/5/08 11:42p erickson
 * PR40799: ensure atomic access to info.mfd
 *
 * 60   10/2/08 12:55p erickson
 * PR40799: create/destroy BVDC_Source when AnalogVideoDecoder is
 *  connected/disconnected
 *
 * 59   9/26/08 2:44p erickson
 * PR43944: merge
 *
 * 58   9/26/08 11:58a erickson
 * PR46515: add VDEC and VBI power management
 *
 * 57   9/24/08 5:21p erickson
 * PR47070: add vbi closedCaption, teletext and gemStar DataReady
 *  callbacks
 *
 * 56   9/8/08 9:46a erickson
 * PR46068: added BDBG_OBJECT_ASSERT for isr-time link
 *
 * 55   9/2/08 10:27p katrep
 * PR46457: Configure nexus features at the run time based on board strap
 *  options
 *
 * 54   9/2/08 5:23p erickson
 * PR46473: NEXUS_VideoInput_Shutdown will call
 *  NEXUS_VideoWindow_RemoveInput if needed
 *
 * 53   8/22/08 11:08p erickson
 * PR45711: must call BVDC_ApplyChanges after clearing VDC source
 *  callback. don't recreate the video input link during
 *  NEXUS_VideoInput_Shutdown if it doesn't already exist.
 *
 * 52   8/22/08 7:05p erickson
 * PR45794: change internal storage
 *
 * 51   8/21/08 12:24p erickson
 * PR45711: stop BVDC_Source_InstallCallback before destroying the video
 *  input
 *
 * 50   8/20/08 12:10p erickson
 * PR45616: prefer data from VDC Source callback over
 *  BVDC_Source_GetInputStatus
 *
 * 49   8/19/08 12:21p erickson
 * PR45794: strengthen ERR message
 *
 * 48   8/8/08 5:34p vishk
 * PR 45390: Nexus VBI reports "No CloseCaption buffer" when VBI not
 *  configured.
 *
 * 47   7/23/08 10:11a erickson
 * PR45073: protect access to NEXUS_P_VertFreq_FromMagnum
 *
 * 46   7/17/08 9:41a erickson
 * PR44764: added BDBG_ERR
 *
 * 45   7/11/08 4:09p erickson
 * PR44764: default MFD info to zero. someone may query status before the
 *  first picture data ready isr
 *
 * 44   7/8/08 4:10p erickson
 * PR44524: fix race condition where multiple sourcePending callbacks
 *  occur. Nexus should not block callbacks if status.sourcePending is
 *  set. app may have already read the status in response to the 1st
 *  callback and so it needs the 2nd callback.
 *
 * 43   7/3/08 5:07p erickson
 * PR43699: Remove unnecessary VDC Get functions from Nexus to avoid race
 *  conditions
 *
 * 42   6/30/08 10:30a erickson
 * PR43785: use pDirty->bCrcValue for CRC check
 *
 * 41   6/26/08 9:07a erickson
 * PR44050: merge
 *
 * PR44050/1   6/25/08 5:42p dyzhang
 * PR44050: h/w and s/w jpeg doesn't work after enabling source pending.
 *  need to apply change after set resume.
 *
 * 40   6/20/08 2:10p erickson
 * PR43859: merge
 *
 * PR43859/5   6/20/08 1:13p dyzhang
 * PR43859: need to return the framerate information by format info. The
 *  framerate code from callback data is not reliable which causes dynamic
 *  rts calculation fail.
 *
 * PR43859/4   6/20/08 1:07p dyzhang
 * PR43859: merge from mainline
 *
 * 39   6/20/08 9:44a erickson
 * PR43859: merge
 *
 * PR43859/3   6/19/08 8:18p dyzhang
 * PR43859: link status sourc pending need to be set to false are resume
 *  now
 *
 * PR43859/2   6/19/08 8:15p dyzhang
 * PR43859: merge from main line
 *
 * 38   6/19/08 4:29p erickson
 * PR43859: merge
 *
 * PR43859/1   6/19/08 3:27p dyzhang
 * PR43859: Need apply change for set resume. And fix a few bugs
 *
 * 37   6/11/08 12:46p vsilyaev
 * PR 43491: Added support for CCIR-656 input
 *
 * 36   5/30/08 5:08p rjlewis
 * PR40352: warnings.
 *
 * 35   5/30/08 3:29p erickson
 * PR41202: fix warning
 *
 * 34   5/20/08 4:34p erickson
 * PR41202: fixed VDC source/window heap support for WXGA support
 *
 * 33   5/12/08 9:28a erickson
 * PR42365: fix crc logic
 *
 * 32   5/9/08 12:44p erickson
 * PR39453: add sourcePending callback, set status.sourcePending based on
 *  VDC dirty bits.
 *
 * 31   5/8/08 2:48p erickson
 * PR42438: derive NEXUS_VideoInputStatus.format for digital decode using
 *  common function
 *
 * 30   5/7/08 2:47p erickson
 * PR42329: fix NEXUS_VideoInput_P_DisconnectVideoDecoder
 *
 * 29   5/5/08 3:02p erickson
 * PR41202: fix logic for when to create a heap
 *
 * 28   5/1/08 12:57p erickson
 * PR41202: call BVDC_Heap_Create for sharedAnalogDigitalHeap mode
 *
 * 27   4/30/08 3:08p erickson
 * PR39453: add 3548/3556 features
 *
 * 26   4/28/08 4:10p erickson
 * PR41533: unify EIA-608 and EIA-708 closed caption data capture in
 *  NEXUS_VideoInput_ReadClosedCaption
 *
 * 25   4/23/08 4:29p erickson
 * PR42071: remove dangling ;
 *
 * 24   4/7/08 4:10p erickson
 * PR32888: added NEXUS_CRC_CAPTURE
 *
 * 23   4/4/08 10:08a erickson
 * PR40513: add repeat
 *
 * 22   3/31/08 1:41p erickson
 * PR41077: added NEXUS_ASSERT_MODULE to _priv function
 *
 * 21   3/31/08 12:32p erickson
 * PR41073: check result of malloc and fail graciously
 *
 * 20   3/27/08 6:51p vsilyaev
 * PR 40818: Fixed 7401 build
 *
 * 19   3/27/08 6:20p vsilyaev
 * PR 40818: Added management of VDC heaps
 *
 * 18   3/27/08 4:22p vsilyaev
 * PR 40818: Improved internal API to allow source private heaps
 *
 * 17   3/26/08 3:38p vsilyaev
 * PR 40862: Added code to propagate display framerate to the decoder
 *
 * 16   3/26/08 10:54a erickson
 * PR40742: added more HdmiInput which is passed in from VDC source
 *  changed callback
 *
 * 15   3/21/08 12:44p vsilyaev
 * PR 39986: Added support for VideoImageInput
 *
 * 14   3/21/08 11:40a erickson
 * PR40716: merge
 *
 * PR40716/1   3/21/08 3:43p dyzhang
 * PR40716: timebase settings for analog inputs are not correct. need to
 *  use the framerate from callback data. the framerate from the format
 *  information is the vertical framerate defined bfmt.c. This is the
 *  value defined in standard and it's the value signal suppose to be but
 *  sometime the real signal doesn't match the standard. The framerate
 *  code from callback data is the real framerate of coming in signal
 *  which is detected by decoder.
 *
 * 12   3/12/08 5:25p erickson
 * PR40412: get analog VideoInputStatus from the VDC source callback.
 *  BVDC_Source_GetInputStatus does not return reliable data.
 *
 * 11   3/12/08 4:32p erickson
 * PR40412: fixed use of BVDC_Source_InstallCallback
 *
 * 10   3/6/08 3:07p erickson
 * PR40103: convert to NEXUS_TaskCallback
 *
 * 9   3/3/08 9:21a erickson
 * PR40156: fix legacy VDC
 *
 * 8   2/29/08 5:39p erickson
 * PR40156: implement ColorMatrix function
 *
 * 7   2/28/08 10:42p vsilyaev
 * PR 40103: Added interfaceHandle and settings for the
 *  NEXUS_IsrCallbackCreate
 *
 * 6   2/19/08 5:24p erickson
 * PR39748: added null check
 *
 * 5   2/19/08 4:12p erickson
 * PR39746: BAVC_MFD_Picture should never be NULL
 *
 * 4   2/5/08 10:13a erickson
 * PR38679: change private FindWindow
 *
 * 3   2/1/08 3:20p erickson
 * PR38679: add vbi support
 *
 * 2   1/31/08 9:55a erickson
 * PR36808: added SyncChannel interface
 *
 * 1   1/18/08 2:20p jgarrett
 * PR 38808: Merging to main branch
 *
 * Nexus_Devel/29   1/11/08 1:51p erickson
 * PR38679: add vbi read/write api's
 *
 * Nexus_Devel/28   1/11/08 12:33p erickson
 * PR36159: set/unset input->destination
 *
 * Nexus_Devel/27   1/9/08 3:55p erickson
 * PR36159: NEXUS_VideoInput_Shutdown should call
 * NEXUS_VideoInput_P_Destroy
 *
 * Nexus_Devel/26   1/9/08 2:51p erickson
 * PR34662: convert 7401 code for digital NEXUS_VideoInput_GetStatus to
 * all platforms
 *
 * Nexus_Devel/25   1/9/08 12:14p vsilyaev
 * PR 34662: Implemented VideoInput_GetStatus for legacy VDC
 *
 * Nexus_Devel/24   1/9/08 10:45a erickson
 * PR34662: implemented sourceChanged callback and all
 * NEXUS_VideoInputStatus members
 *
 * Nexus_Devel/23   1/3/08 9:20p erickson
 * PR38213: rename to AnalogVideoDecoder
 *
 * Nexus_Devel/22   12/18/07 1:49p jgarrett
 * PR 38213: Removing warnings
 *
 * Nexus_Devel/21   12/18/07 1:34p erickson
 * PR38213: analog video input rework
 *
 * Nexus_Devel/20   12/5/07 3:09p vsilyaev
 * PR 37934: Implemented BLST_S_DICT
 *
 * Nexus_Devel/19   11/28/07 1:43p vsilyaev
 * PR 36159: Added global UpdateMode, removed per-function ApplyChanges
 *
 * Nexus_Devel/18   11/26/07 12:51p vsilyaev
 * PR36159: PR36159: Bring up Nexus on 93563 platform
 *
 * Nexus_Devel/PR36159/1   11/26/07 10:09a gezhang
 * PR36159: Bring up Nexus on 93563 platform
 *
 * Nexus_Devel/17   11/9/07 11:51a erickson
 * PR36814: hdmi updaets
 *
 * Nexus_Devel/16   11/8/07 2:43p erickson
 * PR36814: added HdmiInput
 *
 * Nexus_Devel/15   11/6/07 1:13p erickson
 * PR36570: merge 3563
 *
 * Nexus_Devel/14   11/5/07 10:06a vsilyaev
 * PR 36696: Fixed build problem with 7400/7405
 *
 * Nexus_Devel/13   11/2/07 4:42p vsilyaev
 * PR 36696: Used connector model for VideoInput's and VideoOutput's
 *
 * Nexus_Devel/12   10/8/07 12:11p vsilyaev
 * PR 34662: PR34926: reworked VBI interface
 *
 * Nexus_Devel/11   10/4/07 4:15p vsilyaev
 * PR 34662: Removed hardcoded use of videoSource
 *
 * Nexus_Devel/10   10/4/07 3:34p vsilyaev
 * PR 34662: Removed VideoInput descriptor
 *
 * Nexus_Devel/9   10/4/07 12:21p vsilyaev
 * PR 34662: Coded VBI support
 *
 * Nexus_Devel/8   9/28/07 4:24p jgarrett
 * PR 35002: Successful 7400 build
 *
 * Nexus_Devel/7   9/28/07 11:32a erickson
 * PR34926: rename videodecoder to video_decoder
 *
 * Nexus_Devel/6   9/25/07 3:13p vsilyaev
 * PR 34662: Fixed video output
 *
 * Nexus_Devel/5   9/25/07 12:33p vsilyaev
 * PR 34662: Fixed warnings
 *
 * Nexus_Devel/4   9/24/07 2:42p vsilyaev
 * PR 34662: Added code to disconnect input
 *
 * Nexus_Devel/3   9/24/07 12:20p vsilyaev
 * PR 34662: Added connection of window and source
 *
 * Nexus_Devel/2   9/24/07 10:35a vsilyaev
 * PR 34662: Added function to create VDC window
 *
 * Nexus_Devel/1   9/21/07 4:09p vsilyaev
 * PR 34662: Implementation of Video Input API
 *
 **************************************************************************/
#include "nexus_base.h"
#include "nexus_display_module.h"
#if NEXUS_NUM_VIDEO_DECODERS
#include "priv/nexus_video_decoder_priv.h"
#endif

BDBG_MODULE(nexus_video_input);

BDBG_OBJECT_ID(NEXUS_VideoInput_P_Link);

#define pVideo (&g_NEXUS_DisplayModule_State)

static void NEXUS_VideoInput_P_CheckFormatChanged(void *pParam);
static NEXUS_VideoInput_P_Link *NEXUS_VideoInput_P_CreateLink_Init( NEXUS_VideoInput source, const NEXUS_VideoInput_P_LinkData *data, const NEXUS_VideoInput_P_Iface *iface );
static void NEXUS_VideoInput_P_DestroyLink_Uninit( NEXUS_VideoInput_P_Link *link );
static NEXUS_Error NEXUS_VideoInput_P_Create_VdcSource( NEXUS_VideoInput source, NEXUS_VideoInput_P_Link *link, const NEXUS_VideoInput_P_LinkData *data );
static void NEXUS_VideoInput_P_Destroy_VdcSource( NEXUS_VideoInput_P_Link *link );

/* this can be called from a SourceCallback or connecting an input to a window. */
void NEXUS_VideoInput_P_TriggerSourceChanged_isr(NEXUS_VideoInput_P_Link *link, bool sourcePending)
{
    if ( sourcePending )
    {
        BDBG_MSG(("Source Pending ISR"));
        link->status.sourcePending = true;
        if ( link->resumeMode == NEXUS_VideoInputResumeMode_eManual )
        {
            BDBG_MSG(("Manual source pending.  VDB will be handled on resume."));
            NEXUS_IsrCallback_Fire_isr(link->sourcePendingCallback);
        }
        else if ( link->sourcePendingEvent )
        {
            BDBG_MSG(("Auto VDB source pending"));
            BKNI_SetEvent(link->sourcePendingEvent);
        }
    }
    NEXUS_IsrCallback_Fire_isr(link->sourceChangedCallback);
}

static void NEXUS_VideoInput_P_SourceCallback_isr(void *pParam1, int pParam2, void *vdcData)
{
    NEXUS_VideoInput_P_Link *link = pParam1;
    const BVDC_Source_CallbackData *pSrcData = (const BVDC_Source_CallbackData*)vdcData;
    const BVDC_Source_DirtyBits *pDirty = &pSrcData->stDirty;
    NEXUS_VideoInputStatus *pStatus;
    bool srcPending;
    bool formatChanged = false;

    BSTD_UNUSED(pParam2);
    BDBG_OBJECT_ASSERT(link, NEXUS_VideoInput_P_Link);
    pStatus = &link->status;

    /* save all the data for later source calls */
    link->vdcSourceCallbackData = *pSrcData;

    switch(link->id) {
    case BAVC_SourceId_eMpeg0:
    case BAVC_SourceId_eMpeg1:
    case BAVC_SourceId_eMpeg2:
    case BAVC_SourceId_eMpeg3:
#if NEXUS_CRC_CAPTURE
        if (pDirty->bCrcValue) {
            NEXUS_VideoInputCrcData *pData = &link->crc.data[link->crc.wptr];
            pData->idrPictureId = pSrcData->ulIdrPicId;
            pData->pictureOrderCount = pSrcData->lPicOrderCnt;
            pData->lumaCrc = pSrcData->ulLumaCrc;
            pData->chromaCrc = pSrcData->ulChromaCrc;
            pData->lumaCrcR = pSrcData->ulLumaCrcR;
            pData->chromaCrcR = pSrcData->ulChromaCrcR;
            pData->isField = pSrcData->eSourcePolarity != BAVC_Polarity_eFrame;
            if (++link->crc.wptr == NEXUS_VIDEO_INPUT_CRC_QUEUE_SIZE) {
                link->crc.wptr = 0;
            }
            if (link->crc.wptr == link->crc.rptr) {
                BDBG_WRN(("crc capture overflow"));
            }
        }
#endif
        break;

    default:
        /* For analog/hd-dvi sources, we need to capture the data passed into the callback. Calling BVDC_Source_GetInputStatus does
        not return reliable information.
        A change in bActive will always result in a sourceChanged callback. */
        pStatus->videoPresent = pSrcData->bActive;
        /* Based on current VDC impl, pSrcData->pFmtInfo will never be NULL for analog/hddvi. We are checking anyway for code safety.
        The main flag for knowing when video comes and goes is a change in bActive. */
        if (pSrcData->pFmtInfo) {
            if (pDirty->bFmtInfo || pDirty->bFrameRate) { /* save a couple cpu clock cycles */
                NEXUS_VideoFormat videoFormat;
                pStatus->width = pSrcData->pFmtInfo->ulWidth;
                pStatus->height = pSrcData->pFmtInfo->ulHeight;
                videoFormat = NEXUS_P_VideoFormat_FromMagnum(pSrcData->pFmtInfo->eVideoFmt);
                if (videoFormat == NEXUS_VideoFormat_eUnknown) {
                    BDBG_ERR(("Unable to convert analog magnum BFMT_VideoFmt %d to NEXUS_VideoFormat", pSrcData->pFmtInfo->eVideoFmt));
                    /* For nexus_vdb and analog inputs, the videoFormat enum is required. The Nexus and FMT enum lists must be
                    kept in sync.
                    Another option is to learn if the stream is progressive/interlaced, then use NEXUS_P_VideoFormat_FromInfo. */
                }
                if (pStatus->format != videoFormat) {
                    pStatus->format = videoFormat;
                    formatChanged = true;
                }
                pStatus->aspectRatio = NEXUS_P_AspectRatio_FromMagnum(pSrcData->pFmtInfo->eAspectRatio);
                pStatus->frameRate = NEXUS_P_FrameRate_FromMagnum(pSrcData->eFrameRateCode);
                pStatus->refreshRate = NEXUS_P_RefreshRate_FromFrameRate(pStatus->frameRate);
            }
        }
        else {
            pStatus->format = NEXUS_VideoFormat_eUnknown;
        }
    }

    /* don't fire source pending callback when the field is not active (e.g. muted).
    instead, queue it up until we get an active picture. */
    srcPending = false;
    if (pDirty->bSrcPending) {
        if (pSrcData->bActive) {
            srcPending = true;
        }
        else {
            link->deferSourcePending = true;
        }
    }
    if (pSrcData->bActive && link->deferSourcePending) {
        link->deferSourcePending = false;
        srcPending = true;
    }

    NEXUS_VideoInput_P_TriggerSourceChanged_isr(link, srcPending);

    switch (link->input->type) {
#if NEXUS_NUM_ANALOG_VIDEO_DECODERS
    case NEXUS_VideoInputType_eAnalogVideoDecoder:
        /* Allow AnalogVideoDecoder to do ISR processing */
        NEXUS_AnalogVideoDecoder_P_SourceChanged_isr(link->input);

        /* Must reconfigure VBI if the format changes */
        if (formatChanged) {
            BKNI_SetEvent(link->sourceChangedEvent);
        }
        break;
#endif
#if NEXUS_NUM_HDMI_INPUTS
    case NEXUS_VideoInputType_eHdmi:
        /* Allow HdmiInput to do ISR processing */
        NEXUS_VideoInput_P_HdmiSourceCallback_isr(link, pSrcData);

        /* HdmiInput needs a task callback to call BVDC_Source_GetInputStatus and get the remaining status information.
        VideoInput must do the work. */
        BKNI_SetEvent(link->sourceChangedEvent);
        break;
#endif
    default:
        break;
    }
}

static void NEXUS_VideoInput_P_SourceChanged(void *context)
{
    NEXUS_VideoInput input = context;
    NEXUS_VideoInput_P_Link *link = input->destination; /* do not call NEXUS_VideoInput_P_Get here. if the input has been shutdown, we can't recreate it here */

    if (!link) return;
    BDBG_OBJECT_ASSERT(link, NEXUS_VideoInput_P_Link);

    switch (link->input->type) {
#if NEXUS_NUM_HDMI_INPUTS
    case NEXUS_VideoInputType_eHdmi:
        NEXUS_VideoInput_P_SetHdmiInputStatus(link);
        break;
#endif
#if NEXUS_NUM_ANALOG_VIDEO_DECODERS
    case NEXUS_VideoInputType_eAnalogVideoDecoder:
        /* VBI decoders may have to be reconfigured on source format change
        pFmtInfo should always be non-NULL here, but the "if" is to be extra safety */
        if (link->vdcSourceCallbackData.pFmtInfo) {
            (void)NEXUS_AnalogVideoDecoder_P_SetVbi(input->source, &link->vbiSettings, link->vdcSourceCallbackData.pFmtInfo->eVideoFmt);
        }
        break;
#endif
    default:
        break;
    }
}

static void NEXUS_VideoInput_P_SourcePending(void *context)
{
    NEXUS_VideoInput input = context;
    NEXUS_VideoInput_P_Link *link = input->destination; /* do not call NEXUS_VideoInput_P_Get here. if the input has been shutdown, we can't recreate it here */

    if (!link) return;
    BDBG_OBJECT_ASSERT(link, NEXUS_VideoInput_P_Link);

    if ( link->resumeMode == NEXUS_VideoInputResumeMode_eAuto )
    {
        NEXUS_Vdb_P_HandleSourcePending(input);
    }
}

void
NEXUS_VideoInput_P_LinkData_Init(NEXUS_VideoInput_P_LinkData *data, BAVC_SourceId sourceId)
{
    BDBG_ASSERT(data);
    BKNI_Memset(data, 0, sizeof(*data));
    data->sourceId = sourceId;
    return;
}

NEXUS_VideoInput_P_Link *
NEXUS_VideoInput_P_CreateLink(NEXUS_VideoInput source, const NEXUS_VideoInput_P_LinkData *data, const NEXUS_VideoInput_P_Iface *iface)
{
    NEXUS_VideoInput_P_Link *link = NULL;
    NEXUS_Error rc;

    link = NEXUS_VideoInput_P_CreateLink_Init(source, data, iface);
    if (!link) return NULL;

    rc = NEXUS_VideoInput_P_Create_VdcSource(source, link, data);
    if (NEXUS_SUCCESS != rc) {
        NEXUS_VideoInput_P_DestroyLink_Uninit(link);
        return NULL;
    }

    return link;
}

static void NEXUS_VideoInput_P_GetDefaultSettings(NEXUS_VideoInputSettings *pSettings)
{
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    pSettings->video3DSettings.overrideOrientation = false;
    pSettings->video3DSettings.orientation = NEXUS_VideoOrientation_e2D;
    return;
}

static NEXUS_VideoInput_P_Link *
NEXUS_VideoInput_P_CreateLink_Init(NEXUS_VideoInput source, const NEXUS_VideoInput_P_LinkData *data, const NEXUS_VideoInput_P_Iface *iface)
{
    NEXUS_VideoInput_P_Link *link = NULL;
    NEXUS_Error rc = NEXUS_SUCCESS;

    BDBG_ASSERT(source && source->source);
    BDBG_ASSERT(data);
    BDBG_ASSERT(iface);

    BDBG_MSG((">NEXUS_VideoInput_P_CreateLink_Init id %d, heap %p", data->sourceId, data->heap));

    link = BKNI_Malloc(sizeof(*link));
    if(!link) {
        rc = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        goto err_alloc;
    }
    BKNI_Memset(link, 0, sizeof(*link));
    BDBG_OBJECT_SET(link, NEXUS_VideoInput_P_Link);
    NEXUS_VideoInput_P_GetDefaultSettings(&link->cfg);
    link->id = data->sourceId;
    link->heap = data->heap;

    link->input = source;
    link->iface = *iface;
    link->input_info.type = source->type;
    link->input_info.source = source->source;
    link->sourcePendingCallback = NEXUS_IsrCallback_Create(source, NULL);
    link->sourceChangedCallback = NEXUS_IsrCallback_Create(source, NULL);
    BKNI_CreateEvent(&link->sourceChangedEvent);
    link->sourceChangedEventHandler = NEXUS_RegisterEvent(link->sourceChangedEvent, NEXUS_VideoInput_P_SourceChanged, source);
    link->resumeMode = NEXUS_VideoInputResumeMode_eAuto;

    /* Only create this if we're doing dynamic RTS */
    if ( g_NEXUS_DisplayModule_State.moduleSettings.handleDynamicRts )
    {
        BKNI_CreateEvent(&link->sourcePendingEvent);
        link->sourcePendingEventHandler = NEXUS_RegisterEvent(link->sourcePendingEvent, NEXUS_VideoInput_P_SourcePending, source);
        BKNI_CreateEvent(&link->checkFormatChangedEvent);
        link->checkFormatChangedEventHandler = NEXUS_RegisterEvent(link->checkFormatChangedEvent, NEXUS_VideoInput_P_CheckFormatChanged, source);
    }

    /* Only default a CC buffer. All others require app to set buffer size. */
    link->vbiSettings.closedCaptionBufferSize = 30;
    /* VBI decode defaults copied from bvbi.c */
    link->vbiSettings.gemStar.baseLineTop = 10;
    link->vbiSettings.gemStar.lineMaskTop = 0x1f;
    link->vbiSettings.gemStar.baseLineBottom = 273;
    link->vbiSettings.gemStar.lineMaskBottom = 0x1f;
    link->vbiSettings.wss.vline576i = 23;
    link->vbiSettings.cc.ccLineTop      = 21;
    link->vbiSettings.cc.ccLineBottom   = 284;
    /* no default buffer for teletext */
    link->vbi.wss.isrCallback = NEXUS_IsrCallback_Create(source, NULL);
    link->vbi.wss.data = 0xffff; /* no data */
    link->vbi.cgms.isrCallback = NEXUS_IsrCallback_Create(source, NULL);
    link->vbi.vps.isrCallback = NEXUS_IsrCallback_Create(source, NULL);
    link->vbi.cc.name = "closed caption";
    link->vbi.cc.isrCallback = NEXUS_IsrCallback_Create(source, NULL);
    link->vbi.cc.elementSize = sizeof(NEXUS_ClosedCaptionData);
    link->vbi.tt.name = "teletext";
    link->vbi.tt.isrCallback = NEXUS_IsrCallback_Create(source, NULL);
    link->vbi.tt.elementSize = sizeof(NEXUS_TeletextLine);
    link->vbi.gs.name = "gemstar";
    link->vbi.gs.isrCallback = NEXUS_IsrCallback_Create(source, NULL);
    link->vbi.gs.elementSize = sizeof(NEXUS_GemStarData);

    source->ref_cnt++;
    BLST_S_DICT_ADD(&pVideo->inputs, link, NEXUS_VideoInput_P_Link, input, link, err_duplicate);
    BDBG_MSG(("<NEXUS_VideoInput_P_CreateLink_Init %p", link));

    return link;

err_duplicate:
    NEXUS_VideoInput_P_DestroyLink_Uninit(link);
err_alloc:
    return NULL;
}

static NEXUS_Error
NEXUS_VideoInput_P_Create_VdcSource(NEXUS_VideoInput source, NEXUS_VideoInput_P_Link *link, const NEXUS_VideoInput_P_LinkData *data)
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    BVDC_Source_Settings sourceCfg;

    BDBG_ASSERT(source && source->source);
    BDBG_ASSERT(link);
    BDBG_ASSERT(data);

    BDBG_MSG((">NEXUS_VideoInput_P_Create_VdcSource input %p, link %p, heap %p", source, link, data->heap));
    if (data->sourceVdc) {
        link->sourceVdc = data->sourceVdc;
        link->copiedSourceVdc = true;
    }
    else {
        rc = BVDC_Source_GetDefaultSettings(data->sourceId, &sourceCfg);
        if(rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc); goto err_source;}

        if (data->heap && data->heap != pVideo->heap) {
            sourceCfg.hHeap = NEXUS_Display_P_CreateHeap(data->heap);
            if (!sourceCfg.hHeap) { rc = BERR_TRACE(rc); goto err_source;}

            link->vdcHeap = sourceCfg.hHeap;
        }

        if ( source->type == NEXUS_VideoInputType_eImage ) {
            sourceCfg.bGfxSrc = true;
        }

        rc = BVDC_Source_Create(pVideo->vdc, &link->sourceVdc, data->sourceId, &sourceCfg);
        if(rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc); goto err_source;}

        if(pVideo->updateMode != NEXUS_DisplayUpdateMode_eAuto) {rc=BERR_TRACE(NEXUS_NOT_SUPPORTED);}
        rc = BVDC_ApplyChanges(pVideo->vdc);
        if (rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc); goto err_apply_changes;}

        rc = BVDC_Source_InstallCallback(link->sourceVdc, NEXUS_VideoInput_P_SourceCallback_isr, link, 0);
        if(rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc); goto err_apply_changes;}
        
    }

    rc  = NEXUS_VideoInput_SetResumeMode(source, link->resumeMode);
    if(rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc); goto err_apply_changes;}
    
    BDBG_MSG(("<NEXUS_VideoInput_P_Create_VdcSource %s %p:%d", link->copiedSourceVdc?"copied":"created", link->sourceVdc, data->sourceId));

    return NEXUS_SUCCESS;

err_apply_changes:
    if (!link->copiedSourceVdc) {
        rc = BVDC_AbortChanges(pVideo->vdc);
        if (rc!=BERR_SUCCESS) {rc = BERR_TRACE(rc);}
        rc = BVDC_Source_Destroy(link->sourceVdc);
        if (rc!=BERR_SUCCESS) {rc = BERR_TRACE(rc);}
    }
err_source:
    return rc;
}

void
NEXUS_VideoInput_P_DestroyLink(NEXUS_VideoInput_P_Link *link)
{
    NEXUS_VideoInput_P_Destroy_VdcSource(link);

    BKNI_Memset(&link->vbiSettings, 0, sizeof(link->vbiSettings));
    NEXUS_VideoInput_P_SetVbiState(link->input);

    NEXUS_VideoInput_P_DestroyLink_Uninit(link);

    return;
}

static void
NEXUS_VideoInput_P_DestroyLink_Uninit(NEXUS_VideoInput_P_Link *link)
{
    BDBG_MSG((">NEXUS_VideoInput_P_DestroyLink_Uninit %p", link));
    BDBG_OBJECT_ASSERT(link, NEXUS_VideoInput_P_Link);

    NEXUS_IsrCallback_Destroy(link->vbi.gs.isrCallback);
    NEXUS_IsrCallback_Destroy(link->vbi.tt.isrCallback);
    NEXUS_IsrCallback_Destroy(link->vbi.cc.isrCallback);
    NEXUS_IsrCallback_Destroy(link->vbi.wss.isrCallback);
    NEXUS_IsrCallback_Destroy(link->vbi.cgms.isrCallback);
    NEXUS_IsrCallback_Destroy(link->vbi.vps.isrCallback);
    if(link->vbi.cc.data) {
        BKNI_Free(link->vbi.cc.data);
    }
    if(link->vbi.tt.data) {
        BKNI_Free(link->vbi.tt.data);
    }

    NEXUS_IsrCallback_Destroy(link->sourcePendingCallback);
    NEXUS_IsrCallback_Destroy(link->sourceChangedCallback);
    NEXUS_UnregisterEvent(link->sourceChangedEventHandler);
    BKNI_DestroyEvent(link->sourceChangedEvent);
    if ( link->checkFormatChangedEventHandler )
    {
        NEXUS_UnregisterEvent(link->checkFormatChangedEventHandler);
        BKNI_DestroyEvent(link->checkFormatChangedEvent);
    }
    if ( link->sourcePendingEventHandler )
    {
        NEXUS_UnregisterEvent(link->sourcePendingEventHandler);
        BKNI_DestroyEvent(link->sourcePendingEvent);
    }

    link->input->ref_cnt--;
    BLST_S_DICT_REMOVE(&pVideo->inputs, link, link->input, NEXUS_VideoInput_P_Link, input, link);

    BDBG_OBJECT_DESTROY(link, NEXUS_VideoInput_P_Link);
    BKNI_Free(link);

    BDBG_MSG(("<NEXUS_VideoInput_P_DestroyLink_Uninit"));

    return;
}

static void
NEXUS_VideoInput_P_Destroy_VdcSource(NEXUS_VideoInput_P_Link *link)
{
    BERR_Code rc;

    BDBG_MSG((">NEXUS_VideoInput_P_Destroy_VdcSource input %p, link %p", link->input, link));
    BDBG_OBJECT_ASSERT(link, NEXUS_VideoInput_P_Link);

    if (!link->copiedSourceVdc && link->sourceVdc)
    {
        (void)BVDC_Source_InstallCallback(link->sourceVdc, NULL, NULL, 0);

        rc = BVDC_Source_Destroy(link->sourceVdc);
        if (rc!=BERR_SUCCESS) {rc = BERR_TRACE(rc);}

        if(pVideo->updateMode != NEXUS_DisplayUpdateMode_eAuto) {rc=BERR_TRACE(NEXUS_NOT_SUPPORTED);}

        rc = BVDC_ApplyChanges(pVideo->vdc);
        if (rc!=BERR_SUCCESS) {rc = BERR_TRACE(rc);}

        link->sourceVdc = NULL;

        if (link->vdcHeap) {
            NEXUS_Display_P_DestroyHeap(link->vdcHeap);
        }
   }

    BDBG_MSG(("<NEXUS_VideoInput_P_Destroy_VdcSource"));

    return;
}

void
NEXUS_VideoInput_GetSettings(NEXUS_VideoInput input, NEXUS_VideoInputSettings *pSettings)
{
    NEXUS_VideoInput_P_Link *link;

    BDBG_ASSERT(pSettings);
    BDBG_OBJECT_ASSERT(input, NEXUS_VideoInput);
    link = NEXUS_VideoInput_P_Get(input);
    if(!link) {
        NEXUS_VideoInput_P_GetDefaultSettings(pSettings);
        goto done;
    }
    BDBG_OBJECT_ASSERT(link, NEXUS_VideoInput_P_Link);
    *pSettings  = link->cfg;
done:
    return;
}

/*
Summary:
Returns bits 'e'..'b' from word 'w',

Example:
    B_GET_BITS(0xDE,7,4)==0x0D
    B_GET_BITS(0xDE,3,0)=0x0E
*/
#define B_GET_BITS(w,e,b)  (((w)>>(b))&(((unsigned)(-1))>>((sizeof(unsigned))*8-(e+1-b))))


NEXUS_Error
NEXUS_VideoInput_SetSettings( NEXUS_VideoInput input, const NEXUS_VideoInputSettings *pSettings)
{
    BERR_Code rc = NEXUS_SUCCESS;
    NEXUS_VideoInput_P_Link *link;

    BDBG_ASSERT(pSettings);
    BDBG_OBJECT_ASSERT(input, NEXUS_VideoInput);
    link = NEXUS_VideoInput_P_Get(input);
    if(!link) { rc=BERR_TRACE(BERR_NOT_SUPPORTED); goto err_link;}
    if (!link->sourceVdc) {link->isDeferCfg = true; link->cfg = *pSettings; return NEXUS_SUCCESS;}

    if(pSettings->muteColor != link->cfg.muteColor) {
        rc = BVDC_Source_SetVideoMuteColor(link->sourceVdc, B_GET_BITS(pSettings->muteColor, 23, 16),
                                           B_GET_BITS(pSettings->muteColor, 15, 8), B_GET_BITS(pSettings->muteColor, 7, 0));
        if (rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc);goto err_settings;}
    }
    if(pSettings->mute != link->cfg.mute || pSettings->repeat != link->cfg.repeat) {
        rc = BVDC_Source_SetMuteMode(link->sourceVdc,
            pSettings->mute ? BVDC_MuteMode_eConst :
            pSettings->repeat ? BVDC_MuteMode_eRepeat :
            BVDC_MuteMode_eDisable);
        if (rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc);goto err_settings;}
    }
    if (pSettings->video3DSettings.overrideOrientation != link->cfg.video3DSettings.overrideOrientation || 
        pSettings->video3DSettings.orientation != link->cfg.video3DSettings.orientation) 
    {
        rc = BVDC_Source_SetOrientation(link->sourceVdc, pSettings->video3DSettings.overrideOrientation, NEXUS_P_VideoOrientation_ToMagnum(pSettings->video3DSettings.orientation));
        if (rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc);goto err_settings;}
    }

    rc = NEXUS_Display_P_ApplyChanges();
    if (rc) return BERR_TRACE(rc);

    NEXUS_IsrCallback_Set(link->sourceChangedCallback, &pSettings->sourceChanged);
    NEXUS_IsrCallback_Set(link->sourcePendingCallback, &pSettings->sourcePending);

    link->cfg = *pSettings;
    return rc;

err_settings:
    {
        BERR_Code rc = BVDC_AbortChanges(pVideo->vdc);
        if (rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc);}
    }
err_link:
    return rc;
}

NEXUS_Error
NEXUS_VideoInput_GetStatus(NEXUS_VideoInput input,NEXUS_VideoInputStatus *pStatus)
{
    NEXUS_VideoInput_P_Link *link;

    BDBG_ASSERT(pStatus);
    BDBG_OBJECT_ASSERT(input, NEXUS_VideoInput);
    link = NEXUS_VideoInput_P_Get(input);

    BKNI_Memset(pStatus, 0, sizeof(*pStatus));

    if(!link) { goto done;}

    switch(link->id) {
    case BAVC_SourceId_eMpeg0:
    case BAVC_SourceId_eMpeg1:
        if (link->info_valid) {
            BAVC_FrameRateCode frameRate;
            BFMT_AspectRatio aspectRatio;
            bool bStreamProgressive;

            /* the assignment of the BAVC_MFD_Picture structure is not atomic. therefore, protect. */
            BKNI_EnterCriticalSection();
            frameRate = link->info.mfd.eFrameRateCode;
            aspectRatio = link->info.mfd.eAspectRatio;
            bStreamProgressive = link->info.mfd.bStreamProgressive;
            pStatus->width = link->info.mfd.ulSourceHorizontalSize;
            pStatus->height = link->info.mfd.ulSourceVerticalSize;
            pStatus->videoPresent = !link->info.mfd.bMute;
            BKNI_LeaveCriticalSection();

            pStatus->frameRate = NEXUS_P_FrameRate_FromMagnum(frameRate);
            pStatus->format = NEXUS_P_VideoFormat_FromInfo(pStatus->height, pStatus->frameRate, !bStreamProgressive);
            pStatus->aspectRatio = NEXUS_P_AspectRatio_FromMagnum(aspectRatio);
            pStatus->refreshRate = NEXUS_P_RefreshRate_FromFrameRate(pStatus->frameRate);
        }
        break;

    default:
        /* this status was already captured in NEXUS_VideoInput_P_SourceCallback_isr */
        *pStatus = link->status;
        break;
    }

done:
    return 0;
}

#if NEXUS_NUM_VIDEO_DECODERS
static void
NEXUS_VideoInput_P_DecoderDataReady_isr(void *input_, const BAVC_MFD_Picture *pPicture)
{
    NEXUS_VideoInput_P_Link *link = input_;
    BDBG_OBJECT_ASSERT(link, NEXUS_VideoInput_P_Link);
    BDBG_ASSERT(pPicture);
    link->info.mfd = *pPicture;
    link->info_valid = true;

#if NEXUS_NUM_MOSAIC_DECODES
    if (link->mosaic.backendMosaic) {
        unsigned i;
        for (i=0;i<NEXUS_NUM_DISPLAYS;i++) {
            NEXUS_VideoWindowHandle window = link->mosaic.parentWindow[i];
        if (window) {
            /* this takes a single picture and cuts it up into a linked list of mosaic pictures */
            pPicture = NEXUS_VideoWindow_P_CutBackendMosaic_isr(window, pPicture);
                BDBG_ASSERT(pPicture); /* if it can't cut, NEXUS_VideoWindow_P_CutBackendMosaic_isr should have returned the original pPicture */

                /* the first window gets to cut it. the other has to follow. */
                break;
            }
        }
    }
#endif

    BVDC_Source_MpegDataReady_isr(link->sourceVdc, 0 /* unused */, (void *)pPicture);
    return;
}

#if NEXUS_HAS_VIDEO_ENCODER && !NEXUS_NUM_DSP_VIDEO_ENCODERS
static void
NEXUS_VideoInput_P_UserDataCallback_isr(void *input_, const BAVC_USERDATA_info *userDataInfo)
{
    NEXUS_VideoInput_P_Link *link = input_;
    
    BDBG_OBJECT_ASSERT(link, NEXUS_VideoInput_P_Link);

    if( link->hXud )
    {
        BXUDlib_UserDataHandler_isr(link->hXud, userDataInfo);
    }
}
#endif

static BERR_Code
NEXUS_VideoInput_P_ConnectVideoDecoder(NEXUS_VideoInput_P_Link *link)
{
    BERR_Code rc;
    NEXUS_VideoDecoderDisplayConnection decoderConnect;

    BDBG_ASSERT(link->input && link->input->source);

    if (!pVideo->modules.videoDecoder) {
        return BERR_TRACE(NEXUS_NOT_SUPPORTED);
    }

#if NEXUS_NUM_MOSAIC_DECODES
    if (link->mosaic.backendMosaic) {
        /* the parent has already connected to the decoder */
        return 0;
    }
#endif

    NEXUS_Module_Lock(pVideo->modules.videoDecoder);
    NEXUS_VideoDecoder_GetDisplayConnection_priv(link->input->source, &decoderConnect);
    NEXUS_Module_Unlock(pVideo->modules.videoDecoder);

#if NEXUS_NUM_MOSAIC_DECODES
    link->mosaic.index = decoderConnect.mosaicIndex;
#endif
    rc = BVDC_Source_SetPsfMode(link->sourceVdc, decoderConnect.psfMode);
    if (rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc); goto err_setpsf;}

    rc = BVDC_Source_GetInterruptName(link->sourceVdc, BAVC_Polarity_eTopField, &decoderConnect.top.intId);
    if (rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc); goto err_intr_name;}
    rc = BVDC_Source_GetInterruptName(link->sourceVdc, BAVC_Polarity_eBotField, &decoderConnect.bottom.intId);
    if (rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc); goto err_intr_name;}
    rc = BVDC_Source_GetInterruptName(link->sourceVdc, BAVC_Polarity_eFrame, &decoderConnect.frame.intId);
    if (rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc); goto err_intr_name;}
    decoderConnect.callbackContext = link;
    decoderConnect.dataReadyCallback_isr = NEXUS_VideoInput_P_DecoderDataReady_isr;
    
    NEXUS_Module_Lock(pVideo->modules.videoDecoder);
    rc = NEXUS_VideoDecoder_SetDisplayConnection_priv(link->input->source, &decoderConnect);
    NEXUS_Module_Unlock(pVideo->modules.videoDecoder);
    if (rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc); goto err_connect;}

    return rc;

err_connect:
err_intr_name:
err_setpsf:
    return rc;
}

static void
NEXUS_VideoInput_P_DisconnectVideoDecoder(NEXUS_VideoInput_P_Link *link)
{
    NEXUS_Error rc;
    NEXUS_VideoDecoderDisplayConnection decoderConnect;

    if (!pVideo->modules.videoDecoder) {
        BERR_TRACE(NEXUS_NOT_SUPPORTED);
        return;
    }

#if NEXUS_NUM_MOSAIC_DECODES
    if (link->mosaic.backendMosaic) {
        /* the parent is the only one connected to the decoder */
        return;
    }
#endif

    BDBG_ASSERT(link->input && link->input->source);
    NEXUS_Module_Lock(pVideo->modules.videoDecoder);
    NEXUS_VideoDecoder_GetDisplayConnection_priv(link->input->source, &decoderConnect);
    decoderConnect.top.intId = 0;
    decoderConnect.bottom.intId = 0;
    decoderConnect.frame.intId = 0;
    decoderConnect.dataReadyCallback_isr = NULL;
    decoderConnect.userDataCallback_isr = NULL;
    rc = NEXUS_VideoDecoder_SetDisplayConnection_priv(link->input->source, &decoderConnect);
    NEXUS_Module_Unlock(pVideo->modules.videoDecoder);
    if (rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc); }
    return;
}

NEXUS_VideoInput_P_Link *
NEXUS_VideoInput_P_OpenDecoder(NEXUS_VideoInput input, NEXUS_VideoInput_P_Link *mosaicParent)
{
    BAVC_SourceId sourceId;
    NEXUS_VideoInput_P_Iface iface;
    NEXUS_VideoInput_P_LinkData data;
    NEXUS_HeapHandle videoDecoderHeap;
    NEXUS_VideoInput_P_Link *link;

    if (!pVideo->modules.videoDecoder) {
        BERR_TRACE(NEXUS_NOT_SUPPORTED);
        return NULL;
    }

    BDBG_ASSERT(input->source);
    NEXUS_Module_Lock(pVideo->modules.videoDecoder);
    NEXUS_VideoDecoder_GetSourceId_priv(input->source, &sourceId);
    NEXUS_VideoDecoder_GetHeap_priv(input->source, &videoDecoderHeap);
    NEXUS_Module_Unlock(pVideo->modules.videoDecoder);

    iface.connect = NEXUS_VideoInput_P_ConnectVideoDecoder;
    iface.disconnect = NEXUS_VideoInput_P_DisconnectVideoDecoder;
    NEXUS_VideoInput_P_LinkData_Init(&data, sourceId);
    data.heap = videoDecoderHeap;
#if NEXUS_NUM_MOSAIC_DECODES
    if (mosaicParent) {
        data.sourceVdc = mosaicParent->sourceVdc;
    }
#else
    BSTD_UNUSED(mosaicParent);
#endif

    link = NEXUS_VideoInput_P_CreateLink(input, &data, &iface);

#if NEXUS_NUM_MOSAIC_DECODES
    if (link && mosaicParent) {
        link->mosaic.backendMosaic = mosaicParent->mosaic.backendMosaic;
    }
#endif

    return link;
}
#endif

BERR_Code
NEXUS_VideoInput_P_Connect(NEXUS_VideoInput input)
{
    BERR_Code rc;
    NEXUS_VideoInput_P_Link *link;
    
    /* this function is called on the first connection back to the VideoInput source */

    BDBG_OBJECT_ASSERT(input, NEXUS_VideoInput);
    
    link = NEXUS_VideoInput_P_Get(input);
    if(!link) {rc=BERR_TRACE(BERR_NOT_SUPPORTED); goto err_connect;}

    link->info_valid = false;
    input->destination = link;

    rc = link->iface.connect(link);
    if (rc!=BERR_SUCCESS) {rc = BERR_TRACE(rc); goto err_connect;}

    return rc;

err_connect:
    /* NEXUS_VideoInput_Shutdown will still be required, but input->destination must be cleared because
    there is no connection to a window */
    input->destination = NULL;
    return rc;
}

void
NEXUS_VideoInput_P_Disconnect(NEXUS_VideoInput input) {
    NEXUS_VideoInput_P_Link *link;
    BERR_Code rc;
    BVDC_Dnr_Settings dnrSettings;

    BDBG_OBJECT_ASSERT(input, NEXUS_VideoInput);
    link = NEXUS_VideoInput_P_Get(input);
    if(!link) {rc=BERR_TRACE(BERR_NOT_SUPPORTED); goto done;}
    link->iface.disconnect(link);

    /* DNR is window-based in Nexus, source-based in VDC. So, we disable DNR when the VideoInput is disconnected from its last window. */
    rc = BVDC_Source_GetDnrConfiguration(link->sourceVdc, &dnrSettings);
    if (!rc) {
        dnrSettings.eBnrMode = BVDC_FilterMode_eDisable;
        dnrSettings.eDcrMode = BVDC_FilterMode_eDisable;
        dnrSettings.eMnrMode = BVDC_FilterMode_eDisable;
        rc = BVDC_Source_SetDnrConfiguration(link->sourceVdc, &dnrSettings);
        if (rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc); }
    }

    link->info_valid = false;
#if NEXUS_HAS_VIDEO_ENCODER && !NEXUS_NUM_DSP_VIDEO_ENCODERS
    link->hXud = NULL;
#endif
    input->destination = NULL;

done:
    return;
}

NEXUS_VideoInput_P_Link *
NEXUS_VideoInput_P_GetExisting(NEXUS_VideoInput input)
{
    NEXUS_VideoInput_P_Link *link;
    BDBG_OBJECT_ASSERT(input, NEXUS_VideoInput);
    link = input->destination;
    if(!link) {
        BLST_S_DICT_FIND(&pVideo->inputs, link, input, input, link);
        /* If we find one, this is a disconnected VideoInput. We will allow SetSettings to succeed,
        but settings will not be applied until connected. */
    }

    if (link) {
        BDBG_OBJECT_ASSERT(link, NEXUS_VideoInput_P_Link);
    }
    return link;
}

NEXUS_VideoInput_P_Link *
NEXUS_VideoInput_P_Get(NEXUS_VideoInput input)
{
    NEXUS_Error rc;
    NEXUS_VideoInput_P_Link *inputLink;

    BDBG_OBJECT_ASSERT(input, NEXUS_VideoInput);

    inputLink = input->destination;
    if(!inputLink) {
        BLST_S_DICT_FIND(&pVideo->inputs, inputLink, input, input, link);
        /* If we find one, this is a disconnected VideoInput. We will allow SetSettings to succeed,
        but settings will not be applied until connected. */
    }

    if(!inputLink) {
        BDBG_ASSERT(input->destination==NULL);
        switch(input->type) {
#if NEXUS_HAS_VIDEO_DECODER
        case NEXUS_VideoInputType_eDecoder:
            inputLink = NEXUS_VideoInput_P_OpenDecoder(input, NULL);
            break;
#endif
        case NEXUS_VideoInputType_eImage:
            inputLink = NEXUS_VideoImageInput_P_OpenInput(input);
            break;
#if NEXUS_NUM_ANALOG_VIDEO_DECODERS
        case NEXUS_VideoInputType_eAnalogVideoDecoder:
            inputLink = NEXUS_VideoInput_P_OpenAnalogVideoDecoder(input);
            break;
#endif
#if NEXUS_NUM_HDMI_INPUTS
        case NEXUS_VideoInputType_eHdmi:
            inputLink = NEXUS_VideoInput_P_OpenHdmi(input);
            break;
#endif
#if NEXUS_NUM_656_INPUTS
        case NEXUS_VideoInputType_eCcir656:
            inputLink = NEXUS_VideoInput_P_OpenCcir656(input);
            break;
#endif
#if NEXUS_NUM_HDDVI_INPUTS
        case NEXUS_VideoInputType_eHdDvi:
            inputLink = NEXUS_VideoInput_P_OpenHdDviInput(input);
            break;
#endif

        case NEXUS_VideoInputType_ePc:
        case NEXUS_VideoInputType_eComposite:
        case NEXUS_VideoInputType_eSvideo:
        case NEXUS_VideoInputType_eComponent:
        case NEXUS_VideoInputType_eIfd:
            /* These input types are used to connect to AnalogVideoDecoder only. This failure will
            occur with NEXUS_VideoInput_Get/SetSettings, NEXUS_VideoInput_GetStatus or NEXUS_VideoWindow_Add/RemoveInput. */
            BDBG_WRN(("This NEXUS_VideoInput type (%d) does not support this operation", input->type));
            rc = BERR_TRACE(NEXUS_NOT_SUPPORTED);
            inputLink = NULL;
            break;
        default:
            BDBG_WRN(("unknown VideoInput type: %d", input->type));
            rc = BERR_TRACE(NEXUS_NOT_SUPPORTED);
            inputLink = NULL;
            break;
        }
    }

    if(inputLink) {
        BDBG_OBJECT_ASSERT(inputLink, NEXUS_VideoInput_P_Link);
        BDBG_ASSERT(inputLink->input == input);
        BDBG_ASSERT(input->destination == NULL || input->destination == inputLink);
        BDBG_ASSERT(input->type == inputLink->input_info.type && input->source == inputLink->input_info.source);
    }
    return inputLink;
}

void
NEXUS_VideoInput_Shutdown(NEXUS_VideoInput input)
{
    NEXUS_VideoInput_P_Link *inputLink;
    NEXUS_Error rc;
    unsigned int i;

    BSTD_UNUSED(rc);
    NEXUS_OBJECT_ASSERT(NEXUS_VideoInput, input);

    if (input->destination) {
        /* check if this input is connected to a window and auto-remove. */
        while (1) {
            NEXUS_VideoWindowHandle window;
            unsigned num;
            rc = NEXUS_Display_P_GetWindows_priv(input, &window, 1, &num);
            if (rc) {rc = BERR_TRACE(rc); goto err_connected;}

            if (!num) break;

            rc = NEXUS_VideoWindow_RemoveInput(window, input);
            if (rc) {rc = BERR_TRACE(rc); goto err_connected;}
        }

        /* if we're still connected, then this video input must be connected to another module. */
        if (input->destination) {
            BDBG_ERR(("Fatal application error. NEXUS_VideoInput_Shutdown %p called on connected input. You must call NEXUS_VideoWindow_RemoveInput before shutting down, otherwise system is likely to be in a corrupted state and will crash.", input));
            rc = BERR_TRACE(NEXUS_INVALID_PARAMETER); /* input is still connected */
#if 0
            /* Uncomment this BKNI_Fail to get a stack trace into your application. This is a serious app bug and should be corrected. */
            BKNI_Fail();
#endif
            goto err_connected;
        }
    }

    for (i=0;i<NEXUS_NUM_DISPLAYS;i++) {
        if (pVideo->displays[i]) {
            NEXUS_DisplayVbiSettings dispvbiSettings;
            NEXUS_Display_GetVbiSettings(pVideo->displays[i], &dispvbiSettings);
            if (dispvbiSettings.vbiSource == input) {
                dispvbiSettings.vbiSource = NULL;
                NEXUS_Display_SetVbiSettings(pVideo->displays[i], &dispvbiSettings);
            }
        }
    }

    /* this is like calling NEXUS_VideoInput_P_Get, but without the possible CreateLink */
    inputLink = input->destination;
    if (!inputLink) {
        BLST_S_DICT_FIND(&pVideo->inputs, inputLink, input, input, link);
    }

    if (inputLink) {
        BDBG_OBJECT_ASSERT(inputLink, NEXUS_VideoInput_P_Link);
        BDBG_ASSERT(inputLink->input == input);
        BDBG_ASSERT(input->type == inputLink->input_info.type && input->source == inputLink->input_info.source);
        BDBG_ASSERT(input->ref_cnt > 0);

        /* this is the inverse of the OpenXxx calls in NEXUS_VideoInput_P_Get */
        switch (inputLink->input->type) {
#if NEXUS_NUM_ANALOG_VIDEO_DECODERS
        case NEXUS_VideoInputType_eAnalogVideoDecoder:
            /* CloseAnalogVideoDecoder must call NEXUS_VideoInput_P_DestroyLink */
            NEXUS_VideoInput_P_CloseAnalogVideoDecoder(inputLink);
            break;
#endif
        default:
            NEXUS_VideoInput_P_DestroyLink(inputLink);
            break;
        }
    }
err_connected:
    return;
}

void NEXUS_VideoInput_GetColorMatrix(NEXUS_VideoInput input, NEXUS_ColorMatrix *pColorMatrix)
{
    NEXUS_VideoInput_P_Link *link;

    BDBG_OBJECT_ASSERT(input, NEXUS_VideoInput);
    link = NEXUS_VideoInput_P_Get(input);
    if (link && link->sourceVdc) {
        if (link->bColorMatrixSet) {
            /* avoid VDC race conditions and return whatever was last set by the user. */
            *pColorMatrix = link->colorMatrix;
            return;
        }
        else {
            bool bOverride;
            NEXUS_Error rc;

            /* for video inputs, the BVDC_Source exists right away, so we can ask VDC for its default. */
            rc = BVDC_Source_GetColorMatrix(link->sourceVdc, &bOverride, pColorMatrix->coeffMatrix, &pColorMatrix->shift);
            if (!rc) return;
            /* else fall through */
        }
    }

    BKNI_Memset(pColorMatrix, 0, sizeof(*pColorMatrix));

    return;
}

NEXUS_Error NEXUS_VideoInput_SetColorMatrix(NEXUS_VideoInput input, const NEXUS_ColorMatrix *pColorMatrix)
{
    NEXUS_VideoInput_P_Link *link;
    BERR_Code rc;

    BDBG_OBJECT_ASSERT(input, NEXUS_VideoInput);
    link = NEXUS_VideoInput_P_Get(input);
    if (!link) return BERR_TRACE(NEXUS_UNKNOWN);
    if (!link->sourceVdc) {link->isDeferColorMatrix = true; link->colorMatrix = *pColorMatrix; return NEXUS_SUCCESS;}

    if (pColorMatrix) {
        link->colorMatrix = *pColorMatrix;
    }
    link->bColorMatrixSet = pColorMatrix != NULL;

    rc = BVDC_Source_SetColorMatrix(link->sourceVdc, link->bColorMatrixSet, link->colorMatrix.coeffMatrix, link->colorMatrix.shift);
    if (rc) return BERR_TRACE(rc);

    rc = NEXUS_Display_P_ApplyChanges();
    if (rc) return BERR_TRACE(rc);

    return 0;
}

void NEXUS_VideoInput_GetSyncSettings_priv(NEXUS_VideoInput videoInput, NEXUS_VideoInputSyncSettings *pSyncSettings)
{
    BDBG_OBJECT_ASSERT(videoInput, NEXUS_VideoInput);
    NEXUS_ASSERT_MODULE();

    switch (videoInput->type) {
#if NEXUS_HAS_VIDEO_DECODER
    case NEXUS_VideoInputType_eDecoder:
        if (!pVideo->modules.videoDecoder) {
            BERR_TRACE(NEXUS_NOT_SUPPORTED);
            return;
        }
        NEXUS_Module_Lock(pVideo->modules.videoDecoder);
        NEXUS_VideoDecoder_GetSyncSettings_priv((NEXUS_VideoDecoderHandle)videoInput->source, pSyncSettings);
        NEXUS_Module_Unlock(pVideo->modules.videoDecoder);
        break;
#else
        BSTD_UNUSED(pSyncSettings);
#endif
    default:
        BDBG_WRN(("SyncChannel not supported for this video input"));
        break;
    }
}

NEXUS_Error NEXUS_VideoInput_SetSyncSettings_priv(NEXUS_VideoInput videoInput, const NEXUS_VideoInputSyncSettings *pSyncSettings)
{
    NEXUS_Error rc = 0;
    NEXUS_VideoInput_P_Link * link;

    BDBG_OBJECT_ASSERT(videoInput, NEXUS_VideoInput);
    NEXUS_ASSERT_MODULE();

    /* two of the callbacks are handled by the input link class, the rest are handled per input type */
    link = NEXUS_VideoInput_P_Get(videoInput);
    if (!link) { return BERR_TRACE(NEXUS_NOT_SUPPORTED); }
    link->sync.connectionChangedCallback_isr = pSyncSettings->connectionChangedCallback_isr;
    link->sync.callbackContext = pSyncSettings->callbackContext;

    switch (videoInput->type) {
#if NEXUS_HAS_VIDEO_DECODER
    case NEXUS_VideoInputType_eDecoder:
        if (!pVideo->modules.videoDecoder) {
            return BERR_TRACE(NEXUS_NOT_SUPPORTED);
        }
        NEXUS_Module_Lock(pVideo->modules.videoDecoder);
        rc = NEXUS_VideoDecoder_SetSyncSettings_priv((NEXUS_VideoDecoderHandle)videoInput->source, pSyncSettings);
        NEXUS_Module_Unlock(pVideo->modules.videoDecoder);
        break;
#else
        BSTD_UNUSED(pSyncSettings);
#endif
    default:
        BDBG_WRN(("SyncChannel not supported for this video input"));
        break;
    }
    return rc;
}

NEXUS_Error NEXUS_VideoInput_GetSyncStatus_isr(NEXUS_VideoInput videoInput, NEXUS_VideoInputSyncStatus *pSyncStatus)
{
    NEXUS_Error rc = 0;

    BDBG_OBJECT_ASSERT(videoInput, NEXUS_VideoInput);

    switch (videoInput->type) {
#if NEXUS_HAS_VIDEO_DECODER
    case NEXUS_VideoInputType_eDecoder:
        rc = NEXUS_VideoDecoder_GetSyncStatus_isr((NEXUS_VideoDecoderHandle)videoInput->source, pSyncStatus);
        break;
#else
        BSTD_UNUSED(pSyncStatus);
#endif
    default:
        BDBG_WRN(("SyncChannel not supported for this video input"));
        break;
    }
    return rc;
}

NEXUS_Error NEXUS_Display_P_GetWindows_priv(NEXUS_VideoInput videoInput, NEXUS_VideoWindowHandle *pWindowArray, unsigned arraySize, unsigned *numFilled)
{
    unsigned i,j;

    NEXUS_ASSERT_MODULE();
    BDBG_OBJECT_ASSERT(videoInput, NEXUS_VideoInput);

    *numFilled = 0;
    for (i=0;i<NEXUS_NUM_DISPLAYS;i++) {
        for (j=0;j<NEXUS_NUM_VIDEO_WINDOWS;j++) {
            if (*numFilled == arraySize) return 0;
            if (pVideo->displays[i] && pVideo->displays[i]->windows[j].input == videoInput) {
                pWindowArray[(*numFilled)++] = &pVideo->displays[i]->windows[j];
            }
        }
    }
    return 0;
}

void
NEXUS_Display_P_VideoInputDisplayUpdate(NEXUS_VideoInput videoInput, NEXUS_VideoWindowHandle window, const NEXUS_DisplaySettings *pSettings)
{
#if NEXUS_NUM_VIDEO_DECODERS
    NEXUS_VideoDecoder_DisplayInformation displayInformation;
#endif
#if NEXUS_HAS_VIDEO_DECODER
    BVDC_Window_Status vdcWindowStatus;
#endif
#if NEXUS_NUM_MOSAIC_DECODES
    NEXUS_VideoWindowHandle mosaicChild;
#endif
#if defined(HUMAX_PLATFORM_BASE)
	NEXUS_VideoDecoderSettings decoderSettings;
	NEXUS_VideoDecoderSourceOrientation sourceOrientation;
#endif

    BDBG_OBJECT_ASSERT(videoInput, NEXUS_VideoInput);
    BDBG_OBJECT_ASSERT(window, NEXUS_VideoWindow);
    BDBG_ASSERT(pSettings);

#if NEXUS_NUM_MOSAIC_DECODES
    /* pass along to each mosaic child */
    for (mosaicChild = BLST_S_FIRST(&window->mosaic.children); mosaicChild; mosaicChild = BLST_S_NEXT(mosaicChild, mosaic.link)) {
        if (mosaicChild->input) {
            NEXUS_Display_P_VideoInputDisplayUpdate(mosaicChild->input, mosaicChild, pSettings);
        }
    }
#endif

    switch (videoInput->type) {
#if NEXUS_HAS_VIDEO_DECODER
    case NEXUS_VideoInputType_eDecoder:
        if (!pVideo->modules.videoDecoder) {
            BERR_TRACE(NEXUS_NOT_SUPPORTED);
            return;
        }

    #if NEXUS_NUM_MOSAIC_DECODES        
        if (window->mosaic.parent) {
            /* mosaic windows have no VDC window and sync-locked is handled with BVDC_Window_SetMosaicTrack, so always pass the info along. */
        }
        /* if the window is not sync-locked, don't feed the display info back to the video decoder input */
        else
    #endif
        if (!window->vdcState.window || BVDC_Window_GetStatus(window->vdcState.window, &vdcWindowStatus) || !vdcWindowStatus.bSyncLock ) {
            return;
        }
        
        displayInformation.refreshRate = window->display->status.refreshRate / 10;
        displayInformation.stgIndex = 0;
        if(pSettings->timingGenerator == NEXUS_DisplayTimingGenerator_eEncoder) {
            displayInformation.stgIndex = window->display->stgIndex;
            BDBG_MSG(("display stg%d vfreq=%d", displayInformation.stgIndex, displayInformation.refreshRate));
        }
#if defined(HUMAX_PLATFORM_BASE)
		switch(pSettings->display3DSettings.orientation)
		{
			case NEXUS_VideoOrientation_e3D_LeftRight:
				sourceOrientation = NEXUS_VideoDecoderSourceOrientation_e3D_LeftRight;
				break;
			case NEXUS_VideoOrientation_e3D_OverUnder:
				sourceOrientation = NEXUS_VideoDecoderSourceOrientation_e3D_OverUnder;
				break;
			case NEXUS_VideoOrientation_e2D:
			default:
				sourceOrientation = NEXUS_VideoDecoderSourceOrientation_e2D;
				break;
		}

		NEXUS_VideoDecoder_GetSettings((NEXUS_VideoDecoderHandle)videoInput->source, &decoderSettings);
		if(decoderSettings.customSourceOrientation != true || decoderSettings.sourceOrientation != sourceOrientation)
		{
			decoderSettings.customSourceOrientation = true;
			decoderSettings.sourceOrientation = sourceOrientation;
			NEXUS_VideoDecoder_SetSettings((NEXUS_VideoDecoderHandle)videoInput->source, &decoderSettings);
		}
#endif

        NEXUS_Module_Lock(pVideo->modules.videoDecoder);
        NEXUS_VideoDecoder_UpdateDisplayInformation_priv((NEXUS_VideoDecoderHandle)videoInput->source, &displayInformation);
#if NEXUS_HAS_VIDEO_ENCODER && !NEXUS_NUM_DSP_VIDEO_ENCODERS
        {
            NEXUS_VideoInput_P_Link *link;
            NEXUS_VideoDecoderDisplayConnection decoderConnect;

            link = NEXUS_VideoInput_P_Get(videoInput);
            if ( !link ) { BERR_TRACE(NEXUS_UNKNOWN); return; }
            link->hXud = window->display->encodeUserData?window->display->hXud:NULL;

            NEXUS_VideoDecoder_GetDisplayConnection_priv(link->input->source, &decoderConnect);
            decoderConnect.userDataCallback_isr = window->display->encodeUserData?NEXUS_VideoInput_P_UserDataCallback_isr:NULL;
            NEXUS_VideoDecoder_SetDisplayConnection_priv(link->input->source, &decoderConnect);
        }
#endif
        NEXUS_Module_Unlock(pVideo->modules.videoDecoder);
        break;
#endif
    case NEXUS_VideoInputType_eImage:
        NEXUS_VideoImageInput_P_UpdateDisplayInformation((NEXUS_VideoImageInputHandle)videoInput->source, pSettings);
        break;
    default:
        break;
    }
    return;
}

NEXUS_Error NEXUS_VideoInput_GetCrcData( NEXUS_VideoInput input, NEXUS_VideoInputCrcData *pData, unsigned numEntries, unsigned *numEntriesReturned )
{
#if NEXUS_CRC_CAPTURE
    NEXUS_VideoInput_P_Link *link = NEXUS_VideoInput_P_Get(input);

    *numEntriesReturned = 0;

    if (!link) return 0;

    while (*numEntriesReturned < numEntries && link->crc.rptr != link->crc.wptr) {
        pData[*numEntriesReturned] = link->crc.data[link->crc.rptr];
        if (++link->crc.rptr == NEXUS_VIDEO_INPUT_CRC_QUEUE_SIZE) {
            link->crc.rptr = 0;
        }
        (*numEntriesReturned)++;
    }
    return 0;
#else
    BSTD_UNUSED(input);
    BSTD_UNUSED(pData);
    BSTD_UNUSED(numEntries);
    BSTD_UNUSED(numEntriesReturned);
    return BERR_TRACE(NEXUS_NOT_SUPPORTED);
#endif
}

NEXUS_Error NEXUS_VideoInput_SetResumeMode( NEXUS_VideoInput videoInput, NEXUS_VideoInputResumeMode resumeMode )
{
    NEXUS_VideoInput_P_Link *link;
    BERR_Code rc;
    BVDC_ResumeMode vdcMode;

    BDBG_OBJECT_ASSERT(videoInput, NEXUS_VideoInput);
    link = NEXUS_VideoInput_P_Get(videoInput);
    if ( !link ) return BERR_TRACE(NEXUS_UNKNOWN);
    if ( !link->sourceVdc ) { link->resumeMode = resumeMode; return NEXUS_SUCCESS; }

    if ( resumeMode == NEXUS_VideoInputResumeMode_eFreezeRts )
    {
        if ( !g_NEXUS_DisplayModule_State.moduleSettings.handleDynamicRts )
        {
            BDBG_ERR(("Bandwidth can only be frozen while dynamic RTS management is active"));
            return BERR_TRACE(BERR_NOT_SUPPORTED);
        }
    }

    if ( resumeMode == NEXUS_VideoInputResumeMode_eNow ||
         resumeMode == NEXUS_VideoInputResumeMode_eFreezeRts )
    {
        if ( g_NEXUS_DisplayModule_State.moduleSettings.handleDynamicRts &&
             link->resumeMode == NEXUS_VideoInputResumeMode_eManual )
        {
            /* Temporarily set to eNow so that VDB is processed. Otherwise eManual causes VDB-recalc to be bypassed. */
            link->resumeMode = NEXUS_VideoInputResumeMode_eNow;
            rc = NEXUS_Vdb_P_HandleSourcePending(videoInput);
            link->resumeMode = NEXUS_VideoInputResumeMode_eManual;
            if ( rc ) return BERR_TRACE(rc);
        }

        rc = BVDC_Source_Resume(link->sourceVdc);
        if ( rc ) return BERR_TRACE(rc);

        /* We must always apply changes when resuming */
        if ( pVideo->updateMode != NEXUS_DisplayUpdateMode_eAuto )
        {
            rc=BERR_TRACE(NEXUS_NOT_SUPPORTED);
        }

        if ( link->resumeMode == NEXUS_VideoInputResumeMode_eFreezeRts ||
             resumeMode == NEXUS_VideoInputResumeMode_eFreezeRts )
        {
            /* Allow the source to free-run at this point.  The app must exit this mode to change bandwidth later on. */
            (void)BVDC_Source_SetResumeMode(link->sourceVdc, NEXUS_VideoInputResumeMode_eAuto);
            link->resumeMode = NEXUS_VideoInputResumeMode_eFreezeRts;
        }

        rc = BVDC_ApplyChanges(pVideo->vdc);
        if ( rc ) return BERR_TRACE(rc);

        link->status.sourcePending = false;
        return NEXUS_SUCCESS;   /* Nothing more to do, return here to avoid a double apply changes */
    }

    /* If we have a dynamic RTS configuration, use that instead of VDC's auto resume mode. */
    if ( g_NEXUS_DisplayModule_State.moduleSettings.handleDynamicRts )
    {
        vdcMode = BVDC_ResumeMode_eManual;
    }
    else
    {
        vdcMode = (resumeMode == NEXUS_VideoInputResumeMode_eAuto) ? BVDC_ResumeMode_eAuto : BVDC_ResumeMode_eManual;
    }

    rc = BVDC_Source_SetResumeMode(link->sourceVdc, vdcMode);
    if ( rc ) return BERR_TRACE(rc);

    if ( pVideo->updateMode != NEXUS_DisplayUpdateMode_eAuto )
    {
        rc=BERR_TRACE(NEXUS_NOT_SUPPORTED);
    }
    rc = BVDC_ApplyChanges(pVideo->vdc);
    if ( rc ) return BERR_TRACE(rc);

    link->resumeMode = resumeMode;
    return 0;
}

NEXUS_Error NEXUS_VideoInput_ForcePending(
    NEXUS_VideoInput videoInput
    )
{
    NEXUS_VideoInput_P_Link *link;
    BERR_Code rc;

    BDBG_OBJECT_ASSERT(videoInput, NEXUS_VideoInput);
    link = NEXUS_VideoInput_P_Get(videoInput);
    if (!link) {return BERR_TRACE(NEXUS_UNKNOWN);}
    if (!link->sourceVdc) {link->isDeferForcePending = true; return NEXUS_SUCCESS;}

    if ( link->resumeMode != NEXUS_VideoInputResumeMode_eManual )
    {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
    rc = BVDC_Source_ForcePending(link->sourceVdc);
    if ( rc ) {return BERR_TRACE(rc);}
    link->status.sourcePending = true;
    rc = NEXUS_Display_P_ApplyChanges();
    if (rc) {return BERR_TRACE(rc);}

    return NEXUS_SUCCESS;
}

void NEXUS_VideoInput_P_CheckFormatChange_isr(void *pParam)
{
    NEXUS_VideoInput_P_Link *pLink = pParam;

    BDBG_ASSERT(NULL != pLink);

    if ( pLink->checkFormatChangedEvent )
    {
        BKNI_SetEvent_isr(pLink->checkFormatChangedEvent);
    }
}

static void NEXUS_VideoInput_P_CheckFormatChanged(void *pParam)
{
    NEXUS_VideoInput source = pParam;
    NEXUS_Vdb_P_SourceModeChanged(source);
}

