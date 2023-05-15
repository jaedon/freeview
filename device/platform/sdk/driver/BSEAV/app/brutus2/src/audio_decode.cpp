/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: audio_decode.cpp $
* $brcm_Revision: 11 $
* $brcm_Date: 10/31/12 3:18p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/audio_decode.cpp $
* 
* 11   10/31/12 3:18p jrubio
* SW7346-1071: fix usage of audio decoder when in PCM vs Compressed mode
* 
* 10   10/10/12 5:31p tokushig
* SW7231-749: adjust debug output for release
* 
* 9   9/29/12 9:20p tokushig
* SW7231-749: connect audio settings window panel to simple audio
* decoder.  pid, spdif, hdmi, downmix, dualmono, dolby drc, and dialog
* norm are implemented.
* 
* 8   9/26/12 4:06p tokushig
* SW7231-749: add ability to set spdif/hdmi output type
* 
* 7   9/21/12 4:11p tokushig
* SW7231-749: fix close/stop in audio decode
* 
* 6   9/17/12 4:00p tokushig
* SW7231-749: update audio decode to setup source changed callback after
* the open instead of after the start.  this is dependent on
* nexus_simple_audio_decoder.c@@/main/32 or later
* 
* 5   9/10/12 1:56p tokushig
* SW7231-749: add simple audio decoder class.  it is subclassed from
* regular audio decoder class.  it manages it's own resources similar to
* how nexus simple audio decoder does.
* 
* 4   5/25/12 3:12p tokushig
* SW7231-749: remove platform specific file and move to common
* platform.h.  also change appropriate brutus defines to nexus defines
* when available.
* 
* 3   4/15/12 9:41p tokushig
* SW7231-749: added audio source changed callback support
* 
* 2   2/28/12 5:02p tokushig
* SW7405-5581: add header comments
* 
* 
*********************************************************************************/

#include "nexus_audio_decoder.h"
#include "audio_decode.h"
#include "brutus_os.h"

BDBG_MODULE(brutus_audio_decode);


/* bwin io callback that is triggered when it is safe to handle callbacks */
static void bwinAudioDecodeCallback(bwidget_engine_t engine, bwin_io_handle handle)
{
    eRet           ret          = eRet_Ok;
    CAudioDecode * pAudioDecode = (CAudioDecode *)handle;

    BDBG_ASSERT(NULL != pAudioDecode);

    if (true == pAudioDecode->isSourceChanged())
    {
        BDBG_MSG(("Notify Observers for audio decoder event code: %#x", eNotify_AudioSourceChanged));
        ret = pAudioDecode->notifyObservers(eNotify_AudioSourceChanged, pAudioDecode);
        CHECK_ERROR_GOTO("error notifying observers", ret, error);
    }

error:
    return;
}

static void NexusAudioDecodeSourceChangedCallback(void * context, int param)
{
    CAudioDecode * pAudioDecode = (CAudioDecode *)context;
    BSTD_UNUSED(param);
    BDBG_ASSERT(NULL != pAudioDecode);

    pAudioDecode->setSourceChanged(true);

    /* sync with bwin loop */
    bwidget_trigger_io(pAudioDecode->getWidgetEngine(), (bwin_io_handle)pAudioDecode);
}

CAudioDecode::CAudioDecode(const char * name, const uint16_t number, CConfiguration * pCfg) :
    CResource(name, number, eBoardResource_decodeAudio, pCfg),
    _decoder(NULL),
    _sourceChanged(false),
    _started(false)
{
    eRet ret = eRet_Ok;
    BDBG_ASSERT(NEXUS_NUM_AUDIO_DECODERS > _number);

error:
    BDBG_ASSERT(eRet_Ok == ret);
}

CAudioDecode::~CAudioDecode()
{
    if (_sourceChangedCallbackHandle)
    {
        B_SchedulerCallback_Destroy(_sourceChangedCallbackHandle);
        _sourceChangedCallbackHandle = NULL;
    }
}

eRet CAudioDecode::open(bwidget_engine_t widgetEngine, CStc * pStc)
{
    eRet                           ret       = eRet_Ok;
    int                            fifoSize  = 0;
    NEXUS_AudioDecoderOpenSettings settings;

    BDBG_ASSERT(NULL != pStc);

    if (true == isOpened())
    {
        ret = eRet_InvalidState;
        CHECK_ERROR_GOTO("Audio decoder is already opened.", ret, error);
    }

    _widgetEngine = widgetEngine;
    bwidget_add_io_handle(_widgetEngine, (bwin_io_handle)this, bwinAudioDecodeCallback);

    /* save stc for start() */
    _pStc = pStc;

    NEXUS_AudioDecoder_GetDefaultOpenSettings(&settings);

    {
        int multiChFormat = GET_INT(_pCfg, AUDIO_DECODER_MULTICH_FORMAT);
        if ((0 < multiChFormat) &&  (NEXUS_AudioMultichannelFormat_eMax > multiChFormat))
        {
            settings.multichannelFormat = (NEXUS_AudioMultichannelFormat)multiChFormat;
        }
    }

    /* set up optional independent output delay */
    if ((0 < GET_INT(_pCfg, SPDIF_OUTPUT_DELAY)) ||
        (0 < GET_INT(_pCfg, HDMI_OUTPUT_DELAY))  ||
        (0 < GET_INT(_pCfg, DAC_OUTPUT_DELAY)))
    {
        settings.independentDelay = true;
    }

    /* change fifo size if specified */
    fifoSize = GET_INT(_pCfg, AUDIO_DECODER_FIFO_SIZE);
    if (0 < fifoSize)
    {
        settings.fifoSize = fifoSize * 1000; /* dtt - should this be 1024? */
        BDBG_WRN(("****************************************"));
        BDBG_WRN(("* Changing Audio FIFO size to %d bytes *", settings.fifoSize));
        BDBG_WRN(("****************************************"));
    }

    _decoder = NEXUS_AudioDecoder_Open(_number, &settings);
    CHECK_PTR_GOTO("Nexus audio decoder open failed!", _decoder, ret, eRet_NotAvailable, error);

    /* set audio decoder settings */
    {
        NEXUS_Error                 nError                = NEXUS_SUCCESS;
        NEXUS_AudioDecoderSettings  audioDecoderSettings;


        NEXUS_AudioDecoder_GetSettings(_decoder, &audioDecoderSettings);
        audioDecoderSettings.sourceChanged.callback = NexusAudioDecodeSourceChangedCallback;
        audioDecoderSettings.sourceChanged.context  = this;
        audioDecoderSettings.sourceChanged.param    = 0;
        nError = NEXUS_AudioDecoder_SetSettings(_decoder, &audioDecoderSettings);
        CHECK_NEXUS_ERROR_GOTO("Error applying audio decoder settings.", ret, nError, error);
    }

#if B_HAS_DTS_ENCODE
    _dtsEncoder = NEXUS_DtsEncode_Open(NULL);
    CHECK_PTR_GOTO("Dts encode open failed.", _dtsEncoder, ret, eRet_NotAvailable, error);
#endif
#if B_HAS_AC3_ENCODE
    _ac3Encoder = NEXUS_Ac3Encode_Open(NULL);
    CHECK_PTR_GOTO("AC3 encode open failed.", _ac3Encoder, ret, eRet_NotAvailable, error);
#endif
#if B_HAS_SRS_TRUE_VOLUME
    {
        NEXUS_TruVolumeSettings trueVolSettings;
        NEXUS_TruVolume_GetDefaultSettings(&trueVolSettings);
        trueVolSettings.enabled = false;
        /* 
        trueVolSettings.enabled = audio_decode->decode->settings.audio.postproc.srs.tru_volume_enable;
        BDBG_WRN(("true volume %s",audio_decode->decode->settings.audio.postproc.srs.tru_volume_enable?"on":"off")); 
        */ 
        _trueVolume = NEXUS_TruVolume_Open(&trueVolSettings);
        CHECK_PTR_GOTO("TruVolume open failed.", _trueVolume, ret, eRet_NotAvailable, error);
    }
#endif
#if B_HAS_DOLBY_VOLUME
    {
        NEXUS_DolbyVolumeSettings dolbyVolSettings;
        NEXUS_DolbyVolume_GetDefaultSettings(&dolbyVolSettings);
        dolbyVolSettings.enabled = false;
        /*
        dolbyVolSettings.enabled = audio_decode->decode->settings.audio.postproc.dolby.volume_proc_enable;
        BDBG_WRN(("dolby volume %s",audio_decode->decode->settings.audio.postproc.dolby.volume_proc_enable?"on":"off")); 
        */ 
        _dolbyVolume = NEXUS_DolbyVolume_Open(&dolbyVolSettings);
        CHECK_PTR_GOTO("Dolby volume open failed.", _dolbyVolume, ret, eRet_NotAvailable, error);
    }
#endif

    if (GET_BOOL(_pCfg, AUDIO_CAPTURE) || GET_BOOL(_pCfg, AUDIO_CAPTURE_COMPRESSED))
    {
        /* dtt - open audio capture */
    }

#if B_HAS_EXTERNAL_ANALOG
    _i2sInput = NEXUS_I2sInput_Open(0, NULL);
    CHECK_PTR_GOTO("i2s input open failed.", _i2sInput, ret, eRet_NotAvailable, error);
#endif

    /* dtt - open passthru? see bsettop_decode.c */
error:
    return ret;
}

