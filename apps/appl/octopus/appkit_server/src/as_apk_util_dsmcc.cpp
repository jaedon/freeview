#include <apk.h>


#include <vkernel.h>

#include <dbus-c++/dbus.h>

#include <dbus_util_cvt.h>
#include <as_apk_util_dsmcc.h>

char const * const cApkUtilDsmcc::BUSNAME_APK_UTIL_DSMCC = "Octopus.Appkit.Util.Dsmcc";
char const * const cApkUtilDsmcc::BUSPATH_APK_UTIL_DSMCC = "/Octopus/Appkit/Util/Dsmcc";

cApkUtilDsmcc * cApkUtilDsmcc::pMainThis	=	NULL;

void cApkUtilDsmcc::callback_Event (HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3)
{
	if (NULL == pMainThis)
		return;
	
	uint32_t param1=0, param2=0, param3=0;

	param1 = (uint32_t)ulParam1;
	param2 = (uint32_t)ulParam2;
	param3 = (uint32_t)ulParam3;

	pMainThis->event (param1, param2, param3);					
}

cApkUtilDsmcc::cApkUtilDsmcc(DBus::Connection &connection)
  : DBus::ObjectAdaptor(connection, BUSPATH_APK_UTIL_DSMCC)
{
	/* apkit util dsmcc */
    {	
		APK_DSMCC_Init();
		//void APK_DSMCC_DeInit(void);
	}
}

::DBus::Struct< uint32_t, std::string > cApkUtilDsmcc::Create(const uint32_t& type)
{
	::DBus::Struct< uint32_t, std::string > reval;
	
	HERROR hErr = ERR_FAIL;

	HCHAR *pszCachePath = NULL;
	Handle_t hHandle;

	hErr = APK_DSMCC_Create ((DxDSMCC_Type_e)type, &hHandle, &pszCachePath);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_DSMCC_Create Error : %d", hErr);
	}

	{
		reval._1 = (uint32_t)hHandle;
		reval._2 = std::string(pszCachePath);
	}

	APK_DSMCC_RegisterNotifier ((Handle_t)hHandle, cApkUtilDsmcc::callback_Event);

	APK_DSMCC_FreePath (pszCachePath);

	return reval;
}

void cApkUtilDsmcc::Destroy(const uint32_t& handle)
{
	HERROR hErr = ERR_FAIL;

	APK_DSMCC_UnRegisterNotifier ((Handle_t)handle, cApkUtilDsmcc::callback_Event);

	hErr = APK_DSMCC_Destroy ((Handle_t)handle);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_DSMCC_Destroy Error : %d", hErr);
	}

	return ;
}

DbtDSMCC_CarouselList_t cApkUtilDsmcc::GetCarouselList(const uint32_t& handle)
{
	DbtDSMCC_CarouselList_t reval;

	HERROR hErr = ERR_FAIL;

	DxDSMCC_CarouselList_t stList;
	HxSTD_memset (&stList, 0, sizeof(DxDSMCC_CarouselList_t));

	hErr = APK_DSMCC_GetCarouselList ((Handle_t)handle, &stList);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_DSMCC_GetCarouselList Error : %d", hErr);
	}

	reval = cDbusUtilCvt::C2DxDSMCC_CarouselList (&stList);
	return reval;
}

void cApkUtilDsmcc::Start(const uint32_t& handleDsmcc, const uint32_t& handleCarousel, const std::string& cachePath)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_DSMCC_Start ((Handle_t)handleDsmcc, (Handle_t)handleCarousel, (const HCHAR *)&cachePath[0]);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_DSMCC_Start Error : %d", hErr);
	}

	return ;
}

void cApkUtilDsmcc::ReStart(const uint32_t& handleDsmcc, const uint32_t& handleCarousel)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_DSMCC_Restart ((Handle_t)handleDsmcc, (Handle_t)handleCarousel);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_DSMCC_Restart Error : %d", hErr);
	}

	return ;
}

void cApkUtilDsmcc::Stop(const uint32_t& handleDsmcc, const uint32_t& handleCarousel)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_DSMCC_Stop ((Handle_t)handleDsmcc, (Handle_t)handleCarousel);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_DSMCC_Stop Error : %d", hErr);
	}

	return ;
}

void cApkUtilDsmcc::RequestEvent(const uint32_t& handleDsmcc, const bool& all)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_DSMCC_RequestEvent ((Handle_t)handleDsmcc, (HBOOL)all);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_DSMCC_RequestEvent Error : %d", hErr);
	}

	return ;
}

void cApkUtilDsmcc::Open(const uint32_t& handleDsmcc, const uint32_t& handleSession)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_DSMCC_Open ((Handle_t)handleDsmcc, (Handle_t)handleSession);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_DSMCC_Open Error : %d", hErr);
	}

	return ;
}

void cApkUtilDsmcc::Close(const uint32_t& handleDsmcc, const uint32_t& handleSession)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_DSMCC_Close ((Handle_t)handleDsmcc, (Handle_t)handleSession);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_DSMCC_Close Error : %d", hErr);
	}

	return ;
}
