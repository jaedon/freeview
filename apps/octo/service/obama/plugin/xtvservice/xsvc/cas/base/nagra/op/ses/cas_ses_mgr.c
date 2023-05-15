/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <db_param.h>
#include <namgr_main.h>
#include "cas_ses_mgr.h"
#include <_svc_cas_mgr_si.h>
#include <_svc_cas_mgr_common.h>
#include <_svc_cas_mgr_nvram.h>
#include <_svc_cas_sub_svc_api.h>
#include <util.h>

#include <pal_pipe.h>

#include <isosvc.h>

#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
extern HERROR MW_CA_NA_GetAccessStatus(TCaAccess *accessStatus);
#endif
extern HERROR MW_CA_NA_GetSizeOfMacCaDesc(TUnsignedInt32 hAction, TUnsignedInt16 usScCaSysId, TUnsignedInt8 *pucLen);
extern HERROR MW_CA_NA_GetPdExpirationDuration(TCaDuration *expireDuraion);

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/





#define		APS_CHECK_BUFFER				10
#define 	NASP_CA_DESC_SIZE				8
#define 	NASP_CA_DESC_H_SIZE				2

enum
{
	NASP_CA_TYPE_PRE_PAID = 0,
	NASP_CA_TYPE_SUBSCRIP,
	NASP_CA_TYPE_MAX
};

enum
{
	MACD_CA_TYPE_PRE_PAID = 0,
	MACD_CA_TYPE_SUBSCRIP,
	MACD_CA_TYPE_MAX
};
// 문서상에 Other right type이 나중에 추가될 수도 있다고 되어 있어 NASP와 분리해서 정의한다.

enum
{
	APS_PERIOD_ONCE = 0,
	APS_PERIOD_DAILY,
	APS_PERIOD_ALWAYS
};


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef enum
{
	eCAS_APS_RET_CHECK = 0,		// user inpue box!
	eCAS_APS_RET_ERROR,			// real error
	eCAS_APS_RET_NOT_EXPIRED,	// not expired!
	eCAS_APS_RET_EXPIRED		// expired!
} eCAS_APS_EntitleRet;

typedef struct
{
	/* those values are used to confirm of user*/
	HUINT8						bCheckWrite;
	HUINT8 						ucProfieType;
	HUINT8 						ucFrequency;
	NA_IRDCMD_IDX_PROFILE_t 	stCurrentProfileSet;
} NaCasApsValidUserConfirm_t;


/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
static HINT32						s_current_buff_index = APS_CHECK_BUFFER;
static NaCasApsCheck_t				s_cas_na_aps_context[APS_CHECK_BUFFER];
static NaCasApsValidUserConfirm_t	s_cas_na_aps_conform_t;
static HUINT8						s_check_new_service = FALSE;

static HxDATETIME_t					*s_pstDateTimeToExpire = NULL;




/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

static NaCasApsValidCheck_t		*xmgr_cas_Naaps_GetValidContext(void)
{
	s_current_buff_index++;
	if (s_current_buff_index > (APS_CHECK_BUFFER - 1))
	{
		s_current_buff_index = 0;
	}

	s_cas_na_aps_context[s_current_buff_index].eCasCheckType = eCAS_APS_CHECK_HDVALID;
	return &(s_cas_na_aps_context[s_current_buff_index].context.stCasApsValidCheck);
}

static NaCasApsCheck_t		*xmgr_cas_Naaps_GetCurrentCheckMessage(void)
{
	return &(s_cas_na_aps_context[s_current_buff_index]);
}

static HERROR				xmgr_cas_NaSetIndexDisplayProfile(NA_IRDCMD_IDX_PROFILE_t *pstCurrentProfile, HUINT8 ucProfieType)
{
	HUINT32					nOffsetFromType = CA_NVRAM_IRDCMD_CUR_PROFILE_OFFESET;
	HERROR					hReturnError = ERR_OK;

	if (pstCurrentProfile == NULL)
	{
		return ERR_FAIL;
	}

	if (ucProfieType == eProfileProducts)
	{
		nOffsetFromType += sizeof(NA_IRDCMD_IDX_PROFILE_t);
	}

	hReturnError = svc_cas_DevPortNVRAMWriteData(eDxCAS_GROUPID_NA, nOffsetFromType,
											sizeof(NA_IRDCMD_IDX_PROFILE_t),
											(HUINT8 *)pstCurrentProfile);
	if (hReturnError != ERR_OK)
	{
		HxLOG_Error("svc_cas_DevPortNVRAMWriteData error[%x]\n", hReturnError);
		return hReturnError;
	}

	return ERR_OK;
}

static HERROR				xmgr_cas_NaGetIndexDisplayProfile(NA_IRDCMD_IDX_PROFILE_t *pstCurrentProfile, HUINT8 ucProfieType)
{
	HUINT32					nOffsetFromType = CA_NVRAM_IRDCMD_CUR_PROFILE_OFFESET;
	HERROR					hReturnError = ERR_OK;

	if (pstCurrentProfile == NULL)
	{
		return ERR_FAIL;
	}

	HxSTD_memset(pstCurrentProfile, 0x00, sizeof(NA_IRDCMD_IDX_PROFILE_t));

	if (ucProfieType == eProfileProducts)
	{
		nOffsetFromType += sizeof(NA_IRDCMD_IDX_PROFILE_t);
	}

	hReturnError = svc_cas_DevPortNVRAMReadData(eDxCAS_GROUPID_NA, nOffsetFromType,
											sizeof(NA_IRDCMD_IDX_PROFILE_t),
											(HUINT8 *)pstCurrentProfile);
	if (hReturnError != ERR_OK)
	{
		HxLOG_Error("svc_cas_DevPortNVRAMReadData error[%x]\n", hReturnError);
		return hReturnError;
	}

	return ERR_OK;
}

static HERROR				xmgr_cas_NaGetDisplayProfile(NA_IRDCMD_PROFILE_t *pstDisplayProfile, HUINT8 ucProfieType)
{
	HUINT32					nOffsetFromType = CA_NVRAM_IRDCMD_PROFILE_OFFESET;
	HERROR					hReturnError = ERR_OK;

	if (pstDisplayProfile == NULL)
	{
		return ERR_FAIL;
	}

	HxSTD_memset(pstDisplayProfile, 0x00, sizeof(NA_IRDCMD_PROFILE_t));

	if (ucProfieType == eProfileProducts)
	{
		nOffsetFromType += sizeof(NA_IRDCMD_PROFILE_t);
	}

	hReturnError = svc_cas_DevPortNVRAMReadData(eDxCAS_GROUPID_NA, nOffsetFromType,
											sizeof(NA_IRDCMD_PROFILE_t),
											(HUINT8 *)pstDisplayProfile);

	HxLOG_Print("* ucProfieType = %d\n", pstDisplayProfile->ucProfieType);
	HxLOG_Print("* ucPeroid = %02x %02x %02x %02x %02x\n", pstDisplayProfile->ucPeroid[0], pstDisplayProfile->ucPeroid[1], pstDisplayProfile->ucPeroid[2], pstDisplayProfile->ucPeroid[3], pstDisplayProfile->ucPeroid[4]);
	HxLOG_Print("* ucBeginOfPeroid = %02x %02x %02x %02x %02x\n", pstDisplayProfile->ucBeginOfPeroid[0], pstDisplayProfile->ucBeginOfPeroid[1], pstDisplayProfile->ucBeginOfPeroid[2], pstDisplayProfile->ucBeginOfPeroid[3], pstDisplayProfile->ucBeginOfPeroid[4]);
	HxLOG_Print("* ucEndOfPeroid = %02x %02x %02x %02x %02x\n", pstDisplayProfile->ucEndOfPeroid[0], pstDisplayProfile->ucEndOfPeroid[1], pstDisplayProfile->ucEndOfPeroid[2], pstDisplayProfile->ucEndOfPeroid[3], pstDisplayProfile->ucEndOfPeroid[4]);
	HxLOG_Print("* ucFrequency = %02x %02x %02x %02x %02x\n", pstDisplayProfile->ucFrequency[0], pstDisplayProfile->ucFrequency[1], pstDisplayProfile->ucFrequency[2], pstDisplayProfile->ucFrequency[3], pstDisplayProfile->ucFrequency[4]);

	if (hReturnError != ERR_OK)
	{
		HxLOG_Error("svc_cas_DevPortNVRAMReadData error[%x]\n", hReturnError);
		return hReturnError;
	}

	return ERR_OK;
}

