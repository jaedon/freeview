/******************************************************************************
*    (c)2008-2011 Broadcom Corporation
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
 * $brcm_Workfile: transcode_ts.c $
 * $brcm_Revision: Hydra_Software_Devel/126 $
 * $brcm_Date: 10/25/12 3:12p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/encoder/transcode_ts.c $
 * 
 * Hydra_Software_Devel/126   10/25/12 3:12p hongtaoz
 * SW7425-4105: added command option to throttle stream mux NRT speed;
 * 
 * Hydra_Software_Devel/125   10/11/12 11:04a hongtaoz
 * SW7425-4022: removed sync channel restriction for NRT mode;
 * 
 * Hydra_Software_Devel/124   10/8/12 6:31p mward
 * SW7425-3782: use NEXUS_FrontendConnector
 * 
 * Hydra_Software_Devel/123   10/8/12 4:36p hongtaoz
 * SW7425-4021: added decoder status report to command '14'; report AV
 * sync error only if audio is enabled;
 * 
 * Hydra_Software_Devel/122   9/26/12 2:44p hongtaoz
 * SW7425-3999: added window resize option to the loopback player menu;
 * added record stop/start option to main menu to test NRT mode back
 * pressure;
 * 
 * Hydra_Software_Devel/121   9/25/12 2:33p hongtaoz
 * SW7435-371: added level 1.x options for video encoder setting;
 * 
 * Hydra_Software_Devel/120   9/19/12 2:42p hongtaoz
 * SW7425-3953: fixed nexus kernel mode link error;
 * 
 * Hydra_Software_Devel/119   9/17/12 3:08p hongtaoz
 * SW7425-3953: added run-time AV sync error report in the status command
 * '14';
 * 
 * Hydra_Software_Devel/118   9/11/12 3:17p hongtaoz
 * SWDEPRECATED-2917: open STC in PCR mode with PCR pid channel assigned
 * (NULL is not allowed now);
 * 
 * Hydra_Software_Devel/117   8/31/12 3:00p hongtaoz
 * SW7429-254: fixed compile warnings for non-ViCE encoder and release
 * build;
 * 
 * Hydra_Software_Devel/116   8/20/12 4:29p hongtaoz
 * SW7425-3684, SW7425-3704: added mux status; added gfx test;
 * 
 * Hydra_Software_Devel/115   8/16/12 11:29a hongtaoz
 * SW7425-3738: added transcoder stop options;
 * 
 * Hydra_Software_Devel/114   8/15/12 5:42p katrep
 * SW7429-241:fix compile issue for the platforms without frontend
 * 
 * Hydra_Software_Devel/113   8/8/12 2:20p hongtaoz
 * SW7425-3115: added AAC audio transcoder sample rate setting;
 * 
 * Hydra_Software_Devel/112   7/29/12 9:51p agin
 * SWNOOS-559:  Support transcoding for no-os.
 * 
 * Hydra_Software_Devel/111   7/18/12 7:29p hongtaoz
 * SW7425-3454, SW7425-3489: added MTSIF setting for VMS_SFF board
 * frontend; use NEXUS_ANY_ID for STC channels;
 * 
 * Hydra_Software_Devel/110   7/10/12 4:54p gmohile
 * SW7231-894 : Disable de-interlacer for primary display
 * 
 * Hydra_Software_Devel/109   6/25/12 6:58p hongtaoz
 * SW7425-3156: clear error status after each query; allow hexdecimal PID
 * values from config input;
 * 
 * Hydra_Software_Devel/108   6/25/12 2:00p hongtaoz
 * SW7425-3180: added secondary audio PID connection for playback when
 * enabled to avoid secondary audio wraparound stall;
 * 
 * Hydra_Software_Devel/107   6/7/12 2:34p hongtaoz
 * SW7425-2725: released context mutex before context switch; added audio
 * dynamic encode bitrate test;
 * 
 * Hydra_Software_Devel/106   6/6/12 11:34a hongtaoz
 * SW7425-3167: removed trackInput from NEXUS_VideoEncoderStreamStructure;
 * 
 * Hydra_Software_Devel/105   5/31/12 7:27p hongtaoz
 * SW7425-2725: unlock mutext before blocking call; close hdmi output
 * before destroying display;
 * 
 * Hydra_Software_Devel/104   5/29/12 4:56p hongtaoz
 * SW7425-3136, SW7425-2606: added -msaudio, -multichan command line
 * options to test audio transcode path DSP bandwidth; added -msp and -
 * rsp command line option to set mux service period and record service
 * period;
 * 
 * Hydra_Software_Devel/103   5/29/12 2:59p hongtaoz
 * SW7425-2663: removed recpump data ready threshold override now that
 * band hold threshold is de-coupled with data ready threshold;
 * 
 * Hydra_Software_Devel/102   5/25/12 4:33p hongtaoz
 * SW7435-165: always use auto mode for transcode STC channel;
 * 
 * Hydra_Software_Devel/101   5/22/12 11:28a hongtaoz
 * SW7435-197: added '-nodspmix' command line option;
 * 
 * Hydra_Software_Devel/100   5/21/12 6:25p hongtaoz
 * SW7435-165: increased audio module numCompressedBuffers count according
 * to number of xcodes to allow quad audio passthru;
 * 
 * Hydra_Software_Devel/99   5/21/12 1:00p hongtaoz
 * SW7435-165: scale max audio DSP tasks according to xcode needs;
 * 
 * Hydra_Software_Devel/98   5/21/12 11:33a hongtaoz
 * SW7435-165: removed 1u4t compile option; stc set pcr mode for tuner
 * input;
 * 
 * Hydra_Software_Devel/97   5/18/12 1:06p hongtaoz
 * SW7435-48: fixed segfault at quit for video only NRT mode;
 * 
 * Hydra_Software_Devel/96   4/24/12 6:50p hongtaoz
 * SW7435-113: enable loopback decoder when num of xcodes is less than
 * quad;
 * 
 * Hydra_Software_Devel/95   4/24/12 10:55a gmohile
 * SW7231-785 : Add raaga encoder support
 * 
 * Hydra_Software_Devel/94   4/4/12 5:13p hongtaoz
 * SW7425-2781, SW7425-2744: don't force CapScale bvn mode for hdmi
 * transcoding since hdmi input might be 1080p60 and dual transcode RTS
 * doesn't support 1080p60 capture; NRT mode writes all record data;
 * 
 * Hydra_Software_Devel/93   4/2/12 3:20p hongtaoz
 * SW7425-2812: added script mode support;
 * 
 * Hydra_Software_Devel/92   3/29/12 5:44p hongtaoz
 * SW7425-2674, SW7435-48: reduced NRT mode STCs allocation to 2 per
 * xcoder; NRT playback video pid setting is removed, STC is null,
 * encoder/mux STC the same as audio decoder STC;
 * 
 * Hydra_Software_Devel/91   3/27/12 5:39p hongtaoz
 * SW7435-50: only use 2nd DSP if xcode contexts are more 2;
 * 
 * Hydra_Software_Devel/90   3/22/12 1:08p hongtaoz
 * SW7435-50: added 2nd DSP for quad audio xcoders;
 * 
 * Hydra_Software_Devel/90   3/22/12 1:06p hongtaoz
 * SW7435-50: added 2nd DSP for quad audio xcoders;
 * 
 * Hydra_Software_Devel/89   3/12/12 5:54p gkamosa
 * SW7425-2543: Merge 3383SMS platform support to mainline ref_sw
 * 
 * Hydra_Software_Devel/89   3/12/12 4:41p gkamosa
 * SW7425-2543: Merge support for 3383 SMS platform
 * 
 * Hydra_Software_Devel/88   3/6/12 10:34p hongtaoz
 * SW7435-50: fixed typo;
 * 
 * Hydra_Software_Devel/87   3/6/12 7:47p hongtaoz
 * SW7435-50: bring up quad transcoders;
 * 
 * Hydra_Software_Devel/86   3/5/12 10:51a hongtaoz
 * SW7425-1112: set the input user data PID according to auto scan for all
 * pass mode;
 * 
 * Hydra_Software_Devel/85   2/28/12 5:22p vsilyaev
 * SW7425-2271: Don't peek with application into a user data buffer
 * 
 * Hydra_Software_Devel/84   2/28/12 5:19p hongtaoz
 * SW7425-2502: enable recpump band hold for RT mode transcoder output;
 * 
 * Hydra_Software_Devel/83   2/27/12 2:29p hongtaoz
 * SW7425-2429: close/open transcode record file for stop/start test;
 * 
 * Hydra_Software_Devel/82   2/24/12 7:25p hongtaoz
 * SW7425-2348, SW7425-2271: separate system data create/release vs
 * start/stop;
 * 
 * Hydra_Software_Devel/81   2/24/12 6:14p hongtaoz
 * SW7425-2174: reduced NRT mode record polling interval to prevent back
 * pressure in some case; conditionally flush audio mux output at stop
 * only when audio is enabled;
 * 
 * Hydra_Software_Devel/80   2/24/12 3:31p hongtaoz
 * SW7425-2416: flush audio mux output after immediate stop to prevent
 * leftover data at restart;
 * 
 * Hydra_Software_Devel/79   2/17/12 7:15p hongtaoz
 * SW7425-2327: added AFD/Bar size clipping test;
 * 
 * Hydra_Software_Devel/78   2/17/12 5:57p hongtaoz
 * SW7425-2430: added random dynamic resolution and bitrate tests;
 * 
 * Hydra_Software_Devel/77   2/17/12 2:52p hongtaoz
 * SW7425-2348: separated stop/start from open/close; added fast
 * stop/start test;
 * 
 * Hydra_Software_Devel/76   2/14/12 6:58p hongtaoz
 * SW7425-2045: use fixed audio delay mode for hdmi input with dsp mixer;
 * 
 * Hydra_Software_Devel/75   2/9/12 2:10p hongtaoz
 * SW7425-2349: allowed adaptive low latency start of selected transcoder;
 * 
 * Hydra_Software_Devel/74   2/9/12 1:50p hongtaoz
 * SW7425-2349, SW7425-2360: added immediate stop and adaptive latency
 * start test command; adding source trick player;
 * 
 * Hydra_Software_Devel/73   2/8/12 2:39p hongtaoz
 * SW7425-2078: added dynamic GOP setting test command;
 * 
 * Hydra_Software_Devel/72   2/6/12 4:08p hongtaoz
 * SW7425-2271: fixed leaked resource for user data filter event; added
 * context id to the PSI debug messages; fixed number of user data PIDs
 * for all pass mode;
 * 
 * Hydra_Software_Devel/71   2/2/12 7:20p hongtaoz
 * SW7425-2271,SW7425-2272: added PMT PIDs auto scan to search for user
 * data PIDs PSI info; now user can set PMT pid to 0 to enable auto scan;
 * 
 * Hydra_Software_Devel/70   2/1/12 5:31p hongtaoz
 * SW7425-2271, SW7425-2272: added message data ready callback for debug
 * purpose;
 * 
 * Hydra_Software_Devel/69   2/1/12 10:36a hongtaoz
 * SW7425-2271: set maxContiguousMessageSize to 0 for TS user data capture
 * and in-place operation;
 * 
 * Hydra_Software_Devel/68   1/30/12 3:37p hongtaoz
 * SW7425-2271, SW7425-2272: corrected number of user data PIDs for all
 * pass mode;
 * 
 * Hydra_Software_Devel/67   1/28/12 12:10a hongtaoz
 * SW7425-2266, SW7425-2271, SW7425-2272: add encoder stop option; added
 * TS user data transcoder test config; added TS user data PSI info pass-
 * through;
 * 
 * Hydra_Software_Devel/66   12/20/11 2:01p hongtaoz
 * SW7425-2036: type matching in the scanf;
 * 
 * Hydra_Software_Devel/65   12/19/11 11:27p hongtaoz
 * SW7425-1110: wrong codec of source user data decoder;
 * 
 * Hydra_Software_Devel/64   12/19/11 6:54p hongtaoz
 * SW7425-1110: added user data callback to log the source and loopback
 * decoders user data and parsed closed caption user data to verify the
 * cc data transcoding function;
 * 
 * Hydra_Software_Devel/63   12/16/11 7:20p hongtaoz
 * SW7425-1110, SW7425-1878: added CC data transcode config to
 * transcode_ts for NRT and dual test;
 * 
 * Hydra_Software_Devel/62   12/16/11 2:10p hongtaoz
 * SW7425-1748: fixed segfault for hdmi transcode at shutdown;
 * 
 * Hydra_Software_Devel/61   12/13/11 10:58p hongtaoz
 * SW7425-1748: cleaned the usage print;
 * 
 * Hydra_Software_Devel/60   12/9/11 6:57p hongtaoz
 * SW7425-1746, SW7425-1566: added test support for InsertRAP API and
 * single/multi video encoder type;
 * 
 * Hydra_Software_Devel/59   11/21/11 2:57p hongtaoz
 * SW7425-1821: dual QAM transcodes has muted video on the 2nd stream;
 * 
 * Hydra_Software_Devel/58   11/11/11 5:03p hongtaoz
 * SW7425-1748: added -autoquit option;
 * 
 * Hydra_Software_Devel/57   11/7/11 10:29a hongtaoz
 * SW7425-608: further remove the reference to 7.493fps;
 * 
 * Hydra_Software_Devel/56   11/7/11 10:14a hongtaoz
 * SW7425-608: removed 7.493 fps option since FW doesn't support it;
 * 
 * Hydra_Software_Devel/55   10/24/11 4:48p hongtaoz
 * SW7425-1580: removed I-frame count and added dropped count in video
 * encoder status;
 * 
 * Hydra_Software_Devel/54   10/20/11 11:19a hongtaoz
 * SW7425-468, SW7425-1559: added audio mixer for transcoder to have
 * continuous audio timestamps;
 * 
 * Hydra_Software_Devel/53   10/20/11 12:42a hongtaoz
 * SW7425-1549: fixed dual contexts segfault; added NRT mode loop option;
 * 
 * Hydra_Software_Devel/52   10/19/11 11:18a hongtaoz
 * SW7425-1317: added video encoder index in status report;
 * 
 * Hydra_Software_Devel/51   10/19/11 11:15a hongtaoz
 * SW7425-1459: clean shutdown dual NRT transcodes events;
 * 
 * Hydra_Software_Devel/50   10/13/11 11:31p hongtaoz
 * SW7425-1349: use audio dummy outputs to drive live audio transcode
 * paths to save actual audio outputs;
 * 
 * Hydra_Software_Devel/49   10/6/11 6:10p hongtaoz
 * SW7425-1278, SW7425-1320: fixed NRT mode segfault during quit; stop
 * loopback player of NRT context if NRT eos is hit;
 * 
 * Hydra_Software_Devel/48   10/4/11 3:09p hongtaoz
 * SW7425-1278, SW7425-1320: fixed the 2nd xcoder loopback issue; enables
 * DNR/MADR for the 2nd xcoder now;
 * 
 * Hydra_Software_Devel/47   10/3/11 3:26p hongtaoz
 * SW7425-1278, SW7425-1320: added dual transcodes context support
 * including start/stop selected context and stop/start loopback player
 * with a context; added FIFO file record option for long term test;
 * added slow motion control to the loopback player;
 * 
 * Hydra_Software_Devel/46   9/19/11 3:51p hongtaoz
 * SW7425-1317: added video encoder GetStatus command;
 * 
 * Hydra_Software_Devel/45   8/27/11 8:42p hongtaoz
 * SW7425-1132: disable scale factor rounding of the simul debug display
 * path in NRT mode; otherwise the source crop workaound could be
 * cancelled;
 * 
 * Hydra_Software_Devel/44   8/26/11 7:13p hongtaoz
 * SW7425-1132: disable the simul debug display path for NRT mode by
 * default; source cropping might work around the SCL2 error due to early
 * EOP trigger, however it has side effect and is for debug only; use 3rd
 * video decoder for transcode;
 * 
 * Hydra_Software_Devel/43   8/26/11 1:22p hongtaoz
 * SW7425-838: fixed assertion error in app when quit hdmi transcode test;
 * 
 * Hydra_Software_Devel/42   8/24/11 10:39a hongtaoz
 * SW7425-651: fixed user input of boolean setting for delay
 * customization;
 * 
 * Hydra_Software_Devel/41   8/22/11 10:05a hongtaoz
 * SW7425-651: changed custom delay user prompt; removed order dependent
 * workaround for NRT mode; always stop encode first before STG display
 * shutdown;
 * 
 * Hydra_Software_Devel/40   8/21/11 5:39p hongtaoz
 * SW7425-592: added STG display sample aspect ratio x&y custom config;
 * 
 * Hydra_Software_Devel/39   8/18/11 6:18p hongtaoz
 * SW7425-651: added video encoder delay config from user input;
 * 
 * Hydra_Software_Devel/38   8/18/11 4:49p hongtaoz
 * SW7425-1144: set higher dataReadyThreshold for recpump under NRT mode
 * to support faster than realtime NRT performance;
 * 
 * Hydra_Software_Devel/37   8/17/11 1:38p hongtaoz
 * SW7425-651: added new API for lower encode delay;
 * 
 * Hydra_Software_Devel/36   8/15/11 4:59p hongtaoz
 * SW7425-860: removed app workaround for NRT mode transcode; enable the
 * simul display for NRT mode transcode;
 * 
 * Hydra_Software_Devel/35   8/4/11 10:06a vanessah
 * SW7425-1048: add MADR low delay
 * 
 * Hydra_Software_Devel/34   8/3/11 1:20p vanessah
 * SW7425-1048: add low delay mode api
 * 
 * Hydra_Software_Devel/33   8/3/11 11:12a vanessah
 * SW7425-1047: add advanced simple profile for MPEG-4 part2
 * 
 * Hydra_Software_Devel/32   7/21/11 4:47p vanessah
 * SW7425-165: add VC-1 entry
 * 
 * Hydra_Software_Devel/31   7/6/11 12:02p nilesh
 * SW7425-654: Fixed typo for 7.493 fps nexus frame rate constant
 * 
 * Hydra_Software_Devel/30   6/30/11 1:49a hongtaoz
 * SW7425-654: disable simul SD window for NRT mode for now; disable sync
 * channel mute and precision lipsync for NRT mode;
 * 
 * Hydra_Software_Devel/29   6/29/11 12:21p hongtaoz
 * SW7425-654: enable recpump flow control for NRT mode; enable av_window
 * for NRT decodes; default variable frame rate coding off and added menu
 * option to toggle it; add back debug simul display window for NRT mode;
 * 
 * Hydra_Software_Devel/28   6/28/11 6:24p hongtaoz
 * SW7425-654, SW7425-787, SW7425-762: added NRT mode av window setting;
 * shut down transcode window/display before encoder stop to temporarily
 * work around NRT mode shutdown timeout issue (ViCE2 fw doesn't put VIP
 * into drain state when stopped);
 * 
 * Hydra_Software_Devel/27   6/28/11 9:13a hongtaoz
 * SW7425-654: NRT mode audio mux output requires the same STC as decoder;
 * NRT mode audio decoder cannot have DAC/SPDIF outputs; disable NRT STC
 * av_window before the lockup is resolved;
 * 
 * Hydra_Software_Devel/26   6/23/11 3:58p vanessah
 * SW7425-761: add sleep function for easy dubug
 * 
 * Hydra_Software_Devel/25   6/23/11 9:29a vanessah
 * SW7425-762: Display scalefactor rounding for transcoding window
 * 
 * Hydra_Software_Devel/24   6/22/11 4:29p hongtaoz
 * SW7425-654: finish NRT mode transcode at end of file automatically;
 * 
 * Hydra_Software_Devel/23   6/22/11 2:06p hongtaoz
 * SW7425-654, SW7425-165: added NRT audio config; enable MADR for
 * transcoder path from the beginning; added stop/restart iteration test;
 * added dynamic bitrate/format/gop change autotest;
 * 
 * Hydra_Software_Devel/22   6/17/11 1:35p hongtaoz
 * SW7425-733, SW7425-165: use circular buffer to feed PAT/PMT packets to
 * avoid CC value being overwritten by app before it's transferred by ts
 * mux; enable MADR by default for transcode window;
 * 
 * Hydra_Software_Devel/21   6/15/11 6:55p hongtaoz
 * SW7425-733, SW7425-654, SW7425-480: increment CC value for PAT/PMT
 * packets; stop looping for NRT file transcode; add mpeg4 video encode
 * test;
 * 
 * Hydra_Software_Devel/20   6/14/11 11:41a hongtaoz
 * SW7425-654: added video decode and stream mux NRT mode setting; set mux
 * speed to 32x for AFAP;
 * 
 * Hydra_Software_Devel/19   6/7/11 6:17p hongtaoz
 * SW7425-165: added 50/25 hz frame rate settings; add back timeshift gap
 * to avoid stutter; set display format as 50hz if encode frame rate is
 * so; add NRT mode setting for transcode display and encoder;
 * 
 * Hydra_Software_Devel/18   6/3/11 5:43p vanessah
 * SW7425-454: add MADR enable and resolution change into nightly test
 * 
 * Hydra_Software_Devel/17   6/3/11 1:09p hongtaoz
 * SW7425-42, SW7405-4957, SW7425-165: added xcoder scaler coeff
 * selection; fixed PAT/PMT ts packet error; changed loopback display
 * format to 720p as default; removed timeshift playback delay to 0;
 * changed QAM GetStatus API call;
 * 
 * Hydra_Software_Devel/16   5/27/11 11:22a hongtaoz
 * SW7425-165: added source change callback;
 * 
 * Hydra_Software_Devel/15   5/24/11 7:18p hongtaoz
 * SW7425-481,SW7425-477, SW7425-465: add 50hz video encode format; add
 * mp3 audio encode test; add min display format and bandwidth bias to
 * transcode/loopback windows to avoid black frame transition during
 * dynamic resolution test;
 * 
 * Hydra_Software_Devel/14   5/18/11 5:44p hongtaoz
 * SW7425-42, SW7405-4957: add PAT/PMT config for the supported codecs;
 * added periodic PAT/PMT insertion timer;
 * 
 * Hydra_Software_Devel/13   5/18/11 1:16p vanessah
 * SW7425-454: update standalone unittest
 * 
 * Hydra_Software_Devel/12   5/16/11 6:00p hongtaoz
 * SW7425-165: fixed typo;
 * 
 * Hydra_Software_Devel/11   5/16/11 4:18p hongtaoz
 * SW7425-165: add aspect ratio correction choices to the transcoder path
 * and loopback path;
 * 
 * Hydra_Software_Devel/10   5/11/11 5:12p vanessah
 * SW7425-454: standalone test
 * 
 * Hydra_Software_Devel/9   4/24/11 3:04p hongtaoz
 * SW7425-165: added 1080p60 STG format; cosmetic cleanup on prompts;
 * 
 * Hydra_Software_Devel/8   4/22/11 12:17a hongtaoz
 * SW7425-165, SW7425-139: clean shutdown; increased mux finish wait time
 * to accomandate large encode buffer a2p delay;
 * 
 * Hydra_Software_Devel/7   4/19/11 2:33p hongtaoz
 * SW7425-140: added xcode source progressive scan mode control to test
 * 3:2/2:2 progressive ITFP function;
 * 
 * Hydra_Software_Devel/6   4/19/11 1:56p hongtaoz
 * SW7425-171: added SetEncodeSettings_OnInputChange API test to dynamic
 * resolution test; added individual A/V/PCR stream mux PES pacing toggle
 * control to keyhandler;
 * 
 * Hydra_Software_Devel/5   4/18/11 6:25p hongtaoz
 * SW7425-376: enable variable frame rate flag since encoder ITFP is
 * working now;
 * 
 * Hydra_Software_Devel/4   4/17/11 7:08p hongtaoz
 * SW7425-146: added HDMI PCM audio encode to ts unittest;
 * 
 * Hydra_Software_Devel/3   4/15/11 11:44a hongtaoz
 * SW7425-165: differentiate AAC+Loas and AAC+Adts; added interlaced
 * setting for interlaced coding;
 * 
 * Hydra_Software_Devel/2   4/11/11 6:52p hongtaoz
 * SW7425-165: added DNR demo mode control for xcode window;
 * 
 * Hydra_Software_Devel/1   4/8/11 6:50p hongtaoz
 * SW7425-165: move configurable nexus transcode unittests to rockford;
 * 
 * 13   4/7/11 7:25p hongtaoz
 * SW7425-165: added interlaced option to custom xcode format;
 * 
 * 12   4/6/11 7:23p hongtaoz
 * SW7425-165: fixed fast forward; added mad/dnr dynamic settings (you
 *  have to manually enable MAD on xcode path); added loopback display
 *  format control;
 * 
 * 11   4/6/11 3:03p hongtaoz
 * SW7425-165: added EDID to hdmi input;
 * 
 * 10   4/4/11 7:10p hongtaoz
 * SW7425-165:added hdmi video encode record index and loopback display;
 *  consolidate xcode context; changed xcode loopback display to
 *  hdmi/component outputs while source display to cvbs out since VMS_SFF
 *  board only has hdmi output by default;
 * 
 * 9   4/1/11 7:16p hongtaoz
 * SW7425-165: fixed timeshift playback; add audio loopback; add trick
 *  play control for loopback; add QAM retune;
 * 
 * 8   4/1/11 11:12a hongtaoz
 * SW7425-165: clean shutdown hdmi transcode test; added prompt for
 *  dynamic encode setting;
 * 
 * 7   3/31/11 7:39p hongtaoz
 * SW7425-165: add transcode loopback display; add dynamic
 *  bitrate/resolution/frame rate test; enable DNR/MAD on transcode window
 *  by default;
 * 
 * 6   3/28/11 6:58p hongtaoz
 * SW7425-165: combined qam/file transcode test; fixed audio xcode;
 * 
 * 5   3/28/11 1:46p hongtaoz
 * SW7425-165: fixed qam_transcode_ts errors;
 * 
 * 4   3/26/11 8:46p hongtaoz
 * SW7425-165: clean shutdown calls;
 * 
 * 3   3/25/11 12:15a hongtaoz
 * SW7425-165: Get rid of tight loops and flooding msg to console;
 *
 * 2   3/16/11 4:51p hongtaoz
 * SW7425-165: merge fixes to mainline;
 *
 * SW7425-165/2   3/15/11 12:24p vanessah
 * SW7425-165: change displayformat enum err
 *
 * 1   3/14/11 9:07p hongtaoz
 * SW7425-165: add ts transcode test;
 *
 * SW7425-165/1   3/14/11 7:46p vanessah
 * SW7425-165: add ts encode. HDMI and frontend audio to be added
*
*****************************************************************************/
/* nexus unittest: transcode file/qam/hdmi source to ts output */

#include "nexus_platform.h"
#include "nexus_video_decoder.h"
#include "nexus_parser_band.h"
#include "nexus_stc_channel.h"
#include "nexus_display.h"
#include "nexus_video_window.h"
#include "nexus_video_adj.h"
#include "nexus_video_input.h"
#include "nexus_core_utils.h"
#include "nexus_spdif_output.h"
#include "nexus_component_output.h"
#include "nexus_graphics2d.h"
#if NEXUS_HAS_VIDEO_ENCODER && NEXUS_HAS_STREAM_MUX
#include "nexus_playback.h"
#include "nexus_record.h"
#include "nexus_file.h"
#include "nexus_video_encoder.h"
#include "nexus_audio_encoder.h"
#include "nexus_stream_mux.h"
#include "nexus_recpump.h"
#include "nexus_file_fifo.h"
#include "tshdrbuilder.h"
#include "b_os_lib.h"
#include "nexus_audio_mixer.h"
#endif
#if NEXUS_NUM_HDMI_INPUTS
#include "nexus_hdmi_input.h"
#endif
#if NEXUS_DTV_PLATFORM
#include "nexus_platform_boardcfg.h"
#endif
#if NEXUS_HAS_SYNC_CHANNEL
#include "nexus_sync_channel.h"
#endif

#define USE_DDRE    0 /* set to 1 to enable DDRE&DV258 for multi-channel MS-11 -> AAC xcode test */

#if USE_DDRE
#include "nexus_dolby_digital_reencode.h"
#include "nexus_dolby_volume.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "bstd.h"
#include "bkni.h"

#define BTST_ENABLE_FAST_STOP_START 1

/* 2 audio xcoders per DSP core */
#define BTST_NUM_AUDIO_XCODE_PER_DSP 2

/* stop-start iterations */
#define TEST_ITERATIONS 3

/* NRT a+v transcode STC av_window enable test */
#define BTST_ENABLE_NRT_STC_AV_WINDOW 1

/* TODO: set to 0 to enable the debug monitor for NRT mode source decode; set to 1 to disable it. */
#define BTST_DISABLE_NRT_DEBUG_DISPLAY 1

#if NEXUS_HAS_SYNC_CHANNEL
#define BTST_ENABLE_AV_SYNC 1
#else
#define BTST_ENABLE_AV_SYNC 0
#endif

/* to debug TS mux pacing */
#define BTST_DISABLE_VIDEO_ENCODER_PES_PACING 0

/* to enable xcode output lookback to HD display */
#if !defined(NEXUS_HAS_HDMI_OUTPUT) || !defined(NEXUS_HAS_DISPLAY) || NEXUS_NUM_DSP_VIDEO_ENCODERS
#define BTST_ENABLE_XCODE_LOOPBACK    0
#else
#define BTST_ENABLE_XCODE_LOOPBACK    1
#endif

/***********************************
 *   Display assignments 
 */
#if NEXUS_NUM_DSP_VIDEO_ENCODERS
#ifndef NEXUS_ENCODER_DISPLAY_IDX
#define NEXUS_ENCODER_DISPLAY_IDX   1
#endif
/* source display locally as SD CVBS output by default */
#define BTST_SOURCE_DISPLAY_IDX     0
/* xcode loopback display as HD hdmi/component output by default */
#define BTST_LOOPBACK_DISPLAY_IDX   0
#else 
#ifndef NEXUS_ENCODER_DISPLAY_IDX
#define NEXUS_ENCODER_DISPLAY_IDX   3
#endif
/* source display locally as SD CVBS output by default */
#define BTST_SOURCE_DISPLAY_IDX     1
/* xcode loopback display as HD hdmi/component output by default */
#define BTST_LOOPBACK_DISPLAY_IDX   0
#endif

/***********************************
 *   Source ID assignments 
 */
#define BTST_RESOURCE_FILE       0
#define BTST_RESOURCE_HDMI       1
#define BTST_RESOURCE_QAM        2

/***********************************
 *   PID assignments 
 */
#define BTST_MUX_PCR_PID        (0x11)
#define BTST_MUX_VIDEO_PID      (0x12)
#define BTST_MUX_AUDIO_PID      (0x13)
#define BTST_MUX_USER_DATA_PID  (BTST_MUX_AUDIO_PID+6)
#define BTST_MUX_PMT_PID        (0x55)
#define BTST_MUX_PAT_PID        (0x0)

/***********************************
 *   STC assignments (2 per xcoder)
 */

/***********************************
 *   Playpump assignments 
 */
#define BTST_LOOPBACK_PLAYPUMP_IDX           0
#define BTST_STREAM_MUX_VIDEO_PLAYPUMP_IDX  (NEXUS_NUM_VIDEO_ENCODERS)
#define BTST_STREAM_MUX_AUDIO_PLAYPUMP_IDX  (NEXUS_NUM_VIDEO_ENCODERS+1)
#define BTST_STREAM_MUX_PCR_PLAYPUMP_IDX    (NEXUS_NUM_VIDEO_ENCODERS+2)
#define BTST_SOURCE_PLAYPUMP_IDX            (NEXUS_NUM_VIDEO_ENCODERS+3)

/***********************************
 *   Video Decoder assignments 
 */
#ifdef NEXUS_NUM_DSP_VIDEO_ENCODERS
#define BTST_XCODE_VID_DECODE_IDX 0
#else
#define BTST_XCODE_VID_DECODE_IDX       (NEXUS_NUM_VIDEO_DECODERS-1)
#define BTST_LOOPBACK_VID_DECODE_IDX    0
#define BTST_LOOPBACK_AUD_DECODE_IDX    (NEXUS_NUM_AUDIO_DECODERS-1)
#endif

/***********************************
 *   Closed Caption user data count
 */
#define BTST_MAX_CC_DATA_COUNT          32

/* multi buffer to allow the background PSI packet to update CC while foreground one is waiting for TS mux pacing */
#define BTST_PSI_QUEUE_CNT 4 /* every second insert a PSI, so it takes 4 secs to circle; hopefully its transfer won't be delay that far */

/***********************************
 *   TS user data all pass
 */
#define BTST_TS_USER_DATA_ALL           (unsigned)(-1)
#define BTST_MAX_MESSAGE_FILTERS        20

#ifdef NEXUS_NUM_DSP_VIDEO_ENCODERS
#define BTST_INPUT_MAX_WIDTH 416
#define BTST_INPUT_MAX_HEIGHT 224
#endif

BDBG_MODULE(transcode_ts);
BDBG_FILE_MODULE(userdataCb);
BDBG_FILE_MODULE(contcounter);

#if NEXUS_HAS_VIDEO_ENCODER && NEXUS_HAS_STREAM_MUX
typedef struct EncodeSettings {
	char                    fname[256];
	NEXUS_VideoFormat       displayFormat;
	NEXUS_VideoFrameRate    encoderFrameRate;
	int                     encoderBitrate;
	int                     encoderGopStructureFramesP;
	int                     encoderGopStructureFramesB;
	NEXUS_VideoCodec        encoderVideoCodec;
	NEXUS_VideoCodecProfile encoderProfile;
	NEXUS_VideoCodecLevel   encoderLevel;
	NEXUS_DisplayCustomFormatSettings customFormatSettings;
	bool                    bCustom;
	bool                    bAudioEncode;
	NEXUS_AudioCodec        encoderAudioCodec;

	bool                    enableFieldPairing;

	/* 0 to use default 750ms rate buffer delay; allow user to adjust it to lower encode delay at cost of quality reduction! */
	unsigned                rateBufferDelay;

	/* to allow 23.976p passthru; allow user to configure minimum framerate to achieve lower delay!
	 * Note: lower minimum framerate means longer encode delay */
	NEXUS_VideoFrameRate    inputFrameRateMin;

	/* to allow 15 ~ 60p dynamic frame rate coding; allow user to config higher minimum frame rate for lower delay! */
	NEXUS_VideoFrameRate    outputFrameRateMin;

	/* max encode size allows 1080p encode; allow user to choose lower max resolution for lower encode delay */
	unsigned                inputMaxWidth;
	unsigned                inputMaxHeight;
}  EncodeSettings;

typedef struct InputSettings{
	int                    resource;
	char                   fname[256];
	NEXUS_TransportType    eStreamType;
	NEXUS_VideoCodec       eVideoCodec;
	NEXUS_AudioCodec       eAudioCodec;
	NEXUS_AudioMultichannelFormat eMultiChanFmt;
	bool                   bPcmAudio;
	unsigned               iVideoPid;
	unsigned               iAudioPid;
	unsigned               iSecondaryPid;
	unsigned               iPcrPid;
	unsigned               pmtPid;
	unsigned               userDataPid[NEXUS_MAX_MUX_PIDS];
	unsigned               remapUserDataPid[NEXUS_MAX_MUX_PIDS];
	size_t                 numUserDataPids;
	int                    freq;
#if  NEXUS_HAS_FRONTEND
	NEXUS_FrontendQamMode  qamMode;
#endif
	bool                   bTsUserDataInput;
	bool                   bAudioInput;
	bool                   bConfig;
} InputSettings;

typedef struct psi_message_t {
    unsigned short pid;
    NEXUS_MessageHandle message;
    NEXUS_PidChannelHandle pidChannel;
    bool done;
} psi_message_t;

