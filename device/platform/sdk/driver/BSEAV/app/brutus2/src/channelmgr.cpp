/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: channelmgr.cpp $
* $brcm_Revision: 31 $
* $brcm_Date: 11/15/12 10:46a $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/channelmgr.cpp $
* 
* 31   11/15/12 10:46a tokushig
* SW7429-369: fix build for 97429/97241 and other platforms where
* NEXUS_HAS_FRONTEND is false
* 
* 30   10/19/12 1:40p tokushig
* SW7231-749: add force printout option to dumpChannelList() to override
* existing debug level.  default is NOT to override
* 
* 29   10/11/12 7:38p tokushig
* SW7231-749: make record indicator capable of showing multiple
* recordings
* 
* 28   8/27/12 9:59a tokushig
* SW7231-749: add support for updated scan and auto pid acquisition on
* tune
* 
* 27   7/24/12 3:34p tokushig
* SW7231-749: fix ofdm channel creation
* 
* 26   5/25/12 12:23a tokushig
* SW7231-749: remove channel list xml parse debug output
* 
* 25   3/27/12 3:30p tokushig
* SW7405-5581: synchronize lua commands
* 
* 24   3/27/12 10:30a tokushig
* SW7405-5581: add vsb tuning and channel scan
* 
* 23   3/19/12 4:24p tokushig
* SW7405-5581: Add support for filtering channel list during scan.
* Ignore encrypted channels, channels with invalid pcr pids, and audio
* only channels.
* 
* 22   3/14/12 2:38p jrubio
* SW7346-644: make WRN in MSG
* 
* 21   3/13/12 4:00p tokushig
* SW7405-5581: more CModelChannel base class out of channelmgr file and
* into channel file
* 
* 20   3/12/12 9:02p tokushig
* SW7405-5581: fix problem with out of sync codec to string conversion.
* changed conversion of av types code to use hash tables instead of enum
* based array mappings to avoid out of sync lookup problems that can
* occur when NEXUS_AudioCodec and NEXUS_VideoCodec enums do not match
* lookup array mappings.  if a new nexus enum is added, existing lookup
* will still work...lookups involving the new enum value will map to
* unknown.  if a nexus enum is removed, the compiler will catch the
* inconsistency.
* 
* 19   3/12/12 4:42p tokushig
* SW7405-5581: after channel load, unnumbered channels will be auto-
* numbered starting with the last valid channel number in the channel
* list.  channel numbers can not be saved/restored in channel list
* save/load.  moved code to load channels from xml to the appropriate
* channel classes.
* 
* 18   3/9/12 3:48p jrubio
* SW7346-644: rename save xml file
* 
* 17   3/9/12 3:22p jrubio
* SW7346-644: add XML Print
* 
* 16   3/7/12 10:10a jrubio
* SW7346-644: add xml functions,move parsing to channel classes
* 
* 15   3/2/12 4:54p tokushig
* SW7405-5581: brutus.channelListLoad() lua command working.
* 
* 14   2/29/12 11:50a jrubio
* SW7346-644: add avtypes.cpp
* 
* 13   2/28/12 3:01p tokushig
* SW7405-5581: add timer support and use it for deferred tuning using ch
* up/down.  fast pressing of ch up/down remote buttons will update the
* ui but not tune until button presses stop for 500msecs.  this allows
* the user to skip channels while traversing the channel list using the
* remote
* 
* 12   2/20/12 4:47p jrubio
* SW7346-644: read file from config
* 
* 
*********************************************************************************/
#include "channelmgr.h"

#include "mxmlparser.h"
#include "mxmlelement.h"
#include "channel_sat.h"
#include "channel_qam.h"
#include "channel_vsb.h"
#include "channel_ofdm.h"
#include "channel_streamer.h"
#include <unistd.h> //file i/o
#include <fcntl.h>

BDBG_MODULE(brutus_channelmgr);


CModelChannelMgr::CModelChannelMgr() :
    _pCfg(NULL),
    _currentChannel(NULL),
    _deferredChannel(NULL)
{
    _mutex = B_Mutex_Create(NULL);
    BDBG_ASSERT(_mutex);
}

eRet CModelChannelMgr::clearChannelList()
{
    eRet            ret      = eRet_Ok;
    CModelChannel * pChannel = NULL;

    /* serialized access to _channelList */
    CScopedMutex channelListMutex(_mutex);

    /* clear all channels from list (auto deletes data) */
    _channelList.clear();

    return ret;
}

