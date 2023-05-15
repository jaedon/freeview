#include <apk.h>


#include <vkernel.h>

#include <dbus-c++/dbus.h>
#include <as_apk_cas_ci.h>

#include <oapi_cas_ci.h>

char const * const cApkCasCi::BUSNAME_APK_CAS_CI = "Octopus.Appkit.Cas.Ci";
char const * const cApkCasCi::BUSPATH_APK_CAS_CI = "/Octopus/Appkit/Cas/Ci";

cApkCasCi * cApkCasCi::pMainThis	=	NULL;

void cApkCasCi::callback_Event (HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3)
{
	if (NULL == pMainThis)
		return;
	
	uint32_t param1=0, param2=0, param3=0;

	param1 = (uint32_t)ulParam1;
	param2 = (uint32_t)ulParam2;
	param3 = (uint32_t)ulParam3;

	pMainThis->event (param1, param2, param3);					
}

void cApkCasCi::mmi_callback_Event (CI_AMMI_MSG_TYPE enMsgType, void* pvUserData, HUINT32 ulSize)
{
	if (NULL == pMainThis)
		return;

	uint32_t param1=0;
	std::vector< uint8_t > param2;
		
	param1 = (uint32_t)enMsgType;
//	param2 = pvUserData

	pMainThis->mmi_event (param1, param2);					
}

cApkCasCi::cApkCasCi(DBus::Connection &connection)
  : DBus::ObjectAdaptor(connection, BUSPATH_APK_CAS_CI)
{
	if (NULL == pMainThis)
		pMainThis = this;

	/* apkit cas ci */
    {	
		HERROR hErr = ERR_FAIL;
		
		hErr = APK_CAS_CI_Init();
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("APK_CAS_CI_Init Error : %d", hErr);
		}

		APK_CAS_CI_RegisterNotifier (cApkCasCi::callback_Event);
		//void APK_CAS_CI_UnRegisterNotifier(APK_CAS_CI_Notifier_t fnNotifier);

		APK_CAS_CI_RegisterAppMmiCallback (cApkCasCi::mmi_callback_Event);
		//void APK_CAS_CI_UnRegisterAppMmiCallback(APK_CAS_CI_AppMmiNotifier_t pfnCallback);
		
    }
}

bool cApkCasCi::IsSupportedCI()
{
	HERROR hErr = ERR_FAIL;

	HBOOL bSupported = FALSE;
		
	hErr = APK_CAS_CI_IsSupportedCI (&bSupported);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_CAS_CI_IsSupportedCI Error : %d", hErr);
	}

	return (bool)bSupported;
}

bool cApkCasCi::IsSupportedCIPlus()
{
	HERROR hErr = ERR_FAIL;

	HBOOL bSupported = FALSE;

	hErr = APK_CAS_CI_IsSupportedCIPlus (&bSupported);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_CAS_CI_IsSupportedCIPlus Error : %d", hErr);
	}

	return (bool)bSupported;
}

std::string cApkCasCi::GetCIPlusVersionName()
{
	HERROR hErr = ERR_FAIL;

	HCHAR szVersionName[APKS_CI_PLUS_VER_LENG]={0,};
	
	hErr = APK_CAS_CI_GetCIPlusVersionName (szVersionName);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_CAS_CI_GetCIPlusVersionName Error : %d", hErr);
	}

	return std::string(szVersionName);
}

uint32_t cApkCasCi::GetSlotCount()
{
	HERROR hErr = ERR_FAIL;

	HUINT32 ulSlotCount = 0;
	
	hErr = APK_CAS_CI_GetSlotCount (&ulSlotCount);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_CAS_CI_GetSlotCount Error : %d", hErr);
	}

	return (uint32_t)ulSlotCount;
}


uint32_t cApkCasCi::GetCamNumberBySlot(const uint32_t& slot)
{
	HERROR hErr = ERR_FAIL;

	HUINT32 ulCamNumber = 0;

	hErr = APK_CAS_CI_GetCamNumberBySlot ((HUINT32)slot, &ulCamNumber);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_CAS_CI_GetCamNumberBySlot Error : %d", hErr);
	}

	return (uint32_t)ulCamNumber;
}

uint32_t cApkCasCi::GetCamStatusByIndex(const uint32_t& slot)
{
	HERROR hErr = ERR_FAIL;

	DxCASCI_CamStatus_e eCamStatus = eDxCASCI_CAM_STATUS_Extracted;

	hErr = APK_CAS_CI_GetCamStatusByIndex ((HUINT32)slot, &eCamStatus);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_CAS_CI_GetCamStatusByIndex Error : %d", hErr);
	}

	return (uint32_t)eCamStatus;
}

std::string cApkCasCi::GetCamNameByIndex(const uint32_t& slot)
{
	HERROR hErr = ERR_FAIL;

	HCHAR szCamName[APKS_CAM_NAME_LEN] = {0,};

	hErr = APK_CAS_CI_GetCamNameByIndex ((HUINT32)slot, szCamName);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_CAS_CI_GetCamNameByIndex Error : %d", hErr);
	}

	return std::string(szCamName);
}

void cApkCasCi::RequestFileorData(const uint32_t& sessionId, const uint32_t& reqType, const std::string& reqName)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_CAS_CI_RequestFileorData ((HUINT16 )sessionId, (CI_AMMI_REQUEST_TYPE)reqType, (HUINT8 *)&reqName[0], (HULONG)reqName.size());
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_CAS_CI_RequestFileorData Error : %d", hErr);
	}	

	return ;
}

void cApkCasCi::RequestAppAbort(const uint32_t& sessionId, const uint32_t& abortReqCode)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_CAS_CI_RequestAppAbort ((HUINT16 )sessionId, (CI_AMMI_ABORT_REQUEST_CODE)abortReqCode);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_CAS_CI_RequestAppAbort Error : %d", hErr);
	}

	return ;
}
