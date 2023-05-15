/***************************************************************************
 *     Copyright (c) 2002-2003, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: info_status.h $
 * $brcm_Revision: 1 $
 * $brcm_Date: 2/7/05 8:10p $
 *
 * Module Description: GUI for pvr state
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/info/info_status.h $
 * 
 * 1   2/7/05 8:10p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/8   10/15/04 10:51a erickson
 * PR13014: infobar rework
 * 
 * Irvine_BSEAVSW_Devel/7   6/30/04 11:34a erickson
 * PR11135: need to clear FEC cor/uncorr when changing channel, toggling
 * windows
 * 
 * Irvine_BSEAVSW_Devel/6   2/12/04 7:34p erickson
 * PR9465: reworked ChannelManager to support multiple tuners per
 * downstream
 * 
 * Irvine_BSEAVSW_Devel/5   1/29/04 2:26p erickson
 * PR9107: merging back from BRUTUS_PR9107 branch for bwin development
 * 
 * Irvine_BSEAVSW_Devel/4   12/4/03 9:39a erickson
 * PR8813: complete rework of info UI to support vertical bar
 *
 * Irvine_BSEAVSW_Devel/3   11/7/03 1:32a erickson
 * PR8582: changed qam ds status structure in order to get FEC corr/uncorr
 * values
 * 
 * Irvine_BSEAVSW_Devel/2   10/10/03 1:43p erickson
 * converted brutus frontend code so that it can support all frontend
 * types simultaneously. this
 * is in preperation for 97038 and others.
 * 
 * Irvine_BSEAVSW_Devel/1   4/10/03 4:22p erickson
 * info panel rework
 * added edit channels and outputs save-state feature
 *
 ****************************************************************************/
#ifndef INFO_STATUS_H__
#define INFO_STATUS_H__

#include "infopanel.h"
#include "bsettop.h"
#include "channelmap.h" // btuner_stream_type
#include "control.h"
#include <mbutton.h>

class Constellation;
class DecodePage;
class PlaybackBufferPanel;
class FrontEndPage;
class FrontEndSettingsPage;

/**
The StatusInfoPanel is a page-based, readonly set of pages.
You cannot navigate onto the panel.
The actual set of pages may vary depending on the platform and current mode.

TODO:
- no constellation for analog
- add labels to the pages
**/
class StatusInfoPanel : public InfoPanel, public ControlEventHandler {
public:
	StatusInfoPanel(InfoBar *infoBar, const MRect &rect);
	~StatusInfoPanel();

	// overrides
	void update();
	void onControlEvent(Control *control, Control::Event event);
protected:
	Constellation *_const;
	FrontEndPage *_status;
	FrontEndSettingsPage *_feSettings;
	DecodePage *_decode;
	PlaybackBufferPanel *_bars;
};


class StatusInfoBarPlugin : public InfoBarPlugin,
	public ControlEventHandler {
public:
	StatusInfoBarPlugin(InfoBar *parent);
	~StatusInfoBarPlugin();

	// overrides
	void onControlEvent(Control *control, Control::Event event);
protected:
	InfoPanel *_panel;
	bool keyDown(int ascii, int modifiers, int scancode);
};

#endif // INFO_STATUS_H__