typedef struct TranscodeContext {
	/* gfx */
	NEXUS_SurfaceHandle        surface;
	NEXUS_Graphics2DHandle     gfx;
	BKNI_EventHandle           checkpointEvent, spaceAvailableEvent;

	/* xcoder handles */
	NEXUS_VideoEncoderHandle   videoEncoder;
	NEXUS_DisplayHandle        displayTranscode;
	NEXUS_VideoWindowHandle    windowTranscode;
	NEXUS_PlaypumpHandle       playpumpTranscodeVideo;
	NEXUS_PidChannelHandle     pidChannelTranscodeVideo;
	NEXUS_PlaypumpHandle       playpumpTranscodeAudio;
	NEXUS_PidChannelHandle     pidChannelTranscodeAudio;
    NEXUS_AudioMixerHandle     audioMixer;
	NEXUS_AudioEncoderHandle   audioEncoder;
	NEXUS_AudioMuxOutputHandle audioMuxOutput;
#if USE_DDRE
	NEXUS_DolbyDigitalReencodeHandle ddre;
	NEXUS_DolbyVolume258Handle dv258;
#endif
	NEXUS_StreamMuxHandle      streamMux;
	BKNI_EventHandle           finishEvent;
	NEXUS_PlaypumpHandle       playpumpTranscodePcr;
	NEXUS_StcChannelHandle     stcChannelTranscode;

	/* xcoder mux/record handles */
	NEXUS_FifoRecordHandle     fifoFile;
	NEXUS_FileRecordHandle     fileTranscode;
	NEXUS_RecpumpHandle        recpump;
	NEXUS_RecordHandle         record;
	NEXUS_PidChannelHandle     pidChannelTranscodePcr;
	NEXUS_PidChannelHandle     pidChannelTranscodePat;
	NEXUS_PidChannelHandle     pidChannelTranscodePmt;

	/* source local decode/display handles */
	NEXUS_StcChannelHandle     stcVideoChannel;
	NEXUS_StcChannelHandle     stcAudioChannel;
	NEXUS_DisplayHandle        display;
	NEXUS_VideoWindowHandle    window;
	
#if NEXUS_HAS_HDMI_INPUT
	NEXUS_HdmiInputHandle      hdmiInput;
#endif
	NEXUS_FilePlayHandle       file;
	NEXUS_PlaypumpHandle       playpump;
	NEXUS_PlaybackHandle       playback;
	NEXUS_PidChannelHandle     videoPidChannel;
	NEXUS_VideoDecoderHandle   videoDecoder;
	NEXUS_PidChannelHandle     audioPidChannel, secondaryPidChannel;
	NEXUS_AudioDecoderHandle   audioDecoder, secondaryDecoder;
#if BTST_ENABLE_AV_SYNC
	NEXUS_SyncChannelHandle    syncChannel;
#endif
	NEXUS_VideoDecoderStartSettings vidProgram;
	NEXUS_AudioDecoderStartSettings audProgram, secondaryProgram;

#if BTST_ENABLE_XCODE_LOOPBACK
	/* xcode loopback player context */
	NEXUS_FilePlayHandle       filePlayLoopback;
	NEXUS_PlaybackHandle       playbackLoopback;
	NEXUS_VideoDecoderHandle   videoDecoderLoopback;
	NEXUS_StcChannelHandle     stcChannelLoopback;
	NEXUS_AudioDecoderHandle   audioDecoderLoopback;

	NEXUS_PidChannelHandle     videoPidChannelLoopback;
	NEXUS_DisplayHandle        displayLoopback;
	NEXUS_VideoWindowHandle    windowLoopback;
	NEXUS_PidChannelHandle     audioPidChannelLoopback;

	NEXUS_PlaypumpHandle       playpumpLoopback;
#if BTST_ENABLE_AV_SYNC
	NEXUS_SyncChannelHandle    syncChannelLoopback;
#endif
#endif
	/* video encoder settings */
	EncodeSettings             encodeSettings;
	NEXUS_VideoEncoderStartSettings vidEncoderStartSettings;
	NEXUS_AudioMuxOutputStartSettings audMuxStartSettings;
	NEXUS_StreamMuxStartSettings muxConfig;
	bool                       bEncodeCCUserData;

	/* input settings */
	InputSettings              inputSettings;

	NEXUS_FrontendHandle       frontend;
	char                       indexfname[256];

	/* PSI system data */
	void                     *pat[BTST_PSI_QUEUE_CNT];
	void                     *pmt[BTST_PSI_QUEUE_CNT];
	unsigned                  ccValue;
	NEXUS_StreamMuxSystemData psi[2];
	B_MutexHandle             mutexSystemdata;
	B_SchedulerHandle         schedulerSystemdata;
	B_SchedulerTimerId        systemdataTimer;
	B_ThreadHandle            schedulerThread;
	bool                      systemdataTimerIsStarted;

	NEXUS_MessageHandle       userDataMessage[NEXUS_MAX_MUX_PIDS];
	NEXUS_PidChannelHandle    pidChannelUserData[NEXUS_MAX_MUX_PIDS];
	NEXUS_PidChannelHandle    pidChannelTranscodeUserData[NEXUS_MAX_MUX_PIDS];
	bool                      bUserDataStreamValid[NEXUS_MAX_MUX_PIDS];
	int                       userDataDescLen[NEXUS_MAX_MUX_PIDS];
	uint8_t                   userDataDescriptors[NEXUS_MAX_MUX_PIDS][188];
	TS_PMT_stream             userDataStream[NEXUS_MAX_MUX_PIDS];
	bool                      bRemapUserDataPid;
	psi_message_t             psi_message[BTST_MAX_MESSAGE_FILTERS];

	/* xcode system settings */
	bool                      bNonRealTime;
	bool                      bLowDelay;
	bool                      bImmediateStop;
	bool                      bAdaptiveLatencyStart;
	bool                      bCustomizeDelay;
	B_EventHandle             eofEvent;
	B_ThreadHandle            nrtEofHandle;
	B_MutexHandle             mutexStarted;

	int                       contextId;
	bool                      bStarted;

	bool                      bNoStopDecode;
} TranscodeContext;

/* global context 
 * function level context in this */
static TranscodeContext xcodeContext[NEXUS_NUM_VIDEO_ENCODERS];

static NEXUS_PlatformConfiguration g_platformConfig;
static char g_keyReturn ='\0';
static int g_selectedXcodeContextId = 0;
static int g_activeXcodeCount = 0;
static bool g_bScriptMode = false; /* script mode (mutual exclusive with interactive mode) */
static uint32_t g_scriptModeSleepTime = 0;/* sleep time in seconds */
static bool g_bFifo = false; /* FIFO file or complete file record */
static bool g_bNonRealTimeWrap = false;
static bool g_bAutoQuit = false;
static bool g_bEncodeCC = false;
static bool g_bTsUserData = false;
static bool g_bNoDspMixer = false;
static bool g_bSecondAudio = false;
static bool g_bMultiChanAudio = false;
static unsigned g_msp = 50;
static unsigned g_rsp = 50;
static unsigned g_nrtRate = 32;
static NEXUS_VideoEncoderType g_eVideoEncoderType = NEXUS_VideoEncoderType_eMulti;
static B_EventHandle g_eotEvent = NULL;/* end of test event */

#ifdef NEXUS_NUM_DSP_VIDEO_ENCODERS
static int g_encodeDisplayId[NEXUS_NUM_VIDEO_ENCODERS] = {
    NEXUS_ENCODER_DISPLAY_IDX
};
#else
static int g_encodeDisplayId[NEXUS_NUM_VIDEO_ENCODERS] = {
	NEXUS_ENCODER0_DISPLAY_IDX,
	NEXUS_ENCODER1_DISPLAY_IDX,
#ifdef NEXUS_ENCODER2_DISPLAY_IDX
	NEXUS_ENCODER2_DISPLAY_IDX,
#endif
#ifdef NEXUS_ENCODER3_DISPLAY_IDX
	NEXUS_ENCODER3_DISPLAY_IDX,
#endif
};
#endif
#if BTST_ENABLE_XCODE_LOOPBACK
static int g_loopbackXcodeId = 0;
static bool g_loopbackStarted = true;
#endif

static void config_xcoder_context (TranscodeContext *pContext );
static int open_transcode(TranscodeContext *pContext );
static void close_transcode(TranscodeContext *pContext );
static void start_transcode(TranscodeContext *pContext );
static void stop_transcode(TranscodeContext *pContext );
static void bringup_transcode(TranscodeContext *pContext );
static void shutdown_transcode(TranscodeContext *pContext );
static void xcode_start_systemdata( TranscodeContext  *pContext );
static void xcode_stop_systemdata( TranscodeContext  *pContext );

/* xcode window PQ setting */
static void window_dnr( 
	NEXUS_VideoWindowHandle  hWindow,
	bool bCustom )
{
	NEXUS_VideoWindowDnrSettings windowDnrSettings;

	NEXUS_VideoWindow_GetDnrSettings(hWindow, &windowDnrSettings);
	if(bCustom)
	{
		printf("DNR settings:\n");
		printf("MNR mode: (%d) Disable; (%d) Bypass; (%d) Enable; ",
			NEXUS_VideoWindowFilterMode_eDisable, NEXUS_VideoWindowFilterMode_eBypass, NEXUS_VideoWindowFilterMode_eEnable);
		scanf("%d", (int*)&windowDnrSettings.mnr.mode);
		if(windowDnrSettings.mnr.mode == NEXUS_VideoWindowFilterMode_eEnable)
		{
			printf("MNR level (valid range -100 ... 2^31; for filtering, adjQp = Qp * (level + 100) / 100: ");
			scanf("%d", (int*)&windowDnrSettings.mnr.level);
		}
		printf("BNR mode: (%d) Disable; (%d) Bypass; (%d) Enable; ",
			NEXUS_VideoWindowFilterMode_eDisable, NEXUS_VideoWindowFilterMode_eBypass, NEXUS_VideoWindowFilterMode_eEnable);
		scanf("%d", (int*)&windowDnrSettings.bnr.mode);
		if(windowDnrSettings.bnr.mode == NEXUS_VideoWindowFilterMode_eEnable)
		{
			printf("BNR level (valid range -100 ... 2^31): ");
			scanf("%d", (int*)&windowDnrSettings.bnr.level);
		}
		printf("DCR mode: (%d) Disable; (%d) Bypass; (%d) Enable; ",
			NEXUS_VideoWindowFilterMode_eDisable, NEXUS_VideoWindowFilterMode_eBypass, NEXUS_VideoWindowFilterMode_eEnable);
		scanf("%d", (int*)&windowDnrSettings.dcr.mode);
		if(windowDnrSettings.dcr.mode == NEXUS_VideoWindowFilterMode_eEnable)
		{
			printf("DCR level (valid range -100 ... 2^31): ");
			scanf("%d", (int*)&windowDnrSettings.dcr.level);
		}
		if((windowDnrSettings.dcr.mode == NEXUS_VideoWindowFilterMode_eEnable) ||
		   (windowDnrSettings.bnr.mode == NEXUS_VideoWindowFilterMode_eEnable) ||
		   (windowDnrSettings.mnr.mode == NEXUS_VideoWindowFilterMode_eEnable))
		{
			printf("User Qp (default 0 means calculated from decoder source, non-zero is user override): ");
			scanf("%u", &windowDnrSettings.qp);
		}
	}
	else /* default */
	{
		windowDnrSettings.mnr.mode = NEXUS_VideoWindowFilterMode_eEnable;
		windowDnrSettings.bnr.mode = NEXUS_VideoWindowFilterMode_eEnable;
		windowDnrSettings.dcr.mode = NEXUS_VideoWindowFilterMode_eEnable;
		windowDnrSettings.mnr.level = 0;
		windowDnrSettings.bnr.level = 0;
		windowDnrSettings.dcr.level = 0;
		windowDnrSettings.qp = 0;
	}
	NEXUS_VideoWindow_SetDnrSettings(hWindow, &windowDnrSettings);
}

static void window_mad( 
	NEXUS_VideoWindowHandle  hWindow,
	bool bCustom,
	bool bEnable,
	bool bLowDelay)
{
	int choice;
	NEXUS_VideoWindowMadSettings windowMadSettings;
	
	NEXUS_VideoWindow_GetMadSettings(hWindow, &windowMadSettings);

	windowMadSettings.deinterlace = bEnable;
	if(bCustom)
	{
		if(windowMadSettings.deinterlace)
		{
			printf("3:2 reverse pulldown? (0) Disable; (1) Enable; ");
			scanf("%d", &choice); windowMadSettings.enable32Pulldown = choice;
			printf("2:2 reverse pulldown? (0) Disable; (1) Enable; ");
			scanf("%d", &choice);windowMadSettings.enable22Pulldown = choice;
			printf("Game Mode: (%d) Off (2-field delay, best quality); (%d) 1-field delay; (%d) 0-field delay; ",
				NEXUS_VideoWindowGameMode_eOff, NEXUS_VideoWindowGameMode_e4Fields_1Delay, NEXUS_VideoWindowGameMode_e4Fields_0Delay);
			scanf("%d", (int*)&windowMadSettings.gameMode);
		}
	}
	else /* default */
	{
		windowMadSettings.enable32Pulldown = true;
		windowMadSettings.enable22Pulldown = true;
		windowMadSettings.gameMode = bLowDelay?NEXUS_VideoWindowGameMode_e4Fields_0Delay : NEXUS_VideoWindowGameMode_eOff;
	}

	NEXUS_VideoWindow_SetMadSettings(hWindow, &windowMadSettings);
}


static void
transcoderFinishCallback(void *context, int param)
{
	BKNI_EventHandle finishEvent = (BKNI_EventHandle)context;

	BSTD_UNUSED(param);
	BDBG_WRN(("Finish callback invoked, now stop the stream mux."));
	BKNI_SetEvent(finishEvent);
}
#include "nexus_video_decoder_userdata.h"
#include "budp_dccparse.h"
enum BTST_P_DecoderId {
    BTST_P_DecoderId_eLoopback,
    BTST_P_DecoderId_eSource0,
    BTST_P_DecoderId_eSource1,
    BTST_P_DecoderId_eMax
} BTST_P_DecoderId;

static const char *g_userDataDecoderName[] = {
    "loopback",
    "source0",
    "source1"
};
typedef struct BTST_P_DecoderContext {
    struct {
        FILE *fLog;
        bool  bFilterNull;
        bool  bInit;
    } output608, output708;
    NEXUS_VideoDecoderHandle   videoDecoder;
    NEXUS_VideoCodec           codec;
    BUDP_DCCparse_ccdata       ccData[BTST_MAX_CC_DATA_COUNT];
    BAVC_USERDATA_info         userData;
} BTST_P_DecoderContext;
static BTST_P_DecoderContext g_decoderContext[BTST_P_DecoderId_eMax];

static void userdataCallback(void *context, int param)
{
    NEXUS_Error rc;
    BTST_P_DecoderContext *pContext = (BTST_P_DecoderContext*)context;
    NEXUS_VideoDecoderHandle videoDecoder = (NEXUS_VideoDecoderHandle)pContext->videoDecoder;

    BDBG_ASSERT(param < BTST_P_DecoderId_eMax);
    BDBG_MODULE_MSG(userdataCb, ("decoder %d user data callback!",param));

    /* It's crucial that the callback process all userdata packets in the buffer.
    The app cannot assume that it can process one packet per callback. */
    while (1) {
        unsigned size;
        uint8_t *buffer;
        uint32_t offset = 0;
        NEXUS_UserDataHeader *pHeader;
        BAVC_USERDATA_info   *toBeParsed = &pContext->userData;

        /* get the header */
        rc = NEXUS_VideoDecoder_GetUserDataBuffer(videoDecoder, (void**)&buffer, &size);
        BDBG_ASSERT(!rc);

        /* The app can assume that userdata will be placed into the buffer in whole packets.
        The termination condition for the loop is that there are no more bytes in the buffer. */
        if (!size) break;

        /* guaranteed whole header + payload */
        pHeader = (NEXUS_UserDataHeader *)buffer;
        BDBG_ASSERT(size >= pHeader->blockSize);
        BDBG_ASSERT(pHeader->blockSize == sizeof(*pHeader) + pHeader->payloadSize);
        BDBG_ASSERT(pHeader->blockSize % 4 == 0); /* 32 bit aligned */

        /* 1) resume the to-be-parsed user data structure as input of UDP; */
        toBeParsed->pUserDataBuffer     = (void*)((uint8_t*)buffer + sizeof(NEXUS_UserDataHeader));
        toBeParsed->ui32UserDataBufSize = pHeader->payloadSize;
        toBeParsed->eUserDataType = pHeader->type;
        toBeParsed->eSourcePolarity = pHeader->polarity;
        toBeParsed->bTopFieldFirst = pHeader->topFieldFirst;
        toBeParsed->bRepeatFirstField = pHeader->repeatFirstField;
        toBeParsed->bPTSValid = pHeader->ptsValid;
        toBeParsed->ui32PTS = pHeader->pts;
        toBeParsed->ePicCodingType = pHeader->pictureCoding;
        BKNI_Memcpy(toBeParsed->ui32PicCodExt, pHeader->pictureCodingExtension, sizeof(toBeParsed->ui32PicCodExt));

        /* 2) call UDP to parse cc data; */
        while (offset < toBeParsed->ui32UserDataBufSize) {
            BERR_Code rc;
            unsigned i;
            size_t bytesParsed = 0;
            uint8_t ccCount = 0;
            BUDP_DCCparse_ccdata *ccData = pContext->ccData;

            if (pContext->codec == NEXUS_VideoCodec_eH264) {
                rc = BUDP_DCCparse_SEI_isr(toBeParsed, offset, &bytesParsed, &ccCount, ccData);
            }
            else {
                rc = BUDP_DCCparse_isr(toBeParsed, offset, &bytesParsed, &ccCount, ccData);
            }

            for( i = 0; i < ccCount; i++ )
            {
	            FILE *fLog = NULL;
	            bool bHeader = false;

	            if ( true == ccData[i].bIsAnalog )
	            {
	               if ( ( ( true == pContext->output608.bFilterNull ) && ( 0 != (ccData[i].cc_data_1 & 0x7F) ) )
	                    || ( ( false == pContext->output608.bFilterNull ) ) )
	               {
	                  fLog = pContext->output608.fLog;
	                  if ( pContext->output608.bInit )
	                  {
	                     bHeader = true;
	                     pContext->output608.bInit = false;
	                  }
	               }
	            }
	            else
	            {
	               if ( ( ( true == pContext->output708.bFilterNull ) && ( 0 != (ccData[i].cc_data_1) ) )
	                    || ( ( false == pContext->output708.bFilterNull ) ) )
	               {
	                  fLog = pContext->output708.fLog;
	                  if ( pContext->output708.bInit )
	                  {
	                     bHeader = true;
	                     pContext->output708.bInit = false;
	                  }
	               }
	            }

	            if ( NULL != fLog)
	            {
	               if ( true == bHeader )
	               {
	                  fprintf( fLog, "size,ccCnt,PTS,Polarity,Format,CC Valid,CC Priority,CC Line Offset,CC Type,CC Data[0],CC Data[1]\n");
	               }

	               fprintf(fLog, "%u,%u,%u,%d,%d,%d,%d,%d,%d,%d,%d\n",
	                  pHeader->payloadSize, ccCount,
	                  pHeader->pts,
	                  ccData[i].polarity,
	                  ccData[i].format,
	                  ccData[i].cc_valid,
	                  ccData[i].cc_priority,
	                  ccData[i].line_offset,
	                  ccData[i].seq.cc_type,
	                  ccData[i].cc_data_1,
	                  ccData[i].cc_data_2
	                  );
	            }
            }
            offset += bytesParsed;
        }

        NEXUS_VideoDecoder_UserDataReadComplete(videoDecoder, pHeader->blockSize);
    }
}

#if NEXUS_HAS_FRONTEND
static void lock_callback(void *context, int param)
{
	NEXUS_FrontendHandle frontend = (NEXUS_FrontendHandle)context;
	NEXUS_FrontendQamStatus qamStatus;

	BSTD_UNUSED(param);

	fprintf(stderr, "Lock callback, frontend 0x%08x\n", (unsigned)frontend);

	NEXUS_Frontend_GetQamAsyncStatus(frontend, &qamStatus);
	fprintf(stderr, "QAM Lock callback, frontend 0x%08x - lock status %d, %d\n", (unsigned)frontend,
		qamStatus.fecLock, qamStatus.receiverLock);
}
#endif

static void sourcePlayerUsage(void)
{
    printf("? - this help\n"
		   "a - frame Advance\n"
		   "f - Fast Forward\n"
		   "l - Slow Motion\n"
		   "p - Play\n"
		   "r - Fast Rewind\n"
		   "s - pauSe\n"
		   "w - Wait 30msec\n"
		   "+ - Jump forward 5 seconds\n"
		   "- - Jump backward 5 seconds\n"
		   "0 - Jump to the beginning\n"
		   "q - Go back to upper level menu\n"
		   );
}

static void jump_to_beginning(NEXUS_PlaybackHandle playback)
{
    unsigned pos;
    NEXUS_Error rc;
    NEXUS_PlaybackStatus playbackStatus;

    rc = NEXUS_Playback_GetStatus(playback, &playbackStatus);
    BDBG_ASSERT(!rc);
    pos = playbackStatus.first;
    printf("Jump to beginning %u, %u...%u\n", pos, (unsigned)playbackStatus.first, (unsigned)playbackStatus.last);
    (void)NEXUS_Playback_Seek(playback, pos);
}

static void sourcePlayer( void *context )
{
    int rate = NEXUS_NORMAL_PLAY_SPEED;
    TranscodeContext  *pContext = (TranscodeContext  *)context;

    if(pContext->inputSettings.resource != BTST_RESOURCE_FILE || !pContext->bStarted) return;
    for(;;) {
        NEXUS_PlaybackStatus playbackStatus;
        NEXUS_VideoDecoderStatus videoStatus;
        NEXUS_AudioDecoderStatus audioStatus;
        uint32_t stc;
        NEXUS_FilePosition first, last;
        char cmd[16];
        unsigned i;
        bool quit=false;
        NEXUS_PlaybackTrickModeSettings trickmode_settings;


        printf("Xcoder%d source player CMD:>", pContext->contextId);
        sourcePlayerUsage();
        if(fgets(cmd, sizeof(cmd)-1, stdin)==NULL) {
            break;
        }
        for(i=0;cmd[i]!='\0';i++) {
            switch(cmd[i]) {
            case '?':
                sourcePlayerUsage();
                break;
            case 'a':
                printf( "frame advance\n" );
                NEXUS_Playback_FrameAdvance(pContext->playback, rate >= 0);
                break;
            case 's':
                printf( "pause\n" );
                rate = 0;
                NEXUS_Playback_Pause(pContext->playback);
                break;
            case 'p':
                printf( "play\n" );
                NEXUS_Playback_Play(pContext->playback);
                rate = NEXUS_NORMAL_PLAY_SPEED;
                break;
            case 'f':
                NEXUS_Playback_GetDefaultTrickModeSettings( &trickmode_settings );
                if(rate<=0) {
                    rate = NEXUS_NORMAL_PLAY_SPEED;
                } else
                rate *=2;
                trickmode_settings.rate = rate;
                printf( "fast forward %d\n", trickmode_settings.rate );
                NEXUS_Playback_TrickMode( pContext->playback, &trickmode_settings );
                break;
            case 'l':
                NEXUS_Playback_GetDefaultTrickModeSettings( &trickmode_settings );
                rate /=2;
                trickmode_settings.rate = rate;
                printf( "slow down %d\n", trickmode_settings.rate );
                NEXUS_Playback_TrickMode( pContext->playback, &trickmode_settings );
                break;
            case 'q':
                quit = true;
                break;
            case 'r':
                NEXUS_Playback_GetDefaultTrickModeSettings( &trickmode_settings );
                if(rate>=0) {
                    rate = -NEXUS_NORMAL_PLAY_SPEED;
                } else
                rate *=2;
                trickmode_settings.rate = rate;
                printf( "rewind %d\n", trickmode_settings.rate );
                NEXUS_Playback_TrickMode( pContext->playback, &trickmode_settings );
                break;
            case 'w':
                BKNI_Sleep(30);
                break;
            case '-':
                BDBG_ASSERT(!NEXUS_Playback_GetStatus(pContext->playback, &playbackStatus));
                if (playbackStatus.position >= 5*1000) {
                    playbackStatus.position -= 5*1000;
                    /* it's normal for a Seek to fail if it goes past the beginning */
                    printf("Jump to %u, %u...%u\n", (unsigned)playbackStatus.position, (unsigned)playbackStatus.first, (unsigned)playbackStatus.last);
                    (void)NEXUS_Playback_Seek(pContext->playback, playbackStatus.position);
                }
                break;
            case '+':
                BDBG_ASSERT(!NEXUS_Playback_GetStatus(pContext->playback, &playbackStatus));
                /* it's normal for a Seek to fail if it goes past the end */
                playbackStatus.position += 5*1000;
                printf("Jump to %u, %u...%u\n", (unsigned)playbackStatus.position, (unsigned)playbackStatus.first, (unsigned)playbackStatus.last);
                (void)NEXUS_Playback_Seek(pContext->playback, playbackStatus.position);
                break;
            case '0':
                jump_to_beginning(pContext->playback);
                break;
            case '\n':
            case ' ':
                BDBG_ASSERT(!NEXUS_Playback_GetStatus(pContext->playback, &playbackStatus));
                NEXUS_StcChannel_GetStc(pContext->stcVideoChannel, &stc);
                NEXUS_VideoDecoder_GetStatus(pContext->videoDecoder, &videoStatus);
                if(pContext->audioDecoder) NEXUS_AudioDecoder_GetStatus(pContext->audioDecoder, &audioStatus);
                NEXUS_FilePlay_GetBounds(pContext->file, &first, &last);

                printf("%d file %u:%u, playback %u%% position=%u.%03u sec, video %u%% %ux%u (%#x:%#x:%d)",
                    (int)playbackStatus.trickModeSettings.rate,
                    (unsigned)first.mpegFileOffset,  (unsigned)last.mpegFileOffset,
                    playbackStatus.fifoSize ? (playbackStatus.fifoDepth * 100) / playbackStatus.fifoSize : 0,
                    (unsigned)playbackStatus.position/1000, (unsigned)playbackStatus.position%1000,
                    videoStatus.fifoSize ? (videoStatus.fifoDepth * 100) / videoStatus.fifoSize : 0,
                    videoStatus.source.width, videoStatus.source.height, videoStatus.pts, stc, videoStatus.ptsStcDifference
                    );
                if(pContext->audioDecoder) printf(", audio %u%% %uHz (%#x:%#x:%d)\n",
                    audioStatus.fifoSize ? (audioStatus.fifoDepth * 100) / audioStatus.fifoSize : 0,
                    audioStatus.sampleRate, audioStatus.pts, stc, audioStatus.ptsStcDifference);
                else printf("\n");
                break;
            default:
                break;
            }
        }
        if(quit)  {
            break;
        }

    }
}

#if BTST_ENABLE_XCODE_LOOPBACK
static void hotplug_callback(void *pParam, int iParam)
{
    NEXUS_HdmiOutputStatus status;
    NEXUS_HdmiOutputHandle hdmi = pParam;
    NEXUS_DisplayHandle display = (NEXUS_DisplayHandle)iParam;

    NEXUS_HdmiOutput_GetStatus(hdmi, &status);
    fprintf(stderr, "HDMI hotplug event: %s\n", status.connected?"connected":"not connected");

    /* the app can choose to switch to the preferred format, but it's not required. */
    if ( status.connected )
    {
        NEXUS_DisplaySettings displaySettings;
        NEXUS_Display_GetSettings(display, &displaySettings);
        if ( !status.videoFormatSupported[displaySettings.format] )
        {
            fprintf(stderr, "\nCurrent format not supported by attached monitor. Switching to preferred format %d\n", status.preferredVideoFormat);
            displaySettings.format = status.preferredVideoFormat;
            NEXUS_Display_SetSettings(display, &displaySettings);
        }
    }
}

void xcode_loopback_setup( TranscodeContext  *pContext )
{
	NEXUS_HdmiOutputSettings hdmiOutputSettings;
	NEXUS_PlaybackSettings playbackSettings;
	NEXUS_PlaybackPidChannelSettings playbackPidSettings;
	NEXUS_VideoDecoderSettings vidDecodeSettings;
	NEXUS_VideoDecoderStartSettings videoProgram;
	NEXUS_AudioDecoderStartSettings audioProgram;
	NEXUS_StcChannelSettings stcSettings;
#if BTST_ENABLE_AV_SYNC
	NEXUS_SyncChannelSettings syncChannelSettings;
#endif
	NEXUS_DisplaySettings displaySettings;
    NEXUS_VideoWindowScalerSettings scalerSettings;
	EncodeSettings  *pEncodeSettings = &pContext->encodeSettings;
	InputSettings   *pInputSettings = &pContext->inputSettings; 
	int cnt = 0;

	BDBG_MSG(("To start the loopback player for transcoder%d...", pContext->contextId));
	NEXUS_StcChannel_GetDefaultSettings(NEXUS_ANY_ID, &stcSettings);
	stcSettings.timebase = NEXUS_Timebase_e0;
	stcSettings.mode = NEXUS_StcChannelMode_eAuto;
	pContext->stcChannelLoopback = NEXUS_StcChannel_Open(NEXUS_ANY_ID, &stcSettings);
	BDBG_MSG(("Transcoder%d opened Loopback STC [%p].", pContext->contextId, pContext->stcChannelLoopback));
#if BTST_ENABLE_AV_SYNC
	/* create a sync channel for xcoder loopback decode */
	NEXUS_SyncChannel_GetDefaultSettings(&syncChannelSettings);
	pContext->syncChannelLoopback = NEXUS_SyncChannel_Create(&syncChannelSettings);
#endif

	printf("start xcode loopback display on HDMI and Component outputs...\n");
	pContext->videoDecoderLoopback = NEXUS_VideoDecoder_Open(BTST_LOOPBACK_VID_DECODE_IDX, NULL); /* take default capabilities */
	BDBG_MSG(("Transcoder%d opened Loopback video decoder%d [%p].", pContext->contextId, BTST_LOOPBACK_VID_DECODE_IDX, pContext->videoDecoderLoopback));
	NEXUS_VideoDecoder_GetSettings(pContext->videoDecoderLoopback, &vidDecodeSettings);
	BKNI_Memset(vidDecodeSettings.supportedCodecs, 0, sizeof(vidDecodeSettings.supportedCodecs));
	vidDecodeSettings.supportedCodecs[0] = NEXUS_VideoCodec_eH264; /* it's for regular HD decode heap allocation; it covers mpeg2/h264/mpeg4 HD size */
	if(pContext->bEncodeCCUserData) {/* to log loopback xcoded user data */
		char fname[256];
		vidDecodeSettings.userDataEnabled = true;
		vidDecodeSettings.appUserDataReady.callback = userdataCallback;
		g_decoderContext[BTST_P_DecoderId_eLoopback].videoDecoder = pContext->videoDecoderLoopback;
		g_decoderContext[BTST_P_DecoderId_eLoopback].codec = pEncodeSettings->encoderVideoCodec;
		sprintf(fname, "userdata_%s_608.csv", g_userDataDecoderName[BTST_P_DecoderId_eLoopback]);
		g_decoderContext[BTST_P_DecoderId_eLoopback].output608.fLog        = fopen(fname, "wb");
		g_decoderContext[BTST_P_DecoderId_eLoopback].output608.bInit       = true;
		g_decoderContext[BTST_P_DecoderId_eLoopback].output608.bFilterNull = true;
		sprintf(fname, "userdata_%s_708.csv", g_userDataDecoderName[BTST_P_DecoderId_eLoopback]);
		g_decoderContext[BTST_P_DecoderId_eLoopback].output708.fLog = fopen(fname, "wb");
		g_decoderContext[BTST_P_DecoderId_eLoopback].output708.bInit       = true;
		g_decoderContext[BTST_P_DecoderId_eLoopback].output708.bFilterNull = true;
		vidDecodeSettings.appUserDataReady.context  = &g_decoderContext[BTST_P_DecoderId_eLoopback];
		vidDecodeSettings.appUserDataReady.param    = BTST_P_DecoderId_eLoopback;/* loopback */
	}
	/* channel change mode: hold until first new picture */
	vidDecodeSettings.channelChangeMode = NEXUS_VideoDecoder_ChannelChangeMode_eHoldUntilFirstPicture;
	NEXUS_VideoDecoder_SetSettings(pContext->videoDecoderLoopback, &vidDecodeSettings);
	pContext->playpumpLoopback = NEXUS_Playpump_Open(BTST_LOOPBACK_PLAYPUMP_IDX, NULL);
	assert(pContext->playpumpLoopback);
	BDBG_MSG(("Transcoder%d opened Loopback playpump%d [%p].", pContext->contextId, BTST_LOOPBACK_PLAYPUMP_IDX, pContext->playpumpLoopback));
	pContext->playbackLoopback = NEXUS_Playback_Create();
	assert(pContext->playbackLoopback);

	if(g_bFifo) {
		pContext->filePlayLoopback = NEXUS_FifoPlay_Open(pEncodeSettings->fname, pContext->indexfname[0]? pContext->indexfname : NULL,
			pContext->fifoFile);
	}
	else {
		pContext->filePlayLoopback = NEXUS_FilePlay_OpenPosix(pEncodeSettings->fname, pContext->indexfname[0]? pContext->indexfname : NULL);
	}
	if (!pContext->filePlayLoopback) {
		fprintf(stderr, "can't open file: %s, index %s\n", pEncodeSettings->fname, pContext->indexfname);
		BDBG_ASSERT(0);
	}
	
	NEXUS_Playback_GetSettings(pContext->playbackLoopback, &playbackSettings);
	playbackSettings.playpump = pContext->playpumpLoopback;
	/* set a stream format, it could be any audio video transport type or file format, i.e NEXUS_TransportType_eMp4, NEXUS_TransportType_eAvi ... */
	playbackSettings.playpumpSettings.transportType = (NEXUS_TransportType)NEXUS_TransportType_eTs;
	playbackSettings.stcChannel = pContext->stcChannelLoopback; /* loopback channel  */
	playbackSettings.timeshifting = true; /* allow for timeshift the growing file  */
#if 0 /* not suitable for local playback due to STC jamming with 1st PTS which might underflow decoder buffer, but may be used for live streaming to reduce latency */
	playbackSettings.timeshiftingSettings.endOfStreamGap = 1800; /* may need end-to-end latency to match encoder A2P */
#endif
	playbackSettings.endOfStreamAction = NEXUS_PlaybackLoopMode_ePlay; /* when play hits the end, wait for record */
	NEXUS_Playback_SetSettings(pContext->playbackLoopback, &playbackSettings);

	/* Open the video pid channel */
	NEXUS_Playback_GetDefaultPidChannelSettings(&playbackPidSettings);
	playbackPidSettings.pidSettings.pidType = NEXUS_PidType_eVideo;
	playbackPidSettings.pidTypeSettings.video.codec = (NEXUS_VideoCodec)pEncodeSettings->encoderVideoCodec; /* must be told codec for correct handling */
	playbackPidSettings.pidTypeSettings.video.index = pContext->indexfname[0]? true : false;
	playbackPidSettings.pidTypeSettings.video.decoder = pContext->videoDecoderLoopback;
	pContext->videoPidChannelLoopback = NEXUS_Playback_OpenPidChannel(pContext->playbackLoopback, BTST_MUX_VIDEO_PID, &playbackPidSettings);
	
	/* Set up decoder Start structures now. We need to know the audio codec to properly set up
	the audio outputs. */
	NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram);
	videoProgram.codec = pEncodeSettings->encoderVideoCodec;
	videoProgram.pidChannel = pContext->videoPidChannelLoopback;
	videoProgram.stcChannel = pContext->stcChannelLoopback;

	/* Bring up loopback video display and outputs */
	NEXUS_Display_GetDefaultSettings(&displaySettings);
	displaySettings.timebase = NEXUS_Timebase_e0;
	displaySettings.displayType = NEXUS_DisplayType_eAuto;
	displaySettings.format = 
		(pEncodeSettings->encoderFrameRate == NEXUS_VideoFrameRate_e25 || 
		 pEncodeSettings->encoderFrameRate == NEXUS_VideoFrameRate_e50) ?
		NEXUS_VideoFormat_e720p50hz : NEXUS_VideoFormat_e720p;
	pContext->displayLoopback = NEXUS_Display_Open(BTST_LOOPBACK_DISPLAY_IDX, &displaySettings);
	BDBG_MSG(("Transcoder%d opened Loopback display%d [%p].", pContext->contextId, BTST_LOOPBACK_DISPLAY_IDX, pContext->displayLoopback));
#if NEXUS_NUM_COMPONENT_OUTPUTS
	if(g_platformConfig.outputs.component[0]){
		NEXUS_Display_AddOutput(pContext->displayLoopback, NEXUS_ComponentOutput_GetConnector(g_platformConfig.outputs.component[0]));
	}
#endif
#if NEXUS_NUM_HDMI_OUTPUTS
		if(g_platformConfig.outputs.hdmi[0]){
			NEXUS_Display_AddOutput(pContext->displayLoopback, NEXUS_HdmiOutput_GetVideoConnector(g_platformConfig.outputs.hdmi[0]));
		}
