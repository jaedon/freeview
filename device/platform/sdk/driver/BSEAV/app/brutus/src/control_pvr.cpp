/***************************************************************************
 *     Copyright (c) 2002-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: control_pvr.cpp $
 * $brcm_Revision: 16 $
 * $brcm_Date: 3/3/11 9:45a $
 *
 * Module Description: Central control of Brutus
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/control_pvr.cpp $
 * 
 * 16   3/3/11 9:45a erickson
 * SWDEPRECATED-3869: rename brutus "timeshifting" to be "playback &
 * record"
 *
 * 15   8/30/10 11:52a erickson
 * SW7550-551: clear record overflow
 *
 * 14   6/10/10 5:01p erickson
 * SWDEPRECATED-2726: fix continuous record transition on rewind. remove
 * "10 loop check" code. add sleep to allow frameadvance to get first
 * picture out.
 *
 * 13   4/22/10 4:04p erickson
 * SW7125-325: fix gcc-4.4.3 warning
 *
 * 12   4/16/10 4:06p erickson
 * SW7405-4089: make brutus resilient to config with zero decoders
 *
 * 11   6/12/09 11:11a jtna
 * PR43001: support TS/TTS record
 *
 * 10   6/3/09 12:02p jtna
 * PR43001: IP streams background recording work
 *
 * 9   6/2/09 6:35p jtna
 * PR43001: fix record control logic for IP channels
 *
 * 8   6/1/09 7:55p jtna
 * PR43001: added record-related brutus script commands
 *
 * 7   5/29/09 2:40p jtna
 * PR43001: move record logic from recordscreen
 *
 * 6   7/31/08 12:33p katrep
 * PR45316:remove the DRM dialog box when playback is topped
 *
 * 5   7/30/08 1:21p shyam
 * PR 45076 : Add trasncode support for playback streams
 *
 * 4   7/14/08 12:56p ssood
 * PR42739: trickmodes for RTSP channels had gotten disabled due to a
 * previous change
 *
 * 3   7/10/08 11:38p ssood
 * PR42739: Cleaned up logic on accessing playengines for IP playback
 *
 * 2   4/2/08 3:55p mward
 * PR40307: Don't include any thumbnail support with PVR_SUPPORT=n.
 *
 * 1   3/18/08 4:05p erickson
 * PR40307: added thumbnail feature
 *
 ***************************************************************************/
#include "control.h"
#include "mmessagebox.h"
#include "tvscreen.h"
#include "ledengine.h"

/* The entire contents of this file are conditional on #define PVR_SUPPORT */
#ifdef PVR_SUPPORT
#include "playbackscreen.h"

BDBG_MODULE(brutus_control_pvr);

#define tv()            ((TVScreen *)_screens[eTV])
#define playbackScreen()((PlaybackScreen *)_screens[ePlayback])

int Control::totalRecords() {
    if (cfg()->getBool(Config::CONTINUOUS_RECORD_ENABLED)) {
        int record_count = 0;
        for (RecordingStream *rs = _records.first(); rs; rs = _records.next()) {
            Video *video = rs->record->video();
            if (video && !video->isContinuousRecord) {
                record_count++;
            }
        }
        BDBG_MSG(("totalRecords %d (by loop)",record_count));
        return record_count;
    } else{
        BDBG_MSG(("totalRecords %d",_records.total()));
        return _records.total();
    }
}

/* Identifies if a channel is a HTTP playback channel */
bool Control::isHttpChannel(int ch)
{
    ChannelMap::Channel currentChannel;

    if (!_channelMap->getChannel(channel(ch), &currentChannel) &&
                currentChannel.entry.ip.params.open_params.protocol == bsettop_ip_protocol_http ){
        if(_live[ch] && _live[ch]->playEngine()){
            return true;
        }
    }
    return false;
}

/* Identifies if a channel is a RTSP playback channel (considered Live IP channel as server is on internet) */
bool Control::isRtspChannel(int ch)
{
#ifdef LIVEMEDIA_SUPPORT
    ChannelMap::Channel currentChannel;

    if (!_channelMap->getChannel(channel(ch), &currentChannel) &&
                currentChannel.entry.ip.params.open_params.protocol == bsettop_ip_protocol_rtsp ){
        if(_live[ch] && _live[ch]->playEngine()){
            return true;
        }
    }
#endif
    return false;
}

