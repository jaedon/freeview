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
 * $brcm_Workfile: nexus_video_decoder_module.h $
 * $brcm_Revision: 131 $
 * $brcm_Date: 10/10/12 4:24p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/video_decoder/src/nexus_video_decoder_module.h $
 * 
 * 131   10/10/12 4:24p bandrews
 * SW7425-4021: Fix watchdog to work even if data ready callback isn't
 *  coming
 * 
 * 130   8/29/12 4:20p bandrews
 * SW7425-3820: take PTS movement into account for FIFO watchdog; add WRN
 *  when flushing caused by FIFO WD; fix function name for epilogue
 * 
 * 129   8/22/12 8:54p bandrews
 * SW7420-2314: mute control now applies to a persistent startMute flag;
 *  unmute happens immediately
 * 
 * 128   7/27/12 7:02p hongtaoz
 * SW7425-3424: added FNRT support to video decoder;
 * 
 * FNRT_Demo/1   7/6/12 5:30p hongtaoz
 * SW7425-3424: add FNRT support to video decoder;
 * 
 * 127   6/19/12 12:44p vsilyaev
 * SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
 * 
 * 126   6/11/12 2:23p erickson
 * SW7425-3170: refactor VideoDecoderPrimer api
 * 
 * 125   6/8/12 2:18p erickson
 * SW7425-3170: promote NEXUS_VideoDecoderPrimerHandle to a tracked
 *  handle, refactor API with backward compatible macros
 * 
 * 124   6/8/12 10:52a vsilyaev
 * SW7425-3176: Added 'stub' implementation for Get/SetExtendedSettings
 * 
 * 123   6/1/12 4:18p vsilyaev
 * SW7420-2312: Added reference counting of the StcChannel
 * 
 * 122   5/14/12 7:42p bandrews
 * SW7425-2264: merge to main
 * 
 * SW7425-2264/1   5/4/12 9:50p bandrews
 * SW7425-2264: first stab at low latency TSM mode impl
 * 
 * 121   4/11/12 4:08p bandrews
 * SW7405-5612: merge to main
 * 
 * SW7405-5612/4   4/11/12 3:42p bandrews
 * SW7405-5612: merge from main
 * 
 * 120   3/20/12 1:20p erickson
 * SW7420-2280: protect UserDataReadComplete from bad calls. add flush to
 *  assist app recovery.
 * 
 * 119   3/15/12 9:21a erickson
 * SW7425-2607: use NEXUS_MAX_XVD_DEVICES
 * 
 * 118   3/14/12 12:34p erickson
 * SW7401-4517: remove old code
 * 
 * SW7405-5612/3   3/9/12 8:34p bandrews
 * SW7405-5612: merge from main
 * 
 * 117   3/6/12 8:53a erickson
 * SW7425-2404: add NEXUS_VideoDecoderTrickState.maxFrameRepeat
 * 
 * SW7405-5612/2   3/5/12 8:19p bandrews
 * SW7405-5612: merge from main
 * 
 * 116   3/2/12 4:56p bandrews
 * SW7425-2276: merge to main
 * 
 * 115   2/29/12 4:11p jtna
 * SW7425-2507: take XVD's default for SetVeryLateThreshold() in non-trick
 *  mode
 * 
 * SW7425-2276/1   3/1/12 7:06p bandrews
 * SW7425-2276: improve gap detection; use correct placement for BXVD_PVR_
 *  function call; ensure all counters are 64 bit
 * 
 * SW7405-5612/1   2/20/12 8:39p bandrews
 * SW7405-5612: initial implementation
 * 
 * 114   1/24/12 2:44p vsilyaev
 * SW7425-2258: Added Nexus video decoder extension to decode MJPEG
 * 
 * 113   1/16/12 11:23a erickson
 * SW7346-629: add NEXUS_VideoDecoder_GetCrcData
 * 
 * 112   12/14/11 4:24p jgarrett
 * SW7425-1962: Adding SetSettings for DSP decoder and correcting
 *  stop/start behavior
 * 
 * 111   12/12/11 11:24a erickson
 * SW7346-594: move UDP data off stack to avoid kernel mode stack overflow
 * 
 * 110   10/24/11 2:05p erickson
 * SW7425-1264: update NEXUS_TsmMode_eSimulated
 * 
 * 109   10/12/11 10:21a erickson
 * SW7425-1264: add NEXUS_TsmMode_eSimulated
 * 
 * 108   10/3/11 10:15a erickson
 * SW7425-1367: move pictureCoding to NEXUS_VideoDecoderStatus so we get
 *  fewer callbacks
 * 
 * 107   8/12/11 4:18p jtna
 * SW7425-1070: merge video primer ptsStcDiffCorrection feature
 * 
 * 106   7/21/11 4:14p gmohile
 * SW7125-1014 : Rework power management
 * 
 * 105   6/20/11 5:41p jgarrett
 * SW7425-409: Merging VP6 support to main branch
 * 
 * SW7425-409/1   6/6/11 7:16p jgarrett
 * SW7425-409: Adding DSP video decoder extensions for VDE
 * 
 * 104   6/3/11 12:16p erickson
 * SW7425-466: remove unused macro
 * 
 * 103   5/12/11 3:47p hongtaoz
 * SW7425-466: rework avd/mfd/decoder/xdm mapping; renamed mainIndex as
 *  mfdIndex;
 * 
 * SW7425-466/2   5/12/11 12:04p hongtaoz
 * SW7425-466: added the XDM/decoder mapping to be scalable;
 * 
 * SW7425-466/1   5/11/11 12:59p hongtaoz
 * SW7425-466: rework avd/mfd/decoder channel mapping;
 * 
 * 102   3/17/11 1:17p erickson
 * SW7422-333: rely on #define NEXUS_SVC_MVC_SUPPORT in
 *  nexus_platform_features.h
 *
 * 100   3/1/11 10:21a erickson
 * SW7335-1168: add NEXUS_VideoDecoder_DetachRaveContext
 *
 * 99   2/16/11 10:14a gmohile
 * SW7408-210 : Fix kernel mode build
 *
 * 98   1/4/11 10:10a erickson
 * SW7400-2842: remove NEXUS_VIDEO_DECODER_SINGLEDECODE
 *
 * 97   12/22/10 6:17p vsilyaev
 * SW7422-160: Use pictureDeliveryCount to curb fals positives on static
 *  CDB
 *
 * 96   12/13/10 10:34p jrubio
 * SW7344-9: add 7344/7346 to video decoder module
 *
 * 95   12/6/10 2:48p katrep
 * SW7231-4:add svc,mvc support for 7231
 *
 * 94   11/16/10 4:18p jtna
 * SW7405-4997: Add 3DTV status timer
 *
 * 93   11/1/10 5:23p vsilyaev
 * SW7422-65: Added pidChannel for enhancement video data
 *
 * 92   10/22/10 11:16a erickson
 * SW7420-1135: add NEXUS_VideoDecoderMosaicSettings for runtime changes
 *  of maxWidth/maxHeight for mosaic decodes
 *
 * 91   10/18/10 2:27p jtna
 * SW35230-1558: add B_PERF_LINUX support to channel change report
 *
 * 90   10/14/10 6:39p jtna
 * SW7405-4817: add JP3D MPEG2 3D userdata signaling
 *
 * 89   10/11/10 12:00p vsilyaev
 * SW7422-65: SW7468-295: Use generic Api for SetPlaybackSettings
 *
 * 88   9/29/10 3:30p vsilyaev
 * SW7422-65: User per AVD core heap sizes
 *
 * 87   9/29/10 3:18p jtna
 * SW7405-4723: remove unneeded primer profiling code
 *
 * 86   9/24/10 11:14a mward
 * SW7468-129: Fix build of  NEXUS_VIDEO_DECODER_SINGLEDECODE.
 *
 * 85   9/9/10 4:31p erickson
 * SW7468-129: pass open settings to NEXUS_VideoDecoder_P_GetRaveSettings
 *
 * 84   9/8/10 1:15p vsilyaev
 * SW7468-129: After merge fixes
 *
 * 83   9/8/10 12:25p vsilyaev
 * SW7468-129: Added video decoder on ZSP
 *
 * 82   8/16/10 2:57p jtna
 * SW7405-4702: NEXUS_VideoDecoder_CreateStripedSurface() returns NULL on
 *  repeat surfaces
 *
 * 81   8/10/10 1:12p erickson
 * SW3548-3022: switch to UDP (userdata parser) for userdata parsing
 *
 * 80   7/29/10 8:27a erickson
 * SW7405-4637: handle userdata ringbuffer wraps internally for easier app
 *  code
 *
 * 79   7/23/10 4:09p jhaberf
 * SW35230-627: use macro instead of chip list for video decode dual
 *  feeder support
 *
 * 78   7/7/10 11:46a mward
 * SW7400-2842: Add an option NEXUS_VIDEO_DECODER_SINGLEDECODE=y for XVD
 *  singledecode on 97400.
 *
 * 77   6/29/10 2:26p jtna
 * SW7405-3998: update NEXUS_VideoDecoder_Get3DTVStatus to return codec-
 *  generic 3DTV status
 *
 * 76   6/16/10 9:58a erickson
 * SW7405-4378: remove NEXUS_VideoDecoderStatus.coded.width/height using
 *  NEXUS_VideoDecoderStreamInformation, with additional logic for
 *  beginning-of-decode conditions
 *
 * 75   5/19/10 10:37p bandrews
 * SW7325-706: correct check in comment
 *
 * 74   5/19/10 10:17p bandrews
 * SW7325-706: on overflow, video dumps any pcr offsets it had, need to
 *  force a request stc in nexus
 *
 * 73   5/12/10 12:28p jtna
 * SW7405-3998: merge
 *
 * SW7405-3998/2   5/12/10 12:18p jtna
 * SW7405-3998: update SEI function/variable naming
 *
 * SW7405-3998/1   5/4/10 5:14p jtna
 * SW7405-3998: added SEI frame packing arrangement message callback
 *
 * 72   3/18/10 3:50p gmohile
 * SW7405-2993 : Merge XDM changes
 *
 * 71   3/10/10 2:34p erickson
 * SW7405-3965: clarify comments re: mainIndex
 *
 * 70   2/10/10 10:36a erickson
 * SW7405-3891: rename macro to B_USE_NEW_XVD
 *
 * 69   2/1/10 8:08p bandrews
 * SW7405-3774: add mute control support to sync channel
 *
 * 68   12/9/09 3:10p erickson
 * SW7405-3394: refactor mosaic mode API
 *
 * 67   11/6/09 5:27p erickson
 * SW7405-3334: refactor to remove display & mosaic logic from
 *  nexus_video_decoder_userdata.c code
 *
 * 66   11/6/09 4:30p erickson
 * SW7405-3334: implement NEXUS_VideoDecoder_SetMosaicVbiRouting
 *
 * 65   10/26/09 2:09p erickson
 * SW7405-3089: update display information for all mosaics. also, separate
 *  the private API from the internal implementation so we don't get
 *  excessive calls.
 *
 * 64   10/26/09 12:28p erickson
 * SW7405-3287: add support for sw-based pcr offset for mosaic playback
 *  TSM support
 *
 * 63   10/7/09 2:48p nickh
 * SW7420-331: Add 7410 support.  Explicitly define the number of XVD
 *  devices in the platform for both 7420 and 7410
 *
 * 62   9/30/09 6:48p vsilyaev
 * SW7405-2732: Added fragmented mode, for the decoder side it set using
 *  Reoorder and filtering I frames only
 *
 * 61   9/10/09 12:22p jtna
 * SW7405-3003: add video primer profiling
 *
 * 60   9/8/09 4:18p erickson
 * SW7420-283: add support for linked mosaic decoders
 *
 * 59   7/23/09 12:22p erickson
 * PR56967: added NEXUS_VideoDecoderSettings.decodeError callback
 *
 * 58   7/16/09 12:29p jtna
 * PR56696: add preprocess picture hook
 *
 * 57   7/15/09 1:56p erickson
 * PR56522: add nexus_video_decoder_primer.h
 *
 * 56   7/8/09 11:57a vsilyaev
 * PR 55989: Added EOF handling for the OTF PVR
 *
 * 55   6/2/09 12:15p erickson
 * PR55639: improve accuracy of VideoDecoder status and stream information
 *  by adjusting timing and field selection
 *
 * 54   5/20/09 5:17p erickson
 * PR55042: back out bad include
 *
 * 53   5/20/09 3:30p erickson
 * PR55042: refactor active format description support. add callback and 8
 *  bit status value.
 *
 * 52   5/20/09 9:21a erickson
 * PR54880: refactor RAVE configuration, prep work for video primer
 *
 * 51   5/15/09 2:06p erickson
 * PR55094: add NEXUS_VideoDecoderStatus.numWatchdogs
 *
 * 50   5/8/09 12:17p erickson
 * PR54844: mosaicMode must be tracked per channel, not per device.
 *
 * 49   4/1/09 12:54p erickson
 * PR53653: get default discard threshold from XVD instead of duplicating
 *  values in nexus
 *
 * 48   3/31/09 7:29p vsilyaev
 * PR 50887: OTFPVR support for the Nexus video decoder
 *
 * 47   3/30/09 12:52p erickson
 * PR53544: remove vbi dependency from video_decoder
 *
 * 46   3/26/09 10:06a nickh
 * PR53588: Add dual decode support for 7420
 *
 * PR51105/2   2/5/09 3:35p erickson
 * PR51105: update branch
 *
 * 44   1/29/09 5:27p erickson
 * PR51694: added NEXUS_VideoDecoderStatus.ptsStcDifference
 *
 * 43   1/27/09 9:56a erickson
 * PR51468: refactor to reduce exposed globals
 *
 * 42   1/26/09 10:55a erickson
 * PR51468: global variable naming convention
 *
 * 41   1/19/09 5:20p bandrews
 * PR51021: merge
 *
 * PR51021/2   1/16/09 4:13p bandrews
 * PR51021: Updated to separate permission from enable
 *
 * PR51021/1   1/16/09 12:18a bandrews
 * PR51021: First stab at cleaning up ASTM playback blockage
 *
 * 40   12/29/08 2:35p erickson
 * PR50749: fix mosaic mode. multiple OpenChannel's required when one
 *  VideoDecoder is connected to a VideoWindow.
 *
 * 39   12/12/08 4:20p jtna
 * PR49924: add support for
 *  NEXUS_VideoDecoderSettings.preferredUserDataFormat
 *
 * 38   12/10/08 4:41p erickson
 * PR49930: add NEXUS_VideoDecoderPlaybackSettings for playback
 *  accurateSeek algorithm
 *
 * 37   12/9/08 6:22p erickson
 * PR49923: added firstPtsPassed callback
 *
 * 36   12/8/08 12:22p erickson
 * PR48155: check cdb valid and read to determine if fifo is hung
 *
 * 35   12/8/08 11:16a erickson
 * PR49930: add NEXUS_VideoDecoder_SetStartPts
 *
 * 34   11/26/08 12:39p erickson
 * PR49361: added NEXUS_VideoDecoderStatus.numPicturesReceived
 *
 * 33   11/18/08 11:55a erickson
 * PR48155: check for static fifo, then call stc channel to see if we have
 *  a TSM deadlock. if so, flush to recover.
 *
 * 32   10/17/08 9:16a erickson
 * PR47030: open and close the XVD channel when the VideoDecoder is
 *  connected or disconnected to a VideoWindow
 *
 * 31   10/8/08 12:51a erickson
 * PR47666: add firstPts callback
 *
 * 30   9/9/08 10:04a erickson
 * PR45940: use NEXUS_Rave_GetCdbValidPointer_isr to determine if
 *  fifoEmpty should be fired
 *
 * 29   9/2/08 1:51p erickson
 * PR45940: added NEXUS_VideoDecoderSettings.fifoEmpty and
 *  NEXUS_VideoDecoderStatus.tsm
 *
 * 28   8/21/08 3:24p katrep
 * PR45527: NEXUS_CONFIG_IMAGE implementation for runtime FW download
 *
 * 27   8/21/08 3:15p erickson
 * PR45959: flatten still_ext and trick_ext into normal code
 *
 * 26   7/28/08 12:35p erickson
 * PR42924: move NEXUS_VideoDecoder_ChannelChangeReport out of public API
 *
 * 25   7/25/08 4:13p jtna
 * PR45093: defer channel change report using event
 *
 * 24   7/14/08 12:13p erickson
 * PR42739: added playbackOverride option for IPTV. fixed
 *  additionalPtsOffset logic.
 *
 * PR42739/1   7/10/08 10:27p ssood
 * PR42739: Adding support to absorb high jitter for Live IP Channels
 *
 * 23   7/11/08 5:07p jtna
 * PR42924: Add channel change measurements functionality
 *
 * 22   6/13/08 7:40p agin
 * PR43579: Fix compile error with macro for when
 *  NEXUS_VIDEO_DECODER_TRICK is not defined.
 *
 * 21   5/20/08 3:42p erickson
 * PR42128: added dataReadyCallback to ExtendedSettings
 *
 * 20   5/16/08 11:04a erickson
 * PR42751: centralize logic for calling BXVD_SetVideoDisplayMode so that
 *  all aspects can be considered in one spot
 *
 * 19   5/5/08 11:47a jrubio
 * PR40169: add 7325/7335 code
 *
 * 18   4/28/08 4:10p erickson
 * PR41533: unify EIA-608 and EIA-708 closed caption data capture in
 *  NEXUS_VideoInput_ReadClosedCaption
 *
 * 17   4/25/08 10:54a erickson
 * PR41968: added NEXUS_VideoDecoder_Reset
 *
 * 16   4/18/08 1:53p erickson
 * PR41533: impl EIA-708 parsing and capture
 *
 * 15   4/17/08 4:38p erickson
 * PR41533: rework NEXUS_VideoDecoder_GetUserDataBuffer to use
 *  NEXUS_UserDataHeader before payload
 *
 * 14   4/7/08 10:16a erickson
 * PR37893: convert to XPT PI sw rave
 *
 * 13   3/26/08 3:38p vsilyaev
 * PR 40862: Added code to propagate display framerate to the decoder
 *
 * 12   3/25/08 5:28p bandrews
 * PR40090: added compile flag
 *
 * 11   3/25/08 12:55p erickson
 * PR40850: add more status
 *
 * 10   3/24/08 3:16p bandrews
 * PR40090: API changes
 *
 * 9   2/26/08 10:03a erickson
 * PR39934: added SW RAVE support into Transport
 *
 * 8   2/25/08 3:01p erickson
 * PR39786: update
 *
 * 7   2/20/08 4:57p erickson
 * PR39786: initial impl
 *
 * 6   2/7/08 4:41p erickson
 * PR34926: added userDataBufferSize open settings, convert to BMEM_Alloc
 *  for kernel mode support
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
 * 2   1/22/08 3:45p erickson
 * PR38888: added streamCallback for sequence header notification
 *
 * 1   1/18/08 2:20p jgarrett
 * PR 38808: Merging to main branch
 *
 * Nexus_Devel/26   1/15/08 11:11a vsilyaev
 * PR 38682: Removed duplicated globals
 *
 * Nexus_Devel/25   1/2/08 3:54p erickson
 * PR36152: fix 7405 PIP
 *
 * Nexus_Devel/24   11/8/07 9:11a erickson
 * PR36802: added mosaic mode
 *
 * Nexus_Devel/23   11/2/07 4:42p vsilyaev
 * PR 36696: Used connector model for VideoInput's and VideoOutput's
 *
 * Nexus_Devel/22   10/18/07 11:01a erickson
 * PR36152: added 7405 pip
 *
 * Nexus_Devel/21   10/17/07 2:05p erickson
 * PR34926: rework NEXUS_VideoDecoderHandle to internally point to an XVD
 * channel, not device
 *
 * Nexus_Devel/20   10/16/07 4:54p vsilyaev
 * PR 35824: Added implementation of trickmode extension
 *
 * Nexus_Devel/19   10/16/07 3:43p vsilyaev
 * PR 35824: Added trickmode extension
 *
 * Nexus_Devel/18   10/9/07 3:48p erickson
 * PR34926: change Params to StartSettings
 *
 * Nexus_Devel/17   10/8/07 11:56a erickson
 * PR34926: reworked VBI interface
 *
 * Nexus_Devel/16   10/3/07 2:50p erickson
 * PR34926: fix sourceChanged callback
 *
 * Nexus_Devel/15   10/3/07 1:06p erickson
 * PR34926: impl userdata
 *
 * Nexus_Devel/14   10/1/07 1:45p erickson
 * PR34925: make Rave a private API
 *
 * Nexus_Devel/13   9/28/07 1:20p erickson
 * PR35395: added video_decoder thunk
 *
 * Nexus_Devel/12   9/28/07 11:32a erickson
 * PR34926: rename videodecoder to video_decoder
 *
 * Nexus_Devel/11   9/28/07 11:06a erickson
 * PR34926: 0.1 prerel review
 *
 * Nexus_Devel/10   9/28/07 10:01a erickson
 * PR34926: 0.1 prerel review
 *
 * Nexus_Devel/9   9/26/07 11:01a erickson
 * PR34926: refactor implementation into _priv source file
 *
 * Nexus_Devel/8   9/25/07 3:07p erickson
 * PR34926: update
 *
 * Nexus_Devel/7   9/25/07 12:27p erickson
 * PR34926: fix NUMs
 *
 * Nexus_Devel/6   9/24/07 12:52p erickson
 * PR34926: update Display connection
 *
 * Nexus_Devel/5   9/24/07 10:10a erickson
 * PR34926: update
 *
 * Nexus_Devel/4   9/21/07 4:18p erickson
 * PR34926: added DisplayConnection private interface
 *
 * Nexus_Devel/3   9/14/07 4:14p erickson
 * PR34926: initial compile
 *
 * Nexus_Devel/1   9/14/07 12:29p erickson
 * PR34925: update
 *
 **************************************************************************/
