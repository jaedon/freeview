/***************************************************************************
 *     Copyright (c) 2002-2009, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: decodeengine.cpp $
 * $brcm_Revision: 21 $
 * $brcm_Date: 7/9/09 11:51a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/avmanager/decodeengine.cpp $
 * 
 * 21   7/9/09 11:51a jtna
 * PR56522: Remove multi-decode fast channel change feature. To be
 * replaced with video primer FCC.
 * 
 * 20   5/11/09 5:12p erickson
 * PR54915: remove unused and misleading code
 *
 * 19   3/25/09 7:08p jtna
 * PR51960: check-in playengine if playback_start fails to avoid mem leak
 *
 * 18   8/22/08 10:54a mward
 * PR44501: Fix build with PVR_SUPPORT=n.
 *
 * 17   7/30/08 12:35p shyam
 * PR 45076 : Add trasncode support for playback streams
 *
 * 16   7/2/08 2:26p ssood
 * PR44501: Info:Status doesn't show playback buffer fullness for Live IP
 * Channels
 *
 * 15   5/27/08 4:42p jtna
 * PR42200: Multi-decode fast channel change feature
 *
 * 14   5/9/08 12:01p erickson
 * PR42329: null _stream before stopping playback
 *
 * 13   9/12/07 2:39p gmohile
 * PR 33851 : Audio master mode
 *
 * 12   11/2/06 11:05a arbisman
 * PR23775: Modified to use playback manager
 *
 * PROD_IPSTB_REFSW_Devel/1   10/12/06 7:22p arbisman
 * PR23775: Reworked to use playback manager
 *
 * 11   9/28/06 6:56p arbisman
 * PR24289: Back-out changes
 *
 * 9   9/14/06 9:57a jgarrett
 * PR 22634: Removing attach/detach when switching audio program.  Also
 * removing unnecessary restart audio routine.
 *
 * PR22634/1   9/11/06 5:23p jgarrett
 * PR 22634: Removing attach/detach when switching audio program.  Also
 * removing unnecessary restart audio routine.
 *
 * 8   3/15/06 4:03p mphillip
 * PR19786: Initial implementation.  Add CONTINUOUS_RECORD_ENABLED=1 to
 * brutus.cfg to enable
 *
 * 7   2/13/06 11:31a erickson
 * PR17108: move bdecode calls into DecodeEngine
 *
 * 6   10/21/05 3:04p dma
 * PR17733: auto channel/PID scan for IP playback
 *
 * 5   8/24/05 6:10p arbisman
 * PR16678: Add support for WinCE
 *
 * 4   8/8/05 9:08p dlwin
 * PR 16600: Fixed a problem with starting second audio channel when doing
 * dual decode.
 *
 * 3   8/4/05 12:44p erickson
 * PR12103: for dual audio decode systems, we can choose to handle
 * main/pip with one or two audio decoders. on the 97398, it must be two
 * audio decoders because of hsx/remux
 *
 * 2   3/10/05 10:15a erickson
 * PR14354: In HDSD_SIMUL or HDSD_SINGLE, vbi needs to go out 2nd display
 * which is always SD
 *
 * 1   2/7/05 7:53p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 *
 * Irvine_BSEAVSW_Devel/3   12/14/04 12:52p erickson
 * PR13494: we can have a DecodeEngine with no PlayEngine
 *
 * Irvine_BSEAVSW_Devel/2   4/30/04 5:28p erickson
 * PR10832: more debug related to new channelmgr/avmanager
 *
 * Irvine_BSEAVSW_Devel/1   4/30/04 12:06p erickson
 * PR10832: rework of brutus to support single AVManager and
 * ChannelManager for system
 *
 ******************************************************************************/
#include "decodeengine.h"
#include <assert.h>
#include "video.h"
#include "avmanager.h"
#include "recordengine.h"
#include <bstd.h>
BDBG_MODULE(decodeengine);

DecodeEngine::DecodeEngine(const Settings *settings)
{
    _settings = *settings;
    if (!_settings.audioEnabled) {
        /* this might return NULL if no audio decode exists. we might want to
        downgrade the settop api ERR to WRN because of this. */
        _audio = bdecode_detach_audio(_settings.decode);
    }
    else {
        _audio = NULL;
    }
    _stream = NULL;
    _liveStream = NULL;
}

