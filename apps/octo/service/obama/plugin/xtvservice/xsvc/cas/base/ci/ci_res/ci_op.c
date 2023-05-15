/***************************************************************
* $Workfile:   ci_op.c  $
* $Modtime:
* Auther : kkkim / hmkim
*
* Desc : Operator Profile (CI+ v1.3)
*
* Copyright (c) 2011 by Humax Co., Ltd.
* All right reserved
****************************************************************/

/***************************************************************
* #ifdef statements and include
***************************************************************/
#include <pal_ci.h>
#include <cb_list.h>
#include "ci_plus.h"
#include "ci_op.h"

/***************************************************************
* local definition
***************************************************************/

typedef struct
{
	unsigned char	ucInfoVer;
	unsigned char	ucNitVer;
	unsigned char	ucProfileType;
	unsigned char	ucInitialized;
	unsigned char	ucEntitleChange;
	unsigned char	ucEntitleValid;
	unsigned char	ucRefreshReq;
	unsigned char	ucError;
	unsigned char	ucDeliverySysHint;
	UNIXTIME		utRefreshReqTime;
}CI_OP_STATUS;

typedef struct
{
	unsigned char	ucInfoValid;
	unsigned char	ucInfoVer;
	unsigned short	usCiCamOnId;
	unsigned int	unCiCamIdentified;
	unsigned char	ucCharacterCodeTbl;
	unsigned char	ucEncodingTypeId;
	unsigned char	ucSecondByteValue;
	unsigned char	ucThirdByteValue;
	unsigned char	ucSdtRunStatusTrust;
	unsigned char	ucEitRunStatusTrust;
	unsigned char	ucEitPresentFollowUse;
	unsigned char	ucEitScheduleUse;
	unsigned char	ucExtendedEventUse;
	unsigned char	ucSdtOtherTrust;
	unsigned char	ucEitEventTrigger;
	unsigned char	aucIso639LangCode[3];
	unsigned char	ucProfileNameLen;
	unsigned char	szProfileName[256];	// NULL terminate
}CI_OP_OPERATOR_INFO;

typedef struct
{
	unsigned char	ucUnattended;
	unsigned char	ucSvcTypeLen;
	unsigned char	aucSvcType[256];	// 7 bit를 사용하나 apdu 보낼 때 bit 갯수만큼 자를 예정.
	unsigned char	ucDeliveryCapaLen;
	unsigned char	aucDeliveryCapa[256];
	unsigned char	ucAppCapaLen;
	unsigned char	aucAppCapa[256];
}CI_OP_SEARCH_START;

typedef enum
{
	CI_OP_PROFILE_TYPE_NOT_SUPPORT = 0,
	CI_OP_PROFILE_TYPE_SUPPORT
} CI_OP_PROFILE_TYPE;

/*******************/
/* CC task message */
/*******************/
typedef struct {
	unsigned short		usSsNo;
	unsigned long		ulApduTag;
	unsigned char		*pucApduData;
	unsigned long		ulApduLen;
} CI_OP_TASK_MSG;

typedef enum {
	eCiOpSearchStatus_None = 0,
	eCiOpSearchStatus_Using
} CI_OP_SearchStatus_t;


/***************************************************************
* typedef
***************************************************************/
typedef struct
{
	unsigned short		usSsNo;
	CI_SLOT_ID			usSlotId;
	CI_OP_STATUS		stOpStatus;
	CI_OP_OPERATOR_INFO	stOpInfo;
	CI_OP_SearchStatus_t	eSearchStatus;
	HBOOL					bErrOccured;			// 최종 NIT를 받은 후 이 BOOL 값이 올라가 있으면 channel list를 만들어주지 않도록 상태 확인을 위함.
}CI_OP_INSTANCE;


/***************************************************************
* external variables and structures
***************************************************************/
extern CALLBACK_HANDLE	g_hCbNotifyOperatorPrepareReq;
extern CALLBACK_HANDLE	g_hCbNotifyOperatorTune;
extern CALLBACK_HANDLE	g_hCbNotifyOperatorNit;

/***************************************************************
* static variables and structures
***************************************************************/
static CI_OP_INSTANCE		s_astCiOp[MAX_CI_OP_SS];
// CC message queue id and task id
static unsigned long	s_ulCiOpMsgQId;
static unsigned long	s_ulCiOpTaskId;


/***************************************************************
* static function prototypes
***************************************************************/
static CI_NOTIFY_OPERATOR_PREPARE_REQ_FUNC	s_pfnNotifyOpTuneReady;
static CI_NOTIFY_OPERATOR_TUNE_FUNC	s_pfnNotifyOpTuneInfo;
static CI_NOTIFY_OPERATOR_NIT_FUNC	s_pfnNotifyOpCiCamNit;
static HBOOL	s_bWebReadyStatus = FALSE;		// HMS에서는 OP resource가 Init 된 후에 한참 있다가 web이 동작되므로, bool 값을 두어 web init 여부 확인 후 동작하도록 함.

#if defined(CONFIG_DEBUG)
static void CiOp_PrintOperatorStatus(CI_OP_STATUS *pstOpStatus);
static void CiOp_PrintOperatorInfo(CI_OP_OPERATOR_INFO *pstOpInfo);
#endif

int CiOp_SendOperatorInfoReq(unsigned short usSsNo);
int CiOp_SendOperatorNitReq(unsigned short usSsNo);
int CiOp_SendOperatorEntitlementAck(unsigned short usSsNo);
int CiOp_Proc(unsigned short usSsNo, unsigned long ulApduTag, unsigned char *pucApduData, unsigned long ulApduLen);
static CI_OP_INSTANCE *CiOp_GetInstBySsNo(unsigned short usSsNo);


static int CiOp_GetNitVersion(CI_SLOT_ID usSlotId, unsigned char *pucNitVer)
{
	CI_AUTH_CTX stCiAuthCtx;

	if(CI_GetAuthContextBySlotId(usSlotId, &stCiAuthCtx)!= ERR_OK)
		return ERR_FAIL;

	*pucNitVer = stCiAuthCtx.ucNitVer;
	return ERR_OK;
}

static int CiOp_GetInfoVersion(CI_SLOT_ID usSlotId, unsigned char *pucInfoVer)
{
	CI_AUTH_CTX stCiAuthCtx;

	if(CI_GetAuthContextBySlotId(usSlotId, &stCiAuthCtx)!= ERR_OK)
		return ERR_FAIL;

	*pucInfoVer = stCiAuthCtx.ucInfoVer;
	return ERR_OK;
}

static unsigned char CiOp_GetServiceTypeCapability(unsigned char *pucServiceType)
{
	unsigned char ucSize = 0;

	pucServiceType[ucSize++] = 0x01;
	pucServiceType[ucSize++] = 0x02;

#if defined(CONFIG_MW_TELETEXT)
	pucServiceType[ucSize++] = 0x03;
#endif
#if defined(CONFIG_DB_SVC_TYPE_HD)
#endif
#if defined(CONFIG_MW_SUBTITLE)
#endif

	return ucSize;
}

static unsigned char CiOp_GetDeliveryCapability(unsigned char *pucDeliveryCapa)
{
	unsigned char ucSize = 0;

#if defined(CONFIG_MW_CH_SATELLITE)
	pucDeliveryCapa[ucSize++] = 0x43;
#endif
#if defined(CONFIG_MW_CH_DVB_S2)
	pucDeliveryCapa[ucSize++] = 0x79;
#endif

	return ucSize;
}

static unsigned char CiOp_GetApplicationCapability(unsigned char *pucAppCapa)
{
	unsigned char ucSize = 0;

#if defined(CONFIG_MW_DOWNLOAD_HBBTV)
	pucAppCapa[ucSize++] = 0x01;
	pucAppCapa[ucSize++] = 0x23;
#endif

	return ucSize;
}

static CI_OP_INSTANCE *CiOp_GetNewInstance(void)
{
	CI_OP_INSTANCE *hCiOp = NULL;
	int i;

	for(i = 0 ; i < MAX_CI_OP_SS ; i++)
	{
		if(s_astCiOp[i].usSsNo == 0)
		{
			hCiOp = &s_astCiOp[i];
			break;
		}
	}

	return hCiOp;
}

