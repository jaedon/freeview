/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <xmgr_cas.h>
#include <namgr_main.h>
#include "local_include/_xmgr_cas_na_util.h"
#include "local_include/_xmgr_cas_na_prm.h"
#include "xmgr_novaspread.h"
#include "om_common.h"
#include "mgr_satip.h"

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Definitions         **************************/
/*******************************************************************/

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
// TODO : 아래 struct 하나로 정리.
static xMgrCas_NovaDrmSetParameters_t stDrmLicense;
static XmgrCas_NaCmdCaInfo_t stCaInfo;
static xMgrCas_TransportSessionInfo_t stTransportSessionInfo;

#if defined(CONFIG_DEBUG)
static void prm_PrintDrmSetParameter(xMgrCas_NovaDrmSetParameters_t *pDrmLicense)
{
	if(pDrmLicense == NULL)
	{
		HxLOG_Error("[%s:%d] pDrmLicense is fail\n", __FUNCTION__, __LINE__);
		return;
	}

	HxLOG_Debug("*\t ulTransportSessionId(%d)\n", pDrmLicense->ulTransportSessionId);
	xmgr_cas_naprm_PrintHexDump("XMGR : OldLicense : ", &pDrmLicense->stOldLicense.usData[0], pDrmLicense->stOldLicense.ulSize);
	HxLOG_Debug("*\t ulCollectionId(%d)\n", pDrmLicense->ulCollectionId);
	HxLOG_Debug("*\t ulDuration(%d)\n", pDrmLicense->ulDuration);
	xmgr_cas_naprm_PrintHexDump("XMGR : UsageRules : ", &pDrmLicense->stUsageRules.usData[0], pDrmLicense->stUsageRules.ulSize);
}
#endif

static void prm_NovaPlatformUsageRulesNotifier(HUINT32 ulLength, HUINT8 *pUsageRules)
{
	HxLOG_Trace();

//	(void)BUS_PostData(NULL, eMEVT_CASPRM_NOVA_PLATFORM_USAGERULES_NOTIFIER, HANDLE_NULL, ulLength, 0, (void*) pUsageRules, sizeof(ulLength));
}

static void prm_NovaServiceUsageRulesNotifier(HUINT32 ulTransportSessionId, HUINT32 ulLength, HUINT8 *pUsageRules)
{
	HxLOG_Trace();

//	(void)BUS_PostData(NULL, eMEVT_CASPRM_NOVA_SERVICE_USAGERULES_NOTIFIER, HANDLE_NULL, ulTransportSessionId, ulLength, (void*) pUsageRules, sizeof(ulLength));
}

static void prm_NovaTransportSessionIdInfoNotifier(xMgrCas_TransportSessionInfo_t *pTransportSessionInfo)
{
	HxLOG_Trace();

	if(pTransportSessionInfo == NULL)
	{
		HxLOG_Error("[%s:%d]  ccType is fail\n", __FUNCTION__, __LINE__);
		return;
	}

	(void)BUS_PostData(NULL, eMEVT_CASPRM_NOVA_TRANSPORTSESSIONINFO_NOTIFIER, HANDLE_NULL, 0, 0, (void*) pTransportSessionInfo, sizeof(xMgrCas_TransportSessionInfo_t));
}

