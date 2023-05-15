#include <apk.h>

#include <vkernel.h>

#include <dbus-c++/dbus.h>

#include <dbus_util_cvt.h>
#include <as_apk_meta_record.h>

char const * const cApkMetaRecord::BUSNAME_APK_META_RECORD = "Octopus.Appkit.Meta.Record";
char const * const cApkMetaRecord::BUSPATH_APK_META_RECORD = "/Octopus/Appkit/Meta/Record";


cApkMetaRecord::cApkMetaRecord(DBus::Connection &connection)
  : DBus::ObjectAdaptor(connection, BUSPATH_APK_META_RECORD)
{
	/* apkit meta record */
    {
		APK_META_RECORD_Init();
    }
}

uint32_t cApkMetaRecord::GetCID(const std::string& url)
{
	HUINT32 cid;

	cid = APK_META_RECORD_GetCID (&url[0]);

	return cid;
}

uint32_t cApkMetaRecord::Count()
{
	HUINT32 count;

	count = APK_META_RECORD_Count ();

	return count;
}

DbtRecListData_t cApkMetaRecord::GetByIdx(const uint32_t& index)
{
	DbtRecListData_t reval;
	DxRecListData_t* pRecData = NULL;	

	pRecData = APK_META_RECORD_GetByIdx ((const HUINT32)index);

	reval = cDbusUtilCvt::C2DxRecListData (pRecData);
	return reval;
}

DbtRecListData_t cApkMetaRecord::GetByCID(const uint32_t& cid)
{
	DbtRecListData_t reval;
	DxRecListData_t* pRecData = NULL;

	pRecData = APK_META_RECORD_GetByCID ((const HUINT32)cid);

	reval = cDbusUtilCvt::C2DxRecListData (pRecData);
	return reval;
}

uint32_t cApkMetaRecord::GetContentsIdByUrl(const std::string& url)
{
	uint32_t cid;

	cid = APK_META_RECORD_GetContentsIdByUrl ((const HCHAR*)&url[0]);

	return cid;
}

void cApkMetaRecord::Release(const DbtRecListData_t& recData)
{
	DxRecListData_t stRecData;
	HxSTD_memset (&stRecData, 0, sizeof(DxRecListData_t));

	cDbusUtilCvt::C2DbtRecListData (recData, &stRecData);

	APK_META_RECORD_Release (&stRecData);

	return;
}

void cApkMetaRecord::GetItems(const bool& direct)
{
	HERROR	hErr;
	HxVector_t *vector = NULL;

	vector = APK_META_RECORD_GetItems ();
	if (NULL != vector)
	{
		HxLOG_Error ("APK_META_RECORD_GetItems : Error");
	}

	return ;
}

uint32_t cApkMetaRecord::Move(const uint32_t& cid, const std::string& path, const bool& isDecrypt)
{
	HERROR	hErr; 

	hErr = APK_META_RECORD_Move ( (HUINT32)cid, (const HCHAR*)&path[0], (HBOOL)isDecrypt, NULL, NULL);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_META_RECORD_Move : Error");
	}

	return 0;
}

void cApkMetaRecord::CancelFileOperation(const uint32_t& cid)
{
	APK_META_RECORD_CancelFileOperation ( (HUINT32)cid );

	return;
}

void cApkMetaRecord::Scan(const std::string& url)
{
	return ;
}

void cApkMetaRecord::Copy(const uint32_t& cid, const std::string& path, const bool& decrypt)
{
	HERROR	hErr;

	hErr = APK_META_RECORD_Copy ( (HUINT32)cid, (const HCHAR*)&path[0], (HBOOL)decrypt, NULL, NULL);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_META_RECORD_Copy : Error");
	}

	return;
}

void cApkMetaRecord::Cancel()
{
	HERROR	hErr;

	hErr = APK_META_RECORD_Cancel();
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_META_RECORD_Cancel : Error");
	}

	return ;
}

void cApkMetaRecord::Clear()
{
	HERROR	hErr;

	hErr = APK_META_RECORD_Clear();
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_META_RECORD_Clear : Error");
	}

	return ;

}


//HERROR	APK_META_RECORD_Scan(HCHAR	*url, HBOOL	bSyncmode);
//HERROR	APK_META_RECORD_Lock(APK_META_REC_LOCK_t	*pstLock, HBOOL	bSyncmode);
//HERROR	APK_META_RECORD_SetSeriesLock(HCHAR	*pSeriesID, HBOOL	bSeriesLock, HBOOL	bSyncmode);
//HERROR	APK_META_RECORD_Rename(APK_META_REC_RENAME_t	*pstRename, HBOOL	bSyncmode);
//HERROR	APK_META_RECORD_Update(APK_META_REC_UPDATE_t	*pstUpdate, HBOOL	bSyncmode);
//HERROR	APK_META_RECORD_StopRecording(APK_META_REC_STOP_REC_t	*pstStopRec, HBOOL	bSyncmode);

//HERROR	APK_META_RECORD_Delete(APK_META_REC_DELETE_t	*pstDelete, HBOOL	bSyncmode);
//HERROR	APK_META_RECORD_AddBookmark(APK_META_REC_BOOKMARK_t		*pstBookmark, HBOOL	bSyncmode);
//HERROR	APK_META_RECORD_RemoveBookmark(APK_META_REC_BOOKMARK_t	*pstBookmark, HBOOL	bSyncmode);

