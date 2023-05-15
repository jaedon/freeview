/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: output.cpp $
* $brcm_Revision: 9 $
* $brcm_Date: 10/10/12 5:31p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/output.cpp $
* 
* 9   10/10/12 5:31p tokushig
* SW7231-749: adjust debug output for release
* 
* 8   10/7/12 3:06p tokushig
* SW7231-749: add format change feature and display output settings
* 
* 7   9/10/12 3:48p tokushig
* SW7231-749: update output connect/disconnect methods
* 
* 6   5/25/12 3:12p tokushig
* SW7231-749: remove platform specific file and move to common
* platform.h.  also change appropriate brutus defines to nexus defines
* when available.
* 
* 5   5/25/12 9:28a jrubio
* SW7346-644: add HDMI audio functions
* 
* 4   5/23/12 5:24p jrubio
* SW7346-644: add hdmi, configure HDMI to be derived from CVideoOutput
* class
* 
* 3   2/28/12 5:03p tokushig
* SW7405-5581: add header comments
* 
* 
*********************************************************************************/

#include "output.h"
#include "brutus_os.h"
#include "mixer.h"

BDBG_MODULE(brutus_output);



CVideoOutput::CVideoOutput(const char * name, const uint16_t number, eBoardResource type, CConfiguration * pCfg) :
    CResource(name, number, type, pCfg)
{
}

CVideoOutput::~CVideoOutput()
{
}


COutputComponent::COutputComponent(const char * name, const uint16_t number, CConfiguration * pCfg) :
    CVideoOutput(name, number, eBoardResource_outputComponent, pCfg)
{
}

COutputComponent::~COutputComponent()
{
}

eRet COutputComponent::initialize()
{
    NEXUS_PlatformConfiguration platformConfig;
    eRet                        ret             = eRet_Ok;

    NEXUS_Platform_GetConfiguration(&platformConfig);
    _outputComponent = platformConfig.outputs.component[_number];
    _desiredType     = NEXUS_ComponentOutputType_eYPrPb;

    return ret;
}

NEXUS_VideoOutput COutputComponent::getConnector()
{
    BDBG_ASSERT(_outputComponent);
    return NEXUS_ComponentOutput_GetConnector(_outputComponent);
}

eRet COutputComponent::setColorSpace(NEXUS_ColorSpace colorSpace)
{
    NEXUS_ComponentOutputSettings settings;
    NEXUS_Error                   nError    = NEXUS_SUCCESS;
    eRet                          ret       = eRet_Ok;

    NEXUS_ComponentOutput_GetSettings(getOutput(), &settings);

    switch (colorSpace)
    {
    case NEXUS_ColorSpace_eYCbCr422:
    case NEXUS_ColorSpace_eYCbCr444:
        settings.type = NEXUS_ComponentOutputType_eYPrPb;
        break;

    case NEXUS_ColorSpace_eRgb:
    default:
        settings.type = NEXUS_ComponentOutputType_eRGB;
        break;
    }

    nError = NEXUS_ComponentOutput_SetSettings(getOutput(), &settings);
    CHECK_NEXUS_ERROR_GOTO("error setting component output settings", ret, nError, error);

error:
    return ret;
}

eRet COutputComponent::setMpaaDecimation(bool bMpaaDecimation)
{
    NEXUS_ComponentOutputSettings settings;
    NEXUS_Error                   nError    = NEXUS_SUCCESS;
    eRet                          ret       = eRet_Ok;

    NEXUS_ComponentOutput_GetSettings(getOutput(), &settings);
    settings.mpaaDecimationEnabled = bMpaaDecimation;
    nError = NEXUS_ComponentOutput_SetSettings(getOutput(), &settings);
    CHECK_NEXUS_ERROR_GOTO("error setting component output settings", ret, nError, error);

error:
    return ret;
}

bool COutputComponent::isValidVideoFormat(NEXUS_VideoFormat format)
{
    bool bValid = false;

    switch (format)
    {
    case NEXUS_VideoFormat_e1080i:
    case NEXUS_VideoFormat_e720p:
    case NEXUS_VideoFormat_e576p:
    case NEXUS_VideoFormat_e480p:
    case NEXUS_VideoFormat_eNtsc:
    case NEXUS_VideoFormat_ePal:
        bValid = true;
        break;

    default:
        break;
    }

    return bValid;
}

