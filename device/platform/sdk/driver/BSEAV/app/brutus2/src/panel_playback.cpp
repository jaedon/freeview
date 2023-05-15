#include "avtypes.h"
#include "panel_playback.h"

BDBG_MODULE(panel_playback);

CPanelPlayback::CPanelPlayback(bwidget_engine_t   engine, 
                               CWidgetBase      * pParentWidget, 
                               MRect              geometry,
                               bwin_font_t        font,
                               bwin_t             parentWin) :
    CPanel(engine, pParentWidget, geometry, font, parentWin),
    _pPlaybackMenu(NULL),
    _Back(NULL),
    _pList(NULL)
{
}

CPanelPlayback::~CPanelPlayback()
{
    uninitialize();
}

/* JOSE: playback menu implementation is in this file which is directly included into screen_test.cpp.
   it's a cheesy way to split the menu code off, but i couldn't decide on a great way to do it so i
   did this - good enough for now.
 
   JOSE: if you need to process any incomming notifications (processNotification()) we will need to do a similar
   thing and put a processNotificationPlaybackMenu() method in here. */

/* JOSE: returns eRet_Ok if everything gets created...eRet_OutOfMemory otherwise */
eRet CPanelPlayback::initialize(CModel * pModel, CModelConfig * pModelConfig)
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
    _pPlaybackMenu = new CWidgetMenu(getEngine(), this, MRect(0, 0, menuWidth, menuHeight), font);
    CHECK_PTR_GOTO("unable to allocate menu widget", _pPlaybackMenu, ret, eRet_OutOfMemory, error);
    _pPlaybackMenu->setMenuTitle("Playback", "Files");
    _pPlaybackMenu->show(true);
    if (_pList != NULL)
    {
        refreshPlaybackButtonList();
    }
    /* back button */
        /* JOSE: the back button is the button in the upper left area of the CWidgetMenu.  similar to
           the other menu buttons, it is created here and passed in using addBackButton().  it behaves
           similarly where click callbacks come to this CPanelPlayback. */
    _Back = new CWidgetButton(getEngine(), this, MRect(0, 0, 0, 0), font);
    CHECK_PTR_GOTO("unable to allocate button widget", _Back, ret, eRet_OutOfMemory, error);
    _Back->setText("Menu");
    _pPlaybackMenu->addBackButton(_Back);

    _Back->setFocus();

    goto done;
error:
    uninitialize();
done:
    return ret;
}

void CPanelPlayback::uninitialize()
{
    /* remove buttons from menu and free widgets  */
    _pPlaybackMenu->clearButtons();
    _Buttons.clear();

    DEL(_Back);
    DEL(_pPlaybackMenu);
}

/* JOSE: only handles button clicks in playback menu.  if fully handled, then it returns eRet_Ok which
   signifies that the click has been consumed.  otherwise, it returns eRet_NotSupported and the onClick()
   handler in screen_test.cpp will continue looking for a handler. */
void CPanelPlayback::onClick(bwidget_t widget)
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

    for(pButton = itrButtons.first(); pButton; pButton = itrButtons.next())
    {
        MList <CVideo> * pVideoList =  _pList->getVideosList();
        CVideo         * video      = NULL;

        BDBG_MSG(("index %d",i++));

        if(pWidget == pButton)
        {
            video = pVideoList->at(_Buttons.index(pButton));
            BDBG_MSG(("Info FILE selected calling Correct Playback File %s",video->_infoName.s()));
            BDBG_MSG(("MPG FILE selected calling Correct Playback File %s",video->_mediaName.s()));
            {
                CPlaybackData playbackData(video->_mediaName.s(),NULL,NULL,video);
                notifyObservers(eNotify_PlaybackStart, &playbackData);
                show(false);
            }
            break;
        }
    }

done:
    return;
}


void CPanelPlayback::processNotification(CNotification & notification)
{
    BDBG_MSG(("--- CPanelPlayback Notification Received: %d ---", notification.getId()));
    switch(notification.getId())
    {
    case eNotify_PlaybackListChanged:
    {
        CPlaybackList * pPlaybackList = (CPlaybackList *)notification.getData();
        BDBG_ASSERT(pPlaybackList);
        _pList = pPlaybackList;
        refreshPlaybackButtonList();
    }
    break;

    default:
        break;
    }
}

void CPanelPlayback::refreshPlaybackButtonList()
{
    /* Clear the list , then recreate it */
    MListItr <CWidgetButton> itrButtons(&_Buttons);
    MListItr <CVideo>        itrVideos(_pList->getVideosList());

    CWidgetButton * pButton;
    MString         mediaName;
    CVideo        * video;
    bwin_font_t     font;
    CGraphics     * pGraphics      = NULL;
    
    pGraphics = _pModel->getGraphics();
    BDBG_ASSERT(NULL != pGraphics);

    font = pGraphics->getFont();
    BDBG_ASSERT(NULL != font);

    if (true == isVisible())
    {
        /* change focus before deleting buttons */
        _Back->setFocus();
    }

    /* remove buttons from menu and free widgets  */
    _pPlaybackMenu->clearButtons();
    _Buttons.clear();

    /* change playback panel last focus */
    _focusWidget = _Back->getWidget();

    if (_pList == NULL)
    {
        BDBG_ERR((" No playback List"));
        return;
    }

    for (video=itrVideos.first();video;video=itrVideos.next()) 
    {
        mediaName = video->_infoName;
        pButton = new CWidgetButton(getEngine(), this, MRect(0, 0, 0, 22), font);
        BDBG_MSG(("Allocate Button widget, for info file %s,",mediaName.s()));
        mediaName.truncate(mediaName.find(".",0,false));
        pButton->setText(mediaName.s(), bwidget_justify_horiz_left);
        _pPlaybackMenu->addButton(pButton, mediaName);
        _Buttons.add(pButton);
    
    }
}
