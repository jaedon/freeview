#include "avtypes.h"
#include "nexus_video_decoder.h"
#include "panel_buffers.h"

BDBG_MODULE(panel_buffers);

CPanelBuffers::CPanelBuffers(bwidget_engine_t   engine, 
                             CWidgetBase      * pParentWidget, 
                             MRect              geometry,
                             bwin_font_t        font,
                             bwin_t             parentWin) :
    CPanel(engine, pParentWidget, geometry, font, parentWin),
    _pBuffersMenu(NULL),
    _Bkgnd(NULL),
    _LabelSeparator1(NULL),
    _VideoMeter(NULL),
    _VideoLabel(NULL),
    _VideoBufferLabel(NULL),
    _VideoValue(NULL),
    _LabelSeparator2(NULL),
    _VideoPtsStcMinLabel(NULL),
    _VideoPtsStcMaxLabel(NULL),
    _VideoPtsStcLabel(NULL),
    _VideoPtsStcMeter(NULL),
    _AudioMeter(NULL),
    _AudioLabel(NULL),
    _AudioBufferLabel(NULL),
    _AudioValue(NULL),
    _LabelSeparator3(NULL),
    _AudioPtsStcMinLabel(NULL),
    _AudioPtsStcMaxLabel(NULL),
    _AudioPtsStcLabel(NULL),
    _AudioPtsStcMeter(NULL),
    _PlaybackLabel(NULL),
    _PlaybackMeter(NULL),
    _PlaybackBufferLabel(NULL),
    _PlaybackValue(NULL),
    _LabelSeparator4(NULL),
    _Back(NULL),
    _graphicsWidth(0),
    _graphicsHeight(0),
    _timerBuffers(engine, this, 333)
{
}

CPanelBuffers::~CPanelBuffers()
{
    uninitialize();
}

