#include <apk.h>


#include <vkernel.h>

#include <dbus-c++/dbus.h>

#include <dbus_util_cvt.h>
#include <as_apk_meta_service.h>

char const * const cApkMetaService::BUSNAME_APK_META_SERVICE = "Octopus.Appkit.Meta.Service";
char const * const cApkMetaService::BUSPATH_APK_META_SERVICE = "/Octopus/Appkit/Meta/Service";

cApkMetaService * cApkMetaService::pMainThis	=	NULL;

void cApkMetaService::callback_Event (HINT32 event, const HINT32 *args, void *userdata)
{


}

HBOOL cApkMetaService::callback_UpdateChecker (const HCHAR *typeName, const void *_old, const void *_new, HBOOL *updated)
{

	return FALSE;
}

cApkMetaService::cApkMetaService(DBus::Connection &connection)
  : DBus::ObjectAdaptor(connection, BUSPATH_APK_META_SERVICE)
{
	if (NULL == pMainThis)
		pMainThis = this;

	/* apkit meta service */
    {	
		APK_META_SVC_Init (TRUE);
		//HBOOL APK_META_SVC_DeInit (void);
		//void APK_META_SVC_Delete (void *object);
		//void* APK_META_SVC_Clone (const void *object);
		//void APK_META_SVC_InitCommand(void);
		HERROR hErr = ERR_FAIL;
		hErr = APK_META_SVC_SetListener (cApkMetaService::callback_Event, this);
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("APK_META_SVC_SetListener Error : %d", hErr);
		}

		
		APK_META_SVC_SetUpdateChecker (cApkMetaService::callback_UpdateChecker);
    }
}

DbtService_t cApkMetaService::GetService(const int32_t& uid)
{
	DbtService_t	reval;

	DxService_t* pstService = NULL;
	pstService = APK_META_SVC_GetService ((HINT32)uid);

	if (NULL != pstService)
 		reval = cDbusUtilCvt::C2DxService (pstService);


	return reval;
}

DbtNetwork_t cApkMetaService::GetNetwork(const int32_t& uid)
{
	DbtNetwork_t reval;

	DxNetwork_t* pstNetwork = NULL;
	pstNetwork = APK_META_SVC_GetNetwork ((HINT32)uid);

	reval = cDbusUtilCvt::C2DxNetwork (pstNetwork);
	return reval;
}

DbtTransponder_t cApkMetaService::GetTransponder(const int32_t& uid)
{
	DbtTransponder_t reval;

	DxTransponder_t* pstTransponder = NULL;
	pstTransponder = APK_META_SVC_GetTransponder ((HINT32)uid);

	reval = cDbusUtilCvt::C2DxTransponder (pstTransponder);
	return reval;

}

DbtProvider_t cApkMetaService::GetProvider(const int32_t& uid)
{
	DbtProvider_t reval;

	DxProvider_t* pstProvider = NULL;
	pstProvider = APK_META_SVC_GetProvider ((HINT32)uid);

	reval = cDbusUtilCvt::C2DxProvider (pstProvider);
	return reval;
}

DbtGroup_t cApkMetaService::GetGroup(const int32_t& uid)
{
	DbtGroup_t reval;

	DxGroup_t* pstGroup = NULL;
	pstGroup = APK_META_SVC_GetGroup ((HINT32)uid);

	reval = cDbusUtilCvt::C2DxGroup (pstGroup);
	return reval;
}

DbtBouquet_t cApkMetaService::GetBouquet(const int32_t& uid)
{
	DbtBouquet_t reval;

	DxBouquet_t* pstBouquet = NULL;
	pstBouquet = APK_META_SVC_GetBouquet ((HINT32)uid);
	reval = cDbusUtilCvt::C2DxBouquet (pstBouquet);
	return reval;

}

