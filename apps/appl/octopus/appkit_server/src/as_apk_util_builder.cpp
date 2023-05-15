#include <apk.h>


#include <vkernel.h>

#include <dbus-c++/dbus.h>

#include <dbus_util_cvt.h>
#include <as_apk_util_builder.h>

char const * const cApkUtilBuilder::BUSNAME_APK_UTIL_BUILDER = "Octopus.Appkit.Util.Builder";
char const * const cApkUtilBuilder::BUSPATH_APK_UTIL_BUILDER = "/Octopus/Appkit/Util/Builder";

STATIC const char * _apk_GetEventString (DxBuilder_Event_e eEvent)
{
	switch (eEvent)
	{
		ENUM_TO_STR (eDxBUILDER_SEARCH_STATUS_CHANGED)

	default:
		break;
	}

	return "Unknown";
}

void cApkUtilBuilder::callback_Event (HUINT32 ulBuilderId, DxBuilder_Event_e eEvent, DxBuilder_NotifierData_t *pstNotifyData, void *pvUserData)
{
	cApkUtilBuilder *pThis = (cApkUtilBuilder *)pvUserData;
	std::string	eventStr(_apk_GetEventString(eEvent));

	uint32_t param1=0, param2=0, param3=0, param4=0;

	/* emit signal */
	if (NULL != pstNotifyData)
	{
		param1 = (uint32_t)pstNotifyData->stStatusChanged.ulTotalTpNum;
		param2 = (uint32_t)pstNotifyData->stStatusChanged.ulFinishedTpNum;
		param3 = (uint32_t)pstNotifyData->stStatusChanged.ulSearchedSvcNum;
		param4 = (DxBuilder_State_e)pstNotifyData->stStatusChanged.eState;

		//DxTuneParam_t				 stTuneParam;
	}

	pThis->event ((uint32_t)eEvent, eventStr, param1, param2, param3, param4);
}

cApkUtilBuilder::cApkUtilBuilder(DBus::Connection &connection)
  : DBus::ObjectAdaptor(connection, BUSPATH_APK_UTIL_BUILDER)
{
	/* apkit util builder */
    {	
		HERROR hErr = ERR_FAIL;

		hErr = APK_BUILDER_Init ();
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("APK_BUILDER_Init Error : %d", hErr);
		}
    }
}

uint32_t cApkUtilBuilder::OpenSession(const uint32_t& deliveryType, const std::string& scanType, const ::DBus::Struct< uint32_t, uint32_t, std::string >& actionInfo)
{
	HERROR hErr = ERR_FAIL;

	ApkBuilder_Action_t stInfo;
	HUINT32 hBuilderHandle;

	{
		HxSTD_memset (&stInfo, 0, sizeof(ApkBuilder_Action_t));
		stInfo.eDeliveryType 	= 	(DxDeliveryType_b)actionInfo._1;
		stInfo.utInfo.satInfo.ulAntId			=	(HUINT32)actionInfo._2;
		HxSTD_StrNCpy (stInfo.utInfo.satInfo.szSatelliteName, &actionInfo._3[0], DxBUILDER_SATELLITE_NAME_LEN-1);			
	}
	
	hErr = APK_BUILDER_OpenSession ((DxDeliveryType_e)deliveryType, (HCHAR *)&scanType[0], &stInfo, &hBuilderHandle);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_BUILDER_OpenSession Error : %d", hErr);
	}


	hErr = APK_BUILDER_RegisterNotifier (hBuilderHandle, cApkUtilBuilder::callback_Event, this);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_BUILDER_RegisterNotifier Error : %d", hErr);
	}
	
	return (uint32_t)hBuilderHandle;
}

void cApkUtilBuilder::CloseSession(const uint32_t& builderHandle)
{
	HERROR hErr = ERR_FAIL;

	hErr = APK_BUILDER_UnregisterNotifier ((HUINT32)builderHandle, cApkUtilBuilder::callback_Event, this);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_BUILDER_UnregisterNotifier Error : %d", hErr);
	}

	hErr = APK_BUILDER_CloseSession ((HUINT32)builderHandle);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_BUILDER_CloseSession Error : %d", hErr);
	}

	return ;
}

void cApkUtilBuilder::ClearBuildInfo(const uint32_t& builderHandle)
{
	HERROR hErr = ERR_FAIL;

	hErr = APK_BUILDER_ClearBuildInfo ((HUINT32)builderHandle);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_BUILDER_ClearBuildInfo Error : %d", hErr);
	}	

	return ;
}

