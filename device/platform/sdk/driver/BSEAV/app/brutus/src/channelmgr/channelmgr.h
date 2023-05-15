/***************************************************************************
 *     Copyright (c) 2002-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: channelmgr.h $
 * $brcm_Revision: 18 $
 * $brcm_Date: 2/23/12 6:20p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/channelmgr/channelmgr.h $
 * 
 * 18   2/23/12 6:20p gmohile
 * SW7425-2473 : Fix frontend initialization
 * 
 * 17   8/20/10 2:32p randyjew
 * SW7208-21:check for duplicate pids being pased into settop api from
 * tspsimgr.
 * 
 * 16   7/2/09 10:57a erickson
 * PR56558: defer frontend init and channel scan until after the GUI is up
 *
 * 15   9/19/07 12:30p jrubio
 * PR34613: add new psiScan function for total rescan
 *
 * 14   1/29/07 11:18a ssood
 * PR24918: Channel Map Redesign to make its access Thread-safe
 *
 * 13   11/30/06 6:15p ssood
 * PR26252: IP STB SW unification w/ Generic Brutus release
 *
 * 12   11/6/06 10:53a ssood
 * PR25072: Unable to play Video error while recording a SAP channel
 *
 * 11   10/4/06 4:01p vsilyaev
 * PR 23826: Added RTSP support for AVC/AAC decoder over RTP
 *
 * 10   9/28/06 6:59p arbisman
 * PR24289: Back-out changes
 *
 * 7   9/2/05 3:56p erickson
 * PR16639: refactored b_set_bstream_mpeg so that it can set every field
 * in bstream_mpeg (including mpeg_type)
 *
 * 6   6/29/05 9:04a jjordan
 * PR16042: Add support for "IP channels"
 *
 * 5   5/25/05 11:10a erickson
 * PR14721: don't call into AVManager at lock_callback time. just keep
 * track and let it call in at task time.
 *
 * 4   4/9/05 12:21a erickson
 * PR14701: added SUBCHANNEL_SUPPORT, which involves allowing a channel to
 * tune but not lock, then allowing scan to happen per-channel later
 *
 * 3   3/28/05 4:58p erickson
 * PR11759: added multiple linein support
 *
 * 2   2/17/05 1:47p erickson
 * PR9497: fixed compilation warning for vxworks
 *
 * 1   2/7/05 7:58p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 *
 * Irvine_BSEAVSW_Devel/29   11/4/04 5:21p erickson
 * PR11770: added comments
 *
 * Irvine_BSEAVSW_Devel/28   10/15/04 10:50a erickson
 * PR13014: need to be able to reset the Settings
 *
 * Irvine_BSEAVSW_Devel/27   8/17/04 4:41p erickson
 * PR11770: removed brutus inputband support. added streamer as a
 * registered frontend resource, with a mapping between index number and
 * settop api id.
 *
 * Irvine_BSEAVSW_Devel/26   8/3/04 3:14p erickson
 * PR11770: check out streamer bands, not just tuners
 *
 * Irvine_BSEAVSW_Devel/25   7/21/04 4:14p erickson
 * PR11682: improved brutus status msg
 *
 * Irvine_BSEAVSW_Devel/24   7/21/04 10:09a erickson
 * PR11682: move channels.txt support out of ChannelMap and into
 * PSIChannelScan
 *
 * Irvine_BSEAVSW_Devel/23   7/20/04 11:55a erickson
 * PR11682: enable/disable channel for POD must be done after decode is
 * started
 *
 * Irvine_BSEAVSW_Devel/22   7/15/04 5:17p erickson
 * PR11682: added PodChannelScan support
 *
 * Irvine_BSEAVSW_Devel/21   7/14/04 2:34p erickson
 * PR11682: added initial Pod support into channelmgr
 *
 * Irvine_BSEAVSW_Devel/20   5/10/04 4:51p erickson
 * PR10832: added requiredEncodeForDecode flag instead of hardcoding it
 * into AVManager
 *
 * Irvine_BSEAVSW_Devel/19   5/5/04 12:59p erickson
 * PR10832: fixed deadlock when doing linein
 *
 * Irvine_BSEAVSW_Devel/18   5/4/04 10:16a erickson
 * PR10832: consolidated checkout procedure into two-stage process with
 * the same data structure
 *
 * Irvine_BSEAVSW_Devel/17   4/30/04 5:27p erickson
 * PR10832: more debug related to new channelmgr/avmanager
 *
 * Irvine_BSEAVSW_Devel/16   4/30/04 12:05p erickson
 * PR10832: rework of brutus to support single AVManager and
 * ChannelManager for system
 *
 * Irvine_BSEAVSW_Devel/15   3/4/04 1:39p erickson
 * PR9465: completed refactoring of channelmgr by moving channel string
 * generation into Brutus
 *
 * Irvine_BSEAVSW_Devel/14   2/13/04 12:05p erickson
 * PR9465: updated channelmgr naming and comments
 *
 * Irvine_BSEAVSW_Devel/13   2/12/04 7:34p erickson
 * PR9465: reworked ChannelManager to support multiple tuners per
 * downstream
 *
 * Irvine_BSEAVSW_Devel/12   12/16/03 12:25a erickson
 * PR8731: POD ui work
 *
 * Irvine_BSEAVSW_Devel/11   12/11/03 12:31a erickson
 * PR8949: isDigital() takes a channel #, not a tuner now
 *
 * Irvine_BSEAVSW_Devel/10   10/31/03 11:57a erickson
 * settop api changes after internal review
 *
 * Irvine_BSEAVSW_Devel/9   9/12/03 3:29p erickson
 * reworked setChannel deferment
 * fixed pip mode logic with currentSource/currentDecode changes
 *
 * Irvine_BSEAVSW_Devel/8   8/22/03 5:20p erickson
 * venom2 settop api work
 *
 * Irvine_BSEAVSW_Devel/7   8/21/03 9:43a erickson
 * venom2 settop api work
 *
 * Irvine_BSEAVSW_Devel/6   8/18/03 11:03a erickson
 * initial conversion of brutus to settop api, still in progress
 *
 ****************************************************************************/
