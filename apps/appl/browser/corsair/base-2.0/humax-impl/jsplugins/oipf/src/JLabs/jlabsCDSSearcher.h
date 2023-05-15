#if defined OIPF && defined JLABS

#ifndef __JLABSCDSSEARCHER__
#define __JLABSCDSSEARCHER__

#include "jsplugin.h"
#include "ObjectInstance.h"
#include "EventTarget.h"
#include "WrapperObject.h"

#include "NativeJlabsCDSSearcher.h"
#include "NativeJlabsDMS.h"

namespace Ooif
{

	class jlabsCDSSearcher : public EventTarget
	{
		private :
			static list<jlabsCDSSearcher *> instances;
			static void callbackEventHandler( int event, int size,  jlabsDMSDeviceHandle dmsList);

		public :
			jlabsCDSSearcher();
			virtual ~jlabsCDSSearcher();

			int getter( jsplugin_obj *obj, const char *name, jsplugin_value *result );
			int setter( jsplugin_obj *obj, const char *name, jsplugin_value *value );

			static int  CDSSearch(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);

		private :
			static jlabsCDSSearcher *m_sDMSSearcher;

		public :
			static const ClassType class_name = CLASS_JLABS_CDSSEARCHER;

	};

} /* namespace Ooif */

#endif // __JLABSCDSSEARCHER__
#endif
