/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: widget_grid.h $
* $brcm_Revision: 5 $
* $brcm_Date: 10/19/12 10:37a $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/widgets/widget_grid.h $
* 
* 5   10/19/12 10:37a tokushig
* SW7231-749: fix brutus shutdown by freeing resources in the proper
* order.
* 
* 4   10/11/12 11:17a tokushig
* SW7231-749: fix memory leak - added points are never freed
* 
* 3   10/10/12 9:54p jrubio
* SW7346-1004: use int16 instead of uint16 make sure widget is centered
* 
* 2   10/8/12 10:54p jrubio
* SW7346-1004: add functions to access gridPts
* 
* 1   10/8/12 4:19p jrubio
* SW7346-1004: add widget grid
* 
* 1   5/10/12 3:45p tokushig
* SW7231-749: add buffers/tsm popup including new meter widget
* 
* 
*********************************************************************************/

#ifndef BRUTUS_WIDGET_GRID_H__
#define BRUTUS_WIDGET_GRID_H__

#include "widget_label.h"

#ifdef __cplusplus
extern "C" {
#endif

class CWidgetGrid : public CWidgetLabel
{
public:
    CWidgetGrid(bwidget_engine_t   engine, 
                 CWidgetBase      * pParentWidget, 
                 MRect              geometry,
                 bwin_font_t        font,
                 bwin_t             parentWin = NULL); /* optional */
    ~CWidgetGrid(void);

    bool isGridVisible(void) { return _showGrid; };

    void     showMeter(bool show);
    int16_t  getLevel(void) { return _level; };
    void     setLevel(int16_t level);
    uint32_t getColor(void) { return _color; };
    void     setColor(uint32_t color);
    eRet     loadMeterImage(const char             * filename, 
                               bwin_image_render_mode   renderMode = bwin_image_render_mode_single);
    bwin_image_t getMeterImage(void) { return _imageGrid; };
    bwin_image_render_mode getMeterRenderMode(void) { return _imageRenderMode; };

    void     setRangeIndicator(bool showRange) { _showRange = showRange; };
    bool     isRangeIndicatorVisible(void) { return _showRange; };
    void     setRangeIndicatorSize(uint16_t size);
    uint16_t getRangeIndicatorSize(void) { return _rangeSize; };
    void setNumPoints(uint32_t numPoints) {_numPoints = numPoints; };
    uint32_t getNumPoints(void);
    void addCoordinate(int16_t x, int16_t y);
    void getCoordinate(int16_t * x, int16_t * y, int i); 
    void clearGrid(void);
    
    typedef struct gridPoints{
       int16_t    x;
       int16_t    y;
    }gridPoints;

    
protected:
    bool                    _showGrid;
    bwin_image_t            _imageGrid;
    bwin_image_render_mode  _imageRenderMode;
    uint32_t                _color;
    int16_t                 _level;
    bool                    _showRange;
    int16_t                 _rangeSize;
    uint32_t                _numPoints;
    MAutoList <gridPoints>  _gridPoints; 
};


#ifdef __cplusplus
}
#endif

#endif /* BRUTUS_WIDGET_METER_H__ */
