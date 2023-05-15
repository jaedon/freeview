/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: channel_ofdm.h $
* $brcm_Revision: 8 $
* $brcm_Date: 12/5/12 7:03p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/channel_ofdm.h $
* 
* 8   12/5/12 7:03p tokushig
* SW7231-1086: fix comparison for duplicate channels during scan with
* append option set.  comparison was only comparing pids and ignoring
* freq, bandwidth, and other frontend specific parameters.
* 
* 7   11/29/12 6:07p tokushig
* SW7231-1080: fix issue with ofdm scanning where tuner pointer in
* channel was improperly initialized
* 
* 6   11/15/12 10:46a tokushig
* SW7429-369: fix build for 97429/97241 and other platforms where
* NEXUS_HAS_FRONTEND is false
* 
* 5   11/12/12 3:41p tokushig
* SW7231-749: refactoring of fast scan and tuning
* 
* 4   11/7/12 5:54p jrubio
* SW7231-1060: remove mapInputBand Fuction, add handling for dvbt,dvbt2
* 
* 3   8/27/12 9:53a tokushig
* SW7231-749: add support for updated scan and auto pid acquisition on
* tune
* 
* 3   8/27/12 9:50a tokushig
* SW7231-749: add support for updated scan and auto pid acquisition on
* tune
* 
* 2   7/24/12 5:08p jrubio
* SW7346-848: add ofdm cabalities, new parserband function
* 
* 1   7/23/12 4:14p tokushig
* SW7231-749: add ofdm tuning/scanning and gui support (no lua support)
* 
* 
*********************************************************************************/

#ifndef CHANNEL_OFDM_H__
#define CHANNEL_OFDM_H__

#if NEXUS_HAS_FRONTEND

#include "config.h"
#include "channel.h"

class CTunerOfdm;

/*
    CModelChannelOfdm encapulates all the functionality specific to a Ofdm channel.
    This includes tune/untune and serializing to/from disk.  
*/
extern const char *g_nexusOfdmModes[NEXUS_FrontendOfdmMode_eMax];

class CModelChannelOfdm : public CModelChannel
{
public:
    CModelChannelOfdm(CTunerOfdm * pTuner = NULL);
    CModelChannelOfdm(const CModelChannelOfdm & chOfdm); /* copy constructor */
    CModelChannel * createCopy(CModelChannel * pChannel); 

    eRet initialize(PROGRAM_INFO_T *pProgramInfo);

    NEXUS_FrontendOfdmMode stringToNexusOfdmMode(const char * mode);
    const char * nexusOfdmModeToString(NEXUS_FrontendOfdmMode mode);
    uint32_t getFrequency(void) { return _settings.frequency; };
    void setFrequency(uint32_t frequency) { _settings.frequency = frequency; };
    NEXUS_FrontendOfdmMode getMode(void) { return _settings.mode; };
    void setMode(NEXUS_FrontendOfdmMode mode) { _settings.mode = mode; };
    
    NEXUS_FrontendOfdmSettings getSettings(void) { return _settings; };
    void setSettings(NEXUS_FrontendOfdmSettings settings) { _settings = settings; };
    //eRet mapInputBand(CInputBand * pInputBand);

    ~CModelChannelOfdm(void);

    virtual eRet tune(void * id, CModelConfig * pModelConfig, bool bWaitForLock);
    virtual eRet unTune(CModelConfig * pModelConfig, bool bCheckInTuner = true);

    virtual eRet readXML(MXmlElement * xmlElemChannel);
    virtual void writeXML(MXmlElement * xmlElemChannel);

    virtual bool operator==(CModelChannel &other);
protected:
    NEXUS_FrontendOfdmSettings   _settings;
};


#endif /* NEXUS_HAS_FRONTEND */
#endif /* CHANNEL_OFDM_H__ */
