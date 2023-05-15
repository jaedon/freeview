/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: playback.cpp $
* $brcm_Revision: 27 $
* $brcm_Date: 10/19/12 1:14p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/playback.cpp $
* 
* 27   10/19/12 1:14p jrubio
* SW7346-1071: fix beginning of stream segfault
* 
* 26   10/19/12 12:59p jrubio
* SW7346-1071: add playback start function cleanup playback start
* function
* 
* 25   10/19/12 10:36a tokushig
* SW7231-749: fix brutus shutdown by freeing resources in the proper
* order.
* 
* 24   10/12/12 1:00p jrubio
* SW7346-1004: added custom LUA ONLY trickmodes
* 
* 23   10/12/12 10:29a jrubio
* SW7346-1004: take out AutoBitRate flag it makes the playback diregard
* the NAV file
* 
* 22   10/11/12 10:12p jrubio
* SW7346-1004: set _currentVideo to NULL after stop
* 
* 21   10/11/12 7:52p jrubio
* SW7346-1004: remove timeline after stop
* 
* 20   10/11/12 5:03p jrubio
* SW7346-1004: reset timeline when switching playbacks
* 
* 19   10/11/12 2:37p jrubio
* SW7346-1004: simplify trickmodes for remotecontrol
* 
* 18   10/10/12 10:07p jrubio
* SW7346-1004: add additional trickmodes
* 
* 17   10/8/12 9:05a jrubio
* SW7346-1004: add rewind notifications
* 
* 16   10/3/12 5:14p jrubio
* SW7346-1004: add videoName function
* 
* 15   9/17/12 10:26a jrubio
* SW7346-1004: video object should have active record/playback flag
* 
* 14   7/9/12 4:14p jrubio
* SW7346-644: added counter for playback
* 
* 13   5/21/12 5:38p jrubio
* SW7346-644: use Video object more
* 
* 12   5/15/12 4:51p jrubio
* SW7346-644: add write info,parse media files
* 
* 11   5/10/12 3:41p tokushig
* SW7231-749: add support for playback notifications.
* 
* 10   4/20/12 2:21p jrubio
* SW7346-644: use GET_STR for default VIDEOS_PATH
* 
* 9   4/20/12 1:27p jrubio
* SW7346-644: add index support,rewind,fastforward host trickmodes
* 
* 8   4/19/12 11:13p jrubio
* SW7346-644: add start/stop/rewind/forward functions
* 
* 7   4/18/12 5:18p jrubio
* SW7346-644: add CPlaybackList
* 
* 6   4/18/12 12:40a jrubio
* SW7346-644: add refresh from disk function and new CPlaybackList class
* 
* 5   4/16/12 11:18a jrubio
* SW7346-644: close info file handle after reading
* 
* 4   4/15/12 12:14a jrubio
* SW7346-644: add playback stop function
* 
* 3   4/13/12 11:45a jrubio
* SW7346-644: modify info parsing logic, modify playback settings logic,
* modify playpump/playback open logic, complete playback Start function
* 
* 2   4/12/12 12:08a jrubio
* SW7346-644: Push pid information to pid class,start playback at
* resource init
* 
* 1   4/11/12 1:23p jrubio
* SW7346-644: add playback Class
* 
* 
* 
*********************************************************************************/
#include "band.h"
#include "playback.h"
#include "avtypes.h"
#include "xmltags.h"
#include "mxmlparser.h"
#include <unistd.h> //file i/o
#include <fcntl.h>

BDBG_MODULE(brutus_playback);


CPlayback::CPlayback(const char * name, const uint16_t number, CConfiguration * pCfg) :
  CResource(name, number, eBoardResource_playback, pCfg),
  _file(NULL),
  _stickyFile(NULL),
  _customFile(NULL)
{
    
}


CPlayback::~CPlayback()
{
    uninitialize();
}

void CPlayback::dump(void)
{
   BDBG_ERR(("playback number %d",_number));
}

