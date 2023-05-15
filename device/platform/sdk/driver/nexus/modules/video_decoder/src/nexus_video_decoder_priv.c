/***************************************************************************
 *     (c)2007-2013 Broadcom Corporation
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
 * $brcm_Workfile: nexus_video_decoder_priv.c $
 * $brcm_Revision: SW7425-4680-1/1 $
 * $brcm_Date: 6/24/13 12:34p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/video_decoder/src/nexus_video_decoder_priv.c $
 * 
 * SW7425-4680-1/1   6/24/13 12:34p scho
 * SW7425-4680-1: continue processing userdata for 3DTV signaling during
 *  trick mode
 * 
 * 176   10/10/12 4:30p bandrews
 * SW7425-4021: fix watchdog to work even if data ready callback isn't
 *  coming
 * 
 * 175   10/9/12 3:07p jtna
 * SW7425-4047: make nexus set xvd 3d orientation for mpeg2 streams
 * 
 * 174   10/8/12 3:29p jtna
 * SW7425-4047: make nexus set xvd 3d orientation for mpeg2 streams
 * 
 * 173   9/13/12 2:37p erickson
 * SW7435-351: add NEXUS_VideoDecoderStreamInformation.valid
 * 
 * 172   9/12/12 3:59p jgarrett
 * SW7425-3789: Calling appropriate GetPts routine for XDM/VDE decoders
 * 
 * 171   8/31/12 4:22p bandrews
 * SW7420-2314: always check internal mute var instead of sync settings
 * 
 * 170   8/31/12 11:23a erickson
 * SW7425-3594: avoid possible div-by-zero, increase precision by
 *  deferring divide
 * 
 * 169   8/29/12 4:20p bandrews
 * SW7425-3820: take PTS movement into account for FIFO watchdog; add WRN
 *  when flushing caused by FIFO WD; fix function name for epilogue
 * 
 * 168   8/22/12 8:54p bandrews
 * SW7420-2314: mute control now applies to a persistent startMute flag;
 *  unmute happens immediately
 * 
 * 167   8/9/12 2:54p gmohile
 * SW7425-3668 : Check for all active decoder channels before
 *  unregistering vdc interrupt
 * 
 * 166   8/8/12 6:17p jtna
 * SW7425-3488: clear NEXUS_VideoDecoder3DTVStatus.format on timeout
 * 
 * 165   8/8/12 11:34a gmohile
 * SW7425-3668 : Do not register vdc interrupt if decoder is already
 *  started
 * 
 * 164   8/7/12 12:55p gmohile
 * SW7425-3668 : Disconnect BVN interrupt to video decoder
 * 
 * 163   8/3/12 7:04p bandrews
 * SW7420-2314: merge to main
 * 
 * 162   7/27/12 7:02p hongtaoz
 * SW7425-3424: added FNRT support to video decoder;
 * 
 * FNRT_Demo/1   7/6/12 5:30p hongtaoz
 * SW7425-3424: add FNRT support to video decoder;
 * 
 * SW7420-2314/5   7/23/12 7:16p bandrews
 * SW7420-2314: add event for window source changing
 * 
 * SW7420-2314/4   7/5/12 5:21p bandrews
 * SW7231-876: fixed warnings
 * 
 * SW7420-2314/3   6/14/12 5:23p bandrews
 * SW7420-2314: fixing comment
 * 
 * SW7420-2314/2   6/14/12 5:09p bandrews
 * SW7420-2314: actually, don't want to unmute if sync is asking to mute
 *  but video is started
 * 
 * SW7420-2314/1   6/14/12 5:03p bandrews
 * SW7420-2314: avoid muting while started in video decoder
 * 
 * 161   4/11/12 4:08p bandrews
 * SW7405-5612: merge to main
 * 
 * SW7405-5612/2   4/11/12 3:56p bandrews
 * SW7405-5612: merge from main
 * 
 * 160   3/28/12 6:01p bandrews
 * SW7425-2731: override sync mute if entering trick mode
 * 
 * 159   3/14/12 12:34p erickson
 * SW7401-4517: remove old code
 * 
 * 158   3/1/12 4:12p erickson
 * SW7425-2527: disable userdata in host trick mode. it will be corrupted.
 * 
 * SW7405-5612/1   2/20/12 8:39p bandrews
 * SW7405-5612: initial implementation
 * 
 * 157   12/16/11 5:28p erickson
 * SW7425-1878: merge XUDlib support for video encoder userdata
 * 
 * SW7425-1878/1   12/15/11 7:15a ajagmag
 * SW7425-1878: Bug fixes for user data encode use case
 * 
 * 156   11/1/11 2:15p erickson
 * SW7425-1264: enable BXVD_Interrupt_eRequestSTC for simulated STC for
 *  faster response
 * 
 * 155   10/27/11 9:21a erickson
 * SW7425-1264: update NEXUS_TsmMode_eSimulated
 * 
 * 154   10/24/11 2:05p erickson
 * SW7425-1264: update NEXUS_TsmMode_eSimulated
 * 
 * 153   10/12/11 10:21a erickson
 * SW7425-1264: add NEXUS_TsmMode_eSimulated
 *
 * 152   10/3/11 10:15a erickson
 * SW7425-1367: move pictureCoding to NEXUS_VideoDecoderStatus so we get
 *  fewer callbacks
 *
 * 151   9/29/11 10:18a erickson
 * SW7425-1367: added NEXUS_VideoDecoderStreamInformation.pictureCoding
 *
 * 150   9/19/11 10:10a ahulse
 * SW7405-5416: only call SetXvdDisplayIrq when have dataReadyCallback
 *
 * 149   8/12/11 4:18p jtna
 * SW7425-1070: merge video primer ptsStcDiffCorrection feature
 *
 * 148   8/8/11 8:06p bandrews
 * SW3548-1159: merge to main
 *
 * 147   6/14/11 6:14p hongtaoz
 * SW7425-654: improved NRT stcIncrement precision for 1.001 slip cases;
 *
 * 146   6/14/11 3:11p vsilyaev
 * SW7425-654: Propagate STG/trigger index from display to the video
 *  decoder
 *
 * 145   6/13/11 7:50p hongtaoz
 * SW7425-654: fixed NRT stcIncrement math;
 *
 * 144   6/10/11 3:54p vsilyaev
 * SW7425-654: Added triggerNumber to the
 *  NEXUS_StcChannelNonRealtimeSettings
 *
 * 143   6/9/11 6:44p vsilyaev
 * SW7425-654: Added support for non realtime video decode
 *
 * 142   5/13/11 3:24p hongtaoz
 * SW7425-580: changed large ulAdjQp debug message to warning;
 *
 * 141   5/12/11 3:44p hongtaoz
 * SW7425-466: rework avd/mfd/decoder/xdm mapping;
 *
 * SW7425-466/3   5/12/11 3:30p hongtaoz
 * SW7425-466: compiled for !B_USE_NEW_XVD;
 *
 * SW7425-466/2   5/12/11 12:04p hongtaoz
 * SW7425-466: added the XDM/decoder mapping to be scalable;
 *
 * SW7425-466/1   5/11/11 12:59p hongtaoz
 * SW7425-466: rework avd/mfd/decoder channel mapping;
 *
 * 140   4/15/11 7:18p bandrews
 * SW7405-5118: there were several other spots where both audio and video
 *  PTS structs were being copied directly to AVC; needed to convert these
 *  properly
 *
 * 139   4/14/11 3:30p jtna
 * SWDTV-6321: fix build warnings for 2.6.37 kernel
 *
 * 138   3/4/11 4:16p jtna
 * SW7405-5168: schedule another 3DTV timeout if timeout is ignored due to
 *  decoder starvation
 *
 * 137   2/22/11 5:14p jtna
 * SW7405-5135: assume 2D if cancel flag is set
 *
 * 136   1/26/11 7:00p bandrews
 * SW7405-5118: perform correct mapping
 *
 * 135   1/7/11 4:31p jtna
 * SWDEPRECATED-3067: add sync channel support for channel change
 *  measurements
 *
 * 134   12/22/10 6:17p vsilyaev
 * SW7422-160: Use pictureDeliveryCount to curb fals positives on static
 *  CDB
 *
 * 133   12/10/10 5:06p erickson
 * SW35230-2209: add NEXUS_VideoDecoderSettings.scanMode
 *
 * 132   11/16/10 5:34p jtna
 * SW7405-4558: enable userdata parsing based on
 *  NEXUS_VideoDecoderExtendedSettings.s3DTVStatusEnabled
 *
 * 131   11/16/10 4:19p jtna
 * SW7405-4997: Add 3DTV status timer
 *
 * 130   11/8/10 4:46p jtna
 * SW7405-4817: add Top/Bottom case to JP3D signaling
 *
 * 129   10/26/10 3:17p jtna
 * SW7405-3998: added memcmp of 3dtv status
 *
 * SW3548-1159/1   11/2/10 6:39p bandrews
 * SW3548-1159: added non-TSM mode recovery methods and some options for
 *  controlling ASTM better
 *
 * 128   10/19/10 12:50p nickh
 * SW7422-79: Add flexible mapping scheme of video decoders and MFD
 *  sources
 *
 * 127   10/18/10 2:27p jtna
 * SW35230-1558: add B_PERF_LINUX support to channel change report
 *
 * 126   10/14/10 6:39p jtna
 * SW7405-4817: add JP3D MPEG2 3D userdata signaling
 *
 * 125   9/29/10 3:18p jtna
 * SW7405-4723: remove unneeded primer profiling code
 *
 * 124   9/29/10 12:42p erickson
 * SW7550-586: add default values for NEXUS_MatrixCoefficients,
 *  NEXUS_ColorPrimaries, and NEXUS_TransferCharacteristics
 *
 * 123   9/9/10 11:32a jtna
 * SW7405-4702: NEXUS_VideoDecoder_CreateStripedMosaicSurfaces() now
 *  returns NULL on repeat surfaces
 *
 * 122   9/8/10 12:25p vsilyaev
 * SW7468-129: Added video decoder on ZSP
 *
 * 121   9/1/10 10:31a erickson
 * SW7400-2870: fix warnings for NEXUS_VIDEO_DECODER_SINGLEDECODE mode
 *
 * 120   8/26/10 2:00p spothana
 * SW7420-975: Request STC in case XVD input overflows only in the live
 *  mode.
 *
 * 119   8/16/10 2:57p jtna
 * SW7405-4702: NEXUS_VideoDecoder_CreateStripedSurface() returns NULL on
 *  repeat surfaces
 *
 * 118   8/6/10 11:11a jtna
 * SW7405-4560: change Nexus impl to match new XVD API for AVC SEI message
 *  handling
 *
 * 117   7/7/10 11:46a mward
 * SW7400-2842: Add an option NEXUS_VIDEO_DECODER_SINGLEDECODE=y for XVD
 *  singledecode on 97400.
 *
 * 116   6/29/10 2:27p jtna
 * SW7405-3998: update NEXUS_VideoDecoder_Get3DTVStatus to return codec-
 *  generic 3DTV status
 *
 * 115   6/3/10 4:14p erickson
 * SW7405-4378: added
 *  NEXUS_VideoDecoderStreamInformation.codedSourceHorizontalSize and
 *  .codedSourceVerticalSize
 *
 * 114   5/19/10 10:37p bandrews
 * SW7325-706: correct check in comment
 *
 * 113   5/19/10 10:17p bandrews
 * SW7325-706: on overflow, video dumps any pcr offsets it had, need to
 *  force a request stc in nexus
 *
 * 112   5/17/10 11:16a erickson
 * SW7405-4350: fix coverity static overrun warning
 *
 * 111   5/12/10 12:28p jtna
 * SW7405-3998: merge
 *
 * 110   5/4/10 5:31p mward
 * SW7125-347: Set the vbiDataCallback_isr and vbiVideoInput in
 *  CriticalSection.
 *
 * 109   4/21/10 11:43a erickson
 * SW7550-391: must memset(0) NEXUS_VideoDecoderStreamInformation in
 *  NEXUS_VideoDecoder_P_PictureParams_isr to be future-proof
 *
 * 108   3/26/10 2:27p bandrews
 * SW7405-3774: disable freeze control used by sync
 *
 * 107   3/18/10 3:50p gmohile
 * SW7405-2993 : Merge XDM changes
 *
 * 106   3/10/10 2:34p erickson
 * SW7405-3965: add NEXUS_VideoDecoderDisplayConnection.parentIndex
 *
 * 105   2/24/10 6:42p jtna
 * SWDEPRECATED-3067: short circuit channel change report if count is 0
 *
 * 104   2/10/10 10:36a erickson
 * SW7405-3891: rename macro to B_USE_NEW_XVD
 *
 * 103   2/5/10 4:04p erickson
 * SW3548-2766: add BDBG_ASSERT for some BAVC_MFD_Picture fields
 *
 * 102   2/2/10 7:26p bandrews
 * SW7405-3774: add validity flag for delay
 *
 * 101   2/1/10 8:08p bandrews
 * SW7405-3774: add mute control support to sync channel
 *
 * 100   1/11/10 2:02p erickson
 * SW7405-3718: use AVD's parsing of AFD from userdata instead of VBIlib.
 *  this eliminates a redundant code path. it also allows the AFD value to
 *  be obtained even if not connected to a video window.
 *
 * 99   12/30/09 5:17p erickson
 * SW7405-3276: add NEXUS_VideoDecoderSettings.manualPowerState and
 *  NEXUS_VideoDecoder_SetPowerState
 *
 * 98   12/9/09 3:10p erickson
 * SW7405-3394: refactor mosaic mode API
 *
 * 97   11/12/09 11:11a erickson
 * SW3556-836: added
 *  NEXUS_VideoDecoderStreamInformation.fixedFrameRateFlag
 *
 * 96   11/9/09 4:57p erickson
 * SW3556-836: add NEXUS_VideoDecoderStreamInformation.lowDelayFlag
 *
 * 95   11/6/09 5:27p erickson
 * SW7405-3334: refactor to remove display & mosaic logic from
 *  nexus_video_decoder_userdata.c code
 *
 * 94   11/6/09 4:30p erickson
 * SW7405-3334: implement NEXUS_VideoDecoder_SetMosaicVbiRouting
 *
 * 93   11/4/09 3:44p erickson
 * SW7405-3313: copy last_field for every mosaic for GetStatus
 *
 * 92   11/4/09 1:34p erickson
 * SW7405-3332: check if any mosaic started when disconnecting the main
 *  input
 *
 * 91   10/26/09 2:09p erickson
 * SW7405-3089: update display information for all mosaics. also, separate
 *  the private API from the internal implementation so we don't get
 *  excessive calls.
 *
 * 90   10/26/09 12:28p erickson
 * SW7405-3287: add support for sw-based pcr offset for mosaic playback
 *  TSM support
 *
 * 89   10/20/09 10:09a vsilyaev
 * SW7405-3258: Removed reference to priv/nexus_transport_priv.h
 *
 * 88   10/2/09 3:58p erickson
 * SW7405-2991: add frame rate info to dataready_isr MSG
 *
 * 87   9/14/09 10:07a erickson
 * SW7405-2991: update dataready_isr MSG with more useful info
 *
 * 86   9/10/09 12:22p jtna
 * SW7405-3003: add video primer profiling
 *
 * 85   9/8/09 4:18p erickson
 * SW7420-283: add support for linked mosaic decoders
 *
 * 84   8/5/09 10:02a gmohile
 * PR 56512 : Merge 7401/03 support ot mainline
 *
 * 83   7/30/09 3:59p jtna
 * PR42924: add missing frontend support
 *
 * 82   7/24/09 3:50p bandrews
 * PR52812: copy delay status into sync status
 *
 * 81   7/23/09 12:22p erickson
 * PR56967: added NEXUS_VideoDecoderSettings.decodeError callback
 *
 * 80   7/17/09 7:17p bandrews
 * PR49215: astm dvr support
 *
 * 79   7/16/09 12:29p jtna
 * PR56696: add preprocess picture hook
 *
 * 78   6/8/09 7:09a erickson
 * PR55617: allow user to override unknown aspect ratio
 *
 * 77   6/2/09 12:15p erickson
 * PR55639: improve accuracy of VideoDecoder status and stream information
 *  by adjusting timing and field selection
 *
 * 76   5/29/09 11:51a erickson
 * PR55541: don't count or report the first muted pictures after a decode
 *  start
 *
 * 75   5/28/09 11:06a erickson
 * PR55483: NEXUS_VideoDecoder_GetHeap_priv should not return AVD's
 *  default heap
 *
 * 74   5/19/09 11:00a erickson
 * PR54880: fix NEXUS_VideoDecoder_GetHeap_priv
 *
 * 73   5/15/09 2:06p erickson
 * PR55094: add NEXUS_VideoDecoderStatus.numWatchdogs
 *
 * 72   5/8/09 12:17p erickson
 * PR54844: mosaicMode must be tracked per channel, not per device.
 *
 * 71   4/28/09 4:04p erickson
 * PR54676: allow VideoDecoder to incrementally create/destroy channels
 *
 * 70   4/27/09 9:48a erickson
 * PR54614: check for NULL videoDecoder->startSettings.stcChannel
 *
 * 69   4/15/09 4:22p jtna
 * PR53781: check for DSS and don't assume 45KHz
 *
 * 68   3/30/09 11:16a erickson
 * PR48963: added NEXUS_VideoDecoder_GetHeap_priv
 *
 * 67   2/26/09 11:43a erickson
 * PR52487: fix VideoDecoder watchdog. don't enable contexts that aren't
 *  in use.
 *
 * 66   2/17/09 1:47p erickson
 * PR51134: convert to BXVD_MONITOR_REFRESH_RATE values
 *
 * 65   2/17/09 11:46a erickson
 * PR51134: support drop-frame refresh rates in
 *  NEXUS_VideoDecoder_UpdateDisplayInformation_priv
 *
 * PR51105/2   2/5/09 3:35p erickson
 * PR51105: update branch
 *
 * 63   1/29/09 5:27p erickson
 * PR51694: added NEXUS_VideoDecoderStatus.ptsStcDifference
 *
 * 62   1/29/09 4:08p erickson
 * PR51134: add repeat flag, print more relevant address in DataReady isr
 *  MSG
 *
 * 61   1/19/09 5:22p bandrews
 * PR51021: merge
 *
 * PR51021/2   1/16/09 4:13p bandrews
 * PR51021: Updated to separate permission from enable
 *
 * PR51021/1   1/16/09 12:18a bandrews
 * PR51021: First stab at cleaning up ASTM playback blockage
 *
 * 60   12/30/08 1:59p erickson
 * PR49582: added debug
 *
 * 59   12/29/08 2:35p erickson
 * PR50749: fix mosaic mode. multiple OpenChannel's required when one
 *  VideoDecoder is connected to a VideoWindow.
 *
 * 58   12/17/08 2:52p bandrews
 * PR49720: Added watchdog recovery to astm
 *
 * 57   12/15/08 10:33a erickson
 * PR50236: added BDBG_OBJECT_ASSERT
 *
 * 56   12/10/08 4:41p erickson
 * PR49930: add NEXUS_VideoDecoderPlaybackSettings for playback
 *  accurateSeek algorithm
 *
 * 55   12/9/08 6:22p erickson
 * PR49923: added firstPtsPassed callback
 *
 * 54   12/8/08 3:05p bandrews
 * PR50016: ensure that even if playback is set before astm stops we
 *  default dependencies
 *
 * 53   12/8/08 12:22p erickson
 * PR48155: check cdb valid and read to determine if fifo is hung
 *
 * 52   12/8/08 11:16a erickson
 * PR49930: add NEXUS_VideoDecoder_SetStartPts
 *
 * 51   11/18/08 8:53p bandrews
 * PR49212: Temporarily block ASTM during playback
 *
 * 50   11/18/08 3:09p maivu
 * PR 47967: undo 3548/3556 B0 AVD/MEMC hang
 *
 * 49   11/18/08 11:55a erickson
 * PR48155: check for static fifo, then call stc channel to see if we have
 *  a TSM deadlock. if so, flush to recover.
 *
 * 48   11/9/08 11:16a maivu
 * PR47967: Added run-time option to force i-only for "iframe_only" and
 *  "iframe_mpeg2"
 *
 * 47   11/5/08 11:11a erickson
 * PR47967: added AVD/MEMC 3548/3556 B0 workaround for DivX
 *
 * 46   10/30/08 11:42p erickson
 * PR48453: fix warning
 *
 * 45   10/30/08 10:31p bandrews
 * PR48453: DSS support added
 *
 * 44   10/29/08 10:06a erickson
 * PR48406: added NEXUS_VideoDecoderTrickState.stcTrickEnabled so Nexus
 *  can ignore certain PTS error interrupts
 *
 * 43   10/20/08 4:12p erickson
 * PR47030: init struct with BXVD_GetVdcDeviceInterruptDefaultSettings
 *
 * 42   10/17/08 4:24p erickson
 * PR47030: added ERR's for out of sequence calls
 *
 * 41   10/17/08 9:16a erickson
 * PR47030: open and close the XVD channel when the VideoDecoder is
 *  connected or disconnected to a VideoWindow
 *
 * 40   10/8/08 12:51a erickson
 * PR47666: add firstPts callback
 *
 * 39   10/3/08 11:18a vsilyaev
 * PR 45361: Fixed BTRC and BPROFILE support with -std=c89
 *
 * 38   9/9/08 10:04a erickson
 * PR45940: use NEXUS_Rave_GetCdbValidPointer_isr to determine if
 *  fifoEmpty should be fired
 *
 * 37   9/2/08 1:51p erickson
 * PR45940: added NEXUS_VideoDecoderSettings.fifoEmpty and
 *  NEXUS_VideoDecoderStatus.tsm
 *
 * 36   8/21/08 3:15p erickson
 * PR45959: flatten still_ext and trick_ext into normal code
 *
 * 35   8/20/08 1:21p erickson
 * PR30571: increased Qp max to 400
 *
 * 34   8/14/08 5:25p katrep
 * PR45674: Fix compiiler warning in kernel mode non debug builds
 *
 * 33   7/28/08 11:04a jtna
 * PR45093: Move ChannelChangeReport() from public API to private
 *
 * 32   7/25/08 4:13p jtna
 * PR45093: defer channel change report using event
 *
 * 31   7/14/08 12:14p erickson
 * PR42739: added playbackOverride option for IPTV. fixed
 *  additionalPtsOffset logic.
 *
 * 30   7/11/08 5:08p jtna
 * PR42924: Add channel change measurements functionality
 *
 * 29   6/13/08 7:13p agin
 * PR43579: Fix compile error for when NEXUS_VIDEO_DECODER_TRICK is not
 *  enabled.
 *
 * 28   5/20/08 5:51p jrubio
 * PR42353: add 7335/7325 support , move to new vdc
 *
 * 27   5/20/08 3:42p erickson
 * PR42128: added dataReadyCallback to ExtendedSettings
 *
 * 26   5/16/08 11:04a erickson
 * PR42751: centralize logic for calling BXVD_SetVideoDisplayMode so that
 *  all aspects can be considered in one spot
 *
 * 25   4/28/08 4:10p erickson
 * PR41533: unify EIA-608 and EIA-708 closed caption data capture in
 *  NEXUS_VideoInput_ReadClosedCaption
 *
 * 24   4/21/08 1:48p erickson
 * PR41533: add WRN
 *
 * 23   4/17/08 4:38p erickson
 * PR41533: rework NEXUS_VideoDecoder_GetUserDataBuffer to use
 *  NEXUS_UserDataHeader before payload
 *
 * 22   4/7/08 10:16a erickson
 * PR37893: convert to XPT PI sw rave
 *
 * 21   3/31/08 1:41p erickson
 * PR41077: added NEXUS_ASSERT_MODULE to _priv function
 *
 * 20   3/27/08 5:52p bandrews
 * PR40090: guard calling of astm tsm pass / fail callbacks with tsm
 *  disabled / enabled check
 *
 * 19   3/26/08 9:46p bandrews
 * PR40090: stc channel must be passed to astm before video is started
 *
 * 18   3/26/08 3:38p vsilyaev
 * PR 40862: Added code to propagate display framerate to the decoder
 *
 * 17   3/25/08 5:25p bandrews
 * PR40090: Added compile flag for ASTM
 *
 * 16   3/24/08 10:52p bandrews
 * PR40090: astm private settings
 *
 * 15   3/24/08 9:21p bandrews
 * PR40090: astm private settings
 *
 * 14   3/24/08 12:01p erickson
 * PR39000: added NEXUS_VideoDecoder_FlushUserdata. fixed userdata memcpy.
 *
 * 13   3/11/08 2:11p erickson
 * PR40222: add ptsOffset
 *
 * 12   3/5/08 2:40p erickson
 * PR40307: fix VideoDecoder XVD watchdog. check rave contexts before
 *  using.
 *
 * 11   2/28/08 8:54p bandrews
 * PR37951: Synclib must use stream progressive, not frame progressive.
 *  Master frame rate status needs to be updated before handing to sync.
 *
 * 10   2/28/08 12:02p erickson
 * PR38888: remove 2 CASSERT's for 7401. minor status feature.
 *
 * 9   2/27/08 3:33p erickson
 * PR38888: complete VideoDecoder status
 *
 * 8   2/26/08 11:40p erickson
 * PR38888: complete NEXUS_VideoDecoderStreamInformation
 *
 * 7   2/25/08 3:00p erickson
 * PR39786: fix boolean logic
 *
 * 6   2/1/08 3:18p erickson
 * PR38679: rework display connection
 *
 * 5   1/31/08 9:44a erickson
 * PR36808: added private SyncChannel connection
 *
 * 4   1/22/08 5:07p erickson
 * PR38888: added NEXUS_VideoDecoder_GetStreamInformation
 *
 * 3   1/22/08 4:01p erickson
 * PR38888: fix compilation for 740x
 *
 * 2   1/22/08 3:46p erickson
 * PR38888: added streamCallback for sequence header notification
 *
 * 1   1/18/08 2:20p jgarrett
 * PR 38808: Merging to main branch
 *
 * Nexus_Devel/15   11/30/07 1:18p erickson
 * PR34925: rename to nexus_stc_channel.h
 *
 * Nexus_Devel/14   11/8/07 9:11a erickson
 * PR36802: added mosaic mode
 *
 * Nexus_Devel/13   11/1/07 9:06a erickson
 * PR36633: update base enums
 *
 * Nexus_Devel/12   10/18/07 11:01a erickson
 * PR36152: added 7405 pip
 *
 * Nexus_Devel/11   10/17/07 2:05p erickson
 * PR34926: rework NEXUS_VideoDecoderHandle to internally point to an XVD
 * channel, not device
 *
 * Nexus_Devel/10   10/9/07 3:48p erickson
 * PR34926: change Params to StartSettings
 *
 * Nexus_Devel/9   10/8/07 11:56a erickson
 * PR34926: reworked VBI interface
 *
 * Nexus_Devel/8   10/4/07 3:44p erickson
 * PR34926: NEXUS_VideoDecoder_GetSourceId_priv
 *
 * Nexus_Devel/7   10/4/07 3:08p erickson
 * PR34926: remove video descriptor
 *
 * Nexus_Devel/6   10/3/07 4:10p erickson
 * PR34925: added 32 bit size field to userdata byte stream
 *
 * Nexus_Devel/5   10/3/07 2:50p erickson
 * PR34926: fix sourceChanged callback
 *
 * Nexus_Devel/4   10/3/07 1:06p erickson
 * PR34926: impl userdata
 *
 * Nexus_Devel/3   10/1/07 1:45p erickson
 * PR34925: make Rave a private API
 *
 * Nexus_Devel/2   9/28/07 11:32a erickson
 * PR34926: rename videodecoder to video_decoder
 *
 * Nexus_Devel/1   9/26/07 11:01a erickson
 * PR34926: refactor implementation into _priv source file
 *
 *************************************************************************/
