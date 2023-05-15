/* $Header: */

/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
// File Name:	drv_channel_kraken.h
// Original Author: Full Name!!
// File Description:
// Remarks:

/*******************************************************************/
/* Copyright (c) 2006 HUMAX Co., Ltd.                              */
/* All rights reserved.                                            */
/*******************************************************************/

#ifndef	__DRV_CHANNEL_DB7358_H__
#define	__DRV_CHANNEL_DB7358_H__

/*******************************************************************/
/**************************** Header Files**************************/
/*******************************************************************/
// Start Including Header Files
#include "di_channel.h"
// End Including Headers


/*******************************************************************/
/************************ Extern variables *************************/
/*******************************************************************/
// Start Extern variablee

// End Extern variable


/*******************************************************************/
/************************ Macro Definition *************************/
/*******************************************************************/
// Start Macro definition

// End Macro definition


/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
// Start typedef

// End typedef


/*******************************************************************/
/******************** global function prototype ********************/
/*******************************************************************/
// Start global function prototypes
void DRV_S_InstallApi( void );

int DB7346_InitDevice( int nChannelId );
int DB7346_SetTune( int nChannelId, CH_SAT_TuneParam_t *pstSatTuneParam );
int DB7346_CheckTuneState( int nChannelId, HUINT32 ulWorkingId, HUINT32 ulSymbolRate );
HBOOL DB7346_CheckLock( int nChannelId );
int DB7346_GetSignalInfo( int nChannelId, DI_CH_SignalQuality_t *pstSignalInfo );

int DB7346_SetPowerState( int nChannelId, CH_Power_e etPower );
int DB7346_EnableTsOutput( int nChannelId );
int DB7346_DisableTsOutput( int nChannelId );
int DB7346_SendDiseqcMsg( int nChannelId, HUINT8 *pucSendBuf, HUINT8 ucNumOfBuf, CH_SAT_DiseqcInput_e etDiseqcInput );
int DB7346_UpdateTunedParam( int nChannelId, CH_SAT_TuneParam_t * pstTuneParam );
int DB7346_SetInputMode( int nChannelId, CH_SAT_InputMode_e etInputMode );
int DB7346_SetInputSource( int nChannelId, DI_CH_InputSource_e etInputSource );

HBOOL DB7346_LnbGet22KhzTone( int nChannelId );
int DB7346_LnbSet22KhzTone( int nChannelId, HBOOL bEnable );

int DB7346_SetToneBurst(int nChannelId, HBOOL bToneOn, CH_SAT_DiseqcInput_e etInput, CH_SAT_AntennaType_e etAntennaType, CH_SAT_DiseqcVer_e etDiseqcVer );
int DB7346_ResetDiseqc(int nChannelId);

int DB7346_ReceiveDiseqcMsg(int nChannelId,HUINT8 *pucReceiveBuf,HUINT8 ucNumOfBuf,HUINT8 *pucReplyLength);

// End global function prototypes

#endif /* !__DRV_CHANNEL_DB7358_H__ */