#include "nexus_audio_input.h"

CStc * CAudioDecode::close()
{
    CStc * pStc = NULL;

    if (GET_BOOL(_pCfg, AUDIO_CAPTURE) || GET_BOOL(_pCfg, AUDIO_CAPTURE_COMPRESSED))
    {
        /* dtt - close audio capture */
    }

    if (_decoder)
    {
        NEXUS_AudioInput_Shutdown(NEXUS_AudioDecoder_GetConnector(_decoder, NEXUS_AudioDecoderConnectorType_eStereo));
        NEXUS_AudioInput_Shutdown(NEXUS_AudioDecoder_GetConnector(_decoder, NEXUS_AudioDecoderConnectorType_eCompressed));
        NEXUS_AudioInput_Shutdown(NEXUS_AudioDecoder_GetConnector(_decoder, NEXUS_AudioDecoderConnectorType_eMultichannel));

        NEXUS_AudioDecoder_Close(_decoder);
        /* return stc */
        pStc     = _pStc;
        _pStc    = NULL;
        _decoder = NULL;
    }

#if B_HAS_DTS_ENCODE
    if (_dtsEncoder) {
        NEXUS_DtsEncode_RemoveAllInputs(_dtsEncoder);
        NEXUS_AudioInput_Shutdown(NEXUS_DtsEncode_GetConnector(_dtsEncoder));
        NEXUS_DtsEncode_Close(_dtsEncoder);
        _dtsEncoder=NULL;
    }
#endif
#if B_HAS_AC3_ENCODE
    if (_ac3Encoder) {
        NEXUS_Ac3Encode_RemoveAllInputs(_ac3Encoder);
        NEXUS_AudioInput_Shutdown(NEXUS_Ac3Encode_GetConnector(_ac3Encoder));
        NEXUS_Ac3Encode_Close(_ac3Encoder);
        _ac3Encoder=NULL;
    }
#endif
#if B_HAS_SRS_TRUE_VOLUME
    if (_trueVolume) {
        NEXUS_TruVolume_RemoveAllInputs(_trueVolume);
        NEXUS_AudioInput_Shutdown(NEXUS_TruVolume_GetConnector(_trueVolume));
        NEXUS_TruVolume_Close(_trueVolume);
        _trueVolume=NULL;
    }
#endif
#if B_HAS_DOLBY_VOLUME
    if (_dolbyVolume) {
        NEXUS_DolbyVolume_RemoveAllInputs(_dolbyVolume);
        NEXUS_AudioInput_Shutdown(NEXUS_DolbyVolume_GetConnector(_dolbyVolume));
        NEXUS_DolbyVolume_Close(_dolbyVolume);
        _dolbyVolume=NULL;
    }
#endif
#if B_HAS_EXTERNAL_ANALOG
    if (_i2sInput )
    {
        NEXUS_AudioInput_Shutdown(NEXUS_I2sInput_GetConnector(_i2sInput));
        NEXUS_I2sInput_Close(_i2sInput);
        _i2sInput = NULL;
    }
#endif

    /* dtt - close passthru? see bsettop_decode.c */

    return pStc;
}


eRet CAudioDecode::start(CPid * pPid)
{
    eRet                            ret       = eRet_Ok;
    NEXUS_Error                     nerror    = NEXUS_SUCCESS;
    NEXUS_AudioDecoderStartSettings settings;

    BDBG_ASSERT((NULL != pPid) || (NULL != getPid()));
    BDBG_ASSERT(NULL != _pStc);
    BDBG_ASSERT(true == isOpened());

    NEXUS_AudioDecoder_GetDefaultStartSettings(&settings);
    settings.codec      = pPid->getAudioCodec();
    settings.pidChannel = pPid->getPidChannel();
    settings.stcChannel = _pStc->getStcChannel();

    /* setup source changed callback? */
    /* update sync channel and or astm?  or maybe this goes in the open */
    /* set downmix */
    /* start optional audio capture if necessary */
    /* codec object call to enable 96khz mode (only applies to aac) */

    nerror = NEXUS_AudioDecoder_Start(_decoder, &settings);
    CHECK_NEXUS_ERROR_GOTO("audio decoder failed to start", ret, nerror, error);

    /* save pid */
    setPid(pPid);
    _started = true;

    notifyObservers(eNotify_AudioDecodeStarted, this);
error:
    return ret;
}

CPid * CAudioDecode::stop()
{
    eRet   ret  = eRet_Ok;
    CPid * pPid = NULL;

    BDBG_ASSERT(NULL != _decoder);

    if (false == isStarted())
    {
        BDBG_WRN(("Attempting to stop audio decode that is already stopped."));
        goto done;
    }

    NEXUS_AudioDecoder_Stop(_decoder);

    /* return pid */
    pPid     = getPid();
    _started = false;

    notifyObservers(eNotify_AudioDecodeStopped, this);
done:
    return pPid;
}

NEXUS_AudioInput CAudioDecode::getConnector(NEXUS_AudioConnectorType type)
{
    NEXUS_AudioInput nexusInput = NULL;

    BDBG_ASSERT(NEXUS_AudioConnectorType_eMax > type);

    if (NULL != _decoder)
    {
        nexusInput = NEXUS_AudioDecoder_GetConnector(getDecoder(), type);
    }

    return nexusInput;
}

eRet CAudioDecode::getStatus(NEXUS_AudioDecoderStatus * pStatus)
{
    eRet        ret    = eRet_Ok;
    NEXUS_Error nerror = NEXUS_SUCCESS;
    BDBG_ASSERT(NULL != pStatus);

    nerror = NEXUS_AudioDecoder_GetStatus(getDecoder(), pStatus);
    CHECK_NEXUS_ERROR_GOTO("error getting audio decoder status", ret, nerror, error);

error:
    return ret;
}



/*** simple audio decoder class */

#include "nexus_simple_audio_decoder_server.h"
#include "brutus_board.h"
#define BRUTUS_NUM_SIMPLE_AUDIO_DECODERS 1

CSimpleAudioDecode::CSimpleAudioDecode(const char * name, const uint16_t number, CConfiguration * pCfg) :
    CAudioDecode(name, number, pCfg),
    _simpleDecoder(NULL),
    _pBoardResources(NULL),
    _pSpdif(NULL),
    _pHdmi(NULL),
    _numSpdif(0),
    _numHdmi(0),
    _spdifInput(eSpdifInput_Pcm),
    _hdmiInput(eHdmiAudioInput_Pcm),
    _downmix(eAudioDownmix_None),
    _dualMono(eAudioDualMono_Left),
    _dolbyDRC(eDolbyDRC_None),
    _dolbyDialogNorm(false),
    _resourceId(0)
{
    eRet ret = eRet_Ok;
    BDBG_ASSERT(BRUTUS_NUM_SIMPLE_AUDIO_DECODERS > _number);

    /* manually set type since base class CAudioDecoder constructor will default to
       regular audio decoder type */
    setType(eBoardResource_simpleDecodeAudio);

    for (int i = 0; i < 2; i++)
    {
        _pDecoders[i] = NULL;
    }

error:
    BDBG_ASSERT(eRet_Ok == ret);
}


CSimpleAudioDecode::~CSimpleAudioDecode()
{
    stop();
    close();
}

