/**************************************************************************************/
/**
 * @file bpllocalsystem.c
 *
 * System information
 *
 **************************************************************************************
 **/
#include "bpldb.h"
#include "opllocalsystem.h"

#include "oapi.h"
#include "hapi.h"
#ifdef	CONFIG_OP_FREESAT
#include <appkit_freesat.h>
#endif
#include <apk.h>
#include <stdlib.h>
#include <string.h>
#include <hlib.h>
#include <dlib.h>

#include <dlib_cas.h>
#include <uapi.h>
#include <papi.h>
#ifdef	CONFIG_OP_SES
#include <rr.h>
#endif

/******************************************************************
	Define
******************************************************************/
#define	CHECK_BPL_PARAM_PTR(a)		{if (a == NULL) {HxLOG_Error("Input argument is NULL\n"); return BPL_STATUS_ERROR;}}
#define	HCRPATH		"/tmp/hcrplain"
#define	MAXHCRDATALEN 	(1024 * 1024) // 1Mbyte

#define FVC_URL_FILE			"/url/url_fvc.txt"

/******************************************************************
	Static variables
******************************************************************/
static OplLoacalSystemCamStateChange_t  CamStatusChangeListener = NULL;
static OplLocalSystemPowerStateChange_t	s_pfnPowerStateChangeListener = NULL;
static OplLocalSystemPowerStateChangeError_t	s_pfnPowerStateChangeErrorListener = NULL;
static OplLoacalSystemUserActionIdle_t  UserActionIdleListener = NULL;
static OplLoacalSystemKeyboardResult_t  KeyboardResultListener = NULL;

static OplLocalSystemIrdetoInfoChange_t  IrdetoInfoChangeListener = NULL;
static OplLocalSystemIrdetoImportExport_t s_ImportExportListener = NULL;
static OplLocalSystemNagraInfoChange_t  s_NagraInfoChangeListener = NULL;
static OplLocalSystemNagraSCState_t  s_NagraSCStateListener = NULL;
static OplLoacalSystemExtDevInfo_t	s_pfnExtDevInfoListener = NULL;
static OplLocalSystemLcnUpdate_t s_LcnUpdateListener = NULL;
static OplLocalSystemColdBootDone_t s_ColdBootDoneListener = NULL;
static OplLocalSystemCasActionIrEmmForcedOta_t s_CasActionIrEmmForcedOtaListener = NULL;
static OplLocalClockRecovery_t	s_ClockRecoveryListener = NULL;
static OplLocalSystemTrdConflict_t s_TrdConflictListener = NULL;
static OplLocalSystemNcdInfo_t s_NcdInfoListener = NULL;

static OplLocalSystemThermalProtection_t        s_ThermalProtectionOccuredListener = NULL;

static OPL_BOOL		s_bIsRegisterBoxInfoListener = OPL_FALSE;

static HUINT8 		*s_HCRbuffer = NULL;

/* OIPF Power Control */
static int g_nOipfPreviousPowerState = -1;
static int g_nOipfPowerState = -1;
static int g_nOipfTimePowerState = -1;

/*  Corsair Debug  */
static int g_nOipfKeyGrab = -1;

/******************************************************************
	Static functions
******************************************************************/
HCHAR*		OPL_UTIL_GetStringFromValue(EnumStringPair_t *pstPairList, const HUINT32 ulValue)
{
	while (pstPairList->pszValue)
	{
		if (pstPairList->ulValue == ulValue)
			return pstPairList->pszValue;

		pstPairList++;
	}

	return "Unknown";
}

HUINT32 	OPL_UTIL_GetValueFromString(EnumStringPair_t *pstPairList, const HCHAR *pszData)
{
	while (pstPairList->pszValue)
	{
		if (HxSTD_StrCmp(pstPairList->pszValue, pszData) == 0)
			return pstPairList->ulValue;

		pstPairList++;
	}

	return pstPairList->ulValue;
}


static void opl_localsystem_CamStateChangeListener(HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3);

static	void	local_sqcBoxInfo_Notifier(HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3)
{
	int	nOctoLastPowerMode;

	nOctoLastPowerMode = (ulParam1==eAPK_POWER_BOXMODE_STANDBY) ? 2 : 1; /*2: Standby, 1: Operation */

	HxLOG_Warning("System Notifier.... - systemMode : [%s]\n", (nOctoLastPowerMode == 1 ? "normal" : "shutdown"));
	HxLOG_Print("[%s:%d] s_pfnPowerStateChangeListener[0x%p] g_nOipfPowerState[%d] nOctoLastPowerMode[%d]\n",
	             __FUNCTION__,
	             __LINE__,
	             s_pfnPowerStateChangeListener,
	             g_nOipfPowerState,
	             nOctoLastPowerMode);

	if ((s_pfnPowerStateChangeListener) && (g_nOipfPowerState != nOctoLastPowerMode))
	{
		OPL_LocalSystem_SetOipfPowerState(nOctoLastPowerMode);
		s_pfnPowerStateChangeListener(nOctoLastPowerMode);
	}
}


static void	opl_localsystem_registerBoxInfoListener(void)
{
	if (s_bIsRegisterBoxInfoListener == OPL_FALSE)
	{
		//	Register BoxInfo Listener
		APK_POWER_RegisterNotifier(local_sqcBoxInfo_Notifier);
		s_bIsRegisterBoxInfoListener = OPL_TRUE;
	}
}

static void	opl_ImportExport_Db_EvtListener(HUINT32 type	/* 1:import, 2: export*/,
									  								HUINT32 result	/* Error Type */)
{
	if (s_ImportExportListener != NULL)
	{
		s_ImportExportListener(type, result);
	}
}

static void	opl_LcnUpdate_EvtListener(void *pstLcnInform)
{
	if (s_LcnUpdateListener != NULL)
	{
		s_LcnUpdateListener(pstLcnInform);
	}
}

static void	opl_ColdBootDone_EvtListener(void)
{
	if (s_ColdBootDoneListener != NULL)
	{
		s_ColdBootDoneListener();
	}
}

static void	opl_CasActionIrEmmForcedOta_EvtListener(HUINT32 param1, HUINT32 param2, HUINT32 param3)
{
	if (s_CasActionIrEmmForcedOtaListener != NULL)
	{
		s_CasActionIrEmmForcedOtaListener(param1, param2, param3);
	}
}

static void	opl_TrdConflict_EvtListener(DxTrdConflictData_t *pstTrdConflict)
{
	if (s_TrdConflictListener != NULL)
	{
		s_TrdConflictListener(pstTrdConflict);
	}
}

static void	opl_NcdInfo_EvtListener(HCHAR *pstNcdInfo)
{
	if (s_NcdInfoListener != NULL)
	{
		s_NcdInfoListener(pstNcdInfo);
	}
}