#ifndef NEXUS_VIDEO_DECODER_MODULE_H__
#define NEXUS_VIDEO_DECODER_MODULE_H__

#include "nexus_video_decoder_thunks.h"
#include "nexus_base.h"
#include "nexus_video_decoder_trick.h"
#include "nexus_video_decoder_primer.h"
#include "nexus_still_decoder.h"
#include "nexus_video_decoder.h"
#include "nexus_video_decoder_extra.h"
#include "nexus_mosaic_video_decoder.h" /* standard on 740x */
#include "priv/nexus_core_video.h"
#include "priv/nexus_video_decoder_priv.h"
#include "priv/nexus_rave_priv.h"
#include "priv/nexus_core_img.h"
#include "nexus_video_decoder_init.h"
#include "priv/nexus_video_decoder_standby_priv.h"
#include "nexus_platform_features.h"
#include "nexus_core_utils.h"
#include "bxvd.h"
#include "budp_dccparse.h"
#include "budp_jp3dparse.h"
#include "budp_dccparse_dss.h"
#include "priv/nexus_stc_channel_priv.h"
#include "bxdm_dih.h"
#include "bxdm_pp.h"
#if NEXUS_OTFPVR
#include "botf.h"
#endif
#if NEXUS_VIDEO_DECODER_EXTENSION_PREPROCESS_PICTURE
#include "nexus_video_decoder_preprocess_picture.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef NEXUS_MODULE_SELF
#error Cant be in two modules at the same time
#endif