eRet CSimpleAudioDecode::open(bwidget_engine_t widgetEngine, CStc * pStc)
{
    eRet                                   ret       = eRet_Ok;
    int                                    i         = 0;
    NEXUS_SimpleAudioDecoderServerSettings settings;

    BDBG_ASSERT(NULL != pStc);

    if (true == isOpened())
    {
        ret = eRet_InvalidState;
        CHECK_ERROR_GOTO("Simple audio decoder is already opened.", ret, error);
    }

    if (NULL == _pBoardResources)
    {
        ret = eRet_InvalidState;
        CHECK_ERROR_GOTO("Set board resources before opening.", ret, error);
    }

    _widgetEngine = widgetEngine;
    bwidget_add_io_handle(_widgetEngine, (bwin_io_handle)this, bwinAudioDecodeCallback);

    /* save stc for start() */
    _pStc = pStc;

    for (i = 0; i < 2; i++)
    {
        _pDecoders[i] = (CAudioDecode *)_pBoardResources->checkoutResource(_resourceId, eBoardResource_decodeAudio);
        if (NULL != _pDecoders[i])
        {
            ret = _pDecoders[i]->open(_widgetEngine, pStc);
            CHECK_ERROR_GOTO("Audio decode failed to open", ret, error);
        }
    }

    NEXUS_SimpleAudioDecoder_GetDefaultServerSettings(&settings);
    settings.primary   = _pDecoders[0]->getDecoder();
    settings.secondary = _pDecoders[1]->getDecoder();

    /* default spdif to stereo output */
    setSpdifInputDecoderType(eSpdifInput_Pcm, &settings);

    _pSpdif = (COutputSpdif *)_pBoardResources->checkoutResource(_resourceId, eBoardResource_outputSpdif);
    if (NULL != _pSpdif)
    {
        /* assign spdif outputs for simple audio decoder to use */
        settings.spdif.outputs[0] = _pSpdif->getOutput();
    }

    /* default hdmi to stereo output */
    setHdmiInputDecoderType(eHdmiAudioInput_Pcm, &settings);

    /* if a video hdmi resource has been assigned, set up audio output */
    if (NULL != _pHdmi)
    {
        /* assign Hdmi outputs for simple audio decoder to use */
        settings.hdmi.outputs[0] = _pHdmi->getOutput();
    }

    _simpleDecoder = NEXUS_SimpleAudioDecoder_Create(0, &settings);
    CHECK_PTR_GOTO("unable to create a simple audio decoder", _simpleDecoder, ret, eRet_OutOfMemory, error);

    /* set audio decoder settings */
    {
        NEXUS_Error                       nError                = NEXUS_SUCCESS;
        NEXUS_SimpleAudioDecoderSettings  audioDecoderSettings;

        NEXUS_SimpleAudioDecoder_GetSettings(_simpleDecoder, &audioDecoderSettings);
        audioDecoderSettings.primary.sourceChanged.callback = NexusAudioDecodeSourceChangedCallback;
        audioDecoderSettings.primary.sourceChanged.context  = this;
        audioDecoderSettings.primary.sourceChanged.param    = 0;
        audioDecoderSettings.secondary.sourceChanged.callback = NexusAudioDecodeSourceChangedCallback;
        audioDecoderSettings.secondary.sourceChanged.context  = this;
        audioDecoderSettings.secondary.sourceChanged.param    = 0;
        nError = NEXUS_SimpleAudioDecoder_SetSettings(_simpleDecoder, &audioDecoderSettings);
        CHECK_NEXUS_ERROR_GOTO("Error applying simple audio decoder settings.", ret, nError, error);
    }
error:
    return ret;
}

void CSimpleAudioDecode::getSettings(NEXUS_SimpleAudioDecoderServerSettings * pSettings)
{
    BDBG_ASSERT(NULL != _simpleDecoder);

    NEXUS_SimpleAudioDecoder_GetServerSettings(_simpleDecoder, pSettings);
}

eRet CSimpleAudioDecode::setSettings(NEXUS_SimpleAudioDecoderServerSettings * pSettings)
{
    NEXUS_Error nerror = NEXUS_SUCCESS;
    eRet        ret    = eRet_Ok;
    BDBG_ASSERT(NULL != _simpleDecoder);

    nerror = NEXUS_SimpleAudioDecoder_SetServerSettings(_simpleDecoder, pSettings);
    CHECK_NEXUS_ERROR_GOTO("unable to set simple audio decoder server settings", ret, nerror, error);

error:
    return ret;
}

/* if pSettings == NULL, we will get the settings from nexus and set them after making changes.
   if pSettings != NULL, we will fill in the given pSettings - changes will not be applies in nexus */
eRet CSimpleAudioDecode::setSpdifInputDecoderType(eSpdifInput spdifInput,
                                                  NEXUS_SimpleAudioDecoderServerSettings * pSettings)
{
    NEXUS_AudioInput                         input;
    NEXUS_SimpleAudioDecoderServerSettings   settings;
    NEXUS_SimpleAudioDecoderServerSettings * pUseSettings;
    NEXUS_AudioConnectorType                 spdifType; 
    int                                      i         = 0;
    int                                      decoder   = 0;
    eRet                                     ret       = eRet_Ok;

    BDBG_MSG(("setSpdifInputDecoderType:%d", spdifInput));

    if (NULL == pSettings)
    {
        /* use settings from nexus */
        getSettings(&settings);
        pUseSettings = &settings;
    }
    else
    {
        /* use given setting structure */
        pUseSettings = pSettings;
    }

    switch (spdifInput)
    {
    case eSpdifInput_EncodeDts:
    case eSpdifInput_EncodeAc3:
        BDBG_WRN(("encode AC-3/DTS feature currently unsupported - default to compressed"));
        spdifInput = eSpdifInput_Compressed;
        /* fallthru for now */
    case eSpdifInput_Compressed:
        spdifType = NEXUS_AudioConnectorType_eCompressed;
        decoder = 1;
        break;
    case eSpdifInput_Pcm:
    default:
        spdifType = NEXUS_AudioConnectorType_eStereo;
        decoder = 0;
        break;
    }

    for (i = 0; i < NEXUS_AudioCodec_eMax; i++) 
    {
        switch (i) 
        {
        case NEXUS_AudioCodec_eAac:
        case NEXUS_AudioCodec_eAacLoas:
        case NEXUS_AudioCodec_eAacPlus:
        case NEXUS_AudioCodec_eAacPlusAdts:
        case NEXUS_AudioCodec_eAc3:
        case NEXUS_AudioCodec_eDts:
        case NEXUS_AudioCodec_eLpcmDvd:
        case NEXUS_AudioCodec_eLpcmHdDvd:
        case NEXUS_AudioCodec_eLpcmBluRay:
        case NEXUS_AudioCodec_eDtsHd:
        case NEXUS_AudioCodec_eWmaStd:
        case NEXUS_AudioCodec_eWmaStdTs:
        case NEXUS_AudioCodec_eWmaPro:
        case NEXUS_AudioCodec_eDtsLegacy:
        case NEXUS_AudioCodec_eMlp:
            pUseSettings->spdif.input[i] = _pDecoders[decoder]->getConnector(spdifType);
            break;
        case NEXUS_AudioCodec_eAc3Plus: /* Always set this to Decoder 0 no matter what */
           pUseSettings->spdif.input[i] = _pDecoders[0]->getConnector(spdifType);
           break;
        case NEXUS_AudioCodec_eUnknown:
        default:
            pUseSettings->spdif.input[i] = _pDecoders[0]->getConnector(NEXUS_AudioConnectorType_eStereo);
            break;
        }    
    }

    if (NULL == pSettings)
    {
        /* no settings structure given so set in nexus */
        ret = setSettings(&settings);
        CHECK_ERROR_GOTO("set simple audio settings failed", ret, error);

        _spdifInput = spdifInput;
    }

error:
    return ret;
}

/* if pSettings == NULL, we will get the settings from nexus and set them after making changes.
   if pSettings != NULL, we will fill in the given pSettings - changes will not be applies in nexus */