#endif
    /* Install hotplug callback -- video only for now */
    NEXUS_HdmiOutput_GetSettings(g_platformConfig.outputs.hdmi[0], &hdmiOutputSettings);
    hdmiOutputSettings.hotplugCallback.callback = hotplug_callback;
    hdmiOutputSettings.hotplugCallback.context = g_platformConfig.outputs.hdmi[0];
    hdmiOutputSettings.hotplugCallback.param = (int)pContext->displayLoopback;
    NEXUS_HdmiOutput_SetSettings(g_platformConfig.outputs.hdmi[0], &hdmiOutputSettings);

	pContext->windowLoopback = NEXUS_VideoWindow_Open(pContext->displayLoopback, 0);

	NEXUS_VideoWindow_AddInput(pContext->windowLoopback, NEXUS_VideoDecoder_GetConnector(pContext->videoDecoderLoopback));

	/* set loopback vnet mode bias to avoid black frame transition during dynamic resolution change */
    NEXUS_VideoWindow_GetScalerSettings(pContext->windowLoopback, &scalerSettings);
    scalerSettings.bandwidthEquationParams.bias = NEXUS_ScalerCaptureBias_eScalerBeforeCapture;
    scalerSettings.bandwidthEquationParams.delta = 1*1000*1000;
    NEXUS_VideoWindow_SetScalerSettings(pContext->windowLoopback, &scalerSettings);

	/* enable window deinterlacer default setting: it'll grab MCVP first */
	window_mad(pContext->windowLoopback, false, true, false);

	/* if audio DAC0 for loopback player */
	if(pInputSettings->bAudioInput)
	{
		/* Open the audio loopback decoder */
		pContext->audioDecoderLoopback = NEXUS_AudioDecoder_Open(BTST_LOOPBACK_AUD_DECODE_IDX, NULL);
		BDBG_MSG(("Transcoder%d opened Loopback audio decoder%d [%p].", pContext->contextId, BTST_LOOPBACK_AUD_DECODE_IDX, pContext->audioDecoderLoopback));

		/* Open the audio pid channel */
		{
			NEXUS_Playback_GetDefaultPidChannelSettings(&playbackPidSettings);
			playbackPidSettings.pidSettings.pidType = NEXUS_PidType_eAudio;
			playbackPidSettings.pidTypeSettings.audio.primary = pContext->audioDecoderLoopback; 
			pContext->audioPidChannelLoopback = NEXUS_Playback_OpenPidChannel(pContext->playbackLoopback, BTST_MUX_AUDIO_PID, &playbackPidSettings);
		}

		/* Set up decoder Start structures now. We need to know the audio codec to properly set up
		the audio outputs. */
		NEXUS_AudioDecoder_GetDefaultStartSettings(&audioProgram);

		audioProgram.codec = pEncodeSettings->bAudioEncode?
			pEncodeSettings->encoderAudioCodec : pInputSettings->eAudioCodec;
		audioProgram.pidChannel = pContext->audioPidChannelLoopback;
		audioProgram.stcChannel = pContext->stcChannelLoopback;

		/* Connect audio decoders to outputs */
		NEXUS_AudioOutput_AddInput(
			NEXUS_AudioDac_GetConnector(g_platformConfig.outputs.audioDacs[0]),
			NEXUS_AudioDecoder_GetConnector(pContext->audioDecoderLoopback, NEXUS_AudioDecoderConnectorType_eStereo));

		/* add hdmi pcm audio output to loopback display */
		NEXUS_AudioOutput_AddInput(NEXUS_HdmiOutput_GetAudioConnector(g_platformConfig.outputs.hdmi[0]),
								   NEXUS_AudioDecoder_GetConnector(pContext->audioDecoderLoopback, NEXUS_AudioDecoderConnectorType_eStereo));

#if BTST_ENABLE_AV_SYNC
		/* connect sync channel before start decode */
		NEXUS_SyncChannel_GetSettings(pContext->syncChannelLoopback, &syncChannelSettings);
		syncChannelSettings.videoInput = NEXUS_VideoDecoder_GetConnector(pContext->videoDecoderLoopback);
		if(pInputSettings->bAudioInput)
		{
			syncChannelSettings.audioInput[0] = NEXUS_AudioDecoder_GetConnector(pContext->audioDecoderLoopback, NEXUS_AudioDecoderConnectorType_eStereo);
		}
	
		NEXUS_SyncChannel_SetSettings(pContext->syncChannelLoopback, &syncChannelSettings);
#endif
			
		/* start audio loopback decoder */
		NEXUS_AudioDecoder_Start(pContext->audioDecoderLoopback, &audioProgram);
	}
	
		/* Start video loopback decoder */
		NEXUS_VideoDecoder_Start(pContext->videoDecoderLoopback, &videoProgram);
	
	/* delay the loopback until A2P passed to avoid stutter since playback jams PTS to STC instead of locking PCR so the encode buffer
	   model is not enforced at loopback decoder plus the O_DIRECT timeshift buffer seems to have some latency. */
	BKNI_Sleep(3000);
	/* wait for timeshift file index to be available */
	while (pContext->indexfname[0] && cnt++ < 2000) {
	    NEXUS_RecordStatus status;
	    NEXUS_Record_GetStatus(pContext->record, &status);
	    if (status.picturesIndexed) {
	        BDBG_WRN(("%u pictures indexed\n", status.picturesIndexed));
	        break;
	    }
	    BKNI_Sleep(10);
	}
	if(cnt >= 2000) /* try 20 seconds */
	{
		fprintf(stderr, "**** Encoder stalls and cannot timeshift xcoded stream! Please debug! ****\n");
		return;
	}

	/* Linking loopback Playback to Record allows Playback to sleep until Record writes data. Avoids a busyloop near live. */
	BDBG_ASSERT(!NEXUS_Record_AddPlayback(pContext->record, pContext->playbackLoopback));

	/* Start playback */
	NEXUS_Playback_Start(pContext->playbackLoopback, pContext->filePlayLoopback, NULL);
}

void xcode_loopback_shutdown( TranscodeContext  *pContext )
{
#if BTST_ENABLE_AV_SYNC
	NEXUS_SyncChannelSettings syncChannelSettings;
#endif
	InputSettings   *pInputSettings = &pContext->inputSettings; 

	BDBG_MSG(("To shutdown the loopback player for transcoder%d...", pContext->contextId));
	NEXUS_Playback_Stop(pContext->playbackLoopback);
    NEXUS_Record_RemovePlayback(pContext->record, pContext->playbackLoopback);
	NEXUS_VideoDecoder_Stop(pContext->videoDecoderLoopback);
	/* disconnect sync channel after stop decoder */
	if(pInputSettings->bAudioInput)
	{
		NEXUS_AudioDecoder_Stop(pContext->audioDecoderLoopback);
#if BTST_ENABLE_AV_SYNC
		NEXUS_SyncChannel_GetSettings(pContext->syncChannelLoopback, &syncChannelSettings);
		syncChannelSettings.videoInput = NULL;
		syncChannelSettings.audioInput[0] = NULL;
		syncChannelSettings.audioInput[1] = NULL;
		NEXUS_SyncChannel_SetSettings(pContext->syncChannelLoopback, &syncChannelSettings);
#endif
		NEXUS_AudioOutput_RemoveAllInputs(NEXUS_AudioDac_GetConnector(g_platformConfig.outputs.audioDacs[0]));
		NEXUS_AudioInput_Shutdown(NEXUS_AudioDecoder_GetConnector(pContext->audioDecoderLoopback, NEXUS_AudioDecoderConnectorType_eStereo));
		NEXUS_AudioOutput_Shutdown(NEXUS_AudioDac_GetConnector(g_platformConfig.outputs.audioDacs[0]));
		NEXUS_AudioDecoder_Close(pContext->audioDecoderLoopback);
		NEXUS_Playback_ClosePidChannel(pContext->playbackLoopback, pContext->audioPidChannelLoopback);
	}
	NEXUS_Playback_ClosePidChannel(pContext->playbackLoopback, pContext->videoPidChannelLoopback);
	NEXUS_FilePlay_Close(pContext->filePlayLoopback);
	NEXUS_Playback_Destroy(pContext->playbackLoopback);
	NEXUS_Playpump_Close(pContext->playpumpLoopback);
	
	NEXUS_VideoWindow_RemoveInput(pContext->windowLoopback, NEXUS_VideoDecoder_GetConnector(pContext->videoDecoderLoopback));
	NEXUS_VideoInput_Shutdown(NEXUS_VideoDecoder_GetConnector(pContext->videoDecoderLoopback));
	NEXUS_VideoDecoder_Close(pContext->videoDecoderLoopback);
	NEXUS_VideoWindow_Close(pContext->windowLoopback);
#if NEXUS_NUM_HDMI_OUTPUTS
	NEXUS_Display_RemoveOutput(pContext->displayLoopback, NEXUS_HdmiOutput_GetVideoConnector(g_platformConfig.outputs.hdmi[0]));
	NEXUS_StopCallbacks(g_platformConfig.outputs.hdmi[0]);
#endif
	NEXUS_Display_Close(pContext->displayLoopback);
#if BTST_ENABLE_AV_SYNC
	if(pContext->syncChannelLoopback) {
		NEXUS_SyncChannel_Destroy(pContext->syncChannelLoopback);
		pContext->syncChannelLoopback = NULL;
	}
#endif

	NEXUS_StcChannel_Close(pContext->stcChannelLoopback);

	if(g_decoderContext[BTST_P_DecoderId_eLoopback].output608.fLog) {
		fclose(g_decoderContext[BTST_P_DecoderId_eLoopback].output608.fLog);
	}
	if(g_decoderContext[BTST_P_DecoderId_eLoopback].output708.fLog) {
		fclose(g_decoderContext[BTST_P_DecoderId_eLoopback].output708.fLog);
	}
}

static void loopbackUsage(void)
{
    printf("? - this help\n"
		   "a - loopback window Aspect ratio mode\n"
		   "c - Close selected transcoder loopback player\n"
		   "d - loopback Display format switch\n"
		   "f - Fast Forward\n"
		   "i - Select xcoder Id for loopback\n"
		   "l - Slow Motion\n"
		   "o - Open selected transcoder loopback player\n"
		   "p - Play\n"
		   "r - Fast Rewind\n"
		   "s - pauSe\n"
		   "w - Wait 30msec\n"
		   "z - resiZe loopback video window\n"
		   "+ - Jump forward 5 seconds\n"
		   "- - Jump backward 5 seconds\n"
		   "0 - Jump to the beginning\n"
		   "q - Go back to upper level menu\n"
		   );
}

static void loopbackPlayer( TranscodeContext  xcodeContext[] )
{
	int rate = NEXUS_NORMAL_PLAY_SPEED;
	TranscodeContext *pContext = &xcodeContext[g_loopbackXcodeId];
    for(;;) {
        NEXUS_PlaybackStatus playbackStatus;
        NEXUS_VideoDecoderStatus videoStatus;
        NEXUS_AudioDecoderStatus audioStatus;
        uint32_t stc;
        NEXUS_FilePosition first, last;
        char cmd[16];
        unsigned i;
        bool quit=false;
        int choice;
        NEXUS_PlaybackTrickModeSettings trickmode_settings;
        NEXUS_DisplaySettings displaySettings;
        NEXUS_VideoWindowSettings windowSettings;


        printf("Xcoder loopback player CMD:>");
        loopbackUsage();
        if(fgets(cmd, sizeof(cmd)-1, stdin)==NULL) {
            break;
        }
        for(i=0;cmd[i]!='\0';i++) {
            if(!g_loopbackStarted && (cmd[i]!='i' && cmd[i]!='o' && cmd[i]!='q'))
                continue;
            switch(cmd[i]) {
            case '?':
                loopbackUsage();
                break;
            case 's':
                printf( "pause\n" );
                rate = 0;
                NEXUS_Playback_Pause(pContext->playbackLoopback);
                break;
            case 'p':
                printf( "play\n" );
                rate = NEXUS_NORMAL_PLAY_SPEED;
                NEXUS_Playback_Play(pContext->playbackLoopback);
                break;
            case 'f':
                NEXUS_Playback_GetDefaultTrickModeSettings( &trickmode_settings );
                if(rate<=0) {
                    rate = NEXUS_NORMAL_PLAY_SPEED;
                } else
                rate *=2;
                trickmode_settings.rate = rate;
                printf( "fast forward %d\n", trickmode_settings.rate );
                NEXUS_Playback_TrickMode( pContext->playbackLoopback, &trickmode_settings );
                break;
            case 'l':
                NEXUS_Playback_GetDefaultTrickModeSettings( &trickmode_settings );
                rate /=2;
                trickmode_settings.rate = rate;
                printf( "slow down %d\n", trickmode_settings.rate );
                NEXUS_Playback_TrickMode( pContext->playbackLoopback, &trickmode_settings );
                break;
            case 'q':
                quit = true;
                break;
            case 'r':
                NEXUS_Playback_GetDefaultTrickModeSettings( &trickmode_settings );
                if(rate>=0) {
                    rate = -NEXUS_NORMAL_PLAY_SPEED;
                } else
                rate *=2;
                trickmode_settings.rate = rate;
                printf( "rewind %d\n", trickmode_settings.rate );
                NEXUS_Playback_TrickMode( pContext->playbackLoopback, &trickmode_settings );
                break;
            case 'w':
                BKNI_Sleep(30);
                break;
            case '-':
                BDBG_ASSERT(!NEXUS_Playback_GetStatus(pContext->playbackLoopback, &playbackStatus));
                if (playbackStatus.position >= 5*1000) {
                    playbackStatus.position -= 5*1000;
                    /* it's normal for a Seek to fail if it goes past the beginning */
                    printf("Jump to %u, %u...%u\n", (unsigned)playbackStatus.position, (unsigned)playbackStatus.first, (unsigned)playbackStatus.last);
                    (void)NEXUS_Playback_Seek(pContext->playbackLoopback, playbackStatus.position);
                }
                break;
            case '+':
                BDBG_ASSERT(!NEXUS_Playback_GetStatus(pContext->playbackLoopback, &playbackStatus));
                /* it's normal for a Seek to fail if it goes past the end */
                playbackStatus.position += 5*1000;
                printf("Jump to %u, %u...%u\n", (unsigned)playbackStatus.position, (unsigned)playbackStatus.first, (unsigned)playbackStatus.last);
                (void)NEXUS_Playback_Seek(pContext->playbackLoopback, playbackStatus.position);
                break;
            case '0':
                jump_to_beginning(pContext->playbackLoopback);
                break;
            case '\n':
            case ' ':
                BDBG_ASSERT(!NEXUS_Playback_GetStatus(pContext->playbackLoopback, &playbackStatus));
                NEXUS_StcChannel_GetStc(pContext->stcChannelLoopback, &stc);
                NEXUS_VideoDecoder_GetStatus(pContext->videoDecoderLoopback, &videoStatus);
                if(pContext->audioDecoderLoopback) NEXUS_AudioDecoder_GetStatus(pContext->audioDecoderLoopback, &audioStatus);
                if(g_bFifo) {
                    NEXUS_FifoRecord_GetPosition(pContext->fifoFile, &first, &last);
                }
                else {
                    NEXUS_FilePlay_GetBounds(pContext->filePlayLoopback, &first, &last);
                }

                printf("%d file %u:%u, playback %u%% position=%u.%03u sec, video %u%% %ux%u (%#x:%#x:%d)",
                    (int)playbackStatus.trickModeSettings.rate,
                    (unsigned)first.mpegFileOffset,  (unsigned)last.mpegFileOffset,
                    playbackStatus.fifoSize ? (playbackStatus.fifoDepth * 100) / playbackStatus.fifoSize : 0,
                    (unsigned)playbackStatus.position/1000, (unsigned)playbackStatus.position%1000,
                    videoStatus.fifoSize ? (videoStatus.fifoDepth * 100) / videoStatus.fifoSize : 0,
                    videoStatus.source.width, videoStatus.source.height, videoStatus.pts, stc, videoStatus.ptsStcDifference
                    );
                if(pContext->audioDecoderLoopback) printf(", audio %u%% %uHz (%#x:%#x:%d)\n",
                    audioStatus.fifoSize ? (audioStatus.fifoDepth * 100) / audioStatus.fifoSize : 0,
                    audioStatus.sampleRate, audioStatus.pts, stc, audioStatus.ptsStcDifference);
                else printf("\n");
                break;
            case 'd': /* display format switch */
                NEXUS_Display_GetSettings(pContext->displayLoopback, &displaySettings);
                printf("switch loopback display (hdmi/component out) format to:\n");
                printf("\n displayFormat:\n"
					" (%2d) 480i\n"
					" (%2d) 576i\n"
					" (%2d) 480p\n"
					" (%2d) 576p\n"
					" (%2d) 1080i60\n"
					" (%2d) 1080i50\n"
					" (%2d) 1080p24\n"
					" (%2d) 1080p25\n"
					" (%2d) 1080p30\n"
					" (%2d) 1080p50\n"
					" (%2d) 1080p60\n"
					" (%2d) 720p60\n"
					" (%2d) 720p50\n"
					" (%2d) 720p24\n"
					" (%2d) 720p25\n"
					" (%2d) 720p30Hz:\n",
					(NEXUS_VideoFormat_eNtsc),
					(NEXUS_VideoFormat_ePal),
					(NEXUS_VideoFormat_e480p),
					(NEXUS_VideoFormat_e576p),
					(NEXUS_VideoFormat_e1080i),
					(NEXUS_VideoFormat_e1080i50hz),
					(NEXUS_VideoFormat_e1080p24hz),
					(NEXUS_VideoFormat_e1080p25hz),
					(NEXUS_VideoFormat_e1080p30hz),
					(NEXUS_VideoFormat_e1080p50hz),
					(NEXUS_VideoFormat_e1080p60hz),
					(NEXUS_VideoFormat_e720p),
					(NEXUS_VideoFormat_e720p50hz),
					(NEXUS_VideoFormat_e720p24hz),
					(NEXUS_VideoFormat_e720p25hz),
					(NEXUS_VideoFormat_e720p30hz)); 
                scanf("%d", (int32_t*)&displaySettings.format);
                NEXUS_Display_SetSettings(pContext->displayLoopback, &displaySettings);
                break;
            case 'z': /* resize loopback video window */
                NEXUS_VideoWindow_GetSettings(pContext->windowLoopback, &windowSettings);
                printf("Resize loopback window to:\n");
                printf("position x: \n"); scanf("%d", &choice); windowSettings.position.x = (int16_t)choice;
                printf("position y: \n"); scanf("%d", &choice); windowSettings.position.y = (int16_t)choice;
                printf("position width: \n"); scanf("%d", &choice); windowSettings.position.width = (uint16_t)choice;
                printf("position height: \n"); scanf("%d", &choice); windowSettings.position.height= (uint16_t)choice;
                NEXUS_VideoWindow_SetSettings(pContext->windowLoopback, &windowSettings);
                break;
            case 'a': /* change loopback display aspect ratio correction mode */
                printf("\n Please select loopback window's aspect ratio correction mode:\n"
                    " (%d) Zoom\n"
                    " (%d) Box\n"
                    " (%d) Pan and Scan\n"
                    " (%d) Bypass aspect ratio correction\n"
                    " (%d) PanScan without additional aspect ratio correction\n",
                    (NEXUS_VideoWindowContentMode_eZoom),
                    (NEXUS_VideoWindowContentMode_eBox),
                    (NEXUS_VideoWindowContentMode_ePanScan),
                    (NEXUS_VideoWindowContentMode_eFull),
                    (NEXUS_VideoWindowContentMode_ePanScanWithoutCorrection));
                NEXUS_VideoWindow_GetSettings(pContext->windowLoopback, &windowSettings);
                scanf("%d", (int *)&windowSettings.contentMode);
                printf("\n Enable letterbox auto cut? (0) Disable; (1) enable;\n");
                scanf("%d", &choice); windowSettings.letterBoxAutoCut = choice;
                windowSettings.letterBoxDetect = windowSettings.letterBoxAutoCut;
                NEXUS_VideoWindow_SetSettings(pContext->windowLoopback, &windowSettings);
                break;
			case 'i': /* select xcoder loopback ID */
				B_Mutex_Lock(pContext->mutexStarted);
				if(g_loopbackStarted) {
					B_Mutex_Unlock(pContext->mutexStarted);
					printf("Already started loopback on xcoder%d\n", g_loopbackXcodeId);
					break;
				}
				B_Mutex_Unlock(pContext->mutexStarted);
				printf("Please select Xcoder ID for loopback [0 ~ %d]: ", NEXUS_NUM_VIDEO_ENCODERS-1);
				scanf("%d", &g_loopbackXcodeId); 
				g_loopbackXcodeId = (g_loopbackXcodeId > NEXUS_NUM_VIDEO_ENCODERS-1) 
					? (NEXUS_NUM_VIDEO_ENCODERS-1):g_loopbackXcodeId;
				pContext = &xcodeContext[g_loopbackXcodeId];
				break;
			case 'o': /* open xcoder loopback */
				B_Mutex_Lock(pContext->mutexStarted);
				if(!g_loopbackStarted) {
					printf("To start xcoder%d loopback...\n", g_loopbackXcodeId);
					xcode_loopback_setup(&xcodeContext[g_loopbackXcodeId]);
					g_loopbackStarted = true;
				}
				B_Mutex_Unlock(pContext->mutexStarted);
				break;
			case 'c': /* close xcoder loopback */
				B_Mutex_Lock(pContext->mutexStarted);
				if(g_loopbackStarted) {
					printf("To stop xcoder%d loopback...\n", g_loopbackXcodeId);
					xcode_loopback_shutdown(&xcodeContext[g_loopbackXcodeId]);
					g_loopbackStarted = false;
				}
				B_Mutex_Unlock(pContext->mutexStarted);
				break;
            default:
                break;
            }
        }
        if(quit)  {
            break;
        }

    }
}

#endif
#if NEXUS_HAS_FRONTEND
static void tune_qam( TranscodeContext  *pContext )
{
	NEXUS_FrontendQamSettings qamSettings;
#ifdef BTST_PID_RETUNE /* TODO: add PID channel change */
	char input[80];
	NEXUS_VideoDecoderStartSettings videoProgram;
	NEXUS_AudioDecoderStartSettings audioProgram;
	NEXUS_StcChannelSettings stcSettings;
#if BTST_ENABLE_AV_SYNC
		NEXUS_SyncChannelSettings syncChannelSettings;
#endif
	NEXUS_VideoDecoder_Stop(pContext->videoDecoder);
	NEXUS_PidChannel_Close(pContext->videoPidChannel);
#endif

#if NEXUS_HAS_FRONTEND
	printf("\n Front End QAM freq (Mhz) "); 						 scanf("%d", (int32_t*)&pContext->inputSettings.freq);
	printf("\n Front End QAM Mode (%d) 64 (%d) 256 (%d) 1024	", 
		NEXUS_FrontendQamMode_e64, NEXUS_FrontendQamMode_e256, NEXUS_FrontendQamMode_e1024); 
	scanf("%d", (int32_t*)&pContext->inputSettings.qamMode);
#endif

#ifdef BTST_PID_RETUNE
	printf("\n source stream type (2) Ts			 "); pContext->inputSettings.eStreamType = NEXUS_TransportType_eTs;
	printf("\n source stream codec (%d) Mpeg2 (%d) H264",
		NEXUS_VideoCodec_eMpeg2, NEXUS_VideoCodec_eH264); 
	scanf("%d", (int32_t*)&pContext->inputSettings.eVideoCodec);
	printf("\n Video pid									 "); scanf("%s", input); pContext->inputSettings.iVideoPid = strtoul(input, NULL, 0);
	printf("\n Pcr	 pid									 "); scanf("%s", input); pContext->inputSettings.iPcrPid = strtoul(input, NULL, 0);

	pContext->videoPidChannel = NEXUS_PidChannel_Open(NEXUS_ParserBand_e0, pContext->inputSettings.iVideoPid, NULL);

	NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram);
	videoProgram.codec = pContext->inputSettings.eVideoCodec;
	videoProgram.pidChannel = pContext->videoPidChannel;
	videoProgram.stcChannel = pContext->stcChannel;
	if(pContext->inputSettings.bAudioInput)
	{
		NEXUS_AudioDecoder_Stop(pContext->audioDecoder);
		NEXUS_PidChannel_Close(pContext->audioPidChannel);
#if BTST_ENABLE_AV_SYNC
		/* disconnect sync channel after decoders stop */
		NEXUS_SyncChannel_GetSettings(pContext->syncChannel, &syncChannelSettings);
		syncChannelSettings.videoInput = NULL;
		syncChannelSettings.audioInput[0] = NULL;
		syncChannelSettings.audioInput[1] = NULL;
		NEXUS_SyncChannel_SetSettings(pContext->syncChannel, &syncChannelSettings);
#endif
		printf("\n Audio pid									 "); scanf("%s", input); pContext->inputSettings.iAudioPid = strtoul(input, NULL, 0);
		pContext->audioPidChannel = NEXUS_PidChannel_Open(NEXUS_ParserBand_e0, pContext->inputSettings.iAudioPid, NULL);
		NEXUS_AudioDecoder_GetDefaultStartSettings(&audioProgram);
		audioProgram.codec = NEXUS_AudioCodec_eAc3;
		audioProgram.pidChannel = pContext->audioPidChannel;
		audioProgram.stcChannel = pContext->stcChannel;
	}
	
	NEXUS_StcChannel_GetSettings(pContext->stcChannel, &stcSettings);
	stcSettings.mode = NEXUS_StcChannelMode_ePcr; /* live */
	stcSettings.modeSettings.pcr.pidChannel = pContext->videoPidChannel; /* PCR happens to be on video pid */
	NEXUS_StcChannel_SetSettings(pContext->stcChannel, &stcSettings);
#endif

	NEXUS_Frontend_GetDefaultQamSettings(&qamSettings);
	qamSettings.frequency = pContext->inputSettings.freq* 1000000;
	qamSettings.mode = pContext->inputSettings.qamMode;
	switch (pContext->inputSettings.qamMode) {
				case NEXUS_FrontendQamMode_e64: qamSettings.symbolRate = 5056900; break;
				case NEXUS_FrontendQamMode_e256: qamSettings.symbolRate = 5360537; break;
				case NEXUS_FrontendQamMode_e1024: qamSettings.symbolRate = 0; /* TODO */ break;
				default: fprintf(stderr, "Invalid QAM mode!\n"); return;
	}
	qamSettings.annex = NEXUS_FrontendQamAnnex_eB;
	qamSettings.bandwidth = NEXUS_FrontendQamBandwidth_e6Mhz;
	qamSettings.lockCallback.callback = lock_callback;
	qamSettings.lockCallback.context = pContext->frontend;
	NEXUS_Frontend_TuneQam(pContext->frontend, &qamSettings);
	
#ifdef BTST_PID_RETUNE
	if(pContext->inputSettings.bAudioInput)
	{
#if BTST_ENABLE_AV_SYNC
		/* disconnect sync channel after decoders stop */
		NEXUS_SyncChannel_GetSettings(pContext->syncChannel, &syncChannelSettings);
		syncChannelSettings.videoInput = NEXUS_VideoDecoder_GetConnector(pContext->videoDecoder);
		syncChannelSettings.audioInput[0] = NEXUS_AudioDecoder_GetConnector(pContext->audioDecoder, pContext->encodeSettings.bAudioEncode?
			NEXUS_AudioDecoderConnectorType_eStereo : NEXUS_AudioDecoderConnectorType_eCompressed);
		NEXUS_SyncChannel_SetSettings(pContext->syncChannel, &syncChannelSettings);
#endif
		NEXUS_AudioDecoder_Start(pContext->audioDecoder, &audioProgram);
	}
	NEXUS_VideoDecoder_Start(pContext->videoDecoder, &videoProgram);
#endif
}
#else
static void tune_qam( TranscodeContext  *pContext )
{
	BSTD_UNUSED(pContext);
	BDBG_ERR(("No frontend supported"));
}
#endif

static void complete(void *data, int unused)
{
    BSTD_UNUSED(unused);
    BKNI_SetEvent((BKNI_EventHandle)data);
}

static void open_gfx(
	TranscodeContext  *pContext )
{
    NEXUS_SurfaceCreateSettings createSettings;
    NEXUS_GraphicsSettings graphicsSettings;
    NEXUS_Graphics2DFillSettings fillSettings;
    NEXUS_Graphics2DSettings gfxSettings;
    NEXUS_Error rc;

    NEXUS_Surface_GetDefaultCreateSettings(&createSettings);
    createSettings.pixelFormat = NEXUS_PixelFormat_eA8_R8_G8_B8;
    createSettings.width = 100;
    createSettings.height = 100;
    createSettings.heap = NEXUS_Platform_GetFramebufferHeap(g_encodeDisplayId[pContext->contextId]);
    pContext->surface = NEXUS_Surface_Create(&createSettings);

    BKNI_CreateEvent(&pContext->checkpointEvent);
    BKNI_CreateEvent(&pContext->spaceAvailableEvent);

    pContext->gfx = NEXUS_Graphics2D_Open(0, NULL);

    NEXUS_Graphics2D_GetSettings(pContext->gfx, &gfxSettings);
    gfxSettings.checkpointCallback.callback = complete;
    gfxSettings.checkpointCallback.context = pContext->checkpointEvent;
    gfxSettings.packetSpaceAvailable.callback = complete;
    gfxSettings.packetSpaceAvailable.context = pContext->spaceAvailableEvent;
    NEXUS_Graphics2D_SetSettings(pContext->gfx, &gfxSettings);

    /* fill with black */
    NEXUS_Graphics2D_GetDefaultFillSettings(&fillSettings);
    fillSettings.surface = pContext->surface;
    fillSettings.rect.width = createSettings.width;
    fillSettings.rect.height = createSettings.height;
    fillSettings.color = 0x60FF0000;
    NEXUS_Graphics2D_Fill(pContext->gfx, &fillSettings);
    
    rc = NEXUS_Graphics2D_Checkpoint(pContext->gfx, NULL); /* require to execute queue */
    if (rc == NEXUS_GRAPHICS2D_QUEUED) {
        rc = BKNI_WaitForEvent(pContext->checkpointEvent, BKNI_INFINITE);
    }
    BDBG_ASSERT(!rc);

    NEXUS_Display_GetGraphicsSettings(pContext->displayTranscode, &graphicsSettings);
    graphicsSettings.enabled = true;
    graphicsSettings.position.x = 0;
    graphicsSettings.position.y = 0;
    graphicsSettings.position.width  = createSettings.width;
    graphicsSettings.position.height = createSettings.height;
    graphicsSettings.clip.width      = graphicsSettings.position.width;
    graphicsSettings.clip.height     = graphicsSettings.position.height;
    NEXUS_Display_SetGraphicsSettings(pContext->displayTranscode, &graphicsSettings);
    NEXUS_Display_SetGraphicsFramebuffer(pContext->displayTranscode, pContext->surface);
}

static void close_gfx(
	TranscodeContext  *pContext )
{
    NEXUS_GraphicsSettings graphicsSettings;

    if(NULL == pContext->gfx) return;

    /* disable gfx window */
    NEXUS_Display_GetGraphicsSettings(pContext->displayTranscode, &graphicsSettings);
    graphicsSettings.enabled = false;
    NEXUS_Display_SetGraphicsSettings(pContext->displayTranscode, &graphicsSettings);

    NEXUS_Graphics2D_Close(pContext->gfx);
    NEXUS_Surface_Destroy(pContext->surface);
    BKNI_DestroyEvent(pContext->checkpointEvent);
    BKNI_DestroyEvent(pContext->spaceAvailableEvent);
    pContext->gfx = NULL;
}

static void mux_duration( TranscodeContext *pContext )
{
	unsigned hrs, mins, secs;
	NEXUS_StreamMuxStatus status;
	NEXUS_StreamMux_GetStatus(pContext->streamMux, &status);
	secs = status.duration/1000;
	mins = secs/60;
	hrs  = mins/60;
	mins = mins%60;
	secs = secs%60;
	printf("Mux[%u] duration: %02u:%02u:%02u.%03u\n", pContext->contextId,
		hrs, mins, secs, status.duration%1000);
}

static void avsync_correlation_error( TranscodeContext *pContext )
{
	size_t size[2];
	const NEXUS_VideoEncoderDescriptor *vdesc[2];
	const NEXUS_AudioMuxOutputFrame *adesc[2];
	unsigned i, j;
	uint32_t v_opts, a_opts;
	uint64_t v_pts, a_pts, v_stc, a_stc;
	double v_opts2pts, a_opts2pts, error;
	bool validVframe = false, validAframe = false;

	NEXUS_VideoEncoder_GetBuffer(pContext->videoEncoder, &vdesc[0], &size[0], &vdesc[1], &size[1]);
	for(j=0;j<2 && !validVframe;j++) {
		for(i=0;i<size[j];i++) {
			if(vdesc[j][i].flags & NEXUS_VIDEOENCODERDESCRIPTOR_FLAG_PTS_VALID)
			{
				v_opts = vdesc[j][i].originalPts;
				v_pts  = vdesc[j][i].pts;
				v_stc  = vdesc[j][i].stcSnapshot;
				v_opts2pts = (double)v_pts / 90 - (double)v_opts / 45;
				BDBG_MSG(("V: opts=%u pts=%llu, stc=%llu, opts2pts=%f", v_opts, v_pts, v_stc, v_opts2pts));
				validVframe = true;
				break;
			}
		}
	}
	NEXUS_AudioMuxOutput_GetBuffer(pContext->audioMuxOutput, &adesc[0], &size[0], &adesc[1], &size[1]);
	for(j=0;j<2 && !validAframe;j++) {
		for(i=0;i<size[j];i++) {
			if(adesc[j][i].flags & NEXUS_AUDIOMUXOUTPUTFRAME_FLAG_PTS_VALID)
			{
				a_opts = adesc[j][i].originalPts;
				a_pts  = adesc[j][i].pts;
				a_stc  = adesc[j][i].stcSnapshot;
				a_opts2pts = (double)a_pts / 90 - (double)a_opts / 45;
				BDBG_MSG(("A: opts=%u pts=%llu, stc=%llu, opts2pts=%f", a_opts, a_pts, a_stc, a_opts2pts));
				validAframe = true;
				break;
			}
		}
	}
	if(validVframe && validAframe) {
		error = a_opts2pts - v_opts2pts;
		printf("AV sync correlation error (positive means video leads): %.1f ms\n\n", error);
		if(error > 20 || error < -20) BDBG_ERR(("AV sync error is greater than 20ms!\n"));
	}
}

