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
typedef enum
{
	FAI_CHANNEL_S_TEST_TYPE_PERFORMANCE_CHECK_UI_T0,
	FAI_CHANNEL_S_TEST_TYPE_PERFORMANCE_CHECK_UI_T1,
	FAI_CHANNEL_S_TEST_TYPE_NONE
} FAI_CHANNEL_S_TEST_TYPE;

typedef struct FAI_CH_S_MSG
{
	FAI_CHANNEL_S_TEST_TYPE eChTestType;
	unsigned long ulTunerId;
	FDI_CH_SAT_TUNE_PARAM_t tSatTuneParam;
	unsigned long ulAgcMin;
	unsigned long ulAgcMax;
	unsigned long ulBerMin;
	unsigned long ulBerMax;
	FAI_TESTMENU_H hChMenu;
} FAI_CH_S_MSG_t;

/*******************************************************************************
* External variables and functions
*******************************************************************************/
/* external variables  and functions */

/*******************************************************************************
* Global variables and structures
*******************************************************************************/
/* global variables and structures */
static unsigned long s_ulSatChMsgIdForUI, s_ulSatChTaskIdForUI;

extern unsigned long g_ulNumofCHDevice;
extern FAI_CH_TASK_t *g_pstCHTask;

static void INTRN_FAI_CHANNEL_S_Task_UI(void *data);
static FAI_ERR_CODE INTRN_FAI_CHANNEL_S_PerformanceCheck_UI(unsigned long ulTunerId,
												FDI_CH_SAT_TUNE_PARAM_t tSatTuneParam,
												unsigned long ulAgcRangeMin,
												unsigned long ulAgcRangeMax,
												unsigned long ulBerRangeMin,
												unsigned long ulBerRangeMax,
												FAI_TESTMENU_H hMenu);

static FAI_ERR_CODE INTRN_FAI_CHANNEL_S_CheckPerformance(unsigned short usMsgId, unsigned long ulTunerId, FDI_CH_SAT_TUNE_PARAM_t tSatTuneParam) ;
static FAI_ERR_CODE INTRN_FAI_CHANNEL_S_RequestLock(unsigned short usMsgId, unsigned long ulTunerId, FDI_CH_SAT_TUNE_PARAM_t tSatTuneParam);


/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_CHANNEL_S_Init(void)
{
	int i;
	
#if ((SAT_CH_PERFORMANCE_TUNER0_UI_ENABLE==YES) || (SAT_CH_PERFORMANCE_TUNER1_UI_ENABLE==YES))
	if( FVK_MSG_Create(FVK_TASK_DEFAULT_QUEUE_SIZE, sizeof(FAI_CH_S_MSG_t), (char*)"SatChMsgForUI", &s_ulSatChMsgIdForUI) != FVK_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_CHANNEL_Init] : FVK_MSG_Create \n"));
		return FVK_ERR;
	}
	if( FVK_TASK_Create(INTRN_FAI_CHANNEL_S_Task_UI, FVK_TASK_DEFAULT_PRIORITY, FVK_TASK_DEFAULT_STACK_SIZE, "SatChTaskForUI", NULL, &s_ulSatChTaskIdForUI) != FVK_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_CHANNEL_Init] : FVK_TASK_Create Error!!\n"));
		return FVK_ERR;
	}
	if( FVK_TASK_Start(s_ulSatChTaskIdForUI) != FVK_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_CHANNEL_Init] : FVK_TASK_Start Error!!\n"));
		return FVK_ERR;
	}
#endif

#if ((CHANNEL_SAT_LOCK_REQUEST_ENABLE==YES) || (CHANNEL_SAT_LOCK_FOR_PERFORMANCE_ENABLE==YES))
	for(i=0; i<g_ulNumofCHDevice; i++)
	{
		g_pstCHTask[i].pfnSATRequestLock = INTRN_FAI_CHANNEL_S_RequestLock;
		g_pstCHTask[i].pfnSATCheckPerformance = INTRN_FAI_CHANNEL_S_CheckPerformance;
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
#if (CHANNEL_SAT_LNB_OUTPUT_ENABLE==YES)
FAI_ERR_CODE FAI_CHANNEL_S_LNBOutput(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList)
{
	unsigned char ucTunerId, ucPolar, ucTone, ucLnb;
	FAI_TMD_RESULT_T tTMDResult;
	unsigned char ucTmdErrCode[2];
	FDI_ERR_CODE eFdiErr = FDI_NO_ERR;

	FVK_MEM_Set(&tTMDResult, 0x00, sizeof(tTMDResult));

	if( ucArgc != 4 )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[%s] : Argument Count Error=%d\n",  __func__, ucArgc));
		tTMDResult.eResult = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
		ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_INVALID_ARGUMENT;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));
		goto TEST_FAIL;
	}

	FAI_TMD_GetParam(&ucTunerId, pucArgList, 0);
	FAI_TMD_GetParam(&ucPolar, pucArgList, 1);
	FAI_TMD_GetParam(&ucTone, pucArgList, 2);
	FAI_TMD_GetParam(&ucLnb, pucArgList, 3);

	FAI_PRINT(FAI_PRT_DBGINFO, ("===============================================\n"));
	FAI_PRINT(FAI_PRT_DBGINFO, ("[%s]\n", __func__));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tucTunerId=%d\n", ucTunerId));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tucPolar=%d\n", ucPolar));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tucTone=%d\n", ucTone));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tucLnb=%d\n", ucLnb));
	FAI_PRINT(FAI_PRT_DBGINFO, ("===============================================\n"));

	FDI_CHANNEL_S_SetInput(2,ucTunerId);

