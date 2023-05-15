/***************************************************************************
 *     Copyright (c) 2002-2005, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: wow.h $
 * $brcm_Revision: 2 $
 * $brcm_Date: 1/13/06 1:53p $
 *
 * Module Description: Window-on-Window base class
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/wow.h $
 * 
 * 2   1/13/06 1:53p mphillip
 * PR19051: Replaced a pip frame resize call from prior to the sbs/wow
 * rework of pip.
 * 
 * 1   12/20/05 5:30p mphillip
 * PR16055: Abstract class and two sample classes to handle PIP and Side
 * By Side in one place
 * 
 * 
 ****************************************************************************/
#ifndef WOW_H__
#define WOW_H__

#include "bsettop.h"
#include "bsettop_display.h"
#include "input_devicecodes.h"

typedef enum {
	eFullScreenTV, /* Both main and PIP visible. */
	eLargeTV, /* Brutus menu screen. Only one visible. */
	eSmallTV  /* Brutus playback, record screens. Only one visible. */
	/* No window visible has no enum */
} eScreenType;

/**
Window-On-Window base class.  The PIP and Side-by-Side implementations are derived from this.
**/
class WindowOnWindow {
public:
        WindowOnWindow();
        WindowOnWindow(Control *acontrol);
        WindowOnWindow(GenericScreen *screen, Control *acontrol);
        ~WindowOnWindow();

        virtual MRect MainRect(eScreenType screen_type);
        virtual MRect MainRect(eScreenType screen_type, bdisplay_settings *display_settings);
        virtual MRect WowRect(eScreenType screen_type);
        virtual MRect WowRect(eScreenType screen_type, bdisplay_settings *display_settings);
	void SetControl(Control *acontrol);

	virtual MWidget *WowWidget();

        virtual bresult ShowWow( bool show, bool selected); /* true == show, false == hide */
        virtual void Resized();

        bool IsChangeable(); /* wow is resizable or movable, so both rects may need re-fetching */
        bool HandlesInput(); /* wow takes input */

        virtual void ProcessCommand(BrutusCommand cmd);

	Control *control() const { return _control; }
protected:
	Control *_control;
        bool _changeable;
        bool _input;
};

#if BCM_BOARD == 97398 && !defined B_SYSTEM_linux && !defined B_SYSTEM_linuxkernel && !defined B_SYSTEM_vxworks
/* see BSEAV/api/src/magnum/board/bsettop_bsp_7038.c for sys memory
allocation. If there's not enough memory, there are VDC constraints which 
require Brutus to avoid some behavior. */
#define LIMITED_PIP_SIZE
#endif

/* Standard Picture-In-Picture (PIP) implementation */
struct PipData;
class PipWindowOnWindow : public WindowOnWindow {
public:
        PipWindowOnWindow();
        PipWindowOnWindow(Control *acontrol);
        PipWindowOnWindow(GenericScreen *screen, Control *acontrol);
        ~PipWindowOnWindow();
        virtual MRect MainRect(eScreenType screen_type);
        virtual MRect MainRect(eScreenType screen_type, bdisplay_settings *display_settings);
        virtual MRect WowRect(eScreenType screen_type);
        virtual MRect WowRect(eScreenType screen_type, bdisplay_settings *display_settings);
        virtual bresult ShowWow( bool show, bool selected); /* true == show, false == hide */

        virtual void Resized();

	virtual MWidget *WowWidget();
protected:
	struct PipData *_private;
};

/* Standard Side-By-Side (SBS) implementation */
struct SbsData;
class SbsWindowOnWindow : public WindowOnWindow {
public:
        SbsWindowOnWindow();
        SbsWindowOnWindow(Control *acontrol);
        SbsWindowOnWindow(GenericScreen *screen, Control *acontrol);
        ~SbsWindowOnWindow();
        virtual MRect MainRect(eScreenType screen_type);
        virtual MRect MainRect(eScreenType screen_type, bdisplay_settings *display_settings);
        virtual MRect WowRect(eScreenType screen_type);
        virtual MRect WowRect(eScreenType screen_type, bdisplay_settings *display_settings);
        virtual bresult ShowWow( bool show, bool selected); /* true == show, false == hide */

	virtual MWidget *WowWidget();

        virtual void ProcessCommand(BrutusCommand cmd);
protected:
	struct SbsData *_private;
};
#endif // WOW_H__
