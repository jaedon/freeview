// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2013 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifdef OOIF_ASYNC
#include "AsyncHandler.h"
#include "macros.h"
#include "jsplugin.h"
#include <stdlib.h>
#include <stdio.h>

jsplugin_obj *AsyncHandler::currentToken = NULL;
jsplugin_cb_resume *AsyncHandler::resume_cb;
std::set<jsplugin_obj*> AsyncHandler::activeTokens;


void AsyncHandler::setToken(jsplugin_obj *t)
{
	currentToken = t;
}

ResumeToken AsyncHandler::getResumeToken()
{
	return currentToken;
}

void AsyncHandler::setResume(jsplugin_cb_resume *r)
{
	resume_cb = r;
}

void AsyncHandler::resume(ResumeToken t)
{
	activeTokens.erase((jsplugin_obj*)t);
	int r = (*resume_cb)((jsplugin_obj*)t);
	if (r != JSP_CALLBACK_OK)
		OOIF_LOG_ERROR("Failed to resume call");
}

void AsyncHandler::trackToken(jsplugin_obj *t)
{
	activeTokens.insert(t);
}

void AsyncHandler::untrackToken(jsplugin_obj *t)
{
	size_t used = activeTokens.erase(t);
	if (used != 0)
	{
		NATIVE_WARN_NOK(native_AsyncRevokeToken(t));
	}
}
#endif // OOIF_ASYNC
