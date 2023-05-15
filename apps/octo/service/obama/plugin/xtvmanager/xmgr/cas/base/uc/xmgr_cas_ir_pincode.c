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
		//ycgo �켱 compile
		//MWC_UTIL_DvbStrcat(pPincodeStr, (HUINT8*)GetStrRsc(LOC_CAS_IR_MSG_NOT_EIT));
	}
	else	// E42
	{
		MWC_UTIL_DvbStrcpy(pPincodeStr, (HUINT8 *)"E42-32  ");
		//ycgo �켱 compile
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
// Nationality ���޾����� Wait Rating State
	if (bNationalityAvailable != TRUE)
	{
		return TRUE;
	}
#endif

	// CAS State�̸� Wait Rating State
	if ( (pstPgInfo->eCasType == eDxCAS_TYPE_IRDETO) && (pstPgInfo->bCasOk != TRUE) )
	{
		return TRUE;
	}

	// Rating ������ ���޾����� Wait Rating State
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
	//	Rating ALL VIEW�� �̷��� �ʿ���ڳ�...
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

	//	Total Block�̸� ������ ���������..
	if (eRatingSetup == eDxRATING_AGE_LOCK_ALL)
		return FALSE;

	HxLOG_Print("\t=> eRatingSetup (0x%x), eContentRating (0x%x)\n", eRatingSetup, eContentRating);

#if 0 // TODO: �ʿ�� core ������ ������ ��
	/* any public�� age 4�� ������ ����. �� ��� ���Ǹ� ���� age 4�� ��ȯ�ؼ� �������... */
	if(eRatingSetup == eDxRATING_AGE_ANY_PUBLIC)
	{
		eRatingSetup = eDxRATING_AGE_4;
	}
#endif

	if(eRatingSetup == eDxRATING_AGE_LOCK_ALL)
	{ /* Lock all������ ������ block */
		return FALSE;
	}
	else if(eRatingSetup == eDxRATING_VIEW_ALL)
	{ /* View all���������� ������ �� �� �ִ�. */
		return TRUE;
	}
	else
	{ /* �� �ܿ��� ���ؾ� �ϴµ�... */
#if defined(CONFIG_MW_CAS_IRDETO) //Ir spec������ EIT�� rating value�� ���� ��� watch_tv���� lock_all�� �����ؼ� �����Ƿ�..
		if(eContentRating > 0xf && eContentRating != 0x7f)
#else
		if(eContentRating > 0xf)
#endif
		{ /* �� �� 0xf �̻� ���� spec �����̹Ƿ� �ɷ�����. */
			return TRUE;
		}

#if defined(CONFIG_APCORE_BLOCK_EQUAL_RATING)
		if(eContentRating >= eRatingSetup) /* Content�� rating�� �¾����� ���ų� ũ�Ƿ� PIN code check �ؾ� ��. */
#else
		if(eContentRating > eRatingSetup) /* Content�� rating�� �¾����� ũ�Ƿ� PIN code check �ؾ� ��. */
#endif
		{
			HxLOG_Print("\teContentRating > eRatingSetup, : return FALSE\n");
			return FALSE;
		}
		else
		{ /* Content�� rating�� �¾����� ���ų� �۴�. OK to watch. */
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


/* Irdeto Spec�� ����  parantal block ���θ� ����. */
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



