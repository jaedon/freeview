/***************************************************************************
 *     Copyright (c) 2002-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: channelmgr.cpp $
 * $brcm_Revision: 98 $
 * $brcm_Date: 7/30/12 4:30p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/channelmgr/channelmgr.cpp $
 * 
 * 98   7/30/12 4:30p mward
 * SW7425-3254: Deallocate tuner objects when entering S3. Fix typo.
 * 
 * 97   6/27/12 2:40p jessem
 * SW7425-3254: Deallocate tuner objects when entering S3.
 * 
 * 96   3/26/12 10:43a erickson
 * SW7425-2664: remove unnecessary BCHP_CHIP
 * 
 * 95   3/21/12 12:12p erickson
 * SW7425-2664: consolidate force_sds BCHP_CHIP list
 *
 * 94   3/19/12 6:09p erickson
 * SW7425-2664: remove board/chip logic. rely on
 * NEXUS_Platform_GetConfiguration via bsettop_config.
 *
 * 93   3/13/12 12:41p mphillip
 * SW7425-2030: Merge 4528 tuner initialization to main
 *
 * SATIP_7425_4528/2   7/11/11 11:53a vishk
 * SWSATFE-132: Add nexus support for 4528 satellite frontend for 97425
 * SATIPSW platform.
 *
 * SATIP_7425_4528/1   6/29/11 3:44p lihong
 * SW7425-798: setup build system for SATIP
 *
 * 92   2/28/12 9:03p mward
 * SW7435-42:  Do not use tuner 4 or 5 on 7425/7435 SV board.  Those are
 * disabled by pinmux for use by debug UARTs.
 *
 * 91   2/23/12 6:20p gmohile
 * SW7425-2473 : Fix frontend initialization
 *
 * 90   12/21/11 3:53p mward
 * SW7435-7:  Support Brutus build on (new, 7435-based) platform 97435.
 *
 * 89   12/20/11 1:44a xhuang
 * SW7552-58: remove unnecessary board define when open frontend
 *
 * 88   11/16/11 9:29a erickson
 * SW7425-1747: remove unnecessary BCM_BOARD logic and allow loadTuners()
 * to populate brutus frontends based on bconfig
 *
 * 87   10/26/11 1:48p katrep
 * SW7429-1:add support for 7429
 *
 * 86   9/26/11 6:41p randyjew
 * SW7344-140:Add 3123 support to 7418SFF_H
 *
 * 85   6/5/11 8:43p vishk
 * SW7231-32: Implement the Nexus code to support the 3461 frontend chip.
 * Load tuners for 7231 platform.
 *
 * 84   6/1/11 8:49p xhuang
 * SW7552-34: Bringup 7552 frontend
 *
 * 83   5/27/11 9:36p vishk
 * SW7231-32: Implement the Nexus code to support the 3461 frontend chip.
 * Implement brutus support for the downstream core.
 *
 * 82   5/23/11 3:34p jrubio
 * SW7346-213: add untune functions
 *
 * 81   5/19/11 10:31p xhuang
 * SW7552-27: Add 7552 support
 *
 * 80   3/9/11 5:49p katrep
 * SW7231-30:add qam tuner
 *
 * 80   3/9/11 5:41p katrep
 * SW7231-30:add qam tuner
 *
 * 79   3/7/11 10:49a randyjew
 * SW7344-9:Add7418 support
 *
 * 78   2/16/11 5:00p xhuang
 * SW7358-34: Add 7358/7552 support
 *
 * 77   2/15/11 3:02p nickh
 * SW7422-213: Add SVC/MVC support
 *
 * 76   2/15/11 2:32p randyjew
 * SW7358-34: Fix compile warning
 *
 * 76   2/15/11 2:30p randyjew
 * SW7358-34: Fix compile warning
 *
 * 75   2/15/11 10:42a xhuang
 * SW7358-34: Add 7358/7552 support
 *
 * 74   1/21/11 5:18p nitinb
 * SW7550-236: Merge into main branch
 *
 * REFSW_97550_236/1   1/19/11 5:25p nitinb
 * SW7550-236: For 7550, do not wait for OFDM lock
 *
 * 73   12/22/10 4:35p katrep
 * SW7231-25:add initial support for 7230
 *
 * 72   12/15/10 2:59p jrubio
 * SW7344-9: fix 7344 typo
 *
 * 71   12/15/10 10:40a jrubio
 * SW7344-9: add 7344/7346 support
 *
 * 70   12/9/10 5:57p katrep
 * SW7231-4:add initial 7231 support
 *
 * 69   11/4/10 2:16p hongtaoz
 * SW7425-49: added 7425 support;
 *
 * 68   10/21/10 4:57p nickh
 * SW7422-80: Add 7422 support for Cable and Satellite tuner support
 *
 * 67   8/20/10 2:33p randyjew
 * SW7208-21: Check for duplicate pids being passed into settop api from
 * tspsimgr.
 *
 * 66   6/7/10 2:34p erickson
 * SW7125-369: fix typo in case statement
 *
 * 65   5/11/10 9:05a mward
 * SW7125-395: 97019 has only one broadcast tuner.
 *
 * 64   5/6/10 2:05p mward
 * SW7125-369:  Don't check for bvideo_codec_divx_311 in streamType.  Enum
 * value doesn't fit in 8-bit streamType, and this format isn't streamed,
 * only played.
 *
 * 63   4/23/10 2:53p nickh
 * SW7420-709: Add 7420 cable frontend support to Brutus
 *
 * 62   4/19/10 3:30p nickh
 * SW7420-709: Add DBS support
 *
 * 61   1/28/10 10:42a nitinb
 * SW7550-204: DVB-C signal can NOT be locked
 *
 * 60   1/18/10 5:51p mward
 * SW7125-131:  Allow 3 QAM tuners on 97125.
 *
 * 59   12/9/09 5:25p randyjew
 * SW7468-6: Add 7468/7208 support
 *
 * 58   12/9/09 11:55a gmohile
 * SW7408-1 : Add 7408 support
 *
 * 57   12/9/09 1:03p rjain
 * SW7550-112: merge from 7550 branch back to main
 *

 * Refsw_7550/2   11/23/09 2:48p chengli
 * SW7550-64: load tuners for 7550
 *
 * Refsw_7550/1   11/5/09 11:24a nitinb
 * sw7550-64: Initial brutus build for 7550
 *
 * 56   8/20/09 7:33p mward
 * PR55545: Support 7125.
 *
 * 55   8/19/09 11:52a jrubio
 * PR55232: change 7340/7342 #defines
 *
 * 54   8/19/09 10:55a jrubio
 * PR55232: add 7342/7340 support
 *
 * 53   8/17/09 5:24p jrubio
 * PR55882: update with 97340
 *
 * 52   8/14/09 7:09p jrubio
 * PR55232: add 7340/7342
 *
 * 51   7/16/09 5:01p katrep
 * PR56373: Gloval variables need to be set to correct state inorder to
 * correctly wakeup from powerstandby
 *
 * 50   7/2/09 10:57a erickson
 * PR56558: defer frontend init and channel scan until after the GUI is up
 *
 * 49   6/25/09 3:34p katrep
 * PR55809: Add support for PCM audio in AVI files
 *
 * 48   3/30/09 10:24a erickson
 * PR44451: downgrade WRN to MSG
 *
 * 47   3/10/09 11:12a agin
 * PR44451: If not forcing an SDSx, use normal tuner allocation.
 *
 * 46   2/4/09 10:30a jrubio
 * PR51629: add 7336 support
 *
 * 45   12/9/08 10:33a gmohile
 * PR 43467 : Add DRA support
 *
 * dra/1   8/7/08 4:29p gmohile
 * PR 43467 : Added dra support
 *
 * 44   7/21/08 4:32p katrep
 * PR44143: Set the LOAS as default configuration for AAC Plus audio
 * format.
 *
 * 43   7/1/08 11:16a agin
 * PR44451: Add ability to force SDS0/SDS1/SDS2 tuners as the default
 * tuner.
 *
 * 42   2/14/08 5:15p katrep
 * PR39180: OFDM doesnt support lock/unlock callback added forced channel
 * scan.
 *
 * 41   2/1/08 4:59p katrep
 * PR39180: Added support for ofdm/DVB-T THD tuner
 *
 * 40   1/25/08 3:20p katrep
 * PR38429: Added support for AVS
 *
 * 39   9/19/07 12:29p jrubio
 * PR34613: add new function to do a total re-reading of the channelmap
 * and remote server channel map
 *
 * 38   6/5/07 2:41p erickson
 * PR31728: add baudio_format_aac_plus_loas
 *
 * 37   4/25/07 4:35p mward
 * PR29776:  Except for streamer, don't create stream in tune().  Let it
 * wait until lock, or decode may start before lock.
 *
 * 37   4/25/07 4:33p mward
 * PR29776:  Except for streamer, don't create stream in tune().  Let it
 * wait until lock, or decode of bad data may start before lock.
 *
 * 36   1/29/07 5:30p gmohile
 * PR 25704: add divx 3.11 support
 *
 * 35   1/29/07 11:18a ssood
 * PR24918: Channel Map Redesign to make its access Thread-safe
 *
 * 34   12/14/06 10:39a erickson
 * PR24806: must handle baudio_format_mp3 as separate case now
 *
 * 33   11/30/06 6:15p ssood
 * PR26252: IP STB SW unification w/ Generic Brutus release
 *
 * 32   11/21/06 4:41p erickson
 * PR24374: only use pids specified by num_xxxxx_pids
 *
 * 31   11/6/06 10:53a ssood
 * PR25072: Unable to play Video error while recording a SAP channel
 *
 * 30   10/30/06 4:48p erickson
 * PR25109: added DivX support
 *
 * 29   10/5/06 10:25a vsilyaev
 * PR 23826: Use dedicated playpump channels
 *
 * 28   10/4/06 4:01p vsilyaev
 * PR 23826: Added RTSP support for AVC/AAC decoder over RTP
 *
 * 27   9/28/06 6:59p arbisman
 * PR24289: Back-out changes
 *
 * 23   9/8/06 5:20p tokushig
 * PR20685: add support for simple/main profile in brutus
 *
 * SanDiego_DRM_ASF/1   9/7/06 5:08p tokushig
 * add handling for simple/main asf profile
 *
 * 22   7/25/06 5:23p jjordan
 * PR22906: Integrate liveMedia open-source for RTP and RTCP support
 *
 * 21   6/19/06 3:47p erickson
 * PR19526: added ChannelMapEntry::setType to force initialization of
 * Settop API frontend params structures when type changes
 *
 * 20   1/20/06 12:42p erickson
 * PR18840: added LPCM_HDDVD and LPCM_BLURAY to brutus
 *
 * 19   10/13/05 5:24p erickson
 * PR17541: added new audio and video codec support for 7401 and beyond
 *
 * 18   9/2/05 3:55p erickson
 * PR16639: refactored b_set_bstream_mpeg so that it can set every field
 * in bstream_mpeg (including mpeg_type)
 *
 * 17   8/24/05 6:11p arbisman
 * PR16678: Add support for WinCE
 *
 * 16   7/14/05 5:13p erickson
 * PR16044: now that bstream_open can be called on the same band more than
 * once, we don't have to manage streamers as resources
 *
 * 15   6/29/05 9:03a jjordan
 * PR16042: Add support for "IP channels"
 *
 * 14   6/23/05 11:27a mphillip
 * PR15943: Change from groupid to tuner so that SDS/VSB are not
 * mistakenly associated with the QAM tuners
 *
 * 13   6/9/05 10:07a erickson
 * PR15234: added debug
 *
 * 12   5/25/05 11:10a erickson
 * PR14721: don't call into AVManager at lock_callback time. just keep
 * track and let it call in at task time.
 *
 * 11   5/23/05 3:30p erickson
 * PR15518: unlock before sending callback. this code may have a race
 * condition, but fixing it opens us up to deadlock. this needs to be
 * refactored. maybe a single mutex for both AVManager and
 * ChannelManager.
 *
 * 10   4/22/05 4:46p erickson
 * PR14721: lock must acquire mutex before looking up TunerEntry
 *
 * 9   4/19/05 3:46p erickson
 * PR14701: make digital subchannel support standard for all platforms
 *
 * 8   4/14/05 10:55a erickson
 * PR14701: added more debug
 *
 * 7   4/14/05 10:31a erickson
 * PR14593: check failed untune
 *
 * 6   4/9/05 12:21a erickson
 * PR14701: added SUBCHANNEL_SUPPORT, which involves allowing a channel to
 * tune but not lock, then allowing scan to happen per-channel later
 *
 * 5   4/4/05 1:30p erickson
 * PR13823: removed B_HAS_XXXX frontend #defines. use bconfig for runtime
 * check if really needed.
 *
 * 4   3/28/05 4:58p erickson
 * PR11759: added multiple linein support
 *
 * 3   3/24/05 1:21p erickson
 * PR14593: removed old platform
 *
 * 2   2/17/05 1:47p erickson
 * PR9497: fixed compilation warning for vxworks
 *
 * 1   2/7/05 7:58p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 *
 * Irvine_BSEAVSW_Devel/72   2/4/05 10:48a erickson
 * PR12728: cleanup for -pedantic warnings and errors
 *
 * Irvine_BSEAVSW_Devel/71   12/29/04 7:24p marcusk
 * PR13701: Updated with support for H.264 Stream Type (0xB1)
 *
 * Irvine_BSEAVSW_Devel/70   11/19/04 4:03p erickson
 * PR13304: 3d comb enabled by default and shown on ui
 *
 * Irvine_BSEAVSW_Devel/69   11/9/04 7:07p vsilyaev
 * PR 13097: Added support for video stream type.
 *
 * Irvine_BSEAVSW_Devel/68   10/15/04 10:49a erickson
 * PR13014: need to be able to reset the Settings
 *
 * Irvine_BSEAVSW_Devel/67   9/15/04 1:53p erickson
 * PR9683: convert from B_CHIP to BCHP_CHIP in brutus
 *
 * Irvine_BSEAVSW_Devel/66   8/17/04 4:41p erickson
 * PR11770: removed brutus inputband support. added streamer as a
 * registered frontend resource, with a mapping between index number and
 * settop api id.
 *
 * Irvine_BSEAVSW_Devel/65   8/9/04 3:48p erickson
 * PR12104: extended SDS to handle DSS_PES and DSS_ES, along with STREAMER
 * and INPUTBAND
 *
 * Irvine_BSEAVSW_Devel/64   8/3/04 3:14p erickson
 * PR11770: check out streamer bands, not just tuners
 *
 * Irvine_BSEAVSW_Devel/63   7/22/04 12:27p erickson
 * PR11682: use float multiplication to avoid rounding error
 *
 * Irvine_BSEAVSW_Devel/62   7/21/04 4:14p erickson
 * PR11682: improved brutus status msg
 *
 * Irvine_BSEAVSW_Devel/61   7/21/04 12:10p erickson
 * PR11682: adapted to tspsimgr changes to better support PMT capture
 * without hack
 *
 * Irvine_BSEAVSW_Devel/60   7/20/04 12:10p erickson
 * PR11682: fixed for streamer
 *
 * Irvine_BSEAVSW_Devel/59   7/20/04 11:55a erickson
 * PR11682: enable/disable channel for POD must be done after decode is
 * started
 *
 * Irvine_BSEAVSW_Devel/58   7/20/04 10:38a erickson
 * PR11682: fixed streamer tune, more pod work
 *
 ***************************************************************************/
