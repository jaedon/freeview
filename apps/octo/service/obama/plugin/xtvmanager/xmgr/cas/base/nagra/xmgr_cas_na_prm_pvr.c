/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include "local_include/_xmgr_cas_na_util.h"
#include "local_include/_xmgr_cas_na_prm.h"

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Definitions         **************************/
/*******************************************************************/
// #define TODO_PRM_PVR // TODO : PRM PVR 관련해서 좀더 정리필요.
/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/


/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

XmgrCas_NaCmdRecordSessionInformation_t		stPrmRecInfo[REC_MAX_NUM];

#if defined(CONFIG_DEBUG)
extern void _xmgr_cas_NaCmd_DvlPrintCredentialsInformationt(TCredentialsType    xCredentialsType, TCredentialsUnion* pxCredentialsUnion, TSize xGenericMetadataSize,
				  		TUnsignedInt8* pxGenericMetadata,TSize xSpecificMetadataSize,TUnsignedInt8*     pxSpecificMetadata);

static void naprm_pvr_PrintRecordSessionParameters(TDvlRecordSessionParameters *pxParam)
{
	if(pxParam == NULL)
	{
		HxLOG_Error("[%s:%d] pxParam is fail\n", __FUNCTION__, __LINE__);
		return;
	}
	HxLOG_Debug("\n=====================================================================\n");
	HxLOG_Debug("+			DVL TEST CMD : RECORD SESSION PARAMETER INFO\n");
	HxLOG_Debug("=====================================================================\n");

	HxLOG_Debug("================ Set RecordSessiion Paramers (0x%08x) ================\n", pxParam);
	HxLOG_Debug("+ tsId = 0x%08x\n", pxParam->tsId);
	HxLOG_Debug("+ pErrorCB = 0x%08x\n", pxParam->pErrorCB);
	HxLOG_Debug("+ collectionId = 0x%08x\n", pxParam->collectionId);
	HxLOG_Debug("+ inputCredentialsSize = %d\n", pxParam->inputCredentialsSize);
	if(pxParam->pInputCredentials == NULL)
	{
		HxLOG_Debug("+ pInputCredentials = NULL\n");
	}
	else
	{
		HxLOG_Debug("+ pInputCredentials = No NULL\n");
	}
	HxLOG_Debug("+ specificMetadataSize = %d\n", pxParam->specificMetadataSize);
	if(pxParam->pSpecificMetadata == NULL)
	{
		HxLOG_Debug("+ pInputCredentials = No NULL\n");
	}
	else
	{
		HxLOG_Debug("+ pSpecificMetadata = 0x%08x\n", pxParam->pSpecificMetadata);
	}
	HxLOG_Debug("+ viewingWindowRelativity = %d(%d)\n",  pxParam->viewingWindowRelativity, VIEWING_WINDOW_RELATIVE_TO_CREATION_DATE);
	HxLOG_Debug("+ viewingWindowDuration = 0x%08x\n", pxParam->viewingWindowDuration);
	HxLOG_Debug("+ retentionDuration = 0x%08x\n", pxParam->retentionDuration);
	HxLOG_Debug("+ emi = 0x%08x\n", pxParam->emi);
}
#endif

static void naprm_pvr_MemFree(TUnsignedInt8 * pData)
{
	if(pData != NULL)
	{
		HLIB_MEM_Free(pData);
	}

	pData = NULL;
}

static void naprm_pvr_InitRecordSessionByIndex(int index)
{
	XmgrCas_NaCmdRecordSessionInformation_t *pRecInfo = NULL;

	pRecInfo = &stPrmRecInfo[index];
	if(pRecInfo == NULL)
	{
		HxLOG_Error("[%s:%d] pRecInfo is fail\n", __FUNCTION__, __LINE__);
		return;
	}

	if(pRecInfo->pxRecordParameters)
	{
		xmgr_cas_NadvlReleaseBuffer((TUnsignedInt8 *)pRecInfo->pxRecordParameters);
		pRecInfo->pxRecordParameters = NULL;
	}

	pRecInfo->xTSid = INVALID_TRANSPORTSESSIONID;
	pRecInfo->pxRecordHandle = NULL;

	pRecInfo->xRecordingStatus = DVL_RECORD_GRANTED;
	pRecInfo->stCreInfo.usSize = 0;
	if(pRecInfo->stCreInfo.pucData)
	{
		naprm_pvr_MemFree(pRecInfo->stCreInfo.pucData);
	}
}

