/******************************************************************************
* $Workfile:$
* $Modtime:$
* Author :
* description :		 LED & 7Seg Test
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
#include "fdi_panel.h"
#include "fdi_led.h"

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
static unsigned long g_ulNumOfLed;



/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_PANEL_Init()
{
	FDI_LED_GetCapability(&g_ulNumOfLed);

#if (PRODUCT_YSR2000==YES)
#if(WIFI_CONDUCT_ENABLE==YES)
#else
	FDI_LED_SetRGBOn(2,1,0,0);
	FDI_LED_SetRGBOn(3,1,0,0);
	FDI_LED_SetLedsDimmingLevel(2,0); /* 100은 off, 50보다 크면 low brightness, 나머지는 max brightness*/
	FDI_LED_SetLedsDimmingLevel(3,0);

	FDI_LED_SetPowerLedState(FDI_LED_WATCHING);
#endif	
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
#if (FRONT_LED_ONOFF_ENABLE==YES) 
FAI_ERR_CODE FAI_PANEL_LEDCtrl(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList)
{
	FDI_ERR_CODE eFdiErr;
	unsigned char ucOnOff, ucLedId, ucColor, ucDim;
	unsigned long ulNumOfLed, i;
	FAI_TMD_RESULT_T tTMDResult;
	unsigned char ucTmdErrCode[2];
	unsigned char aucLedColor[3];
	FVK_MEM_Set(&tTMDResult, 0x00, sizeof(tTMDResult));

#if 1	/* Factory SW 2.0 */	
	if( ucArgc != 4 )
#else
	if( ucArgc != 3 )
#endif
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_PANEL_LEDCtrl] : Argument Count Error=%d\n", ucArgc));
		
		tTMDResult.eResult = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
		ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_INVALID_ARGUMENT;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));	
		FAI_TMD_SetResult(&tTMDResult);
		return FAI_ERR;
	}

#if (FRONT_DISPLAY_UI_SCENARIO==FRONT_3COLOR_LED_SCNARIO)
	FDI_PANEL_Display(" ");
#endif

	FAI_TMD_GetParam(&ucOnOff, pucArgList, 0);
	FAI_TMD_GetParam(&ucLedId, pucArgList, 1);
#if 1 /* Factory SW 2.0 */
	FAI_TMD_GetParam(&ucColor, pucArgList, 2);	
	FAI_TMD_GetParam(&ucDim, pucArgList, 3);
#else
	FAI_TMD_GetParam(&ucDim, pucArgList, 2);
