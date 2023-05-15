/******************************************************************************
* $Workfile:$
* $Modtime:$
* Author : 
* description :		 Factory Application
*
* Copyright (c) 2008 by Humax Co., Ltd.
* All right reserved
******************************************************************************/

/*******************************************************************************
* header and definitions
*******************************************************************************/
/* global header files */

/* chip manufacture's  header files */

/* humax header files */
#include "fdi_common.h"
#include "fdi_channel.h"

#include "di_err.h"
#include "di_channel.h"

/*******************************************************************************
* Debug level
*******************************************************************************/

/*******************************************************************************
* Definitions, typdef & const data
*******************************************************************************/

/*******************************************************************************
* External variables and functions
*******************************************************************************/
/* external variables  and functions */

/*******************************************************************************
* Global variables and structures
*******************************************************************************/
/* global variables and structures */

static unsigned long s_ulRequestId;

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_CHANNEL_GetNumOfDevice(unsigned long *pulNumOfDevice)
{
	if(pulNumOfDevice == NULL)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_CHANNEL_GetNumOfDevice] Invalid Parameter. pulNumOfDevice==NULL\n"));
		return FDI_ERR;
	}
	
	/* get num of channel instance */
	DI_CH_GetNumOfInstance((HUINT32*)pulNumOfDevice);

	FDI_PRINT(FDI_PRT_DBGINFO, ("[FDI_CHANNEL_GetNumOfDevice] : --\n"));

	return FDI_NO_ERR;
}

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_CHANNEL_GetCapability(unsigned long ulDeviceId, FDI_CH_TYPE_e *peType)
{
	unsigned long ulChType = 0;
	
	if(peType == NULL)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_CHANNEL_GetCapability] Invalid Parameter. peType==NULL\n"));
		return FDI_ERR;
	}

	/* get channel type */
	*peType = 0;
	DI_CH_GetCapability(ulDeviceId, (HUINT32*)&ulChType);

	if(ulChType & DI_CH_TYPE_SAT)
	{
		*peType |= FDI_CH_TYPE_SAT;
	}

	if(ulChType & DI_CH_TYPE_TER)
	{
		*peType |= FDI_CH_TYPE_TER;
	}

	if(ulChType & DI_CH_TYPE_CAB)
	{
		*peType |= FDI_CH_TYPE_CAB;
	}

	if(ulChType & DI_CH_TYPE_ANA)
	{
		*peType |= FDI_CH_TYPE_ANALOG;
	}

	return FDI_NO_ERR;
}

