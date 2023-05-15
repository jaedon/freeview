/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: pid.cpp $
* $brcm_Revision: 18 $
* $brcm_Date: 10/10/12 10:05p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/pid.cpp $
* 
* 18   10/10/12 10:05p jrubio
* SW7346-1004: adjust timeline
* 
* 17   10/10/12 5:31p tokushig
* SW7231-749: adjust debug output for release
* 
* 16   9/25/12 12:44p jrubio
* SW7346-1004: revert back to original open(record),close(record)
* 
* 15   9/18/12 5:44p jrubio
* SW7346-1004: add record logic
* 
* 14   8/29/12 10:51a jrubio
* SW7346-1004: add record pid functions
* 
* 13   8/27/12 10:02a tokushig
* SW7231-749: add == overload for pid class
* 
* 13   8/27/12 10:01a tokushig
* SW7231-749: add == overload for pid class
* 
* 12   7/27/12 11:15a jrubio
* SW7346-848: remove debug message
* 
* 11   4/20/12 1:26p jrubio
* SW7346-644: add index support
* 
* 10   4/15/12 12:12a jrubio
* SW7346-644: add logic for playback PID stop
* 
* 9   4/13/12 8:05p jrubio
* SW7346-644: add playback pasing in pid class
* 
* 8   3/19/12 4:25p tokushig
* SW7405-5581: Add support for filtering channel list during scan.
* Ignore encrypted channels, channels with invalid pcr pids, and audio
* only channels.
* 
* 7   3/12/12 9:02p tokushig
* SW7405-5581: fix problem with out of sync codec to string conversion.
* changed conversion of av types code to use hash tables instead of enum
* based array mappings to avoid out of sync lookup problems that can
* occur when NEXUS_AudioCodec and NEXUS_VideoCodec enums do not match
* lookup array mappings.  if a new nexus enum is added, existing lookup
* will still work...lookups involving the new enum value will map to
* unknown.  if a nexus enum is removed, the compiler will catch the
* inconsistency.
* 
* 6   3/9/12 3:22p jrubio
* SW7346-644: add XML Print
* 
* 5   3/7/12 10:11a jrubio
* SW7346-644: move xml parsing for PID tags
* 
* 4   2/29/12 2:12p jrubio
* SW7346-644: add parse functions to pid
* 
* 3   2/28/12 5:03p tokushig
* SW7405-5581: add header comments
* 
* 
*********************************************************************************/

#include "pid.h"
#include "band.h"
#include "mxmlparser.h"
#include "mstringhash.h"
#include "avtypes.h"
#include "playback.h"
#include "record.h"


BDBG_MODULE(brutus_pid);


eRet CPid::parseAudio(MXmlElement *xmlElemPid)
{
    eRet ret = eRet_NotAvailable;

    MString          strAudio; 
    NEXUS_AudioCodec audioType  = NEXUS_AudioCodec_eUnknown;

    uint16_t audioPidNum=0;

    strAudio = xmlElemPid->attrValue(XML_ATT_AUDIOTYPE);
    if (!strAudio || !strAudio.strncmp("",1)) 
    {
        BDBG_MSG(("No Audio Pid present"));
        return ret;
    }else 
    {
        audioType = stringToAudioCodec(strAudio);
        BDBG_MSG((" Audio Codec %s",strAudio.s()));
    }

     strAudio = xmlElemPid->attrValue(XML_ATT_AUDIOPID);
     if (!strAudio || !strAudio.strncmp("",1)) 
     {
         BDBG_WRN(("Audio PID is _not_ set!"));
         return ret;
     } else 
     {
         audioPidNum = strAudio.toInt();
     }

     BDBG_MSG(("Audio PID is set to %d",audioPidNum));

    /* Set pid info here. Need to move */
    _pid = audioPidNum;
    _pidChannel = NULL;
    _videoCodec = NEXUS_VideoCodec_eUnknown;
    _audioCodec = audioType;
    _isPcr = false;

    return eRet_Ok;

}

