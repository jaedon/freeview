#ifndef ___AS_APK_UTIL_NETWORK_H___
#define ___AS_APK_UTIL_NETWORK_H___

#include <as_apk_util_network_adaptor.h>

class cApkUtilNetwork
  : public Octopus::Appkit::Util::Network_adaptor,
  public DBus::IntrospectableAdaptor,
  public DBus::ObjectAdaptor
{
public:
	static char const * const BUSNAME_APK_UTIL_NETWORK;
	static char const * const BUSPATH_APK_UTIL_NETWORK;

public:
	cApkUtilNetwork(DBus::Connection &connection);

    void BT_SetEnable(const bool& enable);
    bool BT_GetEnable();
    void BT_Scan();
    void BT_Connect(const int32_t& index);
    void BT_ConnectByBdAddr(const std::string& bdAddress);
    void BT_Disconnect(const int32_t& index);
    void BT_DisconnectByBdAddr(const std::string& bdAddress);
    void BT_Remove(const int32_t& index);
    void BT_RemoveByBdAddr(const std::string& bdAddress);
    void BT_SendPinCode(const int32_t& index, const std::vector< uint8_t >& pinCode);
    void BT_SendPinCodeByBdAddr(const std::string& bdAddress, const std::vector< uint8_t >& pinCode);
    uint32_t BT_GetDeviceNum();
   ::DBus::Struct< uint32_t, std::string, uint32_t, std::string, uint32_t, std::string > BT_GetDeviceInfo(const uint32_t& deviceId);
    std::string BT_GetMacAddrInfo();
    uint32_t BT_GetMacAddrInfoSize();
    void BT_GetDevInfoList(const uint32_t& listSize, uint32_t& reval, std::vector< ::DBus::Struct< uint32_t, std::string, uint32_t, std::string, uint32_t, std::string > >& deviceInfoList);
    uint32_t CM_GetStatus();
    int32_t CM_GetDownStreamInfo(const uint32_t& downInfo);
    void CM_SetDownStreamInfo(const uint32_t& downInfo, const int32_t& value);
    int32_t CM_GetUpStreamInfo(const uint32_t& upInfo);
    void DHCPSVR_UpdateInfo();
    bool DHCPSVR_IsRunning();
    int32_t DHCPSVR_GetDevIndex();
    void DHCPSVR_Start(const int32_t& index);
    void DHCPSVR_Stop(const int32_t& index);
    int32_t CONF_GetNumOfNetDev();
    int32_t CONF_GetDefaultDev();
    ::DBus::Struct< uint32_t, bool, bool, bool, std::string, std::string > CONF_GetDevInfo(const int32_t& index);
    std::string CONF_GetMacAddr(const int32_t& index);
    ::DBus::Struct< ::DBus::Struct< bool, bool >, ::DBus::Struct< int64_t, int64_t, int64_t, int64_t, int64_t >, ::DBus::Struct< bool, bool, uint32_t, bool, std::string, std::string, std::string, std::string, uint32_t, int64_t, int64_t > > CONF_GetAddrInfo(const int32_t& index);
    void CONF_SetAddrInfo(const int32_t& index, const ::DBus::Struct< ::DBus::Struct< bool, bool >, ::DBus::Struct< int64_t, int64_t, int64_t, int64_t, int64_t >, ::DBus::Struct< bool, bool, uint32_t, bool, std::string, std::string, std::string, std::string, uint32_t, int64_t, int64_t > >& netAddress);
    void CONF_StopConnecting(const int32_t& index);
    void CONF_SaveEnableStatus(const int32_t& index, const bool& enable);
    std::string CONF_AddrN2A(const uint64_t& address);
    uint64_t CONF_AddrA2N(const std::string& addressString);
    bool CONF_IsSupportNetwork();
    bool CONF_IsNetworkAvailable();
    void WIFI_StartScan(const int32_t& index);
    void WIFI_ConnectAP(const int32_t& index, const ::DBus::Struct< std::string, std::string, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t >& apInfo, const std::string& key);
    void WIFI_ConnectAPByWPS(const int32_t& index, const uint32_t& wpsType, const std::string& pinCode);
    void WIFI_DisconnectAP(const int32_t& index);
    void WIFI_GetConnectedInfo(const int32_t& index, ::DBus::Struct< std::string, std::string, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t >& apInfo, std::string& key);

private:
	static void callback_Event ( HINT32 eEvent, HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3);
	
	/*
	 * APK_OUTPUT_AUDIO의 Event Callback Interface는 userdata를 전달 할 수 없다. 그래서 dbus의 event를 
	   호출 할 수 없어 내장 싱글톤 포인터로 접근한다.
	 */
	static cApkUtilNetwork* pMainThis;
};

#endif
