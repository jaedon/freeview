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
#include "fai_common.h"
#include "fai_testmenu.h"
#include "fai_tmd.h"

#include "fdi_rf.h"

/* model specific configuration header */
#include "factorysw_config.h"

/*******************************************************************************
* Debug level
*******************************************************************************/

/*******************************************************************************
* Definitions, typdef & const data
*******************************************************************************/

/*******************************************************************************
* External variables and functions
*******************************************************************************/
/* external variables  and functions *

/*******************************************************************************
* Global variables and structures
*******************************************************************************/
/* global variables and structures */

/*******************************************************************************
* function : 
* description : 
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_RF_Init(void)
{
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
#if defined(RF_OUTPUT_ENABLE) && (RF_OUTPUT_ENABLE==YES)
FAI_ERR_CODE FAI_RF_Output(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList)
{
	FAI_TMD_RESULT_T tTMDResult;
	FDI_ERR_CODE eFdiErr;
	FAI_TMD_ERR eTmdErr = FAI_TMD_ERR_OK;	
 	FDI_RF_TV_STANDARD_e eRFVideoFormat;
	unsigned char ucChNum, ucOffset;
	unsigned char ucTmdErrCode[2];
	
	FVK_MEM_Set(&tTMDResult, 0x00, sizeof(tTMDResult));

	if( ucArgc != 3 )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_RF_Output] : Argument Count Error=%d\n", ucArgc));
		eTmdErr = FAI_TMD_ERR_FAIL;		
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
		ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_INVALID_ARGUMENT;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));	
		goto END;
	}

	FAI_TMD_GetParam(&eRFVideoFormat, pucArgList, 0);
	FAI_TMD_GetParam(&ucChNum, pucArgList, 1);
	FAI_TMD_GetParam(&ucOffset, pucArgList, 2);

	eFdiErr = FDI_RF_SetModulation(eRFVideoFormat, ucChNum, ucOffset);
	if(eFdiErr != FDI_NO_ERR)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_RF_Output] : FDI_RF_SetModulation Error=%d\n", eFdiErr));
		eTmdErr = FAI_TMD_ERR_FAIL;		
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
		ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_FAIL;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));	
		goto END;
	}

END:
	tTMDResult.eResult = eTmdErr;
	FAI_TMD_SetResult(&tTMDResult);
	
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
#if defined(RF_OUTPUT_CHANNEL_CHECK_ENABLE) && (RF_OUTPUT_CHANNEL_CHECK_ENABLE==YES)
FAI_ERR_CODE FAI_RF_CheckOutputChannel(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList)
{
	FAI_TMD_RESULT_T tTMDResult;
	FDI_ERR_CODE eFdiErr;
	FAI_TMD_ERR eTmdErr = FAI_TMD_ERR_OK;	
	unsigned char ucTmdErrCode[2];
	
	FVK_MEM_Set(&tTMDResult, 0x00, sizeof(tTMDResult));

	if( ucArgc != 0 )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_RF_CheckOutputChannel] : Argument Count Error=%d\n", ucArgc));		
		eTmdErr = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
		ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_INVALID_ARGUMENT;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));	
		goto END;
	}

	/* To do */
END:
	tTMDResult.eResult = eTmdErr;
	FAI_TMD_SetResult(&tTMDResult);
	
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
FAI_ERR_CODE FAI_RF_OutputTest_UI(void)
{
	FDI_ERR_CODE eFdiErr = FDI_NO_ERR;
	
	if( GET_STEP() == 0 )
	{
		FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "RF Output", RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentRCUColor(RGB_COLOR_BLACK);
		
#if(TV_STANDARD_OUTPUT_TYPE == TV_STANDARD_OUTPUT_PAL)
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "CH 68", RGB_COLOR_BLACK);
		FDI_RF_SetModulation(FDI_RF_TV_STD_PAL_I, 68, 0);
#elif(TV_STANDARD_OUTPUT_TYPE == TV_STANDARD_OUTPUT_NTSC)
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "CH 3", RGB_COLOR_BLACK);
		FDI_RF_SetModulation(FDI_RF_TV_STD_NTSC, 3, 0);
#endif
		SET_STEP(1);
	}	
	else if( GET_STEP() == 1 )
	{
		FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "RF Output", RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentRCUColor(RGB_COLOR_GREEN);
		
#if(TV_STANDARD_OUTPUT_TYPE == TV_STANDARD_OUTPUT_PAL)
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "CH 38", RGB_COLOR_BLACK);
		FDI_RF_SetModulation(FDI_RF_TV_STD_PAL_I, 38, 0);
#elif(TV_STANDARD_OUTPUT_TYPE == TV_STANDARD_OUTPUT_NTSC)
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "CH 4", RGB_COLOR_BLACK);
		FDI_RF_SetModulation(FDI_RF_TV_STD_NTSC, 4, 0);
#endif
		SET_STEP(0);
	}		
	return FAI_NO_ERR;
}

