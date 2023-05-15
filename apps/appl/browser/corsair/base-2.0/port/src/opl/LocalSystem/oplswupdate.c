/**************************************************************************************/
/**
 * @file bplupdate.c
 *
 * SW Update Interfaces
 *
 **************************************************************************************
 **/
#include <hlib.h>
#include <apk.h>
#include "oplswupdate.h"
#include <dlib.h>
#include <papi.h>
/******************************************************************
	Define
******************************************************************/
#define	CHECK_BPL_PARAM_PTR(a)		{if (a == NULL) {HxLOG_Error("Input argument is NULL\n"); return BPL_STATUS_ERROR;}}


/******************************************************************
	Typedef
******************************************************************/
typedef void (*pvOplSwupdateEventListener)(DxSwUpdate_EventData_t *psSwUpdateEvent);

/******************************************************************
	Static variables
******************************************************************/
OplSwupdateEvent_t s_pfOplSwUpdatelistener	= NULL;


/******************************************************************
	Static functions
******************************************************************/

STATIC HCHAR* local_oplswupdate_GetStrOfDxSwUpdate_Event_e(DxSwUpdate_Event_e e)
{
#ifdef CONFIG_DEBUG
	switch(e)
	{
	ENUM_TO_STR(eDxSWUPDATE_EVENT_ConnectSuccess);
	ENUM_TO_STR(eDxSWUPDATE_EVENT_ConnectFail);
	ENUM_TO_STR(eDxSWUPDATE_EVENT_DetectSuccess);		// menu ota
	ENUM_TO_STR(eDxSWUPDATE_EVENT_DetectSuccessInLive);	// live ota
	ENUM_TO_STR(eDxSWUPDATE_EVENT_DetectFail);
	ENUM_TO_STR(eDxSWUPDATE_EVENT_DownloadSuccess);
	ENUM_TO_STR(eDxSWUPDATE_EVENT_DownloadFail);
	ENUM_TO_STR(eDxSWUPDATE_EVENT_DownloadProgress);
	ENUM_TO_STR(eDxSWUPDATE_EVENT_InstallSuccess);
	ENUM_TO_STR(eDxSWUPDATE_EVENT_InstallFail);
	ENUM_TO_STR(eDxSWUPDATE_EVENT_InstallProgress);
	ENUM_TO_STR(eDxSWUPDATE_EVENT_Erorr);
	ENUM_TO_STR(eDxSWUPDATE_EVENT_Finish);
	default:
		return "??";
	}
#else
	return "";
#endif
}

STATIC HCHAR* local_oplswupdate_GetStrOfDxSwUpdate_Source_e(DxSwUpdate_Source_e e)
{
#ifdef CONFIG_DEBUG
	switch(e)
	{
	ENUM_TO_STR(eDxSWUPDATE_SOURCE_None);
	ENUM_TO_STR(eDxSWUPDATE_SOURCE_Rf);
	ENUM_TO_STR(eDxSWUPDATE_SOURCE_Ip);
	ENUM_TO_STR(eDxSWUPDATE_SOURCE_File);
	ENUM_TO_STR(eDxSWUPDATE_SOURCE_Usb);
	default:
		return "??";
	}
#else
	return "";
#endif
}

STATIC HCHAR* local_oplswupdate_GetStrOfDxSwUpdate_State_e(DxSwUpdate_State_e e)
{
#ifdef CONFIG_DEBUG
	if(0 == e)		{ return "FAIL"; }
	else if(1 == e)	{ return "SUCCESS"; }
	else if(2 == e)	{ return "PROGRESS"; }
	else			{ return "????"; }
#else
	return "";
#endif
}

