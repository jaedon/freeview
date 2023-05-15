/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bsettop_impl.h $
 * $brcm_Revision: 157 $
 * $brcm_Date: 10/15/12 12:52p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/src/nexus/bsettop_impl.h $
 * 
 * 157   10/15/12 12:52p katrep
 * SW7445-1:add support for 7445
 * 
 * 156   8/20/12 4:51p chengs
 * SW7584-42: Add Brutus support for 7584.
 * 
 * 155   6/27/12 2:57p jtna
 * SW7425-3350: rework Brutus MTSIF frontend handling
 * 
 * 154   5/18/12 7:34p bselva
 * SW7360-19: Add support for 7360 platform
 * 
 * 153   5/2/12 6:46p jgarrett
 * SW7425-2802: Removing RAP_XXX build variables for audio post processing
 * in favor of runtime detection
 * 
 * 152   4/25/12 12:16p sgadara
 * SW7358-272: [7358] Support AVL on Brutus
 * 
 * 151   3/26/12 3:18p vle
 * SW7425-2515: Merge to mainline
 * 
 * SW7425-2515/1   3/23/12 6:19p vle
 * SW7425-2515: Update Brutus to use new NEXUS_Cec module. Remove all CEC
 * related code out of hdmi_output module
 * 
 * 150   2/22/12 1:19p jgarrett
 * SW7346-233: Clearing SPDIF/HDMI forced PCM state when audio decode is
 * stopped
 * 
 * 149   12/21/11 3:48p mward
 * SW7435-7:  Support Brutus build on (new, 7435-based) platform 97435.
 * 
 * 148   10/26/11 1:48p katrep
 * SW7429-1:add support for 7429
 * 
 * 147   9/19/11 3:57p randyjew
 * SW7344-134: Fix for pcr pid similiar to video/audio pid, open as video
 * or audio pic to avoid duplicate pid channel.
 * 
 * 146   9/18/11 4:41p agin
 * SWNOOS-485:  Support no-os.
 * 
 * 145   9/14/11 11:12a xhuang
 * SW7552-115: Add 7574 hardware RFM channel switch support
 * 
 * 144   8/29/11 12:22p agin
 * SWNOOS-481:  For UCOS_DIAGS, add sockaddr_in and in_addr.
 * 
 * 143   2/15/11 2:51p nickh
 * SW7422-213: Add SVC/MVC support
 * 
 * 142   2/15/11 7:31p xhuang
 * SW7358-34: Add 7358/7552 support
 * 
 * 141   12/15/10 10:41a erickson
 * SW7420-941: delete unused and incorrect BLST_Q logic. BLST_Q_REMOVE was
 * being called for a node that wasn't in a list.
 * 
 * 140   12/15/10 10:20a jrubio
 * SW7344-9: update for 7344/7346
 * 
 * 139   12/6/10 3:14p katrep
 * SW7231-4:inital support
 * 
 * 138   11/4/10 2:27p hongtaoz
 * SW7425-49: added 7425 support;
 * 
 * 137   10/20/10 4:35p nickh
 * SW7422-80: Add 7422 support
 * 
 * 136   7/14/10 12:01p erickson
 * SW7405-4621: change to nexus_base_os.h
 *
 * 135   6/10/10 4:57p erickson
 * SWDEPRECATED-2726: implement bsettop_fileio_fifo
 *
 * 134   3/19/10 5:59p jgarrett
 * SW7405-4040: Ensuring audio and video are on the same timebase
 *
 * 133   2/3/10 10:48a jgarrett
 * SW7405-3858: Handling recursive audio display disable calls
 *
 * 132   1/28/10 4:24p mward
 * SW7400-2650:  DTS passthru support for 7400.  Passthru only, no decode.
 *
 * 131   1/26/10 6:02p jgarrett
 * SW7405-3635: Allowing fixed audio decode->display mapping to avoid re-
 * attaching outputs on every bdecode_stop/bdecode_start
 *
 * 130   1/19/10 7:21p nitinb
 * SW7550-193: Merge to main branch
 *
 * SW7550-193/1   1/19/10 3:42p chengli
 * SW7550-193 : change #define B_N_SPDIF_OUTPUTS  1 to #define
 * B_N_SPDIF_OUTPUTS NEXUS_NUM_SPDIF_OUTPUTS
 *
 * 129   12/11/09 4:30p mphillip
 * SW7550-112: Merge 7550 changes to main branch
 *
 * 128   12/9/09 12:03p gmohile
 * SW7408-1 : Add 7408 support
 *
 * 127   12/4/09 9:53p randyjew
 * SW7468-6: Add 7468 support
 *
 * 126   12/3/09 2:45p jgarrett
 * SW7405-3511: Adding compressed mute for outputs
 *
 * 125   12/2/09 7:32p katrep
 * SW7405-3158:support for DTS certification app
 *
 * 124   11/19/09 8:02p jgarrett
 * SW7405-3357: Adding audio output routing for decode and pcm playback
 * prior to start
 *
 * 123   11/10/09 5:55p mward
 * SW7400-2574: HD/SD sharpness optimizations incorporated in VDC,
 * NEXUS_PictureCtrl_SetSharpnessValue() is no longer needed for simple
 * sharpness setting.
 *
 * 122   11/10/09 4:59p mward
 * SW7400-2574: When using the  NEXUS_PictureCtrl_SetSharpnessValue(), the
 * quantity of sharpness must be set using lumaControlGain and
 * chromaControlGain, the nPictureCtrlCommonSettings.sharpness is ignored
 * by BVDC_Window_SetSharpness.
 *
 * 121   11/5/09 10:40a gmohile
 * SW7408-1 : Add 7408 support
 *
 * SW7405-3158/1   10/7/09 7:03p mward
 * SW7405-3158: Remember audio decoder start settings for restart which
 * may be necessary changing DTS decoder settings. Extern audio decoder
 * structure for test command access.
 *
 * 120   9/23/09 5:36p ahulse
 * SW7405-2724: Switch MAD off when in playback pause to allow accurate
 * frame stepping
 *
 * 119   9/11/09 10:05a jtna
 * SWDEPRECATED-3910: add bdecode.primerStart
 *
 * 118   9/4/09 12:46p erickson
 * SW7405-2885: add more extern's
 *
 * 117   8/25/09 12:51p jtna
 * SWDEPRECATED-3910: video primer fast channel change feature
 *
 * 116   8/25/09 11:24a katrep
 * SW7405-2934:add suppoort for audio capture to a file
 *
 * 115   8/20/09 6:46p mward
 * PR55545: Adding 7125 platforms.
 *
 * 115   8/20/09 6:44p mward
 * PR55545: Adding 7125 platforms.
 *
 * 114   8/19/09 11:34a mphillip
 * PR57626: Add zorder support to shim
 *
 * 113   8/19/09 10:00a erickson
 * PR55994: add missing #include
 *
 * 112   8/18/09 6:38p katrep
 * PR56109: Use calllbacks instead of using events for callbacks to the
 * app.
 *
 * 111   8/14/09 3:11p erickson
 * PR55994: add missing #include
 *
 * 110   8/14/09 2:09p jrubio
 * PR55232: add 7340/7342
 *
 * 109   7/31/09 5:25p gmohile
 * PR 56512 : Add 7403 support
 *
 * 108   7/28/09 11:05a lwhite
 * PR55693: Added DTCP/IP intialization
 *
 * 107   6/15/09 4:58p jtna
 * PR43001: add support for TTS+encryption record/playback
 *
 * 106   6/5/09 2:06p vishk
 * PR 55748: (NEXUS) Clipping problem with bsettop Nexus shim
 *
 * 105   4/27/09 8:50a katrep
 * PR54586: astm support
 *
 * 104   4/15/09 3:39p jgarrett
 * PR 53551: Adding non-os define
 *
 * 103   3/20/09 1:09p katrep
 * PR52340: Support srs true volume and dolby volume through brutus ans
 * settop api
 *
 * 102   3/4/09 7:47p jgarrett
 * PR 52269: Adding XMP support
 *
 * 101   3/2/09 6:10p katrep
 * PR52340: Add support srs_true_volume
 *
 * 100   2/26/09 2:53p erickson
 * PR49584: add a dma-based memory bandwidth stress test feature
 *
 * 99   2/25/09 6:44p katrep
 * PR52099: For decoder trickmodes need to mute audio outside of nexus
 *
 * Trinity_Xmp_Support/1   2/24/09 11:30a prasadv
 * Add XMP support into user io. code cleanup
 *
 * 98   2/23/09 5:00p vle
 * PR48785: Add cec support to shim layer
 *
 * 97   2/4/09 10:23a jrubio
 * PR51629: add 7336 support
 *
 * 96   1/8/09 12:21p katrep
 * PR50063: Add suppport dor Ac3 encode in Settop shim and burtus.
 *
 * 95   12/24/08 12:39p jgarrett
 * PR 50703: Allowing for security module to be absesnt
 *
 * 94   12/16/08 6:58a erickson
 * PR50222: remove #if B_HAS_IP
 *
 * 93   12/14/08 6:58p nickh
 * PR50205: Adding 7420 support
 *
 * 92   11/6/08 8:38p katrep
 * PR47230: reworked output port timebase logic
 *
 * 91   10/31/08 12:30p jjordan
 * PR47230: Add TTS Pacing
 *
 * PR47230/1   10/28/08 10:37a jjordan
 * PR47230: Port TTS Pacing to Nexus
 *
 * 90   9/17/08 6:20p katrep
 * PR47061: Add support for DTS encode
 *
 * 89   9/15/08 5:31p jrubio
 * PR46925:  fix PVR_SUPPORT=n compile issue
 *
 * 88   9/11/08 12:38p shyam
 * PR46778 : Add support for AAC audio encode from I2S
 *
 * 87   9/2/08 7:44p katrep
 * PR46457: Configure settop at run time, based on resources exposed by
 * nexus platfrom.
 *
 * 86   8/22/08 7:49p erickson
 * PR45959: StillDecoder is now normal part of VideoDecoder module
 *
 * 85   7/28/08 10:04a erickson
 * PR43490: fix non-656/i2s platforms
 *
 * 84   7/22/08 2:59p erickson
 * PR44874: allow static allocation of bstream for playback. this is
 * required to support bstream_close after a bplayback_stop.
 *
 * 83   7/17/08 4:40p shyam
 * PR43426 : Add 7043 656 Encode support
 *
 * 82   7/16/08 11:19a erickson
 * PR44853: clean up -Wstrict-prototypes warnings
 *
 * 81   7/10/08 10:21p ssood
 * PR42739: Adding support to absorb high jitter for Live IP Playback
 *
 * 80   7/9/08 4:10p katrep
 * PR43490: Add support 656/i2s tuning using analog tune api.
 *
 * 79   7/3/08 6:09p erickson
 * PR36068: impl vesa
 *
 * 78   7/2/08 5:10p vishk
 * PR 40020: bsettop_stream (crypto, network decryption): Develop
 * SettopAPI-to-Nexus shim layer
 *
 * 77   7/2/08 12:33p erickson
 * PR36836: report has_index correctly
 *
 * 76   6/27/08 3:36p mphillip
 * PR42901: Free PVR keyslot on close/stop
 *
 * 75   6/27/08 1:41p erickson
 * PR44194: impl hold_last_picture in Settop API shim
 *
 * 74   6/27/08 9:30a erickson
 * PR43653: add vesa support
 *
 * 73   6/26/08 6:21p mphillip
 * PR42901: Encrypted PVR playback support
 *
 * 72   6/24/08 6:23p shyam
 * PR43426 : Add 7043 Transcode support
 *
 * 73   6/24/08 7:29p ssood
 * PR42739: Adding support to absorb high jitter for IPSTB
 *
 * 72   6/23/08 10:41a ssood
 * PR42739: Adding support to absorb high jitter for IPSTB
 *
 * 71   6/18/08 10:13p erickson
 * PR36068: add DTV panel support to shim (use component api)
 *
 * 70   6/17/08 3:44p erickson
 * PR43489: remove dups
 *
 * 69   6/11/08 7:26p ssood
 * PR42739: added virtual interface back to the playback module to allow
 * ip implementation in addition to the regular playback implementation
 *
 * 68   6/11/08 11:53a jgarrett
 * PR 43489: Adding brutus support for 93549/93556
 *
 * 67   5/29/08 8:48a erickson
 * PR39453: 3548/3556 updates
 *
 * 66   5/20/08 4:27p erickson
 * PR40034: support platforms with no smartcard
 *
 * 65   5/16/08 7:21p vishk
 * PR 40034: Develop Nexus Smartcard Interface/Module.
 * PR 40016: bsettop_smartcard: Develop SettopAPI-to-Nexus shim layer
 *
 * 64   5/6/08 11:36a erickson
 * PR42365: impl playback callbacks
 *
 * 63   5/1/08 6:03p jrubio
 * PR42353: add 7335 support
 * PR42353: add 7335 support
 *
 * 62   5/1/08 4:08p erickson
 * PR42339: properly connect and disconnect timeshifting
 *
 * 61   4/24/08 3:32p jrubio
 * PR42054: add sds2nexus
 *
 * 60   4/24/08 2:59p jgarrett
 * PR 42122: adding display handle to spdif
 *
 * 59   4/18/08 9:18p katrep
 * PR41950: Added support for bdecode_get_audio
 *
 * 58   4/13/08 9:58p katrep
 * PR40824: Apply the VDC settings after all the window settings are
 * programmed,updated debug.
 *
 * 57   4/10/08 6:43p jgarrett
 * PR 41565: Adding audio starting flag
 *
 * 56   4/10/08 10:07a erickson
 * PR36068: coverity fix
 *
 * 55   4/9/08 5:54p jgarrett
 * PR 41567: Adding playback video pid
 *
 * 54   4/9/08 9:16a erickson
 * PR36068: fix 93563
 *
 * 53   4/8/08 3:56p katrep
 * PR41122: Add support more display menu items
 *
 * 52   4/8/08 11:21a jgarrett
 * PR 40024: Changing volume function prototype
 *
 * 51   4/7/08 5:02p erickson
 * PR40024: impl bdecode_display_surface
 *
 * 50   4/7/08 4:11p erickson
 * PR40024: impl bdecode_read_metadata
 *
 * 49   4/7/08 1:35p erickson
 * PR40024: impl bdecode_set_audio_volume
 *
 * 48   4/7/08 10:40a jgarrett
 * PR 41362: Revising pid channel management
 *
 * 47   4/4/08 10:43a jgarrett
 * PR 41312: Synchronizing PVR
 *
 * 46   4/3/08 5:41p jgarrett
 * PR 41312: Setting callback events
 *
 * 45   4/2/08 5:06p jgarrett
 * PR 41233: Adding timer
 *
 * 44   4/2/08 3:50p jrubio
 * PR41191: add uninit function for decode
 *
 * 43   4/1/08 6:02p katrep
 * PR41191: Add various input/output shutdowns
 *
 * 42   3/31/08 6:04p jgarrett
 * PR 40606: Adding display handle to hdmi
 *
 * 41   3/27/08 5:30p jrubio
 * PR40919: add userio init and uninit calls
 *
 * 40   3/25/08 7:43p vsilyaev
 * PR 40862: Use nexus configuration to set supported formats
 *
 * 39   3/25/08 12:52p erickson
 * PR36068: added b_nexus2framerate
 *
 * 38   3/25/08 11:01a erickson
 * PR36068: fix
 *
 * 37   3/25/08 10:55a katrep
 * PR40839: Add support for HDMI capabilities
 *
 * 36   3/25/08 10:33a erickson
 * PR36068: added analog tuner
 *
 * 35   3/25/08 10:13a jgarrett
 * PR 40857: Adding thunk layer
 *
 * 34   3/21/08 8:39p katrep
 * PR40824: Implemented support for PIP swapping.
 *
 * 33   3/19/08 9:23p erickson
 * PR36068: convert bgraphics.surfaces to lst
 *
 * 32   3/19/08 3:10p jrubio
 * PR40015: add hdmi video/audio support
 *
 * 31   3/19/08 1:23p erickson
 * PR36068: fix non-StillDecoder platforms
 *
 * 30   3/18/08 4:03p erickson
 * PR40307: implemented still decode
 *
 * 29   3/17/08 5:40p katrep
 * PR40021:Proper indexing of tuners required to work with the bconfig
 * interface
 *
 * 28   3/12/08 3:17p jgarrett
 * PR 40017: Adding PCM support
 *
 * 27   3/11/08 4:58p jrubio
 * PR40019: add pcr_status variable to bstream
 *
 * 26   3/11/08 11:10a erickson
 * PR36068: impl vbi
 *
 * 25   3/10/08 5:51p erickson
 * PR36068: added boutput_rf
 *
 * 24   3/10/08 4:55p katrep
 * PR40019: Move the header files to bsettop_impl.h
 *
 * 23   3/6/08 4:23p jrubio
 * PR40169: add 7325
 *
 * 22   3/4/08 5:54p jrubio
 * PR40169: add 7325
 *
 * 21   2/25/08 9:23a erickson
 * PR36068: impl more functions
 *
 * 20   1/2/08 9:55a erickson
 * PR36068: Playback changes
 *
 * 19   12/20/07 10:28a erickson
 * PR37590: add timeshifting
 *
 * 18   12/4/07 3:09p erickson
 * PR36068: call NEXUS_PidChannel_Close
 *
 * 17   12/3/07 4:59p erickson
 * PR36068: rework audio, defer playback start until after decode start
 *
 * 16   11/27/07 11:16p erickson
 * PR36068: added g_settopModule to allow use of nexus timers
 *
 * 15   11/27/07 9:19a erickson
 * PR37590: add 93563
 *
 * 14   11/19/07 1:21p erickson
 * PR34925: rename demux to pid_channel
 *
 * 13   11/16/07 4:08p erickson
 * PR36068: added #include
 *
 * 12   11/14/07 1:29p erickson
 * PR36068: added record
 *
 * 11   11/13/07 11:58a erickson
 * PR36068: trick modes working
 *
 * 10   11/13/07 10:25a erickson
 * PR36068: check if video/audio started when stopping
 *
 * 9   11/12/07 3:57p erickson
 * PR36068: update
 *
 * 8   11/12/07 2:34p erickson
 * PR36068: update
 *
 * 7   11/2/07 1:25p erickson
 * PR36633: update nexus enums
 *
 * 6   10/19/07 12:26p erickson
 * PR36068: added blitter
 *
 * 5   10/17/07 9:45a erickson
 * PR36068: there should be one bsurface_settings for framebuffers
 *
 * 4   10/16/07 2:30p erickson
 * PR36068: added refcnt to parser_band for message filtering
 *
 * 3   10/16/07 12:35p erickson
 * PR36068: brutus up over settop api/nexus
 *
 * 2   10/15/07 4:13p erickson
 * PR36068: initial
 *
 * 1   10/15/07 2:36p erickson
 * PR36068: initial
 *
 ***************************************************************************/
