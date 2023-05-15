/**************************************************************
*	@file	rema.c
*	http://www.humaxdigital.com
*	@author			humax
*	@brief			rema
**************************************************************/

/*
* ⓒ 2011-2012 Humax Co., Ltd.
* This program is produced by Humax Co., Ltd. ("Humax") and
* the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
* non-assignable, non-transferable and non-exclusive license to use this Software.
* You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
* you agree to the responsibility to take all reasonable efforts to protect the any information
* you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
* reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
* If you have no authorized license, discontinue using this Software immediately.
*
* THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS,
* IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
* IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
*  CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.
*
* This License is effective until terminated. You may terminate this License at any time by destroying all copies
* of the Software including all documentation. This License will terminate immediately without notice from Humax
* to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
* of the Software and all documentation.
*
* The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
* All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
* in the Republic of Korea.
*/

#define ___HEADER_FILES___

#include <vkernel.h>
#include <hlib.h>
#include <napi.h>
#include <rema_int.h>

#include "rema_cwmp.h"
#include "rema_DM.h"
#include "RDM_Network.h"

#if defined(CONFIG_OP_TDC)
#include "rema_DM_TDC.h"
#else	// There is no Humax ACS for rema.
#define TEST_ACS_SERVER			"http://192.168.1.2:8000/testacs"
#define PRODUCT_ACS_SERVER		"http://10.0.12.145:8080/openacs/acs"
#endif

#define ___DEFINES___
#define	DESCRIPTION_MAX_LEN		256
#define PRODUCTION_SERVER		0	

#define ___MACROS___

#define ___TYPEDEF___

typedef struct RouteHops_DM
{
	HCHAR HopHost[SIZE_256];
}RouteHops_DM_t;

/*
	Device.NSLookupDiagnostics.Result.{i}.	object
	Status	string
	AnswerType	string
	HostNameReturned	string(256)
	IPAddresses	string(45)
	DNSServerIP	string(45)
	ResponseTime	unsignedInt
*/

typedef enum
{
	NRD_None = 0,
	NRD_Status,
	NRD_AnswerType,
	NRD_HostNameReturned,
	NRD_IPAddresses,
	NRD_DNSServerIP,
	NRD_ResponseTime,
	NRD_Max,
}NSLookupDiag_Result_Data_e;


typedef struct NSLookupDiagnostics_Result_DM
{
	HCHAR Status[SIZE_16];
	HCHAR AnswerType[SIZE_16];
	HCHAR HostNameReturned[SIZE_256];
	HCHAR IPAddresses[SIZE_256];
	HCHAR DNSServerIP[SIZE_256];
	HCHAR ResponseTime[SIZE_16];
}NSLookupDiag_Result_DM_t;

/*
	Device.DeviceInfo.SupportedDataModel.{i}.	object
	URL	string(256)
	URN	string(256)
	Features	string
*/
typedef enum
{
	SDM_None = 0,
	SDM_URL,
	SDM_URN,
	SDM_Features,
	SDM_Max,
}SupportedDataModel_e;

typedef struct SupportedDataModel_DM
{
	HCHAR URL[SIZE_256];
	HCHAR URN[SIZE_256];
	HCHAR Features[SIZE_256];
}SupportedDataModel_DM_t;


typedef enum
{
	CDT_None = 0,
	CDT_SupportedDataModel,
	CDT_TraceRouteHops,
	CDT_NSLookupDiagResult,
	CDT_Max,
}CPE_DM_TYPE_e;


typedef struct CPE_DM
{
	SupportedDataModel_DM_t *pstSppportedDM;
	RouteHops_DM_t *pstRouteHops;
	NSLookupDiag_Result_DM_t *pstNSLookupResult;
}CPE_DM_t;


typedef struct Multi_DM_DB
{
	HINT32	nId;
	HINT32	nEntry;
	HINT32	nEntryCount;
}Multi_DM_DB_t;

//tr069 process가 필요한 data를 관리하기 위한 db;
//Tr069 DataModel중 Octo에 없는 data들은 JSON DB를 이용하여 tr069 process에서 관리한다.
//임시 => 향후 Octo에서 관리할 경우 이부분에서 제외
//CPE에서 관리하는 DataModel들도 아래 DB에서 저장한다. Writable/Read Only 모두 CPE가 저장 하도록 한다.
typedef struct trport_object_default
{
	HCHAR							*parameterName;
	HCHAR							*defaultValue;
	HUINT32							ulen;
}trport_object_default_t;

trport_object_default_t s_HardcodingDefaultValue[] = {
	{"DeviceSummary", 					"", SIZE_1024},
	{"X_JCOM_SubscriberID", 			"", SIZE_256},
	{"X_JCOM_ServiceID", 				"", SIZE_256},
	{"X_JCOM_FamilyID", 				"", SIZE_256},
	{"X_JCOM_BusinessCode", 			"", SIZE_256},
	{"Description", 					"Humax_STB_YOUSEE", SIZE_256},
	{"ProductClass",					"YOUSEE", SIZE_64},
	{"HardwareVersion",					"1.00.01", SIZE_64},	
	{"AdditinalHardwareVersion",		"", SIZE_64},
	{"Username", 						"humax", SIZE_256},
	{"Password", 						"humax", SIZE_256},	
	{"PeriodicInformEnable",			"true", SIZE_32},
	{"PeriodicInformInterval", 			"200", SIZE_32},
	{"PeriodicInformTime", 				"2013-01-01T00:00:00", SIZE_32},
	{"ParameterKey", 					"", SIZE_32},
	{"ConnectionRequestURL", 			"", SIZE_256},
	{"ConnectionRequestUsername", 		"humax", SIZE_256},
	{"ConnectionRequestPassword", 		"humax", SIZE_256},
	{"UpgradesManaged", 				"false", SIZE_16},
	{"CombinedAccess", 					"false", SIZE_32},
	{"PeriodicInformationInterval",		"200", SIZE_32},
	{"PeriodicInformationTime",			"2013-01-01T00:00:00", SIZE_32},
	{"PermissionLevel", 				"1", SIZE_32},	
	{"DefaultActiveNotificationThrottle",	"0", SIZE_16},
	{"CWMPRetryMinimumWaitInterval", 		"5", SIZE_32},
	{"CWMPRetryIntervalMultiplier", 		"2000", SIZE_32},
	{"AutonomousTransferCompletePolicy.Enable", 	"true", SIZE_32},
	{"TransferTypeFilter", 					"Download", SIZE_16},
	{"FileTypeFilter", 					"1 Firmware Upgrade Image", SIZE_1024},

	{"IPPingDiagnostics.DiagnosticsState", 	"None", SIZE_32},
	{"IPPingDiagnostics.Host", 				"", SIZE_256},
	{"IPPingDiagnostics.NumberOfRepetitions", 	"30", SIZE_32},
	{"IPPingDiagnostics.Timeout", 			"5000", SIZE_32},
	{"IPPingDiagnostics.DataBlockSize", 	"64", SIZE_32},
	{"IPPingDiagnostics.DSCP", 				"0", SIZE_32},
	{"IPPingDiagnostics.SuccessCount", 		"0", SIZE_32},
	{"IPPingDiagnostics.FailureCount", 		"0", SIZE_32},
	{"IPPingDiagnostics.AverageResponseTime", 	"0", SIZE_32},
	{"IPPingDiagnostics.MinimumResponseTime", 	"0", SIZE_32},
	{"IPPingDiagnostics.MaximumResponseTime", 	"0", SIZE_32},


	{"TraceRouteDiagnostics.DiagnosticsState", 	"None", SIZE_16},
	{"TraceRouteDiagnostics.Host", 				"", SIZE_256},
	{"TraceRouteDiagnostics.Timeout", 			"5000", SIZE_16},
	{"TraceRouteDiagnostics.DataBlockSize", 	"60", SIZE_32},
	{"TraceRouteDiagnostics.MaxHopCount", 		"30", SIZE_16},
	{"TraceRouteDiagnostics.DSCP", 				"0", SIZE_16},
	{"TraceRouteDiagnostics.ResponseTime", 		"0", SIZE_16},
	{"TraceRouteDiagnostics.NumberOfRouteHops",	"0", SIZE_16},

	{"DownloadDiagnostics.DiagnosticsState", 	"None", SIZE_16},
	{"DownloadDiagnostics.Interface", 			"", SIZE_256},
	{"DownloadDiagnostics.DownloadURL", 		"", SIZE_256},
	{"DownloadDiagnostics.DSCP", 				"0", SIZE_16},
	{"DownloadDiagnostics.EthernetPriority", 	"0", SIZE_16}, // not supported
	{"DownloadDiagnostics.ROMTime", 			"2013-01-01T00:00:00", SIZE_32},
	{"DownloadDiagnostics.BOMTime", 			"2013-01-01T00:00:00", SIZE_32},
	{"DownloadDiagnostics.EOMTime", 			"2013-01-01T00:00:00", SIZE_32},
	{"DownloadDiagnostics.TestBytesReceived", 	"0", SIZE_16},
	{"DownloadDiagnostics.TotalBytesReceived", 	"0", SIZE_16},

	{"UploadDiagnostics.DiagnosticsState", 		"None", SIZE_16},
	{"UploadDiagnostics.Interface", 			"", SIZE_256},
	{"UploadDiagnostics.UploadURL", 			"", SIZE_256},
	{"UploadDiagnostics.DSCP", 					"0", SIZE_16},
	{"UploadDiagnostics.EthernetPriority", 		"0", SIZE_16},
	{"UploadDiagnostics.TestFileLength", 		"0", SIZE_16},
	{"UploadDiagnostics.ROMTime", 				"2013-01-01T00:00:00", SIZE_32},
	{"UploadDiagnostics.BOMTime", 				"2013-01-01T00:00:00", SIZE_32},
	{"UploadDiagnostics.EOMTime", 				"2013-01-01T00:00:00", SIZE_32},
	{"UploadDiagnostics.TotalBytesSent", 		"", SIZE_16},

	{"NSLookupDiagnostics.DiagnosticsState", 	"None", SIZE_16},
	{"NSLookupDiagnostics.Interface", 			"", SIZE_256},
	{"NSLookupDiagnostics.HostName", 			"", SIZE_256},
	{"NSLookupDiagnostics.DNSServer", 			"", SIZE_256},
	{"NSLookupDiagnostics.Timeout", 			"0", SIZE_16},
	{"NSLookupDiagnostics.NumberOfRepetitions", 	"0", SIZE_16},
	{"NSLookupDiagnostics.SuccessCount", 		"0", SIZE_16},
	{"NSLookupDiagnostics.ResultNumberOfEntries", 		"0", SIZE_16},

	{"IPPingDiagnostics.JCOM.DiagnosticsState", 		"None", SIZE_16},
	{"IPPingDiagnostics.JCOM.Host", 		"", SIZE_256},
	{"IPPingDiagnostics.JCOM.NumberOfRepetitions", 		"30", SIZE_16},
	{"IPPingDiagnostics.JCOM.Timeout", 		"5000", SIZE_16},
	{"IPPingDiagnostics.JCOM.DataBlockSize", 		"64", SIZE_32},
	{"IPPingDiagnostics.JCOM.SuccessCount", 		"0", SIZE_32},
	{"IPPingDiagnostics.JCOM.FailureCount", 		"0", SIZE_32},
	{"IPPingDiagnostics.JCOM.AverageResponseTime", 		"0", SIZE_32},
	{"IPPingDiagnostics.JCOM.MinimumResponseTime", 		"0", SIZE_32},
	{"IPPingDiagnostics.JCOM.MaximumResponseTime", 		"0", SIZE_32},

	{"WebBrowserURLInput", 		"false", SIZE_16},
	{"SelfTestDiagnostics.DiagnosticsState", 		"None", SIZE_16},
	{"SoftwareModules.ExecEnv.Enable", 		"true", SIZE_16},

	{"STBConfigDownload", 		"0", SIZE_256},

	{"AMConfigurationName", 		"", SIZE_256},
	{"FirstUseDate", 		"0", SIZE_32},
	{"ProvisioningCode", 		"0", SIZE_64},
#if PRODUCTION_SERVER
	{"URL", 						PRODUCT_ACS_SERVER, 	SIZE_128},	// TDC Production ACS
	{"UpgradeProductionServer", 	"true", 				SIZE_128},	// In STB side, Server URL can be change 
#else
	{"URL", 					TEST_ACS_SERVER, SIZE_128},	// TDC test ACS
	{"UpgradeProductionServer", 	"false", 				SIZE_128},	// Do not need to change in test mode
#endif

	{NULL, NULL, 0}
};


#define ___STATIC_VARIABLES___

static	HxUDBHandle		_trdbHandle = NULL;
static	HxUDBHandle		_dldbHandle = NULL;

static	Multi_DM_DB_t stMultiDMEntryDB[eMULTI_DM_DBIndex_MAX];
static	CPE_DM_t	stCpeDm;


#define _____STATIC_FUNCTION________________________________________
HERROR	rema_DM_getTextData(HCHAR *token, HCHAR *text);
HERROR	rema_DM_setTextData(HCHAR *token, HCHAR *text);

HERROR	TRP_DB_CPE_Flush_DMObject(HINT32 nOpt, HINT32 nEntry);

STATIC void local_trp_cpe_InitCpeContext(void);


#define ___INTERNAL_FUNCTIONS___

static HERROR rema_DM_LoadDefaultTRDB(void)
{
	trport_object_default_t	*objectloop;

	REM_ENTER;
	RemLOG_Print("%p\n", _trdbHandle);

	objectloop = s_HardcodingDefaultValue;
	while(objectloop->parameterName)
	{
		RemLOG_Print("parameterName=[%-32s], value=[%s]\n", objectloop->parameterName, objectloop->defaultValue);
		rema_DM_setTextData(objectloop->parameterName, objectloop->defaultValue);
		objectloop++;
	}

	REM_LEAVE(ERR_OK);
}