/* build with export BPROFILE_SUPPORT=y to enable profiling of memory allocation */
#if !BPROFILE_SUPPORT
/* if BPROFILE_SUPPORT not enabled, stub out functions to prevent error */
#define NEXUS_Profile_MarkThread(s)
#define NEXUS_Profile_Start()
#define NEXUS_Profile_Stop(X)
#endif
static void keyHandler( TranscodeContext  xcodeContext[] )
{
	NEXUS_VideoEncoderSettings videoEncoderConfig;
	NEXUS_VideoWindowSplitScreenSettings splitSettings;
	NEXUS_VideoWindowSettings windowSettings;
	NEXUS_VideoWindowCoefficientIndexSettings stCoeffIndex;
	NEXUS_VideoWindowAfdSettings afdSettings;
	NEXUS_DisplaySettings displaySettings;
	NEXUS_DisplayCustomFormatSettings customFormatSettings;
	char key[256];
	int choice, input;
	bool enableMad;
	NEXUS_VideoEncoderSettingsOnInputChange encodeSettingsOnInputChange;
	NEXUS_VideoEncoderStatus videoEncodeStatus;
	NEXUS_VideoDecoderSettings videoDecoderSettings;
	NEXUS_Error rc;
	int rcInput;
	uint32_t ulSecToSleep;
	TranscodeContext *pContext = &xcodeContext[g_selectedXcodeContextId];

	NEXUS_Profile_MarkThread("keyhandler");
	g_keyReturn = 'q';
	for(;;)
	{
		printf("Menu:\n");
#if BTST_ENABLE_XCODE_LOOPBACK
		printf(" 0) xcode loopback player control\n");
#endif
		printf(" 1) change video encoder resolution\n");
		printf(" 2) change video encoder bitrate\n");
		printf(" 3) change video encoder frame rate\n");
		printf(" 4) change xcode video window MAD settings\n");
		printf(" 5) change xcode video window DNR settings\n");
		if(pContext->inputSettings.resource == BTST_RESOURCE_QAM)
			printf(" 6) retune QAM channel\n");
		printf(" 7) change xcode window DNR demo mode\n");
		printf(" 8) change Video stream mux PES pacing\n");
		printf(" 9) change Audio stream mux PES pacing\n");
		printf("10) change PCR stream mux PES pacing\n");
		printf("11) change source video decoder scanout mode\n");
		printf("12) change xcode aspect ratio correction mode\n");
		printf("13) change xcode window scaler coeff index\n");
		printf("14) Get video encoder status\n");
		printf("15) Select xcoder context to config\n");
		printf("16) Start new RAP\n");
		printf("17) change GOP structure setting\n");
		printf("18) change Closed Caption user data transcode setting\n");
		printf("19) change xcode video window AFD/Bar clip setting\n");
		printf("20) Open/Start selected transcoder\n");
		printf("21) Stop/Close selected transcoder\n");
		printf("22) Start selected transcoder in adaptive low latency mode\n");
		printf("23) Stop  selected transcoder in immediate mode\n");
		printf("24) Start system data insertion\n");
		printf("25) Stop system data insertion\n");
		printf("26) Change audio encoder bitrate\n");
		printf("27) Toggle audio encoder sample rate to 32KHz\n");
		printf("28) Open Gfx window\n");
		printf("29) Close Gfx window\n");
		printf("30) Stop stream mux recpump\n");
		printf("31) Start stream mux recpump\n");
		printf("50) random dynamic resolution test\n");
		printf("51) random dynamic bitrate test\n");
		if(pContext->inputSettings.resource == BTST_RESOURCE_FILE)
			printf("90) Source trick mode player\n");
		printf("99) change DEBUG module setting\n");
		printf("100) sleep\n");
		printf(" c) Enter 'c' to stop/start the next iteration\n\n");
		printf("\nEnter 'q' to quit\n\n");
		rcInput = scanf("%s", key);
		if(g_bScriptMode && rcInput == EOF)
		{
			BKNI_Sleep(1000*g_scriptModeSleepTime);
			g_keyReturn = 'q';
			break;
		}
		if(!strcmp(key, "q")) 
		{
			g_keyReturn = 'q';
			break;
		}
		else if(!strcmp(key, "c")) {
			g_keyReturn = 'c';
			break;
		}
		choice = strtoul(key, NULL, 0);
		B_Mutex_Lock(pContext->mutexStarted);
		if(!pContext->bStarted && (choice!=20) && (choice!=22) && (choice!=15) && (choice!=99)) {
			printf("context %d not started yet.\n", pContext->contextId);
			B_Mutex_Unlock(pContext->mutexStarted);
			continue;
		}
		switch(choice)
		{
#if BTST_ENABLE_XCODE_LOOPBACK
			case 0: /* xcoder loopback trick play control */
				B_Mutex_Unlock(pContext->mutexStarted);
				loopbackPlayer(xcodeContext);
				B_Mutex_Lock(pContext->mutexStarted);
				break;
#endif
			case 1: /* resolution change */
				printf("xcode resolution change:   ");

				/* typically resolution change combined with bit rate change */
				printf("Do you want to change bit rate synchronously as well? 0)No 1)Yes\n");
				scanf("%d", &input);
				if(input)
				{
					printf("Bit rate change from %u to:   ", pContext->encodeSettings.encoderBitrate);
					printf("\n Bitrate (bps)	"); 
					scanf("%d", &pContext->encodeSettings.encoderBitrate);

					/* turn on the synchronous change feature first before resolution/bitrate change! */
					NEXUS_VideoEncoder_GetSettingsOnInputChange(pContext->videoEncoder, &encodeSettingsOnInputChange);
					encodeSettingsOnInputChange.bitrateMax = pContext->encodeSettings.encoderBitrate;
					NEXUS_VideoEncoder_SetSettingsOnInputChange(pContext->videoEncoder, &encodeSettingsOnInputChange);
				}

				printf("\n custom format (0) No (1) Yes "); 
				scanf("%d", &choice); pContext->encodeSettings.bCustom = choice;
				if(!pContext->encodeSettings.bCustom)
				{
					printf("\n displayFormat:\n"
						" (%2d) 480i\n"
						" (%2d) 576i\n"
						" (%2d) 480p\n"
						" (%2d) 576p\n"
						" (%2d) 1080i60\n"
						" (%2d) 1080i50\n"
						" (%2d) 1080p24\n"
						" (%2d) 1080p25\n"
						" (%2d) 1080p30\n"
						" (%2d) 1080p50\n"
						" (%2d) 1080p60\n"
						" (%2d) 720p60\n"
						" (%2d) 720p50\n"
						" (%2d) 720p24\n"
						" (%2d) 720p25\n"
						" (%2d) 720p30Hz:\n",
						(NEXUS_VideoFormat_eNtsc),
						(NEXUS_VideoFormat_ePal),
						(NEXUS_VideoFormat_e480p),
						(NEXUS_VideoFormat_e576p),
						(NEXUS_VideoFormat_e1080i),
						(NEXUS_VideoFormat_e1080i50hz),
						(NEXUS_VideoFormat_e1080p24hz),
						(NEXUS_VideoFormat_e1080p25hz),
						(NEXUS_VideoFormat_e1080p30hz),
						(NEXUS_VideoFormat_e1080p50hz),
						(NEXUS_VideoFormat_e1080p60hz),
						(NEXUS_VideoFormat_e720p),
						(NEXUS_VideoFormat_e720p50hz),
						(NEXUS_VideoFormat_e720p24hz),
						(NEXUS_VideoFormat_e720p25hz),
						(NEXUS_VideoFormat_e720p30hz)); 
					scanf("%d", (int32_t*)&pContext->encodeSettings.displayFormat);
					NEXUS_Display_GetSettings(pContext->displayTranscode, &displaySettings);
					displaySettings.format = pContext->encodeSettings.displayFormat;
					NEXUS_Display_SetSettings(pContext->displayTranscode, &displaySettings);
				}
				else
				{
					printf("\n Resolution width & height	");
					scanf("%u %u", &pContext->encodeSettings.customFormatSettings.width, &pContext->encodeSettings.customFormatSettings.height);
					printf("\n Interlaced? (0) No (1) Yes	"); 
					scanf("%d", &choice); pContext->encodeSettings.customFormatSettings.interlaced = choice;
					printf("\n vertical refresh rate (in 1/1000th Hz) 	"); 
					scanf("%u", &pContext->encodeSettings.customFormatSettings.refreshRate);
					printf("\n Aspect Ratio:\n"
						" (%d) Auto\n"
						" (%d) 4x3\n"
						" (%d) 16x9\n"
						" (%d) SAR\n",
						NEXUS_DisplayAspectRatio_eAuto,
						NEXUS_DisplayAspectRatio_e4x3,
						NEXUS_DisplayAspectRatio_e16x9,
						NEXUS_DisplayAspectRatio_eSar); 
					scanf("%d", (int32_t*)&pContext->encodeSettings.customFormatSettings.aspectRatio);
					if(NEXUS_DisplayAspectRatio_eSar == pContext->encodeSettings.customFormatSettings.aspectRatio)
					{
						printf("Please enter Sample Aspect Ratio X and Y: \n");
						scanf("%u %u", &pContext->encodeSettings.customFormatSettings.sampleAspectRatio.x,
							&pContext->encodeSettings.customFormatSettings.sampleAspectRatio.y);
					}
					NEXUS_Display_GetDefaultCustomFormatSettings(&customFormatSettings);
					customFormatSettings.width = pContext->encodeSettings.customFormatSettings.width;
					customFormatSettings.height = pContext->encodeSettings.customFormatSettings.height;
					customFormatSettings.refreshRate = pContext->encodeSettings.customFormatSettings.refreshRate;
					customFormatSettings.interlaced = pContext->encodeSettings.customFormatSettings.interlaced;
					customFormatSettings.aspectRatio = pContext->encodeSettings.customFormatSettings.aspectRatio;
					customFormatSettings.sampleAspectRatio.x = pContext->encodeSettings.customFormatSettings.sampleAspectRatio.x;
					customFormatSettings.sampleAspectRatio.y = pContext->encodeSettings.customFormatSettings.sampleAspectRatio.y;
					customFormatSettings.dropFrameAllowed = true;
					rc = NEXUS_Display_SetCustomFormatSettings(pContext->displayTranscode, NEXUS_VideoFormat_eCustom2, &customFormatSettings);
					assert(!rc);
				}
				/* resolution change typically could come with bit rate change; separate for now */
				break;
			case 2: /* bitrate change */ 
				printf("Bit rate change from %u to:   ", pContext->encodeSettings.encoderBitrate);
				printf("\n Bitrate (bps)	"); 
				scanf("%d", (int32_t*)&pContext->encodeSettings.encoderBitrate);
				NEXUS_VideoEncoder_GetSettings(pContext->videoEncoder, &videoEncoderConfig);
				videoEncoderConfig.bitrateMax = pContext->encodeSettings.encoderBitrate;
				NEXUS_VideoEncoder_SetSettings(pContext->videoEncoder, &videoEncoderConfig);
				break;
			case 3: /* frame rate change */
				printf("frame rate change:   ");
				printf("\n Frame rate\n"
					" (%d) 23.976\n"
					" (%d) 24\n"
					" (%d) 25\n"
					" (%d) 29.97Hz\n"
					" (%d) 30 Hz\n"
					" (%d) 50 Hz\n"
					" (%d) 59.94\n"
					" (%d) 60\n"
					" (%d) 14.985\n",
					NEXUS_VideoFrameRate_e23_976,
					NEXUS_VideoFrameRate_e24,
					NEXUS_VideoFrameRate_e25,
					NEXUS_VideoFrameRate_e29_97,
					NEXUS_VideoFrameRate_e30,
					NEXUS_VideoFrameRate_e50,
					NEXUS_VideoFrameRate_e59_94,
					NEXUS_VideoFrameRate_e60,
					NEXUS_VideoFrameRate_e14_985);
				scanf("%d", (int32_t*)&pContext->encodeSettings.encoderFrameRate);
				NEXUS_VideoEncoder_GetSettings(pContext->videoEncoder, &videoEncoderConfig);
				printf("Current encode variable frame rate mode: %d\n", videoEncoderConfig.variableFrameRate);
				printf("Change encode variable frame rate mode: [0=N/1=Y]   ");
				scanf("%d", &choice); videoEncoderConfig.variableFrameRate = choice;
				videoEncoderConfig.frameRate = pContext->encodeSettings.encoderFrameRate;
				NEXUS_VideoEncoder_SetSettings(pContext->videoEncoder, &videoEncoderConfig);
				break;
			case 4: /* MAD setting */
				/* window PQ custom setting */
				printf("MAD settings:\n");
				printf("Enable? (0) Disable; (1) Enable; ");
				scanf("%d", &choice); enableMad = choice;
				window_mad(pContext->windowTranscode, true, enableMad, false);
				break;
			case 5: /* DNR setting */
				/* window PQ custom setting */
				window_dnr(pContext->windowTranscode, true);
				break;
			case 6: /* retune source channel */
				if(pContext->inputSettings.resource == BTST_RESOURCE_QAM)
					tune_qam(pContext);
				break;
			case 7: /* xcode window demo mode */
				NEXUS_VideoWindow_GetSplitScreenSettings(pContext->windowTranscode, &splitSettings);
				printf("\n xcode window DNR demo mode: (%d)Disable (%d)Left (%d)Right	",
					NEXUS_VideoWindowSplitScreenMode_eDisable, 
					NEXUS_VideoWindowSplitScreenMode_eLeft,
					NEXUS_VideoWindowSplitScreenMode_eRight); 
				scanf("%d", (int32_t*)&splitSettings.dnr);
				NEXUS_VideoWindow_SetSplitScreenSettings(pContext->windowTranscode, &splitSettings);
				break;
			case 8: /* toggle video stream mux pes pacing */
				printf("\n Disable xcode video PES pacing mode: (0)No (1)Yes	"); 
				scanf("%d", &input);
				NEXUS_Playpump_SuspendPacing(pContext->playpumpTranscodeVideo, input);
				break;
			case 9: /* toggle audio stream mux pes pacing */
				printf("\n Disable xcode audio PES pacing mode: (0)No (1)Yes	"); 
				scanf("%d", &input);
				NEXUS_Playpump_SuspendPacing(pContext->playpumpTranscodeAudio, input);
				break;
			case 10: /* toggle system data stream mux pes pacing */
				printf("\n Disable xcode system data PES pacing mode: (0)No (1)Yes	"); 
				scanf("%d", &input);
				NEXUS_Playpump_SuspendPacing(pContext->playpumpTranscodePcr, input);
				break;
			case 11: /* change 1080p24/30 source video decoder's scan mode progressive 3:2/2:2 ITFP test */
				printf("\n Force 1080p24/30 source video decoder progressive scan for 3:2/2:2 ITFP test : (0)No (1)Yes	"); 
				NEXUS_VideoDecoder_GetSettings(pContext->videoDecoder, &videoDecoderSettings);
				scanf("%d", (int *)&videoDecoderSettings.scanMode);
				NEXUS_VideoDecoder_SetSettings(pContext->videoDecoder, &videoDecoderSettings);
				break;
			case 12: /* change xcoder aspect ratio correction mode */
				printf("\n Please select transcoder aspect ratio correction mode:\n"
					" (%d) Zoom\n"
					" (%d) Box\n"
					" (%d) Pan and Scan\n"
					" (%d) Bypass aspect ratio correction\n"
					" (%d) PanScan without additional aspect ratio correction\n",
					(NEXUS_VideoWindowContentMode_eZoom),
					(NEXUS_VideoWindowContentMode_eBox),
					(NEXUS_VideoWindowContentMode_ePanScan),
					(NEXUS_VideoWindowContentMode_eFull),
					(NEXUS_VideoWindowContentMode_ePanScanWithoutCorrection));
				NEXUS_VideoWindow_GetSettings(pContext->windowTranscode, &windowSettings);
				scanf("%d", (int *)&windowSettings.contentMode);
				NEXUS_VideoWindow_SetSettings(pContext->windowTranscode, &windowSettings);
				break;
			case 13: /* change xcoder window scaler coeffs */
				NEXUS_VideoWindow_GetCoefficientIndexSettings(pContext->windowTranscode, &stCoeffIndex);
				printf("\nPlease set transcoder window scaler coefficients index:\n");
				printf("current scaler coeff index = %d;   new index [101~126] = ", stCoeffIndex.sclHorzLuma);
				scanf("%u", (uint32_t *)&stCoeffIndex.sclHorzLuma);
				NEXUS_VideoWindow_SetCoefficientIndexSettings(pContext->windowTranscode, &stCoeffIndex);
				break;
			case 14: /* get video encoder status */
				{ 
					NEXUS_VideoEncoderClearStatus clearStatus;
					NEXUS_VideoDecoderStatus videoDecodeStatus;
					NEXUS_AudioDecoderStatus audioDecodeStatus;
					NEXUS_VideoDecoder_GetStatus(pContext->videoDecoder, &videoDecodeStatus);
					printf("\nVideo Decoder[%d] Status:\n", pContext->contextId);
					printf("----------------------\n");
					printf("data buffer depth     = %u\n", videoDecodeStatus.fifoDepth);
					printf("data buffer size      = %u\n", videoDecodeStatus.fifoSize);
					printf("queued frames         = %u\n", videoDecodeStatus.queueDepth);
					printf("numDecoded count      = %u\n", videoDecodeStatus.numDecoded);
					printf("numDisplayed          = %u\n", videoDecodeStatus.numDisplayed);
					printf("numDecodeDrops        = %u\n", videoDecodeStatus.numDecodeDrops);
					printf("numDisplayDrops       = %u\n", videoDecodeStatus.numDisplayDrops);
					printf("numDecodeOverflows    = %u\n", videoDecodeStatus.numDecodeOverflows);
					printf("numDisplayUnderflows  = %u\n", videoDecodeStatus.numDisplayUnderflows);
					printf("current PTS (45KHz)   = 0x%x\n", videoDecodeStatus.pts);
					printf("PTS error count       = %u\n", videoDecodeStatus.ptsErrorCount);
					printf("----------------------\n\n");
					if(pContext->inputSettings.bAudioInput) {
						NEXUS_AudioDecoder_GetStatus(pContext->audioDecoder, &audioDecodeStatus);
						printf("Audio Decoder[%d] Status:\n", pContext->contextId);
						printf("----------------------\n");
						printf("data buffer depth     = %u\n", audioDecodeStatus.fifoDepth);
						printf("data buffer size      = %u\n", audioDecodeStatus.fifoSize);
						printf("queued frames         = %u\n", audioDecodeStatus.queuedFrames);
						printf("numDecoded count      = %u\n", audioDecodeStatus.framesDecoded);
						printf("numDummyFrames        = %u\n", audioDecodeStatus.dummyFrames);
						printf("numFifoOverflows      = %u\n", audioDecodeStatus.numFifoOverflows);
						printf("numFifoUnderflows     = %u\n", audioDecodeStatus.numFifoUnderflows);
						printf("current PTS (45KHz)   = 0x%x\n", audioDecodeStatus.pts);
						printf("PTS error count       = %u\n", audioDecodeStatus.ptsErrorCount);
						printf("----------------------\n\n");
					}
					NEXUS_VideoEncoder_GetStatus(pContext->videoEncoder, &videoEncodeStatus);
					printf("Video Encoder[%d] Status:\n", pContext->contextId);
					printf("----------------------\n");
					printf("data buffer base address     = %p\n", videoEncodeStatus.bufferBase);
					printf("metadata buffer base address = %p\n", videoEncodeStatus.metadataBufferBase);
					printf("error flags                  = 0x%x\n", videoEncodeStatus.errorFlags);
					printf("error count                  = %u\n", videoEncodeStatus.errorCount);
					printf("picture drops due to error   = %u\n", videoEncodeStatus.picturesDroppedErrors);
					printf("picture drops due to FRC     = %u\n", videoEncodeStatus.picturesDroppedFRC);
					printf("pictures Encoded             = %u\n", videoEncodeStatus.picturesEncoded);
					printf("pictures Received            = %u\n", videoEncodeStatus.picturesReceived);
					printf("picture Id Last Encoded      = 0x%x\n", videoEncodeStatus.pictureIdLastEncoded);
					printf("----------------------\n\n");
					NEXUS_VideoEncoder_GetDefaultClearStatus(&clearStatus);
					NEXUS_VideoEncoder_ClearStatus(pContext->videoEncoder, &clearStatus);
					mux_duration(pContext);
					if(pContext->inputSettings.bAudioInput)
						avsync_correlation_error(pContext);
				}
				break;
			case 15: /* select xcoder context */
				printf("Please select Xcoder context to configure [0 ~ %d]: ", NEXUS_NUM_VIDEO_ENCODERS-1);
				scanf("%d", &g_selectedXcodeContextId); 
				g_selectedXcodeContextId = (g_selectedXcodeContextId > NEXUS_NUM_VIDEO_ENCODERS-1) 
					? (NEXUS_NUM_VIDEO_ENCODERS-1):g_selectedXcodeContextId;
				B_Mutex_Unlock(pContext->mutexStarted);
				pContext = &xcodeContext[g_selectedXcodeContextId];
				B_Mutex_Lock(pContext->mutexStarted);
				break;
			case 16: /* start new RAP */
				NEXUS_VideoEncoder_InsertRandomAccessPoint(pContext->videoEncoder);
				break;
			case 17: /* dynamic change GOP structure */
				NEXUS_VideoEncoder_GetSettings(pContext->videoEncoder, &videoEncoderConfig);
				printf("Context%d current GOP structure: num of P = %u, num of B = %u\n", pContext->contextId,
					videoEncoderConfig.streamStructure.framesP, videoEncoderConfig.streamStructure.framesB);
				printf("Change num of P to: ");
				scanf("%d", &choice);
				videoEncoderConfig.streamStructure.framesP = pContext->encodeSettings.encoderGopStructureFramesP = choice;
				printf("Change num of B to: ");
				scanf("%d", &choice);
				videoEncoderConfig.streamStructure.framesB = pContext->encodeSettings.encoderGopStructureFramesB = choice;
				NEXUS_VideoEncoder_SetSettings(pContext->videoEncoder, &videoEncoderConfig);
				break;
			case 18: /* change CC data transcode ON/OFF */
				printf("Context%d enabled CC data transcoding: %d\n", g_selectedXcodeContextId, g_bEncodeCC);
				printf("Change to [1=ON; 0=OFF]: ");
				scanf("%d", &choice);
				g_bEncodeCC = choice;
				pContext->bEncodeCCUserData = g_bEncodeCC;
				break;
			case 19: /* change AFD/Bar clip setting */
				NEXUS_VideoWindow_GetAfdSettings(pContext->windowTranscode, &afdSettings);
				printf("\nPlease set transcoder window AFD/Bar clip mode:\n");
				printf("current AFD/Bar clip mode = %d; new setting = [0=Dis/1=En]", afdSettings.mode);
				scanf("%d", &choice);
				afdSettings.mode = choice? NEXUS_AfdMode_eStream : NEXUS_AfdMode_eDisabled;
				NEXUS_VideoWindow_SetAfdSettings(pContext->windowTranscode, &afdSettings);
				break;
			case 22: /* start adaptive delay */
				pContext->bAdaptiveLatencyStart = pContext->bImmediateStop;
#if BTST_ENABLE_FAST_STOP_START
				BDBG_MSG(("\t\nTo channel restart...\n"));
				start_transcode(pContext); /* start only */
				pContext->bNoStopDecode = false; /* reset */
#if BTST_ENABLE_XCODE_LOOPBACK
				if((g_selectedXcodeContextId == g_loopbackXcodeId) && !g_loopbackStarted) {
					xcode_loopback_setup(pContext);
					g_loopbackStarted = true;
				}
#endif
				break;
#endif
			case 20: /* start selected xcoder context */
				if(pContext->bStarted) {
					printf("context %d already started.\n", pContext->contextId);
					break;
				}
				printf("To start the xcoder%d...\n", pContext->contextId);
				if(!pContext->inputSettings.bConfig) config_xcoder_context(pContext);
				else {
					printf("Reconfigure the transcoder%d? [1=y/0=n] ", pContext->contextId);
					scanf("%d", &input); 
					if(input) config_xcoder_context(pContext);
				}	
#if BTST_ENABLE_XCODE_LOOPBACK/* shutdown loopback decoder for quad xcodes */
				if(g_activeXcodeCount+1 >= NEXUS_NUM_VIDEO_DECODERS && g_loopbackStarted) {
					g_loopbackStarted = false;
					xcode_loopback_shutdown(&xcodeContext[g_loopbackXcodeId]);
				}
#endif
				/* open & start transcoder */
				bringup_transcode(pContext);
#if BTST_ENABLE_XCODE_LOOPBACK
				if((g_selectedXcodeContextId == g_loopbackXcodeId) && !g_loopbackStarted) {
					xcode_loopback_setup(pContext);
					g_loopbackStarted = true;
				}
#endif
				break;
			case 23: /* stop mode */
				printf("Immediate stop encoder? [1=y/0=n]\n");
				scanf("%d", &input);
				pContext->bImmediateStop = input;
				printf("Don't stop decoder? [1=y/0=n]\n");
				scanf("%d", &input);
				pContext->bNoStopDecode = input;
#if BTST_ENABLE_FAST_STOP_START
				BDBG_MSG(("\t\nTo channel stop...\n"));
#if BTST_ENABLE_XCODE_LOOPBACK /* bringdown loopback path */
				if(g_selectedXcodeContextId == g_loopbackXcodeId && g_loopbackStarted) {
					g_loopbackStarted = false;
					xcode_loopback_shutdown(pContext);
				}
#endif
				stop_transcode(pContext);/* stop only */
				break;
#endif
			case 21: /* stop selected xcoder context */
				printf("To stop xcoder%d [immidiate=%d]...\n", pContext->contextId, pContext->bImmediateStop);
#if BTST_ENABLE_XCODE_LOOPBACK /* bringdown loopback path */
				if(g_selectedXcodeContextId == g_loopbackXcodeId && g_loopbackStarted) {
					g_loopbackStarted = false;
					xcode_loopback_shutdown(pContext);
				}
#endif
				/* stop & close transcoder */
				shutdown_transcode(pContext);
				if(g_bAutoQuit) { 
					int i;
					for(i=0; i<NEXUS_NUM_VIDEO_ENCODERS; i++) {
						if(xcodeContext[i].bStarted) break;
					}
					/* when all contexts are stopped, quit the test */
					if(i == NEXUS_NUM_VIDEO_ENCODERS) {
						g_keyReturn = 'q';
						B_Mutex_Unlock(pContext->mutexStarted);
						goto Done;
					}
				}
				break;
			case 24:
				xcode_start_systemdata(pContext);
				break;
			case 25:
				xcode_stop_systemdata(pContext);
				break;
			case 26:
				if(pContext->encodeSettings.bAudioEncode)
				{
					NEXUS_AudioEncoderCodecSettings codecSettings;
					NEXUS_AudioEncoder_GetCodecSettings(pContext->audioEncoder, pContext->encodeSettings.encoderAudioCodec, &codecSettings);
					switch(pContext->encodeSettings.encoderAudioCodec) {
					case NEXUS_AudioCodec_eAac:
						printf("Context%d AAC audio encoder bitrate = %u bps.\nPlease enter the new bitrate:\n", pContext->contextId, codecSettings.codecSettings.aac.bitRate);
						scanf("%u", &codecSettings.codecSettings.aac.bitRate);
						break;
					case NEXUS_AudioCodec_eAacPlus:
					case NEXUS_AudioCodec_eAacPlusAdts:
						printf("Context%d AAC+ audio encoder bitrate = %u bps.\nPlease enter the new bitrate:\n", pContext->contextId, codecSettings.codecSettings.aacPlus.bitRate);
						scanf("%u", &codecSettings.codecSettings.aacPlus.bitRate);
						break;
					case NEXUS_AudioCodec_eMp3:
						printf("Context%d MP3 audio encoder bitrate = %u bps.\nPlease enter the new bitrate:\n", pContext->contextId, codecSettings.codecSettings.mp3.bitRate);
						scanf("%u", &codecSettings.codecSettings.mp3.bitRate);
						break;
					default:
						printf("Unsupported audio encoder codec %d!\n", pContext->encodeSettings.encoderAudioCodec);
						break;
					}
					NEXUS_AudioEncoder_SetCodecSettings(pContext->audioEncoder, &codecSettings);
				}
				else {
					printf("Context%d audio encoder is disabled!\n", pContext->contextId);
				}
				break;
			case 27:
				if(pContext->encodeSettings.bAudioEncode)
				{
					NEXUS_AudioEncoderCodecSettings codecSettings;
					NEXUS_AudioEncoder_GetCodecSettings(pContext->audioEncoder, pContext->encodeSettings.encoderAudioCodec, &codecSettings);
					switch(pContext->encodeSettings.encoderAudioCodec) {
					case NEXUS_AudioCodec_eAac:
						printf("Context%d AAC audio encoder sample rate = %u KHz.\n", pContext->contextId, codecSettings.codecSettings.aac.sampleRate/1000);
						codecSettings.codecSettings.aac.sampleRate = (codecSettings.codecSettings.aac.sampleRate == 32000)? 48000 : 32000;
						printf("changed to %u KHz.\n", codecSettings.codecSettings.aac.sampleRate/1000);
						break;
					case NEXUS_AudioCodec_eAacPlus:
					case NEXUS_AudioCodec_eAacPlusAdts:
						printf("Context%d AAC+ audio encoder sample rate = %u Hz.\n", pContext->contextId, codecSettings.codecSettings.aacPlus.sampleRate);
						codecSettings.codecSettings.aacPlus.sampleRate = (codecSettings.codecSettings.aacPlus.sampleRate == 32000)? 48000 : 32000;
						printf("changed to %u KHz.\n", codecSettings.codecSettings.aacPlus.sampleRate/1000);
						break;
					default:
						printf("Unsupported audio encoder codec %d for sample rate conversion!\n", pContext->encodeSettings.encoderAudioCodec);
						break;
					}
					NEXUS_AudioEncoder_SetCodecSettings(pContext->audioEncoder, &codecSettings);
				}
				else {
					printf("Context%d audio encoder is not enabled! No SRC.\n", pContext->contextId);
				}
				break;
			case 28:
				fprintf(stderr, "Open gfx window 100x100 at (0, 0)...\n");
				open_gfx(pContext);
				break;
			case 29:
				fprintf(stderr, "Close gfx window...\n");
				close_gfx(pContext);
				break;
			case 30:
				fprintf(stderr, "Stop stream mux %d's recpump...\n", pContext->contextId);
				NEXUS_Record_Stop(pContext->record);
				break;
			case 31:
				fprintf(stderr, "Start stream mux %d's recpump...\n", pContext->contextId);
				NEXUS_Record_Start(pContext->record, pContext->fileTranscode);
				break;
			case 50: /* random dynamic resolution change */
#define BTST_MIN_XCODE_RESOLUTION_X 64
#define BTST_MIN_XCODE_RESOLUTION_Y 64
#define BTST_MAX_XCODE_RESOLUTION_X 1920
#define BTST_MAX_XCODE_RESOLUTION_Y 1080
				printf("random resolution change...\n");
				printf("How many times? "); scanf("%d", &choice);
				while(choice--) {
					pContext->encodeSettings.bCustom = true;
					pContext->encodeSettings.customFormatSettings.width = (rand()%(BTST_MAX_XCODE_RESOLUTION_X+1))& (~0xf);/* MB aligned*/
					if(pContext->encodeSettings.customFormatSettings.width < BTST_MIN_XCODE_RESOLUTION_X) 
						pContext->encodeSettings.customFormatSettings.width += BTST_MIN_XCODE_RESOLUTION_X;
					pContext->encodeSettings.customFormatSettings.height = rand()%(BTST_MAX_XCODE_RESOLUTION_Y+1) & (~0xf);
					if(pContext->encodeSettings.customFormatSettings.height < BTST_MIN_XCODE_RESOLUTION_Y) 
						pContext->encodeSettings.customFormatSettings.height += BTST_MIN_XCODE_RESOLUTION_Y;
					pContext->encodeSettings.customFormatSettings.interlaced = false;
					pContext->encodeSettings.customFormatSettings.refreshRate = 30000;
					pContext->encodeSettings.customFormatSettings.aspectRatio = NEXUS_DisplayAspectRatio_eSar;
					pContext->encodeSettings.customFormatSettings.sampleAspectRatio.x = 1;
					pContext->encodeSettings.customFormatSettings.sampleAspectRatio.y = 1;
					NEXUS_Display_GetDefaultCustomFormatSettings(&customFormatSettings);
					customFormatSettings.width = pContext->encodeSettings.customFormatSettings.width;
					customFormatSettings.height = pContext->encodeSettings.customFormatSettings.height;
					customFormatSettings.refreshRate = pContext->encodeSettings.customFormatSettings.refreshRate;
					customFormatSettings.interlaced = pContext->encodeSettings.customFormatSettings.interlaced;
					customFormatSettings.aspectRatio = pContext->encodeSettings.customFormatSettings.aspectRatio;
					customFormatSettings.sampleAspectRatio.x = pContext->encodeSettings.customFormatSettings.sampleAspectRatio.x;
					customFormatSettings.sampleAspectRatio.y = pContext->encodeSettings.customFormatSettings.sampleAspectRatio.y;
					customFormatSettings.dropFrameAllowed = true;
					printf("resolution loop %d: %ux%up30\n", choice, pContext->encodeSettings.customFormatSettings.width, pContext->encodeSettings.customFormatSettings.height);
					rc = NEXUS_Display_SetCustomFormatSettings(pContext->displayTranscode, NEXUS_VideoFormat_eCustom2, &customFormatSettings);
					BDBG_ASSERT(!rc);
					BKNI_Sleep(3000); /* 3sec per loop */
				}
				/* resolution change typically could come with bit rate change; separate for now */
				break;
			case 51: /* random dynamic bitrate change */
#define BTST_MIN_XCODE_BITRATE 10000/* 10Kbps */
#define BTST_MAX_XCODE_BITRATE 30000000 /* 20 Mbps */
				printf("random bitrate change...\n");
				printf("How many times? "); scanf("%d", &choice);
				while(choice--) {
					pContext->encodeSettings.encoderBitrate = rand()%BTST_MAX_XCODE_BITRATE;
					if(pContext->encodeSettings.encoderBitrate < BTST_MIN_XCODE_BITRATE) pContext->encodeSettings.encoderBitrate += BTST_MIN_XCODE_BITRATE;
					NEXUS_VideoEncoder_GetSettings(pContext->videoEncoder, &videoEncoderConfig);
					videoEncoderConfig.bitrateMax = pContext->encodeSettings.encoderBitrate;
					printf("bps loop %d: %u\n", choice, videoEncoderConfig.bitrateMax);
					rc = NEXUS_VideoEncoder_SetSettings(pContext->videoEncoder, &videoEncoderConfig);
					BDBG_ASSERT(!rc);
					BKNI_Sleep(3000); /* 3sec per loop */
				}
				/* resolution change typically could come with bit rate change; separate for now */
				break;
			case 100: 
				printf("\n How many seconds to sleep?");
				scanf("%u", &ulSecToSleep);
				BKNI_Sleep(1000*ulSecToSleep);
				break;
			case 90: /* source trick mode player */
				if(pContext->inputSettings.resource == BTST_RESOURCE_FILE) {
					B_Mutex_Unlock(pContext->mutexStarted);
					sourcePlayer(pContext);
					B_Mutex_Lock(pContext->mutexStarted);
				}
				break;
			case 99: /* debug module setting */
#if BDBG_DEBUG_BUILD
{
				int  iDbgLevel;
				char achName[256];
				printf("\nPlease enter the debug module name: ");
				scanf("%s", achName);
				printf("(%d)Trace (%d)Message (%d)Warning (%d)Error\n",
					BDBG_eTrace, BDBG_eMsg, BDBG_eWrn, BDBG_eErr);
				printf("Which debug level do you want to set it to? ");
				scanf("%d", &iDbgLevel);
				BDBG_SetModuleLevel(achName, (BDBG_Level)iDbgLevel); 
}
#endif				
				break;
			default:
				break;
		}
		B_Mutex_Unlock(pContext->mutexStarted);
	}
Done:
	if(g_eotEvent) {
		B_Event_Set(g_eotEvent);
	}
}

static void message_overflow_callback(void *context, int param)
{
	TranscodeContext  *pContext = (TranscodeContext  *)context;
	printf("#### Context%d message PID[%d] buffer overflows! ###\n", pContext->contextId, param);
}
static void message_psi_callback(void *context, int param)
{
	BSTD_UNUSED(param);
	BKNI_SetEvent((BKNI_EventHandle)context);
}
#define PROGRAM_INFO_LENGTH_OFFSET (TS_PSI_LAST_SECTION_NUMBER_OFFSET+3)
#define PROGRAM_INFO_LENGTH(buf) (TS_READ_16(&buf[PROGRAM_INFO_LENGTH_OFFSET])&0xFFF)
#define DESCRIPTOR_BASE(buf) (&buf[TS_PSI_LAST_SECTION_NUMBER_OFFSET+5])
#define STREAM_BASE(buf) (TS_PSI_LAST_SECTION_NUMBER_OFFSET + 5 + PROGRAM_INFO_LENGTH(buf))

static int TS_PMT_P_getStreamByteOffset( const uint8_t *buf, unsigned bfrSize, int streamNum )
{
	int byteOffset;
	int i;

	/* After the last descriptor */
	byteOffset = STREAM_BASE(buf);

	for (i=0; i < streamNum; i++)
	{
		if (byteOffset >= (int)bfrSize || byteOffset >= TS_PSI_MAX_BYTE_OFFSET(buf))
			return -1;
		byteOffset += 5 + (TS_READ_16( &buf[byteOffset+3] ) & 0xFFF);
	}

	return byteOffset;
}

int add_psi_filter(const void *context, unsigned short pid, BKNI_EventHandle event)
{
	unsigned i;
	NEXUS_MessageSettings settings;
	NEXUS_MessageStartSettings startSettings;
	NEXUS_Error rc;
	psi_message_t *msg = NULL;
	TranscodeContext  *pContext = (TranscodeContext  *)context;

	for (i=0;i<BTST_MAX_MESSAGE_FILTERS;i++) {
	    if (!pContext->psi_message[i].message) {
	        msg = &pContext->psi_message[i];
	        break;
	    }
	}
	if (!msg) {
	    return -1;
	}
	BDBG_MSG(("adding PSI filter[%u] for PID %u", i, pid));
	if(pContext->inputSettings.resource == BTST_RESOURCE_FILE) {
		msg->pidChannel = NEXUS_Playback_OpenPidChannel(pContext->playback, pid, NULL);
	} else {
		msg->pidChannel = NEXUS_PidChannel_Open(NEXUS_ParserBand_e0+pContext->contextId, pid, NULL);
	}
	BDBG_ASSERT(msg->pidChannel);

	NEXUS_Message_GetDefaultSettings(&settings);
	settings.dataReady.callback = message_psi_callback;
	settings.dataReady.context = event;
	msg->message = NEXUS_Message_Open(&settings);
	msg->pid = pid;
	msg->done = false;

	NEXUS_Message_GetDefaultStartSettings(msg->message, &startSettings);
	startSettings.pidChannel = msg->pidChannel;
	rc = NEXUS_Message_Start(msg->message, &startSettings);
	BDBG_ASSERT(!rc);

	return 0;
}