/* Identifies if a channel is a Live IP channel ( a non-HTTP IP Channel) */
bool Control::isLiveIpChannel(int ch)
{
    ChannelMap::Channel currentChannel;

    if (!_channelMap->getChannel(channel(ch), &currentChannel) &&
            currentChannel.entry.getType() == btuner_ip_stream &&
            currentChannel.entry.ip.params.open_params.protocol != bsettop_ip_protocol_http) {
        if (_live[ch] && _live[ch]->playEngine()) {
            return true;
        }
    }
    return false;
}

PlayEngine *Control::checkoutPlayEngine()
{
    return _playbackManager->checkOut();
}

void Control::checkinPlayEngine(PlayEngine *engine)
{
    _playbackManager->checkIn(engine);
}

void Control::trickCommand(int fast, int dir)
{
    BDBG_MSG(("Control::trickCommand(%d,%d)",fast,dir));

    PlayEngine *currentPvr = playEngine();

    if (dir < 0 && cfg()->getBool(Config::CONTINUOUS_RECORD_ENABLED) && _live[currentDecode()] && _live[currentDecode()]->isDecoding())
    {
        playVideo(_continuous[currentDecode()],false,false,true);
        currentPvr = playEngine();
        if (currentPvr) {
            currentPvr->catchup(); /* skip to the end */
        }
    }

    if (!currentPvr)
        return;

    if ((currentPvr->state() == bplayback_state_paused ||
        currentPvr->state() == bplayback_state_frameadvance)
        && fast == 1)
    {
        currentPvr->frameadvance(dir);
    }
    else {
        currentPvr->trickModeInc(fast, dir);
    }
}

/*
 * Some input functions require knowledge of state or otherwise require more
 * complicated handling than makes sense in inputengine.cpp.  The functions
 * grouped here hold that logic.
 */
void Control::stopCommand()
{
    BDBG_MSG(("Control::stopCommand()"));
    if (isPlaying()) {
        setChannel(channel(), eDecode_Immediate, true);
    }
    /* Ignore pause command for HTTP Channels */
    /* stop is pause with rtsp (for now, because url comes from channels.txt and therefore, logically, we cannot stop it) */
    else if (isRtspChannel(currentDecode())) {
        if (playEngine()->state() != bplayback_state_paused)
            pauseCommand();
    }
    else {
        /* stop first record that's not continuous record */
        for (RecordingStream *rs = _records.last(); rs; rs = _records.prev()) {
            Video *vid = rs->record->video();
            /* there may not be a Video because Record may be waiting for an AVStream::checkLock, which may never come */
            if (vid && vid->isContinuousRecord) {
                continue;
            }

            BDBG_ASSERT(rs->stream);
            stopRecord(rs->stream);
            MMessageBox::show(fb(), "Record stopped", 0, 0, 1000);
            break;
        }
    }
}

void Control::pauseCommand()
{
    BDBG_MSG(("Control::pauseCommand()"));
    /* Are we pausing live TV? */
    if (cfg()->getBool(Config::CONTINUOUS_RECORD_ENABLED) && _live[currentDecode()] && _live[currentDecode()]->isDecoding()) {
        if (_live[currentDecode()]->recordEngine()) {
            playVideo(_live[currentDecode()]->recordEngine()->video(),false,false,true);
        } else {
            playVideo(_continuous[currentDecode()],false,false,true);
        }
        playEngine()->catchup();
        playEngine()->pause();

        /* TODO: replace sleep + frameadvance with stop decode + display last picture. */
        BKNI_Sleep(300);
        playEngine()->frameadvance(1);
    }
    else {
        if (playEngine())
            playEngine()->pause();
    }
}

PlayEngine *Control::playEngine(int index)
{
    if (index == -1)
        index = currentDecode();

    if (isHttpChannel(index) || isRtspChannel(index))
        return _live[index] ? _live[index]->playEngine() : NULL;
    else if (isLiveIpChannel(index))
        /* For Live IP Channels, we dont want to return the associated playengine as we dont want PVR timeline for Live IP Channels */
        return NULL;
    else
        return _decode[index] ? _decode[index]->playEngine() : NULL;
}

RecordEngine *Control::recordEngine(Video *video)
{
    /* Find the Video */
    for (RecordingStream *rs = _records.first(); rs; rs = _records.next()) {
        if (rs->record->video() == video) {
            return rs->record;
        }
    }
    return NULL;
}

