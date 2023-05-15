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

#ifndef __DRV_TS_PATH_H__
#define __DRV_TS_PATH_H__

/*******************************************************************/
/* INCLUDE FILES													*/
/*******************************************************************/
#include "htype.h"
#include "nexus_types.h"
#include "nexus_platform.h"

/*******************************************************************/
/* EXTERN VARIABLES 												*/
/*******************************************************************/


/*******************************************************************/
/* MACRO DEFINITION 												*/
/*******************************************************************/
typedef struct
{
	NEXUS_InputBand ulInband_id;
	HINT32 ulDemod_id;
} DRV_TS_PATH_ID;


/*******************************************************************/
/* MACRO DEFINITION 												*/
/*******************************************************************/
typedef struct
{
	NEXUS_InputBand ulRemuxInband_id;
} DRV_CI_REMUX_PATH_ID;


/*******************************************************************/
/* GLOBAL FUNCTION PROTOTYPE									*/
/*******************************************************************/

/*****************************************************************************
* function: DRV_TS_PATH_GetInbandID
* description : Get Physical input band id from virtual demux ts id
* input: 
*   ulDevice_id : virtual demux ts id
* return: 
*    NEXUS_InputBand_eMax : invalid device id
*    others : physical input band id
******************************************************************************/
NEXUS_InputBand DRV_TS_PATH_GetInbandID(HUINT32 ulDevice_id);

/*****************************************************************************
* function: DRV_TS_PATH_GetDemodID
* description : Get Physical demodulator id from virtual demux ts id
* input: 
*   ulDevice_id : virtual demux ts id
* return: 
*    -1 : invalid device id
*    others : physical demodulator id
******************************************************************************/
HINT32 DRV_TS_PATH_GetDemodID(HUINT32 ulDevice_id);


/*****************************************************************************
* function: DRV_CIRemux_PATH_GetInbandID
* description : Get Physical input band id for Remux
* input: 
*   ulDevice_id : virtual ts id
* return: 
*    others : physical input band id
******************************************************************************/
	NEXUS_InputBand DRV_CIRemux_PATH_GetInbandID(HUINT32 ulDevice_id);


#endif /* !__DRV_TS_PATH_H__ */

