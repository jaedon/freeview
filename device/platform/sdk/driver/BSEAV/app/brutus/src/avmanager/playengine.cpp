/***************************************************************************
 *     Copyright (c) 2002-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: playengine.cpp $
 * $brcm_Revision: 153 $
 * $brcm_Date: 9/14/12 11:11a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/avmanager/playengine.cpp $
 * 
 * 153   9/14/12 11:11a mward
 * SWDEPRECATED-2917:  Can't start decode (which will set STC settings)
 * until PCR PID is known, and PCR PID channel is available.
 * 
 * 152   7/26/12 12:42p katrep
 * SW7429-1:restore pat time out to default after increasing it for ip
 * playback
 * 
 * 151   10/3/11 7:06p piyushg
 * SW7425-1396: Implement dummy playready OPL callback to demo protection
 * levels.
 * 
 * 150   9/28/11 5:25p piyushg
 * SW7425-1358:Fix compilation error when PRDY_ROBUSTNESS_ENABLE is not
 * set
 * 
 * 149   9/27/11 7:15p piyushg
 * SW7425-1358: Make playready compliant with robustness rules and
 * maintain one branch for playready with
 * robustness and without. Use PRDY_ROBUSTNESS_ENABLE=y along with
 * MSDRM_PRDY_SUPPORT=y to have playready with robustness.
 * 
 * 148   9/2/10 4:26p erickson
 * SWDEPRECATED-1003: added TimeSkip
 *
 * 147   6/15/10 4:27p mphillip
 * SW7405-4502: Fix warnings with DEBUG=n
 *
 * 146   6/10/10 5:01p erickson
 * SWDEPRECATED-2726: clear _isStreamEnded when changing state. settop api
 * must notify us again.
 *
 * 145   5/20/10 11:02a mward
 * SW7335-661:  Keep support for BSEAV/lib/divxdrm for 7400, 7403.
 * bdrm_decrypt_init() prototype is different.
 *
 * 144   4/2/10 5:53p ssood
 * SW7468-198: allow un-pause to work w/ RTSP/HTTP channels
 *
 * 143   3/8/10 11:49a gmohile
 * SW7468-100 : Fix DQT
 *
 * 142   2/5/10 5:35p mananp
 * SW7335-661:DivX DRM certification for 7335
 *
 * 142   2/5/10 5:10p mananp
 * SW7335-661:DivX DRM certification for 7335
 *
 * 140   1/22/10 4:03p mward
 * SW7335-661: Restore changes lost in checkin of version 139.
 *
 * 138   12/22/09 1:32p erickson
 * SW7420-70: add BERR_TRACE's to start() to pinpoint failure
 *
 * 137   12/17/09 6:09p mward
 * SW7125-129: And also use DQT only for files with .nav index.
 *
 * 136   12/17/09 4:56p mward
 * SW7125-129: And also use DQT only for h264.
 *
 * 135   12/16/09 11:49a mward
 * SW7400-2602: Now enable B_HAS_PB120_TRICKMODES for 7400.
 * SW7125-129:Use DQT only for bstream_mpeg_type_ts.
 *
 * SW7400-2602/3   12/16/09 11:46a mward
 * SW7125-129:Use DQT only for bstream_mpeg_type_ts.
 *
 * SW7400-2602/2   12/16/09 11:24a mward
 * SW7400-2602: changes from main branch.
 *
 * 134   12/3/09 11:16a mward
 * SW7125-58: SW7342-64: Enable 0.8, 1.2x trick modes for 7125, 7342 RAP
 * supports DSOLA.
 *
 * 133   12/1/09 4:56p mward
 * SW7125-129:Use DQT only for reverse trick modes.
 *
 * SW7400-2602/1   11/19/09 6:56p mward
 * SW7400-2602: Now enable B_HAS_PB120_TRICKMODES for 7400.
 *
 * 132   11/5/09 2:27p mward
 * SW7125-58: SW7342-64: Disable 0.8, 1.2x trick modes for 7125, 7342
 * until RAP supports DSOLA.
 *
 * 131   7/28/09 11:14a lwhite
 * PR55693: Added DTCP/IP intialization
 *
 * 130   7/13/09 1:26p vsilyaev
 * PR 55989: Always use by rate if index file is the same as data file
 *
 * 129   6/29/09 2:11p lwhite
 * PR53773: Fixed SSL initialization
 *
 * 128   6/24/09 7:12p lwhite
 * PR53773: Adding DTCP/IP + SSL Support in Brutus
 *
 * 127   6/3/09 12:01p jtna
 * PR43001: IP streams background recording work
 *
 * 126   5/11/09 5:12p erickson
 * PR54915: remove unused and misleading code
 *
 * 125   5/6/09 3:55p jjordan
 * PR50291: add Wide GA mode (config from brutus.cfg)
 *
 * 124   12/9/08 1:48p mward
 * PR50084: Disable FSPB(80/120 playback), for 7400 not implemented in
 * 7400 audio decoder.
 *
 * 123   12/8/08 12:18p gmohile
 * PR 49887 : Restart divx playback without drm, if drm fails
 *
 * 122   12/4/08 9:41a jjordan
 * PR49939: increase timeout to allow time to charge TTS buffer
 *
 * 121   10/31/08 12:29p jjordan
 * PR47230: Add TTS Pacing
 *
 * 120   10/15/08 9:09a erickson
 * PR47898: convert PB120 condition to runtime test
 *
 * 119   9/18/08 6:28p katrep
 * PR45880: Add support for FSPB(80/120 playback)
 *
 * 118   7/22/08 11:32a jtna
 * PR44493: Add 192byte TS playback support in Brutus
 *
 * 117   7/16/08 1:50p erickson
 * PR44648: fix loopmode
 *
 * 116   7/14/08 12:54p ssood
 * PR42739: fix for getting IP tuning work on both Main & PIP
 *
 * 115   7/2/08 4:10p ssood
 * PR42739: fixing a compile error for non LiveMedia builds
 *
 * 114   7/2/08 2:23p ssood
 * PR42739: Auto PID discovery fails at times for IP with live IP Servers,
 * need to increase the discovery timeout for IP Channels
 *
 * 113   6/27/08 3:49p mphillip
 * PR35310: AES is not necessarily network-encrypted
 *
 * 112   6/11/08 7:18p ssood
 * PR42739: Incorrect check looking for valid input band for IPSTB causes
 * Auto PID discovery to Fail on Nexus based platforms
 *
 * 111   3/20/08 1:49a erickson
 * PR40307: update
 *
 * 110   3/19/08 9:35p erickson
 * PR40307: thumbnail update
 *
 * 109   3/19/08 1:24p erickson
 * PR40307: change non-error ERR's to WRN or MSG. remove unneeded n from
 * BDBG_XXX.
 *
 * 108   3/18/08 4:05p erickson
 * PR40307: added thumbnail feature
 *
 * 107   3/13/08 12:34p erickson
 * PR36068: first AVC ffwd speed should be 1I
 *
 * 106   3/4/08 12:52p vsilyaev
 * PR 39818: Added MKV format
 *
 * 105   2/19/08 11:16a yrajakar
 * PR36762: Update status with first_pts, and duration
 *
 * 104   2/15/08 10:53a ssood
 * PR36419: added support for HTTP playback from a PC streaming server
 * (VLC)
 *
 * 103   2/13/08 1:49p ssood
 * PR36419: added support for PAUSE/STOP trickmodes for HTTP playback
 *
 * 102   2/13/08 1:10p ssood
 * PR36422: added support for inband trick mode transitions for HTTP using
 * a new socket for every transition
 *
 * 101   1/30/08 11:04a gmohile
 * PR 39014 : Seperate DQT input params
 *
 * 100   1/16/08 2:09p ssood
 * PR38608: Merging from Netacel branch to main branch
 *
 * Netaccel_Devel/2   12/20/07 11:55a ssood
 * PR38068: do HTTP trickmodes only if server supports it
 *
 * Netaccel_Devel/1   12/18/07 6:02p ssood
 * PR38179: HTTP trickmodes enhancements
 *
 * PR37038/1   12/11/07 2:51p erickson
 * PR37038: convert continuous record to use beginning_of_stream callbacks
 *
 * 98   12/6/07 2:08p erickson
 * PR37038: add CONTINUOUS_RECORD_DURATION, remove unused fixedSize and
 * CIRCFILE.
 *
 * 97   12/5/07 4:12p erickson
 * PR36068: fix indenting
 *
 * 96   10/2/07 4:12p gmohile
 * PR 34640: Add Divx DRM memory encryption/decryption
 *
 * 95   9/25/07 2:48p gmohile
 * PR 34640 : Add Divx DRM support
 *
 * 94   9/17/07 2:28p yrajakar
 * PR34965: Fixing Simultaneouos HTTP trickmodes for Main and PIP
 *
 * 93   9/4/07 11:20a yrajakar
 * PR34578: Adding support for RTP FEC
 *
 * 92   7/27/07 4:57p mward
 * PR28422: Enable FSPB support for 7118.
 *
 * 91   7/20/07 11:31a katrep
 * PR27643: Fixed compiler error
 *
 * 90   7/18/07 12:16p erickson
 * PR30310: added DQT_ENABLED and DQT_PICS_PER_GOP options
 *
 * 89   7/16/07 5:28p rjlewis
 * PR28665: Fixes to allow IPSTB under VxWorks.
 *
 * 88   7/11/07 6:21p vsilyaev
 * PR 25109: Enabled trickmodes for MP4 container
 *
 * 87   7/11/07 4:24p erickson
 * PR32894: print 2 decimal digits for 0.25x speed
 *
 * 86   7/11/07 11:25a erickson
 * PR32852: rework trick mode logic. handle ASF/AVI similarly. don't
 * advance Brutus state if trick mode fails. don't allow reverse or very-
 * fast without an index.
 *
 * 85   6/22/07 1:36p jjordan
 * PR32034: fix channel change back to TTS stream
 *
 * 84   6/14/07 3:08p gmohile
 * PR 28422: Add FSPB support for 7403
 *
 * 83   6/13/07 11:45a erickson
 * PR28422: restore b_convert_to_lograte for ASF and AVC
 *
 * 82   6/13/07 11:30a gmohile
 * PR 25109 : Use avc I frame trickmodes only for TS streams
 *
 * 81   6/11/07 1:04p jjordan
 * PR32034: Add support for TTS streams in IP channels
 *
 * 80   6/7/07 5:07p erickson
 * PR28422: fix AVC slow reverse. should be I-frame reverse.
 *
 * 79   6/7/07 1:27p erickson
 * PR28422: select 1.2x inside of Brutus and don't rely on
 * bplayback_get_trickmode_by_rate. This is needed for ASF 1.2x.
 *
 * 78   6/6/07 4:49p erickson
 * PR28422: add PB120/80 trick modes for 7401
 *
 * 77   5/22/07 12:06p erickson
 * PR28906: readded fix in version 71 and 72. it was removed by bad merge
 * in version 73.
 *
 * 76   5/17/07 1:23p erickson
 * PR30310: add bplayback_player_mode_by_gop
 *
 * 75   4/25/07 6:48p jrubio
 * PR30010: fixed vxworks compilation
 *
 * 74   4/24/07 10:43a gmohile
 * PR 28585: Use decoder trickmodes if no NAV file is available
 *
 * 73   4/19/07 6:41p jrubio
 * PR30010: added tcp channel
 *
 * 72   3/22/07 11:11a erickson
 * PR28906: use float for by_rate only if there's a decimal portion.
 * easier on the eye.
 *
 * 71   3/20/07 2:48p erickson
 * PR28906: use float for by_rate trick mode UI. better for STC trick
 * modes.
 *
 * 70   3/13/07 11:48a erickson
 * PR26146: make ASF and BCM-only trick modes logrithmic, fixing indenting
 * in trickmode func
 *
 * 69   3/9/07 2:11p piyushg
 * PR28588: Add DRM-NDR support in brutus
 *
 * 68   2/27/07 9:07p erickson
 * PR27950: fix UI for slow VC1 trick modes
 *
 * 67   1/29/07 5:30p gmohile
 * PR 25704: add divx 3.11 support
 *
 * 66   1/18/07 12:24p ahulse
 * PR27186: Differentiate between parser and input bands. AKA, fixing
 * bmessage from playback source
 *
 * 65   1/17/07 6:15p ptimariu
 * PR20685: initializing ext_asf_scanning to 0
 *
 * 64   1/17/07 11:23a rjlewis
 * PR27177: channel info is 14k; use malloc for this information.
 *
 * 63   1/9/07 4:19p gmohile
 * PR 26957: fixed bug in livemedia pause
 *
 * 62   1/2/07 3:59p erickson
 * PR25037: if bplayback_file_open fails with index, try again with no
 * index. make Brutus resilient.
 *
 * 61   12/19/06 10:28a erickson
 * PR26543: removing private include files. no harm seen. must have been a
 * temp hack.
 *
 * 60   12/14/06 12:42p jjordan
 * PR 26473: fix timeout value
 *
 * 59   12/14/06 11:59a jjordan
 * PR26473: modify PSI acquisition timeout for RTSP channels
 *
 * 58   12/7/06 10:48a jgarrett
 * PR 26293: Fixing memory leak
 *
 * 57   12/5/06 5:57p ssood
 * PR26252: IP STB SW unification w/ Generic Brutus release
 *
 * 56   11/30/06 6:34p erickson
 * PR26095: the enum changed, so update the array
 *
 * 55   11/30/06 5:30p ssood
 * PR26252: IP STB SW unification w/ Generic Brutus release
 *
 * 54   11/17/06 1:12p erickson
 * PR25538: added setTrickMode for random access to trick modes from
 * scriptengine
 *
 * 53   10/30/06 4:50p erickson
 * PR25109: added DivX support
 *
 * 52   10/25/06 5:33p arbisman
 * PR25145: Update 97398 Brutus IP STB build to work with latest Settop
 * API
 *
 * 51   10/4/06 4:01p vsilyaev
 * PR 23826: Added RTSP support for AVC/AAC decoder over RTP
 *
 * 50   10/3/06 4:49p jjordan
 * PR24714: Add support for RTSP protocol
 *
 * 49   9/28/06 6:56p arbisman
 * PR24289: Back-out changes
 *
 * 47   9/12/06 5:13p erickson
 * PR23062: added bplayback_player_mode_by_rate
 *
 * 46   9/8/06 5:21p tokushig
 * PR20685: add support for simple/main profile in brutus
 *
 * SanDiego_DRM_ASF/5   9/7/06 6:21p tokushig
 * merge from main
 *
 * 45   9/1/06 4:34p erickson
 * PR23062: support fast play modes
 *
 * 44   8/9/06 5:10p erickson
 * PR21941: don't clear video and audio pids in Video with using
 * VIDEODECODE_ENABLED and AUDIODECODE_ENABLED
 *
 * 42   7/31/06 9:42a jjordan
 * PR22906: fix hang with auto-pids/high bitrate stream
 *
 * 41   7/28/06 12:59p jjordan
 * PR22906: fix plain UDP when running LM
 *
 * 40   7/26/06 2:58p tokushig
 * PR20685: added DRM support to brutus
 *
 * 40   7/26/06 2:15p tokushig
 * PR20685: added DRM support to brutus
 *
 * SanDiego_DRM_ASF/4   9/7/06 5:20p tokushig
 * merge from PR20685_MSDRM branch to include http based drm license
 * retrieval
 *
 * PR20685_MSDRM/2   8/22/06 11:25a ptimariu
 * PR20685: moved key to mpeg from video.mpeg
 *
 * PR20685_MSDRM/1   8/21/06 12:22p ptimariu
 * PR20685: removed explicit lic_extern
 *
 * 44   8/9/06 5:10p erickson
 * PR21941: don't clear video and audio pids in Video with using
 * VIDEODECODE_ENABLED and AUDIODECODE_ENABLED
 *
 * 42   7/31/06 9:42a jjordan
 * PR22906: fix hang with auto-pids/high bitrate stream
 *
 * 41   7/28/06 12:59p jjordan
 * PR22906: fix plain UDP when running LM
 *
 * 40   7/26/06 2:58p tokushig
 * PR20685: added DRM support to brutus
 *
 * 40   7/26/06 2:15p tokushig
 * PR20685: added DRM support to brutus
 *
 * SanDiego_DRM_ASF/3   7/24/06 5:21p tokushig
 * only start playpump once.  pass drm decrypt info thru
 * mpeg.encryption.key
 *
 * SanDiego_DRM_ASF/2   7/21/06 4:18p tokushig
 * do not call drm routines if media file is unencrypted
 *
 * SanDiego_DRM_ASF/1   7/20/06 3:25p tokushig
 * add drm handling in the playengine
 *
 * 39   7/25/06 5:23p jjordan
 * PR22906: Integrate liveMedia open-source for RTP and RTCP support
 *
 * 38   6/5/06 3:36p mphillip
 * PR19786: Handle trimmed areas better
 *
 * 37   4/14/06 1:59p jgarrett
 * PR 20139: Updating include path
 *
 * 36   3/21/06 5:42p ahulse
 * PR18352: Back out change to allow Skip B play IP, because some streams
 * show artefacts
 *
 * 35   3/16/06 3:44p mphillip
 * PR19786: Fix the opening of .NAV files
 *
 * 34   3/15/06 4:04p mphillip
 * PR19786: Initial implementation.  Add CONTINUOUS_RECORD_ENABLED=1 to
 * brutus.cfg to enable
 *
 * 33   3/9/06 6:47p ptimariu
 * PR20107: Fix IPSTB auto-PID scan for 97401
 *
 * 32   3/9/06 11:07a ahulse
 * PR20102: For AVC & VC1, frameafvance is I frame only
 *
 * 31   3/1/06 3:24p jjordan
 * PR18981: fix "live playback" and add config option IP_USE_LIVE_PLAYBACK
 *
 * 30   2/28/06 10:40a jjordan
 * PR18572: merge timeshifting fix for IP STB
 *
 * 29   2/22/06 4:42p erickson
 * PR17108: remove warnings
 *
 * 28   2/6/06 3:05p erickson
 * PR17108: avoid div-b-zero
 *
 * 26   12/19/05 6:18p ahulse
 * PR18352: added skip B & skip IP to avc trickmodes
 *
 * 25   11/4/05 11:13a jjordan
 * PR17950: fix IP STB audio startup problem
 *
 * 24   10/21/05 3:04p dma
 * PR17733: auto channel/PID scan for IP playback
 *
 * 23   10/12/05 5:35p jjordan
 * PR17547: Add support for RTP
 *
 * 22   9/12/05 10:40a erickson
 * PR17033: removed external trick mode options, and made FORCE_HOST and
 * FORCE_I_FRAME set frame advance correctly
 *
 * 21   8/31/05 11:56a erickson
 * PR16921: only allow I frame AVC trick modes for the first release
 *
 * 20   8/24/05 6:10p arbisman
 * PR16678: Add support for WinCE
 *
 * 19   8/17/05 5:25p erickson
 * PR16719: rework Video and Media to never return MString as return value
 *
 * 18   8/12/05 10:42a jjordan
 * PR16669: add IP STB playback from HDD capability
 *
 * 17   8/12/05 10:02a jjordan
 * PR16670: add IP STB record to HDD capability
 *
 * 16   8/12/05 9:59a jjordan
 * PR16668: add IP STB config screen to IP STB GUI
 *
 * 15   8/4/05 9:08a erickson
 * PR16504: converted playpump_params.band to
 * playpump_params.route_through_parser_band
 *
 * 14   8/3/05 4:30p erickson
 * PR16526: if we're playing with no index, use mpeg offset jumping
 *
 * 13   8/1/05 12:24p erickson
 * PR16300: allow IP for AVC
 *
 * 12   7/7/05 1:10p erickson
 * PR16118: slow motion forward is normal play
 *
 * 11   7/6/05 4:39p erickson
 * PR16119: AVC and iframe only needed fix for slow motion
 *
 * 10   6/29/05 9:05a jjordan
 * PR16042: Add support for "IP channels"
 *
 * 9   5/20/05 5:22p mphillip
 * PR15267: Moving AES/SuperScrambler to a more appropriate place
 *
 * 8   5/19/05 6:53p mphillip
 * PR15267: Basic SetTop API support for decryption of AES and DVB files
 * for playback
 *
 * 7   4/20/05 12:21p erickson
 * PR13823: if the index is bad, try again with no index
 *
 * 6   3/24/05 1:21p erickson
 * PR14593: removed old platform
 *
 * 5   3/21/05 1:21p erickson
 * PR14451: default AVC to IFRAME trick modes
 *
 * 4   3/21/05 11:20a erickson
 * PR14451: added impromptu I_FRAME_ONLY for AVC
 *
 * 3   2/25/05 12:40p erickson
 * PR14162: added bplayback_goto_mpeg_file_offset for test (commented out)
 *
 * 2   2/16/05 4:46p erickson
 * PR11077: add some early detection for bad ES values
 *
 * 1   2/7/05 7:54p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 *
 ****************************************************************************/