#define NEXUS_MODULE_NAME videodecoder
#define NEXUS_MODULE_SELF g_NEXUS_videoDecoderModule

#define NEXUS_NUM_XVD_CHANNELS 16

struct NEXUS_VideoDecoderDevice;

typedef struct NEXUS_VideoDecoder_P_Interface {
    void (*Close)( NEXUS_VideoDecoderHandle handle);
    void (*GetOpenSettings)( NEXUS_VideoDecoderHandle handle, NEXUS_VideoDecoderOpenSettings *pOpenSettings);
    void (*GetSettings)( NEXUS_VideoDecoderHandle handle, NEXUS_VideoDecoderSettings *pSettings );
    NEXUS_Error (*SetSettings)( NEXUS_VideoDecoderHandle handle, const NEXUS_VideoDecoderSettings *pSettings);
    NEXUS_Error (*Start)( NEXUS_VideoDecoderHandle handle, const NEXUS_VideoDecoderStartSettings *pSettings);
    void (*Stop)( NEXUS_VideoDecoderHandle handle);
    void (*Flush)( NEXUS_VideoDecoderHandle handle);
    NEXUS_Error (*GetStatus)( NEXUS_VideoDecoderHandle handle, NEXUS_VideoDecoderStatus *pStatus);
    NEXUS_VideoInput (*GetConnector)(NEXUS_VideoDecoderHandle handle);
    NEXUS_Error (*GetStreamInformation)(NEXUS_VideoDecoderHandle handle, NEXUS_VideoDecoderStreamInformation *pStreamInformation);
    NEXUS_Error (*SetStartPts)( NEXUS_VideoDecoderHandle handle, uint32_t pts);
    void (*IsCodecSupported)( NEXUS_VideoDecoderHandle videoDecoder, NEXUS_VideoCodec codec, bool *pSupported);
    NEXUS_Error (*SetPowerState)( NEXUS_VideoDecoderHandle handle, bool powerUp);
    void (*Reset)( NEXUS_VideoDecoderHandle handle);
    NEXUS_Error (*GetExtendedStatus)( NEXUS_VideoDecoderHandle handle, NEXUS_VideoDecoderExtendedStatus *pStatus);
    void (*GetExtendedSettings)( NEXUS_VideoDecoderHandle handle, NEXUS_VideoDecoderExtendedSettings *pSettings);
    NEXUS_Error (*SetExtendedSettings)( NEXUS_VideoDecoderHandle handle, const NEXUS_VideoDecoderExtendedSettings *pSettings);
    NEXUS_StripedSurfaceHandle (*CreateStripedSurface)( NEXUS_VideoDecoderHandle handle);
    void (*DestroyStripedSurface)( NEXUS_VideoDecoderHandle videoDecoder, NEXUS_StripedSurfaceHandle stripedSurface);
    NEXUS_Error (*CreateStripedMosaicSurfaces)( NEXUS_VideoDecoderHandle videoDecoder, NEXUS_StripedSurfaceHandle *pStripedSurfaces, unsigned int maxSurfaces, unsigned int *pSurfaceCount) ;
    void (*DestroyStripedMosaicSurfaces)( NEXUS_VideoDecoderHandle videoDecoder, const NEXUS_StripedSurfaceHandle *pStripedSurfaces, unsigned int surfaceCount);
    NEXUS_Error (*GetMostRecentPts)( NEXUS_VideoDecoderHandle handle, uint32_t *pPts);
    NEXUS_VideoDecoderPrimerHandle (*OpenPrimer)( NEXUS_VideoDecoderHandle videoDecoder);
    NEXUS_Error (*StartPrimer)( NEXUS_VideoDecoderPrimerHandle primer, const NEXUS_VideoDecoderStartSettings *pStartSettings);
    void (*StopPrimer)( NEXUS_VideoDecoderPrimerHandle primer);
    NEXUS_Error (*StopPrimerAndStartDecode)( NEXUS_VideoDecoderPrimerHandle primer, NEXUS_VideoDecoderHandle videoDecoder );
    void (*GetTrickState)( NEXUS_VideoDecoderHandle handle, NEXUS_VideoDecoderTrickState *pState);
    NEXUS_Error (*SetTrickState)( NEXUS_VideoDecoderHandle handle, const NEXUS_VideoDecoderTrickState *pState);
    NEXUS_Error (*FrameAdvance)(NEXUS_VideoDecoderHandle videoDecoder);
    NEXUS_Error (*GetNextPts)( NEXUS_VideoDecoderHandle handle, uint32_t *pNextPts);
    void (*GetPlaybackSettings)( NEXUS_VideoDecoderHandle handle, NEXUS_VideoDecoderPlaybackSettings *pSettings);
    NEXUS_Error (*SetPlaybackSettings)( NEXUS_VideoDecoderHandle handle, const NEXUS_VideoDecoderPlaybackSettings *pSettings);
    NEXUS_StillDecoderHandle (*StillDecoder_Open)( NEXUS_VideoDecoderHandle parentDecoder, unsigned index, const NEXUS_StillDecoderOpenSettings *pSettings );

#if NEXUS_HAS_ASTM
    void (*GetAstmSettings_priv)( NEXUS_VideoDecoderHandle videoDecoder, NEXUS_VideoDecoderAstmSettings * pAstmSettings);
    NEXUS_Error (*SetAstmSettings_priv)(NEXUS_VideoDecoderHandle videoDecoder, const NEXUS_VideoDecoderAstmSettings * pAstmSettings);
    NEXUS_Error (*GetAstmStatus_isr)( NEXUS_VideoDecoderHandle videoDecoder, NEXUS_VideoDecoderAstmStatus * pAstmStatus);
#endif /* NEXUS_HAS_ASTM */

    void (*GetDisplayConnection_priv)( NEXUS_VideoDecoderHandle handle, NEXUS_VideoDecoderDisplayConnection *pConnection);
    NEXUS_Error (*SetDisplayConnection_priv)( NEXUS_VideoDecoderHandle handle, const NEXUS_VideoDecoderDisplayConnection *pConnection);
    void (*GetSourceId_priv)( NEXUS_VideoDecoderHandle handle, BAVC_SourceId *pSource);
    void (*GetHeap_priv)( NEXUS_VideoDecoderHandle handle, NEXUS_HeapHandle *pHeap);
    void (*GetSyncSettings_priv)( NEXUS_VideoDecoderHandle videoDecoder, NEXUS_VideoInputSyncSettings *pSyncSettings);
    NEXUS_Error (*SetSyncSettings_priv)(NEXUS_VideoDecoderHandle videoDecoder, const NEXUS_VideoInputSyncSettings *pSyncSettings);
    NEXUS_Error (*GetSyncStatus_isr)(NEXUS_VideoDecoderHandle videoDecoder, NEXUS_VideoInputSyncStatus *pSyncStatus );
    void (*UpdateDisplayInformation_priv)( NEXUS_VideoDecoderHandle videoDecoder, const NEXUS_VideoDecoder_DisplayInformation *displayInformation);
} NEXUS_VideoDecoder_P_Interface ;

