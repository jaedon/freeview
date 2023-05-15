#include <apk.h>


#include <vkernel.h>

#include <dbus-c++/dbus.h>
#include <as_apk_dlna_util.h>

char const * const cApkDlnaUtil::BUSNAME_APK_DLNA_UTIL = "Octopus.Appkit.Dlna.Util";
char const * const cApkDlnaUtil::BUSPATH_APK_DLNA_UTIL = "/Octopus/Appkit/Dlna/Util";

cApkDlnaUtil * cApkDlnaUtil::pMainThis	=	NULL;

STATIC const char * _apk_dmp_GetEventString (HUINT32 eEvent)
{
	switch (eEvent)
	{
//		ENUM_TO_STR (eAPK_Unknown)

	default:
		break;
	}

	return "Unknown";
}

STATIC const char * _apk_dmr_GetEventString (HUINT32 eEvent)
{
	switch (eEvent)
	{
//		ENUM_TO_STR (eAPK_Unknown)

	default:
		break;
	}

	return "Unknown";
}

STATIC const char * _apk_dms_GetEventString (HUINT32 eEvent)
{
	switch (eEvent)
	{
//		ENUM_TO_STR (eAPK_Unknown)

	default:
		break;
	}

	return "Unknown";
}

STATIC const char * _apk_dial_GetEventString (HUINT32 eEvent)
{
	switch (eEvent)
	{
//		ENUM_TO_STR (eAPK_Unknown)

	default:
		break;
	}

	return "Unknown";
}

STATIC const char * _apk_ws_GetEventString (HUINT32 eEvent)
{
	switch (eEvent)
	{
//		ENUM_TO_STR (eAPK_Unknown)

	default:
		break;
	}

	return "Unknown";
}

STATIC const char * _apk_satip_GetEventString (HUINT32 eEvent)
{
	switch (eEvent)
	{
//		ENUM_TO_STR (eAPK_Unknown)

	default:
		break;
	}

	return "Unknown";
}

STATIC const char * _apk_dlnaruis_GetEventString (HUINT32 eEvent)
{
	switch (eEvent)
	{
//		ENUM_TO_STR (eAPK_Unknown)

	default:
		break;
	}

	return "Unknown";
}

STATIC const char * _apk_obama_GetEventString (HUINT32 eEvent)
{
	switch (eEvent)
	{
//		ENUM_TO_STR (eAPK_Unknown)

	default:
		break;
	}

	return "Unknown";
}



void cApkDlnaUtil::dmp_callback_event(HUINT32 eEvent, HUINT32 ulParam1, HUINT32 ulParam2)
{
	if (NULL == pMainThis)
		return;
	
	std::string eventStr(_apk_dmp_GetEventString(eEvent));

	uint32_t param1=0, param2=0;

	param1 = (uint32_t)ulParam1;
	param2 = (uint32_t)ulParam2;

	pMainThis->dmp_event ((uint32_t)eEvent, eventStr, param1, param2);	
}

void cApkDlnaUtil::dmr_callback_event(HUINT32 eEvent, HUINT32 ulParam1, HUINT32 ulParam2)
{
	if (NULL == pMainThis)
		return;
	
	std::string eventStr(_apk_dmr_GetEventString(eEvent));

	uint32_t param1=0, param2=0;

	param1 = (uint32_t)ulParam1;
	param2 = (uint32_t)ulParam2;

	pMainThis->dmr_event ((uint32_t)eEvent, eventStr, param1, param2);	
}

void cApkDlnaUtil::dms_callback_event(HUINT32 eEvent, HUINT32 ulParam1, HUINT32 ulParam2)
{
	if (NULL == pMainThis)
		return;
	
	std::string eventStr(_apk_dms_GetEventString(eEvent));

	uint32_t param1=0, param2=0;

	param1 = (uint32_t)ulParam1;
	param2 = (uint32_t)ulParam2;

	pMainThis->dms_event ((uint32_t)eEvent, eventStr, param1, param2);	
}

