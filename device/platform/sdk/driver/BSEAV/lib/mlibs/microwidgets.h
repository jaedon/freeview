/***************************************************************************
 *     Copyright (c) 2001-2004, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: microwidgets.h $
 * $brcm_Revision: 1 $
 * $brcm_Date: 2/7/05 11:13p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/mlibs/microwidgets.h $
 * 
 * 1   2/7/05 11:13p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/2   2/26/04 11:03a erickson
 * PR9900: now using MListItr to allow const access to MList's
 *
 ****************************************************************************/
#ifndef MICROWIDGETS_H
#define MICROWIDGETS_H

// global include file
#include <stdlib.h>

#include "mlist.h"
#include "mgeom.h"

////////////////////////////////////////////////////
// Event macros

#define BEGIN_EV(CLASS) \
	class CLASS; \
	class CLASS##EventHandler { public:
#define EV_METHOD(CLASS, METHOD) \
	virtual void on##METHOD(CLASS *self) {}
#define END_EV() \
	};

#define SUPPORT_EV(CLASS) \
public: \
	void addHandler(CLASS##EventHandler *hnd) {_##CLASS##handlers.add(hnd);} \
	void removeHandler(CLASS##EventHandler *hnd) {_##CLASS##handlers.remove(hnd);} \
protected: \
	MList<CLASS##EventHandler> _##CLASS##handlers;

#define SUPPORT_EV_METHOD(METHOD) \
	protected: void fire##METHOD();

/**
There is a fundamental problem with firing these events. That is that in the callback,
the list might get modified. One way to completely prevent this is to make fire##METHOD const,
and then make all classes passed to event handlers const. That might be too drastic.
By using an itr, at least we avoid losing our spot if the event list is reaccessed.
**/
#define FIRE_EV_METHOD(CLASS, METHOD) \
	void CLASS::fire##METHOD() { \
		MListItr<CLASS##EventHandler> itr(&_##CLASS##handlers); \
		for (CLASS##EventHandler *h = itr.first(); h; h = itr.next()) \
			h->on##METHOD(this); \
	}
#define FIRE(METHOD) fire##METHOD()

// Keys
// NOTE: the following definitions are carried over from Microwindows,
// but a conversion layer could be created if Microwidgets ran on another
// windowing system.

#define MWKEY_UNKNOWN           0
/* Following special control keysyms are mapped to ASCII*/
#define MWKEY_BACKSPACE         8
#define MWKEY_TAB               9
#define MWKEY_ENTER             13
#define MWKEY_ESCAPE            27
/* Keysyms from 32-126 are mapped to ASCII*/

#define MWKEY_NONASCII_MASK     0xFF00
/* Following keysyms are mapped to private use portion of Unicode-16*/
/* arrows + home/end pad*/
#define MWKEY_FIRST             0xF800
#define MWKEY_LEFT              0xF800
#define MWKEY_RIGHT             0xF801
#define MWKEY_UP                0xF802
#define MWKEY_DOWN              0xF803
#define MWKEY_INSERT            0xF804
#define MWKEY_DELETE            0xF805
#define MWKEY_HOME              0xF806
#define MWKEY_END               0xF807
#define MWKEY_PAGEUP            0xF808
#define MWKEY_PAGEDOWN          0xF809

/* Numeric keypad*/
#define MWKEY_KP0               0xF80A
#define MWKEY_KP1               0xF80B
#define MWKEY_KP2               0xF80C
#define MWKEY_KP3               0xF80D
#define MWKEY_KP4               0xF80E
#define MWKEY_KP5               0xF80F
#define MWKEY_KP6               0xF810
#define MWKEY_KP7               0xF811
#define MWKEY_KP8               0xF812
#define MWKEY_KP9               0xF813
#define MWKEY_KP_PERIOD         0xF814
#define MWKEY_KP_DIVIDE         0xF815
#define MWKEY_KP_MULTIPLY       0xF816
#define MWKEY_KP_MINUS          0xF817
#define MWKEY_KP_PLUS           0xF818
#define MWKEY_KP_ENTER          0xF819
#define MWKEY_KP_EQUALS         0xF81A

/* Function keys */
#define MWKEY_F1                0xF81B
#define MWKEY_F2                0xF81C
#define MWKEY_F3                0xF81D
#define MWKEY_F4                0xF81E
#define MWKEY_F5                0xF81F
#define MWKEY_F6                0xF820
#define MWKEY_F7                0xF821
#define MWKEY_F8                0xF822
#define MWKEY_F9                0xF823
#define MWKEY_F10               0xF824
#define MWKEY_F11               0xF825
#define MWKEY_F12               0xF827

/* Key state modifier keys*/
#define MWKEY_NUMLOCK           0xF828
#define MWKEY_CAPSLOCK          0xF829
#define MWKEY_SCROLLOCK         0xF82A
#define MWKEY_LSHIFT            0xF82B
#define MWKEY_RSHIFT            0xF82C
#define MWKEY_LCTRL             0xF82D
#define MWKEY_RCTRL             0xF82E
#define MWKEY_LALT              0xF82F
#define MWKEY_RALT              0xF830
#define MWKEY_LMETA             0xF831
#define MWKEY_RMETA             0xF832
#define MWKEY_ALTGR             0xF833

/* Misc function keys*/
#define MWKEY_PRINT             0xF834
#define MWKEY_SYSREQ            0xF835
#define MWKEY_PAUSE             0xF836
#define MWKEY_BREAK             0xF837
#define MWKEY_QUIT              0xF838  /* virtual key*/
#define MWKEY_MENU              0xF839  /* virtual key*/
#define MWKEY_REDRAW            0xF83A  /* virtual key*/

/* Handheld function keys*/
#define MWKEY_RECORD            0xF840
#define MWKEY_PLAY              0xF841
#define MWKEY_CONTRAST          0xF842
#define MWKEY_BRIGHTNESS        0xF843
#define MWKEY_SELECTUP          0xF844
#define MWKEY_SELECTDOWN        0xF845
#define MWKEY_ACCEPT            0xF846
#define MWKEY_CANCEL            0xF847
#define MWKEY_APP1              0xF848
#define MWKEY_APP2              0xF849
#define MWKEY_LAST              0xF849

#define MWKMOD_NONE             0x0000
#define MWKMOD_LSHIFT           0x0001
#define MWKMOD_RSHIFT           0x0002
#define MWKMOD_LCTRL            0x0040
#define MWKMOD_RCTRL            0x0080
#define MWKMOD_LALT             0x0100
#define MWKMOD_RALT             0x0200
#define MWKMOD_LMETA            0x0400          /* Windows key*/
#define MWKMOD_RMETA            0x0800          /* Windows key*/
#define MWKMOD_NUM              0x1000
#define MWKMOD_CAPS             0x2000
#define MWKMOD_ALTGR            0x4000

#define MWKMOD_CTRL     (MWKMOD_LCTRL|MWKMOD_RCTRL)
#define MWKMOD_SHIFT    (MWKMOD_LSHIFT|MWKMOD_RSHIFT)
#define MWKMOD_ALT      (MWKMOD_LALT|MWKMOD_RALT)
#define MWKMOD_META     (MWKMOD_LMETA|MWKMOD_RMETA)

//This is verified in mwidget.cpp using #if sizeof()
// for now, it's hardcoded for RG565.
#define MW_PIXEL unsigned short

#endif //MICROWIDGETS_H
