#include <apk.h>


#include <vkernel.h>

#include <dbus-c++/dbus.h>
#include <as_apk_filemgr_sambaserver.h>

char const * const cApkFilemgrSambaserver::BUSNAME_APK_FILEMGR_SAMBASERVER = "Octopus.Appkit.Filemgr.Sambaserver";
char const * const cApkFilemgrSambaserver::BUSPATH_APK_FILEMGR_SAMBASERVER = "/Octopus/Appkit/Filemgr/Sambaserver";

cApkFilemgrSambaserver * cApkFilemgrSambaserver::pMainThis	=	NULL;

void cApkFilemgrSambaserver::callback_Event (HINT32 eEvent, HINT32 result, void *pvUser)
{
	if (NULL == pMainThis)
		return;
	
//	pMainThis->event ();					
}

cApkFilemgrSambaserver::cApkFilemgrSambaserver(DBus::Connection &connection)
  : DBus::ObjectAdaptor(connection, BUSPATH_APK_FILEMGR_SAMBASERVER)
{
	if (NULL == pMainThis)
		pMainThis = this;

	/* apkit filemgr sambaserver */
    {	
	    HERROR hErr = ERR_FAIL;
		hErr = APK_FILEMGR_SAMBASERVER_Init();
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("APK_FILEMGR_SAMBASERVER_Init Error : %d", hErr);
		}		
		
			
		hErr = APK_FILEMGR_SAMBASERVER_RegisterCallback (cApkFilemgrSambaserver::callback_Event);
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("APK_FILEMGR_SAMBASERVER_RegisterCallback Error : %d", hErr);
		}	
		//HERROR	APK_FILEMGR_SAMBASERVER_UnregisterCallback(APK_FILEMGR_SAMBASERVER_Notifier_t fnNotifier);
		
    }
}

void cApkFilemgrSambaserver::Start()
{
	HINT32	nReval = 0;
	nReval = APK_FILEMGR_SAMBASERVER_Start();

	return ;
}

void cApkFilemgrSambaserver::Stop()
{
	HINT32	nReval = 0;
	nReval = APK_FILEMGR_SAMBASERVER_Stop ();

	return ;
}

void cApkFilemgrSambaserver::SetConfig(const ::DBus::Struct< uint32_t >& configInfo)
{
	HINT32	nReval = 0;
	ApkFileMgr_SMBS_CONF_t stConfig = {0,};

	{
//		stConfig.szWorkgroup	=	configInfo._1;
//		stConfig.szServerName;
//		stConfig.szServerDesc;
	}
	
	nReval = APK_FILEMGR_SAMBASERVER_SetConfig (&stConfig);

	return ;
}

::DBus::Struct< uint32_t > cApkFilemgrSambaserver::GetServerInfo()
{
	::DBus::Struct< uint32_t > reval;

	HINT32	nReval = 0;
	ApkFileMgr_SMBS_INFO_t stInfo={0,};
	nReval = APK_FILEMGR_SAMBASERVER_GetServerInfo (&stInfo);

	{
		reval._1	=	(uint32_t)stInfo.bStarted;
//		stInfo.nClientCnt;
//		stInfo.stConf;		
	}

	return reval;
}

void cApkFilemgrSambaserver::SetPassword(const std::string& userName, const std::string& currentPassword, const std::string& newPassword)
{
	HINT32	nReval = 0;
	nReval = APK_FILEMGR_SAMBASERVER_SetPassword ((const HCHAR *)&userName[0], (const HCHAR *)&currentPassword[0], (const HCHAR *)&newPassword[0]);

	return ;
}

bool cApkFilemgrSambaserver::CheckPassword(const std::string& userName, const std::string& password)
{
	HINT32	nReval = 0;
	HBOOL bIsValid;
	nReval = APK_FILEMGR_SAMBASERVER_CheckPassword ((const HCHAR *)&userName[0], (const HCHAR *)&password[0], &bIsValid);

	return (bool)bIsValid;
}

void cApkFilemgrSambaserver::SetPermission(const std::string& userName, const ::DBus::Struct< uint32_t >& sharedInfo)
{
	HINT32	nReval = 0;
	ApkFileMgr_SMBS_SHARED_t stShared = {0,};

	{
//		stShared.szName		=	sharedInfo._1;
//		stShared.szPath;
//		stShared.nState;
//		stShared.bWritable;
//		stShared.bMounted;
	}
	
	nReval = APK_FILEMGR_SAMBASERVER_SetPermission ((const HCHAR *)&userName[0], &stShared);

	return ;
}

std::vector< ::DBus::Struct< uint32_t > > cApkFilemgrSambaserver::GetSharedList(const std::string& userName)
{
	std::vector< ::DBus::Struct< uint32_t > > reval;

	HERROR hErr = ERR_FAIL;
	HINT32 nCount = 0;
	ApkFileMgr_SMBS_SHARED_t *pstList	=	NULL;
		
	hErr = APK_FILEMGR_SAMBASERVER_GetSharedList ((const HCHAR *)&userName[0], &nCount, &pstList);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_FILEMGR_SAMBASERVER_GetSharedList Error : %d", hErr);
	}	
	else if (NULL != pstList)
	{
		HINT32 i;
		for (i=0 ; i<nCount ; i++)
		{
			::DBus::Struct< uint32_t > item;
	
			{
		//		item._1 =stShared.szName 		
		//		stShared.szPath;
		//		stShared.nState;
		//		stShared.bWritable;
		//		stShared.bMounted;
			}

			reval.push_back (item);
		}
	}

	return reval;
}

void cApkFilemgrSambaserver::SetPublicAccess(const std::string& userName, const bool& access)
{
	HINT32	nReval = 0;
	nReval = APK_FILEMGR_SAMBASERVER_SetPublicAccess ((const HCHAR *)&userName[0], (HBOOL)access);

	return ;
}

bool cApkFilemgrSambaserver::GetPublicAccess(const std::string& userName)
{
	HERROR hErr = ERR_FAIL;
	HBOOL bAccess = FALSE;
	
	hErr = APK_FILEMGR_SAMBASERVER_GetPublicAccess ((const HCHAR *)&userName[0], &bAccess);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_FILEMGR_SAMBASERVER_GetPublicAccess Error : %d", hErr);
	}	

	return (bool)bAccess;
}