#include "bstd.h"
#include "bkni.h"
#include "blst_list.h"
#include "blst_queue.h"
#ifndef BSETTOP_NO_API_THUNKS
#include "bsettop_autogen_thunks.h"
#endif
#include "bsettop.h"
#include "bsettop_os.h"

#if BCHP_CHIP == 7401
#include "bsettop_bsp_7401.h"
#elif BCHP_CHIP == 7400
#include "bsettop_bsp_7400.h"
#elif BCHP_CHIP == 7405
#include "bsettop_bsp_7405.h"
#elif BCHP_CHIP == 7550
#include "bsettop_bsp_7550.h"
#elif BCHP_CHIP == 3563
#include "bsettop_bsp_3563.h"
#elif BCHP_CHIP == 3548
#include "bsettop_bsp_3548.h"
#elif BCHP_CHIP == 3556
#include "bsettop_bsp_3556.h"
#elif BCHP_CHIP == 7325
#include "bsettop_bsp_7325.h"
#elif BCHP_CHIP == 7335 || (BCHP_CHIP==7336)
#include "bsettop_bsp_7335.h"
#elif BCHP_CHIP == 7420
#include "bsettop_bsp_7420.h"
#elif BCHP_CHIP == 7403
#include "bsettop_bsp_7403.h"
#elif BCHP_CHIP == 7342
#include "bsettop_bsp_7342.h"
#elif BCHP_CHIP == 7340
#include "bsettop_bsp_7340.h"
#elif BCHP_CHIP == 7125
#include "bsettop_bsp_7125.h"
#elif BCHP_CHIP == 7408
#include "bsettop_bsp_7408.h"
#elif BCHP_CHIP == 7468
#include "bsettop_bsp_7468.h"
#elif BCHP_CHIP == 7422
#include "bsettop_bsp_7422.h"
#elif BCHP_CHIP == 7344
#include "bsettop_bsp_7344.h"
#elif BCHP_CHIP == 7346
#include "bsettop_bsp_7346.h"
#elif BCHP_CHIP == 7358
#include "bsettop_bsp_7358.h"
#elif BCHP_CHIP == 7360
#include "bsettop_bsp_7360.h"
#elif BCHP_CHIP == 7552
#include "bsettop_bsp_7552.h"
#elif BCHP_CHIP == 7425
#include "bsettop_bsp_7425.h"
#elif BCHP_CHIP == 7231
#include "bsettop_bsp_7231.h"
#elif BCHP_CHIP == 7429
#include "bsettop_bsp_7429.h"
#elif BCHP_CHIP == 7435
#include "bsettop_bsp_7435.h"
#elif BCHP_CHIP == 7445
#include "bsettop_bsp_7445.h"
#elif BCHP_CHIP == 7584
#include "bsettop_bsp_7584.h"
#endif
#include "bsettop_fileio_fifo.h"

