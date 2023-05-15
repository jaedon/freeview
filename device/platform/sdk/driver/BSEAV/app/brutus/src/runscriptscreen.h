/***************************************************************************
 *     Copyright (c) 2002-2003, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: runscriptscreen.h $
 * $brcm_Revision: 1 $
 * $brcm_Date: 2/7/05 8:17p $
 *
 * Module Description: Collection of screens for pvrgui demo app
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/runscriptscreen.h $
 * 
 * 1   2/7/05 8:17p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/2   2/4/04 2:12p erickson
 * PR9107: RunScriptScreen must obtain the ScriptEngine explicitly now
 * 
 * Irvine_BSEAVSW_Devel/1   4/9/03 11:22a erickson
 * rework of pvrgui files and class names
 ***************************************************************************/
#ifndef RUNSCRIPTSCREEN_H__
#define RUNSCRIPTSCREEN_H__

#include "genericscreen.h"
#include "mbutton.h"
#include "mfileview.h"
#include "mlabel.h"
#include "scriptengine.h"

class RunScriptScreen : public GenericScreen,
	public MListViewEventHandler
{
public:
	RunScriptScreen(Control *control);

	/* The linkage to a script engine is external to Control. It must be
	passed in by the application main routine. */
	void setScriptEngine(ScriptEngine *scriptEngine);

	// events
	void onClick(MButton *self);
	void onClick(MListView *sender);

	// overrides
	void show();
	void rememberFocus() {}

protected:
	MLabel _bevel;
	MFileView _fileview;
	MPushButton _home;
	ScriptEngine *_scriptEngine;
};

#endif