#ifndef CHANNELMGR_H__
#define CHANNELMGR_H__

#include <stdlib.h>
#include "mstringlist.h"
#include "mstring.h"
#include "media.h"
#include "channelmap.h"
#include "channelscan.h"
#include "bsettop.h"
#include <bstd.h>
#include <bkni_multi.h>

#define B_MAX_DOWNSTREAMS 2

/**
Summary:
ChannelManager provides a frontend interface for settop applications, including
tuner management and a scanned channel list.

Description:
The inputs to ChannelManager are a "channel map" and a set of tuners (and associated
resources for analog tuners). You also must pass in an instance of ChannelScan which
contains an application-specific method of converting the channel map into the
channel list.

The output is a "channel list" and the ability to tune a channel and get the stream
produced by the tuner command.

ChannelManager does not keep the application state (i.e. current channel) therefore
it doesn't have any channel up/down methods. This should be done in the application.

The ChannelManager can be used as a global resource for multiple application threads.
This allows you to share tuners between applications. However, if you don't want
to share tuners, you can create two instances of ChannelManager, one for each
application.

Terminology:
The term "channel map" refers to the list of frequencies read by ChannelMap. ChannelScan
uses the channel map to produce a channel list. Brutus stores this as channels.txt
by default.

The term "channel list" refers to the results of a channel scan. The channel list is the
list of TV programs that the end user sees when they are channel changing. For the
same channel map, the resulting channel list may vary depending on what channels
are found.

Because of these two entities, the term "channel" often has two meanings depending on context.
To keep things clear, a ChannelMapEntry is one item in the channel map.
A Channel is one item in the channel list.

Ideas on how to manage a UI:
    1. Allow user to tune any channel, but display "no tuner available" message
        if you can't get a tuner for it.
    2. If you can't get a tuner, then have the app poll channel manager in case
        a tuner becomes available.
    3. Channel up/down might skip channels where you have no tuner.
    4. Pop up a UI dialog to either stop a background record or steal a tuner
        from another TV (assuming you know it's unused)

**/

class ChannelManager {
public:
    ChannelManager(ChannelMap *map, ChannelScan *scan);
    virtual ~ChannelManager();

    struct Settings {
        bool audioDecodeEnabled;
        bool videoDecodeEnabled;
        bool sapEnabled; /* BTSC SAP decode */
        bool combFilterEnabled; /* Analog 3D comb */
        int overrideBand;   /* overriding the band is used for exceptions like
                                redirecting a downstream out of the chip and then back
                                in via another band. */
        bool forceUseOfDaughterCards;
    };

    void getSettings(Settings *settings) const {*settings = _settings;}
    void setSettings(const Settings *settings);

    int populateFrontEnd();
    int depopulateFrontEnd();
    bool isFrontEndPopulated() const {return _isPopulated;}

    /**
    Summary:
    Get a specific linein resource from the list.
    TODO: this should be checkin/checkout.
    **/
    btuner_linein_t getLinein(int index);

    /**
    Summary:
    Get the encode and linein associated with an analog tuner.
    **/
    int getEncode(
        btuner_t analogTuner,
        bencode_t *p_encode, /* optional */
        bool *p_requireEncodeForDecode /* optional */
        ) const;

    struct TuneResults {
        btuner_t tuner; /* always set */
        bstream_t stream; /* set if able to lock, analog or digital */
        bband_t band; /* set for digital, may not be locked */
        btuner_linein_t linein; /* set if channel is linein */
    };

