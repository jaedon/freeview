#ifndef ___AS_APK_MEDIA_RECORD_H___
#define ___AS_APK_MEDIA_RECORD_H___

#include <as_apk_media_record_adaptor.h>

class cApkMediaRecord
  : public Octopus::Appkit::Media::Record_adaptor,
  public DBus::IntrospectableAdaptor,
  public DBus::ObjectAdaptor
{
public:
	static char const * const BUSNAME_APK_MEDIA_RECORD;
	static char const * const BUSPATH_APK_MEDIA_RECORD;

public:
	cApkMediaRecord(DBus::Connection &connection);

	bool IsRecording(const uint32_t& svcUid);
	uint32_t GetCID(const uint32_t& svcUid);
	uint32_t Start(const ::DBus::Struct< uint32_t, uint32_t, uint32_t, int32_t, uint32_t, uint32_t, uint32_t, std::vector< uint8_t >, int32_t, std::string >& startInfo);
	void Stop(const uint32_t& sessionId);
	void StopByUID(const uint32_t& svcUid);
	uint32_t GetRecTime(const uint32_t& sessionId);
	uint32_t GetStartTime(const uint32_t& sessionId);
	std::string GetURL(const uint32_t& sessionId);
	uint32_t GetRsvSlotByUID(const uint32_t& svcUid);

private:
	
	static void callback_Event (	HUINT32 ulSvcUid,
									HUINT32 ulSlot,
									OxMediaRec_Event_e eEvent,
									OxMediaRec_NotifierData_t *pstNotifyData,
									void *pvUserData
								);


};

#endif
