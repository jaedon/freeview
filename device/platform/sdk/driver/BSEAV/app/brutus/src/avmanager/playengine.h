/***************************************************************************
 *     Copyright (c) 2002-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: playengine.h $
 * $brcm_Revision: 23 $
 * $brcm_Date: 12/7/10 7:28p $
 *
 * Module Description: Config is pvrgui's registry
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/avmanager/playengine.h $
 * 
 * 23   12/7/10 7:28p piyushg
 * SW7405-4386:Initial intigration of playready with mainline brutus
 * 
 * 22   3/20/08 1:49a erickson
 * PR40307: update
 * 
 * 21   3/19/08 9:35p erickson
 * PR40307: thumbnail update
 * 
 * 20   3/18/08 4:05p erickson
 * PR40307: added thumbnail feature
 * 
 * 19   2/13/08 4:52p tokushig
 * PR39534: added bluetooth remote capabilities to brutus
 * 
 * 18   1/16/08 4:07p ssood
 * PR38608: Merging from Netacel branch to main branch
 * 
 * 17   1/3/08 10:46a erickson
 * PR37038: convert continuous record to use beginning_of_stream callbacks
 * 
 * PR37038/1   12/11/07 2:51p erickson
 * PR37038: convert continuous record to use beginning_of_stream callbacks
 * 
 * Netaccel_Devel/1   12/18/07 6:02p ssood
 * PR38179: HTTP trickmodes enhancements
 * 
 * 16   12/6/07 2:08p erickson
 * PR37038: add CONTINUOUS_RECORD_DURATION, remove unused fixedSize and
 * CIRCFILE.
 * 
 * SanDiego_Brutus_3D/1   12/5/07 5:10p tokushig
 * gfx3d integrated but doesn't play nice with bwin yet.
 * 
 * 15   9/25/07 2:48p gmohile
 * PR 34640 : Add Divx DRM support
 * 
 * 14   4/19/07 6:42p jrubio
 * PR30010: added tcp fields where neccessary....
 * 
 * 13   3/9/07 2:11p piyushg
 * PR28588: Add DRM-NDR support in brutus
 * 
 * 12   11/30/06 5:48p ssood
 * PR26252: IP STB SW unification w/ Generic Brutus release
 * 
 * 11   11/17/06 1:12p erickson
 * PR25538: added setTrickMode for random access to trick modes from
 * scriptengine
 * 
 * 10   11/1/06 4:00p arbisman
 * PR23775: Added playback manager
 * 
 * 9   9/28/06 7:04p arbisman
 * PR24289: Backed-out changes
 * 
 * 7   9/8/06 5:21p tokushig
 * PR20685: add support for simple/main profile in brutus
 * 
 * SanDiego_DRM_ASF/2   9/7/06 5:21p tokushig
 * handle http status message callback
 * 
 * 6   7/26/06 2:15p tokushig
 * PR20685: added DRM support to brutus
 * 
 * SanDiego_DRM_ASF/1   7/20/06 3:25p tokushig
 * add drm handling in the playengine
 * 
 * 5   6/5/06 3:36p mphillip
 * PR19786: Handle trimmed areas better
 * 
 * 4   3/15/06 4:03p mphillip
 * PR19786: Initial implementation.  Add CONTINUOUS_RECORD_ENABLED=1 to
 * brutus.cfg to enable
 * 
 * 3   9/12/05 10:40a erickson
 * PR17033: removed external trick mode options, and made FORCE_HOST and
 * FORCE_I_FRAME set frame advance correctly
 * 
 * 2   6/29/05 9:05a jjordan
 * PR16042: Add support for "IP channels"
 * 
 * 1   2/7/05 7:54p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/24   2/4/05 10:47a erickson
 * PR12728: cleanup for -pedantic warnings and errors
 * 
 * Irvine_BSEAVSW_Devel/23   2/2/05 10:09a erickson
 * PR14006: call file close
 * 
 * Irvine_BSEAVSW_Devel/22   12/14/04 12:51p erickson
 * PR13494: the test for valid PlayEngine is playpump, not playback
 * 
 * Irvine_BSEAVSW_Devel/21   11/23/04 8:34p vsilyaev
 * PR 13351: Refactoring pvr  API.
 * 
 * Irvine_BSEAVSW_Devel/20   9/15/04 10:02a erickson
 * PR12648: removed unused cfg settings and modified LOOP_PLAYBACK setting
 * 
 * Irvine_BSEAVSW_Devel/19   5/25/04 11:50a erickson
 * PR11135: handle bad PlayEngine creation more gracefully
 *
 * Irvine_BSEAVSW_Devel/18   4/30/04 12:06p erickson
 * PR10832: rework of brutus to support single AVManager and
 * ChannelManager for system
 *
 * Irvine_BSEAVSW_Devel/17   1/29/04 2:26p erickson
 * PR9107: merging back from BRUTUS_PR9107 branch for bwin development
 *
 * Irvine_BSEAVSW_Devel/16   11/18/03 10:20a erickson
 * PR8612: added more generic vbi support. should handle playback, pip,
 * etc. needs more testing
 *
 ****************************************************************************/