#ifndef _WIN32_WCE
    #include <sys/types.h>
    #include <errno.h>
    #include <unistd.h>
    #include <assert.h>
#else
    #include "bstd_defs.h" // int64_t
#endif

#include "channelmgr.h"

#include "mstringlist.h"
#include <bsettop.h>
#include <bstd.h>
#include <bkni.h>

#if B_HAS_RTSP
#include "bsettop_playback_rtsp.h"
#endif

BDBG_MODULE(channelmgr);

#if B_CHANNELMGR_FORCE_SDS
extern int force_sds;
#endif

#define MAX_TUNERS 20
static btuner_t g_tuners[MAX_TUNERS];
static unsigned total_tuners = 0;

static struct {
    bobject_t obj;
    btuner_t tuner;
} b_tunerobjs[MAX_TUNERS];
static int total_tunerobjs = 0;
static int tuner_entry_index=0;


ChannelManager::ChannelManager(ChannelMap *map, ChannelScan *scan)
{
    BKNI_CreateMutex(&_mutex);
    _scan = scan;
    _map = map;
    _isPopulated = false;

    memset(&_settings, 0, sizeof(_settings));
    _settings.videoDecodeEnabled = true;
    _settings.audioDecodeEnabled = true;
    _settings.overrideBand = -1;
    _settings.combFilterEnabled = true;

    _scan->setTuneCallback(tuneCallback, untuneCallback, this);
}

