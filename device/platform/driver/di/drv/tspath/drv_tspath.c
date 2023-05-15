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
#include "drv_tspath.h"

/*******************************************************************/
/* EXTERN VARIABLES 												*/
/*******************************************************************/
extern DRV_TS_PATH_ID s_TsIds[];
extern DRV_CI_REMUX_PATH_ID s_RemuxIds[];

extern HUINT32 DRV_CFG_GetTSMaxDevice(void);

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
NEXUS_InputBand DRV_TS_PATH_GetInbandID(HUINT32 ulDevice_id)
{
	if(ulDevice_id >= DRV_CFG_GetTSMaxDevice())
	{
		return NEXUS_InputBand_eMax;
	}
	else
	{
		return s_TsIds[ulDevice_id].ulInband_id;
	}
}

/*****************************************************************************
* function: DRV_TS_PATH_GetDemodID
* description : Get Physical demodulator id from virtual ts id
* input: 
*   ulDevice_id : virtual ts id
* return: 
*    -1 : invalid device id
*    others : physical demodulator id
******************************************************************************/
HINT32 DRV_TS_PATH_GetDemodID(HUINT32 ulDevice_id)
{
	if(ulDevice_id >= DRV_CFG_GetTSMaxDevice())
	{
		return -1;
	}
	else
	{
		return s_TsIds[ulDevice_id].ulDemod_id;
	}
}

/*****************************************************************************
* function: DRV_CIRemux_PATH_GetInbandID
* description : Get Physical input band id for Remux
* input: 
*   ulDevice_id : virtual ts id
* return: 
*    others : physical input band id
******************************************************************************/
NEXUS_InputBand DRV_CIRemux_PATH_GetInbandID(HUINT32 ulDevice_id)
{
	return s_RemuxIds[ulDevice_id].ulRemuxInband_id;
}

