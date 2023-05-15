/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: widget_label.h $
* $brcm_Revision: 7 $
* $brcm_Date: 8/8/12 4:57p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/widgets/widget_label.h $
* 
* 7   8/8/12 4:57p tokushig
* SW7231-749: add font get/set methods
* 
* 6   8/2/12 11:02a tokushig
* SW7231-749: Add missing destructor to label class
* 
* 5   5/4/12 2:04p tokushig
* SW7231-749: add buffers screen, checkbox widget, and new fonts
* 
* 4   4/16/12 5:52p tokushig
* SW7231-749: add progess label widget
* 
* 3   4/14/12 5:24p tokushig
* SW7231-749: refactor widget classes to hide keypress/click callback
* mechanism and simplify screen creation.  also added new menu widget.
* 
* 2   4/13/12 7:25p tokushig
* SW7231-749: add widget features
* 
* 1   4/11/12 12:13p tokushig
* SW7231-749: initial creation
* 
* 
*********************************************************************************/

#ifndef BRUTUS_WIDGET_LABEL_H__
#define BRUTUS_WIDGET_LABEL_H__

#include "widget_base.h"
#define LABEL_MAX_TEXT 255

#ifdef __cplusplus
extern "C" {
#endif

class CWidgetLabel : public CWidgetBase
{
public:
    CWidgetLabel(bwidget_engine_t   engine, 
                 CWidgetBase      * pParentWidget, 
                 MRect              geometry,
                 bwin_font_t        font,
                 bwin_t             parentWin = NULL); /* optional */
    ~CWidgetLabel(void);

    virtual void        setText(const char            * pText, 
                                bwidget_justify_horiz   justifyHoriz = bwidget_justify_horiz_max,
                                bwidget_justify_vert    justifyVert  = bwidget_justify_vert_max);
    virtual void        setTextMargin(int margin);
    virtual int         getBevel(void);
    virtual void        setBevel(int bevel);
    virtual MRect       getGeometry(void);
    virtual void        setGeometry(MRect geometry);
    virtual uint32_t    getBackgroundColor(void);
    virtual void        setBackgroundColor(uint32_t color);
    virtual bwin_font_t getFont(void);
    virtual void        setFont(bwin_font_t font);
    virtual uint32_t    getTextColor(void);
    virtual void        setTextColor(uint32_t color);
    virtual eRet        loadImage(const char             * filename, 
                                  bwin_image_render_mode   renderMode = bwin_image_render_mode_single);

protected:
    char         _pText[LABEL_MAX_TEXT];
    bwin_image_t _image;
};


#ifdef __cplusplus
}
#endif

#endif /* BRUTUS_WIDGET_LABEL_H__ */
