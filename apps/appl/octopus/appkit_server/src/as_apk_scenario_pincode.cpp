#include <apk.h>


#include <vkernel.h>

#include <dbus-c++/dbus.h>
#include <as_apk_scenario_pincode.h>

char const * const cApkScenarioPincode::BUSNAME_APK_SCENARIO_PINCODE = "Octopus.Appkit.Scenario.Pincode";
char const * const cApkScenarioPincode::BUSPATH_APK_SCENARIO_PINCODE = "/Octopus/Appkit/Scenario/Pincode";

cApkScenarioPincode::cApkScenarioPincode(DBus::Connection &connection)
  : DBus::ObjectAdaptor(connection, BUSPATH_APK_SCENARIO_PINCODE)
{
	/* apkit scenario pincode */
    {	
		HERROR hErr = ERR_FAIL;
		hErr = APK_SCENARIO_PINCODE_Init ();
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("APK_SCENARIO_PINCODE_Init Error : %d", hErr);
		}
	}
}

bool cApkScenarioPincode::Check(const std::string& pinString)
{
	return (bool)APK_SCENARIO_PINCODE_Check ((const HCHAR *)&pinString[0]);
}

bool cApkScenarioPincode::Verify(const std::string& pinString)
{
	return (bool)APK_SCENARIO_PINCODE_Verify ((const HCHAR *)&pinString[0]);
}

bool cApkScenarioPincode::Set(const std::string& oldPinString, const std::string& newPinString)
{
	return (bool)APK_SCENARIO_PINCODE_Set ((const HCHAR *)&oldPinString[0], (const HCHAR *)&newPinString[0]);
}

bool cApkScenarioPincode::Reset(const std::string& newPinString)
{
	return (bool)APK_SCENARIO_PINCODE_Reset ((const HCHAR *)&newPinString[0]);
}

bool cApkScenarioPincode::SetTemporaryBlock(const std::string& pinString, const bool& blocked)
{
	return (bool)APK_SCENARIO_PINCODE_SetTemporaryBlock ((const HCHAR *)&pinString[0], (HBOOL)blocked);
}

bool cApkScenarioPincode::GetTemporaryBlock()
{
	return (bool)APK_SCENARIO_PINCODE_GetTemporaryBlock ();
}