static HERROR				xmgr_cas_Naaps_UpdateCasFromAP(eCAS_APS_UPDATE_TYPE type, HUINT32 actionId)
{
	Handle_t 				hAction;
	SvcCas_MsgType_e			eMsgType;
	DxCAS_GroupId_e			eCasId;
	void 					*pMessage;
	SvcCas_ShowType_e 			eShowType;
	SvcCas_ChannelType_e 		eChType;
	NaCasApsValidUserConfirm_t *pstSavedProfileSet = NULL;
	SvcCas_Context_t 	*pstCasContext = NULL;

	// Make action_handle from action_id that is getting from app.
	hAction = PAL_PIPE_GetActionHandle(actionId);

	/* get Cas Action Context */
	pstCasContext = svc_cas_MgrGetActionContextByAction(hAction);
	if (hAction == HANDLE_NULL || pstCasContext == NULL)
	{
		HxLOG_Error("Failed get CasActionContext\n");
		return ERR_FAIL;
	}

	if (CasMgrSI_DefineChannelType(pstCasContext, &eChType) == ERR_OK)
	{
		if (eChType != eCasChannel_Scramble)
		{
			// todo : have to check cas types
			HxLOG_Print(" eChType is fta or unnamed\n");
			return ERR_OK;
		}
	}
	else
	{
		HxLOG_Error(" \n");
		return ERR_FAIL;
	}

	HxLOG_Print("[%s:%d] type(%d)\n", __FUNCTION__, __LINE__, type);

	if (type == eCAS_APS_UPDATE_OK)
	{
		pstSavedProfileSet = &(s_cas_na_aps_conform_t);

		HxLOG_Print("[%s:%d] type(%d), write(%d), profileType(0x%x)\n", __FUNCTION__, __LINE__, type, pstSavedProfileSet->bCheckWrite, pstSavedProfileSet->ucProfieType);

		if ((pstSavedProfileSet->bCheckWrite) && (pstSavedProfileSet->ucProfieType != 0xFF))
		{
			xmgr_cas_NaSetIndexDisplayProfile(&pstSavedProfileSet->stCurrentProfileSet,
														pstSavedProfileSet->ucProfieType);
		}

		if ((s_check_new_service == TRUE) && (pstSavedProfileSet->ucFrequency == APS_PERIOD_ALWAYS))
		{
			CASMGR_APS_SetServiceStatus(FALSE);
		}

	}

	if (type == eCAS_APS_UPDATE_OK || type == eCAS_APS_UPDATE_PREV)
	{
		if (NAMGR_GetLastAlarmData(hAction, &eMsgType, &eCasId, &pMessage, &eShowType) != ERR_OK)
		{
			HxLOG_Error("NAMGR_GetLastAlarmData\n");
			return ERR_FAIL;
		}

		HxLOG_Print("eMsgType[%x],eShowType[%x]\n", eMsgType, eShowType);

		if (CASMGR_APS_UpdateCasMessage(hAction, eMsgType, eCasId, pMessage, eShowType) != ERR_OK)
		{
			HxLOG_Error("CASMGR_APS_UpdateCasMessage\n");
			return ERR_FAIL;
		}
	}
	else
	{
		return ERR_FAIL;
	}

	return ERR_OK;
}

static HERROR				xmgr_cas_NaConvertDataTime(TCalendarTime *pstCalendarTime, HxDATETIME_t *pstDataTime)
{
	if ((pstCalendarTime == NULL) || (pstDataTime == NULL))
	{
		return ERR_FAIL;
	}

	pstDataTime->stDate.usYear	= (pstCalendarTime->year + 1900);
	pstDataTime->stDate.ucMonth	= pstCalendarTime->month;
	pstDataTime->stDate.ucDay	= pstCalendarTime->day;
	pstDataTime->stTime.ucHour 	= pstCalendarTime->hour;
	pstDataTime->stTime.ucMinute = pstCalendarTime->minute;
	pstDataTime->stTime.ucSecond = pstCalendarTime->second;
	pstDataTime->stTime.usMillisecond = 0;

	return ERR_OK;
}

static HERROR				xmgr_cas_NaSettingMessage(NaCasApsValidCheck_t *pContext, eCAS_APS_VALID_TYPE eCasApsValidType, eCAS_APS_VALID_MESSAGE eCasApsValidMessage)
{
	// setting message type and message id
	pContext->eCasApsValidType = eCasApsValidType;
	pContext->eCasApsValidMessage = eCasApsValidMessage;

	return ERR_OK;
}

static HERROR				xmgr_cas_NaSetUserConfirmAndSetEEPROM(				HUINT8 bCheckWrite,
																					HUINT8 ucProfieType,
																					HUINT8 ucFrequency,
																					NA_IRDCMD_IDX_PROFILE_t *pstCurrentProfileSet)
{
	NaCasApsValidUserConfirm_t *pstSavedProfileSet = &(s_cas_na_aps_conform_t);

	if (pstCurrentProfileSet == NULL)
	{
		return ERR_FAIL;
	}

	HxSTD_memset(pstSavedProfileSet, 0, sizeof(NaCasApsValidUserConfirm_t));
	pstSavedProfileSet->bCheckWrite = bCheckWrite;
	pstSavedProfileSet->ucFrequency = ucFrequency;

	if (bCheckWrite == TRUE)
	{
		pstSavedProfileSet->ucProfieType = ucProfieType;
		HxSTD_memcpy(&pstSavedProfileSet->stCurrentProfileSet, pstCurrentProfileSet, sizeof(NA_IRDCMD_IDX_PROFILE_t));
	}

	return ERR_OK;
}

