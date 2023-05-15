/***************************************************************************
 *     Copyright (c) 2002-2004, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: infopanel.h $
 * $brcm_Revision: 1 $
 * $brcm_Date: 2/7/05 8:10p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/info/infopanel.h $
 * 
 * 1   2/7/05 8:10p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/6   10/15/04 10:51a erickson
 * PR13014: infobar rework
 * 
 * Irvine_BSEAVSW_Devel/5   5/18/04 2:59p bandrews
 * PR10148: Removed global image and replaced with global string
 * 
 * Irvine_BSEAVSW_Devel/4   2/17/04 2:28p erickson
 * PR9465: sds and vsb on 7038 need more delay, but qam doesn't. so it
 * needs to be dynamic.
 * 
 * Irvine_BSEAVSW_Devel/3   12/4/03 9:39a erickson
 * PR8813: complete rework of info UI to support vertical bar
 * 
 * Irvine_BSEAVSW_Devel/2   10/10/03 1:43p erickson
 * converted brutus frontend code so that it can support all frontend
 * types simultaneously. this
 * is in preperation for 97038 and others.
 *
 * SanJose_Linux_Devel/11   4/11/03 2:17p erickson
 * 7115 work
 *
 * SanJose_Linux_Devel/10   4/10/03 4:22p erickson
 * info panel rework
 * added edit channels and outputs save-state feature
 *
 * SanJose_Linux_Devel/9   4/9/03 11:21a erickson
 * rework of pvrgui files and class names
 *
 ****************************************************************************/
#ifndef INFOPANEL_H__
#define INFOPANEL_H__

#include "mwidget.h"
#include "mlabel.h"
#include "mtimer.h"
#include "mbutton.h"
#include "infobar.h"
class Control;

/*=******************************
This module contains a set of classes which allow reuse among the
various infobar components. None of these is required in order to
use an infobar, but it makes things easier.

InfoBarPopup's are the simplest.
InfoPanel's support paging.
InfoPanelPage's work inside an InfoPanel.
InfoPanelStatePage's are a special case of InfoPanelPage.

*********************************/

/**
An InfoBarPopup is simply an MLabel with some hooks back into the
InfoBar and Control.
If you're using an infobar but not an infopanel, use this.
**/
class InfoBarPopup : public MLabel {
public:
	InfoBarPopup(InfoBar *infoBar, MWidget *parent, const MRect &rect);

	InfoBar *infoBar() const {return _infoBar;}
	Control *control() const {return infoBar()->control();}

protected:
	InfoBar *_infoBar;
};

/**
An InfoPanel contains mulitple pages and creates a tab control
for the pages.
It also manages time-based updates.

It provides automatic linkage to the InfoBar.
**/
class InfoPanel : public MLabel, public MTimerEventHandler,
	public MButtonEventHandler {
public:
	InfoPanel(InfoBar *infoBar, const MRect &rect);

	virtual void update() = 0;

	InfoBar *infoBar() const {return _infoBar;}
	Control *control() const {return _infoBar->control();}

	MRect infoClientRect();

	virtual void setPage(int page);
	int page() const {return _currentPage;}
	void incPage(int inc);

	// events
	void onTimeout(MTimer *self);
	void onClick(MButton *button);

	// overrides
	void show();
	void hide();

protected:
	InfoBar *_infoBar;
	MTimer _timer; // timer for updating pages
	int _currentPage;
	MLabel *_pageLabel;
	void updateLabel();
	void addPage(MWidget *page, const char *name, int msec);
	void setPageTimer(int pageIndex, int msec);

private:
	MList<MWidget> _pages;
	//MList<MButton> _tabs;
	MImage *_bgImage;
#define MAXPAGES 100
	int _pagetimers[MAXPAGES];
};

/**
An InfoPanelPage is simply an MLabel with some hooks back into the InfoPanel,
InfoBar and Control.
If you're using an InfoPanel, use this to create individual pages.
**/
class InfoPanelPage : public MLabel {
public:
	InfoPanelPage(InfoPanel *parent, const MRect &rect);

	InfoBar *infoBar() const {return _infopanel->infoBar();}
	Control *control() const {return infoBar()->control();}

protected:
	InfoPanel *_infopanel;
};

/**
An InfoPanelStatusPage is an InfoPage with an array of label/value pairs
which can be set.
**/
class InfoPanelStatusPage : public InfoPanelPage {
public:
	InfoPanelStatusPage(InfoPanel *parent, const MRect &rect);
	~InfoPanelStatusPage();
	virtual void update() = 0;

protected:
	void changeLabel(int index, const char *lbl);
	void clearLabels();
	void addLabel(int index, const char *lbl);
	void setLabelValue(int index, const char *format, ...);

#define MAX_INFOPANELPAGE_LABELS 20
	int totallabels;
	MLabel *labels[MAX_INFOPANELPAGE_LABELS];
	MLabel *values[MAX_INFOPANELPAGE_LABELS];
};

#endif //INFOPANEL_H__
