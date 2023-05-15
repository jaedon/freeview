/***************************************************************************
 *     Copyright (c) 2002-2009, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: thumbnail_panel.cpp $
 * $brcm_Revision: 6 $
 * $brcm_Date: 8/14/09 3:38p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/thumbnail_panel.cpp $
 * 
 * 6   8/14/09 3:38p erickson
 * PR55994: refactor thumbnail
 *
 * 5   4/16/08 10:43a erickson
 * PR40307: add currentThumbnailPos logic
 *
 * 4   4/15/08 4:11p erickson
 * PR40307: rework display algo to remove holes for still decode timeouts
 *
 * 3   3/22/08 2:10a erickson
 * PR40307: thumbnail demo update
 *
 * 2   3/20/08 3:19a erickson
 * PR40307: impl localized search for thumbnails
 *
 * 1   3/20/08 1:49a erickson
 * PR40307: update
 *
 ***************************************************************************/
#include "thumbnail_panel.h"
#include "control.h"
#include "cfgsettings.h"
#include "bstd.h"

BDBG_MODULE(thumbnail_panel);

#define TOTAL_DISPLAYED 5 /* TODO: configure */
#define BORDER_WIDTH 3
#define THUMBNAIL_GAP 3

MThumbnailPanel::MThumbnailPanel(MWidget *parent, Control *control, const MRect &rect) : MLabel(parent, rect, (const char *)NULL, "thumbnailpanel")
{
    _video = NULL;
    _control = control;
    _userPos = 0;
    _playerPos = 0;
    _userActive = false;
    _spacing = 10;
    _fullView = false;
    b_time_get(&_lastUserNavigation);
    _backgroundColor = control->cfg()->getInt(Config::BACKGROUND_COLOR);
}

void MThumbnailPanel::setViewWindow(unsigned spacing, uint32_t backgroundColor)
{
    _spacing = spacing;
    _fullView = false;
    _backgroundColor = backgroundColor;
}

void MThumbnailPanel::setFullView(unsigned num, uint32_t backgroundColor)
{
    BSTD_UNUSED(num);
    _fullView = true;
    _backgroundColor = backgroundColor;
}

static void draw_outline(bsurface_t surface, const bsettop_rect *rect, uint32_t color)
{
    int x,y;
    int bottom = rect->y + rect->height;
    int right = rect->x + rect->width;
    bsurface_memory mem;

    bsurface_get_memory(surface, &mem);

    for (y=rect->y;y<rect->y+BORDER_WIDTH;y++) {
        for (x=rect->x;x<right;x++) {
            ((uint32_t*)((uint8_t*)mem.buffer+y*mem.pitch))[x] = color;
        }
    }
    for (y=bottom-BORDER_WIDTH;y<bottom;y++) {
        for (x=rect->x;x<right;x++) {
            ((uint32_t*)((uint8_t*)mem.buffer+y*mem.pitch))[x] = color;
        }
    }
    for (y=rect->y;y<bottom;y++) {
        for (x=rect->x;x<rect->x+BORDER_WIDTH;x++) {
            ((uint32_t*)((uint8_t*)mem.buffer+y*mem.pitch))[x] = color;
        }
        for (x=right-BORDER_WIDTH;x<right;x++) {
            ((uint32_t*)((uint8_t*)mem.buffer+y*mem.pitch))[x] = color;
        }
    }
}

void MThumbnailPanel::draw(const MRect &cliprect)
{
    bthumbnail_stream_t thumbnail_stream;

    MLabel::draw(cliprect);
    if (!_video) return;

    MPainter ptr(this, cliprect);

    thumbnail_stream = _video->thumbnail_data.stream;
    if (!thumbnail_stream) return;

    if (_fullView) {
        drawFullView(ptr, thumbnail_stream);
    }
    else {
        drawViewWindow(ptr, thumbnail_stream);
    }
}

