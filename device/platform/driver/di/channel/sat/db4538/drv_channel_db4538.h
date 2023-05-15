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

#ifndef	__DRV_CHANNEL_DB4538_H__
#define	__DRV_CHANNEL_DB4538_H__

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

int DB4538_InitDevice( int nChannelId );
int DB4538_SetTune( int nChannelId, CH_SAT_TuneParam_t *pstSatTuneParam );
int DB4538_CheckTuneState( int nChannelId, HUINT32 ulWorkingId, HUINT32 ulSymbolRate );
HBOOL DB4538_CheckLock( int nChannelId );
int DB4538_GetSignalInfo( int nChannelId, DI_CH_SignalQuality_t *pstSignalInfo );

int DB4538_SetPowerState( int nChannelId, CH_Power_e etPower );
int DB4538_EnableTsOutput( int nChannelId );
int DB4538_DisableTsOutput( int nChannelId );
int DB4538_SendDiseqcMsg( int nChannelId, HUINT8 *pucSendBuf, HUINT8 ucNumOfBuf, CH_SAT_DiseqcInput_e etDiseqcInput );
int DB4538_UpdateTunedParam( int nChannelId, CH_SAT_TuneParam_t * pstTuneParam );
int DB4538_SetInputMode( int nChannelId, CH_SAT_InputMode_e etInputMode );
int DB4538_SetInputSource( int nChannelId, DI_CH_InputSource_e etInputSource );

HBOOL DB4538_LnbGet22KhzTone( int nChannelId );
int DB4538_LnbSet22KhzTone( int nChannelId, HBOOL bEnable );

int DB4538_SetToneBurst(int nChannelId, HBOOL bToneOn, CH_SAT_DiseqcInput_e etInput, CH_SAT_AntennaType_e etAntennaType, CH_SAT_DiseqcVer_e etDiseqcVer );

// End global function prototypes

#endif /* !__DRV_CHANNEL_DB4538_H__ */

