/******************************************************************************
* $Workfile:$
* $Modtime:$
* Author :
* description :		 Factory System Driver Interface
*
* Copyright (c) 2009 by Humax Co., Ltd.
* All right reserved
******************************************************************************/

/*******************************************************************************
* header and definitions
*******************************************************************************/
/* global header files */

/* chip manufacture's  header files */

/* humax header files */
#include "fdi_common.h"
#include "fdi_panel.h"
#include "fvk.h"

#include "di_err.h"
#include "drv_err.h"
#include "di_front_panel.h"
#include "di_led.h"

#if defined(CONFIG_MICOM)
#include "drv_micom.h"
#endif

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
FDI_FRONT_PANEL_CAP g_tFdiPanelCap;


/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_PANEL_Open(void)
{
	DI_ERR_CODE eDiErr;
	DI_FRONT_PANEL_CAP tFrontCap;

	eDiErr = DI_FRONT_PANEL_GetCapability(&tFrontCap);
	if(eDiErr != DI_ERR_OK)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_SC_Init] DDI_SC_GetCapability Error : %d\n", eDiErr));
		return FDI_ERR;
	}

	g_tFdiPanelCap.max_length = tFrontCap.max_length;
	
	switch(tFrontCap.type)
	{
		case DI_FRONT_PANEL_TYPE_7SEG:
			g_tFdiPanelCap.type = FDI_FRONT_PANEL_TYPE_7SEG;
			break;
		case DI_FRONT_PANEL_TYPE_VFD:
			g_tFdiPanelCap.type = FDI_FRONT_PANEL_TYPE_VFD;
			break;
		case DI_FRONT_PANEL_TYPE_LED:
			g_tFdiPanelCap.type = FDI_FRONT_PANEL_TYPE_LED;
			break;
		case DI_FRONT_PANEL_TYPE_LCD:
			g_tFdiPanelCap.type = FDI_FRONT_PANEL_TYPE_LCD;
			break;
		default:
			g_tFdiPanelCap.type = FDI_FRONT_PANEL_TYPE_7SEG;
			break;
	}
	
	return FDI_NO_ERR;
}




/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_PANEL_GetCapability(FDI_FRONT_PANEL_CAP *ptFrontCap)
{

	ptFrontCap->max_length		= g_tFdiPanelCap.max_length;
	ptFrontCap->type			= g_tFdiPanelCap.type;
	
	return FDI_NO_ERR;
}

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_PANEL_SetPanelPower(BOOL bOnOff)
{
	FDI_ERR_CODE eDiErr;

	eDiErr = DI_FRONT_PANEL_SetPanelPower(bOnOff);
	if(eDiErr != DI_ERR_OK)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_PANEL_SetPanelPower] DI_FRONT_PANEL_SetPanelPower Error !!\n"));
		return FDI_ERR;
	}

	return FDI_NO_ERR;
}

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_PANEL_Display(unsigned char *pucString)
{
	FDI_ERR_CODE eDiErr;

	eDiErr = DI_FRONT_PANEL_Display(pucString);
	if(eDiErr != DI_ERR_OK)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_PANEL_Display] DI_PANNEL_Display Error !!\n"));
		return FDI_ERR;
	}

	return FDI_NO_ERR;
}

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_PANEL_ControlAll(BOOL bOnOff)
{

	if( g_tFdiPanelCap.type == FDI_FRONT_PANEL_TYPE_7SEG )
	{
		int i;
		unsigned char s_pattern[7][4] = 
		{
			{'1' , ' ', ' ', ' '},
			{' ' , '2', ' ', ' '},
			{' ' , '.',' ', '.'},
			{' ' , ' ', '1', ' '},
			{' ' , ' ', ' ', '2'},
			{0x7F , 0x7F, 0x7F, 0x7F},
			{' ' , ' ', ' ', ' '}
		};
		
		if(bOnOff == TRUE)
		{
			for(i=0; i<6; i++)
			{
				DI_FRONT_PANEL_Display(s_pattern[i]);
				FVK_TASK_Sleep(200);
			}
		}
		else
		{
			DI_FRONT_PANEL_Display(s_pattern[6]);
		}

	}
	else
	{
		DRV_Error eDrvErr;
		
		if( bOnOff == TRUE )
		{
#if defined(CONFIG_MICOM)			
			eDrvErr = DRV_MICOM_SetUnitTest(0, TRUE);
			if( eDrvErr != DRV_OK )
			{
				FDI_PRINT(FDI_PRT_ERROR, ("[FDI_PANEL_VFDCtrl] DRV_MICOM_SetUnitTest Error : %d\n", eDrvErr));
				return FDI_ERR;
			}
#endif			
		}
		else
		{
#if defined(CONFIG_MICOM)			
			eDrvErr = DRV_MICOM_SetUnitTest(0, FALSE);
			if( eDrvErr != DRV_OK )
			{
				FDI_PRINT(FDI_PRT_ERROR, ("[FDI_PANEL_VFDCtrl] DRV_MICOM_SetUnitTest Error : %d\n", eDrvErr));
				return FDI_ERR;
			}
#endif			
		}
	}
	return FDI_NO_ERR;
}
/* end of file */

