#ifndef ___AS_APK_MODULE_SUBMODULE_H___
#define ___AS_APK_MODULE_SUBMODULE_H___

#include <as_apk_module_submodule_adaptor.h>

class cApkModuleSubmodule
  : public Octopus::Appkit::Module::Submodule_adaptor,
  public DBus::IntrospectableAdaptor,
  public DBus::ObjectAdaptor
{
public:
	static char const * const BUSNAME_APK_MODULE_SUBMODULE;
	static char const * const BUSPATH_APK_MODULE_SUBMODULE;

public:
	cApkModuleSubmodule(DBus::Connection &connection);

	int32_t Foo(const uint32_t& in_param);

private:


};

#endif