eRet CPanelBuffers::initialize(CModel * pModel, CModelConfig * pModelConfig)
{
    eRet                  ret            = eRet_Ok;
    CGraphics           * pGraphics      = NULL;
    bwin_font_t           font10;
    MRect                 rectMenu;
    MRect                 rectBkgnd;
    MRect                 rectLabel;
    MRect                 rectValue;
    MRect                 rectVideo;
    MRect                 rectAudio;
    MRect                 rectPlayback;
    MRect                 rectPtsStcMin;
    MRect                 rectPtsStcMax;
    MRect                 rectPtsStc;
    MRect                 rectMeter;
    int                   marginBkgnd    = 1; 
    int                   menuWidth      = 400;
    int                   menuHeight     = 25;
    MRect                 rectPanel;

    BDBG_ASSERT(NULL != pModel);
    BDBG_ASSERT(NULL != pModelConfig);

    setModel(pModel);
    setModelConfig(pModelConfig);

    pGraphics = _pModel->getGraphics();
    BDBG_ASSERT(NULL != pGraphics);

    font10 = pGraphics->getFont(10);
    BDBG_ASSERT(NULL != font10);

    _graphicsWidth  = GET_INT(_pCfg, GRAPHICS_SURFACE_WIDTH);
    _graphicsHeight = GET_INT(_pCfg, GRAPHICS_SURFACE_HEIGHT);

    /* set the size of the panel itself */
    rectPanel.set((_graphicsWidth / 2) - (menuWidth / 2), _graphicsHeight - (menuHeight / 2), menuWidth, menuHeight);
    rectPanel.setX(rectPanel.x() - marginBkgnd * 2);
    rectPanel.setY(rectPanel.y() - marginBkgnd * 2);
    rectPanel.grow(marginBkgnd * 2);
    setGeometry(rectPanel);

    /* start with rect sized to fit contents then expand for background border */
    rectMenu = rectPanel;
    rectMenu.setX(0);
    rectMenu.setY(0);
    _pBuffersMenu = new CWidgetLabel(getEngine(), this, rectMenu, font10);
    CHECK_PTR_GOTO("unable to allocate menu widget", _pBuffersMenu, ret, eRet_OutOfMemory, error);
    _pBuffersMenu->setBevel(0);
    _pBuffersMenu->setBackgroundColor(0xFF222222);
    _pBuffersMenu->show(true);

    /* shrink rect back to contents size */
    rectBkgnd = rectMenu;
    rectBkgnd.setX(marginBkgnd);
    rectBkgnd.setY(marginBkgnd);
    rectBkgnd.grow(marginBkgnd * -2);
    _Bkgnd = new CWidgetLabel(getEngine(), _pBuffersMenu, rectBkgnd, font10);
    CHECK_PTR_GOTO("unable to allocate menu widget", _Bkgnd, ret, eRet_OutOfMemory, error);
    {
        int menuBevel      = _Bkgnd->getBevel();
        int avLabelWidth   = 55;
        int progressHeight = 5;
        int labelWidth     = 50;
        int meterWidth     = 160;
        int x              = menuBevel;
        int y              = menuBevel * 2;

        /* video label */
        rectLabel.set(x, y, avLabelWidth, rectBkgnd.height() - menuBevel * 2 - progressHeight);
        _VideoLabel = new CWidgetLabel(getEngine(), _Bkgnd, rectLabel, font10);
        CHECK_PTR_GOTO("unable to allocate menu widget", _VideoLabel, ret, eRet_OutOfMemory, error);
        _VideoLabel->setText("Video:", bwidget_justify_horiz_left, bwidget_justify_vert_bottom);
        _VideoLabel->setBevel(0);

        x += avLabelWidth;

        /* separator */
        _LabelSeparator1 = new CWidgetLabel(getEngine(), _Bkgnd, MRect(x, menuBevel, 1, rectBkgnd.height() * 3), font10);
        CHECK_PTR_GOTO("unable to allocate label widget", _LabelSeparator1, ret, eRet_OutOfMemory, error);
        _LabelSeparator1->setBevel(1);

        x += 1;

        /* video buffer data */
        rectLabel.set(x, y, labelWidth, rectBkgnd.height() - menuBevel * 2 - progressHeight);
        _VideoBufferLabel = new CWidgetLabel(getEngine(), _Bkgnd, rectLabel, font10);
        CHECK_PTR_GOTO("unable to allocate menu widget", _VideoBufferLabel, ret, eRet_OutOfMemory, error);
        _VideoBufferLabel->setText("Buffer", bwidget_justify_horiz_left, bwidget_justify_vert_bottom);
        _VideoBufferLabel->setBevel(0);

        rectValue.set(x + labelWidth, y, 75, rectBkgnd.height() - menuBevel * 2 - progressHeight);
        _VideoValue = new CWidgetLabel(getEngine(), _Bkgnd, rectValue, font10);
        CHECK_PTR_GOTO("unable to allocate menu widget", _VideoValue, ret, eRet_OutOfMemory, error);
        _VideoValue->setText("0/0K", bwidget_justify_horiz_right, bwidget_justify_vert_bottom);
        _VideoValue->setBevel(0);

        rectVideo.set(x, y + rectLabel.height(), rectLabel.width() + rectValue.width(), progressHeight);
        _VideoMeter = new CWidgetProgress(getEngine(), _Bkgnd, rectVideo, font10);
        CHECK_PTR_GOTO("unable to allocate menu widget", _VideoMeter, ret, eRet_OutOfMemory, error);
        _VideoMeter->setLevel(0);
        _VideoMeter->setBevel(0);
        _VideoMeter->setRangeIndicator(true);
        _VideoMeter->showProgress(true);

        x += rectLabel.width() + rectValue.width();
        _LabelSeparator2 = new CWidgetLabel(getEngine(), _Bkgnd, MRect(x, menuBevel, 1, rectBkgnd.height()), font10);
        CHECK_PTR_GOTO("unable to allocate label widget", _LabelSeparator2, ret, eRet_OutOfMemory, error);
        _LabelSeparator2->setBevel(1);

        x += 1;

        /* video pts-stc */
        rectPtsStcMin.set(x, y, labelWidth, rectBkgnd.height() - menuBevel * 2 - progressHeight);
        _VideoPtsStcMinLabel = new CWidgetLabel(getEngine(), _Bkgnd, rectPtsStcMin, font10);
        CHECK_PTR_GOTO("unable to allocate menu widget", _VideoPtsStcMinLabel, ret, eRet_OutOfMemory, error);
        _VideoPtsStcMinLabel->setText("-150ms", bwidget_justify_horiz_left, bwidget_justify_vert_bottom);
        _VideoPtsStcMinLabel->setBevel(0);

        rectPtsStcMax.set(x + meterWidth - labelWidth, y, labelWidth, rectBkgnd.height() - menuBevel * 2 - progressHeight);
        _VideoPtsStcMaxLabel = new CWidgetLabel(getEngine(), _Bkgnd, rectPtsStcMax, font10);
        CHECK_PTR_GOTO("unable to allocate menu widget", _VideoPtsStcMaxLabel, ret, eRet_OutOfMemory, error);
        _VideoPtsStcMaxLabel->setText("+150ms", bwidget_justify_horiz_right, bwidget_justify_vert_bottom);
        _VideoPtsStcMaxLabel->setBevel(0);

        rectPtsStc.set(x + meterWidth / 2 - labelWidth / 2, y, labelWidth, rectBkgnd.height() - menuBevel * 2 - progressHeight);
        _VideoPtsStcLabel = new CWidgetLabel(getEngine(), _Bkgnd, rectPtsStc, font10);
        CHECK_PTR_GOTO("unable to allocate menu widget", _VideoPtsStcLabel, ret, eRet_OutOfMemory, error);
        _VideoPtsStcLabel->setText("PTS-STC", bwidget_justify_horiz_center, bwidget_justify_vert_bottom);
        _VideoPtsStcLabel->setBevel(0);


        rectMeter.set(x, y + rectLabel.height(), meterWidth, progressHeight);
        _VideoPtsStcMeter = new CWidgetMeter(getEngine(), _Bkgnd, rectMeter, font10);
        CHECK_PTR_GOTO("unable to allocate menu widget", _VideoPtsStcMeter, ret, eRet_OutOfMemory, error);
        _VideoPtsStcMeter->setLevel(0);
        _VideoPtsStcMeter->setBevel(0);
        _VideoPtsStcMeter->setRangeIndicator(true);
        _VideoPtsStcMeter->showMeter(true);

        x = menuBevel;
        y = rectLabel.y() + rectLabel.height() + menuBevel * 2;

        /* audio label */
        rectLabel.set(x, y, avLabelWidth, rectBkgnd.height() - menuBevel * 2 - progressHeight);
        _AudioLabel = new CWidgetLabel(getEngine(), _Bkgnd, rectLabel, font10);
        CHECK_PTR_GOTO("unable to allocate menu widget", _AudioLabel, ret, eRet_OutOfMemory, error);
        _AudioLabel->setText("Audio:", bwidget_justify_horiz_left, bwidget_justify_vert_bottom);
        _AudioLabel->setBevel(0);

        x += avLabelWidth + 1;

        /* audio buffer data */
        rectLabel.set(x, y, labelWidth, rectBkgnd.height() - menuBevel * 2 - progressHeight);
        _AudioBufferLabel = new CWidgetLabel(getEngine(), _Bkgnd, rectLabel, font10);
        CHECK_PTR_GOTO("unable to allocate menu widget", _AudioBufferLabel, ret, eRet_OutOfMemory, error);
        _AudioBufferLabel->setText("Buffer", bwidget_justify_horiz_left, bwidget_justify_vert_bottom);
        _AudioBufferLabel->setBevel(0);

        rectValue.set(x + labelWidth, y, 75, rectBkgnd.height() - menuBevel * 2 - progressHeight);
        _AudioValue = new CWidgetLabel(getEngine(), _Bkgnd, rectValue, font10);
        CHECK_PTR_GOTO("unable to allocate menu widget", _AudioValue, ret, eRet_OutOfMemory, error);
        _AudioValue->setText("0/0K", bwidget_justify_horiz_right, bwidget_justify_vert_bottom);
        _AudioValue->setBevel(0);

        rectAudio.set(x, y + rectLabel.height(), rectLabel.width() + rectValue.width(), progressHeight);
        _AudioMeter = new CWidgetProgress(getEngine(), _Bkgnd, rectAudio, font10);
        CHECK_PTR_GOTO("unable to allocate menu widget", _AudioMeter, ret, eRet_OutOfMemory, error);
        _AudioMeter->setLevel(0);
        _AudioMeter->setBevel(0);
        _AudioMeter->setRangeIndicator(true);
        _AudioMeter->showProgress(true);

        x += rectLabel.width() + rectValue.width();
        _LabelSeparator3 = new CWidgetLabel(getEngine(), _Bkgnd, MRect(x, y, 1, rectBkgnd.height()), font10);
        CHECK_PTR_GOTO("unable to allocate label widget", _LabelSeparator3, ret, eRet_OutOfMemory, error);
        _LabelSeparator3->setBevel(1);   

        x += 1;
 
        /* audio pts-stc */
        rectPtsStcMin.set(x, y, labelWidth, rectBkgnd.height() - menuBevel * 2 - progressHeight);
        _AudioPtsStcMinLabel = new CWidgetLabel(getEngine(), _Bkgnd, rectPtsStcMin, font10);
        CHECK_PTR_GOTO("unable to allocate menu widget", _AudioPtsStcMinLabel, ret, eRet_OutOfMemory, error);
        _AudioPtsStcMinLabel->setText("-150ms", bwidget_justify_horiz_left, bwidget_justify_vert_bottom);
        _AudioPtsStcMinLabel->setBevel(0);

        rectPtsStcMax.set(x + meterWidth - labelWidth, y, labelWidth, rectBkgnd.height() - menuBevel * 2 - progressHeight);
        _AudioPtsStcMaxLabel = new CWidgetLabel(getEngine(), _Bkgnd, rectPtsStcMax, font10);
        CHECK_PTR_GOTO("unable to allocate menu widget", _AudioPtsStcMaxLabel, ret, eRet_OutOfMemory, error);
        _AudioPtsStcMaxLabel->setText("+150ms", bwidget_justify_horiz_right, bwidget_justify_vert_bottom);
        _AudioPtsStcMaxLabel->setBevel(0);

        rectPtsStc.set(x + meterWidth / 2 - labelWidth / 2, y, labelWidth, rectBkgnd.height() - menuBevel * 2 - progressHeight);
        _AudioPtsStcLabel = new CWidgetLabel(getEngine(), _Bkgnd, rectPtsStc, font10);
        CHECK_PTR_GOTO("unable to allocate menu widget", _AudioPtsStcLabel, ret, eRet_OutOfMemory, error);
        _AudioPtsStcLabel->setText("PTS-STC", bwidget_justify_horiz_center, bwidget_justify_vert_bottom);
        _AudioPtsStcLabel->setBevel(0);

        rectMeter.set(x, y + rectLabel.height(), meterWidth, progressHeight);
        _AudioPtsStcMeter = new CWidgetMeter(getEngine(), _Bkgnd, rectMeter, font10);
        CHECK_PTR_GOTO("unable to allocate menu widget", _AudioPtsStcMeter, ret, eRet_OutOfMemory, error);
        _AudioPtsStcMeter->setLevel(0);
        _AudioPtsStcMeter->setBevel(0);
        _AudioPtsStcMeter->setRangeIndicator(true);
        _AudioPtsStcMeter->showMeter(true);
        
        x = menuBevel;
        y = rectLabel.y() + rectLabel.height() + menuBevel * 2;

        /* playback label */
        rectLabel.set(x, y, avLabelWidth, rectBkgnd.height() - menuBevel * 2 - progressHeight);
        _PlaybackLabel = new CWidgetLabel(getEngine(), _Bkgnd, rectLabel, font10);
        CHECK_PTR_GOTO("unable to allocate menu widget", _PlaybackLabel, ret, eRet_OutOfMemory, error);
        _PlaybackLabel->setText("Playback:", bwidget_justify_horiz_left, bwidget_justify_vert_bottom);
        _PlaybackLabel->setBevel(0);

        x += avLabelWidth + 1;

        /* playback buffer data */
        rectLabel.set(x, y, labelWidth, rectBkgnd.height() - menuBevel * 2 - progressHeight);
        _PlaybackBufferLabel = new CWidgetLabel(getEngine(), _Bkgnd, rectLabel, font10);
        CHECK_PTR_GOTO("unable to allocate menu widget", _PlaybackBufferLabel, ret, eRet_OutOfMemory, error);
        _PlaybackBufferLabel->setText("Buffer", bwidget_justify_horiz_left, bwidget_justify_vert_bottom);
        _PlaybackBufferLabel->setBevel(0);

        rectValue.set(x + labelWidth, y, 75, rectBkgnd.height() - menuBevel * 2 - progressHeight);
        _PlaybackValue = new CWidgetLabel(getEngine(), _Bkgnd, rectValue, font10);
        CHECK_PTR_GOTO("unable to allocate menu widget", _PlaybackValue, ret, eRet_OutOfMemory, error);
        _PlaybackValue->setText("0/0K", bwidget_justify_horiz_right, bwidget_justify_vert_bottom);
        _PlaybackValue->setBevel(0);

        rectPlayback.set(x, y + rectLabel.height(), rectLabel.width() + rectValue.width(), progressHeight);
        _PlaybackMeter = new CWidgetProgress(getEngine(), _Bkgnd, rectPlayback, font10);
        CHECK_PTR_GOTO("unable to allocate menu widget", _PlaybackMeter, ret, eRet_OutOfMemory, error);
        _PlaybackMeter->setLevel(0);
        _PlaybackMeter->setBevel(0);
        _PlaybackMeter->setRangeIndicator(true);
        _PlaybackMeter->showProgress(true);

        x += rectLabel.width() + rectValue.width();
        _LabelSeparator4 = new CWidgetLabel(getEngine(), _Bkgnd, MRect(x, y, 1, rectBkgnd.height()), font10);
        CHECK_PTR_GOTO("unable to allocate label widget", _LabelSeparator4, ret, eRet_OutOfMemory, error);
        _LabelSeparator4->setBevel(1);   

        /* adjust overall menu to fit contents */
        layoutBuffersMenu();
    }

    goto done;
error:
    uninitialize();
done:
    return ret;
}

