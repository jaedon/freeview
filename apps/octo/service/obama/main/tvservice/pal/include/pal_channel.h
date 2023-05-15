/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  pal_channel.h
	@brief	  Octo pal channel header

	Description: MW channel service의 호출을 받아 실제 ddi를 제어하는 channel PAL의 헤더 파일.\n
	Module: PAL/CH			 	\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	__PAL_CHANNLE_H__
#define	__PAL_CHANNLE_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>

#if defined(CONFIG_MW_CH_SATELLITE)
#include <pal_channel_sat.h>
#endif
#if defined(CONFIG_MW_CH_TERRESTRIAL)
#include <pal_channel_ter.h>
#endif
#if defined(CONFIG_MW_CH_CABLE)
#include <pal_channel_cab.h>
#endif

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
/** @brief  */

typedef enum
{
	ePAL_CH_EVENT_LOCKED,
	ePAL_CH_EVENT_NO_SIGNAL,
	ePAL_CH_EVENT_ANTENNA_FAULT,
	ePAL_CH_EVENT_ANTENNA_OK,
	ePAL_CH_EVENT_MOTOR_MOVING,
	ePAL_CH_EVENT_TSMF_GOOD,
	ePAL_CH_EVENT_TSMF_BAD,
	ePAL_CH_EVENT_EWS_ON,
	ePAL_CH_EVENT_EWS_OFF,
} PalCh_Event_e;

typedef enum
{
	ePAL_CH_DEMUX_OUT_NONE,
	ePAL_CH_DEMUX_OUT_AV,
	ePAL_CH_DEMUX_OUT_REC,
	ePAL_CH_DEMUX_OUT_EXT,
} PalCh_DemuxOut_e;

typedef void (*PalCh_EventCallback_t)(HUINT32 ulTunerGroupId, HUINT32 ulRequestId, PalCh_Event_e event);

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/

/* Public Function 마다 필히 기재하도록 함 */

extern void PAL_CH_Init(void);
extern void PAL_CH_ActivateDemod(HUINT32 ulTunerGroupId, DxDeliveryType_e deliveryType);
extern void PAL_CH_DeactivateDemod(HUINT32 ulTunerGroupId, DxDeliveryType_e deliveryType);
extern void PAL_CH_SetTunerGroupDeliveryType(HUINT32 ulTunerGroupId, DxDeliveryType_e deliveryType);
extern void PAL_CH_ConnectDemux(HUINT32 ulTunerGroupId, HUINT32 ulDemuxId, PalCh_DemuxOut_e eDemuxOut, HUINT32 ulOutDeviceId);
extern void PAL_CH_DisconnectDemux(HUINT32 ulDemuxId);
extern void PAL_CH_GetSupportDeliveryType(HUINT32 ulTunerGroupId, HUINT32 *pulSupportDeliveryType);
extern HUINT32 PAL_CH_GetNumOfTunerGroup(void);
extern void PAL_CH_RegisterCheckTunerCallback(PalCh_EventCallback_t pfEventCallback);
extern void PAL_CH_SetLoopthroughGPIO(HBOOL bHigh);

#endif /* !__PAL_CHANNLE_H__ */

