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
#include "fvk.h"
#include "fai_common.h"
#include "fai_testmenu.h"
#include "fai_tmd.h"
#include "fai_channel.h"
#include "fdi_channel.h"
#include "fdi_panel.h"

/* model specific configuration header */
#include "factorysw_config.h"

/*******************************************************************************
* Debug level
*******************************************************************************/

/*******************************************************************************
* Definitions, typdef & const data
*******************************************************************************/
#define FAI_CHANNEL_C_LOCKING_WAIT		4000
#define FAI_CHANNEL_C_CHANNEL_PERFORMANCE_EXP 7

#define FAI_TMD_PARAM_AGC_ID			0
#define FAI_TMD_PARAM_BER_ID			1
#define FAI_TMD_PARAM_SNR_ID			2

typedef enum
{
	FAI_CHANNEL_C_TEST_TYPE_PERFORMANCE_CHECK_UI,
	FAI_CHANNEL_C_TEST_TYPE_PERFORMANCE_CHECK_UI_T0,
	FAI_CHANNEL_C_TEST_TYPE_PERFORMANCE_CHECK_UI_T1,
	FAI_CHANNEL_C_TEST_TYPE_NONE 
} FAI_CHANNEL_C_TEST_TYPE;

typedef struct FAI_CH_C_MSG
{
	FAI_CHANNEL_C_TEST_TYPE eChTestType;
	unsigned long ulTunerId;
	FDI_CH_CAB_TUNE_PARAM_t tCabTuneParam;
	unsigned long ulAgcMin;
	unsigned long ulAgcMax;
	unsigned long ulBerMin;
	unsigned long ulBerMax;
	unsigned long ulSnrMin;
	unsigned long ulSnrMax;		
	FAI_TESTMENU_H hChMenu;
} FAI_CH_C_MSG_t;


/*******************************************************************************
* External variables and functions
*******************************************************************************/
/* external variables  and functions */
extern unsigned long g_ulNumofCHDevice;
extern FAI_CH_TASK_t *g_pstCHTask;
extern BOOL g_bLocked;


/*******************************************************************************
* Global variables and structures
*******************************************************************************/
/* global variables and structures */
static unsigned long s_ulCabChMsgIdForUI, s_ulCabChTaskIdForUI;
static FAI_CHANNEL_C_TEST_TYPE s_ePrevTest = FAI_CHANNEL_C_TEST_TYPE_NONE;

static void INTRN_FAI_CHANNEL_C_Task_UI(void *data);
static FAI_ERR_CODE INTRN_FAI_CHANNEL_C_PerformanceCheck_UI(unsigned long ulTunerId, 
													FDI_CH_CAB_TUNE_PARAM_t tCabTuneParam, 
													unsigned long ulBerRangeMin, unsigned long ulBerRangeMax, 
													unsigned long ulAgcRangeMin, unsigned long ulAgcRangeMax, 
													unsigned long ulSnrRangeMin, unsigned long ulSnrRangeMax, 
													FAI_TESTMENU_H hMenu);
static FAI_ERR_CODE INTRN_FAI_CHANNEL_C_PerformanceForJp_UI(unsigned long ulTunerId, 
													FDI_CH_CAB_TUNE_PARAM_t tCabTuneParam, 
													long lBerRangeMin, long lBerRangeMax, 
													long lSnrRangeMin, long lSnrRangeMax, 
													FAI_TESTMENU_H hMenu, 
													BOOL bForceLockTry);

static FAI_ERR_CODE INTRN_FAI_CHANNEL_C_CheckPerformance(unsigned short usMsgId, unsigned long ulTunerId, FDI_CH_CAB_TUNE_PARAM_t tCabTuneParam); 
static FAI_ERR_CODE INTRN_FAI_CHANNEL_C_RequestLock(unsigned short usMsgId, unsigned long ulTunerId, FDI_CH_CAB_TUNE_PARAM_t tCabTuneParam);


/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_CHANNEL_C_Init(void)
{
	int i;
	
#if ((CAB_CH_PERFORMANCE_TUNER_UI0_ENABLE==YES) || (CAB_CH_PERFORMANCE_TUNER0_UI_ENABLE==YES) || (CAB_CH_PERFORMANCE_TUNER1_UI_ENABLE==YES))
	if( FVK_MSG_Create(FVK_TASK_DEFAULT_QUEUE_SIZE, sizeof(FAI_CH_C_MSG_t), (char*)"CabChMsgForUI", &s_ulCabChMsgIdForUI) != FVK_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_CHANNEL_C_Init] : FVK_MSG_Create \n"));
		return FVK_ERR;
	}
	if( FVK_TASK_Create(INTRN_FAI_CHANNEL_C_Task_UI, FVK_TASK_DEFAULT_PRIORITY, FVK_TASK_DEFAULT_STACK_SIZE, "CabChTaskForUI", NULL, &s_ulCabChTaskIdForUI) != FVK_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_CHANNEL_C_Init] : FVK_TASK_Create Error!!\n"));
		return FVK_ERR;
	}
	if( FVK_TASK_Start(s_ulCabChTaskIdForUI) != FVK_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_CHANNEL_C_Init] : FVK_TASK_Start Error!!\n"));
		return FVK_ERR;
	}
#endif

#if ((CHANNEL_CAB_LOCK_REQUEST_ENABLE==YES) || (CHANNEL_CAB_LOCK_FOR_PERFORMANCE_ENABLE==YES))
	for(i=0; i<g_ulNumofCHDevice; i++)
	{
		g_pstCHTask[i].pfnCABRequestLock = INTRN_FAI_CHANNEL_C_RequestLock;
		g_pstCHTask[i].pfnCABCheckPerformance = INTRN_FAI_CHANNEL_C_CheckPerformance;
	}
#endif

	return FAI_NO_ERR;
}


/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
#if (CHANNEL_CAB_LOCK_REQUEST_ENABLE==YES)
FAI_ERR_CODE FAI_CHANNEL_C_RequestLock(unsigned short usMsgId, unsigned char ucArgc, unsigned char *pucArgList)
{
	unsigned char ucTunerId, aucFreqeuncy[4], aucSymbolate[2], ucModulation;
	unsigned char ucFreqLen, ucSymbolLen;
	FAI_CH_MSG_t tChMsg;
	FVK_ERR_CODE eVkRet;
	FAI_TMD_RESULT_T tTMDResult;
	unsigned char ucTmdErrCode[2];
	FVK_MEM_Set(&tTMDResult, 0x00, sizeof(tTMDResult));
	
	if( ucArgc != 4 )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[%s] : Argument Count Error=%d\n", __func__, ucArgc));
		tTMDResult.eResult = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
		ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_INVALID_ARGUMENT;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));	
		goto TEST_FAIL;
	}

	/* get params */
	FAI_TMD_GetParam(&ucTunerId, pucArgList, 0);
	ucFreqLen = FAI_TMD_GetParam(aucFreqeuncy, pucArgList, 1);
	ucSymbolLen = FAI_TMD_GetParam(aucSymbolate, pucArgList, 2);
	FAI_TMD_GetParam(&ucModulation, pucArgList, 3);

	if(ucTunerId >= g_ulNumofCHDevice)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[%s] : Invalid parameter Error, TunerID =%d\n", __func__, ucTunerId));
		tTMDResult.eResult = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
		ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_INVALID_PARAMETER;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));	
		goto TEST_FAIL;
	}
	
	/* convert params */
	tChMsg.usMsgId = usMsgId;
	tChMsg.ulTunerId = (unsigned long)ucTunerId;
	tChMsg.eChTestType = FAI_CHANNEL_TEST_TYPE_CAB_REQUEST_LOCK;
	FAI_TMD_ArgToValue(&tChMsg.tCabTuneParam.ulFrequency, aucFreqeuncy, ucFreqLen);
	FAI_TMD_ArgToValue(&tChMsg.tCabTuneParam.ulSymbolRate, aucSymbolate, ucSymbolLen);
	tChMsg.tCabTuneParam.etModulation = ucModulation;
	tChMsg.tCabTuneParam.etSpectrum = FDI_CH_INVERSION_AUTO;

	FAI_PRINT(FAI_PRT_DBGINFO, ("===============================================\n"));
	FAI_PRINT(FAI_PRT_DBGINFO, ("[%s]\n", __func__));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tucTunerId=%d\n",tChMsg.ulTunerId));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\teChTestType=%d\n",tChMsg.eChTestType));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tfreq=%d\n", tChMsg.tCabTuneParam.ulFrequency));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tsymbol=%d\n", tChMsg.tCabTuneParam.ulSymbolRate));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tmod=%d\n", tChMsg.tCabTuneParam.etModulation));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tSpectrum=%d\n", tChMsg.tCabTuneParam.etSpectrum));
	FAI_PRINT(FAI_PRT_DBGINFO, ("===============================================\n"));

	eVkRet = FVK_MSG_Send(g_pstCHTask[ucTunerId].ulMSGQID, &tChMsg, sizeof(FAI_CH_MSG_t));
	if ( eVkRet != FVK_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_CHANNEL_C_PerformanceTuner1_UI] : Error FVK_MSG_Send() : %d\n", eVkRet));
		tTMDResult.eResult = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
		ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_MESSAGE_SEND_FAIL;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));	
		goto TEST_FAIL;
	}

	return FAI_NO_ERR;

