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
							DxDeliveryType_e	 svcDeliveryType,	/** [in] : ���ϰ��� �ϴ� event�� service delivery type.
																			�������� ��� coutry �¾��� ���� rating���� ���� ����. */
							DxRatingAge_e		 eRatingSetup,		/** [in] : Setup�� ����� parental rating ��. */
							DxRatingAge_e		 eContentRating		/** [in] : Content�� parental rating ��. */
							);

HBOOL	pmgr_pg_IsWaitParentalRatingState (MgrPg_Info_t *pstPgInfo);
HUINT32	pmgr_pg_GetRatingForNoEit (DxSvcType_e eSvcType, HBOOL bEitValid);

#endif /* !___PMGR_PG_H___ */