static HUINT8				xmgr_cas_NaCheckRightValidityWithinPeriod(	NaCasApsValidCheck_t *pContext,
																						HUINT8 		ucProfieType,
																						HxDATETIME_t	stCurrentDateTime,
																						HxDATETIME_t	*pstExpirationDateTime,
																						NA_IRDCMD_PROFILE_t		*pstDisplayProfile,
																						NA_IRDCMD_IDX_PROFILE_t	*pstCurrentProfileSet,
																						HUINT8		*pucFrequency)
{
	HUINT8					nIdx = 0;
	HUINT8					bIsFind = FALSE;
	HUINT8					bCheckWrite = FALSE;
	HINT32					ulOffsetDay= 0;
	HUINT8					ucFrequency = 0;

	if ((pucFrequency == NULL) || (pstDisplayProfile == NULL) || (pstCurrentProfileSet == NULL))
	{
		return FALSE;
	}

	pContext->bDisplayWarning = TRUE;

	ulOffsetDay = HLIB_DATETIME_CompareDate(pstExpirationDateTime->stDate, stCurrentDateTime.stDate);
	if (ulOffsetDay < 0)
	{
		HxLOG_Error("not search frequency[%d]\n", ulOffsetDay);

		*pucFrequency = 0;
		return bIsFind;
	}

	HxLOG_Print("ucOffsetDay[%d]\n", ulOffsetDay);

	for (nIdx = 0; nIdx < NA_IRDCMD_MAX_PROFILE; nIdx++)
	{
		if ( (pstDisplayProfile->ucEndOfPeroid[nIdx] <= ulOffsetDay) &&
			 (pstDisplayProfile->ucBeginOfPeroid[nIdx] >= ulOffsetDay) )
		{
			bIsFind 	= TRUE;
			ucFrequency	= pstDisplayProfile->ucFrequency[nIdx];

			HxLOG_Error("nIdx(%d) - ucPeroid[%d] ucBeginOfPeroid[%d] ucEndOfPeroid[%d] ucFrequency[%d]\n", nIdx,
															pstDisplayProfile->ucPeroid[nIdx],
															pstDisplayProfile->ucBeginOfPeroid[nIdx],
															pstDisplayProfile->ucEndOfPeroid[nIdx],
															pstDisplayProfile->ucFrequency[nIdx]);

			pContext->bRemainDay = (HUINT8)ulOffsetDay;
			HxSTD_memcpy(&pContext->bExpiredDate, pstExpirationDateTime, sizeof(HxDATETIME_t));

			HxLOG_Error("pstCurrentProfileSet->nCurrentIdx = %d \n", pstCurrentProfileSet->nCurrentIdx);

			if (pstCurrentProfileSet->nCurrentIdx != 0)
			{
				// eeprom에 있는 index와 찾은 index가 다른 경우 무조건 write
				if (pstCurrentProfileSet->nCurrentIdx != pstDisplayProfile->ucPeroid[nIdx])
				{
					bCheckWrite = TRUE;
				}
				else
				{
					if (ucFrequency == APS_PERIOD_ONCE) // once
					{
						pContext->bDisplayWarning = FALSE;
					}
					else if (ucFrequency == APS_PERIOD_DAILY) // daily
					{
						if ((pstCurrentProfileSet->stData.ucDay != stCurrentDateTime.stDate.ucDay) ||
							(pstCurrentProfileSet->stData.ucMonth != stCurrentDateTime.stDate.ucMonth) ||
							(pstCurrentProfileSet->stData.usYear != stCurrentDateTime.stDate.usYear))
						{
							bCheckWrite = TRUE;
						}
						else
						{
							pContext->bDisplayWarning = FALSE;
						}
					}
					else
					{
						if (s_check_new_service == FALSE)
						{
							pContext->bDisplayWarning = FALSE;
						}

						// CONFIG_MW_CAS_NAGRA_OP_SES_CAKTEST - RRT
						if (pstCurrentProfileSet->nCurrentIdx == pstDisplayProfile->ucPeroid[nIdx])
						{
							pContext->bDisplayWarning = TRUE;
							HxLOG_Error("[MJAHN] 4 bDisplayWarning = TRUE \n");
						}

					}
				}
			}
			else
			{
				// 처음 시도한 경우에 current는 0이므로 다시 써야함
				bCheckWrite = TRUE;
			}

			if (bCheckWrite)
			{
				pstCurrentProfileSet->nCurrentIdx = pstDisplayProfile->ucPeroid[nIdx];
				HxSTD_memcpy(&(pstCurrentProfileSet->stData), &(stCurrentDateTime.stDate), sizeof(HxDATETIME_Date_t));

#if defined(CONFIG_MW_CAS_NAGRA_OP_SES_CAKTEST)
				// CONFIG_MW_CAS_NAGRA_OP_SES_CAKTEST - RRT
				xmgr_cas_NaSetIndexDisplayProfile(pstCurrentProfileSet, ucProfieType);
#endif
			}

			xmgr_cas_NaSetUserConfirmAndSetEEPROM(bCheckWrite, ucProfieType, ucFrequency, pstCurrentProfileSet);

			HxLOG_Print("nIdx[%x], pucFrequency[%x]\n", nIdx, ucFrequency);

			break;
		}
	}

	*pucFrequency = ucFrequency;

	return bIsFind;
}


// HD + Updated Smartcard Info : SC Information 화면에 expiration date를 표시하기 위함.
// NULL 이 들어오면 expire date 를 0 으로 clear 함.
static HERROR		local_cas_na_UpdateExpireDate(HxDATETIME_t *pstDateTime)
{
	if (s_pstDateTimeToExpire == NULL)
	{
		// 처음 들어오는 경우 alloc 해 두고 사용.
		s_pstDateTimeToExpire = (HxDATETIME_t *)MW_MEM_Alloc(sizeof(HxDATETIME_t));
	}

	if (s_pstDateTimeToExpire != NULL)
	{
		HxLOG_Print("~~~~~~~~ copy expiration date  \n");
		if(pstDateTime != NULL)
		{
			HxSTD_memcpy(s_pstDateTimeToExpire, pstDateTime, sizeof(HxDATETIME_t));
		}
		else
		{
			HxLOG_Print("Invalid DateTime \n");
		}
	}

	return ERR_OK;
}


static HERROR		local_cas_na_aps_ClearExpireDate(void)
{
	if (s_pstDateTimeToExpire != NULL)
	{
		HxLOG_Print("==> Clear expiration date   \n");
		HxSTD_memset(s_pstDateTimeToExpire, 0, sizeof(HxDATETIME_t));
	}

	return ERR_OK;
}