uint32_t CModelChannelMgr::filterChannel(CModelChannel * pChannel)
{
    uint32_t ret = CHMGR_FILTER_PASS;

    if ((false == GET_BOOL(_pCfg, SCAN_ENCRYPTED_CHANNELS)) &&
        (true  == pChannel->isEncrypted()))
    {
        BDBG_WRN(("scan ignoring encrypted channel"));
        ret |= CHMGR_FILTER_FAIL_ENCRYPTED;
    }
    
    if (NULL == pChannel->getPid(0, ePidType_Pcr))
    {
        BDBG_WRN(("scan ignoring channel with missing pcr pid"));
        ret |= CHMGR_FILTER_FAIL_PCR;
    }
    
    if ((false == GET_BOOL(_pCfg, SCAN_AUDIO_ONLY_CHANNELS)) &&
        (NULL  == pChannel->getPid(0, ePidType_Video)))
    {
        BDBG_WRN(("scan ignoring audio only channel"));
        ret |= CHMGR_FILTER_FAIL_AUDIOONLY;
    }
    
    if ((false == GET_BOOL(_pCfg, SCAN_DUPLICATE_CHANNELS)) &&
        (true == isDuplicate(pChannel)))
    {
        BDBG_WRN(("scan ignoring duplicate channel"));
        ret |= CHMGR_FILTER_FAIL_DUPLICATE;
    }

done:
    return ret;
}

/* Add given channel to channel list.  Because the channel manager does not know what type
   of channel is being added, the caller must allocate the proper subclass of CModelChannel and
   pass it in for storage.  The channel manager will then own the channel and will handle its
   destruction when necessary.
 */
eRet CModelChannelMgr::addChannel(CModelChannel * pChannel)
{
    /* serialized access to _channelList */
    CScopedMutex channelListMutex(_mutex);

    BDBG_MSG(("--- ADD NEW CHANNEL TO CH LIST ---"));
    _channelList.add(pChannel);

    return eRet_Ok;
}

/* Moves channels from the given list to the actual channel list.  The actual channel
   list will own the new channels and will delete them when necessary.  if append is true,
   the new channels are appended to the channel list.  otherwise, the new channels replace
   the channel list. */
eRet CModelChannelMgr::addChannelList(MList<CModelChannel> * pList, bool append)
{
    if (false == append)
    {
        /* we are replacing the existing channel list - clear current list */
        clearChannelList();
    }

    /* add new channels to existing channel list (note that the allocated channels in
       the new list are simply removed and placed in the actual channel list.
       the actual channel list will own them and delete them when necessary.) */
    CModelChannel * pChannel = NULL;
    while (NULL != (pChannel = pList->remove(0)))
    {
        addChannel(pChannel);
    }

    /* we've modified the channel list so set the current channel to the beginning */
    setCurrentChannel(NULL);
}

#define MAXBUF 1024*20

static int compare(CModelChannel * ch1, CModelChannel * ch2)
{
    uint16_t majorCh1 = 0;
    uint16_t majorCh2 = 0;
    uint8_t  minorCh1 = 0;
    uint8_t  minorCh2 = 0;

    BDBG_ASSERT(NULL != ch1);
    BDBG_ASSERT(NULL != ch2);

    majorCh1 = ch1->getMajor();
    minorCh1 = ch1->getMinor();
    majorCh2 = ch2->getMajor();
    minorCh2 = ch2->getMinor();
    if (majorCh1 == majorCh2)
    {
        return minorCh1 - minorCh2;
    }

    return majorCh1 - majorCh2;
}

