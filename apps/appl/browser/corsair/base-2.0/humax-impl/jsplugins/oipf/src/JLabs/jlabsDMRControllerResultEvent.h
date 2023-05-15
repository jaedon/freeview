#if defined OIPF && defined JLABS 

#ifndef __JLABS_DMR__
#define __JLABS_DMR__

#include "jsplugin.h"
#include "ObjectInstance.h"
#include "EventTarget.h"
#include "WrapperObject.h"

#include "NativeJlabsCDSRecording.h"
#include "NativeJlabsDMRController.h"

namespace Ooif
{
	class jlabsDMRControllerResultEvent: public Event
	{

		private :
			void    *m_handleId;
			int     m_nResult;
			int     m_nEvent;

		public :
			jlabsDMRControllerResultEvent(jsplugin_obj *c, const char *eventName, int state, int result, void *handleId);
			virtual ~jlabsDMRControllerResultEvent();

			int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
			int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
			virtual int serialize(jsplugin_value **argv);
	};

} /* namespace Ooif */

#endif
#endif // OIPF
