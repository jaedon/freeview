#include "avtypes.h"
#include "panel_audio.h"
#include "channelmgr.h"
#include "channel.h"

BDBG_MODULE(panel_audio);

CPanelAudio::CPanelAudio(bwidget_engine_t   engine, 
                         CWidgetBase      * pParentWidget, 
                         MRect              geometry,
                         bwin_font_t        font,
                         bwin_t             parentWin) :
    CPanel(engine, pParentWidget, geometry, font, parentWin),
    _pAudioMenu(NULL),
    _settings(NULL),
    _dialogNorm(NULL),
    _Pid(NULL),
    _PidLabel(NULL),
    _PidPopup(NULL),
    _Spdif(NULL),
    _SpdifLabel(NULL),
    _SpdifPopup(NULL),
    _Hdmi(NULL),
    _HdmiLabel(NULL),
    _HdmiPopup(NULL),
    _Downmix(NULL),
    _DownmixLabel(NULL),
    _DownmixPopup(NULL),
    _DualMono(NULL),
    _DualMonoLabel(NULL),
    _DualMonoPopup(NULL),
    _Back(NULL)
{
}

CPanelAudio::~CPanelAudio()
{
    uninitialize();
}

eRet CPanelAudio::initialize(CModel * pModel, CModelConfig * pModelConfig)
{
    eRet                  ret            = eRet_Ok;
    CGraphics           * pGraphics      = NULL;
    bwin_font_t           font12         = NULL;
    bwin_font_t           font14         = NULL;
    uint32_t              graphicsWidth  = 0;
    uint32_t              graphicsHeight = 0;
    int                   menuWidth      = 200;
    int                   menuHeight     = 218;
    MRect                 rectPanel;

    BDBG_ASSERT(NULL != pModel);
    BDBG_ASSERT(NULL != pModelConfig);

    setModel(pModel);
    setModelConfig(pModelConfig);

    pGraphics = pModel->getGraphics();
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

    _pAudioMenu = new CWidgetMenu(getEngine(), this, MRect(0, 0, menuWidth, menuHeight), font14, font12);
    CHECK_PTR_GOTO("unable to allocate menu widget", _pAudioMenu, ret, eRet_OutOfMemory, error);
    _pAudioMenu->showSubTitle(false);
    _pAudioMenu->showListView(false);
    _pAudioMenu->showEdit(false);
    _pAudioMenu->setMenuTitle("Audio", NULL);
    _pAudioMenu->show(true);
    {
        _settings = new CWidgetMenu(getEngine(), _pAudioMenu, MRect(0, 30, menuWidth, 187), font14, font12);
        CHECK_PTR_GOTO("unable to allocate menu widget", _settings, ret, eRet_OutOfMemory, error);
        _settings->showMenuBar(false);
        _settings->setMenuTitle(NULL, "Settings");
        _settings->setScroll(true);
        {
            CWidgetCheckButton * pButton    = NULL;
            MRect                rectPopup;

            /* PID */
            ret = addLabelPopupButton(_settings, "Pid", &_Pid, &_PidLabel, &_PidPopup, font12, 25);
            CHECK_ERROR_GOTO("unable to allocate label popup list button", ret, error);
            _Pid->setFocusable(false);
            _PidLabel->setText("Pid:", bwidget_justify_horiz_left, bwidget_justify_vert_middle);
            _PidPopup->setText("", bwidget_justify_horiz_right, bwidget_justify_vert_middle);
            /* buttons are stream dependent and will be populated in source changed callback */

            /* SPDIF */
            ret = addLabelPopupButton(_settings, "Spdif", &_Spdif, &_SpdifLabel, &_SpdifPopup, font12, 35);
            CHECK_ERROR_GOTO("unable to allocate label popup list button", ret, error);
            _Spdif->setFocusable(false);
            _SpdifLabel->setText("Spdif:", bwidget_justify_horiz_left, bwidget_justify_vert_middle);
            _SpdifPopup->setText("", bwidget_justify_horiz_right, bwidget_justify_vert_middle);
            rectPopup = _SpdifPopup->getGeometry();
            pButton = _SpdifPopup->addButton("Pcm", rectPopup.width(), rectPopup.height());
            CHECK_PTR_GOTO("unable to add Pcm button to popup list", pButton, ret, eRet_OutOfMemory, error);
            pButton->setValue(eSpdifInput_Pcm);
            pButton = _SpdifPopup->addButton("Compressed", rectPopup.width(), rectPopup.height());
            CHECK_PTR_GOTO("unable to add Compressed button to popup list", pButton, ret, eRet_OutOfMemory, error);
            pButton->setValue(eSpdifInput_Compressed);
            pButton = _SpdifPopup->addButton("Encode DTS", rectPopup.width(), rectPopup.height());
            CHECK_PTR_GOTO("unable to add DTS button to popup list", pButton, ret, eRet_OutOfMemory, error);
            pButton->setValue(eSpdifInput_EncodeDts);
            pButton = _SpdifPopup->addButton("Encode AC-3", rectPopup.width(), rectPopup.height());
            CHECK_PTR_GOTO("unable to add AC-3 button to popup list", pButton, ret, eRet_OutOfMemory, error);
            pButton->setValue(eSpdifInput_EncodeAc3);

            /* HDMI */
            ret = addLabelPopupButton(_settings, "Hdmi", &_Hdmi, &_HdmiLabel, &_HdmiPopup, font12, 35);
            CHECK_ERROR_GOTO("unable to allocate label popup list button", ret, error);
            _Hdmi->setFocusable(false);
            _HdmiLabel->setText("Hdmi:", bwidget_justify_horiz_left, bwidget_justify_vert_middle);
            _HdmiPopup->setText("", bwidget_justify_horiz_right, bwidget_justify_vert_middle);
            rectPopup = _HdmiPopup->getGeometry();
            pButton = _HdmiPopup->addButton("Pcm", rectPopup.width(), rectPopup.height());
            CHECK_PTR_GOTO("unable to add Pcm button to popup list", pButton, ret, eRet_OutOfMemory, error);
            pButton->setValue(eHdmiAudioInput_Pcm);
            pButton = _HdmiPopup->addButton("Compressed", rectPopup.width(), rectPopup.height());
            CHECK_PTR_GOTO("unable to add Compressed button to popup list", pButton, ret, eRet_OutOfMemory, error);
            pButton->setValue(eHdmiAudioInput_Compressed);
            pButton = _HdmiPopup->addButton("Pcm 5.1", rectPopup.width(), rectPopup.height());
            CHECK_PTR_GOTO("unable to add Pcm 5.1 button to popup list", pButton, ret, eRet_OutOfMemory, error);
            pButton->setValue(eHdmiAudioInput_Multichannel);
            pButton = _HdmiPopup->addButton("Encode DTS", rectPopup.width(), rectPopup.height());
            CHECK_PTR_GOTO("unable to add DTS button to popup list", pButton, ret, eRet_OutOfMemory, error);
            pButton->setValue(eHdmiAudioInput_EncodeDts);
            pButton = _HdmiPopup->addButton("Encode AC-3", rectPopup.width(), rectPopup.height());
            CHECK_PTR_GOTO("unable to add AC-3 button to popup list", pButton, ret, eRet_OutOfMemory, error);
            pButton->setValue(eHdmiAudioInput_EncodeAc3);

            /* DOWNMIX */
            ret = addLabelPopupButton(_settings, "Downmix", &_Downmix, &_DownmixLabel, &_DownmixPopup, font12, 35);
            CHECK_ERROR_GOTO("unable to allocate label popup list button", ret, error);
            _Downmix->setFocusable(false);
            _DownmixLabel->setText("Downmix:", bwidget_justify_horiz_left, bwidget_justify_vert_middle);
            _DownmixPopup->setText("", bwidget_justify_horiz_right, bwidget_justify_vert_middle);
            /* buttons are codec dependent and will be populated in source changed callback */

            /* DUAL MONO */
            ret = addLabelPopupButton(_settings, "DualMono", &_DualMono, &_DualMonoLabel, &_DualMonoPopup, font12, 45);
            CHECK_ERROR_GOTO("unable to allocate label popup list button", ret, error);
            _DualMono->setFocusable(false);
            _DualMonoLabel->setText("DualMono:", bwidget_justify_horiz_left, bwidget_justify_vert_middle);
            _DualMonoPopup->setText("", bwidget_justify_horiz_right, bwidget_justify_vert_middle);
            rectPopup = _DualMonoPopup->getGeometry();
            pButton = _DualMonoPopup->addButton("Left", rectPopup.width(), rectPopup.height());
            CHECK_PTR_GOTO("unable to add Left button to popup list", pButton, ret, eRet_OutOfMemory, error);
            pButton->setValue(eAudioDualMono_Left);
            pButton = _DualMonoPopup->addButton("Right", rectPopup.width(), rectPopup.height());
            CHECK_PTR_GOTO("unable to add Right button to popup list", pButton, ret, eRet_OutOfMemory, error);
            pButton->setValue(eAudioDualMono_Right);
            pButton = _DualMonoPopup->addButton("Stereo", rectPopup.width(), rectPopup.height());
            CHECK_PTR_GOTO("unable to add Stereo button to popup list", pButton, ret, eRet_OutOfMemory, error);
            pButton->setValue(eAudioDualMono_Stereo);
            pButton = _DualMonoPopup->addButton("MonoMix", rectPopup.width(), rectPopup.height());
            CHECK_PTR_GOTO("unable to add MonoMix button to popup list", pButton, ret, eRet_OutOfMemory, error);
            pButton->setValue(eAudioDualMono_Monomix);

            /* COMPRESSION */
            ret = addLabelPopupButton(_settings, "Compression", &_Compression, &_CompressionLabel, &_CompressionPopup, font12);
            CHECK_ERROR_GOTO("unable to allocate label popup list button", ret, error);
            _Compression->setFocusable(false);
            _CompressionLabel->setText("Compression:", bwidget_justify_horiz_left, bwidget_justify_vert_middle);
            _CompressionPopup->setText("", bwidget_justify_horiz_right, bwidget_justify_vert_middle);
            rectPopup = _CompressionPopup->getGeometry();
            pButton = _CompressionPopup->addButton("None", rectPopup.width(), rectPopup.height());
            CHECK_PTR_GOTO("unable to add None button to popup list", pButton, ret, eRet_OutOfMemory, error);
            pButton->setValue(eDolbyDRC_None);
            pButton = _CompressionPopup->addButton("Heavy", rectPopup.width(), rectPopup.height());
            CHECK_PTR_GOTO("unable to add Heavy button to popup list", pButton, ret, eRet_OutOfMemory, error);
            pButton->setValue(eDolbyDRC_Heavy);
            pButton = _CompressionPopup->addButton("Medium", rectPopup.width(), rectPopup.height());
            CHECK_PTR_GOTO("unable to add Medium button to popup list", pButton, ret, eRet_OutOfMemory, error);
            pButton->setValue(eDolbyDRC_Medium);

            /* DIALOG NORMALIZATION */
            _dialogNorm = new CWidgetCheckButton(getEngine(), this, MRect(0, 0, 0, 22), font12, _pAudioMenu->getWin());
            CHECK_PTR_GOTO("unable to allocate button widget", _dialogNorm, ret, eRet_OutOfMemory, error);
            _settings->addButton(_dialogNorm, "Dialog Normalization");
            _dialogNorm->setText("Dialog Normalization:", bwidget_justify_horiz_left);
            _dialogNorm->setCheck(true);
        }

        /* back button */
        _Back = new CWidgetButton(getEngine(), this, MRect(0, 0, 0, 0), font12);
        CHECK_PTR_GOTO("unable to allocate button widget", _Back, ret, eRet_OutOfMemory, error);
        _Back->setText("Menu");
        _pAudioMenu->addBackButton(_Back);

        _Back->setFocus();
    }

    goto done;
error:
    uninitialize();
done:
    return ret;
}