void cApkDlnaUtil::dial_callback_event(HUINT32 eEvent, HUINT32 ulParam1, HUINT32 ulParam2)
{
	if (NULL == pMainThis)
		return;
	
	std::string eventStr(_apk_dial_GetEventString(eEvent));

	uint32_t param1=0, param2=0;

	param1 = (uint32_t)ulParam1;
	param2 = (uint32_t)ulParam2;

	pMainThis->dial_event ((uint32_t)eEvent, eventStr, param1, param2);	
}

void cApkDlnaUtil::ws_callback_event(HUINT32 eEvent, HUINT32 ulParam1, HUINT32 ulParam2)
{
	if (NULL == pMainThis)
		return;
	
	std::string eventStr(_apk_ws_GetEventString(eEvent));

	uint32_t param1=0, param2=0;

	param1 = (uint32_t)ulParam1;
	param2 = (uint32_t)ulParam2;

	pMainThis->ws_event ((uint32_t)eEvent, eventStr, param1, param2);	
}

void cApkDlnaUtil::satip_callback_event(HUINT32 eEvent, HUINT32 ulParam1, HUINT32 ulParam2)
{
	if (NULL == pMainThis)
		return;
	
	std::string eventStr(_apk_satip_GetEventString(eEvent));

	uint32_t param1=0, param2=0;

	param1 = (uint32_t)ulParam1;
	param2 = (uint32_t)ulParam2;

	pMainThis->satip_event ((uint32_t)eEvent, eventStr, param1, param2);	
}

void cApkDlnaUtil::dlnaruis_callback_event(HUINT32 eEvent, HUINT32 ulParam1, HUINT32 ulParam2)
{
	if (NULL == pMainThis)
		return;
	
	std::string eventStr(_apk_dlnaruis_GetEventString(eEvent));

	uint32_t param1=0, param2=0;

	param1 = (uint32_t)ulParam1;
	param2 = (uint32_t)ulParam2;

	pMainThis->dlnaruis_event ((uint32_t)eEvent, eventStr, param1, param2);	
}

void cApkDlnaUtil::obama_callback_event(HUINT32 eEvent, HUINT32 ulParam1, HUINT32 ulParam2)
{
	if (NULL == pMainThis)
		return;
	
	std::string eventStr(_apk_obama_GetEventString(eEvent));

	uint32_t param1=0, param2=0;

	param1 = (uint32_t)ulParam1;
	param2 = (uint32_t)ulParam2;

	pMainThis->obama_event ((uint32_t)eEvent, eventStr, param1, param2);	
}

cApkDlnaUtil::cApkDlnaUtil(DBus::Connection &connection)
  : DBus::ObjectAdaptor(connection, BUSPATH_APK_DLNA_UTIL)
{
	if (NULL == pMainThis)
		pMainThis = this;

	/* apkit dlna util */
    {	
		APK_DLNADMP_Init ();
		//void APK_DLNADMP_DeInit( void );
		APK_DLNADMP_RegisterNotifier (cApkDlnaUtil::dmp_callback_event);
		//void APK_DLNADMP_UnRegisterNotifier(APKS_DLNA_CbHandler pfnObjectHandler);

		APK_DLNADMR_Init ();
		//void APK_DLNADMR_DeInit( void );
		APK_DLNADMR_UnRegisterNotifier (cApkDlnaUtil::dmr_callback_event);
		//void APK_DLNADMR_RegisterNotifier( APKS_DLNA_CbHandler pfnObjectHandler );

		APK_DLNADMS_Init ();
		//void APK_DLNADMS_DeInit( void )
		APK_DLNADMS_RegisterNotifier (cApkDlnaUtil::dms_callback_event);
		//void APK_DLNADMS_UnRegisterNotifier( APKS_DLNA_CbHandler pfnObjectHandler );

		APK_DIAL_Init ();
		//void APK_DIAL_DeInit( void );
		APK_DIAL_RegisterNotifier (cApkDlnaUtil::dial_callback_event);
		//void APK_DIAL_UnRegisterNotifier( APKS_DLNA_CbHandler pfnObjectHandler );

		APKI_WOONSERVER_Init ();
		//void APKI_WOONSERVER_DeInit( void );
		APK_WOONSERVER_RegisterNotifier (cApkDlnaUtil::ws_callback_event);
		//void APK_WOONSERVER_UnRegisterNotifier( APKS_DLNA_CbHandler pfnObjectHandler );

		APK_SATIP_Init ();
		//void APK_SATIP_DeInit( void )
		APK_SATIP_RegisterNotifier (cApkDlnaUtil::satip_callback_event);
		//void APK_SATIP_UnRegisterNotifier( APKS_DLNA_CbHandler pfnObjectHandler );
#if 0 // APK 미구현
		APK_DLNARUIS_RegisterNotifier (cApkDlnaUtil::dlnaruis_callback_event);
		//void APK_DLNARUIS_UnRegisterNotifier( APKS_DLNA_CbHandler pfnObjectHandler );

		APK_DLNA_OBAMA_Init ();
		//void APK_DLNA_OBAMA_DeInit( void );
		APK_DLNA_OBAMA_RegisterNotifier (cApkDlnaUtil::obama_callback_event);
		//void APK_DLNA_OBAMA_UnRegisterNotifier( APKS_DLNA_CbHandler pfnObjectHandler );
#endif
    }
}

