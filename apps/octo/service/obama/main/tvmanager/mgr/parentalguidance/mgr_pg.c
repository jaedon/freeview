/**
	@file     mgr_tplist.c
	@brief

	Description :			\n
	Module :				\n
	Remarks :				\n
	 Date			Modification				Name\n
	-----------	-------------------------------------	------------------\n
	 2013/04/16		init							\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/


/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

#include <string.h>

#include <db_svc.h>
#include <mgr_pg.h>
#include <pmgr_pg.h>
#include "./local_include/_mgr_pg.h"

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
/********************      Functions            ********************/
/*******************************************************************/

/* Parental Rating Setup Value Reference */
HUINT32	MGR_PG_GetRatingFromSetup (void)
{
	return pmgr_pg_GetRatingFromSetup();
}

HUINT32	MGR_PG_GetRecordRatingFromSetup (void)
{
	return pmgr_pg_GetRecordRatingFromSetup();
}


/**
  @remark	EIT timeout일 경우 deafult rating value 결정하는 API. \n
  			OP spec별로 달리질 수 있고, 필요한 파라미터가 추가될 경우  \n

  @return   TRUE : OK to watch \n
  			FALSE : Should be blocked \n
*/
HUINT32	MGR_PG_GetRatingForNoEit (DxSvcType_e eSvcType, HBOOL bEitValid)
{
	return pmgr_pg_GetRatingForNoEit(eSvcType, bEitValid);
}


/**
  @remark	설정된 parental rating 값과 content의 parental rating을 비교하여 rating 허가 여부를 알려줌. \n
  			France의 any public 과 spain의 x-rated 지원. \n

  @return   TRUE : OK to watch \n
  			FALSE : Should be blocked \n
*/
HBOOL	MGR_PG_IsRatingOk (HUINT32				 ulActionId,
							Handle_t			 hService,
							DxDeliveryType_e	 svcDeliveryType,	/** [in] : 비교하고자 하는 event의 service delivery type.
																			지상파의 경우 coutry 셋업에 따라 rating동작 차이 있음. */
							DxRatingAge_e		 eRatingSetup,		/** [in] : Setup에 저장된 parental rating 값. */
							DxRatingAge_e		 eContentRating		/** [in] : Content의 parental rating 값. */
							)
{
	return pmgr_pg_IsRatingOK(ulActionId, hService, svcDeliveryType, eRatingSetup, eContentRating);
}

/**
  @remark	Watch TV의 State중 eWtvState_WAIT_RATING로 전이 할지 결정하는 함수.. \n
  			OP spec별로 전이되는 조건이 달라질 수 있기 때문에 AP_SPEC 으로 만듬.  \n

  @return   TRUE : state change to eWtvState_WAIT_RATING \n
  			FALSE : No state change \n
*/
HBOOL	MGR_PG_IsWaitRating (MgrPg_Info_t *pstPgInfo)
{
	return pmgr_pg_IsWaitParentalRatingState(pstPgInfo);
}





/* end of file */

