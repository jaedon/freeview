/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: panel_scan_vsb.cpp $
* $brcm_Revision: 11 $
* $brcm_Date: 11/15/12 10:46a $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/panel_scan_vsb.cpp $
* 
* 11   11/15/12 10:46a tokushig
* SW7429-369: fix build for 97429/97241 and other platforms where
* NEXUS_HAS_FRONTEND is false
* 
* 
*********************************************************************************/
#if NEXUS_HAS_FRONTEND

#include "avtypes.h"
#include "tuner_vsb.h"
#include "panel_scan_vsb.h"

BDBG_MODULE(panel_scan_vsb);

CPanelScanVsb::CPanelScanVsb(bwidget_engine_t   engine, 
                             CWidgetBase      * pParentWidget, 
                             MRect              geometry,
                             bwin_font_t        font,
                             bwin_t             parentWin) :
    CPanel(engine, pParentWidget, geometry, font, parentWin),
    _pScanVsbMenu(NULL),
    _Settings(NULL),
    _StartFreq(NULL),
    _StartFreqLabel(NULL),
    _StartFreqEdit(NULL),
    _StartFreqUnits(NULL),
    _EndFreq(NULL),
    _EndFreqLabel(NULL),
    _EndFreqEdit(NULL),
    _EndFreqUnits(NULL),
    _StepFreq(NULL),
    _StepFreqLabel(NULL),
    _StepFreqEdit(NULL),
    _StepFreqUnits(NULL),
    _Append(NULL),
    _Mode(NULL),
    _ModeLabel(NULL),
    _ModePopup(NULL),
    _Mode8(NULL),
    _Mode16(NULL),
    _Scan(NULL),
    _Back(NULL)
{
}

CPanelScanVsb::~CPanelScanVsb()
{
    uninitialize();
}

eRet CPanelScanVsb::initialize(CModel * pModel, CModelConfig * pModelConfig)
{
    eRet                  ret            = eRet_Ok;
    CGraphics           * pGraphics      = NULL;
    bwin_font_t           font12         = NULL;
    bwin_font_t           font14         = NULL;
    uint32_t              graphicsWidth  = 0;
    uint32_t              graphicsHeight = 0;
    int                   menuWidth      = 230;
    int                   menuHeight     = 215;
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

    _pScanVsbMenu = new CWidgetMenu(getEngine(), this, MRect(0, 0, menuWidth, menuHeight), font14, font12);
    CHECK_PTR_GOTO("unable to allocate menu widget", _pScanVsbMenu, ret, eRet_OutOfMemory, error);
    _pScanVsbMenu->showSubTitle(false);
    _pScanVsbMenu->showListView(false);
    _pScanVsbMenu->showEdit(false);
    _pScanVsbMenu->setMenuTitle("Scan Vsb", NULL);
    _pScanVsbMenu->show(true);
    {
        _Settings = new CWidgetMenu(getEngine(), _pScanVsbMenu, MRect(0, 30, menuWidth, 145), font14, font12);
        CHECK_PTR_GOTO("unable to allocate menu widget", _Settings, ret, eRet_OutOfMemory, error);
        _Settings->showMenuBar(false);
        _Settings->setMenuTitle(NULL, "Settings");
        _Settings->setScroll(true);
        {
            ret = addDualLabelEditButton(_Settings,  "Start Freq", &_StartFreq,  &_StartFreqLabel,  &_StartFreqEdit,  &_StartFreqUnits,  font12);
            CHECK_ERROR_GOTO("unable to allocate dual label edit button", ret, error);
            _StartFreq->setFocusable(false);
            _StartFreqLabel->setText("Start Freq:", bwidget_justify_horiz_left, bwidget_justify_vert_middle);
            _StartFreqEdit->setText("497000000", bwidget_justify_horiz_right);
            _StartFreqUnits->setText("Hz", bwidget_justify_horiz_left, bwidget_justify_vert_middle);

            ret = addDualLabelEditButton(_Settings,  "End Freq", &_EndFreq,  &_EndFreqLabel,  &_EndFreqEdit,  &_EndFreqUnits,  font12);
            CHECK_ERROR_GOTO("unable to allocate dual label edit button", ret, error);
            _EndFreq->setFocusable(false);
            _EndFreqLabel->setText("End Freq:", bwidget_justify_horiz_left, bwidget_justify_vert_middle);
            _EndFreqEdit->setText("629000000", bwidget_justify_horiz_right);
            _EndFreqUnits->setText("Hz", bwidget_justify_horiz_left, bwidget_justify_vert_middle);

            ret = addDualLabelEditButton(_Settings,  "Step Freq", &_StepFreq,  &_StepFreqLabel,  &_StepFreqEdit,  &_StepFreqUnits,  font12);
            CHECK_ERROR_GOTO("unable to allocate dual label edit button", ret, error);
            _StepFreq->setFocusable(false);
            _StepFreqLabel->setText("Step Freq:", bwidget_justify_horiz_left, bwidget_justify_vert_middle);
            _StepFreqEdit->setText("1000000", bwidget_justify_horiz_right);
            _StepFreqUnits->setText("Hz", bwidget_justify_horiz_left, bwidget_justify_vert_middle);

            ret = addLabelPopupButton(_Settings, "Mode", &_Mode, &_ModeLabel, &_ModePopup, font12);
            CHECK_ERROR_GOTO("unable to allocate label popup list button", ret, error);
            _Mode->setFocusable(false);
            _ModeLabel->setText("Mode:", bwidget_justify_horiz_left, bwidget_justify_vert_middle);
            {
                MRect rectSettings = _Settings->getGeometry();
                MRect rect         = _ModePopup->getGeometry();

                _Mode8   = _ModePopup->addButton("8", rect.width(), rect.height());
                _Mode8->setValue(NEXUS_FrontendVsbMode_e8);
                _Mode16  = _ModePopup->addButton("16", rect.width(), rect.height());
                _Mode16->setValue(NEXUS_FrontendVsbMode_e16);

                _ModePopup->select(_Mode8);
            }

            _Append = new CWidgetCheckButton(getEngine(), this, MRect(0, 0, 0, 22), font12, _pScanVsbMenu->getWin());
            CHECK_PTR_GOTO("unable to allocate button widget", _Append, ret, eRet_OutOfMemory, error);
            _Append->setText("Append to Channel List", bwidget_justify_horiz_left);
            _Append->setCheck(true);
            _Settings->addButton(_Append, "Append to Channel List");
        }

        {
            MRect rectMenu;
            MRect rectStart;

            rectMenu = _Settings->getGeometry();

            rectStart.setWidth(80);
            rectStart.setHeight(25);
            rectStart.setX(menuWidth - rectStart.width() - 10);
            rectStart.setY(rectMenu.y() + rectMenu.height() + 5);
            _Scan = new CWidgetButton(getEngine(), this, rectStart, font12, _pScanVsbMenu->getWin());
            _Scan->setBackgroundColor(0xFF80C42F);
            _Scan->setTextColor(0xFF222222);
            _Scan->setText("Scan");
        }

        /* back button */
        _Back = new CWidgetButton(getEngine(), this, MRect(0, 0, 0, 0), font12);
        CHECK_PTR_GOTO("unable to allocate button widget", _Back, ret, eRet_OutOfMemory, error);
        _Back->setText("Menu");
        _pScanVsbMenu->addBackButton(_Back);

        _Back->setFocus();
    }

    goto done;
error:
    uninitialize();
done:
    return ret;
}

