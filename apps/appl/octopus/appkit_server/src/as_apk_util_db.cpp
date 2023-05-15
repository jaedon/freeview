#include <apk.h>


#include <vkernel.h>

#include <dbus-c++/dbus.h>
#include <as_apk_util_db.h>

char const * const cApkUtilDb::BUSNAME_APK_UTIL_DB = "Octopus.Appkit.Util.Db";
char const * const cApkUtilDb::BUSPATH_APK_UTIL_DB = "/Octopus/Appkit/Util/Db";


cApkUtilDb::cApkUtilDb(DBus::Connection &connection)
  : DBus::ObjectAdaptor(connection, BUSPATH_APK_UTIL_DB)
{
	/* apkit util db */
    {	
		HERROR hErr = ERR_FAIL;
		hErr = APK_DB_Init();
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("APK_DB_Init Error : %d", hErr);
		}
	}
}

void cApkUtilDb::Remove(const std::string& key)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_DB_Remove ((const HCHAR *)&key[0]);

}

void cApkUtilDb::SetInt(const std::string& key, const int32_t& value)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_DB_SetInt ((const HCHAR *)&key[0], (HINT32)value);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_DB_SetInt Error : %d", hErr);
	}

	return ;
}

int32_t cApkUtilDb::GetInt(const std::string& key)
{
	HERROR hErr = ERR_FAIL;
	HINT32 nValue = 0;
	
	hErr = APK_DB_GetInt ((const HCHAR *)&key[0], &nValue);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_DB_GetInt Error : %d", hErr);
	}

	return (int32_t)nValue;
}

void cApkUtilDb::SetStr(const std::string& key, const std::string& value)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_DB_SetStr ((const HCHAR *)&key[0], (const HCHAR *)&value[0]);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_DB_SetStr Error : %d", hErr);
	}

	return ;
}

std::string cApkUtilDb::GetStr(const std::string& key)
{
	HERROR hErr = ERR_FAIL;
	HCHAR szString[ 128 ] = { 0, };

	hErr = APK_DB_GetStr ((const HCHAR *)&key[0], szString, 128);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_DB_GetStr Error : %d", hErr);
	}

	return std::string(szString);
}
