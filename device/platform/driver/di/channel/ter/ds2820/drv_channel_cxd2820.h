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

#ifndef	__TEMPLATE_H__
#define	__TEMPLATE_H__

/*******************************************************************/
/**************************** Header Files**************************/
/*******************************************************************/
// Start Including Header Files
#include "htype.h"
#include "di_channel_priv.h"
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
int CXD2820_InitDevice( int nChannelId );
int CXD2820_SetTune( int nChannelId,  HUINT8 *bChecklock, HUINT32 unWorkingId );
HBOOL CXD2820_CheckLock( int nChannelId, HUINT32 ulWorkingId );
int CXD2820_GetSignalInfo( int nChannelId, DI_CH_SignalQuality_t *pstSignalInfo);
int CXD2820_SetPowerState( int nChannelId, CH_Power_e etPower );
int CXD2820_EnableTsOutput( int nChannelId );
int CXD2820_DisableTsOutput( int nChannelId );
int CXD2820_CheckAntState( int nChannelId );
int CXD2820_SetAntState( int nChannelId, DI_CH_AntStatus_e etAntState);
DI_CH_AntStatus_e CXD2820_GetAntState( int nChannelId );
void CXD2820_ProcessRecoverAntenna( int nChannelId , HUINT32 unWorkingId);
void DummyCXD2820_ResetGpio(int nChannelId);
void DummyCXD2820_Initialize(int nChannelId);
int CXD2820_GetPlpCount( int nChannelId, HUINT8 * pucNumPlps );
int CXD2820_GetPlpInfo( int nChannelId,  HUINT8 *pucPlpIds, HUINT8 *pucNumPlps );

// End global function prototypes

#endif /* !__TEMPLATE_H__ */