COutputSVideo::COutputSVideo(const char * name, const uint16_t number, CConfiguration * pCfg) :
    CVideoOutput(name, number, eBoardResource_outputSVideo, pCfg)
{
}

COutputSVideo::~COutputSVideo()
{
}

eRet COutputSVideo::initialize()
{
    NEXUS_PlatformConfiguration platformConfig;
    eRet                        ret             = eRet_Ok;

    NEXUS_Platform_GetConfiguration(&platformConfig);
#if NEXUS_NUM_SVIDEO_OUTPUTS
    _outputSVideo = platformConfig.outputs.svideo[_number];
#else
    _outputSVideo=NULL;
    ret = eRet_NotSupported;
#endif

    return ret;
}

NEXUS_VideoOutput COutputSVideo::getConnector()
{
    BDBG_ASSERT(_outputSVideo);
    return NEXUS_SvideoOutput_GetConnector(_outputSVideo);
}

bool COutputSVideo::isValidVideoFormat(NEXUS_VideoFormat format)
{
    bool bValid = false;

    switch (format)
    {
    case NEXUS_VideoFormat_eNtsc:
    case NEXUS_VideoFormat_ePal:
        bValid = true;
        break;

    default:
        break;
    }

    return bValid;
}


COutputComposite::COutputComposite(const char * name, const uint16_t number, CConfiguration * pCfg) :
    CVideoOutput(name, number, eBoardResource_outputComposite, pCfg)
{
}

COutputComposite::~COutputComposite()
{
}

eRet COutputComposite::initialize()
{
    NEXUS_PlatformConfiguration platformConfig;
    eRet                        ret             = eRet_Ok;

    NEXUS_Platform_GetConfiguration(&platformConfig);
    _outputComposite = platformConfig.outputs.composite[_number];

    return ret;
}

NEXUS_VideoOutput COutputComposite::getConnector()
{
    BDBG_ASSERT(_outputComposite);
    return NEXUS_CompositeOutput_GetConnector(_outputComposite);
}

bool COutputComposite::isValidVideoFormat(NEXUS_VideoFormat format)
{
    bool bValid = false;

    switch (format)
    {
    case NEXUS_VideoFormat_eNtsc:
    case NEXUS_VideoFormat_ePal:
        bValid = true;
        break;

    default:
        break;
    }

    return bValid;
}


void hdmiHotplugCallback(void * context, int param)
{
    B_Event_Set(context);
}

void hdcpStatusCallback(void * context, int param)
{
    B_Event_Set(context);
}

void hdmiCecCallback(void * context, int param)
{
    /* dtt - change to event set and notification? */
    B_SchedulerCallback_Fire((B_SchedulerCallback *)context);
}

void hdmiHdcpRetryCallback(void *context)
{
    COutputHdmi * pOutputHdmi = (COutputHdmi *)context;

    BDBG_ASSERT(NULL != context);
    pOutputHdmi->startHdcpAuthentication();
    pOutputHdmi->clearHdcpTimer();
}

void hdmiHotplugHandler(void * context)
{
    COutputHdmi             * pOutputHdmi = (COutputHdmi *)context;
    NEXUS_HdmiOutputStatus    status;
    NEXUS_HdmiOutputHandle    hdmiOutput;

    BDBG_ASSERT(NULL != pOutputHdmi);

    NEXUS_HdmiOutput_GetStatus(pOutputHdmi->getOutput(), &status);
    BDBG_MSG(("HDMI Hotplug Event status %d connected %d", status.connected, pOutputHdmi->isConnected()));

    if (pOutputHdmi->isConnected() != status.connected)
    {
        pOutputHdmi->processHotplugStatus(&status);

        /* DTT - notify registered observers (display!) of hotplug event (see bsettop_dislay)
           display will have to remove the video output and call setsettings */
    }
}

