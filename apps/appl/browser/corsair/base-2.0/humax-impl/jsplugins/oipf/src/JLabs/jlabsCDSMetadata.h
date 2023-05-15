#ifndef __JLABSCDSMETADATA__
#define __JLABSCDSMETADATA__

#include "jsplugin.h"
#include "ObjectInstance.h"
#include "EventTarget.h"
#include "WrapperObject.h"
#include "NativeJlabsCDSMetadata.h"

namespace Ooif
{

	class jlabsCDSMetaData : public ObjectInstance, public WrapperObject<jlabsCDSMetaData>
	{
		public :
			jlabsCDSMetaData( jlabsCDSMetaHandle id );
			virtual ~jlabsCDSMetaData();
			int getter( jsplugin_obj *obj, const char *name, jsplugin_value *result );
			int setter( jsplugin_obj *obj, const char *name, jsplugin_value *value );
			static void releaseNative( jlabsCDSMetaHandle id);
			static const ClassType class_name = CLASS_JLABS_CDSMETADATA;
	};

} /* namespace Ooif */

#endif // __JLABSCDSMETADATA__
