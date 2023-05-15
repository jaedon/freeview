/*************************************************************************************
 *
 *	DB7584.c
 *
 *	Copyright (c) 2014 by Humax	Corp.
 *
 *
 *************************************************************************************/

#ifndef DB7581_H
#define DB7581_H


/**********************************************************************
 Common identifier definition and structure
***********************************************************************/

/**********************************************************************
 Common functions
***********************************************************************/
/* Common functions which are required at all of the channel devices */
int DB7581_InitDevice( int nChannelId );
int DB7581_SetTune( int nChannelId, HUINT32 ulWorkingId, CH_CAB_TuneParam_t *pstSatTuneParam );

HBOOL DB7581_CheckLock( int nChannelId );
int DB7581_GetStrengthQuality( int nChannelId, DI_CH_SignalQuality_t *pstSignalInfo);
int DB7581_SetPowerState( int nChannelId, CH_Power_e etPower );
int DB7581_EnableTsOutput( int nChannelId );
int DB7581_DisableTsOutput( int nChannelId );
int DB7581_ResetGpio(int nChannelId);
int DB7581_Initialize( int nChannelId );
int DB7581_ReInitDevice( int nChannelId );
int DB7581_GetDeviceStatus ( int nChannelId, DI_CH_DeviceStatus *pstDeviceStatus );

#endif	/* DB7581_H */

