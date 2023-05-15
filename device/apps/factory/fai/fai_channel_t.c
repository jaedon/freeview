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
#include "fdi_scart.h"

/* model specific configuration header */
#include "factorysw_config.h"

/*******************************************************************************
* Debug level
*******************************************************************************/

/*******************************************************************************
* Definitions, typdef & const data
*******************************************************************************/
#define FAI_CHANNEL_T_CHANNEL_PERFORMANCE_EXP 7
	
#define FAI_TMD_PARAM_AGC_ID			0
#define FAI_TMD_PARAM_BER_ID			1
#define FAI_TMD_PARAM_SNR_ID			2
	
typedef enum
{
	FAI_CHANNEL_T_TEST_TYPE_PERFORMANCE_CHECK_UI_T0,
	FAI_CHANNEL_T_TEST_TYPE_PERFORMANCE_CHECK_UI_T1,
	FAI_CHANNEL_T_TEST_TYPE_NONE 
} FAI_CHANNEL_T_TEST_TYPE;

typedef struct FAI_CH_T_MSG
{
	FAI_CHANNEL_T_TEST_TYPE eChTestType;
	unsigned long ulTunerId;
	FDI_CH_TER_TUNE_PARAM_t tTerTuneParam;
	unsigned long ulAgcSnrMin;
	unsigned long ulAgcSnrMax;
	unsigned long ulBerMin;
	unsigned long ulBerMax;
	FAI_TESTMENU_H hChMenu;
} FAI_CH_T_MSG_t;

/*******************************************************************************
* External variables and functions
*******************************************************************************/
/* external variables  and functions */
extern unsigned long g_ulNumofCHDevice;
extern FAI_CH_TASK_t *g_pstCHTask;

/*******************************************************************************
* Global variables and structures
*******************************************************************************/
/* global variables and structures */
static unsigned long s_ulTerChMsgIdForUI, s_ulTerChTaskIdForUI;

static void INTRN_FAI_CHANNEL_T_Task_UI(void *data);
static FAI_ERR_CODE INTRN_FAI_CHANNEL_T_PerformanceCheck_UI(unsigned long ulTunerId,
													FDI_CH_TER_TUNE_PARAM_t tTerTuneParam,
													unsigned long ulAgcRangeMin,
													unsigned long ulAgcRangeMax,
													unsigned long ulBerRangeMin,
													unsigned long ulBerRangeMax,
													FAI_TESTMENU_H hMenu);

static FAI_ERR_CODE INTRN_FAI_CHANNEL_T_CheckPerformance(unsigned short usMsgId, unsigned long ulTunerId, FDI_CH_TER_TUNE_PARAM_t tTerTuneParam);
static FAI_ERR_CODE INTRN_FAI_CHANNEL_T_RequestLock(unsigned short usMsgId, unsigned long ulTunerId, FDI_CH_TER_TUNE_PARAM_t tTerTuneParam);


/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_CHANNEL_T_Init(void)
{
	int i;
	
#if ((TER_CH_PERFORMANCE_TUNER0_UI_ENABLE==YES) || (TER_CH_PERFORMANCE_TUNER1_UI_ENABLE==YES))
	if( FVK_MSG_Create(FVK_TASK_DEFAULT_QUEUE_SIZE, sizeof(FAI_CH_T_MSG_t), (char*)"TerChMsgForUI", &s_ulTerChMsgIdForUI) != FVK_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_CHANNEL_T_Init] : FVK_MSG_Create \n"));
		return FVK_ERR;
	}
	if( FVK_TASK_Create(INTRN_FAI_CHANNEL_T_Task_UI, FVK_TASK_DEFAULT_PRIORITY, FVK_TASK_DEFAULT_STACK_SIZE, "SatChTaskForUI", NULL, &s_ulTerChTaskIdForUI) != FVK_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_CHANNEL_T_Init] : FVK_TASK_Create Error!!\n"));
		return FVK_ERR;
	}
	if( FVK_TASK_Start(s_ulTerChTaskIdForUI) != FVK_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_CHANNEL_T_Init] : FVK_TASK_Start Error!!\n"));
		return FVK_ERR;
	}
#endif