eRet CModelChannelMgr::loadChannelList(const char * listName, const bool append)
{
    MXmlElement * xmlElemTop         = NULL;
    int           fd                 = 0;
    char        * buf                = NULL;
    int           nBufSize           = 0;
    eRet          ret                = eRet_Ok;
    int           channelNum         = 1;
    MXmlElement * xmlElemChannelList = NULL;
    MXmlParser    xmlParser;

    /* if we are replacing the channel list, make sure we are not tuned to any channel */
    BDBG_ASSERT((false == append) ? NULL == findTunedChannel() : true);

    fd = open(listName, O_RDONLY, 0);
    if (!fd)
    {
        BDBG_ERR(("Failed to open file '%s'!", listName));
        return eRet_ExternalError;
    }

    buf = (char *)malloc(sizeof(char) * MAXBUF);
    CHECK_PTR_GOTO("Error malloc parse buffer!", buf, ret, eRet_OutOfMemory, error);

    if (fd >= 0)
    {
        memset(buf, 0, sizeof(char) * MAXBUF);
        nBufSize = read(fd, buf, MAXBUF);
        close(fd);
        fd = 0;
    }

    /*
    BDBG_MSG(("----------------------------------------------------------"));
    BDBG_MSG(("BUF:\n%s", buf));
    */
    
    if (nBufSize <= 0)
    {
        BDBG_WRN(("Cannot open XML channel list file: %s", listName));
        ret = eRet_ExternalError;
        goto error;
    }

    xmlElemTop = xmlParser.parse(buf);
    CHECK_PTR_GOTO("Syntax error parsing channel list XML", xmlElemTop, ret, eRet_ExternalError, error);

    if (false == append)
    {
        clearChannelList();
    }

#if 0
    BDBG_MSG(("----------------------------------------------------------"));
    BDBG_MSG(("xmlElemTop:"));
    xmlElemTop->print();
#endif

    xmlElemChannelList = xmlElemTop->findChild(XML_TAG_CHANNELLIST);
    if (xmlElemChannelList)
    {
        MXmlElement * xmlElemChannel = NULL;

        //save Channel Here
        for (xmlElemChannel = xmlElemChannelList->firstChild(); 
             xmlElemChannel; 
             xmlElemChannel = xmlElemChannelList->nextChild())
        {
            CModelChannel * pCh = NULL;
            BDBG_MSG(("XML CHANNEL channelNum %d", channelNum ));
            
            if (xmlElemChannel)
            {
                if (xmlElemChannel->tag() != XML_TAG_CHANNEL) {
                    BDBG_WRN((" Tag is not Channel its %s",xmlElemChannel->tag().s()));
                    continue;
                }

                MString strName = xmlElemChannel->attrValue(XML_ATT_TYPE);
                BDBG_MSG(("XML_ATT_TYPE : %s", strName.s() ));
    
#if NEXUS_HAS_FRONTEND
                if (strName == "sds")
                {
                    pCh = new CModelChannelSat();
                    CHECK_PTR_GOTO("Error allocating Sat channel", pCh, ret, eRet_OutOfMemory, channel_error);
                }
                else
                if(strName == "qam")
                {
                    pCh = new CModelChannelQam();
                    CHECK_PTR_GOTO("Error allocating QAM channel", pCh, ret, eRet_OutOfMemory, channel_error);
                }
                else
                if(strName == "vsb")
                {
                    pCh = new CModelChannelVsb();
                    CHECK_PTR_GOTO("Error allocating VSB channel", pCh, ret, eRet_OutOfMemory, channel_error);
                }
                else
                if(strName == "ofdm")
                {
                    pCh = new CModelChannelOfdm();
                    CHECK_PTR_GOTO("Error allocating Ofdm channel", pCh, ret, eRet_OutOfMemory, channel_error);
                }
                else
#endif /* NEXUS_HAS_FRONTEND */
                if(strName == "streamer")
                {
                    pCh = new CModelChannelStreamer();
                    CHECK_PTR_GOTO("Error allocating Streamer channel", pCh, ret, eRet_OutOfMemory, channel_error);
                }
                else
                {
                    BDBG_WRN(("Unhandled channel type:%s - skipping", strName.s()));
                }
    
                pCh->readXML(xmlElemChannel);

                addChannel(pCh);
            }

            channelNum++;
            continue;

channel_error:
            if (NULL != pCh)
            {
                delete pCh;
            }
        }

        /* if the added channels did not contain valid major/minor channel numbers then we will
           re-number them here*/
        {
            MListItr <CModelChannel> itr(&_channelList);
            CModelChannel * pChLast = NULL;
            CModelChannel * ptr     = NULL;
            uint8_t         major   = 1;

            /* get the last major channel number and make sure it is non-zero */
            pChLast = getLastChannel();
            if (NULL != pChLast)
            {
                major = pChLast->getMajor();
                if (0 == major)
                {
                    major = 1;
                }
            }

            /* serialized access to _channelList */
            CScopedMutex channelListMutex(_mutex);

            for (ptr = itr.first(); ptr; ptr = itr.next())
            {
                if (0 == ptr->getMajor())
                {
                    ptr->setMajor(major);
                    ptr->setMinor(1);
                    major++;
                }
            }

            /* keep list sorted by channel number */
            _channelList.sort(compare);
        }
    }

error:
    if (fd)
    {
        close(fd);
        fd = 0;
    }

    if (xmlElemTop) 
    {
        delete xmlElemTop;
        xmlElemTop = NULL;
    }

    if (buf)
    {
        free(buf);
        buf = NULL;
    }

    setCurrentChannel();

    notifyObservers(eNotify_ChannelListChanged);
    dumpChannelList();

    return ret;
}