void hdmiHdcpHandler(void * context)
{
    COutputHdmi                 * pOutputHdmi = (COutputHdmi *)context;
    NEXUS_HdmiOutputHdcpStatus    hdcpStatus;

    BDBG_ASSERT(NULL != context);

    if (false == pOutputHdmi->isHdcpPreferred())
    {
        return;
    }

    NEXUS_HdmiOutput_GetHdcpStatus(pOutputHdmi->getOutput(), &hdcpStatus);
    pOutputHdmi->processHdcpStatus(&hdcpStatus);

    /* DTT - notify registered observers (display!) of hdcp status change event (see bsettop_dislay)
       display has to handle the setting/clearing of bluescreen depending on hdcp status */
}

COutputHdmi::COutputHdmi(const char * name, const uint16_t number, CConfiguration * pCfg) :
    CVideoOutput(name, number, eBoardResource_outputHdmi, pCfg),
    _connected(false),
    _preferredVideoFormat(NEXUS_VideoFormat_eUnknown),
    _handleHdcpFailures(true),
    _hdcpFailureRetryDelay(0),
    _hdcpTimer(NULL)
{
}

COutputHdmi::~COutputHdmi()
{
}

bool COutputHdmi::isValidVideoFormat(NEXUS_VideoFormat format)
{
    NEXUS_Error            nerror   = NEXUS_SUCCESS;
    eRet                   ret      = eRet_Ok;
    NEXUS_HdmiOutputStatus status;

    nerror = NEXUS_HdmiOutput_GetStatus(getOutput(), &status);
    CHECK_NEXUS_ERROR_GOTO("unable to get HDMI status", ret, nerror, error);

    return status.videoFormatSupported[format];

error:
    BDBG_WRN(("error checking valid hdmi video format - assume false"));
    return false;
}

eRet COutputHdmi::processHotplugStatus(NEXUS_HdmiOutputStatus * pStatus)
{
    eRet        ret    = eRet_Ok;
    NEXUS_Error nError = NEXUS_SUCCESS;

    BDBG_ASSERT(NULL != pStatus);

    if (_connected == pStatus->connected)
    {
        /* duplicate hotplug status - ignore */
        return ret;
    }

    if (pStatus->connected)
    {
        setPreferredVideoFormat(pStatus->preferredVideoFormat);
    }
    else
    {
        if (true == _preferredHdcp)
        {
            nError = NEXUS_HdmiOutput_DisableHdcpAuthentication(_outputHdmi);
            ret = CHECK_NEXUS_ERROR("error disabling hdcp authentication", nError);
        }
    }

    /* DTT TODO: notify registered observers of hot plug status (display!) */
    BDBG_MSG((" HERE in %s \n",__FUNCTION__));

    _connected = pStatus->connected;

    return ret;
}

eRet COutputHdmi::processHdcpStatus(NEXUS_HdmiOutputHdcpStatus * pStatus)
{
    eRet                     ret                 = eRet_Ok;
    NEXUS_Error              nError              = NEXUS_SUCCESS;
    NEXUS_HdmiOutputSettings hdmiOutputSettings;

    BDBG_ASSERT(NULL != pStatus);

    NEXUS_HdmiOutput_GetSettings(_outputHdmi, &hdmiOutputSettings);

    switch(pStatus->hdcpState)
    {
    case NEXUS_HdmiOutputHdcpState_eUnauthenticated:
        if (true == _handleHdcpFailures)
        {
            NEXUS_HdmiOutput_GetSettings(_outputHdmi, &hdmiOutputSettings);
            hdmiOutputSettings.syncOnly = true;
            nError = NEXUS_HdmiOutput_SetSettings(_outputHdmi, &hdmiOutputSettings);
            CHECK_NEXUS_ERROR_ASSERT("error setting hdmi output settings", nError);

            muteAudio(true);
        }
        break;

    case NEXUS_HdmiOutputHdcpState_eR0LinkFailure:
    case NEXUS_HdmiOutputHdcpState_ePjLinkIntegrityFailure:
    case NEXUS_HdmiOutputHdcpState_eRiLinkIntegrityFailure:
    case NEXUS_HdmiOutputHdcpState_eRepeaterAuthenticationFailure:
        if (true == _handleHdcpFailures)
        {
            NEXUS_HdmiOutput_GetSettings(_outputHdmi, &hdmiOutputSettings);
            hdmiOutputSettings.syncOnly = true;
            nError = NEXUS_HdmiOutput_SetSettings(_outputHdmi, &hdmiOutputSettings);
            CHECK_NEXUS_ERROR_ASSERT("error setting hdmi output settings", nError);

            muteAudio(true);
        }

        /* Retry authorization if requested */
        if (0 < _hdcpFailureRetryDelay)
        {
            if (NULL != _hdcpTimer)
            {
                B_Scheduler_CancelTimer(gScheduler, _hdcpTimer);
                _hdcpTimer = NULL;
            }

            B_Scheduler_StartTimer(gScheduler, gLock, _hdcpFailureRetryDelay, hdmiHdcpRetryCallback, _outputHdmi);
        }
        break;

    case NEXUS_HdmiOutputHdcpState_eLinkAuthenticated:
    case NEXUS_HdmiOutputHdcpState_eEncryptionEnabled:
        /* dtt - shouldn't we always handle this case? */
        if (true == _handleHdcpFailures)
        {
            NEXUS_HdmiOutput_GetSettings(_outputHdmi, &hdmiOutputSettings);
            hdmiOutputSettings.syncOnly = false;
            nError = NEXUS_HdmiOutput_SetSettings(_outputHdmi, &hdmiOutputSettings);
            CHECK_NEXUS_ERROR_ASSERT("error setting hdmi output settings", nError);

            muteAudio(false);
        }
        break;

    default:
        break;
    }

    return ret;
}


