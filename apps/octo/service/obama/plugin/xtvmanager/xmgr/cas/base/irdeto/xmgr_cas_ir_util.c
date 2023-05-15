/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <s3_cadrv.h>
#include <octo_common.h>

#include <svc_pipe.h>
#include <svc_cas.h>
#include <svc_si.h>


#include <mgr_cas.h>

#include "local_include/_xmgr_cas_ir.h"
#include "local_include/_xmgr_cas_ir_adapt.h"
#include "local_include/_xmgr_cas_ir_util.h"
#include "local_include/_xmgr_cas_ir_ui.h"
#include <ir_evt.h>
#include <ir_msg.h>
#include <db_param.h>
#include <mgr_swup.h>
#include <mgr_action.h>
#include <ir_ctrl.h>
#include <mgr_appmgr.h>
#include <_svc_cas_mgr_control.h>

#include <ctype.h>	/* for toupper(...) */
#include "ir_fta_block.h"
#include "../../local_include/_xmgr_cas.h"

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Definitions         **************************/
/*******************************************************************/





#define GET_ASCII_NUM(X)	((X) - '0')

/*******************************************************************/
/********************      Static Varaibles      ********************/
/*******************************************************************/
STATIC HBOOL	s_bCardInserted[CAS_MAX_NUM_OF_SC];
STATIC HBOOL	s_bNationalityAvailable = FALSE;
#if defined(CONFIG_SUPPORT_IRDETO_AP_STATE)
//  irdeto에서 ap state를 관리하기 위해서 만들었음.
STATIC XmgrCas_IrRunningState_e	s_eIrApRunningState;
STATIC XmgrCas_IrSolState_e		s_eIrApSolState;
STATIC XmgrCas_IrMRState_e		s_eIrApMRState;
STATIC XmgrCas_IrSolState_e		s_eIrApFTABlockState;
#endif

/*******************************************************************/
/********************      Local Functions      ********************/
/*******************************************************************/
#define _____XMGR_CAS_IR_LOCAL_FUNCTION______________________________________________________

#if defined (CONFIG_MW_CAS_IRDETO_HOMINGCHANNEL) || defined(CONFIG_MW_CAS_IRDETO_RECOVERY_RESCAN_HOME)
STATIC HUINT8 xmgr_cas_ir_util_ConvertHomingChToPalPolar (IR_HOME_CHANNEL_POLAR_e eValue)
{
	switch (eValue)
	{
	case IR_HOME_CHANNEL_POLAR_H:
		return eDxSAT_POLAR_HOR;
	case IR_HOME_CHANNEL_POLAR_V:
		return eDxSAT_POLAR_VER;
	case IR_HOME_CHANNEL_POLAR_C_L:
		return eDxSAT_POLAR_LEFT;
	case IR_HOME_CHANNEL_POLAR_C_R:
		return eDxSAT_POLAR_RIGHT;
	}

	return eDxSAT_POLAR_AUTO;
}

STATIC HUINT8 xmgr_cas_ir_util_ConvertHomingChToPalFec (IR_HOME_CHANNEL_FEC_e eValue)
{
	switch (eValue)
	{
	case IR_HOME_CHANNEL_FEC_1_2:
		return eDxSAT_CODERATE_1_2;
	case IR_HOME_CHANNEL_FEC_2_3:
		return eDxSAT_CODERATE_2_3;
	case IR_HOME_CHANNEL_FEC_3_4:
		return eDxSAT_CODERATE_3_4;
	case IR_HOME_CHANNEL_FEC_5_6:
		return eDxSAT_CODERATE_5_6;
	case IR_HOME_CHANNEL_FEC_7_8:
		return eDxSAT_CODERATE_7_8;
	}

	return eDxSAT_CODERATE_AUTO;
}

STATIC HUINT8 xmgr_cas_ir_util_ConvertHomingChToPalTransspec (IR_HOME_CHANNEL_MOD_SYS_e eValue)
{
	switch (eValue)
	{
	case IR_HOME_CHANNEL_MOD_SYS_DVB_S:
		return eDxSAT_TRANS_DVBS;
	case IR_HOME_CHANNEL_MOD_SYS_DVB_S2:
		return eDxSAT_TRANS_DVBS2;
	}

	return eDxSAT_TRANS_DVBS;
}

STATIC HUINT8 xmgr_cas_ir_util_ConvertHomingChToPalRollOff (IR_HOME_CHANNEL_ROLL_OFF_e eValue)
{
	switch (eValue)
	{
	case IR_HOME_CHANNEL_ROLL_35:
		return eDxSAT_ROLL_035;
	case IR_HOME_CHANNEL_ROLL_25:
		return eDxSAT_ROLL_025;
	case IR_HOME_CHANNEL_ROLL_20:
		return eDxSAT_ROLL_020;
	}

	return eDxSAT_ROLL_035;
}

STATIC HUINT8	xmgr_cas_ir_util_ConvertHomingChToPalModType (IR_HOME_CHANNEL_MOD_TYPE_e eValue)
{
	switch (eValue)
	{
	case IR_HOME_CHANNEL_MOD_RSV:
		return eDxSAT_PSK_AUTO;
	case IR_HOME_CHANNEL_MOD_QPSK:
		return eDxSAT_PSK_QPSK;
	case IR_HOME_CHANNEL_MOD_8PSK:
		return eDxSAT_PSK_8PSK;
	case IR_HOME_CHANNEL_MOD_16QAM:		// 16 QAM (Not available for DVB-S2)
		return eDxSAT_PSK_AUTO;
	}

	return eDxSAT_PSK_AUTO;
}