#include "nexus_platform.h"
#include "nexus_video_decoder.h"
#include "nexus_video_image_input.h"
#include "nexus_still_decoder.h"
#if B_HAS_ANALOG
#include "nexus_analog_video_decoder.h"
#endif
#include "nexus_audio_playback.h"
#include "nexus_audio_decoder.h"
#if NEXUS_NUM_AUDIO_CAPTURES
#include "nexus_audio_capture.h"
#endif
#include "nexus_display.h"
#if NEXUS_HAS_DMA
#include "nexus_dma.h"
#endif
#include "nexus_video_window.h"
#include "nexus_component_output.h"
#include "nexus_composite_output.h"
#include "nexus_svideo_output.h"
#include "nexus_surface.h"
#include "nexus_pid_channel.h"
#include "nexus_playpump.h"
#if NEXUS_HAS_PLAYBACK
#include "nexus_playback.h"
#endif
#include "nexus_message.h"
#include "nexus_stc_channel.h"
#include "nexus_timebase.h"
#include "nexus_audio_output.h"
#include "nexus_audio_mixer.h"
#include "nexus_audio_dac.h"
#include "nexus_spdif_output.h"
#include "nexus_i2s_output.h"
#include "nexus_recpump.h"
#include "nexus_audio_decoder_trick.h"
#if NEXUS_HAS_RECORD
#include "nexus_record.h"
#include "nexus_file_fifo.h"
#endif
#include "nexus_parser_band.h"
#include "nexus_video_input.h"
#include "nexus_audio_input.h"
#include "nexus_display_vbi.h"
#include "nexus_video_adj.h"
#include "nexus_picture_ctrl.h"
#if NEXUS_HAS_FRONTEND
#include "nexus_frontend.h"
#endif
#if B_HAS_EXTERNAL_ANALOG
#include "nexus_ccir656_input.h"
#include "nexus_i2s_input.h"
#endif
#include "nexus_video_input_crc.h"
#if B_HAS_TRANSCODE
#include "nexus_transcoder.h"
#endif
#if NEXUS_HAS_SMARTCARD
#include "nexus_smartcard.h"
#endif
#if NEXUS_HAS_SYNC_CHANNEL
#include "nexus_sync_channel.h"
#endif
#if NEXUS_HAS_ASTM
#include "nexus_astm.h"
#endif
#if B_N_DVI_OUTPUTS
#include "nexus_hdmi_output.h"
#endif
#if B_N_RF_OUTPUTS
#include "nexus_rfm.h"
#endif

