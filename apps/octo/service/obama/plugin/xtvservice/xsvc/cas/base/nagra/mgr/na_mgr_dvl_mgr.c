/**************************************************************************************************/
#define ________CA_DVL_Private_Include_____________________________________________________
/**************************************************************************************************/
#include <_svc_cas_mgr_common.h>
#include <timeconv.h>

#include "na_glob.h"

#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
#include "namgr_main.h"
#endif

/**************************************************************************************************/
#define ________CA_DVL_Golbal_Value________________________________________________________
/**************************************************************************************************/


/**************************************************************************************************/
#define ________CA_DVL_Private_Define_______________________________________________________
/**************************************************************************************************/


/**************************************************************************************************/
#define ________CA_DVL_Private_Type_Define_________________________________________________
/**************************************************************************************************/

/**************************************************************************************************/
#define ________CA_DVL_Private_Static_Value_________________________________________________
/**************************************************************************************************/
static TDvlVersionString		s_xDvlVersion;
static TDvlChipsetTypeString	s_xDvlChipsetType;
//static TUnsignedInt8 			s_ucDvlPairingData[MAX_PD_SIZE];


/**************************************************************************************************/
#define ________CA_DVL_Private_Static_Prototype______________________________________________
/**************************************************************************************************/

#if 0 //DVL Integration Test 결과 report 작성시 사용하면 편함.
extern void filelogTest(HCHAR *ps);
#endif

/**************************************************************************************************/
#define ________CA_DVL_Private_Static_Prototype_Body________________________________________
/**************************************************************************************************/
#if 0
static void _naMgr_dvlMgr_GetPairingData(void)
{
#if defined(TEMP_SW_PAIRING_KEY)
	#include "na_mgr_cak_pairingdata.h"	// model별 CSO, Operator...
	HxSTD_memcpy(s_ucDvlPairingData, s_aucNaPairingData, MAX_PD_SIZE);
#else
	// TODO: copy decrypted PK from RAM
#endif
}
#endif

static HUINT8 *_naMgr_dlvMgr_GetDvlReturnValueName(TDvlStatus status)
{
	switch(status)
	{
		ENUM_TO_STR(DVL_NO_ERROR);
		  /**<
		   *	The function terminated successfully.
		  */
		ENUM_TO_STR(DVL_ERROR);
		  /**<
		   *	An unspecified error happened during the function execution.
		  */
		ENUM_TO_STR(DVL_ERROR_BAD_PARAMETER);
		  /**<
		   *	One or more function arguments are missing. The request could not be
		   *	successfully processed.
		  */
		ENUM_TO_STR(DVL_ERROR_DVL_NOT_RUNNING);
		  /**<	The Nagra DVR/VOD Library is currently not running and the requested
		   *	operation cannot be executed.
		  */
		ENUM_TO_STR(LAST_DVL_STATUS);
		  /**<
		   *	Last DVL status.
		   *	This status is never returned by the DVR/VOD Library.
		  */
		default:
			return "Unknown Return vaule";
	}
}


static HUINT8 *_naMgr_dlvMgr_GetDvlStatusName(TDvlAccessStatus status)
{
	switch(status)
	{
		ENUM_TO_STR(DVL_ACCESS_GRANTED);
		/**<
		 *    The playback of the content is granted.
		*/
		ENUM_TO_STR(DVL_ACCESS_DENIED);
		/**<
		*    The playback of the content is denied.
		*/
		ENUM_TO_STR(DVL_ACCESS_DENIED_CONTENT_EXPIRED);
		/**<
		*    The content's usage rules deny its playback because it is expired.
		*/
		ENUM_TO_STR(DVL_ACCESS_DENIED_INVALID_CREDENTIALS);
		/**<
		*    The provided content Credentials are invalid.
		*/
		ENUM_TO_STR(DVL_ACCESS_DENIED_WRONG_TSID);
		/**<
		*    The transport session identifier is unknown by the SCR at that time.
		*    This status only applies to a playback request.
		*/
		ENUM_TO_STR(DVL_ACCESS_DENIED_LIMIT_EXCEEDED);
		/**<
		*    Deprecated.
		*    This status is not returned by the DVL.
		*/
		ENUM_TO_STR(DVL_ACCESS_DENIED_INVALID_TIME);
		/**<
		*    The internal time reference is not valid. The DVR/VOD Library is not
		*    authorized to play back any content.
		*/
		ENUM_TO_STR(DVL_ACCESS_DENIED_UNKNOWN_STB);
		/**<
		*    The access to the content is denied because it has been
		*    recorded by an unknown set-top box.
		*/
		ENUM_TO_STR(DVL_ACCESS_DENIED_UNKNOWN_HOME_NETWORK);
		/**<
		*    The access to the content is denied because it has been
		*    recorded by a set-top box belonging to an unknow Home Network.
		*/
		ENUM_TO_STR(DVL_ACCESS_DENIED_UNKNOWN_NUID);
		/**<
		*    The access to the content is denied because it has been
		*    recorded by a set-top box with an unknown NUID.
		*/
		ENUM_TO_STR(DVL_ACCESS_DENIED_INVALID_EMI);
		/**<
		*    The EMI used to protect this content is not supported by the SCR
		*    implementation.
		*/
		ENUM_TO_STR(DVL_ACCESS_DENIED_INVALID_KEY_PROTECTION);
		/**<
		*    The keys needed to decipher the content have been protected using a
		*    mechanism not supported by the SCR implementation.
		*/
		ENUM_TO_STR(LAST_DVL_ACCESS_STATUS);
		/**<
		*    Last DVL Playback status value.
		*    This value is never returned by the DVR/VOD Library.
		*/
		default:
			return "Unknown Staus";
	}
}