ChannelManager::~ChannelManager()
{
    unsigned i;

    // don't sync because we're out of here anyway
    if (totalCheckOuts())
        BDBG_ERR(("Destroying ChannelManager with %d AVStream(s) checked out", totalCheckOuts()));
#if 0
    MListItr<TunerEntry> itr(&_tuners);
    for (TunerEntry *t = itr.first(); t; t = itr.next())
    {
        BDBG_MSG(("index =%d ,total=%d",t->index,_tuners.total()));
        _tuners.remove(t);
    }
#endif

    for (i=0;i<total_tuners;i++) {
        if (g_tuners[i]) {
            btuner_close(g_tuners[i]);
            g_tuners[i] = NULL;
        }
    }
    /* reset the total numbber of tuners */
    total_tuners=0;
    total_tunerobjs=0;
    tuner_entry_index=0;
    BKNI_DestroyMutex(_mutex);
}

void ChannelManager::setSettings(const ChannelManager::Settings *settings)
{
    _settings = *settings;
    /* Cannot retune automatically. App needs to stop decode, untune, retune, start decode */
}

void ChannelManager::addTuner(btuner_stream_type type, btuner_t tuner, int groupId)
{
    TunerEntry *t = new TunerEntry(tuner, type, groupId);
    t->mgr = this;
    BDBG_MSG(("addTuner(type %d, %p, group %d", type, tuner, groupId));

    BKNI_AcquireMutex(_mutex);
    _tuners.add(t);
    BKNI_ReleaseMutex(_mutex);
}