    /**
    Summary:
    Tune a channel and return a set of Settop API handles related to it.

    Description:
    It will use whatever tuner is available which can handle the
    stream type.
    Until you untune, the tuner is exclusively yours.

    The results.stream is all you need in order to record or decode.
    If the tune could not lock, results.stream is NULL, but results.band it set.
    The tuner and linein resource is returned as well in case you want to get frontend status.
    **/
    int tune(
        int channel,
        TuneResults *results /* [out] */
        );


    /**
    Summary:
    When you no longer need a stream or band, stop it.
    This makes the tuner available to others.
    **/
    void untune(bstream_t stream);
    void untune(bband_t band);
    void addStreamToTunerEntry(bband_t band, bstream_t stream);


    /* Retrieve ChannelMap passed into constructor */
    ChannelMap *map() const {return _map;}

    ChannelScan *channelScan() const {return _scan;}

    int scan();
    int psiScan(const  char * filename, const char * ipv4 = NULL);

    bstream_t createStream(bband_t band, ChannelMap::Channel *channel);

    /* Get any band where the lock has changed. This function should be called repeatedly
    until it returns NULL in order to get every band. */
    bband_t getLockChange();    

protected:
    Settings _settings;
    ChannelMap *_map;
    ChannelScan *_scan;
    bool _isPopulated;

    void addTuner( btuner_stream_type type, btuner_t tuner, int groupId = -1 );
    void addStreamer( int index, int settopapi_id );
    void addIP(void);
#if B_HAS_RTSP
    void addRTSP(unsigned index);
#endif
    void addEncode(btuner_t analogTuner, bencode_t encode, bool requireEncodeForDecode = false);
    void addLinein(btuner_linein_t linein, int index);
    void loadTuners(const bconfig_obj_list *obj_list, btuner_stream_type type);

#define B_CHANNELMGR_MAX_LINEIN 10
    btuner_linein_t _linein[B_CHANNELMGR_MAX_LINEIN];

    struct AnalogSupport {
        btuner_t analogTuner;
        bencode_t encode; /* encode associated with this analog tuner */
        bool requireEncodeForDecode; /* If true, then you can only decode an encoded
                                    stream. No analog pass-through. For some platforms
                                    this is asserted true. */
    };
    MAutoList<AnalogSupport> _analogSupport;

    struct TunerEntry {
        /* info about this Entry's storage */
        ChannelManager *mgr;
        int index;

        /* info about this tuner's capabilities */
        btuner_stream_type type;
        btuner_t tuner; /* Used for all tuners (not for streamer inputs) */
        struct {
            int index;
            int settopapi_id;
        } streamer;
        int groupId;

        /* check out flag */
        bool locked;

        /* if checked out, the following will be set */
        bstream_t stream; /* For external tunes */
        bband_t band; /* For tune callbacks from ChannelScan */
        bool got_lock_callback;

        /* info about the stream's content. only valid is stream != NULL */
        int chnum;
        ChannelMap::ChannelMapEntry entry;
#if B_HAS_RTSP
        struct {
            unsigned pump_index;
            bplayback_file_t source;
            bplayback_t play;
            bplaypump_t pump;
        } rtsp;
#endif

        TunerEntry(btuner_t t, btuner_stream_type ty, int g = -1);
    };
    MAutoList<TunerEntry> _tuners;
    BKNI_MutexHandle _mutex;

    bband_t tuneDigital(const ChannelMap::ChannelMapEntry *entry,
        TunerEntry **p_tuner);
    bstream_t tuneAnalog(const ChannelMap::ChannelMapEntry *entry,
        TunerEntry **p_tuner, btuner_linein_t *linein);

    /* callbacks from from ChannelScan */
    static bband_t tuneCallback(ChannelMap::ChannelMapEntry *stream, void *data);
    static void untuneCallback(bband_t band, void *data);

    /**
    Check out a btuner_t based on stream_type. The checkout
    must be "finished" in that either a band or stream should be associated with it.
    This is because only the band or stream is going to be returned to the user.
    When they untune, they will pass back in the band or stream, and we have to
    checkin the tuner based on that information only.
    **/
    TunerEntry *checkOutTuner(btuner_stream_type type,
        int streamer_index = -1);

    /**
    Finish the checkout now that we know the stream or band and how it will
    be used with the channel map.
    **/
    void finishCheckOut(TunerEntry *t, bstream_t stream, bband_t band);

    TunerEntry *findCheckOut(bstream_t stream, bband_t band);
    void checkIn(TunerEntry *t);
    int getBandNum(TunerEntry *t);

    /**
    How many tuners are checked out right now.
    **/
    int totalCheckOuts() const;

    /* callback from Settop API when lock changes */
    static void lock_callback(void *data /*TunerEntry*/);    
};

void b_set_bstream_mpeg(bstream_mpeg *mpeg, const ChannelMap::Channel *channel);
int b_check_duplicate_audiopid(int pid, bstream_mpeg *mpeg, int limit);
int b_check_duplicate_videopid(int pid, bstream_mpeg *mpeg, int limit);

#endif // CHANNELMGR_H__