#ifndef PLAYENGINE_H__
#define PLAYENGINE_H__

#include "bsettop.h"
#include <mtimer.h>
#include <mstring.h>
#include "instmanager.h"

#if B_HAS_DRM
    #include "blst_slist.h"                     /* due to basf_parser.h*/
    #if B_HAS_DRM_PRDY
    #include "prdy_decryptor.h"                   /* drm */
    #include "prdy_core.h"                        /* drm */
    #else
    #include "drmdecryptor.h"                   /* drm */
    #include "drmcore.h"                        /* drm */
    #endif
    #include "mmessagebox.h"
#endif

#ifdef B_HAS_DIVX_DRM
#include "bdrm_decrypt.h"
#endif

class MApplication;
class Config;
class RecordEngine;
class Video;

BEGIN_EV(PlayEngine)
    EV_METHOD(PlayEngine, StateChanged)
    EV_METHOD(PlayEngine, PosChanged)
    EV_METHOD(PlayEngine, FileChanged)
    EV_METHOD(PlayEngine, RecordError)
    EV_METHOD(PlayEngine, SpecialStateMessage)
#if B_HAS_DRM
    EV_METHOD(PlayEngine, DRMLicenseStateChanged)
    EV_METHOD(PlayEngine, DRMHTTPStateChanged)
#endif
END_EV()

/**
Summary:
    PlayEngine is a C++ wrapper around the bplayback interface.
Description:
    It provides events which can be tied into the GUI.
    It reads bplayback position on a timer
    It translates trick mode rates into bplayback trick modes.
**/
class PlayEngine : public MTimerEventHandler {
public:
    /**
    * name = used for debug messages
    **/
    PlayEngine(MApplication *app, Config *cfg, const char *name, int index);
    virtual ~PlayEngine();

    bool valid() const {return _playpump != NULL;}
    int index() const {return _index;}
    const char *name() const {return _name;}
    bplayback_t handle() const {return _play;}

    /**
    * If rec is set, then you're doing simul rec/play. The play engine will
    * wait for the record engine at the end of the file. */
    bstream_t start(Video *video, int start_at_sct, brecord_t record = NULL, RecordEngine *recordEngine = NULL);

    int stop(unsigned long *currentIndex = NULL);

    bstream_t stream() const {return _stream;}
    Video *video() const {return _video;}

    /* If record() is not NULL, this PlayEngine is timeshifting. */
    brecord_t recordHandle() const {return _record;}

    /**
    Resume normal play
    **/
    int play();
    int stopTimeshifting();
    int pause();

    /**
    * Set the trick mode by rate
    **/
    int trickMode(int fastTrickMode, int rate = -1);
    int trickModeHttp( int rate = -1);
    
    /**
    * Increment the trick mode which was set by rate
    **/
    int trickModeInc(int fastTrickMode, int inc);

