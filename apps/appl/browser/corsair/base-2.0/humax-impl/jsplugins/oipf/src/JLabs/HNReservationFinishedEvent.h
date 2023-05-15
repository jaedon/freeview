
#ifndef __HN_RESERVATION_FINISHED_EVENT
#define __HN_RESERVATION_FINISHED_EVENT
#ifdef JLABS

#include "Event.h"
#include "NativeRecording.h"
#include "Recording.h"

namespace Ooif
{
	class HNReservationFinished : public Event
	{
	private:
		ScheduledRecording recording;
		int errorCode;

	public:
		~HNReservationFinished();
		HNReservationFinished(ScheduledRecordingHandle recordingHandle, int errorCode, jsplugin_obj *c);

		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
		virtual int serialize(jsplugin_value **argv);
	};
} /* namespace Ooif */
#endif // JLABS
#endif // !__HN_RESERVATION_FINISHED_EVENT