#include "nexus_dts_encode.h"
#include "nexus_ac3_encode.h"
#include "nexus_tru_volume.h"
#include "nexus_dolby_volume.h"
#include "nexus_auto_volume_level.h"

#if NEXUS_HAS_SECURITY
#include "nexus_security.h"
#endif

#if defined(NO_OS_DIAGS) || defined(UCOS_DIAGS)
struct   in_addr {
    union   {
         struct{
             unsigned  char   s_b1,
                              s_b2,
                              s_b3,
                              s_b4;
        }  S_un_b;
             struct  {
             unsigned  short  s_w1,
                              s_w2;
              }  S_un_w;
               unsigned long  S_addr;
     } S_un;
};

struct sockaddr_in{
   short sin_family;
   unsigned short sin_port;
   struct in_addr sin_addr;
   char sin_zero[8];
};
#endif

#ifndef _WIN32_WCE
   #if !defined B_SYSTEM_linuxkernel && !defined B_SYSTEM_noos
     #include <sys/socket.h>
     #include <netinet/in.h>
     #include <arpa/inet.h>
   #endif
   #ifdef __vxworks
     #include <sockLib.h>
     #include <sysLib.h>
#ifndef VXWORKS6
     typedef int socklen_t;
#endif
       #endif
#else
       #include <winsock2.h>
       #include <ws2tcpip.h>
#endif

#define BSETTOP_ERROR(err) (BDBG_ERR(("error: %d at %s: %d", err, __FILE__, __LINE__)), err)
#define B_ID_GET_INDEX(id) (((int)(id))-1)

#define B_N_SPDIF_OUTPUTS NEXUS_NUM_SPDIF_OUTPUTS
#if BCHP_CHIP == 7325
#define B_PVR_N_PLAYBACKS 2
#else
#define B_PVR_N_PLAYBACKS 4
#endif

#define BSETTOP_MAX_PIDS (BSETTOP_MAX_PROGRAMS*3+1) /* video, audio, other + pcr_pid */


struct bvideo_decode {
    NEXUS_VideoDecoderHandle nVideoDecoder;
    NEXUS_VideoImageInputHandle nVideoImageInput;
    NEXUS_PidChannelHandle nPidChannel;
#if B_HAS_SVC_MVC
    NEXUS_PidChannelHandle enhancementPidChannel;
#endif
#if B_HAS_ANALOG
    NEXUS_AnalogVideoDecoderHandle nAnalogVideoDecoder;
#endif

#if B_HAS_EXTERNAL_ANALOG
    NEXUS_Ccir656InputHandle ccir656Input;
#endif