#include "nexus_video_decoder_module.h"
#include "priv/nexus_core.h"
#include "bvbilib_dccparse.h"

BDBG_MODULE(nexus_video_decoder_priv);

BTRC_MODULE_DECLARE(ChnChange_DecodeStopVideo);
BTRC_MODULE_DECLARE(ChnChange_Tune);
BTRC_MODULE_DECLARE(ChnChange_TuneLock);
BTRC_MODULE_DECLARE(ChnChange_DecodeStartVideo);
BTRC_MODULE_DECLARE(ChnChange_DecodeStartVideoXVD);
BTRC_MODULE_DECLARE(ChnChange_DecodeFirstVideo);
BTRC_MODULE_DECLARE(ChnChange_DecodeFirstvPTSPassed);
BTRC_MODULE_DECLARE(ChnChange_SyncUnmuteVideo);
BTRC_MODULE_DECLARE(ChnChange_Total_Video);

#if B_HAS_TRC
static const BTRC_Module *video_btrc_modules[] = {
    BTRC_MODULE_HANDLE(ChnChange_DecodeStopVideo),
    BTRC_MODULE_HANDLE(ChnChange_Tune),
    BTRC_MODULE_HANDLE(ChnChange_TuneLock),
    BTRC_MODULE_HANDLE(ChnChange_DecodeStartVideo),
    BTRC_MODULE_HANDLE(ChnChange_DecodeStartVideoXVD),
    BTRC_MODULE_HANDLE(ChnChange_DecodeFirstVideo),
    BTRC_MODULE_HANDLE(ChnChange_DecodeFirstvPTSPassed),
    BTRC_MODULE_HANDLE(ChnChange_SyncUnmuteVideo),
    BTRC_MODULE_HANDLE(ChnChange_Total_Video)
};