eRet CSimpleAudioDecode::setHdmiInputDecoderType(eHdmiAudioInput hdmiInput,
                                                 NEXUS_SimpleAudioDecoderServerSettings * pSettings)
{
    NEXUS_AudioInput                         input;
    NEXUS_SimpleAudioDecoderServerSettings   settings;
    NEXUS_SimpleAudioDecoderServerSettings * pUseSettings   = NULL;
    NEXUS_AudioConnectorType                 hdmiType;
    int                                      i              = 0;
    eRet                                     ret            = eRet_Ok;
    int                                      decoder        = 0;
    NEXUS_HdmiOutputStatus                   hdmiStatus;
    

    BDBG_MSG(("setHdmiInputDecoderType:%d", hdmiInput));

    if (NULL == pSettings)
    {
        /* use settings from nexus */
        getSettings(&settings);
        pUseSettings = &settings;
    }
    else
    {
        /* use given setting structure */
        pUseSettings = pSettings;
    }

    

    switch (hdmiInput)
    {
    case eHdmiAudioInput_Multichannel:
        hdmiType = NEXUS_AudioConnectorType_eMultichannel;
        break;
    case eHdmiAudioInput_EncodeDts:
    case eHdmiAudioInput_EncodeAc3:
        BDBG_WRN(("encode AC-3/DTS feature currently unsupported - default to compressed"));
        hdmiInput = eHdmiAudioInput_Compressed;
        /* fallthru for now */
    case eHdmiAudioInput_Compressed:
        hdmiType = NEXUS_AudioConnectorType_eCompressed;
        decoder=1;
        break;
    case eHdmiAudioInput_Pcm:
    default:
        hdmiType = NEXUS_AudioConnectorType_eStereo;
        decoder =0;
        break;
    }

    for (i = 0; i < NEXUS_AudioCodec_eMax; i++) 
    {
        switch (i) 
        {
        case NEXUS_AudioCodec_eAac:
        case NEXUS_AudioCodec_eAacLoas:
        case NEXUS_AudioCodec_eAacPlus:
        case NEXUS_AudioCodec_eAacPlusAdts:
        case NEXUS_AudioCodec_eAc3:
        case NEXUS_AudioCodec_eDts:
        case NEXUS_AudioCodec_eLpcmDvd:
        case NEXUS_AudioCodec_eLpcmHdDvd:
        case NEXUS_AudioCodec_eLpcmBluRay:
        case NEXUS_AudioCodec_eDtsHd:
        case NEXUS_AudioCodec_eWmaStd:
        case NEXUS_AudioCodec_eWmaStdTs:
        case NEXUS_AudioCodec_eWmaPro:
        case NEXUS_AudioCodec_eDtsLegacy:
           /* TODO fix AC3 Plus case if Edid does not support */
        case NEXUS_AudioCodec_eAc3Plus:
        case NEXUS_AudioCodec_eMlp:
            pUseSettings->hdmi.input[i] = _pDecoders[decoder]->getConnector(hdmiType);
            break;
        case NEXUS_AudioCodec_eUnknown:
        default:
            pUseSettings->hdmi.input[i] = _pDecoders[0]->getConnector(NEXUS_AudioConnectorType_eStereo);
            break;
        }    
    }

    if (NULL == pSettings)
    {
        /* no settings structure given so set in nexus */
        ret = setSettings(&settings);
        CHECK_ERROR_GOTO("set simple audio settings failed", ret, error);

        _hdmiInput = hdmiInput;
    }

error:
    return ret;
}


CStc * CSimpleAudioDecode::close()
{
    CStc * pStc = NULL;

    if (NULL == _simpleDecoder)
    {
        NEXUS_SimpleAudioDecoder_Destroy(_simpleDecoder);
        /* return stc */
        pStc           = _pStc;
        _pStc          = NULL;
        _simpleDecoder = NULL;

        for (int i = 0; i < 2; i++)
        {
            if (NULL != _pDecoders[i])
            {
                _pBoardResources->checkinResource(_pDecoders[i]);
                _pDecoders[i] = NULL;
            }
        }

        if (NULL != _pSpdif)
        {
            _pBoardResources->checkinResource(_pSpdif);
            _pSpdif = NULL;
        }

        if (NULL != _pHdmi)
        {
            /* do not check in hdmi output.  if it exists it belongs to the video decoder */
            _pHdmi = NULL;
        }
    }

    return pStc;
}

eRet CSimpleAudioDecode::start(CPid * pPid)
{
    eRet                                  ret       = eRet_Ok;
    NEXUS_Error                           nerror    = NEXUS_SUCCESS;
    NEXUS_SimpleAudioDecoderStartSettings settings;

    BDBG_ASSERT((NULL != pPid) || (NULL != getPid()));
    BDBG_ASSERT(NULL != _pStc);
    BDBG_ASSERT(true == isOpened());

    if (NULL == pPid)
    {
        /* if no pid given use saved pid */
        pPid = getPid();
    }

    NEXUS_SimpleAudioDecoder_GetDefaultStartSettings(&settings);
    settings.primary.codec        = pPid->getAudioCodec();
    settings.primary.pidChannel   = pPid->getPidChannel();
    settings.primary.stcChannel   = _pStc->getStcChannel();
    settings.secondary.codec      = pPid->getAudioCodec();
    settings.secondary.pidChannel = pPid->getPidChannel();
    settings.secondary.stcChannel = _pStc->getStcChannel();

    /* set downmix */
    /* start optional audio capture if necessary */
    /* codec object call to enable 96khz mode (only applies to aac) */

    nerror = NEXUS_SimpleAudioDecoder_Start(_simpleDecoder, &settings);
    CHECK_NEXUS_ERROR_GOTO("simple audio decoder failed to start", ret, nerror, error);

    /* save pid - also propogate to simple decoder's internal decoder objects */
    setPid(pPid);
    _started = true;

    notifyObservers(eNotify_AudioDecodeStarted, this);
error:
    return ret;
}

CPid * CSimpleAudioDecode::stop()
{
    CPid * pPid = NULL;
    eRet   ret  = eRet_Ok;

    BDBG_ASSERT(NULL != _simpleDecoder);

    if (false == isStarted())
    {
        BDBG_WRN(("Attempting to stop simple audio decode that is already stopped."));
        goto done;
    }

    NEXUS_SimpleAudioDecoder_Stop(_simpleDecoder);

    /* return pid */
    pPid     = getPid();
    _started = false;

    notifyObservers(eNotify_AudioDecodeStopped, this);
done:
    return pPid;
}

void CSimpleAudioDecode::setPid(CPid * pPid)
{
    _pPid = pPid;

    /* propogate to child decoders */
    if (NULL != _pDecoders[0])
    {
        _pDecoders[0]->setPid(pPid);
    }
    if (NULL != _pDecoders[1])
    {
        _pDecoders[1]->setPid(pPid);
    }
}

NEXUS_AudioInput CSimpleAudioDecode::getConnector(uint8_t num, NEXUS_AudioConnectorType type)
{
    NEXUS_AudioInput nexusInput = NULL;

    BDBG_ASSERT(2 > num);
    BDBG_ASSERT(NEXUS_AudioConnectorType_eMax > type);

    if (NULL != _pDecoders[num])
    {
        nexusInput = _pDecoders[num]->getConnector(type);
    }

    return nexusInput;
}

void CSimpleAudioDecode::setResources(void * id, CBoardResources * pResources) 
{ 
    BDBG_ASSERT(NULL != id);
    BDBG_ASSERT(NULL != pResources);

    _resourceId      = id; 
    _pBoardResources = pResources; 
}

eRet CSimpleAudioDecode::getStatus(NEXUS_AudioDecoderStatus * pStatus)
{
    eRet        ret    = eRet_Ok;
    NEXUS_Error nerror = NEXUS_SUCCESS;
    BDBG_ASSERT(NULL != pStatus);

    nerror = NEXUS_SimpleAudioDecoder_GetStatus(getSimpleDecoder(), pStatus);
    CHECK_NEXUS_ERROR_GOTO("error getting simple audio decoder status", ret, nerror, error);

error:
    return ret;
}

