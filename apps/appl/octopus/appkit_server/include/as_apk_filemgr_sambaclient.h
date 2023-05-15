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
	 * APK_FILEMGR_SAMBACLIENT�� Event Callback Interface�� userdata�� ���� �� �� ����. �׷��� dbus�� event�� 
	   ȣ�� �� �� ���� ���� �̱��� �����ͷ� �����Ѵ�.
	 */
	static cApkFilemgrSambaclient*	pMainThis;

};

#endif