#define VIDEO_BTRC_N_MODULES ((sizeof(video_btrc_modules)/sizeof(*video_btrc_modules)))
#endif

void NEXUS_VideoDecoder_P_WatchdogHandler(void *data)
{
    struct NEXUS_VideoDecoderDevice *vDevice = (struct NEXUS_VideoDecoderDevice *)data;
    int i;

    vDevice->numWatchdogs++;

    /* when this is called, the xvd core is reset. now we need to disable and flush each rave context,
    then let xvd process the watchdog (which will bring raptor back up to its previous state),
    then enable rave and let the data flow. */
    BDBG_ERR(("BXVD_ProcessWatchdog being called!!!"));
    LOCK_TRANSPORT();
    for (i=0;i<NEXUS_NUM_XVD_CHANNELS;i++) {
        if (vDevice->channel[i].started) {
            NEXUS_Rave_Disable_priv(vDevice->channel[i].rave);
            NEXUS_Rave_Flush_priv(vDevice->channel[i].rave);
        }
    }
    UNLOCK_TRANSPORT();

    BXVD_ProcessWatchdog(vDevice->xvd);

    LOCK_TRANSPORT();
    for (i=0;i<NEXUS_NUM_XVD_CHANNELS;i++) {
        if (vDevice->channel[i].started) {
            NEXUS_Rave_Enable_priv(vDevice->channel[i].rave);
        }
    }
    UNLOCK_TRANSPORT();

#if NEXUS_HAS_ASTM
    for (i = 0; i < NEXUS_NUM_XVD_CHANNELS; i++)
    {
        if (vDevice->channel[i].started && vDevice->channel[i].astm.settings.enableAstm)
        {
            NEXUS_Callback astm_watchdog_isr = vDevice->channel[i].astm.settings.watchdog_isr;
            BDBG_MSG(("Video channel %p is notifying ASTM of its watchdog recovery", &vDevice->channel[i]));
            if (astm_watchdog_isr)
            {
                BKNI_EnterCriticalSection();
                astm_watchdog_isr(vDevice->channel[i].astm.settings.callbackContext, 0);
                BKNI_LeaveCriticalSection();
            }
        }
    }
#endif
}

void NEXUS_VideoDecoder_P_Watchdog_isr(void *data, int not_used, void *not_used2)
{
    struct NEXUS_VideoDecoderDevice *vDevice = (struct NEXUS_VideoDecoderDevice *)data;
    BSTD_UNUSED(not_used);
    BSTD_UNUSED(not_used2);
    /* convert from isr to event */
    BKNI_SetEvent(vDevice->watchdog_event);
}

const BAVC_MFD_Picture *
NEXUS_VideoDecoder_P_DataReady_Generic_Prologue_isr(NEXUS_VideoDecoderHandle videoDecoder, const BAVC_MFD_Picture *pFieldData, BAVC_MFD_Picture *modifiedFieldData)
{
    bool source_changed = false;

    BDBG_OBJECT_ASSERT(videoDecoder, NEXUS_VideoDecoder);

    /* Verify that we've received a good picture from XVD.
    If bad data could be caused by a bad stream, we should not ASSERT. We need to survive that.
    Do not ASSERT if the enum is likely to grow and there is no inherent eMax. No one will know to modify this code and it could die unexpectedly.
    If bad data could only come from bad FW or bad SW, we should ASSERT and get it fixed. */
    BDBG_ASSERT(!pFieldData->hSurface); /* must be NULL for XVD case */
    BDBG_ASSERT(pFieldData->eFrameRateCode < BAVC_FrameRateCode_eMax); /* has an eMax */
    BDBG_ASSERT(pFieldData->eInterruptPolarity <= BAVC_Polarity_eFrame); /* unlikely to expand */


    if (videoDecoder->dataReadyCount == 0 && pFieldData->bMute) {
        /* the first muted frames after a start are not decoded pictures. data is not ready.
        therefore, we should not count them, nor report the BAVC_MFD_Picture data as status, nor compare with previous BAVC_MFD_Picture data.
        the DataReady_isr still needs to go to Display, but it's essentially a vsync interrupt with no data. */
    }
    else {
        videoDecoder->dataReadyCount++;

        /* before storing BAVC_MVD_Field, check for significant changes, but don't set event until end of function */
        if (pFieldData->eAspectRatio != videoDecoder->last_field.eAspectRatio ||
            pFieldData->eFrameRateCode != videoDecoder->last_field.eFrameRateCode ||
            pFieldData->bStreamProgressive != videoDecoder->last_field.bStreamProgressive ||
            pFieldData->ulSourceHorizontalSize != videoDecoder->last_field.ulSourceHorizontalSize ||
            pFieldData->ulSourceVerticalSize != videoDecoder->last_field.ulSourceVerticalSize)
        {
            /* TODO: determine source_changed for each mosaic */
            source_changed = true;
        }

        videoDecoder->last_field = *pFieldData;
        videoDecoder->last_field_new = true;

    }

    /* modify values from XVD if necessary */
    BDBG_ASSERT(pFieldData != modifiedFieldData); /* logic below assumes they start different */
    if (pFieldData->eAspectRatio == BFMT_AspectRatio_eUnknown && videoDecoder->startSettings.aspectRatio != NEXUS_AspectRatio_eUnknown) {
        if (pFieldData != modifiedFieldData) {
            *modifiedFieldData = *pFieldData;
            pFieldData = modifiedFieldData;
        }
        modifiedFieldData->eAspectRatio = NEXUS_P_AspectRatio_ToMagnum(videoDecoder->startSettings.aspectRatio);
        modifiedFieldData->uiSampleAspectRatioX = videoDecoder->startSettings.sampleAspectRatio.x;
        modifiedFieldData->uiSampleAspectRatioY = videoDecoder->startSettings.sampleAspectRatio.y;
    }
    if (pFieldData->eTransferCharacteristics == BAVC_TransferCharacteristics_eUnknown) {
        if (pFieldData != modifiedFieldData) {
            *modifiedFieldData = *pFieldData;
            pFieldData = modifiedFieldData;
        }
        /* BDBG_CASSERT is in NEXUS_VideoDecoder_P_PictureParams_isr */
        modifiedFieldData->eTransferCharacteristics = videoDecoder->startSettings.defaultTransferCharacteristics;
    }
    if (pFieldData->eColorPrimaries == BAVC_ColorPrimaries_eUnknown) {
        if (pFieldData != modifiedFieldData) {
            *modifiedFieldData = *pFieldData;
            pFieldData = modifiedFieldData;
        }
        /* BDBG_CASSERT is in NEXUS_VideoDecoder_P_PictureParams_isr */
        modifiedFieldData->eColorPrimaries = videoDecoder->startSettings.defaultColorPrimaries;
    }
    if (pFieldData->eMatrixCoefficients == BAVC_MatrixCoefficients_eUnknown) {
        if (pFieldData != modifiedFieldData) {
            *modifiedFieldData = *pFieldData;
            pFieldData = modifiedFieldData;
        }
        /* BDBG_CASSERT is in NEXUS_VideoDecoder_P_PictureParams_isr */
        modifiedFieldData->eMatrixCoefficients = videoDecoder->startSettings.defaultMatrixCoefficients;
    }

#if !BDBG_NO_MSG
    {
        char arString[32];
        static const char *g_arStr[BFMT_AspectRatio_eSAR+1] = { "?", "SqPix", "4:3", "16:9", "2.21:1", "15:9", "SAR" };
        static const char *g_frameRateStr[BAVC_FrameRateCode_eMax+1] = { "?", "23.976", "24", "25", "29.97", "30", "50", "59.94", "60", "14.985", "7.493", "invalid" };

        if (pFieldData->eAspectRatio > BFMT_AspectRatio_eSAR) {
            BKNI_Snprintf(arString, 32, "<bad a/r>");
        }
        else if (pFieldData->eAspectRatio == BFMT_AspectRatio_eSAR) {
            BKNI_Snprintf(arString, 32, "%s%d:%d", g_arStr[pFieldData->eAspectRatio], pFieldData->uiSampleAspectRatioX, pFieldData->uiSampleAspectRatioY);
        }
        else {
            BKNI_Snprintf(arString, 32, "%s", g_arStr[pFieldData->eAspectRatio]);
        }

        BDBG_MSG(("ch[%d] %p %dx%d%c %dx%d %shz %s%s%s",
            videoDecoder->mfdIndex,
            pFieldData->pLuminanceFrameBufferAddress, /* use to check for unique buffers */
            pFieldData->ulSourceHorizontalSize, pFieldData->ulSourceVerticalSize,
            pFieldData->eSourcePolarity==BAVC_Polarity_eTopField?'t':pFieldData->eSourcePolarity==BAVC_Polarity_eBotField?'b':'p',
            pFieldData->ulDisplayHorizontalSize, pFieldData->ulDisplayVerticalSize,
            g_frameRateStr[pFieldData->eFrameRateCode],
            arString,
            pFieldData->bMute?" muted":"",
            pFieldData->bPictureRepeatFlag?" repeat":""));
    }
#endif

    if (pFieldData->ulAdjQp > 400) {
        BDBG_WRN(("Too large ulAdjQp: %lu", pFieldData->ulAdjQp));
    }

    if (source_changed) {
        NEXUS_IsrCallback_Fire_isr(videoDecoder->sourceChangedCallback);
    }

    return pFieldData;
}

