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
 * $brcm_Workfile: nexus_video_decoder.c $
 * $brcm_Revision: 334 $
 * $brcm_Date: 10/22/12 3:09p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/video_decoder/src/nexus_video_decoder.c $
 * 
 * 334   10/22/12 3:09p erickson
 * SW7435-429: add ITB, fix warning
 * 
 * 333   10/19/12 9:30a erickson
 * SW7435-429: add nexus_flow DBG
 * 
 * 332   10/16/12 5:10p erickson
 * SW7425-1792: unregister videoInput in _P_Release
 * 
 * 331   10/12/12 3:59p erickson
 * SW7435-419: change proc to BDBG_LOG to that they are still available
 *  with B_REFSW_DEBUG_LEVEL=err
 * 
 * 330   10/11/12 12:25p vsilyaev
 * SW7435-329: Converted NEXUS_VideoInput to NEXUS_VideoInputHandle and
 *  added object verification
 * 
 * 329   10/10/12 4:30p bandrews
 * SW7425-4021: fix watchdog to work even if data ready callback isn't
 *  coming
 * 
 * 328   10/9/12 3:06p jtna
 * SW7425-4047: make nexus set xvd 3d orientation for mpeg2 streams
 * 
 * 327   10/3/12 1:23p ahulse
 * SW7425-3903: turn on userdata while btp's are active
 * 
 * 326   10/1/12 7:34p bandrews
 * SW7425-2264: removed drop threshold as it is not consistent how it
 *  affects the display
 * 
 * 325   9/27/12 1:13p erickson
 * SW7435-362: lock module during init and uninit
 * 
 * 324   9/14/12 1:36p vsilyaev
 * SW7425-3939: Notify consumption for enhancement PidChannel
 * 
 * 323   9/6/12 8:38p bandrews
 * SW7231-967: disable garbage fifo watchdog by default
 * 
 * 322   8/31/12 11:30a erickson
 * SW7429-253: add NEXUS_VideoDecoderStatus.bufferLatency
 * 
 * 321   8/29/12 5:13p bandrews
 * SW7231-975: check CDB/ITB fullness before flushing; one must be full in
 *  order to need to flush, otherwise just allow buildup
 * 
 * 320   8/29/12 4:20p bandrews
 * SW7425-3820: take PTS movement into account for FIFO watchdog; add WRN
 *  when flushing caused by FIFO WD; fix function name for epilogue
 * 
 * 319   8/22/12 9:52p bandrews
 * SW7420-2314: added debug message
 * 
 * 318   8/22/12 8:54p bandrews
 * SW7420-2314: mute control now applies to a persistent startMute flag;
 *  unmute happens immediately
 * 
 * 317   8/10/12 7:40p bandrews
 * SW7346-826: NRT mode means that decoder rate is not necessarily
 *  consumption compatible
 * 
 * 316   7/30/12 3:44p vsilyaev
 * SW7425-3316: Don't call the video decoder HW specific functions inside
 *  the xxx_Generic functions
 * 
 * 315   7/27/12 7:34p hongtaoz
 * SW7425-3424: forced FNRT setting in case it's set before channel is
 *  opened;
 * 
 * 314   7/27/12 7:13p hongtaoz
 * SW7425-3424: fixed typo;
 * 
 * 313   7/27/12 7:02p hongtaoz
 * SW7425-3424: added FNRT support to video decoder;
 * 
 * FNRT_Demo/3   7/27/12 6:34p hongtaoz
 * SW7425-3424: move chunkDone callback to fnrtSettings;
 * 
 * FNRT_Demo/2   7/23/12 12:30p nilesh
 * SW7425-3424: Rebase from mainline
 * 
 * 312   7/25/12 6:24p bandrews
 * SW7125-1309: bandHold should not be enabled for input from live parser
 *  band
 * 
 * 311   7/23/12 11:56a erickson
 * SW7425-3316: call BXVD_GetHardwareCapabilities to reduce list of
 *  supportedCodecs[] based on HW capabilities
 * 
 * 310   7/20/12 2:51p erickson
 * SW7425-3316: add NEXUS_GetVideoDecoderCapabilities
 * 
 * 309   7/17/12 4:55p jtna
 * SW7425-3467: fix avd_monitor thread getting stuck in an infinite loop
 * 
 * FNRT_Demo/1   7/6/12 5:29p hongtaoz
 * SW7425-3424: add FNRT support to video decoder;
 * 
 * 308   7/5/12 5:18p bandrews
 * SW7231-876: fix warnings
 * 
 * 307   7/3/12 5:11p bandrews
 * SW7425-717: merge to main
 * 
 * 306   6/27/12 6:25p bandrews
 * SW7346-826: merge to main
 * 
 * SW7346-826/4   6/20/12 9:14p bandrews
 * SW7346-826: merge from main
 * 
 * 305   6/19/12 12:44p vsilyaev
 * SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
 * 
 * SW7346-826/3   6/20/12 8:39p bandrews
 * SW7346-826: clean up FIFO watchdog code
 * 
 * SW7346-826/2   6/19/12 8:52p bandrews
 * SW7346-826: if video stops consuming CDB, and is not paused or slow stc
 *  trick mode, just flush
 * 
 * SW7346-826/1   6/14/12 8:50p bandrews
 * SW7346-826: add video non-consumption condition detection and flush
 *  resolution
 * 
 * 304   6/11/12 1:37p erickson
 * SW7425-3172: use public ppb struct from xvd
 * 
 * 303   6/11/12 12:51p erickson
 * SW7425-3028: rework supportedCodecs[] to that array index is actually
 *  NEXUS_VideoCodec
 *
 * 302   6/4/12 11:23a vsilyaev
 * SW7420-2312: Removed leaked code from SW7425-3028
 *
 * 301   6/1/12 4:19p vsilyaev
 * SW7420-2312: Added reference counting of the StcChannel
 *
 * SW7425-717/1   6/11/12 7:40p bandrews
 * SW7425-717: fix bugs, add status to sync channel
 * 
 * 300   5/17/12 7:13p bandrews
 * SW7425-3061: fully reset deadlock detection when asked to zero fill
 *
 * 299   5/14/12 7:42p bandrews
 * SW7425-2264: merge to main
 *
 * SW7425-2264/2   5/7/12 7:32p bandrews
 * SW7425-2264: fixed build errors
 *
 * SW7425-2264/1   5/4/12 9:50p bandrews
 * SW7425-2264: first stab at low latency TSM mode impl
 *
 * 298   4/11/12 4:08p bandrews
 * SW7405-5612: merge to main
 *
 * SW7405-5612/3   4/11/12 3:52p bandrews
 * SW7405-5612: merge from main
 *
 * 297   3/20/12 5:35p erickson
 * SWDEPRECATED-3503: don't reset NEXUS_UserDataFormat on Start
 *
 * 296   3/15/12 9:21a erickson
 * SW7425-2607: use NEXUS_MAX_XVD_DEVICES
 *
 * 295   3/14/12 12:34p erickson
 * SW7401-4517: remove old code
 *
 * 294   3/13/12 4:42p erickson
 * SW7425-2607: use unvarying macros in API for binary compatibility
 *
 * 293   3/6/12 8:53a erickson
 * SW7425-2404: add NEXUS_VideoDecoderTrickState.maxFrameRepeat
 *
 * SW7405-5612/2   3/5/12 8:02p bandrews
 * SW7405-5612: merge from main
 *
 * 292   3/2/12 4:56p bandrews
 * SW7425-2276: merge to main
 *
 * 291   2/29/12 4:10p jtna
 * SW7425-2507: take XVD's default for SetVeryLateThreshold() in non-trick
 *  mode
 *
 * 290   2/29/12 10:08a erickson
 * SW7425-1680: add
 *  NEXUS_VideoDecoderExtendedSettings.earlyPictureDeliveryMode
 *
 * SW7425-2276/3   3/1/12 7:06p bandrews
 * SW7425-2276: improve gap detection; use correct placement for BXVD_PVR_
 *  function call; ensure all counters are 64 bit
 *
 * SW7425-2276/2   2/27/12 5:12p bandrews
 * SW7425-2276: call XDM directly, use pcrOffset handle properly
 *
 * SW7425-2276/1   2/23/12 9:08p bandrews
 * SW7425-2276: initial impl of gap detection
 *
 * SW7405-5612/1   2/20/12 8:39p bandrews
 * SW7405-5612: initial implementation
 *
 * 289   2/20/12 7:58p bandrews
 * SW7550-691: merge to main
 *
 * 288   1/19/12 11:51a erickson
 * SW7346-629: fix crc fifo allocation
 *
 * 287   1/19/12 9:47a erickson
 * SW7346-629: test if decoder handle exists in
 *  NEXUS_VideoDecoder_P_SetCrcFifoSize
 *
 * 286   1/16/12 12:25p erickson
 * SW7346-629: fix platforms with no AVD CRC
 *
 * 285   1/16/12 11:23a erickson
 * SW7346-629: add NEXUS_VideoDecoder_GetCrcData
 *
 * 284   12/13/11 9:45p bandrews
 * SW7425-1772: merge to main
 *
 * SW7425-1772/2   12/12/11 10:15p bandrews
 * SW7425-1772: merge from main
 *
 * 283   12/7/11 11:04a erickson
 * SW7420-2182: initial state of videoDecoder->last_field.bMute is true,
 *  not 0
 *
 * SW7425-1772/1   12/12/11 10:02p bandrews
 * SW7425-1772: add priority scheme to stc channel
 *
 * 282   10/24/11 2:05p erickson
 * SW7425-1264: update NEXUS_TsmMode_eSimulated
 *
 * 281   10/12/11 10:20a erickson
 * SW7425-1264: add NEXUS_TsmMode_eSimulated
 *
 * 280   10/3/11 5:04p dlwin
 * SW7425-1387: Fix for Coverity: 35276
 *
 * 279   10/3/11 10:15a erickson
 * SW7425-1367: move pictureCoding to NEXUS_VideoDecoderStatus so we get
 *  fewer callbacks
 *
 * 278   9/26/11 3:00p erickson
 * SW7408-304: call Nexus_Core_P_Img_Destroy
 *
 * 277   9/19/11 11:22a vsilyaev
 * SW7405-5478: Fixed warning
 *
 * 276   9/14/11 2:32p erickson
 * SW7405-5478: added error recovery to NEXUS_VideoDecoder_P_Start_priv
 *
 * 275   9/2/11 6:23p vsilyaev
 * SW7425-1103: Added BDBG_CWARNING
 *
 * 274   8/19/11 7:38p hongtaoz
 * SW7425-767: disable XDM jitter tolerance improvement at NRT mode;
 *
 * 273   8/17/11 12:56p vsilyaev
 * SW7425-1086: Relaxed constraints on BXVD_Orientation
 *
 * 272   8/12/11 4:17p jtna
 * SW7425-1070: merge video primer ptsStcDiffCorrection feature
 *
 * 271   8/8/11 11:29a erickson
 * SW7425-1066: NEXUS_VideoDecoderOpenSettings.heap only applies to per-
 *  channel picture buffer heap
 *
 * 270   8/2/11 4:54p erickson
 * SW7420-1999: set status.firstPtsPassed
 *
 * 269   7/29/11 3:32p gmohile
 * SW7425-533 : Check xvd standby return code
 *
 * 268   7/28/11 1:41p gmohile
 * SW7425-533 : Add XVD standby
 *
 * 267   7/26/11 2:24p erickson
 * SW7405-5424: add
 *  NEXUS_VideoDecoderExtendedSettings.ignoreNumReorderFramesEqZero
 *
 * 266   7/22/11 10:15a gmohile
 * SW7125-1014 : Fix warning
 *
 * 265   7/21/11 4:14p gmohile
 * SW7125-1014 : Rework power management
 *
 * 264   7/21/11 11:39a nickh
 * SW7422-404: Security system test
 *
 * 263   7/11/11 2:26p nickh
 * SW7422-404: Security system test
 *
 * 262   6/28/11 4:34p bandrews
 * SW7425-705: merge to main
 *
 * SW7425-705/1   6/20/11 9:48p bandrews
 * SW7425-705: Disable multi-window sync when source is NRT
 *
 * 261   6/27/11 4:22p bandrews
 * SW7425-654: merge to main
 *
 * 260   6/23/11 2:52p hongtaoz
 * SW7425-466: add 3x decodes support;
 *
 * SW7425-654/1   6/21/11 9:33p bandrews
 * SW7425-654: Support use of SW STC offset and transmission of offset
 *  between paired STC channels
 *
 * 259   6/9/11 6:44p vsilyaev
 * SW7425-654: Added support for non realtime video decode
 *
 * 258   6/3/11 12:17p erickson
 * SW7425-466: fix mosaic decode assignment of XDM resources. remove
 *  unnecessary 7405 channel skipping.
 *
 * 257   5/19/11 6:00p hongtaoz
 * SW7425-466: check re-opened decoder to pass the open test;
 *
 * 256   5/19/11 3:51p hongtaoz
 * SW7425-466: set/clear xdm usage flags at channel open/close time;
 *
 * 255   5/17/11 2:54p hongtaoz
 * SW7425-466: change the default avdMapping for all non-orthorgonal
 *  chips;
 *
 * 254   5/12/11 4:47p hongtaoz
 * SW7425-466: add backward compatible default avd mapping; future non-
 *  orthorgonal chips should override mfd/avd mappings in platform code;
 *
 * 253   5/12/11 3:49p hongtaoz
 * SW7425-466: rework avd/mfd/decoder/xdm mapping;
 *
 * SW7425-466/3   5/12/11 3:17p hongtaoz
 * SW7425-466: removed deprecated feature, linked decoder;
 *
 * SW7425-466/2   5/12/11 12:04p hongtaoz
 * SW7425-466: added the XDM/decoder mapping to be scalable;
 *
 * SW7425-466/1   5/11/11 12:59p hongtaoz
 * SW7425-466: rework avd/mfd/decoder channel mapping;
 *
 * 252   5/10/11 1:55p erickson
 * SW7346-179: fix warnings
 *
 * 251   5/4/11 11:38a vsilyaev
 * SW7346-179, SW7425-416: Use different types for the video decoder and
 *  display orientation
 *
 * 250   4/22/11 3:27p vsilyaev
 * SW7425-416: Removed 3DLeft/3DRight
 *
 * 249   4/21/11 11:39a jtna
 * SW7422-170: better handling of enhancementRave during Flush()
 *
 * 248   4/19/11 2:14p erickson
 * SW7335-1168: rework NEXUS_PidChannelScramblingSettings.raveContext to
 *  require it be already started by decoder
 *
 * 247   4/18/11 10:21a erickson
 * SW7335-1168: verify that RAVE context is disabled before attaching
 *
 * 246   4/15/11 10:44a erickson
 * SWDTV-6630: add NEXUS_VideoDecoderExtendedSettings.treatIFrameAsRap
 *
 * 245   4/14/11 3:30p jtna
 * SWDTV-6321: fix build warnings for 2.6.37 kernel
 *
 * 244   4/14/11 10:42a vsilyaev
 * SW7422-333: Use proper default configuration for
 *  NEXUS_VideoDecoder_Open
 *
 * 243   4/1/11 10:17a randyjew
 * SW7422-333:Remove MVC SVC support in mosaic mode.
 *
 * 242   3/23/11 2:48p vsilyaev
 * SW7422-14: Added status for the enhanced layer FIFO
 *
 * 241   3/17/11 4:42p erickson
 * SW7346-120: add NEXUS_StcChannel_VerifyPidChannel
 *
 * 240   3/17/11 1:17p erickson
 * SW7422-333: rely on #define NEXUS_SVC_MVC_SUPPORT in
 *  nexus_platform_features.h
 *
 * 238   3/7/11 11:03a jtna
 * SW7422-304: add 3D SVC decode support
 *
 * 237   3/1/11 10:21a erickson
 * SW7335-1168: add NEXUS_VideoDecoder_DetachRaveContext
 *
 * 236   2/25/11 2:52p gmohile
 * SW7408-210 : Deprecate use of NEXUS_POWER_STANDBY
 *
 * 235   2/23/11 11:45a hongtaoz
 * SW7422-183, SW7425-40: enable decoder's enhancement rave only if valid;
 *
 * 234   2/22/11 4:16p jtna
 * SW7422-274: add NEXUS_VideoDecoderOpenSettings.supportedCodecs[]
 *
 * 233   2/22/11 2:38p jtna
 * SW7422-183: enable enhancement RAVE context on flush
 *
 * 232   2/22/11 10:22a erickson
 * SW7422-274: add NEXUS_VideoDecoderOpenSettings.supportedCodecs[]
 *
 * 231   2/15/11 5:33p gmohile
 * SW7408-210 : Merge initial version of module standby
 *
 * 230   2/15/11 2:10p erickson
 * SW7420-1491: add BDBG_ERR because of avd_monitor env variable
 *  dependency
 *
 * SW7550-691/1   3/1/11 10:05p bandrews
 * SW7550-691: ensure astm knows about lifecycle events even when it is
 *  stopped
 *
 * 229   1/4/11 10:10a erickson
 * SW7400-2842: remove NEXUS_VIDEO_DECODER_SINGLEDECODE
 *
 * 228   12/30/10 2:49p jrubio
 * SW7344-9: add HD pip for 7344
 *
 * 227   12/29/10 2:06p katrep
 * SW7231-4:7231/7346 enable HD  PIP
 *
 * 226   12/27/10 4:23p erickson
 * SW7405-5051: add NEXUS_VideoDecoderSettings.horizontalOverscanMode
 *
 * 225   12/17/10 4:59p vsilyaev
 * SW7422-154: Added custom sourced 3D configuration
 *
 * 224   12/13/10 4:07p erickson
 * SW35230-179: add NEXUS_VideoDecoderStatus.numBytesDecoded
 *
 * 223   12/10/10 5:06p erickson
 * SW35230-2209: add NEXUS_VideoDecoderSettings.scanMode
 *
 * 222   12/9/10 8:05p vsilyaev
 * SW7425-40: Added new types for the video encoder
 *
 * 221   12/7/10 2:36p jtna
 * SW7408-190: remove NEXUS_PowerManagement_SetCoreState calls. XVD power
 *  management is handled in magnum
 *
 * 220   12/6/10 3:39p erickson
 * SW3556-1213: added enums for still/moving content interpolation modes
 *
 * 219   11/29/10 11:33a vsilyaev
 * SW7422-65: Fixed mapping of SVC/MVC video codecs
 *
 * 218   11/19/10 10:40a erickson
 * SW7335-826: added NEXUS_VideoDecoderStatus.pictureTag
 *
 * 217   11/16/10 4:17p jtna
 * SW7405-4997: Add 3DTV status timer
 *
 * 216   11/3/10 1:50p erickson
 * SW7422-79: remove assumption that BAVC_SourceId_eMpeg0 == 0
 *
 * 215   11/3/10 1:49p erickson
 * SW7422-79: fix default mfdMapping for 7405 PIP
 *
 * 214   11/1/10 5:23p vsilyaev
 * SW7422-65: Added pidChannel for enhancement video data
 *
 * 213   10/22/10 11:16a erickson
 * SW7420-1135: add NEXUS_VideoDecoderMosaicSettings for runtime changes
 *  of maxWidth/maxHeight for mosaic decodes
 *
 * 212   10/19/10 12:49p nickh
 * SW7422-79: Add flexible mapping scheme of video decoders and MFD
 *  sources
 *
 * 211   10/18/10 2:26p jtna
 * SW35230-1558: add B_PERF_LINUX support to channel change report
 *
 * 210   10/14/10 6:38p jtna
 * SW7405-4817: rename struct to s3DTVChangedCallback
 *
 * 209   10/11/10 12:00p vsilyaev
 * SW7422-65: SW7468-295: Use generic Api for SetPlaybackSettings
 *
 * 208   10/7/10 3:54p vsilyaev
 * SW7422-65: Moved XVD calls to XVD specific function
 *
 * 207   10/1/10 12:05p vsilyaev
 * SW7422-65: Fixed user data handling
 *
 * 206   9/29/10 3:30p vsilyaev
 * SW7422-65: User per AVD core heap sizes
 *
 * 205   9/29/10 3:17p jtna
 * SW7405-4723: remove unneeded primer profiling code
 *
 * 204   9/29/10 1:26p vsilyaev
 * SW7422-65: Fixed broken H264 decode
 *
 * 203   9/28/10 6:44p vsilyaev
 * SW7422-65: Added SVC types
 *
 * 202   9/27/10 4:24p jtna
 * SW7405-3998: add back BXVD_EnableInterrupt call for
 *  BXVD_Interrupt_ePictureExtensionData that was inadvertently removed in
 *  /main/196
 *
 * 201   9/24/10 11:14a mward
 * SW7468-129: Fix build of  NEXUS_VIDEO_DECODER_SINGLEDECODE.
 *
 * 200   9/22/10 3:40p mward
 * SWDEPRECATED-3811: Eliminate warning for unused strings with DEBUG=n.
 *
 * 199   9/15/10 5:48p jerrylim
 * SW35230-627: Add dual HD decoding support
 *
 * 198   9/9/10 4:39p erickson
 * SW7468-129: pass open settings to NEXUS_VideoDecoder_P_GetRaveSettings
 *
 * 196   9/8/10 1:15p vsilyaev
 * SW7468-129: After merge fixes
 *
 * 195   9/8/10 12:25p vsilyaev
 * SW7468-129: Added video decoder on ZSP
 *
 * 194   9/3/10 10:48a erickson
 * SWDEPRECATED-3811: improve proc print
 *
 * 193   9/1/10 3:05p erickson
 * SWDEPRECATED-3811: rename module according to coding convention
 *
 * 192   8/25/10 4:19p erickson
 * SW7405-4637: userdata.buf must be a cached memory pointer, otherwise
 *  the kernel mode proxy conversion to user mode cached address will lead
 *  to cache coherency problems.
 *
 * 191   8/10/10 1:12p erickson
 * SW3548-3022: switch to UDP (userdata parser) for userdata parsing
 *
 * 190   7/23/10 4:09p jhaberf
 * SW35230-627: use macro instead of chip list for video decode dual
 *  feeder support
 *
 * 189   7/19/10 6:03p jhaberf
 * SW35230-627: Adding 35230 to list of chips supporting dual decode
 *
 * 188   7/7/10 11:44a mward
 * SW7400-2842: Add an option NEXUS_VIDEO_DECODER_SINGLEDECODE=y for XVD
 *  singledecode on 97400.
 *
 * 187   7/6/10 10:58a erickson
 * SW7405-4047: implement jail_xvd/vdc/xpt/rap using
 *  BMRC_Monitor_JailByFilename
 *
 * 186   6/29/10 2:26p jtna
 * SW7405-3998: update NEXUS_VideoDecoder_Get3DTVStatus to return codec-
 *  generic 3DTV status
 *
 * 185   6/16/10 9:58a erickson
 * SW7405-4378: remove NEXUS_VideoDecoderStatus.coded.width/height using
 *  NEXUS_VideoDecoderStreamInformation, with additional logic for
 *  beginning-of-decode conditions
 *
 * 184   6/2/10 5:09p erickson
 * SW7405-4445: RAVE band hold should be false for playback with PCR TSM.
 *
 * 183   6/1/10 6:28p vsilyaev
 * SW7405-4399:  Don't dereference NULL pointer
 *
 * 182   5/19/10 10:37p bandrews
 * SW7325-706: correct check in comment
 *
 * 181   5/19/10 10:17p bandrews
 * SW7325-706: on overflow, video dumps any pcr offsets it had, need to
 *  force a request stc in nexus
 *
 * 180   5/12/10 12:28p jtna
 * SW7405-3998: merge
 *
 * SW7405-3998/2   5/12/10 12:18p jtna
 * SW7405-3998: update SEI function/variable naming
 *
 * SW7405-3998/1   5/4/10 5:13p jtna
 * SW7405-3998: added SEI frame packing arrangement message callback
 *
 * 179   4/26/10 3:10p erickson
 * SW3556-1090: added
 *  NEXUS_VideoDecoderExtendedSettings.zeroDelayOutputMode
 *
 * 178   4/19/10 11:23a erickson
 * SW35230-178: add codec profile and level information to
 *  NEXUS_VideoDecoderStatus
 *
 * 177   3/25/10 10:48a gmohile
 * SW7400-2729 : Reassign picdata ready callback using XDM apis
 *
 * 176   3/23/10 1:30p erickson
 * SW7405-4051: add explicit BXVD_EnableInterrupt for
 *  BXVD_Interrupt_eDecodeError
 *
 * 175   3/18/10 3:50p gmohile
 * SW7405-2993 : Merge XDM changes
 *
 * 174   3/10/10 3:00p erickson
 * SW7405-3965: deprecrate NEXUS_VideoDecoderModuleSettings.numDecodes,
 *  etc. clean up avd/xvd channel/mfd index assignment in
 *  NEXUS_VideoDecoder_P_Open.
 *
 * 173   3/9/10 2:22p erickson
 * SW7400-2712: bounds check for XVD and XVD channel
 *
 * 172   2/26/10 1:24p erickson
 * SW3556-1058: added
 *  NEXUS_VideoDecoderExtendedSettings.ignoreDpbOutputDelaySyntax
 *
 * 171   2/16/10 11:57a jtna
 * SW3556-1003: add comment on mapping of timestamp ordering
 *
 * 170   2/12/10 5:10p jtna
 * SW3556-1003: NEXUS_VideoDecoderTimestampMode_eDisplay and eDecode were
 *  swapped
 *
 * 169   2/10/10 10:36a erickson
 * SW7405-3891: rename macro to B_USE_NEW_XVD
 *
 * 168   2/8/10 5:31p jtna
 * SW3556-1003: added option to disable reordering of video frames
 *
 * 167   2/2/10 7:26p bandrews
 * SW7405-3774: add validity flag for delay
 *
 * 166   2/1/10 8:08p bandrews
 * SW7405-3774: add mute control support to sync channel
 *
 * 165   1/12/10 2:38p erickson
 * SW3556-980: added NEXUS_VideoDecoderStartSettings.timestampMode
 *
 * 164   12/30/09 5:19p erickson
 * SW7405-3276: add NEXUS_VideoDecoderSettings.manualPowerState and
 *  NEXUS_VideoDecoder_SetPowerState
 *
 * 163   12/29/09 5:20p mphillip
 * SW7125-136: Merge additional security changes
 *
 * 162   12/9/09 3:10p erickson
 * SW7405-3394: refactor mosaic mode API
 *
 * 161   12/7/09 5:30p erickson
 * SW3548-2645: add
 *  NEXUS_VideoDecoderStartSettings.progressiveOverrideMode
 *
 * 160   11/24/09 12:59p erickson
 * SW7420-457: merge security changes
 *
 * 159   11/20/09 2:25p erickson
 * SW7405-3315: move BXVD_SetHwPcrOffsetEnable below where cfg.bPlayback
 *  is set. remove unnecessary setting of cfg.bPlayback. reduce # of
 *  BCHP_CHIP tests.
 *
 * 158   11/17/09 12:30p mward
 * SW7405-3257: 7400 does not have enough timers for XVD for debug in
 *  DUAL_OUTPUT mode, even limited to first xvd. Add env
 *  "xvd_debug_timer".
 *
 * 157   11/16/09 3:12p ahulse
 * SW7405-2279: Updated call to BXVD_SetClipTime to use
 *  BXVD_ClipTimeType_eClipStartOnly
 *
 * 156   11/11/09 2:32p mward
 * SW7405-3257: Only give first XVD instance a TMR handle for debug.  Not
 *  enough timers for both to have it (on 7400).
 *
 * 155   11/4/09 2:29p erickson
 * SW7405-3335: bounds check XVD enums for mosaic mode
 *
 * 154   11/2/09 11:15a erickson
 * SW7405-3315: remove playbackOverride. use NEXUS_StcChannelMode instead.
 *
 * 153   10/28/09 1:34p erickson
 * SW7405-3287: fix condition for setting mosaic-mode setPcrOffset
 *
 * 152   10/26/09 2:09p erickson
 * SW7405-3089: update display information for all mosaics. also, separate
 *  the private API from the internal implementation so we don't get
 *  excessive calls.
 *
 * 151   10/26/09 12:28p erickson
 * SW7405-3287: add support for sw-based pcr offset for mosaic playback
 *  TSM support
 *
 * 150   10/23/09 10:53a erickson
 * SW7405-3287: remove NEXUS_VideoDecoder_P_VerifyTimebase check. too many
 *  exceptions to make this a meaningful check.
 *
 * 149   10/20/09 1:43p erickson
 * SW7405-3257: give XVD a TMR handle for debug
 *
 * 148   10/15/09 12:57p erickson
 * SW7405-3005: add NEXUS_VideoDecoderStatus.numDecodeOverflows
 *
 * 147   10/14/09 10:26a erickson
 * SW7405-3197: NEXUS_VideoDecoderStatus.numDisplayUnderflows reports # of
 *  pictures underflows, so we should only use XVD's
 *  uiVsyncUnderflowCount, not ulUnderflowCount.
 *
 * 146   10/6/09 11:06a erickson
 * SW7405-3127: added NEXUS_VideoDecoderOpenSettings.itbFifoSize
 *
 * 145   10/5/09 12:15p erickson
 * SW7405-2067: add NEXUS_VideoDecoder_P_DisableFwVerification. rename
 *  some regver structures.
 *
 * 144   9/30/09 6:48p vsilyaev
 * SW7405-2732: Added fragmented mode, for the decoder side it set using
 *  Reoorder and filtering I frames only
 *
 * 143   9/24/09 3:34p gmohile
 * SW7403-890 : FIx 7401/03 build
 *
 * 142   9/10/09 12:21p jtna
 * SW7405-3003: add video primer profiling
 *
 * 141   9/9/09 6:38p ahulse
 * SW7405-2724: Register Clipstart interrupt with FirstPtsPassed_isr
 *
 * 140   9/8/09 4:18p erickson
 * SW7420-283: add support for linked mosaic decoders
 *
 * 139   8/5/09 10:02a gmohile
 * PR 56512 : Merge 7401/03 support ot mainline
 *
 * 138   7/30/09 5:39p erickson
 * PR57220: improve BDBG_CASSERT for NEXUS_VideoFrameRate
 *
 * 137   7/30/09 5:13p erickson
 * PR57211: added NEXUS_VideoDecoderOpenSettings.excessDirModeEnabled
 *
 * 136   7/30/09 11:58a erickson
 * PR54880: add pcr_offset support for fcc with TSM
 *
 * 135   7/24/09 9:09a erickson
 * PR56967: added NEXUS_VideoDecoderSettings.decodeError callback
 *
 * 134   7/23/09 10:42a katrep
 * PR57010: fixed for AVD code jail code
 *
 * 134   7/23/09 10:37a katrep
 * PR57010: Fixed issue with memc arc .
 *
 * 132   7/10/09 12:35p erickson
 * PR56701: set BXVD_FrameRateDetectionMode_eStable as always-on feature.
 *  document XVD frame rate handling.
 *
 * 131   7/9/09 11:20a erickson
 * PR35457: add DBG
 *
 * 130   7/8/09 11:57a vsilyaev
 * PR 55989: Added EOF handling for the OTF PVR
 *
 * 129   6/26/09 1:05a mphillip
 * PR54926: Add /proc/videodecoder
 *
 * 128   6/25/09 2:56p erickson
 * PR45902: add nexus-based dynamic power management for video decoder
 *
 * PR55297/1   6/9/09 2:05p nilesh
 * PR55297: Changed bDropField default to match XVD. The default is now
 *  false.
 *
 * 126   6/2/09 12:15p erickson
 * PR55639: improve accuracy of VideoDecoder status and stream information
 *  by adjusting timing and field selection
 *
 * 125   5/22/09 5:21p vsilyaev
 * PR 55376 PR 52344: Added option to enable processing of AVC(H.264)
 *  Level 5.1 video
 *
 * 124   5/20/09 3:30p erickson
 * PR55042: refactor active format description support. add callback and 8
 *  bit status value.
 *
 * 123   5/20/09 9:21a erickson
 * PR54880: refactor RAVE configuration, prep work for video primer
 *
 * 122   5/15/09 2:06p erickson
 * PR55094: add NEXUS_VideoDecoderStatus.numWatchdogs
 *
 * 121   5/8/09 12:17p erickson
 * PR54844: mosaicMode must be tracked per channel, not per device.
 *
 * 120   5/6/09 11:13a erickson
 * PR54844: additional condition on mosaic decoder close
 *
 * 119   5/5/09 4:18p erickson
 * PR54844: handle mosaic decoders which aren't the main mosaic decoder
 *  with different logic
 *
 * 118   4/20/09 9:22a erickson
 * PR53868: added NEXUS_VideoDecoderModuleSettings.hostAccessibleHeapIndex
 *
 * 117   4/16/09 4:15p vsilyaev
 * PR 50887: Fixed typo
 *
 * 116   4/16/09 9:36a erickson
 * PR53868: change FW heap from heap[0] to heap[avdHeapIndex[0]]
 *
 * 115   4/6/09 9:07a erickson
 * PR53868: add NEXUS_VideoDecoderModuleSettings.avdHeapIndex[]
 *
 * 114   4/1/09 12:54p erickson
 * PR53653: get default discard threshold from XVD instead of duplicating
 *  values in nexus
 *
 * 113   3/31/09 7:29p vsilyaev
 * PR 50887: OTFPVR support for the Nexus video decoder
 *
 * 112   3/30/09 10:36a erickson
 * PR50748: support the secure heap using module settings handle
 *
 * 111   3/26/09 10:05a nickh
 * PR53588: Add dual decode support for 7420
 *
 * 110   3/25/09 10:05a erickson
 * PR53384: remove duplicate function, added comments
 *
 * 109   3/24/09 5:00p erickson
 * PR53384: added avd sw uart
 *
 * 108   3/16/09 3:32p erickson
 * PR53229: added NEXUS_VideoDecoderStatus.cabacBinDepth
 *
 * 107   3/9/09 2:34p erickson
 * PR50748: use NEXUS_Heap_GetStatus to determine UMA/non-UMA
 *
 * 106   2/24/09 5:07p erickson
 * PR51134: default dropFieldMode to true to match underlying XVD defaults
 *
 * 105   2/23/09 2:56p erickson
 * PR52404: added NEXUS_VideoDecoder_IsCodecSupported
 *
 * 104   2/13/09 12:44a erickson
 * PR51134: added NEXUS_VideoDecoderSettings.dropFieldMode
 *
 * 103   2/11/09 2:41p mward
 * PR52005:  Handle the two XVDs for 7400 for both PIP and mosaic cases.
 *
 * 102   2/10/09 5:27p mward
 * PR52005: For 7400, use standard indexes for channel and xvd devices .
 *
 * 101   2/9/09 11:05a jrubio
 * PR51629: add 7336 support
 *
 * PR51105/2   2/5/09 3:35p erickson
 * PR51105: update branch
 *
 * 99   1/30/09 9:09a erickson
 * PR51694: added NEXUS_VideoDecoderStatus.ptsStcDifference
 *
 * 98   1/30/09 5:09a bandrews
 * PR51692: register for interrupt and copy status info
 *
 * 97   1/27/09 11:30a erickson
 * PR50367: added memset after Close
 *
 * 96   1/27/09 9:56a erickson
 * PR51468: refactor to reduce exposed globals
 *
 * 95   1/26/09 10:55a erickson
 * PR51468: global variable naming convention
 *
 * 94   1/26/09 9:36a erickson
 * PR51273: make PR 42265 workaround conditional and defaulted off
 *
 * 93   1/20/09 4:59p bandrews
 * PR51021: Change warning to message
 *
 * 92   1/19/09 5:22p bandrews
 * PR51021: merge
 *
 * PR51021/2   1/16/09 4:13p bandrews
 * PR51021: Updated to separate permission from enable
 *
 * PR51021/1   1/16/09 12:18a bandrews
 * PR51021: First stab at cleaning up ASTM playback blockage
 *
 * 91   1/14/09 5:19p erickson
 * PR51121: increase userDataBufferSize default to 16K
 *
 * 90   1/13/09 4:24p erickson
 * PR51016: NULL out StartSettings before Stop returns
 *
 * 89   1/8/09 9:37p erickson
 * PR50757: added NEXUS_VideoDecoderStartSettings.frameRate for default
 *  setting
 *
 * 88   12/29/08 2:35p erickson
 * PR50749: fix mosaic mode. multiple OpenChannel's required when one
 *  VideoDecoder is connected to a VideoWindow.
 *
 * 87   12/22/08 1:26p yili
 * PR42660:IPTV Integration
 *
 * 86   12/16/08 8:05a erickson
 * PR50236: assert when VideoDecoder is closed with dangling connector.
 *  the app must be fixed.
 *
 * 85   12/15/08 10:34a erickson
 * PR50236: improve BDBG_ERR's
 *
 * 84   12/12/08 4:19p jtna
 * PR49924: add support for
 *  NEXUS_VideoDecoderSettings.preferredUserDataFormat
 *
 * 83   12/10/08 4:41p erickson
 * PR49930: add NEXUS_VideoDecoderPlaybackSettings for playback
 *  accurateSeek algorithm
 *
 * 82   12/9/08 6:22p erickson
 * PR49923: added firstPtsPassed callback
 *
 * 81   12/8/08 4:30p bandrews
 * PR41322: auto start astm from decoder start signal
 *
 * 80   12/8/08 11:16a erickson
 * PR49930: add NEXUS_VideoDecoder_SetStartPts
 *
 * 79   12/2/08 5:52p nickh
 * PR48963: Add 7420 support so that the AVD0 Picture heap is allocated
 *  from MEMC0 and AVD1 Picture heap is allocated from MEMC1
 *
 * 78   12/1/08 11:22a erickson
 * PR49672: fixed firstPts callback
 *
 * 77   12/1/08 9:40a erickson
 * PR49500: add NEXUS_VideoDecoderStatus.avdStatusBlock
 *
 * 76   11/26/08 12:39p erickson
 * PR49361: added NEXUS_VideoDecoderStatus.numPicturesReceived
 *
 * 75   11/18/08 3:07p maivu
 * PR 47967: undo 3548/3556 B0 AVD/MEMC hang
 *
 * 74   11/18/08 12:57p erickson
 * PR48155: extend tsm deadlock algo to check more specific case
 *
 * 73   11/18/08 11:55a erickson
 * PR48155: check for static fifo, then call stc channel to see if we have
 *  a TSM deadlock. if so, flush to recover.
 *
 * 72   11/9/08 11:16a maivu
 * PR47967: Added run-time option to force i-only for "iframe_only" and
 *  "iframe_mpeg2"
 *
 * 71   11/5/08 11:11a erickson
 * PR47967: added AVD/MEMC 3548/3556 B0 workaround for DivX
 *
 * 70   11/4/08 4:23p erickson
 * PR47030: add NEXUS_VideoDecoder_GetOpenSettings
 *
 * 69   10/30/08 8:53p erickson
 * PR47030: use NEXUS_HeapHandle for shared analog/digital heap
 *
 * 68   10/24/08 2:05p erickson
 * PR48290: fix logic bug in NEXUS_VideoDecoder_P_SetSettings
 *
 * 67   10/21/08 6:21p katrep
 * PR43187: Report fifo size and depth on the rave context even if the
 *  decoder is not started.
 *
 * 66   10/17/08 4:24p erickson
 * PR47030: added ERR's for out of sequence calls
 *
 * 65   10/17/08 12:23p erickson
 * PR47030: fix misspelling
 *
 * 64   10/17/08 9:16a erickson
 * PR47030: open and close the XVD channel when the VideoDecoder is
 *  connected or disconnected to a VideoWindow
 *
 * 63   10/9/08 2:54p erickson
 * PR47608: fix NEXUS_VideoDecoderStatus.tsm
 *
 * 62   10/8/08 12:51a erickson
 * PR47666: add firstPts callback
 *
 * 61   9/23/08 9:36a erickson
 * PR47231: added NEXUS_VideoDecoderStartSettings.prerollRate
 *
 * 60   9/9/08 10:04a erickson
 * PR45940: use NEXUS_Rave_GetCdbValidPointer_isr to determine if
 *  fifoEmpty should be fired
 *
 * 59   9/2/08 10:27p katrep
 * PR46457: Configure nexus features at the run time based on board strap
 *  options
 *
 * 58   9/2/08 1:51p erickson
 * PR45940: added NEXUS_VideoDecoderSettings.fifoEmpty and
 *  NEXUS_VideoDecoderStatus.tsm
 *
 * 57   8/28/08 11:09a erickson
 * PR45875: add NEXUS_VideoDecoderErrorHandling
 *
 * 56   8/21/08 3:17p katrep
 * PR45527: NEXUS_CONFIG_IMAGE implementation for runtime FW download
 *
 * 55   8/15/08 2:18p erickson
 * PR45732: fix NEXUS_VideoDecoderStatus.numDecodeDrops
 *
 * 54   8/14/08 5:25p katrep
 * PR45674: Fix compiiler warning in kernel mode non debug builds
 *
 * 53   7/31/08 11:36a erickson
 * PR45221: merge
 *
 * PR45221/1   7/31/08 1:22p dyzhang
 * PR45221: Segement fault in BRAP_DSPCHN_P_Stop when switching digital
 *  channel. channelChangeReportEventHandler should be unregistered before
 *  destory channelChangeReportEvent when closing decoder.
 *
 * 52   7/30/08 1:32p erickson
 * PR44919: update comments
 *
 * 51   7/30/08 10:02a erickson
 * PR45197: convert from module settings maxDecodeFormat to max
 *  width/height needed for XVD with mosaic capabilities
 *
 * 50   7/25/08 4:13p jtna
 * PR45093: defer channel change report using event
 *
 * 49   7/23/08 10:10a erickson
 * PR45075: check return code
 *
 * 48   7/17/08 5:24p jgarrett
 * PR 42642: Adding fifoThreshold
 *
 * 47   7/14/08 12:13p erickson
 * PR42739: added playbackOverride option for IPTV. fixed
 *  additionalPtsOffset logic.
 *
 * PR42739/1   7/10/08 10:27p ssood
 * PR42739: Adding support to absorb high jitter for Live IP Channels
 *
 * 46   7/11/08 5:07p jtna
 * PR42924: Add channel change measurements functionality
 *
 * 45   6/30/08 1:46p erickson
 * PR44165: impl NEXUS_CORE_NUM_SECURE_HEAPS
 *
 * 44   6/26/08 1:58p erickson
 * PR43541: allow StcChannel assignment to XVD to be swapped. also allow
 *  for mosaic mode timebase assignment.
 *
 * 43   6/18/08 5:39p erickson
 * PR36808: move syncSettings.startCallback to before BXVD_StartDecode.
 *  null out syncStatus to force format change callbacks.
 *
 * 42   6/17/08 7:59a erickson
 * PR43222: add divx still support
 *
 * 41   6/10/08 7:20p bandrews
 * PR42265: Added 120 ms delay for MPEG-2 video streams
 *
 * 40   5/20/08 3:42p erickson
 * PR42128: added dataReadyCallback to ExtendedSettings
 *
 * 39   5/16/08 11:04a erickson
 * PR42751: centralize logic for calling BXVD_SetVideoDisplayMode so that
 *  all aspects can be considered in one spot
 *
 * 38   5/9/08 3:04p erickson
 * PR42128: provide extended status information
 *
 * 37   5/8/08 2:48p erickson
 * PR42438: derive NEXUS_VideoInputStatus.format for digital decode using
 *  common function
 *
 * 36   5/5/08 4:54p jrubio
 * PR42439: remove HEAP defines, you must pass in heap sizes for xvd
 *
 * 35   5/5/08 2:01p erickson
 * PR40977: check for an illegally shared timebase when starting video
 *  decode
 *
 * 34   4/28/08 4:10p erickson
 * PR41533: unify EIA-608 and EIA-708 closed caption data capture in
 *  NEXUS_VideoInput_ReadClosedCaption
 *
 * 33   4/25/08 10:54a erickson
 * PR41968: added NEXUS_VideoDecoder_Reset
 *
 * 32   4/18/08 1:53p erickson
 * PR41533: impl EIA-708 parsing and capture
 *
 * 31   4/17/08 4:38p erickson
 * PR41533: rework NEXUS_VideoDecoder_GetUserDataBuffer to use
 *  NEXUS_UserDataHeader before payload
 *
 * 30   4/11/08 4:23p erickson
 * PR41618: default astm.settings.enableTsm = true
 *
 * 29   4/7/08 4:10p erickson
 * PR32888: added NEXUS_CRC_CAPTURE
 *
 * 28   4/7/08 10:16a erickson
 * PR37893: convert to XPT PI sw rave
 *
 * 27   4/2/08 5:36p bandrews
 * PR40090: Added check for astm enabled before disabling tsm on video
 *
 * 26   3/28/08 10:41a vsilyaev
 * PR 40818: Fixed 7401 build
 *
 * 25   3/27/08 6:42p vsilyaev
 * PR 40818: Added support for shared heap
 *
 * 24   3/26/08 3:38p vsilyaev
 * PR 40862: Added code to propagate display framerate to the decoder
 *
 * 23   3/25/08 5:30p bandrews
 * PR40090: added ASTM compile flag
 *
 * 22   3/25/08 12:55p erickson
 * PR40850: add more status
 *
 * 21   3/24/08 9:22p bandrews
 * PR40090: astm private settings
 *
 * 20   3/24/08 12:01p erickson
 * PR39000: added NEXUS_VideoDecoder_FlushUserdata. fixed userdata memcpy.
 *
 * 19   3/14/08 7:07p katrep
 * PR40514:Add support for AVS video decode
 *
 * 17   3/10/08 11:29a gmullen
 * PR40425: Ported over fix from IPTV branch
 *
 * 16   3/5/08 4:58p erickson
 * PR40307: add no_watchdog env option
 *
 * 15   3/4/08 5:18p jrubio
 * PR40169: add 7325/7335
 *
 * 14   2/28/08 10:41p vsilyaev
 * PR 40103: Added interfaceHandle and settings for the
 *  NEXUS_IsrCallbackCreate
 *
 * 13   2/27/08 3:33p erickson
 * PR38888: complete VideoDecoder status
 *
 * 12   2/27/08 11:33a erickson
 * PR35457: document XVD CDB/ITB sizes in part
 *
 * 11   2/26/08 10:03a erickson
 * PR39934: added SW RAVE support into Transport
 *
 * 10   2/25/08 3:01p erickson
 * PR39786: update
 *
 * 9   2/20/08 4:57p erickson
 * PR39786: initial impl
 *
 * 8   2/7/08 4:41p erickson
 * PR34926: added userDataBufferSize open settings, convert to BMEM_Alloc
 *  for kernel mode support
 *
 * 7   2/7/08 11:02a jgarrett
 * PR 39270: Adding selectable heap sizes
 *
 * 6   1/31/08 9:44a erickson
 * PR36808: added private SyncChannel connection
 *
 * 5   1/25/08 3:43p erickson
 * PR38882: fix last checkin
 *
 * 4   1/25/08 3:32p erickson
 * PR38882: use NEXUS_VideoDecoderSettings.fifoSize if non-zero
 *
 * 3   1/22/08 5:07p erickson
 * PR38888: added NEXUS_VideoDecoder_GetStreamInformation
 *
 * 2   1/22/08 3:45p erickson
 * PR38888: added streamCallback for sequence header notification
 *
 * 1   1/18/08 2:20p jgarrett
 * PR 38808: Merging to main branch
 *
 **************************************************************************/