TEST_FAIL:
	tTMDResult.ucCategory = FAI_CATEGORY_CHANNEL;
	tTMDResult.ucModule = 0x03;
	tTMDResult.ucAction = 0x01;
	tTMDResult.usMsgId = usMsgId;
	FAI_TMD_SetResult(&tTMDResult);
	return FAI_ERR;
}
#endif
/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
#if (CAB_CH_PERFORMANCE_TUNER_UI0_ENABLE==YES)
FAI_ERR_CODE FAI_CHANNEL_C_PerformanceTuner_UI0(void)
{
	FAI_CH_C_MSG_t tChMsg;
	FVK_ERR_CODE eVkRet;
	static unsigned long ulTunerId = 1;
#if (CHANNEL_CAB_MULTI_TUNER_ENABLE==YES)
	tChMsg.ulTunerId = 0;
	ulTunerId = 0;

#else
	if(ulTunerId == 0)
	{
		tChMsg.ulTunerId = 1;
		ulTunerId = 1;
	}
	else
	{
		tChMsg.ulTunerId = 0;
		ulTunerId = 0;
	}
#endif

	tChMsg.eChTestType = FAI_CHANNEL_C_TEST_TYPE_PERFORMANCE_CHECK_UI;
	tChMsg.tCabTuneParam.ulFrequency = CAB_CH_PERFORMANCE_TUNER_UI0_FREQUENCY;
	tChMsg.tCabTuneParam.ulSymbolRate = CAB_CH_PERFORMANCE_TUNER_UI0_SYMBOL_RATE;
	tChMsg.tCabTuneParam.etModulation = FDI_CH_CMOD_AUTO;
	tChMsg.tCabTuneParam.etSpectrum = FDI_CH_INVERSION_AUTO;
	tChMsg.ulAgcMin = CAB_CH_PERFORMANCE_TUNER_UI0_AGC_MIN;
	tChMsg.ulAgcMax = CAB_CH_PERFORMANCE_TUNER_UI0_AGC_MAX;
	tChMsg.ulBerMin = CAB_CH_PERFORMANCE_TUNER_UI0_BER_MIN;
	tChMsg.ulBerMax = CAB_CH_PERFORMANCE_TUNER_UI0_BER_MAX;
	tChMsg.ulSnrMin = CAB_CH_PERFORMANCE_TUNER_UI0_SNR_MIN;
	tChMsg.ulSnrMax = CAB_CH_PERFORMANCE_TUNER_UI0_SNR_MAX;
	
	FAI_TESTMENU_GetCurrentMenu(&tChMsg.hChMenu);

	eVkRet = FVK_MSG_Send(s_ulCabChMsgIdForUI, &tChMsg, sizeof(FAI_CH_C_MSG_t));
	if ( eVkRet != FVK_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_CHANNEL_C_PerformanceTuner0_UI] : Error FVK_MSG_Send() : %d\n", eVkRet));
		return FAI_ERR;
	}
	return FAI_NO_ERR;
}
#endif

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
#if (CAB_CH_PERFORMANCE_TUNER_UI1_ENABLE==YES)
FAI_ERR_CODE FAI_CHANNEL_C_PerformanceTuner_UI1(void)
{
	FAI_CH_C_MSG_t tChMsg;
	FVK_ERR_CODE eVkRet;
	static unsigned long ulTunerId = 1;
#if (CHANNEL_CAB_MULTI_TUNER_ENABLE==YES)
	tChMsg.ulTunerId = 1;
	ulTunerId = 1;
	
#else

	if(ulTunerId == 0)
	{
		tChMsg.ulTunerId = 1;
		ulTunerId = 1;
	}
	else
	
	{
		tChMsg.ulTunerId = 0;
		ulTunerId = 0;
	}
#endif	
	tChMsg.ulTunerId = 1;
	ulTunerId = 1;
	tChMsg.eChTestType = FAI_CHANNEL_C_TEST_TYPE_PERFORMANCE_CHECK_UI;
	tChMsg.tCabTuneParam.ulFrequency = CAB_CH_PERFORMANCE_TUNER_UI1_FREQUENCY;
	tChMsg.tCabTuneParam.ulSymbolRate = CAB_CH_PERFORMANCE_TUNER_UI1_SYMBOL_RATE;
	tChMsg.tCabTuneParam.etModulation = FDI_CH_CMOD_AUTO;
	tChMsg.tCabTuneParam.etSpectrum = FDI_CH_INVERSION_AUTO;
	tChMsg.ulAgcMin = CAB_CH_PERFORMANCE_TUNER_UI1_AGC_MIN;
	tChMsg.ulAgcMax = CAB_CH_PERFORMANCE_TUNER_UI1_AGC_MAX;
	tChMsg.ulBerMin = CAB_CH_PERFORMANCE_TUNER_UI1_BER_MIN;
	tChMsg.ulBerMax = CAB_CH_PERFORMANCE_TUNER_UI1_BER_MAX;
	tChMsg.ulSnrMin = CAB_CH_PERFORMANCE_TUNER_UI1_SNR_MIN;
	tChMsg.ulSnrMax = CAB_CH_PERFORMANCE_TUNER_UI1_SNR_MAX;
	
	FAI_TESTMENU_GetCurrentMenu(&tChMsg.hChMenu);

	eVkRet = FVK_MSG_Send(s_ulCabChMsgIdForUI, &tChMsg, sizeof(FAI_CH_C_MSG_t));
	if ( eVkRet != FVK_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_CHANNEL_C_PerformanceTuner0_UI] : Error FVK_MSG_Send() : %d\n", eVkRet));
		return FAI_ERR;
	}
	return FAI_NO_ERR;
}
#endif
/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
#if (CAB_CH_PERFORMANCE_TUNER_UI2_ENABLE==YES)
FAI_ERR_CODE FAI_CHANNEL_C_PerformanceTuner_UI2(void)
{
	FAI_CH_C_MSG_t tChMsg;
	FVK_ERR_CODE eVkRet;
	static unsigned long ulTunerId = 1;
#if (CHANNEL_CAB_MULTI_TUNER_ENABLE==YES)
	tChMsg.ulTunerId = 2;
	ulTunerId = 2;
	
#else

	if(ulTunerId == 0)
	{
		tChMsg.ulTunerId = 1;
		ulTunerId = 1;
	}
	else
	{
		tChMsg.ulTunerId = 0;
		ulTunerId = 0;
	}
#endif
	tChMsg.ulTunerId = 2;
	ulTunerId = 2;
	tChMsg.eChTestType = FAI_CHANNEL_C_TEST_TYPE_PERFORMANCE_CHECK_UI;
	tChMsg.tCabTuneParam.ulFrequency = CAB_CH_PERFORMANCE_TUNER_UI2_FREQUENCY;
	tChMsg.tCabTuneParam.ulSymbolRate = CAB_CH_PERFORMANCE_TUNER_UI2_SYMBOL_RATE;
	tChMsg.tCabTuneParam.etModulation = FDI_CH_CMOD_AUTO;
	tChMsg.tCabTuneParam.etSpectrum = FDI_CH_INVERSION_AUTO;
	tChMsg.ulAgcMin = CAB_CH_PERFORMANCE_TUNER_UI2_AGC_MIN;
	tChMsg.ulAgcMax = CAB_CH_PERFORMANCE_TUNER_UI2_AGC_MAX;
	tChMsg.ulBerMin = CAB_CH_PERFORMANCE_TUNER_UI2_BER_MIN;
	tChMsg.ulBerMax = CAB_CH_PERFORMANCE_TUNER_UI2_BER_MAX;
	tChMsg.ulSnrMin = CAB_CH_PERFORMANCE_TUNER_UI2_SNR_MIN;
	tChMsg.ulSnrMax = CAB_CH_PERFORMANCE_TUNER_UI2_SNR_MAX;
	
	FAI_TESTMENU_GetCurrentMenu(&tChMsg.hChMenu);

	eVkRet = FVK_MSG_Send(s_ulCabChMsgIdForUI, &tChMsg, sizeof(FAI_CH_C_MSG_t));
	if ( eVkRet != FVK_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_CHANNEL_C_PerformanceTuner0_UI] : Error FVK_MSG_Send() : %d\n", eVkRet));
		return FAI_ERR;
	}
	return FAI_NO_ERR;
}
#endif

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
#if (CAB_CH_PERFORMANCE_TUNER_UI3_ENABLE==YES)
FAI_ERR_CODE FAI_CHANNEL_C_PerformanceTuner_UI3(void)
{
	FAI_CH_C_MSG_t tChMsg;
	FVK_ERR_CODE eVkRet;
	static unsigned long ulTunerId = 1;
#if (CHANNEL_CAB_MULTI_TUNER_ENABLE==YES)
	tChMsg.ulTunerId = 3;
	ulTunerId = 3;
#else

	if(ulTunerId == 0)
	{
		tChMsg.ulTunerId = 1;
		ulTunerId = 1;
	}
	else
	{
		tChMsg.ulTunerId = 0;
		ulTunerId = 0;
	}
#endif
	tChMsg.ulTunerId = 3;
	ulTunerId = 3;
	tChMsg.eChTestType = FAI_CHANNEL_C_TEST_TYPE_PERFORMANCE_CHECK_UI;
	tChMsg.tCabTuneParam.ulFrequency = CAB_CH_PERFORMANCE_TUNER_UI3_FREQUENCY;
	tChMsg.tCabTuneParam.ulSymbolRate = CAB_CH_PERFORMANCE_TUNER_UI3_SYMBOL_RATE;
	tChMsg.tCabTuneParam.etModulation = FDI_CH_CMOD_AUTO;
	tChMsg.tCabTuneParam.etSpectrum = FDI_CH_INVERSION_AUTO;
	tChMsg.ulAgcMin = CAB_CH_PERFORMANCE_TUNER_UI3_AGC_MIN;
	tChMsg.ulAgcMax = CAB_CH_PERFORMANCE_TUNER_UI3_AGC_MAX;
	tChMsg.ulBerMin = CAB_CH_PERFORMANCE_TUNER_UI3_BER_MIN;
	tChMsg.ulBerMax = CAB_CH_PERFORMANCE_TUNER_UI3_BER_MAX;
	tChMsg.ulSnrMin = CAB_CH_PERFORMANCE_TUNER_UI3_SNR_MIN;
	tChMsg.ulSnrMax = CAB_CH_PERFORMANCE_TUNER_UI3_SNR_MAX;
	
	FAI_TESTMENU_GetCurrentMenu(&tChMsg.hChMenu);

	eVkRet = FVK_MSG_Send(s_ulCabChMsgIdForUI, &tChMsg, sizeof(FAI_CH_C_MSG_t));
	if ( eVkRet != FVK_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_CHANNEL_C_PerformanceTuner0_UI] : Error FVK_MSG_Send() : %d\n", eVkRet));
		return FAI_ERR;
	}
	return FAI_NO_ERR;
}
#endif

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
#if (CAB_CH_PERFORMANCE_TUNER_UI4_ENABLE==YES)
FAI_ERR_CODE FAI_CHANNEL_C_PerformanceTuner_UI4(void)
{
	FAI_CH_C_MSG_t tChMsg;
	FVK_ERR_CODE eVkRet;
	static unsigned long ulTunerId = 1;
#if (CHANNEL_CAB_MULTI_TUNER_ENABLE==YES)
	tChMsg.ulTunerId = 4;
	ulTunerId = 4;
	
#else

	if(ulTunerId == 0)
	{
		tChMsg.ulTunerId = 1;
		ulTunerId = 1;
	}
	else
	{
		tChMsg.ulTunerId = 0;
		ulTunerId = 0;
	}
#endif
	tChMsg.ulTunerId = 4;
	ulTunerId = 4;
	tChMsg.eChTestType = FAI_CHANNEL_C_TEST_TYPE_PERFORMANCE_CHECK_UI;
	tChMsg.tCabTuneParam.ulFrequency = CAB_CH_PERFORMANCE_TUNER_UI4_FREQUENCY;
	tChMsg.tCabTuneParam.ulSymbolRate = CAB_CH_PERFORMANCE_TUNER_UI4_SYMBOL_RATE;
	tChMsg.tCabTuneParam.etModulation = FDI_CH_CMOD_AUTO;
	tChMsg.tCabTuneParam.etSpectrum = FDI_CH_INVERSION_AUTO;
	tChMsg.ulAgcMin = CAB_CH_PERFORMANCE_TUNER_UI4_AGC_MIN;
	tChMsg.ulAgcMax = CAB_CH_PERFORMANCE_TUNER_UI4_AGC_MAX;
	tChMsg.ulBerMin = CAB_CH_PERFORMANCE_TUNER_UI4_BER_MIN;
	tChMsg.ulBerMax = CAB_CH_PERFORMANCE_TUNER_UI4_BER_MAX;
	tChMsg.ulSnrMin = CAB_CH_PERFORMANCE_TUNER_UI4_SNR_MIN;
	tChMsg.ulSnrMax = CAB_CH_PERFORMANCE_TUNER_UI4_SNR_MAX;
	
	FAI_TESTMENU_GetCurrentMenu(&tChMsg.hChMenu);

	eVkRet = FVK_MSG_Send(s_ulCabChMsgIdForUI, &tChMsg, sizeof(FAI_CH_C_MSG_t));
	if ( eVkRet != FVK_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_CHANNEL_C_PerformanceTuner0_UI] : Error FVK_MSG_Send() : %d\n", eVkRet));
		return FAI_ERR;
	}
	return FAI_NO_ERR;
}
#endif

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
#if (CAB_CH_PERFORMANCE_TUNER_UI5_ENABLE==YES)
FAI_ERR_CODE FAI_CHANNEL_C_PerformanceTuner_UI5(void)
{
	FAI_CH_C_MSG_t tChMsg;
	FVK_ERR_CODE eVkRet;
	static unsigned long ulTunerId = 1;
#if (CHANNEL_CAB_MULTI_TUNER_ENABLE==YES)
	tChMsg.ulTunerId = 5;
	ulTunerId = 5;
	
#else

	if(ulTunerId == 0)
	{
		tChMsg.ulTunerId = 1;
		ulTunerId = 1;
	}
	else
	{
		tChMsg.ulTunerId = 0;
		ulTunerId = 0;
	}
#endif
	tChMsg.ulTunerId = 5;
	ulTunerId = 5;
	tChMsg.eChTestType = FAI_CHANNEL_C_TEST_TYPE_PERFORMANCE_CHECK_UI;
	tChMsg.tCabTuneParam.ulFrequency = CAB_CH_PERFORMANCE_TUNER_UI5_FREQUENCY;
	tChMsg.tCabTuneParam.ulSymbolRate = CAB_CH_PERFORMANCE_TUNER_UI5_SYMBOL_RATE;
	tChMsg.tCabTuneParam.etModulation = FDI_CH_CMOD_AUTO;
	tChMsg.tCabTuneParam.etSpectrum = FDI_CH_INVERSION_AUTO;
	tChMsg.ulAgcMin = CAB_CH_PERFORMANCE_TUNER_UI5_AGC_MIN;
	tChMsg.ulAgcMax = CAB_CH_PERFORMANCE_TUNER_UI5_AGC_MAX;
	tChMsg.ulBerMin = CAB_CH_PERFORMANCE_TUNER_UI5_BER_MIN;
	tChMsg.ulBerMax = CAB_CH_PERFORMANCE_TUNER_UI5_BER_MAX;
	tChMsg.ulSnrMin = CAB_CH_PERFORMANCE_TUNER_UI5_SNR_MIN;
	tChMsg.ulSnrMax = CAB_CH_PERFORMANCE_TUNER_UI5_SNR_MAX;
	
	FAI_TESTMENU_GetCurrentMenu(&tChMsg.hChMenu);

	eVkRet = FVK_MSG_Send(s_ulCabChMsgIdForUI, &tChMsg, sizeof(FAI_CH_C_MSG_t));
	if ( eVkRet != FVK_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_CHANNEL_C_PerformanceTuner0_UI] : Error FVK_MSG_Send() : %d\n", eVkRet));
		return FAI_ERR;
	}
	return FAI_NO_ERR;
}
#endif

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
#if (CAB_CH_PERFORMANCE_TUNER_UI6_ENABLE==YES)
FAI_ERR_CODE FAI_CHANNEL_C_PerformanceTuner_UI6(void)
{
	FAI_CH_C_MSG_t tChMsg;
	FVK_ERR_CODE eVkRet;
	static unsigned long ulTunerId = 1;
#if (CHANNEL_CAB_MULTI_TUNER_ENABLE==YES)
	tChMsg.ulTunerId = 6;
	ulTunerId = 6;
	
#else

	if(ulTunerId == 0)
	{
		tChMsg.ulTunerId = 1;
		ulTunerId = 1;
	}
	else
	{
		tChMsg.ulTunerId = 0;
		ulTunerId = 0;
	}
#endif
	tChMsg.ulTunerId = 6;
	ulTunerId = 6;
	tChMsg.eChTestType = FAI_CHANNEL_C_TEST_TYPE_PERFORMANCE_CHECK_UI;
	tChMsg.tCabTuneParam.ulFrequency = CAB_CH_PERFORMANCE_TUNER_UI6_FREQUENCY;
	tChMsg.tCabTuneParam.ulSymbolRate = CAB_CH_PERFORMANCE_TUNER_UI6_SYMBOL_RATE;
	tChMsg.tCabTuneParam.etModulation = FDI_CH_CMOD_AUTO;
	tChMsg.tCabTuneParam.etSpectrum = FDI_CH_INVERSION_AUTO;
	tChMsg.ulAgcMin = CAB_CH_PERFORMANCE_TUNER_UI6_AGC_MIN;
	tChMsg.ulAgcMax = CAB_CH_PERFORMANCE_TUNER_UI6_AGC_MAX;
	tChMsg.ulBerMin = CAB_CH_PERFORMANCE_TUNER_UI6_BER_MIN;
	tChMsg.ulBerMax = CAB_CH_PERFORMANCE_TUNER_UI6_BER_MAX;
	tChMsg.ulSnrMin = CAB_CH_PERFORMANCE_TUNER_UI6_SNR_MIN;
	tChMsg.ulSnrMax = CAB_CH_PERFORMANCE_TUNER_UI6_SNR_MAX;
	
	FAI_TESTMENU_GetCurrentMenu(&tChMsg.hChMenu);

	eVkRet = FVK_MSG_Send(s_ulCabChMsgIdForUI, &tChMsg, sizeof(FAI_CH_C_MSG_t));
	if ( eVkRet != FVK_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_CHANNEL_C_PerformanceTuner0_UI] : Error FVK_MSG_Send() : %d\n", eVkRet));
		return FAI_ERR;
	}
	return FAI_NO_ERR;
}
#endif

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
#if (CAB_CH_PERFORMANCE_TUNER_UI7_ENABLE==YES)
FAI_ERR_CODE FAI_CHANNEL_C_PerformanceTuner_UI7(void)
{
	FAI_CH_C_MSG_t tChMsg;
	FVK_ERR_CODE eVkRet;
	static unsigned long ulTunerId = 1;
#if (CHANNEL_CAB_MULTI_TUNER_ENABLE==YES)
	tChMsg.ulTunerId = 7;
	ulTunerId = 7;
	
#else

	if(ulTunerId == 0)
	{
		tChMsg.ulTunerId = 1;
		ulTunerId = 1;
	}
	else
	{
		tChMsg.ulTunerId = 0;
		ulTunerId = 0;
	}
#endif
	tChMsg.ulTunerId = 7;
	ulTunerId = 7;
	tChMsg.eChTestType = FAI_CHANNEL_C_TEST_TYPE_PERFORMANCE_CHECK_UI;
	tChMsg.tCabTuneParam.ulFrequency = CAB_CH_PERFORMANCE_TUNER_UI7_FREQUENCY;
	tChMsg.tCabTuneParam.ulSymbolRate = CAB_CH_PERFORMANCE_TUNER_UI7_SYMBOL_RATE;
	tChMsg.tCabTuneParam.etModulation = FDI_CH_CMOD_AUTO;
	tChMsg.tCabTuneParam.etSpectrum = FDI_CH_INVERSION_AUTO;
	tChMsg.ulAgcMin = CAB_CH_PERFORMANCE_TUNER_UI7_AGC_MIN;
	tChMsg.ulAgcMax = CAB_CH_PERFORMANCE_TUNER_UI7_AGC_MAX;
	tChMsg.ulBerMin = CAB_CH_PERFORMANCE_TUNER_UI7_BER_MIN;
	tChMsg.ulBerMax = CAB_CH_PERFORMANCE_TUNER_UI7_BER_MAX;
	tChMsg.ulSnrMin = CAB_CH_PERFORMANCE_TUNER_UI7_SNR_MIN;
	tChMsg.ulSnrMax = CAB_CH_PERFORMANCE_TUNER_UI7_SNR_MAX;
	
	FAI_TESTMENU_GetCurrentMenu(&tChMsg.hChMenu);

	eVkRet = FVK_MSG_Send(s_ulCabChMsgIdForUI, &tChMsg, sizeof(FAI_CH_C_MSG_t));
	if ( eVkRet != FVK_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_CHANNEL_C_PerformanceTuner0_UI] : Error FVK_MSG_Send() : %d\n", eVkRet));
		return FAI_ERR;
	}
	return FAI_NO_ERR;
}
#endif

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
#if (CAB_CH_PERFORMANCE_TUNER0_UI_ENABLE==YES)
FAI_ERR_CODE FAI_CHANNEL_C_PerformanceTuner0_UI(void)
{
	FAI_CH_C_MSG_t tChMsg;
	FVK_ERR_CODE eVkRet;

	tChMsg.ulTunerId = 0;
	tChMsg.eChTestType = FAI_CHANNEL_C_TEST_TYPE_PERFORMANCE_CHECK_UI;
	tChMsg.tCabTuneParam.ulFrequency = CAB_CH_PERFORMANCE_TUNER0_UI_FREQUENCY;
	tChMsg.tCabTuneParam.ulSymbolRate = CAB_CH_PERFORMANCE_TUNER0_UI_SYMBOL_RATE;
	tChMsg.tCabTuneParam.etModulation = FDI_CH_CMOD_AUTO;
	tChMsg.tCabTuneParam.etSpectrum = FDI_CH_INVERSION_AUTO;
	tChMsg.ulAgcMin = CAB_CH_PERFORMANCE_TUNER0_UI_AGC_MIN;
	tChMsg.ulAgcMax = CAB_CH_PERFORMANCE_TUNER0_UI_AGC_MAX;
	tChMsg.ulBerMin = CAB_CH_PERFORMANCE_TUNER0_UI_BER_MIN;
	tChMsg.ulBerMax = CAB_CH_PERFORMANCE_TUNER0_UI_BER_MAX;
	FAI_TESTMENU_GetCurrentMenu(&tChMsg.hChMenu);

	eVkRet = FVK_MSG_Send(s_ulCabChMsgIdForUI, &tChMsg, sizeof(FAI_CH_C_MSG_t));
	if ( eVkRet != FVK_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_CHANNEL_C_PerformanceTuner0_UI] : Error FVK_MSG_Send() : %d\n", eVkRet));
		return FAI_ERR;
	}
	return FAI_NO_ERR;
}
#endif

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
#if (CAB_CH_PERFORMANCE_TUNER1_UI_ENABLE==YES)
FAI_ERR_CODE FAI_CHANNEL_C_PerformanceTuner1_UI(void)
{
	FAI_CH_C_MSG_t tChMsg;
	FVK_ERR_CODE eVkRet;

	tChMsg.ulTunerId = 1;
	tChMsg.eChTestType = FAI_CHANNEL_C_TEST_TYPE_PERFORMANCE_CHECK_UI;
	tChMsg.tCabTuneParam.ulFrequency = CAB_CH_PERFORMANCE_TUNER1_UI_FREQUENCY;
	tChMsg.tCabTuneParam.ulSymbolRate = CAB_CH_PERFORMANCE_TUNER1_UI_SYMBOL_RATE;
	tChMsg.tCabTuneParam.etModulation = FDI_CH_CMOD_AUTO;
	tChMsg.tCabTuneParam.etSpectrum = FDI_CH_INVERSION_AUTO;
	tChMsg.ulAgcMin = CAB_CH_PERFORMANCE_TUNER1_UI_AGC_MIN;
	tChMsg.ulAgcMax = CAB_CH_PERFORMANCE_TUNER1_UI_AGC_MAX;
	tChMsg.ulBerMin = CAB_CH_PERFORMANCE_TUNER1_UI_BER_MIN;
	tChMsg.ulBerMax = CAB_CH_PERFORMANCE_TUNER1_UI_BER_MAX;
	FAI_TESTMENU_GetCurrentMenu(&tChMsg.hChMenu);

	eVkRet = FVK_MSG_Send(s_ulCabChMsgIdForUI, &tChMsg, sizeof(FAI_CH_C_MSG_t));
	if ( eVkRet != FVK_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_CHANNEL_C_PerformanceTuner1_UI] : Error FVK_MSG_Send() : %d\n", eVkRet));
		return FAI_ERR;
	}
	return FAI_NO_ERR;
}
#endif

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
#if (CHANNEL_CAB_LOCK_FOR_PERFORMANCE_ENABLE==YES)
FAI_ERR_CODE FAI_CHANNEL_C_PerformanceCheck(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList)
{
	FAI_CH_MSG_t tChMsg;
	FVK_ERR_CODE eVkRet;
	unsigned char ucTunerId, aucFreqeuncy[4], aucSymbolate[4], ucModulation;
	unsigned char ucFreqSize, ucSymbolSize;
	FAI_TMD_RESULT_T tTMDResult;
	unsigned char ucTmdErrCode[2];
	FVK_MEM_Set(&tTMDResult, 0x00, sizeof(tTMDResult));
	
	if( ucArgc != 4 )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_CHANNEL_C_PerformanceCheck] : Argument Count Error=%d\n", ucArgc));
		tTMDResult.eResult = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
		ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_INVALID_ARGUMENT;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));	
		goto TEST_FAIL;

	}

	/* get parameters */
	FAI_TMD_GetParam(&ucTunerId, pucArgList, 0);
	ucFreqSize = FAI_TMD_GetParam(aucFreqeuncy, pucArgList, 1);
	ucSymbolSize = FAI_TMD_GetParam(aucSymbolate, pucArgList, 2);
	FAI_TMD_GetParam(&ucModulation, pucArgList, 3);

	if(ucTunerId >= g_ulNumofCHDevice)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[%s] : Invalid parameter Error, TunerID =%d\n", __func__, ucTunerId));
		tTMDResult.eResult = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
		ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_INVALID_PARAMETER;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));	
		goto TEST_FAIL;
	}

	/* change parameters */
	tChMsg.usMsgId = usMsgId;
	tChMsg.ulTunerId = (unsigned long)ucTunerId;
	tChMsg.eChTestType = FAI_CHANNEL_TEST_TYPE_CAB_CHECK_PERFORMANCE;
	FAI_TMD_ArgToValue(&tChMsg.tCabTuneParam.ulFrequency, aucFreqeuncy, ucFreqSize);
	FAI_TMD_ArgToValue(&tChMsg.tCabTuneParam.ulSymbolRate, aucSymbolate, ucSymbolSize);
	tChMsg.tCabTuneParam.etModulation = ucModulation;
	tChMsg.tCabTuneParam.etSpectrum = FDI_CH_INVERSION_AUTO;

	FAI_PRINT(FAI_PRT_DBGINFO, ("===============================================\n"));
	FAI_PRINT(FAI_PRT_DBGINFO, ("[%s]\n", __func__));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tucTunerId=%d\n",tChMsg.ulTunerId));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\teChTestType=%d\n",tChMsg.eChTestType));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tfreq=%d\n", tChMsg.tCabTuneParam.ulFrequency));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tsymbol=%d\n", tChMsg.tCabTuneParam.ulSymbolRate));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tmod=%d\n", tChMsg.tCabTuneParam.etModulation));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tSpectrum=%d\n", tChMsg.tCabTuneParam.etSpectrum));
	FAI_PRINT(FAI_PRT_DBGINFO, ("===============================================\n"));
	
	eVkRet = FVK_MSG_Send(g_pstCHTask[ucTunerId].ulMSGQID, &tChMsg, sizeof(FAI_CH_MSG_t));
	if ( eVkRet != FVK_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_CHANNEL_C_PerformanceCheck] : Error FVK_MSG_Send() : %d\n", eVkRet));
		tTMDResult.eResult = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
		ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_MESSAGE_SEND_FAIL;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));	
		goto TEST_FAIL;
	}
	return FAI_NO_ERR;
	
