#include <apk.h>


#include <vkernel.h>

#include <dbus-c++/dbus.h>

#include <dbus_util_cvt.h>
#include <as_apk_meta_query.h>

char const * const cApkMetaQuery::BUSNAME_APK_META_QUERY = "Octopus.Appkit.Meta.Query";
char const * const cApkMetaQuery::BUSPATH_APK_META_QUERY = "/Octopus/Appkit/Meta/Query";


cApkMetaQuery::cApkMetaQuery(DBus::Connection &connection)
  : DBus::ObjectAdaptor(connection, BUSPATH_APK_META_QUERY)
{
	/* apkit meta query */
    {
		APK_META_QUERY_Init (TRUE);
		//void APK_META_QUERY_DeInit(void);

		//void APK_EVENT_Delete (DxEvent_t *event);
		//DxEvent_t * APK_EVENT_Clone (DxEvent_t *event);
		//DxEvent_t * APK_EVENT_Incref (DxEvent_t *event);
		//void APK_META_QUERY_SetNotifier (ApkMetaQHandle_t self, APK_META_QUERY_Notifier_t fnNotifier, void *userdata);
    }
}

uint32_t cApkMetaQuery::NewQuery(const std::string& field, const std::string& comparision, const std::string& value)
{
	APK_QUERY_t pvQueryHandle;

	pvQueryHandle = APK_META_QUERY_NewQuery ((const HCHAR *)&field[0], (const HCHAR *)&comparision[0], (const HCHAR *)&value[0]);

	return (uint32_t)pvQueryHandle;
}

void cApkMetaQuery::DeleteQuery(const uint32_t& queryHandle)
{
	APK_META_QUERY_DeleteQuery ((APK_QUERY_t)queryHandle);

	return ;
}

uint32_t cApkMetaQuery::CloneQuery(const uint32_t& queryHandle)
{
	APK_QUERY_t pvQueryHandle;

	pvQueryHandle = APK_META_QUERY_CloneQuery ((const APK_QUERY_t)queryHandle);

	return (uint32_t)pvQueryHandle;
}

uint32_t cApkMetaQuery::And(const uint32_t& queryHandle1, const uint32_t& queryHandle2)
{
	APK_QUERY_t pvQueryHandle;

	pvQueryHandle = APK_META_QUERY_And ((const APK_QUERY_t )queryHandle1, (const APK_QUERY_t)queryHandle2);

	return (uint32_t)pvQueryHandle;
}

uint32_t cApkMetaQuery::Or(const uint32_t& queryHandle1, const uint32_t& queryHandle2)
{
	APK_QUERY_t pvQueryHandle;

	pvQueryHandle = APK_META_QUERY_Or ((const APK_QUERY_t )queryHandle1, (const APK_QUERY_t)queryHandle2);

	return (uint32_t)pvQueryHandle;
}

uint32_t cApkMetaQuery::Not(const uint32_t& queryHandle)
{
	APK_QUERY_t pvQueryHandle;

	pvQueryHandle = APK_META_QUERY_Not ((const APK_QUERY_t )queryHandle);

	return (uint32_t)pvQueryHandle;
}

uint32_t cApkMetaQuery::New(const uint32_t& target)
{
	ApkMetaQHandle_t pstQuery = NULL;

	pstQuery = APK_META_QUERY_New ((APK_META_QUERY_Target_e)target);

	return (uint32_t)pstQuery;
}

void cApkMetaQuery::Delete(const uint32_t& metaQhandle)
{
	APK_META_QUERY_Delete ((ApkMetaQHandle_t)metaQhandle);

	return ;
}

void cApkMetaQuery::SetQuery(const uint32_t& metaQhandle, const uint32_t& queryHandle)
{
	HERROR hErr = ERR_FAIL;

	hErr = APK_META_QUERY_SetQuery ((ApkMetaQHandle_t)metaQhandle, (const APK_QUERY_t)queryHandle);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_META_QUERY_SetQuery Error : %d", hErr);
	}

	return ;
}

uint32_t cApkMetaQuery::GetQuery(const uint32_t& metaQhandle)
{
	APK_QUERY_t pvQueryHandle;

	pvQueryHandle = APK_META_QUERY_GetQuery ((ApkMetaQHandle_t)metaQhandle);

	return (uint32_t)pvQueryHandle;
}