/******************************************************************
	Interfaces
******************************************************************/
BPLStatus OPL_LocalSystem_GetDeviceID( char *pszDevID)
{
	CHECK_BPL_PARAM_PTR(pszDevID);

	APK_SYSINFO_STB_GetDeviceId(pszDevID, 32);

	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_GetSystemReady( BPL_BOOL *pbReady)
{
	*pbReady = TRUE;

	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_GetModelName( char *pszName)
{
	CHECK_BPL_PARAM_PTR(pszName);

	APK_SYSINFO_STB_GetModelName(pszName, 32);

	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_GetUpdatedList( char *pszList)
{
	CHECK_BPL_PARAM_PTR(pszList);

	APK_SYSINFO_STB_GetUpdatedList(pszList, 1024);

	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_GetChipId( char *pszID)
{
	CHECK_BPL_PARAM_PTR(pszID);

	APK_SYSINFO_STB_GetChipId(pszID, 32);

	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_GetUpdateDate( char *pszName)
{
	CHECK_BPL_PARAM_PTR(pszName);

	APK_SYSINFO_STB_GetUpdateDate(pszName, 32);

	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_GetVendorName( char *pszName)
{
	CHECK_BPL_PARAM_PTR(pszName);

	APK_SYSINFO_STB_GetManufacturerName(pszName, 32);

	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_GetSoftwareVersion( char *pszVersion)
{
	CHECK_BPL_PARAM_PTR(pszVersion);
#if defined(CONFIG_PROD_ROCKYPOM) || defined(CONFIG_PROD_ROCKYWPOM)
	sprintf(pszVersion, "DEMO 0.00.02");
#else
	APK_SYSINFO_STB_GetSoftwareVersion(pszVersion, 32);
#endif
	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_GetPackageVersion( char *pszVersion)
{
	CHECK_BPL_PARAM_PTR(pszVersion);
	APK_SYSINFO_STB_GetPackageVersion(pszVersion, 32);
	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_GetHardwareVersion( char *pszVersion)
{
	CHECK_BPL_PARAM_PTR(pszVersion);

	strncpy(pszVersion, "1.0.0", strlen("1.0.0")+1);

	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_GetSerialNumber( char *pszNumber)
{
	CHECK_BPL_PARAM_PTR(pszNumber);

	APK_SYSINFO_STB_GetSerialNumber(pszNumber, 32);

	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_GetPvrEnabled( BPL_BOOL *pbEnabled)
{
	HERROR	hErr;
	HUINT32	ulDevNum = 0;

	CHECK_BPL_PARAM_PTR(pbEnabled);

	hErr = APK_FILEMGR_STORAGE_GetList(eDxSTORAGE_FILTER_PVR, NULL, &ulDevNum);

	if (hErr == ERR_OK && ulDevNum > 0)
		*pbEnabled = TRUE;
	else
		*pbEnabled = FALSE;

	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_GetInternalHddEnabled(BPL_BOOL *pbEnabled)
{
	HERROR hErr = ERR_OK;
	HUINT32	ulDevNum = 0;

	CHECK_BPL_PARAM_PTR(pbEnabled);

	hErr = APK_FILEMGR_STORAGE_GetList(eDxSTORAGE_FILTER_Internal, NULL, &ulDevNum);

	if (hErr == ERR_OK && ulDevNum > 0)
		*pbEnabled = TRUE;
	else
		*pbEnabled = FALSE;

	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_GetVolume( unsigned int *pnVolume)
{
	CHECK_BPL_PARAM_PTR(pnVolume);

	APK_OUTPUT_AUDIO_GetVolume(pnVolume);

	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_SetVolume( unsigned int nVolume)
{
	APK_OUTPUT_AUDIO_SetVolume(nVolume);

	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_GetMute( BPL_BOOL *pbMute)
{
	CHECK_BPL_PARAM_PTR(pbMute);

	APK_OUTPUT_AUDIO_GetMasterMute((HBOOL*)pbMute);

	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_SetMute( BPL_BOOL bMute)
{
	APK_OUTPUT_AUDIO_SetMasterMute(bMute);

	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_GetMicrophoneVolume( unsigned int *pnVolume)
{
	CHECK_BPL_PARAM_PTR(pnVolume);

	APK_EXTDEV_MIC_GetVolume((HINT32 *)pnVolume);

	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_SetMicrophoneVolume( unsigned int nVolume)
{
	APK_EXTDEV_MIC_SetVolume(nVolume);

	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_GetMicrophoneEnabled(BPL_BOOL *pbEnable)
{
	CHECK_BPL_PARAM_PTR(pbEnable);

	APK_EXTDEV_MIC_GetStatus((HBOOL *)pbEnable);

	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_SetMicrophoneEnabled(BPL_BOOL bEnable)
{
	if (bEnable == TRUE)
	{
		APK_EXTDEV_MIC_Start();
	}
	else
	{
		APK_EXTDEV_MIC_Stop();
	}

	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_GetConnectedMicrophone(unsigned int *pnCnt)
{
	CHECK_BPL_PARAM_PTR(pnCnt);

	APK_EXTDEV_MIC_GetCount((HUINT32 *)pnCnt);

	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_GetTVStandard( eOplTVStandard *eStandard)
{
	DxCompositeStandard_e	eVideoStandard;

	CHECK_BPL_PARAM_PTR(eStandard);

	APK_OUTPUT_VIDEO_GetCompositeStandard(&eVideoStandard);
	switch (eVideoStandard)
	{
		case eDxCOMPOSITE_STANDARD_UNKNOWN:
			*eStandard = OPLLOCALSYSTEM_TV_STANDARD_UNKNOWN;
			break;
		case eDxCOMPOSITE_STANDARD_NTSC:
			*eStandard = OPLLOCALSYSTEM_TV_STANDARD_NTSC;
			break;
		case eDxCOMPOSITE_STANDARD_PAL:
			*eStandard = OPLLOCALSYSTEM_TV_STANDARD_PAL;
			break;
		case eDxCOMPOSITE_STANDARD_SECAM:
			*eStandard = OPLLOCALSYSTEM_TV_STANDARD_SECAM;
			break;
		default:
			break;
	}

	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_GetSystemID( char *pszID)
{
	CHECK_BPL_PARAM_PTR(pszID);

	APK_SYSINFO_STB_GetSystemId(pszID, 32);

	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_GetScreenLanguage( char *pszLanguage)
{
	CHECK_BPL_PARAM_PTR(pszLanguage);

	APK_SCENARIO_GetMainLanguage(pszLanguage);

	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_SetScreenLanguage( const char *pszLanguage)
{
	APK_SCENARIO_SetMainLanguage(pszLanguage);

	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_GetPreferredAudioLanuage( char *pszLanguage, int nIndex)
{
	CHECK_BPL_PARAM_PTR(pszLanguage);

	APK_SCENARIO_GetAudioLanguage(nIndex, pszLanguage);

	pszLanguage[3] = '\0';


	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_SetPreferredAudioLanuage( const char *pszLanguage, int nIndex)
{
	CHECK_BPL_PARAM_PTR(pszLanguage);

	if (APK_SCENARIO_SetAudioLanguage(nIndex, pszLanguage) != ERR_OK)
	{
		/* APK_SCK_SCENARIO_SetAudioLanguage´Â octo¿¡ ¹Ý¿µÇÏ±â À§ÇÑ ÄÚµåÀÓ.
		DB¿¡´Â ¹Ý¿µÀÌ µÇ¾îÀÖ°í, OCTO¿¡ ¹Ý¿µ¸¸ FailÀÏ °æ¿ì Error Ã³¸®¸¦ ¾î¶»°Ô ÇÒ °ÍÀÎÁö °í¹ÎÇØ¾ß ÇÔ.
		DB¿¡ Àû¿ëµÈ °ÍÀ» ±âÁØÀ¸·Î ¿ì¼± »ý°¢ÇÏ¿©, APK_SCK_SCENARIO_SetAudioLanguage°¡ Error¸¦ ReturnÇÏ´õ¶óµµ
		Debug Msg¸¸À» Ãâ·ÂÇÏ°í Return Error¸¦ ÇÏÁö´Â ¾Ê´Â´Ù. */
		HxLOG_Error("Setting the preferred audio language is failed..\n");
	}

	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_GetPreferredSubtitleLanuage( char *pszLanguage, int nIndex)
{
	CHECK_BPL_PARAM_PTR(pszLanguage);

	APK_SCENARIO_GetSubtitleLanguage(nIndex, pszLanguage);

	pszLanguage[3] = '\0';

	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_SetPreferredSubtitleLanuage( const char *pszLanguage, int nIndex)
{
	CHECK_BPL_PARAM_PTR(pszLanguage);

	if (APK_SCENARIO_SetSubtitleLanguage(nIndex, pszLanguage) != ERR_OK)
	{
		/* APK_SCK_SCENARIO_SetAudioLanguage´Â octo¿¡ ¹Ý¿µÇÏ±â À§ÇÑ ÄÚµåÀÓ.
		DB¿¡´Â ¹Ý¿µÀÌ µÇ¾îÀÖ°í, OCTO¿¡ ¹Ý¿µ¸¸ FailÀÏ °æ¿ì Error Ã³¸®¸¦ ¾î¶»°Ô ÇÒ °ÍÀÎÁö °í¹ÎÇØ¾ß ÇÔ.
		DB¿¡ Àû¿ëµÈ °ÍÀ» ±âÁØÀ¸·Î ¿ì¼± »ý°¢ÇÏ¿©, APK_SCK_SCENARIO_SetAudioLanguage°¡ Error¸¦ ReturnÇÏ´õ¶óµµ
		Debug Msg¸¸À» Ãâ·ÂÇÏ°í Return Error¸¦ ÇÏÁö´Â ¾Ê´Â´Ù. */
		HxLOG_Error("Setting the preferred subtitle language is failed..\n");
	}

	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_GetPreferredMenuLanuage( char *pszLanguage)
{
	CHECK_BPL_PARAM_PTR(pszLanguage);

	APK_SCENARIO_GetMainLanguage(pszLanguage);
	pszLanguage[3] = '\0';

	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_SetPreferredMenuLanuage( const char *pszLanguage)
{
	CHECK_BPL_PARAM_PTR(pszLanguage);

	if (APK_SCENARIO_SetMainLanguage(pszLanguage) != ERR_OK)
	{
		/* APK_SCK_SCENARIO_SetAudioLanguage´Â octo¿¡ ¹Ý¿µÇÏ±â À§ÇÑ ÄÚµåÀÓ.
		DB¿¡´Â ¹Ý¿µÀÌ µÇ¾îÀÖ°í, OCTO¿¡ ¹Ý¿µ¸¸ FailÀÏ °æ¿ì Error Ã³¸®¸¦ ¾î¶»°Ô ÇÒ °ÍÀÎÁö °í¹ÎÇØ¾ß ÇÔ.
		DB¿¡ Àû¿ëµÈ °ÍÀ» ±âÁØÀ¸·Î ¿ì¼± »ý°¢ÇÏ¿©, APK_SCK_SCENARIO_SetAudioLanguage°¡ Error¸¦ ReturnÇÏ´õ¶óµµ
		Debug Msg¸¸À» Ãâ·ÂÇÏ°í Return Error¸¦ ÇÏÁö´Â ¾Ê´Â´Ù. */
		HxLOG_Error("Setting the preferred menu language is failed..\n");
	}

//	if (APK_SQCSCENARIO_SetAudioLanguage(pszLanguage, 0) != ERR_OK)
//		HxLOG_Error("Setting the second audio language is failed..\n");

//	if (APK_SQCSCENARIO_SetSubtitleLanguage(pszLanguage, 0) != ERR_OK)
//		HxLOG_Error("Setting the second subtitle language is failed..\n");

	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_GetCountryID( char *pszID, unsigned int nLen)
{
	HxCountry_e	eCountryCode;
	const HCHAR *ptemp;

	// Just Read from sqlite DataBase.
	APK_SCENARIO_GetCountryCode(&eCountryCode);

	ptemp = HLIB_COUNTRY_IdTo3166_Alpha3((const HxCountry_e)eCountryCode);

	if (NULL != ptemp)
	{
		HxSTD_StrNCpy(pszID,ptemp, (nLen-1));
	}
	else
	{
		HxSTD_StrNCpy(pszID,"000", (nLen-1));

	}
	pszID[nLen-1] = '\0';

	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_SetCountryID( const char *pszID)
{
	HxCountry_e eHxCountryId;

	eHxCountryId = HLIB_COUNTRY_3166_Alpha3ToId((const HCHAR *)pszID);

	APK_SCENARIO_SetCountryCode(eHxCountryId);
	HAPI_BroadcastSignal("signal:onCountryIdChanged", 1, "i", eHxCountryId);
	return BPL_STATUS_NOT_SUPPORTED;
}

BPLStatus OPL_LocalSystem_GetOperatorID( int *pszID)
{
	DxOperator_e	eOpType = eDxOPERATOR_None;

	APK_SCENARIO_GetOperatorType(&eOpType);

	*pszID = eOpType;

	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_SetOperatorID( int nID)
{
	if(nID >= eDxOPERATOR_Max)
	{
		nID = eDxOPERATOR_NOT_DEFINED;
	}

	APK_SCENARIO_SetOperatorType(nID);

	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_GetRegionID( int *pnRegionId)
{
	int regionId;
//	HERROR hErr;

	if(HAPI_GetRegistryInt(DxDB_APPL_TIME_REGION_ID, &regionId) == ERR_FAIL)
	{
#ifdef CONFIG_OP_AUS_DTT
		HAPI_SetRegistryInt(DxDB_APPL_TIME_REGION_ID, 2); //AUS Region default value
#else
		HAPI_SetRegistryInt(DxDB_APPL_TIME_REGION_ID, 0); //Others default value
#endif
	}

	HAPI_GetRegistryInt(DxDB_APPL_TIME_REGION_ID, &regionId);

	*pnRegionId = regionId;

	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_SetRegionID( int nRegionId)
{

	HAPI_SetRegistryInt(DxDB_APPL_TIME_REGION_ID, nRegionId);

	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_SetApplicationReady(void)
{
	HAPI_BroadcastSignal("signal:onMainApplicationReady", 1, "i", 0);

#if defined(CONFIG_OP_TDC_NETWORK_LED)
	HERROR hErr = ERR_FAIL;

	APK_NETWORK_LED_Init();

	VK_TASK_Sleep(1000);

	hErr = APK_NETWORK_CONF_CheckInternetConnection(CHECK_INTERNET_MAX_TIME);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("APK_NETWORK_CONF_CheckInternetConnection is failed.\n");
	}
#endif

	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_GetAVOutputsCount( int *pnCount)
{
#if 0
	CHECK_BPL_PARAM_PTR(pnCount);

	if (APK_SQCOUTPUT_GetNumOfDevice((HUINT32*)pnCount) != ERR_OK)
	{
		HxLOG_Error("Getting the number of AV Outputs is failed.\n");
		return BPL_STATUS_ERROR;
	}
#else
//	pnCount = 1;
	*pnCount = 1;
#endif

	return BPL_STATUS_OK;
}

OplAVOutput_t *OPL_LocalSystem_GetAVOutputByIndex( int nIndex)
{
	return NULL;
}

void OPL_LocalSystem_FreeAVOutput( OplAVOutput_t *ptOutput)
{
	HLIB_STD_MemFree(ptOutput);
	ptOutput = NULL;
}

BPLStatus OPL_LocalSystem_GetNetInterfacesCount( int *pnCount)
{
	CHECK_BPL_PARAM_PTR(pnCount);

	if (APK_NETWORK_CONF_GetNumOfNetDev(pnCount) != ERR_OK)
	{
		HxLOG_Error("Getting the number of Network Interfaces is failed.\n");
		return BPL_STATUS_ERROR;
	}

	return BPL_STATUS_OK;
}


BPLStatus OPL_LocalSystem_GetDefaultNetInterface(	int *pnIndex)
{
	CHECK_BPL_PARAM_PTR(pnIndex);

	if (APK_NETWORK_CONF_GetDefaultDev(pnIndex) != ERR_OK)
	{
		HxLOG_Error("Getting the default of Network Interfaces is failed.\n");
		return BPL_STATUS_ERROR;
	}

	return BPL_STATUS_OK;
}


OplNetworkInterface_t *OPL_LocalSystem_GetNetInterfaceByIndex( int nIndex)
{
	HERROR eRet = ERR_OK;
	OplNetworkInterface_t *ptNetif = NULL;
	APKS_NETIFADDR_t	ptNetAddr;
	APKS_NETDEVINFO_t	ptDevInfo;

	ptNetif = HLIB_STD_MemAlloc(sizeof(*ptNetif));
	if ( ptNetif == NULL )
		return NULL;

	if (APK_NETWORK_CONF_GetAddrInfo(nIndex, (APKS_NETIFADDR_t*)&ptNetAddr) != ERR_OK)
	{
		HxLOG_Error("Getting the Address Information of Network is failed.\n");
		HLIB_STD_MemFree(ptNetif);
		return NULL;
	}

	if (APK_NETWORK_CONF_AddrN2A(ptNetAddr.ulIpAddr, ptNetif->szIpAddr, OPLNET_MAX_IPADDR_LEN) != ERR_OK)
	{
		HxLOG_Error("Getting the IP Address is failed.\n");
		HLIB_STD_MemFree(ptNetif);
		return NULL;
	}

	eRet = APK_NETWORK_CONF_AddrN2A(ptNetAddr.ulNetmask, ptNetif->szNetmask, OPLNET_MAX_IPADDR_LEN);
	if ( eRet != ERR_OK )
	{
		HxLOG_Error("Getting the Netmask is failed.\n");
		HLIB_STD_MemFree(ptNetif);
		return NULL;
	}

	eRet = APK_NETWORK_CONF_AddrN2A(ptNetAddr.ulGateway, ptNetif->szGateway, OPLNET_MAX_IPADDR_LEN);
	if ( eRet != ERR_OK )
	{
		HxLOG_Error("Getting the Gateway is failed.\n");
		HLIB_STD_MemFree(ptNetif);
		return NULL;
	}

	eRet = APK_NETWORK_CONF_AddrN2A(ptNetAddr.ulDns1, ptNetif->szDns1, OPLNET_MAX_IPADDR_LEN);
	if ( eRet != ERR_OK )
	{
		HxLOG_Error("Getting the DNS1 is failed.\n");
		HLIB_STD_MemFree(ptNetif);
		return NULL;
	}

	eRet = APK_NETWORK_CONF_AddrN2A(ptNetAddr.ulDns2, ptNetif->szDns2, OPLNET_MAX_IPADDR_LEN);
	if ( eRet != ERR_OK )
	{
		HxLOG_Error("Getting the DNS2 is failed.\n");
		HLIB_STD_MemFree(ptNetif);
		return NULL;
	}

	eRet = APK_NETWORK_CONF_GetMacAddr(nIndex, ptNetif->szMacAddress, OPLNET_MAX_MACADDR_LEN);
	if ( eRet != ERR_OK )
	{
		HxLOG_Error("Getting the Mac Address is failed.\n");
		HLIB_STD_MemFree(ptNetif);
		return NULL;
	}

	eRet = APK_NETWORK_CONF_GetDevInfo(nIndex, &ptDevInfo);
	if ( eRet != ERR_OK )
	{
		HxLOG_Error("Getting the Information of Network Device is failed.\n");
		HLIB_STD_MemFree(ptNetif);
		return NULL;
	}

	ptNetif->bEnabled = ptDevInfo.bNetActive;
	ptNetif->bConnected = ptDevInfo.bConnected;

	switch ( ptDevInfo.eType )
	{
		case eINTERFACE_TYPE_LAN :
			ptNetif->eNetType = OPLNET_TYPE_LAN;
			break;
		case eINTERFACE_TYPE_WLAN :
			ptNetif->eNetType = OPLNET_TYPE_WLAN;
			break;
		case eINTERFACE_TYPE_PPPOE :
			ptNetif->eNetType = OPLNET_TYPE_PPPOE;
			break;
		default:
			ptNetif->eNetType = OPLNET_TYPE_NONE;
			break;
	}

	return ptNetif;
}

void OPL_LocalSystem_FreeNetInterface( OplNetworkInterface_t *ptNetif)
{
	HLIB_STD_MemFree(ptNetif);
	ptNetif = NULL;
}

BPLStatus OPL_LocalSystem_LoaderVersion( char *pszLoaderVer)
{
	CHECK_BPL_PARAM_PTR(pszLoaderVer);

	if (APK_SYSINFO_STB_GetLoaderVersion(pszLoaderVer, 32) != ERR_OK)
	{
		HxLOG_Error("Getting the device ID is failed.\n");
		return BPL_STATUS_ERROR;
	}

	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_UpgraderVersion( char *pszUpgraderVer)
{
	CHECK_BPL_PARAM_PTR(pszUpgraderVer);

	if (APK_SYSINFO_STB_GetUpgraderVersion(pszUpgraderVer, 32) != ERR_OK)
	{
		HxLOG_Error("Getting the device ID is failed.\n");
		return BPL_STATUS_ERROR;
	}

	return BPL_STATUS_OK;
}


BPLStatus OPL_LocalSystem_UpdateDate( char *pszUpdateDate)
{
	CHECK_BPL_PARAM_PTR(pszUpdateDate);

	if (APK_SYSINFO_STB_GetUpdateDate(pszUpdateDate, 32) != ERR_OK)
	{
		HxLOG_Error("Getting the device ID is failed.\n");
		return BPL_STATUS_ERROR;
	}

	return BPL_STATUS_OK;
}
BPLStatus OPL_LocalSystem_GetMicomVersion( char *pszMicomVersion)
{
	CHECK_BPL_PARAM_PTR(pszMicomVersion);

	if (APK_SYSINFO_STB_GetMicomVersion(pszMicomVersion, 32) != ERR_OK)
	{
		HxLOG_Error("Getting the device ID is failed.\n");
		return BPL_STATUS_ERROR;
	}

	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_GetRemoteCustomCode(int *pnCustomCode)
{
	HUINT32		ulCustomCode;
	CHECK_BPL_PARAM_PTR(pnCustomCode);

	APK_SYSINFO_STB_GetCustomCode(&ulCustomCode);
	*pnCustomCode = ulCustomCode;

	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_GetWakeupReason( char *pszWakeupReason )
{
	DxWakeUpReason_e	eWakeupReason;

	CHECK_BPL_PARAM_PTR(pszWakeupReason);

	if (APK_POWER_GetWakeupReason(&eWakeupReason) != ERR_OK)
	{
		return BPL_STATUS_ERROR;
	}

	switch (eWakeupReason)
	{
		case eDxWAKEUP_NONE:
			// compile warningÀ¸·Î ÀÎÇØ Ãß°¡µÊ.
			break;
		case eDxWAKEUP_KEY:
			HxSTD_StrNCpy(pszWakeupReason, "BY_KEY", 32);
			break;
		case eDxWAKEUP_TIMER:
			HxSTD_StrNCpy(pszWakeupReason, "BY_TIMER", 32);
			break;
		case eDxWAKEUP_ACPOWER:
			HxSTD_StrNCpy(pszWakeupReason, "BY_ACPOWER", 32);
			break;
		case eDxWAKEUP_HDMICEC:
			HxSTD_StrNCpy(pszWakeupReason, "BY_HDMICEC", 32);
			break;
		case eDxWAKEUP_REBOOT:
			HxSTD_StrNCpy(pszWakeupReason, "BY_REBOOT", 32);
			break;
	}

	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_DoFctoryDefault( const char *Default_Values)
{
	fflush(NULL);
	HxLOG_Print("\n\n\n\n====\n[%s:%d] We have to go Factory Default Process!!!!!!!!!!!!!!!\n====\n\n\n\n",__FUNCTION__,__LINE__);
	HxLOG_Print("----> defaultValues : [%s]\n\n\n\n", Default_Values);
	fflush(NULL);

	//	OPL´Ü¿¡¼­´Â WEB APPLÀÌ ¸¸µå´Â ºÎºÐ¿¡ ´ëÇÑ °ü¸®¸¸ ÇÏÀÚ.
	HLIB_DIR_Delete((const HCHAR *)"/var/lib/humaxtv/browser");
	HLIB_DIR_Delete((const HCHAR *)"/var/lib/humaxtv/drm");
	HLIB_DIR_Delete((const HCHAR *)"/var/lib/humaxtv_backup/browser");
	HLIB_DIR_Delete((const HCHAR *)"/var/lib/humaxtv_backup/drm");
#if defined(CONFIG_PROD_DEVICE_STORAGE_HDD)
	HLIB_DIR_Delete((const HCHAR *)"/mnt/hd4/widgets");
#endif

	// remove APK DB hash (removed from memory, to prevent writing after delete DB file)
	APK_DB_Remove(NULL);

	if (Default_Values == NULL)
	{
		//	Lower Version of Plugin...
		//	do default with all mode.
		APK_EXE_StartFactoryDefault(eDxFAC_DEF_TYPE_ALL);
	} else
	{
		if (strcmp(Default_Values, "none") == 0)
		{
			//	Nothing to do...
			//	APK_SQCSCENARIO_FactoryDefault(eFactoryDefaultMode_None);
		} else
		if (strcmp(Default_Values, "keep") == 0)
		{
			APK_EXE_StartFactoryDefault(eDxFAC_DEF_TYPE_KEEP);
		} else
		if (strcmp(Default_Values, "all") == 0)
		{
#if defined( CONFIG_OP_JAPAN )
{
			/* ÀÏº»Çâfactory reset Àº °á°ú¸¦ ´Ù Ã³¸®ÇØ¾ß ÇÒ ¶§±îÁö ±â´Ù·Á¾ß ÇÏ¹Ç·Î
			qeury ¸¦ ½á¼­ ´Ù¸£°Ô °¨ . ³»¿ëÀº µ¿ÀÏÇØ¼­ ¼öÁ¤½Ã °°ÀÌ ÇØÁÖ¼¼¿ä ~ */
			HBOOL bRet = FALSE;
			APK_SQCSCENARIOJAPAN_FactoryReset( &bRet );
}
#else
			APK_EXE_StartFactoryDefault(eDxFAC_DEF_TYPE_ALL);
#endif
		}
	}

	return BPL_STATUS_OK;
}


BPLStatus OPL_LocalSystem_GetStandByPowerMode( HINT32 *pnStandByPowerMode)
{
	APK_POWER_BoxStandbyType_e	standbyType = eAPK_POWER_STANDBYTYPE_NORMAL;

	CHECK_BPL_PARAM_PTR(pnStandByPowerMode);

	(void)APK_POWER_GetStandbyPowerMode(&standbyType);

	//printf("OPL_LocalSystem_GetStandByPowerMode[%d]\n", standbyType);
	if (standbyType == eAPK_POWER_STANDBYTYPE_NORMAL)
		*pnStandByPowerMode = 0;
	else
		*pnStandByPowerMode = 1;
	return BPL_STATUS_OK;
}


BPLStatus OPL_LocalSystem_SetStandByPowerMode( HINT32 nStandByPowerMode)
{
	if (nStandByPowerMode == 0)
		(void)APK_POWER_SetStandbyPowerMode(eAPK_POWER_STANDBYTYPE_NORMAL);
	else
		(void)APK_POWER_SetStandbyPowerMode(eAPK_POWER_STANDBYTYPE_POWERSAVING);
	return BPL_STATUS_OK;
}

BPLStatus	OPL_LocalSystem_SetSleepModeOnStandby(HINT32 eSleepMode)
{
	(void)APK_POWER_SetSleepModeOnStandby(eSleepMode);
	return BPL_STATUS_OK;
}

BPLStatus	OPL_LocalSystem_GetSleepModeOnStandby(HINT32 *pSleepMode)
{
	APK_POWER_SLEEP_MODE_e mode;

	if(APK_POWER_GetSleepModeOnStandby(&mode) ==ERR_FAIL)
	{
		mode = eAPK_POWER_POWERSTATE_DEEP_SLEEP;
		(void)APK_POWER_SetSleepModeOnStandby(mode);
	}
	*pSleepMode = mode;
	return BPL_STATUS_OK;
}

BPLStatus	OPL_LocalSystem_SetBootModeOnStandby(HINT32	eBootMode)
{
	(void)APK_POWER_SetBootModeOnStandby(eBootMode);
	return BPL_STATUS_OK;
}

BPLStatus	OPL_LocalSystem_GetBootModeOnStandby(HINT32 *pBootMode)
{
	APK_POWER_BOOT_MODE_e mode;

	if(APK_POWER_GetBootModeOnStandby(&mode) == ERR_FAIL)
	{
		mode = eAPK_POWER_BOOTMODE_WARM;
		(void)APK_POWER_SetBootModeOnStandby(mode);
	}
	*pBootMode = mode;
	return BPL_STATUS_OK;
}

BPLStatus	OPL_LocalSystem_SetWifiPowerOnStandby(HINT32 bOnOff)
{
	(void)APK_POWER_SetWifiPowerOnStandby(bOnOff);
	return BPL_STATUS_OK;
}

BPLStatus	OPL_LocalSystem_GetWifiPowerOnStandby(HINT32 *pOnOff)
{
	HBOOL enable = TRUE;

	if(APK_POWER_GetWifiPowerOnStandby(&enable) == ERR_FAIL)
	{
		enable = TRUE;
		(void)APK_POWER_SetWifiPowerOnStandby(TRUE);
	}
	*pOnOff  = enable;
	return BPL_STATUS_OK;
}


BPLStatus OPL_LocalSystem_setPowerStateChangeListener(OplLocalSystemPowerStateChange_t listener)
{
	if (s_pfnPowerStateChangeListener == NULL)
	{
		opl_localsystem_registerBoxInfoListener();
		s_pfnPowerStateChangeListener = listener;
	}

	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_setPowerStateChangeErrorListener(OplLocalSystemPowerStateChangeError_t listener)
{
	if (s_pfnPowerStateChangeErrorListener == NULL)
	{
		// TODO: jlabs spec
		/*
			jlabs expended spec .. ±×·±µ¥ .. ¾Æ·¡ ÀÌÀ¯¶óµµ active standby ³Ñ¾î°¡¼­ µ¿ÀÛÇØ¼­ ..
			ÇÊ¿ä ¾ø´Âµ¥ ¾î¶»°Ô ÇÏÁö? T.T

			1 - don't power state for now recording
			2 - don't power state for send data to dms
			4 - don't power state for ip dubbing
			8 - don't power state for another reason
		*/
		s_pfnPowerStateChangeErrorListener = listener;
	}

	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_GetAutomaticStandby( BPL_BOOL *pbAutoStandby)
{
	CHECK_BPL_PARAM_PTR(pbAutoStandby);

	APK_DB_GetInt(USERDB_SYSTEM_AUTOMATIC_POWER_DOWN, (HINT32*)pbAutoStandby);

	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_SetAutomaticStandby( BPL_BOOL bAutoStandby)
{
	APK_DB_SetInt(USERDB_SYSTEM_AUTOMATIC_POWER_DOWN, bAutoStandby == 0 ? FALSE : TRUE);

	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_GetAutoDelete( BPL_BOOL *pbAutoDelete)
{
	HBOOL	bFlag;

	CHECK_BPL_PARAM_PTR(pbAutoDelete);

	APK_SCENARIO_GetAutoDelete(&bFlag);
	*pbAutoDelete = bFlag;

	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_SetAutoDelete( BPL_BOOL bAutoDelete)
{
	APK_SCENARIO_SetAutoDelete(bAutoDelete);

	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_GetStartPaddingTime( int *pnTime)
{
	APK_SCENARIO_GetStartPadding((HINT32*)pnTime);

	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_SetStartPaddingTime( int nTime)
{
	APK_SCENARIO_SetStartPadding((HUINT32)nTime);

	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_GetEndPaddingTime( int *pnTime)
{
	APK_SCENARIO_GetEndPadding((HINT32 *)pnTime);

	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_SetEndPaddingTime( int nTime)
{
	APK_SCENARIO_SetEndPadding((HUINT32)nTime);

	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_GetTargetStorage( int *pnStorage)
{
	APK_DB_GetInt(USERDB_RECORDING_TARGET_STORAGE, (HINT32*)pnStorage);

	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_SetTargetStorage( int nStorage)
{
	APK_DB_SetInt(USERDB_RECORDING_TARGET_STORAGE, nStorage);

	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_GetAudioBleep( BPL_BOOL *pbAdioBleep)
{
	CHECK_BPL_PARAM_PTR(pbAdioBleep);

	APK_DB_GetInt(USERDB_SYSTEM_AUDIO_BLEEP, (HINT32*)pbAdioBleep);

	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_SetAudioBleep( BPL_BOOL bAudioBleep)
{
	APK_DB_SetInt(USERDB_SYSTEM_AUDIO_BLEEP, bAudioBleep == 0 ? FALSE : TRUE);

	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_GetSubtitleFontType( unsigned int *pnSubtitleFontType)
{
	CHECK_BPL_PARAM_PTR(pnSubtitleFontType);

	APK_DB_GetInt(USERDB_SUBTITLE_FONT, (HINT32*)pnSubtitleFontType);

	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_SetSubtitleFontType( unsigned int nSubtitleFontType)
{
	APK_DB_SetInt(USERDB_SUBTITLE_FONT, nSubtitleFontType);
	if (nSubtitleFontType)
		HAPI_BroadcastSignal("signal:onFontTypeChanged", 1, "i", 1);
	else
		HAPI_BroadcastSignal("signal:onFontTypeChanged", 1, "i", 0);
	//	TODO: Set OAPI Value
	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_GetInfoDisplayTimeout( unsigned int *pnInfiDispTimeout)
{
	CHECK_BPL_PARAM_PTR(pnInfiDispTimeout);

	APK_DB_GetInt(USERDB_SCREENDISP_INFO_DISP_TIME, (HINT32*)pnInfiDispTimeout);

	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_SetInfoDisplayTimeout( unsigned int nInfiDispTimeout)
{
	APK_DB_SetInt(USERDB_SCREENDISP_INFO_DISP_TIME, nInfiDispTimeout);

	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_GetInstantReplayTime( unsigned int *pnInstantReplayTime)
{
	CHECK_BPL_PARAM_PTR(pnInstantReplayTime);

	APK_DB_GetInt(USERDB_RECORDING_INSTANT_REPLAY_TIME, (HINT32*)pnInstantReplayTime);

	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_SetInstantReplayTime( unsigned int nInstantReplayTime)
{
	APK_DB_SetInt(USERDB_RECORDING_INSTANT_REPLAY_TIME, nInstantReplayTime);

	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_GetSkipForwardTime( unsigned int *pnSkipForwardTime)
{
	CHECK_BPL_PARAM_PTR(pnSkipForwardTime);

	APK_DB_GetInt(USERDB_RECORDING_SKIP_FORWARD_TIME, (HINT32*)pnSkipForwardTime);

	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_SetSkipForwardTime( unsigned int nSkipForwardTime)
{
	APK_DB_SetInt(USERDB_RECORDING_SKIP_FORWARD_TIME, nSkipForwardTime);

	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_GetFirstTimeBoot( BPL_BOOL *pbFirstTimeBoot)
{
	HINT32		value;

	CHECK_BPL_PARAM_PTR(pbFirstTimeBoot);

	*pbFirstTimeBoot = 0;
	if (APK_DB_GetInt(USERDB_SYSTEM_B_FIRSTTIME_BOOT, &value) != ERR_OK)
		*pbFirstTimeBoot = 1;
	else
	{
		HxLOG_Print("\n\n\nread udb ok : value is [%d]\n\n\n", value);
		if (value)
			*pbFirstTimeBoot = 1;
	}

	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_SetFirstTimeBoot( BPL_BOOL bFirstTimeBoot)
{
	APK_DB_SetInt(USERDB_SYSTEM_B_FIRSTTIME_BOOT, bFirstTimeBoot);

	return BPL_STATUS_OK;
}


BPLStatus OPL_LocalSystem_GetTimeshiftEnabled( BPL_BOOL *bTSR)
{
	CHECK_BPL_PARAM_PTR(bTSR);

	APK_DB_GetInt(USERDB_RECORDING_TSR_ONOFF, (HINT32*)bTSR);

	return BPL_STATUS_OK;
}
BPLStatus OPL_LocalSystem_SetTimeshiftEnabled( BPL_BOOL bTSR)
{
	APK_DB_SetInt(USERDB_RECORDING_TSR_ONOFF, bTSR == 0 ? FALSE : TRUE);

	// TODO: TSR On/Off command via Media Controller.
	if (0 != bTSR)
	{
		APK_MEDIA_PLAY_SetTSREnable (TRUE);
	}
	else
	{
		APK_MEDIA_PLAY_SetTSREnable (FALSE);
	}

	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_GetFavChListNumberingMode( BPL_BOOL *pbFavChListNumberingMode)
{
	CHECK_BPL_PARAM_PTR(pbFavChListNumberingMode);

	APK_SCENARIO_GetFavChListNumberingMode((HBOOL *)pbFavChListNumberingMode);

	return BPL_STATUS_OK;
}
BPLStatus OPL_LocalSystem_SetFavChListNumberingMode( BPL_BOOL bFavChListNumberingMode)
{
	APK_SCENARIO_SetFavChListNumberingMode(bFavChListNumberingMode);

	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_GetDescrambleOnOff( BPL_BOOL *pbOnOff)
{
	CHECK_BPL_PARAM_PTR(pbOnOff);

	APK_SCENARIO_GetDescrambleOnOff((HBOOL *)pbOnOff);

	return BPL_STATUS_OK;
}
BPLStatus OPL_LocalSystem_SetDescrambleOnOff( BPL_BOOL bOnOff)
{
	APK_SCENARIO_SetDescrambleOnOff(bOnOff);

	return BPL_STATUS_OK;
}

/* OIPF Power Control */

BPLStatus OPL_LocalSystem_GetStandby( BPL_BOOL *pbStandby)
{
	return BPL_STATUS_NOT_SUPPORTED;
}

BPLStatus OPL_LocalSystem_SetStandby( BPL_BOOL bStandby)
{
	if (bStandby) {
		// In this case, going to Passive standby.
		APK_OUTPUT_VIDEO_SetAvEnable(FALSE);
		{
			HUINT32	ulViewId;
			APK_MEDIA_PLAY_GetMainViewId(&ulViewId);
			APK_MEDIA_PLAY_Stop(ulViewId);
		}
		APK_POWER_RequestShutDown();
	} else {
		// In this case, going to normal
		APK_OUTPUT_VIDEO_SetAvEnable(TRUE);
		APK_POWER_RequestOperation();
	}

	return BPL_STATUS_NOT_SUPPORTED;
}

BPLStatus OPL_LocalSystem_PowerControl( int OIPFValue)
{
	// in OIPF DAE 7.3.3.1
	switch(OIPFValue)
	{
		case 0: // OFF, No Power
		case 2: // Passive Standby
			HxLOG_Print("Going to Passive StandBy\n");
			APK_OUTPUT_VIDEO_SetAvEnable(FALSE);
			APK_POWER_RequestShutDown();
			break;
		case 1: // ON, Normal Working
			HxLOG_Print("Going to ON!!!\n");
			APK_OUTPUT_VIDEO_SetAvEnable(TRUE);
#if defined(CONFIG_PROD_YS1000) || defined(CONFIG_PROD_YSR2000)
			APK_OUTPUT_PANEL_SetPowerLedState(eDxPowerLED_WATCHING);
#endif
			break;
		case 3: // Active Standby
			HxLOG_Print("Going to Active StandBy\n");
			APK_OUTPUT_VIDEO_SetAvEnable(FALSE);
#if defined(CONFIG_PROD_YS1000) || defined(CONFIG_PROD_YSR2000)
			APK_OUTPUT_PANEL_SetPowerLedState(eDxPowerLED_STANDBY);
#endif
			break;
		default:
			HxLOG_Error("\nInvalid Standby Mode --> [%d]\n",OIPFValue);
		return BPL_STATUS_ERROR;
	}

	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_GetOipfPowerState( int *pnOipfPowerState)
{
	CHECK_BPL_PARAM_PTR(pnOipfPowerState);

	if (-1 == g_nOipfPowerState) {
		HBOOL bStandby = FALSE;

		PAPI_GetLastStandby((HINT32)&bStandby);
		*pnOipfPowerState = 1; //oipf operation
		if(bStandby == TRUE)
		{
			*pnOipfPowerState = 2; // oipf standby
		}
	} else {
		*pnOipfPowerState = (int)g_nOipfPowerState;
	}

	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_GettimeCurrentPowerState( int *pnOipfTimePowerState)
{
	CHECK_BPL_PARAM_PTR(pnOipfTimePowerState);

	if (-1 == g_nOipfTimePowerState) {
		APK_DB_GetInt(USERDB_OIPF_TIME_CUR_POWER_STATE, (HINT32*)pnOipfTimePowerState);
	} else {
		*pnOipfTimePowerState = (int)g_nOipfTimePowerState;
	}

	return BPL_STATUS_OK;
}


BPLStatus OPL_LocalSystem_SetOipfPowerState( int nOipfPowerState)
{
	UNIXTIME ulCurLocalTime;

	HxLOG_Print("[%s:%d] input param nOipfPowerState : [%d]\n", __FUNCTION__, __LINE__, nOipfPowerState);
	HxLOG_Print("[%s:%d] g_nOipfPowerState : [%d]\n", __FUNCTION__, __LINE__, g_nOipfPowerState);
	HxLOG_Print("[%s:%d] g_nOipfPreviousPowerState : [%d]\n", __FUNCTION__, __LINE__, g_nOipfPreviousPowerState);

	if (g_nOipfPowerState != nOipfPowerState)
	{
		if (-1 != g_nOipfPowerState)
		{
			g_nOipfPreviousPowerState = g_nOipfPowerState;
		} else {
			APK_DB_GetInt(USERDB_OIPF_PREVIOUS_POWER_STATE, (HINT32*)&g_nOipfPreviousPowerState);
		}
		APK_DB_SetInt(USERDB_OIPF_PREVIOUS_POWER_STATE, g_nOipfPreviousPowerState);

		g_nOipfPowerState = nOipfPowerState;

		HLIB_DATETIME_ConvertUnixTmeToLocalUnixTime(HLIB_STD_GetSystemTime(),&ulCurLocalTime);
		g_nOipfTimePowerState = ulCurLocalTime;
		APK_DB_SetInt(USERDB_OIPF_TIME_CUR_POWER_STATE, ulCurLocalTime);
		HxLOG_Print("[%s:%d] ulCurLocalTime : [%d]\n", __FUNCTION__, __LINE__, ulCurLocalTime);
	}

	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_GetPreviousOipfPowerState( int *pnOipfPreviousPowerState)
{
	CHECK_BPL_PARAM_PTR(pnOipfPreviousPowerState);

	if (-1 == g_nOipfPreviousPowerState) {
		APK_DB_GetInt(USERDB_OIPF_PREVIOUS_POWER_STATE, (HINT32*)pnOipfPreviousPowerState);
	} else {
		*pnOipfPreviousPowerState = (int)g_nOipfPreviousPowerState;
	}

	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_GetSatRecordConfirmCode( const char *pszRegistrationCode, const char *pszConfirmCode)
{
#ifdef	CONFIG_OP_SES
	//char registrationCode[REGISTRATION_CODE_LENGTH_CHARS +1];
	struct receiverInformation info;

	long confirmedReceiverId;

	CHECK_BPL_PARAM_PTR(pszRegistrationCode);

	confirmedReceiverId = getReceieverIdFromRegistrationCode((unsigned char *)pszRegistrationCode);

	info.platform = PLATFORM_HD_PLUS;
	info.manufacturer = MANUFACTURER_HUMAX;
	info.tuner_config = CONFIG_TWO_CONSTRAINED_TPS;

	getConfirmationCodeFromRegistrationCodeAndReceiverInformation((unsigned char *)pszRegistrationCode, info, (unsigned char *)pszConfirmCode);

	APK_SCENARIO_SetSatRecReceiverId(confirmedReceiverId);
/*
	APK_DB_SetStr(USERDB_POSTCODE, (const HCHAR *)pszPostcode);

	if (APK_SCENARIO_SetPostCode(pszPostcode) != ERR_OK)
	{
		HxLOG_Error("Setting the SES Satrecord Registration is failed..\n");
	}
*/
#endif // CONFIG_OP_SES
	return BPL_STATUS_OK;
}
/*
BPLStatus OPL_LocalSystem_GetSatRecordConfirmCode( char *pszConfirmCode)
{
//	HUINT32		ulValue;

	CHECK_BPL_PARAM_PTR(pszConfirmCode);

	memset(pszConfirmCode,0,8);

	//APK_DB_GetStr((HUINT8 *)USERDB_POSTCODE, pszConfirmcode, ulLen);
	pszConfirmCode[8-1] = '\0';

	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_GetSatRecUpdateInterval( int *pUpdateType, int *pTime)
{
	HERROR	hErr;
	HINT32	nUpdateInterval;
	DxSatRecIntervalType_e	eTempUpdateType;

	hErr = APK_SCENARIO_GetSatRecUpdateInterval(&nUpdateInterval);

	if(hErr == ERR_OK)
	{
		eTempUpdateType = nUpdateInterval & 0xF0000000;

		if (eTempUpdateType == eDxSATREC_INTERVAL_MANUAL)
		{
			*pTime = (nUpdateInterval & 0x0FFFFFFF) / (60*60);
		}
		else
		{
			*pTime = 0;
		}

		*pUpdateType = eTempUpdateType;
		return BPL_STATUS_OK;
	}

	return BPL_STATUS_ERROR;
}
*/

#if 0
static	EnumStringPair_t	_astGMTOffsetList[] =
{
	{	-12,	"-12"	},	{	-11,	"-11"	},
	{	-10,	"-10"	},	{	-9,		"-9"	},
	{	-8,		"-8"	},	{	-7,		"-7"	},
	{	-6, 	"-6"	},	{	-5, 	"-5"	},
	{	-4, 	"-4"	},	{	-3, 	"-3"	},
	{	-2, 	"-2"	},	{	-1, 	"-1"	},
	{	0, 		"+0"	},	{	1, 	"+1"	},
	{	2,		"+2" 	},	{	3,	"+3" },
	{	4,		"+4" 	},	{	5,	"+5" },
	{	6,		"+6" 	},	{	7,	"+7" },
	{	8,		"+8" 	},	{	9,	"+9" },
	{	10,		"+10"	},	{	11,	"+11" },
	{	12, 	"+12"	}, 	{	13, "+13" },
	{	14, 	"+14"	},	{	-100, "Unknown"}
};
#endif

BPLStatus OPL_LocalSystem_SetField( const char *field, const char *value)
{
	CHECK_BPL_PARAM_PTR(field);
	CHECK_BPL_PARAM_PTR(value);

#ifndef EMUL
	if (strcmp(field, "grabkey") == 0)
	{
		HAPI_SendKeyGrab(eAWINDOW_OIPF, TRUE);
		g_nOipfKeyGrab = 1;
	} else
	if (strcmp(field, "ungrabkey") == 0)
	{
		HAPI_SendKeyGrab(eAWINDOW_OIPF, FALSE);
		g_nOipfKeyGrab = 0;
	} else
	if (strcmp(field, "pageActive") == 0)
	{
		if (strcmp(value, "true") == 0)
			HAPI_SendWindowActive(eAWINDOW_OIPF, TRUE);
		else if (strcmp(value, "false") == 0)
			HAPI_SendWindowActive(eAWINDOW_OIPF, FALSE);
	} else
#endif
	if (strcmp(field, "offsetGMT") == 0)
	{
		HINT32	lGmtOffset = 0;

		lGmtOffset = atoi((char *)value);
		fflush(NULL);HxLOG_Print("\n[%s:%d] value:[%s][%d]\n",__FUNCTION__,__LINE__,value, lGmtOffset);fflush(NULL);

		APK_SCENARIO_SetGMTOffset(lGmtOffset);
		return BPL_STATUS_OK;
	} else
	if (strcmp(field, "enableIPEPG") == 0)
	{
		HUINT32 bEnabled;
		// releated with IP EPG functionality.
		fflush(NULL);HxLOG_Print("\n[%s:%d] value:[%s]\n",__FUNCTION__,__LINE__,value);fflush(NULL);
		if (strcmp(value, "true") == 0)
			bEnabled = 1;
		else
			bEnabled = 0;

		APK_DB_SetInt(USERDB_IPEPG_ONOFF, bEnabled);
		HAPI_BroadcastSignal("signal:onIpEPGStateChanged", 0, "i", bEnabled ? 1 : 0);
		return BPL_STATUS_OK;
	} else
	if (strcmp(field, "thumbnailDisplay") == 0)
	{
		HUINT32 bEnabled;
		fflush(NULL);HxLOG_Print("\n[%s:%d] value:[%s]\n",__FUNCTION__,__LINE__,value);fflush(NULL);
		if (strcmp(value, "true") == 0)
			bEnabled = 1;
		else
			bEnabled = 0;

		APK_DB_SetInt(USERDB_THUMBNAIL_ONOFF, bEnabled);
		return BPL_STATUS_OK;
	} else
	if (strcmp(field, "EPGType") == 0)
	{
		HUINT32 bTVTVEnabled;
		HUINT32 bIPEPGEnabled;

		// releated with EPG Type functionality.
		fflush(NULL);HxLOG_Print("\n[%s:%d] value:[%s]\n",__FUNCTION__,__LINE__,value);fflush(NULL);

		if (strcmp(value, "gracenote") == 0)
		{
			bTVTVEnabled = TRUE;
			bIPEPGEnabled = FALSE;
			APK_SCENARIO_SetTVTVEnable(bTVTVEnabled);

			APK_DB_SetInt(USERDB_IPEPG_ONOFF, bIPEPGEnabled);
			HAPI_BroadcastSignal("signal:onIpEPGStateChanged", 0, "i", bIPEPGEnabled ? 1 : 0);

			APK_EXE_Load_TVTVEPG();
		}
		else
		{
			if (strcmp(value, "humaxepg") == 0)
			{
				bTVTVEnabled = FALSE;
				bIPEPGEnabled = TRUE;
			}
			else
			{
				bTVTVEnabled = FALSE;
				bIPEPGEnabled = FALSE;
			}
			APK_SCENARIO_SetTVTVEnable(bTVTVEnabled);
			APK_EXE_TVTVEPG_Initdirectory();

			APK_DB_SetInt(USERDB_IPEPG_ONOFF, bIPEPGEnabled);
			HAPI_BroadcastSignal("signal:onIpEPGStateChanged", 0, "i", bIPEPGEnabled ? 1 : 0);

			APK_EXE_Load_BaseEPG();
		}

		//APK_DB_SetInt(USERDB_IPEPG_ONOFF, bIPEPGEnabled);

		//HAPI_BroadcastSignal("signal:onIpEPGStateChanged", 0, "i", bIPEPGEnabled ? 1 : 0);
		return BPL_STATUS_OK;
	} else
	if (strcmp(field, "enableRemoteRecord") == 0)
	{
		HUINT32 bEnabled;
		// releated with Remote Programming functionality.
		fflush(NULL);HxLOG_Print("\n[%s:%d] value:[%s]\n",__FUNCTION__,__LINE__,value);fflush(NULL);
		if (strcmp(value, "true") == 0)
			bEnabled = 1;
		else
			bEnabled = 0;

		APK_DB_SetInt(USERDB_RP_ONOFF, bEnabled);
		HAPI_BroadcastSignal("signal:onRpStateChanged", 0, "i", bEnabled ? 1 : 0);
		return BPL_STATUS_OK;
	} else
	if (strcmp(field, "enableSatIpNotification") == 0)
	{
		HUINT32 bEnabled;
		fflush(NULL);HxLOG_Print("\n[%s:%d] value:[%s]\n",__FUNCTION__,__LINE__,value);fflush(NULL);
		if (strcmp(value, "true") == 0)
			bEnabled = 1;
		else
			bEnabled = 0;

		APK_DB_SetInt(USERDB_SATIP_NOTIFICATION_ONOFF, bEnabled);
		//HAPI_BroadcastSignal("signal:onSatIpStateChanged", 0, "i", bEnabled ? 1 : 0);
		return BPL_STATUS_OK;
	} else
	if (strcmp(field, "satRecordUpdateInterval") == 0)
	{
		HUINT32 ulSatRecordUpdateInterval;
		// releated with SES Sat Record functionality.
		ulSatRecordUpdateInterval = atoi((char *)value);
		fflush(NULL);HxLOG_Print("\n[%s:%d] value:[%s][%d]\n",__FUNCTION__,__LINE__,value, ulSatRecordUpdateInterval);fflush(NULL);

		APK_DB_SetInt(USERDB_SATRECORD_INTERVAL, ulSatRecordUpdateInterval);
		//HAPI_BroadcastSignal("signal:onRpIntervalChanged", 0, "i", ulInterval);
		//HAPI_BroadcastSignal("signal:onSatRecordIntervalChanged", 0, "i", ulSatRecordUpdateInterval);
		return BPL_STATUS_OK;
	} else
	if (strcmp(field, "satRecordPriority") == 0)
	{
		HUINT32 bSatRecordPriority;
		// releated with SES Sat Record functionality.
		fflush(NULL);HxLOG_Print("\n[%s:%d] value:[%s]\n",__FUNCTION__,__LINE__,value);fflush(NULL);
		if (strcmp(value, "true") == 0)
			bSatRecordPriority = 1;
		else
			bSatRecordPriority = 0;

		APK_DB_SetInt(USERDB_SATRECORD_PRIORITY, bSatRecordPriority);
		//HAPI_BroadcastSignal("signal:onSatRecordPriorityChanged", 0, "i", bSatRecordPriority ? 1 : 0);
		return BPL_STATUS_OK;
	} else
	if (strcmp(field, "updatePeriodRemoteRecord") == 0)
	{
		// releated with Remote Programming functionality.
		HINT32 ulStartTime = 0, ulEndTime = 0;
		HUINT8 startTime[6], endTime[6];
		HUINT32 i = 0, j = 0;
		HBOOL 	first = TRUE;

		HxSTD_MemSet(startTime, 0, 6);
		HxSTD_MemSet(endTime, 0, 6);

		fflush(NULL);HxLOG_Print("\n[%s:%d] value:[%s]\n",__FUNCTION__,__LINE__,value);fflush(NULL);
		// TODO: Convert to UNIXTIME
		//[00:00~00:00]
		while(value[i] != 0)
		{
			if(value[i] == ':')
			{
				first = FALSE;
				i++;
				continue;
			}

			if(first == TRUE)
			{
				startTime[i] = value[i];
				i++;
			}
			else
			{
				endTime[j] = value[i];
				j++;
				i++;
			}
		}

		ulStartTime = atoi((char *)startTime);
		ulEndTime = atoi((char *)endTime);
		fflush(NULL);HxLOG_Print("updatePeriodRemoteRecord[%s] start[%s] end[%s] start[%d] end[%d]\n", value, startTime, endTime, ulStartTime, ulEndTime);fflush(NULL);

		APK_DB_SetInt(USERDB_RP_PERIOD_START, ulStartTime);
		APK_DB_SetInt(USERDB_RP_PERIOD_END, ulEndTime);

		HAPI_BroadcastSignal("signal:onRpPeriodChanged", 0, "ii", ulStartTime, ulEndTime);
		return BPL_STATUS_OK;
	} else
	if (strcmp(field, "updateIntervalRemoteRecord") == 0)
	{
		HUINT32 ulInterval = 0;
		// releated with Remote Programming functionality.
		ulInterval = atoi((char *)value);
		fflush(NULL);HxLOG_Print("\n[%s:%d] value:[%s][%d]\n",__FUNCTION__,__LINE__,value, ulInterval);fflush(NULL);

		APK_DB_SetInt(USERDB_RP_INTERVAL, ulInterval);

		HAPI_BroadcastSignal("signal:onRpIntervalChanged", 0, "i", ulInterval);

		return BPL_STATUS_OK;
	} else
	if (strcmp(field, "registrationCodeRemoteRecord") == 0)
	{
		// releated with Remote Programming functionality.
		fflush(NULL);HxLOG_Print("\n[%s:%d] value:[%s]\n",__FUNCTION__,__LINE__,value);fflush(NULL);
		APK_DB_SetStr(USERDB_RP_REG_CODE, value);
		// TODO : retrieve & get registration code.
		return BPL_STATUS_OK;
	} else
	if (strcmp(field, "enableSeriesPlay") == 0)
	{
		HUINT32 bEnabled;

		// releated with Series Playback
		// TODO: we have to call APPKIT.
		fflush(NULL);HxLOG_Print("\n[%s:%d] value:[%s]\n",__FUNCTION__,__LINE__,value);fflush(NULL);

		if (strcmp(value, "true") == 0)
			bEnabled = 1;
		else
			bEnabled = 0;

		APK_DB_SetInt(USERDB_SERIES_PLAYBACK, bEnabled);
		return BPL_STATUS_OK;
	} else
	if (strcmp(field, "doReboot") == 0)
	{
		fflush(NULL);HxLOG_Print("\n[%s:%d] value:[%s]\n",__FUNCTION__,__LINE__,value);fflush(NULL);
		//PAPI_RequestReboot();
		APK_POWER_RequestReboot();
		return BPL_STATUS_OK;
	} else
	if (strcmp(field, "daylightsaving") == 0)
	{
		HBOOL	bdaylightsaving = FALSE;

		fflush(NULL);HxLOG_Print("\n[%s:%d] value:[%s]\n",__FUNCTION__,__LINE__,value);fflush(NULL);
		if (strcmp(value, "true") == 0)
			bdaylightsaving = 1;
		else
			bdaylightsaving = 0;

		APK_SCENARIO_SetDayLightSaving(bdaylightsaving);

		return BPL_STATUS_OK;
	} else
	if (strcmp(field, "timeupdateonalltp") == 0)
	{
		HBOOL	bupdatetime = FALSE;

		fflush(NULL);HxLOG_Print("\n[%s:%d] value:[%s]\n",__FUNCTION__,__LINE__,value);fflush(NULL);
		if (strcmp(value, "true") == 0)
			bupdatetime = 1;
		else
			bupdatetime = 0;

		APK_SCENARIO_SetTimeUpdate(bupdatetime);

		return BPL_STATUS_OK;
	} else
	if (strcmp(field, "no_ota_action") == 0)
	{
		HUINT32 bNoAction;

		fflush(NULL);HxLOG_Print("\n[%s:%d] value:[%s]\n",__FUNCTION__,__LINE__,value);fflush(NULL);

		if (strcmp(value, "true") == 0)
			bNoAction = 1;
		else
			bNoAction = 0;

		APK_DB_SetInt(USERDB_NO_OTA_ACTION, bNoAction);
		return BPL_STATUS_OK;
	} else
	if (strcmp(field, "no_ota_swversion") == 0)
	{
		fflush(NULL);HxLOG_Print("\n[%s:%d] value:[%s]\n",__FUNCTION__,__LINE__,value);fflush(NULL);
		APK_DB_SetStr(USERDB_NO_OTA_SWVER, value);
		return BPL_STATUS_OK;
	} else
	if (strcmp(field, "later_ota_count") == 0)
	{
		HUINT32 ulCount = 0;
		ulCount = atoi((char *)value);
		fflush(NULL);HxLOG_Print("\n[%s:%d] value:[%s][%d]\n",__FUNCTION__,__LINE__,value, ulCount);fflush(NULL);

		APK_DB_SetInt(USERDB_LATER_OTA_COUNT, ulCount);
	} else
	if (strcmp(field, "livestream_priority") == 0)
	{
		DxLiveStreamPriority_e		 ePriority = eDxLsPriority_EndOfCase;

		if (strcmp(value, "equal_rec") == 0)
		{
			ePriority = eDxLsPriority_EQUAL_REC;
		} else
		if (strcmp(value, "equal_view") == 0)
		{
			ePriority = eDxLsPriority_EQUAL_VIEW;
		} else
		if (strcmp(value, "below_rec") == 0)
		{
			ePriority = eDxLsPriority_BELOW_VIEW;
		} else
		if (strcmp(value, "upper_rec") == 0)
		{
			ePriority = eDxLsPriority_UPPER_REC;
		}

		if (ePriority < eDxLsPriority_EndOfCase)
		{
			(void)APK_SCENARIO_SetLiveStreamPriority(ePriority);
		}
	}
	else if (strcmp(field, "init_humaxtvportal") == 0)
	{
		HUINT32 unValue = 0;

		if (strcmp(value, "true") == 0)
			unValue = 1;
		else
			unValue = 0;
		APK_DB_SetInt(USERDB_INIT_HUMAX_TVPORTAL, unValue);
		HxLOG_Debug("Value Set: %d \n",unValue);
		return BPL_STATUS_OK;
	}
	else if (strcmp(field, "privacy_policy_ver") == 0)
	{
		fflush(NULL);HxLOG_Print("\n[%s:%d] value:[%s]\n",__FUNCTION__,__LINE__,value);fflush(NULL);
		APK_DB_SetStr(USERDB_PP_VER, value);
		(void)HAPI_BroadcastSignal("signal:onPPVerChanged", 0, "s", value);
		return BPL_STATUS_OK;
	}

	// TODO: add more field related code here..
	return BPL_STATUS_NOT_SUPPORTED;
}

BPLStatus OPL_LocalSystem_GetField( const char *field, char *value, unsigned int n)
{
	CHECK_BPL_PARAM_PTR(field);
	CHECK_BPL_PARAM_PTR(value);

	if (strcmp(field, "antenna") == 0)
	{
		if (APK_META_ANTENNA_Count() > 0)
		{
			HxSTD_PrintToStrN(value, n, "{\"antenna\":true}");
			return BPL_STATUS_OK;
		}
		return BPL_STATUS_ERROR;
	} else
	if (strcmp(field, "offsetGMT") == 0)
	{
		HINT32	nGmtOffset=0;

		APK_SCENARIO_GetGMTOffset(&nGmtOffset);
		HxSTD_PrintToStrN(value, n, "%d", nGmtOffset);
		fflush(NULL);HxLOG_Print("\n[%s:%d] value:[%s]\n",__FUNCTION__,__LINE__,value);fflush(NULL);
		return BPL_STATUS_OK;
	} else
	if (strcmp(field, "enableIPEPG") == 0)
	{
		HUINT32 bEnabled;
		// releated with IP EPG functionality.
		APK_DB_GetInt(USERDB_IPEPG_ONOFF, (HINT32*)&bEnabled);

		if (bEnabled)
			HxSTD_PrintToStrN(value, n, "true");
		else
			HxSTD_PrintToStrN(value, n, "false");

		fflush(NULL);HxLOG_Print("\n[%s:%d] value:[%s]\n",__FUNCTION__,__LINE__,value);fflush(NULL);
		return BPL_STATUS_OK;
	} else
	if (strcmp(field, "thumbnailDisplay") == 0)
	{
		HUINT32 bEnabled;
		APK_DB_GetInt(USERDB_THUMBNAIL_ONOFF, (HINT32*)&bEnabled);

		if (bEnabled)
			HxSTD_PrintToStrN(value, n, "true");
		else
			HxSTD_PrintToStrN(value, n, "false");

		fflush(NULL);HxLOG_Print("\n[%s:%d] value:[%s]\n",__FUNCTION__,__LINE__,value);fflush(NULL);
		return BPL_STATUS_OK;
	} else
	if (strcmp(field, "EPGType") == 0)
	{
		HUINT32 bTVTVEnabled = FALSE;
		HUINT32 bIPEPGEnabled = FALSE;

		// releated with EPG Type functionality.
		APK_SCENARIO_GetTVTVEnable((HINT32*)&bTVTVEnabled);
		APK_DB_GetInt(USERDB_IPEPG_ONOFF, (HINT32*)&bIPEPGEnabled);

		if (bTVTVEnabled == TRUE && bIPEPGEnabled == FALSE)
			HxSTD_PrintToStrN(value, n, "gracenote");
		else if (bTVTVEnabled == FALSE && bIPEPGEnabled == TRUE)
			HxSTD_PrintToStrN(value, n, "humaxepg");
		else
			HxSTD_PrintToStrN(value, n, "basicepg");

		fflush(NULL);HxLOG_Print("\n[%s:%d] value:[%s]\n",__FUNCTION__,__LINE__,value);fflush(NULL);
		return BPL_STATUS_OK;
	} else
	if (strcmp(field, "enableRemoteRecord") == 0)
	{
		HUINT32 bEnabled;
		// releated with Remote Programming functionality.
		APK_DB_GetInt(USERDB_RP_ONOFF, (HINT32*)&bEnabled);

		if (bEnabled)
			HxSTD_PrintToStrN(value, n, "true");
		else
			HxSTD_PrintToStrN(value, n, "false");
		fflush(NULL);HxLOG_Print("\n[%s:%d] value:[%s]\n",__FUNCTION__,__LINE__,value);fflush(NULL);
		return BPL_STATUS_OK;
	} else
	if (strcmp(field, "enableSatIpNotification") == 0)
	{
		HUINT32 bEnabled;
		APK_DB_GetInt(USERDB_SATIP_NOTIFICATION_ONOFF, (HINT32*)&bEnabled);

		if (bEnabled)
			HxSTD_PrintToStrN(value, n, "true");
		else
			HxSTD_PrintToStrN(value, n, "false");
		fflush(NULL);HxLOG_Print("\n[%s:%d] value:[%s]\n",__FUNCTION__,__LINE__,value);fflush(NULL);
		return BPL_STATUS_OK;
	} else
	if (strcmp(field, "satRecordUpdateInterval") == 0)
	{
		HUINT32 ulSatRecordUpdateInterval;
		// releated with Remote Programming functionality.
		APK_DB_GetInt(USERDB_SATRECORD_INTERVAL, (HINT32*)&ulSatRecordUpdateInterval);
		HxSTD_PrintToStrN(value, n, "%d", ulSatRecordUpdateInterval);
		fflush(NULL);HxLOG_Print("\n[%s:%d] value:[%s]\n",__FUNCTION__,__LINE__,value);fflush(NULL);
		return BPL_STATUS_OK;
	} else
	if (strcmp(field, "satRecordPriority") == 0)
	{
		HUINT32 bSatRecordPriority;
		// releated with SES Sat Record functionality.
		APK_DB_GetInt(USERDB_SATRECORD_PRIORITY, (HINT32*)&bSatRecordPriority);

		if (bSatRecordPriority)
			HxSTD_PrintToStrN(value, n, "true");
		else
			HxSTD_PrintToStrN(value, n, "false");
		fflush(NULL);HxLOG_Print("\n[%s:%d] value:[%s]\n",__FUNCTION__,__LINE__,value);fflush(NULL);
		return BPL_STATUS_OK;
	} else
	if (strcmp(field, "updatePeriodRemoteRecord") == 0)
	{
		// releated with Remote Programming functionality.
		HUINT32	ulStartTime = 0, ulEndtime = 0;
		APK_DB_GetInt(USERDB_RP_PERIOD_START, (HINT32*)&ulStartTime);
		APK_DB_GetInt(USERDB_RP_PERIOD_END,   (HINT32*)&ulEndtime);

		// TODO: Convert UNIXTIME to USER TIME
		HxSTD_PrintToStrN(value, n, "%d:%d", ulStartTime, ulEndtime);
		fflush(NULL);HxLOG_Print("\n[%s:%d] value:[%s]\n",__FUNCTION__,__LINE__,value);fflush(NULL);
		return BPL_STATUS_OK;
	} else
	if (strcmp(field, "updateIntervalRemoteRecord") == 0)
	{
		HUINT32 ulInterval = 0;
		// releated with Remote Programming functionality.
		APK_DB_GetInt(USERDB_RP_INTERVAL, (HINT32*)&ulInterval);
		HxSTD_PrintToStrN(value, n, "%d", ulInterval);
		fflush(NULL);HxLOG_Print("\n[%s:%d] value:[%s]\n",__FUNCTION__,__LINE__,value);fflush(NULL);
		return BPL_STATUS_OK;
	} else
	if (strcmp(field, "registrationCodeRemoteRecord") == 0)
	{
		memset(value,0,n);
		// releated with Remote Programming functionality.
		APK_DB_GetStr((HCHAR *)USERDB_RP_REG_CODE, value, n);
		value[n-1]='\0';
		fflush(NULL);HxLOG_Print("\n[%s:%d] value:[%s]\n",__FUNCTION__,__LINE__,value);fflush(NULL);
		// TODO : retrieve & get registration code.
		return BPL_STATUS_OK;
	} else
	if (strcmp(field, "ipepgLastUpdateTime") == 0)
	{
		HUINT32 uLastUpdateTime = 0;
		// releated with Remote Programming functionality.
		APK_DB_GetInt(USERDB_IPEPG_LASTUPDATETIME, (HINT32*)&uLastUpdateTime);

		// TODO: Convert UNIXTIME to USER TIME
		HxSTD_PrintToStrN(value, n, "%d", uLastUpdateTime);
		fflush(NULL);HxLOG_Print("\n[%s:%d] value:[%s]\n",__FUNCTION__,__LINE__,value);fflush(NULL);

		return BPL_STATUS_OK;
	} else
	if (strcmp(field, "ipepgDownloadProgress") == 0)
	{
		HUINT32 uDownloadProgress = 0;
		// releated with Remote Programming functionality.
		APK_DB_GetInt(USERDB_IPEPG_DOWNLOADPROGRESS, (HINT32*)&uDownloadProgress);

		HxSTD_PrintToStrN(value, n, "%d", uDownloadProgress);
		fflush(NULL);HxLOG_Print("\n[%s:%d] value:[%s]\n",__FUNCTION__,__LINE__,value);fflush(NULL);

		return BPL_STATUS_OK;
	} else
	if (strcmp(field, "tvtvEpgLastUpdateTime") == 0)
	{
		HUINT32 uLastUpdateTime = 0;
		// releated with Remote Programming functionality.
		APK_DB_GetInt(USERDB_TVTVEPG_LASTUPDATETIME, (HINT32*)&uLastUpdateTime);

		// TODO: Convert UNIXTIME to USER TIME
		HxSTD_PrintToStrN(value, n, "%d", uLastUpdateTime);
		fflush(NULL);HxLOG_Print("\n[%s:%d] value:[%s]\n",__FUNCTION__,__LINE__,value);fflush(NULL);

		return BPL_STATUS_OK;
	} else
	if (strcmp(field, "enableSeriesPlay") == 0)
	{
		HUINT32 bEnabled;

		// releated with Series Playback
		APK_DB_GetInt(USERDB_SERIES_PLAYBACK, (HINT32*)&bEnabled);

		if (bEnabled)
			HxSTD_PrintToStrN(value, n, "true");
		else
			HxSTD_PrintToStrN(value, n, "false");
		fflush(NULL);HxLOG_Print("\n[%s:%d] value:[%s]\n",__FUNCTION__,__LINE__,value);fflush(NULL);

		return BPL_STATUS_OK;
	} else
	if (strcmp(field, "daylightsaving") == 0)
	{
		HBOOL	bdaylightsaving = FALSE;

		APK_SCENARIO_GetDayLightSaving(&bdaylightsaving);

		if (bdaylightsaving)
			HxSTD_PrintToStrN(value, n, "true");
		else
			HxSTD_PrintToStrN(value, n, "false");

		fflush(NULL);HxLOG_Print("\n[%s:%d] value:[%s]\n",__FUNCTION__,__LINE__,value);fflush(NULL);
		return BPL_STATUS_OK;
	} else
	if (strcmp(field, "timeupdateonalltp") == 0)
	{
		HBOOL	bupdateTime = FALSE;

		APK_SCENARIO_GetTimeUpdate(&bupdateTime);

		if (bupdateTime)
			HxSTD_PrintToStrN(value, n, "true");
		else
			HxSTD_PrintToStrN(value, n, "false");

		fflush(NULL);HxLOG_Print("\n[%s:%d] value:[%s]\n",__FUNCTION__,__LINE__,value);fflush(NULL);
		return BPL_STATUS_OK;
	} else
	if (strcmp(field, "no_ota_action") == 0)
	{
		HUINT32 bNoAction;
		APK_DB_GetInt(USERDB_NO_OTA_ACTION, (HINT32*)&bNoAction);

		if (bNoAction)
			HxSTD_PrintToStrN(value, n, "true");
		else
			HxSTD_PrintToStrN(value, n, "false");
		fflush(NULL);HxLOG_Print("\n[%s:%d] value:[%s]\n",__FUNCTION__,__LINE__,value);fflush(NULL);
		return BPL_STATUS_OK;
	} else
	if (strcmp(field, "no_ota_swversion") == 0)
	{
		memset(value,0,n);
		APK_DB_GetStr((HCHAR *)USERDB_NO_OTA_SWVER, value, n);

		value[n-1]='\0';
		fflush(NULL);HxLOG_Print("\n[%s:%d] value:[%s]\n",__FUNCTION__,__LINE__,value);fflush(NULL);
		return BPL_STATUS_OK;
	} else
	if (strcmp(field, "later_ota_count") == 0)
	{
		HUINT32 ulCount;
		APK_DB_GetInt(USERDB_LATER_OTA_COUNT, (HINT32*)&ulCount);

		HxSTD_PrintToStrN(value, n, "%d", ulCount);
		fflush(NULL);HxLOG_Print("\n[%s:%d] value:[%s]\n",__FUNCTION__,__LINE__,value);fflush(NULL);
		return BPL_STATUS_OK;
	} else
	if (strcmp(field, "swupdate_version") == 0)
	{
		memset(value,0,n);
		APK_SCENARIO_GetSWUpdateVersion(value, n);
		value[n-1]='\0';

		fflush(NULL);HxLOG_Print("\n[%s:%d] value:[%s]\n",__FUNCTION__,__LINE__,value);fflush(NULL);
		return BPL_STATUS_OK;
	} else
	if (strcmp(field, "livestream_priority") == 0)
	{
		DxLiveStreamPriority_e		 ePriority = eDxLsPriority_EndOfCase;

		(void)APK_SCENARIO_GetLiveStreamPriority(&ePriority);

		switch (ePriority)
		{
		case eDxLsPriority_EQUAL_VIEW:
			HxSTD_PrintToStrN(value, n, "equal_view");
			break;

		case eDxLsPriority_BELOW_VIEW:
			HxSTD_PrintToStrN(value, n, "below_rec");
			break;

		case eDxLsPriority_UPPER_REC:
			HxSTD_PrintToStrN(value, n, "upper_rec");
			break;

		case eDxLsPriority_EQUAL_REC:
		default:
			HxSTD_PrintToStrN(value, n, "equal_rec");
			break;
		}

		fflush(NULL);HxLOG_Print("\n[%s:%d] value:[%s]\n",__FUNCTION__,__LINE__,value);fflush(NULL);
		return BPL_STATUS_OK;
	} else
	if (strcmp(field, "ipepgDownloadSvcCount") == 0)
	{
		HUINT32 uDownloadSvcCount = 0;
		// releated with Remote Programming functionality.
		APK_DB_GetInt(USERDB_IPEPG_SVC_COUNT, (HINT32*)&uDownloadSvcCount);

		HxSTD_PrintToStrN(value, n, "%d", uDownloadSvcCount);
		fflush(NULL);
		HxLOG_Print("\n[%s:%d] value:[%s]\n",__FUNCTION__,__LINE__,value);fflush(NULL);

		return BPL_STATUS_OK;
	} else
	if (strcmp(field, "ipepgDownloadEvtCount") == 0)
	{
		HUINT32 uDownloadEvtCount = 0;
		// releated with Remote Programming functionality.
		APK_DB_GetInt(USERDB_IPEPG_EVT_COUNT, (HINT32*)&uDownloadEvtCount);

		HxSTD_PrintToStrN(value, n, "%d", uDownloadEvtCount);
		fflush(NULL);
		HxLOG_Print("\n[%s:%d] value:[%s]\n",__FUNCTION__,__LINE__,value);fflush(NULL);

		return BPL_STATUS_OK;
	}
	else if (strcmp(field, "init_humaxtvportal") == 0)
	{
		HUINT32 unInit= 0;
		if(APK_DB_GetInt(USERDB_INIT_HUMAX_TVPORTAL,(HINT32*)&unInit) == ERR_FAIL)
		{
			unInit = 0;
			APK_DB_SetInt(USERDB_INIT_HUMAX_TVPORTAL,unInit);
			HxLOG_Debug("Default Value Set: %d \n",unInit);
		}
		if (unInit)
			HxSTD_PrintToStrN(value, n, "true");
		else
			HxSTD_PrintToStrN(value, n, "false");

		HxLOG_Debug("Value : %d \n",unInit);
	}
	else if (strcmp(field, "privacy_policy_ver") == 0)
	{
		memset(value,0,n);
		APK_DB_GetStr((HCHAR *)USERDB_PP_VER, value, n);
	
		value[n-1]='\0';
		fflush(NULL);HxLOG_Print("\n[%s:%d] value:[%s]\n",__FUNCTION__,__LINE__,value);fflush(NULL);
		return BPL_STATUS_OK;
	}
	// TODO: add more field related code here..
	return BPL_STATUS_NOT_SUPPORTED;
}

BPLStatus OPL_LocalSystem_SetPostcode( const char *pszPostcode)
{
//	HUINT32		ulValue;

	CHECK_BPL_PARAM_PTR(pszPostcode);

	APK_DB_SetStr(USERDB_POSTCODE, (const HCHAR *)pszPostcode);

	if (APK_SCENARIO_SetPostCode(pszPostcode) != ERR_OK)
	{
		/* APK_SQCSCENARIO_SetFsatPostcodeï¿½ï¿½ octoï¿½ï¿½ ï¿½Ý¿ï¿½ï¿½Ï±ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½Úµï¿½ï¿½ï¿½.
		DBï¿½ï¿½ï¿½ï¿½ ï¿½Ý¿ï¿½ï¿½ï¿½ ï¿½Ç¾ï¿½ï¿½Ö°ï¿½, OCTOï¿½ï¿½ ï¿½Ý¿ï¿½ï¿½ï¿½ Failï¿½ï¿½ ï¿½ï¿½ï¿?Error Ã³ï¿½ï¿½ï¿½ï¿½ ï¿½î¶»ï¿½ï¿½ ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½Ø¾ï¿?ï¿½ï¿½.
		DBï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿?ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ì¼± ï¿½ï¿½ï¿½ï¿½ï¿½Ï¿ï¿½, APK_SQCSCENARIO_SetPostcode ï¿½ï¿½ Errorï¿½ï¿½ Returnï¿½Ï´ï¿½ï¿½ï¿½
		Debug Msgï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½Ï°ï¿?Return Errorï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½Ê´Â´ï¿½. */
		HxLOG_Error("Setting the freesat postcode is failed..\n");
	}

	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_GetPostcode( char *pszPostcode, unsigned int ulLen)
{
//	HUINT32		ulValue;

	CHECK_BPL_PARAM_PTR(pszPostcode);

	memset(pszPostcode,0,ulLen);
	APK_DB_GetStr((HCHAR *)USERDB_POSTCODE, pszPostcode, ulLen);
	pszPostcode[ulLen-1] = '\0';

	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_GetCIPlusEnabled( char *pszEnabled)
{
	HBOOL bCI;
;
	CHECK_BPL_PARAM_PTR(pszEnabled);

	APK_CAS_CI_IsSupportedCIPlus((HBOOL *)&bCI);

	if(bCI == TRUE)
	{
		HxSTD_StrNCpy(pszEnabled, "enable", 7);
	}
	else
	{
		HxSTD_StrNCpy(pszEnabled, "disable", 8);
	}

	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_GetCIPlusVersion( char *pszVersion)
{
	char szVersion[APKS_CI_PLUS_VER_LENG] = {0,};

	CHECK_BPL_PARAM_PTR(pszVersion);

	APK_CAS_CI_GetCIPlusVersionName(szVersion);

	HxSTD_StrNCpy(pszVersion, szVersion, APKS_CI_PLUS_VER_LENG - 1);

	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_GetCISlotCount( char *pszCount)
{
	HUINT32 ulCount;

	CHECK_BPL_PARAM_PTR(pszCount);

	APK_CAS_CI_GetSlotCount((HUINT32 *)&ulCount);

	HxSTD_snprintf(pszCount, 2, "%d", ulCount);

	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_GetCamNameBySlot( unsigned char ucSlot, char *pszCamName)
{
	char					szCamName[APKS_CAM_NAME_LEN] = {0,};
	HUINT32				ulCount;
	DxCASCI_CamStatus_e	eCamStatus;

	CHECK_BPL_PARAM_PTR(pszCamName);

	APK_CAS_CI_GetSlotCount((HUINT32 *)&ulCount);

	if(ucSlot >= ulCount)
	{
		return BPL_STATUS_ERROR;
	}

	APK_CAS_CI_GetCamStatusByIndex(ucSlot, &eCamStatus);
	HxLOG_Print("[OPL_LocalSystem_GetCamNameBySlot] eCamStatus of Slot[%d]:: [%d]\n", ucSlot, eCamStatus);
	if(eCamStatus != eDxCASCI_CAM_STATUS_Initialized)
	{
		return BPL_STATUS_ERROR;
	}
	else
	{
		APK_CAS_CI_GetCamNameByIndex(ucSlot, szCamName);
	}

	HxSTD_StrNCpy(pszCamName, szCamName, APKS_CAM_NAME_LEN - 1);

	return BPL_STATUS_OK;
}

static void opl_localsystem_CamStateChangeListener(HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3)
{
	if(CamStatusChangeListener != NULL)
	{
		CamStatusChangeListener((unsigned char)ulParam1, (unsigned int)ulParam2);
	}

}

static void opl_localsystem_ExtDevInfoListener(DxHotplug_NotiType_e eType, HINT32 p1, HINT32 p2, HINT32 p3)
{
	DxHotplug_Info_t	stHotplugInfo;
	HBOOL				bConnected = p1;

	memset(&stHotplugInfo, 0x00, sizeof(DxHotplug_Info_t));
	if (p2 != (HINT32)NULL)
	{
		memcpy(&stHotplugInfo, (const void *)p2, sizeof(DxHotplug_Info_t));
	}

	switch(eType)
	{
	case eDxHOTPLUG_NOTI_DevChange:
		switch(stHotplugInfo.eDevType)
		{
		case eDxHOTPLUG_DEVTYPE_MIC:
		case eDxHOTPLUG_DEVTYPE_USB_WIRELESS:
			//printf("[%s:%d]eDxHOTPLUG_NOTI_DevChange, %s, %d, %d\n", __FUNCTION__, __LINE__, (p1 == 1) ? "CONNECT" : "DISCONNECT", p2, p3);
			//printf("[%s:%d]ulDeviceId : %d\n", __FUNCTION__, __LINE__, stHotplugInfo.ulDeviceId);
			//printf("[%s:%d]eDevType : %d\n", __FUNCTION__, __LINE__, stHotplugInfo.eDevType);
			//printf("[%s:%d]nPortNum : %d\n", __FUNCTION__, __LINE__, stHotplugInfo.nPortNum);
			s_pfnExtDevInfoListener(stHotplugInfo.eDevType, eType, bConnected, stHotplugInfo.ulDeviceId, stHotplugInfo.nPortNum);
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
}

BPLStatus OPL_LocalSystem_UserActionIdle(void)
{
	if(UserActionIdleListener != NULL)
	{
		UserActionIdleListener();
	}
	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_KeyboardResult(char *resultStr)
{

	if(KeyboardResultListener != NULL)
	{
		KeyboardResultListener(resultStr);
	}
	return BPL_STATUS_OK;
}


BPLStatus OPL_LocalSystem_SetUserActionIdleListener(OplLoacalSystemUserActionIdle_t listener)
{
	if(UserActionIdleListener != NULL)
	{
		HxLOG_Error("Already registered UserActionIdle listner.\n");
		return BPL_STATUS_ERROR;
	}

	UserActionIdleListener = listener;

	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_SetKeyboardResultListener(OplLoacalSystemKeyboardResult_t listener)
{
	if(KeyboardResultListener != NULL)
	{
		HxLOG_Error("Already registered KeyboardResult listner.\n");
		return BPL_STATUS_ERROR;
	}

	KeyboardResultListener = listener;

	return BPL_STATUS_OK;
}


BPLStatus OPL_LocalSystem_setExtDevInfoListener(OplLoacalSystemExtDevInfo_t listener)
{
	if (s_pfnExtDevInfoListener != NULL)
	{
		HxLOG_Error("Already registered ExtDevInfo(External Device Info) listner.\n");
		return BPL_STATUS_ERROR;
	}

	s_pfnExtDevInfoListener = listener;
	APK_EXTDEV_RegisterNotifier(opl_localsystem_ExtDevInfoListener);

	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_SetCamStateChangeListener(OplLoacalSystemCamStateChange_t listener)
{
	if(CamStatusChangeListener != NULL)
	{
		HxLOG_Error("Already registered CamStateChange listner.\n");
		return BPL_STATUS_ERROR;
	}

	CamStatusChangeListener = listener;

	APK_CAS_CI_RegisterNotifier(opl_localsystem_CamStateChangeListener);

	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_UnsetCamStateChangeListener(void)
{
	CamStatusChangeListener = NULL;

	APK_CAS_CI_UnRegisterNotifier(opl_localsystem_CamStateChangeListener);

	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_SetSmartcardStateChangedListener(OplLoacalSystemSmartcardStateChange_t listener)
{
#ifdef CONFIG_OP_SES
	return OPL_LocalSystem_SetNagraSCStateListener(listener);
#else
	return BPL_STATUS_OK;
#endif
}

static void opl_localsystem_ThermalProtectionOccuredListener(HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3)
{
	if(s_ThermalProtectionOccuredListener != NULL)
	{
		s_ThermalProtectionOccuredListener();
	}
}

BPLStatus OPL_LocalSystem_RegisterThermalProtectionOccuredListener(OplLocalSystemThermalProtection_t listener)
{
	s_ThermalProtectionOccuredListener = listener;
	APK_OUTPUT_THERMAL_RegisterNotifier((APLK_OUTPUT_THERMAL_Notifier_t) opl_localsystem_ThermalProtectionOccuredListener);
	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_UnregisterThermalProtectionListener(void)
{
	s_ThermalProtectionOccuredListener = NULL;
	APK_OUTPUT_THERMAL_UnregisterNotifier();
	return BPL_STATUS_OK;
}


#if defined(CONFIG_OP_FREESAT)
static	APKS_Pair_t	_astSQCExePairList[] =
{
	{	eSQC_Execution_StartFSATBGEIT,		"startfsatbgeit"		},
	{	eSQC_Execution_StartFSATBGMETA,		"startfsatbgmeta"		},
	{	eSQC_Execution_StopFSATBGEIT,		"stopfsatbgeit"		},
	{	eSQC_Execution_StopFSATBGMETA, 	"stopfsatbgmeta"		},
	{	eSQC_Execution_MAX, 		NULL		}
};
#endif

BPLStatus OPL_LocalSystem_ExecuteCommand( int eExeValue)
{
	switch (eExeValue)
	{
		case	0://eSQC_Execution_StartFSATBGEIT:
			break;

		default:
			break;
	}

	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_ShowMsgbox(void)
{

	HAPI_BroadcastSignal("signal:onMsgboxShow", 0, "s", 0);

	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_ShowKeyboard(int type, int pwmode, char *str)
{

	printf("@@@ [chlee[ OPL_LocalSystem_ShowKeyboard \r\n");
	HAPI_BroadcastSignal("signal:onOSKModeChanged", 0, "i", pwmode);

	switch(type)
	{
		case 0:
			HAPI_BroadcastSignal("signal:onOSKShow", 0, "s", str);
			break;
		case 1:
			HAPI_BroadcastSignal("signal:onShowSimpleKeyboard", 0, "s", str);
			break;
	}

	return BPL_STATUS_OK;
}



#define	___IrdetoInfo_Related_Functions____

static void opl_localsystem_IrdetoInfoChangeListener(HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3)
{
	if(IrdetoInfoChangeListener != NULL)
	{
		IrdetoInfoChangeListener((unsigned char)ulParam1, (unsigned int)ulParam2);
	}
}

BPLStatus OPL_LocalSystem_SetIrdetoInfoChangeListener(OplLocalSystemIrdetoInfoChange_t listener)
{
	if(IrdetoInfoChangeListener != NULL)
	{
		HxLOG_Error("Already registered CamStateChange listner.\n");
		return BPL_STATUS_ERROR;
	}

	IrdetoInfoChangeListener = listener;

	APK_CAS_IRDETO_RegisterNotifier((APK_CAS_IRDETO_Notifier_t)opl_localsystem_IrdetoInfoChangeListener);

	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_UnsetIrdetoInfoChangeListener(void)
{
	IrdetoInfoChangeListener = NULL;

	APK_CAS_IRDETO_UnregisterNotifier();//opl_localsystem_IrdetoInfoChangeListener);

	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_SetIrdetoVirtualSecureCW( BPL_BOOL bVirtualSecureCW)
{
	APK_CAS_IRDETO_SetVirtualSecureCwMode(bVirtualSecureCW);

	return BPL_STATUS_OK;
}

BPLStatus	OPL_LocalSystem_GetIrdetoVirtualSecureCwMode(BPL_BOOL *pbCwMode)
{
	HBOOL	cwmode;

	CHECK_BPL_PARAM_PTR(pbCwMode);

	if (APK_CAS_IRDETO_GetVirturlSecureCwMode(&cwmode) == ERR_OK)
	{
		*pbCwMode = (BPL_BOOL)cwmode;

		return BPL_STATUS_OK;
	}

	return BPL_STATUS_ERROR;
}


BPLStatus OPL_LocalSystem_GetIrdetoClientVersion( char *pszVersion)
{
	char szVersion[OAPI_CAS_IRDETO_CLIENT_VERSION_LENG] = {0,};

	CHECK_BPL_PARAM_PTR(pszVersion);

	APK_CAS_IRDETO_GetIrdetoClientVersionName(szVersion);

	HxSTD_StrNCpy(pszVersion, szVersion, OAPI_CAS_IRDETO_CLIENT_VERSION_LENG - 1);

	return BPL_STATUS_OK;
}





BPLStatus	OPL_LocalSystem_GetIrdetoCWE(BPL_BOOL *pbCWE)
{
	HBOOL	cwe;

	CHECK_BPL_PARAM_PTR(pbCWE);

	if (APK_CAS_IRDETO_IsCWE(&cwe) == ERR_OK)
	{
		*pbCWE = (BPL_BOOL)cwe;

		return BPL_STATUS_OK;
	}

	return BPL_STATUS_ERROR;
}

BPLStatus	OPL_LocalSystem_GetIrdetoJTAG(BPL_BOOL *pbJTAG)
{
	HBOOL	jtag;

	CHECK_BPL_PARAM_PTR(pbJTAG);

	if (APK_CAS_IRDETO_IsJTAG(&jtag) == ERR_OK)
	{
		*pbJTAG = (BPL_BOOL)jtag;

		return BPL_STATUS_OK;
	}

	return BPL_STATUS_ERROR;
}

BPLStatus	OPL_LocalSystem_GetIrdetoSBOOT(BPL_BOOL *pbSBOOT)
{
	HBOOL	sboot;

	CHECK_BPL_PARAM_PTR(pbSBOOT);

	if (APK_CAS_IRDETO_IsSBOOT(&sboot) == ERR_OK)
	{
		*pbSBOOT = (BPL_BOOL)sboot;

		return BPL_STATUS_OK;
	}

	return BPL_STATUS_ERROR;
}

BPLStatus	OPL_LocalSystem_GetIrdetoOTP(BPL_BOOL *pbOTP)
{
	HBOOL	otp;

	CHECK_BPL_PARAM_PTR(pbOTP);

	if (APK_CAS_IRDETO_IsOTP(&otp) == ERR_OK)
	{
		*pbOTP = (BPL_BOOL)otp;

		return BPL_STATUS_OK;
	}

	return BPL_STATUS_ERROR;
}

BPLStatus OPL_LocalSystem_GetIrdetoManCode( char *pszMancode)
{
	char szManCode[OAPI_CAS_IRDETO_SYSTEM_INFO_COMPONENT_LENG] = {0,};

	CHECK_BPL_PARAM_PTR(pszMancode);

	APK_CAS_IRDETO_GetIrdetoManCode(szManCode);

	HxSTD_StrNCpy(pszMancode, szManCode, OAPI_CAS_IRDETO_CLIENT_VERSION_LENG - 1);

	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_GetIrdetoHwCode( char *pszHWCode)
{
	char szHwCode[OAPI_CAS_IRDETO_SYSTEM_INFO_COMPONENT_LENG] = {0,};

	CHECK_BPL_PARAM_PTR(pszHWCode);

	APK_CAS_IRDETO_GetIrdetoHwCode(szHwCode);

	HxSTD_StrNCpy(pszHWCode, szHwCode, OAPI_CAS_IRDETO_CLIENT_VERSION_LENG - 1);

	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_GetIrdetoVariant( char *pszVariant)
{
	char szVariant[OAPI_CAS_IRDETO_SYSTEM_INFO_COMPONENT_LENG] = {0,};

	CHECK_BPL_PARAM_PTR(pszVariant);

	APK_CAS_IRDETO_GetIrdetoVariant(szVariant);

	HxSTD_StrNCpy(pszVariant, szVariant, OAPI_CAS_IRDETO_CLIENT_VERSION_LENG - 1);

	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_GetIrdetoSysId( char *pszSysId)
{
	char szSysId[OAPI_CAS_IRDETO_SYSTEM_INFO_COMPONENT_LENG] = {0,};

	CHECK_BPL_PARAM_PTR(pszSysId);

	APK_CAS_IRDETO_GetIrdetoSysId(szSysId);

	HxSTD_StrNCpy(pszSysId, szSysId, OAPI_CAS_IRDETO_CLIENT_VERSION_LENG - 1);

	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_GetIrdetoKeyVer( char *pszKeyVer)
{
	char szKeyVer[OAPI_CAS_IRDETO_SYSTEM_INFO_COMPONENT_LENG] = {0,};

	CHECK_BPL_PARAM_PTR(pszKeyVer);

	APK_CAS_IRDETO_GetIrdetoKeyVer(szKeyVer);

	HxSTD_StrNCpy(pszKeyVer, szKeyVer, OAPI_CAS_IRDETO_CLIENT_VERSION_LENG - 1);

	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_GetIrdetoSigVer( char *pszSigVer)
{
	char szSigVer[OAPI_CAS_IRDETO_SYSTEM_INFO_COMPONENT_LENG] = {0,};

	CHECK_BPL_PARAM_PTR(pszSigVer);

	APK_CAS_IRDETO_GetIrdetoSigVer(szSigVer);

	HxSTD_StrNCpy(pszSigVer, szSigVer, OAPI_CAS_IRDETO_CLIENT_VERSION_LENG - 1);

	return BPL_STATUS_OK;
}

BPLStatus	OPL_LocalSystem_GetControlByte(int *pControlByte)
{
	HUINT8	unContolByte;

	CHECK_BPL_PARAM_PTR(pControlByte);

	if (APK_CAS_IRDETO_GetControlByte(&unContolByte) == ERR_OK)
	{
		*pControlByte = unContolByte;

		return BPL_STATUS_OK;
	}

	return BPL_STATUS_ERROR;
}


BPLStatus	OPL_LocalSystem_GetPrivateData(char *pszPrivateData)
{
	char szPrivateData [OAPI_CAS_IRDETO_CLIENT_VERSION_LENG] = {0,};

	CHECK_BPL_PARAM_PTR(pszPrivateData);

	APK_CAS_IRDETO_GetPrivateData((HUINT8 *)szPrivateData);

	HxSTD_StrNCpy(pszPrivateData, szPrivateData, OAPI_CAS_IRDETO_CLIENT_VERSION_LENG - 1);

	return BPL_STATUS_ERROR;
}



#define	___NagraInfo_Related_Functions____

static void opl_localsystem_NagraInfoChangeListener(HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3)
{

	if(s_NagraInfoChangeListener != NULL)
	{
		s_NagraInfoChangeListener((unsigned char)ulParam1, (unsigned int)ulParam2);
	}

}

BPLStatus OPL_LocalSystem_SetNagraInfoChangeListener(OplLocalSystemNagraInfoChange_t listener)
{
	if(s_NagraInfoChangeListener != NULL)
	{
		HxLOG_Error("Already registered CamStateChange listner.\n");
		return BPL_STATUS_ERROR;
	}

	s_NagraInfoChangeListener = listener;

	APK_CAS_NAGRA_RegisterNotifier((APK_CAS_NAGRA_Notifier_t)opl_localsystem_NagraInfoChangeListener);

	return BPL_STATUS_OK;
}

static void opl_localsystem_NagraSCStateListener(HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3)
{
	if(s_NagraSCStateListener != NULL)
	{
		s_NagraSCStateListener(0, (unsigned int)ulParam1);
	}

}

BPLStatus OPL_LocalSystem_SetNagraSCStateListener(OplLocalSystemNagraSCState_t listener)
{
	if(s_NagraSCStateListener != NULL)
	{
		HxLOG_Error("Already registered CamStateChange listner.\n");
		return BPL_STATUS_ERROR;
	}

	s_NagraSCStateListener = listener;

	APK_CAS_NAGRA_RegisterNotifier((APK_CAS_NAGRA_Notifier_t)opl_localsystem_NagraSCStateListener);

	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_UnsetNagraInfoChangeListener(void)
{
	s_NagraInfoChangeListener = NULL;

	APK_CAS_NAGRA_UnregisterNotifier();//opl_localsystem_NagraInfoChangeListener);

	return BPL_STATUS_OK;
}

BPLStatus	OPL_LocalSystem_GetNagraCWE(BPL_BOOL *pbCWE)
{
	HBOOL	cwe;

	CHECK_BPL_PARAM_PTR(pbCWE);

	if (APK_CAS_NAGRA_IsCWE(&cwe) == ERR_OK)
	{
		*pbCWE = (BPL_BOOL)cwe;

		return BPL_STATUS_OK;
	}

	return BPL_STATUS_ERROR;
}

BPLStatus	OPL_LocalSystem_GetNagraJTAG(BPL_BOOL *pbJTAG)
{
	HBOOL	jtag;

	CHECK_BPL_PARAM_PTR(pbJTAG);

	if (APK_CAS_NAGRA_IsJTAG(&jtag) == ERR_OK)
	{
		*pbJTAG = (BPL_BOOL)jtag;

		return BPL_STATUS_OK;
	}

	return BPL_STATUS_ERROR;
}

BPLStatus	OPL_LocalSystem_GetNagraSBOOT(BPL_BOOL *pbSBOOT)
{
	HBOOL	sboot;

	CHECK_BPL_PARAM_PTR(pbSBOOT);

	if (APK_CAS_NAGRA_IsSBOOT(&sboot) == ERR_OK)
	{
		*pbSBOOT = (BPL_BOOL)sboot;

		return BPL_STATUS_OK;
	}

	return BPL_STATUS_ERROR;
}

BPLStatus	OPL_LocalSystem_GetNagraOTP(BPL_BOOL *pbOTP)
{
	HBOOL	otp;

	CHECK_BPL_PARAM_PTR(pbOTP);

	if (APK_CAS_NAGRA_IsOTP(&otp) == ERR_OK)
	{
		*pbOTP = (BPL_BOOL)otp;

		return BPL_STATUS_OK;
	}

	return BPL_STATUS_ERROR;
}

BPLStatus OPL_LocalSystem_GetNagraNUID( char *pszMancode)
{
	char szManCode[OAPI_CAS_NAGRA_SYSTEM_INFO_COMPONENT_LENG] = {0,};

	CHECK_BPL_PARAM_PTR(pszMancode);

	APK_CAS_NAGRA_GetNUID(szManCode);

	HxSTD_StrNCpy(pszMancode, szManCode, OAPI_CAS_NAGRA_CLIENT_VERSION_LENG - 1);

	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_GetNagraCASN( char *pszHWCode)
{
	char szHwCode[OAPI_CAS_NAGRA_SYSTEM_INFO_COMPONENT_LENG] = {0,};

	CHECK_BPL_PARAM_PTR(pszHWCode);

	APK_CAS_NAGRA_GetCASN(szHwCode);

	HxSTD_StrNCpy(pszHWCode, szHwCode, OAPI_CAS_NAGRA_CLIENT_VERSION_LENG - 1);

	return BPL_STATUS_OK;
}
BPLStatus OPL_LocalSystem_NagraBoxFusingInfo(BPL_BOOL *IsSCS, BPL_BOOL *IsJTAG, BPL_BOOL *IsOTP, BPL_BOOL *IsCWE)
{
	HUINT8 ucSCS = 0;
	HUINT8 ucJTAG = 0;
	HUINT8 ucOTP = 0;
	HUINT8 ucCWE = 0;

	CHECK_BPL_PARAM_PTR(IsSCS);
	CHECK_BPL_PARAM_PTR(IsJTAG);
	CHECK_BPL_PARAM_PTR(IsOTP);
	CHECK_BPL_PARAM_PTR(IsCWE);

	APK_CAS_NAGRA_GetFusingInfo(&ucSCS, &ucJTAG, &ucOTP, &ucCWE);
	*IsSCS = (BPL_BOOL)ucSCS ? TRUE : FALSE;
	*IsJTAG = (BPL_BOOL)ucJTAG? TRUE : FALSE;
	*IsOTP = (BPL_BOOL)ucOTP? TRUE : FALSE;
	*IsCWE = (BPL_BOOL)ucCWE? TRUE : FALSE;

	return BPL_STATUS_OK;
}
#define	___SignalInfo_Related_Functions____
BPLStatus	OPL_SignalInfo_GetStrength(unsigned int ulDevIdx, int *pnStrength)
{
	CHECK_BPL_PARAM_PTR(pnStrength);

	if (APK_SYSINFO_TUNER_GetSignalLevel(ulDevIdx, pnStrength) == ERR_OK)
		return BPL_STATUS_OK;

	return BPL_STATUS_ERROR;
}

BPLStatus	OPL_SignalInfo_GetQuality (unsigned int ulDevIdx, int *pnQuality)
{
	CHECK_BPL_PARAM_PTR(pnQuality);

	if (APK_SYSINFO_TUNER_GetSignalQuality(ulDevIdx, pnQuality) == ERR_OK)
		return BPL_STATUS_OK;

	return BPL_STATUS_ERROR;
}

BPLStatus	OPL_SignalInfo_GetBer(unsigned int ulDevIdx, float *pfBer)
{
	HFLOAT32	fBer;

	CHECK_BPL_PARAM_PTR(pfBer);

	if (APK_SYSINFO_TUNER_GetBer(ulDevIdx, &fBer) == ERR_OK)
	{
		*pfBer = fBer;

		return BPL_STATUS_OK;
	}

	return BPL_STATUS_ERROR;
}

BPLStatus	OPL_SignalInfo_GetSnr(unsigned int ulDevIdx, float *pfSnr)
{
	HFLOAT32	fSnr;

	CHECK_BPL_PARAM_PTR(pfSnr);

	if (APK_SYSINFO_TUNER_GetSnr(ulDevIdx, &fSnr) == ERR_OK)
	{
		*pfSnr = fSnr;

		return BPL_STATUS_OK;
	}

	return BPL_STATUS_ERROR;
}

BPLStatus	OPL_SignalInfo_GetAgc(unsigned int ulDevIdx, unsigned int *pulAgc)
{
	HINT32 agc;

	CHECK_BPL_PARAM_PTR(pulAgc);

	if (APK_SYSINFO_TUNER_GetAgc(ulDevIdx, &agc) == ERR_OK)
	{
		*pulAgc = (unsigned int)agc;
		return BPL_STATUS_OK;
	}

	return BPL_STATUS_ERROR;
}

BPLStatus	OPL_SignalInfo_GetLocked(unsigned int ulDevIdx, BPL_BOOL *pbLocked)
{
	HBOOL	locked;

	CHECK_BPL_PARAM_PTR(pbLocked);

	if (APK_SYSINFO_TUNER_IsLocked(ulDevIdx, &locked) == ERR_OK)
	{
		*pbLocked = (BPL_BOOL)locked;

		return BPL_STATUS_OK;
	}

	return BPL_STATUS_ERROR;
}

const HCHAR *local_GETSTR_CodeRate(DxSat_CodeRate_e eCodeRate)
{
	switch(eCodeRate)
	{
		case eDxSAT_CODERATE_AUTO: return "AUTO"; break;
		case eDxSAT_CODERATE_NONE: return "NONE"; break;
		case eDxSAT_CODERATE_1_2: return "1/2"; break;
		case eDxSAT_CODERATE_2_3: return "2/3"; break;
		case eDxSAT_CODERATE_3_4: return "3/4"; break;
		case eDxSAT_CODERATE_5_6: return "5/6"; break;
		case eDxSAT_CODERATE_7_8: return "7/8"; break;
		case eDxSAT_CODERATE_3_5: return "3/5"; break;
		case eDxSAT_CODERATE_4_5: return "4/5"; break;
		case eDxSAT_CODERATE_5_11: return "5/11"; break;
		case eDxSAT_CODERATE_6_7: return "6/7"; break;
		case eDxSAT_CODERATE_8_9: return "8/9"; break;
		case eDxSAT_CODERATE_9_10: return "9/10"; break;
		default: break;
	}

	return ("Unknown");
}

const HCHAR *local_GETSTR_Polar(DxSat_Polarization_e ePolar)
{
	switch(ePolar)
	{
		case eDxSAT_POLAR_HOR: return "H"; break;
		case eDxSAT_POLAR_VER: return "V"; break;
		case eDxSAT_POLAR_LEFT: return "L"; break;
		case eDxSAT_POLAR_RIGHT: return "R"; break;
		default: break;
	}

	return ("AUTO");
}

const HCHAR *local_GETSTR_BandWidth(DxTer_BandWidth_e eBandWidth)
{
	switch(eBandWidth)
	{
		case eDxTER_BANDWIDTH_8M: return "8MHz"; break;
		case eDxTER_BANDWIDTH_7M: return "7MHz"; break;
		case eDxTER_BANDWIDTH_6M: return "6MHz"; break;
		case eDxTER_T2BANDWIDTH_5MHZ: return "5MHz"; break;
		case eDxTER_T2BANDWIDTH_10MHZ: return "10MHz"; break;
		case eDxTER_T2BANDWIDTH_1P7MHZ: return "1P7MHz"; break;
		default: break;
	}

	return ("7MHz");
}

const HCHAR *local_GETSTR_TransMode(DxTer_TransMode_e eTransMode)
{
	switch(eTransMode)
	{
		case eDxTER_TRANSMODE_AUTO: return "AUTO"; break;
		case eDxTER_TRANSMODE_2K: return "2K"; break;
		case eDxTER_TRANSMODE_8K: return "8K"; break;
		case eDxTER_T2TRANSMODE_1K: return "1K"; break;
		case eDxTER_T2TRANSMODE_4K: return "4K"; break;
		case eDxTER_T2TRANSMODE_16K: return "16K"; break;
		case eDxTER_T2TRANSMODE_32K: return "32K"; break;
		default: break;
	}

	return ("AUTO");
}

const HCHAR *local_GETSTR_GuardInterval(DxTer_GuardInterval_e eGuardInterval)
{
	switch(eGuardInterval)
	{
		case eDxTER_GUARDINTERVAL_AUTO: return "AUTO"; break;
		case eDxTER_GUARDINTERVAL_1_4: return "1/4"; break;
		case eDxTER_GUARDINTERVAL_1_8: return "1/8"; break;
		case eDxTER_GUARDINTERVAL_1_16: return "1/16"; break;
		case eDxTER_GUARDINTERVAL_1_32: return "1/32"; break;
		case eDxTER_T2GUARDINTERVAL_1_128: return "1/128"; break;
		case eDxTER_T2GUARDINTERVAL_19_128: return "19/128"; break;
		case eDxTER_T2GUARDINTERVAL_19_256: return "19/256"; break;
		default: break;
	}

	return ("AUTO");
}

const HCHAR *local_GETSTR_Constellation(DxCab_Constellation_e eConstellation)
{
	switch(eConstellation)
	{
		case eDxCAB_CONSTELLATION_AUTO: return "AUTO"; break;
		case eDxCAB_CONSTELLATION_16QAM: return "QAM16"; break;
		case eDxCAB_CONSTELLATION_32QAM: return "QAM32"; break;
		case eDxCAB_CONSTELLATION_64QAM: return "QAM64"; break;
		case eDxCAB_CONSTELLATION_128QAM: return "QAM128"; break;
		case eDxCAB_CONSTELLATION_256QAM: return "QAM256"; break;
		case eDxCAB_CONSTELLATION_1024QAM: return "QAM1024"; break;
		case eDxCAB_CONSTELLATION_4096QAM: return "QAM4096"; break;
		default: break;
	}

	return ("AUTO");
}


BPLStatus	OPL_SignalInfo_GetTunedParamStr(unsigned int ulDevIdx, char *pszTuneParam, int maxStrLen)
{
	DxTuneParam_t	stTuneParam;
	DxAntInfo_t		*pstAntInfo;

	CHECK_BPL_PARAM_PTR(pszTuneParam);

	// temporary....
	pszTuneParam[0] = '\0';

//	APK_SQCCHANNEL_Refresh(ulDevIdx);
	if (APK_SYSINFO_TUNER_GetTuneParam(ulDevIdx, &stTuneParam) == ERR_OK)
	{
		if (eDxDELIVERY_TYPE_SAT == stTuneParam.eDeliType)
		{

			pstAntInfo = APK_META_ANTENNA_GetInfo(stTuneParam.sat.tuningInfo.antuid);
			if (pstAntInfo)
			{
			#if 0
				fflush(NULL);
				printf("\n[%s:%d] dump tuner infor\n",__FUNCTION__,__LINE__);fflush(NULL);
				printf("  pstAntInfo->satName:[%s]\n",pstAntInfo->satName);fflush(NULL);


				printf("\t deliveryType : [%d]\n",stTuneParam.sat.deliveryType);fflush(NULL);
				printf("\t ulTunerGroupId : [%d]\n",stTuneParam.sat.ulTunerGroupId);fflush(NULL);
				printf("\t antennaType : [%d]\n",stTuneParam.sat.antennaType);fflush(NULL);

				printf("\t frequency : [%d]\n",stTuneParam.sat.tuningInfo.ulFrequency);fflush(NULL);
				printf("\t ulSymbolRate : [%d]\n",stTuneParam.sat.tuningInfo.ulSymbolRate);fflush(NULL);
				printf("\t fec : [%d]\n",stTuneParam.sat.tuningInfo.fecCodeRate);fflush(NULL);
				printf("\t polarization : [%d]\n",stTuneParam.sat.tuningInfo.polarization);fflush(NULL);
				printf("\t transSpec : [%d]\n",stTuneParam.sat.tuningInfo.transSpec);fflush(NULL);
				printf("\t pskMod : [%d]\n",stTuneParam.sat.tuningInfo.pskMod);fflush(NULL);
				printf("\t rollOff : [%d]\n",stTuneParam.sat.tuningInfo.rollOff);fflush(NULL);
				printf("\t antuid : [%d]\n",stTuneParam.sat.tuningInfo.antuid);fflush(NULL);
				printf("\t satType : [%s]\n",stTuneParam.sat.tuningInfo.satType);fflush(NULL);
			#endif


				HxSTD_snprintf(pszTuneParam,maxStrLen,"%s, %dMhz, %s, %d, %s",
						pstAntInfo->satName,
						stTuneParam.sat.tuningInfo.ulFrequency,
						local_GETSTR_Polar(stTuneParam.sat.tuningInfo.ePolarization),
						stTuneParam.sat.tuningInfo.ulSymbolRate,
						local_GETSTR_CodeRate(stTuneParam.sat.tuningInfo.eFecCodeRate));


					APK_META_ANTENNA_InfoFree(pstAntInfo);
			}
			else
			{
				HxSTD_snprintf(pszTuneParam,maxStrLen,"%s, %dMhz, %s, %d, %s",
						"Unknown",
						stTuneParam.sat.tuningInfo.ulFrequency,
						local_GETSTR_Polar(stTuneParam.sat.tuningInfo.ePolarization),
						stTuneParam.sat.tuningInfo.ulSymbolRate,
						local_GETSTR_CodeRate(stTuneParam.sat.tuningInfo.eFecCodeRate));

			}

#if 0
	fflush(NULL);
	printf("\n[%s:%d] dump tuner infor\n",__FUNCTION__,__LINE__);fflush(NULL);

	printf("\t deliveryType : [%d]\n",stTuneParam.sat.deliveryType);fflush(NULL);
	printf("\t ulTunerGroupId : [%d]\n",stTuneParam.sat.ulTunerGroupId);fflush(NULL);
	printf("\t antennaType : [%d]\n",stTuneParam.sat.antennaType);fflush(NULL);

	printf("\t frequency : [%d]\n",stTuneParam.sat.tuningInfo.ulFrequency);fflush(NULL);
	printf("\t ulSymbolRate : [%d]\n",stTuneParam.sat.tuningInfo.ulSymbolRate);fflush(NULL);
	printf("\t fec : [%d]\n",stTuneParam.sat.tuningInfo.fecCodeRate);fflush(NULL);
	printf("\t polarization : [%d]\n",stTuneParam.sat.tuningInfo.polarization);fflush(NULL);
	printf("\t transSpec : [%d]\n",stTuneParam.sat.tuningInfo.transSpec);fflush(NULL);
	printf("\t pskMod : [%d]\n",stTuneParam.sat.tuningInfo.pskMod);fflush(NULL);
	printf("\t rollOff : [%d]\n",stTuneParam.sat.tuningInfo.rollOff);fflush(NULL);
	printf("\t antuid : [%d]\n",stTuneParam.sat.tuningInfo.antuid);fflush(NULL);
	printf("\t satType : [%s]\n",stTuneParam.sat.tuningInfo.satType);fflush(NULL);
#endif
		}
		else if (eDxDELIVERY_TYPE_TER == stTuneParam.eDeliType)
		{
			HxSTD_snprintf(pszTuneParam,maxStrLen,"%d,%s,%s,%s",
						stTuneParam.ter.ulFrequency,
						local_GETSTR_BandWidth(stTuneParam.ter.eBandWidth),
						local_GETSTR_TransMode(stTuneParam.ter.eTransMode),
						local_GETSTR_GuardInterval(stTuneParam.ter.eGuardInterval));

		}
		else if (eDxDELIVERY_TYPE_CAB == stTuneParam.eDeliType)
		{
			HxSTD_snprintf(pszTuneParam,maxStrLen,"%d,%s,%d",
						stTuneParam.cab.ulFrequency,
						local_GETSTR_Constellation(stTuneParam.cab.constellation),
						stTuneParam.cab.ulSymbolRate);

		}
		return BPL_STATUS_OK;
	}

	return BPL_STATUS_ERROR;
}

#define	___Tuner_Related_Functions____
BPLStatus	OPL_Tuner_GetNumOfDev(unsigned int *pnNumOfDev)
{
	CHECK_BPL_PARAM_PTR(pnNumOfDev);

	APK_SYSINFO_TUNER_GetDeviceNum(pnNumOfDev);

	return BPL_STATUS_OK;
}

BPLStatus	OPL_Tuner_GetTunerId(unsigned int ulDevIdx, int *pnTunerId)
{
//	HERROR		eRet;

	CHECK_BPL_PARAM_PTR(pnTunerId);

	*pnTunerId = ulDevIdx;

	return BPL_STATUS_OK;
}

BPLStatus	OPL_Tuner_GetPowerState(unsigned int ulDevIdx, BPL_BOOL *pbPowerState)
{
	CHECK_BPL_PARAM_PTR(pbPowerState);

	*pbPowerState = FALSE;

	return BPL_STATUS_ERROR;
}

BPLStatus	OPL_Tuner_GetIdTypes(unsigned int ulDevIdx, int *count, char ***pszIdTypes)
{
	DxDeliveryType_e	eTunerType;
	char		**myStrings = NULL;

	CHECK_BPL_PARAM_PTR(pszIdTypes);
	CHECK_BPL_PARAM_PTR(count);

	if (APK_SYSINFO_TUNER_GetDevType(ulDevIdx, &eTunerType) == ERR_OK)
	{
		int	no_of_id_type;

		no_of_id_type = 1;
		*count = no_of_id_type;
		myStrings = (char**)HLIB_STD_MemAlloc(sizeof(char*) * no_of_id_type);

		switch (eTunerType)
		{
			case	eDxDELIVERY_TYPE_SAT:
				myStrings[0] = HLIB_STD_StrDup("ID_DVB_S2");
				break;
			case	eDxDELIVERY_TYPE_CAB:
				myStrings[0] = HLIB_STD_StrDup("ID_DVB_C2");
				break;
			case	eDxDELIVERY_TYPE_TER:
				myStrings[0] = HLIB_STD_StrDup("ID_DVB_T2");
				break;
			case	eDxDELIVERY_TYPE_MULTICAST:
				myStrings[0] = HLIB_STD_StrDup("ID_IPTV_URI");
				break;
			default:
				HLIB_STD_MemFree(myStrings); myStrings = NULL;
				return BPL_STATUS_ERROR;
		}
		*pszIdTypes = myStrings;

		return BPL_STATUS_OK;
	}

	return BPL_STATUS_ERROR;
}

BPLStatus OPL_Tuner_GetTunerIdByTripleId(int onId, int tsId, int svcId, int *pnTunerId)
{
	CHECK_BPL_PARAM_PTR(pnTunerId);
	if (APK_SYSINFO_TUNER_GetTunerIdByTripleId(onId, tsId, svcId, pnTunerId) == ERR_OK)
	{
		return BPL_STATUS_OK;
	}
	return BPL_STATUS_ERROR;
}


#define ____HX_Output_Related_Functions____

BPLStatus OPL_HMXOutput_GetNumOfDev(unsigned int *pnNumOfDev)
{
//	HERROR		nRet;
	HUINT32 	ulDevNum;

	CHECK_BPL_PARAM_PTR(pnNumOfDev);

#if 0
	nRet = APK_SQCOUTPUT_GetNumOfDevice((HUINT32*)pnNumOfDev);
	if (ERR_OK != nRet)
	{
		HxLOG_Error("APK_SQCOUTPUT_GetNumOfDevice ret Error:0x%X.\n",nRet);
		return BPL_STATUS_ERROR;
	}
#else
	ulDevNum = 1;
#endif
	*pnNumOfDev = ulDevNum;
	return BPL_STATUS_OK;
}

BPLStatus OPL_HMXOutput_IsSupportScart(unsigned int ulDevIdx, BPL_BOOL *pIsExist)
{
	HERROR	err;
	HBOOL exist;

	CHECK_BPL_PARAM_PTR(pIsExist);

	err = APK_OUTPUT_VIDEO_SupportTvScart(&exist);
	*pIsExist = (BPL_BOOL)exist;

	if(err == ERR_OK)
	{
		return BPL_STATUS_OK;
	}
	else
	{
		return BPL_STATUS_ERROR;
	}
}


BPLStatus OPL_HMXOutput_IsSupportTvScart(unsigned int ulDevIdx, BPL_BOOL *pIsExist)
{
	HERROR	err;
	HBOOL exist;

	CHECK_BPL_PARAM_PTR(pIsExist);

	err = APK_OUTPUT_VIDEO_SupportTvScart(&exist);
	*pIsExist = (BPL_BOOL)exist;

	if(err == ERR_OK)
	{
		return BPL_STATUS_OK;
	}
	else
	{
		return BPL_STATUS_ERROR;
	}
}

BPLStatus OPL_HMXOutput_IsSupportVcrScart(unsigned int ulDevIdx, BPL_BOOL *pIsExist)
{
	HERROR	err;
	HBOOL exist;

	CHECK_BPL_PARAM_PTR(pIsExist);

	err = APK_OUTPUT_VIDEO_SupportVcrScart(&exist);
	*pIsExist = (BPL_BOOL)exist;

	if(err == ERR_OK)
	{
		return BPL_STATUS_OK;
	}
	else
	{
		return BPL_STATUS_ERROR;
	}
}

BPLStatus OPL_HMXOutput_IsSupportComposite(unsigned int ulDevIdx, BPL_BOOL *pIsExist)
{
	HERROR	err;
	HBOOL exist;

	CHECK_BPL_PARAM_PTR(pIsExist);

	err = APK_OUTPUT_VIDEO_SupportComposite(&exist);
	*pIsExist = (BPL_BOOL)exist;

	if(err == ERR_OK)
	{
		return BPL_STATUS_OK;
	}
	else
	{
		return BPL_STATUS_ERROR;
	}
}

BPLStatus OPL_HMXOutput_IsSupportComponent(unsigned int ulDevIdx, BPL_BOOL *pIsExist)
{
	CHECK_BPL_PARAM_PTR(pIsExist);

	*pIsExist = FALSE;

	return BPL_STATUS_OK;
}

BPLStatus OPL_HMXOutput_IsSupportRF(unsigned int ulDevIdx, BPL_BOOL *pIsExist)
{
	CHECK_BPL_PARAM_PTR(pIsExist);

	*pIsExist = FALSE;

	return BPL_STATUS_OK;
}

BPLStatus OPL_HMXOutput_IsSupportHdmi(unsigned int ulDevIdx, BPL_BOOL *pIsExist)
{
	CHECK_BPL_PARAM_PTR(pIsExist);

	*pIsExist = TRUE;

	return BPL_STATUS_OK;
}

BPLStatus OPL_HMXOutput_IsSupportSpdif(unsigned int ulDevIdx, BPL_BOOL *pIsExist)
{
	HERROR		err;
	HBOOL exist;

	CHECK_BPL_PARAM_PTR(pIsExist);

	err = APK_OUTPUT_AUDIO_SupportSpdif(&exist);
	*pIsExist = (BPL_BOOL)exist;

	if(err == ERR_OK)
	{
		return BPL_STATUS_OK;
	}
	else
	{
		return BPL_STATUS_ERROR;
	}
}

static	EnumStringPair_t	_astScartPairList[] =
{
	{eDxSCART_FORMAT_UNKNOWN,	"Unknown"},
	{eDxSCART_FORMAT_CVBS,		"CVBS"},
	{eDxSCART_FORMAT_RGB,		"RGB"},
	{eDxSCART_FORMAT_SVIDEO,	"S-Video"},
	{eDxSCART_FORMAT_UNKNOWN, 	NULL}
};

BPLStatus OPL_HMXOutput_GetCurTVScartFormat(unsigned int ulDevIdx, char *pszTvScartFormat)
{
	HCHAR				*data;
	DxScartFormat_e		eScartFormat;

	CHECK_BPL_PARAM_PTR(pszTvScartFormat);

	APK_OUTPUT_VIDEO_GetTvScartFormat(&eScartFormat);

	data = OPL_UTIL_GetStringFromValue(_astScartPairList, eScartFormat);

	HxSTD_StrNCpy(pszTvScartFormat, data, 31);

	return BPL_STATUS_OK;
}

BPLStatus OPL_HMXOutput_SetCurTVScartFormat(unsigned int ulDevIdx, const char *pszTvScartFormat)
{
	DxScartFormat_e		eScartFormat;

	CHECK_BPL_PARAM_PTR(pszTvScartFormat);

	eScartFormat = (DxScartFormat_e)OPL_UTIL_GetValueFromString(_astScartPairList, pszTvScartFormat);
	if (APK_OUTPUT_VIDEO_SetTvScartFormat(eScartFormat) != ERR_OK)
	{
		HxLOG_Error("APK_OUTPUT_VIDEO_SetTvScartFormat ret Error\n");
		return BPL_STATUS_ERROR;
	}

	return BPL_STATUS_OK;
}

#define MAX_SCART_FORMAT	8
//static char		*s_pucSupportScartFormat[MAX_SCART_FORMAT];
BPLStatus OPL_HMXOutput_GetSupportedTVScartFormat(unsigned int ulDevIdx, int *count, char ***pszTvScartFormat)
{
	HERROR			nRet;
	HBOOL			bSupportScart;
	DxScartFormat_e	eScartFormat;
	HUINT32			ulExistCnt = 0, ulCollectionCnt = 0;
	char			**ppszCollection = NULL;
	DxScartFormat_b ulSupportedScartFormat;
	HUINT32			idx;

	if ((NULL == count) || (NULL == pszTvScartFormat))
		return BPL_STATUS_ERROR;

	*pszTvScartFormat = NULL;
	*count = 0;

	APK_OUTPUT_VIDEO_SupportTvScart(&bSupportScart);
	if (bSupportScart == FALSE)
	{
		return BPL_STATUS_OK;
	}

	nRet = APK_OUTPUT_VIDEO_GetSupportTvScartFormat(&ulSupportedScartFormat);
	if (ERR_OK != nRet)
	{
		HxLOG_Error(" APK_OUTPUT_VIDEO_GetSupportTvScartFormat ret Error:0x%X\n",nRet);
		return BPL_STATUS_OK;
	}
	HxLOG_Print(" eScartFormat:0x%X\n",ulSupportedScartFormat);

	for(idx = 0; idx < MAX_SCART_FORMAT; idx++)
	{
		eScartFormat = ulSupportedScartFormat & (0x1 << idx);
		if(eScartFormat)
		{
			ulExistCnt++;
		}
	}

	if(ulExistCnt == 0)
	{
		return BPL_STATUS_OK;
	}
	else
	{
		*count = ulExistCnt;
		ppszCollection = (char**)HLIB_STD_MemAlloc(sizeof(char*)*ulExistCnt);
	}

	for(idx = 0; idx < MAX_SCART_FORMAT; idx++)
	{
		eScartFormat = ulSupportedScartFormat & (0x1 << idx);
		if(eScartFormat)
		{
			ppszCollection[ulCollectionCnt++] = HLIB_STD_StrDup(OPL_UTIL_GetStringFromValue(_astScartPairList, eScartFormat));
		}

		if(ulCollectionCnt == ulExistCnt)
			break;
	}

	*pszTvScartFormat = ppszCollection;

	return BPL_STATUS_OK;
}

BPLStatus OPL_HMXOutput_GetCurVcrScartFormat(unsigned int ulDevIdx, char *pszVcrScartFormat)
{
	HCHAR				*data;
	DxScartFormat_e		eScartFormat;

	CHECK_BPL_PARAM_PTR(pszVcrScartFormat);

	APK_OUTPUT_VIDEO_GetVcrScartFormat(&eScartFormat);

	data = OPL_UTIL_GetStringFromValue(_astScartPairList, eScartFormat);

	HxSTD_StrNCpy(pszVcrScartFormat, data, 31);

	return BPL_STATUS_OK;
}

BPLStatus OPL_HMXOutput_SetCurVcrScartFormat(unsigned int ulDevIdx, const char *pszVcrScartFormat)
{
	DxScartFormat_e		eScartFormat;

	CHECK_BPL_PARAM_PTR(pszVcrScartFormat);

	eScartFormat = (DxScartFormat_e)OPL_UTIL_GetValueFromString(_astScartPairList, pszVcrScartFormat);
	if (APK_OUTPUT_VIDEO_SetVcrScartFormat(eScartFormat) != ERR_OK)
	{
		HxLOG_Error("APK_OUTPUT_VIDEO_SetVcrScartFormat ret Error\n");
		return BPL_STATUS_ERROR;
	}

	return BPL_STATUS_OK;
}

BPLStatus OPL_HMXOutput_GetSupportedVCRScartFormat(unsigned int ulDevIdx, int *count, char ***pszVcrScartFormat)
{
	HERROR			nRet;
	HBOOL			bSupportScart;
	DxScartFormat_e eScartFormat;
	HUINT32 		ulExistCnt = 0, ulCollectionCnt = 0;
	char			**ppszCollection = NULL;
	DxScartFormat_b ulSupportedScartFormat;
	HUINT32 		idx;

	if ((NULL == count) || (NULL == pszVcrScartFormat))
		return BPL_STATUS_ERROR;

	*pszVcrScartFormat = NULL;
	*count = 0;

	APK_OUTPUT_VIDEO_SupportTvScart(&bSupportScart);
	if (bSupportScart == FALSE)
	{
		return BPL_STATUS_OK;
	}

	nRet = APK_OUTPUT_VIDEO_GetSupportVcrScartFormat(&ulSupportedScartFormat);
	if (ERR_OK != nRet)
	{
		HxLOG_Error(" APK_OUTPUT_VIDEO_GetSupportTvScartFormat ret Error:0x%X\n",nRet);
		return BPL_STATUS_OK;
	}
	HxLOG_Print(" eScartFormat:0x%X\n",ulSupportedScartFormat);

	for(idx = 0; idx < MAX_SCART_FORMAT; idx++)
	{
		eScartFormat = ulSupportedScartFormat & (0x1 << idx);
		if(eScartFormat)
		{
			ulExistCnt++;
		}
	}

	if(ulExistCnt == 0)
	{
		return BPL_STATUS_OK;
	}
	else
	{
		*count = ulExistCnt;
		ppszCollection = (char**)HLIB_STD_MemAlloc(sizeof(char*)*ulExistCnt);
	}

	for(idx = 0; idx < MAX_SCART_FORMAT; idx++)
	{
		eScartFormat = ulSupportedScartFormat & (0x1 << idx);
		if(eScartFormat)
		{
			ppszCollection[ulCollectionCnt++] = HLIB_STD_StrDup(OPL_UTIL_GetStringFromValue(_astScartPairList, eScartFormat));
		}

		if(ulCollectionCnt == ulExistCnt)
			break;
	}

	*pszVcrScartFormat = ppszCollection;

	return BPL_STATUS_OK;
}

static	EnumStringPair_t		_astResolutionSelectPairList[] =
{
	{eDxRESOLUTION_SELECT_480I,				"480i"},
	{eDxRESOLUTION_SELECT_480P,				"480p"},
	{eDxRESOLUTION_SELECT_576I,				"576i"},
	{eDxRESOLUTION_SELECT_576P,				"576p"},
	{eDxRESOLUTION_SELECT_720P,				"720p"},
	{eDxRESOLUTION_SELECT_1080I,			"1080i"},
	{eDxRESOLUTION_SELECT_1080P,			"1080p"},
	{eDxRESOLUTION_SELECT_2160P,			"2160p"},
	{eDxRESOLUTION_SELECT_ORIGINAL,			"Original"},
	{eDxRESOLUTION_SELECT_EDID_MAX,			"Native"},
	{eDxRESOLUTION_SELECT_UNKNOWN,			NULL}
};

BPLStatus OPL_HMXOutput_GetResolutionSelect(unsigned int ulDevIdx, char *pszResolutionSelect, int nMaxStrlen)
{
	HCHAR					*data;
	//DxResolutionStatus_e	eResolutionSelect;
	DxResolutionSelect_e	eResolutionSelect;

	APK_OUTPUT_VIDEO_GetResolutionSelect(&eResolutionSelect);

	HxLOG_Print(" eResolution:0x%X\n", eResolutionSelect);

	data = OPL_UTIL_GetStringFromValue(_astResolutionSelectPairList, eResolutionSelect);
	HxSTD_StrNCpy(pszResolutionSelect, data, (nMaxStrlen-1));
	pszResolutionSelect[(nMaxStrlen-1)] = '\0';

	return BPL_STATUS_OK;
}

BPLStatus OPL_HMXOutput_SetResolutionSelect(unsigned int ulDevIdx, const char *pszResolutionSelect)
{
	HERROR					nRet;
	DxResolutionStatus_e	eResolutionSelect;

	CHECK_BPL_PARAM_PTR(pszResolutionSelect);

	eResolutionSelect = OPL_UTIL_GetValueFromString(_astResolutionSelectPairList, pszResolutionSelect);

	HxLOG_Print(" pszResolution:%s eResolution:0x%X\n",pszResolutionSelect, eResolutionSelect);

	nRet = APK_OUTPUT_VIDEO_SetResolutionSelect(eResolutionSelect);
	if (ERR_OK != nRet)
	{
		HxLOG_Error("APK_OUTPUT_VIDEO_SetResolutionSelect ret Error:0x%X.\n",nRet);
		return BPL_STATUS_ERROR;
	}

	return BPL_STATUS_OK;
}

#if 0
static	EnumStringPair_t		_astResolutionStatusPairList[] =
{
	{eDxRESOLUTION_STATUS_480I,				"480i"},
	{eDxRESOLUTION_STATUS_480P,				"480p"},
	{eDxRESOLUTION_STATUS_576I,				"576i"},
	{eDxRESOLUTION_STATUS_576P,				"576p"},
	{eDxRESOLUTION_STATUS_720P,				"720p"},
	{eDxRESOLUTION_STATUS_1080I,			"1080i"},
	{eDxRESOLUTION_STATUS_1080P,			"1080p"},
	{eDxRESOLUTION_STATUS_UNKNOWN,			NULL}
};
#endif

#define MAX_RESOLUTION	16
BPLStatus OPL_HMXOutput_GetSupportedResolutions(unsigned int ulDevIdx, int *count, char ***pszResolutions)
{
	HUINT32					idx;
	DxResolutionStatus_e	eResolution;
	HUINT32					ulExistCnt = 0, ulCollectionCnt = 0;
	char					**ppszCollection = NULL;
	DxResolutionStatus_b	ulResolutionCapability;

	if ((NULL == count) || (NULL == pszResolutions))
		return BPL_STATUS_ERROR;

	*pszResolutions = NULL;
	*count = 0;

	APK_OUTPUT_HDMI_GetEdidResolutionCapability(&ulResolutionCapability);

	for(idx = 0; idx < MAX_RESOLUTION; idx++)
	{
		if(ulResolutionCapability & (0x1 << idx))
		{
			ulExistCnt++;
		}
	}

	if(ulExistCnt == 0)
	{
		return BPL_STATUS_OK;
	}
	else
	{
		*count = ulExistCnt;
		ppszCollection = (char **)HLIB_STD_MemAlloc(sizeof(char*) * ulExistCnt);
	}

	for(idx = 0; idx < MAX_RESOLUTION; idx++)
	{
		eResolution = ulResolutionCapability & (0x1 << idx);
		if(eResolution)
		{
			switch(eResolution)
			{
				case eDxRESOLUTION_STATUS_480I:
					ppszCollection[ulCollectionCnt++] = HLIB_STD_StrDup(OPL_UTIL_GetStringFromValue(_astResolutionSelectPairList, eDxRESOLUTION_SELECT_480I));
					break;

				case eDxRESOLUTION_STATUS_576I:
					ppszCollection[ulCollectionCnt++] = HLIB_STD_StrDup(OPL_UTIL_GetStringFromValue(_astResolutionSelectPairList, eDxRESOLUTION_SELECT_576I));
					break;

				case eDxRESOLUTION_STATUS_480P:
					ppszCollection[ulCollectionCnt++] = HLIB_STD_StrDup(OPL_UTIL_GetStringFromValue(_astResolutionSelectPairList, eDxRESOLUTION_SELECT_480P));
					break;

				case eDxRESOLUTION_STATUS_576P:
					ppszCollection[ulCollectionCnt++] = HLIB_STD_StrDup(OPL_UTIL_GetStringFromValue(_astResolutionSelectPairList, eDxRESOLUTION_SELECT_576P));
					break;

				case eDxRESOLUTION_STATUS_720P:
					ppszCollection[ulCollectionCnt++] = HLIB_STD_StrDup(OPL_UTIL_GetStringFromValue(_astResolutionSelectPairList, eDxRESOLUTION_SELECT_720P));
					break;

				case eDxRESOLUTION_STATUS_1080I:
					ppszCollection[ulCollectionCnt++] = HLIB_STD_StrDup(OPL_UTIL_GetStringFromValue(_astResolutionSelectPairList, eDxRESOLUTION_SELECT_1080I));
					break;

				case eDxRESOLUTION_STATUS_1080P:
					ppszCollection[ulCollectionCnt++] = HLIB_STD_StrDup(OPL_UTIL_GetStringFromValue(_astResolutionSelectPairList, eDxRESOLUTION_SELECT_1080P));
					break;

				case eDxRESOLUTION_STATUS_2160P:
					ppszCollection[ulCollectionCnt++] = HLIB_STD_StrDup(OPL_UTIL_GetStringFromValue(_astResolutionSelectPairList, eDxRESOLUTION_SELECT_2160P));
					break;

				default:
					break;
			}
		}

		if(ulCollectionCnt == ulExistCnt)
			break;
	}

	*pszResolutions = ppszCollection;

	return BPL_STATUS_OK;
}

static	EnumStringPair_t		_astAspectRatioPairList[] =
{
	{	eDxASPECTRATIO_4X3,			"4:3"		},
	{	eDxASPECTRATIO_16X9,		"16:9"		},
	{	eDxASPECTRATIO_14X9,		"14:9"		},
	{	eDxASPECTRATIO_UNKNOWN,		NULL		}
};

BPLStatus OPL_HMXOutput_GetTvAspectRatio(unsigned int ulDevIdx, char *pszTvAspectRatio)
{
	HCHAR				*data;
	DxAspectRatio_e		eTVAspectRatio;

	CHECK_BPL_PARAM_PTR(pszTvAspectRatio);

	APK_OUTPUT_VIDEO_GetTvAspectRatio((DxAspectRatio_e *)&eTVAspectRatio);

	HxLOG_Print(" eTVAspectRatio:0x%X\n",eTVAspectRatio);

	data = OPL_UTIL_GetStringFromValue(_astAspectRatioPairList, eTVAspectRatio);
	HxSTD_StrNCpy(pszTvAspectRatio, data, 31);

	return BPL_STATUS_OK;
}

BPLStatus OPL_HMXOutput_SetTvAspectRatio(unsigned int ulDevIdx, const char *pszTvAspectRatio)
{
	HERROR				nRet;
	DxAspectRatio_e		eTVAspectRatio;

	CHECK_BPL_PARAM_PTR(pszTvAspectRatio);

	eTVAspectRatio = OPL_UTIL_GetValueFromString(_astAspectRatioPairList, pszTvAspectRatio);

	HxLOG_Print(" pszTvAspectRatio:%s eTVAspectRatio:0x%X\n",pszTvAspectRatio, eTVAspectRatio);

	nRet = APK_OUTPUT_VIDEO_SetTvAspectRatio(eTVAspectRatio);
	if (ERR_OK != nRet)
	{
		HxLOG_Error("APK_OUTPUT_VIDEO_SetTvAspectRatio ret Error:0x%X.\n",nRet);
		return BPL_STATUS_ERROR;
	}
	return BPL_STATUS_OK;
}

/**************************************************************************************/
/**
# wide_signal : DFC º¯°æ ¾øÀÌ avi_info, wss, cgms µîÀ» ÀÌ¿ëÇÏ¿© TV¿¡°Ô º¯°æÇÏµµ·Ï ÇÏ´Â ¸ðµå
# Sqeeze_14x9 : Sqeeze¿Í °°Áö¸¸, LB(LetterBox), PB(PillarBox) ¿µ¿ªÀÌ 1/2·Î ÁÙ¾îµë. (DTG Spec)
# CenterCutOut_14x9 : CenterCutOut¿Í °°Áö¸¸, LB(LetterBox), PB(PillarBox) ¿µ¿ªÀÌ 1/2·Î ÁÙ¾îµë. (DTG Spec)

				===================================================
				   16:9 TV 		:	oipf 			:	4:3 TV
				  ---------------------------------------------------
				  	Auto 		:  	'Auto' 			:	Auto
				  ---------------------------------------------------
				  	Pillarbox 	: 	'Sqeeze'		:	Letterbox
				  ---------------------------------------------------
				  	Zoom 		: 	'CenterCutOut'  : 	Centre
				===================================================
**/
/**************************************************************************************/

static	EnumStringPair_t		_astDfcPairList[] =
{
	{eDxDFC_SELECT_DEFAULT,			"Auto"},
	{eDxDFC_SELECT_WIDESIGNAL,		"wide_signal"},
	{eDxDFC_SELECT_SQUEEZE,			"Sqeeze"},
	{eDxDFC_SELECT_SQUEEZE149,		"Sqeeze_14x9"},
	{eDxDFC_SELECT_CENTERCUTOUT,	"CenterCutOut"},
	{eDxDFC_SELECT_CENTERCUTOUT149,	"CenterCutOut_14x9"},
	{eDxDFC_SELECT_FULLSCREEN,		"Anamorphic"},
	{eDxDFC_SELECT_UNKNOWN,			NULL}
};

BPLStatus OPL_HMXOutput_GetDisplayFormat(unsigned int ulDevIdx, char *pszDisplayFormat)
{
	HCHAR					*data;
	DxDfcSelect_e			eDfc;

	CHECK_BPL_PARAM_PTR(pszDisplayFormat);

	APK_OUTPUT_VIDEO_GetDfcSelect((DxDfcSelect_e *)&eDfc);

	data = OPL_UTIL_GetStringFromValue(_astDfcPairList, eDfc);
	HxSTD_StrNCpy(pszDisplayFormat, data, 31);

	HxLOG_Print(" eDispFormat:0x%X[%s]\n",eDfc,pszDisplayFormat);

	return BPL_STATUS_OK;
}

BPLStatus OPL_HMXOutput_SetDisplayFormat(unsigned int ulDevIdx, const char *pszDisplayFormat)
{
	HERROR			nRet;
	DxDfcSelect_e	eDfcSelect;

	CHECK_BPL_PARAM_PTR(pszDisplayFormat);

	eDfcSelect = OPL_UTIL_GetValueFromString(_astDfcPairList, pszDisplayFormat);

	HxLOG_Print(" pszDisplayFormat:%s eDispFormat:0x%X\n",pszDisplayFormat,eDfcSelect);

	nRet = APK_OUTPUT_VIDEO_SetDfcSelect(eDfcSelect);
	if (ERR_OK != nRet)
	{
		HxLOG_Error("APK_OUTPUT_VIDEO_SetDfcSelect ret Error:0x%X.\n",nRet);
		return BPL_STATUS_ERROR;
	}

	return BPL_STATUS_OK;
}

BPLStatus OPL_HMXOutput_GetWss2HdmiMode(unsigned int ulDevIdx, char *pszWss2HdmiMode)
{
	// TODO: OIPF¿¡ ÇÊ¿ä ¾ø´Â Ç×¸ñÀÓ. ¾ø¾ÖÁÖ¼¼¿ä.
	HxSTD_StrNCpy(pszWss2HdmiMode, "None", 31);

	return BPL_STATUS_OK;
}

static	EnumStringPair_t		_astVideoStandardPairList[] =
{
	{	eDxCOMPOSITE_STANDARD_PAL,		"PAL"		},
	{	eDxCOMPOSITE_STANDARD_NTSC,		"NTSC"		},
	{	eDxCOMPOSITE_STANDARD_SECAM,	"SECAM"		},
	{	eDxCOMPOSITE_STANDARD_UNKNOWN,	"Unknown"	},
	{	eDxCOMPOSITE_STANDARD_UNKNOWN,	NULL		}
};

BPLStatus OPL_HMXOutput_GetVideoStandard(unsigned int ulDevIdx, char *pszVideoStandard)
{
	HCHAR					*data;
	DxCompositeStandard_e	eVideoStandard;

	CHECK_BPL_PARAM_PTR(pszVideoStandard);

	APK_OUTPUT_VIDEO_GetCompositeStandard(&eVideoStandard);
	HxLOG_Print(" eVideoStandard:0x%X\n",eVideoStandard);

	data = OPL_UTIL_GetStringFromValue(_astVideoStandardPairList, eVideoStandard);
	HxSTD_StrNCpy(pszVideoStandard, data, 31);

	return BPL_STATUS_OK;
}

BPLStatus OPL_HMXOutput_GetAvAddtionalSignal(unsigned int ulDevIdx, char *pszAvAddtionalSignal)
{
	// TODO: OIPF¿¡ ÇÊ¿ä ¾ø´Â Ç×¸ñÀÓ. ¾ø¾ÖÁÖ¼¼¿ä.
	HxSTD_StrNCpy(pszAvAddtionalSignal, "WSS", 31);

	return BPL_STATUS_OK;
}

BPLStatus OPL_HMXOutput_IsAudioDescriptionEnabled(unsigned int ulDevIdx, BPL_BOOL *pEnabled)
{

	CHECK_BPL_PARAM_PTR(pEnabled);

	APK_DB_GetInt(USERDB_AUDIO_AUDIO_DESCRIPTION, (HINT32*)pEnabled);

	return BPL_STATUS_OK;
}

BPLStatus OPL_HMXOutput_SetAudioDescriptionEnabled(unsigned int ulDevIdx, BPL_BOOL enabled)
{
	HERROR		nRet;
	HBOOL		bEnabled = enabled;

	nRet = APK_OUTPUT_AUDIO_SetAudioDescription(bEnabled);
	if (ERR_OK != nRet)
	{
		HxLOG_Error(" APK_OUTPUT_AUDIO_SetAudioDescription ret Error:0x%X.\n",nRet);
		return BPL_STATUS_ERROR;
	}

	HAPI_BroadcastSignal("signal:onAudioDescriptionState", 1, "i", bEnabled ? 1 : 0);

	APK_DB_SetInt(USERDB_AUDIO_AUDIO_DESCRIPTION, (HUINT32)bEnabled);

	return BPL_STATUS_OK;
}

BPLStatus OPL_HMXOutput_IsSubtitleEnabled(unsigned int ulDevIdx, BPL_BOOL *pEnabled)
{
	CHECK_BPL_PARAM_PTR(pEnabled);

	APK_DB_GetInt(USERDB_SUBTITLE_DISPLAY, (HINT32*)pEnabled);

	return BPL_STATUS_OK;
}

BPLStatus OPL_HMXOutput_SetSubtitleEnabled(unsigned int ulDevIdx, BPL_BOOL enabled)
{
	HERROR		nRet;
	HBOOL		bEnabled = enabled;

	nRet = APK_SCENARIO_SetSubtitleSetting(bEnabled);
	if (ERR_OK != nRet)
	{
		HxLOG_Error(" APK_SCENARIO_SetSubtitleSetting ret Error:0x%X.\n",nRet);
		return BPL_STATUS_ERROR;
	}

	APK_DB_SetInt(USERDB_SUBTITLE_DISPLAY, (HUINT32)bEnabled);

	HAPI_BroadcastSignal("signal:onSubtitleState", 1, "i", bEnabled ? 1 : 0);
	HAPI_BroadcastSignal("signal:onSubtitleStateOplLocalSystem", 1, "i", bEnabled ? 1 : 0); /* MHEG ICS023:MHEG ApplÀ¸·Î º¸³»´Â signal */

	return BPL_STATUS_OK;
}

BPLStatus OPL_HMXOutput_IsHardOfHearingEnabled(unsigned int ulDevIdx, BPL_BOOL *pEnabled)
{
#if 0
	HERROR		nRet;
	HBOOL		bEnabled;
#endif

	CHECK_BPL_PARAM_PTR(pEnabled);

#if 1
	APK_DB_GetInt(USERDB_HARD_OF_HEARING, (HINT32*)pEnabled);
#else
	*pEnabled = 0;

	nRet = APK_SCENARIO_GetHardOfHearing(&bEnabled);
	if (ERR_OK != nRet)
	{
		HxLOG_Error("APK_SCENARIO_GetHardOfHearing ret Error:0x%X.\n",nRet);
		return BPL_STATUS_ERROR;
	}
	*pEnabled = bEnabled;
#endif

	return BPL_STATUS_OK;
}

BPLStatus OPL_HMXOutput_SetHardOfHearingEnabled(unsigned int ulDevIdx, BPL_BOOL enabled)
{
	HERROR		nRet;
	HBOOL		bEnabled = enabled;

//	HxLOG_Error(" bEnabled:%d\n",bEnabled);

	nRet = APK_SCENARIO_SetHardOfHearing(bEnabled);
	if (ERR_OK != nRet)
	{
		HxLOG_Error("APK_SCENARIO_SetHardOfHearing ret Error:0x%X.\n",nRet);
		return BPL_STATUS_ERROR;
	}

	APK_DB_SetInt(USERDB_HARD_OF_HEARING, (HUINT32)bEnabled);

	return BPL_STATUS_OK;
}

#define ____HX_Audio_Related_Functions____

BPLStatus OPL_HMXAudio_GetNumOfDev(unsigned int *pnNumOfDev)
{
//	HERROR		nRet;
	HUINT32		ulDevNum;
#if 0
	nRet = APK_SCK_OUTPUT_GetNumOfDev(APK_SCK_GetHandle(), &ulDevNum);
	if (ERR_OK != nRet)
	{
		HxLOG_Error("APK_SCK_OUTPUT_GetNumOfDev ret Error:0x%X.\n",nRet);
		return BPL_STATUS_ERROR;
	}
#else
#if defined(CONFIG_SUPPORT_PRISM_APP)
	// ¿Ö ÀÌ°ÍÀ» MW¿¡¼­ °®°í ¿ÀÁö ¾Ê°í OPL¿¡¼­ °®°í ÀÖÀ» ±î³ª. À½....ÀÏ´Ü PRISMÀ¸·Î ÅëÇÕ
    /* HMS´Â Audio¸¦ HDMI¿Í SPDIF·Î 2°³·Î ºÐ¸®ÇØ¼­ Àû¿ë */
	ulDevNum = 2;
#else
	ulDevNum = 1;
#endif
#endif
	*pnNumOfDev = ulDevNum;
	return BPL_STATUS_OK;
}

BPLStatus OPL_HMXAudio_GetName(unsigned int ulDevIdx, char* szName)
{
	//HERROR		ulRet = BPL_STATUS_ERROR;
	HUINT32		ulDevNum = 0;

	 OPL_HMXAudio_GetNumOfDev(&ulDevNum);
	if (ulDevNum > 1) {
		snprintf(szName,31,"audio_%s", (ulDevIdx == 0) ? "hdmi" : "spdif");
	} else {
		snprintf(szName,31,"audio_%d", ulDevIdx);
	}

	return BPL_STATUS_OK;
}

BPLStatus OPL_HMXAudio_GetVolumeMax(unsigned int ulDevIdx, int *pVolumeMax)
{
//	HERROR				eRet;

	CHECK_BPL_PARAM_PTR(pVolumeMax);

	if (APK_OUTPUT_AUDIO_GetVolumeMax((unsigned int *)pVolumeMax) != ERR_OK)
	{
		HxLOG_Error("Getting the voulme info is failed.\n");
		return BPL_STATUS_ERROR;
	}

	return BPL_STATUS_OK;
}

BPLStatus OPL_HMXAudio_GetVolumeMin(unsigned int ulDevIdx, int *pVolumeMin)
{
//	HERROR				eRet;

	CHECK_BPL_PARAM_PTR(pVolumeMin);

 	*pVolumeMin = 0;

	return BPL_STATUS_OK;
}

BPLStatus OPL_HMXAudio_GetLipSyncDelay(unsigned int ulDevIdx, int *pnLipSyncDelay)
{
	HUINT32		ulLipSyncDelay;

	CHECK_BPL_PARAM_PTR(pnLipSyncDelay);

#if defined(CONFIG_PROD_HMS1000S) || defined(CONFIG_PROD_HMSLITE)
#define	USERDB_AUDIO_LIPSYNC	"UDB/AUDIO_LipSync"
		if(APK_DB_GetInt(USERDB_AUDIO_LIPSYNC, (HINT32*)&ulLipSyncDelay)== ERR_OK)
		{
			APK_DB_Remove(USERDB_AUDIO_LIPSYNC);
			APK_OUTPUT_AUDIO_SetLipSyncDelay(ulLipSyncDelay);
		}
#undef	USERDB_AUDIO_LIPSYNC
#endif

	if (APK_OUTPUT_AUDIO_GetLipSyncDelay(&ulLipSyncDelay) != ERR_OK)
	{
		HxLOG_Error("Getting the lip sync delay is failed.\n");
		return BPL_STATUS_ERROR;
	}

	*pnLipSyncDelay = ulLipSyncDelay;

	return BPL_STATUS_OK;
}

BPLStatus OPL_HMXAudio_SetLipSyncDelay(unsigned int ulDevIdx, int nLipSyncDelay)
{
	if (APK_OUTPUT_AUDIO_SetLipSyncDelay(nLipSyncDelay) != ERR_OK)
	{
		HxLOG_Error("Setting lip sync delay is failed.\n");
		return BPL_STATUS_ERROR;
	}

	return BPL_STATUS_OK;
}

static	EnumStringPair_t	_SoundModeList[] =
{
		{	eDxSTEREO_SELECT_STEREO,		"Stereo"				},
		{	eDxSTEREO_SELECT_MONOLEFT,		"Left"				},
		{	eDxSTEREO_SELECT_MONORIGHT,		"Right"				},
		{	eDxSTEREO_SELECT_MONOMIXED,		"Mix"				},
		{	eDxSTEREO_SELECT_UNKNOWN,		NULL				}
};
BPLStatus OPL_HMXAudio_GetSoundMode(unsigned int ulDevIdx, char* szSoundMode)
{
	DxStereoSelect_e		eAudioConfig;
	HCHAR					*data;

	CHECK_BPL_PARAM_PTR(szSoundMode);

#if defined(CONFIG_PROD_HMS1000S) || defined(CONFIG_PROD_HMSLITE)
#define	USERDB_AUDIO_SOUND_MODE	"UDB/AUDIO_SoundMode"

	if(APK_DB_GetInt(USERDB_AUDIO_SOUND_MODE, (HINT32*)&eAudioConfig)== ERR_OK)
	{
		APK_DB_Remove(USERDB_AUDIO_SOUND_MODE);
		APK_OUTPUT_AUDIO_SetStereoSelect(eAudioConfig);
	}
#undef	USERDB_AUDIO_SOUND_MODE
#endif
	APK_OUTPUT_AUDIO_GetStereoSelect(&eAudioConfig);

	data = OPL_UTIL_GetStringFromValue(_SoundModeList, eAudioConfig);

	HxLOG_Print("OPL_HMXAudio_GetSoundMode:%s[%d]\n",data, eAudioConfig);
	HxSTD_StrNCpy(szSoundMode, data, 31);

	return BPL_STATUS_OK;
}

#if !defined( CONFIG_OP_JAPAN )
// ÀÏº»ÇâÃ³·³ stereo, dual mono °¡ ui·Î ºÐ¸®µÈ °æ¿ì¸¦ Á¦¿ÜÇÏ°í´Â
// stereo ¼³Á¤ÇÒ ¶§ dual mono ¼³Á¤µµ °°ÀÌ ÇØÁÖ´Â config
#define SET_DUALMONO_IN_SOUNDMODE
#endif

BPLStatus OPL_HMXAudio_SetSoundMode(unsigned int ulDevIdx,const char* szSoundMode)
{
	DxStereoSelect_e		eAudioConfig;
#if defined(SET_DUALMONO_IN_SOUNDMODE)
	DxDualMonoSelect_e		eDualmonoConfig;
#endif

	eAudioConfig = OPL_UTIL_GetValueFromString(_SoundModeList,szSoundMode);

	CHECK_BPL_PARAM_PTR(szSoundMode);

	if (APK_OUTPUT_AUDIO_SetStereoSelect(eAudioConfig) != ERR_OK)
	{
		HxLOG_Error("Setting Sound Mode is failed.\n");
		return BPL_STATUS_ERROR;
	}

#if defined(SET_DUALMONO_IN_SOUNDMODE)

	switch(eAudioConfig)
	{
		case eDxSTEREO_SELECT_MONOLEFT:		eDualmonoConfig = eDxDUALMONO_SELECT_LEFT; 		break;
		case eDxSTEREO_SELECT_MONORIGHT:		eDualmonoConfig = eDxDUALMONO_SELECT_RIGHT; 		break;
		case eDxSTEREO_SELECT_UNKNOWN:		eDualmonoConfig = eDxDUALMONO_SELECT_UNKNOWN; 	break;
		default:								eDualmonoConfig = eDxDUALMONO_SELECT_BOTH; 		break;
	}

#if defined(CONFIG_OP_UK_DTT)
{
	HINT32 Enabled;
	 APK_DB_GetInt(USERDB_AUDIO_AUDIO_DESCRIPTION, &Enabled);
	if(Enabled == TRUE)
	{
		if (APK_OUTPUT_AUDIO_SetDualMonoSelect(eDualmonoConfig) != ERR_OK)
		{
			HxLOG_Error("Setting Dual Mono is failed.\n");
		}
	}
}
#else
	if (APK_OUTPUT_AUDIO_SetDualMonoSelect(eDualmonoConfig) != ERR_OK)
	{
		HxLOG_Error("Setting Dual Mono is failed.\n");
	}
#endif

#endif

	return BPL_STATUS_OK;
}

BPLStatus OPL_HMXAudio_GetDualMono(unsigned int ulDevIdx, int *pnSoundMode)
{
	CHECK_BPL_PARAM_PTR(pnSoundMode);

	DxDualMonoSelect_e		eAudioDualMono;

#if defined(CONFIG_PROD_HMS1000S) || defined(CONFIG_PROD_HMSLITE)
#define	USERDB_AUDIO_DUAL_MONO	"UDB/AUDIO_DualMono"
	if(APK_DB_GetInt(USERDB_AUDIO_DUAL_MONO, (HINT32*)&eAudioDualMono) == ERR_OK)
	{
		APK_DB_Remove(USERDB_AUDIO_DUAL_MONO);
		APK_OUTPUT_AUDIO_SetDualMonoSelect(eAudioDualMono);
	}
#undef 	USERDB_AUDIO_DUAL_MONO
#endif
	if(APK_OUTPUT_AUDIO_GetDualMonoSelect(&eAudioDualMono) != ERR_OK)
	{
		HxLOG_Error("Getting Dual Mono is failed.\n");
		return BPL_STATUS_ERROR;
	}

	*pnSoundMode = (int)eAudioDualMono;
	*pnSoundMode = *pnSoundMode - 1;

	return BPL_STATUS_OK;
}

// TODO: sound mode°¡ index·Î ¿À¸é ¾ÈµÇ°í OPL_HMXAudio_SetSoundMode() Ã³·³ stringÀ¸·Î ³»·Á¿À°Ô °íÃÄ¾ß ÇÑ´Ù..
BPLStatus OPL_HMXAudio_SetDualMono(unsigned int ulDevIdx,int nSoundMode)
{
	DxDualMonoSelect_e		eAudioConfig = (DxDualMonoSelect_e)(nSoundMode + 1);

	if (APK_OUTPUT_AUDIO_SetDualMonoSelect(eAudioConfig) != ERR_OK)
	{
		HxLOG_Error("Setting Dual Mono is failed.\n");
		return BPL_STATUS_ERROR;
	}

	return BPL_STATUS_OK;
}


static	EnumStringPair_t		_DigitalOutList[] =
{
		{	eDxDIGITALAUDIO_FORMAT_PCM,				"PCM"				},
		{	eDxDIGITALAUDIO_FORMAT_MULTICHANNEL, 	"Multi Channel"		},
		{	eDxDIGITALAUDIO_FORMAT_UNKNOWN, 		NULL				}
};

BPLStatus OPL_HMXAudio_GetSpdifAudioOutputMode(unsigned int ulDevIdx, char* szSpdifOutput)
{
	CHECK_BPL_PARAM_PTR(szSpdifOutput);

	DxDigitalAudioFormat_e		eSpdifOut = eDxDIGITALAUDIO_FORMAT_UNKNOWN;
	HCHAR						*data;

#if defined(CONFIG_PROD_HMS1000S) || defined(CONFIG_PROD_HMSLITE)
#define	USERDB_AUDIO_SPDIF_AUDIO_OUT	"UDB/AUDIO_SpdifAudioOut"
	if(APK_DB_GetInt(USERDB_AUDIO_SPDIF_AUDIO_OUT, (HINT32*)&eSpdifOut) == ERR_OK)
	{
		if(eSpdifOut > 2)	// BYPASS AAC AC3¸¦ multichannel¿¡ Æ÷ÇÔÇÑ´Ù. ¼¼ºÎ»çÇ×Àº pluginÀ¸·Î
			eSpdifOut = eDxDIGITALAUDIO_FORMAT_MULTICHANNEL;

		APK_DB_Remove(USERDB_AUDIO_SPDIF_AUDIO_OUT);
		APK_OUTPUT_AUDIO_SetSpdifFormat(eSpdifOut);
	}
#undef USERDB_AUDIO_SPDIF_AUDIO_OUT
#endif
	if(APK_OUTPUT_AUDIO_GetSpdifFormat(&eSpdifOut) != ERR_OK)
	{
		HxLOG_Error("Getting spdifOut format is failed.\n");
		return BPL_STATUS_ERROR;
	}

	data = OPL_UTIL_GetStringFromValue(_DigitalOutList, eSpdifOut);

	HxSTD_StrNCpy(szSpdifOutput, data, 31);

	return BPL_STATUS_OK;
}

BPLStatus OPL_HMXAudio_SetSpdifAudioOutputMode(unsigned int ulDevIdx, const char* szSpdifOutput)
{
	CHECK_BPL_PARAM_PTR(szSpdifOutput);

	DxDigitalAudioFormat_e		eSpdifOut =eDxDIGITALAUDIO_FORMAT_UNKNOWN;
	eSpdifOut = OPL_UTIL_GetValueFromString(_DigitalOutList,szSpdifOutput);

	if (APK_OUTPUT_AUDIO_SetSpdifFormat(eSpdifOut) != ERR_OK)
	{
		HxLOG_Error("Setting SpdifAudioOut is failed.\n");
		return BPL_STATUS_ERROR;
	}
	return BPL_STATUS_OK;
}

BPLStatus OPL_HMXAudio_GetHdmiAudioOutputMode(unsigned int ulDevIdx, char* szHdmiOutput)
{
	CHECK_BPL_PARAM_PTR(szHdmiOutput);

	DxDigitalAudioFormat_e		eHdmiOut = eDxDIGITALAUDIO_FORMAT_UNKNOWN;
	HCHAR						*data;

#if defined(CONFIG_PROD_HMS1000S) || defined(CONFIG_PROD_HMSLITE)
#define	USERDB_AUDIO_HDMI_AUDIO_OUT					"UDB/AUDIO_HdmiAudioOut"
	if(APK_DB_GetInt(USERDB_AUDIO_HDMI_AUDIO_OUT, (HINT32*)&eHdmiOut) == ERR_OK)
	{
		if(eHdmiOut > 2) // BYPASS AAC AC3¸¦ multichannel¿¡ Æ÷ÇÔÇÑ´Ù. ¼¼ºÎ»çÇ×Àº pluginÀ¸·Î
			eHdmiOut = eDxDIGITALAUDIO_FORMAT_MULTICHANNEL;

		APK_DB_Remove(USERDB_AUDIO_HDMI_AUDIO_OUT);
		APK_OUTPUT_AUDIO_SetHdmiFormat(eHdmiOut);
	}
#undef 	USERDB_AUDIO_HDMI_AUDIO_OUT
#endif
	if(APK_OUTPUT_AUDIO_GetHdmiFormat(&eHdmiOut) != ERR_OK)
	{
		HxLOG_Error("Getting HdmiAudioOut is failed.\n");
		return BPL_STATUS_ERROR;
	}

	data = OPL_UTIL_GetStringFromValue(_DigitalOutList, eHdmiOut);

	HxSTD_StrNCpy(szHdmiOutput, data, 31);

	return BPL_STATUS_OK;
}

BPLStatus OPL_HMXAudio_SetHdmiAudioOutputMode(unsigned int ulDevIdx, const char* szHdmiOutput)
{
	CHECK_BPL_PARAM_PTR(szHdmiOutput);

	DxDigitalAudioFormat_e		eHdmiOut = eDxDIGITALAUDIO_FORMAT_UNKNOWN;
	eHdmiOut = OPL_UTIL_GetValueFromString(_DigitalOutList,szHdmiOutput);

	if (APK_OUTPUT_AUDIO_SetHdmiFormat(eHdmiOut) != ERR_OK)
	{
		HxLOG_Error("Setting HdmiAudioOut is failed.\n");
		return BPL_STATUS_ERROR;
	}

	return BPL_STATUS_OK;
}

/* ±âÁ¸¿¡ ÀÖ´ø ÇÔ¼ö ¼Ò½º - JC_NGS´Â À§¿¡ ºÐ¸®µÈ °Í »ç¿ë ~ */
BPLStatus OPL_HMXAudio_GetDigitalAudioOutputMode(unsigned int ulDevIdx, char* szDigitalOutput)
{
	//HERROR		ulRet = BPL_STATUS_ERROR;
	HUINT32		ulDevNum = 0;

	CHECK_BPL_PARAM_PTR(szDigitalOutput);
	OPL_HMXAudio_GetNumOfDev(&ulDevNum);
	if (ulDevNum > 1 ) {
		/* °á±¹ 2°³ÀÇ Instance¸¦ »ç¿ëÇØ¼­ index·Î ±¸ºÐÇÏµµ·Ï ÇÑ´Ù. */
		if (0 == ulDevIdx) // HDMI
		{
			return OPL_HMXAudio_GetHdmiAudioOutputMode(ulDevIdx, szDigitalOutput);
		}
		else if (1 == ulDevIdx) // SPDIF
		{
			return OPL_HMXAudio_GetSpdifAudioOutputMode(ulDevIdx, szDigitalOutput);
		}
		else
		{
			return BPL_STATUS_ERROR;
		}
	}
	else
	{
		return OPL_HMXAudio_GetHdmiAudioOutputMode(ulDevIdx, szDigitalOutput);
	}
}

BPLStatus OPL_HMXAudio_SetDigitalAudioOutputMode(unsigned int ulDevIdx,const char* szDigitalOutput)
{
	//HERROR		ulRet = BPL_STATUS_ERROR;
	HUINT32		ulDevNum = 0;

	CHECK_BPL_PARAM_PTR(szDigitalOutput);
	OPL_HMXAudio_GetNumOfDev(&ulDevNum);
	if (ulDevNum > 1) {
		/* °á±¹ 2°³ÀÇ Instance¸¦ »ç¿ëÇØ¼­ index·Î ±¸ºÐÇÏµµ·Ï ÇÑ´Ù. */
		if (0 == ulDevIdx) // HDMI
		{
			return OPL_HMXAudio_SetHdmiAudioOutputMode(ulDevIdx, szDigitalOutput);
		}
		else if (1 == ulDevIdx) // SPDIF
		{
			return OPL_HMXAudio_SetSpdifAudioOutputMode(ulDevIdx, szDigitalOutput);
		}
		else
		{
			return BPL_STATUS_ERROR;
		}
	}
	else
	{
		return OPL_HMXAudio_SetHdmiAudioOutputMode(ulDevIdx, szDigitalOutput);
	}
}


#if 0
static	EnumStringPair_t		_DigitalCodecList[] =
{
	{	eDxAUDIO_CODEC_MPEG,			"MPEG"		},
	{	eDxAUDIO_CODEC_MP3,				"MP3"		},
	{	eDxAUDIO_CODEC_DOLBY_AC3 ,		"AC3"		},
	{	eDxAUDIO_CODEC_DOLBY_AC3P,		"DDP"		},
	{	eDxAUDIO_CODEC_PCM,				"PCM"		},
	{	eDxAUDIO_CODEC_AAC,				"AAC"		},
	{	eDxAUDIO_CODEC_AAC_LOAS,		"AAC-LOAS",	},
	{	eDxAUDIO_CODEC_AAC_PLUS,		"AAC+"		},
	{	eDxAUDIO_CODEC_AAC_PLUS_ADTS,	"AAC+ADTS"	},
	{	eDxAUDIO_CODEC_DTS,				"DTS"		},
	{	eDxAUDIO_CODEC_DTS_HD,			"DTS-HD"	},
	{	eDxAUDIO_CODEC_LPCM_DVD,		"LPCM-DVD"	},
	{	eDxAUDIO_CODEC_LPCM_HD_DVD,		"LPCM-HDDVD"},
	{	eDxAUDIO_CODEC_LPCM_BLUERAY,	"LPCM-BD"	},
	{	eDxAUDIO_CODEC_WMA_STD,			"WMA"		},
	{	eDxAUDIO_CODEC_WMA_PRO,			"WMA-PRO"	},
	{	eDxAUDIO_CODEC_PCMWAVE,			"PCMWAVE"	},
	{	eDxAUDIO_CODEC_DRA,				"DRA"		},
	{	eDxAUDIO_CODEC_AIFF,			"AIFF"		},
	{	eDxAUDIO_CODEC_UNKNOWN,			NULL		}
};
#endif


// TODO: ÀÌ API »èÁ¦ ¹Ù¶÷.
BPLStatus OPL_HMXAudio_GetDigitalAudioOutputCodec(unsigned int ulDevIdx, char* szCodec)
{
	CHECK_BPL_PARAM_PTR(szCodec);

	//DxAudioCodec_e		eCodec;
	//HCHAR					*data;

	//data = OPL_UTIL_GetStringFromValue(_DigitalCodecList, eCodec);

	//HxSTD_StrNCpy(szCodec, data, 31);

	return BPL_STATUS_OK;
}

// TODO: ÀÌ API »èÁ¦ ¹Ù¶÷.
BPLStatus OPL_HMXAudio_SetDigitalAudioOutputCodec(unsigned int ulDevIdx,const char* szCodec)
{
	return BPL_STATUS_OK;
}

BPLStatus	OPL_HFrontPanel_SetString( const char *panelString, int align, int dimlevel, BPL_BOOL bAuto )
{
	if (bAuto)
		APK_OUTPUT_PANEL_SetPanelAuto(eApkPANEL_PRIORITY_MIDDLE);
	else
		APK_OUTPUT_PANEL_SetPanel(eApkPANEL_PRIORITY_MIDDLE, panelString, align);

	return BPL_STATUS_OK;
}

BPLStatus	OPL_HFrontPanel_SetStringAsTopPriority( const char *panelString, int align, int dimlevel, BPL_BOOL bAuto )
{
	if (bAuto)
		APK_OUTPUT_PANEL_SetPanelAuto(eApkPANEL_PRIORITY_UPPER);
	else
		APK_OUTPUT_PANEL_SetPanel(eApkPANEL_PRIORITY_UPPER, panelString, align);

	return BPL_STATUS_OK;
}


BPLStatus	OPL_HFrontPanel_SetLedOnOff(int ledId, BPL_BOOL bOnOff, BPL_BOOL bAuto, BPL_BOOL bAllClear)
{
	if (bAuto)
		APK_OUTPUT_PANEL_SetLedAuto();
	else if (bAllClear)
		APK_OUTPUT_PANEL_ClearLed();
	else
		APK_OUTPUT_PANEL_SetLed(ledId, (bOnOff ? eDxLEDSTATUS_ON : eDxLEDSTATUS_OFF));

	return BPL_STATUS_OK;
}

BPLStatus	OPL_HFrontPanel_SetAnimation(int fpItemType, int param1, int param2, int param3)
{
	switch(fpItemType)
	{
		case eApkPANEL_TYPE_STRING:
			APK_OUTPUT_PANEL_SetPanelAnimation(eApkPANEL_PRIORITY_MIDDLE);
			break;
		default:
			break;
	}

	return BPL_STATUS_OK;
}

BPLStatus 	OPL_HFrontPanel_SetCategoryInform(int nCategory)
{
	APK_OUTPUT_PANEL_SetLedCategory(nCategory);

	return BPL_STATUS_OK;
}

BPLStatus OPL_HMXIrdetoImportDBToUsb(OplLocalSystemIrdetoImportExport_t listener, BPL_BOOL bSync)
{
	s_ImportExportListener	= listener;

	APK_EXE_ImportDB(opl_ImportExport_Db_EvtListener, bSync);

	return BPL_STATUS_OK;
}

BPLStatus OPL_HMXIrdetoExportDBToUsb(OplLocalSystemIrdetoImportExport_t listener, BPL_BOOL bSync)
{
	s_ImportExportListener 	= listener;

	APK_EXE_ExportDB(opl_ImportExport_Db_EvtListener, bSync);

	return BPL_STATUS_OK;
}
BPLStatus OPL_HMXTrdConflictInfo(OplLocalSystemTrdConflict_t listener,unsigned int depth)
{
	s_TrdConflictListener 	= listener;

	APK_EXE_GetTrdConflictInfo(opl_TrdConflict_EvtListener,depth);

	return BPL_STATUS_OK;
}

BPLStatus OPL_HMXSetTrdConflictInfo(unsigned int depth, unsigned int index)
{
	APK_EXE_SetTrdConflictInfo(depth, index);

	return BPL_STATUS_OK;
}


BPLStatus OPL_HMXStartLcnUpdate(OplLocalSystemLcnUpdate_t listener, int searchMode)
{
	s_LcnUpdateListener	= listener;

	APK_EXE_StartLcnUpdate((APK_EXE_LcnUpdateNotifier_t)opl_LcnUpdate_EvtListener, searchMode);

	return BPL_STATUS_OK;
}

BPLStatus OPL_HMXStopLcnUpdate(OplLocalSystemLcnUpdate_t listener)
{
	s_LcnUpdateListener 	= listener;

	APK_EXE_StopLcnUpdate((APK_EXE_LcnUpdateNotifier_t)opl_LcnUpdate_EvtListener);

	return BPL_STATUS_OK;
}

BPLStatus OPL_HMXSaveLcnUpdate(OplLocalSystemLcnUpdate_t listener, int searchMode, int index)
{
	s_LcnUpdateListener 	= listener;

	APK_EXE_SaveLcnUpdate((APK_EXE_LcnUpdateNotifier_t)opl_LcnUpdate_EvtListener, searchMode, index);

	return BPL_STATUS_OK;
}

BPLStatus OPL_HMXGetLcnUpdateInfo(OplLocalSystemLcnUpdate_t listener)
{
	s_LcnUpdateListener 	= listener;

	APK_EXE_GetLcnUpdateInfo((APK_EXE_LcnUpdateNotifier_t)opl_LcnUpdate_EvtListener);

	return BPL_STATUS_OK;
}

BPLStatus OPL_HMXRemoveLcnUpdateInfo(BPL_BOOL syncMode)
{
	APK_EXE_RemoveLcnUpdateInfo(syncMode);

	return BPL_STATUS_OK;
}

BPLStatus OPL_HMXColdBoot_GetColdBootDone(BPL_BOOL *pbColdBootDone, OplLocalSystemColdBootDone_t  listener)
{
	HBOOL checkColdBootDone;

	s_ColdBootDoneListener 	= listener;

	APK_SYSINFO_STB_GetColdBootDone(&checkColdBootDone, opl_ColdBootDone_EvtListener);
	*pbColdBootDone = (BPL_BOOL)checkColdBootDone;

	return BPL_STATUS_OK;
}

BPLStatus OPL_HMXColdBoot_UnregisterNotifier(void)
{
	s_ColdBootDoneListener 	= NULL;
	APK_SYSINFO_STB_UnregisterListener(NULL);

	return BPL_STATUS_OK;
}

static APK_EXE_BATCH_Notifier_t opl_ClockRecovery_EvtListener(DxBatchType_e eType, DxBatchProc_b eProc, HERROR eERR)
{
	if (s_ClockRecoveryListener != NULL)
		s_ClockRecoveryListener(eType, eProc, eERR);

	return BPL_STATUS_OK;
}

BPLStatus OPL_HMXClockRecovery(OplLocalClockRecovery_t listener)
{
	APK_EXE_BATCH_Install(2,eDxBAT_TYPE_USER, eDxBAT_PROC_TIMERECOVERY);
	s_ClockRecoveryListener = listener;
	APK_EXE_BATCH_RegisterNotifier((APK_EXE_BATCH_Notifier_t)opl_ClockRecovery_EvtListener);
	APK_EXE_BATCH_Start(eDxBAT_TYPE_USER);

	return BPL_STATUS_OK;
}

BPLStatus OPL_HMXCasActionIrEmmForcedOta_RegisterNotifier(OplLocalSystemCasActionIrEmmForcedOta_t  listener)
{
	s_CasActionIrEmmForcedOtaListener 	= listener;

	APK_CAS_ACTION_RegisterNotifier(opl_CasActionIrEmmForcedOta_EvtListener);

	return BPL_STATUS_OK;
}

BPLStatus OPL_HMXCasActionIrEmmForcedOta_UnregisterNotifier(OplLocalSystemCasActionIrEmmForcedOta_t  listener)
{
	s_CasActionIrEmmForcedOtaListener 	= NULL;

	APK_CAS_ACTION_UnRegisterNotifier(opl_CasActionIrEmmForcedOta_EvtListener);

	return BPL_STATUS_OK;
}
#define	___NcdInfo_Functions____

BPLStatus OPL_HMXOperatorFeauture_GetNcdInfo(OplLocalSystemNcdInfo_t listener,unsigned int param)
{
	s_NcdInfoListener 	= listener;

	APK_EXE_GetNcdInfo(opl_NcdInfo_EvtListener,param);

	return BPL_STATUS_OK;
}
BPLStatus OPL_HMXOperatorFeauture_SetNcdNotifyData(HUINT32 ulASONotifyFlag)
{

	APK_EXE_SetNcdNotifyData(ulASONotifyFlag);

	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_GetChannelSetupPIN( BPL_BOOL *pbChannelSetupPin)
{
	HBOOL	bFlag;

	CHECK_BPL_PARAM_PTR(pbChannelSetupPin);

	APK_SCENARIO_GetChannelSetupPIN(&bFlag);
	*pbChannelSetupPin = (BPL_BOOL)bFlag;

	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_SetChannelSetupPIN( BPL_BOOL bChannelSetupPin)
{
	APK_SCENARIO_SetChannelSetupPIN(bChannelSetupPin);

	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_SetGuidancePolicy(HUINT32 guidancePolicy)
{
	APK_SCENARIO_SetGuidancePolicy(guidancePolicy);

	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_GetGuidancePolicy(HUINT32 *guidancePolicy)
{
	CHECK_BPL_PARAM_PTR(guidancePolicy);
	APK_SCENARIO_GetGuidancePolicy(guidancePolicy);

	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_SetHdmiCecMode( BPL_BOOL HdmiCecMode)
{
	APK_OUTPUT_HDMI_SetCecEnable(HdmiCecMode);
	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_GetHdmiCecMode( BPL_BOOL *pHdmiCecMode)
{
	CHECK_BPL_PARAM_PTR(pHdmiCecMode);
	APK_OUTPUT_HDMI_GetCecEnable((HBOOL *)pHdmiCecMode);
	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_GetHdmiEdidInfo( char *pszEdidInfo)
{
        APK_OUTPUT_HDMI_GetEdidInfo(pszEdidInfo);
        return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_SetHdmiCecVolumeCtrlMode( BPL_BOOL HdmiCecVolMode)
{
	APK_OUTPUT_HDMI_SetCecVolumeCtrlEnable(HdmiCecVolMode);
	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_GetHdmiCecVolumeCtrlMode( BPL_BOOL *pHdmiCecVolMode)
{
	CHECK_BPL_PARAM_PTR(pHdmiCecVolMode);
	APK_OUTPUT_HDMI_GetCecVolumeCtrlEnable((HBOOL *)pHdmiCecVolMode);
	return BPL_STATUS_OK;
}


BPLStatus OPL_LocalSystem_GetRpURL( char *pszUrl, unsigned int ulLen)
{
#define _RP_URL_FILE	"/url/url_rp.txt"
	char * pTempUrl = NULL;
	HxSTD_MemSet(pszUrl,0,ulLen);
	pTempUrl = (char *)UAPI_FM_GetDefaultURL((HCHAR *)CONFIG_RP_SERVER_URL, (HCHAR *)_RP_URL_FILE);
	if(pTempUrl) HxSTD_StrNCpy(pszUrl, pTempUrl, ulLen);
	pszUrl[ulLen-1] = '\0';
#undef _RP_URL_FILE
	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_GetPortalURL( char *pszUrl, unsigned int ulLen)
{
#define _PORTAL_URL_FILE	"/url/url_portal.txt"

	char * pTempUrl = NULL;
	HxSTD_MemSet(pszUrl,0,ulLen);
	pTempUrl = (char *)UAPI_FM_GetDefaultURL((HCHAR *)CONFIG_APPPORTAL_SERVER_URL, (HCHAR *)_PORTAL_URL_FILE);
	if(pTempUrl) HxSTD_StrNCpy(pszUrl, pTempUrl, ulLen);
	pszUrl[ulLen-1] = '\0';

#undef _PORTAL_URL_FILE
	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_GetIpOtaURL( char *pszUrl, unsigned int ulLen)
{
#define _IPOTA_URL_FILE	"/url/url_ipota.txt"

	char * pTempUrl = NULL;
	HxSTD_MemSet(pszUrl,0,ulLen);
	pTempUrl = (char *)UAPI_FM_GetDefaultURL((HCHAR *)CONFIG_IPOTA_SERVER_URL, (HCHAR *)_IPOTA_URL_FILE);
	if(pTempUrl) HxSTD_StrNCpy(pszUrl, pTempUrl, ulLen);
	pszUrl[ulLen-1] = '\0';

#undef _IPOTA_URL_FILE
	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_GetHbbtvURL( char *pszUrl, unsigned int ulLen)
{
#define _PORTAL_RUL_FILE	"/url/url_hbbtv.txt"

	char * pTempUrl = NULL;
	HxSTD_MemSet(pszUrl,0,ulLen);
	pTempUrl = (char *)UAPI_FM_GetDefaultURL(NULL, (HCHAR *)_PORTAL_RUL_FILE);
	if(pTempUrl == NULL)
		return BPL_STATUS_ERROR;

	if(pTempUrl) HxSTD_StrNCpy(pszUrl, pTempUrl, ulLen);
	pszUrl[ulLen-1] = '\0';

#undef _PORTAL_RUL_FILE
	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_SetNtpServerURL(const char *pszUrl)
{
	HERROR	hErr;

	hErr = APK_SCENARIO_SetNtpServerURL((HCHAR *)pszUrl);
	if (hErr != ERR_OK)
	{
		return BPL_STATUS_ERROR;
	}

	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_GetNetworkIds(char *pszNetworkId, unsigned int ulLen)
{
	HxVector_t	*pstNetworkVector = NULL;
	DxNetwork_t	*pstNetwork		  = NULL;
	HUINT32		i, ulNetSize;
	HINT32					nStream;

	nStream = HLIB_RWSTREAM_OpenEx(NULL, 512, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
	if (!nStream)
	{
		HxLOG_Error("HLIB_RWSTREAM_OpenEx failed. \n");
		return BPL_STATUS_ERROR;
	}

	HLIB_RWSTREAM_Print(nStream,"{\"NETWORK_ID\":[");
	pstNetworkVector = APK_META_SVC_GetNetworkList();
	if (pstNetworkVector != NULL)
	{
		ulNetSize = HLIB_VECTOR_Length(pstNetworkVector);
		for (i=0;i<ulNetSize;i++)
		{
			pstNetwork = (DxNetwork_t *)HLIB_VECTOR_ItemAt(pstNetworkVector, i);
			if (pstNetwork == NULL)
			{
				continue;
			}
			if (i == 0)
			{
				HLIB_RWSTREAM_Print(nStream,"%d",pstNetwork->onid);
			}
			else
			{
				HLIB_RWSTREAM_Print(nStream,",%d",pstNetwork->onid);
			}

		}
	}
	HLIB_RWSTREAM_Print(nStream,"]}");
	HxSTD_snprintf(pszNetworkId,ulLen,"%s",(HCHAR*)HLIB_RWSTREAM_GetBuf(nStream));

	HLIB_RWSTREAM_Close(nStream);
	if (pstNetworkVector) {HLIB_VECTOR_Delete(pstNetworkVector);}

	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_GetFvcNetworkUriPrefix(char *pszUrl, unsigned int ulLen)
{
#if defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME)
	HCHAR		*uriPrefix		  = NULL;
	HxVector_t	*pstNetworkVector = NULL;
	DxNetwork_t	*pstNetwork		  = NULL;

	pstNetworkVector = APK_META_SVC_GetNetworkList();
	if (pstNetworkVector == NULL)
	{
		return BPL_STATUS_ERROR;
	}

	pstNetwork = (DxNetwork_t *)HLIB_VECTOR_ItemAt(pstNetworkVector, 0);
	if (pstNetwork == NULL)
	{
		if (pstNetworkVector) {HLIB_VECTOR_Delete(pstNetworkVector);}
		return BPL_STATUS_ERROR;
	}

	// priority 1. USB
	uriPrefix = UAPI_FM_GetDefaultURL(NULL, (HCHAR *)FVC_URL_FILE);

	if (uriPrefix == NULL)
	{
		// priority 2. LIVE SI
		uriPrefix = pstNetwork->ex.tva.UriPrefix;
		HxLOG_Print("[%s:%d] Read LIVE uriPrefix [%s]\n", __FUNCTION__, __LINE__,uriPrefix);
	}
	else
	{
		HxLOG_Print("[%s:%d] Read USB uriPrefix [%s]\n", __FUNCTION__, __LINE__,uriPrefix);
	}

	if (HxSTD_StrEmpty(uriPrefix) == TRUE)
	{
		HxLOG_Error("empty fvc network uri prefix \n");
		if (pstNetworkVector) {HLIB_VECTOR_Delete(pstNetworkVector);}
		return BPL_STATUS_ERROR;
	}

	HxSTD_MemSet(pszUrl,0,ulLen);
	HxSTD_StrNCpy(pszUrl, uriPrefix, ulLen);
	pszUrl[ulLen-1] = '\0';

	if (pstNetworkVector) {HLIB_VECTOR_Delete(pstNetworkVector);}

	return BPL_STATUS_OK;
#else
	return BPL_STATUS_ERROR;
#endif
}

BPLStatus OPL_LocalSystem_GetFvcBaseURL(char *pszUrl, unsigned int ulLen)
{
	HERROR	hErr;

	hErr = APK_SCENARIO_GetFvcBaseUrl(pszUrl, ulLen);
	if (hErr != ERR_OK)
	{
		return BPL_STATUS_ERROR;
	}

	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_SetFvcBaseURL(const char *pszUrl)
{
	HCHAR	*szCurProcessName = NULL;

	(void)APK_SCENARIO_SetFvcBaseUrl(pszUrl);

	szCurProcessName = HLIB_STD_GetCurProcessName();
	(void)HAPI_BroadcastSignal("signal:onFvcBaseUrlChanged", 0, "ss", pszUrl, szCurProcessName);

	return BPL_STATUS_OK;
}
BPLStatus OPL_LocalSystem_GetFvcAuthHash(char *str, unsigned int ulLen)
{
	HERROR	hErr;

	hErr = APK_SCENARIO_GetFvcAuthHash(str, ulLen);
	if (hErr != ERR_OK)
	{
		return BPL_STATUS_ERROR;
	}

	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_SetFvcAuthHash(const char *str)
{
	HCHAR	*szCurProcessName = NULL;

	(void)APK_SCENARIO_SetFvcAuthHash(str);

	szCurProcessName = HLIB_STD_GetCurProcessName();
	(void)HAPI_BroadcastSignal("signal:onFvcAuthHashChanged", 0, "ss", str, szCurProcessName);

	return BPL_STATUS_OK;
}
BPLStatus OPL_LocalSystem_GetFvcAuthTimeStamp(char *str, unsigned int ulLen)
{
	HERROR	hErr;

	hErr = APK_SCENARIO_GetFvcAuthTimeStamp(str, ulLen);
	if (hErr != ERR_OK)
	{
		return BPL_STATUS_ERROR;
	}

	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_SetFvcAuthTimeStamp(const char *str)
{
	HCHAR	*szCurProcessName = NULL;

	(void)APK_SCENARIO_SetFvcAuthTimeStamp(str);

	szCurProcessName = HLIB_STD_GetCurProcessName();
	(void)HAPI_BroadcastSignal("signal:onFvcAuthTimeStampChanged", 0, "ss", str, szCurProcessName);

	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_CheckFileExist(char *pszUri, BPL_BOOL *IsExist)
{
	*IsExist = (BPL_BOOL)HLIB_FILE_Exist((HUINT8 *)pszUri);
	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_SetFirstBootStatusOnProduction(BPL_BOOL IsFirstBoot)
{
	APK_SYSINFO_STB_SetFirstBootOnProduction(IsFirstBoot);

	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_GetFirstBootStatusOnProduction(BPL_BOOL *IsFirstBoot)
{
	HBOOL	bfirstbootonproduction;

	CHECK_BPL_PARAM_PTR(IsFirstBoot);

	if (APK_SYSINFO_STB_GetFirstBootOnProduction(&bfirstbootonproduction) == ERR_OK)
	{
		*IsFirstBoot = (BPL_BOOL)bfirstbootonproduction;

		return BPL_STATUS_OK;
	}

	return BPL_STATUS_ERROR;
}

BPLStatus OPL_LocalSystem_GetLaunchAppMode(HUINT32 *pulLaunchAppMode)
{
	HUINT32 ulLaunchAppMode;

	CHECK_BPL_PARAM_PTR(pulLaunchAppMode);

	if (APK_SYSINFO_STB_GetLaunchAppMode(&ulLaunchAppMode) == ERR_OK)
	{
		*pulLaunchAppMode = (HUINT32)ulLaunchAppMode;

		return BPL_STATUS_OK;
	}

	return BPL_STATUS_ERROR;
}

BPLStatus OPL_LocalSystem_GetFusingInfo(BPL_BOOL *IsSCS, BPL_BOOL *IsJTAG, BPL_BOOL *IsOTP, BPL_BOOL *IsCWE, unsigned int *pulMarketID)
{
	HUINT8 ucSCS = 0;
	HUINT8 ucJTAG = 0;
	HUINT8 ucOTP = 0;
	HUINT8 ucCWE = 0;
	HUINT32 ulMarketID = 0;

	CHECK_BPL_PARAM_PTR(IsSCS);
	CHECK_BPL_PARAM_PTR(IsJTAG);
	CHECK_BPL_PARAM_PTR(IsOTP);
	CHECK_BPL_PARAM_PTR(IsCWE);
	CHECK_BPL_PARAM_PTR(pulMarketID);

	APK_SYSINFO_STB_GetFusingInfo(&ucSCS, &ucJTAG, &ucOTP, &ucCWE, &ulMarketID);
	*IsSCS = (BPL_BOOL)ucSCS ? TRUE : FALSE;
	*IsJTAG = (BPL_BOOL)ucJTAG? TRUE : FALSE;
	*IsOTP = (BPL_BOOL)ucOTP? TRUE : FALSE;
	*IsCWE = (BPL_BOOL)ucCWE? TRUE : FALSE;
	*pulMarketID = ulMarketID;

	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_GetFusingInfoExt(BPL_BOOL *IsSCS, BPL_BOOL *IsJTAG, BPL_BOOL *IsOTP, BPL_BOOL *IsCWE, unsigned int* pulOTPField1, unsigned int* pulOTPField2)
{
	HUINT8 ucSCS = 0;
	HUINT8 ucJTAG = 0;
	HUINT8 ucOTP = 0;
	HUINT8 ucCWE = 0;
	HUINT32	ulOTPField1 = 0;
	HUINT32	ulOTPField2 = 0;

	CHECK_BPL_PARAM_PTR(IsSCS);
	CHECK_BPL_PARAM_PTR(IsJTAG);
	CHECK_BPL_PARAM_PTR(IsOTP);
	CHECK_BPL_PARAM_PTR(IsCWE);
	CHECK_BPL_PARAM_PTR(pulOTPField1);
	CHECK_BPL_PARAM_PTR(pulOTPField2);

	APK_SYSINFO_STB_GetFusingInfoExt(&ucSCS, &ucJTAG, &ucOTP, &ucCWE, &ulOTPField1, &ulOTPField2);
	*IsSCS = (BPL_BOOL)ucSCS ? TRUE : FALSE;
	*IsJTAG = (BPL_BOOL)ucJTAG? TRUE : FALSE;
	*IsOTP = (BPL_BOOL)ucOTP? TRUE : FALSE;
	*IsCWE = (BPL_BOOL)ucCWE? TRUE : FALSE;
	*pulOTPField1 = ulOTPField1;
	*pulOTPField2 = ulOTPField2;

	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_SetAntennaPower(BPL_BOOL bAntPower)
{
	APK_SYSINFO_STB_SetAntennaPower(bAntPower);

	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_GetAntennaPower(BPL_BOOL *pbAntPower)
{
	HBOOL bAntennaPower;

	CHECK_BPL_PARAM_PTR(pbAntPower);

	if (APK_SYSINFO_STB_GetAntennaPower(&bAntennaPower) == ERR_OK)
	{
		*pbAntPower = (BPL_BOOL)bAntennaPower;

		return BPL_STATUS_OK;
	}

	return BPL_STATUS_ERROR;
}

BPLStatus OPL_LocalSystem_SetServiceUpdateFlag(BPL_BOOL bOnOff)
{
	APK_SYSINFO_STB_SetServiceUpdateFlag(bOnOff);

	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_GetServiceUpdateFlag(BPL_BOOL *pbOnOff)
{
	HBOOL bSvcUpdateFlag;

	CHECK_BPL_PARAM_PTR(pbOnOff);

	if (APK_SYSINFO_STB_GetServiceUpdateFlag(&bSvcUpdateFlag) == ERR_OK)
	{
		*pbOnOff = (BPL_BOOL)bSvcUpdateFlag;

		return BPL_STATUS_OK;
	}

	return BPL_STATUS_ERROR;
}


BPLStatus OPL_LocalSystem_GetNandBadBlocks(unsigned int *pulCntLdr, unsigned int *pulCntUBI, unsigned int *pulCntUBIFS)
{
	HUINT32 ulLdr = 0;
	HUINT32 ulUBI = 0;
	HUINT32 ulUBIFS = 0;

	CHECK_BPL_PARAM_PTR(pulCntLdr);
	CHECK_BPL_PARAM_PTR(pulCntUBI);
	CHECK_BPL_PARAM_PTR(pulCntUBIFS);

	if (APK_SYSINFO_STB_GetNandBadBlocks(&ulLdr, &ulUBI, &ulUBIFS) == ERR_OK)
	{
		*pulCntLdr = (unsigned int)ulLdr;
		*pulCntUBI = (unsigned int)ulUBI;
		*pulCntUBIFS = (unsigned int)ulUBIFS;

		return BPL_STATUS_OK;
	}

	return BPL_STATUS_ERROR;
}

BPLStatus OPL_LocalSystem_GetHCRData(char **pszbuffer)
{
	HUINT32 size = 0;
	HUINT8 *pszUri = (HUINT8 *)HCRPATH;
	HxFILE_t hSelf;

	if ((HBOOL)HLIB_FILE_Exist(pszUri))
	{
		hSelf = HLIB_FILE_Open(pszUri, (const HUINT8 *)"r");
		if(hSelf == NULL)
		{
			HxLOG_Error("error occurred in HLIB_FILE_Open(%s).\n", pszUri);
			return ERR_FAIL;
		}
		HLIB_FILE_Size32(hSelf, &size);
		HLIB_FILE_Close(hSelf);

		printf("[Humax Crash Report] opl get HCR size (%d).\n", size);

		if (size > 0 && size <= MAXHCRDATALEN)
		{
			if(s_HCRbuffer)
			{
				HLIB_MEM_Free(s_HCRbuffer);
				s_HCRbuffer = NULL;
			}
			s_HCRbuffer = (HUINT8 *)HLIB_MEM_Malloc(size + 1); // + 1 is \0
			if(s_HCRbuffer != NULL)
			{
				HxSTD_MemSet(s_HCRbuffer, 0, size + 1);
				if (ERR_OK == HLIB_FILE_ReadFile(pszUri, (void **)&s_HCRbuffer, &size))
				{
					*pszbuffer = (char *)s_HCRbuffer;
					(void)HLIB_FILE_Delete(pszUri);
					return BPL_STATUS_OK;
				}
				else
				{
					HxLOG_Error("error occurred in HLIB_FILE_ReadFile(%s).\n", pszUri);
				}
			}
		}
	}

	return BPL_STATUS_ERROR;
}

BPLStatus OPL_LocalSystem_CheckHCRData()
{
	HUINT32 size = 0;
	HUINT8 *pszUri = (HUINT8 *)HCRPATH;
	HxFILE_t hSelf;

	if ((HBOOL)HLIB_FILE_Exist(pszUri))
	{
		hSelf = HLIB_FILE_Open(pszUri, (const HUINT8 *)"r");
		if(hSelf != NULL)
		{
			HLIB_FILE_Size32(hSelf, &size);
			HLIB_FILE_Close(hSelf);

			printf("[Humax Crash Report] opl check HCR size (%d).\n", size);

			if (size <= MAXHCRDATALEN)
			{
				return BPL_STATUS_OK;
			}
			else
			{
				HxLOG_Error("HCR size over limitation(%d).\n", size);
			}
		}
	}

	return BPL_STATUS_ERROR;
}

BPLStatus OPL_LocalSystem_GetBatUpdateMode(HINT32 *pnBatUpdateMode)
{
	BPL_BOOL bBatUpdateMode = FALSE;

	CHECK_BPL_PARAM_PTR(pnBatUpdateMode);

	APK_SCENARIO_GetBatUpdateOnOff((HBOOL *)&bBatUpdateMode);

	if (bBatUpdateMode == FALSE)
		*pnBatUpdateMode = 0;
	else
		*pnBatUpdateMode = 1;

	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_SetBatUpdateMode(HINT32 nBatUpdateMode)
{
	if (nBatUpdateMode == 0)
	{
		APK_SCENARIO_SetBatUpdateOnOff(FALSE);
	}
	else
	{
		APK_SCENARIO_SetBatUpdateOnOff(TRUE);
	}

	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_GetNitLcnUpdateMode(HINT32 *pnNitLcnUpdateMode)
{
	BPL_BOOL bNitLcnUpdateMode = FALSE;

	CHECK_BPL_PARAM_PTR(pnNitLcnUpdateMode);

	APK_SCENARIO_GetNitLcnUpdateOnOff((HBOOL *)&bNitLcnUpdateMode);

	if (bNitLcnUpdateMode == FALSE)
		*pnNitLcnUpdateMode = 0;
	else
		*pnNitLcnUpdateMode = 1;

	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_SetNitLcnUpdateMode(HINT32 nNitLcnUpdateMode)
{
	if (nNitLcnUpdateMode == 0)
	{
		//APK_EXE_StartLcnRenumbering();
		APK_SCENARIO_SetNitLcnUpdateOnOff(FALSE);
	}
	else
	{
		APK_SCENARIO_SetNitLcnUpdateOnOff(TRUE);
	}

	return BPL_STATUS_OK;
}


BPLStatus OPL_LocalSystem_SetHbbTvEnabled(BPL_BOOL bEnabled)
{

	HBOOL	bHbbTvEnabled = bEnabled;

	APK_DB_SetInt(USERDB_HBBTV_ENABLE, (HUINT32)bHbbTvEnabled);

	HAPI_BroadcastSignal("signal:onHbbTVEnabled", 1, "i", (HUINT32)bHbbTvEnabled);

	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_GetHbbTvEnabled(BPL_BOOL *pbEnabled)
{

	HBOOL	bHbbTvEnabled = FALSE;
	if(APK_DB_GetInt(USERDB_HBBTV_ENABLE, (HINT32*)&bHbbTvEnabled) != ERR_OK)
	{
#if defined(CONFIG_OP_AUS_DTT)
		bHbbTvEnabled = FALSE;
#else
		bHbbTvEnabled = TRUE;
#endif
		APK_DB_SetInt(USERDB_HBBTV_ENABLE, (HUINT32)bHbbTvEnabled);
		HAPI_BroadcastSignal("signal:onHbbTVEnabled", 1, "i", (HUINT32)bHbbTvEnabled);
	}

	*pbEnabled = (BPL_BOOL)bHbbTvEnabled;

	return BPL_STATUS_OK;
}



BPLStatus OPL_LocalSystem_getOipfGrab(int *value)
{
	*value = g_nOipfKeyGrab;
	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_GetLastServiceGroup( int *pnLastSvcGroup)
{
	APK_SCENARIO_GetLastServiceGroup((HINT32*)pnLastSvcGroup);

	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_SetLastServiceGroup( int nLastSvcGroup)
{
	APK_SCENARIO_SetLastServiceGroup((HINT32)nLastSvcGroup);

	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_GetAutomaticPowerdown(unsigned int *pnTimeout)
{
	APK_SCENARIO_GetAutomaticPowerdown(pnTimeout);
	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_SetAutomaticPowerdown(unsigned int nTimeout)
{

	APK_SCENARIO_SetAutomaticPowerdown((HUINT32)nTimeout);
	return BPL_STATUS_OK;
}

// priority outputs
// 0 : all   //default
// 1 : hdmi
// 2 : scart
BPLStatus OPL_LocalSystem_GetPriorityHDMIOutput(unsigned int *priority_outputs)
{
	HBOOL priority = 0;

	APK_SCENARIO_GetPriorityHDMIOutput(&priority);    // scart cut off


    if( priority == 0 ) // scart on & hdmi on
	{
        *priority_outputs = 0;
	}
    else
    {
        *priority_outputs = 1;
    }


	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_SetPriorityHDMIOutput(unsigned int priority_outputs)
{


	switch( priority_outputs )
	{
		case 1 :   //hdmi
			APK_SCENARIO_SetPriorityHDMIOutput(1); // scart cut off
			break;
		case 0 :
			APK_SCENARIO_SetPriorityHDMIOutput(0); // scart on & hdmi on
			break;
		default:
			APK_SCENARIO_SetPriorityHDMIOutput(0);   // work as default now
	}
	return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_GetVerimatrixLibVersion(eOplVerimatirxLibType type, char * version, unsigned int max_len)
{
        HERROR result = ERR_OK;

     switch(type)
     {
        case OPLVERIMATRIX_LIB_IPTV:
            result= APK_CAS_VERIMATRIX_GetIptvHybridLibVersion((HCHAR *)version);
            break;
        case OPLVERIMATRIX_LIB_WEB:
            result= APK_CAS_VERIMATRIX_GetWebLibVersion((HCHAR *)version);
            break;
        case OPLVERIMATRIX_LIB_GATEWAY:
            result= APK_CAS_VERIMATRIX_GetGatewayLibVersion((HCHAR *)version);
            break;
        default:
            result = ERR_FAIL;
            break;
     }

    if(result != ERR_OK)
        memset(version, 0, sizeof(char) * max_len);

    return BPL_STATUS_OK;
}

BPLStatus OPL_LocalSystem_GetNflixESN( char *pszNetflixEsn)
{
#if defined(CONFIG_NETFLIX_APP)
	CHECK_BPL_PARAM_PTR(pszNetflixEsn);
	APK_SYSINFO_STB_GetNetflixESN(pszNetflixEsn,OAPI_SYSINFO_ESN_LEN);
#endif
	return BPL_STATUS_OK;
}

/* end of file */