static HERROR rema_DM_LoadDefaultDNLDB(void)
{
	HUINT8					i;
	trport_download_info_t	stDefaultData;

	REM_ENTER;
	RemLOG_Print(_dldbHandle);

	rema_DM_SetResetFlag(eFLAG_NEED_TO_RESET);
	rema_DM_SetPrevVersion(DEFAULT_PREV_SW_VERSION);

	VK_memset (&stDefaultData, 0x00, sizeof (trport_download_info_t));
	for(i=eDOWNLOAD_TYPE_FIRMWARE; i<eDOWNLOAD_TYPE_MAX; i++)
	{
		rema_DM_StoreDownloadInfo(i, &stDefaultData);
	}

	REM_LEAVE(ERR_OK);
}


#define ___PUBLIC_FUNCTIONS___

HERROR rema_DM_StoreDownloadInfo(HUINT32 ulId, trport_download_info_t *stInfo)
{
	HCHAR	szBuffer[64];

	HLIB_UDB_ClearStartNode(_dldbHandle);

	HxSTD_snprintf(szBuffer, 64, "%02d/%s", ulId, TRPORT_DB_DOWNLOADID_STATE);
	HLIB_UDB_SetValue(_dldbHandle, szBuffer, stInfo->state);

	HxSTD_snprintf(szBuffer, 64, "%02d/%s", ulId, TRPORT_DB_DOWNLOADID_FILEPATH);
	HLIB_UDB_SetText(_dldbHandle, szBuffer, stInfo->filepath);

	HxSTD_snprintf(szBuffer, 64, "%02d/%s", ulId, TRPORT_DB_DOWNLOADID_FILENAME);
	HLIB_UDB_SetText(_dldbHandle, szBuffer, stInfo->filename);

	HxSTD_snprintf(szBuffer, 64, "%02d/%s", ulId, TRPORT_DB_DOWNLOADID_FILESIZE);
	HLIB_UDB_SetValue(_dldbHandle, szBuffer, stInfo->filesize);

	HLIB_UDB_Sync(_dldbHandle);
	REM_LEAVE(ERR_OK);
}


HERROR	rema_DM_getTextData(HCHAR *token, HCHAR *text)
{
	HCHAR	szBuffer[SIZE_64];

	HxSTD_snprintf(szBuffer, SIZE_64, "%s", token);
	HLIB_UDB_GetText(_trdbHandle, szBuffer, text, 0);

	return ERR_OK;
}

HERROR	rema_DM_setTextData(HCHAR *token, HCHAR *text)
{
	HCHAR	szBuffer[SIZE_64];

	HxSTD_snprintf(szBuffer, SIZE_64, "%s", token);
	HLIB_UDB_SetText(_trdbHandle, szBuffer, text);
	HLIB_UDB_Sync(_trdbHandle);

	return ERR_OK;
}

void rema_DM_SetPrevVersion(HCHAR *pszVer)
{
	HLIB_UDB_ClearStartNode(_dldbHandle);
	HLIB_UDB_SetText(_dldbHandle, TRPORT_DB_DOWNLOADID_PREVIOUS_VERSION, pszVer);
}

HERROR rema_DM_ProcessACSURL(void)
{
	HERROR hErr = ERR_OK;
	HCHAR	szUpgradeACSURL[8] = {0, };
	HCHAR	szACSURL[256] = {0, };

	rema_DM_getTextData(TRPORT_DB_TR069_UPGRADE_ACSURL, szUpgradeACSURL);

#if PRODUCTION_SERVER
	// File is existed, but szUpgradeACSURL flag is not exist. ( for drop 1.0 release of TDC )
	if(!strcmp(szUpgradeACSURL, ""))
	{
		rema_DM_setTextData("URL", PRODUCT_ACS_SERVER);	
		rema_DM_setTextData(TRPORT_DB_TR069_UPGRADE_ACSURL, "false");
	}
	else
	{
		// File is not existed ( new STB or Factory Default )
		if(!strcmp(szUpgradeACSURL, "true"))
		{
			rema_DM_setTextData("URL", PRODUCT_ACS_SERVER);
			rema_DM_setTextData(TRPORT_DB_TR069_UPGRADE_ACSURL, "false");	
		}
		else
		{
			// if 'UpgradeProductionServer' is false, do not anything ( included situation that even though set 'URL' parameter by ACS ) 
		}
	}
#else
	// In Test mode, URL should be set Test Server. but In urgent situation, server can be change URL
	rema_DM_getTextData("URL", szACSURL);
	if(!strcmp(szACSURL, TEST_ACS_SERVER))
	{
		rema_DM_setTextData("URL", TEST_ACS_SERVER);
	}
	else
	{
		rema_DM_setTextData("URL", PRODUCT_ACS_SERVER);
	}
	// When URL is changed to Production server, using this flag URL is changed.
	rema_DM_setTextData(TRPORT_DB_TR069_UPGRADE_ACSURL, "true");	
#endif

	rema_DM_getTextData("URL", szACSURL);
	rema_DM_getTextData(TRPORT_DB_TR069_UPGRADE_ACSURL, szUpgradeACSURL);

	RemLOG_Info("URL : %s, UpgradeProductionServer : %s\n", szACSURL, szUpgradeACSURL);

	REM_LEAVE(hErr);
}

#define _____GLOBAL_FUNCTION________________________________________

HERROR rema_DM_Init(void)
{
	HBOOL		bNewFile, bReset;
	HERROR		hErr;
	REM_ENTER;

	HLIB_DIR_Make(TRPORT_DB_PATH);
	_trdbHandle = HLIB_UDB_CreateInstance(TRPORT_DB_PATH, TRPORT_TR_DB_FILENAME, &bNewFile, TRPORT_DB_BACKUP_PATH, 0);
	if (bNewFile)
	{
		RemLOG_Print("[%s:%d] make new TR069 DB FILE[%s%s]\n",__FUNCTION__,__LINE__,TRPORT_DB_PATH, TRPORT_TR_DB_FILENAME);
		rema_DM_LoadDefaultTRDB();
	}

	_dldbHandle = HLIB_UDB_CreateInstance(TRPORT_DB_PATH, TRPORT_DL_DB_FILENAME, &bNewFile, TRPORT_DB_BACKUP_PATH, 0);
	hErr = rema_DM_GetResetFlag(&bReset);
	if (bNewFile || bReset == eFLAG_NEED_TO_RESET)
	{
		RemLOG_Print("[%s:%d] make new download DB FILE[%s%s]\n",__FUNCTION__,__LINE__,TRPORT_DB_PATH, TRPORT_DL_DB_FILENAME);
		rema_DM_LoadDefaultDNLDB();
	}
	else//download db도 존재하고, reset flag가 eFLAG_NO_NEED_TO_RESET일경우  => library가 reset을 시킨경우
	{
		rema_DM_SetResetFlag(eFLAG_NEED_TO_RESET);
	}

	rema_DM_ProcessACSURL();

	//Multi Instance Index DB
	HxSTD_MemSet(stMultiDMEntryDB, 0, sizeof(Multi_DM_DB_t));

	/*Default Set*/
	rema_DM_MultiInstance_SetEntry(eTR135A1_Device_Services_STBServiceNumberOfEntries,1);
	rema_DM_MultiInstance_SetEntry(eTR157A3_Device_SmartCardReaderNumberOfEntries,2);

	local_trp_cpe_InitCpeContext();


	REM_LEAVE(ERR_OK);

}

void rema_DM_Deinit(void)
{
	REM_FUNC_ENTER;


	REM_FUNC_LEAVE;
}

HERROR rema_DM_GetResetFlag(HBOOL *flag)
{
	REM_LEAVE(HLIB_UDB_GetValue(_dldbHandle, TRPORT_DB_DOWNLOADID_RESET, flag));
}

void rema_DM_SetResetFlag(HBOOL flag)
{
	HLIB_UDB_ClearStartNode(_dldbHandle);
	HLIB_UDB_SetValue(_dldbHandle, TRPORT_DB_DOWNLOADID_RESET, flag);
}

// Multi Instance Entry DB
HINT32	rema_DM_MultiInstance_GetIndex(eTRXDMInstancetype_e eId)
{
	MULTI_DM_DBIndex_e	eDbIndex;

	switch(eId)
	{
		case eTR157A3_Device_SmartCardReaderNumberOfEntries:
			eDbIndex = eTR157A3_Device_SmartCardReader_M_Index;
			break;
		case eTR135A1_Device_Services_STBServiceNumberOfEntries:
			eDbIndex = eTR135A1_Device_Services_STBService_M_Index;
			break;
		case eTR140A1_Device_Services_StorageServiceNumberOfEntries:
			eDbIndex = eTR140A1_DSStorageSvc_M_Index;
			break;
		case eTR135A1_DSSTB_1_Capa_VideoDecoder_MPEG2Part2_ProfileLevelNumberOfEntries:
			eDbIndex = eTR135A1_DSSTB_1_Capa_VideoDecoder_MPEG2Part2_ProfileLevel_M_Index;
			break;
		case eTR135A1_DSSTB_1_Capa_VideoDecoder_MPEG4Part2_ProfileLevelNumberOfEntries:
			eDbIndex = eTR135A1_DSSTB_1_Capa_VideoDecoder_MPEG4Part2_ProfileLevel_M_Index;
			break;
		case eTR135A1_DSSTB_1_Capa_VideoDecoder_MPEG4Part10_ProfileLevelNumberOfEntries:
			eDbIndex = eTR135A1_DSSTB_1_Capa_VDec_MPEG4Part10_ProfileLevel_M_Index;
			break;
		case eTR135A1_DSSTB_1_Comp_FrontEndNumberOfEntries:
			eDbIndex = eTR135A1_DSSTB_1_Comp_FrontEnd_M_Index;
			break;
		case eTR135A1_DSSTB_1_Comp_PVR_StorageNumberOfEntries:
			eDbIndex = eTR135A1_DSSTB_1_Comp_PVR_Storage_M_Index;
			break;
		case eTR135A1_DSSTB_1_Comp_AudioDecoderNumberOfEntries:
			eDbIndex = eTR135A1_DSSTB_1_Comp_AudioDecoder_M_Index;
			break;
		case eTR135A1_DSSTB_1_Comp_VideoDecoderNumberOfEntries:
			eDbIndex = eTR135A1_DSSTB_1_Comp_VideoDecoder_M_Index;
			break;
		case eTR135A1_DSSTB_1_Comp_AudioOutputNumberOfEntries:
			eDbIndex = eTR135A1_DSSTB_1_Comp_AudioOutput_M_Index;
			break;
		case eTR135A1_DSSTB_1_Comp_SPDIFNumberOfEntries:
			eDbIndex = eTR135A1_DSSTB_1_Comp_SPDIF_M_Index;
			break;
		case eTR135A1_DSSTB_1_Comp_VideoOutputNumberOfEntries:
			eDbIndex = eTR135A1_DSSTB_1_Comp_VideoOutput_M_Index;
			break;
		case eTR135A1_DSSTB_1_Comp_HDMINumberOfEntries:
			eDbIndex = eTR135A1_DSSTB_1_Comp_HDMI_M_Index;
			break;
		case eTR135A1_DSSTB_1_Comp_CANumberOfEntries:
			eDbIndex = eTR135A1_DSSTB_1_Comp_CA_M_Index;
			break;
		case eTR135A1_DSSTB_1_AVS_AVStreamNumberOfEntries:
			eDbIndex = eTR135A1_DSSTB_1_AVS_AVStream_M_Index;
			break;
		case eTR135A1_DSSTB_1_AVP_AVPlayerNumberOfEntries:
			eDbIndex = eTR135A1_DSSTB_1_AVP_AVPlayer_M_Index;
			break;
		case eTR135A1_DSSTB_1_JCOM_ClientEquipmentNumberOfEntries:
			eDbIndex = eTR135A1_DSSTB_1_JCOM_ClientEquipment_M_Index;
			break;
		case eTR135A1_DSSTB_1_JCOM_RemoteEquipmentNumberOfEntries:
			eDbIndex = eTR135A1_DSSTB_1_JCOM_RemoteEquipment_M_Index;
			break;
		case eTR135A1_DSSTB_1_JCOM_PurchasedPPV_HistoryNumberOfEntries:
			eDbIndex = eTR135A1_DSSTB_1_JCOM_PurchasedPPV_History_M_Index;
			break;
		case eTR135A1_DSSTB_1_JCOM_ISDBC_ServiceListDatabaseNumberOfEntries:
			eDbIndex = eTR135A1_DSSTB_1_JCOM_ISDBC_ServiceListDatabase_M_Index;
			break;
		case eTR157A3_Device_DeviceInfo_VendorConfigFileNumberOfEntries:
			eDbIndex = eTR157A3_Device_DeviceInfo_VendorConfigFile_M_Index;
			break;
		case eTR157A3_Device_DeviceInfo_SupportedDataModelNumberOfEntries:
			eDbIndex = eTR157A3_Device_DeviceInfo_SupportedDataMode_M_Index;
			break;
		case eTR157A3_Device_DeviceInfo_ProcessorNumberOfEntries:
			eDbIndex = eTR157A3_Device_DeviceInfo_Processor_M_Index;
			break;
		case eTR157A3_Device_DeviceInfo_VendorLogFileNumberOfEntries:
			eDbIndex = eTR157A3_Device_DeviceInfo_VendorLogFile_M_Index;
			break;
		case eTR157A3_Device_DeviceInfo_ProcessStatus_ProcessNumberOfEntries:
			eDbIndex = eTR157A3_Device_DeviceInfo_ProcessStatus_Process_M_Index;
			break;
		case eTR157A3_Device_UPnP_Discovery_RootDeviceNumberOfEntries:
			eDbIndex = eTR157A3_Device_UPnP_Discovery_RootDevice_M_Index;
			break;
		case eTR157A3_Device_UPnP_Discovery_DeviceNumberOfEntries:
			eDbIndex = eTR157A3_Device_UPnP_Discovery_Device_M_Index;
			break;
		case eTR157A3_Device_NSLookupDiagnostics_ResultNumberOfEntries:
			eDbIndex = eTR157A3_Device_NSLookupDiagnostics_Result_M_Index;
			break;
		case eTR157A3_Device_USBHosts_HostNumberOfEntries:
			eDbIndex = eTR157A3_DUSBHosts_Host_M_Index;
			break;
		case eTR157A3_Device_PeriodicStatistics_SampleSetNumberOfEntries:
			eDbIndex = eTR157A3_Device_PeriodicStatistics_SampleSet_M_Index;
			break;
		case eTR135A1_DSSTB_1_JCOM_ISDBC_ServiceListDatabase_M_LCNOfEntries:
			eDbIndex = eTR135A1_DSSTB_1_JCOM_ISDBC_ServiceListDatabase_M_LogicalChannel_M_Index;
			break;
		case eTR140A1_DSStorageSvc_M_PhysicalMediumNumberOfEntries:
			eDbIndex = eTR140A1_DSStorageSvc_M_PhysicalMedium_M_Index;
			break;
		case eTR140A1_DSStorageSvc_M_LogicalVolumeNumberOfEntries:
			eDbIndex = eTR140A1_DSStorageSvc_M_LogicalVolume_M_Index;
			break;
		case eTR140A1_DSStorageSvc_M_USB_HDDNumberOfEntries:
			eDbIndex = eTR140A1_DSStorageSvc_M_X_JCOM_USB_HDD_M_Index;
			break;
		case eTR157A3_Device_SmartCardReader_M_SmartCard_X_JCOM_GroupIdNumberOfEntries:
			eDbIndex = eTR157A3_Device_SmartCardReader_M_SmartCard_X_JCOM_GroupID_M_Index;
			break;
		case eTR157A3_Device_USBHosts_Host_M_DeviceNumberOfEntries:
			eDbIndex = eTR157A3_DUSBHosts_HDev_M_Index;
			break;
		case eTR157A3_DUSBHosts_HDev_M_ConfigurationNumberOfEntries:
			eDbIndex = eTR157A3_DUSBHosts_HDev_Configuration_M_Index;
			break;
		case eTR157A3_DUSBHosts_HDev_Configuration_M_InterfaceNumberOfEntries:
			eDbIndex = eTR157A3_DUSBHosts_HDev_Configuration_Interface_M_Index;
			break;
		case eTR157A3_Device_PeriodicStatistics_SampleSet_M_ParameterNumberOfEntries:
			eDbIndex = eTR157A3_Device_PeriodicStatistics_SampleSet_Parameter_M_Index;
			break;
		case eTR157A3_Device_LAN_TraceRouteDiagnostics_NumberOfRouteHops:
			eDbIndex = eTR157A3_Device_LAN_TraceRouteDiagnostics_M_Index;
			break;
		case eTR157A3_Device_UPnP_Discovery_ServiceNumberOfEntries:
			eDbIndex = eTR157A3_Device_UPnP_Discovery_Service_M_Index;
			break;
		case eTR157A3_Device_SoftwareModules_ExecEnvNumberOfEntries:
			eDbIndex = eTR157A3_Device_SoftwareModules_ExecEnv_M_Index;
			break;
		case eTR181A1_Device_WiFi_AccessPointNumberOfEntries:
			eDbIndex = eTR181A1_Device_WiFi_AccessPoint_M_Index;
			break;
		default:
			RemLOG_Print("[%s] not multi instance value(%d)\r\n",__func__,eId);
			return (HINT32)0;
			break;
	}

	return (HINT32)eDbIndex;
}