static HERROR naprm_pvr_FreeRecordSessionFromTsId(TTransportSessionId xTsId)
{
	int i = 0;
	HERROR hErr = ERR_FAIL;
	XmgrCas_NaCmdRecordSessionInformation_t *pRecInfo = NULL;

	for(i=0;i<REC_MAX_NUM;i++)
	{
		pRecInfo = &stPrmRecInfo[i];
		if(pRecInfo == NULL)
		{
			continue;
		}
		
		if(pRecInfo->xTSid == xTsId)
		{
			naprm_pvr_InitRecordSessionByIndex(i); 
			hErr = ERR_OK;
			HxLOG_Debug("[%s][%d] index[%d] = xTSid(%d)\n", __FUNCTION__, __LINE__, i, xTsId);
			break;
		}
	}

	return hErr;
}

static HERROR naprm_pvr_FreeRecordSessionFromRecordHandle(TDvlHandle* pxRecordHandle)
{
	int i = 0;
	HERROR hErr = ERR_FAIL;
	XmgrCas_NaCmdRecordSessionInformation_t *pRecInfo = NULL;

	for(i=0;i<REC_MAX_NUM;i++)
	{
		pRecInfo = &stPrmRecInfo[i];
		if(pRecInfo == NULL)
		{
			continue;
		}

		if(pRecInfo->pxRecordHandle == pxRecordHandle)
		{
			naprm_pvr_InitRecordSessionByIndex(i);
			HxLOG_Debug("[%s][%d] index[%d] = pxRecordHandle(0x%08x)\n", __FUNCTION__, __LINE__, i, pxRecordHandle);

			hErr = ERR_OK;
			break;
		}
	}
	return hErr;	
}

static XmgrCas_NaCmdRecordSessionInformation_t * naprm_pvr_GetRecordSessionFromTsId(TTransportSessionId xTsId)
{
	int i = 0;
	XmgrCas_NaCmdRecordSessionInformation_t *pRecInfo = NULL;

	for(i=0;i<REC_MAX_NUM;i++)
	{
		pRecInfo = &stPrmRecInfo[i];
		if(pRecInfo == NULL)
		{
			continue;
		}

		if(pRecInfo->xTSid == xTsId)
		{
			HxLOG_Debug("[%s][%d] index[%d] = xTSid(%d)\n", __FUNCTION__, __LINE__, i, xTsId);
			return &stPrmRecInfo[i];
		}
	}
	HxLOG_Debug("[%s][%d] index[%d] = xTSid(%d) is not found\n", __FUNCTION__, __LINE__, i, xTsId);
	return NULL;
}

static XmgrCas_NaCmdRecordSessionInformation_t * naprm_pvr_GetRecordSessionFromRecordHandle(TDvlHandle* pxRecordHandle)
{
	int i = 0;
	XmgrCas_NaCmdRecordSessionInformation_t *pRecInfo = NULL;

	for(i=0;i<REC_MAX_NUM;i++)
	{
		pRecInfo = &stPrmRecInfo[i];
		if(pRecInfo == NULL)
		{
			continue;
		}

		if(pRecInfo->pxRecordHandle == pxRecordHandle)
		{
			HxLOG_Debug("[%s][%d] index[%d] = pxRecordHandle(0x%08x)\n", __FUNCTION__, __LINE__, i, pxRecordHandle);
			return &stPrmRecInfo[i];
		}
	}
	return NULL;	
}

static void naprm_pvr_InitRecordSession(void)
{
	int i = 0;

	for(i=0;i<REC_MAX_NUM;i++)
	{
		naprm_pvr_InitRecordSessionByIndex(i);
	}
}