static void _naMgr_dvlMgr_DvlPairingDataImportation(const TUnsignedInt8 **ppxNasps_ucPairingData)
{
	HxLOG_Print("_naMgr_dvlMgr_DvlPairingDataImportation()...\n", __FUNCTION__);

	if (ppxNasps_ucPairingData == NULL)
	{
		HxLOG_Error("target is null...\n");
		return;
	}

	*ppxNasps_ucPairingData = (TUnsignedInt8 *)NAMGR_GetPairingData();
}

static void _naMgr_dvlMgr_DvlUtcTimeImportation(TUnixDate *pxUtcTime)
{
	HxLOG_Print("_naMgr_dvlMgr_DvlUtcTimeImportation()...\n", __FUNCTION__);

	if (pxUtcTime == NULL)
	{
		HxLOG_Error("target is null...\n");
		return;
	}

	if(VK_CLOCK_GetTime((unsigned long*)pxUtcTime) != VK_OK)
	{
		HxLOG_Error("error in reading VK_CLOCK_GetTime [%d]\n", *pxUtcTime);
#if 0
		*pxUtcTime = 1356998400; /* Tue, 1 Jan 2013 00:00:00 UTC  */
		//*pxUtcTime = 1388534400; /* Tue, 1 Jan 2014 00:00:00 UTC	*/
#endif
		return;
	}
#if 0	// APS Test Suite : time setting <- test only
	{
		DATE_TIME_T stTime;
		HxLOG_Print ("=============== utc time ====================\n");
		//*pxUtcTime = 1363705200-(3600*24*15);//2013/03/19  16:00:00

		HLIB_DATETIME_ConvertUnixTimeToDateTime(*pxUtcTime, &stTime);
		HxLOG_Print ("(%04d/%02d/%02d  %02d:%02d:%02d)\n",
			stTime.stDate.usYear, stTime.stDate.ucMonth, stTime.stDate.ucDay,
			stTime.stTime.ucHour, stTime.stTime.ucMinute, stTime.stTime.ucSecond);
	}
#endif
}


static void _naMgr_dvlMgr_SendKeyDataEvent(void *pvData)
{
	HERROR hErr;
	NaPrmKeyInfo_t *pKeyInfo = (NaPrmKeyInfo_t *)pvData;

	hErr = svc_cas_MgrSubUpdateCasEtcEventData((Handle_t)NULL, (Handle_t)NULL, eDxCAS_GROUPID_NA, eSEVT_CAS_UPDATE_KEYINFO, 0, eDxCAS_GROUPID_NA, (void*)pKeyInfo, sizeof(NaPrmKeyInfo_t));
	if(hErr != ERR_OK)
	{
		HxLOG_Error("svc_cas_MgrSubUpdateCasEtcEvent() failed.\n");
	}
}

static void _naMgr_dvlMgr_RegisterKeyDataCalback(TTransportSessionId xTSid, TUnsignedInt32 ulPlaybackHandle)
{
	HxLOG_Print("[%s] TsId [%d], PlaybackHandle [0x%x]\n", __FUNCTION__, xTSid, ulPlaybackHandle);
#if 1
#if defined(CONFIG_MW_CAS_NAGRA_DALAPI_7_1_ABOVE)
#if !defined(_NA_PRM_KEY_NOTIFY_FROM_NEXUS_)
	CAS_NA_SecRegisterKeyDataCallback(xTSid, ulPlaybackHandle, _naMgr_dvlMgr_SendKeyDataEvent);
#endif
#elif defined(CONFIG_MW_CAS_NAGRA_OP_SES) //&& defined(CONFIG_MW_CAS_NAGRA_PRM_2_6)
	CAS_NA_ScrExtRegisterKeyDataCallback(xTSid, ulPlaybackHandle, _naMgr_dvlMgr_SendKeyDataEvent);
#endif
#else
#if defined(CONFIG_MW_CAS_NAGRA_OP_SES) //&& defined(CONFIG_MW_CAS_NAGRA_PRM_2_6)
	CAS_NA_ScrExtRegisterKeyDataCallback(xTSid, ulPlaybackHandle, _naMgr_dvlMgr_SendKeyDataEvent);
#elif defined(CONFIG_MW_CAS_NAGRA_DALAPI_7_1_ABOVE)
#if !defined(_NA_PRM_KEY_NOTIFY_FROM_NEXUS_)
	CAS_NA_SecRegisterKeyDataCallback(xTSid, ulPlaybackHandle, _naMgr_dvlMgr_SendKeyDataEvent);
#endif
#endif
#endif
}