#if ((CHANNEL_TER_LOCK_REQUEST_ENABLE==YES) || (CHANNEL_TER_LOCK_FOR_PERFORMANCE_ENABLE==YES))
	for(i=0; i<g_ulNumofCHDevice; i++)
	{
		g_pstCHTask[i].pfnTERRequestLock = INTRN_FAI_CHANNEL_T_RequestLock;
		g_pstCHTask[i].pfnTERCheckPerformance = INTRN_FAI_CHANNEL_T_CheckPerformance;
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
#if defined(CHANNEL_TER_LOCK_REQUEST_ENABLE) && (CHANNEL_TER_LOCK_REQUEST_ENABLE==YES)
FAI_ERR_CODE FAI_CHANNEL_T_RequestLock(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList)
{
	unsigned char ucTunerId, ucTerSystem, aucFreqeuncy[4], ucBandWidth, ucModulation, ucFreqLen;
	FAI_CH_MSG_t tChMsg;
	FVK_ERR_CODE eVkRet;
	FAI_TMD_RESULT_T tTMDResult;
	unsigned char ucTmdErrCode[2];

	FVK_MEM_Set(&tTMDResult, 0x00, sizeof(tTMDResult));

	if( ucArgc != 5 )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[%s] : Argument Count Error=%d\n", __func__, ucArgc));
		tTMDResult.eResult = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
		ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_INVALID_ARGUMENT;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));		
		goto TEST_FAIL;
	}

	FAI_TMD_GetParam(&ucTunerId, pucArgList, 0);
	FAI_TMD_GetParam(&ucTerSystem, pucArgList, 1);
	ucFreqLen = FAI_TMD_GetParam(aucFreqeuncy, pucArgList, 2);
	FAI_TMD_GetParam(&ucBandWidth, pucArgList, 3);
	FAI_TMD_GetParam(&ucModulation, pucArgList, 4);

	if(ucTunerId >= g_ulNumofCHDevice)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[%s] : Invalid parameter Error, TunerID =%d\n", __func__, ucTunerId));
		tTMDResult.eResult = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
		ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_INVALID_PARAMETER;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));	
		goto TEST_FAIL;
	}

	tChMsg.usMsgId = usMsgId;
	tChMsg.ulTunerId = ucTunerId;

    //sorry for hard coding 
	if(ucTerSystem == 3)tChMsg.tTerTuneParam.etTerSystem = FDI_CH_DVBT;
	else tChMsg.tTerTuneParam.etTerSystem = FDI_CH_DVBT2;
	
	tChMsg.eChTestType = FAI_CHANNEL_TEST_TYPE_TER_REQUEST_LOCK;
	FAI_TMD_ArgToValue(&tChMsg.tTerTuneParam.ulFrequency, aucFreqeuncy, ucFreqLen);
	tChMsg.tTerTuneParam.etBandWidth = ucBandWidth;
	tChMsg.tTerTuneParam.etCodeRate = FDI_CH_TCODERATE_AUTO;
	tChMsg.tTerTuneParam.etGuardInterval = FDI_CH_GUARDINTERVAL_AUTO;
	tChMsg.tTerTuneParam.etHierarchy = FDI_CH_HIERARCHY_AUTO;
	tChMsg.tTerTuneParam.etModulation = ucModulation;
	tChMsg.tTerTuneParam.etOffset = FDI_CH_OFFSET_AUTO;
	tChMsg.tTerTuneParam.etTransMode = FDI_CH_TRANSMODE_AUTO;
	tChMsg.tTerTuneParam.etStream = FDI_CH_STREAM_HP;

	FAI_PRINT(FAI_PRT_DBGINFO, ("===============================================\n"));
	FAI_PRINT(FAI_PRT_DBGINFO, ("[%s]\n", __func__));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tucTunerId=%d\n",tChMsg.ulTunerId));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\teChTestType=%d\n",tChMsg.eChTestType));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tfreq=%d\n", tChMsg.tTerTuneParam.ulFrequency));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tetBandWidth=%d\n", tChMsg.tTerTuneParam.etBandWidth));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tetModulation=%d\n", tChMsg.tTerTuneParam.etModulation));
	FAI_PRINT(FAI_PRT_DBGINFO, ("===============================================\n"));

	eVkRet = FVK_MSG_Send(g_pstCHTask[ucTunerId].ulMSGQID, &tChMsg, sizeof(FAI_CH_MSG_t));
	if ( eVkRet != FVK_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[%s] : Error FVK_MSG_Send() : %d\n", __func__, eVkRet));
		tTMDResult.eResult = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
		ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_MESSAGE_SEND_FAIL;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));	
		goto TEST_FAIL;
	}

	return FAI_NO_ERR;