static HERROR naprm_pvr_MakeRecordSession(TDvlRecordSessionParameters *pxRecordParameters, TTransportSessionId xTSid, TDvlHandle*pxRecordHandle, TSize xCredentialsSize, const TUnsignedInt8* pxCredentials, TDvlRecordStatus xRecordingStatus)
{
	int i = 0;
	HERROR hErr = ERR_FAIL;

	XmgrCas_NaCmdRecordSessionInformation_t *pRecInfo = NULL;

	HxLOG_Trace();

	for(i=0;i<REC_MAX_NUM;i++)
	{
		pRecInfo = &stPrmRecInfo[i];
		if(pRecInfo == NULL)
		{
			continue;
		}

		#if 0 //defined(TODO_PRM_PVR)
		// 아래 // 내용들으 pvr을위해 해야할일이다.
		if(pRecInfo->xTSid == INVALID_TRANSPORTSESSIONID)
		//if(pRecInfo->stCreInfo.pucData == NULL)
		{
			pRecInfo->stCreInfo.pucData = (HUINT8*)HLIB_STD_MemAlloc(xCredentialsSize + 1);
			if(pRecInfo->stCreInfo.pucData)
			{
				pRecInfo->pxRecordParameters = pxRecordParameters;
				pRecInfo->ppxRecordHandle = ppxRecordHandle;
				pRecInfo->xTSid = xTSid;
				pRecInfo->xRecordingStatus = xRecordingStatus;

				HxSTD_memset(pRecInfo->stCreInfo.pucData, 0, xCredentialsSize + 1);
				HxSTD_memcpy(pRecInfo->stCreInfo.pucData, pxCredentials, xCredentialsSize);
				pRecInfo->stCreInfo.usSize = xCredentialsSize;
				
				HxLOG_Debug("[%s][%d] index[%d] = xTSid(%d), pxRecordHandle(0x%08x), CredentialSize(%d), xRecordingStatus(%d)\n", __FUNCTION__, __LINE__, i, xTSid, pxRecordHandle, xCredentialsSize, xRecordingStatus);
				hErr = ERR_OK;
				break;
			}
			else
			{
				HxLOG_Critical("HLIB_STD_MemAlloc Error\n");
			}
		}
		#else
		// 아래 // 내용들으 pvr을위해 해야할일이다.
		if(pRecInfo->xTSid == INVALID_TRANSPORTSESSIONID) 
		{ 
			pRecInfo->pxRecordParameters = pxRecordParameters;
			pRecInfo->pxRecordHandle = pxRecordHandle;
			pRecInfo->xTSid = xTSid;
			HxLOG_Debug("[%s:%d] xTSid(%d), pxRecordParameters(0x%08x), pxRecordHandle(0x%08x)\n", __FUNCTION__, __LINE__, pRecInfo->xTSid, pRecInfo->pxRecordParameters, pRecInfo->pxRecordHandle);
			hErr = ERR_OK;
			break; 
 		}
		#endif 
	}

	return hErr;
}

static HERROR naprm_pvr_GetLcmDataFromCredential(TSize xCredentialsSize, const TUnsignedInt8* pxCredentials, TLcmData *pLcmData)
{
	HERROR hErr = ERR_OK;
	TSize				xGenericMetadataSize;
	TUnsignedInt8		*pxGenericMetadata;
	TSize			 	xSpecificMetadataSize;
	TUnsignedInt8		*pxSpecificMetadata;	
	TCredentialsType 	xCredentialsType;
	TCredentialsUnion	*pxCredentialsUnion;

	HxLOG_Trace();

	hErr = xmgr_cas_NadvlCredentialsInformationRequest(xCredentialsSize, (const TUnsignedInt8 *)pxCredentials, &xCredentialsType, (const TCredentialsUnion **)&pxCredentialsUnion, &xGenericMetadataSize, (const TUnsignedInt8 **)&pxGenericMetadata, &xSpecificMetadataSize, (const TUnsignedInt8 **)&pxSpecificMetadata);

	if(hErr != ERR_OK)
	{
		HxLOG_Error("[%s:%d] This is fail\n", __FUNCTION__, __LINE__);
		return ERR_FAIL;
	}
	else
	{
#if defined(CONFIG_DEBUG)		
		_xmgr_cas_NaCmd_DvlPrintCredentialsInformationt(xCredentialsType, pxCredentialsUnion, xGenericMetadataSize, pxGenericMetadata, xSpecificMetadataSize, pxSpecificMetadata);
#endif

#if 0//defined(UPDATE_SECIFICLCMMETADATA)
		if(pxSpecificMetadata != NULL)
		{
			xmgr_cas_naprm_ParseUsageRule(FALSE, pxSpecificMetadata, &stPlayControl.stUpSMD.stUsageRule);
		}
#endif
		if(xCredentialsType == CREDENTIALS_TYPE_LCM)
		{
			HxSTD_memset(pLcmData, 0, sizeof(TLcmData));
			HxSTD_memcpy(pLcmData, &pxCredentialsUnion->lcmData, sizeof(TLcmData));
		}
		else
		{
			pLcmData = NULL;
		}
		
		if(pxCredentialsUnion != NULL)
		{
			hErr = xmgr_cas_NadvlReleaseBuffer((const TUnsignedInt8 *)pxCredentialsUnion);
			if(hErr != ERR_OK)
			{
				HxLOG_Error("[%s:%d] This is fail\n", __FUNCTION__, __LINE__);
			}
		}

		if(pxGenericMetadata != NULL)
		{
			hErr = xmgr_cas_NadvlReleaseBuffer(pxGenericMetadata);
			if(hErr != ERR_OK)
			{
				HxLOG_Error("[%s:%d] This is fail\n", __FUNCTION__, __LINE__);
			}
		}

		if(pxSpecificMetadata != NULL)
		{
			hErr = xmgr_cas_NadvlReleaseBuffer(pxSpecificMetadata);
			if(hErr != ERR_OK)
			{
				HxLOG_Error("[%s:%d] This is fail\n", __FUNCTION__, __LINE__);
			}
		}
	}

	return ERR_OK;
}

