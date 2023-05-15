#if defined OIPF && defined JLABS

#ifndef __JLABSDMS__
#define __JLABSDMS__

#include "jsplugin.h"
#include "ObjectInstance.h"
#include "EventTarget.h"
#include "WrapperObject.h"
#include "NativeJlabsDMS.h"

namespace Ooif
{

	class jlabsDMS : public EventTarget, public WrapperObject<jlabsDMS>
	{
		private :
			static jlabsDMS *m_sDMSDevice;

		public :
			jlabsDMS( jlabsDMSDeviceHandle id );
			jlabsDMS( const jlabsDMS &original );
			virtual ~jlabsDMS();

			int getter( jsplugin_obj *obj, const char *name, jsplugin_value *result );
			int setter( jsplugin_obj *obj, const char *name, jsplugin_value *value );

			static void releaseNative( jlabsDMSDeviceHandle id);

			static int  addEventListener(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
			static int  browseDirectChildren(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
			static int  createContentSearch(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
			static int  getAvailableStorageSize(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
			static int  getRecordDestinations(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);

			static void callbackEventHandler( int event, int size, void **content );

		public :
			static const ClassType class_name = CLASS_JLABS_DMSDEVICE;
	};

} /* namespace Ooif */

#endif // __JLABSCDSSEARCHER__
#endif
