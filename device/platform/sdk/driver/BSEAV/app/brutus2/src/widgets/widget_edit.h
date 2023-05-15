/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: widget_edit.h $
* $brcm_Revision: 1 $
* $brcm_Date: 4/20/12 3:58p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/widgets/widget_edit.h $
* 
* 1   4/20/12 3:58p tokushig
* SW7231-749: added edit widgets
* 
* 
*********************************************************************************/

#ifndef BRUTUS_WIDGET_EDIT_H__
#define BRUTUS_WIDGET_EDIT_H__

#include "widget_button.h"
#define EDIT_MAX_TEXT 255

#ifdef __cplusplus
extern "C" {
#endif

class CWidgetEdit : public CWidgetButton
{
public:
    CWidgetEdit(bwidget_engine_t   engine, 
                CWidgetBase      * pParentWidget,
                MRect              geometry,
                bwin_font_t        font,
                bwin_t             parentWin = NULL); /* optional */

    MString getText(void);
    uint16_t getPosition(void) { return _position; };
    void setPosition(uint16_t position);
    void addChar(char ch, uint16_t * pPosition);
    void removeChar(uint16_t * pPosition, bool right);

    virtual eRet onKeyDown(bwidget_t widget, bwidget_key key);

protected:
    uint16_t    _position;
};


#ifdef __cplusplus
}
#endif

#endif /* BRUTUS_WIDGET_EDIT_H__ */
