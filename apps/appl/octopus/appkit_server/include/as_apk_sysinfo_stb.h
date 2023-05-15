#ifndef ___AS_APK_SYSINFO_STB_H___
#define ___AS_APK_SYSINFO_STB_H___

#include <as_apk_sysinfo_stb_adaptor.h>

class cApkSysinfoStb
  : public Octopus::Appkit::Sysinfo::Stb_adaptor,
  public DBus::IntrospectableAdaptor,
  public DBus::ObjectAdaptor
{
public:
	static char const * const BUSNAME_APK_SYSINFO_STB;
	static char const * const BUSPATH_APK_SYSINFO_STB;

public:
	cApkSysinfoStb(DBus::Connection &connection);

    std::string GetModelName();
    std::string GetUpdatedList();
    std::string GetSoftwareVersion();
    std::string GetLoaderName();
    std::string GetLoaderVersion();
    std::string GetManufacturerName();
    std::string GetSystemId();
    std::string GetDeviceId();
    std::string GetChipId();
    std::string GetObamaVersion();
    std::string GetMicomVersion();
    std::string GetUpdateDate();
    std::string GetSerialNumber();
    uint32_t GetCustomCode();
    bool GetColdBootDone();
    void SetFirstBootOnProduction(const bool& firstBootOnProduction);
    bool GetFirstBootOnProduction();
    uint32_t GetLaunchAppMode();
    void SetAntennaPower(const bool& antPower);
    bool GetAntennaPower();
    void SetServiceUpdateFlag(const bool& enable);
    bool GetServiceUpdateFlag();
    ::DBus::Struct< uint32_t, uint32_t, uint32_t > GetNandBadBlocks();
    ::DBus::Struct< uint8_t, uint8_t, uint8_t, uint8_t > GetFusingInfo();

private:
	static void callback_Event (void);

	/*
	 * APK_SYSINFO_STB�� Event Callback Interface�� userdata�� ���� �� �� ����. �׷��� dbus�� event��
	   ȣ�� �� �� ���� ���� �̱��� �����ͷ� �����Ѵ�.
	 */
	static cApkSysinfoStb* pMainThis;


};

#endif