void cApkUtilBuilder::AddBuildInfo_Sat(const uint32_t& builderHandle, const DbtTuneParamSat_t& tunerParam)
{
	HERROR hErr = ERR_FAIL;
	DxTuneParam_t stInfo;
	cDbusUtilCvt::C2DbtTuneParamSat (tunerParam, &stInfo);
	
	hErr = APK_BUILDER_AddBuildInfo ((HUINT32)builderHandle, &stInfo);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_BUILDER_AddBuildInfo Error : %d", hErr);
	}	

	return ;
}

void cApkUtilBuilder::AddBuildInfo_Ter(const uint32_t& builderHandle, const DbtTuneParamTer_t& tunerParam)
{
	HERROR hErr = ERR_FAIL;
	DxTuneParam_t stInfo;
	cDbusUtilCvt::C2DbtTuneParamTer (tunerParam, &stInfo);
	
	hErr = APK_BUILDER_AddBuildInfo ((HUINT32)builderHandle, &stInfo);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_BUILDER_AddBuildInfo Error : %d", hErr);
	}	

	return ;
}

void cApkUtilBuilder::AddBuildInfo_Cab(const uint32_t& builderHandle, const DbtTuneParamCab_t& tunerParam)
{
	HERROR hErr = ERR_FAIL;
	DxTuneParam_t stInfo;
	cDbusUtilCvt::C2DbtTuneParamCab (tunerParam, &stInfo);
	
	hErr = APK_BUILDER_AddBuildInfo ((HUINT32)builderHandle, &stInfo);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_BUILDER_AddBuildInfo Error : %d", hErr);
	}	

	return ;
}

void cApkUtilBuilder::RemoveBuildInfo(const uint32_t& builderHandle, const DbtBuilder_BuildInfo_t& buildInfo)
{
	HERROR hErr = ERR_FAIL;
	DxBuilder_BuildInfo_t stParam;

	cDbusUtilCvt::C2DbtBuilder_BuildInfo (buildInfo, &stParam);
	hErr = APK_BUILDER_RemoveBuildInfo ((HUINT32)builderHandle, &stParam);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_BUILDER_RemoveBuildInfo Error : %d", hErr);
	}	

	return ;
}

DbtBuilder_Option_t cApkUtilBuilder::GetOption(const uint32_t& builderHandle)
{
	DbtBuilder_Option_t reval;

	HERROR hErr = ERR_FAIL;

	DxBuilder_Option_t stInfo;
	HxSTD_memset (&stInfo, 0, sizeof(DxBuilder_Option_t));

	hErr = APK_BUILDER_GetOption ((HUINT32 )builderHandle, &stInfo);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_BUILDER_GetOption Error : %d", hErr);
	}	

	reval = cDbusUtilCvt::C2DxBuilder_Option (&stInfo);
	return reval;
}

void cApkUtilBuilder::SetOption(const uint32_t& builderHandle, const DbtBuilder_Option_t& buildOption)
{
	HERROR hErr = ERR_FAIL;

	DxBuilder_Option_t stInfo;
	cDbusUtilCvt::C2DbtBuilder_Option (buildOption, &stInfo);
	
	hErr = APK_BUILDER_SetOption ((HUINT32)builderHandle, &stInfo);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_BUILDER_SetOption Error : %d", hErr);
	}		

	return ;
}

::DBus::Struct< uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, ::DBus::Struct< uint32_t > > cApkUtilBuilder::GetBuilderStatus(const uint32_t& builderHandle)
{
	::DBus::Struct< uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, ::DBus::Struct< uint32_t > > reval;

	HERROR hErr = ERR_FAIL;

	HUINT32 ulTotalTuningNum		=	0;
	HUINT32 ulCompleteTuningNum		=	0;
	HUINT32 ulSearchedTuningNum		=	0;
	HUINT32 ulSearchState			=	0;
	HUINT32 ulFrequency				=	0;
	DxDeliveryType_e eDeliveryType	=	eDxDELIVERY_TYPE_ALL;
	DxTuneParam_t stTuningInfo;
	HxSTD_memset (&stTuningInfo, 0, sizeof(DxTuneParam_t));

	hErr =  APK_BUILDER_GetBuilderStatus ((HUINT32)builderHandle, &ulTotalTuningNum, &ulCompleteTuningNum, &ulSearchedTuningNum, &ulSearchState, &ulFrequency, &eDeliveryType, &stTuningInfo);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_BUILDER_GetBuilderStatus Error : %d", hErr);
	}	

	{
		reval._1	=	(uint32_t)ulTotalTuningNum;
		reval._2	=	(uint32_t)ulCompleteTuningNum;
		reval._3	=	(uint32_t)ulSearchedTuningNum;
		reval._4	=	(uint32_t)ulSearchState;
		reval._5	=	(uint32_t)ulFrequency;
		reval._6	=	(uint32_t)eDeliveryType;

		//::DBus::Struct< uint32_t >& tunerParam
	}

	return reval;
}

