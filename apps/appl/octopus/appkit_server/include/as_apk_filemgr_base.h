#ifndef ___AS_APK_FILEMGR_BASE_H___
#define ___AS_APK_FILEMGR_BASE_H___

#include <as_apk_filemgr_base_adaptor.h>

class cApkFilemgrBase
  : public Octopus::Appkit::Filemgr::Base_adaptor,
  public DBus::IntrospectableAdaptor,
  public DBus::ObjectAdaptor
{
public:
	static char const * const BUSNAME_APK_FILEMGR_BASE;
	static char const * const BUSPATH_APK_FILEMGR_BASE;

public:
	cApkFilemgrBase(DBus::Connection &connection);

    int32_t BrowserFiles(const std::string& path, const std::string& filter, const std::string& keyword, const uint32_t& browsingMode);
    int32_t ScanDir(const std::string& source, const std::string& extFilter, const bool& recursive);
    int32_t Remove(const std::string& source, const std::string& extFilter, const bool& recursive);
    int32_t Copy(const std::string& source, const std::string& dest, const std::string& extFilter, const bool& decryption);
    int32_t Move(const std::string& source, const std::string& dest, const std::string& extFilter, const bool& decryption);
    int32_t MakeDir(const std::string& source);
    int32_t RemoveDir(const std::string& source);
    int32_t Cancel(const int32_t& cancleRequestId);
    uint64_t GetSizeOfPath(const std::string& path, const std::string& extFilter);

private:
	static void api_callback(int requestId, void *pvUserData, HxList_t *pstResultList, void *pvCbInfo);


};

#endif
