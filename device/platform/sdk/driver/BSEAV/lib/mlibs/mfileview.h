/***************************************************************************
 *     Copyright (c) 2002, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: mfileview.h $
 * $brcm_Revision: 1 $
 * $brcm_Date: 2/7/05 11:10p $
 *
 * Module Description: MListView adapted to browsing a file system
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/mlibs/mfileview.h $
 * 
 * 1   2/7/05 11:10p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/4   4/30/03 9:06a erickson
 * added findItem and made file method
 *
 ***************************************************************************/
#ifndef MFILEVIEW_H
#define MFILEVIEW_H

#include "mlistview.h"
#include "mtimer.h"

class MFileView : public MListView, public MTimerEventHandler {
public:
	MFileView(MWidget *parent, const MRect &rect, const char *name = NULL);

	void setHomeDir(const char *homedir);
	void refresh();
	void gotoDir(const char *dir = NULL);
	void addToPath(const char *path, bool refresh = true);

	MString file(MListViewItem *item);
	MString currentFile() {return file(current());}
	bool currentFileIsDir();
	const char *currentDir() const {return _currentDir;}
	const char *homeDir() const {return _homeDir;}

	/**
	* Tries to find the item in the current directory.
	**/
	MListViewItem *findItem(const char *pathAndFile);

	// events
	void onClick(MButton *sender);
	void onTimeout(MTimer *sender);

protected:
	MString _homeDir, _currentDir;
	MTimer _timer;

	class MFileViewItem : public MListViewItem {
	public:
		MFileViewItem(MListView *v, MWidget *parent) : MListViewItem(v,parent) {}
		bool isDir;
	};

	virtual MListViewItem *createItem();
};

#endif // MFILEVIEW_H
