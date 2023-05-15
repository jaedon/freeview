#ifndef ___AS_APK_FILEMGR_SAMBACLIENT_H___
#define ___AS_APK_FILEMGR_SAMBACLIENT_H___

#include <as_apk_filemgr_sambaclient_adaptor.h>

class cApkFilemgrSambaclient
  : public Octopus::Appkit::Filemgr::Sambaclient_adaptor,
  public DBus::IntrospectableAdaptor,
  public DBus::ObjectAdaptor
{
public:
	static char const * const BUSNAME_APK_FILEMGR_SAMBACLIENT;
	static char const * const BUSPATH_APK_FILEMGR_SAMBACLIENT;

public:
	cApkFilemgrSambaclient(DBus::Connection &connection);

    void StartScan();
    void StopScan();
    void Mount(const ::DBus::Struct< uint32_t >& sharedFolderInfo);
    void UnMount(const ::DBus::Struct< uint32_t >& sharedFolderInfo);
    void UnmountAll();

private:
	static void callback_Event(const HCHAR *pszEventName, void *pvData, void *pvUser);

	/*
	 * APK_FILEMGR_SAMBACLIENT의 Event Callback Interface는 userdata를 전달 할 수 없다. 그래서 dbus의 event를 
	   호출 할 수 없어 내장 싱글톤 포인터로 접근한다.
	 */
	static cApkFilemgrSambaclient*	pMainThis;

};

#endif
