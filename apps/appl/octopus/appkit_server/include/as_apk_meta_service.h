#ifndef ___AS_APK_META_SERVICE_H___
#define ___AS_APK_META_SERVICE_H___

#include <as_apk_meta_service_adaptor.h>

class cApkMetaService
  : public Octopus::Appkit::Meta::Service_adaptor,
  public DBus::IntrospectableAdaptor,
  public DBus::ObjectAdaptor
{
public:
	static char const * const BUSNAME_APK_META_SERVICE;
	static char const * const BUSPATH_APK_META_SERVICE;

public:
	cApkMetaService(DBus::Connection &connection);

    DbtService_t GetService(const int32_t& uid);
    DbtNetwork_t GetNetwork(const int32_t& uid);
    DbtTransponder_t GetTransponder(const int32_t& uid);
    DbtProvider_t GetProvider(const int32_t& uid);
    DbtGroup_t GetGroup(const int32_t& uid);
    DbtBouquet_t GetBouquet(const int32_t& uid);
    DbtChannelLogo_t GetLogoUrl(const int32_t& uid, const DbtChannelLogo_t& bufChannelLogoInfo);
    ::DBus::Struct< int32_t, int32_t, int32_t > GetServiceTriplet(const int32_t& uid);
    std::vector< DbtService_t > FindServiceByTriplet(const int32_t& onid, const int32_t& tsid, const int32_t& svcid);
    std::vector< DbtService_t > FindServiceByNumber(const int32_t& number);
    std::vector< DbtService_t > GetServiceList();
    std::vector< DbtGroup_t > GetGroupList();
    void Load();
    void Save();
    void SetService(const DbtService_t& service);
    void AddService(const DbtService_t& service);
    void RemoveService(const DbtService_t& service);
    void Reset();
    void LoadPreferredList();
    void LoadupdatedList();
    void ChangeUpdateFlag(const uint32_t& fromFlag, const uint32_t& toFlag);
    void RemoveServiceWithFlag(const uint32_t& flag);


private:
	static void callback_Event (HINT32 event, const HINT32 *args, void *userdata);
	static HBOOL callback_UpdateChecker (const HCHAR *typeName, const void *_old, const void *_new, HBOOL *updated);

/*
 * APK_META_SERVICE의 Event Callback Interface는 userdata를 전달 할 수 없다. 그래서 dbus의 event를 
   호출 할 수 없어 내장 싱글톤 포인터로 접근한다.
 */
 	static cApkMetaService*	pMainThis;

};

#endif
