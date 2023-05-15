/*
 * (c) 2011-2012 Humax Co., Ltd.
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
 * CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.

 * This License is effective until terminated. You may terminate this License at any time by destroying all copies
 * of the Software including all documentation. This License will terminate immediately without notice from Humax
 * to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
 * of the Software and all documentation.

 * The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
 * All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
 * in the Republic of Korea.
*/
#include "octo_common.h"

#include "vd_humax.h"

#include "ir_platform.h"
#include "ir_fta_block.h"
#include "ir_svc.h"
#include "ir_evt.h"
#include <svc_si.h>

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

#if defined(CONFIG_DEBUG)
HUINT32 g_IrFTABlock_Level = (DBG_ASSERT);
#endif

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define ________Debugging_Macro________
#if defined(CONFIG_DEBUG)
#define IrFTABlockPrint(level, message)	(((level) & g_IrFTABlock_Level) ? (VK_Print message) : 0)
#else
#define IrFTABlockPrint(level, message)
#endif

#define	ALMAJD_FTA_FLAG	0x4000

#define	SMARTCARD_SET_CARD_IN					0x01
#define	SMARTCARD_SET_CARD_OK				0x02
#define	SMARTCARD_SET_CARD_OUT				0x04
#define	SMARTCARD_SET_PRODUCT_STATUS		0x08
#define	SMARTCARD_SET_CARD_NUMBER			0x10

/*******************************************************************/
/********************      Extern Function         *************************/
/*******************************************************************/
extern HVD_Result  HUMAX_VD_SC_GetCardPhysicalStatus(HUINT8 ucSlotID, HBOOL *pSCInserted);
extern HERROR AP_CAR_IR_GetSCValidCardInserted(HBOOL *bValidCard);


/*******************************************************************/
/********************      Static Function         *************************/
/*******************************************************************/

static HIR_ERR local_cas_ir_FTA_BLOCK_SendToAPP_UpdateNeeded(void);
#if defined (CONFIG_OP_ALMAJD)
HUINT32 local_cas_ir_FTA_BLOCK_is_service_blocked_AlMajd (Handle_t hAction);
#endif
#if defined (CONFIG_OP_UPC)
HUINT32 local_cas_ir_FTA_BLOCK_is_service_blocked_UPC (Handle_t hAction);
#endif

static HUINT32 s_SmartCardStatus = 0;

/*****************************************************************************
  Function    : local_cas_ir_FTA_BLOCK_SendToAPP_UpdateNeeded
  Parameters  :
  Output      :
  Return      :
  Description :
*****************************************************************************/
static HIR_ERR local_cas_ir_FTA_BLOCK_SendToAPP_UpdateNeeded(void)
{
	HMSG				msg;
	IR_SERVICE_INFO	*pstIrSvcInfo;

	IrFTABlockPrint(DBG_TRACE , ("[local_cas_ir_FTA_BLOCK_SendToAPP_UpdateNeeded] ++\n"));

	pstIrSvcInfo = CAS_IR_DRV_GetLiveSvcInfo();

	if(pstIrSvcInfo != NULL)
	{
		VK_memset32(&msg, 0, sizeof(HMSG));

		msg.usMsgClass = IREVT_CLASS_FTA_BLOCK_STATUS_CHANGED;
		CAS_IR_EVT_SendEvent(pstIrSvcInfo->hAction, &msg);

		IrFTABlockPrint(DBG_TRACE , ("send IREVT_CLASS_FTA_BLOCK_STATUS_CHANGED to pstIrSvcInfo->hAction(0x%08x)\n", pstIrSvcInfo->hAction));
	}

	IrFTABlockPrint(DBG_TRACE , ("[local_cas_ir_FTA_BLOCK_SendToAPP_UpdateNeeded] --\n"));

	return ERR_OK;
}

