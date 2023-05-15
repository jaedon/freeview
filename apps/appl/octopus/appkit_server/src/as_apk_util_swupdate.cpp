#include <apk.h>


#include <vkernel.h>

#include <dbus-c++/dbus.h>

#include <dbus_util_cvt.h>
#include <as_apk_util_swupdate.h>

char const * const cApkUtilSwupdate::BUSNAME_APK_UTIL_SWUPDATE = "Octopus.Appkit.Util.Swupdate";
char const * const cApkUtilSwupdate::BUSPATH_APK_UTIL_SWUPDATE = "/Octopus/Appkit/Util/Swupdate";

cApkUtilSwupdate * cApkUtilSwupdate::pMainThis	=	NULL;

void cApkUtilSwupdate::callback_Event (DxSwUpdate_EventData_t *pstData)
{
	if (NULL == pMainThis)
		return;

	if (NULL != pstData)
	{
		pMainThis->event ((uint32_t)pstData->eEvent, 0, 0, 0);
	}
}

cApkUtilSwupdate::cApkUtilSwupdate(DBus::Connection &connection)
  : DBus::ObjectAdaptor(connection, BUSPATH_APK_UTIL_SWUPDATE)
{
	if (NULL == pMainThis)
		pMainThis = this;

	/* apkit util swupdate */
    {	
		HERROR hErr = ERR_FAIL;

		APK_SWUPDATE_Init ();
		//void APK_SWUPDATE_DeInit(void);

		hErr = APK_SWUPDATE_SwupdateInit (cApkUtilSwupdate::callback_Event);
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("APK_SWUPDATE_SwupdateInit Error : %d", hErr);
		}

    }
}

uint32_t cApkUtilSwupdate::Create(const uint32_t& swUpdateSpec)
{
	HERROR hErr = ERR_FAIL;

	Handle_t hSwupdate;
		
	hErr = APK_SWUPDATE_Create ((DxSwUpdate_Spec_e)swUpdateSpec, &hSwupdate, NULL);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_SWUPDATE_Create Error : %d", hErr);
	}

	return (uint32_t)hSwupdate;
}

uint32_t cApkUtilSwupdate::CreateManualConnect(const uint32_t& swUpdateSpec, const DbtTransponder_t& tunningInfo)
{
	HERROR hErr = ERR_FAIL;

	Handle_t hSwupdate;
	DxTransponder_t stTunningInfo;

	cDbusUtilCvt::C2DbtTransponder (tunningInfo, &stTunningInfo);
	
	hErr = APK_SWUPDATE_CreateManualConnect ((DxSwUpdate_Spec_e)swUpdateSpec, &hSwupdate, &stTunningInfo);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_SWUPDATE_CreateManualConnect Error : %d", hErr);
	}

	return hSwupdate;
}

void cApkUtilSwupdate::StartDetect(const uint32_t& handle)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_SWUPDATE_StartDetect (cApkUtilSwupdate::callback_Event, (Handle_t)handle);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_SWUPDATE_StartDetect Error : %d", hErr);
	}

	return ;
}

void cApkUtilSwupdate::StopDetect(const uint32_t& handle)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_SWUPDATE_StopDetect ((Handle_t)handle);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_SWUPDATE_StopDetect Error : %d", hErr);
	}

	return ;
}

void cApkUtilSwupdate::StartDownload(const uint32_t& handle)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_SWUPDATE_StartDownload (cApkUtilSwupdate::callback_Event, (Handle_t)handle);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_SWUPDATE_StartDownload Error : %d", hErr);
	}	

	return ;
}

void cApkUtilSwupdate::StopDownload(const uint32_t& handle)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_SWUPDATE_StopDownload ((Handle_t)handle);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_SWUPDATE_StopDownload Error : %d", hErr);
	}

	return ;
}

void cApkUtilSwupdate::StartInstall(const uint32_t& handle)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_SWUPDATE_StartInstall (cApkUtilSwupdate::callback_Event, (Handle_t)handle);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_SWUPDATE_StartInstall Error : %d", hErr);
	}

	return ;
}

void cApkUtilSwupdate::StopInstall(const uint32_t& handle)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_SWUPDATE_StopInstall ((Handle_t)handle);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_SWUPDATE_StopInstall Error : %d", hErr);
	}

	return ;
}

void cApkUtilSwupdate::Cancel(const uint32_t& handle)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_SWUPDATE_Cancel ((Handle_t)handle);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_SWUPDATE_Cancel Error : %d", hErr);
	}

	return ;
}
