#include "avtypes.h"
#include "panel_decode.h"

BDBG_MODULE(panel_decode);

CPanelDecode::CPanelDecode(bwidget_engine_t   engine, 
                           CWidgetBase      * pParentWidget, 
                           MRect              geometry,
                           bwin_font_t        font,
                           bwin_t             parentWin) :
    CPanel(engine, pParentWidget, geometry, font, parentWin),
    _pDecodeMenu(NULL),

    _Video(NULL),
    _Video_pid(NULL),
    _Video_pidLabel(NULL),
    _Video_pidValue(NULL),
    _Video_codec(NULL),
    _Video_codecLabel(NULL),
    _Video_codecValue(NULL),
    _Video_size(NULL),
    _Video_sizeLabel(NULL),
    _Video_sizeValue(NULL),
    _Video_framerate(NULL),
    _Video_framerateLabel(NULL),
    _Video_framerateValue(NULL),
    _Video_aspect(NULL),
    _Video_aspectLabel(NULL),
    _Video_aspectValue(NULL),

    _Audio(NULL),
    _Audio_pid(NULL),
    _Audio_pidLabel(NULL),
    _Audio_pidValue(NULL),
    _Audio_codec(NULL),
    _Audio_codecLabel(NULL),
    _Audio_codecValue(NULL),
    _Audio_channels(NULL),
    _Audio_channelsLabel(NULL),
    _Audio_channelsValue(NULL),
    _Audio_sample(NULL),
    _Audio_sampleLabel(NULL),
    _Audio_sampleValue(NULL),

    _Pcr(NULL),
    _Pcr_pid(NULL),
    _Pcr_pidLabel(NULL),
    _Pcr_pidValue(NULL)
{
}

CPanelDecode::~CPanelDecode()
{
    uninitialize();
}