static void _naMgr_dvlMgr_UnRegisterKeyDataCalback(void)
{
#if 1
#if defined(CONFIG_MW_CAS_NAGRA_DALAPI_7_1_ABOVE)
#if !defined(_NA_PRM_KEY_NOTIFY_FROM_NEXUS_)
	CAS_NA_SecRegisterKeyDataCallback(0, 0, NULL);
#endif
#elif defined(CONFIG_MW_CAS_NAGRA_OP_SES) //&& defined(CONFIG_MW_CAS_NAGRA_PRM_2_6)
	CAS_NA_ScrExtUnRegisterKeyDataCallback();
#endif
#else
#if defined(CONFIG_MW_CAS_NAGRA_OP_SES) //&& defined(CONFIG_MW_CAS_NAGRA_PRM_2_6)
	CAS_NA_ScrExtUnRegisterKeyDataCallback();
#elif defined(CONFIG_MW_CAS_NAGRA_DALAPI_7_1_ABOVE)
#if !defined(_NA_PRM_KEY_NOTIFY_FROM_NEXUS_)
	CAS_NA_SecRegisterKeyDataCallback(0, 0, NULL);
#endif
#endif

#endif
}


/**************************************************************************************************/
#define ________CA_DVL_Public_Functions_Body________________________________________________
/**************************************************************************************************/
HERROR _naMgr_DvlInitialization(void)
{
	TDvlStatus			eDvlStatus;
	TDvlVersionString	xVersionString;
	TUnsignedInt32		xIrdSerialNumberValue;
	TDvlUniqueIdString	xIrdSerialNumberString;
	TUnsignedInt32		xNuidValue;
	TDvlUniqueIdString	xNuidString;
	TDvlChipsetInfo 	*pxChipsetInformation = NULL;

	HxSTD_memset(s_xDvlChipsetType, 0, sizeof(TDvlChipsetTypeString));
	HxSTD_memset(s_xDvlVersion, 0, sizeof(TDvlVersionString));

#if 0
	/* Loading */
	_naMgr_dvlMgr_GetPairingData();
#endif

	/* DVL Initialization */
	eDvlStatus = dvlInitialize(_naMgr_dvlMgr_DvlPairingDataImportation, _naMgr_dvlMgr_DvlUtcTimeImportation);

	if (eDvlStatus != DVL_NO_ERROR)
	{
		HxLOG_Error("dvlInitialize return error [%d]\n", eDvlStatus);
		return ERR_FAIL;
	}

#if defined(CONFIG_MW_CAS_NAGRA_PRM_2_6_DVL_TEST)
	dvlDebugControlRole(DVL_MOD_DVL, DVL_VERBOSITY_MAX, DVL_CLASS_ALL, DVL_DEBUG_ROLE_ALL) ;
#endif
#if 0
	dvlDebugControlRole(DVL_MOD_DVL, DVL_VERBOSITY_MAX, DVL_CLASS_ALL, DVL_DEBUG_ROLE_ALL) ;
	dvlDebugControlRole(DVL_MOD_SCE, DVL_VERBOSITY_MAX, DVL_CLASS_ALL, DVL_DEBUG_ROLE_ALL) ;
#endif

	eDvlStatus = dvlSystemInformationRequest(xVersionString, &xIrdSerialNumberValue, xIrdSerialNumberString, &xNuidValue, xNuidString);
	if(eDvlStatus != DVL_NO_ERROR)
	{
		HxLOG_Error("error in dvlSystemInformationRequest (0x%x)\n", eDvlStatus);
	}

	HxSTD_memcpy(s_xDvlVersion, xVersionString, sizeof(TDvlVersionString));

	eDvlStatus = dvlChipsetInformationRequest((TDvlChipsetInfo**)&pxChipsetInformation);
	if(eDvlStatus != DVL_NO_ERROR)
	{
		HxLOG_Error("error in dvlChipsetInformationRequest (0x%x)\n", eDvlStatus);
	}

	if(pxChipsetInformation != NULL)
	{
		HxSTD_memcpy(s_xDvlChipsetType, pxChipsetInformation->type, sizeof(TDvlChipsetTypeString));
	}

#if defined(CONFIG_MW_CAS_NAGRA_DALTEST) && defined(CONFIG_MW_CAS_NAGRA_DALAPI_7_1_ABOVE) /* DAL Test시에는 필요한 callback을 등록 하는 부부이 없어 Init시 등록 함 */
	_naMgr_dvlMgr_RegisterKeyDataCalback(0, 0);
#endif

	return ERR_OK;
}


