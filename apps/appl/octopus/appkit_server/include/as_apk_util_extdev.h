#ifndef ___AS_APK_UTIL_EXTDEV_H___
#define ___AS_APK_UTIL_EXTDEV_H___

#include <as_apk_util_extdev_adaptor.h>

class cApkUtilExtdev
  : public Octopus::Appkit::Util::Extdev_adaptor,
  public DBus::IntrospectableAdaptor,
  public DBus::ObjectAdaptor
{
public:
	static char const * const BUSNAME_APK_UTIL_EXTDEV;
	static char const * const BUSPATH_APK_UTIL_EXTDEV;

public:
	cApkUtilExtdev(DBus::Connection &connection);

    void MIC_Start();
    void MIC_Stop();
    void MIC_SetVolume(const int32_t& value);
    int32_t MIC_GetVolume();
    int32_t MIC_GetCount();

private:
	static void callback_event (DxHotplug_NotiType_e eType, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3);

/*
 * APK_UTIL_EXTDEV�� Event Callback Interface�� userdata�� ���� �� �� ����. �׷��� dbus�� event�� 
   ȣ�� �� �� ���� ���� �̱��� �����ͷ� �����Ѵ�.
 */
 	static cApkUtilExtdev*	pMainThis;


};

#endif