#endif
	switch(ucColor)
	{
		case 0x00: /* OFF */
			aucLedColor[0]=0;
			aucLedColor[1]=0;
			aucLedColor[2]=0;
			FAI_PRINT(FAI_PRT_ERROR, ("[FAI_PANEL_LEDCtrl] OFF (OnOff=%d, Id=%d, R=%d, G=%d, B=%d, DIMM=%d)\n", ucOnOff, ucLedId,aucLedColor[0], aucLedColor[1],aucLedColor[2],ucDim ));
			break;
		case 0x01: /* RED */
			aucLedColor[0]=1;
			aucLedColor[1]=0;
			aucLedColor[2]=0;
			FAI_PRINT(FAI_PRT_ERROR, ("[FAI_PANEL_LEDCtrl] RED (OnOff=%d, Id=%d, R=%d, G=%d, B=%d, DIMM=%d)\n", ucOnOff, ucLedId,aucLedColor[0], aucLedColor[1],aucLedColor[2],ucDim ));			
			break;			
		case 0x02: /* GREEN */
			aucLedColor[0]=0;
			aucLedColor[1]=1;
			aucLedColor[2]=0;
			FAI_PRINT(FAI_PRT_ERROR, ("[FAI_PANEL_LEDCtrl] GREEN (OnOff=%d, Id=%d, R=%d, G=%d, B=%d, DIMM=%d)\n", ucOnOff, ucLedId,aucLedColor[0], aucLedColor[1],aucLedColor[2],ucDim ));			
			break;
		case 0x03: /* BLUE */
			aucLedColor[0]=0;
			aucLedColor[1]=0;
			aucLedColor[2]=1;
			FAI_PRINT(FAI_PRT_ERROR, ("[FAI_PANEL_LEDCtrl] BLUE (OnOff=%d, Id=%d, R=%d, G=%d, B=%d, DIMM=%d)\n", ucOnOff, ucLedId,aucLedColor[0], aucLedColor[1],aucLedColor[2],ucDim ));				
			break;
			
		default:
			aucLedColor[0]=1;
			aucLedColor[1]=1;
			aucLedColor[2]=1;
			FAI_PRINT(FAI_PRT_ERROR, ("[FAI_PANEL_LEDCtrl] ALL ON (OnOff=%d, Id=%d, R=%d, G=%d, B=%d, DIMM=%d)\n", ucOnOff, ucLedId,aucLedColor[0], aucLedColor[1],aucLedColor[2],ucDim ));				
			break;
	}
	FAI_PRINT(FAI_PRT_DBGINFO, ("[FAI_PANEL_LEDCtrl]++ : parm (%d, %d)\n", ucOnOff, ucLedId));

	if( ucOnOff == 1 )
	{
		if (ucLedId == 0xFF)
		{
			/* All LED */
#if (FRONT_DISPLAY_UI_SCENARIO==FRONT_3COLOR2STATUS_LED_SCNARIO)
			if(ucDim == 0) /* Dimm low */
			{
				eFdiErr = FDI_LED_SetRGBOn(2,aucLedColor[0],aucLedColor[1],aucLedColor[2]);
				eFdiErr |= FDI_LED_SetRGBOn(3,aucLedColor[0],aucLedColor[1],aucLedColor[2]);
				eFdiErr |= FDI_LED_SetLedsDimmingLevel(2,60);/* 100은 off, 50보다 크면 low brightness, 나머지는 max brightness*/
				eFdiErr |= FDI_LED_SetLedsDimmingLevel(3,60);
				if( eFdiErr != FDI_NO_ERR )
				{
					FAI_PRINT(FAI_PRT_ERROR, ("[FAI_PANEL_LEDCtrl] FDI_LED_SetRGBOn Full LED ON Error : %d\n", eFdiErr));
					tTMDResult.eResult = FAI_TMD_ERR_FAIL;
					ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
					ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_FAIL;
					FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));	
					FAI_TMD_SetResult(&tTMDResult);
					return FAI_ERR;
				}
			}
			else if(ucDim == 1) /* Dimm high */
			{
				eFdiErr = FDI_LED_SetRGBOn(2,aucLedColor[0],aucLedColor[1],aucLedColor[2]);
				eFdiErr |= FDI_LED_SetRGBOn(3,aucLedColor[0],aucLedColor[1],aucLedColor[2]);
				eFdiErr |= FDI_LED_SetLedsDimmingLevel(2,0); /* 100은 off, 50보다 크면 low brightness, 나머지는 max brightness*/
				eFdiErr |= FDI_LED_SetLedsDimmingLevel(3,0);
				if( eFdiErr != FDI_NO_ERR )
				{
					FAI_PRINT(FAI_PRT_ERROR, ("[FAI_PANEL_LEDCtrl] FDI_LED_SetRGBOn Full LED ON Error : %d\n", eFdiErr));
					tTMDResult.eResult = FAI_TMD_ERR_FAIL;
					ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
					ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_FAIL;
					FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));	
					FAI_TMD_SetResult(&tTMDResult);
					return FAI_ERR;
				}				
			}
			else if(ucDim == 2)
			{

			}

#else
			
			if(ucDim == 0)
			{
				eFdiErr = FDI_LED_DimmingLevel(FDI_LED_DIMMING_HIGH);
			}
			else if(ucDim == 1)
			{
				eFdiErr = FDI_LED_DimmingLevel(FDI_LED_DIMMING_MID);
			}
			else if(ucDim == 2)
			{
				eFdiErr = FDI_LED_DimmingLevel(FDI_LED_DIMMING_LOW);
			}
			eFdiErr = FDI_LED_ControlAll(TRUE);
			if(eFdiErr != FDI_NO_ERR)
			{
				FAI_PRINT(FAI_PRT_ERROR, ("[FAI_PANEL_LEDCtrl] FDI_LED_ControlAll(On) Error : %d\n", eFdiErr));
						tTMDResult.eResult = FAI_TMD_ERR_FAIL;
						ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
						ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_FAIL;
						FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));	
						FAI_TMD_SetResult(&tTMDResult);
						return FAI_ERR;
					}
#endif			
		}
