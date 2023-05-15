/***************************************************************************
 *  Copyright (c) 2003-2012, Broadcom Corporation
 *  All Rights Reserved
 *  Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bsettop_decode.c $
 * $brcm_Revision: 253 $
 * $brcm_Date: 10/26/12 12:14p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/src/nexus/bsettop_decode.c $
 * 
 * 253   10/26/12 12:14p mward
 * SW7435-433: Downgrade "Invalid channel mode" in
 * baudio_decode_get_status() to BDBG_WRN.
 * 
 * 253   10/26/12 12:11p mward
 * SW7435-433: Downgrade "Invalid channel mode" in
 * baudio_decode_get_status() to BDBG_WRN.
 * 
 * 252   10/24/12 3:18p spothana
 * SW7420-2357: add a check for pcr PID channel before configuring the stc
 * channel.
 * 
 * 251   9/11/12 4:51p spothana
 * SW7420-2346: disable lip sync precision and mute control if only audio
 * or video ES streams are present in a program.
 * 
 * 250   7/6/12 6:07p dlwin
 * SW7425-3354: Updated SettopAPI to set the video fifo threshold to a
 * minimum of 1.5MB.  Setting of less than 1.5MB causes failure to
 * playback some streams.
 * 
 * 249   5/14/12 8:04p bandrews
 * SW7552-271: remove warnings
 * 
 * 248   5/14/12 7:31p bandrews
 * SW7552-271: disassociate pid channel from stc channel before freeing it
 * 
 * 247   5/8/12 5:08p jgarrett
 * SW7425-2746: Removing inputs to SPDIF when enabling transcoding
 * 
 * 246   5/8/12 4:07p jgarrett
 * SW7231-721: Clearing secondary decoder if we drop out of passthrough
 * mode without closing the pidchannel
 * 
 * 245   5/2/12 6:46p jgarrett
 * SW7425-2802: Removing RAP_XXX build variables for audio post processing
 * in favor of runtime detection
 * 
 * 244   4/26/12 9:25p sgadara
 * SW7358-272: [7358,7420,7405] Add check as only one of the audio volume
 * algorithms SRS Truvolume or Dolby Volume or AVL is supported a time
 * 
 * 243   4/25/12 12:15p sgadara
 * SW7358-272: [7358] Support AVL on Brutus
 * 
 * 242   3/26/12 2:03p erickson
 * SW7405-2364: silence WRN
 * 
 * 241   3/22/12 5:25p mward
 * SW7435-55: Correct handling of enhancement PID channel.
 * 
 * 240   3/15/12 12:19p erickson
 * SW7401-4517: remove old code
 * 
 * 239   2/24/12 1:56p xhuang
 * SW7552-162 : show DRA and DTS audio information
 * 
 * 238   2/22/12 1:19p jgarrett
 * SW7346-233: Clearing SPDIF/HDMI forced PCM state when audio decode is
 * stopped
 * 
 * 237   2/21/12 4:23p jgarrett
 * SW7425-2389: Coverity CID 34213
 * 
 * 236   2/13/12 1:36p jessem
 * SW7425-2390: Fixed coverity issues.
 *
 * 235   12/16/11 7:18p jgarrett
 * SW7425-1406: Adding OGG/Vorbis
 *
 * 234   12/14/11 6:26p jgarrett
 * SW7346-176: Removing code that disabled 5.1 PCM on 7344 and 7346
 *
 * 233   11/3/11 12:10p erickson
 * SW7231-391: use NEXUS_Timebase_e0 as proper base
 *
 * 232   10/11/11 1:20p jgarrett
 * SW7425-880: Disabling compressed passthrough for AAC, DRA, Cook, and
 * ADPCM codecs
 *
 * 231   9/19/11 3:57p randyjew
 * SW7344-134: Fix for pcr pid similiar to video/audio pid, open as video
 * or audio pic to avoid duplicate pid channel.
 *
 * 230   6/30/11 6:53p jgarrett
 * SW7425-828: Opening PCR PID Channel for playback streams to quiet
 * warnings from the media library
 *
 * 229   5/27/11 9:41p vishk
 * SW7231-32: Implement the Nexus code to support the 3461 frontend chip.
 * Implement brutus support for the downstream core.
 *
 * 228   5/18/11 7:24p jgarrett
 * SW7231-148: Adding RMFF and Cook support
 *
 * 227   5/6/11 4:11p jgarrett
 * SW7346-191: Adding warning messages when DTS/AC3 transcode can not be
 * performed
 *
 * 226   3/21/11 3:17p nitinb
 * SW7550-709: Set fields of union
 * NEXUS_AudioDecoderStatus.codecStatus.mpeg only for mpeg codec
 *
 * 225   3/8/11 10:58a katrep
 * SW7231-39:disbale wmapassthrough on 7231
 *
 * 224   2/28/11 10:47a nickh
 * SW7422-273: Check for AAC codecs before setting AAC downmix
 *
 * 223   2/18/11 1:20p nickh
 * SW7422-268: Temporarily disable WMAPro passthru until is implemented in
 * the FW
 *
 * 222   2/15/11 1:59p nickh
 * SW7422-213: Add SVC/MVC support
 *
 * 222   2/15/11 1:54p nickh
 * SW7422-213: Add SVC/MVC support
 *
 * 221   12/15/10 11:47a jrubio
 * SW7346-5: do not call MAD when B_HAS_MAD is not enabled
 *
 * 220   11/1/10 11:37a jgarrett
 * SW7408-137: Adding compressed audio capture option
 *
 * 219   7/28/10 4:25p jgarrett
 * SW7405-4664: Not enabling multichannel decode for PCM/PCMWAV
 *
 * 218   7/8/10 4:35p jgarrett
 * SW7408-62: Fixing codec-specifics
 *
 * 217   5/25/10 4:09p calvinho
 * SW7125-279: Coverity Defect ID:20338 CHECKED_RETURN
 *
 * 216   4/14/10 11:58a calvinho
 * SW7125-279: Fix Coverity Defects ID:20338, 20337 in bsettop_decode.c
 *
 * 215   3/19/10 5:59p jgarrett
 * SW7405-4040: Ensuring audio and video are on the same timebase
 *
 * 214   3/16/10 10:47a jtna
 * SW3556-1051: revert back to host-reordering
 *
 * 213   2/16/10 5:42p jtna
 * SW3556-1051: use video decoder timestamp reordering
 *
 * 212   2/11/10 3:52p bandrews
 * SW7550-241: clean up logic for assigning ASTM audio channels in
 * set_astm
 *
 * 211   1/28/10 4:23p mward
 * SW7400-2650:  DTS passthru support for 7400.  Passthru only, no decode.
 *
 * 210   1/27/10 10:01a jgarrett
 * SW7550-213: Merge to main branch
 *
 * 209   1/26/10 6:02p jgarrett
 * SW7405-3635: Allowing fixed audio decode->display mapping to avoid re-
 * attaching outputs on every bdecode_stop/bdecode_start
 *
 * SW7550-213/1   1/25/10 5:22p jianweiz
 * SW7550-213: set the stereo to dual mono when initialize the audio
 * decoder.
 *
 * 208   12/9/09 12:03p gmohile
 * SW7408-1 : Add 7408 support
 *
 * 207   12/3/09 2:45p jgarrett
 * SW7405-3511: Adding compressed mute for outputs
 *
 * 206   12/2/09 7:31p katrep
 * SW7405-3158:support for DTS certification app
 *
 * 205   12/1/09 7:26p katrep
 * SW7405-3515:Add downmix modes for AAC formats
 *
 * 204   11/25/09 5:36p katrep
 * SW7405-2740: Add support for WMA pro drc
 *
 * 203   11/24/09 2:41p katrep
 * SW7405-3419: Add decode_aac_96khz run time environment variable
 *
 * 202   11/23/09 5:52p katrep
 * SW7405-3457:Add DRA support
 *
 * 201   11/19/09 8:02p jgarrett
 * SW7405-3357: Adding audio output routing for decode and pcm playback
 * prior to start
 *
 * 200   11/12/09 11:41a gmohile
 * SW7401-4340 : 7401 only supports simul mode for compressed DDP output
 *
 * 199   11/10/09 5:54p mward
 * SW7400-2574: HD/SD sharpness optimizations incorporated in VDC,
 * NEXUS_PictureCtrl_SetSharpnessValue() is no longer needed for simple
 * sharpness setting.
 *
 * 198   11/10/09 4:57p mward
 * SW7400-2574: When using the  NEXUS_PictureCtrl_SetSharpnessValue(), the
 * quantity of sharpness must be set using lumaControlGain and
 * chromaControlGain, the nPictureCtrlCommonSettings.sharpness is ignored
 * by BVDC_Window_SetSharpness.
 *
 * 197   11/9/09 6:05p mward
 * SW7400-2574:  Sharpness control optimization depends on source format,
 * check and update when it changes.
 *
 * 196   11/2/09 11:16a erickson
 * SW7405-3315: remove playbackOverride. use NEXUS_StcChannelMode instead.
 *
 * 195   10/23/09 4:26p jtna
 * SW7405-3265: use same stc channel when using video primer FCC
 *
 * SW7405-3158/1   10/7/09 7:03p mward
 * SW7405-3158: Remember audio decoder start settings for restart which
 * may be necessary changing DTS decoder settings.
 *
 * 194   9/29/09 11:48a jgarrett
 * SW7405-3100: Adding display underflows and display drops to video
 * decode status
 *
 * 193   9/18/09 5:49p mward
 * SW7400-2374:  For 7400, even timebases are used with decoder0, use
 * audio pll0, odd with decoder1, use pll1.
 *
 * 192   9/14/09 4:26p jgarrett
 * SW7405-3011: Fixing sync enable flag
 *
 * 191   9/14/09 3:05p jgarrett
 * SW7405-3011: Splitting sync control into precision and basic
 *
 * 190   9/11/09 1:02p jjordan
 * SW7405-2969: only alter the fifoThreshold for the PVR playback case,
 * delayed startup of audio is not a problem for non-playback
 *
 * 189   9/11/09 10:05a jtna
 * SWDEPRECATED-3910: improve FCC performance
 *
 * 188   9/10/09 6:04p mward
 * SW7400-2374:  For 7400, select audio output PLL to match timebase
 * number.
 *
 * 187   9/10/09 5:19p jgarrett
 * SW7405-3011: Adding sync options to bsettop_init_settings
 *
 * 187   9/10/09 5:17p jgarrett
 * SW7405-3011: Adding sync options to bsettop_init_settings
 *
 * 186   9/10/09 1:43p mward
 * SW7400-2387: When forcing to PCM due to algorithm, remove inputs to
 * HDMI, as well as to SPDIF.
 *
 * 185   9/4/09 11:45a jgarrett
 * SWDEPRECATED-3910: Moving fifo_size to bsettop_init_settings
 *
 * 184   9/1/09 12:47p katrep
 * SW7405-2934:Enable DTS encode for DTS Streams
 *
 * 183   8/25/09 12:52p jtna
 * SWDEPRECATED-3910: video primer fast channel change feature
 *
 * 182   8/25/09 11:24a katrep
 * SW7405-2934:add suppoort for audio capture to a file
 *
 * 181   8/18/09 6:44p katrep
 * PR56109: Use calllbacks instead of using events for callbacks to the
 * app
 *
 * 180   8/18/09 9:59a mward
 * PR56489: Single PLL setting for DDP passthru now available on 7400.
 *
 * 179   8/4/09 2:26p mward
 * PR56560: Passthru for SPDIF (and HDMI) should be disabled for MPEG
 * decoder (on 7400).
 *
 * 178   8/4/09 12:10p katrep
 * PR57100: Preserve dolby voulme,srs voulme setting across channel
 * changes.
 *
 * 177   7/30/09 7:26p mward
 * PR57243:  Don't need to check for non-null audio->decode in
 * baudio_decode_get_status().
 *
 * 176   7/30/09 7:10p mward
 * PR57242:  PR57241: Don't alter fifoThreshold if
 * NEXUS_VideoDecoder_GetStatus() failed.
 *
 * 175   7/30/09 6:45p mward
 * PR57241:  Failure of NEXUS_VideoDecoder_GetStatus() in
 * bdecode_p_enable_audio is harmless.
 *
 * 174   7/27/09 1:46p mward
 * PR45344: In some cases, (e.g. Brutus B) there may be no SPDIF,
 * settings.spdif may be NULL.
 *
 * 173   7/17/09 7:33p bandrews
 * PR49215: added back accidentally removed static functions
 *
 * 172   7/17/09 7:24p bandrews
 * PR49215: astm dvr support
 *
 * 171   7/16/09 4:12p jjordan
 * PR56878: only set non-zero PTS offset for "live playback" mode
 *
 * 170   7/7/09 6:21p katrep
 * PR45344: spdif status reporting issue
 *
 * 169   7/1/09 9:25a erickson
 * PR56446: added drop_field_mode=y env variable
 *
 * 168   6/30/09 4:27p katrep
 * PR55809: add support for avi pcm
 *
 * 167   6/30/09 12:17p mward
 * PR52051: PR 52051: Add dialog normalization setting.
 *
 * 166   6/29/09 6:01p mward
 * PR 56384:  Set PLL based on decoder, not output timebase.
 *
 * 165   6/29/09 10:12a erickson
 * PR56373: fix bdecode_close so that we can call bsettop_init a second
 * time
 *
 * 164   6/26/09 6:49p mward
 * PR 56384:  Moved pll setting for 7400 from Nexus.
 *
 * 163   6/26/09 9:32a katrep
 * PR55074: Add DTS and PCM audio support for get status api
 *
 * 161   6/19/09 6:15p mward
 * PR 47739: 7400 DDP passthru requires RAP DSP context 1, used by Nexus
 * audio decoder 1.  Swap it for use by passthru if it is not currently
 * in use.
 *
 * 160   6/19/09 6:01p mward
 * PR 47729: DDP passthru for 7400 requires use of RAP channel 1, use
 * Nexus audio decoder 1 for it, swap if it's not in use for second
 * decode.
 *
 * 159   6/18/09 7:06p mward
 * PR 56145: Hook up the Dolby codec settings, and initialize them to the
 * settings used in the native Settop API implementation (DRC mode RF,
 * standard downmix mode.
 *
 * 157   6/2/09 7:02p mward
 * PR 50613:  Move open for TruVolume and Dolby Volume to audio start and
 * close to audio stop, to avoid leaking the post-proc stage handle for
 * detached audio decoder.
 *
 * 156   5/15/09 3:32p jjordan
 * PR52687: PR52687: improve AAC downmix API
 * PR52687: Add method to set audio downmix type
 *
 * 155   5/8/09 5:45p mward
 * PR47739: Move create/register for audio source change event to audio
 * start and unregister/destroy to audio stop, as in native Settop API to
 * avoid leaking event  of detached audio decoder.
 *
 * 154   5/6/09 4:27p jjordan
 * PR50291: add Wide GA mode
 *
 * 153   4/29/09 2:14p katrep
 * PR54656: Fixed assert when sync_disabled=y
 *
 * 152   4/29/09 12:53p katrep
 * PR54656: astm diabled by default, use astm_enabled=y to enable
 *
 * 151   4/29/09 11:15a katrep
 * PR51938: Changing decode default volume to max. 0 mean half volume
 *
 * 150   4/27/09 6:42p katrep
 * PR54586: crash with astm_disabled=y
 *
 * 149   4/27/09 3:29p katrep
 * PR54586: Some cleanup and better debug
 *
 * 148   4/24/09 7:41p katrep
 * PR54586: Add astm support,add audio_fifo_size,add use_first_pts run
 * time variables
 *
 * 147   4/14/09 12:08p katrep
 * PR52862: Fixed coveiry issue
 *
 * 146   3/26/09 6:12p katrep
 * PR53631: Preserve brutus audio setting after forced PCM on WMA channel.
 *
 * 145   3/24/09 6:18p katrep
 * PR52384: Do not start dts or ac3 encode if codec doesnt support encode.
 *
 * 144   3/23/09 3:45p katrep
 * PR52340:More updates for true and dolby vol
 *
 * 143   3/23/09 3:05p katrep
 * PR52340: Allow cascading for true and dolby voules if both are compiled
 * in
 *
 * 142   3/20/09 1:09p katrep
 * PR52340: Support srs true volume and dolby volume through brutus ans
 * settop api
 *
 * 141   3/6/09 11:26a katrep
 * PR50291: Fixed kernel crash in cases where secondary audio is not
 * started (WMA)
 *
 * 140   3/3/09 2:55p jjordan
 * PR50291: PR50291: conform to naming convention
 * PR50291: add enable of audio wide gross adjustment mode
 *
 * 139   3/3/09 9:42a katrep
 * PR52340: mixer was not getting configured when src_true_volume not set
 *
 * 138   3/2/09 6:10p katrep
 * PR52340: Add support srs_true_volume
 *
 * 137   1/27/09 7:21p katrep
 * PR43688: Add support for downmix and dual momo modes
 *
 * 136   1/19/09 4:26p mward
 * PR47739: settings.dvi may be null, this display might not have HDMI
 * output.
 *
 * 135   1/15/09 9:44a katrep
 * PR50063: Fixed compiler warning in non debug builds
 *
 * 134   1/14/09 6:46p katrep
 * PR44767: Set the correct dual mono mode
 *
 * 133   1/9/09 12:09a erickson
 * PR50063: separate B_HAS_DTS_ENCODE and B_HAS_AC3_ENCODE logic
 *
 * 132   1/8/09 7:41p katrep
 * PR50063: force PCM on DVD LPCM
 *
 * 131   1/8/09 12:21p katrep
 * PR50063: Add suppport dor Ac3 encode in Settop shim and burtus.
 *
 * 130   12/15/08 4:49p katrep
 * PR50222: Add independent delay support for audio dacs. dac_output_delay
 *
 * 129   12/3/08 5:29p jjordan
 * PR49939: fix b/w video on IP channel change (during "auto-pids")
 *
 * 128   12/3/08 4:57p katrep
 * PR49939: Set the frame mastership when decode started on the first
 * window
 *
 * 127   12/2/08 12:10p jgarrett
 * PR 47993: Allowing PID values > 0x1fff for VOB streams
 *
 * 126   11/25/08 11:19a mward
 * PR47739: On 7400, a display might not have HDMI.
 *
 * 125   11/7/08 1:11p katrep
 * PR47230: Fixed non ip build
 *
 * 124   11/6/08 8:36p katrep
 * PR47230: reworked the output port timebase logic
 *
 * 123   11/6/08 4:48p katrep
 * PR47230: Only pcr pid chanel only for live and ip modes
 *
 * 122   11/6/08 11:12a ssood
 * PR48146: TSM broken for local & HTTP playback of ASF & MP4 files
 *
 * 121   11/4/08 2:56p katrep
 * PR48292: Fixed coverity issues
 *
 * 120   11/3/08 2:56p erickson
 * PR47230: fix non-TTS pacing case
 *
 * 119   10/31/08 12:29p jjordan
 * PR47230: Add TTS Pacing
 *
 * 118   10/21/08 6:49p katrep
 * PR47198: Add ind delay for audio decoder
 *
 * 117   10/21/08 6:21p katrep
 * PR43187: Report fifo size and depth on the rave context even if the
 * decoder is not started.
 *
 * 116   10/13/08 5:20p katrep
 * PR46677: Loss of audio on spdif if previous channel was wma.
 *
 * 115   10/8/08 3:44p ssood
 * PR47521: lowered the MAX ERROR to match the OFFSET THRESHOLD in PCR
 * Offset block
 *
 * 114   9/25/08 3:31p katrep
 * PR47073: fifo_size runtime varibale required to increase the CDB size
 * to decode some VC1 streams.
 *
 * 113   9/24/08 7:14p katrep
 * PR43187: lowered the cdb threshold for non ip cases. Improve loss of
 * audio duration due audio decode stop and decode start.
 *
 * 112   9/23/08 6:02p katrep
 * PR47229: Fixed segfault on channel change.
 *
 * 111   9/23/08 1:13p katrep
 * PR47229: WMA pro should be processed as simul mode not passthrough mode
 *
 * 110   9/23/08 10:55a erickson
 * PR47231: add preroll_rate env variable for test
 *
 * 109   9/22/08 10:03a erickson
 * PR47145: fix warning
 *
 * 108   9/17/08 6:20p katrep
 * PR47061: Add support for DTS encode
 *
 * 107   9/16/08 12:33p katrep
 * PR46896: Fixed application crash,during pcm playback
 *
 * 106   9/15/08 5:27p jrubio
 * PR46925:  fix PVR_SUPPORT=n compile issue
 *
 * 105   9/12/08 8:47a erickson
 * PR44959: protect bdecode_set if decode->video_decode is NULL
 *
 * 104   9/11/08 2:04p vishk
 * PR44959: impl bdecode_settings.stop_mode
 *
 * 103   9/10/08 7:57p katrep
 * PR46732: Force pcm for wma compressed. Fixed no audio on hdmi during
 * pcm pb and i2s capture.
 *
 * 102   9/8/08 10:26a erickson
 * PR46534: fix order of NEXUS_VideoWindow_RemoveInput for clone window
 *
 * 101   8/26/08 8:30p katrep
 * PR45577: Moved the HDMI audio ouput port settings from dispaly to
 * decode.
 *
 * 100   8/26/08 6:47p katrep
 * PR46089: Use the multichannel settings at the decoder open time.
 *
 * 99   8/22/08 7:49p erickson
 * PR45959: StillDecoder is now normal part of VideoDecoder module
 *
 * 98   8/22/08 7:02p jgarrett
 * PR 44767: Reworking dual-mono logic
 *
 * 97   8/13/08 7:08p katrep
 * PR45577: Add supoort for 5.1 pcm audio on hdmi
 *
 * 96   8/5/08 6:19p zmao
 * PR45311: Fix a build warning
 *
 * 95   8/5/08 5:55p zmao
 * PR45311: Remove C++ style comments
 *
 * 94   8/5/08 5:25p zmao
 * PR45311:Add status->downmix_mode and status->layer in
 * baudio_decode_get_status function
 *
 * 93   8/5/08 11:37a jrubio
 * PR45361: remove "//"
 *
 * 92   8/1/08 5:37p erickson
 * PR45361: c89 fixes
 *
 * 91   7/29/08 1:08p katrep
 * PR45045:WMA compressed request should yield no audio.
 *
 * 90   7/29/08 8:11a erickson
 * PR42739: support IP TV platforms with no HdmiOutput
 *
 * 89   7/24/08 11:52a erickson
 * PR44959: fix DTV builds
 *
 * 88   7/22/08 5:50p katrep
 * PR45045:WMA/WMA passthrough support
 *
 * 87   7/21/08 10:31a erickson
 * PR44959: fix DTV builds
 *
 * 86   7/18/08 4:22p erickson
 * PR44959: impl bdecode_settings.stop_mode
 *
 * 85   7/17/08 6:40p jgarrett
 * PR 44509: Converting audio decoder open to new API
 *
 * 84   7/16/08 3:37p ssood
 * PR42739: fixing warning message for non-ip builds
 *
 * 83   7/14/08 4:42p ssood
 * PR42739: fixing another non-ip compile error
 *
 * 82   7/14/08 4:33p ssood
 * PR42739: fixing non-ip compile error
 *
 * 81   7/14/08 1:59p ssood
 * PR42739: getting the audio dac handle via display structure
 *
 * 80   7/14/08 1:28p ssood
 * PR42739: further changes to support high jitter support for live IP
 * playback
 *
 * 80   7/14/08 1:20p ssood
 * PR42739: further changes to support high jitter support for live IP
 * playback
 *
 * 79   7/11/08 8:49a ssood
 * PR42739: fixing IP related build error
 *
 * 78   7/10/08 10:19p ssood
 * PR42739: Adding support to absorb high jitter for IPSTB
 *
 * 77   7/9/08 4:10p katrep
 * PR43490: Add support 656/i2s tuning using analog tune api.
 *
 * 76   7/2/08 11:43a erickson
 * PR44482: check dvi handle before using it
 *
 * 75   6/30/08 2:06p erickson
 * PR44194: fix bdecode_p_set_video_input
 *
 * 74   6/30/08 10:03a erickson
 * PR44321: check return code
 *
 * 73   6/27/08 1:41p erickson
 * PR44194: impl hold_last_picture in Settop API shim
 *
 * 72   6/25/08 2:32p erickson
 * PR39453: add hold_last_picture option
 *
 * 71   6/23/08 12:24p erickson
 * PR43399: fix non-HDMI platforms
 *
 * 70   6/20/08 6:27p katrep
 * PR43399: Do not set HDMI for compressed output if HDMI is not
 * connected.
 *
 ***************************************************************************/
#include "bsettop_impl.h"
#include <string.h>
#include <stdlib.h> /* atoi */

BDBG_MODULE(decode);


/* TODO: rework the attach/detach method. don't allow dangling decoders. */
struct bdecode g_decode[B_MAX_DECODES+B_MAX_STILL_DECODES];
struct bvideo_decode g_video_decode[B_MAX_DECODES];
#if BCHP_CHIP == 7400
struct baudio_decode g_audio_decode[B_MAX_DECODES];
struct baudio_decode g_secondary_audio_decode[B_MAX_DECODES];
#else
struct baudio_decode g_audio_decode[1];
struct baudio_decode g_secondary_audio_decode[1];
#endif


/* Max Supported Jitter for the IPSTB Configuration */
#define IP_NETWORK_JITTER 300

static bresult bdecode_p_open_sync_channel(bdecode_t decode);
static bresult bdecode_p_close_sync_channel(bdecode_t decode);
static bresult bdecode_p_set_sync_channel(bdecode_t decode, bool video,bool audio_pcm,bool audio_passthrough);
static bresult bdecode_p_unset_sync_channel(bdecode_t decode,bool video,bool audio_pcm,bool audio_passthrough);
static bresult bdecode_set_audio_ip_tsm_settings(baudio_decode_t audio_decode);

static bresult bdecode_p_open_astm(bdecode_t decode);
static bresult bdecode_p_close_astm(bdecode_t decode);
static bresult bdecode_p_set_astm(bdecode_t decode, bool video,bool audio_pcm,bool audio_passthrough);
static bresult bdecode_p_unset_astm(bdecode_t decode,bool video,bool audio_pcm,bool audio_passthrough);
static bresult bdecode_p_start_astm(bdecode_t decode);
static bresult bdecode_p_stop_astm(bdecode_t decode);
static NEXUS_AudioDecoderDolbyStereoDownmixMode baudio_dolby_stereo_downmix_mode_to_nexus_ac3plus_stereo_mode(baudio_dolby_stereo_downmix_mode stereo_downmix_mode);
static NEXUS_AudioDecoderDolbyDrcMode baudio_dolby_drc_mode_to_nexus_ac3plus_comp_mode(baudio_dolby_drc_mode drc_mode);
int baudio_capture_open(baudio_decode_t audio);
int baudio_capture_close(baudio_decode_t audio);
int baudio_capture_start(baudio_decode_t audio);
int baudio_capture_stop(baudio_decode_t audio);
static bresult bdecode_p_set_audio_display(baudio_decode_t audio_decode, bdisplay_t display);



