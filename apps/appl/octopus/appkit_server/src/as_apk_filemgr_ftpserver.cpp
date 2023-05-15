#include <apk.h>


#include <vkernel.h>

#include <dbus-c++/dbus.h>
#include <as_apk_filemgr_ftpserver.h>

char const * const cApkFilemgrFtpserver::BUSNAME_APK_FILEMGR_FTPSERVER = "Octopus.Appkit.Filemgr.Ftpserver";
char const * const cApkFilemgrFtpserver::BUSPATH_APK_FILEMGR_FTPSERVER = "/Octopus/Appkit/Filemgr/Ftpserver";

cApkFilemgrFtpserver * cApkFilemgrFtpserver::pMainThis	=	NULL;

void cApkFilemgrFtpserver::callback_Event(HINT32 eEvent, HINT32 nResult, void *pvUser)
{
	if (NULL == pMainThis)
		return;
	
	uint32_t event=0;
	int32_t result=0;

	event = (uint32_t)eEvent;
	result = (int32_t)nResult;

	pMainThis->event (event, result);
}

cApkFilemgrFtpserver::cApkFilemgrFtpserver(DBus::Connection &connection)
  : DBus::ObjectAdaptor(connection, BUSPATH_APK_FILEMGR_FTPSERVER)
{
	if (NULL == pMainThis)
		pMainThis = this;

	/* apkit filemgr ftpserver */
    {	
		HERROR hErr = ERR_FAIL;
		hErr = APK_FILEMGR_FTPSERVER_Init ();
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("APK_FILEMGR_FTPSERVER_Init Error : %d", hErr);
		}

		hErr = APK_FILEMGR_FTPSERVER_RegisterCallback (cApkFilemgrFtpserver::callback_Event);
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("APK_FILEMGR_FTPSERVER_RegisterCallback Error : %d", hErr);
		}
		//HERROR APK_FILEMGR_FTPSERVER_UnregisterCallback(APK_FILEMGR_FTPSERVER_Notifier_t fnNotifier);
    }
}

int32_t cApkFilemgrFtpserver::Start()
{
	HINT32 reval;
	reval = APK_FILEMGR_FTPSERVER_Start ();

	return reval;
}

int32_t cApkFilemgrFtpserver::Stop()
{
	HINT32 reval;
	reval = APK_FILEMGR_FTPSERVER_Stop ();

	return reval;
}

int32_t cApkFilemgrFtpserver::SetConfig(const ::DBus::Struct< std::string, int32_t >& configInfo)
{
	HINT32 reval;

	ApkFileMgr_FTPS_CONF_t stConfig;

	HxSTD_memset (&stConfig, 0, sizeof(ApkFileMgr_FTPS_CONF_t));

	{
		HxSTD_StrNCpy (stConfig.szServerName, &configInfo._1[0], configInfo._1.size());
		stConfig.nPort	=	(HINT32)configInfo._2;
	}
		
	reval = APK_FILEMGR_FTPSERVER_SetConfig (&stConfig);

	return reval;
}

void cApkFilemgrFtpserver::GetServerInfo(int32_t& reval, ::DBus::Struct< int32_t, int32_t, std::string, int32_t >& ftpInfo)
{
	ApkFileMgr_FTPS_INFO_t stInfo = {0,};
	reval = APK_FILEMGR_FTPSERVER_GetServerInfo (&stInfo);

	{
		ftpInfo._1 = (int32_t)stInfo.bStarted;
		ftpInfo._2 = (int32_t)stInfo.nClientCnt;
		ftpInfo._3 = (int32_t)stInfo.stConf.szServerName;
		ftpInfo._4 = (int32_t)stInfo.stConf.nPort;		
	}

	return ;
}

int32_t cApkFilemgrFtpserver::SetPassword(const std::string& userName, const std::string& currentPassword, const std::string& newPassword)
{
	HINT32 reval;
	reval = APK_FILEMGR_FTPSERVER_SetPassword ((const HCHAR *)&userName[0], (const HCHAR *)&currentPassword[0], (const HCHAR *)&newPassword[0]);

	return reval;
}

void cApkFilemgrFtpserver::CheckPassword(const std::string& userName, const std::string& password, bool& isVaild, int32_t& reval)
{
	reval = (int32_t)APK_FILEMGR_FTPSERVER_CheckPassword ((const HCHAR *)&userName[0], (const HCHAR *)&password[0], (HBOOL *)&isVaild);

	return ;
}

int32_t cApkFilemgrFtpserver::SetPermission(const std::string& userName, const ::DBus::Struct< std::string, std::string, int32_t, bool, bool >& sharedInfo)
{
	HINT32 reval;

	ApkFileMgr_FTPS_SHARED_t stShared;

	HxSTD_memset (&stShared, 0, sizeof(ApkFileMgr_FTPS_SHARED_t));

	{
		HxSTD_StrNCpy (stShared.szName, &sharedInfo._1[0], sharedInfo._1.size());
		HxSTD_StrNCpy (stShared.szPath, &sharedInfo._2[0], sharedInfo._2.size());			
		stShared.nState		=	sharedInfo._3;		
		stShared.bWritable	=	sharedInfo._4;
		stShared.bMounted	=	sharedInfo._5;
	}
		
	reval = APK_FILEMGR_FTPSERVER_SetPermission ((const HCHAR *)&userName[0], &stShared);

	return reval;
}

std::vector< ::DBus::Struct< std::string, std::string, int32_t, bool, bool > > cApkFilemgrFtpserver::GetSharedList(const std::string& userName)
{
	HERROR hErr = ERR_FAIL;

	std::vector< ::DBus::Struct< std::string, std::string, int32_t, bool, bool > > reval;

	HINT32 nCount = 0;
	ApkFileMgr_FTPS_SHARED_t *pstList = NULL;

	hErr = APK_FILEMGR_FTPSERVER_GetSharedList ((const HCHAR *)&userName[0], &nCount, &pstList);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_FILEMGR_FTPSERVER_GetSharedList Error : %d", hErr);
	}

	if (NULL != pstList)
	{
		HINT32 i;
		for (i=0 ; i<nCount ; i++)
		{
			::DBus::Struct< std::string, std::string, int32_t, bool, bool > item;

			item._1	= std::string(pstList[i].szName);
			item._2	= std::string(pstList[i].szPath);
			item._3	= (int32_t)pstList[i].nState;
			item._4	= (bool)pstList[i].bWritable;
			item._5	= (bool)pstList[i].bMounted;

			reval.push_back (item);
		}

		HLIB_MEM_Free(pstList);
	}

	return reval;
}

int32_t cApkFilemgrFtpserver::SetPublicAccess(const std::string& userName, const bool& access)
{
	HINT32 reval;
	reval = APK_FILEMGR_FTPSERVER_SetPublicAccess ((const HCHAR *)&userName[0], (HBOOL)access);

	return reval;
}

bool cApkFilemgrFtpserver::GetPublicAccess(const std::string& userName)
{
	HERROR hErr = ERR_FAIL;

	HBOOL bAccess = FALSE;

	hErr = APK_FILEMGR_FTPSERVER_GetPublicAccess ((const HCHAR *)&userName[0], (HBOOL *)&bAccess);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_FILEMGR_FTPSERVER_GetPublicAccess Error : %d", hErr);
	}

	return (bool)bAccess;
}