TEST_FAIL:
	
	tTMDResult.ucCategory = FAI_CATEGORY_CHANNEL;
	tTMDResult.ucModule = 0x03;
	tTMDResult.ucAction = 0x02;
	tTMDResult.usMsgId = usMsgId;
	FAI_TMD_SetResult(&tTMDResult);
	return FAI_ERR;
}
#endif

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
static void INTRN_FAI_CHANNEL_C_Task_UI(void *data)
{
	FAI_CH_C_MSG_t tChMsg;
	FVK_ERR_CODE eVkRet;
	BOOL bForceLock;
	
	while(1)
	{
		eVkRet = FVK_MSG_Receive(s_ulCabChMsgIdForUI, &tChMsg, sizeof(FAI_CH_C_MSG_t));
		if ( eVkRet != FVK_NO_ERR )
		{
			FAI_PRINT(FAI_PRT_WARNING, ("[INTRN_FAI_CHANNEL_C_Task_UI] : Error FVK_MSG_Receive() %d\n", eVkRet));
			continue;
		}

		/* 강제로 Lock을 시도할지 결정 */
		(tChMsg.eChTestType != s_ePrevTest) ? (bForceLock=TRUE) : (bForceLock=FALSE);
		s_ePrevTest = tChMsg.eChTestType;
		if( tChMsg.eChTestType == FAI_CHANNEL_C_TEST_TYPE_PERFORMANCE_CHECK_UI )
		{
			INTRN_FAI_CHANNEL_C_PerformanceCheck_UI(tChMsg.ulTunerId, 
												tChMsg.tCabTuneParam, 
												tChMsg.ulBerMin, 
												tChMsg.ulBerMax, 
												tChMsg.ulAgcMin, 
												tChMsg.ulAgcMax, 
												tChMsg.ulSnrMin,
												tChMsg.ulSnrMax,
												tChMsg.hChMenu);
		}
 	}
}

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
static FAI_ERR_CODE INTRN_FAI_CHANNEL_C_CheckPerformance(unsigned short usMsgId, unsigned long ulTunerId, 
												FDI_CH_CAB_TUNE_PARAM_t tCabTuneParam) 
{
	FDI_CH_CAB_TUNE_PARAM_t tCabParam;
	FDI_CH_CAB_SIGNAL_QUALITY_t tSignalInfo;
	signed long alAgcValue[FAI_CHANNEL_BER_TEST_SAMPLING_NUM];
	signed long alBerValue[FAI_CHANNEL_BER_TEST_SAMPLING_NUM];
	signed long alSnrValue[FAI_CHANNEL_BER_TEST_SAMPLING_NUM];
	signed long lAgcAverage, lAgcMin, lAgcMax, lSumOfAgc;
	signed long lBerAverage, lBerMin, lBerMax, lSumOfBer;
	signed long lSnrAverage, lSnrMin, lSnrMax, lSumOfSnr; 
	unsigned char aucAgcResult[FAI_CHANNEL_AGC_TEST_RESULT_LEN];
	unsigned char aucBerResult[FAI_CHANNEL_BER_TEST_RESULT_LEN];
	unsigned char aucSnrResult[FAI_CHANNEL_BER_TEST_RESULT_LEN];
	FAI_TMD_RESULT_T tTMDResult;
	unsigned char ucTmdErrCode[2];
	FDI_ERR_CODE eFdiErr = FDI_NO_ERR;
	BOOL bIsLocked = FALSE;
	int i;
	unsigned int dec = 0,exp = 0;

	FAI_PRINT(FAI_PRT_DBGINFO, ("===============================================\n"));
	FAI_PRINT(FAI_PRT_DBGINFO, ("[%s]\n", __func__));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tucTunerId=%d\n", ulTunerId));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tfreq=%d\n", tCabTuneParam.ulFrequency));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tsymbol=%d\n", tCabTuneParam.ulSymbolRate));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tmod=%d\n", tCabTuneParam.etModulation));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tSpectrum=%d\n", tCabTuneParam.etSpectrum));
	FAI_PRINT(FAI_PRT_DBGINFO, ("===============================================\n"));

	FVK_MEM_Set(&tTMDResult, 0x00, sizeof(tTMDResult));
	FVK_MEM_Set(aucAgcResult, 0x00, sizeof(aucAgcResult));
	FVK_MEM_Set(aucBerResult, 0x00, sizeof(aucBerResult));
	FVK_MEM_Set(aucSnrResult, 0x00, sizeof(aucSnrResult));
	FVK_MEM_Set(&tCabParam, 0x00, sizeof(tCabParam));
	FVK_MEM_Set(&tSignalInfo, 0x00, sizeof(tSignalInfo));
	
	/* 이미 원하는 주파수로 락이 되어있으면 정보만 업데이트한다(requirement). */
	FDI_CHANNEL_C_IsLocked(ulTunerId, &bIsLocked);
	FDI_CHANNEL_C_GetLockedInfo(ulTunerId, &tCabParam);
	if( (bIsLocked == FALSE) || (tCabTuneParam.ulFrequency != tCabParam.ulFrequency) )
	{
		eFdiErr = FDI_CHANNEL_C_RequestLock(ulTunerId, &tCabTuneParam);
		if( eFdiErr != FDI_NO_ERR )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[%s] : Request lock Fail~!\n",__func__));
			ucTmdErrCode[0] = FAI_CATEGORY_CHANNEL;
			ucTmdErrCode[1] = FAI_CHANNEL_TMD_ERR_REQUEST_LOCK_FAIL;
			goto TEST_FAIL;
		}

		/* Locking check */
		for( i = 0; i < FAI_CHANNEL_LOCK_CHECK_RETRY_NUM; i++ )
		{
			FDI_CHANNEL_C_IsLocked(ulTunerId, &bIsLocked);
			if(bIsLocked == TRUE)
			{
				break;
			}
			FVK_TASK_Sleep(FAI_CHANNEL_LOCK_RETRY_DELAY);
		}

		if( bIsLocked == FALSE )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[%s] : Locking Fail~!\n",__func__));
			ucTmdErrCode[0] = FAI_CATEGORY_CHANNEL;
			ucTmdErrCode[1] = FAI_CHANNEL_TMD_ERR_UNLOCK;
			goto TEST_FAIL;
		}

		FVK_TASK_Sleep(FAI_CHANNEL_C_LOCKING_WAIT); 	
	}

	for( i = 0; i < FAI_CHANNEL_BER_TEST_SAMPLING_NUM; i++ )
	{
		FVK_TASK_Sleep(FAI_CHANNEL_BER_TEST_SAMPLING_DELAY);
		eFdiErr = FDI_CHANNEL_C_GetSignalInfo(ulTunerId, &tSignalInfo);
		if( eFdiErr != FDI_NO_ERR )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[%s] : Get signal info Fail~!\n",__func__));
			ucTmdErrCode[0] = FAI_CATEGORY_CHANNEL;
			ucTmdErrCode[1] = FAI_CHANNEL_TMD_ERR_GET_SIGNAL_INFO_FAIL;
			goto TEST_FAIL;
		}
