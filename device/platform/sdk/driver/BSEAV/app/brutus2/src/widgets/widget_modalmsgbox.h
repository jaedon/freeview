/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: widget_modalmsgbox.h $
* $brcm_Revision: 3 $
* $brcm_Date: 10/7/12 3:32p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/widgets/widget_modalmsgbox.h $
* 
* 3   10/7/12 3:32p tokushig
* SW7231-749: add ability to set default msg box button
* 
* 2   7/9/12 4:24p tokushig
* SW7231-749: remove msgbox parent widget parameter.  widget will find
* framebuffer bwin internally for msgbox.  this will allow the msgbox to
* display outside the confines of it's parent widget's geometry.
* 
* 1   6/26/12 4:00p tokushig
* SW7231-749: add modal message box capability
* 
* 
*********************************************************************************/

#ifndef BRUTUS_WIDGET_MODALMSGBOX_H__
#define BRUTUS_WIDGET_MODALMSGBOX_H__

#include "widget_label.h"
#include "widget_button.h"

#ifdef __cplusplus
extern "C" {
#endif

class CWidgetModalMsgBox : public CWidgetLabel
{
public:
    CWidgetModalMsgBox(bwidget_engine_t   engine, 
                       CWidgetBase      * pParentWidget, 
                       MRect              geometry,
                       bwin_font_t        font);
    ~CWidgetModalMsgBox();

    virtual void onClick(bwidget_t widget);
    virtual eRet onKeyDown(bwidget_t widget, bwidget_key key);

    void setText(const char            * pText, 
                 bwidget_justify_horiz   justifyHoriz = bwidget_justify_horiz_max,
                 bwidget_justify_vert    justifyVert  = bwidget_justify_vert_max);

    MString showModal(const char * msg,
                      uint8_t      focusButton = 0,
                      const char * name1       = "Ok", 
                      const char * name2       = "Cancel", 
                      const char * name3       = NULL);

    MString getResult(void) { return _result; };

    void cancelModal(const char * name) { _result = name; _done = true; };

protected:
    bool            _done;
    CWidgetLabel  * _pLabel;
    CWidgetButton * _pOk;
    CWidgetButton * _pCancel;
    MString         _result;
};


#ifdef __cplusplus
}
#endif

#endif /* BRUTUS_WIDGET_MODALMSGBOX_H__ */