#include "playengine.h"
#include "recordengine.h"
#include "cfgsettings.h"
#include "bsettop.h"
#include "video.h"
#include "bsettop_fileio_fifo.h"
#include "bkni.h"

#ifndef _WIN32_WCE
    #include <unistd.h>
    #include <errno.h>
    #include <assert.h>
#endif

#include "bstd.h"

/* all chips now support PB120, default on. the use_rap_trick_modes env variable
is used in Settop API. */
#if (BCHP_CHIP == 7408)
#define B_HAS_PB120_TRICKMODES() 0
#else
#define B_HAS_PB120_TRICKMODES() (!getenv("use_rap_trick_modes"))
#endif

#include <tspsimgr.h> /* for auto PIDs detection */
#define PLAYBACK_IP_PAT_TIMEOUT 5000
#ifdef LIVEMEDIA_SUPPORT
  #include "lm_helper.h"
#endif

BDBG_MODULE(brutus_playengine);

#define TIMER_INTERVAL 100 //msec
#define NETWORK_PVR_PORT        5001

#ifdef B_HAS_IP

#ifdef __vxworks
#include <sockLib.h>
#endif

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <stdio.h>

/* network PVR control port */
#ifndef USE_STRM_RESYNC_LOGIC
static bresult http_set_pvr_speed(void *engine, bplayback_t playback, const char *ip_addr, unsigned int port, int speed)
{
    unsigned int pts, first_pts;
    bplayback_status status;
    bresult rc;
    Video * video = ((PlayEngine*)engine)->video();

    if (video->socket.open_params.num_play_speed_entries == 1) {
        BDBG_ERR(("%s: server doesn't support trickmodes", __FUNCTION__));
        return berr_invalid_parameter;
    }

    first_pts = video->socket.open_params.first_pts;
    if (rc = bplayback_get_status(((PlayEngine*)engine)->handle(), &status)) {
        pts = ((PlayEngine*)engine)->pts();
        BDBG_MSG(("playback get status = %u, use last pts = %u, first_pts=%u", rc, pts,
        first_pts));
    }
    else {
        pts = status.position.timestamp;
    }

    /* notify playback ip thread about this trick mode transition */
    bplayback_goto_index(playback, (long)pts, speed);
    return b_ok;
}