#ifdef __cplusplus
}
#endif

/* NEXUS_VideoDecoder_P_Interface  should be already defined */

#if NEXUS_NUM_DSP_VIDEO_DECODERS
#include "nexus_video_decoder_module_dsp.h"
#endif
#if NEXUS_NUM_ZSP_VIDEO_DECODERS
#include "nexus_video_decoder_module_zsp.h"
#endif

#if NEXUS_NUM_SOFT_VIDEO_DECODERS
#include "nexus_video_decoder_module_soft.h"
#endif

#if NEXUS_NUM_SID_VIDEO_DECODERS
#include "nexus_video_decoder_module_sid.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

extern const NEXUS_VideoDecoder_P_Interface NEXUS_VideoDecoder_P_Interface_Avd;

BDBG_OBJECT_ID_DECLARE(NEXUS_VideoDecoder_P_Xdm);

#define NEXUS_VIDEO_DECODER_P_USE_XVD_DIH   1

typedef struct NEXUS_VideoDecoder_P_Xdm {
    BDBG_OBJECT(NEXUS_VideoDecoder_P_Xdm)
    BXDM_PictureProvider_Handle pictureProvider;
#if !NEXUS_VIDEO_DECODER_P_USE_XVD_DIH
    BXDM_DisplayInterruptHandler_Handle displayInterrupt;
    BINT_CallbackHandle topInt;
    BINT_CallbackHandle bottomInt;
    BINT_CallbackHandle frameInt;
    BAVC_Polarity lastPolarity;
    unsigned interruptCount;
#endif
} NEXUS_VideoDecoder_P_Xdm;