eRet CPid::parseVideo(MXmlElement *xmlElemPid)
{
    eRet ret = eRet_NotAvailable;

    MString          strVideo; 
    NEXUS_VideoCodec videoType  = NEXUS_VideoCodec_eUnknown;
    uint16_t videoPidNum=0;

    strVideo = xmlElemPid->attrValue(XML_ATT_VIDEOTYPE);
    if (!strVideo || !strVideo.strncmp("",1)) 
    {
        BDBG_MSG(("Video Type is _not_ set!"));
        /* TRY PCR */
        strVideo = xmlElemPid->attrValue(XML_ATT_PCRPID);
        if (!strVideo || !strVideo.strncmp("",1)) 
        {
            BDBG_MSG(("PCR Type is _not_ set!"));
            return ret;
        } else
        {   /* PCR */
            videoPidNum = strVideo.toInt();
            BDBG_MSG(("PCR PID is set to %d",videoPidNum));
            _pidType = ePidType_Pcr;
            _pidChannel = NULL;
            _pid = videoPidNum;
            return eRet_Ok;
        }

    } else 
    {
        videoType = stringToVideoCodec(strVideo);
        BDBG_MSG((" Video Codec %s",strVideo.s()));
    }

    strVideo = xmlElemPid->attrValue(XML_ATT_VIDEOPID);
    if (!strVideo || !strVideo.strncmp("",1)) 
    {
        BDBG_WRN(("Video PID is _not_ set"));
        return ret;
    } else 
    {
        videoPidNum = strVideo.toInt();
    }

    
    BDBG_MSG(("Video PID is set to %d",videoPidNum));

    /* Set pid info here. Need to move */
    _pid = videoPidNum;
    _pidChannel = NULL;
    _videoCodec = videoType;
    _audioCodec = NEXUS_AudioCodec_eUnknown;
    /*_isPcr = true;  default to PCR pid */ 
    _pidType = ePidType_Video;

    return eRet_Ok;

}

CPid::CPid(uint16_t               pid,
           ePidType               type) :
    _pid(pid),
    _caPid(0),
    _pidChannel(NULL),
    _videoCodec(NEXUS_VideoCodec_eUnknown),
    _audioCodec(NEXUS_AudioCodec_eUnknown),
    _isPcr(false),
    _pidType(type),
    _isPlayback(false)
{
    NEXUS_PidChannel_GetDefaultSettings(&_settings);
    NEXUS_Playback_GetDefaultPidChannelSettings(&_playbackSettings);
}

CPid::CPid(uint16_t               pid, 
           NEXUS_VideoCodec       videoCodec) : 
    _pid(pid),
    _caPid(0),
    _pidChannel(NULL),
    _videoCodec(videoCodec),
    _audioCodec(NEXUS_AudioCodec_eUnknown),
    _isPcr(false),
    _pidType(ePidType_Video),
    _isPlayback(false)
{
    NEXUS_PidChannel_GetDefaultSettings(&_settings);
    NEXUS_Playback_GetDefaultPidChannelSettings(&_playbackSettings);
}

CPid::CPid(uint16_t               pid, 
           NEXUS_AudioCodec       audioCodec) : 
    _pid(pid),
    _caPid(0),
    _pidChannel(NULL),
    _videoCodec(NEXUS_VideoCodec_eUnknown),
    _audioCodec(audioCodec),
    _isPcr(false),
    _pidType(ePidType_Audio),
    _isPlayback(false)
{
    NEXUS_PidChannel_GetDefaultSettings(&_settings);
    NEXUS_Playback_GetDefaultPidChannelSettings(&_playbackSettings);
}

CPid::CPid(MXmlElement *xmlElemPid)
{
    NEXUS_PidChannel_GetDefaultSettings(&_settings);
    NEXUS_Playback_GetDefaultPidChannelSettings(&_playbackSettings);
    
    /* Need to handle PCR case */
    if ( parseAudio(xmlElemPid) != eRet_Ok )
    {
        if ( parseVideo(xmlElemPid) == eRet_Ok)
        {
            BDBG_MSG(("Added Video Pid"));
           
        }
        else
        {

            BDBG_WRN(("Not a Valid Pid Entry"));
            _pidType= ePidType_Unknown;
        }
    }       
    else
    {
        BDBG_MSG(("Added Audio Pid"));
        _pidType = ePidType_Audio;
    }

    if (_pidType != ePidType_Unknown) 
    {
        if ((xmlElemPid->attrValue(XML_ATT_TYPE))== MString("playback"))
            _isPlayback = true;
    }
}


