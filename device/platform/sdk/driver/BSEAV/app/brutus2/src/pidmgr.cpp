/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: pidmgr.cpp $
* $brcm_Revision: 17 $
* $brcm_Date: 10/25/12 3:46p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/pidmgr.cpp $
* 
* 17   10/25/12 3:46p tokushig
* SW7231-749: fix memory stomping issue where loading a new channel list
* could delete pid memory more than once if the pcr pid was the same as
* the video pid.
* 
* 16   10/8/12 6:47p tokushig
* SW7231-749: add lua commands for display settings
* 
* 15   10/4/12 8:29a jrubio
* SW7346-1004: remove isRecording
* 
* 14   9/25/12 12:44p jrubio
* SW7346-1004: revert back to original open(record),close(record)
* 
* 13   9/21/12 8:11p tokushig
* SW7231-749: add findPid method to pidmgr
* 
* 13   9/21/12 4:21p tokushig
* SW7231-749: add find pid method
* 
* 12   9/18/12 5:44p jrubio
* SW7346-1004: add record logic
* 
* 11   8/29/12 10:57a jrubio
* SW7346-1004: add record event
* 
* 10   8/27/12 3:13p tokushig
* SW7231-749: fix channel list read issue when no pids are specified.
* 
* 9   8/27/12 10:11a tokushig
* SW7231-749: fix copy constructor, == overload, and add clear pids
* method
* 
* 9   8/27/12 10:10a tokushig
* SW7231-749: fix copy constructor, == overload, and add clear pids
* method
* 
* 8   7/27/12 1:59p jrubio
* SW7346-848: Do not save duplicate Pids in the channel map
* 
* 7   7/27/12 11:15a jrubio
* SW7346-848: add pcr code
* 
* 6   5/21/12 5:38p jrubio
* SW7346-644: handle closing pidChannels
* 
* 5   4/16/12 11:17a jrubio
* SW7346-644: remove playback pid from Video/Audio List
* 
* 4   4/15/12 12:14a jrubio
* SW7346-644: add playback stop function
* 
* 3   4/13/12 11:44a jrubio
* SW7346-644: add more playback pid info
* 
* 2   4/12/12 12:09a jrubio
* SW7346-644: add Playback information
* 
* 1   4/11/12 1:24p jrubio
* SW7346-644: add CPidMgr Class, replaces CModelSource Class
* 
* 7   3/19/12 4:25p tokushig
* SW7405-5581: Add support for filtering channel list during scan.
* Ignore encrypted channels, channels with invalid pcr pids, and audio
* only channels.
* 
* 6   3/12/12 4:42p tokushig
* SW7405-5581: after channel load, unnumbered channels will be auto-
* numbered starting with the last valid channel number in the channel
* list.  channel numbers can not be saved/restored in channel list
* save/load.  moved code to load channels from xml to the appropriate
* channel classes.
*
*********************************************************************************/

#include "pidmgr.h"
#include "band.h"

#include "nexus_parser_band.h"

BDBG_MODULE(pidmgr);


CPidMgr::CPidMgr() :
    _pParserBand(NULL),
    _pPcrPid(NULL),
    _transportType(NEXUS_TransportType_eUnknown)
{
    _videoPidList.clear();
    _audioPidList.clear();
    _ancillaryPidList.clear();
}

