/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  pal_cec.h
	@brief	  cec driver의 adaptation layer 													\n

	Description: hdmi 동작과 각 parameter의 조합을 판단하여, HMDI Setting을 한다. 				\n
	Module: PAL/HDMI			 																\n

	Copyright (c) 2008 HUMAX Co., Ltd.															\n
	All rights reserved.																		\n
*/


#ifndef	__PAL_CEC_H__
#define	__PAL_CEC_H__


/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include "octo_common.h"


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

#define CEC_MAX_OPERAND_LEN	15

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/


/*******************************************************************/

typedef enum
{
	eCEC_PLUG_OUT,
	eCEC_PLUG_IN,
	eCEC_MESSAGE_RCVED

} PalCec_Event_t;


typedef struct
{
	HUINT8	initiator;
	HUINT8	destination;

	HBOOL	opcodeValid;
	HUINT8	opcode;

	HUINT32	numOperand;
	HUINT8	operand[CEC_MAX_OPERAND_LEN];

} PalCec_Msg_t;

typedef void (* PalCecNotifier_t)(PalCec_Event_t eEvent, HUINT32 ulParam);

HERROR PAL_CEC_Init(void);
HERROR PAL_CEC_RegisterCallback(PalCecNotifier_t fnCecNotifier);
HERROR PAL_CEC_GetLogicalAddress(HUINT8 *pucLogicalAddr);
HERROR PAL_CEC_GetPhysicalAddress(HUINT16 *pusPhysicalAddr);
HERROR PAL_CEC_SendCecMessage(PalCec_Msg_t *pstTxMsg);


/*******************************************************************/


#endif	//__PAL_VIDEO_H__