#else /* USE_STRM_RESYNC_LOGIC */
/* network PVR control port */
static bresult http_set_pvr_speed(void *engine, bplayback_t playback, const char *ip_addr, unsigned int port, int speed)
{
    bplayback_status status;
    double seek_range;
    unsigned int pts, first_pts;
    struct sockaddr_in addr;
    socklen_t addrlen = sizeof(struct sockaddr_in);
    Video * video = ((PlayEngine*)engine)->video();
    bresult rc;

    struct sockaddr_in localAddr;
    struct {
        int                     fd;
        struct sockaddr_in      local_addr;
        struct sockaddr_in      remote_addr;
        struct hostent *        he;
        socklen_t               addr_len;
    } pvr_socket;

    if(getsockname(video->socket.open_params.fd, (struct sockaddr *) &addr, &addrlen))
    {
        BDBG_ERR(("Socket Port error"));
        return berr_invalid_parameter;
    }

    /* this can block, but it's short...and this is idle time, right? */
    first_pts = video->socket.open_params.first_pts;
    if (rc = bplayback_get_status(((PlayEngine*)engine)->handle(), &status))
    {
        pts = ((PlayEngine*)engine)->pts();
        BDBG_MSG(("playback get status = %u, use last pts = %u, first_pts=%u", rc, pts,
            first_pts));
    }
    else
    {
        pts = status.position.timestamp;
    }
    /* add 200 msec network delay */
    if(pts > first_pts)
    {
        seek_range = (pts - first_pts + 12000.) / 45000;
    }
    else
        seek_range = 0.;

    pvr_socket.remote_addr.sin_family = AF_INET;
    pvr_socket.remote_addr.sin_port = htons (port);
#ifdef __vxworks
    pvr_socket.remote_addr.sin_addr.s_addr = inet_addr((char*)ip_addr);
#else
    pvr_socket.remote_addr.sin_addr.s_addr = inet_addr(ip_addr);
#endif

    if ((pvr_socket.fd = socket (AF_INET, SOCK_STREAM, 0)) == -1 ) {
        BDBG_ERR(("input engine fails to open udp socket"));
        return berr_invalid_parameter;
    }


        localAddr.sin_family = AF_INET;
        localAddr.sin_addr.s_addr = htonl(INADDR_ANY);
        localAddr.sin_port = htons(0);

        if(bind(pvr_socket.fd,(struct sockaddr *) &localAddr, sizeof(localAddr))){
            BDBG_ERR(("Socket Bind Err"));
            return berr_invalid_parameter;
        }

        if (connect(pvr_socket.fd, (struct sockaddr *)&(pvr_socket.remote_addr), sizeof(pvr_socket.remote_addr)) < 0) {
            BDBG_ERR(("Socket connect Err"));
            return berr_invalid_parameter;
        }
    BDBG_WRN(("HTTP %s:%d connected, socket=%d seek_range=%f, speed=%d", inet_ntoa(pvr_socket.remote_addr.sin_addr),
              ntohs(pvr_socket.remote_addr.sin_port),pvr_socket.fd, seek_range, speed ));

        /* Now that we have successfully connected to Server on the PVR Port, we can notify */
        /* playback ip thread about this trick mode transition */
        bplayback_goto_index(playback, 0, 0);

    {
        int cnt;
        char *p_psi;
        char write_buf[256];
        char read_buf[512];

        cnt = sprintf(write_buf,"GET HTTP/1.1\r\n"
                          "User-Agent: BRCM 97401\r\n"
                          "PlaySpeed.dlna.org: speed=%d\r\n"
                          "TimeSeekRange.dlna.org: npt=%.3f\r\n"
                          "PeerPort: %d\r\n"
                          "\r\n\r\n",
                speed, seek_range,ntohs(addr.sin_port));



        if (write(pvr_socket.fd, write_buf, cnt) < 0) {
            BDBG_ERR(("HTTP write Err"));
            return berr_invalid_parameter;
        }
        BDBG_MSG(("after write request"));
        BKNI_Sleep(10);
        if ((cnt = read(pvr_socket.fd, read_buf, 511)) < 0) {
            BDBG_ERR(("HTTP read Err"));
            return berr_invalid_parameter;
        }

        read_buf[cnt] = 0;
        p_psi=strstr(read_buf,"MediaDuration: ");
        if (p_psi)
        {
                    p_psi += strlen("MediaDuration: ");
                    sscanf(p_psi,"%lf",&video->socket.open_params.duration);
                    BDBG_MSG(("media-Duration[sec]: %f",video->socket.open_params.duration));
        }

        BDBG_WRN(("HTTP [%s]",read_buf));
    }
    close(pvr_socket.fd);
    BDBG_WRN(("Closing PVR Event Socket %d; waiting 300 mec",pvr_socket.fd));

    return b_ok;
}
#endif //USE_STRM_RESYNC_LOGIC
#else
static bresult http_set_pvr_speed(void *engine, bplayback_t playback, const char *ip_addr, unsigned int port, int speed)
{
    return b_ok;
}
#endif

#if B_HAS_DRM

/* example callback function for the license grabber
*/
bdrm_err
bdrm_util_license_extern (const bdrm_license_challenge* lic_challenge,
                          bdrm_license_response* lic_response,
                          const void * context)
{
    bdrm_err     ret      = bdrm_err_ok;
    PlayEngine * pEngine  = NULL;

    BSTD_UNUSED(lic_response);

    if (context)
    {
        pEngine = (PlayEngine *)context;
    }

    if ((lic_challenge != NULL) && (lic_response != NULL))
    {
        lic_response->len = 0; lic_response->data = NULL;   /* response data goes here */
        BDBG_MSG(("############################## extern grabber: challenge len=(%d) url=%s data=%c",
                  lic_challenge->len, lic_challenge->url, lic_challenge->data[0]));
    }
    return(bdrm_err_fail); /* TEMPORARY */
    return(ret);
}

/* example callback function for the license output protection level
*/
bdrm_err 
bdrm_util_policy_callback(bdrm_policy_t *policy, const void* ctx)
{

    /* Set output protection level to match the level required in the license */
    BDBG_ERR(("EXAMPLE CALLBACK FUNC for OPL::Set output protection level to match the license"));
    return bdrm_err_ok;
}

/* example callback function for the license state notification
*/
bdrm_err
bdrm_util_license_callback (bdrm_acq_state* lic_acq_state,
                            bdrm_license_t license,
                            const void * context)
{
    MString             type_msg;
    bdrm_license_info   lic_info;
    bdrm_license_state  lic_state;
    bdrm_http_state     http_state;
    uint8_t             count       = 0;
    uint8_t             index       = 0;
    PlayEngine        * pEngine     = NULL;
    bdrm_err            ret         = bdrm_err_ok;

    if (lic_acq_state)
    {
        lic_state  = lic_acq_state->license_state;
        http_state = lic_acq_state->http_state;
        fprintf(stderr, "==================== setting drm license state:%d http state: %d\n", lic_state, http_state);
    }
    else
    {
        return (ret) = bdrm_err_fail;
    }

    BDBG_MSG(("---------------------------------->>license state changed TO (%d)", lic_state));

    if (context)
    {
        MString strMsg;
        pEngine = (PlayEngine *)context;

        pEngine->setDRMLicenseState(lic_state);
        pEngine->setDRMHTTPState(http_state);
    }
    else
    {
        BDBG_ERR(("Cannot save DRM license state!"));
    }

    if ((lic_state == bdrm_lic_acquired)  ||
        (lic_state == bdrm_lic_faillocal) ||
        (lic_state == bdrm_lic_failtransfer))
    {
        count = bdrm_license_get_count(license);
        for (index = 0; index < count; index++)
        {
            memset(&lic_info, 0, sizeof(bdrm_license_info));
            bdrm_license_get_item(license, index, &lic_info);
            switch (lic_info.type)
            {
            case bdrm_lictype_from: type_msg = "from"; break;
            case bdrm_lictype_until: type_msg = "until"; break;
            case bdrm_lictype_fromuntil: type_msg = "from-until"; break;
            case bdrm_lictype_count: type_msg = "count"; break;
            case bdrm_lictype_unlimited: type_msg = "unlimited"; break;
            case bdrm_lictype_noright: type_msg = "noright"; break;
            case bdrm_lictype_unknown: type_msg = "unknown"; break;
            }
            BDBG_MSG(("########################## stream id=%d type_msg=\"%s\" type=(%d) "
                      "play_count=(%d) plays=%d times_count=(%d) times=%s",
                      lic_info.stream_id, type_msg.s(), lic_info.type,
                      lic_info.plays_count, lic_info.plays[0],
                      lic_info.times_count, asctime(&lic_info.times[0])));
        }
    }
    else ret = bdrm_err_fail;

    return(ret);
}

