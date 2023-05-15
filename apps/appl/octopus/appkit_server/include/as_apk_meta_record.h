#ifndef ___AS_APK_META_RECORD_H___
#define ___AS_APK_META_RECORD_H___

#include <as_apk_meta_record_adaptor.h>

class cApkMetaRecord
  : public Octopus::Appkit::Meta::Record_adaptor,
  public DBus::IntrospectableAdaptor,
  public DBus::ObjectAdaptor
{
public:
	static char const * const BUSNAME_APK_META_RECORD;
	static char const * const BUSPATH_APK_META_RECORD;

public:
	cApkMetaRecord(DBus::Connection &connection);

	uint32_t GetCID(const std::string& url);
	uint32_t Count();
	DbtRecListData_t GetByIdx(const uint32_t& index);
	DbtRecListData_t GetByCID(const uint32_t& cid);
	uint32_t GetContentsIdByUrl(const std::string& url);
	void Release(const DbtRecListData_t& recData);
	void GetItems(const bool& direct);
	uint32_t Move(const uint32_t& cid, const std::string& path, const bool& isDecrypt);
	void CancelFileOperation(const uint32_t& cid);
	void Scan(const std::string& url);
	void Copy(const uint32_t& cid, const std::string& path, const bool& decrypt);
	void Cancel();
	void Clear();

private:

};

#endif
