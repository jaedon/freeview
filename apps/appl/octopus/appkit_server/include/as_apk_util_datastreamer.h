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
 * APK_DATASTREAMER�� Event Callback Interface�� userdata�� ���� �� �� ����. �׷��� dbus�� event�� 
   ȣ�� �� �� ���� ���� �̱��� �����ͷ� �����Ѵ�.
 */
 	static cApkUtilDatastreamer*	pMainThis;

};

#endif