void PlayEngine::setDRMLicenseState(bdrm_license_state state)
{
    _drm_license_state = state;
    FIRE(DRMLicenseStateChanged);
}

void PlayEngine::setDRMHTTPState(bdrm_http_state state)
{
    _drm_http_state = state;
    FIRE(DRMHTTPStateChanged);
}

#endif /* B_HAS_DRM */


bplayback_loopmode PlayEngine::beginning_of_stream(void *context)
{
    PlayEngine *p = (PlayEngine*)context;
    const char *str = p->_record ? p->_cfg->get(Config::PLAYBACK_AT_BOF_WITH_RECORD) : p->_cfg->get(Config::PLAYBACK_AT_BOF);
    BDBG_ASSERT(str); /* config will always have a default */

    BDBG_MSG(("beginning_of_stream"));
    if (!strcasecmp(str, "pause")) {
        return bplayback_loopmode_pause;
    }
    else if (!strcasecmp(str, "play")) {
        return bplayback_loopmode_play;
    }
    else {
        return bplayback_loopmode_loop;
    }
}

bplayback_loopmode PlayEngine::end_of_stream(void *context)
{
    PlayEngine *p = (PlayEngine*)context;
    const char *str = p->_record ? p->_cfg->get(Config::PLAYBACK_AT_EOF_WITH_RECORD) : p->_cfg->get(Config::PLAYBACK_AT_EOF);
    BDBG_ASSERT(str); /* config will always have a default */

    BDBG_MSG(("end_of_stream"));
    if (!strcasecmp(str, "pause")) {
        p->_isStreamEnded = true;
        return bplayback_loopmode_pause;
    }
    else if (!strcasecmp(str, "play")) {
        p->_isStreamEnded = true;
        return bplayback_loopmode_play;
    }
    else {
        return bplayback_loopmode_loop;
    }
}

PlayEngine::PlayEngine(MApplication *app, Config *cfg,
                       const char *aname, int index) : _timer(app)
{
    _cfg = cfg;
    _name = aname;
    _current.firstpos = _current.lastpos = _current.pos = 0;
    _state = bplayback_state_stopped;
    _timer.addHandler(this);
    _index = index;
    _isStreamEnded = false;

    _stream = NULL;

#if B_HAS_DRM
    _drm       = NULL;
    _dcrypt    = NULL;
#endif

#if B_HAS_DIVX_DRM
    _divx_decrypt=NULL;
#endif

    /* open regular playback and IP playback */
    _playRegular = bplayback_open();
    _playIP = bplayback_ip_open();

    /* default active playback to regular playback */
    _play = _playRegular;

    {
        bplaypump_open_params open_params;
        bplaypump_get_open_params(B_ID(index), &open_params);
        open_params.buffer_size *= 2;
        open_params.num_descriptors *= 2;
        _playpump = bplaypump_open(B_ID(index), &open_params);
    }
    if (!_playpump)
        BDBG_ERR(("Unable to open bplaypump. No PVR support."));
}

PlayEngine::~PlayEngine()
{
    stop();
    if (_playpump)
    {
        bplaypump_close(_playpump);
    }
    if (_playIP)
    {
        bplayback_ip_close(_playIP);
    }
    if (_playRegular)
    {
        bplayback_close(_playRegular);
    }
}