static void
NEXUS_VideoDecoder_P_UpdateFifoEmpty_isr(NEXUS_VideoDecoderHandle videoDecoder)
{
    unsigned cdbValidPointer, cdbReadPointer;

    NEXUS_Rave_GetCdbPointers_isr(videoDecoder->rave, &cdbValidPointer, &cdbReadPointer);
    
    if (cdbValidPointer == videoDecoder->fifoEmpty.lastCdbValidPointer && videoDecoder->pictureDeliveryCount == 0) {
        if (videoDecoder->fifoEmpty.count < 20) {
            videoDecoder->fifoEmpty.count++;
        }
        else if (videoDecoder->fifoEmpty.count == 20) {
            /* fire it once when we reach 20 */
            NEXUS_IsrCallback_Fire_isr(videoDecoder->fifoEmpty.callback);
            videoDecoder->fifoEmpty.count++; /* set to 21 and leave it there */
            /* we can't just keep on incrementing. a box could be left in paused state for days and eventually the count would wrap. */
        }
    }
    else {
        videoDecoder->fifoEmpty.count = 0;
    }
    videoDecoder->fifoEmpty.lastCdbValidPointer = cdbValidPointer;
    videoDecoder->fifoEmpty.lastCdbReadPointer = cdbReadPointer;
}
            
void
NEXUS_VideoDecoder_P_DataReady_Generic_Epilogue_isr(NEXUS_VideoDecoderHandle videoDecoder, const BAVC_MFD_Picture *pFieldData, unsigned pictureDeliveryCount)
{
    if (videoDecoder->displayConnection.dataReadyCallback_isr) {
        (*videoDecoder->displayConnection.dataReadyCallback_isr)(videoDecoder->displayConnection.callbackContext, pFieldData);
    }

    videoDecoder->pictureDeliveryCount = pictureDeliveryCount;
    
    NEXUS_VideoDecoder_P_UpdateFifoEmpty_isr(videoDecoder);

    NEXUS_IsrCallback_Fire_isr(videoDecoder->dataReadyCallback);
    return;
}

void
NEXUS_VideoDecoder_P_DataReady_isr(void *data, int unused, void *field)
{
    NEXUS_VideoDecoderHandle videoDecoder = (NEXUS_VideoDecoderHandle)data;
    const BAVC_MFD_Picture *pFieldData = (BAVC_MFD_Picture*)field;
    BAVC_MFD_Picture modifiedFieldData;
    BXVD_ChannelStatus xvdChannelStatus;

    BDBG_OBJECT_ASSERT(videoDecoder, NEXUS_VideoDecoder);
    BSTD_UNUSED(unused);
    
    BXVD_GetChannelStatus_isr(videoDecoder->dec, &xvdChannelStatus);
    /* BDBG_ERR(("%u %u %u", pFieldData->eMatrixCoefficients, pFieldData->eColorPrimaries, pFieldData->eTransferCharacteristics)); */

    pFieldData = NEXUS_VideoDecoder_P_DataReady_Generic_Prologue_isr(videoDecoder, pFieldData, &modifiedFieldData);

    if (videoDecoder->dataReadyCount == 0 && pFieldData->bMute) {
        /* the first muted frames after a start are not decoded pictures. data is not ready.
        therefore, we should not count them, nor report the BAVC_MFD_Picture data as status, nor compare with previous BAVC_MFD_Picture data.
        the DataReady_isr still needs to go to Display, but it's essentially a vsync interrupt with no data. */
    }
    else {
#if B_HAS_TRC
        /* here we are unmuted and we have valid data ready, so stop ch chg meas. */
        if (!videoDecoder->validOutputPic) /* this flag is checked before update, so we can catch the edge */
        {
            if (BTRC_MODULE_HANDLE(ChnChange_DecodeStopVideo)->stats[0].count) {
                BTRC_TRACE(ChnChange_Total_Video, STOP);
                BKNI_SetEvent(videoDecoder->channelChangeReportEvent);
            }
        }
#endif
        videoDecoder->validOutputPic = true;
        
        /* copy last_field for every mosaic for GetStatus */
        if (pFieldData->pNext) {
            BAVC_MFD_Picture *pNext = pFieldData->pNext;
            unsigned channelIndex = 0;
            /* the mosaic decode channel indices may not be sequential, but the linked list of
               mosaic pictures (BAVC_MFD_Picture->pNext) is always sequential. the logic below handles this */
            while (pNext && ++channelIndex < NEXUS_NUM_XVD_CHANNELS) {
                NEXUS_VideoDecoderHandle v = &videoDecoder->device->channel[channelIndex];
                if (v->dec && v->mosaicMode) {
                    v->last_field = *pNext;
                    v->last_field_new = true;
                    pNext = pNext->pNext;
                }
            }
        }
    }

#if NEXUS_VIDEO_DECODER_EXTENSION_PREPROCESS_PICTURE
    NEXUS_VideoDecoder_P_PreprocessPicture_isr(videoDecoder, pFieldData);
#endif
    NEXUS_VideoDecoder_P_DataReady_Generic_Epilogue_isr(videoDecoder, pFieldData, xvdChannelStatus.ulPictureDeliveryCount);
    return;
}

void
NEXUS_VideoDecoder_P_RequestStc_isr(void *data, int unused, void *pts_info)
{
    BERR_Code rc;
    BAVC_PTSInfo pts;
    NEXUS_VideoDecoderHandle videoDecoder = (NEXUS_VideoDecoderHandle)data;

    BSTD_UNUSED(unused);
    BDBG_OBJECT_ASSERT(videoDecoder, NEXUS_VideoDecoder);
    BTRC_TRACE(ChnChange_DecodeFirstVideo, STOP);

    /* if force vsync, no first vPTS passed will occur */
    if (!NEXUS_GetEnv("force_vsync"))
    {
        BTRC_TRACE(ChnChange_DecodeFirstvPTSPassed, START);
    }

    pts.ui32CurrentPTS = *(uint32_t*)pts_info;
    pts.ePTSType = BAVC_PTSType_eCoded;
    BDBG_MSG(("video:%d Request STC %#x", (unsigned)videoDecoder->mfdIndex, pts.ui32CurrentPTS));

    if (videoDecoder->trickState.tsmEnabled == NEXUS_TsmMode_eSimulated) {
        (void)NEXUS_VideoDecoder_P_SetSimulatedStc_isr(videoDecoder, pts.ui32CurrentPTS);
    }
    else {
        if (videoDecoder->startSettings.stcChannel) {
           rc = NEXUS_StcChannel_RequestStc_isr(videoDecoder->startSettings.stcChannel, NEXUS_StcChannelDecoderType_eVideo0, &pts);
           if (rc) {rc=BERR_TRACE(rc);} /* keep going */
        }
    }

#if NEXUS_HAS_ASTM
    if (videoDecoder->astm.settings.enableAstm && videoDecoder->astm.settings.enableTsm)
    {
        videoDecoder->astm.status.pts = pts.ui32CurrentPTS;

        if (videoDecoder->astm.settings.firstPts_isr)
        {
            videoDecoder->astm.settings.firstPts_isr(videoDecoder->astm.settings.callbackContext, 0);
        }
    }
#endif

    NEXUS_IsrCallback_Fire_isr(videoDecoder->firstPtsCallback);
    NEXUS_IsrCallback_Fire_isr(videoDecoder->playback.firstPtsCallback);
}

/* SW7405-5118 bandrews - XDM has one less enum than AVC, so
invalid maps differently */
#define NEXUS_VIDEO_DECODER_P_XVD_PTS_INFO_TO_AVC_PTS_INFO(pAvcPts, pXvdPts) \
do \
{ \
    switch ((pXvdPts)->ePTSType) \
    { \
        case BXDM_PictureProvider_PTSType_eCoded: \
            (pAvcPts)->ePTSType = BAVC_PTSType_eCoded; \
            break; \
        case BXDM_PictureProvider_PTSType_eInterpolatedFromValidPTS: \
            (pAvcPts)->ePTSType = BAVC_PTSType_eInterpolatedFromValidPTS; \
            break; \
        case BXDM_PictureProvider_PTSType_eInterpolatedFromInvalidPTS: \
        default: \
            (pAvcPts)->ePTSType = BAVC_PTSType_eInterpolatedFromInvalidPTS; \
            break; \
    } \
} \
while (0)

void
NEXUS_VideoDecoder_P_PtsError_isr(void *data, int unused, void *pts_info)
{
    BERR_Code rc;
    BAVC_PTSInfo pts;
    BXVD_PTSInfo * xvdPts = pts_info;
    NEXUS_VideoDecoderHandle videoDecoder = (NEXUS_VideoDecoderHandle)data;

    BSTD_UNUSED(unused);
    BDBG_OBJECT_ASSERT(videoDecoder, NEXUS_VideoDecoder);

    pts.ui32CurrentPTS = xvdPts->ui32RunningPTS;
    NEXUS_VIDEO_DECODER_P_XVD_PTS_INFO_TO_AVC_PTS_INFO(&pts, xvdPts);
    BDBG_MSG(("video:%d PTS error %#x", (unsigned)videoDecoder->mfdIndex, pts.ui32CurrentPTS));

    if (videoDecoder->trickState.tsmEnabled == NEXUS_TsmMode_eSimulated) {
        (void)NEXUS_VideoDecoder_P_SetSimulatedStc_isr(videoDecoder, pts.ui32CurrentPTS);
    }
    else {
        if (videoDecoder->trickState.stcTrickEnabled && videoDecoder->startSettings.stcChannel) {
            uint32_t stc;
            /* in STC trick mode, PTS might lag the STC because of decoder drop algorithm. don't reset STC in this case. */
            NEXUS_StcChannel_GetStc_isr(videoDecoder->startSettings.stcChannel, &stc);
            if (stc > pts.ui32CurrentPTS &&
                (NEXUS_IS_DSS_MODE(videoDecoder->transportType) ? (stc - pts.ui32CurrentPTS < 27000000 * 8) : (stc - pts.ui32CurrentPTS < 45000 * 8)) ) {
                return;
            }
        }

        if (videoDecoder->startSettings.stcChannel) {
            rc = NEXUS_StcChannel_PtsError_isr(videoDecoder->startSettings.stcChannel, NEXUS_StcChannelDecoderType_eVideo0, &pts);
            if (rc) {rc=BERR_TRACE(rc);} /* keep going */
        }
    }

#if NEXUS_HAS_ASTM
    if (videoDecoder->astm.settings.enableAstm && videoDecoder->astm.settings.enableTsm)
    {
        videoDecoder->astm.status.pts = pts.ui32CurrentPTS;

        if (videoDecoder->astm.settings.tsmFail_isr)
        {
            videoDecoder->astm.settings.tsmFail_isr(videoDecoder->astm.settings.callbackContext, 0);
        }
    }
#endif

    videoDecoder->pts_error_cnt++;

    NEXUS_IsrCallback_Fire_isr(videoDecoder->ptsErrorCallback);
}

#if NEXUS_HAS_ASTM
void
NEXUS_VideoDecoder_P_TsmPass_isr(void *data, int unused, void *pts_info)
{
    BAVC_PTSInfo pts;
    BXVD_PTSInfo * xvdPts = pts_info;
    NEXUS_VideoDecoderHandle videoDecoder = (NEXUS_VideoDecoderHandle)data;

    BSTD_UNUSED(unused);
    BDBG_OBJECT_ASSERT(videoDecoder, NEXUS_VideoDecoder);

    pts.ui32CurrentPTS = ((BXVD_PTSInfo *)pts_info)->ui32RunningPTS;
    NEXUS_VIDEO_DECODER_P_XVD_PTS_INFO_TO_AVC_PTS_INFO(&pts, xvdPts);
    BDBG_MSG(("video:%d TSM pass %#x", (unsigned)videoDecoder->mfdIndex, pts.ui32CurrentPTS));

    if (videoDecoder->astm.settings.enableAstm && !videoDecoder->astm.settings.enableTsm)
    {
        videoDecoder->astm.status.pts = pts.ui32CurrentPTS;

        if (videoDecoder->astm.settings.tsmPass_isr)
        {
            videoDecoder->astm.settings.tsmPass_isr(videoDecoder->astm.settings.callbackContext, 0);
        }
    }
}