#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
static HERROR	local_cas_na_aps_Check_EntitlementMessage136(NaCasApsValidCheck_t *pContext, UNIXTIME productExpireUnixTime)
{
		HERROR				nErrorReturn;
		TSystemInfoStatus	xStatus = CA_SYSTEM_INFO_ERROR;
		TIrdInfo		*pxIrdInfo = NULL;
		TUnsignedInt8		xNumberOfSmartcards = 0;
		TSmartcardInfoItem	*pxSmartcardInfoArray = NULL;
		TChar			SC_NewSerialNumber[64];
		TChar			SC_OldSerialNumber[64];
		UNIXTIME		stOldExpireDate = 0;
		HxDATETIME_t		convertOldExpireTime;
		HUINT16			initYear = 1992;

		//HINT32			oldSerialNumberBufferSize =0;
		TCaAccess 		accessStatus;
		HINT32		 		nValue;
		// AS-3101 HD+ Platform System Messages, CA Alarms and Entitlement Messages CAK V7 (Ver1.4)
		// (Right validity within display profiles periods) -> No !!!
		// Pre-paid right prolonged? Yes !!
		// MW_CA_NA_GetSystemInfo call Current status values.
		// We have to make this step by step.

		HxSTD_memset(SC_NewSerialNumber, 0, sizeof(HUINT8)*64);

		if(ERR_OK != MW_CA_NA_GetAccessStatus(&accessStatus))
		{
			// bRightValidityWithinPeriod = false 에 해당하는 else 문에서 별다른 이변이 없는 한
			// 기존의 흐름대로 eCAS_APS_RET_NOT_EXPIRED 를 리턴한다.
			HxLOG_Print("CA_NA_GetAccessStatus Fail\n");
			return ERR_FAIL;
		}
		// 1. Access to service is granted?
		if(accessStatus != CA_ACCESS_GRANTED)
		{
			// bRightValidityWithinPeriod = false 에 해당하는 else 문에서 별다른 이변이 없는 한
			// 기존의 흐름대로 eCAS_APS_RET_NOT_EXPIRED 를 리턴한다.
			HxLOG_Print("ACCESS Error\n");
			return ERR_FAIL;
		}
		// 2. dateOfPrePaidRightExpirationNew!=dateOfPrePaidRightExpirationOld

#if 1 // 작업 진행 필요 by mslee
//		nErrorReturn = DB_SETUP_GetUserItemValue((HINT8*)USERCONFIG_ITEMNAME_OLDPREPAIDRIGHT, (HINT32 *)&stOldExpireDate);
		nErrorReturn = DB_PARAM_GetItem(eDB_PARAM_ITEM_OLDPREPAIDRIGHT, (HUINT32 *)&stOldExpireDate, 0);
		if(nErrorReturn != ERR_OK)
		{
			// bRightValidityWithinPeriod = false 에 해당하는 else 문에서 별다른 이변이 없는 한
			// 기존의 흐름대로 eCAS_APS_RET_NOT_EXPIRED 를 리턴한다.
			HxLOG_Print("Can not found OLD Expiration Date\n");
			return ERR_FAIL;
		}
#endif
		HLIB_DATETIME_ConvertUnixTimeToDateTime(stOldExpireDate, &convertOldExpireTime);
		HxLOG_Print("Old Expiration date : year [%d] M %d d %d\n", convertOldExpireTime.stDate.usYear,
					convertOldExpireTime.stDate.ucMonth,
					convertOldExpireTime.stDate.ucDay);
		if(productExpireUnixTime >= stOldExpireDate)
		{
			// Summer Time 으로 인해 1시간의 변동가능성 존재하여 여유분 두어 체크함....갱신은 보통 1년단위이므로 2시간정도는 상관없을 듯 함...
			if(productExpireUnixTime < stOldExpireDate + 60*60*2)
			{
				// bRightValidityWithinPeriod = false 에 해당하는 else 문에서 별다른 이변이 없는 한
				// 기존의 흐름대로 eCAS_APS_RET_NOT_EXPIRED 를 리턴한다.
#if 1 // 작업 진행 필요 by mslee
				//DB_SETUP_GetUserItemValue ((HINT8*)USERCONFIG_ITEMNAME_EM36_OPEN, &nValue);
				nErrorReturn = DB_PARAM_GetItem(eDB_PARAM_ITEM_EM36_OPEN, (HUINT32 *)&nValue, 0);
				if (nErrorReturn != ERR_OK)
				{
					HxLOG_Print("Can not found eDB_PARAM_ITEM_EM36_OPEN\n");
					return ERR_FAIL;
				}
#endif
				if(nValue == TRUE)
				{
					HxLOG_Print("136 Message Dialog Open State \n");
					HLIB_DATETIME_ConvertUnixTimeToDateTime(productExpireUnixTime, &pContext->bExpiredDate);
					HxLOG_Print("Expiration date : year [%d] M %d d %d\n", pContext->bExpiredDate.stDate.usYear,
								pContext->bExpiredDate.stDate.ucMonth,
								pContext->bExpiredDate.stDate.ucDay);
					xmgr_cas_NaSettingMessage(pContext, eCAS_APS_MSG_T_DIALOG, eCAS_APS_MSG_REF136);
					return ERR_OK;
				}
				HxLOG_Print("Need not renew.\n");
				return ERR_FAIL;
			}
		}
		else
		{
			// Summer Time 으로 인해 1시간의 변동가능성 존재하여 여유분 두어 체크함....갱신은 보통 1년단위이므로 2시간정도는 상관없을 듯 함...
			if(stOldExpireDate < productExpireUnixTime + 60*60*2)
			{
				// bRightValidityWithinPeriod = false 에 해당하는 else 문에서 별다른 이변이 없는 한
				// 기존의 흐름대로 eCAS_APS_RET_NOT_EXPIRED 를 리턴한다.
#if 1 // 작업 진행 필요 by mslee
				//DB_SETUP_GetUserItemValue ((HINT8*)USERCONFIG_ITEMNAME_EM36_OPEN, &nValue);
				DB_PARAM_GetItem(eDB_PARAM_ITEM_EM36_OPEN, (HUINT32 *)&nValue, 0);
#endif
				if(nValue == TRUE)
				{
					HxLOG_Print("136 Message Dialog Open State \n");
					HLIB_DATETIME_ConvertUnixTimeToDateTime(productExpireUnixTime, &pContext->bExpiredDate);
					HxLOG_Print("Expiration date : year [%d] M %d d %d\n", pContext->bExpiredDate.stDate.usYear,
								pContext->bExpiredDate.stDate.ucMonth,
								pContext->bExpiredDate.stDate.ucDay);
					xmgr_cas_NaSettingMessage(pContext, eCAS_APS_MSG_T_DIALOG, eCAS_APS_MSG_REF136);
					return ERR_OK;
				}
				HxLOG_Print("Need not renew.\n");
				return ERR_FAIL;
			}
		}
		// 3. dateOfPrePaidRightExpirationOld!=initialValue

		if(convertOldExpireTime.stDate.usYear <= initYear)
		{
			HxLOG_Print("Old DB Data is smaller than 1992. \n");
			// 원래 1992년 1월 1일과 비교를 해야 하지만...기타...타이밍적으로 잘못된 시간값이 들어와 막게됨...
			return ERR_FAIL;
		}
#if 1 // 작업 진행 필요 by mslee
		// 4. SCSerialNumberOld == SCSerialNumberNew
		//nErrorReturn = DB_SETUP_GetUserItemBinary((HINT8*)USERCONFIG_ITEMNAME_SC_OLD_SN, &oldSerialNumberBufferSize, (HUINT8 *)&SC_OldSerialNumber);
		nErrorReturn = DB_PARAM_GetItem(eDB_PARAM_ITEM_SC_OLD_SN, (HUINT32 *)SC_OldSerialNumber, 0);
		if(nErrorReturn != ERR_OK)
		{
			// bRightValidityWithinPeriod = false 에 해당하는 else 문에서 별다른 이변이 없는 한
			// 기존의 흐름대로 eCAS_APS_RET_NOT_EXPIRED 를 리턴한다.
			HxLOG_Print("Can not found OLD SC Serial Number\n");
			return ERR_FAIL;
		}
		HxLOG_Print("[SC_OldSerialNumber] (%s)\n", SC_OldSerialNumber);
#endif
		nErrorReturn = CA_NA_CakGetSystemInfo(&xStatus, &pxIrdInfo, &xNumberOfSmartcards, &pxSmartcardInfoArray);
		if(nErrorReturn != ERR_OK)
		{
			// bRightValidityWithinPeriod = false 에 해당하는 else 문에서 별다른 이변이 없는 한
			// 기존의 흐름대로 eCAS_APS_RET_NOT_EXPIRED 를 리턴한다.
			HxLOG_Print("MW_CA_NA_GetSystemInfo return Fail\n");
			return ERR_FAIL;
		}

		if(pxSmartcardInfoArray[0].pSerialNumber != NULL)
		{
			MWC_UTIL_DvbStrncpy((HUINT8 *)SC_NewSerialNumber, (const HUINT8 *)pxSmartcardInfoArray[0].pSerialNumber, 64 -1);
			HxLOG_Print("[SC_NewSerialNumber] (%s)\n", SC_NewSerialNumber);
			// 5. Display message 136
			if(MWC_UTIL_DvbStrcmp((HUINT8 *)SC_NewSerialNumber, (HUINT8 *)SC_OldSerialNumber)==0)
			{
				HLIB_DATETIME_ConvertUnixTimeToDateTime(productExpireUnixTime, &(pContext->bExpiredDate));
				HxLOG_Print("Expiration date : year [%d] M %d d %d\n", pContext->bExpiredDate.stDate.usYear,
								pContext->bExpiredDate.stDate.ucMonth,
								pContext->bExpiredDate.stDate.ucDay);
				// Message to prolonged Successfully.
				HxLOG_Print("Display 136 Message\n");
				xmgr_cas_NaSettingMessage(pContext, eCAS_APS_MSG_T_DIALOG, eCAS_APS_MSG_REF136);
				return ERR_OK;
			}
			else
			{
				HxLOG_Print("It is different from OLD Serial Number\n");
			}
		}
		return ERR_FAIL;
}
#endif


