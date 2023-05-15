#ifdef OIPF

#include "jsplugin.h"
#include "ObjectInstance.h"
#include "EventTarget.h"
#include "WrapperObject.h"
#include "NativeJlabsDMS.h"

namespace Ooif
{

	class jlabsCDSSearcheResultEvent : public Event
	{
		private :
			void    *m_handleId;
			int     m_nState;
			int     m_nHandleCount;
		public :
			jlabsCDSSearcheResultEvent( jsplugin_obj *c, const char *eventName, int size, jlabsDMSDeviceHandle handleId );
			virtual ~jlabsCDSSearcheResultEvent();

			int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
			int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
			virtual int serialize(jsplugin_value **argv);
	};


} /* namespace Ooif */

#endif // OIPF