::DBus::Struct< int32_t, int32_t > cApkUtilBuilder::GetBuilderSignalStatus(const uint32_t& builderHandle)
{
	::DBus::Struct< int32_t, int32_t > reval;

	HERROR hErr = ERR_FAIL;

	HINT32 nSignalStrength	=	0;
	HINT32 nSignalQuality	=	0;
	
	hErr = APK_BUILDER_GetBuilderSignalStatus ((HUINT32)builderHandle, &nSignalStrength, &nSignalQuality);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_BUILDER_GetBuilderSignalStatus Error : %d", hErr);
	}

	{
		reval._1	=	(int32_t)nSignalStrength;
		reval._2	=	(int32_t)nSignalQuality;
	}

	return reval;
}

std::vector< DbtBuilder_SearchedInfo_t > cApkUtilBuilder::GetBuilderSvcInfo(const uint32_t& builderHandle, const int32_t& startIndex, const int32_t& count)
{
	HERROR hErr = ERR_FAIL;
	std::vector< DbtBuilder_SearchedInfo_t > reval;
		
	HINT32 i;
	HINT32 nResultNo = 0;
	DxBuilder_SearchedInfo_t *pstSvcInfoList = NULL;

	hErr = APK_BUILDER_GetBuilderSvcInfo ((HUINT32)builderHandle, (HINT32)startIndex, (HINT32)count, &nResultNo, &pstSvcInfoList);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_BUILDER_GetBuilderSvcInfo Error : %d", hErr);
	}	

	for (i = 0; i < nResultNo; i++)
	{
		reval.push_back (cDbusUtilCvt::C2DxBuilder_SearchedInfo(&pstSvcInfoList[i]));
	}

	hErr = APK_BUILDER_FreeBuilderSvcInfo (pstSvcInfoList);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_BUILDER_GetBuilderSvcInfo Error : %d", hErr);
	}	

	return reval;
}

std::string cApkUtilBuilder::GetScanType(const uint32_t& builderHandle)
{
	std::string reval;

	const HCHAR* pStrScanType = APK_BUILDER_GetScanType((HUINT32)builderHandle);

	if (NULL != pStrScanType)
	{
		reval.append(pStrScanType);
	}

	return reval; 
}

void cApkUtilBuilder::StartScan(const uint32_t& builderHandle)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_BUILDER_StartScan ((HUINT32)builderHandle);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_BUILDER_StartScan Error : %d", hErr);
	}

	return ;
}

void cApkUtilBuilder::StopScan(const uint32_t& builderHandle)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_BUILDER_StopScan ((HUINT32)builderHandle);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_BUILDER_StopScan Error : %d", hErr);
	}

	return ;
}

void cApkUtilBuilder::PauseScan(const uint32_t& builderHandle)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_BUILDER_PauseScan ((HUINT32)builderHandle);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_BUILDER_PauseScan Error : %d", hErr);
	}

	return ;
}

void cApkUtilBuilder::ResumeScan(const uint32_t& builderHandle)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_BUILDER_ResumeScan ((HUINT32)builderHandle);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_BUILDER_ResumeScan Error : %d", hErr);
	}

	return ;
}

void cApkUtilBuilder::SetAdditionalNumbers(const uint32_t& builderHandle)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_BUILDER_SetAdditionalNumbers((HUINT32)builderHandle);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_BUILDER_SetAdditionalNumbers Error : %d", hErr);
	}

	return ;
}

void cApkUtilBuilder::MotorGotoPosition(const uint32_t& builderHandle, const int32_t& position)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_BUILDER_MotorGotoPosition ((HUINT32)builderHandle, (HINT32)position);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_BUILDER_MotorGotoPosition Error : %d", hErr);
	}

	return ;
}

