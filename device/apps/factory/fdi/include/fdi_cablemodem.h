/******************************************************************************
* $Workfile:$
* $Modtime:$
* Author :
* description :		 Factory Error Code Definition
*
* Copyright (c) 2008 by Humax Co., Ltd.
* All right reserved
******************************************************************************/
#ifndef _FDI_CABLEMODEM_H_
#define _FDI_CABLEMODEM_H_

/******************************************************************************
* header and definitions
******************************************************************************/
#include "fdi_err.h"
#include "fdi_common.h"

/******************************************************************************
* Definitions, typdef & const data
******************************************************************************/

typedef enum
{
	CM_EVENT_ONLINE=0,
	CM_EVENT_MAX
} FDI_CM_EVENT;

/******************************************************************************
* Global variables and structures
******************************************************************************/
/* global variables and structures */

/******************************************************************************
* External variables and functions
******************************************************************************/
/* external variables  and functions */

/******************************************************************************
* Static variables and structures
******************************************************************************/
/* static variables and structures */

/******************************************************************************
* Static function prototypes
******************************************************************************/
FDI_ERR_CODE FDI_CABLEMODEM_Init(void);
FDI_ERR_CODE FDI_CABLEMODEM_Open(void);
FDI_ERR_CODE FDI_CABLEMODEM_Status();
FDI_ERR_CODE FDI_CABLEMODEM_Scan();
FDI_ERR_CODE FDI_CABLEMODEM_GetSNR(int ChId, unsigned int *pulSnr);
FDI_ERR_CODE FDI_CABLEMODEM_GetPWR(int ChId, unsigned int *pulPwr);

#endif

