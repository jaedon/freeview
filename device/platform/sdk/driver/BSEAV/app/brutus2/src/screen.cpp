/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: screen.cpp $
* $brcm_Revision: 11 $
* $brcm_Date: 9/15/12 1:09a $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/screen.cpp $
* 
* 11   9/15/12 1:09a tokushig
* SW7231-749: get fonts from graphics object
* 
* 10   7/9/12 4:30p tokushig
* SW7231-749: refactor the main screen so that it can coexist with other
* screens and properly segment code into cpanels.  partially complete -
* info menu and buffers menu still need refactoring.
* 
* 9   4/14/12 5:28p tokushig
* SW7231-749: updated screen and test screen classes to work with new
* widget classes.
* 
* 8   4/11/12 12:16p tokushig
* SW7231-749: updated screens to use new label/button widget classes
* 
* 7   4/6/12 12:43p tokushig
* SW7231-749: fixed callback handling issue in register/notification
* mechanism which prevented control from manually notifying views.  view
* and controller classes now hide the callback mechanism from the mvc
* based classes for notifications and there is a consistent
* processNotification() method for both views and controllers (models do
* not receive notifications.
* 
* 6   3/2/12 3:53p tokushig
* SW7405-5581: propagate key presses to active window and its parents if
* necessary.  allow controller last chance to act on key presses ignored
* by screens/bwidgets.  move some callback code to cscreen to hide from
* individual screen classes.
* 
* 5   3/1/12 3:56p tokushig
* SW7405-5581: moved qam scan into separate thread to enable screen
* updates while scan is running.  added on screen scan progress to test.
* NOTE: this does not yet have protection against conflicts so trying to
* channel up/down while scan is running will result in a crash
* 
* 4   2/28/12 5:03p tokushig
* SW7405-5581: add header comments
* 
* 
*********************************************************************************/

#include "brutus.h"
#include "brutus_os.h"
#include "screen.h"
#include "widget_label.h"

BDBG_MODULE(brutus_screen);

CScreen::CScreen(bwidget_engine_t widgetEngine, CModelConfig * pModelConfig, CWidgetBase * pParent, bwin_t win) :
    CWidgetLabel(widgetEngine, pParent, MRect(0, 0, 0, 0), NULL, win),
    _widgetEngine(widgetEngine),
    _pModelConfig(pModelConfig),
    _pCfg(NULL),
    _framebufferWin(NULL)
{
    BDBG_ASSERT(NULL != widgetEngine);
    BDBG_ASSERT(NULL != pModelConfig);

    _pCfg = _pModelConfig->getCfg();
}

CScreen::~CScreen()
{
}

eRet CScreen::initialize(CModel * pModel)
{
    eRet              ret       = eRet_Ok;
    CGraphics       * pGraphics = NULL;
    BDBG_ASSERT(NULL != pModel);

    pGraphics       = pModel->getGraphics();
    _pModel         = pModel;
    _framebufferWin = pGraphics->getWinFramebuffer();

    {
        int screenWidth          = GET_INT(_pCfg, GRAPHICS_SURFACE_WIDTH);
        int screenHeight         = GET_INT(_pCfg, GRAPHICS_SURFACE_HEIGHT); 
        uint32_t backgroundColor = GET_INT(_pCfg, WINDOW_BACKGROUND_COLOR);

        setGeometry(MRect(0, 0, screenWidth, screenHeight));
        setFont(pGraphics->getFont());
        setBackgroundColor(backgroundColor);
    }

    return ret;
}

void CScreen::onClick(bwidget_t widget)
{
}

eRet CScreen::onKeyDown(bwidget_t widget, bwidget_key key)
{
    /* indicate that key is not consumed */
    return eRet_NotSupported;
}

void CScreen::show(bool bShow)
{
    CWidgetLabel::show(bShow);
}

void CScreen::processNotification(CNotification & notification)
{
}