static CI_OP_INSTANCE *CiOp_GetInstBySsNo(unsigned short usSsNo)
{
	CI_OP_INSTANCE *hCiOp = NULL;
	int i;

	for(i = 0 ; i < MAX_CI_OP_SS ; i++)
	{
		if(usSsNo && s_astCiOp[i].usSsNo == usSsNo)
		{
			hCiOp = &s_astCiOp[i];
			break;
		}
	}

	return hCiOp;
}

static CI_OP_INSTANCE *CiOp_GetInstBySlotId(CI_SLOT_ID usSlotId)
{
	CI_OP_INSTANCE *hCiOp = NULL;
	int i;

	for(i = 0 ; i < MAX_CI_OP_SS ; i++)
	{
		if(s_astCiOp[i].usSsNo && s_astCiOp[i].usSlotId == usSlotId)
		{
			hCiOp = &s_astCiOp[i];
			break;
		}
	}

	return hCiOp;
}

int CiOp_GetOperatorReservedTime(HUINT16 usSlotId, UNIXTIME *putTime)
{
	CI_OP_INSTANCE *hCiOp = NULL;

	hCiOp = CiOp_GetInstBySlotId(usSlotId);

	*putTime = hCiOp->stOpStatus.utRefreshReqTime;

	return ERR_OK;
}

static int CiOp_OperatorStatus(unsigned short usSsNo, unsigned char *pucApduData, unsigned long ulApduLen)
{
	CI_OP_INSTANCE *hCiOp;
	HUINT16			usRsvDate;
	HUINT8			ucRsvTime;

	if(pucApduData == NULL || ulApduLen == 0)
		return ERR_FAIL;

	hCiOp = CiOp_GetInstBySsNo(usSsNo);
	if(hCiOp == NULL)
	{
		HxLOG_Error("CiOp_GetInstBySsNo() NULL!!!\n");
		return ERR_FAIL;
	}

	HxSTD_memset(&hCiOp->stOpStatus, 0, sizeof(CI_OP_STATUS));

	hCiOp->stOpStatus.ucInfoVer				= (pucApduData[0] >> 5) & 0x07;
	hCiOp->stOpStatus.ucNitVer				= pucApduData[0] & 0x1f;
	hCiOp->stOpStatus.ucProfileType			= (pucApduData[1] >> 6) & 0x03;
	hCiOp->stOpStatus.ucInitialized			= (pucApduData[1] >> 5) & 0x01;
	hCiOp->stOpStatus.ucEntitleChange		= (pucApduData[1] >> 4) & 0x01;
	hCiOp->stOpStatus.ucEntitleValid		= (pucApduData[1] >> 3) & 0x01;
	hCiOp->stOpStatus.ucRefreshReq			= pucApduData[1] & 0x03;
	hCiOp->stOpStatus.ucError				= (pucApduData[2] >> 4) & 0x0f;
	hCiOp->stOpStatus.ucDeliverySysHint		= pucApduData[2] & 0x0f;
	usRsvDate								= (pucApduData[3] << 8) | pucApduData[4];
	ucRsvTime								= pucApduData[5];

	if (hCiOp->stOpStatus.ucError != 0 || hCiOp->stOpStatus.ucDeliverySysHint == 0x1 || hCiOp->stOpStatus.ucDeliverySysHint == 0x4)
	{
		HxLOG_Info("[CiOp_OperatorStatus] >> error_flag = %d, ucDeliverySysHint = 0x%x\n", hCiOp->stOpStatus.ucError, hCiOp->stOpStatus.ucDeliverySysHint);
		HxLOG_Info("[CiOp_OperatorStatus] hCiOp->bErrOccured [%d] -> channel list 구성 금지...due to unsupported delivery system..\n", hCiOp->bErrOccured);
		hCiOp->bErrOccured = TRUE;
	}
	else
	{
		hCiOp->bErrOccured = FALSE;		// FALSE로 바꿔주는 것은 여기 한곳에서만 한다. 이유는 어떤 동작이 일어나든 항상 Operator Status로 시작되기 때문. 혹시 문제 있으면 바꿀 것!
	}

	if(usRsvDate)
	{
		HxDATETIME_t		stDateTime;

		HLIB_DATETIME_ConvertMJDToYMD(usRsvDate, &stDateTime.stDate);
		stDateTime.stTime.ucHour = ucRsvTime * 6 / 60;
		stDateTime.stTime.ucMinute = (ucRsvTime * 6) % 60;
		stDateTime.stTime.ucSecond = 0;
		HLIB_DATETIME_ConvertDateTimeToUnixTime(&stDateTime, &hCiOp->stOpStatus.utRefreshReqTime);
	}
	else
	{
		hCiOp->stOpStatus.utRefreshReqTime = 0;
	}

#if defined(CONFIG_DEBUG)
	CiOp_PrintOperatorStatus(&hCiOp->stOpStatus);
#endif
// CI_OP_PROFILE_TYPE_NOT_SUPPORT 일 경우 Operator Info를 받았을 때 실제 다음 동작으로 하지 않도록 처리하기 때문에 이 부분에서는 주석 처리 함. 필요하다면 열어도 됨.
/*
	if((CI_OP_PROFILE_TYPE)hCiOp->stOpStatus.ucProfileType == CI_OP_PROFILE_TYPE_NOT_SUPPORT)
	{
		HxLOG_Info("The CICAM does not support any profiles and descrambles elementary streams as per DVB CI.\n");
		return ERR_OK;
	}
*/
	if(hCiOp->stOpStatus.ucInitialized == 0)
	{
		return CiOp_SendOperatorInfoReq(usSsNo);
	}
	else
	{
		// initialized 되어 있다면...
		// 1. NIT VER 비교
		// 2. Refresh 비교
		//

		unsigned char ucNitVer = 0;
		unsigned char ucInfoVer = 0;

		if(CiOp_GetNitVersion(hCiOp->usSlotId, &ucNitVer)!=ERR_OK)
		{
			HxLOG_Critical("CiOp_GetNitVersion() ERROR!! check it!!\n");
		}

		if(CiOp_GetInfoVersion(hCiOp->usSlotId, &ucInfoVer)!=ERR_OK)
		{
			HxLOG_Critical("CiOp_GetInfoVersion() ERROR!! check it!!\n");
		}

		HxLOG_Info("ucRefreshReq:[%02x]\n", hCiOp->stOpStatus.ucRefreshReq);
		HxLOG_Info("hCiOp->stOpStatus.ucInfoVer:[%02x], ucInfoVer:[%02x]\n", hCiOp->stOpStatus.ucInfoVer, ucInfoVer);
		HxLOG_Info("hCiOp->stOpStatus.ucInfoVer:[%02x], ucInfoVer:[%02x]\n", hCiOp->stOpStatus.ucNitVer, ucNitVer);

		if(hCiOp->stOpStatus.ucRefreshReq
			|| (hCiOp->stOpStatus.ucInfoVer && (hCiOp->stOpStatus.ucInfoVer == ucInfoVer + 1))	// 이전에 저장되어 있는 Info ver 보다 현재 받은 ver 이 1 update 되었을 경우
			|| (hCiOp->stOpStatus.ucNitVer && (hCiOp->stOpStatus.ucNitVer == ucNitVer + 1)))	// 이전에 저장되어 있는 NIT ver 보다 현재 받은 ver 이 1 update 되었을 경우
		{
			return CiOp_SendOperatorInfoReq(usSsNo);
		}
		else
		{
			;
		}
	}

	return ERR_OK;
}

