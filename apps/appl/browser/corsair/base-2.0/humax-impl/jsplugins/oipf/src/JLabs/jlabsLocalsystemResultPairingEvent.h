/*
 * jlabsLocalsystemResultPairingEvent.h
 *
 *  Created on: 2012. 10. 17.
 *      Author: choism
 */

#ifdef OIPF
#ifndef __JLABS_RESULT_PAIRING_EVENT
#define __JLABS_RESULT_PAIRING_EVENT

#include "Event.h"

namespace Ooif
{
	class JlabsResultPairingEvent : public Event
	{
	private:
		OOIFNumber result;

	public:
		JlabsResultPairingEvent(jsplugin_obj *context, OOIFNumber result);
		~JlabsResultPairingEvent();

		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);

		virtual int serialize(jsplugin_value **values);
	};
} /* namespace Ooif */
#endif // __JLABS_RESULT_PAIRING_EVENT
#endif // OIPF