#if (CHANNEL_CAB_AGC_PERFORMANCE_ENABLE == YES)
		alAgcValue[i] = (signed long)(tSignalInfo.unAgc);
#endif
#if (CHANNEL_CAB_BER_PERFORMANCE_ENABLE == YES)
		alBerValue[i] = (signed long)(tSignalInfo.fBer);
#endif
#if (CHANNEL_CAB_SNR_PERFORMANCE_ENABLE == YES)
		alSnrValue[i] = (signed long)(tSignalInfo.fSnr);
#endif
	}

#if (CHANNEL_CAB_AGC_PERFORMANCE_ENABLE == YES)
	lAgcMin = lAgcMax = alAgcValue[0];
	for( i = 1; i < FAI_CHANNEL_BER_TEST_SAMPLING_NUM; i++ )
	{
		if( lAgcMin > alAgcValue[i] )	/* min agc */
		{
			lAgcMin = alAgcValue[i];
		}
		if( lAgcMax < alAgcValue[i] )	/* max agc */
		{
			lAgcMax = alAgcValue[i];
		}
	}

	lSumOfAgc = 0;

	for( i = 0; i < FAI_CHANNEL_BER_TEST_SAMPLING_NUM; i++ )
	{
		lSumOfAgc += alAgcValue[i];
	}

	lAgcAverage = (long)(lSumOfAgc-lAgcMin-lAgcMax)/(FAI_CHANNEL_BER_TEST_SAMPLING_NUM-2);

	FAI_PRINT(FAI_PRT_DBGINFO, ("lAgcAverage : %d, lSumOfAgc:%d, lAgcMin=%d, lAgcMax=%d\n", lAgcAverage, lSumOfAgc, lAgcMin, lAgcMax));

	/* Save result parameters */
	if( lAgcAverage < 0 )
	{
		aucAgcResult[5] = 0x01; /* 부호 (-) */
		lAgcAverage = lAgcAverage*(-1);
	}
	else
	{
		aucAgcResult[5] = 0x00; /* 부호 (+) */
	}

	aucAgcResult[0] = (lAgcAverage&0x7F000000)>>24;
	aucAgcResult[1] = (lAgcAverage&0x00FF0000)>>16;
	aucAgcResult[2] = (lAgcAverage&0x0000FF00)>>8;
	aucAgcResult[3] = (lAgcAverage&0x000000FF);
	aucAgcResult[4] = (CHANNEL_CAB_AGC_POWER_CONSTANT); /* 소수점 지수 */
