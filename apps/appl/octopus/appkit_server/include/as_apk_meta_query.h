#ifndef ___AS_APK_META_QUERY_H___
#define ___AS_APK_META_QUERY_H___

#include <as_apk_meta_query_adaptor.h>

class cApkMetaQuery
  : public Octopus::Appkit::Meta::Query_adaptor,
  public DBus::IntrospectableAdaptor,
  public DBus::ObjectAdaptor
{
public:
	static char const * const BUSNAME_APK_META_QUERY;
	static char const * const BUSPATH_APK_META_QUERY;

public:
	cApkMetaQuery(DBus::Connection &connection);

    uint32_t NewQuery(const std::string& field, const std::string& comparision, const std::string& value);
    void DeleteQuery(const uint32_t& queryHandle);
    uint32_t CloneQuery(const uint32_t& queryHandle);
    uint32_t And(const uint32_t& queryHandle1, const uint32_t& queryHandle2);
    uint32_t Or(const uint32_t& queryHandle1, const uint32_t& queryHandle2);
    uint32_t Not(const uint32_t& queryHandle);
    uint32_t New(const uint32_t& target);
    void Delete(const uint32_t& metaQhandle);
    void SetQuery(const uint32_t& metaQhandle, const uint32_t& queryHandle);
    uint32_t GetQuery(const uint32_t& metaQhandle);
    void AddSvcConstraint_uid(const uint32_t& metaQhandle, const uint32_t& svcuid);
    void AddSvcConstraint_tripleid(const uint32_t& metaQhandle, const uint32_t& onid, const uint32_t& tsid, const uint32_t& svcid);
    void ClearSvcConstraint(const uint32_t& metaQhandle);
    void OrderBy(const uint32_t& metaQhandle, const std::string& field, const bool& ascending);
    void SetFilter(const uint32_t& metaQhandle, const uint32_t& filter);
    void Search(const uint32_t& metaQhandle, const bool& direct);
    void Abort(const uint32_t& metaQhandle);
    std::vector< DbtEvent_t > Result(const uint32_t& metaQhandle);
    DbtEvent_t GetEvent_uid(const uint32_t& type, const uint32_t& svcuid);
    DbtEvent_t GetEvent_tripleid(const uint32_t& type, const uint32_t& onid, const uint32_t& tsid, const uint32_t& svcid);
    DbtEvent_t GetEventWithIds_uid(const uint32_t& svcuid, const uint32_t& eventid);
    DbtEvent_t GetEventWithIds_tripleid(const uint32_t& onid, const uint32_t& tsid, const uint32_t& svcid, const uint32_t& eventid);
    DbtEvent_t GetIpEvent_uid(const uint32_t& svcuid, const uint32_t& eventid);
    DbtEvent_t GetIpEvent_tripleid(const uint32_t& onid, const uint32_t& tsid, const uint32_t& svcid, const uint32_t& eventid);
    ::DBus::Struct< std::vector< DbtEvent_t > > GetPF_uid(const uint32_t& svcuid);
    ::DBus::Struct< std::vector< DbtEvent_t > > GetPF_tripleid(const uint32_t& onid, const uint32_t& tsid, const uint32_t& svcid);
    void SetIpEvent(const int64_t& uid, const std::vector< uint8_t >& data);
    void ResetIpEvent(const int64_t& uid);
    void ResetIpEventTable();
    void DeletIpEventByEndTime(const uint32_t& endtime);
    void SetIpSvc(const int64_t& uid, const std::vector< uint8_t >& data);
    void ResetIpSvc(const int64_t& uid);
    void ResetIpSvcTable();
    void SetIpLogo(const int64_t& uid, const std::vector< uint8_t >& data);
    void ResetIpLogo(const int64_t& uid);
    void ResetIpLogoTable();
    void SyncIpEventTable();
    void SyncIpSvcTable();
    void SyncIpLogoTable();
    void ResetTVTVEventTable();
    void ResetTVTVSvcTable();
    void SyncTVTVSvcTable();

private:


};

#endif