#if (PRODUCT_YSR2000==YES)		
		else if(ucLedId==0xFE)
		{
			eFdiErr |= FDI_LED_SetPowerLedState(FDI_LED_STANDBY_LEDLOW);
			if( eFdiErr != FDI_NO_ERR )
			{
				FAI_PRINT(FAI_PRT_ERROR, ("[FAI_PANEL_LEDCtrl] FDI_LED_SetPowerLedState STANDBY_LEDLOW Error : %d\n", eFdiErr));
				tTMDResult.eResult = FAI_TMD_ERR_FAIL;
				ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
				ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_FAIL;
				FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));	
				FAI_TMD_SetResult(&tTMDResult);
				return FAI_ERR;
			}
		}
		else if(ucLedId==0xFD)
		{
			eFdiErr |= FDI_LED_SetPowerLedState(FDI_LED_WATCHING_LEDLOW);
			if( eFdiErr != FDI_NO_ERR )
			{
				FAI_PRINT(FAI_PRT_ERROR, ("[FAI_PANEL_LEDCtrl] FDI_LED_SetPowerLedState WATCHING_LEDLOW Error : %d\n", eFdiErr));
				tTMDResult.eResult = FAI_TMD_ERR_FAIL;
				ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
				ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_FAIL;
				FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));	
				FAI_TMD_SetResult(&tTMDResult);
				return FAI_ERR;
			}
		}
		else if(ucLedId==0xFC)
		{
			eFdiErr |= FDI_LED_SetPowerLedState(FDI_LED_WATCHING);
			if( eFdiErr != FDI_NO_ERR )
			{
				FAI_PRINT(FAI_PRT_ERROR, ("[FAI_PANEL_LEDCtrl] FDI_LED_SetPowerLedState FDI_LED_WATCHING Error : %d\n", eFdiErr));
				tTMDResult.eResult = FAI_TMD_ERR_FAIL;
				ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
				ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_FAIL;
				FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));	
				FAI_TMD_SetResult(&tTMDResult);
				return FAI_ERR;
			}

		}
		else if(ucLedId==0xFB)
		{
			eFdiErr |= FDI_LED_SetPowerLedState(FDI_LED_STANDBY);
			if( eFdiErr != FDI_NO_ERR )
			{
				FAI_PRINT(FAI_PRT_ERROR, ("[FAI_PANEL_LEDCtrl] FDI_LED_SetPowerLedState FDI_LED_STANDBY Error : %d\n", eFdiErr));
				tTMDResult.eResult = FAI_TMD_ERR_FAIL;
				ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
				ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_FAIL;
				FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));	
				FAI_TMD_SetResult(&tTMDResult);
				return FAI_ERR;
			}

		}		
#endif		
		else
		{
			if(ucDim == 0)
			{
				eFdiErr = FDI_LED_DimmingLevel(FDI_LED_DIMMING_HIGH);
			}
			else if(ucDim == 1)
			{
				eFdiErr = FDI_LED_DimmingLevel(FDI_LED_DIMMING_MID);
			}
			else if(ucDim == 2)
			{
				eFdiErr = FDI_LED_DimmingLevel(FDI_LED_DIMMING_LOW);
			}
			eFdiErr = FDI_LED_Control(ucLedId, TRUE);
			if( eFdiErr != FDI_NO_ERR )
			{
				FAI_PRINT(FAI_PRT_ERROR, ("[FAI_PANEL_LEDCtrl] FDI_LED_Control Error : %d\n", eFdiErr));
				tTMDResult.eResult = FAI_TMD_ERR_FAIL;
				ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
				ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_FAIL;
				FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));	
				FAI_TMD_SetResult(&tTMDResult);
				return FAI_ERR;
			}
#if (FRONT_DISPLAY_UI_SCENARIO==FRONT_3COLOR_LED_SCNARIO)
			if(ucLedId == 0)
			{
				FDI_PANEL_ControlAll(TRUE);
			}
			if(ucLedId == 1)
			{
				FDI_PANEL_ControlAll(TRUE);
			}
			if(ucLedId == 2)	
			{
				FDI_PANEL_ControlAll(TRUE);
			}				
#endif			
		}
	}
	else
	{
		if (ucLedId == 0xFF)
		{
#if (FRONT_DISPLAY_UI_SCENARIO==FRONT_3COLOR2STATUS_LED_SCNARIO)
			eFdiErr = FDI_LED_SetRGBOn(2,aucLedColor[0],aucLedColor[1],aucLedColor[2]);
			eFdiErr |= FDI_LED_SetRGBOn(3,aucLedColor[0],aucLedColor[1],aucLedColor[2]);
			eFdiErr |= FDI_LED_SetLedsDimmingLevel(2,100);/* 100은 off, 50보다 크면 low brightness, 나머지는 max brightness*/
			eFdiErr |= FDI_LED_SetLedsDimmingLevel(3,100);
			eFdiErr |= FDI_LED_SetPowerLedState(FDI_LED_OFF);			
			if( eFdiErr != FDI_NO_ERR )
			{
				FAI_PRINT(FAI_PRT_ERROR, ("[FAI_PANEL_LEDCtrl] FDI_LED_SetRGBOn Full LED ON Error : %d\n", eFdiErr));
				tTMDResult.eResult = FAI_TMD_ERR_FAIL;
				ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
				ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_FAIL;
				FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));	
				FAI_TMD_SetResult(&tTMDResult);
				return FAI_ERR;
			}

