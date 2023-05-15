/*
 * jlabsLocalsystemFinishDiscFormatEvent.h
 *
 *  Created on: 2012. 07. 27.
 *      Author: jykim2
 */

#ifdef JLABS
#ifndef __JLABS_FINISH_DISC_FORMAT_EVENT
#define __JLABS_FINISH_DISC_FORMAT_EVENT

#include "Event.h"
#include "HDiscInfo.h"

namespace Ooif
{
	class JlabsFinishDiscFormatEvent : public Event
	{
	private:
		int error;
		HDiscInfo *disc;
	public:
		JlabsFinishDiscFormatEvent(jsplugin_obj *context, HDiscInfoHandle handle, int state);
		~JlabsFinishDiscFormatEvent();

		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);

		virtual int serialize(jsplugin_value **values);
	};
} /* namespace Ooif */
#endif // __JLABS_FINISH_DISC_FORMAT_EVENT
#endif
