/***************************************************************************
 *     Copyright (c) 2001-2006, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: mpixmap.h $
 * $brcm_Revision: 4 $
 * $brcm_Date: 5/31/06 4:17p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/mlibs/mpixmap.h $
 * 
 * 4   5/31/06 4:17p tokushig
 * PR21891: Merging updated picture viewer into mainline.
 * 
 * SanDiego_Brutus_Skin/4   5/16/06 10:17a tokushig
 * Merge from main
 * 
 * SanDiego_Brutus_Skin/3   5/4/06 2:30p tokushig
 * added handling for thumbnail width/height.  also now using hardware
 * accelerated bwin framebuffer api for drawing images.
 * 
 * SanDiego_Brutus_Skin/2   4/27/06 10:46a tokushig
 * add constructor to create MImage from image data in memory
 * 
 * 3   4/17/06 5:05p jgarrett
 * PR 20951: Merging skinned UI into mainline
 * 
 * SanDiego_Brutus_Skin/1   4/4/06 11:08a tokushig
 * added ability to not load compressed image into memory but rather open
 * file handle only.  this case assumes that the associated image
 * decompressor can read the image file directly.
 * 
 * 2   4/18/05 10:50a erickson
 * PR14593: added comments
 * 
 * 1   2/7/05 11:16p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/7   2/4/05 10:49a erickson
 * PR12728: clcleaned up -pedantic warnings
 * 
 * Irvine_BSEAVSW_Devel/6   1/24/05 2:27p erickson
 * PR13097: updated docs
 * 
 * Irvine_BSEAVSW_Devel/5   1/4/05 12:46p erickson
 * PR13639: added bwin_image_close and bwin_image_load_memory support
 *
 **************************************************************/

#ifndef MPIXMAP_H
#define MPIXMAP_H

#include "mpaintdevice.h"
class MApplication;
class MFrameBuffer;

#ifdef BWIN_SUPPORT
/* bwin must be exposed outside of mlibs. This is because
we want to create the framebuffers outside of mlibs.
*/
#include "bwin.h"
#endif

/**
* An image is a graphic with a rendering (i.e. decoding) engine.
* You learn its width and height after opening it.
* It can render into whatever pixel format that the rendering engine supports.
* You can scale the image when you render.
*
* There are 2 choices to handle image loading:
*
* 1. The compressed image is loaded into memory until MImage is destroyed.
* The uncompressed image resides in memory only during the decoding process.
* If you want to decode the image only one, you'll need to draw into an MPixmap.
*
* 2. The image file is opened, but not read until needed by the decompressor.
* This saves considerable memory, but requires the decompressor to be able
* to access the image file directly.
*/
class MImage {
public:
    MImage() {_id = 0;}
    MImage(MApplication *app, const char *filename, bool bLoadToMem = true);
    MImage(MApplication *app, const char *data, int length);
    MImage(const MImage &copy);
    
    ~MImage();

    bool load(const char *filename, bool bLoadToMem = true);
    bool loadFromData(const char *data, int len);

    int id() const {return _id;}
    int width() const {return _width;}
    int height() const {return _height;}
    void setWidth(int width) { _width = width; };
    void setHeight(int height) { _height = height; };

    int widthThumb() const {return _width_thumb;}
    int heightThumb() const {return _height_thumb;}
    void setWidthThumb(int width) { _width_thumb = width; };
    void setHeightThumb(int height) { _height_thumb = height; };

    bool isNull() const {return _id == 0;}
    void clear();

protected:
    int _id;
    int _width, _height;
    int _width_thumb, _height_thumb;
    MApplication *_app;
};

/**
* A pixmap is an off-screen framebuffer.
* You can use it for caching graphics.
* You tell it it's width and height. You pass in a framebuffer with the same
* pixel format as the pixmap.
*
* You can draw into an MPixmap just like you draw with a widget.
*/
class MPixmap : public MPaintDevice {
public:
    /* The pixmap refers to the framebuffer only to get the same pixel_format. */
    MPixmap(MFrameBuffer *fb, int w = 0, int h = 0);

    /* Copy the image into the pixmap. If the image has an alpha channel,
    this method should not be used because the image will blend with 
    uninitialized memory. */
    MPixmap(MFrameBuffer *fb, const MImage &image);

    /* Copy the pixmap */
    MPixmap(const MPixmap &copy);

    virtual ~MPixmap() {clear();}

    int id() const {return _id;}
    unsigned width() const {return _settings.width;}
    unsigned height() const {return _settings.height;}
    MStyle *style() const;
    void clear();

protected:
#ifdef BWIN_SUPPORT
    bwin_framebuffer_settings _settings;
    bwin_framebuffer_t _fbid;
#endif
    int _id; /* bwin_t */
    void alloc();
    void init(int w, int h);
};

#endif //MPIXMAP_H
