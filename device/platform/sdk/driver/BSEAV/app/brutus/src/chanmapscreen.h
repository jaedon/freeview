/***************************************************************************
 *     Copyright (c) 2002-2007, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: chanmapscreen.h $
 * $brcm_Revision: 1 $
 * $brcm_Date: 4/24/07 12:46p $
 *
 * Module Description: Collection of screens for pvrgui demo app
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/chanmapscreen.h $
 * 
 * 1   4/24/07 12:46p jrubio
 * PR30010: adding GUI file for STREAMER server list
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
#ifndef CHANMAPSCREEN_H__
#define CHANMAPSCREEN_H__

#include "genericscreen.h"
#include <mlistbox.h>
#include "control.h"

class ChanMapScreen : public GenericScreen,
    public MListBoxEventHandler {
public:
    ChanMapScreen(Control *control, MRect rectTVWin = SMALLTV);
    void onClick(MButton *self);
    void onItemClicked(MListBox *self);
    bool focus();

    int refresh();

protected:
    MColumnListBox *_list;
    bool _modified;

};

#endif /* CHANMAPSCREEN_H__ */