STATIC HINT32 xmgr_cas_ir_util_CompareTuningInfoFromHomingChInfo(DbSvc_SatTuningInfo_t *pstTune1, DbSvc_SatTuningInfo_t *pstTune2)
{
	HINT32 nDiff;

	// Symbol Rate
	if (pstTune1->ulSymbolRate < pstTune2->ulSymbolRate)					{ return -1; }
	else if (pstTune1->ulSymbolRate > pstTune2->ulSymbolRate)				{ return  1; }

	//////////////// Frequency 비교 ////////////////
	//		45~18Ms/s	=	+/- (8MHz) search
	//		18 ~ 11Ms/s =	+/- (6MHz) search
	//		11 ~ 8Ms/s	=	+/- (5MHz) search
	//		8 ~ 4Ms/s	=	+/- (4MHz) search
	//		4 ~ 2Ms/s	=	+/- (3.8MHz) search
	//	=> 2003.09.09, 권기호 과장으로부터 입수

	// Frequency
	nDiff = (HINT32)pstTune1->ulFrequency - (HINT32)pstTune2->ulFrequency;
	if (pstTune1->ulSymbolRate >= 18000)
	{
		if (nDiff <= -8)					{ return -1; }
		else if (nDiff >= 8)				{ return  1; }
	}
	else if (pstTune1->ulSymbolRate >= 11000)
	{
		if (nDiff <= -6)					{ return -1; }
		else if (nDiff >= 6)				{ return  1; }
	}
	else if (pstTune1->ulSymbolRate >= 8000)
	{
		if (nDiff <= -5)					{ return -1; }
		else if (nDiff >= 5)				{ return  1; }
	}
/*
	else if (pstTune1->ulSymbolRate >= 4000)
	{
		if (nDiff <= -4)					{ return -1; }
		else if (nDiff >= 4)				{ return  1; }
	}
*/
	else if (pstTune1->ulSymbolRate >= 2000)
	{
		if (nDiff <= -4)					{ return -1; }
		else if (nDiff >= 4)				{ return  1; }
	}
	else
	{
		if (nDiff < 0)						{ return -1; }
		else if (nDiff > 0)					{ return  1; }
	}

	// Polarization
	if (pstTune1->ePolarization != eDxSAT_POLAR_AUTO && pstTune2->ePolarization != eDxSAT_POLAR_AUTO)
	{
		if (pstTune1->ePolarization < pstTune2->ePolarization)			{ return -1; }
		else if (pstTune1->ePolarization > pstTune2->ePolarization)		{ return  1; }
	}

	// FEC
	if (pstTune1->eFecCodeRate != eDxSAT_CODERATE_AUTO && pstTune2->eFecCodeRate != eDxSAT_CODERATE_AUTO)
	{
		if (pstTune1->eFecCodeRate < pstTune2->eFecCodeRate)							{ return -1; }
		else if (pstTune1->eFecCodeRate > pstTune2->eFecCodeRate)						{ return  1; }
	}

	// Transmission
	if (pstTune1->eTransSpec < pstTune2->eTransSpec)					{ return -1; }
	else if (pstTune1->eTransSpec > pstTune2->eTransSpec)				{ return  1; }

	// Modulation
	if (pstTune1->ePskMod != eDxSAT_PSK_AUTO && pstTune2->ePskMod != eDxSAT_PSK_AUTO)
	{
		if (pstTune1->ePskMod < pstTune2->ePskMod)		{ return -1; }
		else if (pstTune1->ePskMod > pstTune2->ePskMod)	{ return  1; }
	}

	// Pilot On Off
	if (pstTune1->ePilot != eDxSAT_PILOT_AUTO && pstTune2->ePilot != eDxSAT_PILOT_AUTO)
	{
		if (pstTune1->ePilot < pstTune2->ePilot)				{ return -1; }
		else if (pstTune1->ePilot > pstTune2->ePilot)			{ return  1; }
	}

	// Roll Off
	if (pstTune1->eRollOff < pstTune2->eRollOff)						{ return -1; }
	else if (pstTune1->eRollOff > pstTune2->eRollOff)					{ return  1; }

	// Same TP
	return 0;
}