#else		
			eFdiErr = FDI_LED_ControlAll(FALSE);
			if(eFdiErr != FDI_NO_ERR)
			{
				FAI_PRINT(FAI_PRT_ERROR, ("[FAI_PANEL_LEDCtrl] FDI_LED_ControlAll(Off) Error : %d\n", eFdiErr));
				tTMDResult.eResult = FAI_TMD_ERR_FAIL;
				ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
				ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_FAIL;
				FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));	
				FAI_TMD_SetResult(&tTMDResult);
				return FAI_ERR;
			}
#endif			
		}
		else if(ucLedId == 0xFE)
		{
			eFdiErr |= FDI_LED_SetPowerLedState(FDI_LED_OFF);
			if( eFdiErr != FDI_NO_ERR )
			{
				FAI_PRINT(FAI_PRT_ERROR, ("[FAI_PANEL_LEDCtrl] FDI_LED_SetPowerLedState FDI_LED_OFF Error : %d\n", eFdiErr));
				tTMDResult.eResult = FAI_TMD_ERR_FAIL;
				ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
				ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_FAIL;
				FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));	
				FAI_TMD_SetResult(&tTMDResult);
				return FAI_ERR;
			}
		}
		else
		{
			eFdiErr = FDI_LED_Control(ucLedId, FALSE);
			if( eFdiErr != FDI_NO_ERR )
			{
				FAI_PRINT(FAI_PRT_ERROR, ("[FAI_PANEL_LEDCtrl] FDI_PANEL_VFDCtrl Error : %d\n", eFdiErr));
				tTMDResult.eResult = FAI_TMD_ERR_FAIL;
				ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
				ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_FAIL;
				FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));	
				FAI_TMD_SetResult(&tTMDResult);
				return FAI_ERR;
			}
		}
	}
	tTMDResult.eResult = FAI_TMD_ERR_OK;
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
FAI_ERR_CODE FAI_PANEL_DisplayCtrl(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList)
{
	unsigned char ucOnOff;
	FDI_ERR_CODE eFdiErr;
	FAI_TMD_RESULT_T tTMDResult;
	unsigned char ucTmdErrCode[2];
	
	FVK_MEM_Set(&tTMDResult, 0x00, sizeof(tTMDResult));

	if( ucArgc != 1 )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_PANEL_DisplayCtrl] : Argument Count Error=%d\n", ucArgc));
		tTMDResult.eResult = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
		ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_INVALID_ARGUMENT;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));	
		FAI_TMD_SetResult(&tTMDResult);
		return FAI_ERR;
	}

	FAI_TMD_GetParam(&ucOnOff, pucArgList, 0);

	FAI_PRINT(FAI_PRT_DBGINFO, ("[FAI_PANEL_DisplayCtrl]++ : parm (%d)\n", ucOnOff));

	if( ucOnOff == 1 )
	{
		eFdiErr = FDI_PANEL_ControlAll(TRUE);
		if( eFdiErr != FDI_NO_ERR )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[FAI_PANEL_DisplayCtrl] FDI_PANEL_ControlAll Error : %d\n", eFdiErr));
			tTMDResult.eResult = FAI_TMD_ERR_FAIL;
			ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
			ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_FAIL;
			FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));	
			FAI_TMD_SetResult(&tTMDResult);
			return FAI_ERR;
		}
	}
	else
	{
		eFdiErr = FDI_PANEL_ControlAll(FALSE);
		if( eFdiErr != FDI_NO_ERR )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[FAI_PANEL_DisplayCtrl] FDI_PANEL_ControlAll Error : %d\n", eFdiErr));
			tTMDResult.eResult = FAI_TMD_ERR_FAIL;
			ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
			ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_FAIL;
			FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));	
			FAI_TMD_SetResult(&tTMDResult);
			return FAI_ERR;
		}
	}
	tTMDResult.eResult = FAI_TMD_ERR_OK;
	FAI_TMD_SetResult(&tTMDResult);
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
#if (FRONT_DISPLAY_UI_SCENARIO==FRONT_DEFAULT_SCNARIO)
static FAI_ERR_CODE INTRN_FAI_PANEL_DefaultTest_UI(unsigned int step)
{
	int i;
	FDI_ERR_CODE eFdiErr;

	if( step == 0 )
	{
		FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "Front Display", RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "ON", RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentRCUColor(RGB_COLOR_BLACK);

		eFdiErr = FDI_LED_ControlAll(TRUE);
		if(eFdiErr != FDI_NO_ERR)
		{
			for(i=0; i<g_ulNumOfLed; i++)
			{
				eFdiErr = FDI_LED_Control(i, TRUE);
				if( eFdiErr != FDI_NO_ERR )
				{
					FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_PANEL_DefaultTest_UI] FDI_LED_Control Error : %d\n", eFdiErr));
					return FAI_ERR;
				}
			}
		}		
		eFdiErr = FDI_PANEL_ControlAll(TRUE);
		if( eFdiErr != FDI_NO_ERR )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_PANEL_DefaultTest_UI] FDI_PANEL_ControlAll Error : %d\n", eFdiErr));
			return FAI_ERR;
		}
		SET_STEP(1);
	}
	else
	{
		FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "Front Display", RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "OFF", RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentRCUColor(RGB_COLOR_GREEN);

		eFdiErr = FDI_LED_ControlAll(FALSE);
		if(eFdiErr != FDI_NO_ERR)
		{
			for(i=0; i<g_ulNumOfLed; i++)
			{
				eFdiErr = FDI_LED_Control(i, FALSE);
				if( eFdiErr != FDI_NO_ERR )
				{
					FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_PANEL_DefaultTest_UI] FDI_PANEL_LedCtrl Error : %d\n", eFdiErr));
					return FAI_ERR;
				}
			}
		}
		eFdiErr = FDI_PANEL_ControlAll(FALSE);
		if( eFdiErr != FDI_NO_ERR )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_PANEL_DefaultTest_UI] FDI_PANEL_VFDCtrl Error : %d\n", eFdiErr));
			return FAI_ERR;
		}

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

