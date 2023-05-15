/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: widget_grid.cpp $
* $brcm_Revision: 5 $
* $brcm_Date: 10/19/12 10:37a $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/widgets/widget_grid.cpp $
* 
* 5   10/19/12 10:37a tokushig
* SW7231-749: fix brutus shutdown by freeing resources in the proper
* order.
* 
* 4   10/11/12 11:16a tokushig
* SW7231-749: change draw to fill entire control.  adjust grid axis color
* to match bevel colors.  restore focus capability
* 
* 3   10/10/12 9:53p jrubio
* SW7346-1004: make sure widget is centered
* 
* 2   10/8/12 10:45p jrubio
* SW7346-1004: fix x/y mapping
* 
* 1   10/8/12 4:19p jrubio
* SW7346-1004: add widget grid
* 
* 
*********************************************************************************/

#include "widget_grid.h"
#include "string.h"
#include "bdbg.h"

BDBG_MODULE(widget_grid);

#define ABS(x) (((x)<0)?-(x):(x))

/* override default label draw routine to add meter option */
void grid_bwidget_draw(bwidget_t widget, const bwin_rect *cliprect)
{
    bwin_t win = bwidget_win(widget);
    bwin_settings win_settings;
    int right, bottom, middle, half;
    blabel_settings * label_settings;
    blabel_settings labelSettings;
    CWidgetGrid * pGrid;
    bool down = false;
    

    blabel_get(widget, &labelSettings);
    label_settings = &labelSettings;

    pGrid = (CWidgetGrid *)label_settings->widget.data;
    BDBG_ASSERT(NULL != pGrid);
    
    /* TODO: for efficieny, we may want bwidgets to have access to bwin private structures. */
    bwin_get(win, &win_settings);
    
    /* only draw if something's within the cliprect */
    if (cliprect && !BWIN_RECT_ISINTERSECTING(&win_settings.rect, cliprect)) {
        return;
    }
    
    right = win_settings.rect.x + win_settings.rect.width;
    middle = win_settings.rect.x + win_settings.rect.width/2;
    half = win_settings.rect.height/2;
    bottom = win_settings.rect.y + win_settings.rect.height;

    if (label_settings->image) {
        bwin_image_render(win, label_settings->image, label_settings->render_mode, &win_settings.rect, NULL, cliprect);
    } 
    else {
        bwin_fill_rect(win, &win_settings.rect, label_settings->background_color, cliprect);
        
        if (label_settings->bevel) {
            int i;
            /* top */
            for (i=0;i<label_settings->bevel;i++) {
                bwin_draw_line(win, i, i, win_settings.rect.width-i, i,
                    label_settings->bevel_color[down?2:0], cliprect);
            }
            /* right */
            for (i=0;i<label_settings->bevel;i++) {
                bwin_draw_line(win, right-i-1, i, right-i-1, win_settings.rect.height-i,
                    label_settings->bevel_color[down?3:1], cliprect);
            }
            /* bottom */
            for (i=0;i<label_settings->bevel;i++) {
                bwin_draw_line(win, i, bottom-i-1, win_settings.rect.width-i, bottom-i-1,
                    label_settings->bevel_color[down?0:2], cliprect);
            }
            /* left */
            for (i=0;i<label_settings->bevel;i++) {
                bwin_draw_line(win, i, i, i, win_settings.rect.height-i,
                    label_settings->bevel_color[down?1:3], cliprect);
            }
        }
    }

    /* X Axis - use average color between left/right bevels */
    bwin_draw_line(win, middle,win_settings.rect.y, middle,bottom ,
                   ((label_settings->bevel_color[1] + label_settings->bevel_color[3]) / 2) | 0xFF000000,
                   cliprect);
        
    /* Y Axis - use average color between top/bottom bevels */
    bwin_draw_line(win, win_settings.rect.x,half, win_settings.rect.width, half,
                   ((label_settings->bevel_color[0] + label_settings->bevel_color[2]) / 2) | 0xFF000000,
                   cliprect);
    
    /* Draw X/Y Coordiantes */
    {
        int w,h,i,max;
        int16_t gridx,gridy;

        max = pGrid->getNumPoints();

        w = win_settings.rect.width - label_settings->bevel;
        h = win_settings.rect.height - label_settings->bevel;

        for (i=0; i < max; i++)
        {
            // find the center
            pGrid->getCoordinate(&gridx,&gridy,i);
            int x = middle;
            int y = half;

            // convert width/height from 32768/-32767 to +/- width/2 and height/2
            x += gridx * w / 65535;
            y += gridy * h / 65535;
            BDBG_MSG(("%d,%d => %d,%d", gridx,gridy,x,y));
            unsigned index = y*win_settings.rect.width + x;

            /* prevent the app from crashing if our algorithm isn't quite right */
            if (x >= (int)win_settings.rect.width || y >= (int)win_settings.rect.height || x < 0 || y < 0) {
                BDBG_ERR(("Invalid constellation value: %d %d", x, y));
                continue;
            }

            /* Draw Constellation points */
            bwin_fill_ellipse(win, x, y,2, 2, 0xFF000000, cliprect);
        }
    }
    
    if (widget == bwidget_get_focus(pGrid->getEngine())) {
        /* draw focus */
        int i;
        for (i=0;i<2;i++) {
            bwin_draw_line(win, i, i, win_settings.rect.width-i, i,
                label_settings->focus_color, cliprect);
            bwin_draw_line(win, right-i-1, i, right-i-1, win_settings.rect.height-i,
                label_settings->focus_color, cliprect);
            bwin_draw_line(win, i, bottom-i-1, win_settings.rect.width-i, bottom-i-1,
                label_settings->focus_color, cliprect);
            bwin_draw_line(win, i, i, i, win_settings.rect.height-i,
                label_settings->focus_color, cliprect);
        }
    }
}

