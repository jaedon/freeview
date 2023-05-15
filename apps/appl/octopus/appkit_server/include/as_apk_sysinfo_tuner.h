#ifndef ___AS_APK_SYSINFO_TUNER_H___
#define ___AS_APK_SYSINFO_TUNER_H___

#include <as_apk_sysinfo_tuner_adaptor.h>

class cApkSysinfoTuner
  : public Octopus::Appkit::Sysinfo::Tuner_adaptor,
  public DBus::IntrospectableAdaptor,
  public DBus::ObjectAdaptor
{
public:
	static char const * const BUSNAME_APK_SYSINFO_TUNER;
	static char const * const BUSPATH_APK_SYSINFO_TUNER;

public:
	cApkSysinfoTuner(DBus::Connection &connection);

    uint32_t GetDeviceNum();
    uint32_t GetDevType(const int32_t& index);
    bool IsLocked(const int32_t& index);
    int32_t GetSignalLevel(const int32_t& index);
    int32_t GetSignalQuality(const int32_t& index);
    int32_t GetAgc(const int32_t& index);
    double GetBer(const int32_t& index);
    double GetSnr(const int32_t& index);
    double GetInputPower(const int32_t& index);
    ::DBus::Struct< uint32_t > GetTuneParam(const int32_t& index);
    int32_t GetTunerIdByTripleId(const int32_t& onId, const int32_t& tsId, const int32_t& svcId);

private:
	static void callback_Event (void);
	
	/*
	 * APK_SYSINFO_TUNER�� Event Callback Interface�� userdata�� ���� �� �� ����. �׷��� dbus�� event�� 
	   ȣ�� �� �� ���� ���� �̱��� �����ͷ� �����Ѵ�.
	 */
	static cApkSysinfoTuner* pMainThis;


};

#endif
