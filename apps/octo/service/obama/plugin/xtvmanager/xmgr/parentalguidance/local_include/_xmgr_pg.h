/**
	@file     _xmgr_pg.h
	@brief    file_name & simple comment.

	Description :			\n
	Module :				\n
	Remarks :				\n
	 Date			Modification				Name\n
	-----------	-------------------------------------	------------------\n
	 2013/04/17		initiated					chmin\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	__XMGR_PG_INT_H__
#define	__XMGR_PG_INT_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

#include <db_svc.h>
#include <stb_params.h>
#include <mgr_pg.h>
//#include <pmgr_pg.h>

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/
extern HUINT32	xmgr_pg_GetRatingFromSetup_Base (void);
extern HUINT32	xmgr_pg_GetRecordRatingFromSetup_Base (void);

extern HBOOL	xmgr_pg_IsRatingOK_Base (HUINT32 ulActionId, Handle_t hService, DxDeliveryType_e svcDeliveryType, DxRatingAge_e eRatingSetup, DxRatingAge_e eContentRating);
extern HBOOL	xmgr_pg_IsWaitParentalRatingState_Base (MgrPg_Info_t *pstPgInfo);
extern HUINT32	xmgr_pg_GetNoEitRatingValue_Base (DxSvcType_e eSvcType, HBOOL bEitValid);

extern HUINT32	xmgr_pg_GetNoEitRatingValue_Ziggo (DxSvcType_e eSvcType, HBOOL bEitValid);
extern HUINT32	xmgr_pg_GetNoEitRatingValue_MeSat (DxSvcType_e eSvcType, HBOOL bEitValid);

#endif /* __XMGR_PG_INT_H__ */