/**************************************************************************************************/
#define ________CA_CAK_External_Functions_Body____________________________________________
/**************************************************************************************************/
HERROR CA_NA_DvlInitialze(void)
{
	return _naMgr_DvlInitialization();
}

HERROR CA_NA_GetDvlVersion(TUnsignedInt8** ppxVersion)
{
	if(ppxVersion)
		*ppxVersion = s_xDvlVersion;

	return ERR_OK;
}

HERROR CA_NA_GetDvlChipsetType(TUnsignedInt8** ppxChipsetType)
{
	if(ppxChipsetType)
		*ppxChipsetType = s_xDvlChipsetType;

	return ERR_OK;
}

/*
System Requirements Specification
DVR/VOD Library - PRM
Version 4.0.3

RQ_M-STB-FLASH-CN-0060 V2 :
In the case of a reset to factory settings of the STB, (e.g. in factory in the case of STB
refurbishment or in the field), there shall be a method enabling the STB application to erase
the persistent memory storage assigned to the DVL.
*/
HERROR CA_NA_DvlResetPersistentMemory(void)
{
	HINT32	nErrCode;
	HUINT8	ucFileName[128];
	HUINT32	ulStorageId;

#if 1//(CONFIG_NA_PERSISTENT_MEM == 1)

#if (CONFIG_PROTECT_CW == 1)
#if defined(CONFIG_MW_CAS_NAGRA_PRM)
	ulStorageId = STORAGE_ID_5;
#endif
#else
#if defined(CONFIG_MW_CAS_NAGRA_PRM)
	ulStorageId = STORAGE_ID_3;
#endif
#endif

	HxSTD_snprintf(ucFileName, 128, "%21s%9s%d", CAS_NA_JFFS2_PATH, CAS_FILE_PERSISTENT_MEM, ulStorageId);
	HxLOG_Print("[%s] is removed.\n", ucFileName);
	nErrCode = svc_cas_DevFsDelete(0, ucFileName);
	if (nErrCode)
	{
		HxLOG_Error("error occurred in svc_cas_DevFsDelete(%d).\n", (unsigned int)nErrCode);
		return ERR_FAIL;
	}

	return ERR_OK;

#endif
}

void CA_NA_dvlTerminate(void)
{
	dvlTerminate();

#if defined(CONFIG_MW_CAS_NAGRA_DALTEST) || defined(CONFIG_MW_CAS_NAGRA_DALAPI_7_1_ABOVE)
	_naMgr_dvlMgr_UnRegisterKeyDataCalback();
#endif
}

TDvlStatus CA_NA_dvlSystemInformationRequest(TDvlVersionString xVersionString, TUnsignedInt32* pxIrdSerialNumberValue, TDvlUniqueIdString xIrdSerialNumberString,
														TUnsignedInt32*    pxNuidValue, TDvlUniqueIdString  xNuidString)
{
	TDvlStatus tStatus;

	HxLOG_Trace();
	tStatus = dvlSystemInformationRequest(xVersionString, pxIrdSerialNumberValue, xIrdSerialNumberString, pxNuidValue, xNuidString);
	if (tStatus != DVL_NO_ERROR)
	{
		HxLOG_Error("return error [%d]\n", tStatus);
	}
	HxLOG_Trace();
	return tStatus;
}

TDvlStatus CA_NA_dvlChipsetInformationRequest(TDvlChipsetInfo** ppxChipsetInformation)
{
	TDvlStatus tStatus;

	HxLOG_Trace();
	tStatus = dvlChipsetInformationRequest(ppxChipsetInformation);
	if (tStatus != DVL_NO_ERROR)
	{
		HxLOG_Error("return error [%d]\n", tStatus);
	}
	HxLOG_Trace();

	return tStatus;
}

