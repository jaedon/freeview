/***************************************************************************
 *     Copyright (c) 2003, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: infobar.h $
 * $brcm_Revision: 1 $
 * $brcm_Date: 2/7/05 8:10p $
 *
 * Module Description: GUI for pvr state
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/info/infobar.h $
 * 
 * 1   2/7/05 8:10p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/11   11/19/04 3:29p erickson
 * PR13256: added bitrate and allowed panel to remain visible for AVC demo
 * 
 * Irvine_BSEAVSW_Devel/10   10/15/04 10:51a erickson
 * PR13014: infobar rework
 * 
 * Irvine_BSEAVSW_Devel/9   9/16/04 3:38p erickson
 * PR11081: more work on info panel display
 * 
 * Irvine_BSEAVSW_Devel/8   9/13/04 1:54p erickson
 * PR11081: refactored info panel controls to be more minimal, support
 * more options
 * 
 * Irvine_BSEAVSW_Devel/7   5/18/04 2:59p bandrews
 * PR10148: Removed global image and replaced with global string
 *
 * Irvine_BSEAVSW_Devel/6   4/19/04 2:02p erickson
 * PR10471: made all filepaths configurable using brutus.cfg
 *
 * Irvine_BSEAVSW_Devel/5   1/29/04 2:27p erickson
 * PR9107: merging back from BRUTUS_PR9107 branch for bwin development
 *
 * Irvine_BSEAVSW_Devel/4   12/16/03 12:25a erickson
 * PR8731: POD ui work
 *
 * Irvine_BSEAVSW_Devel/3   12/10/03 1:36p erickson
 * PR8813: info UI rework
 *
 * Irvine_BSEAVSW_Devel/2   12/8/03 3:12p erickson
 * PR8813: still working on getting new info UI smooth
 *
 * Irvine_BSEAVSW_Devel/1   12/4/03 9:39a erickson
 * PR8813: complete rework of info UI to support vertical bar
 *
 ****************************************************************************/
#ifndef INFOBAR_H__
#define INFOBAR_H__

#include "mwidget.h"
#include "mlabel.h"
#include "mbutton.h"
#include "mtimer.h"
#include "miconroundedlabel.h"

class Control;
class Config;
class InfoBarPlugin;

/**
Vertical stack of panels. You can scroll through the panels which activates and
deactivates them.

The InfoBar also contains Brutus-specific information about the current channel
or PVR state.
**/
class InfoBar : public MLabel, public MTimerEventHandler {
	friend class InfoBarPlugin;
public:
	InfoBar(Control *control, MWidget *parent, const MRect &rect);

	Control *control() const {return _control;}

	void setCurrent(int index);

	// overrides
	bool focus();
	void hide();
	void show();
	void onTimeout(MTimer *self);

	enum Mode {
		eFull, eCompact
	};
	void setMode(Mode mode);
	Mode mode() const {return _mode;}
	void showMessage(bool withinfo);

protected:
	Control *_control;
	MTimer _timer;
	MLabel *_label;
	Mode _mode;
	int _autohide;

	// content
	MList<InfoBarPlugin> _plugins;
	InfoBarPlugin *_lastactive;
	void addPlugin(InfoBarPlugin *plugin);
	void activate(InfoBarPlugin *plugin);
	void layout();
	bool keyDown(int ascii, int modifiers, int scancode);
};

/**
Generic plugin for the InfoBar
The plugin facilities automatically showing/hiding the linked widget,
whatever it may be.
**/
class InfoBarPlugin : public MIconRoundedLabel {
public:
	InfoBarPlugin(InfoBar *parent, const char *name = NULL);

	InfoBar *infoBar() const {return _infobar;}
	virtual void setActive(bool active);

	// overrides
	bool focus();
protected:
	InfoBar *_infobar;
	MWidget *_linkedWidget;
	bool _active;
};

#endif //INFOBAR_H__