eRet CPanelDecode::initialize(CModel * pModel, CModelConfig * pModelConfig)
{
    eRet                  ret            = eRet_Ok;
    CGraphics           * pGraphics      = NULL;
    bwin_font_t           font12;
    bwin_font_t           font14;
    uint32_t              graphicsWidth  = 0;
    uint32_t              graphicsHeight = 0;
    int                   menuWidth      = 210;
    int                   menuHeight     = 322;
    MRect                 rectPanel;

    BDBG_ASSERT(NULL != pModel);
    BDBG_ASSERT(NULL != pModelConfig);

    setModel(pModel);
    setModelConfig(pModelConfig);

    pGraphics = _pModel->getGraphics();
    BDBG_ASSERT(NULL != pGraphics);

    font12 = pGraphics->getFont(12);
    BDBG_ASSERT(NULL != font12);
    font14 = pGraphics->getFont(14);
    BDBG_ASSERT(NULL != font14);

    graphicsWidth  = GET_INT(_pCfg, GRAPHICS_SURFACE_WIDTH);
    graphicsHeight = GET_INT(_pCfg, GRAPHICS_SURFACE_HEIGHT);

    /* set the size of the panel itself */
    rectPanel.set(50, 50, menuWidth, menuHeight);
    setGeometry(rectPanel);

    _pDecodeMenu = new CWidgetMenu(getEngine(), this, MRect(0, 0, menuWidth, menuHeight), font14, font12);
    CHECK_PTR_GOTO("unable to allocate menu widget", _pDecodeMenu, ret, eRet_OutOfMemory, error);
    _pDecodeMenu->showSubTitle(false);
    _pDecodeMenu->showListView(false);
    _pDecodeMenu->showEdit(false);
    _pDecodeMenu->setMenuTitle("Decode", NULL);
    _pDecodeMenu->show(true);
    {
        _Video = new CWidgetMenu(getEngine(), _pDecodeMenu, MRect(0, 0, menuWidth, 133), font14, font12);
        CHECK_PTR_GOTO("unable to allocate menu widget", _Video, ret, eRet_OutOfMemory, error);
        _Video->showMenuBar(false);
        _Video->setMenuTitle(NULL, "Video");
        _Video->setScroll(true);
        {
            addDualLabelButton(_Video, "PID", &_Video_pid, &_Video_pidLabel, &_Video_pidValue, font12);
            _Video_pid->setFocusable(false);
            _Video_pidLabel->setText("PID:", bwidget_justify_horiz_left, bwidget_justify_vert_middle);
            _Video_pidValue->setText("", bwidget_justify_horiz_left, bwidget_justify_vert_middle);

            addDualLabelButton(_Video, "Codec", &_Video_codec, &_Video_codecLabel, &_Video_codecValue, font12);
            _Video_codec->setFocusable(false);
            _Video_codecLabel->setText("Codec:", bwidget_justify_horiz_left, bwidget_justify_vert_middle);
            _Video_codecValue->setText("", bwidget_justify_horiz_left, bwidget_justify_vert_middle);

            addDualLabelButton(_Video, "Size", &_Video_size, &_Video_sizeLabel, &_Video_sizeValue, font12);
            _Video_size->setFocusable(false);
            _Video_sizeLabel->setText("Size:", bwidget_justify_horiz_left, bwidget_justify_vert_middle);
            _Video_sizeValue->setText("", bwidget_justify_horiz_left, bwidget_justify_vert_middle);

            addDualLabelButton(_Video, "Frame Rate", &_Video_framerate, &_Video_framerateLabel, &_Video_framerateValue, font12);
            _Video_framerate->setFocusable(false);
            _Video_framerateLabel->setText("Frame Rate:", bwidget_justify_horiz_left, bwidget_justify_vert_middle);
            _Video_framerateValue->setText("", bwidget_justify_horiz_left, bwidget_justify_vert_middle);

            addDualLabelButton(_Video, "Aspect", &_Video_aspect, &_Video_aspectLabel, &_Video_aspectValue, font12);
            _Video_aspect->setFocusable(false);
            _Video_aspectLabel->setText("Aspect:", bwidget_justify_horiz_left, bwidget_justify_vert_middle);
            _Video_aspectValue->setText("", bwidget_justify_horiz_left, bwidget_justify_vert_middle);
        }

        _Audio = new CWidgetMenu(getEngine(), _pDecodeMenu, MRect(0, 0, menuWidth, 114), font14, font12);
        CHECK_PTR_GOTO("unable to allocate menu widget", _Audio, ret, eRet_OutOfMemory, error);
        _Audio->showMenuBar(false);
        _Audio->setMenuTitle(NULL, "Audio");
        _Audio->setScroll(true);
        {
            addDualLabelButton(_Audio, "PID", &_Audio_pid, &_Audio_pidLabel, &_Audio_pidValue, font12);
            _Audio_pid->setFocusable(false);
            _Audio_pidLabel->setText("PID:", bwidget_justify_horiz_left, bwidget_justify_vert_middle);
            _Audio_pidValue->setText("", bwidget_justify_horiz_left, bwidget_justify_vert_middle);

            addDualLabelButton(_Audio, "Codec", &_Audio_codec, &_Audio_codecLabel, &_Audio_codecValue, font12);
            _Audio_codec->setFocusable(false);
            _Audio_codecLabel->setText("Codec:", bwidget_justify_horiz_left, bwidget_justify_vert_middle);
            _Audio_codecValue->setText("", bwidget_justify_horiz_left, bwidget_justify_vert_middle);

            addDualLabelButton(_Audio, "Channels", &_Audio_channels, &_Audio_channelsLabel, &_Audio_channelsValue, font12);
            _Audio_channels->setFocusable(false);
            _Audio_channelsLabel->setText("Channels:", bwidget_justify_horiz_left, bwidget_justify_vert_middle);
            _Audio_channelsValue->setText("", bwidget_justify_horiz_left, bwidget_justify_vert_middle);

            addDualLabelButton(_Audio, "Sample", &_Audio_sample, &_Audio_sampleLabel, &_Audio_sampleValue, font12);
            _Audio_sample->setFocusable(false);
            _Audio_sampleLabel->setText("Sample:", bwidget_justify_horiz_left, bwidget_justify_vert_middle);
            _Audio_sampleValue->setText("", bwidget_justify_horiz_left, bwidget_justify_vert_middle);
        }

        _Pcr = new CWidgetMenu(getEngine(), _pDecodeMenu, MRect(0, 0, menuWidth, 56), font14, font12);
        CHECK_PTR_GOTO("unable to allocate menu widget", _Pcr, ret, eRet_OutOfMemory, error);
        _Pcr->showMenuBar(false);
        _Pcr->setMenuTitle(NULL, "PCR");
        _Pcr->setScroll(true);
        {
            addDualLabelButton(_Pcr, "PID", &_Pcr_pid, &_Pcr_pidLabel, &_Pcr_pidValue, font12);
            _Pcr_pid->setFocusable(false);
            _Pcr_pidLabel->setText("PID:", bwidget_justify_horiz_left, bwidget_justify_vert_middle);
            _Pcr_pidValue->setText("", bwidget_justify_horiz_left, bwidget_justify_vert_middle);
        }

        /* back button */
        _Back = new CWidgetButton(getEngine(), this, MRect(0, 0, 0, 0), font12);
        CHECK_PTR_GOTO("unable to allocate button widget", _Back, ret, eRet_OutOfMemory, error);
        _Back->setText("Menu");
        _pDecodeMenu->addBackButton(_Back);

        _Back->setFocus();
    }

    layoutInfoMenu();

    goto done;
error:
    uninitialize();
done:
    return ret;
}