void cApkUtilBuilder::MotorStorePosition(const uint32_t& builderHandle, const int32_t& position)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_BUILDER_MotorStorePosition ((HUINT32)builderHandle, (HINT32)position);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_BUILDER_MotorStorePosition Error : %d", hErr);
	}

	return ;
}

void cApkUtilBuilder::MotorResetPosition(const uint32_t& builderHandle)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_BUILDER_MotorResetPosition ((HUINT32)builderHandle);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_BUILDER_MotorResetPosition Error : %d", hErr);
	}

	return ;
}

void cApkUtilBuilder::MotorRecalcuate(const uint32_t& builderHandle, const int32_t& position)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_BUILDER_MotorRecalculate ((HUINT32)builderHandle, (HINT32)position);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("MotorRecalcuate Error : %d", hErr);
	}

	return ;
}

void cApkUtilBuilder::MotorMove(const uint32_t& builderHandle, const uint32_t& direction, const uint32_t& mode, const int32_t& step)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_BUILDER_MotorMove ((HUINT32)builderHandle, (DxSat_MOTOR_DIRECTION_e)direction, (DxSat_MOTOR_DRIVEMODE_e)direction, (HINT32)step);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("MotorMove Error : %d", hErr);
	}

	return ;
}

void cApkUtilBuilder::MotorStop(const uint32_t& builderHandle)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_BUILDER_MotorStop ((HUINT32)builderHandle);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("MotorStop Error : %d", hErr);
	}

	return ;
}

void cApkUtilBuilder::MotorSetLimit(const uint32_t& builderHandle, const uint32_t& direction)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_BUILDER_MotorSetLimit ((HUINT32)builderHandle, (DxSat_MOTOR_DIRECTION_e)direction);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_BUILDER_MotorSetLimit Error : %d", hErr);
	}

	return ;
}

void cApkUtilBuilder::MotorEnableLimit(const uint32_t& builderHandle)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_BUILDER_MotorEnableLimit ((HUINT32)builderHandle);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_BUILDER_MotorEnableLimit Error : %d", hErr);
	}

	return ;
}

void cApkUtilBuilder::MotorDisableLimit(const uint32_t& builderHandle)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_BUILDER_MotorDisableLimit ((HUINT32)builderHandle);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_BUILDER_MotorDisableLimit Error : %d", hErr);
	}

	return ;
}

void cApkUtilBuilder::MotorGotoDegree(const uint32_t& builderHandle, const int32_t& degree)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_BUILDER_MotorGotoDegree ((HUINT32)builderHandle, (HINT32)degree);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_BUILDER_MotorGotoDegree Error : %d", hErr);
	}

	return ;
}

void cApkUtilBuilder::MotorGotoSatellite(const uint32_t& builderHandle, const int32_t& satellite)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_BUILDER_MotorGotoSatellite ((HUINT32)builderHandle, (HINT32)satellite);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_BUILDER_MotorGotoSatellite Error : %d", hErr);
	}

	return ;
}

void cApkUtilBuilder::MotorSetMyLongitude(const int32_t& newLongitude)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_BUILDER_MotorSetMyLongitude ((HINT32)newLongitude);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_BUILDER_MotorSetMyLongitude Error : %d", hErr);
	}	

	return ;
}

void cApkUtilBuilder::MotorSetMyLatitude(const int32_t& newLatitude)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_BUILDER_MotorSetMyLatitude ((HINT32)newLatitude);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_BUILDER_MotorSetMyLatitude Error : %d", hErr);
	}

	return ;
}

int32_t cApkUtilBuilder::MotorGetMyLongitude()
{
	HERROR hErr = ERR_FAIL;

	HINT32 nMyLongitude = 0;
	hErr = APK_BUILDER_MotorGetMyLongitude (&nMyLongitude);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_BUILDER_MotorGetMyLongitude Error : %d", hErr);
	}

	return (int32_t)nMyLongitude;
}

int32_t cApkUtilBuilder::MotorGetMyLatitude()
{
	HERROR hErr = ERR_FAIL;

	HINT32 nMyLatitude = 0;
	hErr = APK_BUILDER_MotorGetMyLatitude (&nMyLatitude);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_BUILDER_MotorGetMyLatitude Error : %d", hErr);
	}

	return (int32_t)nMyLatitude;
}
