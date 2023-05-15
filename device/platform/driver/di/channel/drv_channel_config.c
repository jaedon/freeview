/* $Header: */
/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
// File Name: drv_ts_path.c
// Original Author:
// File Description:
// Remarks:

/*******************************************************************/
/* Copyright (c) 2008 HUMAX Co., Ltd.								*/
/* All rights reserved. 												*/
/*******************************************************************/

/*******************************************************************/
/* INCLUDE FILES													*/
/*******************************************************************/
#include "htype.h"
#include "drv_channel_config.h"

/*******************************************************************/
/* EXTERN VARIABLES 												*/
/*******************************************************************/
extern DRV_CH_CAPABILITY s_ChConfig[];
extern DRV_CH_TSOUTPUTMODE s_TSConfig[];
extern DI_CH_TunerCapacity_t s_SatTunerCapacity[];
extern HUINT32 DRV_CFG_GetMaxChannel(void);

/*******************************************************************/
/* MACRO DEFINITION 												*/
/*******************************************************************/


/*******************************************************************/
/* GLOBAL FUNCTION PROTOTYPE									*/
/*******************************************************************/


/*****************************************************************************
* function: DRV_TS_PATH_GetInbandID
* description : Get Physical input band id from virtual ts id
* input:
*   ulDevice_id : virtual ts id
* return:
*    NEXUS_InputBand_eMax : invalid device id
*    others : physical input band id
******************************************************************************/
HUINT32 DRV_CH_GetMaxInstance(void) // Max Tuner Num
{
	int nMaxChannelNum , i;

	HUINT32 ulTuner_id= 0;

	nMaxChannelNum = DRV_CFG_GetMaxChannel();

	for(i=0; i<nMaxChannelNum; i++)
	{
		if( i ==  s_ChConfig[i].ulTuner_id)
		{
			ulTuner_id++;
		}
	}

	return ulTuner_id;
}

HUINT32 DRV_CH_GetMaxChannel(void)
{
	return DRV_CFG_GetMaxChannel();
}

HUINT32 DRV_CH_GetTunerType(int nDevice_id)
{
	if(nDevice_id >= (int)DRV_CFG_GetMaxChannel())
	{
		return -1;
	}
	else
	{
		return s_ChConfig[nDevice_id].ulChannelType;
	}
}

CH_TsOutMode_e DRV_CH_GetTSOutputMode(int nChannel_id)
{
	if(nChannel_id >= (int)DRV_CFG_GetMaxChannel())
	{
		return -1;
	}
	else
	{
		return s_TSConfig[nChannel_id].etTSoutput;
	}
}

DI_CH_TunerCapacity_t * DRV_CH_GetTunerCapability(int nChannel_id)
{
	if(nChannel_id >= (int)DRV_CFG_GetMaxChannel())
	{
		return NULL;
	}
	else
	{
		return &s_SatTunerCapacity[nChannel_id];
	}
}

