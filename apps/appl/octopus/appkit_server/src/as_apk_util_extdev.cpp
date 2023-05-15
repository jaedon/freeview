#include <apk.h>


#include <vkernel.h>

#include <dbus-c++/dbus.h>
#include <as_apk_util_extdev.h>

char const * const cApkUtilExtdev::BUSNAME_APK_UTIL_EXTDEV = "Octopus.Appkit.Util.Extdev";
char const * const cApkUtilExtdev::BUSPATH_APK_UTIL_EXTDEV = "/Octopus/Appkit/Util/Extdev";

cApkUtilExtdev * cApkUtilExtdev::pMainThis	=	NULL;

void cApkUtilExtdev::callback_event (DxHotplug_NotiType_e eType, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	if (NULL == pMainThis)
		return;
	
	uint32_t type=0, param1=0, param2=0, param3=0;

	type = (uint32_t)eType;
	param1 = (uint32_t)nParam1;
	param2 = (uint32_t)nParam2;
	param3 = (uint32_t)nParam3;

	pMainThis->event (type, param1, param2, param3);					
}


cApkUtilExtdev::cApkUtilExtdev(DBus::Connection &connection)
  : DBus::ObjectAdaptor(connection, BUSPATH_APK_UTIL_EXTDEV)
{
	if (NULL == pMainThis)
		pMainThis = this;

	/* apkit util extdev */
    {	
		HERROR hErr = ERR_FAIL;
		hErr = APK_EXTDEV_Init ();
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("APK_EXTDEV_Init Error : %d", hErr);
		}

		APK_EXTDEV_RegisterNotifier (cApkUtilExtdev::callback_event);	
    }
}

void cApkUtilExtdev::MIC_Start()
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_EXTDEV_MIC_Start ();
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_EXTDEV_MIC_Start Error : %d", hErr);
	}

	return ;
}

void cApkUtilExtdev::MIC_Stop()
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_EXTDEV_MIC_Stop ();
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_EXTDEV_MIC_Stop Error : %d", hErr);
	}

	return ;
}

void cApkUtilExtdev::MIC_SetVolume(const int32_t& value)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_EXTDEV_MIC_SetVolume ((HINT32)value);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_EXTDEV_MIC_SetVolume Error : %d", hErr);
	}

	return ;
}

int32_t cApkUtilExtdev::MIC_GetVolume()
{
	HERROR hErr = ERR_FAIL;

	HINT32 nValue = 0;
	
	hErr = APK_EXTDEV_MIC_GetVolume (&nValue);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_EXTDEV_MIC_GetVolume Error : %d", hErr);
	}

	return (int32_t)nValue;
}

int32_t cApkUtilExtdev::MIC_GetCount()
{
	HERROR hErr = ERR_FAIL;

	HUINT32 nCnt = 0;

	hErr = APK_EXTDEV_MIC_GetCount (&nCnt);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_EXTDEV_MIC_GetCount Error : %d", hErr);
	}

	return (int32_t)nCnt;
}
