#ifndef ___AS_APK_UTIL_POWER_H___
#define ___AS_APK_UTIL_POWER_H___

#include <as_apk_util_power_adaptor.h>

class cApkUtilPower
  : public Octopus::Appkit::Util::Power_adaptor,
  public DBus::IntrospectableAdaptor,
  public DBus::ObjectAdaptor
{
public:
	static char const * const BUSNAME_APK_UTIL_POWER;
	static char const * const BUSPATH_APK_UTIL_POWER;

public:
	cApkUtilPower(DBus::Connection &connection);

    uint32_t GetWakeupReason();
    uint32_t GetWakeupSecondReason();
    void SetStandbyPowerMode(const uint32_t& powerMode);
    uint32_t GetStandbyPowerMode();
    bool IsStandbyMode();
    bool GoingToReboot();
    bool GoingToShutdown();
    bool GoingToOperate();
    void RequestShutDown();
    void RequestReboot();
    void RequestOperation();
    void WaitShutDown(const uint32_t& waitTime);

private:
	static void	callback_Event(HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3);

/*
 * APK_UTIL_POWER의 Event Callback Interface는 userdata를 전달 할 수 없다. 그래서 dbus의 event를 
   호출 할 수 없어 내장 싱글톤 포인터로 접근한다.
 */
 	static cApkUtilPower*	pMainThis;

};

#endif
