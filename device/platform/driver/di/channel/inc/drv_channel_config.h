/* $Header: */
/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
// File Name: drv_ts_path.h
// Original Author:
// File Description:
// Remarks:

/*******************************************************************/
/* Copyright (c) 2008 HUMAX Co., Ltd.								*/
/* All rights reserved. 												*/
/*******************************************************************/

#ifndef __DRV_CHANNEL_CONFIG_H__
#define __DRV_CHANNEL_CONFIG_H__

/*******************************************************************/
/* INCLUDE FILES													*/
/*******************************************************************/
#include "htype.h"
#include "di_channel_priv.h"

/*******************************************************************/
/* EXTERN VARIABLES 												*/
/*******************************************************************/


/*******************************************************************/
/* MACRO DEFINITION 												*/
/*******************************************************************/
typedef struct
{
	HINT32   ulTuner_id;
	HUINT32 ulChannelType;	
} DRV_CH_CAPABILITY;

typedef struct
{
	HINT32   ulChannelType;
	CH_TsOutMode_e   etTSoutput;	
} DRV_CH_TSOUTPUTMODE;


/*******************************************************************/
/* GLOBAL FUNCTION PROTOTYPE									*/
/*******************************************************************/
HUINT32 DRV_CH_GetMaxInstance(void);
HUINT32 DRV_CH_GetMaxChannel(void);

HUINT32 DRV_CH_GetTunerType(int nDevice_id);
HUINT32 DRV_CH_GetTSOutputMode(int ulChannel_id);

DI_CH_TunerCapacity_t * DRV_CH_GetTunerCapability(int nChannel_id);

#endif /* !__DRV_CHANNEL_CONFIG_H__ */

