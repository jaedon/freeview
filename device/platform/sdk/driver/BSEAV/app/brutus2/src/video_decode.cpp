/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: video_decode.cpp $
* $brcm_Revision: 11 $
* $brcm_Date: 10/19/12 10:36a $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/video_decode.cpp $
* 
* 11   10/19/12 10:36a tokushig
* SW7231-749: fix brutus shutdown by freeing resources in the proper
* order.
* 
* 10   10/10/12 5:31p tokushig
* SW7231-749: adjust debug output for release
* 
* 9   10/7/12 3:11p tokushig
* SW7231-749: add support for auto format change
* 
* 8   9/26/12 4:45p tokushig
* SW7231-749: allow video decoder stop/start without having to resubmit
* the pid
* 
* 7   9/21/12 4:11p tokushig
* SW7231-749: fix close/stop in video decode
* 
* 6   9/10/12 3:49p tokushig
* SW7231-749: add simple video decoder support
* 
* 5   5/25/12 3:12p tokushig
* SW7231-749: remove platform specific file and move to common
* platform.h.  also change appropriate brutus defines to nexus defines
* when available.
* 
* 4   4/15/12 9:47p tokushig
* SW7231-749: add start/stop decode notification
* 
* 3   4/9/12 7:29p tokushig
* SW7405-5581: added video source changed callback support
* 
* 2   2/28/12 5:03p tokushig
* SW7405-5581: add header comments
* 
* 
*********************************************************************************/

#include "nexus_video_decoder.h"
#include "nexus_simple_video_decoder_server.h"
#include "nexus_core_utils.h"
#include "video_decode.h"
#include "video_window.h"
#include "output.h"
#include "brutus_os.h"

BDBG_MODULE(brutus_video_decode);


/* bwin io callback that is triggered when it is safe to handle callbacks */
static void bwinVideoDecodeCallback(bwidget_engine_t engine, bwin_io_handle handle)
{
    eRet                       ret          = eRet_Ok;
    CSimpleVideoDecode       * pVideoDecode = (CSimpleVideoDecode *)handle;
    NEXUS_VideoDecoderStatus   status;

    BDBG_ASSERT(NULL != pVideoDecode);

    if (true == pVideoDecode->isSourceChanged())
    {
        pVideoDecode->getStatus(&status);

        /* ignore 0 source change callback */
        if (0 == status.aspectRatio + status.source.width + status.source.height)
            goto error;
       
        BDBG_MSG(("Notify Observers for video decoder event code: %#x", eNotify_VideoSourceChanged));
        ret = pVideoDecode->notifyObservers(eNotify_VideoSourceChanged, pVideoDecode);
        CHECK_ERROR_GOTO("error notifying observers", ret, error);
    }

error:
    return;
}

/* nexus callback that is executed when the video source has changed */
static void NexusVideoDecodeSourceChangedCallback(void * context, int param)
{
    CVideoDecode * pVideoDecode = (CVideoDecode *)context;
    BSTD_UNUSED(param);
    BDBG_ASSERT(NULL != pVideoDecode);

    pVideoDecode->setSourceChanged(true);

    /* sync with bwin loop */
    bwidget_trigger_io(pVideoDecode->getWidgetEngine(), (bwin_io_handle)pVideoDecode);
}


CVideoDecode::CVideoDecode(const char * name, const uint16_t number, CConfiguration * pCfg) :
    CResource(name, number, eBoardResource_decodeVideo, pCfg),
    _decoder(NULL),
    _sourceChanged(false),
    _started(false)
{
    eRet ret = eRet_Ok;
    BDBG_ASSERT(NEXUS_NUM_VIDEO_DECODERS > _number);

error:
    BDBG_ASSERT(eRet_Ok == ret);
}

CVideoDecode::~CVideoDecode()
{
}