typedef enum
{
    SOURCE_CHANGE_EVENT_AUDIO,
    SOURCE_CHANGE_EVENT_VIDEO

}SourceChangeEventType;

static int g_open_audio_decodes=0;
static NEXUS_AudioDecoderHandle g_n_audio_decoder[NEXUS_NUM_AUDIO_DECODERS];
static NEXUS_AudioDecoderHandle g_audio_passthrough;
static baudio_decode_t g_audio_passthrough_owner;

static void bdecode_p_still_handler(void *context,int param);

static void bdecode_p_source_changed(void *context, int param)
{
    bdecode_t decode = (bdecode_t)context;
    SourceChangeEventType event = (SourceChangeEventType)param;
    if (event==SOURCE_CHANGE_EVENT_VIDEO && decode->video_decode)
    {
        BDBG_MSG(("video source change"));
        b_callback_fire(decode->video_decode->sourceChangedCallback);
        /*B_Event_Set(decode->video_decode->sourceChangedEvent);*/
    }
    else if (event==SOURCE_CHANGE_EVENT_AUDIO && decode->audio_decode && decode->settings.audio.source_changed)
    {
        BDBG_MSG(("audio source change"));
        /*B_Event_Set(decode->audio_decode->sourceChangedEvent);*/
        b_callback_fire(decode->audio_decode->sourceChangedCallback);
    }
}

static void bdecode_p_video_source_changed(void *context,int param )
{
    bdecode_t decode = context;
    BSTD_UNUSED(param);
    BDBG_ASSERT(NULL != decode);

    if ( decode->settings.video.source_changed )
    {
        /* b_unlock();*/
        (*decode->settings.video.source_changed)(decode->settings.callback_context);
        /*b_lock();*/
    }
}

static void bdecode_p_audio_source_changed(void *context,int param)
{
    bdecode_t decode = context;
    BSTD_UNUSED(param);
    BDBG_ASSERT(NULL != decode);
    if ( decode->settings.audio.source_changed )
    {
        /*b_unlock();*/
        (*decode->settings.audio.source_changed)(decode->settings.callback_context);
        /*b_lock();*/
    }
}

void bdecode_p_init()
{
    unsigned i;

    BDBG_ASSERT(g_open_audio_decodes == 0);
    BDBG_ASSERT(g_n_audio_decoder[0] == NULL);
    BDBG_ASSERT(g_audio_passthrough == NULL);

    BKNI_Memset(g_decode, 0, sizeof(g_decode));
    BKNI_Memset(g_video_decode, 0, sizeof(g_video_decode));
    BKNI_Memset(g_audio_decode, 0, sizeof(g_audio_decode));
    BKNI_Memset(g_secondary_audio_decode, 0, sizeof(g_secondary_audio_decode));

    for (i=0;i<B_MAX_DECODES;i++)
    {
        g_decode[i].volume.left = BAUDIO_LEVEL_MAX;
        g_decode[i].volume.right = BAUDIO_LEVEL_MAX;

        g_decode[i].video_decode = &g_video_decode[i];
        g_decode[i].video_decode->decode = &g_decode[i];
#if BCHP_CHIP == 7400
        g_decode[i].audio_decode = &g_audio_decode[i];
        g_decode[i].audio_decode->decode = &g_decode[i];
        g_secondary_audio_decode[i].decode = &g_decode[i];
        g_decode[i].audio_decode->secondary_audio_decode = &g_secondary_audio_decode[i];
#else
        if (i == 0)
        {
            g_decode[i].audio_decode = &g_audio_decode[i];
            g_decode[i].audio_decode->decode = &g_decode[i];
            g_secondary_audio_decode[i].decode = &g_decode[i];
            g_decode[i].audio_decode->secondary_audio_decode = &g_secondary_audio_decode[i];
        }
#endif
       g_decode[i].settings.audio.codec.dolby.drc_mode = baudio_dolby_drc_mode_rf;
       g_decode[i].settings.audio.codec.dolby.stereo_downmix_mode = baudio_dolby_stereo_downmix_mode_standard;
       g_decode[i].settings.audio.codec.dolby.dialog_norm = true;
       g_decode[i].settings.audio.dualmono = baudio_dualmono_stereo;
        if (bsettop_get_config("dolby_volume")){
           g_decode[i].settings.audio.postproc.dolby.volume_proc_enable = true;
        }
        else {
           g_decode[i].settings.audio.postproc.dolby.volume_proc_enable = false;
        }

        if (bsettop_get_config("hold_last_picture"))
        {
            g_decode[i].settings.stop_mode = bdecode_stop_mode_last_picture;
        }
    }
}

void bdecode_p_uninit()
{
    unsigned i;

    for (i=0;i<B_MAX_DECODES;i++)
    {
        if( g_decode[i].video_decode && g_decode[i].video_decode->nVideoDecoder)
        {
            bdecode_stop(&g_decode[i]);
            bdecode_close(&g_decode[i]);
        }
    }
}

bdecode_t bdecode_open(bobject_t decode_id)
{
    unsigned index = B_ID_GET_INDEX(decode_id);
    bdecode_t decode;
    NEXUS_VideoDecoderSettings  nVideoDecodeSettings;
    NEXUS_AudioDecoderSettings  nAudioDecodeSettings;
    NEXUS_StcChannelSettings    nStcChannelSettings;
    NEXUS_AudioDecoderOpenSettings nAudioDecodeOpenSettings;
    bsettop_init_settings initSettings;
    const char* fifo_size=NULL;

    bsettop_get_init_settings(&initSettings);

    if (index >= B_MAX_DECODES+B_MAX_STILL_DECODES)
    {
        BSETTOP_ERROR(berr_not_available);
        return NULL;
    }
    decode = &g_decode[index];

    if (index >= B_MAX_DECODES)
    {
        /* still decode */
        /* TODO: must open after main decode */
        if ( NULL == g_decode[index-B_MAX_DECODES].video_decode ||
             NULL == g_decode[index-B_MAX_DECODES].video_decode->nVideoDecoder )
        {
            BDBG_ERR(("Must open corresponding video decoder first"));
            BSETTOP_ERROR(berr_not_available);
            return NULL;
        }
        if ( g_decode[index].nStillDecoder )
        {
            BDBG_ERR(("Still decoder already open"));
            BSETTOP_ERROR(berr_not_available);
            return NULL;
        }
        decode->nStillDecoder = NEXUS_StillDecoder_Open(g_decode[index-B_MAX_DECODES].video_decode->nVideoDecoder, 0, NULL);
        if (!decode->nStillDecoder)
        {
            BSETTOP_ERROR(berr_not_available);
            return NULL;
        }
#if 0
        decode->stillEvent = B_Event_Create(NULL);
        if ( NULL == decode->stillEvent )
        {
            NEXUS_StillDecoder_Close(decode->nStillDecoder);
            decode->nStillDecoder = NULL;
            BSETTOP_ERROR(berr_external_error);
            return NULL;
        }
        decode->stillEventId = b_event_register(decode->stillEvent, bdecode_p_still_handler, decode);
        if ( NULL == decode->stillEventId )
        {
            B_Event_Destroy(decode->stillEvent);
            NEXUS_StillDecoder_Close(decode->nStillDecoder);
            decode->nStillDecoder = NULL;
            BSETTOP_ERROR(berr_external_error);
            return NULL;
        }
#endif
        decode->stillEventCallback= b_callback_create(decode,bdecode_p_still_handler,decode,0);
        if(decode->stillEventCallback== NULL)
        {
            BSETTOP_ERROR(berr_external_error);
            return NULL;
        }
        BDBG_MSG(("bdecode_open %p (still decoder)", decode));
        return decode;
    }

    /* open a sync channnel */
    bdecode_p_open_sync_channel(decode);

    if (decode->video_decode)
    {
        NEXUS_VideoDecoderOpenSettings openSettings;

        if (decode->video_decode->nVideoDecoder)
        {
            BSETTOP_ERROR(berr_not_available);
            return NULL;
        }

        NEXUS_VideoDecoder_GetDefaultOpenSettings(&openSettings);
        fifo_size=bsettop_get_config("fifo_size");
        if(fifo_size)
        {
            openSettings.fifoSize = atoi(fifo_size)*1000;
            BDBG_WRN(("****************************************"));
            BDBG_WRN(("* Changing Video FIFO size to %d bytes *",openSettings.fifoSize));
            BDBG_WRN(("****************************************"));
        }
        else if ( initSettings.video_fifo_size )
        {
            openSettings.fifoSize = initSettings.video_fifo_size;
        }

        decode->video_decode->nVideoDecoder = NEXUS_VideoDecoder_Open(index, &openSettings);
        if (!decode->video_decode->nVideoDecoder)
        {
            BSETTOP_ERROR(berr_external_error);
            goto error;
        }


        decode->video_decode->sourceChangedCallback = b_callback_create(decode->video_decode,bdecode_p_video_source_changed,decode,0);
        if(NULL == decode->video_decode->sourceChangedCallback)
        {
            BSETTOP_ERROR(berr_out_of_memory);
            goto error;
        }
        BDBG_MSG(("video_decode->sourceChangedCallback = %p, video_decode->decode = %p",decode->video_decode->sourceChangedCallback,decode->video_decode->decode));
#if 0
        decode->video_decode->sourceChangedEvent = B_Event_Create(NULL);
        if ( NULL == decode->video_decode->sourceChangedEvent )
        {
            BSETTOP_ERROR(berr_external_error);
            goto error;
        }
        decode->video_decode->sourceChangedEventId = b_event_register(decode->video_decode->sourceChangedEvent,
                                                                      bdecode_p_video_source_changed,
                                                                      decode);
        if ( NULL == decode->video_decode->sourceChangedEventId )
        {
            BSETTOP_ERROR(berr_external_error);
            goto error;
        }
#endif

        /* add source changed callback */
        NEXUS_VideoDecoder_GetSettings(decode->video_decode->nVideoDecoder,&nVideoDecodeSettings);

        nVideoDecodeSettings.sourceChanged.callback = bdecode_p_source_changed;
        nVideoDecodeSettings.sourceChanged.context = decode;
        nVideoDecodeSettings.sourceChanged.param = SOURCE_CHANGE_EVENT_VIDEO;

        if (bsettop_get_config("drop_field_mode")) {
            nVideoDecodeSettings.dropFieldMode = true;
        }

        if (NEXUS_VideoDecoder_SetSettings(decode->video_decode->nVideoDecoder,&nVideoDecodeSettings))
        {
            BSETTOP_ERROR(berr_external_error);
            goto error;
        }

#if B_HAS_ANALOG
        decode->video_decode->nAnalogVideoDecoder = NEXUS_AnalogVideoDecoder_Open(index, NULL);
        if (!decode->video_decode->nAnalogVideoDecoder)
        {
            BSETTOP_ERROR(berr_external_error);
            goto error;
        }
#endif

        decode->video_decode->nVideoImageInput = NEXUS_VideoImageInput_Open(index, NULL);
        if (!decode->video_decode->nVideoImageInput)
        {
            BSETTOP_ERROR(berr_external_error);
            goto error;
        }
    }

    if (decode->audio_decode)
    {
        NEXUS_Error rc;
        NEXUS_AudioDecoder_GetDefaultOpenSettings(&nAudioDecodeOpenSettings);
        nAudioDecodeOpenSettings.multichannelFormat=NEXUS_AudioMultichannelFormat_e5_1;
        if(bsettop_get_config("spdif_output_delay")|| bsettop_get_config("hdmi_output_delay")||bsettop_get_config("dac_output_delay"))
            nAudioDecodeOpenSettings.independentDelay=true;

        fifo_size=bsettop_get_config("audio_fifo_size");
        if(fifo_size)
        {
            nAudioDecodeOpenSettings.fifoSize = atoi(fifo_size)*1000;
            BDBG_WRN(("****************************************"));
            BDBG_WRN(("* Changing Audio FIFO size to %d bytes *",nAudioDecodeOpenSettings.fifoSize));
            BDBG_WRN(("****************************************"));
        }
        else if ( initSettings.audio_fifo_size )
        {
            nAudioDecodeOpenSettings.fifoSize = initSettings.audio_fifo_size;
        }

        g_n_audio_decoder[index] = decode->audio_decode->nAudioDecoder = NEXUS_AudioDecoder_Open(index, &nAudioDecodeOpenSettings);
        if (!decode->audio_decode->nAudioDecoder)
        {
            BSETTOP_ERROR(berr_external_error);
            goto error;
        }
        NEXUS_AudioDecoder_GetSettings(decode->audio_decode->nAudioDecoder, &nAudioDecodeSettings);

        nAudioDecodeSettings.sourceChanged.callback = bdecode_p_source_changed;
        nAudioDecodeSettings.sourceChanged.context = decode;
        nAudioDecodeSettings.sourceChanged.param = SOURCE_CHANGE_EVENT_AUDIO;

        rc = NEXUS_AudioDecoder_SetSettings(decode->audio_decode->nAudioDecoder, &nAudioDecodeSettings);
        if (rc)
        {
            BSETTOP_ERROR(berr_external_error);
            goto error;
        }

        if ( bconfig->features.dts_transcode )
        {
            decode->audio_decode->nDtsEncoder=NEXUS_DtsEncode_Open(NULL);
            if (!decode->audio_decode->nDtsEncoder)
            {
                BSETTOP_ERROR(berr_external_error);
                goto error;
            }
        }

        if ( bconfig->features.ac3_transcode )
        {
            decode->audio_decode->nAc3Encoder=NEXUS_Ac3Encode_Open(NULL);
            if (!decode->audio_decode->nAc3Encoder)
            {
                BSETTOP_ERROR(berr_external_error);
                goto error;
            }
        }

        if ( bconfig->features.srs_volume )
        {
            NEXUS_TruVolumeSettings trueVolSettings;
            NEXUS_TruVolume_GetDefaultSettings(&trueVolSettings);
            trueVolSettings.enabled = false;
            /*
            trueVolSettings.enabled = audio_decode->decode->settings.audio.postproc.srs.tru_volume_enable;
            BDBG_WRN(("true volume %s",audio_decode->decode->settings.audio.postproc.srs.tru_volume_enable?"on":"off"));
            */
            decode->audio_decode->nTrueVolume = NEXUS_TruVolume_Open(&trueVolSettings);
            if (!decode->audio_decode->nTrueVolume)
            {
                BSETTOP_ERROR(berr_external_error);
                goto error;
            }
        }

        if ( bconfig->features.dolby_volume )
        {
            NEXUS_DolbyVolumeSettings dolbyVolSettings;
            NEXUS_DolbyVolume_GetDefaultSettings(&dolbyVolSettings);
            dolbyVolSettings.enabled = false;
            /*
            dolbyVolSettings.enabled = audio_decode->decode->settings.audio.postproc.dolby.volume_proc_enable;
            BDBG_WRN(("dolby volume %s",audio_decode->decode->settings.audio.postproc.dolby.volume_proc_enable?"on":"off"));
            */
            decode->audio_decode->nDolbyVolume = NEXUS_DolbyVolume_Open(&dolbyVolSettings);
            if (!decode->audio_decode->nDolbyVolume)
            {
                BSETTOP_ERROR(berr_external_error);
                goto error;
            }
        }

        if ( bconfig->features.auto_volume_level )
        {
            NEXUS_AutoVolumeLevelSettings nAutoVolumeLevelSettings;
            NEXUS_AutoVolumeLevel_GetDefaultSettings(&nAutoVolumeLevelSettings);
            nAutoVolumeLevelSettings.enabled = false;

            decode->audio_decode->nAutoVolumeLevel = NEXUS_AutoVolumeLevel_Open(&nAutoVolumeLevelSettings);
            if (!decode->audio_decode->nAutoVolumeLevel)
            {
                BSETTOP_ERROR(berr_external_error);
                goto error;
            }
        }

        if(bsettop_get_config("audio_capture")||bsettop_get_config("audio_capture_compressed"))
        {
            baudio_capture_open(decode->audio_decode);
        }

#if B_HAS_EXTERNAL_ANALOG
        decode->audio_decode->i2sInput = NEXUS_I2sInput_Open(0, NULL);
        if ( NULL == decode->audio_decode->i2sInput )
        {
            BSETTOP_ERROR(berr_external_error);
            goto error;
        }
#endif
    }
    if ( 0 == g_open_audio_decodes++ )
    {
        NEXUS_AudioDecoder_GetDefaultOpenSettings(&nAudioDecodeOpenSettings);

        if(bsettop_get_config("spdif_output_delay")|| bsettop_get_config("hdmi_output_delay"))
            nAudioDecodeOpenSettings.independentDelay=true;

        fifo_size=bsettop_get_config("audio_fifo_size");
        if(fifo_size)
        {
            nAudioDecodeOpenSettings.fifoSize = atoi(fifo_size)*1000;
            BDBG_WRN(("****************************************"));
            BDBG_WRN(("* Changing Audio FIFO size to %d bytes *",nAudioDecodeOpenSettings.fifoSize));
            BDBG_WRN(("****************************************"));
        }

        g_n_audio_decoder[NEXUS_NUM_AUDIO_DECODERS-1] = g_audio_passthrough = NEXUS_AudioDecoder_Open(NEXUS_NUM_AUDIO_DECODERS-1, &nAudioDecodeOpenSettings);
        if ( NULL == g_audio_passthrough )
        {
            BSETTOP_ERROR(berr_external_error);
            goto error;
        }
        g_audio_passthrough_owner = NULL;
    }
    NEXUS_StcChannel_GetDefaultSettings(index, &nStcChannelSettings);
    /* each decode has its own timebase */
    nStcChannelSettings.timebase = NEXUS_Timebase_e0 + index;
    if(bsettop_get_config("use_first_pts"))
    {
        BDBG_WRN(("PTS auto behavior default %d,changing to %d",nStcChannelSettings.modeSettings.Auto.behavior,NEXUS_StcChannelAutoModeBehavior_eFirstAvailable));
        nStcChannelSettings.modeSettings.Auto.behavior=NEXUS_StcChannelAutoModeBehavior_eFirstAvailable;
    }
    decode->stcChannel = NEXUS_StcChannel_Open(index, &nStcChannelSettings);
    if (!decode->stcChannel)
    {
        BSETTOP_ERROR(berr_external_error);
        goto error;
    }

    decode->settings.audio.secondary_audio_program = -1;

    if ( bsettop_get_config("astm_enabled") )
    {
        bdecode_p_open_astm(decode);
    }

    BDBG_MSG(("bdecode_open %p", decode));
    return decode;

    error:
    bdecode_close(decode);
    return NULL;
}

void bdecode_mosaic_settings_init(bdecode_t parent, bdecode_mosaic_settings *settings)
{
    BSTD_UNUSED(parent);
    BKNI_Memset(settings, 0, sizeof(*settings));
}

bdecode_t bdecode_open_mosaic(bdecode_t parent,bobject_t decode_id, const bdecode_mosaic_settings *settings)
{
    BSTD_UNUSED(parent);
    BSTD_UNUSED(decode_id);
    BSTD_UNUSED(settings);
    /* TODO */
    BSETTOP_ERROR(berr_not_supported);
    return NULL;
}

