/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  pmgr_pg.h
	@brief

	Description:			\n
	Module: 			 	\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	___PMGR_PG_H___
#define	___PMGR_PG_H___


/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#include <mgr_pg.h>

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Plug-In Functions    ********************/
/*******************************************************************/

HUINT32	pmgr_pg_GetRatingFromSetup (void);
HUINT32	pmgr_pg_GetRecordRatingFromSetup (void);
HBOOL	pmgr_pg_IsRatingOK (HUINT32				 ulActionId,
							Handle_t			 hService,
							DxDeliveryType_e	 svcDeliveryType,	/** [in] : 비교하고자 하는 event의 service delivery type.
																			지상파의 경우 coutry 셋업에 따라 rating동작 차이 있음. */
							DxRatingAge_e		 eRatingSetup,		/** [in] : Setup에 저장된 parental rating 값. */
							DxRatingAge_e		 eContentRating		/** [in] : Content의 parental rating 값. */
							);

HBOOL	pmgr_pg_IsWaitParentalRatingState (MgrPg_Info_t *pstPgInfo);
HUINT32	pmgr_pg_GetRatingForNoEit (DxSvcType_e eSvcType, HBOOL bEitValid);

#endif /* !___PMGR_PG_H___ */