static eCAS_APS_EntitleRet	local_cas_na_EntitlementProcess(Handle_t hAction)
{
	HERROR				nErrorReturn;
	// smart expiration infomation
	HUINT8				bEndOfTime;
	TCalendarTime		stDataTime;
	UNIXTIME			scUnixTime;
	HxDATETIME_t		stSCExpirationDateTime;
	// product expiration infomation
	HUINT8				bProductExpired;
	HUINT32				ulProductExpireDuration = 0;
	UNIXTIME			productExpireUnixTime;
	HxDATETIME_t		stPDExpirationDateTime;
	// current time infomation
	UNIXTIME			currentUnixTime;
#if defined(CONFIG_MW_CAS_NAGRA_OP_SES_CAKTEST)
	UNIXTIME			currentCmpUnixTime;
#endif
	HxDATETIME_t		stCurrentDateTime;

	HxDATETIME_t		*pstExpirationDateTime;

	HUINT8				ucFrequency = 0;
	HUINT8 				ucProfieType = 0;
	NA_IRDCMD_PROFILE_t		stDisplayProfile;
	NA_IRDCMD_IDX_PROFILE_t	stCurrentProfileSet;
	HUINT8				bsubscriptionRenewable = FALSE;
	HUINT8				bRightValidityWithinPeriod = FALSE;
	HUINT8				bIsDisplayWarning = FALSE;
	////////////////////////////////////////////////////////////////////////
	HxDATETIME_t			stSubExpirationDateTime;
	HUINT8				*arDescOfMAC_CA[MACD_CA_TYPE_MAX];
	HUINT8				ucIdx;
	HUINT8				ucPrivateDataType = MACD_CA_TYPE_MAX;
	HUINT16				usPrivateDataLen[MACD_CA_TYPE_MAX];
	HUINT16				usScCaSysId;
#if !defined(CONFIG_MW_CAS_NAGRA_OP_SES_CAKTEST)
	TUnsignedInt32			ulZipCode = 0;
#endif
	TRightType			bRightType;

	HUINT8				*parDesc = NULL, *parDesc_t = NULL;
	HUINT8				ucLen;
	HINT16				nDescLen;
	HUINT8				bIncludeSubscription = FALSE;
	////////////////////////////////////////////////////////////////////////
	NaCasApsValidCheck_t	*pContext;

#if !defined(CONFIG_MW_CAS_NAGRA_OP_SES_CAKTEST)
	// Check Zip Code is valid or not.
	// CAK7 System Message 사양에서만 140 메시지가 언급되고 있다.
	CA_NA_CakGetZipCode (0, &ulZipCode);
	HxLOG_Print("ulZipCode (%d)\n", (int)ulZipCode);
	if ((ulZipCode >= 1) && (ulZipCode <= 10))
	{
		// HD+ SC is activated !!!!
	}
	else
	{
		pContext = xmgr_cas_Naaps_GetValidContext();
		HxSTD_memset(pContext, 0, sizeof(NaCasApsValidCheck_t));

		pContext->hAction = hAction;

		xmgr_cas_NaSettingMessage(pContext, eCAS_APS_MSG_T_BLOCK, eCAS_APS_MSG_REF140);
		return eCAS_APS_RET_CHECK;
	}
#endif

	for (ucIdx = 0; ucIdx < MACD_CA_TYPE_MAX; ucIdx++)
	{
		arDescOfMAC_CA[ucIdx] = NULL;
		usPrivateDataLen[ucIdx] = 0;
	}

	nErrorReturn = CA_NA_CakGetEcmCaSystemId(0, &usScCaSysId);
	if (nErrorReturn != ERR_OK)
	{
		HxLOG_Info(" \n");
		goto error_aps_spec;
	}


	//2 // to get MAC_descriptor  (Merlin Access Criteria)
	nErrorReturn = MW_CA_NA_GetSizeOfMacCaDesc(hAction, usScCaSysId, &ucLen);
	if ((nErrorReturn == ERR_OK) && (ucLen > 0))
	{
		parDesc = (HUINT8*)OxCAS_Malloc(ucLen);
		if (parDesc == NULL)
		{
			HxLOG_Info(" \n");
 			goto error_aps_spec;
		}
	}
	else
	{
		HxLOG_Info(" \n");
		goto error_aps_spec;
	}

	nErrorReturn = MW_CA_NA_GetContentOfMacCaDesc(hAction, usScCaSysId, parDesc);
	if (nErrorReturn != ERR_OK)
	{
		HxLOG_Info(" \n");

		// descriptor memory free
		if (parDesc != NULL)
			OxCAS_Free(parDesc);

		goto error_aps_spec;
	}


	//2 // to split and modify MAC_descriptor
	bIncludeSubscription = FALSE;

	parDesc_t = parDesc;
	nDescLen = *(parDesc+1);
	parDesc += 2;

	while(nDescLen > 0)
	{
		ucPrivateDataType = *parDesc;
		usPrivateDataLen[ucPrivateDataType] = (HUINT16)get16bit(parDesc+1);

		if ((ucPrivateDataType == MACD_CA_TYPE_PRE_PAID) || (ucPrivateDataType == MACD_CA_TYPE_SUBSCRIP))
		{
			if (ucPrivateDataType == MACD_CA_TYPE_SUBSCRIP)
			{
				bIncludeSubscription = TRUE;
			}

			if (usPrivateDataLen[ucPrivateDataType] > 0)
			{
				arDescOfMAC_CA[ucPrivateDataType] = OxCAS_Malloc (usPrivateDataLen[ucPrivateDataType]);
			}

			if ((arDescOfMAC_CA[ucPrivateDataType] != NULL) && (usPrivateDataLen[ucPrivateDataType] > 0))
			{
				HxSTD_memcpy(arDescOfMAC_CA[ucPrivateDataType],  parDesc+3, usPrivateDataLen[ucPrivateDataType]);
			}
		}
		else
		{
			HxLOG_Info(" \n");
		}

		nDescLen -= (usPrivateDataLen[ucPrivateDataType]+3);
		parDesc += (usPrivateDataLen[ucPrivateDataType]+3);
	}


#ifdef CONFIG_DEBUG
{
	int x__d = 0, y__d = 0;

	for (x__d = 0; x__d < MACD_CA_TYPE_MAX; x__d++)
	{
		HxLOG_Print("\n\n");
		for (y__d = 0; y__d < usPrivateDataLen[x__d]; y__d++)
		{
			if (arDescOfMAC_CA[x__d] != NULL)
			{
				HxLOG_Print("0x%02X ", arDescOfMAC_CA[x__d][y__d]);
			}

			if (y__d % 16 == 15)
			{
				HxLOG_Print("\n");
			}
		}
	}
	HxLOG_Print("\n\n");
}
#endif

	// descriptor memory free
	if (parDesc_t != NULL)
		OxCAS_Free(parDesc_t);

	// 현재시간 불러오는 함수는 최대한 사용할 곳에 밀착하여 붙여야 오차가 줄어든다...
	// 1초차이로 Date정보 오류가 발생하니 오차를 최대한 줄일 것....
	//2 // to get current unixtime
	nErrorReturn = VK_CLOCK_GetTime(&currentUnixTime);
	if(nErrorReturn != ERR_OK)
	{
		HxLOG_Info("VK_CLOCK_GetTime error[%x] \n", nErrorReturn);
		return eCAS_APS_RET_ERROR;
	}

	HLIB_DATETIME_ConvertUnixTimeToDateTime(currentUnixTime, &stCurrentDateTime);

	//2 // to check subscription existance in MAC_descriptor
	if (bIncludeSubscription == TRUE)
	{
		//2 // to get subscription expireduration

		bRightType = eSubscription;
		nErrorReturn = MW_CA_NA_GetValidityHdProduct(bRightType,
								usPrivateDataLen[MACD_CA_TYPE_SUBSCRIP],
								arDescOfMAC_CA[MACD_CA_TYPE_SUBSCRIP],
								&bProductExpired,
								&ulProductExpireDuration);
		if (nErrorReturn != ERR_OK)
		{
			HxLOG_Info(" MW_CA_NA_GetValidityHdProduct eSubscription - ERR_FAIL Return\n");
			goto error_aps_spec;
		}
		HxLOG_Print("[subscription] bProductExpired[%d] \n", bProductExpired);

		if (bProductExpired != TRUE)
		{
			HxLOG_Info(" a HD+ subscription exist in the NASP_CA_descriptor and it doesn't expired! \n");

			// Subscription 의 exp date update
			productExpireUnixTime = currentUnixTime + ulProductExpireDuration;
			HLIB_DATETIME_ConvertUnixTimeToDateTime(productExpireUnixTime, &stSubExpirationDateTime);
			local_cas_na_UpdateExpireDate(&stSubExpirationDateTime);
			for (ucIdx = 0; ucIdx < MACD_CA_TYPE_MAX; ucIdx++)
			{
				if (arDescOfMAC_CA[ucIdx] != NULL)
				{
					 OxCAS_Free (arDescOfMAC_CA[ucIdx]);
					 arDescOfMAC_CA[ucIdx] = NULL;
				}
			}
			CASMGR_APS_SaveSCSerialNum_ExpireDate(currentUnixTime, stCurrentDateTime);

			return eCAS_APS_RET_NOT_EXPIRED;
		}
	}
	else
	{
		HxLOG_Print(" a HD+ subscription don't exist in the NASP_CA_descriptor~\n");
	}


	//2 // to get pre-paid expireduration

	bRightType = ePre_Paid;
	nErrorReturn = MW_CA_NA_GetValidityHdProduct(bRightType, usPrivateDataLen[MACD_CA_TYPE_PRE_PAID], arDescOfMAC_CA[MACD_CA_TYPE_PRE_PAID],
													&bProductExpired, (TCaDuration*)&ulProductExpireDuration);
	if(nErrorReturn != ERR_OK)
	{
		HxLOG_Info("VK_CLOCK_GetTime error[%x] \n", nErrorReturn);
		goto error_aps_spec;
	}

#if defined(CONFIG_MW_CAS_NAGRA_OP_SES_CAKTEST)
	nErrorReturn = VK_CLOCK_GetTime(&currentCmpUnixTime);
	// currentUnixTime과 currentCmpUnixTime의 시간차가 날경우 최신의 시간으로 다시 설정한다.
	if(nErrorReturn == ERR_OK && currentCmpUnixTime > currentUnixTime)
	{
		HxLOG_Print("Convert ::: currentUnixTime = (%d)(%d)\n", currentUnixTime, currentCmpUnixTime);
		currentUnixTime = currentCmpUnixTime;
	}
#endif

	HLIB_DATETIME_ConvertUnixTimeToDateTime(currentUnixTime, &stCurrentDateTime);

	productExpireUnixTime = currentUnixTime + ulProductExpireDuration;
	HLIB_DATETIME_ConvertUnixTimeToDateTime(productExpireUnixTime, &stPDExpirationDateTime);


	//2 // to get smartcard expiration data
	nErrorReturn = MW_CA_NA_GetScExpirationDate(0, &bEndOfTime, &stDataTime);
	if (nErrorReturn != ERR_OK)
	{
		HxLOG_Info(" MW_CA_NA_GetScExpirationDate error [%x]\n", nErrorReturn);
		goto error_aps_spec;
	}
	xmgr_cas_NaConvertDataTime(&stDataTime, &stSCExpirationDateTime);
	HLIB_DATETIME_ConvertDateTimeToUnixTime(&stSCExpirationDateTime, &scUnixTime);

	//2 // to get context!
	pContext = xmgr_cas_Naaps_GetValidContext();
	HxSTD_memset(pContext, 0, sizeof(NaCasApsValidCheck_t));

	pContext->hAction = hAction;

	// check subscription renewable & get profile
	if (bEndOfTime)
	{
		HxLOG_Error("eProfileProducts \n");
		bsubscriptionRenewable = TRUE;
		ucProfieType = eProfileProducts;
	}
	else
	{
		HxLOG_Error("eProfileSmartcard\n");
		bsubscriptionRenewable = FALSE;
		ucProfieType = eProfileSmartcard;
	}

	nErrorReturn = xmgr_cas_NaGetDisplayProfile(&stDisplayProfile, ucProfieType);
	if (nErrorReturn != ERR_OK)
	{
		HxLOG_Info(" xmgr_cas_NaGetDisplayProfile error [%x]\n", nErrorReturn);
		goto error_aps_spec;
	}


	nErrorReturn = xmgr_cas_NaGetIndexDisplayProfile(&stCurrentProfileSet, ucProfieType);
	if (nErrorReturn != ERR_OK)
	{
		HxLOG_Info(" xmgr_cas_NaGetIndexDisplayProfile error [%x]\n", nErrorReturn);
		goto error_aps_spec;
	}

	HxLOG_Print("stCurrentDateTime[%d],[%d],[%d]\n",stCurrentDateTime.stDate.usYear,	stCurrentDateTime.stDate.ucMonth, stCurrentDateTime.stDate.ucDay);
	HxLOG_Print("stSCExpirationDateTime[%d],[%d],[%d]\n",stSCExpirationDateTime.stDate.usYear, stSCExpirationDateTime.stDate.ucMonth, stSCExpirationDateTime.stDate.ucDay);
	HxLOG_Print("stPDExpirationDateTime[%d],[%d],[%d]\n",stPDExpirationDateTime.stDate.usYear, stPDExpirationDateTime.stDate.ucMonth, stPDExpirationDateTime.stDate.ucDay);

	//2 // smartcard validity expired
	if ((bEndOfTime == FALSE) && (currentUnixTime >= scUnixTime))
	{
		// message display ref 133
		xmgr_cas_NaSettingMessage(pContext, eCAS_APS_MSG_T_BLOCK, eCAS_APS_MSG_REF133);
			// update card expire date
			local_cas_na_UpdateExpireDate(&stSCExpirationDateTime);
			for (ucIdx = 0; ucIdx < MACD_CA_TYPE_MAX; ucIdx++)
			{
				if (arDescOfMAC_CA[ucIdx] != NULL)
				{
					 OxCAS_Free (arDescOfMAC_CA[ucIdx]);
					 arDescOfMAC_CA[ucIdx] = NULL;
				}
			}

		CASMGR_APS_SaveSCSerialNum_ExpireDate(currentUnixTime, stCurrentDateTime);
		return eCAS_APS_RET_EXPIRED;
	}

	if ((bProductExpired) || (ulProductExpireDuration == 0)) // if the ulProductExpireDuration value is zero, the product expired!
	{
		if (bsubscriptionRenewable)
		{
			// message display ref 131
            HxLOG_Print("Expired, but it's renewable. 131Message Display \n");
			xmgr_cas_NaSettingMessage(pContext, eCAS_APS_MSG_T_BLOCK, eCAS_APS_MSG_REF131);
		}
		else
		{
			// message display ref 133
            HxLOG_Print("Expired, but it's renewable. 131Message Display \n");
			xmgr_cas_NaSettingMessage(pContext, eCAS_APS_MSG_T_BLOCK, eCAS_APS_MSG_REF133);
		}

		//update pre-paid expire date
		if (bProductExpired == TRUE)
		{
			local_cas_na_UpdateExpireDate(&stPDExpirationDateTime);
		}
		else
		{	// expire 아니고,  ulProductExpireDuration =0 일 경우는 현재 일자가 출력되지 않도록 clear 함.
			local_cas_na_aps_ClearExpireDate();
		}
		for (ucIdx = 0; ucIdx < MACD_CA_TYPE_MAX; ucIdx++)
		{
			if (arDescOfMAC_CA[ucIdx] != NULL)
			{
				 OxCAS_Free (arDescOfMAC_CA[ucIdx]);
				 arDescOfMAC_CA[ucIdx] = NULL;
			}
		}
		CASMGR_APS_SaveSCSerialNum_ExpireDate(currentUnixTime, stCurrentDateTime);
		return eCAS_APS_RET_EXPIRED;
	}

	pstExpirationDateTime = &stPDExpirationDateTime;

	if (!bsubscriptionRenewable)
	{
		if (HLIB_DATETIME_CompareDate(stPDExpirationDateTime.stDate, stSCExpirationDateTime.stDate) > 0)
		{
			HxLOG_Print("stSCExpirationDateTime close to current time\n");
			pstExpirationDateTime = &stSCExpirationDateTime;
		}

	}

        // HD + : update expire date to display on the smartcard info
	local_cas_na_UpdateExpireDate(pstExpirationDateTime);
	bRightValidityWithinPeriod = xmgr_cas_NaCheckRightValidityWithinPeriod( pContext,
												ucProfieType, stCurrentDateTime, pstExpirationDateTime,
												&stDisplayProfile, &stCurrentProfileSet, &ucFrequency);

	HxLOG_Print("bRightValidityWithinPeriod (%d)\n", bRightValidityWithinPeriod);
	if ( bRightValidityWithinPeriod )
	{
		if (pContext->bDisplayWarning)
		{
			if (ucFrequency == APS_PERIOD_ONCE) // once
			{
				bIsDisplayWarning = TRUE;
				HxLOG_Error("APS_PERIOD_ONCE \n");
			}
			else if (ucFrequency == APS_PERIOD_DAILY) // daily
			{
				bIsDisplayWarning = TRUE;
				HxLOG_Error("APS_PERIOD_DAILY \n");
			}
			else if (ucFrequency == APS_PERIOD_ALWAYS) // always
			{
				bIsDisplayWarning = TRUE;
				HxLOG_Error("APS_PERIOD_ALWAYS \n");
			}
		}

		if (bIsDisplayWarning)
		{
			if (pContext->bRemainDay == 0) // today
			{
				if (bsubscriptionRenewable)
				{
					// message display ref 134
					HxLOG_Print("Remain Day is one, but it's renewable. 134 Message Display \n");
					xmgr_cas_NaSettingMessage(pContext, eCAS_APS_MSG_T_DIALOG, eCAS_APS_MSG_REF134);
				}
				else
				{
					// message display ref 135
                                        HxLOG_Print("Remain Day is one, but it's not renewable. 135 Message Display \n");
					xmgr_cas_NaSettingMessage(pContext, eCAS_APS_MSG_T_DIALOG, eCAS_APS_MSG_REF135);
				}
			}
			else
			{
				if (bsubscriptionRenewable)
				{
					// message display ref 130
					HxLOG_Print("Remain Day is a few, but it's renewable. 130 Message Display \n");
					xmgr_cas_NaSettingMessage(pContext, eCAS_APS_MSG_T_DIALOG, eCAS_APS_MSG_REF130A);
				}
				else
				{
					// message display ref 132
					HxLOG_Print("Remain Day is a few, but it's not renewable. 132 Message Display \n");
					xmgr_cas_NaSettingMessage(pContext, eCAS_APS_MSG_T_DIALOG, eCAS_APS_MSG_REF132A);
				}
			}

			for (ucIdx = 0; ucIdx < MACD_CA_TYPE_MAX; ucIdx++)
			{
				if (arDescOfMAC_CA[ucIdx] != NULL)
				{
					 OxCAS_Free (arDescOfMAC_CA[ucIdx]);
					 arDescOfMAC_CA[ucIdx] = NULL;
				}
			}

			CASMGR_APS_SaveSCSerialNum_ExpireDate(currentUnixTime, stCurrentDateTime);
			return eCAS_APS_RET_CHECK;
		}
		else
		{
			HxLOG_Print("don't display warning! ucFrequency[%d]\n", ucFrequency);
		}
	}
	else
	{
		if(stCurrentDateTime.stDate.usYear >= 1992)
		{
			// 1992년 1월 1일이 SPEC상 초기값이므로...
			nErrorReturn = local_cas_na_aps_Check_EntitlementMessage136(pContext, productExpireUnixTime);
			if(nErrorReturn == ERR_OK)
			{
				for (ucIdx = 0; ucIdx < MACD_CA_TYPE_MAX; ucIdx++)
				{
					if (arDescOfMAC_CA[ucIdx] != NULL)
					{
						 OxCAS_Free (arDescOfMAC_CA[ucIdx]);
						 arDescOfMAC_CA[ucIdx] = NULL;
					}
				}
					CASMGR_APS_SaveSCSerialNum_ExpireDate(currentUnixTime, stCurrentDateTime);
					return eCAS_APS_RET_CHECK;
			}
		}
		else
		{
			for (ucIdx = 0; ucIdx < MACD_CA_TYPE_MAX; ucIdx++)
			{
				if (arDescOfMAC_CA[ucIdx] != NULL)
				{
					 OxCAS_Free (arDescOfMAC_CA[ucIdx]);
					 arDescOfMAC_CA[ucIdx] = NULL;
				}
			}
			return eCAS_APS_RET_NOT_EXPIRED;
		}
	}

	for (ucIdx = 0; ucIdx < MACD_CA_TYPE_MAX; ucIdx++)
	{
		if (arDescOfMAC_CA[ucIdx] != NULL)
		{
			 OxCAS_Free (arDescOfMAC_CA[ucIdx]);
			 arDescOfMAC_CA[ucIdx] = NULL;
		}
	}
	CASMGR_APS_SaveSCSerialNum_ExpireDate(currentUnixTime, stCurrentDateTime);
	return eCAS_APS_RET_NOT_EXPIRED;

error_aps_spec:
	for (ucIdx = 0; ucIdx < MACD_CA_TYPE_MAX; ucIdx++)
	{
		if (arDescOfMAC_CA[ucIdx] != NULL)
		{
			 OxCAS_Free (arDescOfMAC_CA[ucIdx]);
			 arDescOfMAC_CA[ucIdx] = NULL;
		}
	}

	return eCAS_APS_RET_ERROR;
}