/* This structure provides the implementation of the NEXUS_VideoDecoderHandle.
It corresponds to one XVD channel. */
struct NEXUS_VideoDecoder {
    NEXUS_OBJECT(NEXUS_VideoDecoder);
    const NEXUS_VideoDecoder_P_Interface *interface;
    struct NEXUS_VideoDecoderDevice *device;
    BXVD_ChannelHandle dec;
    BKNI_EventHandle source_changed_event;
    NEXUS_RaveHandle rave;
    bool raveDetached;
    bool enhancementRaveDetached;
    NEXUS_RaveHandle savedRave; /* the primer interface may replace the main RAVE context with a primed context. */
    NEXUS_RaveHandle enhancementRave; /* rave context for  enhancementPidChannel */
    NEXUS_VideoDecoderStartSettings startSettings;
    NEXUS_VideoDecoderExtendedSettings extendedSettings;

    struct {
        BXVD_Userdata_Handle handle;
        unsigned char *buf; /* size is settings.userDataBufferSize */
        unsigned rptr, wptr, wrap_ptr;
        NEXUS_VideoDecoderUserDataStatus status;
        NEXUS_DigitalVbiDataCallback vbiDataCallback_isr;
        NEXUS_VideoInput vbiVideoInput;
        unsigned lastGetBufferSize;    
    } userdata;

    BXVD_StillPictureBuffers still_picture_buffer;
    BKNI_EventHandle still_picture_event;
    BAVC_MFD_Picture last_field; /* current picture going to display */
    bool last_field_new;
    NEXUS_VideoDecoderStreamInformation streamInfo; /* newest picture coming from decoder when starting */
    NEXUS_VideoDecoderStreamInformation lastStreamInfo; /* last picture coming from decoder. stored after stop decode. */
    struct {
        BXVD_PictureCoding pictureCoding;
    } pictureParameterInfo; /* TODO: consider storing whole BXVD_PictureParameterInfo at isr time, then building NEXUS_VideoDecoderStreamInformation at get time */
    NEXUS_TransportType transportType;
    NEXUS_VideoDecoderOpenMosaicSettings openSettings; /* superset */
    NEXUS_VideoDecoderMosaicSettings mosaicSettings;
    NEXUS_VideoDecoderSettings settings;
    NEXUS_IsrCallbackHandle userdataCallback;
    NEXUS_IsrCallbackHandle afdChangedCallback;
    NEXUS_IsrCallbackHandle streamChangedCallback;
    NEXUS_IsrCallbackHandle sourceChangedCallback;
    NEXUS_IsrCallbackHandle ptsErrorCallback;
    NEXUS_IsrCallbackHandle firstPtsCallback;
    NEXUS_IsrCallbackHandle firstPtsPassedCallback;
    NEXUS_IsrCallbackHandle decodeErrorCallback;
    NEXUS_IsrCallbackHandle fnrtChunkDoneCallback;
    struct {
        NEXUS_IsrCallbackHandle firstPtsCallback;
        NEXUS_IsrCallbackHandle firstPtsPassedCallback;
        NEXUS_VideoDecoderPlaybackSettings videoDecoderPlaybackSettings;
    } playback;
    unsigned dataReadyCount;
    unsigned numPicturesReceivedToFlush;
    NEXUS_IsrCallbackHandle dataReadyCallback;
    NEXUS_VideoDecoderDisplayConnection displayConnection;
    NEXUS_IsrCallbackHandle s3DTVChangedCallback;
    NEXUS_VideoDecoder3DTVStatus s3DTVStatus;
    BKNI_EventHandle s3DTVStatusEvent;
    NEXUS_EventCallbackHandle s3DTVStatusEventHandler;
    NEXUS_TimerHandle s3DTVStatusTimer;
    uint32_t s3DTVStatusPts;

    BUDP_DCCparse_Format currentUserDataFormat;
    NEXUS_UserDataFormat userDataFormat;
    bool useUserDataFormat;
    struct {
#define B_MAX_VBI_CC_COUNT 32 /* required by UDPlib */
        BUDP_DCCparse_ccdata ccData[B_MAX_VBI_CC_COUNT];
        BUDP_DCCparse_dss_cc_subtitle dssCcData[B_MAX_VBI_CC_COUNT];
        BUDP_DCCparse_dss_cc_subtitle dssSubtitle[B_MAX_VBI_CC_COUNT];
    } udpData;

    uint32_t primerPtsOffset;
#if NEXUS_HAS_SYNC_CHANNEL
    struct
    {
        NEXUS_VideoInputSyncSettings settings;
        NEXUS_VideoInputSyncStatus status;
        bool startMuted;
        bool mute;
    } sync;
#endif

#if NEXUS_HAS_ASTM
    struct
    {
        bool permitted;
        NEXUS_VideoDecoderAstmSettings settings;
        NEXUS_VideoDecoderAstmStatus status;
    } astm;
#endif

    unsigned pts_error_cnt;
    bool started;
    bool tsm;
    int32_t ptsStcDifference;
    unsigned defaultDiscardThreshold; /* store the default from XVD after Start */
    unsigned defaultVeryLateThreshold;
    bool firstPtsPassed;

    unsigned pictureDeliveryCount;

    struct
    {
        unsigned count;  /* we believe the fifo is empty. this requires extra logic because internal buffers might cause false detect */
        uint32_t lastCdbValidPointer;
        uint32_t lastCdbReadPointer;
        NEXUS_IsrCallbackHandle callback;
    } fifoEmpty;
    
    struct
    {
        unsigned staticCount; /* fifo has not moved */
        unsigned lastPictureDeliveryCount;
        uint32_t lastCdbValidPointer;
        uint32_t lastCdbReadPointer;
        uint32_t lastPts;
        uint32_t lastPtsValid;
        NEXUS_TimerHandle timer;
    } fifoWatchdog;

    unsigned overflowCount;

    struct {
        bool wasActive; /* set to true if OTF PVR was activated between start and stop */
#if NEXUS_OTFPVR
        bool active; /* set to true if OTF PVR currently active */
        bool eos;
        NEXUS_TimerHandle timer;
        NEXUS_RaveHandle rave; /* rave context that is used by the OTFPVR to feed ITB entires into the decoder */
        BAVC_XptContextMap xptContextMap; /* we need to keep this since XVD expects pointer to BAVC_XptContextMap, and we can't use automatic variable */
        BOTF_Handle otf;
        BOTF_Status status;
#endif
    } otfPvr; /* state specific for the OTF PVR operations */

    NEXUS_VideoDecoderTrickState trickState;
    struct {
        unsigned pictureId;
        unsigned pictureDisplayCount;
    } maxFrameRepeat; /* state if trickState.maxFrameRepeat != 0 */
        
    unsigned xdmIndex; /* XDM index. Each XDM instance is mapped with a decoder object and an MFD interrupt. */
    unsigned mfdIndex; /* MFD index. This is the apparent "main index" to users of VideoDecoder (e.g. Display).
                           For 7405, AVD0 services MFD0 and MFD1, so mainIndex can be 0 or 1.
                           For 7420, AVD0 services MFD0 and AVD1 services MFD1, so mainIndex can be 0 or 1.
                           For 3548, AVD0 services MFD0, so mainIndex can be 0. */
    bool isInterruptChannel; /* For mosaic mode, only the first channel on a device gives interrupts. */
    NEXUS_VideoDecoder_DisplayInformation displayInformation;
    NEXUS_VideoInputObject input;
    unsigned additionalPtsOffset; /* stores additional pts offset that is applied for certain codecs (MPEG2 needs additional 120msec of delay */
    bool errorHandlingOverride; /* if true should use the errorHandlingMode instead of the user supplied value */
    NEXUS_VideoDecoderErrorHandling errorHandlingMode;

    bool mosaicMode;
    bool mosaicParent; /* set true if this is the mosaic parent */
    NEXUS_VideoDecoderHandle linkedDecoder;