void CPanelDecode::uninitialize()
{
    DEL(_Back);
    DEL(_Pcr_pidValue);
    DEL(_Pcr_pidLabel);
    DEL(_Pcr_pid);
    DEL(_Pcr);
    DEL(_Audio_sampleValue);
    DEL(_Audio_sampleLabel);
    DEL(_Audio_sample);
    DEL(_Audio_channelsValue);
    DEL(_Audio_channelsLabel);
    DEL(_Audio_channels);
    DEL(_Audio_codecValue);
    DEL(_Audio_codecLabel);
    DEL(_Audio_codec);
    DEL(_Audio_pidValue);
    DEL(_Audio_pidLabel);
    DEL(_Audio_pid);
    DEL(_Audio);
    DEL(_Video_aspectValue);
    DEL(_Video_aspectLabel);
    DEL(_Video_aspect);
    DEL(_Video_framerateValue);
    DEL(_Video_framerateLabel);
    DEL(_Video_framerate);
    DEL(_Video_sizeValue);
    DEL(_Video_sizeLabel);
    DEL(_Video_size);
    DEL(_Video_codecValue);
    DEL(_Video_codecLabel);
    DEL(_Video_codec);
    DEL(_Video_pidValue);
    DEL(_Video_pidLabel);
    DEL(_Video_pid);
    DEL(_Video);
    DEL(_pDecodeMenu);
}

void CPanelDecode::layoutInfoMenu()
{
    MRect rectInfoMenu = _pDecodeMenu->getGeometry();
    MRect rectVideo;
    MRect rectAudio;
    MRect rectPcr;

    rectVideo = _Video->getGeometry();
    rectVideo.setX(0);
    rectVideo.setY(30);
    rectVideo.setWidth(rectInfoMenu.width());
    _Video->setGeometry(rectVideo);

    rectAudio = _Audio->getGeometry();
    rectAudio.setX(0);
    rectAudio.setY(rectVideo.y() + rectVideo.height() - 7);
    rectAudio.setWidth(rectInfoMenu.width());
    _Audio->setGeometry(rectAudio);

    rectPcr = _Pcr->getGeometry();
    rectPcr.setX(0);
    rectPcr.setY(rectAudio.y() + rectAudio.height() - 7);
    rectPcr.setWidth(rectInfoMenu.width());
    _Pcr->setGeometry(rectPcr);
}

void CPanelDecode::onClick(bwidget_t widget)
{
    eRet            ret           = eRet_NotSupported;  /* assume we will not consume click */
    eNotification   notification  = eNotify_Invalid;
    CWidgetBase   * pWidget       = NULL;

    /* find the widget object that corresponds to the given bwidget_t */
    {
        blabel_settings   labelSettings;
        blabel_get(widget, &labelSettings);
        pWidget = (CWidgetBase *)labelSettings.widget.data;
    }

    if (_Back == pWidget)
    {
        show(false);
        getParent()->show(true);
    }
}