#include "nexus_video_decoder_module.h"
#include "nexus_power_management.h"
#include "priv/nexus_stc_channel_priv.h"
#include "priv/nexus_core.h"
#include "priv/nexus_core_img_id.h"
#include "priv/nexus_core_img.h"

#include "bchp_xpt_rave.h"
#include "bxvd_pvr.h"

#if (defined(NEXUS_SECURITY_VIDEO_VERIFICATION) || defined (NEXUS_ENABLE_VICH))
#include "nexus_video_decoder_security.h"
#endif

BDBG_MODULE(nexus_video_decoder);
BDBG_FILE_MODULE(nexus_flow_video_decoder);

#define BDBG_MSG_TRACE(X) /* BDBG_MSG(X) */

BTRC_MODULE(ChnChange_Tune, ENABLE);
BTRC_MODULE(ChnChange_TuneLock, ENABLE);
BTRC_MODULE(ChnChange_DecodeStopVideo, ENABLE);
BTRC_MODULE(ChnChange_DecodeStartVideo, ENABLE);
BTRC_MODULE(ChnChange_DecodeStartVideoXVD, ENABLE);
BTRC_MODULE(ChnChange_DecodeFirstVideo, ENABLE);
BTRC_MODULE(ChnChange_DecodeFirstvPTSPassed, ENABLE);
BTRC_MODULE(ChnChange_SyncTsmLock, ENABLE); /* TODO: add when sync_channel support is available */
BTRC_MODULE(ChnChange_SyncUnmuteVideo, ENABLE); /* TODO: add when sync_channel support is available */
BTRC_MODULE(ChnChange_Total_Video, ENABLE);

/*
NEXUS_NUM_XVD_DEVICES = # of AVD cores to use. can be overridden in nexus_platform_features.h.
NEXUS_MAX_XVD_DEVICES = # of AVD cores that can be supported by the API. must be >= NEXUS_NUM_XVD_DEVICES.
*/
#ifndef NEXUS_NUM_XVD_DEVICES
#if BCHP_CHIP == 7400 || BCHP_CHIP == 7425 || BCHP_CHIP == 7420 || BCHP_CHIP == 7435
#define NEXUS_NUM_XVD_DEVICES 2
#else
#define NEXUS_NUM_XVD_DEVICES 1
#endif
#endif

/* non-static global module data */
NEXUS_ModuleHandle g_NEXUS_videoDecoderModule = NULL;
NEXUS_VideoDecoderModuleSettings g_NEXUS_videoDecoderModuleSettings;
struct NEXUS_VideoDecoderDevice g_NEXUS_videoDecoderXvdDevices[NEXUS_MAX_XVD_DEVICES];