void DecodeEngine::stop()
{
#ifdef PVR_SUPPORT
    if (isPlaying())
        stopPlayback();
    else
#endif
    if (isLiveDecode())
        stopLive();
    assert(!_stream);
}

#ifdef PVR_SUPPORT
int DecodeEngine::startPlayback(Video *vid, bool resume, bool catchup, RecordEngine *recordEngine) {
    stop();
    if (!_settings.playEngine)
    {
        if(!_settings.playManager)
            return -1;
        _settings.playEngine = _settings.playManager->checkOut();
        if(!_settings.playEngine)
        {
            BDBG_WRN(("All Play engines are busy"));
            return -1;
        }
    }

    //TODO: I've removed fixed size file support from PlayEngine
    _stream = _settings.playEngine->start(
            vid,
            resume?vid->resumeindex:(catchup?-1:0), // startAtIndex
            vid->record,
            recordEngine);

    setAudioProgram(0);

    if (!_stream || bdecode_start(_settings.decode, _stream, _settings.window)) {
        // if decode failed, we need to stop playback
        if (_stream) {
            _settings.playEngine->stop();
            _stream = NULL;
        }
        _settings.playManager->checkIn(_settings.playEngine);
        BDBG_ERR(("Unable to play video"));
        return -1;
    }

    vid->playRefCnt++;
    if (isAudioEnabled())
        startVbi();
    return 0;
}

#if B_HAS_TRANSCODE
int DecodeEngine::startTranscodeOfPlayback(Video *vid) {
    btranscode_settings transcode_settings ;
    bstream_t _newstream ;

    stop();
    if (!_settings.playEngine)
    {
        if(!_settings.playManager)
            return -1;
        _settings.playEngine = _settings.playManager->checkOut();
        if(!_settings.playEngine)
        {
            BDBG_WRN(("All Play engines are busy"));
            return -1;
        }
    }

    //TODO: I've removed fixed size file support from PlayEngine
    _stream = _settings.playEngine->start(vid, 0);

    setAudioProgram(0);

    btranscode_settings_init(&transcode_settings,
        bencode_quality_best,
        bencode_vpp_none);
    _newstream = btranscode_start(_settings.transcode, _stream, &transcode_settings);

    if (!_stream || bdecode_start(_settings.decode, _stream, _settings.window)) {
        // if decode failed, we need to stop playback
        if (_stream) {
            _settings.playEngine->stop();
            _stream = NULL;
        }
        BDBG_ERR(("Unable to play video"));
        return -1;
    }
    bdecode_stop(_settings.decode) ;

    if (!_newstream || bdecode_start(_settings.decode, _newstream, _settings.window)) {
        // if decode failed, we need to stop playback
        if (_newstream) {
            _settings.playEngine->stop();
            _newstream = NULL;
        }
        BDBG_ERR(("Unable to play video"));
        return -1;
    }

    vid->playRefCnt++;
    return 0;
}

#endif

void DecodeEngine::stopPlayback(unsigned long *currentIndex) {
    if (!isPlaying())
        return;

    Video *vid = _settings.playEngine->video();
    assert(vid);

    vid->playRefCnt--;
    stopVbi();
    _settings.playEngine->stopTimeshifting();

    /* No explicit bdecode_stop required. bplayback_stop will stop it. */

    _stream = NULL; /* null out stream before it goes invalid */
    _settings.playEngine->stop(currentIndex);
    _settings.playManager->checkIn(_settings.playEngine);
    _settings.playEngine = NULL;
}
#endif

int DecodeEngine::disableAudio() {
    if (!isAudioEnabled())
        return -1;

    stopVbi(); /* vbi is paired with audio decode */
    _audio = bdecode_detach_audio(_settings.decode);
    if (!_audio) {
        BDBG_ERR(("Expected audio decode to be present, but it wasn't"));
    }
    _settings.audioEnabled = false;
    return _audio?0:-1;
}

