/***************************************************************************
 *     Copyright (c) 2002-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: avstream.cpp $
 * $brcm_Revision: 61 $
 * $brcm_Date: 10/9/12 3:06p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/avmanager/avstream.cpp $
 * 
 * 61   10/9/12 3:06p erickson
 * SW7435-392: convert ERR to WRN
 * 
 * 60   8/3/11 6:24p jtna
 * SW7425-1040: do not set pcr offsetThreshold to 0; it's no longer needed
 * 
 * 59   5/27/11 9:42p vishk
 * SW7231-32: Implement the Nexus code to support the 3461 frontend chip.
 * Implement brutus support for the downstream core.
 * 
 * 58   3/7/11 12:28p jtna
 * SW7550-680: short-circuit initPrimers() if needed
 * 
 * 57   1/5/11 10:52a rjlewis
 * SW7550-530: we reserve an STC channel so use one less when deciding on
 * max.
 * 
 * 56   8/2/10 3:46p randyjew
 * SW7208-98: Set  brecord struct to Null,  if there is an Error in
 * StartRecord.
 * 
 * 55   8/2/10 3:36p randyjew
 * SW7208-98: Set RecordEngine DecodeEngine to Null,  if there is an Error
 * in StartRecord.
 * 
 * 54   7/23/10 12:38p jtna
 * SW7550-478: extend video primer FCC support to platforms with fewer STC
 * channels
 * 
 * 53   6/15/10 4:27p mphillip
 * SW7405-4502: Fix warnings with DEBUG=n
 * 
 * 52   6/14/10 3:30p jtna
 * SW7401-4412: allow 7401/7403 to compile without video primer FCC
 * support in shim
 * 
 * 51   1/21/10 5:42p mward
 * SW7400-2517: merge to main support for BUILD_SYSTEM=magnum.
 * 
 * SW7400-2517/3   1/7/10 11:57a mward
 * SW7420-70: add BERR_TRACE's to help debug
 * 
 * 50   12/22/09 1:35p erickson
 * SW7420-70: add BERR_TRACE's to help debug
 *
 * SW7400-2517/2   11/18/09 9:14a mward
 * SW7400-2517: merge latest from main.
 * 
 * 49   10/23/09 4:28p jtna
 * SW7405-3265: handle video primer FCC better
 *
 * 48   9/29/09 3:41p jtna
 * SWDEPRECATED-3910: increase PB data rate and allow pcrPid != vPid
 *
 * SW7400-2517/1   9/14/09 3:09p mward
 * SW7400-2517: Maintain Native Settop API (BUILD_SYSTEM=magnum) build for
 * 97400.
 * 
 * 47   9/11/09 10:04a jtna
 * SWDEPRECATED-3910: improve FCC performance
 *
 * 46   8/25/09 12:47p jtna
 * SWDEPRECATED-3910: video primer fast channel change feature
 *
 * 45   7/9/09 11:50a jtna
 * PR56522: Remove multi-decode fast channel change feature. To be
 * replaced with video primer FCC.
 *
 * 44   6/24/09 7:15p lwhite
 * PR53773: Adding DTCP/IP + SSL Support in Brutus
 *
 * 43   6/12/09 11:10a jtna
 * PR43001: support TS/TTS record
 *
 * 42   6/1/09 7:54p jtna
 * PR43001: added record-related brutus script commands
 *
 * 41   5/11/09 4:39p ssood
 * PR54955: Record TTS streams over HTTP: set flag in video context to
 * indicate a TTS stream
 *
 * 40   9/16/08 1:48p erickson
 * PR46783: added WRN if record is started even though stream is not
 * locked
 *
 * 39   9/15/08 12:40p tokushig
 * PR43426: merge to main
 *
 * BRUTUS_97405_97043/1   9/3/08 2:51p tokushig
 * PR43426: use transcode handle opened in control in avstream.  do not
 * open another one in startLiveTranscode
 *
 * 38   7/30/08 8:10p shyam
 * PR43426 : Add dynamic toggling between transcode and decode
 *
 * 37   7/17/08 6:22p shyam
 * PR43426 : Add 7043 656 Encode support
 *
 * 36   6/25/08 12:54p shyam
 * PR43426 : Add 7043 Transcode support
 *
 * 35   5/27/08 4:42p jtna
 * PR42200: Multi-decode fast channel change feature
 *
 * 34   2/14/08 5:15p katrep
 * PR39180: OFDM doesnt support lock/unlock callback added forced channel
 * scan.
 *
 * 33   2/1/08 4:59p katrep
 * PR39180: Added support for ofdm/DVB-T THD tuner
 *
 * 32   12/6/07 2:08p erickson
 * PR37038: add CONTINUOUS_RECORD_DURATION, remove unused fixedSize and
 * CIRCFILE.
 *
 * 31   6/22/07 1:51p erickson
 * PR30701: reimplemented AVStream's record on lock feature. we must
 * support AVStream's which don't have bstream_t yet.
 *
 * 30   6/11/07 1:05p jjordan
 * PR32034: Add support for TTS streams in IP channels
 *
 * 29   3/13/07 11:42a mward
 * PR28358:  Returning -1 from AVStream::tune() for streamer causes
 * TunerEntry to not be checked in at quit.
 *
 * 28   1/29/07 11:18a ssood
 * PR24918: Channel Map Redesign to make its access Thread-safe
 *
 * 27   12/13/06 5:18p erickson
 * PR26538: fix no PVR_SUPPORT
 *
 * 26   11/30/06 6:54p ssood
 * PR26252: IP STB SW unification w/ Generic Brutus release
 *
 * 25   11/6/06 10:53a ssood
 * PR25072: Unable to play Video error while recording a SAP channel
 *
 * 24   11/2/06 11:06a arbisman
 * PR23775: Modified to use playback manager
 *
 * 23   9/28/06 6:55p arbisman
 * PR24289: Back-out changes
 *
 * 22   9/26/06 4:13p jgarrett
 * PR 20139: Fixing support when PVR_SUPPORT=n
 *
 * 20   7/21/06 10:25a erickson
 * PR22837: downgrade WRN to MSG
 *
 * 19   7/13/06 3:33p jrubio
 * PR21507: tune() must not return 0 when it doesn't have a lock!
 *
 * 18   6/19/06 3:47p erickson
 * PR19526: added ChannelMapEntry::setType to force initialization of
 * Settop API frontend params structures when type changes
 *
 * 17   4/7/06 10:52a ahulse
 * PR20504: Merge Vpp Modifications into main branch
 *
 * Vpp_7042_Demo/3   4/5/06 3:50p ahulse
 * PR20504: removed call to bencode_settings_init() because
 * bencode_extended_settings_init no calls it internally.
 *
 * Vpp_7042_Demo/2   4/4/06 1:17p ahulse
 * PR20504: change use of _vpp_default to _vpp_none
 *
 * Vpp_7042_Demo/1   3/29/06 5:51p ahulse
 * PR20504: Enable support for changeing 7042's vpp settings when encoding
 * analog
 *
 * 16   3/22/06 12:59p mphillip
 * PR19786: Fix a couple errors with PVR recording, and transitioning
 * in/out of PVR record
 *
 * 15   3/15/06 4:03p mphillip
 * PR19786: Initial implementation.  Add CONTINUOUS_RECORD_ENABLED=1 to
 * brutus.cfg to enable
 *
 * 14   8/12/05 10:43a jjordan
 * PR16670: add IP STB record to HDD capability
 *
 * 13   7/18/05 2:55p erickson
 * PR16322: mpeg_type needs to be !ts
 *
 * 12   7/15/05 10:44p erickson
 * PR16322: push more Video structure setup work into Video so that it can
 * keep it consistent
 *
 * 11   6/7/05 2:04p erickson
 * PR15234: added dbg
 *
 * 10   5/5/05 9:05a erickson
 * PR14701: don't findNextChannel when relocking, because if there's no
 * programs, we'll end up cycling through the channel map
 *
 * 9   4/20/05 11:41a erickson
 * PR14701: handle locked state but no program info
 *
 * 8   4/19/05 3:46p erickson
 * PR14701: make digital subchannel support standard for all platforms
 *
 * 7   4/19/05 10:42a erickson
 * PR14721: handle relock for dual tuner
 *
 * 6   4/18/05 12:24p erickson
 * PR14593: update the ui when an AVStream changes in the background
 *
 * 5   4/9/05 12:20a erickson
 * PR14701: added SUBCHANNEL_SUPPORT and moved tune/untune code from
 * AVManager to AVStream. This way, AVStream can retune and rescan itself
 * as it comes in and out of lock.
 *
 * 4   4/7/05 5:13p erickson
 * PR14701: rearrange AVStream so that current channel is stored in a
 * wrapper
 *
 * 3   3/21/05 11:20a erickson
 * PR14451: allow indexes for AVC content
 *
 * 2   2/14/05 4:08p erickson
 * PR11077: cache the tuner as well as record for timeshifting video
 *
 * 1   2/7/05 7:53p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 *
 * Irvine_BSEAVSW_Devel/9   1/4/05 9:43a erickson
 * PR13097: added AVC_TS record support
 *
 * Irvine_BSEAVSW_Devel/8   11/19/04 3:31p erickson
 * PR13199: reworked record error return codes
 *
 * Irvine_BSEAVSW_Devel/7   11/11/04 10:57a erickson
 * PR13199: improved record error messages
 *
 * Irvine_BSEAVSW_Devel/6   8/6/04 12:05p erickson
 * PR11332: added error msg
 *
 * Irvine_BSEAVSW_Devel/5   5/27/04 2:05p bandrews
 * PR11250: Fixed logic for checking if video status shows video present
 *
 * Irvine_BSEAVSW_Devel/4   5/25/04 11:50a erickson
 * PR11135: use bband_get for proxy support
 *
 * Irvine_BSEAVSW_Devel/3   5/10/04 4:51p erickson
 * PR10832: added requiredEncodeForDecode flag instead of hardcoding it
 * into AVManager
 *
 * Irvine_BSEAVSW_Devel/2   4/30/04 5:27p erickson
 * PR10832: more debug related to new channelmgr/avmanager
 *
 * Irvine_BSEAVSW_Devel/1   4/30/04 12:06p erickson
 * PR10832: rework of brutus to support single AVManager and
 * ChannelManager for system
 *
 ******************************************************************************/