HINT32	rema_DM_MultiInstance_GetEntry(eTRXDMInstancetype_e eId)
{
	HINT32 nIndex, nResult = -1;

	nIndex = rema_DM_MultiInstance_GetIndex(eId);
	if(nIndex != -1)
	{
		RemLOG_Print("get stMultiDMEntryDB[%d].nEntry(%d)\n",nIndex,stMultiDMEntryDB[nIndex].nEntry);

		nResult = stMultiDMEntryDB[nIndex].nEntry;
	}
	return nResult;
}


HERROR	rema_DM_MultiInstance_SetEntry(eTRXDMInstancetype_e eId, HINT32 nEntry)
{
	HINT32 nIndex;

	nIndex = rema_DM_MultiInstance_GetIndex(eId);

	stMultiDMEntryDB[nIndex].nEntry = nEntry;

	RemLOG_Print("set stMultiDMEntryDB[%d].nEntry(%d), nEntry(%d)\n",nIndex,stMultiDMEntryDB[nIndex].nEntry,nEntry);

	return ERR_OK;
}

HERROR	rema_DM_MultiInstance_SetEntryCount(eTRXDMInstancetype_e eId, HINT32 nEntry)
{
	HINT32 nIndex;

	nIndex = rema_DM_MultiInstance_GetIndex(eId);
	if(nIndex != -1)
	{
		stMultiDMEntryDB[nIndex].nEntryCount = nEntry;

		RemLOG_Print("setcount stMultiDMEntryDB[%d].nEntryCount(%d), nEntry(%d)\n",nIndex,stMultiDMEntryDB[nIndex].nEntryCount,nEntry);
	}
	return ERR_OK;
}

STATIC CPE_DM_t* local_trp_cpe_GetCpeContext(void)
{
	return &stCpeDm;
}
STATIC void local_trp_cpe_InitCpeContext(void)
{
	HxSTD_MemSet(&stCpeDm, 0x00, sizeof(CPE_DM_t));
}

HERROR	TRP_DB_CPE_Flush_DMObject(HINT32 nOpt, HINT32 nEntry)
{
	CPE_DM_t* pstCpe = local_trp_cpe_GetCpeContext();
	CPE_DM_TYPE_e eType = (CPE_DM_TYPE_e)nOpt;

	switch(eType)
	{
		case CDT_SupportedDataModel:
			if(pstCpe->pstSppportedDM != NULL)
			{
				HLIB_STD_MemFree(pstCpe->pstSppportedDM);
				pstCpe->pstSppportedDM = NULL;
			}
			break;
		case CDT_TraceRouteHops:
			if(pstCpe->pstRouteHops != NULL)
			{
				HLIB_STD_MemFree(pstCpe->pstRouteHops);
				pstCpe->pstRouteHops = NULL;
			}
			break;

		case CDT_NSLookupDiagResult:
			if(pstCpe->pstNSLookupResult != NULL)
			{
				HLIB_STD_MemFree(pstCpe->pstNSLookupResult);
				pstCpe->pstNSLookupResult = NULL;
			}
			break;
		default:
			break;
	}

	return ERR_OK;
}
HERROR	TRP_DB_CPE_Build_DMObject(HINT32 nOpt, HINT32 *pnEntry)
{
	CPE_DM_t* pstCpe = local_trp_cpe_GetCpeContext();
	CPE_DM_TYPE_e eType = (CPE_DM_TYPE_e)nOpt;
	HINT32 nOldEntry = 0;
	HINT32 nEntry = *pnEntry;

	RemLOG_Print("TRP_DB_CPE_Build_DMObject nOpt(%d), nEntry(%d)\r\n",nOpt,nEntry);

	switch(eType)
	{
		case CDT_SupportedDataModel:
			nOldEntry = rema_DM_MultiInstance_GetEntry(eTR157A3_Device_DeviceInfo_SupportedDataModelNumberOfEntries);
			if(nOldEntry > 0)
			{
				TRP_DB_CPE_Flush_DMObject(eType,nOldEntry);
			}
			pstCpe->pstSppportedDM = (SupportedDataModel_DM_t*)HLIB_STD_MemAlloc(nEntry*sizeof(SupportedDataModel_DM_t));
			break;

		case CDT_TraceRouteHops:
			nOldEntry = rema_DM_MultiInstance_GetEntry(eTR157A3_Device_LAN_TraceRouteDiagnostics_NumberOfRouteHops);
			if(nOldEntry > 0)
			{
				TRP_DB_CPE_Flush_DMObject(eType,nOldEntry);
			}
			pstCpe->pstRouteHops = (RouteHops_DM_t*)HLIB_STD_MemAlloc(nEntry*sizeof(RouteHops_DM_t));
			break;

		case CDT_NSLookupDiagResult:
			nOldEntry = rema_DM_MultiInstance_GetEntry(eTR157A3_Device_NSLookupDiagnostics_ResultNumberOfEntries);
			if(nOldEntry > 0)
			{
				TRP_DB_CPE_Flush_DMObject(eType,nOldEntry);
			}
			pstCpe->pstNSLookupResult = (NSLookupDiag_Result_DM_t*)HLIB_STD_MemAlloc(nEntry*sizeof(NSLookupDiag_Result_DM_t));
			break;
		default:
			break;
	}
	return ERR_OK;
}
HERROR	TRP_DB_CPE_Set_DMObject(HINT32 nOpt1, HINT32 nOpt2, HINT32 nIndex, HCHAR *pszValue)
{
	CPE_DM_t* pstCpe = local_trp_cpe_GetCpeContext();
	CPE_DM_TYPE_e eType = (CPE_DM_TYPE_e)nOpt1;
	NSLookupDiag_Result_Data_e eSubType = (NSLookupDiag_Result_Data_e)nOpt2;

	SupportedDataModel_DM_t *pstSpp=NULL;
	RouteHops_DM_t *pstRou=NULL;
	NSLookupDiag_Result_DM_t *pstNSL=NULL;


	if(pstCpe == NULL || pszValue == NULL)
	{
		return ERR_FAIL;
	}

	RemLOG_Print("%s,nOpt1(%d),nOpt2(%d),nIndex(%d),pszValue(%s)\n",__func__,nOpt1,nOpt2,nIndex,pszValue);

	switch(eType)
	{
		case CDT_SupportedDataModel:
			pstSpp = pstCpe->pstSppportedDM;
			if( pstSpp != NULL)
			{
				pstSpp = (pstSpp+nIndex);

				switch(eSubType)
				{
					case SDM_URL:
						HxSTD_StrNCpy(pstSpp->URL, pszValue,(SIZE_256-1));
						break;
					case SDM_URN:
						HxSTD_StrNCpy(pstSpp->URN, pszValue,(SIZE_256-1));
						break;
					case SDM_Features:
						HxSTD_StrNCpy(pstSpp->Features, pszValue,(SIZE_256-1));
						break;
					default:
						break;
				}
			}
			break;
		case CDT_TraceRouteHops:
			pstRou = pstCpe->pstRouteHops;
			if (pstRou != NULL)
			{
				pstRou = (pstRou+nIndex);

				HxSTD_StrNCpy(pstRou->HopHost, pszValue,(SIZE_256-1));
			}
			break;
		case CDT_NSLookupDiagResult:
			pstNSL = pstCpe->pstNSLookupResult;
			if(pstNSL != NULL)
			{
				pstNSL = (pstNSL+nIndex);

				switch(eSubType)
				{
					case NRD_Status:
						HxSTD_StrNCpy(pstNSL->Status, pszValue,(SIZE_16-1));
						break;
					case NRD_AnswerType:
						HxSTD_StrNCpy(pstNSL->AnswerType, pszValue,(SIZE_16-1));
						break;
					case NRD_HostNameReturned:
						HxSTD_StrNCpy(pstNSL->HostNameReturned, pszValue,(SIZE_256-1));
						break;
					case NRD_IPAddresses:
						HxSTD_StrNCpy(pstNSL->IPAddresses, pszValue,(SIZE_256-1));
						break;
					case NRD_DNSServerIP:
						HxSTD_StrNCpy(pstNSL->DNSServerIP, pszValue,(SIZE_256-1));
						break;
					case NRD_ResponseTime:
						HxSTD_StrNCpy(pstNSL->ResponseTime, pszValue,(SIZE_16-1));
						break;
					default:
						break;
				}
			}
			break;
		default:
			break;
	}
	return ERR_OK;
}
HERROR	TRP_DB_CPE_Get_DMObject(HINT32 nOpt1, HINT32 nOpt2, HINT32 nIndex, HCHAR *pszValue)
{
	CPE_DM_t* pstCpe = local_trp_cpe_GetCpeContext();
	CPE_DM_TYPE_e eType = (CPE_DM_TYPE_e)nOpt1;
	NSLookupDiag_Result_Data_e eSubType = (NSLookupDiag_Result_Data_e)nOpt2;

	SupportedDataModel_DM_t *pstSpp=NULL;
	RouteHops_DM_t *pstRou=NULL;
	NSLookupDiag_Result_DM_t *pstNSL=NULL;

	if(pstCpe == NULL || pszValue == NULL)
	{
		return ERR_FAIL;
	}

	switch(eType)
	{
		case CDT_SupportedDataModel:
			pstSpp = pstCpe->pstSppportedDM;
			if(pstSpp != NULL)
			{
				pstSpp = (pstSpp+nIndex);

				switch(eSubType)
				{
					case SDM_URL:
						HxSTD_StrNCpy(pszValue, pstSpp->URL,(SIZE_256-1));
						break;
					case SDM_URN:
						HxSTD_StrNCpy(pszValue, pstSpp->URN,(SIZE_256-1));
						break;
					case SDM_Features:
						HxSTD_StrNCpy(pszValue, pstSpp->Features,(SIZE_256-1));
						break;
					default:
						return ERR_FAIL;
						break;
				}
			}
			break;
		case CDT_TraceRouteHops:
			pstRou = pstCpe->pstRouteHops;
			if(pstRou != NULL)
			{
				pstRou = (pstRou+nIndex);

				HxSTD_StrNCpy(pszValue, pstRou->HopHost,(SIZE_256-1));
			}
			break;
		case CDT_NSLookupDiagResult:
			pstNSL = pstCpe->pstNSLookupResult;
			if(pstNSL != NULL)
			{
				pstNSL = (pstNSL+nIndex);

				switch(eSubType)
				{
					case NRD_Status:
						HxSTD_StrNCpy(pszValue, pstNSL->Status,(SIZE_16-1));
						break;
					case NRD_AnswerType:
						HxSTD_StrNCpy(pszValue, pstNSL->AnswerType,(SIZE_16-1));
						break;
					case NRD_HostNameReturned:
						HxSTD_StrNCpy(pszValue, pstNSL->HostNameReturned,(SIZE_256-1));
						break;
					case NRD_IPAddresses:
						HxSTD_StrNCpy(pszValue, pstNSL->IPAddresses,(SIZE_256-1));
						break;
					case NRD_DNSServerIP:
						HxSTD_StrNCpy(pszValue, pstNSL->DNSServerIP,(SIZE_256-1));
						break;
					case NRD_ResponseTime:
						HxSTD_StrNCpy(pszValue, pstNSL->ResponseTime,(SIZE_16-1));
						break;
					default:
						return ERR_FAIL;
						break;
				}
			}
			break;
		default:
			return ERR_FAIL;
			break;
	}

	RemLOG_Print("%s,nOpt1(%d),nOpt2(%d),nIndex(%d),pszValue(%s)\n",__func__,nOpt1,nOpt2,nIndex,pszValue);

	return ERR_OK;
}
void	TRP_DB_CPE_PrintDB(void)
{
	HINT32 nEntry = 0, i = 0;
	CPE_DM_t* pstCpe = local_trp_cpe_GetCpeContext();
	SupportedDataModel_DM_t *pstSpp=NULL;
	RouteHops_DM_t *pstRou=NULL;
	NSLookupDiag_Result_DM_t *pstNSL=NULL;

	if(pstCpe == NULL)
	{
		return;
	}

	nEntry = rema_DM_MultiInstance_GetEntry(eTR157A3_Device_DeviceInfo_SupportedDataModelNumberOfEntries);
	if(nEntry > 0)
	{
		pstSpp = pstCpe->pstSppportedDM;
		for(i = 0; i < nEntry; i++)
		{
			if(pstSpp == NULL)	break;

			RemLOG_Print("SupportedDataModel URL(%s)\n",pstSpp->URL);
			RemLOG_Print("SupportedDataModel URN(%s)\n",pstSpp->URN);
			RemLOG_Print("SupportedDataModel Features(%s)\n",pstSpp->Features);
			pstSpp = (pstSpp+i);
		}
	}
	nEntry = rema_DM_MultiInstance_GetEntry(eTR157A3_Device_LAN_TraceRouteDiagnostics_NumberOfRouteHops);
	if(nEntry > 0)
	{
		pstRou = pstCpe->pstRouteHops;
		for(i = 0; i < nEntry; i++)
		{
			if(pstRou == NULL)	break;

			RemLOG_Print("TraceRouteDiagnostics HopHost(%s)\n",pstRou->HopHost);
			pstRou = (pstRou+i);
		}
	}
	nEntry = rema_DM_MultiInstance_GetEntry(eTR157A3_Device_NSLookupDiagnostics_ResultNumberOfEntries);
	if(nEntry > 0)
	{
		pstNSL = pstCpe->pstNSLookupResult;
		for(i = 0; i < nEntry; i++)
		{
			if(pstNSL == NULL)	break;

			RemLOG_Print("NSLookupDiagnostics_Result Status(%s)\n",pstNSL->Status);
			RemLOG_Print("NSLookupDiagnostics_Result SAnswerType(%s)\n",pstNSL->AnswerType);
			RemLOG_Print("NSLookupDiagnostics_Result SHostNameReturned(%s)\n",pstNSL->HostNameReturned);
			RemLOG_Print("NSLookupDiagnostics_Result SIPAddresses(%s)\n",pstNSL->IPAddresses);
			RemLOG_Print("NSLookupDiagnostics_Result SDNSServerIP(%s)\n",pstNSL->DNSServerIP);
			RemLOG_Print("NSLookupDiagnostics_Result SResponseTime(%s)\n",pstNSL->ResponseTime);
			pstNSL = (pstNSL+i);
		}
	}
}



