#ifndef ___AS_APK_UTIL_EXE_H___
#define ___AS_APK_UTIL_EXE_H___

#include <as_apk_util_exe_adaptor.h>

class cApkUtilExe
  : public Octopus::Appkit::Util::Exe_adaptor,
  public DBus::IntrospectableAdaptor,
  public DBus::ObjectAdaptor
{
public:
	static char const * const BUSNAME_APK_UTIL_EXE;
	static char const * const BUSPATH_APK_UTIL_EXE;

public:
	cApkUtilExe(DBus::Connection &connection);

    void BATCH_Stop();
    void BGEIT_Start_FreeSat();
    void BGEIT_Stop_FreeSat();
    void FTP_Start();
    void FTP_Stop();
    bool FTP_GetStatus();
    void StartFactoryDefault(const uint32_t& factoryDefaultMode);
    void RemoveLcnUpdateInfo(const bool& syncMode);
    void BML_Set(const bool& BMLKeyControlMode);
    void RFVOD_Start(const bool& vodMode, const std::string& url);
    void RFVOD_Stop();
    void MHEG_Start(const uint32_t& wndId, const uint32_t& stillWndId);
    void MHEG_SetKey(const uint32_t& key);
    void MHEG_DeepLink(const std::string& url, const uint32_t& prevLcn);
    void MHEG_ControlOP(const bool& control);
    void MHEGIB_RequestFile(const uint32_t& sessionId, const int32_t& requestNameLen, const std::string& requestName);
    void MHEGIB_RequestData(const uint32_t& sessionId, const int32_t& requestNameLen, const std::string& requestName);
    void MHEGIB_AbortByUser(const uint32_t& sessionId);
    void MHEGIB_AbortBySystem(const uint32_t& sessionId);
    void StartSatRecord();
    void TVTVEPG_Initdirectory();
    void Load_BaseEPG();
    void Load_TVTVEPG();

private:


};

#endif
