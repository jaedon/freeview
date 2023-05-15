#ifndef ___AS_APK_FILEMGR_FTPSERVER_H___
#define ___AS_APK_FILEMGR_FTPSERVER_H___

#include <as_apk_filemgr_ftpserver_adaptor.h>

class cApkFilemgrFtpserver
  : public Octopus::Appkit::Filemgr::Ftpserver_adaptor,
  public DBus::IntrospectableAdaptor,
  public DBus::ObjectAdaptor
{
public:
	static char const * const BUSNAME_APK_FILEMGR_FTPSERVER;
	static char const * const BUSPATH_APK_FILEMGR_FTPSERVER;

public:
	cApkFilemgrFtpserver(DBus::Connection &connection);

	int32_t Start();
	int32_t Stop();
	int32_t SetConfig(const ::DBus::Struct< std::string, int32_t >& configInfo);
	void GetServerInfo(int32_t& reval, ::DBus::Struct< int32_t, int32_t, std::string, int32_t >& ftpInfo);
	int32_t SetPassword(const std::string& userName, const std::string& currentPassword, const std::string& newPassword);
	void CheckPassword(const std::string& userName, const std::string& password, bool& isVaild, int32_t& reval);
	int32_t SetPermission(const std::string& userName, const ::DBus::Struct< std::string, std::string, int32_t, bool, bool >& sharedInfo);
	std::vector< ::DBus::Struct< std::string, std::string, int32_t, bool, bool > > GetSharedList(const std::string& userName);
	int32_t SetPublicAccess(const std::string& userName, const bool& access);
	bool GetPublicAccess(const std::string& userName);

private:
	static void	callback_Event(HINT32 eEvent, HINT32 nResult, void *pvUser);

	/*
	 * APK_FILEMGR_FTPSERVER의 Event Callback Interface는 userdata를 전달 할 수 없다. 그래서 dbus의 event를 
	   호출 할 수 없어 내장 싱글톤 포인터로 접근한다.
	 */
	static cApkFilemgrFtpserver*	pMainThis;


};

#endif