static void opl_dl_swupdateEvtListener(DxSwUpdate_EventData_t *pvData)
{
	DxSwUpdate_EventData_t 	*pstEvent = NULL;

	if(pvData == NULL)
	{
		HxLOG_Error("pvData == NULL \n");
		return;
	}

#ifdef CONFIG_DEBUG
	HxLOG_Print("----------------------------------------\n");
	HxLOG_Print("pvData->eEvent      = %s\n", local_oplswupdate_GetStrOfDxSwUpdate_Event_e(pvData->eEvent));
	HxLOG_Print("pvData->eMethodType = %s\n", (eDxSWUPDATE_METHOD_MENU == pvData->eMethodType) ? "MENU":"LIVE");
	HxLOG_Print("pvData->eSignalType = %s\n", (eDxSWUPDATE_SIGNAL_SINGLE == pvData->eSignalType) ? "SINGLE":"MULTI");
	HxLOG_Print("pvData->eSourceType = %s\n", local_oplswupdate_GetStrOfDxSwUpdate_Source_e(pvData->eSourceType));
	HxLOG_Print("pvData->eState      = %s\n", local_oplswupdate_GetStrOfDxSwUpdate_State_e(pvData->eState));

	if(eDxSWUPDATE_EVENT_DetectSuccess == pvData->eEvent)
	{
		HxLOG_Print("\t - szCurrentVer = %s, szFoundVer = %s\n"
			, pvData->unEventData.stDetectEvent.szCurrentVer
			, pvData->unEventData.stDetectEvent.szFoundVer
			);
	}
	if(eDxSWUPDATE_EVENT_DownloadProgress == pvData->eEvent)
	{
		HxLOG_Print("\t - Progress(%d/%d)\n"
			, pvData->unEventData.stProgressEvent.nProgress
			, pvData->unEventData.stProgressEvent.nProgressMax
			);
	}
	HxLOG_Print("\t - ucSwupType(%d), (%d ~ %d)\n"
		, pvData->unExEventData.stExSwupTypeData.ucSwupType
		, pvData->unExEventData.stExSwupTypeData.ulStartTime
		, pvData->unExEventData.stExSwupTypeData.ulEndTime
		);
	HxLOG_Print("----------------------------------------\n");
#endif

	pstEvent = (DxSwUpdate_EventData_t *)HLIB_STD_MemCalloc(sizeof(DxSwUpdate_EventData_t));
	if(pstEvent != NULL)
	{
		if(pvData != NULL)
		{
			HxSTD_MemCopy(pstEvent, pvData, sizeof(DxSwUpdate_EventData_t));
		}
	}
	else
	{
		HxLOG_Error("alloc fail! \n");
		return;
	}

	if(s_pfOplSwUpdatelistener != NULL)
	{
		s_pfOplSwUpdatelistener(pstEvent);
	}

	if(pstEvent)
	{
		HLIB_STD_MemFree(pstEvent);
		pstEvent = NULL;
	}

	return;
}

/******************************************************************
	Interfaces
******************************************************************/

// OTA를 여러 Type 별로 진행이 가능 하도록 하기위해 OPL_Swupdate_New()를 변형 하여 만듦.
OPL_SwupdateHandle OPL_Swupdate_New_withType(int otaType)
{
	HBOOL		 hErr;
	Handle_t	 hSwupdate	= HANDLE_NULL;

	hErr = APK_SWUPDATE_Create (otaType, &hSwupdate, NULL);

	if (ERR_OK != hErr)
	{
		HxLOG_Error("[%s:%04d] APK_DL_SWUPDATE_Create() Error : hErr [0x%x] \n", __FUNCTION__, __LINE__, hErr);
		return NULL;
	}
	if (HANDLE_NULL == hSwupdate)
	{
		HxLOG_Error("[%s:%04d] HANDLE_NULL == hSwupdate \n", __FUNCTION__, __LINE__);
		return NULL;
	}

	if (hSwupdate != HANDLE_NULL)
	{
		return (OPL_SwupdateHandle)hSwupdate;
	}
	else
	{
		return NULL;
	}
}

// 아래 함수는 사용하지 말것, OPL_Swupdate_New_withType을 사용하도록...
OPL_SwupdateHandle OPL_Swupdate_New(void)
{
	HBOOL		 hErr;
	Handle_t	 hSwupdate	= HANDLE_NULL;


	hErr = APK_SWUPDATE_Create (eDxSwUpdate_SPEC_HUMAX, &hSwupdate, NULL);

	if (ERR_OK != hErr)
	{
		HxLOG_Error("[%s:%04d] APK_DL_SWUPDATE_Create() Error : hErr [0x%x] \n", __FUNCTION__, __LINE__, hErr);
		return NULL;
	}
	if (HANDLE_NULL == hSwupdate)
	{
		HxLOG_Error("[%s:%04d] HANDLE_NULL == hSwupdate \n", __FUNCTION__, __LINE__);
		return NULL;
	}

	if (hSwupdate != HANDLE_NULL)
	{
		return (OPL_SwupdateHandle)hSwupdate;
	}
	else
	{
		return NULL;
	}
}

void	OPL_SwupdateInit(OplSwupdateEvent_t listener)
{
	s_pfOplSwUpdatelistener = listener;
	HxLOG_Print("call APK_SWUPDATE_SwupdateInit()\n");
	APK_SWUPDATE_SwupdateInit(opl_dl_swupdateEvtListener);
}