    /**
    * Set specific trick mode
    **/
    int setTrickMode(const bplayback_trickmode_params *trickmode_params);

    /**
    * trickModeRate returns the rate value specified or derived from the
    * previous call to trickMode().
    */
    int trickModeRate() const {return _trickModeRate;}
    int isFastTrickMode() const {return _fastTrickMode;}

    int frameadvance(
        int dir         // 1 = fwd, -1 = rev
        );
    int catchup();
    int skip(int msec);
    int gotoTimestamp(unsigned timestamp);

    unsigned long pts() const {return _current.pts;}
    
    /**
    The pos, lastpos, firstpos functions return time in milliseconds.
    If the stream has no index, then PlayEngine will approximate a time based on file size & read offset.
    **/
    unsigned pos() const {return _current.pos;}
    unsigned lastpos() const {return _current.lastpos;}
    unsigned firstpos() const {return _current.firstpos;} // for non-circular files, this is always 0.
    
    bplayback_state state() const {return _state;}
    int direction() const {return _direction;}
    const char *stateStr() const;
    bool isActive() const {return valid() && _state > bplayback_state_stopped;} // common check

    // call this after an OnSpecialStateMessage event.
    const char *specialStateMessage() const {return _specialStateMessage;}

    // event handlers
    void onTimeout(MTimer *self);
    
    // flag an end-of-stream situation
    bool isStreamEnded() { return _isStreamEnded; }
    void clearStreamEnded() { _isStreamEnded = false; }
    
#if B_HAS_DRM
    bdrm_license_state DRMLicenseState() { return _drm_license_state; };
    bdrm_http_state DRMHTTPState() { return _drm_http_state; };
    void setDRMLicenseState(bdrm_license_state state);
    void setDRMHTTPState(bdrm_http_state state);
#endif
    bplaypump_t playpump() { return _playpump; };
    bplayback_t playback() { return _play; };
protected:
    Video *_video;
    bindex_format _indexFormat;
    MString _name;
    bplayback_t _play;
    bplaypump_t _playpump;
    brecord_t _record;
    bstream_t _stream;
    bplayback_file_t _file;
    int _trickModeRate, _fastTrickMode;
    int _index;
    MTimer _timer;
    MString _specialStateMessage;
    MString _trickModeState;
    Config *_cfg;
    bool _isStreamEnded;
    struct playbackpos {
        long pos, firstpos, lastpos;
        unsigned long pts;
    } _current;

    /* maintain pointers for both regular playback and IP channel playback */
    bplayback_t _playIP;
    bplayback_t _playRegular;
    int _playSpeedIndex;

#if B_HAS_DRM
    bdrm_cfg                _drm_cfg; 
    bdrm_t                  _drm;
    bdrm_decryptor_t        _dcrypt;
    bplaypump_open_params   _open_params;
    bdrm_license_state      _drm_license_state;
    bdrm_http_state         _drm_http_state;
#endif /* B_HAS_DRM */

#ifdef B_HAS_DIVX_DRM
    bdrm_decrypt_t          _divx_decrypt;
#endif


    void setState(bplayback_state s, int direction = 1);

    SUPPORT_EV(PlayEngine)
    SUPPORT_EV_METHOD(StateChanged)
    SUPPORT_EV_METHOD(PosChanged)
    SUPPORT_EV_METHOD(FileChanged)
    SUPPORT_EV_METHOD(SpecialStateMessage)
#if B_HAS_DRM
    SUPPORT_EV_METHOD(DRMLicenseStateChanged)
    SUPPORT_EV_METHOD(DRMHTTPStateChanged)
#endif

private:
    int _cachedlastpos, _cachedfirstpos, _cachedpos;
    bplayback_state _state;
    int _direction;

    static bplayback_loopmode beginning_of_stream(void *context);
    static bplayback_loopmode end_of_stream(void *context);
};

/**
Use a PlaybackManager to check in/out PlayEngine's.
**/
class PlaybackManager : public InstanceManager<PlayEngine> {
};

#endif // PLAYENGINE_H__