void cApkMetaQuery::AddSvcConstraint_uid(const uint32_t& metaQhandle, const uint32_t& svcuid)
{
#if defined(CONFIG_FUNC_EPG_USE_SVCUID)
	HERROR hErr = ERR_FAIL;

	hErr = APK_META_QUERY_AddSvcConstraint ((ApkMetaQHandle_t)metaQhandle, (HINT32)svcuid);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_META_QUERY_AddSvcConstraint Error : %d", hErr);
	}
#endif
	return ;
}

void cApkMetaQuery::AddSvcConstraint_tripleid(const uint32_t& metaQhandle, const uint32_t& onid, const uint32_t& tsid, const uint32_t& svcid)
{
#if !defined(CONFIG_FUNC_EPG_USE_SVCUID)
	HERROR hErr = ERR_FAIL;

	hErr = APK_META_QUERY_AddSvcConstraint ((ApkMetaQHandle_t)metaQhandle, (HINT32)onid, (HINT32)tsid, (HINT32)svcid);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_META_QUERY_AddSvcConstraint Error : %d", hErr);
	}
#endif
	return ;
}

void cApkMetaQuery::ClearSvcConstraint(const uint32_t& metaQhandle)
{
	APK_META_QUERY_ClearSvcConstraint ((ApkMetaQHandle_t)metaQhandle);

	return ;
}

void cApkMetaQuery::OrderBy(const uint32_t& metaQhandle, const std::string& field, const bool& ascending)
{
	APK_META_QUERY_OrderBy ((ApkMetaQHandle_t)metaQhandle, (const HCHAR *)&field[0], (HBOOL)ascending);

	return ;
}

void cApkMetaQuery::SetFilter(const uint32_t& metaQhandle, const uint32_t& filter)
{
	APK_META_QUERY_SetFilter ((ApkMetaQHandle_t)metaQhandle,(APK_META_QUERY_Filter_e)filter);

	return ;
}

void cApkMetaQuery::Search(const uint32_t& metaQhandle, const bool& direct)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_META_QUERY_Search ((ApkMetaQHandle_t)metaQhandle, (HBOOL)direct);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_META_QUERY_Search Error : %d", hErr);
	}

	return ;
}

void cApkMetaQuery::Abort(const uint32_t& metaQhandle)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_META_QUERY_Abort ((ApkMetaQHandle_t)metaQhandle);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_META_QUERY_Abort Error : %d", hErr);
	}

	return ;
}

std::vector< DbtEvent_t > cApkMetaQuery::Result(const uint32_t& metaQhandle)
{
	std::vector< DbtEvent_t >	reval;

	HxVector_t *pList = NULL;
	pList = APK_META_QUERY_Result ((ApkMetaQHandle_t)metaQhandle);
	if (NULL != pList)
	{
		HINT32 i;
		HINT32 nListCnt = HLIB_VECTOR_Length(pList);
		for (i = 0 ; i < nListCnt ; i++)
		{
			DxEvent_t *pstEvent = (DxEvent_t *)HLIB_VECTOR_ItemAt(pList, i);
			if (NULL != pstEvent)
			{
				DbtEvent_t item;
				item = cDbusUtilCvt::C2DxEvent (pstEvent);
				reval.push_back (item);
			}
		}
		HLIB_VECTOR_Delete(pList);
	}

	return reval;
}

DbtEvent_t cApkMetaQuery::GetEvent_uid(const uint32_t& type, const uint32_t& svcuid)
{
	DbtEvent_t reval;

#if defined(CONFIG_FUNC_EPG_USE_SVCUID)
	DxEvent_t * pstEvent = NULL;

	pstEvent = APK_META_QUERY_GetEvent ((DxEPG_Type_e)type, (HUID)svcuid, NULL, NULL);

	reval = cDbusUtilCvt::C2DxEvent (pstEvent);
#endif
	return reval;
}

DbtEvent_t cApkMetaQuery::GetEvent_tripleid(const uint32_t& type, const uint32_t& onid, const uint32_t& tsid, const uint32_t& svcid)
{
	DbtEvent_t reval;

#if !defined(CONFIG_FUNC_EPG_USE_SVCUID)
	DxEvent_t * pstEvent = NULL;

	pstEvent = APK_META_QUERY_GetEvent ((DxEPG_Type_e)type, (HINT32)onid, (HINT32)tsid, (HINT32)svcid, (HINT32)eventid, NULL, NULL);

	reval = cDbusUtilCvt::C2DxEvent (pstEvent);
#endif
	return reval;
}

