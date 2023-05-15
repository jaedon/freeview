#include <apk.h>


#include <vkernel.h>

#include <dbus-c++/dbus.h>
#include <as_apk_util_exe.h>

char const * const cApkUtilExe::BUSNAME_APK_UTIL_EXE = "Octopus.Appkit.Util.Exe";
char const * const cApkUtilExe::BUSPATH_APK_UTIL_EXE = "/Octopus/Appkit/Util/Exe";


cApkUtilExe::cApkUtilExe(DBus::Connection &connection)
  : DBus::ObjectAdaptor(connection, BUSPATH_APK_UTIL_EXE)
{
	/* apkit util exe */
    {	
		HERROR hErr = ERR_FAIL;
#if 0
		hErr = APK_EXE_FTP_Init ();
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("APK_EXE_FTP_Init Error : %d", hErr);
		}
		
		hErr = APK_EXE_BML_Init ();
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("APK_EXE_BML_Init Error : %d", hErr);
		}
#endif
		hErr = APK_EXE_Init ();
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("APK_EXE_Init Error : %d", hErr);
		}

		
		//HERROR APK_EXE_ExportDB(APK_EXE_ImExportDBNotifier_t fnNotifier, HBOOL bSync);
		//HERROR APK_EXE_ImportDB(APK_EXE_ImExportDBNotifier_t fnNotifier, HBOOL bSync);
		//HERROR APK_EXE_StartLcnUpdate(APK_EXE_LcnUpdateNotifier_t fnNotifier, HUINT32 ulSearchMode);
		//HERROR APK_EXE_StopLcnUpdate(APK_EXE_LcnUpdateNotifier_t fnNotifier);
		//HERROR APK_EXE_SaveLcnUpdate(APK_EXE_LcnUpdateNotifier_t fnNotifier, HUINT32 ulSearchMode, HUINT32 ulIndex);
		//HERROR APK_EXE_GetLcnUpdateInfo(APK_EXE_LcnUpdateNotifier_t fnNotifier);
		//HERROR APK_EXE_RFVOD_RegisterNotifier(APK_EXE_RFVOD_Notifier_t fnNotifier);
		//HERROR APK_EXE_BML_RegisterNotifier(APK_EXE_BML_Notifier_t fnNotifier, void * user_data);
		//HERROR APK_EXE_MHEG_RegisterNotifier(APK_EXE_MHEG_Notifier_t fnNotifier);

    }
}

void cApkUtilExe::BATCH_Stop()
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_EXE_BATCH_Stop ();
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_EXE_BATCH_Stop Error : %d", hErr);
	}

	return ;
}

void cApkUtilExe::BGEIT_Start_FreeSat()
{
#if 0	
	HERROR hErr = ERR_FAIL;
	hErr = APK_EXE_BGEIT_Start_FreeSat ();
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_EXE_BGEIT_Start_FreeSatError : %d", hErr);
	}
#endif
	return ;
}

void cApkUtilExe::BGEIT_Stop_FreeSat()
{
#if 0	
	HERROR hErr = ERR_FAIL;
	hErr = APK_EXE_BGEIT_Stop_FreeSat ();
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_EXE_BGEIT_Stop_FreeSat Error : %d", hErr);
	}
#endif

	return ;
}

void cApkUtilExe::FTP_Start()
{
#if 0
	HERROR hErr = ERR_FAIL;
	hErr = APK_EXE_FTP_Start ();
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_EXE_FTP_Start Error : %d", hErr);
	}
#endif
	return ;
}

void cApkUtilExe::FTP_Stop()
{
#if 0
	HERROR hErr = ERR_FAIL;
	hErr = APK_EXE_FTP_Stop ();
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_EXE_FTP_Stop Error : %d", hErr);
	}
#endif
	return ;
}

bool cApkUtilExe::FTP_GetStatus()
{
#if 0
	HERROR hErr = ERR_FAIL;

	HBOOL bFtpRun = FALSE;
		
	hErr = APK_EXE_FTP_GetStatus (&bFtpRun);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_EXE_FTP_GetStatus Error : %d", hErr);
	}

	return (bool)bFtpRun;
#else
	return false;
#endif	
}

void cApkUtilExe::StartFactoryDefault(const uint32_t& factoryDefaultMode)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_EXE_StartFactoryDefault ((DxFactoryDefault_type_e)factoryDefaultMode);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_EXE_StartFactoryDefault Error : %d", hErr);
	}

	return ;
}