static void prm_NovaLicenseNotifier(HUINT32 ulTransportSessionId, XmgrCas_NaPrmCredential_t *pLicense, HBOOL bRequestLicenseAgain)
{
	HUINT32 p2 = 0;

	HxLOG_Trace();

	if(pLicense == NULL)
	{
		HxLOG_Error("[%s:%d]  pLicense is fail\n", __FUNCTION__, __LINE__);
		return;
	}

	p2 += 	(bRequestLicenseAgain << 31) & 0x80000000;
	p2 += 	(ulTransportSessionId << 16) & 0x7fff0000;
	p2 +=  	pLicense->ulSize & 0xffff;

#if defined(CONFIG_DEBUG)
	HxLOG_Debug("*\t ulTransportSessionId(%d)\n", ulTransportSessionId);
	HxLOG_Debug("*\t bRequestLicenseAgain(0x%08x)\n", bRequestLicenseAgain);
	HxLOG_Debug("*\t pLicense->ulSize(%d)\n", pLicense->ulSize);
	if(pLicense->pData)
	{
		HxLOG_Debug("*\t ucData = { %02x, %02x, %02x, ... }\n", pLicense->pData[0], pLicense->pData[1], pLicense->pData[2]);	
	}
	else
	{
		HxLOG_Error("[%s:%d]  pLicense Data  is fail\n", __FUNCTION__, __LINE__);
	}
#endif

	(void)BUS_PostData(NULL, eMEVT_CASPRM_NOVA_LICENSE_NOTIFIER, HANDLE_NULL, 0, p2, (void*)pLicense->pData, pLicense->ulSize);
}

/*
	NOTE :NaPrmPvrKeyInfo_t안의 구조가 변경되다면 수신하는 곳의 구조도 동일하게 변경해주어야한다.
	- nova_cas.h 에서 NOVA_CAS_KeyHandlerInfo_t 변경 필요.
**/
static HERROR prm_UpdatePvrKeyHandler(HINT32 message, NaPrmPvrKeyInfo_t *pstKeyInfo)
{
	if(pstKeyInfo == NULL)
	{
		HxLOG_Error("*\t pstKeyInfo : fail\n");
		return ERR_FAIL;
	}

	HxLOG_Debug("@@@@@@ DrmKeyUpdate @@@@@@@@\n");
	HxLOG_Debug("@ xTSid(%d) : KeyHander = 0x%08x\n", pstKeyInfo->xTSid, pstKeyInfo->ulKeyHandler);

	(void)BUS_PostData(NULL, eMEVT_CASPRM_NOVA_KEYHANDLER_NOTIFIER, HANDLE_NULL, HANDLE_NULL, sizeof(NaPrmPvrKeyInfo_t), (void*)pstKeyInfo, sizeof(NaPrmPvrKeyInfo_t));

	return ERR_OK;
}

static HERROR prm_NovaDrmServiceUsageRules(Handle_t hAction, HUINT32 p1, HUINT32 p2, HUINT32 p3)
{
	HUINT32 ulTransportSessionId= 0;
	HUINT8 usUR[EX_COPY_CONTROL_BYTE] = { 0, 0, 0};
	DxCopyrightControl_t	stDrmInfo;
	DxCopyrightControl_CasCcInfo_t *pCasInfo = NULL;
	DxCopyrightControl_CasCcType_e			eCasCcType;

	HxLOG_Trace();

	HxSTD_MemSet(&stDrmInfo, 0, sizeof(DxCopyrightControl_t));
	HxSTD_MemCopy(&stDrmInfo, p3, sizeof(DxCopyrightControl_t));

	eCasCcType = stDrmInfo.stCasCcInfo.eCasCcType;
	if(eCasCcType != eDxCopyrightControl_CasCcType_NagraSes)
	{
		HxLOG_Info("[%s:%d] URI Info is not for Nagra...\n", __FUNCTION__, __LINE__);
		return ERR_OK;
	}

	pCasInfo = &stDrmInfo.stCasCcInfo;
	if(pCasInfo== NULL)
	{
		HxLOG_Error("[%s:%d]  pCasInfo is fail\n", __FUNCTION__, __LINE__);
		return ERR_FAIL;
	}

	ulTransportSessionId = hAction;

	usUR[0] = pCasInfo->info.stNagraDrmInfo_Ses.ucControlByteA;
	usUR[1] = pCasInfo->info.stNagraDrmInfo_Ses.ucControlByteB;
	usUR[2] = 0;//pCasInfo->info.stNagraDrmInfo_Ses.ucControlByteC;

	HxLOG_Debug("*\t ulTransportSessionId  = 0x%08x\n",ulTransportSessionId);
	HxLOG_Debug("*\t ServiceUsageRules = [ %02x, %02x, %02x ]\n",usUR[0], usUR[1], usUR[2]);

	prm_NovaServiceUsageRulesNotifier(ulTransportSessionId, EX_COPY_CONTROL_BYTE, &usUR[0]);

	return ERR_OK;
}

