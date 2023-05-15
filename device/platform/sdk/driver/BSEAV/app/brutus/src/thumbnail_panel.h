/***************************************************************************
 *     Copyright (c) 2002-2009, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: thumbnail_panel.h $
 * $brcm_Revision: 4 $
 * $brcm_Date: 8/14/09 3:38p $
 *
 * Module Description: Collection of screens for pvrgui demo app
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/thumbnail_panel.h $
 * 
 * 4   8/14/09 3:38p erickson
 * PR55994: refactor thumbnail
 * 
 * 3   4/16/08 10:43a erickson
 * PR40307: add currentThumbnailPos logic
 * 
 * 2   3/22/08 2:10a erickson
 * PR40307: thumbnail demo update
 *
 * 1   3/20/08 1:49a erickson
 * PR40307: update
 *
 ***************************************************************************/
#ifndef THUMBNAIL_PANEL_H__
#define THUMBNAIL_PANEL_H__

#include <mlabel.h>
#include "bsettop.h"
#include "bsettop_os.h"
#include "bthumbnail.h"

class Video;
class Control;

/**
All time units are in seconds.
Be aware that most of brutus uses time units of milliseconds.
**/
class MThumbnailPanel : public MLabel {
public:
    MThumbnailPanel(MWidget *parent, Control *control, const MRect &rect);
    void setVideo(Video *video);

    /* increment should be +1 or -1 for jumps of one "spacing" */
    void userNavigate(int increment);

    void setPlayerPos(unsigned playerPos);

    void setViewWindow(unsigned spacing, uint32_t backgroundColor);
    void setFullView(unsigned num, uint32_t backgroundColor);

    /* get the pos of the currently highlighted thumbnail */
    unsigned currentThumbnailPos();

    void setVisible(Video *video, bool view_window, bool visible);

protected:
    Video *_video;
    Control *_control;

    unsigned _playerPos;
    unsigned _userPos;
    bool _userActive;

    unsigned _actualDisplayed;
    unsigned _spacing;
    unsigned _fullView;
    unsigned _currentThumbnailPos;
    uint32_t _backgroundColor;
    b_time_t _lastUserNavigation;
	unsigned viewing_real_time_pos[30]; //make sure it is bigger than TOTAL_DISPLAYED
	unsigned current_index;
    virtual void draw(const MRect &cliprect);
    void drawFullView(MPainter &ptr, bthumbnail_stream_t thumbnail_stream);
    void drawViewWindow(MPainter &ptr, bthumbnail_stream_t thumbnail_stream);
};

#endif /* THUMBNAIL_PANEL_H__ */