void 	OPL_StartSwupdateDetect(OplSwupdateEvent_t listener, OPL_SwupdateHandle hSwupHandle)
{
	s_pfOplSwUpdatelistener = listener;

	HxLOG_Print("call APK_SWUPDATE_StartDetect()\n");

	APK_SWUPDATE_StartDetect(opl_dl_swupdateEvtListener, (Handle_t)hSwupHandle);
}

void	OPL_StartSwupdateConnect(OplSwupdateEvent_t listener, OPL_SwupdateHandle hSwupHandle)
{
	s_pfOplSwUpdatelistener = listener;

	HxLOG_Print("call //APK_DL_SWUPDATE_StartConnect()\n");
	//APK_DL_SWUPDATE_StartConnect(opl_dl_swupdateEvtListener, (Handle_t)hSwupHandle);
}

void	OPL_StartSwupdateDownload(OplSwupdateEvent_t listener, OPL_SwupdateHandle hSwupHandle)
{
	s_pfOplSwUpdatelistener = listener;

	HxLOG_Print("call APK_SWUPDATE_StartDownload()\n");
	APK_SWUPDATE_StartDownload(opl_dl_swupdateEvtListener, (Handle_t)hSwupHandle);
}

void	OPL_StartSwupdateInstall(OplSwupdateEvent_t listener, OPL_SwupdateHandle hSwupHandle)
{
	s_pfOplSwUpdatelistener = listener;

	HxLOG_Print("call APK_SWUPDATE_StartInstall()\n");
	APK_SWUPDATE_StartInstall(opl_dl_swupdateEvtListener, (Handle_t)hSwupHandle);
}

void	OPL_StopSwupdateUpdate(OPL_SwupdateHandle hSwupHandle)
{
	//s_pfOplSwUpdatelistener = NULL;

	HxLOG_Print("call APK_SWUPDATE_StartDetect()\n");
	APK_SWUPDATE_Cancel((Handle_t)hSwupHandle);
}

void	OPL_StopSwupdateDetect(OPL_SwupdateHandle hSwupHandle)
{
	//s_pfOplSwUpdatelistener = NULL;

	HxLOG_Print("call APK_SWUPDATE_StopDetect()\n");
	APK_SWUPDATE_StopDetect((Handle_t)hSwupHandle);
}

void	OPL_StopSwupdateConnect(OPL_SwupdateHandle hSwupHandle)
{
	//s_pfOplSwUpdatelistener = NULL;

	HxLOG_Print("call APK_SWUPDATE_StartDetect()\n");
	//APK_SWUPDATE_StopConnect((Handle_t)hSwupHandle);
}

void	OPL_StopSwupdateDownload(OPL_SwupdateHandle hSwupHandle)
{
	//s_pfOplSwUpdatelistener = NULL;

	HxLOG_Print("call APK_SWUPDATE_StopDownload()\n");
	APK_SWUPDATE_StopDownload((Handle_t)hSwupHandle);
}

void	OPL_StopSwupdateInstall(OPL_SwupdateHandle hSwupHandle)
{
	//s_pfOplSwUpdatelistener = NULL;

	HxLOG_Print("call APK_SWUPDATE_StopInstall()\n");
	APK_SWUPDATE_StopInstall((Handle_t)hSwupHandle);
}

void	OPL_SystemRestartSwUpdate(void)
{
	APK_SCENARIO_RemoveSWUpdateVersion();
	#ifdef CONFIG_DEBUG
	fflush(NULL);
	{
		HCHAR	szTempVer[64];
		APK_SCENARIO_GetSWUpdateVersion(szTempVer, 64);
		HxLOG_Print("swupdate version(%s) after remove\n", szTempVer);
	}
	fflush(NULL);
	#endif

	HxLOG_Print("call APK_POWER_RequestReboot()\n");
	APK_POWER_RequestReboot();
}

BPLStatus	OPL_StartSwupdateGetVersionBootupHumaxOta( char *pszVersion)
{
	CHECK_BPL_PARAM_PTR(pszVersion);

	APK_SCENARIO_GetSWUpdateVersion(pszVersion, 64);
	HxLOG_Print("call APK_SCENARIO_GetSWUpdateVersion() - pszVersion(%s)\n", pszVersion);

	return BPL_STATUS_OK;
}

/**
 * @todo We have to connect apk API.
 */
BPLStatus	OPL_SwUpdateGetNetworkId(int *networkId)
{
    return BPL_STATUS_OK;
}

/**
 * @todo We have to connect apk API.
 */
BPLStatus	OPL_SwUpdateSetNetworkId(int networkId)
{
    return BPL_STATUS_OK;
}