int DecodeEngine::enableAudio() {
    if (isAudioEnabled())
        return -1;

    /* If we don't have an audio decode to attach, then see if there
    is a global audio available. */
    if (!_audio) {
        /* Take away the audio decode from another DecodeEngine. */
        _audio = _settings.mgr->takeDetachedAudio();

        /* There's no audio decode to attach. We're done. */
        if (!_audio) {
            BDBG_WRN(("No audio decode available to take"));
            return -1;
        }
    }

    if (!bdecode_attach_audio(_settings.decode, _audio)) {
        startVbi();
        _audio = NULL;
        _settings.audioEnabled = true;
        return 0;
    }
    else
        return -1;
}

int DecodeEngine::startLive(AVStream *stream)
{
    stop();

    setAudioProgram(0);
    _stream = stream->liveStream();
    BDBG_MSG(("calling bdecode_start"));
    if(getenv("audio_master_mode"))
    {
           bdecode_settings decsettings;

           BDBG_WRN(("SETTING TSM MODE TO AUDIO MASTER"));
           bdecode_get(_settings.decode, &decsettings);
           decsettings.tsm.mode = bdecode_tsm_mode_audio_master;
           decsettings.tsm.sync_limit = 5000;
           bdecode_set(_settings.decode, &decsettings);
    }

#ifdef PVR_SUPPORT
/* PR44501: this enables info status to acquire playback ip stats for Live IP playback */
    if (!_settings.playEngine)
        _settings.playEngine = stream->playEngine();
#endif
    if (bdecode_start(_settings.decode, _stream, _settings.window)) {
        _stream = NULL;
        return -1;
    }

    /* vbi must be started after decode */
    if (isAudioEnabled())
        startVbi();
    _liveStream = stream;

    return 0;
}

void DecodeEngine::stopLive()
{
    if (!isLiveDecode())
        return;

    stopVbi();
    BDBG_MSG(("calling bdecode_stop"));
    bdecode_stop(_settings.decode);
#ifdef PVR_SUPPORT
    _settings.playEngine = NULL;
#endif
    _stream = NULL;
    _liveStream = NULL;
}

int DecodeEngine::startVbi()
{
    if (!_stream) return 0;

    /* decode based vbi */
    bstream_vbi_settings vbisettings;
    bstream_vbi_get_settings(_stream, &vbisettings);
    vbisettings.cc.display = _settings.vbi_display;
    return !bstream_vbi_set_settings(_stream, &vbisettings);
}

int DecodeEngine::stopVbi()
{
    if (!_stream) return 0;

    bstream_vbi_settings vbisettings;
    bstream_vbi_get_settings(_stream, &vbisettings);
    vbisettings.cc.display = NULL;
    return !bstream_vbi_set_settings(_stream, &vbisettings);
}

baudio_decode_t DecodeEngine::takeDetachedAudio()
{
    baudio_decode_t audio = _audio;
    _audio = NULL;
    return audio;
}

void DecodeEngine::setWindow(bdecode_window_t window)
{
    _settings.window = window;
}

void DecodeEngine::setAudioProgram(unsigned program)
{
    if (!_settings.decode) return;

    /* This no longer needs a detach/attach -- this minimizes restart lag */
    bdecode_set_audio_program(_settings.decode, program);
}

///////////////////////////////////////////////////////

#ifdef PVR_SUPPORT
void DecodeManager::stopTimeshifting(RecordEngine *rec)
{
    // this is called from AVStream and must be synchronized
    for (DecodeEngine *decode = _decodes.first(); decode; decode = _decodes.next()) {
        if (decode->isPlaying()) {
            PlayEngine *play = decode->playEngine();
            if (play->recordHandle() == rec->handle()) {
                play->stopTimeshifting();
                // don't break, there could be more than one
            }
        }
    }
}
#endif

baudio_decode_t DecodeManager::takeDetachedAudio()
{
    baudio_decode_t audio = NULL;
    for (DecodeEngine *decode = _decodes.first(); decode; decode = _decodes.next()) {
        audio = decode->takeDetachedAudio();
        if (audio)
            break;
    }
    return audio;
}