STATIC HERROR xmgr_cas_ir_util_GetTuningInfoFromHomingChInfo(IR_SC_HOME_CHANNEL_INFO *pstHomingChInfo, DbSvc_SatTuningInfo_t *pstTuningParam)
{
	if (pstHomingChInfo == NULL)			{ return ERR_FAIL; }
	if (pstTuningParam == NULL)				{ return ERR_FAIL; }

	HxSTD_memset(pstTuningParam, 0, sizeof(DbSvc_TuningInfo_t));

	pstTuningParam->ulFrequency		 = pstHomingChInfo->dwFreq / 100;
	pstTuningParam->ulSymbolRate	 = pstHomingChInfo->dwSymbolRate / 10;
	pstTuningParam->ePolarization	 = xmgr_cas_ir_util_ConvertHomingChToPalPolar (pstHomingChInfo->ePolar);
	pstTuningParam->eFecCodeRate	 = xmgr_cas_ir_util_ConvertHomingChToPalFec (pstHomingChInfo->eFECInner);
	pstTuningParam->eTransSpec		 = xmgr_cas_ir_util_ConvertHomingChToPalTransspec (pstHomingChInfo->eModulSystem);
	pstTuningParam->eRollOff		 = xmgr_cas_ir_util_ConvertHomingChToPalRollOff (pstHomingChInfo->eRollOff);
	pstTuningParam->ePskMod			 = xmgr_cas_ir_util_ConvertHomingChToPalModType (pstHomingChInfo->eModulType);
	pstTuningParam->ePilot			 = (pstTuningParam->eTransSpec == eDxSAT_TRANS_DVBS2) ? eDxSAT_PILOT_AUTO : eDxSAT_PILOT_OFF;

	HxLOG_Print("\t\tFreq[%d]\n", pstTuningParam->ulFrequency);
	HxLOG_Print("\t\tSymbol[%d]\n", pstTuningParam->ulSymbolRate);
	HxLOG_Print("\t\tPolar[%d]\n", pstTuningParam->ePolarization);
	HxLOG_Print("\t\tFEC[%d]\n", pstTuningParam->eFecCodeRate);
	HxLOG_Print("\t\tTans[%d]\n", pstTuningParam->eTransSpec);
	HxLOG_Print("\t\tRoll[%d]\n", pstTuningParam->eRollOff);
	HxLOG_Print("\t\tPskMod[%d]\n", pstTuningParam->ePskMod);
	HxLOG_Print("\t\tPilot[%d]\n", pstTuningParam->ePilot);

	return ERR_OK;
}
STATIC HERROR	xmgr_cas_ir_util_UpdateHomingChannelInfo(IR_SC_HOME_CHANNEL_INFO *pstHomingChInfo)
{
	HERROR hErr;
	DbSvc_SatTuningInfo_t stTuningParam1, stTuningParam2;

	hErr = xmgr_cas_ir_util_GetTuningInfoFromHomingChInfo(pstHomingChInfo, &stTuningParam1);
	if(hErr != ERR_OK)
	{
		return hErr;
	}

	hErr = DB_PARAM_GetItem(MENUCFG_IR_HOMING_CHANNEL, (HUINT32 *)&stTuningParam2, 0);
	if(hErr != ERR_OK)
	{
		HxLOG_Critical("\n\n");
	}

	if(xmgr_cas_ir_util_CompareTuningInfoFromHomingChInfo(&stTuningParam1, &stTuningParam2) == 0)
	{
		return ERR_FAIL;
	}

	hErr = DB_PARAM_SetItem(MENUCFG_IR_HOMING_CHANNEL, sizeof(DbSvc_SatTuningInfo_t), (HUINT8 *)&stTuningParam2);
	if(hErr != ERR_OK)
	{
		HxLOG_Critical("\n\n");
	}

	return hErr;
}
#endif

