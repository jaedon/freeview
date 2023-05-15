/*******************************************************************************
* File name : di_irb.c
* Author : 
* description :
*
* Copyright (c) 2007 by Humax Co., Ltd.
* All right reserved
*******************************************************************************/

#ifndef __DI_IRB_H__
#define __DI_IRB_H__

/*******************************************************************************
* header and definitions
*******************************************************************************/
/* global header files */

/* humax header files */
#include "htype.h"
#include "di_err.h"

/*******************************************************************************
* Definitions, typdef & const data
*******************************************************************************/
typedef struct
{
	HBOOL	bUseCarrier;
	HUINT32	ulCarrierFreqHz;	/* carrier frequency */
	HUINT32	ulPeriodTusec;		/* 1 T period */
	HUINT32	ulSpaceTusec;		/* 마지막 data bit 에서 repeat 코드 전까지의 길이 */
	HUINT8	ucRepeatCnt;		/* if want to send a frame repeatly */
	HUINT8	ucRepeatStartIndex;	/* start index of repeat pulse */
	HUINT32	aulDuration[14];	/* high/low duration(real period /usec) */
	HUINT8	ucLastIndex;		/* last duration index */
	HUINT8	ucDataBitCnt;	/* total data bit count (include Lead/End pulse) */
	struct
	{
		HUINT8	ucOnPeriod;		/* T */
		HUINT8	ucOffPeriod;	/* T */	
	}tData[80];	/* duration pointer for actual T value */
}DI_IRB_DATA_t;


/*******************************************************************************
* Global variables and structures
*******************************************************************************/


/*******************************************************************************
* Function prototype
*******************************************************************************/
DI_ERR_CODE DI_IRB_SendMessage(DI_IRB_DATA_t *pstIrbData);


#endif /* __DI_IRB_H__ */