/* static global module data */
static bool g_chanInUse[NEXUS_NUM_XVD_DEVICES][NEXUS_NUM_XVD_CHANNELS] = {{false,},}; /* channel usage status */
static NEXUS_VideoDecoderHandle g_videoDecoders[NEXUS_NUM_VIDEO_DECODERS] = {NULL,}; /* decoder usage */

static NEXUS_Error NEXUS_VideoDecoder_P_SetSettings(NEXUS_VideoDecoderHandle videoDecoder, const NEXUS_VideoDecoderSettings *pSettings, bool force);

/* Please refer to the XVD memory spreadsheet for more details on these values. */

/* CDB and ITB sizes */
#define B_XVD_VIDEO_CDB_SIZE (3*1024*1024/2) /* 1.5 MB for main video decode CDB */
#define B_XVD_VIDEO_ITB_SIZE (512*1024)      /* 512 KB for main video decode ITB */
#define B_XVD_MOSAIC_CDB_SIZE (150*1024)     /* 150 KB for mosaic-only video decode */
#define B_XVD_MOSAIC_ITB_SIZE (32*1024)      /* A very small ITB is needed for mosaic-only video decode */

/**************************************
* Module functions
**************************************/

void NEXUS_VideoDecoderModule_GetDefaultSettings(NEXUS_VideoDecoderModuleSettings *pSettings)
{
    unsigned i;
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    pSettings->watchdogEnabled = true;

    /* default is one heap per memc. if your platform requires a custom setting, that could should be put into the platform. */
    for (i=0;i<NEXUS_NUM_XVD_DEVICES;i++) {
        pSettings->avdHeapIndex[i] = i;
    }

    BDBG_CASSERT(NEXUS_MAX_XVD_DEVICES >= NEXUS_NUM_XVD_DEVICES);

    /* for instance, 7405 has one AVD and two MFD's. each MFD gets a separate BAVC_SourceId for VDC */
    for (i=0;i<NEXUS_NUM_VIDEO_DECODERS;i++) {
        pSettings->mfdMapping[i] = i;
        /* for backward compatible, non-7405 orthorgonal chips have one decode on each core; all future non-orthorgonal chips
           should really override the default in platform code. */
#if NEXUS_NUM_XVD_DEVICES == NEXUS_NUM_VIDEO_DECODERS
        pSettings->avdMapping[i] = i; /* set orthorgonal default */
#endif
    }

    /* Default to MPEG, AVC, VC1, HD and SD. We don't have to specify every codec to get this. See below for exact mapping to XVD.
    This array is deprecated, but is still used to populate the default for NEXUS_VideoDecoderOpenSettings.

    The semantics of the array used to be:
        supportedCodecs[0] = NEXUS_VideoCodec_eMpeg2;
    ...which is not application programmable w/o reading nexus implementation    
    but will be changed to:
        supportedCodecs[NEXUS_VideoCodec_eMpeg2] = true;
    ...because it is application programmable    
    but for backward compatibility will be this for now:
        supportedCodecs[NEXUS_VideoCodec_eMpeg2] = NEXUS_VideoCodec_eMpeg2;
    */
    pSettings->supportedCodecs[NEXUS_VideoCodec_eMpeg2] = NEXUS_VideoCodec_eMpeg2;
    pSettings->supportedCodecs[NEXUS_VideoCodec_eH264] = NEXUS_VideoCodec_eH264;
    pSettings->supportedCodecs[NEXUS_VideoCodec_eVc1] = NEXUS_VideoCodec_eVc1;
    pSettings->supportedCodecs[NEXUS_VideoCodec_eAvs] = NEXUS_VideoCodec_eAvs;
#if NEXUS_SVC_MVC_SUPPORT
    pSettings->supportedCodecs[NEXUS_VideoCodec_eH264_Svc] = NEXUS_VideoCodec_eH264_Svc;
    pSettings->supportedCodecs[NEXUS_VideoCodec_eH264_Mvc] = NEXUS_VideoCodec_eH264_Mvc;
#endif
    pSettings->maxDecodeFormat = NEXUS_VideoFormat_e1080p30hz;
    pSettings->maxStillDecodeFormat = NEXUS_VideoFormat_e1080p30hz;
    return;
}

/* set at channel open time */
static void NEXUS_VideoDecoder_P_ConvertCapabilities(
    NEXUS_VideoDecoderHandle videoDecoder,
    BXVD_ChannelSettings *channelSettings)
{
    /* Assume MPEG and SD. We need to know AVC or VC1, HD or SD. */
    bool avc = false;
    bool svc = false;
    bool mvc = false;
    bool vc1 = false;
    unsigned n;

    for (n=0;n<NEXUS_VideoCodec_eMax;n++) {
        switch (videoDecoder->settings.supportedCodecs[n]) {
        case NEXUS_VideoCodec_eH263:
        case NEXUS_VideoCodec_eH264:
        case NEXUS_VideoCodec_eMpeg4Part2:
        case NEXUS_VideoCodec_eDivx311:
            /* AVC and DivX are added together */
            avc = true;
            break;
        case NEXUS_VideoCodec_eH264_Svc:
            svc = true;
            break;
        case NEXUS_VideoCodec_eH264_Mvc:
            mvc = true;
            break;
        case NEXUS_VideoCodec_eVc1:
        case NEXUS_VideoCodec_eVc1SimpleMain:
            vc1 = true;
            break;
        default:
            break;
        }
    }

    channelSettings->uiVideoCmprCount = 0;
    channelSettings->peVideoCmprStdList[channelSettings->uiVideoCmprCount++] = BAVC_VideoCompressionStd_eMPEG2;
    if (avc) {
        channelSettings->peVideoCmprStdList[channelSettings->uiVideoCmprCount++] = BAVC_VideoCompressionStd_eH264;
        channelSettings->peVideoCmprStdList[channelSettings->uiVideoCmprCount++] = BAVC_VideoCompressionStd_eMPEG4Part2;
    }
    if (svc) {
        channelSettings->peVideoCmprStdList[channelSettings->uiVideoCmprCount++] = BAVC_VideoCompressionStd_eSVC;
    }
    if (mvc) {
        channelSettings->peVideoCmprStdList[channelSettings->uiVideoCmprCount++] = BAVC_VideoCompressionStd_eMVC;
    }
    if (vc1) {
        channelSettings->peVideoCmprStdList[channelSettings->uiVideoCmprCount++] = BAVC_VideoCompressionStd_eVC1;
        channelSettings->peVideoCmprStdList[channelSettings->uiVideoCmprCount++] = BAVC_VideoCompressionStd_eVC1SimpleMain;
    }

    if (videoDecoder->mosaicSettings.maxHeight == 0 || videoDecoder->mosaicSettings.maxWidth == 0 || videoDecoder->mosaicSettings.maxHeight > 1088 || videoDecoder->mosaicSettings.maxWidth > 1920 ) {
        BDBG_ERR(("Invalid height or width specified"));
        channelSettings->eDecodeResolution = BXVD_DecodeResolution_eHD;
    }
    if (videoDecoder->mosaicSettings.maxHeight <= 144 && videoDecoder->mosaicSettings.maxWidth <= 176) {
        channelSettings->eDecodeResolution = BXVD_DecodeResolution_eQCIF;
    }
    else if (videoDecoder->mosaicSettings.maxHeight <= 288 && videoDecoder->mosaicSettings.maxWidth <= 352) {
        channelSettings->eDecodeResolution = BXVD_DecodeResolution_eCIF;
    }
    else if (videoDecoder->mosaicSettings.maxHeight <= 576 && videoDecoder->mosaicSettings.maxWidth <= 720) {
        channelSettings->eDecodeResolution = BXVD_DecodeResolution_eSD;
    }
    else {
        channelSettings->eDecodeResolution = BXVD_DecodeResolution_eHD;
    }
    return;
}

static void NEXUS_VideoDecoderModule_P_Print_Avd(void)
{
    unsigned i;
    
    BDBG_LOG(("VideoDecoderModule:"));
    for (i=0; i < NEXUS_NUM_XVD_DEVICES; i++) {
        unsigned j;
        BDBG_LOG(("Heap Sizes[%u]: general:%uMB secure:%uMB picture:%uMB",i,
            g_NEXUS_videoDecoderModuleSettings.heapSize[i].general/1024/1024,
            g_NEXUS_videoDecoderModuleSettings.heapSize[i].secure/1024/1024,
            g_NEXUS_videoDecoderModuleSettings.heapSize[i].picture/1024/1024));
        BDBG_LOG(("AVD%u: (%p)",i, g_NEXUS_videoDecoderXvdDevices[i].xvd));
        BDBG_LOG((" watchdog: %s cnt=%d",g_NEXUS_videoDecoderModuleSettings.watchdogEnabled?"enabled":"disabled", g_NEXUS_videoDecoderXvdDevices[i].numWatchdogs));
        for (j=0; j < NEXUS_NUM_XVD_CHANNELS; j++) {
            NEXUS_VideoDecoderHandle videoDecoder = &g_NEXUS_videoDecoderXvdDevices[i].channel[j];
            if (videoDecoder->dec) {
                NEXUS_VideoDecoderStatus status;
                NEXUS_VideoDecoderTrickState trickState;
                NEXUS_PidChannelStatus pidChannelStatus;
                unsigned itbDepth, itbSize;
                NEXUS_Error rc;
                #if !BDBG_NO_LOG
                static const char *g_decodeModeStr[NEXUS_VideoDecoderDecodeMode_eMax] = {"All", "IP", "I"};
                #endif
                rc = NEXUS_VideoDecoder_GetStatus(videoDecoder, &status);
                if (rc) continue;
                NEXUS_VideoDecoder_GetTrickState(videoDecoder, &trickState);

                if (videoDecoder->started) {
                    NEXUS_PidChannel_GetStatus(videoDecoder->startSettings.pidChannel, &pidChannelStatus);
                }
                BKNI_EnterCriticalSection();                
                NEXUS_Rave_GetItbBufferInfo_isr(videoDecoder->rave, &itbDepth, &itbSize);
                BKNI_LeaveCriticalSection();

                BDBG_LOG((" channel%d: (%p) %s", j, videoDecoder->dec, videoDecoder->mosaicMode ? "mosaic" : ""));
                BDBG_LOG(("  started=%c, codec=%d, pid=%#x", videoDecoder->started?'y':'n',
                    videoDecoder->started?videoDecoder->startSettings.codec:0,
                    videoDecoder->started?pidChannelStatus.pid:0));
                BDBG_LOG(("  CDB: %d/%d (%d%%), ITB: %d/%d (%d%%), pic_queue=%d", 
                    status.fifoDepth, status.fifoSize, status.fifoSize?status.fifoDepth*100/status.fifoSize:0, 
                    itbDepth, itbSize, itbSize?itbDepth*100/itbSize:0,
                    status.queueDepth));
                BDBG_LOG(("  TSM: %s pts=%#x pts_stc_diff=%d", videoDecoder->tsm?"enabled":"disabled", status.pts, status.ptsStcDifference));
                BDBG_LOG(("  Trick: rate=%d %s brcm=%c host=%c stc=%c top=%c",
                    trickState.rate, g_decodeModeStr[trickState.decodeMode],
                    trickState.brcmTrickModesEnabled?'y':'n',
                    trickState.hostTrickModesEnabled?'y':'n',
                    trickState.stcTrickEnabled?'y':'n',
                    trickState.topFieldOnly?'y':'n'));
            }
        }
    }
}

NEXUS_ModuleHandle NEXUS_VideoDecoderModule_P_Init_Avd(const NEXUS_VideoDecoderModuleSettings *pSettings)
{
    BERR_Code rc;
    NEXUS_ModuleSettings moduleSettings;
    unsigned i,j;
    BDBG_ASSERT(!g_NEXUS_videoDecoderModule);

    /* init global module handle */
    NEXUS_Module_GetDefaultSettings(&moduleSettings);
    moduleSettings.priority = NEXUS_ModulePriority_eLow; /* decoder interface is slow */
    moduleSettings.dbgPrint = NEXUS_VideoDecoderModule_P_Print_Avd;
    moduleSettings.dbgModules = "nexus_video_decoder";
    g_NEXUS_videoDecoderModule = NEXUS_Module_Create("video_decoder", &moduleSettings);
    if (!g_NEXUS_videoDecoderModule) {
        return NULL;
    }
    NEXUS_LockModule();

    /* init global data */
    BKNI_Memset(&g_NEXUS_videoDecoderXvdDevices, 0, sizeof(g_NEXUS_videoDecoderXvdDevices));
    g_NEXUS_videoDecoderModuleSettings = *pSettings;

    /* open XVD device, not channels */
    for (i=0;i<NEXUS_NUM_XVD_DEVICES;i++) {
        BXVD_Settings xvdSettings;
        BXVD_DecodeMode decodeModes[20];
        struct NEXUS_VideoDecoderDevice *vDevice = &g_NEXUS_videoDecoderXvdDevices[i];

        BDBG_CASSERT(NEXUS_NUM_XVD_DEVICES <= 2);
        BDBG_CASSERT(NEXUS_NUM_XVD_DEVICES <= NEXUS_MAX_HEAPS);
        vDevice->mem = g_pCoreHandles->heap[pSettings->avdHeapIndex[i]];

        if ( NEXUS_GetEnv("jail_avd") || NEXUS_GetEnv("jail_xvd")) {
            NEXUS_MemoryStatus memStatus;
            (void)NEXUS_Heap_GetStatus(g_pCoreHandles->nexusHeap[pSettings->avdHeapIndex[i]], &memStatus);
            BMRC_Monitor_JailByFilename(g_pCoreHandles->memc[memStatus.memcIndex].rmm, "bxvd_");
            BMRC_Monitor_JailUpdate(g_pCoreHandles->memc[memStatus.memcIndex].rmm);
        }

        BXVD_GetDefaultSettings(&xvdSettings);

        decodeModes[0] = BXVD_DecodeMode_eCustom;
        decodeModes[1] = BXVD_DecodeMode_eTerminateList;
        if ( 0 == pSettings->heapSize[i].general )
        {
            BDBG_ERR(("XVD general heap size is 0. Please refer set correct XVD heap size according to XVD Memory Worksheet"));
            goto error;
        }
        else
        {
            xvdSettings.stFWMemConfig.uiGeneralHeapSize = pSettings->heapSize[i].general;  /* Context memory (not included FW, see below) */
            xvdSettings.stFWMemConfig.uiCabacHeapSize =   pSettings->heapSize[i].secure;   /* SVP secure memory */
            xvdSettings.stFWMemConfig.uiPictureHeapSize = pSettings->heapSize[i].picture;  /* Picture memory, only needed for non-UMA systems */
        }
        xvdSettings.pDecodeModes = decodeModes;

        BDBG_MSG(("BXVD_Open %d: fw heap=%d, avd heap=%d", i, pSettings->hostAccessibleHeapIndex, pSettings->avdHeapIndex[i]));
        xvdSettings.uiAVDInstance = i;
        xvdSettings.hFrmMemory = g_pCoreHandles->heap[pSettings->hostAccessibleHeapIndex]; /* FW always in main heap */
        #if (BCHP_CHIP==7400)
        /* 7400 does not have enough timers do DUAL_OUTPUT mode, with its default BRAP_P_WATERMARK_WORKAROUND
           even limited to first xvd. Use this env to use the debug timer feature on 7400. */
        if (NEXUS_GetEnv("xvd_debug_timer")) {
        #else
        {
        #endif
            if (i==0)
            {
                    xvdSettings.hTimerDev = g_pCoreHandles->tmr;
            }
        }

        if (NEXUS_GetEnv("avd_monitor")) {
            xvdSettings.uiDecoderDebugLogBufferSize = 10 * 1024;
        }

        /* Image download */
        if (Nexus_Core_P_Img_Create(NEXUS_CORE_IMG_ID_XVD,&vDevice->img_context,&vDevice->img_interface )== NEXUS_SUCCESS)
        {
            BDBG_WRN(("FW download used"));
            xvdSettings.pImgInterface = (BIMG_Interface*)&vDevice->img_interface;
            xvdSettings.pImgContext   = vDevice->img_context;
        }
        else
        {
            BDBG_WRN(("FW download not used"));
            vDevice->img_context= NULL;
        }

        if (pSettings->secureHeap) {
            xvdSettings.hSecureMemory = NEXUS_Heap_GetMemHandle(pSettings->secureHeap);
        }

#if (defined (NEXUS_SECURITY_VIDEO_VERIFICATION) || defined (NEXUS_ENABLE_VICH) || defined (NEXUS_VIDEO_DECODER_NO_SECURE_HEAP))
        {
            NEXUS_VideoDecoderSecureConfig config;
#ifdef NEXUS_SECURITY_VIDEO_VERIFICATION_LEGACY_65NM
            BMEM_Handle hFWMem;
            config.hReg = g_pCoreHandles->reg;
            config.hMem = g_pCoreHandles->mem;
            config.index = i;

            NEXUS_VideoDecoder_P_EnableSecurity (
                                                &xvdSettings,
                                                &config,
                                                &hFWMem);

            rc = BXVD_Open(&vDevice->xvd, g_pCoreHandles->chp, g_pCoreHandles->reg,
                hFWMem,
                vDevice->mem,
                g_pCoreHandles->bint, &xvdSettings);
        }
#else
            config.hReg = g_pCoreHandles->reg;
            config.index = i;

            NEXUS_VideoDecoder_P_EnableSecurity (
                                                &xvdSettings,
                                                &config);

            rc = BXVD_Open(&vDevice->xvd, g_pCoreHandles->chp, g_pCoreHandles->reg,
                g_pCoreHandles->heap[pSettings->hostAccessibleHeapIndex],
                vDevice->mem,
                g_pCoreHandles->bint, &xvdSettings);
        }
#endif
#else
        rc = BXVD_Open(&vDevice->xvd, g_pCoreHandles->chp, g_pCoreHandles->reg,
            g_pCoreHandles->heap[pSettings->hostAccessibleHeapIndex],  /* this is for FW. because we not setting this to NULL, XVD will alloc from here. Therefore, do not add space for FW in uiGeneralHeapSize */
            vDevice->mem,
            g_pCoreHandles->bint, &xvdSettings);
#endif
        if (rc) {rc=BERR_TRACE(rc); goto error;}

        /* register event for task-time execution of xvd watchdog. the watchdog event and isr is device-level, even
        though the BXVD_Interrupt is channel-level. */
        BKNI_CreateEvent(&vDevice->watchdog_event);
        vDevice->watchdogEventHandler = NEXUS_RegisterEvent(vDevice->watchdog_event, NEXUS_VideoDecoder_P_WatchdogHandler, vDevice);

        /* install XVD device interrupts */
        if (g_NEXUS_videoDecoderModuleSettings.watchdogEnabled && !NEXUS_GetEnv("no_watchdog")) {
            rc = BXVD_InstallDeviceInterruptCallback(vDevice->xvd, BXVD_DeviceInterrupt_eWatchdog, NEXUS_VideoDecoder_P_Watchdog_isr, vDevice, 0);
            if (rc) {rc=BERR_TRACE(rc); goto error;}
        }
        for (j = 0; j < BXVD_DisplayInterrupt_eMax; j++)
        {
           rc = BXDM_DisplayInterruptHandler_Create(&vDevice->hXdmDih[j]);
            if (rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc); goto error;}
        }
    }

    NEXUS_UnlockModule();
    return g_NEXUS_videoDecoderModule;
    
error:
    NEXUS_UnlockModule();
    return NULL;
}

void NEXUS_VideoDecoderModule_P_Uninit_Avd(void)
{
    unsigned i, j;

    NEXUS_LockModule();

    for (i=0;i<NEXUS_NUM_XVD_DEVICES;i++) {
        struct NEXUS_VideoDecoderDevice *vDevice = &g_NEXUS_videoDecoderXvdDevices[i];

        for (j=0;j<NEXUS_NUM_XVD_CHANNELS;j++) {
            if (vDevice->channel[j].dec) {
                NEXUS_VideoDecoder_Close(&vDevice->channel[j]);
            }
        }

        for (j = 0; j < BXVD_DisplayInterrupt_eMax; j++)
        {
           BXDM_DisplayInterruptHandler_Destroy(vDevice->hXdmDih[j]);
           vDevice->hXdmDih[j] = NULL;
        }
        if (g_NEXUS_videoDecoderModuleSettings.watchdogEnabled) {
            BXVD_UnInstallDeviceInterruptCallback(vDevice->xvd, BXVD_DeviceInterrupt_eWatchdog);
        }

        NEXUS_UnregisterEvent(vDevice->watchdogEventHandler);
        BKNI_DestroyEvent(vDevice->watchdog_event);
        BXVD_Close(vDevice->xvd);
        Nexus_Core_P_Img_Destroy(vDevice->img_context);

#ifdef NEXUS_SECURITY_VIDEO_VERIFICATION
        NEXUS_VideoDecoder_P_DisableFwVerification(i);
#endif
    }

    NEXUS_UnlockModule();
    NEXUS_Module_Destroy(g_NEXUS_videoDecoderModule);
    g_NEXUS_videoDecoderModule = NULL;
}

/**************************************
* Interface functions
**************************************/
void NEXUS_VideoDecoder_P_GetRaveSettings(NEXUS_VideoDecoderHandle videoDecoder, NEXUS_RaveOpenSettings *pRaveSettings, const NEXUS_VideoDecoderOpenMosaicSettings *pOpenSettings)
{
    int32_t pic_buf_length;

    LOCK_TRANSPORT();
    NEXUS_Rave_GetDefaultOpenSettings_priv(pRaveSettings);
    UNLOCK_TRANSPORT();

    (void)BXVD_GetBufferConfig(videoDecoder->device->xvd, &pRaveSettings->config, &pic_buf_length);

    if (pOpenSettings->openSettings.fifoSize == 0) {
        /* For mosaic-capable systems, we get the endianness from BXVD_GetBufferConfig, but we calc the required size. */
        /* Anything below 720x480 is mosaic mode (CIF or QCIF) so allocation smaller CDB/ITB */
        if (pOpenSettings->maxHeight < 352) {
            pRaveSettings->config.Cdb.Length = B_XVD_MOSAIC_CDB_SIZE;
            pRaveSettings->config.Itb.Length = B_XVD_MOSAIC_ITB_SIZE;
        }
        else {
            pRaveSettings->config.Cdb.Length = B_XVD_VIDEO_CDB_SIZE;
            pRaveSettings->config.Itb.Length = B_XVD_VIDEO_ITB_SIZE;
        }
    }
    else {
        pRaveSettings->config.Cdb.Length = B_XVD_VIDEO_CDB_SIZE;
        pRaveSettings->config.Itb.Length = B_XVD_VIDEO_ITB_SIZE;

        /* If the user has specified a CDB size, use it. */
        pRaveSettings->config.Cdb.Length = pOpenSettings->openSettings.fifoSize;

        if (pOpenSettings->openSettings.itbFifoSize) {
            /* user-specific ITB */
            pRaveSettings->config.Itb.Length = pOpenSettings->openSettings.itbFifoSize;
        }
        else {
            /* change ITB size relative to the fifoSize change */
            pRaveSettings->config.Itb.Length = pRaveSettings->config.Itb.Length * (pOpenSettings->openSettings.fifoSize/10000) / (pRaveSettings->config.Cdb.Length/10000);
        }
    }
    BDBG_MSG(("[%p] CDB %d, ITB %d", videoDecoder, pRaveSettings->config.Cdb.Length, pRaveSettings->config.Itb.Length));

    pRaveSettings->allocSoftRave = true;
}

void NEXUS_VideoDecoder_P_GetMosaicOpenSettings(NEXUS_VideoDecoderOpenMosaicSettings *mosaicSettings, const NEXUS_VideoDecoderOpenSettings *openSettings)
{
    NEXUS_VideoFormatInfo videoFormatInfo;

    NEXUS_VideoDecoder_GetDefaultOpenMosaicSettings(mosaicSettings);
    if (openSettings) {
        mosaicSettings->openSettings = *openSettings;
    }
    /* convert from module settings max format to OpenMosaicSettings max width/height */
    NEXUS_VideoFormat_GetInfo(g_NEXUS_videoDecoderModuleSettings.maxDecodeFormat, &videoFormatInfo);
    mosaicSettings->maxWidth = videoFormatInfo.width;
    mosaicSettings->maxHeight = videoFormatInfo.height;
    return;
}


