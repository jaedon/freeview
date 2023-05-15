/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: screen.h $
* $brcm_Revision: 9 $
* $brcm_Date: 7/9/12 4:30p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/screen.h $
* 
* 9   7/9/12 4:30p tokushig
* SW7231-749: refactor the main screen so that it can coexist with other
* screens and properly segment code into cpanels.  partially complete -
* info menu and buffers menu still need refactoring.
* 
* 8   4/14/12 5:28p tokushig
* SW7231-749: updated screen and test screen classes to work with new
* widget classes.
* 
* 7   4/11/12 12:16p tokushig
* SW7231-749: updated screens to use new label/button widget classes
* 
* 6   4/6/12 12:43p tokushig
* SW7231-749: fixed callback handling issue in register/notification
* mechanism which prevented control from manually notifying views.  view
* and controller classes now hide the callback mechanism from the mvc
* based classes for notifications and there is a consistent
* processNotification() method for both views and controllers (models do
* not receive notifications.
* 
* 5   3/2/12 3:53p tokushig
* SW7405-5581: propagate key presses to active window and its parents if
* necessary.  allow controller last chance to act on key presses ignored
* by screens/bwidgets.  move some callback code to cscreen to hide from
* individual screen classes.
* 
* 4   2/28/12 5:03p tokushig
* SW7405-5581: add header comments
* 
* 
*********************************************************************************/

#ifndef BRUTUS_SCREEN_H__
#define BRUTUS_SCREEN_H__

#include "view.h"
#include "bwidgets.h"
#include "widget_base.h"
#include "widget_label.h"
#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
    CScreen is the abstract base class from which all user inteface screens are
    derived.  It implements the common window methods and attributes.
*/
class CScreen : public CWidgetLabel
{
public:
    CScreen(bwidget_engine_t    widgetEngine, 
            CModelConfig      * pModelConfig, 
            CWidgetBase       * pParent,
            bwin_t              win             = NULL); /* optional if pParent is defined */
    ~CScreen(void);
    virtual eRet initialize(CModel * pModel);
    virtual void onClick(bwidget_t widget);
    virtual eRet onKeyDown(bwidget_t widget, bwidget_key key);
    virtual void processNotification(CNotification & notification);
    virtual void show(bool bShow);

    virtual bwin_engine_t getWinEngine(void) { return bwidget_win_engine(getEngine()); };

protected:
    bwidget_engine_t    _widgetEngine;
    CModelConfig      * _pModelConfig;
    CConfiguration    * _pCfg;
    bwin_t              _framebufferWin;
};


#ifdef __cplusplus
}
#endif

#endif /* BRUTUS_SCREEN_H__ */