void ChannelManager::addStreamer(int streamer_index, int streamer_settopapi_id)
{
    TunerEntry *t = new TunerEntry(NULL, btuner_streamer_stream);
    t->streamer.index = streamer_index;
    t->streamer.settopapi_id = streamer_settopapi_id ;
    t->mgr = this;

    BKNI_AcquireMutex(_mutex);
    _tuners.add(t);
    BKNI_ReleaseMutex(_mutex);
}

void ChannelManager::addIP(void)
{
    BKNI_AcquireMutex(_mutex);
    TunerEntry *t = new TunerEntry(NULL, btuner_ip_stream);
    _tuners.add(t);
    BKNI_ReleaseMutex(_mutex);
}

#if B_HAS_RTSP
void ChannelManager::addRTSP(unsigned index)
{
    BKNI_AcquireMutex(_mutex);
    TunerEntry *t = new TunerEntry(NULL, btuner_rtsp_stream);
    t->rtsp.pump_index = index;
    t->rtsp.pump = NULL;
    t->rtsp.play = bplayback_rtsp_open();
    if (t->rtsp.play) {
        _tuners.add(t);
        BDBG_MSG(("Added RTSP tuner: %#lx", (unsigned long)t));
    } else {
        delete t;
    }
    BKNI_ReleaseMutex(_mutex);
}
#endif

void ChannelManager::addLinein(btuner_linein_t linein, int index)
{
    _linein[index] = linein;
}

btuner_linein_t ChannelManager::getLinein(int index)
{
    return _linein[index];
}

void ChannelManager::addEncode(btuner_t analogTuner, bencode_t encode,
    bool requireEncodeForDecode)
{
    AnalogSupport *as = new AnalogSupport;
    as->analogTuner = analogTuner;
    as->encode = encode;
    as->requireEncodeForDecode = requireEncodeForDecode;

    if (as->requireEncodeForDecode && !as->encode) {
        BDBG_ERR(("Encoder required, but no encoder given."));
        delete as;
        return;
    }

    BKNI_AcquireMutex(_mutex);
    _analogSupport.add(as);
    BKNI_ReleaseMutex(_mutex);
}

int ChannelManager::getEncode(btuner_t analogTuner, bencode_t *p_encode,
    bool *p_requireEncodeForDecode) const
{
    int rc = -1;
    MListItr<AnalogSupport> itr(&_analogSupport);

    BKNI_AcquireMutex(_mutex);
    for (AnalogSupport *as = itr.first(); as; as = itr.next()) {
        if (as->analogTuner == analogTuner) {
            if (p_encode)
                *p_encode = as->encode;
            if (p_requireEncodeForDecode)
                *p_requireEncodeForDecode = as->requireEncodeForDecode;
            rc = 0;
            break;
        }
    }
    BKNI_ReleaseMutex(_mutex);
    return rc;
}
int b_check_duplicate_audiopid(int pid, bstream_mpeg *mpeg, int limit)
{

    if(limit ==0)
        return 0;

    for(int i=0; i<limit; i++){

         if(pid == mpeg->audio[i].pid)
         return -1;
    }
    return 0;
}
int b_check_duplicate_videopid(int pid, bstream_mpeg *mpeg, int limit)
{
       if(limit ==0)
        return 0;

    for(int i=0; i<limit; i++){

         if(pid == mpeg->video[i].pid)
         return -1;
    }
    return 0;
}
void b_set_bstream_mpeg(bstream_mpeg *mpeg, const ChannelMap::Channel *channel)
{
    int limit;
    bstream_mpeg_init(mpeg);

    if (channel->validProgramInfo == false)
        return;

    // now copy relevant data
    limit = channel->program.num_audio_pids;
    if (limit > BSETTOP_MAX_PROGRAMS)
        limit = BSETTOP_MAX_PROGRAMS;
    for (int i=0;i<limit;i++) {
     /* Check for duplicate audio pid insertion before adding pid */
     if(b_check_duplicate_audiopid(channel->program.audio_pids[i].pid, mpeg, i))
        continue;

        mpeg->audio[i].pid = channel->program.audio_pids[i].pid;

        /* Make sure it's a recognized audio format */
        switch (channel->program.audio_pids[i].streamType) {
        case baudio_format_aac:
        case baudio_format_aac_plus:
        /* case baudio_format_aac_plus_loas: */ /* same stream id as aac plus */
        case baudio_format_aac_plus_adts:
        case baudio_format_ac3:
        case baudio_format_ac3_plus:
        case baudio_format_dts:
        case baudio_format_lpcm_hddvd:
        case baudio_format_lpcm_bluray:
        case baudio_format_mp3:
        case baudio_format_avs:
        case baudio_format_dra:
        case baudio_format_pcm:
            mpeg->audio[i].format = (baudio_format)channel->program.audio_pids[i].streamType;
            break;
        default: /* valid values include 0, 3, 4 */
            mpeg->audio[i].format = baudio_format_mpeg;
            break;
        }
    }

    limit = channel->program.num_video_pids;
    if (limit > BSETTOP_MAX_PROGRAMS)
        limit = BSETTOP_MAX_PROGRAMS;
    for (int i=0;i<limit;i++) {
        /* Check for duplicate video pid insertion before adding pid */
     if(b_check_duplicate_videopid(channel->program.video_pids[i].pid, mpeg,i))
        continue;

        mpeg->video[i].pid = channel->program.video_pids[i].pid;
        switch(channel->program.video_pids[i].streamType) {
        case bvideo_codec_mpeg1:
        case bvideo_codec_mpeg2:
        case bvideo_codec_h263:
        case bvideo_codec_h264:
        case bvideo_codec_h264_svc:
        case bvideo_codec_h264_mvc:
        case bvideo_codec_vc1:
        case bvideo_codec_vc1_sm:
        case bvideo_codec_mpeg4_part2:
        case bvideo_codec_avs:
            mpeg->video[i].format = (bvideo_codec)channel->program.video_pids[i].streamType;
            break;
        default:
            mpeg->video[i].format = bvideo_codec_mpeg2;
            break;
        }
    }
    mpeg->pcr_pid = channel->program.pcr_pid;
    mpeg->mpeg_type = channel->entry.mpegType();
}

