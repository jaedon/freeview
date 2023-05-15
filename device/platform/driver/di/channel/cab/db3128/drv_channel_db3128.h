/*************************************************************************************
 *
 *	DB3128.c
 *
 *	Copyright (c) 2011 by Humax	Corp.
 *
 *
 *************************************************************************************/

#ifndef DB3128_H
#define DB3128_H


/**********************************************************************
 Common identifier definition and structure
***********************************************************************/

#define	BUS_NUM						NUM_OF_CHANNEL

/**********************************************************************
 Common functions
***********************************************************************/
/* Common functions which are required at all of the channel devices */
int DB3128_InitDevice( int nChannelId );
int DB3128_SetTune( int nChannelId, HUINT32 ulWorkingId, CH_CAB_TuneParam_t *pstSatTuneParam );

HBOOL DB3128_CheckLock( int nChannelId );
int DB3128_GetStrengthQuality( int nChannelId, DI_CH_SignalQuality_t *pstSignalInfo);
int DB3128_SetPowerState( int nChannelId, CH_Power_e etPower );
int DB3128_EnableTsOutput( int nChannelId );
int DB3128_DisableTsOutput( int nChannelId );
int DB3128_UpdateTunedParam( int nChannelId, CH_CAB_TuneParam_t * pstTuneParam );
int DB3128_ResetGpio(int nChannelId);
void DB3128_DumpRegister( int nChannelId);
int DB3128_GetDeviceStatus ( int nChannelId, DI_CH_DeviceStatus *pstDeviceStatus );
#endif