CPidMgr::CPidMgr(const CPidMgr & src) :
    CMvcModel(),
    _caPid(src._caPid),
    _pParserBand(src._pParserBand),
    _pPcrPid(NULL),
    _transportType(src._transportType)
{
    /* copy pids */
    MListItr <CPid>   itrVideo(&(src._videoPidList));
    MListItr <CPid>   itrAudio(&(src._audioPidList));
    MListItr <CPid>   itrAncillary(&(src._ancillaryPidList));
    CPid            * pPid                                  = NULL;

    for (pPid = itrVideo.first(); pPid; pPid = itrVideo.next())
    {
        eRet   ret     = eRet_Ok;
        CPid * pNewPid = NULL;

        pNewPid = new CPid(*pPid);
        BDBG_ASSERT(NULL != pNewPid);

        ret = addPid(pNewPid);
        BDBG_ASSERT(eRet_Ok == ret);

        if (pPid == src._pPcrPid)
        {
            /* pcr pid is video pid */
            _pPcrPid = pNewPid;
        }
    }

    for (pPid = itrAudio.first(); pPid; pPid = itrAudio.next())
    {
        eRet   ret     = eRet_Ok;
        CPid * pNewPid = NULL;

        pNewPid = new CPid(*pPid);
        BDBG_ASSERT(NULL != pNewPid);

        ret = addPid(pNewPid);
        BDBG_ASSERT(eRet_Ok == ret);

        if (pPid == src._pPcrPid)
        {
            /* pcr pid is audio pid */
            _pPcrPid = pNewPid;
        }
    }

    for (pPid = itrAncillary.first(); pPid; pPid = itrAncillary.next())
    {
        eRet   ret     = eRet_Ok;
        CPid * pNewPid = NULL;

        pNewPid = new CPid(*pPid);
        BDBG_ASSERT(NULL != pNewPid);

        ret = addPid(pNewPid);
        BDBG_ASSERT(eRet_Ok == ret);

        if (pPid == src._pPcrPid)
        {
            /* pcr pid is ancillary pid */
            _pPcrPid = pNewPid;
        }
    }

    if (NULL != src._pPcrPid)
    {
        if (src._pPcrPid->isUniquePcrPid())
        {
            /* if the pid is a unique pcr pid (i.e. not the same as a video/audio/ancillary pid)
               then we must explicitly create it. */
            _pPcrPid = new CPid(*(src._pPcrPid));
            BDBG_ASSERT(NULL != _pPcrPid);
        }
    }

}
    

CPidMgr::~CPidMgr()
{
    if (NULL != _pPcrPid)
    {
        if (_pPcrPid->isUniquePcrPid())
        {
            /* if the pid is a unique pcr pid (i.e. not the same as a video/audio/ancillary pid)
               then we must explicitly delete it.  otherwise it is stored in one of the other
               pid type's pid lists. */
            DEL(_pPcrPid);
        }
    }

    _pPcrPid = NULL;
}

eRet CPidMgr::addPid(CPid * pPid)
{
    eRet     ret  = eRet_NotAvailable; /* assume error */

    BDBG_ASSERT(pPid);

    if (pPid->isVideoPid())
    {
        if (BRUTUS_MAX_PROGRAMS >= _videoPidList.total())
        {
            _videoPidList.add(pPid);
            ret = eRet_Ok;
        }
    }
    else
    if (pPid->isAudioPid())
    {
        if (BRUTUS_MAX_PROGRAMS >= _audioPidList.total())
        {
            _audioPidList.add(pPid);
            ret = eRet_Ok;
        }
    }
    else
    if (pPid->isAncillaryPid())
    {
        if (BRUTUS_MAX_PROGRAMS >= _ancillaryPidList.total())
        {
            _ancillaryPidList.add(pPid);
            ret = eRet_Ok;
        }
    }
    
    if (pPid->isPcrPid())
    {
        _pPcrPid = pPid;
        ret = eRet_Ok;
    }

    return ret;
}

eRet CPidMgr::removePid(CPid * pPid)
{
    eRet     ret  = eRet_NotAvailable; /* assume error */

    BDBG_ASSERT(pPid);

    if (pPid->isVideoPid())
    {
        if (BRUTUS_MAX_PROGRAMS >= _videoPidList.total())
        {
            _videoPidList.remove(pPid);
            ret = eRet_Ok;
        }
    }
    else
    if (pPid->isAudioPid())
    {
        if (BRUTUS_MAX_PROGRAMS >= _audioPidList.total())
        {
            _audioPidList.remove(pPid);
            ret = eRet_Ok;
        }
    }
    else
    if (pPid->isAncillaryPid())
    {
        if (BRUTUS_MAX_PROGRAMS >= _ancillaryPidList.total())
        {
            _ancillaryPidList.remove(pPid);
            ret = eRet_Ok;
        }
    }
    
    if (pPid->isPcrPid())
    {
        _pPcrPid = pPid;
        ret = eRet_Ok;
    }

    return ret;
}

