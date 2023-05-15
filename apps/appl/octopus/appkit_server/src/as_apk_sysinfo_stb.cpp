#include <apk.h>


#include <vkernel.h>

#include <dbus-c++/dbus.h>
#include <as_apk_sysinfo_stb.h>

#define MAX_BUFFSIZE	1024

char const * const cApkSysinfoStb::BUSNAME_APK_SYSINFO_STB = "Octopus.Appkit.Sysinfo.Stb";
char const * const cApkSysinfoStb::BUSPATH_APK_SYSINFO_STB = "/Octopus/Appkit/Sysinfo/Stb";

cApkSysinfoStb * cApkSysinfoStb::pMainThis	=	NULL;

void cApkSysinfoStb::callback_Event (void)
{
	if (NULL == pMainThis)
		return;

	pMainThis->event ();
}

cApkSysinfoStb::cApkSysinfoStb(DBus::Connection &connection)
  : DBus::ObjectAdaptor(connection, BUSPATH_APK_SYSINFO_STB)
{
	/* apkit sysinfo stb */
    {
		HERROR hErr = ERR_FAIL;
		hErr = APK_SYSINFO_STB_Init ();
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("APK_SYSINFO_STB_Init Error : %d", hErr);
		}

		//HERROR APK_SYSINFO_STB_DeInit(void);
		//HERROR APK_SYSINFO_STB_Initialized(void);

		hErr =APK_SYSINFO_STB_RegisterListener (cApkSysinfoStb::callback_Event);
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("APK_SYSINFO_STB_RegisterListener Error : %d", hErr);
		}

		//HERROR APK_SYSINFO_STB_UnregisterListener(APK_SYSINFO_STB_Notifier_t fnNotifier);
    }
}

std::string cApkSysinfoStb::GetModelName()
{
	HERROR hErr = ERR_FAIL;

	HCHAR szBuff[MAX_BUFFSIZE] = {0,};

	hErr = APK_SYSINFO_STB_GetModelName (szBuff, MAX_BUFFSIZE);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_SYSINFO_STB_GetModelName Error : %d", hErr);
	}

	return std::string(szBuff);
}

std::string cApkSysinfoStb::GetUpdatedList()
{
	HERROR hErr = ERR_FAIL;

	HCHAR szBuff[MAX_BUFFSIZE] = {0,};

	hErr = APK_SYSINFO_STB_GetUpdatedList (szBuff, MAX_BUFFSIZE);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_SYSINFO_STB_GetUpdatedList Error : %d", hErr);
	}

	return std::string(szBuff);
}

std::string cApkSysinfoStb::GetSoftwareVersion()
{
	HERROR hErr = ERR_FAIL;

	HCHAR szBuff[MAX_BUFFSIZE] = {0,};

	hErr = APK_SYSINFO_STB_GetSoftwareVersion (szBuff, MAX_BUFFSIZE);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_SYSINFO_STB_GetSoftwareVersion Error : %d", hErr);
	}

	return std::string(szBuff);
}

std::string cApkSysinfoStb::GetLoaderName()
{
	HERROR hErr = ERR_FAIL;

	HCHAR szBuff[MAX_BUFFSIZE] = {0,};

	hErr = APK_SYSINFO_STB_GetLoaderName (szBuff, MAX_BUFFSIZE);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_SYSINFO_STB_GetLoaderName Error : %d", hErr);
	}

	return std::string(szBuff);
}

std::string cApkSysinfoStb::GetLoaderVersion()
{
	HERROR hErr = ERR_FAIL;

	HCHAR szBuff[MAX_BUFFSIZE] = {0,};

	hErr = APK_SYSINFO_STB_GetLoaderVersion (szBuff, MAX_BUFFSIZE);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_SYSINFO_STB_GetLoaderVersion Error : %d", hErr);
	}

	return std::string(szBuff);
}

std::string cApkSysinfoStb::GetManufacturerName()
{
	HERROR hErr = ERR_FAIL;

	HCHAR szBuff[MAX_BUFFSIZE] = {0,};

	hErr = APK_SYSINFO_STB_GetManufacturerName (szBuff, MAX_BUFFSIZE);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_SYSINFO_STB_GetManufacturerName Error : %d", hErr);
	}

	return std::string(szBuff);
}

