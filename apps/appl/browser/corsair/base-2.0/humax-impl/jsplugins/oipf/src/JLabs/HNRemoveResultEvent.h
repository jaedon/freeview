
#ifndef __HN_REMOVE_RESULT_EVENT
#define __HN_REMOVE_RESULT_EVENT
#ifdef JLABS

#include "Event.h"
#include "NativeRecording.h"
#include "Recording.h"

namespace Ooif
{
	class HNRemoveResult : public Event
	{
	private:
		const char	*id;
		int 			errorCode;

	public:
		~HNRemoveResult();
		HNRemoveResult(const char *id, int errorCode, jsplugin_obj *c);

		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
		virtual int serialize(jsplugin_value **argv);
	};
} /* namespace Ooif */
#endif // JLABS
#endif // !__HN_REMOVE_RESULT_EVENT