#ifndef ___AS_APK_UTIL_DATASTREAMER_H___
#define ___AS_APK_UTIL_DATASTREAMER_H___

#include <as_apk_util_datastreamer_adaptor.h>

class cApkUtilDatastreamer
  : public Octopus::Appkit::Util::Datastreamer_adaptor,
  public DBus::IntrospectableAdaptor,
  public DBus::ObjectAdaptor
{
public:
	static char const * const BUSNAME_APK_UTIL_DATASTREAMER;
	static char const * const BUSPATH_APK_UTIL_DATASTREAMER;

public:
	cApkUtilDatastreamer(DBus::Connection &connection);

    void ConnectDataStreamer(const std::string& dataType);
    void DisconnectDataStreamer(const std::string& dataType);

private:

	static void callback_Event (HINT32 size, void *pvData);

/*
 * APK_DATASTREAMER의 Event Callback Interface는 userdata를 전달 할 수 없다. 그래서 dbus의 event를 
   호출 할 수 없어 내장 싱글톤 포인터로 접근한다.
 */
 	static cApkUtilDatastreamer*	pMainThis;

};

#endif
