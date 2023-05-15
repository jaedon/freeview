// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifndef __VISUAL_OBJECT
#define __VISUAL_OBJECT

#include "EventTarget.h"

namespace Ooif
{
/**
   Super class for objects implementing a visual JS object.
   Note that sup classes should also implement all TV visualization
   callback functions in #jsplugin_cb_add_tv_visual.
*/
class VisualObject: public EventTarget
{
private:
	/** Window position and size */
	int x_pos, y_pos, width, height;
	/** Window visible */
	int is_visible;
	/** saves the state the object was in before switching to fullscreen*/
	jsplugin_obj *saved_state;

protected:
	enum WindowState
	{
	    FULLSCREEN,
	    WINDOWED
	} requested_state;
private:

	/** Called when starting a transaction to (or out of)
		fullscreen, to change the state of the object (as queried by
		isFullScreen()). Calling rollbackWindowStateTransaction()
		will cancel this, and revert to the previous state. */
	void startWindowStateTransaction(WindowState requested_state);

	/** Called when an in-progress transaction to (or out of) fullscreen
		fails, to restore the state as it was before the transaction
		was attempted. Call before saveState or popState.*/
	void rollbackWindowStateTransaction();

	/** Stores and protects against garbage collection the state of
		the object before switching to fullscreen.
		If a state has previously been stored, futher calls to this
		function will not overwrite it. This concludes a
		transaction started with startWindowStateTransaction */
	void completeFullScreenTransaction(jsplugin_obj *state);

	/** Forgets the saved state of the object as it was before
		switching to fullscreen, if it has previously been stored,
		remove the garbage collection protection.
		This concludes a transaction started with
		startWindowStateTransaction */
	void completeWindowedTransaction();

	/** Callback called when the ooif_helper.fullscreen function
		has been found.
		Triggered from visualFullScreen(true), do not call directly. */
	static void foundFullscreenHelper(int status, jsplugin_value *return_value, void *user_data);

	/** Callback called when ooif_helper.fullscreen has completed,
		to store its result.
		Triggered indirectly from visualFullScreen(true), do
		not call directly. */
	static void fullscreenHelperResult(int status, jsplugin_value *return_value, void *user_data);

	/** Callback called when the ooif_helper.restore function
		has been found.
		Triggered from visualFullScreen(false), do not call directly. */
	static void foundRestoreHelper(int status, jsplugin_value *return_value, void *user_data);

protected:
	/** Resize the visual window. Returns false if it did not need
		to be resized (if neither position nor size was
		changed.)
	*/
	bool resize(int x, int y, int w, int h);

	/** Toggle window visibility. Returns false if the value was
		not changed.*/
	bool toggleVisibility(int visibility);

	/** Returns true if the object is currently displaying in
		fullscreen, false otherwise */
	bool isFullScreen();

	virtual OOIFReturnCode updateNativeWindowState(WindowState requested_state) = 0;

public:
	VisualObject() : x_pos(-1), y_pos(-1), width(-1), height(-1),
		is_visible(-1), saved_state(NULL), requested_state(WINDOWED) {}
	virtual ~VisualObject();

	virtual void videoVisible(int visibility, int unloading) = 0;
	virtual void videoPosition(int x, int y, int w, int h) = 0;

	static int setFullScreen(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
};

} /* namespace Ooif */

#endif // __VISUAL_OBJECT