void cApkDlnaUtil::DMP_Start()
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_DLNADMP_Start ();
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_AIRPLAY_Start Error : %d", hErr);
	}

	return ;
}

void cApkDlnaUtil::DMP_Stop()
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_DLNADMP_Stop ();
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_DLNADMP_Stop Error : %d", hErr);
	}


}

std::vector< ::DBus::Struct< std::string, std::string, std::string, int32_t, std::string, std::string, bool, uint32_t, uint32_t, uint32_t, uint32_t, std::vector< std::string >, std::vector< std::string >, std::vector< std::string >, std::vector< std::string > > > cApkDlnaUtil::DMP_GetListOfDMS()
{
	std::vector< ::DBus::Struct< std::string, std::string, std::string, int32_t, std::string, std::string, bool, uint32_t, uint32_t, uint32_t, uint32_t, std::vector< std::string >, std::vector< std::string >, std::vector< std::string >, std::vector< std::string > > >	reval;

	HERROR hErr = ERR_FAIL;
	HUINT32 nCount = 0;
	APKS_DLNA_DMP_ServerDevice_t *pResultData = NULL;
	 	
	hErr = APK_DLNADMP_getListOfDMS (&nCount, &pResultData);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_DLNADMP_getListOfDMS Error : %d", hErr);
	}

	if (NULL != pResultData)
	{
		HINT32 i,j;
		for (i=0 ; i<(HINT32)nCount ; i++)
		{
			::DBus::Struct< std::string, std::string, std::string, int32_t, std::string, std::string, bool, uint32_t, uint32_t, uint32_t, uint32_t, std::vector< std::string >, std::vector< std::string >, std::vector< std::string >, std::vector< std::string > > item;

			item._1	=	std::string(pResultData[i].szTitle);
			item._2	=	std::string(pResultData[i].szUDN);
			item._3	=	std::string(pResultData[i].szIP);

			item._4	=	(int32_t)pResultData[i].nDeviceHandle;
			item._5	=	std::string(pResultData[i].szMACAddress);
			item._6	=	std::string(pResultData[i].szTotalStroageSize);

			item._7	=	(bool)pResultData[i].bIsSupportSrs;

			item._8		=	(uint32_t)pResultData[i].ucSortCapNum;
			item._9		=	(uint32_t)pResultData[i].ucSearchCapNum;
			item._10	=	(uint32_t)pResultData[i].ucDlnaCapNum;
			item._11	=	(uint32_t)pResultData[i].ucJlabsCapNum;

			HINT32 nSortCap, nSerchCap, nDlnaCap, nJlabsCap;
			nSortCap	=	(HINT32)pResultData[i].ucSortCapNum;
			nSerchCap	=	(HINT32)pResultData[i].ucSearchCapNum;
			nDlnaCap	=	(HINT32)pResultData[i].ucDlnaCapNum;
			nJlabsCap	=	(HINT32)pResultData[i].ucJlabsCapNum;

			if (nSortCap>APKD_DLNA_DMP_MAX_UNKNOWN_CAP) nSortCap = APKD_DLNA_DMP_MAX_UNKNOWN_CAP;
			if (nSerchCap>APKD_DLNA_DMP_MAX_UNKNOWN_CAP) nSerchCap = APKD_DLNA_DMP_MAX_UNKNOWN_CAP;
			if (nDlnaCap>APKD_DLNA_DMP_MAX_UNKNOWN_CAP) nDlnaCap = APKD_DLNA_DMP_MAX_UNKNOWN_CAP;
			if (nJlabsCap>APKD_DLNA_DMP_MAX_UNKNOWN_CAP) nJlabsCap = APKD_DLNA_DMP_MAX_UNKNOWN_CAP;

			for (j=0 ; j<nSortCap ; j++)
				item._12.push_back(std::string(pResultData[i].szSortCAP[i]));
			for (j=0 ; j<nSerchCap ; j++)
				item._12.push_back(std::string(pResultData[i].szSearchCAP[i]));
			for (j=0 ; j<nDlnaCap ; j++)
				item._12.push_back(std::string(pResultData[i].szDlnaCAP[i]));
			for (j=0 ; j<nJlabsCap ; j++)
				item._12.push_back(std::string(pResultData[i].szJlabsCAP[i]));

			reval.push_back (item);
		}
	}

	return reval;
}

