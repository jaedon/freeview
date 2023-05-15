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
#include "fai_scart.h"
#include "fdi_scart.h"
#include "fdi_panel.h"

/* model specific configuration header */
#include "factorysw_config.h"

/*******************************************************************************
* Debug level
*******************************************************************************/

/*******************************************************************************
* Definitions, typdef & const data
*******************************************************************************/
#define VCR_BYPASS_TMD	1
#define VCR_BYPASS_RCU	2


/*******************************************************************************
* External variables and functions
*******************************************************************************/
/* external variables  and functions */

/*******************************************************************************
* Global variables and structures
*******************************************************************************/
/* global variables and structures */
unsigned char g_bVcrBypassOn, g_bVcrIntOccured;
unsigned long g_ulNumOfScart;
void FAI_SCART_VCRCallback(void *pData);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_SCART_Init(void)
{
	FDI_ERR_CODE eFdiErr;

	FDI_SCART_GetCapability(&g_ulNumOfScart);

	eFdiErr = FDI_SCART_SetHwBypass(FALSE);
	if( eFdiErr != FDI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_SCART_Init] : FDI_SCART_SetHwBypass Error!!\n"));
		return FAI_ERR;
	}
	
	/* RGB mode */
	eFdiErr = FDI_SCART_SetVideoFormat(FDI_SCART_OUTPUT_TV, FDI_SCART_VFORMAT_RGB);
	if( eFdiErr != FDI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_SCART_Init] : FDI_SCART_SetVideoFormat Error!!\n"));
		return FAI_ERR;
	}
	
	/* 4:3 aspect ratio */
	eFdiErr = FDI_SCART_SetAspectRatio(FDI_SCART_OUTPUT_TV, FDI_SCART_AR_4_3);
	if( eFdiErr != FDI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_SCART_Init] : FDI_SCART_SetAspectRatio Error!!\n"));
		return FAI_ERR;
	}

	/* RGB mode */
	eFdiErr = FDI_SCART_SetVideoFormat(FDI_SCART_OUTPUT_VCR, FDI_SCART_VFORMAT_RGB);
	if( eFdiErr != FDI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_SCART_Init] : FDI_SCART_SetVideoFormat Error!!\n"));
		return FAI_ERR;
	}
	
	/* 4:3 aspect ratio */
	eFdiErr = FDI_SCART_SetAspectRatio(FDI_SCART_OUTPUT_VCR, FDI_SCART_AR_4_3);
	if( eFdiErr != FDI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_SCART_Init] : FDI_SCART_SetAspectRatio Error!!\n"));
		return FAI_ERR;
	}

	g_bVcrBypassOn = g_bVcrIntOccured = FALSE;
	eFdiErr = FDI_SCART_RegisterVCRCallback(FAI_SCART_VCRCallback);
	if( eFdiErr != FDI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_SCART_Init] : FDI_SCART_RegisterVCRCallback Error!!\n"));
		return FAI_ERR;
	}
	
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
#if (SCART_OUTPUT_ENABLE==YES)
FAI_ERR_CODE FAI_SCART_Output(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList)
{
	FDI_ERR_CODE eFdiErr;
	FAI_TMD_RESULT_T tTMDResult;
	FAI_TMD_ERR eTmdErr;
	unsigned char ucTmdErrCode[2];
	FDI_SCART_OUTPUT_e eFdiOutput;
	FDI_SCART_VFORMAT_e eFdiVFormat;
	FDI_SCART_AR_e eFdiAr;
	unsigned char ucOutput, ucAspectRatio, ucVideoFormat;

	FVK_MEM_Set(&tTMDResult, 0x00, sizeof(tTMDResult));

	if( ucArgc != 3 )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_SCART_Output] : Argument Count Error=%d\n", ucArgc));
		eTmdErr = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
		ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_INVALID_ARGUMENT;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));	
		goto END;
	}

	FAI_TMD_GetParam(&ucOutput, pucArgList, 0);
	FAI_TMD_GetParam(&ucVideoFormat, pucArgList, 1);
	FAI_TMD_GetParam(&ucAspectRatio, pucArgList, 2);

	switch(ucOutput)
	{
		case 0: /* TV */
			eFdiOutput = FDI_SCART_OUTPUT_TV;
			break;
		case 1: /* VCR */
			eFdiOutput = FDI_SCART_OUTPUT_VCR;
			break;
		case 2: /* TV & VCR */
			eFdiOutput = FDI_SCART_OUTPUT_TV_VCR;
			break;
		default:
			eTmdErr = FAI_TMD_ERR_FAIL;
			ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
			ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_INVALID_PARAMETER;
			FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));	
			goto END;
	}
	
	switch(ucVideoFormat)
	{
		case 0:	/* CVBS */
			eFdiVFormat = FDI_SCART_VFORMAT_CVBS;
			break;
		case 1: /* RGB */
			eFdiVFormat = FDI_SCART_VFORMAT_RGB;
			break;
		case 2: /* SVHS */
			eFdiVFormat = FDI_SCART_VFORMAT_SVHS;
			break;
		default:
			eTmdErr = FAI_TMD_ERR_FAIL;
			ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
			ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_INVALID_PARAMETER;
			FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));	
			goto END;
	}
	switch(ucAspectRatio)
	{
		case 0: /* 4_3 */
			eFdiAr = FDI_SCART_AR_4_3;
			break;
		case 2: /* 16_9 */
			eFdiAr = FDI_SCART_AR_16_9;
			break;
		case 3: /* 0ff */
			eFdiAr = FDI_SCART_AR_NONE;
			break;
		default:
			eTmdErr = FAI_TMD_ERR_FAIL;
			ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
			ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_INVALID_PARAMETER;
			FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));	
			goto END;
	}

	if (eFdiOutput == FDI_SCART_OUTPUT_TV_VCR)
	{
		eFdiErr = FDI_SCART_SetVideoFormat(FDI_SCART_OUTPUT_TV, eFdiVFormat);
		if(eFdiErr != FDI_NO_ERR)
		{
			eTmdErr = FAI_TMD_ERR_FAIL;
			ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_SCART;
			ucTmdErrCode[1] = FAI_SCART_TMD_ERR_VIDEO_FORMAT;
			FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));
			goto END;			
		}
		eFdiErr = FDI_SCART_SetAspectRatio(FDI_SCART_OUTPUT_TV, eFdiAr);
		if(eFdiErr != FDI_NO_ERR)
		{
			eTmdErr = FAI_TMD_ERR_FAIL;
			ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_SCART;
			ucTmdErrCode[1] = FAI_SCART_TMD_ERR_ASPECT_RATIO;
			FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));
			goto END;			
		}

		eFdiErr = FDI_SCART_SetVideoFormat(FDI_SCART_OUTPUT_VCR, eFdiVFormat);
		if(eFdiErr != FDI_NO_ERR)
		{
			eTmdErr = FAI_TMD_ERR_FAIL;
			ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_SCART;
			ucTmdErrCode[1] = FAI_SCART_TMD_ERR_VIDEO_FORMAT;
			FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));
			goto END;			
		}

		eFdiErr = FDI_SCART_SetAspectRatio(FDI_SCART_OUTPUT_VCR, eFdiAr);
		if(eFdiErr != FDI_NO_ERR)
		{
			eTmdErr = FAI_TMD_ERR_FAIL;
			ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_SCART;
			ucTmdErrCode[1] = FAI_SCART_TMD_ERR_ASPECT_RATIO;
			FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));
			goto END;			
		}

	}
	else
	{
		eFdiErr = FDI_SCART_SetVideoFormat(eFdiOutput, eFdiVFormat);
		if(eFdiErr != FDI_NO_ERR)
		{
			eTmdErr = FAI_TMD_ERR_FAIL;
			ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_SCART;
			ucTmdErrCode[1] = FAI_SCART_TMD_ERR_VIDEO_FORMAT;
			FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));
			goto END;			
		}
    
		eFdiErr = FDI_SCART_SetAspectRatio(eFdiOutput, eFdiAr);
		if(eFdiErr != FDI_NO_ERR)
		{
			eTmdErr = FAI_TMD_ERR_FAIL;
			ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_SCART;
			ucTmdErrCode[1] = FAI_SCART_TMD_ERR_ASPECT_RATIO;
			FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));
			goto END;			
		}
	}

	eTmdErr = FAI_TMD_ERR_OK;
	
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
#if (SCART_BYPASS_ENABLE==YES)
FAI_ERR_CODE FAI_SCART_Bypass(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList)
{
	FDI_ERR_CODE eFdiErr;
	FAI_TMD_RESULT_T tTMDResult;
	FAI_TMD_ERR eTmdErr;
	unsigned char ucTmdErrCode[2];
	unsigned char ucType, ucOnOff;

	FVK_MEM_Set(&tTMDResult, 0x00, sizeof(tTMDResult));

	if( ucArgc != 2 )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_SCART_Bypass] : Argument Count Error=%d\n", ucArgc));
		eTmdErr = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
		ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_INVALID_ARGUMENT;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));	
		goto END;
	}

	FAI_TMD_GetParam(&ucType, pucArgList, 0);
	FAI_TMD_GetParam(&ucOnOff, pucArgList, 1);

	if( ucType == 0 )
	{
		/* type(0 : output is TV)	 VCR Bypass : VCR -> TV */
		if( ucOnOff == 0 )
		{
			eFdiErr = FDI_SCART_SetBypass(FALSE);
			if(eFdiErr != FDI_NO_ERR)
			{
				eTmdErr = FAI_TMD_ERR_FAIL;
				ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_SCART;
				ucTmdErrCode[1] = FAI_SCART_TMD_ERR_BYPASS;
				FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));
				goto END;
			}
			g_bVcrBypassOn = 0;
		}
		else
		{
			if( g_bVcrIntOccured == TRUE )
			{
				eFdiErr = FDI_SCART_SetBypass(TRUE);
				if(eFdiErr != FDI_NO_ERR)
				{
					eTmdErr = FAI_TMD_ERR_FAIL;
					ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_SCART;
					ucTmdErrCode[1] = FAI_SCART_TMD_ERR_BYPASS;
					FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));
					goto END;					
				}				
			}
			g_bVcrBypassOn |= VCR_BYPASS_TMD;
		}
	}
	else if( ucType == 1 )
	{
		/* type(1 : output is VCR)	   TV Bypass : TV -> VCR */
		if( ucOnOff == 0 )
		{
			eFdiErr = FDI_SCART_SetBypass(FALSE);
			if(eFdiErr != FDI_NO_ERR)
			{
				eTmdErr = FAI_TMD_ERR_FAIL;
				ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_SCART;
				ucTmdErrCode[1] = FAI_SCART_TMD_ERR_BYPASS;
				FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));
				goto END;				
			}			
			g_bVcrBypassOn = 0;
		}
		else
		{
			eFdiErr = FDI_SCART_SetBypass(TRUE);
			if(eFdiErr != FDI_NO_ERR)
			{
				eTmdErr = FAI_TMD_ERR_FAIL;
				ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_SCART;
				ucTmdErrCode[1] = FAI_SCART_TMD_ERR_BYPASS;
				FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));
				goto END;				
			}			
		}
	}
	else
	{
		eTmdErr = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
		ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_INVALID_ARGUMENT;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));	
	}

	eTmdErr = FAI_TMD_ERR_OK;

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
#if (SCART_OUTPUT_UI_ENABLE==YES)
FAI_ERR_CODE FAI_SCART_Output_UI(void)
{
	int i;
	FDI_ERR_CODE eFdiErr;
	
	if( GET_STEP() == 0 )
	{
		/* 1. Scart output aspect ratio : 16_9 */
		FDI_SCART_SetAspectRatio(FDI_SCART_OUTPUT_TV, FDI_SCART_AR_16_9);
		if( g_ulNumOfScart > 1 )
		{
			FDI_SCART_SetAspectRatio(FDI_SCART_OUTPUT_VCR, FDI_SCART_AR_16_9);
		}

		/* 2. Scart output video format : CVBS */
		FDI_SCART_SetVideoFormat(FDI_SCART_OUTPUT_TV, FDI_SCART_VFORMAT_CVBS);
		if( g_ulNumOfScart > 1 )
		{		
			FDI_SCART_SetVideoFormat(FDI_SCART_OUTPUT_VCR, FDI_SCART_VFORMAT_CVBS);
		}
		FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "STEP1", RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "4.5~7V/0~0.4V", RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentRCUColor(RGB_COLOR_BLACK);
		FDI_PANEL_Display(FAI_VOLTAGE_STEP1_STR);
		SET_STEP(1);
	}
	else if( GET_STEP() == 1 )
	{
		/* 2. Scart output aspect ratio : 4_3 */
		FDI_SCART_SetAspectRatio(FDI_SCART_OUTPUT_TV, FDI_SCART_AR_4_3);
		if( g_ulNumOfScart > 1 )
		{		
			FDI_SCART_SetAspectRatio(FDI_SCART_OUTPUT_VCR, FDI_SCART_AR_4_3);
		}
		/* 3. Scart output video format : RGB */
		FDI_SCART_SetVideoFormat(FDI_SCART_OUTPUT_TV, FDI_SCART_VFORMAT_RGB);
		if( g_ulNumOfScart > 1 )
		{		
			FDI_SCART_SetVideoFormat(FDI_SCART_OUTPUT_VCR, FDI_SCART_VFORMAT_RGB);
		}

		FAI_TESTMENU_ChangeCurrentRCUColor(RGB_COLOR_GREEN);
		FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "STEP2", RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "9.5~12V/1~3V", RGB_COLOR_BLACK);
		FDI_PANEL_Display(FAI_VOLTAGE_STEP2_STR);
#if (SCART_OUTPUT_UI_SUPPORT_STANDBY==YES)		
		SET_STEP(2);
	}
	else if( GET_STEP() == 2 )
	{
		/* 1. Scart output aspect ratio : No Signal */
		FDI_SCART_SetAspectRatio(FDI_SCART_OUTPUT_TV, FDI_SCART_AR_NONE);
		FDI_SCART_SetAspectRatio(FDI_SCART_OUTPUT_VCR, FDI_SCART_AR_NONE);

		/* 2. Scart output video format : CVBS */
		FDI_SCART_SetVideoFormat(FDI_SCART_OUTPUT_TV, FDI_SCART_VFORMAT_CVBS);
		FDI_SCART_SetVideoFormat(FDI_SCART_OUTPUT_VCR, FDI_SCART_VFORMAT_CVBS);

		FAI_TESTMENU_ChangeCurrentRCUColor(RGB_COLOR_YELLOW);
		FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "STEP3", RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "0~2V/0~0.4V", RGB_COLOR_BLACK);
		FDI_PANEL_Display(FAI_VOLTAGE_STEP3_STR);
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
#if (SCART_VCR_BYPASS_UI_ENABLE==YES)
/*    VCR Bypass : VCR -> TV */
FAI_ERR_CODE FAI_SCART_VcrBypass_UI(void)
{
	if( GET_STEP() == 0 )
	{
		FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "VCR SCART", RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "Bypass ON", RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentRCUColor(RGB_COLOR_BLACK);
		FDI_PANEL_Display(FAI_SCART_VCRBYPASS_ON_STR);
		if(	g_bVcrIntOccured == TRUE )
		{
 			if( FDI_SCART_SetHwBypass(TRUE) != FDI_NO_ERR )
 			{
				FAI_PRINT(FAI_PRT_ERROR, ("[FAI_SCART_VcrBypass_UI] : FDI_SCART_SetHwBypass Error!!\n"));
				return FAI_ERR;
			}
		}
		g_bVcrBypassOn |= VCR_BYPASS_RCU;
		SET_STEP(1);
	}
	else if( GET_STEP() == 1 )
	{
		FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "VCR SCART", RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "Bypass OFF", RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentRCUColor(RGB_COLOR_GREEN);
		FDI_PANEL_Display(FAI_SCART_VCRBYPASS_OFF_STR);

 		if( FDI_SCART_SetHwBypass(FALSE) != FDI_NO_ERR )
 		{
			FAI_PRINT(FAI_PRT_ERROR, ("[FAI_SCART_VcrBypass_UI] : FDI_SCART_SetHwBypass Error!!\n"));	
			return FAI_ERR;
		}
		g_bVcrBypassOn = 0;
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
#if (SCART_TV_BYPASS_UI_ENABLE==YES)
/*	  TV Bypass : TV -> VCR */
FAI_ERR_CODE FAI_SCART_TvBypass_UI(void)
{
	if( GET_STEP() == 0 )
	{
		FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "TV SCART", RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "Bypass ON", RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentRCUColor(RGB_COLOR_BLACK);
		FDI_PANEL_Display(FAI_SCART_TVBYPASS_ON_STR);
		if( FDI_SCART_SetBypass(TRUE) != FDI_NO_ERR )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[FAI_SCART_TvBypass_UI] : FDI_SCART_SetBypass Error!!\n"));
			return FAI_ERR;
		}
		SET_STEP(1);
	}
	else if( GET_STEP() == 1 )
	{
		FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "TV SCART", RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "Bypass OFF", RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentRCUColor(RGB_COLOR_GREEN);
		FDI_PANEL_Display(FAI_SCART_TVBYPASS_OFF_STR);
		if( FDI_SCART_SetBypass(FALSE) != FDI_NO_ERR )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[FAI_SCART_TvBypass_UI] : FDI_SCART_SetBypass Error!!\n"));	
			return FAI_ERR;
		}
		g_bVcrBypassOn = 0;
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
void FAI_SCART_VCRCallback(void *pData)
{
	if( *(unsigned long *)pData == 1 )
	{
		g_bVcrIntOccured = TRUE;

		if( (g_bVcrBypassOn & VCR_BYPASS_RCU) == VCR_BYPASS_RCU )
		{
			FDI_SCART_SetHwBypass(TRUE);
		}

		else if( (g_bVcrBypassOn & VCR_BYPASS_TMD) == VCR_BYPASS_TMD )
		{
			FDI_SCART_SetBypass(TRUE);
		}
	}
	else
	{
		g_bVcrIntOccured = FALSE;

/*  Bypass 를 자동으로 끊지는 않는다. */
#if 0
		FDI_SCART_SetBypass(FALSE);
#endif
	}
}