    bdecode_t decode;
    bool started;
    /* B_EventHandle sourceChangedEvent;
    b_event_id_t sourceChangedEventId; */
    b_calllback_id_t sourceChangedCallback;

};

struct baudio_decode {
    NEXUS_AudioDecoderHandle nAudioDecoder;
    NEXUS_PidChannelHandle nPidChannel;
    NEXUS_PidChannelHandle nSecondaryPidChannel;
    bdecode_t decode;
    bdisplay_t display;
    bool started;
    bool busy;
    /* B_EventHandle sourceChangedEvent;
    b_event_id_t sourceChangedEventId; */
    NEXUS_AudioDecoderStartSettings decoderStartSettings;
    b_calllback_id_t sourceChangedCallback;
    struct baudio_decode *secondary_audio_decode;    /* for secondary decoder this will be NULL */
    NEXUS_AudioDecoderStartSettings restartSettings;
#if B_HAS_EXTERNAL_ANALOG
    NEXUS_I2sInputHandle i2sInput;
#endif
    NEXUS_DtsEncodeHandle nDtsEncoder;
    NEXUS_Ac3EncodeHandle nAc3Encoder;
    NEXUS_TruVolumeHandle nTrueVolume;
    NEXUS_DolbyVolumeHandle nDolbyVolume;
    NEXUS_AutoVolumeLevelHandle nAutoVolumeLevel;
#if NEXUS_NUM_AUDIO_CAPTURES
    NEXUS_AudioCaptureHandle nAudioCapture;
#endif
#if (BCHP_CHIP==7400)
    bool spdif_forced_compressed;
    bool hdmi_forced_compressed;
#endif
    bool spdif_forced_pcm;
    bool hdmi_forced_pcm;
};

typedef enum {
    BDECODE_STC_CHANNEL_CFG_TYPE_PARSER_BAND,
    BDECODE_STC_CHANNEL_CFG_TYPE_TTS_PACING,
    BDECODE_STC_CHANNEL_CFG_TYPE_PCR_PACING,
    BDECODE_STC_CHANNEL_CFG_TYPE_IP_LIVE_PB,
    BDECODE_STC_CHANNEL_CFG_TYPE_PVR_PB
} bdecode_stc_channel_cfg_type;

struct bdecode {
    bvideo_decode_t video_decode; /* attached = !NULL */
    baudio_decode_t audio_decode; /* attached = !NULL */
    NEXUS_StcChannelHandle stcChannel;
    bdecode_stc_channel_cfg_type cfg_type;
    NEXUS_Timebase output_timebase;         /* timebase for outputs connected to this decoder */
    NEXUS_PidChannelHandle pcrPidChannel;
    bdecode_settings settings;

    /* still decode */
    NEXUS_StillDecoderHandle nStillDecoder;
    NEXUS_PidChannelHandle nStillDecoderPid;
    bstream_t stillStream;
    /*B_EventHandle stillEvent;
    b_event_id_t stillEventId;*/
    b_calllback_id_t stillEventCallback;

    unsigned video_program, audio_program;
    bool started;
    bool image_input_started;
    bool tsm;
    bool audio_disabled;
    bdecode_window_t window; /* current decode */
    bstream_t source; /* current decode */
    baudio_volume volume;
    NEXUS_VideoDecoderStartSettings primerStart;
#if NEXUS_HAS_SYNC_CHANNEL
    NEXUS_SyncChannelHandle nSyncChannel;    /* Nexus Sync channel associated with each decode */
#endif
#if NEXUS_HAS_ASTM
    NEXUS_AstmHandle nAstm;
#endif

#if B_DECODE_CRC_CAPTURE || B_DECODE_USERDATA_CAPTURE
    struct {
/* for a convenient impl, make B_DECODE_CRC_CAPTURE_SIZE a multiple of the crc atom (which is 20 bytes) */
#define B_DECODE_METADATA_CAPTURE_SIZE 8000
        uint32_t data[B_DECODE_METADATA_CAPTURE_SIZE];
        unsigned wptr, rptr;
    } metadata;
#endif
};

struct btuner_linein {
    /* can be only one at a time */
#if B_HAS_ANALOG
    NEXUS_CompositeInputHandle composite;
    NEXUS_ComponentInputHandle component;
    NEXUS_SvideoInputHandle svideo;
#else
    int dummy;
#endif

#ifdef B_HAS_TRANSCODE
    unsigned int vdec7043DeviceNo ;
#endif

};



#if B_HAS_TRANSCODE
struct btranscode
{
    NEXUS_Transcoder_OutputPortHandle hPortHandle ;
    NEXUS_TranscoderHandle nexus_transcoder ;
    btranscode_settings settings ;
    NEXUS_RemuxHandle rmx[B_MAX_REMUXES];

} ;

struct bencode
{
    NEXUS_Transcoder_OutputPortHandle hPortHandle ;
    NEXUS_TranscoderHandle nexus_encoder ;
    NEXUS_TranscoderHandle nexus_audEncoder ;
    bencode_settings settings ;
} ;

#endif

struct boutput_rf {
#if B_N_RF_OUTPUTS
    NEXUS_RfmHandle handle;
    NEXUS_GpioHandle pin;
#endif
    bdisplay_t master_display;
    boutput_rf_settings desired, cfg;
};

struct boutput_hdmi {
#if B_N_DVI_OUTPUTS
    NEXUS_HdmiOutputHandle handle;
#endif
#if NEXUS_HAS_CEC
    NEXUS_CecHandle cecHandle;
    bool deviceReady;
    bool messageReceived;   
#endif
    bool connected;
    bool compressed_mute;
    bdisplay_t display;
    boutput_hdmi_settings desired, cfg;
    b_timer_t hdcpTimer;
    b_calllback_id_t hotplugCallback;
    b_calllback_id_t disconnectCalllback;
    b_calllback_id_t hdcpCallback;
    b_calllback_id_t cecTransmittedCallback;
    b_calllback_id_t cecReceivedCallback;
    b_calllback_id_t cecDeviceReadyCallback;
    B_EventHandle event;
    b_event_id_t eventId;
    B_EventHandle hdcpEvent;
    b_event_id_t hdcpEventId;
};

struct boutput_svideo {
    NEXUS_SvideoOutputHandle handle;
};

struct boutput_component {
    NEXUS_ComponentOutputHandle handle;
    boutput_component_settings desired, cfg;
    NEXUS_VideoOutput videoOutput;
#if NEXUS_NUM_PANEL_OUTPUTS
    NEXUS_PanelOutputHandle panelHandle;
#endif
};

struct boutput_composite {
    NEXUS_CompositeOutputHandle handle;
};

struct boutput_spdif {
    NEXUS_SpdifOutputHandle handle;
    bdisplay_t display;
    boutput_spdif_settings desired, cfg;
    baudio_volume volume;
    bool compressed_mute;
};

