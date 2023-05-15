/***************************************************************************
 *     Copyright (c) 2001-2006, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: mpixmap.cpp $
 * $brcm_Revision: 5 $
 * $brcm_Date: 5/31/06 4:17p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/mlibs/mpixmap.cpp $
 * 
 * 5   5/31/06 4:17p tokushig
 * PR21891: Merging updated picture viewer into mainline.
 * 
 * SanDiego_Brutus_Skin/5   5/16/06 10:17a tokushig
 * Merge from main
 * 
 * SanDiego_Brutus_Skin/4   5/4/06 2:30p tokushig
 * added handling for thumbnail width/height.  also now using hardware
 * accelerated bwin framebuffer api for drawing images.
 * 
 * SanDiego_Brutus_Skin/3   4/27/06 10:47a tokushig
 * add constructor to create MImage from image data in memory
 * 
 * 4   4/17/06 5:08p jgarrett
 * PR 20951: Merging skinned UI into mainline
 * 
 * SanDiego_Brutus_Skin/2   4/4/06 4:59p tokushig
 * do not copy overridden drawops for MPixmap.  drawing an MImage into an
 * MPixmap will not work using and overrided drawops (which probably use
 * hardware acceleration).
 * 
 * SanDiego_Brutus_Skin/1   4/4/06 11:17a tokushig
 * allow mimage objects to inheirit the drawops of their parents.  also
 * mimage objects to load images as file pointers instead of the entire
 * compressed image in memory.
 * 
 * 3   8/24/05 6:20p arbisman
 * PR16678: Add support for WinCE
 * 
 * 2   4/18/05 10:50a erickson
 * PR14593: fixed MPixmap constructor to render image into pixmap mem
 * 
 * 1   2/7/05 11:16p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/6   1/4/05 12:46p erickson
 * PR13639: added bwin_image_close and bwin_image_load_memory support
 *
 **************************************************************/

#include "mpixmap.h"
#include "mapplication.h"
#include "mapp_priv.h"
#include "bwin.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
BDBG_MODULE(mpixmap);


MImage::MImage(MApplication *app, const char *filename, bool bLoadToMem) {
    _app = app;
    _id = 0;
    load(filename, bLoadToMem);
}

MImage::MImage(MApplication *app, const char *data, int length) {
    _app = app;
    _id  = 0;
    loadFromData(data, length);
}

MImage::MImage(const MImage &copy) {
    _app = copy._app;
    _id = copy.id();
    _width = copy.width();
    _height = copy.height();
    _width_thumb  = copy.widthThumb();
    _height_thumb = copy.heightThumb();
}

void MImage::clear() {
    if (_id) {
        bwin_image_close((bwin_image_t)_id);
        _id = 0;
    }
    _width        = 0;
    _height       = 0;
    _width_thumb  = 0;
    _height_thumb = 0;
}

MImage::~MImage()
{
    clear();
}

bool MImage::load(const char *filename, bool bLoadToMem) {
    clear();

    if (bLoadToMem)
    {
        _id = (int)bwin_image_load(_app->_private->win_engine, (char*)filename);
        if (!_id)
            return false;
    }
    else
    {
        _id = (int)bwin_image_file_io(_app->_private->win_engine, (char*)filename);
        if (!_id)
            return false;
    }

    bwin_image_settings s;
    bwin_image_get_settings((bwin_image_t)_id, &s);
    _width = s.width;
    _height = s.height;
    _width_thumb  = s.thumb_width;
    _height_thumb = s.thumb_height;

    return true;
}

bool MImage::loadFromData(const char *data, int len) {
    clear();
    _id = (int)bwin_image_load_memory(_app->_private->win_engine, data, len);
    if (!_id)
        return false;

    bwin_image_settings s;
    bwin_image_get_settings((bwin_image_t)_id, &s);
    _width = s.width;
    _height = s.height;
    _width_thumb  = s.thumb_width;
    _height_thumb = s.thumb_height;

    return true;
}

//////////////////////////////////////////////

MPixmap::MPixmap(MFrameBuffer *fb, int w, int h) : MPaintDevice(fb) {
    init(w, h);
}

MPixmap::MPixmap(MFrameBuffer *fb, const MImage &image) : MPaintDevice(fb) {
    init(image.width(), image.height());
    bwin_rect rect = {0,0,_settings.width,_settings.height};

    bwin_image_render((bwin_t)_id, (bwin_image_t)image.id(),
        bwin_image_render_mode_single, NULL, NULL, NULL);
}

MPixmap::MPixmap(const MPixmap &copy) : MPaintDevice(copy._fb) {
    init(copy.width(), copy.height());
    bwin_rect rect = {0,0,_settings.width,_settings.height};

    bwin_copy_rect((bwin_t)_id, &rect, (bwin_t)copy.id(), &rect, NULL);
}

void MPixmap::init(int w, int h) {
    /* learn the desired pixel format from MFrameBuffer */
    MFrameBuffer::Settings fb_settings;
    bwin_framebuffer_settings copy_settings;

    _fb->getSettings(&fb_settings);
    bwin_get_framebuffer_settings(fb_settings.framebuffer, &copy_settings);

    /* now we can create a pixmap with the same pixel_format */
    bwin_framebuffer_settings_init(&_settings);
    _settings.pixel_format = copy_settings.pixel_format;
    _settings.width = w;
    _settings.height = h;
    /* copy these settings for hardware acceleration */
    _settings.drawops = copy_settings.drawops;
    _settings.data    = copy_settings.data;
    
    _fbid = NULL;
    _id = 0;

    alloc();
}

/* A pixmap requires a bwin_framebuffer_t for storage and a single bwin_t for
drawing operations. */
void MPixmap::alloc() {
    clear();

    if (!_settings.width || !_settings.height)
        return;

    _settings.pitch = bwin_get_bpp(_settings.pixel_format)/8 * _settings.width;

    /* we're not handling non-byte oriented pixels, so assert it */
    assert(bwin_get_bpp(_settings.pixel_format) % 8 == 0);
    //make bwin_open_framebuffer() allocate memory for us - it will attempt
    //to use graphics memory for hardware acceleration if possible.
    _settings.buffer = NULL;

    BDBG_MSG(("Creating pixmap framebuffer %d,%d,%d,%d %p", 
        _settings.pixel_format, _settings.width,
        _settings.height,_settings.pitch,
        _settings.buffer));
    _fbid = bwin_open_framebuffer(_fb->app()->_private->win_engine, &_settings);

    bwin_get_framebuffer_settings(_fbid, &_settings);
    _id = (int)_settings.window;
}

void MPixmap::clear() {
    if (_fbid) {
        bwin_close_framebuffer(_fbid);
        _fbid = NULL;
    }
}

MStyle *MPixmap::style() const {return _fb->app()->style();}