NEXUS_Error NEXUS_VideoDecoder_P_Init_Generic(NEXUS_VideoDecoderHandle videoDecoder, const NEXUS_RaveOpenSettings *raveSettings, const NEXUS_VideoDecoderOpenMosaicSettings *pOpenSettings)
{
    NEXUS_Error rc;
    bool openEnhancementRave;

    /* verify API macros aren't below nexus_platform_features.h */
    BDBG_CASSERT(NEXUS_MAX_VIDEO_DECODERS >= NEXUS_NUM_VIDEO_DECODERS);

    videoDecoder->otfPvr.wasActive = false;
    videoDecoder->errorHandlingOverride = false;
    videoDecoder->errorHandlingMode = NEXUS_VideoDecoderErrorHandling_ePicture;

    openEnhancementRave = pOpenSettings->openSettings.enhancementPidChannelSupported;

    NEXUS_VIDEO_INPUT_INIT(&videoDecoder->input, NEXUS_VideoInputType_eDecoder, videoDecoder);
    videoDecoder->openSettings = *pOpenSettings;
    BDBG_OBJECT_SET(videoDecoder, NEXUS_VideoDecoder);
    videoDecoder->displayInformation.refreshRate = 60; /* Hz */

#if NEXUS_HAS_ASTM
    BKNI_Memset(&videoDecoder->astm.settings, 0, sizeof(NEXUS_VideoDecoderAstmSettings));
    BKNI_Memset(&videoDecoder->astm.status, 0, sizeof(NEXUS_VideoDecoderAstmStatus));
#endif

    videoDecoder->userdataCallback = NEXUS_IsrCallback_Create(videoDecoder, NULL);
    if(!videoDecoder->userdataCallback) {rc=BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);goto error;}
    videoDecoder->sourceChangedCallback = NEXUS_IsrCallback_Create(videoDecoder, NULL);
    if(!videoDecoder->sourceChangedCallback) {rc=BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);goto error;}
    videoDecoder->streamChangedCallback = NEXUS_IsrCallback_Create(videoDecoder, NULL);
    if(!videoDecoder->streamChangedCallback) {rc=BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);goto error;}
    videoDecoder->ptsErrorCallback = NEXUS_IsrCallback_Create(videoDecoder, NULL);
    if(!videoDecoder->ptsErrorCallback) {rc=BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);goto error;}
    videoDecoder->firstPtsCallback = NEXUS_IsrCallback_Create(videoDecoder, NULL);
    if(!videoDecoder->firstPtsCallback) {rc=BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);goto error;}
    videoDecoder->firstPtsPassedCallback = NEXUS_IsrCallback_Create(videoDecoder, NULL);
    if(!videoDecoder->firstPtsPassedCallback) {rc=BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);goto error;}
    videoDecoder->dataReadyCallback = NEXUS_IsrCallback_Create(videoDecoder, NULL);
    if(!videoDecoder->dataReadyCallback) {rc=BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);goto error;}
    videoDecoder->fifoEmpty.callback = NEXUS_IsrCallback_Create(videoDecoder, NULL);
    if(!videoDecoder->fifoEmpty.callback) {rc=BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);goto error;}
    videoDecoder->afdChangedCallback = NEXUS_IsrCallback_Create(videoDecoder, NULL);
    if(!videoDecoder->afdChangedCallback) {rc=BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);goto error;}
    videoDecoder->decodeErrorCallback = NEXUS_IsrCallback_Create(videoDecoder, NULL);
    if(!videoDecoder->decodeErrorCallback) {rc=BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);goto error;}
    videoDecoder->s3DTVChangedCallback = NEXUS_IsrCallback_Create(videoDecoder, NULL);
    if(!videoDecoder->s3DTVChangedCallback) {rc=BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);goto error;}
    videoDecoder->fnrtChunkDoneCallback = NEXUS_IsrCallback_Create(videoDecoder, NULL);
    if(!videoDecoder->fnrtChunkDoneCallback) {rc=BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);goto error;}

    videoDecoder->playback.firstPtsCallback = NEXUS_IsrCallback_Create(videoDecoder, NULL);
    if(!videoDecoder->playback.firstPtsCallback) {rc=BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);goto error;}
    videoDecoder->playback.firstPtsPassedCallback = NEXUS_IsrCallback_Create(videoDecoder, NULL);
    if(!videoDecoder->playback.firstPtsPassedCallback) {rc=BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);goto error;}

    {
        BMEM_Heap_Handle mem = g_pCoreHandles->heap[g_NEXUS_videoDecoderModuleSettings.hostAccessibleHeapIndex];
        void *temp;
        videoDecoder->userdata.buf = BMEM_Alloc(mem, pOpenSettings->openSettings.userDataBufferSize);
        if (!videoDecoder->userdata.buf) {rc = BERR_TRACE(NEXUS_OUT_OF_DEVICE_MEMORY); goto error;}
        rc = BMEM_Heap_ConvertAddressToCached(g_pCoreHandles->heap[0], videoDecoder->userdata.buf, (void **)&temp);
        if (rc) {rc=BERR_TRACE(rc); goto error;}
        videoDecoder->userdata.buf = temp;
    }
    rc = BKNI_CreateEvent(&videoDecoder->source_changed_event);
    if (rc) {rc=BERR_TRACE(rc); goto error;}

    rc = BKNI_CreateEvent(&videoDecoder->channelChangeReportEvent);
    if (rc) {rc=BERR_TRACE(rc); goto error;}
    videoDecoder->channelChangeReportEventHandler = NEXUS_RegisterEvent(videoDecoder->channelChangeReportEvent, NEXUS_VideoDecoder_P_ChannelChangeReport, videoDecoder);
    if (videoDecoder->channelChangeReportEventHandler == NULL) {rc=BERR_TRACE(rc); goto error;}

    rc= BKNI_CreateEvent(&videoDecoder->s3DTVStatusEvent);
    if (rc) {rc=BERR_TRACE(rc); goto error;}
    videoDecoder->s3DTVStatusEventHandler = NEXUS_RegisterEvent(videoDecoder->s3DTVStatusEvent, NEXUS_VideoDecoder_P_3DTVTimer, videoDecoder);
    if (videoDecoder->s3DTVStatusEventHandler == NULL) {rc=BERR_TRACE(rc); goto error;}

    videoDecoder->cdbLength = raveSettings->config.Cdb.Length;
    videoDecoder->itbLength = raveSettings->config.Itb.Length;

    LOCK_TRANSPORT();
    videoDecoder->rave = NEXUS_Rave_Open_priv(raveSettings);
    UNLOCK_TRANSPORT();
    if (!videoDecoder->rave) {rc=BERR_TRACE(rc); goto error;}
    LOCK_TRANSPORT();
    videoDecoder->enhancementRave = NULL;
    if(openEnhancementRave) {
        videoDecoder->enhancementRave = NEXUS_Rave_Open_priv(raveSettings);
    }
    UNLOCK_TRANSPORT();
    if (openEnhancementRave && !videoDecoder->enhancementRave) {rc=BERR_TRACE(rc); goto error;}

    /* set default settings */
    BKNI_Memset(&videoDecoder->settings, 0, sizeof(videoDecoder->settings));
    videoDecoder->settings.dropFieldMode = false;
    
    return NEXUS_SUCCESS;

error:
    return BERR_TRACE(rc);
}

NEXUS_VideoDecoderHandle NEXUS_VideoDecoder_P_Open(int parentIndex, unsigned index, const NEXUS_VideoDecoderOpenMosaicSettings *pOpenSettings)
{
    BERR_Code rc;
    NEXUS_VideoDecoderHandle videoDecoder;
    NEXUS_RaveOpenSettings raveSettings;
    unsigned mfdIndex, channelIndex, avdIndex;

    if (!g_NEXUS_videoDecoderModule) {
        BDBG_ERR(("Module not initialized"));
        return NULL;
    }

    BDBG_MSG(("%s(%d,%d,%dx%d)",__FUNCTION__,parentIndex,index,pOpenSettings ? pOpenSettings->maxWidth : 0,
              pOpenSettings ? pOpenSettings->maxHeight : 0));

    /**
    7405 has one AVD outputting to two MFD's. g_NEXUS_videoDecoderXvdDevices[] represents the AVD. mainIndex represents the MFD.
    The MFD0 is used for main and mosaic. mainIndex = 0, channelIndex = 0,2,3,4,... (i.e. skip 1).
    The MFD1 is used for PIP. In that case, mainIndex = 1, channelIndex = 1.
    7422/25 has two AVDs outputting to three MFD's. AVD1 drives MFD1&2, while AVD0 (SVC capable) drives MFD0.
    **/
    /* mfdIndex is the MFD index; avdIndex is the device index; channelIndex is the channel num on that device. */
    if (parentIndex == -1) {
        if (index >= NEXUS_NUM_VIDEO_DECODERS) {
            BERR_TRACE(NEXUS_INVALID_PARAMETER);
            return NULL;
        }
        mfdIndex = g_NEXUS_videoDecoderModuleSettings.mfdMapping[index];
        avdIndex  = g_NEXUS_videoDecoderModuleSettings.avdMapping[index];
        if(g_videoDecoders[index]) {
            BDBG_ERR(("VideoDecoder %d already open", index));
            return NULL;
        }
    }
    else {
        if (parentIndex >= NEXUS_NUM_VIDEO_DECODERS) {
            BERR_TRACE(NEXUS_INVALID_PARAMETER);
            return NULL;
        }
        mfdIndex = g_NEXUS_videoDecoderModuleSettings.mfdMapping[parentIndex];
        avdIndex  = g_NEXUS_videoDecoderModuleSettings.avdMapping[parentIndex];
    }
    if (avdIndex >= NEXUS_NUM_XVD_DEVICES) {
        BDBG_ERR(("AVD%d not supported. %d AVD's supported.", avdIndex, NEXUS_NUM_XVD_DEVICES));
        return NULL;
    }
    if (mfdIndex >= NEXUS_NUM_VIDEO_DECODERS) {
        BDBG_ERR(("MFD%d not supported. %d MFD's supported.", mfdIndex, NEXUS_NUM_VIDEO_DECODERS));
        return NULL;
    }

    /* find available channel */
    for(channelIndex = 0; channelIndex < NEXUS_NUM_XVD_CHANNELS; channelIndex++)
    {
        /* search for unused channel per the AVD device */
        if(!g_chanInUse[avdIndex][channelIndex])
        {
            break;
        }
    }
    BDBG_MSG(("To open video decoder, parent%d, id%d, avd%d, mfd%d, channel%d",
        parentIndex, index, avdIndex, mfdIndex, channelIndex));
    if (channelIndex >= NEXUS_NUM_XVD_CHANNELS) {
        BDBG_ERR(("XVD channel %d not supported. %d channels supported.", channelIndex, NEXUS_NUM_XVD_CHANNELS));
        return NULL;
    }

    videoDecoder = &g_NEXUS_videoDecoderXvdDevices[avdIndex].channel[channelIndex];
    videoDecoder->device = &g_NEXUS_videoDecoderXvdDevices[avdIndex];

    if (videoDecoder->rave) {
        BDBG_ERR(("VideoDecoder %d,%d (%d,%d) already open", parentIndex, index, avdIndex, channelIndex));
        return NULL;
    }

    videoDecoder->mosaicMode = (parentIndex != -1);
    videoDecoder->mfdIndex = mfdIndex;

    /* Note, the global flag should be set after all previous error checking passed to avoid side effect. */
    /* avoid re-open an opened decoder */
    if (!videoDecoder->mosaicMode) {
        g_videoDecoders[index] = videoDecoder;
    }

    /* finally mark the channel in use */
    g_chanInUse[avdIndex][channelIndex] = true;

    BDBG_MSG(("NEXUS_VideoDecoder_P_Open %d %p", index, videoDecoder));

    /* copy from open to runtime settings. they may change. */
    videoDecoder->mosaicSettings.maxWidth = pOpenSettings->maxWidth;
    videoDecoder->mosaicSettings.maxHeight = pOpenSettings->maxHeight;

    NEXUS_VideoDecoder_P_GetRaveSettings(videoDecoder, &raveSettings, pOpenSettings);
    videoDecoder->interface = &NEXUS_VideoDecoder_P_Interface_Avd;
    rc = NEXUS_VideoDecoder_P_Init_Generic(videoDecoder, &raveSettings, pOpenSettings);
    if(rc!=NEXUS_SUCCESS) { rc=BERR_TRACE(rc);goto error; }
        
    /* set supportedCodecs[] based on module supportedCodecs[] and XVD's report of HW capabilities */
    {
        BXVD_HardwareCapabilities cap;
        NEXUS_VideoCodec i;
        rc = BXVD_GetHardwareCapabilities(videoDecoder->device->xvd, &cap);
        if (rc) { rc = BERR_TRACE(rc); goto error; }
        for (i=0;i<NEXUS_VideoCodec_eMax;i++) {
            BAVC_VideoCompressionStd xvdCodec = NEXUS_P_VideoCodec_ToMagnum(i, NEXUS_TransportType_eTs);
            if (g_NEXUS_videoDecoderModuleSettings.supportedCodecs[i] && cap.bCodecSupported[xvdCodec]) {
                videoDecoder->settings.supportedCodecs[i] = i;
            }
            else {
                videoDecoder->settings.supportedCodecs[i] = NEXUS_VideoCodec_eNone;
            }
            if (videoDecoder->mosaicMode) {
                switch (i) {
                case NEXUS_VideoCodec_eH264_Svc:
                case NEXUS_VideoCodec_eH264_Mvc:
                    /* default svc/mvc off for mosaic */
                    videoDecoder->settings.supportedCodecs[i] = NEXUS_VideoCodec_eNone;
                    break;
                default:
                    break;
                }
            }
        }
    }
    NEXUS_OBJECT_REGISTER(NEXUS_VideoInput, &videoDecoder->input, Open);
    

    return videoDecoder;

error:
    /* this will close whatever has been opened */
    NEXUS_VideoDecoder_Close(videoDecoder);
    return NULL;
}

static BERR_Code NEXUS_VideoDecoder_P_PictureDataReady_isr(void *context, int32_t param, BAVC_MFD_Picture *picture)
{
    NEXUS_VideoDecoder_P_DataReady_isr(context, param, picture);
    return BERR_SUCCESS;
}

static void NEXUS_VideoDecoder_P_TsmResult_isr( void *pParm1, int iParm2, void *pData )
{
   BERR_Code rc;
   BXVD_TSMInfo *pTSMInfo = (BXVD_TSMInfo *)pData;
   NEXUS_VideoDecoderHandle videoDecoder = pParm1;
   const BXVD_PPBParameterInfo * pstPPBParameterInfo;
   uint32_t n;

   BSTD_UNUSED(iParm2);

   if (videoDecoder->extendedSettings.lowLatencyTsmMode)
   {

#define NEXUS_LOW_LATENCY_MAX_PICTURES_TO_EXAMINE 10     /* Will retrieved the information for 10 pictures. */

      /* If waiting on a picture, check the depth of the delivery queue. */
      if ( BXVD_TSMResult_eWait == pTSMInfo->eTSMResult )
      {
         const BXVD_PPBParameterInfo * apUnifiedPictures[NEXUS_LOW_LATENCY_MAX_PICTURES_TO_EXAMINE];

         uint32_t                uiNumberOfPicturesOnTheQueue;
         BXVD_ChannelHandle      hXvdCh = videoDecoder->dec;
         NEXUS_StcChannelHandle  stcChannel = videoDecoder->startSettings.stcChannel;

         /* Get the parameters for the next 10 pictures on the queue. */
         BXVD_GetPPBParameterQueueInfo_isr(
                        hXvdCh,
                        &(apUnifiedPictures[0]),
                        NEXUS_LOW_LATENCY_MAX_PICTURES_TO_EXAMINE,
                        &uiNumberOfPicturesOnTheQueue
                        );

         /* If there are any pictures on the queue, check if there are more than the latency drop threshold. */
         if ( uiNumberOfPicturesOnTheQueue >= 1 )
         {
            uint32_t uiPictureTime;
            bool     bReloadStc;

            /* Use the frame rate of the first picture on the queue to determine
             * how long each picture should be displayed. (The frame rate should
             * be the same for all the pictures on the queue.)
             */
            switch ( apUnifiedPictures[0]->stFrameRate.stRate.uiNumerator )
            {
               case 60000:    uiPictureTime = 750;    break;
               case 50000:    uiPictureTime = 900;    break;
               case 30000:    uiPictureTime = 1500;   break;
               case 25000:    uiPictureTime = 1800;   break;
               case 24000:    uiPictureTime = 1875;   break;
               default:       uiPictureTime = 1500;   break;
            }

            /* If the frame rate is 59.94, 29.97... bump the picture time.
             * We may not need to be this accurate.
             */
            if ( 1000 != apUnifiedPictures[0]->stFrameRate.stRate.uiDenominator )
            {
               uiPictureTime += 1;
            }

#if 0 /* removed because it isn't clear how this impacts the display */
            if (videoDecoder->extendedSettings.lowLatencyDropThreshold)
            {
                bReloadStc = ( uiNumberOfPicturesOnTheQueue > videoDecoder->extendedSettings.lowLatencyDropThreshold );
            }
            else
#endif
            {
                /* This logic is from the Wifi display group.  It sets the drop threshold based
                 * on the framerate. This is the default if no nonzero drop threshold was specified
                 */
                if ( uiPictureTime > 900 )
                {
                    bReloadStc = ( uiNumberOfPicturesOnTheQueue > 2 );
                }
                else
                {
                    bReloadStc = ( uiNumberOfPicturesOnTheQueue > 3 );
                }
            }

            if ( true == bReloadStc )
            {
               uint32_t uiNewStc;

               /* If the last picture on the queue has a valid PTS, set the STC to that value.
                * If it does not have a valid PTS, set the STC equal to the current STC,
                * then add "n" picture times to it.  Note: this case hasn't been tested
                * very extensively.
                *
                * By setting the STC ahead, XDM will burn through all the pictues
                * on the queue on the next vsync.
                */
               if ( true == apUnifiedPictures[ uiNumberOfPicturesOnTheQueue-1 ]->stPTS.bValid )
               {
                  uiNewStc = apUnifiedPictures[ uiNumberOfPicturesOnTheQueue-1 ]->stPTS.uiValue;
               }
               else
               {
                  BXVD_PTSInfo stCurrentPtsInfo;
                  BXVD_GetPTS_isr( hXvdCh, &stCurrentPtsInfo );

                  uiNewStc = stCurrentPtsInfo.uiSTCSnapShot;
                  uiNewStc += ( uiPictureTime * uiNumberOfPicturesOnTheQueue );
               }

               /* Add a 1/4 of a picture time to get away from the edge of the pass window. */
               uiNewStc += ( uiPictureTime / 4 );

#if NEXUS_HAS_SYNC_CHANNEL
               /* include sync delay (display offset) in determining new stc */
               uiNewStc += videoDecoder->sync.settings.delay;
#endif

               /* Force this picture to be evaluated in vsync mode.
                * It will be displayed on this vsync.
                */
               pTSMInfo->ePictureHandlingMode = BXVD_PictureHandlingMode_eIgnorePTS;

               /* This was added for testing.  NEXUS_StcChannel_SetStc can't be called from this module. */
                rc = NEXUS_StcChannel_SetStc_isr( stcChannel, uiNewStc );
                if (rc) BERR_TRACE(rc);
            }
         }
      }

      return; /* TODO: don't do any other stuff when in low latency mode? */

   }  /* end of if ( low latency mode is enabled ) */

   if (!videoDecoder->trickState.maxFrameRepeat || videoDecoder->trickState.tsmEnabled != NEXUS_TsmMode_eSimulated) {
       return;
   }

   /* Get the parameters of the next picture on the delivery queue. */
   rc = BXVD_GetPPBParameterQueueInfo_isr(videoDecoder->dec, &pstPPBParameterInfo, 1, &n);
   if (rc || !n) {
       return;
   }

    /* If the same picture is sitting at the head of the queue, bump the display count.
    * Note: this is not the picture being displayed, but the easiest way to detect that a picture is being repeated.
    */
    if ( videoDecoder->maxFrameRepeat.pictureId == pstPPBParameterInfo->uiSerialNumber ) {
        videoDecoder->maxFrameRepeat.pictureDisplayCount++;
        if (videoDecoder->maxFrameRepeat.pictureDisplayCount >= videoDecoder->trickState.maxFrameRepeat ) {
            BDBG_MSG(("maxFrameRepeat: id=%d max=%d,%d", pstPPBParameterInfo->uiSerialNumber, videoDecoder->maxFrameRepeat.pictureDisplayCount, videoDecoder->trickState.maxFrameRepeat));
            pTSMInfo->ePictureHandlingMode = BXVD_PictureHandlingMode_eIgnorePTS;
            videoDecoder->maxFrameRepeat.pictureDisplayCount = 0;
        }
    }
    else {
        /* A new picture, reset the values. */
        videoDecoder->maxFrameRepeat.pictureId = pstPPBParameterInfo->uiSerialNumber;
        videoDecoder->maxFrameRepeat.pictureDisplayCount = 0;
    }
}