#define __________GET_FUNCTION__________
HERROR	rema_DM_getData_Device_DeviceSummary(HCHAR *pszOutput)
{
	RemLOG_Print("%p\n", _trdbHandle);
	CHECK_NULL(pszOutput);
	return rema_DM_getTextData("DeviceSummary", pszOutput);
}

HERROR	rema_DM_getData_DeviceInfo_X_JCOM_SubscriberID(HCHAR *pszOutput)
{
	RemLOG_Print("%p\n", _trdbHandle);
	CHECK_NULL(pszOutput);
	return rema_DM_getTextData("X_JCOM_SubscriberID", pszOutput);
}

HERROR	rema_DM_getData_DeviceInfo_X_JCOM_ServiceID(HCHAR *pszOutput)
{
	RemLOG_Print("%p\n", _trdbHandle);
	CHECK_NULL(pszOutput);
	return rema_DM_getTextData("X_JCOM_ServiceID", pszOutput);
}
HERROR	rema_DM_getData_DeviceInfo_X_JCOM_FamilyID(HCHAR *pszOutput)
{
	RemLOG_Print("%p\n", _trdbHandle);
	CHECK_NULL(pszOutput);
	return rema_DM_getTextData("X_JCOM_FamilyID", pszOutput);
}
HERROR	rema_DM_getData_DeviceInfo_X_JCOM_BusinessCode(HCHAR *pszOutput)
{
	RemLOG_Print("%p\n", _trdbHandle);
	CHECK_NULL(pszOutput);
	return rema_DM_getTextData("X_JCOM_BusinessCode", pszOutput);
}
HERROR	rema_DM_getData_DeviceInfo_Description(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
#if defined(CONFIG_OP_TDC)	
	HCHAR szDescription[DESCRIPTION_MAX_LEN] = {0, };	
#endif
	
	RemLOG_Print("%p\n", _trdbHandle);
	CHECK_NULL(pszOutput);

#if defined(CONFIG_OP_TDC)
	hErr = OAPI_SYSINFO_STB_GetUpdateDate(szDescription, DESCRIPTION_MAX_LEN);
	if(szDescription)
	{
		HxSTD_MemCopy(pszOutput, szDescription, HxSTD_StrLen(szDescription));
	}
#else
	hErr = rema_DM_getTextData("ProductClass", pszOutput);
#endif

	return hErr;
}

HERROR	rema_DM_getData_DeviceInfo_ProductClass(HCHAR *pszOutput)
{
	RemLOG_Print("%p\n", _trdbHandle);
	CHECK_NULL(pszOutput);
	return rema_DM_getTextData("ProductClass", pszOutput);
}

HERROR rema_DM_getData_DeviceInfo_HardwareVersion(HCHAR *pszOutput)
{
	RemLOG_Print("%p\n", _trdbHandle);
	CHECK_NULL(pszOutput);
	return rema_DM_getTextData("HardwareVersion", pszOutput);
}

HERROR rema_DM_getData_ManagementServer_Username(HCHAR *pszOutput)
{
	RemLOG_Print("%p\n", _trdbHandle);
	CHECK_NULL(pszOutput);
	return rema_DM_getTextData("Username", pszOutput);
}

HERROR rema_DM_getData_ManagementServer_Password(HCHAR *pszOutput)
{
	RemLOG_Print("%p\n", _trdbHandle);
	CHECK_NULL(pszOutput);
	return rema_DM_getTextData("Password", pszOutput);
}

HERROR rema_DM_getData_ManagementServer_Url(HCHAR *pszOutput)
{
	RemLOG_Print("%p\n", _trdbHandle);
	CHECK_NULL(pszOutput);
	return rema_DM_getTextData("URL", pszOutput);
}


HERROR rema_DM_getData_ManagementServer_PeriodicInformEnable(HCHAR *pszOutput)
{
	RemLOG_Print("%p\n", _trdbHandle);
	CHECK_NULL(pszOutput);
	return rema_DM_getTextData("PeriodicInformEnable", pszOutput);
}

HERROR rema_DM_getData_ManagementServer_PeriodicInformInterval(HCHAR *pszOutput)
{
	RemLOG_Print("%p\n", _trdbHandle);
	CHECK_NULL(pszOutput);
	return rema_DM_getTextData("PeriodicInformInterval", pszOutput);
}

HERROR rema_DM_getData_ManagementServer_PeriodicInformTime(HCHAR *pszOutput)
{
	RemLOG_Print("%p\n", _trdbHandle);
	CHECK_NULL(pszOutput);
	return rema_DM_getTextData("PeriodicInformTime", pszOutput);
}

HERROR rema_DM_getData_ManagementServer_ParameterKey(HCHAR *pszOutput)
{
	RemLOG_Print("%p\n", _trdbHandle);
	CHECK_NULL(pszOutput);
	return rema_DM_getTextData("ParameterKey", pszOutput);
}

HERROR rema_DM_getData_ManagementServer_ConnectionRequestURL(HCHAR *pszOutput)
{
#if 0
	HINT32 index;
	RDM_NETIFADDR_t	ptNetAddr;


	RDM_NETWORK_CONF_GetDefaultDev(&index);

	if (RDM_NETWORK_CONF_SetAddrInfo(index, (RDM_NETIFADDR_t*)&ptNetAddr) != ERR_OK)
	{
		HxLOG_Error("Getting the Address Information of Network is failed.\n");		
	}

	if (RDM_NETWORK_CONF_AddrN2A(ptNetAddr.ulIpAddr, pszOutput, RDM_IPADDR_LEN) != ERR_OK)
	{
		HxLOG_Error("Getting the IP Address is failed.\n");		
	}

	if (ptNetAddr.ulIpAddr != 0)
	{
			HxSTD_sprintf(pszOutput, "http://%s%s", ptNetAddr.ulIpAddr, ":9090/connreq");
			return ERR_OK;
	}
	
#endif
	return ERR_OK;

}



HERROR TRP157_Get_DevInfo_SupportedDataModel_M_URL(HCHAR *pszOutput)
{
	HINT32 nArg1 = 0;
	rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_DeviceInfo_SupportedDataModelNumberOfEntries,&nArg1);
	TRP_DB_CPE_Get_DMObject(CDT_SupportedDataModel,SDM_URL,nArg1,pszOutput);
	return ERR_OK;

}
HERROR TRP157_Get_DevInfo_SupportedDataModel_M_URN(HCHAR *pszOutput)
{
	HINT32 nArg1 = 0;
	rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_DeviceInfo_SupportedDataModelNumberOfEntries,&nArg1);
	TRP_DB_CPE_Get_DMObject(CDT_SupportedDataModel,SDM_URN,nArg1,pszOutput);
	return ERR_OK;
}
HERROR TRP157_Get_DevInfo_SupportedDataModel_M_Features(HCHAR *pszOutput)
{
	HINT32 nArg1 = 0;
	rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_DeviceInfo_SupportedDataModelNumberOfEntries,&nArg1);
	TRP_DB_CPE_Get_DMObject(CDT_SupportedDataModel,SDM_Features,nArg1,pszOutput);
	return ERR_OK;

}


HERROR TRP_Get_STBConfigDownloadResult(HBOOL *pbResult)
{
	HERROR hErr = ERR_OK;
	HCHAR szOutput[256] = {0,};
	RemLOG_Print("%p\n", _trdbHandle);
	CHECK_NULL(pbResult);

	hErr = rema_DM_getTextData("STBConfigDownload", szOutput);
	*pbResult = (HBOOL)PARAM_ATOI(szOutput);

	return hErr;
}
HERROR TRP_Get_SoftwareModules_ExecEnv_Enable(HCHAR *pszOutput)
{
	RemLOG_Print("%p\n", _trdbHandle);
	CHECK_NULL(pszOutput);
	return rema_DM_getTextData("SoftwareModules.ExecEnv.Enable", pszOutput);
}
HERROR TRP_Get_AMConfigurationName(HCHAR *pszOutput)
{
	RemLOG_Print("%p\n", _trdbHandle);
	CHECK_NULL(pszOutput);
	return rema_DM_getTextData("AMConfigurationName", pszOutput);
}
HERROR TRP_Get_FirstUseDate(HCHAR *pszOutput)
{
	RemLOG_Print("%p\n", _trdbHandle);
	CHECK_NULL(pszOutput);
	return rema_DM_getTextData("FirstUseDate", pszOutput);
}
HERROR TRP_Get_ProvisioningCode(HCHAR *pszOutput)
{
	RemLOG_Print("%p\n", _trdbHandle);
	CHECK_NULL(pszOutput);
	return rema_DM_getTextData("ProvisioningCode", pszOutput);
}


HERROR rema_DM_getData_ManagementServer_ConnectionRequestUsername(HCHAR *pszOutput)
{
	RemLOG_Print("%p\n", _trdbHandle);
	CHECK_NULL(pszOutput);
	return rema_DM_getTextData("ConnectionRequestUsername", pszOutput);
}

HERROR rema_DM_getData_ManagementServer_ConnectionRequestPassword(HCHAR *pszOutput)
{
	RemLOG_Print("%p\n", _trdbHandle);
	CHECK_NULL(pszOutput);
	return rema_DM_getTextData("ConnectionRequestPassword", pszOutput);
}

HERROR rema_DM_getData_ManagementServer_UpgradesManaged(HCHAR *pszOutput)
{
	RemLOG_Print("%p\n", _trdbHandle);
	CHECK_NULL(pszOutput);
	return rema_DM_getTextData("UpgradesManaged", pszOutput);
}