eRet CSimpleAudioDecode::setDownmix(eAudioDownmix audioDownmix)
{
    NEXUS_SimpleAudioDecoderSettings         decoderSettings;
    NEXUS_SimpleAudioDecoderHandle           decoder                 = getSimpleDecoder();
    NEXUS_AudioDecoderCodecSettings          codecSettings;
    int32_t                                  volumeLeft              = NEXUS_AUDIO_VOLUME_LINEAR_NORMAL;
    int32_t                                  volumeRight             = NEXUS_AUDIO_VOLUME_LINEAR_NORMAL;
    eRet                                     ret                     = eRet_Ok;
    NEXUS_Error                              nerror                  = NEXUS_SUCCESS;

    BDBG_ASSERT(false == isStarted());

    NEXUS_SimpleAudioDecoder_GetSettings(decoder, &decoderSettings);

    switch (audioDownmix)
    {
    default:
    case eAudioDownmix_None:
        decoderSettings.primary.volumeMatrix[NEXUS_AudioChannel_eLeft][NEXUS_AudioChannel_eLeft]     = volumeLeft;
        decoderSettings.primary.volumeMatrix[NEXUS_AudioChannel_eRight][NEXUS_AudioChannel_eRight]   = volumeRight;
        decoderSettings.primary.volumeMatrix[NEXUS_AudioChannel_eLeft][NEXUS_AudioChannel_eRight]    = 0;
        decoderSettings.primary.volumeMatrix[NEXUS_AudioChannel_eRight][NEXUS_AudioChannel_eLeft]    = 0;

        decoderSettings.secondary.volumeMatrix[NEXUS_AudioChannel_eLeft][NEXUS_AudioChannel_eLeft]   = volumeLeft;
        decoderSettings.secondary.volumeMatrix[NEXUS_AudioChannel_eRight][NEXUS_AudioChannel_eRight] = volumeRight;
        decoderSettings.secondary.volumeMatrix[NEXUS_AudioChannel_eLeft][NEXUS_AudioChannel_eRight]  = 0;
        decoderSettings.secondary.volumeMatrix[NEXUS_AudioChannel_eRight][NEXUS_AudioChannel_eLeft]  = 0;
        nerror = NEXUS_SimpleAudioDecoder_SetSettings(decoder, &decoderSettings);
        CHECK_NEXUS_ERROR_GOTO("unable to set audio downmix setting", ret, nerror, error);
        BDBG_MSG(("Setting standard downmix mode"));
        break;

    case eAudioDownmix_Left:
        decoderSettings.primary.volumeMatrix[NEXUS_AudioChannel_eLeft][NEXUS_AudioChannel_eLeft]     = volumeLeft;
        decoderSettings.primary.volumeMatrix[NEXUS_AudioChannel_eRight][NEXUS_AudioChannel_eRight]   = 0;
        decoderSettings.primary.volumeMatrix[NEXUS_AudioChannel_eLeft][NEXUS_AudioChannel_eRight]    = 0;
        decoderSettings.primary.volumeMatrix[NEXUS_AudioChannel_eRight][NEXUS_AudioChannel_eLeft]    = volumeRight;

        decoderSettings.secondary.volumeMatrix[NEXUS_AudioChannel_eLeft][NEXUS_AudioChannel_eLeft]   = volumeLeft;
        decoderSettings.secondary.volumeMatrix[NEXUS_AudioChannel_eRight][NEXUS_AudioChannel_eRight] = 0;
        decoderSettings.secondary.volumeMatrix[NEXUS_AudioChannel_eLeft][NEXUS_AudioChannel_eRight]  = 0;
        decoderSettings.secondary.volumeMatrix[NEXUS_AudioChannel_eRight][NEXUS_AudioChannel_eLeft]  = volumeRight;
        nerror = NEXUS_SimpleAudioDecoder_SetSettings(decoder, &decoderSettings);
        CHECK_NEXUS_ERROR_GOTO("unable to set audio downmix setting", ret, nerror, error);
        BDBG_MSG(("Setting downmix left"));
        break;

    case eAudioDownmix_Right:
        decoderSettings.primary.volumeMatrix[NEXUS_AudioChannel_eLeft][NEXUS_AudioChannel_eLeft]     = 0;
        decoderSettings.primary.volumeMatrix[NEXUS_AudioChannel_eRight][NEXUS_AudioChannel_eRight]   = volumeRight;
        decoderSettings.primary.volumeMatrix[NEXUS_AudioChannel_eLeft][NEXUS_AudioChannel_eRight]    = volumeLeft;
        decoderSettings.primary.volumeMatrix[NEXUS_AudioChannel_eRight][NEXUS_AudioChannel_eLeft]    = 0;

        decoderSettings.secondary.volumeMatrix[NEXUS_AudioChannel_eLeft][NEXUS_AudioChannel_eLeft]   = 0;
        decoderSettings.secondary.volumeMatrix[NEXUS_AudioChannel_eRight][NEXUS_AudioChannel_eRight] = volumeRight;
        decoderSettings.secondary.volumeMatrix[NEXUS_AudioChannel_eLeft][NEXUS_AudioChannel_eRight]  = volumeLeft;
        decoderSettings.secondary.volumeMatrix[NEXUS_AudioChannel_eRight][NEXUS_AudioChannel_eLeft]  = 0;
        nerror = NEXUS_SimpleAudioDecoder_SetSettings(decoder, &decoderSettings);
        CHECK_NEXUS_ERROR_GOTO("unable to set audio downmix setting", ret, nerror, error);
        BDBG_MSG(("Setting downmix right"));
        break;

    case eAudioDownmix_Monomix:
        decoderSettings.primary.volumeMatrix[NEXUS_AudioChannel_eLeft][NEXUS_AudioChannel_eLeft]     = volumeLeft/2;
        decoderSettings.primary.volumeMatrix[NEXUS_AudioChannel_eRight][NEXUS_AudioChannel_eRight]   = volumeRight/2;
        decoderSettings.primary.volumeMatrix[NEXUS_AudioChannel_eLeft][NEXUS_AudioChannel_eRight]    = volumeLeft/2;
        decoderSettings.primary.volumeMatrix[NEXUS_AudioChannel_eRight][NEXUS_AudioChannel_eLeft]    = volumeRight/2;
        decoderSettings.secondary.volumeMatrix[NEXUS_AudioChannel_eLeft][NEXUS_AudioChannel_eLeft]   = volumeLeft/2;
        decoderSettings.secondary.volumeMatrix[NEXUS_AudioChannel_eRight][NEXUS_AudioChannel_eRight] = volumeRight/2;
        decoderSettings.secondary.volumeMatrix[NEXUS_AudioChannel_eLeft][NEXUS_AudioChannel_eRight]  = volumeLeft/2;
        decoderSettings.secondary.volumeMatrix[NEXUS_AudioChannel_eRight][NEXUS_AudioChannel_eLeft]  = volumeRight/2;
        nerror = NEXUS_SimpleAudioDecoder_SetSettings(decoder, &decoderSettings);
        CHECK_NEXUS_ERROR_GOTO("unable to set audio downmix setting", ret, nerror, error);

        BDBG_MSG(("Setting downmix monomix"));
        break;

    case eAudioDownmix_Matrix:
        NEXUS_SimpleAudioDecoder_GetCodecSettings(decoder, NEXUS_SimpleAudioDecoderSelector_ePrimary, NEXUS_AudioCodec_eAacAdts, &codecSettings);
        codecSettings.codecSettings.aac.downmixMode = NEXUS_AudioDecoderAacDownmixMode_eMatrix;
        nerror = NEXUS_SimpleAudioDecoder_SetCodecSettings(decoder, NEXUS_SimpleAudioDecoderSelector_ePrimary, &codecSettings);
        CHECK_NEXUS_ERROR_GOTO("unable to set codec settings", ret, nerror, error);
        NEXUS_SimpleAudioDecoder_GetCodecSettings(decoder, NEXUS_SimpleAudioDecoderSelector_ePrimary, NEXUS_AudioCodec_eAacLoas, &codecSettings);
        codecSettings.codecSettings.aac.downmixMode = NEXUS_AudioDecoderAacDownmixMode_eMatrix;
        nerror = NEXUS_SimpleAudioDecoder_SetCodecSettings(decoder, NEXUS_SimpleAudioDecoderSelector_ePrimary, &codecSettings);
        CHECK_NEXUS_ERROR_GOTO("unable to set codec settings", ret, nerror, error);
        NEXUS_SimpleAudioDecoder_GetCodecSettings(decoder, NEXUS_SimpleAudioDecoderSelector_ePrimary, NEXUS_AudioCodec_eAacPlusAdts, &codecSettings);
        codecSettings.codecSettings.aacPlus.downmixMode = NEXUS_AudioDecoderAacDownmixMode_eMatrix;
        nerror = NEXUS_SimpleAudioDecoder_SetCodecSettings(decoder, NEXUS_SimpleAudioDecoderSelector_ePrimary, &codecSettings);
        CHECK_NEXUS_ERROR_GOTO("unable to set codec settings", ret, nerror, error);
        NEXUS_SimpleAudioDecoder_GetCodecSettings(decoder, NEXUS_SimpleAudioDecoderSelector_ePrimary, NEXUS_AudioCodec_eAacPlusLoas, &codecSettings);
        codecSettings.codecSettings.aacPlus.downmixMode = NEXUS_AudioDecoderAacDownmixMode_eMatrix;
        nerror = NEXUS_SimpleAudioDecoder_SetCodecSettings(decoder, NEXUS_SimpleAudioDecoderSelector_ePrimary, &codecSettings);
        CHECK_NEXUS_ERROR_GOTO("unable to set codec settings", ret, nerror, error);

        NEXUS_SimpleAudioDecoder_GetCodecSettings(decoder, NEXUS_SimpleAudioDecoderSelector_eSecondary, NEXUS_AudioCodec_eAacAdts, &codecSettings);
        codecSettings.codecSettings.aac.downmixMode = NEXUS_AudioDecoderAacDownmixMode_eMatrix;
        nerror = NEXUS_SimpleAudioDecoder_SetCodecSettings(decoder, NEXUS_SimpleAudioDecoderSelector_eSecondary, &codecSettings);
        CHECK_NEXUS_ERROR_GOTO("unable to set codec settings", ret, nerror, error);
        NEXUS_SimpleAudioDecoder_GetCodecSettings(decoder, NEXUS_SimpleAudioDecoderSelector_eSecondary, NEXUS_AudioCodec_eAacLoas, &codecSettings);
        codecSettings.codecSettings.aac.downmixMode = NEXUS_AudioDecoderAacDownmixMode_eMatrix;
        nerror = NEXUS_SimpleAudioDecoder_SetCodecSettings(decoder, NEXUS_SimpleAudioDecoderSelector_eSecondary, &codecSettings);
        CHECK_NEXUS_ERROR_GOTO("unable to set codec settings", ret, nerror, error);
        NEXUS_SimpleAudioDecoder_GetCodecSettings(decoder, NEXUS_SimpleAudioDecoderSelector_eSecondary, NEXUS_AudioCodec_eAacPlusAdts, &codecSettings);
        codecSettings.codecSettings.aacPlus.downmixMode = NEXUS_AudioDecoderAacDownmixMode_eMatrix;
        nerror = NEXUS_SimpleAudioDecoder_SetCodecSettings(decoder, NEXUS_SimpleAudioDecoderSelector_eSecondary, &codecSettings);
        CHECK_NEXUS_ERROR_GOTO("unable to set codec settings", ret, nerror, error);
        NEXUS_SimpleAudioDecoder_GetCodecSettings(decoder, NEXUS_SimpleAudioDecoderSelector_eSecondary, NEXUS_AudioCodec_eAacPlusLoas, &codecSettings);
        codecSettings.codecSettings.aacPlus.downmixMode = NEXUS_AudioDecoderAacDownmixMode_eMatrix;
        nerror = NEXUS_SimpleAudioDecoder_SetCodecSettings(decoder, NEXUS_SimpleAudioDecoderSelector_eSecondary, &codecSettings);
        CHECK_NEXUS_ERROR_GOTO("unable to set codec settings", ret, nerror, error);
        BDBG_MSG(("Setting AAC downmix type to (BRCM) matrix"));
        break;

    case eAudioDownmix_Arib:
        NEXUS_SimpleAudioDecoder_GetCodecSettings(decoder, NEXUS_SimpleAudioDecoderSelector_ePrimary, NEXUS_AudioCodec_eAacAdts, &codecSettings);
        codecSettings.codecSettings.aac.downmixMode = NEXUS_AudioDecoderAacDownmixMode_eArib;
        nerror = NEXUS_SimpleAudioDecoder_SetCodecSettings(decoder, NEXUS_SimpleAudioDecoderSelector_ePrimary, &codecSettings);
        CHECK_NEXUS_ERROR_GOTO("unable to set codec settings", ret, nerror, error);
        NEXUS_SimpleAudioDecoder_GetCodecSettings(decoder, NEXUS_SimpleAudioDecoderSelector_ePrimary, NEXUS_AudioCodec_eAacLoas, &codecSettings);
        codecSettings.codecSettings.aac.downmixMode = NEXUS_AudioDecoderAacDownmixMode_eArib;
        nerror = NEXUS_SimpleAudioDecoder_SetCodecSettings(decoder, NEXUS_SimpleAudioDecoderSelector_ePrimary, &codecSettings);
        CHECK_NEXUS_ERROR_GOTO("unable to set codec settings", ret, nerror, error);
        NEXUS_SimpleAudioDecoder_GetCodecSettings(decoder, NEXUS_SimpleAudioDecoderSelector_ePrimary, NEXUS_AudioCodec_eAacPlusAdts, &codecSettings);
        codecSettings.codecSettings.aacPlus.downmixMode = NEXUS_AudioDecoderAacDownmixMode_eArib;
        nerror = NEXUS_SimpleAudioDecoder_SetCodecSettings(decoder, NEXUS_SimpleAudioDecoderSelector_ePrimary, &codecSettings);
        CHECK_NEXUS_ERROR_GOTO("unable to set codec settings", ret, nerror, error);
        NEXUS_SimpleAudioDecoder_GetCodecSettings(decoder, NEXUS_SimpleAudioDecoderSelector_ePrimary, NEXUS_AudioCodec_eAacPlusLoas, &codecSettings);
        codecSettings.codecSettings.aacPlus.downmixMode = NEXUS_AudioDecoderAacDownmixMode_eArib;
        nerror = NEXUS_SimpleAudioDecoder_SetCodecSettings(decoder, NEXUS_SimpleAudioDecoderSelector_ePrimary, &codecSettings);
        CHECK_NEXUS_ERROR_GOTO("unable to set codec settings", ret, nerror, error);

        NEXUS_SimpleAudioDecoder_GetCodecSettings(decoder, NEXUS_SimpleAudioDecoderSelector_eSecondary, NEXUS_AudioCodec_eAacAdts, &codecSettings);
        codecSettings.codecSettings.aac.downmixMode = NEXUS_AudioDecoderAacDownmixMode_eArib;
        nerror = NEXUS_SimpleAudioDecoder_SetCodecSettings(decoder, NEXUS_SimpleAudioDecoderSelector_eSecondary, &codecSettings);
        CHECK_NEXUS_ERROR_GOTO("unable to set codec settings", ret, nerror, error);
        NEXUS_SimpleAudioDecoder_GetCodecSettings(decoder, NEXUS_SimpleAudioDecoderSelector_eSecondary, NEXUS_AudioCodec_eAacLoas, &codecSettings);
        codecSettings.codecSettings.aac.downmixMode = NEXUS_AudioDecoderAacDownmixMode_eArib;
        nerror = NEXUS_SimpleAudioDecoder_SetCodecSettings(decoder, NEXUS_SimpleAudioDecoderSelector_eSecondary, &codecSettings);
        CHECK_NEXUS_ERROR_GOTO("unable to set codec settings", ret, nerror, error);
        NEXUS_SimpleAudioDecoder_GetCodecSettings(decoder, NEXUS_SimpleAudioDecoderSelector_eSecondary, NEXUS_AudioCodec_eAacPlusAdts, &codecSettings);
        codecSettings.codecSettings.aacPlus.downmixMode = NEXUS_AudioDecoderAacDownmixMode_eArib;
        nerror = NEXUS_SimpleAudioDecoder_SetCodecSettings(decoder, NEXUS_SimpleAudioDecoderSelector_eSecondary, &codecSettings);
        CHECK_NEXUS_ERROR_GOTO("unable to set codec settings", ret, nerror, error);
        NEXUS_SimpleAudioDecoder_GetCodecSettings(decoder, NEXUS_SimpleAudioDecoderSelector_eSecondary, NEXUS_AudioCodec_eAacPlusLoas, &codecSettings);
        codecSettings.codecSettings.aacPlus.downmixMode = NEXUS_AudioDecoderAacDownmixMode_eArib;
        nerror = NEXUS_SimpleAudioDecoder_SetCodecSettings(decoder, NEXUS_SimpleAudioDecoderSelector_eSecondary, &codecSettings);
        CHECK_NEXUS_ERROR_GOTO("unable to set codec settings", ret, nerror, error);

        BDBG_MSG(("Setting downmix type to ARIB"));
        break;

    case eAudioDownmix_LtRt:
        NEXUS_SimpleAudioDecoder_GetCodecSettings(decoder, NEXUS_SimpleAudioDecoderSelector_ePrimary, NEXUS_AudioCodec_eAacAdts, &codecSettings);
        codecSettings.codecSettings.aac.downmixMode = NEXUS_AudioDecoderAacDownmixMode_eLtRt;
        nerror = NEXUS_SimpleAudioDecoder_SetCodecSettings(decoder, NEXUS_SimpleAudioDecoderSelector_ePrimary, &codecSettings);
        CHECK_NEXUS_ERROR_GOTO("unable to set codec settings", ret, nerror, error);
        NEXUS_SimpleAudioDecoder_GetCodecSettings(decoder, NEXUS_SimpleAudioDecoderSelector_ePrimary, NEXUS_AudioCodec_eAacLoas, &codecSettings);
        codecSettings.codecSettings.aac.downmixMode = NEXUS_AudioDecoderAacDownmixMode_eLtRt;
        nerror = NEXUS_SimpleAudioDecoder_SetCodecSettings(decoder, NEXUS_SimpleAudioDecoderSelector_ePrimary, &codecSettings);
        CHECK_NEXUS_ERROR_GOTO("unable to set codec settings", ret, nerror, error);
        NEXUS_SimpleAudioDecoder_GetCodecSettings(decoder, NEXUS_SimpleAudioDecoderSelector_ePrimary, NEXUS_AudioCodec_eAacPlusAdts, &codecSettings);
        codecSettings.codecSettings.aacPlus.downmixMode = NEXUS_AudioDecoderAacDownmixMode_eLtRt;
        nerror = NEXUS_SimpleAudioDecoder_SetCodecSettings(decoder, NEXUS_SimpleAudioDecoderSelector_ePrimary, &codecSettings);
        CHECK_NEXUS_ERROR_GOTO("unable to set codec settings", ret, nerror, error);
        NEXUS_SimpleAudioDecoder_GetCodecSettings(decoder, NEXUS_SimpleAudioDecoderSelector_ePrimary, NEXUS_AudioCodec_eAacPlusLoas, &codecSettings);
        codecSettings.codecSettings.aacPlus.downmixMode = NEXUS_AudioDecoderAacDownmixMode_eLtRt;
        nerror = NEXUS_SimpleAudioDecoder_SetCodecSettings(decoder, NEXUS_SimpleAudioDecoderSelector_ePrimary, &codecSettings);
        CHECK_NEXUS_ERROR_GOTO("unable to set codec settings", ret, nerror, error);

        NEXUS_SimpleAudioDecoder_GetCodecSettings(decoder, NEXUS_SimpleAudioDecoderSelector_eSecondary, NEXUS_AudioCodec_eAacAdts, &codecSettings);
        codecSettings.codecSettings.aac.downmixMode = NEXUS_AudioDecoderAacDownmixMode_eLtRt;
        nerror = NEXUS_SimpleAudioDecoder_SetCodecSettings(decoder, NEXUS_SimpleAudioDecoderSelector_eSecondary, &codecSettings);
        CHECK_NEXUS_ERROR_GOTO("unable to set codec settings", ret, nerror, error);
        NEXUS_SimpleAudioDecoder_GetCodecSettings(decoder, NEXUS_SimpleAudioDecoderSelector_eSecondary, NEXUS_AudioCodec_eAacLoas, &codecSettings);
        codecSettings.codecSettings.aac.downmixMode = NEXUS_AudioDecoderAacDownmixMode_eLtRt;
        nerror = NEXUS_SimpleAudioDecoder_SetCodecSettings(decoder, NEXUS_SimpleAudioDecoderSelector_eSecondary, &codecSettings);
        CHECK_NEXUS_ERROR_GOTO("unable to set codec settings", ret, nerror, error);
        NEXUS_SimpleAudioDecoder_GetCodecSettings(decoder, NEXUS_SimpleAudioDecoderSelector_ePrimary, NEXUS_AudioCodec_eAacPlusAdts, &codecSettings);
        codecSettings.codecSettings.aacPlus.downmixMode = NEXUS_AudioDecoderAacDownmixMode_eLtRt;
        nerror = NEXUS_SimpleAudioDecoder_SetCodecSettings(decoder, NEXUS_SimpleAudioDecoderSelector_eSecondary, &codecSettings);
        CHECK_NEXUS_ERROR_GOTO("unable to set codec settings", ret, nerror, error);
        NEXUS_SimpleAudioDecoder_GetCodecSettings(decoder, NEXUS_SimpleAudioDecoderSelector_eSecondary, NEXUS_AudioCodec_eAacPlusLoas, &codecSettings);
        codecSettings.codecSettings.aacPlus.downmixMode = NEXUS_AudioDecoderAacDownmixMode_eLtRt;
        nerror = NEXUS_SimpleAudioDecoder_SetCodecSettings(decoder, NEXUS_SimpleAudioDecoderSelector_eSecondary, &codecSettings);
        CHECK_NEXUS_ERROR_GOTO("unable to set codec settings", ret, nerror, error);

        BDBG_MSG(("Setting downmix type to ARIB"));
        break;
    }

    _downmix = audioDownmix;
    BDBG_MSG(("setting audio downmix:%d", _downmix));

error:
    return ret;
}