struct boutput_audio_dac {
    NEXUS_AudioDacHandle handle;
};

struct boutput_i2s {
    NEXUS_I2sOutputHandle handle;
};

struct bdecode_window {
    bdecode_window_settings settings;
    NEXUS_VideoWindowHandle nWindow;
    NEXUS_VideoInput currentVideoInput; /* which input is currently connected */
    bdisplay_t display;
    bdecode_window_t parent; /* clone parent */
    bdecode_window_t clone; /* clone child */
    bdecode_t decode;
    bool playbackDeinterlacer;  /* true to turn off deinterlacer in playback pause */
    int16_t sharpness;
};

struct zorder_item {
    bdecode_window_t window;
    unsigned zorder;
};

BDBG_OBJECT_ID_DECLARE(bpcm_play);
typedef struct bpcm_play
{
    BDBG_OBJECT(bpcm_play)
    bool opened;
    bool disabled;
    NEXUS_AudioPlaybackHandle nPlayback;
    NEXUS_AudioPlaybackStartSettings nSettings;
    bdisplay_t display;
    B_EventHandle event;
    b_event_id_t eventId;
    bsettop_callback callback;
    void *context;
    bpcm_play_output_settings output_settings;
} bpcm_play;

BDBG_OBJECT_ID_DECLARE(bdisplay);
struct bdisplay {
    BDBG_OBJECT(bdisplay)
    unsigned index;
    bgraphics_t graphics;
    bdisplay_settings settings;
    struct bdecode_window window[B_N_WINDOWS];
    unsigned  numWindowsSupported;

    baudio_volume volume; /* DAC */
    bool cgms_enabled, wss_enabled;

    NEXUS_DisplayHandle nDisplay;
    NEXUS_AudioMixerHandle nAudioMixer;
    unsigned mixerInputsStarted;
    unsigned audioDisableCount;
    struct {
        NEXUS_AudioDacHandle handle;
    } audioDac;
    /* mapping from virtual to physical (slot number) z-order */
    struct zorder_item zorderList[B_N_WINDOWS]; /* number of windows */
    bdecode_t masterDecode;
    unsigned outputTimebase;
};

/* bparser_band is an internal structure */
struct bparser_band {
    bband_t band; /* current input band mapped to this parser band */
    int refcnt; /* number of bstream_t's open on this parser band. */

    NEXUS_ParserBand nParserBand;
};

/*
* Maintain Conditional Access state inside the stream, necessary with HSM-related changes.
* See bstream_set_encryption for the use of these variables.
*/
typedef struct b_ca_state {
    uint16_t video_odd_key_config;
    uint16_t video_even_key_config;
    uint16_t audio_odd_key_config;
    uint16_t audio_even_key_config;
} b_ca_state;

typedef struct b_ca_state *b_ca_state_t;

BDBG_OBJECT_ID_DECLARE(bstream);
struct bstream {
    BDBG_OBJECT(bstream)
    struct {
        /* should be only one at a time */
        bband_t band; /* live digital */
        btuner_t tuner; /* live analog */
        btuner_linein_t linein; /* live linein analog */
        bplaypump_t playpump; /* pvr */
        bplayback_t playback; /* pvr */
#ifdef B_HAS_IP
        bool is_playback_ip; /* true if stream is IP playback, false otherwise */
        bool use_live_playback_mode; /* live playback operates like a live decode, but uses playback to send data to the decoders */
        bool timestamp_active; /* use timestamps to dynamically control playback rate (pacing) */
        bool timestamp_enabled;
#endif
    } producer;
    struct {
        /* can be many at the same time */
        bdecode_t decode, still;
        brecpump_t recpump;
        brecord_t record;
        /* TODO: Remux? */
    } consumers;
    b_ca_state ca_state;
    bstream_pcr_status pcr_status;
    bstream_mpeg mpeg;
    struct bparser_band *parser_band;
    bstream_vbi_settings vbi_settings;
    b_timer_t cc_data_ready_timer;
    NEXUS_PidChannelHandle nIndexPid; /* must open indexed video pid before starting playback */
    NEXUS_PidChannelHandle pcrPidChannel; /* if pcr pid is same as video or audio, we must open it as video or audio to avoid the duplicate pid channel */
    bool static_alloc; /* set true if bstream_close should not dealloc the bstream */
    bool video_primer;
    struct {
        uint16_t pid;
        unsigned pidchannel;
        NEXUS_KeySlotHandle KeySlotHandle;
    } pids[BSETTOP_MAX_PIDS];
};


struct bband {
    NEXUS_InputBand nInputBand;
    bstream_mpeg_type mpeg_type;
    btuner_t tuner;
    NEXUS_FrontendHandle nFrontend; /* parserband input can come from host inputband, or demod inputband over MTSIF */
};
#if NEXUS_NUM_INPUT_BANDS
extern struct bband g_bands[B_MAX_INPUTS];
#endif

BDBG_OBJECT_ID_DECLARE(bmessage);
struct bmessage_struct {
    BDBG_OBJECT(bmessage)
    NEXUS_MessageHandle nMessage;
    NEXUS_PidChannelHandle nPidChannel;
    bmessage_stream_params params;
    bmessage_format format;
    bstream_t stream;
    /*B_EventHandle dataEvent;
    b_event_id_t dataEventId;*/
    b_calllback_id_t dataCallback;
    BLST_Q_ENTRY(bmessage_struct) streamNode;
};

#if NEXUS_HAS_DMA
struct bdma {
    NEXUS_DmaHandle hDma;
};
extern struct bdma g_dma;
#endif

BDBG_OBJECT_ID_DECLARE(bplaypump);

struct bplaypump {
    BDBG_OBJECT(bplaypump)
    NEXUS_PlaypumpHandle nPlaypump;
    unsigned index;
    bplaypump_params params;
    bplaypump_settings settings;
    bstream_t stream;
    bplaypump_state state;
    bool forward;
    unsigned rate;
    B_EventHandle event;
    b_event_id_t eventId;

    NEXUS_KeySlotHandle hKeySlot;
};