eRet COutputHdmi::startHdcpAuthentication()
{
    eRet        ret     = eRet_Ok;
    NEXUS_Error nError  = NEXUS_SUCCESS;              

    if (false == _preferredHdcp)
    {
        return ret;
    }
    
    BDBG_WRN(("Starting HDCP authentication"));
    nError = NEXUS_HdmiOutput_StartHdcpAuthentication(_outputHdmi);
    ret = CHECK_NEXUS_ERROR("error starting hdcp authentication", nError);

    return ret;
}

eRet COutputHdmi::muteAudio(bool mute)
{
    eRet                      ret           = eRet_Ok;
    NEXUS_Error               nError        = NEXUS_SUCCESS;              
    NEXUS_AudioOutput         audioOutput;
    NEXUS_AudioOutputSettings audioSettings;

    audioOutput = NEXUS_HdmiOutput_GetAudioConnector(_outputHdmi);

    NEXUS_AudioOutput_GetSettings(audioOutput, &audioSettings);
    audioSettings.muted = mute;
    nError = NEXUS_AudioOutput_SetSettings(audioOutput, &audioSettings);
    CHECK_NEXUS_ERROR_ASSERT("error setting audio output settings", nError);

    return ret;
}

eRet COutputHdmi::initialize()
{
    eRet                           ret             = eRet_Ok;
    NEXUS_PlatformConfiguration    platformConfig;
    NEXUS_Error                    nError;
    NEXUS_HdmiOutputSettings       hdmiSettings;
    NEXUS_HdmiOutputHdcpSettings * pHdcpSettings   = NULL;

    NEXUS_Platform_GetConfiguration(&platformConfig);

    /* NEXUS_HdmiOutputHdcpSettings > 1024 bytes so we'll temporarily dynamically allocate it */
    pHdcpSettings = (NEXUS_HdmiOutputHdcpSettings *)B_Os_Malloc(sizeof(NEXUS_HdmiOutputHdcpSettings));
    if(!pHdcpSettings)
    {
        ret = BRUTUS_ERROR("Unable to malloc memory for HDCP settings!", eRet_OutOfMemory);
        BDBG_ASSERT(eRet_Ok != ret);
    }

    _outputHdmi = platformConfig.outputs.hdmi[_number];

    /* set HDMI handles here */
    _event     = B_Event_Create(NULL);
    _hdcpEvent = B_Event_Create(NULL);
    BDBG_ASSERT(NULL != _event);
    BDBG_ASSERT(NULL != _hdcpEvent);

    /* register handler functions which are called by scheduler when given event is set.
       nexus related callbacks will set these events when they fire */
    _eventId     = B_Scheduler_RegisterEvent(gScheduler, gLock, _event,     hdmiHotplugHandler, this);
    _hdcpEventId = B_Scheduler_RegisterEvent(gScheduler, gLock, _hdcpEvent, hdmiHdcpHandler,    this);

#if 0
    _cecEvent = B_Event_Create(NULL);
    BDBG_ASSERT(NULL != _cecEvent);
    _cecEventId  = b_event_register(_cecEvent,  hdmiCecHandler,     this);
#endif

    /* register nexus callbacks for hdmi events */
    NEXUS_HdmiOutput_GetSettings(_outputHdmi, &hdmiSettings);
    hdmiSettings.hotplugCallback.callback = hdmiHotplugCallback;
    hdmiSettings.hotplugCallback.context  = _event;
    hdmiSettings.cecCallback.callback     = hdmiCecCallback;
    /*hdmiSettings.cecCallback.context      = _cecEvent;*/
    #if 0 /* dtt - disabled for now...add back later */
    hdmiSettings.cecCallback.context      = _cecCallback;
    #endif
    nError = NEXUS_HdmiOutput_SetSettings(_outputHdmi, &hdmiSettings);
    CHECK_NEXUS_ERROR_ASSERT("error setting hdmi output settings", nError);

    /* register nexus callbacks for hdcp events */
    NEXUS_HdmiOutput_GetHdcpSettings(_outputHdmi, pHdcpSettings);
    pHdcpSettings->stateChangedCallback.callback = hdcpStatusCallback;
    pHdcpSettings->stateChangedCallback.context  = _hdcpEvent;
    pHdcpSettings->failureCallback.callback      = hdcpStatusCallback;
    pHdcpSettings->failureCallback.context       = _hdcpEvent;
    /* TO DO: Add successCallback */
    nError = NEXUS_HdmiOutput_SetHdcpSettings(_outputHdmi, pHdcpSettings);
    CHECK_NEXUS_ERROR_ASSERT("error setting hdcp output settings", nError);

    /* add audio delay if specified by brutus configuration option */
    if (0 < GET_INT(_pCfg, HDMI_OUTPUT_DELAY))
    {
        NEXUS_AudioOutputSettings nOutputSettings;
        NEXUS_AudioOutput_GetSettings(NEXUS_HdmiOutput_GetAudioConnector(_outputHdmi), &nOutputSettings);
        nOutputSettings.additionalDelay = GET_INT(_pCfg, HDMI_OUTPUT_DELAY);
        BDBG_MSG(("HDMI output delay %d",nOutputSettings.additionalDelay));
        nError = NEXUS_AudioOutput_SetSettings(NEXUS_HdmiOutput_GetAudioConnector(_outputHdmi), &nOutputSettings);
        CHECK_NEXUS_ERROR_ASSERT("error setting audio output settings", nError);
    }

    if(pHdcpSettings)
        B_Os_Free(pHdcpSettings);

    return ret;
}