#include "avstream.h"
#include "avmanager.h"
#include <bsettop.h>
#include "channelmgr.h"
#include "decodeengine.h"
#include "recordengine.h"
BDBG_MODULE(avstream);

#define LIVE_ENCODING_QUALITY bencode_quality_best

AVStream::AVStream(ChannelManager *chanmgr)
{
    _unlockTime = _lastTime = 0;
    _chanmgr = chanmgr;
    _digitalStream = NULL;
    _analogStream = NULL;
    _band = NULL;
    _tuner = NULL;
    _linein = NULL;
    _encode = NULL;
    _encodeForDecode = false;
    _record = NULL;
    _decodeManager = NULL;
    _decode = NULL;
    _callback = NULL;
    _context = NULL;
    _onLockVideo = NULL;
#ifdef PVR_SUPPORT
    _playback = NULL;
#endif
    _chDeferDelete = false;

#ifdef B_HAS_TRANSCODE
    _transcodedStream = NULL ;
    _transcodeOn = false;
#endif

    enablePrimers = false;
    numOpenedPrimers = 0;
}

#ifdef B_HAS_TRANSCODE
void NEXUS_Transcoder_Debug(int loops);

#include <unistd.h>

int AVStream::startLiveTranscode(bencode_quality quality, bool transcodeEnable)
{
    btranscode_settings settings;

    if(transcodeEnable)
    {
        DecodeEngine *d = _decode;

        if(_transcodeOn)
            return -1 ;
        _transcodeOn = true ;

        if(NULL == _transcodedStream)
        {
            btranscode_settings_init(&settings, quality, bencode_vpp_none);
            _transcodedStream = btranscode_start(d->transcode(), _digitalStream, &settings);
        }

        BDBG_WRN(("Started transcode"));
        stopDecode() ;
        startDecode(d) ;

    }else
    {
        _transcodeOn = false ;
        BDBG_WRN(("Started regular decode"));
        DecodeEngine *d = _decode;
        stopDecode() ;
        startDecode(d) ;
    }

    return 0 ;

}
#endif