TEST_FAIL:
	
	tTMDResult.ucCategory = FAI_CATEGORY_CHANNEL;
	tTMDResult.ucModule = 0x02;
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
#if (CHANNEL_TER_CH_ANT_VOLT_ENABLE==YES)
FAI_ERR_CODE FAI_CHANNEL_T_AntennaVolt(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList)
{
	unsigned char ucOnOff;
	BOOL bAntPowerOn_nOff = FALSE;
	FAI_TMD_RESULT_T tTMDResult;
	unsigned char ucTmdErrCode[2];
	FDI_ERR_CODE eFdiErr = FDI_NO_ERR;
	int i;
     
	FVK_MEM_Set(&tTMDResult, 0x00, sizeof(tTMDResult));

	if( ucArgc != 1 )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[%s] : Argument Count Error=%d\n", __func__, ucArgc));
		tTMDResult.eResult = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
		ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_INVALID_ARGUMENT;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));			
		goto TEST_FAIL;
	}

	FAI_TMD_GetParam(&ucOnOff, pucArgList, 0);

	FAI_PRINT(FAI_PRT_DBGINFO, ("===============================================\n"));
	FAI_PRINT(FAI_PRT_DBGINFO, ("[%s]\n", __func__));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tucOnOff=%d\n", ucOnOff));
	FAI_PRINT(FAI_PRT_DBGINFO, ("===============================================\n"));

	bAntPowerOn_nOff = ucOnOff?TRUE:FALSE;

	for( i = 0; i < g_ulNumofCHDevice; i++)
	{
		/* anntenna power on_off */
		eFdiErr = FDI_CHANNEL_T_SetAntennaPower(i, bAntPowerOn_nOff);
		if( eFdiErr != FDI_NO_ERR )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[%s] : SetAntennaPower Error=%d\n", __func__, eFdiErr));
			tTMDResult.eResult = FAI_TMD_ERR_FAIL;
			ucTmdErrCode[0] = FAI_CATEGORY_CHANNEL;
			ucTmdErrCode[1] = FAI_CHANNEL_TMD_ERR_SET_ANTENNA_POWER_FAIL;
			FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));
			goto TEST_FAIL;
		}
		FAI_PRINT(FAI_PRT_DBGINFO, ("[%s] : SetAntennaPower ok! ~\n", __func__));
	}

	tTMDResult.eResult = FAI_TMD_ERR_OK;
	FAI_TMD_SetResult(&tTMDResult);
	return FAI_NO_ERR;
	
TEST_FAIL:
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
FAI_ERR_CODE FAI_CHANNEL_T_PerformanceCheck(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList)
{
	unsigned char ucTunerId, aucFreqeuncy[4], ucBandWidth, ucModulation, ucFreqLen;
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

	FAI_TMD_GetParam(&ucTunerId, pucArgList, 0);
	ucFreqLen = FAI_TMD_GetParam(aucFreqeuncy, pucArgList, 1);
	FAI_TMD_GetParam(&ucBandWidth, pucArgList, 2);
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

	tChMsg.usMsgId = usMsgId;
	tChMsg.ulTunerId = ucTunerId;
	tChMsg.eChTestType = FAI_CHANNEL_TEST_TYPE_TER_CHECK_PERFORMANCE;
	FAI_TMD_ArgToValue(&tChMsg.tTerTuneParam.ulFrequency, aucFreqeuncy, ucFreqLen);
	tChMsg.tTerTuneParam.etBandWidth = ucBandWidth;
	tChMsg.tTerTuneParam.etCodeRate = FDI_CH_TCODERATE_AUTO;
	tChMsg.tTerTuneParam.etGuardInterval = FDI_CH_GUARDINTERVAL_AUTO;
	tChMsg.tTerTuneParam.etHierarchy = FDI_CH_HIERARCHY_AUTO;
	tChMsg.tTerTuneParam.etModulation = ucModulation;
	tChMsg.tTerTuneParam.etOffset = FDI_CH_OFFSET_AUTO;
	tChMsg.tTerTuneParam.etTransMode = FDI_CH_TRANSMODE_AUTO;
	tChMsg.tTerTuneParam.etStream = FDI_CH_STREAM_HP;

	FAI_PRINT(FAI_PRT_DBGINFO, ("===============================================\n"));
	FAI_PRINT(FAI_PRT_DBGINFO, ("[%s]\n", __func__));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tucTunerId=%d\n",tChMsg.ulTunerId));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\teChTestType=%d\n",tChMsg.eChTestType));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tfreq=%d\n", tChMsg.tTerTuneParam.ulFrequency));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tetBandWidth=%d\n", tChMsg.tTerTuneParam.etBandWidth));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tetModulation=%d\n", tChMsg.tTerTuneParam.etModulation));
	FAI_PRINT(FAI_PRT_DBGINFO, ("===============================================\n"));

	eVkRet = FVK_MSG_Send(g_pstCHTask[ucTunerId].ulMSGQID, &tChMsg, sizeof(FAI_CH_MSG_t));
	if ( eVkRet != FVK_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[%s] : Error FVK_MSG_Send() : %d\n", __func__, eVkRet));
		tTMDResult.eResult = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
		ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_MESSAGE_SEND_FAIL;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));	
		goto TEST_FAIL;
	}

	return FAI_NO_ERR;

