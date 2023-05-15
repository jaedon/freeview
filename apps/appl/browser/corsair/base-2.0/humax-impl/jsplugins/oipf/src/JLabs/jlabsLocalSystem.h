
/* jLAB 확장 클래스 .*/

#ifdef OIPF
#ifndef __JLABS_LOCAL_SYSTEM
#define __JLABS_LOCAL_SYSTEM

#include "jsplugin.h"
#include "ObjectInstance.h"
#include "Collection.h"
#include "NativeConfiguration.h"
#include "NativeJlabsLocalSystem.h"
#include "jlabsNetworkInterface.h"
#include "jlabsNetworkService.h"
#include "jlabsPPVHistory.h"
#include "jlabsPPVHistoryCollection.h"
#include "jlabsScanning.h"
#include "jlabsSystemInfo.h"
#include "EventTarget.h"


namespace Ooif
{
	class jlabsLocalSystem : public EventTarget//ObjectInstance
	{
	private:
		static list<jlabsLocalSystem*> instances;
		// 아래 함수는 Getter 에서 사용할 서브 함수.
		jlabsNetworkInterfaceCollection	*getNetworkInterfaces(int count, jlabsNetworkInterfaceHandle *handles);
		jlabsNetworkServiceCollection	*getNetworkService(int count, jlabsNetworkServiceHandle *handles);
		jlabsWiFiAccessPointCollection	*getWiFiAccessPoint(int count, jlabsNetworkServiceHandle *handles);
		jlabsPPVHistoryCollection		*getPPVHistory(int count, jlabsPPVHistoryHandle *handles);
		jlabsScanningCollection		*getScanComponent(int count, jlabsScanningHandle *handles);

	protected:

	public:
		jlabsLocalSystem();
		~jlabsLocalSystem();

		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);

		//// Method
		static int openBrowser(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int ping(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int setFactoryReset(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int resetUserInfo(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);

		static HDiscInfoCollection	*getDiscs(int count, HDiscInfoHandle *handles);
		static void 	onJlabsLocalSystemEvent(JlabsLocalsystemEvent event, JlabsLocalsystemEventInfo* info);
		static int		formatDisc(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int		detachDisc(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int		startPairing(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int		abortPairing(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int		startResidentApp(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int		ResidentAppFinish(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int		insertSetupInfo(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int		loginApp(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
		static int		logoffApp(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);

		static const ClassType class_name = CLASS_JLABS_LOCAL_SYSTEM;
	};
} /* namespace Ooif */

#endif // __JLABS_LOCAL_SYSTEM
#endif // OIPF