void CPanelBuffers::uninitialize()
{
    DEL(_Back);
    DEL(_LabelSeparator4);
    DEL(_PlaybackMeter);
    DEL(_PlaybackValue);
    DEL(_PlaybackBufferLabel);
    DEL(_PlaybackLabel);
    DEL(_AudioPtsStcMeter);
    DEL(_AudioPtsStcLabel);
    DEL(_AudioPtsStcMaxLabel);
    DEL(_AudioPtsStcMinLabel);
    DEL(_LabelSeparator3);
    DEL(_AudioMeter);
    DEL(_AudioValue);
    DEL(_AudioBufferLabel);
    DEL(_AudioLabel);
    DEL(_VideoPtsStcMeter);
    DEL(_VideoPtsStcLabel);
    DEL(_VideoPtsStcMaxLabel);
    DEL(_VideoPtsStcMinLabel);
    DEL(_LabelSeparator2);
    DEL(_VideoMeter);
    DEL(_VideoValue);
    DEL(_VideoBufferLabel);
    DEL(_LabelSeparator1);
    DEL(_VideoLabel);
    DEL(_Bkgnd);
    DEL(_pBuffersMenu);

}

bool CPanelBuffers::isPlaybackActive()
{
    bool        bActive   = false;
    CPlayback * pPlayback = _pModel->getPlayback();

    if (NULL != pPlayback)
    {
        bActive = pPlayback->isActive();
    }

    return bActive;
}