static int CiOp_OperatorInfoReply(unsigned short usSsNo, unsigned char *pucApduData, unsigned long ulApduLen)
{
	unsigned long			unOffset;
	CI_OP_INSTANCE			*hCiOp;
	CI_OP_REFRESH_REQUEST	eOpRefreshReq = 0;

	HxLOG_Info("[CIOP]---> CiOp_OperatorInfoReply() called...\n");

	hCiOp = CiOp_GetInstBySsNo(usSsNo);
	if(hCiOp == NULL)
	{
		HxLOG_Error("[CIOP]---> CiOp_OperatorInfoReply()---> Error : CiOp_GetInstBySsNo(SsNo[%d]) !!!\n", usSsNo);
		return ERR_FAIL;
	}

	if(pucApduData == NULL || ulApduLen == 0)
	{
		HxLOG_Error("[CIOP]---> CiOp_OperatorInfoReply()---> Error : Invalid params!!!\n");
		return ERR_FAIL;
	}

	if((CI_OP_PROFILE_TYPE)hCiOp->stOpStatus.ucProfileType == CI_OP_PROFILE_TYPE_NOT_SUPPORT)
	{
		HxLOG_Info("The CICAM does not support any profiles and descrambles elementary streams as per DVB CI.\n");
#if !defined(CONFIG_MW_CH_CABLE)	// SPEC에 따르면 해당 값이 0이더라도, Cable 지원 모델일 때는 사용 될 수 있다고 정의 됨. Cable 모델에서 확인 필요.
		HxLOG_Warning("[CIOP]---> CiOp_OperatorInfoReply()---> Not support cable network!!!\n");
		return ERR_OK;
#endif
	}

	HxSTD_memset(&hCiOp->stOpInfo, 0, sizeof(CI_OP_OPERATOR_INFO));
	unOffset = 0;

	hCiOp->stOpInfo.ucInfoValid	= (pucApduData[unOffset] >> 3) & 0x01;
	hCiOp->stOpInfo.ucInfoVer	= pucApduData[unOffset] & 0x03;
	unOffset++;

	if(hCiOp->stOpInfo.ucInfoValid == 0)
	{
		HxLOG_Warning("[CIOP]---> CiOp_OperatorInfoReply()---> !!!\n");
		return ERR_OK;
	}

	if(hCiOp->stOpStatus.ucInfoVer != hCiOp->stOpInfo.ucInfoVer)
	{
		HxLOG_Info("[CIOP]---> CiOp_OperatorInfoReply()---> Invalid info version (%d, %d)!!!\n", hCiOp->stOpStatus.ucInfoVer, hCiOp->stOpInfo.ucInfoVer);
		return ERR_FAIL;
	}

	hCiOp->stOpInfo.usCiCamOnId				= (pucApduData[unOffset] << 8) | pucApduData[unOffset+1];
	unOffset += 2;

	hCiOp->stOpInfo.unCiCamIdentified		= (pucApduData[unOffset] << 24) | (pucApduData[unOffset+1] << 16) | (pucApduData[unOffset+2] << 8) | pucApduData[unOffset+3];
	unOffset += 4;

	hCiOp->stOpInfo.ucCharacterCodeTbl		= pucApduData[unOffset++];
	if(hCiOp->stOpInfo.ucCharacterCodeTbl == 0x1f)
	{
		hCiOp->stOpInfo.ucEncodingTypeId	= pucApduData[unOffset++];
	}
	else if(hCiOp->stOpInfo.ucCharacterCodeTbl == 0x10)
	{
		hCiOp->stOpInfo.ucSecondByteValue	= pucApduData[unOffset++];
		hCiOp->stOpInfo.ucThirdByteValue	= pucApduData[unOffset++];
	}

	hCiOp->stOpInfo.ucSdtRunStatusTrust		= (pucApduData[unOffset] >> 7) & 0x01;
	hCiOp->stOpInfo.ucEitRunStatusTrust		= (pucApduData[unOffset] >> 6) & 0x01;
	hCiOp->stOpInfo.ucEitPresentFollowUse	= (pucApduData[unOffset] >> 4) & 0x03;
	hCiOp->stOpInfo.ucEitScheduleUse		= (pucApduData[unOffset] >> 1) & 0x07;
	hCiOp->stOpInfo.ucExtendedEventUse		= pucApduData[unOffset] & 0x01;
	unOffset++;

	hCiOp->stOpInfo.ucSdtOtherTrust			= (pucApduData[unOffset] >> 7) & 0x01;
	hCiOp->stOpInfo.ucEitEventTrigger		= (pucApduData[unOffset] >> 6) & 0x01;
	unOffset++;

	HxSTD_memcpy(hCiOp->stOpInfo.aucIso639LangCode, &pucApduData[unOffset], 3);
	unOffset += 3;

	hCiOp->stOpInfo.ucProfileNameLen		= pucApduData[unOffset++];
	if(hCiOp->stOpInfo.ucProfileNameLen)
	{
		HxSTD_memcpy(hCiOp->stOpInfo.szProfileName, &pucApduData[unOffset], hCiOp->stOpInfo.ucProfileNameLen);
		hCiOp->stOpInfo.szProfileName[hCiOp->stOpInfo.ucProfileNameLen] = '\0';
		unOffset += hCiOp->stOpInfo.ucProfileNameLen;
	}

#if defined(CONFIG_DEBUG)
	CiOp_PrintOperatorInfo(&hCiOp->stOpInfo);
#endif

	eOpRefreshReq = (CI_OP_REFRESH_REQUEST)hCiOp->stOpStatus.ucRefreshReq;
	HxLOG_Info("eOpRefreshReq: [%02x]\n", eOpRefreshReq);

	if(eOpRefreshReq == CI_OP_REFRESH_REQUEST_NONE)
	{
		if(hCiOp->stOpStatus.ucInitialized == 0)
		{
			if(hCiOp->stOpStatus.ucProfileType == 1)
				eOpRefreshReq = CI_OP_REFRESH_REQUEST_FORCED_TUNE;
			else
				eOpRefreshReq = CI_OP_REFRESH_REQUEST_TUNE;
		}
	}

	// 아래 루틴 확인해보자!!
	if( eOpRefreshReq == CI_OP_REFRESH_REQUEST_NONE)
	{
		unsigned char ucNitVer = 0;
		unsigned char ucInfoVer = 0;

		if(CiOp_GetNitVersion(hCiOp->usSlotId, &ucNitVer)!=ERR_OK)
		{
			HxLOG_Critical("CiOp_GetNitVersion() ERROR!! check it!!\n");
		}

		if(CiOp_GetInfoVersion(hCiOp->usSlotId, &ucInfoVer)!=ERR_OK)
		{
			HxLOG_Critical("CiOp_GetInfoVersion() ERROR!! check it!!\n");
		}

		if((hCiOp->stOpStatus.ucInfoVer && (hCiOp->stOpStatus.ucInfoVer == ucInfoVer + 1))	// 이전에 저장되어 있는 Info ver 보다 현재 받은 ver 이 1 update 되었을 경우
			|| (hCiOp->stOpStatus.ucNitVer && (hCiOp->stOpStatus.ucNitVer == ucNitVer + 1)))	// 이전에 저장되어 있는 NIT ver 보다 현재 받은 ver 이 1 update 되었을 경우
		{
			return CiOp_SendOperatorNitReq(hCiOp->usSsNo);
		}
		else
		{
			HxLOG_Critical("Sequence ERROR!! check it!!\n");
		}
	}

	Callback_Reset(g_hCbNotifyOperatorPrepareReq);
	while ((s_pfnNotifyOpTuneReady = (CI_NOTIFY_OPERATOR_PREPARE_REQ_FUNC)Callback_GetNextFunc(g_hCbNotifyOperatorPrepareReq)) != NULL)
	{
		if ((*s_pfnNotifyOpTuneReady)(hCiOp->usSlotId, eOpRefreshReq, hCiOp->stOpInfo.szProfileName) != ERR_OK)
		{
			HxLOG_Error("[CIOP]---> CiOp_OperatorInfoReply()---> Error : SsNo[%d] !!!\n", usSsNo);
			return ERR_FAIL;
		}
	}

	HxLOG_Info("[CIOP]---> CiOp_OperatorInfoReply()---> SsNo[%d]\n", usSsNo);

	return ERR_OK;
}