/*****************************************************************************
  Function    : local_cas_ir_FTA_BLOCK_is_service_blocked_AlMajd
  Parameters  :
  Output      :
  Return      :
  Description :
*****************************************************************************/
#if defined (CONFIG_OP_ALMAJD)
HUINT32 local_cas_ir_FTA_BLOCK_is_service_blocked_AlMajd (Handle_t hAction)
{
	IR_DRV_SC_INFO	stIrSCInfo;
	eIrDrvCaType		eCasType;
	HUINT16			usRegionCode;
	HIR_ERR			hIrError;
	HBOOL 				bSCInserted;
	HVD_Result			hVdResult;
	HUINT8 				bExist;
	HBOOL 				bResult;
	HINT32				i;
	IR_SERVICE_INFO	*pstIrSvcInfo;
	DbSvc_Info_t 			stSvcInfo;
	HERROR			hError;

	IrFTABlockPrint(DBG_TRACE, ("[CAS_IR_FTA_BLOCK_is_service_blocked] ++\n"));

	/*Check fta svc, or not */
	CAS_IR_DRV_GetSvcCasKind(hAction,&eCasType);
	if( !(eIrDrvCaType_Fta == eCasType || eIrDrvCaType_None == eCasType))
	{
		IrFTABlockPrint(DBG_TRACE, (":SRAMBLED SVC (eCasType:0x%X)\n",eCasType));

		return eFTABlockStateOK; // FTA block 은 FTA 에만
	}

	IrFTABlockPrint(DBG_TRACE, (":FTA SVC\n"));
	/*In case, FTA*/
	IrFTABlockPrint(DBG_TRACE, ("s_SmartCardStatus: 0x%08x\n", s_SmartCardStatus));
	if (((s_SmartCardStatus & (SMARTCARD_SET_PRODUCT_STATUS | SMARTCARD_SET_CARD_NUMBER)) != (SMARTCARD_SET_PRODUCT_STATUS | SMARTCARD_SET_CARD_NUMBER))
			&& ((s_SmartCardStatus & SMARTCARD_SET_CARD_OUT) == 0))
	{
		IrFTABlockPrint(DBG_TRACE, ("eFTABlockStateFail, s_SmartCardStatus: 0x%08x\n", s_SmartCardStatus));
		return eFTABlockStateFail;
	}

	for (i = 0; i < HUMAX_VD_SC_SLOT_COUNT; i++)	// multi card 도 신경 써주는 척 하자
	{
		hVdResult = HUMAX_VD_SC_GetCardPhysicalStatus (i, &bSCInserted);
		IrFTABlockPrint(DBG_TRACE, ("bSCInserted: %d\n", bSCInserted));
		if (hVdResult == eHVD_ERR)
		{
			bSCInserted = FALSE;
		}

		if (bSCInserted == TRUE)
		{
			break;
		}
	}

	if (bSCInserted == FALSE)
	{
		IrFTABlockPrint(DBG_TRACE, ("bSCInserted == FALSE  => eFTABlockStateOK\n"));
		return eFTABlockStateOK;	// CAS message 들이 처리 한다. E04-4, I-7-4
	}

	if (CAS_IR_NVR_SavedNumberIsInitial () == TRUE)
	{
		IrFTABlockPrint(DBG_TRACE, ("CAS_IR_NVR_SavedNumberIsInitial () == TRUE  => eFTABlockStateFail_Need_Activation\n"));
		return eFTABlockStateFail_Need_Activation; // AlMajd Error, 생산 초기 상태 아직 smartcard 가 pair 되지 않았다
	}

	if (HUMAX_VdSc_IsSameWithSavedSmartcardNumber () == FALSE)
	{
		IrFTABlockPrint(DBG_TRACE, ("HUMAX_VdSc_IsSameWithSavedSmartcardNumber () == FALSE  => eFTABlockStateFail_NotMatch\n"));
		return eFTABlockStateFail_NotMatch;
	}

	for (i = 0; i < HUMAX_VD_SC_SLOT_COUNT; i++)	// multi card 도 신경 써주는 척 하자
	{
		hVdResult = HUMAX_VD_SC_GetCardPhysicalStatus (i, &bSCInserted);
		IrFTABlockPrint(DBG_TRACE, ("bSCInserted: %d\n", bSCInserted));
		if (hVdResult == eHVD_ERR)
		{
			bSCInserted = FALSE;
		}

		if (bSCInserted == TRUE)
		{
			bResult = XSVC_CAS_IR_DRV_GetSCProductExistance (i, &bExist);
			if ((bResult == TRUE) && (bExist != FALSE))
			{
				IrFTABlockPrint(DBG_TRACE, ("SC Product not exist!, => eFTABlockStateOK\n"));
				return eFTABlockStateOK;
			}
		}
	}

	pstIrSvcInfo = CAS_IR_DRV_GetLiveSvcInfo();
	if(pstIrSvcInfo != NULL)
	{
		hError = DB_SVC_GetServiceInfo (pstIrSvcInfo->hSvc, &stSvcInfo);
		if (hError == ERR_OK)
		{
			hIrError = CAS_IR_NVR_ReadRegionCode (&usRegionCode, IR_PARTION_CUSTOM_REGION_CODE_SIZE);
			if ((hIrError == HIR_OK) && ((usRegionCode & ALMAJD_FTA_FLAG) == 0))
			{
				if (stSvcInfo.usBouqId == ALMAJD_BOUQUET_ID)
				{
					IrFTABlockPrint(DBG_TRACE, ("stSvcInfo.usBouqId == ALMAJD_BOUQUET_ID  => eFTABlockStateFail_AlMajdService\n"));
					return eFTABlockStateFail_AlMajdService;
				}
			}
			else
			{
				if (stSvcInfo.ucGlobalWhiteSvc == 1)	// Global white service 인지 check
				{
					IrFTABlockPrint(DBG_TRACE, ("stSvcInfo.ucGlobalWhiteSvc == 1  => eFTABlockStateOK\n"));
					return eFTABlockStateOK;	// Global white service 는 card 에 권한이 없어도 FTA  flag 가 on 이라면 보여 준다.
				}
			}
		}
	}

	IrFTABlockPrint(DBG_TRACE, ("return eFTABlockStateFail_ASIService\n"));

	IrFTABlockPrint(DBG_TRACE, ("[CAS_IR_FTA_BLOCK_is_service_blocked] --\n"));

	return eFTABlockStateFail_ASIService;
}
#endif
#if defined (CONFIG_OP_UPC)
HUINT32 local_cas_ir_FTA_BLOCK_is_service_blocked_UPC (Handle_t hAction)
{
	IR_DRV_SC_INFO	stIrSCInfo;
	eIrDrvCaType		eCasType;
	HUINT16			usRegionCode;
	HIR_ERR			hIrError;
	HBOOL 				bSCInserted;
	HVD_Result			hVdResult;
	HUINT8 				bExist;
	HBOOL 				bResult;
	HINT32				i;
	IR_SERVICE_INFO	*pstIrSvcInfo;
	DbSvc_Info_t 			stSvcInfo;
	HERROR			hError;
	HBOOL			bScValid = FALSE;

	IrFTABlockPrint(DBG_TRACE, ("[CAS_IR_FTA_BLOCK_is_service_blocked] ++\n"));

	/*Check fta svc, or not */
	CAS_IR_DRV_GetSvcCasKind(hAction,&eCasType);
	if( !(eIrDrvCaType_Fta == eCasType || eIrDrvCaType_None == eCasType))
	{
		IrFTABlockPrint(DBG_TRACE, (":SCRAMBLED SVC (eCasType:0x%X)\n",eCasType));

		return eFTABlockStateOK; // FTA block 은 FTA 에만
	}

	IrFTABlockPrint(DBG_TRACE, (":FTA SVC\n"));
	/*In case, FTA*/
	for (i = 0; i < HUMAX_VD_SC_SLOT_COUNT; i++)	// multi card 도 신경 써주는 척 하자
	{
		hVdResult = HUMAX_VD_SC_GetCardPhysicalStatus (i, &bSCInserted);
		IrFTABlockPrint(DBG_TRACE, ("bSCInserted: %d\n", bSCInserted));
		if (hVdResult == eHVD_ERR)
		{
			bSCInserted = FALSE;
		}

		if (bSCInserted == TRUE)
		{
			// 스마트 카드가 VALID 한지 check 해준다 .
			bScValid = FALSE;
			AP_CAR_IR_GetSCValidCardInserted(&bScValid);
			if(bScValid)
			{
				IrFTABlockPrint(DBG_TRACE, ("SC is valid !!, => eFTABlockStateOK\n"));
				return eFTABlockStateOK;
			}
		}
	}
	// smart card 가 존재 하지 않을 경우 E04-4
	if(bSCInserted == FALSE)
	{
		IrFTABlockPrint(DBG_TRACE, ("SC is not Exist  !!, => eFTABlockStateFail_NoCard\n"));
		return eFTABlockStateFail_NoCard;
	}

	// smart card 가 존재하지만 ,valid 하지 않을 경우에는 E05-4
	if((bSCInserted == TRUE)&&(bScValid == FALSE))
	{
		// Unknown Smartcard
		IrFTABlockPrint(DBG_TRACE, ("SC is not valid  !!, => eFTABlockStateFail\n"));
		return	eFTABlockStateFail;
	}
	IrFTABlockPrint(DBG_TRACE, ("return eFTABlockStateFail\n"));
	IrFTABlockPrint(DBG_TRACE, ("[CAS_IR_FTA_BLOCK_is_service_blocked] --\n"));

	return eFTABlockStateFail;
}