void MThumbnailPanel::drawFullView(MPainter &ptr, bthumbnail_stream_t thumbnail_stream)
{
    bthumbnail_t thumbnail_array[TOTAL_DISPLAYED];
    bsurface_t fb = _control->display()->mainsurface;
    bsettop_rect dest;

    if (bthumbnail_stream_get_full_view_thumbnails(thumbnail_stream, thumbnail_array, TOTAL_DISPLAYED, &_actualDisplayed)) {
        return;
    }

    MPoint point = ptr.mapToGlobal(MPoint(0, 0));
    dest.x = point.x() + _control->display()->view_rect.x;
    dest.y = point.y() + _control->display()->view_rect.y;
    dest.width = ((width()-THUMBNAIL_GAP)/TOTAL_DISPLAYED) - THUMBNAIL_GAP;
    dest.height = height();

    for (unsigned i=0;i<_actualDisplayed;i++) {
        /* To integrate thumbnails within the windowing system, we need to invoke several layers of software.
        For performance and a quick impl, I'm going to render straight to the framebuffer. */

        bthumbnail_data data;
        if (!thumbnail_array[i] || bthumbnail_get_data(thumbnail_array[i], &data)) continue;

        bsurface_copy(fb, &dest, (bsurface_t)data.surface, NULL);
        dest.x += dest.width + THUMBNAIL_GAP;
    }

    /* TODO: this is only needed because we are over-drawing on the right side of this Widget */
    /* erase thumbnails that we didn't get */
    for (unsigned i=0;i<TOTAL_DISPLAYED;i++) {
        if (i >= _actualDisplayed || !thumbnail_array[i]) {
            bsurface_fill(fb, &dest, _backgroundColor);
            dest.x += dest.width + THUMBNAIL_GAP;
        }
    }
}