static HERROR prm_NovaDrmRelease(HUINT32 p1, HUINT32 p2, HUINT32 p3)
{
	HUINT32 ulTransportSessionId = 0;	

	HxLOG_Trace();

	ulTransportSessionId = p1;

	if(xmgr_cas_naprm_pvr_NovaStopRecordSession(ulTransportSessionId) != ERR_OK)
	{
		HxLOG_Error("*\t xmgr_cas_naprm_pvr_NovaStopRecordSession is fail\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

static HERROR prm_NovaDrmHandle(Handle_t hAction, Handle_t hSvcHandle)
{
	TTransportSessionId		xTsId;
	DbSvc_Info_t		stDbSvcInfo;

	HxLOG_Trace();
	HxLOG_Debug("*\t hAction(0x%08x), hSvcHandle(0x%08x)\n", hAction, hSvcHandle);

	if(MGR_SATIP_GetDrmFlag(svc_cas_SubSvcRmGetActionId(hAction)) == FALSE)
	{
		HxLOG_Debug("*\t This hAction(0x%08x) is not cas channel for Multiscreen \n", hAction);
		return ERR_FAIL;
	}

	HxSTD_memset(&stTransportSessionInfo, 0, sizeof(xMgrCas_TransportSessionInfo_t));

	HxSTD_memset(&stDbSvcInfo, 0, sizeof(DbSvc_Info_t));
	if (DB_SVC_GetServiceInfo(hSvcHandle, &stDbSvcInfo) != ERR_OK)
	{
		HxLOG_Error("*\t svc_cas_SubSvcDbGetSvcInfoByServiceHandle : hSvc:0x%X\n", hSvcHandle);
		return ERR_FAIL;
	}

	if(svc_cas_SubSvcRmGetResourceId(svc_cas_SubSvcRmGetActionId(hAction), eCAS_RM_ITEM_DEMUX, (HUINT32 *)&xTsId) != ERR_OK)
	{
		HxLOG_Error("*\t svc_cas_SubSvcRmGetResourceId :  hSvc:0x%X\n", hAction);
		return ERR_FAIL;
	}

	stTransportSessionInfo.hAction = hAction;
	stTransportSessionInfo.ulTransportSessionId = xTsId;
	stTransportSessionInfo.usOnId = stDbSvcInfo.stDxInfo.onid;
	stTransportSessionInfo.usTsId = stDbSvcInfo.stDxInfo.tsid;
	stTransportSessionInfo.usSvcId = stDbSvcInfo.stDxInfo.svcid;
	stTransportSessionInfo.usAudioPid = stDbSvcInfo.stDxInfo.audioPid;
	stTransportSessionInfo.usVideoPid = stDbSvcInfo.stDxInfo.videoPid;

#if defined(CONFIG_DEBUG)
	HxLOG_Debug("*\t uid(0x%08x), name(%s) PMTPid(0x%08x), VPid(0x%08x), APid(0x%08x), PCRPid(0x%08x)\n"
		, stDbSvcInfo.stDxInfo.uid
		, &stDbSvcInfo.stDxInfo.name[0]
		, stDbSvcInfo.stDxInfo.pmtPid
		, stDbSvcInfo.stDxInfo.videoPid
		, stDbSvcInfo.stDxInfo.audioPid
		, stDbSvcInfo.stDxInfo.pcrPid);
	HxLOG_Debug("*\t transportSessionId(%d) : usOnId(0x%08x), usTsId(0x%08x), usSvcId(0x%08x) \n"
		, stTransportSessionInfo.ulTransportSessionId
		, stDbSvcInfo.stDxInfo.onid
		, stDbSvcInfo.stDxInfo.tsid
		, stDbSvcInfo.stDxInfo.svcid);
#endif

	prm_NovaTransportSessionIdInfoNotifier(&stTransportSessionInfo);

	return ERR_OK;
}

static HERROR prm_NovaDrmSetParameter(HUINT32 p1, HUINT32 p2, HUINT32 p3)
{
	XmgrCas_NaPrm_SetRecordParameters_t stSetRecordParam;
	HBOOL bRequestLicenseAgain = FALSE;

	HxLOG_Trace();

	HxSTD_MemSet(&stDrmLicense, 0, sizeof(xMgrCas_NovaDrmSetParameters_t));
	HxSTD_memcpy(&stDrmLicense, (void *)p3, sizeof(xMgrCas_NovaDrmSetParameters_t));

#if defined(CONFIG_DEBUG)
	prm_PrintDrmSetParameter(&stDrmLicense);
#endif

#if 0//defined (CONFIG_MW_CAS_NAGRA_DALAPI_7_1_ABOVE)
	if(xmgr_cas_AddTransportSessionIdDirect(stDrmLicense.ulTransportSessionId) != ERR_OK)
	{
		HxLOG_Error("[%s:%d] This is fail\n", __FUNCTION__, __LINE__);
		goto GOTONOVA;
	}
#endif

	HxSTD_MemSet(&stSetRecordParam, 0, sizeof(XmgrCas_NaPrm_SetRecordParameters_t));
	stSetRecordParam.ulTransportSessionId = stDrmLicense.ulTransportSessionId;

#if 0
	pRecInfo = prm_MakeRecordInfoFromTsId(stSetRecordParam.ulTransportSessionId);
	if(pRecInfo == NULL)
	{
		HxLOG_Error("[%s:%d] This is fail\n", __FUNCTION__, __LINE__);
		goto GOTONOVA;
	}
	pRecInfo->ulTransportSessionId =  stSetRecordParam.ulTransportSessionId;
#endif	
	
	stSetRecordParam.ulCollectionId = stDrmLicense.ulCollectionId;
	stSetRecordParam.ulDuration = stDrmLicense.ulDuration;
	stSetRecordParam.stUsageRules.ulSize = stDrmLicense.stUsageRules.ulSize;
	if(stSetRecordParam.stUsageRules.ulSize > 0)
	{
		stSetRecordParam.stUsageRules.pData = (HUINT8*)HLIB_STD_MemAlloc(stSetRecordParam.stUsageRules.ulSize+1);
		HxSTD_memcpy(stSetRecordParam.stUsageRules.pData, &stDrmLicense.stUsageRules.usData[0], sizeof(HUINT8)*stSetRecordParam.stUsageRules.ulSize);
	}

	// OldLicense를 이용가능한지 check한다. 이용가능하면 OldLicense를 다시 nova로 전달한다. 이용가능하지 않으면 NULL로 return하여 다시 xmgr_cas_na_prm_NovaDrmSetParameter를 call할수 
	// 있도록 한다.
	if(stDrmLicense.stOldLicense.usData && stDrmLicense.stOldLicense.ulSize > 0)
	{
		HxLOG_Debug("*\t OldLIcense[%d] is exist\n", stDrmLicense.stOldLicense.ulSize);
		stSetRecordParam.stOldLicense.pData = &stDrmLicense.stOldLicense.usData[0];
		stSetRecordParam.stOldLicense.ulSize = stDrmLicense.stOldLicense.ulSize;
	}

	if(xmgr_cas_naprm_pvr_NovaGetNewLicense(&stSetRecordParam) != ERR_OK)
	{
#if defined (CONFIG_MW_CAS_NAGRA_DALAPI_7_1_ABOVE)
		if(xmgr_cas_RemoveTransportSessionIdDirect(stDrmLicense.ulTransportSessionId) != ERR_OK)
		{
			HxLOG_Error("Error in [xmgr_cas_RemoveTransportSessionIdDirect] TSID[%d]\n", stDrmLicense.ulTransportSessionId);
		}
#endif
		bRequestLicenseAgain = TRUE;
		HxLOG_Error("[%s:%d] This is fail\n", __FUNCTION__, __LINE__);
	}

//GOTONOVA:
	prm_NovaLicenseNotifier(stDrmLicense.ulTransportSessionId, &stSetRecordParam.stNewLicense, bRequestLicenseAgain);

	if(stSetRecordParam.stUsageRules.pData)
	{
		HLIB_STD_MemFree(stSetRecordParam.stUsageRules.pData);
		stSetRecordParam.stUsageRules.pData = NULL;
	}

	if(stSetRecordParam.stNewLicense.pData)
	{
		HLIB_STD_MemFree(stSetRecordParam.stNewLicense.pData);
		stSetRecordParam.stNewLicense.pData = NULL;
	}

	return ERR_OK;
}

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/
#define _____PUBLIC_FUNCTION______________________________________________________
HERROR xmgr_cas_na_prm_NovaCaInfo(HUINT8 *pucData)
{
	HxLOG_Trace();

	if(pucData == NULL)
	{
		return ERR_FAIL;
	}	

	xmgr_cas_NaUpdateSystemInfo();

	if(xmgr_cas_NaGetSystemInfo((XmgrCas_NaSystemInfoItem_e)eNaSysInfoItem_Chipset_Nuid, &stCaInfo.ucChipsetUid[0], MAX_CAS_INFO_STRING_LEGTH, FALSE) != ERR_OK)
	{
		HxLOG_Error("[%s:%d]  is fail\n", __FUNCTION__, __LINE__);
	}

	sprintf(stCaInfo.ucCaVendor, "%s", "Nagra"); // TODO : 어떻게 얻는攘?

	if(xmgr_cas_NaGetSystemInfo((XmgrCas_NaSystemInfoItem_e)eNaSysInfoItem_Chipset_Type, &stCaInfo.ucChipsetType[0], MAX_CAS_INFO_STRING_LEGTH, FALSE) != ERR_OK)
	{
		HxLOG_Error("[%s:%d]  is fail\n", __FUNCTION__, __LINE__);
	}

	if(xmgr_cas_NaGetSystemInfo((XmgrCas_NaSystemInfoItem_e)eNaSysInfoItem_Chipset_Rev, &stCaInfo.ucChipsetRevision[0], MAX_CAS_INFO_STRING_LEGTH, FALSE) != ERR_OK)
	{
		HxLOG_Error("[%s:%d]  is fail\n", __FUNCTION__, __LINE__);
	}

	if(xmgr_cas_NaGetSystemInfo((XmgrCas_NaSystemInfoItem_e)eNaSysInfoItem_caVersion, &stCaInfo.ucCaVersion[0], MAX_CAS_INFO_STRING_LEGTH, FALSE) != ERR_OK)
	{
		HxLOG_Error("[%s:%d]  is fail\n", __FUNCTION__, __LINE__);
	}

	if(xmgr_cas_NaGetSystemInfo((XmgrCas_NaSystemInfoItem_e)eNaSysInfoItem_CA_Serial_Number, &stCaInfo.ucCaNumber[0], MAX_CAS_INFO_STRING_LEGTH, FALSE) != ERR_OK)
	{
		HxLOG_Error("[%s:%d]  is fail\n", __FUNCTION__, __LINE__);
	}

	if(xmgr_cas_NaGetSCRemoved())
	{
		stCaInfo.ucSmartcardInserted = FALSE;
	}
	else
	{
		stCaInfo.ucSmartcardInserted = TRUE;
	}

	XMGR_CAS_NaIsHdPlusSmartCard(0, &stCaInfo.ucSmartcardSuitable);

	if(xmgr_cas_NaGetSystemInfo((XmgrCas_NaSystemInfoItem_e)eNaSysInfoItem_iccVersion, &stCaInfo.ucSmartcardType[0], MAX_CAS_INFO_STRING_LEGTH, FALSE) != ERR_OK)
	{
		HxLOG_Error("[%s:%d]  is fail\n", __FUNCTION__, __LINE__);
	}

	if(xmgr_cas_NaGetSystemInfo((XmgrCas_NaSystemInfoItem_e)eNaSysInfoItem_iccSerial_Number, &stCaInfo.ucSmartcardNumber[0], MAX_CAS_INFO_STRING_LEGTH, FALSE) != ERR_OK)
	{
		HxLOG_Error("[%s:%d]  is fail\n", __FUNCTION__, __LINE__);
	}

	stCaInfo.ucSmartcardStatus =  1; // TODO 확인 필요

	if(stCaInfo.ucSmartcardStatus == 1/*activated*/) // TODO
	{
		xmgr_cas_NaGetExpirationDate(&stCaInfo.ulExpirationDate);
	}
	else
	{
		stCaInfo.ulExpirationDate = 0;
	}

	HxSTD_MemSet(pucData, 0, sizeof(XmgrCas_NaCmdCaInfo_t));
	HxSTD_memcpy(pucData, (HUINT8 *)&stCaInfo, sizeof(XmgrCas_NaCmdCaInfo_t));

#if defined(CONFIG_DEBUG)
	/* Example
	cs-uid="22 2222 0815 12"
	cs-type="12 3456 789A BC"
	cs-revision="A1"
	ca-vendor="Nagra"
	ca-version="R-AHNAH-ADFBE-DOLAI"
	ca-number="00 4711 0815 12"
	sc-inserted="true"
	sc-suitable="true"
	sc-type="DNASP142 RevGC6"
	sc-number="17 0815 0816 69"
	sc-status="activated"
	expiration="2016-06-19"
	**/

	HxLOG_Debug("*\t pChipsetUid = %s\n", stCaInfo.ucChipsetUid);
	HxLOG_Debug("*\t pucCaVendor = %s\n", stCaInfo.ucCaVendor);
	HxLOG_Debug("*\t pucChipsetType = %s\n", stCaInfo.ucChipsetType);
	HxLOG_Debug("*\t pucChipsetRevision = %s\n",  stCaInfo.ucChipsetRevision);
	HxLOG_Debug("*\t pucCaVersion = %s\n", stCaInfo.ucCaVersion);
	HxLOG_Debug("*\t pucCaNumber = %s\n", stCaInfo.ucCaNumber);
	HxLOG_Debug("*\t pbSmartcardInserted = %d\n",stCaInfo.ucSmartcardInserted);
	HxLOG_Debug("*\t pbSmartcardSuitable = %d\n",stCaInfo.ucSmartcardSuitable);
	HxLOG_Debug("*\t pucCaNumber = %s\n",stCaInfo.ucSmartcardType);
	HxLOG_Debug("*\t pucSmartcardNumber = %s\n",stCaInfo.ucSmartcardNumber);
	HxLOG_Debug("*\t pucSmartcardStatus = %d\n", stCaInfo.ucSmartcardStatus);
	HxLOG_Debug("*\t pulExpirationDate = %d\n\n", stCaInfo.ulExpirationDate);
#endif

	return ERR_OK;
}

BUS_Result_t 	xproc_cas_NaPrmNovaMain(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t			eBusResult = ERR_BUS_NO_ERROR;

	switch(message)
	{
		case eMEVT_BUS_CREATE :
			xmgr_cas_naprm_pvr_InitRecordSession();
			break;

		case eMEVT_BUS_TIMER :
			break;

		case eMEVT_BUS_READY_SHUTDOWN:
			return MESSAGE_PASS;

		case eMEVT_BUS_DESTROY :
			break;

		case eSEVT_CAS_UPDATE_NOVA_DRM_HANDLE:
			if(prm_NovaDrmHandle(hAction, p1) != ERR_OK)
			{
				HxLOG_Debug("*\t Noti : This hAction(0x%08x) is not cas channel for Multiscreen \n", hAction);
			}
			break;
		case eOEVT_CAS_NA_NOVA_DRMSETPARAMETER:
			if(prm_NovaDrmSetParameter(p1, p2, p3) != ERR_OK)
			{
				HxLOG_Error("Error in eOEVT_CAS_NA_NOVA_DRMSETPARAMETER()\n");
			}
			break;

		case eOEVT_CAS_NA_NOVA_DRMRELEASE:
			if(prm_NovaDrmRelease(p1, p2, p3) != ERR_OK)
			{
				HxLOG_Error("Error in eOEVT_CAS_NA_NOVA_DRMRELEASE()\n");
			}
			break;

#if defined(_NA_PRM_PVR_KEY_NOTIFY_FRM_NEXUS_)
		case eMEVT_CAS_PRM_PVR_UPDATE_KEYINFO:
			{
				NaPrmPvrKeyInfo_t stKeyInfo;

				HxSTD_MemSet(&stKeyInfo, 0, sizeof(NaPrmPvrKeyInfo_t));
				HxSTD_memcpy(&stKeyInfo, (void *)p3, sizeof(NaPrmPvrKeyInfo_t));
				stKeyInfo.ulKeyLength = 0;

				if(prm_UpdatePvrKeyHandler(eMEVT_CAS_PRM_PVR_UPDATE_KEYINFO, &stKeyInfo) != ERR_OK)
				{
					HxLOG_Error("Error in eMEVT_CAS_PRM_PVR_UPDATE_KEYINFO()\n");
				}
			}
			break;
#endif
		case eMEVT_CAS_UPDATE_KEYINFO:
			{
#if defined(CONFIG_DEBUG)
				NaPrmKeyInfo_t stKeyDataInfo;
				NaPrmPvrKeyInfo_t stKeyInfo;

				HxSTD_MemSet(&stKeyDataInfo, 0, sizeof(NaPrmKeyInfo_t));
				HxSTD_memcpy(&stKeyDataInfo, (void *)p3, sizeof(NaPrmKeyInfo_t));
				HxSTD_MemSet(&stKeyInfo, 0, sizeof(NaPrmPvrKeyInfo_t));

				if(stKeyDataInfo.eKeyType != ePRM_KEYTYPE_CLEARTEXTKEY)
				{
					break;
				}

				stKeyInfo.xTSid = stKeyDataInfo.xTSid;
				stKeyInfo.ulKeyHandler = stKeyDataInfo.ulKeyHandler;

				if(stKeyDataInfo.ulKeyLength > 0  && stKeyDataInfo.ulKeyLength < MAX_PRMKEY_LENGTH)
				{
					stKeyInfo.ulKeyLength = stKeyDataInfo.ulKeyLength;
					HxSTD_memcpy(&stKeyInfo.ucKey[0], &stKeyDataInfo.ucKey[0], sizeof(stKeyDataInfo.ucKey));
				}
				
				if(prm_UpdatePvrKeyHandler(eMEVT_CAS_UPDATE_KEYINFO, &stKeyInfo) != ERR_OK)
				{
					HxLOG_Error("Error in eMEVT_CAS_PRM_PVR_UPDATE_KEYINFO()\n");
				}
#endif				
				break;
			}

		case eSEVT_CAS_DRM_SIGNALLED:
			break;

		case eOEVT_CAS_NA_NOVA_DRMSETNOTIFIER:
			break;

		case eOEVT_CAS_NA_NOVA_CASERVICEUSAGERULES:
			break;

		case eOEVT_CAS_NA_NOVA_CAPLATFORMUSAGERULES:
			break;

		default:
			break;
	}

	if(eBusResult != ERR_BUS_NO_ERROR)
	{
		return eBusResult;
	}

	return BUS_ProcessMessageDefault(message, hAction, p1, p2, p3);
}