/* 
 Print the list of channels to XML, traverse the channel list
 */
eRet CModelChannelMgr::saveChannelList(const char * fileName,bool append)
{
    CModelChannel            * ch                 = NULL;
    MXmlElement              * xmlElemChannelList = new MXmlElement(NULL,XML_TAG_CHANNELLIST);
    MXmlElement              * xmlElemChannel     = NULL;
    FILE                     * file               = NULL;
    MListItr <CModelChannel>   itr(&_channelList);

    file = fopen(fileName,(append)?"a":"w");
    if (!file ) 
    {
        BDBG_ERR(("Cannot open file for writing %s," ,fileName));
        return eRet_ExternalError;
    }

    /* serialized access to _channelList */
    CScopedMutex channelListMutex(_mutex);
    
    for (ch = itr.first(); ch; ch = itr.next())
    {
        /* Start with first Element */
        xmlElemChannel = new MXmlElement(xmlElemChannelList, XML_TAG_CHANNEL);
        ch->writeXML(xmlElemChannel );
    }

    xmlElemChannelList->print(file);

    if (file) 
    {
        fclose(file);
    }

    BDBG_MSG(("----------------------------------------------------------"));
    BDBG_MSG(("Saved channel list to file: %s", fileName));

    return eRet_Ok;
}

/* if bForce == true then override existing debug level and force printout */
void CModelChannelMgr::dumpChannelList(bool bForce)
{
    MListItr <CModelChannel>   itr(&_channelList);
    CModelChannel            * ptr = NULL;
    BDBG_Level                 level;

    /* serialized access to _channelList */
    CScopedMutex channelListMutex(_mutex);

    if (true == bForce)
    {
        BDBG_GetModuleLevel("brutus_channel", &level);
        BDBG_SetModuleLevel("brutus_channel", BDBG_eMsg);
    }

    for (ptr = itr.first(); ptr; ptr = itr.next())
    {
        ptr->dump();
    }

    if (true == bForce)
    {
        BDBG_SetModuleLevel("brutus_channel", level);
    }
}

/* find any tuned channel */
CModelChannel * CModelChannelMgr::findTunedChannel()
{
    MListItr <CModelChannel> itr(&_channelList);
    CModelChannel * ptr = NULL;

    /* serialized access to _channelList */
    CScopedMutex channelListMutex(_mutex);

    for (ptr = itr.first(); ptr; ptr = itr.next())
    {
        if (true == ptr->isTuned())
        {
            break;
        }
    }

    return ptr;
}

/* find any tuned channel that matches the given tuner type */
/* dtt todo: extend this to check only a particular given tuner (so if there are 2 qam tuners,
             we could find the tuned channel for say the 2nd tuner only */
CModelChannel * CModelChannelMgr::findTunedChannel(eBoardResource tunerType)
{
    MListItr <CModelChannel> itr(&_channelList);
    CModelChannel * ptr = NULL;

    /* serialized access to _channelList */
    CScopedMutex channelListMutex(_mutex);

    for (ptr = itr.first(); ptr; ptr = itr.next())
    {
        if ((true == ptr->isTuned()) && (tunerType == ptr->getType()))
        {
            break;
        }
    }

    return ptr;
}

CModelChannel * CModelChannelMgr::getCurrentChannel()
{
    if (NULL == _currentChannel)
    {
        _currentChannel = _channelList.first();
    }
    
    return _currentChannel;
}

void CModelChannelMgr::setCurrentChannel(CModelChannel * pChannel)
{
    {
        /* serialized access to _channelList */
        CScopedMutex channelListMutex(_mutex);

        if (NULL == pChannel)
        {
            _currentChannel = _channelList.first();
        }
        else
        {
            _currentChannel = pChannel;
        }
    }

    notifyObservers(eNotify_CurrentChannel);
}

