#ifndef ___AS_APK_SCENARIO_PINCODE_H___
#define ___AS_APK_SCENARIO_PINCODE_H___

#include <as_apk_scenario_pincode_adaptor.h>

class cApkScenarioPincode
  : public Octopus::Appkit::Scenario::Pincode_adaptor,
  public DBus::IntrospectableAdaptor,
  public DBus::ObjectAdaptor
{
public:
	static char const * const BUSNAME_APK_SCENARIO_PINCODE;
	static char const * const BUSPATH_APK_SCENARIO_PINCODE;

public:
	cApkScenarioPincode(DBus::Connection &connection);

    bool Check(const std::string& pinString);
    bool Verify(const std::string& pinString);
    bool Set(const std::string& oldPinString, const std::string& newPinString);
    bool Reset(const std::string& newPinString);
    bool SetTemporaryBlock(const std::string& pinString, const bool& blocked);
    bool GetTemporaryBlock();

private:


};

#endif
