/*************************************************************************************
 *
 *	DB7584.c
 *
 *	Copyright (c) 2014 by Humax	Corp.
 *
 *
 *************************************************************************************/

#ifndef DB7584_H
#define DB7584_H


/**********************************************************************
 Common identifier definition and structure
***********************************************************************/

/**********************************************************************
 Common functions
***********************************************************************/
/* Common functions which are required at all of the channel devices */
int DB7584_InitDevice( int nChannelId );
int DB7584_SetTune( int nChannelId, HUINT32 ulWorkingId, CH_CAB_TuneParam_t *pstSatTuneParam );

HBOOL DB7584_CheckLock( int nChannelId );
int DB7584_GetStrengthQuality( int nChannelId, DI_CH_SignalQuality_t *pstSignalInfo);
int DB7584_SetPowerState( int nChannelId, CH_Power_e etPower );
int DB7584_EnableTsOutput( int nChannelId );
int DB7584_DisableTsOutput( int nChannelId );
int DB7584_ResetGpio(int nChannelId);
int DB7584_Initialize( int nChannelId );
int DB7584_ReInitDevice( int nChannelId );

#endif	/* DB7584_H */

