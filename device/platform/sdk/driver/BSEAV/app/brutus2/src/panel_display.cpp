#include "panel_display.h"
#include "avtypes.h"

BDBG_MODULE(panel_display);

CPanelDisplay::CPanelDisplay(bwidget_engine_t   engine, 
                             CWidgetBase      * pParentWidget, 
                             MRect              geometry,
                             bwin_font_t        font,
                             bwin_t             parentWin) :
    CPanel(engine, pParentWidget, geometry, font, parentWin),
    _pDisplayMenu(NULL),
    _settings(NULL),
    _Format(NULL),
    _FormatLabel(NULL),
    _FormatPopup(NULL),
    _AutoFormat(NULL),
    _Color(NULL),
    _ColorLabel(NULL),
    _ColorPopup(NULL),
    _Deinterlacer(NULL),
    _BoxDetect(NULL),
    _MpaaDecimation(NULL),
    _AspectRatio(NULL),
    _AspectRatioLabel(NULL),
    _AspectRatioPopup(NULL),
    _MsgBox(NULL),
    _Back(NULL),
    _lastVideoFormat(NEXUS_VideoFormat_e1080i),
    _showFormatConfirmMsgBox(false),
    _timerMsgBox(engine, this, 7000)
{
}

CPanelDisplay::~CPanelDisplay()
{
    uninitialize();
}

