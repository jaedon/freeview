/* $Header: */

/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
// File Name:	template.h
// Original Author: Full Name!!
// File Description:
// Remarks:

/*******************************************************************/
/* Copyright (c) 2006 HUMAX Co., Ltd.                              */
/* All rights reserved.                                            */
/*******************************************************************/

#ifndef	__DRV_CHANNEL_C_H__
#define	__DRV_CHANNEL_C_H__

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
void DRV_C_InstallApi( void );
DI_CHANNEL_ERR CH_C_InitDev( int nChannelId );
DI_CHANNEL_ERR CH_C_ReInitDevice( int nChannelId );
DI_CHANNEL_ERR CH_C_UnInitDevice( int nChannelId );
DI_CHANNEL_ERR CH_C_SetTune( int nChannelId, HUINT32 unWorkingId );
HBOOL CH_C_CheckLock ( int nChannelId);
void CH_C_CheckStatus ( int nChannelId, HUINT32 unWorkingId );
int CH_C_GetLockStatus (int nChannelId);
int CH_C_GetSignalInfo ( int nChannelId, DI_CH_SignalQuality_t *pstSignalInfo );
void CH_C_SetPowerState( int nChannelId, CH_Power_e etPower );
int CH_C_EnableTsOutput ( int nChannelId );
int CH_C_DisableTsOutput ( int nChannelId );
int CH_C_SetTsOutputMode ( int ChannelId, CH_TsOutMode_e OutputMode );
void CH_C_ProcessAutoSearch( int nChannelId, HUINT32 ulWorkingId );

int CH_C_GetDeviceStatus ( int nChannelId, DI_CH_DeviceStatus *pstDeviceStatus );

void DRV_C_InstallApi( void );
int (*CAB_InitDevice)( int nChannelId );
int (*CAB_SetTune)( int nChannelId, HUINT32 ulWorkingId, CH_CAB_TuneParam_t *pstSatTuneParam );
HBOOL (*CAB_CheckLock)( int nChannelId);
int (*CAB_GetStrengthQuality)( int nChannelId, DI_CH_SignalQuality_t *pstSignalInfo);
int (*CAB_SetPowerState)( int nChannelId, CH_Power_e etPower );
int (*CAB_EnableTsOutput)( int nChannelId );
int (*CAB_DisableTsOutput)( int nChannelId );
int (*CAB_ResetGpio)( int nChannelId );
int (*CAB_Initialize)( int nChannelId );
int (*CAB_ReInitDevice)( int nChannelId );
int (*CAB_UnInitDevice)( int nChannelId );

int (*CAB_GetDeviceStatus) ( int nChannelId, DI_CH_DeviceStatus *pstDeviceStatus );

#if defined(CONFIG_DEMOD_TSMF)
void (*CAB_GetPathInfo)(int nChannelId);
void CH_C_GetPathInfo(void);
#endif
int CH_C_GetStatus( int nChannelId, HUINT32 frequency);
int CH_C_GetTsmfData( int nChannelId, HUINT16 *ucTsNo, HINT32 *nTsId, HINT32 *nNetId);
HBOOL CH_C_CheckTsmfLock( int nChannelId, HUINT32 frequency );
int CH_C_StopQAM(int nChannelId);
int CH_C_GetAGC( int nChannelId, HUINT8 *ucRfAgc, HUINT8 *ucIfAgc);
int CH_C_GetEwsInfo( int nChannelId, HUINT8 *ucIsEws);

/* This API will be deprecated */
int CH_C_GetLockInfo(int nChannelId, HINT32 *nSnr, HINT32 *nber_decimal, HINT32 *nber_float, HINT32 *nber_exp, HINT32 *nUncorrected, HINT32 *nSignallevel);

// End global function prototypes

#endif /* !__DRV_CHANNEL_C_H__ */