DbtChannelLogo_t cApkMetaService::GetLogoUrl(const int32_t& uid, const DbtChannelLogo_t& bufChannelLogoInfo)
{
	DbtChannelLogo_t	reval;
#if 0
//	* (HINT32 uid, DxChannelLogo_t *buf);
	DxChannelLogo_t stChLogo;
	HxSTD_memset (&stChLogo, 0, sizeof(DxChannelLogo_t));
	{
		stChLogo.svcuid				=	bufChannelLogoInfo._1;
		HxSTD_StrNCpy(stChLogo.ucServerLogoUrl, &bufChannelLogoInfo._2[0], sizeof(HCHAR)*CHANNEL_LOGO_URL_LENGTH);
		HxSTD_StrNCpy(stChLogo.ucLocalLogoUrl, &bufChannelLogoInfo._3[0], sizeof(HCHAR)*CHANNEL_LOGO_URL_LENGTH);
	}

	DxChannelLogo_t* pstChannelLogo = NULL;
	pstChannelLogo = APK_META_SVC_GetLogoUrl ((HINT32)uid, &stChLogo);
	reval = cDbusUtilCvt::C2DxChannelLogo (pstChannelLogo);
#endif
	return reval;
}

::DBus::Struct< int32_t, int32_t, int32_t > cApkMetaService::GetServiceTriplet(const int32_t& uid)
{
	::DBus::Struct< int32_t, int32_t, int32_t > reval;
	HINT32 nTsid, nOnid, nSvcid;

	APK_META_SVC_GetServiceTriplet ((HINT32)uid, &nTsid, &nOnid, &nSvcid);

	{
		reval._1	=	(int32_t)nTsid;
		reval._2	=	(int32_t)nOnid;
		reval._3	=	(int32_t)nSvcid;
	}
	
	return reval;;
}

std::vector< DbtService_t > cApkMetaService::FindServiceByTriplet(const int32_t& onid, const int32_t& tsid, const int32_t& svcid)
{
	std::vector< DbtService_t > reval; 

	{
		HxVector_t * pstSvcList	=	NULL;

		pstSvcList = APK_META_SVC_FindServiceByTriplet ((HINT32)onid, (HINT32)tsid, (HINT32)svcid);
		if (pstSvcList != NULL)
		{
			HINT32 i;
			HINT32 nListCount = HLIB_VECTOR_Length (pstSvcList);

			for (i=0 ; i<nListCount ; i++)
			{
				DxService_t * pstService = (DxService_t*)HLIB_VECTOR_ItemAt(pstSvcList, i);

				if (NULL != pstService)
				{
					DbtService_t item;
					item = cDbusUtilCvt::C2DxService (pstService);
					reval.push_back (item);
				}
			}
			
			HLIB_VECTOR_Delete(pstSvcList);
		}
	}

	return reval;
}

std::vector< DbtService_t > cApkMetaService::FindServiceByNumber(const int32_t& number)
{
	std::vector< DbtService_t >	reval;

	{
		HxVector_t * pstSvcList	=	NULL;
	
		pstSvcList = APK_META_SVC_FindServiceByNumber ((HINT32)number);
		if (pstSvcList != NULL)
		{
			HINT32 i;
			HINT32 nListCount = HLIB_VECTOR_Length (pstSvcList);

			for (i=0 ; i<nListCount ; i++)
			{
				DxService_t * pstService = (DxService_t*)HLIB_VECTOR_ItemAt(pstSvcList, i);

				if (NULL != pstService)
				{
					DbtService_t item;
					item = cDbusUtilCvt::C2DxService (pstService);
					reval.push_back (item);
				}
			}
			
			HLIB_VECTOR_Delete(pstSvcList);
		}
	}

	return reval;
}

