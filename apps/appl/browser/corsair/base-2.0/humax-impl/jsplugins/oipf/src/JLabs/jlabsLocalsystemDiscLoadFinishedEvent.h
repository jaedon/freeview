/*
 * jlabsLocalsystemDiscLoadFinishedEvent.h
 *
 *  Created on: 2012. 07. 27.
 *      Author: jykim2
 */

#ifdef OIPF
#ifndef __JLABS_DISC_LOAD_FINISHED_EVENT
#define __JLABS_DISC_LOAD_FINISHED_EVENT

#include "Event.h"
#include "HDiscInfo.h"

namespace Ooif
{
	class JlabsDiscLoadFinishedEvent : public Event
	{
	private:
		HDiscInfo *disc;

	public:
		JlabsDiscLoadFinishedEvent(jsplugin_obj *context, HDiscInfoHandle handle);
		~JlabsDiscLoadFinishedEvent();

		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);

		virtual int serialize(jsplugin_value **values);
	};
} /* namespace Ooif */
#endif // __JLABS_DISC_LOAD_FINISHED_EVENT
#endif // OIPF
