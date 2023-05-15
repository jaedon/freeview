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
#include "di_irb.h"

/* humax header files */
#include "fdi_common.h"
#include "fdi_irb.h"


/*******************************************************************************
* Debug level
*******************************************************************************/


/*******************************************************************************
* Definitions, typdef & const data
*******************************************************************************/
#define FDI_IRB_CARRIER_FREQENCY	36700 /* Hz */
#define FDI_IRB_ONE_TIME_PERIOD		436 /* us */


/*******************************************************************************
* External variables and functions
*******************************************************************************/


/*******************************************************************************
* Global variables and structures
*******************************************************************************/


/*******************************************************************************
* Global variables and structures
*******************************************************************************/
static unsigned char s_aucHumaxRcu2CodeCmd[6] = {0x01, 0xAA, 0xA1, 0x12, 0x05, 0x1D};


/*******************************************************************************
* function : FDI_KEY_Init
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_IRB_Open(void)
{
	return FDI_NO_ERR;
}

/*******************************************************************************
* function : FDI_IRB_SendData
* description : send data of PRODUCT ir format
* input : key value for sending
* output : none
* return : FDI_ERR_CODE
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_IRB_SendData(unsigned char ucKeyValue) /* wrapping */
{
	FDI_ERR_CODE	eFdiError = FDI_NO_ERR;
	DI_ERR_CODE	eDiError = DI_ERR_OK;
	DI_IRB_DATA_t	tIrbData;
	unsigned char	ucIrDataCount = 0;
	unsigned char	ucIndexI = 0;
	unsigned char	ucIndexJ = 0;
	
	FDI_FUNC_ENTER;

	ucIrDataCount = sizeof(s_aucHumaxRcu2CodeCmd);
	
	tIrbData.bUseCarrier = TRUE;
	tIrbData.ucDataBitCnt = ucIrDataCount * 8 + 2;
	tIrbData.ulCarrierFreqHz = FDI_IRB_CARRIER_FREQENCY;
	tIrbData.ulPeriodTusec = FDI_IRB_ONE_TIME_PERIOD;		
	tIrbData.aulDuration[0] = tIrbData.ulPeriodTusec * 0;	/* 0us */
	tIrbData.aulDuration[1] = tIrbData.ulPeriodTusec * 1;		
	tIrbData.aulDuration[2] = tIrbData.ulPeriodTusec * 2;		
	tIrbData.aulDuration[3] = tIrbData.ulPeriodTusec * 3;		
	tIrbData.aulDuration[4] = tIrbData.ulPeriodTusec * 4;		
	tIrbData.aulDuration[5] = tIrbData.ulPeriodTusec * 8;		
	tIrbData.ucLastIndex = 6;	/* duration last index */				

	/* make data */
	s_aucHumaxRcu2CodeCmd[4] = ucKeyValue;
	s_aucHumaxRcu2CodeCmd[5] = 0;
	for(ucIndexI = 0; ucIndexI < 5; ucIndexI++)
	{
		s_aucHumaxRcu2CodeCmd[5] ^= s_aucHumaxRcu2CodeCmd[ucIndexI];
	}
	
	tIrbData.tData[0].ucOnPeriod = 5;		
	tIrbData.tData[0].ucOffPeriod = 4;		
	for(ucIndexJ=0; ucIndexJ<ucIrDataCount; ucIndexJ++)		
	{			
		for(ucIndexI=0; ucIndexI<8; ucIndexI++)		
		{				
			if(((s_aucHumaxRcu2CodeCmd[ucIndexJ]>>ucIndexI) & 0x01) == 0)		
			{					
				tIrbData.tData[(ucIndexJ*8) + 1 + ucIndexI].ucOnPeriod = 1;		
				tIrbData.tData[(ucIndexJ*8) + 1 + ucIndexI].ucOffPeriod = 1;	
			}			
			else				
			{				
				tIrbData.tData[(ucIndexJ*8) + 1 + ucIndexI].ucOnPeriod = 1;		
				tIrbData.tData[(ucIndexJ*8) + 1 + ucIndexI].ucOffPeriod = 3;		
			}			
		}		
	}		

	tIrbData.tData[tIrbData.ucDataBitCnt - 1] = tIrbData.tData[tIrbData.ucDataBitCnt - 2];		
	eDiError = DI_IRB_SendMessage(&tIrbData);	
	if(eDiError != DI_ERR_OK)
	{
		FDI_PRINT(FDI_PRT_ERROR, (" IRB Send Message Error: 0x%x \n", eDiError));
	}
	
	FDI_FUNC_EXIT;
	return eFdiError;
}