#endif

#if (CHANNEL_CAB_BER_PERFORMANCE_ENABLE == YES)
	lBerMin = lBerMax = alBerValue[0];
	for( i = 1; i < FAI_CHANNEL_BER_TEST_SAMPLING_NUM; i++ )
	{
		if( lBerMin > alBerValue[i] ) /* min ber */
		{
			lBerMin = alBerValue[i];
		}
		if( lBerMax < alBerValue[i] ) /* max ber */
		{
			lBerMax = alBerValue[i];
		}
	}

	lSumOfBer = 0;
	for( i = 0; i < FAI_CHANNEL_BER_TEST_SAMPLING_NUM; i++ )
	{
		lSumOfBer += alBerValue[i];
	}

	lBerAverage = (long)(lSumOfBer-lBerMin-lBerMax)/(FAI_CHANNEL_BER_TEST_SAMPLING_NUM-2);

	FAI_PRINT(FAI_PRT_DBGINFO, ("lBerAverage : %d, lSumOfBer:%d, lBerMin=%d, lBerMax=%d\n", lBerAverage, lSumOfBer, lBerMin, lBerMax));

	/* save ber result */	
	if( lBerAverage < 0 )
	{
		aucBerResult[5] = 0x01; /* 부호 (-) */
		lBerAverage = lBerAverage*(-1);
	}
	else
	{
		aucBerResult[5] = 0x00; /* 부호 (+) */
	}
