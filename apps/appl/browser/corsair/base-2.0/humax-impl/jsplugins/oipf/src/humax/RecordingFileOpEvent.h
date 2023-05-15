/*
 * RecordingFileOpEvent.h
 */

#ifndef RECORDINGFILEOPEVENT_H_
#define RECORDINGFILEOPEVENT_H_

#ifdef HMX_WEBUI

#include "Event.h"

namespace Ooif {
	class RecordingFileOpEvent : public Event {
	public:
		RecordingFileOpEvent(int _ticket, int _state, jsplugin_obj *c);
		virtual ~RecordingFileOpEvent();
	private: // from Event
		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		virtual int serialize(jsplugin_value** argv);
		int ticket;
		int state;
	};
} /* namespace Ooif */

#endif /* HMX_WEBUI */

#endif /* RECORDINGFILEOPEVENT_H_ */

/* EOF */