CPid * CPidMgr::findPid(uint16_t pidNum, ePidType type)
{
    CPid * pPidFound = NULL;

    switch (type)
    {
    case ePidType_Video:
        {
            CPid            * pPid                 = NULL;
            MListItr <CPid>   itr(&_videoPidList);

            for (pPid = itr.first(); NULL != pPid; pPid = itr.next())
            {
                if (pidNum == pPid->getPid())
                {
                    /* found match */
                    pPidFound = pPid;
                    break;
                }
            }
        }
        break;

    case ePidType_Audio:
        {
            CPid            * pPid                 = NULL;
            MListItr <CPid>   itr(&_audioPidList);

            for (pPid = itr.first(); NULL != pPid; pPid = itr.next())
            {
                if (pidNum == pPid->getPid())
                {
                    /* found match */
                    pPidFound = pPid;
                    break;
                }
            }
        }
        break;

    case ePidType_Ancillary:
        {
            CPid            * pPid                 = NULL;
            MListItr <CPid>   itr(&_ancillaryPidList);

            for (pPid = itr.first(); NULL != pPid; pPid = itr.next())
            {
                if (pidNum == pPid->getPid())
                {
                    /* found match */
                    pPidFound = pPid;
                    break;
                }
            }
        }
        break;

    case ePidType_Pcr:
        if (pidNum == _pPcrPid->getPid())
        {
            pPidFound = _pPcrPid;
        }
        break;

    default:
        break;
    }

    return pPidFound;
}

CPid * CPidMgr::getPid(uint16_t index, ePidType type)
{
    CPid * pPid = NULL;

    switch (type)
    {
    case ePidType_Video:
        if (index < _videoPidList.total())
        {
            pPid = _videoPidList.get(index);
        }
        break;

    case ePidType_Audio:
        if (index < _audioPidList.total())
        {
            pPid = _audioPidList.get(index);
        }
        break;

    case ePidType_Ancillary:
        if (index < _ancillaryPidList.total())
        {
            pPid = _ancillaryPidList.get(index);
        }
        break;

    case ePidType_Pcr:
        pPid = _pPcrPid; 
        break;

    default:
        break;
    }

    return pPid;
}

/* Live PIDS */
eRet CPidMgr::createPids(PROGRAM_INFO_T * pProgramInfo)
{
    eRet ret        = eRet_Ok;
    int  j          = 0;
    bool bUniquePcr = true;

    /* save video pids */
    for (j = 0; j < pProgramInfo->num_video_pids; j++)
    {
        CPid * pPid = NULL;
        pPid = new CPid(pProgramInfo->video_pids[j].pid,
                        (NEXUS_VideoCodec)pProgramInfo->video_pids[j].streamType);
        CHECK_PTR_GOTO("unable to create pid", pPid, ret, eRet_OutOfMemory, error);

        if (pProgramInfo->pcr_pid == pPid->getPid())
        {
            pPid->setPcrType(true);
            bUniquePcr = false;
        }

        pPid->setCaPid(pProgramInfo->video_pids[j].ca_pid);
        addPid(pPid);
    }

    /* save audio pids */
    for (j = 0; j < pProgramInfo->num_audio_pids; j++)
    {
        CPid * pPid = NULL;
        pPid = new CPid(pProgramInfo->audio_pids[j].pid,
                        (NEXUS_AudioCodec)pProgramInfo->audio_pids[j].streamType);
        CHECK_PTR_GOTO("unable to create pid", pPid, ret, eRet_OutOfMemory, error);

        pPid->setCaPid(pProgramInfo->audio_pids[j].ca_pid);
        addPid(pPid);
    }

    /* save ancillary pids */
    for (j = 0; j < pProgramInfo->num_other_pids; j++)
    {
        CPid * pPid = NULL;
        pPid = new CPid(pProgramInfo->other_pids[j].pid,
                        ePidType_Ancillary);
        CHECK_PTR_GOTO("unable to create pid", pPid, ret, eRet_OutOfMemory, error);

        pPid->setCaPid(pProgramInfo->other_pids[j].ca_pid);
        addPid(pPid);
    }

    /* save pcr pid if not duplicate */
    if (true == bUniquePcr)
    {
        CPid * pPid = NULL;
        pPid = new CPid(pProgramInfo->pcr_pid,
                        ePidType_Pcr);
        CHECK_PTR_GOTO("unable to create pid", pPid, ret, eRet_OutOfMemory, error);

        pPid->setPcrType(true);
        addPid(pPid);
    }

    /* save program ca pid if valid */
    setCaPid(pProgramInfo->ca_pid);

error:
    return ret;
}