bool Control::isPlaying(int index) const {
    if (index == -1)
        index = currentDecode();
    return _decode[index] && _decode[index]->isPlaying();
}

void Control::onStateChanged(PlayEngine *playEngine) {
    if (_currentScreen == ePlayback)
        playbackScreen()->refreshList();
}

#if B_HAS_DRM
void Control::onDRMLicenseStateChanged(PlayEngine *playEngine)
{
    bdrm_license_state lic_state = playEngine->DRMLicenseState();
    MString            strMsg;

    switch (lic_state)
    {
    case bdrm_lic_pending:      /* license is about to be acquired/loaded */
        strMsg = "Requesting DRM License";
        break;
    case bdrm_lic_transferring: /* license is currently being transferred from a remote location */
        strMsg = "Transferring DRM License";
        break;
    case bdrm_lic_localstore:   /* a valid license was found in the local store */
        strMsg = "Local DRM License Loaded";
        break;
    case bdrm_lic_acquired:     /* a valid license was loaded from the inet */
        strMsg = "Remote DRM License Loaded";
        break;
    case bdrm_lic_faillocal:    /* could not load a license, locally from the hds */
        strMsg = "Local DRM License Load Failure";
        break;
    case bdrm_lic_failtransfer:    /* could not transfer a license, */
        {
        strMsg = "DRM License Transfer Failure";

            bdrm_http_state http_state = playEngine->DRMHTTPState();
            switch (http_state)
            {
            case bdrm_http_status_failed_getpost:
                strMsg += " (getpost)";
                break;
            case bdrm_http_status_failed_connect:
                strMsg += " (connect)";
                break;
            case bdrm_http_status_failed_internal:
                strMsg += " (internal)";
                break;
            case bdrm_http_status_failed_response_read:
                strMsg += " (response read)";
                break;
            case bdrm_http_status_failed_response_code:
                strMsg += " (response code)";
                break;
            case bdrm_http_status_failed_no_license:
                strMsg += " (no license)";
                break;
            case bdrm_http_status_failed_generic:
                strMsg += " (generic)";
                break;
            case bdrm_http_status_ok:
                BDBG_WRN(("DRM license status indicates transfer failure but http state is ok - conflict!"));
                break;
            case bdrm_http_status_undefined:
                BDBG_WRN(("DRM license status indicates transfer failure but http state is undefined!"));
                break;
            default:
                BDBG_WRN(("DRM http state is invalid!"));
                break;
            }
        }
        break;
    case bdrm_lic_expired:      /* license was found but is expired */
        strMsg = "DRM License Expired";
        break;
    case bdrm_lic_failcount:    /* license was found but is played too many times */
        strMsg = "DRM License Exceeds Max Play Count";
        break;
    case bdrm_lic_stored:       /* license was stored in the local hds */
        strMsg = "DRM License Saved Locally";
        break;
    default:
        strMsg = "Please Wait";
        break;
    }

    BDBG_MSG(("########################################################"));
    BDBG_MSG(("DRM License State:%s", strMsg.s()));
    BDBG_MSG(("########################################################"));

    if (lic_state == bdrm_lic_failtransfer)
    {
        _msgDRMStatus->hide();
        _msgDRMStatus->set(strMsg.s(), 0);
        _msgDRMStatus->process(3000);
    }
    else
    if (lic_state == bdrm_lic_acquired)
    {
        _msgDRMStatus->hide();
    }
    else
    {
        _msgDRMStatus->hide();
        _msgDRMStatus->set(strMsg.s(), 0);
        _msgDRMStatus->show();
        _msgDRMStatus->drawNow();
    }
}

void Control::onDRMHTTPStateChanged(PlayEngine *playEngine)
{
    onDRMLicenseStateChanged(playEngine);
}
#endif

void Control::onStateChanged(RecordEngine *recordEngine) {
    if (_currentScreen == ePlayback)
        playbackScreen()->refreshList();
}