/**
NEXUS_VideoDecoder_P_OpenChannel is called when the VideoDecoder is connected to a Display.
This must happen before Start.
This allows for power down during analog video decode.
It also allows for XVD/VDC memory sharing during analog video decode.
It's important that no BMEM_Alloc occur in NEXUS_VideoDecoder_P_OpenChannel to prevent memory fragmentation.
**/
NEXUS_Error NEXUS_VideoDecoder_P_OpenChannel(NEXUS_VideoDecoderHandle videoDecoder)
{
    BXVD_ChannelSettings channelSettings;
    BERR_Code rc;
    unsigned channelIndex = videoDecoder - videoDecoder->device->channel;
    BAVC_VideoCompressionStd magnum_formats[10];

    BDBG_ASSERT(!videoDecoder->dec);

    rc = BXVD_GetChannelDefaultSettings(videoDecoder->device->xvd, channelIndex, &channelSettings);
    if (rc) return BERR_TRACE(rc);

    if (NEXUS_GetEnv("xvd_removal_delay")) {
        channelSettings.ulRemovalDelay = NEXUS_atoi(NEXUS_GetEnv("xvd_removal_delay"));
    }

    channelSettings.eChannelMode = BXVD_ChannelMode_eVideo;
    channelSettings.peVideoCmprStdList = magnum_formats;
    channelSettings.bAVC51Enable = videoDecoder->openSettings.openSettings.avc51Enabled;
    channelSettings.bExcessDirModeEnable = videoDecoder->openSettings.openSettings.excessDirModeEnabled;

#if (BCHP_CHIP == 7405)
    if (videoDecoder->mfdIndex == 1 && videoDecoder->mosaicSettings.maxHeight > 576) {
        videoDecoder->mosaicSettings.maxWidth = 720;
        videoDecoder->mosaicSettings.maxHeight = 576;
        BDBG_WRN(("forcing 7405 PIP to SD resolution"));
    }
#endif

    NEXUS_VideoDecoder_P_ConvertCapabilities(videoDecoder, &channelSettings);
    if (videoDecoder->openSettings.openSettings.heap) {
       channelSettings.hChannelPictureHeap = NEXUS_Heap_GetMemHandle(videoDecoder->openSettings.openSettings.heap);
    }

    channelSettings.bSVC3DModeEnable = videoDecoder->openSettings.openSettings.svc3dSupported;

    BDBG_MODULE_MSG(nexus_flow_video_decoder, ("open %p, XVD channel %d", videoDecoder, channelIndex));
    rc = BXVD_OpenChannel(videoDecoder->device->xvd, &videoDecoder->dec, channelIndex, &channelSettings);
    if (rc) return BERR_TRACE(rc);
    rc = BXVD_Userdata_Open(videoDecoder->dec, &videoDecoder->userdata.handle, NULL);
    if (rc) {rc=BERR_TRACE(rc); goto error;}
    rc = BXVD_Userdata_InstallInterruptCallback(videoDecoder->userdata.handle, (BINT_CallbackFunc)NEXUS_VideoDecoder_P_UserdataReady_isr, videoDecoder, 0);
    if (rc) {rc=BERR_TRACE(rc); goto error;}

    if (videoDecoder->mosaicMode) {
        videoDecoder->device->mosaicCount++;
    }
    if (!videoDecoder->mosaicMode || videoDecoder->device->mosaicCount == 1) {
        BXDM_DisplayInterruptHandler_Handle hXdmDih = NULL;
        unsigned xdmIndex = 0;
        /* supports scalable XDM mapping */
        for(xdmIndex = 0; xdmIndex < BXVD_DisplayInterrupt_eMax; xdmIndex++) {
            if(!videoDecoder->device->xdmInUse[xdmIndex]) {
                /* mark the XDM usage flag when found */
                videoDecoder->device->xdmInUse[xdmIndex] = true;
                videoDecoder->xdmIndex = xdmIndex;
                break;
            }
        }
        if(xdmIndex >= BXVD_DisplayInterrupt_eMax) {
            BDBG_ERR(("MFD %d runs out of XDM!", videoDecoder->mfdIndex));
            rc = BERR_INVALID_PARAMETER;
            goto error;
        }

        hXdmDih = videoDecoder->device->hXdmDih[videoDecoder->xdmIndex];
        if ( NULL == hXdmDih )
        {
           rc = BXVD_InstallDeviceInterruptCallback(videoDecoder->device->xvd,
               BXVD_DeviceInterrupt_ePictureDataReady0 + videoDecoder->xdmIndex,
               NEXUS_VideoDecoder_P_DataReady_isr, videoDecoder, 0);
        }
        else
        {
           rc = BXDM_DisplayInterruptHandler_InstallCallback_PictureDataReadyInterrupt(hXdmDih,
                    NEXUS_VideoDecoder_P_PictureDataReady_isr, videoDecoder, 0);
        }
        if (rc) {rc=BERR_TRACE(rc); goto error;}

        videoDecoder->isInterruptChannel = true;
    }
    else if (videoDecoder->mosaicMode) {
        /* this isn't the first mosaic, so look up the xdmIndex */
        unsigned i;
        for (i=0;i<NEXUS_NUM_XVD_CHANNELS;i++) {
            NEXUS_VideoDecoderHandle v = &videoDecoder->device->channel[i];
            if (v->mosaicMode && (v != videoDecoder) && v->isInterruptChannel) {
                videoDecoder->xdmIndex = v->xdmIndex;
            }
        }
    }

    rc = BXVD_InstallInterruptCallback(videoDecoder->dec, BXVD_Interrupt_eRequestSTC, NEXUS_VideoDecoder_P_RequestStc_isr, videoDecoder, 0);
    if (rc) {rc=BERR_TRACE(rc); goto error;}
    rc = BXVD_InstallInterruptCallback(videoDecoder->dec, BXVD_Interrupt_ePTSError, NEXUS_VideoDecoder_P_PtsError_isr, videoDecoder, 0);
    if (rc) {rc=BERR_TRACE(rc); goto error;}
    rc = BXVD_InstallInterruptCallback(videoDecoder->dec, BXVD_Interrupt_ePictureParameters, NEXUS_VideoDecoder_P_PictureParams_isr, videoDecoder, 0);
    if (rc) {rc=BERR_TRACE(rc); goto error;}
    rc = BXVD_InstallInterruptCallback(videoDecoder->dec, BXVD_Interrupt_eFirstPTSPassed, NEXUS_VideoDecoder_P_FirstPtsPassed_isr, videoDecoder, 0);
    if (rc) {rc=BERR_TRACE(rc); goto error;}
    rc = BXVD_InstallInterruptCallback(videoDecoder->dec, BXVD_Interrupt_ePtsStcOffset, NEXUS_VideoDecoder_P_PtsStcOffset_isr, videoDecoder, 0);
    if (rc) {rc=BERR_TRACE(rc); goto error;}
    rc = BXVD_InstallInterruptCallback(videoDecoder->dec, BXVD_Interrupt_eDecodeError, NEXUS_VideoDecoder_P_DecodeError_isr, videoDecoder, 0);
    if (rc) {rc=BERR_TRACE(rc); goto error;}
    rc = BXVD_EnableInterrupt(videoDecoder->dec, BXVD_Interrupt_eDecodeError, true);
    if (rc) {rc=BERR_TRACE(rc); goto error;}
    rc = BXVD_InstallInterruptCallback(videoDecoder->dec, BXVD_Interrupt_eTSMResult, NEXUS_VideoDecoder_P_TsmResult_isr, videoDecoder, 0);
    if (rc) {rc=BERR_TRACE(rc); goto error;}
    rc = BXVD_EnableInterrupt(videoDecoder->dec, BXVD_Interrupt_eTSMResult, true);
    if (rc) {rc=BERR_TRACE(rc); goto error;}
    rc = BXVD_InstallInterruptCallback(videoDecoder->dec, BXVD_Interrupt_eClipStart,NEXUS_VideoDecoder_P_FirstPtsPassed_isr, videoDecoder, 0);
    if (rc) {rc=BERR_TRACE(rc); goto error;}
    rc = BXVD_InstallInterruptCallback(videoDecoder->dec, BXVD_Interrupt_ePictureExtensionData, NEXUS_VideoDecoder_P_PictureExtensionData_isr, videoDecoder, 0);
    if (rc) {rc=BERR_TRACE(rc); goto error;}
    rc = BXVD_InstallInterruptCallback(videoDecoder->dec, BXVD_Interrupt_eChunkDone, NEXUS_VideoDecoder_P_FnrtChunkDone_isr, videoDecoder, 0);
    if (rc) {rc=BERR_TRACE(rc); goto error;}
    /* BXVD_Interrupt_ePictureExtensionData interrupt is enabled in NEXUS_VideoDecoder_P_Start_priv */
    /* we must apply all settings which may have been deferred here */
    rc = NEXUS_VideoDecoder_P_SetSettings(videoDecoder, &videoDecoder->settings, true);
    if (rc) {rc=BERR_TRACE(rc); goto error;}
    rc = NEXUS_VideoDecoder_P_SetCrcFifoSize(videoDecoder, false);
    if (rc) {rc=BERR_TRACE(rc); goto error;}
#if NEXUS_HAS_SYNC_CHANNEL
    rc = NEXUS_VideoDecoder_SetSyncSettings_priv(videoDecoder, &videoDecoder->sync.settings);
    if (rc) {rc=BERR_TRACE(rc); goto error;}
#endif
    NEXUS_VideoDecoder_P_ApplyDisplayInformation(videoDecoder);

    return 0;

error:
    NEXUS_VideoDecoder_P_CloseChannel(videoDecoder);
    BDBG_ASSERT(rc);
    return rc;
}

void NEXUS_VideoDecoder_P_CloseChannel(NEXUS_VideoDecoderHandle videoDecoder)
{
    BDBG_ASSERT(videoDecoder->dec);

    if (videoDecoder->userdata.handle) {
        BXVD_Userdata_UninstallInterruptCallback(videoDecoder->userdata.handle, (BINT_CallbackFunc)NEXUS_VideoDecoder_P_UserdataReady_isr);
        BXVD_Userdata_Close(videoDecoder->userdata.handle);
        videoDecoder->userdata.handle = NULL;
    }

    (void)NEXUS_VideoDecoder_P_SetCrcFifoSize(videoDecoder, true);

    BXVD_UnInstallInterruptCallback(videoDecoder->dec, BXVD_Interrupt_ePTSError);
    BXVD_UnInstallInterruptCallback(videoDecoder->dec, BXVD_Interrupt_eRequestSTC);
    BXVD_UnInstallInterruptCallback(videoDecoder->dec, BXVD_Interrupt_ePictureParameters);
    BXVD_UnInstallInterruptCallback(videoDecoder->dec, BXVD_Interrupt_eFirstPTSPassed);
    BXVD_UnInstallInterruptCallback(videoDecoder->dec, BXVD_Interrupt_ePtsStcOffset);
    BXVD_UnInstallInterruptCallback(videoDecoder->dec, BXVD_Interrupt_eDecodeError);
    BXVD_UnInstallInterruptCallback(videoDecoder->dec, BXVD_Interrupt_eTSMResult);
    BXVD_UnInstallInterruptCallback(videoDecoder->dec, BXVD_Interrupt_eClipStart);
    BXVD_UnInstallInterruptCallback(videoDecoder->dec, BXVD_Interrupt_ePictureExtensionData);
    BXVD_UnInstallInterruptCallback(videoDecoder->dec, BXVD_Interrupt_eChunkDone);
    if (videoDecoder->mosaicMode) {
        --videoDecoder->device->mosaicCount;
    }
    if (videoDecoder->isInterruptChannel) {
        BXDM_DisplayInterruptHandler_Handle hXdmDih = NULL;

        hXdmDih = videoDecoder->device->hXdmDih[videoDecoder->xdmIndex];

        if ( NULL == hXdmDih )
        {
           BXVD_UnInstallDeviceInterruptCallback(videoDecoder->device->xvd,
               BXVD_DeviceInterrupt_ePictureDataReady0 + videoDecoder->xdmIndex);
        }
        else
        {
           BXDM_DisplayInterruptHandler_UnInstallCallback_PictureDataReadyInterrupt(hXdmDih);
        }
        videoDecoder->isInterruptChannel = false;

        /* was this a mosaic and are there other mosaics still active? */
        if (videoDecoder->mosaicMode && videoDecoder->device->mosaicCount) {
            unsigned i;
            /* reassign to another mosaic */
            for (i=0;i<NEXUS_NUM_XVD_CHANNELS;i++) {
                NEXUS_VideoDecoderHandle v = &videoDecoder->device->channel[i];
                if (v->mosaicMode && (v != videoDecoder) && v->dec) {
                    BERR_Code rc;
                    if  ( NULL == hXdmDih ) {
                        rc = BXVD_InstallDeviceInterruptCallback(v->device->xvd,
                            BXVD_DeviceInterrupt_ePictureDataReady0 + videoDecoder->xdmIndex,
                            NEXUS_VideoDecoder_P_DataReady_isr, v, 0);
                    }
                    else {
                        rc = BXDM_DisplayInterruptHandler_InstallCallback_PictureDataReadyInterrupt(hXdmDih,
                            (BXDM_DisplayInterruptHandler_PictureDataReady_isr) NEXUS_VideoDecoder_P_DataReady_isr,
                            v, 0);
                    }
                    if (rc) {rc=BERR_TRACE(rc);} /* fall through */
                    v->isInterruptChannel = true;
                    break;
                }
            }
        }
        else { /* if XDM is uninstalled, clear its usage flag */
            videoDecoder->device->xdmInUse[videoDecoder->xdmIndex] = false;
        }
    }
    BDBG_MODULE_MSG(nexus_flow_video_decoder, ("close %p XVD channel", videoDecoder));
    BXVD_CloseChannel(videoDecoder->dec);
    videoDecoder->dec = NULL;
}

void NEXUS_VideoDecoder_GetDefaultOpenSettings(NEXUS_VideoDecoderOpenSettings *pOpenSettings)
{
    unsigned i;

    BKNI_Memset(pOpenSettings, 0, sizeof(*pOpenSettings));
    pOpenSettings->userDataBufferSize = 16 * 1024;

    for (i=0;i<NEXUS_VideoCodec_eMax;i++) {
        if (g_NEXUS_videoDecoderModuleSettings.supportedCodecs[i] == NEXUS_VideoCodec_eH264_Svc ||
            g_NEXUS_videoDecoderModuleSettings.supportedCodecs[i] == NEXUS_VideoCodec_eH264_Mvc)
        {
            pOpenSettings->enhancementPidChannelSupported = true;
            break;
        }
    }
}

NEXUS_VideoDecoderHandle NEXUS_VideoDecoder_P_Open_Avd(unsigned index, const NEXUS_VideoDecoderOpenSettings *pOpenSettings)
{
    NEXUS_VideoDecoderOpenMosaicSettings mosaicSettings;
    NEXUS_VideoDecoder_P_GetMosaicOpenSettings(&mosaicSettings, pOpenSettings);
    if(pOpenSettings==NULL) {
        NEXUS_VideoDecoder_GetDefaultOpenSettings(&mosaicSettings.openSettings);
    }
    return NEXUS_VideoDecoder_P_Open(-1, index, &mosaicSettings);
}

void NEXUS_VideoDecoder_GetDefaultOpenMosaicSettings(NEXUS_VideoDecoderOpenMosaicSettings *pOpenSettings)
{
    BKNI_Memset(pOpenSettings, 0, sizeof(*pOpenSettings));
    NEXUS_VideoDecoder_GetDefaultOpenSettings(&pOpenSettings->openSettings);
    pOpenSettings->maxWidth = 1920;
    pOpenSettings->maxHeight = 1080;
    pOpenSettings->openSettings.enhancementPidChannelSupported = false;
}

NEXUS_VideoDecoderHandle NEXUS_VideoDecoder_P_OpenMosaic_Avd(unsigned parentIndex, unsigned index, const NEXUS_VideoDecoderOpenMosaicSettings *pOpenSettings)
{
    BERR_Code rc=0;
    if (!pOpenSettings) {
        /* no meaningful default. params required. */
        rc=BERR_TRACE(BERR_INVALID_PARAMETER);
        return NULL;
    }
    return NEXUS_VideoDecoder_P_Open(parentIndex, index, pOpenSettings);
}

void NEXUS_VideoDecoder_P_Close_Generic(NEXUS_VideoDecoderHandle videoDecoder)
{
    if (videoDecoder->started) {
        NEXUS_VideoDecoder_Stop(videoDecoder);
    }

    if (videoDecoder->input.destination) {
        BDBG_ERR(("NEXUS_VideoDecoder_Close: closing decoder while still using it as input. You must call NEXUS_VideoWindow_RemoveInput and NEXUS_VideoInput_Shutdown before Close."));
        BDBG_ASSERT(0);
    }
    else if (videoDecoder->input.ref_cnt) {
        BDBG_ERR(("NEXUS_VideoDecoder_Close: closing decoder with active input. You must call NEXUS_VideoInput_Shutdown before Close."));
        BDBG_ASSERT(0);
    }

    (void)NEXUS_VideoDecoder_SetPowerState(videoDecoder, false);

    if (videoDecoder->source_changed_event) {
        BKNI_DestroyEvent(videoDecoder->source_changed_event);
        videoDecoder->source_changed_event = NULL;
    }
    if (videoDecoder->channelChangeReportEventHandler) {
        NEXUS_UnregisterEvent(videoDecoder->channelChangeReportEventHandler);
        videoDecoder->channelChangeReportEventHandler = NULL;
    }
    if (videoDecoder->channelChangeReportEvent) {
        BKNI_DestroyEvent(videoDecoder->channelChangeReportEvent);
        videoDecoder->channelChangeReportEvent = NULL;
    }
    if (videoDecoder->s3DTVStatusEventHandler) {
        NEXUS_UnregisterEvent(videoDecoder->s3DTVStatusEventHandler);
        videoDecoder->s3DTVStatusEventHandler = NULL;
    }
    if (videoDecoder->s3DTVStatusEvent) {
        BKNI_DestroyEvent(videoDecoder->s3DTVStatusEvent);
        videoDecoder->s3DTVStatusEvent = NULL;
    }

    if(videoDecoder->enhancementRave) {
        LOCK_TRANSPORT();
        NEXUS_Rave_Close_priv(videoDecoder->enhancementRave);
        UNLOCK_TRANSPORT();
        videoDecoder->enhancementRave = NULL;
    }

    if (videoDecoder->rave) {
        LOCK_TRANSPORT();
        NEXUS_Rave_Close_priv(videoDecoder->rave);
        UNLOCK_TRANSPORT();
        videoDecoder->rave = NULL;
    }

    if (videoDecoder->userdataCallback) {
        NEXUS_IsrCallback_Destroy(videoDecoder->userdataCallback);
        videoDecoder->userdataCallback = NULL;
    }
    if (videoDecoder->sourceChangedCallback) {
        NEXUS_IsrCallback_Destroy(videoDecoder->sourceChangedCallback);
        videoDecoder->sourceChangedCallback = NULL;
    }
    if (videoDecoder->streamChangedCallback) {
        NEXUS_IsrCallback_Destroy(videoDecoder->streamChangedCallback);
        videoDecoder->streamChangedCallback = NULL;
    }
    if (videoDecoder->ptsErrorCallback) {
        NEXUS_IsrCallback_Destroy(videoDecoder->ptsErrorCallback);
        videoDecoder->ptsErrorCallback = NULL;
    }
    if (videoDecoder->firstPtsCallback) {
        NEXUS_IsrCallback_Destroy(videoDecoder->firstPtsCallback);
        videoDecoder->firstPtsCallback = NULL;
    }
    if (videoDecoder->firstPtsPassedCallback) {
        NEXUS_IsrCallback_Destroy(videoDecoder->firstPtsPassedCallback);
        videoDecoder->firstPtsPassedCallback = NULL;
    }
    if (videoDecoder->dataReadyCallback) {
        NEXUS_IsrCallback_Destroy(videoDecoder->dataReadyCallback);
        videoDecoder->dataReadyCallback = NULL;
    }
    if (videoDecoder->fifoEmpty.callback) {
        NEXUS_IsrCallback_Destroy(videoDecoder->fifoEmpty.callback);
        videoDecoder->fifoEmpty.callback = NULL;
    }
    if (videoDecoder->afdChangedCallback) {
        NEXUS_IsrCallback_Destroy(videoDecoder->afdChangedCallback);
        videoDecoder->afdChangedCallback = NULL;
    }
    if (videoDecoder->decodeErrorCallback) {
        NEXUS_IsrCallback_Destroy(videoDecoder->decodeErrorCallback);
        videoDecoder->decodeErrorCallback = NULL;
    }
    if (videoDecoder->s3DTVChangedCallback) {
        NEXUS_IsrCallback_Destroy(videoDecoder->s3DTVChangedCallback);
        videoDecoder->s3DTVChangedCallback = NULL;
    }
    if (videoDecoder->userdata.buf) {
        BMEM_Heap_FreeCached(g_pCoreHandles->heap[g_NEXUS_videoDecoderModuleSettings.hostAccessibleHeapIndex], videoDecoder->userdata.buf);
        videoDecoder->userdata.buf = NULL;
    }

    if (videoDecoder->playback.firstPtsCallback) {
        NEXUS_IsrCallback_Destroy(videoDecoder->playback.firstPtsCallback);
        videoDecoder->playback.firstPtsCallback = NULL;
    }
    if (videoDecoder->playback.firstPtsPassedCallback) {
        NEXUS_IsrCallback_Destroy(videoDecoder->playback.firstPtsPassedCallback);
        videoDecoder->playback.firstPtsPassedCallback = NULL;
    }
    if (videoDecoder->fnrtChunkDoneCallback) {
        NEXUS_IsrCallback_Destroy(videoDecoder->fnrtChunkDoneCallback);
        videoDecoder->fnrtChunkDoneCallback = NULL;
    }

    return;
}

void NEXUS_VideoDecoder_P_Close_Avd(NEXUS_VideoDecoderHandle videoDecoder)
{
    unsigned avdIndex;
    unsigned index;
    BDBG_OBJECT_ASSERT(videoDecoder, NEXUS_VideoDecoder);

    avdIndex = videoDecoder->device - g_NEXUS_videoDecoderXvdDevices;
    index = videoDecoder - videoDecoder->device->channel;

    NEXUS_VideoDecoder_P_Close_Generic(videoDecoder);

    /* soft decoder doesn't need this */
    BDBG_MSG(("Close avd[%u].channel[%u].", avdIndex, index));
    g_chanInUse[avdIndex][index] = false;

    /* clear the decoder usage status if this is the last reference of that decoder. */
    if(!videoDecoder->mosaicMode) {
        for(index=0; index<NEXUS_NUM_VIDEO_DECODERS; index++) {
            if(g_videoDecoders[index] == videoDecoder) {
                g_videoDecoders[index] = NULL;
            }
        }
    }

    BDBG_OBJECT_UNSET(videoDecoder, NEXUS_VideoDecoder);
    BKNI_Memset(videoDecoder, 0, sizeof(*videoDecoder));
}

NEXUS_Error NEXUS_VideoDecoder_P_SetSettings_Avd(NEXUS_VideoDecoderHandle videoDecoder, const NEXUS_VideoDecoderSettings *pSettings)
{
    BDBG_OBJECT_ASSERT(videoDecoder, NEXUS_VideoDecoder);
    return NEXUS_VideoDecoder_P_SetSettings(videoDecoder, pSettings, false);
}

NEXUS_Error NEXUS_VideoDecoder_P_SetChannelChangeMode(NEXUS_VideoDecoderHandle videoDecoder, NEXUS_VideoDecoder_ChannelChangeMode channelChangeMode)
{
    BXVD_ChannelChangeMode mode;
    BERR_Code rc;

    switch (channelChangeMode) {
    case NEXUS_VideoDecoder_ChannelChangeMode_eMute:
        mode = BXVD_ChannelChangeMode_eMute;
        break;
    case NEXUS_VideoDecoder_ChannelChangeMode_eHoldUntilTsmLock:
        mode = BXVD_ChannelChangeMode_eLastFramePreviousChannel;
        break;
    case NEXUS_VideoDecoder_ChannelChangeMode_eMuteUntilFirstPicture:
        mode = BXVD_ChannelChangeMode_eMuteWithFirstPicturePreview;
        break;
    case NEXUS_VideoDecoder_ChannelChangeMode_eHoldUntilFirstPicture:
        mode = BXVD_ChannelChangeMode_eLastFramePreviousWithFirstPicturePreview;
        break;
    default:
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }
    rc = BXVD_SetChannelChangeMode(videoDecoder->dec, mode);
    if (rc) return BERR_TRACE(rc);
    return NEXUS_SUCCESS;
}

static NEXUS_Error NEXUS_VideoDecoder_P_SetSettings(NEXUS_VideoDecoderHandle videoDecoder, const NEXUS_VideoDecoderSettings *pSettings, bool force)
{
    BERR_Code rc = 0;
    bool setMute = false;
    bool setFreeze = false;
    bool setUserdata = false;
    bool setPtsOffset = false;

    BDBG_OBJECT_ASSERT(videoDecoder, NEXUS_VideoDecoder);
    if (!videoDecoder->dec) {
        goto skip;
    }

    if (force || (pSettings->fnrtSettings.enable != videoDecoder->settings.fnrtSettings.enable) || (pSettings->fnrtSettings.preChargeCount != videoDecoder->settings.fnrtSettings.preChargeCount)) {
        BXVD_PVR_FNRTSettings settings;
        BXVD_PVR_GetDefaultFNRTSettings(videoDecoder->dec, &settings);
        settings.bEnabled = pSettings->fnrtSettings.enable;
        settings.uiPreChargeCount = pSettings->fnrtSettings.preChargeCount;
        rc = BXVD_PVR_SetFNRTSettings(videoDecoder->dec, &settings);
        if (rc) return BERR_TRACE(rc);
    }

    /* convert to single XVD calls. this requires that Nexus init in the same configuration as XVD, or that it asserts its configuration elsewhere */
    if (force || pSettings->freeze != videoDecoder->settings.freeze) {
        setFreeze = true; /* defer to bottom */
    }
    if (pSettings->mute != videoDecoder->settings.mute) {
        setMute = true; /* defer to bottom */
    }
    if (force || pSettings->channelChangeMode != videoDecoder->settings.channelChangeMode) {
        rc = NEXUS_VideoDecoder_P_SetChannelChangeMode(videoDecoder, pSettings->channelChangeMode);
        if (rc) return BERR_TRACE(rc);
    }
    if (pSettings->userDataEnabled != videoDecoder->settings.userDataEnabled) {
        setUserdata = true;
    }
    if (pSettings->ptsOffset != videoDecoder->settings.ptsOffset) {
        setPtsOffset = true;
    }
    if (force || pSettings->dropFieldMode != videoDecoder->settings.dropFieldMode) {
        rc = BXVD_SetPictureDropMode(videoDecoder->dec, pSettings->dropFieldMode ? BXVD_PictureDropMode_eField : BXVD_PictureDropMode_eFrame);
        if (rc) return BERR_TRACE(rc);
    }
    if (force || pSettings->stillContentInterpolationMode != videoDecoder->settings.stillContentInterpolationMode) {
        BDBG_CWARNING(NEXUS_StillContentInterpolationMode_eMax == (NEXUS_StillContentInterpolationMode)BXVD_StillContentInterpolationMode_eMaxModes);
        rc = BXVD_SetInterpolationModeForStillContent(videoDecoder->dec, pSettings->stillContentInterpolationMode);
        if (rc) return BERR_TRACE(rc);
    }
    if (force || pSettings->movingContentInterpolationMode != videoDecoder->settings.movingContentInterpolationMode) {
        BDBG_CWARNING(NEXUS_MovingContentInterpolationMode_eMax == (NEXUS_MovingContentInterpolationMode)BXVD_MovingContentInterpolationMode_eMaxModes);
        rc = BXVD_SetInterpolationModeForMovingContent(videoDecoder->dec, pSettings->movingContentInterpolationMode);
        if (rc) return BERR_TRACE(rc);
    }
    if (force || pSettings->scanMode != videoDecoder->settings.scanMode) {
        BDBG_CWARNING(NEXUS_VideoDecoderScanMode_eMax == (NEXUS_VideoDecoderScanMode)BXVD_1080pScanMode_eMax);
        rc = BXVD_Set1080pScanMode(videoDecoder->dec, pSettings->scanMode);
        if (rc) return BERR_TRACE(rc);
    }
    if(force || pSettings->sourceOrientation != videoDecoder->settings.sourceOrientation || pSettings->customSourceOrientation != videoDecoder->settings.customSourceOrientation) {
        BXVD_3DSetting setting3d;
        BDBG_CASSERT(NEXUS_VideoDecoderSourceOrientation_e2D == (NEXUS_VideoDecoderSourceOrientation)BXVD_Orientation_e2D);
        BDBG_CASSERT(NEXUS_VideoDecoderSourceOrientation_e3D_LeftRight == (NEXUS_VideoDecoderSourceOrientation)BXVD_Orientation_eLeftRight);
        BDBG_CASSERT(NEXUS_VideoDecoderSourceOrientation_e3D_OverUnder == (NEXUS_VideoDecoderSourceOrientation)BXVD_Orientation_eOverUnder);
        BDBG_CASSERT(NEXUS_VideoDecoderSourceOrientation_e3D_LeftRightFullFrame == (NEXUS_VideoDecoderSourceOrientation)BXVD_Orientation_eLeftRightFullFrame);
        BDBG_CASSERT(NEXUS_VideoDecoderSourceOrientation_e3D_RightLeftFullFrame == (NEXUS_VideoDecoderSourceOrientation)BXVD_Orientation_eRightLeftFullFrame);
        BDBG_CASSERT(NEXUS_VideoDecoderSourceOrientation_e3D_LeftRightEnhancedResolution == (NEXUS_VideoDecoderSourceOrientation)BXVD_Orientation_eLeftRightEnhancedResolution);
        BDBG_CWARNING(NEXUS_VideoDecoderSourceOrientation_eMax == (NEXUS_VideoDecoderSourceOrientation)BXVD_Orientation_eMax);
        setting3d.bOverrideOrientation = pSettings->customSourceOrientation;
        setting3d.eOrientation = pSettings->sourceOrientation;
        setting3d.bSetNextPointer = false;
        rc = BXVD_Set3D(videoDecoder->dec,  &setting3d);
        if (rc) return BERR_TRACE(rc);
    }
    if (force || pSettings->horizontalOverscanMode != videoDecoder->settings.horizontalOverscanMode) {
        BDBG_CWARNING(NEXUS_VideoDecoderHorizontalOverscanMode_eMax == (NEXUS_VideoDecoderHorizontalOverscanMode)BXVD_HorizontalOverscanMode_eMax);
        rc = BXVD_SetHorizontalOverscanMode(videoDecoder->dec, pSettings->horizontalOverscanMode);
        if (rc) return BERR_TRACE(rc);
    }
skip:
    NEXUS_IsrCallback_Set(videoDecoder->userdataCallback, &pSettings->appUserDataReady);
    NEXUS_IsrCallback_Set(videoDecoder->sourceChangedCallback, &pSettings->sourceChanged);
    NEXUS_IsrCallback_Set(videoDecoder->streamChangedCallback, &pSettings->streamChanged);
    NEXUS_IsrCallback_Set(videoDecoder->ptsErrorCallback, &pSettings->ptsError);
    NEXUS_IsrCallback_Set(videoDecoder->firstPtsCallback, &pSettings->firstPts);
    NEXUS_IsrCallback_Set(videoDecoder->firstPtsPassedCallback, &pSettings->firstPtsPassed);
    NEXUS_IsrCallback_Set(videoDecoder->fifoEmpty.callback, &pSettings->fifoEmpty);
    NEXUS_IsrCallback_Set(videoDecoder->afdChangedCallback, &pSettings->afdChanged);
    NEXUS_IsrCallback_Set(videoDecoder->decodeErrorCallback, &pSettings->decodeError);
    NEXUS_IsrCallback_Set(videoDecoder->fnrtChunkDoneCallback, &pSettings->fnrtSettings.chunkDone);

    if ( pSettings->fifoThreshold != videoDecoder->settings.fifoThreshold )
    {
        LOCK_TRANSPORT();

        rc = NEXUS_Rave_SetCdbThreshold_priv(videoDecoder->rave, pSettings->fifoThreshold);

        UNLOCK_TRANSPORT();

        if (rc) return BERR_TRACE(rc);
    }

    videoDecoder->settings = *pSettings;

    if (setMute) {
        /* this setting is shared with SyncChannel, and depends on ch chg mode */
        rc = NEXUS_VideoDecoder_P_SetMute(videoDecoder);
        if (rc) return BERR_TRACE(rc);
    }
    if (setFreeze) {
        /* this setting is shared with SyncChannel, and depends on ch chg mode */
        rc = NEXUS_VideoDecoder_P_SetFreeze(videoDecoder);
        if (rc) return BERR_TRACE(rc);
    }
    if (setUserdata) {
        /* this setting is shared with VideoInput's priv interface */
        rc = NEXUS_VideoDecoder_P_SetUserdata(videoDecoder);
        if (rc) return BERR_TRACE(rc);
    }
    if (setPtsOffset) {
        rc = NEXUS_VideoDecoder_P_SetPtsOffset(videoDecoder);
        if (rc) return BERR_TRACE(rc);
    }

    return NEXUS_SUCCESS;
}