bstream_t PlayEngine::start(Video *video, int start_at_sct, brecord_t record, RecordEngine *recordEngine)
{
    bstream_t stream = NULL;
    const char *indexfile;
    bplayback_params params;
    bstream_mpeg mpeg; /* cached value so it can be tweaked */

    if (!valid()) {
        BERR_Code rc=BERR_TRACE(berr_invalid_state);
        return NULL;
    }
    if (isActive())
    {
        if (stop()) {
            BERR_Code rc=BERR_TRACE(berr_invalid_state);
            return NULL;
        }
    }

    _video = video;

    mpeg = _video->mpeg; /* copy so we can disable audio/video is requested */
    if (!_cfg->getBool(Config::AUDIODECODE_ENABLED))
        memset(mpeg.audio, 0, sizeof(mpeg.audio));
    if (!_cfg->getBool(Config::VIDEODECODE_ENABLED))
        memset(mpeg.video, 0, sizeof(mpeg.video));

    _record = record;
    _indexFormat = video->indexFormat(); // cache for speed
    if (!_cfg->getBool(Config::TRICKMODES_ENABLED))
        _indexFormat = bindex_format_none;
    if (video->isIP)
    {
        /* assign active playback to be IP playback and open socket */
        _play = _playIP;

#ifdef LIVEMEDIA_SUPPORT
        if((video->socket.open_params.protocol != bsettop_ip_protocol_udp) ||
           (video->socket.open_params.protocol != bsettop_ip_protocol_rtp_fec)) {
            video->socket.lm_callbacks.lm_session_open = liveMediaSessionOpen;
            video->socket.lm_callbacks.lm_session_start = liveMediaSessionStart;
            video->socket.lm_callbacks.lm_session_rtsp_ctrl = liveMediaSessionRTSPControl;
            video->socket.lm_callbacks.lm_session_stop = liveMediaSessionStop;
            video->socket.lm_callbacks.lm_session_close = liveMediaSessionClose;
            video->socket.lm_callbacks.lm_get_data = liveMediaSessionGetData;
        }
#endif


        switch (video->socket.open_params.security.security_protocol) {
        case bsettop_ip_security_ssl:
            BDBG_MSG(("Playengine: SSL configured" ));
            if (!(_cfg->get(Config::SSL_CA_FILE)) && !(_cfg->get(Config::SSL_CA_PATH))) {
                    BDBG_ERR(("Neither SSL_CA_FILE or SSL_CA_PATH are set in cfg, but channel requires SSL"));
                    return NULL;
            }
            if (_cfg->get(Config::SSL_CA_FILE))
                strncpy(video->socket.open_params.security.security_info.ssl.ca_file, _cfg->get(Config::SSL_CA_FILE),128);
            else
                memset(video->socket.open_params.security.security_info.ssl.ca_file, 0, 128);
            if (_cfg->get(Config::SSL_CA_PATH))
                strncpy(video->socket.open_params.security.security_info.ssl.ca_path, _cfg->get(Config::SSL_CA_PATH),128);
            else
                memset(video->socket.open_params.security.security_info.ssl.ca_path, 0, 128);
            if (_cfg->get(Config::SSL_CLIENT_CERT_FILE))
                strncpy(video->socket.open_params.security.security_info.ssl.client_cert_file, _cfg->get(Config::SSL_CLIENT_CERT_FILE),128);
            else
                memset(video->socket.open_params.security.security_info.ssl.client_cert_file, 0, 128);
            if (_cfg->get(Config::SSL_CLIENT_CERT_KEY))
                strncpy(video->socket.open_params.security.security_info.ssl.client_cert_key, _cfg->get(Config::SSL_CLIENT_CERT_KEY),128);
            else
                memset(video->socket.open_params.security.security_info.ssl.client_cert_key, 0, 128);
            if (_cfg->get(Config::SSL_CLIENT_CERT_PASSWORD))
                strncpy(video->socket.open_params.security.security_info.ssl.password, _cfg->get(Config::SSL_CLIENT_CERT_PASSWORD),16);
            else
                memset(video->socket.open_params.security.security_info.ssl.password, 0, 16);
            break;
        case bsettop_ip_security_dtcp_ip:
            video->socket.open_params.security.security_info.dtcp_ip.dtcp_ip_ctx = video->dtcpIpCtx;
            video->socket.open_params.security.security_info.dtcp_ip.ake_server_port = _cfg->getInt(Config::DTCP_IP_AKE_SERVER_PORT);
            break;
        default:
            video->socket.open_params.security.security_protocol = bsettop_ip_security_none;
            break;
        }

        _file = bplayback_socket_open(_play, &video->socket);
        if (!_file) {
            BERR_Code rc=BERR_TRACE(berr_not_supported);
            return NULL;
        }

        bplayback_params_init(&params, _play);
        params.index_format = bindex_format_none;

        if (_cfg->getBool(Config::MANAGE_IP_NETWORK_BUFFER))
            params.ip_params.buffer_scheme = bplayback_ip_buffer_scheme_stc_clock_steering;
        if (_cfg->getBool(Config::IP_USE_LIVE_PLAYBACK))
            params.ip_params.use_live_playback_mode = true;
        else
            params.ip_params.use_live_playback_mode = false;

        {
            bdecode_settings dec_settings;
            bdecode_get(video->adecode[video->decoderIndex], &dec_settings);
            dec_settings.tsm.wga_mode_enable = _cfg->getInt(Config::WGA_MODE_ENABLED);
            bdecode_set(video->adecode[video->decoderIndex], &dec_settings);
        }
        // route through pid parser (needed for record)
        params.route_through_parser_band = true;
            /* check if program info has been already received part of the HTTP response from the server */
            if(video->socket.open_params.protocol == bsettop_ip_protocol_http &&
                    video->socket.open_params.psi.mpeg_type != bstream_mpeg_type_unknown){
            video->mpeg = mpeg = video->socket.open_params.psi;       /* structure copy */
            //_playSpeedIndex = video->socket.open_params.num_play_speed_entries
            video->doAutoPids = false;  /* already got the PIDs */
        }
        else{
            params.tts_params.auto_detect = _cfg->getBool(Config::TTS_AUTO_DETECT);
            if(video->timestamp_enabled || params.tts_params.auto_detect) {
                int patTimeout, pmtTimeout;
                params.tts_params.pacing_max_error = _cfg->getInt(Config::TTS_PACING_MAX_ERROR);
                params.tts_params.init_buf_depth = _cfg->getInt(Config::TTS_INIT_BUF_DEPTH);
                params.tts_params.min_buf_depth = _cfg->getInt(Config::TTS_MIN_BUF_DEPTH);
                params.tts_params.max_buf_depth = _cfg->getInt(Config::TTS_MAX_BUF_DEPTH);
                params.tts_params.max_clock_mismatch = _cfg->getInt(Config::TTS_MAX_CLOCK_MISMATCH);
                /* increase PAT timeout for TTS/PCR pacing sessions to allow time for network buffer to fill */
                tsPsi_getTimeout(&patTimeout, &pmtTimeout);
                tsPsi_setTimeout(PLAYBACK_IP_PAT_TIMEOUT, pmtTimeout);
            }

            if(video->timestamp_enabled && !params.tts_params.auto_detect) {
                /* definitely a TTS stream */
                params.timestamp_active = true;
                params.timestamp_enabled = true;
            }
            else {
/* TODO: add support for PCR pacing */
#if 0
                /* could be a TTS or TS */
                if(_cfg->getBool(Config::IP_PCR_PACING)) {
                    /* PCR pacing now uses tts for pacing clock control: this is configured
                       by setting timestamp_enabled=false and timestamp_active=true (it's just
                       that the timestamp is now PCR and it's software paced ;-) */
                    params.timestamp_active = true;
                    params.timestamp_enabled = false; /* may get overridden by auto-detect */
                    BDBG_WRN(("TTS/PCR pacing enabled!"));
                }
                else
#endif
                {
                    params.timestamp_active = false;
                    params.timestamp_enabled = false; /* may get overridden by auto-detect */
                    BDBG_WRN(("PCR pacing not enabled, will default to mode: \"live playback\" for TS streams!"));
                }
            }
            /* dma: PR#17733 for auto PIDs parsing */
            if (video->doAutoPids == true)
            {        /* dma */
                /* do auto pids */
                bstream_status  stream_status;
                bstream_mpeg    pb_mpeg;
                bstream_t       astream;
                int             ret;

                bstream_mpeg_init(&pb_mpeg);
                pb_mpeg.mpeg_type = bstream_mpeg_type_ts;
                pb_mpeg.video[0].pid = 1;

                if ((astream = bplayback_start(_play, _playpump, &pb_mpeg, _file, &params)) == NULL)
                {
                    BDBG_ERR(("auto PIDs: bplayback_start() failed"));
                    if (_file)
                    {
                        bplayback_socket_close(_play);
                        _file = NULL;
                    }
                    return NULL;
                }

                if (bstream_get_mpeg_parameters(astream, &stream_status))
                {
                    BERR_Code rc=BERR_TRACE(berr_invalid_state);
                    if (_file)
                    {
                        bplayback_socket_close(_play);
                        _file = NULL;
                    }
                    return NULL;
                }

                /* this check doesn't seem correct for IP paths as band can be NULL for IP */
                if (0)//(stream_status.band == NULL)
                {
                    BDBG_ERR(("can't attach band for auto PIDs"));
                    /* if can't do auto pids, can't play the stream,
                    maybe should put up error dialog box
                    */
                    bplayback_stop(_play);
                    bdecode_stop(video->adecode[video->decoderIndex]);
                    if (_file)
                    {
                        bplayback_socket_close(_play);
                        _file = NULL;
                    }
                    return NULL;
                }
                else
                {
                    /* Setup PIDs from channel info */
                    int patTimeout, pmtTimeout;
                    /* increase PAT timeout for all Live IP Channels (including HTTP/RTSP playbacks) */
                    tsPsi_getTimeout(&patTimeout, &pmtTimeout);
                    tsPsi_setTimeout(PLAYBACK_IP_PAT_TIMEOUT, pmtTimeout);

                    /* malloc this table -- its 14k */
                    CHANNEL_INFO_T  *channel_info = (CHANNEL_INFO_T *)malloc(sizeof(CHANNEL_INFO_T));
                    if (!channel_info) {
                        BERR_Code rc=BERR_TRACE(berr_out_of_memory);
                        bplayback_stop(_play);
                        bdecode_stop(video->adecode[video->decoderIndex]);
                        if (_file)
                        {
                            bplayback_socket_close(_play);
                            _file = NULL;
                        }
                        return NULL;
                    }

                    ret = tsPsi_getChannelInfo(channel_info, NULL , astream );
					/* revert to original default timout */
					tsPsi_setTimeout(patTimeout, pmtTimeout);
                    if (!ret)
                    {
                        pb_mpeg.pcr_pid = channel_info->program_info[0].pcr_pid;
                        pb_mpeg.video[0].pid = channel_info->program_info[0].video_pids[0].pid;
                        pb_mpeg.video[0].format = (bvideo_codec)channel_info->program_info[0].video_pids[0].streamType;
                        pb_mpeg.audio[0].pid = channel_info->program_info[0].audio_pids[0].pid;
                        if (channel_info->program_info[0].audio_pids[0].streamType == 4)
                            pb_mpeg.audio[0].format = baudio_format_mpeg;
                        else
                            pb_mpeg.audio[0].format = (baudio_format)channel_info->program_info[0].audio_pids[0].streamType;

                        BDBG_WRN(("auto detected PIDs:"));
                        BDBG_WRN(("   video pid  = %4d (%#x)", pb_mpeg.video[0].pid, pb_mpeg.video[0].pid));
                        BDBG_WRN(("   video type = %4d (%#x)", pb_mpeg.video[0].format, pb_mpeg.video[0].format));
                        BDBG_WRN(("   audio pid  = %4d (%#x)", pb_mpeg.audio[0].pid, pb_mpeg.audio[0].pid));
                        BDBG_WRN(("   audio type = %4d (%#x)", pb_mpeg.audio[0].format, pb_mpeg.audio[0].format));
                        BDBG_WRN(("   pcr   pid  = %4d (%#x)", pb_mpeg.pcr_pid, pb_mpeg.pcr_pid));

                        mpeg = video->mpeg = pb_mpeg;       /* structure copy */
                        video->doAutoPids = false;  /* got the PIDs now */
                        free(channel_info);
                    }
                    else
                    {
                        BDBG_ERR(("auto PIDs from streams failed!"));
                        bplayback_stop(_play);
                        bdecode_stop(video->adecode[video->decoderIndex]);
                        if (_file)
                        {
                            bplayback_socket_close(_play);
                            _file = NULL;
                        }
                        free(channel_info);
                        return NULL;
                    }

                    if(video->socket.open_params.protocol == bsettop_ip_protocol_rtsp ||
                       video->timestamp_enabled)
                        tsPsi_setTimeout(patTimeout, pmtTimeout); /* reset the PAT timeout */

                    /* stop playback so we can restart and use new pids */
                    bplayback_stop(_play);
#if B_HAS_PLAYPUMP_IP
                    bplayback_socket_close(_play);
                    _file = bplayback_socket_open(_play, &video->socket);
#endif
                }
            }
        }
        /* no timeshifting playback is needed in IP playback */
#if 0
        if (_record)
            stream = bplayback_start_timeshifting(_play, _playpump, &mpeg, _file, &params, _record);
        else
#endif
            stream = bplayback_start(_play, _playpump, &mpeg, _file, &params);

        // for record, need to get this stream to the AVStream::_digitalStream
        video->stream = stream;

        goto done;
    }
    else
    {
        _play = _playRegular;
    }

    // this returns an MString, so capture it
    indexfile = (_indexFormat == bindex_format_none) ? NULL : video->indexfilePath();

    bplayback_params_init(&params, _play);
    params.starting_index_offset = start_at_sct;
    params.index_format = video->indexFormat();
    params.callback_context = (void *)this;
    params.end_of_stream = end_of_stream;
    params.beginning_of_stream = beginning_of_stream;
    params.loopmode.end_of_stream = PlayEngine::end_of_stream(this);
    params.loopmode.beginning_of_stream = PlayEngine::beginning_of_stream(this);
    if (video->timestamp_enabled) {
        params.timestamp_enabled = true;
    }

    /* network decryption requires parser band */
    if ((_video->mpeg.encryption.type == bencryption_type_aes) ||
        (_video->mpeg.encryption.type == bencryption_type_dvb))
        params.route_through_parser_band = true;

    /* validate the mpeg information, but only change the copy */
    if (mpeg.mpeg_type == bstream_mpeg_type_es)
    {
        if (mpeg.video[0].pid && mpeg.audio[0].pid)
        {
            BDBG_ERR(("ES stream cannot have both video and audio pids set"));
            return NULL;
        }
        else if (mpeg.video[0].pid > 1)
        {
            BDBG_WRN(("ES stream needs vidpid of 0 or 1 to indicate video"));
            /* otherwise MVD chokes on it */
            mpeg.video[0].pid = 1;
        }
        else if (mpeg.audio[0].pid > 1)
        {
            BDBG_WRN(("ES stream needs audpid of 0 or 1 to indicate audio"));
            mpeg.audio[0].pid = 1;
        }
    }

    tryagain:
    if (video->isContinuousRecord && recordEngine)
    {
        _file = bfile_fifo_in_open(video->mediafilePath(), indexfile, (bfile_out_fifo_t)(recordEngine->fifo_file()));
    }
    else
    {
        _file = bplayback_file_open(video->mediafilePath(), indexfile);
    }
    if (!_file) {
        BERR_Code rc;
        if (indexfile) {
            indexfile = NULL;
            params.index_format = bindex_format_none;
            BDBG_WRN(("Failed to open with index, so try again without index"));
            goto tryagain;
        }
        rc=BERR_TRACE(berr_external_error);
        return NULL;
    }

#if B_HAS_DRM
    if ((video->mpeg.mpeg_type == bstream_mpeg_type_asf) && (video->isForPurchase))
    {
        _drm_cfg.lic_extern = NULL;
        _drm_cfg.lic_extern_ctx = NULL;
        _drm_cfg.ext_asf_scanning = 0;
        _drm_cfg.opl_cback = bdrm_util_policy_callback;
        _drm_cfg.opl_ctx = NULL;

        if ((_drm = bdrm_open(video->mediafilePath(), &_drm_cfg)) != NULL)
        {
            /* set up drm callbacks and contexts */
            _drm_cfg.lic_callback     = bdrm_util_license_callback;
            _drm_cfg.lic_callback_ctx = (void *)this;
            bdrm_init_handlers(_drm);

            /* at this point we should have a valid license to show drm content */
            /* if NOT, abort */
            if (DRMLicenseState() == bdrm_lic_failtransfer)
            {
                BDBG_WRN(("DRM license request failed!"));

                /* do clean up and return NULL to cancel playback attempt */
                if (_drm)
                {
                    bdrm_close(_drm);
                    _dcrypt = NULL;
                    _drm = NULL;
                }

                if (_file)
                {
                    bplayback_file_close(_file);
                    _file = NULL;
                }

                return NULL;
            }

            bdrm_dcrypt_instance(_drm, &_dcrypt);
            memcpy(&mpeg.encryption.key, &_dcrypt, sizeof(&_dcrypt));
        }
    }
#endif /* B_HAS_DRM */

#if B_HAS_DIVX_DRM
    if ((video->mpeg.mpeg_type == bstream_mpeg_type_avi) && (video->isForPurchase)){
        #if (BCHP_CHIP==7400) || (BCHP_CHIP==7403)
        _divx_decrypt = bdrm_decrypt_init(video->mediafilePath());
        #else
        _divx_decrypt = bdrm_decrypt_init(video->mediafilePath(), NULL, NULL);
        #endif
        memcpy(&mpeg.encryption.key, &_divx_decrypt, sizeof(_divx_decrypt));
    }
#endif

    if (_record) {
        stream = bplayback_start_timeshifting(_play, _playpump, &mpeg, _file, &params, _record);
    }
    else {
        stream = bplayback_start(_play, _playpump, &mpeg, _file, &params);

        /* modify video->mpeg */
        if (stream) {
            if (_video->mpeg.encryption.type == bencryption_type_dvb)
                bstream_set_encryption(stream, &_video->mpeg.encryption);
        }
    }

    /* If we failed once with the index, try again without an index */
    if (!stream && indexfile)
    {
        indexfile = NULL;
        params.index_format = bindex_format_none;
        bplayback_file_close(_file);
#if B_HAS_DIVX_DRM
        if (_divx_decrypt){
            bdrm_decrypt_uninit(_divx_decrypt);
            _divx_decrypt = NULL;
        }
#endif
        BDBG_WRN(("Failed to play with index, so try again without index"));
        goto tryagain;
    }

#if B_HAS_DIVX_DRM
    if ((video->mpeg.mpeg_type == bstream_mpeg_type_avi) && (video->isForPurchase) && !stream && !indexfile){
        bplayback_file_close(_file);
        video->isForPurchase = false;
        indexfile = (_indexFormat == bindex_format_none) ? NULL : video->indexfilePath();
        params.index_format = video->indexFormat();
        memset(&mpeg.encryption.key, 0, sizeof(_divx_decrypt));
        if (_divx_decrypt){
            bdrm_decrypt_uninit(_divx_decrypt);
            _divx_decrypt = NULL;
        }
        BDBG_WRN(("Failed to play with DRM, so try again without DRM"));
        goto tryagain;
    }
#endif

done:
    if (stream)
    {
        // TODO: _current.lastpos should be correct initially
        _current.lastpos = 10;
        FIRE(FileChanged);
        setState(bplayback_state_playing);
        _timer.start(TIMER_INTERVAL, MTimer::eForever);
    }
    else
    {
        _record = NULL;
        _video = NULL;
#if B_HAS_DIVX_DRM
        if (_divx_decrypt){
          bdrm_decrypt_uninit(_divx_decrypt);
          _divx_decrypt = NULL;
        }
#endif
        BDBG_ERR(("bplayback_start failed"));
    }
    _stream = stream;
    return _stream;
}

