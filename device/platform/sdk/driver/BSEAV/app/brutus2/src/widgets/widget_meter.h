/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: widget_meter.h $
* $brcm_Revision: 2 $
* $brcm_Date: 10/19/12 10:36a $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/widgets/widget_meter.h $
* 
* 2   10/19/12 10:36a tokushig
* SW7231-749: fix brutus shutdown by freeing resources in the proper
* order.
* 
* 1   5/10/12 3:45p tokushig
* SW7231-749: add buffers/tsm popup including new meter widget
* 
* 
*********************************************************************************/

#ifndef BRUTUS_WIDGET_METER_H__
#define BRUTUS_WIDGET_METER_H__

#include "widget_label.h"

#ifdef __cplusplus
extern "C" {
#endif

class CWidgetMeter : public CWidgetLabel
{
public:
    CWidgetMeter(bwidget_engine_t   engine, 
                 CWidgetBase      * pParentWidget, 
                 MRect              geometry,
                 bwin_font_t        font,
                 bwin_t             parentWin = NULL); /* optional */
    ~CWidgetMeter(void);

    bool isMeterVisible(void) { return _showMeter; };

    void     showMeter(bool show);
    int16_t  getLevel(void) { return _level; };
    void     setLevel(int16_t level);
    uint32_t getColor(void) { return _color; };
    void     setColor(uint32_t color);
    eRet     loadMeterImage(const char             * filename, 
                               bwin_image_render_mode   renderMode = bwin_image_render_mode_single);
    bwin_image_t getMeterImage(void) { return _imageMeter; };
    bwin_image_render_mode getMeterRenderMode(void) { return _imageRenderMode; };

    void     setRangeIndicator(bool showRange) { _showRange = showRange; };
    bool     isRangeIndicatorVisible(void) { return _showRange; };
    void     setRangeIndicatorSize(uint16_t size);
    uint16_t getRangeIndicatorSize(void) { return _rangeSize; };

protected:
    bool                   _showMeter;
    bwin_image_t           _imageMeter;
    bwin_image_render_mode _imageRenderMode;
    uint32_t               _color;
    int16_t                _level;
    bool                   _showRange;
    uint16_t               _rangeSize;
};


#ifdef __cplusplus
}
#endif

#endif /* BRUTUS_WIDGET_METER_H__ */
