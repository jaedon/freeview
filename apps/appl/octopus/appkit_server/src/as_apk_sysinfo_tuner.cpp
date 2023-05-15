#include <apk.h>


#include <vkernel.h>

#include <dbus-c++/dbus.h>
#include <as_apk_sysinfo_tuner.h>

char const * const cApkSysinfoTuner::BUSNAME_APK_SYSINFO_TUNER = "Octopus.Appkit.Sysinfo.Tuner";
char const * const cApkSysinfoTuner::BUSPATH_APK_SYSINFO_TUNER = "/Octopus/Appkit/Sysinfo/Tuner";

cApkSysinfoTuner * cApkSysinfoTuner::pMainThis	=	NULL;

void cApkSysinfoTuner::callback_Event (void)
{
	if (NULL == pMainThis)
		return;

	pMainThis->event ();					
}

cApkSysinfoTuner::cApkSysinfoTuner(DBus::Connection &connection)
  : DBus::ObjectAdaptor(connection, BUSPATH_APK_SYSINFO_TUNER)
{
	if (NULL == pMainThis)
		pMainThis = this;

	/* apkit sysinfo tuner */
    {	
		HERROR hErr = ERR_FAIL;
		hErr = APK_SYSINFO_TUNER_Init ();
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("APK_SYSINFO_TUNER_Init Error : %d", hErr);
		}

		//HERROR APK_SYSINFO_TUNER_DeInit(void);
		hErr = APK_SYSINFO_TUNER_RegisterListener (cApkSysinfoTuner::callback_Event);
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("APK_SYSINFO_TUNER_RegisterListener Error : %d", hErr);
		}

		
		//HERROR APK_SYSINFO_TUNER_UnregisterListener(APK_SYSINFO_TUNER_Notifier_t fnNotifier);
    }
}

uint32_t cApkSysinfoTuner::GetDeviceNum()
{
	HERROR hErr = ERR_FAIL;

	HUINT32 ulDevNum = 0;
		
	hErr = APK_SYSINFO_TUNER_GetDeviceNum (&ulDevNum);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_SYSINFO_TUNER_GetDeviceNum Error : %d", hErr);
	}

	return (uint32_t)ulDevNum;
}

uint32_t cApkSysinfoTuner::GetDevType(const int32_t& index)
{
	HERROR hErr = ERR_FAIL;

	DxDeliveryType_e eDeliType	=	eDxDELIVERY_TYPE_ALL;
		
	hErr = APK_SYSINFO_TUNER_GetDevType ((HINT32)index, &eDeliType);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_SYSINFO_TUNER_GetDevType Error : %d", hErr);
	}

	return (uint32_t)eDeliType;
}

bool cApkSysinfoTuner::IsLocked(const int32_t& index)
{
	HERROR hErr = ERR_FAIL;

	HBOOL bLocked = FALSE;
		
	hErr = APK_SYSINFO_TUNER_IsLocked ((HINT32)index, &bLocked);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_SYSINFO_TUNER_IsLocked Error : %d", hErr);
	}

	return (bool)bLocked;
}

int32_t cApkSysinfoTuner::GetSignalLevel(const int32_t& index)
{
	HERROR hErr = ERR_FAIL;

	HINT32 nSignalLevel = 0;
		
	hErr = APK_SYSINFO_TUNER_GetSignalLevel ((HINT32)index, &nSignalLevel);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_SYSINFO_TUNER_GetSignalLevel Error : %d", hErr);
	}

	return (int32_t)nSignalLevel;
}

int32_t cApkSysinfoTuner::GetSignalQuality(const int32_t& index)
{
	HERROR hErr = ERR_FAIL;

	HINT32 nSignalQuality = 0;

	hErr = APK_SYSINFO_TUNER_GetSignalQuality ((HINT32)index, &nSignalQuality);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_SYSINFO_TUNER_GetSignalQuality Error : %d", hErr);
	}


	return (int32_t)nSignalQuality;
}

int32_t cApkSysinfoTuner::GetAgc(const int32_t& index)
{
	HERROR hErr = ERR_FAIL;

	HINT32 nAgc = 0;
		
	hErr = APK_SYSINFO_TUNER_GetAgc ((HINT32)index, &nAgc);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_SYSINFO_TUNER_GetAgc Error : %d", hErr);
	}

	return (int32_t)nAgc;
}

double cApkSysinfoTuner::GetBer(const int32_t& index)
{
	HERROR hErr = ERR_FAIL;

	HFLOAT32 fBer;
	
	hErr = APK_SYSINFO_TUNER_GetBer ((HINT32)index, &fBer);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_SYSINFO_TUNER_GetBer Error : %d", hErr);
	}

	return (double)fBer;
}

double cApkSysinfoTuner::GetSnr(const int32_t& index)
{
	HERROR hErr = ERR_FAIL;

	HFLOAT32 fSnr;

	hErr = APK_SYSINFO_TUNER_GetSnr ((HINT32)index, &fSnr);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_SYSINFO_TUNER_GetSnr Error : %d", hErr);
	}

	return (double)fSnr;
}

double cApkSysinfoTuner::GetInputPower(const int32_t& index)
{
	HERROR hErr = ERR_FAIL;

	HFLOAT32 fInputPower;

	hErr = APK_SYSINFO_TUNER_GetInputPower ((HINT32)index, &fInputPower);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_SYSINFO_TUNER_GetInputPower Error : %d", hErr);
	}

	return (double)fInputPower;
}

::DBus::Struct< uint32_t > cApkSysinfoTuner::GetTuneParam(const int32_t& index)
{
	::DBus::Struct< uint32_t > reval;

	HERROR hErr = ERR_FAIL;

	DxTuneParam_t stTuneParam;
	
	hErr = APK_SYSINFO_TUNER_GetTuneParam ((HINT32)index, &stTuneParam);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_SYSINFO_TUNER_GetTuneParam Error : %d", hErr);
	}

	// union 으로된 DxTuneParam_t 의 3가지 type 전달 방식 고려 해야함.
	{
		reval._1	=	stTuneParam.eDeliType;
	}

	return reval;
}

int32_t cApkSysinfoTuner::GetTunerIdByTripleId(const int32_t& onId, const int32_t& tsId, const int32_t& svcId)
{
	HERROR hErr = ERR_FAIL;

	HINT32 nTunerId = 0;
		
	hErr = APK_SYSINFO_TUNER_GetTunerIdByTripleId ((HINT32)onId, (HINT32)tsId, (HINT32)svcId, &nTunerId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_SYSINFO_TUNER_GetTunerIdByTripleId Error : %d", hErr);
	}

	return (int32_t)nTunerId;
}