NEXUS_VideoOutput COutputHdmi::getConnector()
{
    BDBG_ASSERT(_outputHdmi);
    return NEXUS_HdmiOutput_GetVideoConnector(_outputHdmi);
}

NEXUS_AudioOutput COutputHdmi::getAudioConnector()
{
    BDBG_ASSERT(_outputHdmi);
    return NEXUS_HdmiOutput_GetAudioConnector(_outputHdmi);
}

eRet COutputHdmi::setColorSpace(NEXUS_ColorSpace colorSpace)
{
    eRet                     ret        = eRet_Ok;
    NEXUS_Error              nError     = NEXUS_SUCCESS;
    NEXUS_HdmiOutputSettings settings;

    BDBG_ASSERT(NEXUS_ColorSpace_eMax > colorSpace);

    NEXUS_HdmiOutput_GetSettings(getOutput(), &settings);
    settings.autoColorSpace = false;
    settings.colorSpace     = colorSpace;
    nError = NEXUS_HdmiOutput_SetSettings(getOutput(), &settings);
    CHECK_NEXUS_ERROR_GOTO("error setting hdmi color space settings", ret, nError, error);

error:
    return ret;
}

eRet COutputHdmi::connect(NEXUS_AudioInput input)
{
    eRet        ret    = eRet_Ok;
    NEXUS_Error nerror = NEXUS_SUCCESS;

    BDBG_ASSERT(NULL != input);

    nerror = NEXUS_AudioOutput_AddInput(getAudioConnector(), input);
    CHECK_NEXUS_ERROR_GOTO("error adding input to hdmi output", ret, nerror, error);

error:
    return ret;
}




