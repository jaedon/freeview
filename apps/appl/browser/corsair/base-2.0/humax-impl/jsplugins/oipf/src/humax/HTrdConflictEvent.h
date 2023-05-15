// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2011 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */
#ifdef OIPF
#ifndef __TRD_CONFLICT_NOTIFIER_EVENT
#define __TRD_CONFLICT_NOTIFIER_EVENT

#include "Event.h"
#include "HTrdConflict.h"

namespace Ooif
{
	class HTrdConflictEvent : public Event
	{
	private:
		
		humax::TrdConflictEventType_e 	m_eventType;
		int						m_count;
		//humax::TrdConflictList_t *trdconflictlist;
		char *m_trdconflictstr;
		
	public:
#if 0		
		HTrdConflictEvent(jsplugin_obj *context, humax::TrdConflictEventType_e _eventType,
							int _numOfList, humax::TrdConflictList_t *_trdconflictList);
#else
		HTrdConflictEvent(jsplugin_obj *context, humax::TrdConflictEventType_e _eventType, int count,
							char *_trdconflictStr);
#endif
		~HTrdConflictEvent();

		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);

		virtual int serialize(jsplugin_value **values);
	};
} /* namespace Ooif */
#endif // __TRD_CONFLICT_NOTIFIER_EVENT
#endif // OIPF



