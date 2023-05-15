/***************************************************************************
 *     Copyright (c) 2002-2009, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: decodeengine.h $
 * $brcm_Revision: 13 $
 * $brcm_Date: 7/9/09 11:51a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/avmanager/decodeengine.h $
 * 
 * 13   7/9/09 11:51a jtna
 * PR56522: Remove multi-decode fast channel change feature. To be
 * replaced with video primer FCC.
 * 
 * 12   5/11/09 5:12p erickson
 * PR54915: remove unused and misleading code
 *
 * 11   9/15/08 12:40p tokushig
 * PR43426: merge to main
 *
 * BRUTUS_97405_97043/1   9/3/08 2:51p tokushig
 * PR43426: use transcode handle opened in control in avstream.  do not
 * open another one in startLiveTranscode
 *
 * 10   7/30/08 12:35p shyam
 * PR 45076 : Add trasncode support for playback streams
 *
 * 9   5/27/08 4:43p jtna
 * PR42200: Multi-decode fast channel change feature
 *
 * 8   11/2/06 11:03a arbisman
 * PR23775: Modified to use playback manager
 *
 * PROD_IPSTB_REFSW_Devel/1   10/12/06 7:20p arbisman
 * PR23775: Add playback manager
 *
 * 7   9/14/06 9:57a jgarrett
 * PR 22643: Removing restartAudio()
 *
 * PR22634/1   9/11/06 5:24p jgarrett
 * PR 22643: Removing restartAudio()
 *
 * 6   3/15/06 4:03p mphillip
 * PR19786: Initial implementation.  Add CONTINUOUS_RECORD_ENABLED=1 to
 * brutus.cfg to enable
 *
 * 5   2/13/06 11:31a erickson
 * PR17108: move bdecode calls into DecodeEngine
 *
 * 4   8/24/05 6:10p arbisman
 * PR16678: Add support for WinCE
 *
 * 3   8/24/05 11:09a erickson
 * PR16819: return bstream_t, either live or pvr
 *
 * 2   3/10/05 10:15a erickson
 * PR14354: In HDSD_SIMUL or HDSD_SINGLE, vbi needs to go out 2nd display
 * which is always SD
 *
 * 1   2/7/05 7:53p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 *
 * Irvine_BSEAVSW_Devel/2   4/30/04 5:28p erickson
 * PR10832: more debug related to new channelmgr/avmanager
 *
 * Irvine_BSEAVSW_Devel/1   4/30/04 12:06p erickson
 * PR10832: rework of brutus to support single AVManager and
 * ChannelManager for system
 *
 ******************************************************************************/
#ifndef DECODEENGINE_H__
#define DECODEENGINE_H__

#include <bsettop.h>
#include <mlist.h>
#include "playengine.h"
#include "recordengine.h"


class AVStream;
class DecodeManager;

/**
TODO: PlayEngine should produce an AVStream and AVStream::startDecode should decode it.
Handle it the same way as live.
**/
class DecodeEngine {
    friend class AVStream; // allow AVStream::startDecode to call in
    friend class DecodeManager; // allow takeAudioDecode to be called
public:
    struct Settings {
        DecodeManager *mgr;
#ifdef PVR_SUPPORT
        PlaybackManager *playManager;
        PlayEngine *playEngine;
#endif
        bdecode_t decode;
        bool audioEnabled; /* You must tell DecodeEngine if there is a baudio_decode
                                attached to the bdecode_t handle. If you misreport
                                this, you'll get an error message when something
                                unexpected happens. */
        bdecode_window_t window;
        bdisplay_t vbi_display;
#if B_HAS_TRANSCODE
        btranscode_t transcode ;
#endif
    };

    DecodeEngine(const Settings *settings);

    void getSettings(Settings *settings) const {*settings = _settings;}
    bstream_t getStream() const {return _stream;}

    bool isActive() const {return (_stream != 0);}
    bool isLiveDecode() {return _stream && _liveStream;}
    AVStream *liveStream() const {return _liveStream;}

#ifdef PVR_SUPPORT
    int startPlayback(Video *vid, bool resume = false, bool catchup = false, RecordEngine *recordEngine = NULL);
    void stopPlayback(unsigned long *currentIndex = NULL);
    bool isPlaying() const {return _stream && !_liveStream;}
    PlayEngine *playEngine() const {return _settings.playEngine;}

#if B_HAS_TRANSCODE
    btranscode_t transcode() {return _settings.transcode;}
    int startTranscodeOfPlayback(Video *vid)  ;
#endif

#else
    bool isPlaying() const {return false;}
#endif

    /* Stop either live decode or playback */
    void stop();

    /**
    Enable audio decode by attaching a baudio_decode_t resource.
    If it does not have one, it will see if one is available from another
    DecodeEngine through the AVManager.
    **/
    int enableAudio();

    /**
    Disable audio decode by detaching a baudio_decode_t resource.
    After it detaches, the audio decode might be grabbed by another DecodeEngine
    through the AVManager.
    **/
    int disableAudio();

    void setAudioProgram(unsigned program);

    /**
    Returns true if a baudio_decode_t is attached to the bdecode_t.
    Doesn't matter is audio is actually decoding or not.
    **/
    bool isAudioEnabled() const {return _settings.audioEnabled;}

    /* You can recreate the window midstream. You'll need to restart decode. */
    void setWindow(bdecode_window_t window);

protected:
    Settings _settings;

    /* We keep DecodeEngine::startLive private so that AVStream has a consist start/stop
    interface for both record and decode. */
    int startLive(AVStream *stream);
    void stopLive();

    /* This takes away a baudio_decode_t if it is already detached. */
    baudio_decode_t takeDetachedAudio();

    int startVbi();
    int stopVbi();

    AVStream *_liveStream;
    bstream_t _stream; /* current stream being decoded */
    int _isHD; /* -1 unknown, 0 no, 1 yes */
    baudio_decode_t _audio; /* detached audio decode */
};

/**
Summary:
Manage interdependencies between DecodeEngine's.

Description:
You should instantiate two DecodeManager per application instance. Like the DecodeEngine,
it is not re-entrant.

DecodeManager is not a global resource because the DecodeEngine's are not shared.
This means that you cannot run in timeshifting mode with a video being recorded
on one application and played on another.

Detaching and attaching audio is usually used for sharing a single audio decode
between main and PIP, which is done in one application.
**/
class DecodeManager {
public:
    void add(DecodeEngine *decode) {_decodes.add(decode);}

#ifdef PVR_SUPPORT
    /**
    Stop timeshifting on all DecodeEngine's connected to this RecordEngine.
    **/
    void stopTimeshifting(RecordEngine *rec);
#endif

    /**
    See if any DecodeEngine has a baudio_decode_t which is available, and if
    so, take it.
    **/
    baudio_decode_t takeDetachedAudio();

protected:
    MList<DecodeEngine> _decodes;
};

#endif // DECODEENGINE_H__
