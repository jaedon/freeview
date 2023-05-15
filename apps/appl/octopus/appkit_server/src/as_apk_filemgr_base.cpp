#include <apk.h>


#include <vkernel.h>

#include <dbus-c++/dbus.h>
#include <as_apk_filemgr_base.h>

char const * const cApkFilemgrBase::BUSNAME_APK_FILEMGR_BASE = "Octopus.Appkit.Filemgr.Base";
char const * const cApkFilemgrBase::BUSPATH_APK_FILEMGR_BASE = "/Octopus/Appkit/Filemgr/Base";

void cApkFilemgrBase::api_callback(int requestId, void *pvUserData, HxList_t *pstResultList, void *pvCbInfo)
{

}


cApkFilemgrBase::cApkFilemgrBase(DBus::Connection &connection)
  : DBus::ObjectAdaptor(connection, BUSPATH_APK_FILEMGR_BASE)
{
	/* apkit filemgr base */
    {	
		HERROR hErr = ERR_FAIL;
		hErr = APK_FILEMGR_Init();
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("APK_FILEMGR_Init Error : %d", hErr);
		}
    }
}


int32_t cApkFilemgrBase::BrowserFiles(const std::string& path, const std::string& filter, const std::string& keyword, const uint32_t& browsingMode)
{
	HERROR hErr = ERR_FAIL;

	HINT32 nTicket = 0;

	hErr = APK_FILEMGR_BrowserFiles ((const HCHAR *)&path[0], (const HCHAR *)&filter[0], (const HCHAR *)&keyword[0], (HUINT32)browsingMode, cApkFilemgrBase::api_callback, &nTicket);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_FILEMGR_BrowserFiles Error : %d", hErr);
	}

	return (int32_t)nTicket; //??? event signal 의 reqId 용도로 맞는지 확인 필!
}

int32_t cApkFilemgrBase::ScanDir(const std::string& source, const std::string& extFilter, const bool& recursive)
{
	HERROR hErr = ERR_FAIL;

	HINT32 nRequestId = 0;
		
	hErr = APK_FILEMGR_ScanDir ((const HCHAR *)&source[0], (const HCHAR *)&extFilter[0], (HBOOL)recursive, cApkFilemgrBase::api_callback, &nRequestId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_FILEMGR_ScanDir Error : %d", hErr);
	}

	return (int32_t)nRequestId;
}

int32_t cApkFilemgrBase::Remove(const std::string& source, const std::string& extFilter, const bool& recursive)
{
	HERROR hErr = ERR_FAIL;

	HINT32 nRequestId = 0;
		
	hErr = APK_FILEMGR_Remove ((const HCHAR *)&source[0], (const HCHAR *)&extFilter[0], (HBOOL)recursive, cApkFilemgrBase::api_callback, &nRequestId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_FILEMGR_Remove Error : %d", hErr);
	}

	return nRequestId;
}

int32_t cApkFilemgrBase::Copy(const std::string& source, const std::string& dest, const std::string& extFilter, const bool& decryption)
{
	HERROR hErr = ERR_FAIL;

	HINT32 nRequestId = 0;

	hErr = APK_FILEMGR_Copy ((const HCHAR *)&source[0], (const HCHAR *)&dest[0], (const HCHAR *)&extFilter[0], (HBOOL)decryption, cApkFilemgrBase::api_callback, &nRequestId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_FILEMGR_Copy Error : %d", hErr);
	}

	return nRequestId;
}

int32_t cApkFilemgrBase::Move(const std::string& source, const std::string& dest, const std::string& extFilter, const bool& decryption)
{
	HERROR hErr = ERR_FAIL;

	HINT32 nRequestId = 0;

	hErr = APK_FILEMGR_Move ((const HCHAR *)&source[0], (const HCHAR *)&dest[0], (const HCHAR *)&extFilter[0], (HBOOL)decryption, cApkFilemgrBase::api_callback, &nRequestId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_FILEMGR_Move Error : %d", hErr);
	}

	return nRequestId;
}

int32_t cApkFilemgrBase::MakeDir(const std::string& source)
{
	HERROR hErr = ERR_FAIL;

	HINT32 nRequestId = 0;

	hErr = APK_FILEMGR_MakeDir ((const HCHAR *)&source[0], cApkFilemgrBase::api_callback, &nRequestId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_FILEMGR_MakeDir Error : %d", hErr);
	}

	return nRequestId;
}

int32_t cApkFilemgrBase::RemoveDir(const std::string& source)
{
	HERROR hErr = ERR_FAIL;

	HINT32 nRequestId = 0;

	hErr = APK_FILEMGR_RemoveDir ((const HCHAR *)&source[0], cApkFilemgrBase::api_callback, &nRequestId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_FILEMGR_RemoveDir Error : %d", hErr);
	}

	return nRequestId;
}

int32_t cApkFilemgrBase::Cancel(const int32_t& cancleRequestId)
{
	HERROR hErr = ERR_FAIL;

	HINT32 nRequestId = 0;

	hErr = APK_FILEMGR_Cancel ((HINT32)cancleRequestId, cApkFilemgrBase::api_callback, &nRequestId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_FILEMGR_Cancel Error : %d", hErr);
	}

	return nRequestId;
}

uint64_t cApkFilemgrBase::GetSizeOfPath(const std::string& path, const std::string& extFilter)
{
	HERROR hErr = ERR_FAIL;

	HUINT64 ulSize = 0;
		
	hErr = APK_FILEMGR_GetSizeOfPath ((const HCHAR *)&path[0], (const HCHAR *)&extFilter[0], &ulSize);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_FILEMGR_GetSizeOfPath Error : %d", hErr);
	}

	return (uint64_t)ulSize;
}

/*
hErr = APK_FILEMGR_AddListener(HINT32 nRequestId, void *pvUser, APK_FILEMGR_Notifier_t fnNotifier);
if (ERR_OK != hErr)
{
	HxLOG_Error ("APK_FILEMGR_AddListener Error : %d", hErr);
}
hErr = APK_FILEMGR_FreeResultList(HxList_t *pstResultList);
if (ERR_OK != hErr)
{
	HxLOG_Error ("APK_FILEMGR_FreeResultList Error : %d", hErr);
}
*/