void
NEXUS_VideoDecoder_P_TsmLog_isr(void *data, int unused, void *pts_info)
{
    BSTD_UNUSED(data);
    BSTD_UNUSED(unused);
    BSTD_UNUSED(pts_info);
    /* TODO: update astm tsm log */
}

#endif /* NEXUS_HAS_ASTM */

void
NEXUS_VideoDecoder_P_PtsStcOffset_isr(void *data, int unused, void *pOffset)
{
    NEXUS_VideoDecoderHandle videoDecoder = (NEXUS_VideoDecoderHandle)data;

    BSTD_UNUSED(unused);
    BDBG_OBJECT_ASSERT(videoDecoder, NEXUS_VideoDecoder);

    videoDecoder->ptsStcDifference = (int32_t)*(uint32_t*)pOffset; /* in PTS units */
#if NEXUS_HAS_SYNC_CHANNEL
    videoDecoder->sync.status.delay = videoDecoder->ptsStcDifference;
    videoDecoder->sync.status.delayValid = true;

    if (videoDecoder->sync.settings.delayCallback_isr) 
    {
        uint32_t offset = *(uint32_t*)pOffset;

        /* PR:48453 bandrews - convert to 45 KHz units for sync channel */
        switch (videoDecoder->transportType)
        {
            case NEXUS_TransportType_eDssEs:
            case NEXUS_TransportType_eDssPes:
                offset /= 600;
                break;
            default:
                break;
        }

        (*videoDecoder->sync.settings.delayCallback_isr)(videoDecoder->sync.settings.callbackContext, offset);
    }
#endif
}

void
NEXUS_VideoDecoder_P_DecodeError_isr(void *data, int unused, void *unused2)
{
    NEXUS_VideoDecoderHandle videoDecoder = (NEXUS_VideoDecoderHandle)data;
    BSTD_UNUSED(unused);
    BSTD_UNUSED(unused2);
    BDBG_OBJECT_ASSERT(videoDecoder, NEXUS_VideoDecoder);
    NEXUS_IsrCallback_Fire_isr(videoDecoder->decodeErrorCallback);
}

void
NEXUS_VideoDecoder_P_FnrtChunkDone_isr(void *data, int unused, void *unused2)
{
    NEXUS_VideoDecoderHandle videoDecoder = (NEXUS_VideoDecoderHandle)data;
    BSTD_UNUSED(unused);
    BSTD_UNUSED(unused2);
    BDBG_OBJECT_ASSERT(videoDecoder, NEXUS_VideoDecoder);
    NEXUS_IsrCallback_Fire_isr(videoDecoder->fnrtChunkDoneCallback);
}

void
NEXUS_VideoDecoder_P_PictureParams_isr(void *data, int unused, void *info_)
{
    NEXUS_VideoDecoderHandle videoDecoder = (NEXUS_VideoDecoderHandle)data;
    BXVD_PictureParameterInfo *info = (BXVD_PictureParameterInfo *)info_;
    NEXUS_VideoDecoderStreamInformation streamInfo;
    BXVD_ChannelStatus status;
    BERR_Code rc;
    BAVC_PTSInfo apts;
    BXVD_PTSInfo xpts;
    BXVD_DecodeSettings DecodeSettings;

    BSTD_UNUSED(unused);
    BDBG_OBJECT_ASSERT(videoDecoder, NEXUS_VideoDecoder);

    videoDecoder->pictureParameterInfo.pictureCoding = info->uiPictureCodingType;

    BKNI_Memset(&streamInfo, 0, sizeof(streamInfo));
    streamInfo.valid = true;
    streamInfo.sourceHorizontalSize = info->ulSourceHorizontalSize;
    streamInfo.sourceVerticalSize = info->ulSourceVerticalSize;
    streamInfo.codedSourceHorizontalSize = info->uiCodedSourceWidth;
    streamInfo.codedSourceVerticalSize = info->uiCodedSourceHeight;
    streamInfo.displayHorizontalSize = info->ulDisplayHorizontalSize;
    streamInfo.displayVerticalSize = info->ulDisplayVerticalSize;
    streamInfo.aspectRatio = NEXUS_P_AspectRatio_FromMagnum(info->eAspectRatio);
    streamInfo.sampleAspectRatioX = info->uiSampleAspectRatioX;
    streamInfo.sampleAspectRatioY = info->uiSampleAspectRatioY;

    /* pick 4 values to help assure Nexus and Magnum enums stay in sync */
    BDBG_CASSERT(NEXUS_MatrixCoefficients_eXvYCC_601 == (NEXUS_MatrixCoefficients)BAVC_MatrixCoefficients_eXvYCC_601);
    BDBG_CASSERT(NEXUS_TransferCharacteristics_eIec_61966_2_4 == (NEXUS_TransferCharacteristics)BAVC_TransferCharacteristics_eIec_61966_2_4);
    BDBG_CASSERT(NEXUS_ColorPrimaries_eGenericFilm == (NEXUS_ColorPrimaries)BAVC_ColorPrimaries_eGenericFilm);
    BDBG_CASSERT(NEXUS_VideoFrameRate_e60 == (NEXUS_VideoFrameRate)BAVC_FrameRateCode_e60);

    /* These enums are straight mappings from Magnum */
    streamInfo.frameRate = info->eFrameRateCode;
    streamInfo.colorPrimaries = info->eColorPrimaries;
    streamInfo.transferCharacteristics = info->eTransferCharacteristics;
    streamInfo.matrixCoefficients = info->eMatrixCoefficients;

    streamInfo.streamProgressive = info->bStreamProgressive; /* don't use the bStreamProgressive_7411 flavor. it could lock onto a false value. */
    streamInfo.frameProgressive = info->bFrameProgressive;
    streamInfo.horizontalPanScan = info->i32_HorizontalPanScan;
    streamInfo.verticalPanScan = info->i32_VerticalPanScan;
    streamInfo.lowDelayFlag = info->uiLowDelayFlag;
    streamInfo.fixedFrameRateFlag = info->uiFixedFrameRateFlag;
    if (info->bValidAfd && info->uiAfd != videoDecoder->userdata.status.afdValue) {
        videoDecoder->userdata.status.afdValue = info->uiAfd;
        NEXUS_IsrCallback_Fire_isr(videoDecoder->afdChangedCallback);
    }

    if (BKNI_Memcmp(&streamInfo, &videoDecoder->streamInfo, sizeof(streamInfo))) {
        videoDecoder->streamInfo = streamInfo;
        NEXUS_IsrCallback_Fire_isr(videoDecoder->streamChangedCallback);

#if NEXUS_HAS_SYNC_CHANNEL
        /* notify sync., but only if things have changed */
        if (videoDecoder->sync.status.height != streamInfo.sourceVerticalSize
            ||
            videoDecoder->sync.status.interlaced != !streamInfo.streamProgressive
            ||
            videoDecoder->sync.status.frameRate != info->eFrameRateCode)
        {
            videoDecoder->sync.status.height = streamInfo.sourceVerticalSize;
            videoDecoder->sync.status.interlaced = !streamInfo.streamProgressive;
            videoDecoder->sync.status.frameRate = info->eFrameRateCode;
            if (videoDecoder->sync.settings.formatCallback_isr) {
                (*videoDecoder->sync.settings.formatCallback_isr)(videoDecoder->sync.settings.callbackContext, 0);
            }
        }
#endif
    }

    /* 20100519 bandrews - to fix SW7325-706 */
    BXVD_GetChannelStatus_isr(videoDecoder->dec, &status);

    if (status.uiDecoderInputOverflow > videoDecoder->overflowCount)
    {
        videoDecoder->overflowCount = status.uiDecoderInputOverflow;

        /* will this pts be valid after overflow? prob not, but in live it
        doesn't matter and pb shouldn't overflow */
        BXVD_GetPTS_isr(videoDecoder->dec, &xpts);

        NEXUS_VIDEO_DECODER_P_XVD_PTS_INFO_TO_AVC_PTS_INFO(&apts, &xpts);

        apts.ui32CurrentPTS = xpts.ui32RunningPTS;

        BXVD_GetDecodeSettings(videoDecoder->dec,&DecodeSettings);
        if (videoDecoder->startSettings.stcChannel && !DecodeSettings.bPlayback) {
        BDBG_WRN(("video:%d Overflow -> Request STC %#x", (unsigned)videoDecoder->mfdIndex, apts.ui32CurrentPTS));
           rc = NEXUS_StcChannel_RequestStc_isr(videoDecoder->startSettings.stcChannel, NEXUS_StcChannelDecoderType_eVideo0, &apts);
           if (rc) {rc=BERR_TRACE(rc);} /* keep going */
        }
    }
}

void
NEXUS_VideoDecoder_P_FirstPtsPassed_isr(void *data, int unused, void *pts_info)
{
    NEXUS_VideoDecoderHandle videoDecoder = (NEXUS_VideoDecoderHandle)data;

    BSTD_UNUSED(unused);
    BSTD_UNUSED(pts_info);
    BDBG_OBJECT_ASSERT(videoDecoder, NEXUS_VideoDecoder);

    BTRC_TRACE(ChnChange_DecodeFirstvPTSPassed, STOP);

    /* if sync_disabled, or sync already unmuted, then no additional sync mute
    time will occur */
    if (!NEXUS_GetEnv("sync_disabled")
#if NEXUS_HAS_SYNC_CHANNEL
    && videoDecoder->sync.settings.mute
#endif
    )
    { 
        BTRC_TRACE(ChnChange_SyncUnmuteVideo, START);
    }

    videoDecoder->firstPtsPassed = true;
    NEXUS_IsrCallback_Fire_isr(videoDecoder->firstPtsPassedCallback);
    NEXUS_IsrCallback_Fire_isr(videoDecoder->playback.firstPtsPassedCallback);
}

void NEXUS_VideoDecoder_P_3DTVTimeout(void* context)
{
    NEXUS_VideoDecoderHandle videoDecoder = (NEXUS_VideoDecoderHandle)context;
    NEXUS_VideoDecoderStatus status;
    videoDecoder->s3DTVStatusTimer = NULL;

    NEXUS_VideoDecoder_GetStatus(videoDecoder, &status);
    if (videoDecoder->s3DTVStatusPts==status.pts) { /* if the PTS hasn't moved, then the decoder must have been starved. don't fire the callback */
        NEXUS_VideoDecoder_P_3DTVTimer(videoDecoder);
        return;
    }
    else {
        BKNI_Memset(&videoDecoder->s3DTVStatus.codecData, 0, sizeof(videoDecoder->s3DTVStatus.codecData));
        videoDecoder->s3DTVStatus.format = NEXUS_VideoDecoder3DTVFormat_eNone;
        
        BDBG_WRN(("3DTV message timeout"));

        if (videoDecoder->settings.customSourceOrientation==false) {
            BXVD_3DSetting xvd3dSetting;
            BXVD_Get3D(videoDecoder->dec, &xvd3dSetting);
            xvd3dSetting.bOverrideOrientation = false;
            xvd3dSetting.eOrientation = BXDM_PictureProvider_Orientation_e2D;
            BXVD_Set3D(videoDecoder->dec, &xvd3dSetting);
        }

        BKNI_EnterCriticalSection();
        NEXUS_IsrCallback_Fire_isr(videoDecoder->s3DTVChangedCallback);
        BKNI_LeaveCriticalSection();
    }
}

void NEXUS_VideoDecoder_P_3DTVTimer(void* context)
{
    NEXUS_VideoDecoderHandle videoDecoder = (NEXUS_VideoDecoderHandle)context;
    NEXUS_VideoDecoderStatus status;
    if (videoDecoder->s3DTVStatusTimer) {
        NEXUS_CancelTimer(videoDecoder->s3DTVStatusTimer);
        videoDecoder->s3DTVStatusTimer = NULL;
    }

    if (videoDecoder->extendedSettings.s3DTVStatusTimeout) { /* Timeout==0 means no timer */
        videoDecoder->s3DTVStatusTimer = NEXUS_ScheduleTimer(videoDecoder->extendedSettings.s3DTVStatusTimeout,
            NEXUS_VideoDecoder_P_3DTVTimeout, videoDecoder);
    }

    NEXUS_VideoDecoder_GetStatus(videoDecoder, &status);
    videoDecoder->s3DTVStatusPts = status.pts;
}


