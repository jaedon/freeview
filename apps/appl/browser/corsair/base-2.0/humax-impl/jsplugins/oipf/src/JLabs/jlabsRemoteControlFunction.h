#ifndef __JLABSNETWORKSERVICE__
#define __JLABSNETWORKSERVICE__

#include "jsplugin.h"
#include "Collection.h"
#include "ObjectInstance.h"
#include "WrapperObject.h"
#include "NativeJlabsRemoteControlFunction.h"


namespace Ooif
{
	class jlabsRemoteControlFunction : public EventTarget, public WrapperObject<jlabsRemoteControlFunction>
	{
	private:
		static list<jlabsRemoteControlFunction*> instances;

	public:
		jlabsRemoteControlFunction(jlabsRemoteControlFunctionHandle i);
		virtual ~jlabsRemoteControlFunction();
		
		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
		// functions
		static int useServerSideXMLUIListing(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int sendRemoteMessage(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int sendMulticastNotif(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int sendInternalServerError(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int dropConnection(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);

		static void releaseNative(jlabsRemoteControlFunctionHandle id);

		static void EventHandler( int nServiceIndex, int nEvent, int nParam1, int nParam2 );
		static const ClassType class_name = CLASS_JLABS_REMOTECONTROL_FUNCTION;
	};
} /* namespace Ooif */

#endif // __JLABSNETWORKSERVICE__
