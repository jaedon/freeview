/**
	@file     	xmgr_pg_base.c
	@brief

	Description:  			\n
	Module: 					 					\n
	Remarks : 										\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#include <bus.h>
#include <mgr_common.h>
#include <mgr_pg.h>
#include <mgr_cas.h>
#include "../local_include/_xmgr_pg.h"
#if defined(CONFIG_MW_CAS_VERIMATRIX_IPTV)
#include "../../cas/base/verimatrix/local_include/_xmgr_cas_vmx.h"
#endif

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/


HUINT32 xmgr_pg_GetRatingFromSetup_Base (void)
{
	HERROR			hErr;
	DxRatingAge_e	eParentalSetup;
#if defined(CONFIG_OP_SES)
{
	HULONG	utTime;
	HxDATETIME_Time_t	time;

	if(VK_CLOCK_GetTime(&utTime) == VK_OK)
	{
		HLIB_DATETIME_ConvertUnixTimeToTime(utTime, &time);
		if( (time.ucHour >= 6) && (time.ucHour < 22) )
		{
			//VK_Print("APS rating spec(6:00~22:00) : Rating setup is 16\n");
			return eDxRATING_AGE_16;
		}
		else if( (time.ucHour >= 22) && (time.ucHour < 23) )
		{
			//VK_Print("APS rating spec(22:00~23:00) : Rating setup is 18\n");
			return eDxRATING_AGE_18;
		}
		else /* (time.ucHour >= 23) || (time.ucHour < 6) */
		{
			//VK_Print("APS rating spec(23:00~6:00) : Rating setup is VIEW ALL\n");
			return eDxRATING_VIEW_ALL;
		}
	}
	else
	{
		VK_Print("Time is NOT available. Rating setup is 16)\n");
		return eDxRATING_AGE_16;
	}
	NOT_USED_PARAM(hErr);
}
#else
	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_MATURITYRATING, (HUINT32 *)&eParentalSetup, 0);
	if (ERR_OK != hErr)
	{
		HxLOG_Warning("DB_PARAM_GetItem(eDB_PARAM_ITEM_MATURITYRATING) Error!!!\n");
	}
#endif
	return (HUINT32)eParentalSetup;
}

HUINT32 xmgr_pg_GetRecordRatingFromSetup_Base (void)
{
#if defined(CONFIG_MW_MM_PVR)
	HERROR			hErr;
	DxRatingAge_e	eParentalSetup;

#if defined(CONFIG_OP_SES)
	eParentalSetup = eDxRATING_AGE_16;
	NOT_USED_PARAM(hErr);
#else
	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_MATURITYRATING, (HUINT32 *)&eParentalSetup, 0);
	if (ERR_OK != hErr)
	{
		HxLOG_Warning("DB_PARAM_GetItem(eDB_PARAM_ITEM_MATURITYRATING) Error!!!\n");
	}
#endif
	return (HUINT32)eParentalSetup;
#else
	return 0;
#endif
}

STATIC HBOOL xmgr_pg_CheckRatingOK_Base(DxDeliveryType_e svcDeliveryType, DxRatingAge_e eRatingSetup, DxRatingAge_e eContentRating)
{
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
		if(eContentRating > eDxRATING_AGE_18) /* 그 외 eDxRATING_AGE_18 이상 값은 spec 위반이므로 걸러낸다. */
		{
			return TRUE;
		}

#if defined(CONFIG_APCORE_BLOCK_EQUAL_RATING)
		if(eContentRating >= eRatingSetup) /* Content의 rating이 셋업보다 같거나 크므로 PIN code check 해야 함. */
#else
		if(eContentRating > eRatingSetup) /* Content의 rating이 셋업보다 크므로 PIN code check 해야 함. */
#endif
		{
			return FALSE;
		}
		else
		{ /* Content의 rating이 셋업보다 같거나 작다. OK to watch. */
			return TRUE;
		}
	}
}

/* Rating setup 값과 content rating을 비교하여 parantal block 여부를 판정. */
HBOOL xmgr_pg_IsRatingOK_Base(HUINT32 ulActionId, Handle_t hService, DxDeliveryType_e svcDeliveryType, DxRatingAge_e eRatingSetup, DxRatingAge_e eContentRating)
{
#if defined(CONFIG_MW_CAS) || defined(CONFIG_MW_CI)
	if (MGR_CAS_NeedCasRatingCheck() == TRUE)
	{
		return MGR_CAS_IsRatingOK(ulActionId, hService, svcDeliveryType, eRatingSetup, eContentRating);
	}
#endif

	return xmgr_pg_CheckRatingOK_Base(svcDeliveryType, eRatingSetup, eContentRating);
}


// 이 함수의 Return 값에 따라 Watch TV의 State가 eWtvState_WAIT_RATING 변경될 수 있다.
HBOOL xmgr_pg_IsWaitParentalRatingState_Base(MgrPg_Info_t *pstPgInfo)
{
	if (pstPgInfo == NULL)
	{
		return FALSE;
	}

#if defined(CONFIG_MW_CAS) || defined(CONFIG_MW_CI)
	if (MGR_CAS_NeedCasRatingWaitCheck() == TRUE)
	{
		return MGR_CAS_IsWaitParentalRatingState(pstPgInfo);
	}
#endif

	/* CAS 채널에서 CAS OK가 안왔거나 아직 rating 정보가 결졍되지 않았다.
	   Rating check할 정보가 부족함.
	   CAS에서 계약 사항을 parental 보다 먼저 확인 해야 하고, EIT등에서 rating 정보를 받아오던가, EIT timeout이 나야 함. */

	if ( (pstPgInfo->eCasType != eDxCAS_TYPE_FTA) && (pstPgInfo->bCasOk != TRUE) )
	{
		return TRUE;
	}

	if (pstPgInfo->bRatingValid != TRUE)
	{
		return TRUE;
	}

	return FALSE;
}

HUINT32 xmgr_pg_GetNoEitRatingValue_Base(DxSvcType_e eSvcType, HBOOL bEitValid)
{
#if defined(CONFIG_MW_CAS_IRDETO)
	return eDxRATING_AGE_LOCK_ALL;
#else
	return eDxRATING_VIEW_ALL;
#endif
}
/*********************** End of File ******************************/