CAudioOutput::CAudioOutput(const char * name, const uint16_t number, eBoardResource type, CConfiguration * pCfg) :
    CResource(name, number, type, pCfg)
{
}

CAudioOutput::~CAudioOutput()
{
}

eRet CAudioOutput::connect(NEXUS_AudioInput input)
{
    eRet        ret    = eRet_Ok;
    NEXUS_Error nerror = NEXUS_SUCCESS;

    BDBG_ASSERT(NULL != input);

    nerror = NEXUS_AudioOutput_AddInput(getConnector(), input);
    CHECK_NEXUS_ERROR_GOTO("error adding input to audio output", ret, nerror, error);

error:
    return ret;
}

eRet CAudioOutput::disconnect()
{
    eRet        ret    = eRet_Ok;
    NEXUS_Error nerror = NEXUS_SUCCESS;

    nerror = NEXUS_AudioOutput_RemoveAllInputs(getConnector());
    CHECK_NEXUS_ERROR_GOTO("error removing all inputs from audio output", ret, nerror, error);

error:
    return ret;
}

COutputSpdif::COutputSpdif(const char * name, const uint16_t number, CConfiguration * pCfg) :
    CAudioOutput(name, number, eBoardResource_outputSpdif, pCfg)
{
}

COutputSpdif::~COutputSpdif()
{
}

eRet COutputSpdif::initialize()
{
    NEXUS_PlatformConfiguration platformConfig;
    eRet                        ret             = eRet_Ok;
    NEXUS_Error                 nError          = NEXUS_SUCCESS;

    NEXUS_Platform_GetConfiguration(&platformConfig);
    if (NULL != platformConfig.outputs.spdif[_number])
    {
        int outputDelay = 0;

        _outputSpdif = platformConfig.outputs.spdif[_number];
        outputDelay = GET_INT(_pCfg, SPDIF_OUTPUT_DELAY);
        if (0 < outputDelay)
        {
            NEXUS_AudioOutputSettings outputSettings;
            NEXUS_AudioOutput_GetSettings(NEXUS_SpdifOutput_GetConnector(_outputSpdif), &outputSettings);
            outputSettings.additionalDelay = outputDelay;
            nError = NEXUS_AudioOutput_SetSettings(NEXUS_SpdifOutput_GetConnector(_outputSpdif), &outputSettings);
            CHECK_NEXUS_ERROR_ASSERT("error setting audio out settings", nError);
        }
    }

    return ret;
}

NEXUS_AudioOutput COutputSpdif::getConnector()
{
    BDBG_ASSERT(_outputSpdif);
    return NEXUS_SpdifOutput_GetConnector(_outputSpdif);
}

COutputAudioDac::COutputAudioDac(const char * name, const uint16_t number, CConfiguration * pCfg) :
    CAudioOutput(name, number, eBoardResource_outputAudioDac, pCfg)
{
}

COutputAudioDac::~COutputAudioDac()
{
    disconnect();
}

eRet COutputAudioDac::initialize()
{
    NEXUS_PlatformConfiguration platformConfig;
    eRet                        ret             = eRet_Ok;
    NEXUS_Error                 nError          = NEXUS_SUCCESS;

    NEXUS_Platform_GetConfiguration(&platformConfig);
    if (NULL != platformConfig.outputs.audioDacs[_number])
    {
        int outputDelay = 0;

        _outputAudioDac = platformConfig.outputs.audioDacs[_number];
        outputDelay = GET_INT(_pCfg, DAC_OUTPUT_DELAY);
        if (0 < outputDelay)
        {
            NEXUS_AudioOutputSettings outputSettings;
            NEXUS_AudioOutput_GetSettings(NEXUS_AudioDac_GetConnector(_outputAudioDac), &outputSettings);
            outputSettings.additionalDelay = outputDelay;
            nError = NEXUS_AudioOutput_SetSettings(NEXUS_AudioDac_GetConnector(_outputAudioDac), &outputSettings);
            CHECK_NEXUS_ERROR_ASSERT("error setting audio output settings", nError);
        }
    }
    
    return ret;
}

NEXUS_AudioOutput COutputAudioDac::getConnector()
{
    BDBG_ASSERT(_outputAudioDac);
    return NEXUS_AudioDac_GetConnector(_outputAudioDac);
}
