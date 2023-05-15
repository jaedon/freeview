#ifndef ___AS_APK_FILEMGR_GROUP_H___
#define ___AS_APK_FILEMGR_GROUP_H___

#include <as_apk_filemgr_group_adaptor.h>

class cApkFilemgrGroup
  : public Octopus::Appkit::Filemgr::Group_adaptor,
  public DBus::IntrospectableAdaptor,
  public DBus::ObjectAdaptor
{
public:
	static char const * const BUSNAME_APK_FILEMGR_GROUP;
	static char const * const BUSPATH_APK_FILEMGR_GROUP;

public:
	cApkFilemgrGroup(DBus::Connection &connection);

    uint32_t Create(const std::string& extFilter);
    void Release(const uint32_t& handle);
    void AddPath(const uint32_t& handle, const std::string& path);
    void RemovePath(const uint32_t& handle, const std::string& path);
    void ExcludePath(const uint32_t& handle, const std::string& path);
    uint32_t Copy(const uint32_t& handle, const std::string& path);
    uint32_t Move(const uint32_t& handle, const std::string& path);
    uint32_t Remove(const uint32_t& handle);
    uint32_t Cancel(const uint32_t& handle);

private:


};

#endif
