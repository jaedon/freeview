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
	{ /* Lock all������ ������ block */
		return FALSE;
	}
	else if(eRatingSetup == eDxRATING_VIEW_ALL)
	{ /* View all���������� ������ �� �� �ִ�. */
		return TRUE;
	}
	else
	{ /* �� �ܿ��� ���ؾ� �ϴµ�... */
		if(eContentRating > eDxRATING_AGE_18) /* �� �� eDxRATING_AGE_18 �̻� ���� spec �����̹Ƿ� �ɷ�����. */
		{
			return TRUE;
		}

#if defined(CONFIG_APCORE_BLOCK_EQUAL_RATING)
		if(eContentRating >= eRatingSetup) /* Content�� rating�� �¾����� ���ų� ũ�Ƿ� PIN code check �ؾ� ��. */
#else
		if(eContentRating > eRatingSetup) /* Content�� rating�� �¾����� ũ�Ƿ� PIN code check �ؾ� ��. */
#endif
		{
			return FALSE;
		}
		else
		{ /* Content�� rating�� �¾����� ���ų� �۴�. OK to watch. */
			return TRUE;
		}
	}
}

/* Rating setup ���� content rating�� ���Ͽ� parantal block ���θ� ����. */
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


// �� �Լ��� Return ���� ���� Watch TV�� State�� eWtvState_WAIT_RATING ����� �� �ִ�.
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

	/* CAS ä�ο��� CAS OK�� �ȿ԰ų� ���� rating ������ �������� �ʾҴ�.
	   Rating check�� ������ ������.
	   CAS���� ��� ������ parental ���� ���� Ȯ�� �ؾ� �ϰ�, EIT��� rating ������ �޾ƿ�����, EIT timeout�� ���� ��. */

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