eRet CVideoDecode::open(bwidget_engine_t widgetEngine, CStc * pStc)
{
    eRet                           ret       = eRet_Ok;
    int                            fifoSize  = 0;
    NEXUS_VideoDecoderOpenSettings settings;

    BDBG_ASSERT(NULL != pStc);

    if (true == isOpened())
    {
        ret = eRet_InvalidState;
        CHECK_ERROR_GOTO("Video decoder is already opened.", ret, error);
    }

    _widgetEngine = widgetEngine;
    bwidget_add_io_handle(_widgetEngine, (bwin_io_handle)this, bwinVideoDecodeCallback);

    /* save stc for start() */
    _pStc = pStc;

    /* dtt - todo sync channel should NOT be in cvideodecode because caudiodecode needs it too! */
#if NEXUS_HAS_SYNC_CHANNEL
    {
        NEXUS_SyncChannelSettings settings;
        NEXUS_SyncChannel_GetDefaultSettings(&settings);
        settings.enablePrecisionLipsync = GET_BOOL(_pCfg, PRECISION_LIPSYNC_ENABLED);
        _syncChannel = NEXUS_SyncChannel_Create(&settings);
        CHECK_PTR_GOTO("Nexus sync channel create failed", _syncChannel, ret, eRet_ExternalError, error);
    }
#endif

    NEXUS_VideoDecoder_GetDefaultOpenSettings(&settings);
    fifoSize = GET_INT(_pCfg, VIDEO_DECODER_FIFO_SIZE);
    if (0 < fifoSize)
    {
        settings.fifoSize = fifoSize * 1000; /* dtt - should this be 1024? */
        BDBG_WRN(("****************************************"));
        BDBG_WRN(("* Changing Video FIFO size to %d bytes *", settings.fifoSize));
        BDBG_WRN(("****************************************"));
    }

    _decoder = NEXUS_VideoDecoder_Open(_number, &settings);
    CHECK_PTR_GOTO("Nexus video decoder open failed!", _decoder, ret, eRet_ExternalError, error);

    /* set video decoder settings */
    #if 0
    {
        NEXUS_Error                 nError                = NEXUS_SUCCESS;
        NEXUS_VideoDecoderSettings  videoDecoderSettings;

        NEXUS_VideoDecoder_GetSettings(_decoder, &videoDecoderSettings);
        videoDecoderSettings.sourceChanged.callback = NexusVideoDecodeSourceChangedCallback;
        videoDecoderSettings.sourceChanged.context  = this;
        videoDecoderSettings.sourceChanged.param    = 0;
        videoDecoderSettings.dropFieldMode = GET_BOOL(_pCfg, DROP_FIELD_MODE_ENABLED);
        nError = NEXUS_VideoDecoder_SetSettings(_decoder, &videoDecoderSettings);
        CHECK_NEXUS_ERROR_GOTO("Error applying video decoder settings.", ret, nError, error);
    }
    #endif

    /* dtt - create astm (adaptive system time managment) */

error:
    return ret;
}

CStc * CVideoDecode::close()
{
    CStc * pStc = NULL;

    if (_decoder)
    {
        NEXUS_VideoInput_Shutdown(NEXUS_VideoDecoder_GetConnector(_decoder));
        NEXUS_VideoDecoder_Close(_decoder);

        /* return stc */
        pStc     = _pStc;
        _pStc    = NULL;
        _decoder = NULL;
    }

    return pStc;
}

eRet CVideoDecode::getStatus(NEXUS_VideoDecoderStatus * pStatus)
{
    eRet        ret    = eRet_Ok;
    NEXUS_Error nerror = NEXUS_SUCCESS;

    BDBG_ASSERT(NULL != pStatus);

    nerror = NEXUS_VideoDecoder_GetStatus(getDecoder(), pStatus);
    CHECK_NEXUS_ERROR_GOTO("getting video decoder status failed!", ret, nerror, error);

error:
    return ret;
}


eRet CVideoDecode::start(CPid * pPid)
{
    eRet                                  ret         = eRet_Ok;
    NEXUS_Error                           nerror      = NEXUS_SUCCESS;
    NEXUS_VideoDecoderStartSettings       settings;

    BDBG_ASSERT((NULL != pPid) || (NULL != _pPid));
    BDBG_ASSERT(NULL != _pStc);
    BDBG_ASSERT(true == isOpened());

    NEXUS_VideoDecoder_GetDefaultStartSettings(&settings);
    settings.codec      = pPid->getVideoCodec();
    settings.pidChannel = pPid->getPidChannel();
    settings.stcChannel = _pStc->getStcChannel();
    nerror = NEXUS_VideoDecoder_Start(_decoder, &settings);
    CHECK_NEXUS_ERROR_GOTO("starting video decoder failed!", ret, nerror, error);

    /* save pid */
    _pPid    = pPid;
    _started = true;

    notifyObservers(eNotify_VideoDecodeStarted, this);
error:
    return ret;
}