TEST_FAIL:
	
	tTMDResult.ucCategory = FAI_CATEGORY_CHANNEL;
	tTMDResult.ucModule = 0x02;
	tTMDResult.ucAction = 0x03;
	tTMDResult.usMsgId = usMsgId;
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
#if (TER_CH_ANT_VOLT_UI_ENABLE==YES) 
FAI_ERR_CODE FAI_CHANNEL_T_AntennaVolt_UI(void)
{
	int i;
	FDI_ERR_CODE eFdiErr;
	FDI_CH_POWER_e eChPower;

	if( GET_STEP() == 0 )
	{
		FDI_PANEL_Display(FAI_VOLTAGE_STEP1_STR);
		FAI_TESTMENU_ChangeCurrentRCUColor(RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "STEP1", RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "5V ON", RGB_COLOR_BLACK);
		for( i = 0; i < g_ulNumofCHDevice; i++)
		{
			/* anntenna power on */
			FDI_CHANNEL_T_SetAntennaPower(i, TRUE);
		}
		SET_STEP(1);
	}
	else if( GET_STEP() == 1 )
	{
		FDI_PANEL_Display(FAI_VOLTAGE_STEP2_STR);
		FAI_TESTMENU_ChangeCurrentRCUColor(RGB_COLOR_GREEN);
		FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "STEP2", RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "5V OFF", RGB_COLOR_BLACK);
		for( i = 0; i < g_ulNumofCHDevice; i++)
		{
			/* anntenna power off */
			FDI_CHANNEL_T_SetAntennaPower(i, FALSE);
		}
#if (TER_CH_ANT_VOLT_UI_USE_3STEP==YES)
		SET_STEP(2);
	}
	else if( GET_STEP() == 2 )
	{
		FDI_PANEL_Display(FAI_VOLTAGE_STEP2_STR);
		FAI_TESTMENU_ChangeCurrentRCUColor(RGB_COLOR_YELLOW);
		FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "STEP2", RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "5V OFF", RGB_COLOR_BLACK);
