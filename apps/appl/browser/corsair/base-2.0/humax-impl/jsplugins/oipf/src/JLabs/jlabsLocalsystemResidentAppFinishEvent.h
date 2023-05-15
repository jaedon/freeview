/*
 * jlabsLocalsystemResidentAppFinishEvent.h
 *
 *  Created on: 2012. 10. 17.
 *      Author: choism
 */

#ifdef OIPF
#ifndef __JLABS_RESIDENT_APP_FINISH_EVENT
#define __JLABS_RESIDENT_APP_FINISH_EVENT

#include "Event.h"

namespace Ooif
{
	class JlabsResidentAppFinishEvent : public Event
	{
	private:
		char *appName;

	public:
		JlabsResidentAppFinishEvent(jsplugin_obj *context, char *appName);
		~JlabsResidentAppFinishEvent();

		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);

		virtual int serialize(jsplugin_value **values);
	};
} /* namespace Ooif */
#endif // __JLABS_RESIDENT_APP_FINISH_EVENT
#endif // OIPF