DbtEvent_t cApkMetaQuery::GetEventWithIds_uid(const uint32_t& svcuid, const uint32_t& eventid)
{
	DbtEvent_t reval;

#if defined(CONFIG_FUNC_EPG_USE_SVCUID)
	DxEvent_t * pstEvent = NULL;

	pstEvent = APK_META_QUERY_GetEventWithIds ((HINT32)svcuid, (HINT32)eventid);

	reval = cDbusUtilCvt::C2DxEvent (pstEvent);
#endif
	return reval;
}

DbtEvent_t cApkMetaQuery::GetEventWithIds_tripleid(const uint32_t& onid, const uint32_t& tsid, const uint32_t& svcid, const uint32_t& eventid)
{
	DbtEvent_t reval;

#if !defined(CONFIG_FUNC_EPG_USE_SVCUID)
	DxEvent_t * pstEvent = NULL;

	pstEvent = APK_META_QUERY_GetEventWithIds ((HINT32)onid, (HINT32)tsid, (HINT32)svcid, (HINT32)eventid);

	reval = cDbusUtilCvt::C2DxEvent (pstEvent);
#endif

	return reval;
}

DbtEvent_t cApkMetaQuery::GetIpEvent_uid(const uint32_t& svcuid, const uint32_t& eventid)
{
	DbtEvent_t reval;

#if defined(CONFIG_FUNC_EPG_USE_SVCUID)
	DxEvent_t * pstEvent = NULL;

	pstEvent = APK_META_QUERY_GetIpEvent ((HINT32)svcuid, (HINT32)eventid);

	reval = cDbusUtilCvt::C2DxEvent (pstEvent);
#endif

	return reval;
}

DbtEvent_t cApkMetaQuery::GetIpEvent_tripleid(const uint32_t& onid, const uint32_t& tsid, const uint32_t& svcid, const uint32_t& eventid)
{
	DbtEvent_t reval;

#if !defined(CONFIG_FUNC_EPG_USE_SVCUID)
	DxEvent_t * pstEvent = NULL;

	pstEvent = APK_META_QUERY_GetIpEvent ((HINT32)onid, (HINT32)tsid, (HINT32)svcid, (HINT32)eventid);

	reval = cDbusUtilCvt::C2DxEvent (pstEvent);
#endif

	return reval;
}

::DBus::Struct< std::vector< DbtEvent_t > > cApkMetaQuery::GetPF_uid(const uint32_t& svcuid)
{
	::DBus::Struct< std::vector< DbtEvent_t > > reval;

#if defined(CONFIG_FUNC_EPG_USE_SVCUID)
//	HxVector_t * APK_META_QUERY_GetPF_uid(HINT32 svcuid);
#endif

	return reval;
}

::DBus::Struct< std::vector< DbtEvent_t > > cApkMetaQuery::GetPF_tripleid(const uint32_t& onid, const uint32_t& tsid, const uint32_t& svcid)
{
	::DBus::Struct< std::vector< DbtEvent_t > > reval;

#if !defined(CONFIG_FUNC_EPG_USE_SVCUID)
//	HxVector_t * APK_META_QUERY_GetPF_tripleid (HINT32 onid, HINT32 tsid, HINT32 svcid);
#endif

	return reval;
}

void cApkMetaQuery::SetIpEvent(const int64_t& uid, const std::vector< uint8_t >& data)
{
	HERROR hErr = ERR_FAIL;

	hErr = APK_META_QUERY_SetIpEvent ((HUID)uid, (void *)&data[0], (HINT32)data.size());
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_META_QUERY_SetIpEvent Error : %d", hErr);
	}

	return ;
}

void cApkMetaQuery::ResetIpEvent(const int64_t& uid)
{
	HERROR hErr = ERR_FAIL;

	hErr = APK_META_QUERY_ResetIpEvent ((HUID)uid);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_META_QUERY_ResetIpEvent Error : %d", hErr);
	}

	return ;
}