void cApkDlnaUtil::DMP_BrowsingChildItem(const std::string& udn, const std::string& cid, const std::string& sort, const int32_t& startIndex, const int32_t& request)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_DLNADMP_BrowsingChildItem ((const HCHAR *)&udn[0], (const HCHAR *)&cid[0], (const HCHAR *)&sort[0], (HINT32)startIndex, (HINT32)request);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_DLNADMP_BrowsingChildItem Error : %d", hErr);
	}

	return ;
}

void cApkDlnaUtil::DMP_SearchingItem(const std::string& udn, const std::string& cid, const std::string& category, const std::string& keyword, const std::string& sort, const int32_t& startIndex, const int32_t& request, const int32_t& requestId)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_DLNADMP_SearchingItem ((const HCHAR *)&udn[0], (const HCHAR *)&cid[0], (const HCHAR *)&category[0], (const HCHAR *)&keyword[0], (const HCHAR *)&sort[0], (HINT32)startIndex, (HINT32)request, (HINT32)requestId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_DLNADMP_SearchingItem Error : %d", hErr);
	}

	return ;
}

void cApkDlnaUtil::DMP_SearchingAll(const std::string& cid, const std::string& category, const std::string& keyword, const std::string& sort, const int32_t& requestId)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_DLNADMP_SearchingAll ((const HCHAR *)&cid[0], (const HCHAR *)&category[0], (const HCHAR *)&keyword[0], (const HCHAR *)&sort[0], (HINT32)requestId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_DLNADMP_SearchingAll Error : %d", hErr);
	}

}

void cApkDlnaUtil::DMP_GetRecordDestinations(const std::string& udn)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_DLNADMP_GetRecordDestinations ((const HCHAR *)&udn[0]);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_DLNADMP_GetRecordDestinations Error : %d", hErr);
	}

	return ;
}

void cApkDlnaUtil::DMP_GetRecordDiskInfo(const std::string& udn, const std::string& id)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_DLNADMP_GetRecordDiskInfo ((const HCHAR *)&udn[0], (const HCHAR *)&id[0]);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_DLNADMP_GetRecordDiskInfo Error : %d", hErr);
	}

	return ;
}

std::string cApkDlnaUtil::DMP_RemoveItem(const std::string& udn, const std::string& id)
{
	HERROR hErr = ERR_FAIL;
	HCHAR * pRetval = NULL;

	hErr = APK_DLNADMP_RemoveItem ((const HCHAR *)&udn[0], (const HCHAR *)&id[0], &pRetval);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_DLNADMP_RemoveItem Error : %d", hErr);
	}

	return std::string(pRetval);
}

