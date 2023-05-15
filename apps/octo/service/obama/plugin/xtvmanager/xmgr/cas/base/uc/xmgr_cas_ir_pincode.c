/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#ifdef DxMAX_TS_NUM
#undef DxMAX_TS_NUM
#endif
#include "local_include/_xmgr_cas_ir.h"
#include "local_include/_xmgr_cas_ir_adapt.h"
#include "local_include/_xmgr_cas_ir_util.h"
#include "local_include/_xmgr_cas_ir_ui.h"
#include <mgr_action.h>
#include <db_param.h>
#include <stb_params.h>
#include <svc_pipe.h>
#if defined(CONFIG_MW_MM_PVR)
#endif
#include <svc_si.h>
#include <svc_cas.h>
#include <mgr_pg.h>

#include <ctype.h>	/* for toupper(...) */
#include <xmgr_cas.h>


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/





/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define	BLOCK_UNIXTIME					(5*60L)
#define	MAX_RETRY_PININPUT				3
#define NUM_OF_CARD_SLOTS				3
#define MAX_SMC_ID_LENGTH				20
#define DEF_FAILURE_COUNTER_NULL		0xFF


/*******************************************************************/
/********************      Structure       *************************/
/*******************************************************************/
typedef struct tagPCPinInfo
{
	UNIXTIME	retryTime;
	HUINT8		nRetryInput;
	HUINT8		fAccess;
	HUINT8		aucFutureReserv[2];
	HUINT8 		aucSMCId[MAX_SMC_ID_LENGTH];
} IR_UI_PCPININFO;


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/
IR_UI_PCPININFO 		s_pinInfo[NUM_OF_CARD_SLOTS];

STATIC HBOOL			s_SiTableIsTimeOut = 0;


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/



/*******************************************************************/
/********************      Local Functions      ********************/
/*******************************************************************/
#define _____XMGR_CAS_IR_PINCODE_LOCAL_FUNCTION______________________________________________________

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/
#define _____XMGR_CAS_IR_PINCODE_PUBLIC_FUNCTION______________________________________________________


HINT32	xmgr_cas_IrGetRatingErrorCode (void)
{
	if( s_SiTableIsTimeOut )	// E44
		return IRUC_STATUS_MSG_E044_32;
	return IRUC_STATUS_MSG_E042_32;
}

void	xmgr_cas_IrGetParentalRatingErrorCode(HUINT8 *pPincodeStr)
{
	if( s_SiTableIsTimeOut )	// E44
	{
		MWC_UTIL_DvbStrcpy(pPincodeStr, (HUINT8 *)"E44-32  ");
		//ycgo 우선 compile
		//MWC_UTIL_DvbStrcat(pPincodeStr, (HUINT8*)GetStrRsc(LOC_CAS_IR_MSG_NOT_EIT));
	}
	else	// E42
	{
		MWC_UTIL_DvbStrcpy(pPincodeStr, (HUINT8 *)"E42-32  ");
		//ycgo 우선 compile
		//MWC_UTIL_DvbStrcat(pPincodeStr, (HUINT8*)GetStrRsc(LOC_CAS_IR_MSG_PARENTAL_LOCK));
	}
}


void	XMGR_CAS_IrSetSiTableStateTimeout(HBOOL siTableState)
{
	s_SiTableIsTimeOut = siTableState;
}

HBOOL	XMGR_CAS_IrIsWaitParentalRatingState(MgrPg_Info_t *pstPgInfo)
{
	HBOOL		bNationalityAvailable;

	if (pstPgInfo == NULL)
	{
		return FALSE;
	}

	bNationalityAvailable = xmgr_cas_IrGetNationalityAvailable();

	HxLOG_Print(">> s_bNationalityAvailable [%d]\n", bNationalityAvailable);
	HxLOG_Print(">> eCasType [%d]\n", pstPgInfo->eCasType );
	HxLOG_Print(">> bCasOk [%d]\n", pstPgInfo->bCasOk);
	HxLOG_Print(">> bRatingValid [%d]\n", pstPgInfo->bRatingValid);



#if !defined(CONFIG_OP_MIDDLE_EAST)	// not use Nationality flag because not need compare nationality code in MiddleEast
// Nationality 못받았으면 Wait Rating State
	if (bNationalityAvailable != TRUE)
	{
		return TRUE;
	}
#endif

	// CAS State이면 Wait Rating State
	if ( (pstPgInfo->eCasType == eDxCAS_TYPE_IRDETO) && (pstPgInfo->bCasOk != TRUE) )
	{
		return TRUE;
	}

	// Rating 정보를 못받았으면 Wait Rating State
	if (pstPgInfo->bRatingValid != TRUE)
	{
		return TRUE;
	}

	return FALSE;
}

STATIC HBOOL local_cas_ir_SkipRatingCheckForPVR(HUINT32 ulActionId, Handle_t hService)
{
	return FALSE;
}

STATIC HBOOL local_cas_ir_SkipRatingCheck(HUINT32 ulActionId, Handle_t hService)
{
	return FALSE;
}

STATIC	HUINT8		_aucCountryCode[4] = "";