eRet CSimpleAudioDecode::setDualMono(eAudioDualMono dualMono)
{
    NEXUS_SimpleAudioDecoderSettings         decoderSettings;
    NEXUS_SimpleAudioDecoderHandle           decoder                 = getSimpleDecoder();
    eRet                                     ret                     = eRet_Ok;
    NEXUS_Error                              nerror                  = NEXUS_SUCCESS;
    
    BDBG_ASSERT(false == isStarted());

    NEXUS_SimpleAudioDecoder_GetSettings(decoder, &decoderSettings);

    switch (dualMono)
    {
    case eAudioDualMono_Left:
        decoderSettings.primary.dualMonoMode = NEXUS_AudioDecoderDualMonoMode_eLeft;
        decoderSettings.secondary.dualMonoMode = NEXUS_AudioDecoderDualMonoMode_eLeft;
        break;
    case eAudioDualMono_Right:
        decoderSettings.primary.dualMonoMode = NEXUS_AudioDecoderDualMonoMode_eRight;
        decoderSettings.secondary.dualMonoMode = NEXUS_AudioDecoderDualMonoMode_eRight;
        break;
    default:
    case eAudioDualMono_Stereo:
        decoderSettings.primary.dualMonoMode = NEXUS_AudioDecoderDualMonoMode_eStereo;
        decoderSettings.secondary.dualMonoMode = NEXUS_AudioDecoderDualMonoMode_eStereo;
        break;
    case eAudioDualMono_Monomix:
        decoderSettings.primary.dualMonoMode = NEXUS_AudioDecoderDualMonoMode_eMix;
        decoderSettings.secondary.dualMonoMode = NEXUS_AudioDecoderDualMonoMode_eMix;
        break;
    }

    nerror = NEXUS_SimpleAudioDecoder_SetSettings(decoder, &decoderSettings);
    CHECK_NEXUS_ERROR_GOTO("unable to set audio decoder settings", ret, nerror, error);

    _dualMono = dualMono;
    BDBG_MSG(("setting dual mono:%d", _dualMono));

error:
    return ret;
}

