#ifndef ___AS_APK_FILEMGR_STORAGE_H___
#define ___AS_APK_FILEMGR_STORAGE_H___

#include <as_apk_filemgr_storage_adaptor.h>

class cApkFilemgrStorage
  : public Octopus::Appkit::Filemgr::Storage_adaptor,
  public DBus::IntrospectableAdaptor,
  public DBus::ObjectAdaptor
{
public:
	static char const * const BUSNAME_APK_FILEMGR_STORAGE;
	static char const * const BUSPATH_APK_FILEMGR_STORAGE;

public:
	cApkFilemgrStorage(DBus::Connection &connection);

    void Format(const uint32_t& deviceId, const std::string& volume);
    void StartDST(const uint32_t& deviceId);
    void SetDefaultHdd(const std::string& storageId);
    void DisablePVRHdd(const std::string& storageId);
    void PairingHdd(const std::string& storageId);
    void UnPairingHdd(const std::string& storageId);
    void SetName(const std::string& storageId, const std::string& name);
    void SafeRemove(const uint32_t& deviceId, const bool& forced);
    std::vector< ::DBus::Struct< uint32_t > > GetList(const uint32_t& filter);
    ::DBus::Struct< uint64_t, uint64_t, uint64_t, uint64_t, uint64_t > GetSize(const uint32_t& deviceId);
    uint64_t GetPvrReservedSize(const uint32_t& deviceId);
    void CheckUsbSpeed(const std::string& storageId);
    std::string GetDevTypeString(const uint32_t& deviceType);
    std::string GetFsString(const uint32_t& fsType);

private:
		static void callback_event (DxStorage_NotiType_e eType, HINT32	p1, HINT32 p2, HINT32 p3);
	
	/*
	 * APK_FILEMGR_STORAGE의 Event Callback Interface는 userdata를 전달 할 수 없다. 그래서 dbus의 event를 
	   호출 할 수 없어 내장 싱글톤 포인터로 접근한다.
	 */
	static cApkFilemgrStorage*	pMainThis;
};

#endif