void CPidMgr::clearPids()
{
    _videoPidList.clear();
    _audioPidList.clear();
    _ancillaryPidList.clear();

    _caPid = 0;

    clearPcrPid();
}

void CPidMgr::clearPcrPid()
{
    if (NULL != _pPcrPid)
    {
        if (_pPcrPid->isUniquePcrPid())
        {
            /* if the pid is a unique pcr pid (i.e. not the same as a video/audio/ancillary pid)
               then we must explicitly delete it.  otherwise it is stored in one of the other
               pid type's pid lists. */
            DEL(_pPcrPid);
        }
    }

    _pPcrPid = NULL;
}

void CPidMgr::readPidXML(MXmlElement * xmlElemChannel)
{
    MXmlElement * xmlElemPid = NULL;
    CPid        * pPid       = NULL;
    MListItr <CPid> itrVideo(&_videoPidList);

    for (xmlElemPid = xmlElemChannel->firstChild(); xmlElemPid; xmlElemPid = xmlElemChannel->nextChild())
    {
        if (xmlElemPid->tag() != XML_TAG_PID)  
        {
            BDBG_WRN((" Tag is not PID its %s",xmlElemChannel->tag().s()));
            continue;
        }

        /* Function will parse the XML element  */
        pPid = new CPid(xmlElemPid);
        if (pPid->getPidType() == ePidType_Unknown) 
        {
            BDBG_WRN((" Unknown PID entry "));
            DEL(pPid);
            continue;
        }

        addPid(pPid);
        BDBG_MSG((" Added Pid %s ",xmlElemChannel->tag().s()));
    }
    
    for (pPid = itrVideo.first() ; pPid; pPid = itrVideo.next())
    {
        if (_pPcrPid != NULL && (pPid->getPid() == _pPcrPid->getPid()))
        {
            BDBG_MSG((" Pids are the same delete PCR pid"));
            clearPcrPid();
            break;
        } 
            
    }

    if(_pPcrPid != NULL)
    {
        BDBG_MSG((" PCR PID is Valid return "));
        return;
    }

    BDBG_MSG((" NO PCR pid set it to first video pid"));
    /* Cannot find a pcr pid */
    pPid = itrVideo.first();
    if (NULL != pPid)
    {
        pPid->setPcrType(true);
        _pPcrPid = pPid;
    }

}

void CPidMgr::writePidXML(MXmlElement * xmlElemChannel)
{
    CPid * pPid = NULL;
    bool   bUniquePcr = true;
    MListItr <CPid> itrVideo(&_videoPidList);
    MListItr <CPid> itrAudio(&_audioPidList);
    MListItr <CPid> itrAncillary(&_ancillaryPidList);

    /* Iterate through all Pid list*/
    for (pPid = itrVideo.first() ; pPid; pPid = itrVideo.next())
    {
        pPid->writeXML(xmlElemChannel);
        if (_pPcrPid != NULL) 
        {
            if( _pPcrPid == pPid)
                bUniquePcr = false;

        }
    }

    for (pPid = itrAudio.first(); pPid; pPid = itrAudio.next() )
    {
        pPid->writeXML(xmlElemChannel);
    }

    for (pPid = itrAncillary.first(); pPid; pPid = itrAncillary.next() )
    {
        pPid->writeXML(xmlElemChannel);
    }

    if (_pPcrPid != NULL && bUniquePcr == true)
    {
        _pPcrPid->writeXML(xmlElemChannel);
    }

    BDBG_MSG(("Done write Pids"));

}