int AVStream::startEncode(bencode_quality quality, bencode_vpp_mode vpp_mode )
{
    bencode_settings settings;

    // don't encode linein that isn't present because we get long delays when
    // the encoder has to reset. rf doesn't have the same problem because the
    // vdec always produces 656
    if (_linein) {
        btuner_analog_status status;
        if (btuner_get_analog_status(_tuner, &status) || !status.video_present)
            return -1;
    }

    bencode_extended_settings_init(&settings, quality, _encode, vpp_mode);
#if B_HAS_TRANSCODE
    settings.video_format = bvideo_codec_h264 ;
#endif
    _digitalStream = bencode_start(_encode, _analogStream, &settings);
    return _digitalStream ? 0 : -1;
}

void AVStream::stopEncode()
{
    if (isEncoding()) {
        bstream_close(_digitalStream);
        _digitalStream = NULL;
    }
}

#ifdef PVR_SUPPORT
 /* see Control::recordVideo() for final presentation of non-zero error codes */
int AVStream::startRecord(RecordEngine *record, DecodeManager *decodeManager, Video *vid)
{
    bstream_status status;
    int result = -1;

    BDBG_MSG(("AVStream::startRecord(): Checking record status"));
    if (_record)
        return -2;

    /* make the link, even if the stream doesn't exist */
    _record = record;
    vid->record = _record->handle();
    _decodeManager = decodeManager;

    /* get the digital stream, possibly starting encoding */
    vid->stream = NULL;

    if (_analogStream) {
        if (!_encode) {
            BDBG_ERR(("No encoder available"));
            result = -3;
            goto error; /* can't encode */
        }

        if (_encodeForDecode) {
            /* Switch from live quality to desired record quality */
            if (vid->encodingQuality != LIVE_ENCODING_QUALITY) {
                stopEncode();
                if (startEncode(vid->encodingQuality,vid->encodingVpp)) {
                    result = -4;
                    goto error;
                }
            }
        }
        else {
            if (startEncode(vid->encodingQuality,vid->encodingVpp)) {
                result = -4;
                goto error;
            }
        }
    }
    else if (!_digitalStream) {
        /* We are attempting record before the stream is locked. Save info and allow record to start in checkLock.
        This was added to prevent an app race condition from stopping record.
        If the lock never comes, the application will be left in a strange state. TODO: reconsider this. */
        BDBG_WRN(("Record started before stream is locked"));
        _onLockVideo = vid;
        return 0;
    }

    vid->stream = _digitalStream;

    /* get info from the live stream to populate Video */
    if (bstream_get_mpeg_parameters(vid->stream, &status))
        return -1; /* if digital, shouldn't fail */

    /* PVR encryption was already set by record screen, so add this in */
    status.mpeg.encryption = vid->mpeg.encryption;

    /* now save all this info to the Video object. don't try to set individual members because
    things will get inconsistent. */
    vid->set(&status.mpeg);
    if (!vid->timestamp_override) {
        vid->timestamp_enabled = status.timestamp_enabled;
    }

    /* in certain cases, no index will be produced, so don't try to record it */
    if (vid->mpeg.mpeg_type != bstream_mpeg_type_ts ||
        vid->mpeg.video[0].pid == 0 ||
        vid->mpeg.video[0].pid >= 0x1fff)
    {
        vid->indexfile = NULL;
    }

    result = record->start(vid);
    if (result) {
        /* just pass through the error, which is using non-zero values
        coordinated with this code */
        goto error;
    }

    vid->tuner = tuner();

    return 0;

error:
    BDBG_ERR(("startRecord() Failed"));
    _record = NULL;	
    vid->record = NULL;
    _decodeManager = NULL;
    return result;
}