std::vector< DbtService_t > cApkMetaService::GetServiceList()
{
	std::vector< DbtService_t >	reval;

	{
		HxVector_t * pstSvcList	=	NULL;
	
		pstSvcList = APK_META_SVC_GetServiceList ();
		if (pstSvcList != NULL)
		{
			HINT32 i;
			HINT32 nListCount = HLIB_VECTOR_Length (pstSvcList);

			for (i=0 ; i<nListCount ; i++)
			{
				DxService_t * pstService = (DxService_t*)HLIB_VECTOR_ItemAt(pstSvcList, i);

				if (NULL != pstService)
				{
					DbtService_t item;
					item = cDbusUtilCvt::C2DxService (pstService);
					reval.push_back (item);
				}
			}
			
			HLIB_VECTOR_Delete(pstSvcList);
		}
	}

	return reval;
}

std::vector< DbtGroup_t > cApkMetaService::GetGroupList()
{
	std::vector< DbtGroup_t >	reval;

	{
		HxVector_t * pstGroupList	=	NULL;
	
		pstGroupList = APK_META_SVC_GetGroupList ();
		if (pstGroupList != NULL)
		{
			HINT32 i;
			HINT32 nListCount = HLIB_VECTOR_Length (pstGroupList);

			for (i=0 ; i<nListCount ; i++)
			{
				DxGroup_t * pstGroup = (DxGroup_t*)HLIB_VECTOR_ItemAt(pstGroupList, i);

				DbtGroup_t item;
				item = cDbusUtilCvt::C2DxGroup (pstGroup);

				reval.push_back (item);
			}
			
			HLIB_VECTOR_Delete(pstGroupList);
		}
	}

	return reval;
}

void cApkMetaService::Load()
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_META_SVC_Load ();
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_META_SVC_Load Error : %d", hErr);
	}
	return ;
}

void cApkMetaService::Save()
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_META_SVC_Save ();
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_META_SVC_Save Error : %d", hErr);
	}
	return ;
}

void cApkMetaService::SetService(const DbtService_t& service)
{
	HERROR hErr = ERR_FAIL;

	DxService_t stService;
	cDbusUtilCvt::C2DbtService (service, &stService);
	
	hErr = APK_META_SVC_SetService (&stService);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_META_SVC_SetService Error : %d", hErr);
	}
	return ;
}

void cApkMetaService::AddService(const DbtService_t& service)
{
	HERROR hErr = ERR_FAIL;

	DxService_t stService;
	cDbusUtilCvt::C2DbtService (service, &stService);

	hErr = APK_META_SVC_AddService (&stService);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_META_SVC_AddService Error : %d", hErr);
	}
	return ;
}

void cApkMetaService::RemoveService(const DbtService_t& service)
{
	HERROR hErr = ERR_FAIL;

	DxService_t stService;
	cDbusUtilCvt::C2DbtService (service, &stService);

	hErr = APK_META_SVC_RemoveService (&stService);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_META_SVC_RemoveService Error : %d", hErr);
	}
	return ;
}

void cApkMetaService::Reset()
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_META_SVC_Reset ();
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_META_SVC_Reset Error : %d", hErr);
	}
	return ;
}

void cApkMetaService::LoadPreferredList()
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_META_SVC_LoadPreferredList ();
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_META_SVC_LoadPreferredList Error : %d", hErr);
	}
	return ;
}

void cApkMetaService::LoadupdatedList()
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_META_SVC_LoadupdatedList ();
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_META_SVC_LoadupdatedList Error : %d", hErr);
	}
	return ;
}

void cApkMetaService::ChangeUpdateFlag(const uint32_t& fromFlag, const uint32_t& toFlag)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_META_SVC_ChangeUpdateFlag ((DxSvcUpdateFlag_e)fromFlag, (DxSvcUpdateFlag_e)toFlag);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_META_SVC_ChangeUpdateFlagError : %d", hErr);
	}
	return ;
}

void cApkMetaService::RemoveServiceWithFlag(const uint32_t& flag)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_META_SVC_RemoveServiceWithFlag ((DxSvcUpdateFlag_e)flag);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_META_SVC_RemoveServiceWithFlagError : %d", hErr);
	}
	return ;
}