#if NEXUS_HAS_PLAYBACK
/* VMT (interface) table for a playback implementations, this interface shall be first member of the playback context(state) */
typedef struct {
    /* bool synchronized;  */ /* if true interface would call implmentation within b_lock/b_unlock brackets */
    /* documentation for methods could be found in the bsettop_pvr.h file */
    void (*params_init)(bplayback_params *params, bplayback_t playback);
    bstream_t (*start)(bplayback_t playback, bplaypump_t playpump, const bstream_mpeg *mpeg, bplayback_file_t source, const bplayback_params *params);
    bstream_t (*start_timeshifting)(bplayback_t playback, bplaypump_t playpump, const bstream_mpeg *mpeg, bplayback_file_t source,  const bplayback_params *params, brecord_t record );
    bresult (*stop_timeshifting)(bplayback_t playback);
    bresult (*stop)(bplayback_t playback);
    bresult (*get_status)(bplayback_t p, bplayback_status *status);
    bresult (*pause)(bplayback_t p);
    bresult (*catchup_record)(bplayback_t p);
    bresult (*play)(bplayback_t p);
    bresult (*frameadvance)(bplayback_t p, bplayback_player_mode playermode,    bool forward);
    bresult (*trickmode)(bplayback_t p, const bplayback_trickmode_params *trickmode);
    bresult (*get_trickmode_by_rate)(bplayback_t p, int rate, bplayback_trickmode_params *trickmode);
    long (*goto_index)(bplayback_t p, long index_offset, int whence);
    long (*goto_timestamp)(bplayback_t p, long timestamp, int whence);
    off_t (*goto_mpeg_file_offset)(bplayback_t p, off_t mpeg_file_offset, int whence);
    void (*close)(bplayback_t p);
}bplayback_iface;

/* typedef struct bplayback_iface bplayback_iface_t;*/
void bplayback_iface_init(bplayback_iface *iface);

struct bplayback {
    bplayback_iface iface; /* bplayback_iface shall be the first member in the structure */
    NEXUS_PlaybackHandle nPlayback;
    bplayback_trickmode_params trick_mode_params;

    bplayback_params params;
    bstream_t stream;    /* this is set if playback is started */
    bstream_t stream_alloc; /* this is set at open time managing the "static" alloc of bplayback's bstream */
    bplayback_file_t source;
    brecord_t record;

    B_EventHandle end_of_stream_event, beginning_of_stream_event, error_event;
    b_event_id_t end_of_stream_event_id, beginning_of_stream_event_id, error_event_id;

    NEXUS_KeySlotHandle hKeySlot;
};

struct bplayback_file {
    NEXUS_FilePlayHandle nFile;
    bool has_index;
    struct {
        int                     fd;
        struct sockaddr_in      local_addr;
        struct sockaddr_in      remote_addr;
        struct hostent *        he;
        socklen_t               addr_len;
    } socket;
};
#endif

BDBG_OBJECT_ID_DECLARE(brecpump);

struct brecpump {
    BDBG_OBJECT(brecpump)
    NEXUS_RecpumpHandle nRecpump;
    bstream_t stream;
    brecpump_params params;
    B_EventHandle dataEvent, indexEvent, overflowEvent;
    b_event_id_t dataEventId, indexEventId, overflowEventId;
    NEXUS_PidChannelHandle videoPid[BSETTOP_MAX_PROGRAMS];
    NEXUS_PidChannelHandle audioPid[BSETTOP_MAX_PROGRAMS];
    NEXUS_PidChannelHandle ancillaryPid[BSETTOP_MAX_PROGRAMS];
    NEXUS_PidChannelHandle pcrPid;
    NEXUS_KeySlotHandle hKeySlot;
};

#if NEXUS_HAS_RECORD
struct brecord {
    NEXUS_RecordHandle nRecord;
    bplayback_t playback; /* cross linking */
    bool started;
    brecord_params params;
    bstream_t stream;
    int errorcode;
    B_EventHandle errorEvent, overflowEvent;
    b_event_id_t errorEventId, overflowEventId;

    NEXUS_PidChannelHandle videoPid[BSETTOP_MAX_PROGRAMS];
    NEXUS_PidChannelHandle audioPid[BSETTOP_MAX_PROGRAMS];
    NEXUS_PidChannelHandle ancillaryPid[BSETTOP_MAX_PROGRAMS];
    NEXUS_PidChannelHandle pcrPid;
    NEXUS_KeySlotHandle hKeySlot;
};

struct brecord_file {
    NEXUS_FileRecordHandle nFile;
    NEXUS_FifoRecordHandle nFileFifo;
};

#endif

/* 7400 & 7440 can use 2, otherwise default to 1.
set to 0 to test with cpu-only graphics. */
#ifndef B_N_GRC
#if B_HAS_SOFT_GFX
#define B_N_GRC 0
#else
#define B_N_GRC 1
#endif /* B_HAS_SOFT_GFX */
#endif /* B_N_GRC */

BDBG_OBJECT_ID_DECLARE(bgraphics);
BDBG_OBJECT_ID_DECLARE(bsurface);
BDBG_OBJECT_ID_DECLARE(bstream);

struct bsurface {
    BDBG_OBJECT(bsurface)

    BLST_Q_ENTRY(bsurface) link; /* list of surfaces in bgraphics */
    bool in_list; /* true if this bsurface was added to bgraphics.surfaces */

    NEXUS_SurfaceHandle nSurface;
    bsurface_memory mem;
    bsurface_settings settings;
    bsurface_create_settings create_settings;
    bgraphics_t graphics;
    bool is_fb; /* if true, this surface is in graphics->fb[] */
};

#define MAX_GRAPHICS_FBS 3

#if B_N_GRC
#include "nexus_graphics2d.h"
#endif

#if B_HAS_SOFT_GFX
#include "b_softgfx_lib.h"
#endif /* B_HAS_SOFT_GFX */

struct bgraphics {
    BDBG_OBJECT(bgraphics)
    unsigned index;
    bdisplay_t display;
    bool closing;
    bsurface_t fb[MAX_GRAPHICS_FBS];
    unsigned num_fbs; /* total in fb[] */
    unsigned cur_fb; /* index into fb[] for the currently displayed framebuffer. get_memory/blit/fill all operate on the next framebuffer. */
    bgraphics_settings settings;
    BLST_Q_HEAD(surfacelist, bsurface) surfaces; /* non-fb surfaces */
#if B_N_GRC
    NEXUS_Graphics2DHandle graphics2d;
    BKNI_EventHandle gEvent;
#endif
};

void bstream_p_init(void);
void bdisplay_p_init(void);
void bdisplay_p_uninit(void);
void buserio_p_init(void);
void buserio_p_uninit(void);
void bdecode_p_init(void);
void bdecode_p_uninit(void);
void bconfig_p_init(void);
bresult b_event_init(void);
void btuner_p_init(void);
void bconfig_p_set_tuners(void);
void bdma_p_init(void);
void bdma_p_uninit(void);
void bdma_p_mem_stress(void);
#ifdef B_HAS_IP
void bplayback_ip_p_init(void);
void bplayback_ip_p_uninit(void);
#endif

/* open a live or playback parser band */
bstream_t bstream_p_open(bband_t band, bplaypump_t playpump, bplayback_t playback, unsigned parser_band, const bstream_mpeg *mpeg);
bresult bstream_p_set(bstream_t stream, const bstream_mpeg *mpeg);
bstream_t bstream_p_open_message(bband_t band, const bstream_mpeg *mpeg);
bresult bplayback_p_start(bplayback_t playback);
brecord_file_t brecord_p_file_open(const char *mpeg_file_name, const char *index_file_name, bool fifo);
bplayback_file_t bplayback_p_file_open(const char *mpeg_file_name, const char *index_file_name, bfile_out_fifo_t writer);

