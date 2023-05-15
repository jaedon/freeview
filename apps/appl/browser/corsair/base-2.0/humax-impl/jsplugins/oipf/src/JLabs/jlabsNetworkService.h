#ifndef __JLABSNETWORKSERVICE__
#define __JLABSNETWORKSERVICE__

#include "jsplugin.h"
#include "Collection.h"
#include "ObjectInstance.h"
#include "WrapperObject.h"
#include "NativeJlabsNetworkService.h"


namespace Ooif
{
	class jlabsNetworkService : public EventTarget, public WrapperObject<jlabsNetworkService>
	{
	private:
		static list<jlabsNetworkService*> instances;

	public:
		jlabsNetworkService(jlabsNetworkServiceHandle i);
		jlabsNetworkService(const jlabsNetworkService &original);
		virtual ~jlabsNetworkService();
		
		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
		// functions
		static int get(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int set(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int restart(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int stop(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);

		static void releaseNative(jlabsNetworkServiceHandle id);

		static void EventHandler( int nServiceIndex, int nEvent, int nParam1, int nParam2 );
		static const ClassType class_name = CLASS_JLABS_NETWORK_SERVICE;
	};
} /* namespace Ooif */

#endif // __JLABSNETWORKSERVICE__