void
NEXUS_VideoDecoder_P_Jp3dSignal_isr(NEXUS_VideoDecoderHandle videoDecoder, uint16_t type)
{
    BKNI_SetEvent(videoDecoder->s3DTVStatusEvent); /* this calls NEXUS_VideoDecoder_P_3DTVTimer() */
    /*BDBG_WRN(("JP3D message"));*/

    /* has the type changed? */
    if (type != videoDecoder->s3DTVStatus.codecData.mpeg2.jp3d.stereoVideoFormatSignalingType) {
        BXVD_Orientation xvdFormat = BXVD_Orientation_e2D;
        BXVD_3DSetting xvdSetting;
        /* videoDecoder->s3DTVStatus is cleared in NEXUS_VideoDecoder_P_Start_Generic_Body() */
        videoDecoder->s3DTVStatus.codec = NEXUS_VideoCodec_eMpeg2;

        switch (type) {
            case 0x3:
                videoDecoder->s3DTVStatus.format = NEXUS_VideoDecoder3DTVFormat_eSideBySideHalf;
                xvdFormat = BXVD_Orientation_eLeftRight;
                break;
            case 0x4:
                videoDecoder->s3DTVStatus.format = NEXUS_VideoDecoder3DTVFormat_eTopAndBottomHalf;
                xvdFormat = BXVD_Orientation_eOverUnder;
                break;
            case 0x8: /* 2D */
                videoDecoder->s3DTVStatus.format = NEXUS_VideoDecoder3DTVFormat_eNone;
                break;
        }
        videoDecoder->s3DTVStatus.codecData.mpeg2.jp3d.stereoVideoFormatSignalingType = type;
        videoDecoder->s3DTVStatus.codecData.mpeg2.valid = true;

        if (videoDecoder->settings.customSourceOrientation==false) { /* if app specifies custom orientation, then honor that and don't override */
            /* for mpeg2, the 3d info parsing is done outside of XVD, so we need to inform XVD what the orientation is */
            BXVD_Get3D_isr(videoDecoder->dec, &xvdSetting);
            if (xvdSetting.eOrientation!=xvdFormat) {
                NEXUS_Error rc;
                xvdSetting.bOverrideOrientation = true;
                xvdSetting.eOrientation = xvdFormat;
                xvdSetting.bSetNextPointer = false;
                rc = BXVD_Set3D_isr(videoDecoder->dec, &xvdSetting);
                if (rc!=BERR_SUCCESS) {
                    rc = BERR_TRACE(rc);
                }
                videoDecoder->settings.sourceOrientation = (NEXUS_VideoDecoderSourceOrientation)xvdFormat;
            }
        }
        
        NEXUS_IsrCallback_Fire_isr(videoDecoder->s3DTVChangedCallback);
    }
}

void
NEXUS_VideoDecoder_P_PictureExtensionData_isr(void *data, int unused, void *pData)
{
    NEXUS_VideoDecoderHandle videoDecoder = (NEXUS_VideoDecoderHandle)data;
    BXDM_Picture_ExtensionInfo *extInfo = (BXDM_Picture_ExtensionInfo *)pData;
    BXDM_Picture_Extension_SEIFramePacking *seiMessage = NULL;
    unsigned newMsg = 0;
    unsigned i;

    BSTD_UNUSED(unused);
    BDBG_OBJECT_ASSERT(videoDecoder, NEXUS_VideoDecoder);
    BDBG_CASSERT(sizeof(videoDecoder->s3DTVStatus.codecData.avc.data) == sizeof(*extInfo->astExtensionData[0].data.stSEIFramePacking.pstSeiData));

    if (!videoDecoder->extendedSettings.s3DTVStatusEnabled) {
        return;
    }

    for (i=0; i<extInfo->uiCount; i++) {
        if (extInfo->astExtensionData[i].eType == BXDM_Picture_ExtensionType_eSEIMsg_FramePacking) {
            seiMessage = extInfo->astExtensionData[i].data.stSEIFramePacking.pstSeiData;
            /* SEI messages are valid for a duration defined by the repetition period.
               newMsg != 0 indicates that either the message has changed or the previous duration expired */
            newMsg = extInfo->astExtensionData[i].data.stSEIFramePacking.uiFlags & BXDM_PICTURE_SEIMSG_FRAMEPACK_NEW_MESSAGE_FLAG;
            break;
        }
    }

    if (seiMessage) {
        BKNI_SetEvent(videoDecoder->s3DTVStatusEvent); /* this calls NEXUS_VideoDecoder_P_3DTVTimer() */

#if 0
        BDBG_MSG(("new %u, flags %u, id %u, type %u, ciType %u, pos %u:%u:%u:%u, res %u, rep %u", 
            newMsg, seiMessage->uiFlags, seiMessage->uiFramePackingArrangementId, seiMessage->uiFramePackingArrangementType, seiMessage->uiContentInterpretationType,
            seiMessage->uiFrame0GridPositionX, seiMessage->uiFrame0GridPositionY, seiMessage->uiFrame1GridPositionX, seiMessage->uiFrame1GridPositionY,
            seiMessage->uiFramePackingArrangementReservedByte, seiMessage->uiFramePackingArrangementRepetitionPeriod));
#endif

        if (newMsg) {
            /* don't use BKNI_Memcmp */
            if (videoDecoder->s3DTVStatus.codecData.avc.data.flags == seiMessage->uiFlags &&
                videoDecoder->s3DTVStatus.codecData.avc.data.framePackingArrangementId == seiMessage->uiFramePackingArrangementId &&
                videoDecoder->s3DTVStatus.codecData.avc.data.framePackingArrangementType == seiMessage->uiFramePackingArrangementType &&
                videoDecoder->s3DTVStatus.codecData.avc.data.contentInterpretationType == seiMessage->uiContentInterpretationType &&
                videoDecoder->s3DTVStatus.codecData.avc.data.frame0GridPositionX == seiMessage->uiFrame0GridPositionX &&
                videoDecoder->s3DTVStatus.codecData.avc.data.frame0GridPositionY == seiMessage->uiFrame0GridPositionY &&
                videoDecoder->s3DTVStatus.codecData.avc.data.frame1GridPositionX == seiMessage->uiFrame1GridPositionX &&
                videoDecoder->s3DTVStatus.codecData.avc.data.frame1GridPositionY == seiMessage->uiFrame1GridPositionY &&
                videoDecoder->s3DTVStatus.codecData.avc.data.framePackingArrangementReservedByte == seiMessage->uiFramePackingArrangementReservedByte &&
                videoDecoder->s3DTVStatus.codecData.avc.data.framePackingArrangementRepetitionPeriod == seiMessage->uiFramePackingArrangementRepetitionPeriod)
            {
                return; /* same message */
            }
        }
        else {
            return;
        }

        /* videoDecoder->s3DTVStatus is cleared in NEXUS_VideoDecoder_P_Start_Generic_Body() */
        videoDecoder->s3DTVStatus.codec = NEXUS_VideoCodec_eH264;
        videoDecoder->s3DTVStatus.format = NEXUS_VideoDecoder3DTVFormat_eNone;
        switch (seiMessage->uiFramePackingArrangementType) {
            case 3:
                videoDecoder->s3DTVStatus.format = NEXUS_VideoDecoder3DTVFormat_eSideBySideHalf;
                break;
            case 4:
                videoDecoder->s3DTVStatus.format = NEXUS_VideoDecoder3DTVFormat_eTopAndBottomHalf;
                break;
        }
        BKNI_Memcpy(&videoDecoder->s3DTVStatus.codecData.avc.data, seiMessage, sizeof(*seiMessage));
        videoDecoder->s3DTVStatus.codecData.avc.valid = true;
        NEXUS_IsrCallback_Fire_isr(videoDecoder->s3DTVChangedCallback);
    }
}

BERR_Code NEXUS_VideoDecoder_P_GetPtsCallback_isr(void *pContext, BAVC_PTSInfo *pPTSInfo)
{
    NEXUS_VideoDecoderHandle videoDecoder = (NEXUS_VideoDecoderHandle)pContext;
    BXVD_PTSInfo ptsInfo;

    BDBG_OBJECT_ASSERT(videoDecoder, NEXUS_VideoDecoder);
    BXVD_GetPTS_isr(videoDecoder->dec, &ptsInfo);

    /* map data structures */
    pPTSInfo->ui32CurrentPTS = ptsInfo.ui32RunningPTS;
    NEXUS_VIDEO_DECODER_P_XVD_PTS_INFO_TO_AVC_PTS_INFO(pPTSInfo, &ptsInfo);
    return 0;
}

BERR_Code NEXUS_VideoDecoder_P_GetCdbLevelCallback_isr(void *pContext, unsigned *pCdbLevel)
{
    unsigned depth, size;
    NEXUS_VideoDecoderHandle videoDecoder = (NEXUS_VideoDecoderHandle)pContext;

    BDBG_OBJECT_ASSERT(videoDecoder, NEXUS_VideoDecoder);
    NEXUS_Rave_GetCdbBufferInfo_isr(videoDecoder->rave, &depth, &size);
    *pCdbLevel = depth;
    return 0;
}

BERR_Code NEXUS_VideoDecoder_P_StcValidCallback_isr(void *pContext)
{
    NEXUS_VideoDecoderHandle videoDecoder = (NEXUS_VideoDecoderHandle)pContext;
    BDBG_OBJECT_ASSERT(videoDecoder, NEXUS_VideoDecoder);
    return BXVD_SetSTCInvalidFlag_isr(videoDecoder->dec, false);
}

BERR_Code NEXUS_VideoDecoder_P_SetPcrOffset_isr(void *pContext, uint32_t pcrOffset)
{
    NEXUS_VideoDecoderHandle videoDecoder = (NEXUS_VideoDecoderHandle)pContext;
    BDBG_OBJECT_ASSERT(videoDecoder, NEXUS_VideoDecoder);
    return BXVD_SetSwPcrOffset_isr(videoDecoder->dec, pcrOffset);
}

BERR_Code NEXUS_VideoDecoder_P_GetPcrOffset_isr(void *pContext, uint32_t *pPcrOffset)
{
    NEXUS_VideoDecoderHandle videoDecoder = (NEXUS_VideoDecoderHandle)pContext;
    BDBG_OBJECT_ASSERT(videoDecoder, NEXUS_VideoDecoder);
    return BXVD_GetSwPcrOffset_isr(videoDecoder->dec, pPcrOffset);
}

/***********
* Private API
***********/

NEXUS_Error NEXUS_VideoDecoder_P_SetXvdDisplayInterrupt(NEXUS_VideoDecoderHandle videoDecoder, const NEXUS_VideoDecoderDisplayConnection *connection, BXVD_DisplayInterrupt interrupt)
{
    BERR_Code rc;

    BXVD_DeviceVdcInterruptSettings VDCDevIntrSettings;

    BXVD_GetVdcDeviceInterruptDefaultSettings(videoDecoder->device->xvd, &VDCDevIntrSettings);

    VDCDevIntrSettings.VDCIntId_Topfield = connection->top.intId;
    VDCDevIntrSettings.VDCIntId_Botfield = connection->bottom.intId;
    VDCDevIntrSettings.VDCIntId_Frame = connection->frame.intId;
    VDCDevIntrSettings.eDisplayInterrupt = interrupt;
    VDCDevIntrSettings.hAppXdmDih = connection->dataReadyCallback_isr?videoDecoder->device->hXdmDih[interrupt]:NULL;
    BDBG_MSG(("connect %d: ints %d, %d, %d", videoDecoder->mfdIndex,
        connection->top.intId, connection->bottom.intId, connection->frame.intId));

    rc = BXVD_RegisterVdcDeviceInterrupt(videoDecoder->device->xvd, &VDCDevIntrSettings);
    if (rc) {return BERR_TRACE(rc);}

    if (videoDecoder->linkedDecoder) {
        VDCDevIntrSettings.uiFlags |= BXVD_DeviceVdcIntrSettingsFlags_Linked;
        rc = BXVD_RegisterVdcDeviceInterrupt(videoDecoder->linkedDecoder->device->xvd, &VDCDevIntrSettings);
        if (rc) {return BERR_TRACE(rc);}
    }

    return NEXUS_SUCCESS;
}