void CPanelAudio::uninitialize()
{
    DEL(_Back);
    DEL(_dialogNorm);
    DEL(_CompressionPopup);
    DEL(_CompressionLabel);
    DEL(_Compression);
    DEL(_DualMonoPopup);
    DEL(_DualMonoLabel);
    DEL(_DualMono);
    DEL(_DownmixPopup);
    DEL(_DownmixLabel);
    DEL(_Downmix);
    DEL(_HdmiPopup);
    DEL(_HdmiLabel);
    DEL(_Hdmi);
    DEL(_SpdifPopup);
    DEL(_SpdifLabel);
    DEL(_Spdif);
    DEL(_PidPopup);
    DEL(_PidLabel);
    DEL(_Pid);
    DEL(_settings);
    DEL(_pAudioMenu);
}

void CPanelAudio::onClick(bwidget_t widget)
{
    eNotification   notification  = eNotify_Invalid;
    CWidgetBase   * pWidget       = NULL;
    CModel        * pModel        = getModel();

    BDBG_ASSERT(NULL != pModel);

    /* find the widget object that corresponds to the given bwidget_t */
    {
        blabel_settings   labelSettings;
        blabel_get(widget, &labelSettings);
        pWidget = (CWidgetBase *)labelSettings.widget.data;
    }

    if (0 <= _PidPopup->getItemListIndex(pWidget->getWidget()))
    {
        uint32_t pid = ((CWidgetCheckButton *)pWidget)->getValue();

        BDBG_WRN(("selected audio pid:0x%x", pid));
        notifyObservers(eNotify_SetAudioProgram, &pid);
    }
    else
    if (0 <= _SpdifPopup->getItemListIndex(pWidget->getWidget()))
    {
        CWidgetCheckButton * pButton    = (CWidgetCheckButton *)pWidget;
        eSpdifInput          spdifInput = (eSpdifInput)pButton->getValue();

        BDBG_WRN(("selected audio spdif input:%s", pButton->getText().s()));
        notifyObservers(eNotify_SetSpdifInput, &spdifInput);
    }
    else
    if (0 <= _HdmiPopup->getItemListIndex(pWidget->getWidget()))
    {
        CWidgetCheckButton * pButton = (CWidgetCheckButton *)pWidget;
        eHdmiAudioInput      hdmiInput = (eHdmiAudioInput)pButton->getValue();

        BDBG_WRN(("selected audio hdmi input:%s (%d)", pButton->getText().s(), hdmiInput));
        notifyObservers(eNotify_SetHdmiAudioInput, &hdmiInput);
    }
    else
    if (0 <= _DownmixPopup->getItemListIndex(pWidget->getWidget()))
    {
        CWidgetCheckButton * pButton = (CWidgetCheckButton *)pWidget;
        eAudioDownmix        downmix = (eAudioDownmix)pButton->getValue();

        BDBG_WRN(("selected audio Downmix:%s(%d)", pButton->getText().s(), downmix));
        notifyObservers(eNotify_SetAudioDownmix, &downmix);
    }
    else
    if (0 <= _DualMonoPopup->getItemListIndex(pWidget->getWidget()))
    {
        CWidgetCheckButton * pButton  = (CWidgetCheckButton *)pWidget;
        eAudioDualMono       dualMono = (eAudioDualMono)pButton->getValue();
        
        BDBG_WRN(("selected audio DualMono:%s", pButton->getText().s()));
        notifyObservers(eNotify_SetAudioDualMono, &dualMono);
    }
    else
    if (0 <= _CompressionPopup->getItemListIndex(pWidget->getWidget()))
    {
        CWidgetCheckButton * pButton = (CWidgetCheckButton *)pWidget;
        eDolbyDRC       dolbyDRC = (eDolbyDRC)pButton->getValue();

        BDBG_WRN(("selected Dolby DRC:%s", pButton->getText().s()));
        notifyObservers(eNotify_SetDolbyDRC, &dolbyDRC);
    }
    else
    if (_dialogNorm == pWidget)
    {
        bool dialogNorm = _dialogNorm->isChecked();

        BDBG_WRN(("dialog normalization:%d", dialogNorm));
        notifyObservers(eNotify_SetDolbyDialogNorm, &dialogNorm);
    }
    else
    if (_Back == pWidget)
    {
        show(false);
        getParent()->show(true);
    }

error:
    return;
}

