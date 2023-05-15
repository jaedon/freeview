/***************************************************************************
 *     Copyright (c) 2002-2008, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: videoscreen.h $
 * $brcm_Revision: 9 $
 * $brcm_Date: 10/8/08 11:53a $
 *
 * Module Description: Collection of screens for pvrgui demo app
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/videoscreen.h $
 * 
 * 9   10/8/08 11:53a tokushig
 * PR43426: merge to main
 * 
 * BRUTUS_97405_97043/3   10/8/08 10:50a tokushig
 * PR43426: move global _buttonActionList to a class attribute for
 * homescreen and videoscreen.  this will allow the code to work properly
 * when 2 instances of brutus exist as well as avoid a global constructor
 * issue in vxworks.
 * 
 * 8   9/29/08 6:00p jtna
 * PR47417: Set focus on Play button if stream delete succeeds
 * 
 * 7   9/15/08 12:40p tokushig
 * PR43426: merge to main
 * 
 * BRUTUS_97405_97043/2   8/28/08 10:16p tokushig
 * PR43426: remove varibles that are no longer used
 * 
 * BRUTUS_97405_97043/1   8/21/08 5:21p tokushig
 * PR43426: added scrolling button listboxes to both homescreen and
 * videoscreen.  separated buttonlistbox class to common file for reuse
 * (buttonlistbox.cpp/h)
 * 
 * 5   7/30/08 1:22p shyam
 * PR 45076 : Add trasncode support for playback streams
 * 
 * 4   5/31/06 4:19p tokushig
 * PR21891: Merging updated picture viewer into mainline.
 * 
 * SanDiego_Brutus_Skin/2   5/16/06 11:33a tokushig
 * merge from main
 * 
 * 3   4/17/06 5:06p jgarrett
 * PR 20951: Merging skinned UI into mainline
 * 
 * SanDiego_Brutus_Skin/1   1/26/06 9:52a tokushig
 * add skinning capability
 * 
 * 2   8/22/05 9:25a erickson
 * PR16747: added Rename button
 * 
 * 1   2/7/05 8:18p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/1   4/9/03 11:22a erickson
 * rework of pvrgui files and class names
 *
 **************************************************************************/
#ifndef VIDEOSCREEN_H__
#define VIDEOSCREEN_H__

#include <mlistbox.h>
#include "genericscreen.h"
#include "mlabel.h"
#include "mlist.h"
#include "mbutton.h"
#include "video.h"
#include "control.h"
#include "buttonlistbox.h"

class VideoScreen : public GenericScreen, public MListBoxEventHandler {
public:
    VideoScreen(Control *control, MRect rectTVWin = SMALLTV);
    void setVideo(Video *v);
    void playVideo(bool resume = false);

#if B_HAS_TRANSCODE
    void transcodePlaybackVideo(void) ;
#endif

    void renameVideo(Video *v);
    bool deleteVideo(Video *v);
    Video *video() const {return _video;}
    void onClick(MButton *self);
    void onItemClicked(MListBox *listbox);
    bool focus();
protected:
    void createButton(ButtonListBox * pListBox, int index, const char * name,
                      MImage * icon, eSkinAction action);

    MList<eSkinAction> _buttonActionList;
    Video *_video;
    MLabel *l1, *l2, *l3;
    MPushButton *_back;
    MColumnListBox * _listbox;
};

#endif /* VIDEOSCREEN_H__ */