#if (FRONT_DISPLAY_UI_SCENARIO==FRONT_3COLOR2STATUS_LED_SCNARIO)
FAI_ERR_CODE INTRN_FAI_PANEL_3Color2StatusLEDTest(unsigned int step)
{
	FAI_ERR_CODE eFdiErr;

	if( step == 0 )
	{
		/* RED LED ON */
		eFdiErr = FDI_LED_SetRGBOn(2,1,0,0);
		eFdiErr |= FDI_LED_SetRGBOn(3,1,0,0);
		eFdiErr |= FDI_LED_SetLedsDimmingLevel(2,50);
		eFdiErr |= FDI_LED_SetLedsDimmingLevel(3,50);
		eFdiErr |= FDI_LED_SetPowerLedState(FDI_LED_STANDBY);
		if( eFdiErr != FDI_NO_ERR )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_PANEL_3Color2StatusLEDTest] FDI_LED_SetRGBOn RED Error : %d\n", eFdiErr));
			return FAI_ERR;
		}

	}
	else if( step == 1 )
	{
		/* GREEN LED ON */
		eFdiErr = FDI_LED_SetRGBOn(2,0,1,0);
		eFdiErr |= FDI_LED_SetRGBOn(3,0,1,0);
		eFdiErr |= FDI_LED_SetLedsDimmingLevel(2,0);
		eFdiErr |= FDI_LED_SetLedsDimmingLevel(3,0);
				eFdiErr |= FDI_LED_SetPowerLedState(FDI_LED_WATCHING);		
		if( eFdiErr != FDI_NO_ERR )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_PANEL_3Color2StatusLEDTest] FDI_LED_SetRGBOn GREEN Error : %d\n", eFdiErr));
			return FAI_ERR;
		}

	}
	else if( step == 2 )
	{
		/* BLUE LED ON */
		eFdiErr = FDI_LED_SetRGBOn(2,0,0,1);
		eFdiErr |= FDI_LED_SetRGBOn(3,0,0,1);
		eFdiErr |= FDI_LED_SetLedsDimmingLevel(2,50);
		eFdiErr |= FDI_LED_SetLedsDimmingLevel(3,50);
		eFdiErr |= FDI_LED_SetPowerLedState(FDI_LED_STANDBY_LEDLOW);		
		if( eFdiErr != FDI_NO_ERR )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_PANEL_3Color2StatusLEDTest] FDI_LED_SetRGBOn BLUE Error : %d\n", eFdiErr));
			return FAI_ERR;
		}

	}
	else if( step == 3 )
	{
		/* LED All on */
		eFdiErr = FDI_LED_SetRGBOn(2,1,1,1);
		eFdiErr |= FDI_LED_SetRGBOn(3,1,1,1);
		eFdiErr |= FDI_LED_SetLedsDimmingLevel(2, 0);
		eFdiErr |= FDI_LED_SetLedsDimmingLevel(3, 0);
		eFdiErr |= FDI_LED_SetPowerLedState(FDI_LED_WATCHING_LEDLOW);
		if( eFdiErr != FDI_NO_ERR )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_PANEL_3Color2StatusLEDTest] FDI_LED_SetRGBOn LED ALL OFF Error : %d\n", eFdiErr));
			return FAI_ERR;
		}
	}
	else
	{
		/* LED All Off */
		eFdiErr = FDI_LED_SetRGBOn(2,0,0,0);
		eFdiErr |= FDI_LED_SetRGBOn(3,0,0,0);
		eFdiErr |= FDI_LED_SetPowerLedState(FDI_LED_OFF);
		if( eFdiErr != FDI_NO_ERR )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_PANEL_3Color2StatusLEDTest] FDI_LED_SetRGBOn LED ALL OFF Error : %d\n", eFdiErr));
			return FAI_ERR;
		}
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
FAI_ERR_CODE INTRN_FAI_PANEL_3Color2StatusLEDTest_UI(unsigned int step)
{
	FAI_ERR_CODE eFaiErr;


	FAI_PRINT(FAI_PRT_DBGINFO, ("[INTRN_FAI_PANEL_3Color2StatusLEDTest_UI] : ++\n"));

	FDI_PANEL_Display(" ");

	if( step == 0 )
	{
		FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "Front Display", RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "Red", RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_4, "LED: RED", RGB_COLOR_BLACK);				
		FAI_TESTMENU_ChangeCurrentRCUColor(RGB_COLOR_BLACK);

		eFaiErr =  INTRN_FAI_PANEL_3Color2StatusLEDTest(step);
		if( eFaiErr != FDI_NO_ERR )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_PANEL_3Color2StatusLEDTest_UI] INTRN_FAI_PANEL_3Color2StatusLEDTest Error : %d\n", eFaiErr));
			return FAI_ERR;
		}

		SET_STEP(1);
	}
	else if( step == 1 )
	{
		FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "Front Display", RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "GREEN", RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_4, "LED: GREEN", RGB_COLOR_BLACK);				
		FAI_TESTMENU_ChangeCurrentRCUColor(RGB_COLOR_GREEN);

		eFaiErr =  INTRN_FAI_PANEL_3Color2StatusLEDTest(step);
		if( eFaiErr != FDI_NO_ERR )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_PANEL_3Color2StatusLEDTest_UI] INTRN_FAI_PANEL_3Color2StatusLEDTest Error : %d\n", eFaiErr));
			return FAI_ERR;
		}

		SET_STEP(2);
	}
	else if( step == 2 )
	{
		FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "Front Display", RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "BLUE", RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_4, "LED: RED(LOW)", RGB_COLOR_BLACK);		
		FAI_TESTMENU_ChangeCurrentRCUColor(RGB_COLOR_BLACK);

		eFaiErr =  INTRN_FAI_PANEL_3Color2StatusLEDTest(step);
		if( eFaiErr != FDI_NO_ERR )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_PANEL_3Color2StatusLEDTest_UI] INTRN_FAI_PANEL_3Color2StatusLEDTest Error : %d\n", eFaiErr));
			return FAI_ERR;
		}

		SET_STEP(3);
	}
	else if( step == 3 )
	{
		FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "Front Display", RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "LED AllOn", RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_4, "LED: GREEN(LOW)", RGB_COLOR_BLACK);				
		FAI_TESTMENU_ChangeCurrentRCUColor(RGB_COLOR_BLACK);

		eFaiErr =  INTRN_FAI_PANEL_3Color2StatusLEDTest(step);
		if( eFaiErr != FDI_NO_ERR )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_PANEL_3Color2StatusLEDTest_UI] INTRN_FAI_PANEL_3Color2StatusLEDTest Error : %d\n", eFaiErr));
			return FAI_ERR;
		}

		SET_STEP(4);
	}
	else if( step == 4 )
	{
		FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "Front Display", RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "LED AllOff", RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_4, "LED: OFF", RGB_COLOR_BLACK);				
		FAI_TESTMENU_ChangeCurrentRCUColor(RGB_COLOR_BLACK);

		eFaiErr =  INTRN_FAI_PANEL_3Color2StatusLEDTest(step);
		if( eFaiErr != FDI_NO_ERR )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_PANEL_3Color2StatusLEDTest_UI] INTRN_FAI_PANEL_3Color2StatusLEDTest Error : %d\n", eFaiErr));
			return FAI_ERR;
		}

		SET_STEP(0);
	}

	FAI_PRINT(FAI_PRT_DBGINFO, ("[INTRN_FAI_PANEL_3Color2StatusLEDTest_UI] : --\n"));

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
#if (FRONT_DISPLAY_UI_SCENARIO==FRONT_3COLOR_LED_SCNARIO)
static FAI_ERR_CODE INTRN_FAI_PANEL_3ColorLEDTest(unsigned int step)
{
	FAI_ERR_CODE eFdiErr;
	
	if( step == 0 )
	{
		/* BLUE LED ON / VFD All On */
		eFdiErr = FDI_LED_DimmingLevel(FDI_LED_DIMMING_HIGH);
		eFdiErr |= FDI_LED_Control(0, TRUE);	/* 21 ID is BLUE */
		eFdiErr |= FDI_PANEL_ControlAll(TRUE);
		if( eFdiErr != FDI_NO_ERR )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[FAI_PANEL_Test_UI] FDI_PANEL_VFDCtrl Error : %d\n", eFdiErr));
			return FAI_ERR;
		}

	}
	else if( step == 1 )
	{
		/* RED LED ON / VFD All Off */
		eFdiErr = FDI_LED_DimmingLevel(FDI_LED_DIMMING_LOW);
		eFdiErr |= FDI_LED_Control(0, TRUE);	/* 20 ID is RED */
		eFdiErr |= FDI_PANEL_ControlAll(TRUE);
		if( eFdiErr != FDI_NO_ERR )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[FAI_PANEL_Test_UI] FDI_PANEL_VFDCtrl Error : %d\n", eFdiErr));
			return FAI_ERR;
		}

	}
	else if( step == 2 )
	{
		/* AMBER LED ON / VFD All On */
		eFdiErr = FDI_LED_Control(2, FALSE);	/* 22 ID is AMBER */
		eFdiErr |= FDI_LED_Control(1, FALSE);	/* 22 ID is AMBER */
		eFdiErr |= FDI_PANEL_ControlAll(TRUE);
		if( eFdiErr != FDI_NO_ERR )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[FAI_PANEL_Test_UI] FDI_PANEL_VFDCtrl Error : %d\n", eFdiErr));
			return FAI_ERR;
		}

	}
	else
	{
		/* AMBER LED Dim Low / VFD All Off */
		eFdiErr = FDI_LED_DimmingLevel(FDI_LED_DIMMING_LOW);
		eFdiErr |= FDI_LED_Control(2, TRUE);	/* 22 ID is AMBER */
		eFdiErr |= FDI_PANEL_ControlAll(FALSE);
		if( eFdiErr != FDI_NO_ERR )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[FAI_PANEL_Test_UI] FDI_PANEL_VFDCtrl Error : %d\n", eFdiErr));
			return FAI_ERR;
		}
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
static FAI_ERR_CODE INTRN_FAI_PANEL_3ColorLEDTest_UI(unsigned int step)
{
	FAI_ERR_CODE eFaiErr;


	FAI_PRINT(FAI_PRT_DBGINFO, ("[INTRN_FAI_PANEL_3ColorLEDTest_UI] : ++\n"));

	FDI_PANEL_Display(" ");

	if( step == 0 )
	{
		FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "Front Display", RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "DIM:HI,SB:ON", RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentRCUColor(RGB_COLOR_BLACK);

		eFaiErr =  INTRN_FAI_PANEL_3ColorLEDTest(step);
		if( eFaiErr != FDI_NO_ERR )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_PANEL_3ColorLEDTest_UI] INTRN_FAI_PANEL_3ColorLEDTest Error : %d\n", eFaiErr));
			return FAI_ERR;
		}

		SET_STEP(1);
	}
	else if( step == 1 )
	{
		FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "Front Display", RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "DIM:LOW,SB:ON", RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentRCUColor(RGB_COLOR_GREEN);

		eFaiErr =  INTRN_FAI_PANEL_3ColorLEDTest(step);
		if( eFaiErr != FDI_NO_ERR )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_PANEL_3ColorLEDTest_UI] INTRN_FAI_PANEL_3ColorLEDTest Error : %d\n", eFaiErr));
			return FAI_ERR;
		}

		SET_STEP(2);
	}
	else if( step == 2 )
	{
		FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "Front Display", RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "ALL:OFF", RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentRCUColor(RGB_COLOR_BLACK);

		eFaiErr =  INTRN_FAI_PANEL_3ColorLEDTest(step);
		if( eFaiErr != FDI_NO_ERR )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_PANEL_3ColorLEDTest_UI] INTRN_FAI_PANEL_3ColorLEDTest Error : %d\n", eFaiErr));
			return FAI_ERR;
		}

		SET_STEP(0);
	}

	FAI_PRINT(FAI_PRT_DBGINFO, ("[INTRN_FAI_PANEL_3ColorLEDTest_UI] : --\n"));

	return FAI_NO_ERR;
}
#endif