eRet CSimpleAudioDecode::setDolbyDRC(eDolbyDRC dolbyDRC)
{
    NEXUS_AudioDecoderCodecSettings          codecSettings;
    NEXUS_SimpleAudioDecoderHandle           decoder                 = getSimpleDecoder();
    CPid                                   * pPid                    = NULL;
    eRet                                     ret                     = eRet_Ok;
    NEXUS_Error                              nerror                  = NEXUS_SUCCESS;

    BDBG_ASSERT(false == isStarted());

    pPid = getPid();
    CHECK_PTR_GOTO("audio decoder has missing pid", pPid, ret, eRet_InvalidState, error);
    
    switch (pPid->getAudioCodec())
    {
    case NEXUS_AudioCodec_eAc3:
        NEXUS_SimpleAudioDecoder_GetCodecSettings(decoder, NEXUS_SimpleAudioDecoderSelector_ePrimary, NEXUS_AudioCodec_eAc3, &codecSettings);
        switch (dolbyDRC)
        {
        case eDolbyDRC_Light:
        case eDolbyDRC_Medium:
            codecSettings.codecSettings.ac3.drcMode = NEXUS_AudioDecoderDolbyDrcMode_eLine;
            codecSettings.codecSettings.ac3.boost   = GET_INT(_pCfg, AUDIO_DOLBY_DRC_AC3_BOOST);
            codecSettings.codecSettings.ac3.cut     = GET_INT(_pCfg, AUDIO_DOLBY_DRC_AC3_CUT);
            break;
        case eDolbyDRC_Heavy:
            codecSettings.codecSettings.ac3.drcMode = NEXUS_AudioDecoderDolbyDrcMode_eRf;
            codecSettings.codecSettings.ac3.boost   = 0;
            codecSettings.codecSettings.ac3.cut     = 0;
            break;
        case eDolbyDRC_None:
        default:
            codecSettings.codecSettings.ac3.drcMode = NEXUS_AudioDecoderDolbyDrcMode_eLine;
            codecSettings.codecSettings.ac3.boost   = 0;
            codecSettings.codecSettings.ac3.cut     = 0;
            break;
        }
        nerror = NEXUS_SimpleAudioDecoder_SetCodecSettings(decoder, NEXUS_SimpleAudioDecoderSelector_ePrimary, &codecSettings);
        CHECK_NEXUS_ERROR_GOTO("unable to set codec settings", ret, nerror, error);

        NEXUS_SimpleAudioDecoder_GetCodecSettings(decoder, NEXUS_SimpleAudioDecoderSelector_eSecondary, NEXUS_AudioCodec_eAc3, &codecSettings);
        switch (dolbyDRC)
        {
        case eDolbyDRC_Light:
        case eDolbyDRC_Medium:
            codecSettings.codecSettings.ac3.drcMode = NEXUS_AudioDecoderDolbyDrcMode_eLine;
            codecSettings.codecSettings.ac3.boost   = GET_INT(_pCfg, AUDIO_DOLBY_DRC_AC3_BOOST);
            codecSettings.codecSettings.ac3.cut     = GET_INT(_pCfg, AUDIO_DOLBY_DRC_AC3_CUT);
            break;
        case eDolbyDRC_Heavy:
            codecSettings.codecSettings.ac3.drcMode = NEXUS_AudioDecoderDolbyDrcMode_eRf;
            codecSettings.codecSettings.ac3.boost   = 0;
            codecSettings.codecSettings.ac3.cut     = 0;
            break;
        case eDolbyDRC_None:
        default:
            codecSettings.codecSettings.ac3.drcMode = NEXUS_AudioDecoderDolbyDrcMode_eLine;
            codecSettings.codecSettings.ac3.boost   = 0;
            codecSettings.codecSettings.ac3.cut     = 0;
            break;
        }
        nerror = NEXUS_SimpleAudioDecoder_SetCodecSettings(decoder, NEXUS_SimpleAudioDecoderSelector_eSecondary, &codecSettings);
        CHECK_NEXUS_ERROR_GOTO("unable to set codec settings", ret, nerror, error);
        break;

    case NEXUS_AudioCodec_eAc3Plus:
        NEXUS_SimpleAudioDecoder_GetCodecSettings(decoder, NEXUS_SimpleAudioDecoderSelector_ePrimary, NEXUS_AudioCodec_eAc3Plus, &codecSettings);
        switch (dolbyDRC)
        {
        case eDolbyDRC_Light:
        case eDolbyDRC_Medium:
            codecSettings.codecSettings.ac3Plus.drcMode = NEXUS_AudioDecoderDolbyDrcMode_eLine;
            codecSettings.codecSettings.ac3Plus.boost   = GET_INT(_pCfg, AUDIO_DOLBY_DRC_AC3_BOOST);
            codecSettings.codecSettings.ac3Plus.cut     = GET_INT(_pCfg, AUDIO_DOLBY_DRC_AC3_CUT);
            break;
        case eDolbyDRC_Heavy:
            codecSettings.codecSettings.ac3Plus.drcMode = NEXUS_AudioDecoderDolbyDrcMode_eRf;
            codecSettings.codecSettings.ac3Plus.boost   = 0;
            codecSettings.codecSettings.ac3Plus.cut     = 0;
            break;
        case eDolbyDRC_None:
        default:
            codecSettings.codecSettings.ac3Plus.drcMode = NEXUS_AudioDecoderDolbyDrcMode_eLine;
            codecSettings.codecSettings.ac3Plus.boost   = 0;
            codecSettings.codecSettings.ac3Plus.cut     = 0;
            break;
        }
        nerror = NEXUS_SimpleAudioDecoder_SetCodecSettings(decoder, NEXUS_SimpleAudioDecoderSelector_ePrimary, &codecSettings);
        CHECK_NEXUS_ERROR_GOTO("unable to set codec settings", ret, nerror, error);

        NEXUS_SimpleAudioDecoder_GetCodecSettings(decoder, NEXUS_SimpleAudioDecoderSelector_eSecondary, NEXUS_AudioCodec_eAc3Plus, &codecSettings);
        switch (dolbyDRC)
        {
        case eDolbyDRC_Light:
        case eDolbyDRC_Medium:
            codecSettings.codecSettings.ac3Plus.drcMode = NEXUS_AudioDecoderDolbyDrcMode_eLine;
            codecSettings.codecSettings.ac3Plus.boost   = GET_INT(_pCfg, AUDIO_DOLBY_DRC_AC3_BOOST);
            codecSettings.codecSettings.ac3Plus.cut     = GET_INT(_pCfg, AUDIO_DOLBY_DRC_AC3_CUT);
            break;
        case eDolbyDRC_Heavy:
            codecSettings.codecSettings.ac3Plus.drcMode = NEXUS_AudioDecoderDolbyDrcMode_eRf;
            codecSettings.codecSettings.ac3Plus.boost   = 0;
            codecSettings.codecSettings.ac3Plus.cut     = 0;
            break;
        case eDolbyDRC_None:
        default:
            codecSettings.codecSettings.ac3Plus.drcMode = NEXUS_AudioDecoderDolbyDrcMode_eLine;
            codecSettings.codecSettings.ac3Plus.boost   = 0;
            codecSettings.codecSettings.ac3Plus.cut     = 0;
            break;
        }
        nerror = NEXUS_SimpleAudioDecoder_SetCodecSettings(decoder, NEXUS_SimpleAudioDecoderSelector_eSecondary, &codecSettings);
        CHECK_NEXUS_ERROR_GOTO("unable to set codec settings", ret, nerror, error);
        break;

    default:
        BDBG_WRN(("ignoring attempt to set dolby drc for invalid codec:%d", pPid->getAudioCodec()));
        ret = eRet_InvalidState;
        goto error;
    }

    _dolbyDRC = dolbyDRC;
    BDBG_MSG(("setting dolby drc:%d", _dolbyDRC));

error:
    return ret;
}


