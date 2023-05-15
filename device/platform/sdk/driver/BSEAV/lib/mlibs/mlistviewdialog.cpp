/***************************************************************************
 *     Copyright (c) 2002-2007, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: mlistviewdialog.cpp $
 * $brcm_Revision: 2 $
 * $brcm_Date: 2/2/07 10:09a $
 *
 * Module Description: Collection of screens for pvrgui demo app
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/mlibs/mlistviewdialog.cpp $
 * 
 * 2   2/2/07 10:09a erickson
 * PR26661: added MApplication::clearModalState. this should be called
 * after modal loop is done.
 * 
 * 1   2/7/05 11:15p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/2   1/29/04 2:26p erickson
 * PR9107: merging back from BRUTUS_PR9107 branch for bwin development
 * 
 * Irvine_BSEAVSW_Devel/1   4/10/03 4:20p erickson
 * added MListViewDialog
 *
 **************************************************************************/
#include "mlistviewdialog.h"
#include "mapplication.h"

MListViewDialog::MListViewDialog(MWidget *parent) :
	MLabel(parent, MRect(170,120,300,200)),
	_cancel(this, MRect(100,150,100,40), "Cancel"),
	_view(this, MRect(10,10,280,130))
{
	_result = 0;
	_cancel.addHandler(this);
	_view.addHandler(this);
	_view.setView(MListView::ListView);
	_view.setVScrollBarMode(MScrollView::Auto);
	_view.setHScrollBarMode(MScrollView::AlwaysOff);
	setBevel(5);
}

void MListViewDialog::onClick(MButton *sender)
{
	if (sender == &_cancel)
		_result = 2;
}

void MListViewDialog::onClick(MListView *sender)
{
	_result = 1;
}

void MListViewDialog::add(const char *name)
{
	MListViewItem *item = _view.add();
	item->setLabel(name);
}

const char *MListViewDialog::select()
{
	MWidget *wasFocus = app()->focusedWidget();
	show();
	raise();
	if (_view.total())
		_view.focus();
	else
		_cancel.focus();
	_result = 0;
	// process all messages while we wait for the result
	while (!_result && app()->processEvent(true));
    app()->clearModalState();
    
	MListViewItem *item = _view.current();
	hide();
	if (wasFocus)
		wasFocus->focus();

	if (_result != 1 || !item)
		return NULL;

	return item->label();
}
