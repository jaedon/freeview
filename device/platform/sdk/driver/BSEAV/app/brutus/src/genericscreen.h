/***************************************************************************
 *     Copyright (c) 2002-2006, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: genericscreen.h $
 * $brcm_Revision: 3 $
 * $brcm_Date: 5/31/06 4:20p $
 *
 * Module Description: GUI for pvr state
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/genericscreen.h $
 * 
 * 3   5/31/06 4:20p tokushig
 * PR21891: Merging updated picture viewer into mainline.
 * 
 * SanDiego_Brutus_Skin/3   5/16/06 11:33a tokushig
 * merge from main
 * 
 * 2   4/17/06 5:06p jgarrett
 * PR 20951: Merging skinned UI into mainline
 * 
 * SanDiego_Brutus_Skin/2   1/11/06 11:18a tokushig
 * make sure tvwindow is on top in zorder
 * 
 * SanDiego_Brutus_Skin/1   1/3/06 10:25a tokushig
 * Added XML based skin capability to brutus.  "-skin <name>" command line
 * option
 * allows user to specify skin on startup.  Note that running brutus
 * without the
 * -skin option will use the default GUI.
 * 
 * 1   2/7/05 8:07p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/6   1/7/05 10:41a erickson
 * PR11077: added BACKGROUND_IMAGE support
 * 
 * Irvine_BSEAVSW_Devel/5   2/19/04 1:43p erickson
 * PR9684: added name to GenericScreen for debugging
 * 
 * Irvine_BSEAVSW_Devel/4   2/13/04 4:07p erickson
 * PR8850: don't resize decode window to NOTV dimensions
 * 
 * Irvine_BSEAVSW_Devel/3   8/13/03 10:43a erickson
 * fixed default key processing
 * 
 * Irvine_BSEAVSW_Devel/2   7/23/03 4:00p erickson
 * renamed from pvrgui to brutus
 * 
 * Irvine_BSEAVSW_Devel/1   4/9/03 11:20a erickson
 * rework of pvrgui files and class names
 *
 ****************************************************************************/
#ifndef GENERICSCREEN_H__
#define GENERICSCREEN_H__

#include "mbutton.h"

class GenericScreen;
class Control;
class Config;

class TVBevel : public MPushButton {
public:
    TVBevel(GenericScreen *parent, Config *cfg, const MRect &rect);
protected:
    void draw(const MRect &cliprect);
    Config *_cfg;
};

class GenericScreen : public MWidget, public MButtonEventHandler {
public:
    /**
    * Graphics screen with TV window
    */
    GenericScreen(Control *control, const MRect &tvrect, const char *name = NULL);
    /**
    * TV screen
    */
    GenericScreen(Control *control, const char *name = NULL);

    void repaintTVBevel();
    void onClick(MButton *self);
    Control *control() const {return _control;}
    Config *cfg() const;
    
    /**
    Summary:
    Returns true if a Picture-In-Graphics is visible.
    **/
    bool hasTV();

    void setGeometryTV(const MRect &r) { _tvbevel.setGeometry(r); _tvbevel.raise();};

    /**
    * rememberFocus is convenient, but awfully dangerous. See .cpp notes.
    **/
    virtual void rememberFocus();

    bool focus();

protected:
    TVBevel _tvbevel;
    MWidget *_lastFocus;

private:
    Control *_control;
    void init(bool useColorKey);
};

#endif // GENERICSCREEN_H__