bool cApkDlnaUtil::DMP_GetIsIPDubbing()
{
	HERROR hErr = ERR_FAIL;

	HBOOL val = FALSE;
		
	hErr = APK_DLNADMP_GetIsIPDubbing (&val);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_DLNADMP_GetIsIPDubbing Error : %d", hErr);
	}

	return (bool)val;
}

void cApkDlnaUtil::DMP_RefreshDms(const std::string& udn)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_DLNADMP_RefreshDms ((const HCHAR *)&udn[0]);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_DLNADMP_RefreshDms Error : %d", hErr);
	}

	return ;
}

void cApkDlnaUtil::DMR_Start(const std::string& friendlyName)
{
	HERROR hErr = ERR_FAIL;	
	hErr = APK_DLNADMR_Start ((HCHAR *)&friendlyName[0]);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_DLNADMR_Start Error : %d", hErr);
	}

	return ;
}

void cApkDlnaUtil::DMR_Stop()
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_DLNADMR_Stop ();
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_DLNADMR_Stop Error : %d", hErr);
	}

	return ;
}

std::string cApkDlnaUtil::DMR_GetDefaultFriendlyName()
{
	HERROR hErr = ERR_FAIL;

	HCHAR szFriendlyName[1024] = {0,};

	hErr = APK_DLNADMR_GetDefaultFriendlyName (szFriendlyName, 1024);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_DLNADMR_GetDefaultFriendlyName Error : %d", hErr);
	}

	return std::string(szFriendlyName);
}

void cApkDlnaUtil::DMR_SetFriendlyName(const std::string& friendlyName)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_DLNADMR_SetFriendlyName ((const HCHAR *)&friendlyName[0]);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_DLNADMR_SetFriendlyName Error : %d", hErr);
	}

	return ;
}

void cApkDlnaUtil::DMR_SetBringInUri(const std::string& url)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_DLNADMR_SetBringInUri ((const HCHAR *)&url[0]);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_DLNADMR_SetBringInUri Error : %d", hErr);
	}

	return ;
}

void cApkDlnaUtil::DMR_SetOperationMode(const int32_t& mode)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_DLNADMR_SetOperationMode ((HINT32)mode );
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_DLNADMR_SetOperationMode Error : %d", hErr);
	}

	return ;
}

void cApkDlnaUtil::DMS_Start(const std::string& friendlyName, const bool& boottimeStart)
{
	HERROR hErr = ERR_FAIL;	
	hErr = APK_DLNADMS_Start ((HCHAR *)&friendlyName[0], (HBOOL)boottimeStart);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_DLNADMS_Start Error : %d", hErr);
	}

	return ;
}

void cApkDlnaUtil::DMS_Stop()
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_DLNADMS_Stop ();
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_DLNADMS_Stop Error : %d", hErr);
	}

	return ;
}

std::string cApkDlnaUtil::DMS_GetDefaultFriendlyName()
{
	HERROR hErr = ERR_FAIL;

	HCHAR szFriendlyName[1024] = {0,};
	
	hErr = APK_DLNADMS_GetDefaultFriendlyName (szFriendlyName, 1024);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_DLNADMS_GetDefaultFriendlyName Error : %d", hErr);
	}

	return std::string(szFriendlyName);
}

void cApkDlnaUtil::DMS_SetFriendlyName(const std::string& friendlyName)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_DLNADMS_SetFriendlyName ((const HCHAR *)&friendlyName[0]);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_DLNADMS_SetFriendlyName Error : %d", hErr);
	}

	return ;
}

void cApkDlnaUtil::DIAL_Start(const std::string& friendlyName)
{
	HERROR hErr = ERR_FAIL;
	
	hErr = APK_DIAL_Start ((HCHAR *)&friendlyName[0]);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_DIAL_Start Error : %d", hErr);
	}

	return ;
}

void cApkDlnaUtil::DIAL_Stop()
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_DIAL_Stop ();
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_DIAL_Stop Error : %d", hErr);
	}

	return ;
}

void cApkDlnaUtil::DIAL_SetFriendlyName(const std::string& friendlyName)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_DIAL_SetFriendlyName ((HCHAR *)&friendlyName[0]);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_DIAL_SetFriendlyName Error : %d", hErr);
	}

	return ;
}

