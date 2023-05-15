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

#ifndef	__DI_CHANNEL_UTIL_H__
#define	__DI_CHANNEL_UTIL_H__

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
HBOOL CompareTuneParam( int nChannelId );
int UpdatePreviousLockedParam( int nChannelId );
int UpdateLockedParam( int nChannelId, void * pvTarget );
int UpdateTuneParam( int nChannelID, HUINT32 unRequestID, void * vpSource, HUINT32 unSize );
int SendChannelCommand( CH_Command_e etCommand, int nChannelID, HUINT32 unRequestID );

void PrintSatTuneParam (CH_SAT_TuneParam_t* stInputParam );
void PrintTerTuneParam (CH_TER_TuneParam_t* stInputParam );
void PrintCabTuneParam (CH_CAB_TuneParam_t* stInputParam );
void PrintAnaTuneParam (CH_ANA_TuneParam_t* stInputParam );
// End global function prototypes

#endif /* !__DI_CHANNEL_UTIL_H__ */