int ChannelManager::tune(int chnum, TuneResults *results)
{
    ChannelMap::Channel channel;
    int rc = 0;
    TunerEntry *t = NULL;

    BDBG_MSG(("tune %d", chnum));
    BKNI_Memset(results, 0, sizeof(*results));

    /* It's important that everyone leaves this function through the done:
    label so that this mutex is released. */
    BKNI_AcquireMutex(_mutex);

    if (map()->getChannel(chnum, &channel)) {
        rc = -1;
        goto done;
    }

    if (channel.entry.getType() == btuner_analog_stream) {
        results->stream = tuneAnalog(&channel.entry, &t, &results->linein);
        if (!results->stream) {
            t = NULL;
            rc = -1;
            goto done;
        }
        goto success;
    }
#if B_HAS_RTSP
    if (channel.entry.getType() == btuner_rtsp_stream) {
        bplaypump_params playpump_params;
        bplayback_file_t source;
        bplayback_params playback_params;
        bstream_mpeg mpeg;

        t = checkOutTuner(channel.entry.getType(), 0);
        if (t==NULL) { goto done; }
        t->rtsp.source = bplayback_rtsp_open_source(t->rtsp.play, channel.entry.rtsp.url);
        if (t->rtsp.source==NULL) {
            goto done;
        }
        t->rtsp.pump = bplaypump_open(B_ID(t->rtsp.pump_index), NULL);
        if (t->rtsp.pump==NULL) {
            bplayback_file_close(t->rtsp.source);
            goto done;
        }
        bplayback_params_init(&playback_params, t->rtsp.play);
        bstream_mpeg_init(&mpeg);
        results->stream = bplayback_start(t->rtsp.play, t->rtsp.pump, &mpeg, t->rtsp.source, &playback_params);
        if (results->stream==NULL) {
            bplayback_file_close(t->rtsp.source);
            bplaypump_close(t->rtsp.pump);
            t->rtsp.pump = NULL;
        }
        results->band = NULL;
        goto success;
    }
#endif /* B_HAS_RTSP */

    // tune frontend. if this is a stream, we'll get a band
    // but no TunerEntry.
    results->band = tuneDigital(&channel.entry, &t);

    /* if this is an IP stream, we'll get a TunerEntry, but no band */
    if(channel.entry.getType() == btuner_ip_stream) {
        goto done;
    }

    if (!results->band) {
        BDBG_ERR(("Unable to tune digital"));
        rc = -1;
        goto done;
    }

    /* We may tune but not have any program info. The app will handle this. */
    if (channel.validProgramInfo == false) {
        goto success;
    }

    if (channel.entry.getType() == btuner_streamer_stream) {
        results->stream = createStream(results->band, &channel);
        if (!results->stream) {
            rc = -1;
            goto done;
        }
    }

success:
    /* We've successfully tuned, so keep track of it */
    BDBG_ASSERT(t);
    finishCheckOut(t, results->stream, results->band);
    /* store the content on this tuner */
    t->chnum = chnum;
    t->entry = channel.entry;   //copy
    results->tuner = t->tuner;
    rc = 0;

done:
    if (rc && t) {
        /* We've failed, so check in the tuner */
        checkIn(t);
    }

    BKNI_ReleaseMutex(_mutex);
    return rc;
}

bstream_t ChannelManager::createStream(bband_t band, ChannelMap::Channel *channel)
{
    bstream_mpeg mpeg;

    if (channel->validProgramInfo == false) return NULL;

    b_set_bstream_mpeg(&mpeg, channel);

    // apply special settings
    if (!_settings.audioDecodeEnabled)
        memset(mpeg.audio, 0, sizeof(mpeg.audio));
    if (!_settings.videoDecodeEnabled)
        memset(mpeg.video, 0, sizeof(mpeg.video));

    /* you've got to have something */
    if (!mpeg.video[0].pid && !mpeg.pcr_pid && !mpeg.audio[0].pid) {
        return NULL;
    }

    /* Create the stream. This stream must be deallocated using bstream_close, otherwise
    there is a leak. This is done in untune. */
    return bstream_open(band, &mpeg);
}