void AVStream::stopRecord()
{
    if (!_record) {
        return;
    }

    /* call back into the manager so that linked playbacks can be unlinked */
    if (_decodeManager) {
        _decodeManager->stopTimeshifting(_record);
    }

    Video *vid = _record->video();
    if (vid) {
        _record->stop();
        BDBG_ASSERT(!_onLockVideo);
    }
    else {
        /* the only way to get here is if we have on _onLockVideo pending */
        BDBG_ASSERT(_onLockVideo);
        vid = _onLockVideo;
        _onLockVideo = NULL;
    }
    vid->record = NULL;
    vid->tuner = NULL;

    /* If we're encoding only for record, we need to stop so that any possible horizontal
    scaling will be turned off. */
    if (isEncoding()) {
        if (_encodeForDecode) {
            /* Switch back to live quality */
            if (vid->encodingQuality != LIVE_ENCODING_QUALITY) {
                stopEncode();
                if (startEncode(LIVE_ENCODING_QUALITY, bencode_vpp_none ))
                    BDBG_ERR(("Unable to restart encoding for live decode"));
            }
        }
        else {
            stopEncode();
        }
    }

    _record = NULL;
    _decodeManager = NULL;
}
#endif

bstream_t AVStream::liveStream() const
{
#ifdef B_HAS_TRANSCODE
    if(_transcodeOn)
    {
        return _transcodedStream ;
    }
    else
#endif
    {
        return isDigitalDecode()?_digitalStream:_analogStream;
    }
}

int AVStream::startDecode(DecodeEngine *decode)
{
    if (isDecoding())
        return -1;

    _decode = decode;

    bstream_t stream = liveStream();
    if (!stream) {
        BDBG_WRN(("No stream to decode"));
        return -1;
    }

    BDBG_MSG(("startDecode %p", this));
    return decode->startLive(this);
}

void AVStream::stopDecode()
{
    if (_decode) {
        BDBG_MSG(("stopDecode %p", this));
        _decode->stopLive();
        uninitPrimers();
        _decode = NULL;
    }
}

bool AVStream::isLocked()
{
    switch (_channel.entry.getType()) {
    case btuner_sds_stream:
        {
        btuner_sds_status st;
        btuner_get_sds_status(_tuner, &st);
        return st.rs_locked;
        }
    case btuner_vsb_stream:
        {
        btuner_vsb_status st;
        btuner_get_vsb_status(_tuner, &st);
        return st.receiver_lock;
        }
    case btuner_qam_stream:
        {
        btuner_qam_status st;
        btuner_get_qam_status(_tuner, &st);
        return st.qam_lock;
        }
    case btuner_ofdm_stream:
        {
        btuner_ofdm_status st;
        btuner_get_ofdm_status(_tuner, &st);
        return st.receiver_lock;
        }
    default:
        /* TODO: check if analog present? */
        /* TODO: for streamer, can we check if there's any data in the tpband? */
        /* can't fall out of lock, therefore it's locked */
        return true;
    }
}

void AVStream::timer(unsigned long time)
{
    if (_unlockTime && time > _unlockTime)
        checkLock(true);
    _lastTime = time;
}

void AVStream::checkLock(bool fromTimer)
{
    // analog
    if (!_band) return;

    int entryIndex = MAINCH(_channel.chnum);
    if (!isLocked()) {
        BDBG_MSG(("%p: ch%d unlocked", this, entryIndex));
        if (fromTimer) {
            /* clear all programs.  */
            BDBG_MSG(("programs cleared"));
            _chanmgr->map()->clearPrograms(entryIndex);
            _unlockTime = 0;
        }
        else {
            _unlockTime = _lastTime + 2000;
        }
    }
    else {
        _unlockTime = 0;
        BDBG_MSG(("%p: ch%d locked", this, entryIndex));
        /* if there are no programs, scan */
        if (!_chanmgr->map()->totalPrograms(entryIndex)) {
            _chanmgr->channelScan()->scan(entryIndex, _band);
        }

        if (!_digitalStream) {
            /* our channel numbers might have changed, so allow a hop */
            //int chnum = _chanmgr->map()->findNextChannel(_channel.chnum, 0);
            /* the downside is that if we never lock, we keep hopping all over the place. the ideal
            solution is to allow only hopping down if subchannel > 1. until then, don't hop. */
            int chnum = _channel.chnum;

            if (_chanmgr->map()->getChannel(chnum, &_channel)) {
                BDBG_WRN(("Unable to find ch%d", chnum));
                return;
            }
            _digitalStream = _chanmgr->createStream(_band, &_channel);
            if (!_digitalStream) return;

            /* _digitalStream has been created, which maps the IB to a PB. we can setup the primers now */
            initPrimers(SUBCH(ch()));


            _chanmgr->addStreamToTunerEntry(_band, _digitalStream);

            /* if we were or want to decode, start it up */
            if (_decode) {
                /* we have to fool it into thinking we're not already decoding */
                DecodeEngine *d = _decode;
                _decode = NULL;
                startDecode(d);
            }

            /* fire optional callback to application */
            if (_callback)
                (*_callback)(_context);
#ifdef PVR_SUPPORT
            /* start recording, if we need to */
            if (_record && _onLockVideo) {
                RecordEngine *record = _record;
                _record = NULL;
                startRecord(record, _decodeManager, _onLockVideo);
                _onLockVideo->write(); /* the index file must be written now, the pids were written as 0 initially */
                _onLockVideo = NULL;
            }
#endif
        }
    }
}