static void endOfStreamCallback(void *context, int param)
{
    CPlayback * playback = (CPlayback *) context;
    BSTD_UNUSED(context);
    if (param) {
        BDBG_WRN(("end of stream"));
    }
    else { 
        BDBG_WRN(("beginning of stream"));
    }
    return;
}

eRet CPlayback::initialize()
{
    BDBG_MSG(("INIT!"));
    _playpump = NEXUS_Playpump_Open(_number, NULL);
    if (!_playpump)
    {
        BDBG_ERR(("Cannot Open Playpump"));
        return eRet_ExternalError;
    }
    _playback = NEXUS_Playback_Create();
    if (!_playback)
    {
        BDBG_ERR(("Cannot Open Playback"));
        NEXUS_Playpump_Close(_playpump);
        return eRet_ExternalError;
    }

    /* Initialize Playback Settings */
    NEXUS_Playback_GetDefaultSettings(&_playbackSettings);
    _playbackSettings.playpump = _playpump;
    _playbackSettings.beginningOfStreamAction = NEXUS_PlaybackLoopMode_ePlay; /* Normal Play at the beginning */
    _playbackSettings.beginningOfStreamCallback.context = this;
    _playbackSettings.beginningOfStreamCallback.callback = endOfStreamCallback;
    _playbackSettings.beginningOfStreamCallback.param = 0;
    _playbackSettings.endOfStreamAction = NEXUS_PlaybackLoopMode_eLoop; /* Do Loop Mode for Now*/
    _playbackSettings.endOfStreamCallback.callback = endOfStreamCallback;
    _playbackSettings.endOfStreamCallback.context = this;
    _playbackSettings.endOfStreamCallback.param = 1;
    _playbackSettings.enableStreamProcessing = false;
    if (NEXUS_Playback_SetSettings(_playback, &_playbackSettings)!= NEXUS_SUCCESS)
    {
        BDBG_ERR(("Cannot SetSettings"));
        return eRet_ExternalError;
    }

    NEXUS_Playback_GetDefaultTrickModeSettings(&_pTrickModeSettings);

    BDBG_MSG(("added playback/playpump #%d",_number));
    return eRet_Ok;
}

eRet CPlayback::uninitialize()
{
    if (NULL != _playback)
    {
        NEXUS_Playback_Destroy(_playback);
    }

    if (NULL != _playpump)
    {
        NEXUS_Playpump_Close(_playpump);
    }

    return eRet_Ok;
}

/* LUA playback */
eRet CPlayback::start(MString filename, MString indexname, MString path)
{
    
    eRet ret = eRet_Ok;
    NEXUS_Error rc = NEXUS_SUCCESS;
    MString fullFilePath;
    MString fullIndexPath;


    if(_currentVideo && _currentVideo->_isPlaybackActive)
    {
        BDBG_MSG(("Resuming Play back"));
        _trickModeRate = 1;
        NEXUS_Playback_Play(_playback);
        _trickModeState= "Play";
        goto done;
    }

    fullFilePath = path + filename;
    fullIndexPath = path + indexname;
    BDBG_MSG(("PLAYBACK START %s, %s",fullFilePath.s(),fullIndexPath.s()));


    _file = NEXUS_FilePlay_OpenPosix(fullFilePath.s(),fullIndexPath.s());
    if (!_file) 
    {
        BDBG_ERR(("can't open files: '%s' '%s'",fullFilePath.s(), fullIndexPath.s()));
        ret = eRet_InvalidParameter;
        goto error;
    }

    /* Start playback */
    NEXUS_Playback_GetDefaultStartSettings(&_playbackStartSettings);
    _trickModeRate =1;
    rc = NEXUS_Playback_Start(_playback, _file, &_playbackStartSettings);
    if(rc!=NEXUS_SUCCESS)
    {
        ret = eRet_ExternalError;
        goto error;
    }

    _currentVideo->_isPlaybackActive = true;
    _active = true;

done:
    _trickModeState= "Play";
    ret = notifyObservers(eNotify_PlaybackStateChanged, this);
    CHECK_ERROR_GOTO("error notifying observers", ret, error);


    return eRet_Ok;
error:
    return ret;
}