bresult bdecode_p_window_set_no_delay( bdecode_window_t window, bool noDelay );

/* keyslot management functions for PVR */
NEXUS_KeySlotHandle b_keyslot_m2m_allocate(const bencryption_params *encryption, bool encrypt, bool timestamp);
void b_keyslot_m2m_free(NEXUS_KeySlotHandle);

/* Hooks from display->audio */
void bdisplay_p_set_compressed_mute(bdisplay_t display, bool compressed_mute);
void bdisplay_p_enable_audio(bdisplay_t display, bool enabled);
void bdecode_p_enable_audio(bdecode_t decode, bool enabled);
void bpcm_play_p_enable(bdisplay_t display, bool enabled);
void bdisplay_p_decode_starting(bdisplay_t display, bdecode_window_t window, bdecode_t decode);

extern struct bdecode g_decode[B_MAX_DECODES+B_MAX_STILL_DECODES];
extern struct bdisplay g_display[B_N_DISPLAYS];
#if B_N_DVI_OUTPUTS
extern struct boutput_hdmi g_hdmi[B_N_DVI_OUTPUTS];
#endif
#if BCHP_CHIP == 7400
extern struct baudio_decode g_audio_decode[B_MAX_DECODES];
#else
extern struct baudio_decode g_audio_decode[1];
#endif

#if B_HAS_ANALOG
bresult btuner_p_set_analog_input(NEXUS_AnalogVideoDecoderHandle analogVideoDecoder, btuner_t tuner, btuner_linein_t linein);
#endif

NEXUS_VideoCodec b_videocodec2nexus(bvideo_codec codec);
NEXUS_AudioCodec b_audiocodec2nexus(baudio_format format);
NEXUS_VideoFormat b_displayformat2nexus(bvideo_format settop_value, const bvideo_format_settings *vesa_settings);
NEXUS_DisplayAspectRatio b_displayaspectratio2nexus(bdisplay_aspect_ratio aspect_ratio);
NEXUS_PixelFormat b_graphicspixelformat2nexus(bgraphics_pixel_format pixel_format);
NEXUS_MessageFormat b_messageformat2nexus(bmessage_format message_format);
#if NEXUS_HAS_FRONTEND
NEXUS_FrontendSatelliteMode b_sdsmode2nexus(btuner_sds_mode mode);
#endif
unsigned b_get_parser_band(bband_t band);
NEXUS_TransportType b_mpegtype2nexus(bstream_mpeg_type);
#if NEXUS_HAS_PLAYBACK
NEXUS_VideoDecoderDecodeMode b_decodermode2nexus(bplaypump_decode_mode);
#endif
int b_volume2nexus(int settop_vol, bool decibel, int *out_volume);
const char *bsettop_get_config(const char *name);

bvideo_codec b_nexus2videocodec(NEXUS_VideoCodec nexus_value);
baudio_format b_nexus2audiocodec(NEXUS_AudioCodec nexus_value);
bvideo_format b_nexus2displayformat(
    NEXUS_VideoFormat nexus_value,
    bvideo_format_settings *vesa_settings /* [out] optional */
    );
bvideo_frame_rate b_nexus2framerate(NEXUS_VideoFrameRate nexus_value);

bresult bdecode_p_set_framemaster(bdecode_t decode, bdecode_window_t window);
bool b_window_is_full_screen(const bdisplay_settings *display_settings, const bsettop_rect *position);

/* Close a stream from playback/playpump */
void bstream_p_close(bstream_t stream);
/* PidChannel management routines */
typedef enum bstream_pid_type
{
    bstream_pid_type_audio,
    bstream_pid_type_video,
    bstream_pid_type_pcr,
    bstream_pid_type_other
} bstream_pid_type;

NEXUS_PidChannelHandle bstream_p_open_pid(bstream_t stream, uint16_t pid, bstream_pid_type type);
void bstream_p_close_pid(bstream_t stream, NEXUS_PidChannelHandle pidChannel);
/* Notify consumers of bstream_mpeg changes */
void brecpump_p_mpeg_change(brecpump_t recpump, const bstream_mpeg *new_settings);
void brecord_p_mpeg_change(brecord_t recpump, const bstream_mpeg *new_settings);
void bdecode_p_mpeg_change(bdecode_t decode, const bstream_mpeg *new_settings);
/* Have consumers release pid channels before a source is closed */
void bstream_p_stop_consumers(bstream_t stream);

#define B_PVR_ATOM_SIZE ((188/4)*4096)
#define B_PVR_PLAYBACK_BUFFER (B_PVR_ATOM_SIZE*8) /* = 1540096 or 1504K or 1.5MB */
#define B_PVR_N_PLAYBACK_DESC 100
#define B_PVR_RECORD_BUFFER (B_PVR_ATOM_SIZE*12)*2  /* Need 4.4MB for network record */
#define B_PVR_N_RECORD_DESC 12

#include "nexus_base_os.h"
#define NEXUS_MODULE_SELF g_settopModule
extern NEXUS_ModuleHandle g_settopModule;

int b_strcmp(const char *s1, const char *s2);
bool is_panel_output(void);

#if defined(XMP1_SUPPORT) || defined(XMP2_SUPPORT)
#include "nexus_ir_blaster.h"
/* The following have been added for the XMP Operation */

/*
Summary:
    Handle returned by buser_xmp_input_open().
*/
typedef struct buser_input_xmp *buser_input_xmp_t;


/** Summary
 XMP Handle structure
 **/
struct buser_input_xmp {
    buser_input_xmp_settings xmp_settings;/* Settings for the XMP */
    buser_input_xmp_status xmp_status;/* Status for the XMP */
    B_EventHandle xmp_status_event; /* Event when status is received from the XMP */
    B_EventHandle xmp_response_event;/* Event when response is received from the XMP */
    B_EventHandle irb_event; /* Event when IR blaster is finished sending data */
    b_event_id_t xmp_response_event_id; /* event id for the response event */
    NEXUS_IrBlasterHandle irBlaster; /* IR Blaster Handle */
};


bresult buser_input_p_send_xmp(
    buser_input_xmp_t remote,
    const uint8_t *input_buffer, /* [out,size_is(length)] */
    size_t length
    );

bresult buser_input_p_receive_xmp(
    buser_input_xmp_t remote,
    uint8_t *output_data, /* [out,size_is(length)] */
    size_t length,
    unsigned *output_size,
    uint32_t *data_packet
    );

void bsettop_p_userio_xmp(buser_input_xmp_t ui_xmp, unsigned *code);
#endif

