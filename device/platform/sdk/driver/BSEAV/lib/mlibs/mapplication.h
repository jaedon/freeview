/***************************************************************************
 *     Copyright (c) 2002-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: mapplication.h $
 * $brcm_Revision: 5 $
 * $brcm_Date: 1/19/12 4:10p $
 *
 * Module Description: 
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/mlibs/mapplication.h $
 * 
 * 5   1/19/12 4:10p gmohile
 * SW7425-2213 : Add standby support
 * 
 * 4   2/2/07 10:09a erickson
 * PR26661: added MApplication::clearModalState. this should be called
 * after modal loop is done.
 * 
 * 3   12/21/06 5:35p ahulse
 * PR26203: When in modal state, redraw modal box on entering idle
 * 
 * 2   4/8/05 7:33p erickson
 * PR14701: tickCount should be static. no state kept.
 * 
 * 1   2/7/05 11:09p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/9   4/30/04 3:13p erickson
 * PR10883: MApplication now keeps a list of MFrameBuffers and
 * MFrameBuffer keeps a list of top level widgets
 * 
 * Irvine_BSEAVSW_Devel/8   3/8/04 11:31a erickson
 * PR9883: added MApplication name for debugging dual output/single app
 * configuration
 * 
 * Irvine_BSEAVSW_Devel/7   2/26/04 11:03a erickson
 * PR9900: now using MListItr to allow const access to MList's
 * 
 * Irvine_BSEAVSW_Devel/6   2/5/04 11:51a erickson
 * PR9629: Don't allocate a default style as a global variable.
 * 
 * Irvine_BSEAVSW_Devel/5   2/4/04 2:11p erickson
 * PR9107: implemented new device io system
 * 
 * Irvine_BSEAVSW_Devel/4   1/29/04 2:25p erickson
 * PR9107: merging back from BRUTUS_PR9107 branch for bwin development
 *
 * Irvine_BSEAVSW_Devel/3   8/13/03 10:42a erickson
 * made setMainWidget truly optional
 * added globalKeyProcessor
 *
 * Irvine_BSEAVSW_Devel/2   7/24/03 6:02p erickson
 * new IdleLock functions to allow other threads to interact with
 * microwidgets code
 *
 * Irvine_BSEAVSW_Devel/13   4/11/03 2:16p erickson
 * added terminate(), changed header
 *
 *************************************************************************/
#ifndef MAPPLICATION_H
#define MAPPLICATION_H

#include "mwidget.h"
#include "mtimer.h"

#ifdef BWIN_SUPPORT
#include "bwin.h"
#endif

class MIoDevice;

/**
Summary:
The main message processor for mwidgets.
Description:
It implemenents the main message loop which feeds paint events to the widgets.
It also manages focus and IO devices.
**/
class MApplication {
	friend class MWidget;
	friend class MTimer;
	friend class MFont;
	friend class MPixmap;
	friend class MImage;
	friend class MIoDevice;
	friend class MFrameBuffer; // _fbs.add/remove
public:
	struct Settings {
		bool keyboard_enabled;
#ifdef BWIN_SUPPORT
		bwin_engine_t win_engine;
#endif
	};

	/**
	Summary:
	Initialize the Settings structure before use.
	**/
	static void getDefaultSettings(Settings *settings);

	/**
	Summary:
	Create a new microwidgets application.
	**/
	MApplication(const Settings *settings, const char *name = NULL);

	/**
	Summary:
	Destroy the application.
	**/
	virtual ~MApplication();

	/**
	Summary:
	Check if the constructor completed successfully.
	**/
	bool isValid() const {return _valid;}

	/**
	Summary:
	When the main widget closes, the application terminates.
	Description:
	This is optional.
	**/
	void setMainWidget(MWidget *main) {_main = main;}

	/**
	Summary:
	When the no one wants a keyDown, it is sent to the global key processor widget.
	Description:
	This is optional.
	**/
	void setGlobalKeyProcessor(MWidget *globalKeyProcessor) {_globalKeyProcessor = globalKeyProcessor;}