void cApkMetaQuery::ResetIpEventTable()
{
	HERROR hErr = ERR_FAIL;

	hErr = APK_META_QUERY_ResetIpEventTable ();
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_META_QUERY_ResetIpEventTable Error : %d", hErr);
	}

	return ;
}

void cApkMetaQuery::DeletIpEventByEndTime(const uint32_t& endtime)
{
	HERROR hErr = ERR_FAIL;

	hErr = APK_META_QUERY_DeletIpEventByEndTime ((HUINT32)endtime);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_META_QUERY_DeletIpEventByEndTime Error : %d", hErr);
	}

	return ;
}

void cApkMetaQuery::SetIpSvc(const int64_t& uid, const std::vector< uint8_t >& data)
{
	HERROR hErr = ERR_FAIL;

	hErr = APK_META_QUERY_SetIpSvc ((HUID)uid, (void *)&data[0], (HINT32)data.size());
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_META_QUERY_SetIpSvc Error : %d", hErr);
	}

	return ;
}

void cApkMetaQuery::ResetIpSvc(const int64_t& uid)
{
	HERROR hErr = ERR_FAIL;

	hErr = APK_META_QUERY_ResetIpSvc ((HUID)uid);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_META_QUERY_ResetIpSvc Error : %d", hErr);
	}

	return ;
}

void cApkMetaQuery::ResetIpSvcTable()
{
	HERROR hErr = ERR_FAIL;

	hErr = APK_META_QUERY_ResetIpSvcTable ();
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_META_QUERY_ResetIpSvcTable Error : %d", hErr);
	}

	return ;
}

void cApkMetaQuery::SetIpLogo(const int64_t& uid, const std::vector< uint8_t >& data)
{
	HERROR hErr = ERR_FAIL;

	hErr = APK_META_QUERY_SetIpLogo ((HUID)uid, (void *)&data[0], (HINT32)data.size());
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_META_QUERY_ResetIpSvcTable Error : %d", hErr);
	}

	return ;
}

void cApkMetaQuery::ResetIpLogo(const int64_t& uid)
{
	HERROR hErr = ERR_FAIL;

	hErr = APK_META_QUERY_ResetIpLogo ((HUID)uid);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_META_QUERY_ResetIpSvcTable Error : %d", hErr);
	}

	return ;
}

void cApkMetaQuery::ResetIpLogoTable()
{
	HERROR hErr = ERR_FAIL;

	hErr = APK_META_QUERY_ResetIpLogoTable ();
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_META_QUERY_ResetIpLogoTable Error : %d", hErr);
	}

	return ;
}

void cApkMetaQuery::SyncIpEventTable()
{
	HERROR hErr = ERR_FAIL;

	hErr = APK_META_QUERY_SyncIpEventTable ();
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_META_QUERY_SyncIpEventTable Error : %d", hErr);
	}

	return ;
}

void cApkMetaQuery::SyncIpSvcTable()
{
	HERROR hErr = ERR_FAIL;

	hErr = APK_META_QUERY_SyncIpSvcTable ();
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_META_QUERY_SyncIpSvcTable Error : %d", hErr);
	}

	return ;
}

void cApkMetaQuery::SyncIpLogoTable()
{
	HERROR hErr = ERR_FAIL;

	hErr = APK_META_QUERY_SyncIpLogoTable ();
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_META_QUERY_SyncIpLogoTable Error : %d", hErr);
	}

	return ;
}

void cApkMetaQuery::ResetTVTVEventTable()
{
	HERROR hErr = ERR_FAIL;

	hErr = APK_META_QUERY_ResetTVTVEventTable ();
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_META_QUERY_ResetTVTVEventTable Error : %d", hErr);
	}

	return ;
}

void cApkMetaQuery::ResetTVTVSvcTable()
{
	HERROR hErr = ERR_FAIL;

	hErr = APK_META_QUERY_ResetTVTVSvcTable ();
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_META_QUERY_ResetTVTVSvcTable Error : %d", hErr);
	}

	return ;
}

void cApkMetaQuery::SyncTVTVSvcTable()
{
	HERROR hErr = ERR_FAIL;

	hErr = APK_META_QUERY_SyncTVTVSvcTable ();
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_META_QUERY_SyncTVTVSvcTable Error : %d", hErr);
	}

	return ;
}
