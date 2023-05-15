#ifndef __JLABSCDSRECORDING__
#define __JLABSCDSRECORDING__

#include "jsplugin.h"
#include "ObjectInstance.h"
#include "EventTarget.h"
#include "WrapperObject.h"
#include "NativeJlabsCDSRecording.h"
#include "ParentalRatingCollection.h"

namespace Ooif
{

	class jlabsCDSRecording : public ObjectInstance, public WrapperObject<jlabsCDSRecording>
	{
		private :
			ParentalRatingCollection *getParentalRatings(int count, ParentalRatingHandle *handles);

		public :
			jlabsCDSRecording( jlabsCDSRecordingHandle id, jlabsCDSMetaHandle metaId );
			jlabsCDSRecording(const jlabsCDSRecording &original );
			virtual ~jlabsCDSRecording();
			int getter( jsplugin_obj *obj, const char *name, jsplugin_value *result );
			int setter( jsplugin_obj *obj, const char *name, jsplugin_value *value );

			jlabsCDSMetaHandle m_metaDataHandle;
			static void releaseNative( jlabsCDSRecordingHandle id);
			static const ClassType class_name = CLASS_JLABS_CDSRECORDING;
	};

} /* namespace Ooif */

#endif // __JLABSCDSRECORDING__
