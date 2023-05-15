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

#ifndef	_DI_CHANNEL_ATTR_H_
#define	_DI_CHANNEL_ATTR_H_

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
int FindChannelID ( HUINT32 ulInstanceId, DI_CH_Type_e etChannelType );
int 	GetChannelType ( int nChannelId );
int 	GetPreviousTuneStatus ( int nChannelId );
int 	GetTuneStatus ( int nChannelId );
void 	SetTuneStatus ( int nChannelId, CH_Status_e etStatus );
HBOOL	IsStateChanged ( int nChannelId );
HUINT32 GetRequestedId ( int nChannelId );
HUINT32 GetSearchMode ( int nChannelId );
int 	SetSearchMode ( int nChannelId, HUINT32 ulMode );
HUINT32 	GetInstanceId ( int nChannelId );
int 	GetDeviceUnitId ( int nChannelId );
void * 	GetWorkingTuneInfo( int nChannelId );
void * 	GetRequestedTuneInfo( int nChannelId );
void * 	GetPreviousLockedTuneInfo( int nChannelId );
int 	GetTsOutputMode ( int nChannelId );
int 	SetTsOutputMode ( int nChannelId, CH_TsOutMode_e etTsMode );
int 	GetTsStatus ( int nChannelId );
int 	SetTsStatus ( int nChannelId, CH_TsStatus_e TsOnOff );
int 	GetActiveStatus ( int nChannelId );
int 	SetActiveStatus ( int nChannelId, CH_Active_e etAction );

int	GetRfInputId ( int nChannelId );
int	SetRfInputId ( int nChannelId, DI_CH_InputSource_e etInputSource );

// End global function prototypes

#endif /* !_DI_CHANNEL_PRIV_H_ */