void CPanelDecode::processNotification(CNotification & notification)
{
    switch(notification.getId())
    {
    case eNotify_VideoSourceChanged:
    {
        eRet                 ret          = eRet_Ok;
        NEXUS_Error          nerror       = NEXUS_SUCCESS;
        CSimpleVideoDecode * pVideoDecode = (CSimpleVideoDecode *)notification.getData();
        CPid               * pPid         = NULL;

        NEXUS_VideoDecoderStatus nDecodeStatus;

        CHECK_PTR_GOTO("decode start notification received with invalid decoder data", pVideoDecode, ret, eRet_Ok, error);

        ret = pVideoDecode->getStatus(&nDecodeStatus);
        CHECK_ERROR_GOTO("getting video decoder status failed!", ret, error);

        pPid = pVideoDecode->getPid();
        if (pVideoDecode->isStarted() && (NULL != pPid))
        {
            char    strText[64];
            MString mstr;

            snprintf(strText, 64, "0x%X", pPid->getPid());
            _Video_pidValue->setText(strText);

            mstr = videoCodecToString(pPid->getVideoCodec());
            snprintf(strText, 64, "%s", mstr.s());
            _Video_codecValue->setText(strText);

            /* update these controls only if video decoder status is valid */
            if (0 < nDecodeStatus.aspectRatio + nDecodeStatus.source.width + nDecodeStatus.source.height)
            {
                snprintf(strText, 64, "%dx%d %c", 
                         nDecodeStatus.display.width, 
                         nDecodeStatus.display.height,
                         nDecodeStatus.interlaced ? 'i' : 'p');
                _Video_sizeValue->setText(strText);

                mstr = videoFrameRateToString(nDecodeStatus.frameRate);
                snprintf(strText, 64, "%s", mstr.s());
                _Video_framerateValue->setText(strText);

                mstr = aspectRatioToString(nDecodeStatus.aspectRatio);
                snprintf(strText, 64, "%s", mstr.s());
                _Video_aspectValue->setText(strText);
            }

            {
                CStc * pStc = pVideoDecode->getStc();
                if (NULL != pStc)
                {
                    CPid * pPidPcr = pStc->getPid();
                    snprintf(strText, 64, "0x%X", pPidPcr->getPid());
                    _Pcr_pidValue->setText(strText);
                }
            }
        }
    }
        break;

    /*case eNotify_VideoDecodeStarted:*/
    case eNotify_VideoDecodeStopped:
        _Video_pidValue->setText("");
        _Video_codecValue->setText("");
        _Video_sizeValue->setText("");
        _Video_framerateValue->setText("");
        _Video_aspectValue->setText("");
        break;

    case eNotify_AudioSourceChanged:
    {
        eRet                 ret          = eRet_Ok;
        NEXUS_Error          nerror       = NEXUS_SUCCESS;
        CSimpleAudioDecode * pAudioDecode = (CSimpleAudioDecode *)notification.getData();
        CPid               * pPid         = NULL;

        NEXUS_AudioDecoderStatus nDecodeStatus;

        CHECK_PTR_GOTO("decode start notification received with invalid decoder data", pAudioDecode, ret, eRet_Ok, error);

        ret = pAudioDecode->getStatus(&nDecodeStatus);
        CHECK_ERROR_GOTO("getting audio decoder status failed!", ret, error);

        pPid = pAudioDecode->getPid();
        if (pAudioDecode->isStarted() && (NULL != pPid))
        {
            char    strText[64];
            MString mstr;

            snprintf(strText, 64, "0x%X", pPid->getPid());
            _Audio_pidValue->setText(strText);

            mstr = audioCodecToString(pPid->getAudioCodec());
            snprintf(strText, 64, "%s", mstr.s());
            _Audio_codecValue->setText(strText);

            mstr = audioDecodeStatusToNumChannelsString(&nDecodeStatus);
            snprintf(strText, 64, "%s", mstr.s());
            _Audio_channelsValue->setText(strText);

            snprintf(strText, 64, "%d kHz", nDecodeStatus.sampleRate / 1000);
            _Audio_sampleValue->setText(strText);

            {
                CStc * pStc = pAudioDecode->getStc();
                if (NULL != pStc)
                {
                    CPid * pPidPcr = pStc->getPid();
                    snprintf(strText, 64, "0x%X", pPidPcr->getPid());
                    _Pcr_pidValue->setText(strText);
                }
            }
        }
    }
        break;

    /*case eNotify_AudioDecodeStarted:*/
    case eNotify_AudioDecodeStopped:
        _Audio_pidValue->setText("");
        _Audio_codecValue->setText("");
        _Audio_channelsValue->setText("");
        _Audio_sampleValue->setText("");

        _Pcr_pidValue->setText("PID:");
        break;

    default:
        break;
    }

error:
    return;
}