/* get TS user data PSI info from input PMT */
static void getUserDataPsiFromPmt(void *context)
{
	TranscodeContext  *pContext = (TranscodeContext  *)context;
	BKNI_EventHandle event;
	NEXUS_Error rc;
	unsigned i=0, count = 0, loop = 0;
	bool bFound = false;

	/* 1) parse input PAT and set up PMT filters if no specified PMT PID;
	 * 2) parse each PMTs to find one with matching video PID;
	 * 3) match selected user data stream PID;
	 * 4) get user data descriptors;
	 */

	if(pContext->inputSettings.resource == BTST_RESOURCE_FILE) {
		/* assume source playback is not started yet, we need to get PSI about user data */
		NEXUS_Playback_Start(pContext->playback, pContext->file, NULL);
	}

	/* to get input PMT, need to set up message filter */
	BKNI_CreateEvent(&event);

	BDBG_WRN(("starting PSI filter PID = %u", pContext->inputSettings.pmtPid));

	/* if user specified PMT PID, msg[0] is for the PMT; else msg[0] is for PAT; */
	add_psi_filter(pContext, pContext->inputSettings.pmtPid, event);

	/* Read the PAT/PMT */
	for (count=0;;count++) {
		const uint8_t *buffer;
		size_t size;
		unsigned programNum, message_length, streamNum;
		size_t num = NEXUS_MAX_MUX_PIDS;

		if (count == BTST_MAX_MESSAGE_FILTERS) {
			count = 0;
			if(++loop > BTST_MAX_MESSAGE_FILTERS) {BDBG_ERR(("failed to get input user data PMT!")); rc = -1; break;}
		}

		if (!pContext->psi_message[count].message || pContext->psi_message[count].done) {
			continue;
		}

		rc = NEXUS_Message_GetBuffer(pContext->psi_message[count].message, (const void **)&buffer, &size);
		BDBG_ASSERT(!rc);

		if (!size) {
			BERR_Code rc = BKNI_WaitForEvent(event, 5 * 1000); /* wait 5 seconds */
			if (rc == NEXUS_TIMEOUT) {
			    BDBG_WRN(("no PSI messages")); 
			    rc = -1; 
			    break;
			}
			BDBG_ASSERT(!rc);
			continue;
		}

		/* We should always get whole PAT's because maxContiguousMessageSize is 4K */
		message_length = TS_PSI_GET_SECTION_LENGTH(buffer) + 3;
		BDBG_ASSERT(size >= (size_t)message_length);
		BDBG_MSG(("message[%u] size = %d, table ID = %u", count, size, buffer[0]));

		if (buffer[0] == 0) {
			/* 1) Program Association Table */
			fprintf(stderr, "PAT: size=%d\n", message_length);
			for (programNum=0;programNum<(unsigned)(TS_PSI_GET_SECTION_LENGTH(buffer)-7)/4;programNum++) {
			    unsigned byteOffset = 8 + programNum*4;
			    unsigned program = TS_READ_16( &buffer[byteOffset] );
			    unsigned short pid = (uint16_t)(TS_READ_16( &buffer[byteOffset+2] ) & 0x1FFF);
			    fprintf(stderr, "  program %d: PID %u\n", program, pid);
			    /* add PMT filters for all programs */
			    add_psi_filter(pContext, pid, event);
			}
		}
		else if (buffer[0] == 2) { /* PMT */
			TS_PMT_stream pmtStream;

			/* Program Table */
			fprintf(stderr,"Context%d found PMT PID[%u]:\nprogram number %d size=%d\n", pContext->contextId, 
				pContext->psi_message[count].pid, TS_READ_16(&buffer[3]), message_length);
			/* need to validate the PMT section */
			if(!TS_PMT_validate(buffer, size)) {BDBG_ERR(("invalid PMT")); goto Done_getUserDataPsi;}

			streamNum = TS_PMT_getNumStreams(buffer, size);
			fprintf(stderr, "total streams: %d\n", streamNum);

			/* 2) search for all streams to match the video PID if no specified PMT PID */
			if(0 == pContext->inputSettings.pmtPid) {
				for (i=0;i<streamNum;i++) {
				    TS_PMT_getStream(buffer, size, i, &pmtStream);
				    fprintf(stderr, "\tPID: %d, strem_type: %d\n", pmtStream.elementary_PID, pmtStream.stream_type);
				    if(pmtStream.elementary_PID == pContext->inputSettings.iVideoPid) {
				        fprintf(stderr, "Found matching video PID!\n");
				        break;
				    }
				}
				if(i == streamNum)  goto Done_getUserDataPsi; /* not found so continue to next PMT */
			}
			bFound = true;/* found PMT */

			/* else, found the matching program */
			/* 3) search for all streams to extract user data PSI info */
			for (i=0;i<streamNum;i++) {
			    unsigned streamOffset;

			    TS_PMT_getStream(buffer, size, i, &pmtStream);

			    /* 2) match user data PID */
			    if(pContext->inputSettings.numUserDataPids == BTST_TS_USER_DATA_ALL) {
			        /* all pass the VBI user data PES streams */
			        if(pmtStream.stream_type != TS_PSI_ST_13818_1_PrivatePES) continue;
			        if(num == NEXUS_MAX_MUX_PIDS) num = 0;/* start from 0 */
			        else {
			            ++num;
			            if(num >= NEXUS_MAX_MUX_PIDS) break;
			        }
			        pContext->inputSettings.userDataPid[num] = pmtStream.elementary_PID;
			    } else {
			        for(num=0; num < pContext->inputSettings.numUserDataPids; num++) {
			            /* 3) bingo! save the stream info and stream descriptor */
			            if(pmtStream.elementary_PID == (uint16_t)pContext->inputSettings.userDataPid[num]) {
			                break;
			            }
			        }
			        /* not found, check next stream */
			        if(num == pContext->inputSettings.numUserDataPids) continue;
			    }

			    /* 4) save user data PSI info */
			    fprintf(stderr, "\tuser data PID: %d, strem_type: %d\n", pmtStream.elementary_PID, pmtStream.stream_type);
			    /* save pmt stream info and remap PID */
			    pContext->userDataStream[num] = pmtStream;
			    if(pContext->bRemapUserDataPid) {
			        pContext->userDataStream[num].elementary_PID = pContext->inputSettings.remapUserDataPid[num];
			    } else {
			        pContext->inputSettings.remapUserDataPid[num] = pContext->userDataStream[num].elementary_PID;
			    }
			    /* save stream descriptor size */
			    streamOffset = TS_PMT_P_getStreamByteOffset(buffer, size, i);
			    pContext->userDataDescLen[num] = TS_READ_16(&buffer[streamOffset+3])&0x3FF;
			    fprintf(stderr, "\tdescriptor length: %d\n", pContext->userDataDescLen[num]);

			    /* sanity check descriptor size */
			    if(pContext->userDataDescLen[num] > 188) {
			        BDBG_ERR(("User data descriptor length %d too long!"));
			        pContext->bUserDataStreamValid[num] = false;/* invalidate */
			        goto Done_getUserDataPsi;
			    }
			    BKNI_Memcpy(pContext->userDataDescriptors[num],&buffer[streamOffset+5],pContext->userDataDescLen[num]);
			    /* mark it valid finally */
			    pContext->bUserDataStreamValid[num] = true;
			}
			if(pContext->inputSettings.numUserDataPids == BTST_TS_USER_DATA_ALL) {
			    pContext->inputSettings.numUserDataPids = num+1;/* found num of user data streams */
			    BDBG_MSG(("Context%d found %d user data PIDs to pass through.", pContext->contextId, pContext->inputSettings.numUserDataPids));
			}

		}
Done_getUserDataPsi:
		/* XPT HW is configured to pad all messages to 4 bytes. If we are calling NEXUS_Message_ReadComplete
		based on message length and not the size returned by NEXUS_Message_GetBuffer, then we must add that pad.
		If we are wrong, NEXUS_Message_ReadComplete will fail. */
		if (message_length % 4) {
		    message_length += 4 - (message_length % 4);
		}
		/* only complete one PMT */
		rc = NEXUS_Message_ReadComplete(pContext->psi_message[count].message, message_length);
		BDBG_ASSERT(!rc);

		pContext->psi_message[count].done = true; /* don't parse this table any more */

		/* Only do once. TODO: may periodically watch for updated PMT info. */
		if(bFound) break;
	}

	for (i=0;i<BTST_MAX_MESSAGE_FILTERS;i++) {
		if (pContext->psi_message[i].message) {
		    NEXUS_Message_Close(pContext->psi_message[i].message);
		    pContext->psi_message[i].message = NULL;
		}
		if (pContext->psi_message[i].pidChannel) {
		    if(pContext->inputSettings.resource == BTST_RESOURCE_FILE) {
		        NEXUS_Playback_ClosePidChannel(pContext->playback, pContext->psi_message[i].pidChannel);
		    }
		    else {
		        NEXUS_PidChannel_Close(pContext->psi_message[i].pidChannel);
		    }
		    pContext->psi_message[i].pidChannel = NULL;
		}
	}
	/* free the event resource */
	BKNI_DestroyEvent(event);

	if(pContext->inputSettings.resource == BTST_RESOURCE_FILE) {
	    NEXUS_Playback_Stop(pContext->playback);
	}
}

/* TSHDRBUILDER has one extra byte at the beginning to describe the variable length TS header buffer */
#define BTST_TS_HEADER_BUF_LENGTH   189
void add_pat_pmt(
    TranscodeContext *pContext,
    uint16_t pcrPid,
    uint16_t vidPid,
    uint16_t audPid,
    uint8_t  vidStreamType,
    uint8_t  audStreamType
)
{
    uint8_t pat_pl_buf[BTST_TS_HEADER_BUF_LENGTH], pmt_pl_buf[BTST_TS_HEADER_BUF_LENGTH];
    size_t pat_pl_size, pmt_pl_size;
    size_t buf_used = 0;
    size_t payload_pked = 0;
    unsigned streamNum;
    int i;

    TS_PAT_state patState;
    TS_PSI_header psi_header;
    TS_PMT_state pmtState;
    TS_PAT_program program;
    TS_PMT_stream pmt_stream;

    TS_PID_info pidInfo;
    TS_PID_state pidState;

    /* == CREATE PSI TABLES == */
    TS_PSI_header_Init(&psi_header);
    TS_PAT_Init(&patState, &psi_header, pat_pl_buf, BTST_TS_HEADER_BUF_LENGTH);

    TS_PAT_program_Init(&program, 1, BTST_MUX_PMT_PID);
    TS_PAT_addProgram(&patState, &pmtState, &program, pmt_pl_buf, BTST_TS_HEADER_BUF_LENGTH);

    TS_PMT_setPcrPid(&pmtState, pcrPid);

    TS_PMT_stream_Init(&pmt_stream, vidStreamType, vidPid);
    TS_PMT_addStream(&pmtState, &pmt_stream, &streamNum);

    if(audPid)
    {
        TS_PMT_stream_Init(&pmt_stream, audStreamType, audPid);
        TS_PMT_addStream(&pmtState, &pmt_stream, &streamNum);
    }

    /* add user data PID stream PSI info */
    if(pContext->inputSettings.bTsUserDataInput && pContext->inputSettings.resource != BTST_RESOURCE_HDMI) {
        getUserDataPsiFromPmt(pContext);
        for(i=0; i<NEXUS_MAX_MUX_PIDS; i++) {
            if(pContext->bUserDataStreamValid[i]) {
                TS_PMT_addStream(&pmtState, &pContext->userDataStream[i], &streamNum);
                TS_PMT_setDescriptor(&pmtState,
                    pContext->userDataDescriptors[i],
                    pContext->userDataDescLen[i],
                    streamNum);
            }
        }
    }

    TS_PAT_finalize(&patState, &pat_pl_size);
    TS_PMT_finalize(&pmtState, &pmt_pl_size);
    BDBG_MSG(("\nContext%d output PMT section:", pContext->contextId));
    for(i=0; i < (int)pmtState.size; i+=8) {
        BDBG_MSG(("%02x %02x %02x %02x %02x %02x %02x %02x", pmtState.buf[i], pmtState.buf[i+1], pmtState.buf[i+2], pmtState.buf[i+3],
            pmtState.buf[i+4], pmtState.buf[i+5], pmtState.buf[i+6], pmtState.buf[i+7]));
    }

    /* == CREATE TS HEADERS FOR PSI INFORMATION == */
    TS_PID_info_Init(&pidInfo, 0x0, 1);
    pidInfo.pointer_field = 1;
    TS_PID_state_Init(&pidState);
    TS_buildTSHeader(&pidInfo, &pidState, pContext->pat[0], BTST_TS_HEADER_BUF_LENGTH, &buf_used, patState.size, &payload_pked, 1);
    BKNI_Memcpy((uint8_t*)pContext->pat[0] + buf_used, pat_pl_buf, pat_pl_size);

    TS_PID_info_Init(&pidInfo, BTST_MUX_PMT_PID, 1);
    pidInfo.pointer_field = 1;
    TS_PID_state_Init(&pidState);
    TS_buildTSHeader(&pidInfo, &pidState, pContext->pmt[0], BTST_TS_HEADER_BUF_LENGTH, &buf_used, pmtState.size, &payload_pked, 1);
    BKNI_Memcpy((uint8_t*)pContext->pmt[0] + buf_used, pmt_pl_buf, pmt_pl_size);
    BDBG_MSG(("\nContext%d output PMT packet:", pContext->contextId));
    for(i=0; i < BTST_TS_HEADER_BUF_LENGTH; i+=8) {
        BDBG_MSG(("%02x %02x %02x %02x %02x %02x %02x %02x", 
            *((uint8_t*)pContext->pmt[0]+i), *((uint8_t*)pContext->pmt[0]+i+1), *((uint8_t*)pContext->pmt[0]+i+2), *((uint8_t*)pContext->pmt[0]+i+3),
            *((uint8_t*)pContext->pmt[0]+i+4), *((uint8_t*)pContext->pmt[0]+i+5), *((uint8_t*)pContext->pmt[0]+i+6), *((uint8_t*)pContext->pmt[0]+i+7)));
    }

}

static void insertSystemDataTimer(void *context)
{
	TranscodeContext *pContext = context;
	uint8_t ccByte;

	++pContext->ccValue;/* increment CC synchronously with PAT/PMT */
	ccByte = *((uint8_t*)pContext->pat[pContext->ccValue % BTST_PSI_QUEUE_CNT] + 4); /* the 1st byte of pat/pmt arrays is for TSheader builder use */

	/* need to increment CC value for PAT/PMT packets */
	ccByte = (ccByte & 0xf0) | (pContext->ccValue & 0xf);
	*((uint8_t*)pContext->pat[pContext->ccValue % BTST_PSI_QUEUE_CNT] + 4) = ccByte;
	/* need to flush the cache before set to TS mux hw */
	NEXUS_Memory_FlushCache((void*)((uint8_t*)pContext->pat[pContext->ccValue % BTST_PSI_QUEUE_CNT] + 4), 1);
	/* ping pong PAT pointer */
	pContext->psi[0].pData = (void*)((uint8_t*)pContext->pat[pContext->ccValue % BTST_PSI_QUEUE_CNT] + 1);

	ccByte = *((uint8_t*)pContext->pmt[pContext->ccValue % BTST_PSI_QUEUE_CNT] + 4);
	ccByte = (ccByte & 0xf0) | (pContext->ccValue & 0xf);
	*((uint8_t*)pContext->pmt[pContext->ccValue % BTST_PSI_QUEUE_CNT] + 4) = ccByte;
	NEXUS_Memory_FlushCache((void*)((uint8_t*)pContext->pmt[pContext->ccValue % BTST_PSI_QUEUE_CNT] + 4), 1);
	/* ping pong PMT pointer */
	pContext->psi[1].pData = (void*)((uint8_t*)pContext->pmt[pContext->ccValue % BTST_PSI_QUEUE_CNT] + 1);

	NEXUS_StreamMux_AddSystemDataBuffer(pContext->streamMux, &pContext->psi[0]);
	NEXUS_StreamMux_AddSystemDataBuffer(pContext->streamMux, &pContext->psi[1]);
	BDBG_MODULE_MSG(contcounter,("Context%d insert PAT&PMT... ccPAT = %x ccPMT=%x", pContext->contextId, *((uint8_t*)pContext->pat[pContext->ccValue % BTST_PSI_QUEUE_CNT] + 4) & 0xf,
		*((uint8_t*)pContext->pmt[pContext->ccValue  % BTST_PSI_QUEUE_CNT] + 4) & 0xf));
	if(pContext->systemdataTimerIsStarted)
	{
		pContext->systemdataTimer = B_Scheduler_StartTimer(
			pContext->schedulerSystemdata,pContext->mutexSystemdata, 1000, insertSystemDataTimer, pContext);
		if(pContext->systemdataTimer==NULL) {BDBG_ERR(("schedule timer error %d", NEXUS_OUT_OF_SYSTEM_MEMORY));}
	}
	return;
}

/*******************************
 * Add system data to stream_mux
 */
static void xcode_create_systemdata( TranscodeContext  *pContext )
{
	uint8_t vidStreamType, audStreamType;
	uint16_t audPid = 0;
	unsigned i;
	NEXUS_AudioCodec audCodec = NEXUS_AudioCodec_eUnknown;

	for(i=0; i<BTST_PSI_QUEUE_CNT; i++)
	{
		NEXUS_Memory_Allocate(BTST_TS_HEADER_BUF_LENGTH, NULL, &pContext->pat[i]);
		NEXUS_Memory_Allocate(BTST_TS_HEADER_BUF_LENGTH, NULL, &pContext->pmt[i]);
	}

	/* decide the stream type to set in PMT */
	switch(pContext->encodeSettings.encoderVideoCodec)
	{
		case NEXUS_VideoCodec_eMpeg2:         vidStreamType = 0x2; break;
		case NEXUS_VideoCodec_eMpeg4Part2:    vidStreamType = 0x10; break;
		case NEXUS_VideoCodec_eH264:          vidStreamType = 0x1b; break;
		case NEXUS_VideoCodec_eVc1SimpleMain: vidStreamType = 0xea; break;
		default:
			BDBG_ERR(("Video encoder codec %d is not supported!\n", pContext->encodeSettings.encoderVideoCodec));
			BDBG_ASSERT(0);
	}
	if(pContext->encodeSettings.bAudioEncode)
	{/* audio transcode */
		audCodec = pContext->encodeSettings.encoderAudioCodec;
		audPid = BTST_MUX_AUDIO_PID;
	}
	else if(pContext->inputSettings.bAudioInput)
	{/* audio passthrough */
		audCodec = pContext->inputSettings.eAudioCodec;
		audPid = BTST_MUX_AUDIO_PID;
	}
	if(pContext->inputSettings.bAudioInput)
	{
		switch(audCodec)
		{
			case NEXUS_AudioCodec_eMpeg:         audStreamType = 0x4; break;
			case NEXUS_AudioCodec_eMp3:          audStreamType = 0x4; break;
			case NEXUS_AudioCodec_eAac    :      audStreamType = 0xf; break; /* ADTS */
			case NEXUS_AudioCodec_eAacPlus:      audStreamType = 0x11; break;/* LOAS */
			/* MP2TS doesn't allow 14496-3 AAC+ADTS; here is placeholder to test AAC-HE before LOAS encode is supported; */
			case NEXUS_AudioCodec_eAacPlusAdts:  audStreamType = 0x11; break;
			case NEXUS_AudioCodec_eAc3:          audStreamType = 0x81; break;
			default:
				BDBG_ERR(("Audio encoder codec %d is not supported!\n", audCodec));
		}
	}

	add_pat_pmt(pContext, BTST_MUX_PCR_PID, BTST_MUX_VIDEO_PID, audPid, vidStreamType, audStreamType);
	for(i=0; i<BTST_PSI_QUEUE_CNT; i++)
	{
		if(i > 0)
		{
			BKNI_Memcpy(pContext->pat[i], pContext->pat[0], BTST_TS_HEADER_BUF_LENGTH);
			BKNI_Memcpy(pContext->pmt[i], pContext->pmt[0], BTST_TS_HEADER_BUF_LENGTH);
		}
		NEXUS_Memory_FlushCache(pContext->pat[i], BTST_TS_HEADER_BUF_LENGTH);
		NEXUS_Memory_FlushCache(pContext->pmt[i], BTST_TS_HEADER_BUF_LENGTH);
	}
	BKNI_Memset(pContext->psi, 0, sizeof(pContext->psi));
	pContext->psi[0].size = 188;
	/* TSHDRBUILDER has one extra byte at the beginning to describe the variable length TS header buffer */
	pContext->psi[0].pData = (void*)((uint8_t*)pContext->pat[0] + 1);
	pContext->psi[0].timestampDelta = 0;
	pContext->psi[1].size = 188;
	/* TSHDRBUILDER has one extra byte at the beginning to describe the variable length TS header buffer */
	pContext->psi[1].pData = (void*)((uint8_t*)pContext->pmt[0] + 1);
	pContext->psi[1].timestampDelta = 0;
	NEXUS_StreamMux_AddSystemDataBuffer(pContext->streamMux, &pContext->psi[0]);
	NEXUS_StreamMux_AddSystemDataBuffer(pContext->streamMux, &pContext->psi[1]);
	BDBG_MSG(("insert PAT&PMT... ccPAT = %x ccPMT=%x", *((uint8_t*)pContext->pat[0] + 4) & 0xf,
		*((uint8_t*)pContext->pmt[0] + 4) & 0xf));

}

static void xcode_start_systemdata( TranscodeContext  *pContext )
{
	if(!pContext->systemdataTimerIsStarted) {
		/* schedule a periodic timer to insert PAT/PMT */
		B_ThreadSettings settingsThread;
		pContext->mutexSystemdata = B_Mutex_Create(NULL);
		pContext->schedulerSystemdata = B_Scheduler_Create(NULL);
		/* create thread to run scheduler */
		B_Thread_GetDefaultSettings(&settingsThread);
		pContext->schedulerThread = B_Thread_Create("systemdata_Scheduler", 
			(B_ThreadFunc)B_Scheduler_Run, 
			pContext->schedulerSystemdata, 
			&settingsThread);
		if (NULL == pContext->schedulerThread)
		{
			BDBG_ERR(("failed to create scheduler thread"));
		}
		pContext->systemdataTimer = B_Scheduler_StartTimer(
			pContext->schedulerSystemdata,pContext->mutexSystemdata, 1000, insertSystemDataTimer, pContext);
		if(pContext->systemdataTimer==NULL) {BDBG_ERR(("schedule timer error"));}
		pContext->systemdataTimerIsStarted = true;
	}
}

static void xcode_stop_systemdata( TranscodeContext  *pContext )
{
	/* cancel system data timer */
	if(pContext->systemdataTimerIsStarted)
	{
		B_Scheduler_CancelTimer(pContext->schedulerSystemdata, pContext->systemdataTimer);
		B_Scheduler_Stop(pContext->schedulerSystemdata);
		B_Scheduler_Destroy(pContext->schedulerSystemdata);
		if (pContext->schedulerThread)
		{
			B_Thread_Destroy(pContext->schedulerThread);
			pContext->schedulerThread = NULL;
		}
		B_Mutex_Destroy(pContext->mutexSystemdata);
		pContext->systemdataTimer = NULL;
		pContext->systemdataTimerIsStarted = false;
	}
}

static void xcode_release_systemdata( TranscodeContext  *pContext )
{
	unsigned i;

	for(i=0; i<BTST_PSI_QUEUE_CNT; i++)
	{
		NEXUS_Memory_Free(pContext->pat[i]);
		NEXUS_Memory_Free(pContext->pmt[i]);
	}
}

/************************************
 * Set up encoder AV sync.
 * encode setting and startSetting to be set after end-to-end delay is determined */
/* get end-to-end delay (Dee) for audio and video encoders;
* TODO: match AV delay! In other words,
*   if (aDee > vDee) {
*       vDee' = aDee' = aDee;
*   }
*   else {
*       vDee' = aDee' = vDee;
*   }
*/
static void xcode_av_sync( 
	TranscodeContext  *pContext,
	NEXUS_VideoEncoderSettings *pVideoEncoderConfig,
	NEXUS_VideoEncoderStartSettings *pVideoEncoderStartConfig )
{
	NEXUS_VideoEncoderDelayRange videoDelay;
	NEXUS_AudioMuxOutputDelayStatus audioDelayStatus;
	NEXUS_AudioMuxOutputStartSettings audioMuxStartSettings;
	unsigned Dee;

	/******************************************
	 * add configurable delay to video path 
	 */
	/* disable Inverse Telecine Field Pairing for extreme low delay mode
	 * NOTE: ITFP is encoder feature to detect and lock on 3:2/2:2 cadence in the video content to help 
	 * efficient coding for interlaced formats; disabling ITFP will impact the bit efficiency but reduce the encode delay. */
#ifdef NEXUS_NUM_DSP_VIDEO_ENCODERS
	pVideoEncoderStartConfig->bounds.inputDimension.max.width = BTST_INPUT_MAX_WIDTH;
	pVideoEncoderStartConfig->bounds.inputDimension.max.height = BTST_INPUT_MAX_HEIGHT;
#else	
	if(pContext->bCustomizeDelay)
	{
		pVideoEncoderConfig->enableFieldPairing = pContext->encodeSettings.enableFieldPairing;

		/* 0 to use default 750ms rate buffer delay; TODO: allow user to adjust it to lower encode delay at cost of quality reduction! */
		pVideoEncoderStartConfig->rateBufferDelay = pContext->encodeSettings.rateBufferDelay;

		/* to allow 23.976p passthru; TODO: allow user to configure minimum framerate to achieve lower delay!
		 * Note: lower minimum framerate means longer encode delay */
		pVideoEncoderStartConfig->bounds.inputFrameRate.min = pContext->encodeSettings.inputFrameRateMin;

		/* to allow 15 ~ 60p dynamic frame rate coding TODO: allow user to config higher minimum frame rate for lower delay! */
		pVideoEncoderStartConfig->bounds.outputFrameRate.min = pContext->encodeSettings.outputFrameRateMin;
		pVideoEncoderStartConfig->bounds.outputFrameRate.max = NEXUS_VideoFrameRate_e60;

		/* max encode size allows 1080p encode; TODO: allow user to choose lower max resolution for lower encode delay */
		pVideoEncoderStartConfig->bounds.inputDimension.max.width = pContext->encodeSettings.inputMaxWidth;
		pVideoEncoderStartConfig->bounds.inputDimension.max.height = pContext->encodeSettings.inputMaxHeight;
	}
	else
	{
		pVideoEncoderConfig->enableFieldPairing = true;

		/* 0 to use default 750ms rate buffer delay; TODO: allow user to adjust it to lower encode delay at cost of quality reduction! */
		pVideoEncoderStartConfig->rateBufferDelay = 0;

		/* to allow 23.976p passthru; TODO: allow user to configure minimum framerate to achieve lower delay!
		 * Note: lower minimum framerate means longer encode delay */
		pVideoEncoderStartConfig->bounds.inputFrameRate.min = NEXUS_VideoFrameRate_e23_976;

		/* to allow 15 ~ 60p dynamic frame rate coding TODO: allow user to config higher minimum frame rate for lower delay! */
		pVideoEncoderStartConfig->bounds.outputFrameRate.min = NEXUS_VideoFrameRate_e14_985;
		pVideoEncoderStartConfig->bounds.outputFrameRate.max = NEXUS_VideoFrameRate_e60;

		/* max encode size allows 1080p encode; TODO: allow user to choose lower max resolution for lower encode delay */
		pVideoEncoderStartConfig->bounds.inputDimension.max.width = 1920;
		pVideoEncoderStartConfig->bounds.inputDimension.max.height = 1088;
	}
#endif
	/*****************************************
	 * calculate video encoder A2P delay 
	 */
	/* NOTE: video encoder delay is in 27MHz ticks */
	NEXUS_VideoEncoder_GetDelayRange(pContext->videoEncoder, pVideoEncoderConfig, pVideoEncoderStartConfig, &videoDelay);
	BDBG_WRN(("\n\tVideo encoder end-to-end delay = [%u ~ %u] ms", videoDelay.min/27000, videoDelay.max/27000));

	if(pContext->inputSettings.bAudioInput)
	{
		NEXUS_AudioMuxOutput_GetDelayStatus(pContext->audioMuxOutput, pContext->encodeSettings.encoderAudioCodec, &audioDelayStatus);
		BDBG_WRN(("\n\tAudio codec %d end-to-end delay = %u ms", pContext->encodeSettings.encoderAudioCodec, audioDelayStatus.endToEndDelay));

		Dee = audioDelayStatus.endToEndDelay * 27000; /* in 27MHz ticks */
		if(Dee > videoDelay.min)
		{
			if(Dee > videoDelay.max)
			{
				BDBG_ERR(("Audio Dee is way too big! Use video Dee max!"));
				Dee = videoDelay.max;
			}
			else
			{
				BDBG_WRN(("Use audio Dee %u ms %u ticks@27Mhz!", Dee/27000, Dee));
			}
		}
		else
		{
			Dee = videoDelay.min;
			BDBG_WRN(("Use video Dee %u ms %u ticks@27Mhz!", Dee/27000, Dee));
		}
		pVideoEncoderConfig->encoderDelay = Dee;

		/* Start audio mux output */
		NEXUS_AudioMuxOutput_GetDefaultStartSettings(&audioMuxStartSettings);
		/* audio NRT requires mux out to take NRT decode STC */
		audioMuxStartSettings.stcChannel        = pContext->bNonRealTime?
			pContext->stcAudioChannel : pContext->stcChannelTranscode;
		audioMuxStartSettings.presentationDelay = Dee/27000;/* in ms */
		audioMuxStartSettings.nonRealTime       = pContext->bNonRealTime;
		pContext->audMuxStartSettings = audioMuxStartSettings;
	}
	else
		pVideoEncoderConfig->encoderDelay = videoDelay.min;
}

static void xcode_index_filename(char *indexFile, const char *mediaFile )
{
	char *tmpPtr;
	strncpy(indexFile, mediaFile, strlen(mediaFile));
	tmpPtr = strstr(indexFile, ".");
	if (tmpPtr) {
	    strncpy(tmpPtr+1, "nav", 3);
	}
	else {
	    strncat(indexFile, ".nav", sizeof(indexFile)-1);
	}
	BDBG_MSG(("Media file name: %s, index file name %s", mediaFile, indexFile));
}

/* file i/o page size */
#define BTST_RECORD_PAGE_SIZE        (4096)
#define BTST_RECORD_ATOM_SIZE        (47*BTST_RECORD_PAGE_SIZE)
/* nexus_record limits the per file write transaction size */
#define BTST_RECORD_WRITE_SIZE_LIMIT (3*BTST_RECORD_ATOM_SIZE)

/*******************************
 * Set up stream_mux and record
 */
static void xcode_setup_mux_record( TranscodeContext  *pContext )
{
	NEXUS_StreamMuxCreateSettings muxCreateSettings;
	NEXUS_StreamMuxStartSettings muxConfig;
	NEXUS_PlaypumpOpenSettings playpumpConfig;
	NEXUS_RecpumpOpenSettings recpumpOpenSettings;
	NEXUS_RecordSettings recordSettings;
	NEXUS_MessageSettings messageSettings;
	NEXUS_MessageStartSettings messageStartSettings;
	NEXUS_PlaybackPidChannelSettings playbackPidSettings;
	size_t i;


	NEXUS_Playpump_GetDefaultOpenSettings(&playpumpConfig);
	playpumpConfig.fifoSize = 16384; /* reduce FIFO size allocated for playpump */
	playpumpConfig.numDescriptors = 64; /* set number of descriptors */
	playpumpConfig.streamMuxCompatible = true;
	pContext->playpumpTranscodeVideo = NEXUS_Playpump_Open(BTST_STREAM_MUX_VIDEO_PLAYPUMP_IDX+pContext->contextId*4, &playpumpConfig);
	assert(pContext->playpumpTranscodeVideo);
	BDBG_MSG(("Transcoder%d opened TS mux video PES playpump%d [%p].", pContext->contextId, BTST_STREAM_MUX_VIDEO_PLAYPUMP_IDX+pContext->contextId*4, pContext->playpumpTranscodeVideo));
#if BTST_DISABLE_VIDEO_ENCODER_PES_PACING
	NEXUS_Playpump_SuspendPacing(pContext->playpumpTranscodeVideo, true);
#endif

	pContext->playpumpTranscodePcr = NEXUS_Playpump_Open(BTST_STREAM_MUX_PCR_PLAYPUMP_IDX+pContext->contextId*4, &playpumpConfig);
	BDBG_MSG(("Transcoder%d opened TS mux PCR playpump%d [%p].", pContext->contextId, BTST_STREAM_MUX_PCR_PLAYPUMP_IDX+pContext->contextId*4, pContext->playpumpTranscodePcr));
	assert(pContext->playpumpTranscodePcr);

	BKNI_CreateEvent(&pContext->finishEvent);
	NEXUS_StreamMux_GetDefaultCreateSettings(&muxCreateSettings);
	muxCreateSettings.finished.callback = transcoderFinishCallback;
	muxCreateSettings.finished.context = pContext->finishEvent;
	pContext->streamMux = NEXUS_StreamMux_Create(&muxCreateSettings);
	NEXUS_StreamMux_GetDefaultStartSettings(&muxConfig);
	muxConfig.transportType = NEXUS_TransportType_eTs;
	muxConfig.stcChannel = pContext->stcChannelTranscode;
	muxConfig.nonRealTime = pContext->bNonRealTime;
	muxConfig.nonRealTimeRate = g_nrtRate * NEXUS_NORMAL_PLAY_SPEED; /* AFAP */
	muxConfig.servicePeriod = g_msp; /* MSP */

	muxConfig.video[0].pid = BTST_MUX_VIDEO_PID;
	muxConfig.video[0].encoder = pContext->videoEncoder;
	muxConfig.video[0].playpump = pContext->playpumpTranscodeVideo;

	if(pContext->inputSettings.bAudioInput)
	{
		/* audio playpump here is for ts muxer */
		NEXUS_Playpump_GetDefaultOpenSettings(&playpumpConfig);
		playpumpConfig.fifoSize = 16384; /* reduce FIFO size allocated for playpump */
		playpumpConfig.numDescriptors = 64; /* set number of descriptors */
		playpumpConfig.streamMuxCompatible = true;
		pContext->playpumpTranscodeAudio = NEXUS_Playpump_Open(BTST_STREAM_MUX_AUDIO_PLAYPUMP_IDX+pContext->contextId*4, &playpumpConfig);
		assert(pContext->playpumpTranscodeAudio);
		BDBG_MSG(("Transcoder%d opened TS mux audio PES playpump%d [%p].", pContext->contextId, BTST_STREAM_MUX_AUDIO_PLAYPUMP_IDX+pContext->contextId*4, pContext->playpumpTranscodeAudio));

		muxConfig.audio[0].pid = BTST_MUX_AUDIO_PID;
		muxConfig.audio[0].muxOutput = pContext->audioMuxOutput;
		muxConfig.audio[0].playpump = pContext->playpumpTranscodeAudio;
	}

	muxConfig.pcr.pid = BTST_MUX_PCR_PID;
	muxConfig.pcr.playpump = pContext->playpumpTranscodePcr;
	muxConfig.pcr.interval = 50;

	/******************************************
	 * Set up xcoder record output
	 */
	NEXUS_Recpump_GetDefaultOpenSettings(&recpumpOpenSettings);
	/******************************************
	 *  NRT workaround for the XPT band hold and data ready events sharing the same threshold: avoid band hold from occuring,
	 * otherwise, video stutter would happen!
	 * 1) The nexus_record timer fires at 250 ms interval to service record data as bottom line;
	 * 2) whenever nexus record timer fires, it'll consume up to Nx(3x(47x4096)) cdb data;
	 * 3) so if file i/o can keep up, band hold threshold = 2x(3x(47x4096)) = 1.1MB can sustain record bit rate up to
	 *       2 * 3 * 188K * 8 / 250ms = 36 Mbps without reaching band hold;
	 * 4) larger band hold threshold can sustain higher record bit rate throughput.
	 * NOTE: after SW7425-1663, recpump data ready threshold is decoupled with RAVE upper (band-hold) threshold, so
	 * we do not need to mess with data ready threshold any more!
	 */
	BDBG_MSG(("To open recpump with dataReadyThreshold = %d indexReadyThreshold=%d", 
		recpumpOpenSettings.data.dataReadyThreshold, recpumpOpenSettings.index.dataReadyThreshold));
	BDBG_MSG(("        recpump with data fifo size     = %d index fifo size    =%d", 
		recpumpOpenSettings.data.bufferSize, recpumpOpenSettings.index.bufferSize));
	pContext->recpump = NEXUS_Recpump_Open(pContext->contextId, &recpumpOpenSettings);
	assert(pContext->recpump);
	BDBG_MSG(("Transcoder%d opened TS mux recpump%d [%p].", pContext->contextId, pContext->contextId, pContext->recpump));

	pContext->record = NEXUS_Record_Create();
	assert(pContext->record);

	NEXUS_Record_GetSettings(pContext->record, &recordSettings);
	recordSettings.recpump = pContext->recpump;
	/* NOTE: enable band hold to allow recpump to stall TS mux to avoid ave data corruption in case file i/o is slow
	 * The encoders should make sure no output CDB/ITB buffer corruption (instead do frame drop) when output overflow! */
	recordSettings.recpumpSettings.bandHold = NEXUS_RecpumpFlowControl_eEnable;
	recordSettings.pollingTimer = 50;
	if(pContext->bNonRealTime) {/* write all data for NRT */
		recordSettings.writeAllTimeout = 100;
	}
	NEXUS_Record_SetSettings(pContext->record, &recordSettings);

	/*******************************
	 * create system data PAT/PMT
	 */
	xcode_create_systemdata(pContext);

	/*******************************
	 *  TS user data pass through setup
	 */
	if(pContext->inputSettings.bTsUserDataInput && pContext->inputSettings.resource != BTST_RESOURCE_HDMI) {
		NEXUS_Message_GetDefaultSettings(&messageSettings);
		/* SCTE 270 spec max TS VBI user data bitrate=270Kbps, 256KB buffer can hold 7.5 seconds;
		   worthy user data for video synchronization; TODO: may be reduced if unnecessary */
		messageSettings.bufferSize = 256*1024;
		messageSettings.maxContiguousMessageSize = 0; /* to support TS capture and in-place operation */
		messageSettings.overflow.callback = message_overflow_callback; /* report overflow error */
		messageSettings.overflow.context  = pContext;

		/* open source user data PID channels */
		NEXUS_Playback_GetDefaultPidChannelSettings(&playbackPidSettings);
		playbackPidSettings.pidSettings.pidType = NEXUS_PidType_eOther; /* capture the TS packets with the user data PES */
		playbackPidSettings.pidSettings.pidSettings.pidChannelIndex = NEXUS_PID_CHANNEL_OPEN_MESSAGE_CAPABLE;

		BDBG_ASSERT(NEXUS_MAX_MUX_PIDS >= pContext->inputSettings.numUserDataPids);
		for(i = 0; i < pContext->inputSettings.numUserDataPids; i++) {
			if(pContext->bUserDataStreamValid[i]) {
				messageSettings.overflow.param = pContext->inputSettings.remapUserDataPid[i];
				BDBG_MSG(("context%d opened message buffer for user data PID %d remapped %d", pContext->contextId, 
					pContext->inputSettings.userDataPid[i], pContext->inputSettings.remapUserDataPid[i]));
				muxConfig.userdata[i].message = NEXUS_Message_Open(&messageSettings);
				BDBG_ASSERT(muxConfig.userdata[i].message);
				pContext->userDataMessage[i] = muxConfig.userdata[i].message;

				if(pContext->bRemapUserDataPid) {
					playbackPidSettings.pidSettings.pidSettings.remap.enabled = true;
					playbackPidSettings.pidSettings.pidSettings.remap.pid     = pContext->inputSettings.remapUserDataPid[i];/* optional PID remap */
				}
				pContext->pidChannelUserData[i] = NEXUS_Playback_OpenPidChannel(pContext->playback, 
					pContext->inputSettings.userDataPid[i], &playbackPidSettings);
				BDBG_ASSERT(pContext->pidChannelUserData[i]);

				/* must start message before stream mux starts */
				NEXUS_Message_GetDefaultStartSettings(muxConfig.userdata[i].message, &messageStartSettings);
				messageStartSettings.format = NEXUS_MessageFormat_eTs;
				messageStartSettings.pidChannel = pContext->pidChannelUserData[i];
				NEXUS_Message_Start(muxConfig.userdata[i].message, &messageStartSettings);

				/* open transcode mux output user data PidChannels out of system data channel */
				pContext->pidChannelTranscodeUserData[i] = NEXUS_Playpump_OpenPidChannel(pContext->playpumpTranscodePcr, 
					pContext->inputSettings.remapUserDataPid[i], NULL);
				BDBG_ASSERT(pContext->pidChannelTranscodeUserData[i]);
			}
		}
	}

	/* store the mux config */
	pContext->muxConfig = muxConfig;

	/* open PidChannels */
	pContext->pidChannelTranscodePcr = NEXUS_Playpump_OpenPidChannel(pContext->playpumpTranscodePcr, muxConfig.pcr.pid, NULL);
	assert(pContext->pidChannelTranscodePcr);
	pContext->pidChannelTranscodePmt = NEXUS_Playpump_OpenPidChannel(pContext->playpumpTranscodePcr, BTST_MUX_PMT_PID, NULL);
	assert(pContext->pidChannelTranscodePmt);
	pContext->pidChannelTranscodePat = NEXUS_Playpump_OpenPidChannel(pContext->playpumpTranscodePcr, BTST_MUX_PAT_PID, NULL);
	assert(pContext->pidChannelTranscodePat);

	/* set record index file name and open the record file handle */
	if((pContext->encodeSettings.encoderVideoCodec==NEXUS_VideoCodec_eMpeg2) ||
	   (pContext->encodeSettings.encoderVideoCodec==NEXUS_VideoCodec_eH264))
	{
		xcode_index_filename(pContext->indexfname, pContext->encodeSettings.fname);
	}
	else BDBG_WRN(("no index record"));

}

