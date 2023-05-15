/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#ifdef DxMAX_TS_NUM
#undef DxMAX_TS_NUM
#endif
#include <hlib.h>

#include <mgr_cas.h>
#include <xmgr_cas.h>
#include <mgr_action.h>

#include <xmgr_cas_res_str.h>
#include <svc_cas.h>
#include <svc_sys.h>
#include <db_svc.h>
#include <db_param.h>
#include <isosvc.h>

#include <namgr_main.h>

#include "local_include/_xmgr_cas_na_util.h"
#include "local_include/_xmgr_cas_na_adapt.h"
#include "local_include/_xmgr_cas_na.h"


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

typedef struct tagSystemInfo
{
	TSystemInfoStatus 	status;
	TIrdInfo			*pIrdInfo;
	TUnsignedInt8 		nNumberOfSmartcards;
	TSmartcardInfoItem	*aSmartcardInfoArray;
	NaChipsetInfo_t		*pstChipInfo;
	TUnsignedInt8 		nCscMaxIndex;
#if defined(CONFIG_MW_CAS_NAGRA_PRM)
	TDvlVersionString	*pxDvlVersion;
	TDvlChipsetTypeString *pxDvlChipsetType;
#endif
} XmgrCas_NaUiSystemInfoData_t;

#if defined(CONFIG_MW_CAS_NAGRA_PRM)
// 아래는 다시 정리 필요...
#if 0
static TDvlChipsetInfo 		stxChipsetInformation;
static HUINT8     			stxPlaybackSpecificMetadata[50], stxCredentialSpecificMetadata[50];
static TDvlHandle 			*pstxPlaybackHandle;
static TCredentialsUnion  	stxCredentialsUnion;
static NA_CODEC_DvlStartPlaybackInfo_t stDvlStartPlaybackInfo;
static NA_CODEC_DvlCredentialsInformationRequestInfo_t stCredentaisInfoRequestInfo;
#endif
#endif

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC XmgrCas_NaUiSystemInfoData_t	s_stNaUiSystemInfo;


/*******************************************************************/
/********************      Local Functions      ********************/
/*******************************************************************/
#define _____XMGR_CAS_NA_UTIL_LOCAL_FUNCTION______________________________________________________

STATIC XmgrCas_NaUiSystemInfoData_t	*_xmgr_cas_NaGetSystemInfoData(void)
{
	return &s_stNaUiSystemInfo;
}

STATIC HERROR	_xmgr_cas_NaCmdGetCountrycode(HUINT8* pszCurLang1, HUINT8* pszCurLang2)
{
	HERROR				hError;
	HxLANG_Id_e			eLangId = 0;
	HINT32				nMatchedCount = 0;

//	eLangId = MWC_PARAM_GetMainLanguage();
	hError = DB_PARAM_GetItem(eDB_PARAM_ITEM_LANG_MAIN, (HUINT32 *)&eLangId, 0);
	if (ERR_OK != hError)
	{
		HxLOG_Warning("DB_PARAM_GetItem(eDB_PARAM_ITEM_LANG_MAIN) Error!!\n");
	}

	hError = MWC_UTIL_Get639CodeByLangIdx((const HxLANG_Id_e)eLangId, &nMatchedCount, pszCurLang1, pszCurLang2, pszCurLang2);
	if(hError != ERR_OK)
	{
		HxLOG_Print("MWC_UTIL_Get639CodeByLangIdx fail \n");
		return hError;
	}

	return ERR_OK;
}

STATIC HERROR	_xmgr_cas_NaGetCAProjectName(HUINT8* paucCaSer, HINT32 nLength)
{
	XmgrCas_NaUiSystemInfoData_t	*pstNaUiSystemInfo = NULL;

	pstNaUiSystemInfo = _xmgr_cas_NaGetSystemInfoData();
	if (pstNaUiSystemInfo->pIrdInfo != NULL)
	{
		if (pstNaUiSystemInfo->pIrdInfo->pProjectName != NULL)
		{
			HxLOG_Print("eCasNaSCInfo_Project_Name[%s]\n",
									pstNaUiSystemInfo->pIrdInfo->pProjectName);
			MWC_UTIL_DvbStrncpy(paucCaSer, pstNaUiSystemInfo->pIrdInfo->pProjectName, nLength - 1);

			return ERR_OK;
		}
	}

	return ERR_FAIL;
}

HERROR	xmgr_cas_NaGetCANumber(HUINT8* paucCaSer)
{
	XmgrCas_NaUiSystemInfoData_t	*pstNaUiSystemInfo = NULL;

	pstNaUiSystemInfo = _xmgr_cas_NaGetSystemInfoData();
	if (pstNaUiSystemInfo->pIrdInfo != NULL)
	{
		if (pstNaUiSystemInfo->pIrdInfo->pSerialNumber != NULL)
		{
			HxLOG_Print("eCasNaSCInfo_CA_Serial_Number[%s]\n",
							pstNaUiSystemInfo->pIrdInfo->pSerialNumber);
			MWC_UTIL_DvbStrncpy(paucCaSer, pstNaUiSystemInfo->pIrdInfo->pSerialNumber, NA_UI_CA_NUMBER_STRING_LEN - 1);

			return ERR_OK;
		}
	}

	return ERR_FAIL;
}

HERROR	xmgr_cas_NaGetCardSerialNumber(HUINT8* paucCardSer)
{
	XmgrCas_NaUiSystemInfoData_t	*pstNaUiSystemInfo = NULL;

	pstNaUiSystemInfo = _xmgr_cas_NaGetSystemInfoData();
	if ( pstNaUiSystemInfo->nNumberOfSmartcards > 0)
	{
		if (pstNaUiSystemInfo->aSmartcardInfoArray != NULL )
		{
			if (pstNaUiSystemInfo->aSmartcardInfoArray[0].pSerialNumber)
			{
				HxLOG_Error("eCasNaSCInfo_iccSerial_Number[%s]\n", pstNaUiSystemInfo->aSmartcardInfoArray[0].pSerialNumber);
				MWC_UTIL_DvbStrncpy(paucCardSer, pstNaUiSystemInfo->aSmartcardInfoArray[0].pSerialNumber, NA_UI_SC_NUMBER_STRING_LEN - 1);
				return ERR_OK;
			}
			HxLOG_Error("eCasNaSCInfo_iccSerial_Number[NULL]\n");
		}
	}

	return ERR_FAIL;
}

HERROR	xmgr_cas_NaGetCAVersion(HUINT8* pszStr)
{
	XmgrCas_NaUiSystemInfoData_t	*pstNaUiSystemInfo = NULL;
	eHxCP_CodePage_e	encType;
	HUINT8			szBuffer[NA_UI_CA_VERSION_STRING_LEN] = {0, };
	HINT32  		nSvcNameLength;
	HINT32			nUsedBytes;
#if 0		// not used
	HINT32			nUsedUTF8Bytes;
#endif

	pstNaUiSystemInfo = _xmgr_cas_NaGetSystemInfoData();
	if (pstNaUiSystemInfo->pIrdInfo != NULL)
	{
		if (pstNaUiSystemInfo->pIrdInfo->pVersion != NULL)
		{
			snprintf(szBuffer, NA_UI_CA_VERSION_STRING_LEN, "%s %s",
								pstNaUiSystemInfo->pIrdInfo->pVersion,
								pstNaUiSystemInfo->pIrdInfo->pRevision);


			encType = MWC_UTIL_GetEncodingByLocaleCode(szBuffer);
			nSvcNameLength = SvcCas_UtilStrLen(szBuffer);

			nUsedBytes = HLIB_CP_UTIL_ConvertCodepageToUTF8(encType, szBuffer, pszStr, NA_UI_CA_VERSION_STRING_LEN);

			HxLOG_Print("eCasNaSCInfo_caVersion[%s][%s] : nSvcNameLength(%d), nUsedBytes(%d)\n",
								pstNaUiSystemInfo->pIrdInfo->pVersion,
								pstNaUiSystemInfo->pIrdInfo->pRevision,
								nSvcNameLength,
								nUsedBytes);

			return ERR_OK;
		}
	}

	return ERR_FAIL;
}


HERROR	xmgr_cas_NaGetSmartcardVersion(HUINT8* pszStr, HINT32 nLength)
{
	XmgrCas_NaUiSystemInfoData_t	*pstNaUiSystemInfo = NULL;

	pstNaUiSystemInfo = _xmgr_cas_NaGetSystemInfoData();
	if ( pstNaUiSystemInfo->nNumberOfSmartcards > 0 )
	{
		if (pstNaUiSystemInfo->aSmartcardInfoArray != NULL )
		{
			HxLOG_Print("eCasNaSCInfo_iccVersion[%s][%s]\n",
							pstNaUiSystemInfo->aSmartcardInfoArray[0].pVersion,
							pstNaUiSystemInfo->aSmartcardInfoArray[0].pRevision);
#if defined(CONFIG_MW_CAS_NAGRA_OP_MEDIASET)
			snprintf(pszStr, nLength, "%s",
							pstNaUiSystemInfo->aSmartcardInfoArray[0].pVersion);
#else
			snprintf(pszStr, nLength, "%s %s",
							pstNaUiSystemInfo->aSmartcardInfoArray[0].pVersion,
							pstNaUiSystemInfo->aSmartcardInfoArray[0].pRevision);
#endif
			return ERR_OK;
		}
	}
	return ERR_FAIL;
}


#if 1
HERROR	xmgr_cas_NaGetSysNUID(HUINT8* pszStr, HINT32 nLength)
{
	HUINT32 ulNuid = 0;
	HERROR	ulRet = ERR_FAIL;

	if (pszStr == NULL)
	{
		return ERR_FAIL;
	}

	ulRet = SVC_SYS_GetChipId(&ulNuid);
	if ( ulRet == ERR_OK )
	{
		snprintf(pszStr, nLength, "%02X%02X %02X%02X", ((ulNuid>>24) & 0xFF), ((ulNuid>>16) & 0xFF), ((ulNuid>>8) & 0xFF), (ulNuid & 0xFF));
//		sprintf(pszStr, "%02X%02X %02X%02X", ((ulNuid>>24) & 0xFF), ((ulNuid>>16) & 0xFF), ((ulNuid>>8) & 0xFF), (ulNuid & 0xFF));
		return ERR_OK;
	}

	return ERR_FAIL;
}
#endif

HERROR	xmgr_cas_NaGetSystemScInfo(TSystemInfoStatus *xStatus, TIrdInfo **pxIrdInfo,
 											TUnsignedInt8 *xNumberOfSmartcards, TSmartcardInfoItem **pxSmartcardInfoArray)
{
	SvcCas_CtrlParam_t			stParam;
	NA_GetSystemInfoOutParam_t	stOutParam;

	stParam.ulControlType	= eNaCtrl_GetSystemInfo;
	stParam.pvIn			= NULL;
	stParam.pvOut			= (void *)&stOutParam;

	if(SVC_CAS_CtrlGet(eDxCAS_GROUPID_NA, eNaCtrl_GetSystemInfo, &stParam) != ERR_OK)
	{
		HxLOG_Error("SVC_CAS_CtrlGet(eNaCtrl_GetSystemInfo)error\n");
		return ERR_FAIL;
	}

	*xStatus = stOutParam.xStatus;
	*pxIrdInfo = stOutParam.pxIrdInfo;
	*xNumberOfSmartcards = stOutParam.xNumberOfSmartcards;
	*pxSmartcardInfoArray = stOutParam.pxSmartcardInfoArray;

	return ERR_OK;
}

HERROR	xmgr_cas_NaGetChipsetInfo(NaChipsetInfo_t **pstChipInfo)
{
	SvcCas_CtrlParam_t			stParam;
	NA_GetSystemInfoOutParam_t	stOutParam;

	stParam.ulControlType	= eNaCtrl_GetChipsetInfo;
	stParam.pvIn			= NULL;
	stParam.pvOut			= (void *)&stOutParam;

	if(SVC_CAS_CtrlGet(eDxCAS_GROUPID_NA, eNaCtrl_GetChipsetInfo, &stParam) != ERR_OK)
	{
		HxLOG_Error("SVC_CAS_CtrlGet(eNaCtrl_GetChipsetInfo) error\n");
		return ERR_FAIL;
	}

	*pstChipInfo = stOutParam.pstChipInfo;

	return ERR_OK;
}

HUINT8 xmgr_cas_NaGetSCRemoved(void )
{
	XmgrCas_NaAlamType_e eScAlarm;

	if (xmgr_cas_NaGetSmartCardStatus(&eScAlarm) == ERR_OK)
	{
		if (eScAlarm == eCAS_NA_CA_SMARTCARD_REMOVED)
		{
			return TRUE;
		}
	}

	return FALSE;
}

#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
HERROR xmgr_cas_NaGetExpirationDate(HUINT32 *pUnixTime)
{
	Handle_t 		hAction;
	TUnsignedInt32		convertZipCode;
	UNIXTIME		productUnixTime = 0;
	TCaDuration 		expireDuraion = 0;
	UNIXTIME		currentUnixTime = 0;
	HxDATETIME_t		stCurrentDateTime;
	HxDATETIME_t		stSCExpirationDateTime;
#if !(CONFIG_MW_CAS_NUM_SC_SLOT > 1)
	HERROR			hErr = ERR_OK;
#endif
	TBoolean   eStatus;

	hAction = SVC_PIPE_GetActionHandle(MGR_ACTION_GetMainActionId());

	VK_memset(&stSCExpirationDateTime, 0, sizeof(HxDATETIME_t));
	hErr = xmgr_cas_NaIsHdPlusService(hAction, &eStatus);
	if(hErr == ERR_OK && eStatus == TRUE)
	{
		xmgr_cas_NaGetZipCode (0, &convertZipCode);
		HxLOG_Print("convertZipCode (%d)\n" , (int)convertZipCode);
		if ((convertZipCode >= 1) && (convertZipCode <= 10))
		{
			// HD+ SC is activated !!!!
			hErr = VK_CLOCK_GetTime((unsigned long*)&currentUnixTime);
			HLIB_DATETIME_ConvertUnixTimeToDateTime(currentUnixTime, &stCurrentDateTime);

			if(hErr == ERR_OK)
			{		
				hErr = xmgr_cas_NaGetPdExpirationDuration(&expireDuraion);
				HxLOG_Print("expireDuraion (%d)\n" , (int)expireDuraion);

				if(hErr == ERR_OK)
				{
					HxLOG_Print("currentUnixTime (%d)\n" , currentUnixTime);
					productUnixTime = currentUnixTime + expireDuraion;
					*pUnixTime = productUnixTime;
					HLIB_DATETIME_ConvertUnixTimeToDateTime(productUnixTime, &stSCExpirationDateTime);
					HxLOG_Print("productUnixTime (%ld)\n" , (long int)productUnixTime);
				}
				else
				{

				}
			}
			else
			{

			}
		}
		else
		{

		}
	}
	else
	{
		xmgr_cas_NaGetZipCode (0, &convertZipCode);

		HxLOG_Print("convertZipCode (%d)\n" , (int)convertZipCode);
		if ((convertZipCode >= 1) && (convertZipCode <= 10))
		{
			// HD+ SC is activated !!!!
			// Info nur auf HD+ Kanal verfugbar
		}
		else
		{
			// HD+ Karte noch nicht aktiviert
		}
	}

	return ERR_OK;
}
#endif


#if defined(CONFIG_MW_CAS_NAGRA_CA_PRODUCT_ICME) || defined(CONFIG_MW_CAS_NAGRA_CA_PRODUCT_ME) || defined(CONFIG_MW_CAS_NAGRA_CA_PRODUCT_ALME)
HERROR	xmgr_cas_NaGetCscMaxIndex(TUnsignedInt8 *pxCscMaxIndex)
{
	SvcCas_CtrlParam_t			stParam;
	TUnsignedInt8				xCscMaxIndex;

	stParam.ulControlType	= eNaCtrl_GetCscMaxIndex;
	stParam.pvIn			= NULL;
	stParam.pvOut			= (void *)&xCscMaxIndex;

	if(SVC_CAS_CtrlGet(eDxCAS_GROUPID_NA, eNaCtrl_GetCscMaxIndex, &stParam) != ERR_OK)
	{
		HxLOG_Error("SVC_CAS_CtrlGet(eNaCtrl_GetCscMaxIndex) error\n");
		return ERR_FAIL;
	}

	*pxCscMaxIndex = xCscMaxIndex;

	return ERR_OK;
}
#endif