eRet CPanelDisplay::initialize(CModel * pModel, CModelConfig * pModelConfig)
{
    eRet                  ret            = eRet_Ok;
    CGraphics           * pGraphics      = NULL;
    bwin_font_t           font12         = NULL;
    bwin_font_t           font14         = NULL;
    uint32_t              graphicsWidth  = 0;
    uint32_t              graphicsHeight = 0;
    int                   menuWidth      = 200;
    int                   menuHeight     = 224;
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

    _pDisplayMenu = new CWidgetMenu(getEngine(), this, MRect(0, 0, menuWidth, menuHeight), font14, font12);
    CHECK_PTR_GOTO("unable to allocate menu widget", _pDisplayMenu, ret, eRet_OutOfMemory, error);
    _pDisplayMenu->showSubTitle(false);
    _pDisplayMenu->showListView(false);
    _pDisplayMenu->showEdit(false);
    _pDisplayMenu->setMenuTitle("Display", NULL);
    _pDisplayMenu->show(true);
    {
        _settings = new CWidgetMenu(getEngine(), _pDisplayMenu, MRect(0, 30, menuWidth, 193), font14, font12);
        CHECK_PTR_GOTO("unable to allocate menu widget", _settings, ret, eRet_OutOfMemory, error);
        _settings->showMenuBar(false);
        _settings->setMenuTitle(NULL, "Settings");
        _settings->setScroll(true);
        {
            CWidgetCheckButton * pButton    = NULL;
            MRect                rectPopup;

            /* Format */
            ret = addLabelPopupButton(_settings, "Format", &_Format, &_FormatLabel, &_FormatPopup, font12, 30);
            CHECK_ERROR_GOTO("unable to allocate label popup list button", ret, error);
            _Format->setFocusable(false);
            _FormatLabel->setText("Format:", bwidget_justify_horiz_left, bwidget_justify_vert_middle);
            _FormatPopup->setText("", bwidget_justify_horiz_right, bwidget_justify_vert_middle);
            rectPopup = _FormatPopup->getGeometry();
            /* add buttons for supported formats */
            for (int i = 0; i < NEXUS_VideoFormat_eMax; i++)
            {
                CBoardFeatures * pFeatures = pModelConfig->getBoardFeatures();
                BDBG_ASSERT(NULL != pFeatures);
                if (true == pFeatures->_videoFormatIsSupported[(NEXUS_VideoFormat)i])
                {
                    /* video format supported - add button */
                    pButton = _FormatPopup->addButton(videoFormatToString((NEXUS_VideoFormat)i), rectPopup.width(), rectPopup.height());
                    CHECK_PTR_GOTO("unable to add video format button to popup list", pButton, ret, eRet_OutOfMemory, error);
                    pButton->setValue(i);
                }
            }
            _FormatPopup->sort();
            _FormatPopup->select(_lastVideoFormat);

            /* AUTO FORMAT */
            _AutoFormat = new CWidgetCheckButton(getEngine(), this, MRect(0, 0, 0, 22), font12, _pDisplayMenu->getWin());
            CHECK_PTR_GOTO("unable to allocate button widget", _AutoFormat, ret, eRet_OutOfMemory, error);
            _settings->addButton(_AutoFormat, "Auto Format");
            _AutoFormat->setText("Auto Format:", bwidget_justify_horiz_left);
            _AutoFormat->setCheck(false);

            /* COLOR */
            ret = addLabelPopupButton(_settings, "Colorspace", &_Color, &_ColorLabel, &_ColorPopup, font12);
            CHECK_ERROR_GOTO("unable to allocate label popup list button", ret, error);
            _Color->setFocusable(false);
            _ColorLabel->setText("Colorspace:", bwidget_justify_horiz_left, bwidget_justify_vert_middle);
            _ColorPopup->setText("", bwidget_justify_horiz_right, bwidget_justify_vert_middle);
            rectPopup = _ColorPopup->getGeometry();
            pButton = _ColorPopup->addButton("YPrPb", rectPopup.width(), rectPopup.height());
            CHECK_PTR_GOTO("unable to add color space YPrPb button to popup list", pButton, ret, eRet_OutOfMemory, error);
            pButton->setValue(NEXUS_ColorSpace_eYCbCr444);
            pButton = _ColorPopup->addButton("RGB", rectPopup.width(), rectPopup.height());
            CHECK_PTR_GOTO("unable to add color space RGB button to popup list", pButton, ret, eRet_OutOfMemory, error);
            pButton->setValue(NEXUS_ColorSpace_eRgb);

            /* DEINTERLACER */
            _Deinterlacer = new CWidgetCheckButton(getEngine(), this, MRect(0, 0, 0, 22), font12, _pDisplayMenu->getWin());
            CHECK_PTR_GOTO("unable to allocate button widget", _Deinterlacer, ret, eRet_OutOfMemory, error);
            _settings->addButton(_Deinterlacer, "Deinterlacer");
            _Deinterlacer->setText("Deinterlacer:", bwidget_justify_horiz_left);
            _Deinterlacer->setCheck(false);

            /* BOX DETECT */
            _BoxDetect = new CWidgetCheckButton(getEngine(), this, MRect(0, 0, 0, 22), font12, _pDisplayMenu->getWin());
            CHECK_PTR_GOTO("unable to allocate button widget", _BoxDetect, ret, eRet_OutOfMemory, error);
            _settings->addButton(_BoxDetect, "Box Detect");
            _BoxDetect->setText("Box Detect:", bwidget_justify_horiz_left);
            _BoxDetect->setCheck(true);

            /* MPAA DECIMATION */
            _MpaaDecimation = new CWidgetCheckButton(getEngine(), this, MRect(0, 0, 0, 22), font12, _pDisplayMenu->getWin());
            CHECK_PTR_GOTO("unable to allocate button widget", _MpaaDecimation, ret, eRet_OutOfMemory, error);
            _settings->addButton(_MpaaDecimation, "MPAA Decimation");
            _MpaaDecimation->setText("MPAA Decimation:", bwidget_justify_horiz_left);
            _MpaaDecimation->setCheck(false);

            /* ASPECT RATIO */
            ret = addLabelPopupButton(_settings, "Aspect Ratio", &_AspectRatio, &_AspectRatioLabel, &_AspectRatioPopup, font12, 60);
            CHECK_ERROR_GOTO("unable to allocate label popup list button", ret, error);
            _AspectRatio->setFocusable(false);
            _AspectRatioLabel->setText("Aspect Ratio:", bwidget_justify_horiz_left, bwidget_justify_vert_middle);
            _AspectRatioPopup->setText("", bwidget_justify_horiz_right, bwidget_justify_vert_middle);
            rectPopup = _AspectRatioPopup->getGeometry();
            pButton = _AspectRatioPopup->addButton("Auto", rectPopup.width(), rectPopup.height());
            CHECK_PTR_GOTO("unable to add Auto HD aspect ratio button to popup list", pButton, ret, eRet_OutOfMemory, error);
            pButton->setValue(NEXUS_DisplayAspectRatio_eAuto);
            pButton = _AspectRatioPopup->addButton("16x9", rectPopup.width(), rectPopup.height());
            CHECK_PTR_GOTO("unable to add 16x9 Auto HD aspect ratio button to popup list", pButton, ret, eRet_OutOfMemory, error);
            pButton->setValue(NEXUS_DisplayAspectRatio_e16x9);
            pButton = _AspectRatioPopup->addButton("4x3", rectPopup.width(), rectPopup.height());
            CHECK_PTR_GOTO("unable to add 4x3 Auto HD aspect ratio button to popup list", pButton, ret, eRet_OutOfMemory, error);
            pButton->setValue(NEXUS_DisplayAspectRatio_e4x3);
        }

        /* back button */
        _Back = new CWidgetButton(getEngine(), this, MRect(0, 0, 0, 0), font12);
        CHECK_PTR_GOTO("unable to allocate button widget", _Back, ret, eRet_OutOfMemory, error);
        _Back->setText("Menu");
        _pDisplayMenu->addBackButton(_Back);

        {
            MRect    geom;
            uint32_t msgBoxWidth  = 340;
            uint32_t msgBoxHeight = 100;

            /* center msg box on screen - note that we will set the msg box bwin parent to be the framebuffer
               so it can draw outside the borders of it's parent widget (this) */
            geom.setX(graphicsWidth / 2 - msgBoxWidth / 2);
            geom.setY(graphicsHeight / 2 - msgBoxHeight / 2);
            geom.setWidth(msgBoxWidth);
            geom.setHeight(msgBoxHeight);

            _MsgBox = new CWidgetModalMsgBox(getEngine(), this, geom, font12);
            CHECK_PTR_GOTO("unable to allocate MsgBox widget", _MsgBox, ret, eRet_OutOfMemory, error);
            _MsgBox->setText("Sample Text", bwidget_justify_horiz_center, bwidget_justify_vert_middle);
            _MsgBox->show(false);
        }

        _Back->setFocus();
    }

    goto done;
error:
    uninitialize();
done:
    return ret;
}

