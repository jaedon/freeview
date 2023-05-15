#include <apk.h>


#include <vkernel.h>

#include <dbus-c++/dbus.h>
#include <as_apk_filemgr_browse.h>

char const * const cApkFilemgrBrowse::BUSNAME_APK_FILEMGR_BROWSE = "Octopus.Appkit.Filemgr.Browse";
char const * const cApkFilemgrBrowse::BUSPATH_APK_FILEMGR_BROWSE = "/Octopus/Appkit/Filemgr/Browse";

cApkFilemgrBrowse * cApkFilemgrBrowse::pMainThis	=	NULL;

void cApkFilemgrBrowse::callback_Event(HUINT32 nHandleId, HINT32 eEvent, HINT32 nResult, void *pvData)
{
	if (NULL == pMainThis)
		return;
	
	uint32_t handleId=0, event=0;
	int32_t result=0;

	handleId = (uint32_t)nHandleId;
	event = (uint32_t)eEvent;
	result = (int32_t)result;

	pMainThis->event (handleId, event, result);
}

cApkFilemgrBrowse::cApkFilemgrBrowse(DBus::Connection &connection)
  : DBus::ObjectAdaptor(connection, BUSPATH_APK_FILEMGR_BROWSE)
{
	if (NULL == pMainThis)
		pMainThis = this;

	/* apkit filemgr browse */
    {	
    	HERROR hErr = ERR_FAIL;
		hErr = APK_FILEMGR_BROWSE_RegisterCallback (cApkFilemgrBrowse::callback_Event);
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("APK_FILEMGR_BROWSE_RegisterCallback Error : %d", hErr);
		}
		//HERROR APK_FILEMGR_BROWSE_UnregisterCallback(APK_FILEMGR_BROWSE_Notifier_t fnNotifier);
    }
}

uint32_t cApkFilemgrBrowse::Open(const std::string& path, const uint32_t& category, const std::string& extFilter, const uint32_t& optFlags)
{
	HUINT32 ulResult = 0;

	ulResult = APK_FILEMGR_BROWSE_Open ((HCHAR *)&path[0], (HUINT32)category, (HCHAR *)&extFilter[0], (HUINT32)optFlags);
	return ulResult;
}

void cApkFilemgrBrowse::Close(const uint32_t& handle)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_FILEMGR_BROWSE_Close ((HUINT32)handle);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_FILEMGR_BROWSE_Close Error : %d", hErr);
	}

	return ;
}

void cApkFilemgrBrowse::GetItems(const uint32_t& handle, const uint32_t& index, const uint32_t& reqCount, const int32_t& option)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_FILEMGR_BROWSE_GetItems ((HUINT32)handle, (HUINT32)index, (HUINT32)reqCount, (HINT32)option);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_FILEMGR_BROWSE_GetItems Error : %d", hErr);
	}

	return ;
}

void cApkFilemgrBrowse::ScanItems(const uint32_t& handle, const uint32_t& reqCount, const int32_t& option)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_FILEMGR_BROWSE_ScanItems ((HUINT32)handle, (HUINT32)reqCount, (HINT32)option);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_FILEMGR_BROWSE_ScanItems Error : %d", hErr);
	}

	return ;
}

void cApkFilemgrBrowse::Sort(const uint32_t& handle, const uint32_t& sort)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_FILEMGR_BROWSE_Sort ((HUINT32)handle, (HUINT32)sort);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_FILEMGR_BROWSE_Sort Error : %d", hErr);
	}

	return ;
}