#if defined(CONFIG_MW_CAS_NAGRA_CA_PRODUCT_ICME)
HUINT8 xmgr_cas_NaUtil_GetNumberOfIcc(void)
{
	SvcCas_CtrlParam_t		stParam;
	TUnsignedInt8			xNumberOfIcc = 0;

	stParam.ulControlType	= eNaCtrl_GetNumberOfIcc;
	stParam.pvIn			= NULL;
	stParam.pvOut			= (void *)&xNumberOfIcc;

	if(SVC_CAS_CtrlGet(eDxCAS_GROUPID_NA, stParam.ulControlType, &stParam) != ERR_OK)
	{
		HxLOG_Error("SVC_CAS_CtrlGet(eNaCtrl_GetNumberOfIcc) error\n");
	}

	return xNumberOfIcc;
}
#endif

#if defined(CONFIG_MW_CAS_NAGRA_PRM)
HERROR	xmgr_cas_NaGetDvlVersion(TDvlVersionString **ppxDvlVersion)
{
	SvcCas_CtrlParam_t			stParam;
	NA_GetSystemInfoOutParam_t	stOutParam;

	stParam.ulControlType	= eNaCtrl_GetDvlVersion;
	stParam.pvIn			= NULL;
	stParam.pvOut			= (void *)&stOutParam;

	if(SVC_CAS_CtrlGet(eDxCAS_GROUPID_NA, eNaCtrl_GetDvlVersion, &stParam) != ERR_OK)
	{
		HxLOG_Error("SVC_CAS_CtrlGet(eNaCtrl_GetDvlVersion) error\n");
		return ERR_FAIL;
	}

	*ppxDvlVersion = stOutParam.pxDvlVersion;

	return ERR_OK;
}

HERROR	xmgr_cas_NaGetDvlChipsetType(TDvlChipsetTypeString **ppxDvlChipsetType)
{
	SvcCas_CtrlParam_t			stParam;
	NA_GetSystemInfoOutParam_t	stOutParam;

	stParam.ulControlType	= eNaCtrl_GetDvlChipsetType;
	stParam.pvIn			= NULL;
	stParam.pvOut			= (void *)&stOutParam;

	if(SVC_CAS_CtrlGet(eDxCAS_GROUPID_NA, eNaCtrl_GetDvlChipsetType, &stParam) != ERR_OK)
	{
		HxLOG_Error("SVC_CAS_CtrlGet(eNaCtrl_GetDvlChipsetType) error\n");
		return ERR_FAIL;
	}

	*ppxDvlChipsetType = stOutParam.pxDvlChipsetType;

	return ERR_OK;
}
#endif

HERROR	xmgr_cas_NaUpdateSystemInfo(void)
{
	XmgrCas_NaUiSystemInfoData_t	*pstNaUiSystemInfo = NULL;

	pstNaUiSystemInfo = _xmgr_cas_NaGetSystemInfoData();

	if(xmgr_cas_NaGetSystemScInfo(&pstNaUiSystemInfo->status, &pstNaUiSystemInfo->pIrdInfo, &pstNaUiSystemInfo->nNumberOfSmartcards, &pstNaUiSystemInfo->aSmartcardInfoArray) != ERR_OK)
	{
		HxLOG_Error("xmgr_cas_NaGetSystemScInfo() error\n");
	}

	if(xmgr_cas_NaGetChipsetInfo(&pstNaUiSystemInfo->pstChipInfo) != ERR_OK)
	{
		HxLOG_Error("xmgr_cas_NaGetChipsetInfo() error\n");
	}

#if defined(CONFIG_MW_CAS_NAGRA_CA_PRODUCT_ICME) || defined(CONFIG_MW_CAS_NAGRA_CA_PRODUCT_ME) || defined(CONFIG_MW_CAS_NAGRA_CA_PRODUCT_ALME)
	if(xmgr_cas_NaGetCscMaxIndex(&pstNaUiSystemInfo->nCscMaxIndex) != ERR_OK)
	{
		HxLOG_Error("xmgr_cas_NaGetCscMaxIndex() error\n");
	}
#endif

#if defined(CONFIG_MW_CAS_NAGRA_PRM)
	if(xmgr_cas_NaGetDvlVersion(&pstNaUiSystemInfo->pxDvlVersion) != ERR_OK)
	{
		HxLOG_Error("xmgr_cas_NaGetDvlVersion() error\n");
	}

	if(xmgr_cas_NaGetDvlChipsetType(&pstNaUiSystemInfo->pxDvlChipsetType) != ERR_OK)
	{
		HxLOG_Error("xmgr_cas_NaGetDvlChipsetType() error\n");
	}
#endif

	return ERR_OK;
}

HERROR	xmgr_cas_NaGetCurrentSCInfo(HUINT8* pszData, HINT32 nLength)
{
	HERROR				hErr;
	HUINT8*				pszSmartcardMsg;
	XmgrCas_NaAlamType_e	eAlarmType;

	hErr = xmgr_cas_NaGetSmartCardStatus(&eAlarmType);
	if (hErr == ERR_OK)
	{
		if (eAlarmType == eCAS_NA_CA_SMARTCARD_OK)
		{
			pszData[0] = '\0';
			return hErr;
		}

		pszSmartcardMsg = xmgr_cas_NaGetMessage(eAlarmType);
		if (pszSmartcardMsg != NULL)
		{
			MWC_UTIL_DvbStrncpy(pszData, pszSmartcardMsg, nLength - 1);
#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
			HLIB_STD_MemFree(pszSmartcardMsg);
#endif
		}
	}

	return hErr;
}

HUINT8	xmgr_cas_NaCheckCardErrorOnMenu(void)
{
	XmgrCas_NaAlamType_e 		eScAlarm;
	HERROR				hErrResult;

#if (CONFIG_MW_CAS_NUM_SC_SLOT > 1)
	HUINT32 i = 0;
	for (i = 0; i < CAS_MAX_NUM_OF_SC; i ++)
	{
		xmgr_cas_NaGetSmartCardStatusBySlotID(i, &eScAlarm);
		if ((eScAlarm == eCAS_NA_CA_SMARTCARD_OK) || (eScAlarm == eCAS_NA_CA_SMARTCARD_EXPIRED))
		{
			return FALSE;
		}
	}

	return TRUE;
#else
	hErrResult = xmgr_cas_NaGetSmartCardStatus(&eScAlarm);
	// xmgr_cas_NaGetSmartCardStatus에서 ERR_FAIL로 return되는 경우에는 무조건 eScAlarm == eCAS_NA_CA_SMARTCARD_REMOVED 로 만들어 준다.
	// 그래서 hErrResult == ERR_FAIL && eScAlarm == eCAS_NA_CA_SMARTCARD_REMOVED 인경우에 아래 CODE에 TRUE를 return해줌으로 여기서 TRUE로 return시킴
	// 그리고 hErrResult == ERR_FAIL이고 eScAlarm == eCAS_NA_CA_SMARTCARD_REMOVED가 아닌경우는 없으므로 FALSE로 return 시켜줌.
	if(hErrResult == ERR_FAIL)
	{
		if(eScAlarm == eCAS_NA_CA_SMARTCARD_REMOVED)
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}

#if defined(CONFIG_MW_CAS_NAGRA_OP_MEDIASET) || defined(CONFIG_MW_CAS_NAGRA_OP_TIVU)
	if ((eScAlarm != eCAS_NA_CA_SMARTCARD_OK) && (eScAlarm != eCAS_NA_CA_SMARTCARD_EXPIRED))
#else
	if ((eScAlarm == eCAS_NA_CA_SMARTCARD_REMOVED) || (eScAlarm == eCAS_NA_CA_SMARTCARD_COM_ERROR) ||
		(eScAlarm == eCAS_NA_CA_SMARTCARD_MUTE) || (eScAlarm == eCAS_NA_CA_SMARTCARD_INVALID) ||
		(eScAlarm == eCAS_NA_CA_SMARTCARD_NOT_CERTIFIED))
#endif
	{
		return TRUE;
	}

	return FALSE;
#endif
}

#if (CONFIG_MW_CAS_NUM_SC_SLOT > 1)
HERROR	xmgr_cas_NaGetSlotIDbySessionID(TIccSessionId iccSessionId, TUnsignedInt32 *pulSlotId)
{
	HERROR								hErr;
	SvcCas_CtrlParam_t					stParam;
	NA_GetSlotIDbySessionIDInParam_t	stInParam;
	NA_GetSlotIDbySessionIDOutParam_t	stOutParam;

	stParam.ulControlType = eNaCtrl_GetSlotIDbySessionID;
	stInParam.iccSessionId = iccSessionId;
	stParam.pvIn = (void *)&stInParam;
	stParam.pvOut = (void *)&stOutParam;
	hErr = SVC_CAS_CtrlGet(eDxCAS_GROUPID_NA, eNaCtrl_GetSlotIDbySessionID, &stParam);
	*pulSlotId = stOutParam.ulSlotId;

	return hErr;
	//hErr = MW_CA_NA_GetSlotIDbySessionID(iccSessionId, pulSlotId);
}
#endif