CPid::CPid(const CPid & pid) :
    _pid(pid._pid),
    _caPid(pid._caPid),
    _pidType(pid._pidType),
    _isPcr(pid._isPcr),
    _isPlayback(pid._isPlayback),
    _pidChannel(NULL),
    _settings(pid._settings),
    _playbackSettings(pid._playbackSettings),
    _videoCodec(pid._videoCodec),
    _audioCodec(pid._audioCodec)
{
    /* do not copy unique pid channel handle, etc */
}

void CPid::writeXML(MXmlElement * xmlElemChannel)
{
    MXmlElement * xmlElemPid = new MXmlElement(xmlElemChannel,XML_TAG_PID);

    if (!xmlElemPid) 
    {
        BDBG_ERR((" Cannot Create PID element "));
        return;
    }

    if (_isPlayback) 
    {
        xmlElemPid->addAttr(XML_ATT_TYPE, MString("playback"));
    }

    switch(_pidType ) 
    {
    case ePidType_Audio:
        BDBG_MSG((" Value of Audio string is 0x%x", _pid));
        xmlElemPid->addAttr(XML_ATT_AUDIOPID, MString(_pid));
        xmlElemPid->addAttr(XML_ATT_AUDIOTYPE, audioCodecToString(_audioCodec));
        break;

    case ePidType_Video:
        BDBG_MSG((" Value of Video string is 0x%x", _pid));
        xmlElemPid->addAttr(XML_ATT_VIDEOPID, MString(_pid));
        xmlElemPid->addAttr(XML_ATT_VIDEOTYPE, videoCodecToString(_videoCodec));
        break;

    case ePidType_Ancillary:
        xmlElemPid->addAttr(XML_ATT_ANCILLARYPID, NULL);
        break;

    case ePidType_Pcr:
        BDBG_MSG((" Value of PCR string is 0x%x", _pid));
        xmlElemPid->addAttr(XML_ATT_PCRPID, MString(_pid));
        break;

    default:
        BDBG_WRN((" unknown PID type", _pidType));
        delete xmlElemPid;
        return;
        break;
    }
}

CPid::~CPid()
{
}

eRet CPid::open(CParserBand * pParserBand)
{
    eRet                     ret        = eRet_Ok;
    
    BDBG_ASSERT(pParserBand);

    if (true == isOpen())
    {
        BDBG_WRN(("trying to open an already opened pid channel (pid:0x%lu)", _pid));
        goto error;
    }

    _pidChannel = NEXUS_PidChannel_Open(pParserBand->getBand(), _pid, &_settings);
    CHECK_PTR_GOTO("nexus pid channel open failed", _pidChannel, ret, eRet_NotAvailable, error);

error:
    return ret;
}

/* Playback Version */
eRet CPid::open(CPlayback * pPlayback)
{
    eRet                     ret        = eRet_Ok;
    

    if (true == isOpen())
    {
        BDBG_WRN(("trying to open an already opened pid channel (pid:0x%lu)", _pid));
        goto error;
    }

    /* Only set what Pid knows about */
    if (_pidType == ePidType_Video) 
    {
    
        _playbackSettings.pidSettings.pidType = NEXUS_PidType_eVideo;
        _playbackSettings.pidTypeSettings.video.codec = _videoCodec; /* must be told codec for correct handling */
        if (pPlayback->hasIndex()) 
            _playbackSettings.pidTypeSettings.video.index = true;
    }

    if (_pidType == ePidType_Audio) 
    {
        _playbackSettings.pidSettings.pidType = NEXUS_PidType_eAudio;
    }

    _pidChannel = NEXUS_Playback_OpenPidChannel(pPlayback->getPlayback(), _pid, &_playbackSettings);
    CHECK_PTR_GOTO("nexus pid channel open failed", _pidChannel, ret, eRet_NotAvailable, error);

error:
    return ret;
}

