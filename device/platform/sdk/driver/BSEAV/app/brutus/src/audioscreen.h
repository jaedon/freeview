/***************************************************************************
 *     Copyright (c) 2002-2009, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: audioscreen.h $
 * $brcm_Revision: 5 $
 * $brcm_Date: 6/26/09 11:37a $
 *
 * Module Description: Collection of screens for pvrgui demo app
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/audioscreen.h $
 * 
 * 5   6/26/09 11:37a erickson
 * PR56373: remove out of date comment
 *
 * 4   8/15/08 10:08a erickson
 * PR42789: bluetooth update
 *
 * 3   5/7/08 11:51a erickson
 * PR42329: fix minor BKNI memory leaks
 *
 * 2   2/21/08 2:09p ahulse
 * PR25541: Add ability to play indexed audio file, plus list out audio
 * files
 *
 * 1   2/7/05 7:52p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 *
 * Irvine_BSEAVSW_Devel/2   7/23/03 4:00p erickson
 * renamed from pvrgui to brutus
 *
 * Irvine_BSEAVSW_Devel/3   5/9/03 5:33p erickson
 * cheops is now browseable in pvrgui with broadcast notification
 *
 * Irvine_BSEAVSW_Devel/2   4/30/03 9:11a erickson
 * audio rework
 *
 * Irvine_BSEAVSW_Devel/1   4/9/03 11:20a erickson
 * rework of pvrgui files and class names
 ***************************************************************************/
#ifndef AUDIOSCREEN_H__
#define AUDIOSCREEN_H__

#include "genericscreen.h"
#include "mbutton.h"
#include "mlabel.h"
#include "mlistview.h"
#include "mtimer.h"

class AudioControl;
class MFileView;
class AudioBufferBar;

class AudioScreen : public GenericScreen,
    public MListViewEventHandler, public MTimerEventHandler {
public:
    AudioScreen(Control *control);
    ~AudioScreen();

    int readDisk();

    // play file at index "idx"
    void playAudio(int idx);
    // play current file or current directory
    void playAudio();
    // list everything in current directory
    void listAll();
    // play everything in current directory
    void playAll();
    // stop everything
    void stopAudio();

    // overrides
    bool focus();
    void show();

    // events
    void onClick(MButton *self);
    void onClick(MListView *sender);
    void onLayout(MListView *sender);
    void onTimeout(MTimer *sender);
protected:
    MLabel *_title, *_currentDir;
    MPushButton *_stop, *_playall, *_play;
    MLabel *bevel;
    MFileView *fileview;
    MString _selected;
    MTimer _timer;
    MImage *_leftbtn, *_middlebtn, *_rightbtn, *_stopbtn, *_playbtn, *_playallbtn;

    // TODO: Control isn't needed by AudioScreen. Rework GenericScreen
    // to not require it. This will require a TvBevelScreen.
    Config *_cfg;
    AudioControl *_audioControl;

    AudioBufferBar *_lbar, *_rbar;

    void setCurrentDir();

    void startMeter();
    void stopMeter();
};

#endif