    BKNI_EventHandle channelChangeReportEvent;
    NEXUS_EventCallbackHandle channelChangeReportEventHandler;
    bool validOutputPic;
    unsigned cdbLength; /* actual size of CDB buffer */
    unsigned itbLength; /* actual size of ITB buffer */
    NEXUS_VideoDecoder_P_Xdm xdm;
    union {
#if NEXUS_NUM_DSP_VIDEO_DECODERS
        NEXUS_VideoDecoder_Dsp dsp;
#endif
#if NEXUS_NUM_ZSP_VIDEO_DECODERS
        NEXUS_VideoDecoder_Zsp zsp;
#endif
#if NEXUS_NUM_SOFT_VIDEO_DECODERS
        NEXUS_VideoDecoder_Soft soft;
#endif
#if NEXUS_NUM_SID_VIDEO_DECODERS
        NEXUS_VideoDecoder_Sid sid;
#endif
        unsigned unused;
    } decoder;
    
    struct {
        NEXUS_VideoDecoderCrc *data;
        unsigned size, wptr, rptr;
    } crc;
};

/* The NEXUS_VideoDecoderDevice corresponds to an AVD HW block. */
struct NEXUS_VideoDecoderDevice {
    BXVD_Handle xvd;
    BKNI_EventHandle watchdog_event;
    NEXUS_EventCallbackHandle watchdogEventHandler;
    unsigned numWatchdogs;
    struct NEXUS_VideoDecoder channel[NEXUS_NUM_XVD_CHANNELS];
    BMEM_Handle mem; /* heap used by AVD */
    /* image interface */
    void * img_context;
    BIMG_Interface img_interface;

    unsigned mosaicCount;
    BXDM_DisplayInterruptHandler_Handle hXdmDih[BXVD_DisplayInterrupt_eMax];
    bool xdmInUse[BXVD_DisplayInterrupt_eMax];
};

/* global instance data */
extern NEXUS_ModuleHandle g_NEXUS_videoDecoderModule;
extern NEXUS_VideoDecoderModuleSettings g_NEXUS_videoDecoderModuleSettings;
extern struct NEXUS_VideoDecoderDevice g_NEXUS_videoDecoderXvdDevices[NEXUS_MAX_XVD_DEVICES];

#define LOCK_TRANSPORT()    NEXUS_Module_Lock(g_NEXUS_videoDecoderModuleSettings.transport)
#define UNLOCK_TRANSPORT()  NEXUS_Module_Unlock(g_NEXUS_videoDecoderModuleSettings.transport)

/**
Priv functions
**/
NEXUS_Error NEXUS_VideoDecoder_P_OpenChannel(NEXUS_VideoDecoderHandle videoDecoder);
void NEXUS_VideoDecoder_P_CloseChannel(NEXUS_VideoDecoderHandle videoDecoder);
void NEXUS_VideoDecoder_P_WatchdogHandler(void *data);
void NEXUS_VideoDecoder_P_Watchdog_isr(void *data, int not_used, void *not_used2);
void NEXUS_VideoDecoder_P_DataReady_isr(void *data, int chIndex, void *field);
void NEXUS_VideoDecoder_P_RequestStc_isr(void *data, int chIndex, void *pts_info);
void NEXUS_VideoDecoder_P_PtsError_isr(void *data, int chIndex, void *pts_info);
void NEXUS_VideoDecoder_P_PictureParams_isr(void *data, int chIndex, void *info_);
void NEXUS_VideoDecoder_P_FirstPtsPassed_isr(void *data, int chIndex, void *pts_info);
void NEXUS_VideoDecoder_P_UserdataReady_isr(void *data, int chInt, void *not_used);
void NEXUS_VideoDecoder_P_PtsStcOffset_isr(void *data, int unused, void *pOffset);
void NEXUS_VideoDecoder_P_DecodeError_isr(void *data, int unused, void *unused2);
void NEXUS_VideoDecoder_P_3DTVTimer(void* context);
void NEXUS_VideoDecoder_P_Jp3dSignal_isr(NEXUS_VideoDecoderHandle videoDecoder, uint16_t type);
void NEXUS_VideoDecoder_P_PictureExtensionData_isr(void *data, int unused, void *pData);
void NEXUS_VideoDecoder_P_FnrtChunkDone_isr(void *data, int unused, void *unused2);
BERR_Code NEXUS_VideoDecoder_P_GetCdbLevelCallback_isr(void *pContext, unsigned *pCdbLevel);
BERR_Code NEXUS_VideoDecoder_P_GetPtsCallback_isr(void *pContext, BAVC_PTSInfo *pPTSInfo);
BERR_Code NEXUS_VideoDecoder_P_StcValidCallback_isr(void *pContext);
BERR_Code NEXUS_VideoDecoder_P_SetPcrOffset_isr(void *pContext, uint32_t pcrOffset);
BERR_Code NEXUS_VideoDecoder_P_GetPcrOffset_isr(void *pContext, uint32_t *pPcrOffset);

NEXUS_Error NEXUS_VideoDecoder_P_SetCrcFifoSize(NEXUS_VideoDecoderHandle videoDecoder, bool forceDisable);
NEXUS_Error NEXUS_VideoDecoder_P_SetPtsOffset(NEXUS_VideoDecoderHandle videoDecoder);
NEXUS_Error NEXUS_VideoDecoder_P_SetMute(NEXUS_VideoDecoderHandle videoDecoder);
NEXUS_Error NEXUS_VideoDecoder_P_SetFreeze(NEXUS_VideoDecoderHandle videoDecoder);
NEXUS_Error NEXUS_VideoDecoder_P_SetUserdata(NEXUS_VideoDecoderHandle videoDecoder);
NEXUS_Error NEXUS_VideoDecoder_P_SetTsm(NEXUS_VideoDecoderHandle videoDecoder);
void NEXUS_VideoDecoder_P_ChannelChangeReport(void *context);
/* SetChannelChangeMode, priv Start and Stop  are used to implement controllable video decoder Flush*/
void NEXUS_VideoDecoder_P_Stop_priv(NEXUS_VideoDecoderHandle videoDecoder);
NEXUS_Error NEXUS_VideoDecoder_P_Start_priv(NEXUS_VideoDecoderHandle videoDecoder, const NEXUS_VideoDecoderStartSettings *pStartSettings, bool otfPvr);
NEXUS_Error NEXUS_VideoDecoder_P_SetChannelChangeMode(NEXUS_VideoDecoderHandle videoDecoder, NEXUS_VideoDecoder_ChannelChangeMode mode);
void NEXUS_VideoDecoder_P_GetRaveSettings(NEXUS_VideoDecoderHandle videoDecoder, NEXUS_RaveOpenSettings *pRaveSettings, const NEXUS_VideoDecoderOpenMosaicSettings *pOpenSettings);
void NEXUS_VideoDecoder_P_ApplyDisplayInformation( NEXUS_VideoDecoderHandle videoDecoder);


#if NEXUS_OTFPVR
void NEXUS_VideoDecoder_P_OtfPvr_Stop(NEXUS_VideoDecoderHandle videoDecoder);
void NEXUS_VideoDecoder_P_OtfPvr_DisableForFlush(NEXUS_VideoDecoderHandle videoDecoder);
void NEXUS_VideoDecoder_P_OtfPvr_Flush(NEXUS_VideoDecoderHandle videoDecoder);
NEXUS_Error NEXUS_VideoDecoder_P_OtfPvr_Activate(NEXUS_VideoDecoderHandle videoDecoder, const NEXUS_RaveStatus *raveStatus, BXVD_DecodeSettings *xvdCfg);
NEXUS_Error NEXUS_VideoDecoder_P_OtfPvr_SetTrickState(NEXUS_VideoDecoderHandle videoDecoder, const NEXUS_VideoDecoderTrickState *pState, bool *complete);
void NEXUS_VideoDecoder_P_OtfPvr_UpdateStatus(NEXUS_VideoDecoderHandle videoDecoder, NEXUS_VideoDecoderStatus *pStatus);
#endif


/**
Common
**/
NEXUS_VideoFormat NEXUS_VideoDecoder_P_DeriveFormat(unsigned height, unsigned frameRate, bool interlaced);

void NEXUS_VideoDecoder_P_Trick_Reset_Generic(NEXUS_VideoDecoderHandle videoDecoder);


