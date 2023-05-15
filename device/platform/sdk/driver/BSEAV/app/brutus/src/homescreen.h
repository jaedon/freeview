/***************************************************************************
 *     Copyright (c) 2002-2009, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: homescreen.h $
 * $brcm_Revision: 9 $
 * $brcm_Date: 7/8/09 11:43a $
 *
 * Module Description: Collection of screens for pvrgui demo app
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/homescreen.h $
 * 
 * 9   7/8/09 11:43a erickson
 * PR56373: fix mem leak
 *
 * 8   7/2/09 5:47p erickson
 * PR56373: add standby button
 *
 * 7   10/8/08 11:53a tokushig
 * PR43426: merge to main
 *
 * BRUTUS_97405_97043/2   10/8/08 10:50a tokushig
 * PR43426: move global _buttonActionList to a class attribute for
 * homescreen and videoscreen.  this will allow the code to work properly
 * when 2 instances of brutus exist as well as avoid a global constructor
 * issue in vxworks.
 *
 * 6   9/15/08 12:40p tokushig
 * PR43426: merge to main
 *
 * BRUTUS_97405_97043/1   7/31/08 5:38p tokushig
 * PR45076: added button scrolling on homescreen and blue skin support.
 *
 * 5   5/7/08 11:51a erickson
 * PR42329: fix minor BKNI memory leaks
 *
 * 4   5/31/06 4:20p tokushig
 * PR21891: Merging updated picture viewer into mainline.
 *
 * SanDiego_Brutus_Skin/3   5/16/06 11:33a tokushig
 * merge from main
 *
 * 3   4/17/06 5:06p jgarrett
 * PR 20951: Merging skinned UI into mainline
 *
 * SanDiego_Brutus_Skin/2   1/19/06 12:48p tokushig
 * update merge from main
 *
 * SanDiego_Brutus_Skin/1   1/3/06 10:25a tokushig
 * Added XML based skin capability to brutus.  "-skin <name>" command line
 * option
 * allows user to specify skin on startup.  Note that running brutus
 * without the
 * -skin option will use the default GUI.
 *
 * 2   12/20/05 6:22a erickson
 * PR18256: reworked HomeScreen to use Config enum and eliminate #defines
 * in the headerfile
 *
 * 1   2/7/05 8:07p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 *
 * Irvine_BSEAVSW_Devel/4   11/2/04 3:45p arbisman
 * PR13142: Moved DSG status screen to a separate screen
 *
 * Irvine_BSEAVSW_Devel/3   9/13/04 1:54p erickson
 * PR11081: replaced Outputs screen with Info panel button
 *
 * Irvine_BSEAVSW_Devel/2   9/9/04 1:54p erickson
 * PR12608: convert JPEG_SUPPORT to PICTURES_SUPPORT
 *
 * SanJose_Linux_Devel/57   4/16/03 12:35p erickson
 * implemented font-string support
 *
 * SanJose_Linux_Devel/56   4/15/03 12:39p erickson
 * 7115 work, added more build conditionals
 *
 * SanJose_Linux_Devel/55   4/9/03 11:21a erickson
 * rework of pvrgui files and class names
 ***************************************************************************/
#ifndef HOMESCREEN_H__
#define HOMESCREEN_H__

#include <mlistbox.h>
#include "genericscreen.h"
#include "mbutton.h"
#include "mlabel.h"
#include "mlist.h"
#include "control.h"
#include "buttonlistbox.h"

class HomeScreen : public GenericScreen, public MListBoxEventHandler {
public:
    HomeScreen(Control *control, MRect rectTVWin = LARGETV);
    ~HomeScreen();
    void onClick(MButton *button);
    void onItemClicked(MListBox *listbox);

protected:
    void createButton(ButtonListBox * pListBox, int index, const char * name,
                      MImage * icon, eSkinAction action);

    MList <eSkinAction> _buttonActionList;
    MImage *_homelogo;
    MPushButton *_standby;
    MImage *_standbyLogo;

    MColumnListBox * listbox;
};

#endif

