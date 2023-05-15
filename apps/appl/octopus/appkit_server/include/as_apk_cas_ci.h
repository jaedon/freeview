#ifndef ___AS_APK_CAS_CI_H___
#define ___AS_APK_CAS_CI_H___

#include <as_apk_cas_ci_adaptor.h>

class cApkCasCi
  : public Octopus::Appkit::Cas::Ci_adaptor,
  public DBus::IntrospectableAdaptor,
  public DBus::ObjectAdaptor
{
public:
	static char const * const BUSNAME_APK_CAS_CI;
	static char const * const BUSPATH_APK_CAS_CI;

public:
	cApkCasCi(DBus::Connection &connection);

    bool IsSupportedCI();
    bool IsSupportedCIPlus();
    std::string GetCIPlusVersionName();
    uint32_t GetSlotCount();
    uint32_t GetCamNumberBySlot(const uint32_t& slot);
    uint32_t GetCamStatusByIndex(const uint32_t& slot);
    std::string GetCamNameByIndex(const uint32_t& slot);
    void RequestFileorData(const uint32_t& sessionId, const uint32_t& reqType, const std::string& reqName);
    void RequestAppAbort(const uint32_t& sessionId, const uint32_t& abortReqCode);

private:

	static void callback_Event (HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3);

	static void mmi_callback_Event (	CI_AMMI_MSG_TYPE enMsgType,
										void* pvUserData,
										HUINT32 ulSize);

/*
 * APK_CAS_CI의 Event Callback Interface는 userdata를 전달 할 수 없다. 그래서 dbus의 event를 
   호출 할 수 없어 내장 싱글톤 포인터로 접근한다.
 */
 	static cApkCasCi*	pMainThis;

};

#endif