#endif
		SET_STEP(0);
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
#if (TER_CH_PERFORMANCE_TUNER0_UI_ENABLE==YES)
FAI_ERR_CODE FAI_CHANNEL_T_PerformanceTuner0_UI(void)
{
	FAI_CH_T_MSG_t tChMsg;
	FVK_ERR_CODE eVkRet;

	tChMsg.ulTunerId = 0;
	tChMsg.eChTestType = FAI_CHANNEL_T_TEST_TYPE_PERFORMANCE_CHECK_UI_T0;
	tChMsg.ulAgcSnrMin = TER_CH_PERFORMANCE_TUNER0_UI_AGC_MIN;
	tChMsg.ulAgcSnrMax = TER_CH_PERFORMANCE_TUNER0_UI_AGC_MAX;
	tChMsg.ulBerMin = TER_CH_PERFORMANCE_TUNER0_UI_BER_MIN;
	tChMsg.ulBerMax = TER_CH_PERFORMANCE_TUNER0_UI_BER_MAX;
	FAI_TESTMENU_GetCurrentMenu(&tChMsg.hChMenu);

	tChMsg.tTerTuneParam.ulFrequency = TER_CH_PERFORMANCE_TUNER0_UI_FREQUENCY;
	tChMsg.tTerTuneParam.etBandWidth = TER_CH_PERFORMANCE_TUNER0_UI_BANDWITDH;
	tChMsg.tTerTuneParam.etCodeRate = FDI_CH_TCODERATE_AUTO;
	tChMsg.tTerTuneParam.etGuardInterval = FDI_CH_GUARDINTERVAL_AUTO;
	tChMsg.tTerTuneParam.etHierarchy = FDI_CH_HIERARCHY_AUTO;
	tChMsg.tTerTuneParam.etModulation = TER_CH_PERFORMANCE_TUNER0_UI_MODULATION;
	tChMsg.tTerTuneParam.etOffset = FDI_CH_OFFSET_AUTO;
	tChMsg.tTerTuneParam.etTransMode = FDI_CH_TRANSMODE_AUTO;
	tChMsg.tTerTuneParam.etStream = FDI_CH_STREAM_HP;

	eVkRet = FVK_MSG_Send(s_ulTerChMsgIdForUI, &tChMsg, sizeof(FAI_CH_T_MSG_t));
	if (eVkRet != FVK_NO_ERR)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_CHANNEL_T_PerformanceTuner0_UI] : Error FVK_MSG_Send() : %d\n", eVkRet));
		return FAI_ERR;
	}
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
#if (TER_CH_PERFORMANCE_TUNER1_UI_ENABLE==YES)
FAI_ERR_CODE FAI_CHANNEL_T_PerformanceTuner1_UI(void)
{
	FAI_CH_T_MSG_t tChMsg;
	FVK_ERR_CODE eVkRet;

	tChMsg.ulTunerId = 1;
	tChMsg.eChTestType = FAI_CHANNEL_T_TEST_TYPE_PERFORMANCE_CHECK_UI_T0;
	tChMsg.ulAgcSnrMin = TER_CH_PERFORMANCE_TUNER1_UI_AGC_MIN;
	tChMsg.ulAgcSnrMax = TER_CH_PERFORMANCE_TUNER1_UI_AGC_MAX;
	tChMsg.ulBerMin = TER_CH_PERFORMANCE_TUNER1_UI_BER_MIN;
	tChMsg.ulBerMax = TER_CH_PERFORMANCE_TUNER1_UI_BER_MAX;
	FAI_TESTMENU_GetCurrentMenu(&tChMsg.hChMenu);

	tChMsg.tTerTuneParam.ulFrequency = TER_CH_PERFORMANCE_TUNER1_UI_FREQUENCY;
	tChMsg.tTerTuneParam.etBandWidth = TER_CH_PERFORMANCE_TUNER1_UI_BANDWITDH;
	tChMsg.tTerTuneParam.etCodeRate = FDI_CH_TCODERATE_AUTO;
	tChMsg.tTerTuneParam.etGuardInterval = FDI_CH_GUARDINTERVAL_AUTO;
	tChMsg.tTerTuneParam.etHierarchy = FDI_CH_HIERARCHY_AUTO;
	tChMsg.tTerTuneParam.etModulation = TER_CH_PERFORMANCE_TUNER1_UI_MODULATION;
	tChMsg.tTerTuneParam.etOffset = FDI_CH_OFFSET_AUTO;
	tChMsg.tTerTuneParam.etTransMode = FDI_CH_TRANSMODE_AUTO;
	tChMsg.tTerTuneParam.etStream = FDI_CH_STREAM_HP;

	eVkRet = FVK_MSG_Send(s_ulTerChMsgIdForUI, &tChMsg, sizeof(FAI_CH_T_MSG_t));
	if (eVkRet != FVK_NO_ERR)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_CHANNEL_T_PerformanceTuner0_UI] : Error FVK_MSG_Send() : %d\n", eVkRet));
		return FAI_ERR;
	}
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
static void INTRN_FAI_CHANNEL_T_Task_UI(void *data)
{
	FAI_CH_T_MSG_t tChMsg;
	FVK_ERR_CODE eVkRet;

	while(1)
	{
		eVkRet = FVK_MSG_Receive(s_ulTerChMsgIdForUI, &tChMsg, sizeof(FAI_CH_T_MSG_t));
		if (eVkRet != FVK_NO_ERR)
		{
			FAI_PRINT(FAI_PRT_WARNING, ("[INTRN_FAI_CHANNEL_T_Task] : Error FVK_MSG_Receive() %d\n", eVkRet));
			continue;
		}

		if( (tChMsg.eChTestType == FAI_CHANNEL_T_TEST_TYPE_PERFORMANCE_CHECK_UI_T0) || (tChMsg.eChTestType == FAI_CHANNEL_T_TEST_TYPE_PERFORMANCE_CHECK_UI_T1) )
		{
			INTRN_FAI_CHANNEL_T_PerformanceCheck_UI(tChMsg.ulTunerId,
   											   tChMsg.tTerTuneParam,
   											   tChMsg.ulAgcSnrMin,
   											   tChMsg.ulAgcSnrMax,
   											   tChMsg.ulBerMin,
   											   tChMsg.ulBerMax,
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
static FAI_ERR_CODE INTRN_FAI_CHANNEL_T_RequestLock(unsigned short usMsgId, unsigned long ulTunerId, FDI_CH_TER_TUNE_PARAM_t tTerTuneParam)
{
	BOOL bIsLocked;
	FAI_TMD_RESULT_T tTMDResult;
	unsigned char ucTmdErrCode[2];
	FDI_ERR_CODE eFdiErr = FDI_NO_ERR;
	int i;

	FVK_MEM_Set(&tTMDResult, 0x00, sizeof(tTMDResult));

	FAI_PRINT(FAI_PRT_DBGINFO, ("===============================================\n"));
	FAI_PRINT(FAI_PRT_DBGINFO, ("[%s]\n", __func__));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tucTunerId=%d\n", ulTunerId));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tfreq=%d\n", tTerTuneParam.ulFrequency));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tetBandWidth=%d\n", tTerTuneParam.etBandWidth));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tetModulation=%d\n", tTerTuneParam.etModulation));
	FAI_PRINT(FAI_PRT_DBGINFO, ("===============================================\n"));

	eFdiErr = FDI_CHANNEL_T_RequestLock(ulTunerId, &tTerTuneParam);
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
		FDI_CHANNEL_T_IsLocked(ulTunerId, &bIsLocked);
		if(bIsLocked == TRUE)
		{
			break;
		}
		VK_TASK_Sleep(FAI_CHANNEL_LOCK_RETRY_DELAY);
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
	tTMDResult.ucModule = 0x02;
	tTMDResult.ucAction = 0x02;
	tTMDResult.eResult = FAI_TMD_ERR_OK;
	tTMDResult.usMsgId = usMsgId;
	FAI_TMD_SetResult(&tTMDResult);
	return FAI_NO_ERR;

TEST_FAIL:
	tTMDResult.ucCategory = 0x01;
	tTMDResult.ucModule = 0x02;
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
static FAI_ERR_CODE INTRN_FAI_CHANNEL_T_CheckPerformance(unsigned short usMsgId, unsigned long ulTunerId, FDI_CH_TER_TUNE_PARAM_t tTerTuneParam)
{
	FDI_CH_TER_TUNE_PARAM_t tTerParam;
	FDI_CH_TER_SIGNAL_QUALITY_t tSignalInfo;
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

	FAI_PRINT(FAI_PRT_DBGINFO, ("===============================================\n"));
	FAI_PRINT(FAI_PRT_DBGINFO, ("[%s]\n", __func__));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tucTunerId=%d\n", ulTunerId));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tfreq=%d\n", tTerTuneParam.ulFrequency));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tetBandWidth=%d\n", tTerTuneParam.etBandWidth));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tetModulation=%d\n", tTerTuneParam.etModulation));
	FAI_PRINT(FAI_PRT_DBGINFO, ("===============================================\n"));

	FVK_MEM_Set(&tTMDResult, 0x00, sizeof(tTMDResult));
	FVK_MEM_Set(aucAgcResult, 0x00, sizeof(aucAgcResult));
	FVK_MEM_Set(aucBerResult, 0x00, sizeof(aucBerResult));
	FVK_MEM_Set(aucSnrResult, 0x00, sizeof(aucSnrResult));
	FVK_MEM_Set(&tTerParam, 0x00, sizeof(tTerParam));
	FVK_MEM_Set(&tSignalInfo, 0x00, sizeof(tSignalInfo));

	/* 이미 원하는 주파수로 락이 되어있으면 정보만 업데이트한다(requirement). */
	FDI_CHANNEL_T_IsLocked(ulTunerId, &bIsLocked);
	FDI_CHANNEL_T_GetLockedInfo(ulTunerId, &tTerParam);
	if( (bIsLocked == FALSE) || (tTerTuneParam.ulFrequency != tTerParam.ulFrequency) )
	{
		FAI_PRINT(FAI_PRT_DBGINFO, ("==>Lock Try\n"));

		eFdiErr = FDI_CHANNEL_T_RequestLock(ulTunerId, &tTerTuneParam);
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
			FDI_CHANNEL_T_IsLocked(ulTunerId, &bIsLocked);
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
	}

	for( i = 0; i < FAI_CHANNEL_BER_TEST_SAMPLING_NUM; i++ )
	{
		FVK_TASK_Sleep(FAI_CHANNEL_BER_TEST_SAMPLING_DELAY);
		eFdiErr = FDI_CHANNEL_T_GetSignalInfo(ulTunerId, &tSignalInfo);
		if( eFdiErr != FDI_NO_ERR )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[%s] : Get signal info Fail~!\n",__func__));
			ucTmdErrCode[0] = FAI_CATEGORY_CHANNEL;
			ucTmdErrCode[1] = FAI_CHANNEL_TMD_ERR_GET_SIGNAL_INFO_FAIL;
			goto TEST_FAIL;
		}
