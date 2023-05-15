/*******************************************************************************
* File name : di_irb.c
* Author :
* description :
*
* Copyright (c) 2009 by Humax Co., Ltd.
* All right reserved
*******************************************************************************/

/*******************************************************************************
* header and definitions
*******************************************************************************/
/* global header files */
#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif
#include "bstd.h"

/* chip manufacture's  header files */
#include "nexus_base_types.h"
#include "nexus_platform.h"
#include "nexus_ir_blaster.h"
#include "nexus_ir_blaster_custom.h"

/* humax header files */
#include "drv_err.h"
#include "di_err.h"
#include "di_uart.h"
#include "vkernel.h"

#include "di_irb.h"

/*******************************************************************************
* Debug level
*******************************************************************************/
#define DEBUG_MODULE

#ifdef DEBUG_TEST_DI_IRB_ENTER_LEAVE
#define PrintEnter()	DI_UART_Print("(+)%s\n", __FUNCTION__)
#define PrintExit()		DI_UART_Print("(-)%s\n", __FUNCTION__)
#else
#define PrintEnter()
#define PrintExit()
#endif

#define PrintDebug		VK_printf
#define PrintError		VK_printf

/*******************************************************************************
* Definitions, typdef & const data
*******************************************************************************/
#define DRV_IRB_SEND_COMPLETE_TIMEOUT	(5*1000)

/*******************************************************************************
* Global variables and structures
*******************************************************************************/


/*******************************************************************************
* External variables and functions
*******************************************************************************/


/*******************************************************************************
* Static variables and structures
*******************************************************************************/
static NEXUS_IrBlasterHandle	s_pstIrbHandle = NULL;
static BKNI_EventHandle	s_pstIrbEventHandle = NULL;

/*******************************************************************************
* Static function prototypes
*******************************************************************************/
static void DRV_IRB_SendComplete_Callback(void *pContext, HUINT32 ulParam);