int AVStream::tune(int chnum, Video* video, PlaybackManager *playMgr)
{
    ChannelManager::TuneResults results;

    if (_chanmgr->map()->getChannel(chnum, &_channel)) {
        BDBG_WRN(("Unable to find ch%d", chnum));
        return -1;
    }

    /* Streamer is a special case. If there are no programs, tune should
    result in a scan */
    if (_chanmgr->map()->totalPrograms(chnum) == 0 &&
        _channel.entry.getType() == btuner_streamer_stream)
    {
        _chanmgr->channelScan()->scan(chnum, _band);
    }

    if(_channel.entry.getType() == btuner_ip_stream)
    {
        if( _channel.entry.ip.params.open_params.protocol == bsettop_ip_protocol_rtsp)
            BDBG_MSG(("Got RTSP chan(%d) at %s",chnum,_channel.entry.ip.params.open_params.url));
        else
            BDBG_MSG(("Got UDP/RTP chan(%d) at %s:%d",chnum,_channel.entry.ip.params.open_params.ip_addr, _channel.entry.ip.params.open_params.port));

        video->isIP = true;
        if( !_chanmgr->map()->totalPrograms(chnum) )
        {
            video->doAutoPids = true;
        }
        else
        {
            b_set_bstream_mpeg(&video->mpeg, &_channel);
        }
        strcpy(video->socket.open_params.ip_addr, _channel.entry.ip.params.open_params.ip_addr);
        video->socket.open_params.port = _channel.entry.ip.params.open_params.port;
        video->socket.open_params.protocol = _channel.entry.ip.params.open_params.protocol;
        video->socket.open_params.url = _channel.entry.ip.params.open_params.url;
        video->timestamp_enabled = _channel.entry.ip.timestamp_enabled;
        video->socket.open_params.security.security_protocol = _channel.entry.ip.params.open_params.security.security_protocol;

#ifdef PVR_SUPPORT
        _pbManager = playMgr;
        _playback = playMgr->checkOut();
        if(!_playback) {
            return BERR_TRACE(berr_not_available);
        }

        results.stream = _playback->start(video, 0, NULL, NULL);
        if (!results.stream)
        {
            playMgr->checkIn(_playback);
            _playback = NULL;
            return BERR_TRACE(berr_external_error);
        }
#endif

        if( !_chanmgr->map()->totalPrograms(chnum) && (video->mpeg.pcr_pid != 0) )
        {
            PROGRAM_INFO_T program;
            /* 2. set program PIDs info */
            ChannelMap::setProgram(&program, video->mpeg.pcr_pid,
                                            video->mpeg.video[0].pid,
                                            video->mpeg.audio[0].pid,
                                            video->mpeg.audio[0].format,
                                            video->mpeg.video[0].format);
            _chanmgr->map()->addProgram(chnum, &program, NULL, true /* need rescan */);
        }

    }
    else {
        if (_chanmgr->tune(chnum, &results)) {
            BDBG_WRN(("Unable to tune ch%d", chnum));
            return -1;
        }
    }

    /* at this point, we've at least found the channel. unless there's a
    system failure after this, we need to return 0. */

    _band = results.band;
    _tuner = results.tuner;

    if (!results.stream) {
            /* we acquired, but weren't able to lock or there are no programs. */
            BDBG_MSG(("Did not lock ch%d (%p)", chnum, _band));

            /* hack hack remove this code after ofdm callback is implemented*/
            if((_channel.entry.getType() == btuner_ofdm_stream)||(_channel.entry.getType() == btuner_qam_stream))
                checkLock();

            return 0;
    }

    BDBG_MSG(("Tuned and locked ch%d", chnum));
    if (_chanmgr->map()->isDigital(chnum)) {
        _digitalStream = results.stream;
    }
    else {
        _analogStream = results.stream;
        _linein = results.linein;

        /* Get the other handles associated with an analog stream */
        if (_chanmgr->getEncode(results.tuner, &_encode,
            &_encodeForDecode))
        {
            BDBG_MSG(("No analog support resources: %p", results.tuner));
        }
        else {
            BDBG_MSG(("getEncode %c, liveencode=%c",
                _encode?'y':'n',
                _encodeForDecode?'y':'n'));
        }
    }

    /* moved this into here because if we are already encoding a recorded stream, we
        don't want to stop and restart it */
    if (_encodeForDecode) {
        startEncode(LIVE_ENCODING_QUALITY, bencode_vpp_none );
    }

    return 0;
}