void Control::checkForRecordError(RecordEngine *rec) {
    brecord_error error;
    bool overflow;
    char buf[256];

    if (!rec->getError(&error, &overflow) || rec->getModalState() )
        return; /* no error */

    if (overflow) {
        sprintf(buf, "Record %d overflow", rec->index());
    }
    else {
        switch (error) {
        case brecord_error_disk_full:
            sprintf(buf, "Record %d: Disk full", rec->index());
            break;
        case brecord_error_max_file_size:
            sprintf(buf, "Record %d has reached the maximum size", rec->index());
            break;
        default:
            sprintf(buf, "Record %d stopped because of error.", rec->index());
            break;
        }
    }
    rec->setModalState(true);
    MMessageBox::info(fb(), buf);

    /* Except for overflow, all errors are fatal, so have the ui bring record
    down. This introduces recursion into RecordEngine, and so it's a bit dangerous.
    I put the stopRecord above the MMessageBox::info, and it caused erratic behavior
    that the debugger wasn't able to pinpoint. So beware that this might be problematic.
    If so, just remove it. */
    if (!overflow || (error == brecord_error_disk_full) ) {
        stopRecord(rec->video());
    }
    rec->setModalState(false);

    if (overflow) {
        rec->clearOverflowError();
    }

    tv()->updateRecordIndicator();
}


bool Control::recordVideoPrepare(Video* vid) {
    unsigned long total, avail;
    bool rc;

    /* check disk availability */
    if (!vid->isNetRecord) {
        if( b_get_disk_usage(_videos->defaultVideoDir(), &total, &avail) ) {
            BDBG_WRN(("Unable to get Disk Stats" ));
            MMessageBox::info(fb(), "Cannot get Disk Stats, aborting record", 5000);
            showScreen(Control::eHome);
            return false;
        }

        /* avail gives stats in 1024 blocks, so if less than 30MB, show disk as full */
        if (avail < 30000) {
            BDBG_WRN(("Disk is Full"));
            MMessageBox::info(fb(), "Disk Full, therefore cannot start record", 5000);
            showScreen(Control::eHome);
            return false;
        }
    }

    /* prepare record files */
    int nextid, sortIndex;

#ifdef DUAL_CMP_SUPPORT
    bool got_lock = b_acquire_system_lock() == 0;
    if (!got_lock) {
        /* there's a very good chance that we can't write to the filesystem, so we're going to fail later. */
        BDBG_ERR(("Unable to lock filesystem for PVR"));
        /* continue */
    }
#endif

    // generate the brutus file name
    _videos->findNextVideoIndex(&nextid, &sortIndex);
    MString cnt(nextid);
    MString filename("brutus"+cnt);

    /* if record filename was specified via brutus script, use it */
    if (vid->mediafile) {
        filename = vid->mediafile;
        int idx = vid->mediafile.findRev('.');
        if (idx!=-1) {
            filename.truncate(idx);
        }
    }
    else {
        vid->mediafile = filename+".mpg";
    }

    // read UI and current time
#ifndef _WIN32_WCE
    vid->recordtime = time(NULL);
#else
    vid->recordtime = timeGetTime();
#endif
    vid->sortIndex = sortIndex;

    vid->dirLabel = _videos->defaultVideoDirLabel();
    vid->setPath(_videos->defaultVideoDir());

    /* vid->title is set in recordscreen */

    vid->addPath(vid->infofile, filename+".info");
    vid->indexfile = filename+"";
    vid->description = _cfg->get(Config::NEW_VIDEO_DESCRIPTION);

    if (vid->isNetRecord==true) {
        /* Rewrite the name of the file before we do a network record */
        MString temp;
        temp.strncpy(_cfg->get(Config::IPSERVER_CLIENT_ID));
        vid->infofile = temp+"_"+filename+".info";
        vid->mediafile = temp+"_"+filename+".mpg";
        vid->indexfile = temp+"_"+filename+"";

        vid->port = _cfg->getInt(Config::IPSERVER_REC_PORT);
        strncpy(vid->ipv4_addr, _cfg->get(Config::IPSERVER_IPV4),16);
    }

#ifdef DUAL_CMP_SUPPORT
    /* we have to write it immediately so that another proccess can get its info file.
    now that the file's there, we can unlock */
    if(!vid->isNetRecord)
        vid->write();

    if (got_lock)
        b_release_system_lock();
#endif

#if 0
//testing, not a normal app option
    vid->indexfile.clear();
#else
    vid->indexfile += ".nav";
#endif

    return true;
}

