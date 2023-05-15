#ifndef ___AS_APK_MEDIA_AIRPLAY_H___
#define ___AS_APK_MEDIA_AIRPLAY_H___

#include <as_apk_media_airplay_adaptor.h>

class cApkMediaAirplay
  : public Octopus::Appkit::Media::Airplay_adaptor,
  public DBus::IntrospectableAdaptor,
  public DBus::ObjectAdaptor
{
public:
	static char const * const BUSNAME_APK_MEDIA_AIRPLAY;
	static char const * const BUSPATH_APK_MEDIA_AIRPLAY;

public:
	cApkMediaAirplay(DBus::Connection &connection);

    void Start();
    void Stop();
    void SetPlaybackInfo(const ::DBus::Struct< uint32_t, int64_t, int64_t, int64_t, int64_t, int64_t, int32_t >& playbackInfo);

private:

	static void callback_Event (HUINT32 eEvent, HUINT32 ulParam1, HUINT32 ulParam2);

/*
 * APK_AIRPLAY의 Event Callback Interface는 userdata를 전달 할 수 없다. 그래서 dbus의 event를 
   호출 할 수 없어 내장 싱글톤 포인터로 접근한다.
 */
 	static cApkMediaAirplay*	pMainThis;

};

#endif
