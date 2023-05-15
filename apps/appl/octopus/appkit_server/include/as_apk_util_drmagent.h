#ifndef ___AS_APK_UTIL_DRMAGENT_H___
#define ___AS_APK_UTIL_DRMAGENT_H___

#include <as_apk_util_drmagent_adaptor.h>

class cApkUtilDrmagent
  : public Octopus::Appkit::Util::Drmagent_adaptor,
  public DBus::IntrospectableAdaptor,
  public DBus::ObjectAdaptor
{
public:
	static char const * const BUSNAME_APK_UTIL_DRMAGENT;
	static char const * const BUSPATH_APK_UTIL_DRMAGENT;

public:
	cApkUtilDrmagent(DBus::Connection &connection);

    void SendDrmMessage(const std::string& msgType, const std::string& message, const std::string& drmSystemId, const uint32_t& msgId);
    void SendDrmSystemStatusRequest(const std::string& drmSystemId);

private:
	static void msg_callback_event(const HCHAR *pzResultMsg, HUINT16 usMsgId, HUINT32 ulResultCode);
	static void status_callback_event(const HCHAR *pzDrmSystemId);

/*
 * APK_DRMAGENT의 Event Callback Interface는 userdata를 전달 할 수 없다. 그래서 dbus의 event를 
   호출 할 수 없어 내장 싱글톤 포인터로 접근한다.
 */
 	static cApkUtilDrmagent*	pMainThis;

};

#endif