#if BDBG_DEBUG_BUILD
static const struct {
    double frequency;
    NEXUS_VideoFrameRate nexusFramerate;
} verticalfrequency[NEXUS_VideoFrameRate_eMax] = {
    {0, NEXUS_VideoFrameRate_eUnknown},
    {23.976, NEXUS_VideoFrameRate_e23_976},
    {24, NEXUS_VideoFrameRate_e24},
    {25, NEXUS_VideoFrameRate_e25},
    {29.970, NEXUS_VideoFrameRate_e29_97},
    {30, NEXUS_VideoFrameRate_e30},
    {50, NEXUS_VideoFrameRate_e50},
    {59.940, NEXUS_VideoFrameRate_e59_94},
    {60, NEXUS_VideoFrameRate_e60},
    {14.985, NEXUS_VideoFrameRate_e14_985}};

static double convert_RefreshRate_FromFrameRate(NEXUS_VideoFrameRate frameRate)
{
    unsigned i;
    for(i=0;i<sizeof(verticalfrequency)/sizeof(*verticalfrequency);i++) {
        if (frameRate == verticalfrequency[i].nexusFramerate) {
            return verticalfrequency[i].frequency;
        }
    }
    BERR_TRACE(NEXUS_NOT_SUPPORTED);
    return 0; /* NEXUS_VideoFrameRate_eUnknown */
}
#endif

#if NEXUS_HAS_HDMI_INPUT
static uint8_t SampleEDID[] = 
{
	0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x08, 0x6D, 0x74, 0x22, 0x05, 0x01, 0x11, 0x20,
	0x00, 0x14, 0x01, 0x03, 0x80, 0x00, 0x00, 0x78, 0x0A, 0xDA, 0xFF, 0xA3, 0x58, 0x4A, 0xA2, 0x29,
	0x17, 0x49, 0x4B, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x02, 0x3A, 0x80, 0x18, 0x71, 0x38, 0x2D, 0x40, 0x58, 0x2C,
	0x45, 0x00, 0xBA, 0x88, 0x21, 0x00, 0x00, 0x1E, 0x01, 0x1D, 0x80, 0x18, 0x71, 0x1C, 0x16, 0x20,
	0x58, 0x2C, 0x25, 0x00, 0xBA, 0x88, 0x21, 0x00, 0x00, 0x9E, 0x00, 0x00, 0x00, 0xFC, 0x00, 0x42,
	0x43, 0x4D, 0x37, 0x34, 0x32, 0x32, 0x2F, 0x37, 0x34, 0x32, 0x35, 0x0A, 0x00, 0x00, 0x00, 0xFD,
	0x00, 0x17, 0x3D, 0x0F, 0x44, 0x0F, 0x00, 0x0A, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x01, 0x89,

	0x02, 0x03, 0x3C, 0x71, 0x7F, 0x03, 0x0C, 0x00, 0x40, 0x00, 0xB8, 0x2D, 0x2F, 0x80, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0xE3, 0x05, 0x1F, 0x01, 0x49, 0x90, 0x05, 0x20, 0x04, 0x03, 0x02, 0x07,
	0x06, 0x01, 0x29, 0x09, 0x07, 0x01, 0x11, 0x07, 0x00, 0x15, 0x07, 0x00, 0x01, 0x1D, 0x00, 0x72,
	0x51, 0xD0, 0x1E, 0x20, 0x6E, 0x28, 0x55, 0x00, 0xBA, 0x88, 0x21, 0x00, 0x00, 0x1E, 0x8C, 0x0A,
	0xD0, 0x8A, 0x20, 0xE0, 0x2D, 0x10, 0x10, 0x3E, 0x96, 0x00, 0xBA, 0x88, 0x21, 0x00, 0x00, 0x18,
	0x8C, 0x0A, 0xD0, 0x8A, 0x20, 0xE0, 0x2D, 0x10, 0x10, 0x3E, 0x96, 0x00, 0x0B, 0x88, 0x21, 0x00,
	0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x9D
};

static void
avInfoFrameChangedCallback(void *context, int param)
{
	NEXUS_HdmiInputStatus status;
	NEXUS_HdmiInputHandle hdmiInput = (NEXUS_HdmiInputHandle)context;

	BSTD_UNUSED(param);
	NEXUS_HdmiInput_GetStatus(hdmiInput, &status);
	BDBG_WRN(("HDMI Source AV InfoFrame Change callback: video format %ux%u@%.3f%c", 
		status.avWidth,
		status.avHeight,
		(double)status.vertFreq/BFMT_FREQ_FACTOR,
		status.interlaced? 'i' : 'p'));
}
#endif

static void
vidSrcStreamChangedCallback(void *context, int param)
{
	NEXUS_VideoDecoderStreamInformation streamInfo;
	NEXUS_VideoDecoderHandle decoder = (NEXUS_VideoDecoderHandle)context;

	BSTD_UNUSED(param);
	NEXUS_VideoDecoder_GetStreamInformation(decoder, &streamInfo);
	BDBG_WRN(("Video Source Stream Change callback: %ux%u@%.3f%c", 
		streamInfo.sourceHorizontalSize,
		streamInfo.sourceVerticalSize,
		convert_RefreshRate_FromFrameRate(streamInfo.frameRate),
		streamInfo.streamProgressive? 'p' : 'i'));
}

static void play_endOfStreamCallback(void *context, int param)
{
	TranscodeContext  *pContext = (TranscodeContext  *)context;
	BSTD_UNUSED(param);

	printf("Context%d endOfStream\n", pContext->contextId);
	if(pContext->bNonRealTime && !g_bNonRealTimeWrap)
	{
		/* terminate the NRT context */
		B_Event_Set(pContext->eofEvent);
	}
	return;
}

static void nrt_endOfStreamHandler(void *context)
{
	int i;
	TranscodeContext  *pContext = (TranscodeContext  *)context;

	while(B_Event_Wait(pContext->eofEvent, B_WAIT_FOREVER)!=B_ERROR_SUCCESS);
	fprintf(stderr, "Context%d NRT mode ending...\n", pContext->contextId);

	/* stop the NRT context at EOS event */
	B_Mutex_Lock(pContext->mutexStarted);
	if(pContext->bStarted) {
#if BTST_ENABLE_XCODE_LOOPBACK
		if(g_loopbackStarted && (g_loopbackXcodeId == pContext->contextId)) {
			fprintf(stderr, "NRT mode EOS to stop xcoder%d loopback...\n", g_loopbackXcodeId);
			xcode_loopback_shutdown(pContext);
			g_loopbackStarted = false;
		}
#endif
		shutdown_transcode(pContext);
		BDBG_WRN(("NRT context%d completes.", pContext->contextId));
	}
	B_Mutex_Unlock(pContext->mutexStarted);
	if(g_bAutoQuit) { /* when all contexts are stopped, quit automatically */
		for(i=0; i<NEXUS_NUM_VIDEO_ENCODERS; i++) {
			if(xcodeContext[i].bStarted) return;
		}
		if(g_eotEvent) {
			g_keyReturn = 'q';
			B_Event_Set(g_eotEvent);
		}
	}
	return;
}

static int open_transcode(
	TranscodeContext  *pContext )
{
#if NEXUS_HAS_FRONTEND
	NEXUS_FrontendUserParameters userParams;
	NEXUS_FrontendQamSettings qamSettings;
	NEXUS_ParserBandSettings parserBandSettings;
	NEXUS_ParserBand parserBand;
#endif
	NEXUS_StcChannelSettings stcSettings;
	NEXUS_VideoDecoderStartSettings videoProgram;
	NEXUS_VideoDecoderSettings videoDecodeSettings;
	NEXUS_VideoEncoderSettings videoEncoderConfig;
	NEXUS_VideoEncoderOpenSettings videoEncoderOpenSettings;
	NEXUS_VideoEncoderStartSettings videoEncoderStartConfig;

	NEXUS_AudioDecoderStartSettings audioProgram;
	NEXUS_AudioEncoderSettings encoderSettings;
	NEXUS_AudioCodec audioCodec;

	NEXUS_PlaybackSettings playbackSettings;
	NEXUS_PlaybackPidChannelSettings playbackPidSettings;

#if NEXUS_HAS_HDMI_INPUT
	NEXUS_HdmiInputSettings hdmiInputSettings;
	NEXUS_TimebaseSettings timebaseSettings;
#endif

#if BTST_ENABLE_AV_SYNC
	NEXUS_SyncChannelSettings syncChannelSettings;
#if BTST_ENABLE_NRT_STC_AV_WINDOW
	NEXUS_StcChannelPairSettings stcAudioVideoPair;
#endif
#endif

#ifndef NEXUS_NUM_DSP_VIDEO_ENCODERS
	NEXUS_DisplayStgSettings stgSettings;
#endif
	NEXUS_DisplaySettings displaySettings;
	NEXUS_VideoWindowSettings windowSettings;
	NEXUS_VideoWindowScalerSettings scalerSettings;
	NEXUS_VideoFormatInfo fmtInfo;
	bool bInterlacedXcode;
	NEXUS_DisplayCustomFormatSettings customFormatSettings;
	NEXUS_Error rc;
	EncodeSettings  *pEncodeSettings = &pContext->encodeSettings;
	InputSettings   *pInputSettings = &pContext->inputSettings; 

	if(pContext->videoEncoder) {
		return 0;
	}

#if NEXUS_HAS_HDMI_INPUT
	if(pInputSettings->resource == BTST_RESOURCE_HDMI) {
		NEXUS_Timebase_GetSettings(NEXUS_Timebase_e0+pContext->contextId, &timebaseSettings);
		timebaseSettings.sourceType = NEXUS_TimebaseSourceType_eHdDviIn;
		NEXUS_Timebase_SetSettings(NEXUS_Timebase_e0+pContext->contextId, &timebaseSettings);
		BDBG_MSG(("Transcoder%d set timebase %d.", pContext->contextId, NEXUS_Timebase_e0+pContext->contextId));

		NEXUS_HdmiInput_GetDefaultSettings(&hdmiInputSettings);
		hdmiInputSettings.timebase = NEXUS_Timebase_e0+pContext->contextId;
		/* use NEXUS_HdmiInput_OpenWithEdid ()
		    if EDID PROM (U1304 and U1305) is NOT installed; 
		    reference boards usually have the PROMs installed.
		    this example assumes Port1 EDID has been removed 
		*/

		/* all HDMI Tx/Rx combo chips have EDID RAM */
		hdmiInputSettings.useInternalEdid = true ;
		pContext->hdmiInput = NEXUS_HdmiInput_OpenWithEdid(0, &hdmiInputSettings, 
		    &SampleEDID[0], (uint16_t) sizeof(SampleEDID));
		if(!pContext->hdmiInput) {
			fprintf(stderr, "Can't get hdmi input\n");
			return -1;
		}
		NEXUS_HdmiInput_GetSettings(pContext->hdmiInput, &hdmiInputSettings);
		hdmiInputSettings.aviInfoFrameChanged.callback = avInfoFrameChangedCallback;
		hdmiInputSettings.aviInfoFrameChanged.context  = pContext->hdmiInput;
		NEXUS_HdmiInput_SetSettings(pContext->hdmiInput, &hdmiInputSettings);
	}
	else /* TODO: add sync channel support for hdmi source */
#endif
	{
#if BTST_ENABLE_AV_SYNC
		/* create a sync channel */
		NEXUS_SyncChannel_GetDefaultSettings(&syncChannelSettings);
		pContext->syncChannel = NEXUS_SyncChannel_Create(&syncChannelSettings);
#endif
	}

	/* bring up decoder and connect to local display */
	if(pInputSettings->resource != BTST_RESOURCE_HDMI) {
		NEXUS_VideoDecoderOpenSettings openSettings;
		NEXUS_VideoDecoder_GetDefaultOpenSettings(&openSettings);
		openSettings.avc51Enabled = false;
		openSettings.svc3dSupported = false;
		openSettings.excessDirModeEnabled = false;
		openSettings.enhancementPidChannelSupported = false;
		pContext->videoDecoder = NEXUS_VideoDecoder_Open(BTST_XCODE_VID_DECODE_IDX-pContext->contextId, &openSettings); /* take default capabilities */
		BDBG_MSG(("Transcoder%d opened source decoder %d.", pContext->contextId, BTST_XCODE_VID_DECODE_IDX-pContext->contextId));
		NEXUS_VideoDecoder_GetSettings(pContext->videoDecoder, &videoDecodeSettings);
		videoDecodeSettings.streamChanged.callback = vidSrcStreamChangedCallback;
		videoDecodeSettings.streamChanged.context  = pContext->videoDecoder;
		BKNI_Memset(videoDecodeSettings.supportedCodecs, 0, sizeof(videoDecodeSettings.supportedCodecs));
		videoDecodeSettings.supportedCodecs[0] = NEXUS_VideoCodec_eH264; /* it's for regular HD decode heap allocation; it covers mpeg2/h264/mpeg4 HD size */
		if(pContext->bEncodeCCUserData) {/* to log source user data */
			char fname[256];
			int idx = BTST_P_DecoderId_eSource0 + pContext->contextId;

			videoDecodeSettings.userDataEnabled = true;
			videoDecodeSettings.appUserDataReady.callback = userdataCallback;
			g_decoderContext[idx].videoDecoder = pContext->videoDecoder;
			g_decoderContext[idx].codec = pInputSettings->eVideoCodec;
			sprintf(fname, "userdata_%s_608.csv", g_userDataDecoderName[idx]);
			g_decoderContext[idx].output608.fLog        = fopen(fname, "wb");
			g_decoderContext[idx].output608.bInit       = true;
			g_decoderContext[idx].output608.bFilterNull = true;
			sprintf(fname, "userdata_%s_708.csv", g_userDataDecoderName[idx]);
			g_decoderContext[idx].output708.fLog = fopen(fname, "wb");
			g_decoderContext[idx].output708.bInit       = true;
			g_decoderContext[idx].output708.bFilterNull = true;
			videoDecodeSettings.appUserDataReady.context  = &g_decoderContext[idx];
			videoDecodeSettings.appUserDataReady.param    = idx;
		}
		videoDecodeSettings.channelChangeMode = NEXUS_VideoDecoder_ChannelChangeMode_eMuteUntilFirstPicture;
		NEXUS_VideoDecoder_SetSettings(pContext->videoDecoder, &videoDecodeSettings);

		/****************************************
		 * set up xcoder source
		 */
#if NEXUS_HAS_FRONTEND
		if(pInputSettings->resource == BTST_RESOURCE_QAM)
		{
			NEXUS_FrontendCapabilities capabilities;
			BDBG_ASSERT(pContext->contextId < NEXUS_MAX_FRONTENDS);
			pContext->frontend = g_platformConfig.frontend[pContext->contextId];
			if (pContext->frontend) {
				NEXUS_Frontend_GetCapabilities(pContext->frontend, &capabilities);
				/* Does this frontend support qam? */
				if ( !capabilities.qam )
				{
					fprintf(stderr, "This platform doesn't support QAM frontend!\n");
					return -1;
				}
			}
			
			NEXUS_Frontend_GetDefaultQamSettings(&qamSettings);
			qamSettings.frequency = pInputSettings->freq* 1000000;
			qamSettings.mode = pInputSettings->qamMode;
			switch (pInputSettings->qamMode) {
			case NEXUS_FrontendQamMode_e64: qamSettings.symbolRate = 5056900; break;
			case NEXUS_FrontendQamMode_e256: qamSettings.symbolRate = 5360537; break;
			case NEXUS_FrontendQamMode_e1024: qamSettings.symbolRate = 0; /* TODO */ break;
			default: fprintf(stderr, "Invalid QAM mode!\n"); return -1;
			}
			qamSettings.annex = NEXUS_FrontendQamAnnex_eB;
			qamSettings.bandwidth = NEXUS_FrontendQamBandwidth_e6Mhz;
			qamSettings.lockCallback.callback = lock_callback;
			qamSettings.lockCallback.context = pContext->frontend;
			
			NEXUS_Frontend_GetUserParameters(pContext->frontend, &userParams);
			
			/* Map a parser band to the demod's input band. */
			parserBand = NEXUS_ParserBand_e0+pContext->contextId;
			NEXUS_ParserBand_GetSettings(parserBand, &parserBandSettings);
			if (userParams.isMtsif) {
				parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eMtsif;
				parserBandSettings.sourceTypeSettings.mtsif = NEXUS_Frontend_GetConnector(pContext->frontend); /* NEXUS_Frontend_TuneXyz() will connect this frontend to this parser band */
			}
			else {
				parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
				parserBandSettings.sourceTypeSettings.inputBand = userParams.param1;  /* Platform initializes this to input band */
			}
			parserBandSettings.transportType = NEXUS_TransportType_eTs;
			NEXUS_ParserBand_SetSettings(parserBand, &parserBandSettings);
			pContext->videoPidChannel = NEXUS_PidChannel_Open(parserBand, pInputSettings->iVideoPid, NULL);
			BDBG_MSG(("Transcoder%d opened PID channel for parser band %d.", pContext->contextId, parserBand));

			/* each live transcoder should only use 2 STCs on the same timebase: 
			       1) audio/video decode STC; (RT)
			       2) encode/mux STC;  (RT)
			   NOTE: to avoid the 3rd one since NRT mode doesn't really need it, 
			         encoder/mux STC is only required in RT mode; and in RT mode, a/v decoders share the same STC.
			 */
			NEXUS_StcChannel_GetDefaultSettings(NEXUS_ANY_ID, &stcSettings);
			stcSettings.timebase = NEXUS_Timebase_e0+pContext->contextId;
			/* Tuner input has live PCR locked timebase */
			stcSettings.mode = NEXUS_StcChannelMode_ePcr; /* live */
			stcSettings.modeSettings.pcr.pidChannel = pContext->videoPidChannel; /* PCR happens to be on video pid */
			pContext->stcVideoChannel = NEXUS_StcChannel_Open(NEXUS_ANY_ID, &stcSettings);
			BDBG_MSG(("Transcoder%d opened source vSTC [%p].", pContext->contextId, pContext->stcVideoChannel));

			pContext->stcAudioChannel = pContext->stcVideoChannel;
			BDBG_MSG(("Transcoder%d has source aSTC [%p].", pContext->contextId, pContext->stcAudioChannel));

			/* encoders/mux require different STC broadcast mode from decoder for RT mode; NRT mode can share decoder STC */
			NEXUS_StcChannel_GetDefaultSettings(NEXUS_ANY_ID, &stcSettings);
			stcSettings.timebase = NEXUS_Timebase_e0+pContext->contextId;/* should be the same timebase for end-to-end locking */
			stcSettings.mode = NEXUS_StcChannelMode_eAuto;/* for encoder&mux, only timebase matters */
			stcSettings.pcrBits = NEXUS_StcChannel_PcrBits_eFull42;/* ViCE2 requires 42-bit STC broadcast */
			pContext->stcChannelTranscode = NEXUS_StcChannel_Open(NEXUS_ANY_ID, &stcSettings);
			BDBG_MSG(("Transcoder%d opened encoder STC [%p].", pContext->contextId, pContext->stcChannelTranscode));

			NEXUS_Frontend_TuneQam(pContext->frontend, &qamSettings);
		}
#else
		if(pInputSettings->resource == BTST_RESOURCE_QAM)
        {
			BDBG_ERR(("QAM Inout not supported"));
			BKNI_Fail();
		}
#endif
		else
		{
			char srcIndexFileName[256] = {'\0',};
			FILE *indexFile;
			/* 4 playpumps per transcoder context: 1x src + 3x mux playpumps; */
			pContext->playpump = NEXUS_Playpump_Open(BTST_SOURCE_PLAYPUMP_IDX+pContext->contextId*4, NULL);
			assert(pContext->playpump);
			BDBG_MSG(("Transcoder%d opened source playpump %d.", pContext->contextId, BTST_SOURCE_PLAYPUMP_IDX+pContext->contextId*4));
			pContext->playback = NEXUS_Playback_Create();
			assert(pContext->playback);

			xcode_index_filename(srcIndexFileName, pInputSettings->fname);
			indexFile = fopen(srcIndexFileName, "r");
			if (!indexFile) {
				fprintf(stderr, "Source file %s supports limited trick mode without index.\n", pInputSettings->fname);
				srcIndexFileName[0] = '\0';
			} else {
				fclose(indexFile);
			}

			pContext->file = NEXUS_FilePlay_OpenPosix(pInputSettings->fname, srcIndexFileName[0]? srcIndexFileName : NULL);
			if (!pContext->file) {
				fprintf(stderr, "can't open file:%s\n", pInputSettings->fname);
				return -1;
			}

			/* file source could run in RT or NRT mode.
			       each transcoder should only use 2 STCs on the same timebase: 
			       1) NRT video decode STC; (RT/NRT)
			       2) NRT audio decode STC; (NRT)
			       3) encode/mux STC;  (RT)
			   NOTE: encoder/mux STC is only required in RT mode; and in RT mode, a/v decoders share the same STC.
			 */
			NEXUS_StcChannel_GetDefaultSettings(NEXUS_ANY_ID, &stcSettings);
			stcSettings.timebase = NEXUS_Timebase_e0+pContext->contextId;
			stcSettings.mode = NEXUS_StcChannelMode_eAuto;
			pContext->stcVideoChannel = NEXUS_StcChannel_Open(NEXUS_ANY_ID, &stcSettings);
			BDBG_MSG(("Transcoder%d opened source vSTC [%p].", pContext->contextId, pContext->stcVideoChannel));

			/* NRT mode uses separate STCs for audio and video decoders; */
			pContext->stcAudioChannel = (pContext->bNonRealTime && pInputSettings->bAudioInput)?
				NEXUS_StcChannel_Open(NEXUS_ANY_ID, &stcSettings) :
				pContext->stcVideoChannel;
			BDBG_MSG(("Transcoder%d opened source aSTC [%p].", pContext->contextId, pContext->stcAudioChannel));

			/* encoders/mux require different STC broadcast mode from decoder for RT mode; NRT mode can share decoder STC */
			if(pContext->bNonRealTime && pInputSettings->bAudioInput) {
				pContext->stcChannelTranscode = pContext->stcAudioChannel;
			} else {
				NEXUS_StcChannel_GetDefaultSettings(NEXUS_ANY_ID, &stcSettings);
				stcSettings.timebase = NEXUS_Timebase_e0+pContext->contextId;/* should be the same timebase for end-to-end locking */
				stcSettings.mode = NEXUS_StcChannelMode_eAuto;/* for encoder&mux, only timebase matters */
				stcSettings.pcrBits = NEXUS_StcChannel_PcrBits_eFull42;/* ViCE2 requires 42-bit STC broadcast */
				pContext->stcChannelTranscode = NEXUS_StcChannel_Open(NEXUS_ANY_ID, &stcSettings);
			}
			BDBG_MSG(("Transcoder%d opened encoder STC [%p].", pContext->contextId, pContext->stcChannelTranscode));
			
			NEXUS_Playback_GetSettings(pContext->playback, &playbackSettings);
			playbackSettings.playpump = pContext->playpump;
			/* set a stream format, it could be any audio video transport type or file format, i.e NEXUS_TransportType_eMp4, NEXUS_TransportType_eAvi ... */
			playbackSettings.playpumpSettings.transportType = (NEXUS_TransportType)pInputSettings->eStreamType;
			playbackSettings.stcChannel = pContext->bNonRealTime? 
				NULL : /* NRT mode won't use playback stc channel; set to NULL! */
				pContext->stcVideoChannel; /* RT mode playback shares the sasme STC channel as a/v decoders */

			/* NRT mode file transcode doesn not need loop */
			playbackSettings.endOfStreamAction = (pContext->bNonRealTime && !g_bNonRealTimeWrap)? NEXUS_PlaybackLoopMode_ePause : NEXUS_PlaybackLoopMode_eLoop; /* when play hits the end, wait for record */
			playbackSettings.endOfStreamCallback.callback = play_endOfStreamCallback;
			playbackSettings.endOfStreamCallback.context  = pContext;
			NEXUS_Playback_SetSettings(pContext->playback, &playbackSettings);

			/* Open the video pid channel */
			NEXUS_Playback_GetDefaultPidChannelSettings(&playbackPidSettings);
			/***************************************************************
			 * NOTE: NRT mode doesn't require source trick mode, so disable video pid setting
			 *  to allow NULL STC channel for playback!!! otherwise NRT video stalls */
			if(!pContext->bNonRealTime) {
				playbackPidSettings.pidSettings.pidType = NEXUS_PidType_eVideo;
				playbackPidSettings.pidTypeSettings.video.codec = (NEXUS_VideoCodec)pInputSettings->eVideoCodec; /* must be told codec for correct handling */
				playbackPidSettings.pidTypeSettings.video.index = true;
				playbackPidSettings.pidTypeSettings.video.decoder = pContext->videoDecoder;
			}
			pContext->videoPidChannel = NEXUS_Playback_OpenPidChannel(pContext->playback, pInputSettings->iVideoPid, &playbackPidSettings);
		}

		/* Set up decoder Start structures now. We need to know the audio codec to properly set up
		the audio outputs. */
		NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram);
		videoProgram.codec = (NEXUS_VideoCodec)pInputSettings->eVideoCodec;
		videoProgram.pidChannel = pContext->videoPidChannel;
		videoProgram.stcChannel = pContext->stcVideoChannel;
		videoProgram.nonRealTime = pContext->bNonRealTime;
		pContext->vidProgram = videoProgram;
	} else {/* HDMI input */
		/* HDMI source runs in RT mode.
		       each transcoder should only use 1 STCs on the same timebase: 
		       1) encode/mux STC;  (RT)
		   NOTE: encoder/mux STC is only required in RT mode; and for sloppiness, open the other stc to simplify close.
		 */
		NEXUS_StcChannel_GetDefaultSettings(NEXUS_ANY_ID, &stcSettings);
		stcSettings.timebase = NEXUS_Timebase_e0+pContext->contextId;
		stcSettings.mode = NEXUS_StcChannelMode_eAuto;
		pContext->stcVideoChannel = NEXUS_StcChannel_Open(NEXUS_ANY_ID, &stcSettings);
		BDBG_MSG(("Transcoder%d opened source vSTC [%p].", pContext->contextId, pContext->stcVideoChannel));

		pContext->stcAudioChannel = pContext->stcVideoChannel;
		BDBG_MSG(("Transcoder%d has source aSTC [%p].", pContext->contextId, pContext->stcAudioChannel));

		/* encoders/mux require different STC broadcast mode from decoder for RT mode; NRT mode can share decoder STC */
		NEXUS_StcChannel_GetDefaultSettings(NEXUS_ANY_ID, &stcSettings);
		stcSettings.timebase = NEXUS_Timebase_e0+pContext->contextId;/* should be the same timebase for end-to-end locking */
		stcSettings.mode = NEXUS_StcChannelMode_eAuto;/* for encoder&mux, only timebase matters */
		stcSettings.pcrBits = NEXUS_StcChannel_PcrBits_eFull42;/* ViCE2 requires 42-bit STC broadcast */
		pContext->stcChannelTranscode = NEXUS_StcChannel_Open(NEXUS_ANY_ID, &stcSettings);
		BDBG_MSG(("Transcoder%d opened encoder STC [%p].", pContext->contextId, pContext->stcChannelTranscode));
	}

	/****************************************
	 * Bring up video display and outputs 
	 */
	if(pContext->contextId == 0) {/* only simul display the 1st context source for now */
		NEXUS_Display_GetDefaultSettings(&displaySettings);
		displaySettings.format = 
			(pEncodeSettings->encoderFrameRate == NEXUS_VideoFrameRate_e25 ||
			 pEncodeSettings->encoderFrameRate == NEXUS_VideoFrameRate_e50) ?
			NEXUS_VideoFormat_ePal : NEXUS_VideoFormat_eNtsc;
		pContext->display = NEXUS_Display_Open(BTST_SOURCE_DISPLAY_IDX, &displaySettings);
		BDBG_MSG(("Transcoder%d opened display%d [%p]", pContext->contextId, BTST_SOURCE_DISPLAY_IDX, pContext->display));
#if NEXUS_NUM_COMPOSITE_OUTPUTS /* CVBS display from transcoder 0's source */
		if(g_platformConfig.outputs.composite[0] && (0==pContext->contextId)){
			NEXUS_Display_AddOutput(pContext->display, NEXUS_CompositeOutput_GetConnector(g_platformConfig.outputs.composite[0]));
		}
#endif
		pContext->window = NEXUS_VideoWindow_Open(pContext->display, 0);

#ifdef NEXUS_NUM_DSP_VIDEO_ENCODERS
		window_mad(pContext->window, false, false, false);
#endif
	}

	/* NOTE: must open video encoder before display; otherwise open will init ViCE2 core
	* which might cause encoder display GISB error since encoder display would
	* trigger RDC to program mailbox registers in ViCE2;
	*/
	NEXUS_VideoEncoder_GetDefaultOpenSettings(&videoEncoderOpenSettings);
	videoEncoderOpenSettings.type = g_eVideoEncoderType;
	pContext->videoEncoder = NEXUS_VideoEncoder_Open(pContext->contextId, &videoEncoderOpenSettings);
	assert(pContext->videoEncoder);
	BDBG_MSG(("Transcoder%d opened video encoder %d.", pContext->contextId, pContext->contextId));

	/* Bring up video encoder display */
	NEXUS_Display_GetDefaultSettings(&displaySettings);
	
#ifndef NEXUS_NUM_DSP_VIDEO_ENCODERS
	displaySettings.displayType = NEXUS_DisplayType_eAuto;
	displaySettings.timingGenerator = NEXUS_DisplayTimingGenerator_eEncoder;
	displaySettings.frameRateMaster = NULL;/* disable frame rate tracking for now */
#endif

	if(!pEncodeSettings->bCustom)
	{
		displaySettings.format = (NEXUS_VideoFormat)pEncodeSettings->displayFormat;
		pContext->displayTranscode = NEXUS_Display_Open(g_encodeDisplayId[pContext->contextId], &displaySettings);
		assert(pContext->displayTranscode);
		BDBG_MSG(("Transcoder%d opened encoder display%d [%p].", pContext->contextId, g_encodeDisplayId[pContext->contextId], pContext->displayTranscode));
		pContext->windowTranscode = NEXUS_VideoWindow_Open(pContext->displayTranscode, 0);
		assert(pContext->windowTranscode);
		NEXUS_VideoFormat_GetInfo(displaySettings.format, &fmtInfo);
		bInterlacedXcode = fmtInfo.interlaced;
	}
	else
	{
		pContext->displayTranscode = NEXUS_Display_Open(g_encodeDisplayId[pContext->contextId], &displaySettings);
		assert(pContext->displayTranscode);
		BDBG_MSG(("Transcoder%d opened encoder display%d [%p].", pContext->contextId, g_encodeDisplayId[pContext->contextId], pContext->displayTranscode));
		pContext->windowTranscode = NEXUS_VideoWindow_Open(pContext->displayTranscode, 0);
		assert(pContext->windowTranscode);

		NEXUS_Display_GetDefaultCustomFormatSettings(&customFormatSettings);
		customFormatSettings.width = pEncodeSettings->customFormatSettings.width;
		customFormatSettings.height = pEncodeSettings->customFormatSettings.height;
		customFormatSettings.refreshRate = pEncodeSettings->customFormatSettings.refreshRate;
		customFormatSettings.interlaced = pEncodeSettings->customFormatSettings.interlaced;
		customFormatSettings.aspectRatio = pEncodeSettings->customFormatSettings.aspectRatio;
		customFormatSettings.sampleAspectRatio.x = pEncodeSettings->customFormatSettings.sampleAspectRatio.x;
		customFormatSettings.sampleAspectRatio.y = pEncodeSettings->customFormatSettings.sampleAspectRatio.y;
		customFormatSettings.dropFrameAllowed = true;
		rc = NEXUS_Display_SetCustomFormatSettings(pContext->displayTranscode, NEXUS_VideoFormat_eCustom2, &customFormatSettings);
		assert(!rc);
		bInterlacedXcode = customFormatSettings.interlaced;
	}

#ifndef NEXUS_NUM_DSP_VIDEO_ENCODERS
	/* NRT setup - AFAP mode */
	NEXUS_Display_GetStgSettings(pContext->displayTranscode, &stgSettings);
	stgSettings.enabled     = true;
	stgSettings.nonRealTime = pContext->bNonRealTime;
	NEXUS_Display_SetStgSettings(pContext->displayTranscode, &stgSettings);
#endif

	NEXUS_VideoWindow_GetSettings(pContext->windowTranscode, &windowSettings);
#ifdef NEXUS_NUM_DSP_VIDEO_ENCODERS
	windowSettings.position.width = BTST_INPUT_MAX_WIDTH;
	windowSettings.position.height = BTST_INPUT_MAX_HEIGHT;
	windowSettings.pixelFormat = NEXUS_PixelFormat_eCr8_Y18_Cb8_Y08;    
	windowSettings.visible = false;
#else 
	/* set transcoder minimum display format before AddInput to avoid black frame transition during dynamic resolution change */	
	windowSettings.minimumDisplayFormat = NEXUS_VideoFormat_e1080p;
	windowSettings.scaleFactorRounding.enabled = false;
	windowSettings.scaleFactorRounding.horizontalTolerance = 0;
	windowSettings.scaleFactorRounding.verticalTolerance   = 0;	