/* Playbck from Video Object */
eRet CPlayback::start()
{
    if (_currentVideo == NULL) 
    {
        BDBG_ERR(("Please setVideo(CVideo *Video) before calling %s ",__FUNCTION__));
        return  eRet_ExternalError;    
    }

    return start(_currentVideo->_mediaName,_currentVideo->_indexName, _currentVideo->_path);
}

/* Playbck from Video Object */
eRet CPlayback::play()
{
   NEXUS_Error ret = NEXUS_SUCCESS;

   if (_currentVideo == NULL ||  _currentVideo->_isPlaybackActive == true) 
   {
      BDBG_ERR(("No Video attached to this playback or Video is not currently Playing Back "));
      return  eRet_ExternalError;    
   }

   ret = NEXUS_Playback_Play(_playback);
   if (ret != NEXUS_SUCCESS)
   {
      BDBG_ERR(("Cannot Continue Playback"));
      return eRet_ExternalError;
   }

   return eRet_Ok;
}

bool CPlayback::isActive(void)
{
   if (_currentVideo == NULL || _currentVideo->_isPlaybackActive == false)
   {
      return false;
   }else
      return true;
      
}

eRet CPlayback::stop()
{
    eRet ret = eRet_Ok;

    if (_currentVideo == NULL || _currentVideo->_isPlaybackActive == false ) 
    {
        BDBG_ERR((" Playback is not active"));
        return eRet_ExternalError;
    }
    _trickModeState= "Stop";

    /* Close all Pids associate with this plyaback and close the PID channels */
    NEXUS_Playback_Stop(_playback);
    NEXUS_FilePlay_Close(_file);
    _currentVideo->_pidMgr.closePlaybackPidChannels(this);
    
    /* Reset structure ready to use again */
        /* Initialize Playback Settings */
    NEXUS_Playback_GetDefaultSettings(&_playbackSettings);
    _playbackSettings.playpump = _playpump;
    _playbackSettings.beginningOfStreamAction = NEXUS_PlaybackLoopMode_ePlay; /* Normal Play at the beginning */
    _playbackSettings.beginningOfStreamCallback.callback = endOfStreamCallback;
    _playbackSettings.beginningOfStreamCallback.param = 0;
    _playbackSettings.endOfStreamAction = NEXUS_PlaybackLoopMode_eLoop; /* Do Loop Mode for Now*/
    _playbackSettings.endOfStreamCallback.callback = endOfStreamCallback;
    _playbackSettings.endOfStreamCallback.param = 1;
    _playbackSettings.enableStreamProcessing = false;
    _playbackSettings.playpumpSettings.transportType = NEXUS_TransportType_eTs;
    _playbackSettings.playpumpSettings.timestamp.pacing = false;
    if (NEXUS_Playback_SetSettings(_playback, &_playbackSettings)!= NEXUS_SUCCESS)
    {
        BDBG_ERR(("Cannot SetSettings"));
        return eRet_ExternalError;
    }

    BKNI_Memset(&_playbackStartSettings,0,sizeof(_playbackStartSettings));
    NEXUS_Playback_GetDefaultTrickModeSettings(&_pTrickModeSettings);
    _active = false;
    _currentVideo->_isPlaybackActive = false;
    _currentVideo->closeVideo();

    _currentVideo = NULL;
    _trickModeRate =1;
    _trickModeState= "Stop";
    ret = notifyObservers(eNotify_PlaybackStateChanged, this);
    CHECK_ERROR_GOTO("error notifying observers", ret, error);

error:
    return ret;
}


eRet CPlayback::pause()
{
    eRet ret = eRet_Ok;

    NEXUS_Playback_Pause(_playback);

    _trickModeState= "Pause";
    ret = notifyObservers(eNotify_PlaybackStateChanged, this);
    CHECK_ERROR_GOTO("error notifying observers", ret, error);

error:
    return ret;
}

