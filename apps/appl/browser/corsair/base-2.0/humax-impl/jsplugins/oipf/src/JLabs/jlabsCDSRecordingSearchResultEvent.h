#ifdef OIPF

#include "jsplugin.h"
#include "ObjectInstance.h"
#include "EventTarget.h"
#include "WrapperObject.h"

#include "NativeJlabsCDSRecording.h"
#include "NativeJlabsCDSMetadata.h"

#include "jlabsCDSRecording.h"
#include "jlabsDMS.h"

namespace Ooif
{
	class jlabsCDSRecordingSearchResultEvent : public Event
	{
		private :

			jlabsCDSRecordingHandle 	*m_handleId;
			jlabsCDSRecordingCollection *m_collection;

			int     m_nState;
			int     m_nTotal_matches;
			int     m_nHandleCount;

			jlabsCDSRecordingCollection *getJlabsCDSRecordingCollection(int count, jlabsCDSRecordingHandle *handles );
			OOIFReturnCode native_jlabsLocalSystemGetNetworkInterfaces(int *count, jlabsCDSRecordingHandle **handles);

		public :
			jlabsCDSRecordingSearchResultEvent( jsplugin_obj *c, int state, int size, jlabsCDSRecordingHandle *handleId );
			virtual ~jlabsCDSRecordingSearchResultEvent();

			int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
			int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
			virtual int serialize(jsplugin_value **argv);
	};

} /* namespace Ooif */

#endif // OIPF
