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
#include "fdi_audio.h"
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
FAI_ERR_CODE FAI_AUDIO_Init(void)
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
#if (HW_AUDIO_MUTE_OUTPUT_ENABLE==YES)
FAI_ERR_CODE FAI_AUDIO_HwMute(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList)
{
	FAI_TMD_RESULT_T tTMDResult;
	FAI_TMD_ERR eTmdErr = FAI_TMD_ERR_OK;
	FDI_ERR_CODE eFdiErr;
	unsigned char ucTmdErrCode[2];
	unsigned char ucOnOff;

	FVK_MEM_Set(&tTMDResult, 0x00, sizeof(tTMDResult));
	
	if( ucArgc != 1 )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_AUDIO_HwMute] : Argument Count Error=%d\n", ucArgc));
		eTmdErr = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
		ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_INVALID_ARGUMENT;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));	

		goto END;
	}

	FAI_TMD_GetParam(&ucOnOff, pucArgList, 0);

	eFdiErr = FDI_AUDIO_SetHWtMute((BOOL)ucOnOff);
	if(eFdiErr!= FDI_NO_ERR)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_AUDIO_HwMute] : FDI_AUDIO_SetHWtMute Error\n"));
		eTmdErr = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
		ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_FAIL;
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
#if (HW_AUDIO_MUTE_UI_ENABLE==YES)
FAI_ERR_CODE FAI_AUDIO_HwMute_UI(void)
{
	if( GET_STEP() == 0 )
	{
		FDI_PANEL_Display(FAI_AUDIO_MUTE_ON_STR);
		FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "H/W AUDIO", RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "Mute ON", RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentRCUColor(RGB_COLOR_BLACK);
		if( FDI_AUDIO_SetHWtMute(TRUE) != FDI_NO_ERR )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[FAI_AUDIO_MuteOnOff] : FDI_AUDIO_SetMute Error\n"));
			return FAI_ERR;
		}
		SET_STEP(1);
	}	
	else if( GET_STEP() == 1 )
	{
		FDI_PANEL_Display(FAI_AUDIO_MUTE_OFF_STR);
		FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "H/W AUDIO", RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "Mute OFF", RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentRCUColor(RGB_COLOR_GREEN);
		if( FDI_AUDIO_SetHWtMute(FALSE) != FDI_NO_ERR )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[FAI_AUDIO_MuteOnOff] : FDI_AUDIO_SetMute Error\n"));
			return FAI_ERR;
		}		
		SET_STEP(0);
	}

	return FAI_NO_ERR;
}
#endif

