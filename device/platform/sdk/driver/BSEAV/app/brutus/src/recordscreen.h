/***************************************************************************
 *	   Copyright (c) 2002-2007, Broadcom Corporation
 *	   All Rights Reserved
 *	   Confidential Property of Broadcom Corporation
 *
 *	THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *	AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *	EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: recordscreen.h $
 * $brcm_Revision: 7 $
 * $brcm_Date: 12/6/07 2:08p $
 *
 * Module Description: Collection of screens for pvrgui demo app
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/recordscreen.h $
 * 
 * 7   12/6/07 2:08p erickson
 * PR37038: add CONTINUOUS_RECORD_DURATION, remove unused fixedSize and
 * CIRCFILE.
 *
 * 6   12/6/07 12:50p erickson
 * PR37959: check _startNetrecord before using it. fix indenting.
 *
 * 5   8/9/07 5:31p jrubio
 * PR33830: Added Network Record changes
 *
 * 4   3/14/07 10:16a erickson
 * PR26267: refactor Brutus into separate VideoList and PlaybackScreen
 *
 * 3   5/4/06 3:33p erickson
 * PR21047: RecordScreen::canRecord depends on digital or encoder
 *
 * 2   4/7/06 10:52a ahulse
 * PR20504: Merge Vpp Modifications into main branch
 *
 * Vpp_7042_Demo/2	 4/4/06 1:18p ahulse
 * PR20504: Use a modal box to set vpp settings. Modal box is only active
 * if VPP_ENABLED is set
 *
 * Vpp_7042_Demo/1	 3/29/06 5:52p ahulse
 * PR20504: Enable support for changeing 7042's vpp settings when encoding
 * analog
 *
 * 1   2/7/05 8:16p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 *
 * Irvine_BSEAVSW_Devel/5	9/21/04 2:22p erickson
 * PR12706: allowed record source label to wrap and removed worthless
 * display label
 *
 * Irvine_BSEAVSW_Devel/4	8/27/03 4:40p erickson
 * converted encryption from checkbox to radiogroup to allow on-screen
 * selection of encryption type
 *
 * Irvine_BSEAVSW_Devel/3	8/21/03 9:43a erickson
 * venom2 settop api work
 *
 * Irvine_BSEAVSW_Devel/2	8/18/03 11:03a erickson
 * initial conversion of brutus to settop api, still in progress
 *
 * Irvine_BSEAVSW_Devel/1	4/9/03 11:22a erickson
 * rework of pvrgui files and class names
 ***************************************************************************/
#ifndef RECORDSCREEN_H__
#define RECORDSCREEN_H__

#include "genericscreen.h"
#include "mbutton.h"
#include "mradiobuttongroup.h"
#include "mlineedit.h"
#include "mlistbox.h"
#include "mlabel.h"
#include "video.h"

class EditVpp;

class RecordScreen : public GenericScreen {
friend class EditVpp;
public:
	RecordScreen(Control *control);
	~RecordScreen();

	void newVideo();
	bool focus() {return _le->focus();}
	bool startRecord();
	bool canRecord() const {return _canRecord;}
	void setSource();

	// events
	void onClick(MButton *self);

	// overrides
	void show();

protected:
	bool keyDown(int ascii, int modifiers, int scancode);
	EditVpp *_eVpp;
	MLabel *l1, *l2, *l3, *l4, *l5, *l6, *l7;
	MLabel *_display;
	MPushButton *_record, *_back, *_setGuide;
	MLineEdit *_le;
	MRadioButtonGroup *_quality;
	MCheckBox *_startPlayback;
	MCheckBox *_startNetrecord;
	MRadioButtonGroup *_encrypt;
	Video *_currentVideo;
	int _prevCh;
	bool _canRecord;
};

#endif