int PlayEngine::stop(unsigned long *currentIndex)
{
    if (!isActive()) return 0;

    // stop the timer first so that no state update will happen
    _timer.stop();
    _current.firstpos = _current.lastpos = _current.pos = 0;

    /* get the current position if requested */
    if (currentIndex)
    {
        bplayback_status status;
        if (!bplayback_get_status(_play, &status))
            *currentIndex = status.position.index_offset;
    }


    /* If playback succeeds but decode fails, then bplayback_stop is going
    to fail. So don't check the return code. Just assert the stopped state. */
    bplayback_stop(_play);
    _stream = NULL;

    if (_video->isIP)
        bplayback_socket_close(_play);
    else
        bplayback_file_close(_file);
    _file = NULL;

#if B_HAS_DRM
    if (_drm)
    {
        bdrm_close(_drm);
        _dcrypt = NULL;
        _drm = NULL;
    }
#endif

#if B_HAS_DIVX_DRM
    if (_divx_decrypt){
        bdrm_decrypt_uninit(_divx_decrypt);
        _divx_decrypt = NULL;
    }
#endif

    setState(bplayback_state_stopped);
    return 0;
}

int PlayEngine::play()
{
    BDBG_MSG(("Playengine: play: state %d", state()));
    if (state() == bplayback_state_playing)
        return 0;
    else if (isActive())
    {
        int rc;
        /* We have to put the HTTP FLAG HERE */
        if ((_video->socket.open_params.protocol == bsettop_ip_protocol_http) &&
            (state() != bplayback_state_frameadvance)) {
            BDBG_WRN(("Sending Play Speed Command to server"));
            if (http_set_pvr_speed((void*)this, _playIP, _video->socket.open_params.ip_addr, NETWORK_PVR_PORT, 1)) {
                /* No PVR server */
                _playSpeedIndex = 0;
            }
        }
        rc = bplayback_play(_play);
        if (!rc)
            setState(bplayback_state_playing);
        _fastTrickMode = 0;
        _trickModeRate = 1;
        return rc;
    }
    else
        return -1;
}

int PlayEngine::pause()
{
    if (state() == bplayback_state_paused) {
        return play();
    }
    else if (isActive()) {
        int rc;
        if ((_video->socket.open_params.protocol == bsettop_ip_protocol_http) &&
                (_video->socket.open_params.num_play_speed_entries == 1))
            /* if server doesn't support trickmodes, return */
            return play();
        rc = bplayback_pause(_play);
        if (!rc)
            setState(bplayback_state_paused);
        return rc;
    }
    else
        return -1;
}

static const char *g_trickmode_str[bplayback_player_mode_max] = {
    "Play",
    "I",
    "Skip B",
    "IP",
    "Skip P",
    "BCM",
    "GOP",
    "ByRate",
    "TimeSkip"
};

void trick_getLabel(const bplayback_trickmode_params *params, MString &result)
{
    const char *str = g_trickmode_str[params->mode];
    char buf[30];

    /* only display it as slow motion if there's no mode_modifier. otherwise
    we're just slowing down a fast trick mode. */
    if (params->mode_modifier <= 1 && params->mode_modifier >= -1 &&
        params->slow_motion_rate > 1)
    {
        if (params->mode_modifier < 0)
            sprintf(buf, "-1/%d %s", params->slow_motion_rate, str);
        else
            sprintf(buf, "1/%d %s", params->slow_motion_rate, str);
    }
    else if (params->mode == bplayback_player_mode_i ||
        params->mode == bplayback_player_mode_brcm)
    {
        sprintf(buf, "%d %s", params->mode_modifier, str);
    }
    else if (params->mode == bplayback_player_mode_normal && params->decode_mode != bplaypump_decode_mode_all)
    {
        /* decoder skip modes */
        switch (params->decode_mode) {
        case bplaypump_decode_mode_ip: sprintf(buf, "Play IP"); break;
        case bplaypump_decode_mode_i: sprintf(buf, "Play I"); break;
        default: sprintf(buf, "Unknown"); break;
        }
    }
    else if (params->mode == bplayback_player_mode_normal && params->mode_modifier > 1)
    {
        sprintf(buf, "Play %dx", params->mode_modifier);
    }
    else if (params->mode == bplayback_player_mode_by_gop)
    {
        sprintf(buf, "%d GOP", params->mode_modifier>0?params->gop_skip_count:-params->gop_skip_count);
    }
    else if (params->mode == bplayback_player_mode_by_rate)
    {
        /* we're bumping up against the limits of BPLAYBACK_NORMALPLAY_SPEED. 1/100 is the slowest we can go. STC trick modes can go slower. */
        if (params->mode_modifier > 0 && params->mode_modifier < 30) {
            sprintf(buf, "%0.2fx", (float)params->mode_modifier/BPLAYBACK_NORMALPLAY_SPEED);
        }
        else if (params->mode_modifier % BPLAYBACK_NORMALPLAY_SPEED) {
            sprintf(buf, "%0.1fx", (float)params->mode_modifier/BPLAYBACK_NORMALPLAY_SPEED);
        }
        else {
            if (params->mode_modifier > 0 && params->mode_modifier < BPLAYBACK_NORMALPLAY_SPEED) {
                sprintf(buf, "1/%dx", BPLAYBACK_NORMALPLAY_SPEED/params->mode_modifier);
            }
            else if (params->mode_modifier < 0 && -params->mode_modifier < BPLAYBACK_NORMALPLAY_SPEED) {
                sprintf(buf, "-1/%dx", BPLAYBACK_NORMALPLAY_SPEED/-params->mode_modifier);
            }
            else {
                sprintf(buf, "%dx", params->mode_modifier/BPLAYBACK_NORMALPLAY_SPEED);
            }
        }
    }
    else
    {
        sprintf(buf, "%s", str);
    }

    result = buf;
}

/* convert linear brutus rate into logrithmic brutus rate */
static int b_convert_to_lograte(int rate)
{
    bool neg = rate < 0;
    if (neg) rate *= -1;
    switch (rate) {
    case 1: rate = 1; break;
    case 2: rate = 2; break;
    case 3: rate = 5; break;
    case 4: rate = 10; break;
    case 5: rate = 25; break;
    case 6: rate = 50; break;
    case 7: rate = 100; break;
    default:rate = 200; break; /* max */
    }
    if (neg) rate *= -1;
    return rate;
}

int PlayEngine::trickModeHttp( int rate){
    if (!isActive()) return -1;

    int lastrate = _trickModeRate;
    _trickModeRate = rate;
    int rc = 0;
    bplayback_trickmode_params trickmode;
    int frameRepeat = _video->socket.open_params.http_frame_repeat;

    bplayback_trickmode_params_init(&trickmode);
    trickmode.mode_modifier = rate;
    if(rate >= _video->socket.open_params.http_min_iframe_speed)
        trickmode.slow_motion_rate = frameRepeat;
    else if(rate <= -_video->socket.open_params.http_min_iframe_speed)
        trickmode.slow_motion_rate = frameRepeat;
    else
        trickmode.slow_motion_rate = 1;

    BDBG_WRN((">>>>> rate %d=> mode=%d, modifier=%d, slow=%d", rate, trickmode.mode, trickmode.mode_modifier, trickmode.slow_motion_rate));
    rc = setTrickMode(&trickmode);
    if (rc) {
        _trickModeRate = lastrate;
    }
    return rc;
}