std::string cApkSysinfoStb::GetSystemId()
{
	HERROR hErr = ERR_FAIL;

	HCHAR szBuff[MAX_BUFFSIZE] = {0,};

	hErr = APK_SYSINFO_STB_GetSystemId (szBuff, MAX_BUFFSIZE);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_SYSINFO_STB_GetSystemId Error : %d", hErr);
	}

	return std::string(szBuff);
}

std::string cApkSysinfoStb::GetDeviceId()
{
	HERROR hErr = ERR_FAIL;

	HCHAR szBuff[MAX_BUFFSIZE] = {0,};

	hErr = APK_SYSINFO_STB_GetDeviceId (szBuff, MAX_BUFFSIZE);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_SYSINFO_STB_GetDeviceId Error : %d", hErr);
	}

	return std::string(szBuff);
}

std::string cApkSysinfoStb::GetChipId()
{
	HERROR hErr = ERR_FAIL;

	HCHAR szBuff[MAX_BUFFSIZE] = {0,};

	hErr = APK_SYSINFO_STB_GetChipId (szBuff, MAX_BUFFSIZE);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_SYSINFO_STB_GetChipId Error : %d", hErr);
	}

	return std::string(szBuff);
}

std::string cApkSysinfoStb::GetObamaVersion()
{
	HERROR hErr = ERR_FAIL;

	HCHAR szBuff[MAX_BUFFSIZE] = {0,};

	hErr = APK_SYSINFO_STB_GetObamaVersion (szBuff, MAX_BUFFSIZE);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_SYSINFO_STB_GetObamaVersion Error : %d", hErr);
	}

	return std::string(szBuff);
}

std::string cApkSysinfoStb::GetMicomVersion()
{
	HERROR hErr = ERR_FAIL;

	HCHAR szBuff[MAX_BUFFSIZE] = {0,};

	hErr = APK_SYSINFO_STB_GetMicomVersion (szBuff, MAX_BUFFSIZE);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_SYSINFO_STB_GetMicomVersion Error : %d", hErr);
	}

	return std::string(szBuff);
}

std::string cApkSysinfoStb::GetUpdateDate()
{
	HERROR hErr = ERR_FAIL;

	HCHAR szBuff[MAX_BUFFSIZE] = {0,};

	hErr = APK_SYSINFO_STB_GetUpdateDate (szBuff, MAX_BUFFSIZE);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_SYSINFO_STB_GetUpdateDate Error : %d", hErr);
	}

	return std::string(szBuff);
}

std::string cApkSysinfoStb::GetSerialNumber()
{
	HERROR hErr = ERR_FAIL;

	HCHAR szBuff[MAX_BUFFSIZE] = {0,};

	hErr = APK_SYSINFO_STB_GetSerialNumber (szBuff, MAX_BUFFSIZE);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_SYSINFO_STB_GetSerialNumber Error : %d", hErr);
	}

	return std::string(szBuff);
}

uint32_t cApkSysinfoStb::GetCustomCode()
{
	HERROR hErr = ERR_FAIL;

	HUINT32 ulCustomCode = 0;

	hErr = APK_SYSINFO_STB_GetCustomCode (&ulCustomCode);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_SYSINFO_STB_GetCustomCode Error : %d", hErr);
	}

	return (uint32_t)ulCustomCode;
}

bool cApkSysinfoStb::GetColdBootDone()
{
	HERROR hErr = ERR_FAIL;

	HBOOL bCheckColdBootDone = FALSE;

	hErr = APK_SYSINFO_STB_GetColdBootDone (&bCheckColdBootDone, NULL);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_SYSINFO_STB_GetColdBootDone Error : %d", hErr);
	}

	return (bool)bCheckColdBootDone;
}

