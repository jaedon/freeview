#ifndef ___AS_APK_META_ANTENNA_H___
#define ___AS_APK_META_ANTENNA_H___

#include <as_apk_meta_antenna_adaptor.h>

class cApkMetaAntenna
  : public Octopus::Appkit::Meta::Antenna_adaptor,
  public DBus::IntrospectableAdaptor,
  public DBus::ObjectAdaptor
{
public:
	static char const * const BUSNAME_APK_META_ANTENNA;
	static char const * const BUSPATH_APK_META_ANTENNA;

public:
	cApkMetaAntenna(DBus::Connection &connection);

    int32_t Add(const DbtAntInfo_t& antInfo);
    void RemoveAll();
    uint32_t Count();
    int32_t GetUidAt(const int32_t& index);
    DbtAntInfo_t GetInfo(const int32_t& uid);
    void SetInfo(const int32_t& uid, const DbtAntInfo_t& antInfo);
    void Load();
    void Save();

private:


};

#endif
