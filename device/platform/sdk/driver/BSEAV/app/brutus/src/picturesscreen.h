/***************************************************************************
 *     Copyright (c) 2002-2009, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: picturesscreen.h $
 * $brcm_Revision: 10 $
 * $brcm_Date: 8/14/09 3:38p $
 *
 * Module Description: App logic for pvrgui app
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/picturesscreen.h $
 * 
 * 10   8/14/09 3:38p erickson
 * PR55994: refactor thumbnail
 *
 * 9   8/13/08 10:12a erickson
 * PR42789: add BLUETOOTH_SUPPORT demo code
 *
 * 8   2/21/08 2:11p ahulse
 * PR25542: Add ability to display a picture from name of file, plus list
 * picture files on disk
 *
 * 7   3/13/07 9:40a erickson
 * PR28342: removed unused MQueue data structure. see PR for design notes.
 *
 ****************************************************************************/
#ifndef PICTURESSCREEN_H
#define PICTURESSCREEN_H

#include "genericscreen.h"
#include <mtimer.h>
#include <mpainter.h>
#include <mlistview.h>
#include <mlabel.h>
#include <mcache.h>
#include "bsettop.h"
#include "bsettop_types.h"

struct Picture;
class PictureSender;
class PictureWebServerPlugin;

class FullScreenView : public MLabel
{
public:
    FullScreenView(MWidget *parent, const MRect &rect, const char *text = NULL,
                   const char *name = NULL) :
    MLabel(parent, rect, text, name)
    {
            _transition = MPainter::eSwap;
        _cacheFullSize.clear();
        _cacheFullSize.setMaxElements(3);
        _newImageName.clear();
        _oldImageName.clear();
    };

    ~FullScreenView()
    {
        clear();
    }

    void setImage(const Picture *pic, MPainter::DrawImageMode
                  drawImageMode = MPainter::eSingle,
                  MPainter::TransitionMode transitionMode = MPainter::eSwap);

    void setTransition(MPainter::TransitionMode transition)
    {
        _transition = transition;
    };
    void clear();
    void hide();

protected:
    virtual void draw(const MRect &cliprect);

    MPainter::TransitionMode   _transition;

    MCache<MPixmap> _cacheFullSize;

    MString _newImageName;
    MString _oldImageName;
};

class PicturesScreen : public GenericScreen, MListViewEventHandler, MTimerEventHandler
{
public:
    PicturesScreen(Control *control);
    ~PicturesScreen();

    bool fullScreenMode() const;
    void setFullScreenMode(bool on);
    int totalPictures();
    const char *getMediaFileName(int idx);
    int showPicture(const char *filename);
    int showPicture(int idx);
    void advance(int inc);

    int renamePicture(Picture *pic);
    int deletePictureDeferred(Picture *pic, bool prompt = true);
    int deletePicture(Picture *pic);
    int sendPicture(Picture *pic);
    Picture *currentPicture();

    void setDrawImageMode(MPainter::DrawImageMode mode);

    // process remote control/front panel cmds
    bool processCommand(int cmd);

    int readCamera();

    // overrides
    void show();
    void hide();
    void rememberFocus()
    {
    } // I'm deleting widgets, so don't remember

    // events
    void onClick(MListView *sender);
    void onTimeout(MTimer *sender);

    void addPicture(Picture *pic, int index = -1);
    void addPicture(const char *name, const char *filename, int index = -1);

    /**
    Summary:
    Find an unused picture name.
    Description:
    Returns both the full path name and the basename.
    **/
    void newPictureName(MString &name, MString &file);

    const char *picturePath() const
    {
        return _path;
    }

protected:
    MLabel *_bevel;
    MListView *_view;
    MString _path;
    MAutoList<Picture> _list;
    Picture *_deletePic;
    FullScreenView *_fullScreenView;
    MLabel *_fullScreenStatus;
    MLabel *_title;
    int _index;
    int _indexLast;
    int _maxPictures;
    MPainter::DrawImageMode _mode;
    MTimer _slideShowTimer, _statusTimer, _deltimer, _idleTimer;
    PictureSender *_picSender;
    PictureWebServerPlugin *_plugin;
    MPainter::TransitionMode _lastTransition;

    void cleanup(void);
    int readDisk(const char *path);
    void showStatus(const char *text, int timeout = -1);
    void updateFullScreenImage();

    // overrides
    bool keyDown(int ascii, int modifiers, int scancode);
};

#endif /* PICTURESSCREEN_H */