HERROR	xmgr_cas_NaGetSystemInfo(XmgrCas_NaSystemInfoItem_e eItem, HUINT8* pszData, HINT32 nLength, HBOOL bCheckCardError)
{
	XmgrCas_NaUiSystemInfoData_t	*pstNaUiSystemInfo = NULL;
#if (CONFIG_MW_CAS_NUM_SC_SLOT > 1)
	TUnsignedInt32	i = 0, ulTempSlotId = 0;
	HERROR			hErr = ERR_OK;
#endif

	if (pszData == NULL)
	{
		HxLOG_Error("pszData is NULL\n");
		return ERR_FAIL;
	}

	if(bCheckCardError == TRUE)
	{
		if (xmgr_cas_NaCheckCardErrorOnMenu())
		{
			if (eItem != eNaSysInfoItem_Smartcard_Status)
			{
				HxSTD_memset(pszData, 0, nLength);
				return ERR_OK;
			}
		}
	}

	pstNaUiSystemInfo = _xmgr_cas_NaGetSystemInfoData();

	switch (eItem)
	{
		case eNaSysInfoItem_Project_Name :
			if (pstNaUiSystemInfo->pIrdInfo != NULL && pstNaUiSystemInfo->pIrdInfo->pProjectName != NULL)
			{
				HxLOG_Print("eNaSysInfoItem_Project_Name[%s]\n", pstNaUiSystemInfo->pIrdInfo->pProjectName);
				MWC_UTIL_DvbStrncpy(pszData, pstNaUiSystemInfo->pIrdInfo->pProjectName, nLength - 1);
			}
			break;

		case eNaSysInfoItem_iccVersion :
			if ( pstNaUiSystemInfo->nNumberOfSmartcards > 0 && pstNaUiSystemInfo->aSmartcardInfoArray != NULL )
			{
#if (CONFIG_MW_CAS_NUM_SC_SLOT > 1)
				for (i = 0; i < CAS_MAX_NUM_OF_SC; i ++)
				{
					HxLOG_Print("iccSessionId[%d], eNaSysInfoItem_iccSerial_Number[%s], provider[0x%x]\n", pstNaUiSystemInfo->aSmartcardInfoArray[i].iccSessionId, pstNaUiSystemInfo->aSmartcardInfoArray[i].pSerialNumber, pstNaUiSystemInfo->aSmartcardInfoArray[i].provider);

					hErr = xmgr_cas_NaGetSlotIDbySessionID(pstNaUiSystemInfo->aSmartcardInfoArray[i].iccSessionId, &ulTempSlotId);
					if (hErr == ERR_OK && ulTempSlotId == 0)
					{
						snprintf(pszData, nLength, "%s %s",
									pstNaUiSystemInfo->aSmartcardInfoArray[i].pVersion,
									pstNaUiSystemInfo->aSmartcardInfoArray[i].pRevision);
/*						sprintf(pszData, "%s %s",
									pstNaUiSystemInfo->aSmartcardInfoArray[i].pVersion,
									pstNaUiSystemInfo->aSmartcardInfoArray[i].pRevision);*/
						break;
					}
				}
#else
				if(pstNaUiSystemInfo->aSmartcardInfoArray[0].pVersion)
				{
					if(pstNaUiSystemInfo->aSmartcardInfoArray[0].pRevision)
					{
						HxLOG_Print("eNaSysInfoItem_iccVersion[%s][%s]\n", pstNaUiSystemInfo->aSmartcardInfoArray[0].pVersion, pstNaUiSystemInfo->aSmartcardInfoArray[0].pRevision);
						snprintf(pszData, nLength, "%s %s", pstNaUiSystemInfo->aSmartcardInfoArray[0].pVersion, pstNaUiSystemInfo->aSmartcardInfoArray[0].pRevision);
					//	sprintf(pszData, "%s %s", pstNaUiSystemInfo->aSmartcardInfoArray[0].pVersion, pstNaUiSystemInfo->aSmartcardInfoArray[0].pRevision);
					}
					else
					{
						HxLOG_Print("eNaSysInfoItem_iccVersion[%s]\n", pstNaUiSystemInfo->aSmartcardInfoArray[0].pVersion);
						MWC_UTIL_DvbStrncpy(pszData, pstNaUiSystemInfo->aSmartcardInfoArray[0].pVersion, nLength - 1);
					}
				}
#endif
			}
			break;

		case eNaSysInfoItem_caVersion :
			if (pstNaUiSystemInfo->pIrdInfo != NULL && pstNaUiSystemInfo->pIrdInfo->pVersion != NULL)
			{
				if(pstNaUiSystemInfo->pIrdInfo->pRevision)
				{
					HxLOG_Print("eNaSysInfoItem_caVersion[%s][%s]\n", pstNaUiSystemInfo->pIrdInfo->pVersion, pstNaUiSystemInfo->pIrdInfo->pRevision);
					snprintf(pszData, nLength, "%s %s", pstNaUiSystemInfo->pIrdInfo->pVersion, pstNaUiSystemInfo->pIrdInfo->pRevision);
//					sprintf(pszData, "%s %s", pstNaUiSystemInfo->pIrdInfo->pVersion, pstNaUiSystemInfo->pIrdInfo->pRevision);
				}
				else
				{
					HxLOG_Print("eNaSysInfoItem_caVersion[%s]\n", pstNaUiSystemInfo->pIrdInfo->pVersion);
					MWC_UTIL_DvbStrncpy(pszData, pstNaUiSystemInfo->pIrdInfo->pVersion, nLength - 1);
				}
			}
			break;

		case eNaSysInfoItem_iccSerial_Number :
			if ( pstNaUiSystemInfo->nNumberOfSmartcards > 0 && pstNaUiSystemInfo->aSmartcardInfoArray != NULL )
			{
#if (CONFIG_MW_CAS_NUM_SC_SLOT > 1)
				for (i = 0; i < CAS_MAX_NUM_OF_SC; i ++)
				{
					HxLOG_Print("iccSessionId[%d], eNaSysInfoItem_iccSerial_Number[%s], provider[0x%x]\n",
									pstNaUiSystemInfo->aSmartcardInfoArray[i].iccSessionId, pstNaUiSystemInfo->aSmartcardInfoArray[i].pSerialNumber, pstNaUiSystemInfo->aSmartcardInfoArray[i].provider);

					hErr = xmgr_cas_NaGetSlotIDbySessionID(pstNaUiSystemInfo->aSmartcardInfoArray[i].iccSessionId, &ulTempSlotId);
					if (hErr == ERR_OK && ulTempSlotId == 0)
					{
						HxLOG_Print("eNaSysInfoItem_iccSerial_Number[%s]\n",
										pstNaUiSystemInfo->aSmartcardInfoArray[i].pSerialNumber);
						MWC_UTIL_DvbStrncpy(pszData, pstNaUiSystemInfo->aSmartcardInfoArray[i].pSerialNumber, nLength - 1);
						break;
					}
				}
#else
				if(pstNaUiSystemInfo->aSmartcardInfoArray[0].pSerialNumber)
				{
					HxLOG_Print("eNaSysInfoItem_iccSerial_Number[%s]\n", pstNaUiSystemInfo->aSmartcardInfoArray[0].pSerialNumber);
					MWC_UTIL_DvbStrncpy(pszData, pstNaUiSystemInfo->aSmartcardInfoArray[0].pSerialNumber, nLength - 1);
				}
#endif
			}
			break;

		case eNaSysInfoItem_CA_Serial_Number :
			if (pstNaUiSystemInfo->pIrdInfo != NULL && pstNaUiSystemInfo->pIrdInfo->pSerialNumber != NULL)
			{
				HxLOG_Print("eNaSysInfoItem_CA_Serial_Number[%s]\n", pstNaUiSystemInfo->pIrdInfo->pSerialNumber);
				MWC_UTIL_DvbStrncpy(pszData, pstNaUiSystemInfo->pIrdInfo->pSerialNumber, nLength - 1);
			}
			break;

		case eNaSysInfoItem_Chipset_Nuid :
			#if 0
			{
				HUINT32 uNuid = 0;
				SVC_SYS_GetChipId(&uNuid);
				sprintf(pszData, "%02X%02X %02X%02X", ((uNuid>>24) & 0xFF), ((uNuid>>16) & 0xFF), ((uNuid>>8) & 0xFF), (uNuid & 0xFF));
			}
			#else
			if(pstNaUiSystemInfo->pstChipInfo != NULL && pstNaUiSystemInfo->pstChipInfo->pcNuid != NULL)
			{
				HxLOG_Print("eNaSysInfoItem_Chipset_Nuid[%s]\n", pstNaUiSystemInfo->pstChipInfo->pcNuid);
				MWC_UTIL_DvbStrncpy(pszData, pstNaUiSystemInfo->pstChipInfo->pcNuid, nLength - 1);
			}
			#endif
			break;

		case eNaSysInfoItem_Chipset_Type :
			if(pstNaUiSystemInfo->pstChipInfo && pstNaUiSystemInfo->pstChipInfo->pcChipsetType)
			{
				HxLOG_Print("eNaSysInfoItem_Chipset_Type[%s]\n", pstNaUiSystemInfo->pstChipInfo->pcChipsetType);
				MWC_UTIL_DvbStrncpy(pszData, pstNaUiSystemInfo->pstChipInfo->pcChipsetType, nLength - 1);
			}
			break;

		case eNaSysInfoItem_Chipset_Rev :
			if(pstNaUiSystemInfo->pstChipInfo && pstNaUiSystemInfo->pstChipInfo->pcChipsetRev)
			{
				HxLOG_Print("eNaSysInfoItem_Chipset_Rev[%s]\n", pstNaUiSystemInfo->pstChipInfo->pcChipsetRev);
				MWC_UTIL_DvbStrncpy(pszData, pstNaUiSystemInfo->pstChipInfo->pcChipsetRev, nLength - 1);
			}
			break;

#if defined(CONFIG_MW_CAS_NAGRA_CA_PRODUCT_ICME) || defined(CONFIG_MW_CAS_NAGRA_CA_PRODUCT_ME) || defined(CONFIG_MW_CAS_NAGRA_CA_PRODUCT_ALME)
		case eNaSysInfoItem_CSC_Index :
			if(pstNaUiSystemInfo->nCscMaxIndex == 0)
				snprintf(pszData, nLength, "%s", "-");
			else
				snprintf(pszData, nLength, "%d", pstNaUiSystemInfo->nCscMaxIndex);
//			sprintf(pszData, "%d", pstNaUiSystemInfo->nCscMaxIndex);
			break;
#endif

#if (CONFIG_MW_CAS_NUM_SC_SLOT > 1)
		case eNaSysInfoItem_iccVersion_2nd :
			if ( pstNaUiSystemInfo->nNumberOfSmartcards > 0 && pstNaUiSystemInfo->aSmartcardInfoArray != NULL )
			{
				for (i = 0; i < CAS_MAX_NUM_OF_SC; i ++)
				{
					HxLOG_Print("iccSessionId[%d], eNaSysInfoItem_iccVersion_2nd[%s], provider[0x%x]\n",
									pstNaUiSystemInfo->aSmartcardInfoArray[i].iccSessionId, pstNaUiSystemInfo->aSmartcardInfoArray[i].pSerialNumber, pstNaUiSystemInfo->aSmartcardInfoArray[i].provider);

					hErr = xmgr_cas_NaGetSlotIDbySessionID(pstNaUiSystemInfo->aSmartcardInfoArray[i].iccSessionId, &ulTempSlotId);
					if (hErr == ERR_OK && ulTempSlotId == 1)
					{
						snprintf(pszData, nLength, "%s %s",
										pstNaUiSystemInfo->aSmartcardInfoArray[i].pVersion,
										pstNaUiSystemInfo->aSmartcardInfoArray[i].pRevision);
	/*					sprintf(pszData, "%s %s",
										pstNaUiSystemInfo->aSmartcardInfoArray[i].pVersion,
										pstNaUiSystemInfo->aSmartcardInfoArray[i].pRevision);*/
						break;
					}
				}
			}
			break;

		case eNaSysInfoItem_iccSerial_Number_2nd :
			if ( pstNaUiSystemInfo->nNumberOfSmartcards > 0 && pstNaUiSystemInfo->aSmartcardInfoArray != NULL )
			{
				for (i = 0; i < CAS_MAX_NUM_OF_SC; i ++)
				{
					HxLOG_Print("iccSessionId[%d], eNaSysInfoItem_iccSerial_Number_2nd[%s], provider[0x%x]\n",
									pstNaUiSystemInfo->aSmartcardInfoArray[i].iccSessionId, pstNaUiSystemInfo->aSmartcardInfoArray[i].pSerialNumber, pstNaUiSystemInfo->aSmartcardInfoArray[i].provider);

					hErr = xmgr_cas_NaGetSlotIDbySessionID(pstNaUiSystemInfo->aSmartcardInfoArray[i].iccSessionId, &ulTempSlotId);
					if (hErr == ERR_OK && ulTempSlotId == 1)
					{
						MWC_UTIL_DvbStrncpy(pszData, pstNaUiSystemInfo->aSmartcardInfoArray[i].pSerialNumber, nLength - 1);
						break;
					}
				}
			}
			break;
#endif

#if defined(CONFIG_MW_CAS_NAGRA_PRM)
		case eNaSysInfoItem_DVL_Version:
			if(pstNaUiSystemInfo->pxDvlVersion != NULL)
			{
				HxLOG_Print("eNaSysInfoItem_DVL_Version[%s]\n", pstNaUiSystemInfo->pxDvlVersion);
				MWC_UTIL_DvbStrncpy(pszData, (const HUINT8 *)pstNaUiSystemInfo->pxDvlVersion, nLength - 1);
			}
			break;
#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
		case eNaSysInfoItem_DVL_ChipsetType:
			if(pstNaUiSystemInfo->pxDvlChipsetType!= NULL)
			{
				HxLOG_Print("eNaSysInfoItem_DVL_ChipsetType[%s]\n", pstNaUiSystemInfo->pxDvlChipsetType);
				MWC_UTIL_DvbStrncpy(pszData, (const HUINT8 *)pstNaUiSystemInfo->pxDvlChipsetType, nLength - 1);
			}
			break;
#endif
#endif
		case eNaSysInfoItem_Smartcard_Status :
#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
			{
				Handle_t 		hAction;
				TUnsignedInt32		convertZipCode;
				UNIXTIME		productUnixTime = 0;
				TCaDuration 		expireDuraion = 0;
				UNIXTIME		currentUnixTime = 0;
				HxDATETIME_t		stCurrentDateTime;
				HxDATETIME_t		stSCExpirationDateTime;
#if !(CONFIG_MW_CAS_NUM_SC_SLOT > 1)
				HERROR			hErr = ERR_OK;
#endif
				TBoolean   eStatus;

				hAction = SVC_PIPE_GetActionHandle(MGR_ACTION_GetMainActionId());

				VK_memset(pszData, 0, nLength);
				VK_memset(&stSCExpirationDateTime, 0, sizeof(HxDATETIME_t));
				hErr = xmgr_cas_NaIsHdPlusService(hAction, &eStatus);
				if(hErr == ERR_OK && eStatus == TRUE)
				{
					xmgr_cas_NaGetZipCode (0, &convertZipCode);
					HxLOG_Print("convertZipCode (%d)\n" , (int)convertZipCode);
					if ((convertZipCode >= 1) && (convertZipCode <= 10))
					{
						// HD+ SC is activated !!!!

						hErr = VK_CLOCK_GetTime((unsigned long*)&currentUnixTime);
						HLIB_DATETIME_ConvertUnixTimeToDateTime(currentUnixTime, &stCurrentDateTime);

						if(hErr == ERR_OK)
						{
							hErr = xmgr_cas_NaGetPdExpirationDuration(&expireDuraion);
							HxLOG_Print("expireDuraion (%d)\n" , (int)expireDuraion);

							if(hErr == ERR_OK)
							{
								HxLOG_Print("currentUnixTime (%d)\n" , currentUnixTime);
								productUnixTime = currentUnixTime + expireDuraion;
								HLIB_DATETIME_ConvertUnixTimeToDateTime(productUnixTime, &stSCExpirationDateTime);
								HxLOG_Print("productUnixTime (%ld)\n" , (long int)productUnixTime);

								if(currentUnixTime < productUnixTime)
								{
									// valid
									// Expiration date in format "<dd.mm.yyyy>"
									// Note: Date in German format
									// e.g. "02.03.2012"
									if ((stSCExpirationDateTime.stDate.usYear == 0) || (stSCExpirationDateTime.stDate.ucMonth == 0) || (stSCExpirationDateTime.stDate.ucDay == 0))
									{
										MWC_UTIL_DvbUtf8Sprintf(pszData, "--.--.----");
									}
									else
									{
											MWC_UTIL_DvbUtf8Sprintf(pszData, "%02d.%02d.%04d", stSCExpirationDateTime.stDate.ucDay, stSCExpirationDateTime.stDate.ucMonth, stSCExpirationDateTime.stDate.usYear);
											HxLOG_Print("stSCExpirationDateTime (%s)\n" , (int)pszData);
									}
								}
								else
								{
									// expired
									// abgelaufen
									MWC_UTIL_DvbStrncpy(pszData, (HUINT8 *)"LOC_CAS_NA_SMARTCARD_STATUS_ITEM_4_ID", nLength -1);
									HxLOG_Print("LOC_CAS_NA_SMARTCARD_STATUS_ITEM_4_ID\n");
								}
							}
							else
							{
								MWC_UTIL_DvbUtf8Sprintf(pszData, "--.--.----");
								// DURATION을 불러오는데 실패하면 ???
							}
						}
						else
						{
							MWC_UTIL_DvbUtf8Sprintf(pszData, "--.--.----");
							// 시간을 불러오는데 실패하면 ???
						}
					}
					else
					{
						// HD+ Karte wird aktiviert
						MWC_UTIL_DvbStrncpy(pszData, (HUINT8 *)"LOC_CAS_NA_SMARTCARD_STATUS_ITEM_3_ID", nLength - 1);
						HxLOG_Print("LOC_CAS_NA_SMARTCARD_STATUS_ITEM_3_ID\n");
					}
				}
				else
				{
					xmgr_cas_NaGetZipCode (0, &convertZipCode);

					HxLOG_Print("convertZipCode (%d)\n" , (int)convertZipCode);
					if ((convertZipCode >= 1) && (convertZipCode <= 10))
					{
						// HD+ SC is activated !!!!
						// Info nur auf HD+ Kanal verfugbar
						MWC_UTIL_DvbStrncpy(pszData, (HUINT8 *)"LOC_CAS_NA_SMARTCARD_STATUS_ITEM_2_ID", nLength - 1);
						HxLOG_Print("LOC_CAS_NA_SMARTCARD_STATUS_ITEM_2_ID\n");
					}
					else
					{
						// HD+ Karte noch nicht aktiviert
						MWC_UTIL_DvbStrncpy(pszData, (HUINT8 *)"LOC_CAS_NA_SMARTCARD_STATUS_ITEM_1_ID", nLength - 1);
						HxLOG_Print("LOC_CAS_NA_SMARTCARD_STATUS_ITEM_1_ID\n");
					}
				}
				// MWC_UTIL_DvbStrncpy(pszData, (HUINT8 *)GetStrRsc(nLocId), nLength - 1);
			}
			break;
#else
			xmgr_cas_NaGetCurrentSCInfo(pszData, nLength);
#endif
			break;

		default :
			return ERR_FAIL;

	}

	return ERR_OK;
}

TCaStatus	xmgr_cas_NaSystemInfoCallback(TSystemInfoStatus xStatus, TIrdInfo* pxIrdInfo, TUnsignedInt8 xNumberOfSmartcards, TSmartcardInfoItem* pxSmartcardInfoArray)
{
	HxLOG_Print("[%s:%d]...\n", __FUNCTION__, __HxLINE__);

	xmgr_cas_NaUpdateSystemInfo();
	BUS_PostMessage(NULL, eMEVT_CAS_SYSINFO_UPDATED, (Handle_t)NULL, eDxCAS_GROUPID_NA, 0, 0);

	return ERR_OK;
}

#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
HERROR	xmgr_cas_NaGetPdExpirationDuration(TCaDuration *expireDuraion)
{
	SvcCas_CtrlParam_t						stParam;
	NA_GetPdExpirationDurationOutParam_t	stOutParam;

	stParam.ulControlType	= eNaCtrl_GetPdExpirationDuration;
	stParam.pvIn			= NULL;
	stParam.pvOut			= (void *)&stOutParam;

	if(SVC_CAS_CtrlGet(eDxCAS_GROUPID_NA, eNaCtrl_GetPdExpirationDuration, &stParam) != ERR_OK)
	{
		HxLOG_Error("SVC_CAS_CtrlGet(eNaCtrl_GetPdExpirationDuration) error\n");
		return ERR_FAIL;
	}

	*expireDuraion = stOutParam.expireDuraion;

	return ERR_OK;
}

HERROR	xmgr_cas_NaGetZipCode(TUnsignedInt8 ucSlotId, TUnsignedInt32 *pulZipCode)
{
	SvcCas_CtrlParam_t				stParam;
	NA_GetZipCodeInParam_t 	stInParam;
	NA_GetZipCodeOutParam_t	stOutParam;

	stParam.ulControlType = eNaCtrl_GetZipCode;

	stInParam.ucSlotId = ucSlotId;
	stParam.pvIn = (void *)&stInParam;
	stParam.pvOut = (void *)&stOutParam;

	if(SVC_CAS_CtrlGet(eDxCAS_GROUPID_NA, eNaCtrl_GetZipCode, &stParam) != ERR_OK)
	{
		HxLOG_Error("SVC_CAS_CtrlGet(eNaCtrl_GetZipCode) error\n");
		return ERR_FAIL;
	}

	*pulZipCode = stOutParam.ulZipCode;

	return ERR_OK;
}

