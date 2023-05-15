/*
 * BmlVodEvent.h
 *
 */

#ifdef OIPF
#ifndef __BML_VOD_EVENT
#define __BML_VOD_EVENT

#include "Event.h"

namespace Ooif
{
	class BmlVodEvent : public Event
	{
	private:
		char *appName;
		char *param;

	public:
		BmlVodEvent(jsplugin_obj *context, const char *_appName, const char *_param);
		~BmlVodEvent();

		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);

		virtual int serialize(jsplugin_value **values);
	};
} /* namespace Ooif */
#endif // __BML_VOD_EVENT
#endif // OIPF

