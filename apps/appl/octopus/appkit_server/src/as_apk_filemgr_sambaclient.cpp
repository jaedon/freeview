#include <apk.h>


#include <vkernel.h>

#include <dbus-c++/dbus.h>
#include <as_apk_filemgr_sambaclient.h>

char const * const cApkFilemgrSambaclient::BUSNAME_APK_FILEMGR_SAMBACLIENT = "Octopus.Appkit.Filemgr.Sambaclient";
char const * const cApkFilemgrSambaclient::BUSPATH_APK_FILEMGR_SAMBACLIENT = "/Octopus/Appkit/Filemgr/Sambaclient";

cApkFilemgrSambaclient * cApkFilemgrSambaclient::pMainThis	=	NULL;

void cApkFilemgrSambaclient::callback_Event (const HCHAR *pszEventName, void *pvData, void *pvUser)
{
	if (NULL == pMainThis)
		return;
	
//	pMainThis->event ();					
}

cApkFilemgrSambaclient::cApkFilemgrSambaclient(DBus::Connection &connection)
  : DBus::ObjectAdaptor(connection, BUSPATH_APK_FILEMGR_SAMBACLIENT)
{
	if (NULL == pMainThis)
		pMainThis = this;

	/* apkit filemgr sambaclient */
    {	
		HERROR hErr = ERR_FAIL;
		hErr = APK_FILEMGR_SAMBACLIENT_Init ();
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("APK_FILEMGR_SAMBACLIENT_Init Error : %d", hErr);
		}		

		hErr = APK_FILEMGR_SAMBACLIENT_RegisterCallback (cApkFilemgrSambaclient::callback_Event);
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("APK_FILEMGR_SAMBACLIENT_RegisterCallback Error : %d", hErr);
		}

		//HERROR	APK_FILEMGR_SAMBACLIENT_UnregisterCallback(APK_FILEMGR_SAMBACLIENT_Notifier_t fnNotifier);
    }
}

void cApkFilemgrSambaclient::StartScan()
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_FILEMGR_SAMBACLIENT_StartScan ();
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_FILEMGR_SAMBACLIENT_StartScan Error : %d", hErr);
	}

	return ;
}

void cApkFilemgrSambaclient::StopScan()
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_FILEMGR_SAMBACLIENT_StopScan ();
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_FILEMGR_SAMBACLIENT_StopScan Error : %d", hErr);
	}

	return ;
}

void cApkFilemgrSambaclient::Mount(const ::DBus::Struct< uint32_t >& sharedFolderInfo)
{
	HERROR hErr = ERR_FAIL;
	ApkFileMgr_SMBSharedFolderInfo_t stInfo = {0,};

	{
//		stInfo.szServerName;
//		stInfo.szSharedName;
//		stInfo.szIp;
//		stInfo.szId;
//		stInfo.szPassword;
//		stInfo.szMountPath;
//		stInfo.szLogMsg;
//		stInfo.bMounted;
		stInfo.nState			=	(HINT32)sharedFolderInfo._1;
	}
	
	hErr = APK_FILEMGR_SAMBACLIENT_Mount (&stInfo);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_FILEMGR_SAMBACLIENT_Mount Error : %d", hErr);
	}

	return;
}

void cApkFilemgrSambaclient::UnMount(const ::DBus::Struct< uint32_t >& sharedFolderInfo)
{
	HERROR hErr = ERR_FAIL;
	ApkFileMgr_SMBSharedFolderInfo_t stInfo = {0,};

	{
//		stInfo.szServerName;
//		stInfo.szSharedName;
//		stInfo.szIp;
//		stInfo.szId;
//		stInfo.szPassword;
//		stInfo.szMountPath;
//		stInfo.szLogMsg;
//		stInfo.bMounted;
		stInfo.nState			=	(HINT32)sharedFolderInfo._1;
	}

	hErr = APK_FILEMGR_SAMBACLIENT_Unmount (&stInfo);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_FILEMGR_SAMBACLIENT_Unmount Error : %d", hErr);
	}

	return;
}

void cApkFilemgrSambaclient::UnmountAll()
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_FILEMGR_SAMBACLIENT_UnmountAll ();
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_FILEMGR_SAMBACLIENT_UnmountAll Error : %d", hErr);
	}

	return;
}
