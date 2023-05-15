// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifdef OOIF_ASYNC
#ifndef ASYNC_H
#define ASYNC_H

#include "NativeAsync.h"
#include "jsplugin.h"
#include <set>

class AsyncHandler
{
private:
	/**
	  jsplugin's resume callback
	 */
	static jsplugin_cb_resume *resume_cb;

	/**
	  Token that can be used to suspend the current function
	 */
	static jsplugin_obj * currentToken;

	/**
	  Tokens for the pages that are currently suspended.
	 */
	static std::set<jsplugin_obj*> activeTokens;
public:
	// Functions that need to be called when making native functions
	// support asynchronous.
	/**
	  Call immediately before and after invoking a native function that
	  supports asynchronous operations, to set and reset the ResumeToken.

	  Before the native function invocation, the parameter should be an
	  object that belongs to the current page. For example, in a getter,
	  "obj" is appropriate.

	  After the native function invocation, the parameter should be NULL.
	 */
	static void setToken(jsplugin_obj *t);

	/**
	  Call if a native function invocation for which a ResumeToken was
	  prepared using setToken returns OOIF_RETURN_ASYNC.
	  The parameter should be the same the one passed to SetToken.
	 */
	static void trackToken(jsplugin_obj *t);

	// Functions used for internal house keeping. No need to call these
	// when making more native functions asynchronous.
	/**
	  Called on OOIF startup, to register the jsplugin resume function
	 */
	static void setResume(jsplugin_cb_resume * r);

	/**
	  Automatically called when javascript objects are unloaded. If the
	  integration layer used this object as a ResumeToken, it will be
	  informed that this token has become invalid.
	 */
	static void untrackToken(jsplugin_obj *t);

	// Used by the integration layer, registered on startup through
	// native_AsyncSetResumer and native_AsyncSetTokenGetter. See
	// NativeAsync.h for more info..
	static void resume(ResumeToken t);
	static ResumeToken getResumeToken();
};

#endif // ASYNC_H
#endif // OOIF_ASYNC
