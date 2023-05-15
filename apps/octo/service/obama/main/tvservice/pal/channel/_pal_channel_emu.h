/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  _pal_channel.h
	@brief	  Octo pal channel header

	Description: MW channel service의 호출을 받아 실제 ddi를 제어하는 channel PAL의 헤더 파일.\n
	Module: PAL/CH			 	\n
	Remarks : 										\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	___PAL_CHANNEL_EMU_H___
#define	___PAL_CHANNEL_EMU_H___

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/

extern HERROR	PAL_CH_EMU_Init (void *pvCbFunc);
extern HBOOL	PAL_CH_EMU_CheckIpTuneReady (void);
extern HINT32	PAL_CH_EMU_GetFirstPlayerIdForFakeTuner (void);
extern HERROR	PAL_CH_EMU_GetSatTunerCapacity (HUINT32 ulTunerGroupId, PalCh_SatTunerCapacity_t *pstSatTunerCap);
extern HUINT32	PAL_CH_EMU_CountSatRfInput (void);
extern HERROR	PAL_CH_EMU_GetSatRfInput (HUINT32 ulRfInputIdx, PalCh_SatRfInput_t *pstRfInput);
extern HERROR	PAL_CH_EMU_SetSatRfInputConnection (HUINT32 ulTunerGroupId, HUINT32 ulRfInputId);
extern HERROR	PAL_CH_EMU_ResetSatRfInputConnection (HUINT32 ulTunerGroupId);
extern HERROR	PAL_CH_EMU_StartTune (HINT32 nDiChType, HUINT32 ulTunerGroupId, HUINT32 ulRequestId, void *pvDiParam);
extern HERROR	PAL_CH_EMU_StopTune (HINT32 nDiChType, HUINT32 ulTunerGroupId);
extern void		PAL_CH_EMU_GetSupportDeliveryType (HUINT32 ulTunerGroupId, HUINT32 *pulSupportDeliveryType);
extern HUINT32	PAL_CH_EMU_GetNumOfTunerGroup (void);
extern HERROR	PAL_CH_EMU_GetLockedInfoSat (HUINT32 ulTunerGroupId, HUINT32 ulTpFreq, HUINT32 	ulLnbFreq, HUINT32 *pulActualTpFreq, DxSat_Polarization_e *pActualPolar, DxSat_CodeRate_e *pActualCodeRate, DxSat_PskModulation_e *pActualPskMod, DxSat_Pilot_e *pActualPilot);

#endif /* !___PAL_CHANNEL_EMU_H___ */

