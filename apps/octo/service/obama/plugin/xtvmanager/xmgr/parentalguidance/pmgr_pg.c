/**
	@file     	pmgr_pg.c
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
#include <db_param.h>
#include <bus.h>
#include <mgr_common.h>
#include <mgr_pg.h>
#include "./local_include/_xmgr_pg.h"

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


HUINT32 pmgr_pg_GetRatingFromSetup (void)
{
	return xmgr_pg_GetRatingFromSetup_Base();
}

HUINT32 pmgr_pg_GetRecordRatingFromSetup(void)
{
	return xmgr_pg_GetRecordRatingFromSetup_Base();
}

/* Rating setup ���� content rating�� ���Ͽ� parantal block ���θ� ����. */
HBOOL pmgr_pg_IsRatingOK(HUINT32 ulActionId, Handle_t hService, DxDeliveryType_e svcDeliveryType, DxRatingAge_e eRatingSetup, DxRatingAge_e eContentRating)
{
	return xmgr_pg_IsRatingOK_Base(ulActionId, hService, svcDeliveryType, eRatingSetup, eContentRating);
}

// �� �Լ��� Return ���� ���� Watch TV�� State�� eWtvState_WAIT_RATING ����� �� �ִ�.
HBOOL pmgr_pg_IsWaitParentalRatingState(MgrPg_Info_t *pstPgInfo)
{
	return xmgr_pg_IsWaitParentalRatingState_Base(pstPgInfo);
}

HUINT32 pmgr_pg_GetRatingForNoEit(DxSvcType_e eSvcType, HBOOL bEitValid)
{
#if defined(CONFIG_OP_ZIGGO)
	return xmgr_pg_GetNoEitRatingValue_Ziggo(eSvcType, bEitValid);
#elif defined(CONFIG_OP_MIDDLE_EAST)
	return xmgr_pg_GetNoEitRatingValue_MeSat(eSvcType, bEitValid);
#else
	return xmgr_pg_GetNoEitRatingValue_Base(eSvcType, bEitValid);
#endif
}


/*********************** End of File ******************************/