#if 0
	if(lBerAverage == 0)
	{
		dec = 0;
		exp = 0;
	}
	else
	{
		INTRN_FAI_UTIL_GetBerNum(&dec,&exp,lBerAverage);
	}
	aucBerResult[0] = 0;
	aucBerResult[1] = 0;
	aucBerResult[2] = (dec&0xff00)>>8;
	aucBerResult[3] = dec&0x00ff;
	aucBerResult[4] = (exp&0xf);					/* 소수점 지수 */

#else
	
	if(lBerAverage == 0)
	{
		aucBerResult[0] = (lBerAverage&0x7F000000)>>24;
		aucBerResult[1] = (lBerAverage&0x00FF0000)>>16;
		aucBerResult[2] = (lBerAverage&0x0000FF00)>>8;
		aucBerResult[3] = (lBerAverage&0x000000FF);
		aucBerResult[4] = (0); /* 소수점 지수 */
	}
	else
	{
	aucBerResult[0] = (lBerAverage&0x7F000000)>>24;
	aucBerResult[1] = (lBerAverage&0x00FF0000)>>16;
	aucBerResult[2] = (lBerAverage&0x0000FF00)>>8;
	aucBerResult[3] = (lBerAverage&0x000000FF);
	aucBerResult[4] = (CHANNEL_CAB_BER_POWER_CONSTANT); /* 소수점 지수 */
	}
#endif	
#endif

#if (CHANNEL_CAB_SNR_PERFORMANCE_ENABLE == YES)
	lSnrMin = lSnrMax = alSnrValue[0];
	for( i = 1; i < FAI_CHANNEL_BER_TEST_SAMPLING_NUM; i++ )
	{
		if( lSnrMin > alSnrValue[i] ) /* min snr */
		{
			lSnrMin = alSnrValue[i];
		}		
		if( lSnrMax < alSnrValue[i] ) /* max snr */
		{
			lSnrMax = alSnrValue[i];
		}		
	}

	lSumOfSnr = 0;
	
	for( i = 0; i < FAI_CHANNEL_BER_TEST_SAMPLING_NUM; i++ )
	{
		lSumOfSnr += alSnrValue[i]; 	
	}

	lSnrAverage = (long)(lSumOfSnr-lSnrMin-lSnrMax)/(FAI_CHANNEL_BER_TEST_SAMPLING_NUM-2);

	FAI_PRINT(FAI_PRT_DBGINFO, ("lSnrAverage : %d, lSumOfSnr:%d, lSnrMin=%d, lSnrMax=%d\n", lSnrAverage, lSumOfSnr, lSnrMin, lSnrMax));

	if( lSnrAverage < 0 )
	{
		aucSnrResult[5] = 0x01; /* 부호 (-) */
		lSnrAverage = lSnrAverage*(-1);
	}
	else
	{
		aucSnrResult[5] = 0x00; /* 부호 (+) */
	}
