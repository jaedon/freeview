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

#ifndef	__DRV_CHANNEL_S_H__
#define	__DRV_CHANNEL_S_H__

/*******************************************************************/
/**************************** Header Files**************************/
/*******************************************************************/
// Start Including Header Files
#include "di_channel.h"
#include "di_channel_s.h"
#include "di_channel_priv.h"
// End Including Headers


/*******************************************************************/
/************************ Extern variables *************************/
/*******************************************************************/
// Start Extern variablee
#define LOW_SYMBOL_RATE_LIMIT	2500 // 2.5 MSymbol/sec
#define HIGH_SYMBOL_RATE_LIMIT	35000 // 35 MSymbol/sec
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
//ODU : Out-door Unit
typedef enum {
	ODU_Channel_change,
	ODU_PowerOFF,
	ODU_UbxSignal_ON,
	ODU_Config,
	ODU_LoFreq,
} CH_SAT_ScdCmd_e;

typedef struct{
    HBOOL 					bEnabled;
    CH_SAT_Polarization_e 	etPolarization;
    CH_SAT_LnbVoltage_e 	etVoltage;
    HBOOL 					bToneEnabled;
    HUINT8 					ucControl0;
    HUINT8 					ucControl1;
} CH_SAT_LnbStatus_t;

// End typedef


/*******************************************************************/
/******************** global function prototype ********************/
/*******************************************************************/
// Start global function prototypes
DI_CHANNEL_ERR CH_S_InitDev( int nChannelId );
DI_CHANNEL_ERR CH_S_ReInitDevice( int nChannelId );
DI_CHANNEL_ERR CH_S_UnInitDevice( int nChannelId );
DI_CHANNEL_ERR CH_S_SetTune( int nChannelId, HUINT32 unWorkingId );
int CH_S_GetSignalInfo ( int nChannelId, DI_CH_SignalQuality_t *pstSignalInfo );
int CH_S_SetPowerState( int nChannelId, CH_Power_e etPower );
int CH_S_EnableTsOutput ( int nChannelId );
int CH_S_DisableTsOutput ( int nChannelId );
int CH_S_SendDiseqcMsg( int nChannelId, HUINT8 *pucSendBuf, HUINT8 ucNumOfBuf, CH_SAT_DiseqcInput_e etDiseqcInput );
int CH_S_ReceiveDiseqcMsg( int nChannelId, HUINT8 *pucSendBuf, HUINT8 ucNumOfBuf, HUINT8 *pucReplyLength );

int CH_S_SetLnbOutput( int nChannelId, HBOOL bEnable );
int CH_S_SetLnb22KhzTone( int nChannelId, HBOOL bEnable );
int CH_S_SetLnbPolarisation( int nChannelId, CH_SAT_Polarization_e etPolar, CH_SAT_LnbVoltage_e etVoltage );

int CH_S_ControlLnb( int nChannelId, CH_SAT_TuneParam_t *pstSatTuneParam );
int CH_S_ControlSwitch( int nChannelId, CH_SAT_TuneParam_t *pstSatTuneParam );
int CH_S_ControlScd( int nChannelId, CH_SAT_ScdCmd_e etScdCmd, CH_SAT_TuneParam_t *pstSatTuneParam );
int CH_S_ControlMotor( int nChannelId, HUINT8 *pucData, HUINT8 ucLength );

HBOOL CH_S_CheckLock ( int nChannelId );
void CH_S_CheckStatus ( int nChannelId, HUINT32 unWorkingId );
void CH_S_ProcessAutoSearch( int nChannelId, HUINT32 ulWorkingId );

int CH_S_SetInputMode( int nChannelId, CH_SAT_InputMode_e etInputMode );
int CH_S_SetInputSource( int nChannelId, DI_CH_InputSource_e etInputSource );
int CH_S_SetToneBurst(int nChannelId, HBOOL bToneOn, CH_SAT_DiseqcInput_e etInput, CH_SAT_AntennaType_e etAntennaType, CH_SAT_DiseqcVer_e etDiseqcVer );

int CH_S_GetDeviceStatus ( int nChannelId, DI_CH_DeviceStatus *pstDeviceStatus );

void DRV_S_InstallApi( void );
int (*NIM_InitDevice)( int nChannelId );
int (*NIM_SetTune)( int nChannelId, CH_SAT_TuneParam_t *pstSatTuneParam );
int (*NIM_CheckTuneState)( int nChannelId, CH_SAT_TransportSpec_e etSatSystem, HUINT32 ulSymbolRate );
int (*NIM_CheckLock)( int nChannelId );
int (*NIM_GetSignalInfo)( int nChannelId, DI_CH_SignalQuality_t *pstSignalInfo );
int (*NIM_SetPowerState)( int nChannelId, CH_Power_e etPower );
int (*NIM_EnableTsOutput)( int nChannelId );
int (*NIM_DisableTsOutput)( int nChannelId );
int (*NIM_SendDiseqcMsg)( int nChannelId, HUINT8 *pucSendBuf, HUINT8 ucNumOfBuf, CH_SAT_DiseqcInput_e etDiseqcInput );
int (*NIM_ReceiveDiseqcMsg)( int nChannelId, HUINT8 *pucSendBuf, HUINT8 ucNumOfBuf, HUINT8 *pucReplyLength );
int (*NIM_UpdateTunedParam)( int nChannelId, CH_SAT_TuneParam_t *pstTuneParam );
int (*NIM_SetInputMode)( int nChannelId, CH_SAT_InputMode_e etInputMode );
int (*NIM_SetInputSource)( int nChannelId, DI_CH_InputSource_e etInputSource );
int (*NIM_SetToneBurst)(int nChannelId, HBOOL bToneOn, CH_SAT_DiseqcInput_e etInput, CH_SAT_AntennaType_e etAntennaType, CH_SAT_DiseqcVer_e etDiseqcVer );
int (*NIM_ResetDiseqc)( int nChannelId );
int (*NIM_ReInitDevice)( int nChannelId );
int (*NIM_UnInitDevice)( int nChannelId );
int (*NIM_GetDeviceStatus) ( int nChannelId, DI_CH_DeviceStatus *pstDeviceStatus );



HBOOL (*LNB_Get22KhzTone)( int nChannelId );
int (*LNB_Set22KhzTone)( int nChannelId, HBOOL bEnable );
int (*LNB_InitDevice)( int nUnitId );
int (*LNB_SetOutput)( int nUnitId, HBOOL bEnable );
int (*LNB_SetPolarisation)( int nUnitId, CH_SAT_Polarization_e etPolar, CH_SAT_LnbVoltage_e etVoltage );
int (*LNB_CheckAntState)( int nUnitId );

// End global function prototypes

#endif /* !__DRV_CHANNEL_A_S__ */