CPid * CVideoDecode::stop()
{
    CPid * pPid = NULL;
    eRet   ret  = eRet_Ok;

    if (false == isStarted())
    {
        BDBG_WRN(("Attempting to stop video decode that is already stopped."));
        goto done;
    }

    BDBG_ASSERT(NULL != _decoder);
    NEXUS_VideoDecoder_Stop(_decoder);

    /* return pid */
    pPid     = _pPid;
    _started = false;

    notifyObservers(eNotify_VideoDecodeStopped, this);
done:
    return pPid;
}


/*** simple video decoder class */

#include "nexus_simple_video_decoder_server.h"
#include "brutus_board.h"
#define BRUTUS_NUM_SIMPLE_VIDEO_DECODERS 1

CSimpleVideoDecode::CSimpleVideoDecode(const char * name, const uint16_t number, CConfiguration * pCfg) :
    CVideoDecode(name, number, pCfg),
    _simpleDecoder(NULL),
    _pBoardResources(NULL),
    _pVideoWindow(NULL),
    _resourceId(0)
{
    eRet ret = eRet_Ok;
    BDBG_ASSERT(BRUTUS_NUM_SIMPLE_VIDEO_DECODERS > _number);

    /* manually set type since base class CVideoDecoder constructor will default to
       regular video decoder type */
    setType(eBoardResource_simpleDecodeVideo);

error:
    BDBG_ASSERT(eRet_Ok == ret);
}

CSimpleVideoDecode::~CSimpleVideoDecode()
{
    stop();
    close();
}

eRet CSimpleVideoDecode::open(bwidget_engine_t widgetEngine, CStc * pStc)
{
    eRet                                   ret       = eRet_Ok;
    int                                    i         = 0;
    NEXUS_SimpleVideoDecoderServerSettings settings;

    BDBG_ASSERT(NULL != pStc);

    if (true == isOpened())
    {
        ret = eRet_InvalidState;
        CHECK_ERROR_GOTO("Simple Video decoder is already opened.", ret, error);
    }

    if (NULL == _pBoardResources)
    {
        ret = eRet_InvalidState;
        CHECK_ERROR_GOTO("Set board resources before opening.", ret, error);
    }

    if (NULL == _pDisplay)
    {
        ret = eRet_InvalidState;
        CHECK_ERROR_GOTO("Set display before opening.", ret, error);
    }

    _widgetEngine = widgetEngine;
    bwidget_add_io_handle(_widgetEngine, (bwin_io_handle)this, bwinVideoDecodeCallback);

    /* save stc for start() */
    _pStc = pStc;

    _pVideoWindow = _pDisplay->checkoutVideoWindow();
    CHECK_PTR_GOTO("unable to check out video window", _pVideoWindow, ret, eRet_NotAvailable, error);
    ret = _pVideoWindow->open();
    CHECK_ERROR_GOTO("display failed to open", ret, error);
    {
        NEXUS_VideoWindowSettings winSettings;
        NEXUS_VideoWindow_GetSettings(_pVideoWindow->getWindow(), &winSettings);
        winSettings.contentMode = NEXUS_VideoWindowContentMode_eBox;
        NEXUS_VideoWindow_SetSettings(_pVideoWindow->getWindow(), &winSettings);
    }

    _pDecoder = (CVideoDecode *)_pBoardResources->checkoutResource(_resourceId, eBoardResource_decodeVideo);
    CHECK_PTR_GOTO("unable to check out video decoder", _pDecoder, ret, eRet_NotAvailable, error);
    
    ret = _pDecoder->open(_widgetEngine, pStc);
    CHECK_ERROR_GOTO("Video decode failed to open", ret, error);

    /* create simple video decoder */
    {
        NEXUS_SimpleVideoDecoderServerSettings settings;
        NEXUS_SimpleVideoDecoder_GetDefaultServerSettings(&settings);
        settings.videoDecoder = _pDecoder->getDecoder();
        settings.window[0]   = _pVideoWindow->getWindow();
        _simpleDecoder = NEXUS_SimpleVideoDecoder_Create(0, &settings);
    }

    /* set video decoder settings */
    {
        NEXUS_Error                 nError                = NEXUS_SUCCESS;
        NEXUS_VideoDecoderSettings  videoDecoderSettings;

        NEXUS_SimpleVideoDecoder_GetSettings(_simpleDecoder, &videoDecoderSettings);
        videoDecoderSettings.sourceChanged.callback = NexusVideoDecodeSourceChangedCallback;
        videoDecoderSettings.sourceChanged.context  = this;
        videoDecoderSettings.sourceChanged.param    = 0;
        videoDecoderSettings.dropFieldMode = GET_BOOL(_pCfg, DROP_FIELD_MODE_ENABLED);
        nError = NEXUS_SimpleVideoDecoder_SetSettings(_simpleDecoder, &videoDecoderSettings);
        CHECK_NEXUS_ERROR_GOTO("Error applying simple video decoder settings.", ret, nError, error);
    }

error:
    return ret;
}

