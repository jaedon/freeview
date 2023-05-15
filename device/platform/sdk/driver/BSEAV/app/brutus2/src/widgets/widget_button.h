/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: widget_button.h $
* $brcm_Revision: 8 $
* $brcm_Date: 8/8/12 4:57p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/widgets/widget_button.h $
* 
* 8   8/8/12 4:57p tokushig
* SW7231-749: add font get/set methods
* 
* 7   8/2/12 11:01a tokushig
* SW7231-749: Add missing destructor to button class
* 
* 6   6/26/12 4:00p tokushig
* SW7231-749: add modal message box capability
* 
* 5   5/25/12 12:27a tokushig
* SW7231-749: added method to get current button text
* 
* 4   4/20/12 2:45p tokushig
* SW7231-749: add support for scan menu
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

#ifndef BRUTUS_WIDGET_BUTTON_H__
#define BRUTUS_WIDGET_BUTTON_H__

#include "widget_base.h"
#include "mstring.h"
#define BUTTON_MAX_TEXT 255

#ifdef __cplusplus
extern "C" {
#endif

class CWidgetButton : public CWidgetBase
{
public:
    CWidgetButton(bwidget_engine_t   engine, 
                  CWidgetBase      * pParentWidget,
                  MRect              geometry,
                  bwin_font_t        font,
                  bwin_t             parentWin = NULL); /* optional */
    ~CWidgetButton(void);

    virtual MString     getText(void) { return MString(_pText); };
    virtual void        setText(const char            * pText, 
                                bwidget_justify_horiz   justifyHoriz = bwidget_justify_horiz_max,
                                bwidget_justify_vert    justifyVert  = bwidget_justify_vert_max);
    virtual void        setTextMargin(int margin);
    virtual void        setBevel(int bevel);
    virtual MRect       getGeometry(void);
    virtual void        setGeometry(MRect geometry);
    virtual uint32_t    getTextColor(void);
    virtual void        setTextColor(uint32_t color);
    virtual uint32_t    getBackgroundColor(void);
    virtual void        setBackgroundColor(uint32_t color);
    virtual bwin_font_t getFont(void);
    virtual void        setFont(bwin_font_t font);

    virtual eRet        loadImage(const char             * filename, 
                                  bwin_image_render_mode   renderMode = bwin_image_render_mode_single);
protected:
    char         _pText[BUTTON_MAX_TEXT];
    bwin_image_t _image;
};


#ifdef __cplusplus
}
#endif

#endif /* BRUTUS_WIDGET_BUTTON_H__ */
