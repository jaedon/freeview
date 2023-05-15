/***************************************************************************
 *     Copyright (c) 2002-2009, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: playbackscreen.h $
 * $brcm_Revision: 7 $
 * $brcm_Date: 6/1/09 7:56p $
 *
 * Module Description: Collection of screens for pvrgui demo app
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/playbackscreen.h $
 * 
 * 7   6/1/09 7:56p jtna
 * PR43001: added record-related brutus script commands
 * 
 * 6   4/2/08 3:56p mward
 * PR40307: Don't include any thumbnail support with PVR_SUPPORT=n.
 * 
 * 5   3/20/08 3:18a erickson
 * PR40307: remove unused function
 * 
 * 4   3/20/08 1:49a erickson
 * PR40307: update
 * 
 * 3   3/19/08 9:35p erickson
 * PR40307: thumbnail update
 * 
 * 2   7/26/07 8:41p erickson
 * PR33552: added title of Video to top of screen so we can read long
 * names
 * 
 * 1   3/14/07 10:15a erickson
 * PR26267: refactor Brutus into separate VideoList and PlaybackScreen
 * 
 * 5   10/27/06 11:47a erickson
 * PR24374: add play(++) support, echo script commands to console
 * 
 * 4   5/31/06 4:18p tokushig
 * PR21891: Merging updated picture viewer into mainline.
 * 
 * SanDiego_Brutus_Skin/2   5/16/06 11:33a tokushig
 * merge from main
 * 
 * 3   4/17/06 5:06p jgarrett
 * PR 20951: Merging skinned UI into mainline
 * 
 * SanDiego_Brutus_Skin/1   1/11/06 11:18a tokushig
 * added skinning capability from xml skin file
 * 
 * 2   8/22/05 9:23a erickson
 * PR16747: removed hidden rename key
 * 
 * 1   2/7/05 8:14p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/6   11/19/04 3:27p erickson
 * PR13256: converted Playback UI to more useful data like bitrate and
 * length
 *
 * Irvine_BSEAVSW_Devel/5   3/8/04 2:55p erickson
 * PR9883: removed global
 *
 * Irvine_BSEAVSW_Devel/4   2/26/04 11:07a erickson
 * PR9900: made some methods const using MListItr
 *
 * Irvine_BSEAVSW_Devel/3   9/19/03 1:29p erickson
 * fixed Delete Recordings (PR8060)
 *
 * Irvine_BSEAVSW_Devel/2   7/24/03 6:03p erickson
 * added new interactive mode
 *
 * SanJose_Linux_Devel/3   4/9/03 11:21a erickson
 * rework of pvrgui files and class names
 ***************************************************************************/
#ifndef PLAYBACK_SCREEN_H__
#define PLAYBACK_SCREEN_H__

#include <mlistbox.h>
#include <mlabel.h>
#include <mtimer.h>
#include "genericscreen.h"
#include "videolist.h"
#include "control.h"
#ifdef PVR_SUPPORT
#include "thumbnail_panel.h"
#endif
/**
UI for selecting files that Brutus can play.
Control's VideoList is the source.
**/
class PlaybackScreen : public GenericScreen, public MListBoxEventHandler,
    public MTimerEventHandler, public VideoListEventHandler {
public:
    PlaybackScreen(Control *control, VideoList *videos, MRect rectTVWin = SMALLTV);

    /**
    * Update the GUI from the current video list and pvr state.
    **/
    void refreshList(bool onlyRecording = false);
    void refreshFromDisk();
    bool selectVideo(bool play = false);
    bool deleteVideo(int index);
    bool deleteVideo(Video *vid, bool prompt = true);
    Video *currentVideo() const;

    // events
    void onClick(MButton *button);
    void onItemClicked(MListBox *listbox);
    void onCurrentItemChanged(MListBox *listbox);
    void onInsertItem(MListBox *listbox);
    void onDeleteItem(MListBox *listbox);
    void onTimeout(MTimer *timer);
    void onUpdate(VideoList *videoList);
    
    // overrides
    bool focus();
    void show();

protected:
    void update(Video *vid, int index);
    void setDiskFree();

    MLabel *l1, *_diskFree, *_description;
    MTimer _timer;
    VideoList *_videos;
    MThumbnailPanel *_thumbnailPanel;

    /**
    * listbox and VideoList MUST be in the same order.
    **/
    MColumnListBox *listbox;
    static void updateVideoCallback(void *context, Video *vid, int index);
};

#endif /* PLAYBACK_SCREEN_H__ */
