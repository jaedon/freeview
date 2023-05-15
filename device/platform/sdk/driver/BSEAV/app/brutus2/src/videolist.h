/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: videolist.h $
* $brcm_Revision: 10 $
* $brcm_Date: 10/11/12 10:11p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/videolist.h $
* 
* 10   10/11/12 10:11p jrubio
* SW7346-1004: revert MAUTOList back to MList
* 
* 9   10/11/12 5:24p tokushig
* SW7231-749: fix memory leak on videolist update
* 
* 8   10/10/12 9:54p jrubio
* SW7346-1004: add more functions into videolist
* 
* 7   10/8/12 4:18p jrubio
* SW7346-1004: add sync functions that does not delete current
* playbackList
* 
* 6   10/3/12 5:15p jrubio
* SW7346-1004: add getVideoName function
* 
* 5   9/25/12 12:46p jrubio
* SW7346-1004: remove * _pidRecord
* 
* 4   9/21/12 8:23p tokushig
* SW7231-749: add method to get pidmgr
* 
* 4   9/21/12 8:19p tokushig
* SW7231-749: add method to get pidmgr
* 
* 3   9/18/12 5:45p jrubio
* SW7346-1004: temp add _recordPidMgr pointer
* 
* 2   9/17/12 10:28a jrubio
* SW7346-1004: add record attributes to video object
* 
* 1   5/21/12 5:46p jrubio
* SW7346-644: break off video class into its own file
* 
* 10   5/15/12 4:51p jrubio
* SW7346-644: add write info,parse media files
*
*********************************************************************************/

#ifndef VIDEOLIST_H__
#define VIDEOLIST_H__

#include "nexus_types.h"
#include "nexus_playback.h"
#include "nexus_file.h"

#include "resource.h"
#include "pid.h"
#include "mvc.h"
#include "pidmgr.h"
#include "playback.h"
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
class CVideo;

class CPlaybackList : public CMvcModel
{
public:
    CPlaybackList(){};
    ~CPlaybackList() {};
    void refreshFromDisk(MString path);
    void removeVideo(CVideo *video);
    void removeVideo(int index);
    void addVideo(CVideo *video);
    eRet readInfo(MString infoName);
    void createInfo(CVideo * video);
    void sync(void);
    MList <CVideo> * getVideosList() {return &_videos;};
    MString getNextBrutusName(void);
    void generateVideoName(MString path);

    CVideo * getVideo(MString  name);
    CVideo * checkName(MString name);
    bool     hasIndex();

    void print();

protected:
    eRet createInfo(MString &mediafile, bmedia_probe_t probe,CVideo * video);
    int  saveInfo(MString &mediafile, CVideo * video,const bmedia_probe_stream *stream);

    MList <CVideo>        _videos;
    CVideo              * _lastVideo;
    int                   _index;
    int                   _nextVideoNumber;

};

class CVideo : public CPlaybackList
{
public:
    CVideo();
    ~CVideo();

    void closeVideo();
    bool hasIndex();
    eRet write();
    eRet readInfo(MString infoName);
    CPid * getPid(uint16_t index, ePidType type){return _pidMgr.getPid(index,type); };
    CPidMgr * getPidMgr(void) { return &_pidMgr; };
    MString getVideoName(void) { return _mediaName; };

    void print();

    MString   _description;
    MString   _infoName;
    MString   _mediaName;
    MString   _indexName;
    MString   _path;

    CPidMgr   _pidMgr;

    bool      _isForPurchase;
    bool      _timestamp_enabled;
    bool      _encrypted;
    bool      _active;
    bool      _isPlaybackActive;
    bool      _isRecordActive;
    bool      _timeShifting;
    int       _position;
    
protected:

};



#ifdef __cplusplus
}
#endif

#endif /* VIDEOLIST_H__ */