static void naprm_pvr_GetCredentialsInformation(TSize xCredentialsSize, const TUnsignedInt8* pxCredentials)//, HBOOL *pValid)
{
	HERROR hErr = ERR_OK;
	TSize				xGenericMetadataSize;
	TUnsignedInt8		*pxGenericMetadata;
	TSize			 	xSpecificMetadataSize;
	TUnsignedInt8		*pxSpecificMetadata;	
	TCredentialsType 	xCredentialsType;
	TCredentialsUnion	*pxCredentialsUnion;

	HxLOG_Trace();

	hErr = xmgr_cas_NadvlCredentialsInformationRequest(xCredentialsSize, (const TUnsignedInt8 *)pxCredentials, &xCredentialsType, (const TCredentialsUnion **)&pxCredentialsUnion, &xGenericMetadataSize, (const TUnsignedInt8 **)&pxGenericMetadata, &xSpecificMetadataSize, (const TUnsignedInt8 **)&pxSpecificMetadata);

	if(hErr != ERR_OK)
	{
		HxLOG_Error("[%s:%d] This is fail\n", __FUNCTION__, __LINE__);
	}
	else
	{
//		_xmgr_cas_NaCmd_DvlPrintCredentialsInformationt(xCredentialsType, pxCredentialsUnion, xGenericMetadataSize, pxGenericMetadata, xSpecificMetadataSize, pxSpecificMetadata);

#if 0//defined(UPDATE_SECIFICLCMMETADATA)
		if(pxSpecificMetadata != NULL)
		{
			xmgr_cas_naprm_ParseUsageRule(FALSE, pxSpecificMetadata, &stPlayControl.stUpSMD.stUsageRule);
		}
#endif
		
		if(pxCredentialsUnion != NULL)
		{
			hErr = xmgr_cas_NadvlReleaseBuffer((const TUnsignedInt8 *)pxCredentialsUnion);
			if(hErr != ERR_OK)
			{
				HxLOG_Error("[%s:%d] This is fail\n", __FUNCTION__, __LINE__);
			}
		}

		if(pxGenericMetadata != NULL)
		{
			hErr = xmgr_cas_NadvlReleaseBuffer(pxGenericMetadata);
			if(hErr != ERR_OK)
			{
				HxLOG_Error("[%s:%d] This is fail\n", __FUNCTION__, __LINE__);
			}
		}

		if(pxSpecificMetadata != NULL)
		{
			hErr = xmgr_cas_NadvlReleaseBuffer(pxSpecificMetadata);
			if(hErr != ERR_OK)
			{
				HxLOG_Error("[%s:%d] This is fail\n", __FUNCTION__, __LINE__);
			}
		}
	}
}

static void _xmgr_cas_naprm_pvr_RecordErrorCB(TDvlHandle* pxRecordHandle, TDvlRecordStatus  xRecordStatus)
{
	HxLOG_Trace();

	HxLOG_Debug("[%s][%d] pxRecordHandle:[%p], xRecordStatus[%d]\n", __FUNCTION__, __LINE__, pxRecordHandle, xRecordStatus);
	HxLOG_Debug("\n\n\n\n\033[1;92m ############################################################################ \033[0m \n");
	HxLOG_Debug("\t \033[1;92m xRecordStatus : [%s] \033[0m \n", _xmgr_cas_naprm_GetDvlStatusName(xRecordStatus));
	HxLOG_Debug("\033[1;92m ############################################################################ \033[0m \n\n\n\n\n");

	#if 0 // TODO : 아직 TARA & SES와 결정난것이 없음.
	if(pxRecordHandle == NULL)
	{
		HxLOG_Error("[%s:%d] pDrmParam is NULL\n", __FUNCTION__, __LINE__);
		return;
	}

	naprm_pvr_GetRecordSessionFromRecordHandle(pxRecordHandle);
	#endif
}

