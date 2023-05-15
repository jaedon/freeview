/*
 * jlabsLocalsystemSetupInfoInsertFinishedEvent.h
 *
 *  Created on: 2013. 01. 24.
 *      Author: shlee3
 */

#ifdef OIPF
#ifndef __JLABS_SETUP_INFO_INSERT_FINISHED_EVENT
#define __JLABS_SETUP_INFO_INSERT_FINISHED_EVENT

#include "Event.h"

namespace Ooif
{
	class jlabsSetupInfoInsertFinishedEvent : public Event
	{
	private:
		OOIFNumber error_Number;

	public:
		jlabsSetupInfoInsertFinishedEvent(jsplugin_obj *context, OOIFNumber error);
		~jlabsSetupInfoInsertFinishedEvent();

		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);

		virtual int serialize(jsplugin_value **values);
		static const ClassType class_name = CLASS_JLABS_SETUPINFO_INSERT_FINISHED_EVENT;
	};
} /* namespace Ooif */
#endif // __JLABS_SETUP_INFO_INSERT_FINISHED_EVENT
#endif // OIPF