void CPanelAudio::processNotification(CNotification & notification)
{
    BDBG_MSG(("CPanelAudio::processNotification():%d", notification.getId()));

    switch (notification.getId())
    {
    case eNotify_AudioSourceChanged:
        {
            CSimpleAudioDecode       * pAudioDecode     = (CSimpleAudioDecode *)notification.getData();
            NEXUS_AudioDecoderStatus   status;
            CModelChannelMgr         * pModelChannelMgr = NULL;
            NEXUS_Error                nerror           = NEXUS_SUCCESS;
            eRet                       ret              = eRet_Ok;

            BDBG_ASSERT(NULL != pAudioDecode);

            ret = pAudioDecode->getStatus(&status);
            CHECK_ERROR_GOTO("unable to get Audio decoder status", ret, error);

            pModelChannelMgr = _pModel->getModelChannelMgr();
            CHECK_PTR_GOTO("unable to get channel mgr", pModelChannelMgr, ret, eRet_NotAvailable, error);

            /* add new pid values in menu */
            {
                CPidMgr            * pPidMgr   = NULL;
                MRect                rect      = _PidPopup->getGeometry();

                _PidPopup->popup(false);
                _PidPopup->clearButtons();

                /* get pidmgr based on current mode */
                switch (_pModel->getMode())
                {
                case eMode_Live:
                    pPidMgr = pModelChannelMgr->getCurrentChannel()->getPidMgr();
                    break;

                case eMode_Playback:
                    pPidMgr = _pModel->getPlayback()->getVideo()->getPidMgr();
                    break;

                default:
                    break;
                }
                CHECK_PTR_GOTO("unable to get pidmgr", pPidMgr, ret, eRet_NotAvailable, error);

                {
                    CPid * pPidAudio = NULL;
                    int    i         = 0;

                    while (NULL != (pPidAudio = pPidMgr->getPid(i, ePidType_Audio)))
                    {
                        CWidgetCheckButton * pButton     = NULL;
                        char                 strPid[16];

                        sprintf(strPid, "0x%x (%s)", pPidAudio->getPid(), audioCodecToString(pPidAudio->getAudioCodec()).s());

                        pButton = _PidPopup->addButton(strPid, rect.width(), rect.height());
                        CHECK_PTR_GOTO("unable to add pid button to popup list", pButton, ret, eRet_OutOfMemory, error);
                        pButton->setValue(pPidAudio->getPid());

                        if (pPidAudio == pAudioDecode->getPid())
                        {
                            _PidPopup->select(pButton);
                        }

                        i++;
                    }
                }
            }

            /* update spdif setting */
            _SpdifPopup->select(pAudioDecode->getSpdifInputDecoderType());

            /* update hdmi setting */
            _HdmiPopup->select(pAudioDecode->getHdmiInputDecoderType());

            /* add codec dependent downmix values to menu */
            {
                CWidgetCheckButton * pButton         = NULL;
                MRect                rect;

                _DownmixPopup->popup(false);
                _DownmixPopup->clearButtons();

                rect = _DownmixPopup->getGeometry();

                switch (status.codec)
                {
                case NEXUS_AudioCodec_eAac:
                case NEXUS_AudioCodec_eAacLoas:
                case NEXUS_AudioCodec_eAacPlus:
                case NEXUS_AudioCodec_eAacPlusAdts:
                    pButton = _DownmixPopup->addButton("Matrix", rect.width(), rect.height());
                    CHECK_PTR_GOTO("unable to add Matrix button to popup list", pButton, ret, eRet_OutOfMemory, error);
                    pButton->setValue(eAudioDownmix_Matrix);

                    pButton = _DownmixPopup->addButton("Arib", rect.width(), rect.height());
                    CHECK_PTR_GOTO("unable to add Arib button to popup list", pButton, ret, eRet_OutOfMemory, error);
                    pButton->setValue(eAudioDownmix_Arib);

                    pButton = _DownmixPopup->addButton("LeftRight", rect.width(), rect.height());
                    CHECK_PTR_GOTO("unable to add LeftRight button to popup list", pButton, ret, eRet_OutOfMemory, error);
                    pButton->setValue(eAudioDownmix_LtRt);

                    break;

                default:
                    pButton = _DownmixPopup->addButton("None", rect.width(), rect.height());
                    CHECK_PTR_GOTO("unable to add None button to popup list", pButton, ret, eRet_OutOfMemory, error);
                    pButton->setValue(eAudioDownmix_None);

                    pButton = _DownmixPopup->addButton("Left", rect.width(), rect.height());
                    CHECK_PTR_GOTO("unable to add Left button to popup list", pButton, ret, eRet_OutOfMemory, error);
                    pButton->setValue(eAudioDownmix_Left);

                    pButton = _DownmixPopup->addButton("Right", rect.width(), rect.height());
                    CHECK_PTR_GOTO("unable to add Right button to popup list", pButton, ret, eRet_OutOfMemory, error);
                    pButton->setValue(eAudioDownmix_Right);

                    pButton = _DownmixPopup->addButton("Mono", rect.width(), rect.height());
                    CHECK_PTR_GOTO("unable to add Mono button to popup list", pButton, ret, eRet_OutOfMemory, error);
                    pButton->setValue(eAudioDownmix_Monomix);

                    break;
                }

                _DownmixPopup->select(pAudioDecode->getDownmix());
            }

            /* update dualmono setting */
            _DualMonoPopup->select(pAudioDecode->getDualMono());

            /* update dolby DRC setting */
            _CompressionPopup->select(pAudioDecode->getDolbyDRC());

            /* update dolby dialog normalization setting */
            _dialogNorm->setCheck(pAudioDecode->getDolbyDialogNorm());
        }
        break;

    default:
        break;
    }

error:
    return;
}

