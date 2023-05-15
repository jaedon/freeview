/**
	@file	  pal_sc.h
	@brief	  																					\n

	Description: 																				\n
	Module: PAL/SC				 																\n

	Copyright (c) 2008 HUMAX Co., Ltd.															\n
	All rights reserved.																		\n
*/


#ifndef	__PAL_SC_H__
#define	__PAL_SC_H__


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


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef enum
{
	ePAL_SC_RESETTYPE_NONE,
	ePAL_SC_RESETTYPE_COLD,
	ePAL_SC_RESETTYPE_WARM
}PalSc_ResetType_e;

typedef enum
{
	ePAL_SC_FLAG_DISABLE,
	ePAL_SC_FLAG_ENABLE
} PalSc_Flag_e;

typedef enum { /* 경고 : 값을 수정하지 말것 !!!! */
	ePAL_SC_PROTOCOL_T0			= 0,
	ePAL_SC_PROTOCOL_T1			= 1,

	ePAL_SC_PROTOCOL_T0_IR,
	ePAL_SC_PROTOCOL_T0_CX,
	ePAL_SC_PROTOCOL_T1_NA,
	ePAL_SC_PROTOCOL_T1_ARIB,

	ePAL_SC_PROTOCOL_T14			= 14,
	ePAL_SC_PROTOCOL_END
} PalSc_Protocol_e;

typedef enum {
	ePAL_SC_CARDSTATUS_NOT_PRESENT,
	ePAL_SC_CARDSTATUS_PRESENT
} PalSc_CardStatus_e;

typedef enum {
	ePAL_SC_CARDEVENT_EXTRACTED,
	ePAL_SC_CARDEVENT_INSERTED
} PalSc_CardEvent_e;

typedef struct tSCI_SC_PARAM
{
	PalSc_Protocol_e eProtocol;
	HUINT32 ulClock;
	HUINT32	ulBaudrate;
	HUINT8	ucFI;
	HUINT8	ucDI;
	HUINT8	ucN;
	HUINT8	ucCWI;
	HUINT8	ucBWI;
} PalSc_Param_t;

typedef HUINT32 PalSc_Handle_t;
typedef void (*PalSc_Callback_t)(PalSc_Handle_t hSci, HUINT8 ucSlotId, PalSc_CardEvent_e eStatus);

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/
HERROR PAL_SC_Init(void);
HUINT32 PAL_SC_GetNumberOfModule(void);

HERROR PAL_SC_RegisterCbFunc(HUINT32 ulSlotId, DxCAS_GroupId_e eCasId, PalSc_Callback_t pfnCb, PalSc_Handle_t *hSci);
HERROR PAL_SC_UnregisterCbFunc(PalSc_Handle_t hSci);

/*
handle에 해당하는 cas id 및 slot이 init 완료 됨을 의미.
sub cas가 callback을 받으면 자체적으로 Reset등의 과정을 진행한 후 자신의 card인지 여부를 다시 알려주어야 한다.
*/
HERROR PAL_SC_NotifyScValidity(PalSc_Handle_t hSci, PalSc_Flag_e eMatched);

/* cold , warm에 대한 파라미터 비교. */
/* 개선 사항 : sci sc param의 경우에는 protocol에 dependent한 값이므로 초기 default 값은 cas가 전달해주도록 한다. */
HERROR PAL_SC_Reset(
					PalSc_Handle_t hSci,
					HUINT8 *pucAtr,
					HUINT32 *pulLen,
					PalSc_ResetType_e eResetType,
					PalSc_Param_t *pstDefaultParam
					);

/* 현재 sc의 status read */
HERROR PAL_SC_GetCardStatus(PalSc_Handle_t hSci, PalSc_CardStatus_e *peStatus);
HERROR PAL_SC_GetCardStatusBySlotID(HUINT32 ulSlotId, PalSc_CardStatus_e *peStatus);
HERROR PAL_SC_GetSlotId(PalSc_Handle_t hSci, HUINT8 *pucSlotId);

/* after ATR */
HERROR PAL_SC_SetParams(PalSc_Handle_t hSci, PalSc_Param_t *pstParam);
HERROR PAL_SC_GetParams(PalSc_Handle_t hSci, PalSc_Param_t *pstParam);

/* 사용처 확인. T0 */
HERROR PAL_SC_Write(PalSc_Handle_t hSci, HUINT8 *pucWriteBuf, HUINT32 ulNumWrite, HUINT8 *pucReadBuf, HUINT32 *pulNumRead);
HERROR PAL_SC_Read(PalSc_Handle_t hSci, HUINT8 *pucWriteBuf, HUINT32 ulNumWrite, HUINT8 *pucReadBuf, HUINT32 *pulNumRead);


/* T1 */
HERROR PAL_SC_Transfer(PalSc_Handle_t hSci, HUINT8 *pucWriteBuf, HUINT32 ulNumWrite, HUINT8 *pucReadBuf, HUINT32 *pulNumRead, HUINT32 ulExpectedNumRead);

HERROR PAL_SC_Deactivate(PalSc_Handle_t hSci);

HERROR PAL_SC_Activate(PalSc_Handle_t hSci);

HERROR PAL_SC_NegotiatePTS(PalSc_Handle_t hSci, HUINT8 *pucWriteBuf, HUINT8 ucNumToWrite, HUINT8 *pucReadBuf, HUINT8 *pucNumRead);


/********************************************************************
							ATR INFO
	K = T0 & 0x0F
	FI = (TA(1) >> 4) & 0x0F
	DI = TA(1) & 0x0F
	II = (TB(1) >> 5) & 0x07
	PI1 = TB(1) & 0x1F
	N = TC(1)
	T_first = TD(1) & 0x0F (TD(1)이 있으면)
	T_first = 0 (TD(1)이 없으면)
	T = TD(i) & 0x0F
	Specific Mode = True (TA(2)가 있으면)
		T = TA(2) & 0x0F
		FI, DI 사용 ( if(TA(2) & 0x10) )
		PPS Handled(Possible) = False ( if(TA(2) & 0x80 ), True ( if(!(TA(2) & 0x80) )
	PI2 = TB(2)
	WI = TC(2)
	IFSC = TA(i) ( TD(2) 이후의 TD(i)에 T=1이 지정된 후의 TA(i))
	CWI = TB(i) & 0x0F ( TD(2) 이후의 TD(i)에 T=1이 지정된 후의 TA(i))
	BWI = (TB(i) >> 4) & 0x0F ( TD(2) 이후의 TD(i)에 T=1이 지정된 후의 TA(i))
 ********************************************************************/


#endif	//__PAL_SC_H__