/*
Polarization_e
Auto:0x00  Horizontal:0x01  Vertical:0x02  Left:0x03  Right:0x04
*/

	/* Check parameters */
	if( (ucTunerId >= g_ulNumofCHDevice) || (ucPolar >= FDI_CH_POLAR_MAX) || (ucTone > 1) || (ucLnb > 1) )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[%s] : Invalid Parameter!! ucTunerId=%d, ucPolar=%d, ucTone=%d, ucLnb=%d\n", __func__, ucTunerId, ucPolar, ucTone, ucLnb));
		tTMDResult.eResult = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
		ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_INVALID_PARAMETER;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));
		goto TEST_FAIL;
	}

	if( ucLnb == 1 )
	{
			eFdiErr = FDI_CHANNEL_S_SetLnbPolarisation(ucTunerId, ucPolar, FDI_CH_LNB_VOLT_STD);
			if( eFdiErr != FDI_NO_ERR )
			{
				FAI_PRINT(FAI_PRT_ERROR, ("[%s] : SetLnbPolarisation ( %x ) Error=%d\n", __func__,ucPolar,eFdiErr));
				tTMDResult.eResult = FAI_TMD_ERR_FAIL;
				ucTmdErrCode[0] = FAI_CATEGORY_CHANNEL;
				ucTmdErrCode[1] = FAI_CHANNEL_TMD_ERR_SET_LNB_POLARIZATION_FAIL;
				FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));
				goto TEST_FAIL;
			}
			FAI_PRINT(FAI_PRT_DBGINFO, ("[%s] : SetLnbPolarisation ( %x ) ok! ~\n", __func__,ucPolar));
		eFdiErr = FDI_CHANNEL_S_SetLnb22KhzTone(ucTunerId, ucTone);
		if( eFdiErr != FDI_NO_ERR )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[%s] : SetLnb22KhzTone Error=%d\n", __func__, eFdiErr));
			tTMDResult.eResult = FAI_TMD_ERR_FAIL;
			ucTmdErrCode[0] = FAI_CATEGORY_CHANNEL;
			ucTmdErrCode[1] = FAI_CHANNEL_TMD_ERR_SET_LNB_22KHZ_TONE_FAIL;
			FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));
			goto TEST_FAIL;
		}
		FAI_PRINT(FAI_PRT_DBGINFO, ("[%s] : SetLnb22KhzTone ok! ~\n", __func__));
	}
	else
	{
		eFdiErr = FDI_CHANNEL_S_SetLnbOutput(ucTunerId, FALSE);
		if( eFdiErr != FDI_NO_ERR )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[%s] : SetLnbOutput Error=%d\n", __func__, eFdiErr));
			tTMDResult.eResult = FAI_TMD_ERR_FAIL;
			ucTmdErrCode[0] = FAI_CATEGORY_CHANNEL;
			ucTmdErrCode[1] = FAI_CHANNEL_TMD_ERR_SET_LNB_OUTPUT_FAIL;
			FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));
			goto TEST_FAIL;
		}
		FAI_PRINT(FAI_PRT_DBGINFO, ("[%s] : SetLnbOutpu ok! ~\n", __func__));
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
#if (CHANNEL_SAT_LOCK_REQUEST_ENABLE==YES)
FAI_ERR_CODE FAI_CHANNEL_S_RequestLock(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList)
{
	unsigned char ucTunerId, ucTransSpec, ucModulation, ucCodeRate, aucFreqeuncy[4], aucSymbolate[4], ucDiseqc, ucPilot, ucRollOff;
	FAI_CH_MSG_t tChMsg;
	FVK_ERR_CODE eVkRet;
	FAI_TMD_RESULT_T tTMDResult;
	unsigned char ucFreqSize, ucSymbolSize;
	unsigned char ucTmdErrCode[2];

	FVK_MEM_Set(&tTMDResult, 0x00, sizeof(tTMDResult));

	if( ucArgc != 9 )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[%s] : Argument Count Error=%d\n", __func__, ucArgc));
		tTMDResult.eResult = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
		ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_INVALID_ARGUMENT;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));
		goto TEST_FAIL;
	}

	FAI_TMD_GetParam(&ucTunerId, pucArgList, 0);
	FAI_TMD_GetParam(&ucTransSpec, pucArgList, 1);
	FAI_TMD_GetParam(&ucModulation, pucArgList, 2);
	FAI_TMD_GetParam(&ucCodeRate, pucArgList, 3);
	ucFreqSize = FAI_TMD_GetParam(aucFreqeuncy, pucArgList, 4);
	ucSymbolSize = FAI_TMD_GetParam(aucSymbolate, pucArgList, 5);
	FAI_TMD_GetParam(&ucDiseqc, pucArgList, 6);
	FAI_TMD_GetParam(&ucPilot, pucArgList, 7);
	FAI_TMD_GetParam(&ucRollOff, pucArgList, 8);

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
	tChMsg.ulTunerId = (unsigned long)ucTunerId;
	tChMsg.eChTestType = FAI_CHANNEL_TEST_TYPE_SAT_REQUEST_LOCK;
	tChMsg.tSatTuneParam.etAntennaType= FDI_CH_ANT_TYPE_DISEQC;
	FAI_TMD_ArgToValue(&tChMsg.tSatTuneParam.ulFrequency, aucFreqeuncy, ucFreqSize);
	FAI_TMD_ArgToValue(&tChMsg.tSatTuneParam.ulSymbolRate, aucSymbolate, ucSymbolSize);
	tChMsg.tSatTuneParam.etFecCodeRate = ucCodeRate;
	tChMsg.tSatTuneParam.etPolarization = FDI_CH_POLAR_HOR;
	tChMsg.tSatTuneParam.etTransSpec = ucTransSpec;
	tChMsg.tSatTuneParam.etDiseqcInput = ucDiseqc;
	tChMsg.tSatTuneParam.b22kTone = TRUE;
	tChMsg.tSatTuneParam.etModulation = ucModulation;
	tChMsg.tSatTuneParam.etPilot = ucPilot;
	tChMsg.tSatTuneParam.etRollOff = ucRollOff;
	tChMsg.tSatTuneParam.etLnbVoltage = FDI_CH_LNB_VOLT_STD;
	tChMsg.tSatTuneParam.etDiseqcVersion = FDI_CH_DISEQC_VER_1_0;
	tChMsg.tSatTuneParam.ulScdUserBandNum = 0;
	tChMsg.tSatTuneParam.ulScdUserBandFreq = 0;

	FAI_PRINT(FAI_PRT_DBGINFO, ("===============================================\n"));
	FAI_PRINT(FAI_PRT_DBGINFO, ("[%s]\n", __func__));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tucTunerId=%d\n",tChMsg.ulTunerId));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\teChTestType=%d\n",tChMsg.eChTestType));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tetAntennaType=%d\n",tChMsg.tSatTuneParam.etAntennaType));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tfreq=%d\n", tChMsg.tSatTuneParam.ulFrequency));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tsymbol=%d\n", tChMsg.tSatTuneParam.ulSymbolRate));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tetFecCodeRate=%d\n", tChMsg.tSatTuneParam.etFecCodeRate));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tetPolarization=%d\n", tChMsg.tSatTuneParam.etPolarization));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tetTransSpec=%d\n", tChMsg.tSatTuneParam.etTransSpec));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tetDiseqcInput=%d\n", tChMsg.tSatTuneParam.etDiseqcInput));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tb22kTone=%d\n", tChMsg.tSatTuneParam.b22kTone));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tetModulation=%d\n", tChMsg.tSatTuneParam.etModulation));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tetPilot=%d\n", tChMsg.tSatTuneParam.etPilot));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tetRollOff=%d\n", tChMsg.tSatTuneParam.etRollOff));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tetLnbVoltage=%d\n", tChMsg.tSatTuneParam.etLnbVoltage));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tetDiseqcVersion=%d\n", tChMsg.tSatTuneParam.etDiseqcVersion));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tulScdUserBandNum=%d\n", tChMsg.tSatTuneParam.ulScdUserBandNum));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tulScdUserBandFreq=%d\n", tChMsg.tSatTuneParam.ulScdUserBandFreq));
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
	tTMDResult.ucModule = 0x01;
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
#if (CHANNEL_SAT_USED4CHANNEL_ENABLE==YES)
FAI_ERR_CODE FAI_CHANNEL_S_RequestLockFor4Ch(unsigned short usMsgId, unsigned char ucArgc, unsigned char *pucArgList)
{
	unsigned char ucTunerId,usModulatorId ,ucTransSpec, ucModulation, ucCodeRate, aucFreqeuncy[2], aucSymbolate[2], ucDiseqc, ucPilot, ucRollOff;
	FAI_CH_MSG_t tChMsg;
	FVK_ERR_CODE eVkRet;
	FAI_TMD_RESULT_T tTMDResult;
	unsigned char ucFreqSize, ucSymbolSize;
	unsigned char ucTmdErrCode[2];

	FDI_ERR_CODE fdiErr = 0;

	if( ucArgc != 10 )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_CHANNEL_LNBOutput] : Argument Count Error=%d\n", ucArgc));
		return FAI_ERR;
	}

	FAI_TMD_GetParam(&ucTunerId, pucArgList, 0);
	FAI_TMD_GetParam(&usModulatorId, pucArgList, 1);
	FAI_TMD_GetParam(&ucTransSpec, pucArgList, 2);
	FAI_TMD_GetParam(&ucModulation, pucArgList, 3);
	FAI_TMD_GetParam(&ucCodeRate, pucArgList, 4);
	ucFreqSize = FAI_TMD_GetParam(aucFreqeuncy, pucArgList, 5);
	ucSymbolSize = FAI_TMD_GetParam(aucSymbolate, pucArgList, 6);
	FAI_TMD_GetParam(&ucDiseqc, pucArgList, 7);
	FAI_TMD_GetParam(&ucPilot, pucArgList, 8);
	FAI_TMD_GetParam(&ucRollOff, pucArgList, 9);
	

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
	tChMsg.ulTunerId = (unsigned long)ucTunerId;
	tChMsg.eChTestType = FAI_CHANNEL_TEST_TYPE_SAT_REQUEST_LOCK;
	tChMsg.tSatTuneParam.etAntennaType= FDI_CH_ANT_TYPE_DISEQC;
	FAI_TMD_ArgToValue(&tChMsg.tSatTuneParam.ulFrequency, aucFreqeuncy, ucFreqSize);
	FAI_TMD_ArgToValue(&tChMsg.tSatTuneParam.ulSymbolRate, aucSymbolate, ucSymbolSize);
	tChMsg.tSatTuneParam.etFecCodeRate = ucCodeRate;
	tChMsg.tSatTuneParam.etPolarization = FDI_CH_POLAR_HOR;
	tChMsg.tSatTuneParam.etTransSpec = ucTransSpec;
	tChMsg.tSatTuneParam.etDiseqcInput = ucDiseqc;
	tChMsg.tSatTuneParam.b22kTone = TRUE;
	tChMsg.tSatTuneParam.etModulation = ucModulation;
	tChMsg.tSatTuneParam.etPilot = ucPilot;
	tChMsg.tSatTuneParam.etRollOff = ucRollOff;
	tChMsg.tSatTuneParam.etLnbVoltage = FDI_CH_LNB_VOLT_STD;
	tChMsg.tSatTuneParam.etDiseqcVersion = FDI_CH_DISEQC_VER_1_0;
	tChMsg.tSatTuneParam.ulScdUserBandNum = 0;
	tChMsg.tSatTuneParam.ulScdUserBandFreq = 0;

	FAI_PRINT(FAI_PRT_DBGINFO, ("===============================================\n"));
	FAI_PRINT(FAI_PRT_DBGINFO, ("[%s]\n", __func__));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tucTunerId=%d\n",tChMsg.ulTunerId));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tucTunerId=%d\n",usModulatorId));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\teChTestType=%d\n",tChMsg.eChTestType));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tetAntennaType=%d\n",tChMsg.tSatTuneParam.etAntennaType));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tfreq=%d\n", tChMsg.tSatTuneParam.ulFrequency));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tsymbol=%d\n", tChMsg.tSatTuneParam.ulSymbolRate));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tetFecCodeRate=%d\n", tChMsg.tSatTuneParam.etFecCodeRate));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tetPolarization=%d\n", tChMsg.tSatTuneParam.etPolarization));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tetTransSpec=%d\n", tChMsg.tSatTuneParam.etTransSpec));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tetDiseqcInput=%d\n", tChMsg.tSatTuneParam.etDiseqcInput));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tb22kTone=%d\n", tChMsg.tSatTuneParam.b22kTone));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tetModulation=%d\n", tChMsg.tSatTuneParam.etModulation));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tetPilot=%d\n", tChMsg.tSatTuneParam.etPilot));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tetRollOff=%d\n", tChMsg.tSatTuneParam.etRollOff));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tetLnbVoltage=%d\n", tChMsg.tSatTuneParam.etLnbVoltage));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tetDiseqcVersion=%d\n", tChMsg.tSatTuneParam.etDiseqcVersion));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tulScdUserBandNum=%d\n", tChMsg.tSatTuneParam.ulScdUserBandNum));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tulScdUserBandFreq=%d\n", tChMsg.tSatTuneParam.ulScdUserBandFreq));
	FAI_PRINT(FAI_PRT_DBGINFO, ("===============================================\n"));


	fdiErr = FDI_CHANNEL_S_SetInput(ucTunerId, usModulatorId);
	if(fdiErr != FDI_NO_ERR)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[%s] : Error FDI_CHANNEL_S_SetInput() : %d\n", __func__, fdiErr));
		tTMDResult.eResult = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
		ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_MESSAGE_SEND_FAIL;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));
		goto TEST_FAIL;
	}

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
	tTMDResult.ucModule = 0x01;
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
#if (CHANNEL_SAT_LOCK_FOR_PERFORMANCE_ENABLE==YES)
FAI_ERR_CODE FAI_CHANNEL_S_PerformanceCheck(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList)
{
	unsigned char ucTunerId, ucTransSpec, ucModulation, ucCodeRate, aucFreqeuncy[2], aucSymbolate[2], ucDiseqc, ucPilot, ucRollOff;
	FAI_CH_MSG_t tChMsg;
	FVK_ERR_CODE eVkRet;
	FAI_TMD_RESULT_T tTMDResult;
	unsigned char ucFreqSize, ucSymbolSize;
	unsigned char ucTmdErrCode[2];

	FVK_MEM_Set(&tTMDResult, 0x00, sizeof(tTMDResult));

	if( ucArgc != 9 )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[%s] : Argument Count Error=%d\n", __func__, ucArgc));
		tTMDResult.eResult = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
		ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_INVALID_ARGUMENT;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));
		goto TEST_FAIL;
	}

	FAI_TMD_GetParam(&ucTunerId, pucArgList, 0);
	FAI_TMD_GetParam(&ucTransSpec, pucArgList, 1);
	FAI_TMD_GetParam(&ucModulation, pucArgList, 2);
	FAI_TMD_GetParam(&ucCodeRate, pucArgList, 3);
	ucFreqSize = FAI_TMD_GetParam(aucFreqeuncy, pucArgList, 4);
	ucSymbolSize = FAI_TMD_GetParam(aucSymbolate, pucArgList, 5);
	FAI_TMD_GetParam(&ucDiseqc, pucArgList, 6);
	FAI_TMD_GetParam(&ucPilot, pucArgList, 7);
	FAI_TMD_GetParam(&ucRollOff, pucArgList, 8);

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
	tChMsg.ulTunerId = (unsigned long)ucTunerId;
	tChMsg.eChTestType = FAI_CHANNEL_TEST_TYPE_SAT_CHECK_PERFORMANCE;
	tChMsg.tSatTuneParam.etAntennaType= FDI_CH_ANT_TYPE_DISEQC;
	FAI_TMD_ArgToValue(&tChMsg.tSatTuneParam.ulFrequency, aucFreqeuncy, ucFreqSize);
	FAI_TMD_ArgToValue(&tChMsg.tSatTuneParam.ulSymbolRate, aucSymbolate, ucSymbolSize);
	tChMsg.tSatTuneParam.etFecCodeRate = ucCodeRate;
	tChMsg.tSatTuneParam.etPolarization = FDI_CH_POLAR_HOR;
	tChMsg.tSatTuneParam.etTransSpec = FDI_CH_TRANS_DVBS;
	tChMsg.tSatTuneParam.etDiseqcInput = ucDiseqc;
	tChMsg.tSatTuneParam.b22kTone = TRUE;
	tChMsg.tSatTuneParam.etModulation = ucModulation;
	tChMsg.tSatTuneParam.etPilot = ucPilot;
	tChMsg.tSatTuneParam.etRollOff = ucRollOff;
	tChMsg.tSatTuneParam.etLnbVoltage = FDI_CH_LNB_VOLT_STD;
	tChMsg.tSatTuneParam.etDiseqcVersion = FDI_CH_DISEQC_VER_1_0;
	tChMsg.tSatTuneParam.ulScdUserBandNum = 0;
	tChMsg.tSatTuneParam.ulScdUserBandFreq = 0;

	FAI_PRINT(FAI_PRT_DBGINFO, ("===============================================\n"));
	FAI_PRINT(FAI_PRT_DBGINFO, ("[%s]\n", __func__));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tucTunerId=%d\n",tChMsg.ulTunerId));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\teChTestType=%d\n",tChMsg.eChTestType));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tetAntennaType=%d\n",tChMsg.tSatTuneParam.etAntennaType));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tfreq=%d\n", tChMsg.tSatTuneParam.ulFrequency));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tsymbol=%d\n", tChMsg.tSatTuneParam.ulSymbolRate));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tetFecCodeRate=%d\n", tChMsg.tSatTuneParam.etFecCodeRate));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tetPolarization=%d\n", tChMsg.tSatTuneParam.etPolarization));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tetTransSpec=%d\n", tChMsg.tSatTuneParam.etTransSpec));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tetDiseqcInput=%d\n", tChMsg.tSatTuneParam.etDiseqcInput));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tb22kTone=%d\n", tChMsg.tSatTuneParam.b22kTone));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tetModulation=%d\n", tChMsg.tSatTuneParam.etModulation));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tetPilot=%d\n", tChMsg.tSatTuneParam.etPilot));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tetRollOff=%d\n", tChMsg.tSatTuneParam.etRollOff));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tetLnbVoltage=%d\n", tChMsg.tSatTuneParam.etLnbVoltage));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tetDiseqcVersion=%d\n", tChMsg.tSatTuneParam.etDiseqcVersion));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tulScdUserBandNum=%d\n", tChMsg.tSatTuneParam.ulScdUserBandNum));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tulScdUserBandFreq=%d\n", tChMsg.tSatTuneParam.ulScdUserBandFreq));
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
	tTMDResult.ucModule = 0x01;
	tTMDResult.ucAction = 0x03;
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
#if (CHANNEL_SAT_LNB_BYPASS_ENABLE==YES)
FAI_ERR_CODE FAI_CHANNEL_S_LNBBypass(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList)
{
	unsigned char ucOnOff;
	FAI_TMD_RESULT_T tTMDResult;
	unsigned char ucTmdErrCode[2];
	FDI_ERR_CODE eFdiErr = FDI_NO_ERR;

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

	/* Check parameters */
	if( ucOnOff > 1 )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[%s] : Invalid Parameter!! ucOnOff=%d\n", __func__, ucOnOff));
		tTMDResult.eResult = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
		ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_INVALID_PARAMETER;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));
		goto TEST_FAIL;
	}

	if( ucOnOff == 1 )
	{
		eFdiErr = FDI_CHANNEL_S_SetLoopthrouth(TRUE);
		if( eFdiErr != FDI_NO_ERR )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[%s] : SetLoopthrouth Error=%d\n", __func__, eFdiErr));
			tTMDResult.eResult = FAI_TMD_ERR_FAIL;
			ucTmdErrCode[0] = FAI_CATEGORY_CHANNEL;
			ucTmdErrCode[1] = FAI_CHANNEL_TMD_ERR_SET_LNB_LOOP_THROUGH_FAIL;
			FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));
			goto TEST_FAIL;
		}
		FAI_PRINT(FAI_PRT_DBGINFO, ("[%s] : SetLoopthrouth TRUE ok! ~\n", __func__));
	}
	else
	{
		eFdiErr = FDI_CHANNEL_S_SetLoopthrouth(FALSE);
		if( eFdiErr != FDI_NO_ERR )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[%s] : SetLoopthrouth Error=%d\n", __func__, eFdiErr));
			tTMDResult.eResult = FAI_TMD_ERR_FAIL;
			ucTmdErrCode[0] = FAI_CATEGORY_CHANNEL;
			ucTmdErrCode[1] = FAI_CHANNEL_TMD_ERR_SET_LNB_LOOP_THROUGH_FAIL;
			FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));
			goto TEST_FAIL;
		}
		FAI_PRINT(FAI_PRT_DBGINFO, ("[%s] : SetLoopthrouth FALSE ok! ~\n", __func__));
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
#if (LNB_VOLT_UI_ENABLE==YES)
FAI_ERR_CODE FAI_CHANNEL_S_LNBOutput_UI(void)
{
	int i;
	FDI_ERR_CODE eFdiErr;
	FDI_CH_POWER_e eChPower;

	if( GET_STEP() == 0 )
	{
		SET_STEP(1);
		FDI_PANEL_Display(FAI_VOLTAGE_STEP1_STR);
		FAI_TESTMENU_ChangeCurrentRCUColor(RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "STEP1", RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "18V/22K ON", RGB_COLOR_BLACK);

		for( i = 0; i < g_ulNumofCHDevice; i++)
		{
			/* 1. enable LNB ouptut */
			FDI_CHANNEL_S_SetLnbOutput(i, TRUE);
			/* 2. set LNB horizontal */
			FDI_CHANNEL_S_SetLnbPolarisation(i, FDI_CH_POLAR_HOR, FDI_CH_LNB_VOLT_STD);
			/* 3. set LNB tone on */
			FDI_CHANNEL_S_SetLnb22KhzTone(i, 1);
		}
		/* 4. set loop through false(disable bypass) */
		FDI_CHANNEL_S_SetLoopthrouth(FALSE);
	}
	else if( GET_STEP() == 1 )
	{
		SET_STEP(2);
		FDI_PANEL_Display(FAI_VOLTAGE_STEP2_STR);
		FAI_TESTMENU_ChangeCurrentRCUColor(RGB_COLOR_GREEN);
		FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "STEP2", RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "13V/22K OFF", RGB_COLOR_BLACK);
		
		for( i = 0; i < g_ulNumofCHDevice; i++)
		{
			/* 1. set LNB vertical */
			FDI_CHANNEL_S_SetLnbPolarisation(i, FDI_CH_POLAR_VER, FDI_CH_LNB_VOLT_STD);
			/* 3. set LNB tone off */
			FDI_CHANNEL_S_SetLnb22KhzTone(i, 0);
		}
	}
	else if( GET_STEP() == 2 )
	{
		SET_STEP(0);
		FDI_PANEL_Display(FAI_VOLTAGE_STEP3_STR);
		FAI_TESTMENU_ChangeCurrentRCUColor(RGB_COLOR_YELLOW);
		FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "STEP3", RGB_COLOR_BLACK);
