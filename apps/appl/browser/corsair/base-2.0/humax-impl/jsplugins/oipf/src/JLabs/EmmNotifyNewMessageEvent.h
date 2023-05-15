// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2011 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#ifndef __EMM_NOTIFY_NEW_MESSAGE_EVENT
#define __EMM_NOTIFY_NEW_MESSAGE_EVENT

#include "Event.h"
#include "jlabsEmmMessage.h"

namespace Ooif
{
	class EmmNotifyNewMessageEvent : public Event
	{
	private:
		int type;
		int msg_id;
		jlabsEmmMessage *new_msg;
		
	public:
		EmmNotifyNewMessageEvent(jsplugin_obj *c, int _type, int _msg_id, jlabsEmmMessageHandle id);
		~EmmNotifyNewMessageEvent();

		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
		virtual int serialize(jsplugin_value **argv);
	};
} /* namespace Ooif */

#endif // __EMM_NOTIFY_NEW_MESSAGE_EVENT