#endif

/*****************************************************************************
  Function    : CAS_IR_FTA_BLOCK_update_SC_Status
  Parameters  :
  Output      :
  Return      :
  Description :
*****************************************************************************/
HUINT32 CAS_IR_FTA_BLOCK_update_Status( FTA_BLOCK_SC_UPDATE_TYPE eUpdateType)
{
	IrFTABlockPrint( DBG_TRACE, ("[CAS_IR_FTA_BLOCK_update_SC_Status] ++, eUpdateType: %d, s_SmartCardStatus: 0x%08x\n", eUpdateType, s_SmartCardStatus));

	if (eUpdateType == FTA_BLOCK_UPDATE_IRCARD_STATUS_IN)
	{
		s_SmartCardStatus |= SMARTCARD_SET_CARD_IN;
		s_SmartCardStatus &= ~SMARTCARD_SET_CARD_OUT;
		s_SmartCardStatus &= ~SMARTCARD_SET_PRODUCT_STATUS;	// card 가 in 되고 다시 받으려고
		s_SmartCardStatus &= ~SMARTCARD_SET_CARD_NUMBER;
		s_SmartCardStatus &= ~SMARTCARD_SET_CARD_OK;
	}
	else if (eUpdateType == FTA_BLOCK_UPDATE_IRCARD_STATUS_OK)
	{
		s_SmartCardStatus |= SMARTCARD_SET_CARD_IN;
		s_SmartCardStatus |= SMARTCARD_SET_CARD_OK;
	}
	else if (eUpdateType == FTA_BLOCK_UPDATE_IRCARD_STATUS_OUT)
	{
		s_SmartCardStatus |= SMARTCARD_SET_CARD_OUT;
		s_SmartCardStatus &= ~SMARTCARD_SET_CARD_IN;
		s_SmartCardStatus &= ~SMARTCARD_SET_CARD_NUMBER;
		s_SmartCardStatus &= ~SMARTCARD_SET_PRODUCT_STATUS;
		s_SmartCardStatus &= ~SMARTCARD_SET_CARD_OK;
	}
	else if (eUpdateType == FTA_BLOCK_UPDATE_PRODUCT_STATUS)
	{
		s_SmartCardStatus |= SMARTCARD_SET_PRODUCT_STATUS;
	}
	else if (eUpdateType == FTA_BLOCK_UPDATE_SMARTCARD_NUMBER)
	{
		s_SmartCardStatus |= SMARTCARD_SET_CARD_NUMBER;
		s_SmartCardStatus &= ~SMARTCARD_SET_CARD_OUT;
	}

	IrFTABlockPrint( DBG_TRACE, ("changed s_SmartCardStatus: 0x%08x\n", s_SmartCardStatus));


	if ((s_SmartCardStatus & (SMARTCARD_SET_CARD_OK | SMARTCARD_SET_CARD_IN)) == (SMARTCARD_SET_CARD_OK | SMARTCARD_SET_CARD_IN))
	{
		IrFTABlockPrint( DBG_TRACE, ("(s_SmartCardStatus & (SMARTCARD_SET_CARD_OK | SMARTCARD_SET_CARD_IN)) == (SMARTCARD_SET_CARD_OK | SMARTCARD_SET_CARD_IN) \n"));
		local_cas_ir_FTA_BLOCK_SendToAPP_UpdateNeeded();
	}

	if (eUpdateType == FTA_BLOCK_UPDATE_IRCARD_STATUS_OUT)
	{
		IrFTABlockPrint( DBG_TRACE, ("eUpdateType == FTA_BLOCK_UPDATE_IRCARD_STATUS_OUT \n"));
		local_cas_ir_FTA_BLOCK_SendToAPP_UpdateNeeded();
	}
	else if (eUpdateType == FTA_BLOCK_UPDATE_PRODUCT_STATUS)
	{
		IrFTABlockPrint( DBG_TRACE, ("eUpdateType == FTA_BLOCK_UPDATE_PRODUCT_STATUS \n"));
		local_cas_ir_FTA_BLOCK_SendToAPP_UpdateNeeded();
	}

	IrFTABlockPrint( DBG_TRACE, ("[CAS_IR_FTA_BLOCK_update_SC_Status] --\n"));

	return ERR_OK;
}

/*****************************************************************************
  Function    : CAS_IR_FTA_BLOCK_is_service_blocked
  Parameters  :
  Output      :
  Return      :
  Description :
*****************************************************************************/
HUINT32 CAS_IR_FTA_BLOCK_is_service_blocked(Handle_t hAction)
{
#if defined (CONFIG_OP_ALMAJD)
	return local_cas_ir_FTA_BLOCK_is_service_blocked_AlMajd (hAction);
#elif defined (CONFIG_OP_UPC)
	return local_cas_ir_FTA_BLOCK_is_service_blocked_UPC (hAction);
#endif
	return eFTABlockStateOK;
}