#endif
	NEXUS_VideoWindow_SetSettings(pContext->windowTranscode, &windowSettings);

	/* set transcoder window vnet mode bias to avoid black frame transition during dynamic resolution change for mfd source;
	   NOTE: HDMI input might dynamically switch to 1080p60 and xcode BVN path doesn't have bandwidth to capture 1080p60! */
	if(pInputSettings->resource != BTST_RESOURCE_HDMI) {
		NEXUS_VideoWindow_GetScalerSettings(pContext->windowTranscode, &scalerSettings);
#ifdef NEXUS_NUM_DSP_VIDEO_ENCODERS
		scalerSettings.bandwidthEquationParams.bias = NEXUS_ScalerCaptureBias_eScalerBeforeCapture;
#else		
		scalerSettings.bandwidthEquationParams.bias = NEXUS_ScalerCaptureBias_eScalerAfterCapture;
#endif
		scalerSettings.bandwidthEquationParams.delta = 1*1000*1000;
		NEXUS_VideoWindow_SetScalerSettings(pContext->windowTranscode, &scalerSettings);
	}


	/* connect same decoder to the encoder display;
	* NOTE: simul display + transcode mode might have limitation in audio path;
	* here is for video transcode bringup/debug purpose;
	*/
	if(pInputSettings->resource == BTST_RESOURCE_HDMI) {
#if NEXUS_HAS_HDMI_INPUT
		NEXUS_VideoInput videoInput = NEXUS_HdmiInput_GetVideoConnector(pContext->hdmiInput);
		NEXUS_VideoWindow_AddInput(pContext->windowTranscode, videoInput);
#endif
	}
	else
	{
		NEXUS_VideoWindow_AddInput(pContext->windowTranscode, NEXUS_VideoDecoder_GetConnector(pContext->videoDecoder));
	}

#ifndef NEXUS_NUM_DSP_VIDEO_ENCODERS
	/* xcode window PQ default setting */
	window_dnr(pContext->windowTranscode, false);
#endif
	/* VDC madr assignment is fixed */
	window_mad(pContext->windowTranscode, false, true, pContext->bLowDelay?true:false);

	/* temporarily disable simul display for NRT mode since the nexus sync channel tried to add vsync delay to balance two BVN path delays
	   which caused forced capture on NRT window that's not right; 
	   TODO: remove this when sync channel is fixed for NRT mode. */
#if BTST_DISABLE_NRT_DEBUG_DISPLAY
	if(!pContext->bNonRealTime)
#endif
	{
		if(pContext->contextId == 0) {/* only simul display the 1st context source for now */
			if(pInputSettings->resource == BTST_RESOURCE_HDMI) {
#if NEXUS_HAS_HDMI_INPUT
				NEXUS_VideoWindow_AddInput(pContext->window, NEXUS_HdmiInput_GetVideoConnector(pContext->hdmiInput));
#endif
			}
			else {
				NEXUS_VideoWindow_AddInput(pContext->window, NEXUS_VideoDecoder_GetConnector(pContext->videoDecoder));
			}
		}
	}
#if !BTST_DISABLE_NRT_DEBUG_DISPLAY /* SW workaround for simul debug display path with NRT mode transcode */
	if(pContext->bNonRealTime && (pContext->contextId == 0)) /* crop debug display to force the BVN eop order so that STG eop always fire later than mfd and simul capture path. */
	{
		NEXUS_VideoWindow_GetSettings(pContext->window, &windowSettings);
		windowSettings.sourceClip.bottom = 10; /* source cropping on debug monitor path may work around early EOP trigger issue */
		windowSettings.scaleFactorRounding.enabled = false;/* disable SFR to avoid it cancelling the source crop */
		NEXUS_VideoWindow_SetSettings(pContext->window, &windowSettings);
	}