/* This TrickMode overRides the Regular Trickmode - Command Line ONLY */
eRet CPlayback::trickMode(CPlaybackTrickModeData * pTrickModeData)
{
    eRet ret = eRet_Ok;
    const char * buf = pTrickModeData->_trickModeBuf.s();
    NEXUS_Error rc = NEXUS_SUCCESS;


    if (!strcmp(buf, "play")) {
                rc = NEXUS_Playback_Play(_playback);
                if (rc!=NEXUS_SUCCESS)
                {
                    BDBG_WRN((" Play failed"));
                    ret = eRet_NotSupported;
                    goto error;
                }
            }
            else if (!strcmp(buf, "fa")) {
                rc = NEXUS_Playback_FrameAdvance(_playback, true);
                if (rc!=NEXUS_SUCCESS)
                {
                    BDBG_WRN((" Playback Frame Advaced failed"));
                    ret = eRet_NotSupported;
                    goto error;
                }
            }
            else if (!strcmp(buf, "fr")) {
                rc = NEXUS_Playback_FrameAdvance(_playback, false);
                if (rc!=NEXUS_SUCCESS)
                {
                    BDBG_WRN((" Playback Frame Advaced failed"));
                    ret = eRet_NotSupported;
                    goto error;
                }
            }
            else if (!strcmp(buf, "pause")) {
                rc = NEXUS_Playback_Pause(_playback);
                 if (rc!=NEXUS_SUCCESS)
                 {
                    BDBG_WRN((" Pause failed"));
                    ret = eRet_NotSupported;
                    goto error;
                 }
            }
            else if (!strcmp(buf, "i")) {
                NEXUS_PlaybackTrickModeSettings trickSettings;
                NEXUS_Playback_GetDefaultTrickModeSettings(&trickSettings);
                trickSettings.mode = NEXUS_PlaybackHostTrickMode_ePlayI;
                trickSettings.skipControl = NEXUS_PlaybackSkipControl_eDecoder;
                trickSettings.rateControl = NEXUS_PlaybackRateControl_eDecoder;
                rc = NEXUS_Playback_TrickMode(_playback, &trickSettings);
                if(rc != NEXUS_SUCCESS)
                 {
                    BDBG_WRN((" i frame trickmode failed error code %d",rc));
                    ret = eRet_NotSupported;
                    goto error;
                 }

            }
            else if (!strcmp(buf, "ip")) {
                NEXUS_PlaybackTrickModeSettings trickSettings;
                NEXUS_Playback_GetDefaultTrickModeSettings(&trickSettings);
                trickSettings.mode = NEXUS_PlaybackHostTrickMode_ePlayIP;
                trickSettings.skipControl = NEXUS_PlaybackSkipControl_eDecoder;
                trickSettings.rateControl = NEXUS_PlaybackRateControl_eDecoder;
                rc = NEXUS_Playback_TrickMode(_playback, &trickSettings);
                if (rc!=NEXUS_SUCCESS)
                 {
                    BDBG_WRN((" Ip Trickmode failed"));
                    ret = eRet_NotSupported;
                    goto error;
                 }
            }
            else if (!strcmp(buf, "all")) {
                NEXUS_PlaybackTrickModeSettings trickSettings;
                NEXUS_Playback_GetDefaultTrickModeSettings(&trickSettings);
                trickSettings.mode = NEXUS_PlaybackHostTrickMode_eNormal;
                trickSettings.skipControl = NEXUS_PlaybackSkipControl_eDecoder;
                trickSettings.rateControl = NEXUS_PlaybackRateControl_eDecoder;
                rc = NEXUS_Playback_TrickMode(_playback, &trickSettings);
                if (rc!=NEXUS_SUCCESS)
                 {
                    BDBG_WRN((" decode all failed"));
                    ret = eRet_NotSupported;
                    goto error;
                 }
            }
            else if (strstr(buf, "sleep(") == buf) {
                unsigned msec;
                sscanf(buf+6, "%u", &msec);
                BKNI_Sleep(msec);
            }
            else if (strstr(buf, "rate(") == buf) {
                NEXUS_PlaybackTrickModeSettings trickSettings;
                float rate;

                sscanf(buf+5, "%f", &rate);
                NEXUS_Playback_GetDefaultTrickModeSettings(&trickSettings);
                trickSettings.rate = NEXUS_NORMAL_DECODE_RATE * rate;
                if (!trickSettings.rate && rate) {
                    trickSettings.rate = rate>0?1:-1;
                }
                #if 0 /* TODO*/
                if(_trickModemaxDecoderRate) {
                    trickSettings.maxDecoderRate = opts.maxDecoderRate;
                }
                #endif
                rc = NEXUS_Playback_TrickMode(_playback, &trickSettings);
                /* don't assert for fast or reverse trick modes */
                if (trickSettings.rate >=0 && trickSettings.rate <= NEXUS_NORMAL_DECODE_RATE) {
                    BDBG_WRN((" Playback Rate Setting Failed"));
                    ret = eRet_NotSupported;
                    goto error;         
                }

                if (rc!=NEXUS_SUCCESS)
                 {
                    BDBG_WRN((" Rate TrickMode failed"));
                    ret = eRet_NotSupported;
                    goto error;
                 }

                BDBG_WRN(("Trickmode rate %d",trickSettings.rate ));
            }
            else if (strstr(buf, "host(") == buf) {
                char *s;
                char trick[64];
                int n, modifier, decoder_slowrate;
                NEXUS_PlaybackTrickModeSettings trickSettings;

                /* convert , and ) into spaces for sscanf */
                while ((s = strpbrk(buf, ",)"))) {
                    *s = ' ';
                }
                
                n = sscanf(buf+5, "%s %d %d", trick, &modifier, &decoder_slowrate);
                if (n < 2 || modifier == 0) modifier = 1;
                if (n < 3 || decoder_slowrate == 0) decoder_slowrate = 1;
                
                BDBG_WRN(("host(%s,%d,%d)", trick, modifier, decoder_slowrate));
                
                NEXUS_Playback_GetDefaultTrickModeSettings(&trickSettings);
                trickSettings.skipControl = NEXUS_PlaybackSkipControl_eHost;
                trickSettings.rateControl = NEXUS_PlaybackRateControl_eDecoder;
                trickSettings.mode_modifier = modifier;
                if (!strcasecmp(trick, "ip")) {
                    trickSettings.mode = NEXUS_PlaybackHostTrickMode_ePlayIP;
                }
                else if (!strcasecmp(trick, "i")) {
                    trickSettings.mode = NEXUS_PlaybackHostTrickMode_ePlayI;
                }
                else if (!strcasecmp(trick, "all")) {
                    trickSettings.mode = NEXUS_PlaybackHostTrickMode_eNormal;
                }
                else if (!strcasecmp(trick, "gop")) {
                    trickSettings.mode = NEXUS_PlaybackHostTrickMode_ePlayGop;
                }
                else if (!strcasecmp(trick, "bcm")) {
                    trickSettings.mode = NEXUS_PlaybackHostTrickMode_ePlayBrcm;
                }
                else {
                    BDBG_WRN(("unknown trick mode: %s", trick));
                    trickSettings.mode = NEXUS_PlaybackHostTrickMode_eNone;
                }
                trickSettings.rate = NEXUS_NORMAL_DECODE_RATE / decoder_slowrate;
                if (decoder_slowrate && !trickSettings.rate) {
                    trickSettings.rate = 1;
                }
                BDBG_ERR(("trickSettings.mode %d",(int)trickSettings.mode ));
                BDBG_ERR(("trickSettings.rate %d",trickSettings.rate ));
                rc = NEXUS_Playback_TrickMode(_playback, &trickSettings);
                if (rc!=NEXUS_SUCCESS)
                {
                    BDBG_WRN((" Playback Host Trick Mode failed"));
                    ret = eRet_NotSupported;
                    goto error;
                }
            }
            else if (strstr(buf, "seek(") == buf) {
                unsigned pos;
                unsigned min,sec,msec;
                NEXUS_PlaybackStatus status;

                if(sscanf(buf+5,"%u:%u.%u", &min, &sec, &msec)==3) {
                    pos = (min*60+sec)*1000+msec;
                } if(sscanf(buf+5,"%u:%u", &min, &sec)==2) {
                    pos = (min*60+sec)*1000;
                } else {
                    sscanf(buf+5, "%u", &pos);
                }
                
                rc = NEXUS_Playback_GetStatus(_playback, &status);
                if (rc!=NEXUS_SUCCESS) {
                    if (pos < status.first) {
                        pos = status.first;
                    }
                    else if (pos > status.last) {
                        pos = status.last;
                    }
                }
                
                rc = NEXUS_Playback_Seek(_playback, pos);
                if (rc!=NEXUS_SUCCESS)
                {
                    BDBG_WRN((" Playback Seek Failed"));
                    ret = eRet_NotSupported;
                    goto error;
                }
            }else {
                ret = eRet_NotSupported;
                goto error;
            }



done:
    /* Using the remote control now will send you to x2 by default */
   _trickModeRate = 2;
   _trickModeState= "Lua Command";
    
   ret = notifyObservers(eNotify_PlaybackStateChanged, this);
   CHECK_ERROR_GOTO("error notifying observers", ret, error);

   return ret;
error:
   _trickModeRate = 1;
   _trickModeState= "Play";
    
   NEXUS_Playback_Play(_playback);
   ret = notifyObservers(eNotify_PlaybackStateChanged, this);
   CHECK_ERROR_GOTO("error notifying observers", ret, error);
   return ret;
}