HERROR rema_DM_getData_AudienceMeasurement_CombinedAccess(HCHAR *pszOutput)
{
	RemLOG_Print("%p\n", _trdbHandle);
	CHECK_NULL(pszOutput);
	return rema_DM_getTextData("CombinedAccess", pszOutput);
}

HERROR rema_DM_getData_AudienceMeasurement_PeriodicInformationInterval(HCHAR *pszOutput)
{
	RemLOG_Print("%p\n", _trdbHandle);
	CHECK_NULL(pszOutput);
	return rema_DM_getTextData("PeriodicInformationInterval", pszOutput);
}

HERROR rema_DM_getData_AudienceMeasurement_PeriodicInformationTime(HCHAR *pszOutput)
{
	RemLOG_Print("%p\n", _trdbHandle);
	CHECK_NULL(pszOutput);
	return rema_DM_getTextData("PeriodicInformationTime", pszOutput);
}

HERROR rema_DM_getData_AudienceMeasurement_PermissionLevel(HCHAR *pszOutput)
{
#if 0
	APKE_JLABS_AMPermissionLevel_t level;
	APK_SQCSCENARIOJAPAN_GetAmPermissionLevel(&level);

	CHECK_NULL(pszOutput);
	HxSTD_sprintf(pszOutput, "%d", level);
	return ERR_OK;
#else
	return ERR_FAIL;
#endif

}


HINT32	rema_DM_MultiInstance_AdjustEntryCount(HINT32 nEntry)
{
	HINT32 nAdjust;

	if(nEntry > 0)	nAdjust = nEntry - ENTRY_OFFSET;
	else			nAdjust = nEntry;

	return nAdjust;
}


HERROR	rema_DM_MultiInstance_GetEntryCount(eTRXDMInstancetype_e eId, HINT32 *pnEntryCount)
{
	HINT32 nIndex;

	nIndex = rema_DM_MultiInstance_GetIndex(eId);

	*pnEntryCount = rema_DM_MultiInstance_AdjustEntryCount(stMultiDMEntryDB[nIndex].nEntryCount);

	RemLOG_Print("getcount stMultiDMEntryDB[%d].nEntryCount(%d)\n",nIndex,stMultiDMEntryDB[nIndex].nEntryCount);

	return ERR_OK;
}

HERROR TRP_Set_SoftwareModules_ExecEnv_Enable(HCHAR *pszInput)
{
	RemLOG_Print("%p\n", _trdbHandle);

	return rema_DM_setTextData("SoftwareModules.ExecEnv.Enable", pszInput);
}

HERROR TRP_Set_AMConfigurationName(HCHAR *pszInput)
{
	RemLOG_Print("%p\n", _trdbHandle);

	return rema_DM_setTextData("AMConfigurationName", pszInput);
}
HERROR TRP_Set_FirstUseDate(HCHAR *pszInput)
{
	RemLOG_Print("%p\n", _trdbHandle);

	return rema_DM_setTextData("FirstUseDate", pszInput);
}

HERROR TRP_Set_ProvisioningCode(HCHAR *pszInput)
{
	RemLOG_Print("%p\n", _trdbHandle);

	return rema_DM_setTextData("ProvisioningCode", pszInput);
}



#define __________SET_FUNCTION__________
HERROR rema_DM_setData_DeviceInfo_X_JCOM_SubscriberID(HCHAR *pszInput)
{
	RemLOG_Print("%p\n", _trdbHandle);
	return rema_DM_setTextData("X_JCOM_SubscriberID", pszInput);
}

HERROR rema_DM_setData_DeviceInfo_X_JCOM_ServiceID(HCHAR *pszInput)
{
	RemLOG_Print("%p\n", _trdbHandle);
	return rema_DM_setTextData("X_JCOM_ServiceID", pszInput);
}

HERROR rema_DM_setData_DeviceInfo_X_JCOM_FamilyID(HCHAR *pszInput)
{
	RemLOG_Print("%p\n", _trdbHandle);
	return rema_DM_setTextData("X_JCOM_FamilyID", pszInput);
}

HERROR rema_DM_setData_DeviceInfo_X_JCOM_BusinessCode(HCHAR *pszInput)
{
	RemLOG_Print("%p\n", _trdbHandle);
	return rema_DM_setTextData("X_JCOM_BusinessCode", pszInput);
}

HERROR rema_DM_setData_ManagementServer_Username(HCHAR *pszInput)
{
	RemLOG_Print("%p\n", _trdbHandle);
	return rema_DM_setTextData("Username", pszInput);
}

HERROR rema_DM_setData_ManagementServer_Password(HCHAR *pszInput)
{
	RemLOG_Print("%p\n", _trdbHandle);
	return rema_DM_setTextData("Password", pszInput);
}

HERROR rema_DM_setData_ManagementServer_Url(HCHAR *pszInput)
{
	RemLOG_Print("%p\n", _trdbHandle);
	return rema_DM_setTextData("URL", pszInput);
}


HERROR rema_DM_setData_ManagementServer_PeriodicInformEnable(HCHAR *pszInput)
{
	RemLOG_Print("%p\n", _trdbHandle);
	return rema_DM_setTextData("PeriodicInformEnable", pszInput);
}

HERROR rema_DM_setData_ManagementServer_PeriodicInformInterval(HCHAR *pszInput)
{
	RemLOG_Print("%p\n", _trdbHandle);
	return rema_DM_setTextData("PeriodicInformInterval", pszInput);
}

HERROR rema_DM_setData_ManagementServer_PeriodicInformTime(HCHAR *pszInput)
{
	RemLOG_Print("%p\n", _trdbHandle);
	return rema_DM_setTextData("PeriodicInformTime", pszInput);
}

HERROR rema_DM_setData_ManagementServer_ConnectionRequestURL(HCHAR *pszInput)
{
	RemLOG_Print("%p\n", _trdbHandle);
	return rema_DM_setTextData("ConnectionRequestURL", pszInput);
}

HERROR rema_DM_setData_ManagementServer_ConnectionRequestUsername(HCHAR *pszInput)
{
	RemLOG_Print("%p\n", _trdbHandle);
	return rema_DM_setTextData("ConnectionRequestUsername", pszInput);
}

HERROR rema_DM_setData_ManagementServer_ConnectionRequestPassword(HCHAR *pszInput)
{
	RemLOG_Print("%p\n", _trdbHandle);
	return rema_DM_setTextData("ConnectionRequestPassword", pszInput);
}

HERROR rema_DM_setData_ManagementServer_UpgradesManaged(HCHAR *pszInput)
{
	RemLOG_Print("%p\n", _trdbHandle);
	return rema_DM_setTextData("UpgradesManaged", pszInput);
}

HERROR rema_DM_setData_AudienceMeasurement_CombinedAccess(HCHAR *pszInput)
{
	RemLOG_Print("%p\n", _trdbHandle);
	return rema_DM_setTextData("CombinedAccess", pszInput);
}

HERROR rema_DM_setData_AudienceMeasurement_PeriodicInformationInterval(HCHAR *pszInput)
{
	RemLOG_Print("%p\n", _trdbHandle);
	return rema_DM_setTextData("PeriodicInformationInterval", pszInput);
}

HERROR rema_DM_setData_AudienceMeasurement_PeriodicInformationTime(HCHAR *pszInput)
{
	RemLOG_Print("%p\n", _trdbHandle);
	return rema_DM_setTextData("PeriodicInformationTime", pszInput);
}

HERROR rema_DM_setData_AudienceMeasurement_PermissionLevel(HCHAR *pszInput)
{
#if 0
	return APK_SQCSCENARIOJAPAN_SetAmPermissionLevel((APKE_JLABS_AMPermissionLevel_t)PARAM_ATOI(pszInput));
#else
	return ERR_FAIL;
#endif
}

#define ___135_GET_FUNCTIONS___

HERROR TRP135_Get_JCOM_Config_WebBrowserURLInput(HCHAR *pszOutput)
{
	RemLOG_Print("%p\n", _trdbHandle);
	CHECK_NULL(pszOutput);
	return rema_DM_getTextData("WebBrowserURLInput", pszOutput);
}

#define ___135_SET_FUNCTIONS___

HERROR TRP135_Set_JCOM_Config_WebBrowserURLInput(HCHAR *pszInput)
{
	RemLOG_Print("%p\n", _trdbHandle);

	return rema_DM_setTextData("WebBrowserURLInput", pszInput);
}


#define ___157_GET_FUNCTIONS___

HERROR TRP157_Get_MgmtServer_DefaultActiveNotificationThrottle(HCHAR *pszOutput)
{
	RemLOG_Print("%p\n", _trdbHandle);
	CHECK_NULL(pszOutput);
	return rema_DM_getTextData("DefaultActiveNotificationThrottle", pszOutput);
}
HERROR TRP157_Get_MgmtServer_CWMPRetryMinimumWaitInterval(HCHAR *pszOutput)
{
	RemLOG_Print("%p\n", _trdbHandle);
	CHECK_NULL(pszOutput);
	return rema_DM_getTextData("CWMPRetryMinimumWaitInterval", pszOutput);
}
HERROR TRP157_Get_MgmtServer_CWMPRetryIntervalMultiplier(HCHAR *pszOutput)
{
	RemLOG_Print("%p\n", _trdbHandle);
	CHECK_NULL(pszOutput);
	return rema_DM_getTextData("CWMPRetryIntervalMultiplier", pszOutput);
}


HERROR TRP157_Get_DevInfo_SupportedDataModelNumberOfEntries(HINT32 *pszOutput)
{
	*pszOutput = rema_DM_MultiInstance_GetEntry(eTR157A3_Device_DeviceInfo_SupportedDataModelNumberOfEntries);
	return ERR_OK;
}

HERROR TRP157_Get_MgmtServer_AutonomousTxCompletePolicy_Enable(HCHAR *pszOutput)
{
	RemLOG_Print("%p\n", _trdbHandle);
	CHECK_NULL(pszOutput);
	return rema_DM_getTextData("AutonomousTransferCompletePolicy.Enable", pszOutput);
}
HERROR TRP157_Get_MgmtServer_AutonomousTxCompletePolicy_TxTypeFilter(HCHAR *pszOutput)
{
	RemLOG_Print("%p\n", _trdbHandle);
	CHECK_NULL(pszOutput);
	return rema_DM_getTextData("TransferTypeFilter", pszOutput);
}
HERROR TRP157_Get_MgmtServer_AutonomousTxCompletePolicy_FileTypeFilter(HCHAR *pszOutput)
{
	RemLOG_Print("%p\n", _trdbHandle);
	CHECK_NULL(pszOutput);
	return rema_DM_getTextData("FileTypeFilter", pszOutput);
}

HERROR TRP157_Get_LAN_IPPingDiagnostics_DiagnosticsState(HCHAR *pszOutput)
{
	RemLOG_Print("%p\n", _trdbHandle);
	CHECK_NULL(pszOutput);
	return rema_DM_getTextData("IPPingDiagnostics.DiagnosticsState", pszOutput);
}
HERROR TRP157_Get_LAN_IPPingDiagnostics_Host(HCHAR *pszOutput)
{
	RemLOG_Print("%p\n", _trdbHandle);
	CHECK_NULL(pszOutput);
	return rema_DM_getTextData("IPPingDiagnostics.Host", pszOutput);
}
HERROR TRP157_Get_LAN_IPPingDiagnostics_NumberOfRepetitions(HCHAR *pszOutput)
{
	RemLOG_Print("%p\n", _trdbHandle);
	CHECK_NULL(pszOutput);
	return rema_DM_getTextData("IPPingDiagnostics.NumberOfRepetitions", pszOutput);
}
HERROR TRP157_Get_LAN_IPPingDiagnostics_Timeout(HCHAR *pszOutput)
{
	RemLOG_Print("%p\n", _trdbHandle);
	CHECK_NULL(pszOutput);
	return rema_DM_getTextData("IPPingDiagnostics.Timeout", pszOutput);
}
HERROR TRP157_Get_LAN_IPPingDiagnostics_DataBlockSize(HCHAR *pszOutput)
{
	RemLOG_Print("%p\n", _trdbHandle);
	CHECK_NULL(pszOutput);
	return rema_DM_getTextData("IPPingDiagnostics.DataBlockSize", pszOutput);
}
HERROR TRP157_Get_LAN_IPPingDiagnostics_DSCP(HCHAR *pszOutput)
{
	RemLOG_Print("%p\n", _trdbHandle);
	CHECK_NULL(pszOutput);
	return rema_DM_getTextData("IPPingDiagnostics.DSCP", pszOutput);
}
HERROR TRP157_Get_LAN_IPPingDiagnostics_SuccessCount(HCHAR *pszOutput)
{
	RemLOG_Print("%p\n", _trdbHandle);
	CHECK_NULL(pszOutput);
	return rema_DM_getTextData("IPPingDiagnostics.SuccessCount", pszOutput);
}
HERROR TRP157_Get_LAN_IPPingDiagnostics_FailureCount(HCHAR *pszOutput)
{
	RemLOG_Print("%p\n", _trdbHandle);
	CHECK_NULL(pszOutput);
	return rema_DM_getTextData("IPPingDiagnostics.FailureCount", pszOutput);
}
HERROR TRP157_Get_LAN_IPPingDiagnostics_AverageResponseTime(HCHAR *pszOutput)
{
	RemLOG_Print("%p\n", _trdbHandle);
	CHECK_NULL(pszOutput);
	return rema_DM_getTextData("IPPingDiagnostics.AverageResponseTime", pszOutput);
}
HERROR TRP157_Get_LAN_IPPingDiagnostics_MinimumResponseTime(HCHAR *pszOutput)
{
	RemLOG_Print("%p\n", _trdbHandle);
	CHECK_NULL(pszOutput);
	return rema_DM_getTextData("IPPingDiagnostics.MinimumResponseTime", pszOutput);
}
HERROR TRP157_Get_LAN_IPPingDiagnostics_MaximumResponseTime(HCHAR *pszOutput)
{
	RemLOG_Print("%p\n", _trdbHandle);
	CHECK_NULL(pszOutput);
	return rema_DM_getTextData("IPPingDiagnostics.MaximumResponseTime", pszOutput);
}