#if defined(CONFIG_MW_CAS_IRDETO_HOMINGCHANNEL) || defined(CONFIG_MW_CAS_IRDETO_RECOVERY_RESCAN_HOME)
HERROR xmgr_cas_ir_util_ConvPolarIr2Db(IR_HOME_CHANNEL_POLAR_e ePolar, DxSat_Polarization_e *pePolar)
{
	switch(ePolar)
	{
		case IR_HOME_CHANNEL_POLAR_H:
			*pePolar = eDxSAT_POLAR_HOR;
			break;
		case IR_HOME_CHANNEL_POLAR_V:
			*pePolar = eDxSAT_POLAR_VER;
			break;
		case IR_HOME_CHANNEL_POLAR_C_L:
			*pePolar = eDxSAT_POLAR_LEFT;
			break;
		case IR_HOME_CHANNEL_POLAR_C_R:
			*pePolar = eDxSAT_POLAR_RIGHT;
			break;
		default:
			return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR xmgr_cas_ir_util_ConvCodeRateIr2Db(IR_HOME_CHANNEL_FEC_e eFec, DxSat_CodeRate_e *peCodeRate)
{
	switch(eFec)
	{
		case IR_HOME_CHANNEL_FEC_1_2:
			*peCodeRate = eDxSAT_CODERATE_1_2;
			break;
		case IR_HOME_CHANNEL_FEC_3_4:
			*peCodeRate = eDxSAT_CODERATE_3_4;
			break;
		case IR_HOME_CHANNEL_FEC_5_6:
			*peCodeRate = eDxSAT_CODERATE_5_6;
			break;
		case IR_HOME_CHANNEL_FEC_7_8:
			*peCodeRate = eDxSAT_CODERATE_7_8;
			break;
		default:
			*peCodeRate = eDxSAT_CODERATE_AUTO;
			break;
	}

	return ERR_OK;
}

HERROR xmgr_cas_ir_util_ConvRolloffIr2Db(IR_HOME_CHANNEL_ROLL_OFF_e eRolloff, DxSat_RollOff_e *peRolloff)
{
	switch(eRolloff)
	{
		case IR_HOME_CHANNEL_ROLL_20:
			*peRolloff = eDxSAT_ROLL_020;
			break;
		case IR_HOME_CHANNEL_ROLL_25:
			*peRolloff = eDxSAT_ROLL_025;
			break;
		case IR_HOME_CHANNEL_ROLL_35:
			*peRolloff = eDxSAT_ROLL_035;
			break;
		default:
			return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR xmgr_cas_ir_util_ConvPskModeIr2Db(IR_HOME_CHANNEL_MOD_TYPE_e eModType, DxSat_PskModulation_e *pePskMode)
{
	switch(eModType)
	{
		case IR_HOME_CHANNEL_MOD_RSV:
			*pePskMode = eDxSAT_PSK_AUTO;
			break;
		case IR_HOME_CHANNEL_MOD_QPSK:
			*pePskMode = eDxSAT_PSK_QPSK;
			break;
		case IR_HOME_CHANNEL_MOD_8PSK:
			*pePskMode = eDxSAT_PSK_8PSK;
			break;
		case IR_HOME_CHANNEL_MOD_16QAM:
			*pePskMode = eDxSAT_PSK_AUTO;
			break;
		default:
			return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR xmgr_cas_ir_util_ConvTransSpecIr2Db(IR_HOME_CHANNEL_MOD_SYS_e eModSys, DxSat_TransportSpec_e *peTransSpec)
{
	switch(eModSys)
	{
		case IR_HOME_CHANNEL_MOD_SYS_DVB_S:
			*peTransSpec = eDxSAT_TRANS_DVBS;
			break;
		case IR_HOME_CHANNEL_MOD_SYS_DVB_S2:
			*peTransSpec = eDxSAT_TRANS_DVBS2;
			break;
		default:
			return ERR_FAIL;
	}

	return ERR_OK;
}

#endif

#if defined (CONFIG_MW_CAS_IRDETO_PPV)
STATIC HBOOL	xmgr_cas_ir_util_ProcessPPVQuery(HUINT32 msg, HUINT8 *pData)
{
	switch(msg)
	{
		case	MSG_IPPV_BUY_QUERY:
			XSVC_CAS_IR_IPPV_Buy_Query(pData);
			break;

		case	MSG_IPPV_SECTOR_INFORMATION_QUERY:
			XSVC_CAS_IR_IPPV_Sector_Info_Query(pData);
			break;

		case	MSG_IPPV_SET_DEBIT_THRESHOLD:
			XSVC_CAS_IR_IPPV_Debit_Threshold(pData);
			break;

		case	MSG_IPPV_EVENT_LIST_QUERY:
			XSVC_CAS_IR_IPPV_Event_List_Query(pData);
			break;

		case	MSG_IPPV_EVENT_CHECK_QUERY:
			XSVC_CAS_IR_IPPV_Event_Check_Query(pData);
			break;

		case	MSG_IPPV_PHONE_NUMBER_QUERY:
			XSVC_CAS_IR_IPPV_Phone_Number_Query(pData);
			break;

		case	MSG_LPPV_PLACE_ORDER_QUERY:
			XSVC_CAS_IR_LPPV_Place_Order_Query(pData);
			break;

		case	MSG_LPPV_READ_ORDER_QUERY:
			XSVC_CAS_IR_LPPV_Read_Order_Query(pData);
			break;

		case	MSG_LPPV_READ_CREDIT_QUERY:
			XSVC_CAS_IR_LPPV_Read_Credit_Query(pData);
			break;

		case	MSG_LPPV_DELETE_TICKET_QUERY:
			XSVC_CAS_IR_LPPV_Delete_Ticket_Query(pData);
			break;

		case	MSG_LPPV_READ_TICKET_QUERY:
			XSVC_CAS_IR_LPPV_Read_Ticket_Query(pData);
			break;

		case	MSG_LPPV_AWARE_QUERY:
			XSVC_CAS_IR_LPPV_Aware_Query(pData);
			break;

		case	MSG_LPPV_CANCEL_ORDER_QUERY:
			XSVC_CAS_IR_LPPV_Cancel_Order_Query(pData);
			break;

		default:
			return FALSE;
	}

	return TRUE;
}

STATIC HBOOL	xmgr_cas_ir_util_ProcessPPVReply(HUINT32 msg, HUINT8 *pData)
{
	switch(msg)
	{
		case	MSG_IPPV_EVENT_NOTIFY:
			XMGR_CAS_IR_IPPV_EventNotify(pData);
			break;

		case	MSG_IPPV_BUY_REPLY:
			XMGR_CAS_IR_IPPV_IPPVBuyReply(pData);
			break;

		case	MSG_IPPV_SECTOR_INFORMATION_REPLY:
			XMGR_CAS_IR_IPPV_SectorInfoReply(pData);
			break;

		case	MSG_IPPV_SET_DEBIT_THRESHOLD:
			break;

		case	MSG_IPPV_CALLBACK_REQUEST:
			break;

		case	MSG_IPPV_EVENT_LIST_REPLY:
			XMGR_CAS_IR_IPPV_RequestEventListReply(pData);
			break;

		case	MSG_IPPV_EVENT_CHECK_REPLY:
			break;

		case	MSG_IPPV_PHONE_NUMBER_REPLY:
			break;

		case	MSG_LPPV_AWARE_REPLY:
			break;

		case	MSG_LPPV_EVENT_NOTIFY:
			XMGR_CAS_IR_LPPV_EventNotify(pData);
			break;

		case	MSG_LPPV_READ_TICKET_REPLY:
			XMGR_CAS_IR_LPPV_ReadTicketReply(pData);
			break;

		case	MSG_LPPV_DELETE_TICKET_REPLY:
			break;

		case	MSG_LPPV_READ_CREDIT_REPLY:
			break;

		case	MSG_LPPV_READ_ORDER_REPLY:
			XMGR_CAS_IR_LPPV_ReadOrderReply(pData);
			break;

		case	MSG_LPPV_PLACE_ORDER_REPLY:
			XMGR_CAS_IR_LPPV_PlaceOrderReply(pData);
			break;

		case	MSG_LPPV_CANCEL_ORDER_REPLY:
			XMGR_CAS_IR_LPPV_CancelOrderReply(pData);
			break;

		case	MSG_EMM_TLV_NOTIFY:
			XMGR_CAS_IR_LPPV_EMMTLV_Notify(pData);
			break;
	}

	if(pData)
		OxCAS_Free(pData);

	return TRUE;

}
#endif


/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/
#define _____XMGR_CAS_IR_PUBLIC_FUNCTION______________________________________________________
#if defined (CONFIG_MW_CAS_IRDETO_PPV)
 HBOOL	xmgr_cas_IrProcessPPVQuery(HUINT32 msg, HUINT8 *pData)
{
	return xmgr_cas_ir_util_ProcessPPVQuery(msg, pData);
}

HBOOL	xmgr_cas_IrProcessPPVReply(HUINT32 msg, HUINT8 *pData)
{
	return xmgr_cas_ir_util_ProcessPPVReply(msg, pData);
}
#endif

#if defined (CONFIG_MW_CAS_IRDETO_HOMINGCHANNEL) || defined(CONFIG_MW_CAS_IRDETO_RECOVERY_RESCAN_HOME)
HERROR	xmgr_cas_IrUpdateHomingChannelInfo(void *pvData)
{
	IR_SC_HOME_CHANNEL_INFO *pstHomingChInfo = (IR_SC_HOME_CHANNEL_INFO *)pvData;

	return xmgr_cas_ir_util_UpdateHomingChannelInfo(pstHomingChInfo);

}
#endif
HBOOL xmgr_cas_IrGetCardInOutStatus(HUINT8 ucSlotId)
{
	if(ucSlotId >= CAS_MAX_NUM_OF_SC)
		return FALSE;

	return s_bCardInserted[ucSlotId];
}

void xmgr_cas_IrSetCardInOutStatus(HUINT8 ucSlotId, HUINT8 bInserted)
{
	if(ucSlotId < CAS_MAX_NUM_OF_SC)
	{
		s_bCardInserted[ucSlotId] = bInserted;
	}
}


HBOOL xmgr_cas_IrGetNationalityAvailable(void)
{
	return s_bNationalityAvailable;
}

void xmgr_cas_IrSetNationalityAvailable(HBOOL bAvailable)
{
	s_bNationalityAvailable = bAvailable;
}

HERROR	xmgr_cas_IrInitialize(void)
{
	HERROR			hError = ERR_FAIL;
	HUINT8			aucNationalCode[4] = {0, };

	hError = DB_PARAM_GetItem(eDxSETUP_SYSTEM_IR_NATIONAL_CODE, (HUINT32 *)aucNationalCode, 4);

	if ((hError == ERR_OK) && (SvcCas_UtilStrLen(aucNationalCode) > 0))
	{
		xmgr_cas_IrSetNationalityAvailable(TRUE);
		return	ERR_OK;
	}
	else
	{
		HxLOG_Critical("\n\n");
	}

	return	ERR_FAIL;
}

#if defined(CONFIG_SUPPORT_IRDETO_AP_STATE)
void	xmgr_cas_IrInitApState(void)
{
	DxOperator_e eOperator;
	HERROR		 hErr;

	/* operator이 None일 경우는 SDT로 화면을 hide하지 않는다. */
	//ycgo - operator 가져 오는 부분으로 action ID가 없는 것을 고려 해야한다. 우선 주석처리함.
//	eOperator = MWC_PARAM_GetOperatorType();
	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_OPERATOR_TYPE, (HUINT32 *)&eOperator, 0);
	if (ERR_OK != hErr)
	{
		HxLOG_Warning("DB_PARAM_GetItem(eDB_PARAM_ITEM_OPERATOR_TYPE) Error!!\n");
	}
	if (eOperator == eDxOPERATOR_NOT_DEFINED)
	{
		s_eIrApRunningState = eRunningStateOK;
	}
	else
	{
		s_eIrApRunningState = eRunningStateWaitSdt;
	}

	s_eIrApSolState 	= eSolStateWaitSol;
	s_eIrApMRState 		= eMRStateWait;
	s_eIrApFTABlockState = eFTABlockStateOK;
}

HERROR	xmgr_cas_IrGetApState(XmgrCas_IrApState_e eIrApState,   HUINT32 *pulState)
{
	if(pulState == NULL || eIrApState >= eIrApStatusMax)
	{
		return ERR_FAIL;
	}

	switch(eIrApState)
	{
	case eIrApRunningState:
		*pulState = (HUINT32)s_eIrApRunningState;
		break;

	case eIrApSolState:
		*pulState = (HUINT32)s_eIrApSolState;
		break;

	case eIrApMaturityState:
		*pulState = (HUINT32)s_eIrApMRState;
		break;

	case eIrApFTABlockState:
		*pulState = (HUINT32)s_eIrApFTABlockState;
		break;

	default :
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR	xmgr_cas_IrSetApState(XmgrCas_IrApState_e eIrApState,   HUINT32 ulState)
{
	if(eIrApState >= eIrApStatusMax)
	{
		return ERR_FAIL;
	}

	switch(eIrApState)
	{
	case eIrApRunningState:
		if(ulState >= eRunningStateMax)
		{
			return ERR_FAIL;
		}
		s_eIrApRunningState = (XmgrCas_IrRunningState_e)ulState;
		break;

	case eIrApSolState:
		if(ulState >= eSolStateMax)
		{
			return ERR_FAIL;
		}
		s_eIrApSolState = (XmgrCas_IrSolState_e)ulState;

		break;

	case eIrApMaturityState:
		if(ulState >= eMRStateMax)
		{
			return ERR_FAIL;
		}
		s_eIrApMRState = (XmgrCas_IrMRState_e)ulState;

		break;

	case eIrApFTABlockState:
		if(ulState >= eFTABlockStateMax)
		{
			return ERR_FAIL;
		}
		s_eIrApFTABlockState = (eIrdetoFTABlockState)ulState;
		break;

	default :
		return ERR_FAIL;
	}

	return ERR_OK;
}

HINT8* xmgr_cas_IrGetStateStr(XmgrCas_IrApState_e eIrApState, HUINT32 ulState)
{
	switch(eIrApState)
	{
	case eIrApRunningState:
		switch(ulState)
		{
			ENUM_TO_STR(eRunningStateWaitSdt);
			ENUM_TO_STR(eRunningStateOK);
			ENUM_TO_STR(eRunningStateFail);
			ENUM_TO_STR(eRunningStateSdtTimeOut);
			default: break;
		}
		break;

	case eIrApSolState:
		switch(ulState)
		{
			ENUM_TO_STR(eSolStateWaitSol);
			ENUM_TO_STR(eSolStateOK);
			ENUM_TO_STR(eSolStateFail);
			ENUM_TO_STR(eSolStateTimeOut);
			default: break;
		}
		break;

	case eIrApMaturityState:
		switch(ulState)
		{
			ENUM_TO_STR(eMRStateWait);
			ENUM_TO_STR(eMRStateOK);
			ENUM_TO_STR(eMRStateFail);
			default: break;
		}
		break;
	default : break;
	}

	return "Unknown irState";
}

#if 0	// should be removed
HERROR	XMGR_CAS_IR_ProcessWtvState(WTV_AppState_t *peWtvState)
{
	WTV_AppState_t	eIrWtvState = eWtvState_MAX;
	HERROR			hRet = ERR_FAIL;

	if(peWtvState == NULL)
	{
		return hRet;
	}

#if defined(CONFIG_MW_CAS_IRDETO_SOL)
	if(s_eIrApSolState == eSolStateFail)
	{
		eIrWtvState = eWtvState_CAS;
	}
	else if(s_eIrApSolState == eSolStateWaitSol)
	{
		eIrWtvState = eWtvState_WAIT_RATING;
	}
	else
#endif

#if defined(CONFIG_MW_CAS_IRDETO_FTA_BLOCK)
	if(s_eIrApFTABlockState != eFTABlockStateOK)
	{
		eIrWtvState = eWtvState_SVC_FTA_BLOCK;
	}
	else
#endif

#if defined(CONFIG_MW_CAS_IRDETO_SDT_RUNNING_STATUS_BLOCK)
	// SDT가 running이 아닌 경우는 화면을 show하지 않는다.
	if(s_eIrApRunningState == eRunningStateFail)
	{
		eIrWtvState = eWtvState_SVC_NOT_RUNNING;
	}
	else if(s_eIrApRunningState == eRunningStateWaitSdt)
	{
		eIrWtvState = eWtvState_WAIT_RATING;
	}
	else
#endif

#if defined(CONFIG_MW_CAS_IRDETO_MATURITY_RATING)
#endif

	{	// if else 집단의 마지막 else 임 지우면 안 됨
		;
	}

	if(eIrWtvState != eWtvState_MAX)
	{
		hRet = ERR_OK;
	}

	*peWtvState = eIrWtvState;

	return hRet;
}
#endif

#if defined(CONFIG_MW_CAS_IRDETO_SOL)
HERROR	xmgr_cas_IrProcessSolState(Handle_t		 hAction, HUINT16 usOnId, HUINT16 usTsId)
{
	HUINT32 						ulSolState;
	HERROR						hRet = ERR_FAIL;
	SOL_SERVICE_STATE			eSolStatus;
	SvcCas_CtrlParam_t				stOutParam;
	IR_GetsolStatusOutParam_t		stSolStatus;

	stOutParam.ulControlType		= eCACTRL_IR_GetSolStatus;
	stOutParam.pvOut				= &stSolStatus;
	stSolStatus.hAction				= hAction;
	stSolStatus.ONID				= usOnId;
	stSolStatus.TSID				= usTsId;
	CASMGR_CTRL_Get(eDxCAS_GROUPID_IR, eCACTRL_IR_GetSolStatus, &stOutParam);

	eSolStatus = stSolStatus.eSolState;

	if(eSolStatus == SOL_VISIBLE_SERVICE)
	{
		ulSolState = eSolStateOK;
	}
	else if(eSolStatus == SOL_UNKNOWN_SERVICE)
	{
		ulSolState = eSolStateWaitSol;
	}
	else
	{
		ulSolState = eSolStateFail;
	}

	xmgr_cas_IrSetApState(eIrApSolState, ulSolState);

	return ERR_OK;
}
#endif

#if defined(CONFIG_MW_CAS_IRDETO_FTA_BLOCK)
HERROR	xmgr_cas_IrProcessFTABlockState(Handle_t hAction)
{
	eIrdetoFTABlockState 	ulFTABlockState;
	HERROR				hRet = ERR_FAIL;

	SvcCas_CtrlParam_t				stIrCtrlParam;
	IR_GetFtaBlockStatusOutParam_t	stGetFtaBlockStatusParam;

	stIrCtrlParam.ulControlType		= eCACTRL_IR_GetFTABlockStatus;
	stIrCtrlParam.pvOut			= &stGetFtaBlockStatusParam;
	stGetFtaBlockStatusParam.hAction	= hAction;
	CASMGR_CTRL_Get(eDxCAS_GROUPID_IR, eCACTRL_IR_GetFTABlockStatus, &stIrCtrlParam);
	ulFTABlockState = stGetFtaBlockStatusParam.unFtaBlockStatus;

	xmgr_cas_IrSetApState(eIrApFTABlockState, ulFTABlockState);

	return ERR_OK;
}
#endif

#if defined(CONFIG_MW_CAS_IRDETO_SDT_RUNNING_STATUS_BLOCK)
HERROR	xmgr_cas_IrSdtProcess(HUINT32 ulStatus)
{
	HUINT32 					ulSetRunningStatus;
	DxOperator_e				eOperator;
	SvcSi_SdtRunningStatusType_e	eRunningStatus = (SvcSi_SdtRunningStatusType_e)ulStatus;
	HERROR						hErr;

	if(eRunningStatus >= eSdtRunningStatusType_MAX)
	{
		return ERR_FAIL;
	}

	//ycgo - operator 가져 오는 부분으로 action ID가 없는 것을 고려 해야한다. 우선 주석처리함.
//	eOperator = MWC_PARAM_GetOperatorType();
	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_OPERATOR_TYPE, (HUINT32 *)&eOperator, 0);
	if (ERR_OK != hErr)
	{
		HxLOG_Warning("DB_PARAM_GetItem(eDB_PARAM_ITEM_OPERATOR_TYPE) Error!!\n");
	}
	/* operator이 None일 경우는 SDT로 화면을 hide하지 않는다. */
	if( (eOperator == eDxOPERATOR_None)
		|| (eOperator == eDxOPERATOR_NOT_DEFINED)
		|| (eRunningStatus == eSdtRunningStatusType_RUNNING)
		|| (eRunningStatus == eSdtRunningStatusType_TIMEOUT) )
	{
		// AV Block 해제
		ulSetRunningStatus	= eRunningStateOK;
	}
	else if(eRunningStatus == eSdtRunningStatusType_NOT_RUNNING)
	{
		// AV Block
		ulSetRunningStatus	= eRunningStateFail;
	}
	else
	{
		// 초기값 (문제되면 수정할 것)
		ulSetRunningStatus	= eRunningStateWaitSdt;
	}

	xmgr_cas_IrSetApState(eIrApRunningState, ulSetRunningStatus);

	return ERR_OK;
}
#endif

#endif

#if defined(CONFIG_MW_CAS_IRDETO_MATURITY_RATING)
HBOOL		xmgr_cas_IrCheckIsMrDisable(void)
{
	IR_API_SC_INFO					stIrApiScInfo;
	AXI_STRING_IDX					ErrStrIdx = 0;
	HIR_ERR							irError;
	SvcCas_CtrlParam_t					stOutParam;
	IR_GetSCInfoOutParam_t			stSCInfo;
	IR_GetStrIdxFromErrCodeOutParam_t	stGetStrIdx;

	stOutParam.ulControlType		= eCACTRL_IR_SC_Info;
	stOutParam.pvOut				= &stSCInfo;
	stSCInfo.ucScSlotId			= 0;
	stSCInfo.pstIrApiScInfo			= &stIrApiScInfo;
	irError = CASMGR_CTRL_Get(eDxCAS_GROUPID_IR, eCACTRL_IR_SC_Info, &stOutParam);
	if(irError == HIR_NOT_OK)
	{
		return TRUE;
	}

	HxLOG_Info("xmgr_cas_IrCheckIsMrDisable - bSCSupportMR = %d\n", stIrApiScInfo.stMRInfo.bSCSupportMR);

	if(stIrApiScInfo.stMRInfo.bSCSupportMR == FALSE)
	{
		return TRUE;
	}

	stOutParam.ulControlType		= eCACTRL_IR_DRV_GetStringIdxFromErrorCode;
	stOutParam.pvOut				= &stGetStrIdx;
	stGetStrIdx.pstIrStatus			= &stIrApiScInfo.stStatus;
	CASMGR_CTRL_Get(eDxCAS_GROUPID_IR, eCACTRL_IR_DRV_GetStringIdxFromErrorCode, &stOutParam);
	ErrStrIdx = stGetStrIdx.unStrIdx;

	switch (ErrStrIdx)
	{
		case AXI_STATUSCODE_IDX_E04_4:
		case AXI_STATUSCODE_IDX_E05_4:
		case AXI_STATUSCODE_IDX_E06_4:
		case AXI_STATUSCODE_IDX_I07_4:
			return TRUE;

		default:
			break;
	}

	return FALSE;
}

HERROR xmgr_cas_IrMrUpdatePin(Handle_t hAction, HUINT8 *szPinCodeStr)
{
	HUINT8						szDECPinCode[4];
	SvcCas_CtrlParam_t			stInParam;
	IR_MR_UpdatePinInParam_t	stUpdatePin;

	szDECPinCode[0] = HLIB_MATH_Bcd2Dec(GET_ASCII_NUM(szPinCodeStr[0]));
	szDECPinCode[1] = HLIB_MATH_Bcd2Dec(GET_ASCII_NUM(szPinCodeStr[1]));
	szDECPinCode[2] = HLIB_MATH_Bcd2Dec(GET_ASCII_NUM(szPinCodeStr[2]));
	szDECPinCode[3] = HLIB_MATH_Bcd2Dec(GET_ASCII_NUM(szPinCodeStr[3]));

	stUpdatePin.hAction			= hAction;
	VK_memcpy32(stUpdatePin.aucDecimalPIN, szDECPinCode, 4);

	stInParam.ulControlType 	= eCACTRL_IR_MR_UpdatePIN;
	stInParam.pvIn 				= &stUpdatePin;

	return CASMGR_CTRL_Call(eDxCAS_GROUPID_IR, eCACTRL_IR_MR_UpdatePIN, &stInParam);
}
#endif

#if defined(CONFIG_MW_CAS_IRDETO_HOMINGCHANNEL) || defined(CONFIG_MW_CAS_IRDETO_RECOVERY_RESCAN_HOME)
HERROR XMGR_CAS_IrGetSmcHomeTuneInfo(DbSvc_TuningInfo_t *pstTuningInfo)
{
	HERROR						hErr;
	IR_SC_HOME_CHANNEL_INFO		stHomeChannel;

	if (pstTuningInfo == NULL)
	{
		return ERR_FAIL;
	}

	hErr = xsvc_cas_IrGetHomeChannelInfo(&stHomeChannel);

	if( (hErr == ERR_OK) && (stHomeChannel.dwFreq != 0) )
	{
		pstTuningInfo->sat.ulFrequency		= (stHomeChannel.dwFreq / 100);
		pstTuningInfo->sat.ulSymbolRate		= (stHomeChannel.dwSymbolRate / 10);
		hErr = xmgr_cas_ir_util_ConvPolarIr2Db(stHomeChannel.ePolar, &pstTuningInfo->sat.ePolarization);
		if(hErr != ERR_OK)			HxLOG_Critical("Critical Eror!!!\n");
		hErr = xmgr_cas_ir_util_ConvCodeRateIr2Db(stHomeChannel.eFECInner, &pstTuningInfo->sat.eFecCodeRate);
		if(hErr != ERR_OK)			HxLOG_Critical("Critical Eror!!!\n");
		hErr = xmgr_cas_ir_util_ConvRolloffIr2Db(stHomeChannel.eRollOff, &pstTuningInfo->sat.eRollOff);
		if(hErr != ERR_OK)			HxLOG_Critical("Critical Eror!!!\n");
		hErr = xmgr_cas_ir_util_ConvPskModeIr2Db(stHomeChannel.eModulType, &pstTuningInfo->sat.ePskMod);
		if(hErr != ERR_OK)			HxLOG_Critical("Critical Eror!!!\n");
		hErr = xmgr_cas_ir_util_ConvTransSpecIr2Db(stHomeChannel.eModulSystem, &pstTuningInfo->sat.eTransSpec);
		if(hErr != ERR_OK)			HxLOG_Critical("Critical Eror!!!\n");

		/* Position정보 사용이 필요할 경우 재고려 필요 */
		//stHomeChannel.wOrbitPos;
		//stHomeChannel.eWestEastFlag;
		//stHomeChannel.eMuxVerifierMode;

		return ERR_OK;
	}

	return ERR_FAIL;
}
#endif


HERROR xmgr_cas_IrScInfo(HUINT8 ucSlotId, void *pvData)
{
	HERROR						hErr;
	SvcCas_CtrlParam_t			stOutParam;
	IR_API_SC_INFO				stIrApiScInfo, *pstScInfo = (IR_API_SC_INFO *)pvData;
	IR_GetSCInfoOutParam_t		stSCInfo;

	if(pstScInfo == NULL)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	stOutParam.ulControlType	= eCACTRL_IR_SC_Info;
	stOutParam.pvOut 			= &stSCInfo;
	stSCInfo.ucScSlotId			= ucSlotId;
	stSCInfo.pstIrApiScInfo 	= &stIrApiScInfo;

	hErr = svc_cas_CtrlGet(eDxCAS_GROUPID_IR, eCACTRL_IR_SC_Info, &stOutParam);

	if(hErr == ERR_OK)
	{
		HxSTD_memcpy(pstScInfo, &stIrApiScInfo, sizeof(IR_API_SC_INFO));
	}

	return hErr;
}


