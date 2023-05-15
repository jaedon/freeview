#include <apk.h>


#include <vkernel.h>

#include <dbus-c++/dbus.h>
#include <as_apk_module_submodule.h>

char const * const cApkModuleSubmodule::BUSNAME_APK_MODULE_SUBMODULE = "Octopus.Appkit.Module.Submodule";
char const * const cApkModuleSubmodule::BUSPATH_APK_MODULE_SUBMODULE = "/Octopus/Appkit/Module/Submodule";


cApkModuleSubmodule::cApkModuleSubmodule(DBus::Connection &connection)
  : DBus::ObjectAdaptor(connection, BUSPATH_APK_MODULE_SUBMODULE)
{
	/* apkit module submodule */
    {	
		//APK_MODULE_SUBMODULE_Init();
    }
}

int32_t cApkModuleSubmodule::Foo(const uint32_t& in_param)
{
	printf ("TRACE = MODULE:SUBMODULE:Foo -> %d", in_param);

	return 999;
}