int PlayEngine::trickMode(int fastTrickMode, int rate)
{
    if (!isActive()) return -1;

    int lastrate = _trickModeRate;
    _trickModeRate = rate;
    int rc = 0;
    bplayback_trickmode_params trickmode;
    bool require_brcm_trick_modes = false;
    bool hostonly = false;
    bool iframe_only = false;

    if (_video->isIP )
    { /* need to do IP the old way */
        bvideo_codec video_codec = _video->mpeg.video[0].format;

        if (rate == 0 || rate == 1)
            return play();

        bplayback_trickmode_params_init(&trickmode);
        if (_video->isHits || _cfg->getBool(Config::FORCE_BCM_TRICK_MODES))
        {
            require_brcm_trick_modes = true;
        }

        // Broadcom trick modes only allowed for MPEG video.
        // Check for other codecs, no video, audio only, or app option
        if (!_video->mpeg.video[0].pid ||
            (video_codec != bvideo_codec_mpeg2 && video_codec != bvideo_codec_mpeg1) ||
            !_cfg->getBool(Config::VIDEODECODE_ENABLED) ||
            _cfg->getBool(Config::FORCE_HOST_TRICK_MODES))
        {
            hostonly = true;
        }

        /* PR 18352 allow skip B and IP  */
        if (video_codec == bvideo_codec_h264 &&
            !_cfg->getBool("ALLOW_ALL_AVC_TRICK_MODES"))
        {
            iframe_only = true;
        }
        else
        {
            /* This is permanent code */
            iframe_only = _cfg->getBool(Config::FORCE_IFRAME_TRICK_MODES);
        }

        if (iframe_only)
        {
            hostonly = true;
        }

        if (require_brcm_trick_modes)
        {
            trickmode.mode = bplayback_player_mode_brcm;
            trickmode.mode_modifier = b_convert_to_lograte(rate);
        }
        else if (iframe_only && rate > 0)
        {
            /* forward I frame only. handle reverse I frame only as hostonly futher down. */
            if (fastTrickMode)
            {
                trickmode.mode_modifier = rate-1;
                trickmode.mode = bplayback_player_mode_i;
            }
            else
            {
                trickmode.slow_motion_rate = rate;
            }
        }
        else
        {
            int lookuprate;

            if (fastTrickMode)
            {
                if (_video->mpeg.mpeg_type == bstream_mpeg_type_asf)
                {
                    /* use a quasi-logrithmic scale */
                    rate = b_convert_to_lograte(rate);
                }
                lookuprate = BPLAYBACK_NORMALPLAY_SPEED * rate;
            }
            else
            {
                /* slow */
                if (rate < 0)
                    lookuprate = BPLAYBACK_NORMALPLAY_SPEED / (rate-1);
                else
                    lookuprate = BPLAYBACK_NORMALPLAY_SPEED / rate;
            }

            if (rate < 0 && hostonly)
            {
                /* hostonly reverse is iframe only */
                trickmode.mode = bplayback_player_mode_i;
                if (fastTrickMode)
                {
                    trickmode.mode_modifier = rate;
                    trickmode.slow_motion_rate = 1;
                }
                else
                {
                    trickmode.mode_modifier = -1;
                    trickmode.slow_motion_rate = (-rate) + 1;
                }
            }
            else
            {
                /* This is the default case. */
                rc = bplayback_get_trickmode_by_rate(_play, lookuprate, &trickmode);

                /* we can ignore hostonly because bplayback_get_trickmode_by_rate never uses
                brcm trick modes in forward direction. the following assert verifies it. */
                if (rate > 0)
                {
                    BDBG_ASSERT(trickmode.mode != bplayback_player_mode_brcm);
                }
            }
        }
    }
    else
    {
/********************************
* This is the non-IP code.
****/

        bvideo_codec video_codec = _video->mpeg.video[0].format;
        bool require_by_rate_trick_modes = video()->indexfile == video()->mediafile;


        if (rate == 0 || rate == 1)
            return play();

        bplayback_trickmode_params_init(&trickmode);

        if (_video->isHits || _cfg->getBool(Config::FORCE_BCM_TRICK_MODES)) {
            require_brcm_trick_modes = true;
        }

        // Broadcom trick modes only allowed for MPEG video.
        // Check for other codecs, no video, audio only, or app option
        if (!_video->mpeg.video[0].pid ||
            (video_codec != bvideo_codec_mpeg2 && video_codec != bvideo_codec_mpeg1) ||
            !_cfg->getBool(Config::VIDEODECODE_ENABLED) ||
            _cfg->getBool(Config::FORCE_HOST_TRICK_MODES))
        {
            hostonly = true;
        }

        if (video_codec == bvideo_codec_h264 && !_cfg->getBool("ALLOW_ALL_AVC_TRICK_MODES") && _video->mpeg.mpeg_type == bstream_mpeg_type_ts) {
            /* other exceptions apply too */
            hostonly = true;
        }

        iframe_only = _cfg->getBool(Config::FORCE_IFRAME_TRICK_MODES);
        if (iframe_only) {
            hostonly = true;
        }

        if (video()->indexfile.isNull() && fastTrickMode)
        {
            /* fast trick modes with no index */

            if (rate < 1) {
                BDBG_ERR(("no reverse trick modes with no index"));
                return -1;
            }

            if (B_HAS_PB120_TRICKMODES()) {
                switch(rate)
                {
                case 2:
                    trickmode.decode_mode = bplaypump_decode_mode_all;
                    trickmode.mode = bplayback_player_mode_by_rate;
                    trickmode.mode_modifier = 120; /* PB120 STC trick mode */
                    break;
                case 3:
                    trickmode.decode_mode = bplaypump_decode_mode_ip; /* decoder skip */
                    trickmode.mode = bplayback_player_mode_normal;
                    trickmode.mode_modifier = 1;
                    break;
                case 4:
                    trickmode.decode_mode = bplaypump_decode_mode_i; /* decoder skip */
                    trickmode.mode = bplayback_player_mode_normal;
                    trickmode.mode_modifier = 1;
                    break;
                default:
                    BDBG_WRN(("no faster trick mode with no index"));
                    _trickModeRate = 4;
                    return -1;
                }
            }
            else {
                switch(rate)
                {
                case 2:
                    trickmode.decode_mode = bplaypump_decode_mode_ip; /* decoder skip */
                    trickmode.mode = bplayback_player_mode_normal;
                    trickmode.mode_modifier = 1;
                    break;
                case 3:
                    trickmode.decode_mode = bplaypump_decode_mode_i; /* decoder skip */
                    trickmode.mode = bplayback_player_mode_normal;
                    trickmode.mode_modifier = 1;
                    break;
                default:
                    BDBG_WRN(("no faster trick mode with no index"));
                    _trickModeRate = 3;
                    return -1;
                }
            }
        }
        else if (!fastTrickMode && rate > 1)
        {
            /* slow forward - this is standard for all codecs */
            if (B_HAS_PB120_TRICKMODES()) {
                /* STC trick mode */
                trickmode.mode = bplayback_player_mode_by_rate;
                if (rate == 2) {
                    trickmode.mode_modifier = 80; /* 0.8x STC trick mode */
                }
                else {
                    trickmode.mode_modifier = BPLAYBACK_NORMALPLAY_SPEED/(rate - 1);
                }
            }
            else {
                /* decoder trick modes */
                trickmode.slow_motion_rate = rate;
            }
        }
        else if (_cfg->getBool("DQT_ENABLED") && (rate < 0) && fastTrickMode &&
                 (_video->mpeg.mpeg_type == bstream_mpeg_type_ts) &&
                 (video_codec == bvideo_codec_h264) &&
                 (!require_by_rate_trick_modes)) {
        int dqt_pics_per_gop;

        dqt_pics_per_gop = _cfg->getInt("DQT_PICS_PER_GOP");
        if (!dqt_pics_per_gop) dqt_pics_per_gop = 10;

        trickmode.mode = bplayback_player_mode_by_gop;
        trickmode.mode_modifier = rate*100 + dqt_pics_per_gop*(rate<0?-1:1);
        trickmode.gop_skip_count = rate;
        }
        else if (require_brcm_trick_modes)
        {
            trickmode.mode = bplayback_player_mode_brcm;
            trickmode.mode_modifier = rate;
#if 0
            /* PR 8867 - we need some decoder slow motion with brcm trick modes.
            The value needs to give the decoder enough time for worst case
            decoding (reference frame + all the P's in a GOP) */
            if (rate > 2 || rate < -2)
                trickmode.slow_motion_rate = 4;
#endif
        }
        else if (require_by_rate_trick_modes)
        {
            if (fastTrickMode) {
                /* use a quasi-logrithmic scale for fast forward/reverse */
                if (B_HAS_PB120_TRICKMODES()) {
                    if (rate == 2)
                        rate = 120;
                    else
                        rate = b_convert_to_lograte(rate>0?rate-1:rate) * BPLAYBACK_NORMALPLAY_SPEED;
                }
                else {
                   rate = b_convert_to_lograte(rate) * BPLAYBACK_NORMALPLAY_SPEED;
               }
            }
            else {
                rate = BPLAYBACK_NORMALPLAY_SPEED/(rate<0?rate-1:rate);
            }
            trickmode.mode = bplayback_player_mode_by_rate;
            trickmode.mode_modifier = rate;
        }
        else if (!fastTrickMode)
        {
            /* slow reverse */
            BDBG_ASSERT(rate < 0);
            trickmode.mode = hostonly?bplayback_player_mode_i:bplayback_player_mode_brcm;
            trickmode.mode_modifier = -1;
            trickmode.slow_motion_rate = (-rate) + 1;
        }
        else if (iframe_only)
        {
            /* forward I frame only. handle reverse I frame only as hostonly futher down. */
            trickmode.mode = bplayback_player_mode_i;
            trickmode.mode_modifier = b_convert_to_lograte(rate>0?rate-1:rate);
        }
        else if (hostonly)
        {
            if (B_HAS_PB120_TRICKMODES()) {
                /* for AVC, use 1.2x, then I-frame only */
                if (rate == 2) {
                    trickmode.mode = bplayback_player_mode_by_rate;
                    trickmode.mode_modifier = 120;
                }
                else {
                    trickmode.mode = bplayback_player_mode_i;
                    trickmode.mode_modifier = b_convert_to_lograte(rate>0?rate-2:rate);
                }
            }
            else {
                trickmode.mode = bplayback_player_mode_i;
                trickmode.mode_modifier = b_convert_to_lograte(rate>0?rate-1:rate);
            }
        }
        else
        {
            if (rate == 2 && B_HAS_PB120_TRICKMODES()) {
                trickmode.mode = bplayback_player_mode_by_rate;
                trickmode.mode_modifier = 120; /* PB120 */
                trickmode.slow_motion_rate = 1;
            }
            else {
                /* This is the default case. */
                rc = bplayback_get_trickmode_by_rate(_play, BPLAYBACK_NORMALPLAY_SPEED * rate, &trickmode);

                /* we can ignore hostonly because bplayback_get_trickmode_by_rate never uses
                brcm trick modes in forward direction. the following assert verifies it. */
                if (rate > 0) {
                    BDBG_ASSERT(trickmode.mode != bplayback_player_mode_brcm);
                }
            }
        }
    }

    if (!rc) {
        if(_video->isIP && _video->socket.open_params.protocol == bsettop_ip_protocol_rtsp)
        {
            if (trickmode.mode_modifier > BPLAYBACK_NORMALPLAY_SPEED ||
                trickmode.mode_modifier <= -BPLAYBACK_NORMALPLAY_SPEED)
            {
                /* FFW/REW */
                goto settrick;
            }
            else
            {
                /* slow-motion (not supported for RTSP yet) */
                BDBG_MSG(("supressed slow-motion command"));
                _trickModeRate = lastrate;
                goto done;
            }
        }
        else
        {
            goto settrick;
        }
    }

settrick:
    _fastTrickMode = fastTrickMode;
    BDBG_MSG(("rate %d=> mode %d, modifier %d, slow %d", rate, trickmode.mode, trickmode.mode_modifier, trickmode.slow_motion_rate));
    rc = setTrickMode(&trickmode);
    if (rc) {
        _trickModeRate = lastrate;
    }

done:
    return rc;
}