/**
These functions are called by Display when connecting a NEXUS_VideoInput to a NEXUS_VideoWindow.
**/
NEXUS_Error NEXUS_VideoDecoder_SetDisplayConnection_priv_Avd(NEXUS_VideoDecoderHandle videoDecoder, const NEXUS_VideoDecoderDisplayConnection *connection)
{
    BERR_Code rc;   
    unsigned i;

    NEXUS_ASSERT_MODULE();
    BDBG_OBJECT_ASSERT(videoDecoder, NEXUS_VideoDecoder);

    videoDecoder->displayConnection = *connection;

    if (videoDecoder->dec && !connection->dataReadyCallback_isr) {
        if (!videoDecoder->settings.manualPowerState) {
            rc = NEXUS_VideoDecoder_SetPowerState(videoDecoder, false);
            if (rc) return BERR_TRACE(rc);
        }
    }
    else if (connection->dataReadyCallback_isr) {
        /* even in manualPowerState, it's ok to automatically power up here. */
        rc = NEXUS_VideoDecoder_SetPowerState(videoDecoder, true);
        if (rc) return BERR_TRACE(rc);
    }

    /* If any of the decoder channels is already running or 
       any of the channels are still conencted to a window
       then do not unregister VDC device interrupt */
    if(!connection->dataReadyCallback_isr) {
    for(i=0; i<NEXUS_NUM_XVD_CHANNELS; i++){
        if(videoDecoder->device->channel[i].started ||
           videoDecoder->device->channel[i].displayConnection.top.intId ||
           videoDecoder->device->channel[i].displayConnection.bottom.intId ||
           videoDecoder->device->channel[i].displayConnection.frame.intId)
        return 0;
    }
    }
        
    rc = NEXUS_VideoDecoder_P_SetXvdDisplayInterrupt(videoDecoder, connection, videoDecoder->xdmIndex + BXVD_DisplayInterrupt_eZero);
    if (rc!=BERR_SUCCESS) { return BERR_TRACE(rc); }    

    NEXUS_VideoDecoder_P_SetUserdata(videoDecoder);

    return 0;
}

void NEXUS_VideoDecoder_GetDisplayConnection_priv_Avd(NEXUS_VideoDecoderHandle videoDecoder, NEXUS_VideoDecoderDisplayConnection *pConnection)
{
    BDBG_OBJECT_ASSERT(videoDecoder, NEXUS_VideoDecoder);
    NEXUS_ASSERT_MODULE();
    *pConnection = videoDecoder->displayConnection;
    pConnection->mosaicIndex = videoDecoder - videoDecoder->device->channel; /* compute channel index */
    pConnection->parentIndex = videoDecoder->mfdIndex;
    pConnection->psfMode = (videoDecoder->settings.scanMode == NEXUS_VideoDecoderScanMode_ePsF);
}

void NEXUS_VideoDecoder_GetSourceId_priv_Avd(NEXUS_VideoDecoderHandle videoDecoder, BAVC_SourceId *pSource)
{
    BDBG_OBJECT_ASSERT(videoDecoder, NEXUS_VideoDecoder);
    NEXUS_ASSERT_MODULE();
    *pSource = BAVC_SourceId_eMpeg0 + videoDecoder->mfdIndex;
}

/**
This function is only called from the Display to get the Display heap.
If openSettings.heap is NULL, we should return NULL, then Display will get its own default heap.
**/
void NEXUS_VideoDecoder_GetHeap_priv_Common( NEXUS_VideoDecoderHandle videoDecoder, NEXUS_HeapHandle *pHeap )
{
    BDBG_OBJECT_ASSERT(videoDecoder, NEXUS_VideoDecoder);
    NEXUS_ASSERT_MODULE();
    /* TODO: openSettings.heap is used for both AVD and Display. We need to add another heap handle to support
    setting a customer AVD heap and a customer Display heap for the VideoDecoder.
    Therefore, in non-UMA mode, the user must use the default heap for VideoDecoder and Display. */
    *pHeap = videoDecoder->openSettings.openSettings.heap;
}

#if NEXUS_HAS_SYNC_CHANNEL
void NEXUS_VideoDecoder_GetSyncSettings_priv_Common(NEXUS_VideoDecoderHandle videoDecoder, NEXUS_VideoInputSyncSettings *pSyncSettings)
{
    BDBG_OBJECT_ASSERT(videoDecoder, NEXUS_VideoDecoder);
    NEXUS_ASSERT_MODULE();
    *pSyncSettings = videoDecoder->sync.settings;
}

NEXUS_Error NEXUS_VideoDecoder_SetSyncSettings_priv_Avd(NEXUS_VideoDecoderHandle videoDecoder, const NEXUS_VideoInputSyncSettings *pSyncSettings)
{
    BERR_Code rc;
    unsigned delayCallbackThreshold;
    bool oldMuteStatus;

    BDBG_OBJECT_ASSERT(videoDecoder, NEXUS_VideoDecoder);
    NEXUS_ASSERT_MODULE();

    oldMuteStatus = videoDecoder->sync.mute;
    videoDecoder->sync.settings = *pSyncSettings;

    delayCallbackThreshold = videoDecoder->sync.settings.delayCallbackThreshold;

    /* PR:48453 bandrews - convert 45 KHz to appropriate decoder units */
    switch (videoDecoder->transportType)
    {
        case NEXUS_TransportType_eDssEs:
        case NEXUS_TransportType_eDssPes:
            delayCallbackThreshold *= 600;
            break;
        default:
            break;
    }
    if (videoDecoder->dec) {
        rc = BXVD_SetPtsStcDiffThreshold(videoDecoder->dec, delayCallbackThreshold);
        if (rc) {return BERR_TRACE(rc);}
    }

    /* the following settings are shared with the user */

    rc = NEXUS_VideoDecoder_P_SetPtsOffset(videoDecoder);
    if (rc) {return BERR_TRACE(rc);}

    if (!videoDecoder->started)
    {
        /* if sync sets mute before video is started, it is to modify start behavior */
        videoDecoder->sync.startMuted = videoDecoder->sync.settings.mute;
    }

    videoDecoder->sync.mute = videoDecoder->sync.settings.mute;
    
    if ((oldMuteStatus) && (!videoDecoder->sync.mute)) 
    {
        /* apply unmute edge */
        
        rc = NEXUS_VideoDecoder_P_SetMute(videoDecoder);
        if (rc) {return BERR_TRACE(rc);}

#if B_HAS_TRC
        /* don't count sync unmute if it occurs before first vpts passed */
        if (videoDecoder->firstPtsPassed)
        {
            BTRC_TRACE(ChnChange_SyncUnmuteVideo, STOP);
        }
#endif
    }

    return 0;
}

NEXUS_Error NEXUS_VideoDecoder_GetSyncStatus_isr_Common(NEXUS_VideoDecoderHandle videoDecoder, NEXUS_VideoInputSyncStatus *pSyncStatus)
{
    BDBG_OBJECT_ASSERT(videoDecoder, NEXUS_VideoDecoder);
    *pSyncStatus = videoDecoder->sync.status;
    return 0;
}
#endif /* NEXUS_HAS_SYNC_CHANNEL */

NEXUS_Error NEXUS_VideoDecoder_P_SetPtsOffset(NEXUS_VideoDecoderHandle videoDecoder)
{
    unsigned syncPtsOffset;
    unsigned astmPtsOffset;

    syncPtsOffset = 0;
    astmPtsOffset = 0;

    BDBG_OBJECT_ASSERT(videoDecoder, NEXUS_VideoDecoder);
    if (!videoDecoder->dec) {
        return 0;
    }

    /* PR:48453 bandrews - convert 45 KHz to appropriate decoder units */
#if NEXUS_HAS_SYNC_CHANNEL
    syncPtsOffset = videoDecoder->sync.settings.delay;
#endif
#if NEXUS_HAS_ASTM
    astmPtsOffset = videoDecoder->astm.settings.ptsOffset;
#endif
    switch (videoDecoder->transportType)
    {
        case NEXUS_TransportType_eDssEs:
        case NEXUS_TransportType_eDssPes:
            syncPtsOffset *= 600;
            astmPtsOffset *= 600;
            break;
        default:
            break;
    }

    return BXVD_SetDisplayOffset(videoDecoder->dec, syncPtsOffset + astmPtsOffset +
        videoDecoder->settings.ptsOffset + videoDecoder->primerPtsOffset + videoDecoder->additionalPtsOffset);
}

NEXUS_Error NEXUS_VideoDecoder_P_SetFreeze(NEXUS_VideoDecoderHandle videoDecoder)
{
    BERR_Code rc = BERR_SUCCESS;

    BDBG_OBJECT_ASSERT(videoDecoder, NEXUS_VideoDecoder);
    if (!videoDecoder->dec) {
        return 0;
    }

    /* user freeze is highest precedence */
    if (videoDecoder->settings.freeze)
    {
        BDBG_MSG(("Freezing video per user request"));
        rc = BXVD_EnableVideoFreeze(videoDecoder->dec);
        if (rc) return BERR_TRACE(rc);
    }
/* 20100326 bandrews - freeze doesn't do normal TSM in the bg while displaying the frozen frame, so can't use it */
#if 0
    else if (videoDecoder->syncSettings.mute)
    {
        /* only sync freeze if ch chg mode says so */
        switch (videoDecoder->settings.channelChangeMode)
        {
            case NEXUS_VideoDecoder_ChannelChangeMode_eMuteUntilFirstPicture:
            case NEXUS_VideoDecoder_ChannelChangeMode_eHoldUntilFirstPicture:
                /* the above two modes are here because the user expects a freeze after the first
                picture is shown anyway (while TSM for the second picture matures).  XVD team says
                if both FREEZE and "first picture preview" are enabled, "first picture preview" will
                take precendence...and then freeze will occur on the first picture */
            case NEXUS_VideoDecoder_ChannelChangeMode_eHoldUntilTsmLock:
                BDBG_MSG(("Freezing video per sync request"));
                rc = BXVD_EnableVideoFreeze(videoDecoder->dec);
                if (rc!=BERR_SUCCESS) {rc = BERR_TRACE(rc);}
                break;
            default:
                break;
        }
    }
#endif
    else /* unfreeze */
    {
        BDBG_MSG(("Unfreezing video"));
        rc = BXVD_DisableVideoFreeze(videoDecoder->dec);
        if (rc!=BERR_SUCCESS) {rc = BERR_TRACE(rc);}
    }

    return rc;
}

NEXUS_Error NEXUS_VideoDecoder_P_SetMute(NEXUS_VideoDecoderHandle videoDecoder)
{
    BERR_Code rc = BERR_SUCCESS;

    BDBG_OBJECT_ASSERT(videoDecoder, NEXUS_VideoDecoder);
    if (!videoDecoder->dec) {
        return 0;
    }

    /* user mute is highest precedence */
    if (videoDecoder->settings.mute)
    {
        BDBG_MSG(("Muting video per user request"));
        rc = BXVD_EnableMute(videoDecoder->dec, videoDecoder->settings.mute);
        if (rc!=BERR_SUCCESS) {rc = BERR_TRACE(rc);}
    }
#if NEXUS_HAS_SYNC_CHANNEL
    else if (videoDecoder->sync.mute 
        /* if preroll is nonzero, we want to unmute now */
        && !(videoDecoder->started && videoDecoder->startSettings.prerollRate))
    {
        /* SW7420-2314: if decoder was already started, don't mute */
        if (!videoDecoder->started)
        {
            NEXUS_VideoDecoderTrickState normal;
            
            /* unmute if trick mode and sync mute activated */
            NEXUS_VideoDecoder_GetNormalPlay(&normal);
            if (BKNI_Memcmp(&normal, &videoDecoder->trickState, sizeof(NEXUS_VideoDecoderTrickState)))
            {
                /* this overrides mute control of sync */
                videoDecoder->sync.mute = false;
                rc = BXVD_EnableMute(videoDecoder->dec, false);
                if (rc!=BERR_SUCCESS) {rc = BERR_TRACE(rc);}
            }
            else
            {
                /* only sync mute if ch chg mode says so */
                switch (videoDecoder->settings.channelChangeMode)
                {
                    case NEXUS_VideoDecoder_ChannelChangeMode_eMute:
                        BDBG_MSG(("Muting video per sync request"));
                        rc = BXVD_EnableMute(videoDecoder->dec, true);
                        if (rc!=BERR_SUCCESS) {rc = BERR_TRACE(rc);}
                        break;
                    default:
                        break;
                }
            }
        }
    }
#endif
    else /* unmute */
    {
        BDBG_MSG(("Unmuting video"));
        rc = BXVD_EnableMute(videoDecoder->dec, false);
        if (rc!=BERR_SUCCESS) {rc = BERR_TRACE(rc);}
    }

    return rc;
}