bool CPidMgr::isEncrypted(void)
{
    bool bEncrypted = false;

    if (true == VALID_PID(getCaPid()))
    {
        return true;
    }

    /* also have to check ca pid values for each video pid object */
    MListItr <CPid>   itrVideo(&_videoPidList);
    CPid            * pPid                      = NULL;

    for (pPid = itrVideo.first() ; pPid; pPid = itrVideo.next())
    {
        if (pPid->isEncrypted())
        {
            return true;
        }
    }

    /* also have to check ca pid values for each audio pid object */
    MListItr <CPid> itrAudio(&_audioPidList);
    for (pPid = itrAudio.first(); pPid; pPid = itrAudio.next() )
    {
        if (pPid->isEncrypted())
        {
            return true;
        }
    }

    return false;
}

eRet CPidMgr::mapInputBand(CInputBand * pInputBand)
{
    eRet                       ret        = eRet_Ok;
    NEXUS_Error                nerror     = NEXUS_SUCCESS;
    NEXUS_ParserBandSettings   settings;
    NEXUS_ParserBand           band;

    BDBG_ASSERT(NULL != _pParserBand);
    BDBG_ASSERT(NULL != pInputBand);

    band = _pParserBand->getBand();

    NEXUS_ParserBand_GetSettings(band, &settings);
    settings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
    settings.sourceTypeSettings.inputBand = pInputBand->getBand();
    BDBG_ASSERT((NEXUS_TransportType_eTs     == _transportType) ||
                (NEXUS_TransportType_eDssEs  == _transportType) ||
                (NEXUS_TransportType_eDssPes == _transportType));
    settings.transportType = _transportType;
    nerror = NEXUS_ParserBand_SetSettings(band, &settings);
    CHECK_NEXUS_ERROR_GOTO("error setting parser band settings", ret, nerror, error);

error:
    return ret;
}

bool CPidMgr::operator==(CPidMgr &other) 
{
    CPid * pVideoPid      = getPid(0, ePidType_Video);
    CPid * pOtherVideoPid = other.getPid(0, ePidType_Video);

    CPid * pAudioPid      = getPid(0, ePidType_Audio);
    CPid * pOtherAudioPid = other.getPid(0, ePidType_Audio);

    if ((NULL != _pPcrPid) && (NULL != other._pPcrPid))
    {
        if (!(*_pPcrPid == *(other._pPcrPid)))
        {
            return false;
        }
    }
    else
    if ((NULL == _pPcrPid) ^ (NULL == other._pPcrPid))
    {
        /* XOR */
        return false;
    }

    /* note: we will only check the first video and audio pid - technically we should
       check all the pids but this is sufficient in all but the rarest of cases. */
    if ((NULL != pVideoPid) && (NULL != pOtherVideoPid))
    {
        if (!(*pVideoPid == *pOtherVideoPid))
        {
            return false;
        }
    }
    else
    if ((NULL == pVideoPid) ^ (NULL == pOtherVideoPid))
    {
        /* XOR */
        return false;
    }

    if ((NULL != pAudioPid) && (NULL != pOtherAudioPid))
    {
        if (!(*pAudioPid == *pOtherAudioPid))
        {
            return false;
        }
    }
    else
    if ((NULL == pAudioPid) ^ (NULL == pOtherAudioPid))
    {
        /* XOR */
        return false;
    }
    
    if (other.getCaPid() != getCaPid())
    {
        return false;
    }

    return true;
}

