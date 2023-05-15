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

#ifndef	__DRV_CHANNEL_A_H__
#define	__DRV_CHANNEL_A_H__

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
DI_CHANNEL_ERR CH_A_SetTune (int nChannelID, HUINT32 unRequestID);
HUINT32 CH_A_CheckStatus (int nChannelId, HUINT32 unWorkingId);
DI_CHANNEL_ERR CH_A_CheckTuneParams (CH_ANA_TuneParam_t * pstParam);

// End global function prototypes

#endif /* !__DRV_CHANNEL_A_H__ */