HERROR	xmgr_cas_NaIsHdPlusSmartCard(TUnsignedInt8 ucSlotId, TBoolean *pbHdPlusSc)
{
	SvcCas_CtrlParam_t				stParam;
	NA_IsHdPlusSmartCardInParam_t		stInParam;
	NA_IsHdPlusSmartCardOutParam_t		stOutParam;

	stParam.ulControlType = eNaCtrl_IsHdPlusSmartCard;

	stInParam.ucSlotNo = ucSlotId;
	stParam.pvIn = (void *)&stInParam;
	stParam.pvOut = (void *)&stOutParam;

	if(SVC_CAS_CtrlGet(eDxCAS_GROUPID_NA, eNaCtrl_IsHdPlusSmartCard, &stParam) != ERR_OK)
	{
		HxLOG_Error("SVC_CAS_CtrlGet(eNaCtrl_IsHdPlusSmartCard) error\n");
		return ERR_FAIL;
	}

	*pbHdPlusSc = stOutParam.bHdPlusSc;

	return ERR_OK;
}

HERROR	xmgr_cas_NaIsHdPlusService(TUnsignedInt32 hAction, TBoolean *peStatus)
{
	SvcCas_CtrlParam_t				stParam;
	NA_IsHdPlusServiceInParam_t 	stInParam;
	NA_IsHdPlusServiceOutParam_t	stOutParam;

	stParam.ulControlType = eNaCtrl_IsHdPlusService;

	stInParam.hAction = hAction;
	stParam.pvIn = (void *)&stInParam;
	stParam.pvOut = (void *)&stOutParam;

	if(SVC_CAS_CtrlGet(eDxCAS_GROUPID_NA, eNaCtrl_IsHdPlusService, &stParam) != ERR_OK)
	{
		HxLOG_Error("SVC_CAS_CtrlGet(eNaCtrl_IsHdPlusService) error\n");
		return ERR_FAIL;
	}

	*peStatus = stOutParam.bStatus;

	return ERR_OK;
}
#endif

#if defined (CONFIG_OP_SES)
HERROR	xmgr_cas_NaIsSameCasIdInSc(TUnsignedInt8 ucSlotId,TUnsignedInt16 usCaSysId, TBoolean *pbSame)
#else
HERROR	xmgr_cas_NaIsSameCasIdInSc(TUnsignedInt32 ulIndex, TBoolean *pbSame)
#endif
{
	SvcCas_CtrlParam_t				stParam;
	NA_IsSameCasIdInScInParam_t		stInParam;
	NA_IsSameCasIdInScOutParam_t		stOutParam;

	stParam.ulControlType = eNaCtrl_IsSameCasIdInSc;

#if defined (CONFIG_OP_SES)
	stInParam.ucSlotId = ucSlotId;
	stInParam.usCaSysId = usCaSysId;
#else
	stInParam.ucSlotId = ulIndex;
#endif
	stParam.pvIn = (void *)&stInParam;
	stParam.pvOut = (void *)&stOutParam;

	if(SVC_CAS_CtrlGet(eDxCAS_GROUPID_NA, eNaCtrl_IsSameCasIdInSc, &stParam) != ERR_OK)
	{
		HxLOG_Error("SVC_CAS_CtrlGet(eNaCtrl_GetCurrScStatus) error\n");
		return ERR_FAIL;
	}

	*pbSame = stOutParam.bSame;
	HxLOG_Print("*pbSame = stOutParam.bSame = %d \n", *pbSame);

	return ERR_OK;
}

HERROR	xmgr_cas_NaGetCurrScStatus(TUnsignedInt32 ulIndex, TSmartcardAlarm *peScAlarm)
{
	SvcCas_CtrlParam_t				stParam;
	NA_GetCurrScStatusInParam_t		stInParam;
	NA_GetCurrScStatusOutParam_t	stOutParam;

	stParam.ulControlType = eNaCtrl_GetCurrScStatus;

	stInParam.ulSlotId = ulIndex;
	stParam.pvIn = (void *)&stInParam;
	stParam.pvOut = (void *)&stOutParam;

	if(SVC_CAS_CtrlGet(eDxCAS_GROUPID_NA, eNaCtrl_GetCurrScStatus, &stParam) != ERR_OK)
	{
		HxLOG_Error("SVC_CAS_CtrlGet(eNaCtrl_GetCurrScStatus) error\n");
		return ERR_FAIL;
	}

	*peScAlarm = stOutParam.eScAlarm;

	return ERR_OK;
}

void	xmgr_cas_NaSmartcardInfoCallback(TUnsignedInt8 ucSlotId, TSmartcardState eScAlarm)
{
	HxLOG_Print("[%s:%d]...\n", __FUNCTION__, __HxLINE__);

	BUS_PostMessage(NULL, eMEVT_CAS_UPDATE_SMARTCARD_INFO, (Handle_t)NULL, eDxCAS_GROUPID_NA, 0, 0);

#if (CONFIG_MW_CAS_NUM_SC_SLOT > 1)
	if (eScAlarm == CA_SMARTCARD_STATE_INCOMPATIBLE)
	{
#if 0
		BUS_SendMessage (NULL, MSG_APPC2U_SAME_SMARTCARD_CONFLICT, (Handle_t)NULL, eDxCAS_GROUPID_NA, ucSlotId, eScAlarm);
#endif
	}
	else
	{
		HUINT32 i = 0;
		HUINT32 eScAlarmStatus = CA_SMARTCARD_STATE_INVALID;
		HERROR	hErr = ERR_OK;

		for (i = 0; i < CAS_MAX_NUM_OF_SC; i ++)
		{
			if (i == ucSlotId)
			{
				continue;
			}

			hErr = xmgr_cas_NaGetCurrScStatus(i, &eScAlarmStatus);
			HxLOG_Print("xmgr_cas_NaGetCurrScStatus = 0x%x, eScAlarmStatus = %d, eScAlarm = %d\n", hErr, eScAlarmStatus, eScAlarm);
			if (hErr == ERR_OK && eScAlarm == CA_SMARTCARD_STATE_INCOMPATIBLE)
			{
				HxLOG_Print("CA_SMARTCARD_STATE_INCOMPATIBLE still exist => PASS \n");
				return;
			}
		}

		if (i == CAS_MAX_NUM_OF_SC)
		{
			HxLOG_Print("CA_ALARM_SMARTCARD_INCOMPATIBLE doesn't exist => CLEAR \n");
#if 0
			BUS_SendMessage (NULL, MSG_APPC2U_SAME_SMARTCARD_FREE, (Handle_t)NULL, eDxCAS_GROUPID_NA, ucSlotId, eScAlarm);
#endif
		}
	}
#endif
}

#if (CONFIG_MW_CAS_NUM_SC_SLOT > 1)
HERROR	xmgr_cas_NaGetCardSerialNumberBySlotID(HUINT8 ucSlotId, HUINT8* paucCardSer)
{
	XmgrCas_NaUiSystemInfoData_t	*pstNaUiSystemInfo = NULL;
	TUnsignedInt32			i = 0, ulTempSlotId = 0;
	HERROR					hErr = ERR_OK;

	pstNaUiSystemInfo = _xmgr_cas_NaGetSystemInfoData();
	if ( pstNaUiSystemInfo->nNumberOfSmartcards > 0)
	{
		if (pstNaUiSystemInfo->aSmartcardInfoArray != NULL )
		{
			for (i = 0; i < CAS_MAX_NUM_OF_SC; i ++)
			{
				HxLOG_Print("iccSessionId[%d], eNaSysInfoItem_iccSerial_Number[%s], provider[0x%x]\n",
								pstNaUiSystemInfo->aSmartcardInfoArray[i].iccSessionId, pstNaUiSystemInfo->aSmartcardInfoArray[i].pSerialNumber, pstNaUiSystemInfo->aSmartcardInfoArray[i].provider));

				hErr = xmgr_cas_NaGetSlotIDbySessionID(pstNaUiSystemInfo->aSmartcardInfoArray[i].iccSessionId, &ulTempSlotId);
				if (hErr == ERR_OK && ulTempSlotId == ucSlotId)
				{
//					if (pstNaUiSystemInfo->aSmartcardInfoArray[i].provider == NA_TIVU_PROVIDER_ID || pstNaUiSystemInfo->aSmartcardInfoArray[i].provider = NA_TIVU_TEST_PROVIDER_ID)
					{
						MWC_UTIL_DvbStrncpy(paucCardSer, pstNaUiSystemInfo->aSmartcardInfoArray[i].pSerialNumber, NA_UI_SC_NUMBER_STRING_LEN - 1);
						return ERR_OK;
					}
				}
			}
		}
	}

	return ERR_FAIL;
}

#endif

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/
#define _____XMGR_CAS_NA_UTIL_PUBLIC_FUNCTION______________________________________________________

void	xmgr_cas_NaCMD(HUINT8 *pszCmd, HINT32 ulParam1, HINT32 ulParam2, HUINT8 *pszmsg)
{
#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
	if (pszCmd == NULL)
	{
		HxLOG_Error("pszCmd = NULL return\n");
		return;
	}
	if (pszmsg == NULL)
	{
		HxLOG_Error(" pszmsg = NULL return\n");
		return;
	}

	xmgr_cas_NaCmd_Aps(pszCmd, ulParam1, ulParam2, pszmsg);
#endif
#if defined(CONFIG_MW_CAS_NAGRA_OP_SKYD) || defined(CONFIG_MW_CAS_NAGRA_OP_KDG)
	// todo:
#endif
#if defined(CONFIG_MW_CAS_NAGRA_OP_MEDIASET)
	// todo:
	if (MWC_UTIL_DvbStrcmp(pszCmd, "ippv") == 0)
	{
		if(ulParam1 == 0) //mediaset
		{
			BUS_MGR_Create("xmgr_cas_NaPpvBanner_Mediaset",
							APP_CAS_PPV_PRIORITY,
							xmgr_cas_NaPpvBanner_Mediaset,
							SVC_PIPE_GetActionHandle(MGR_ACTION_GetMainActionId()),
							0,
							0,
							0);
		}
		else if(ulParam1 == 1) //pangea
		{
			BUS_MGR_Create("xproc_cas_NaPpvBanner_Pangea",
							APP_CAS_PPV_PRIORITY,
							xproc_cas_NaPpvBanner_Pangea,
							SVC_PIPE_GetActionHandle(MGR_ACTION_GetMainActionId()),
							0,
							0,
							0);
		}
	}
	else if (MWC_UTIL_DvbStrcmp(pszCmd, "ppt") == 0)
	{
		if(ulParam1 == 0) //mediaset
		{
			BUS_MGR_Create("xmgr_cas_NaPpvBanner_Mediaset",
							APP_CAS_PPV_PRIORITY,
							xmgr_cas_NaPpvBanner_Mediaset,
							SVC_PIPE_GetActionHandle(MGR_ACTION_GetMainActionId()),
							0,
							1,
							0);
		}
		else if(ulParam1 == 1) //pangea
		{
			BUS_MGR_Create("xproc_cas_NaPpvBanner_Pangea",
							APP_CAS_PPV_PRIORITY,
							xproc_cas_NaPpvBanner_Pangea,
							SVC_PIPE_GetActionHandle(MGR_ACTION_GetMainActionId()),
							0,
							1,
							0);
		}
	}
	else if (MWC_UTIL_DvbStrcmp(pszCmd, "recharge") == 0)
	{
		if(ulParam1 == 0) //mediaset
		{
			BUS_PostMessage(NULL, eMEVT_XMGR_CAS_NA_NOTIFY_PPV_RECHARGE_ALARM, (Handle_t)CA_OBJECT_NO_ERROR, eDxCAS_GROUPID_NA, (HINT32)0, (HINT32)0);
		}
		else if(ulParam1 == 1) //pangea
		{
			BUS_MGR_Create("xproc_cas_NaPpvPopupRecharge_Pangea",
							APP_CAS_PRIORITY,
							xproc_cas_NaPpvPopupRecharge_Pangea,
							SVC_PIPE_GetActionHandle(MGR_ACTION_GetMainActionId()),
							0,
							ulParam2,
							0);
		}
	}
	else if (MWC_UTIL_DvbStrcmp(pszCmd, "oppv") == 0)
	{
		if(ulParam1 == 0) //mediaset
		{
			OPPV_PURCHASE_INFO_ITEM	item;

			item.nameLength = SvcCas_UtilStrLen(pszmsg);
			item.pName = pszmsg;
			item.pNext = NULL;
			BUS_PostMessage(NULL, eMEVT_XMGR_CAS_NA_NOTIFY_PPV_OPPV_LIST, (Handle_t)CA_OBJECT_NO_ERROR, eDxCAS_GROUPID_NA, (HINT32)1, (HINT32)&item);
		}
		else if(ulParam1 == 1) //pangea
		{
			BUS_MGR_Create("xproc_cas_NaPpvPopupOppv_Pangea",
							APP_CAS_PRIORITY,
							xproc_cas_NaPpvPopupOppv_Pangea,
							SVC_PIPE_GetActionHandle(MGR_ACTION_GetMainActionId()),
							ulParam2,
							SvcCas_UtilStrLen(pszmsg),
							(HINT32)pszmsg);
		}
	}
#endif
#if 0 //defined(CONFIG_MW_CAS_NAGRA_IRDCMD_FINGER) /*To comile for Octo2*/
	if (MWC_UTIL_DvbStrcmp(pszCmd, "ird_finger") == 0)
	{
		if(BUS_MGR_Get(xmgr_cas_NaIRD_FingerPrint_Proc) != NULL)
		{
			HxLOG_Print("[CMD] destroy previous finger print window \n");
			BUS_MGR_Destroy(xmgr_cas_NaIRD_FingerPrint_Proc);
		}
		// ulParam1 = 0 = normal
		// ulParam1 = 1 = standby
		BUS_MGR_Create("xmgr_cas_NaIRD_FingerPrint_Proc", APP_CAS_HIGH_PRIORITY, xmgr_cas_NaIRD_FingerPrint_Proc, (HINT32)0, ulParam1, 0, 0);
	}
#endif
#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_MAIL)
	if (MWC_UTIL_DvbStrcmp(pszCmd, "ird_mail") == 0)
	{
#if defined(CONFIG_DEBUG)
#if 0 //To comile for Octo2
		xmgr_cas_NaDbgCmdIrdCmdMail();
#endif
#endif
	}
#endif

#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_FORMATDISK)
	if (MWC_UTIL_DvbStrcmp(pszCmd, "ird_fd") == 0)
	{
#if defined(CONFIG_DEBUG)
#if 0 //To comile for Octo2
		xmgr_cas_NaDbgCmdIrdCmdFormatDisk();
#endif
#endif
	}
#endif

#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_PINCTL)
	if (MWC_UTIL_DvbStrcmp(pszCmd, "ird_pinctl") == 0)
	{
#if defined(CONFIG_DEBUG)
#if 0 //To comile for Octo2
		xmgr_cas_NaDbgCmd_IrdCmdPincode();
#endif
#endif
	}
#endif

#if defined(CONFIG_MW_CAS_NAGRA_OP_TIVU)
	if (MWC_UTIL_DvbStrcmp(pszCmd, "tivu_ban") == 0)
	{
		HUINT32 nMessage = 0x0;

		if(ulParam2 == 1)
			nMessage = 0x100a;	/*eCAS_NA_CA_SMARTCARD_REMOVED*/
		else if(ulParam2 == 2)
			nMessage =0x1001;	/*eCAS_NA_CA_SMARTCARD_COM_ERROR*/
		else if(ulParam2 == 3)
			nMessage = 0x1004;	/*eCAS_NA_CA_SMARTCARD_BLACKLISTED*/
		else if(ulParam2 == 4)
			nMessage =0x1005;	/*eCAS_NA_CA_SMARTCARD_SUSPENDED*/
		else
			nMessage =0x1000;	/*eCAS_NA_CA_SMARTCARD_OK*/

		if(nMessage != 0x1000)
		{
#if 0
			if(BUS_MGR_Get(xproc_cas_NaBannerSmartCardError_Tivu) != NULL)
			{
				HxLOG_Print("[CMD] destroy previous finger print window \n");
				BUS_MGR_Destroy(xproc_cas_NaBannerSmartCardError_Tivu);
			}

			BUS_MGR_Create("xproc_cas_NaBannerSmartCardError_Tivu", APP_CAS_PPV_PRIORITY, xproc_cas_NaBannerSmartCardError_Tivu,
							SVC_PIPE_GetActionHandle(MGR_ACTION_GetMainActionId()),
							ulParam1,
							nMessage,
							0);
#endif
		}
	}

	if (MWC_UTIL_DvbStrcmp(pszCmd, "tivu_menu") == 0)
	{
		HUINT32 nMessage = 0x0;

		if(ulParam2 == 0)
			nMessage =0x1000;	/*eCAS_NA_CA_SMARTCARD_OK*/
		else if(ulParam2 == 1)
			nMessage = 0x100a;	/*eCAS_NA_CA_SMARTCARD_REMOVED*/
		else if(ulParam2 == 2)
			nMessage =0x1001;	/*eCAS_NA_CA_SMARTCARD_COM_ERROR*/
		else if(ulParam2 == 3)
			nMessage = 0x1004;	/*eCAS_NA_CA_SMARTCARD_BLACKLISTED*/
		else if(ulParam2 == 4)
			nMessage =0x1005;	/*eCAS_NA_CA_SMARTCARD_SUSPENDED*/
		else
			nMessage =0x1000;

#if 0
		if(BUS_MGR_Get(xproc_cas_NaPpvMenuSmartCardError_Tivu) != NULL)
		{
			HxLOG_Print("[CMD] destroy previous finger print window \n");
			BUS_MGR_Destroy(xproc_cas_NaPpvMenuSmartCardError_Tivu);
		}

		BUS_MGR_Create("xproc_cas_NaPpvMenuSmartCardError_Tivu", APP_CAS_PPV_PRIORITY, xproc_cas_NaPpvMenuSmartCardError_Tivu,
						SVC_PIPE_GetActionHandle(MGR_ACTION_GetMainActionId()),
						ulParam1,
						nMessage,
						0);
#endif
	}
#endif

}