void CPanelDisplay::uninitialize()
{
    DEL(_MsgBox);
    DEL(_Back);
    DEL(_AspectRatioPopup);
    DEL(_AspectRatioLabel);
    DEL(_AspectRatio);
    DEL(_MpaaDecimation);
    DEL(_BoxDetect);
    DEL(_Deinterlacer);
    DEL(_ColorPopup);
    DEL(_ColorLabel);
    DEL(_Color);
    DEL(_AutoFormat);
    DEL(_FormatPopup);
    DEL(_FormatLabel);
    DEL(_Format);
    DEL(_settings);
    DEL(_pDisplayMenu);
}

void CPanelDisplay::onClick(bwidget_t widget)
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

    if (0 <= _FormatPopup->getItemListIndex(pWidget->getWidget()))
    {
        CWidgetCheckButton * pButton     = (CWidgetCheckButton *)pWidget;
        NEXUS_VideoFormat    videoFormat = (NEXUS_VideoFormat)pWidget->getValue();

        _showFormatConfirmMsgBox = true;
        BDBG_WRN(("selected video format:%s", pButton->getText().s()));
        notifyObservers(eNotify_SetVideoFormat, &videoFormat);
    }
    if (_AutoFormat == pWidget)
    {
        bool                 bAutoFormat = _AutoFormat->isChecked();
        CDisplay           * pDisplay    = _pModel->getDisplay();
        NEXUS_VideoFormat    videoFormat = NEXUS_VideoFormat_eUnknown;

        notifyObservers(eNotify_SetAutoVideoFormat, &bAutoFormat);

        if (true == bAutoFormat)
        {
            _MsgBox->showModal("Auto format takes effect on channel change", 0, "Ok", NULL, NULL);
            _AutoFormat->setFocus();
        }
    }
    else
    if (0 <= _ColorPopup->getItemListIndex(pWidget->getWidget()))
    {
        NEXUS_ColorSpace   colorSpace = (NEXUS_ColorSpace)pWidget->getValue();
        CDisplay         * pDisplay   = _pModel->getDisplay();

        notifyObservers(eNotify_SetColorSpace, &colorSpace);
    }
    else
    if (_MpaaDecimation == pWidget)
    {
        bool       bMpaaDecimation = _MpaaDecimation->isChecked();
        CDisplay * pDisplay        = _pModel->getDisplay();

        notifyObservers(eNotify_SetMpaaDecimation, &bMpaaDecimation);
    }
    else
    if (_Deinterlacer == pWidget)
    {
        bool       bDeinterlacer = _Deinterlacer->isChecked();
        CDisplay * pDisplay     = _pModel->getDisplay();

        notifyObservers(eNotify_SetDeinterlacer, &bDeinterlacer);
    }
    else
    if (_BoxDetect == pWidget)
    {
        bool       bBoxDetect = _BoxDetect->isChecked();
        CDisplay * pDisplay   = _pModel->getDisplay();

        notifyObservers(eNotify_SetBoxDetect, &bBoxDetect);
    }
    else
    if (0 <= _AspectRatioPopup->getItemListIndex(pWidget->getWidget()))
    {
        NEXUS_DisplayAspectRatio   aspectRatio = (NEXUS_DisplayAspectRatio)pWidget->getValue();
        CDisplay                 * pDisplay    = _pModel->getDisplay();
        
        notifyObservers(eNotify_SetAspectRatio, &aspectRatio);
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

void CPanelDisplay::processNotification(CNotification & notification)
{
    BDBG_MSG(("CPanelDisplay::processNotification():%d", notification.getId()));

    switch (notification.getId())
    {
    case eNotify_Timeout:
    {
        CTimer * pTimer = (CTimer *)notification.getData();
        if (&_timerMsgBox == pTimer)
        {
            /* _MsgBox did not get a response from user so we'll timeout
               here and select the "cancel" option */
            _MsgBox->cancelModal("Cancel");
        }
    }
        break;

    case eNotify_VideoFormatChanged:
        {
            CDisplay          * pDisplay     = getModel()->getDisplay();
            NEXUS_VideoFormat * pVideoFormat = (NEXUS_VideoFormat *)notification.getData();

            _FormatPopup->select(*pVideoFormat);

            if (true == _showFormatConfirmMsgBox)
            {
                _timerMsgBox.start(7000);
                if (_MsgBox->showModal("Keep new Video Format?", 1) != "Ok")
                {
                    _timerMsgBox.stop();

                    /* revert to old video format - remember we are reverting so we
                       don't show the modal dialog next time we are notified of a
                       video format change. */
                    _showFormatConfirmMsgBox = false;

                    notifyObservers(eNotify_SetVideoFormat, &_lastVideoFormat);
                }

                _showFormatConfirmMsgBox = false;
                _FormatPopup->setFocus();
            }

            /* save current video format */
            _lastVideoFormat = pDisplay->getFormat();
        }
        break;

    case eNotify_AutoVideoFormatChanged:
        {
            CDisplay * pDisplay         = getModel()->getDisplay();
            bool     * pAutoVideoFormat = (bool *)notification.getData();

            _AutoFormat->setCheck(*pAutoVideoFormat);
        }
        break;

    case eNotify_ColorSpaceChanged:
        {
            NEXUS_ColorSpace * pColorSpace = (NEXUS_ColorSpace *)notification.getData();
            _ColorPopup->select(*pColorSpace);
        }
        break;
    
    case eNotify_MpaaDecimationChanged:
        {
            bool * pMpaaDecimation = (bool *)notification.getData();
            _MpaaDecimation->setCheck(*pMpaaDecimation);
        }
        break;
    
    case eNotify_DeinterlacerChanged:
        {
            bool * pDeinterlacer = (bool *)notification.getData();
            _Deinterlacer->setCheck(*pDeinterlacer);
        }
        break;
    
    case eNotify_BoxDetectChanged:
        {
            bool * pBoxDetect = (bool *)notification.getData();
            _BoxDetect->setCheck(*pBoxDetect);
        }
        break;
    
    case eNotify_AspectRatioChanged:
        {
            NEXUS_DisplayAspectRatio * pAspectRatio = (NEXUS_DisplayAspectRatio *)notification.getData();
            _AspectRatioPopup->select(*pAspectRatio);
        }
        break;
    }

error:
    return;
}