void ChannelManager::untune(bstream_t stream)
{
    /* IP channels do not have streams, so check for stream before closing */
    if(!stream)
        return;

    BKNI_AcquireMutex(_mutex);
    TunerEntry *t = findCheckOut(stream, NULL);
    BDBG_MSG(("untune stream %p", stream));
    if (t) {
#if B_HAS_RTSP
        if(t->rtsp.pump) {
            bplayback_stop(t->rtsp.play);
            bplaypump_close(t->rtsp.pump);
            t->rtsp.pump = NULL;
        }
#endif
       checkIn(t);
    }
    else {
        BDBG_ERR(("Failed untune: stream %p", stream));
    }
    bstream_close(stream);
    BKNI_ReleaseMutex(_mutex);
}

void ChannelManager::untune(bband_t band)
{
    BKNI_AcquireMutex(_mutex);
    TunerEntry *t = findCheckOut(NULL, band);
    BDBG_MSG(("untune band %p", band));
    if (t) {
        checkIn(t);
    }
    else {
        BDBG_ERR(("Failed untune: band %p", band));
    }
    BKNI_ReleaseMutex(_mutex);
}


/* static */
bband_t ChannelManager::tuneCallback(ChannelMap::ChannelMapEntry *entry, void *data)
{
    TunerEntry *tuner;
    ChannelManager *mgr = (ChannelManager*)data;

    BKNI_AcquireMutex(mgr->_mutex);
    bband_t band = mgr->tuneDigital(entry, &tuner);
    if (band)
        mgr->finishCheckOut(tuner, NULL, band);
    BKNI_ReleaseMutex(mgr->_mutex);
    return band;
}

/* static */
void ChannelManager::untuneCallback(bband_t band, void *data)
{
    ChannelManager *mgr = (ChannelManager*)data;

    BKNI_AcquireMutex(mgr->_mutex);
    TunerEntry *t = mgr->findCheckOut(NULL, band);
    BDBG_MSG(("untuneCallback band %p", band));
    if (t)
        mgr->checkIn(t);
    BKNI_ReleaseMutex(mgr->_mutex);
}

/* static */
void ChannelManager::lock_callback(void *data)
{
    TunerEntry *t = (TunerEntry *)data;
    /* must sync with chanmgr. this makes sure that the tune function is all the
    way done. this works because the Settop API's lock_callback comes from
    idle context, so we can be slow about it. */
    BKNI_AcquireMutex(t->mgr->_mutex);
    BDBG_MSG(("lock_callback TunerEntry=%p", t));
    t->got_lock_callback = true;
    BKNI_ReleaseMutex(t->mgr->_mutex);
}

bband_t ChannelManager::getLockChange()
{
    bband_t band = NULL;
    BKNI_AcquireMutex(_mutex);

    MListItr<TunerEntry> itr(&_tuners);
    TunerEntry *t;
    for (t = itr.first(); t; t = itr.next()) {
        if (t->got_lock_callback) {
            t->got_lock_callback = false;
            band = t->band;
            break;
        }
    }
    BKNI_ReleaseMutex(_mutex);
    return band;
}

/***************************************************

All functions beyond this point are internal and can assume to be
already synchronized.

****************************************************/

//TODO: sync of channelmap, channelscan

int ChannelManager::totalCheckOuts() const
{
    MListItr<TunerEntry> itr(&_tuners);
    int count = 0;
    for (TunerEntry *t = itr.first(); t; t = itr.next())
        if (t->locked)
            count++;
    return count;
}

bband_t ChannelManager::tuneDigital(const ChannelMap::ChannelMapEntry *entry,
    TunerEntry **p_tuner)
{
    bband_t band;
    btuner_t tuner = NULL;
    int streamer_index = -1;

    BDBG_MSG(("tuneDigital %f, type %d", entry->freq, entry->getType()));

    if (entry->getType() == btuner_streamer_stream) {
        streamer_index = entry->streamer.index;
    }

    *p_tuner = checkOutTuner(entry->getType(), streamer_index);
    if (!*p_tuner)
        return NULL;
    BDBG_MSG(("TunerEntry=%p", *p_tuner));
    tuner = (*p_tuner)->tuner;

    switch (entry->getType()) {
    case btuner_streamer_stream:
        band = bstreamer_attach(B_ID((*p_tuner)->streamer.settopapi_id),
            entry->streamer.mpeg_type);
        break;
    case btuner_vsb_stream:
        {
        btuner_vsb_params vsb = entry->vsb;
        vsb.wait_for_lock = false;
        vsb.lock_callback = lock_callback;
        vsb.callback_context = *p_tuner;
        band = btuner_tune_vsb(tuner, (unsigned)(entry->freq * 1000000), &vsb);
        }
        break;
    case btuner_ofdm_stream:
        {
        btuner_ofdm_params ofdm = entry->ofdm;
        ofdm.wait_for_lock = false;
        ofdm.lock_callback = lock_callback;
        ofdm.callback_context = *p_tuner;
        band = btuner_tune_ofdm(tuner, (unsigned)(entry->freq * 1000000), &ofdm);
        }
        break;
    case btuner_sds_stream:
        {
        btuner_sds_params sds = entry->sds.params;
        sds.wait_for_lock = false;
        sds.lock_callback = lock_callback;
        sds.callback_context = *p_tuner;
        band = btuner_tune_sds(tuner, (unsigned)(entry->freq * 1000000), &sds);
        }
        break;
    case btuner_qam_stream:
        {
        btuner_qam_params qam = entry->qam;
        qam.wait_for_lock = false;
        qam.lock_callback = lock_callback;
        qam.callback_context = *p_tuner;
        band = btuner_tune_qam(tuner, (unsigned)(entry->freq * 1000000), &qam);
        }
        break;
    case btuner_ip_stream:
        /* no action, IP channel "tuning" is handled via playback */
        BDBG_MSG(("Tuning to IP address: %s, port: %d\n", entry->ip.params.open_params.ip_addr, entry->ip.params.open_params.port));
        return NULL;
    default:
        return NULL;
    }

    if (_settings.overrideBand != -1) {
        printf("override band %d\n", _settings.overrideBand);
        band = bstreamer_attach(B_ID(100 + _settings.overrideBand), bstream_mpeg_type_ts);
    }

    if (!band && *p_tuner) {
        /* unable to tune */
        BDBG_MSG(("unable to tune"));
        checkIn(*p_tuner);
        *p_tuner = NULL;
    }

    /* Caller is responsible to checkOut(tuner, stream) when stream is obtained.
    If stream cannot be obtained from band, caller must checkIn tuner. */
    return band;
}

