/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: widget_base.h $
* $brcm_Revision: 8 $
* $brcm_Date: 10/7/12 3:25p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/widgets/widget_base.h $
* 
* 8   10/7/12 3:25p tokushig
* SW7231-749: add method to get parent bwin win
* 
* 7   5/25/12 12:26a tokushig
* SW7231-749: added additional support for bwidget features for all
* widget classes
* 
* 6   4/20/12 2:45p tokushig
* SW7231-749: add support for scan menu
* 
* 5   4/17/12 4:36p tokushig
* SW7231-749: menu button on remote will show/hide menu.  on screen
* channel number will timeout in 3 secs if the menu is not shown.  if
* the menu is shown, the channel number will always be displayed.
* 
* 4   4/14/12 7:26p tokushig
* SW7231-749: change the way click/keydown callbacks execute and modify
* way listview widget redirects keydown events to itself (so it can do
* scrolling internally)
* 
* 2   4/13/12 7:25p tokushig
* SW7231-749: add widget features
* 
* 1   4/11/12 12:12p tokushig
* SW7231-749: initial creation
* 
* 
*********************************************************************************/

#ifndef BRUTUS_WIDGET_BASE_H__
#define BRUTUS_WIDGET_BASE_H__

#include "brutus.h"
#include "mgeom.h"
#include "bwidgets.h"
#include "view.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum bevelParts_t
{
    bevel_eTop, 
    bevel_eRight, 
    bevel_eBottom, 
    bevel_eLeft,
    bevel_eMax       /* sentinal */
} bevelParts_t;


class CWidgetBase : public CView
{
public:
    CWidgetBase(bwidget_engine_t engine, CWidgetBase * pParent);

    bwidget_engine_t  getEngine(void) { return _engine; };
    void              setFocusable(bool bFocusable);
    int               getZOrder(void);
    void              setZOrder(int zorder);
    CWidgetBase     * getParent(void) { return _pParent; };
    void              setParent(CWidgetBase * parent) { _pParent = parent; }; /* note: does not change bwidget parent! */
    bwin_t            getParentWin(); 
    void              setParentWin(bwin_t parentWin); /* note: does not change the CWidgetBase parent! */
    int               getValue(void) { return _value; };
    void              setValue(int value) { _value = value; };
    
    virtual bool          setFocus(void);
    virtual void          show(bool bShow);
    virtual bwidget_t     getWidget(void) { return _widget; };
    virtual bwin_t        getWin(void) { return bwidget_win(getWidget()); };
    virtual bwin_engine_t getWinEngine(void) { return bwidget_win_engine(getEngine()); };
    virtual MRect         getGeometry(void)           = 0;
    virtual void          setGeometry(MRect geometry) = 0;

    virtual eRet onKeyDown(bwidget_t widget, bwidget_key key);
    virtual void onClick(bwidget_t widget);

    virtual bool isVisible(void);

protected:
    bwidget_engine_t   _engine;
    CWidgetBase      * _pParent;
    bwidget_t          _widget;
    int                _value;
};

#ifdef __cplusplus
}
#endif

#endif /* BRUTUS_WIDGET_BASE_H__ */
