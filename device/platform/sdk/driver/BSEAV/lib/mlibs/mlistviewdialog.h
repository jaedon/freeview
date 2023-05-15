/***************************************************************************
 *     Copyright (c) 2002-2003, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: mlistviewdialog.h $
 * $brcm_Revision: 1 $
 * $brcm_Date: 2/7/05 11:15p $
 *
 * Module Description: Collection of screens for pvrgui demo app
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/mlibs/mlistviewdialog.h $
 * 
 * 1   2/7/05 11:15p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/1   4/10/03 4:20p erickson
 * added MListViewDialog
 *
 **************************************************************************/
#ifndef MLISTVIEWDIALOG_H__
#define MLISTVIEWDIALOG_H__

#include "mlabel.h"
#include "mbutton.h"
#include "mlistview.h"

class MListViewDialog : public MLabel,
	public MButtonEventHandler,
	public MListViewEventHandler
{
public:
	MListViewDialog(MWidget *parent);

	void add(const char *name);
	const char *select();

	// event handlers
	void onClick(MButton *sender);
	void onClick(MListView *sender);

protected:
	MListView _view;
	MPushButton _cancel;
	int _result;
};

#endif //MLISTVIEWDIALOG_H__