static HERROR naprm_pvr_StartRecordSession(XmgrCas_NaPrm_SetRecordParameters_t *pDrmParam)
{
	TDvlRecordSessionParameters *pxRecordParameters = NULL;
	TSize xOutputCredentialsSize = 0;
	const TUnsignedInt8* pxOutputCredentials = NULL;
	TDvlRecordStatus xRecordingStatus = DVL_RECORD_GRANTED;
	TDvlHandle* pxRecordHandle = NULL;

	HxLOG_Trace();

	if(pDrmParam == NULL)
	{
		HxLOG_Error("[%s:%d] pDrmParam is NULL\n", __FUNCTION__, __LINE__);
		return ERR_FAIL;
	}

#if defined (CONFIG_MW_CAS_NAGRA_DALAPI_7_1_ABOVE)
	if(xmgr_cas_AddTransportSessionIdDirect(pDrmParam->ulTransportSessionId) != ERR_OK)
	{
		HxLOG_Error("[%s:%d] This is fail\n", __FUNCTION__, __LINE__);
		return ERR_FAIL;
	}
#endif

	if(xmgr_cas_NadvlCreateRecordSessionParameters((TDvlRecordSessionParameters**)&pxRecordParameters) == ERR_OK)
	{
		pxRecordParameters->tsId = pDrmParam->ulTransportSessionId;
		pxRecordParameters->pErrorCB = _xmgr_cas_naprm_pvr_RecordErrorCB;
		pxRecordParameters->collectionId = pDrmParam->ulCollectionId;
		pxRecordParameters->inputCredentialsSize = pDrmParam->stOldLicense.ulSize;
		pxRecordParameters->pInputCredentials = pDrmParam->stOldLicense.pData;
		pxRecordParameters->specificMetadataSize = pDrmParam->stUsageRules.ulSize;
		pxRecordParameters->pSpecificMetadata = pDrmParam->stUsageRules.pData;
		pxRecordParameters->viewingWindowRelativity = VIEWING_WINDOW_RELATIVE_TO_CREATION_DATE; // Multiscreen_DRM_Nagra_API_1.6에 fix되어 있음.
		pxRecordParameters->viewingWindowDuration = pDrmParam->ulDuration;
		pxRecordParameters->retentionDuration = pDrmParam->ulDuration;
		pxRecordParameters->emi = DEFAULT_EMI_FOR_RECORD;

#if defined(CONFIG_DEBUG)
		naprm_pvr_PrintRecordSessionParameters(pxRecordParameters);
#endif

		if(xmgr_cas_NadvlStartRecordEx(pxRecordParameters, &xOutputCredentialsSize, &pxOutputCredentials, &xRecordingStatus, &pxRecordHandle) == ERR_OK)
		{
			if(xOutputCredentialsSize == 0 || pxOutputCredentials == NULL)
			{
				HxLOG_Error("pxOutputCredentials is not fail\n");
				return ERR_FAIL;
			}

#if defined(CONFIG_DEBUG)
			HxLOG_Debug("*\t &pxRecordHandle = 0x%08x, status = %s\n", &pxRecordHandle, _xmgr_cas_naprm_GetDvlStatusName(xRecordingStatus));
			xmgr_cas_naprm_PrintHexDump("Output Credential", (HUINT8*)pxOutputCredentials, (HUINT32)xOutputCredentialsSize);
			naprm_pvr_GetCredentialsInformation(xOutputCredentialsSize, (const TUnsignedInt8 *)pxOutputCredentials);
#endif
#if 0 //defined(TODO_PRM_PVR)
			naprm_pvr_MakeRecordSession(pxRecordParameters, pDrmParam->ulTransportSessionId, pxRecordHandle, xOutputCredentialsSize, pxOutputCredentials, xRecordingStatus);
#else
			naprm_pvr_MakeRecordSession(pxRecordParameters, pDrmParam->ulTransportSessionId, pxRecordHandle, 0, NULL, DVL_RECORD_GRANTED);
#endif
			pDrmParam->stNewLicense.pData = (HUINT8*)HLIB_STD_MemAlloc(xOutputCredentialsSize+1);
			if(pDrmParam->stNewLicense.pData == NULL)
			{
				return ERR_FAIL;
			}

			pDrmParam->stNewLicense.ulSize = xOutputCredentialsSize;
			if(pxOutputCredentials)
			{
				HxSTD_memcpy(pDrmParam->stNewLicense.pData, pxOutputCredentials, sizeof(HUINT8)*xOutputCredentialsSize);
			}
			else
			{
				HxLOG_Error("[%s:%d] pxOutputCredentials is fail\n", __FUNCTION__, __LINE__);
			}

			if(pxOutputCredentials)
			{
				xmgr_cas_NadvlReleaseBuffer(pxOutputCredentials);
			}

			if(pDrmParam->stOldLicense.pData != NULL && pDrmParam->stOldLicense.ulSize > 0)
			{
				if(xRecordingStatus != DVL_RECORD_GRANTED)
				{
					return ERR_FAIL;
				}
			}
		}
		else
		{
			HxLOG_Error("[%s:%d] xmgr_cas_NadvlStartRecordEx is fail\n", __FUNCTION__, __LINE__);
		}
	}
	else
	{
		HxLOG_Error("[%s:%d] xmgr_cas_NadvlCreateRecordSessionParameters is fail\n", __FUNCTION__, __LINE__);
	}

	return ERR_OK;
}