void CPanelBuffers::layoutBuffersMenu()
{
    MRect       rectMenu;
    MRect       rectVideoTitle  = _VideoLabel->getGeometry();
    MRect       rectVideoMeter  = _VideoMeter->getGeometry();
    MRect       rectVideoPtsStc = _VideoPtsStcMeter->getGeometry();
    MRect       rectBkgnd;
    int         menuBevel       = _Bkgnd->getBevel();
    int         marginBkgnd     = 1;
    int         width           = rectVideoTitle.width() + rectVideoMeter.width() + rectVideoPtsStc.width() + menuBevel * 2 + 4;

    /* assume geometry where playback buffer data is available */
    CPlayback * pPlayback       = _pModel->getPlayback();
    int         y               = 100;
    int         height          = 23 * 3 + 1;

    if (isPlaybackActive())
    {
        y +=30; /* account for playback timeline */
        /* playback data is available */
        _PlaybackLabel->show(true);
        _PlaybackBufferLabel->show(true);
        _PlaybackValue->show(true);
        _PlaybackMeter->show(true);
    }
    else
    {
        /* adjust for no playback buffer data */
        y      -= 10;
        height -= 20;

        _PlaybackLabel->show(false);
        _PlaybackBufferLabel->show(false);
        _PlaybackValue->show(false);
        _PlaybackMeter->show(false);
    }
    
    /* resize window to fit contents */
    rectMenu.set((_graphicsWidth / 2) - (width + marginBkgnd * 2) / 2, 
                  _graphicsHeight - y - marginBkgnd * 2, 
                  width + marginBkgnd * 2, 
                  height + marginBkgnd * 2);
    setGeometry(rectMenu);

    _pBuffersMenu->setGeometry(MRect(0, 0, rectMenu.width(), rectMenu.height()));
    rectBkgnd.set(marginBkgnd, marginBkgnd, width, height);
    _Bkgnd->setGeometry(rectBkgnd);
}