#if (CHANNEL_TER_AGC_PERFORMANCE_ENABLE == YES)
		alAgcValue[i] = (signed long)(tSignalInfo.unAgc);
#endif
#if (CHANNEL_TER_BER_PERFORMANCE_ENABLE == YES)
		alBerValue[i] = (signed long)(tSignalInfo.fBer);
#endif
#if (CHANNEL_TER_SNR_PERFORMANCE_ENABLE == YES)
		alSnrValue[i] = (signed long)(tSignalInfo.fSnr);
#endif
	}

#if (CHANNEL_TER_AGC_PERFORMANCE_ENABLE == YES)
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
	aucAgcResult[4] = (CHANNEL_TER_AGC_POWER_CONSTANT); /* 소수점 지수 */
#endif

#if (CHANNEL_TER_BER_PERFORMANCE_ENABLE == YES)
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

	aucBerResult[0] = (lBerAverage&0x7F000000)>>24;
	aucBerResult[1] = (lBerAverage&0x00FF0000)>>16;
	aucBerResult[2] = (lBerAverage&0x0000FF00)>>8;
	aucBerResult[3] = (lBerAverage&0x000000FF);
	aucBerResult[4] = (CHANNEL_TER_BER_POWER_CONSTANT); /* 소수점 지수 */
#endif