int PlayEngine::setTrickMode(const bplayback_trickmode_params *trickmode_params)
{
    bresult result;

    result = bplayback_trickmode(_play, trickmode_params);
    if (!result)
    {
        if(_video->socket.open_params.protocol == bsettop_ip_protocol_http){
            char buf[30];
            sprintf( buf, "Http %dx", trickmode_params->mode_modifier);
            _trickModeState = buf;
        }
        else{
            trick_getLabel(trickmode_params, _trickModeState);
        }
        setState(bplayback_state_trickmode, trickmode_params->mode_modifier<0?-1:1);
    }
    return result?-1:0;
}

int PlayEngine::trickModeInc(int fastTrickMode, int inc)
{
    int result=0;
    int *speed = _video->socket.open_params.play_speed;
    if (!isActive()) return -1;

    /* determine what the new trickmode should be */
    int rate = _trickModeRate;
    if (_video->socket.open_params.protocol == bsettop_ip_protocol_http)
    {
        if (state() == bplayback_state_trickmode)
        {
            _playSpeedIndex += inc;
        }
        else if(state() == bplayback_state_playing)
        {
            unsigned i;

            for(i = 0; i < _video->socket.open_params.num_play_speed_entries; i++)
            {
                if(inc > 0 && speed[i] > 0)
                {
                    _playSpeedIndex = i;
                    break;
                }
                else if(inc < 0 && speed[_video->socket.open_params.num_play_speed_entries-i] < 0)
                {
                    _playSpeedIndex = _video->socket.open_params.num_play_speed_entries - i;
                    break;
                }
            }
        }

        if(_playSpeedIndex >= (int) _video->socket.open_params.num_play_speed_entries)
        {
            _playSpeedIndex = _video->socket.open_params.num_play_speed_entries - 1;
        }
        else if(_playSpeedIndex < 0)
            _playSpeedIndex = 0;

        rate = speed[_playSpeedIndex];
        if (http_set_pvr_speed((void*)this, _playIP, _video->socket.open_params.ip_addr, NETWORK_PVR_PORT,  rate) != b_ok) {
            /* No PVR server */
            _playSpeedIndex = 0;
            return -1;
        }

        result = trickModeHttp(rate);
    } else
    {
        if (state() == bplayback_state_trickmode && fastTrickMode == _fastTrickMode)
        {
            // subsequent
            rate += inc;
        }
        else
        {
            rate = inc>0?2:-1;
        }
        result = trickMode(fastTrickMode, rate);
    }

    /* set the trick mode */
    return result;
}

int PlayEngine::frameadvance(int dir)
{
    if (!isActive()) return -1;
    int rc;

    /* RTSP, supress frame-advance (not supported for RTSP) */
    rc = 0;
    if(_video->isIP && _video->socket.open_params.protocol == bsettop_ip_protocol_rtsp) {
        if(dir>0)
            trickModeInc(1, 1);
        else
            trickModeInc(1, 0);
    }
    else if (_cfg->getBool(Config::FORCE_HOST_TRICK_MODES) ||
        _cfg->getBool(Config::FORCE_IFRAME_TRICK_MODES) ||
        _video->mpeg.video[0].format == bvideo_codec_h264 ||
        _video->mpeg.video[0].format == bvideo_codec_mpeg4_part2 ||
    _video->mpeg.video[0].format == bvideo_codec_divx_311 ||
        _video->mpeg.video[0].format == bvideo_codec_vc1 ||
        _video->mpeg.video[0].format == bvideo_codec_vc1_sm)
    {
        rc = bplayback_frameadvance(_play, bplayback_player_mode_i, dir>0);
    }
    else
    {
        rc = bplayback_frameadvance(_play, bplayback_player_mode_brcm, dir>0);
    }
    if (!rc)
    {
        setState(bplayback_state_paused);
        if (dir > 0)
            _specialStateMessage = "Adv";
        else
            _specialStateMessage = "Rev";
        FIRE(SpecialStateMessage);
    }
    return rc;
}

int PlayEngine::catchup()
{
    if (!isActive()) return -1;
    return bplayback_catchup_record(_play);
}

/* as long as pos, firstpos and lastpos are scaled the same way, you can apply
any scale factor */
#define SCALE_MPEGPOS(POS) ((POS) >> 5)

void PlayEngine::onTimeout(MTimer *self)
{
    bplayback_status status;
    unsigned long current=0, firstpos=0, lastpos=0;

    if (bplayback_get_status(_play, &status))
        return;

    _current.pts = status.position.timestamp;

    /**
    This code used to be more complicated. It can and should remain simple now.
    If the pos needs to be adjusted for good UI, please do that in the UI code.
    If timestamp is unavailable, please add timestamp approximation in Settop API.
    **/
    if (!status.has_index)
    {
        /* no index, so use mpeg file data */
        current = SCALE_MPEGPOS(status.position.mpeg_file_offset);
        firstpos = 0;
        lastpos = SCALE_MPEGPOS(status.mpeg_file_size);
    }
    else
    {
        current = status.position.timestamp;
        firstpos = status.first_timestamp;
        lastpos = status.last_timestamp;
    }
    BDBG_MSG(("pos %d in %d...%d (from %s)", current, firstpos, lastpos, status.has_index?"index":"data"));

    /* Getting state doesn't block, and doing it in the monitor thread leads
       to all kinds of problems. So do it here. */
    if (status.state == bplayback_state_frameadvance)
        status.state = bplayback_state_paused;
    int direction = status.trickmode_params.mode_modifier < 0?-1:1;
    if (status.state != _state || direction != _direction)
        setState(status.state, direction);

    if ((int)current != _current.pos)
    {
        _current.pos = current;
        FIRE(PosChanged);
    }
    if ((int)firstpos != _current.firstpos || (int)lastpos != _current.lastpos)
    {
        _current.firstpos = firstpos;
        _current.lastpos = lastpos;
        FIRE(FileChanged);
    }
}

void PlayEngine::setState(bplayback_state s, int direction)
{
    // Changes in trick mode speed need to get through
    // However, frame advance shouldn't reassert the pause state
    if (s != _state || _state == bplayback_state_trickmode || direction != _direction)
    {
        BDBG_MSG(("PlayEngine::setState %d ==> %d", _state, s));
        _state = s;
        _direction = direction;
        _isStreamEnded = false; /* reset this on state change */
        FIRE(StateChanged);
    }
}

FIRE_EV_METHOD(PlayEngine, StateChanged)
FIRE_EV_METHOD(PlayEngine, PosChanged)
FIRE_EV_METHOD(PlayEngine, FileChanged)
FIRE_EV_METHOD(PlayEngine, SpecialStateMessage)
#if B_HAS_DRM
FIRE_EV_METHOD(PlayEngine, DRMLicenseStateChanged)
FIRE_EV_METHOD(PlayEngine, DRMHTTPStateChanged)
#endif

int PlayEngine::skip(int msec)
{
    int rc;
    if (!isActive())
        return -1;
    if (_indexFormat == bindex_format_bcm)
    {
        /* This assumes that bcmindexer is timestamping in millisecond units, which is
        the default. */
        rc = bplayback_goto_timestamp(_play, msec, SEEK_CUR);
    }
    else
    {
        /* if no index, we can also make absolute MPEG jumps. This hardcoded value
        is correct for 4 Mbps. */
        rc = bplayback_goto_mpeg_file_offset(_play, msec * 500, SEEK_CUR);
    }
    if (!rc)
    {
        int secs = msec/1000;
        if (secs == 0)
            secs = 1;
        _specialStateMessage = (secs>0?"+":"") + MString(secs) + " sec" +
                               (secs!=1?"s":"");
        FIRE(SpecialStateMessage);
    }
    return rc;
}

const char *PlayEngine::stateStr() const
{
    switch (_state)
    {
    case bplayback_state_stopped:
        return "Stop";
    case bplayback_state_playing:
        return "Play";
    case bplayback_state_paused:
    case bplayback_state_frameadvance:
        return "Pause";
    case bplayback_state_trickmode:
        return _trickModeState;
    default:
        BDBG_ERR(("Invalid state: %d", _state));
        BDBG_ASSERT(false);
    }
    return NULL;
}

int PlayEngine::stopTimeshifting()
{
    if (!_record) return -1;
    if (bplayback_stop_timeshifting(_play)) return -1;
    _record = NULL;
    FIRE(FileChanged);
    return 0;
}

int PlayEngine::gotoTimestamp(unsigned timestamp)
{
    return bplayback_goto_timestamp(_play, timestamp, SEEK_SET);
}
