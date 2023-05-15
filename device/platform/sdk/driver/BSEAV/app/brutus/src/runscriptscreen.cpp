/***************************************************************************
 *     Copyright (c) 2002-2003, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: runscriptscreen.cpp $
 * $brcm_Revision: 1 $
 * $brcm_Date: 2/7/05 8:16p $
 *
 * Module Description: 2nd Collection of screens for pvrgui demo app.
 *  Screens are: Outputs, Settings, Admin, Audio, RunScript
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/runscriptscreen.cpp $
 * 
 * 1   2/7/05 8:16p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/8   4/19/04 2:02p erickson
 * PR10471: made all filepaths configurable using brutus.cfg
 * 
 * Irvine_BSEAVSW_Devel/7   3/9/04 10:15a erickson
 * PR9883: handle a few more conditions when starting a script
 * 
 * Irvine_BSEAVSW_Devel/6   2/4/04 2:12p erickson
 * PR9107: RunScriptScreen must obtain the ScriptEngine explicitly now
 * 
 * Irvine_BSEAVSW_Devel/5   1/29/04 2:26p erickson
 * PR9107: merging back from BRUTUS_PR9107 branch for bwin development
 * 
 * Irvine_BSEAVSW_Devel/4   9/22/03 2:33p erickson
 * reworked scripting to implemented prompt command. Now all scripting
 * happens in another thread.
 * This allows sleep to be a real sleep and allows blocking on read(stdin)
 * 
 * Irvine_BSEAVSW_Devel/3   9/17/03 12:31p erickson
 * converted from original debug interface to magnum debug interface
 * 
 * Irvine_BSEAVSW_Devel/2   7/23/03 4:01p erickson
 * renamed from pvrgui to brutus
 * 
 * SanJose_Linux_Devel/19   4/16/03 12:35p erickson
 * implemented font-string support
 * 
 * SanJose_Linux_Devel/18   4/9/03 11:22a erickson
 * rework of pvrgui files and class names
 ****************************************************************************/
#include "runscriptscreen.h"
#include "cfgsettings.h"
#include "scriptengine.h"
#include "control.h"
#include "mmessagebox.h"
#include "bstd.h"
BDBG_MODULE(brutus_runscriptscreen);

RunScriptScreen::RunScriptScreen(Control *acontrol) :
	GenericScreen(acontrol, NOTV),
	_bevel(this, MRect(20, 80, width()-40, 330), (const char *)NULL, "bevel"),
	_fileview(&_bevel, MRect(2,2,_bevel.width()-4, _bevel.height()-4)),
	_home(this, MRect(500, 25,120,40), "Home")
{
	MLabel *l1 = new MLabel(this, MRect(52-38,25,350,40), "Run Script");
	l1->setFont(MFont(app(), control()->cfg()->get(Config::SCREEN_TITLE_FONT)));
	_scriptEngine = NULL;
	_bevel.setBevel(2, MPainter::bsRaised);
	_home.addHandler(this);
	_fileview.setHomeDir(control()->cfg()->get(Config::SCRIPTS_PATH));
	_fileview.addHandler(this);
}

void RunScriptScreen::setScriptEngine(ScriptEngine *scriptEngine)
{
	_scriptEngine = scriptEngine;
}

void RunScriptScreen::onClick(MButton *sender)
{
	control()->showScreen(Control::eHome);
}

void RunScriptScreen::onClick(MListView *sender)
{
	if (_fileview.currentFile() && !_fileview.currentFileIsDir()) {
		if (!_scriptEngine)
			MMessageBox::info(fb(), "Script engine is not available.");
		else if (_scriptEngine->isActive())
			/* A script cannot invoke another script! */
			MMessageBox::info(fb(), "Script engine is already running.");
		else if (_scriptEngine->runScript(_fileview.currentFile()))
			MMessageBox::info(fb(), "Error running script.");
		else
			control()->showScreen(Control::eHome);
	}
}

void RunScriptScreen::show()
{
	// go to home dir a refresh
	_fileview.gotoDir(NULL);
	_fileview.focus();
	GenericScreen::show();
}
