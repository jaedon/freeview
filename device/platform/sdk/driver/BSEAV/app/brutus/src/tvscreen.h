/***************************************************************************
 *     Copyright (c) 2002-2005, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: tvscreen.h $
 * $brcm_Revision: 2 $
 * $brcm_Date: 12/20/05 5:28p $
 *
 * Module Description: GUI for pvr state
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/tvscreen.h $
 * 
 * 2   12/20/05 5:28p mphillip
 * PR16055: Initial rework of the PIP system to include a generic Window
 * on Window architecture
 * 
 * 1   2/7/05 8:18p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/15   12/3/04 3:19p erickson
 * PR13256: moved pip gfx size logic to tvscreen
 * 
 * Irvine_BSEAVSW_Devel/14   10/15/04 10:51a erickson
 * PR13014: infobar rework
 * 
 * Irvine_BSEAVSW_Devel/13   9/13/04 1:55p erickson
 * PR11081: info panel is now tightly integrated with TVMessage
 * 
 * Irvine_BSEAVSW_Devel/12   7/20/04 10:38a erickson
 * PR11682: updated tvscreen ui
 * 
 * Irvine_BSEAVSW_Devel/11   5/4/04 10:12a erickson
 * PR10782: more space given
 * 
 * Irvine_BSEAVSW_Devel/10   4/6/04 3:48p bandrews
 * PR10368: Added goto(info) command to reset infobar plugin focus to the
 * top.  goto(tv) will now turn off info as well.
 * 
 * Irvine_BSEAVSW_Devel/9   4/2/04 5:39p bandrews
 * PR10031: Fixed to use timeline bgcolor and make the recordindicator
 * look like it's part of the timeline while playback is enabled.
 * 
 * Irvine_BSEAVSW_Devel/8   1/29/04 2:26p erickson
 * PR9107: merging back from BRUTUS_PR9107 branch for bwin development
 * 
 * Irvine_BSEAVSW_Devel/7   12/19/03 10:05a erickson
 * PR8731: For POD demo, we undef PVR_SUPPORT only for the UI. Also, I
 * removed the "NEW_UI" code.
 * 
 * Irvine_BSEAVSW_Devel/6   12/10/03 10:25p erickson
 * PR8813: don't repaint tvscreen when it loses focus
 * 
 * Irvine_BSEAVSW_Devel/5   12/8/03 3:12p erickson
 * PR8813: still working on getting new info UI smooth
 * 
 * Irvine_BSEAVSW_Devel/4   12/4/03 9:41a erickson
 * PR8813: info system rework
 * 
 * Irvine_BSEAVSW_Devel/3   7/1/03 5:57p erickson
 * fixed for 7320
 * 
 * Irvine_BSEAVSW_Devel/2   6/30/03 1:34p erickson
 * new ui work
 * 
 * Irvine_BSEAVSW_Devel/2   4/15/03 12:40p erickson
 * 7115 work, added more build conditionals
 * 
 * Irvine_BSEAVSW_Devel/1   4/9/03 11:22a erickson
 * rework of pvrgui files and class names
 *
 ****************************************************************************/
#ifndef TVSCREEN_H__
#define TVSCREEN_H__

#include "wow.h"
#include "genericscreen.h"
#include <mlabel.h>
#include <mtimer.h>

struct TVScreenData;

/**
Full-screen TV ui. This controls overlay graphics including:
1) infopanel
2) pvr timelines
3) other popups
**/
class TVScreen : public GenericScreen {
public:
	TVScreen(Control *control);
	~TVScreen();

	void showPipOutline(bool focused);
	void hidePipOutline();

	void setPlayEngines();
	void updateRecordIndicator();

	bool isInfoVisible();
	void showMessage(bool withinfo = false);
	void hideMessage();

	// pip graphics must change when switching between hd and sd
	void setPipGeometry();

	// overrides
	bool focus();
	bool focusUpperLeft();
	void focusRepaint() {}

	// current WindowOnWindow
	WindowOnWindow *wow();

protected:
	bool keyDown(int ascii, int modifiers, int scancode);
	TVScreenData *_private;
	
	/* Functions which calculate sizes of graphics objects */
	MRect timelineRect(int disp) const;
	MRect recordIndicatorRect() const;
	MRect fullScreenPipOutlineRect() const;
};

#endif // TVSCREEN_H__