#endif

	/**************************************
	 * encoder settings 
	 */
	NEXUS_VideoEncoder_GetSettings(pContext->videoEncoder, &videoEncoderConfig);
	videoEncoderConfig.variableFrameRate = false;
	videoEncoderConfig.frameRate = pEncodeSettings->encoderFrameRate;
	videoEncoderConfig.bitrateMax = pEncodeSettings->encoderBitrate;
	videoEncoderConfig.streamStructure.framesP = pEncodeSettings->encoderGopStructureFramesP;
	videoEncoderConfig.streamStructure.framesB = pEncodeSettings->encoderGopStructureFramesB;

	NEXUS_VideoEncoder_GetDefaultStartSettings(&videoEncoderStartConfig);
	videoEncoderStartConfig.codec = pEncodeSettings->encoderVideoCodec;
	videoEncoderStartConfig.profile = pEncodeSettings->encoderProfile;
	videoEncoderStartConfig.level = pEncodeSettings->encoderLevel;
	videoEncoderStartConfig.input = pContext->displayTranscode;
	videoEncoderStartConfig.stcChannel = pContext->stcChannelTranscode;
	videoEncoderStartConfig.interlaced = bInterlacedXcode;
	videoEncoderStartConfig.nonRealTime = pContext->bNonRealTime;
	videoEncoderStartConfig.lowDelayPipeline = pContext->bLowDelay;
	videoEncoderStartConfig.encodeUserData = pContext->bEncodeCCUserData;

	/*********************************
	 * Set up audio xcoder 
	 */
	if(pInputSettings->bAudioInput)
	{
		NEXUS_AudioMixerSettings audioMixerSettings;
		NEXUS_AudioDecoderOpenSettings audioDecoderOpenSettings;

		/* Open the audio decoder */
		/* Connect audio decoders to outputs */
		NEXUS_AudioDecoder_GetDefaultOpenSettings(&audioDecoderOpenSettings);
		audioDecoderOpenSettings.dspIndex    = pContext->contextId / BTST_NUM_AUDIO_XCODE_PER_DSP;
		if(g_bMultiChanAudio) {
			audioDecoderOpenSettings.multichannelFormat = pContext->inputSettings.eMultiChanFmt;
		}
		pContext->audioDecoder = NEXUS_AudioDecoder_Open(pContext->contextId, &audioDecoderOpenSettings);
		if(g_bSecondAudio) {
			pContext->secondaryDecoder = NEXUS_AudioDecoder_Open(pContext->contextId + NEXUS_NUM_VIDEO_ENCODERS, &audioDecoderOpenSettings);
		}

		/* Open the audio and pcr pid channel */
#if NEXUS_HAS_FRONTEND
		if(pInputSettings->resource == BTST_RESOURCE_QAM)
		{
			pContext->audioPidChannel = NEXUS_PidChannel_Open(parserBand, pInputSettings->iAudioPid, NULL);
			if(g_bSecondAudio) {
				pContext->secondaryPidChannel = NEXUS_PidChannel_Open(parserBand, pInputSettings->iSecondaryPid, NULL);
			}
		}
		else
#endif
		if(pInputSettings->resource == BTST_RESOURCE_FILE)
		{
			NEXUS_Playback_GetDefaultPidChannelSettings(&playbackPidSettings);
			playbackPidSettings.pidSettings.pidType = NEXUS_PidType_eAudio;
			playbackPidSettings.pidTypeSettings.audio.primary = pContext->audioDecoder;
			if(g_bSecondAudio) {
				playbackPidSettings.pidTypeSettings.audio.secondary = pContext->secondaryDecoder;
			}
			pContext->audioPidChannel = NEXUS_Playback_OpenPidChannel(pContext->playback, pInputSettings->iAudioPid, &playbackPidSettings);
			if(g_bSecondAudio) {
				pContext->secondaryPidChannel = NEXUS_Playback_OpenPidChannel(pContext->playback, pInputSettings->iSecondaryPid, &playbackPidSettings);
			}
		}

		/* Set up decoder Start structures now. We need to know the audio codec to properly set up
		the audio outputs. */
		NEXUS_AudioDecoder_GetDefaultStartSettings(&audioProgram);
		if(pInputSettings->resource == BTST_RESOURCE_HDMI) {
#if NEXUS_HAS_HDMI_INPUT
			audioProgram.input = NEXUS_HdmiInput_GetAudioConnector(pContext->hdmiInput);
			audioProgram.delayMode = NEXUS_AudioDecoderDelayMode_eFixed;/* dsp mixer after hdmi cannot use low delay mode */
#endif
		} else {
			audioProgram.codec = pInputSettings->eAudioCodec;
			audioProgram.pidChannel = pContext->audioPidChannel;
			audioProgram.nonRealTime= pContext->bNonRealTime;
		}
		audioProgram.stcChannel = pContext->stcAudioChannel;
		pContext->audProgram = audioProgram;
		if(g_bSecondAudio) {
			audioProgram.pidChannel = pContext->secondaryPidChannel;
			audioProgram.secondaryDecoder = true;   /* Indicate this is a secondary channel for STC Channel/PCRlib functions */
			pContext->secondaryProgram = audioProgram;
		}

		/* Open audio mixer.  The mixer can be left running at all times to provide continuous audio output despite input discontinuities.  */
		if(!g_bNoDspMixer) {
			NEXUS_AudioMixer_GetDefaultSettings(&audioMixerSettings);
			audioMixerSettings.mixUsingDsp = true;
			audioMixerSettings.dspIndex    = pContext->contextId / BTST_NUM_AUDIO_XCODE_PER_DSP;
			pContext->audioMixer = NEXUS_AudioMixer_Open(&audioMixerSettings);
			assert(pContext->audioMixer);
		}

		/* Open audio mux output */
		pContext->audioMuxOutput = NEXUS_AudioMuxOutput_Create(NULL);
		assert(pContext->audioMuxOutput);

		if(pEncodeSettings->bAudioEncode || (pInputSettings->resource == BTST_RESOURCE_HDMI))
		{
			/* Open audio encoder */
			NEXUS_AudioEncoder_GetDefaultSettings(&encoderSettings);
			encoderSettings.codec = pEncodeSettings->encoderAudioCodec;
			audioCodec = pEncodeSettings->encoderAudioCodec;
			pContext->audioEncoder = NEXUS_AudioEncoder_Open(&encoderSettings);
			assert(pContext->audioEncoder);

			if(g_bNoDspMixer) {
				/* Connect decoder to encoder */
				NEXUS_AudioEncoder_AddInput(pContext->audioEncoder,
					NEXUS_AudioDecoder_GetConnector(pContext->audioDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
				BDBG_WRN(("No DSP mixer..."));
			}
			else {
				/* Connect decoder to mixer and set as master */
				NEXUS_AudioMixer_AddInput(pContext->audioMixer,
					NEXUS_AudioDecoder_GetConnector(pContext->audioDecoder, 
						(pContext->inputSettings.eMultiChanFmt > NEXUS_AudioMultichannelFormat_eStereo && USE_DDRE)?
						NEXUS_AudioDecoderConnectorType_eMultichannel : NEXUS_AudioDecoderConnectorType_eStereo));
				if(g_bSecondAudio) {
					NEXUS_AudioMixer_AddInput(pContext->audioMixer, 
						NEXUS_AudioDecoder_GetConnector(pContext->secondaryDecoder, 
							(pContext->inputSettings.eMultiChanFmt > NEXUS_AudioMultichannelFormat_eStereo && USE_DDRE)?
							NEXUS_AudioDecoderConnectorType_eMultichannel : NEXUS_AudioDecoderConnectorType_eStereo));
				}
				audioMixerSettings.master = NEXUS_AudioDecoder_GetConnector(pContext->audioDecoder, 
						(pContext->inputSettings.eMultiChanFmt > NEXUS_AudioMultichannelFormat_eStereo && USE_DDRE)?
						NEXUS_AudioDecoderConnectorType_eMultichannel : NEXUS_AudioDecoderConnectorType_eStereo);
				NEXUS_AudioMixer_SetSettings(pContext->audioMixer, &audioMixerSettings);

				#if USE_DDRE
					/* Add the DDRE processor after mixing */
					pContext->ddre = NEXUS_DolbyDigitalReencode_Open(NULL);
					{ NEXUS_DolbyVolume258Settings setting;
					/* Open DV258 and add between encoder and DDRE */
					NEXUS_DolbyVolume258_GetDefaultSettings(&setting);
					setting.enabled = true;
					pContext->dv258 = NEXUS_DolbyVolume258_Open(&setting);
					}
					NEXUS_DolbyVolume258_AddInput(pContext->dv258, 
						NEXUS_AudioMixer_GetConnector(pContext->audioMixer));
					NEXUS_DolbyDigitalReencode_AddInput(pContext->ddre, 
						NEXUS_DolbyVolume258_GetConnector(pContext->dv258));

					/* Connect dv258 to encoder */
					NEXUS_AudioEncoder_AddInput(pContext->audioEncoder, 
						NEXUS_DolbyDigitalReencode_GetConnector(pContext->ddre, NEXUS_DolbyDigitalReencodeConnectorType_eStereo));
				#else
					/* Connect mixer to encoder */
					NEXUS_AudioEncoder_AddInput(pContext->audioEncoder, 
						NEXUS_AudioMixer_GetConnector(pContext->audioMixer));
				#endif
			}

			/* Connect mux to encoder */
			NEXUS_AudioOutput_AddInput(
				NEXUS_AudioMuxOutput_GetConnector(pContext->audioMuxOutput), 
				NEXUS_AudioEncoder_GetConnector(pContext->audioEncoder));
		}
		else
		{
			if(g_bNoDspMixer) {
				/* Connect decoder to mux out */
				NEXUS_AudioOutput_AddInput(
					NEXUS_AudioMuxOutput_GetConnector(pContext->audioMuxOutput),
					NEXUS_AudioDecoder_GetConnector(pContext->audioDecoder, NEXUS_AudioDecoderConnectorType_eCompressed));
			}
			else {
				NEXUS_AudioMixer_AddInput(pContext->audioMixer,
					NEXUS_AudioDecoder_GetConnector(pContext->audioDecoder, NEXUS_AudioDecoderConnectorType_eCompressed));
				audioMixerSettings.master = NEXUS_AudioDecoder_GetConnector(pContext->audioDecoder, NEXUS_AudioDecoderConnectorType_eCompressed);
				NEXUS_AudioMixer_SetSettings(pContext->audioMixer, &audioMixerSettings);
				NEXUS_AudioOutput_AddInput(
					NEXUS_AudioMuxOutput_GetConnector(pContext->audioMuxOutput),
					NEXUS_AudioMixer_GetConnector(pContext->audioMixer));
			}
			audioCodec = audioProgram.codec;
		}

		/* Attach outputs for real-time transcoding */
		if(!pContext->bNonRealTime)
		{
			if(g_bNoDspMixer) {
				NEXUS_AudioOutput_AddInput(
					NEXUS_AudioDummyOutput_GetConnector(g_platformConfig.outputs.audioDummy[pContext->contextId]),
					NEXUS_AudioDecoder_GetConnector(pContext->audioDecoder, pEncodeSettings->bAudioEncode?
						NEXUS_AudioDecoderConnectorType_eStereo :
						NEXUS_AudioDecoderConnectorType_eCompressed));
			}
			else {
			#if USE_DDRE
				NEXUS_AudioOutput_AddInput(
					NEXUS_AudioDummyOutput_GetConnector(g_platformConfig.outputs.audioDummy[pContext->contextId]),
					NEXUS_DolbyDigitalReencode_GetConnector(pContext->ddre, NEXUS_DolbyDigitalReencodeConnectorType_eStereo));
			#else
				NEXUS_AudioOutput_AddInput(
					NEXUS_AudioDummyOutput_GetConnector(g_platformConfig.outputs.audioDummy[pContext->contextId]),
					NEXUS_AudioMixer_GetConnector(pContext->audioMixer));
			#endif
			}
		}
	}

	/************************************
	 * Set up encoder AV sync.
	 * encode setting and startSetting to be set after end-to-end delay is determined */
	/* get end-to-end delay (Dee) for audio and video encoders;
	* TODO: match AV delay! In other words,
	*   if (aDee > vDee) {
	*       vDee' = aDee' = aDee;
	*   }
	*   else {
	*       vDee' = aDee' = vDee;
	*   }
	*/
	xcode_av_sync(pContext, &videoEncoderConfig, &videoEncoderStartConfig);

	if(pContext->inputSettings.resource != BTST_RESOURCE_HDMI) {
#if BTST_ENABLE_AV_SYNC
		/* connect sync channel */
		NEXUS_SyncChannel_GetSettings(pContext->syncChannel, &syncChannelSettings);
		syncChannelSettings.videoInput = NEXUS_VideoDecoder_GetConnector(pContext->videoDecoder);
		if(pContext->inputSettings.bAudioInput)
		{
			if(pContext->encodeSettings.bAudioEncode)
				syncChannelSettings.audioInput[0] = NEXUS_AudioDecoder_GetConnector(pContext->audioDecoder, NEXUS_AudioDecoderConnectorType_eStereo);
			else
				syncChannelSettings.audioInput[0] = NEXUS_AudioDecoder_GetConnector(pContext->audioDecoder, NEXUS_AudioDecoderConnectorType_eCompressed);
#if BTST_ENABLE_NRT_STC_AV_WINDOW
			/* NRT mode pairs AV stc channels */
			if(pContext->bNonRealTime) {
				NEXUS_StcChannel_GetDefaultPairSettings(&stcAudioVideoPair);
				stcAudioVideoPair.connected = true;
				stcAudioVideoPair.window = 300; /* 300ms AV window means when source discontinuity occurs, up to 300ms transition could occur with NRT transcoded stream */
				NEXUS_StcChannel_SetPairSettings(pContext->stcVideoChannel, pContext->stcAudioChannel, &stcAudioVideoPair);
			}
#endif
		}

		NEXUS_SyncChannel_SetSettings(pContext->syncChannel, &syncChannelSettings);
#endif
	}

	/****************************
	 * setup video encoder
	 */
	NEXUS_VideoEncoder_SetSettings(pContext->videoEncoder, &videoEncoderConfig);

	/************************************************
	 * Set up xcoder stream_mux and record
	 */
	xcode_setup_mux_record(pContext);

	/* store encoder start settings */
	pContext->vidEncoderStartSettings = videoEncoderStartConfig;

	g_activeXcodeCount++;
	return 0;
}

static void close_transcode(
	TranscodeContext  *pContext )
{
	size_t i;

	if(!pContext->videoEncoder) {
		return;
	}

	g_activeXcodeCount--;

	/*******************************
	 * release system data PAT/PMT
	 */
	xcode_release_systemdata(pContext);

	/************************************
	 * Bring down transcoder 
	 */
	if(pContext->inputSettings.bTsUserDataInput && pContext->inputSettings.resource != BTST_RESOURCE_HDMI) {
		for(i = 0; i < pContext->inputSettings.numUserDataPids; i++) {
			if(pContext->bUserDataStreamValid[i]) {
				NEXUS_Message_Stop(pContext->userDataMessage[i]);
				NEXUS_Message_Close(pContext->userDataMessage[i]);
			}
		}
	}
	NEXUS_Record_Destroy(pContext->record);
	NEXUS_Recpump_Close(pContext->recpump);

	if(pContext->inputSettings.resource == BTST_RESOURCE_FILE)
	{
		NEXUS_Playback_CloseAllPidChannels(pContext->playback);
		NEXUS_FilePlay_Close(pContext->file);
		NEXUS_Playback_Destroy(pContext->playback);
		NEXUS_Playpump_Close(pContext->playpump);
	}
	else if(pContext->inputSettings.resource == BTST_RESOURCE_QAM)
	{
		if(pContext->inputSettings.bAudioInput)
			NEXUS_PidChannel_Close(pContext->audioPidChannel);
		NEXUS_PidChannel_Close(pContext->videoPidChannel);
	}
	NEXUS_Playpump_CloseAllPidChannels(pContext->playpumpTranscodePcr);

	/******************************************
	 * nexus kernel mode requires explicit remove/shutdown video inputs before close windows/display
	 */
#if NEXUS_HAS_HDMI_INPUT
	if(pContext->inputSettings.resource == BTST_RESOURCE_HDMI) {
		NEXUS_VideoInput_Shutdown(NEXUS_HdmiInput_GetVideoConnector(pContext->hdmiInput));
	}
	else 
#endif
	{
#if BTST_ENABLE_AV_SYNC
		/* disconnect sync channel after decoders stop */
		NEXUS_SyncChannel_Destroy(pContext->syncChannel);
#endif
#if BTST_DISABLE_NRT_DEBUG_DISPLAY
		if(!pContext->bNonRealTime && (pContext->contextId == 0))
#endif
		{
			NEXUS_VideoWindow_RemoveInput(pContext->window, NEXUS_VideoDecoder_GetConnector(pContext->videoDecoder));
		}
		NEXUS_VideoWindow_RemoveInput(pContext->windowTranscode, NEXUS_VideoDecoder_GetConnector(pContext->videoDecoder));
		NEXUS_VideoInput_Shutdown(NEXUS_VideoDecoder_GetConnector(pContext->videoDecoder));
		NEXUS_VideoDecoder_Close(pContext->videoDecoder);
	}
	if(pContext->contextId == 0)
		NEXUS_VideoWindow_Close(pContext->window);
	NEXUS_VideoWindow_Close(pContext->windowTranscode);
	close_gfx(pContext);
	if(pContext->contextId == 0)
		NEXUS_Display_Close(pContext->display);
	NEXUS_Display_Close(pContext->displayTranscode);

	BKNI_DestroyEvent(pContext->finishEvent);
	NEXUS_StreamMux_Destroy(pContext->streamMux);

	NEXUS_Playpump_Close(pContext->playpumpTranscodePcr);
	
	NEXUS_Playpump_Close(pContext->playpumpTranscodeVideo);
	NEXUS_VideoEncoder_Close(pContext->videoEncoder);
	pContext->videoEncoder = NULL;
	
	if(pContext->inputSettings.bAudioInput)
	{
		if(pContext->encodeSettings.bAudioEncode || (pContext->inputSettings.resource == BTST_RESOURCE_HDMI))
		{
			NEXUS_AudioOutput_RemoveAllInputs(NEXUS_AudioMuxOutput_GetConnector(pContext->audioMuxOutput));
			NEXUS_AudioEncoder_RemoveAllInputs(pContext->audioEncoder);
			NEXUS_AudioInput_Shutdown(NEXUS_AudioEncoder_GetConnector(pContext->audioEncoder));
			NEXUS_AudioEncoder_Close(pContext->audioEncoder);
			if(!g_bNoDspMixer) {
				NEXUS_AudioMixer_RemoveAllInputs(pContext->audioMixer);
			}
			if(!pContext->bNonRealTime) {
				NEXUS_AudioOutput_RemoveAllInputs(NEXUS_AudioDummyOutput_GetConnector(g_platformConfig.outputs.audioDummy[pContext->contextId]));
				NEXUS_AudioOutput_Shutdown(NEXUS_AudioDummyOutput_GetConnector(g_platformConfig.outputs.audioDummy[pContext->contextId]));
			}
#if USE_DDRE
			NEXUS_DolbyDigitalReencode_RemoveAllInputs(pContext->ddre);
			NEXUS_DolbyDigitalReencode_Close(pContext->ddre);
			NEXUS_DolbyVolume258_RemoveAllInputs(pContext->dv258);
			NEXUS_DolbyVolume258_Close(pContext->dv258);
#endif
			if(!g_bNoDspMixer) {
				NEXUS_AudioInput_Shutdown(NEXUS_AudioMixer_GetConnector(pContext->audioMixer));
				NEXUS_AudioMixer_Close(pContext->audioMixer);
			}
			NEXUS_AudioInput_Shutdown(NEXUS_AudioDecoder_GetConnector(pContext->audioDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
			NEXUS_AudioDecoder_Close(pContext->audioDecoder);
			if(g_bSecondAudio) {
				NEXUS_AudioDecoder_Close(pContext->secondaryDecoder);
			}
		}
		else
		{
			NEXUS_AudioOutput_RemoveAllInputs(NEXUS_AudioMuxOutput_GetConnector(pContext->audioMuxOutput));
			if(!g_bNoDspMixer) {
				NEXUS_AudioMixer_RemoveAllInputs(pContext->audioMixer);
			}
			if(!pContext->bNonRealTime) {
				NEXUS_AudioOutput_RemoveAllInputs(NEXUS_AudioDummyOutput_GetConnector(g_platformConfig.outputs.audioDummy[pContext->contextId]));
				NEXUS_AudioOutput_Shutdown(NEXUS_AudioDummyOutput_GetConnector(g_platformConfig.outputs.audioDummy[pContext->contextId]));
			}
			if(!g_bNoDspMixer) {
				NEXUS_AudioInput_Shutdown(NEXUS_AudioMixer_GetConnector(pContext->audioMixer));
				NEXUS_AudioMixer_Close(pContext->audioMixer);
			}
			NEXUS_AudioInput_Shutdown(NEXUS_AudioDecoder_GetConnector(pContext->audioDecoder, NEXUS_AudioDecoderConnectorType_eCompressed));
			NEXUS_AudioDecoder_Close(pContext->audioDecoder);
		}
	
		NEXUS_Playpump_Close(pContext->playpumpTranscodeAudio);
		NEXUS_AudioOutput_Shutdown(NEXUS_AudioMuxOutput_GetConnector(pContext->audioMuxOutput));
		NEXUS_AudioMuxOutput_Destroy(pContext->audioMuxOutput);
	}

	NEXUS_StcChannel_Close(pContext->stcVideoChannel);
	NEXUS_StcChannel_Close(pContext->stcChannelTranscode);

#if NEXUS_HAS_HDMI_INPUT
	if(pContext->inputSettings.resource == BTST_RESOURCE_HDMI) {
		NEXUS_HdmiInput_Close(pContext->hdmiInput);
	}
#endif
}

/* stop without close */
static void stop_transcode(
	TranscodeContext  *pContext )
{
	NEXUS_VideoEncoderStopSettings videoEncoderStopSettings;

	if(!pContext->bStarted) {
		return;
	}

	/*******************************
	 * stop system data scheduler
	 */
	xcode_stop_systemdata(pContext);

	/**************************************************
	 * NOTE: stop sequence should be in front->back order
	 */
	if((pContext->inputSettings.resource == BTST_RESOURCE_FILE) && !pContext->bNoStopDecode)
	{
		NEXUS_Playback_Stop(pContext->playback);
	}
	if(pContext->inputSettings.bAudioInput)
	{
		if(!pContext->bNoStopDecode) {
		NEXUS_AudioDecoder_Stop(pContext->audioDecoder);
		if(g_bSecondAudio) {
			NEXUS_AudioDecoder_Stop(pContext->secondaryDecoder);
		}
		}
		if(!g_bNoDspMixer) {
			NEXUS_AudioMixer_Stop(pContext->audioMixer);
		}
		NEXUS_AudioMuxOutput_Stop(pContext->audioMuxOutput);
	}
	if(pContext->inputSettings.resource != BTST_RESOURCE_HDMI && !pContext->bNoStopDecode) {
		NEXUS_VideoDecoder_Stop(pContext->videoDecoder);
	}
	NEXUS_VideoEncoder_GetDefaultStopSettings(&videoEncoderStopSettings);
	videoEncoderStopSettings.mode = pContext->bImmediateStop? NEXUS_VideoEncoderStopMode_eImmediate : NEXUS_VideoEncoderStopMode_eNormal;
	NEXUS_VideoEncoder_Stop(pContext->videoEncoder, &videoEncoderStopSettings);
	if(!pContext->bImmediateStop) {
		NEXUS_StreamMux_Finish(pContext->streamMux);
		if(BKNI_WaitForEvent(pContext->finishEvent, 4000)!=BERR_SUCCESS) {
			fprintf(stderr, "TIMEOUT\n");
		}
	}
	pContext->bImmediateStop = false; /* reset stop mode whenever used; it needs to be set everytime to be used */
	NEXUS_Record_Stop(pContext->record);
	NEXUS_FileRecord_Close(pContext->fileTranscode);
	/*****************************************
	 * Note: remove all record PID channels before stream 
	 * mux stop since streammux would close the A/V PID channels 
	 */
	NEXUS_Record_RemoveAllPidChannels(pContext->record);
	NEXUS_StreamMux_Stop(pContext->streamMux);
    /* Temporary workaround to flush pending descriptors from NEXUS_AudioMuxOutput prior to restarting it.
       Restarting will flush the pending descriptors. */
    #if 1
    if (pContext->inputSettings.bAudioInput)
    {
        const NEXUS_AudioMuxOutputFrame *pBuf,*pBuf2;
        size_t size,size2;
        NEXUS_Error errCode;

        errCode = NEXUS_AudioMuxOutput_GetBuffer(pContext->audioMuxOutput, &pBuf, &size, &pBuf2, &size2);
        if ( BERR_SUCCESS == errCode )
        {
            size += size2;
            if ( size > 0 )
            {
                BDBG_WRN(("Flushing %u outstanding audio descriptors", size));
                NEXUS_AudioMuxOutput_ReadComplete(pContext->audioMuxOutput, size);
            }
        }
    }
    #endif

	/* stopped */
	pContext->bStarted = false;
}

/* start without open */
static void start_transcode(
	TranscodeContext  *pContext )
{
	NEXUS_StreamMuxOutput muxOutput;
	NEXUS_RecordPidChannelSettings recordPidSettings;
	if(pContext->bStarted) {
		return;
	}

	/*******************************
	 * Add system data to stream_mux
	 */
	xcode_start_systemdata(pContext);

	/**************************************************
	 * NOTE: transcoder record/mux resume 
	 */
	/* start mux */
	NEXUS_StreamMux_Start(pContext->streamMux,&pContext->muxConfig, &muxOutput);
	pContext->pidChannelTranscodeVideo = muxOutput.video[0];

	/* add multiplex data to the same record */
	/* configure the video pid for indexing */
	NEXUS_Record_GetDefaultPidChannelSettings(&recordPidSettings);
	recordPidSettings.recpumpSettings.pidType = NEXUS_PidType_eVideo;
	recordPidSettings.recpumpSettings.pidTypeSettings.video.index = /* RAVE only support mpeg2&avc indexing */
			(pContext->encodeSettings.encoderVideoCodec==NEXUS_VideoCodec_eMpeg2) ||
			(pContext->encodeSettings.encoderVideoCodec==NEXUS_VideoCodec_eH264);
	recordPidSettings.recpumpSettings.pidTypeSettings.video.codec = pContext->encodeSettings.encoderVideoCodec;
	NEXUS_Record_AddPidChannel(pContext->record, pContext->pidChannelTranscodeVideo, &recordPidSettings);

	if(pContext->inputSettings.bAudioInput)
	{
		pContext->pidChannelTranscodeAudio = muxOutput.audio[0];
		NEXUS_Record_AddPidChannel(pContext->record, pContext->pidChannelTranscodeAudio, NULL);
	}

	NEXUS_Record_AddPidChannel(pContext->record, pContext->pidChannelTranscodePcr, NULL);
	NEXUS_Record_AddPidChannel(pContext->record, pContext->pidChannelTranscodePat, NULL);
	NEXUS_Record_AddPidChannel(pContext->record, pContext->pidChannelTranscodePmt, NULL);
	if(pContext->inputSettings.bTsUserDataInput && pContext->inputSettings.resource != BTST_RESOURCE_HDMI) {
		size_t i;
		BDBG_ASSERT(NEXUS_MAX_MUX_PIDS >= pContext->inputSettings.numUserDataPids);
		for(i = 0; i < pContext->inputSettings.numUserDataPids; i++) {
			if(pContext->bUserDataStreamValid[i]) {
				NEXUS_Record_AddPidChannel(pContext->record, pContext->pidChannelTranscodeUserData[i], NULL);
			}
		}
	}

	if(g_bFifo) {
		NEXUS_FifoRecordSettings fifoRecordSettings;

		pContext->fifoFile = NEXUS_FifoRecord_Create(pContext->encodeSettings.fname,
			pContext->indexfname[0]? pContext->indexfname : NULL);

		NEXUS_FifoRecord_GetSettings(pContext->fifoFile, &fifoRecordSettings);
		fifoRecordSettings.interval = 60;
		BDBG_ASSERT(!NEXUS_FifoRecord_SetSettings(pContext->fifoFile, &fifoRecordSettings));

		pContext->fileTranscode = NEXUS_FifoRecord_GetFile(pContext->fifoFile);
		BDBG_MSG(("Opened record fifo file."));
	}
	else {
		pContext->fileTranscode = NEXUS_FileRecord_OpenPosix(pContext->encodeSettings.fname,
			pContext->indexfname[0]? pContext->indexfname : NULL);
	}
	assert(pContext->fileTranscode);

	/* Start record of stream mux output */
	NEXUS_Record_Start(pContext->record, pContext->fileTranscode);

	/****************************
	 * start decoders
	 */
	if((pContext->inputSettings.resource != BTST_RESOURCE_HDMI) && !pContext->bNoStopDecode) {
		/* Start decoder */
		NEXUS_VideoDecoder_Start(pContext->videoDecoder, &pContext->vidProgram);
	}

	if(pContext->inputSettings.bAudioInput)
	{
		/* Start audio mux output */
		NEXUS_AudioMuxOutput_Start(pContext->audioMuxOutput, &pContext->audMuxStartSettings);
		/* Start audio mixer */
		if(!g_bNoDspMixer) {
			NEXUS_AudioMixer_Start(pContext->audioMixer);
		}

		if(!pContext->bNoStopDecode) {
			NEXUS_AudioDecoder_Start(pContext->audioDecoder, &pContext->audProgram);
			if(g_bSecondAudio) {
				BDBG_WRN(("Starting Secondary audio"));
				NEXUS_AudioDecoder_Start(pContext->secondaryDecoder, &pContext->secondaryProgram);
			}
		}
	}
	
	/* Start playback before mux set up PAT/PMT which may depend on PMT user data PSI extraction */
	if((pContext->inputSettings.resource == BTST_RESOURCE_FILE) && !pContext->bNoStopDecode)
	{
		NEXUS_Playback_Start(pContext->playback, pContext->file, NULL);
	}

	/************************************************
	 * Start video encoder
	 */
	pContext->vidEncoderStartSettings.adaptiveLowDelayMode = pContext->bAdaptiveLatencyStart;
	NEXUS_VideoEncoder_Start(pContext->videoEncoder, &pContext->vidEncoderStartSettings);

	/* started */
	pContext->bStarted = true;
}

static void bringup_transcode(
	TranscodeContext  *pContext )
{
	/* open the transcode context */
	open_transcode(pContext);

	/*******************************
	 * START transcoder 
	 */
	start_transcode(pContext);

	/* increment NRT reference count */
	if(pContext->bNonRealTime && (NULL==pContext->eofEvent)) {
		pContext->eofEvent = B_Event_Create(NULL);
		B_Event_Reset(pContext->eofEvent);
		pContext->nrtEofHandle = B_Thread_Create("NRT mode EOF handler", (B_ThreadFunc)nrt_endOfStreamHandler, pContext, NULL);
	}
}
static void shutdown_transcode(
	TranscodeContext  *pContext )
{
	/*******************************
	 * START transcoder 
	 */
	stop_transcode(pContext);

	/* close the transcode context */
	close_transcode(pContext);

	if(g_decoderContext[BTST_P_DecoderId_eSource0 + pContext->contextId].output608.fLog) {/* to log loopback xcoded user data */
		fclose(g_decoderContext[BTST_P_DecoderId_eSource0 + pContext->contextId].output608.fLog);
		g_decoderContext[BTST_P_DecoderId_eSource0 + pContext->contextId].output608.fLog = NULL;
	}
	if(g_decoderContext[BTST_P_DecoderId_eSource0 + pContext->contextId].output708.fLog) {/* to log loopback xcoded user data */
		fclose(g_decoderContext[BTST_P_DecoderId_eSource0 + pContext->contextId].output708.fLog);
		g_decoderContext[BTST_P_DecoderId_eSource0 + pContext->contextId].output708.fLog = NULL;
	}
}
#endif

static void config_xcoder_context (
	TranscodeContext *pContext )
{
	int choice;
	char input[80];
	pContext->inputSettings.bConfig = true;
	printf("\n choose source (0) File playback (1) HDMI (2) QAM	  ");scanf("%d", (int32_t*)&pContext->inputSettings.resource);

	switch (pContext->inputSettings.resource)
	{
	case (BTST_RESOURCE_HDMI):
#if !NEXUS_HAS_HDMI_INPUT
		fprintf(stderr, "HDMI input is not supported!\n");
		return;
#endif
		break;
#if NEXUS_HAS_FRONTEND
	case (BTST_RESOURCE_QAM):
		{
			printf("\n Front End QAM freq (Mhz): ");						     scanf("%d", (int32_t*)&pContext->inputSettings.freq);
			printf("\n Front End QAM Mode:\n"
				" (%d) 64\n"
				" (%d) 256\n"
				" (%d) 1024\n", 
				NEXUS_FrontendQamMode_e64, NEXUS_FrontendQamMode_e256, NEXUS_FrontendQamMode_e1024); 
			scanf("%d", (int32_t*)&pContext->inputSettings.qamMode);
			printf("\n source stream type: (2) Ts             "); pContext->inputSettings.eStreamType = NEXUS_TransportType_eTs;
			printf("\n source stream codec: (%d) Mpeg2 (%d) H264",
				NEXUS_VideoCodec_eMpeg2, NEXUS_VideoCodec_eH264); 
			scanf("%d", (int32_t*)&pContext->inputSettings.eVideoCodec);
			printf("\n Video pid:                                     "); scanf("%s", input); pContext->inputSettings.iVideoPid = strtoul(input, NULL, 0);
			printf("\n Pcr   pid:                                     "); scanf("%s", input); pContext->inputSettings.iPcrPid = strtoul(input, NULL, 0);
			break;
		}
#else
	case (BTST_RESOURCE_QAM):
		printf("\n QAM tuning not supported ");
		break;
#endif
	case (BTST_RESOURCE_FILE):
	default:
		{
			printf("\n source stream file: ");
			scanf("%s", pContext->inputSettings.fname);
			printf("\n source stream type:  (%d) Es (%d) Ts           ", 
				NEXUS_TransportType_eEs, NEXUS_TransportType_eTs);	   scanf("%d", (int32_t*)&pContext->inputSettings.eStreamType);
			printf("\n source stream codec: (%d) Mpeg2 (%d) H264      ",
				NEXUS_VideoCodec_eMpeg2, NEXUS_VideoCodec_eH264);	   
			scanf("%d", (int32_t*)&pContext->inputSettings.eVideoCodec);
			printf("\n Video pid:                                           ");	   scanf("%s", input); pContext->inputSettings.iVideoPid = strtoul(input, NULL, 0);
			printf("\n Pcr   pid:                                           ");	   scanf("%s", input); pContext->inputSettings.iPcrPid = strtoul(input, NULL, 0);
		}
	}

	printf("\n Input Audio Parameters");
	printf("\n Enable Audio channel: (0) No (1) Yes             ");	   scanf("%d", &choice);
	pContext->inputSettings.bAudioInput = (choice!=0);
	if(pContext->inputSettings.bAudioInput)
	{
		if (pContext->inputSettings.resource == BTST_RESOURCE_HDMI)
		{
			printf("\n Is HDMI input audio uncompressed? (0)No; (1)Yes    ");   scanf("%d", &choice);
			pContext->inputSettings.bPcmAudio = choice;
		}
		else
		{
			printf("\n Audio Pid:                                           "); scanf("%s", input); pContext->inputSettings.iAudioPid = strtoul(input, NULL, 0);
		}

		/* PCM audio must be encoded */
		if(pContext->inputSettings.bPcmAudio)
		{
			pContext->encodeSettings.bAudioEncode = true;
		}
		else
		{
			printf("\n Audio Codec:\n"
				" (%d)Mpg\n"
				" (%d)AAC\n"
				" (%d)AAC+Loas\n"
				" (%d)AAC+Adts\n"
				" (%d)AC3\n"
				" (%d)AC3+\n",
				NEXUS_AudioCodec_eMpeg,
				NEXUS_AudioCodec_eAac,
				NEXUS_AudioCodec_eAacPlusLoas,
				NEXUS_AudioCodec_eAacPlusAdts,
				NEXUS_AudioCodec_eAc3,
				NEXUS_AudioCodec_eAc3Plus);
			scanf("%d", (int32_t*)&pContext->inputSettings.eAudioCodec);
			pContext->encodeSettings.encoderAudioCodec = pContext->inputSettings.eAudioCodec;
			if(g_bMultiChanAudio) {
				printf("\n Multi-channel audio format: \n"
					" (%d)None\n"
					" (%d)Stereo\n"
					" (%d)5.1\n"
					" (%d)7.1\n",
					NEXUS_AudioMultichannelFormat_eNone,
					NEXUS_AudioMultichannelFormat_eStereo,
					NEXUS_AudioMultichannelFormat_e5_1,
					NEXUS_AudioMultichannelFormat_e7_1);
				scanf("%d", (int32_t*)&pContext->inputSettings.eMultiChanFmt);
			}
			if(g_bSecondAudio) {
				printf("\n Secondary Audio Pid:                    "); scanf("%s", input); pContext->inputSettings.iSecondaryPid = strtoul(input, NULL, 0);
			}
		}
	}

	printf("\n****************************************************************\n");
	printf("\n Input Parameters\n");
	printf("\n SourceType:   %d", pContext->inputSettings.resource);
	if(pContext->inputSettings.resource == BTST_RESOURCE_FILE)
		printf("\n filename:       %s", pContext->inputSettings.fname);
#if NEXUS_HAS_FRONTEND
	if(pContext->inputSettings.resource == BTST_RESOURCE_QAM)
		printf("\n freq: %d MHz;  qam mode: %d", pContext->inputSettings.freq, pContext->inputSettings.qamMode);
#endif

	if(pContext->inputSettings.resource != BTST_RESOURCE_HDMI)
	{
		printf("\n Streamtype:  %d \n encoderVideoCodec:  %d", pContext->inputSettings.eStreamType, pContext->inputSettings.eVideoCodec);
		printf("\n vPid:  %#x;  PcrPid:   %#x", pContext->inputSettings.iVideoPid, pContext->inputSettings.iPcrPid);
	}
	if(pContext->inputSettings.bAudioInput)
	{
		printf("\n Enable Audio channel Pid: %#x\n audio codec: %d           ", pContext->inputSettings.iAudioPid, pContext->inputSettings.eAudioCodec);
	}
	printf("\n Config %d", pContext->inputSettings.bConfig);
	printf("\n****************************************************************\n");

	printf("\n\n Encode Settings");
	printf("\n encode stream:                                                                        "); scanf("%s", pContext->encodeSettings.fname);
	printf("\n custom format: (0) No (1) Yes                                                         "); scanf("%d", &choice);
	pContext->encodeSettings.bCustom = choice;
	if(!pContext->encodeSettings.bCustom)
	{
		printf("\n enocder displayFormat:\n"
			" (%2d) 480i\n"
			" (%2d) 576i\n"
			" (%2d) 480p\n"
			" (%2d) 576p\n"
			" (%2d) 1080i60\n"
			" (%2d) 1080i50\n"
			" (%2d) 1080p24\n"
			" (%2d) 1080p25\n"
			" (%2d) 1080p30\n"
			" (%2d) 1080p50\n"
			" (%2d) 1080p60\n"
			" (%2d) 720p60\n"
			" (%2d) 720p50\n"
			" (%2d) 720p24\n"
			" (%2d) 720p25\n"
			" (%2d) 720p30Hz:\n",
			(NEXUS_VideoFormat_eNtsc),
			(NEXUS_VideoFormat_ePal),
			(NEXUS_VideoFormat_e480p),
			(NEXUS_VideoFormat_e576p),
			(NEXUS_VideoFormat_e1080i),
			(NEXUS_VideoFormat_e1080i50hz),
			(NEXUS_VideoFormat_e1080p24hz),
			(NEXUS_VideoFormat_e1080p25hz),
			(NEXUS_VideoFormat_e1080p30hz),
			(NEXUS_VideoFormat_e1080p50hz),
			(NEXUS_VideoFormat_e1080p60hz),
			(NEXUS_VideoFormat_e720p),
			(NEXUS_VideoFormat_e720p50hz),
			(NEXUS_VideoFormat_e720p24hz),
			(NEXUS_VideoFormat_e720p25hz),
			(NEXUS_VideoFormat_e720p30hz)); 
		scanf("%d", (int32_t*)&pContext->encodeSettings.displayFormat);
	}
	else
	{
		pContext->encodeSettings.displayFormat = NEXUS_VideoFormat_eCustom2;
		printf("\n Resolution width & height:                                                        ");
		scanf("%d %d", (int32_t*)&pContext->encodeSettings.customFormatSettings.width, (int32_t*)&pContext->encodeSettings.customFormatSettings.height);
		printf("\n Interlaced? (0) No (1) Yes	"); 
		scanf("%d", &choice); pContext->encodeSettings.customFormatSettings.interlaced = choice;
		printf("\n Vertical refresh rate (in 1/1000th Hz):                                           "); scanf("%u", (unsigned*)&pContext->encodeSettings.customFormatSettings.refreshRate);
		printf("\n Aspect Ratio:\n"
			" (%d) Auto\n"
			" (%d) 4x3\n"
			" (%d) 16x9\n"
			" (%d) SAR\n",
			NEXUS_DisplayAspectRatio_eAuto,
			NEXUS_DisplayAspectRatio_e4x3,
			NEXUS_DisplayAspectRatio_e16x9,
			NEXUS_DisplayAspectRatio_eSar); 
		scanf("%d", (int32_t*)&pContext->encodeSettings.customFormatSettings.aspectRatio);
		if(NEXUS_DisplayAspectRatio_eSar == pContext->encodeSettings.customFormatSettings.aspectRatio)
		{
			printf("Please enter Sample Aspect Ratio X and Y: \n");
			scanf("%u %u", &pContext->encodeSettings.customFormatSettings.sampleAspectRatio.x,
				&pContext->encodeSettings.customFormatSettings.sampleAspectRatio.y);
		}
	}

	printf("\n Frame rate:\n"
		" (%d) 23.976\n"
		" (%d) 24\n"
		" (%d) 25\n"
		" (%d) 29.97Hz\n"
		" (%d) 30 Hz\n"
		" (%d) 50 Hz\n"
		" (%d) 59.94\n"
		" (%d) 60\n"
		" (%d) 14.985\n",
		NEXUS_VideoFrameRate_e23_976,
		NEXUS_VideoFrameRate_e24,
		NEXUS_VideoFrameRate_e25,
		NEXUS_VideoFrameRate_e29_97,
		NEXUS_VideoFrameRate_e30,
		NEXUS_VideoFrameRate_e50,
		NEXUS_VideoFrameRate_e59_94,
		NEXUS_VideoFrameRate_e60,
		NEXUS_VideoFrameRate_e14_985);
	scanf("%d", (int*)&pContext->encodeSettings.encoderFrameRate);
	printf("\n Bitrate (bps):                                                                       "); scanf("%d", (int*)&pContext->encodeSettings.encoderBitrate);
	printf("\n # of P frames per GOP:                                                               "); scanf("%d", (int*)&pContext->encodeSettings.encoderGopStructureFramesP);
	printf("\n # of B frames between reference I or P frames:                                       "); scanf("%d", (int*)&pContext->encodeSettings.encoderGopStructureFramesB);
	printf("\n Encode Video Codec: (%d) MPEG2 (%d) MPEG4 Part2 (%d) H264                            ", 
		NEXUS_VideoCodec_eMpeg2, NEXUS_VideoCodec_eMpeg4Part2, NEXUS_VideoCodec_eH264); 
	scanf("%d", (int*)&pContext->encodeSettings.encoderVideoCodec);
	printf("\n Profile: (%d) Simple (%d) AdvancedSimple (%d) Main (%d) High (%d) Baseline                               ",
		NEXUS_VideoCodecProfile_eSimple, NEXUS_VideoCodecProfile_eAdvancedSimple, NEXUS_VideoCodecProfile_eMain, NEXUS_VideoCodecProfile_eHigh, NEXUS_VideoCodecProfile_eBaseline); 
	scanf("%d", (int*)&pContext->encodeSettings.encoderProfile);
	printf("\n Level:\n"
		" (%2d) 1.0\n"
		" (%2d) 1.1\n"
		" (%2d) 1.2\n"
		" (%2d) 1.3\n"
		" (%2d) 2.0\n"
		" (%2d) 2.1\n"
		" (%2d) 2.2\n"
		" (%2d) 3.0\n"
		" (%2d) 3.1\n"
		" (%2d) 3.2\n"
		" (%2d) 4.0\n"
		" (%2d) 4.1\n"
		" (%2d) 4.2\n"
		" (%2d) 5.0\n"
		" (%2d) Low\n"
		" (%2d) Main\n"
		" (%2d) High\n",
		NEXUS_VideoCodecLevel_e10, NEXUS_VideoCodecLevel_e11, NEXUS_VideoCodecLevel_e12, NEXUS_VideoCodecLevel_e13, 
		NEXUS_VideoCodecLevel_e20, NEXUS_VideoCodecLevel_e21, NEXUS_VideoCodecLevel_e22, NEXUS_VideoCodecLevel_e30,
		NEXUS_VideoCodecLevel_e31, NEXUS_VideoCodecLevel_e32, NEXUS_VideoCodecLevel_e40, NEXUS_VideoCodecLevel_e41,
		NEXUS_VideoCodecLevel_e42, NEXUS_VideoCodecLevel_e50, NEXUS_VideoCodecLevel_eLow, NEXUS_VideoCodecLevel_eMain, NEXUS_VideoCodecLevel_eHigh); 
	scanf("%d", (int*)&pContext->encodeSettings.encoderLevel);

	if(pContext->inputSettings.bAudioInput)
	{
		if(pContext->inputSettings.bPcmAudio)
		{
			pContext->encodeSettings.bAudioEncode = true;
		}
		else
		{
			printf("\n Enable Audio transcode: (0) No (1) Yes                                                       "); scanf("%d", &choice);
			pContext->encodeSettings.bAudioEncode = choice;
		}

		if(pContext->encodeSettings.bAudioEncode)
		{
			printf("\n Encode Audio Codec: (%d)Mpg (%d)Mp3 (%d)AAC (%d)AAC+ (%d)AAC+Loas (%d)AAC+Adts (%d)AC3                              ",
				NEXUS_AudioCodec_eMpeg,
				NEXUS_AudioCodec_eMp3,
				NEXUS_AudioCodec_eAac,
				NEXUS_AudioCodec_eAacPlus,
				NEXUS_AudioCodec_eAacPlusLoas,
				NEXUS_AudioCodec_eAacPlusAdts,
				NEXUS_AudioCodec_eAc3); 
			scanf("%d", (int32_t*)&pContext->encodeSettings.encoderAudioCodec);
		}
		else {
			if(g_bSecondAudio) {printf("Multi audio streams cannot be passed through...\n"); BDBG_ASSERT(0);}
		}
	}
	if(pContext->inputSettings.resource == BTST_RESOURCE_FILE)
	{
		int choice;
		printf("\nNon-Realtime transcode? [1=y/0=n]\n");
		scanf("%d", &choice);
		pContext->bNonRealTime = choice;
	}

	if(!pContext->bNonRealTime)
	{
		unsigned iChoice;
		printf("\n Customize video encoder delay setting? [1=y/0=n]\n");
		scanf("%u", &iChoice);
		pContext->bCustomizeDelay = iChoice;
		if(pContext->bCustomizeDelay)
		{
			printf("\n enable Inverse Telecine Field Pairing coding? [1=y/0=n]\n");
			scanf("%u", &iChoice);
			pContext->encodeSettings.enableFieldPairing = iChoice;

			printf("\n video encoder rate buffer delay (ms):\n");
			scanf("%u", &iChoice);
			pContext->encodeSettings.rateBufferDelay = iChoice;

			printf("\n video encode display minimum frame rate:\n"
				" (%d) 23.976\n"
				" (%d) 24\n"
				" (%d) 25\n"
				" (%d) 29.97Hz\n"
				" (%d) 30 Hz\n"
				" (%d) 50 Hz\n"
				" (%d) 59.94\n"
				" (%d) 60\n"
				" (%d) 14.985\n",
				NEXUS_VideoFrameRate_e23_976,
				NEXUS_VideoFrameRate_e24,
				NEXUS_VideoFrameRate_e25,
				NEXUS_VideoFrameRate_e29_97,
				NEXUS_VideoFrameRate_e30,
				NEXUS_VideoFrameRate_e50,
				NEXUS_VideoFrameRate_e59_94,
				NEXUS_VideoFrameRate_e60,
				NEXUS_VideoFrameRate_e14_985);
			scanf("%d", (int*)&pContext->encodeSettings.inputFrameRateMin);

			printf("\n video encoder output minimum frame rate:\n"
				" (%d) 23.976\n"
				" (%d) 24\n"
				" (%d) 25\n"
				" (%d) 29.97Hz\n"
				" (%d) 30 Hz\n"
				" (%d) 50 Hz\n"
				" (%d) 59.94\n"
				" (%d) 60\n"
				" (%d) 14.985\n",
				NEXUS_VideoFrameRate_e23_976,
				NEXUS_VideoFrameRate_e24,
				NEXUS_VideoFrameRate_e25,
				NEXUS_VideoFrameRate_e29_97,
				NEXUS_VideoFrameRate_e30,
				NEXUS_VideoFrameRate_e50,
				NEXUS_VideoFrameRate_e59_94,
				NEXUS_VideoFrameRate_e60,
				NEXUS_VideoFrameRate_e14_985);
			scanf("%d", (int*)&pContext->encodeSettings.outputFrameRateMin);

			printf("\n video encode maximum resolution width:\n");
			scanf("%u", &pContext->encodeSettings.inputMaxWidth);
			printf("\n video encode maximum resolution height:\n");
			scanf("%u", &pContext->encodeSettings.inputMaxHeight);
			printf("\n video encoder pipeline low delay? [1=Y/0=N]:\n");
			scanf("%d", &choice);
			pContext->bLowDelay = choice;
		}
	}

	/* TS user data config */
	if(pContext->inputSettings.resource != BTST_RESOURCE_HDMI) {
		pContext->bEncodeCCUserData = g_bEncodeCC;
		pContext->inputSettings.bTsUserDataInput = g_bTsUserData;
		if(pContext->inputSettings.bTsUserDataInput) {
			printf("\n PMT pid [0=auto detection]:                           "); scanf("%s", input); pContext->inputSettings.pmtPid = strtoul(input, NULL, 0);
			printf("\n Do you want all TS user data passed thru? [1=y/0=n]   ");   scanf("%d", &choice);
			if(choice) {
				pContext->inputSettings.numUserDataPids = BTST_TS_USER_DATA_ALL;
				pContext->bRemapUserDataPid = false;
			} else {
				printf("\n How many user data PIDs are passed thru?   ");   scanf("%u", &pContext->inputSettings.numUserDataPids);
				pContext->inputSettings.numUserDataPids = (pContext->inputSettings.numUserDataPids > NEXUS_MAX_MUX_PIDS) ?
					NEXUS_MAX_MUX_PIDS : pContext->inputSettings.numUserDataPids;
				printf("\n Please enter the input user data PIDs list: ");
				for(choice=0; choice<(int)pContext->inputSettings.numUserDataPids; choice++) {
					scanf("%s", input); pContext->inputSettings.userDataPid[choice] = strtoul(input, NULL, 0);
				}
				printf("\n Remap the output TS user data PIDs? [1=y/0=n]  ");   scanf("%d", &choice); pContext->bRemapUserDataPid = choice;
				for(choice=0; choice<(int)pContext->inputSettings.numUserDataPids; choice++) {
					if(pContext->bRemapUserDataPid) {
						pContext->inputSettings.remapUserDataPid[choice] = BTST_MUX_USER_DATA_PID+choice;
					} else {/* no change */
						pContext->inputSettings.remapUserDataPid[choice] = pContext->inputSettings.userDataPid[choice];
					}
				}
			}
		}
	}

	printf("\n****************************************************************\n");
	printf("\n Encode Parameters\n");
	printf("\n filename:  %s", pContext->encodeSettings.fname);
	printf("\n display format:  %d", pContext->encodeSettings.displayFormat);
	printf("\n Frame Rate: %d\n Bit Rate: %d", pContext->encodeSettings.encoderFrameRate, pContext->encodeSettings.encoderBitrate);
	printf("\n P frames: %d;     B frames: %d", pContext->encodeSettings.encoderGopStructureFramesP, pContext->encodeSettings.encoderGopStructureFramesB);
	printf("\n Video Codec: %4d;  Profile: %4d;  Level: %4d", pContext->encodeSettings.encoderVideoCodec, pContext->encodeSettings.encoderProfile,pContext->encodeSettings.encoderLevel );
	if(pContext->encodeSettings.bAudioEncode)
		printf("\n Audio Codec: %4d", pContext->encodeSettings.encoderAudioCodec);
	printf("\n****************************************************************\n");
	printf("\n%s transcode...%s \n", pContext->bNonRealTime? "Non-Realtime":"Real time", 
		pContext->bLowDelay? "Low Delay pipeline Mode": "Normal delay pipeline Mode");
}

void print_usage(void) {
			printf("\ntranscode_ts usage:\n");
			printf("  Without options, it transcodes default stream file /data/videos/avatar_AVC_15M.ts into TS file: /data/BAT/encode.ts\n");
			printf("\nOptions:\n");
			printf("  -h        - to print the usage info\n");
			printf("  -scr N    - to run in script mode and quit in N seconds\n");
			printf("  -cfg      - to set the test configuration\n");
			printf("  -fifo     - to use FIFO file record instead of unbounded file record for long term test\n");
			printf("  -NRTwrap  - to loop source file for long term Non Real Time transcode test\n");
			printf("  -autoquit - to quit the test automatically when all transcoders are stopped\n");
			printf("  -cc       - to enable Closed Caption user data transcode\n");
			printf("  -tsud     - to enable TS layer user data passthrough.\n");
			printf("  -nodspmix - to disable audio DSP fw mixer.\n");
			printf("  -multichan - to enable audio multi-channel mode setting.\n");
			printf("  -msaudio  - to enable audio multi-stream decode mode.\n");
			printf("  -nrtRate N - to set stream mux NRT rate as N times fast as RT mode.\n");
			printf("  -msp MSEC - to set TS mux service period in miliseconds.\n");
			printf("  -rsp MSEC - to set record service period in miliseconds.\n");
			printf("  -type {single|multi|multiNRT}\n");
			printf("     Note: 1) single type supports single encode, low delay mode and up to 1080p30 realtime encode\n");
			printf("           2) multiNRT type supports up to 1080p30 with dual NRT encodes\n");
			printf("           3) multi type is the default, supports up to 2 encodes with combined throughput up to 1080p30\n");
}

#ifdef DIAGS
int transcode_main(int argc, char **argv)  {
#else
int main(int argc, char **argv)  {
#endif

#if NEXUS_HAS_VIDEO_ENCODER && NEXUS_HAS_STREAM_MUX

	NEXUS_PlatformSettings platformSettings;
	TranscodeContext *pContext = xcodeContext;
	int i, iteration = 1;

	BSTD_UNUSED(argv);
	BKNI_Memset(pContext, 0, sizeof(xcodeContext));

	if (argc == 1){ /* default test config for auto test */
			print_usage();
			printf("\nYou're testing with the default configuration:\n");
			/* Input setting */
			pContext->inputSettings.resource = BTST_RESOURCE_FILE;
			BKNI_Snprintf(pContext->inputSettings.fname, 256, "/data/videos/avatar_AVC_15M.ts");
			pContext->inputSettings.eStreamType=NEXUS_TransportType_eTs;
			pContext->inputSettings.eVideoCodec=NEXUS_VideoCodec_eH264;
			pContext->inputSettings.eAudioCodec=NEXUS_AudioCodec_eAc3;
			pContext->inputSettings.bPcmAudio=false;
			pContext->inputSettings.iVideoPid=0x101;
			pContext->inputSettings.iAudioPid=0x104;
			pContext->inputSettings.iPcrPid=0x101;
			pContext->inputSettings.freq=0;
#if NEXUS_HAS_FRONTEND
			pContext->inputSettings.qamMode=NEXUS_FrontendQamMode_e64;
#endif
			pContext->inputSettings.bAudioInput=0;
			pContext->inputSettings.bConfig = false;

			printf("\n****************************************************************\n");
			printf("\n Input Parameters\n");
			printf("\n SourceType:   %d", pContext->inputSettings.resource);
			if(pContext->inputSettings.resource == BTST_RESOURCE_FILE)
				printf("\n filename:       %s", pContext->inputSettings.fname);
#if NEXUS_HAS_FRONTEND
			if(pContext->inputSettings.resource == BTST_RESOURCE_QAM)
				printf("\n freq: %d MHz;  qam mode: %d", pContext->inputSettings.freq, pContext->inputSettings.qamMode);
#endif

			if(pContext->inputSettings.resource != BTST_RESOURCE_HDMI)
			{
				printf("\n Streamtype:  %d \n encoderVideoCodec:  %d", pContext->inputSettings.eStreamType, pContext->inputSettings.eVideoCodec);
				printf("\n vPid:  %4d;  PcrPid:   %4d", pContext->inputSettings.iVideoPid, pContext->inputSettings.iPcrPid);
			}
			if(pContext->inputSettings.bAudioInput)
			{
				printf("\n Enable Audio channel Pid: %d\n audio codec: %d           ", pContext->inputSettings.iAudioPid, pContext->inputSettings.eAudioCodec);
			}
			printf("\n Config %1d", pContext->inputSettings.bConfig);
			printf("\n****************************************************************\n");
			

			/*Encode settings */
			BKNI_Snprintf(pContext->encodeSettings.fname, 256, "/data/BAT/encode.ts");
#ifdef NEXUS_NUM_DSP_VIDEO_ENCODERS
			pContext->encodeSettings.displayFormat=NEXUS_VideoFormat_e480p;
#else
			pContext->encodeSettings.displayFormat=NEXUS_VideoFormat_e720p;
#endif
			pContext->encodeSettings.encoderFrameRate=NEXUS_VideoFrameRate_e29_97;			
			pContext->encodeSettings.encoderGopStructureFramesP=23;
			pContext->encodeSettings.encoderGopStructureFramesB=0;
			pContext->encodeSettings.encoderVideoCodec=NEXUS_VideoCodec_eH264;
#ifdef NEXUS_NUM_DSP_VIDEO_ENCODERS
			pContext->encodeSettings.encoderProfile=NEXUS_VideoCodecProfile_eBaseline;
			pContext->encodeSettings.encoderLevel=NEXUS_VideoCodecLevel_e30;
			pContext->encodeSettings.encoderBitrate=400000;
#else
			pContext->encodeSettings.encoderProfile=NEXUS_VideoCodecProfile_eMain;
			pContext->encodeSettings.encoderLevel=NEXUS_VideoCodecLevel_e40;
			pContext->encodeSettings.encoderBitrate=2000000;
#endif			 
			pContext->encodeSettings.bCustom=false;
			pContext->encodeSettings.bAudioEncode=false;
			pContext->encodeSettings.encoderAudioCodec=NEXUS_AudioCodec_eAc3;

			printf("\n****************************************************************\n");
			printf("\n Encode Parameters\n");
			printf("\n filename:  %s", pContext->encodeSettings.fname);
			printf("\n display format:  %d", pContext->encodeSettings.displayFormat);
			printf("\n Frame Rate: %d\n Bit Rate: %d", pContext->encodeSettings.encoderFrameRate, pContext->encodeSettings.encoderBitrate);
			printf("\n P frames: %d;     B frames: %d", pContext->encodeSettings.encoderGopStructureFramesP, pContext->encodeSettings.encoderGopStructureFramesB);
			printf("\n Video Codec: %4d;  Profile: %4d;  Level: %4d", pContext->encodeSettings.encoderVideoCodec, pContext->encodeSettings.encoderProfile,pContext->encodeSettings.encoderLevel );
			if(pContext->encodeSettings.bAudioEncode)
				printf("\n Audio Codec: %4d", pContext->encodeSettings.encoderAudioCodec);
			printf("\n****************************************************************\n");
	}
	if ( argc >= 2) {
		for(i=1; i<argc; i++) {
			if(!strcmp("-h",argv[i])) {
				print_usage();
				return 0;
			}
			if(!strcmp("-fifo",argv[i])) {
				g_bFifo = true;
				fprintf(stderr, "Enabled fifo record...\n");
			}
			if(!strcmp("-NRTwrap",argv[i])) {
				g_bNonRealTimeWrap = true;
				fprintf(stderr, "Enabled NRT wraparound...\n");
			}
			if(!strcmp("-autoquit",argv[i])) {
				g_bAutoQuit= true;
				fprintf(stderr, "Enabled auto quit...\n");
			}
			if(!strcmp("-cc",argv[i])) {
				g_bEncodeCC = true;
				fprintf(stderr, "Enabled CC data transcode...\n");
			}
			if(!strcmp("-tsud",argv[i])) {
				g_bTsUserData = true;
				fprintf(stderr, "Enabled TS layer user data transcode...\n");
			}
			if(!strcmp("-nodspmix",argv[i])) {
				g_bNoDspMixer = true;
				fprintf(stderr, "No DSP mixer for audio transcode...\n");
			}
			if(!strcmp("-multichan",argv[i])) {
				g_bMultiChanAudio = true;
				fprintf(stderr, "Multi-channel format for audio source. must have fw mixer..\n");
			}
			if(!strcmp("-msaudio",argv[i])) {
				g_bSecondAudio = true;
				fprintf(stderr, "Multi-stream audio source. must have fw mixer.\n");
			}
			if(!strcmp("-nrtRate",argv[i])) {
				g_nrtRate = strtoul(argv[++i], NULL, 0);
				fprintf(stderr, "NRT rate = %u x\n", g_nrtRate);
			}
			if(!strcmp("-msp",argv[i])) {
				g_msp = strtoul(argv[++i], NULL, 0);
				fprintf(stderr, "Mux Service Period = %u ms.\n", g_msp);
			}
			if(!strcmp("-rsp",argv[i])) {
				g_rsp = strtoul(argv[++i], NULL, 0);
				fprintf(stderr, "Record Service Period = %u ms.\n", g_rsp);
			}
			if(!strcmp("-type",argv[i])) {
				i++;
				if(!strcmp("single",argv[i])) {
					g_eVideoEncoderType = NEXUS_VideoEncoderType_eSingle;
				}
				else if(!strcmp("multiNRT",argv[i])) {
					g_eVideoEncoderType = NEXUS_VideoEncoderType_eMultiNonRealTime;
				}
				else {
					g_eVideoEncoderType = NEXUS_VideoEncoderType_eMulti;
				}
				fprintf(stderr, "Set video encoder type %d...\n", g_eVideoEncoderType);
			}
			if(!strcmp("-scr",argv[i])) {
				g_bScriptMode = true;
				g_scriptModeSleepTime = strtoul(argv[++i], NULL, 0);
				fprintf(stderr, "Enabled script mode and quit in %u seconds...\n", g_scriptModeSleepTime);
			}
		}
		if(g_bSecondAudio) g_bNoDspMixer = false;
		config_xcoder_context(pContext);/* transcoder 0 context user settings */
	}

	/* init platform */
	B_Os_Init();
	NEXUS_Platform_GetDefaultSettings(&platformSettings);
	/* enable frontend if the 1st xcoder needs it for now; not always enable to avoid slow frontend init for other cases;
	   TODO: init frontend in case 1st disable but 2nd enables frontend. */
	platformSettings.openFrontend = (pContext->inputSettings.resource == BTST_RESOURCE_QAM);
	/* audio PI supports 4 by default; we need one extra mixers for each transcoders; */
#ifdef NEXUS_HAS_VIDEO_ENCODER
	platformSettings.audioModuleSettings.maxAudioDspTasks += NEXUS_NUM_VIDEO_ENCODERS;
	platformSettings.audioModuleSettings.numCompressedBuffers += NEXUS_NUM_VIDEO_ENCODERS;
#endif
	NEXUS_Platform_Init(&platformSettings);
	NEXUS_Platform_GetConfiguration(&g_platformConfig);
	for(i=0; i< NEXUS_NUM_VIDEO_ENCODERS; i++) {
		xcodeContext[i].mutexStarted = B_Mutex_Create(NULL);
		xcodeContext[i].contextId = i;
	}

again:
	/* bringup the transcode context */
	bringup_transcode(pContext);

	/****************************************************
	 * set up xcoder0 loopback decode/display for debug purpose
	 */
#if BTST_ENABLE_XCODE_LOOPBACK
	xcode_loopback_setup(pContext);
	g_loopbackStarted = true;
	g_loopbackXcodeId = 0;
#endif

	/****************************************************
	 *                       key handler                                                 *
	 *****************************************************/
	/* wait for 'q' to exit the test */
	if(g_bScriptMode)
	{
		keyHandler(xcodeContext);
	}
	else if (pContext->inputSettings.bConfig)
	{
		B_ThreadSettings threadSettings;
		B_ThreadHandle keyHandle;

		g_eotEvent = B_Event_Create(NULL);
		B_Event_Reset(g_eotEvent);
		B_Thread_GetDefaultSettings(&threadSettings);
		threadSettings.terminate = true;
		keyHandle = B_Thread_Create("key handler", (B_ThreadFunc)keyHandler, xcodeContext, &threadSettings);
		NEXUS_Profile_Start();
		while(B_Event_Wait(g_eotEvent, B_WAIT_FOREVER)!=B_ERROR_SUCCESS);
		NEXUS_Profile_Stop("transcode_ts");
		B_Thread_Destroy(keyHandle);
		B_Event_Destroy(g_eotEvent);
	}
	else
	{
		NEXUS_VideoEncoderSettings videoEncoderConfig;
		NEXUS_DisplaySettings displaySettings;
		BDBG_WRN(("Auto test iteration: %d\n", iteration));
		BKNI_Sleep(30000);/* 30 seconds */
		/* To Do add BVN status and ViCE status check */
		/* Enable MADR*/
		window_mad(pContext->windowTranscode, false, true, false);
		/* change resolution */
		NEXUS_Display_GetSettings(pContext->displayTranscode, &displaySettings);
		displaySettings.format = NEXUS_VideoFormat_e480p;
		NEXUS_Display_SetSettings(pContext->displayTranscode, &displaySettings);
		BDBG_WRN(("format switch to 480p60"));
		NEXUS_VideoEncoder_GetSettings(pContext->videoEncoder, &videoEncoderConfig);
		videoEncoderConfig.bitrateMax = 2000000;
		NEXUS_VideoEncoder_SetSettings(pContext->videoEncoder, &videoEncoderConfig);
		BDBG_WRN(("bitrate switch to 2Mbps"));
		NEXUS_VideoEncoder_GetSettings(pContext->videoEncoder, &videoEncoderConfig);
		videoEncoderConfig.variableFrameRate = false;
		videoEncoderConfig.frameRate = NEXUS_VideoFrameRate_e30;
		videoEncoderConfig.streamStructure.framesP = 29;
		videoEncoderConfig.streamStructure.framesB = 2;
		NEXUS_VideoEncoder_SetSettings(pContext->videoEncoder, &videoEncoderConfig);
		BDBG_WRN(("frame rate switch to 30fps\n"));
		BKNI_Sleep(30000);/* 30 seconds */

		/* change resolution */
		NEXUS_Display_GetSettings(pContext->displayTranscode, &displaySettings);
		displaySettings.format = NEXUS_VideoFormat_e720p;
		NEXUS_Display_SetSettings(pContext->displayTranscode, &displaySettings);
		BDBG_WRN(("format switch to 720p60"));
		NEXUS_VideoEncoder_GetSettings(pContext->videoEncoder, &videoEncoderConfig);
		videoEncoderConfig.bitrateMax = 10000000; /* 10Mbps */
		NEXUS_VideoEncoder_SetSettings(pContext->videoEncoder, &videoEncoderConfig);
		BDBG_WRN(("bitrate switch to 10 Mbps"));
		NEXUS_VideoEncoder_GetSettings(pContext->videoEncoder, &videoEncoderConfig);
		videoEncoderConfig.variableFrameRate = false;
		videoEncoderConfig.frameRate = NEXUS_VideoFrameRate_e60;
		videoEncoderConfig.streamStructure.framesP = 29;
		videoEncoderConfig.streamStructure.framesB = 0;
		NEXUS_VideoEncoder_SetSettings(pContext->videoEncoder, &videoEncoderConfig);
		BDBG_WRN(("frame rate switch to 60fps"));
		BKNI_Sleep(30000);
		g_keyReturn = iteration > TEST_ITERATIONS ? 'q' : 'c'; /* continue looping until quit */
	}

#if BTST_ENABLE_XCODE_LOOPBACK /* bringdown loopback path */
	if(g_loopbackStarted) {
		xcode_loopback_shutdown(&xcodeContext[g_loopbackXcodeId]);
	}
#endif

	for(i = 0; i < NEXUS_NUM_VIDEO_ENCODERS; i++) {
		B_Mutex_Lock(xcodeContext[i].mutexStarted);
		if(xcodeContext[i].bStarted) {
			shutdown_transcode(&xcodeContext[i]);
		}
		if(xcodeContext[i].nrtEofHandle) {
			B_Event_Set(xcodeContext[i].eofEvent);
			BKNI_Sleep(1);
			B_Thread_Destroy(xcodeContext[i].nrtEofHandle);
			xcodeContext[i].nrtEofHandle = NULL;
		}
		if(xcodeContext[i].eofEvent) {
			B_Event_Destroy(xcodeContext[i].eofEvent);
			xcodeContext[i].eofEvent = NULL;
		}
		B_Mutex_Unlock(xcodeContext[i].mutexStarted);
	}

	if(g_keyReturn != 'q')
	{
		iteration++;
		BDBG_WRN(("Start %d iteration.....", iteration));
		pContext = &xcodeContext[0];
		goto again;
	}

	for(i=1; i< NEXUS_NUM_VIDEO_ENCODERS; i++) {
		B_Mutex_Destroy(xcodeContext[i].mutexStarted);
	}
	/* uninit platform */
	NEXUS_Platform_Uninit();
	B_Os_Uninit();
#endif
	return 0;
}

/* End of file */