HERROR	xmgr_cas_NaGetCountrycode(HUINT8* pszCurLang1, HUINT8* pszCurLang2)
{
	return _xmgr_cas_NaCmdGetCountrycode(pszCurLang1, pszCurLang2);
}


HERROR	xmgr_cas_NaGetCAPrjName(HUINT8* paucCaPrjName, HINT32 nLength)
{
	return _xmgr_cas_NaGetCAProjectName(paucCaPrjName, nLength);
}

HERROR	xmgr_cas_NaRegisterSystemInfoCallback(void)
{
	SvcCas_CtrlParam_t						stParam;
	NA_RegisterSystemInfoCallbackInParam_t	stInParam;

	stParam.ulControlType = eNaCtrl_RegisterSystemInfoCallback;
	stInParam.callback = (TSystemInfoCallback)xmgr_cas_NaSystemInfoCallback;
	stParam.pvIn = (void *)&stInParam;

	return SVC_CAS_CtrlSet(eDxCAS_GROUPID_NA, eNaCtrl_RegisterSystemInfoCallback, &stParam);
	//return MW_CA_NA_RegisterSystemInfoCallback(xmgr_cas_NaSystemInfoCallback);
}

HERROR	xmgr_cas_NaRegisterSmartcardInfoCallback(void)
{
	SvcCas_CtrlParam_t						stParam;
	NA_RegisterScAlarmCallbackInParam_t		stInParam;

	stParam.ulControlType = eNaCtrl_RegisterScAlarmCallback;
	stInParam.callback = (TSmartcardAlarmCallback)xmgr_cas_NaSmartcardInfoCallback;
	stParam.pvIn = (void *)&stInParam;

	return SVC_CAS_CtrlSet(eDxCAS_GROUPID_NA, eNaCtrl_RegisterScAlarmCallback, &stParam);
	//return MW_CA_NA_RegisterScAlarmCallback(xmgr_cas_NaSmartcardInfoCallback);
}

// ird cmd mail
#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_MAIL)
HERROR	xmgr_cas_NaIrdCmdMailCheckUnreadMessage(void)
{
	HERROR										hErr;
	SvcCas_CtrlParam_t							stParam;
	NA_IRDCMD_MAIL_CheckUnreadMessageOutParam_t	stOutParam;

	stParam.ulControlType = eNaCtrl_IRDCMD_MAIL_CheckUnreadMessage;
	stParam.pvOut = (void *)&stOutParam;

	hErr = SVC_CAS_CtrlCall(eDxCAS_GROUPID_NA, eNaCtrl_IRDCMD_MAIL_CheckUnreadMessage, &stParam);

	if(stOutParam.bUnread == TRUE)
	{
		hErr = ERR_OK;
	}
	else
	{
		hErr = ERR_FAIL;
	}

	return hErr;

	//return MW_CA_NA_IRDCMD_MAIL_CheckUnreadMessage();
}

HERROR	xmgr_cas_NaIrdCmdMailClearMailMessageHeader(void)
{
	SvcCas_CtrlParam_t	stParam;

	stParam.ulControlType = eNaCtrl_IRDCMD_MAIL_ClearMailMessageHeader;

	return SVC_CAS_CtrlCall(eDxCAS_GROUPID_NA, eNaCtrl_IRDCMD_MAIL_ClearMailMessageHeader, &stParam);
	//return MW_CA_NA_IRDCMD_MAIL_ClearMailMessageHeader();
}

HERROR	xmgr_cas_NaIrdCmdMailMakeMailMessage(HUINT8 *pIrdCmd, HUINT16	*pnMailId, HBOOL *pfCompleted, HUINT16 *pnDeletedMailId)
{
	HERROR										hErr;
	SvcCas_CtrlParam_t							stParam;
	NA_IRDCMD_MAIL_MakeMailMessageInParam_t		stInParam;
	NA_IRDCMD_MAIL_MakeMailMessageOutParam_t	stOutParam;

	stParam.ulControlType = eNaCtrl_IRDCMD_MAIL_MakeMailMessage;
	stInParam.pIrdCmd = pIrdCmd;
	stParam.pvIn = (void *)&stInParam;
	stParam.pvOut = (void *)&stOutParam;
	hErr = SVC_CAS_CtrlCall(eDxCAS_GROUPID_NA, eNaCtrl_IRDCMD_MAIL_MakeMailMessage, &stParam);

	*pnMailId = stOutParam.nMailId;
	*pfCompleted = stOutParam.fCompleted;
	*pnDeletedMailId = stOutParam.nDeletedMailId;

	return hErr;
	//return MW_CA_NA_IRDCMD_MAIL_MakeMailMessage((TUnsignedInt8*)pIrdCmd, (TUnsignedInt16*)pnMailId, (TBoolean*) pfCompleted, (TUnsignedInt16*) pnDeletedMailId);
}

HERROR	xmgr_cas_NaIrdCmdMailLoadMailMessages(void)
{
	SvcCas_CtrlParam_t		stParam;

	stParam.ulControlType = eNaCtrl_IRDCMD_MAIL_LoadMailMessages;
	return SVC_CAS_CtrlCall(eDxCAS_GROUPID_NA, eNaCtrl_IRDCMD_MAIL_LoadMailMessages, &stParam);

	//return MW_CA_NA_IRDCMD_MAIL_LoadMailMessages();
}

HERROR	xmgr_cas_NaIrdCmdMailGetMailPriority(HUINT16 nMailId, NA_MAIL_PRIORITY *pnPriority)
{
	HERROR										hErr;
	SvcCas_CtrlParam_t							stParam;
	NA_IRDCMD_MAIL_GetMailPriorityInParam_t		stInParam;
	NA_IRDCMD_MAIL_GetMailPriorityOutParam_t	stOutParam;

	stParam.ulControlType = eNaCtrl_IRDCMD_MAIL_GetMailPriority;
	stInParam.nMailId = nMailId;
	stParam.pvIn = (void *)&stInParam;
	stParam.pvOut = (void *)&stOutParam;
	hErr = SVC_CAS_CtrlGet(eDxCAS_GROUPID_NA, eNaCtrl_IRDCMD_MAIL_GetMailPriority, &stParam);
	*pnPriority = stOutParam.nPriority;

	//return MW_CA_NA_IRDCMD_MAIL_GetMailPriority((TUnsignedInt16) nMailId, pnPriority);

	return hErr;
}

HERROR	xmgr_cas_NaIrdCmdMailGetMailList(NA_MAIL_MSG ***pppMailList, HUINT32 *pnCount)
{
	HERROR									hErr;
	SvcCas_CtrlParam_t						stParam;
	NA_IRDCMD_MAIL_GetMailListOutParam_t	stOutParam;

	stParam.ulControlType = eNaCtrl_IRDCMD_MAIL_GetMailList;
	stParam.pvOut = (void *)&stOutParam;
	stOutParam.pppMailList = pppMailList;
	hErr = SVC_CAS_CtrlGet(eDxCAS_GROUPID_NA, eNaCtrl_IRDCMD_MAIL_GetMailList, &stParam);
	*pnCount = stOutParam.nCount;

	return hErr;
	//return MW_CA_NA_IRDCMD_MAIL_GetMailList(pppMailList, (TUnsignedInt32 *)pnCount);
}

HERROR	xmgr_cas_NaIrdCmdMailGetMailData(HUINT16 nMailId, NA_MAIL_MSG **ppMailData)
{
	HERROR									hErr;
	SvcCas_CtrlParam_t						stParam;
	NA_IRDCMD_MAIL_GetMailDataInParam_t		stInParam;
	NA_IRDCMD_MAIL_GetMailDataOutParam_t	stOutParam;

	stParam.ulControlType = eNaCtrl_IRDCMD_MAIL_GetMailPriority;
	stInParam.nMailId = nMailId;
	stParam.pvIn = (void *)&stInParam;
	stOutParam.ppMailData = ppMailData;
	stParam.pvOut = (void *)&stOutParam;
	hErr = SVC_CAS_CtrlGet(eDxCAS_GROUPID_NA, eNaCtrl_IRDCMD_MAIL_GetMailPriority, &stParam);

	return hErr;
	//return MW_CA_NA_IRDCMD_MAIL_GetMailData((TUnsignedInt16) nMailId, ppMailData);
}

HERROR	xmgr_cas_NaIrdCmdMailDeleteMailMessage(HUINT16 nMailId)
{
	HERROR										hErr;
	SvcCas_CtrlParam_t							stParam;
	NA_IRDCMD_MAIL_DeleteMailMessageInParam_t	stInParam;

	stParam.ulControlType = eNaCtrl_IRDCMD_MAIL_DeleteMailMessage;
	stInParam.nMailId = nMailId;
	stParam.pvIn = (void *)&stInParam;
	hErr = SVC_CAS_CtrlCall(eDxCAS_GROUPID_NA, eNaCtrl_IRDCMD_MAIL_DeleteMailMessage, &stParam);

	return hErr;
	//return MW_CA_NA_IRDCMD_MAIL_DeleteMailMessage((TUnsignedInt16) nMailId);
}

HERROR	xmgr_cas_NaIrdCmdMailUpdateReadFlag(HUINT16 nMailId, HBOOL bRead)
{
	HERROR									hErr;
	SvcCas_CtrlParam_t						stParam;
	NA_IRDCMD_MAIL_UpdateReadFlagInParam_t	stInParam;

	stParam.ulControlType = eNaCtrl_IRDCMD_MAIL_UpdateReadFlag;
	stInParam.nMailId = nMailId;
	stInParam.bRead = bRead;

	hErr = SVC_CAS_CtrlCall(eDxCAS_GROUPID_NA, eNaCtrl_IRDCMD_MAIL_UpdateReadFlag, &stParam);

	return hErr;
	//return MW_CA_NA_IRDCMD_MAIL_UpdateReadFlag((TUnsignedInt16) nMailId, (TBoolean) bRead);
}

HERROR	xmgr_cas_NaIrdCmdMailGetMailTitleAndText(HUINT8 *pszMailMessage, HUINT8 **ppszMailTitle, HUINT32 *pnTitleLength, HUINT8 **ppszMailText)
{
	HUINT32	nStrLen = 0;
	HUINT32	i = 0;
	HUINT8	ucPrevChar = 0;
	HUINT8	*pszMsg = NULL;

	if ( pszMailMessage == NULL )
	{
		HxLOG_Error("pszMailMessage == NULL \n");
		return ERR_FAIL;
	}

	if ( ppszMailTitle == NULL || pnTitleLength == NULL || ppszMailText == NULL )
	{
		HxLOG_Error("ppszMailTitle == NULL || pnTitleLength == NULL || ppszMailText == NULL\n");
		return ERR_FAIL;
	}

	nStrLen = SvcCas_UtilStrLen(pszMailMessage);

	if ( nStrLen == 0 )
	{
		HxLOG_Error("nStrLen == 0\n");
		return ERR_FAIL;
	}

	pszMsg = pszMailMessage;
	*ppszMailTitle = pszMailMessage;
	i = 0;
	while( *pszMsg != '\0' )
	{
		if ( ucPrevChar == ':'  && *pszMsg == ':' )
		{
			*pnTitleLength = i - 1;
			*ppszMailText = pszMsg + 1;
			return ERR_OK;
		}
		ucPrevChar = *pszMsg;
		pszMsg++;
		i++;
	}

	*ppszMailTitle = NULL;
	*pnTitleLength = 0;
	*ppszMailText = pszMailMessage;

	return ERR_OK;
}
#endif

#if defined(CONFIG_MW_CAS_NAGRA_OP_MEDIASET)
HERROR	xmgr_cas_NaGetScExpireDateStr(HUINT8 *pszDate, HBOOL	 *bExpired)
{
	HERROR								hErr;
	SvcCas_CtrlParam_t					stParam;
	NA_GetScExpireDateForPpvOutParam_t	stOutParam;

	stParam.ulControlType = eNaCtrl_GetScExpireDateForPpv;
	stOutParam.pszDate = pszDate;
	stParam.pvOut = (void *)&stOutParam;

	hErr = SVC_CAS_CtrlGet(eDxCAS_GROUPID_NA, eNaCtrl_GetScExpireDateForPpv, &stParam);

	*bExpired = stOutParam.bExpired;

	return hErr;
	//return MW_CA_NA_GetScExpireDateForPpv(pszDate, bExpired);
}

HERROR	xmgr_cas_NaGetScCreditStr(HUINT8 *pszCredit)
{
	HERROR						hErr;
	SvcCas_CtrlParam_t			stParam;
	NA_GetScCreditStrOutParam_t	stOutParam;

	stParam.ulControlType = eNaCtrl_GetScCreditStr;
	stOutParam.pszCredit = pszCredit;
	stParam.pvOut = (void *)&stOutParam;

	hErr = SVC_CAS_CtrlGet(eDxCAS_GROUPID_NA, eNaCtrl_GetScCreditStr, &stParam);

	return hErr;
	//return MW_CA_NA_GetScCreditStr(pszCredit);
}

HERROR	xmgr_cas_NaGetScCredit(HUINT32 *pCredit)
{
	HERROR						hErr;
	SvcCas_CtrlParam_t			stParam;
	NA_GetScCreditOutParam_t	stOutParam;

	stParam.ulControlType = eNaCtrl_GetScCredit;
	stParam.pvOut = (void *)&stOutParam;

	hErr = SVC_CAS_CtrlGet(eDxCAS_GROUPID_NA, eNaCtrl_GetScCredit, &stParam);
	*pCredit = stOutParam.ulCredit;

	return hErr;
	//return MW_CA_NA_GetScCredit((TCreditAmount*)pCredit);
}

HERROR	xmgr_cas_NaGetCurrentDateStr(HUINT8 *pszDate)
{
	HERROR							hErr;
	SvcCas_CtrlParam_t				stParam;
	NA_GetCurrentDateStrOutParam_t	stOutParam;

	stParam.ulControlType = eNaCtrl_GetCurrentDateStr;
	stOutParam.pszDate = pszDate;
	stParam.pvOut = (void *)&stOutParam;

	hErr = SVC_CAS_CtrlGet(eDxCAS_GROUPID_NA, eNaCtrl_GetCurrentDateStr, &stParam);

	return hErr;
	//return MW_CA_NA_GetCurrentDateStr(pszDate);
}


