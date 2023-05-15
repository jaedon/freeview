/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  layer_module.h File Name을 적습니다.
	@brief	  file_name & simple comment.

	Description: File에 대한 설명을 적습니다.		\n
	Module: Layer와 모듈 명을 적습니다.			 	\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef __IR_DRV_FN_MR_H__
#define __IR_DRV_FN_MR_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include 	"octo_common.h"
#include "s3_maturity_rating.h"

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/
/*	Generally, One service provider has one or two original network ID.
	Additionally, SOL operation is set in only Home Network */




/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/



/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/


#if defined(CONFIG_MW_CAS_IRDETO_MATURITY_RATING)
HUINT32 XSVC_CAS_IR_DRV_MR_CapabilityQuery(HUINT32 resourceId);
HUINT32 XSVC_CAS_IR_DRV_MR_ConfigurationQuery(HUINT32 resourceId, HUINT8 *aucSubscriberPin,msg_mr_config_action_e eAction, HUINT8 bEnableMREvaluation );
HUINT32 XSVC_CAS_IR_DRV_MR_UserProfileQuery(HUINT32 resourceId, HUINT8 *aucSubscriberPin,msg_mr_user_profile_action_e eAction, msg_mr_user_profile_data_st *pstUserProfile);
HUINT32 XSVC_CAS_IR_DRV_MR_UpdateResource(Handle_t hAction, HUINT8 *aucPIN);
#endif


#endif 	/*	__IR_DRV_FN_SOL_H__		*/

