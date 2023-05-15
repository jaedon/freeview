#ifndef ___AS_APK_DLNA_UTIL_H___
#define ___AS_APK_DLNA_UTIL_H___

#include <as_apk_dlna_util_adaptor.h>

class cApkDlnaUtil
  : public Octopus::Appkit::Dlna::Util_adaptor,
  public DBus::IntrospectableAdaptor,
  public DBus::ObjectAdaptor
{
public:
	static char const * const BUSNAME_APK_DLNA_UTIL;
	static char const * const BUSPATH_APK_DLNA_UTIL;

public:
	cApkDlnaUtil(DBus::Connection &connection);

    void DMP_Start();
    void DMP_Stop();
    std::vector< ::DBus::Struct< std::string, std::string, std::string, int32_t, std::string, std::string, bool, uint32_t, uint32_t, uint32_t, uint32_t, std::vector< std::string >, std::vector< std::string >, std::vector< std::string >, std::vector< std::string > > > DMP_GetListOfDMS();
    void DMP_BrowsingChildItem(const std::string& udn, const std::string& cid, const std::string& sort, const int32_t& startIndex, const int32_t& request);
    void DMP_SearchingItem(const std::string& udn, const std::string& cid, const std::string& category, const std::string& keyword, const std::string& sort, const int32_t& startIndex, const int32_t& request, const int32_t& requestId);
    void DMP_SearchingAll(const std::string& cid, const std::string& category, const std::string& keyword, const std::string& sort, const int32_t& requestId);
    void DMP_GetRecordDestinations(const std::string& udn);
    void DMP_GetRecordDiskInfo(const std::string& udn, const std::string& id);
    std::string DMP_RemoveItem(const std::string& udn, const std::string& id);
    bool DMP_GetIsIPDubbing();
    void DMP_RefreshDms(const std::string& udn);
    void DMR_Start(const std::string& friendlyName);
    void DMR_Stop();
    std::string DMR_GetDefaultFriendlyName();
    void DMR_SetFriendlyName(const std::string& friendlyName);
    void DMR_SetBringInUri(const std::string& url);
    void DMR_SetOperationMode(const int32_t& mode);
    void DMS_Start(const std::string& friendlyName, const bool& boottimeStart);
    void DMS_Stop();
    std::string DMS_GetDefaultFriendlyName();
    void DMS_SetFriendlyName(const std::string& friendlyName);
    void DIAL_Start(const std::string& friendlyName);
    void DIAL_Stop();
    void DIAL_SetFriendlyName(const std::string& friendlyName);
    void DIAL_AppRegist(const std::string& app);
    void DIAL_AppUnregist(const std::string& app);
    void DIAL_SetAppState(const std::string& app, const int32_t& state);
    void WOONSERVER_Start(const std::string& friendlyName);
    void WOONSERVER_Stop();
    std::string WOONSERVER_GetDefaultFriendlyName();
    void WOONSERVER_SetFriendlyName(const std::string& friendlyName);
    void WOONSERVER_TestConnection();
    void SATIP_Start(const std::string& friendlyName);
    void SATIP_Stop();
    std::string SATIP_GetDefaultFriendlyName();
    void SATIP_SetFriendlyName(const std::string& friendlyName);
    void DLNARUIS_SendMessage(const int32_t& deviceHandle, const int32_t& reqHandle, const std::vector< uint8_t >& header, const std::vector< uint8_t >& message);
    void DLNARUIS_SendMulticast(const int32_t& deviceHandle, const int32_t& eventLevel, const std::vector< uint8_t >& notifCEHTML, const std::string& friendlyName, const std::vector< uint8_t >& profielList);
    void DLNARUIS_SendErrorCode(const int32_t& deviceHandle, const int32_t& reqHandle, const uint32_t& error);
    void LiveStream_Start(const ::DBus::Struct< bool, uint32_t, int32_t, bool, uint32_t, uint32_t >& setupInfo);
    void LiveStream_Stop(const int32_t& handle);
    int32_t LiveStream_ItemindexByProfile(const std::string& profileName);
    ::DBus::Struct< bool, uint32_t > LiveStream_Status(const int32_t& handle, const int32_t& type);

private:
	static void dmp_callback_event(HUINT32 eEvent, HUINT32 ulParam1, HUINT32 ulParam2);
	static void dmr_callback_event(HUINT32 eEvent, HUINT32 ulParam1, HUINT32 ulParam2);
	static void dms_callback_event(HUINT32 eEvent, HUINT32 ulParam1, HUINT32 ulParam2);
	static void dial_callback_event(HUINT32 eEvent, HUINT32 ulParam1, HUINT32 ulParam2);
	static void ws_callback_event(HUINT32 eEvent, HUINT32 ulParam1, HUINT32 ulParam2);
	static void satip_callback_event(HUINT32 eEvent, HUINT32 ulParam1, HUINT32 ulParam2);
	static void dlnaruis_callback_event(HUINT32 eEvent, HUINT32 ulParam1, HUINT32 ulParam2);
	static void obama_callback_event(HUINT32 eEvent, HUINT32 ulParam1, HUINT32 ulParam2);
	
	
	/*
	 * APK_DLAN_UTIL의 Event Callback Interface는 userdata를 전달 할 수 없다. 그래서 dbus의 event를 
	   호출 할 수 없어 내장 싱글톤 포인터로 접근한다.
	 */
	static cApkDlnaUtil*	pMainThis;
};

#endif
