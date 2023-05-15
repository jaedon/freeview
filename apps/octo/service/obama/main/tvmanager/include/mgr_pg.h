/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  mgr_pg.h
	@brief

	Description:			\n
	Module: 			 	\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	___MGR_PG_H___
#define	___MGR_PG_H___

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#include <db_svc.h>
#include <mgr_common.h>

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

enum
{
	eMEVT_PG		= eMEVT_PG_START,

	/* - Description : AP layer에서 rating 관련 설정을 변경했을 때 rating 관련 체크를 하라고 AP에 broadcasting하는 이벤트 메시지.
	   - Parameters Usage :
		   handle : NA
		   p1 : TBD
		   p2 : TBD
		   p3 : TBD */
	eOEVT_PG_CHECK_RATING,

	/* - Description : AP layer에서 System Lock 관련 설정을 변경했을 때 Lock 관련 체크를 하라고 AP에 broadcasting하는 이벤트 메시지.
	   - Parameters Usage :
		   handle : NA
		   p1 : TBD
		   p2 : TBD
		   p3 : TBD */
	eOEVT_PG_CHECK_LOCK,


	/* - Description : PIN code appl에서 PIN code가 pass되었다는 이벤트 메시지.
					Broadcating하지 말고 PIN code appl을 생성한 parent appl에만 전달되어야 함.
	   - Parameters Usage :
		   handle : NA
		   p1 : TBD
		   p2 : TBD
		   p3 : 이 메시지를 발행한 PIN dialog application callback(app proc) */
	eMEVT_PG_PIN_CHECK_OK,

	/* - Description : PIN code appl에서 틀린 PIN code가 입력 되었다는 이벤트 메시지.
					Broadcating하지 말고 PIN code appl을 생성한 parent appl에만 전달되어야 함.
	   - Parameters Usage :
		   handle : NA
		   p1 : TBD
		   p2 : TBD
		   p3 : 이 메시지를 발행한 PIN dialog application callback(app proc) */
	eMEVT_PG_PIN_CHECK_DENIED,

};

typedef struct
{
	DxCasType_e		eCasType;
	HBOOL			bCasOk;
	HBOOL			bRatingValid;
	DxSvcType_e		eSvcType;
} MgrPg_Info_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/


/* Parental Rating Setup Value Reference */
HUINT32	MGR_PG_GetRatingFromSetup (void);
HUINT32	MGR_PG_GetRecordRatingFromSetup (void);

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
							);

/**
  @remark	Watch TV의 State중 eWtvState_WAIT_RATING로 전이 할지 결정하는 함수.. \n
  			OP spec별로 전이되는 조건이 달라질 수 있기 때문에 AP_SPEC 으로 만듬.  \n

  @return   TRUE : state change to eWtvState_WAIT_RATING \n
  			FALSE : No state change \n
*/
HBOOL	MGR_PG_IsWaitRating (MgrPg_Info_t *pstPgInfo);

/**
  @remark	EIT timeout일 경우 deafult rating value 결정하는 API. \n
  			OP spec별로 달리질 수 있고, 필요한 파라미터가 추가될 경우  \n

  @return   Rating Value \n
*/
HUINT32	MGR_PG_GetRatingForNoEit (DxSvcType_e eSvcType, HBOOL bEitValid);



#endif /* !___MGR_PG_H___ */

