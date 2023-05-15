/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: pidmgr.h $
* $brcm_Revision: 12 $
* $brcm_Date: 10/25/12 3:46p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/pidmgr.h $
* 
* 12   10/25/12 3:46p tokushig
* SW7231-749: fix memory stomping issue where loading a new channel list
* could delete pid memory more than once if the pcr pid was the same as
* the video pid.
* 
* 11   10/3/12 5:14p jrubio
* SW7346-1004: remove isRecording
* 
* 10   9/25/12 12:44p jrubio
* SW7346-1004: revert back to original open(record),close(record)
* 
* 9   9/21/12 8:11p tokushig
* SW7231-749: add findPid method to pidmgr
* 
* 9   9/21/12 4:21p tokushig
* SW7231-749: add find pid method
* 
* 8   9/18/12 5:44p jrubio
* SW7346-1004: add record logic
* 
* 7   8/29/12 11:00a jrubio
* SW7346-1004: add record pids
* 
* 6   8/27/12 10:10a tokushig
* SW7231-749: fix copy constructor, == overload, and add clear pids
* method
* 
* 5   5/21/12 5:38p jrubio
* SW7346-644: handle closing pidChannels
* 
* 4   4/16/12 11:17a jrubio
* SW7346-644: remove playback pid from Video/Audio List
* 
* 3   4/15/12 12:13a jrubio
* SW7346-644: add playback pid stop function
* 
* 2   4/13/12 11:44a jrubio
* SW7346-644: add more playback pid info
* 
* 1   4/11/12 1:24p jrubio
* SW7346-644: add CPidMgr Class, replaces CModelSource Class
* 
*
* 
*********************************************************************************/

#ifndef PIDMGR_H__
#define PIDMGR_H__

#include "nexus_types.h"

#include "pid.h"
#include "mvc.h"
#include "mlist.h"
#include "tspsimgr2.h"

class CInputBand;
class CParserBand;

#ifdef __cplusplus
extern "C" {
#endif

class CPidMgr : public CMvcModel
{
public:
    CPidMgr(void);
    CPidMgr(const CPidMgr & src);  /* copy constructor */

    ~CPidMgr(void);

    eRet mapInputBand(CInputBand * pInputBand); //move
    eRet addPid(CPid * pPid);
    eRet removePid(CPid * pPid);
    CPid * getPid(uint16_t index, ePidType type);
    CPid * findPid(uint16_t pidNum, ePidType type);
    eRet createPids(PROGRAM_INFO_T * pProgramInfo);
    void clearPids(void);
    void clearPcrPid(void);
    void setTransportType(NEXUS_TransportType type) { _transportType = type; }; //move
    void setParserBand(CParserBand * pParserBand) { _pParserBand = pParserBand; }; //move
    CParserBand * getParserBand(void) { return _pParserBand; }; //move
    void readPidXML(MXmlElement * xmlElemChannel);
    void writePidXML(MXmlElement * xmlElemChannel);
    uint16_t getCaPid(void) { return _caPid; };
    void setCaPid(uint16_t caPid) { _caPid = caPid; };
    void closePlaybackPids(CPlayback * pPlayback);
    void closePlaybackPidChannels(CPlayback * pPlayback);
    void closeRecordPids(CRecord * pRecord);
    void closeRecordPidChannels(CRecord * pRecord);
    bool isEncrypted(void);
    bool isEmpty(void);

    virtual bool operator==(CPidMgr &other);
    virtual void operator=(CPidMgr &other);
    void print(void);
    
protected:
    CParserBand         * _pParserBand;
    MAutoList <CPid>      _videoPidList;
    MAutoList <CPid>      _audioPidList;
    MAutoList <CPid>      _ancillaryPidList;
    CPid                * _pPcrPid;
    NEXUS_TransportType   _transportType;
    uint16_t              _caPid;
};

#ifdef __cplusplus
}
#endif

#endif /*PIDMGR_H__ */