HERROR TRP157_Get_LAN_TraceRouteDiagnostics_DiagnosticsState(HCHAR *pszOutput)
{
	RemLOG_Print("%p\n", _trdbHandle);
	CHECK_NULL(pszOutput);
	return rema_DM_getTextData("TraceRouteDiagnostics.DiagnosticsState", pszOutput);
}
HERROR TRP157_Get_LAN_TraceRouteDiagnostics_Host(HCHAR *pszOutput)
{
	RemLOG_Print("%p\n", _trdbHandle);
	CHECK_NULL(pszOutput);
	return rema_DM_getTextData("TraceRouteDiagnostics.Host", pszOutput);
}
HERROR TRP157_Get_LAN_TraceRouteDiagnostics_Timeout(HCHAR *pszOutput)
{
	RemLOG_Print("%p\n", _trdbHandle);
	CHECK_NULL(pszOutput);
	return rema_DM_getTextData("TraceRouteDiagnostics.Timeout", pszOutput);
}
HERROR TRP157_Get_LAN_TraceRouteDiagnostics_DataBlockSize(HCHAR *pszOutput)
{
	RemLOG_Print("%p\n", _trdbHandle);
	CHECK_NULL(pszOutput);
	return rema_DM_getTextData("TraceRouteDiagnostics.DataBlockSize", pszOutput);
}
HERROR TRP157_Get_LAN_TraceRouteDiagnostics_MaxHopCount(HCHAR *pszOutput)
{
	RemLOG_Print("%p\n", _trdbHandle);
	CHECK_NULL(pszOutput);
	return rema_DM_getTextData("TraceRouteDiagnostics.MaxHopCount", pszOutput);
}
HERROR TRP157_Get_LAN_TraceRouteDiagnostics_DSCP(HCHAR *pszOutput)
{
	RemLOG_Print("%p\n", _trdbHandle);
	CHECK_NULL(pszOutput);
	return rema_DM_getTextData("TraceRouteDiagnostics.DSCP", pszOutput);
}
HERROR TRP157_Get_LAN_TraceRouteDiagnostics_ResponseTime(HCHAR *pszOutput)
{
	RemLOG_Print("%p\n", _trdbHandle);
	CHECK_NULL(pszOutput);
	return rema_DM_getTextData("TraceRouteDiagnostics.ResponseTime", pszOutput);
}
HERROR TRP157_Get_LAN_TraceRouteDiagnostics_NumberOfRouteHops(HINT32 *pszOutput)
{
#if 0
	HERROR hErr = ERR_OK;
	HCHAR szOutput[256] = {0,};
	RemLOG_Print("%p\n", _trdbHandle);
	CHECK_NULL(pszOutput);

	hErr = rema_DM_getTextData("TraceRouteDiagnostics.NumberOfRouteHops", szOutput);
	*pszOutput = PARAM_ATOI(szOutput);

	return hErr;
#endif
	*pszOutput = rema_DM_MultiInstance_GetEntry(eTR157A3_Device_LAN_TraceRouteDiagnostics_NumberOfRouteHops);
	return ERR_OK;
}
HERROR TRP157_Get_LAN_TraceRouteDiagnostics_RouteHops_M_HopHost(HCHAR *pszOutput)
{
#if 0
	RemLOG_Print("%p\n", _trdbHandle);
	CHECK_NULL(pszOutput);
	return rema_DM_getTextData("TraceRouteDiagnostics.RouteHops.HopHost", pszOutput);
#endif
	HINT32 nArg1 = 0;
	rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_LAN_TraceRouteDiagnostics_NumberOfRouteHops,&nArg1);
	TRP_DB_CPE_Get_DMObject(CDT_TraceRouteHops,NRD_None,nArg1,pszOutput);
	return ERR_OK;
}
HERROR TRP157_Get_DownloadDiagnostics_DiagnosticsState(HCHAR *pszOutput)
{
	RemLOG_Print("%p\n", _trdbHandle);
	CHECK_NULL(pszOutput);
	return rema_DM_getTextData("DownloadDiagnostics.DiagnosticsState", pszOutput);
}
HERROR TRP157_Get_DownloadDiagnostics_Interface(HCHAR *pszOutput)
{
	RemLOG_Print("%p\n", _trdbHandle);
	CHECK_NULL(pszOutput);
	return rema_DM_getTextData("DownloadDiagnostics.Interface", pszOutput);
}
HERROR TRP157_Get_DownloadDiagnostics_DownloadURL(HCHAR *pszOutput)
{
	RemLOG_Print("%p\n", _trdbHandle);
	CHECK_NULL(pszOutput);
	return rema_DM_getTextData("DownloadDiagnostics.DownloadURL", pszOutput);
}
HERROR TRP157_Get_DownloadDiagnostics_DSCP(HCHAR *pszOutput)
{
	RemLOG_Print("%p\n", _trdbHandle);
	CHECK_NULL(pszOutput);
	return rema_DM_getTextData("DownloadDiagnostics.DSCP", pszOutput);
}
HERROR TRP157_Get_DownloadDiagnostics_EthernetPriority(HCHAR *pszOutput)
{
	RemLOG_Print("%p\n", _trdbHandle);
	CHECK_NULL(pszOutput);
	return rema_DM_getTextData("DownloadDiagnostics.EthernetPriority", pszOutput);
}
HERROR TRP157_Get_DownloadDiagnostics_ROMTime(HCHAR *pszOutput)
{
	RemLOG_Print("%p\n", _trdbHandle);

	return rema_DM_getTextData("DownloadDiagnostics.ROMTime", pszOutput);
}
HERROR TRP157_Get_DownloadDiagnostics_BOMTime(HCHAR *pszOutput)
{
	RemLOG_Print("%p\n", _trdbHandle);
	CHECK_NULL(pszOutput);
	return rema_DM_getTextData("DownloadDiagnostics.BOMTime", pszOutput);
}
HERROR TRP157_Get_DownloadDiagnostics_EOMTime(HCHAR *pszOutput)
{
	RemLOG_Print("%p\n", _trdbHandle);
	CHECK_NULL(pszOutput);
	return rema_DM_getTextData("DownloadDiagnostics.EOMTime", pszOutput);
}

HERROR TRP157_Get_DownloadDiagnostics_TestBytesReceived(HCHAR *pszOutput)
{
	RemLOG_Print("%p\n", _trdbHandle);
	CHECK_NULL(pszOutput);
	return rema_DM_getTextData("DownloadDiagnostics.TestBytesReceived", pszOutput);
}

HERROR TRP157_Get_DownloadDiagnostics_TotalBytesReceived(HCHAR *pszOutput)
{
	RemLOG_Print("%p\n", _trdbHandle);
	CHECK_NULL(pszOutput);
	return rema_DM_getTextData("DownloadDiagnostics.TotalBytesReceived", pszOutput);
}
HERROR TRP157_Get_UploadDiagnostics_DiagnosticsState(HCHAR *pszOutput)
{
	RemLOG_Print("%p\n", _trdbHandle);
	CHECK_NULL(pszOutput);
	return rema_DM_getTextData("UploadDiagnostics.DiagnosticsState", pszOutput);
}
HERROR TRP157_Get_UploadDiagnostics_Interface(HCHAR *pszOutput)
{
	RemLOG_Print("%p\n", _trdbHandle);
	CHECK_NULL(pszOutput);
	return rema_DM_getTextData("UploadDiagnostics.Interface", pszOutput);
}
HERROR TRP157_Get_UploadDiagnostics_UploadURL(HCHAR *pszOutput)
{
	RemLOG_Print("%p\n", _trdbHandle);
	CHECK_NULL(pszOutput);
	return rema_DM_getTextData("UploadDiagnostics.UploadURL", pszOutput);
}
HERROR TRP157_Get_UploadDiagnostics_DSCP(HCHAR *pszOutput)
{
	RemLOG_Print("%p\n", _trdbHandle);
	CHECK_NULL(pszOutput);
	return rema_DM_getTextData("UploadDiagnostics.DSCP", pszOutput);
}
HERROR TRP157_Get_UploadDiagnostics_EthernetPriority(HCHAR *pszOutput)
{
	RemLOG_Print("%p\n", _trdbHandle);
	CHECK_NULL(pszOutput);
	return rema_DM_getTextData("UploadDiagnostics.EthernetPriority", pszOutput);
}
HERROR TRP157_Get_UploadDiagnostics_TestFileLength(HCHAR *pszOutput)
{
	RemLOG_Print("%p\n", _trdbHandle);
	CHECK_NULL(pszOutput);
	return rema_DM_getTextData("UploadDiagnostics.TestFileLength", pszOutput);
}
HERROR TRP157_Get_UploadDiagnostics_ROMTime(HCHAR *pszOutput)
{
	RemLOG_Print("%p\n", _trdbHandle);
	CHECK_NULL(pszOutput);
	return rema_DM_getTextData("UploadDiagnostics.ROMTime", pszOutput);
}
HERROR TRP157_Get_UploadDiagnostics_BOMTime(HCHAR *pszOutput)
{
	RemLOG_Print("%p\n", _trdbHandle);
	CHECK_NULL(pszOutput);
	return rema_DM_getTextData("UploadDiagnostics.BOMTime", pszOutput);
}
HERROR TRP157_Get_UploadDiagnostics_EOMTime(HCHAR *pszOutput)
{
	RemLOG_Print("%p\n", _trdbHandle);
	CHECK_NULL(pszOutput);
	return rema_DM_getTextData("UploadDiagnostics.EOMTime", pszOutput);
}
HERROR TRP157_Get_UploadDiagnostics_TotalBytesSent(HCHAR *pszOutput)
{
	RemLOG_Print("%p\n", _trdbHandle);
	CHECK_NULL(pszOutput);
	return rema_DM_getTextData("UploadDiagnostics.TotalBytesSent", pszOutput);
}



HERROR TRP157_Get_NSLookupDiagnostics_DiagnosticsState(HCHAR *pszOutput)
{
	RemLOG_Print("%p\n", _trdbHandle);
	CHECK_NULL(pszOutput);
	return rema_DM_getTextData("NSLookupDiagnostics.DiagnosticsState", pszOutput);
}
HERROR TRP157_Get_NSLookupDiagnostics_Interface(HCHAR *pszOutput)
{
	RemLOG_Print("%p\n", _trdbHandle);
	CHECK_NULL(pszOutput);
	return rema_DM_getTextData("NSLookupDiagnostics.Interface", pszOutput);
}
HERROR TRP157_Get_NSLookupDiagnostics_HostName(HCHAR *pszOutput)
{
	RemLOG_Print("%p\n", _trdbHandle);
	CHECK_NULL(pszOutput);
	return rema_DM_getTextData("NSLookupDiagnostics.HostName", pszOutput);
}
HERROR TRP157_Get_NSLookupDiagnostics_DNSServer(HCHAR *pszOutput)
{
	RemLOG_Print("%p\n", _trdbHandle);
	CHECK_NULL(pszOutput);
	return rema_DM_getTextData("NSLookupDiagnostics.DNSServer", pszOutput);
}
HERROR TRP157_Get_NSLookupDiagnostics_Timeout(HCHAR *pszOutput)
{
	RemLOG_Print("%p\n", _trdbHandle);
	CHECK_NULL(pszOutput);
	return rema_DM_getTextData("NSLookupDiagnostics.Timeout", pszOutput);
}
HERROR TRP157_Get_NSLookupDiagnostics_NumberOfRepetitions(HCHAR *pszOutput)
{
	RemLOG_Print("%p\n", _trdbHandle);
	CHECK_NULL(pszOutput);
	return rema_DM_getTextData("NSLookupDiagnostics.NumberOfRepetitions", pszOutput);
}
HERROR TRP157_Get_NSLookupDiagnostics_SuccessCount(HCHAR *pszOutput)
{
	RemLOG_Print("%p\n", _trdbHandle);
	CHECK_NULL(pszOutput);
	return rema_DM_getTextData("NSLookupDiagnostics.SuccessCount", pszOutput);
}
HERROR TRP157_Get_NSLookupDiagnostics_ResultNumberOfEntries(HINT32 *pszOutput)
{
#if 0
	HERROR hErr = ERR_OK;
	HCHAR szOutput[256] = {0,};
	RemLOG_Print("%p\n", _trdbHandle);
	CHECK_NULL(pszOutput);

	hErr = rema_DM_getTextData("NSLookupDiagnostics.ResultNumberOfEntries", szOutput);
	*pszOutput = PARAM_ATOI(szOutput);

	return hErr;
#endif
	*pszOutput = rema_DM_MultiInstance_GetEntry(eTR157A3_Device_NSLookupDiagnostics_ResultNumberOfEntries);
	return ERR_OK;
}
HERROR TRP157_Get_NSLookupDiagnostics_Result_M_Status(HCHAR *pszOutput)
{
	HINT32 nArg1 = 0;
	rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_NSLookupDiagnostics_ResultNumberOfEntries,&nArg1);
	TRP_DB_CPE_Get_DMObject(CDT_NSLookupDiagResult,NRD_Status,nArg1,pszOutput);
	return ERR_OK;
}
HERROR TRP157_Get_NSLookupDiagnostics_Result_M_AnswerType(HCHAR *pszOutput)
{
	HINT32 nArg1 = 0;
	rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_NSLookupDiagnostics_ResultNumberOfEntries,&nArg1);
	TRP_DB_CPE_Get_DMObject(CDT_NSLookupDiagResult,NRD_AnswerType,nArg1,pszOutput);
	return ERR_OK;
}
HERROR TRP157_Get_NSLookupDiagnostics_Result_M_HostNameReturned(HCHAR *pszOutput)
{
	HINT32 nArg1 = 0;
	rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_NSLookupDiagnostics_ResultNumberOfEntries,&nArg1);
	TRP_DB_CPE_Get_DMObject(CDT_NSLookupDiagResult,NRD_HostNameReturned,nArg1,pszOutput);
	return ERR_OK;
}
HERROR TRP157_Get_NSLookupDiagnostics_Result_M_IPAddresses(HCHAR *pszOutput)
{
	HINT32 nArg1 = 0;
	rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_NSLookupDiagnostics_ResultNumberOfEntries,&nArg1);
	TRP_DB_CPE_Get_DMObject(CDT_NSLookupDiagResult,NRD_IPAddresses,nArg1,pszOutput);
	return ERR_OK;
}
HERROR TRP157_Get_NSLookupDiagnostics_Result_M_DNSServerIP(HCHAR *pszOutput)
{
	HINT32 nArg1 = 0;
	rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_NSLookupDiagnostics_ResultNumberOfEntries,&nArg1);
	TRP_DB_CPE_Get_DMObject(CDT_NSLookupDiagResult,NRD_DNSServerIP,nArg1,pszOutput);
	return ERR_OK;
}
HERROR TRP157_Get_NSLookupDiagnostics_Result_M_ResponseTime(HCHAR *pszOutput)
{
	HINT32 nArg1 = 0;
	rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_NSLookupDiagnostics_ResultNumberOfEntries,&nArg1);
	TRP_DB_CPE_Get_DMObject(CDT_NSLookupDiagResult,NRD_ResponseTime,nArg1,pszOutput);
	return ERR_OK;
}