void CPanelBuffers::onClick(bwidget_t widget)
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
}


void CPanelBuffers::show(bool bShow)
{
    /* avoid CPanel's "restore last focus" feature since
       there is no focusable widgets in this panel */
    CWidgetLabel::show(bShow);

    if (true == bShow)
    {
        layoutBuffersMenu();
        _timerBuffers.start(GET_INT(_pCfg, UI_BUFFERS_UPDATE_TIMEOUT));
    }
    else
    {
        _timerBuffers.stop();
    }
}

void CPanelBuffers::processNotification(CNotification & notification)
{
    switch(notification.getId())
    {
    case eNotify_Timeout:
    {
        CTimer * pTimer = (CTimer *)notification.getData();
        if (&_timerBuffers == pTimer)
        {
            if (_pBuffersMenu->isVisible())
            {
                updateBuffersData();
            }

            _timerBuffers.start();
        }
    }
        break;

    case eNotify_PlaybackStateChanged:
        if (_pBuffersMenu->isVisible())
        {
            layoutBuffersMenu();
        }
        break;

    default:
        break;
    }

error:
    return;
}

#define CALC_BUF_LEVEL(BUF, LEVEL, NSTATUS) \
    if (1024 * 1024 > NSTATUS.fifoDepth) \
        snprintf(BUF, 32, "%ldK/%ldK", \
                NSTATUS.fifoDepth / 1024, \
                NSTATUS.fifoSize / 1024); \
    else \
        snprintf(BUF, 32, "%0.1fM/%0.1fM", \
                (NSTATUS.fifoDepth / 1024.0) / 1024, \
                (NSTATUS.fifoSize / 1024.0) / 1024); \
    LEVEL = 65535.0 * NSTATUS.fifoDepth / NSTATUS.fifoSize


