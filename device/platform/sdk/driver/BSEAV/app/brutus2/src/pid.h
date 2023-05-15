/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: pid.h $
* $brcm_Revision: 16 $
* $brcm_Date: 9/25/12 12:44p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/pid.h $
* 
* 16   9/25/12 12:44p jrubio
* SW7346-1004: revert back to original open(record),close(record)
* 
* 15   9/18/12 5:44p jrubio
* SW7346-1004: add record logic
* 
* 14   8/29/12 10:52a jrubio
* SW7346-1004: add record event
* 
* 13   8/27/12 10:01a tokushig
* SW7231-749: add == overload for pid class
* 
* 12   5/15/12 4:51p jrubio
* SW7346-644: add additional writeXML info
* 
* 11   4/15/12 12:13a jrubio
* SW7346-644: add playback stop function
* 
* 10   4/13/12 11:43a jrubio
* SW7346-644: modify the open pid calls
* 
* 9   4/12/12 12:11a jrubio
* SW7346-644: add Playback Pid Channel Settings
* 
* 8   3/19/12 4:25p tokushig
* SW7405-5581: Add support for filtering channel list during scan.
* Ignore encrypted channels, channels with invalid pcr pids, and audio
* only channels.
* 
* 7   3/9/12 3:22p jrubio
* SW7346-644: add XML Print
* 
* 6   3/7/12 10:11a jrubio
* SW7346-644: move xml parsing for PID tags
* 
* 5   2/29/12 2:12p jrubio
* SW7346-644: add parse functions to pid
* 
* 4   2/28/12 5:03p tokushig
* SW7405-5581: add header comments
* 
* 
*********************************************************************************/

#ifndef PID_H__
#define PID_H__

#include "brutus.h"
#include "nexus_video_types.h"
#include "nexus_audio_types.h"
#include "nexus_pid_channel.h"
#include "nexus_playback.h"
#include "nexus_record.h"
#include "mxmlelement.h"
#include "xmltags.h"

#ifdef __cplusplus
extern "C" {
#endif

class CParserBand;
class CPlayback;
class CRecord;

typedef enum 
{
    ePidType_Unknown,
    ePidType_Video,
    ePidType_Audio,
    ePidType_Ancillary,
    ePidType_Pcr,
    ePidType_Max
} ePidType;

/* 0 is a valid pid, but only with respect to PSI table acquisition */
#define VALID_PID(pid) ((pid)!=0 && (pid)<0x1FFF)

class CPid
{
public:
    CPid(uint16_t               pid = 0xFFFF,
         ePidType                type = ePidType_Unknown);
    CPid(uint16_t               pid, 
         NEXUS_VideoCodec       videoCodec);
    CPid(uint16_t               pid, 
         NEXUS_AudioCodec       audioCodec);

    CPid(MXmlElement *xmlElemPid);

    CPid(const CPid & pid); /* copy constructor */
    ~CPid(void);

    eRet open(CParserBand * pParserBand);
    eRet open(CPlayback * pPlayback);
    eRet open(CRecord * pRecord);
    eRet close(void);
    eRet close(CPlayback * pPlayback);
    eRet close(CRecord * pRecord);

    /* todo: these should be readXML() */
    eRet parseAudio(MXmlElement * xmlElemPid);
    eRet parseVideo(MXmlElement * xmlElemPid);

    uint16_t getPid(void) { return _pid; };
    NEXUS_PidChannelHandle getPidChannel(void) { return _pidChannel; };
    void setPidChannel(NEXUS_PidChannelHandle pidChannel) { _pidChannel = pidChannel; };
    void setPidType(ePidType pidType) { _pidType = pidType; };
    void setPcrType(bool isPcr) { _isPcr = isPcr; };
    void setPlayback(bool isPlayback) { _isPlayback = isPlayback; };
    ePidType getPidType(void) { return _pidType; };
    void getSettings(NEXUS_PidChannelSettings * pSettings) { *pSettings = _settings; };
    void setSettings(NEXUS_PidChannelSettings * pSettings) { _settings = *pSettings; };
    void getSettings(NEXUS_PlaybackPidChannelSettings * pSettings) { *pSettings = _playbackSettings; };
    void setSettings(NEXUS_PlaybackPidChannelSettings * pSettings) { _playbackSettings = *pSettings; };
    uint16_t getCaPid(void) { return _caPid; };
    void setCaPid(uint16_t caPid) { _caPid = caPid; };

    NEXUS_VideoCodec getVideoCodec() { return _videoCodec; };
    NEXUS_AudioCodec getAudioCodec() { return _audioCodec; };
    void setVideoCodec(NEXUS_VideoCodec videoCodec) { _videoCodec= videoCodec; };
    void setAudioCodec(NEXUS_AudioCodec audioCodec) { _audioCodec= audioCodec; };

    bool isPcrPid(void)   { return ((true == _isPcr) || (ePidType_Pcr == _pidType)); };
    bool isUniquePcrPid(void)   { return (ePidType_Pcr == _pidType); };
    bool isVideoPid(void) { return (ePidType_Video == _pidType); };
    bool isAudioPid(void) { return (ePidType_Audio == _pidType); };
    bool isAncillaryPid(void) { return (ePidType_Ancillary == _pidType); };
    bool isEncrypted(void) { return VALID_PID(_caPid); };
    bool isPlayback(void) { return _isPlayback; };

    bool isOpen() { return (NULL != _pidChannel); };

    void writeXML(MXmlElement * xmlElemChannel);

    virtual bool operator==(CPid &other);
    void print(void);
    
protected:
    uint16_t                 _pid;
    uint16_t                 _caPid;
    ePidType                 _pidType;
    bool                     _isPcr;
    bool                     _isPlayback;
    NEXUS_PidChannelHandle   _pidChannel;
    NEXUS_PidChannelSettings _settings;
    NEXUS_PlaybackPidChannelSettings _playbackSettings;
    NEXUS_RecordPidChannelSettings _recordSettings;

    NEXUS_VideoCodec _videoCodec;
    NEXUS_AudioCodec _audioCodec;
};


class CPidVideo : public CPid
{
public:
    CPidVideo(uint16_t pid, NEXUS_VideoCodec videoCodec) :
        CPid(pid, ePidType_Video) {};

    NEXUS_VideoCodec getCodec() { return _codec; };

    eRet readXML(MXmlElement * xmlElemPid);
    void writeXML(MXmlElement * xmlElemChannel);

protected:
    NEXUS_VideoCodec _codec;
};

class CPidAudio : public CPid
{
public:
    CPidAudio(uint16_t pid, NEXUS_AudioCodec audioCodec) :
        CPid(pid, ePidType_Audio) {};
        
    NEXUS_AudioCodec getCodec() { return _codec; };

    eRet readXML(MXmlElement * xmlElemPid);
    void writeXML(MXmlElement * xmlElemChannel);

protected:
    NEXUS_AudioCodec _codec;
};

class CPidPcr : public CPid
{
public:
    CPidPcr(uint16_t pid) : 
        CPid(pid, ePidType_Pcr) {};

    eRet readXML(MXmlElement * xmlElemPid);
    void writeXML(MXmlElement * xmlElemChannel);
};

class CPidAncillary : public CPid
{
public:
    CPidAncillary(uint16_t pid) : 
        CPid(pid, ePidType_Ancillary) {};

    eRet readXML(MXmlElement * xmlElemPid);
    void writeXML(MXmlElement * xmlElemChannel);
};

#ifdef __cplusplus
}
#endif

#endif /* PID_H__ */