void bdecode_close(bdecode_t decode)
{
    BDBG_MSG(("bdecode_close %p", decode));
    if (decode->started) {
        bdecode_stop(decode);
    }

    if (decode->nStillDecoder) {
        /* just in case it wasn't already stopped */
        NEXUS_StillDecoder_Stop(decode->nStillDecoder);
        NEXUS_StillDecoder_Close(decode->nStillDecoder);
#if 0
        b_event_unregister(decode->stillEventId);
        B_Event_Destroy(decode->stillEvent);
#endif
        b_callback_destroy(decode->stillEventCallback);
        decode->nStillDecoder = NULL;
        return;
    }

    bdecode_p_close_astm(decode);

    /* destroy the any syncChannel is till open */
    bdecode_p_close_sync_channel(decode);

    if (decode->video_decode && decode->video_decode->nVideoDecoder) {
        NEXUS_VideoInput_Shutdown(NEXUS_VideoDecoder_GetConnector(decode->video_decode->nVideoDecoder));
        NEXUS_VideoDecoder_Close(decode->video_decode->nVideoDecoder);
        decode->video_decode->nVideoDecoder = NULL;
#if 0
        if ( decode->video_decode->sourceChangedEventId )
        {
            b_event_unregister(decode->video_decode->sourceChangedEventId);
        }
        if ( decode->video_decode->sourceChangedEvent )
        {
            B_Event_Destroy(decode->video_decode->sourceChangedEvent);
        }
#endif
        if (decode->video_decode->sourceChangedCallback)
        {
            b_callback_destroy(decode->video_decode->sourceChangedCallback);
        }

    }
#if B_HAS_ANALOG
    if (decode->video_decode && decode->video_decode->nAnalogVideoDecoder) {
        NEXUS_VideoInput_Shutdown(NEXUS_AnalogVideoDecoder_GetConnector(decode->video_decode->nAnalogVideoDecoder));
        NEXUS_AnalogVideoDecoder_Close(decode->video_decode->nAnalogVideoDecoder);
        decode->video_decode->nAnalogVideoDecoder = NULL;
    }
#endif

    if (decode->video_decode && decode->video_decode->nVideoImageInput) {
        NEXUS_VideoImageInput_Close(decode->video_decode->nVideoImageInput);
        decode->video_decode->nVideoImageInput = NULL;
    }

    if (decode->audio_decode) {

#if NEXUS_NUM_AUDIO_CAPTURES
        if(bsettop_get_config("audio_capture")||bsettop_get_config("audio_capture_compressed"))
        {
            if(decode->audio_decode->nAudioCapture)
            {
                baudio_capture_close(decode->audio_decode);
            }
        }
#endif

        if (decode->audio_decode->nAudioDecoder) {
            int i;
            NEXUS_AudioInput_Shutdown(NEXUS_AudioDecoder_GetConnector(decode->audio_decode->nAudioDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
            NEXUS_AudioInput_Shutdown(NEXUS_AudioDecoder_GetConnector(decode->audio_decode->nAudioDecoder, NEXUS_AudioDecoderConnectorType_eCompressed));
            NEXUS_AudioInput_Shutdown(NEXUS_AudioDecoder_GetConnector(decode->audio_decode->nAudioDecoder, NEXUS_AudioDecoderConnectorType_eMultichannel));
            NEXUS_AudioDecoder_Close(decode->audio_decode->nAudioDecoder);
            for (i=0;i<NEXUS_NUM_AUDIO_DECODERS;i++)
            {
                if (decode->audio_decode->nAudioDecoder == g_n_audio_decoder[i])
                {
                    g_n_audio_decoder[i] = NULL;
                }
            }
            decode->audio_decode->nAudioDecoder = NULL;
        }
        if (decode->audio_decode->nDtsEncoder) {
            NEXUS_DtsEncode_RemoveAllInputs(decode->audio_decode->nDtsEncoder);
            NEXUS_AudioInput_Shutdown(NEXUS_DtsEncode_GetConnector(decode->audio_decode->nDtsEncoder));
            NEXUS_DtsEncode_Close(decode->audio_decode->nDtsEncoder);
            decode->audio_decode->nDtsEncoder=NULL;
        }

        if (decode->audio_decode->nAc3Encoder) {
            NEXUS_Ac3Encode_RemoveAllInputs(decode->audio_decode->nAc3Encoder);
            NEXUS_AudioInput_Shutdown(NEXUS_Ac3Encode_GetConnector(decode->audio_decode->nAc3Encoder));
            NEXUS_Ac3Encode_Close(decode->audio_decode->nAc3Encoder);
            decode->audio_decode->nAc3Encoder=NULL;
        }

        if (decode->audio_decode->nTrueVolume) {
            NEXUS_TruVolume_RemoveAllInputs(decode->audio_decode->nTrueVolume);
            NEXUS_AudioInput_Shutdown(NEXUS_TruVolume_GetConnector(decode->audio_decode->nTrueVolume));
            NEXUS_TruVolume_Close(decode->audio_decode->nTrueVolume);
            decode->audio_decode->nTrueVolume=NULL;
        }

        if (decode->audio_decode->nDolbyVolume) {
            NEXUS_DolbyVolume_RemoveAllInputs(decode->audio_decode->nDolbyVolume);
            NEXUS_AudioInput_Shutdown(NEXUS_DolbyVolume_GetConnector(decode->audio_decode->nDolbyVolume));
            NEXUS_DolbyVolume_Close(decode->audio_decode->nDolbyVolume);
            decode->audio_decode->nDolbyVolume=NULL;
        }

        if (decode->audio_decode->nAutoVolumeLevel) {
            NEXUS_AutoVolumeLevel_RemoveAllInputs(decode->audio_decode->nAutoVolumeLevel);
            NEXUS_AudioInput_Shutdown(NEXUS_AutoVolumeLevel_GetConnector(decode->audio_decode->nAutoVolumeLevel));
            NEXUS_AutoVolumeLevel_Close(decode->audio_decode->nAutoVolumeLevel);
            decode->audio_decode->nAutoVolumeLevel=NULL;
        }

#if B_HAS_EXTERNAL_ANALOG
        if ( decode->audio_decode->i2sInput )
        {
            NEXUS_AudioInput_Shutdown(NEXUS_I2sInput_GetConnector(decode->audio_decode->i2sInput));
            NEXUS_I2sInput_Close(decode->audio_decode->i2sInput);
            decode->audio_decode->i2sInput = NULL;
        }
#endif
    }
    if ( 0 == --g_open_audio_decodes )
    {
        if ( NULL != g_audio_passthrough )
        {
            int i;
            NEXUS_AudioInput_Shutdown(NEXUS_AudioDecoder_GetConnector(g_audio_passthrough, NEXUS_AudioDecoderConnectorType_eCompressed));
            NEXUS_AudioDecoder_Close(g_audio_passthrough);
            for (i=0;i<NEXUS_NUM_AUDIO_DECODERS;i++)
            {
                if (g_audio_passthrough == g_n_audio_decoder[i])
                {
                    g_n_audio_decoder[i] = NULL;
                }
            }
            g_audio_passthrough = NULL;
        }
    }

    if (decode->stcChannel) {
        NEXUS_StcChannel_Close(decode->stcChannel);
        decode->stcChannel = NULL;
    }
}


static bresult bdecode_p_set_downmix(bdecode_t decode)
{
    NEXUS_AudioDecoderSettings nDecoderSettings;
    NEXUS_AudioDecoderCodecSettings nCodecSettings;
    int32_t leftVolume=0,rightVolume=0;
    int nLeftVolume=0,nRightVolume=0;
    NEXUS_Error rc=0;

    if (decode->window && decode->audio_decode && decode->audio_decode->nAudioDecoder)
    {
        NEXUS_AudioDecoder_GetSettings(decode->audio_decode->nAudioDecoder,&nDecoderSettings);

        nLeftVolume = b_volume2nexus(decode->volume.left, false, &leftVolume);
        nRightVolume = b_volume2nexus(decode->volume.right, false, &rightVolume);

        nDecoderSettings.muted = decode->volume.muted;
        /* BDBG_MSG(("left 0x%08x %d",nLeftVolume,decode->volume.left)); */

        switch (decode->settings.audio.downmix)
        {
        case baudio_downmix_stereo: /* If applicable, down mix source to stereo output [default]. */
        case baudio_downmix_multichannel: /* Output decoded data as-is. If number of channels exceeds HW capabilities,
                                            downmix to match maximum number of output channels. No audio will be lost. */
        case baudio_downmix_none: /* Output decoded data as-is. If number of channels exceeds HW capabilities,
                                    additional audio channels will be lost. */
            nDecoderSettings.volumeMatrix[NEXUS_AudioChannel_eLeft][NEXUS_AudioChannel_eLeft] = nLeftVolume;
            nDecoderSettings.volumeMatrix[NEXUS_AudioChannel_eRight][NEXUS_AudioChannel_eRight] = nRightVolume;
            nDecoderSettings.volumeMatrix[NEXUS_AudioChannel_eLeft][NEXUS_AudioChannel_eRight] = 0;
            nDecoderSettings.volumeMatrix[NEXUS_AudioChannel_eRight][NEXUS_AudioChannel_eLeft] = 0;
            BDBG_WRN(("Setting standard downmix mode"));
            break;

        case baudio_downmix_left: /* Take left channel and duplicate to left/right outputs. */
            nDecoderSettings.volumeMatrix[NEXUS_AudioChannel_eLeft][NEXUS_AudioChannel_eLeft] = nLeftVolume;
            nDecoderSettings.volumeMatrix[NEXUS_AudioChannel_eRight][NEXUS_AudioChannel_eRight] = 0;
            nDecoderSettings.volumeMatrix[NEXUS_AudioChannel_eLeft][NEXUS_AudioChannel_eRight] = 0;
            nDecoderSettings.volumeMatrix[NEXUS_AudioChannel_eRight][NEXUS_AudioChannel_eLeft] = nRightVolume;
            BDBG_WRN(("Setting downmix left"));

            break;
        case baudio_downmix_right: /* Take right channel and duplicate to left/right outputs. */
            nDecoderSettings.volumeMatrix[NEXUS_AudioChannel_eLeft][NEXUS_AudioChannel_eLeft] = 0;
            nDecoderSettings.volumeMatrix[NEXUS_AudioChannel_eRight][NEXUS_AudioChannel_eRight] = nRightVolume;
            nDecoderSettings.volumeMatrix[NEXUS_AudioChannel_eLeft][NEXUS_AudioChannel_eRight] = nLeftVolume;
            nDecoderSettings.volumeMatrix[NEXUS_AudioChannel_eRight][NEXUS_AudioChannel_eLeft] = 0;
            BDBG_WRN(("Setting downmix right"));
            break;
        case baudio_downmix_monomix:  /* Mix left and right channels and output result to left/right outputs. This only applies if the stream
                                        is a dual mono stream. If not, it will be equivalent to baudio_downmix_stereo. */
            nDecoderSettings.volumeMatrix[NEXUS_AudioChannel_eLeft][NEXUS_AudioChannel_eLeft] = nLeftVolume/2;
            nDecoderSettings.volumeMatrix[NEXUS_AudioChannel_eRight][NEXUS_AudioChannel_eRight] = nRightVolume/2;
            nDecoderSettings.volumeMatrix[NEXUS_AudioChannel_eLeft][NEXUS_AudioChannel_eRight] = nLeftVolume/2;
            nDecoderSettings.volumeMatrix[NEXUS_AudioChannel_eRight][NEXUS_AudioChannel_eLeft] = nRightVolume/2;
            BDBG_WRN(("Setting downmix monomix"));
            break;
        }
        rc = NEXUS_AudioDecoder_SetSettings(decode->audio_decode->nAudioDecoder,&nDecoderSettings);
        if (rc) return BSETTOP_ERROR(rc);

        if (decode->audio_decode->restartSettings.codec >= NEXUS_AudioCodec_eAac && decode->audio_decode->restartSettings.codec <= NEXUS_AudioCodec_eAacPlusAdts)
        {
            switch (decode->settings.audio.codec.aac.downmix)
            {
            case baudio_aac_downmix_matrix:
                NEXUS_AudioDecoder_GetCodecSettings(decode->audio_decode->nAudioDecoder, NEXUS_AudioCodec_eAacAdts, &nCodecSettings);
                nCodecSettings.codecSettings.aac.downmixMode = NEXUS_AudioDecoderAacDownmixMode_eMatrix;
                rc = NEXUS_AudioDecoder_SetCodecSettings(decode->audio_decode->nAudioDecoder, &nCodecSettings);
                if (rc) return BSETTOP_ERROR(rc);
                NEXUS_AudioDecoder_GetCodecSettings(decode->audio_decode->nAudioDecoder, NEXUS_AudioCodec_eAacLoas, &nCodecSettings);
                nCodecSettings.codecSettings.aac.downmixMode = NEXUS_AudioDecoderAacDownmixMode_eMatrix;
                rc = NEXUS_AudioDecoder_SetCodecSettings(decode->audio_decode->nAudioDecoder, &nCodecSettings);
                if (rc) return BSETTOP_ERROR(rc);
                NEXUS_AudioDecoder_GetCodecSettings(decode->audio_decode->nAudioDecoder, NEXUS_AudioCodec_eAacPlusAdts, &nCodecSettings);
                nCodecSettings.codecSettings.aacPlus.downmixMode = NEXUS_AudioDecoderAacDownmixMode_eMatrix;
                rc = NEXUS_AudioDecoder_SetCodecSettings(decode->audio_decode->nAudioDecoder, &nCodecSettings);
                if (rc) return BSETTOP_ERROR(rc);
                NEXUS_AudioDecoder_GetCodecSettings(decode->audio_decode->nAudioDecoder, NEXUS_AudioCodec_eAacPlusLoas, &nCodecSettings);
                nCodecSettings.codecSettings.aacPlus.downmixMode = NEXUS_AudioDecoderAacDownmixMode_eMatrix;
                rc = NEXUS_AudioDecoder_SetCodecSettings(decode->audio_decode->nAudioDecoder, &nCodecSettings);
                if (rc) return BSETTOP_ERROR(rc);
                BDBG_WRN(("Setting downmix type to (BRCM) matrix"));
                break;
            case baudio_aac_downmix_arib:
                NEXUS_AudioDecoder_GetCodecSettings(decode->audio_decode->nAudioDecoder, NEXUS_AudioCodec_eAacAdts, &nCodecSettings);
                nCodecSettings.codecSettings.aac.downmixMode = NEXUS_AudioDecoderAacDownmixMode_eArib;
                rc = NEXUS_AudioDecoder_SetCodecSettings(decode->audio_decode->nAudioDecoder, &nCodecSettings);
                if (rc) return BSETTOP_ERROR(rc);
                NEXUS_AudioDecoder_GetCodecSettings(decode->audio_decode->nAudioDecoder, NEXUS_AudioCodec_eAacLoas, &nCodecSettings);
                nCodecSettings.codecSettings.aac.downmixMode = NEXUS_AudioDecoderAacDownmixMode_eArib;
                rc = NEXUS_AudioDecoder_SetCodecSettings(decode->audio_decode->nAudioDecoder, &nCodecSettings);
                if (rc) return BSETTOP_ERROR(rc);
                NEXUS_AudioDecoder_GetCodecSettings(decode->audio_decode->nAudioDecoder, NEXUS_AudioCodec_eAacPlusAdts, &nCodecSettings);
                nCodecSettings.codecSettings.aacPlus.downmixMode = NEXUS_AudioDecoderAacDownmixMode_eArib;
                rc = NEXUS_AudioDecoder_SetCodecSettings(decode->audio_decode->nAudioDecoder, &nCodecSettings);
                if (rc) return BSETTOP_ERROR(rc);
                NEXUS_AudioDecoder_GetCodecSettings(decode->audio_decode->nAudioDecoder, NEXUS_AudioCodec_eAacPlusLoas, &nCodecSettings);
                nCodecSettings.codecSettings.aacPlus.downmixMode = NEXUS_AudioDecoderAacDownmixMode_eArib;
                rc = NEXUS_AudioDecoder_SetCodecSettings(decode->audio_decode->nAudioDecoder, &nCodecSettings);
                if (rc) return BSETTOP_ERROR(rc);
                BDBG_WRN(("Setting downmix type to ARIB"));
                break;
            case baudio_aac_downmix_ltrt:
                NEXUS_AudioDecoder_GetCodecSettings(decode->audio_decode->nAudioDecoder, NEXUS_AudioCodec_eAacAdts, &nCodecSettings);
                nCodecSettings.codecSettings.aac.downmixMode = NEXUS_AudioDecoderAacDownmixMode_eLtRt;
                rc = NEXUS_AudioDecoder_SetCodecSettings(decode->audio_decode->nAudioDecoder, &nCodecSettings);
                if (rc) return BSETTOP_ERROR(rc);
                NEXUS_AudioDecoder_GetCodecSettings(decode->audio_decode->nAudioDecoder, NEXUS_AudioCodec_eAacLoas, &nCodecSettings);
                nCodecSettings.codecSettings.aac.downmixMode = NEXUS_AudioDecoderAacDownmixMode_eLtRt;
                rc = NEXUS_AudioDecoder_SetCodecSettings(decode->audio_decode->nAudioDecoder, &nCodecSettings);
                if (rc) return BSETTOP_ERROR(rc);
                NEXUS_AudioDecoder_GetCodecSettings(decode->audio_decode->nAudioDecoder, NEXUS_AudioCodec_eAacPlusAdts, &nCodecSettings);
                nCodecSettings.codecSettings.aacPlus.downmixMode = NEXUS_AudioDecoderAacDownmixMode_eLtRt;
                rc = NEXUS_AudioDecoder_SetCodecSettings(decode->audio_decode->nAudioDecoder, &nCodecSettings);
                if (rc) return BSETTOP_ERROR(rc);
                NEXUS_AudioDecoder_GetCodecSettings(decode->audio_decode->nAudioDecoder, NEXUS_AudioCodec_eAacPlusLoas, &nCodecSettings);
                nCodecSettings.codecSettings.aacPlus.downmixMode = NEXUS_AudioDecoderAacDownmixMode_eLtRt;
                rc = NEXUS_AudioDecoder_SetCodecSettings(decode->audio_decode->nAudioDecoder, &nCodecSettings);
                if (rc) return BSETTOP_ERROR(rc);
                BDBG_WRN(("Setting downmix type to ARIB"));
                break;
            }
        }

        bdisplay_p_set_compressed_mute(decode->window->display, decode->volume.muted);
    }
    return 0;

}

void bdecode_get(bdecode_t decode, bdecode_settings *settings)
{
    *settings = decode->settings;
}

bresult bdecode_set(bdecode_t decode, const bdecode_settings *settings)
{

    NEXUS_AudioDecoderSettings nDecoderSettings;

    if ( decode->audio_decode )
    {
        if ( decode->settings.audio.display != settings->audio.display )
        {
            if ( decode->started )
            {
                BDBG_ERR(("Cannot change audio display while running."));
                return BSETTOP_ERROR(berr_busy);
            }
            if ( settings->audio.display && settings->audio.display->mixerInputsStarted > 0 )
            {
                return BSETTOP_ERROR(berr_busy);
            }
            if ( decode->settings.audio.display && decode->settings.audio.display->mixerInputsStarted )
            {
                return BSETTOP_ERROR(berr_busy);
            }
            if ( decode->settings.audio.display )
            {
                bdecode_p_set_audio_display(decode->audio_decode, NULL);
                decode->settings.audio.display = NULL;
            }
            if ( settings->audio.display )
            {
                bdecode_p_set_audio_display(decode->audio_decode, settings->audio.display);
                decode->settings.audio.display = settings->audio.display;
            }
        }
    }

    /* set dual mono mode */
    if (decode->audio_decode && decode->audio_decode->nAudioDecoder)
    {
        if ( decode->settings.audio.dualmono != settings->audio.dualmono )
        {
            NEXUS_Error rc;
            BDBG_WRN(("Setting dual mono %d",settings->audio.dualmono));
            NEXUS_AudioDecoder_GetSettings(decode->audio_decode->nAudioDecoder,&nDecoderSettings);
            switch (settings->audio.dualmono)
            {
            case baudio_dualmono_left:
                nDecoderSettings.dualMonoMode = NEXUS_AudioDecoderDualMonoMode_eLeft;
                break;
            case baudio_dualmono_right:
                nDecoderSettings.dualMonoMode = NEXUS_AudioDecoderDualMonoMode_eRight;
                break;
            case baudio_dualmono_stereo:
                nDecoderSettings.dualMonoMode = NEXUS_AudioDecoderDualMonoMode_eStereo;
                break;
            case baudio_dualmono_monomix:
                nDecoderSettings.dualMonoMode = NEXUS_AudioDecoderDualMonoMode_eMix;
                break;
            }
            rc = NEXUS_AudioDecoder_SetSettings(decode->audio_decode->nAudioDecoder,&nDecoderSettings);
            if (rc) {return BSETTOP_ERROR(rc);}
        }
    }

    /* set Dolby decoder configuration settings */
    if (decode->audio_decode && decode->audio_decode->nAudioDecoder)
    {
        NEXUS_Error rc;
        NEXUS_AudioDecoderCodecSettings nCodecSettings,newCodecSettings;
        NEXUS_AudioDecoderStatus nDecoderStatus;
        /* coverity[check_return] */ /* coverity[unchecked_value] */
        NEXUS_AudioDecoder_GetStatus(decode->audio_decode->nAudioDecoder,&nDecoderStatus);

        switch(nDecoderStatus.codec)
        {
        case NEXUS_AudioCodec_eAc3:
        NEXUS_AudioDecoder_GetCodecSettings(decode->audio_decode->nAudioDecoder, NEXUS_AudioCodec_eAc3, &nCodecSettings);
        newCodecSettings = nCodecSettings;
        newCodecSettings.codecSettings.ac3.drcMode = baudio_dolby_drc_mode_to_nexus_ac3plus_comp_mode(settings->audio.codec.dolby.drc_mode);
        newCodecSettings.codecSettings.ac3.stereoDownmixMode = baudio_dolby_stereo_downmix_mode_to_nexus_ac3plus_stereo_mode(settings->audio.codec.dolby.stereo_downmix_mode);
        newCodecSettings.codecSettings.ac3.cut = settings->audio.codec.dolby.cut;
        newCodecSettings.codecSettings.ac3.boost = settings->audio.codec.dolby.boost;
        newCodecSettings.codecSettings.ac3.dialogNormalization = settings->audio.codec.dolby.dialog_norm;
        BDBG_MSG(("ac3.drcMode = %d",newCodecSettings.codecSettings.ac3.drcMode));
        BDBG_MSG(("ac3.stereoDownmixMode = %d",newCodecSettings.codecSettings.ac3.stereoDownmixMode));
        BDBG_MSG(("ac3.cut = %d",newCodecSettings.codecSettings.ac3.cut));
        BDBG_MSG(("ac3.boost = %d",newCodecSettings.codecSettings.ac3.boost));
        BDBG_MSG(("ac3.dialogNormalization = %d",newCodecSettings.codecSettings.ac3.dialogNormalization));
        /* note that settings->audio.codec.dolby.lfe is not yet supported by Nexus */
        if ( (newCodecSettings.codecSettings.ac3.drcMode != nCodecSettings.codecSettings.ac3.drcMode) ||
             (newCodecSettings.codecSettings.ac3.stereoDownmixMode != nCodecSettings.codecSettings.ac3.stereoDownmixMode) ||
             (newCodecSettings.codecSettings.ac3.cut != nCodecSettings.codecSettings.ac3.cut) ||
             (newCodecSettings.codecSettings.ac3.boost != nCodecSettings.codecSettings.ac3.boost) ||
             (newCodecSettings.codecSettings.ac3.dialogNormalization != nCodecSettings.codecSettings.ac3.dialogNormalization) )
        {   /* Only set these if they've changed. */
            rc = NEXUS_AudioDecoder_SetCodecSettings(decode->audio_decode->nAudioDecoder, &newCodecSettings);
            if (rc) {return BSETTOP_ERROR(rc);}
        }
            break;
        case NEXUS_AudioCodec_eAc3Plus:
        NEXUS_AudioDecoder_GetCodecSettings(decode->audio_decode->nAudioDecoder, NEXUS_AudioCodec_eAc3Plus, &nCodecSettings);
        newCodecSettings = nCodecSettings;
        newCodecSettings.codecSettings.ac3Plus.drcMode = baudio_dolby_drc_mode_to_nexus_ac3plus_comp_mode(settings->audio.codec.dolby.drc_mode);
        newCodecSettings.codecSettings.ac3Plus.stereoDownmixMode = baudio_dolby_stereo_downmix_mode_to_nexus_ac3plus_stereo_mode(settings->audio.codec.dolby.stereo_downmix_mode);
        newCodecSettings.codecSettings.ac3Plus.cut = settings->audio.codec.dolby.cut;
        newCodecSettings.codecSettings.ac3Plus.boost = settings->audio.codec.dolby.boost;
        newCodecSettings.codecSettings.ac3Plus.dialogNormalization = settings->audio.codec.dolby.dialog_norm;
        BDBG_MSG(("ac3Plus.drcMode = %d",newCodecSettings.codecSettings.ac3Plus.drcMode));
        BDBG_MSG(("ac3Plus.stereoDownmixMode = %d",newCodecSettings.codecSettings.ac3Plus.stereoDownmixMode));
        BDBG_MSG(("ac3Plus.cut = %d",newCodecSettings.codecSettings.ac3Plus.cut));
        BDBG_MSG(("ac3Plus.boost = %d",newCodecSettings.codecSettings.ac3Plus.boost));
        BDBG_MSG(("ac3Plus.dialogNormalization = %d",newCodecSettings.codecSettings.ac3Plus.dialogNormalization));
        /* note that settings->audio.codec.dolby.lfe is not yet supported by Nexus */
        if ( (newCodecSettings.codecSettings.ac3Plus.drcMode != nCodecSettings.codecSettings.ac3Plus.drcMode) ||
             (newCodecSettings.codecSettings.ac3Plus.stereoDownmixMode != nCodecSettings.codecSettings.ac3Plus.stereoDownmixMode) ||
             (newCodecSettings.codecSettings.ac3Plus.cut != nCodecSettings.codecSettings.ac3Plus.cut) ||
             (newCodecSettings.codecSettings.ac3Plus.boost != nCodecSettings.codecSettings.ac3Plus.boost)  ||
             (newCodecSettings.codecSettings.ac3Plus.dialogNormalization != nCodecSettings.codecSettings.ac3Plus.dialogNormalization) )
        {   /* Only set these if they've changed. */
            rc = NEXUS_AudioDecoder_SetCodecSettings(decode->audio_decode->nAudioDecoder, &newCodecSettings);
            if (rc) {return BSETTOP_ERROR(rc);}
        }
            break;
        case NEXUS_AudioCodec_eAac:            /* Advanced audio coding. Part of MPEG-4 */
        case NEXUS_AudioCodec_eAacLoas:        /* Advanced audio coding. Part of MPEG-4 */
            NEXUS_AudioDecoder_GetCodecSettings(decode->audio_decode->nAudioDecoder, nDecoderStatus.codec, &nCodecSettings);
            newCodecSettings = nCodecSettings;
            newCodecSettings.codecSettings.aac.downmixMode = settings->audio.codec.aac.downmix;

            BDBG_MSG(("aac.downmix = %d",newCodecSettings.codecSettings.aac.downmixMode ));

            /* note that settings->audio.codec.dolby.lfe is not yet supported by Nexus */
            if (newCodecSettings.codecSettings.aac.downmixMode != nCodecSettings.codecSettings.aac.downmixMode )
            {   /* Only set these if they've changed. */
                rc = NEXUS_AudioDecoder_SetCodecSettings(decode->audio_decode->nAudioDecoder, &newCodecSettings);
                if (rc) {return BSETTOP_ERROR(rc);}
            }
            break;
        case NEXUS_AudioCodec_eAacPlus:        /* AAC plus SBR. aka MPEG-4 High Efficiency (AAC-HE) with ADTS (Audio Data Transport Format) */
        case NEXUS_AudioCodec_eAacPlusAdts:
            NEXUS_AudioDecoder_GetCodecSettings(decode->audio_decode->nAudioDecoder, nDecoderStatus.codec, &nCodecSettings);
            newCodecSettings = nCodecSettings;
            newCodecSettings.codecSettings.aacPlus.downmixMode = settings->audio.codec.aac.downmix;

            BDBG_MSG(("aacPlus.downmix = %d",newCodecSettings.codecSettings.aacPlus.downmixMode ));

            /* note that settings->audio.codec.dolby.lfe is not yet supported by Nexus */
            if (newCodecSettings.codecSettings.aacPlus.downmixMode != nCodecSettings.codecSettings.aacPlus.downmixMode )
            {   /* Only set these if they've changed. */
                rc = NEXUS_AudioDecoder_SetCodecSettings(decode->audio_decode->nAudioDecoder, &newCodecSettings);
                if (rc) {return BSETTOP_ERROR(rc);}
            }
            break;
            break;
        default:
            break;
        }
    }

    if(decode->audio_decode && decode->audio_decode->nTrueVolume)
    {
        NEXUS_TruVolumeSettings nTrueVolumeSettings;
        NEXUS_TruVolume_GetSettings(decode->audio_decode->nTrueVolume,&nTrueVolumeSettings);
        nTrueVolumeSettings.enabled = settings->audio.postproc.srs.tru_volume_enable;
        BDBG_WRN(("true volume %s",nTrueVolumeSettings.enabled?"on":"off"));
        NEXUS_TruVolume_SetSettings(decode->audio_decode->nTrueVolume,&nTrueVolumeSettings);
    }

    if(decode->audio_decode && decode->audio_decode->nDolbyVolume)
    {
        NEXUS_DolbyVolumeSettings nDolbyVolumeSettings;
        NEXUS_DolbyVolume_GetSettings(decode->audio_decode->nDolbyVolume,&nDolbyVolumeSettings);
        nDolbyVolumeSettings.enabled = settings->audio.postproc.dolby.volume_proc_enable;
        BDBG_WRN(("dolby volume %s",nDolbyVolumeSettings.enabled?"on":"off"));
        NEXUS_DolbyVolume_SetSettings(decode->audio_decode->nDolbyVolume,&nDolbyVolumeSettings);
    }

    if(decode->audio_decode && decode->audio_decode->nAutoVolumeLevel)
    {
        NEXUS_AutoVolumeLevelSettings nAutoVolumeLevelSettings;
        NEXUS_AutoVolumeLevel_GetSettings(decode->audio_decode->nAutoVolumeLevel,&nAutoVolumeLevelSettings);
        nAutoVolumeLevelSettings.enabled = settings->audio.postproc.avl.proc_enable;
        BDBG_WRN(("auto volume level %s",nAutoVolumeLevelSettings.enabled?"on":"off"));
        NEXUS_AutoVolumeLevel_SetSettings(decode->audio_decode->nAutoVolumeLevel,&nAutoVolumeLevelSettings);
    }

    /* Settings are applied later on next bdecode_start except for freeze and stop_mode */
    decode->settings = *settings;

    if (decode->video_decode)
    {
        NEXUS_Error rc;
        NEXUS_VideoDecoderSettings nVideoDecodeSettings;

        NEXUS_VideoDecoder_GetSettings(decode->video_decode->nVideoDecoder, &nVideoDecodeSettings);

        switch (decode->settings.stop_mode)
        {
        case bdecode_stop_mode_last_picture:
            nVideoDecodeSettings.channelChangeMode = NEXUS_VideoDecoder_ChannelChangeMode_eHoldUntilFirstPicture;
            break;
        default:
            nVideoDecodeSettings.channelChangeMode = NEXUS_VideoDecoder_ChannelChangeMode_eMute;
            break;
        }

        nVideoDecodeSettings.freeze = settings->video.freeze;

        rc = NEXUS_VideoDecoder_SetSettings(decode->video_decode->nVideoDecoder, &nVideoDecodeSettings);
        if (rc) {return BSETTOP_ERROR(rc);}
    }

    return 0;
}

bresult bdecode_display_surface(bdecode_t decode, bsurface_t surface, bdecode_window_t window)
{
    NEXUS_Error rc;

    if (!decode->video_decode) {
        return BSETTOP_ERROR(berr_not_available);
    }
    if (decode->started) {
        bdecode_stop(decode);
    }

    rc = NEXUS_VideoImageInput_SetSurface(decode->video_decode->nVideoImageInput, surface->nSurface);
    if (rc) return BSETTOP_ERROR(rc);

    if (!decode->image_input_started) {
        rc = NEXUS_VideoWindow_AddInput(window->nWindow, NEXUS_VideoImageInput_GetConnector(decode->video_decode->nVideoImageInput));
        if (rc) return BSETTOP_ERROR(rc);

        decode->image_input_started = true;
    }

    return 0;
}

/* bdecode_p_set_video_input allows the VDC window to be torn down only if there's an input change
This is needed for hold_last_picture mode. */
static void bdecode_p_set_video_input(bdecode_window_t window, NEXUS_VideoInput videoInput)
{
    if (videoInput != window->currentVideoInput) {
        if (window->currentVideoInput) {
            if (window->clone) {
                /* VDC requires that clone be destroyed first */
                NEXUS_VideoWindow_RemoveInput(window->clone->nWindow, window->currentVideoInput);
            }
            NEXUS_VideoWindow_RemoveInput(window->nWindow, window->currentVideoInput);
        }
        window->currentVideoInput = videoInput;
        if (videoInput) {
            bsettop_init_settings initSettings;
            bsettop_get_init_settings(&initSettings);
            if ( !initSettings.sync_path_swap )
            {
                /* Add primary window before clone normally */
                NEXUS_VideoWindow_AddInput(window->nWindow, videoInput);
            }
            if (window->clone)
            {
                NEXUS_VideoWindow_AddInput(window->clone->nWindow, videoInput);
            }
            if ( initSettings.sync_path_swap )
            {
                /* Add primary window after clone if sync paths are swapped */
                NEXUS_VideoWindow_AddInput(window->nWindow, videoInput);
            }
        }
    }
}

#if B_HAS_ANALOG
static bresult bdecode_p_start_analog_video(bvideo_decode_t video_decode, bstream_t source, bdecode_window_t window)
{
    if (!video_decode->nAnalogVideoDecoder) {
        return BSETTOP_ERROR(berr_not_available);
    }
    btuner_p_set_analog_input(video_decode->nAnalogVideoDecoder, source->producer.tuner, source->producer.linein);

    bdecode_p_set_video_input(window, NEXUS_AnalogVideoDecoder_GetConnector(video_decode->nAnalogVideoDecoder));

    video_decode->started = true;

    return 0;
}
#endif

#if B_HAS_EXTERNAL_ANALOG
static bresult bdecode_p_start_656_video(bvideo_decode_t video_decode, bstream_t source, bdecode_window_t window)
{
    NEXUS_Ccir656InputHandle ccir656Input = NEXUS_Ccir656Input_Open(0, NULL);
    BSTD_UNUSED(source);
    BDBG_MSG(("Start 656 input"));
    video_decode->ccir656Input = ccir656Input;
    bdecode_p_set_video_input(window, NEXUS_Ccir656Input_GetConnector(ccir656Input));
    video_decode->started = true;
    return 0;
}
#endif

static bresult bdecode_set_video_ip_tsm_settings(bvideo_decode_t video_decode)
{
#ifdef B_HAS_IP
    if (video_decode && video_decode->nVideoDecoder)
    {
        NEXUS_VideoDecoderSettings  nVideoDecodeSettings;

        NEXUS_VideoDecoder_GetSettings(video_decode->nVideoDecoder, &nVideoDecodeSettings);
        switch (video_decode->decode->cfg_type) {
        case BDECODE_STC_CHANNEL_CFG_TYPE_PARSER_BAND:
            nVideoDecodeSettings.ptsOffset = 0;
            break;
        case BDECODE_STC_CHANNEL_CFG_TYPE_TTS_PACING:
        case BDECODE_STC_CHANNEL_CFG_TYPE_PCR_PACING:
            nVideoDecodeSettings.ptsOffset = 0;
            break;
        case BDECODE_STC_CHANNEL_CFG_TYPE_IP_LIVE_PB:
            nVideoDecodeSettings.ptsOffset = 45 * IP_NETWORK_JITTER;
            break;
        case BDECODE_STC_CHANNEL_CFG_TYPE_PVR_PB:
            nVideoDecodeSettings.ptsOffset = 0;
            break;
        default:
            return BSETTOP_ERROR(berr_invalid_parameter);
            break;
        }
        if (NEXUS_VideoDecoder_SetSettings(video_decode->nVideoDecoder, &nVideoDecodeSettings))
        {
            return BSETTOP_ERROR(berr_external_error);
        }
    }
    return b_ok;
#else
    BSTD_UNUSED(video_decode);
    return (berr_not_supported);
#endif
}

bresult bdecode_start_video(bvideo_decode_t video_decode, bstream_t source, bdecode_window_t window)
{
    uint16_t pid;
#if B_HAS_SVC_MVC
    uint16_t enhancementPid = 0;
#endif
    NEXUS_Error rc;
    NEXUS_VideoDecoderStartSettings startSettings;
    bsettop_init_settings initSettings;
    bsettop_get_init_settings(&initSettings);

    if (initSettings.lipsync_mode == bsettop_lipsync_mode_disabled) {
        video_decode->decode->tsm = false;
    }

#if B_HAS_ANALOG
    if (source->producer.tuner) {
        return bdecode_p_start_analog_video(video_decode, source, window);
    }
#endif
#if B_HAS_EXTERNAL_ANALOG
    if (source->producer.tuner) {
        return bdecode_p_start_656_video(video_decode, source, window);
    }
#endif
    if (!video_decode->nVideoDecoder) {
        return BSETTOP_ERROR(berr_not_available);
    }
    if (window->parent) {
        BDBG_ERR(("can't start decode on cloned window"));
        return BSETTOP_ERROR(berr_not_available);
    }

    BDBG_MSG(("start_video decode %p,src stream %p,window %p,zorder %d,%s",
              video_decode,source,window,window->settings.zorder,video_decode->decode->tsm?"TSM":"VSYNC"));

    BDBG_MSG(("video stc channel %p",video_decode->decode->stcChannel));

    pid = source->mpeg.video[video_decode->decode->video_program].pid;
    if ( pid == 0 || pid >= 0x1fff )
    {
        /* No video */
        bdecode_p_unset_sync_channel(video_decode->decode,true,true,true);
        return b_ok;
    }
    
#if B_HAS_SVC_MVC
    if (source->mpeg.video[video_decode->decode->video_program+1].format == bvideo_codec_h264_svc)
        enhancementPid = source->mpeg.video[video_decode->decode->video_program+1].pid;
#endif

    video_decode->nPidChannel = bstream_p_open_pid(source, pid, bstream_pid_type_video);
    if ( NULL == video_decode->nPidChannel )
    {
        BDBG_ERR(("Unable to allocate PID channel for video program"));
        return BSETTOP_ERROR(berr_invalid_parameter);
    }

#if B_HAS_SVC_MVC
    if (enhancementPid) {
        video_decode->enhancementPidChannel = bstream_p_open_pid(source, enhancementPid, bstream_pid_type_video);
        if ( NULL == video_decode->enhancementPidChannel)
        {
            BDBG_ERR(("Unable to allocate PID channel for enhancement video program"));
            return BSETTOP_ERROR(berr_invalid_parameter);
        }
    }
#endif

    bdecode_p_set_video_input(window, NEXUS_VideoDecoder_GetConnector(video_decode->nVideoDecoder));

    /* ip TSM related settings: specify additional pts offset & override decoder playback mode */
    bdecode_set_video_ip_tsm_settings(video_decode);

    NEXUS_VideoDecoder_GetDefaultStartSettings(&startSettings);
#if 0 /* TODO: decoder feature is being reworked. use host-reordering in the meantime */
    startSettings.timestampMode = (NEXUS_VideoDecoderTimestampMode)source->mpeg.timestamp_order;
#endif
    startSettings.pidChannel = video_decode->nPidChannel;

#if B_HAS_SVC_MVC
    if (enhancementPid)
    {
        startSettings.enhancementPidChannel = video_decode->enhancementPidChannel;
        startSettings.codec = b_videocodec2nexus(source->mpeg.video[video_decode->decode->video_program+1].format);
    }
    else
#endif
    startSettings.codec = b_videocodec2nexus(source->mpeg.video[video_decode->decode->video_program].format);
    startSettings.stcChannel = video_decode->decode->tsm? video_decode->decode->stcChannel : NULL;

    if (bsettop_get_config("preroll_rate")) {
        startSettings.prerollRate = atoi(bsettop_get_config("preroll_rate"));
    }

    if (video_decode->decode->primerStart.stcChannel && !source->producer.playback) {
        startSettings.pidChannel = video_decode->decode->primerStart.pidChannel;
        startSettings.stcChannel = video_decode->decode->tsm? video_decode->decode->primerStart.stcChannel : NULL;
    }

    /* set the sync channel for video decoder */
    bdecode_p_set_sync_channel(video_decode->decode, true,false,false);
    bdecode_p_set_astm(video_decode->decode, true,false,false);

    rc = NEXUS_VideoDecoder_Start(video_decode->nVideoDecoder, &startSettings);
    if (rc)
    {
        BSETTOP_ERROR(berr_external_error);
    }

#if NEXUS_HAS_PLAYBACK
    if (source->producer.playback) {
        /* update playback pidchannel */
        NEXUS_PlaybackPidChannelSettings settings;
        NEXUS_Playback_GetPidChannelSettings(source->producer.playback->nPlayback, video_decode->nPidChannel, &settings);
        BDBG_ASSERT(settings.pidSettings.pidType == NEXUS_PidType_eVideo);
        settings.pidTypeSettings.video.decoder = video_decode->nVideoDecoder;
        NEXUS_Playback_SetPidChannelSettings(source->producer.playback->nPlayback, video_decode->nPidChannel, &settings);
#if B_HAS_SVC_MVC
        if (enhancementPid){
            NEXUS_Playback_GetPidChannelSettings(source->producer.playback->nPlayback, video_decode->enhancementPidChannel, &settings);
            BDBG_ASSERT(settings.pidSettings.pidType == NEXUS_PidType_eVideo);
            settings.pidTypeSettings.video.decoder = video_decode->nVideoDecoder;
            NEXUS_Playback_SetPidChannelSettings(source->producer.playback->nPlayback, video_decode->enhancementPidChannel, &settings);
        }
#endif
    }
#endif

    video_decode->started = true;

    return 0;
}

#if B_HAS_ANALOG
static void bdecode_p_stop_analog_video(bvideo_decode_t video_decode)
{
    bdecode_window_t window = video_decode->decode->window;

    if (video_decode->decode->settings.stop_mode == bdecode_stop_mode_mute) {
        bdecode_p_set_video_input(window, NULL);
    }
    video_decode->started = false;
}
#endif

#if B_HAS_EXTERNAL_ANALOG
static void bdecode_p_stop_656_video(bvideo_decode_t video_decode)
{
    bdecode_window_t window = video_decode->decode->window;
    BDBG_MSG(("Stop 656 input"));
    if (video_decode->decode->settings.stop_mode == bdecode_stop_mode_mute) {
        bdecode_p_set_video_input(window, NULL);
    }
    NEXUS_VideoInput_Shutdown(NEXUS_Ccir656Input_GetConnector(video_decode->ccir656Input));
    NEXUS_Ccir656Input_Close(video_decode->ccir656Input);
    video_decode->ccir656Input=NULL;
    video_decode->started = false;
}
#endif

void bdecode_stop_video(bvideo_decode_t video_decode)
{
    bstream_t source = video_decode->decode->source;
    if (!video_decode->started) {
        return;
    }
    BDBG_MSG(("stop_video %p",video_decode));

#if B_HAS_ANALOG
    if (source->producer.tuner) {
        bdecode_p_stop_analog_video(video_decode);
        return;
    }
#endif

#if B_HAS_EXTERNAL_ANALOG
    if (source->producer.tuner) {
        bdecode_p_stop_656_video(video_decode);
        return;
    }
#endif

    if (video_decode->started) {
        NEXUS_VideoDecoder_Stop(video_decode->nVideoDecoder);
        bdecode_p_unset_sync_channel(video_decode->decode,true,false,false);
        bdecode_p_unset_astm(video_decode->decode,true,false,false);
        bstream_p_close_pid(source, video_decode->nPidChannel);
#if B_HAS_SVC_MVC
        if (video_decode->enhancementPidChannel) {
            bstream_p_close_pid(source, video_decode->enhancementPidChannel);
            video_decode->enhancementPidChannel = NULL;
        }
#endif

        if (video_decode->decode->settings.stop_mode == bdecode_stop_mode_mute) {
            bdecode_p_set_video_input(video_decode->decode->window, NULL);
        }
        video_decode->started = false;
    }

}

#if B_HAS_EXTERNAL_ANALOG
static bresult bdecode_p_start_i2s_audio(baudio_decode_t audio_decode,bdecode_window_t window)
{
    BDBG_MSG(("Start i2s audio"));
    if ( NULL == audio_decode->display )
    {
        /* TODO: Handle multiple decodes/displays with analog more gracefully... */
    audio_decode->busy = true;
    bdisplay_p_enable_audio(window->display, false);
    NEXUS_AudioMixer_AddInput(window->display->nAudioMixer,NEXUS_I2sInput_GetConnector(audio_decode->i2sInput));
    bdisplay_p_enable_audio(window->display, true);
    audio_decode->busy = false;
    }
    NEXUS_I2sInput_Start(audio_decode->i2sInput);
    window->display->mixerInputsStarted++;
    audio_decode->started = true;
    return 0;
}
static void bdecode_p_stop_i2s_audio(baudio_decode_t audio_decode,bdecode_window_t window)
{
    BDBG_MSG(("Stop i2s audio"));
    if (!audio_decode->i2sInput)
    {
        BDBG_ERR(("I2S input not open"));
        BSETTOP_ERROR(berr_invalid_parameter);
        return;
    }
    audio_decode->started = false;
    NEXUS_I2sInput_Stop(audio_decode->i2sInput);
    window->display->mixerInputsStarted--;
    if ( NULL == audio_decode->display )
    {
    audio_decode->busy = true;
    bdisplay_p_enable_audio(window->display, false);
    NEXUS_AudioMixer_RemoveInput(audio_decode->decode->window->display->nAudioMixer,NEXUS_I2sInput_GetConnector(audio_decode->i2sInput));
    bdisplay_p_enable_audio(window->display, true);
    audio_decode->busy = false;
    }
    audio_decode->i2sInput=NULL;
    return ;
}
#endif


const char* g_audioCodecName[]=
{
    "unknown",
    "mpeg",
    "mp3",
    "aac",
    "aac_loas",
    "aac_plus",
    "aac_plus_adts",
    "ac3",
    "ac3_plus",
    "dts",
    "lpcm_dvd",
    "lpcm_hddvd",
    "lpcm_bluray",
    "dts_hd",
    "wma",
    "wma_ts",
    "wma_pro",
    "avs",
    "pcm",
    "pcm_wave",
    "amr",
    "dra",
    "cook",
    "adpcm",
    "sbc",
    "dts_legacy",
    "vorbis",
    ""
 };


#if B_N_DVI_OUTPUTS
static bool bdecode_p_hdmi_audio_format_supported(NEXUS_HdmiOutputHandle hdmi,NEXUS_AudioCodec codec)
{
    NEXUS_HdmiOutputStatus nHdmiStatus;
    BSTD_UNUSED(hdmi);
    BSTD_UNUSED(codec);

#if (BCHP_CHIP==7400)
    if (NEXUS_AudioCodec_eAc3Plus == codec)
    {
        if (g_decode[1].audio_decode)
        {
            return false;
        }
        else if (g_audio_passthrough == g_n_audio_decoder[2])
        {
            /* DDP passthru on 7400 must use full-featured DSP context 1. Take it from g_decode[1] */
            if (NULL != g_audio_passthrough)
            {
                NEXUS_AudioInput_Shutdown(NEXUS_AudioDecoder_GetConnector(g_audio_passthrough, NEXUS_AudioDecoderConnectorType_eCompressed));
                NEXUS_AudioDecoder_Close(g_audio_passthrough);
                g_n_audio_decoder[2] = NULL;
            }
            g_audio_passthrough = g_n_audio_decoder[1];
            BDBG_WRN(("Swapped audio decoder 1 for DDP passthru.  Dual audio decode disabled"));
        }
    }
#endif

    if (bsettop_get_config("hdmi_bypass_audio_edid"))
    {
        BDBG_WRN(("bypass edid for %s audio",g_audioCodecName[codec]));
        return true;
    }
    #if B_N_DVI_OUTPUTS
    NEXUS_HdmiOutput_GetStatus(hdmi, &nHdmiStatus);
    if(nHdmiStatus.connected && nHdmiStatus.audioCodecSupported[codec])
        return true;
    else
        return false;
    #endif
}
#endif

static bresult bdecode_p_set_audio_display(baudio_decode_t audio_decode, bdisplay_t display)
{
    NEXUS_Error rc =0;
    NEXUS_AudioInput mixerSource;
    if ( NULL == display )
    {
        display = audio_decode->display;
        if ( display == NULL )
        {
            return b_ok;
        }
        audio_decode->display = NULL;
        mixerSource = NEXUS_AudioDecoder_GetConnector(audio_decode->nAudioDecoder, NEXUS_AudioDecoderConnectorType_eStereo);
        if(audio_decode->nTrueVolume)
        {
            mixerSource = NEXUS_TruVolume_GetConnector(audio_decode->nTrueVolume);
            NEXUS_TruVolume_RemoveAllInputs(audio_decode->nTrueVolume);
        }
        if(audio_decode->nDolbyVolume)
        {
            mixerSource = NEXUS_DolbyVolume_GetConnector(audio_decode->nDolbyVolume);
            NEXUS_DolbyVolume_RemoveAllInputs(audio_decode->nDolbyVolume);
        }
        if(audio_decode->nAutoVolumeLevel)
        {
            mixerSource = NEXUS_AutoVolumeLevel_GetConnector(audio_decode->nAutoVolumeLevel);
            NEXUS_AutoVolumeLevel_RemoveAllInputs(audio_decode->nAutoVolumeLevel);
        }
        NEXUS_AudioMixer_RemoveInput(display->nAudioMixer, mixerSource);
    }
    else
    {
        mixerSource = NEXUS_AudioDecoder_GetConnector(audio_decode->nAudioDecoder, NEXUS_AudioDecoderConnectorType_eStereo);
        if(audio_decode->nTrueVolume)
        {
            BDBG_MSG(("Setting up SRS True Vol post processing stage"));
            NEXUS_TruVolume_RemoveAllInputs(audio_decode->nTrueVolume);
            rc = NEXUS_TruVolume_AddInput(audio_decode->nTrueVolume, mixerSource);
            if(rc)
            {
                BSETTOP_ERROR(berr_external_error); return rc;
            }
            mixerSource = NEXUS_TruVolume_GetConnector(audio_decode->nTrueVolume);
        }
        if(audio_decode->nDolbyVolume)
        {
            BDBG_MSG(("Setting up Dolby volume post processing stage"));
            NEXUS_DolbyVolume_RemoveAllInputs(audio_decode->nDolbyVolume);
            rc = NEXUS_DolbyVolume_AddInput(audio_decode->nDolbyVolume, mixerSource);
            if(rc)
            {
                BSETTOP_ERROR(berr_external_error); return rc;
            }
            mixerSource = NEXUS_DolbyVolume_GetConnector(audio_decode->nDolbyVolume);
        }
        if(audio_decode->nAutoVolumeLevel)
        {
            BDBG_MSG(("Setting up Auto Volume Level post processing stage "));
            NEXUS_AutoVolumeLevel_RemoveAllInputs(audio_decode->nAutoVolumeLevel);
            rc = NEXUS_AutoVolumeLevel_AddInput(audio_decode->nAutoVolumeLevel, mixerSource);
            if(rc)
            {
                BSETTOP_ERROR(berr_external_error); return rc;
            }
            mixerSource = NEXUS_AutoVolumeLevel_GetConnector(audio_decode->nAutoVolumeLevel);
        }

        rc = NEXUS_AudioMixer_AddInput(display->nAudioMixer, mixerSource);
        if(rc)
        {
            BSETTOP_ERROR(berr_external_error); return rc;
        }

        audio_decode->display = display;
    }
    return rc;
}

static bool bdecode_p_dts_encode_supported(NEXUS_AudioCodec codec)
{
    bool codecSupported;
    switch(codec)
    {
    default:
        codecSupported=false;
        break;
    case NEXUS_AudioCodec_eAac:
    case NEXUS_AudioCodec_eAacLoas:
    case NEXUS_AudioCodec_eAacPlus:
    case NEXUS_AudioCodec_eAacPlusAdts:
    case NEXUS_AudioCodec_eDts:
        codecSupported=true;
        break;

    }
    return codecSupported;
}

static bool bdecode_p_ac3_encode_supported(NEXUS_AudioCodec codec)
{
    bool codecSupported;
    switch(codec)
    {
    default:
        codecSupported=false;
        break;
    case NEXUS_AudioCodec_eAac:
    case NEXUS_AudioCodec_eAacLoas:
    case NEXUS_AudioCodec_eAacPlus:
    case NEXUS_AudioCodec_eAacPlusAdts:
        codecSupported=true;
        break;
    }
    return codecSupported;
}


bresult bdecode_start_audio(baudio_decode_t audio_decode, bstream_t source, bdecode_window_t window)
{
    uint16_t pid;
    NEXUS_Error rc;
    NEXUS_AudioDecoderStartSettings startSettings;
    bool spdifDts=false;
    bool spdifAc3=false;
#if B_N_DVI_OUTPUTS
    bool hdmiDts=false;
    bool hdmiAc3=false;
    bool video=true;
    bdecode_t decode=audio_decode->decode;
    NEXUS_HdmiOutputStatus nHdmiStatus;
#endif
    bsettop_init_settings initSettings;
    bsettop_get_init_settings(&initSettings);

    if (initSettings.lipsync_mode == bsettop_lipsync_mode_disabled)
        audio_decode->decode->tsm = false;

    if (source->producer.tuner) {
#if B_HAS_EXTERNAL_ANALOG
        return bdecode_p_start_i2s_audio(audio_decode,window);
#endif
        return 0;
    }

    if (!audio_decode->nAudioDecoder) {
        return BSETTOP_ERROR(berr_not_available);
    }
    BDBG_MSG(("start_audio decode %p,src stream %p,window %p,%s",
              audio_decode,source,window,audio_decode->decode->tsm?"TSM":"VSYNC"));

    BDBG_ASSERT(false == audio_decode->decode->audio_disabled);

    NEXUS_AudioDecoder_GetDefaultStartSettings(&startSettings);

    BDBG_MSG(("audio stc channel %p",audio_decode->decode->stcChannel));

    pid = source->mpeg.video[decode->video_program].pid;
    if (pid == 0 || pid >= 0x1fff)
    {
        video=false;
    }
    pid = source->mpeg.audio[audio_decode->decode->audio_program].pid;
    if ( pid == 0 || (source->mpeg.mpeg_type != bstream_mpeg_type_vob && pid >= 0x1fff) )
    {
        BDBG_MSG(("No audio PID"));
        bdecode_p_unset_sync_channel(audio_decode->decode,true,true,true);
        /* No audio */
        return b_ok;
    }
    audio_decode->nPidChannel = bstream_p_open_pid(source, pid, bstream_pid_type_audio);
    if ( NULL == audio_decode->nPidChannel )
    {
        BDBG_ERR(("Unable to allocate PID channel for audio program"));
        return BSETTOP_ERROR(berr_invalid_parameter);
    }

    if ( audio_decode->decode->settings.audio.secondary_audio_program != -1 )
    {
        pid = source->mpeg.audio[audio_decode->decode->settings.audio.secondary_audio_program].pid;
        if ( pid == 0 || pid >= 0x1fff )
        {
            BDBG_WRN(("Invalid secondary audio program.  Using Primary."));
        }
        else
        {
            audio_decode->nSecondaryPidChannel = bstream_p_open_pid(source, pid, bstream_pid_type_audio);
            if ( NULL == audio_decode->nSecondaryPidChannel )
            {
                BDBG_WRN(("Unable to open pid channel for secondary audio progam.  Using Primary."));
            }
        }
    }
#if 0
    audio_decode->sourceChangedEvent = B_Event_Create(NULL);
    BDBG_MSG(("audio_decode->sourceChangedEvent = %p, audio_decode->decode = %p",
              audio_decode->sourceChangedEvent,audio_decode->decode));
    if ( NULL == audio_decode->sourceChangedEvent )
    {
        return BSETTOP_ERROR(berr_out_of_memory);
    }
    audio_decode->sourceChangedEventId = b_event_register(audio_decode->sourceChangedEvent,
                                                                  bdecode_p_audio_source_changed,
                                                                  audio_decode->decode);
    if ( NULL == audio_decode->sourceChangedEventId )
    {
        B_Event_Destroy(audio_decode->sourceChangedEvent);
        return BSETTOP_ERROR(berr_out_of_memory);
    }
#endif
    audio_decode->sourceChangedCallback = b_callback_create(audio_decode,bdecode_p_audio_source_changed,audio_decode->decode,0);
    if(NULL == audio_decode->sourceChangedCallback)
        return BSETTOP_ERROR(berr_out_of_memory);
    BDBG_MSG(("audio_decode->sourceChangedCallback = %p, audio_decode->decode = %p",audio_decode->sourceChangedCallback,audio_decode->decode));

    audio_decode->restartSettings.pidChannel = startSettings.pidChannel = audio_decode->nPidChannel;
    audio_decode->restartSettings.codec = startSettings.codec = b_audiocodec2nexus(source->mpeg.audio[audio_decode->decode->audio_program].format);
    /* BDBG_MSG(("nexus %d, settop %d",startSettings.codec,source->mpeg.audio[audio_decode->decode->audio_program].format)); */
    audio_decode->restartSettings.stcChannel = startSettings.stcChannel = audio_decode->decode->tsm? audio_decode->decode->stcChannel : NULL;

    if ( NULL == audio_decode->decode->settings.audio.display )
    {
        audio_decode->busy = true;
        bdisplay_p_enable_audio(window->display, false);
        bdecode_p_set_audio_display(audio_decode, window->display);
        bdisplay_p_enable_audio(window->display, true);
        audio_decode->busy = false;
    }

    if (audio_decode->decode->primerStart.stcChannel  && !source->producer.playback) {
        startSettings.stcChannel = audio_decode->decode->tsm? audio_decode->decode->primerStart.stcChannel : NULL;
    }

#if B_N_DVI_OUTPUTS
    if (window->display->settings.dvi) {
        NEXUS_HdmiOutput_GetStatus(window->display->settings.dvi->handle, &nHdmiStatus);
    }

    if( window->display->settings.dvi && (window->display->settings.dvi->desired.hdmi_audio_mode == boutput_hdmi_audio_mode_pcm_6ch && nHdmiStatus.connected) )
    {
        bool isMultiChannelSupported=true;
        switch ( startSettings.codec )
        {
        case NEXUS_AudioCodec_eMpeg:
        case NEXUS_AudioCodec_eMp3:
        case NEXUS_AudioCodec_eWmaStd:
        case NEXUS_AudioCodec_eLpcmDvd:
        case NEXUS_AudioCodec_eLpcmHdDvd:
        case NEXUS_AudioCodec_eLpcmBluRay:
        case NEXUS_AudioCodec_ePcmWav:
        case NEXUS_AudioCodec_ePcm:
        case NEXUS_AudioCodec_eAmr:
        case NEXUS_AudioCodec_eCook:
        case NEXUS_AudioCodec_eAdpcm:
        #if (BCHP_CHIP == 7400)
        case NEXUS_AudioCodec_eDts:
        case NEXUS_AudioCodec_eDtsHd:
        #endif
            isMultiChannelSupported=0;
            break;
        default:
            isMultiChannelSupported=1;
        }
        if (isMultiChannelSupported)
        {
            BDBG_MSG(("Attaching HDMI audio to decoder %p",audio_decode));
            /* remove any inputs*/
            NEXUS_AudioOutput_RemoveAllInputs(NEXUS_HdmiOutput_GetAudioConnector(window->display->settings.dvi->handle));
            NEXUS_AudioOutput_AddInput(NEXUS_HdmiOutput_GetAudioConnector(window->display->settings.dvi->handle),
                                       NEXUS_AudioDecoder_GetConnector(audio_decode->nAudioDecoder,
                                                                       NEXUS_AudioDecoderConnectorType_eMultichannel));
        }
        else
        {
            BDBG_MSG(("Multichannel PCM audio not supported for codec %s",g_audioCodecName[startSettings.codec]));
        }

    }
#endif

    if(audio_decode->nDtsEncoder)
        NEXUS_DtsEncode_RemoveAllInputs(audio_decode->nDtsEncoder);
    if(audio_decode->nAc3Encoder)
        NEXUS_Ac3Encode_RemoveAllInputs(audio_decode->nAc3Encoder);

#if B_N_SPDIF_OUTPUTS
    if (window->display->settings.spdif && !window->display->settings.spdif->desired.pcm)
    {

        if (window->display->settings.spdif->desired.compressed_audio_format == baudio_format_dts)
        {
            if ( bdecode_p_dts_encode_supported(startSettings.codec) )
            {
                if ( bconfig->features.dts_transcode )
                {
                    BDBG_WRN(("DTS encoded output on spdif"));
                    /* Send multichannel output to DTS encoder */
                    NEXUS_DtsEncode_AddInput(audio_decode->nDtsEncoder,
                                             NEXUS_AudioDecoder_GetConnector(audio_decode->nAudioDecoder, NEXUS_AudioDecoderConnectorType_eMultichannel));
                    /* Send compressed DTS data to SPDIF */
                    NEXUS_AudioOutput_RemoveAllInputs(NEXUS_SpdifOutput_GetConnector(window->display->settings.spdif->handle));
                    NEXUS_AudioOutput_AddInput(NEXUS_SpdifOutput_GetConnector(window->display->settings.spdif->handle),
                                               NEXUS_DtsEncode_GetConnector(audio_decode->nDtsEncoder));
                    spdifDts=true;
                }
                else
                {
                    BDBG_WRN(("DTS encode requested on SPDIF but not available in this build."));
                }
            }
            else
            {
                BDBG_WRN(("DTS encode requested on SPDIF but can not be performed for this source codec."));
            }
        }
        else if (window->display->settings.spdif->desired.compressed_audio_format == baudio_format_ac3)
        {
            if ( bdecode_p_ac3_encode_supported(startSettings.codec) )
            {
                if ( bconfig->features.ac3_transcode )
                {
                    BDBG_WRN(("Ac3 encoded output on spdif"));
                    /* Send multichannel output to DTS encoder */
                    NEXUS_Ac3Encode_AddInput(audio_decode->nAc3Encoder,
                                             NEXUS_AudioDecoder_GetConnector(audio_decode->nAudioDecoder, NEXUS_AudioDecoderConnectorType_eMultichannel));
                    /* Send compressed Ac3 data to SPDIF */
                    NEXUS_AudioOutput_RemoveAllInputs(NEXUS_SpdifOutput_GetConnector(window->display->settings.spdif->handle));
                    NEXUS_AudioOutput_AddInput(NEXUS_SpdifOutput_GetConnector(window->display->settings.spdif->handle),
                                               NEXUS_Ac3Encode_GetConnector(audio_decode->nAc3Encoder));
                    spdifAc3=true;
                }
                else
                {
                    BDBG_WRN(("AC3 encode requested on SPDIF but not available in this build."));
                }
            }
            else
            {
                BDBG_WRN(("AC3 encode requested on SPDIF but can not be performed for this source codec."));
            }
        }
    }
#endif /* B_N_SPDIF_OUTPUTS */

#if B_N_DVI_OUTPUTS
    if(window->display->settings.dvi && nHdmiStatus.connected)
    {
        switch(window->display->settings.dvi->desired.hdmi_audio_mode)
        {
        case boutput_hdmi_audio_mode_pcm:
        case boutput_hdmi_audio_mode_pcm_6ch:
            break;
        default:
            if(window->display->settings.dvi->desired.compressed_audio_format == baudio_format_dts )
            {
                if ( bdecode_p_dts_encode_supported(startSettings.codec) )
                {
                    if ( bconfig->features.dts_transcode )
                    {
                        if(bdecode_p_hdmi_audio_format_supported(window->display->settings.dvi->handle,NEXUS_AudioCodec_eDts))
                        {
                            BDBG_WRN(("DTS encoded output on hdmi"));
                            hdmiDts=true;
                            if(!spdifDts)
                            {
                                /* Send multichannel output to DTS encoder */
                                NEXUS_DtsEncode_AddInput(audio_decode->nDtsEncoder,
                                   NEXUS_AudioDecoder_GetConnector(audio_decode->nAudioDecoder, NEXUS_AudioDecoderConnectorType_eMultichannel));
                            }
                            /* Send compressed DTS data to HDMI */
                            NEXUS_AudioOutput_RemoveAllInputs(NEXUS_HdmiOutput_GetAudioConnector(window->display->settings.dvi->handle));
                            NEXUS_AudioOutput_AddInput(NEXUS_HdmiOutput_GetAudioConnector(window->display->settings.dvi->handle),
                                       NEXUS_DtsEncode_GetConnector(audio_decode->nDtsEncoder));
    
                        }
                        else
                        {
                            BDBG_WRN(("DTS Encode requested on HDMI but receiver does not support DTS audio."));
                        }
                    }
                    else
                    {
                        BDBG_WRN(("DTS encode requested on HDMI but not available in this build."));
                    }
                }
                else
                {
                    BDBG_WRN(("DTS encode requested on HDMI but can not be performed for this source codec."));
                }
            }
            else if(window->display->settings.dvi->desired.compressed_audio_format == baudio_format_ac3)
            {
                if ( bdecode_p_ac3_encode_supported(startSettings.codec) )
                {
                    if ( bconfig->features.ac3_transcode )
                    {
                        if(bdecode_p_hdmi_audio_format_supported(window->display->settings.dvi->handle,NEXUS_AudioCodec_eAc3))
                        {
                            BDBG_WRN(("AC3 encoded output on hdmi"));
                            hdmiAc3=true;
                            if(!spdifAc3)
                            {
                                /* Send multichannel output to DTS encoder */
                                NEXUS_Ac3Encode_AddInput(audio_decode->nAc3Encoder,
                                   NEXUS_AudioDecoder_GetConnector(audio_decode->nAudioDecoder, NEXUS_AudioDecoderConnectorType_eMultichannel));
                            }
                            /* Send compressed AC3 data to HDMI */
                            NEXUS_AudioOutput_RemoveAllInputs(NEXUS_HdmiOutput_GetAudioConnector(window->display->settings.dvi->handle));
                            NEXUS_AudioOutput_AddInput(NEXUS_HdmiOutput_GetAudioConnector(window->display->settings.dvi->handle),
                                       NEXUS_Ac3Encode_GetConnector(audio_decode->nAc3Encoder));
    
                        }
                        else
                        {
                            BDBG_WRN(("AC3 Encode requested on HDMI but receiver does not support AC3 audio."));
                        }
                    }
                    else
                    {
                        BDBG_WRN(("AC3 encode requested on HDMI but not available in this build."));
                    }
                }
                else
                {
                    BDBG_WRN(("AC3 encode requested on HDMI but can not be performed for this source codec."));
                }
            }
        }
    }
#endif /* B_N_DVI_OUTPUTS */

    /* Connect compressed outputs */
#if B_N_SPDIF_OUTPUTS || B_N_DVI_OUTPUTS
    if ( (window->display->settings.spdif && (!window->display->settings.spdif->desired.pcm
                                              #if (BCHP_CHIP==7400)
                                              || (startSettings.codec == NEXUS_AudioCodec_eDts)
                                              #endif
                                              ) )
#if B_N_DVI_OUTPUTS
         || (window->display->settings.dvi && ((window->display->settings.dvi->desired.hdmi_audio_mode == boutput_hdmi_audio_mode_compressed)
                                              #if (BCHP_CHIP==7400)
                                              || (startSettings.codec == NEXUS_AudioCodec_eDts)
                                              #endif
                                               )
              && nHdmiStatus.connected )
#endif
        )
    {
        if ( NULL == g_audio_passthrough_owner )
        {
            /* g_audio_passthrough_owner = audio_decode; */
            switch ( startSettings.codec )
            {
            case NEXUS_AudioCodec_eAc3Plus:
                #if B_N_SPDIF_OUTPUTS
                /* Simul mode on SPDIF */
                if ( window->display->settings.spdif && !window->display->settings.spdif->desired.pcm && !spdifDts && !spdifAc3 )
                {
                    BDBG_MSG(("DDP-DD converted(simul) output on SPDIF"));
                    NEXUS_AudioOutput_RemoveAllInputs(NEXUS_SpdifOutput_GetConnector(window->display->settings.spdif->handle));
                    NEXUS_AudioOutput_AddInput(NEXUS_SpdifOutput_GetConnector(window->display->settings.spdif->handle),
                                               NEXUS_AudioDecoder_GetConnector(audio_decode->nAudioDecoder, NEXUS_AudioDecoderConnectorType_eCompressed));
                }
                #if (BCHP_CHIP == 7400)
                audio_decode->spdif_forced_compressed = false;
                #endif
                #endif
                #if B_N_DVI_OUTPUTS
                if ( window->display->settings.dvi && window->display->settings.dvi->desired.hdmi_audio_mode == boutput_hdmi_audio_mode_compressed
                     &&nHdmiStatus.connected && !hdmiDts && !hdmiAc3 )
                {
                    if(bdecode_p_hdmi_audio_format_supported(window->display->settings.dvi->handle,NEXUS_AudioCodec_eAc3Plus))
                    {
                        BDBG_MSG(("DDP passthrough on HDMI"));
                        g_audio_passthrough_owner = audio_decode;
                        NEXUS_AudioOutput_RemoveAllInputs(NEXUS_HdmiOutput_GetAudioConnector(window->display->settings.dvi->handle));
                        NEXUS_AudioOutput_AddInput(NEXUS_HdmiOutput_GetAudioConnector(window->display->settings.dvi->handle),
                                               NEXUS_AudioDecoder_GetConnector(g_audio_passthrough, NEXUS_AudioDecoderConnectorType_eCompressed));
                    }
                    else if(bdecode_p_hdmi_audio_format_supported(window->display->settings.dvi->handle,NEXUS_AudioCodec_eAc3))
                    {
                        BDBG_MSG(("DDP-DD converted(simul) output on hdmi"));
                        NEXUS_AudioOutput_RemoveAllInputs(NEXUS_HdmiOutput_GetAudioConnector(window->display->settings.dvi->handle));
                        NEXUS_AudioOutput_AddInput(NEXUS_HdmiOutput_GetAudioConnector(window->display->settings.dvi->handle),
                                               NEXUS_AudioDecoder_GetConnector(audio_decode->nAudioDecoder, NEXUS_AudioDecoderConnectorType_eCompressed));
                    }
                    else
                    {
                        BDBG_WRN(("DDP audio on hdmi not suppored by receiver"));
                    }
                }
                #if (BCHP_CHIP == 7400)
                audio_decode->hdmi_forced_compressed = false;
                #endif
                #endif /* B_N_DVI_OUTPUTS */
                break;
            #if (BCHP_CHIP == 7400)
            case NEXUS_AudioCodec_eMpeg:
            case NEXUS_AudioCodec_eMp3:
            #endif
            case NEXUS_AudioCodec_eWmaStd:
            case NEXUS_AudioCodec_eLpcmDvd:
            case NEXUS_AudioCodec_eLpcmHdDvd:
            case NEXUS_AudioCodec_eLpcmBluRay:
            case NEXUS_AudioCodec_ePcmWav:
            case NEXUS_AudioCodec_eCook:
            case NEXUS_AudioCodec_eAacAdts:
            case NEXUS_AudioCodec_eAacLoas:
            case NEXUS_AudioCodec_eAacPlusAdts:
            case NEXUS_AudioCodec_eAacPlusLoas:
            case NEXUS_AudioCodec_eDra:
            case NEXUS_AudioCodec_eAmr:
            case NEXUS_AudioCodec_eAdpcm:
            case NEXUS_AudioCodec_eVorbis:
            #if (NEXUS_AUDIO_MODULE_FAMILY == 7422)
            case NEXUS_AudioCodec_eWmaPro:
            #endif

                #if B_N_SPDIF_OUTPUTS
                /* katrep: Do not like to forcing PCM if compressed is request, I would rather have not audio
                  on hdmi and spdif, however brutus and traditional settop implementation PCM is forced forcing pcm to
                  maintain compatibilty of the shim layer */
                if ( window->display->settings.spdif && !window->display->settings.spdif->desired.pcm && !spdifDts && !spdifAc3 )
                {
                    NEXUS_AudioOutput_RemoveAllInputs(NEXUS_SpdifOutput_GetConnector(window->display->settings.spdif->handle));
                    NEXUS_AudioOutput_AddInput(NEXUS_SpdifOutput_GetConnector(window->display->settings.spdif->handle),
                                                NEXUS_AudioMixer_GetConnector(window->display->nAudioMixer));
                    /* window->display->settings.spdif->desired.pcm = true; */
                    window->display->settings.spdif->cfg.pcm = true;
                    audio_decode->spdif_forced_pcm = true;
                    BDBG_WRN(("forcing pcm on spdif for %s",g_audioCodecName[startSettings.codec]));
                }
                #if (BCHP_CHIP == 7400)
                audio_decode->spdif_forced_compressed = false;
                #endif
                #endif
                #if B_N_DVI_OUTPUTS
                if ( window->display->settings.dvi && window->display->settings.dvi->desired.hdmi_audio_mode == boutput_hdmi_audio_mode_compressed
                     && nHdmiStatus.connected &&!hdmiDts && !hdmiAc3 )
                {
                    NEXUS_AudioOutput_RemoveAllInputs(NEXUS_HdmiOutput_GetAudioConnector(window->display->settings.dvi->handle));
                    NEXUS_AudioOutput_AddInput(NEXUS_HdmiOutput_GetAudioConnector(window->display->settings.dvi->handle),
                                                NEXUS_AudioMixer_GetConnector(window->display->nAudioMixer));
                    /* window->display->settings.dvi->desired.hdmi_audio_mode = boutput_hdmi_audio_mode_pcm; */
                    window->display->settings.dvi->cfg.hdmi_audio_mode=boutput_hdmi_audio_mode_pcm;
                    audio_decode->hdmi_forced_pcm = true;
                    BDBG_WRN(("forcing pcm on hdmi for %s",g_audioCodecName[startSettings.codec]));
                }
                #if (BCHP_CHIP == 7400)
                audio_decode->hdmi_forced_compressed = false;
                #endif
                #endif
                break;
            #if (BCHP_CHIP == 7400)
            case NEXUS_AudioCodec_eDts:
                #if B_N_SPDIF_OUTPUTS
                /* 7400 can pass thru, but can't decode DTS, force compressed */
                if ( window->display->settings.spdif )
                {
                    g_audio_passthrough_owner = audio_decode;
                    NEXUS_AudioOutput_RemoveAllInputs(NEXUS_SpdifOutput_GetConnector(window->display->settings.spdif->handle));
                    NEXUS_AudioOutput_AddInput(NEXUS_SpdifOutput_GetConnector(window->display->settings.spdif->handle),
                                               NEXUS_AudioDecoder_GetConnector(g_audio_passthrough, NEXUS_AudioDecoderConnectorType_eCompressed));
                    /* window->display->settings.spdif->desired.pcm = true; */
                    if (window->display->settings.spdif->cfg.pcm)
                    {
                        window->display->settings.spdif->cfg.pcm = false;
                        audio_decode->spdif_forced_compressed = true;
                        BDBG_WRN(("forcing compressed on spdif for %s",g_audioCodecName[startSettings.codec]));
                    }
                    else
                    {
                        audio_decode->spdif_forced_compressed = false;
                    }
                }
                #endif
                #if B_N_DVI_OUTPUTS
                if ( window->display->settings.dvi && nHdmiStatus.connected )
                {
                    g_audio_passthrough_owner = audio_decode;
                    NEXUS_AudioOutput_RemoveAllInputs(NEXUS_HdmiOutput_GetAudioConnector(window->display->settings.dvi->handle));
                    NEXUS_AudioOutput_AddInput(NEXUS_HdmiOutput_GetAudioConnector(window->display->settings.dvi->handle),
                                           NEXUS_AudioDecoder_GetConnector(g_audio_passthrough, NEXUS_AudioDecoderConnectorType_eCompressed));
                    /* window->display->settings.dvi->desired.hdmi_audio_mode = boutput_hdmi_audio_mode_pcm; */
                    if (window->display->settings.dvi->cfg.hdmi_audio_mode!=boutput_hdmi_audio_mode_compressed)
                    {
                        window->display->settings.dvi->cfg.hdmi_audio_mode=boutput_hdmi_audio_mode_compressed;
                        audio_decode->hdmi_forced_compressed = true;
                        BDBG_WRN(("forcing compressed on hdmi for %s",g_audioCodecName[startSettings.codec]));
                    }
                    else
                    {
                        audio_decode->hdmi_forced_compressed = false;
                    }
                }
                #endif
                break;
            #endif
            /* Temporarily disable WMAPro passthru code path for these chips until it gets implemented in the FW */
            #if (NEXUS_AUDIO_MODULE_FAMILY != 7422)
            case NEXUS_AudioCodec_eWmaPro:
                #if B_N_SPDIF_OUTPUTS
                /* WMAPRO needs simul mode, passthrough is not supported */
                if ( window->display->settings.spdif && !window->display->settings.spdif->desired.pcm && !spdifDts && !spdifAc3 )
                {
                    BDBG_MSG(("WMAPRO simul mode on SPDIF",startSettings.codec));
                    NEXUS_AudioOutput_RemoveAllInputs(NEXUS_SpdifOutput_GetConnector(window->display->settings.spdif->handle));
                    NEXUS_AudioOutput_AddInput(NEXUS_SpdifOutput_GetConnector(window->display->settings.spdif->handle),
                                               NEXUS_AudioDecoder_GetConnector(audio_decode->nAudioDecoder, NEXUS_AudioDecoderConnectorType_eCompressed));
                }
                #if (BCHP_CHIP == 7400)
                audio_decode->spdif_forced_compressed = false;
                #endif
                #endif
                #if B_N_DVI_OUTPUTS
                if ( window->display->settings.dvi && window->display->settings.dvi->desired.hdmi_audio_mode == boutput_hdmi_audio_mode_compressed
                     && nHdmiStatus.connected &&!hdmiDts && !hdmiAc3 )
                {
                    if(bdecode_p_hdmi_audio_format_supported(window->display->settings.dvi->handle,NEXUS_AudioCodec_eWmaPro)||
                       bdecode_p_hdmi_audio_format_supported(window->display->settings.dvi->handle,NEXUS_AudioCodec_eWmaStd))
                    {
                        BDBG_MSG(("WMAPRO simul mode  on HDMI"));
                        NEXUS_AudioOutput_RemoveAllInputs(NEXUS_HdmiOutput_GetAudioConnector(window->display->settings.dvi->handle));
                        NEXUS_AudioOutput_AddInput(NEXUS_HdmiOutput_GetAudioConnector(window->display->settings.dvi->handle),
                                               NEXUS_AudioDecoder_GetConnector(audio_decode->nAudioDecoder, NEXUS_AudioDecoderConnectorType_eCompressed));
                    }
                    else
                    {
                        BDBG_WRN(("WMAPRO audio on hdmi not suppored by receiver"));
                    }
                }
                #if (BCHP_CHIP == 7400)
                audio_decode->hdmi_forced_compressed = false;
                #endif
                #endif
                break;
            #endif

            default:
                #if B_N_SPDIF_OUTPUTS
                /* Standard Cases */
                if ( window->display->settings.spdif && !window->display->settings.spdif->desired.pcm && !spdifDts && !spdifAc3)
                {
                    BDBG_MSG(("%s passthrough on SPDIF",g_audioCodecName[startSettings.codec]));
                    g_audio_passthrough_owner = audio_decode;
                    NEXUS_AudioOutput_RemoveAllInputs(NEXUS_SpdifOutput_GetConnector(window->display->settings.spdif->handle));
                    NEXUS_AudioOutput_AddInput(NEXUS_SpdifOutput_GetConnector(window->display->settings.spdif->handle),
                                               NEXUS_AudioDecoder_GetConnector(g_audio_passthrough, NEXUS_AudioDecoderConnectorType_eCompressed));
                }
                #if (BCHP_CHIP == 7400)
                audio_decode->spdif_forced_compressed = false;
                #endif
                #endif
                #if B_N_DVI_OUTPUTS
                if ( window->display->settings.dvi && window->display->settings.dvi->desired.hdmi_audio_mode == boutput_hdmi_audio_mode_compressed
                     && nHdmiStatus.connected &&!hdmiDts && !hdmiAc3 )
                {
                    if(bdecode_p_hdmi_audio_format_supported(window->display->settings.dvi->handle,startSettings.codec))
                    {
                        BDBG_MSG((" %s passthrough on HDMI",g_audioCodecName[startSettings.codec]));
                        g_audio_passthrough_owner = audio_decode;
                        NEXUS_AudioOutput_RemoveAllInputs(NEXUS_HdmiOutput_GetAudioConnector(window->display->settings.dvi->handle));
                        NEXUS_AudioOutput_AddInput(NEXUS_HdmiOutput_GetAudioConnector(window->display->settings.dvi->handle),
                                               NEXUS_AudioDecoder_GetConnector(g_audio_passthrough, NEXUS_AudioDecoderConnectorType_eCompressed));
                    }
                    else
                    {
                        BDBG_WRN(("%s audio on hdmi not suppored by receiver",g_audioCodecName[startSettings.codec]));
                    }
                }
                #if (BCHP_CHIP == 7400)
                audio_decode->hdmi_forced_compressed = false;
                #endif
                #endif
                break;
            }
        }
        else
        {
            BDBG_WRN(("Compressed passthrough not available"));
        }
    }
#endif

    #if (BCHP_CHIP == 7400)
    /* 7400 can pass, but not decode, DTS. Undo force to compressed for non-DTS */
    if (startSettings.codec != NEXUS_AudioCodec_eDts)
    {
        #if B_N_SPDIF_OUTPUTS
        if ( window->display->settings.spdif && window->display->settings.spdif->desired.pcm &&
             audio_decode->spdif_forced_compressed )
        {
            NEXUS_AudioOutput_RemoveAllInputs(NEXUS_SpdifOutput_GetConnector(window->display->settings.spdif->handle));
            NEXUS_AudioOutput_AddInput(NEXUS_SpdifOutput_GetConnector(window->display->settings.spdif->handle),
                                       NEXUS_AudioMixer_GetConnector(window->display->nAudioMixer));
            window->display->settings.spdif->cfg.pcm = true;
            BDBG_MSG(("Undo forced compressed on spdif for %s",g_audioCodecName[startSettings.codec]));
            audio_decode->spdif_forced_compressed = false;
        }
        #endif
        #if B_N_DVI_OUTPUTS
        if ( window->display->settings.dvi && window->display->settings.dvi->desired.hdmi_audio_mode == boutput_hdmi_audio_mode_pcm
             && nHdmiStatus.connected && audio_decode->hdmi_forced_compressed )
        {
            NEXUS_AudioOutput_RemoveAllInputs(NEXUS_HdmiOutput_GetAudioConnector(window->display->settings.dvi->handle));
            NEXUS_AudioOutput_AddInput(NEXUS_HdmiOutput_GetAudioConnector(window->display->settings.dvi->handle),
                                       NEXUS_AudioMixer_GetConnector(window->display->nAudioMixer));
            window->display->settings.dvi->cfg.hdmi_audio_mode=boutput_hdmi_audio_mode_pcm;
            BDBG_MSG(("Undo forced on hdmi for %s",g_audioCodecName[startSettings.codec]));
            audio_decode->hdmi_forced_compressed = false;
        }
        #endif
    #else
    {  /* ! 7400 */
    #endif
        /* set the sync channel for audio decoder pcm */
        if(video) 
        {
            bdecode_p_set_sync_channel(audio_decode->decode,false,true,false);
        }
        bdecode_p_set_astm(audio_decode->decode,false,true,false);

        /* ip TSM related settings: specify additional pts offset & override decoder playback mode */
        bdecode_set_audio_ip_tsm_settings(audio_decode);

        bdecode_p_set_downmix(audio_decode->decode);

        if(bsettop_get_config("audio_capture")||bsettop_get_config("audio_capture_compressed"))
        {
            baudio_capture_start(audio_decode);
        }

        /* enable/disable 96khz mode for aac streams */
        if( ((startSettings.codec >= NEXUS_AudioCodec_eAac) && (startSettings.codec <= NEXUS_AudioCodec_eAacPlusAdts))
             &&  bsettop_get_config("decode_aac_96khz"))
        {

            NEXUS_AudioDecoderCodecSettings codecSettings;

            BDBG_WRN(("aac 96khz decode is enabled"));

            NEXUS_AudioDecoder_GetCodecSettings(audio_decode->nAudioDecoder,startSettings.codec,&codecSettings);
            codecSettings.codecSettings.aac.decode96Khz = true;
            rc = NEXUS_AudioDecoder_SetCodecSettings(audio_decode->nAudioDecoder,&codecSettings);
      if(rc != BERR_SUCCESS)
      {
          BDBG_WRN(("NEXUS_AudioDecoder_SetCodecSettings failed"));
      }

            if(g_audio_passthrough_owner == audio_decode)
            {
                NEXUS_AudioDecoder_GetCodecSettings(g_audio_passthrough,startSettings.codec,&codecSettings);
                codecSettings.codecSettings.aac.decode96Khz = true;
                rc = NEXUS_AudioDecoder_SetCodecSettings(g_audio_passthrough,&codecSettings);
        if(rc != BERR_SUCCESS)
        {
            BDBG_WRN(("NEXUS_AudioDecoder_SetCodecSettings failed"));
        }
            }

        }

        audio_decode->decoderStartSettings = startSettings;
        rc = NEXUS_AudioDecoder_Start(audio_decode->nAudioDecoder, &startSettings);
        if (rc) BSETTOP_ERROR(berr_external_error);
        window->display->mixerInputsStarted++;
    }

    if ( g_audio_passthrough_owner == audio_decode )
    {
        if ( NULL != audio_decode->nSecondaryPidChannel )
        {
            startSettings.pidChannel = audio_decode->nSecondaryPidChannel;
        }
        /* set the sync channel for audio decoder passthrough */
        if(video) 
        {
            bdecode_p_set_sync_channel(audio_decode->decode,false,false,true);
        }
        bdecode_p_set_astm(audio_decode->decode,false,false,true);
        audio_decode->secondary_audio_decode->nAudioDecoder = g_audio_passthrough;

        /* ip TSM related settings: specify additional pts offset & override decoder playback mode */
        bdecode_set_audio_ip_tsm_settings(audio_decode->secondary_audio_decode);

        rc = NEXUS_AudioDecoder_Start(g_audio_passthrough, &startSettings);
        if (rc) {
            audio_decode->secondary_audio_decode->nAudioDecoder = NULL;
            BSETTOP_ERROR(berr_external_error);
        }
    }

#if NEXUS_HAS_PLAYBACK
    if (source->producer.playback)
    {
        NEXUS_PlaybackPidChannelSettings settings;
        NEXUS_Playback_GetPidChannelSettings(source->producer.playback->nPlayback, audio_decode->nPidChannel, &settings);
                BDBG_ASSERT(settings.pidSettings.pidType == NEXUS_PidType_eAudio);
                settings.pidTypeSettings.audio.primary = audio_decode->nAudioDecoder;
        if ( g_audio_passthrough_owner == audio_decode && !audio_decode->nSecondaryPidChannel )
        {
            settings.pidTypeSettings.audio.secondary = g_audio_passthrough;
        }
        else
        {
            settings.pidTypeSettings.audio.secondary = NULL;
        }
        NEXUS_Playback_SetPidChannelSettings(source->producer.playback->nPlayback, audio_decode->nPidChannel, &settings);
        if ( g_audio_passthrough && audio_decode->nSecondaryPidChannel )
        {
            NEXUS_Playback_GetPidChannelSettings(source->producer.playback->nPlayback, audio_decode->nSecondaryPidChannel, &settings);
            BDBG_ASSERT(settings.pidSettings.pidType == NEXUS_PidType_eAudio);
            settings.pidTypeSettings.audio.primary = g_audio_passthrough;
            NEXUS_Playback_SetPidChannelSettings(source->producer.playback->nPlayback, audio_decode->nSecondaryPidChannel, &settings);
        }
    }
#endif

    audio_decode->started = true;
    return 0;
}

void bdecode_stop_audio(baudio_decode_t audio_decode)
{
    bstream_t source = audio_decode->decode->source;
    bdecode_window_t window = audio_decode->decode->window;
    if (!audio_decode->started) {
        return;
    }
    BDBG_MSG(("stop_audio %p",audio_decode));

    audio_decode->secondary_audio_decode->nAudioDecoder = NULL;
    if (source->producer.tuner) {
#if B_HAS_EXTERNAL_ANALOG
        bdecode_p_stop_i2s_audio( audio_decode, window);
#endif
        return;
    }

    if ( !audio_decode->started )
    {
        return;
    }

    /* Clear this before doing anything else */
    audio_decode->started = false;

    BDBG_ASSERT(window);
    if (audio_decode->nAudioDecoder) {
        #if (BCHP_CHIP==7400)
        /* could have passthru only DTS */
        if (window->display->mixerInputsStarted)
        #endif
        {
            NEXUS_AudioDecoder_Stop(audio_decode->nAudioDecoder);
            window->display->mixerInputsStarted--;
        }
        if ( g_audio_passthrough_owner == audio_decode )
        {
            NEXUS_AudioDecoder_Stop(g_audio_passthrough);
            g_audio_passthrough_owner = NULL;
        }

        if(bsettop_get_config("audio_capture")||bsettop_get_config("audio_capture_compressed"))
        {
            baudio_capture_stop(audio_decode);
        }

        bdecode_p_unset_sync_channel(audio_decode->decode,false,true,true);
        bdecode_p_unset_astm(audio_decode->decode,false,true,true);

        if ( audio_decode->nPidChannel )
        {
            bstream_p_close_pid(source, audio_decode->nPidChannel);
            audio_decode->nPidChannel = NULL;
        }
        if ( audio_decode->nSecondaryPidChannel )
        {
            bstream_p_close_pid(source, audio_decode->nSecondaryPidChannel);
            audio_decode->nSecondaryPidChannel = NULL;
        }

        if ( NULL == audio_decode->decode->settings.audio.display )
        {
            audio_decode->busy = true;
            bdisplay_p_enable_audio(window->display, false);

            bdecode_p_set_audio_display(audio_decode, NULL);

            bdisplay_p_enable_audio(window->display, true);
            audio_decode->busy = false;
        }

        bdisplay_p_set_compressed_mute(audio_decode->decode->window->display, false);

#if B_N_SPDIF_OUTPUTS
        if ( audio_decode->decode->window->display->settings.spdif &&   /* Wow, that's a lot of indirection... :) */
             !audio_decode->decode->window->display->settings.spdif->cfg.pcm )
        {
            /* Break compressed connection */
            NEXUS_AudioOutput_RemoveAllInputs(
                NEXUS_SpdifOutput_GetConnector(audio_decode->decode->window->display->settings.spdif->handle));
        }
#endif
#if B_N_DVI_OUTPUTS
        if ( audio_decode->decode->window->display->settings.dvi   &&   /* Wow, that's a lot of indirection... :) */
             (audio_decode->decode->window->display->settings.dvi->cfg.hdmi_audio_mode == boutput_hdmi_audio_mode_compressed ||
              audio_decode->decode->window->display->settings.dvi->cfg.hdmi_audio_mode == boutput_hdmi_audio_mode_pcm_6ch) )
        {
            /* Break compressed connection */
            NEXUS_AudioOutput_RemoveAllInputs(NEXUS_HdmiOutput_GetAudioConnector(audio_decode->decode->window->display->settings.dvi->handle));
        }

#endif

        if (audio_decode->nDtsEncoder) {
            NEXUS_DtsEncode_RemoveAllInputs(audio_decode->nDtsEncoder);
        }

        if (audio_decode->nAc3Encoder) {
            NEXUS_Ac3Encode_RemoveAllInputs(audio_decode->nAc3Encoder);
        }

#if 0
        if ( audio_decode->sourceChangedEventId )
        {
            BDBG_MSG(("b_event_unregister(%p)",audio_decode->sourceChangedEventId));
            b_event_unregister(audio_decode->sourceChangedEventId);
            audio_decode->sourceChangedEventId = NULL;
        }
        if ( audio_decode->sourceChangedEvent )
        {
            BDBG_MSG(("B_Event_Destroy(%p)",audio_decode->sourceChangedEvent));
            B_Event_Destroy(audio_decode->sourceChangedEvent);
            audio_decode->sourceChangedEvent = NULL;
        }
#endif
        if ( audio_decode->sourceChangedCallback)
        {
            BDBG_MSG(("b_callback_destroy(%p)",audio_decode->sourceChangedCallback));
            b_callback_destroy(audio_decode->sourceChangedCallback);
        }

        /* Force SPDIF/HDMI back to their desired states */
        if ( audio_decode->spdif_forced_pcm )
        {
            if ( audio_decode->decode->window->display->settings.spdif )
            {
                audio_decode->decode->window->display->settings.spdif->cfg.pcm = false;
            }
            audio_decode->spdif_forced_pcm = false;
        }
        if ( audio_decode->hdmi_forced_pcm )
        {
            if ( audio_decode->decode->window->display->settings.dvi )
            {
                audio_decode->decode->window->display->settings.dvi->cfg.hdmi_audio_mode = boutput_hdmi_audio_mode_compressed;
            }
            audio_decode->hdmi_forced_pcm = false;
        }
    }
}

void bdecode_p_still_ready(void *context, int param)
{
    bdecode_t decode = context;
    BSTD_UNUSED(param);
    BDBG_MSG(("still_ready"));
    /*B_Event_Set(decode->stillEvent);*/
    b_callback_fire(decode->stillEventCallback);
}

static void bdecode_p_still_handler(void *context,int param)
{
    bdecode_t decode = context;
    BDBG_ASSERT(NULL != context);
    BSTD_UNUSED(param);
    bdecode_stop(decode);
    if (decode->settings.still_picture_done) {
        /*b_unlock();*/
        (*decode->settings.still_picture_done)(decode->settings.callback_context);
        /*b_lock();*/
    }
}

static bresult bdecode_p_set_playpump_timebase(bstream_t source, NEXUS_Timebase timebase) {
    if (source->producer.playpump) {
        bplaypump_t playpump = NULL;
        NEXUS_Error nrc = NEXUS_SUCCESS;
        NEXUS_PlaypumpSettings playpumpSettings;

        BDBG_MSG(("Setting playpumpSettings.timestamp.timebase to: %d", timebase));
        playpump = source->producer.playpump;
        NEXUS_Playpump_GetSettings(playpump->nPlaypump, &playpumpSettings);
        playpumpSettings.timestamp.timebase = timebase;
        nrc = NEXUS_Playpump_SetSettings(playpump->nPlaypump, &playpumpSettings);
        if (nrc!=NEXUS_SUCCESS) return BSETTOP_ERROR(berr_external_error);
    }

    return b_ok;
}

/* todo: delet this once we have NEXUS_Timebase_GetDefaultSettings*/
void bdecode_p_set_timebase_default_settings(NEXUS_Timebase timebase)
{
    NEXUS_Error nrc = NEXUS_SUCCESS;
    NEXUS_TimebaseSettings timebaseSettings;
    NEXUS_Timebase_GetSettings(timebase, &timebaseSettings);
    timebaseSettings.freeze = false;
    timebaseSettings.sourceType = NEXUS_TimebaseSourceType_eFreeRun;
    timebaseSettings.sourceSettings.pcr.pidChannel = NULL;
    timebaseSettings.sourceSettings.pcr.trackRange = NEXUS_TimebaseTrackRange_e61ppm;
    timebaseSettings.sourceSettings.pcr.maxPcrError = 0xFF;
    nrc = NEXUS_Timebase_SetSettings(timebase, &timebaseSettings);
    if (nrc!=NEXUS_SUCCESS)  BSETTOP_ERROR(berr_external_error);
    return;
}

static bresult bdecode_p_config_timebases(bdecode_t decode, bstream_t source, bdecode_stc_channel_cfg_type cfg_type) {
    bresult rc = b_ok;
    NEXUS_Error nrc = NEXUS_SUCCESS;
    NEXUS_StcChannelSettings stcChannelSettings;
    NEXUS_TimebaseSettings timebaseSettings;
    NEXUS_Timebase new_timebase;


    NEXUS_StcChannel_GetSettings(decode->stcChannel, &stcChannelSettings);
    bdecode_p_set_timebase_default_settings(stcChannelSettings.timebase);

    /* save the output timebase for this decode */
    decode->output_timebase = stcChannelSettings.timebase;

    switch (cfg_type) {
    case BDECODE_STC_CHANNEL_CFG_TYPE_PARSER_BAND:
        /* configuredecoder/output timebase */
        if(decode->pcrPidChannel != NULL) {
            NEXUS_Timebase_GetSettings(stcChannelSettings.timebase, &timebaseSettings);
            timebaseSettings.freeze = false;
            timebaseSettings.sourceType = NEXUS_TimebaseSourceType_ePcr;
            timebaseSettings.sourceSettings.pcr.pidChannel = decode->pcrPidChannel;
            timebaseSettings.sourceSettings.pcr.trackRange = NEXUS_TimebaseTrackRange_e61ppm;
            timebaseSettings.sourceSettings.pcr.maxPcrError = 0xFF;
            nrc = NEXUS_Timebase_SetSettings(stcChannelSettings.timebase, &timebaseSettings);
            if (nrc!=NEXUS_SUCCESS) return BSETTOP_ERROR(berr_external_error);
        }
        else {
            NEXUS_Timebase_GetSettings(stcChannelSettings.timebase, &timebaseSettings);
            timebaseSettings.freeze = true;
            timebaseSettings.sourceType = NEXUS_TimebaseSourceType_eFreeRun;
            timebaseSettings.sourceSettings.freeRun.trackRange = NEXUS_TimebaseTrackRange_e61ppm;
            nrc = NEXUS_Timebase_SetSettings(stcChannelSettings.timebase, &timebaseSettings);
            if (nrc!=NEXUS_SUCCESS) return BSETTOP_ERROR(berr_external_error);
        }
        break;
    case BDECODE_STC_CHANNEL_CFG_TYPE_TTS_PACING:
    case BDECODE_STC_CHANNEL_CFG_TYPE_PCR_PACING:
        /* configure shared decoder/output timebase */
        if(decode->pcrPidChannel != NULL) {
            NEXUS_Timebase_GetSettings(stcChannelSettings.timebase, &timebaseSettings);
            timebaseSettings.freeze = false;
            timebaseSettings.sourceType = NEXUS_TimebaseSourceType_ePcr;
            timebaseSettings.sourceSettings.pcr.pidChannel = decode->pcrPidChannel;
            timebaseSettings.sourceSettings.pcr.trackRange = NEXUS_TimebaseTrackRange_e61ppm;
            timebaseSettings.sourceSettings.pcr.maxPcrError = 0xFF;
            nrc = NEXUS_Timebase_SetSettings(stcChannelSettings.timebase, &timebaseSettings);
            if (nrc!=NEXUS_SUCCESS) return BSETTOP_ERROR(berr_external_error);
        }
        else {
            NEXUS_Timebase_GetSettings(stcChannelSettings.timebase, &timebaseSettings);
            timebaseSettings.freeze = true;
            timebaseSettings.sourceType = NEXUS_TimebaseSourceType_eFreeRun;
            timebaseSettings.sourceSettings.freeRun.trackRange = NEXUS_TimebaseTrackRange_e61ppm;
            timebaseSettings.sourceSettings.freeRun.centerFrequency = 0x400000;
            nrc = NEXUS_Timebase_SetSettings(stcChannelSettings.timebase, &timebaseSettings);
            if (nrc!=NEXUS_SUCCESS) return BSETTOP_ERROR(berr_external_error);
        }
        /* set and configure playpump timebase */

        new_timebase = stcChannelSettings.timebase + B_MAX_DECODES;

        rc = bdecode_p_set_playpump_timebase(source, new_timebase);
        if (rc != b_ok) return rc;
        NEXUS_Timebase_GetSettings(new_timebase, &timebaseSettings);
        timebaseSettings.freeze = true;
        timebaseSettings.sourceType = NEXUS_TimebaseSourceType_eFreeRun;
        timebaseSettings.sourceSettings.pcr.trackRange = NEXUS_TimebaseTrackRange_e122ppm;
        nrc = NEXUS_Timebase_SetSettings(new_timebase, &timebaseSettings);
        if (nrc!=NEXUS_SUCCESS) return BSETTOP_ERROR(berr_external_error);
        break;
    case BDECODE_STC_CHANNEL_CFG_TYPE_IP_LIVE_PB:
        /* configure shared playpump/decoder timebase */
        if(decode->pcrPidChannel != NULL) {
            NEXUS_Timebase_GetSettings(stcChannelSettings.timebase, &timebaseSettings);
            timebaseSettings.freeze = false;
            timebaseSettings.sourceType = NEXUS_TimebaseSourceType_ePcr;
            timebaseSettings.sourceSettings.pcr.pidChannel = decode->pcrPidChannel;
            timebaseSettings.sourceSettings.pcr.trackRange = NEXUS_TimebaseTrackRange_e244ppm;
            timebaseSettings.sourceSettings.pcr.maxPcrError = IP_NETWORK_JITTER * 183/2;
            nrc = NEXUS_Timebase_SetSettings(stcChannelSettings.timebase, &timebaseSettings);
            if (nrc!=NEXUS_SUCCESS) return BSETTOP_ERROR(berr_external_error);
        }

        else {
            NEXUS_Timebase_GetSettings(stcChannelSettings.timebase, &timebaseSettings);
            timebaseSettings.freeze = true;
            timebaseSettings.sourceType = NEXUS_TimebaseSourceType_eFreeRun;
            timebaseSettings.sourceSettings.freeRun.trackRange = NEXUS_TimebaseTrackRange_e61ppm;
            nrc = NEXUS_Timebase_SetSettings(stcChannelSettings.timebase, &timebaseSettings);
            if (nrc!=NEXUS_SUCCESS) return BSETTOP_ERROR(berr_external_error);
        }
        /* set playpump timebase */
        rc = bdecode_p_set_playpump_timebase(source, stcChannelSettings.timebase);
        if (rc != b_ok) return rc;

        /* set and configure outputs timebase */
        new_timebase = stcChannelSettings.timebase + B_MAX_DECODES;

        NEXUS_Timebase_GetSettings(new_timebase, &timebaseSettings);
        timebaseSettings.freeze = true;
        timebaseSettings.sourceType = NEXUS_TimebaseSourceType_eFreeRun;
        timebaseSettings.sourceSettings.pcr.trackRange = NEXUS_TimebaseTrackRange_e61ppm;
        nrc = NEXUS_Timebase_SetSettings(new_timebase, &timebaseSettings);
        if (nrc!=NEXUS_SUCCESS) return BSETTOP_ERROR(berr_external_error);
        /* save the output timebase for this decode */
        decode->output_timebase = new_timebase;
        break;
    case BDECODE_STC_CHANNEL_CFG_TYPE_PVR_PB:
        /* do nothing: STC Channel config takes care of this */
        break;
    default:
        return BSETTOP_ERROR(berr_invalid_parameter);
    }

    return b_ok;
}

static void bdecode_p_unconfig_stc_channel(bdecode_t decode) 
{
    NEXUS_Error nrc = NEXUS_SUCCESS;
    NEXUS_StcChannelSettings stcChannelSettings;

    NEXUS_StcChannel_GetSettings(decode->stcChannel, &stcChannelSettings);
    stcChannelSettings.modeSettings.pcr.pidChannel = NULL;
    stcChannelSettings.mode = NEXUS_StcChannelMode_eAuto;
    stcChannelSettings.modeSettings.Auto.transportType = NEXUS_TransportType_eTs;
    nrc = NEXUS_StcChannel_SetSettings(decode->stcChannel, &stcChannelSettings);
    if (nrc) BSETTOP_ERROR(berr_external_error);
}


static bresult bdecode_p_config_stc_channel(bdecode_t decode, bstream_t source, bdecode_stc_channel_cfg_type cfg_type) {
    bresult rc = b_ok;
    NEXUS_Error nrc = NEXUS_SUCCESS;
    NEXUS_StcChannelSettings stcChannelSettings;

    NEXUS_StcChannel_GetSettings(decode->stcChannel, &stcChannelSettings);

    switch (cfg_type) {
    case BDECODE_STC_CHANNEL_CFG_TYPE_PARSER_BAND:
        BDBG_MSG(("Setting BDECODE_STC_CHANNEL_CFG_TYPE_PARSER_BAND"));
        stcChannelSettings.mode = NEXUS_StcChannelMode_ePcr;
        stcChannelSettings.autoConfigTimebase = true;
        stcChannelSettings.modeSettings.pcr.offsetThreshold = 0x465;
        stcChannelSettings.modeSettings.pcr.maxPcrError = 0x470;
        stcChannelSettings.modeSettings.pcr.disableTimestampCorrection = false;
        stcChannelSettings.modeSettings.pcr.disableJitterAdjustment = false;
        stcChannelSettings.modeSettings.pcr.pidChannel = decode->pcrPidChannel;
        break;
    case BDECODE_STC_CHANNEL_CFG_TYPE_TTS_PACING:
        BDBG_MSG(("Setting BDECODE_STC_CHANNEL_CFG_TYPE_TTS_PACING"));
        stcChannelSettings.mode = NEXUS_StcChannelMode_ePcr;
        stcChannelSettings.autoConfigTimebase = false;
        stcChannelSettings.modeSettings.pcr.offsetThreshold = 8;
        stcChannelSettings.modeSettings.pcr.maxPcrError = 255;
        stcChannelSettings.modeSettings.pcr.disableTimestampCorrection = true;
        stcChannelSettings.modeSettings.pcr.disableJitterAdjustment = true;
        stcChannelSettings.modeSettings.pcr.pidChannel = decode->pcrPidChannel;
        break;
    case BDECODE_STC_CHANNEL_CFG_TYPE_PCR_PACING:
        BDBG_MSG(("Setting BDECODE_STC_CHANNEL_CFG_TYPE_PCR_PACING"));
        stcChannelSettings.mode = NEXUS_StcChannelMode_ePcr;
        stcChannelSettings.autoConfigTimebase = false;
        stcChannelSettings.modeSettings.pcr.offsetThreshold = 900;
        stcChannelSettings.modeSettings.pcr.maxPcrError = 450;
        stcChannelSettings.modeSettings.pcr.disableTimestampCorrection = true;
        stcChannelSettings.modeSettings.pcr.disableJitterAdjustment = true;
        stcChannelSettings.modeSettings.pcr.pidChannel = decode->pcrPidChannel;
        break;
    case BDECODE_STC_CHANNEL_CFG_TYPE_IP_LIVE_PB:
        BDBG_MSG(("Setting BDECODE_STC_CHANNEL_CFG_TYPE_IP_LIVE_PB"));
        stcChannelSettings.mode = NEXUS_StcChannelMode_ePcr;
        stcChannelSettings.autoConfigTimebase = false;
        stcChannelSettings.modeSettings.pcr.offsetThreshold = IP_NETWORK_JITTER * 183;
        stcChannelSettings.modeSettings.pcr.maxPcrError = IP_NETWORK_JITTER * 183;
        stcChannelSettings.modeSettings.pcr.disableTimestampCorrection = true;
        stcChannelSettings.modeSettings.pcr.disableJitterAdjustment = true;
        stcChannelSettings.modeSettings.pcr.pidChannel = decode->pcrPidChannel;
        break;
    case BDECODE_STC_CHANNEL_CFG_TYPE_PVR_PB:
        BDBG_MSG(("Setting BDECODE_STC_CHANNEL_CFG_TYPE_PVR_PB"));
        stcChannelSettings.mode = NEXUS_StcChannelMode_eAuto;
        stcChannelSettings.modeSettings.Auto.transportType = b_mpegtype2nexus(source->mpeg.mpeg_type);
        break;
    default:
        return BSETTOP_ERROR(berr_invalid_parameter);
        break;
    }

    if(decode->pcrPidChannel)
    {
        nrc = NEXUS_StcChannel_SetSettings(decode->stcChannel, &stcChannelSettings);
        if (nrc) return BSETTOP_ERROR(berr_external_error);
    }
    return rc;
}

bresult bdecode_start(bdecode_t decode, bstream_t source, bdecode_window_t window)
{
    bresult result;
    NEXUS_Error rc = b_ok;
    bsettop_init_settings initSettings;
    bsettop_get_init_settings(&initSettings);

    if (!source) {
        return BSETTOP_ERROR(berr_invalid_parameter);
    }
    BDBG_OBJECT_ASSERT(source, bstream);

    BDBG_MSG(("bdecode_start decode %p src %p window %p", decode, source, window));

    if (decode->nStillDecoder) {
        NEXUS_StillDecoderStartSettings startSettings;
        if (window) {
            return BSETTOP_ERROR(berr_invalid_parameter);
        }

        if ( decode->stillStream ) {
            bdecode_stop(decode);
        }
        if ( source->consumers.still ) {
            BDBG_ERR(("Can only start one still decoder per stream"));
            return BSETTOP_ERROR(berr_invalid_parameter);
        }

        NEXUS_StillDecoder_GetDefaultStartSettings(&startSettings);
        decode->nStillDecoderPid = bstream_p_open_pid(source, source->mpeg.video[0].pid, bstream_pid_type_other);
        if ( NULL == decode->nStillDecoderPid ) {
            BDBG_ERR(("Unable to open still decoder PID channel"));
            return BSETTOP_ERROR(berr_external_error);
        }
        decode->stillStream = source;
        source->consumers.still = decode;
        startSettings.pidChannel = decode->nStillDecoderPid;
        startSettings.codec = b_videocodec2nexus(source->mpeg.video[0].format);
        startSettings.stillPictureReady.callback = bdecode_p_still_ready;
        startSettings.stillPictureReady.context = decode;
        BDBG_MSG(("starting still %p %d", startSettings.pidChannel, startSettings.codec));
        rc = NEXUS_StillDecoder_Start(decode->nStillDecoder, &startSettings);
        if ( rc ) {
            return BSETTOP_ERROR(berr_external_error);
        }
        return b_ok;
    }

    if (decode->started) {
        return BSETTOP_ERROR(berr_not_available);
    }
    if ( window->decode ) {
        return BSETTOP_ERROR(berr_not_available);
    }

    decode->tsm = (initSettings.lipsync_mode == bsettop_lipsync_mode_disabled)?false:true;

#if B_DECODE_CRC_CAPTURE || B_DECODE_USERDATA_CAPTURE
    decode->metadata.wptr = 0;
    decode->metadata.rptr = 0;
#endif

    decode->window = window;

    if (source->video_primer) {
        goto video_primer;
    }

    if (!source->producer.tuner) {
        /* source is digital (not analog) */
        NEXUS_StcChannelSettings stcChannelSettings;

        NEXUS_StcChannel_GetSettings(decode->stcChannel, &stcChannelSettings);
        if (source->mpeg.mpeg_type == bstream_mpeg_type_es) {
            decode->tsm = false;
        }

        decode->pcrPidChannel = source->pcrPidChannel;

        if (!decode->pcrPidChannel && source->parser_band) {
            decode->tsm = false;
        }

        if (source->parser_band) {
            /* Live */
            decode->cfg_type = BDECODE_STC_CHANNEL_CFG_TYPE_PARSER_BAND;
        }
#ifdef B_HAS_IP
        else if (source->producer.is_playback_ip) {
            /* IP */
            if (source->producer.timestamp_active) {
                if (source->producer.timestamp_enabled) {
                    decode->cfg_type = BDECODE_STC_CHANNEL_CFG_TYPE_TTS_PACING;
                }
                else {
                    decode->cfg_type = BDECODE_STC_CHANNEL_CFG_TYPE_PCR_PACING;
                }
            }
            else if (source->producer.use_live_playback_mode) {
                decode->cfg_type = BDECODE_STC_CHANNEL_CFG_TYPE_IP_LIVE_PB;
            }
            else {
                decode->cfg_type = BDECODE_STC_CHANNEL_CFG_TYPE_PVR_PB;
            }
        }
#endif
        else {
            /* PVR local playback */
            decode->cfg_type = BDECODE_STC_CHANNEL_CFG_TYPE_PVR_PB;
        }

        rc = bdecode_p_config_timebases(decode, source, decode->cfg_type);
        if (rc != b_ok) return rc;

        rc = bdecode_p_config_stc_channel(decode, source, decode->cfg_type);
        if (rc != b_ok) return rc;
   }

    /* Have display check output timebases prior to video/audio start */
    bdisplay_p_decode_starting(window->display, window, decode);

    if (decode->image_input_started) {
        NEXUS_VideoWindow_RemoveInput(window->nWindow, NEXUS_VideoImageInput_GetConnector(decode->video_decode->nVideoImageInput));
        decode->image_input_started = false;
    }

    if (decode->video_decode) {

#ifdef B_HAS_IP
        /* for playback ip fifo size was increased, for live and playback decode
            we dont need it the video threshold to be set to large fifo size */
        if (!source->producer.is_playback_ip && decode->video_decode->nVideoDecoder && NULL==bsettop_get_config("fifo_size")) {
            NEXUS_VideoDecoderSettings nVideoDecoderSettings;
            NEXUS_VideoDecoderStatus nVideoDecoderStatus;
            rc = NEXUS_VideoDecoder_GetStatus(decode->video_decode->nVideoDecoder,&nVideoDecoderStatus);
            if (rc)
            {
                rc=BERR_TRACE(rc);
            }
            else
            {
                NEXUS_VideoDecoder_GetSettings(decode->video_decode->nVideoDecoder,&nVideoDecoderSettings);
				/* Video fifo and fifo threshold must be 1.5MB or bigger, otherwise some streams will not play.
				 * 1.5MB is required for a minimum fifo size to decode HD streams
				 * Reducing the video threshold size improves the time it takes to do main/pip swap for audio */
                if(nVideoDecoderStatus.fifoSize/2 < 1572864) 
                {
                    nVideoDecoderSettings.fifoThreshold = 1572864;
                }
				else
				{
                    nVideoDecoderSettings.fifoThreshold = nVideoDecoderStatus.fifoSize/2;
				}
    	        BDBG_WRN(("Non IP mode,changing threshold to fifo_size(%d)",nVideoDecoderSettings.fifoThreshold));
                NEXUS_VideoDecoder_SetSettings(decode->video_decode->nVideoDecoder,&nVideoDecoderSettings);
            }
        }
#endif

        result = bdecode_start_video(decode->video_decode, source, window);
        if (result) BSETTOP_ERROR(result);
    }
video_primer:
    if (decode->audio_decode) {
        result = bdecode_start_audio(decode->audio_decode, source, window);
        if (result) BSETTOP_ERROR(result);
    }

    decode->started = true;
    decode->window = window;
    decode->source = source;
    source->consumers.decode = decode;
    window->decode = decode;

    if (source->video_primer) {
        return rc;
    }

    if (source->producer.playback) {
        rc = bplayback_p_start(source->producer.playback);
    }

    result = bdecode_p_start_astm(decode);
    if (result) BSETTOP_ERROR(result);

    return rc;
}

void bdecode_stop(bdecode_t decode)
{
    BDBG_MSG(("bdecode_stop %p", decode)); 

    if (decode->nStillDecoder) {
        if ( decode->stillStream )
        {
            NEXUS_StillDecoder_Stop(decode->nStillDecoder);
            bstream_p_close_pid(decode->stillStream, decode->nStillDecoderPid);
            decode->nStillDecoderPid = NULL;
            decode->stillStream->consumers.still = NULL;
            decode->stillStream = NULL;
            return;
        }
    }

    if (decode->image_input_started) {
        NEXUS_VideoWindow_RemoveInput(decode->window->nWindow, NEXUS_VideoImageInput_GetConnector(decode->video_decode->nVideoImageInput));
        decode->image_input_started = false;
    }
    if (!decode->started) {
        return;
    }

    bdecode_p_stop_astm(decode);

    BDBG_ASSERT(decode->source->consumers.decode == decode);

    if (decode->video_decode) {
        bdecode_stop_video(decode->video_decode);
    }
    if (decode->audio_decode) {
        bdecode_stop_audio(decode->audio_decode);
    }

    bdecode_p_unconfig_stc_channel(decode);

    if ( decode->pcrPidChannel )
    {
        bstream_p_close_pid(decode->source, decode->pcrPidChannel);
        decode->pcrPidChannel = NULL;
    }

    decode->started = false;
    decode->window->decode = NULL;
    decode->window = NULL;
    BDBG_OBJECT_ASSERT(decode->source, bstream);
    decode->source->consumers.decode = NULL;
    decode->source = NULL;
}

baudio_decode_t bdecode_detach_audio(bdecode_t decode)
{
    baudio_decode_t audio_decode;
    BDBG_ASSERT(NULL != decode);
    audio_decode = decode->audio_decode;

    if ( decode->settings.audio.display )
    {
        if ( decode->settings.audio.display->mixerInputsStarted > decode->started )
        {
            BDBG_ERR(("Can't detach audio while other audio inputs to the display are running."));
            return NULL;
        }
    }

    if (audio_decode) {
        if (decode->started) {
            bdecode_stop_audio(decode->audio_decode);
        }
        bdecode_p_set_audio_display(decode->audio_decode, NULL);
        audio_decode->decode = NULL;
        decode->audio_decode = NULL;
    }
    return audio_decode;
}

bresult bdecode_attach_audio(bdecode_t decode, baudio_decode_t audio)
{
    bresult rc =0;
    BDBG_ASSERT(NULL != decode);

    if(audio==NULL)
        return BSETTOP_ERROR(berr_invalid_parameter);

    if (decode->audio_decode) {
        return BSETTOP_ERROR(berr_invalid_parameter);
    }
    if ( decode->started && decode->settings.audio.display && decode->window->display->mixerInputsStarted > 0 )
    {
        BDBG_ERR(("Can't attach to a display while other inputs are running."));
        return BSETTOP_ERROR(berr_busy);
    }
    decode->audio_decode = audio;
    audio->decode = decode;
    if (decode->started) {
        /* Have display check output timebases prior to video/audio start to refresh PLL/timebase settings (needed if only detach/attach is called) */
        bdisplay_p_decode_starting(decode->window->display, decode->window, decode);
        if ( decode->settings.audio.display )
        {
            bdecode_p_set_audio_display(audio, decode->settings.audio.display);
        }
        rc = bdecode_start_audio(decode->audio_decode, decode->source, decode->window);
        if (rc) return BSETTOP_ERROR(berr_external_error);
    }
    return 0;
}

bresult bdecode_set_audio_program(bdecode_t decode, unsigned index)
{
    /* TODO:Add support for quick audio decode in case of playback*/
    baudio_decode_t audio_decode;
    audio_decode = decode->audio_decode;

    BDBG_MSG(("set audio program %d",index));

    if ( NULL == audio_decode )
    {
        return b_ok;
    }

    if ( decode->started )
    {
        bdecode_stop_audio(audio_decode);
        decode->audio_program = index;
        bdecode_start_audio(audio_decode, decode->source, decode->window);
    }
    else
    {
        decode->audio_program = index;
    }

    return b_ok;
}

baudio_decode_t bdecode_get_audio(bdecode_t decode, bobject_t audio_id)
{
    BDBG_ASSERT(NULL != decode);
    if (B_ID_GET_INDEX(audio_id) == 0)
    {
        return decode->audio_decode;
    }
    else if (decode->audio_decode && decode->audio_decode->secondary_audio_decode && decode->audio_decode->secondary_audio_decode->nAudioDecoder )
    {
        return decode->audio_decode->secondary_audio_decode;
    }
    else
    {
        /* this is normal */
        return NULL;
    }
}

bvideo_decode_t bdecode_get_video(bdecode_t decode, bobject_t video_id)
{
    BSTD_UNUSED(video_id);
    return decode->video_decode;
}

bvideo_decode_t bdecode_detach_video(bdecode_t decode)
{
    bvideo_decode_t video_decode = decode->video_decode;
    if (video_decode) {
        if (decode->started) {
            bdecode_stop_video(decode->video_decode);
            bdecode_p_unset_sync_channel(decode,true,false,false);
            bdecode_p_unset_astm(decode,true,false,false);
        }
        decode->video_decode = NULL;
    }
    return video_decode;
}

bresult bdecode_attach_video(bdecode_t decode, bvideo_decode_t video)
{
    bresult rc =0;
    if (decode->video_decode) {
        return BSETTOP_ERROR(berr_invalid_parameter);
    }
    decode->video_decode = video;

    /* apply settings that were deferred before */
    rc = bdecode_set(decode, &decode->settings);
    if (rc) return BSETTOP_ERROR(berr_external_error);

    if (decode->started) {
        rc =  bdecode_start_video(decode->video_decode, decode->source, decode->window);
        if (rc) return BSETTOP_ERROR(berr_external_error);
    }
    return rc;
}

bresult bdecode_set_video_program(bdecode_t decode, unsigned index)
{
    decode->video_program = index;
    return 0;
}

/* this works because the struct is already memset to 0 */
#define BRCM_AUD_MEMCPY(a,b) BKNI_Memcpy(a,b,strlen(b))

static const char * audio_layer_table[] = {
    "AAC", /* AAC is extension of mpeg spec, and spec gives it a layer 0 value */
    "Layer 3",
    "Layer 2",
    "Layer 1",
    "Unknown"
};
/* audio sample rate table, must match enum in bavc.h */
static const unsigned long sr_table[] = {
    32000, /* BAVC_AudioSamplingRate_e32k */
    44100, /* BAVC_AudioSamplingRate_e44_1k */
    48000, /* BAVC_AudioSamplingRate_e48k */
    96000, /* BAVC_AudioSamplingRate_e96k */
    /* added to support for digital audio receiver */
    16000, /* BAVC_AudioSamplingRate_e16k */
    22050, /* BAVC_AudioSamplingRate_e22_05k */
    24000, /* BAVC_AudioSamplingRate_e24k */
    64000, /* BAVC_AudioSamplingRate_e64k */
    88200, /* BAVC_AudioSamplingRate_e88_2k */
    128960, /* BAVC_AudioSamplingRate_e128k */
    176400, /* BAVC_AudioSamplingRate_e176_4k */
    192960 /* BAVC_AudioSamplingRate_e192k */
};

/* NOTE: lfe is low frequency effects, somewhat similar to subwoofer */
static const char * rap_ac3_chan_table[] = {
    "1+1", "1/0.0", "2/0.0", "3/0.0", "2/1.0", "3/1.0", "2/2.0", "3/2.0", "eMax"
};
static const char * rap_ac3_chan_table_lfe[] = {
    "1+1", "1/0.1", "2/0.1", "3/0.1", "2/1.1", "3/1.1", "2/2.1", "3/2.1", "eMax"
};
static const char * rap_downmix_mode_table[] = {
    /* based on BRAP_OutputMode, beware deprecated values */
/*    "1/0", "1/1", "unused", "2/0", "3/0", "2/1", "3/1", "2/2", "3/2", "eMax" */
/* TODO: remap BRAP_OutputMode to downmixType and dolby.stereoDownmixMode */
    "Auto", "1_0", "1_1", "2_0", "3_0", "2_1", "3_1", "2_2", "3_2", "3_4", "eMax"
};

static const char * rap_dts_chan_table[] = {
    "1.0", "2.0", "2.0", "2.0", "2.0", "2.1", "2.1", "2.1.1", "2.2", "5.1", "5.1", "5.1", "5.1", "7.1", "7.1", "7.1", "User Defined", "eMax"
};
static const char * rap_dra_chan_table[] = {
    "1.1", "2.0", "2.1", "2.2", "3.2", "3.3", "5.2", "5.3", "eMax"
};
void baudio_decode_get_status(baudio_decode_t audio, baudio_decode_status *status)
{
    NEXUS_AudioDecoderStatus nStatus;
    NEXUS_Error rc;
    NEXUS_AudioDecoderSettings nSetting;

    BKNI_Memset(status, 0, sizeof(*status));

    status->common.program = audio->decode->audio_program;

    if ( audio == g_audio_passthrough_owner || NULL == g_audio_passthrough_owner )
    {
        status->compressed_spdif_output = true;
    }

    NEXUS_AudioDecoder_GetSettings(audio->nAudioDecoder, &nSetting);

    rc = NEXUS_AudioDecoder_GetStatus(audio->nAudioDecoder, &nStatus);
    if (rc) return;

    if(nStatus.codec==NEXUS_AudioCodec_eMpeg)
    {
        if(nStatus.codecStatus.mpeg.layer < NEXUS_AudioMpegLayer_e3 || nStatus.codecStatus.mpeg.layer > NEXUS_AudioMpegLayer_e1)
           nStatus.codecStatus.mpeg.layer = NEXUS_AudioMpegLayer_eMax;   /* Init to a default value */
    }

    if (!nStatus.started) return;

    status->common.fifo_depth = nStatus.fifoDepth;
    status->common.fifo_size = nStatus.fifoSize;
    status->common.pts = nStatus.pts;
    switch (nStatus.ptsType) {
    case NEXUS_PtsType_eCoded:
        status->common.pts_type = bdecode_pts_type_coded; break;
    case NEXUS_PtsType_eInterpolatedFromValidPTS:
        status->common.pts_type = bdecode_pts_type_interpolated; break;
    default:
        status->common.pts_type = bdecode_pts_type_invalid; break;
    }
    bdecode_get_stc(audio->decode, &status->common.stc);
    status->common.started = nStatus.started;
    status->common.pts_error_cnt = nStatus.ptsErrorCount;

    if(audio->decode->window && audio->decode->window->display && audio->decode->window->display->settings.spdif)
    {
        if(audio->decode->window->display->settings.spdif->desired.pcm)
            status->compressed_spdif_output = false;
        else
            status->compressed_spdif_output = true;
    }

    status->codec = b_nexus2audiocodec(nStatus.codec);
    status->sample_rate = nStatus.sampleRate;
    status->layer[0] = '\0';
    switch(nStatus.codec)
    {
    case NEXUS_AudioCodec_eAc3:
    case NEXUS_AudioCodec_eAc3Plus:
        status->bitrate = nStatus.codecStatus.ac3.bitrate;
        if(nStatus.codecStatus.ac3.acmod <= NEXUS_AudioAc3Acmod_eMax)
        {
            if(nStatus.codecStatus.ac3.lfe)
            {
                BRCM_AUD_MEMCPY(status->channels, rap_ac3_chan_table_lfe[nStatus.codecStatus.ac3.acmod]);
            }
            else
            {
                BRCM_AUD_MEMCPY(status->channels, rap_ac3_chan_table[nStatus.codecStatus.ac3.acmod]);
            }
        }

        /*status->status->channels*/
        break;
    case NEXUS_AudioCodec_eAac:
    case NEXUS_AudioCodec_eAacPlus:
        status->bitrate = nStatus.codecStatus.aac.bitrate;
        if(nStatus.codecStatus.aac.acmod <NEXUS_AudioAacAcmod_eMax)
        {
            if(nStatus.codecStatus.aac.lfe)
            {
                BRCM_AUD_MEMCPY(status->channels, rap_ac3_chan_table_lfe[nStatus.codecStatus.aac.acmod]);
            }
            else
            {
                BRCM_AUD_MEMCPY(status->channels, rap_ac3_chan_table[nStatus.codecStatus.aac.acmod]);
            }
        }
        break;
    case NEXUS_AudioCodec_eMpeg:
    case NEXUS_AudioCodec_eMp3:
        status->bitrate = nStatus.codecStatus.mpeg.bitrate;

        switch(nStatus.codecStatus.mpeg.channelMode)
        {
        case NEXUS_AudioMpegChannelMode_eStereo:
            BRCM_AUD_MEMCPY(status->channels, "2.0");
            break;
        case NEXUS_AudioMpegChannelMode_eIntensityStereo:
            BRCM_AUD_MEMCPY(status->channels, "2.0!");
            break;
        case NEXUS_AudioMpegChannelMode_eDualChannel:
            BRCM_AUD_MEMCPY(status->channels, "1+1");
            break;
        case NEXUS_AudioMpegChannelMode_eSingleChannel:
            BRCM_AUD_MEMCPY(status->channels, "1.0");
            break;
        default:
            BDBG_WRN(("Invalid channel mode"));
        }

        BRCM_AUD_MEMCPY(status->layer, audio_layer_table[nStatus.codecStatus.mpeg.layer]);
        break;
    case NEXUS_AudioCodec_eWmaStd:
    case NEXUS_AudioCodec_eWmaPro:
#if 0
        switch (nStatus.codecStatus.wma.acmod) {
        case BRAP_DSPCHN_WmaStdAcmod_eOneCh:
            BRCM_AUD_MEMCPY(status->channels, "1.0");
            break;
        case BRAP_DSPCHN_WmaStdAcmod_eTwoCh:
            BRCM_AUD_MEMCPY(status->channels, "2.0");

            break;
        }
#endif
        break;
    case NEXUS_AudioCodec_eDts:        
    case NEXUS_AudioCodec_eDtsHd:
        if(nStatus.codecStatus.dts.amode < NEXUS_AudioDtsAmode_eMax)
        {            
            BRCM_AUD_MEMCPY(status->channels, rap_dts_chan_table[nStatus.codecStatus.dts.amode]);
        }
        else
        {
            BDBG_ERR(("Invalid channel mode"));
        }
        break;
    case NEXUS_AudioCodec_eDra:
        if(nStatus.codecStatus.dra.acmod < NEXUS_AudioDraAcmod_eMax)
        {            
            BRCM_AUD_MEMCPY(status->channels, rap_dra_chan_table[nStatus.codecStatus.dra.acmod]);
        }
        else
        {
            BDBG_ERR(("Invalid channel mode"));
        }
        break;
    case NEXUS_AudioCodec_eLpcmDvd:
    case NEXUS_AudioCodec_eLpcmHdDvd:
    case NEXUS_AudioCodec_eLpcmBluRay:
    case NEXUS_AudioCodec_ePcm:
    case NEXUS_AudioCodec_ePcmWav:
    case NEXUS_AudioCodec_eCook:
    case NEXUS_AudioCodec_eVorbis:
    case NEXUS_AudioCodec_eUnknown:
        /* this is normal in transitions */
        break;
    default:
        BDBG_MSG(("Unsupported audio format %d",nStatus.codec));
        break;
    }

    /* supress warning message TODO: status->downmix_mode */
    BRCM_AUD_MEMCPY(status->downmix_mode, rap_downmix_mode_table[nSetting.outputMode]);


#if 0
    /* TODO:Need support for these */
    status->frame_queue;
    status->downmix_mode;
#endif
}

void bvideo_decode_get_status(bvideo_decode_t video, bvideo_decode_status *status)
{
    NEXUS_VideoDecoderStatus nStatus;
    NEXUS_Error rc;

    BKNI_Memset(status, 0, sizeof(*status));

    rc = NEXUS_VideoDecoder_GetStatus(video->nVideoDecoder, &nStatus);
    if (rc) return;

    status->common.fifo_depth = nStatus.fifoDepth;
    status->common.fifo_size = nStatus.fifoSize;
    status->common.pts = nStatus.pts;
    switch (nStatus.ptsType) {
    case NEXUS_PtsType_eCoded:
        status->common.pts_type = bdecode_pts_type_coded; break;
    case NEXUS_PtsType_eInterpolatedFromValidPTS:
        status->common.pts_type = bdecode_pts_type_interpolated; break;
    default:
        status->common.pts_type = bdecode_pts_type_invalid; break;
    }
    bdecode_get_stc(video->decode, &status->common.stc);
    status->common.program = video->decode->video_program;
    status->common.started = nStatus.started;
    status->common.pts_error_cnt = nStatus.ptsErrorCount;

    if (video->decode->source) {
        status->codec = video->decode->source->mpeg.video[video->decode->video_program].format;
    }
    /* TODO: status->panscan_status; */
    status->aspect_ratio = (unsigned)nStatus.aspectRatio;

    status->source_width = nStatus.source.width;
    status->source_height = nStatus.source.height;
    status->picture_queue = nStatus.queueDepth;
    status->interlaced = nStatus.interlaced;
    status->time_code.hours = nStatus.timeCode.hours;
    status->time_code.minutes = nStatus.timeCode.minutes;
    status->time_code.seconds = nStatus.timeCode.seconds;
    status->time_code.pictures = nStatus.timeCode.pictures;
    status->frame_rate = b_nexus2framerate(nStatus.frameRate);
    status->display_drops = nStatus.numDisplayDrops;
    status->display_underflows = nStatus.numDisplayUnderflows;
}

bresult bdecode_get_status(bdecode_t decode, bdecode_status *status)
{
    BKNI_Memset(status, 0, sizeof(*status));
    if (decode->video_decode) {
        bvideo_decode_status video;

        bvideo_decode_get_status(decode->video_decode, &video);
        /* copy status to legacy structure */
        status->source_width = video.source_width;
        status->source_height = video.source_height;
        status->interlaced = video.interlaced;
        status->video_codec = video.codec;
        status->video_frame_rate = video.frame_rate;
        status->video_fifo_depth = video.common.fifo_depth;
        status->video_fifo_size = video.common.fifo_size;
        status->video_pts = video.common.pts;
        status->video_pts_type = video.common.pts_type;
        status->video_stc = video.common.stc;
        status->video_aspect_ratio = video.aspect_ratio;
        status->video_program = video.common.program;
        status->video_picture_queue = video.picture_queue;
        status->video_panscan_status = video.panscan_status;
        status->time_code = video.time_code;
        status->video_pts_error_cnt = video.common.pts_error_cnt;
        status->video_display_underflows = video.display_underflows;
        status->video_display_drops = video.display_drops;
    }
    if (decode->audio_decode) {
        baudio_decode_status audio;
        baudio_decode_get_status(decode->audio_decode, &audio);
        status->audio_fifo_depth = audio.common.fifo_depth;
        status->audio_fifo_size = audio.common.fifo_size;
        status->audio_pts = audio.common.pts;
        status->audio_pts_type = audio.common.pts_type;
        status->audio_stc = audio.common.stc;
        status->audio_program = audio.common.program;
        status->audio_sample_rate = audio.sample_rate;
        status->audio_frame_queue = audio.frame_queue;
        BKNI_Memcpy(status->audio_channels, audio.channels, sizeof(audio.channels));
        BKNI_Memcpy(status->audio_downmix_mode, audio.downmix_mode, sizeof(audio.downmix_mode));
        BKNI_Memcpy(status->audio_layer, audio.layer, sizeof(audio.layer));
        status->audio_compressed_spdif_output= audio.compressed_spdif_output;
        status->audio_bitrate = audio.bitrate;
        status->audio_format = audio.codec;
        status->audio_pts_error_cnt = audio.common.pts_error_cnt;
    }
    return 0;
}

bresult bdecode_set_audio_volume(bdecode_t decode, const baudio_volume *volume)
{
    int leftVolume;
    int rightVolume;

    decode->volume = *volume;
    leftVolume = b_volume2nexus(volume->left, false, &decode->volume.left);
    rightVolume = b_volume2nexus(volume->right, false, &decode->volume.right);
    if (decode->window && decode->audio_decode && decode->audio_decode->nAudioDecoder)
    {
        bdecode_p_set_downmix(decode);
    }
    return 0;
}

static bresult bdecode_set_audio_ip_tsm_settings(baudio_decode_t audio_decode)
{
#ifdef B_HAS_IP
    if (audio_decode && audio_decode->nAudioDecoder)
    {
        NEXUS_AudioDecoderSettings  nAudioDecodeSettings;

        NEXUS_AudioDecoder_GetSettings(audio_decode->nAudioDecoder, &nAudioDecodeSettings);

        switch (audio_decode->decode->cfg_type) {
        case BDECODE_STC_CHANNEL_CFG_TYPE_PARSER_BAND:
            nAudioDecodeSettings.ptsOffset = 0;
            break;
        case BDECODE_STC_CHANNEL_CFG_TYPE_TTS_PACING:
        case BDECODE_STC_CHANNEL_CFG_TYPE_PCR_PACING:
            nAudioDecodeSettings.ptsOffset = 0;
            break;
        case BDECODE_STC_CHANNEL_CFG_TYPE_IP_LIVE_PB:
            nAudioDecodeSettings.ptsOffset = 45 * IP_NETWORK_JITTER;
            break;
        case BDECODE_STC_CHANNEL_CFG_TYPE_PVR_PB:
            nAudioDecodeSettings.ptsOffset = 0;
            break;
        default:
            return BSETTOP_ERROR(berr_invalid_parameter);
            break;
        }
        if(audio_decode->decode->settings.tsm.wga_mode_enable) {
            BDBG_WRN(("enabling WGA mode"));
            nAudioDecodeSettings.wideGaThreshold = true;
        }

        if (NEXUS_AudioDecoder_SetSettings(audio_decode->nAudioDecoder, &nAudioDecodeSettings))
        {
            return BSETTOP_ERROR(berr_external_error);
        }
    }
    return b_ok;
#else
    BSTD_UNUSED(audio_decode);
    return (berr_not_supported);
#endif
}

bresult bdecode_get_audio_volume(bdecode_t decode, baudio_volume *volume)
{
    *volume = decode->volume;
    return 0;
}

bresult bdecode_get_stc(bdecode_t decode, uint32_t *stc)
{
    NEXUS_StcChannel_GetStc(decode->stcChannel, stc);
    return 0;
}

bresult bdecode_set_stc(bdecode_t decode, uint32_t stc)
{
    NEXUS_StcChannel_SetStc(decode->stcChannel, stc);
    return 0;
}

void bdecode_slow_clock(bdecode_t decode, bool slow)
{
    /* This function is not used in Settop API. It is deprecated. See bplaypump_set_pwm_value instead. */
    BSTD_UNUSED(decode);
    BSTD_UNUSED(slow);
    BSETTOP_ERROR(berr_not_supported);
}

#if B_DECODE_CRC_CAPTURE || B_DECODE_USERDATA_CAPTURE
static void bdecode_p_add_metadata(bdecode_t decode, uint32_t *data, uint32_t size)
{
    uint32_t empty_space;

    if(decode->metadata.wptr < decode->metadata.rptr)
        empty_space = decode->metadata.rptr- decode->metadata.wptr;
    else
        empty_space = B_DECODE_METADATA_CAPTURE_SIZE - decode->metadata.wptr + decode->metadata.rptr;

    if(size <= empty_space){
        if((decode->metadata.wptr + size)<=B_DECODE_METADATA_CAPTURE_SIZE){
            BKNI_Memcpy(&decode->metadata.data[decode->metadata.wptr], data, size*sizeof(uint32_t));
        }
        else{
            uint32_t partial_size = B_DECODE_METADATA_CAPTURE_SIZE-decode->metadata.wptr;
            BKNI_Memcpy(&decode->metadata.data[decode->metadata.wptr], data, partial_size*sizeof(uint32_t));
            BKNI_Memcpy(&decode->metadata.data[0], data+partial_size, (size-partial_size)*sizeof(uint32_t));
        }

        decode->metadata.wptr += size;
        decode->metadata.wptr = decode->metadata.wptr % B_DECODE_METADATA_CAPTURE_SIZE;

    }

    BDBG_ASSERT(decode->metadata.wptr != decode->metadata.rptr);
}
#endif

#if B_DECODE_USERDATA_CAPTURE
static void check_for_userdata(bdecode_t decode)
{
    uint32_t header[4];

    header[0] = 1; /* 1 is user data */
#if 0
    header[1] = info->ui32UserDataBufSize + 2*sizeof(uint32_t);
    header[2] = info->ui32PTS;
    header[3] = ((((uint16_t)info->bPTSValid)<<16) | ((uint16_t)info->ui32UserDataBufSize));
#endif

    bdecode_p_add_metadata(decode, header, 4);
    bdecode_p_add_metadata(decode, (uint32_t*)info->pUserDataBuffer, info->ui32UserDataBufSize/sizeof(uint32_t));
    if (info->ui32UserDataBufSize % sizeof(uint32_t)) {
        BDBG_ERR(("user data dropped"));
    }
}
#endif

#if B_DECODE_CRC_CAPTURE
void check_for_crcdata(bdecode_t decode)
{
    if (!decode->video_decode) return;
    while (1) {
        NEXUS_Error rc;
        NEXUS_VideoInputCrcData crcData;
        uint32_t crc_data[7];
        unsigned n;

        rc = NEXUS_VideoInput_GetCrcData(NEXUS_VideoDecoder_GetConnector(decode->video_decode->nVideoDecoder), &crcData, 1, &n);
        if (rc) {
            BDBG_ERR(("NEXUS_VideoInput_GetCrcData failed %d", rc));
            break;
        }
        if (!n) {
            break;
        }

        crc_data[0] = 0; /* 0 is CRC */
        crc_data[1] = (sizeof(uint32_t)*5);
        crc_data[2] = crcData.idrPictureId;
        crc_data[3] = crcData.pictureOrderCount;
        crc_data[4] = crcData.lumaCrc;
        crc_data[5] = crcData.chromaCrc;
        crc_data[6] = crcData.isField;

        bdecode_p_add_metadata(decode, crc_data, 7);
    }
}
#endif


bresult bdecode_read_metadata(bdecode_t decode, void *buffer, unsigned size, unsigned *amount_read)
{
    *amount_read = 0;
#if B_DECODE_CRC_CAPTURE || B_DECODE_USERDATA_CAPTURE
#if B_DECODE_USERDATA_CAPTURE
    check_for_userdata(decode);
#endif
#if B_DECODE_CRC_CAPTURE
    check_for_crcdata(decode);
#endif

    if (decode->metadata.rptr != decode->metadata.wptr) {
        unsigned n;
        uint32_t consumable_buffer_size;

        if (decode->metadata.rptr < decode->metadata.wptr)
            n = decode->metadata.wptr - decode->metadata.rptr;
        else
            n = decode->metadata.wptr + B_DECODE_METADATA_CAPTURE_SIZE - decode->metadata.rptr;

        /* calculate the buffer that's consumable */
        consumable_buffer_size = n * sizeof(uint32_t);

        /* determine the final payload size */
        if (consumable_buffer_size > size)
            consumable_buffer_size = size;

        *amount_read = consumable_buffer_size;

        if(((B_DECODE_METADATA_CAPTURE_SIZE-decode->metadata.rptr)*sizeof(uint32_t))>=consumable_buffer_size)
            BKNI_Memcpy(buffer, &decode->metadata.data[decode->metadata.rptr], consumable_buffer_size);
        else{
            uint32_t patrial_size = consumable_buffer_size - (decode->metadata.wptr * sizeof(uint32_t));
            BKNI_Memcpy(buffer, &decode->metadata.data[decode->metadata.rptr], patrial_size);
            BKNI_Memcpy((uint8_t*)buffer + patrial_size, &decode->metadata.data[0], decode->metadata.wptr * sizeof(uint32_t));
        }

        /* update the read pointer */
        decode->metadata.rptr +=  consumable_buffer_size / sizeof(uint32_t);
        decode->metadata.rptr = decode->metadata.rptr % B_DECODE_METADATA_CAPTURE_SIZE;
    }
#else
    BSTD_UNUSED(decode);
    BSTD_UNUSED(buffer);
    BSTD_UNUSED(size);
#endif
    return b_ok;
}

bresult bdecode_allocate_capture_buffers(bdecode_t decode, unsigned count)
{
    /* This does nothing at the time. By default, Nexus allows 1 capture buffer. This could be
    increased if needed. */
    BSTD_UNUSED(decode);
    BSTD_UNUSED(count);
    return 0;
}

bsurface_t bdecode_acquire_capture_buffer(bdecode_t decode, bgraphics_t graphics, bool *top_field)
{
    bsurface_t surface = NULL;
    NEXUS_SurfaceHandle nSurface;

#if B_N_GRC
    if (decode->nStillDecoder) {
        NEXUS_StripedSurfaceHandle stripedSurface;
        if (NEXUS_StillDecoder_GetStripedSurface(decode->nStillDecoder, &stripedSurface)) {
            return NULL;
        }
        nSurface = NEXUS_Graphics2D_Destripe(graphics->graphics2d, stripedSurface);
    }
    else
#endif
    {
        BSTD_UNUSED(graphics);
        nSurface = NEXUS_VideoWindow_CaptureVideoBuffer(decode->window->nWindow);
    }

    if (top_field) *top_field = true; /* TODO */

    if (nSurface) {
        NEXUS_SurfaceMemory mem;
        NEXUS_SurfaceCreateSettings createSettings;

        NEXUS_Surface_GetMemory(nSurface, &mem);
        NEXUS_Surface_GetCreateSettings(nSurface, &createSettings);

        surface = BKNI_Malloc(sizeof(*surface));
        BKNI_Memset(surface, 0, sizeof(*surface));
        BDBG_OBJECT_SET(surface, bsurface);
        surface->graphics = graphics;
        surface->nSurface = nSurface;
        surface->mem.buffer = mem.buffer;
        surface->mem.pitch = mem.pitch;
        surface->is_fb = false;
        surface->create_settings.pixel_format = bgraphics_pixel_format_y08_cb8_y18_cr8; /* TODO */
        surface->create_settings.width = createSettings.width;
        surface->create_settings.height = createSettings.height;
        surface->settings.position.width = surface->create_settings.width;
        surface->settings.position.height = surface->create_settings.height;
        surface->settings.position.x = 0;
        surface->settings.position.y = 0;
        surface->settings.cliprect = surface->settings.position;
        surface->settings.zorder = 0;
        surface->settings.visible = false;
        surface->settings.color = 0xFF000000;
        surface->settings.alpha = 0xFF;
    }

    return surface;
}

bresult bdecode_release_capture_buffer(bdecode_t decode, bsurface_t surface)
{
    NEXUS_Error rc;

    if (decode->nStillDecoder) {
        return BSETTOP_ERROR(berr_invalid_parameter);
    }

    rc = NEXUS_VideoWindow_ReleaseVideoBuffer(decode->window->nWindow, surface->nSurface);
    BKNI_Free(surface);

    return rc;
}

void bdecode_p_enable_audio(bdecode_t decode, bool enabled)
{
    NEXUS_VideoDecoderSettings nVideoDecoderSettings;
    NEXUS_VideoDecoderStatus nVideoDecoderStatus;
    NEXUS_Error rc;

    BKNI_Memset(&nVideoDecoderSettings,0,sizeof(nVideoDecoderSettings));
    BKNI_Memset(&nVideoDecoderStatus,0,sizeof(nVideoDecoderStatus));
    if(decode->video_decode && decode->video_decode->nVideoDecoder)
    {
        rc = NEXUS_VideoDecoder_GetStatus(decode->video_decode->nVideoDecoder,&nVideoDecoderStatus);
        if (rc)
        {
            rc=BERR_TRACE(rc);
        }
        NEXUS_VideoDecoder_GetSettings(decode->video_decode->nVideoDecoder,&nVideoDecoderSettings);
    }

    /* Be careful how you use this function.  The asserts do their best to catch errors, but
       you must remember to disable only once and enable before leaving the thunk layer */
    B_LOCK_ASSERT();
    BDBG_MSG(("Checking decode %p started %d audio %p", decode, decode->started, decode->audio_decode));
    if ( decode->audio_decode && !decode->audio_decode->busy )
    {
        if ( enabled )
        {
            BDBG_ASSERT(true == decode->audio_disabled);
            decode->audio_disabled = false;
            BDBG_MSG(("ENABLING audio decode %p", decode->audio_decode));
            bdecode_start_audio(decode->audio_decode, decode->source, decode->window);
#if B_HAS_IP /* restore the threshold */
            if(nVideoDecoderSettings.fifoThreshold)
                nVideoDecoderSettings.fifoThreshold *=2;
#else
            nVideoDecoderSettings.fifoThreshold=0;
#endif
            if(decode->video_decode && decode->video_decode->nVideoDecoder)
                NEXUS_VideoDecoder_SetSettings(decode->video_decode->nVideoDecoder,&nVideoDecoderSettings);
        }
        else
        {
#if B_HAS_IP /* lower the threshold */
            if(nVideoDecoderSettings.fifoThreshold)
            {
                nVideoDecoderSettings.fifoThreshold /=2;
            }
            else if (!decode->source->parser_band && !decode->source->producer.is_playback_ip)
            {
                /* only do this for the PVR playback case, delayed startup of audio is not a problem for non-playback */
                nVideoDecoderSettings.fifoThreshold = nVideoDecoderStatus.fifoDepth/2;
            }
#else
            if(nVideoDecoderSettings.fifoThreshold)
            {
                BDBG_ERR(("not using default threshold"));
            }
            if (rc == NEXUS_SUCCESS)
            {
                nVideoDecoderSettings.fifoThreshold = nVideoDecoderStatus.fifoDepth/2;
            }
#endif
            if(decode->video_decode && decode->video_decode->nVideoDecoder)
                NEXUS_VideoDecoder_SetSettings(decode->video_decode->nVideoDecoder,&nVideoDecoderSettings);

            decode->audio_disabled = true;  /* MUST SET THIS FIRST */
            BDBG_MSG(("DISABLING audio decode %p", decode->audio_decode));
            bdecode_stop_audio(decode->audio_decode);
        }
    }
}

void bdecode_p_mpeg_change(bdecode_t decode, const bstream_mpeg *new_settings)
{
    /* TODO: This currently does nothing to the decoder, based on settop api's definition of
       bstream_set_mpeg_parameters.  If later on we want the decoder to automatically restart,
       that can be done here */
    BSTD_UNUSED(decode);
    BSTD_UNUSED(new_settings);
}

#if NEXUS_HAS_SYNC_CHANNEL
/* open and sync channel with default settings */
static bresult bdecode_p_open_sync_channel(bdecode_t decode)
{
    NEXUS_Error rc =0;
    NEXUS_SyncChannelSettings nSyncChannelSettings;
    bsettop_init_settings initSettings;
    bsettop_get_init_settings(&initSettings);

    switch ( initSettings.lipsync_mode )
    {
    case bsettop_lipsync_mode_tsm:
    case bsettop_lipsync_mode_disabled:
        BDBG_WRN(("sync channel disabled"));
        return b_ok;
    default:
        break;
    }

    BDBG_MSG(("Open sync channel for decode %p",decode));
    NEXUS_SyncChannel_GetDefaultSettings(&nSyncChannelSettings);
    nSyncChannelSettings.enablePrecisionLipsync = (initSettings.lipsync_mode == bsettop_lipsync_mode_precision)?true:false;
    decode->nSyncChannel = NEXUS_SyncChannel_Create(&nSyncChannelSettings);
    if (!decode->nSyncChannel) return BSETTOP_ERROR(berr_external_error);
    return rc;
}

/* close sync channel */
static bresult bdecode_p_close_sync_channel(bdecode_t decode)
{
    bresult rc=0;
    if(decode->nSyncChannel)
    {
        NEXUS_SyncChannel_Destroy(decode->nSyncChannel);
        decode->nSyncChannel=NULL;
    }
    return rc;
}
/* set only the decoders we need */
static bresult bdecode_p_set_sync_channel(bdecode_t decode, bool video,bool audio_pcm,bool audio_passthrough)
{

    NEXUS_Error rc =0;
    NEXUS_SyncChannelSettings nSyncChannelSettings;
    bsettop_init_settings initSettings;
    bsettop_get_init_settings(&initSettings);

    switch ( initSettings.lipsync_mode )
    {
    case bsettop_lipsync_mode_tsm:
    case bsettop_lipsync_mode_disabled:
        BDBG_MSG(("Skipping sync channel configuration"));
        return b_ok;
    default:
        break;
    }

    /* sync channnel must be open */
    BDBG_ASSERT(decode->nSyncChannel);

    BDBG_MSG(("Set sync channel decode %p %s %s %s",decode,video?"video":"",audio_pcm?"pcm":"",audio_passthrough?"passthrough":""));

    NEXUS_SyncChannel_GetSettings(decode->nSyncChannel,&nSyncChannelSettings);
    if(decode->video_decode && video)
            nSyncChannelSettings.videoInput = NEXUS_VideoDecoder_GetConnector(decode->video_decode->nVideoDecoder);
    if(decode->audio_decode)
    {
        if(audio_pcm)
            nSyncChannelSettings.audioInput[0] = NEXUS_AudioDecoder_GetConnector(decode->audio_decode->nAudioDecoder, NEXUS_AudioDecoderConnectorType_eStereo);

        if(audio_passthrough && (g_audio_passthrough && g_audio_passthrough_owner == decode->audio_decode))
                nSyncChannelSettings.audioInput[1] = NEXUS_AudioDecoder_GetConnector(g_audio_passthrough, NEXUS_AudioDecoderConnectorType_eCompressed);
     }

     rc = NEXUS_SyncChannel_SetSettings(decode->nSyncChannel,&nSyncChannelSettings);
     if (rc) return BSETTOP_ERROR(berr_external_error);
     return rc;
}

static bresult bdecode_p_unset_sync_channel(bdecode_t decode,bool video,bool audio_pcm,bool audio_passthrough)
{
    bresult rc=0;
    NEXUS_SyncChannelSettings nSyncChannelSettings;
    bsettop_init_settings initSettings;
    bsettop_get_init_settings(&initSettings);

    switch ( initSettings.lipsync_mode )
    {
    case bsettop_lipsync_mode_tsm:
    case bsettop_lipsync_mode_disabled:
        return b_ok;
    default:
        break;
    }

    BDBG_MSG(("Unset sync channel decode %p %s %s %s",decode,video?"video":"",audio_pcm?"pcm":"",audio_passthrough?"passthrough":""));

    /* sync channnel must be open */
    BDBG_ASSERT(decode->nSyncChannel);

    if(decode->nSyncChannel)
    {
        NEXUS_SyncChannel_GetSettings(decode->nSyncChannel,&nSyncChannelSettings);
        if(video)
            nSyncChannelSettings.videoInput = NULL;
        if(audio_pcm)
            nSyncChannelSettings.audioInput[0] = NULL;
        if(audio_passthrough)
            nSyncChannelSettings.audioInput[1] = NULL;
        rc = NEXUS_SyncChannel_SetSettings(decode->nSyncChannel,&nSyncChannelSettings);
        if (rc) return BSETTOP_ERROR(berr_external_error);

    }
    return rc;
}
#else
static bresult bdecode_p_open_sync_channel(bdecode_t decode)
{
    BSTD_UNUSED(decode);
    return 0;
}
static bresult bdecode_p_close_sync_channel(bdecode_t decode)
{
    BSTD_UNUSED(decode);
    return 0;
}
static bresult bdecode_p_set_sync_channel(bdecode_t decode, bool video,bool audio_pcm,bool audio_passthrough)
{
    BSTD_UNUSED(decode);
    BSTD_UNUSED(video);
    BSTD_UNUSED(audio_pcm);
    BSTD_UNUSED(audio_passthrough);
    return 0;
}
static bresult bdecode_p_unset_sync_channel(bdecode_t decode,bool video,bool audio_pcm,bool audio_passthrough)
{
    BSTD_UNUSED(decode);
    BSTD_UNUSED(video);
    BSTD_UNUSED(audio_pcm);
    BSTD_UNUSED(audio_passthrough);
    return 0;
}
#endif

#if NEXUS_HAS_ASTM
static bresult bdecode_p_open_astm(bdecode_t decode)
{
    NEXUS_Error rc =0;
    NEXUS_AstmSettings nAstmSettings;

    BDBG_MSG(("Open astm for decode %p",decode));
    NEXUS_Astm_GetDefaultSettings(&nAstmSettings);
    if (decode->stcChannel)
        nAstmSettings.stcChannel = decode->stcChannel;
    if (decode->video_decode && decode->video_decode->nVideoDecoder)
        nAstmSettings.videoDecoder = decode->video_decode->nVideoDecoder;
    if (decode->audio_decode)
    {
        if (decode->audio_decode->nAudioDecoder)
            nAstmSettings.audioDecoder[0]=decode->audio_decode->nAudioDecoder;
        if (g_audio_passthrough && g_audio_passthrough_owner == decode->audio_decode)
            nAstmSettings.audioDecoder[1]=g_audio_passthrough;
    }

    /* set STC master */
    if(nAstmSettings.videoDecoder && nAstmSettings.audioDecoder[0])
    {
        nAstmSettings.stcMaster = nAstmSettings.audioDecoder[0];
    }
    else if(nAstmSettings.videoDecoder)
    {
        nAstmSettings.stcMaster = nAstmSettings.videoDecoder;
    }
    else if(nAstmSettings.audioDecoder[0])
    {
        nAstmSettings.stcMaster = nAstmSettings.audioDecoder[0];
    }
    else if(nAstmSettings.audioDecoder[1])
    {
        nAstmSettings.stcMaster = nAstmSettings.audioDecoder[1];
    }


    decode->nAstm  =  NEXUS_Astm_Create(&nAstmSettings);
    if (!decode->nAstm) return BSETTOP_ERROR(berr_external_error);
    return rc;

}
static bresult bdecode_p_close_astm(bdecode_t decode)
{
    bresult rc=0;
    if (decode->nAstm)
    {
        NEXUS_Astm_Destroy(decode->nAstm);
        decode->nAstm=NULL;
    }
    return rc;
}

static bresult bdecode_p_set_astm(bdecode_t decode, bool video,bool audio_pcm,bool audio_passthrough)
{
    NEXUS_Error rc =0;
    NEXUS_AstmSettings nAstmSettings;

    if ( !bsettop_get_config("astm_enabled") )
    {
        BDBG_MSG(("Skipping astm configuration"));
        return b_ok;
    }

    /* astm channnel must be open */
    BDBG_ASSERT(decode->nAstm);

    BDBG_MSG(("Set astm decode %p %s %s %s",decode,video?"video":"",audio_pcm?"pcm":"",audio_passthrough?"passthrough":""));

    NEXUS_Astm_GetSettings(decode->nAstm,&nAstmSettings);

    if (video && decode->video_decode) /* setting video */
    {
        nAstmSettings.videoDecoder = decode->video_decode->nVideoDecoder;
    }

    if (audio_pcm) /* setting audio pcm channel */
    {
        if (decode->audio_decode)
        {
            /* if 0 is not pcm ... */
            if (nAstmSettings.audioDecoder[0] != decode->audio_decode->nAudioDecoder)
            {
                /* ... and it's not NULL ... */
                if (nAstmSettings.audioDecoder[0] != NULL)
                {
                    /* ... then it must be passthrough, so move passthrough to 1 because ... */
                    nAstmSettings.audioDecoder[1] = nAstmSettings.audioDecoder[0];
                }

                /* pcm is always 0 when set */
                nAstmSettings.audioDecoder[0] = decode->audio_decode->nAudioDecoder;
            }
        }
    }

    if (audio_passthrough) /* setting passthrough channel */
    {
        if (g_audio_passthrough && g_audio_passthrough_owner == decode->audio_decode)
        {
            /* if 0 is not passthrough ... */
            if (nAstmSettings.audioDecoder[0] != g_audio_passthrough)
            {
                /* ... and it's not NULL ... */
                if (nAstmSettings.audioDecoder[0] != NULL)
                {
                    /* ... then 0 must be pcm, so put passthrough at 1 */
                    nAstmSettings.audioDecoder[1] = g_audio_passthrough;
                }
                else
                {
                    /* ... otherwise put passthrough at 0 */
                    nAstmSettings.audioDecoder[0]=g_audio_passthrough;
                }
            }
        }
    }

    if(nAstmSettings.videoDecoder && nAstmSettings.audioDecoder[0])
    {
        BDBG_MSG(("Decode type is %d", decode->cfg_type));
        switch (decode->cfg_type)
        {
            case BDECODE_STC_CHANNEL_CFG_TYPE_PVR_PB:
                /* if a/v and playback, prefer VMM */
                nAstmSettings.stcMaster = nAstmSettings.videoDecoder;
                break;
            default:
                /* else prefer AMM */
        nAstmSettings.stcMaster = nAstmSettings.audioDecoder[0];
                break;
        }
    }
    else if(nAstmSettings.videoDecoder)
    {
        nAstmSettings.stcMaster = nAstmSettings.videoDecoder;
    }
    else if(nAstmSettings.audioDecoder[0])
    {
        nAstmSettings.stcMaster = nAstmSettings.audioDecoder[0];
    }
    else if(nAstmSettings.audioDecoder[1])
    {
        nAstmSettings.stcMaster = nAstmSettings.audioDecoder[1];
    }
    else
    {
        nAstmSettings.stcMaster = NULL;
    }

    nAstmSettings.enableAutomaticLifecycleControl = false;

    rc = NEXUS_Astm_SetSettings(decode->nAstm,&nAstmSettings);
    if (rc) return BSETTOP_ERROR(berr_external_error);
    return rc;
}

static bresult bdecode_p_unset_astm(bdecode_t decode,bool video,bool audio_pcm,bool audio_passthrough)
{
    bresult rc=0;
    NEXUS_AstmSettings nAstmSettings;

    if ( !bsettop_get_config("astm_enabled") )
    {
        return b_ok;
    }

    /* astm must be open */
    BDBG_ASSERT(decode->nAstm);
    BDBG_MSG(("Unset astm decode %p %s %s %s",decode,video?"video":"",audio_pcm?"pcm":"",audio_passthrough?"passthrough":""));

    if (decode->nAstm)
    {
        NEXUS_Astm_GetSettings(decode->nAstm,&nAstmSettings);
        if (video)
        {
            nAstmSettings.videoDecoder=NULL;
        }

        if (audio_pcm)
        {
            nAstmSettings.audioDecoder[0]=NULL;
            /* if pass is still set, move it down to 0 */
            if (nAstmSettings.audioDecoder[1])
            {
                nAstmSettings.audioDecoder[0] = nAstmSettings.audioDecoder[1];
                nAstmSettings.audioDecoder[1] = NULL;
            }
        }

        if (audio_passthrough)
        {
            if (nAstmSettings.audioDecoder[1])
            {
                /* pass at 1 */
            nAstmSettings.audioDecoder[1]=NULL;
            }
            else
            {
                /* pass at 0 */
                nAstmSettings.audioDecoder[0] = NULL;
            }
        }

        if (nAstmSettings.videoDecoder && nAstmSettings.audioDecoder[0])
        {
            BDBG_MSG(("Decode type is %d", decode->cfg_type));
            switch (decode->cfg_type)
            {
                case BDECODE_STC_CHANNEL_CFG_TYPE_PVR_PB:
                    /* if a/v and playback, prefer VMM */
                    nAstmSettings.stcMaster = nAstmSettings.videoDecoder;
                    break;
                default:
                    /* else prefer AMM */
            nAstmSettings.stcMaster = nAstmSettings.audioDecoder[0];
                    break;
            }
        }
        else if (nAstmSettings.videoDecoder)
        {
            nAstmSettings.stcMaster = nAstmSettings.videoDecoder;
        }
        else if (nAstmSettings.audioDecoder[0])
        {
            nAstmSettings.stcMaster = nAstmSettings.audioDecoder[0];
        }
        else if (nAstmSettings.audioDecoder[1])
        {
            nAstmSettings.stcMaster = nAstmSettings.audioDecoder[1];
        }
        else
        {
            nAstmSettings.stcMaster = NULL;
        }

        rc = NEXUS_Astm_SetSettings(decode->nAstm,&nAstmSettings);
        if (rc) return BSETTOP_ERROR(berr_external_error);

    }
    return rc;

}

static bresult bdecode_p_start_astm(bdecode_t decode)
{
    NEXUS_Error rc =0;

    if ( !bsettop_get_config("astm_enabled") )
    {
        BDBG_MSG(("Skipping astm start"));
        return b_ok;
    }

    /* astm channnel must be open */
    BDBG_ASSERT(decode->nAstm);

    BDBG_MSG(("Start astm decode %p",decode));

    rc = NEXUS_Astm_Start(decode->nAstm);
    if (rc) return BSETTOP_ERROR(berr_external_error);

    return rc;
}

static bresult bdecode_p_stop_astm(bdecode_t decode)
{
    bresult rc =0;

    if ( !bsettop_get_config("astm_enabled") )
    {
        BDBG_MSG(("Skipping astm stop"));
        return b_ok;
    }

    /* astm channnel must be open */
    BDBG_ASSERT(decode->nAstm);

    BDBG_MSG(("Stop astm decode %p",decode));

    NEXUS_Astm_Stop(decode->nAstm);

    return rc;
}

#else
static bresult bdecode_p_open_astm(bdecode_t decode)
{
     BSTD_UNUSED(decode);
    return 0;
}
static bresult bdecode_p_close_astm(bdecode_t decode)
{
    BSTD_UNUSED(decode);
    return 0;
}

static bresult bdecode_p_set_astm(bdecode_t decode, bool video,bool audio_pcm,bool audio_passthrough)
{
    BSTD_UNUSED(decode);
    BSTD_UNUSED(video);
    BSTD_UNUSED(audio_pcm);
    BSTD_UNUSED(audio_passthrough);
    return 0;
}
static bresult bdecode_p_unset_astm(bdecode_t decode,bool video,bool audio_pcm,bool audio_passthrough)
{
    BSTD_UNUSED(decode);
    BSTD_UNUSED(video);
    BSTD_UNUSED(audio_pcm);
    BSTD_UNUSED(audio_passthrough);
    return 0;
}

static bresult bdecode_p_start_astm(bdecode_t decode)
    {
     BSTD_UNUSED(decode);
    return 0;
}

static bresult bdecode_p_stop_astm(bdecode_t decode)
    {
    BSTD_UNUSED(decode);
    return 0;
}
#endif

static NEXUS_AudioDecoderDolbyDrcMode baudio_dolby_drc_mode_to_nexus_ac3plus_comp_mode(baudio_dolby_drc_mode drc_mode)
{
    NEXUS_AudioDecoderDolbyDrcMode eCompMode;

    switch (drc_mode)
    {
        case baudio_dolby_drc_mode_rf:

            eCompMode = NEXUS_AudioDecoderDolbyDrcMode_eRf;
            break;

        case baudio_dolby_drc_mode_line:
        default:

            eCompMode = NEXUS_AudioDecoderDolbyDrcMode_eLine;
            break;
    }

    return eCompMode;
}

static NEXUS_AudioDecoderDolbyStereoDownmixMode baudio_dolby_stereo_downmix_mode_to_nexus_ac3plus_stereo_mode(baudio_dolby_stereo_downmix_mode stereo_downmix_mode)
{
    NEXUS_AudioDecoderDolbyStereoDownmixMode eStereoMode;

    switch (stereo_downmix_mode)
    {
        case baudio_dolby_stereo_downmix_mode_automatic:

            eStereoMode = NEXUS_AudioDecoderDolbyStereoDownmixMode_eAutomatic;
            break;

        case baudio_dolby_stereo_downmix_mode_dolby_surround_compatible:

            eStereoMode = NEXUS_AudioDecoderDolbyStereoDownmixMode_eDolbySurroundCompatible;
            break;

        case baudio_dolby_stereo_downmix_mode_standard:
        default:

            eStereoMode = NEXUS_AudioDecoderDolbyStereoDownmixMode_eStandard;
            break;
    }

    return eStereoMode;
}