CModelChannel * CModelChannelMgr::getDeferredChannel()
{
    return (NULL != _deferredChannel) ? _deferredChannel : getCurrentChannel();
}

void CModelChannelMgr::setDeferredChannel(CModelChannel * pChannel)
{
    _deferredChannel = pChannel;

    notifyObservers(eNotify_DeferredChannel);
}

/* note that this method does not alter the _currentChannel */
CModelChannel * CModelChannelMgr::getNextChannel(CModelChannel * pChannel, bool bWrap)
{
    CModelChannel * pCh       = NULL;
    CModelChannel * pChFound  = NULL;

    /* serialized access to _channelList */
    CScopedMutex channelListMutex(_mutex);

    if (NULL != pChannel)
    {
        /* use channel as starting point if given */
        pCh = pChannel;
    }
    else
    {
        /* no channel given so use current channel as starting point */
        pCh = getCurrentChannel();
    }

    if (_channelList.last() == pCh)
    {
        if (true == bWrap)
        {
            /* we are at the end of the channel list, wrap to beginning */
            pChFound = _channelList.first();
        }
        else
        {
            goto done;
        }
    }
    else
    {
        /* set internal list iterator to the given channel so we can use "next()" */
        _channelList.index(pCh);
        pChFound = _channelList.next();
    }

done:
    return pChFound;
}

/* note that this method does not alter the _currentChannel */
CModelChannel * CModelChannelMgr::getPrevChannel(CModelChannel * pChannel, bool bWrap)
{
    CModelChannel * pCh      = NULL;
    CModelChannel * pChFound = NULL;

    /* serialized access to _channelList */
    CScopedMutex channelListMutex(_mutex);

    if (NULL != pChannel)
    {
        /* use channel as starting point if given */
        pCh = pChannel;
    }
    else
    {
        /* no channel given so use current channel as starting point */
        pCh = getCurrentChannel();
    }

    if (_channelList.first() == pCh)
    {
        if (true == bWrap)
        {
            /* we are at the beginning of the channel list, wrap to end */
            pChFound = _channelList.last();
        }
        else
        {
            goto done;
        }
    }
    else
    {
        /* set internal list iterator to the given channel so we can use "prev()" */
        _channelList.index(pCh);
        pChFound = _channelList.prev();
    }

done:
    return pChFound;
}

CModelChannel * CModelChannelMgr::getFirstChannel()
{
    /* serialized access to _channelList */
    CScopedMutex channelListMutex(_mutex);

    return _channelList.first();
}

CModelChannel * CModelChannelMgr::getLastChannel()
{
    /* serialized access to _channelList */
    CScopedMutex channelListMutex(_mutex);

    return _channelList.last();
}

/* two channels are duplicates if either the channel contents are identical or
   the pointer point to the same channel object */
bool CModelChannelMgr::isDuplicate(CModelChannel * pChannel)
{
    MListItr <CModelChannel> itr(&_channelList);
    CModelChannel * ptr = NULL;

    /* serialized access to _channelList */
    CScopedMutex channelListMutex(_mutex);

    for (ptr = itr.first(); ptr; ptr = itr.next())
    {
        /* compare channel contents */
        if (*ptr == *pChannel)
        {
            break;
        }
    }

    return (NULL != ptr ? true : false);
}

static int compareChannels(CModelChannel * ch1, CModelChannel * ch2)
{
    if (ch1->getMajor() > ch2->getMajor())
    {
        return 1;
    }
    else
    if (ch1->getMajor() < ch2->getMajor())
    {
        return -1;
    }

    /* major numbers are equal */

    if (ch1->getMinor() > ch2->getMinor())
    {
        return 1;
    }
    else
    if (ch1->getMinor() < ch2->getMinor())
    {
        return -1;
    }

    /* minor number are equal */

    return 0;
}

void CModelChannelMgr::sortChannelList()
{
    /* serialized access to _channelList */
    CScopedMutex channelListMutex(_mutex);
    
    _channelList.sort(compareChannels);
}

CModelChannelMgr::~CModelChannelMgr()
{
    if (_mutex)
    {
        B_Mutex_Destroy(_mutex);
        _mutex = NULL;
    }
}


