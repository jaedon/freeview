// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#include "VisualObject.h"
#include "macros.h"

namespace Ooif
{
extern std::list<jsplugin_obj*> g_proteges;
}

using namespace Ooif;

VisualObject::~VisualObject()
{
	g_proteges.remove(getHost());
}

bool VisualObject::resize(int x, int y, int w, int h)
{
	bool resize = x_pos != x || y_pos != y || width != w || height != h;
	if (resize)
	{
		x_pos = x;
		y_pos = y;
		width = w;
		height = h;
	}
	return resize;
}

bool VisualObject::toggleVisibility(int visibility)
{
	if (visibility == is_visible)
		return false;

	is_visible = visibility;
	return true;
}

bool VisualObject::isFullScreen()
{
	return requested_state == FULLSCREEN;
}

void VisualObject::startWindowStateTransaction(WindowState requested_state)
{
	this->requested_state = requested_state;
	g_proteges.push_back(getHost());
}

void VisualObject::rollbackWindowStateTransaction()
{
	WindowState old_state = (saved_state == NULL) ? WINDOWED : FULLSCREEN;

	if (requested_state != old_state)
	{
		requested_state = (requested_state == FULLSCREEN) ? WINDOWED : FULLSCREEN;
		NATIVE_WARN_NOK(updateNativeWindowState(requested_state));
	}
	g_proteges.remove(getHost());
}

//static
void VisualObject::fullscreenHelperResult(int status, jsplugin_value *return_value, void *user_data)
{
	jsplugin_obj *this_obj = (jsplugin_obj*)user_data;
	VisualObject *instance = EXTRACT_OBJECT<VisualObject>(this_obj);

	if (status != JSP_CALLBACK_OK || return_value->type != JSP_TYPE_NATIVE_OBJECT || !instance)
	{
		OOIF_LOG_WARNING("Reached fullscreenHelperResult with incorrect parameters");
		if (instance) instance->rollbackWindowStateTransaction();
		return;
	}

	instance->completeFullScreenTransaction(return_value->u.object);;
}

void VisualObject::completeFullScreenTransaction(jsplugin_obj *state)
{
	g_proteges.remove(getHost());
	if (!saved_state)
	{
		addProtege(state);
		saved_state = state;
	}
}

//static
void VisualObject::foundFullscreenHelper(int status, jsplugin_value *return_value, void *user_data)
{
	jsplugin_obj *this_obj = (jsplugin_obj*)user_data;
	VisualObject *instance = EXTRACT_OBJECT<VisualObject>(this_obj);

	if (status != JSP_CALLBACK_OK || return_value->type != JSP_TYPE_NATIVE_OBJECT || !this_obj)
	{
		OOIF_LOG_WARNING("Reached findFullscreenHelper with incorrect parameters");
		if (instance) instance->rollbackWindowStateTransaction();
		return;
	}

	jsplugin_obj *fs_helper = return_value->u.object;

	jsplugin_value param;
	param.type = JSP_TYPE_OBJECT;
	param.u.object = this_obj;

	int ret = g_opera_callbacks->call_function(this_obj, NULL, fs_helper, 1, &param, this_obj, fullscreenHelperResult);
	if (ret != JSP_CALL_NO_VALUE)
	{
		OOIF_LOG_WARNING("Failed to call ooif_helper.fullscreen()");
	   	if (instance) instance->rollbackWindowStateTransaction();
	}
}

//static
void VisualObject::foundRestoreHelper(int status, jsplugin_value *return_value, void *user_data)
{
	jsplugin_obj *this_obj = (jsplugin_obj*)user_data;
	VisualObject *instance = EXTRACT_OBJECT<VisualObject>(this_obj);

	if (status != JSP_CALLBACK_OK || return_value->type != JSP_TYPE_NATIVE_OBJECT || !instance)
	{
		OOIF_LOG_WARNING("Reached findRestoreHelper with incorrect parameters");
		if (instance) instance->rollbackWindowStateTransaction();
		return;
	}

	jsplugin_obj *restore_helper = return_value->u.object;

	jsplugin_value param;
	param.type = JSP_TYPE_NATIVE_OBJECT;
	param.u.object = instance->saved_state;

	if (param.u.object)
	{
		int ret = g_opera_callbacks->call_function(this_obj, NULL, restore_helper, 1, &param, NULL, NULL);
		if (ret != JSP_CALL_NO_VALUE)
		{
			OOIF_LOG_WARNING("Failed to call ooif_helper.restore()");
			instance->rollbackWindowStateTransaction();
			return;
		}
		instance->completeWindowedTransaction();
	}
}

void VisualObject::completeWindowedTransaction()
{
	g_proteges.remove(getHost());
	if (saved_state)
	{
		removeProtege(saved_state);
		saved_state = NULL;
	}
}

//static
int VisualObject::setFullScreen(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	CHECK_ARGUMENT_COUNT(1);
	VisualObject *instance = EXTRACT_OBJECT_OR_THROW(instance, this_obj, VisualObject);
	WindowState requested_state = BOOLEAN_VALUE(argv[0]) ? FULLSCREEN : WINDOWED;

	bool request_changed = requested_state != instance->requested_state;

	if (request_changed)
	{
		NATIVE_CALL(instance->updateNativeWindowState(requested_state));
	}
	else if (requested_state == WINDOWED)
	{
		return JSP_CALL_NO_VALUE;
	}
	// in the FULLSCREEN -> FULLSCREEN case, we need to keep going, as we want the
	// video to be resized to cover the full screen again, in case it had
	// somehow been resized.

	// Initiating the transaction after the native call, so that the
	// following code isn't run if the integration layer returns an error.
	instance->startWindowStateTransaction(requested_state);

	int ret;
	if (requested_state == FULLSCREEN)
		ret = g_opera_callbacks->eval(this_obj, "ooif_helper.fullscreen", this_obj, foundFullscreenHelper );
	else
		ret = g_opera_callbacks->eval(this_obj, "ooif_helper.restore", this_obj, foundRestoreHelper );

	if (ret != JSP_CALLBACK_OK)
	{
		OOIF_LOG_WARNING("Failed to eval ooif_helper function");
		instance->rollbackWindowStateTransaction();
	}

	return JSP_CALL_NO_VALUE;
}

