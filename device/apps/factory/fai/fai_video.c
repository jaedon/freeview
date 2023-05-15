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
#include "fai_tmd.h"
#include "fai_common.h"
#include "fai_testmenu.h"
#include "fai_video.h"
#include "fdi_video.h"
#include "fdi_panel.h"

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
/* external variables  and functions */

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
FAI_ERR_CODE FAI_VIDEO_Init(void)
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
#if (SVIDEO_OUTPUT_ENABLE==YES)
FAI_ERR_CODE FAI_VIDEO_SVideoOutput(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList)
{
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
#if (COMPOSITE_AUDIO_DECTION_ENABLE==YES)
FAI_ERR_CODE FAI_VIDEO_CompositeAudioDetection(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList)
{
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
#if (DCONNECTOR_OUTPUT_ENABLE==YES)
FAI_ERR_CODE FAI_VIDEO_DConnectorOutput(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList)
{
	FAI_TMD_RESULT_T tTMDResult;
	FAI_TMD_ERR eTmdErr = FAI_TMD_ERR_OK;
	unsigned char ucTmdErrCode[2];
	unsigned long ulDeviceId = 0;
	unsigned char 	ulPortID = 0;
	unsigned char	eHdResolution;
	unsigned char	eAspectRatio;
	FDI_ERR_CODE eFdiErr;

	FVK_MEM_Set(&tTMDResult, 0x00, sizeof(tTMDResult));
	
	if( ucArgc != 3 )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_VIDEO_DConnectorOutput] : Argument Count Error=%d\n", ucArgc));
		eTmdErr = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
		ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_INVALID_ARGUMENT;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));	

		goto END;
	}
	
	FAI_TMD_GetParam((unsigned char*)&ulPortID, pucArgList, 0);
	FAI_TMD_GetParam((unsigned char*)&eHdResolution, pucArgList, 1);
	FAI_TMD_GetParam((unsigned char*)&eAspectRatio, pucArgList, 2);

	eFdiErr = FDI_VIDEO_ControlDPort(ulDeviceId, eHdResolution, eAspectRatio);
	if(eFdiErr != FDI_NO_ERR)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_VIDEO_DConnectorOutput] : FDI_VIDEO_ControlDPort Error\n"));		
		eTmdErr = FAI_TMD_ERR_FAIL;
		if(eFdiErr == FDI_ERR_RESOLUTION)
		{
			ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_DCONNECTOR;
			ucTmdErrCode[1] = FAI_DCONNECTOR_TMD_ERR_RESOLUTION;
		}
		else if(eFdiErr == FDI_ERR_ASPECT)
		{
			ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_DCONNECTOR;
			ucTmdErrCode[1] = FAI_DCONNECTOR_TMD_ERR_ASPECT;
		}
		else
		{
			ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_DCONNECTOR;
			ucTmdErrCode[1] = FAI_DCONNECTOR_TMD_ERR_DPORT_FUNCTION;
		}
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));	
		goto END;
	}
		
END:
	tTMDResult.eResult= eTmdErr;
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
#if (DCONNECTOR_SVIDEO_UI_ENABLE==YES)
FAI_ERR_CODE FAI_VIDEO_DConnectorOutput_UI(void)
{
	FDI_ERR_CODE	eFdiError = FDI_NO_ERR;
	
	if( GET_STEP() == 0 )
	{
		FDI_PANEL_Display(FAI_DPORT_CTRL_STEP1_STR);

		FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_1, "STEP1", RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "0~0.2V", RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "0~0.2V", RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_4, "0~0.2V", RGB_COLOR_BLACK);

		FAI_TESTMENU_ChangeCurrentRCUColor(RGB_COLOR_BLACK);

		eFdiError = FDI_VIDEO_ControlDPort(0, FDI_VIDEO_RESOLUTION_480I, FDI_VIDEO_ASPECTRATIO_4_3);
		if(eFdiError != FDI_NO_ERR)
		{
			FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_RED);
			FDI_PANEL_Display(FAI_DPORT_CTRL_STEP1_FAIL);
		}
		
		SET_STEP(1);
	}
	else if( GET_STEP() == 1 )
	{
		FDI_PANEL_Display(FAI_DPORT_CTRL_STEP2_STR);

		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_1, "STEP2", RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "1.7~2.4V", RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "1.7~2.4V", RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_4, "4~5V", RGB_COLOR_BLACK);

		FAI_TESTMENU_ChangeCurrentRCUColor(RGB_COLOR_GREEN);

		eFdiError = FDI_VIDEO_ControlDPort(0, FDI_VIDEO_RESOLUTION_720P, FDI_VIDEO_ASPECTRATIO_4_3_LETTER);
		if(eFdiError != FDI_NO_ERR)
		{
			FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_RED);
			FDI_PANEL_Display(FAI_DPORT_CTRL_STEP2_FAIL);
		}
		
		SET_STEP(2);
	}
	else if( GET_STEP() == 2 )
	{
		FDI_PANEL_Display(FAI_DPORT_CTRL_STEP3_STR);

		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_1, "STEP3", RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "4~5V", RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "4~5V", RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_4, "0~0.2V", RGB_COLOR_BLACK);

		FAI_TESTMENU_ChangeCurrentRCUColor(RGB_COLOR_YELLOW);

		eFdiError = FDI_VIDEO_ControlDPort(0, FDI_VIDEO_RESOLUTION_1080I, FDI_VIDEO_ASPECTRATIO_16_9);
		if(eFdiError != FDI_NO_ERR)
		{
			FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_RED);
			FDI_PANEL_Display(FAI_DPORT_CTRL_STEP3_FAIL);
		}
		SET_STEP(0);
	}

	return FAI_NO_ERR;
}
#endif

