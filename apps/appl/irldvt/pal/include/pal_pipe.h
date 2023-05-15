/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  pal_pipe.h
	@brief

	Description:  \n
	Module: PAL/PIPE							 	\n

	Copyright (c) 2012 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/


#ifndef	___PAL_PIPE_H___
#define	___PAL_PIPE_H___

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#include <rlib.h>

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
	ePAL_PIPE_INPUT_NONE,
	ePAL_PIPE_INPUT_TUNER,
	ePAL_PIPE_INPUT_RECODER,
	ePAL_PIPE_INPUT_SCRAMBLED_RECODER,
	ePAL_PIPE_INPUT_PLAYER,
	ePAL_PIPE_INPUT_SCRAMBLED_PLAYER,
	ePAL_PIPE_INPUT_MEDIA_PLAYER,
	ePAL_PIPE_INPUT_EXTERNAL,

	ePAL_PIPE_INPUT_MAX
} PalPipe_Input_e;

typedef enum
{
	ePAL_PIPE_OUTPUT_NONE,
	ePAL_PIPE_OUTPUT_RECORDER,
	ePAL_PIPE_OUTPUT_DECORDER,
	ePAL_PIPE_OUTPUT_EXTERNAL,
	ePAL_PIPE_OUTPUT_TRANSCODER,

	ePAL_PIPE_OUTPUT_MAX
} PalPipe_Output_e;


/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/
extern HERROR			PAL_PIPE_Init (void);
extern Handle_t			PAL_PIPE_GetRscSessionHandle (void);

// Action ID
extern HUINT32			PAL_PIPE_GetActionId (Handle_t hAction);
extern HUINT32			PAL_PIPE_GetSessionVersion (Handle_t hAction);
extern Handle_t			PAL_PIPE_GetActionHandle (HUINT32 ulActionId);
extern Handle_t			PAL_PIPE_IncreaseActionVersion (HUINT32 ulActionId);
extern HERROR			PAL_PIPE_IsActionHandleLatest (Handle_t hAction);

// Pipe Setting
extern HERROR			PAL_PIPE_SetDemux (HUINT32 ulActionId, HUINT32 ulDemuxId);
extern HERROR			PAL_PIPE_SetInputResource (HUINT32 ulActionId, PalPipe_Input_e eType, HUINT32 ulRscId);
extern HERROR			PAL_PIPE_SetOutputResource (HUINT32 ulActionId, PalPipe_Output_e eType, HUINT32 ulRscId);
extern HERROR			PAL_PIPE_SetCi (HUINT32 ulActionId, HINT32 nCiId);
extern HERROR			PAL_PIPE_ResetPipe (HUINT32 ulActionId);

// Resource ID
extern HERROR			PAL_PIPE_GetResourceNumber (RxRscType_e eRscType, HUINT32 *pulRscNum);
extern HERROR			PAL_PIPE_GetResourceId (HUINT32 ulActionId, RxRscType_e eRscType, HUINT32 *pulRscId);
extern HERROR			PAL_PIPE_GetActionIdByResourceId (HUINT32 ulRscId, RxRscType_e eRscType, HUINT32 *pulActionId);
extern HERROR			PAL_PIPE_ChangeResourceId (HUINT32 ulActionId, RxRscType_e eRscType, HUINT32 ulRscId);

extern HERROR			PAL_PIPE_GetResourceObject (HUINT32 ulActionId, RxRscType_e eRscType, RxRscObject_t *pstRscObj);

extern HERROR			PAL_PIPE_GetRfInputAbility (HUINT32 ulRfInputId, HBOOL *pbAbility);
extern HERROR			PAL_PIPE_SetRfInputAbility (HUINT32 ulRfInputId, HBOOL bAbility);
extern HERROR			PAL_PIPE_GetRfInputCapacity (HUINT32 ulRscId, RxRfInfoCapacity_t *pstCapacity);
extern HERROR			PAL_PIPE_GetTunerCapacity (HUINT32 ulRscId, DxDeliveryType_e *peSupportedDeliType);
extern HERROR			PAL_PIPE_GetResourceUsageType (RxRscType_e eRscType, HUINT32 ulRscId, RxUsageType_e *peUsageType);

#endif	//___PAL_PIPE_H___

