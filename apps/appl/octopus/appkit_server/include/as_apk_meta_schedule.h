#ifndef ___AS_APK_META_SCHEDULE_H___
#define ___AS_APK_META_SCHEDULE_H___

#include <as_apk_meta_schedule_adaptor.h>

class cApkMetaSchedule
  : public Octopus::Appkit::Meta::Schedule_adaptor,
  public DBus::IntrospectableAdaptor,
  public DBus::ObjectAdaptor
{
public:
	static char const * const BUSNAME_APK_META_SCHEDULE;
	static char const * const BUSPATH_APK_META_SCHEDULE;

public:
	cApkMetaSchedule(DBus::Connection &connection);

    DbtSchedule_t GetSchedule(const uint32_t& slot);
    DbtSchedule_t GetScheduleByIndex(const uint32_t& index);
    DbtSchedule_t GetScheduleByType(const uint32_t& rsvType, const uint32_t& index);
    uint32_t CountSchedule();
    uint32_t CountScheduleByType(const uint32_t& rsvType);
    uint32_t AddSchedule(const DbtSchedule_t & schedule);
    void UpdateSchedule(const uint32_t& slot, const DbtSchedule_t & schedule);
    void CancelSchedule(const uint32_t& slot);
    void StopSchedule(const uint32_t& slot);
    void PauseSchedule(const uint32_t& slot);
    void ResumeSchedule(const uint32_t& slot);
    ::DBus::Struct< uint32_t, std::vector< uint32_t > > GetConflictResult (const uint32_t& slot);

private:

	static void callback_Event (HINT32 event, const HINT32 *args, void *userdata);
	static HBOOL callback_UpdateChecker (const HCHAR *typeName, const void *_old, const void *_new, HBOOL *updated);

};

#endif
