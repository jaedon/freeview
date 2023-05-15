/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: widget_check_button.h $
* $brcm_Revision: 1 $
* $brcm_Date: 5/4/12 2:05p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/widgets/widget_check_button.h $
* 
* 1   5/4/12 2:05p tokushig
* SW7231-749: add buffers screen, checkbox widget, and new fonts
* 
* 
* 
*********************************************************************************/

#ifndef BRUTUS_WIDGET_CHECK_BUTTON_H__
#define BRUTUS_WIDGET_CHECK_BUTTON_H__

#include "widget_button.h"

#ifdef __cplusplus
extern "C" {
#endif

class CWidgetCheckButton : public CWidgetButton
{
public:
    CWidgetCheckButton(bwidget_engine_t   engine, 
                       CWidgetBase      * pParentWidget,
                       MRect              geometry,
                       bwin_font_t        font,
                       bwin_t             parentWin = NULL); /* optional */

    virtual void setCheck(bool bChecked = false);
    virtual bool isChecked(void) { return _bChecked; };

    virtual void setBoxColor(uint32_t color);
    virtual uint32_t getBoxColor(void) { return _boxColor; };
    virtual void setBoxSize(uint16_t size) { _boxSize = size; };
    virtual uint16_t getBoxSize(void) { return _boxSize; };

    virtual void setCheckColor(uint32_t color);
    virtual uint32_t getCheckColor(void) { return _checkColor; };

protected:
    bool     _bChecked;
    uint32_t _boxColor;
    uint16_t _boxSize;
    uint32_t _checkColor;
};


#ifdef __cplusplus
}
#endif

#endif /* BRUTUS_WIDGET_CHECK_BUTTON_H__ */