int AVStream::untune()
{
    if (isDecoding()) {
        BDBG_ERR(("Can't untune while decoding"));
        return -1;
    }
    if (isRecording()) {
        BDBG_ERR(("Can't untune while recording"));
        return -1;
    }
    if (isEncoding()) {
        BDBG_WRN(("Forcing encode stop"));
        stopEncode();
    }
#ifdef PVR_SUPPORT
    if( _channel.entry.getType() == btuner_ip_stream )
    {
        _playback->stop(NULL);
        _pbManager->checkIn(_playback);
        _playback = NULL;
        // check if the channel associated w/ this stream has been marked for
        // deletion. If so, delete the channel. A channel can be marked for deletion
        // when it was added via SAP and was later deleted before user switched from
        // that channel or before recording was stopped on that channel.
        if (_chDeferDelete)
        {
            _chanmgr->map()->deleteEntry(_channel.entry.getSubType(),
                                                              _channel.entry.sourceChNum);
            _chDeferDelete = false;
        }
        goto done;
    }
#endif
    if (_analogStream)
        _chanmgr->untune(_analogStream);
    else if (_digitalStream)
        _chanmgr->untune(_digitalStream);
    else if (_band)
        _chanmgr->untune(_band);
done:
    _digitalStream = NULL;
    _analogStream = NULL;
    _band = NULL;
    _tuner = NULL;
    _linein = NULL;

    return 0;
}

void AVStream::setCallback(bsettop_callback callback, void *context)
{
    _callback = callback;
    _context = context;
}

#ifdef PVR_SUPPORT
RecordEngine *AVStream::recordEngine() const
{
    return _record;
}
#endif

#ifdef NEXUS_BUILD
/* Brutus implements video primer fast channel change by accessing Nexus directly */
#include "bsettop_types.h"
#define BSETTOP_NO_API_THUNKS
extern "C" {
    #include "bsettop_impl.h"
}
#endif

#if (!defined (NEXUS_BUILD)) || (BCHP_CHIP==7401) || (BCHP_CHIP==7403)
#undef VIDEO_PRIMER_FCC_SUPPORT
#else
#define VIDEO_PRIMER_FCC_SUPPORT
#endif

/* open and start one primer for each program in the channel. if the number of programs exceeds 
   MAX_FCC_PRIMERS or NEXUS_NUM_STC_CHANNELS, then prime as many channels as we can, 
   and don't do anything about the rest. the first x channels will have fcc. the others won't. */