static int CiOp_OperatorTune(unsigned short usSsNo, unsigned char *pucApduData, unsigned long ulApduLen)
{
	CI_OP_INSTANCE		*hCiOp;
	CI_SLOT_ID			usSlotId;
	unsigned short		usDesLen = 0;

	HxLOG_Info("[CIOP]---> CiOp_OperatorTune() called...\n");

	hCiOp = CiOp_GetInstBySsNo(usSsNo);
	if(hCiOp == NULL)
	{
		HxLOG_Error("[CIOP]---> CiOp_OperatorTune()---> Error : CiOp_GetInstBySsNo(SsNo[%d]) !!!\n", usSsNo);
		return ERR_FAIL;
	}

	if(pucApduData == NULL || ulApduLen == 0)
	{
		HxLOG_Error("[CIOP]---> CiOp_OperatorTune()---> Error : Invalid params!!!\n");
		return ERR_FAIL;
	}

	if (CIRH_GetSlotIdBySsNo(usSsNo, &usSlotId) < 0)
	{
		HxLOG_Error("[CIOP]---> CiOp_OperatorTune()---> Error : CIRH_GetSlotIdBySsNo(SsNo[%d]) !!!\n", usSsNo);
		return ERR_FAIL;
	}

	usDesLen = ((pucApduData[0] & 0x0f) << 8) | pucApduData[1];
	if(usDesLen == 0)
	{
		HxLOG_Print("\n");
		return ERR_OK;
	}

	Callback_Reset(g_hCbNotifyOperatorTune);
	while ((s_pfnNotifyOpTuneInfo = (CI_NOTIFY_OPERATOR_TUNE_FUNC)Callback_GetNextFunc(g_hCbNotifyOperatorTune)) != NULL)
	{
		if ((*s_pfnNotifyOpTuneInfo)(usSlotId, pucApduData + 2, usDesLen) != ERR_OK)
		{
			HxLOG_Error("[CIOP]---> CiOp_OperatorTune()---> Error : SsNo[%d] - operator tunning failed !!!\n", usSsNo);
			return ERR_FAIL;
		}
	}

	HxLOG_Info("[CIOP]---> CiOp_OperatorTune()---> SsNo[%d]\n", usSsNo);

	return ERR_OK;
}

static int CiOp_OperatorSearchStatus(unsigned short usSsNo, unsigned char *pucApduData, unsigned long ulApduLen)
{
	CI_OP_INSTANCE		*hCiOp;
	HUINT16				usRsvDate;
	HUINT8				ucRsvTime;

	HxLOG_Info("[CIOP]---> CiOp_OperatorSearchStatus() called...\n");

	hCiOp = CiOp_GetInstBySsNo(usSsNo);
	if(hCiOp == NULL)
	{
		HxLOG_Error("[CIOP]---> CiOp_OperatorTune()---> Error : CiOp_GetInstBySsNo(SsNo[%d]) !!!\n", usSsNo);
		return ERR_FAIL;
	}

	if(pucApduData == NULL || ulApduLen == 0)
	{
		HxLOG_Error("[CIOP]---> CiOp_OperatorTune()---> Error : Invalid params!!!\n");
		return ERR_FAIL;
	}
	VK_memset(&hCiOp->stOpStatus, 0, sizeof(CI_OP_STATUS));

	hCiOp->stOpStatus.ucInfoVer				= (pucApduData[0] >> 5) & 0x07;
	hCiOp->stOpStatus.ucNitVer				= pucApduData[0] & 0x1f;
	hCiOp->stOpStatus.ucProfileType			= (pucApduData[1] >> 6) & 0x03;
	hCiOp->stOpStatus.ucInitialized			= (pucApduData[1] >> 5) & 0x01;
	hCiOp->stOpStatus.ucEntitleChange		= (pucApduData[1] >> 4) & 0x01;
	hCiOp->stOpStatus.ucEntitleValid		= (pucApduData[1] >> 3) & 0x01;
	hCiOp->stOpStatus.ucRefreshReq			= pucApduData[1] & 0x03;
	hCiOp->stOpStatus.ucError				= (pucApduData[2] >> 4) & 0x0f;
	hCiOp->stOpStatus.ucDeliverySysHint		= pucApduData[2] & 0x0f;
	usRsvDate								= (pucApduData[3] << 8) | pucApduData[4];
	ucRsvTime								= pucApduData[5];

	if(usRsvDate)
	{
		HxDATETIME_t		stDateTime;

		HLIB_DATETIME_ConvertMJDToYMD(usRsvDate, &stDateTime.stDate);
		stDateTime.stTime.ucHour = ucRsvTime * 6 / 60;
		stDateTime.stTime.ucMinute = (ucRsvTime * 6) % 60;
		stDateTime.stTime.ucSecond = 0;
		HLIB_DATETIME_ConvertDateTimeToUnixTime(&stDateTime, &hCiOp->stOpStatus.utRefreshReqTime);
	}
	else
	{
		hCiOp->stOpStatus.utRefreshReqTime = 0;
	}

	if (hCiOp->stOpStatus.ucError != 0 || hCiOp->stOpStatus.ucDeliverySysHint == 0x1 || hCiOp->stOpStatus.ucDeliverySysHint == 0x4)
	{
		hCiOp->bErrOccured = TRUE;

		HxLOG_Info("[CiOp_OperatorSearchStatus] >> error_flag = %d, ucDeliverySysHint = 0x%x\n", hCiOp->stOpStatus.ucError, hCiOp->stOpStatus.ucDeliverySysHint);
		HxLOG_Info("[CiOp_OperatorSearchStatus] hCiOp->bErrOccured [%d] -> channel list 구성 금지...due to unsupported delivery system..\n", hCiOp->bErrOccured);
	}

#if defined(CONFIG_DEBUG)
	CiOp_PrintOperatorStatus(&hCiOp->stOpStatus);
#endif

	if(CiOp_SendOperatorNitReq(usSsNo) != ERR_OK)
	{
		HxLOG_Error("[CIOP]---> CiOp_OperatorSearchStatus()---> Error : CiOp_OperatorStatus(SsNo[%d]) !!!\n", usSsNo);
		return ERR_FAIL;
	}

	HxLOG_Info("[CIOP]---> CiOp_OperatorSearchStatus()---> SsNo[%d]\n", usSsNo);

	return ERR_OK;
}

static int CiOp_OperatorNitReply(unsigned short usSsNo, unsigned char *pucApduData, unsigned long ulApduLen)
{
	CI_OP_INSTANCE	*hCiOp;
	HUINT16			usCiCamNitLen = 0;
	unsigned char	aucTextCoding[7]; // language table value & language code (0 bit:CharacterCodeTbl, 1:EncodingTypeId, 2:SecondByteValue, 3:ThirdByteValue, 4-6:Iso639LangCode)

	HxLOG_Info("[CIOP]---> CiOp_OperatorNitReply() called...\n");

	hCiOp = CiOp_GetInstBySsNo(usSsNo);
	if(hCiOp == NULL)
	{
		HxLOG_Error("[CIOP]---> CiOp_OperatorTune()---> Error : CiOp_GetInstBySsNo(SsNo[%d]) !!!\n", usSsNo);
		return ERR_FAIL;
	}

	if(pucApduData == NULL || ulApduLen == 0)
	{
		HxLOG_Error("[CIOP]---> CiOp_OperatorTune()---> Error : Invalid params!!!\n");
		return ERR_FAIL;
	}

	usCiCamNitLen = (pucApduData[0] << 8) | pucApduData[1];
	if(usCiCamNitLen == 0)
	{
		HxLOG_Print("\n");
		return ERR_OK;
	}

	HxLOG_Info("CiCamNitLen:[%d]\n", usCiCamNitLen);

	aucTextCoding[0] = hCiOp->stOpInfo.ucCharacterCodeTbl;
	aucTextCoding[1] = hCiOp->stOpInfo.ucEncodingTypeId;
	aucTextCoding[2] = hCiOp->stOpInfo.ucSecondByteValue;
	aucTextCoding[3] = hCiOp->stOpInfo.ucThirdByteValue;
	VK_memcpy(&aucTextCoding[4], hCiOp->stOpInfo.aucIso639LangCode, 3);

	Callback_Reset(g_hCbNotifyOperatorNit);
	while ((s_pfnNotifyOpCiCamNit = (CI_NOTIFY_OPERATOR_NIT_FUNC)Callback_GetNextFunc(g_hCbNotifyOperatorNit)) != NULL)
	{
		if ((*s_pfnNotifyOpCiCamNit)(hCiOp->usSlotId, hCiOp->stOpInfo.szProfileName, aucTextCoding, pucApduData + 2, usCiCamNitLen) != ERR_OK)
		{
			HxLOG_Error("[CIOP]---> CiOp_OperatorNitReply()---> Error : SsNo[%d] - operator tunning failed !!!\n", usSsNo);
			return ERR_FAIL;
		}
	}

	if(hCiOp->stOpStatus.ucEntitleChange == 1)
		CiOp_SendOperatorEntitlementAck(usSsNo);

	if(CI_SetOperatorProfileSupportInfo(hCiOp->usSlotId, hCiOp->stOpStatus.ucNitVer, hCiOp->stOpStatus.ucInfoVer) != ERR_OK)
	{
		// 만약 여기 걸리면, CC resource가 제대로 열리지 않은 상태에서 OP 동작이 일어났을 가능성이 있음.
		HxLOG_Critical("[CIOP]---> CiOp_OperatorNitReply()---> Error : OP Info is not saved at AuthContext!!! check it!!!");
	}

	hCiOp->eSearchStatus = eCiOpSearchStatus_None;

	HxLOG_Info("[CIOP]---> CiOp_OperatorNitReply()---> SsNo[%d]\n", usSsNo);

	return ERR_OK;
}

