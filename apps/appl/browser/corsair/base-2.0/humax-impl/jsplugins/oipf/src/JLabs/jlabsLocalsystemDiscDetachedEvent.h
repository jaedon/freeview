/*
 * jlabsLocalsystemDiscDetachedEvent.h
 *
 *  Created on: 2012. 07. 27.
 *      Author: jykim2
 */

#ifdef OIPF
#ifndef __JLABS_DISC_DETACHED_EVENT
#define __JLABS_DISC_DETACHED_EVENT

#include "Event.h"

namespace Ooif
{
	class JlabsDiscDetachedEvent : public Event
	{
	private:
		char *discId;
		char *discName;

	public:
		JlabsDiscDetachedEvent(jsplugin_obj *context, const char *_discId, const char *_discName);
		~JlabsDiscDetachedEvent();

		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);

		virtual int serialize(jsplugin_value **values);
	};
} /* namespace Ooif */
#endif // __JLABS_DISC_DETACHED_EVENT
#endif // OIPF

