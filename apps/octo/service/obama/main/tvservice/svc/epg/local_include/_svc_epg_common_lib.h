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

#ifndef	__SVC_EPG_COMMON_LIB_H__
#define	__SVC_EPG_COMMON_LIB_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/



/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

#define CONFIG_EPG_PARSING_SLEEP			1


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/

HERROR	SVC_EPG_SendMessage(HUINT32 ulMessage, Handle_t hAction, HUINT32 param2, HUINT32 param3);
HERROR	SVC_EPG_SendMessageEx(HUINT32 ulMessage, Handle_t hAction, HUINT32 param2, HUINT32 param3, HUINT32 param4, HUINT32 param5);
HERROR	SVC_EPG_MgrPostMessage(HUINT32 nMessage, Handle_t hAction, HUINT32 param1, HUINT32 param2, HUINT32 param3);
HERROR	SVC_EPG_MakeEpgMessage(HUINT16 usCommand, HUINT16 usProcess, HUINT32 *pulMessage);
HERROR	SVC_EPG_GetEpgMessageInfo(HUINT32 ulMessage, HUINT16 *pusCommand, HUINT16 *pusProcess);
HERROR	SVC_EPG_GetMsgQueueId(HUINT32 *pulMsgQueueId);
HERROR	SVC_EPG_GetEpgActionType(Handle_t hAction, HUINT16 *pusActionType);
HERROR	SVC_EPG_GetSpec(DxDeliveryType_e eDeliType, Handle_t hSvcHandle, EPG_SpecType_e *peSpecType);

#endif /* !__SVC_EPG_COMMON_LIB_H__ */