/* copy constructor */
void CPidMgr::operator=(CPidMgr &other) 
{
   MListItr <CPid>   itrVideo(&(other._videoPidList));
   MListItr <CPid>   itrAudio(&(other._audioPidList));
   MListItr <CPid>   itrAncillary(&(other._ancillaryPidList));
   CPid            * pPid                                  = NULL;

    for (pPid = itrVideo.first(); pPid; pPid = itrVideo.next())
    {
        eRet   ret     = eRet_Ok;
        CPid * pNewPid = NULL;

        pNewPid = new CPid(*pPid);
        BDBG_ASSERT(NULL != pNewPid);

        ret = addPid(pNewPid);
        BDBG_ASSERT(eRet_Ok == ret);

        if (pPid == other._pPcrPid)
        {
            /* pcr pid is video pid */
            _pPcrPid = pNewPid;
        }
    }

    for (pPid = itrAudio.first(); pPid; pPid = itrAudio.next())
    {
        eRet   ret     = eRet_Ok;
        CPid * pNewPid = NULL;

        pNewPid = new CPid(*pPid);
        BDBG_ASSERT(NULL != pNewPid);

        ret = addPid(pNewPid);
        BDBG_ASSERT(eRet_Ok == ret);

        if (pPid == other._pPcrPid)
        {
            /* pcr pid is audio pid */
            _pPcrPid = pNewPid;
        }
    }

    for (pPid = itrAncillary.first(); pPid; pPid = itrAncillary.next())
    {
        eRet   ret     = eRet_Ok;
        CPid * pNewPid = NULL;

        pNewPid = new CPid(*pPid);
        BDBG_ASSERT(NULL != pNewPid);

        ret = addPid(pNewPid);
        BDBG_ASSERT(eRet_Ok == ret);

        if (pPid == other._pPcrPid)
        {
            /* pcr pid is ancillary pid */
            _pPcrPid = pNewPid;
        }
    }

    if (NULL != other._pPcrPid)
    {
        if (other._pPcrPid->isUniquePcrPid())
        {
            /* if the pid is a unique pcr pid (i.e. not the same as a video/audio/ancillary pid)
               then we must explicitly create it. */
            _pPcrPid = new CPid(*(other._pPcrPid));
            BDBG_ASSERT(NULL != _pPcrPid);
        }
    }

}

/* This Function does delete the playback pids */
void CPidMgr::closePlaybackPids(CPlayback * pPlayback)
{
    MListItr <CPid>   itrVideo(&_videoPidList);
    CPid            * pPid                      = NULL;

    for (pPid = itrVideo.first() ; pPid; pPid = itrVideo.next())
    {
        pPid->close(pPlayback);
        BDBG_MSG(("Closing VIDEO PID"));
        /* pPid->print();*/
    }

    _videoPidList.clear();

    /* also have to check ca pid values for each audio pid object */
    MListItr <CPid> itrAudio(&_audioPidList);
    for (pPid = itrAudio.first(); pPid; pPid = itrAudio.next() )
    {
        pPid->close(pPlayback);
        BDBG_MSG(("Closing AUDIO PID"));
        /* pPid->print(); */
    }
    _audioPidList.clear();

    /* also have to check ca pid values for each audio pid object */
    MListItr <CPid> itrAncillary(&_ancillaryPidList);
    for (pPid = itrAncillary.first(); pPid; pPid = itrAncillary.next() )
    {
        pPid->close(pPlayback);
        BDBG_MSG(("Closing AUDIO PID"));
        /* pPid->print(); */
    }

    _ancillaryPidList.clear();

    _caPid = false;
    _pPcrPid = NULL;

}
/* This Function closes Playback pid Channel */
void CPidMgr::closePlaybackPidChannels(CPlayback * pPlayback)
{
    MListItr <CPid>   itrVideo(&_videoPidList);
    CPid            * pPid                      = NULL;

    for (pPid = itrVideo.first() ; pPid; pPid = itrVideo.next())
    {
        pPid->close(pPlayback);
        BDBG_MSG(("Closing VIDEO PID CHANNEL "));
        /* pPid->print();*/
    }

    /* also have to check ca pid values for each audio pid object */
    MListItr <CPid> itrAudio(&_audioPidList);
    for (pPid = itrAudio.first(); pPid; pPid = itrAudio.next() )
    {
        pPid->close(pPlayback);
        BDBG_MSG(("Closing AUDIO PID CHANNEL"));
        /* pPid->print(); */
    }

    /* also have to check ca pid values for each audio pid object */
    MListItr <CPid> itrAncillary(&_ancillaryPidList);
    for (pPid = itrAncillary.first(); pPid; pPid = itrAncillary.next() )
    {
        pPid->close(pPlayback);
        BDBG_MSG(("Closing AUDIO PID CHANNEL"));
        /* pPid->print(); */
    }

}