static HERROR naprm_pvr_StopRecordSession(HUINT32 ulTsId)
{
	XmgrCas_NaCmdRecordSessionInformation_t *pRecInfo = NULL;

	HxLOG_Trace();

	pRecInfo = naprm_pvr_GetRecordSessionFromTsId(ulTsId);
	if(pRecInfo  == NULL)
	{
		HxLOG_Error("[%s:%d] pRecInfo is NULL\n", __FUNCTION__, __LINE__);
		return ERR_FAIL;
	}

	HxLOG_Debug("*\t ulTransportSessionId(%d), pxRecordHandle = 0x%08x\n", ulTsId, pRecInfo->pxRecordHandle);
	
	if(xmgr_cas_NadvlStopRecord(pRecInfo->pxRecordHandle) != ERR_OK)
	{
		HxLOG_Error("[%s:%d] pxOutputCredentials is fail\n", __FUNCTION__, __LINE__);
		return ERR_FAIL;
	}

	(void)naprm_pvr_FreeRecordSessionFromTsId(pRecInfo->xTSid);

#if defined (CONFIG_MW_CAS_NAGRA_DALAPI_7_1_ABOVE)
	if(xmgr_cas_RemoveTransportSessionIdDirect(ulTsId) != ERR_OK)
	{
		HxLOG_Error("Error in [xmgr_cas_RemoveTransportSessionIdDirect] TSID[%d]\n", ulTsId);
		return ERR_FAIL;
	}
#endif

	return ERR_OK;
}

#if defined(CONFIG_DEBUG)
void xmgr_cas_naprm_pvr_PrintRecordSessionParameters(TDvlRecordSessionParameters *pxParam)
{
	naprm_pvr_PrintRecordSessionParameters(pxParam);
}
#endif

HERROR xmgr_cas_naprm_pvr_GetLcmDataFromCredential(TSize xCredentialsSize, const TUnsignedInt8* pxCredentials, TLcmData *pLcmData)
{
	return naprm_pvr_GetLcmDataFromCredential(xCredentialsSize, pxCredentials, pLcmData);
}

void xmgr_cas_naprm_pvr_InitRecordSession(void)
{
	naprm_pvr_InitRecordSession();
}

#if defined(CONFIG_SUPPORT_NOVA_APP)
HERROR xmgr_cas_naprm_pvr_NovaGetNewLicense(XmgrCas_NaPrm_SetRecordParameters_t *pDrmParam)
{
	return naprm_pvr_StartRecordSession(pDrmParam);
}

HERROR xmgr_cas_naprm_pvr_NovaStopRecordSession(HUINT32 ulTransportSessionId)
{
	return naprm_pvr_StopRecordSession(ulTransportSessionId);
}

#endif