HERROR	xmgr_cas_NaDisposeCaRequest(HUINT32* pRequest)
{
	HERROR						hErr;
	SvcCas_CtrlParam_t			stParam;
	NA_DisposeRequestInParam_t	stInParam;

	stParam.ulControlType = eNaCtrl_DisposeRequest;
	stInParam.pRequest = pRequest;
	stParam.pvIn = (void *)&stInParam;
	hErr = SVC_CAS_CtrlReq(eDxCAS_GROUPID_NA, eNaCtrl_DisposeRequest, &stParam);

	return hErr;
	//return MW_CA_NA_DisposeRequest(pRequest);
}

HERROR	xmgr_cas_NaRequestTsCheck(Handle_t hSvcHandle)
{
	HERROR				hErr;
	SvcCas_CtrlParam_t	stParam;
	NA_TsCheckInParam_t	stInParam;

	stParam.ulControlType = eNaCtrl_TsCheck;
	stInParam.hSvcHandle = hSvcHandle;
	stParam.pvIn = (void *)&stInParam;
	hErr = SVC_CAS_CtrlReq(eDxCAS_GROUPID_NA, eNaCtrl_TsCheck, &stParam);

	return hErr;
	//return MW_CA_NA_TsCheck(hSvcHandle);
}

HERROR	xmgr_cas_NaRequestEventInfo(void)
{
	HERROR				hErr;
	SvcCas_CtrlParam_t	stParam;

	stParam.ulControlType = eNaCtrl_RequestEventInfo;
	hErr = SVC_CAS_CtrlReq(eDxCAS_GROUPID_NA, eNaCtrl_RequestEventInfo, &stParam);

	return hErr;
	//return MW_CA_NA_RequestEventInfo();
}

HERROR	xmgr_cas_NaGetPurchaseHistory(HUINT32 ulProviderId)
{
	HERROR								hErr;
	SvcCas_CtrlParam_t					stParam;
	NA_RequestPurchaseHistoryInParam_t	stInParam;

	stParam.ulControlType = eNaCtrl_RequestPurchaseHistory;
	stInParam.ulProviderId = ulProviderId;
	stParam.pvIn = (void *)&stInParam;
	hErr = SVC_CAS_CtrlReq(eDxCAS_GROUPID_NA, eNaCtrl_RequestPurchaseHistory, &stParam);

	return hErr;
	//return MW_CA_NA_RequestPurchaseHistory(ulProviderId);
}

HERROR	xmgr_cas_NaGetRechargeList(void)
{
	HERROR				hErr;
	SvcCas_CtrlParam_t	stParam;

	stParam.ulControlType = eNaCtrl_RequestRechargeList;
	hErr = SVC_CAS_CtrlReq(eDxCAS_GROUPID_NA, eNaCtrl_RequestRechargeList, &stParam);

	return hErr;
	//return MW_CA_NA_RequestRechargeList();
}

HERROR	xmgr_cas_NaCakGetProviderId(HUINT16 *pProviderId)
{
	HERROR							hErr;
	SvcCas_CtrlParam_t				stParam;
	NA_CakGetProviderIdOutParam_t	stOutParam;

	stParam.ulControlType = eNaCtrl_CakGetProviderId;
	stParam.pvOut = (void *)&stOutParam;
	hErr = SVC_CAS_CtrlGet(eDxCAS_GROUPID_NA, eNaCtrl_CakGetProviderId, &stParam);
	*pProviderId = stOutParam.usProviderId;

	return hErr;
	//return MW_CA_NA_CakGetProviderId((TPpid*)pProviderId);
}

HERROR	xmgr_cas_NaCakGetFullPackageList(HUINT16 usProviderId)
{
	HERROR									hErr;
	SvcCas_CtrlParam_t						stParam;
	NA_RequestFullInfoPackageListInParam_t	stInParam;

	stParam.ulControlType = eNaCtrl_RequestFullInfoPackageList;
	stInParam.providerId = usProviderId;
	stParam.pvIn = (void *)&stInParam;
	hErr = SVC_CAS_CtrlReq(eDxCAS_GROUPID_NA, eNaCtrl_RequestFullInfoPackageList, &stParam);

	return hErr;
	//return MW_CA_NA_RequestFullInfoPackageList((TPpid)usProviderId);
}

HERROR	xmgr_cas_NaGetPackageList(HUINT32 ulProviderId)
{
	HERROR							hErr;
	SvcCas_CtrlParam_t				stParam;
	NA_RequestPackageListInParam_t	stInParam;

	stParam.ulControlType = eNaCtrl_RequestPackageList;
	stInParam.ulProviderId = ulProviderId;
	stParam.pvIn = (void *)&stInParam;
	hErr = SVC_CAS_CtrlReq(eDxCAS_GROUPID_NA, eNaCtrl_RequestPackageList, &stParam);

	return hErr;
	//return MW_CA_NA_RequestPackageList(ulProviderId);
}

HERROR	xmgr_cas_NaGetPackageEventInfo(HUINT32* pRequest, HUINT32* pPackage, void **ppPackageInfo)
{
	HERROR								hErr;
	SvcCas_CtrlParam_t					stParam;
	NA_GetPackageEventInfoInParam_t		stInParam;
	NA_GetPackageEventInfoOutParam_t	stOutParam;

	stParam.ulControlType = eNaCtrl_GetPackageEventInfo;
	stInParam.pRequest = pRequest;
	stInParam.pPackage = pPackage;
	stParam.pvIn = (void *)&stInParam;
	stOutParam.pPackageInfo = ppPackageInfo;
	stParam.pvOut = (void *)&stOutParam;
	hErr = SVC_CAS_CtrlGet(eDxCAS_GROUPID_NA, eNaCtrl_GetPackageEventInfo, &stParam);

	return hErr;
	//return MW_CA_NA_GetPackageEventInfo(pRequest, pPackage, (PACKAGE_INFO **)pPackageInfo);
}

HERROR	xmgr_cas_NaGetPurchaseHistoryNum(HUINT32* pNbItems)
{
	HERROR								hErr;
	SvcCas_CtrlParam_t					stParam;
	NA_GetPurchaseHistoryNumOutParam_t	stOutParam;

	stParam.ulControlType = eNaCtrl_GetPurchaseHistoryNum;
	stParam.pvOut = (void *)&stOutParam;
	hErr = SVC_CAS_CtrlGet(eDxCAS_GROUPID_NA, eNaCtrl_GetPurchaseHistoryNum, &stParam);
	*pNbItems = stOutParam.ulItems;

	return hErr;
	//return MW_CA_NA_GetPurchaseHistoryNum(pNbItems);
}

HERROR	xmgr_cas_NaCheckProductPurchasable(HUINT32* pRequest, HUINT32*  pProduct)
{
	HERROR									hErr;
	SvcCas_CtrlParam_t						stParam;
	NA_RequestProductPurchasableInParam_t	stInParam;

	stParam.ulControlType = eNaCtrl_RequestProductPurchasable;
	stInParam.pRequest = pRequest;
	stInParam.pProduct = pProduct;

	stParam.pvIn = (void *)&stInParam;
	hErr = SVC_CAS_CtrlCall(eDxCAS_GROUPID_NA, eNaCtrl_RequestProductPurchasable, &stParam);

	return hErr;
	//return MW_CA_NA_RequestProductPurchasable(pRequest, pProduct);
}

HERROR	xmgr_cas_NaRegisterDgtviSmartcardInfoCallback_Mediaset(void* callback)
{
	HERROR										hErr;
	SvcCas_CtrlParam_t							stParam;
	NA_RegisterDgtviScAlarmCallbackInParam_t	stInParam;

	stParam.ulControlType = eNaCtrl_RegisterDgtviScAlarmCallback;
	stInParam.callback = (TSmartcardAlarmCallback*)callback;
	stParam.pvIn = (void *)&stInParam;
	hErr = SVC_CAS_CtrlSet(eDxCAS_GROUPID_NA, eNaCtrl_RegisterDgtviScAlarmCallback, &stParam);

	return hErr;
	//return MW_CA_NA_RegisterDgtviScAlarmCallback((TSmartcardAlarmCallback*)callback);
}
#endif	//end of #if defined(CONFIG_MW_CAS_NAGRA_OP_MEDIASET)

#if defined(CONFIG_MW_CAS_NAGRA_OP_TIVU)
HBOOL	xmgr_cas_NaCheckTivuService(void)
{
	HERROR								hErr;
	SvcCas_CtrlParam_t					stParam;
	NA_CheckTivuServiceOutParam_t		stOutParam;

	stParam.ulControlType = eNaCtrl_CheckTivuService;
	stParam.pvOut = (void *)&stOutParam;
	hErr = SVC_CAS_CtrlCall(eDxCAS_GROUPID_NA, eNaCtrl_CheckTivuService, &stParam);

	return (HBOOL)stOutParam.bStatus;
	//return (HBOOL)MW_CA_NA_CheckTivuService();
}

HBOOL	xmgr_cas_NaCheckTivuCardSystemId(void)
{
	HERROR								hErr;
	SvcCas_CtrlParam_t					stParam;
	NA_CheckTivuCardSystemIdOutParam_t	stOutParam;

	stParam.ulControlType = eNaCtrl_CheckTivuCardSystemId;
	stParam.pvOut = (void *)&stOutParam;
	hErr = SVC_CAS_CtrlCall(eDxCAS_GROUPID_NA, eNaCtrl_CheckTivuCardSystemId, &stParam);

	return (HBOOL)stOutParam.bStatus;
	//return (HBOOL)MW_CA_NA_CheckTivuCardSystemId();
}
#endif

HERROR	xmgr_cas_NaIsScRemoved(HUINT8 ucIndex, HBOOL *pbRemoved)
{
	HERROR hErr;
	TSmartcardAlarm eScAlarm;

	if (NULL == pbRemoved)
		return ERR_FAIL;

	*pbRemoved = FALSE;

	hErr = xmgr_cas_NaGetCurrScStatus(ucIndex, &eScAlarm);
	if ((hErr == ERR_OK) && (eScAlarm == CA_ALARM_SMARTCARD_REMOVED))
	{
		*pbRemoved = TRUE;
		return ERR_OK;
	}

	return ERR_FAIL;
}

#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD)
HERROR	xmgr_cas_NaResetIRDCmdData(void)
{
	HERROR					hErr;
	SvcCas_CtrlParam_t		stParam;

	stParam.ulControlType = eNaCtrl_ResetIRDCmdData;

	hErr = SVC_CAS_CtrlCall(eDxCAS_GROUPID_NA, eNaCtrl_ResetIRDCmdData, &stParam);

	return hErr;
}
#endif

#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_EMMWAKEUP)
HERROR	xmgr_cas_NaIrdCmdEmmGetWakeupTime(UNIXTIME *pulTime)
{
	HERROR									hErr;
	SvcCas_CtrlParam_t						stParam;
	NA_IRDCMD_EMM_GetWakeupTimeOutParam_t	stOutParam;

	stParam.ulControlType = eNaCtrl_IRDCMD_EMM_GetWakeupTime;
	stParam.pvOut = (void *)&stOutParam;
	hErr = SVC_CAS_CtrlGet(eDxCAS_GROUPID_NA, eNaCtrl_IRDCMD_EMM_GetWakeupTime, &stParam);
	*pulTime = stOutParam.ulTime;

	return hErr;
	//hErr = MW_CA_NA_IRDCMD_EMM_GetWakeupTime(pulTime);
}

HERROR	xmgr_cas_NaIrdCmdEmmInitWakingTable(void)
{
	HERROR				hErr;
	SvcCas_CtrlParam_t	stParam;

	stParam.ulControlType = eNaCtrl_IRDCMD_EMM_InitWakingTable;

	hErr = SVC_CAS_CtrlCall(eDxCAS_GROUPID_NA, eNaCtrl_IRDCMD_EMM_InitWakingTable, &stParam);

	return hErr;
	//return MW_CA_NA_IRDCMD_EMM_InitWakingTable();
}

HERROR	xmgr_cas_NaIrdCmdEmmHasMoreNextWaking(TBoolean *pbNext)
{
	HERROR										hErr;
	SvcCas_CtrlParam_t							stParam;
	NA_IRDCMD_EMM_HasMoreNextWakingOutParam_t	stOutParam;

	stParam.ulControlType = eNaCtrl_IRDCMD_EMM_HasMoreNextWaking;
	stParam.pvOut = (void *)&stOutParam;

	hErr = SVC_CAS_CtrlCall(eDxCAS_GROUPID_NA, eNaCtrl_IRDCMD_EMM_HasMoreNextWaking, &stParam);
	*pbNext = stOutParam.bNext;

	return hErr;
	//return MW_CA_NA_IRDCMD_EMM_HasMoreNextWaking(bNext);
}

HERROR	xmgr_cas_NaIrdCmdEmmGetWakingTuneInfo(TUnsignedInt8 *pucNetworkIdFlag, TUnsignedInt16 *pusNetworkId,
																TUnsignedInt8 *pucTsIdFlag, TUnsignedInt16 *pusTsId,
																TUnsignedInt8 *pucServiceIdFlag, TUnsignedInt16 *pusServiceId)
{
	HERROR										hErr;
	SvcCas_CtrlParam_t							stParam;
	NA_IRDCMD_EMM_GetWakingTuneInfoOutParam_t	stOutParam;

	stParam.ulControlType = eNaCtrl_IRDCMD_EMM_GetWakingTuneInfo;
	stParam.pvOut = (void *)&stOutParam;
	hErr = SVC_CAS_CtrlGet(eDxCAS_GROUPID_NA, eNaCtrl_IRDCMD_EMM_GetWakingTuneInfo, &stParam);
	*pucNetworkIdFlag = stOutParam.ucNetworkIdFlag;
	*pusNetworkId = stOutParam.usNetworkId;
	*pucTsIdFlag = stOutParam.ucTsIdFlag;
	*pusTsId = stOutParam.usTsId;
	*pucServiceIdFlag = stOutParam.ucServiceIdFlag;
	*pusServiceId = stOutParam.usServiceId;

	return hErr;
	//return MW_CA_NA_IRDCMD_EMM_GetWakingTuneInfo(pucNetworkIdFlag, pusNetworkId,
	//													pucTsIdFlag, pusTsId,
	//													pucServiceIdFlag, pusServiceId);
}

HERROR	xmgr_cas_NaIrdCmdEmmGetWakingTimer(UNIXTIME *pulTime)
{
	HERROR										hErr;
	SvcCas_CtrlParam_t							stParam;
	NA_IRDCMD_EMM_GetWakingTimerOutParam_t		stOutParam;

	stParam.ulControlType = eNaCtrl_IRDCMD_EMM_GetWakingTimer;
	stParam.pvOut = (void *)&stOutParam;
	hErr = SVC_CAS_CtrlGet(eDxCAS_GROUPID_NA, eNaCtrl_IRDCMD_EMM_GetWakingTimer, &stParam);
	*pulTime = stOutParam.ulTime;

	return hErr;
	//return MW_CA_NA_IRDCMD_EMM_GetWakingTimer(pulTime);
}
#endif

#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_POPUP)
HERROR	xmgr_cas_NaIrdCmdPopupDisplayPopupMessage(void)
{
	HERROR										hErr;
	SvcCas_CtrlParam_t	stParam;

	stParam.ulControlType = eNaCtrl_IRDCMD_POPUP_DisplayPopupMessage;
	hErr = SVC_CAS_CtrlCall(eDxCAS_GROUPID_NA, eNaCtrl_IRDCMD_POPUP_DisplayPopupMessage, &stParam);

	return hErr;
	//return MW_CA_NA_IRDCMD_POPUP_DisplayPopupMessage();
}
#endif