#if (CHANNEL_TER_SNR_PERFORMANCE_ENABLE == YES)
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

	aucSnrResult[0] = (lSnrAverage&0x7F000000)>>24;
	aucSnrResult[1] = (lSnrAverage&0x00FF0000)>>16;
	aucSnrResult[2] = (lSnrAverage&0x0000FF00)>>8;
	aucSnrResult[3] = (lSnrAverage&0x000000FF);
	aucSnrResult[4] = (CHANNEL_TER_SNR_POWER_CONSTANT); /* 소수점 지수 */
#endif

	FAI_PRINT(FAI_PRT_DBGINFO, ("===============================================\n"));
	FAI_PRINT(FAI_PRT_DBGINFO, ("AGC : 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x\n", aucAgcResult[0], aucAgcResult[1], aucAgcResult[2], aucAgcResult[3], aucAgcResult[4], aucAgcResult[5]));
	FAI_PRINT(FAI_PRT_DBGINFO, ("BER : 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x\n", aucBerResult[0], aucBerResult[1], aucBerResult[2], aucBerResult[3], aucBerResult[4], aucBerResult[5]));
	FAI_PRINT(FAI_PRT_DBGINFO, ("SNR : 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x\n", aucSnrResult[0], aucSnrResult[1], aucSnrResult[2], aucSnrResult[3], aucSnrResult[4], aucSnrResult[5]));
	FAI_PRINT(FAI_PRT_DBGINFO, ("===============================================\n"));
	
	tTMDResult.ucCategory = 0x01;
	tTMDResult.ucModule = 0x02;
	tTMDResult.ucAction = 0x03;
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
	tTMDResult.ucModule = 0x02;
	tTMDResult.ucAction = 0x03;
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
static FAI_ERR_CODE INTRN_FAI_CHANNEL_T_PerformanceCheck_UI(unsigned long ulTunerId,
													FDI_CH_TER_TUNE_PARAM_t tTerTuneParam,
													unsigned long ulAgcRangeMin,
													unsigned long ulAgcRangeMax,
													unsigned long ulBerRangeMin,
													unsigned long ulBerRangeMax,
													FAI_TESTMENU_H hMenu)
{
	FDI_CH_TER_SIGNAL_QUALITY_t tSignalInfo;
	BOOL bIsLocked;
	int i;
	signed long alAgcValue[FAI_CHANNEL_BER_TEST_SAMPLING_NUM], alBerValue[FAI_CHANNEL_BER_TEST_SAMPLING_NUM];
	signed long lAgcMin, lAgcMax, lBerMin, lBerMax, lSumOfAgc, lSumOfBer;
	signed long lAgcAverage, lBerAverage;
	char szCheckStr[FAI_CHANNEL_AVG_VALUE_STR_LEN], szAgcStr[FAI_CHANNEL_AVG_VALUE_STR_LEN], szBerStr[FAI_CHANNEL_AVG_VALUE_STR_LEN];
	unsigned char aucAgcResult[9], aucBerResult[9];
	
	if( hMenu != NULL )
	{
		FAI_TESTMENU_ChangeRCUColor(hMenu, RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeMenuColor(hMenu, RGB_COLOR_YELLOW);
		FAI_TESTMENU_ChangeMenu(hMenu, TESTMENU_LINE_2, "BER Check", RGB_COLOR_YELLOW);
	}
	
	FDI_CHANNEL_T_RequestLock(ulTunerId, &tTerTuneParam);

	/* Locking check */
	for( i = 0; i < FAI_CHANNEL_LOCK_CHECK_RETRY_NUM; i++ )
	{
		if( hMenu != NULL )
		{
			sprintf(szCheckStr, "BER Check(%d)", i);
			FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, szCheckStr, RGB_COLOR_YELLOW);
		}
		FDI_CHANNEL_T_IsLocked(ulTunerId, &bIsLocked);
		if(bIsLocked == TRUE)
		{
			break;
		}
		VK_TASK_Sleep(FAI_CHANNEL_LOCK_RETRY_DELAY);
	}
	if( bIsLocked == FALSE )
	{
		goto TEST_FAIL;
	}

	for( i = 0; i < FAI_CHANNEL_BER_TEST_SAMPLING_NUM; i++ )
	{
		VK_TASK_Sleep(FAI_CHANNEL_BER_TEST_SAMPLING_DELAY);
		FDI_CHANNEL_T_GetSignalInfo(ulTunerId, &tSignalInfo);
		alAgcValue[i] = (signed long)(tSignalInfo.unAgc);
		alBerValue[i] = (signed long)(tSignalInfo.fBer);
	}

	lAgcMin = lAgcMax = alAgcValue[0];
	lBerMin = lBerMax = alBerValue[0];
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
		if( lAgcMax < alAgcValue[i] )	/* max agc */
		{
			lAgcMax = alAgcValue[i];
		}
		if( lBerMax < alBerValue[i] )	/* max ber */
		{
			lBerMax = alBerValue[i];
		}
	}

	lSumOfAgc = lSumOfBer = 0;
	for( i = 0; i < FAI_CHANNEL_BER_TEST_SAMPLING_NUM; i++ )
	{
		lSumOfAgc += alAgcValue[i];
		lSumOfBer += alBerValue[i];
	}
	printf("(lAgcMin %d, lAgcMax %d, lBerMin %d, lBerMax %d)\n", lAgcMin, lAgcMax, lBerMin, lBerMax);
	printf("(lSumOfAgc %d, lSumOfBer %d)\n", lSumOfAgc, lSumOfBer);

	lAgcAverage = (long)(lSumOfAgc-lAgcMin-lAgcMax)/(FAI_CHANNEL_BER_TEST_SAMPLING_NUM-2);
	lBerAverage = (long)(lSumOfBer-lBerMin-lBerMax)/(FAI_CHANNEL_BER_TEST_SAMPLING_NUM-2);
	printf("(lSumOfAgc %d, lSumOfBer %d) (lAgcAverage %d, lBerAverage %d)\n", lSumOfAgc, lSumOfBer, lAgcAverage, lBerAverage);
	printf("(ulAgcRangeMin %d, ulAgcMax %d) (ulBerMin %d, ulBerMax %d)\n", ulAgcRangeMin, ulAgcRangeMax, ulBerRangeMin, ulBerRangeMax);
	if( hMenu != NULL )
	{
#if 0
		sprintf(szAgcStr, "(%s %d.%d)", (lSumOfAgc<0)?"-":"+", ulAgcAverage/100, ulAgcAverage%100);
		sprintf(szBerStr, "(%s %d.%d)", (lSumOfBer<0)?"-":"+", ulBerAverage/100, ulBerAverage%100);
#else
		sprintf(szAgcStr, "(%d)", lAgcAverage);
		sprintf(szBerStr, "(%d)", lBerAverage);
#endif
		if((lAgcAverage >= ulAgcRangeMin) && (lAgcAverage <= ulAgcRangeMax))
		{
			FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_1, "AGC Pass", RGB_COLOR_BLACK);
			FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, szAgcStr, RGB_COLOR_BLACK);
		}
		else
		{
			FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_1, "AGC Fail", RGB_COLOR_RED);
			FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, szAgcStr, RGB_COLOR_RED);
		}

		if((lBerAverage >= ulBerRangeMin) && (lBerAverage <= ulBerRangeMax))
		{
			FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "BER Pass", RGB_COLOR_BLACK);
			FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_4, szBerStr, RGB_COLOR_BLACK);
		}
		else
		{
			FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "BER Fail", RGB_COLOR_RED);
			FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_4, szBerStr, RGB_COLOR_RED);
		}
	}
	else
	{
		/* Save result parameters */
		aucAgcResult[0] = 1;
		if((lAgcAverage >= ulAgcRangeMin) && (lAgcAverage <= ulAgcRangeMax))
		{
			aucAgcResult[1] = 1;
		}
		else
		{
			aucAgcResult[1] = 0;
		}
		aucAgcResult[2] = 3;
		if( lAgcAverage < 0 )
		{
			aucAgcResult[5] = 0x10;
			lAgcAverage = lAgcAverage*(-1);
		}
		else
		{
			aucAgcResult[5] = 0x00;
		}
		aucAgcResult[3] = (lAgcAverage&0xff00)>>8;
		aucAgcResult[4] = lAgcAverage&0x00ff;

		aucBerResult[0] = 1;
		if((lBerAverage >= ulBerRangeMin) && (lBerAverage <= ulBerRangeMax))
		{
			aucBerResult[1] = 1;
		}
		else
		{
			aucBerResult[1] = 0;
		}
		aucBerResult[2] = 3;
		if( lBerAverage < 0 )
		{
			aucBerResult[5] = 0x10;
			lBerAverage = lBerAverage*(-1);
		}
		else
		{
			aucBerResult[5] = 0x00;
		}
		aucBerResult[3] = (lBerAverage&0xff00)>>8;
		aucBerResult[4] = lBerAverage&0x00ff;

		FAI_TMD_SetCurrentResult(0, FAI_TMD_TEST_NO_ERROR, 2, aucAgcResult, 6);
		FAI_TMD_SetCurrentResult(1, FAI_TMD_TEST_NO_ERROR, 2, aucBerResult, 6);

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
	return FAI_NO_ERR;

TEST_FAIL:
	if( hMenu != NULL )
	{
		FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_RED);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "NO SIGNAL", RGB_COLOR_RED);
		FAI_TESTMENU_ChangeCurrentRCUColor(RGB_COLOR_BLACK);
		FDI_PANEL_Display(FAI_CHANNEL_PERFORMANCE_FAIL_STR);
	}
	return FAI_ERR;;
}

