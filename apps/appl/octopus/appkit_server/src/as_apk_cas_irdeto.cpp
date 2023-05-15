#include <apk.h>


#include <vkernel.h>

#include <dbus-c++/dbus.h>
#include <as_apk_cas_irdeto.h>

#include <oapi_cas_irdeto.h>

char const * const cApkCasIrdeto::BUSNAME_APK_CAS_IRDETO = "Octopus.Appkit.Cas.Irdeto";
char const * const cApkCasIrdeto::BUSPATH_APK_CAS_IRDETO = "/Octopus/Appkit/Cas/Irdeto";

cApkCasIrdeto * cApkCasIrdeto::pMainThis	=	NULL;

void cApkCasIrdeto::callback_Event (void)
{
	if (NULL == pMainThis)
		return;
	
	uint32_t param1=0, param2=0, param3=0;

	param1 = (uint32_t)0;
	param2 = (uint32_t)0;
	param3 = (uint32_t)0;

	pMainThis->event (param1, param2, param3);					
}


cApkCasIrdeto::cApkCasIrdeto(DBus::Connection &connection)
  : DBus::ObjectAdaptor(connection, BUSPATH_APK_CAS_IRDETO)
{
	if (NULL == pMainThis)
		pMainThis = this;

	/* apkit cas irdeto */
    {	
		HERROR hErr = ERR_FAIL;
		hErr = APK_CAS_IRDETO_Init ();
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("APK_CAS_IRDETO_Init Error : %d", hErr);
		}

		hErr = APK_CAS_IRDETO_RegisterNotifier (cApkCasIrdeto::callback_Event);
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("APK_CAS_IRDETO_RegisterNotifier Error : %d", hErr);
		}

		//HERROR APK_CAS_IRDETO_UnregisterNotifier(void);
		
    }
}

void cApkCasIrdeto::SetVirtualSecureCwMode(const bool& virtualSecureCwMode)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_CAS_IRDETO_SetVirtualSecureCwMode ((HBOOL)virtualSecureCwMode);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_CAS_IRDETO_SetVirtualSecureCwMode Error : %d", hErr);
	}

	return ;
}

bool cApkCasIrdeto::GetVirtualSecureCwMode()
{
	HERROR hErr = ERR_FAIL;

	HBOOL bCwMode = FALSE;
		
	hErr = APK_CAS_IRDETO_GetVirturlSecureCwMode (&bCwMode);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_CAS_IRDETO_GetVirturlSecureCwMode Error : %d", hErr);
	}

	return (bool)bCwMode;
}

std::string cApkCasIrdeto::GetIrdetoClientVersionName()
{
	HERROR hErr = ERR_FAIL;

	HCHAR szVesionName[OAPI_CAS_IRDETO_CLIENT_VERSION_LENG]={0,};
		
	hErr = APK_CAS_IRDETO_GetIrdetoClientVersionName (szVesionName);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_CAS_IRDETO_GetIrdetoClientVersionName Error : %d", hErr);
	}

	return std::string(szVesionName);
}

bool cApkCasIrdeto::IsCWE()
{
	HERROR hErr = ERR_FAIL;

	HBOOL bCWE = FALSE;

	hErr = APK_CAS_IRDETO_IsCWE (&bCWE);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_CAS_IRDETO_IsCWE Error : %d", hErr);
	}

	return (bool)bCWE;
}

bool cApkCasIrdeto::IsJTAG()
{
	HERROR hErr = ERR_FAIL;

	HBOOL bJTAG = FALSE;

	hErr = APK_CAS_IRDETO_IsJTAG (&bJTAG);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_CAS_IRDETO_IsJTAG Error : %d", hErr);
	}

	return (bool)bJTAG;
}

bool cApkCasIrdeto::IsSBOOT()
{
	HERROR hErr = ERR_FAIL;

	HBOOL bSBOOT = FALSE;

	hErr = APK_CAS_IRDETO_IsSBOOT (&bSBOOT);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_CAS_IRDETO_IsSBOOT Error : %d", hErr);
	}

	return (bool)bSBOOT;
}

bool cApkCasIrdeto::IsOTP()
{
	HERROR hErr = ERR_FAIL;

	HBOOL bOTP = FALSE;

	hErr = APK_CAS_IRDETO_IsOTP (&bOTP);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_CAS_IRDETO_IsOTP Error : %d", hErr);
	}

	return (bool)bOTP;
}

std::string cApkCasIrdeto::GetIrdetoManCode()
{
	HERROR hErr = ERR_FAIL;

	HCHAR szMancode[OAPI_CAS_IRDETO_SYSTEM_INFO_COMPONENT_LENG] = {0,};
		
	hErr = APK_CAS_IRDETO_GetIrdetoManCode (szMancode);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_CAS_IRDETO_GetIrdetoManCode Error : %d", hErr);
	}

	return std::string(szMancode);

}

std::string cApkCasIrdeto::GetIrdetoHwCode()
{
	HERROR hErr = ERR_FAIL;

	HCHAR szHWCode[OAPI_CAS_IRDETO_SYSTEM_INFO_COMPONENT_LENG] = {0,};
	
	hErr = APK_CAS_IRDETO_GetIrdetoHwCode (szHWCode);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_CAS_IRDETO_GetIrdetoHwCode Error : %d", hErr);
	}

	return std::string(szHWCode);
}

std::string cApkCasIrdeto::GetIrdetoVariant()
{
	HERROR hErr = ERR_FAIL;

	HCHAR szVariant[OAPI_CAS_IRDETO_SYSTEM_INFO_COMPONENT_LENG] = {0,};

	hErr = APK_CAS_IRDETO_GetIrdetoVariant (szVariant);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_CAS_IRDETO_GetIrdetoVariant Error : %d", hErr);
	}

	return std::string(szVariant);
}

std::string cApkCasIrdeto::GetIrdetoSysId()
{
	HERROR hErr = ERR_FAIL;

	HCHAR szSysId[OAPI_CAS_IRDETO_SYSTEM_INFO_COMPONENT_LENG] = {0,};

	hErr = APK_CAS_IRDETO_GetIrdetoSysId (szSysId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_CAS_IRDETO_GetIrdetoSysId Error : %d", hErr);
	}

	return std::string(szSysId);
}

std::string cApkCasIrdeto::GetIrdetoKeyVer()
{
	HERROR hErr = ERR_FAIL;

	HCHAR szKeyVer[OAPI_CAS_IRDETO_SYSTEM_INFO_COMPONENT_LENG] = {0,};

	hErr = APK_CAS_IRDETO_GetIrdetoKeyVer (szKeyVer);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_CAS_IRDETO_GetIrdetoKeyVer Error : %d", hErr);
	}

	return std::string(szKeyVer);
}

std::string cApkCasIrdeto::GetIrdetoSigVer()
{
	HERROR hErr = ERR_FAIL;

	HCHAR szSigVer[OAPI_CAS_IRDETO_SYSTEM_INFO_COMPONENT_LENG] = {0,};
	
	hErr = APK_CAS_IRDETO_GetIrdetoSigVer (szSigVer);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_CAS_IRDETO_GetIrdetoSigVer Error : %d", hErr);
	}

	return std::string(szSigVer);
}

uint32_t cApkCasIrdeto::GetControlByte()
{
	HERROR hErr = ERR_FAIL;

	HUINT8 ucControlByte = 0;
		
	hErr = APK_CAS_IRDETO_GetControlByte (&ucControlByte);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_CAS_IRDETO_GetControlByte Error : %d", hErr);
	}

	return (uint32_t)ucControlByte;
}
