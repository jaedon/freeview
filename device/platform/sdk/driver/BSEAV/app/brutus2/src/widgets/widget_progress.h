/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: widget_progress.h $
* $brcm_Revision: 3 $
* $brcm_Date: 10/19/12 10:36a $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/widgets/widget_progress.h $
* 
* 3   10/19/12 10:36a tokushig
* SW7231-749: fix brutus shutdown by freeing resources in the proper
* order.
* 
* 2   5/10/12 3:45p tokushig
* SW7231-749: add buffers/tsm popup including new meter widget
* 
* 1   4/16/12 5:52p tokushig
* SW7231-749: add progess label widget
* 
* 
*********************************************************************************/

#ifndef BRUTUS_WIDGET_PROGRESS_H__
#define BRUTUS_WIDGET_PROGRESS_H__

#include "widget_label.h"

#ifdef __cplusplus
extern "C" {
#endif

class CWidgetProgress : public CWidgetLabel
{
public:
    CWidgetProgress(bwidget_engine_t   engine, 
                    CWidgetBase      * pParentWidget, 
                    MRect              geometry,
                    bwin_font_t        font,
                    bwin_t             parentWin = NULL); /* optional */
    ~CWidgetProgress(void);

    bool isProgressVisible(void) { return _showProgress; };

    void     showProgress(bool show);
    uint16_t getLevel(void) { return _level; };
    void     setLevel(uint16_t level);
    uint32_t getColor(void) { return _color; };
    void     setColor(uint32_t color);
    eRet     loadProgressImage(const char             * filename, 
                               bwin_image_render_mode   renderMode = bwin_image_render_mode_single);
    bwin_image_t getProgressImage(void) { return _imageProgress; };
    bwin_image_render_mode getProgressRenderMode(void) { return _imageRenderMode; };

    void     setRangeIndicator(bool showRange) { _showRange = showRange; };
    bool     isRangeIndicatorVisible(void) { return _showRange; };
    void     setRangeIndicatorSize(uint16_t size);
    uint16_t getRangeIndicatorSize(void) { return _rangeSize; };

protected:
    bool                   _showProgress;
    bwin_image_t           _imageProgress;
    bwin_image_render_mode _imageRenderMode;
    uint32_t               _color;
    uint16_t               _level;
    bool                   _showRange;
    uint16_t               _rangeSize;
};


#ifdef __cplusplus
}
#endif

#endif /* BRUTUS_WIDGET_PROGRESS_H__ */
