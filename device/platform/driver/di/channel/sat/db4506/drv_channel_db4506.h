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

#ifndef	__DRV_CHANNEL_DB4506_H__
#define	__DRV_CHANNEL_DB4506_H__

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

int DB4506_InitDevice( int nChannelId );
int DB4506_SetTune( int nChannelId, CH_SAT_TuneParam_t *pstSatTuneParam );
int DB4506_CheckTuneState( int nChannelId, HUINT32 ulWorkingId, HUINT32 ulSymbolRate );
HBOOL DB4506_CheckLock( int nChannelId );
int DB4506_GetSignalInfo( int nChannelId, DI_CH_SignalQuality_t *pstSignalInfo );

int DB4506_SetPowerState( int nChannelId, CH_Power_e etPower );
int DB4506_EnableTsOutput( int nChannelId );
int DB4506_DisableTsOutput( int nChannelId );
int DB4506_SendDiseqcMsg( int nChannelId, HUINT8 *pucSendBuf, HUINT8 ucNumOfBuf, CH_SAT_DiseqcInput_e etDiseqcInput );
int DB4506_UpdateTunedParam( int nChannelId, CH_SAT_TuneParam_t * pstTuneParam );
int DB4506_SetInputMode( int nChannelId, CH_SAT_InputMode_e etInputMode );
int DB4506_SetInputSource( int nChannelId, DI_CH_InputSource_e etInputSource );
int DB4506_SetToneBurst(int nChannelId, HBOOL bToneOn, CH_SAT_DiseqcInput_e etInput, CH_SAT_AntennaType_e etAntennaType, CH_SAT_DiseqcVer_e etDiseqcVer );

HBOOL DB4506_LnbGet22KhzTone( int nChannelId );
int DB4506_LnbSet22KhzTone( int nChannelId, HBOOL bEnable );


// End global function prototypes

#endif /* !__DRV_CHANNEL_DB4506_H__ */

