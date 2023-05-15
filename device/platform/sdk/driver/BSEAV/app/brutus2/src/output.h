/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: output.h $
* $brcm_Revision: 8 $
* $brcm_Date: 10/7/12 3:06p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/output.h $
* 
* 8   10/7/12 3:06p tokushig
* SW7231-749: add format change feature and display output settings
* 
* 7   9/10/12 3:48p tokushig
* SW7231-749: update output connect/disconnect methods
* 
* 6   5/25/12 9:28a jrubio
* SW7346-644: add HDMI audio functions
* 
* 5   5/23/12 5:24p jrubio
* SW7346-644: add hdmi, configure HDMI to be derived from CVideoOutput
* class
* 
* 4   3/28/12 4:45p jrubio
* SW7231-749: add 7231 support
* 
* 3   2/28/12 5:03p tokushig
* SW7405-5581: add header comments
* 
* 
*********************************************************************************/

#ifndef OUTPUT_H__
#define OUTPUT_H__

#include "resource.h"
#include "brutus_cfg.h"
#include "brutus_os.h"

#include "nexus_svideo_output.h"
#include "nexus_component_output.h"
#include "nexus_composite_output.h"

#include "nexus_spdif_output.h"
#include "nexus_audio_dac.h"

class CVideoOutput : public CResource
{
public:
    CVideoOutput(const char * name, const uint16_t number, const eBoardResource type, CConfiguration * pCfg);
    ~CVideoOutput(void);

    virtual NEXUS_VideoOutput getConnector() = 0;
    virtual eRet setColorSpace(NEXUS_ColorSpace colorSpace) { return eRet_Ok; };
    virtual eRet setMpaaDecimation(bool bMpaaDecimation) { return eRet_Ok; };
    virtual bool isValidVideoFormat(NEXUS_VideoFormat format) = 0;

protected:
};

class COutputComponent : public CVideoOutput
{
public:
    COutputComponent(const char * name, const uint16_t number, CConfiguration * pCfg);
    ~COutputComponent(void);
    
    virtual eRet initialize();
    virtual NEXUS_VideoOutput getConnector();
    virtual eRet setColorSpace(NEXUS_ColorSpace colorSpace);
    virtual eRet setMpaaDecimation(bool bMpaaDecimation);
    virtual bool isValidVideoFormat(NEXUS_VideoFormat format);

    NEXUS_ComponentOutputHandle getOutput() { return _outputComponent; };
    NEXUS_ComponentOutputType getDesiredType() { return _desiredType; };

protected:
    NEXUS_ComponentOutputHandle     _outputComponent;
    NEXUS_ComponentOutputType       _desiredType;
};


class COutputSVideo : public CVideoOutput
{
public:
    COutputSVideo(const char * name, const uint16_t number, CConfiguration * pCfg);
    ~COutputSVideo(void);
    
    virtual eRet initialize();
    virtual NEXUS_VideoOutput getConnector();
    virtual bool isValidVideoFormat(NEXUS_VideoFormat format);

protected:
    NEXUS_SvideoOutputHandle     _outputSVideo;
};



class COutputComposite : public CVideoOutput
{
public:
    COutputComposite(const char * name, const uint16_t number, CConfiguration * pCfg);
    ~COutputComposite(void);
    
    virtual eRet initialize();
    virtual NEXUS_VideoOutput getConnector();
    virtual bool isValidVideoFormat(NEXUS_VideoFormat format);

    NEXUS_CompositeOutputHandle getOutput() { return _outputComposite; };

protected:
    NEXUS_CompositeOutputHandle     _outputComposite;
};


#include "nexus_hdmi_output_hdcp.h"

class COutputHdmi : public CVideoOutput
{
public:
    COutputHdmi(const char * name, const uint16_t number, CConfiguration * pCfg);
    ~COutputHdmi(void);
    
    virtual eRet initialize();
    virtual NEXUS_VideoOutput getConnector();
    virtual NEXUS_AudioOutput getAudioConnector();
    virtual eRet setColorSpace(NEXUS_ColorSpace colorSpace);
    virtual bool isValidVideoFormat(NEXUS_VideoFormat format);

    NEXUS_HdmiOutputHandle getOutput(void) { return _outputHdmi; };
    eRet connect(NEXUS_AudioInput input);

    bool isConnected(void) { return _connected; };
    bool isHdcpPreferred(void) { return _preferredHdcp; };
    void setPreferredVideoFormat(NEXUS_VideoFormat format) { _preferredVideoFormat = format; };
    eRet processHotplugStatus(NEXUS_HdmiOutputStatus * pStatus);
    eRet processHdcpStatus(NEXUS_HdmiOutputHdcpStatus * pStatus);
    eRet startHdcpAuthentication(void);
    void clearHdcpTimer(void) { _hdcpTimer = NULL; };
    eRet muteAudio(bool mute);

protected:
    bool                       _connected;
    NEXUS_HdmiOutputHandle     _outputHdmi;
    B_EventHandle              _event;
    B_EventHandle              _hdcpEvent;
    B_SchedulerEventId         _eventId;
    B_SchedulerEventId         _hdcpEventId;

    NEXUS_VideoFormat          _preferredVideoFormat;
    bool                       _preferredHdcp;
    bool                       _handleHdcpFailures;
    uint16_t                   _hdcpFailureRetryDelay;
    B_SchedulerTimerId         _hdcpTimer;
    CAudioDecode             * _pAudioDecode;
};




#include "audio_decode.h"

class CAudioOutput : public CResource
{
public:
    CAudioOutput(const char * name, const uint16_t number, eBoardResource type, CConfiguration * pCfg);
    ~CAudioOutput(void);

    virtual NEXUS_AudioOutput getConnector() = 0;
    virtual eRet connect(NEXUS_AudioInput input);
    virtual eRet disconnect();

protected:
    CAudioDecode           * _pAudioDecode;
};


class COutputSpdif : public CAudioOutput
{
public:
    COutputSpdif(const char * name, const uint16_t number, CConfiguration * pCfg);
    ~COutputSpdif(void);
    
    virtual eRet initialize();
    virtual NEXUS_AudioOutput getConnector();
    NEXUS_SpdifOutputHandle getOutput(void) { return _outputSpdif; };

protected:
    NEXUS_SpdifOutputHandle     _outputSpdif;
};


class COutputAudioDac : public CAudioOutput
{
public:
    COutputAudioDac(const char * name, const uint16_t number, CConfiguration * pCfg);
    ~COutputAudioDac(void);
    
    virtual eRet initialize();
    virtual NEXUS_AudioOutput getConnector();

protected:
    NEXUS_AudioDacHandle     _outputAudioDac;
};


#endif /* OUTPUT_H__ */
