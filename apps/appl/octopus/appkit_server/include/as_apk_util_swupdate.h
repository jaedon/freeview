#ifndef ___AS_APK_UTIL_SWUPDATE_H___
#define ___AS_APK_UTIL_SWUPDATE_H___

#include <as_apk_util_swupdate_adaptor.h>

class cApkUtilSwupdate
  : public Octopus::Appkit::Util::Swupdate_adaptor,
  public DBus::IntrospectableAdaptor,
  public DBus::ObjectAdaptor
{
public:
	static char const * const BUSNAME_APK_UTIL_SWUPDATE;
	static char const * const BUSPATH_APK_UTIL_SWUPDATE;

public:
	cApkUtilSwupdate(DBus::Connection &connection);

    uint32_t Create(const uint32_t& swUpdateSpec);
    uint32_t CreateManualConnect(const uint32_t& swUpdateSpec, const DbtTransponder_t& tunningInfo);
    void StartDetect(const uint32_t& handle);
    void StopDetect(const uint32_t& handle);
    void StartDownload(const uint32_t& handle);
    void StopDownload(const uint32_t& handle);
    void StartInstall(const uint32_t& handle);
    void StopInstall(const uint32_t& handle);
    void Cancel(const uint32_t& handle);

private:
	static void callback_Event (DxSwUpdate_EventData_t *pstData);

/*
 * APK_OUTPUT_AUDIO의 Event Callback Interface는 userdata를 전달 할 수 없다. 그래서 dbus의 event를 
   호출 할 수 없어 내장 싱글톤 포인터로 접근한다.
 */
 	static cApkUtilSwupdate*	pMainThis;


};

#endif
