/******************************************************************************
* $Workfile:$
* $Modtime:$
* Author :
* description :		 Factory Application Library
*
* Copyright (c) 2008 by Humax Co., Ltd.
* All right reserved
******************************************************************************/
#ifndef _FAI_CHANNEL_H_
#define _FAI_CHANNEL_H_

/******************************************************************************
* header and definitions
******************************************************************************/
#include "fdi_channel.h"
#include "fai_err.h"
#include "fai_testmenu.h"


/******************************************************************************
* Definitions, typdef & const data
******************************************************************************/
#if (PRODUCT_YSR2000==YES) /* fix for YSR2000 lock fail issue */
#define FAI_CHANNEL_LOCK_CHECK_RETRY_NUM			30 // 3
#define FAI_CHANNEL_LOCK_RETRY_DELAY				500 // 500
#else
#define FAI_CHANNEL_LOCK_CHECK_RETRY_NUM			10 // 3
#define FAI_CHANNEL_LOCK_RETRY_DELAY				300 // 500
#endif

#define FAI_CHANNEL_BER_TEST_SAMPLING_NUM			5
#define FAI_CHANNEL_BER_TEST_MINIMUM_SAMPLING_NUM	3
#define FAI_CHANNEL_BER_TEST_SAMPLING_DELAY			500
#define FAI_CHANNEL_AVG_VALUE_STR_LEN				20

#define FAI_CHANNEL_BER_TEST_RESULT_LEN 6
#define FAI_CHANNEL_AGC_TEST_RESULT_LEN 6
#define FAI_CHANNEL_SNR_TEST_RESULT_LEN 6

#define FAI_CHANNEL_TASK_NAME_LEN 20
#define FAI_CHANNEL_MSGQ_NAME_LEN 20

typedef enum
{
	FAI_CHANNEL_TMD_ERR_REQUEST_LOCK_FAIL = 0,
	FAI_CHANNEL_TMD_ERR_UNLOCK,
	FAI_CHANNEL_TMD_ERR_GET_SIGNAL_INFO_FAIL,
	FAI_CHANNEL_TMD_ERR_SET_LNB_POLARIZATION_FAIL,
	FAI_CHANNEL_TMD_ERR_SET_LNB_22KHZ_TONE_FAIL,
	FAI_CHANNEL_TMD_ERR_SET_LNB_OUTPUT_FAIL,
	FAI_CHANNEL_TMD_ERR_SET_LNB_LOOP_THROUGH_FAIL,
	FAI_CHANNEL_TMD_ERR_SET_ANTENNA_POWER_FAIL
}FAI_CHANNEL_TMD_ERR;

typedef enum
{
	FAI_CHANNEL_TEST_TYPE_SAT_REQUEST_LOCK,
	FAI_CHANNEL_TEST_TYPE_SAT_CHECK_PERFORMANCE, 
	FAI_CHANNEL_TEST_TYPE_CAB_REQUEST_LOCK,
	FAI_CHANNEL_TEST_TYPE_CAB_CHECK_PERFORMANCE, 
	FAI_CHANNEL_TEST_TYPE_TER_REQUEST_LOCK,
	FAI_CHANNEL_TEST_TYPE_TER_CHECK_PERFORMANCE, 
	FAI_CHANNEL_TEST_TYPE_NONE 
} FAI_CHANNEL_TEST_TYPE;

typedef struct FAI_CH_MSG
{
	unsigned short usMsgId;
	unsigned long ulTunerId;
	FAI_CHANNEL_TEST_TYPE eChTestType;
	FDI_CH_SAT_TUNE_PARAM_t tSatTuneParam;
	FDI_CH_CAB_TUNE_PARAM_t tCabTuneParam;
	FDI_CH_TER_TUNE_PARAM_t tTerTuneParam;
} FAI_CH_MSG_t;

typedef struct FAI_CH_TASK
{
	unsigned long ulCHID;
	unsigned long ulTaskID;
	unsigned long ulMSGQID;
	unsigned char aucTaskName[FAI_CHANNEL_TASK_NAME_LEN];
	unsigned char aucMSGQName[FAI_CHANNEL_MSGQ_NAME_LEN];
	FAI_ERR_CODE  (*pfnSATRequestLock)(unsigned short, unsigned long, FDI_CH_SAT_TUNE_PARAM_t);
	FAI_ERR_CODE  (*pfnSATCheckPerformance)(unsigned short, unsigned long, FDI_CH_SAT_TUNE_PARAM_t);
	FAI_ERR_CODE  (*pfnCABRequestLock)(unsigned short, unsigned long, FDI_CH_CAB_TUNE_PARAM_t);
	FAI_ERR_CODE  (*pfnCABCheckPerformance)(unsigned short, unsigned long, FDI_CH_CAB_TUNE_PARAM_t);
	FAI_ERR_CODE  (*pfnTERRequestLock)(unsigned short, unsigned long, FDI_CH_TER_TUNE_PARAM_t);
	FAI_ERR_CODE  (*pfnTERCheckPerformance)(unsigned short, unsigned long, FDI_CH_TER_TUNE_PARAM_t);
}FAI_CH_TASK_t;


/******************************************************************************
* Global variables and structures
******************************************************************************/


/******************************************************************************
* External variables and functions
******************************************************************************/


/******************************************************************************
* Static variables and structures
******************************************************************************/


/******************************************************************************
* Static function prototypes
******************************************************************************/

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_CHANNEL_Init(void);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_CHANNEL_S_PerformanceTuner0_UI();

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_CHANNEL_S_PerformanceTuner1_UI();

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_CHANNEL_S_Init(void);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_CHANNEL_S_LNBOutput(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_CHANNEL_S_RequestLock(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_CHANNEL_S_PerformanceCheck(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_CHANNEL_S_LNBBypass(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_CHANNEL_S_LNBOutput_UI(void);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_CHANNEL_T_Init(void);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_CHANNEL_T_RequestLock(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_CHANNEL_T_PerformanceCheck(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_CHANNEL_T_AntennaVolt(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_CHANNEL_T_AntennaVolt_UI(void);


/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_CHANNEL_T_PerformanceTuner0_UI(void);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_CHANNEL_T_PerformanceTuner1_UI(void);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_CHANNEL_C_Init(void);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_CHANNEL_C_RequestLock(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_CHANNEL_C_PerformanceCheck(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_CHANNEL_C_PerformanceTuner0_UI(void);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_CHANNEL_C_PerformanceTuner1_UI(void);


/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_CHANNEL_C_PerformanceTuner_UI0(void);
/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_CHANNEL_C_PerformanceTuner_UI1(void);
/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_CHANNEL_C_PerformanceTuner_UI2(void);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_CHANNEL_C_PerformanceTuner_UI3(void);
/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_CHANNEL_C_PerformanceTuner_UI4(void);
/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/

FAI_ERR_CODE FAI_CHANNEL_C_PerformanceTuner_UI5(void);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_CHANNEL_C_PerformanceTuner_UI6(void);
/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_CHANNEL_C_PerformanceTuner_UI7(void);


/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_CHANNEL_S_RequestLockFor4Ch(unsigned short usMsgId, unsigned char ucArgc, unsigned char *pucArgList);

#endif

