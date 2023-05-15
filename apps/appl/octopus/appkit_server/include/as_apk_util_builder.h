#ifndef ___AS_APK_UTIL_BUILDER_H___
#define ___AS_APK_UTIL_BUILDER_H___

#include <as_apk_util_builder_adaptor.h>

class cApkUtilBuilder
  : public Octopus::Appkit::Util::Builder_adaptor,
  public DBus::IntrospectableAdaptor,
  public DBus::ObjectAdaptor
{
public:
	static char const * const BUSNAME_APK_UTIL_BUILDER;
	static char const * const BUSPATH_APK_UTIL_BUILDER;

public:
	cApkUtilBuilder(DBus::Connection &connection);

    uint32_t OpenSession(const uint32_t& deliveryType, const std::string& scanType, const ::DBus::Struct< uint32_t, uint32_t, std::string >& actionInfo);
    void CloseSession(const uint32_t& builderHandle);
    void ClearBuildInfo(const uint32_t& builderHandle);
    void AddBuildInfo_Sat(const uint32_t& builderHandle, const DbtTuneParamSat_t& tunerParam);
    void AddBuildInfo_Ter(const uint32_t& builderHandle, const DbtTuneParamTer_t& tunerParam);
    void AddBuildInfo_Cab(const uint32_t& builderHandle, const DbtTuneParamCab_t& tunerParam);
    void RemoveBuildInfo(const uint32_t& builderHandle, const DbtBuilder_BuildInfo_t& buildInfo);
    DbtBuilder_Option_t GetOption(const uint32_t& builderHandle);
    void SetOption(const uint32_t& builderHandle, const DbtBuilder_Option_t& buildOption);
    ::DBus::Struct< uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, ::DBus::Struct< uint32_t > > GetBuilderStatus(const uint32_t& builderHandle);
    ::DBus::Struct< int32_t, int32_t > GetBuilderSignalStatus(const uint32_t& builderHandle);
    std::vector< DbtBuilder_SearchedInfo_t > GetBuilderSvcInfo(const uint32_t& builderHandle, const int32_t& startIndex, const int32_t& count);
    std::string GetScanType(const uint32_t& builderHandle);
    void StartScan(const uint32_t& builderHandle);
    void StopScan(const uint32_t& builderHandle);
    void PauseScan(const uint32_t& builderHandle);
    void ResumeScan(const uint32_t& builderHandle);
    void SetAdditionalNumbers(const uint32_t& builderHandle);
    void MotorGotoPosition(const uint32_t& builderHandle, const int32_t& position);
    void MotorStorePosition(const uint32_t& builderHandle, const int32_t& position);
    void MotorResetPosition(const uint32_t& builderHandle);
    void MotorRecalcuate(const uint32_t& builderHandle, const int32_t& position);
    void MotorMove(const uint32_t& builderHandle, const uint32_t& direction, const uint32_t& mode, const int32_t& step);
    void MotorStop(const uint32_t& builderHandle);
    void MotorSetLimit(const uint32_t& builderHandle, const uint32_t& direction);
    void MotorEnableLimit(const uint32_t& builderHandle);
    void MotorDisableLimit(const uint32_t& builderHandle);
    void MotorGotoDegree(const uint32_t& builderHandle, const int32_t& degree);
    void MotorGotoSatellite(const uint32_t& builderHandle, const int32_t& satellite);
    void MotorSetMyLongitude(const int32_t& newLongitude);
    void MotorSetMyLatitude(const int32_t& newLatitude);
    int32_t MotorGetMyLongitude();
    int32_t MotorGetMyLatitude();


private:

	static void callback_Event (	HUINT32 ulBuilderId,
									DxBuilder_Event_e eEvent,
									DxBuilder_NotifierData_t *pstNotifyData,
									void *pvUserData
									);
							
};

#endif
