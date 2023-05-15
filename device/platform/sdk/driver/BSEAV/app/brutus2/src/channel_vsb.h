/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: channel_vsb.h $
* $brcm_Revision: 6 $
* $brcm_Date: 12/5/12 7:02p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/channel_vsb.h $
* 
* 6   12/5/12 7:02p tokushig
* SW7231-1086: fix comparison for duplicate channels during scan with
* append option set.  comparison was only comparing pids and ignoring
* freq, bandwidth, and other frontend specific parameters.
* 
* 5   11/29/12 6:07p tokushig
* SW7231-1080: fix issue with ofdm scanning where tuner pointer in
* channel was improperly initialized
* 
* 4   11/15/12 10:46a tokushig
* SW7429-369: fix build for 97429/97241 and other platforms where
* NEXUS_HAS_FRONTEND is false
* 
* 3   11/12/12 3:41p tokushig
* SW7231-749: refactoring of fast scan and tuning
* 
* 2   8/27/12 9:50a tokushig
* SW7231-749: add support for updated scan and auto pid acquisition on
* tune
* 
* 1   3/27/12 10:31a tokushig
* SW7405-5581: add vsb tuning and channel scan
* 
* 
*********************************************************************************/

#ifndef CHANNEL_VSB_H__
#define CHANNEL_VSB_H__

#if NEXUS_HAS_FRONTEND

#include "config.h"
#include "channel.h"

class CTunerVsb;

/*
    CModelChannelVsb encapulates all the functionality specific to a Vsb channel.
    This includes tune/untune and serializing to/from disk.  
*/
extern const char *g_nexusVsbModes[NEXUS_FrontendVsbMode_eMax];

class CModelChannelVsb : public CModelChannel
{
public:
    CModelChannelVsb(CTunerVsb * pTuner = NULL);
    CModelChannelVsb(const CModelChannelVsb & chVsb); /* copy constructor */
    CModelChannel * createCopy(CModelChannel * pChannel); 

    eRet initialize(PROGRAM_INFO_T *pProgramInfo);

    NEXUS_FrontendVsbMode stringToNexusVsbMode(const char * mode);
    const char * nexusVsbModeToString(NEXUS_FrontendVsbMode mode);
    uint32_t getFrequency(void) { return _settings.frequency; };
    void setFrequency(uint32_t frequency) { _settings.frequency = frequency; };
    NEXUS_FrontendVsbMode getMode(void) { return _settings.mode; };
    void setMode(NEXUS_FrontendVsbMode mode) { _settings.mode = mode; };
    
    NEXUS_FrontendVsbSettings getSettings(void) { return _settings; };
    void setSettings(NEXUS_FrontendVsbSettings settings) { _settings = settings; };

    ~CModelChannelVsb(void);

    virtual eRet tune(void * id, CModelConfig * pModelConfig, bool bWaitForLock);
    virtual eRet unTune(CModelConfig * pModelConfig, bool bCheckInTuner = true);

    virtual eRet readXML(MXmlElement * xmlElemChannel);
    virtual void writeXML(MXmlElement * xmlElemChannel);

    uint32_t mode2SymbolRate(NEXUS_FrontendVsbMode mode);

    virtual bool operator==(CModelChannel &other);
protected:
    NEXUS_FrontendVsbSettings   _settings;
};

#endif /* NEXUS_HAS_FRONTEND */
#endif /* CHANNEL_VSB_H__ */