void cApkDlnaUtil::DIAL_AppRegist(const std::string& app)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_DIAL_AppRegist ((HCHAR *)&app[0]);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_DIAL_AppRegist Error : %d", hErr);
	}

	return ;
}

void cApkDlnaUtil::DIAL_AppUnregist(const std::string& app)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_DIAL_AppUnregist ((HCHAR *)&app[0]);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_DIAL_AppUnregist Error : %d", hErr);
	}	

	return ;
}

void cApkDlnaUtil::DIAL_SetAppState(const std::string& app, const int32_t& state)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_DIAL_SetAppState ((HCHAR *)&app[0], (HINT32)state);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_DIAL_SetAppState Error : %d", hErr);
	}

	return ;
}

void cApkDlnaUtil::WOONSERVER_Start(const std::string& friendlyName)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_WOONSERVER_Start ((HCHAR *)&friendlyName[0]);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_WOONSERVER_Start Error : %d", hErr);
	}

	return ;
}

void cApkDlnaUtil::WOONSERVER_Stop()
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_WOONSERVER_Stop();
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_WOONSERVER_Stop Error : %d", hErr);
	}

	return ;
}

std::string cApkDlnaUtil::WOONSERVER_GetDefaultFriendlyName()
{
	HERROR hErr = ERR_FAIL;

	HCHAR szFriendlyName[1024] = {0,};
	
	hErr = APK_WOONSERVER_GetDefaultFriendlyName (szFriendlyName, 1024);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_WOONSERVER_GetDefaultFriendlyName Error : %d", hErr);
	}

	return std::string(szFriendlyName);
}

void cApkDlnaUtil::WOONSERVER_SetFriendlyName(const std::string& friendlyName)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_WOONSERVER_SetFriendlyName ((HCHAR *)&friendlyName[0]);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_WOONSERVER_SetFriendlyName Error : %d", hErr);
	}

	return ;
}

void cApkDlnaUtil::WOONSERVER_TestConnection()
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_WOONSERVER_TestConnection();
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_WOONSERVER_TestConnection Error : %d", hErr);
	}
	
	return ;
}

void cApkDlnaUtil::SATIP_Start(const std::string& friendlyName)
{
	HERROR hErr = ERR_FAIL;

	hErr = APK_SATIP_Start ((HCHAR *)&friendlyName[0]);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_SATIP_Start Error : %d", hErr);
	}

	return ;
}

void cApkDlnaUtil::SATIP_Stop()
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_SATIP_Stop ();
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_SATIP_Stop Error : %d", hErr);
	}

	return ;
}

std::string cApkDlnaUtil::SATIP_GetDefaultFriendlyName()
{
	HERROR hErr = ERR_FAIL;

	HCHAR szFriendlyName[1024] = {0,};
#if 0 //APK 함수 미구현	
	hErr = APK_SATIP_GetDefaultFriendlyName (szFriendlyName, 1024);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_SATIP_GetDefaultFriendlyName Error : %d", hErr);
	}
#endif
	return std::string(szFriendlyName);
}

void cApkDlnaUtil::SATIP_SetFriendlyName(const std::string& friendlyName)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_SATIP_SetFriendlyName ((HCHAR *)&friendlyName[0]);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_SATIP_SetFriendlyName Error : %d", hErr);
	}

	return ;
}

void cApkDlnaUtil::DLNARUIS_SendMessage(const int32_t& deviceHandle, const int32_t& reqHandle, const std::vector< uint8_t >& header, const std::vector< uint8_t >& message)
{
#if 0 //APK 함수 미구현	
	HERROR hErr = ERR_FAIL;
	hErr = APK_DLNARUIS_SendMessage ((HINT32)deviceHandle, (HINT32)reqHandle, (HCHAR *)&header[0], (HCHAR *)&message[0] );
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_DLNARUIS_SendMessage Error : %d", hErr);
	}
#endif
	
	return ;
}

