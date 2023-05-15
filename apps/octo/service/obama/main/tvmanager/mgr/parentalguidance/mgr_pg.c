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
  @remark	EIT timeout�� ��� deafult rating value �����ϴ� API. \n
  			OP spec���� �޸��� �� �ְ�, �ʿ��� �Ķ���Ͱ� �߰��� ���  \n

  @return   TRUE : OK to watch \n
  			FALSE : Should be blocked \n
*/
HUINT32	MGR_PG_GetRatingForNoEit (DxSvcType_e eSvcType, HBOOL bEitValid)
{
	return pmgr_pg_GetRatingForNoEit(eSvcType, bEitValid);
}


/**
  @remark	������ parental rating ���� content�� parental rating�� ���Ͽ� rating �㰡 ���θ� �˷���. \n
  			France�� any public �� spain�� x-rated ����. \n

  @return   TRUE : OK to watch \n
  			FALSE : Should be blocked \n
*/
HBOOL	MGR_PG_IsRatingOk (HUINT32				 ulActionId,
							Handle_t			 hService,
							DxDeliveryType_e	 svcDeliveryType,	/** [in] : ���ϰ��� �ϴ� event�� service delivery type.
																			�������� ��� coutry �¾��� ���� rating���� ���� ����. */
							DxRatingAge_e		 eRatingSetup,		/** [in] : Setup�� ����� parental rating ��. */
							DxRatingAge_e		 eContentRating		/** [in] : Content�� parental rating ��. */
							)
{
	return pmgr_pg_IsRatingOK(ulActionId, hService, svcDeliveryType, eRatingSetup, eContentRating);
}

/**
  @remark	Watch TV�� State�� eWtvState_WAIT_RATING�� ���� ���� �����ϴ� �Լ�.. \n
  			OP spec���� ���̵Ǵ� ������ �޶��� �� �ֱ� ������ AP_SPEC ���� ����.  \n

  @return   TRUE : state change to eWtvState_WAIT_RATING \n
  			FALSE : No state change \n
*/
HBOOL	MGR_PG_IsWaitRating (MgrPg_Info_t *pstPgInfo)
{
	return pmgr_pg_IsWaitParentalRatingState(pstPgInfo);
}





/* end of file */