#if (FRONT_DISPLAY_UI_SCENARIO==FRONT_2COLOR_LED_SCNARIO)
/*******************************************************************************
* function : 
* description : Front 관련 함수가 config로 동작하지 않는  상태여서 HDR2000T를 위한  함수를 추가함.
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
static FAI_ERR_CODE INTRN_FAI_PANEL_2ColorLEDTest(unsigned int step)
{
	FAI_ERR_CODE eFdiErr;

	if( step == 0 )
	{
		/* BLUE LED ON  */
		/* FDI_LED_Control 함수를 수정 없이 사용하기 위해 여기서 +1 해준다.*/
		eFdiErr = FDI_LED_Control(FRONT_DISPLAY_UI_LED_PORT_BLUE+1, TRUE);	
		if( eFdiErr != FDI_NO_ERR )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[FAI_PANEL_Test_UI] INTRN_FAI_PANEL_2ColorLEDTest Error : %d\n", eFdiErr));
			return FAI_ERR;
		}

	}
	else if( step == 1 )
	{
		/* RED LED ON  */
		eFdiErr = FDI_LED_Control(FRONT_DISPLAY_UI_LED_PORT_RED+1, TRUE);	
		if( eFdiErr != FDI_NO_ERR )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[FAI_PANEL_Test_UI] INTRN_FAI_PANEL_2ColorLEDTest Error : %d\n", eFdiErr));
			return FAI_ERR;
		}

	}
	else
	{
		/* All Off */
		eFdiErr = FDI_LED_Control(FRONT_DISPLAY_UI_LED_PORT_RED+1, FALSE);	/* 22 ID is AMBER */
		if( eFdiErr != FDI_NO_ERR )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[FAI_PANEL_Test_UI] FDI_PANEL_VFDCtrl Error : %d\n", eFdiErr));
			return FAI_ERR;
		}
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
static FAI_ERR_CODE INTRN_FAI_PANEL_2ColorLEDTest_UI(unsigned int step)
{
	FAI_ERR_CODE eFaiErr;


	FAI_PRINT(FAI_PRT_DBGINFO, ("[INTRN_FAI_PANEL_2ColorLEDTest_UI] : ++\n"));


	if( step == 0 )
	{
		FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "Front Display", RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "BLUE", RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentRCUColor(RGB_COLOR_BLACK);

		eFaiErr =  INTRN_FAI_PANEL_2ColorLEDTest(step);
		if( eFaiErr != FDI_NO_ERR )
		{
			FAI_PRINT(FAI_PRT_ERROR, (" INTRN_FAI_PANEL_2ColorLEDTest Error : %d\n", eFaiErr));
			return FAI_ERR;
		}

		SET_STEP(1);
	}
	else if( step == 1 )
	{
		FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "Front Display", RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "RED", RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentRCUColor(RGB_COLOR_GREEN);

		eFaiErr =  INTRN_FAI_PANEL_2ColorLEDTest(step);
		if( eFaiErr != FDI_NO_ERR )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("INTRN_FAI_PANEL_2ColorLEDTest Error : %d\n", eFaiErr));
			return FAI_ERR;
		}

		SET_STEP(2);
	}
	else if( step == 2 )
	{
		FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "Front Display", RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "OFF", RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentRCUColor(RGB_COLOR_BLACK);

		eFaiErr =  INTRN_FAI_PANEL_2ColorLEDTest(step);
		if( eFaiErr != FDI_NO_ERR )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("INTRN_FAI_PANEL_2ColorLEDTest Error : %d\n", eFaiErr));
			return FAI_ERR;
		}

		SET_STEP(0);
	}

	FAI_PRINT(FAI_PRT_DBGINFO, ("[INTRN_FAI_PANEL_3ColorLEDTest_UI] : --\n"));

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
#if (FRONT_DISPLAY_UI_ENABLE==YES)
FAI_ERR_CODE FAI_PANEL_Test_UI()
{
#if (FRONT_DISPLAY_UI_SCENARIO==FRONT_DEFAULT_SCNARIO)	
	return INTRN_FAI_PANEL_DefaultTest_UI(GET_STEP());
#endif
#if (FRONT_DISPLAY_UI_SCENARIO==FRONT_3COLOR2STATUS_LED_SCNARIO)	
	return INTRN_FAI_PANEL_3Color2StatusLEDTest_UI(GET_STEP());
#endif
#if (FRONT_DISPLAY_UI_SCENARIO==FRONT_3COLOR_LED_SCNARIO)	
	return INTRN_FAI_PANEL_3ColorLEDTest_UI(GET_STEP());
#endif
#if (FRONT_DISPLAY_UI_SCENARIO==FRONT_2COLOR_LED_SCNARIO)	
	return INTRN_FAI_PANEL_2ColorLEDTest_UI(GET_STEP());
#endif
}
#endif