void NEXUS_VideoDecoder_GetDefaultStartSettings(NEXUS_VideoDecoderStartSettings *pStartSettings)
{
    BKNI_Memset(pStartSettings, 0, sizeof(*pStartSettings));
    pStartSettings->codec = NEXUS_VideoCodec_eMpeg2;
    pStartSettings->enhancementPidChannel = NULL;
    pStartSettings->nonRealTime = false;
    return;
}

#define NEXUS_VIDEO_DECODER_P_FIFO_WATCHDOG_MAX_FIFO_STATIC_COUNT 4

static void NEXUS_VideoDecoder_P_UpdateFifoStaticCount_isr(
    NEXUS_VideoDecoderHandle videoDecoder
)
{
    BERR_Code rc;
    BXDM_PictureProvider_PTSInfo ptsInfo;
    unsigned cdbValidPointer, cdbReadPointer;

    /* We can tell if the source has run dry by looking at the RAVE CDB VALID pointer. If it stops moving, then
    either there's no source, or the XVD DM has throttled it (e.g. pause). If XVD DM is throttling, then there must
    be pictures in the queue. If there are no pictures in the queue, then the source has run dry. */
    NEXUS_Rave_GetCdbPointers_isr(videoDecoder->rave, &cdbValidPointer, &cdbReadPointer);
    if (cdbValidPointer == videoDecoder->fifoWatchdog.lastCdbValidPointer 
        && cdbReadPointer == videoDecoder->fifoWatchdog.lastCdbReadPointer 
        && videoDecoder->pictureDeliveryCount == videoDecoder->fifoWatchdog.lastPictureDeliveryCount) 
    {
        if (videoDecoder->fifoWatchdog.staticCount < NEXUS_VIDEO_DECODER_P_FIFO_WATCHDOG_MAX_FIFO_STATIC_COUNT) 
        {
            videoDecoder->fifoWatchdog.staticCount++;
        }
    }
    else 
    {
        videoDecoder->fifoWatchdog.staticCount = 0;
    }

    if ( videoDecoder->dec )
    {
        rc = BXVD_GetPTS_isr(videoDecoder->dec, &ptsInfo);
    }
    else if ( videoDecoder->xdm.pictureProvider )
    {
        rc = BXDM_PictureProvider_GetCurrentPTSInfo_isr(videoDecoder->xdm.pictureProvider, &ptsInfo);
    }
    else
    {
        rc = BERR_NOT_SUPPORTED;
    }

    if (!rc)
    {
        /* reset static count if PTS is still moving */
        if (videoDecoder->fifoWatchdog.lastPtsValid && ptsInfo.ui32RunningPTS != videoDecoder->fifoWatchdog.lastPts)
        {
            videoDecoder->fifoWatchdog.staticCount = 0;
        }
        
        videoDecoder->fifoWatchdog.lastPts = ptsInfo.ui32RunningPTS;
        videoDecoder->fifoWatchdog.lastPtsValid = true;
    }

    videoDecoder->fifoWatchdog.lastPictureDeliveryCount = videoDecoder->pictureDeliveryCount;
    videoDecoder->fifoWatchdog.lastCdbValidPointer = cdbValidPointer;
    videoDecoder->fifoWatchdog.lastCdbReadPointer = cdbReadPointer;
}

#define NEXUS_VIDEO_DECODER_P_FIFO_WATCHDOG_FULL_THRESHOLD 95
#define NEXUS_VIDEO_DECODER_P_FIFO_WATCHDOG_TIMEOUT 125

static void NEXUS_VideoDecoder_P_FifoWatchdog(void *context)
{
    bool decoderRateConsumptionCompatible;
    bool decoderNotConsuming;
    bool shouldFlush, isHung, isCdbFull, isItbFull, isFull, shouldZeroFill;
    bool stcChannelWantsFlush;
    unsigned percentCdbFull, percentItbFull;
    unsigned cdbDepth, cdbSize;
    unsigned itbDepth, itbSize;
    NEXUS_VideoDecoderHandle videoDecoder = context;

    videoDecoder->fifoWatchdog.timer = NULL;

    /* the decoder should consume at a rate that is compatible with our counter threshold and
    watchdog task period (NRT mode is not normal consumption rate compatible) */
    decoderRateConsumptionCompatible = 
        videoDecoder->started 
        && videoDecoder->trickState.rate >= NEXUS_NORMAL_DECODE_RATE;

    /* don't bother incrementing if we aren't supposed to be consuming */
    if (decoderRateConsumptionCompatible)
    {
        BKNI_EnterCriticalSection();
        /* checks valid and read pointers and display queue for staticicity */
        NEXUS_VideoDecoder_P_UpdateFifoStaticCount_isr(videoDecoder);
        BKNI_LeaveCriticalSection();
    }

    decoderNotConsuming = videoDecoder->fifoWatchdog.staticCount >= NEXUS_VIDEO_DECODER_P_FIFO_WATCHDOG_MAX_FIFO_STATIC_COUNT;

    /* hung = not consuming when it should be */
    isHung = decoderRateConsumptionCompatible && decoderNotConsuming;

    BKNI_EnterCriticalSection();
    /* decoder hang detection requires knowledge of buffer depths */
    NEXUS_Rave_GetCdbBufferInfo_isr(videoDecoder->rave, &cdbDepth, &cdbSize);
    NEXUS_Rave_GetItbBufferInfo_isr(videoDecoder->rave, &itbDepth, &itbSize);
    BKNI_LeaveCriticalSection();

    percentCdbFull = cdbSize ? cdbDepth * 100 / cdbSize : 0;
    isCdbFull = percentCdbFull > NEXUS_VIDEO_DECODER_P_FIFO_WATCHDOG_FULL_THRESHOLD;
    percentItbFull = itbSize ? itbDepth * 100 / itbSize : 0;
    isItbFull = percentItbFull > NEXUS_VIDEO_DECODER_P_FIFO_WATCHDOG_FULL_THRESHOLD;

    /* no point in flushing if we can still take some more data; if either buf is "full", new data will be blocked */
    isFull = isCdbFull || isItbFull;
    
    BDBG_MSG(("FIFO Watchdog: CDB: %u; ITB: %u", percentCdbFull, percentItbFull));

    stcChannelWantsFlush = false;
    shouldZeroFill = false;

    if (videoDecoder->startSettings.stcChannel) {
        bool stcRateConsumptionCompatible;
        bool stcFrozen;
        unsigned num, den;

        /* the STC should be running at a rate that is >= 1.0x 
        otherwise it might be too slow and cause a false positive */
        NEXUS_StcChannel_GetRate_priv(videoDecoder->startSettings.stcChannel, &num, &den);
        stcFrozen = NEXUS_StcChannel_IsFrozen_priv(videoDecoder->startSettings.stcChannel);
        stcRateConsumptionCompatible = !stcFrozen && (num >= den + 1);

        /* if stc rate is not compatible, we can't say we are hung */
        isHung = isHung && stcRateConsumptionCompatible;

        LOCK_TRANSPORT();
        NEXUS_StcChannel_ReportDecoderHang_priv(videoDecoder->startSettings.stcChannel,
            NEXUS_StcChannelDecoderType_eVideo0, isHung, percentCdbFull, &stcChannelWantsFlush, &shouldZeroFill);
        UNLOCK_TRANSPORT();
    }

    /* the methods for determining video is locked up due to garbage data in the FIFO break certain host trick modes,
    therefore we require a compile time flag to enable them */
    shouldFlush = stcChannelWantsFlush;
#ifdef NEXUS_VIDEO_DECODER_GARBAGE_FIFO_WATCHDOG_SUPPORT
    shouldFlush = shouldFlush || (isHung && isFull);
#endif

    /* if both flush and zero-fill are set, flush first */
    if (shouldFlush) {
        BDBG_WRN(("Video FIFO watchdog flush"));
        NEXUS_VideoDecoder_Flush(videoDecoder);
        videoDecoder->fifoWatchdog.staticCount = 0;
        if (videoDecoder->startSettings.stcChannel)
        {
            LOCK_TRANSPORT();
            NEXUS_StcChannel_ReportFlush_priv(videoDecoder->startSettings.stcChannel,
                NEXUS_StcChannelDecoderType_eVideo0);
            UNLOCK_TRANSPORT();
        }
    }

    if (shouldZeroFill)
    {
        BDBG_WRN(("Video FIFO watchdog zero-fill"));
        videoDecoder->fifoWatchdog.staticCount = 0;
        BKNI_EnterCriticalSection();
        NEXUS_VideoDecoder_P_SetUnderflowMode_isr(videoDecoder, true);
        BKNI_LeaveCriticalSection();
    }

    videoDecoder->fifoWatchdog.timer = NEXUS_ScheduleTimer(NEXUS_VIDEO_DECODER_P_FIFO_WATCHDOG_TIMEOUT, NEXUS_VideoDecoder_P_FifoWatchdog, videoDecoder);
}

NEXUS_Error NEXUS_VideoDecoder_P_Start_Generic_Prologue(NEXUS_VideoDecoderHandle videoDecoder, const NEXUS_VideoDecoderStartSettings *pStartSettings)
{

    if (videoDecoder->started) {
        BDBG_ERR(("VideoDecoder already started"));
        return BERR_TRACE(NEXUS_NOT_SUPPORTED);
    }
    if (!pStartSettings->pidChannel) {
        BDBG_ERR(("NEXUS_VideoDecoder_Start requires pid channel"));
        return NEXUS_INVALID_PARAMETER;
    }
    if(pStartSettings->enhancementPidChannel && !videoDecoder->enhancementRave) {
        return BERR_TRACE(NEXUS_NOT_SUPPORTED);
    }
    if (videoDecoder->raveDetached || (pStartSettings->enhancementPidChannel && videoDecoder->enhancementRaveDetached)) {
        BDBG_ERR(("NEXUS_VideoDecoder_Start requires RAVE contexts to be attached"));
        return BERR_TRACE(NEXUS_NOT_SUPPORTED);
    }

    videoDecoder->startSettings = *pStartSettings;
    return NEXUS_SUCCESS;
}

NEXUS_Error NEXUS_VideoDecoder_P_Start_Avd(NEXUS_VideoDecoderHandle videoDecoder, const NEXUS_VideoDecoderStartSettings *pStartSettings)
{
    NEXUS_Error rc;

    BDBG_OBJECT_ASSERT(videoDecoder, NEXUS_VideoDecoder);
    BDBG_ASSERT(pStartSettings);

    if (!videoDecoder->dec) {
        BDBG_ERR(("Must call NEXUS_VideoWindow_AddInput before starting VideoDecoder"));
        return BERR_TRACE(NEXUS_NOT_SUPPORTED);
    }

    rc = NEXUS_VideoDecoder_P_Start_Generic_Prologue(videoDecoder, pStartSettings);
    if(rc!=NEXUS_SUCCESS) { return BERR_TRACE(rc); }

    rc = NEXUS_VideoDecoder_P_Start_priv(videoDecoder, pStartSettings, false);
    if(rc!=NEXUS_SUCCESS) { return BERR_TRACE(rc); }

    return NEXUS_SUCCESS;
}

NEXUS_Error NEXUS_VideoDecoder_P_Start_Generic_Body(NEXUS_VideoDecoderHandle videoDecoder, const NEXUS_VideoDecoderStartSettings *pStartSettings, bool otfPvr, BAVC_VideoCompressionStd *pVideoCmprStd, NEXUS_RaveStatus *raveStatus, const NEXUS_StcChannelCallbacks *stcChannelCallbacks, bool *pPlayback, unsigned *stcChannelIndex)
{
    BERR_Code rc;
    NEXUS_RaveSettings raveSettings;
    NEXUS_PidChannelStatus pidChannelStatus;
    BTRC_TRACE(ChnChange_DecodeStartVideo, START);

    /* clear stuff out */
    BKNI_Memset(&videoDecoder->last_field, 0, sizeof(videoDecoder->last_field));
    videoDecoder->last_field.bMute = true;
    BKNI_Memset(&videoDecoder->s3DTVStatus, 0, sizeof(videoDecoder->s3DTVStatus));
    videoDecoder->lastStreamInfo = videoDecoder->streamInfo;
    BKNI_Memset(&videoDecoder->streamInfo, 0, sizeof(videoDecoder->streamInfo));
#if NEXUS_HAS_SYNC_CHANNEL
    BKNI_Memset(&videoDecoder->sync.status, 0, sizeof(videoDecoder->sync.status));
#endif
    videoDecoder->dataReadyCount = 0;
    videoDecoder->numPicturesReceivedToFlush = 0;
    videoDecoder->firstPtsPassed = false;

    NEXUS_PidChannel_GetStatus(pStartSettings->pidChannel, &pidChannelStatus);
    videoDecoder->transportType = pidChannelStatus.originalTransportType; /* saved for later ISR processing */
    videoDecoder->currentUserDataFormat = BUDP_DCCparse_Format_Unknown;
    videoDecoder->useUserDataFormat = false;
    videoDecoder->ptsStcDifference = 0;

    NEXUS_VideoDecoder_P_Trick_Reset_Generic(videoDecoder);

    videoDecoder->pts_error_cnt = 0;
    videoDecoder->fifoEmpty.lastCdbValidPointer = 0;
    videoDecoder->fifoEmpty.count = 0;
    videoDecoder->fifoWatchdog.staticCount = 0;
    videoDecoder->fifoWatchdog.lastCdbValidPointer = 0;
    videoDecoder->fifoWatchdog.lastCdbReadPointer = 0;
    videoDecoder->overflowCount = 0;


    *pVideoCmprStd = NEXUS_P_VideoCodec_ToMagnum(pStartSettings->codec, videoDecoder->transportType);

    rc = NEXUS_VideoDecoder_P_SetTsm(videoDecoder);
    if (rc) {return BERR_TRACE(rc);}

    *stcChannelIndex = videoDecoder->mfdIndex; /* this is the default */

    if (pStartSettings->stcChannel) {
        NEXUS_StcChannelSettings stcChannelSettings;

        LOCK_TRANSPORT();

        NEXUS_StcChannel_SetCallbacks_priv(pStartSettings->stcChannel, NEXUS_StcChannelDecoderType_eVideo0, 0 /* highest priority video */, stcChannelCallbacks);
        if (!videoDecoder->savedRave) {
            NEXUS_StcChannel_EnablePidChannel_priv(pStartSettings->stcChannel, pStartSettings->pidChannel);
            if(pStartSettings->enhancementPidChannel) {
                NEXUS_StcChannel_EnablePidChannel_priv(pStartSettings->stcChannel, pStartSettings->enhancementPidChannel);
            }
        }
        NEXUS_StcChannel_GetIndex_priv(pStartSettings->stcChannel, stcChannelIndex); /* this allows the StcChannels to be swapped */

        UNLOCK_TRANSPORT();

        /* TODO: this is bad, this function also acquires transport lock, but is called from Start and SetExtendedSettings, so... */
        NEXUS_VideoDecoder_P_SetLowLatencySettings(videoDecoder);

        NEXUS_StcChannel_GetSettings(pStartSettings->stcChannel, &stcChannelSettings);
        /* tell the decoder what type of TSM to do based on NEXUS_StcChannelMode, not transport source */
        *pPlayback = (stcChannelSettings.mode != NEXUS_StcChannelMode_ePcr);

        if ( NEXUS_StcChannel_VerifyPidChannel( pStartSettings->stcChannel, pStartSettings->pidChannel ) ) {
            BDBG_WRN(("StcChannel and PidChannel transportType are not compatible"));
        }
    }
    else {
        *pPlayback = pidChannelStatus.playback; /* this enabled band hold on vsync playback */
    }

    if (videoDecoder->savedRave) {
        /* the rave context is already configured */
        LOCK_TRANSPORT();
        rc = NEXUS_Rave_GetStatus_priv(videoDecoder->rave, raveStatus);
        UNLOCK_TRANSPORT();
        if (rc) {return BERR_TRACE(rc);}
    }
    else {
        LOCK_TRANSPORT();
        NEXUS_Rave_GetDefaultSettings_priv(&raveSettings);
        raveSettings.pidChannel = pStartSettings->pidChannel;
        /* 20120725 bandrews - bandHold cannot be enabled if pid channel is from live band */
        raveSettings.bandHold = *pPlayback && pidChannelStatus.playback; /* playback needs band hold, unless it's doing PCR TSM */
        raveSettings.continuityCountEnabled = !*pPlayback;
        raveSettings.disableReordering = (pStartSettings->timestampMode == NEXUS_VideoDecoderTimestampMode_eDecode);
        raveSettings.numOutputBytesEnabled = true; /* rave will poll every 250 msec. this is no worse than SW rave, so we enable always. */
    #if NEXUS_OTFPVR
        if(otfPvr) {
            raveSettings.otfPvr = true;
        }
    #else
        BSTD_UNUSED(otfPvr);
    #endif
        rc = NEXUS_Rave_ConfigureVideo_priv(videoDecoder->rave, pStartSettings->codec, &raveSettings);
        if (rc==BERR_SUCCESS) {
            rc = NEXUS_Rave_GetStatus_priv(videoDecoder->rave, raveStatus);
        } else {
            rc = BERR_TRACE(rc);
        }
        if(rc==BERR_SUCCESS) {
            if(pStartSettings->enhancementPidChannel) {
                BDBG_ASSERT(videoDecoder->enhancementRave); /* should have verified prior */
                raveSettings.pidChannel = pStartSettings->enhancementPidChannel;
                rc = NEXUS_Rave_ConfigureVideo_priv(videoDecoder->enhancementRave, pStartSettings->codec, &raveSettings);
                if (rc!=BERR_SUCCESS) {
                    rc = BERR_TRACE(rc);
                }
            }
        }
        UNLOCK_TRANSPORT();
        if (rc!=BERR_SUCCESS) {return BERR_TRACE(rc);}
    }
    if (rc) {return BERR_TRACE(rc);}

    /* PR42265 20080610 bandrews */
    if (NEXUS_GetEnv("sarnoff_video_delay_workaround") && pStartSettings->codec == NEXUS_VideoCodec_eMpeg2)
    {
        BDBG_MSG(("Applying additional 120 ms to video decoder for MPEG-2 streams"));
        videoDecoder->additionalPtsOffset =
            NEXUS_IS_DSS_MODE(videoDecoder->transportType) ? (120 * 27000) : (120 * 45); /* 120 ms @ 27Mhz (DSS) or 45KHz (MPEG2 TS) */
    }
    else {
        videoDecoder->additionalPtsOffset = 0;
    }

#if NEXUS_HAS_SYNC_CHANNEL
    /* notify sync - this must be called before BXVD_StartDecode so that SyncChannel is ready for the
    ISR-driven callbacks, which may occur before BXVD_StartDecode returns. */
    if (videoDecoder->sync.settings.startCallback_isr) {
        videoDecoder->sync.status.started = true;
        videoDecoder->sync.status.digital = true;
        videoDecoder->sync.status.lastPictureHeld = true; /* TODO: user setting */
        videoDecoder->sync.status.delayValid = false;
        videoDecoder->sync.status.nonRealTime = pStartSettings->nonRealTime;
        videoDecoder->sync.status.customPtsOffset = /* expected in milliseconds */
            (videoDecoder->primerPtsOffset + videoDecoder->additionalPtsOffset)
            / (NEXUS_IS_DSS_MODE(videoDecoder->transportType) ? 27000 : 45);
        BKNI_EnterCriticalSection();
        (*videoDecoder->sync.settings.startCallback_isr)(videoDecoder->sync.settings.callbackContext, 0);
        BKNI_LeaveCriticalSection();
    }

    if (videoDecoder->sync.startMuted)
    {
        /* re-arm mute if sync asks us to */
        videoDecoder->sync.mute = true;
        BDBG_MSG(("Sync requested to start %p muted", videoDecoder));
    }

    NEXUS_VideoDecoder_P_SetMute(videoDecoder);
#endif

    return NEXUS_SUCCESS;
}

NEXUS_Error NEXUS_VideoDecoder_P_Start_Generic_Epilogue(NEXUS_VideoDecoderHandle videoDecoder, const NEXUS_VideoDecoderStartSettings *pStartSettings)
{
    NEXUS_PidChannel_ConsumerStarted(pStartSettings->pidChannel); /* needed to unpause playback & stuff like that */
    if(pStartSettings->enhancementPidChannel) {
        NEXUS_PidChannel_ConsumerStarted(pStartSettings->enhancementPidChannel); /* needed to unpause playback & stuff like that */
    }

    NEXUS_VideoDecoder_FlushUserData(videoDecoder);

    LOCK_TRANSPORT();
    NEXUS_Rave_Enable_priv(videoDecoder->rave);
    if(pStartSettings->enhancementPidChannel) {
        BDBG_ASSERT(videoDecoder->enhancementRave);
        NEXUS_Rave_Enable_priv(videoDecoder->enhancementRave);
    }
    UNLOCK_TRANSPORT();

#if NEXUS_HAS_ASTM
    videoDecoder->astm.status.started = true;

    if (videoDecoder->astm.settings.lifecycle_isr)
    {
        BDBG_MSG(("Video channel %p is notifying ASTM of its start action", videoDecoder));
        BKNI_EnterCriticalSection();
        (*videoDecoder->astm.settings.lifecycle_isr)(videoDecoder->astm.settings.callbackContext, 0);
        BKNI_LeaveCriticalSection();
    }
#endif
    if(pStartSettings->stcChannel) {
        NEXUS_OBJECT_ACQUIRE(videoDecoder, NEXUS_StcChannel, pStartSettings->stcChannel);
    }

    BTRC_TRACE(ChnChange_DecodeFirstVideo, START);
    videoDecoder->started = true;
    videoDecoder->maxFrameRepeat.pictureDisplayCount = 0;
    BTRC_TRACE(ChnChange_DecodeStartVideo, STOP);

    /* need to see if preroll -> unmute */
    NEXUS_VideoDecoder_P_SetMute(videoDecoder);

    return NEXUS_SUCCESS;
}

