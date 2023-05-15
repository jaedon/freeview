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

#ifndef __IR_DRV_FN_SOL_H__
#define __IR_DRV_FN_SOL_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include 	"octo_common.h"

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/
/*	Generally, One service provider has one or two original network ID.
	Additionally, SOL operation is set in only Home Network */

#define TAG_MUX_TRANSPORT_LIST_DESCRIPTOR			0x95
#define TAG_MUX_SIGNATURE_DESCRIPTOR				0x96


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef enum
{
	SOL_DISABLED,
	SOL_VERIFY_INIT,
	SOL_VERIFY_FAIL,
	SOL_VERIFY_SUCCESS
}SOL_VERIFY_STATE;

typedef enum
{
	SOL_SC_UPDATE_IRCARD_STATUS ,
	SOL_SC_UPDATE_PRODUCT_STATUS
}SOL_SC_UPDATE_TYPE;


/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/


void 				IR_SOL_initialize(void);
SOL_VERIFY_STATE	CAS_IR_SOL_IsActivated(void);
HUINT32 			CAS_IR_SOL_SetActivation(SOL_VERIFY_STATE state);
HBOOL 				CAS_IR_SOL_Is_NIT_Changed(HUINT32 crc);
HUINT8 				CAS_IR_SOL_Is_BAT_Changed(HUINT32 crc);
void 				CAS_IR_SOL_Clear_NIT_Info(void);
void 				CAS_IR_SOL_Clear_BAT_Info(void);
void 				CAS_IR_SOL_Complete_NITInfo(HUINT8 ucChanged);
void 				CAS_IR_SOL_Complete_BATInfo( HUINT8 ucChanged );
HUINT32 			CAS_IR_SOL_update_SC_Status( SOL_SC_UPDATE_TYPE eUpdateType, HUINT8 bValid );
HUINT32				CAS_IR_SOL_NIT_UpdateCRC(HUINT32 crc);
HUINT32				CAS_IR_SOL_BAT_UpdateCRC(HUINT32 crc);
HUINT32				CAS_IR_SOL_update_DeliveryDesc( HUINT16 ONID, HUINT16 TSID, HUINT8* pDescriptor);
HUINT32 			CAS_IR_SOL_update_MuxTPDesc( HUINT8* pDesc);
HUINT32 			CAS_IR_SOL_update_SignatureDesc( HUINT8* pDescriptor);
void 				CAS_IR_SOL_UpdatePMT(void);
SOL_SERVICE_STATE	CAS_IR_SOL_is_service_blocked(Handle_t hAction, HUINT16 ONID, HUINT16 TSID);



#endif 	/*	__IR_DRV_FN_SOL_H__		*/