/* This Function does delete the record pids */
void CPidMgr::closeRecordPids(CRecord * pRecord)
{
    MListItr <CPid>   itrVideo(&_videoPidList);
    CPid            * pPid                      = NULL;

    for (pPid = itrVideo.first() ; pPid; pPid = itrVideo.next())
    {
        pPid->close(pRecord);
        BDBG_MSG(("Closing VIDEO PID"));
        /* pPid->print();*/
    }

    _videoPidList.clear();

    /* also have to check ca pid values for each audio pid object */
    MListItr <CPid> itrAudio(&_audioPidList);
    for (pPid = itrAudio.first(); pPid; pPid = itrAudio.next() )
    {
        pPid->close(pRecord);
        BDBG_MSG(("Closing AUDIO PID"));
        /* pPid->print(); */
    }
    _audioPidList.clear();

    /* also have to check ca pid values for each audio pid object */
    MListItr <CPid> itrAncillary(&_ancillaryPidList);
    for (pPid = itrAncillary.first(); pPid; pPid = itrAncillary.next() )
    {
        pPid->close(pRecord);
        BDBG_MSG(("Closing AUDIO PID"));
        /* pPid->print(); */
    }

    _ancillaryPidList.clear();

    _caPid = false;
    _pPcrPid = NULL;

}


/* This Function closes Playback pid Channel but does not delete it */
void CPidMgr::closeRecordPidChannels(CRecord * pRecord)
{
    MListItr <CPid>   itrVideo(&_videoPidList);
    CPid            * pPid                      = NULL;

    for (pPid = itrVideo.first() ; pPid; pPid = itrVideo.next())
    {
        pPid->close(pRecord);
        BDBG_MSG(("Closing VIDEO PID CHANNEL "));
        /* pPid->print();*/
    }

    /* also have to check ca pid values for each audio pid object */
    MListItr <CPid> itrAudio(&_audioPidList);
    for (pPid = itrAudio.first(); pPid; pPid = itrAudio.next() )
    {
        pPid->close(pRecord);
        BDBG_MSG(("Closing AUDIO PID CHANNEL"));
        /* pPid->print(); */
    }

    /* also have to check ca pid values for each audio pid object */
    MListItr <CPid> itrAncillary(&_ancillaryPidList);
    for (pPid = itrAncillary.first(); pPid; pPid = itrAncillary.next() )
    {
        pPid->close(pRecord);
        BDBG_MSG(("Closing AUDIO PID CHANNEL"));
        /* pPid->print(); */
    }

}

void CPidMgr::print(void)
{
    MListItr <CPid>   itrVideo(&_videoPidList);
    CPid            * pPid                      = NULL;

    for (pPid = itrVideo.first() ; pPid; pPid = itrVideo.next())
    {
        pPid->print();
    }

    /* also have to check ca pid values for each audio pid object */
    MListItr <CPid> itrAudio(&_audioPidList);
    for (pPid = itrAudio.first(); pPid; pPid = itrAudio.next() )
    {
        pPid->print();
    }

    /* also have to check ca pid values for each audio pid object */
    MListItr <CPid> itrAncillary(&_ancillaryPidList);
    for (pPid = itrAncillary.first(); pPid; pPid = itrAncillary.next() )
    {
        pPid->print();
    }
}

bool CPidMgr::isEmpty(void)
{
    CPid * pPid;
    
    MListItr <CPid>   itrVideo(&_videoPidList);
    for (pPid = itrVideo.first() ; pPid; pPid = itrVideo.next())
    {
        return false;
    }

    /* also have to check ca pid values for each audio pid object */
    MListItr <CPid> itrAudio(&_audioPidList);
    for (pPid = itrAudio.first(); pPid; pPid = itrAudio.next() )
    {

        return false;
    }

    /* also have to check ca pid values for each audio pid object */
    MListItr <CPid> itrAncillary(&_ancillaryPidList);
    for (pPid = itrAncillary.first(); pPid; pPid = itrAncillary.next() )
    {
        /* Not handled yet*/
        return true;
    }

    return true;

}
