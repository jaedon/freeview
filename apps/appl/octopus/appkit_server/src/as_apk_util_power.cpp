#include <apk.h>


#include <vkernel.h>

#include <dbus-c++/dbus.h>

#include <dbus_util_cvt.h>
#include <as_apk_util_power.h>

char const * const cApkUtilPower::BUSNAME_APK_UTIL_POWER = "Octopus.Appkit.Util.Power";
char const * const cApkUtilPower::BUSPATH_APK_UTIL_POWER = "/Octopus/Appkit/Util/Power";

cApkUtilPower * cApkUtilPower::pMainThis	=	NULL;

void cApkUtilPower::callback_Event (HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3)
{
	if (NULL == pMainThis)
		return;

	uint32_t param1=0, param2=0, param3=0;

	/* emit signal */
	{
		param1 = (uint32_t)ulParam1;
		param2 = (uint32_t)ulParam2;
		param3 = (uint32_t)ulParam3;
	}

	pMainThis->event (param1, param2, param3);					
}

cApkUtilPower::cApkUtilPower(DBus::Connection &connection)
  : DBus::ObjectAdaptor(connection, BUSPATH_APK_UTIL_POWER)
{
	if (NULL == pMainThis)
		pMainThis = this;

	/* apkit util power */
    {	
		HERROR hErr = ERR_FAIL;
		hErr = APK_POWER_Init ();
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("APK_POWER_Init Error : %d", hErr);
		}

		APK_POWER_RegisterNotifier (cApkUtilPower::callback_Event);
    }
}

uint32_t cApkUtilPower::GetWakeupReason()
{
	HERROR hErr = ERR_FAIL;

	DxWakeUpReason_e eWakeupReason	=	eDxWAKEUP_NONE;
		
	hErr = APK_POWER_GetWakeupReason (&eWakeupReason);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_POWER_GetWakeupReason Error : %d", hErr);
	}

	return (uint32_t)eWakeupReason;
}

uint32_t cApkUtilPower::GetWakeupSecondReason()
{
	HERROR hErr = ERR_FAIL;

	DxWakeUp2rdReason_e eWakeupSecondReason	=	eDxWAKEUP_2RD_UNDECIED;
		
	hErr = APK_POWER_GetWakeupSecondReason (&eWakeupSecondReason);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_POWER_GetWakeupSecondReason Error : %d", hErr);
	}

	return (uint32_t)eWakeupSecondReason;
}

void cApkUtilPower::SetStandbyPowerMode(const uint32_t& powerMode)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_POWER_SetStandbyPowerMode ((APK_POWER_BoxStandbyType_e)powerMode);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_POWER_SetStandbyPowerMode Error : %d", hErr);
	}

	return ;
}

uint32_t cApkUtilPower::GetStandbyPowerMode()
{
	HERROR hErr = ERR_FAIL;

	APK_POWER_BoxStandbyType_e ePowerMode	=	eAPK_POWER_STANDBYTYPE_ASITWAS;
		
	hErr = APK_POWER_GetStandbyPowerMode (&ePowerMode);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_POWER_Init Error : %d", hErr);
	}

	return (uint32_t)ePowerMode;
}

bool cApkUtilPower::IsStandbyMode()
{
	return (bool)APK_POWER_IsStandbyMode ();
}

bool cApkUtilPower::GoingToReboot()
{
	return (bool)APK_POWER_GoingToReboot ();
}

bool cApkUtilPower::GoingToShutdown()
{
	return (bool)APK_POWER_GoingToShutdown ();
}

bool cApkUtilPower::GoingToOperate()
{
	return (bool)APK_POWER_GoingToOperate ();
}

void cApkUtilPower::RequestShutDown()
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_POWER_RequestShutDown ();
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_POWER_RequestReboot Error : %d", hErr);
	}

	return ;
}

void cApkUtilPower::RequestReboot()
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_POWER_RequestReboot ();
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_POWER_RequestReboot Error : %d", hErr);
	}

	return ;
}

void cApkUtilPower::RequestOperation()
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_POWER_RequestOperation ();
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_POWER_RequestOperation Error : %d", hErr);
	}

	return;
}

void cApkUtilPower::WaitShutDown(const uint32_t& waitTime)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_POWER_WaitShutDown ((HUINT32)waitTime);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_POWER_WaitShutDown Error : %d", hErr);
	}

	return ;
}
