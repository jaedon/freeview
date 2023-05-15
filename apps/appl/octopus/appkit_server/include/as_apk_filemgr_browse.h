#ifndef ___AS_APK_FILEMGR_BROWSE_H___
#define ___AS_APK_FILEMGR_BROWSE_H___

#include <as_apk_filemgr_browse_adaptor.h>

class cApkFilemgrBrowse
  : public Octopus::Appkit::Filemgr::Browse_adaptor,
  public DBus::IntrospectableAdaptor,
  public DBus::ObjectAdaptor
{
public:
	static char const * const BUSNAME_APK_FILEMGR_BROWSE;
	static char const * const BUSPATH_APK_FILEMGR_BROWSE;

public:
	cApkFilemgrBrowse(DBus::Connection &connection);

    uint32_t Open(const std::string& path, const uint32_t& category, const std::string& extFilter, const uint32_t& optFlags);
    void Close(const uint32_t& handle);
    void GetItems(const uint32_t& handle, const uint32_t& index, const uint32_t& reqCount, const int32_t& option);
    void ScanItems(const uint32_t& handle, const uint32_t& reqCount, const int32_t& option);
    void Sort(const uint32_t& handle, const uint32_t& sort);

private:

	static void	callback_Event(HUINT32 nHandleId, HINT32 eEvent, HINT32 nResult, void *pvData);

	/*
	 * APK_CAS_NAGRA�� Event Callback Interface�� userdata�� ���� �� �� ����. �׷��� dbus�� event�� 
	   ȣ�� �� �� ���� ���� �̱��� �����ͷ� �����Ѵ�.
	 */
	static cApkFilemgrBrowse*	pMainThis;
	

};

#endif
