/*************************************************************************************
 *
 *	DB3383.c
 *
 *	Copyright (c) 2011 by Humax	Corp.
 *
 *
 *************************************************************************************/

#ifndef DB3383_H
#define DB3383_H


/**********************************************************************
 Common identifier definition and structure
***********************************************************************/

#define	BUS_NUM						NUM_OF_CHANNEL

typedef struct
{
	HUINT16	tsNo;
	HINT32	tsID[15];
	HINT32	netID[15];
	HINT32     uItsmfStatus[15];
} DB3383_TsmfInfo_t;
/**********************************************************************
 Common functions
***********************************************************************/
/* Common functions which are required at all of the channel devices */
int DB3383_InitDevice( int nChannelId );
int DB3383_SetTune( int nChannelId, HUINT32 ulWorkingId, CH_CAB_TuneParam_t *pstSatTuneParam );

HBOOL DB3383_CheckLock( int nChannelId );
int DB3383_GetStrengthQuality( int nChannelId, DI_CH_SignalQuality_t *pstSignalInfo);
int DB3383_SetPowerState( int nChannelId, CH_Power_e etPower );
int DB3383_EnableTsOutput( int nChannelId );
int DB3383_DisableTsOutput( int nChannelId );
int DB3383_UpdateTunedParam( int nChannelId, CH_CAB_TuneParam_t * pstTuneParam );
int DB3383_ResetGpio(int nChannelId);
void DB3383_DumpRegister( int nChannelId);
void DB3383_TsmfData(int nChannelId, HUINT16 *tsNo, HINT32 *tsID, HINT32 *netID);
#if defined(CONFIG_DEMOD_TSMF)
void DB3383_GetPathInfo(int nChannelId);
#endif
#endif

