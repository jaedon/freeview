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
	 * APK_OUTPUT_HDMI�� Event Callback Interface�� userdata�� ���� �� �� ����. �׷��� dbus�� event�� 
	   ȣ�� �� �� ���� ���� �̱��� �����ͷ� �����Ѵ�.
	 */
	static cApkOutputHdmi* pMainThis;
	

};

#endif