#if (LNB_VOLT_SUPPORT==LNB_VOLT_SUPPORT_OFF)
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "LNB OFF", RGB_COLOR_BLACK);
		for( i = 0; i < g_ulNumofCHDevice; i++)
		{
			/* 1-1. disable LNB output */
			FDI_CHANNEL_S_SetLnbOutput(i, FALSE);
		}
#endif
#if (LNB_VOLT_SUPPORT==LNB_VOLT_SUPPORT_BYPASS)
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "LNB BYPASS", RGB_COLOR_BLACK);
		/* 1-2. set loop through false(enable bypass) */
		FDI_CHANNEL_S_SetLoopthrouth(TRUE);
#endif
#if (LNB_VOLT_SUPPORT==LNB_VOLT_SUPPORT_NONE)
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "13V/22K OFF", RGB_COLOR_BLACK);
		/* 1-3. nothing */
#endif
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
#if (SAT_CH_PERFORMANCE_TUNER0_UI_ENABLE==YES)
FAI_ERR_CODE FAI_CHANNEL_S_PerformanceTuner0_UI()
{
	FAI_CH_S_MSG_t tChMsg;
	FVK_ERR_CODE eVkRet;

	tChMsg.ulTunerId = 0;
	tChMsg.eChTestType = FAI_CHANNEL_S_TEST_TYPE_PERFORMANCE_CHECK_UI_T0;
	tChMsg.ulAgcMin = SAT_CH_PERFORMANCE_TUNER0_UI_AGC_MIN;
	tChMsg.ulAgcMax = SAT_CH_PERFORMANCE_TUNER0_UI_AGC_MAX;
	tChMsg.ulBerMin = SAT_CH_PERFORMANCE_TUNER0_UI_BER_MIN;
	tChMsg.ulBerMax = SAT_CH_PERFORMANCE_TUNER0_UI_BER_MAX;
	FAI_TESTMENU_GetCurrentMenu(&tChMsg.hChMenu);

	tChMsg.tSatTuneParam.etAntennaType= FDI_CH_ANT_TYPE_DISEQC;
	tChMsg.tSatTuneParam.ulFrequency = SAT_CH_PERFORMANCE_TUNER0_UI_FREQUENCY;
	tChMsg.tSatTuneParam.ulSymbolRate = SAT_CH_PERFORMANCE_TUNER0_UI_SYMBOL_RATE;
	tChMsg.tSatTuneParam.etFecCodeRate = SAT_CH_PERFORMANCE_TUNER0_UI_CODERATE;
	tChMsg.tSatTuneParam.etPolarization = SAT_CH_PERFORMANCE_TUNER0_UI_POLARIZATION;
	tChMsg.tSatTuneParam.etTransSpec = FDI_CH_TRANS_DVBS;
	tChMsg.tSatTuneParam.etDiseqcInput = SAT_CH_PERFORMANCE_TUNER0_UI_DISEQC_INPUT;
	tChMsg.tSatTuneParam.b22kTone = SAT_CH_PERFORMANCE_TUNER0_UI_LNB22K_TONE;
	tChMsg.tSatTuneParam.etModulation = SAT_CH_PERFORMANCE_TUNER0_UI_MODULATION;
	tChMsg.tSatTuneParam.etPilot = SAT_CH_PERFORMANCE_TUNER0_UI_PILOT;
	tChMsg.tSatTuneParam.etRollOff = SAT_CH_PERFORMANCE_TUNER0_UI_ROLL_OFF;
	tChMsg.tSatTuneParam.etLnbVoltage = SAT_CH_PERFORMANCE_TUNER0_UI_LNB_VOLTAGE;
	tChMsg.tSatTuneParam.etDiseqcVersion = SAT_CH_PERFORMANCE_TUNER0_UI_DISEQC_VER;
	tChMsg.tSatTuneParam.ulScdUserBandNum = 0;
	tChMsg.tSatTuneParam.ulScdUserBandFreq = 0;

	eVkRet = FVK_MSG_Send(s_ulSatChMsgIdForUI, &tChMsg, sizeof(FAI_CH_S_MSG_t));
	if (eVkRet != FVK_NO_ERR)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_CHANNEL_S_PerformanceTuner0_UI] : Error FVK_MSG_Send() : %d\n", eVkRet));
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
#if (SAT_CH_PERFORMANCE_TUNER1_UI_ENABLE==YES)
FAI_ERR_CODE FAI_CHANNEL_S_PerformanceTuner1_UI()
{
	FAI_CH_S_MSG_t tChMsg;
	FVK_ERR_CODE eVkRet;

	tChMsg.ulTunerId = 1;
	tChMsg.eChTestType = FAI_CHANNEL_S_TEST_TYPE_PERFORMANCE_CHECK_UI_T1;
	tChMsg.ulAgcMin = SAT_CH_PERFORMANCE_TUNER1_UI_AGC_MIN;
	tChMsg.ulAgcMax = SAT_CH_PERFORMANCE_TUNER1_UI_AGC_MAX;
	tChMsg.ulBerMin = SAT_CH_PERFORMANCE_TUNER1_UI_BER_MIN;
	tChMsg.ulBerMax = SAT_CH_PERFORMANCE_TUNER1_UI_BER_MAX;
	FAI_TESTMENU_GetCurrentMenu(&tChMsg.hChMenu);

	tChMsg.tSatTuneParam.etAntennaType= FDI_CH_ANT_TYPE_DISEQC;
	tChMsg.tSatTuneParam.ulFrequency = SAT_CH_PERFORMANCE_TUNER1_UI_FREQUENCY;
	tChMsg.tSatTuneParam.ulSymbolRate = SAT_CH_PERFORMANCE_TUNER1_UI_SYMBOL_RATE;
	tChMsg.tSatTuneParam.etFecCodeRate = SAT_CH_PERFORMANCE_TUNER1_UI_CODERATE;
	tChMsg.tSatTuneParam.etPolarization = SAT_CH_PERFORMANCE_TUNER1_UI_POLARIZATION;
	tChMsg.tSatTuneParam.etTransSpec = FDI_CH_TRANS_DVBS;
	tChMsg.tSatTuneParam.etDiseqcInput = SAT_CH_PERFORMANCE_TUNER1_UI_DISEQC_INPUT;
	tChMsg.tSatTuneParam.b22kTone = SAT_CH_PERFORMANCE_TUNER1_UI_LNB22K_TONE;
	tChMsg.tSatTuneParam.etModulation = SAT_CH_PERFORMANCE_TUNER1_UI_MODULATION;
	tChMsg.tSatTuneParam.etPilot = SAT_CH_PERFORMANCE_TUNER1_UI_PILOT;
	tChMsg.tSatTuneParam.etRollOff = SAT_CH_PERFORMANCE_TUNER1_UI_ROLL_OFF;
	tChMsg.tSatTuneParam.etLnbVoltage = SAT_CH_PERFORMANCE_TUNER1_UI_LNB_VOLTAGE;
	tChMsg.tSatTuneParam.etDiseqcVersion = SAT_CH_PERFORMANCE_TUNER1_UI_DISEQC_VER;
	tChMsg.tSatTuneParam.ulScdUserBandNum = 0;
	tChMsg.tSatTuneParam.ulScdUserBandFreq = 0;

	eVkRet = FVK_MSG_Send(s_ulSatChMsgIdForUI, &tChMsg, sizeof(FAI_CH_S_MSG_t));
	if (eVkRet != FVK_NO_ERR)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_CHANNEL_S_PerformanceTuner1_UI] : Error FVK_MSG_Send() : %d\n", eVkRet));
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
static void INTRN_FAI_CHANNEL_S_Task_UI(void *data)
{
	FAI_CH_S_MSG_t tChMsg;
	FVK_ERR_CODE eVkRet;

	while(1)
	{
		eVkRet = FVK_MSG_Receive(s_ulSatChMsgIdForUI, &tChMsg, sizeof(FAI_CH_S_MSG_t));
		if ( eVkRet != FVK_NO_ERR )
		{
			FAI_PRINT(FAI_PRT_WARNING, ("[INTRN_FAI_CHANNEL_S_Task_UI] : Error FVK_MSG_Receive() %d\n", eVkRet));
			continue;
		}

		if( (tChMsg.eChTestType == FAI_CHANNEL_S_TEST_TYPE_PERFORMANCE_CHECK_UI_T0) || (tChMsg.eChTestType == FAI_CHANNEL_S_TEST_TYPE_PERFORMANCE_CHECK_UI_T1) )
		{
			INTRN_FAI_CHANNEL_S_PerformanceCheck_UI(tChMsg.ulTunerId,
   											   tChMsg.tSatTuneParam,
   											   tChMsg.ulAgcMin,
   											   tChMsg.ulAgcMax,
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
static FAI_ERR_CODE INTRN_FAI_CHANNEL_S_PerformanceCheck_UI(unsigned long ulTunerId,
												FDI_CH_SAT_TUNE_PARAM_t tSatTuneParam,
												unsigned long ulAgcRangeMin,
												unsigned long ulAgcRangeMax,
												unsigned long ulBerRangeMin,
												unsigned long ulBerRangeMax,
												FAI_TESTMENU_H hMenu)
{
	FDI_CH_SAT_SIGNAL_QUALITY_t tSatSignalInfo;
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

	FDI_CHANNEL_S_RequestLock(ulTunerId, &tSatTuneParam);

	/* Locking check */
	for( i = 0; i < FAI_CHANNEL_LOCK_CHECK_RETRY_NUM; i++ )
	{
		if( hMenu != NULL )
		{
			sprintf(szCheckStr, "BER Check(%d)", i);
			FAI_TESTMENU_ChangeMenu(hMenu, TESTMENU_LINE_2, szCheckStr, RGB_COLOR_YELLOW);
		}
		FDI_CHANNEL_S_IsLocked(ulTunerId, &bIsLocked);
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
		FDI_CHANNEL_S_GetSignalInfo(ulTunerId, &tSatSignalInfo);
		alAgcValue[i] = (signed long)(tSatSignalInfo.fAgc);
		alBerValue[i] = (signed long)(tSatSignalInfo.nBer);
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
	printf("(lSumOfAgc %d, lSumOfBer %d)\n", lSumOfAgc, lSumOfBer);
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

		FAI_PRINT(FAI_PRT_DBGINFO, ("AGC = %d\n", lAgcAverage));
		FAI_PRINT(FAI_PRT_DBGINFO, ("BER = %d\n", lBerAverage));
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
		FAI_TESTMENU_ChangeMenuColor(hMenu, RGB_COLOR_RED);
		FAI_TESTMENU_ChangeMenu(hMenu, TESTMENU_LINE_2, "NO SIGNAL", RGB_COLOR_RED);
		FAI_TESTMENU_ChangeRCUColor(hMenu, RGB_COLOR_BLACK);
		FDI_PANEL_Display(FAI_CHANNEL_PERFORMANCE_FAIL_STR);
	}
	else
	{
		/* Save failed result parameters */
		aucAgcResult[0] = aucBerResult[0] = 1;
		aucAgcResult[1] = aucBerResult[1] = 0;
		aucAgcResult[2] = aucBerResult[2] = 1;
		aucAgcResult[3] = aucBerResult[3] = 0;
		aucAgcResult[4] = aucBerResult[4] = 0;
		aucAgcResult[5] = aucBerResult[5] = 2;	/* length of param */
		aucAgcResult[6] = aucBerResult[6] = (FAI_TMD_DEFAULT_ERROR_CODE>>8) & 0xFF; /* result - error code high */
		aucAgcResult[7] = aucBerResult[7] = (FAI_TMD_DEFAULT_ERROR_CODE) & 0xFF;		/* result - error code low */
		
		FAI_TMD_SetCurrentResult(0, FAI_TMD_TEST_ERROR, 2+1, aucAgcResult, 6+FAI_TMD_ERROR_PARM_LENGTH);
		FAI_TMD_SetCurrentResult(1, FAI_TMD_TEST_ERROR, 2+1, aucBerResult, 6+FAI_TMD_ERROR_PARM_LENGTH);
	}
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
static FAI_ERR_CODE INTRN_FAI_CHANNEL_S_RequestLock(unsigned short usMsgId, unsigned long ulTunerId, FDI_CH_SAT_TUNE_PARAM_t tSatTuneParam)
{
	BOOL bIsLocked;
	FAI_TMD_RESULT_T tTMDResult;
	unsigned char ucTmdErrCode[2];
	FDI_ERR_CODE eFdiErr = FDI_NO_ERR;
	int i;

	FAI_PRINT(FAI_PRT_DBGINFO, ("===============================================\n"));
	FAI_PRINT(FAI_PRT_DBGINFO, ("[%s]\n", __func__));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tucTunerId=%d\n", ulTunerId));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tetAntennaType=%d\n",tSatTuneParam.etAntennaType));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tfreq=%d\n", tSatTuneParam.ulFrequency));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tsymbol=%d\n", tSatTuneParam.ulSymbolRate));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tetFecCodeRate=%d\n", tSatTuneParam.etFecCodeRate));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tetPolarization=%d\n", tSatTuneParam.etPolarization));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tetTransSpec=%d\n", tSatTuneParam.etTransSpec));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tetDiseqcInput=%d\n", tSatTuneParam.etDiseqcInput));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tb22kTone=%d\n", tSatTuneParam.b22kTone));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tetModulation=%d\n", tSatTuneParam.etModulation));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tetPilot=%d\n", tSatTuneParam.etPilot));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tetRollOff=%d\n", tSatTuneParam.etRollOff));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tetLnbVoltage=%d\n", tSatTuneParam.etLnbVoltage));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tetDiseqcVersion=%d\n", tSatTuneParam.etDiseqcVersion));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tulScdUserBandNum=%d\n", tSatTuneParam.ulScdUserBandNum));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tulScdUserBandFreq=%d\n", tSatTuneParam.ulScdUserBandFreq));
	FAI_PRINT(FAI_PRT_DBGINFO, ("===============================================\n"));

	FVK_MEM_Set(&tTMDResult, 0x00, sizeof(tTMDResult));

	eFdiErr = FDI_CHANNEL_S_RequestLock(ulTunerId, &tSatTuneParam);
	if(eFdiErr != FDI_NO_ERR)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[%s] : Request lock Fail~!\n",__func__));
		ucTmdErrCode[0] = FAI_CATEGORY_CHANNEL;
		ucTmdErrCode[1] = FAI_CHANNEL_TMD_ERR_REQUEST_LOCK_FAIL;
		goto TEST_FAIL;
	}

	/* Locking check */
	for( i = 0; i < FAI_CHANNEL_LOCK_CHECK_RETRY_NUM; i++ )
	{
		FDI_CHANNEL_S_IsLocked(ulTunerId, &bIsLocked);
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
	tTMDResult.ucModule = 0x01;
	tTMDResult.ucAction = 0x02;
	tTMDResult.eResult = FAI_TMD_ERR_OK;
	tTMDResult.usMsgId = usMsgId;
	FAI_TMD_SetResult(&tTMDResult);

	return FAI_NO_ERR;

TEST_FAIL:
	tTMDResult.ucCategory = 0x01;
	tTMDResult.ucModule = 0x01;
	tTMDResult.ucAction = 0x02;
	tTMDResult.eResult = FAI_TMD_ERR_FAIL;
	FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));
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
static FAI_ERR_CODE INTRN_FAI_CHANNEL_S_CheckPerformance(unsigned short usMsgId, unsigned long ulTunerId, FDI_CH_SAT_TUNE_PARAM_t tSatTuneParam) 
{
	FDI_CH_SAT_TUNE_PARAM_t tSatParam;
	FDI_CH_SAT_SIGNAL_QUALITY_t tSignalInfo;
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
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tetAntennaType=%d\n",tSatTuneParam.etAntennaType));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tfreq=%d\n", tSatTuneParam.ulFrequency));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tsymbol=%d\n", tSatTuneParam.ulSymbolRate));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tetFecCodeRate=%d\n", tSatTuneParam.etFecCodeRate));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tetPolarization=%d\n", tSatTuneParam.etPolarization));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tetTransSpec=%d\n", tSatTuneParam.etTransSpec));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tetDiseqcInput=%d\n", tSatTuneParam.etDiseqcInput));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tb22kTone=%d\n", tSatTuneParam.b22kTone));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tetModulation=%d\n", tSatTuneParam.etModulation));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tetPilot=%d\n", tSatTuneParam.etPilot));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tetRollOff=%d\n", tSatTuneParam.etRollOff));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tetLnbVoltage=%d\n", tSatTuneParam.etLnbVoltage));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tetDiseqcVersion=%d\n", tSatTuneParam.etDiseqcVersion));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tulScdUserBandNum=%d\n", tSatTuneParam.ulScdUserBandNum));
	FAI_PRINT(FAI_PRT_DBGINFO, ("\tulScdUserBandFreq=%d\n", tSatTuneParam.ulScdUserBandFreq));
	FAI_PRINT(FAI_PRT_DBGINFO, ("===============================================\n"));

	FVK_MEM_Set(&tTMDResult, 0x00, sizeof(tTMDResult));
	FVK_MEM_Set(aucAgcResult, 0x00, sizeof(aucAgcResult));
	FVK_MEM_Set(aucBerResult, 0x00, sizeof(aucBerResult));
	FVK_MEM_Set(aucSnrResult, 0x00, sizeof(aucSnrResult));
	FVK_MEM_Set(&tSatParam, 0x00, sizeof(tSatParam));
	FVK_MEM_Set(&tSignalInfo, 0x00, sizeof(tSignalInfo));
	
	/* 이미 원하는 주파수로 락이 되어있으면 정보만 업데이트한다(requirement). */
	FDI_CHANNEL_S_IsLocked(ulTunerId, &bIsLocked);
	FDI_CHANNEL_S_GetLockedInfo(ulTunerId, &tSatParam);
	if( (bIsLocked == FALSE) || (tSatTuneParam.ulFrequency != tSatParam.ulFrequency) )
	{
		eFdiErr = FDI_CHANNEL_S_RequestLock(ulTunerId, &tSatTuneParam);
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
			FDI_CHANNEL_S_IsLocked(ulTunerId, &bIsLocked);
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
		eFdiErr = FDI_CHANNEL_S_GetSignalInfo(ulTunerId, &tSignalInfo);
		if(eFdiErr != FDI_NO_ERR)
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[%s] : Get signal info Fail~!\n",__func__));
			ucTmdErrCode[0] = FAI_CATEGORY_CHANNEL;
			ucTmdErrCode[1] = FAI_CHANNEL_TMD_ERR_GET_SIGNAL_INFO_FAIL;
			goto TEST_FAIL;
		}