CStc * CSimpleVideoDecode::close()
{
    CStc * pStc = NULL;

    if (NULL != _simpleDecoder)
    { 
        NEXUS_SimpleVideoDecoderServerSettings settings;

        NEXUS_SimpleVideoDecoder_GetDefaultServerSettings(&settings);
        settings.enabled = false;
        NEXUS_SimpleVideoDecoder_SetServerSettings(_simpleDecoder, &settings);

        NEXUS_SimpleVideoDecoder_Destroy(_simpleDecoder);
        _simpleDecoder = NULL;
    }

    if (NULL != _pDecoder)
    {
        _pDecoder->close();
        _pBoardResources->checkinResource(_pDecoder);
    }

    if (NULL != _pDisplay)
    {
        _pDisplay->checkinVideoWindow(_pVideoWindow);
    }

    /* return stc */
    pStc  = _pStc;
    _pStc = NULL;

    return pStc;
}

eRet CSimpleVideoDecode::start(CPid * pPid)
{
    eRet                                  ret         = eRet_Ok;
    NEXUS_Error                           nerror      = NEXUS_SUCCESS;
    NEXUS_SimpleVideoDecoderStartSettings simpleSettings;

    BDBG_ASSERT((NULL != pPid) || (NULL != _pPid));
    BDBG_ASSERT(NULL != _pStc);
    BDBG_ASSERT(true == isOpened());

    NEXUS_SimpleVideoDecoder_GetDefaultStartSettings(&simpleSettings);
    simpleSettings.settings.codec      = pPid->getVideoCodec();
    simpleSettings.settings.pidChannel = pPid->getPidChannel();
    CHECK_PTR_GOTO("cannot start simple decode without valid stc", getStc(), ret, eRet_InvalidParameter, error);
    simpleSettings.settings.stcChannel = getStc()->getStcChannel();
    nerror = NEXUS_SimpleVideoDecoder_Start(_simpleDecoder, &simpleSettings);
    CHECK_NEXUS_ERROR_GOTO("starting simple video decoder failed!", ret, nerror, error);

    /* save pid */
    _pPid    = pPid;
    _started = true;

    notifyObservers(eNotify_VideoDecodeStarted, this);
error:
    return ret;
}

CPid * CSimpleVideoDecode::stop()
{
    CPid * pPid = NULL;
    eRet   ret  = eRet_Ok;

    if (false == isStarted())
    {
        BDBG_WRN(("Attempting to stop video decode that is already stopped."));
        goto done;
    }

    BDBG_ASSERT(NULL != _simpleDecoder);
    NEXUS_SimpleVideoDecoder_Stop(_simpleDecoder);

    /* return pid */
    pPid     = _pPid;
    _started = false;

    notifyObservers(eNotify_VideoDecodeStopped, this);
done:
    return pPid;
}

eRet CSimpleVideoDecode::getStatus(NEXUS_VideoDecoderStatus * pStatus)
{
    eRet        ret    = eRet_Ok;
    NEXUS_Error nerror = NEXUS_SUCCESS;

    BDBG_ASSERT(NULL != pStatus);

    nerror = NEXUS_SimpleVideoDecoder_GetStatus(getSimpleDecoder(), pStatus);
    CHECK_NEXUS_ERROR_GOTO("getting simple video decoder status failed!", ret, nerror, error);

error:
    return ret;
}