TDvlStatus CA_NA_dvlHomeNetworkInformationRequest(TUnsignedInt32* pxHomeNetworkIdValue, TDvlUniqueIdString xHomeNetworkIdString, TUnsignedInt8* pxHomeNetworkVersion,
																TUnsignedInt8* pxNumberOfNeighbourhoodInfoItems, const TDvlNeighbourhoodInfo** ppxNeighbourhoodInfoItems)
{
	TDvlStatus tStatus;

	HxLOG_Trace();
	tStatus = dvlHomeNetworkInformationRequest(pxHomeNetworkIdValue, xHomeNetworkIdString, pxHomeNetworkVersion, pxNumberOfNeighbourhoodInfoItems, ppxNeighbourhoodInfoItems);
	if (tStatus != DVL_NO_ERROR)
	{
		HxLOG_Error("return error [%d]\n", tStatus);
	}
	HxLOG_Trace();

	return tStatus;
}

TDvlStatus CA_NA_dvlCreateRecordSessionParameters(TDvlRecordSessionParameters** ppxRecordParameters)
{
	TDvlStatus tStatus;

	HxLOG_Trace();
	tStatus = dvlCreateRecordSessionParameters(ppxRecordParameters);
	if (tStatus != DVL_NO_ERROR)
	{
		HxLOG_Error("return error [%d]\n", tStatus);
	}
	HxLOG_Trace();

	return tStatus;
}

TDvlStatus CA_NA_dvlStartRecordEx(TDvlRecordSessionParameters* pxRecordParameters, TSize* pxOutputCredentialsSize,const TUnsignedInt8** ppxOutputCredentials,
										TDvlRecordStatus* pxRecordingStatus, TDvlHandle** ppxRecordHandle)
{
	TDvlStatus tStatus;

	HxLOG_Trace();

#if defined(CONFIG_DEBUG)
		/* with CA_NA_dvlStopRecord
		TODO : NOVA에서 encryption/decryption 모두를 test하기위해 clear key를 올려주기위해 사용된다.
		 prm vod를 고려해서 다시 진행 필요. 현재 HGS-1000S는 vod를 사용하지 않고 단지 clear key를 nova로 올려주는 test용이므로 vod and prm
		은 따로 구분하지 않음.
		**/
	_naMgr_dvlMgr_RegisterKeyDataCalback(pxRecordParameters->tsId, 0);
#endif
	
  	tStatus = dvlStartRecordEx(pxRecordParameters, pxOutputCredentialsSize, ppxOutputCredentials, pxRecordingStatus, ppxRecordHandle);
	if (tStatus != DVL_NO_ERROR)
	{
		HxLOG_Error("return error [%d]\n", tStatus);
	}
	HxLOG_Trace();

	return tStatus;
}

TDvlStatus CA_NA_dvlStartRecord(TTransportSessionId xTSid, TSize xProfileSize,  const TUnsignedInt8* pxProfile, TSize  xSpecificMetadataSize,  const TUnsignedInt8* pxSpecificMetadata,
        						TUnsignedInt32        xCollectionId, TSize xInputCredentialsSize, const TUnsignedInt8* pxInputCredentials, TDvlRecordErrorCB xRecordErrorCB, TSize* pxOutputCredentialsSize,
	 						const TUnsignedInt8** ppxOutputCredentials, TDvlRecordStatus* pxRecordingStatus, TSize* pxGenericMetadataSize, const TUnsignedInt8** ppxGenericMetadata, TDvlHandle** ppxRecordHandle)
{
	TDvlStatus tStatus;

	HxLOG_Trace();
  	tStatus = dvlStartRecord(xTSid, xProfileSize, pxProfile, xSpecificMetadataSize, pxSpecificMetadata, 
							xCollectionId, xInputCredentialsSize, pxInputCredentials, xRecordErrorCB, pxOutputCredentialsSize,
							ppxOutputCredentials, pxRecordingStatus, pxGenericMetadataSize, ppxGenericMetadata, ppxRecordHandle);
	if (tStatus != DVL_NO_ERROR)
	{
		HxLOG_Error("return error [%d]\n", tStatus);
	}
	HxLOG_Trace();

	return tStatus;	
  	return DVL_NO_ERROR;
}

TDvlStatus CA_NA_dvlStopRecord(TDvlHandle* pxRecordHandle)
{
	TDvlStatus tStatus;

	HxLOG_Trace();

#if defined(CONFIG_DEBUG)
	/* with CA_NA_dvlStartRecordEx
	TODO : NOVA에서 encryption/decryption 모두를 test하기위해 clear key를 올려주기위해 사용된다.
	 prm vod를 고려해서 다시 진행 필요. 현재 HGS-1000S는 vod를 사용하지 않고 단지 clear key를 nova로 올려주는 test용이므로 vod and prm
	은 따로 구분하지 않음.
	**/
	_naMgr_dvlMgr_UnRegisterKeyDataCalback();
#endif

	tStatus = dvlStopRecord(pxRecordHandle);
	if (tStatus != DVL_NO_ERROR)
	{
		HxLOG_Error("return error [%d]\n", tStatus);
	}
	HxLOG_Trace();

	return tStatus;
}