bool Control::recordVideo(Video *vid, int ch) {
    RecordEngine *record = NULL;
    const char *msg = NULL;
    MMessageBox *msgbox = NULL;
    AVStream* stream;
    int rc;

    if (!vid->isBackgroundRecord || channel()==ch) { /* if not doing background record, show the current decode, then record it */
        int dec = currentDecode();
        stream = _live[dec];
        if (!stream) {
            /* If you're doing playback, stop it and see if we get live */
            /* TODO: it might be nice to determine if the live decode is going to work
            before killing the playback */
            if (isPlaying(dec)) {
                setChannel(ch, eDecode_Immediate);
                stream = _live[dec];
            }
            if (!stream) {
                msg = "No stream to record";
                goto error;
            }
        }
    }
    else { /* background record */
        if (ch==channel(0) && _live[0])
            stream = _live[0];
        else if (ch==channel(1) && _live[1])
            stream = _live[1];
        else { /* if currently not being decoded, try to tune */
            ChannelMap::Channel currentChannel;
            int dec = currentDecode();
            if (!_channelMap->getChannel(ch, &currentChannel) &&
                currentChannel.entry.getType() == btuner_ip_stream) {
                _ipVideo->isBackgroundRecord = vid->isBackgroundRecord;
                _ipVideo->decoderIndex = dec;
                stream = _avManager->tune(ch, false, _ipVideo, _playbackManager);
            }
            else {
                stream = _avManager->tune(ch, false, NULL, NULL);
            }

            if (!stream) {
                /* keep things simple and just fail if tune fails */
                msg = "Tuning failed while attempting background record";
                goto error;
            }
        }
    }
    vid->ch = ch;

    /* release the continuous recording */
    if (cfg()->getBool(Config::CONTINUOUS_RECORD_ENABLED)) {
        stopRecord(_live[currentDecode()]);
    }

    if (stream->isRecording()) {
        ChannelMap::Channel currentChannel;
        if (!_channelMap->getChannel(ch, &currentChannel) &&
            currentChannel.entry.getType() == btuner_ip_stream) {
            _ipVideo->decoderIndex = currentDecode();
            stream = _avManager->tune(ch, false, _ipVideo, _playbackManager);
        }
        else {
            stream = _avManager->tune(channel(), false, vid, _playbackManager); /* setting allowRecordingStream=true allows multiple recordings of the same channel */
        }

        if (!stream) {
            msg = "Channel is already being recorded and another tuner isn't available.";
            goto error;
        }
        stream->setCallback(avstreamCallback, this);
    }

    record = _recordManager->checkOut();
    if (!record) {
        msg = "No record channel available";
        goto error;
    }

    _leds->write("REC");
    rc = stream->startRecord(record, _decodeManager, vid);
    BDBG_MSG(("startRecord returned: %d",rc));
    switch (rc) {
    case 0: break;
    case -2: msg = "Already recording this stream"; break;
    case -3: msg = "No encoder available for analog stream"; break;
    case -4: msg = "Unable to start the encoder"; break;
    case -5: msg = "Unable to create new file"; break;
    default: msg = "Unable to start record"; break;
    }
    if (rc) goto error;

    /* We are recording, so keep track and update the UI */
    _records.add(new RecordingStream(stream, record));
    tv()->updateRecordIndicator();
    updateLeds();

    if (vid->isContinuousRecord) {
        return true; /* we're finished */
    }

    if (vid->infofile && !vid->isContinuousRecord && !vid->isNetRecord ) {
        vid->write(); /* write info file */
    }

    if (vid->startPlaybackOfRecord && !vid->isNetRecord)
    {
        msgbox = new MMessageBox(fb(), 350, 100);
        msgbox->set("Starting Record & Playback...", 0);
        msgbox->show();
        msgbox->drawNow();

        /**
        * We have to wait until some of the index is written to disk for these reasons:
        * 1) play shouldn't get too close to record, or decoder gets starved and there's
        *    a hickup.
        * 2) bcmplayer NAV version detection
        * 3) hits detection
        **/
        int cnt = 0;
        /* PR17632 TODO: Implement a better solution to waiting 2 seconds.
        See nexus/examples/dvr/timeshift.c. We can poll on NEXUS_RecordStatus.picturesIndexed. */
#define PLAY_RECORD_GAP_IN_FRAMES 60
        while (!vid->indexfile ||
            vid->lastIndex() < PLAY_RECORD_GAP_IN_FRAMES)
        {
            BKNI_Sleep(100); // milliseconds
            if (!vid->indexfile) {
                if (cnt++ == 2 * 10) // 2 seconds are needed just to gap play and record
                    break;
            }
            else {
                if (cnt++ == 5 * 10) {
                    /* After 5 seconds, we don't have an index. Therefore bcmplayer
                    is not going to be able to be opened.
                    This can happen because the disk is full, or there is no video
                    data, therefore no index. In any case, playback is going to fail,
                    so notify the user and keep record going. */
                    rc = false;
                    BDBG_WRN(("No index data recorded, therefore we cannot start playback"));
                    if (_cfg->getBool(Config::PROMPT_ENABLED))
                        MMessageBox::info(fb(), "No index data recorded, therefore we cannot start playback", 5000);
                    goto skip_play;
                }
            }
        }
        //printf("starting simul play at %d\n", _currentVideo->lastIndex());
        playVideo(vid);
    }
    else if (_cfg->getBool(Config::THUMBNAIL_MODE)) {
        BKNI_Sleep(3000);
    }

    /* now we're recording, start making thumbnails. we have to wait until the index file exists before opening bthumbnail. */
    vid->createThumbnails(thumbnail_manager());

skip_play:
    delete msgbox;
    if(!vid->isNetRecord)
        _videos->add(vid); /* add new video into playback list */

    return true;

error:
    if (record)
        _recordManager->checkIn(record);
    if (stream && !stream->isDecoding() && !stream->isRecording()) {
        /* Because it's not decoding, this must be a stream we just tuned. */
        _avManager->untune(stream);
    }
    if (cfg()->getBool(Config::PROMPT_ENABLED))
        MMessageBox::info(fb(), msg);
    return false;
}