NEXUS_Error NEXUS_VideoDecoder_P_SetTsm(NEXUS_VideoDecoderHandle videoDecoder)
{
    BXVD_ClockOverride clockOverride;
    BXVD_DisplayMode displayMode;
    bool tsm;
    NEXUS_Error rc;
    
    BDBG_OBJECT_ASSERT(videoDecoder, NEXUS_VideoDecoder);
    if (!videoDecoder->dec) {
        return 0;
    }

    /* Lots of things can turn off TSM */
    tsm = videoDecoder->trickState.tsmEnabled != NEXUS_TsmMode_eDisabled &&
          !NEXUS_GetEnv("force_vsync") &&
          videoDecoder->transportType != NEXUS_TransportType_eEs;
          
    /* HW TSM requires STC channel. SW TSM does not. */          
    if (videoDecoder->trickState.tsmEnabled == NEXUS_TsmMode_eEnabled) {
        tsm = tsm && videoDecoder->startSettings.stcChannel;
    }

#if NEXUS_HAS_ASTM
    /* ASTM can turn off TSM */
    if (videoDecoder->astm.settings.enableAstm)
    {
        BDBG_MSG(("ASTM is %s TSM for video channel %p", videoDecoder->astm.settings.enableTsm ? "enabling" : "disabling", videoDecoder));
        tsm = tsm && videoDecoder->astm.settings.enableTsm;
    }
#endif

#if NEXUS_CRC_CAPTURE
    tsm = false;
#endif

    (void)BXVD_GetClockOverride(videoDecoder->dec, &clockOverride);
    clockOverride.bLoadSwStc = false;

    if (tsm) {
        if (videoDecoder->trickState.tsmEnabled == NEXUS_TsmMode_eSimulated) {
            int rate;
            unsigned displayFrameRate; /* units of 1/100 second */
            
            displayMode = BXVD_DisplayMode_eTSMMode;
            clockOverride.bEnableClockOverride = true;
            
            /* SW STC TSM - compute per-vsync STC delta based on trick rate, source frame rate, TS type */
            displayFrameRate = videoDecoder->displayInformation.refreshRate; /* applied every vsync, so it must be field or frame. */
            rate = NEXUS_IS_DSS_MODE(videoDecoder->transportType) ? 27000000 : 45000;
            rate = rate * 100 / (displayFrameRate?displayFrameRate:3000);
            rate = rate * videoDecoder->trickState.rate / (int)NEXUS_NORMAL_DECODE_RATE;
            clockOverride.iStcDelta = rate;
            
            /* we only enable simulated STC and set rate here. the stc value will be loaded 
            in NEXUS_VideoDecoder_P_SetSimulatedStc_isr in response to requeststc/ptserror isr.
            we also reenable the request interrupt so we can that interrupt as fast as possible. */
            
            rc = BXVD_EnableInterrupt(videoDecoder->dec, BXVD_Interrupt_eRequestSTC, true);
            if (rc) return BERR_TRACE(rc);
        }
        else {
            /* HW STC TSM */
            displayMode = BXVD_DisplayMode_eTSMMode;
            clockOverride.bEnableClockOverride = false;
        }
    }
    else {
        displayMode = BXVD_DisplayMode_eVSYNCMode;
        clockOverride.bEnableClockOverride = false;
    }

    rc = BXVD_SetVideoDisplayMode(videoDecoder->dec, displayMode);
    if (rc) return BERR_TRACE(rc);

    BDBG_MSG(("BXVD_SetClockOverride %d %d %d %d", displayMode, clockOverride.bEnableClockOverride, videoDecoder->trickState.rate, clockOverride.iStcDelta));
    rc = BXVD_SetClockOverride(videoDecoder->dec, &clockOverride);
    if (rc) return BERR_TRACE(rc);
    
    videoDecoder->tsm = tsm;

    return 0;

}

NEXUS_Error NEXUS_VideoDecoder_P_SetUserdata(NEXUS_VideoDecoderHandle videoDecoder)
{
    if (!videoDecoder->userdata.handle) {
        return 0;
    }

    BKNI_EnterCriticalSection();
    videoDecoder->userdata.vbiDataCallback_isr = videoDecoder->displayConnection.vbiDataCallback_isr;
    videoDecoder->userdata.vbiVideoInput = &videoDecoder->input;
    BKNI_LeaveCriticalSection();

    BDBG_MSG(("NEXUS_VideoDecoder_P_SetUserdata %p: started=%d userDataEnabled=%d vbiDataCallback_isr=%p", videoDecoder, videoDecoder->started,
        videoDecoder->settings.userDataEnabled, videoDecoder->userdata.vbiDataCallback_isr));

    return BXVD_Userdata_Enable(videoDecoder->userdata.handle,
        videoDecoder->started &&
#if !(defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD))
        !videoDecoder->trickState.hostTrickModesEnabled && 
#endif
        (videoDecoder->settings.userDataEnabled || videoDecoder->extendedSettings.s3DTVStatusEnabled ||
         videoDecoder->userdata.vbiDataCallback_isr || videoDecoder->displayConnection.userDataCallback_isr));
}

void
NEXUS_VideoDecoder_P_ApplyDisplayInformation( NEXUS_VideoDecoderHandle videoDecoder)
{
    BERR_Code rc;
    bool b1001Slip = false;
    unsigned vsyncRate = videoDecoder->displayInformation.refreshRate;

    BDBG_OBJECT_ASSERT(videoDecoder, NEXUS_VideoDecoder);
    if (videoDecoder->dec) {
        uint32_t refreshRate;
        switch(videoDecoder->displayInformation.refreshRate) {
        case 2398:
        case 2397: /* real value is 23.976 */
            vsyncRate = 2400;
            b1001Slip = true;
            refreshRate = BXVD_MONITOR_REFRESH_RATE_23_976Hz; break;
        case 2400:
            vsyncRate = 2400;
            b1001Slip = false;
            refreshRate = BXVD_MONITOR_REFRESH_RATE_24Hz; break;
        case 2500:
            vsyncRate = 2500;
            b1001Slip = false;
            refreshRate = BXVD_MONITOR_REFRESH_RATE_25Hz; break;
        case 2997:
            vsyncRate = 3000;
            b1001Slip = true;
            refreshRate = BXVD_MONITOR_REFRESH_RATE_29_97Hz; break;
        case 3000:
            vsyncRate = 3000;
            b1001Slip = false;
            refreshRate = BXVD_MONITOR_REFRESH_RATE_30Hz; break;
        case 4800:
            vsyncRate = 4800;
            b1001Slip = false;
            refreshRate = BXVD_MONITOR_REFRESH_RATE_48Hz; break;
        case 5000:
            vsyncRate = 5000;
            b1001Slip = false;
            refreshRate = BXVD_MONITOR_REFRESH_RATE_50Hz; break;
        case 5994:
            vsyncRate = 6000;
            b1001Slip = true;
            refreshRate = BXVD_MONITOR_REFRESH_RATE_59_94Hz; break;
        default:
        case 6000:
            refreshRate = BXVD_MONITOR_REFRESH_RATE_60Hz; break;
        }
        rc = BXVD_SetMonitorRefreshRate(videoDecoder->dec, refreshRate);
        if (rc!=BERR_SUCCESS) {rc = BERR_TRACE(rc);}
    }
    if(videoDecoder->startSettings.stcChannel) {
        NEXUS_StcChannelNonRealtimeSettings stcNonRealtimeSettings;
        LOCK_TRANSPORT();
        NEXUS_StcChannel_GetDefaultNonRealtimeSettings_priv( &stcNonRealtimeSettings);
        stcNonRealtimeSettings.externalTrigger = 0;
        if(videoDecoder->startSettings.nonRealTime) {
            stcNonRealtimeSettings.triggerMode = NEXUS_StcChannelTriggerMode_eExternalTrig;
            stcNonRealtimeSettings.externalTrigger = videoDecoder->displayInformation.stgIndex;
            if(videoDecoder->displayInformation.refreshRate) {
                stcNonRealtimeSettings.stcIncrement = ((uint64_t)27000000 * (b1001Slip?1001:1000) / 10) / vsyncRate; /* convert from 1/100Hz to 27MHz ticks */
            } else {
                stcNonRealtimeSettings.stcIncrement = 0;
            }
        } else {
            stcNonRealtimeSettings.triggerMode = NEXUS_StcChannelTriggerMode_eTimebase;
        }
        rc = NEXUS_StcChannel_SetNonRealtimeConfig_priv(videoDecoder->startSettings.stcChannel, &stcNonRealtimeSettings);
        UNLOCK_TRANSPORT();
        if(rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc); /* keep going */}
    }
    
    if (videoDecoder->trickState.tsmEnabled == NEXUS_TsmMode_eSimulated) {
        (void)NEXUS_VideoDecoder_P_SetTsm(videoDecoder);
    }
}

void
NEXUS_VideoDecoder_UpdateDisplayInformation_priv_Avd( NEXUS_VideoDecoderHandle videoDecoder, const NEXUS_VideoDecoder_DisplayInformation *pDisplayInformation)
{
    NEXUS_ASSERT_MODULE();
    BDBG_OBJECT_ASSERT(videoDecoder, NEXUS_VideoDecoder);
    BDBG_MSG(("NEXUS_VideoDecoder_UpdateDisplayInformation_priv_Avd: %#lx  refreshRate:%u", (unsigned long)videoDecoder, pDisplayInformation->refreshRate));
    videoDecoder->displayInformation = *pDisplayInformation;
    NEXUS_VideoDecoder_P_ApplyDisplayInformation(videoDecoder);
}


void NEXUS_VideoDecoder_P_ChannelChangeReport(void *context)
{
#if B_HAS_TRC
#define REPORT_FREQUENCY 10

    const unsigned chn = 0;
    unsigned i = 0;
#if (defined B_PERF_BMIPS3300) || (defined B_PERF_BMIPS4380)
    const unsigned COUNTER = 3; /* Cycles */
    const unsigned CPU_CLOCK_RATE = 405; /* MHz */
#elif (defined B_PERF_LINUX)
    const unsigned COUNTER = 0; /* Time, us */
    const unsigned CPU_CLOCK_RATE = 1;
#else
    BSTD_UNUSED(context);
    BERR_TRACE(NEXUS_NOT_SUPPORTED);
    return;
#endif

    if (BTRC_MODULE_HANDLE(ChnChange_Total_Video)->stats[0].count % REPORT_FREQUENCY == 0) {
        if (NEXUS_GetEnv("force_vsync")) {BKNI_Printf("Channel change measurements with force_vsync=y\n");}
        if (NEXUS_GetEnv("sync_disabled")) {BKNI_Printf("Channel change measurements with sync_disabled=y\n");}
        BKNI_Printf("%-32s %6s %12s %12s %12s\n", "Name", "Cnt", "Avg(us)", "Max(us)", "Min(us)");

        for (i=0; i < VIDEO_BTRC_N_MODULES; i++) {
            const struct BTRC_P_Stats *stats = &video_btrc_modules[i]->stats[chn];

            if (stats->count) {
                const unsigned avg = (unsigned)(((((uint64_t)stats->total_hi.perf.data[COUNTER])<<((sizeof(unsigned))*8)) + stats->total.perf.data[COUNTER])/stats->count / CPU_CLOCK_RATE);
                const unsigned max = stats->max.perf.data[COUNTER] / CPU_CLOCK_RATE;
                const unsigned min = stats->min.perf.data[COUNTER] / CPU_CLOCK_RATE;
                BKNI_Printf("%-32s %6u %12u %12u %12u\n", video_btrc_modules[i]->name, stats->count, avg, max, min);
            }
        }
    }
#endif
    BSTD_UNUSED(context);
    return;
}

#if NEXUS_HAS_ASTM
void NEXUS_VideoDecoder_GetAstmSettings_priv_Common(NEXUS_VideoDecoderHandle videoDecoder, NEXUS_VideoDecoderAstmSettings * pAstmSettings)
{
    BDBG_OBJECT_ASSERT(videoDecoder, NEXUS_VideoDecoder);
    NEXUS_ASSERT_MODULE();

    *pAstmSettings = videoDecoder->astm.settings;
}

NEXUS_Error NEXUS_VideoDecoder_SetAstmSettings_priv_Avd(NEXUS_VideoDecoderHandle videoDecoder, const NEXUS_VideoDecoderAstmSettings *pAstmSettings)
{
    NEXUS_Error rc = NEXUS_SUCCESS;

    BDBG_OBJECT_ASSERT(videoDecoder, NEXUS_VideoDecoder);
    NEXUS_ASSERT_MODULE();

    /* copy settings as-is, this way ASTM will always get what it set */
    videoDecoder->astm.settings = *pAstmSettings;

    /* if ASTM is internally permitted, apply settings */
    rc = NEXUS_VideoDecoder_P_SetTsm(videoDecoder);

    rc = NEXUS_VideoDecoder_P_SetPtsOffset(videoDecoder);

    /* PR:49215 use HW PCR offset depends on playback flag */
    if (videoDecoder->dec)
    {
        rc = BXVD_SetHwPcrOffsetEnable(videoDecoder->dec, !videoDecoder->astm.settings.enablePlayback);
    }

    return rc;
}

NEXUS_Error NEXUS_VideoDecoder_GetAstmStatus_isr_Common(NEXUS_VideoDecoderHandle videoDecoder, NEXUS_VideoDecoderAstmStatus *pAstmStatus)
{
    BDBG_OBJECT_ASSERT(videoDecoder, NEXUS_VideoDecoder);

    *pAstmStatus = videoDecoder->astm.status;

    return NEXUS_SUCCESS;
}

#endif /* NEXUS_HAS_ASTM */