eRet CPanelBuffers::updateBuffersData()
{
    eRet                       ret              = eRet_Ok;
    CSimpleVideoDecode       * pVideoDecode     = _pModel->getSimpleVideoDecode();
    CSimpleAudioDecode       * pAudioDecode     = _pModel->getSimpleAudioDecode();
    CPlayback                * pPlayback        = _pModel->getPlayback();
    NEXUS_VideoDecoderStatus   nVideoStatus;
    NEXUS_AudioDecoderStatus   nAudioStatus;
    NEXUS_PlaybackStatus       nPlaybackStatus;
    NEXUS_Error                nerror           = NEXUS_SUCCESS;
    char                       buf[32];
    uint16_t                   level            = 0;

    ret = pVideoDecode->getStatus(&nVideoStatus);
    CHECK_ERROR_GOTO("unable to retrieve video decode status", ret, error);

    CALC_BUF_LEVEL(buf, level, nVideoStatus);
    _VideoValue->setText(buf);
    _VideoMeter->setLevel(level);

    ret = pAudioDecode->getStatus(&nAudioStatus);
    CHECK_ERROR_GOTO("getting audio decoder status failed!", ret, error);

    CALC_BUF_LEVEL(buf, level, nAudioStatus);
    _AudioValue->setText(buf);
    _AudioMeter->setLevel(level);

    strncpy(buf, "0/0K", 32);
    level = 0;
    if (NULL != pPlayback)
    {
        nerror = NEXUS_Playback_GetStatus(pPlayback->getPlayback(), &nPlaybackStatus);
        CHECK_NEXUS_ERROR_ASSERT("getting playback status failed!", nerror);

        CALC_BUF_LEVEL(buf, level, nPlaybackStatus);
    }
    _PlaybackValue->setText(buf);
    _PlaybackMeter->setLevel(level);

    /* update pts-stc meters */
    {
        int diff = nVideoStatus.ptsStcDifference / 45; /* convert to msec */
        if (nVideoStatus.ptsStcDifference < 0)
        {
            _VideoPtsStcMeter->setLevel(MAX(diff, -150) * 32767 / 150);
        }
        else
        {
            _VideoPtsStcMeter->setLevel(MIN(diff, 150) * 32767 / 150);
        }

        diff = nAudioStatus.ptsStcDifference / 45; /* convert to msec */
        if (nAudioStatus.ptsStcDifference < 0)
        {
            _AudioPtsStcMeter->setLevel(MAX(diff, -150) * 32767 / 150);
        }
        else
        {
            _AudioPtsStcMeter->setLevel(MIN(diff, 150) * 32767 / 150);
        }
    }

error:
    return ret;
}