/* JMKU channel driver(YSR-2000/YS-1000) 변경에 따른 수정 기존에는 1000단위로 전달됨,그래서 SNR 나누기 100 함  , CM 모델 전체 적용 불필요의견  by FE: sykim*/
/*	lSnrAverage = lSnrAverage/100; */

	aucSnrResult[0] = (lSnrAverage&0x7F000000)>>24;
	aucSnrResult[1] = (lSnrAverage&0x00FF0000)>>16;
	aucSnrResult[2] = (lSnrAverage&0x0000FF00)>>8;
	aucSnrResult[3] = (lSnrAverage&0x000000FF);
	aucSnrResult[4] = (CHANNEL_CAB_SNR_POWER_CONSTANT); /* 소수점 지수 */
#endif

	FAI_PRINT(FAI_PRT_DBGINFO, ("===============================================\n"));
	FAI_PRINT(FAI_PRT_DBGINFO, ("AGC : 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x\n", aucAgcResult[0], aucAgcResult[1], aucAgcResult[2], aucAgcResult[3], aucAgcResult[4], aucAgcResult[5]));
	FAI_PRINT(FAI_PRT_DBGINFO, ("BER : 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x\n", aucBerResult[0], aucBerResult[1], aucBerResult[2], aucBerResult[3], aucBerResult[4], aucBerResult[5]));
	FAI_PRINT(FAI_PRT_DBGINFO, ("SNR : 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x\n", aucSnrResult[0], aucSnrResult[1], aucSnrResult[2], aucSnrResult[3], aucSnrResult[4], aucSnrResult[5]));
	FAI_PRINT(FAI_PRT_DBGINFO, ("===============================================\n"));
	
	tTMDResult.ucCategory = 0x01;
	tTMDResult.ucModule = 0x03;
	tTMDResult.ucAction = 0x02;
	tTMDResult.eResult = FAI_TMD_ERR_OK;
	tTMDResult.usMsgId = usMsgId;
	FAI_TMD_AddResultParam(&tTMDResult, aucAgcResult, FAI_CHANNEL_AGC_TEST_RESULT_LEN);
	FAI_TMD_AddResultParam(&tTMDResult, aucBerResult, FAI_CHANNEL_BER_TEST_RESULT_LEN);
	FAI_TMD_AddResultParam(&tTMDResult, aucSnrResult, FAI_CHANNEL_SNR_TEST_RESULT_LEN);
	FAI_TMD_SetResult(&tTMDResult);
	return FAI_NO_ERR;
	