	/**
	Summary:
	Main application loop. Returns the exit code.
	**/
	int run();

	/**
	Summary:
	Process a single event from the message queue. Returns true if app is still
	running, false if application is shutting down.
	*/
	bool processEvent(bool modalState = false);

	/**
	Summary:
	Test is an event is pending without consuming it.
	**/
	bool eventPending();

	/**
	Summary:
	If inModalState() is true, then there is a modal dialog up.
	Any timer processing should take this into account.
	*/
	bool inModalState() const {return _modalState;}
	
	/* After calling processEvent(true) for the last time (when exiting the loop),
	call this function. This is important if applications are limiting behavior based on inModalState() */
	void clearModalState() {_modalState = false;}

	/**
	Summary:
	Check if the application is in a terminating state.
	**/
	bool terminating() const {return _terminating;}

	/**
	Summary:
	Check the standby state
	**/
	bool isStandby() const {return _standby;}

	/**
	Summary:
	Set the termination flag which will cause run() to exit.
	**/
	void terminate();

	/**
	Summary:
	Set the standby flag 
	**/
	void standby();

	/**
	Summary:
	Clear the standby flag 
	**/
	void resume();

	/**
	Summary:
	Return the currently focused widget, which must be visible.
	**/
	MWidget *focusedWidget() const;


	/**
	Summary:
	Find a widget based on its id.
	**/
	MWidget *findWidget(int wid) const;


	/**
	Summary:
	Tab from the currently focused widget.
	**/
	void tab(MWidget::TabDirection dir);

	/**
	Summary:
	Send a key down message to the focused widget, then to parents.
	**/
	void sendKeyDown(MWidget *widget, int ascii, int modifiers, int scancode);

	/**
	Summary:
	Returns a tick count in millisecond.
	Description:
	This tick count may wrap around at any time. The initial value is undefined.
	**/
	static unsigned long tickCount();

	/**
	Summary:
	Set the font that is used if no other font has been specified for a widget.
	**/
	void setDefaultFont(MFont font) {_defaultFont = font;}

	/**
	Summary:
	Get the application default font.
	**/
	MFont defaultFont() const {return _defaultFont;}

	/**
	Summary:
	Set the default style object to be used if no other style object has
	been specified for a widget.
	**/
	void setStyle(MStyle *style) {_style = style;}

	/**
	Summary:
	Get the application default style.
	**/
	MStyle *style() const {return _style;}

	/**
	Summary:
	This allows another thread to join the microwidget thread.
	The lock is acquired when onIdle is called. microwidgets will then block
	in onIdle until releaseIdleLock is called.
	**/
	void acquireIdleLock();

	/**
	Summary:
	Release the lock obtained by acquireIdleLock.
	**/
	void releaseIdleLock();

protected:
	virtual void onIdle();

	void widgetDestroyed(MWidget *w);

	/**
	* List of all running MTimer instances.
	*/
	void add(MTimer *t) {_timers.add(t);}
	void remove(MTimer *t) {_timers.remove(t);}
	MList<MTimer> _timers;

	void add(MIoDevice *io);
	void remove(MIoDevice *io);
	MList<MFrameBuffer> _fbs;

private:
	MString _name;
	MWidget *_main;
	MWidget *_globalKeyProcessor;
	MFont _defaultFont;
	bool _terminating;
	bool _standby;
	MWidget *_lastEventWidget;
	MWidget *_modalWidget;		/* Modal Dialog box widget */	
	bool _modalState;
	MStyle *_style;
	MStyle _defaultStyle;
	bool _valid;
	struct Private;
	Private *_private;
	MWidget *findEventWidget(int wid) const;

	/* Internal function to set focus. Use MWidget::setFocus for external use. */
	void setFocus(MWidget *widget);
	MWidget *_focus;

	MList<MIoDevice> _iodevices;
};

#endif //MAPPLICATION_H