HERROR TRP157_Get_X_JCOM_HomeNet_IPPingDiagnostics_DiagnosticsState(HCHAR *pszOutput)
{
	RemLOG_Print("%p\n", _trdbHandle);
	CHECK_NULL(pszOutput);
	return rema_DM_getTextData("IPPingDiagnostics.JCOM.DiagnosticsState", pszOutput);
}
HERROR TRP157_Get_X_JCOM_HomeNet_IPPingDiagnostics_Host(HCHAR *pszOutput)
{
	RemLOG_Print("%p\n", _trdbHandle);
	CHECK_NULL(pszOutput);
	return rema_DM_getTextData("IPPingDiagnostics.JCOM.Host", pszOutput);
}
HERROR TRP157_Get_X_JCOM_HomeNet_IPPingDiagnostics_NumberOfRepetitions(HCHAR *pszOutput)
{
	RemLOG_Print("%p\n", _trdbHandle);
	CHECK_NULL(pszOutput);
	return rema_DM_getTextData("IPPingDiagnostics.JCOM.NumberOfRepetitions", pszOutput);
}
HERROR TRP157_Get_X_JCOM_HomeNet_IPPingDiagnostics_Timeout(HCHAR *pszOutput)
{
	RemLOG_Print("%p\n", _trdbHandle);
	CHECK_NULL(pszOutput);
	return rema_DM_getTextData("IPPingDiagnostics.JCOM.Timeout", pszOutput);
}
HERROR TRP157_Get_X_JCOM_HomeNet_IPPingDiagnostics_DataBlockSize(HCHAR *pszOutput)
{
	RemLOG_Print("%p\n", _trdbHandle);
	CHECK_NULL(pszOutput);
	return rema_DM_getTextData("IPPingDiagnostics.JCOM.DataBlockSize", pszOutput);
}
HERROR TRP157_Get_X_JCOM_HomeNet_IPPingDiagnostics_SuccessCount(HCHAR *pszOutput)
{
	RemLOG_Print("%p\n", _trdbHandle);
	CHECK_NULL(pszOutput);
	return rema_DM_getTextData("IPPingDiagnostics.JCOM.SuccessCount", pszOutput);
}
HERROR TRP157_Get_X_JCOM_HomeNet_IPPingDiagnostics_FailureCount(HCHAR *pszOutput)
{
	RemLOG_Print("%p\n", _trdbHandle);
	CHECK_NULL(pszOutput);
	return rema_DM_getTextData("IPPingDiagnostics.JCOM.FailureCount", pszOutput);
}
HERROR TRP157_Get_X_JCOM_HomeNet_IPPingDiagnostics_AverageResponseTime(HCHAR *pszOutput)
{
	RemLOG_Print("%p\n", _trdbHandle);
	CHECK_NULL(pszOutput);
	return rema_DM_getTextData("IPPingDiagnostics.JCOM.AverageResponseTime", pszOutput);
}
HERROR TRP157_Get_X_JCOM_HomeNet_IPPingDiagnostics_MinimumResponseTime(HCHAR *pszOutput)
{
	RemLOG_Print("%p\n", _trdbHandle);
	CHECK_NULL(pszOutput);
	return rema_DM_getTextData("IPPingDiagnostics.JCOM.MinimumResponseTime", pszOutput);
}
HERROR TRP157_Get_X_JCOM_HomeNet_IPPingDiagnostics_MaximumResponseTime(HCHAR *pszOutput)
{
	RemLOG_Print("%p\n", _trdbHandle);
	CHECK_NULL(pszOutput);
	return rema_DM_getTextData("IPPingDiagnostics.JCOM.MaximumResponseTime", pszOutput);
}




#define ___157_SET_FUNCTIONS___

HERROR TRP157_Set_MgmtServer_DefaultActiveNotificationThrottle(HCHAR *pszInput)
{
	RemLOG_Print("%p\n", _trdbHandle);

	return rema_DM_setTextData("DefaultActiveNotificationThrottle", pszInput);
}
HERROR TRP157_Set_MgmtServer_CWMPRetryMinimumWaitInterval(HCHAR *pszInput)
{
	RemLOG_Print("%p\n", _trdbHandle);

	return rema_DM_setTextData("CWMPRetryMinimumWaitInterval", pszInput);
}
HERROR TRP157_Set_MgmtServer_CWMPRetryIntervalMultiplier(HCHAR *pszInput)
{
	RemLOG_Print("%p\n", _trdbHandle);

	return rema_DM_setTextData("CWMPRetryIntervalMultiplier", pszInput);
}
HERROR TRP157_Set_MgmtServer_AutonomousTxCompletePolicy_Enable(HCHAR *pszInput)
{
	RemLOG_Print("%p\n", _trdbHandle);

	return rema_DM_setTextData("AutonomousTransferCompletePolicy.Enable", pszInput);
}
HERROR TRP157_Set_MgmtServer_AutonomousTxCompletePolicy_TxTypeFilter(HCHAR *pszInput)
{
	RemLOG_Print("%p\n", _trdbHandle);

	return rema_DM_setTextData("TransferTypeFilter", pszInput);
}
HERROR TRP157_Set_MgmtServer_AutonomousTxCompletePolicy_FileTypeFilter(HCHAR *pszInput)
{
	RemLOG_Print("%p\n", _trdbHandle);

	return rema_DM_setTextData("FileTypeFilter", pszInput);
}
HERROR TRP157_Set_LAN_IPPingDiagnostics_DiagnosticsState(HCHAR *pszInput)
{
	RemLOG_Print("%p\n", _trdbHandle);

	return rema_DM_setTextData("IPPingDiagnostics.DiagnosticsState", pszInput);
}
HERROR TRP157_Set_LAN_IPPingDiagnostics_Host(HCHAR *pszInput)
{
	RemLOG_Print("%p\n", _trdbHandle);

	return rema_DM_setTextData("IPPingDiagnostics.Host", pszInput);
}
HERROR TRP157_Set_LAN_IPPingDiagnostics_NumberOfRepetitions(HCHAR *pszInput)
{
	RemLOG_Print("%p\n", _trdbHandle);

	return rema_DM_setTextData("IPPingDiagnostics.NumberOfRepetitions", pszInput);
}
HERROR TRP157_Set_LAN_IPPingDiagnostics_Timeout(HCHAR *pszInput)
{
	RemLOG_Print("%p\n", _trdbHandle);

	return rema_DM_setTextData("IPPingDiagnostics.Timeout", pszInput);
}
HERROR TRP157_Set_LAN_IPPingDiagnostics_DataBlockSize(HCHAR *pszInput)
{
	RemLOG_Print("%p\n", _trdbHandle);

	return rema_DM_setTextData("IPPingDiagnostics.DataBlockSize", pszInput);
}
HERROR TRP157_Set_LAN_IPPingDiagnostics_DSCP(HCHAR *pszInput)
{
	RemLOG_Print("%p\n", _trdbHandle);

	return rema_DM_setTextData("IPPingDiagnostics.DSCP", pszInput);
}
HERROR TRP157_Set_LAN_IPPingDiagnostics_SuccessCount(HCHAR *pszInput)
{
	RemLOG_Print("%p\n", _trdbHandle);

 	return rema_DM_setTextData("IPPingDiagnostics.SuccessCount", pszInput);
}
HERROR TRP157_Set_LAN_IPPingDiagnostics_FailureCount(HCHAR *pszInput)
{
	RemLOG_Print("%p\n", _trdbHandle);

 	return rema_DM_setTextData("IPPingDiagnostics.FailureCount", pszInput);
}
HERROR TRP157_Set_LAN_IPPingDiagnostics_AverageResponseTime(HCHAR *pszInput)
{
	RemLOG_Print("%p\n", _trdbHandle);

 	return rema_DM_setTextData("IPPingDiagnostics.AverageResponseTime", pszInput);
}
HERROR TRP157_Set_LAN_IPPingDiagnostics_MinimumResponseTime(HCHAR *pszInput)
{
	RemLOG_Print("%p\n", _trdbHandle);

 	return rema_DM_setTextData("IPPingDiagnostics.MinimumResponseTime", pszInput);
}
HERROR TRP157_Set_LAN_IPPingDiagnostics_MaximumResponseTime(HCHAR *pszInput)
{
	RemLOG_Print("%p\n", _trdbHandle);

 	return rema_DM_setTextData("IPPingDiagnostics.MaximumResponseTime", pszInput);
}
HERROR TRP157_Set_LAN_TraceRouteDiagnostics_DiagnosticsState(HCHAR *pszInput)
{
	RemLOG_Print("%p\n", _trdbHandle);

	return rema_DM_setTextData("TraceRouteDiagnostics.DiagnosticsState", pszInput);
}
HERROR TRP157_Set_LAN_TraceRouteDiagnostics_Host(HCHAR *pszInput)
{
	RemLOG_Print("%p\n", _trdbHandle);

	return rema_DM_setTextData("TraceRouteDiagnostics.Host", pszInput);
}
HERROR TRP157_Set_LAN_TraceRouteDiagnostics_Timeout(HCHAR *pszInput)
{
	RemLOG_Print("%p\n", _trdbHandle);

	return rema_DM_setTextData("TraceRouteDiagnostics.Timeout", pszInput);
}
HERROR TRP157_Set_LAN_TraceRouteDiagnostics_DataBlockSize(HCHAR *pszInput)
{
	RemLOG_Print("%p\n", _trdbHandle);

	return rema_DM_setTextData("TraceRouteDiagnostics.DataBlockSize", pszInput);
}
HERROR TRP157_Set_LAN_TraceRouteDiagnostics_MaxHopCount(HCHAR *pszInput)
{
	RemLOG_Print("%p\n", _trdbHandle);

	return rema_DM_setTextData("TraceRouteDiagnostics.MaxHopCount", pszInput);
}
HERROR TRP157_Set_LAN_TraceRouteDiagnostics_DSCP(HCHAR *pszInput)
{
	RemLOG_Print("%p\n", _trdbHandle);

	return rema_DM_setTextData("TraceRouteDiagnostics.DSCP", pszInput);
}
HERROR TRP157_Set_LAN_TraceRouteDiagnostics_ResponseTime(HCHAR *pszInput)
{
	RemLOG_Print("%p\n", _trdbHandle);

	return rema_DM_setTextData("TraceRouteDiagnostics.ResponseTime", pszInput);
}
HERROR TRP157_Set_LAN_TraceRouteDiagnostics_NumberOfRouteHops(HINT32 *pszInput)
{
	if(pszInput == NULL) return ERR_OK;

	TRP_DB_CPE_Build_DMObject(CDT_TraceRouteHops, pszInput);
	return ERR_OK;
}
HERROR TRP157_Set_LAN_TraceRouteDiagnostics_M_RouteHops_HopHost(HCHAR *pszInput)
{
	HINT32 nArg1 = 0;
	rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_LAN_TraceRouteDiagnostics_NumberOfRouteHops,&nArg1);
	TRP_DB_CPE_Set_DMObject(CDT_TraceRouteHops,NRD_None,nArg1,pszInput);
	return ERR_OK;
}
HERROR TRP157_Set_DownloadDiagnostics_DiagnosticsState(HCHAR *pszInput)
{
	RemLOG_Print("%p\n", _trdbHandle);

	return rema_DM_setTextData("DownloadDiagnostics.DiagnosticsState", pszInput);
}
HERROR TRP157_Set_DownloadDiagnostics_Interface(HCHAR *pszInput)
{
	RemLOG_Print("%p\n", _trdbHandle);

	return rema_DM_setTextData("DownloadDiagnostics.Interface", pszInput);
}
HERROR TRP157_Set_DownloadDiagnostics_DownloadURL(HCHAR *pszInput)
{
	RemLOG_Print("%p\n", _trdbHandle);

	return rema_DM_setTextData("DownloadDiagnostics.DownloadURL", pszInput);
}
HERROR TRP157_Set_DownloadDiagnostics_DSCP(HCHAR *pszInput)
{
	RemLOG_Print("%p\n", _trdbHandle);

	return rema_DM_setTextData("DownloadDiagnostics.DSCP", pszInput);
}
HERROR TRP157_Set_DownloadDiagnostics_EthernetPriority(HCHAR *pszInput)
{
	RemLOG_Print("%p\n", _trdbHandle);

	return rema_DM_setTextData("DownloadDiagnostics.EthernetPriority", pszInput);
}
HERROR TRP157_Set_DownloadDiagnostics_ROMTime(HCHAR *pszInput)
{
	RemLOG_Print("%p\n", _trdbHandle);

	return rema_DM_setTextData("DownloadDiagnostics.ROMTime", pszInput);
}
HERROR TRP157_Set_DownloadDiagnostics_BOMTime(HCHAR *pszInput)
{
	RemLOG_Print("%p\n", _trdbHandle);

	return rema_DM_setTextData("DownloadDiagnostics.BOMTime", pszInput);
}
HERROR TRP157_Set_DownloadDiagnostics_EOMTime(HCHAR *pszInput)
{
	RemLOG_Print("%p\n", _trdbHandle);

	return rema_DM_setTextData("DownloadDiagnostics.EOMTime", pszInput);
}
HERROR TRP157_Set_DownloadDiagnostics_TestBytesReceived(HCHAR *pszInput)
{
	RemLOG_Print("%p\n", _trdbHandle);

	return rema_DM_setTextData("DownloadDiagnostics.TestBytesReceived", pszInput);
}
HERROR TRP157_Set_DownloadDiagnostics_TotalBytesReceived(HCHAR *pszInput)
{
	RemLOG_Print("%p\n", _trdbHandle);

	return rema_DM_setTextData("DownloadDiagnostics.TotalBytesReceived", pszInput);
}