TEST_FAIL:
	/* Save failed result parameters */
	tTMDResult.ucCategory = 0x01;
	tTMDResult.ucModule = 0x03;
	tTMDResult.ucAction = 0x02;
	tTMDResult.eResult = FAI_TMD_ERR_FAIL;
	tTMDResult.usMsgId = usMsgId;
	FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));
	FAI_TMD_SetResult(&tTMDResult);
	return FAI_ERR;
}

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
static FAI_ERR_CODE INTRN_FAI_CHANNEL_C_PerformanceCheck_UI(unsigned long ulTunerId, 
												FDI_CH_CAB_TUNE_PARAM_t tCabTuneParam, 
												unsigned long ulBerRangeMin, unsigned long ulBerRangeMax, 
												unsigned long ulAgcRangeMin, unsigned long ulAgcRangeMax, 
												unsigned long ulSnrRangeMin, unsigned long ulSnrRangeMax, 
												FAI_TESTMENU_H hMenu)
{
	FDI_CH_CAB_SIGNAL_QUALITY_t tCabSignalInfo;
	BOOL bIsLocked;
	int i;
	signed long alAgcValue[FAI_CHANNEL_BER_TEST_SAMPLING_NUM], alBerValue[FAI_CHANNEL_BER_TEST_SAMPLING_NUM],alSnrValue[FAI_CHANNEL_BER_TEST_SAMPLING_NUM];
	signed long lAgcMin, lAgcMax, lBerMin, lBerMax,lSnrMin, lSnrMax, lSumOfAgc, lSumOfBer, lSumOfSnr;
	signed long lAgcAverage, lBerAverage, lSnrAverage;
	char szCheckStr[FAI_CHANNEL_AVG_VALUE_STR_LEN], szAgcStr[FAI_CHANNEL_AVG_VALUE_STR_LEN], szBerStr[FAI_CHANNEL_AVG_VALUE_STR_LEN], szSnrStr[FAI_CHANNEL_AVG_VALUE_STR_LEN];

	FAI_TESTMENU_ChangeRCUColor(hMenu, RGB_COLOR_BLACK);
	FAI_TESTMENU_ChangeMenuColor(hMenu, RGB_COLOR_YELLOW);
	FAI_TESTMENU_ChangeMenu(hMenu, TESTMENU_LINE_2, "BER Check", RGB_COLOR_YELLOW);

	FDI_CHANNEL_C_RequestLock(ulTunerId, &tCabTuneParam);

	sprintf(szCheckStr, "%dMhz",tCabTuneParam.ulFrequency/1000);

	FAI_TESTMENU_ChangeMenu(hMenu, TESTMENU_LINE_2, szCheckStr, RGB_COLOR_YELLOW);	
	for( i = 0; i < FAI_CHANNEL_LOCK_CHECK_RETRY_NUM; i++ )
	{
		sprintf(szCheckStr, "Testing(%d)", i);
		FAI_TESTMENU_ChangeMenu(hMenu, TESTMENU_LINE_3, szCheckStr, RGB_COLOR_YELLOW);
		FDI_CHANNEL_C_IsLocked(ulTunerId, &bIsLocked);
		if(bIsLocked == TRUE)
		{
			break;
		}
		FVK_TASK_Sleep(FAI_CHANNEL_LOCK_RETRY_DELAY);
	}

	if( bIsLocked == FALSE )
	{
		goto TEST_FAIL;
	}

	for( i = 0; i < FAI_CHANNEL_BER_TEST_SAMPLING_NUM; i++ )
	{
		FVK_TASK_Sleep(FAI_CHANNEL_BER_TEST_SAMPLING_DELAY);
		FDI_CHANNEL_C_GetSignalInfo(ulTunerId, &tCabSignalInfo);
		alAgcValue[i] = (signed long)(tCabSignalInfo.unAgc);
		alBerValue[i] = (signed long)(tCabSignalInfo.fBer);
		alSnrValue[i] = (signed long)(tCabSignalInfo.fSnr);
	}

	lAgcMin = lAgcMax = alAgcValue[0];
	lBerMin = lBerMax = alBerValue[0];
	lSnrMin = lSnrMax = alSnrValue[0];
	for( i = 1; i < FAI_CHANNEL_BER_TEST_SAMPLING_NUM; i++ )
	{
		if( lAgcMin > alAgcValue[i] )	/* min agc */
		{
			lAgcMin = alAgcValue[i];
		}
		if( lBerMin > alBerValue[i] )	/* min ber */
		{
			lBerMin = alBerValue[i];
		}
		if( lSnrMin > alSnrValue[i] )	/* min snr */
		{
			lSnrMin = alSnrValue[i];
		}		
		if( lAgcMax < alAgcValue[i] )	/* max agc */
		{
			lAgcMax = alAgcValue[i];
		}
		if( lBerMax < alBerValue[i] )	/* max ber */
		{
			lBerMax = alBerValue[i];
		}
		if( lSnrMax < alSnrValue[i] )	/* max snr */
		{
			lSnrMax = alSnrValue[i];
		}
	}

	lSumOfAgc = lSumOfBer = lSumOfSnr = 0;
	for( i = 0; i < FAI_CHANNEL_BER_TEST_SAMPLING_NUM; i++ )
	{
		lSumOfAgc += alAgcValue[i];
		lSumOfBer += alBerValue[i];
		lSumOfSnr += alSnrValue[i];		
	}

	lAgcAverage = (long)(lSumOfAgc-lAgcMin-lAgcMax)/(FAI_CHANNEL_BER_TEST_SAMPLING_NUM-2);
	lBerAverage = (long)(lSumOfBer-lBerMin-lBerMax)/(FAI_CHANNEL_BER_TEST_SAMPLING_NUM-2);
	lSnrAverage = (long)(lSumOfSnr-lSnrMin-lSnrMax)/(FAI_CHANNEL_BER_TEST_SAMPLING_NUM-2);
	
	INTRN_FAI_UTIL_GetBerString(szBerStr,lBerAverage);
	sprintf(szAgcStr, "(%d)", lAgcAverage);
	sprintf(szSnrStr, "(%d)", lSnrAverage);	

	if( (ulAgcRangeMin == 0) && (ulAgcRangeMax == 0))
	{
		if((lBerAverage >= ulBerRangeMin) && (lBerAverage <= ulBerRangeMax))
		{
			FAI_TESTMENU_ChangeMenu(hMenu, TESTMENU_LINE_1, "SNR Pass", RGB_COLOR_BLACK);
			FAI_TESTMENU_ChangeMenu(hMenu, TESTMENU_LINE_2, szSnrStr, RGB_COLOR_BLACK);
		}
		else
		{
			FAI_TESTMENU_ChangeMenu(hMenu, TESTMENU_LINE_1, "SNR Fail", RGB_COLOR_RED);
			FAI_TESTMENU_ChangeMenu(hMenu, TESTMENU_LINE_2, szSnrStr, RGB_COLOR_RED);
		}
		
		if((lBerAverage >= ulBerRangeMin) && (lBerAverage <= ulBerRangeMax))
		{
			FAI_TESTMENU_ChangeMenu(hMenu, TESTMENU_LINE_3, "BER Pass", RGB_COLOR_BLACK);
			FAI_TESTMENU_ChangeMenu(hMenu, TESTMENU_LINE_4, szBerStr, RGB_COLOR_BLACK);
		}
		else
		{
			FAI_TESTMENU_ChangeMenu(hMenu, TESTMENU_LINE_3, "BER Fail", RGB_COLOR_RED);
			FAI_TESTMENU_ChangeMenu(hMenu, TESTMENU_LINE_4, szBerStr, RGB_COLOR_RED);
		}

		
		if((lSnrAverage >= ulSnrRangeMin) &&
			(lSnrAverage <= ulSnrRangeMax) &&
			(lBerAverage >= ulBerRangeMin) &&
			(lBerAverage <= ulBerRangeMax))
		{
			FDI_PANEL_Display(FAI_CHANNEL_PERFORMANCE_PASS_STR);
		}
		else
		{
			FDI_PANEL_Display(FAI_CHANNEL_PERFORMANCE_FAIL_STR);
		}

	}
	else
	{
		if((lAgcAverage >= ulAgcRangeMin) && (lAgcAverage <= ulAgcRangeMax))
		{
			FAI_TESTMENU_ChangeMenu(hMenu, TESTMENU_LINE_1, "AGC Pass", RGB_COLOR_BLACK);
			FAI_TESTMENU_ChangeMenu(hMenu, TESTMENU_LINE_2, szAgcStr, RGB_COLOR_BLACK);
		}
		else
		{
			FAI_TESTMENU_ChangeMenu(hMenu, TESTMENU_LINE_1, "AGC Fail", RGB_COLOR_RED);
			FAI_TESTMENU_ChangeMenu(hMenu, TESTMENU_LINE_2, szAgcStr, RGB_COLOR_RED);
		}
		
		if((lBerAverage >= ulBerRangeMin) && (lBerAverage <= ulBerRangeMax))
		{
			FAI_TESTMENU_ChangeMenu(hMenu, TESTMENU_LINE_3, "BER Pass", RGB_COLOR_BLACK);
			FAI_TESTMENU_ChangeMenu(hMenu, TESTMENU_LINE_4, szBerStr, RGB_COLOR_BLACK);
		}
		else
		{
			FAI_TESTMENU_ChangeMenu(hMenu, TESTMENU_LINE_3, "BER Fail", RGB_COLOR_RED);
			FAI_TESTMENU_ChangeMenu(hMenu, TESTMENU_LINE_4, szBerStr, RGB_COLOR_RED);
		}

		
		if((lAgcAverage >= ulAgcRangeMin) &&
			(lAgcAverage <= ulAgcRangeMax) &&
			(lBerAverage >= ulBerRangeMin) &&
			(lBerAverage <= ulBerRangeMax))
		{
			FDI_PANEL_Display(FAI_CHANNEL_PERFORMANCE_PASS_STR);
		}
		else
		{
			FDI_PANEL_Display(FAI_CHANNEL_PERFORMANCE_FAIL_STR);
		}
	}

	return FAI_NO_ERR;

TEST_FAIL:
	sprintf(szCheckStr, "T%d NO SIGNAL",ulTunerId);
	FAI_TESTMENU_ChangeMenuColor(hMenu, RGB_COLOR_RED);
	FAI_TESTMENU_ChangeMenu(hMenu, TESTMENU_LINE_2, szCheckStr, RGB_COLOR_RED);
	FAI_TESTMENU_ChangeRCUColor(hMenu, RGB_COLOR_BLACK);
	FDI_PANEL_Display(FAI_CHANNEL_PERFORMANCE_FAIL_STR);

	return FAI_ERR;;
}

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
static FAI_ERR_CODE INTRN_FAI_CHANNEL_C_RequestLock(unsigned short usMsgId, unsigned long ulTunerId, 	FDI_CH_CAB_TUNE_PARAM_t tCabTuneParam)
{
	BOOL bIsLocked;
	FAI_TMD_RESULT_T tTMDResult;
	unsigned char ucTmdErrCode[2];
	FDI_ERR_CODE eFdiErr = FDI_NO_ERR;
	int i;

	FDI_CHANNEL_C_CancelLock(ulTunerId);//add junkh
	
	FVK_MEM_Set(&tTMDResult, 0x00, sizeof(tTMDResult));
	
	FAI_PRINT(FAI_PRT_DBGINFO, ("===============================================\n"));
	FAI_PRINT(FAI_PRT_DBGINFO, ("[%s]\n", __func__));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tucTunerId=%d\n", ulTunerId));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tfreq=%d\n", tCabTuneParam.ulFrequency));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tsymbol=%d\n", tCabTuneParam.ulSymbolRate));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tmod=%d\n", tCabTuneParam.etModulation));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tSpectrum=%d\n", tCabTuneParam.etSpectrum));
	FAI_PRINT(FAI_PRT_DBGINFO, ("===============================================\n"));

	eFdiErr = FDI_CHANNEL_C_RequestLock(ulTunerId, &tCabTuneParam);
	if( eFdiErr != FDI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[%s] : Request lock Fail~!\n",__func__));
		ucTmdErrCode[0] = FAI_CATEGORY_CHANNEL;
		ucTmdErrCode[1] = FAI_CHANNEL_TMD_ERR_REQUEST_LOCK_FAIL;
		goto TEST_FAIL;
	}

	/* Locking check */
	for( i = 0; i < 30; i++ )//FAI_CHANNEL_LOCK_CHECK_RETRY_NUM
	{
		FDI_CHANNEL_C_IsLocked(ulTunerId, &bIsLocked);
		if( bIsLocked == TRUE )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[%s] : %d tuner Locking ok!\n",__func__, ulTunerId));
			break;
		}
		FVK_TASK_Sleep(FAI_CHANNEL_LOCK_RETRY_DELAY);
	}
	
	if( bIsLocked == FALSE )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[%s] : Locking Fail~!\n",__func__));
		ucTmdErrCode[0] = FAI_CATEGORY_CHANNEL;
		ucTmdErrCode[1] = FAI_CHANNEL_TMD_ERR_UNLOCK;
		goto TEST_FAIL;
	}

	FAI_PRINT(FAI_PRT_DBGINFO, ("[%s] %d tuner Locking Ok~!\n", __func__, ulTunerId));

	tTMDResult.ucCategory = 0x01;
	tTMDResult.ucModule = 0x03;
	tTMDResult.ucAction = 0x01;
	tTMDResult.eResult = FAI_TMD_ERR_OK;
	tTMDResult.usMsgId = usMsgId;
	FAI_TMD_SetResult(&tTMDResult);
	return FAI_NO_ERR;

TEST_FAIL:
	tTMDResult.ucCategory = 0x01;
	tTMDResult.ucModule = 0x03;
	tTMDResult.ucAction = 0x01;
	tTMDResult.eResult = FAI_TMD_ERR_FAIL;
	tTMDResult.usMsgId = usMsgId;
	FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));
	FAI_TMD_SetResult(&tTMDResult);
	return FAI_ERR;
}

