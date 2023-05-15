#ifndef ___AS_APK_FILEMGR_SAMBASERVER_H___
#define ___AS_APK_FILEMGR_SAMBASERVER_H___

#include <as_apk_filemgr_sambaserver_adaptor.h>

class cApkFilemgrSambaserver
  : public Octopus::Appkit::Filemgr::Sambaserver_adaptor,
  public DBus::IntrospectableAdaptor,
  public DBus::ObjectAdaptor
{
public:
	static char const * const BUSNAME_APK_FILEMGR_SAMBASERVER;
	static char const * const BUSPATH_APK_FILEMGR_SAMBASERVER;

public:
	cApkFilemgrSambaserver(DBus::Connection &connection);

    void Start();
    void Stop();
    void SetConfig(const ::DBus::Struct< uint32_t >& configInfo);
    ::DBus::Struct< uint32_t > GetServerInfo();
    void SetPassword(const std::string& userName, const std::string& currentPassword, const std::string& newPassword);
    bool CheckPassword(const std::string& userName, const std::string& password);
    void SetPermission(const std::string& userName, const ::DBus::Struct< uint32_t >& sharedInfo);
    std::vector< ::DBus::Struct< uint32_t > > GetSharedList(const std::string& userName);
    void SetPublicAccess(const std::string& userName, const bool& access);
    bool GetPublicAccess(const std::string& userName);

private:
	static void callback_Event(HINT32 eEvent, HINT32 result, void *pvUser);

	/*
	 * APK_FILEMGR_SAMBACLIENT의 Event Callback Interface는 userdata를 전달 할 수 없다. 그래서 dbus의 event를 
	   호출 할 수 없어 내장 싱글톤 포인터로 접근한다.
	 */
	static cApkFilemgrSambaserver*	pMainThis;


};

#endif
