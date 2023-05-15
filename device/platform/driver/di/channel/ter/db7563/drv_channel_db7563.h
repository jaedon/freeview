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

#ifndef	__TEMPLATE_H__
#define	__TEMPLATE_H__

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
int DB3461_InitDevice( int nChannelId );
int DB3461_SetTune( int nChannelId,  HUINT8 *bChecklock, HUINT32 unWorkingId );
HBOOL DB3461_CheckLock( int nChannelId, HUINT32 ulWorkingId );
int DB3461_GetStrengthQuality( int nChannelId, DI_CH_SignalQuality_t *pstSignalInfo);
int DB3461_SetPowerState( int nChannelId, CH_Power_e etPower );
int DB3461_EnableTsOutput( int nChannelId );
int DB3461_DisableTsOutput( int nChannelId );
int DB3461_SetTSOutputMode(int nChannelId , CH_TsOutMode_e OutputMode);
int DB3461_CheckAntState( int nChannelId );
int DB3461_SetAntState( int nChannelId, CH_TER_AntStatus_e etAntState);
CH_TER_AntStatus_e DB3461_GetAntState( int nChannelId );
void DB3461_ProcessRecoverAntenna( int nChannelId , HUINT32 unWorkingId);

int DB3461_GetPlpCount( int nChannelId, HUINT8 * pucNumPlps );
int DB3461_GetPlpInfo( int nChannelId, HUINT8 * pucPlpIds, HUINT8 * pucNumPlps );
int DB3461_GetCellId(int nChannelId, HUINT16 *pusCellId);
int DB3461_GetLockedParam(int nChannelId, CH_TER_TuneParam_t * pstTerTuneParam);

// End global function prototypes

#endif /* !__TEMPLATE_H__ */