eRet CSimpleAudioDecode::setDolbyDialogNorm(bool dolbyDialogNorm)
{
    NEXUS_AudioDecoderCodecSettings          codecSettings;
    NEXUS_SimpleAudioDecoderHandle           decoder                 = getSimpleDecoder();
    CPid                                   * pPid                    = NULL;
    eRet                                     ret                     = eRet_Ok;
    NEXUS_Error                              nerror                  = NEXUS_SUCCESS;

    BDBG_ASSERT(false == isStarted());

    pPid = getPid();
    CHECK_PTR_GOTO("audio decoder has missing pid", pPid, ret, eRet_InvalidState, error);
        
    switch (pPid->getAudioCodec())
    {
    case NEXUS_AudioCodec_eAc3:
        NEXUS_SimpleAudioDecoder_GetCodecSettings(decoder, NEXUS_SimpleAudioDecoderSelector_ePrimary, NEXUS_AudioCodec_eAc3, &codecSettings);
        codecSettings.codecSettings.ac3.dialogNormalization = dolbyDialogNorm;
        nerror = NEXUS_SimpleAudioDecoder_SetCodecSettings(decoder, NEXUS_SimpleAudioDecoderSelector_ePrimary, &codecSettings);
        CHECK_NEXUS_ERROR_GOTO("unable to set codec settings", ret, nerror, error);
        break;

    case NEXUS_AudioCodec_eAc3Plus:
        NEXUS_SimpleAudioDecoder_GetCodecSettings(decoder, NEXUS_SimpleAudioDecoderSelector_eSecondary, NEXUS_AudioCodec_eAc3Plus, &codecSettings);
        codecSettings.codecSettings.ac3Plus.dialogNormalization = dolbyDialogNorm;
        nerror = NEXUS_SimpleAudioDecoder_SetCodecSettings(decoder, NEXUS_SimpleAudioDecoderSelector_eSecondary, &codecSettings);
        CHECK_NEXUS_ERROR_GOTO("unable to set codec settings", ret, nerror, error);
        break;

    default:
        BDBG_WRN(("ignoring attempt to set dolby dialog normalization for invalid codec:%d", pPid->getAudioCodec()));
        ret = eRet_InvalidState;
        goto error;
    }

    _dolbyDialogNorm = dolbyDialogNorm;
    BDBG_MSG(("setting dolby dialog normalization:%d", _dolbyDialogNorm));

error:
    return ret;
}
