/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: channelmgr.h $
* $brcm_Revision: 21 $
* $brcm_Date: 11/15/12 10:46a $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/channelmgr.h $
* 
* 21   11/15/12 10:46a tokushig
* SW7429-369: fix build for 97429/97241 and other platforms where
* NEXUS_HAS_FRONTEND is false
* 
* 20   10/19/12 1:40p tokushig
* SW7231-749: add force printout option to dumpChannelList() to override
* existing debug level.  default is NOT to override
* 
* 19   10/11/12 7:38p tokushig
* SW7231-749: make record indicator capable of showing multiple
* recordings
* 
* 18   8/27/12 9:59a tokushig
* SW7231-749: add support for updated scan and auto pid acquisition on
* tune
* 
* 17   4/13/12 11:41a jrubio
* SW7346-644: remove source.h
* 
* 16   3/19/12 4:25p tokushig
* SW7405-5581: Add support for filtering channel list during scan.
* Ignore encrypted channels, channels with invalid pcr pids, and audio
* only channels.
* 
* 15   3/13/12 4:00p tokushig
* SW7405-5581: more CModelChannel base class out of channelmgr file and
* into channel file
* 
* 14   3/12/12 4:42p tokushig
* SW7405-5581: after channel load, unnumbered channels will be auto-
* numbered starting with the last valid channel number in the channel
* list.  channel numbers can not be saved/restored in channel list
* save/load.  moved code to load channels from xml to the appropriate
* channel classes.
* 
* 13   3/9/12 3:49p jrubio
* SW7346-644: rename write XML function
* 
* 12   3/9/12 3:46p tokushig
* SW7405-5581: integrate lua channel list save command with channelmgr
* code.
* 
* 11   3/9/12 3:22p jrubio
* SW7346-644: add XML Print
* 
* 10   3/7/12 10:10a jrubio
* SW7346-644: add xml functions,move parsing to channel classes
* 
* 9   3/2/12 4:54p tokushig
* SW7405-5581: brutus.channelListLoad() lua command working.
* 
* 8   2/29/12 11:50a jrubio
* SW7346-644: add avtypes.cpp
* 
* 7   2/28/12 3:01p tokushig
* SW7405-5581: add timer support and use it for deferred tuning using ch
* up/down.  fast pressing of ch up/down remote buttons will update the
* ui but not tune until button presses stop for 500msecs.  this allows
* the user to skip channels while traversing the channel list using the
* remote
* 
* 
*********************************************************************************/

#ifndef CHANNELMGR_H__
#define CHANNELMGR_H__

#include "brutus_os.h"
#include "config.h"
#include "mvc.h"
#include "pidmgr.h"
#include "mlist.h"
#include "mxmlparser.h"
#include "mstring.h"
#include "xmltags.h"


#ifdef __cplusplus
extern "C" {
#endif

#define CHMGR_FILTER_PASS               0
#define CHMGR_FILTER_FAIL               1
#define CHMGR_FILTER_FAIL_ENCRYPTED     2
#define CHMGR_FILTER_FAIL_PCR           4
#define CHMGR_FILTER_FAIL_AUDIOONLY     8
#define CHMGR_FILTER_FAIL_DUPLICATE     16

class CModelChannel;

class CChannelMgrLoadSaveData
{
public:
    CChannelMgrLoadSaveData(const char * strFileName = "channels.xml", const char * strListName = "default", bool append = false) :
        _strFileName(strFileName),
        _strListName(strListName),
        _append(append) {};

public:
    MString _strFileName;
    MString _strListName;
    bool    _append;
};

class CModelChannelMgr : public CMvcModel
{
public:
    CModelChannelMgr(void);
    void               setCfg(CConfiguration * pCfg) { _pCfg = pCfg; };
    eRet               loadChannelList(const char * listName, const bool append = false);
    eRet               saveChannelList(const char * fileName, const bool append = false);
    eRet               clearChannelList(void);
    void               sortChannelList(void);
    eRet               addChannel(CModelChannel * pChannel);
    uint32_t           filterChannel(CModelChannel * pChannel);
    eRet               addChannelList(MList<CModelChannel> * pList, bool append = true);
    CModelChannel    * findTunedChannel(void);
    CModelChannel    * findTunedChannel(eBoardResource tunerType);
    void               dumpChannelList(bool bForce = false);
    CModelChannel    * getCurrentChannel(void);
    void               setCurrentChannel(CModelChannel * pChannel = NULL);
    CModelChannel    * getDeferredChannel(void);
    void               setDeferredChannel(CModelChannel * pChannel = NULL);
    CModelChannel    * getNextChannel(CModelChannel * pChannel = NULL, bool bWrap = true);
    CModelChannel    * getPrevChannel(CModelChannel * pChannel = NULL, bool bWrap = true);
    CModelChannel    * getFirstChannel(void);
    CModelChannel    * getLastChannel(void);
#if NEXUS_HAS_FRONTEND
    CTuner           * checkoutTuner(void);
#endif
    bool               isDuplicate(CModelChannel * pChannel);

    ~CModelChannelMgr(void);

protected:
    CConfiguration            * _pCfg;
    CModelChannel             * _currentChannel;
    CModelChannel             * _deferredChannel;
    MAutoList <CModelChannel>   _channelList;
    B_MutexHandle               _mutex;
};

#ifdef __cplusplus
}
#endif

#endif /* CHANNELMGR_H__ */