HBOOL CiOp_CheckSimpleEntitlementChange(CI_SLOT_ID usSlotId)
{
	CI_OP_INSTANCE *hCiOp = NULL;

	hCiOp = CiOp_GetInstBySlotId(usSlotId);

	if(hCiOp->eSearchStatus)
		return FALSE;

	return TRUE;
}

/***************************************************************
* function bodies
***************************************************************/
static void CiOp_Task(void)
{
	CI_OP_TASK_MSG 	msg;
	CI_OP_INSTANCE	*hCiOp;

	while (1)
	{
		if (VK_MSG_Receive(s_ulCiOpMsgQId, &msg, sizeof(CI_OP_TASK_MSG)) != VK_OK)
		{
			HxLOG_Error("Error : fail to get msg !!!\n");
			continue;
		}

		hCiOp = CiOp_GetInstBySsNo(msg.usSsNo);
		if(hCiOp == NULL)
		{
			HxLOG_Error("Error : CiOp_GetInstBySsNo(SsNo[%d]) !!!\n", msg.usSsNo);
			return;
		}

		// wait until CC authentication process complete...
		while (FALSE == CI_CheckCcAuthenticationComplete(hCiOp->usSlotId) || s_bWebReadyStatus == FALSE)
		{
			HxLOG_Info("Box is not ready for Ci+ OP operation... 1000ms wait...\n");
			VK_TASK_Sleep(1000);	// 급한 사항은 아니므로 1초정도 쉬면서 check 해도 충분
			// TODO: CAM이 빠졌을 때 (여기 루틴 타고있는 상태에서...) 체크해서 continue 하도록...
		}

		if (CiOp_Proc(msg.usSsNo, msg.ulApduTag, msg.pucApduData, msg.ulApduLen))
		{
			HxLOG_Error("Error : CiCc_Proc(%d, 0x%lx) !!!\n", msg.usSsNo, msg.ulApduTag);
		}

		if (msg.pucApduData)
			OxCAS_Free(msg.pucApduData);
	}
}

// Web Ready 여부를 위쪽에서 준비가 다 되면 내려와서 setting 해야 하는 것이 맞지만, java script 단이 init완료 여부 판단이 쉽지 않아 일단 timer로 판단하도록 함.
static void CiOp_SetWebReadyStatus(unsigned long ulTimerId, void *pvUserData)
{
	HxLOG_Info("set s_bWebReadyStatus to TRUE...\n");
	s_bWebReadyStatus = TRUE;

	(void)ulTimerId;
	(void)pvUserData;
}

int CiOp_Init(void)
{
	unsigned long ulTimerID = 0;

	HxSTD_memset(&s_astCiOp, 0, sizeof(CI_OP_INSTANCE) * MAX_CI_OP_SS);

	/* System 시간 값을 얻어 온다. */
	VK_TIMER_EventAfter(45000, CiOp_SetWebReadyStatus, NULL, 0, (unsigned long *)&ulTimerID);

	return ERR_OK;
}

int CiOp_SsOpenedProc(unsigned short usSsNo)
{
	CI_OP_INSTANCE	*hCiOp;
	unsigned short	usSlotId;

	HxLOG_Print("[CIOP]---> CiOp_SsOpenedProc()---> SsNo[%d]...\n", usSsNo);

	hCiOp = CiOp_GetNewInstance();
	if(hCiOp == NULL)
	{
		HxLOG_Error("CiOp_GetNewInstance() NULL!!!\n");
		return ERR_FAIL;
	}

	if (CIRH_GetSlotIdBySsNo(usSsNo, &usSlotId) < 0)
	{
		HxLOG_Error("[CIOP]---> CiOp_SsOpenedProc()---> Error : CIRH_GetSlotIdBySsNo(SsNo[%d]) !!!\n", usSsNo);
		return -1;
	}

	hCiOp->usSsNo	= usSsNo;
	hCiOp->usSlotId	= usSlotId;

	if (VK_MSG_Create(PAL_CI_OP_MSG_Q_SIZE, sizeof(CI_OP_TASK_MSG), "qCIOP", &s_ulCiOpMsgQId, VK_SUSPENDTYPE_PRIORITY))
	{
		HxLOG_Error("Error : VK_MSG_Create() !!!\n");
		goto ERROR;
	}

	if (VK_TASK_Create((void*)CiOp_Task, PAL_CI_OP_TASK_PRIORITY, PAL_CI_OP_TASK_STACK_SIZE, "tCIOP", NULL, &s_ulCiOpTaskId, 0))
	{
		HxLOG_Error("Error : tCIOP VK_TASK_Create() !!!\n");
		goto ERROR;
	}

	if (VK_TASK_Start(s_ulCiOpTaskId))
	{
		HxLOG_Error("Error : tCIOP VK_TASK_Start() !!!\n");
		goto ERROR;
	}

	return ERR_OK;

	ERROR:
	HxLOG_Error("ERROR:: Create MSG or TASK ERROR...\n");

	if (s_ulCiOpTaskId != 0)
	{
		VK_TASK_Stop(s_ulCiOpTaskId);
		VK_TASK_Destroy(s_ulCiOpTaskId);
		s_ulCiOpTaskId = 0;
		VK_TASK_Sleep(1);
	}

	if (s_ulCiOpMsgQId != 0)
	{
		VK_MSG_Destroy(s_ulCiOpMsgQId);
		s_ulCiOpMsgQId = 0;
	}

	return -1;
}

int CiOp_SsClosedProc(unsigned short usSsNo)
{
	CI_OP_INSTANCE		*hCiOp;

	HxLOG_Print("[CIOP]---> CiOp_SsClosedProc()---> SsNo[%d]...\n", usSsNo);

	hCiOp = CiOp_GetInstBySsNo(usSsNo);
	if(hCiOp == NULL)
	{
		HxLOG_Error("CiOp_GetInstBySsNo() NULL!!!\n");
		return ERR_FAIL;
	}

	HxSTD_memset(hCiOp, 0, sizeof(CI_OP_INSTANCE));

	if (s_ulCiOpTaskId != 0)
	{
		VK_TASK_Stop(s_ulCiOpTaskId);
		VK_TASK_Destroy(s_ulCiOpTaskId);
		s_ulCiOpTaskId = 0;
		VK_TASK_Sleep(1);
	}

	if (s_ulCiOpMsgQId != 0)
	{
		VK_MSG_Destroy(s_ulCiOpMsgQId);
		s_ulCiOpMsgQId = 0;
	}

	return ERR_OK;
}

int CiOp_ApduProc(unsigned short usSsNo, unsigned long ulApduTag, unsigned char *pucApduData, unsigned long ulApduLen)
{
	CI_OP_TASK_MSG msg;

	msg.usSsNo = usSsNo;
	msg.ulApduTag = ulApduTag;
	if (ulApduLen)
	{
		msg.pucApduData = (unsigned char *)OxCAS_Malloc(ulApduLen);
		if(msg.pucApduData != NULL)
		{
			VK_memcpy(msg.pucApduData, pucApduData, ulApduLen);
			msg.ulApduLen = ulApduLen;
		}
		else
		{
			HxLOG_Error("Error : malloc pucApduData fail !!!\n");
			return -1;
		}	
	}
	else
	{
		msg.pucApduData = NULL;
		msg.ulApduLen = 0;
	}

	if (VK_MSG_Send(s_ulCiOpMsgQId, &msg, sizeof(CI_OP_TASK_MSG)))
	{
		HxLOG_Error("Error : fail to send msg !!!\n");
		OxCAS_Free(msg.pucApduData);
		return -1;
	}

	return ERR_OK;
}