/* Record Version */
eRet CPid::open(CRecord * pRecord)
{
    eRet                     ret        = eRet_Ok;
    
    //BDBG_ASSERT(_isPlayback);

    CParserBand   * pParserBand = pRecord->getBand();

    if (true == isOpen())
    {
        BDBG_WRN(("trying to open an already opened pid channel (pid:0x%lu)", _pid));
        goto error;
    }

    /* Only set what Pid knows about */
    if (_pidType == ePidType_Video) 
    {
    
       /* configure the video pid for indexing */
       BKNI_Memset(&_recordSettings,0,sizeof(_recordSettings));
       NEXUS_Record_GetDefaultPidChannelSettings(&_recordSettings);
       _recordSettings.recpumpSettings.pidType = NEXUS_PidType_eVideo;
       if (pRecord->hasIndex()) 
          _recordSettings.recpumpSettings.pidTypeSettings.video.index = true;
       else
          BDBG_ERR(("NO INDEX for REcording"));
       _recordSettings.recpumpSettings.pidTypeSettings.video.codec = _videoCodec; /* must be told codec for correct handling */
       /* Need to do special handling for H264 case */
       print();
       _pidChannel = NEXUS_PidChannel_Open(pParserBand->getBand(), _pid, NULL);
       CHECK_PTR_GOTO("nexus pid channel open failed", _pidChannel, ret, eRet_NotAvailable, error);
       if (NEXUS_Record_AddPidChannel(pRecord->getRecord(),_pidChannel, &_recordSettings) != NEXUS_SUCCESS)
       {
          BDBG_ERR((" Cannot add VIDEO pidChannel to Record_AddPidChannel"));
          return eRet_NotAvailable;
       }

   } else if (_pidType == ePidType_Audio)
   {
      _pidChannel = NEXUS_PidChannel_Open(pParserBand->getBand(), _pid, NULL);
      CHECK_PTR_GOTO("nexus pid channel open failed", _pidChannel, ret, eRet_NotAvailable, error);
      if (NEXUS_Record_AddPidChannel(pRecord->getRecord(),_pidChannel, NULL) != NEXUS_SUCCESS)
      {
         BDBG_ERR((" Cannot add AUDIO pidChannel to Record_AddPidChannel"));
         return eRet_NotAvailable;
      }

    }


    
    
error:
    return ret;
}



eRet CPid::close(void)
{
    eRet ret = eRet_Ok;

    if (NULL == _pidChannel)
    {
        BDBG_WRN(("trying to close an already closed pid channel (pid:0x%lu)", _pid));
        goto error;
    }

    /* Playback will Close the pid channels
       NEXUS_Playback_ClosePidChannel*/
    if(!_isPlayback)
    {
        NEXUS_PidChannel_Close(_pidChannel);
        _pidChannel = NULL;
    }

error:
    return ret;
}

eRet CPid::close(CPlayback * pPlayback)
{
    eRet ret = eRet_Ok;

    if (NULL == _pidChannel)
    {
        BDBG_WRN(("trying to close an already closed pid channel (pid:0x%lu)", _pid));
        goto error;
    }

    /* Playback will Close the pid channels
       NEXUS_Playback_ClosePidChannel*/
    if(_isPlayback)
    {
        NEXUS_Playback_ClosePidChannel(pPlayback->getPlayback(),_pidChannel);
        _pidChannel = NULL;
    } else
    {
        BDBG_WRN((" Calling Playback PID close but PID is not a Playback PID"));
        return eRet_InvalidParameter;
    }

error:
    return ret;
}

eRet CPid::close(CRecord * pRecord)
{
    eRet ret = eRet_Ok;

    if (NULL == _pidChannel)
    {
        BDBG_WRN(("trying to close an already closed pid channel (pid:0x%lu)", _pid));
        goto error;
    }

    if (_pidType == ePidType_Video)
    {
        NEXUS_Record_RemovePidChannel(pRecord->getRecord(),_pidChannel);
    }
       
    NEXUS_PidChannel_Close(_pidChannel);
    _pidChannel = NULL;

error:
    return ret;
}

void CPid::print(void)
{
   if (_pidType == ePidType_Video) 
   {
       BDBG_ERR(("VIDEO PID number is 0x%0x, %s PID, Video Codec %s",_pid, (_isPlayback)?"Playback":"LIVE",videoCodecToString(_videoCodec).s()));
   }else if (_pidType == ePidType_Audio) 
   {
       BDBG_ERR(("AUDIO PID number is 0x%0x, %s PID, Audio Codec %s",_pid, (_isPlayback)?"Playback":"LIVE",audioCodecToString(_audioCodec).s()));
   }

    if(_isPlayback)
        BDBG_ERR(("PlaybackPIDSettings.pidSettings.pidChannelIndex = %d",_playbackSettings.pidSettings.pidSettings.pidChannelIndex));

}
    

bool CPid::operator==(CPid &other) 
{
    if (getPid() == other.getPid())
    {
        if (getPidType() == other.getPidType())
        {
            return true;
        }
    }

    return false;
}