eRet CSimpleVideoDecode::getOptimalVideoFormat(CVideoOutput * pOutput, NEXUS_VideoFormat * pFormat)
{
    NEXUS_VideoDecoderStatus   status;
    NEXUS_VideoFormatInfo      formatInfo;
    bool                       isPal          = false;
    eRet                       ret            = eRet_Ok;

    getStatus(&status);

    NEXUS_VideoFormat_GetInfo(status.format, &formatInfo);
    if ((5000 == formatInfo.verticalFreq) || (2500 == formatInfo.verticalFreq))
    {
        isPal = true;
    }

    if (240 > status.source.height)
    {
        /* current source height is too small */
        ret = eRet_NotAvailable;
        goto error;
    }

    if (false == isPal)
    {
        if (1080 <= status.source.height)
        {
            if (false == formatInfo.interlaced) 
            {
                *pFormat = NEXUS_VideoFormat_e1080p;
                goto done;
            }
            else
            {
                *pFormat = NEXUS_VideoFormat_e1080i;
                goto done;
            }
        }
        
        if (720 <= status.source.height)
        {
            if (false == formatInfo.interlaced)
            {
                *pFormat = NEXUS_VideoFormat_e720p;
                goto done;
            }
        }
        
        if (576 <= status.source.height)
        {
            if ((false == formatInfo.interlaced) && (true == pOutput->isValidVideoFormat(NEXUS_VideoFormat_e576p)))
            {
                *pFormat = NEXUS_VideoFormat_e576p;
                goto done;
            }
            else
            if ((true == formatInfo.interlaced) && (true == pOutput->isValidVideoFormat(NEXUS_VideoFormat_ePal)))
            {
                *pFormat = NEXUS_VideoFormat_ePal;
                goto done;
            }
        }

        if ((false == formatInfo.interlaced) && (true == pOutput->isValidVideoFormat(NEXUS_VideoFormat_e480p))) 
        {
            *pFormat = NEXUS_VideoFormat_e480p;
            goto done;
        }

        if (true == pOutput->isValidVideoFormat(NEXUS_VideoFormat_eNtsc))
        {
            *pFormat = NEXUS_VideoFormat_eNtsc;
            goto done;
        }

        if (true == pOutput->isValidVideoFormat(NEXUS_VideoFormat_eNtscJapan))
        {
            *pFormat = NEXUS_VideoFormat_eNtscJapan;
            goto done;
        }

        if (true == pOutput->isValidVideoFormat(NEXUS_VideoFormat_ePalM))
        {
            *pFormat = NEXUS_VideoFormat_eNtsc;
            goto done;
        }
    }
    else  /* Pal */
    {
        if (1080 <= status.source.height)
        {
            if (false == formatInfo.interlaced) 
            {
                *pFormat = NEXUS_VideoFormat_e1080p50hz;
                goto done;
            }
            else
            {
                *pFormat = NEXUS_VideoFormat_e1080i50hz;
                goto done;
            }
        }
        
        if (720 <= status.source.height)
        {
            if (false == formatInfo.interlaced)
            {
                *pFormat = NEXUS_VideoFormat_e720p50hz;
                goto done;
            }
        }
        
        if (576 <= status.source.height)
        {
            if ((false == formatInfo.interlaced) && (true == pOutput->isValidVideoFormat(NEXUS_VideoFormat_e576p)))
            {
                *pFormat = NEXUS_VideoFormat_e576p;
                goto done;
            }
            else
            if ((true == formatInfo.interlaced) && (true == pOutput->isValidVideoFormat(NEXUS_VideoFormat_ePal)))
            {
                *pFormat = NEXUS_VideoFormat_ePal;
                goto done;
            }
        }

        if ((false == formatInfo.interlaced) && (true == pOutput->isValidVideoFormat(NEXUS_VideoFormat_e576p))) 
        {
            *pFormat = NEXUS_VideoFormat_e576p;
            goto done;
        }

        if (true == pOutput->isValidVideoFormat(NEXUS_VideoFormat_ePal))
        {
            *pFormat = NEXUS_VideoFormat_ePal;
            goto done;
        }

        if (true == pOutput->isValidVideoFormat(NEXUS_VideoFormat_ePalN))
        {
            *pFormat = NEXUS_VideoFormat_ePalN;
            goto done;
        }

        if (true == pOutput->isValidVideoFormat(NEXUS_VideoFormat_ePalNc))
        {
            *pFormat = NEXUS_VideoFormat_ePalNc;
            goto done;
        }

        if (true == pOutput->isValidVideoFormat(NEXUS_VideoFormat_eSecam))
        {
            *pFormat = NEXUS_VideoFormat_eSecam;
            goto done;
        }
    }

error:
    /* did not find a suitable video format to match source */
    *pFormat = status.format;
    ret = eRet_NotAvailable;

done:
    return ret;
}
