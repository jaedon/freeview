#ifndef ___AS_APK_OUTPUT_PANEL_H___
#define ___AS_APK_OUTPUT_PANEL_H___

#include <as_apk_output_panel_adaptor.h>

class cApkOutputPanel
  : public Octopus::Appkit::Output::Panel_adaptor,
  public DBus::IntrospectableAdaptor,
  public DBus::ObjectAdaptor
{
public:
	static char const * const BUSNAME_APK_OUTPUT_PANEL;
	static char const * const BUSPATH_APK_OUTPUT_PANEL;

public:
	cApkOutputPanel(DBus::Connection &connection);

    void SetAppPriority(const int32_t& priority);
    void SetLed(const uint32_t& ledId, const uint32_t& ledStatus);
    void SetLedAuto();
    void SetLedCategory(const uint32_t& category);
    void ClearLed();
    void SetPanel(const int32_t& priority, const std::string& text, const uint32_t& panelAlign);
    void SetPanelAuto(const int32_t& priority);
    void SetPanelDimLevel(const int32_t& priority, const int32_t& dimLevel);
    void SetPanelAnimation(const int32_t& priority);

private:


};

#endif
