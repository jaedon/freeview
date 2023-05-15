/***************************************************************************
 *     Copyright (c) 2003-2008, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: info_misc.cpp $
 * $brcm_Revision: 6 $
 * $brcm_Date: 3/17/08 3:18p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/info/info_misc.cpp $
 * 
 * 6   3/17/08 3:18p erickson
 * PR40307: remove old, unused code before adding new
 * 
 * 5   5/31/06 4:20p tokushig
 * PR21891: Merging updated picture viewer into mainline.
 * 
 * SanDiego_Brutus_Skin/2   5/16/06 11:33a tokushig
 * merge from main
 * 
 * 4   4/17/06 5:12p jgarrett
 * PR 20951: Merging skinned UI into mainline
 * 
 * SanDiego_Brutus_Skin/1   3/30/06 10:52a tokushig
 * remove prerendering of icon if magnum based.  since we are using the
 * blitter to display pixmaps, any pixmap with alpha values cannot be
 * displayed properly.  legacy platforms will operate as usual
 * 
 * 2   4/18/05 10:56a erickson
 * PR14593: prerender images for faster draw time on slow platforms
 * 
 * 1   2/7/05 8:09p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/20   10/15/04 10:51a erickson
 * PR13014: infobar rework
 *
 ****************************************************************************/
#include "info_misc.h"
#include "cfgsettings.h"
#include "control.h"
#include <bstd.h>
BDBG_MODULE(info_misc);

#define PIXMAPPATH (MString(infoBar->control()->cfg()->get(Config::IMAGES_PATH)) + "/info/")

ExitInfoBarPlugin::ExitInfoBarPlugin(InfoBar *infoBar):
    InfoBarPlugin(infoBar, "hide_plugin")
{
    setText("Hide");
    _icon = new MImage(app(), PIXMAPPATH + "exit.png");
}

ExitInfoBarPlugin::~ExitInfoBarPlugin()
{
    delete _icon;
}

bool ExitInfoBarPlugin::keyDown(int ascii, int modifiers, int scancode)
{
    switch (ascii) {
    case MWKEY_ENTER:
        infoBar()->hide();
        throwFocus();
        return true;
    default: return MLabel::keyDown(ascii, modifiers, scancode);
    }
}
