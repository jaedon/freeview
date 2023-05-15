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
	 * APK_FILEMGR_SAMBACLIENT�� Event Callback Interface�� userdata�� ���� �� �� ����. �׷��� dbus�� event�� 
	   ȣ�� �� �� ���� ���� �̱��� �����ͷ� �����Ѵ�.
	 */
	static cApkFilemgrSambaserver*	pMainThis;


};

#endif