void cApkDlnaUtil::DLNARUIS_SendMulticast(const int32_t& deviceHandle, const int32_t& eventLevel, const std::vector< uint8_t >& notifCEHTML, const std::string& friendlyName, const std::vector< uint8_t >& profielList)
{
#if 0 //APK 함수 미구현	
	HERROR hErr = ERR_FAIL;
	hErr = APK_DLNARUIS_SendMulticast ((HINT32)deviceHandle, (HINT32)eventLevel, (HCHAR *)&notifCEHTML[0], (HCHAR *)&friendlyName[0], (HCHAR *)&profielList[0]);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_DLNARUIS_SendMulticast Error : %d", hErr);
	}
#endif

	return ;
}

void cApkDlnaUtil::DLNARUIS_SendErrorCode(const int32_t& deviceHandle, const int32_t& reqHandle, const uint32_t& error)
{
#if 0 //APK 함수 미구현	
	HERROR hErr = ERR_FAIL;
	hErr = APK_DLNARUIS_SendErrorCode ((HINT32)deviceHandle,(HINT32)reqHandle, (APKE_RUIS_ErrorCode_e)error );
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_DLNARUIS_SendErrorCode Error : %d", hErr);
	}
#endif

	return ;
}

void cApkDlnaUtil::LiveStream_Start(const ::DBus::Struct< bool, uint32_t, int32_t, bool, uint32_t, uint32_t >& setupInfo)
{
#if 0 //APK 함수 미구현	
	HERROR hErr = ERR_FAIL;
	APKS_DLNA_LIVESTREAM_SETUP_t stSetup;

	HxSTD_memset (&stSetup, 0, sizeof(APKS_DLNA_LIVESTREAM_SETUP_t));
	{
		stSetup.bTranscode			=	(HBOOL)setupInfo._1;
		stSetup.svc_handle			=	(HUINT32)setupInfo._2;
		stSetup.profile_id			=	(HINT32)setupInfo._3;
		stSetup.bPcrTsPacketInsert	=	(HBOOL)setupInfo._4;

		stSetup.source				=	(APKE_DLNA_LIVESTREAM_SourceType_e)setupInfo._5;
		stSetup.output				=	(APKE_DLNA_LIVESTREAM_OutputType_e)setupInfo._6;		
	}
	
	hErr = APK_DLNA_OBAMA_LiveStream_Start (&stSetup);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_DLNA_OBAMA_LiveStream_Start Error : %d", hErr);
	}
#endif

	return ;
}

void cApkDlnaUtil::LiveStream_Stop(const int32_t& handle)
{
#if 0 //APK 함수 미구현	
	HERROR hErr = ERR_FAIL;
	hErr = APK_DLNA_OBAMA_LiveStream_Stop ((HINT32)handle);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_DLNA_OBAMA_LiveStream_Stop Error : %d", hErr);
	}
#endif

	return ;
}

int32_t cApkDlnaUtil::LiveStream_ItemindexByProfile(const std::string& profileName)
{
	HERROR hErr = ERR_FAIL;
	HINT32 nIndex = 0;
#if 0 //APK 함수 미구현	
	hErr = APK_DLNA_OBAMA_LiveStream_ItemindexByProfile ((HCHAR *)&profileName[0], &nIndex);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_DLNA_OBAMA_LiveStream_ItemindexByProfile Error : %d", hErr);
	}
#endif

	return (int32_t)nIndex;
}

::DBus::Struct< bool, uint32_t > cApkDlnaUtil::LiveStream_Status(const int32_t& handle, const int32_t& type)
{
	HERROR hErr = ERR_FAIL;

	::DBus::Struct< bool, uint32_t > reval;
#if 0 //APK 함수 미구현	

	APKS_DLNA_LIVESTREAM_STATUS_t stStatus = {0,};

	hErr = APK_DLNA_OBAMA_LiveStream_Status ((HINT32)handle, (HINT32)type, &stStatus);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_DLNA_OBAMA_LiveStream_Status Error : %d", hErr);
	}

	{
		reval._1	=	stStatus.is_transcode;	
		reval._2	=	stStatus.rating;
	}
#endif

	return reval;
}