NEXUS_VideoDecoderHandle NEXUS_VideoDecoder_P_Open_Avd( unsigned index, const NEXUS_VideoDecoderOpenSettings *pOpenSettings);
void NEXUS_VideoDecoder_P_Close_Avd( NEXUS_VideoDecoderHandle handle);
NEXUS_Error NEXUS_VideoDecoder_P_SetSettings_Avd( NEXUS_VideoDecoderHandle handle, const NEXUS_VideoDecoderSettings *pSettings);
NEXUS_Error NEXUS_VideoDecoder_P_Start_Avd( NEXUS_VideoDecoderHandle handle, const NEXUS_VideoDecoderStartSettings *pSettings);
void NEXUS_VideoDecoder_P_Stop_Avd( NEXUS_VideoDecoderHandle handle);
void NEXUS_VideoDecoder_P_Flush_Avd( NEXUS_VideoDecoderHandle handle);
NEXUS_Error NEXUS_VideoDecoder_P_GetStatus_Avd( NEXUS_VideoDecoderHandle handle, NEXUS_VideoDecoderStatus *pStatus);
NEXUS_Error NEXUS_VideoDecoder_P_GetStreamInformation_Avd(NEXUS_VideoDecoderHandle handle, NEXUS_VideoDecoderStreamInformation *pStreamInformation);
NEXUS_Error NEXUS_VideoDecoder_P_SetStartPts_Avd( NEXUS_VideoDecoderHandle handle, uint32_t pts);
void NEXUS_VideoDecoder_P_IsCodecSupported_Avd( NEXUS_VideoDecoderHandle videoDecoder, NEXUS_VideoCodec codec, bool *pSupported);
NEXUS_Error NEXUS_VideoDecoder_P_SetPowerState_Avd( NEXUS_VideoDecoderHandle handle, bool powerUp);
NEXUS_VideoDecoderHandle NEXUS_VideoDecoder_P_OpenMosaic_Avd( unsigned parentIndex, unsigned index, const NEXUS_VideoDecoderOpenMosaicSettings *pOpenSettings );
void NEXUS_VideoDecoder_P_Reset_Avd( NEXUS_VideoDecoderHandle handle);
NEXUS_Error NEXUS_VideoDecoder_P_GetExtendedStatus_Avd( NEXUS_VideoDecoderHandle handle, NEXUS_VideoDecoderExtendedStatus *pStatus);
void NEXUS_VideoDecoder_P_GetExtendedSettings_Avd( NEXUS_VideoDecoderHandle handle, NEXUS_VideoDecoderExtendedSettings *pSettings);
NEXUS_Error NEXUS_VideoDecoder_P_SetExtendedSettings_Avd( NEXUS_VideoDecoderHandle handle, const NEXUS_VideoDecoderExtendedSettings *pSettings);
NEXUS_StripedSurfaceHandle NEXUS_VideoDecoder_P_CreateStripedSurface_Avd( NEXUS_VideoDecoderHandle handle);
void NEXUS_VideoDecoder_P_DestroyStripedSurface_Avd( NEXUS_VideoDecoderHandle videoDecoder, NEXUS_StripedSurfaceHandle stripedSurface);
NEXUS_Error NEXUS_VideoDecoder_P_CreateStripedMosaicSurfaces_Avd( NEXUS_VideoDecoderHandle videoDecoder, NEXUS_StripedSurfaceHandle *pStripedSurfaces, unsigned int maxSurfaces, unsigned int *pSurfaceCount) ;
void NEXUS_VideoDecoder_P_DestroyStripedMosaicSurfaces_Avd( NEXUS_VideoDecoderHandle videoDecoder, const NEXUS_StripedSurfaceHandle *pStripedSurfaces, unsigned int surfaceCount);
NEXUS_Error NEXUS_VideoDecoder_P_GetMostRecentPts_Avd( NEXUS_VideoDecoderHandle handle, uint32_t *pPts);

NEXUS_VideoDecoderPrimerHandle NEXUS_VideoDecoderPrimer_P_Open_Avd( NEXUS_VideoDecoderHandle videoDecoder);
void NEXUS_VideoDecoderPrimer_P_Close_Avd(NEXUS_VideoDecoderPrimerHandle primer);
NEXUS_Error NEXUS_VideoDecoderPrimer_P_Start_Avd(NEXUS_VideoDecoderPrimerHandle primer, const NEXUS_VideoDecoderStartSettings *pStartSettings);
void NEXUS_VideoDecoderPrimer_P_Stop_Avd(NEXUS_VideoDecoderPrimerHandle primer);
NEXUS_Error NEXUS_VideoDecoderPrimer_P_StopPrimerAndStartDecode_Avd( NEXUS_VideoDecoderPrimerHandle primer, NEXUS_VideoDecoderHandle videoDecoder);
NEXUS_VideoDecoderHandle nexus_videodecoderprimer_p_getdecoder(NEXUS_VideoDecoderPrimerHandle primer);

NEXUS_Error NEXUS_VideoDecoder_P_SetTrickState_Avd( NEXUS_VideoDecoderHandle handle, const NEXUS_VideoDecoderTrickState *pState);
NEXUS_Error NEXUS_VideoDecoder_P_GetNextPts_Avd( NEXUS_VideoDecoderHandle handle, uint32_t *pNextPts);
NEXUS_Error NEXUS_VideoDecoder_P_FrameAdvance_Avd(NEXUS_VideoDecoderHandle videoDecoder);
void NEXUS_VideoDecoder_P_GetPlaybackSettings_Common( NEXUS_VideoDecoderHandle handle, NEXUS_VideoDecoderPlaybackSettings *pSettings);
NEXUS_Error NEXUS_VideoDecoder_P_SetPlaybackSettings_Common( NEXUS_VideoDecoderHandle handle, const NEXUS_VideoDecoderPlaybackSettings *pSettings);

NEXUS_ModuleHandle NEXUS_VideoDecoderModule_P_Init_Avd(const NEXUS_VideoDecoderModuleSettings *pSettings);
void NEXUS_VideoDecoderModule_P_Uninit_Avd(void);

void NEXUS_VideoDecoder_GetDisplayConnection_priv_Avd( NEXUS_VideoDecoderHandle handle, NEXUS_VideoDecoderDisplayConnection *pConnection);
NEXUS_Error NEXUS_VideoDecoder_SetDisplayConnection_priv_Avd( NEXUS_VideoDecoderHandle handle, const NEXUS_VideoDecoderDisplayConnection *pConnection);
void NEXUS_VideoDecoder_GetSourceId_priv_Avd( NEXUS_VideoDecoderHandle handle, BAVC_SourceId *pSource);
void NEXUS_VideoDecoder_GetHeap_priv_Common( NEXUS_VideoDecoderHandle handle, NEXUS_HeapHandle *pHeap);
void NEXUS_VideoDecoder_GetSyncSettings_priv_Common( NEXUS_VideoDecoderHandle videoDecoder, NEXUS_VideoInputSyncSettings *pSyncSettings);
NEXUS_Error NEXUS_VideoDecoder_SetSyncSettings_priv_Avd(NEXUS_VideoDecoderHandle videoDecoder, const NEXUS_VideoInputSyncSettings *pSyncSettings);
NEXUS_Error NEXUS_VideoDecoder_GetSyncStatus_isr_Common(NEXUS_VideoDecoderHandle videoDecoder, NEXUS_VideoInputSyncStatus *pSyncStatus );
void NEXUS_VideoDecoder_UpdateDisplayInformation_priv_Avd( NEXUS_VideoDecoderHandle videoDecoder, const NEXUS_VideoDecoder_DisplayInformation *displayInformation);
#if NEXUS_HAS_ASTM
void NEXUS_VideoDecoder_GetAstmSettings_priv_Common( NEXUS_VideoDecoderHandle videoDecoder, NEXUS_VideoDecoderAstmSettings * pAstmSettings);
NEXUS_Error NEXUS_VideoDecoder_SetAstmSettings_priv_Avd(NEXUS_VideoDecoderHandle videoDecoder, const NEXUS_VideoDecoderAstmSettings * pAstmSettings);
NEXUS_Error NEXUS_VideoDecoder_GetAstmStatus_isr_Common( NEXUS_VideoDecoderHandle videoDecoder, NEXUS_VideoDecoderAstmStatus * pAstmStatus);
#endif /* NEXUS_HAS_ASTM */





