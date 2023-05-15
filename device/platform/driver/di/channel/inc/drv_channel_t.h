/*
 * (c) 2011-2012 Humax Co., Ltd.
 * This program is produced by Humax Co., Ltd. ("Humax") and
 * the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
 * non-assignable, non-transferable and non-exclusive license to use this Software.
 * You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
 * you agree to the responsibility to take all reasonable efforts to protect the any information
 * you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
 * reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
 * If you have no authorized license, discontinue using this Software immediately.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS,
 * IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
 * IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
 * CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.

 * This License is effective until terminated. You may terminate this License at any time by destroying all copies
 * of the Software including all documentation. This License will terminate immediately without notice from Humax
 * to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
 * of the Software and all documentation.

 * The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
 * All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
 * in the Republic of Korea.
*/
/* $Header: */

/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
// File Name:	drv_channel_t.h
// Original Author: Full Name!!
// File Description: Channel driver header  : channel driver specific functions
// Remarks:



#ifndef	__DRV_CHANNEL_T_H__
#define	__DRV_CHANNEL_T_H__

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
typedef struct
{
    CH_TER_AntStatus_e 	etDesiredAntState;
    CH_TER_AntStatus_e 	etCurrentAntState;

} CH_TER_AntStatus_t;

// End typedef


/*******************************************************************/
/******************** global function prototype ********************/
/*******************************************************************/
// Start global function prototypes
DI_CHANNEL_ERR CH_T_InitDev( int nChannelId );
DI_CHANNEL_ERR CH_T_ReInitDevice( int nChannelId );
DI_CHANNEL_ERR CH_T_UnInitDevice( int nChannelId );
DI_CHANNEL_ERR CH_T_SetTune( int nChannelId, HUINT32 unWorkingId );
int CH_T_GetStatus( int nChannelId );
HBOOL CH_T_CheckLock ( int nChannelId, HUINT32 unWorkingID );
void CH_T_CheckStatus ( int nChannelId, HUINT32 unWorkingId );
int CH_T_EnableTsOutput ( int nChannelId );
int CH_T_DisableTsOutput ( int nChannelId );
int CH_T_SetTsOutputMode ( int nChannelId, CH_TsOutMode_e OutputMode );
int CH_T_GetSignalInfo (int nChannelId, DI_CH_SignalQuality_t *pstSignalInfo);
void CH_T_SetPowerState( int nChannelId, CH_Power_e etPower );
int CH_T_GetLockedInfo(int nChannelId, CH_TER_TuneParam_t * pstTerParam);

int CH_T_SetAntState( int nChannelId, CH_TER_AntStatus_e etAntState );
CH_TER_AntStatus_e CH_T_GetAntState( int nChannelId );

int CH_T_CheckTsmfStatus( int nChannelId );
int CH_T_StopTER(int nChannelId);
int CH_T_GetAGC( int nChannelId, HUINT8 *ucRfAgc, HUINT8 *ucIfAgc);
int CH_T_GetEwsInfo( int nChannelId, HUINT8 *bIsEws);
int CH_T_BERCalTime( int nChannelId, HUINT32 berCalMode);

int CH_T_GetDataPlpInfo( int nChannelId,  HUINT8 *pucPlpIds, HUINT8 *pucNumPlps );
int CH_T_GetDataPlpCount( int nChannelId,   HUINT8 *pucNumPlps );
int CH_T_GetCellId( int nChannelId,   HUINT16 *pusCellId );

int CH_T_GetDeviceStatus(int nChannelId,DI_CH_DeviceStatus * pstDeviceStatus);
//JAMES
int CH_T_DumpRegister(int nChannelId);
int CH_T_READ(int nChannelId, int uAddr);
int CH_T_WRITE(int nChannelId, int uAddr, int uData);

void DRV_T_InstallApi( void );
int (*TER_InitDevice)( int nChannelId );
int (*TER_SetTune)( int nChannelId,  HUINT8 *bChecklock, HUINT32 unWorkingId );
HBOOL (*TER_CheckLock)( int nChannelId, HUINT32 ulWorkingId );
int (*TER_GetSignalInfo)( int nChannelId, DI_CH_SignalQuality_t *pstSignalInfo);
int (*TER_SetPowerState)( int nChannelId, CH_Power_e etPower );
int (*TER_SetTsOutputMode)( int nChannelId, CH_TsOutMode_e etTsMode );
int (*TER_EnableTsOutput)( int nChannelId );
int (*TER_DisableTsOutput)( int nChannelId );
int (*TER_CheckAntState)( int nChannelId );
int (*TER_SetAntState)( int nChannelId, CH_TER_AntStatus_e etAntState);
CH_TER_AntStatus_e (*TER_GetAntState)( int nChannelId );
void (*TER_ProcessRecoverAntenna)( int nChannelId , HUINT32 unWorkingId);
int (*TER_GetDataPlpCount)( int nChannelId,   HUINT8 *pucNumPlps );
int (*TER_GetDataPlpInfo)( int nChannelId,  HUINT8 *pucPlpIds, HUINT8 *pucNumPlps );
int (*TER_GetCellId)( int nChannelId,  HUINT16*pusCellId);
int (*TER_GetLockedInfo)(int nChannelId, CH_TER_TuneParam_t * pstTerParam);

void (*TER_ResetGpio)(int nChannelId);
void (*TER_Initialize)(int nChannelId);

int (*TER_ReInitDevice)( int nChannelId );
int (*TER_UnInitDevice)( int nChannelId );

int (*TER_GetDeviceStatus) ( int nChannelId, DI_CH_DeviceStatus *pstDeviceStatus );

//JAMES
int (*TER_DumpRegister)( int nChannelId );
int (*TER_READ)( int nChannelId , int nAddr);
int (*TER_WRITE)( int nChannelId , int nAddr, int nData);


// End global function prototypes

#endif /* !__DRV_CHANNEL_T_H__ */