void cApkSysinfoStb::SetFirstBootOnProduction(const bool& firstBootOnProduction)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_SYSINFO_STB_SetFirstBootOnProduction ((HBOOL)firstBootOnProduction);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_SYSINFO_STB_SetFirstBootOnProduction Error : %d", hErr);
	}

	return ;
}

bool cApkSysinfoStb::GetFirstBootOnProduction()
{
	HERROR hErr = ERR_FAIL;

	HBOOL bFirstBootOnProduction = FALSE;

	hErr = APK_SYSINFO_STB_GetFirstBootOnProduction ((HBOOL *)&bFirstBootOnProduction);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_SYSINFO_STB_GetFirstBootOnProduction Error : %d", hErr);
	}

	return (bool)bFirstBootOnProduction;
}

uint32_t cApkSysinfoStb::GetLaunchAppMode()
{
	HERROR hErr = ERR_FAIL;

	uint32_t ulLaunchAppMode = 0;

	hErr = APK_SYSINFO_STB_GetLaunchAppMode((HUINT32 *)&ulLaunchAppMode);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_SYSINFO_STB_GetLaunchAppMode Error : %d", hErr);
	}

	return ulLaunchAppMode;
}

void cApkSysinfoStb::SetAntennaPower(const bool& antPower)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_SYSINFO_STB_SetAntennaPower ((HBOOL)antPower);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_SYSINFO_STB_SetAntennaPower Error : %d", hErr);
	}

	return ;
}

bool cApkSysinfoStb::GetAntennaPower()
{
	HERROR hErr = ERR_FAIL;

	HBOOL bAntPower = FALSE;

	hErr = APK_SYSINFO_STB_GetAntennaPower ((HBOOL *)&bAntPower);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_SYSINFO_STB_GetAntennaPower Error : %d", hErr);
	}

	return (bool)bAntPower;
}

void cApkSysinfoStb::SetServiceUpdateFlag(const bool& enable)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_SYSINFO_STB_SetServiceUpdateFlag ((HBOOL)enable);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_SYSINFO_STB_SetServiceUpdateFlag Error : %d", hErr);
	}

	return ;
}

bool cApkSysinfoStb::GetServiceUpdateFlag()
{
	HERROR hErr = ERR_FAIL;

	HBOOL bOnOff	=	FALSE;

	hErr = APK_SYSINFO_STB_GetServiceUpdateFlag (&bOnOff);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_SYSINFO_STB_GetServiceUpdateFlag Error : %d", hErr);
	}

	return (bool)bOnOff;
}

::DBus::Struct< uint32_t, uint32_t, uint32_t > cApkSysinfoStb::GetNandBadBlocks()
{
	::DBus::Struct< uint32_t, uint32_t, uint32_t > reval;

	HERROR hErr = ERR_FAIL;
	HUINT32 ulCntLdr, ulCntUBI, ulCntUBIFS;

	hErr = APK_SYSINFO_STB_GetNandBadBlocks (&ulCntLdr, &ulCntUBI, &ulCntUBIFS);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_SYSINFO_STB_GetNandBadBlocks Error : %d", hErr);
	}

	{
		reval._1	=	(uint32_t)ulCntLdr;
		reval._2	=	(uint32_t)ulCntUBI;
		reval._3	=	(uint32_t)ulCntUBIFS;
	}

	return reval;
}

::DBus::Struct< uint8_t, uint8_t, uint8_t, uint8_t > cApkSysinfoStb::GetFusingInfo()
{
	::DBus::Struct< uint8_t, uint8_t, uint8_t, uint8_t > reval;

	HERROR hErr = ERR_FAIL;
	HUINT8 ucSCS, ucJTAG, ucOTP, ucCWE;

	hErr = APK_SYSINFO_STB_GetFusingInfo (&ucSCS, &ucJTAG, &ucOTP, &ucCWE);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_SYSINFO_STB_GetFusingInfo Error : %d", hErr);
	}

	{
		reval._1	=	(uint8_t)ucSCS;
		reval._2	=	(uint8_t)ucJTAG;
		reval._3	=	(uint8_t)ucOTP;
		reval._4	=	(uint8_t)ucCWE;
	}

	return reval;
}
