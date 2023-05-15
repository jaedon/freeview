#ifndef __JLABSNETWORKINTERFACE__
#define __JLABSNETWORKINTERFACE__

#include "jsplugin.h"
#include "EventTarget.h"
#include "Collection.h"
#include "ObjectInstance.h"
#include "WrapperObject.h"
#include "NativeJlabsNetworkInterface.h"

namespace Ooif
{
	class jlabsNetworkInterface : public EventTarget, public WrapperObject<jlabsNetworkInterface>
	{
		private:
			static list<jlabsNetworkInterface *> instances;
			static jlabsNetworkInterface * GetInstanceById( jlabsNetworkInterfaceHandle id );
			static jlabsNetworkInterfaceHandle m_sApplyIdentifier;

		protected:
			virtual OOIFReturnCode checkListenerAccessAllowed(const char *eventName);

		public:
			jlabsNetworkInterface(jlabsNetworkInterfaceHandle i);
			jlabsNetworkInterface(const jlabsNetworkInterface &original);
			virtual ~jlabsNetworkInterface();

			int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
			int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);

			static int get(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
			static int set(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
			static int restart(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
			static int isDown(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
			static int isUp(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);

			static void releaseNative(jlabsNetworkInterfaceHandle id);
			static void callbackEventHandler( int state, jlabsNetworkInterfaceHandle id, int nDevIndex );
			static const ClassType class_name = CLASS_JLABS_NETWORK_INTERFACE;
	};
} /* namespace Ooif */

#endif // __JLABSNETWORKINTERFACE__