bstream_t ChannelManager::tuneAnalog(const ChannelMap::ChannelMapEntry *entry,
    TunerEntry **p_tuner, btuner_linein_t *p_linein)
{
    bstream_t api_stream = NULL;
    btuner_t tuner;
    btuner_analog_params params = entry->analog.params;

    BDBG_ASSERT(entry->getType() == btuner_analog_stream);

    *p_tuner = checkOutTuner(entry->getType());
    if (!*p_tuner)
        goto error;
    tuner = (*p_tuner)->tuner;

    params.btsc_mode = _settings.sapEnabled?
        boutput_rf_btsc_mode_sap:boutput_rf_btsc_mode_stereo;
    params.comb_filter = _settings.combFilterEnabled;

    if (entry->analog.linein >= 0) {
        btuner_linein_t linein = getLinein(entry->analog.linein);
        if (!linein) {
            BDBG_ERR(("Linein %d not supported", entry->analog.linein));
            goto error;
        }
        api_stream = btuner_tune_linein(tuner, linein, &params);
        if (p_linein) *p_linein = linein;
    }
    else {
        api_stream = btuner_tune_rf(tuner, (unsigned)(entry->freq * 1000000.0),
            &params);
    }
    if (!api_stream)
        goto error;

    /* Caller is responsible to checkOutTuner(tuner, stream) */
    return api_stream;

error:
    if (*p_tuner) {
        BDBG_MSG(("unable to tune analog"));
        checkIn(*p_tuner);
    }
    return NULL;
}

void ChannelManager::addStreamToTunerEntry(bband_t band, bstream_t stream)
{
    MListItr<TunerEntry> itr(&_tuners);
    for (TunerEntry *t = itr.first(); t; t = itr.next()) {
        if (t->locked && t->band == band) {
            t->stream = stream;
            break;
        }
    }
}

ChannelManager::TunerEntry *ChannelManager::checkOutTuner(btuner_stream_type type,
    int streamer_index)
{
    TunerEntry *selection = NULL;
    bool foundtype = false;

    MListItr<TunerEntry> itr(&_tuners);
    for (TunerEntry *t = itr.first(); t; t = itr.next()) {
        if (type == t->type) {
            foundtype = true; /* This tells us that it's at least possible */

            if (type == btuner_streamer_stream) {
                if (t->streamer.index != streamer_index)
                    continue;
                /* no need to check out */
                selection = t;
                break;
            }
            /* no need to checkOut IP tuner, so don't check for lock */
            if(type == btuner_ip_stream)
            {
                selection = t;
                break;
            }

            if (!t->locked) {
                /* tentatively select it */
                selection = t;

                /* Now check the lock state of all other tuners with the same groupId */
                if (t->groupId != -1)
                for (TunerEntry *t2 = _tuners.first(); t2; t2 = _tuners.next()) {
                    if (t2 != t &&
                        t2->tuner == t->tuner &&
                        t2->locked)
                    {
                        /* The groupId has excluded it. Keep trying. */
                        selection = NULL;
                        break;
                    }
                }

                /* We've got a tuner */
                if (selection)
                {
                    #if B_CHANNELMGR_FORCE_SDS
                    if (force_sds == -1)
                    {
                        BDBG_WRN(("use first available tuner, groupId=%d\n", t->groupId));
                    }
                    else
                    {
                        if (t->groupId != force_sds)
                        {
                            continue;
                        }
                        else
                        {
                            BDBG_WRN(("use forced tuner, groupId=%d\n", t->groupId));
                        }
                    }
                    #endif
                    break;
                }
            }
        }
    }

    if (!foundtype) {
        BDBG_WRN(("Tuner type not supported"));
        return NULL;
    }
    else if (!selection) {
        BDBG_WRN(("Unable to checkout tuner type"));
        return NULL;
    }
    else {
        selection->locked = true;
        selection->chnum = -1;
        selection->stream = NULL;
        selection->band = NULL;
        BDBG_MSG(("checkOutTuner[%d], %p", selection->index, selection));
        return selection;
    }
}

void ChannelManager::finishCheckOut(TunerEntry *t,
    bstream_t stream, bband_t band)
{
    BDBG_ASSERT(t->locked); // already checked out using checkOutTuner()
    BDBG_ASSERT(stream || band);
    t->stream = stream;
    t->band = band;
    BDBG_MSG(("finishCheckOut TunerEntry=%p,stream=%p,band=%p,total=%d", t, stream, band, totalCheckOuts()));
}