void CPanelScanVsb::uninitialize()
{
    DEL(_Back);
    DEL(_Scan);
    DEL(_Append);
    DEL(_ModePopup);
    DEL(_ModeLabel);
    DEL(_Mode);
    DEL(_StepFreqUnits);
    DEL(_StepFreqEdit);
    DEL(_StepFreqLabel);
    DEL(_StepFreq);
    DEL(_EndFreqUnits);
    DEL(_EndFreqEdit);
    DEL(_EndFreqLabel);
    DEL(_EndFreq);
    DEL(_StartFreqUnits);
    DEL(_StartFreqEdit);
    DEL(_StartFreqLabel);
    DEL(_StartFreq);
    DEL(_Settings);
    DEL(_pScanVsbMenu);
}

void CPanelScanVsb::onClick(bwidget_t widget)
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
    else
    if (_Scan == pWidget)
    {
        eRet                ret       = eRet_Ok;
        CTunerVsbScanData   scanData  = NULL;
        uint32_t            bandwidth = 0;
        uint32_t            startFreq = 0;
        uint32_t            endFreq   = 0;

        startFreq = (uint32_t)_StartFreqEdit->getText().toLong();
        endFreq   = (uint32_t)_EndFreqEdit->getText().toLong();
        bandwidth = (uint32_t)_StepFreqEdit->getText().toLong();

        /* create list of freq to scan */
        for (uint32_t freq = startFreq; freq <= endFreq; freq += bandwidth)
        {
            scanData._freqList.add(new uint32_t(freq));
        }

        scanData._bandwidth           = bandwidth;
        scanData._appendToChannelList = _Append->isChecked();
        scanData._mode                = _ModePopup->getSelection()->getValue();

        scanData.dump();
        notifyObservers(eNotify_ScanStart, &scanData);
        show(false);
    }

error:
    return;
}

void CPanelScanVsb::processNotification(CNotification & notification)
{
    BDBG_MSG(("CPanelScanVsb::processNotificationScanVsbMenu:%d", notification.getId()));
}

#endif /* NEXUS_HAS_FRONTEND */