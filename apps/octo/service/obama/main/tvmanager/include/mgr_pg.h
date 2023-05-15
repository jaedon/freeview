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

	/* - Description : AP layer���� rating ���� ������ �������� �� rating ���� üũ�� �϶�� AP�� broadcasting�ϴ� �̺�Ʈ �޽���.
	   - Parameters Usage :
		   handle : NA
		   p1 : TBD
		   p2 : TBD
		   p3 : TBD */
	eOEVT_PG_CHECK_RATING,

	/* - Description : AP layer���� System Lock ���� ������ �������� �� Lock ���� üũ�� �϶�� AP�� broadcasting�ϴ� �̺�Ʈ �޽���.
	   - Parameters Usage :
		   handle : NA
		   p1 : TBD
		   p2 : TBD
		   p3 : TBD */
	eOEVT_PG_CHECK_LOCK,


	/* - Description : PIN code appl���� PIN code�� pass�Ǿ��ٴ� �̺�Ʈ �޽���.
					Broadcating���� ���� PIN code appl�� ������ parent appl���� ���޵Ǿ�� ��.
	   - Parameters Usage :
		   handle : NA
		   p1 : TBD
		   p2 : TBD
		   p3 : �� �޽����� ������ PIN dialog application callback(app proc) */
	eMEVT_PG_PIN_CHECK_OK,

	/* - Description : PIN code appl���� Ʋ�� PIN code�� �Է� �Ǿ��ٴ� �̺�Ʈ �޽���.
					Broadcating���� ���� PIN code appl�� ������ parent appl���� ���޵Ǿ�� ��.
	   - Parameters Usage :
		   handle : NA
		   p1 : TBD
		   p2 : TBD
		   p3 : �� �޽����� ������ PIN dialog application callback(app proc) */
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
							);

/**
  @remark	Watch TV�� State�� eWtvState_WAIT_RATING�� ���� ���� �����ϴ� �Լ�.. \n
  			OP spec���� ���̵Ǵ� ������ �޶��� �� �ֱ� ������ AP_SPEC ���� ����.  \n

  @return   TRUE : state change to eWtvState_WAIT_RATING \n
  			FALSE : No state change \n
*/
HBOOL	MGR_PG_IsWaitRating (MgrPg_Info_t *pstPgInfo);

/**
  @remark	EIT timeout�� ��� deafult rating value �����ϴ� API. \n
  			OP spec���� �޸��� �� �ְ�, �ʿ��� �Ķ���Ͱ� �߰��� ���  \n

  @return   Rating Value \n
*/
HUINT32	MGR_PG_GetRatingForNoEit (DxSvcType_e eSvcType, HBOOL bEitValid);



#endif /* !___MGR_PG_H___ */