bool Control::stopRecordCh(int channel)
{
    /* stop the last record with the given channel number, if one exists */
    int i = _records.total()-1;
    for (RecordingStream *rs=_records.last(); rs; rs=_records.prev(), i--) {
        if (rs->record && rs->record->video()->ch == channel) {
            stopRecord(i);
            return true;
        }
    }
    return false;
}

int Control::stopRecordLast()
{
    /* stop the record started last */
    int i = _records.total()-1;
    int ch;
    for (RecordingStream *rs=_records.last(); rs; rs=_records.prev(), i--) {
        if (rs->record) {
            ch = rs->record->video()->ch;
            stopRecord(i);
            return ch;
        }
    }
    return -1;
}

int Control::stopRecordAll()
{
    int count = totalRecords();
    while (totalRecords()) {
        stopRecord(totalRecords()-1);
    }
    return count;
}

void Control::stopRecord(Video *video)
{
    int i = 0;
    for (RecordingStream *rs = _records.first(); rs; rs = _records.next(), i++) {
        /* stop actual record */
        if (rs->record && rs->record->video() == video) {
            stopRecord(i);
            break;
        }
        /* stop pending AVStream::checkLock record */
        else if (rs->stream && rs->stream->onLockVideo() == video) {
            stopRecord(i);
            break;
        }
    }
}

void Control::stopRecord(AVStream *avstream)
{
    int i = 0;
    for (RecordingStream *rs = _records.first(); rs; rs = _records.next(), i++) {
        if (rs->stream == avstream) {
            stopRecord(i);
            break;
        }
    }
}

void Control::stopRecord(int index) {
    RecordingStream *rs = _records[index];
    if (!rs)
        return;
    BDBG_ASSERT(rs->stream);
    BDBG_ASSERT(rs->record);

    BDBG_MSG(("stopRecord(%d): %p %p", index, rs->record->video(), rs->stream->onLockVideo()));

    /* This AVStream may not be recording. It may be waiting for an AVStream::checkLock. */
    rs->stream->stopRecord();

    /* All AVStreams must be either in _live[] or _records[]. If it's only in _records[], we need to untune. */
    if (_live[0] != rs->stream && _live[1] != rs->stream) {
        _avManager->untune(rs->stream);
        rs->stream = NULL; /* the memory is deallocated */
    }

    /* we're done with this record */
    _recordManager->checkIn(rs->record);
    _records.remove(rs);
    delete rs;

    updateLeds();
    tv()->updateRecordIndicator();

    /* update the playback screen. if there's an AVStream::checkLock record pending, there will be no notification to PlaybackScreen */
    if (_currentScreen == ePlayback)
        playbackScreen()->refreshList();
}