HERROR TRP157_Set_UploadDiagnostics_DiagnosticsState(HCHAR *pszInput)
{
	RemLOG_Print("%p\n", _trdbHandle);

	return rema_DM_setTextData("UploadDiagnostics.DiagnosticsState", pszInput);
}
HERROR TRP157_Set_UploadDiagnostics_Interface(HCHAR *pszInput)
{
	RemLOG_Print("%p\n", _trdbHandle);

	return rema_DM_setTextData("UploadDiagnostics.Interface", pszInput);
}
HERROR TRP157_Set_UploadDiagnostics_UploadURL(HCHAR *pszInput)
{
	RemLOG_Print("%p\n", _trdbHandle);

	return rema_DM_setTextData("UploadDiagnostics.UploadURL", pszInput);
}
HERROR TRP157_Set_UploadDiagnostics_DSCP(HCHAR *pszInput)
{
	RemLOG_Print("%p\n", _trdbHandle);

	return rema_DM_setTextData("UploadDiagnostics.DSCP", pszInput);
}
HERROR TRP157_Set_UploadDiagnostics_EthernetPriority(HCHAR *pszInput)
{
	RemLOG_Print("%p\n", _trdbHandle);

	return rema_DM_setTextData("UploadDiagnostics.EthernetPriority", pszInput);
}
HERROR TRP157_Set_UploadDiagnostics_TestFileLength(HCHAR *pszInput)
{
	RemLOG_Print("%p\n", _trdbHandle);

	return rema_DM_setTextData("UploadDiagnostics.TestFileLength", pszInput);
}
HERROR TRP157_Set_UploadDiagnostics_ROMTime(HCHAR *pszInput)
{
	RemLOG_Print("%p\n", _trdbHandle);

	return rema_DM_setTextData("UploadDiagnostics.ROMTime", pszInput);
}
HERROR TRP157_Set_UploadDiagnostics_BOMTime(HCHAR *pszInput)
{
	RemLOG_Print("%p\n", _trdbHandle);

	return rema_DM_setTextData("UploadDiagnostics.BOMTime", pszInput);
}
HERROR TRP157_Set_UploadDiagnostics_EOMTime(HCHAR *pszInput)
{
	RemLOG_Print("%p\n", _trdbHandle);

	return rema_DM_setTextData("UploadDiagnostics.EOMTime", pszInput);
}
HERROR TRP157_Set_UploadDiagnostics_TotalBytesSent(HCHAR *pszInput)
{
	RemLOG_Print("%p\n", _trdbHandle);

	return rema_DM_setTextData("UploadDiagnostics.TotalBytesSent", pszInput);
}
HERROR TRP157_Set_NSLookupDiagnostics_DiagnosticsState(HCHAR *pszInput)
{
	RemLOG_Print("%p\n", _trdbHandle);

	return rema_DM_setTextData("NSLookupDiagnostics.DiagnosticsState", pszInput);
}
HERROR TRP157_Set_NSLookupDiagnostics_Interface(HCHAR *pszInput)
{
	RemLOG_Print("%p\n", _trdbHandle);

	return rema_DM_setTextData("NSLookupDiagnostics.Interface", pszInput);
}
HERROR TRP157_Set_NSLookupDiagnostics_HostName(HCHAR *pszInput)
{
	RemLOG_Print("%p\n", _trdbHandle);

	return rema_DM_setTextData("NSLookupDiagnostics.HostName", pszInput);
}
HERROR TRP157_Set_NSLookupDiagnostics_DNSServer(HCHAR *pszInput)
{
	RemLOG_Print("%p\n", _trdbHandle);

	return rema_DM_setTextData("NSLookupDiagnostics.DNSServer", pszInput);
}
HERROR TRP157_Set_NSLookupDiagnostics_Timeout(HCHAR *pszInput)
{
	RemLOG_Print("%p\n", _trdbHandle);

	return rema_DM_setTextData("NSLookupDiagnostics.Timeout", pszInput);
}
HERROR TRP157_Set_NSLookupDiagnostics_NumberOfRepetitions(HCHAR *pszInput)
{
	RemLOG_Print("%p\n", _trdbHandle);

	return rema_DM_setTextData("NSLookupDiagnostics.NumberOfRepetitions", pszInput);
}
HERROR TRP157_Set_NSLookupDiagnostics_SuccessCount(HCHAR *pszInput)
{
	RemLOG_Print("%p\n", _trdbHandle);

	return rema_DM_setTextData("NSLookupDiagnostics.SuccessCount", pszInput);
}
HERROR TRP157_Set_NSLookupDiagnostics_ResultNumberOfEntries(HINT32 *pszInput)
{
	if(pszInput == NULL) return ERR_OK;

	TRP_DB_CPE_Build_DMObject(CDT_NSLookupDiagResult, pszInput);
	return ERR_OK;

}
HERROR TRP157_Set_NSLookupDiagnostics_Result_M_Status(HCHAR *pszInput)
{
	HINT32 nArg1 = 0;
	rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_NSLookupDiagnostics_ResultNumberOfEntries,&nArg1);
	TRP_DB_CPE_Set_DMObject(CDT_NSLookupDiagResult,NRD_Status,nArg1,pszInput);
	return ERR_OK;

}
HERROR TRP157_Set_NSLookupDiagnostics_Result_M_AnswerType(HCHAR *pszInput)
{
	HINT32 nArg1 = 0;
	rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_NSLookupDiagnostics_ResultNumberOfEntries,&nArg1);
	TRP_DB_CPE_Set_DMObject(CDT_NSLookupDiagResult,NRD_AnswerType,nArg1,pszInput);
	return ERR_OK;
}
HERROR TRP157_Set_NSLookupDiagnostics_Result_M_HostNameReturned(HCHAR *pszInput)
{
	HINT32 nArg1 = 0;
	rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_NSLookupDiagnostics_ResultNumberOfEntries,&nArg1);
	TRP_DB_CPE_Set_DMObject(CDT_NSLookupDiagResult,NRD_HostNameReturned,nArg1,pszInput);
	return ERR_OK;
}
HERROR TRP157_Set_NSLookupDiagnostics_Result_M_IPAddresses(HCHAR *pszInput)
{
	HINT32 nArg1 = 0;
	rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_NSLookupDiagnostics_ResultNumberOfEntries,&nArg1);
	TRP_DB_CPE_Set_DMObject(CDT_NSLookupDiagResult,NRD_IPAddresses,nArg1,pszInput);
	return ERR_OK;
}
HERROR TRP157_Set_NSLookupDiagnostics_Result_M_DNSServerIP(HCHAR *pszInput)
{
	HINT32 nArg1 = 0;
	rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_NSLookupDiagnostics_ResultNumberOfEntries,&nArg1);
	TRP_DB_CPE_Set_DMObject(CDT_NSLookupDiagResult,NRD_DNSServerIP,nArg1,pszInput);
	return ERR_OK;
}
HERROR TRP157_Set_NSLookupDiagnostics_Result_M_ResponseTime(HCHAR *pszInput)
{
	HINT32 nArg1 = 0;
	rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_NSLookupDiagnostics_ResultNumberOfEntries,&nArg1);
	TRP_DB_CPE_Set_DMObject(CDT_NSLookupDiagResult,NRD_ResponseTime,nArg1,pszInput);
	return ERR_OK;
}

HERROR TRP157_Set_X_JCOM_HomeNet_IPPingDiagnostics_DiagnosticsState(HCHAR *pszInput)
{
	RemLOG_Print("%p\n", _trdbHandle);

	return rema_DM_setTextData("IPPingDiagnostics.JCOM.DiagnosticsState", pszInput);
}
HERROR TRP157_Set_X_JCOM_HomeNet_IPPingDiagnostics_Host(HCHAR *pszInput)
{
	RemLOG_Print("%p\n", _trdbHandle);

	return rema_DM_setTextData("IPPingDiagnostics.JCOM.Host", pszInput);
}
HERROR TRP157_Set_X_JCOM_HomeNet_IPPingDiagnostics_NumberOfRepetitions(HCHAR *pszInput)
{
	RemLOG_Print("%p\n", _trdbHandle);

	return rema_DM_setTextData("IPPingDiagnostics.JCOM.NumberOfRepetitions", pszInput);
}
HERROR TRP157_Set_X_JCOM_HomeNet_IPPingDiagnostics_Timeout(HCHAR *pszInput)
{
	RemLOG_Print("%p\n", _trdbHandle);

	return rema_DM_setTextData("IPPingDiagnostics.JCOM.Timeout", pszInput);
}
HERROR TRP157_Set_X_JCOM_HomeNet_IPPingDiagnostics_DataBlockSize(HCHAR *pszInput)
{
	RemLOG_Print("%p\n", _trdbHandle);

	return rema_DM_setTextData("IPPingDiagnostics.JCOM.DataBlockSize", pszInput);
}
HERROR TRP157_Set_X_JCOM_HomeNet_IPPingDiagnostics_SuccessCount(HCHAR *pszInput)
{
	RemLOG_Print("%p\n", _trdbHandle);

	return rema_DM_setTextData("IPPingDiagnostics.JCOM.SuccessCount", pszInput);
}
HERROR TRP157_Set_X_JCOM_HomeNet_IPPingDiagnostics_FailureCount(HCHAR *pszInput)
{
	RemLOG_Print("%p\n", _trdbHandle);

	return rema_DM_setTextData("IPPingDiagnostics.JCOM.FailureCount", pszInput);
}
HERROR TRP157_Set_X_JCOM_HomeNet_IPPingDiagnostics_AverageResponseTime(HCHAR *pszInput)
{
	RemLOG_Print("%p\n", _trdbHandle);

	return rema_DM_setTextData("IPPingDiagnostics.JCOM.AverageResponseTime", pszInput);
}
HERROR TRP157_Set_X_JCOM_HomeNet_IPPingDiagnostics_MinimumResponseTime(HCHAR *pszInput)
{
	RemLOG_Print("%p\n", _trdbHandle);

	return rema_DM_setTextData("IPPingDiagnostics.JCOM.MinimumResponseTime", pszInput);
}
HERROR TRP157_Set_X_JCOM_HomeNet_IPPingDiagnostics_MaximumResponseTime(HCHAR *pszInput)
{
	RemLOG_Print("%p\n", _trdbHandle);

	return rema_DM_setTextData("IPPingDiagnostics.JCOM.MaximumResponseTime", pszInput);
}
HERROR TRP157_Set_SelfTestDiagnostics_DiagnosticsState(HCHAR *pszInput)
{
	RemLOG_Print("%p\n", _trdbHandle);

	return rema_DM_setTextData("SelfTestDiagnostics.DiagnosticsState", pszInput);
}
HERROR TRP157_Set_DevInfo_SupportedDataModelNumberOfEntries(HINT32 *pszInput)
{
	if(pszInput == NULL) return ERR_OK;

	TRP_DB_CPE_Build_DMObject(CDT_SupportedDataModel, pszInput);
	return ERR_OK;
}
HERROR TRP157_Set_DevInfo_SupportedDataModel_M_URL(HCHAR *pszInput)
{
	HINT32 nArg1 = 0;
	rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_DeviceInfo_SupportedDataModelNumberOfEntries,&nArg1);
	TRP_DB_CPE_Set_DMObject(CDT_SupportedDataModel,SDM_URL,nArg1,pszInput);
	return ERR_OK;
}
HERROR TRP157_Set_DevInfo_SupportedDataModel_M_URN(HCHAR *pszInput)
{
	HINT32 nArg1 = 0;
	rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_DeviceInfo_SupportedDataModelNumberOfEntries,&nArg1);
	TRP_DB_CPE_Set_DMObject(CDT_SupportedDataModel,SDM_URN,nArg1,pszInput);
	return ERR_OK;
}
HERROR TRP157_Set_DevInfo_SupportedDataModel_M_Features(HCHAR *pszInput)
{
	HINT32 nArg1 = 0;
	rema_DM_MultiInstance_GetEntryCount(eTR157A3_Device_DeviceInfo_SupportedDataModelNumberOfEntries,&nArg1);
	TRP_DB_CPE_Set_DMObject(CDT_SupportedDataModel,SDM_Features,nArg1,pszInput);
	return ERR_OK;
}





#define ___EXTERNAL_FUNCTIONS___

#define _____END_OF_FILE_________

