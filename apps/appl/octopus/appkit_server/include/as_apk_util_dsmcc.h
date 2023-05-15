#ifndef ___AS_APK_UTIL_DSMCC_H___
#define ___AS_APK_UTIL_DSMCC_H___

#include <as_apk_util_dsmcc_adaptor.h>

class cApkUtilDsmcc
  : public Octopus::Appkit::Util::Dsmcc_adaptor,
  public DBus::IntrospectableAdaptor,
  public DBus::ObjectAdaptor
{
public:
	static char const * const BUSNAME_APK_UTIL_DSMCC;
	static char const * const BUSPATH_APK_UTIL_DSMCC;

public:
	cApkUtilDsmcc(DBus::Connection &connection);

    ::DBus::Struct< uint32_t, std::string > Create(const uint32_t& type);
    void Destroy(const uint32_t& handle);
    DbtDSMCC_CarouselList_t GetCarouselList(const uint32_t& handle);
    void Start(const uint32_t& handleDsmcc, const uint32_t& handleCarousel, const std::string& cachePath);
    void ReStart(const uint32_t& handleDsmcc, const uint32_t& handleCarousel);
    void Stop(const uint32_t& handleDsmcc, const uint32_t& handleCarousel);
    void RequestEvent(const uint32_t& handleDsmcc, const bool& all);
    void Open(const uint32_t& handleDsmcc, const uint32_t& handleSession);
    void Close(const uint32_t& handleDsmcc, const uint32_t& handleSession);

private:
	static void callback_Event (HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3);

/*
 * APK_UTIL_DSMCC의 Event Callback Interface는 userdata를 전달 할 수 없다. 그래서 dbus의 event를 
   호출 할 수 없어 내장 싱글톤 포인터로 접근한다.
 */
	static cApkUtilDsmcc*	pMainThis;


};

#endif