#if (CHANNEL_SAT_AGC_PERFORMANCE_ENABLE == YES)
		alAgcValue[i] = (signed long)(tSignalInfo.fAgc);
#endif
#if (CHANNEL_SAT_BER_PERFORMANCE_ENABLE == YES)
		alBerValue[i] = (signed long)(tSignalInfo.nBer);
#endif
#if (CHANNEL_SAT_SNR_PERFORMANCE_ENABLE == YES)
		alSnrValue[i] = (signed long)(tSignalInfo.fSnr);
#endif
	}

#if (CHANNEL_SAT_AGC_PERFORMANCE_ENABLE == YES)
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
	aucAgcResult[4] = (CHANNEL_SAT_AGC_POWER_CONSTANT); /* 소수점 지수 */
#endif

#if (CHANNEL_SAT_BER_PERFORMANCE_ENABLE == YES)
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
	aucBerResult[4] = (CHANNEL_SAT_BER_POWER_CONSTANT); /* 소수점 지수 */
#endif

#if (CHANNEL_SAT_SNR_PERFORMANCE_ENABLE == YES)
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
	aucSnrResult[4] = (CHANNEL_SAT_SNR_POWER_CONSTANT); /* 소수점 지수 */
#endif

	FAI_PRINT(FAI_PRT_DBGINFO, ("===============================================\n"));
	FAI_PRINT(FAI_PRT_DBGINFO, ("AGC : 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x\n", aucAgcResult[0], aucAgcResult[1], aucAgcResult[2], aucAgcResult[3], aucAgcResult[4], aucAgcResult[5]));
	FAI_PRINT(FAI_PRT_DBGINFO, ("BER : 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x\n", aucBerResult[0], aucBerResult[1], aucBerResult[2], aucBerResult[3], aucBerResult[4], aucBerResult[5]));
	FAI_PRINT(FAI_PRT_DBGINFO, ("SNR : 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x\n", aucSnrResult[0], aucSnrResult[1], aucSnrResult[2], aucSnrResult[3], aucSnrResult[4], aucSnrResult[5]));
	FAI_PRINT(FAI_PRT_DBGINFO, ("===============================================\n"));
	
	tTMDResult.ucCategory = 0x01;
	tTMDResult.ucModule = 0x01;
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
	tTMDResult.ucModule = 0x01;
	tTMDResult.ucAction = 0x03;
	tTMDResult.eResult = FAI_TMD_ERR_FAIL;
	tTMDResult.usMsgId = usMsgId;
	FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));
	FAI_TMD_SetResult(&tTMDResult);
	return FAI_ERR;
}