void cApkUtilExe::RemoveLcnUpdateInfo(const bool& syncMode)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_EXE_RemoveLcnUpdateInfo ((HBOOL)syncMode);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_EXE_RemoveLcnUpdateInfo Error : %d", hErr);
	}	

	return ;
}

void cApkUtilExe::BML_Set(const bool& BMLKeyControlMode)
{
#if 0
	HERROR hErr = ERR_FAIL;
	hErr = APK_EXE_BML_Set ((HBOOL)BMLKeyControlMode);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_EXE_BML_Set Error : %d", hErr);
	}
#endif
	return ;
}

void cApkUtilExe::RFVOD_Start(const bool& vodMode, const std::string& url)
{
#if 0
	HERROR hErr = ERR_FAIL;
	hErr = APK_EXE_RFVOD_Start ((HBOOL) vodMode, (HUINT8 *)&url[0]);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_EXE_RFVOD_Start Error : %d", hErr);
	}
#endif
	return ;
}

void cApkUtilExe::RFVOD_Stop()
{
#if 0
	HERROR hErr = ERR_FAIL;
	hErr = APK_EXE_RFVOD_Stop ();
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_EXE_RFVOD_Stop Error : %d", hErr);
	}
#endif
	return ;
}

void cApkUtilExe::MHEG_Start(const uint32_t& wndId, const uint32_t& stillWndId)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_EXE_MHEG_Start ((HUINT32)wndId, (HUINT32)stillWndId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_EXE_MHEG_Start Error : %d", hErr);
	}

	return ;
}

void cApkUtilExe::MHEG_SetKey(const uint32_t& key)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_EXE_MHEG_SetKey ((HUINT32)key);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_EXE_MHEG_SetKey Error : %d", hErr);
	}

	return ;
}

void cApkUtilExe::MHEG_DeepLink(const std::string& url, const uint32_t& prevLcn)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_EXE_MHEG_DeepLink ((const HCHAR *)&url[0], (HUINT32)prevLcn);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_EXE_MHEG_DeepLink Error : %d", hErr);
	}

	return ;
}

void cApkUtilExe::MHEG_ControlOP(const bool& control)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_EXE_MHEG_ControlOP ((HBOOL)control);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_EXE_MHEG_ControlOP Error : %d", hErr);
	}

	return ;
}

void cApkUtilExe::MHEGIB_RequestFile(const uint32_t& sessionId, const int32_t& requestNameLen, const std::string& requestName)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_EXE_MHEGIB_RequestFile ((HUINT32)sessionId, (HINT32)requestNameLen, (const HCHAR *)&requestName[0]);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_EXE_MHEGIB_RequestFile Error : %d", hErr);
	}

	return ;
}

void cApkUtilExe::MHEGIB_RequestData(const uint32_t& sessionId, const int32_t& requestNameLen, const std::string& requestName)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_EXE_MHEGIB_RequestData ((HUINT32)sessionId, (HINT32)requestNameLen, (const HCHAR *)&requestName[0]);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_EXE_MHEGIB_RequestData Error : %d", hErr);
	}

	return ;
}

void cApkUtilExe::MHEGIB_AbortByUser(const uint32_t& sessionId)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_EXE_MHEGIB_AbortByUser ((HUINT32)sessionId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_EXE_MHEGIB_AbortByUser Error : %d", hErr);
	}

	return ;
}

void cApkUtilExe::MHEGIB_AbortBySystem(const uint32_t& sessionId)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_EXE_MHEGIB_AbortBySystem ((HUINT32)sessionId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_EXE_MHEGIB_AbortBySystem Error : %d", hErr);
	}

	return ;
}

void cApkUtilExe::StartSatRecord()
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_EXE_StartSatRecord ();
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_EXE_StartSatRecord Error : %d", hErr);
	}

	return ;
}

void cApkUtilExe::TVTVEPG_Initdirectory()
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_EXE_TVTVEPG_Initdirectory ();
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_EXE_TVTVEPG_Initdirectory Error : %d", hErr);
	}

	return ;
}

void cApkUtilExe::Load_BaseEPG()
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_EXE_Load_BaseEPG ();
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_EXE_Load_BaseEPG Error : %d", hErr);
	}

	return ;
}

void cApkUtilExe::Load_TVTVEPG()
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_EXE_Load_TVTVEPG ();
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_EXE_Load_TVTVEPG Error : %d", hErr);
	}

	return ;
}
