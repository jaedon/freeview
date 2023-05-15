#ifdef OIPF

#include "jsplugin.h"
#include "ObjectInstance.h"
#include "EventTarget.h"
#include "WrapperObject.h"

#include "jlabsRecordDestination.h"

#include "Event.h"

namespace Ooif
{
	class jlabsRecordDestinationResultEvent: public Event
	{
		private :
			jlabsRecordDestinationHandle *m_handleId;
			int     m_nState;
			int     m_nHandleCount;

		public :
			jlabsRecordDestinationResultEvent( jsplugin_obj *c, int state, int size, jlabsRecordDestinationHandle *handleId );
			virtual ~jlabsRecordDestinationResultEvent();

			int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
			int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
			virtual int serialize(jsplugin_value **argv);
	};


	class jlabsRecordDestinationInfoResultEvent: public Event
	{
		private :
			double 			m_nTotalSize;
			double 			m_nAvailSize;
			char 			*m_dst;

		public :
			jlabsRecordDestinationInfoResultEvent( jsplugin_obj *c, double total, double availabe, char *dst_id);
			virtual ~jlabsRecordDestinationInfoResultEvent();

			int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
			int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
			virtual int serialize(jsplugin_value **argv);
	};
} /* namespace Ooif */
#endif // OIPF
