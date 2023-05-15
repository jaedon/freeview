/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: channel_qam.h $
* $brcm_Revision: 14 $
* $brcm_Date: 11/29/12 6:07p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/channel_qam.h $
* 
* 14   11/29/12 6:07p tokushig
* SW7231-1080: fix issue with ofdm scanning where tuner pointer in
* channel was improperly initialized
* 
* 13   11/15/12 10:46a tokushig
* SW7429-369: fix build for 97429/97241 and other platforms where
* NEXUS_HAS_FRONTEND is false
* 
* 12   11/12/12 3:40p tokushig
* SW7231-749: refactoring of fast scan and tuning
* 
* 11   8/27/12 9:50a tokushig
* SW7231-749: add support for updated scan and auto pid acquisition on
* tune
* 
* 10   3/13/12 4:00p tokushig
* SW7405-5581: more CModelChannel base class out of channelmgr file and
* into channel file
* 
* 9   3/12/12 4:42p tokushig
* SW7405-5581: after channel load, unnumbered channels will be auto-
* numbered starting with the last valid channel number in the channel
* list.  channel numbers can not be saved/restored in channel list
* save/load.  moved code to load channels from xml to the appropriate
* channel classes.
* 
* 8   3/9/12 3:47p jrubio
* SW7346-644: add qam write XML
* 
* 7   3/9/12 2:55p tokushig
* SW7405-5581: added support for channel list save.  also added initial
* code to handle tuner lock/unlock after first tune.
* 
* 6   3/7/12 10:09a jrubio
* SW7346-644: add some functions
* 
* 5   2/28/12 5:03p tokushig
* SW7405-5581: add header comments
* 
* 
*********************************************************************************/

#ifndef CHANNEL_QAM_H__
#define CHANNEL_QAM_H__

#if NEXUS_HAS_FRONTEND 

#include "config.h"
#include "channel.h"

class CTunerQam;

/*
    CModelChannelQam encapulates all the functionality specific to a Qam channel.
    This includes tune/untune and serializing to/from disk.  
*/
extern const char *g_nexusQamModes[NEXUS_FrontendQamMode_eMax];

class CModelChannelQam : public CModelChannel
{
public:
    CModelChannelQam(CTunerQam * pTuner = NULL);
    CModelChannelQam(const CModelChannelQam & chQam); /* copy constructor */
    CModelChannel * createCopy(CModelChannel * pChannel); 

    eRet initialize(PROGRAM_INFO_T *pProgramInfo);

    NEXUS_FrontendQamMode stringToNexusQamMode(const char * mode);
    const char * nexusQamModeToString(NEXUS_FrontendQamMode mode);
    uint32_t getFrequency(void) { return _settings.frequency; };
    void setFrequency(uint32_t frequency) { _settings.frequency = frequency; };
    NEXUS_FrontendQamMode getMode(void) { return _settings.mode; };
    void setMode(NEXUS_FrontendQamMode mode) { _settings.mode = mode; };
    NEXUS_FrontendQamAnnex getAnnex(void) { return _settings.annex; };
    void setAnnex(NEXUS_FrontendQamAnnex annex) { _settings.annex = annex; };
    
    NEXUS_FrontendQamSettings getSettings(void) { return _settings; };
    void setSettings(NEXUS_FrontendQamSettings settings) { _settings = settings; };

    ~CModelChannelQam(void);

    virtual eRet tune(void * id, CModelConfig * pModelConfig, bool bWaitForLock);
    virtual eRet unTune(CModelConfig * pModelConfig, bool bCheckInTuner = true);

    virtual eRet readXML(MXmlElement * xmlElemChannel);
    virtual void writeXML(MXmlElement * xmlElemChannel);

    uint32_t mode2SymbolRate(NEXUS_FrontendQamMode mode);

    virtual bool operator==(CModelChannel &other);

protected:
    NEXUS_FrontendQamSettings   _settings;
};


#endif /* NEXUS_HAS_FRONTEND */
#endif /* CHANNEL_QAM_H__ */
