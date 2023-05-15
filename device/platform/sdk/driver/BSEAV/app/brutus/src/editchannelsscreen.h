/***************************************************************************
 *     Copyright (c) 2002-2007, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: editchannelsscreen.h $
 * $brcm_Revision: 4 $
 * $brcm_Date: 1/29/07 11:17a $
 *
 * Module Description: Collection of screens for pvrgui demo app
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/editchannelsscreen.h $
 * 
 * 4   1/29/07 11:17a ssood
 * PR24918: Channel Map Redesign to make its access Thread-safe
 * 
 * 3   6/9/05 3:53p erickson
 * PR15524: removed Load and added confirm dialog to Save
 * 
 * 2   4/19/05 3:46p erickson
 * PR14701: make digital subchannel support standard for all platforms
 * 
 * 1   2/7/05 8:07p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/4   2/13/04 12:05p erickson
 * PR9465: updated channelmgr naming and comments
 * 
 * Irvine_BSEAVSW_Devel/3   1/7/04 11:18a erickson
 * PR8363: The user is now warned any time the EditChannels screen is
 * exited if the channel map hasn't been saved. Before it only worked
 * with the "Back" button.
 * 
 * Irvine_BSEAVSW_Devel/2   7/18/03 11:49a erickson
 * channelmgr rework to support new scan methods, specifying programs or
 * pids in the channel map
 * 
 * SanJose_Linux_Devel/6   4/10/03 4:22p erickson
 * info panel rework
 * added edit channels and outputs save-state feature
 *
 * SanJose_Linux_Devel/5   4/9/03 11:20a erickson
 * rework of pvrgui files and class names
 *
 **************************************************************************/
#ifndef EDITCHANNELSSCREEN_H
#define EDITCHANNELSSCREEN_H

#include "genericscreen.h"
#include <mlistview.h>
#include <mlistbox.h>
#include <mbutton.h>
#include "channelmgr.h"

class EditChannel;

class EditChannelsScreen : public GenericScreen,
	public MListBoxEventHandler {
friend class EditChannel;
public:
	EditChannelsScreen(Control *control);
	~EditChannelsScreen();

	// events
	void onClick(MButton *self);
	void onItemClicked(MListBox *self);
	void rememberFocus() {}

	// called from Control.cpp for CHAN UP/DOWN
	void rearrange(int inc);
	
	// called from Control before hiding the screen
	void checkModified();

	bool focus();

protected:
	MColumnListBox *_list;
	MPushButton _back, _save, _reset, _add;
	EditChannel *_ec;
	ChannelManager *_mgr;
	bool _modified;

	int refresh();
	void deleteChannel(int index);
	void save();
#if 0
	void load();
	void writeChannelMapSelectFile();
#endif

	bool editChannel(int index, ChannelMap::ChannelMapEntry *entry);
};

#endif /* EDITCHANNELSSCREEN_H */