static HERROR	xmgr_cas_NaUpdateCasMessage(Handle_t hAction, SvcCas_MsgType_e eMsgType, DxCAS_GroupId_e eCasId, void *pMessage, SvcCas_ShowType_e eShowType)
{
	HUINT32				ulActionId;
	HUINT8				bCheckMainHandle = FALSE;
	HUINT8 				bCheckRightValidity = FALSE;
	NaAlarmData_t 		*pstNaAlarmData = NULL;
	eCAS_APS_EntitleRet	eReturnRightValidity;

	ulActionId = svc_cas_SubSvcRmGetActionId(hAction);
	if ((ulActionId == eActionId_VIEW_0) || (ulActionId == eActionId_VIEW_1))
	{
		HxLOG_Print("matching main action id[%x]\n", ulActionId);
		bCheckMainHandle = TRUE;
	}
	else
	{
		HxLOG_Print("not matching main action id[%x]\n", ulActionId);
		bCheckMainHandle = FALSE;
	}

	if (bCheckMainHandle && (MW_CA_NA_IsHdPlusService(hAction) == ERR_OK))
	{
		if (eMsgType == eCAS_MSG_TYPE_Fail)
		{
			pstNaAlarmData = (NaAlarmData_t *)pMessage;
			if ( ((pstNaAlarmData->eAlarmType == eAlarm_Access) &&
				(pstNaAlarmData->pstAccessAlarmItems->eAlarm == CA_ACCESS_DENIED)) ||
				 ((pstNaAlarmData->eAlarmType == eAlarm_Smartcard) &&
				 		(pstNaAlarmData->pstScAlarmItems->eAlarm == CA_SMARTCARD_STATE_EXPIRED)) )
				 // the processing of smartcard suspended don't mention at here.
				 // both hdplus and non-hdplus processes SC suspend message.
			{
				bCheckRightValidity = TRUE;
			}
		}
		else if (eMsgType == eCAS_MSG_TYPE_Ok)
		{
			bCheckRightValidity = TRUE;
		}
		else
		{
			HxLOG_Error("don't enter here[%x]\n", eMsgType);
		}
	}

	if (bCheckRightValidity)
	{
		eReturnRightValidity = local_cas_na_EntitlementProcess(hAction);
		HxLOG_Print("eReturnRightValidity[%x]\n", eReturnRightValidity);

		switch (eReturnRightValidity)
		{
		case eCAS_APS_RET_CHECK :
			eMsgType	= eCAS_MSG_TYPE_Check;
			pMessage	= (void*)xmgr_cas_Naaps_GetCurrentCheckMessage();
			break;

		case eCAS_APS_RET_EXPIRED :
			eMsgType	= eCAS_MSG_TYPE_Check;
			eShowType	= eCasShowType_None;
			pMessage	= (void*)xmgr_cas_Naaps_GetCurrentCheckMessage();
			break;

		case eCAS_APS_RET_NOT_EXPIRED :
			// pass the message
			break;

		case eCAS_APS_RET_ERROR :
			// pass the message
			break;

		default :
			break;
		}
	}

	if (svc_cas_MgrSubUpdateCasMessage(hAction, eMsgType, eCasId, pMessage, eShowType) != ERR_OK)
	{
		HxLOG_Error("svc_cas_MgrSubUpdateCasMessage\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}


HERROR	CASMGR_APS_SaveSCSerialNum_ExpireDate(UNIXTIME		currentUnixTime, HxDATETIME_t stCurrentDateTime)
{
	HERROR			err = ERR_FAIL;
	HERROR			db1_err = ERR_FAIL;
	HERROR			db2_err = ERR_FAIL;
	TChar			SC_SerialNumber[64];
	TChar			SC_OldSerialNumber[64];
	UNIXTIME		stOldExpireDate = 0;
	TCaDuration 		expireDuraion;
	UNIXTIME		productUnixTime;
	TSystemInfoStatus	xStatus = CA_SYSTEM_INFO_ERROR;
	TIrdInfo			*pxIrdInfo = NULL;
	TUnsignedInt8		xNumberOfSmartcards = 0;
	TSmartcardInfoItem	*pxSmartcardInfoArray = NULL;
	//HINT32			oldSerialNumberBufferSize =0;

	if(stCurrentDateTime.stDate.usYear < 1992)
	{
		// 초기값 보다 작은 DATE 제외...
		HxLOG_Print("Could not Receive Current Date...\n");
		return ERR_FAIL;
	}
	err = MW_CA_NA_GetSystemInfo(&xStatus, &pxIrdInfo, &xNumberOfSmartcards, &pxSmartcardInfoArray);
	if(err == ERR_OK)
	{
		if (pxSmartcardInfoArray)
		{
			HxSTD_memset(SC_SerialNumber, 0, sizeof(HUINT8)*64);
			if(pxSmartcardInfoArray[0].pSerialNumber != NULL)
			{
				MWC_UTIL_DvbStrncpy((HUINT8 *)SC_SerialNumber, (const HUINT8 *)pxSmartcardInfoArray[0].pSerialNumber, 64 -1);
			}
			err = MW_CA_NA_GetPdExpirationDuration(&expireDuraion);
			if (err == ERR_OK)
			{
				if(expireDuraion == 0)
				{
					HxLOG_Print("expireDuraion is Zero\n");
					//Duration이 0인 경우는 Rental Time을 불러오지 못해서다...
					return ERR_FAIL;
				}

					productUnixTime = currentUnixTime + expireDuraion;

					// 같은 타이밍에 저장되지 않으면 의미가 없다.
// 수정 필요 by mslee 2013/09/08
#if 1
					//db1_err = DB_SETUP_GetUserItemBinary((HINT8*)USERCONFIG_ITEMNAME_SC_OLD_SN, &oldSerialNumberBufferSize, (HUINT8 *)&SC_OldSerialNumber);
					//db2_err = DB_SETUP_GetUserItemValue((HINT8*)USERCONFIG_ITEMNAME_OLDPREPAIDRIGHT, (HINT32 *)&stOldExpireDate);
					db1_err = DB_PARAM_GetItem(eDB_PARAM_ITEM_SC_OLD_SN, (HUINT32 *)SC_OldSerialNumber, 0);
					db2_err = DB_PARAM_GetItem(eDB_PARAM_ITEM_OLDPREPAIDRIGHT, (HUINT32 *)&stOldExpireDate, 0);
					if(db1_err == ERR_OK)
					{
						// 동일할 경우 굳이 Write작업은 하지 않는다.
						if(VK_strcmp(&SC_OldSerialNumber, &SC_SerialNumber) != 0)
						{
//							db1_err = DB_SETUP_SetUserItemBinary((HINT8*)USERCONFIG_ITEMNAME_SC_OLD_SN, 64, (HUINT8*)&SC_SerialNumber);
//							DB_SETUP_SyncUserItem((HINT8*)USERCONFIG_ITEMNAME_SC_OLD_SN);
							db1_err = DB_PARAM_SetItem(eDB_PARAM_ITEM_SC_OLD_SN, (HUINT32)SC_SerialNumber, 64);
							DB_PARAM_Sync ();
							HxLOG_Print("Write Current SmartCard Serial Number\n");
						}
						else
						{
							HxLOG_Print("Skip to Write Current SmartCard Serial Number\n");
						}
					}
					else
					{
//						db1_err = DB_SETUP_SetUserItemBinary((HINT8*)USERCONFIG_ITEMNAME_SC_OLD_SN, 64, (HUINT8*)&SC_SerialNumber);
//						DB_SETUP_SyncUserItem((HINT8*)USERCONFIG_ITEMNAME_SC_OLD_SN);
						db1_err = DB_PARAM_SetItem(eDB_PARAM_ITEM_SC_OLD_SN, (HUINT32)SC_SerialNumber, 64);
						DB_PARAM_Sync ();
						HxLOG_Print("Write Current SmartCard Serial Number\n");
					}

					if(db2_err == ERR_OK)
					{
						// 동일할 경우 굳이 Write작업은 하지 않는다.
						if(productUnixTime != stOldExpireDate)
						{
//							db2_err = DB_SETUP_SetUserItemValue((HINT8*)USERCONFIG_ITEMNAME_OLDPREPAIDRIGHT, (HINT32)productUnixTime);
//							DB_SETUP_SyncUserItem((HINT8*)USERCONFIG_ITEMNAME_OLDPREPAIDRIGHT);
							db2_err = DB_PARAM_SetItem (eDB_PARAM_ITEM_OLDPREPAIDRIGHT, productUnixTime, 0);
							DB_PARAM_Sync ();
							HxLOG_Print("Write Current Expire Date\n");
						}
						else
						{
							HxLOG_Print("Skip to Write Current Expire Date\n");
						}
					}
					else
					{
//						db2_err = DB_SETUP_SetUserItemValue((HINT8*)USERCONFIG_ITEMNAME_OLDPREPAIDRIGHT, (HINT32)productUnixTime);
//						DB_SETUP_SyncUserItem((HINT8*)USERCONFIG_ITEMNAME_OLDPREPAIDRIGHT);
						db2_err = DB_PARAM_SetItem (eDB_PARAM_ITEM_OLDPREPAIDRIGHT, productUnixTime, 0);
						DB_PARAM_Sync ();
						HxLOG_Print("Write Current Expire Date\n");
					}

					if(db2_err != ERR_OK || db2_err != ERR_OK)
					{
						HxLOG_Print("When fail to write DB, Write the before values.\n");
//						DB_SETUP_SetUserItemValue((HINT8*)USERCONFIG_ITEMNAME_OLDPREPAIDRIGHT, (HINT32)stOldExpireDate);
//						DB_SETUP_SyncUserItem((HINT8*)USERCONFIG_ITEMNAME_OLDPREPAIDRIGHT);
//						DB_SETUP_SetUserItemBinary((HINT8*)USERCONFIG_ITEMNAME_SC_OLD_SN, 64, (HUINT8*)&SC_OldSerialNumber);
//						DB_SETUP_SyncUserItem((HINT8*)USERCONFIG_ITEMNAME_SC_OLD_SN);
						DB_PARAM_SetItem (eDB_PARAM_ITEM_OLDPREPAIDRIGHT, stOldExpireDate, 0);
						DB_PARAM_Sync ();
						DB_PARAM_SetItem(eDB_PARAM_ITEM_SC_OLD_SN, (HUINT32)SC_SerialNumber, 64);
						DB_PARAM_Sync ();
					}
#endif
					return ERR_OK;
			}
		}
	}
	return ERR_FAIL;

}


HERROR	CASMGR_APS_SetServiceStatus(HUINT8 bflag)
{
#if defined(CONFIG_DEBUG) && defined(CONFIG_MW_CAS_NAGRA_SES_CAKTEST)
	HxLOG_Print("===============================\n");
	HxLOG_Print("[CASMGR_APS_SetServiceStatus] bflag(%d)\n", bflag);
	HxLOG_Print("===============================\n");
#endif
	s_check_new_service  = bflag;
	return ERR_OK;
}


HERROR	CASMGR_APS_UpdateCasFromAP(eCAS_APS_UPDATE_TYPE type, HUINT32 actionId)
{
	xmgr_cas_Naaps_UpdateCasFromAP(type, actionId);
	return ERR_OK;
}

HERROR	CASMGR_APS_UpdateCasMessage(Handle_t hAction, SvcCas_MsgType_e eMsgType, DxCAS_GroupId_e eCasId, void *pMessage, SvcCas_ShowType_e eShowType)
{
	if (hAction == HANDLE_NULL)
	{
		HxLOG_Print("hAction == HANDLE_NULL\n");
		return ERR_FAIL;
	}

	if ((eMsgType >= eCAS_MSG_TYPE_Max) || (eMsgType == eCAS_MSG_TYPE_None))
	{
		HxLOG_Print("eMsgType Error\n");
		return ERR_FAIL;
	}

	if (pMessage == NULL)
	{
		HxLOG_Print("pMessage == NULL\n");
		return ERR_FAIL;
	}

	return xmgr_cas_NaUpdateCasMessage(hAction, eMsgType, eCasId, pMessage, eShowType);
}

HERROR	CASMGR_APS_NaGetIndexDisplayProfile(NA_IRDCMD_IDX_PROFILE_t *pstCurrentProfile, HUINT8 ucProfieType)
{
	xmgr_cas_NaGetIndexDisplayProfile(pstCurrentProfile, ucProfieType);
	return ERR_OK;
}

HERROR	CASMGR_APS_NaPrintDisplayProfile(HUINT8 ucProfieType)
{
	NA_IRDCMD_PROFILE_t pstDisplayProfile;

	xmgr_cas_NaGetDisplayProfile(&pstDisplayProfile, ucProfieType);
	return ERR_OK;
}

/*********************** End of File ******************************/