HERROR	xmgr_cas_NaCheckPincode(TPinIndex xPinIndex, TPinCode xPinCode, TUnsignedInt8 *pucPinCode, TPinCodeStatus *xStatus)
{
	HERROR						hErr;
	SvcCas_CtrlParam_t			stParam;
	NA_CheckPincodeInParam_t	stInParam;
	NA_CheckPincodeOutParam_t	stOutParam;

	stInParam.xPinIndex = xPinIndex;
	stInParam.xPinCode = xPinCode;
	stInParam.pucPinCode = pucPinCode;

	stParam.ulControlType = eNaCtrl_CheckPincode;
	stParam.pvIn = (void *)&stInParam;
	stParam.pvOut = (void *)&stOutParam;
	hErr = SVC_CAS_CtrlCall(eDxCAS_GROUPID_NA, eNaCtrl_CheckPincode, &stParam);
	*xStatus = stOutParam.xStatus;

	return hErr;
	//return MW_CA_NA_CheckPincode(xPinIndex, xPinCode, pucPinCode, xStatus);
}

HERROR	xmgr_cas_NaChangePincode(TPinIndex xPinIndex, TPinCode xOldPinCode, TPinCode xNewPinCode, TUnsignedInt8 *pucOldPinCode, TUnsignedInt8 *pucNewPinCode, TPinCodeStatus *xStatus)
{
	HERROR						hErr;
	SvcCas_CtrlParam_t			stParam;
	NA_ChangePincodeInParam_t	stInParam;
	NA_ChangePincodeOutParam_t	stOutParam;

	stInParam.xPinIndex = xPinIndex;
	stInParam.xOldPinCode = xOldPinCode;
	stInParam.xNewPinCode = xNewPinCode;
	stInParam.pucOldPinCode = pucOldPinCode;
	stInParam.pucNewPinCode = pucNewPinCode;

	stParam.ulControlType = eNaCtrl_ChangePincode;
	stParam.pvIn = (void *)&stInParam;
	stParam.pvOut = (void *)&stOutParam;
	hErr = SVC_CAS_CtrlCall(eDxCAS_GROUPID_NA, eNaCtrl_ChangePincode, &stParam);
	*xStatus = stOutParam.xStatus;

	return hErr;
	//return MW_CA_NA_ChangePincode(xPinIndex, xOldPinCode, xNewPinCode, pucOldPinCode, pucNewPinCode, xStatus);
}
/* End of File */

#if defined(CONFIG_OP_CAS_NAGRA_CAKTEST_PPT)
HERROR	xmgr_cas_NaCak7EnablingConsumptionForced(HBOOL bFlag)
{
	HERROR										hErr;
	SvcCas_CtrlParam_t							stParam;
	NA_Cak7_EnablingConsumptionForcedInParam_t	stInParam;

	stParam.ulControlType = eNaCtrl_Cak7_EnablingConsumptionForced;
	stInParam.bOneShot = bFlag;
	stParam.pvIn = (void *)&stInParam;
	hErr = SVC_CAS_CtrlReq(eDxCAS_GROUPID_NA, eNaCtrl_Cak7_EnablingConsumptionForced, &stParam);

	return hErr;
	//return MW_CA_NA_Cak7_EnablingConsumptionForced(ulProviderId);
}
#endif

#if defined(CONFIG_MW_CAS_NAGRA_PRM)
HERROR xmgr_cas_NadvlSystemInformationRequest
(
	TDvlVersionString   xVersionString,
	TUnsignedInt32*    pxIrdSerialNumberValue,
	TDvlUniqueIdString  xIrdSerialNumberString,
	TUnsignedInt32*    pxNuidValue,
	TDvlUniqueIdString  xNuidString
)
{
	HERROR							hErr;
	SvcCas_CtrlParam_t				stParam;
	NA_dvlSystemInformationRequestOutParam_t	stOutParam;

	stParam.ulControlType = eNaCtrl_dvlSystemInformationRequest;
	stParam.pvIn			= NULL;
	stParam.pvOut = (void *)&stOutParam;

	hErr = SVC_CAS_CtrlCall(eDxCAS_GROUPID_NA, eNaCtrl_dvlSystemInformationRequest, &stParam);

	if(hErr == ERR_OK)
	{
		HxSTD_memcpy(xVersionString, stOutParam.xVersionString, DVL_VERSION_STRING_LENGTH);
		*pxIrdSerialNumberValue = stOutParam.xIrdSerialNumberValue;
		HxSTD_memcpy(xIrdSerialNumberString, stOutParam.xIrdSerialNumberString, DVL_UNIQUE_ID_STRING_LENGTH);
		*pxNuidValue = stOutParam.xNuidValue;
		HxSTD_memcpy(xNuidString, stOutParam.xNuidString, DVL_UNIQUE_ID_STRING_LENGTH);
	}

	return hErr;
}

HERROR xmgr_cas_NadvlChipsetInformationRequest(TDvlChipsetInfo**   ppxChipsetInformation)
{
	HERROR							hErr;
	SvcCas_CtrlParam_t				stParam;
	NA_dvlChipsetInformationRequestOutParam_t	stOutParam;

	stParam.ulControlType = eNaCtrl_dvlChipsetInformationRequest;
	stParam.pvIn			= NULL;
	stParam.pvOut = (void *)&stOutParam;

	stOutParam.ppxChipsetInformation = ppxChipsetInformation;

	hErr = SVC_CAS_CtrlCall(eDxCAS_GROUPID_NA, eNaCtrl_dvlChipsetInformationRequest, &stParam);

	return hErr;
}

HERROR xmgr_cas_NadvlReleaseBuffer(const TUnsignedInt8* pxBuffer)
{
	HERROR							hErr;
	SvcCas_CtrlParam_t				stParam;
	NA_dvlReleaseBufferInParam_t	stInParam;

	stParam.ulControlType = eNaCtrl_dvlReleaseBuffer;
	stInParam.pxBuffer = pxBuffer;
	stParam.pvIn = (void *)&stInParam;

	hErr = SVC_CAS_CtrlCall(eDxCAS_GROUPID_NA, eNaCtrl_dvlReleaseBuffer, &stParam);

	return hErr;
}

HERROR xmgr_cas_NadvlCredentialsInformationRequest
(
        TSize                 xCredentialsSize,
  const TUnsignedInt8*       pxCredentials,
        TCredentialsType*    pxCredentialsType,
  const TCredentialsUnion** ppxCredentialsUnion,
        TSize*               pxGenericMetadataSize,
  const TUnsignedInt8**     ppxGenericMetadata,
        TSize*               pxSpecificMetadataSize,
  const TUnsignedInt8**     ppxSpecificMetadata
)
{
	HERROR											hErr;
	SvcCas_CtrlParam_t								stParam;
	NA_dvlCredentialsInformationRequestInParam_t	stInParam;
	NA_dvlCredentialsInformationRequestOutParam_t	stOutParam;

	stParam.ulControlType = eNaCtrl_dvlCredentialsInformationRequest;
	stInParam.xCredentialsSize = xCredentialsSize;
	stInParam.pxCredentials = pxCredentials;
	stParam.pvIn = (void *)&stInParam;

	stOutParam.pxCredentialsType = pxCredentialsType;
	stOutParam.ppxCredentialsUnion = ppxCredentialsUnion;
	stOutParam.pxGenericMetadataSize = pxGenericMetadataSize;
	stOutParam.ppxGenericMetadata = ppxGenericMetadata;
	stOutParam.pxSpecificMetadataSize = pxSpecificMetadataSize;
	stOutParam.ppxSpecificMetadata = ppxSpecificMetadata;
	stParam.pvOut = (void *)&stOutParam;

	hErr = SVC_CAS_CtrlReq(eDxCAS_GROUPID_NA, eNaCtrl_dvlCredentialsInformationRequest, &stParam);

	return hErr;
}

HERROR xmgr_cas_NadvlStartPlayback(
		TTransportSessionId xTSid,
		TSize xInputCredentialsSize,
  const TUnsignedInt8* pxInputCredentials,
  		TDvlPlaybackErrorCB xPlaybackErrorCB,
  		TSize* pxOutputCredentialsSize,
  const TUnsignedInt8** ppxOutputCredentials,
  		TDvlAccessStatus* pxPlaybackStatus,
  		TSize* pxGenericMetadataSize,
  const TUnsignedInt8** ppxGenericMetadata,
		TSize* pxSpecificMetadataSize,
  const TUnsignedInt8** ppxSpecificMetadata,
  TDvlHandle** ppxPlaybackHandle)
{
	HERROR								hErr;
	SvcCas_CtrlParam_t					stParam;
	NA_dvlStartPlaybackInParam_t	stInParam;
	NA_dvlStartPlaybackOutParam_t	stOutParam;

	stParam.ulControlType = eNaCtrl_dvlStartPlayback;
	stInParam.xTSid = xTSid;
	stInParam.xInputCredentialsSize = xInputCredentialsSize;
	stInParam.pxInputCredentials = pxInputCredentials;
	stParam.pvIn = (void *)&stInParam;

	stOutParam.xPlaybackErrorCB = xPlaybackErrorCB;
	stOutParam.pxOutputCredentialsSize = pxOutputCredentialsSize;
	stOutParam.ppxOutputCredentials = ppxOutputCredentials;
	stOutParam.pxPlaybackStatus = pxPlaybackStatus;
	stOutParam.pxGenericMetadataSize = pxGenericMetadataSize;
	stOutParam.ppxGenericMetadata = ppxGenericMetadata;
	stOutParam.pxSpecificMetadataSize = pxSpecificMetadataSize;
	stOutParam.ppxSpecificMetadata = ppxSpecificMetadata;
	stOutParam.ppxPlaybackHandle = ppxPlaybackHandle;
	stParam.pvOut = (void *)&stOutParam;

	hErr = SVC_CAS_CtrlCall(eDxCAS_GROUPID_NA, eNaCtrl_dvlStartPlayback, &stParam);

	return hErr;
}

HERROR xmgr_cas_NadvlStopPlayback(TDvlHandle* pxPlaybackHandle)
{
	HERROR							hErr;
	SvcCas_CtrlParam_t				stParam;
	NA_dvlStopPlaybackInParam_t	stInParam;

	stParam.ulControlType = eNaCtrl_dvlStopPlayback;
	stInParam.pxPlaybackHandle = pxPlaybackHandle;
	stParam.pvIn = (void *)&stInParam;

	hErr = SVC_CAS_CtrlCall(eDxCAS_GROUPID_NA, eNaCtrl_dvlStopPlayback, &stParam);

	return hErr;
}

#if defined(CONFIG_MW_CAS_NAGRA_PRM_3_1)
HERROR xmgr_cas_NadvlPlaybackOpenSession(TTransportSessionId xTSid, TDvlHandle** ppxPlaybackHandle)
{
	HERROR								hErr;
	SvcCas_CtrlParam_t					stParam;
	NA_dvlPlaybackOpenSessionInParam_t	stInParam;
	NA_dvlPlaybackOpenSessionOutParam_t	stOutParam;

	stParam.ulControlType = eNaCtrl_dvlPlaybackOpenSession;
	stInParam.xTSid = xTSid;
	stParam.pvIn = (void *)&stInParam;

	stOutParam.ppxPlaybackHandle = ppxPlaybackHandle;
	stParam.pvOut = (void *)&stOutParam;

	hErr = SVC_CAS_CtrlCall(eDxCAS_GROUPID_NA, eNaCtrl_dvlPlaybackOpenSession, &stParam);

	return hErr;
}

HERROR xmgr_cas_NadvlPlaybackSetEntitlements(
        TDvlHandle*                 pxPlaybackHandle,
        TDvlEntitlementsParameters* pxEntitlementsParameters,
        TDvlAccessStatus*           pxPlaybackStatus,
        TSize*                      pxOutputCredentialsSize,
  const TUnsignedInt8**            ppxOutputCredentials,
        TSize*                      pxKeyIdSize,
  const TUnsignedInt8**            ppxKeyId
)
{
	HERROR								hErr;
	SvcCas_CtrlParam_t					stParam;
	NA_dvlPlaybackSetEntitlementsInParam_t	stInParam;
	NA_dvlPlaybackSetEntitlementsOutParam_t stOutParam;

	stParam.ulControlType = eNaCtrl_dvlPlaybackSetEntitlements;
	stInParam.pxPlaybackHandle = pxPlaybackHandle;
	stInParam.pxEntitlementsParameters = pxEntitlementsParameters;
	stParam.pvIn = (void *)&stInParam;

	stOutParam.pxPlaybackStatus = pxPlaybackStatus;
	stOutParam.pxOutputCredentialsSize = pxOutputCredentialsSize;
	stOutParam.ppxOutputCredentials = ppxOutputCredentials;
	stOutParam.pxKeyIdSize = pxKeyIdSize;
	stOutParam.ppxKeyId = ppxKeyId;
	stParam.pvOut = (void *)&stOutParam;

	hErr = SVC_CAS_CtrlSet(eDxCAS_GROUPID_NA, eNaCtrl_dvlPlaybackSetEntitlements, &stParam);

	return hErr;
}


HERROR xmgr_cas_NadvlPlaybackCloseSession(TDvlHandle* pxPlaybackHandle)
{
	HERROR								hErr;
	SvcCas_CtrlParam_t					stParam;
	NA_dvlPlaybackCloseSessionInParam_t	stInParam;

	stParam.ulControlType = eNaCtrl_dvlPlaybackCloseSession;
	stInParam.pxPlaybackHandle = pxPlaybackHandle;
	stParam.pvIn = (void *)&stInParam;

	hErr = SVC_CAS_CtrlCall(eDxCAS_GROUPID_NA, eNaCtrl_dvlPlaybackCloseSession, &stParam);

	return hErr;
}


HERROR xmgr_cas_NadvlPostPrmManagementMessage(TSize xPrmManagementMessageSize, const TUnsignedInt8* pxPrmManagementMessage)
{
	HERROR									hErr;
	SvcCas_CtrlParam_t						stParam;
	NA_dvlPostPrmManagementMessageInParam_t stInParam;

	stParam.ulControlType = eNaCtrl_dvlPostPrmManagementMessage;
	stInParam.xPrmManagementMessageSize = xPrmManagementMessageSize;
	stInParam.pxPrmManagementMessage = pxPrmManagementMessage;
	stParam.pvIn = (void *)&stInParam;

	hErr = SVC_CAS_CtrlCall(eDxCAS_GROUPID_NA, eNaCtrl_dvlPostPrmManagementMessage, &stParam);

	return hErr;
}


HERROR xmgr_cas_NadvlGetAuthenticatedMessage
(
        TSize             xCipheredMessageSize,
  const TUnsignedInt8*   pxCipheredMessage,
        TSize*           pxAuthenticatedDataSize,
  const TUnsignedInt8** ppxAuthenticatedData
)
{
	HERROR											hErr;
	SvcCas_CtrlParam_t								stParam;
	NA_dvlGetAuthenticatedMessageInParam_t	stInParam;
	NA_dvlGetAuthenticatedMessageOutParam_t	stOutParam;

	stParam.ulControlType = eNaCtrl_dvlGetAuthenticatedMessage;
	stInParam.xCipheredMessageSize = xCipheredMessageSize;
	stInParam.pxCipheredMessage = pxCipheredMessage;
	stParam.pvIn = (void *)&stInParam;

	stOutParam.pxAuthenticatedDataSize = pxAuthenticatedDataSize;
	stOutParam.ppxAuthenticatedData = ppxAuthenticatedData;
	stParam.pvOut = (void *)&stOutParam;

	hErr = SVC_CAS_CtrlGet(eDxCAS_GROUPID_NA, eNaCtrl_dvlGetAuthenticatedMessage, &stParam);

	return hErr;
}


HERROR xmgr_cas_NadvlOpaqueDataMessageCreate(TOdmHandle** ppxOpaqueDataMessageHandle)
{
	HERROR									hErr;
	SvcCas_CtrlParam_t						stParam;
	NA_dvlOpaqueDataMessageCreateOutParam_t stOutParam;

	stParam.ulControlType = eNaCtrl_dvlOpaqueDataMessageCreate;
	stOutParam.ppxOpaqueDataMessageHandle = ppxOpaqueDataMessageHandle;
	stParam.pvOut = (void *)&stOutParam;

	hErr = SVC_CAS_CtrlCall(eDxCAS_GROUPID_NA, eNaCtrl_dvlOpaqueDataMessageCreate, &stParam);

	return hErr;
}