int CiOp_Proc(unsigned short usSsNo, unsigned long ulApduTag, unsigned char *pucApduData, unsigned long ulApduLen)
{
	int nRtnCode = ERR_OK;

	switch (ulApduTag)
	{
	case CI_OPERATOR_STATUS:
		HxLOG_Print("CI_OPERATOR_STATUS received (len=%ld)\n", ulApduLen);
		nRtnCode = CiOp_OperatorStatus(usSsNo, pucApduData, ulApduLen);
		break;

	case CI_OPERATOR_NIT_REPLY:
		HxLOG_Print("CI_OPERATOR_NIT_REPLY received (len=%ld)\n", ulApduLen);
		nRtnCode = CiOp_OperatorNitReply(usSsNo, pucApduData, ulApduLen);
		break;

	case CI_OPERATOR_INFO_REPLY:
		HxLOG_Print("CI_OPERATOR_INFO_REPLY received (len=%ld)\n", ulApduLen);
		nRtnCode = CiOp_OperatorInfoReply(usSsNo, pucApduData, ulApduLen);
		break;

	case CI_OPERATOR_SEARCH_STATUS:
		HxLOG_Print("CI_OPERATOR_SEARCH_STATUS received (len=%ld)\n", ulApduLen);
		nRtnCode = CiOp_OperatorSearchStatus(usSsNo, pucApduData, ulApduLen);
		break;

	case CI_OPERATOR_TUNE:
		HxLOG_Print("CI_OPERATOR_TUNE received (len=%ld)\n", ulApduLen);
		nRtnCode = CiOp_OperatorTune(usSsNo, pucApduData, ulApduLen);
		break;

	default:
		nRtnCode = -1;
		HxLOG_Error("[CIOP]---> CiOp_ApduProc()---> Error : Unknown Apdu Tag[0x%lx] !!!\n", ulApduTag);
	}

	HxLOG_Print("[CIOP]---> CiOp_ApduProc()---> SsNo[%d], tag[0x%lx], len[%ld]---> %d\n", usSsNo, ulApduTag, ulApduLen, nRtnCode);

	return nRtnCode;
}

int CiOp_SendOperatorInfoReq(unsigned short usSsNo)
{
	unsigned char aucMsg[4];

	HxLOG_Info("[CIOP]---> CiOp_SendOperatorInfoReq() called...\n");

	aucMsg[0] = (unsigned char)((CI_OPERATOR_INFO_REQ >> 16) & 0xff);
	aucMsg[1] = (unsigned char)((CI_OPERATOR_INFO_REQ >> 8) & 0xff);
	aucMsg[2] = (unsigned char)(CI_OPERATOR_INFO_REQ & 0xff);
	aucMsg[3] = 0x00;

	// TODO: Operator Profile을 지원하는 CAM 인지 CI_OP_RES_V1_ID res 가 열렸는지 확인 후 SendApdu 하도록 변경
	// 일단 아직 CI+ 1.3을 사용하지 않는다는 가정하여 Assert 시키는데, 만약 여기를 타면 근본적으로 불르는 부분 확인하여 수정 해야 함.
	HxLOG_Assert(FALSE);

	if (CIRH_SendApdu(usSsNo, aucMsg, 4) != ERR_OK)
	{
		HxLOG_Error("[CIOP]---> CiOp_SendOperatorInfoReq()---> Error : fail to send CI_OPERATOR_INFO_REQ for SsNo[%d] !!!\n", usSsNo);
		return -1;
	}

	HxLOG_Info("[CIOP]---> CiOp_SendOperatorInfoReq()---> SsNo[%d]\n", usSsNo);

	return ERR_OK;
}

int CiOp_SendOperatorSearchStart(CI_SLOT_ID usSlotId)
{
	unsigned char		*pucMsg = NULL;
	unsigned long		ulMsgPayloadLen, ulOffset = 0;
	unsigned char		ucLenFieldSize;
	CI_OP_SEARCH_START	stSearchParam;
	CI_OP_INSTANCE		*hCiOp;

	HxLOG_Info("[CIOP]---> CiOp_SendOperatorSearchStart() called...\n");

	hCiOp = CiOp_GetInstBySlotId(usSlotId);
	if(hCiOp == NULL)
	{
		HxLOG_Error("[CIOP]---> CiOp_SendOperatorSearchStart()---> Error : CiOp_GetInstBySlotId(SlotId[%d]) !!!\n", usSlotId);
		return ERR_FAIL;
	}

	stSearchParam.ucUnattended		= 0x1;
	stSearchParam.ucSvcTypeLen		= CiOp_GetServiceTypeCapability(stSearchParam.aucSvcType);
	stSearchParam.ucDeliveryCapaLen	= CiOp_GetDeliveryCapability(stSearchParam.aucDeliveryCapa);
	stSearchParam.ucAppCapaLen		= CiOp_GetApplicationCapability(stSearchParam.aucAppCapa);

	ulMsgPayloadLen = 3 + stSearchParam.ucSvcTypeLen + stSearchParam.ucDeliveryCapaLen + stSearchParam.ucAppCapaLen;

	pucMsg = (unsigned char *)OxCAS_Malloc(8 + ulMsgPayloadLen);
	if(pucMsg == NULL)
	{
		HxLOG_Error("[CIOP]---> CiOp_SendOperatorSearchStart()---> Error : fail to send CI_OPERATOR_SEARCH_START for SsNo[%d] !!!\n", hCiOp->usSsNo);
		return -1;
	}

	pucMsg[0] = (unsigned char)((CI_OPERATOR_SEARCH_START >> 16) & 0xff);
	pucMsg[1] = (unsigned char)((CI_OPERATOR_SEARCH_START >> 8) & 0xff);
	pucMsg[2] = (unsigned char)(CI_OPERATOR_SEARCH_START & 0xff);

	CIRH_ConvertApduLengthField(ulMsgPayloadLen, &pucMsg[3], &ucLenFieldSize);

	ulOffset = 3 + ucLenFieldSize;

	pucMsg[ulOffset] = (stSearchParam.ucUnattended & 0x01) << 7;
	pucMsg[ulOffset] = stSearchParam.ucSvcTypeLen & 0x7f;
	if(stSearchParam.ucSvcTypeLen)
		HxSTD_memcpy(&pucMsg[ulOffset+1], stSearchParam.aucSvcType, stSearchParam.ucSvcTypeLen & 0x7f);
	ulOffset += stSearchParam.ucSvcTypeLen + 1;

	pucMsg[ulOffset] = stSearchParam.ucDeliveryCapaLen;
	if(stSearchParam.ucDeliveryCapaLen)
		HxSTD_memcpy(&pucMsg[ulOffset+1], stSearchParam.aucDeliveryCapa, stSearchParam.ucDeliveryCapaLen);
	ulOffset += stSearchParam.ucDeliveryCapaLen + 1;

	pucMsg[ulOffset] = stSearchParam.ucAppCapaLen;
	if(stSearchParam.ucAppCapaLen)
		HxSTD_memcpy(&pucMsg[ulOffset+1], stSearchParam.aucAppCapa, stSearchParam.ucAppCapaLen);
	ulOffset += stSearchParam.ucAppCapaLen + 1;

	// TODO: Operator Profile을 지원하는 CAM 인지 CI_OP_RES_V1_ID res 가 열렸는지 확인 후 SendApdu 하도록 변경
	// 일단 아직 CI+ 1.3을 사용하지 않는다는 가정하여 Assert 시키는데, 만약 여기를 타면 근본적으로 불르는 부분 확인하여 수정 해야 함.
	HxLOG_Assert(FALSE);

	if (CIRH_SendApdu(hCiOp->usSsNo, pucMsg, 3 + ucLenFieldSize + ulMsgPayloadLen) != ERR_OK)
	{
		HxLOG_Error("[CIOP]---> CiOp_SendOperatorSearchStart()---> Error : fail to send CI_OPERATOR_SEARCH_START for SsNo[%d] !!!\n", hCiOp->usSsNo);
		OxCAS_Free(pucMsg);
		return -1;
	}

	OxCAS_Free(pucMsg);

	hCiOp->eSearchStatus = eCiOpSearchStatus_Using;
	HxLOG_Info("[CIOP]---> CiOp_SendOperatorSearchStart()---> SsNo[%d]\n", hCiOp->usSsNo);

	return ERR_OK;
}