#if !NEXUS_OTFPVR
#define NEXUS_VideoDecoder_P_OtfPvr_Activate(videoDecoder, raveStatus, cfg) (NEXUS_SUCCESS)
#endif

NEXUS_Error NEXUS_VideoDecoder_P_Start_priv(NEXUS_VideoDecoderHandle videoDecoder, const NEXUS_VideoDecoderStartSettings *pStartSettings, bool otfPvr)
{
    NEXUS_Error rc;
    BXVD_DecodeSettings cfg;
    NEXUS_StcChannelCallbacks callbacks;
    NEXUS_RaveStatus raveStatus;
    NEXUS_RaveStatus enhancementRaveStatus;
    BXVD_DisplayThresholds thresholds;
    unsigned stcChannelIndex;

    BXVD_GetDecodeDefaultSettings(videoDecoder->dec, &cfg);
    LOCK_TRANSPORT();
    NEXUS_StcChannel_GetDefaultCallbacks_priv(&callbacks);
    callbacks.getPts_isr = NEXUS_VideoDecoder_P_GetPtsCallback_isr;
    callbacks.getCdbLevel_isr = NEXUS_VideoDecoder_P_GetCdbLevelCallback_isr;
    callbacks.stcValid_isr = NEXUS_VideoDecoder_P_StcValidCallback_isr;
    if (videoDecoder->mosaicMode || videoDecoder->startSettings.nonRealTime) {
        callbacks.setPcrOffset_isr = NEXUS_VideoDecoder_P_SetPcrOffset_isr;
        callbacks.getPcrOffset_isr = NEXUS_VideoDecoder_P_GetPcrOffset_isr;
        /* NRT mode disables XDM Jitter Tolerance Improvement (JTI) since the NRT TSM may have very small dStcPts jitter */
        if(videoDecoder->startSettings.nonRealTime) {
            rc = BXVD_SetJitterToleranceImprovementEnable(videoDecoder->dec, false);
            if (rc!=BERR_SUCCESS) {rc = BERR_TRACE(rc);}
        }
    }
    callbacks.pDevContext = videoDecoder;
    UNLOCK_TRANSPORT();

    rc = NEXUS_VideoDecoder_P_Start_Generic_Body(videoDecoder, pStartSettings, otfPvr, &cfg.eVideoCmprStd, &raveStatus, &callbacks, &cfg.bPlayback, &stcChannelIndex);
    if(rc!=NEXUS_SUCCESS) {rc = BERR_TRACE(rc); goto err_start_generic_body;}

    /* this needs to happen some time before BXVD_Start */
    videoDecoder->validOutputPic = false;

    /* enable interrupt for 3DTV AVC SEI messages */
    rc = BXVD_EnableInterrupt(videoDecoder->dec, BXVD_Interrupt_ePictureExtensionData, true);
    if (rc) {rc=BERR_TRACE(rc);}

    /* reset internal override of 3D orientation, if and only if app did not specify an orientation */
    if (videoDecoder->settings.customSourceOrientation==false) {
        BXVD_3DSetting xvd3dSetting;
        BXVD_Get3D(videoDecoder->dec, &xvd3dSetting);
        xvd3dSetting.bOverrideOrientation = false;
        xvd3dSetting.eOrientation = BXDM_PictureProvider_Orientation_e2D;
        BXVD_Set3D(videoDecoder->dec, &xvd3dSetting);
    }

    /* PR49215 this needs to be reset on restart */
    rc = BXVD_SetHwPcrOffsetEnable(videoDecoder->dec, !cfg.bPlayback);

    videoDecoder->fifoWatchdog.lastPtsValid = false;
    videoDecoder->fifoWatchdog.timer = NEXUS_ScheduleTimer(NEXUS_VIDEO_DECODER_P_FIFO_WATCHDOG_TIMEOUT, NEXUS_VideoDecoder_P_FifoWatchdog, videoDecoder);

    rc = NEXUS_VideoDecoder_P_SetPtsOffset(videoDecoder);
    if (rc) {rc = BERR_TRACE(rc);goto err_setptsoffset;}

    cfg.eSTC = BXVD_STC_eZero + stcChannelIndex;
    if(cfg.eSTC >= BXVD_STC_eMax) {
        rc = BERR_TRACE(NEXUS_INVALID_PARAMETER);
        goto err_invalid_stc;
    }

    /* map the XDM with decoder */
    cfg.eDisplayInterrupt = videoDecoder->xdmIndex + BXVD_DisplayInterrupt_eZero;

#if NEXUS_HAS_ASTM
    BDBG_MSG(("%s ASTM mode for video channel %p", videoDecoder->astm.settings.enableAstm ? "Enabling" : "Disabling", videoDecoder));
    cfg.bAstmMode = videoDecoder->astm.settings.enableAstm;
#endif

    NEXUS_VideoDecoder_P_ApplyDisplayInformation(videoDecoder);

    BDBG_CASSERT(BXVD_ErrorHandling_ePrognostic == (BXVD_Picture_ErrorHandling)NEXUS_VideoDecoderErrorHandling_ePrognostic);
    cfg.eErrorHandling = (BXVD_Picture_ErrorHandling)(pStartSettings->errorHandling);
    cfg.uiPreRollRate = pStartSettings->prerollRate;
    cfg.pContextMap = &raveStatus.xptContextMap;
#if NEXUS_CRC_CAPTURE
    cfg.bCrcMode = true;
#endif
    cfg.bIgnoreDPBOutputDelaySyntax = videoDecoder->extendedSettings.ignoreDpbOutputDelaySyntax;
    cfg.bZeroDelayOutputMode = videoDecoder->extendedSettings.zeroDelayOutputMode;
    cfg.bEarlyPictureDeliveryMode = videoDecoder->extendedSettings.earlyPictureDeliveryMode;
    cfg.bSVC3DModeEnable = videoDecoder->extendedSettings.svc3dEnabled;
    cfg.bIFrameAsRAP = videoDecoder->extendedSettings.treatIFrameAsRap;
    cfg.bIgnoreNumReorderFramesEqZero = videoDecoder->extendedSettings.ignoreNumReorderFramesEqZero;

    if (videoDecoder->extendedSettings.svc3dEnabled && !videoDecoder->openSettings.openSettings.svc3dSupported) {
        BDBG_ERR(("You must set NEXUS_VideoDecoderOpenSettings.svc3dSupported before setting NEXUS_VideoDecoderExtendedSettings.svc3dEnabled"));
        cfg.bSVC3DModeEnable = false;
    }

    /* XVD frame rate handling is managed in the following priority scheme:
       1. use the frame rate coded in the stream
       2. If that's unknown, use eDefaultFrameRate
       3. If that's unspecified, use FRD logic to determine frame rate
       4. If frame rate cannot be determined, report "unknown" */
    cfg.eFrameRateDetectionMode = BXVD_FrameRateDetectionMode_eStable;
    rc = NEXUS_P_FrameRate_ToMagnum(pStartSettings->frameRate, &cfg.eDefaultFrameRate);
    if(rc!=NEXUS_SUCCESS) {rc = BERR_TRACE(rc); goto err_framerate_tomagnum;}

    if(otfPvr) {
        rc = NEXUS_VideoDecoder_P_OtfPvr_Activate(videoDecoder, &raveStatus, &cfg);
        if(rc!=NEXUS_SUCCESS) {rc = BERR_TRACE(rc); goto err_otfpvr_activate;}
    }

    BDBG_CWARNING(NEXUS_VideoDecoderProgressiveOverrideMode_eMax == (NEXUS_VideoDecoderProgressiveOverrideMode)BXVD_ProgressiveOverrideMode_eMax);
    cfg.eProgressiveOverrideMode = pStartSettings->progressiveOverrideMode;

    BDBG_CWARNING(NEXUS_VideoDecoderTimestampMode_eMax == (NEXUS_VideoDecoderTimestampMode)BXVD_TimestampMode_eMaxModes);
    /* The enums in XVD and FW are swapped, e.g.
       NEXUS_NEXUS_VideoDecoderTimestampMode_eDecode maps to BXVD_TimestampMode_eDisplay.
       The Nexus enums are correct. As long as we're assigning a number to a number, we are ok. */
    cfg.eTimestampMode = pStartSettings->timestampMode;
    if(pStartSettings->enhancementPidChannel) {
        BDBG_ASSERT(videoDecoder->enhancementRave);
        LOCK_TRANSPORT();
        rc = NEXUS_Rave_GetStatus_priv(videoDecoder->enhancementRave, &enhancementRaveStatus);
        UNLOCK_TRANSPORT();
        if(rc!=BERR_SUCCESS) {rc = BERR_TRACE(rc);goto err_rave_status;}
        cfg.aContextMapExtended[0] = &enhancementRaveStatus.xptContextMap;
        cfg.uiContextMapExtNum = 1;
    }
    cfg.bUserDataBTPModeEnable = true; 

    BDBG_MODULE_MSG(nexus_flow_video_decoder, ("start %p, pidChannel %p, stcChannel %p, codec %d", videoDecoder, pStartSettings->pidChannel, pStartSettings->stcChannel, pStartSettings->codec));
    BTRC_TRACE(ChnChange_DecodeStartVideoXVD, START);
    rc = BXVD_StartDecode(videoDecoder->dec, &cfg);
    BTRC_TRACE(ChnChange_DecodeStartVideoXVD, STOP);
    if (rc) {rc = BERR_TRACE(rc); goto err_startdecode;}

    /* XVD gets a default discard threshold per codec. Capture is here so we can restore it after a trick mode. */
    rc = BXVD_GetDisplayThresholds(videoDecoder->dec, &thresholds);
    if (rc) {rc = BERR_TRACE(rc); goto err_getdisplaythresholds;}
    videoDecoder->defaultDiscardThreshold = thresholds.ui32DiscardThreshold;
    videoDecoder->defaultVeryLateThreshold = thresholds.ui32VeryLateThreshold;

    rc = NEXUS_VideoDecoder_P_Start_Generic_Epilogue(videoDecoder, pStartSettings);
    if(rc!=NEXUS_SUCCESS) {return BERR_TRACE(rc);}

    NEXUS_VideoDecoder_P_SetUserdata(videoDecoder);

    return rc;

err_getdisplaythresholds:
    BXVD_StopDecode(videoDecoder->dec);
err_startdecode:
err_rave_status:
err_framerate_tomagnum:
err_otfpvr_activate:
err_invalid_stc:
err_setptsoffset:
    NEXUS_CancelTimer(videoDecoder->fifoWatchdog.timer);
    videoDecoder->fifoWatchdog.timer = NULL;
err_start_generic_body:
    return rc;
}

NEXUS_Error NEXUS_VideoDecoder_P_Stop_Generic_Prologue(NEXUS_VideoDecoderHandle videoDecoder)
{
    if (!videoDecoder->started) {
        BDBG_ERR(("video decoder not started"));
        return BERR_TRACE(NEXUS_UNKNOWN);
    }
    return NEXUS_SUCCESS;
}

void NEXUS_VideoDecoder_P_Stop_Generic_Epilogue(NEXUS_VideoDecoderHandle videoDecoder)
{
    videoDecoder->started = false;
    if(videoDecoder->startSettings.stcChannel) {
        NEXUS_OBJECT_RELEASE(videoDecoder, NEXUS_StcChannel, videoDecoder->startSettings.stcChannel);
    }
    BKNI_Memset(&videoDecoder->startSettings, 0, sizeof(videoDecoder->startSettings));
    return;
}

void NEXUS_VideoDecoder_P_Stop_Avd(NEXUS_VideoDecoderHandle videoDecoder)
{
    NEXUS_Error rc;
    BDBG_OBJECT_ASSERT(videoDecoder, NEXUS_VideoDecoder);

    rc = NEXUS_VideoDecoder_P_Stop_Generic_Prologue(videoDecoder);
    if(rc!=NEXUS_SUCCESS) {return;}

    if (!videoDecoder->dec) {
        BDBG_ERR(("NEXUS_VideoDecoder_Stop must be called before NEXUS_VideoWindow_RemoveInput. Please reverse the order of calls in your application."));
        return;
    }
    if(videoDecoder->otfPvr.wasActive) {
        videoDecoder->otfPvr.wasActive = false;
        NEXUS_VideoDecoder_P_SetChannelChangeMode(videoDecoder, videoDecoder->settings.channelChangeMode); /* reapply the channel change mode */
    }
    NEXUS_VideoDecoder_P_Stop_priv(videoDecoder);
#if NEXUS_OTFPVR
    if(videoDecoder->otfPvr.active)  {
        videoDecoder->otfPvr.active = false;
        NEXUS_VideoDecoder_P_OtfPvr_Stop(videoDecoder);
    }
#endif

    /* The primer interface may have replaced the main RAVE context with a primed context. this reverses that. */
    if (videoDecoder->savedRave) {
        videoDecoder->rave = videoDecoder->savedRave;
        videoDecoder->savedRave = NULL;
    }
    NEXUS_VideoDecoder_P_Stop_Generic_Epilogue(videoDecoder);
    return;
}


void NEXUS_VideoDecoder_P_Stop_Priv_Generic_Prologue(NEXUS_VideoDecoderHandle videoDecoder)
{
    BTRC_TRACE(ChnChange_Total_Video, START);
    BTRC_TRACE(ChnChange_DecodeStopVideo, START);

    if (videoDecoder->fifoWatchdog.timer) {
        NEXUS_CancelTimer(videoDecoder->fifoWatchdog.timer);
        videoDecoder->fifoWatchdog.timer = NULL;
    }
    if (videoDecoder->s3DTVStatusTimer) {
        NEXUS_CancelTimer(videoDecoder->s3DTVStatusTimer);
        videoDecoder->s3DTVStatusTimer = NULL;
    }

    if ( videoDecoder->startSettings.stcChannel )
    {
        NEXUS_StcChannelNonRealtimeSettings stcNonRealtimeSettings;
        NEXUS_Error rc;

        LOCK_TRANSPORT();
        /* set STC to freerunning mode on stop */
        NEXUS_StcChannel_GetDefaultNonRealtimeSettings_priv( &stcNonRealtimeSettings);
        stcNonRealtimeSettings.triggerMode = NEXUS_StcChannelTriggerMode_eTimebase;
        rc = NEXUS_StcChannel_SetNonRealtimeConfig_priv(videoDecoder->startSettings.stcChannel, &stcNonRealtimeSettings);
        if (rc) rc=BERR_TRACE(rc); /* keep going */

        NEXUS_StcChannel_DisablePidChannel_priv(videoDecoder->startSettings.stcChannel, videoDecoder->startSettings.pidChannel);
        if(videoDecoder->startSettings.enhancementPidChannel) {
            NEXUS_StcChannel_DisablePidChannel_priv(videoDecoder->startSettings.stcChannel, videoDecoder->startSettings.enhancementPidChannel);
        }
        NEXUS_StcChannel_ClearCallbacks_priv(videoDecoder->startSettings.stcChannel, NEXUS_StcChannelDecoderType_eVideo0);
        UNLOCK_TRANSPORT();
    }
    return;
}

void NEXUS_VideoDecoder_P_Stop_Priv_Generic_Epilogue(NEXUS_VideoDecoderHandle videoDecoder)
{
    LOCK_TRANSPORT();
    NEXUS_Rave_Disable_priv(videoDecoder->rave);
    NEXUS_Rave_Flush_priv(videoDecoder->rave);
    NEXUS_Rave_RemovePidChannel_priv(videoDecoder->rave);
    if(videoDecoder->startSettings.enhancementPidChannel) {
        NEXUS_Rave_Disable_priv(videoDecoder->enhancementRave);
        NEXUS_Rave_Flush_priv(videoDecoder->enhancementRave);
        NEXUS_Rave_RemovePidChannel_priv(videoDecoder->enhancementRave);
    }
    UNLOCK_TRANSPORT();

#if NEXUS_HAS_ASTM
    videoDecoder->astm.status.started = false;

    if (videoDecoder->astm.settings.lifecycle_isr)
    {
        BDBG_MSG(("Video channel %p is notifying ASTM of its stop action", videoDecoder));
        BKNI_EnterCriticalSection();
        (*videoDecoder->astm.settings.lifecycle_isr)(videoDecoder->astm.settings.callbackContext, 0);
        BKNI_LeaveCriticalSection();
    }
#endif

    videoDecoder->primerPtsOffset = 0;
    
#if NEXUS_HAS_SYNC_CHANNEL
    /* notify sync */
    if (videoDecoder->sync.settings.startCallback_isr) 
    {
        videoDecoder->sync.status.started = false;
        BKNI_EnterCriticalSection();
        (*videoDecoder->sync.settings.startCallback_isr)(videoDecoder->sync.settings.callbackContext, 0);
        BKNI_LeaveCriticalSection();
    }
#endif
    BTRC_TRACE(ChnChange_DecodeStopVideo, STOP);
}

void NEXUS_VideoDecoder_P_Stop_priv(NEXUS_VideoDecoderHandle videoDecoder)
{
    BERR_Code rc;

    NEXUS_VideoDecoder_P_Stop_Priv_Generic_Prologue(videoDecoder);

    NEXUS_VideoDecoder_P_SetUserdata(videoDecoder);

    BDBG_MODULE_MSG(nexus_flow_video_decoder, ("stop %p", videoDecoder));
    rc = BXVD_StopDecode(videoDecoder->dec);
    if (rc) {rc=BERR_TRACE(rc); /* keep going */}
    NEXUS_VideoDecoder_P_Stop_Priv_Generic_Epilogue(videoDecoder);

    return;
}

void NEXUS_VideoDecoder_P_GetStatus_Generic(NEXUS_VideoDecoderHandle videoDecoder, NEXUS_VideoDecoderStatus *pStatus)
{
    BDBG_OBJECT_ASSERT(videoDecoder, NEXUS_VideoDecoder);
    BKNI_Memset(pStatus, 0, sizeof(*pStatus));

    if (videoDecoder->rave) {
        unsigned depth, size;
        NEXUS_RaveStatus raveStatus;
        int rc;

        BKNI_EnterCriticalSection();
        NEXUS_Rave_GetCdbBufferInfo_isr(videoDecoder->rave, &depth, &size);
        BKNI_LeaveCriticalSection();
        pStatus->fifoDepth = depth;
        pStatus->fifoSize = size;
#if 0
        if(size != videoDecoder->cdbLength) { /* number returned by Rave is off by 1 */
            BDBG_WRN(("NEXUS_VideoDecoder_GetStatus:%#x mismatch in CDB size %u:%u", (unsigned)videoDecoder, (unsigned)size, (unsigned)videoDecoder->cdbLength));
        }
#endif
        if(videoDecoder->startSettings.enhancementPidChannel) {
            BKNI_EnterCriticalSection();
            NEXUS_Rave_GetCdbBufferInfo_isr(videoDecoder->enhancementRave, &depth, &size);
            BKNI_LeaveCriticalSection();
            pStatus->enhancementFifoDepth = depth;
            pStatus->enhancementFifoSize = size;
        }

        LOCK_TRANSPORT();
        rc = NEXUS_Rave_GetStatus_priv(videoDecoder->rave, &raveStatus);
        UNLOCK_TRANSPORT();
        if (!rc) {
            pStatus->numBytesDecoded = raveStatus.numOutputBytes;
        }
    }

    pStatus->started = videoDecoder->started;
    pStatus->muted = videoDecoder->last_field.bMute;
    pStatus->source.width = videoDecoder->last_field.ulSourceHorizontalSize;
    pStatus->source.height = videoDecoder->last_field.ulSourceVerticalSize;
    if (videoDecoder->firstPtsPassed ||
        !videoDecoder->tsm ||
        (videoDecoder->settings.channelChangeMode != NEXUS_VideoDecoder_ChannelChangeMode_eHoldUntilFirstPicture &&
         videoDecoder->settings.channelChangeMode != NEXUS_VideoDecoder_ChannelChangeMode_eMuteUntilFirstPicture))
    {
        /* streamInfo now matches the last_field, so we can mix them. */
        pStatus->coded.width = videoDecoder->streamInfo.codedSourceHorizontalSize;
        pStatus->coded.height = videoDecoder->streamInfo.codedSourceVerticalSize;
    }
    else {
        /* last_field is still reporting the old picture, so we can't mix with current streamInfo. */
        pStatus->coded.width = videoDecoder->lastStreamInfo.codedSourceHorizontalSize;
        pStatus->coded.height = videoDecoder->lastStreamInfo.codedSourceVerticalSize;
    }
    pStatus->display.width = videoDecoder->last_field.ulDisplayHorizontalSize;
    pStatus->display.height = videoDecoder->last_field.ulDisplayVerticalSize;

    pStatus->interlaced = !videoDecoder->last_field.bStreamProgressive;
    pStatus->aspectRatio = NEXUS_P_AspectRatio_FromMagnum(videoDecoder->last_field.eAspectRatio);
    BDBG_CWARNING(BAVC_FrameRateCode_eMax == (BAVC_FrameRateCode)NEXUS_VideoFrameRate_eMax);
    pStatus->frameRate = videoDecoder->last_field.eFrameRateCode;
    pStatus->ptsStcDifference = videoDecoder->ptsStcDifference;
    pStatus->format = NEXUS_P_VideoFormat_FromInfo(pStatus->source.height, pStatus->frameRate, pStatus->interlaced);

    pStatus->ptsErrorCount = videoDecoder->pts_error_cnt;
    pStatus->tsm = videoDecoder->tsm;
    pStatus->firstPtsPassed = videoDecoder->firstPtsPassed;

    switch (videoDecoder->pictureParameterInfo.pictureCoding) {
    case BXVD_PictureCoding_eI: pStatus->pictureCoding = NEXUS_PictureCoding_eI; break;
    case BXVD_PictureCoding_eP: pStatus->pictureCoding = NEXUS_PictureCoding_eP; break;
    case BXVD_PictureCoding_eB: pStatus->pictureCoding = NEXUS_PictureCoding_eB; break;
    default: pStatus->pictureCoding = NEXUS_PictureCoding_eUnknown; break;
    }

    return;
}

