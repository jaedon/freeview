/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: widget_progress.cpp $
* $brcm_Revision: 6 $
* $brcm_Date: 10/19/12 10:36a $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/widgets/widget_progress.cpp $
* 
* 6   10/19/12 10:36a tokushig
* SW7231-749: fix brutus shutdown by freeing resources in the proper
* order.
* 
* 5   8/8/12 6:00p tokushig
* SW7231-749: shrink focus indicator size
* 
* 4   5/25/12 12:34a tokushig
* SW7231-749: fixed problem where top/bottom vertical justification were
* switched.
* 
* 3   5/10/12 3:45p tokushig
* SW7231-749: add buffers/tsm popup including new meter widget
* 
* 2   4/20/12 2:45p tokushig
* SW7231-749: add support for scan menu
* 
* 1   4/16/12 5:52p tokushig
* SW7231-749: add progess label widget
* 
* 
*********************************************************************************/

#include "widget_progress.h"
#include "string.h"
#include "bdbg.h"

BDBG_MODULE(widget_progress);


/* override default label draw routine to add progress option */
void progress_bwidget_draw(bwidget_t widget, const bwin_rect *cliprect)
{
    bwin_t win = bwidget_win(widget);
    bwin_settings win_settings;
    int right, bottom;
    blabel_settings * label_settings;
    blabel_settings labelSettings;
    CWidgetProgress * pProgress;
    bool down = false;

    blabel_get(widget, &labelSettings);
    label_settings = &labelSettings;

    pProgress = (CWidgetProgress *)label_settings->widget.data;
    BDBG_ASSERT(NULL != pProgress);
    
    /* TODO: for efficieny, we may want bwidgets to have access to bwin private structures. */
    bwin_get(win, &win_settings);
    
    /* only draw if something's within the cliprect */
    if (cliprect && !BWIN_RECT_ISINTERSECTING(&win_settings.rect, cliprect)) {
        return;
    }
    
    right = win_settings.rect.x + win_settings.rect.width;
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

    /* draw range indicator if necessary */
    if (true == pProgress->isRangeIndicatorVisible())
    {
        uint16_t level;
        bwin_rect rectProgress = win_settings.rect;
        int spacer = label_settings->bevel + 1;

        rectProgress.x = spacer;
        rectProgress.y = rectProgress.y + rectProgress.height / 2;
        rectProgress.width = rectProgress.width - (spacer * 2);
        rectProgress.height = pProgress->getRangeIndicatorSize();

        if (NULL != pProgress->getProgressImage()) {
            bwin_image_render(win, pProgress->getProgressImage(), pProgress->getProgressRenderMode(), &rectProgress, NULL, cliprect);
        } 
        else {
            bwin_fill_rect(win, &rectProgress, 0xffCCCCCC, cliprect);
        }
    }

    /* draw progress if necessary*/
    if (true == pProgress->isProgressVisible()) {
        uint16_t level;
        bwin_rect rectProgress = win_settings.rect;
        int spacer = label_settings->bevel + 1;

        rectProgress.x = spacer;
        rectProgress.y = spacer;
        rectProgress.width = rectProgress.width - (spacer * 2);
        rectProgress.height = rectProgress.height - (spacer * 2);

        /* adjust progress rect width based on actual level */
        level = pProgress->getLevel();
        rectProgress.width = rectProgress.width * level / (uint16_t)-1;


        if (NULL != pProgress->getProgressImage()) {
            bwin_image_render(win, pProgress->getProgressImage(), pProgress->getProgressRenderMode(), &rectProgress, NULL, cliprect);
        } 
        else {
            bwin_fill_rect(win, &rectProgress, pProgress->getColor(), cliprect);
        }
    }

    if (widget == bwidget_get_focus(pProgress->getEngine())) {
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

    if (label_settings->text) {
        int width, height, base, x, y;
        bwin_measure_text(label_settings->font, label_settings->text, -1,
            &width, &height, &base);
        
        /* calculate vertical justification */
        y = 0;
        if (label_settings->text_justify_vert == bwidget_justify_vert_middle) {
            y = (win_settings.rect.height - height) / 2;
        } 
        else if (label_settings->text_justify_vert == bwidget_justify_vert_bottom) {
            y += label_settings->text_margin;
        }
        else { /* bwidget_justify_vert_top */
            y = win_settings.rect.height - height;
            y -= label_settings->text_margin;
        }

        /* calculate horizontal justification */
        x = 0;
        if (label_settings->text_justify_horiz == bwidget_justify_horiz_center) {
            x = (win_settings.rect.width - width) / 2;
        } 
        else if (label_settings->text_justify_horiz == bwidget_justify_horiz_right) {
            x = win_settings.rect.width - width;
            x -= label_settings->text_margin;
            x -= label_settings->bevel;
        }
        else { /* bwidget_justify_horiz_left */
            x += label_settings->text_margin;
            x += label_settings->bevel;
        }

        bwin_draw_text(win, label_settings->font, x, y,
            label_settings->text, -1, label_settings->text_color,
            cliprect);
    }
}

CWidgetProgress::CWidgetProgress(bwidget_engine_t   engine, 
                                 CWidgetBase      * pParentWidget, 
                                 MRect              geometry,
                                 bwin_font_t        font,
                                 bwin_t             parentWin) :
    CWidgetLabel(engine, pParentWidget, geometry, font, parentWin),
    _showProgress(false),
    _imageProgress(NULL),
    _imageRenderMode(bwin_image_render_mode_tile),
    _color(0xFF80C42F),
    _level(0),
    _showRange(false),
    _rangeSize(1)
{
    blabel_settings labelSettings;

    /* use custom draw routine so we can draw the background progress bar */
    blabel_get(getWidget(), &labelSettings);
    labelSettings.text_justify_horiz = bwidget_justify_horiz_center;
    labelSettings.widget.draw = progress_bwidget_draw;
    blabel_set(getWidget(), &labelSettings);
}

CWidgetProgress::~CWidgetProgress()
{
    if (NULL != _imageProgress)
    {
        bwin_image_close(_imageProgress);
    }
}

void CWidgetProgress::setColor(uint32_t color)
{
    _color = color;
    bwin_repaint(bwidget_win(getWidget()), NULL);
}

eRet CWidgetProgress::loadProgressImage(const char * filename, bwin_image_render_mode renderMode)
{
    eRet            ret           = eRet_Ok;
    blabel_settings labelSettings;

    BDBG_ASSERT(NULL != filename);

    if (NULL != _imageProgress)
    {
        bwin_image_close(_imageProgress);
    }

    _imageProgress = bwin_image_load(getWinEngine(), filename);
    CHECK_PTR_GOTO("unable to load bwin image", _imageProgress, ret, eRet_ExternalError, error);

    _imageRenderMode = renderMode;

    bwin_repaint(bwidget_win(getWidget()), NULL);

error:
    return ret;
}

void CWidgetProgress::showProgress(bool show)
{
    _showProgress = show;

    bwin_repaint(bwidget_win(getWidget()), NULL);
}

/* sets the progress value (range 0-65535) */
void CWidgetProgress::setLevel(uint16_t level)
{
    _level = level;

    bwin_repaint(bwidget_win(getWidget()), NULL);
}