int AVStream::initPrimers(int stopIndex)
{
#ifdef VIDEO_PRIMER_FCC_SUPPORT
    if (!enablePrimers) {
        return 0; /* cfgsetting not enabled */
    }
    if (numOpenedPrimers) {
        BDBG_ERR(("initPrimers: primers already started"));
        return 0;
    }

    int mainch = MAINCH(_channel.chnum);
    NEXUS_Error rc;
    BKNI_Memset(videoProgram, 0, sizeof(videoProgram));
    BKNI_Memset(videoPrimer, 0, sizeof(videoPrimer));

    int numPrimers = _chanmgr->map()->totalPrograms(mainch);
    if (MAX_FCC_PRIMERS < numPrimers) {
        numPrimers = MAX_FCC_PRIMERS;
    }
    /* some platforms have fewer STC channels */
	/* Note: we reserve one in the open request below (use i+1) so account for that here */
    if (NEXUS_NUM_STC_CHANNELS-1 < numPrimers) {
        numPrimers = NEXUS_NUM_STC_CHANNELS-1;
    }
    if (numPrimers < _chanmgr->map()->totalPrograms(mainch)) {
        BDBG_WRN(("initPrimers: limited to priming the first %d programs", numPrimers));
    }

    BDBG_MSG(("initPrimers: stopIndex %d", stopIndex));

    for (int i=0; i<numPrimers; i++) {
        ChannelMap::Channel chan;
        int chnum;
        bstream_mpeg mpeg;
        ChannelManager::Settings chmgrSettings;
        NEXUS_StcChannelSettings stcSettings;
        NEXUS_PidChannelHandle pcrPidChannel;
        bdecode_t decode;

        /* get program info for subchannel */
        chnum = MAKESUBCH(mainch, i);
        chan.chnum = chnum;
        rc = _chanmgr->map()->getChannel(chnum, &chan);
        if (rc || !chan.validProgramInfo) {
            BDBG_ERR(("initPrimers: unable to get program info"));
            goto error;
        }
        b_set_bstream_mpeg(&mpeg, &chan);

        _chanmgr->getSettings(&chmgrSettings);
        if (!chmgrSettings.audioDecodeEnabled) {
            memset(mpeg.audio, 0, sizeof(mpeg.audio));
        }
        if (!chmgrSettings.videoDecodeEnabled) {
            goto error;
        }

        if (!mpeg.video[0].pid || !mpeg.pcr_pid) {
            BDBG_ERR(("initPrimers: unable to get valid pid"));
            goto error;
        }

        if (!_decode || !_decode->_settings.decode) {
            /* since we never stop decode, this should always be true */
            BDBG_ERR(("initPrimers: no decode"));
            goto error;
        }
        decode = _decode->_settings.decode;

        if (!_digitalStream) {
            goto error;
        }

        NEXUS_ParserBand parserBand = (NEXUS_ParserBand)(_digitalStream->parser_band->nParserBand);
        if (i==0) {
            NEXUS_ParserBandSettings parserBandSettings;
            NEXUS_ParserBand_GetSettings(parserBand, &parserBandSettings);
            parserBandSettings.maxDataRate = 42 * 1024 * 1024; /* for HD streams */
            rc = NEXUS_ParserBand_SetSettings(parserBand, &parserBandSettings);
            if (rc) {
                rc=BERR_TRACE(rc);
            }
        }

        numOpenedPrimers++;

        NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram[i]);
        videoProgram[i].codec = b_videocodec2nexus(mpeg.video[0].format);
        videoProgram[i].pidChannel = NEXUS_PidChannel_Open(parserBand, mpeg.video[0].pid, NULL);

        if (mpeg.video[0].pid == mpeg.pcr_pid) {
            pcrPidChannel = videoProgram[i].pidChannel;
        }
        else {
            pcrPidChannel = NEXUS_PidChannel_Open(parserBand, mpeg.pcr_pid, NULL);
        }

        if (!videoProgram[i].pidChannel || !pcrPidChannel) {
            BDBG_ERR(("initPrimers: unable to open pid channel"));
            goto error;
        }

        NEXUS_StcChannel_GetDefaultSettings(0, &stcSettings);
        stcSettings.autoConfigTimebase = false; /* must do it manually */
        stcSettings.timebase = decode->output_timebase;
        stcSettings.mode = NEXUS_StcChannelMode_ePcr; /* live */
        stcSettings.modeSettings.pcr.pidChannel = pcrPidChannel;
        stcSettings.stcIndex = 0;

        /* main.cpp disables SECOND_DISPLAY and PIP when FCC support is on. only one STC channel should have been consumed */
        videoProgram[i].stcChannel = NEXUS_StcChannel_Open(i+1, &stcSettings);
        if (!videoProgram[i].stcChannel) {
            BDBG_ERR(("initPrimers: unable to open stc channel"));
            goto error;
        }

        _decode->_settings.decode->primerStart.stcChannel = NULL;
        _decode->_settings.decode->primerStart.pidChannel = NULL;
        if (stopIndex < numPrimers) {
            _decode->_settings.decode->primerStart = videoProgram[SUBCH(_channel.chnum)]; /* this forces bdecode_start_video to use our startSettings (pidChannel and stcChannel), not its own */
        }

        videoPrimer[i] = NEXUS_VideoDecoder_OpenPrimer(decode->video_decode->nVideoDecoder);
        if (!videoPrimer[i]) {
            BDBG_ERR(("initPrimers: unable to open primer"));
            goto error;        }

        /* initPrimers() is typically called after tuner lock, and the first decode is started without a primer.
           open, but do not start the primer for that decode */
        if (i==stopIndex) {
            BDBG_MSG(("Opened   video primer %p: program %p, pid %p, stc %p ", videoPrimer[i], videoProgram[i], videoProgram[i].pidChannel, videoProgram[i].stcChannel));
            continue;
        }

        BDBG_MSG(("Starting video primer %p: program %p, pid %p, stc %p", videoPrimer[i], videoProgram[i], videoProgram[i].pidChannel, videoProgram[i].stcChannel));
        rc = NEXUS_VideoDecoder_StartPrimer(decode->video_decode->nVideoDecoder, videoPrimer[i], &videoProgram[i]);
        if (rc) {
            rc=BERR_TRACE(rc);
            goto error;
        }
    }

    return 0;

error:
    uninitPrimers();
    return -1;
#else
    if (!enablePrimers) {
        return 0; /* cfgsetting not enabled */
    }
    else {
        BDBG_ERR(("VIDEO_PRIMER_FASTCHANNELCHANGE_ENABLED is not supported for this build or platform"));
        return 0; /* cfgsetting not supported */
    }
#endif
}

/* for streamer input, since decodeEngine must be known in advance */
int AVStream::initPrimers(DecodeEngine *d, int stopIndex)
{
#ifdef VIDEO_PRIMER_FCC_SUPPORT
    if (d) {
        int rc;
        _decode = d;
        rc = initPrimers(stopIndex);
        _decode = NULL; /* restore to NULL, otherwise we assume that we're currently decoding */
        return rc;
    }
    else {
        return -1;
    }
#else
    return -1;
#endif
}

