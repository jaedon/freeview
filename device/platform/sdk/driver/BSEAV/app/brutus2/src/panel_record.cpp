#include "avtypes.h"
#include "panel_record.h"

BDBG_MODULE(panel_record);

CPanelRecord::CPanelRecord(bwidget_engine_t   engine, 
                               CWidgetBase      * pParentWidget, 
                               MRect              geometry,
                               bwin_font_t        font,
                               bwin_t             parentWin) :
    CPanel(engine, pParentWidget, geometry, font, parentWin),
    _pRecordMenu(NULL),
    _Back(NULL)
{
}

CPanelRecord::~CPanelRecord()
{
    uninitialize();
}


/* returns eRet_Ok if everything gets created...eRet_OutOfMemory otherwise */
eRet CPanelRecord::initialize(CModel * pModel, CModelConfig * pModelConfig)
{
    eRet                  ret            = eRet_Ok;
    CGraphics           * pGraphics      = NULL;
    bwin_font_t           font           = NULL;
    uint32_t              graphicsWidth  = 0;
    uint32_t              graphicsHeight = 0;
    int                   menuWidth      = 200;
    int                   menuHeight     = 350;
    MRect                 rectPanel;

    BDBG_ASSERT(NULL != pModel);
    BDBG_ASSERT(NULL != pModelConfig);

    setModel(pModel);
    setModelConfig(pModelConfig);

    pGraphics = _pModel->getGraphics();
    BDBG_ASSERT(NULL != pGraphics);

    font = pGraphics->getFont();
    BDBG_ASSERT(NULL != font);
        
    graphicsWidth  = GET_INT(_pCfg, GRAPHICS_SURFACE_WIDTH);
    graphicsHeight = GET_INT(_pCfg, GRAPHICS_SURFACE_HEIGHT);

    /* set the size of the panel itself */
    rectPanel.set(50, 50, menuWidth, menuHeight);
    setGeometry(rectPanel);

    /* JOSE: CWidgetMenu's consist of the menu bar, the scrolling list view, and the other stuff that
       makes it look like a window.   It will internally handle all the scrolling.  you must create buttons
       outside of the CWidgetMenu and then call addButton() to add it to the widget.  because you create
       the buttons outside of the CWidgetMenu, we will set them up so when clicked, the callback comes
       to the CPanelPlayback and not to the CWidgetMenu itself. */
    _pRecordMenu = new CWidgetMenu(getEngine(), this, MRect(0, 0, menuWidth, menuHeight), font);
    CHECK_PTR_GOTO("unable to allocate menu widget", _pRecordMenu, ret, eRet_OutOfMemory, error);
    _pRecordMenu->setMenuTitle("Record", "Files");
    _pRecordMenu->show(true);

    _Back = new CWidgetButton(getEngine(), this, MRect(0, 0, 0, 0), font);
    CHECK_PTR_GOTO("unable to allocate button widget", _Back, ret, eRet_OutOfMemory, error);
    _Back->setText("Menu");
    _pRecordMenu->addBackButton(_Back);

    _Back->setFocus();

    goto done;
error:
    uninitialize();
done:
    return ret;
}

void CPanelRecord::uninitialize()
{
    DEL(_Back);
    DEL(_pRecordMenu);
}

/* JOSE: only handles button clicks in playback menu.  if fully handled, then it returns eRet_Ok which
   signifies that the click has been consumed.  otherwise, it returns eRet_NotSupported and the onClick()
   handler in screen_test.cpp will continue looking for a handler. */
void CPanelRecord::onClick(bwidget_t widget)
{
    MListItr <CWidgetButton>   itrButtons(&_Buttons);
    eNotification    notification  = eNotify_Invalid;
    CWidgetBase    * pWidget       = NULL;
    CWidgetButton  * pButton;
    int i=0;

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

    

done:
    return;
}


void CPanelRecord::processNotification(CNotification & notification)
{
    BDBG_MSG(("--- CPanelPlayback Notification Received: %d ---", notification.getId()));
    switch(notification.getId())
    {
    case eNotify_RecordStarted:
    {
       BDBG_ERR(("Record Started"));
    }

    break;

    default:
        break;
    }
}