/*******************************************************************************
* function : DI_IRB_SendMessage
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
DI_ERR_CODE DI_IRB_SendMessage(DI_IRB_DATA_t *pstIrbData)
{
	DI_ERR_CODE	eDiError = DI_ERR_OK;
	NEXUS_Error	eNexusError = NEXUS_SUCCESS;
	BERR_Code	eBerrCode = BERR_SUCCESS;
	HUINT8	ucIndex = 0;
	HUINT32 ulCarrierDivisor = 0;
	NEXUS_IrBlasterCustomSettings	tIrbCumstomSetting;

	PrintEnter();

#ifdef DEBUG_DI_IRB
	PrintError("[Frequency] %d Hz \n", pstIrbData->ulCarrierFreqHz);
	PrintError("[Period] %d us \n", pstIrbData->ulPeriodTusec);
	PrintError("[RepeatCnt] %d \n", pstIrbData->ucRepeatCnt);
	PrintError("[RepeatStartIndex] %d \n", pstIrbData->ucRepeatStartIndex);
	PrintError("[LastIndex] %d \n", pstIrbData->ucLastIndex);
	PrintError("[DataBitCnt] %d \n", pstIrbData->ucDataBitCnt);
#endif

	/* 1. create frequency (carrier & modulating frequency) */
	NEXUS_IrBlaster_GetDefaultCustomSettings(&tIrbCumstomSetting);
	tIrbCumstomSetting.masterClockDivisor = 27;	/* 27/27 = 1MHz */
	tIrbCumstomSetting.carrierClockDivisor = 1;	/* 1/1 = 1MHz => 1us : carrier frequency */

	/* ex) 1/1 = 1MHz => 1us : modulation frequency, 436us를 만들기 위해 1us 단위가 유리하다. */
	tIrbCumstomSetting.indexClockDivisor = 1;
	tIrbCumstomSetting.noCarrier = FALSE;

	ulCarrierDivisor = (1000000/pstIrbData->ulCarrierFreqHz);
	if((1000000%pstIrbData->ulCarrierFreqHz) > (pstIrbData->ulCarrierFreqHz/2) )
	{
		ulCarrierDivisor = ulCarrierDivisor + 1;
	}
	tIrbCumstomSetting.carrierHighCount = ulCarrierDivisor/2;
	tIrbCumstomSetting.carrierLowCount = ulCarrierDivisor - tIrbCumstomSetting.carrierHighCount;

	/* 2. set period to each position */
	tIrbCumstomSetting.numberSequences = pstIrbData->ucDataBitCnt;	/* lead pulse + data + end(?) */
	for(ucIndex=0; ucIndex<pstIrbData->ucLastIndex; ucIndex++)
	{
		tIrbCumstomSetting.duration[ucIndex] = pstIrbData->aulDuration[ucIndex];
	}
    tIrbCumstomSetting.duration[pstIrbData->ucLastIndex] = pstIrbData->ulSpaceTusec;

	/* 2. set data sequence using duration field  */
	for(ucIndex=0; ucIndex<pstIrbData->ucDataBitCnt; ucIndex++)
	{
		tIrbCumstomSetting.sequenceIndex[ucIndex].on = pstIrbData->tData[ucIndex].ucOnPeriod;
		tIrbCumstomSetting.sequenceIndex[ucIndex].off = pstIrbData->tData[ucIndex].ucOffPeriod;
	}
    tIrbCumstomSetting.sequenceIndex[pstIrbData->ucDataBitCnt].on = 1;
    tIrbCumstomSetting.sequenceIndex[pstIrbData->ucDataBitCnt].off = pstIrbData->ucLastIndex;

	tIrbCumstomSetting.repeatCount = pstIrbData->ucRepeatCnt;
	tIrbCumstomSetting.repeatStartIndex = pstIrbData->ucRepeatStartIndex;

	eNexusError = NEXUS_IrBlaster_SetCustomSettings(s_pstIrbHandle, &tIrbCumstomSetting);
	eNexusError |= NEXUS_IrBlaster_Send(s_pstIrbHandle, "", 0, FALSE);
	if(eNexusError != NEXUS_SUCCESS)
	{
		eDiError = DI_ERR_ERROR;
		goto done;
	}

	eBerrCode = BKNI_WaitForEvent(s_pstIrbEventHandle, DRV_IRB_SEND_COMPLETE_TIMEOUT);
	if(eBerrCode != BERR_SUCCESS)
	{
		PrintError("[%s] Error, eBerrCode : 0x%x \n", __func__, eBerrCode);
		eDiError = DI_ERR_ERROR;
	}

done:
	PrintExit();
	return eDiError;
}

/*******************************************************************************
* function : DRV_IRB_Init
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
DRV_Error DRV_IRB_Init(void)
{
	DRV_Error	eDrvError = DRV_OK;
	NEXUS_IrBlasterSettings	tIrbSetting;

	PrintEnter();

	BKNI_CreateEvent(&s_pstIrbEventHandle);

	NEXUS_IrBlaster_GetDefaultSettings(&tIrbSetting);

	tIrbSetting.mode = NEXUS_IrBlasterMode_eCustom;
	tIrbSetting.transmitComplete.callback = (NEXUS_Callback)DRV_IRB_SendComplete_Callback;
	tIrbSetting.transmitComplete.context = (void *)s_pstIrbEventHandle;
	s_pstIrbHandle = NEXUS_IrBlaster_Open(0, &tIrbSetting);
	if(s_pstIrbHandle == NULL)
	{
		eDrvError = DI_ERR_ERROR;
	}

	PrintExit();
	return eDrvError;
}

/*******************************************************************************
* function : DRV_IRB_SendComplete_Callback
* description : IRB sending이 완료되면 driver로부터 불린다.
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
static void DRV_IRB_SendComplete_Callback(void *pContext, HUINT32 ulParam)
{
	BKNI_EventHandle	tEventHandle;

	BSTD_UNUSED(ulParam);

	tEventHandle = (BKNI_EventHandle)pContext;

	BKNI_SetEvent(tEventHandle);

	PrintDebug("[DRV_IRB_SendComplete_Callback] complete send data !!\n");
}

/* end of file */