TDvlStatus CA_NA_dvlReleaseBuffer(const TUnsignedInt8* pxBuffer)
{
	TDvlStatus tStatus;

	HxLOG_Trace();
	tStatus = dvlReleaseBuffer(pxBuffer);
	if (tStatus != DVL_NO_ERROR)
	{
		HxLOG_Error("return error [%d]\n", tStatus);
	}
	HxLOG_Trace();

	return tStatus;
}

TDvlStatus CA_NA_dvlCredentialsInformationRequest(TSize xCredentialsSize, const TUnsignedInt8* pxCredentials, TCredentialsType* pxCredentialsType, const TCredentialsUnion** ppxCredentialsUnion,
											TSize* pxGenericMetadataSize, const TUnsignedInt8** ppxGenericMetadata, TSize* pxSpecificMetadataSize, const TUnsignedInt8** ppxSpecificMetadata)
{
	TDvlStatus tStatus;

	HxLOG_Trace();
	tStatus = dvlCredentialsInformationRequest(xCredentialsSize, pxCredentials, pxCredentialsType, ppxCredentialsUnion, pxGenericMetadataSize, ppxGenericMetadata, pxSpecificMetadataSize, ppxSpecificMetadata);
	if (tStatus != DVL_NO_ERROR)
	{
		HxLOG_Error("return error [%d]\n", tStatus);
	}
	HxLOG_Trace();

	return tStatus;
}

TDvlStatus CA_NA_dvlStartPlayback(TTransportSessionId xTSid, TSize xInputCredentialsSize, const TUnsignedInt8* pxInputCredentials, TDvlPlaybackErrorCB xPlaybackErrorCB, TSize* pxOutputCredentialsSize,
								const TUnsignedInt8** ppxOutputCredentials, TDvlAccessStatus* pxPlaybackStatus, TSize* pxGenericMetadataSize, const TUnsignedInt8** ppxGenericMetadata,
								TSize* pxSpecificMetadataSize, const TUnsignedInt8** ppxSpecificMetadata, TDvlHandle** ppxPlaybackHandle)
{
	TDvlStatus tStatus = DVL_NO_ERROR;

	HxLOG_Trace();
	/* VOD Service 일 경우 imma에서 decryption 동작을 하므로 key 정보를 전달하기 위한 callback을 등록함, 일단 PVR일경우에 대해서는 고려치 않음.   */
	_naMgr_dvlMgr_RegisterKeyDataCalback(xTSid, (TUnsignedInt32)0);

	tStatus= dvlStartPlayback(xTSid, xInputCredentialsSize, pxInputCredentials, xPlaybackErrorCB, pxOutputCredentialsSize, ppxOutputCredentials, pxPlaybackStatus, pxGenericMetadataSize,
								ppxGenericMetadata, pxSpecificMetadataSize, ppxSpecificMetadata, ppxPlaybackHandle);

#if 0 //DVL Integration Test 결과 report 작성시 사용하면 편함.
	{
		HCHAR alog[128] = {0,};

		HxSTD_sprintf(alog, "[dvlStartPlayback()], return value = %d[%s], xPlaybackStatus: [%d, %s]\n",
						tStatus, _naMgr_dlvMgr_GetDvlReturnValueName(tStatus), *pxPlaybackStatus, _naMgr_dlvMgr_GetDvlStatusName(*pxPlaybackStatus));
		filelogTest(alog);
	}
#endif

	if (tStatus != DVL_NO_ERROR)
	{
		HxLOG_Error("return error [%d]\n", tStatus);
	}
	HxLOG_Trace();

	return tStatus;
}

TDvlStatus CA_NA_dvlStopPlayback(TDvlHandle* pxPlaybackHandle)
{
	TDvlStatus tStatus = DVL_NO_ERROR;

	HxLOG_Trace();

	/* VOD Service일 경우 등록되었던 callback을 해제함, 일단 PVR일경우에 대해서는 고려치 않음.   */
	_naMgr_dvlMgr_UnRegisterKeyDataCalback();

	tStatus = dvlStopPlayback(pxPlaybackHandle);

#if 0 //DVL Integration Test 결과 report 작성시 사용하면 편함.
	{
		HCHAR alog[128] = {0,};

		HxSTD_sprintf(alog, "[dvlStopPlayback()], return value = %d[%s]\n", tStatus, _naMgr_dlvMgr_GetDvlReturnValueName(tStatus));
		filelogTest(alog);
	}
#endif

	if (tStatus != DVL_NO_ERROR)
	{
		HxLOG_Error("return error [%d]\n", tStatus);
	}
	HxLOG_Trace();

	return tStatus;
}

