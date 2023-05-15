/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: playback.h $
* $brcm_Revision: 22 $
* $brcm_Date: 10/19/12 12:59p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/playback.h $
* 
* 22   10/19/12 12:59p jrubio
* SW7346-1071: add playback start function cleanup playback start
* function
* 
* 21   10/19/12 10:36a tokushig
* SW7231-749: fix brutus shutdown by freeing resources in the proper
* order.
* 
* 20   10/12/12 1:00p jrubio
* SW7346-1004: added custom LUA ONLY trickmodes
* 
* 19   10/11/12 7:52p jrubio
* SW7346-1004: remove timeline after stop
* 
* 18   10/11/12 5:03p jrubio
* SW7346-1004: reset timeline when switching playbacks
* 
* 17   10/11/12 2:37p jrubio
* SW7346-1004: simplify trickmodes for remotecontrol
* 
* 16   10/10/12 10:07p jrubio
* SW7346-1004: add additional trickmodes
* 
* 15   10/3/12 5:14p jrubio
* SW7346-1004: add videoName function
* 
* 14   9/21/12 8:17p tokushig
* SW7231-749: add findPid method
* 
* 13   9/17/12 10:27a jrubio
* SW7346-1004: video object should have active record/playback flag
* 
* 12   7/9/12 4:14p jrubio
* SW7346-644: added counter for playback
* 
* 11   5/21/12 5:38p jrubio
* SW7346-644: use Video object more
* 
* 10   5/15/12 4:51p jrubio
* SW7346-644: add write info,parse media files
* 
* 9   4/20/12 2:22p jrubio
* SW7346-644: pass in path to refreshFromDisk function
* 
* 8   4/20/12 1:27p jrubio
* SW7346-644: add isActive function,and if it has an index
* 
* 7   4/19/12 11:13p jrubio
* SW7346-644: add start/stop/rewind/forward functions
* 
* 6   4/18/12 5:18p jrubio
* SW7346-644: add function to CPlaybackList
* 
* 5   4/18/12 12:40a jrubio
* SW7346-644: add refresh from disk function and new CPlaybackList class
* 
* 4   4/15/12 12:14a jrubio
* SW7346-644: add playback stop function
* 
* 3   4/13/12 11:46a jrubio
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
*********************************************************************************/

#ifndef PLAYBACK_H__
#define PLAYBACK_H__

#include "nexus_types.h"
#include "nexus_playback.h"
#include "nexus_file.h"

#include "resource.h"
#include "pid.h"
#include "videolist.h"
#include "mvc.h"
#include "pidmgr.h"
#include "mlist.h"
#include "tspsimgr2.h"
#include "mstring.h"
#include "mstringlist.h"
#include "mxmlelement.h"
#include "bmedia_probe.h"


#ifdef __cplusplus
extern "C" {
#endif

class CControl;

/* Used only by Custom Trick Mode data through LUA interface ONLY*/
class CPlaybackTrickModeData
{
public:
    CPlaybackTrickModeData(const char * command = "help") :
        _trickModeBuf(command) {};
    

public:
    MString  _trickModeBuf;
};

class CPlaybackData
{
public:
    CPlaybackData(const char * strFileName = "filename.mpg", const char * strIndexName = NULL, const char * strPath = NULL,CVideo * video=NULL) :
        _strFileName(strFileName),
        _strIndexName(strIndexName),
        _strPath(strPath), 
        _video(video) {};

public:
    MString  _strFileName;
    MString  _strIndexName;
    MString  _strPath;
    CVideo * _video;
};

class CPlayback : public CResource
{
public:

    CPlayback(const char * name, const uint16_t number, CConfiguration * pCfg);
    
    ~CPlayback(void);
    eRet initialize(void);
    eRet uninitialize(void);

    eRet start(MString filename, MString indexname, MString path);
    eRet start(void);
    eRet stop(void);
    eRet play(void);
    eRet pause(void);
    eRet trickMode(bool fastFoward); /* True for FastForward and False for Rewind */
    eRet trickMode(CPlaybackTrickModeData * pTrickModeData); /*Custom Trick Mode function used only through LUA */
    
    eRet parseInfo(const char * filename);
    void closePids(void);
    NEXUS_PlaybackHandle getPlayback(void) {return _playback; };
    NEXUS_PlaybackSettings getSettings(void);
    void setSettings(NEXUS_PlaybackSettings playbackSettings);

    void setVideo(CVideo * video) { _currentVideo = video; };
    CVideo * getVideo(void) { return _currentVideo; };
    MString getVideoName(void);
    MString getTrickModeState(void) {return _trickModeState;};

    CPid * getPid(uint16_t index, ePidType type){return _currentVideo->_pidMgr.getPid(index,type); };
    CPid * findPid(uint16_t pidNum, ePidType type) { return _currentVideo->getPidMgr()->findPid(pidNum, type); };
    
    bool hasIndex(void) {return _currentVideo->hasIndex();};
    bool isActive(void);
    void dump(void);
    void printPids(void) {_currentVideo->_pidMgr.print();};
    
protected:
    NEXUS_FilePlayHandle  _file;
    NEXUS_FilePlayHandle  _customFile;
    NEXUS_FilePlayHandle  _stickyFile;
    NEXUS_PlaypumpHandle  _playpump;
    NEXUS_PlaybackHandle  _playback;
    NEXUS_PlaybackSettings _playbackSettings;
    NEXUS_PlaybackStartSettings _playbackStartSettings;
    NEXUS_PlaybackTrickModeSettings _pTrickModeSettings;

    bool                  _active;
    CVideo              * _currentVideo;
    int                   _trickModeRate;
    MString               _trickModeState;

    /*TODO delete or Use*/
    struct playbackpos {
        long pos, firstpos, lastpos;
        unsigned long pts;
    } _current;
            
};



#ifdef __cplusplus
}
#endif

#endif /* PLAYBACK_H__ */
