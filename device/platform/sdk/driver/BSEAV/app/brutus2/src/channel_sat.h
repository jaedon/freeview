/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: channel_sat.h $
* $brcm_Revision: 10 $
* $brcm_Date: 12/5/12 7:02p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/channel_sat.h $
* 
* 10   12/5/12 7:02p tokushig
* SW7231-1086: fix comparison for duplicate channels during scan with
* append option set.  comparison was only comparing pids and ignoring
* freq, bandwidth, and other frontend specific parameters.
* 
* 9   11/29/12 6:07p tokushig
* SW7231-1080: fix issue with ofdm scanning where tuner pointer in
* channel was improperly initialized
* 
* 8   11/15/12 10:46a tokushig
* SW7429-369: fix build for 97429/97241 and other platforms where
* NEXUS_HAS_FRONTEND is false
* 
* 7   11/12/12 3:40p tokushig
* SW7231-749: refactoring of fast scan and tuning
* 
* 6   8/27/12 9:50a tokushig
* SW7231-749: add support for updated scan and auto pid acquisition on
* tune
* 
* 5   3/14/12 12:40p jrubio
* SW7346-644: add tuner sat
* 
* 
*********************************************************************************/
#ifndef CHANNEL_SAT_H__
#define CHANNEL_SAT_H__

#if NEXUS_HAS_FRONTEND

#include "config.h"
#include "channel.h"

class CTunerSat;

/*
    CModelChannelSat encapulates all the functionality specific to a Sat channel.
    This includes tune/untune and serializing to/from disk.  
*/


typedef  struct satSettings{
            NEXUS_FrontendSatelliteSettings sat;
            NEXUS_FrontendDiseqcSettings diseqc;
            uint8_t diseqcChannel;
}satSettings;


extern const char *g_nexusSatModes[NEXUS_FrontendSatelliteMode_eMax];

class CModelChannelSat : public CModelChannel
{
public:
    CModelChannelSat(CTunerSat * pTuner = NULL);
    CModelChannelSat(const CModelChannelSat & chSat); /* copy constructor */
    CModelChannel * createCopy(CModelChannel * pChannel); 

    eRet initialize(PROGRAM_INFO_T *pProgramInfo);
    NEXUS_FrontendSatelliteMode stringToNexusSatMode(const char * mode);
    const char * nexusSatModeToString(NEXUS_FrontendSatelliteMode mode);
    uint32_t getFrequency(void) { return _settings.sat.frequency; };
    void setFrequency(uint32_t frequency) { _settings.sat.frequency = frequency; };
    NEXUS_FrontendSatelliteMode getMode(void) { return _settings.sat.mode; };
    void setMode(NEXUS_FrontendSatelliteMode mode) { _settings.sat.mode = mode; };
    
    satSettings getSettings(void) { return _settings; };
    void setSettings(satSettings settings ) { _settings = settings; };

    virtual eRet tune(void * id, CModelConfig * pModelConfig, bool bWaitForLock);
    virtual eRet unTune(CModelConfig * pModelConfig, bool bCheckInTuner = true);

    virtual eRet readXML(MXmlElement * xmlElemChannel);
    virtual void writeXML(MXmlElement * xmlElemChannel);

    ~CModelChannelSat(void);

    virtual bool operator==(CModelChannel &other);
protected:
    satSettings           _settings;
 
};

#endif /* NEXUS_HAS_FRONTEND */
#endif /* CHANNEL_SAT_H__ */