#if defined(CONFIG_MW_CAS_NAGRA_PRM_3_1)
TDvlStatus CA_NA_dvlPlaybackOpenSession(TTransportSessionId xTSid, TDvlHandle** ppxPlaybackHandle)
{
	TDvlStatus tStatus = DVL_NO_ERROR;

	HxLOG_Trace();

	tStatus = dvlPlaybackOpenSession(xTSid, ppxPlaybackHandle);
	if(tStatus == DVL_NO_ERROR)
	{
		/* VOD Service 일 경우 imma에서 decryption 동작을 하므로 key 정보를 전달하기 위한 callback을 등록함, 일단 PVR일경우에 대해서는 고려치 않음.   */
		_naMgr_dvlMgr_RegisterKeyDataCalback(xTSid, (TUnsignedInt32)*ppxPlaybackHandle);
	}
	else
	{
		HxLOG_Error("return error [%d]\n", tStatus);
	}
	HxLOG_Trace();

	return tStatus;
}

TDvlStatus CA_NA_dvlPlaybackSetEntitlements(TDvlHandle* pxPlaybackHandle, TDvlEntitlementsParameters* pxEntitlementsParameters, TDvlAccessStatus* pxPlaybackStatus, TSize* pxOutputCredentialsSize,
													const TUnsignedInt8** ppxOutputCredentials, TSize* pxKeyIdSize, const TUnsignedInt8** ppxKeyId)
{
	TDvlStatus tStatus;

	HxLOG_Trace();

	HxLOG_Print("ppxOutputCredentials [%p], pxKeyIdSize[%p], ppxKeyId[%p]\n", ppxOutputCredentials, pxKeyIdSize, ppxKeyId);
	tStatus = dvlPlaybackSetEntitlements(pxPlaybackHandle, pxEntitlementsParameters, pxPlaybackStatus, pxOutputCredentialsSize, ppxOutputCredentials, pxKeyIdSize, ppxKeyId);
	if (tStatus != DVL_NO_ERROR)
	{
		HxLOG_Error("return error [%d]\n", tStatus);
	}
	HxLOG_Trace();

	return tStatus;
}

TDvlStatus CA_NA_dvlPlaybackCloseSession(TDvlHandle* pxPlaybackHandle)
{
	TDvlStatus tStatus;

	HxLOG_Trace();
	/* VOD Service일 경우 등록되었던 callback을 해제함, 일단 PVR일경우에 대해서는 고려치 않음.   */
	_naMgr_dvlMgr_UnRegisterKeyDataCalback();

	tStatus = dvlPlaybackCloseSession(pxPlaybackHandle);
	if (tStatus != DVL_NO_ERROR)
	{
		HxLOG_Error("return error [%d]\n", tStatus);
	}
	HxLOG_Trace();

	return tStatus;
}

TDvlStatus CA_NA_dvlPostPrmManagementMessage(TSize xPrmManagementMessageSize,const TUnsignedInt8* pxPrmManagementMessage)
{
	TDvlStatus tStatus;

	HxLOG_Trace();
	tStatus = dvlPostPrmManagementMessage(xPrmManagementMessageSize, pxPrmManagementMessage);
	if (tStatus != DVL_NO_ERROR)
	{
		HxLOG_Error("return error [%d]\n", tStatus);
	}
	HxLOG_Trace();

	return tStatus;
}

TDvlStatus CA_NA_dvlGetAuthenticatedMessage(TSize xCipheredMessageSize, const TUnsignedInt8* pxCipheredMessage, TSize* pxAuthenticatedDataSize, const TUnsignedInt8** ppxAuthenticatedData)
{
	TDvlStatus tStatus;

	HxLOG_Trace();
	tStatus = dvlGetAuthenticatedMessage(xCipheredMessageSize, pxCipheredMessage, pxAuthenticatedDataSize, ppxAuthenticatedData);
	if (tStatus != DVL_NO_ERROR)
	{
		HxLOG_Error("return error [%d]\n", tStatus);
	}
	HxLOG_Trace();

	return tStatus;
}

TDvlStatus CA_NA_dvlOpaqueDataMessageCreate(TOdmHandle** ppxOpaqueDataMessageHandle)
{
	TDvlStatus tStatus;

	HxLOG_Trace();
	tStatus = dvlOpaqueDataMessageCreate(ppxOpaqueDataMessageHandle);
	if (tStatus != DVL_NO_ERROR)
	{
		HxLOG_Error("return error [%d]\n", tStatus);
	}
	HxLOG_Trace();

	return tStatus;
}

TDvlStatus CA_NA_dvlOpaqueDataMessageRelease(TOdmHandle* pxOpaqueDataMessageHandle)
{
	TDvlStatus tStatus;

	HxLOG_Trace();
	tStatus =  dvlOpaqueDataMessageRelease(pxOpaqueDataMessageHandle);
	if (tStatus != DVL_NO_ERROR)
	{
		HxLOG_Error("return error [%d]\n", tStatus);
	}
	HxLOG_Trace();

	return tStatus;
}