#if VIDEO_PRIMER_STATS
void NEXUS_VideoDecoder_DataReady_P_PrimerHook_isr_Avd(NEXUS_VideoDecoderHandle videoDecoder, BAVC_MFD_Picture *pFieldData);
void NEXUS_VideoDecoder_Stop_P_PrimerHook_Avd(NEXUS_VideoDecoderHandle videoDecoder);
#endif


void NEXUS_VideoDecoder_P_GetOpenSettings_Common( NEXUS_VideoDecoderHandle videoDecoder, NEXUS_VideoDecoderOpenSettings *pOpenSettings);
void NEXUS_VideoDecoder_P_GetSettings_Common(NEXUS_VideoDecoderHandle videoDecoder, NEXUS_VideoDecoderSettings *pSettings);
void NEXUS_VideoDecoder_P_GetTrickState_Common( NEXUS_VideoDecoderHandle handle, NEXUS_VideoDecoderTrickState *pState);
NEXUS_VideoInput NEXUS_VideoDecoder_P_GetConnector_Common( NEXUS_VideoDecoderHandle videoDecoder);


NEXUS_Error NEXUS_VideoDecoder_P_Init_Generic(NEXUS_VideoDecoderHandle videoDecoder, const NEXUS_RaveOpenSettings *raveSettings, const NEXUS_VideoDecoderOpenMosaicSettings *openSettings);
void NEXUS_VideoDecoder_P_GetMosaicOpenSettings(NEXUS_VideoDecoderOpenMosaicSettings *openMosaicSettings, const NEXUS_VideoDecoderOpenSettings *openSettings);
void NEXUS_VideoDecoder_P_Close_Generic(NEXUS_VideoDecoderHandle videoDecoder);
NEXUS_Error NEXUS_VideoDecoder_P_Start_Generic_Prologue(NEXUS_VideoDecoderHandle videoDecoder, const NEXUS_VideoDecoderStartSettings *pStartSettings);
NEXUS_Error NEXUS_VideoDecoder_P_Start_Generic_Epilogue(NEXUS_VideoDecoderHandle videoDecoder, const NEXUS_VideoDecoderStartSettings *pStartSettings);
NEXUS_Error NEXUS_VideoDecoder_P_Start_Generic_Body(NEXUS_VideoDecoderHandle videoDecoder, const NEXUS_VideoDecoderStartSettings *pStartSettings, bool otfPvr, BAVC_VideoCompressionStd *pVideoCmprStd, NEXUS_RaveStatus *raveStatus, const NEXUS_StcChannelCallbacks *stcChannelCallbacks, bool *pPlayback, unsigned *stcChannelIndex);
NEXUS_Error NEXUS_VideoDecoder_P_Stop_Generic_Prologue(NEXUS_VideoDecoderHandle videoDecoder);
void NEXUS_VideoDecoder_P_Stop_Priv_Generic_Prologue(NEXUS_VideoDecoderHandle videoDecoder);
void NEXUS_VideoDecoder_P_Stop_Priv_Generic_Epilogue(NEXUS_VideoDecoderHandle videoDecoder);
void NEXUS_VideoDecoder_P_Stop_Generic_Epilogue(NEXUS_VideoDecoderHandle videoDecoder);
void NEXUS_VideoDecoder_P_GetStatus_Generic(NEXUS_VideoDecoderHandle videoDecoder, NEXUS_VideoDecoderStatus *pStatus);
const BAVC_MFD_Picture * NEXUS_VideoDecoder_P_DataReady_Generic_Prologue_isr(NEXUS_VideoDecoderHandle videoDecoder, const BAVC_MFD_Picture *pFieldData, BAVC_MFD_Picture *modifiedFieldData);
void NEXUS_VideoDecoder_P_DataReady_Generic_Epilogue_isr(NEXUS_VideoDecoderHandle videoDecoder, const BAVC_MFD_Picture *pFieldData, unsigned pictureDeliveryCount);

void NEXUS_VideoDecoder_P_GetExtendedSettings_NotImplemented( NEXUS_VideoDecoderHandle handle, NEXUS_VideoDecoderExtendedSettings *pSettings);
NEXUS_Error NEXUS_VideoDecoder_P_SetExtendedSettings_NotImplemented( NEXUS_VideoDecoderHandle handle, const NEXUS_VideoDecoderExtendedSettings *pSettings);


NEXUS_Error NEXUS_VideoDecoder_P_Xdm_Initialize(NEXUS_VideoDecoderHandle decoder, const BXDM_Decoder_Interface *decoderInterface, void *decoderContext);
void NEXUS_VideoDecoder_P_Xdm_Shutdown(NEXUS_VideoDecoderHandle decoder);
NEXUS_Error NEXUS_VideoDecoder_P_Xdm_Start(NEXUS_VideoDecoderHandle decoder);
void NEXUS_VideoDecoder_P_Xdm_Stop(NEXUS_VideoDecoderHandle decoder);
NEXUS_Error NEXUS_VideoDecoder_P_Xdm_ApplySettings(NEXUS_VideoDecoderHandle decoder, const NEXUS_VideoDecoderSettings *pSettings, bool force);
NEXUS_Error NEXUS_VideoDecoder_P_Xdm_SetChannelChangeMode(NEXUS_VideoDecoderHandle videoDecoder, NEXUS_VideoDecoder_ChannelChangeMode channelChangeMode);
NEXUS_Error NEXUS_VideoDecoder_P_Xdm_SetMute(NEXUS_VideoDecoderHandle videoDecoder);
NEXUS_Error NEXUS_VideoDecoder_P_Xdm_SetFreeze(NEXUS_VideoDecoderHandle videoDecoder);
NEXUS_Error NEXUS_VideoDecoder_SetSyncSettings_priv_Xdm(NEXUS_VideoDecoderHandle videoDecoder, const NEXUS_VideoInputSyncSettings *pSyncSettings);
NEXUS_Error NEXUS_VideoDecoder_P_Xdm_SetPtsOffset(NEXUS_VideoDecoderHandle videoDecoder);

NEXUS_Error NEXUS_VideoDecoder_SetDisplayConnection_priv_Xdm(NEXUS_VideoDecoderHandle decoder, const NEXUS_VideoDecoderDisplayConnection *connection);
void NEXUS_VideoDecoder_UpdateDisplayInformation_priv_Xdm(NEXUS_VideoDecoderHandle decoder, const NEXUS_VideoDecoder_DisplayInformation *pDisplayInformation);
NEXUS_Error NEXUS_VideoDecoder_P_GetStatus_Generic_Xdm( NEXUS_VideoDecoderHandle decoder, NEXUS_VideoDecoderStatus *pStatus);
BERR_Code NEXUS_VideoDecoder_P_GetPtsCallback_Xdm_isr(void *pContext, BAVC_PTSInfo *pPTSInfo);
BERR_Code NEXUS_VideoDecoder_P_StcValidCallback_Xdm_isr(void *pContext);

NEXUS_Error NEXUS_VideoDecoder_P_SetXvdDisplayInterrupt(NEXUS_VideoDecoderHandle videoDecoder, const NEXUS_VideoDecoderDisplayConnection *connection, BXVD_DisplayInterrupt interrupt);
NEXUS_Error NEXUS_VideoDecoder_P_SetSimulatedStc_isr(NEXUS_VideoDecoderHandle videoDecoder, uint32_t pts);
NEXUS_Error NEXUS_VideoDecoder_P_SetUnderflowMode_isr(NEXUS_VideoDecoderHandle videoDecoder, bool underflowMode);

NEXUS_Error NEXUS_VideoDecoder_P_SetLowLatencySettings(NEXUS_VideoDecoderHandle videoDecoder);

NEXUS_OBJECT_CLASS_DECLARE(NEXUS_VideoDecoder);
NEXUS_OBJECT_CLASS_DECLARE(NEXUS_StillDecoder);
NEXUS_OBJECT_CLASS_DECLARE(NEXUS_VideoDecoderPrimer);

#ifdef __cplusplus
}
#endif

#endif