void Control::stopPlayback(Video *video, bool startLiveDecode, bool ignoreContinuous)
{
    BDBG_MSG(("Control::stopPlayback(%p,%s,%s)",video,startLiveDecode?"true":"false",ignoreContinuous?"true":"false"));
    for (int i=0;i<_totalDecodes;i++) {
        if (_decode[i] && _decode[i]->isPlaying()) {
            if (_decode[i]->playEngine()->video() == video) {
                if (startLiveDecode) {
                    int keep = _currentDecode;
                    _currentDecode = i;
                    setChannel(channel(i), eDecode_Immediate, ignoreContinuous, i); /* this will stop playback */
                    _currentDecode = keep;
                }
                else
                    stopPlayback(i);
                break;
            }
        }
    }
}

void Control::stopPlayback(int index) {
    BDBG_MSG(("Control::stopPlayback(%d)",index));
    if (index == -1)
        index = currentDecode();
    DecodeEngine *decode = _decode[index];
    if (!decode || !decode->isPlaying())
        return;

#if B_HAS_DRM
    /* hide is drm dialog box */
    if(decode->playEngine()->video()->isForPurchase)
        _msgDRMStatus->hide();
#endif

    if (cfg()->getBool(Config::RESUME_ENABLED)) {
        unsigned long currentIndex;
        Video *vid = decode->playEngine()->video();
        BDBG_ASSERT(vid);
        decode->stopPlayback(&currentIndex);
        vid->resumeindex = currentIndex;
        vid->write();
    }
    else {
        /* pass NULL so that no delay will happen */
        decode->stopPlayback();
    }
    fireEvent(evStopDecode);
    /* Update PVR timeline with new PlayEngine (currently NULL, this will remove the timeline) */
    tv()->setPlayEngines();

    updateLeds();

}

bool Control::deleteStream(int index)
{
    return playbackScreen()->deleteVideo(index);
}

AVStream* Control::getLiveStream(PlayEngine *playEngine) const
{
    /* find the AVStream by looking up the brecord_t in the list of current records. */
    if (playEngine && playEngine->isActive() && playEngine->recordHandle()) {
        /* find the RecordEngine */
        MListItr<RecordingStream> itr(&_records);
        for (RecordingStream *rs = itr.first(); rs; rs = itr.next()) {
            if (rs->record && rs->record->handle() == playEngine->recordHandle()) {
                return rs->stream;
            }
        }
    }
    return NULL;
}

bool Control::playVideo(Video *vid, bool resume, bool catchup, bool ignoreContinuous) {
    DecodeEngine *d = _decode[currentDecode()];

    if (!d) {
        BDBG_ERR(("Cannot play video because there's no decoder"));
    }

    stopLiveDecode(-1, ignoreContinuous);
    if (!d || d->startPlayback(vid, resume, catchup, vid->isContinuousRecord?recordEngine(vid):NULL)) {

        if (cfg()->getBool(Config::PROMPT_ENABLED))
            MMessageBox::info(fb(), "Unable to play video");
        // start live decode again
        setChannel(channel());
        return false;
    }
    fireEvent(evStartDecode);
    /* Update PVR timeline with new PlayEngine info */
    tv()->setPlayEngines();

    _leds->write("PLAY");
    updateLeds();
    showModeMessage();
    return true;
}

void Control::stopAllPvr()
{
    BDBG_MSG(("Control::stopAllPvr"));
    for (int i=0;i<_totalDecodes;i++) {
        stopPlayback(i);
    }
    while (totalRecords()) {
        stopRecord(totalRecords()-1);
    }
    if (cfg()->getBool(Config::CONTINUOUS_RECORD_ENABLED)) {
        stopRecord(_live[0]);
        stopRecord(_live[1]);
    }
}

#if B_HAS_TRANSCODE
bool Control::transcodePlaybackVideo(Video *vid, bool ignoreContinuous) {
    DecodeEngine *d = _decode[currentDecode()];
    BDBG_ASSERT(d);

    stopLiveDecode(-1, ignoreContinuous);

    if (d->startTranscodeOfPlayback(vid)) {

        if (cfg()->getBool(Config::PROMPT_ENABLED))
            MMessageBox::info(fb(), "Unable to play video");
        // start live decode again
        setChannel(channel());
        return false;
    }
    fireEvent(evStartDecode);
    /* Update PVR timeline with new PlayEngine info */
    tv()->setPlayEngines();

    _leds->write("XCD");
    updateLeds();
    showModeMessage();
    return true;
}

#endif

#endif /* PVR_SUPPORT */

