#ifndef ___AS_APK_SCENARIO_BASE_H___
#define ___AS_APK_SCENARIO_BASE_H___

#include <as_apk_scenario_base_adaptor.h>

class cApkScenarioBase
  : public Octopus::Appkit::Scenario::Base_adaptor,
  public DBus::IntrospectableAdaptor,
  public DBus::ObjectAdaptor
{
public:
	static char const * const BUSNAME_APK_SCENARIO_BASE;
	static char const * const BUSPATH_APK_SCENARIO_BASE;

public:
	cApkScenarioBase(DBus::Connection &connection);

    std::string GetMainLanguage();
    void SetMainLanguage(const std::string& language);
    std::string GetAudioLanguage(const int32_t& index);
    void SetAudioLanguage(const int32_t& index, const std::string& language);
    std::string GetSubtitleLanguage(const int32_t& index);
    void SetSubtitleLanguage(const int32_t& index, const std::string& language);
    bool GetHardOfHearing();
    void SetHardOfHearing(const bool& enable);
    uint32_t GetMaturityRating();
    void SetMaturityRating(const uint32_t& rating);
    uint32_t GetCountryCode();
    void SetCountryCode(const uint32_t& countryCode);
    uint32_t GetOperatorType();
    void SetOperatorType(const uint32_t& opType);
    uint32_t GetAntennaConnection();
    void SetAntennaConnection(const uint32_t& connectionType);
    int32_t GetStartPadding();
    void SetStartPadding(const int32_t& padding);
    int32_t GetEndPadding();
    void SetEndPadding(const int32_t& padding);
    bool GetAutoDelete();
    void SetAutoDelete(const bool& autoDelete);
    std::string GetPostCode();
    void SetPostCode(const std::string& postcode);
    int32_t GetGMTOffset();
    void SetGMTOffset(const int32_t& gmtOffset);
    bool GetChannelSetupPIN();
    void SetChannelSetupPIN(const bool& enable);
    uint32_t GetGuidancePolicy();
    void SetGuidancePolicy(const uint32_t& guidancePolicy);
    bool GetSubtitleSetting();
    void SetSubtitleSetting(const bool& enable);
    bool GetDayLightSaving();
    void SetDayLightSaving(const bool& enable);
    bool GetTimeUpdate();
    void SetTimeUpdate(const bool& enable);
    std::string GetBouquetName();
    void SetBouquetName(const std::string& bouquet);
    int32_t GetBouquetID();
    void SetBouquetID(const int32_t& bouquetId);
    uint32_t GetSIMode();
    void SetSIMode(const uint32_t& SIMode);
    int32_t GetRegionCode();
    void SetRegionCode(const int32_t& regionCode);
    uint32_t GetAdultChannels();
    void SetAdultChannels(const uint32_t& adultChannel);
    std::string GetSWUpdateVersion();
    void SetSatRecReceiverId(const int32_t& receiverId);
    bool GetTVTVEnable();
    void SetTVTVEnable(const bool& enable);
    bool GetTVTVEPGDownload_StandbyEnable();
    void SetTVTVEPGDownload_StandbyEnable(const bool& enable);
    bool GetDescrambleOnOff();
    void SetDescrambleOnOff(const bool& enable);

private:


};

#endif