HERROR	xmgr_cas_IrSetCountryCodeFromEIT(HUINT8 *aucCountryCode)
{
	HxSTD_memcpy(_aucCountryCode, aucCountryCode, sizeof(HUINT8) * 4);

	return ERR_OK;
}

STATIC HBOOL local_cas_ir_IsRatingOK(HBOOL bPvrMode, HUINT32 ulActionId, Handle_t hService, DxDeliveryType_e svcDeliveryType, DxRatingAge_e eRatingSetup, DxRatingAge_e eContentRating)
{
	//	Rating ALL VIEW면 이런거 필요없자네...
	if (eRatingSetup == eDxRATING_VIEW_ALL)
		return TRUE;

#if defined(CONFIG_MW_MM_PVR)
	if (bPvrMode)
	{
		// Skip parental rating chekc for specific service type
		if (local_cas_ir_SkipRatingCheckForPVR(ulActionId, hService))
			return TRUE;
	}
	else
#endif
	{
		// Skip parental rating chekc for specific service type
		if (local_cas_ir_SkipRatingCheck(ulActionId, hService))
			return TRUE;
	}

	//	Total Block이면 무조건 물어봐야함..
	if (eRatingSetup == eDxRATING_AGE_LOCK_ALL)
		return FALSE;

	HxLOG_Print("\t=> eRatingSetup (0x%x), eContentRating (0x%x)\n", eRatingSetup, eContentRating);

#if 0 // TODO: 필요시 core 팀에게 문의할 것
	/* any public은 age 4와 동작이 같다. 비교 계산 편의를 위해 age 4로 변환해서 사용하자... */
	if(eRatingSetup == eDxRATING_AGE_ANY_PUBLIC)
	{
		eRatingSetup = eDxRATING_AGE_4;
	}
#endif

	if(eRatingSetup == eDxRATING_AGE_LOCK_ALL)
	{ /* Lock all에서는 무조건 block */
		return FALSE;
	}
	else if(eRatingSetup == eDxRATING_VIEW_ALL)
	{ /* View all설정에서는 무조건 볼 수 있다. */
		return TRUE;
	}
	else
	{ /* 그 외에는 비교해야 하는데... */
#if defined(CONFIG_MW_CAS_IRDETO) //Ir spec에서는 EIT의 rating value가 없을 경우 watch_tv에서 lock_all로 설정해서 들어오므로..
		if(eContentRating > 0xf && eContentRating != 0x7f)
#else
		if(eContentRating > 0xf)
#endif
		{ /* 그 외 0xf 이상 값은 spec 위반이므로 걸러낸다. */
			return TRUE;
		}

#if defined(CONFIG_APCORE_BLOCK_EQUAL_RATING)
		if(eContentRating >= eRatingSetup) /* Content의 rating이 셋업보다 같거나 크므로 PIN code check 해야 함. */
#else
		if(eContentRating > eRatingSetup) /* Content의 rating이 셋업보다 크므로 PIN code check 해야 함. */
#endif
		{
			HxLOG_Print("\teContentRating > eRatingSetup, : return FALSE\n");
			return FALSE;
		}
		else
		{ /* Content의 rating이 셋업보다 같거나 작다. OK to watch. */
			return TRUE;
		}
	}

	return FALSE;
}


STATIC HBOOL local_cas_ir_checkActionIdToForPVRSiParentalRating(HUINT32 ulActionId)
{
#if defined(CONFIG_MW_MM_PVR)
	MgrAction_Type_e	eActionType = MGR_ACTION_GetType(ulActionId);

	if (eActionType == eViewType_TSR_PB || eActionType == eViewType_CHASE_PB)
	{
		return TRUE;
	}

	else if (ulActionId >= eActionId_REC_FIRST && ulActionId <= eActionId_REC_LAST)
	{
		return TRUE;
	}
#endif

	return FALSE;
}

STATIC HBOOL local_cas_ir_checkActionIdToForLiveSiParentalRating(HUINT32 ulActionId)
{
	MgrAction_Type_e	eActionType = MGR_ACTION_GetType(ulActionId);

	if (eActionType == eViewType_WTV)
	{
		return TRUE;
	}

	return FALSE;
}


/* Irdeto Spec에 따라  parantal block 여부를 판정. */
HBOOL	XMGR_CAS_IrIsRatingOK(HUINT32 ulActionId, Handle_t hService, DxDeliveryType_e svcDeliveryType, DxRatingAge_e eRatingSetup, DxRatingAge_e eContentRating)
{
	XMGR_CAS_IrSetSiTableStateTimeout(FALSE);

	// REC_0, REC_1, TSRREC, TSR_PB, CHASE_PB
	if (local_cas_ir_checkActionIdToForPVRSiParentalRating(ulActionId))
	{
		return local_cas_ir_IsRatingOK(TRUE, ulActionId, hService, svcDeliveryType, eRatingSetup, eContentRating);
	}
	// WTV
	else if (local_cas_ir_checkActionIdToForLiveSiParentalRating(ulActionId))
	{
		return local_cas_ir_IsRatingOK(FALSE, ulActionId, hService, svcDeliveryType, eRatingSetup, eContentRating);
	}
	else
	{
		return FALSE;
	}

	return FALSE;
}