int CiOp_SendOperatorSearchCancel(unsigned short usSsNo)
{
	unsigned char aucMsg[4];

	HxLOG_Info("[CIOP]---> CiOp_SendOperatorSearchCancel() called...\n");

	aucMsg[0] = (unsigned char)((CI_OPERATOR_SEARCH_CANCEL >> 16) & 0xff);
	aucMsg[1] = (unsigned char)((CI_OPERATOR_SEARCH_CANCEL >> 8) & 0xff);
	aucMsg[2] = (unsigned char)(CI_OPERATOR_SEARCH_CANCEL & 0xff);
	aucMsg[3] = 0x00;

	// TODO: Operator Profile을 지원하는 CAM 인지 CI_OP_RES_V1_ID res 가 열렸는지 확인 후 SendApdu 하도록 변경
	// 일단 아직 CI+ 1.3을 사용하지 않는다는 가정하여 Assert 시키는데, 만약 여기를 타면 근본적으로 불르는 부분 확인하여 수정 해야 함.
	HxLOG_Assert(FALSE);

	if (CIRH_SendApdu(usSsNo, aucMsg, 4) != ERR_OK)
	{
		HxLOG_Error("[CIOP]---> CiOp_SendOperatorSearchCancel()---> Error : fail to send CI_OPERATOR_SEARCH_CANCEL for SsNo[%d] !!!\n", usSsNo);
		return -1;
	}

	HxLOG_Info("[CIOP]---> CiOp_SendOperatorSearchCancel()---> SsNo[%d]\n", usSsNo);

	return ERR_OK;
}

int CiOp_SendOperatorTuneStatus(CI_SLOT_ID usSlotId, unsigned char ucStrength, unsigned char ucQuality, unsigned char ucStatus, HUINT8 *pucDes, HUINT8 ucDesLen, unsigned char ucNextDesNum)
{
	CI_OP_INSTANCE		*hCiOp;
	unsigned char *pucMsg = NULL;
	unsigned long ulMsgPayloadLen, ulOffset = 0;
	unsigned char ucLenFieldSize;
	unsigned short	usDescLen;

	HxLOG_Info("[CIOP]---> CiOp_SendOperatorTuneStatus() called...\n");

	hCiOp = CiOp_GetInstBySlotId(usSlotId);
	if(hCiOp == NULL)
	{
		HxLOG_Error("[CIOP]---> CiOp_SendOperatorTuneStatus()---> Error : CiOp_GetInstBySlotId(SlotId[%d]) !!!\n", usSlotId);
		return ERR_FAIL;
	}

	usDescLen = ucDesLen;
	ulMsgPayloadLen = 5 + usDescLen;

	pucMsg = (unsigned char *)OxCAS_Malloc(8 + ulMsgPayloadLen);
	if(pucMsg == NULL)
	{
		HxLOG_Error("[CIOP]---> CiOp_SendOperatorTuneStatus()---> Error : fail to send CI_OPERATOR_TUNE_STATUS for SsNo[%d] !!!\n", hCiOp->usSsNo);
		return -1;
	}

	pucMsg[0] = (unsigned char)((CI_OPERATOR_TUNE_STATUS >> 16) & 0xff);
	pucMsg[1] = (unsigned char)((CI_OPERATOR_TUNE_STATUS >> 8) & 0xff);
	pucMsg[2] = (unsigned char)(CI_OPERATOR_TUNE_STATUS & 0xff);

	CIRH_ConvertApduLengthField(ulMsgPayloadLen, &pucMsg[3], &ucLenFieldSize);

	ulOffset = 3 + ucLenFieldSize;

	pucMsg[ulOffset++] = ucNextDesNum;
	pucMsg[ulOffset++] = ucStrength;
	pucMsg[ulOffset++] = ucQuality;
	pucMsg[ulOffset++] = ((ucStatus << 4) & 0xf0) | ((usDescLen >> 8) & 0x0f);
	pucMsg[ulOffset++] = usDescLen & 0xff;
	if(usDescLen && pucDes)
		HxSTD_memcpy(&pucMsg[ulOffset], pucDes, usDescLen);

	if(ucStatus != 0)
	{
		hCiOp->bErrOccured = TRUE;
	}

	// TODO: Operator Profile을 지원하는 CAM 인지 CI_OP_RES_V1_ID res 가 열렸는지 확인 후 SendApdu 하도록 변경
	// 일단 아직 CI+ 1.3을 사용하지 않는다는 가정하여 Assert 시키는데, 만약 여기를 타면 근본적으로 불르는 부분 확인하여 수정 해야 함.
	HxLOG_Assert(FALSE);

	if (CIRH_SendApdu(hCiOp->usSsNo, pucMsg, 3 + ucLenFieldSize + ulMsgPayloadLen) != ERR_OK)
	{
		HxLOG_Error("[CIOP]---> CiOp_SendOperatorTuneStatus()---> Error : fail to send CI_OPERATOR_TUNE_STATUS for SsNo[%d] !!!\n", hCiOp->usSsNo);
		OxCAS_Free(pucMsg);
		return -1;
	}

	OxCAS_Free(pucMsg);

	HxLOG_Info("[CIOP]---> CiOp_SendOperatorTuneStatus() called...\n");

	return ERR_OK;
}

HBOOL CiOp_GetOperatorProfileErrStatus(CI_SLOT_ID usSlotId)
{
	CI_OP_INSTANCE		*hCiOp;

	hCiOp = CiOp_GetInstBySlotId(usSlotId);
	if(hCiOp == NULL)
	{
		HxLOG_Error("[CIOP]---> CiOp_SendOperatorTuneStatus()---> Error : CiOp_GetInstBySlotId(SlotId[%d]) !!!\n", usSlotId);
		return ERR_FAIL;
	}

	return hCiOp->bErrOccured;
}

int CiOp_SendOperatorNitReq(unsigned short usSsNo)
{
	unsigned char aucMsg[4];

	HxLOG_Info("[CIOP]---> CiOp_SendOperatorNitReq() called...\n");

	aucMsg[0] = (unsigned char)((CI_OPERATOR_NIT_REQ >> 16) & 0xff);
	aucMsg[1] = (unsigned char)((CI_OPERATOR_NIT_REQ >> 8) & 0xff);
	aucMsg[2] = (unsigned char)(CI_OPERATOR_NIT_REQ & 0xff);
	aucMsg[3] = 0x00;

	// TODO: Operator Profile을 지원하는 CAM 인지 CI_OP_RES_V1_ID res 가 열렸는지 확인 후 SendApdu 하도록 변경
	// 일단 아직 CI+ 1.3을 사용하지 않는다는 가정하여 Assert 시키는데, 만약 여기를 타면 근본적으로 불르는 부분 확인하여 수정 해야 함.
	HxLOG_Assert(FALSE);

	if (CIRH_SendApdu(usSsNo, aucMsg, 4) != ERR_OK)
	{
		HxLOG_Error("[CIOP]---> CiOp_SendOperatorNitReq()---> Error : fail to send CI_OPERATOR_NIT_REQ for SsNo[%d] !!!\n", usSsNo);
		return -1;
	}

	HxLOG_Info("[CIOP]---> CiOp_SendOperatorNitReq()---> SsNo[%d]\n", usSsNo);

	return ERR_OK;
}

int CiOp_SendOperatorNitReqBySlotId(CI_SLOT_ID usSlotId)
{
	CI_OP_INSTANCE *hCiOp;

	hCiOp = CiOp_GetInstBySlotId(usSlotId);
	if(hCiOp == NULL)
	{
		return ERR_FAIL;
	}

	return CiOp_SendOperatorNitReq(hCiOp->usSsNo);
}

