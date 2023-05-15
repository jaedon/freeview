// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2011 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#ifndef __EMM_AUTO_DISPLAY_MESSAGE_NOTIFY_EVENT
#define __EMM_AUTO_DISPLAY_MESSAGE_NOTIFY_EVENT

#include "Event.h"
#include "jlabsEmmAutoDisplayMessage.h"

namespace Ooif
{
	class EmmAutoDisplayMessageNotifyEvent : public Event
	{
	private:
		jlabsEmmAutoDisplayMessage *adm_info;
		
	public:
		EmmAutoDisplayMessageNotifyEvent(jsplugin_obj *c, jlabsEmmAutoDisplayMessageHandle id);
		~EmmAutoDisplayMessageNotifyEvent();

		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
		virtual int serialize(jsplugin_value **argv);
	};
} /* namespace Ooif */

#endif // __EMM_AUTO_DISPLAY_MESSAGE_NOTIFY_EVENT