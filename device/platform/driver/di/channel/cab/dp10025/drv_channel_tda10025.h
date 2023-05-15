/* $Header: */

/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
// File Name:
// Original Author: Full Name!!
// File Description:
// Remarks:

/*******************************************************************/
/* Copyright (c) 2006 HUMAX Co., Ltd.                              */
/* All rights reserved.                                            */
/*******************************************************************/

#ifndef	__DRV_CHANNEL_TDA10025_H__
#define	__DRV_CHANNEL_TDA10025_H__

/*******************************************************************/
/**************************** Header Files**************************/
/*******************************************************************/
// Start Including Header Files

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
void DRV_C_InstallApi( void );
int TDA10025_InitDevice( int nChannelId );
int TDA10025_SetTune( int nChannelId, HUINT32 ulWorkingId, CH_CAB_TuneParam_t *pstSatTuneParam );
HBOOL TDA10025_CheckLock( int nChannelId);
int TDA10025_GetStrengthQuality( int nChannelId, DI_CH_SignalQuality_t *pstSignalInfo);
int TDA10025_SetPowerState( int nChannelId, CH_Power_e etPower );
int TDA10025_EnableTsOutput( int nChannelId );
int TDA10025_DisableTsOutput( int nChannelId );
int TDA10025_ResetGpio( int nChannelId );
int TDA10025_Initialize( int nChannelId );
// End global function prototypes

#endif /* !__DRV_CHANNEL_TDA10025_H__ */