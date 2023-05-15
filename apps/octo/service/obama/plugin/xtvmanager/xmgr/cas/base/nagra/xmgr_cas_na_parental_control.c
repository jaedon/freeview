/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#include <mgr_pg.h>

#include <namgr_main.h>

#include "local_include/_xmgr_cas_na_util.h"
#include "local_include/_xmgr_cas_na_adapt.h"


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/



/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/



/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/



/*******************************************************************/
/********************      Structure       *************************/
/*******************************************************************/



/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/



/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/



/*******************************************************************/
/********************      Local Functions      ********************/
/*******************************************************************/
#define _____XMGR_CAS_NA_PARENTAL_CONTROL_LOCAL_FUNCTION______________________________________________________



/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/
#define _____XMGR_CAS_NA_PARENTAL_CONTROL_PUBLIC_FUNCTION______________________________________________________
HERROR xmgr_cas_Na_SetParentalControlLevel(void)
{
	SvcCas_CtrlParam_t stParam;

	if(SVC_CAS_CtrlSet(eDxCAS_GROUPID_NA, eNaCtrl_SetParentalControlLevel, &stParam) != ERR_OK)
	{
		HxLOG_Error("SVC_CAS_CtrlSet(eNaCtrl_SetParentalControlLevel) error\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR xmgr_cas_Na_SetParentalControl(Handle_t hAction)
{
	SvcCas_CtrlParam_t				stParam;
	NA_SetParentalControlInParam_t	stInParam;

	stInParam.xTsId			= hAction;
	stInParam.bUserPinPass	= TRUE;

	stParam.pvIn			= &stInParam;

	if(SVC_CAS_CtrlSet(eDxCAS_GROUPID_NA, eNaCtrl_SetParentalControl, &stParam) != ERR_OK)
	{
		HxLOG_Error("SVC_CAS_CtrlSet(eNaCtrl_SetParentalControl) error\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

HBOOL xmgr_cas_Na_IsWaitParentalRatingState(MgrPg_Info_t *pstPgInfo)
{
	/*	NAGRA CAS의 PARENTAL CONTROL을 사용할 경우 RATING 정보를 기다리지 않도록 한다.
		(NAGRA CAS SYSTEM에 의한 RATING CONTROL만을 사용)
		추후, EIT 등의 RATING 정보와 함께 사용해야 하는 MODEL의 경우는 시나리오를 고려하자. */

	return (pstPgInfo->bRatingValid == FALSE ? TRUE : FALSE);
}

HBOOL xmgr_cas_Na_IsRatingOK(HUINT32 ulActionId, DxRatingAge_e eRatingSetup)
{
	/*	NAGRA CAS의 PARENTAL CONTROL을 사용할 경우 RATING 정보를 기다리지 않도록 한다.
		(NAGRA CAS SYSTEM에 의한 RATING CONTROL만을 사용)
		추후, EIT 등의 RATING 정보와 함께 사용해야 하는 MODEL의 경우는 시나리오를 고려하자. */

	HBOOL bRatingOK = TRUE;

	if(MGR_ACTION_GetMainActionId() == ulActionId)
	{
		if(eRatingSetup == eDxRATING_AGE_LOCK_ALL)
			bRatingOK = FALSE;

		if(xmgr_cas_Na_GetAlarmType() == eCAS_NA_CA_ACCESS_DENIED_PARENTAL_CONTROL)
			bRatingOK = FALSE;
	}

	return bRatingOK;
}

/* end of file */
