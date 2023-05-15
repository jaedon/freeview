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
#include "di_err.h"
#include "fvk.h"
#include "fdi_cablemodem.h"

/*******************************************************************************
* Debug level
*******************************************************************************/

/*******************************************************************************
* Definitions, typdef & const data
*******************************************************************************/
	
#define FDI_CM_STATUS_TIMEOUT			17*1000 /* filed test후 최적화 필요 */
#define FDI_CM_STATUS_OPERATION		6

FDI_ERR_CODE FDI_CABLEMODEM_Status();
FDI_ERR_CODE FDI_CABLEMODEM_Scan();
FDI_ERR_CODE FDI_CABLEMODEM_GetSNR(int ChId, unsigned int *pulSnr);
FDI_ERR_CODE FDI_CABLEMODEM_GetPWR(int ChId, unsigned int *pulPwr);

/*******************************************************************************
* External variables and functions
*******************************************************************************/
/* external variables  and functions */

/*******************************************************************************
* Global variables and structures
*******************************************************************************/
/* global variables and structures */

/*******************************************************************************
* function :  FDI_CABLEMODEM_Init
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_CABLEMODEM_Init(void)
{
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
FDI_ERR_CODE FDI_CABLEMODEM_Open(void)
{
	return FDI_NO_ERR;
}

/*******************************************************************************
* function : FDI_CM_Status
* description :
* input :
* output :
* return : 
* 기타 참고자료 및 파일
*******************************************************************************/

FDI_ERR_CODE FDI_CABLEMODEM_Status()
{
	DI_ERR_CODE eDiErr=0;
	int ulWaitTime = 0;

	while(eDiErr<FDI_CM_STATUS_OPERATION) /* cable online status condition: ret  >=  6 */
	{
		FDI_PRINT(FDI_PRT_ERROR,("FDI_CM_Status[%d]=%d\n",ulWaitTime,eDiErr));
		eDiErr=DI_CM_CmStatus();
		FVK_TASK_Sleep(500);
		ulWaitTime += 500;
		if(FDI_CM_STATUS_TIMEOUT <= ulWaitTime)
		{
			FDI_PRINT(FDI_PRT_ERROR, ("FDI_CM_Status timeout:%d\n",ulWaitTime));
			return FDI_ERR;
		}
	}
	return eDiErr;
}
/*******************************************************************************
* function : FDI_CM_Scan
* description :
* input :
* output :
* return : 
* 기타 참고자료 및 파일
*******************************************************************************/

FDI_ERR_CODE FDI_CABLEMODEM_Scan()
{
	DI_ERR_CODE eDiErr;
	unsigned int ulFreq=0;

	eDiErr=DI_CM_SetScanDSFreq(&ulFreq);

	return eDiErr;
}
/*******************************************************************************
* function : FDI_CM_GetSNR
* description :
* input :
* output :
* return : 
* 기타 참고자료 및 파일
*******************************************************************************/

FDI_ERR_CODE FDI_CABLEMODEM_GetSNR(int ChId, unsigned int *pulSnr)
{
	DI_ERR_CODE eDiErr;
	unsigned int ulSnr;
	
	eDiErr=DI_CM_GetFDCSnr30(ChId, &ulSnr);
	*pulSnr=ulSnr;
	
	return eDiErr;	
}
/*******************************************************************************
* function : FDI_CM_GetPWR
* description :
* input :
* output :
* return : 
* 기타 참고자료 및 파일
*******************************************************************************/

FDI_ERR_CODE FDI_CABLEMODEM_GetPWR(int ChId, unsigned int *pulPwr)
{
	DI_ERR_CODE eDiErr;
	unsigned int ulPwr;
	
	eDiErr=DI_CM_GetRDCPower30(ChId, &ulPwr);
	*pulPwr=ulPwr;

	return eDiErr;

}