OPL_SwupdateHandle	OPL_StartSwupdateManualDetectForSat(
							HINT32 	frequency ,		/* Frequency */
							HINT32 	polarization ,	/* Polarization */
							HINT32	symbolrate , 	/* Symbol Rate */
							HINT32	transport , 	/* Transmission */
							HINT32	pskmode,		/* psk Mode */
							HINT32	fecrate 		/* FEC Code Rate */
							)
{
	HBOOL		 hErr;
	Handle_t	 hSwupdate	= HANDLE_NULL;
	DxTransponder_t	trans;

	HxLOG_Print("call APK_SWUPDATE_CreateManualConnect() - frequency(%d)\n", frequency);

#if !defined( CONFIG_OP_JAPAN )	// TODO: sbkim, to be delete util.. search developed
	trans.deliType			=	eDxDELIVERY_TYPE_SAT;
#endif
	trans.tuningParam.sat.tuningInfo.ulFrequency	=	frequency;
	trans.tuningParam.sat.tuningInfo.ulSymbolRate	=	symbolrate;
	trans.tuningParam.sat.tuningInfo.eTransSpec		=	transport;
	trans.tuningParam.sat.tuningInfo.ePskMod 		=	pskmode;
	trans.tuningParam.sat.tuningInfo.ePolarization	=	polarization;
	trans.tuningParam.sat.tuningInfo.eFecCodeRate	=	fecrate;

#if defined(CONFIG_OP_SES)
	hErr = APK_SWUPDATE_CreateManualConnect (eDxSwUpdate_SPEC_SES, &hSwupdate, &trans);
#else
	hErr = APK_SWUPDATE_CreateManualConnect (eDxSwUpdate_SPEC_HUMAX, &hSwupdate, &trans);
#endif

	if (ERR_OK != hErr)
	{
		HxLOG_Error("[%s:%04d] APK_DL_SWUPDATE_Create() Error : hErr [0x%x] \n", __FUNCTION__, __LINE__, hErr);
		return NULL;
	}
	if (HANDLE_NULL == hSwupdate)
	{
		HxLOG_Error("[%s:%04d] HANDLE_NULL == hSwupdate \n", __FUNCTION__, __LINE__);
		return NULL;
	}

	return (hSwupdate != HANDLE_NULL) ? (OPL_SwupdateHandle)hSwupdate : NULL;
}

OPL_SwupdateHandle	OPL_StartSwupdateManualDetectForCab(
							HINT32 	frequency ,		/* Frequency */
							HINT32 	constellation ,	/* Constellation */
							HINT32	symbolrate  	 /* Symbol Rate */
							)
{
	HBOOL		 hErr;
	Handle_t	 hSwupdate	= HANDLE_NULL;
	DxTransponder_t	trans;

	HxLOG_Print("call APK_SWUPDATE_CreateManualConnect() - frequency(%d)\n", frequency);

#if !defined( CONFIG_OP_JAPAN )	// TODO: sbkim, to be delete util.. search developed
	trans.deliType			=	eDxDELIVERY_TYPE_CAB;
#endif
	trans.tuningParam.cab.ulFrequency	=	frequency;
	trans.tuningParam.cab.constellation	=	constellation;
	trans.tuningParam.cab.ulSymbolRate	=	symbolrate;
	trans.tuningParam.cab.spectrum		=	eDxCAB_SPECTRUM_AUTO;

#if defined(CONFIG_OP_SES)
	hErr = APK_SWUPDATE_CreateManualConnect (eDxSwUpdate_SPEC_SES, &hSwupdate, &trans);
#elif defined(CONFIG_OP_NORDIG)
	hErr = APK_SWUPDATE_CreateManualConnect (eDxSwUpdate_SPEC_DVBSSU, &hSwupdate, &trans);
#else
	hErr = APK_SWUPDATE_CreateManualConnect (eDxSwUpdate_SPEC_HUMAX, &hSwupdate, &trans);
#endif

	if (ERR_OK != hErr)
	{
		HxLOG_Error("[%s:%04d] APK_DL_SWUPDATE_Create() Error : hErr [0x%x] \n", __FUNCTION__, __LINE__, hErr);
		return NULL;
	}
	if (HANDLE_NULL == hSwupdate)
	{
		HxLOG_Error("[%s:%04d] HANDLE_NULL == hSwupdate \n", __FUNCTION__, __LINE__);
		return NULL;
	}

	return (hSwupdate != HANDLE_NULL) ? (OPL_SwupdateHandle)hSwupdate : NULL;
}

