#ifndef ___AS_APK_UTIL_DB_H___
#define ___AS_APK_UTIL_DB_H___

#include <as_apk_util_db_adaptor.h>

class cApkUtilDb
  : public Octopus::Appkit::Util::Db_adaptor,
  public DBus::IntrospectableAdaptor,
  public DBus::ObjectAdaptor
{
public:
	static char const * const BUSNAME_APK_UTIL_DB;
	static char const * const BUSPATH_APK_UTIL_DB;

public:
	cApkUtilDb(DBus::Connection &connection);

    void Remove(const std::string& key);
    void SetInt(const std::string& key, const int32_t& value);
    int32_t GetInt(const std::string& key);
    void SetStr(const std::string& key, const std::string& value);
    std::string GetStr(const std::string& key);

private:


};

#endif