NEXUS_Error NEXUS_VideoDecoder_P_GetStatus_Avd(NEXUS_VideoDecoderHandle videoDecoder, NEXUS_VideoDecoderStatus *pStatus)
{
    BXVD_PTSInfo ptsInfo;
    BXVD_GopTimeCode gopTimeCode;
    BXVD_ChannelStatus channelStatus;
    BERR_Code rc = BERR_SUCCESS;

    BDBG_OBJECT_ASSERT(videoDecoder, NEXUS_VideoDecoder);

    NEXUS_VideoDecoder_P_GetStatus_Generic(videoDecoder, pStatus);

    if (!videoDecoder->dec) return NEXUS_SUCCESS;

    BKNI_EnterCriticalSection();
    rc |= BXVD_GetPTS_isr(videoDecoder->dec, &ptsInfo);
    BKNI_LeaveCriticalSection();
    rc |= BXVD_GetChannelStatus(videoDecoder->dec, &channelStatus);
    rc |= BXVD_GetGopTimeCode(videoDecoder->dec, &gopTimeCode);
    rc |= BXVD_GetPictureTag(videoDecoder->dec, &pStatus->pictureTag);
    /* if a status function failed, we'll fail in the end. but return what we can. */

    pStatus->pts = ptsInfo.ui32RunningPTS;
    pStatus->ptsType = (NEXUS_PtsType)ptsInfo.ePTSType;

    pStatus->queueDepth = channelStatus.ulPictureDeliveryCount;
    pStatus->cabacBinDepth = channelStatus.uiCabacBinDepth;

    pStatus->timeCode.hours = gopTimeCode.ulTimeCodeHours;
    pStatus->timeCode.minutes = gopTimeCode.ulTimeCodeMinutes;
    pStatus->timeCode.seconds = gopTimeCode.ulTimeCodeSeconds;
    pStatus->timeCode.pictures = gopTimeCode.ulTimeCodePictures;

    pStatus->numDecoded = channelStatus.uiPicturesDecodedCount;
    pStatus->numDisplayed = channelStatus.uiDisplayedCount;
    pStatus->numDecodeErrors = channelStatus.uiDecodeErrorCount;
    pStatus->numDecodeOverflows = channelStatus.uiDecoderInputOverflow;
    pStatus->numDisplayErrors = channelStatus.uiDisplayedParityFailureCount;
    pStatus->numDecodeDrops = channelStatus.uiDecoderDroppedCount;
    pStatus->avdStatusBlock = channelStatus.uiAVDStatusBlock;
    pStatus->numPicturesReceived = channelStatus.uiPicturesReceivedCount + videoDecoder->numPicturesReceivedToFlush;
    pStatus->numDisplayDrops = channelStatus.uiDisplayManagerDroppedCount;
    pStatus->numDisplayUnderflows = channelStatus.uiVsyncUnderflowCount;
    pStatus->numWatchdogs = videoDecoder->device->numWatchdogs;

    BDBG_CASSERT(BXVD_Video_Protocol_eLevel_MaxLevel == (BXVD_Video_Protocol_Level)NEXUS_VideoProtocolLevel_eMax);
    pStatus->protocolLevel = channelStatus.eProtocolLevel;
    BDBG_CASSERT(BXVD_Video_Protocol_eProfile_MaxProfile == (BXVD_Video_Protocol_Profile)NEXUS_VideoProtocolProfile_eMax);
    pStatus->protocolProfile = channelStatus.eProtocolProfile;

    {
        uint32_t mostRecentPts;
        int rc;
        LOCK_TRANSPORT();
        rc = NEXUS_Rave_GetMostRecentPts_priv(videoDecoder->rave, &mostRecentPts);
        UNLOCK_TRANSPORT();
        /* rc!=0 will occur if no PTS found.
        PTS 0 is valid and also means "no PTS". so, if queueDepth is non-zero, we regard PTS 0 as valid, otherwise as "no PTS". */
        if (!rc && (pStatus->pts || pStatus->queueDepth)) {
            /* check for backwards PTS discontinuity */
            if (mostRecentPts > pStatus->pts) {
                pStatus->bufferLatency = (mostRecentPts - pStatus->pts) / (NEXUS_IS_DSS_MODE(videoDecoder->transportType) ? 27000 : 45);
                BDBG_MSG_TRACE(("pts calc: %#x %#x --> %d", mostRecentPts, pStatus->pts, pStatus->bufferLatency));
            }
            else {
                /* we can't know (even if we scanned entire ITB) until discont passes through. */
                pStatus->bufferLatency = 0;
            }
        }
        else {
            /* no PTS found, so we can measure time the display queue, assuming constant source frame rate */
            unsigned refreshRate = NEXUS_P_RefreshRate_FromFrameRate(videoDecoder->streamInfo.frameRate);
            pStatus->bufferLatency = pStatus->queueDepth * 1000000 / (refreshRate?refreshRate:30000);
            BDBG_MSG_TRACE(("queue calc: %d pic, %d refresh --> %d", pStatus->queueDepth, refreshRate, pStatus->bufferLatency));
        }
    }

#if NEXUS_OTFPVR
    if(videoDecoder->otfPvr.active)  {
        NEXUS_VideoDecoder_P_OtfPvr_UpdateStatus(videoDecoder, pStatus);
    }
#endif
    return BERR_TRACE(rc);
}

void NEXUS_VideoDecoder_P_Flush_Avd(NEXUS_VideoDecoderHandle videoDecoder)
{
    BERR_Code rc;
    BXVD_ChannelStatus channelStatus;

    BDBG_OBJECT_ASSERT(videoDecoder, NEXUS_VideoDecoder);
    if (!videoDecoder->started) return;
    BDBG_ASSERT(videoDecoder->dec);

    BDBG_MSG(("flush"));
    rc = BXVD_DisableForFlush(videoDecoder->dec);
    if (rc) {rc=BERR_TRACE(rc);} /* fall through */

    /* BXVD_FlushDecode will clear the uiPicturesReceivedCount. Nexus needs to accumulate this
    total since Start. */
    rc = BXVD_GetChannelStatus(videoDecoder->dec, &channelStatus);
    if (!rc) {
        videoDecoder->numPicturesReceivedToFlush += channelStatus.uiPicturesReceivedCount;
    }

#if NEXUS_OTFPVR
    if(videoDecoder->otfPvr.active)  {
        NEXUS_VideoDecoder_P_OtfPvr_DisableForFlush(videoDecoder);
    }
#endif

    LOCK_TRANSPORT();
    NEXUS_Rave_Disable_priv(videoDecoder->rave);
    NEXUS_Rave_Flush_priv(videoDecoder->rave);
    if(videoDecoder->startSettings.enhancementPidChannel) {
        NEXUS_Rave_Disable_priv(videoDecoder->enhancementRave);
        NEXUS_Rave_Flush_priv(videoDecoder->enhancementRave);
    }
    UNLOCK_TRANSPORT();

    rc |= BXVD_FlushDecode(videoDecoder->dec);
    if (rc) {rc=BERR_TRACE(rc);} /* fall through */
    LOCK_TRANSPORT();
    NEXUS_Rave_Enable_priv(videoDecoder->rave);
    if (videoDecoder->startSettings.enhancementPidChannel) {
        NEXUS_Rave_Enable_priv(videoDecoder->enhancementRave);
    }
    UNLOCK_TRANSPORT();

#if NEXUS_OTFPVR
    if(videoDecoder->otfPvr.active)  {
        NEXUS_VideoDecoder_P_OtfPvr_Flush(videoDecoder);
    }
#endif

    NEXUS_VideoDecoder_FlushUserData(videoDecoder);
    videoDecoder->maxFrameRepeat.pictureDisplayCount = 0;
}

NEXUS_VideoInput NEXUS_VideoDecoder_P_GetConnector_Avd( NEXUS_VideoDecoderHandle videoDecoder)
{
    BDBG_OBJECT_ASSERT(videoDecoder, NEXUS_VideoDecoder);
    return &videoDecoder->input;
}

NEXUS_Error NEXUS_VideoDecoder_P_GetStreamInformation_Avd(NEXUS_VideoDecoderHandle videoDecoder, NEXUS_VideoDecoderStreamInformation *pStreamInformation)
{
    BDBG_OBJECT_ASSERT(videoDecoder, NEXUS_VideoDecoder);
    *pStreamInformation = videoDecoder->streamInfo;
    return 0;
}

void NEXUS_VideoDecoder_P_Reset_Avd( NEXUS_VideoDecoderHandle videoDecoder )
{
    BDBG_OBJECT_ASSERT(videoDecoder, NEXUS_VideoDecoder);
    NEXUS_VideoDecoder_P_WatchdogHandler(videoDecoder->device);
}

void NEXUS_VideoDecoder_P_GetOpenSettings_Avd( NEXUS_VideoDecoderHandle videoDecoder, NEXUS_VideoDecoderOpenSettings *pOpenSettings )
{
    BDBG_OBJECT_ASSERT(videoDecoder, NEXUS_VideoDecoder);
    *pOpenSettings = videoDecoder->openSettings.openSettings;
}

NEXUS_Error NEXUS_VideoDecoder_P_SetStartPts_Avd( NEXUS_VideoDecoderHandle videoDecoder, uint32_t pts )
{
    BDBG_OBJECT_ASSERT(videoDecoder, NEXUS_VideoDecoder);
    if (videoDecoder->dec && videoDecoder->startSettings.stcChannel) {
        BERR_Code rc;

        BDBG_WRN(("NEXUS_VideoDecoder_SetStartPts %#x", pts));
        rc =BXVD_SetClipTime(videoDecoder->dec, BXVD_ClipTimeType_eClipStartOnly, pts, 0xFFFFFFFF);
        if (rc) return BERR_TRACE(rc);
        rc = BXVD_SetTSMWaitForValidSTC(videoDecoder->dec);
        if (rc) return BERR_TRACE(rc);
    }
    return 0;
}

void NEXUS_VideoDecoder_P_GetPlaybackSettings_Common( NEXUS_VideoDecoderHandle videoDecoder, NEXUS_VideoDecoderPlaybackSettings *pSettings )
{
    BDBG_OBJECT_ASSERT(videoDecoder, NEXUS_VideoDecoder);
    *pSettings = videoDecoder->playback.videoDecoderPlaybackSettings;
}

NEXUS_Error NEXUS_VideoDecoder_P_SetPlaybackSettings_Common( NEXUS_VideoDecoderHandle videoDecoder, const NEXUS_VideoDecoderPlaybackSettings *pSettings )
{
    BDBG_OBJECT_ASSERT(videoDecoder, NEXUS_VideoDecoder);
    NEXUS_IsrCallback_Set(videoDecoder->playback.firstPtsCallback, &pSettings->firstPts);
    NEXUS_IsrCallback_Set(videoDecoder->playback.firstPtsPassedCallback, &pSettings->firstPtsPassed);
    videoDecoder->playback.videoDecoderPlaybackSettings = *pSettings;
    return 0;
}

void NEXUS_VideoDecoder_P_IsCodecSupported_Avd( NEXUS_VideoDecoderHandle videoDecoder, NEXUS_VideoCodec codec, bool *pSupported )
{
    unsigned i;
    BDBG_OBJECT_ASSERT(videoDecoder, NEXUS_VideoDecoder);

    if (codec >= NEXUS_VideoCodec_eMax) {
        *pSupported = false;
        return;
    }

    /* massage the result slightly */
    switch (codec) {
    case NEXUS_VideoCodec_eH263:
    case NEXUS_VideoCodec_eH264: /* AVC, Mpeg4Part10 */
    case NEXUS_VideoCodec_eMpeg4Part2: /* DivX 4/5/6 */
    case NEXUS_VideoCodec_eDivx311:
        /* AVC and DivX are supported together */
        codec = NEXUS_VideoCodec_eH264;
        break;
    case NEXUS_VideoCodec_eH264_Mvc:
    case NEXUS_VideoCodec_eH264_Svc:
        /* SVC and MVC are supported together */
        codec = NEXUS_VideoCodec_eH264_Svc;
        break;
    case NEXUS_VideoCodec_eVc1:
    case NEXUS_VideoCodec_eVc1SimpleMain:
        /* Both flavors of VC1 are supported together. */
        codec = NEXUS_VideoCodec_eVc1;
        break;
    default:
        break;
    }

    /* it's possible that it's more complicated than this, but this is a start. we can extend as needed. */
    for (i=0;i<NEXUS_VideoCodec_eMax;i++) {
        if (videoDecoder->settings.supportedCodecs[i] == codec) {
            *pSupported = true;
            return;
        }
    }

    *pSupported = false;
}

#include "bxvd_dbg.h"
NEXUS_Error NEXUS_VideoDecoderModule_SetDebugLog(unsigned avdCore, bool enabled)
{
    NEXUS_Error rc;

    if (avdCore >= NEXUS_NUM_XVD_DEVICES) {
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }

    /* If not connected, then it's not powered up yet, so we can't start the debug log. Don't use BERR_TRACE because this is normal. */
    if (!g_NEXUS_videoDecoderXvdDevices[avdCore].channel[0].dec) {
        return NEXUS_UNKNOWN;
    }

    if (!NEXUS_GetEnv("avd_monitor")) {
        /* TODO: consider a NEXUS_VideoDecoderModuleSettings allocation if this feature needs general use */
        BDBG_ERR(("You must define avd_monitor to enable allocations for this feature."));
        return BERR_TRACE(NEXUS_NOT_SUPPORTED);
    }

    rc = BXVD_DBG_ControlDecoderDebugLog(g_NEXUS_videoDecoderXvdDevices[avdCore].xvd, enabled?BXVD_DBG_DebugLogging_eStart:BXVD_DBG_DebugLogging_eStop);
    return BERR_TRACE(rc);
}

NEXUS_Error NEXUS_VideoDecoderModule_ReadDebugLog(unsigned avdCore, void *buffer, unsigned bufferSize, unsigned *pAmountRead)
{
    NEXUS_Error rc;

    if (avdCore >= NEXUS_NUM_XVD_DEVICES || buffer==NULL || pAmountRead==NULL) {
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }

    *pAmountRead = 0;

    /* If not connected, then it's not powered up yet, so we can't start the debug log. Don't use BERR_TRACE because this is normal. */
    if (!g_NEXUS_videoDecoderXvdDevices[avdCore].channel[0].dec) {
        return 0;
    }
    rc = BXVD_DBG_ReadDecoderDebugLog(g_NEXUS_videoDecoderXvdDevices[avdCore].xvd, buffer, bufferSize, pAmountRead);
    return BERR_TRACE(rc);
}

NEXUS_Error NEXUS_VideoDecoder_P_SetPowerState_Avd( NEXUS_VideoDecoderHandle videoDecoder, bool powerUp )
{
    BDBG_OBJECT_ASSERT(videoDecoder, NEXUS_VideoDecoder);

    /* internally, powering up means opening the XVD channel. XVD takes care of the actual power up/down. */

    if (powerUp) {
        NEXUS_Error rc;

        if (!videoDecoder->dec) {
            /* if the user has not call NEXUS_VideoWindow_AddInput at least once, this function will fail.
            however, once they have called it once, it's ok to power up even if not connected. This is quirky and could
            easily change. For that reason, nexus is justing making the call and will allow XVD to fail if it must. */
            rc = NEXUS_VideoDecoder_P_OpenChannel(videoDecoder);
            if (rc) return BERR_TRACE(rc);
        }
    }
    else {
        /* if started, we can automatically stop. */
        if (videoDecoder->started) {
            NEXUS_VideoDecoder_Stop(videoDecoder);
        }

        /* if we're connected, we must fail. destroying a connected XVD channel will cause VDC window timeouts.
        the VDC window must be destroyed using NEXUS_VideoWindow_RemoveInput. */
        if (videoDecoder->displayConnection.dataReadyCallback_isr) {
            BDBG_ERR(("You must call NEXUS_VideoWindow_RemoveInput before NEXUS_VideoDecoder_SetPowerState(false)."));
            return BERR_TRACE(NEXUS_UNKNOWN);
        }

        if (videoDecoder->dec) {
            NEXUS_VideoDecoder_P_CloseChannel(videoDecoder);
        }
    }
    return 0;
}

void NEXUS_VideoDecoder_GetMosaicSettings( NEXUS_VideoDecoderHandle videoDecoder, NEXUS_VideoDecoderMosaicSettings *pSettings )
{
    BDBG_OBJECT_ASSERT(videoDecoder, NEXUS_VideoDecoder);
    *pSettings = videoDecoder->mosaicSettings;
}

NEXUS_Error NEXUS_VideoDecoder_SetMosaicSettings( NEXUS_VideoDecoderHandle videoDecoder, const NEXUS_VideoDecoderMosaicSettings *pSettings )
{
    NEXUS_Error rc;

    BDBG_OBJECT_ASSERT(videoDecoder, NEXUS_VideoDecoder);

    if (pSettings->maxWidth != videoDecoder->mosaicSettings.maxWidth  ||
        pSettings->maxHeight != videoDecoder->mosaicSettings.maxHeight)
    {
        /* must set before calling NEXUS_VideoDecoder_P_OpenChannel */
        videoDecoder->mosaicSettings = *pSettings;

        if (videoDecoder->dec) {
            if (videoDecoder->input.destination || videoDecoder->input.ref_cnt) {
                BDBG_ERR(("Cannot change maxWidth/Height while still connected as an input. You must call NEXUS_VideoWindow_RemoveInput and NEXUS_VideoInput_Shutdown beforehand."));
                return BERR_TRACE(NEXUS_NOT_SUPPORTED);
            }

            /* must close and reopen XVD channel */
            NEXUS_VideoDecoder_P_CloseChannel(videoDecoder);
            rc = NEXUS_VideoDecoder_P_OpenChannel(videoDecoder);
            if (rc) return BERR_TRACE(rc);
        }
    }

    /* this redundant set is here in case more settings than maxWidth/Height are added */
    videoDecoder->mosaicSettings = *pSettings;

    return 0;
}

NEXUS_Error NEXUS_VideoDecoderModule_Standby_priv(bool enabled, const NEXUS_StandbySettings *pSettings)
{
#if NEXUS_POWER_MANAGEMENT
    unsigned avdIndex;
    NEXUS_Error rc;

    BSTD_UNUSED(pSettings);

    /* We only check whether any video decoder is started. This forces application to stop decoder
       before entering standby.
       The actual power down is done from display when we remove the input */
    for(avdIndex=0; avdIndex<NEXUS_NUM_XVD_DEVICES; avdIndex++) {
        unsigned channelIndex;
        for(channelIndex=0; channelIndex<NEXUS_NUM_XVD_CHANNELS; channelIndex++) {
            NEXUS_VideoDecoderHandle videoDecoder;
            videoDecoder = &g_NEXUS_videoDecoderXvdDevices[avdIndex].channel[channelIndex];

            if(!videoDecoder) {
                continue;
            }

            if(videoDecoder->started) {
                BDBG_WRN(("Video decoder needs to be stopped before entering standby"));
                return NEXUS_NOT_SUPPORTED;
            }
        }
    }

    for(avdIndex=0; avdIndex<NEXUS_NUM_XVD_DEVICES; avdIndex++) {
        if(enabled) {
            rc = BXVD_Standby(g_NEXUS_videoDecoderXvdDevices[avdIndex].xvd);
        } else {
            rc = BXVD_Resume(g_NEXUS_videoDecoderXvdDevices[avdIndex].xvd);
        }
        if (rc) return BERR_TRACE(rc);
    }
#else
    BSTD_UNUSED(enabled);
    BSTD_UNUSED(pSettings);
#endif
    return NEXUS_SUCCESS;
}

NEXUS_RaveHandle NEXUS_VideoDecoder_DetachRaveContext( NEXUS_VideoDecoderHandle videoDecoder )
{
    BDBG_OBJECT_ASSERT(videoDecoder, NEXUS_VideoDecoder);
    if (!videoDecoder->raveDetached) {
        videoDecoder->raveDetached = true;
        return videoDecoder->rave;
    }
    else {
        BERR_TRACE(NEXUS_NOT_AVAILABLE);
        return NULL;
    }
}

NEXUS_RaveHandle NEXUS_VideoDecoder_DetachEnhancementRaveContext( NEXUS_VideoDecoderHandle videoDecoder )
{
    BDBG_OBJECT_ASSERT(videoDecoder, NEXUS_VideoDecoder);
    if (!videoDecoder->enhancementRaveDetached) {
        videoDecoder->enhancementRaveDetached = true;
        return videoDecoder->enhancementRave;
    }
    else {
        BERR_TRACE(NEXUS_NOT_AVAILABLE);
        return NULL;
    }
}

NEXUS_Error NEXUS_VideoDecoder_AttachRaveContext( NEXUS_VideoDecoderHandle videoDecoder, NEXUS_RaveHandle rave )
{
    BDBG_OBJECT_ASSERT(videoDecoder, NEXUS_VideoDecoder);
    if (videoDecoder->raveDetached && rave == videoDecoder->rave) {
        videoDecoder->raveDetached = false;
        return 0;
    }
    else if (videoDecoder->enhancementRaveDetached && rave == videoDecoder->enhancementRave) {
        videoDecoder->enhancementRaveDetached = false;
        return 0;
    }
    else {
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);;
    }
}

#include "bxvd_ppb.h"

#if BXVD_PPB_EXTENDED
static void NEXUS_VideoDecoder_P_AddCrc_isr( NEXUS_VideoDecoderHandle videoDecoder, const NEXUS_VideoDecoderCrc *entry )
{
    if (!videoDecoder->crc.size) return;

    videoDecoder->crc.data[videoDecoder->crc.wptr] = *entry;
    if (++videoDecoder->crc.wptr == videoDecoder->crc.size) {
        videoDecoder->crc.wptr = 0;
    }
    if (videoDecoder->crc.rptr == videoDecoder->crc.wptr) {
        BDBG_WRN(("crc overflow"));
    }
}
#endif

static void NEXUS_VideoDecoder_P_PPBReceived_isr(void *context, int unused, void *pp_ppb)
{
    NEXUS_VideoDecoderHandle videoDecoder = (NEXUS_VideoDecoderHandle)context;


#if BXVD_PPB_EXTENDED
    const BXVD_PPB *pPPB = *(BXVD_PPB **)pp_ppb;
    NEXUS_VideoDecoderCrc crc;
#endif

    BSTD_UNUSED(unused);
    BDBG_OBJECT_ASSERT(videoDecoder, NEXUS_VideoDecoder);
#if BXVD_PPB_EXTENDED 
    crc.top.luma = pPPB->crc.crc_luma;
    crc.top.cr = pPPB->crc.crc_cr;
    crc.top.cb = pPPB->crc.crc_cb;
    crc.bottom.luma = pPPB->crc.crc_luma_bot;
    crc.bottom.cr = pPPB->crc.crc_cr_bot;
    crc.bottom.cb = pPPB->crc.crc_cb_bot;
    NEXUS_VideoDecoder_P_AddCrc_isr(videoDecoder, &crc);
#else
    BSTD_UNUSED(pp_ppb);
#endif
}

NEXUS_Error NEXUS_VideoDecoder_P_SetCrcFifoSize(NEXUS_VideoDecoderHandle videoDecoder, bool forceDisable)
{
    unsigned size = forceDisable?0:videoDecoder->extendedSettings.crcFifoSize;
    BDBG_OBJECT_ASSERT(videoDecoder, NEXUS_VideoDecoder);

    if (size != videoDecoder->crc.size && videoDecoder->crc.size) {
        BKNI_EnterCriticalSection();
        videoDecoder->crc.size = 0;
        BKNI_LeaveCriticalSection();
        BKNI_Free(videoDecoder->crc.data);
    }
    if (size && !videoDecoder->crc.size) {
        videoDecoder->crc.data = BKNI_Malloc(size*sizeof(NEXUS_VideoDecoderCrc));
        if (!videoDecoder->crc.data) {
            return BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);
        }
        BKNI_EnterCriticalSection();
        videoDecoder->crc.size = size;
        videoDecoder->crc.rptr = videoDecoder->crc.wptr = 0;
        BKNI_LeaveCriticalSection();
    }

    if (videoDecoder->dec) {
        if (videoDecoder->crc.size) {
            int rc;
            rc = BXVD_InstallInterruptCallback(videoDecoder->dec, BXVD_Interrupt_ePPBReceived, NEXUS_VideoDecoder_P_PPBReceived_isr, videoDecoder, 0);
            if (rc) return BERR_TRACE(rc);
            rc = BXVD_EnableInterrupt(videoDecoder->dec, BXVD_Interrupt_ePPBReceived, true);
            if (rc) return BERR_TRACE(rc);
        }
        else {
            BXVD_UnInstallInterruptCallback(videoDecoder->dec, BXVD_Interrupt_ePPBReceived);
        }
    }

    return 0;
}

NEXUS_Error NEXUS_VideoDecoder_GetCrcData( NEXUS_VideoDecoderHandle videoDecoder, NEXUS_VideoDecoderCrc *pEntries, unsigned numEntries, unsigned *pNumReturned )
{
    BDBG_OBJECT_ASSERT(videoDecoder, NEXUS_VideoDecoder);

    *pNumReturned = 0;
    BKNI_EnterCriticalSection();
    while (numEntries--) {
        if (videoDecoder->crc.rptr != videoDecoder->crc.wptr) {
            pEntries[*pNumReturned] = videoDecoder->crc.data[videoDecoder->crc.rptr];
            (*pNumReturned)++;
            if (++videoDecoder->crc.rptr == videoDecoder->crc.size) {
                videoDecoder->crc.rptr = 0;
            }
        }
    }
    BKNI_LeaveCriticalSection();
    return 0;
}

NEXUS_Error NEXUS_VideoDecoder_P_SetLowLatencySettings(NEXUS_VideoDecoderHandle videoDecoder)
{
    NEXUS_Error rc = NEXUS_SUCCESS;

    if (videoDecoder->startSettings.stcChannel)
    {
        /* low latency mode must be communicated to stc channel, so it will handle requests and errors properly */
        NEXUS_StcChannelLowLatencySettings llSettings;

        LOCK_TRANSPORT();

        NEXUS_StcChannel_GetDefaultLowLatencySettings_priv(&llSettings);
        llSettings.enabled = videoDecoder->extendedSettings.lowLatencyTsmMode;
        rc = NEXUS_StcChannel_SetLowLatencySettings_priv(videoDecoder->startSettings.stcChannel, NEXUS_StcChannelDecoderType_eVideo0, &llSettings);

        UNLOCK_TRANSPORT();

        if (rc) return BERR_TRACE(rc);
    }

    return rc;
}

void NEXUS_GetVideoDecoderCapabilities( NEXUS_VideoDecoderCapabilities *pCapabilities )
{
    BKNI_Memset(pCapabilities, 0, sizeof(*pCapabilities));
    pCapabilities->numVideoDecoders = NEXUS_NUM_VIDEO_DECODERS;
#if NEXUS_NUM_DSP_VIDEO_DECODERS
    pCapabilities->dspVideoDecoder.total = NEXUS_NUM_DSP_VIDEO_DECODERS;
    pCapabilities->dspVideoDecoder.baseIndex = NEXUS_NUM_VIDEO_DECODERS;
    pCapabilities->dspVideoDecoder.useForVp6 = true;
#endif
}