void AVStream::uninitPrimers()
{
#ifdef VIDEO_PRIMER_FCC_SUPPORT
    if (!_decode || !_decode->_settings.decode || !numOpenedPrimers) { /* don't dereference potentially null pointers */
        return;
    }

    BDBG_MSG(("uninitPrimers: %d primers", numOpenedPrimers));
    NEXUS_VideoDecoderHandle nVideoDecoder = _decode->_settings.decode->video_decode->nVideoDecoder;

    for (int i=0; i<numOpenedPrimers; i++) {
        if (videoPrimer[i]) {
            NEXUS_VideoDecoder_StopPrimer(nVideoDecoder, videoPrimer[i]);
            NEXUS_VideoDecoder_ClosePrimer(nVideoDecoder, videoPrimer[i]);
        }
        if (videoProgram[i].stcChannel) {
            NEXUS_StcChannel_Close(videoProgram[i].stcChannel);
        }
        if (videoProgram[i].pidChannel) {
            NEXUS_PidChannel_Close(videoProgram[i].pidChannel);
        }
    }
    numOpenedPrimers = 0;
    return;
#endif /* VIDEO_PRIMER_FCC_SUPPORT */
}

int AVStream::setChannelFromPrimer(int oldCh, int newCh)
{
#ifdef VIDEO_PRIMER_FCC_SUPPORT
    int oldIndex = SUBCH(oldCh);
    int newIndex = SUBCH(newCh);

    if (oldCh==newCh || MAINCH(oldCh)!=MAINCH(newCh) || newIndex>=numOpenedPrimers) {
        BDBG_ERR(("setChannelFromPrimer: primer unavailable"));
        return -1;
    }

    BDBG_MSG(("setChannelFromPrimer: index %d -> %d", oldIndex, newIndex));

#define FCC_AUDIO 1 /* toggle audio with FCC feature */
#if FCC_AUDIO
    /* save contexts and nullify, to trick bdecode_stop into leaving the video untouched */
    bvideo_decode_t saveVideoDecode = _decode->_settings.decode->video_decode;
    NEXUS_PidChannelHandle savePcrPid = _decode->_settings.decode->pcrPidChannel;
    bdecode_window_t saveWindow = _decode->_settings.decode->window;
    _decode->_settings.decode->video_decode = NULL;
    _decode->_settings.decode->pcrPidChannel = NULL;

    bdecode_stop(_decode->_settings.decode);
    /* restore contexts */
    _decode->_settings.decode->video_decode = saveVideoDecode;
    _decode->_settings.decode->pcrPidChannel = savePcrPid;
#endif

    NEXUS_VideoDecoderHandle nVideoDecoder = _decode->_settings.decode->video_decode->nVideoDecoder;
    NEXUS_VideoDecoder_Stop(nVideoDecoder);

    NEXUS_Error rc = NEXUS_SUCCESS;
    if (oldIndex < numOpenedPrimers) {
        BDBG_MSG(("setChannelFromPrimer: Starting primer %p (index %d)", videoPrimer[oldIndex], oldIndex));
        rc = NEXUS_VideoDecoder_StartPrimer(nVideoDecoder, videoPrimer[oldIndex], &videoProgram[oldIndex]);
        if (rc) { rc=BERR_TRACE(rc); }
    }

    NEXUS_Timebase timebase = _decode->_settings.decode->output_timebase;
    NEXUS_TimebaseSettings timebaseSettings;
    NEXUS_Timebase_GetSettings(timebase, &timebaseSettings);
    timebaseSettings.sourceType = NEXUS_TimebaseSourceType_ePcr;
    timebaseSettings.sourceSettings.pcr.pidChannel = videoProgram[newIndex].pidChannel;
    timebaseSettings.sourceSettings.pcr.maxPcrError = 0xff;
    timebaseSettings.sourceSettings.pcr.trackRange = NEXUS_TimebaseTrackRange_e61ppm;
    rc = NEXUS_Timebase_SetSettings(timebase, &timebaseSettings);
    if (rc) { rc=BERR_TRACE(rc); }

    rc = NEXUS_VideoDecoder_StartDecodeWithPrimer(nVideoDecoder, videoPrimer[newIndex]);
    if (rc) {
        return BERR_TRACE(rc); /* return error and try to do normal channel change */
    }

    /* get program info for new channel so we can keep _digitalStream in sync with what's being decoded */
    ChannelMap::Channel chan;
    chan.chnum = newCh;
    _chanmgr->map()->getChannel(newCh, &chan);
    if (!chan.validProgramInfo) {
        BDBG_WRN(("setChannelFromPrimer: not a valid program"));
    }

    bstream_mpeg mpeg;
    b_set_bstream_mpeg(&mpeg, &chan);
    bstream_p_set(_digitalStream, &mpeg);

#if FCC_AUDIO
    saveVideoDecode = _decode->_settings.decode->video_decode;
    _decode->_settings.decode->video_decode = NULL;
    _digitalStream->video_primer = true; /* this tells bdecode_start to skip some parts in bdecode_start that can't be skipped by simply nullifying contexts */
    _decode->_settings.decode->primerStart = videoProgram[newIndex]; /* this tells bdecode_start_audio to use the same stcChannel as video */

    rc = bdecode_start(_decode->_settings.decode, _digitalStream, saveWindow);
    if (rc) { rc=BERR_TRACE(rc); }
    /* restore contexts */
    _decode->_settings.decode->video_decode = saveVideoDecode;
    _digitalStream->video_primer = false;
#endif
#endif /* VIDEO_PRIMER_FCC_SUPPORT */
    return 0;
}