ChannelManager::TunerEntry *ChannelManager::findCheckOut(bstream_t stream, bband_t band)
{
    MListItr<TunerEntry> itr(&_tuners);
    TunerEntry *t;
    BDBG_MSG(("findCheckOut s=%p,b=%p", stream, band));
    for (t = itr.first(); t; t = itr.next()) {
        BDBG_MSG(("  s=%p,b=%p: locked=%d", t->stream, t->band, t->locked));
        if (t->locked) {
            if ((stream && t->stream == stream) ||
                (band && t->band == band))
                return t;
        }
    }
    return NULL;
}

void ChannelManager::checkIn(TunerEntry *t)
{
    BDBG_ASSERT(t->locked);
    BDBG_MSG(("checkIn[%d] TunerEntry=%p,stream=%p,band=%p", t->index, t, t->stream, t->band));
    t->locked = false;
    t->stream = NULL;
    t->band = NULL;
    if (t->type!=btuner_streamer_stream) {
        btuner_untune(t->tuner);
    }
    BDBG_MSG(("Total checkouts: %d", totalCheckOuts()));
}


int ChannelManager::psiScan(const  char * filename , const char * ipv4)
{
    /* Perform the channelmap-based scan */
    if(_scan)
        return _scan->read(filename, ipv4);
    else
        return -1;
}

int ChannelManager::scan()
{
    /* Perform the channelmap-based scan */
    if (_scan)
        return _scan->scan();
    else
        return -1;
}

ChannelManager::TunerEntry::TunerEntry(btuner_t t, btuner_stream_type ty, int g)
{
    index = tuner_entry_index++;
    tuner = t;
    streamer.index = -1;
    streamer.settopapi_id = -1;
    type = ty;
    groupId = g;
    locked = false;
    got_lock_callback = false;
}

int ChannelManager::populateFrontEnd()
{
    btuner_t tuner;

    /* Add streamers using bconfig. If your platform needs a streamer or input band, please update
    bconfig and don't hardcode in this file. */
    for (unsigned i=0;i<bconfig->resources.streamers.total;i++)
        addStreamer(i, i);
    /* Enable all input bands for debug mode */
    for (unsigned i=0;i<bconfig->resources.input_bands.total;i++)
        addStreamer(100+i, 100+i);

    /* for all other chips & boards, assume that bsettop_config.c is working. it is working, if NEXUS_Platform_GetConfiguration is working...and it is. */
    BDBG_MSG(("Total tuners: SDS=%d, VSB=%d, QAM=%d, OFDM=%d, input_bands=%d",
              bconfig->resources.sds.total, bconfig->resources.vsb.total, bconfig->resources.qam.total,bconfig->resources.ofdm.total,bconfig->resources.input_bands.total));
    loadTuners(&bconfig->resources.sds, btuner_sds_stream);
    loadTuners(&bconfig->resources.vsb, btuner_vsb_stream);
    loadTuners(&bconfig->resources.qam, btuner_qam_stream);
    loadTuners(&bconfig->resources.ofdm, btuner_ofdm_stream);

    /* add "IP Tuner" to channel manager */
    addIP();

#if B_HAS_RTSP
    /* add "IP Tuner" to channel manager */
    for (unsigned int i=bconfig->resources.decode.total;i<bconfig->resources.playback.total;i++) {
        addRTSP(i);
    }
#endif

    _isPopulated = true;

    return 0;
}


int ChannelManager::depopulateFrontEnd()
{
    unsigned i;
    int group_index;

    if(totalCheckOuts()) {
	BDBG_WRN(("Cannot enter standby when tuners are still Checked out"));
	return -1;
    }

    MListItr<TunerEntry> itr(&_tuners);
    for (TunerEntry *t = itr.first(); t; t = itr.next()) {
	BKNI_AcquireMutex(_mutex);
	_tuners.remove(t);
	 BKNI_ReleaseMutex(_mutex);
    }

    for (i=0;i<total_tuners;i++) {
        if (g_tuners[i]) {
            btuner_close(g_tuners[i]);
            g_tuners[i] = NULL;
        }
    }

    for(group_index=total_tunerobjs;group_index>=0;group_index--){
	b_tunerobjs[group_index].obj = NULL;
	b_tunerobjs[total_tunerobjs].tuner = NULL;
	total_tunerobjs--;
    }

    _isPopulated = false;
}

/**
Open all tuners associated with a bconfig_obj_list. Add them
to the channelManager according to type. Do not open a btuner_t
more than once, so use a cache to associate obj and tuner.
Note: if we process the tuners with daughter cards first, then
plug in cards will come first IF it is a duplicate of an onboard
tuner (i.e. qam, sds, etc.).
**/
void ChannelManager::loadTuners(const bconfig_obj_list *obj_list, btuner_stream_type type)
{
    for (unsigned i=0; i<obj_list->total; i++)
    {
        int index = _settings.forceUseOfDaughterCards ? obj_list->total-1 - i : i;
        bobject_t obj = obj_list->objects[index];
        btuner_t tuner = NULL;
        int group_index;

        /* look if we've already opened the tuner */
        for (group_index=0; group_index<total_tunerobjs; group_index++)
            if (b_tunerobjs[group_index].obj == obj) {
                tuner = b_tunerobjs[group_index].tuner;
                break;
            }

        if (!tuner) {
            tuner = btuner_open(obj);
            if (!tuner)
                continue;
            g_tuners[total_tuners++] = tuner;
            if (total_tunerobjs == MAX_TUNERS)
                continue;
            b_tunerobjs[total_tunerobjs].obj = obj;
            b_tunerobjs[total_tunerobjs].tuner = tuner;
            group_index = total_tunerobjs;
            total_tunerobjs++;
        }

        addTuner(type, tuner, group_index);
    }
}
