#include <apk.h>


#include <vkernel.h>

#include <dbus-c++/dbus.h>
#include <as_apk_cas_nagra.h>

#include <oapi_cas_nagra.h>

char const * const cApkCasNagra::BUSNAME_APK_CAS_NAGRA = "Octopus.Appkit.Cas.Nagra";
char const * const cApkCasNagra::BUSPATH_APK_CAS_NAGRA = "/Octopus/Appkit/Cas/Nagra";

cApkCasNagra * cApkCasNagra::pMainThis	=	NULL;

void cApkCasNagra::callback_Event (HBOOL *pbRet)
{
	if (NULL == pMainThis)
		return;

	//pbRet
	
	uint32_t param1=0, param2=0, param3=0;

	param1 = (uint32_t)0;//ulParam1;
	param2 = (uint32_t)0;//ulParam2;
	param3 = (uint32_t)0;//ulParam3;

	pMainThis->event (param1, param2, param3);					
}

cApkCasNagra::cApkCasNagra(DBus::Connection &connection)
  : DBus::ObjectAdaptor(connection, BUSPATH_APK_CAS_NAGRA)
{
	if (NULL == pMainThis)
		pMainThis = this;

	/* apkit cas nagra */
    {	
		HERROR hErr = ERR_FAIL;
		hErr = APK_CAS_NAGRA_Init ();
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("APK_CAS_NAGRA_Init Error : %d", hErr);
		}
		
		hErr = APK_CAS_NAGRA_RegisterNotifier (cApkCasNagra::callback_Event);
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("APK_CAS_NAGRA_RegisterNotifier Error : %d", hErr);
		}
		
		//HERROR APK_CAS_NAGRA_UnregisterNotifier(void);
    }
}

bool cApkCasNagra::IsCWE()
{
	HERROR hErr = ERR_FAIL;

	HBOOL bCWE = FALSE;

	hErr = APK_CAS_NAGRA_IsCWE (&bCWE);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_CAS_NAGRA_IsCWE Error : %d", hErr);
	}

	return (bool)bCWE;
}

bool cApkCasNagra::IsJTAG()
{
	HERROR hErr = ERR_FAIL;

	HBOOL bJTAG = FALSE;

	hErr = APK_CAS_NAGRA_IsJTAG (&bJTAG);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_CAS_NAGRA_IsJTAG Error : %d", hErr);
	}

	return (bool)bJTAG;

}

bool cApkCasNagra::IsSBOOT()
{
	HERROR hErr = ERR_FAIL;

	HBOOL bSBOOT = FALSE;

	hErr = APK_CAS_NAGRA_IsSBOOT (&bSBOOT);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_CAS_NAGRA_IsSBOOT Error : %d", hErr);
	}

	return (bool)bSBOOT;

}

bool cApkCasNagra::IsOTP()
{
	HERROR hErr = ERR_FAIL;

	HBOOL bOTP = FALSE;

	hErr = APK_CAS_NAGRA_IsOTP (&bOTP);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_CAS_NAGRA_IsOTP Error : %d", hErr);
	}

	return (bool)bOTP;

}

std::string cApkCasNagra::GetNUID()
{
	HERROR hErr = ERR_FAIL;

	HCHAR szNUID[OAPI_CAS_NAGRA_SYSTEM_INFO_COMPONENT_LENG] = {0,};
	
	hErr = APK_CAS_NAGRA_GetNUID (szNUID);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_CAS_NAGRA_GetNUID Error : %d", hErr);
	}

	return std::string(szNUID);
}

std::string cApkCasNagra::GetCASN()
{
	HERROR hErr = ERR_FAIL;

	HCHAR szCASN[OAPI_CAS_NAGRA_SYSTEM_INFO_COMPONENT_LENG] = {0,};

	hErr = APK_CAS_NAGRA_GetCASN (szCASN);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_CAS_NAGRA_GetCASN Error : %d", hErr);
	}

	return std::string(szCASN);
}

::DBus::Struct< bool, bool, bool, bool > cApkCasNagra::GetFusingInfo()
{
	::DBus::Struct< bool, bool, bool, bool > reval;

	HERROR hErr = ERR_FAIL;
	HUINT8 ucSCS, ucJTAG, ucOTP, ucCWE;
	
	hErr = APK_CAS_NAGRA_GetFusingInfo (&ucSCS, &ucJTAG, &ucOTP, &ucCWE);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_CAS_NAGRA_GetFusingInfo Error : %d", hErr);	
	}

	{
		reval._1	=	(bool)ucSCS;
		reval._2	=	(bool)ucJTAG;
		reval._3	=	(bool)ucOTP;
		reval._4	=	(bool)ucCWE;
	}

	return reval;
}

bool cApkCasNagra::IsSCInserted()
{
	return (bool)APK_CAS_NAGRA_IsSCInserted ();
}

bool cApkCasNagra::VerifyPincode(const std::string& pinStr)
{
	return (bool)APK_CAS_NAGRA_PINCODE_Verify ((const HCHAR *)&pinStr[0]);
}

::DBus::Struct< uint32_t, uint32_t, uint32_t, bool > cApkCasNagra::GetAccessibleState()
{
	::DBus::Struct< uint32_t, uint32_t, uint32_t, bool > reval;

	HBOOL	bReval;
	HUINT8 nAccessState;
	HUINT32 ulRemainTime;
	HUINT8 nRetryCount;

	bReval = APK_CAS_NAGRA_GetAccessibleState (&nAccessState, &ulRemainTime, &nRetryCount);

	{
		reval._1	=	(uint32_t)nAccessState;
		reval._2	=	(uint32_t)ulRemainTime;
		reval._3	=	(uint32_t)nRetryCount;
		reval._4	=	(bool)bReval;
	}
	
	return reval;
}

::DBus::Struct< uint32_t, uint32_t, uint32_t, bool > cApkCasNagra::VerifyPincodeGetAccessibleState(const std::string& pinStr)
{
	::DBus::Struct< uint32_t, uint32_t, uint32_t, bool > reval;
	
	HBOOL	bReval;
	HUINT8 nAccessState;
	HUINT32 ulRemainTime;
	HUINT8 nRetryCount;

	bReval = APK_CAS_NAGRA_VerifyPincodeGetAccessibleState (&nAccessState, &ulRemainTime, &nRetryCount, (const HCHAR *)&pinStr[0]);

	{
		reval._1	=	(uint32_t)nAccessState;
		reval._2	=	(uint32_t)ulRemainTime;
		reval._3	=	(uint32_t)nRetryCount;
		reval._4	=	(bool)bReval;
	}

	return reval;
}

bool cApkCasNagra::SetPincode(const std::string& oldPinStr, const std::string& newPinStr)
{
	return (bool)APK_CAS_NAGRA_PINCODE_Set ((const HCHAR *)&oldPinStr[0], (const HCHAR *)&newPinStr[0]);
}