TDvlStatus CA_NA_dvlOpaqueDataMessageSetApplicationData(TOdmHandle* pxOpaqueDataMessageHandle,TSize xApplicationDataSize, const TUnsignedInt8* pxApplicationData)
{
	TDvlStatus tStatus;

	HxLOG_Trace();
	tStatus = dvlOpaqueDataMessageSetApplicationData(pxOpaqueDataMessageHandle, xApplicationDataSize, pxApplicationData);
	if (tStatus != DVL_NO_ERROR)
	{
		HxLOG_Error("return error [%d]\n", tStatus);
	}
	HxLOG_Trace();

	return tStatus;
}

TDvlStatus CA_NA_dvlOpaqueDataMessageSetPrmSignalization(TOdmHandle* pxOpaqueDataMessageHandle, TSize xPrmSignalizationSize, const TUnsignedInt8* pxPrmSignalization)
{
	TDvlStatus tStatus;

	HxLOG_Trace();
	tStatus = dvlOpaqueDataMessageSetPrmSignalization(pxOpaqueDataMessageHandle, xPrmSignalizationSize, pxPrmSignalization);
	if (tStatus != DVL_NO_ERROR)
	{
		HxLOG_Error("return error [%d]\n", tStatus);
	}
	HxLOG_Trace();

	return tStatus;
}

TDvlStatus CA_NA_dvlOpaqueDataMessageGet(TOdmHandle* pxOpaqueDataMessageHandle,TSize* pxOpaqueDataMessageSize, const TUnsignedInt8** ppxOpaqueDataMessage)
{
	TDvlStatus tStatus;

	HxLOG_Trace();
	tStatus = dvlOpaqueDataMessageGet(pxOpaqueDataMessageHandle, pxOpaqueDataMessageSize, ppxOpaqueDataMessage);
	if (tStatus != DVL_NO_ERROR)
	{
		HxLOG_Error("return error [%d]\n", tStatus);
	}
	HxLOG_Trace();

	return tStatus;
}
#endif

TDvlStatus CA_NA_dvlUpdateSpecificLcmMetadata(TSize xInLcmCredentialsSize, const TUnsignedInt8* pxInLcmCredentials, TSize xSpecificMetadataSize,
											const TUnsignedInt8* pxSpecificMetadata, TSize* pxOutLcmCredentialsSize, const TUnsignedInt8** ppxOutLcmCredentials)
{
	TDvlStatus tStatus;

	HxLOG_Trace();
	tStatus = dvlUpdateSpecificLcmMetadata(xInLcmCredentialsSize, pxInLcmCredentials, xSpecificMetadataSize,
											pxSpecificMetadata, pxOutLcmCredentialsSize, ppxOutLcmCredentials);
	if (tStatus != DVL_NO_ERROR)
	{
		HxLOG_Error("return error [%d]\n", tStatus);
	}
	HxLOG_Trace();

	return tStatus;
}

TDvlStatus CA_NA_dvlUpdateGenericLcmMetadata(TSize xInLcmCredentialsSize, const TUnsignedInt8* pxInLcmCredentials, TSize xGenericMetadataSize, const TUnsignedInt8* pxGenericMetadata,
												TSize* pxOutLcmCredentialsSize, const TUnsignedInt8**     ppxOutLcmCredentials)
{
	TDvlStatus tStatus;

	HxLOG_Trace();
	tStatus = dvlUpdateGenericLcmMetadata(xInLcmCredentialsSize, pxInLcmCredentials, xGenericMetadataSize, pxGenericMetadata, pxOutLcmCredentialsSize, ppxOutLcmCredentials);
	if (tStatus != DVL_NO_ERROR)
	{
		HxLOG_Error("return error [%d]\n", tStatus);
	}
	HxLOG_Trace();

	return tStatus;
}

TDvlStatus CA_NA_dvlCreateLegacyLcm(TSize xSpecificMetadataSize, const TUnsignedInt8* pxSpecificMetadata, TSize* pxOutputCredentialsSize, const TUnsignedInt8** ppxOutputCredentials)
{
	TDvlStatus tStatus;

	HxLOG_Trace();
	tStatus = dvlCreateLegacyLcm(xSpecificMetadataSize, pxSpecificMetadata, pxOutputCredentialsSize, ppxOutputCredentials);
	if (tStatus != DVL_NO_ERROR)
	{
		HxLOG_Error("return error [%d]\n", tStatus);
	}
	HxLOG_Trace();

	return tStatus;
}

/*** End of File ***/