HERROR xmgr_cas_NadvlOpaqueDataMessageRelease
(
          TOdmHandle*     pxOpaqueDataMessageHandle
)
{
	HERROR									hErr;
	SvcCas_CtrlParam_t						stParam;
	NA_dvlOpaqueDataMessageReleaseInParam_t stInParam;

	stParam.ulControlType = eNaCtrl_dvlOpaqueDataMessageRelease;
	stInParam.pxOpaqueDataMessageHandle = pxOpaqueDataMessageHandle;
	stParam.pvIn = (void *)&stInParam;

	hErr = SVC_CAS_CtrlCall(eDxCAS_GROUPID_NA, eNaCtrl_dvlOpaqueDataMessageRelease, &stParam);

	return hErr;
}


HERROR xmgr_cas_NadvlOpaqueDataMessageSetApplicationData
(
        TOdmHandle*     pxOpaqueDataMessageHandle,
        TSize	           xApplicationDataSize,
  const TUnsignedInt8* 	pxApplicationData
)
{
	HERROR												hErr;
	SvcCas_CtrlParam_t									stParam;
	NA_dvlOpaqueDataMessageSetApplicationDataInParam_t	stInParam;

	stParam.ulControlType = eNaCtrl_dvlOpaqueDataMessageSetApplicationData;
	stInParam.pxOpaqueDataMessageHandle = pxOpaqueDataMessageHandle;
	stInParam.xApplicationDataSize = xApplicationDataSize;
	stInParam.pxApplicationData = pxApplicationData;
	stParam.pvIn = (void *)&stInParam;

	hErr = SVC_CAS_CtrlSet(eDxCAS_GROUPID_NA, eNaCtrl_dvlOpaqueDataMessageSetApplicationData, &stParam);

	return hErr;
}


HERROR xmgr_cas_NadvlOpaqueDataMessageSetPrmSignalization
(
        TOdmHandle*     pxOpaqueDataMessageHandle,
        TSize	           xPrmSignalizationSize,
  const TUnsignedInt8* 	pxPrmSignalization
)
{
	HERROR												hErr;
	SvcCas_CtrlParam_t									stParam;
	NA_dvlOpaqueDataMessageSetPrmSignalizationInParam_t stInParam;

	stParam.ulControlType = eNaCtrl_dvlOpaqueDataMessageSetPrmSignalization;
	stInParam.pxOpaqueDataMessageHandle = pxOpaqueDataMessageHandle;
	stInParam.xPrmSignalizationSize = xPrmSignalizationSize;
	stInParam.pxPrmSignalization = pxPrmSignalization;
	stParam.pvIn = (void *)&stInParam;

	hErr = SVC_CAS_CtrlSet(eDxCAS_GROUPID_NA, eNaCtrl_dvlOpaqueDataMessageSetPrmSignalization, &stParam);

	return hErr;
}


HERROR xmgr_cas_NadvlOpaqueDataMessageGet
(
        TOdmHandle*      pxOpaqueDataMessageHandle,
        TSize* 	         pxOpaqueDataMessageSize,
  const TUnsignedInt8** ppxOpaqueDataMessage
)
{
	HERROR									hErr;
	SvcCas_CtrlParam_t						stParam;
	NA_dvlOpaqueDataMessageGetInParam_t		stInParam;
	NA_dvlOpaqueDataMessageGetOutParam_t	stOutParam;

	stParam.ulControlType = eNaCtrl_dvlOpaqueDataMessageGet;
	stInParam.pxOpaqueDataMessageHandle = pxOpaqueDataMessageHandle;
	stParam.pvIn = (void *)&stInParam;

	stOutParam.pxOpaqueDataMessageSize = pxOpaqueDataMessageSize;
	stOutParam.ppxOpaqueDataMessage = ppxOpaqueDataMessage;
	stParam.pvOut = (void *)&stOutParam;

	hErr = SVC_CAS_CtrlGet(eDxCAS_GROUPID_NA, eNaCtrl_dvlOpaqueDataMessageGet, &stParam);

	return hErr;
}

HERROR xmgr_cas_AddTransportSessionIdDirect(TTransportSessionId xTsId)
{
	HERROR									hErr;
	SvcCas_CtrlParam_t						stParam;
	NA_AddTransportSessionIdForPrmInParam_t	stInParam;

	stParam.ulControlType = eNaCtrl_AddTransportSessionIdDirect;
	stInParam.xTSid = xTsId;
	stParam.pvIn = (void *)&stInParam;

	hErr = SVC_CAS_CtrlSet(eDxCAS_GROUPID_NA, eNaCtrl_AddTransportSessionIdDirect, &stParam);

	return hErr;
}

HERROR xmgr_cas_RemoveTransportSessionIdDirect(TTransportSessionId xTsId)
{
	HERROR										hErr;
	SvcCas_CtrlParam_t							stParam;
	NA_RemoveTransportSessionIdForPrmInParam_t	stInParam;

	stParam.ulControlType = eNaCtrl_RemoveTransportSessionIdDirect;
	stInParam.xTSid = xTsId;
	stParam.pvIn = (void *)&stInParam;

	hErr = SVC_CAS_CtrlSet(eDxCAS_GROUPID_NA, eNaCtrl_RemoveTransportSessionIdDirect, &stParam);

	return hErr;
}


#endif

HERROR xmgr_cas_NadvlUpdateSpecificLcmMetadata(TSize  xInLcmCredentialsSize, const TUnsignedInt8 *  pxInLcmCredentials, TSize  xSpecificMetadataSize, const TUnsignedInt8 *  pxSpecificMetadata,
									TSize *  pxOutLcmCredentialsSize, const TUnsignedInt8 **  ppxOutLcmCredentials)
{
	HERROR										hErr;
	SvcCas_CtrlParam_t							stParam;
	NA_dvlUpdateSpecificLcmMetadataInParam_t	stInParam;
	NA_dvlUpdateSpecificLcmMetadataOutParam_t  stOutParam;

	stParam.ulControlType = eNaCtrl_dvlUpdateSpecificLcmMetadata;
	stInParam.xInLcmCredentialsSize = xInLcmCredentialsSize;
	stInParam.pxInLcmCredentials = pxInLcmCredentials;
	stInParam.xSpecificMetadataSize = xSpecificMetadataSize;
	stInParam.pxSpecificMetadata = pxSpecificMetadata;
	stParam.pvIn = (void *)&stInParam;

	stOutParam.pxOutLcmCredentialsSize = pxOutLcmCredentialsSize;
	stOutParam.ppxOutLcmCredentials = ppxOutLcmCredentials;
	stParam.pvOut= (void *)&stOutParam;

	hErr = SVC_CAS_CtrlSet(eDxCAS_GROUPID_NA, eNaCtrl_dvlUpdateSpecificLcmMetadata, &stParam);

	return hErr;	
}

#if defined(CONFIG_MW_CAS_NAGRA_PRM_VOD_PVR)
HERROR xmgr_cas_NadvlCreateRecordSessionParameters(TDvlRecordSessionParameters** ppxRecordParameters)
{
	HERROR							hErr;
	SvcCas_CtrlParam_t				stParam;
	NA_dvlCreateRecordSessionParametersOutParam_t	stOutParam;

	stParam.ulControlType = eNaCtrl_dvlCreateRecordSessionParameters;
	stParam.pvIn = NULL;

	stOutParam.ppxRecordParameters = ppxRecordParameters;
	stParam.pvOut = (void *)&stOutParam;

	hErr = SVC_CAS_CtrlCall(eDxCAS_GROUPID_NA, eNaCtrl_dvlCreateRecordSessionParameters, &stParam);

	return hErr;
}

HERROR xmgr_cas_NadvlStartRecordEx(TDvlRecordSessionParameters* pxRecordParameters, TSize* pxOutputCredentialsSize,const TUnsignedInt8** ppxOutputCredentials,
										TDvlRecordStatus* pxRecordingStatus, TDvlHandle** ppxRecordHandle)
{
	HERROR							hErr;
	SvcCas_CtrlParam_t				stParam;
	NA_dvlCreateRecordSessionParametersInParam_t stInParam;
	NA_dvlStartRecordExOutParam_t	stOutParam;

	stParam.ulControlType = eNaCtrl_dvlStartRecordEx;
	stInParam.pxRecordParameters = pxRecordParameters;
	stParam.pvIn = (void *)&stInParam;

	stOutParam.pxOutputCredentialsSize = pxOutputCredentialsSize;
	stOutParam.ppxOutputCredentials = ppxOutputCredentials;
	stOutParam.pxRecordingStatus = pxRecordingStatus;
	stOutParam.ppxRecordHandle = ppxRecordHandle;
	stParam.pvOut = (void *)&stOutParam;

	hErr = SVC_CAS_CtrlCall(eDxCAS_GROUPID_NA, eNaCtrl_dvlStartRecordEx, &stParam);

	return hErr;
}

HERROR xmgr_cas_NadvlStopRecord(TDvlHandle* pxRecordHandle)
{
	HERROR							hErr;
	SvcCas_CtrlParam_t				stParam;
	NA_dvlStopRecordInParam_t	stInParam;

	stParam.ulControlType = eNaCtrl_dvlStopRecord;
	stInParam.pxRecordHandle = pxRecordHandle;
	stParam.pvIn = (void *)&stInParam;

	hErr = SVC_CAS_CtrlCall(eDxCAS_GROUPID_NA, eNaCtrl_dvlStopRecord, &stParam);

	return hErr;
}
#endif

#endif

HCHAR *xmgr_cas_NaUtil_EncodeString(HCHAR *string)
{
    HUINT32		Len = 0;
	HCHAR		*buf = NULL;

	Len = HLIB_STD_URLStrLen(string) + 1;
    buf = HLIB_STD_MemCalloc(Len+1);
    HLIB_STD_URLEncode(buf,string,Len);

    return buf;
}

HERROR xmgr_cas_NaUtil_PostMessageSessionEventToOM(DxCAS_MmiSessionType_e eSessionType, BUS_Callback_t ulUiHandle, Handle_t hSession, HUINT32 ulSlotNumber, HINT32 eEvent)
{
	OxMgrCasUiEvtSession_t	*pstEvent = NULL;

	pstEvent = (OxMgrCasUiEvtSession_t *)HLIB_STD_MemCalloc(sizeof(OxMgrCasUiEvtSession_t));
	if(pstEvent == NULL)
	{
		HxLOG_Error("HLIB_STD_MemCalloc() error\n");
		return ERR_FAIL;
	}

	pstEvent->eSessionType	= eSessionType;
	pstEvent->ulUiHandle	= (HUINT32)ulUiHandle;
	pstEvent->hSession		= hSession;

	if(BUS_PostMessageToOM(NULL, eEvent, HANDLE_NULL, (HINT32)eDxCAS_GROUPID_NA, (HINT32)ulSlotNumber, (HINT32)pstEvent) != ERR_BUS_NO_ERROR)
	{
		HxLOG_Error("BUS_PostMessageToOM() error\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

// HLIB Codepage issue로 임시로 추가한 API
void xmgr_cas_NaUtil_ConvertLanguageCode(HUINT8 *pszMessage)
{
	if(pszMessage[0] == EN300468_8859_3BTYE_FIRST_VALUE && pszMessage[1] == EN300468_8859_3BTYE_2ND_VALUE)
	{
		HUINT32 ulStrOnlyLen = MWC_UTIL_DvbStrlenStrOnly(pszMessage);

		switch(pszMessage[2])
		{
			case EN300468_8859_5_3BYTE_3TH_VALUE:
				pszMessage[2] = eHxCP_DVBLANG_VALUE_8859_05;
				break;

			default:
				return;
		}

		HxSTD_MemMove(&pszMessage[0], &pszMessage[2], ulStrOnlyLen);
		pszMessage[ulStrOnlyLen+1] = '\0';
	}
}

HERROR	xmgr_cas_NaUtil_FactoryDefault(void)
{
	HERROR					hErr;
	SvcCas_CtrlParam_t		stParam;

	stParam.ulControlType = eNaCtrl_FactoryDefault;

	hErr = SVC_CAS_CtrlSet(eDxCAS_GROUPID_NA, eNaCtrl_FactoryDefault, &stParam);

	return hErr;
}

HERROR XMGR_CAS_NaIsHdPlusSmartCard(HUINT8 ucSlotId, HUINT8 *pbHdPlusSc)
{
	return xmgr_cas_NaIsHdPlusSmartCard(ucSlotId, pbHdPlusSc);
}

#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
HERROR	XMGR_CAS_NaIsSameCasIdInSmartCard(HUINT8 ucSlotId, HUINT16 usCaSysId, HBOOL *pbSame)
{
	return xmgr_cas_NaIsSameCasIdInSc(ucSlotId, usCaSysId, (TBoolean *)pbSame);
}
#endif

#if defined(CONFIG_MW_CAS_NAGRA_DALAPI_7_1_ABOVE)
HERROR	xmgr_cas_NaGetStandardErrorCode(TUnsignedInt32 ulSessionId, TUnsignedInt32* pStandardErrorCode)
{
	SvcCas_CtrlParam_t stParam;
	NA_GetStandardErrorCodeInParam_t stInParam;
	NA_GetStandardErrorCodeOutParam_t stOutParam;

	stParam.ulControlType = eNaCtrl_GetStandardErrorCode;

	stInParam.ulSessionId= ulSessionId;
	stParam.pvIn = (void *)&stInParam;
	stParam.pvOut = (void *)&stOutParam;

	if(SVC_CAS_CtrlGet(eDxCAS_GROUPID_NA, eNaCtrl_GetStandardErrorCode, &stParam) != ERR_OK)
	{
		HxLOG_Error("xmgr_cas_NaGetStandardErrorCode(eNaCtrl_GetStandardErrorCode) error\n");
		return ERR_FAIL;
	}

	*pStandardErrorCode = stOutParam.ulStandardErrorCode;

	return ERR_OK;
}
#endif

#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_ZONEID)
HERROR	xmgr_cas_NaGetZoneId(TUnsignedInt8* pZoneId, TUnsignedInt32 ulMaxLen)
{
	SvcCas_CtrlParam_t stParam;
	NA_GetZoneIdInParam_t stInParam;
	NA_GetZoneIdOutParam_t stOutParam;

	stParam.ulControlType = eNaCtrl_GetZoneId;

	stInParam.ulMaxLen= ulMaxLen;
	stParam.pvIn = (void *)&stInParam;

	stOutParam.pZoneId = pZoneId;
	stParam.pvOut = (void *)&stOutParam;

	if(SVC_CAS_CtrlGet(eDxCAS_GROUPID_NA, eNaCtrl_GetZoneId, &stParam) != ERR_OK)
	{
		HxLOG_Error("xmgr_cas_NaGetStandardErrorCode(eNaCtrl_GetZoneId) error\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR	xmgr_cas_NaGetCountryCode(TUnsignedInt8* pCountryCode, TUnsignedInt32 ulMaxLen)
{
	SvcCas_CtrlParam_t stParam;
	NA_GetCountryCodeInParam_t stInParam;
	NA_GetCountryCodeOutParam_t stOutParam;

	stParam.ulControlType = eNaCtrl_GetCountryCode;

	stInParam.ulMaxLen= ulMaxLen;
	stParam.pvIn = (void *)&stInParam;

	stOutParam.pCountryCode = pCountryCode;
	stParam.pvOut = (void *)&stOutParam;

	if(SVC_CAS_CtrlGet(eDxCAS_GROUPID_NA, eNaCtrl_GetCountryCode, &stParam) != ERR_OK)
	{
		HxLOG_Error("xmgr_cas_NaGetStandardErrorCode(eNaCtrl_GetCountryCode) error\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}
#endif