int CiOp_SendOperatorEntitlementAck(unsigned short usSsNo)
{
	unsigned char aucMsg[4];

	HxLOG_Info("[CIOP]---> CiOp_SendOperatorEntitlementAck() called...\n");

	aucMsg[0] = (unsigned char)((CI_OPERATOR_ENTITLEMENT_ACK >> 16) & 0xff);
	aucMsg[1] = (unsigned char)((CI_OPERATOR_ENTITLEMENT_ACK >> 8) & 0xff);
	aucMsg[2] = (unsigned char)(CI_OPERATOR_ENTITLEMENT_ACK & 0xff);
	aucMsg[3] = 0x00;

	// TODO: Operator Profile을 지원하는 CAM 인지 CI_OP_RES_V1_ID res 가 열렸는지 확인 후 SendApdu 하도록 변경
	// 일단 아직 CI+ 1.3을 사용하지 않는다는 가정하여 Assert 시키는데, 만약 여기를 타면 근본적으로 불르는 부분 확인하여 수정 해야 함.
	HxLOG_Assert(FALSE);

	if (CIRH_SendApdu(usSsNo, aucMsg, 4) != ERR_OK)
	{
		HxLOG_Error("[CIOP]---> CiOp_SendOperatorEntitlementAck()---> Error : fail to send CI_OPERATOR_ENTITLEMENT_ACK for SsNo[%d] !!!\n", usSsNo);
		return -1;
	}

	HxLOG_Info("[CIOP]---> CiOp_SendOperatorEntitlementAck()---> SsNo[%d]\n", usSsNo);

	return ERR_OK;
}

int CiOp_SendOperatorExit(unsigned short usSsNo)
{
	unsigned char aucMsg[4];

	HxLOG_Info("[CIOP]---> CiOp_SendOperatorExit() called...\n");

	aucMsg[0] = (unsigned char)((CI_OPERATOR_EXIT >> 16) & 0xff);
	aucMsg[1] = (unsigned char)((CI_OPERATOR_EXIT >> 8) & 0xff);
	aucMsg[2] = (unsigned char)(CI_OPERATOR_EXIT & 0xff);
	aucMsg[3] = 0x00;

	// TODO: Operator Profile을 지원하는 CAM 인지 CI_OP_RES_V1_ID res 가 열렸는지 확인 후 SendApdu 하도록 변경
	// 일단 아직 CI+ 1.3을 사용하지 않는다는 가정하여 Assert 시키는데, 만약 여기를 타면 근본적으로 불르는 부분 확인하여 수정 해야 함.
	HxLOG_Assert(FALSE);

	if (CIRH_SendApdu(usSsNo, aucMsg, 4) != ERR_OK)
	{
		HxLOG_Error("[CIOP]---> CiOp_SendOperatorExit()---> Error : fail to send CI_OPERATOR_STATUS_REQ for SsNo[%d] !!!\n", usSsNo);
		return -1;
	}

	HxLOG_Info("[CIOP]---> CiOp_SendOperatorExit()---> SsNo[%d]\n", usSsNo);

	return ERR_OK;
}

int CiOp_SendOperatorExitBySlotId(CI_SLOT_ID usSlotId)
{
	CI_OP_INSTANCE *hCiOp;

	hCiOp = CiOp_GetInstBySlotId(usSlotId);
	if(hCiOp == NULL)
	{
		return ERR_FAIL;
	}

	return CiOp_SendOperatorExit(hCiOp->usSsNo);
}

int CiOp_SendOperatorStatusReq(unsigned short usSsNo)
{
	unsigned char aucMsg[4];

	HxLOG_Info("[CIOP]---> CiOp_SendOperatorStatusReq() called...\n");

	aucMsg[0] = (unsigned char)((CI_OPERATOR_STATUS_REQ >> 16) & 0xff);
	aucMsg[1] = (unsigned char)((CI_OPERATOR_STATUS_REQ >> 8) & 0xff);
	aucMsg[2] = (unsigned char)(CI_OPERATOR_STATUS_REQ & 0xff);
	aucMsg[3] = 0x00;

	// TODO: Operator Profile을 지원하는 CAM 인지 CI_OP_RES_V1_ID res 가 열렸는지 확인 후 SendApdu 하도록 변경
	// 일단 아직 CI+ 1.3을 사용하지 않는다는 가정하여 Assert 시키는데, 만약 여기를 타면 근본적으로 불르는 부분 확인하여 수정 해야 함.
	HxLOG_Assert(FALSE);

	if (CIRH_SendApdu(usSsNo, aucMsg, 4) != ERR_OK)
	{
		HxLOG_Error("[CIOP]---> CiOp_SendOperatorStatusReq()---> Error : fail to send CI_OPERATOR_STATUS_REQ for SsNo[%d] !!!\n", usSsNo);
		return -1;
	}

	HxLOG_Info("[CIOP]---> CiOp_SendOperatorStatusReq()---> SsNo[%d]\n", usSsNo);

	return ERR_OK;
}

int CiOp_SendOperatorStatusReqBySlotId(CI_SLOT_ID usSlotId)
{
	CI_OP_INSTANCE *hCiOp;

	hCiOp = CiOp_GetInstBySlotId(usSlotId);
	if(hCiOp == NULL)
	{
		return ERR_FAIL;
	}

	return CiOp_SendOperatorStatusReq(hCiOp->usSsNo);
}

#if defined(CONFIG_DEBUG)
static void CiOp_PrintOperatorStatus(CI_OP_STATUS *pstOpStatus)
{
	HxLOG_Info("info_version = %d\n", pstOpStatus->ucInfoVer);
	HxLOG_Info("nit_version = %d\n", pstOpStatus->ucNitVer);
	HxLOG_Info("profile_type = %d\n", pstOpStatus->ucProfileType);
	HxLOG_Info("initialised_flag = %d\n", pstOpStatus->ucInitialized);
	HxLOG_Info("entitlement_change_flag = %d\n", pstOpStatus->ucEntitleChange);
	HxLOG_Info("entitlement_valid_flag = %d\n", pstOpStatus->ucEntitleValid);
	HxLOG_Info("refresh_request_flag = %d\n", pstOpStatus->ucRefreshReq);
	HxLOG_Info("error_flag = %d\n", pstOpStatus->ucError);
	HxLOG_Info("delivery_system_hint = 0x%x\n", pstOpStatus->ucDeliverySysHint);
	HxLOG_Info("refresh_request_date_time = [%08x]\n", pstOpStatus->utRefreshReqTime);
}

static void CiOp_PrintOperatorInfo(CI_OP_OPERATOR_INFO *pstOpInfo)
{
	HxLOG_Info("info_valid = %d\n", pstOpInfo->ucInfoValid);
	HxLOG_Info("info_version = %d\n", pstOpInfo->ucInfoVer);
	HxLOG_Info("cicam_original_network_id = %d\n", pstOpInfo->usCiCamOnId);
	HxLOG_Info("cicam_identifier = 0x%x\n", pstOpInfo->unCiCamIdentified);
	HxLOG_Info("character_code_table = %d\n", pstOpInfo->ucCharacterCodeTbl);
	HxLOG_Info("encoding_type_id = %d\n", pstOpInfo->ucEncodingTypeId);
	HxLOG_Info("second_byte_value = %d\n", pstOpInfo->ucSecondByteValue);
	HxLOG_Info("third_byte_value = %d\n", pstOpInfo->ucThirdByteValue);
	HxLOG_Info("sdt_running_status_trusted = %d\n", pstOpInfo->ucSdtRunStatusTrust);
	HxLOG_Info("eit_running_status_trusted = %d\n", pstOpInfo->ucEitRunStatusTrust);
	HxLOG_Info("eit_present_following_usage = %d\n", pstOpInfo->ucEitPresentFollowUse);
	HxLOG_Info("eit_schedule_usage = %d\n", pstOpInfo->ucEitScheduleUse);
	HxLOG_Info("extended_event_usage = %d\n", pstOpInfo->ucExtendedEventUse);
	HxLOG_Info("sdt_other_trusted = %d\n", pstOpInfo->ucSdtOtherTrust);
	HxLOG_Info("eit_event_trigger = %d\n", pstOpInfo->ucEitEventTrigger);
	HxLOG_Info("ISO_639_language_code = %c%c%c\n", pstOpInfo->aucIso639LangCode[0], pstOpInfo->aucIso639LangCode[1], pstOpInfo->aucIso639LangCode[2]);
	HxLOG_Info("profile_name_length = %d\n", pstOpInfo->ucProfileNameLen);
	HxLOG_Info("profile_name = %s\n", pstOpInfo->szProfileName);
}
#endif

