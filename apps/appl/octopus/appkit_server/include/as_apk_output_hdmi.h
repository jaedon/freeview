#ifndef ___AS_APK_OUTPUT_HDMI_H___
#define ___AS_APK_OUTPUT_HDMI_H___

#include <as_apk_output_hdmi_adaptor.h>

class cApkOutputHdmi
  : public Octopus::Appkit::Output::Hdmi_adaptor,
  public DBus::IntrospectableAdaptor,
  public DBus::ObjectAdaptor
{
public:
	static char const * const BUSNAME_APK_OUTPUT_HDMI;
	static char const * const BUSPATH_APK_OUTPUT_HDMI;

public:
	cApkOutputHdmi(DBus::Connection &connection);

    bool SupportCec();
    uint32_t GetHdcpStatus();
    void Set3DSelect(const uint32_t& hdmi3DSelect);
    uint32_t Get3DSelect();
    uint32_t GetEdid3DFormatCapability();
    uint32_t Get3DStatus();
    uint32_t GetEdidResolutionCapability();
    void SetCecEnable(const bool& support);
    bool GetCecEnable();

private:
	static void callback_Event (void);

	/*
	 * APK_OUTPUT_HDMI의 Event Callback Interface는 userdata를 전달 할 수 없다. 그래서 dbus의 event를 
	   호출 할 수 없어 내장 싱글톤 포인터로 접근한다.
	 */
	static cApkOutputHdmi* pMainThis;
	

};

#endif