void MThumbnailPanel::drawViewWindow(MPainter &ptr, bthumbnail_stream_t thumbnail_stream)
{
    bthumbnail_t thumbnail_array[TOTAL_DISPLAYED];
    bsurface_t fb = _control->display()->mainsurface;
    bsettop_rect dest;
    bthumbnail_stream_settings settings;

    unsigned focus_pos = _userActive?_userPos:_playerPos; /* player/user position */
    unsigned pos = focus_pos; /* where we will display thumbnails from */

    /* apply bounds */
    bthumbnail_stream_get_settings(thumbnail_stream, &settings);

    settings.last_time -= settings.last_time % _spacing;

    if (settings.last_time - settings.first_time < _spacing * TOTAL_DISPLAYED) {
        pos = settings.first_time;
    }
    else if ((int)settings.last_time - (int)pos <= (int)_spacing*3) {
        pos = settings.last_time - _spacing*4;
    }
    else if ((int)pos - (int)settings.first_time > (int)_spacing*2) {
        pos -= _spacing*2;
    }
    else {
        pos = settings.first_time;
    }
    pos -= pos%_spacing;

    settings.view_window.first_time = pos;
    settings.view_window.last_time = settings.view_window.first_time + _spacing*TOTAL_DISPLAYED;
    settings.view_window.spacing = _spacing;
    bthumbnail_stream_set_settings(thumbnail_stream, &settings);

    if (bthumbnail_stream_get_thumbnails(thumbnail_stream, settings.view_window.first_time, settings.view_window.last_time, thumbnail_array, TOTAL_DISPLAYED, &_actualDisplayed)) {
        return;
    }
    BDBG_MSG(("got %d thumbnails at pos %d (%d..%d), focus_pos %d", _actualDisplayed, pos, settings.first_time, settings.last_time, focus_pos));

    MPoint point = ptr.mapToGlobal(MPoint(0, 0));
    dest.x = point.x() + _control->display()->view_rect.x;
    dest.y = point.y() + _control->display()->view_rect.y;
    dest.width = ((width()-THUMBNAIL_GAP)/TOTAL_DISPLAYED) - THUMBNAIL_GAP;
    dest.height = height();

    bsettop_rect closest_rect;
    int closest_time = -1;
    unsigned diff = 0xFFFFFFFF;
	current_index = 0xffff;
    for (unsigned i=0;i<TOTAL_DISPLAYED;i++) {
		viewing_real_time_pos[i] = 0xffff;
        unsigned target_time = settings.view_window.first_time + _spacing * i;

        /* To integrate thumbnails within the windowing system, we need to invoke several layers of software.
        For performance and a quick impl, I'm going to render straight to the framebuffer. */

        bthumbnail_data data;

        if (i < _actualDisplayed && thumbnail_array[i]) {
            if (bthumbnail_get_data(thumbnail_array[i], &data)) {
                data.surface = NULL;
            }
        }
        else {
            data.surface = NULL;
        }

#if 0
/* TODO: this code has accurate spacing, but shows holes if a thumbnail is missing. */
        if (data.time == target_time && data.surface) {
#else
        if (data.surface) {
#endif
			viewing_real_time_pos[i] = data.time;
            BDBG_MSG(("  thumbnail %d (%d,%d,%d,%d)", data.time, dest.x, dest.y, dest.width, dest.height));
            bsurface_copy(fb, &dest, (bsurface_t)data.surface, NULL);
        }
        else {
            /* no thumbnail, but draw the outline */
            draw_outline(fb, &dest, 0xFF333333);
        }

        /* we should be at or past the focus point */
        if (target_time <= focus_pos && focus_pos-target_time<diff) {
            diff = focus_pos-target_time;
            closest_time = target_time;
			current_index = i;
            BKNI_Memcpy(&closest_rect, &dest, sizeof(dest));
        }
        else if (i == 0) {
			current_index = i;
            closest_time = target_time;
            BKNI_Memcpy(&closest_rect, &dest, sizeof(dest));
        }

        dest.x += dest.width + THUMBNAIL_GAP;
    }

    if (closest_time != -1) {
        bsurface_sync(fb);
        draw_outline(fb, &closest_rect, 0xFF00FF00);
        _currentThumbnailPos = closest_time;
    }
    else {
        _currentThumbnailPos = 0;
    }
}

void MThumbnailPanel::userNavigate(int increment)
{
    bthumbnail_stream_settings settings;

    if (!_video || !_video->thumbnail_data.stream) return;

    if (!_userActive) {
        _userPos = _playerPos;
    }

    bthumbnail_stream_get_settings(_video->thumbnail_data.stream, &settings);

    increment *= _spacing;
    BDBG_MSG(("userNavigate %d: %d %d..%d", increment, _userPos, settings.first_time, settings.last_time));
    if ((int)_userPos + increment < (int)settings.first_time) {
        _userPos = settings.first_time;
    }
    else if ((int)_userPos + increment >= (int)settings.last_time) {
        _userPos = settings.last_time;
    }
    else {
        _userPos += increment;
    }

    _userActive = true;
    b_time_get(&_lastUserNavigation);

    repaint();
}

void MThumbnailPanel::setPlayerPos(unsigned playerPos)
{
    b_time_t now;

    _playerPos = playerPos;

    /* if it's been more than 10 seconds since the user pressed the button, then the player's new pos drives the thumbnails */
    b_time_get(&now);
    if (b_time_diff(&now, &_lastUserNavigation) > 10*1000) {
        _userActive = false;
    }
}

void MThumbnailPanel::setVideo(Video *video)
{
    if (_video != video) {
        _video = video;
        _playerPos = _userPos = 0;
        _userActive = false;
    }
}

void MThumbnailPanel::setVisible(Video *video, bool view_window, bool visible)
{
    if (!video) {
        bthumbnail_manager_clear_visible(_control->thumbnail_manager());
    }
    else {
        bthumbnail_stream_settings settings;
        bthumbnail_stream_t thumbnail_stream;

        thumbnail_stream = video->thumbnail_data.stream;
        if (!thumbnail_stream) return;

        bthumbnail_stream_get_settings(thumbnail_stream, &settings);
        if (view_window) {
            settings.view_window.visible = visible;
        }
        else {
            settings.full_view.visible = visible;
            settings.full_view.number = TOTAL_DISPLAYED;
            settings.full_view.min_spacing = 3; /* 3 seconds is enough to get distinct pictures. */
        }
        bthumbnail_stream_set_settings(thumbnail_stream, &settings);
    }
}
unsigned MThumbnailPanel::currentThumbnailPos()
{
	unsigned indexed_time;
	printf("index:%d index time:%d cur:%d\n",
		current_index,
		viewing_real_time_pos[current_index],
		_currentThumbnailPos);
	if(current_index == 0xffff )
	{
		return _currentThumbnailPos;
	}
	if (viewing_real_time_pos[current_index] == 0xffff)
	{
		return _currentThumbnailPos;
	}
	indexed_time = viewing_real_time_pos[current_index];
	if(_currentThumbnailPos >= indexed_time)
	{
		if (_currentThumbnailPos - indexed_time < _spacing / 2)
		{
			return _currentThumbnailPos;
		}
	}
	else
	{
		if (indexed_time - _currentThumbnailPos < _spacing / 2)
		{
			return _currentThumbnailPos;
		}
	}

	return indexed_time;
}
