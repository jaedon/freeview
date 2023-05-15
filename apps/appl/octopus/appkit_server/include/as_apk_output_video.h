#ifndef ___AS_APK_OUTPUT_VIDEO_H___
#define ___AS_APK_OUTPUT_VIDEO_H___

#include <as_apk_output_video_adaptor.h>

class cApkOutputVideo
  : public Octopus::Appkit::Output::Video_adaptor,
  public DBus::IntrospectableAdaptor,
  public DBus::ObjectAdaptor
{
public:
	static char const * const BUSNAME_APK_OUTPUT_VIDEO;
	static char const * const BUSPATH_APK_OUTPUT_VIDEO;

public:
	cApkOutputVideo(DBus::Connection &connection);

    bool SupportComposite();
    bool SupportTvScart();
    bool SupportVcrScart();
    uint32_t GetSupportTvScartFormat();
    uint32_t GetSupportVcrScartFormat();
    void SetTvScartFormat(const uint32_t& scartFormat);
    uint32_t GetTvScartFormat();
    void SetVcrScartFormat(const uint32_t& scartFormat);
    uint32_t GetVcrScartFormat();
    void SetScartPath(const uint32_t& scartPath);
    uint32_t GetScartPath();
    void SetResolutionSelect(const uint32_t& resolutionSelect);
    uint32_t GetResolutionSelect();
    uint32_t GetResolutionStatus();
    void SetTvAspectRatio(const uint32_t& tvAspectRatio);
    uint32_t GetTvAspectRatio();
    uint32_t GetPictureRate();
    void SetDfcSelect(const uint32_t& dfcSelect);
    uint32_t GetDfcSelect();
    uint32_t GetDfcStatus();
    void SetBackGroundColor(const uint8_t& red, const uint8_t& green, const uint8_t& blue);
    ::DBus::Struct< uint8_t, uint8_t, uint8_t > GetBackGroundColor();
    void SetAvEnable(const bool& enable);
    bool GetAvEnable();
    uint32_t GetCompositeStandard();

private:
	static void callback_Event (void);
	
	/*
	 * APK_OUTPUT_VIDEO의 Event Callback Interface는 userdata를 전달 할 수 없다. 그래서 dbus의 event를 
	   호출 할 수 없어 내장 싱글톤 포인터로 접근한다.
	 */
	static cApkOutputVideo* pMainThis;


};

#endif
