#ifndef __JLABSRECORDDESTINATION__
#define __JLABSRECORDDESTINATION__

#include "jsplugin.h"
#include "ObjectInstance.h"
#include "EventTarget.h"
#include "WrapperObject.h"

#include "NativeJlabsRecordDestination.h"

namespace Ooif
{

	class jlabsRecordDestination : public ObjectInstance, public WrapperObject<jlabsRecordDestination>
	{
		public :
			jlabsRecordDestination( jlabsRecordDestinationHandle id );
			jlabsRecordDestination(const jlabsRecordDestination &original );
			virtual ~jlabsRecordDestination();
			int getter( jsplugin_obj *obj, const char *name, jsplugin_value *result );
			int setter( jsplugin_obj *obj, const char *name, jsplugin_value *value );

			static void releaseNative( jlabsRecordDestinationHandle id);
			static const ClassType class_name = CLASS_JLABS_RECORDDESTINATION;
	};

} /* namespace Ooif */

#endif // __JLABSRECORDDESTINATION__ 