CWidgetGrid::CWidgetGrid(bwidget_engine_t   engine, 
                           CWidgetBase      * pParentWidget, 
                           MRect              geometry,
                           bwin_font_t        font,
                           bwin_t             parentWin) :
    CWidgetLabel(engine, pParentWidget, geometry, font, parentWin),
    _showGrid(false),
    _imageGrid(NULL),
    _imageRenderMode(bwin_image_render_mode_tile),
    _color(0xFF80C42F),
    _level(0),
    _showRange(false),
    _rangeSize(1)
{
    blabel_settings labelSettings;

    /* use custom draw routine so we can draw the background Meter bar */
    blabel_get(getWidget(), &labelSettings);
    labelSettings.text_justify_horiz = bwidget_justify_horiz_center;
    labelSettings.widget.draw = grid_bwidget_draw;
    blabel_set(getWidget(), &labelSettings);
}

CWidgetGrid::~CWidgetGrid()
{
    if (NULL != _imageGrid)
    {
        bwin_image_close(_imageGrid);
    }
}

void CWidgetGrid::setColor(uint32_t color)
{
    _color = color;
    bwin_repaint(bwidget_win(getWidget()), NULL);
}

eRet CWidgetGrid::loadMeterImage(const char * filename, bwin_image_render_mode renderMode)
{
    eRet            ret           = eRet_Ok;
    blabel_settings labelSettings;

    BDBG_ASSERT(NULL != filename);

    if (NULL != _imageGrid)
    {
        bwin_image_close(_imageGrid);
    }

    _imageGrid = bwin_image_load(getWinEngine(), filename);
    CHECK_PTR_GOTO("unable to load bwin image", _imageGrid, ret, eRet_ExternalError, error);

    _imageRenderMode = renderMode;

    bwin_repaint(bwidget_win(getWidget()), NULL);

error:
    return ret;
}

void CWidgetGrid::showMeter(bool show)
{
    _showGrid = show;

    bwin_repaint(bwidget_win(getWidget()), NULL);
}

/* sets the Meter value (range -32767-32767) */
void CWidgetGrid::setLevel(int16_t level)
{
    _level = level;

    bwin_repaint(bwidget_win(getWidget()), NULL);
}

/* Add an X,Y pair */
void CWidgetGrid::addCoordinate(int16_t x, int16_t y)
{
   gridPoints * gridPts = new gridPoints;
   
   gridPts->x = x;
   gridPts->y = y;

   BDBG_MSG(("Add coordinates %d %d",x, y));
   _gridPoints.add(gridPts);

   bwin_repaint(bwidget_win(getWidget()), NULL );
}

/* Get Coordinates */
void CWidgetGrid::getCoordinate(int16_t * x, int16_t * y, int i)
{
    MListItr<gridPoints> itr(&_gridPoints);
    gridPoints * pGridPts = NULL;

    if (x == NULL || y == NULL)
    {
        BDBG_ERR((" cannot pass in NULL pointers"));
        return;
    }

    if (i > _gridPoints.total() || i < 0)
    {
        BDBG_ERR(("Cannot request index %d",i));
        return;
    }

    pGridPts = itr.at(i);
    if (pGridPts == NULL)
    {
        BDBG_WRN(("Cannot request Grid Points at index %d, Its NULL",i));
        return;
    }

    *x = pGridPts->x;
    *y = pGridPts->y;
}


/* Number of Actual Points */
uint32_t CWidgetGrid::getNumPoints()
{
  return _gridPoints.total();

}

/* Clear Grid */
void CWidgetGrid::clearGrid()
{
   _gridPoints.clear();
}