eRet CPlayback::trickMode(bool fastFoward)
{
    eRet ret = eRet_Ok;

    if (fastFoward)
    {
        /* TrickMode Rate should not be 0*/
        _trickModeRate++;

        if (_trickModeRate==0 || _trickModeRate==1) 
        {
            _trickModeRate =1;
        

            _trickModeState= "Play";
            NEXUS_Playback_Play(_playback);
            ret = notifyObservers(eNotify_PlaybackStateChanged, this);
            CHECK_ERROR_GOTO("error notifying observers", ret, error);
            return eRet_Ok;
        }
    }else{
        _trickModeRate--;
        if (_trickModeRate==1) 
        {
            _trickModeRate = -1;
        } else
            _trickModeRate--;
    }
    
    NEXUS_Playback_GetDefaultTrickmodeSettings(&_pTrickModeSettings);
    _pTrickModeSettings.rate = NEXUS_NORMAL_DECODE_RATE *(_trickModeRate);
    NEXUS_Playback_TrickMode(_playback,&_pTrickModeSettings);

    BDBG_ERR((" TrickMode %s , rate %d",(fastFoward)?"fastForward":"Rewind",_pTrickModeSettings.rate ));

    if (_trickModeRate <0)
    {
        _trickModeState= "Rewind";
        ret = notifyObservers(eNotify_PlaybackStateChanged, this);
        CHECK_ERROR_GOTO("error notifying observers", ret, error);

    } else
    {
        _trickModeState= "FastForward";
    
        ret = notifyObservers(eNotify_PlaybackStateChanged, this);
        CHECK_ERROR_GOTO("error notifying observers", ret, error);
    }
   
error:
   return ret;
       
}

void CPlayback::closePids(void)
{
    _currentVideo->_pidMgr.closePlaybackPids(this);
}

NEXUS_PlaybackSettings CPlayback::getSettings(void)
{
    NEXUS_Playback_GetSettings(_playback, &_playbackSettings);
    return _playbackSettings;
}

void CPlayback::setSettings(NEXUS_PlaybackSettings playbackSettings)
{
    _playbackSettings = playbackSettings;
    if (NEXUS_Playback_SetSettings(_playback, &_playbackSettings)!= NEXUS_SUCCESS)
    {
        BDBG_ERR(("Cannot SetSettings"));
    }
}

MString CPlayback::getVideoName(void)
{
   return _currentVideo->getVideoName();
}
